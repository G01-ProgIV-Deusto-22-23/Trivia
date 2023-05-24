#include "port.h"

#ifdef _WIN32
    #define _SHARED_MEMORY_NAME "Global\\TRIVIA_SHRMEM"
#else
    #define _SHARED_MEMORY_NAME "/TRIVIA_SHRMEM"
#endif

static char SHARED_MEMORY_NAME [sizeof (_SHARED_MEMORY_NAME) + sizeof (stringify (IANA_DYNAMIC_PORT_END)) - 1];
static char START_SERVER_COMMAND [] = "$TRIVIA_INIT_SERVER";

static _Atomic server_status_t *SERVER_STATUS = NULL;

static cmd_t PREALLOC_RECV_BUF [MAX_RECV];

#ifdef _WIN32
static HANDLE TERM_THREAD = NULL;
static HANDLE MAP_FILE    = NULL;
#endif

static
#ifdef _WIN32
    HANDLE
#else
    int
#endif
        SERVER_PROC = 0;

static sqlite3 *SERVER_DATABASE = NULL;

const char *get_start_server_command (void) {
    return START_SERVER_COMMAND;
}

cmd_t *get_default_recv_buf (void) {
    return PREALLOC_RECV_BUF;
}

static void unmap_status (void) {
    if (!SERVER_STATUS)
        return;

#ifdef _WIN32
    if (!UnmapViewOfFile (SERVER_STATUS))
        warning ("could not unmap the view on the shared memory file.");

    if (CloseHandle (MAP_FILE))
        warning ("could not close the shared memory file.");
#else
    if (munmap (SERVER_STATUS, ({
                    size_t sz = (size_t) sysconf (_SC_PAGESIZE);
                    if (sz >= sizeof (_Atomic server_status_t)) {
                        if (sz % sizeof (_Atomic server_status_t))
                            sz += sizeof (_Atomic server_status_t) - sizeof (_Atomic server_status_t) % sz;
                        else
                            sz = sizeof (_Atomic server_status_t);
                    }
                    sz;
                })) == -1)
        warning ("could not unmap the shared memory segment.");

    if (shm_unlink (SHARED_MEMORY_NAME) == -1 && errno != ENOENT)
        warning ("could not unlink the shared memory resource.");
#endif

    SERVER_STATUS = NULL;
}

server_status_t get_server_status (void) {
    if (!SERVER_STATUS) {
        const cmd_t cmd = { .cmd = cmd_packet };
        for (size_t i                   = 0; i < sizeof (PREALLOC_RECV_BUF) / sizeof (*PREALLOC_RECV_BUF);
             *(PREALLOC_RECV_BUF + i++) = cmd)
            ;

#ifdef _WIN32
        if (sprintf (SHARED_MEMORY_NAME, _SHARED_MEMORY_NAME "%d", SERVER_PORT) == -1)
            error ("could not open the shared memory resource.");

        if (!(MAP_FILE = CreateFileMappingA (
                  INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof (_Atomic server_status_t), SHARED_MEMORY_NAME
              )))
            error ("could not create the shared memory file.");

        if (!(SERVER_STATUS = MapViewOfFile (MAP_FILE, FILE_MAP_ALL_ACCESS, 0, 0, sizeof (_Atomic server_status_t)))) {
            CloseHandle (MAP_FILE);

            error ("could not map the shared memory segment.");
        }

        if (atexit (unmap_status))
            warning ("could make the status segment unmap at program exit.");
#else
        if (sprintf (SHARED_MEMORY_NAME, _SHARED_MEMORY_NAME "%d", SERVER_PORT) == -1)
            error ("could not open the shared memory resource.");

        int    fd;
        size_t sz;

        sz = (size_t) sysconf (_SC_PAGESIZE);
        if (sz >= sizeof (_Atomic server_status_t)) {
            if (sz % sizeof (_Atomic server_status_t))
                sz += sizeof (_Atomic server_status_t) - sizeof (_Atomic server_status_t) % sz;
            else
                sz = sizeof (_Atomic server_status_t);
        }

        if ((fd = shm_open (SHARED_MEMORY_NAME, O_RDWR, S_IRUSR | S_IWUSR | S_IXUSR)) == -1) {
            if ((fd = shm_open (SHARED_MEMORY_NAME, O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR | S_IXUSR)) == -1)
                error ("could not create the shared memory resource.");

            if (ftruncate (fd, (off_t) sz) == -1)
                error ("could not truncate the shared memory resource to the next multiple of page size of " stringify (
                    sizeof (_Atomic server_status_t)
                ) " bytes.");
        }

        if ((SERVER_STATUS = mmap (NULL, sz, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED)
            error ("could not map the shared memory segment.");

        if (close (fd) == -1)
            warning ("could not close the file descriptor associated with the shared memory resource.");

        if (atexit (unmap_status))
            warning ("could make the status segment unmap at program exit.");
#endif
    }

#ifdef _WIN32
    SOCKET
#else
    int
#endif
    sock = connect_server (NULL, SERVER_PORT);

#ifndef _WIN32
    msync (SERVER_STATUS, sizeof (_Atomic server_status_t), MS_SYNC);
#endif

    server_status_t s;
    atomic_store (
        SERVER_STATUS, s =
                           (sock ==
#ifdef _WIN32
                                    INVALID_SOCKET
#else
                                    -1
#endif
                                ? get_next_free_port (SERVER_PORT, SERVER_PORT) == -1
                                      ? atomic_load (SERVER_STATUS) == server_starting ||
                                            atomic_load (SERVER_STATUS) == server_restarting
                                      : server_off
                                : server_on)
    );

    disconnect_server (sock, true);

#ifdef _WIN32
    FlushViewOfFile (SERVER_STATUS, sizeof (_Atomic server_status_t));
    FlushFileBuffers (MAP_FILE);
#else
    msync (SERVER_STATUS, sizeof (_Atomic server_status_t), MS_SYNC);
#endif

    return s;
}

#if defined(__cpp_attributes) || __STDC_VERSION__ > 201710L
[[__noreturn__]] static void
    #ifdef _WIN32
    impl_term_handler
    #else
    term_handler
    #endif
    (
    #ifdef _WIN32
        void [[unused]] * unused
    #else
        const int [[unused]] signum
    #endif
    ) {
#else
__attribute__ ((__noreturn__)) static void
    #ifdef _WIN32
    impl_term_handler
    #else
    term_handler
    #endif
    (
    #ifdef _WIN32
        void __attribute__ ((unused)) * unused
    #else
        const int __attribute__ ((unused)) signum
    #endif
    ) {
#endif
    if (!end_games ())
        warning ("could not end all games successfully");

#ifdef _WIN32
    if (atomic_load (SERVER_STATUS) != server_restarting) {
        atomic_store (SERVER_STATUS, server_off);
        FlushViewOfFile (SERVER_STATUS, sizeof (_Atomic server_status_t));
        FlushFileBuffers (MAP_FILE);
    }

    if (!UnmapViewOfFile (SERVER_STATUS))
        warning ("could not unmap the view on the shared memory file.");

    if (CloseHandle (MAP_FILE))
        warning ("could not close the shared memory file.");
#else
    msync (SERVER_STATUS, sizeof (_Atomic server_status_t), MS_SYNC);
    if (atomic_load (SERVER_STATUS) != server_restarting) {
        atomic_store (SERVER_STATUS, server_off);
        msync (SERVER_STATUS, sizeof (_Atomic server_status_t), MS_SYNC);
    }

    if (sqlite3_close (SERVER_DATABASE) == SQLITE_BUSY) {
        warning ("the database is busy, all statements will be finished before attempting to close it again.");

        for (sqlite3_stmt *stmt = sqlite3_next_stmt (SERVER_DATABASE, NULL); stmt;
             stmt               = sqlite3_next_stmt (SERVER_DATABASE, stmt))
            if (sqlite3_finalize (stmt) != SQLITE_OK) {
                warning ("the statement was not finalized properly.");
                print_db_err (SERVER_DATABASE);
            }

        if (sqlite3_close (SERVER_DATABASE) != SQLITE_OK) {
            warning ("it was not possible to close the database despite having finished the statements.");
            print_db_err (SERVER_DATABASE);
        }
    }

    SERVER_DATABASE = NULL;

    if (munmap (SERVER_STATUS, ({
                    size_t sz = (size_t) sysconf (_SC_PAGESIZE);
                    if (sz >= sizeof (_Atomic server_status_t)) {
                        if (sz % sizeof (_Atomic server_status_t))
                            sz += sizeof (_Atomic server_status_t) - sizeof (_Atomic server_status_t) % sz;
                        else
                            sz = sizeof (_Atomic server_status_t);
                    }
                    sz;
                })) == -1)
        warning ("could not unmap the shared memory region");

    if (sprintf (SHARED_MEMORY_NAME, _SHARED_MEMORY_NAME "%d", SERVER_PORT) == -1 ||
        shm_unlink (SHARED_MEMORY_NAME) == -1)
        warning ("could not unlink the shared memory resource.");
#endif

    message ("server terminated.");

    exit (0);
}

#ifdef _WIN32
    #if defined(__cpp_attributes) || __STDC_VERSION__ > 201710L
static void sigsegv_handler (int [[unused]] signum) {
    #else
static void term_handler (const int __attribute__ ((unused)) signum) {
    #endif
    ResumeThread (impl_term_handler);
    Sleep (INFINITE);
}
#endif

__attribute__ ((noreturn)) void impl_start_server (void) {
    static char LOG_FILE
        [sizeof ("server_port_pid.log") + sizeof (stringify (IANA_DYNAMIC_PORT_END)) + sizeof (" ") +
         sizeof (stringify (
#ifdef _WIN32
             DWORD_MAX
#else
             INT_MAX
#endif
         )) -
         3] = { 0 };

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-extra-args"

    set_log_file ((char *) (sprintf (
                                LOG_FILE,
                                "server_port%d_pid%"
#ifdef _WIN32
                                "lu"
#else
                                "d"
#endif
                                ".log",
                                SERVER_PORT,
#ifdef _WIN32
                                GetProcessId (GetCurrentProcess ())
#else
                                getpid ()
#endif
                            ) == -1
                                ? (warning ("could not generate a valid log file name."), NULL)
                                : LOG_FILE));
    open_log_file ();

#pragma GCC diagnostic pop

#ifdef _WIN32
    if (!TERM_THREAD &&
        !(TERM_THREAD = CreateThread (
              &(SECURITY_ATTRIBUTES
              ) { .nLength = sizeof (SECURITY_ATTRIBUTES), .lpSecurityDescriptor = NULL, .bInheritHandle = TRUE },
              0, NULL, term_handler, CREATE_SUSPENDED, NULL
          )))
        error ("could not create the server termination handling thread.", impl_term_handler);

    signal (SIGTERM, term_handler);
    signal (SIGABRT, term_handler);

    if (sprintf (SHARED_MEMORY_NAME, _SHARED_MEMORY_NAME "%d", SERVER_PORT) == -1)
        error ("could not open the shared memory resource.");

    if (!(MAP_FILE = OpenFileMappingA (FILE_MAP_ALL_ACCESS, FALSE, SHARED_MEMORY_NAME)))
        error ("could not create the shared memory file.");

    if (!(SERVER_STATUS = MapViewOfFile (MAP_FILE, FILE_MAP_ALL_ACCESS, 0, 0, sizeof (_Atomic server_status_t)))) {
        CloseHandle (MAP_FILE);

        error ("could not map the shared memory segment.");
    }

    if (atexit (unmap_status))
        warning ("could make the status segment unmap at program exit.");
#else
    sigaction (SIGTERM, &(struct sigaction) { .sa_handler = term_handler }, NULL);
    sigaction (SIGABRT, &(struct sigaction) { .sa_handler = term_handler }, NULL);

    int fd;
    if (sprintf (SHARED_MEMORY_NAME, _SHARED_MEMORY_NAME "%d", SERVER_PORT) == -1)
        error ("could not open the shared memory resource.");

    static size_t sz;
    sz = (size_t) sysconf (_SC_PAGESIZE);
    if (sz >= sizeof (_Atomic server_status_t)) {
        if (sz % sizeof (_Atomic server_status_t))
            sz += sizeof (_Atomic server_status_t) - sizeof (_Atomic server_status_t) % sz;
        else
            sz = sizeof (_Atomic server_status_t);
    }

    if ((fd = shm_open (SHARED_MEMORY_NAME, O_RDWR, S_IRUSR | S_IWUSR | S_IXUSR)) == -1) {
        if ((fd = shm_open (SHARED_MEMORY_NAME, O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR | S_IXUSR)) == -1)
            error ("could not create the shared memory resource.");

        if (ftruncate (fd, (off_t) sz) == -1)
            error ("could not truncate the shared memory resource to the next multiple of page size of " stringify (
                sizeof (_Atomic server_status_t)
            ) " bytes.");
    }

    if ((SERVER_STATUS = mmap (NULL, sz, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED)
        error ("could not map the shared memory segment.");
#endif

    open_db (get_config_database (NULL), &SERVER_DATABASE);

    static map_t   users                               = NULL;
    static Usuario user_arr [ct_next2pow (UINT16_MAX)] = { 0 };
    size_t         nusers                              = (size_t) numeroUsuarios (SERVER_DATABASE);
    {
        if (!(users = create_map (nusers * 2)))
            error ("could not create the user map.");

        Usuario *u = NULL;
        if (!(u = obtenerUsuarios (SERVER_DATABASE)))
            error ("could not retrieve the users from the database.");

        memcpy (user_arr, u, sizeof (Usuario) * nusers);
        free (u);

        for (size_t i = 0; i < nusers; i++)
            put_map (users, (user_arr + i)->username, user_arr + i);
    }

    static linkedlist_t questions = NULL;
    if (!(questions = get_questions ()))
        error ("could not get the questions.");

#ifdef _WIN32
    WSADATA
    wsa;
    if (WSAStartup (MAKEWORD (2, 2), &wsa)) {
        int r = WSAGetLastError ();

        if (r == WSASYSNOTREADY)
            error ("the underlying network subsystem is not ready for network communication.");

        else if (r == WSAVERNOTSUPPORTED)
            error (
                "the version of Windows Sockets support requested is not provided by this particular Windows Sockets implementation."
            );

        else if (r == WSAEINPROGRESS)
            error ("a blocking Windows Sockets 1.1 operation is in progress.");

        else if (r == WSAEPROCLIM)
            error ("a limit on the number of tasks supported by the Windows Sockets implementation has been reached.");

        else if (r == WSAEFAULT)
            error ("The lpWSAData parameter is not a valid pointer.");
    }
#endif

    if (get_next_free_port (SERVER_PORT, SERVER_PORT) == -1) {
#ifndef _WIN32
        msync (SERVER_STATUS, sizeof (_Atomic server_status_t), MS_SYNC);
#endif

        atomic_store (SERVER_STATUS, server_error);

#ifdef _WIN32
        FlushViewOfFile (SERVER_STATUS, sizeof (_Atomic server_status_t));
        FlushFileBuffers (MAP_FILE);
#else
        msync (SERVER_STATUS, sizeof (_Atomic server_status_t), MS_SYNC);
#endif

        error ("the server port is not available.");
    }

    struct sockaddr_in addr;
    addr.sin_family      = AF_INET;
    addr.sin_port        = htons ((uint16_t) SERVER_PORT);
    addr.sin_addr.s_addr = inet_addr ("127.0.0.1");

    const
#ifdef _WIN32
        SOCKET
#else
        int
#endif
            sock = socket (AF_INET, SOCK_STREAM, 0);
    if (sock ==
#ifdef _WIN32
        INVALID_SOCKET
#else
        -1
#endif
    ) {
#ifndef _WIN32
        msync (SERVER_STATUS, sizeof (_Atomic server_status_t), MS_SYNC);
#endif

        atomic_store (SERVER_STATUS, server_error);

#ifdef _WIN32
        FlushViewOfFile (SERVER_STATUS, sizeof (_Atomic server_status_t));
        FlushFileBuffers (MAP_FILE);
#else
        msync (SERVER_STATUS, sizeof (_Atomic server_status_t), MS_SYNC);
#endif

        error ("could not create the server socket.");
    }

    if (setsockopt (sock, SOL_SOCKET, SO_REUSEADDR, (void *) &(int) { 1 }, sizeof (int)) ==
#ifdef _WIN32
        SOCKET_ERROR
#else
        -1
#endif
    )
        warning ("could make the socket reuse local addresses.");

    if (bind (sock, (struct sockaddr *) &addr, sizeof (addr)) ==
#ifdef _WIN32
        SOCKET_ERROR
#else
        -1
#endif
    ) {
        disconnect_server (sock, true);

#ifndef _WIN32
        msync (SERVER_STATUS, sizeof (_Atomic server_status_t), MS_SYNC);
#endif

        atomic_store (SERVER_STATUS, server_error);

#ifdef _WIN32
        FlushViewOfFile (SERVER_STATUS, sizeof (_Atomic server_status_t));
        FlushFileBuffers (MAP_FILE);
#else
        msync (SERVER_STATUS, sizeof (_Atomic server_status_t), MS_SYNC);
#endif

        error ("could not bind.");
    }

    if (listen (sock, 1) == -1) {
        disconnect_server (sock, true);

#ifndef _WIN32
        msync (SERVER_STATUS, sizeof (_Atomic server_status_t), MS_SYNC);
#endif

        atomic_store (SERVER_STATUS, server_error);

#ifdef _WIN32
        FlushViewOfFile (SERVER_STATUS, sizeof (_Atomic server_status_t));
        FlushFileBuffers (MAP_FILE);
#else
        msync (SERVER_STATUS, sizeof (_Atomic server_status_t), MS_SYNC);
#endif

        error ("could not listen.");
    }

    if (!init_games ())
        warning ("could not start all games.");

#ifndef _WIN32
    msync (SERVER_STATUS, sizeof (_Atomic server_status_t), MS_SYNC);
#endif

    atomic_store (SERVER_STATUS, server_on);

#ifdef _WIN32
    FlushViewOfFile (SERVER_STATUS, sizeof (_Atomic server_status_t));
    FlushFileBuffers (MAP_FILE);
#else
    msync (SERVER_STATUS, sizeof (_Atomic server_status_t), MS_SYNC);
#endif

    message ("server started.");

    for (;;) {
        static
#ifdef _WIN32
            int
#else
            socklen_t
#endif
                sl = sizeof (addr);
        static
#ifdef _WIN32
            SOCKET
#else
            int
#endif
                       client_sock;
        static cmd_t   cmd;
        static ssize_t recvlen;

        if ((client_sock = accept (sock, (struct sockaddr *) &addr, &sl)) ==
#ifdef _WIN32
            INVALID_SOCKET
#else
            -1
#endif
        )
            continue;

        message ("client established a connection.");

#ifdef _WIN32
        if (ioctlsocket (client_sock, (long) FIONBIO, &(u_long) { 1 }))
            warning ("could not make the socket non-blocking.");
#else
        {
            const int opts = fcntl (client_sock, F_GETFL, NULL);
            if (opts == -1)
                warning ("could not get the file status flags of the socket.");

            if (fcntl (client_sock, F_SETFL, max (opts, 0) | O_NONBLOCK) == -1)
                warning ("could not make the socket non-blocking.");
        }
#endif

        if ((recvlen = recv (client_sock, (void *) &cmd, sizeof (cmd_t), 0)) ==
#ifdef _WIN32
            SOCKET_ERROR
#else
            -1
#endif
        ) {
            {
#ifdef _WIN32
                WSAPOLLFD pollfd = { .fd = client_sock };
                if (WSAPoll (&pollfd, 1, RECV_TIMEOUT) || (pollfd.revents & (POLLERR | POLLHUP | POLLNVAL)))
#else
                struct pollfd pollfd = { .fd = client_sock, .events = POLLRDHUP };
                if (errno != EINPROGRESS || poll (&pollfd, 1, RECV_TIMEOUT) < 0 ||
                    (pollfd.revents & (POLLERR | POLLHUP | POLLRDHUP | POLLNVAL)))
#endif
                    warning ("could not get any data from the client.");
            }
        }

        disconnect_server (client_sock);

        continue;

        if (!recvlen) {
            message ("the client closed the connection");

            disconnect_server (client_sock);

            continue;
        }

        if (!(cmd.cmd == cmd_kill || cmd.cmd == cmd_game_list || cmd.cmd == cmd_game_connect ||
              cmd.cmd == cmd_game_create || cmd.cmd == cmd_user_fetch || cmd.cmd == cmd_user_creds ||
              cmd.cmd == cmd_user_insert || cmd.cmd == cmd_user_update)) {
            warning (
                "The server only accepts nine commands (killing the server, listing the games, connecting to a game, sending game results, starting a game, comparing/gathering user credentials and updating or inserting those credentials), ignoring the received command."
            );

            disconnect_server (client_sock);

            continue;
        }

        if (cmd.cmd == cmd_game_list) {
            if (send (client_sock, (packet (&cmd, game_list_raw (), 0, false), (void *) &cmd), sizeof (cmd_t), 0) ==
#ifdef _WIN32
                SOCKET_ERROR
#else
                -1
#endif
            ) {
                message ("waiting for packet delivery (" stringify (SEND_TIMEOUT) " milliseconds max).");

                {
#ifdef _WIN32
                    WSAPOLLFD pollfd = { .fd = client_sock };
                    if (WSAPoll (&pollfd, 1, SEND_TIMEOUT) || (pollfd.revents & (POLLERR | POLLHUP | POLLNVAL)))
#else
                    struct pollfd pollfd = { .fd = client_sock, .events = POLLRDHUP };
                    if (errno != EINPROGRESS || poll (&pollfd, 1, SEND_TIMEOUT) < 0 ||
                        (pollfd.revents & (POLLERR | POLLHUP | POLLRDHUP | POLLNVAL)))
#endif
                        warning ("could not send the game IDs in raw form to the client.");
                }
            }

            disconnect_server (client_sock);

            continue;
        }

        if (cmd.cmd == cmd_game_connect) {
            const int port = get_game_port (cmd.info.pack.text);
            if (port == -1) {
                message ("the game ID issued by the client could not be found on the games list.");
                disconnect_server (client_sock);

                continue;
            }

            const
#ifdef _WIN32
                SOCKET
#else
                int
#endif
                    game_sock = connect_server (NULL, port);
#ifdef _WIN32
            if (ioctlsocket (game_sock, (long) FIONBIO, &(u_long) { 1 }))
                warning ("could not make the socket non-blocking.");
#else
            {
                const int opts = fcntl (game_sock, F_GETFL, NULL);
                if (opts == -1)
                    warning ("could not get the file status flags of the socket.");

                if (fcntl (game_sock, F_SETFL, max (opts, 0) | O_NONBLOCK) == -1)
                    warning ("could not make the socket non-blocking.");
            }
#endif

            static uint8_t qs [sizeof (game_attr_t) + MAX_ROUNDS];
            ct_error (
                MAX_ROUNDS > MAX_RECV,
                "the maximum number of rounds (" stringify (MAX_ROUNDS
                ) ") cannot exceed the maximum number of receivable packets (" stringify (MAX_RECV) ")."
            );
            if (recv (client_sock, (void *) qs, sizeof (qs), 0) ==
#ifdef _WIN32
                SOCKET_ERROR
#else
                -1
#endif
            ) {
#ifdef _WIN32
                WSAPOLLFD pollfd = { .fd = game_sock };
                if (WSAPoll (&pollfd, 1, RECV_TIMEOUT) || (pollfd.revents & (POLLERR | POLLHUP | POLLNVAL)))
#else
                struct pollfd pollfd = { .fd = game_sock, .events = POLLRDHUP };
                if (errno != EINPROGRESS || poll (&pollfd, 1, RECV_TIMEOUT) < 0 ||
                    (pollfd.revents & (POLLERR | POLLHUP | POLLRDHUP | POLLNVAL)))
#endif
                {
                    warning ("could not get any data fromt the game.");

                    memset (
                        mempcpy (
                            qs,
                            &(game_attr_t
                            ) { .players = MAX_PLAYERS, .round_time = DEFAULT_ROUND_TIME, .rounds = MAX_ROUNDS },
                            1
                        ),
                        0, MAX_ROUNDS
                    );
                }
            }

            disconnect_server (game_sock);

            static question_t *q_ptrs [MAX_QUESTIONS] = { 0 };

            for (uint8_t i = 0, j = ((game_attr_t *) qs)->rounds; i < j; i++) {
                if (!length_linkedlist (questions)) {
                    warning ("no questions remaining.");

                    break;
                }

                if (send (
                        client_sock,
                        (packet (
                             &cmd,
                             (char
                                  *) (*(q_ptrs + i) = pop_linkedlist (questions, *((uint8_t *) ((char *) qs + sizeof (game_attr_t)) + i) % length_linkedlist (questions))),
                             sizeof (question_t), i < j - 1
                         ),
                         (void *) &cmd),
                        sizeof (cmd_t), 0
                    ) ==
#ifdef _WIN32
                    SOCKET_ERROR
#else
                    -1
#endif
                ) {
                    message ("waiting for packet delivery (" stringify (SEND_TIMEOUT) " milliseconds max).");

                    {
#ifdef _WIN32
                        WSAPOLLFD pollfd = { .fd = client_sock };
                        if (WSAPoll (&pollfd, 1, SEND_TIMEOUT) || (pollfd.revents & (POLLERR | POLLHUP | POLLNVAL)))
#else
                        struct pollfd pollfd = { .fd = client_sock, .events = POLLRDHUP };
                        if (errno != EINPROGRESS || poll (&pollfd, 1, SEND_TIMEOUT) < 0 ||
                            (pollfd.revents & (POLLERR | POLLHUP | POLLRDHUP | POLLNVAL)))
#endif
                            warning ("could not send the question to the client.");

                        break;
                    }
                }
            }

            disconnect_server (client_sock);

            for (size_t i = 0; i < MAX_QUESTIONS && *(q_ptrs + i); insert_linkedlist (questions, *(q_ptrs + i++)))
                ;
            memset (q_ptrs, 0, sizeof (q_ptrs));

            continue;
        }

        if (cmd.cmd == cmd_game_create) {
            if (send (
                    client_sock, (packet (&cmd, init_game (cmd.info.game, 0, false), 0, false), (void *) &cmd),
                    sizeof (cmd_t), 0
                ) ==
#ifdef _WIN32
                SOCKET_ERROR
#else
                -1
#endif
            ) {
                message ("waiting for packet delivery (" stringify (SEND_TIMEOUT) " milliseconds max).");

                {
#ifdef _WIN32
                    WSAPOLLFD pollfd = { .fd = client_sock };
                    if (WSAPoll (&pollfd, 1, SEND_TIMEOUT) || (pollfd.revents & (POLLERR | POLLHUP | POLLNVAL)))
#else
                    struct pollfd pollfd = { .fd = client_sock, .events = POLLRDHUP };
                    if (errno != EINPROGRESS || poll (&pollfd, 1, SEND_TIMEOUT) < 0 ||
                        (pollfd.revents & (POLLERR | POLLHUP | POLLRDHUP | POLLNVAL)))
#endif
                        warning ("could not send the game details to the client.");
                }
            }

            disconnect_server (client_sock);

            continue;
        }

        if (cmd.cmd == cmd_user_fetch || cmd.cmd == cmd_user_creds) {
            Usuario *const u = get_map (users, cmd.info.user.username);
            if (send (
                    client_sock,
                    (cmd = u && (cmd.cmd == cmd_user_fetch || !strcmp (u->contrasena, cmd.info.user.contrasena))
                               ? user_creds_command (*u)
                               : error_command (CMD_ERROR_NO_USER),
                     (void *) &cmd),
                    sizeof (cmd_t), 0
                ) ==
#ifdef _WIN32
                SOCKET_ERROR
#else
                -1
#endif
            ) {
                message ("waiting for packet delivery (" stringify (SEND_TIMEOUT) " milliseconds max).");

                {
#ifdef _WIN32
                    WSAPOLLFD pollfd = { .fd = client_sock };
                    if (WSAPoll (&pollfd, 1, SEND_TIMEOUT) || (pollfd.revents & (POLLERR | POLLHUP | POLLNVAL)))
#else
                    struct pollfd pollfd = { .fd = client_sock, .events = POLLRDHUP };
                    if (errno != EINPROGRESS || poll (&pollfd, 1, SEND_TIMEOUT) < 0 ||
                        (pollfd.revents & (POLLERR | POLLHUP | POLLRDHUP | POLLNVAL)))
#endif
                        warning ("could not send the results of the operation to the client.");
                }
            }

            disconnect_server (client_sock);

            continue;
        }

        if (cmd.cmd == cmd_user_update) {
            Usuario *const u = get_map (users, cmd.info.user.username);
            if (!u) {
                if (send (client_sock, (cmd = error_command (CMD_ERROR_NO_USER), (void *) &cmd), sizeof (cmd_t), 0) ==
#ifdef _WIN32
                    SOCKET_ERROR
#else
                    -1
#endif
                ) {
                    message ("waiting for packet delivery (" stringify (SEND_TIMEOUT) " milliseconds max).");

                    {
#ifdef _WIN32
                        WSAPOLLFD pollfd = { .fd = client_sock };
                        if (WSAPoll (&pollfd, 1, SEND_TIMEOUT) || (pollfd.revents & (POLLERR | POLLHUP | POLLNVAL)))
#else
                        struct pollfd pollfd = { .fd = client_sock, .events = POLLRDHUP };
                        if (errno != EINPROGRESS || poll (&pollfd, 1, SEND_TIMEOUT) < 0 ||
                            (pollfd.revents & (POLLERR | POLLHUP | POLLRDHUP | POLLNVAL)))
#endif
                            warning ("could not send the results of the operation to the client.");
                    }
                }

                continue;
            }

            Usuario prev = *u;
            *u           = cmd.info.user;

            if (strcmp (u->nombreVisible, prev.nombreVisible))
                modifyNombreVisible (SERVER_DATABASE, u->nombreVisible, u->ID_Usuario);

            if (strcmp (u->contrasena, prev.contrasena))
                modifyContrasena (SERVER_DATABASE, u->nombreVisible, u->ID_Usuario);

            if (u->aciertosTotales != prev.aciertosTotales)
                modifyAciertosTotales (SERVER_DATABASE, u->aciertosTotales, u->ID_Usuario);

            if (u->fallosTotales != prev.fallosTotales)
                modifyFallosTotales (SERVER_DATABASE, u->fallosTotales, u->ID_Usuario);

            if (u->ID_Presets != prev.ID_Presets)
                modifyIDPresets (SERVER_DATABASE, u->ID_Presets, u->ID_Usuario);

            if (send (client_sock, (cmd = success_command (), (void *) &cmd), sizeof (cmd_t), 0) ==
#ifdef _WIN32
                SOCKET_ERROR
#else
                -1
#endif
            ) {
                message ("waiting for packet delivery (" stringify (SEND_TIMEOUT) " milliseconds max).");

                {
#ifdef _WIN32
                    WSAPOLLFD pollfd = { .fd = client_sock };
                    if (WSAPoll (&pollfd, 1, SEND_TIMEOUT) || (pollfd.revents & (POLLERR | POLLHUP | POLLNVAL)))
#else
                    struct pollfd pollfd = { .fd = client_sock, .events = POLLRDHUP };
                    if (errno != EINPROGRESS || poll (&pollfd, 1, SEND_TIMEOUT) < 0 ||
                        (pollfd.revents & (POLLERR | POLLHUP | POLLRDHUP | POLLNVAL)))
#endif
                        warning ("could not send the results of the operation to the client.");
                }
            }

            disconnect_server (client_sock);

            continue;
        }

        if (cmd.cmd == cmd_user_insert) {
            if (send (
                    client_sock,
                    (cmd = get_map (users, cmd.info.user.username)
                               ? error_command (CMD_ERROR_INSERT_USER)
                               : (insertarUsuario (SERVER_DATABASE, *(user_arr + ++nusers) = cmd.info.user),
                                  user_creds_command (cmd.info.user)),
                     (void *) &cmd),
                    sizeof (cmd_t), 0
                ) ==
#ifdef _WIN32
                SOCKET_ERROR
#else
                -1
#endif
            ) {
                message ("waiting for packet delivery (" stringify (SEND_TIMEOUT) " milliseconds max).");

                {
#ifdef _WIN32
                    WSAPOLLFD pollfd = { .fd = client_sock };
                    if (WSAPoll (&pollfd, 1, SEND_TIMEOUT) || (pollfd.revents & (POLLERR | POLLHUP | POLLNVAL)))
#else
                    struct pollfd pollfd = { .fd = client_sock, .events = POLLRDHUP };
                    if (errno != EINPROGRESS || poll (&pollfd, 1, SEND_TIMEOUT) < 0 ||
                        (pollfd.revents & (POLLERR | POLLHUP | POLLRDHUP | POLLNVAL)))
#endif
                        warning ("could not send the results of the operation to the client.");
                }
            }

            disconnect_server (client_sock);

            if (cmd.cmd == cmd_error)
                put_map (users, cmd.info.user.username, user_arr + nusers);

            continue;
        }

        message ("kill command received.");
        if (send (client_sock, (cmd = success_command (), (void *) &cmd), sizeof (cmd_t), 0) ==
#ifdef _WIN32
            SOCKET_ERROR
#else
            -1
#endif
        ) {
            message ("waiting for packet delivery (" stringify (SEND_TIMEOUT) " milliseconds max).");

            {
#ifdef _WIN32
                WSAPOLLFD pollfd = { .fd = client_sock };
                if (WSAPoll (&pollfd, 1, SEND_TIMEOUT) || (pollfd.revents & (POLLERR | POLLHUP | POLLNVAL)))
#else
                struct pollfd pollfd = { .fd = client_sock, .events = POLLRDHUP };
                if (errno != EINPROGRESS || poll (&pollfd, 1, SEND_TIMEOUT) < 0 ||
                    (pollfd.revents & (POLLERR | POLLHUP | POLLRDHUP | POLLNVAL)))
#endif
                    warning ("could not send the command feedback to the client.");
            }
        }

        disconnect_server (client_sock);
        disconnect_server (sock, true);

        destroy_map (users);

        raise (SIGTERM);
        exit (0);
    }

    raise (SIGTERM);
    exit (0);
}

void start_server (void) {
#ifdef _WIN32
    static STARTUPINFOA        SI;
    static PROCESS_INFORMATION PI;
    static char                PROG_PATH [MAX_PATH + 1]                                                  = { 0 };
    static char COMMAND [sizeof (START_SERVER_COMMAND) + sizeof (stringify (IANA_DYNAMIC_PORT_END)) - 1] = { 0 };

    if (!GetModuleFileNameA (NULL, PROG_PATH, MAX_PATH + 1))
        error ("could not get the executable filename.");

    GetStartupInfoA (&SI);

    if (SERVER_STATUS &&
        (atomic_load (SERVER_STATUS) == server_starting || atomic_load (SERVER_STATUS) == server_restarting))
        return;

    if (!SERVER_STATUS) {
        if (sprintf (SHARED_MEMORY_NAME, _SHARED_MEMORY_NAME "%d", SERVER_PORT) == -1)
            error ("could not open the shared memory resource.");

        if (!(MAP_FILE = CreateFileMappingA (
                  INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof (_Atomic server_status_t), SHARED_MEMORY_NAME
              )))
            error ("could not create the shared memory file.");

        if (!(SERVER_STATUS = MapViewOfFile (MAP_FILE, FILE_MAP_ALL_ACCESS, 0, 0, sizeof (_Atomic server_status_t)))) {
            CloseHandle (MAP_FILE);

            error ("could not map the shared memory segment.");
        }

        if (atexit (unmap_status))
            warning ("could make the status segment unmap at program exit.");
    }

    if (sprintf (COMMAND, "%s%d", START_SERVER_COMMAND, SERVER_PORT) == -1 ||
        !CreateProcessA (PROG_PATH, COMMAND, NULL, NULL, FALSE, DETACHED_PROCESS, NULL, NULL, &SI, &PI))
        error ("could not start the server.");

    SERVER_PROC = PI.hProcess;

    if (!CloseHandle (PI.hThread))
        warning ("could not close the handle to the new primary thread of the new process.");
#else
    static char PROG_PATH [PATH_MAX + 1]                                                                 = { 0 };
    static char COMMAND [sizeof (START_SERVER_COMMAND) + sizeof (stringify (IANA_DYNAMIC_PORT_END)) - 1] = { 0 };

    if (SERVER_STATUS &&
        (msync (SERVER_STATUS, sizeof (_Atomic server_status_t), MS_SYNC),
         atomic_load (SERVER_STATUS) == server_starting || atomic_load (SERVER_STATUS) == server_restarting))
        return;

    if (!SERVER_STATUS) {
        const cmd_t cmd = { .cmd = cmd_packet };
        for (size_t i                   = 0; i < sizeof (PREALLOC_RECV_BUF) / sizeof (*PREALLOC_RECV_BUF);
             *(PREALLOC_RECV_BUF + i++) = cmd)
            ;

        int fd;
        if (sprintf (SHARED_MEMORY_NAME, _SHARED_MEMORY_NAME "%d", SERVER_PORT) == -1)
            error ("could not open the shared memory resource.");

        static size_t sz;
        sz = (size_t) sysconf (_SC_PAGESIZE);
        if (sz >= sizeof (_Atomic server_status_t)) {
            if (sz % sizeof (_Atomic server_status_t))
                sz += sizeof (_Atomic server_status_t) - sizeof (_Atomic server_status_t) % sz;
            else
                sz = sizeof (_Atomic server_status_t);
        }

        if ((fd = shm_open (SHARED_MEMORY_NAME, O_RDWR, S_IRUSR | S_IWUSR | S_IXUSR)) == -1) {
            if ((fd = shm_open (SHARED_MEMORY_NAME, O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR | S_IXUSR)) == -1)
                error ("could not create the shared memory resource.");

            if (ftruncate (fd, (off_t) sz) == -1)
                error ("could not truncate the shared memory resource to the next multiple of page size of " stringify (
                    sizeof (_Atomic server_status_t)
                ) " bytes.");
        }

        if ((SERVER_STATUS = mmap (NULL, sz, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED)
            error ("could not map the shared memory segment.");

        if (close (fd) == -1)
            warning ("could not close the file descriptor associated with the shared memory resource.");

        if (atexit (unmap_status))
            warning ("could make the status segment unmap at program exit.");
    }

    if ((SERVER_PROC = vfork ()) == -1) {
        msync (SERVER_STATUS, sizeof (_Atomic server_status_t), MS_SYNC);
        atomic_store (SERVER_STATUS, server_error);
        msync (SERVER_STATUS, sizeof (_Atomic server_status_t), MS_SYNC);

        return warning ("could not start the server.");
    }

    if (SERVER_PROC) {
        msync (SERVER_STATUS, sizeof (_Atomic server_status_t), MS_SYNC);
        if (atomic_load (SERVER_STATUS) != server_restarting) {
            atomic_store (SERVER_STATUS, server_starting);
            msync (SERVER_STATUS, sizeof (_Atomic server_status_t), MS_SYNC);
        }

        return;
    }

    close_log_file ();
    stderr_to_null ();

    if (sprintf (COMMAND, "%s%d", START_SERVER_COMMAND, SERVER_PORT) == -1 ||
        execvp (
            ({
                if (readlink ("/proc/self/exe", PROG_PATH, PATH_MAX) == -1)
                    error ("could not get the program name.");
                PROG_PATH;
            }),
            (char *const []) { PROG_PATH, COMMAND, NULL }
        ) == -1)
        error ("could not start the server");

    exit (0);
#endif
}

void stop_server (int port) {
    send_server (NULL, port, kill_command (), NULL, 0);
}

void restart_server (int port) {
    server_status_t s = get_server_status ();
    if (s == server_off)
        return start_server ();

    atomic_store (SERVER_STATUS, server_restarting);

    stop_server (port);
    start_server ();
}

#ifdef _WIN32
SOCKET
#else
int
#endif
impl_connect_server (const char *ip, int port, const char *prevf) {
#ifdef _WIN32
    WSADATA wsa;
    if (WSAStartup (MAKEWORD (2, 2), &wsa)) {
        int r = WSAGetLastError ();

        if (r == WSASYSNOTREADY)
            warning ("the underlying network subsystem is not ready for network communication.");

        else if (r == WSAVERNOTSUPPORTED)
            warning (
                "the version of Windows Sockets support requested is not provided by this particular Windows Sockets implementation."
            );

        else if (r == WSAEINPROGRESS)
            warning ("a blocking Windows Sockets 1.1 operation is in progress.");

        else if (r == WSAEPROCLIM)
            warning ("a limit on the number of tasks supported by the Windows Sockets implementation has been reached."
            );

        else if (r == WSAEFAULT)
            warning ("The lpWSAData parameter is not a valid pointer.");

        return INVALID_SOCKET;
    }
#endif

    struct sockaddr_in addr;

    if (ip) {
        const
#ifdef _WIN32
            unsigned long
#else
            in_addr_t
#endif
                ipaddr = inet_addr (ip);
        if (ipaddr == (
#ifdef _WIN32
                          unsigned long
#else
                          in_addr_t
#endif
                      ) -1) {
            warning ("not a valid IP address.");

            return
#ifdef _WIN32
                (disconnect_server (INVALID_SOCKET), INVALID_SOCKET)
#else
                (disconnect_server (-1), -1)
#endif
                    ;
        }
    }

    else {
        struct hostent *serv;
        if (!(serv = gethostbyname ("localhost"))) {
            warning ("no such host.");

            return
#ifdef _WIN32
                INVALID_SOCKET
#else
                -1
#endif
                ;
        }

        memset (&addr, 0, sizeof (addr));
        addr.sin_family = AF_INET;
        addr.sin_port   = htons ((uint16_t) port);

#ifdef _WIN32
        memcpy (serv->h_addr, &addr.sin_addr.s_addr, (size_t) serv->h_length);
#else
        memcpy (serv->h_addr, &addr.sin_addr.s_addr, (size_t) serv->h_length);
#endif
    }

#ifdef _WIN32
    SOCKET
#else
    int
#endif
    sock;

    if ((sock = socket (AF_INET, SOCK_STREAM, 0)) ==
#ifdef _WIN32
        INVALID_SOCKET
#else
        -1
#endif
    ) {
        warning ("could not create a socket.");

        return sock;
    }

    if (setsockopt (
            sock, SOL_SOCKET, SO_REUSEADDR,
#ifdef _WIN32
            (void *)
#endif
            &(int) { 1 },
            sizeof (int)
        ) == -1)
        warning ("could make the socket reuse local addresses.");

#ifdef _WIN32
    if (ioctlsocket (sock, (long) FIONBIO, &(u_long) { 1 }))
        warning ("could not make the socket non-blocking.");
#else
    {
        const int opts = fcntl (sock, F_GETFL, NULL);
        if (opts == -1)
            warning ("could not get the file status flags of the socket.");

        if (fcntl (sock, F_SETFL, max (opts, 0) | O_NONBLOCK) == -1)
            warning ("could not make the socket non-blocking.");
    }
#endif

    if (connect (sock, (struct sockaddr *) &addr, sizeof (addr)) ==
#ifdef _WIN32
        SOCKET_ERROR
#else
        -1
#endif
    ) {
        message ("waiting for connection (" stringify (CONNECT_TIMEOUT) " milliseconds max).");

        {
#ifdef _WIN32
            WSAPOLLFD pollfd = { .fd = sock };
            if (WSAPoll (&pollfd, 1, CONNECT_TIMEOUT) || (pollfd.revents & (POLLERR | POLLHUP | POLLNVAL)))
#else
            struct pollfd pollfd = { .fd = sock, .events = POLLRDHUP };
            if (errno != EINPROGRESS || poll (&pollfd, 1, CONNECT_TIMEOUT) < 0 ||
                (pollfd.revents & (POLLERR | POLLHUP | POLLRDHUP | POLLNVAL)))
#endif
            {
                if (strcmp (prevf, stringify (get_server_status)))
                    warning ("could not connect to the server.");
                disconnect_server (sock);

                return
#ifdef _WIN32
                    INVALID_SOCKET
#else
                    -1
#endif
                    ;
            }
        }
    }

    return sock;
}

void impl_disconnect_server (
    const
#ifdef _WIN32
    SOCKET
#else
    int
#endif
        socket,
    const bool __attribute__ ((unused)) wsacleanup
) {
#ifdef _WIN32
    DWORD e;
#endif

    if (socket !=
#ifdef _WIN32
        INVALID_SOCKET
#else
        -1
#endif
    ) {
        if (shutdown (
                socket,
#ifdef _WIN32
                SD_BOTH
#else
                SHUT_RDWR
#endif
            ) ==
#ifdef _WIN32
                SOCKET_ERROR
#else
                -1
#endif
            &&
#ifdef _WIN32
            WSAGetLastError () != WSAENOTCONN && WSAGetLastError () != WSAECONNABORTED
#else
            errno == ENOTCONN
#endif
        )
            warning ("could not shut down the connection on the socket");

        if (
#ifdef _WIN32
            closesocket
#else
            close
#endif
            (socket) ==
#ifdef _WIN32
            SOCKET_ERROR
#else
            -1
#endif
        ) {
            warning ("could not close the socket");
        }

#ifdef _WIN32
        if (!wsacleanup)
            return;

        e = GetLastError ();

        if (e == WSANOTINITIALISED)
            return warning ("a successful WSAStartup call must occur before using this function.");

        else if (e == WSAENETDOWN)
            warning ("the network subsystem has failed.");

        else if (e == WSAENOTSOCK)
            warning ("the descriptor is not a socket.");

        else if (e == WSAEINPROGRESS)
            warning (
                "a blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function."
            );

        else if (e == WSAEINTR)
            warning ("the (blocking) Windows Socket 1.1 call was canceled through WSACancelBlockingCall.");

        else if (e == WSAEWOULDBLOCK)
            warning (
                "the socket is marked as nonblocking, but the l_onoff member of the linger structure is set to nonzero and the l_linger member of the linger structure is set to a nonzero timeout value."
            );
#else
        if (errno == EBADF)
            warning ("not a valid file descriptor.");

        else if (errno == EINTR)
            warning ("the close() call was interrupted by a signal.");

        else if (errno == EIO)
            warning ("an I/O error occurred.");
#endif
    }

#ifdef _WIN32
    if (WSACleanup ()) {
        int r = WSAGetLastError ();

        if (r == WSAENETDOWN)
            warning ("the network subsystem has failed.");

        else if (r == WSAEINPROGRESS)
            warning (
                "a blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function."
            );
    }
#endif
}

cmd_t send_server (
    const char *const restrict ip, const int port, const cmd_t cmd, cmd_t *const restrict recv_buf, const size_t recv_sz
) {
#ifdef _WIN32
    SOCKET
#else
    int
#endif
    socket;

    if (cmd.cmd == cmd_success || cmd.cmd == cmd_error) {
        warning ("the server can only be sent packets and commands, not status.");

        return error_command (CMD_ERROR_INVALID);
    }

    if ((socket = connect_server (ip, port)) ==
#ifdef _WIN32
        INVALID_SOCKET
#else
        -1
#endif
    )
        return error_command (CMD_ERROR_SEND);

    if (send (
            socket, (const char *) &cmd,
            cmd.cmd == cmd_packet ? sizeof (cmd_t) : (offsetof (cmd_t, info.arg) + sizeof (cmd.info.arg)), 0
        ) ==
#ifdef _WIN32
        SOCKET_ERROR
#else
        -1
#endif
    ) {
        if (cmd.cmd == cmd_packet)
            warning ("could not send packet to the server.");

        else
            warning ("could not send the command to the server.");

        disconnect_server (socket);

        return error_command (CMD_ERROR_SEND);
    }

    cmd_t resp;
    if (recv (socket, (char *) &resp, sizeof (resp), 0) ==
#ifdef _WIN32
        SOCKET_ERROR
#else
        -1
#endif
    ) {
        message ("waiting for packet receival (" stringify (RECV_TIMEOUT) " milliseconds max).");

        {
#ifdef _WIN32
            struct pollfd pollfd = { .fd = socket };
            if (WSAPoll (&pollfd, 1, CONNECT_TIMEOUT) || (pollfd.revents & (POLLERR | POLLHUP | POLLNVAL)))
#else
            struct pollfd pollfd = { .fd = socket, .events = POLLIN | POLLPRI };
            if (errno != EINPROGRESS || poll (&pollfd, 1, RECV_TIMEOUT) < 0 || !(pollfd.revents & (POLLIN | POLLPRI)))
#endif
            {
                warning ("could not get a response from the server.");
                disconnect_server (socket);

                return error_command (CMD_ERROR_RECV);
            }
        }
    }

    if (resp.cmd == cmd_packet_cont && recv_buf) {
        for (size_t i = 0; i < recv_sz; i++) {
            if (recv (socket, (void *) (recv_buf + i), sizeof (cmd_t), 0) ==
#ifdef _WIN32
                SOCKET_ERROR
#else
                -1
#endif
            ) {
                message ("waiting for packet receival (" stringify (RECV_TIMEOUT) " milliseconds max).");

#ifdef _WIN32
                {
                    struct pollfd pollfd = { .fd = socket };
                    if (WSAPoll (&pollfd, 1, CONNECT_TIMEOUT) || (pollfd.revents & (POLLERR | POLLHUP | POLLNVAL)))
#else
                struct pollfd pollfd = { .fd = socket, .events = POLLIN | POLLPRI };
                if (errno != EINPROGRESS || poll (&pollfd, 1, RECV_TIMEOUT) < 0 ||
                    !(pollfd.revents & (POLLIN | POLLPRI)))
#endif
                    {
                        warning ("could not receive a packet from the server");

                        break;
                    }
#ifdef _WIN32
                }
#endif
            }

            if ((recv_buf + i)->cmd != cmd_packet_cont)
                break;
        }
    }

    disconnect_server (socket);

    return resp;
}
