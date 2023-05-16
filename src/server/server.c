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

    disconnect_server (sock);

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

    close_log_file ();
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

#ifdef _WIN32
    WSADATA wsa;
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
        disconnect_server (sock);

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
        disconnect_server (sock);

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
        if (ioctlsocket (client_sock, FIONBIO, &(u_long) { 1 }))
            warning ("could not make the socket non-blocking.");
#else
        {
            const int opts = fcntl (client_sock, F_GETFL, NULL);
            if (opts == -1)
                warning ("could not get the file status flags of the socket.");

            if (fcntl (sock, F_SETFL, max (opts, 0) | O_NONBLOCK) == -1)
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
            warning ("could not receive any data from the client.");

            continue;
        }

        if (!recvlen) {
            message ("the client closed the connection");

            continue;
        }

        if (!(cmd.cmd == cmd_kill || cmd.cmd == cmd_game)) {
            warning ("The server only accepts two commands (kill and game), ignoring the received command.");

            continue;
        }

        if (cmd.cmd == cmd_game) {
            if (send (
                    client_sock, (packet (&cmd, init_game (cmd.info.game, 0), 0, false), (void *) &cmd), sizeof (cmd_t),
                    0
                ) ==
#ifdef _WIN32
                SOCKET_ERROR
#else
                -1
#endif
            )
                warning ("could not send the details of the game to the client.");

            disconnect_server (client_sock);

            continue;
        }

        message ("kill command received.");
        if (send (client_sock, (cmd = success_command (), (void *) &cmd), sizeof (cmd_t), 0) ==
#ifdef _WIN32
            SOCKET_ERROR
#else
            -1
#endif
        )
            warning ("could not send the command feedback to the client.");

        disconnect_server (client_sock);
        disconnect_server (sock);

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
            warning ("not a valid ip address.");

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
    if (ioctlsocket (sock, FIONBIO, &(u_long) { 1 }))
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
            int r;
            if (!(r = WSAPoll (&(WSAPOLLFD) { .fd = sock }, 1, CONNECT_TIMEOUT)) || r == SOCKET_ERROR)
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

void disconnect_server (const
#ifdef _WIN32
                        SOCKET
#else
                        int
#endif
                            socket) {
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
            int r;
            if (!(r = WSAPoll (&(WSAPOLLFD) { .fd = socket }, 1, RECV_TIMEOUT)) || r == SOCKET_ERROR)
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
        for (size_t i = 0; i < recv_sz;) {
            if (recv (socket, recv_buf + i++, sizeof (cmd_t), 0) ==
#ifdef _WIN32
                SOCKET_ERROR
#else
                -1
#endif
            ) {
                message ("waiting for packet receival (" stringify (RECV_TIMEOUT) " milliseconds max).");

#ifdef _WIN32
                {
                    int r;
                    if (!(r = WSAPoll (&(WSAPOLLFD) { .fd = socket }, 1, RECV_TIMEOUT)) || r == SOCKET_ERROR)
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
        }
    }

    disconnect_server (socket);

    return resp;
}
