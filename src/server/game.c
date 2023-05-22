typedef struct __game_struct {
#ifdef _WIN32
        HANDLE
#else
        int
#endif
        proc;
        int  port;
        int  res [MAX_PLAYERS];
        bool pub;
} *game_t;

static size_t NGAMES    = DEFAULT_GAMES;
static size_t PUB_GAMES = DEFAULT_PUB_GAMES;
static size_t
#ifndef _WIN32
    *
#endif
                            CURGAME                                            = 0;
static map_t                GAMES                                              = NULL;
static struct __game_struct GAME_STRUCTS [MAX_GAMES]                           = { 0 };
static char                 GAME_IDS [MAX_GAMES][sizeof ("XXXX")]              = { 0 };
static char                 GAME_LIST_RAW [MAX_GAMES * (sizeof ("XXXX;") - 1)] = { 0 };
static size_t __attribute__ ((unused)) NPLAYERS [MAX_GAMES]                    = { 0 };
static int PLAYER_RESULTS [MAX_GAMES][MAX_PLAYERS];

static
#ifndef _WIN32
    pthread_t GAME_THREADS [MAX_GAMES][MAX_PLAYERS + 1];
static atomic_bool __attribute__ ((unused)) PLAYER_THREAD_STILL_RUNNING [MAX_GAMES][MAX_PLAYERS];
#endif

#ifdef _WIN32
static const char GAME_ARG [] = "juan";

const char *get_game_arg (void) {
    return GAME_ARG;
}
#endif

const char *game_list_raw (void) {
    ct_error (
        MAX_GAMES * (sizeof ("XXXX;") - 1) > (sizeof (packet_t) - offsetof (packet_t, text)),
        "the size of the games raw list cannot be larger than that of the contents of a packet."
    );

    return GAME_LIST_RAW;
}

void game_list_parse (char dest [MAX_GAMES][sizeof ("XXXX")], const char *src) {
    if (!(dest || src))
        return;

    for (size_t i = 0; *src; src += sizeof ("XXXX") - 1)
        *(char *) mempcpy (*(dest + i++), ++src, sizeof ("XXXX")) = '\0';
}

size_t get_games (void) {
    return NGAMES;
}

size_t set_games (const size_t n) {
    return NGAMES = n ? min ((size_t) MAX_GAMES, n) : DEFAULT_GAMES;
}

size_t get_pub_games_perc (void) {
    return PUB_GAMES;
}

size_t set_pub_games_perc (const size_t perc) {
    return PUB_GAMES = max ((size_t) 100, perc);
}

int get_game_port (const char *const restrict id) {
    const game_t g = get_map (GAMES, id);
    return g ? g->port : -1;
}

void gen_game_ids (void) {
    static const char     l [] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    static uint64_t       v;
    static struct timeval tv;

    gettimeofday (&tv, NULL);
    v += ((uint64_t) tv.tv_usec << 16) ^ (uint64_t) tv.tv_usec ^
         (uint64_t)
#ifdef _WIN32
             GetProcessId (GetCurrentProcess ())
#else
             getpid ()
#endif
        ;

    char *off = GAME_LIST_RAW;
    for (size_t i = 0; i < sizeof (GAME_IDS) / sizeof (*GAME_IDS);
         *off++   = i == sizeof (GAME_IDS) / sizeof (*GAME_IDS) - 1 ? '\0' : ';') {
        srand ((unsigned) v);
        v += (unsigned) rand ();

        **(GAME_IDS + i)       = *(l + v % (sizeof (l) - 1));
        v                      /= sizeof (l) - 1;
        *(*(GAME_IDS + i) + 1) = *(l + v % (sizeof (l) - 1));
        v                      /= sizeof (l) - 1;
        *(*(GAME_IDS + i) + 2) = *(l + v % (sizeof (l) - 1));
        v                      /= sizeof (l) - 1;
        *(*(GAME_IDS + i) + 3) = *(l + v % (sizeof (l) - 1));

        v   += 7777;
        off = mempcpy (off, *(GAME_IDS + i++), sizeof ("XXXX"));
    }
}

const char *init_game (game_attr_t attr, int port, const bool pub) {
    if (
#ifndef _WIN32
        !CURGAME || *
#endif
                                   CURGAME >= capacity_map (GAMES))
        return false;

#ifdef _WIN32
    STARTUPINFOA        si;
    PROCESS_INFORMATION pi;
    char                path [MAX_PATH + 1] = { 0 };
    char                cmd
        [sizeof (GAME_ARG) + sizeof (stringify (IANA_DYNAMIC_PORT_END)) + sizeof (" ") +
         sizeof (stringify (MAX_PLAYERS)) - 3] = { 0 };

    if (!GetModuleFileNameA (NULL, path, MAX_PATH + 1))
        error ("could not get the executable filename.");

    GetStartupInfoA (&si);
#else
    pid_t p;
#endif

    if (!attr.players)
        attr.players = MAX_PLAYERS;

    if (!attr.round_time)
        attr.round_time = DEFAULT_ROUND_TIME;

    if (!attr.rounds)
        attr.rounds = MAX_ROUNDS;

    bool r = (port = get_next_free_port (port, get_game_port_end ())) == -1 ||
#ifdef _WIN32
             !CreateProcessA (
                 path, ({
                     snprintf (
                         cmd, sizeof (cmd), "%s%d %" PRISZ " %" PRIu8 " %" PRIu8, GAME_ARG, port, CURGAME, attr.players,
                         attr.round_time
                     );
                     cmd;
                 }),
                 NULL, NULL, FALSE, DETACHED_PROCESS, NULL, NULL, &si, &pi
             )
#else
             ({
                 p = fork ();
                 if (!p) {
                     game_server (*CURGAME, port, attr);

                     exit (0);
                 }
                 p;
             }) == -1
#endif
             || !put_map (
                    GAMES,
                    *(GAME_IDS +
#ifdef _WIN32
                      CURGAME++
#else
                      (*CURGAME)++
#endif
                    ),
                    memcpy (
                        GAME_STRUCTS + 1,
                        &(struct __game_struct
                        ) { .proc =
#ifdef _WIN32
                                (CloseHandle (pi.hThread)
                                     ? (void) 0
                                     : warning ("could not close the handle to the primary thread of the new process."),
                                 pi.hProcess)
#else
                                p
#endif
                                ,
                            .port = port,
                            .pub  = pub },
                        sizeof (struct __game_struct)
                    )
                );

#ifdef _WIN32
    memset (path, 0, sizeof (path));
    memset (cmd, 0, sizeof (cmd));
#endif

    return r ? ""
             : *(GAME_IDS +
#ifdef _WIN32
                 CURGAME
#else
                 *CURGAME
#endif
                 - 1);
}

bool init_games (void) {
#ifndef _WIN32
    if (CURGAME)
        if (munmap (CURGAME, sizeof (size_t)) == -1)
            warning ("could not unmap the shared memory segment.");

    if ((CURGAME = mmap (NULL, sizeof (size_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)) ==
        MAP_FAILED)
        error ("could not map the shared memory segment.");
#endif

    __builtin_choose_expr (
        sizeof (wchar_t) == sizeof (int) && (wchar_t) -1 == (int) -1,
        wmemset ((wchar_t *) PLAYER_RESULTS, (wchar_t) -1, sizeof (PLAYER_RESULTS) / sizeof (**PLAYER_RESULTS)), ({
            for (size_t i = 0, j; i < sizeof (PLAYER_RESULTS) / sizeof (**PLAYER_RESULTS); i++)
                for (j = 0; j < sizeof (*PLAYER_RESULTS) / sizeof (**PLAYER_RESULTS);
                     **(PLAYER_RESULTS + i * sizeof (*PLAYER_RESULTS) / sizeof (**PLAYER_RESULTS) + j++) = -1)
                    ;
        })
    );

    size_t prev = capacity_map (GAMES);

    if (!resize_map (GAMES, NGAMES))
        return false;

    bool e = false;

    if (!prev)
        gen_game_ids ();

    int p = get_game_port_start ();

#ifdef _WIN32
    BYTE
#else
    unsigned char
#endif
        randbytes [MAX_GAMES] = { 0 };

#ifdef _WIN32
    char       crypto_name [512] = { 0 };
    DWORD      crypto_name_sz    = sizeof (crypto_name);
    HCRYPTPROV crypto            = NULL;

    if (!(CryptGetDefaultProviderA (PROV_RSA_FULL, NULL, CRYPT_MACHINE_DEFAULT, crypto_name, &crypto_name_sz) &&
          CryptAcquireContextA (&crypto, NULL, crypto_name, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_SILENT) &&
          CryptGetRandom (crypto, MAX_GAMES, randbytes)) ||
        crypto_name_sz < sizeof (randbytes))
        warning ("could not get enough random bytes to fill the buffer with " stringify (MAX_GAMES) " bytes.");

    CryptReleaseContext (crypto, 0);
#else
    if (getrandom (randbytes, MAX_GAMES, 0) < MAX_GAMES)
        warning ("could not get enough random bytes from /dev/urandom to fill the buffer with " stringify (MAX_GAMES
        ) " bytes.");
#endif

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnull-dereference"

    for (size_t n = (size_t) ((double) (NGAMES * PUB_GAMES) / 100.0);
#ifdef _WIN32
         CURGAME
#else
         *CURGAME
#endif
         < n;)
        e |= !*init_game (
            (game_attr_t) { .players =
#ifndef _WIN32
                                *
#endif
                                        CURGAME < n / 3
                                    ? MAX_PLAYERS >> 2
                                :
#ifndef _WIN32
                                *
#endif
                                        CURGAME < 2 * n / 3
                                    ? MAX_PLAYERS >> 1
                                    : MAX_PLAYERS,
                            .round_time = DEFAULT_ROUND_TIME,
                            .rounds =
                                *(((uint8_t []) { MAX_ROUNDS, MAX_ROUNDS / 2, MAX_ROUNDS / 4 }) + *(randbytes +
#ifndef _WIN32
                                                                                                    *
#endif
                                                                                                    CURGAME) %
                                                                                                      3) },
            p++, true
        );

#pragma GCC diagnostic pop

    return !e;
}

static bool kill_game (const struct __game_struct g) {
#ifdef _WIN32
    static DWORD   ExitCode;
    static WINBOOL ec;
#endif

    return
#ifdef _WIN32
        ({
            if (!(ec = GetExitCodeProcess (g.proc, (LPDWORD) &ExitCode)))
                ExitCode = 0;
            !(TerminateProcess (g.proc, ExitCode) && ec);
        })
#else
        kill (g.proc, SIGTERM) == -1 && (errno != ESRCH)
#endif
            ;
}

static bool kill_games (void) {
    static game_t g;
    static size_t n;
    if (!(g = values_map (GAMES, &n))) {
        warning ("could not retrieve the games array from the map.");

        return false;
    }

    bool e = false;
    for (size_t i =
#ifndef _WIN32
             *
#endif
             CURGAME = 0;
         i < n; e    |= kill_game (*(g + i++)) ? (warning ("could not kill game."), true) : false)
        ;

    free (g);
    memset (GAME_STRUCTS, 0, sizeof (GAME_STRUCTS));

#ifndef _WIN32
    if (munmap (CURGAME, sizeof (size_t)) == -1)
        warning ("could not unmap the shared memory segment.");
#endif

    return !e;
}

bool end_games (void) {
    const bool e = kill_games ();

    if (!destroy_map (GAMES))
        warning ("could not destroy the games map successfully.");

    return e;
}

#ifndef _WIN32
static void *watch_for_parent (void __attribute__ ((unused)) * unused) {
    waitpid (getppid (), NULL, 0);
    raise (SIGTERM);

    return NULL;
}
#endif

__attribute__ ((noreturn)) void game_server (const size_t ngame, const int port, game_attr_t attr) {
    ct_error (
        sizeof (question_t) > sizeof (packet_t), "the size of a question cannot be bigger than that of a packet."
    );

    ct_error (40 > MAX_QUESTIONS, "the maximum number of questions is smaller than the maximum of rounds.");

    ct_error (
        MAX_QUESTIONS > UINT8_MAX,
        "the maximum amount of questions cannot be bigger than the possible number that fits in " stringify (uint8_t
        ) " " stringify (UINT8_MAX) "."
    );

    static char LOG_FILE
        [sizeof ("game_port_pid.log") + sizeof (stringify (IANA_DYNAMIC_PORT_END)) + sizeof (" ") +
         sizeof (stringify (
#ifdef _WIN32
             DWORD_MAX
#else
             INT_MAX
#endif
         )) -
         3] = { 0 };

#ifndef _WIN32
    sigaction (SIGTERM, &(struct sigaction) { .sa_handler = SIG_DFL }, NULL);
    sigaction (SIGABRT, &(struct sigaction) { .sa_handler = SIG_DFL }, NULL);

    pthread_create (*(GAME_THREADS + ngame), NULL, watch_for_parent, NULL);
#endif

    if (!attr.players)
        attr.players = MAX_PLAYERS;

    if (!attr.round_time)
        attr.round_time = DEFAULT_ROUND_TIME;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-extra-args"

    set_log_file ((char *) (sprintf (
                                LOG_FILE,
                                "game_port%d_pid%"
#ifdef _WIN32
                                "lu"
#else
                                "d"
#endif
                                ".log",
                                port,
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

    static uint8_t info [sizeof (game_attr_t) + MAX_ROUNDS];
    memcpy (info, &attr, sizeof (game_attr_t));

#ifdef _WIN32
    static char       crypto_name [512] = { 0 };
    static DWORD      crypto_name_sz    = sizeof (crypto_name);
    static HCRYPTPROV crypto            = NULL;

    if (!(CryptGetDefaultProviderA (PROV_RSA_FULL, NULL, CRYPT_MACHINE_DEFAULT, crypto_name, &crypto_name_sz) &&
          CryptAcquireContextA (&crypto, NULL, crypto_name, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_SILENT) &&
          CryptGetRandom (crypto, attr.rounds, info + sizeof (game_attr_t))) ||
        crypto_name_sz < attr.rounds)
        warning ("could not get enough random bytes to fill the buffer");

    CryptReleaseContext (crypto, 0);
#else
    if (getrandom (info + sizeof (game_attr_t), attr.rounds, 0) < attr.rounds)
        warning ("could not get enough random bytes from /dev/urandom to fill the buffer");
#endif

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

    if (port < 0 || port > IANA_DYNAMIC_PORT_END)
        error ("not a valid port.");

    if (port < IANA_DYNAMIC_PORT_START)
        warning ("the minimum recommended port for games is the first IANA dynamic port (" stringify (
            IANA_DYNAMIC_PORT_START
        ) ").");

    if (get_next_free_port (port, port) == -1)
        error ("the port is not available.");

    struct sockaddr_in addr;
    addr.sin_family      = AF_INET;
    addr.sin_port        = htons ((uint16_t) port);
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

        error ("could not bind.");
    }

    if (listen (sock, 1) == -1) {
        disconnect_server (sock, true);

        error ("could not listen.");
    }

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

        if (send (client_sock, info, sizeof (game_attr_t) + attr.rounds, 0) ==
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
                    warning ("could not the game information (game attributes and questions indices).");
            }
        }

        disconnect_server (client_sock);
    }

    disconnect_server (sock, true);

    exit (0);
}