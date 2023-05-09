#include "port.h"

static _Atomic server_status_t *SERVER_STATUS = NULL;

#ifdef _WIN32
static HANDLE TERM_THREAD = NULL;
#endif

static
#ifdef _WIN32
    HANDLE
#else
    pthread_t
#endif
        UNMAP_THREAD;

static
#ifdef _WIN32
    HANDLE
#else
    int
#endif
        SERVER_PROC = 0;

static void *unmap_status (
#if defined(__cpp_attributes) || __STDC_VERSION__ > 201710L
    void [[unused]] * unused
#else
    void __attribute__ ((unused)) * unused
#endif
) {
    if (!SERVER_STATUS)
        return NULL;

#ifdef _WIN32

#else
    waitpid (SERVER_PROC, NULL, WNOHANG);

    if (munmap (SERVER_STATUS, sizeof (size_t)) == -1)
        warning ("could not unmap the shared memory segment.");
#endif

    return SERVER_STATUS = NULL;
}

server_status_t get_server_status (void) {
    return SERVER_STATUS ? atomic_load (SERVER_STATUS) : server_off;
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

    if (atomic_load (SERVER_STATUS) != server_restarting)
        atomic_store (SERVER_STATUS, server_off);

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

void start_server (void) {
    if (atomic_load (SERVER_STATUS) == server_on)
        return;

    if (!SERVER_STATUS) {
#ifdef _WIN32
    }
#else
        if ((SERVER_STATUS = mmap (
                 NULL, sizeof (_Atomic server_status_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0
             )) == MAP_FAILED)
            error ("could not map the shared memory segment.");
    }

    if ((SERVER_PROC = vfork ()) == -1)
        error ("could not start the server.");

    if (SERVER_PROC) {
        if (pthread_create (&UNMAP_THREAD, NULL, unmap_status, NULL))
            warning ("could not create the status unmap thread.");

        for (server_status_t s; (s = atomic_load (SERVER_STATUS)) == server_starting || s == server_restarting;)
            ;

        return;
    }
#endif

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
#else
    sigaction (SIGTERM, &(struct sigaction) { .sa_handler = term_handler }, NULL);
    sigaction (SIGABRT, &(struct sigaction) { .sa_handler = term_handler }, NULL);
#endif

    if (atomic_load (SERVER_STATUS) != server_restarting)
        atomic_store (SERVER_STATUS, server_starting);

    return atomic_store (SERVER_STATUS, server_error);

    atomic_store (SERVER_STATUS, server_on);
}

void stop_server (int port) {
    if (atomic_load (SERVER_STATUS) == server_off)
        return;

    if (SERVER_PROC)
        return (void) send_server (NULL, port, kill_command ());

    raise (SIGTERM);
}

void restart_server (int port) {
    if (!SERVER_STATUS)
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
connect_server (const char *ip, int port) {
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

    if (connect (sock, (struct sockaddr *) &addr, sizeof (addr)) ==
#ifdef _WIN32
        SOCKET_ERROR
#else
        -1
#endif
    ) {
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

cmd_t send_server (const char *ip, int port, const cmd_t cmd) {
#ifdef _WIN32
    DWORD e;
#endif

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
        warning ("could not get a response from the server.");
        disconnect_server (socket);

        return error_command (CMD_ERROR_RECV);
    }

    return resp;
}
