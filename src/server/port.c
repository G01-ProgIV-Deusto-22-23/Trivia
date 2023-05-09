int SERVER_PORT     = DEFAULT_SERVER_PORT;
int GAME_PORT_START = DEFAULT_GAME_PORT_START;
int GAME_PORT_END   = DEFAULT_GAME_PORT_END;

int get_server_port (void) {
    return SERVER_PORT;
}

int set_server_port (int port) {
    if (!port)
        return SERVER_PORT = DEFAULT_SERVER_PORT;

    if (port > IANA_DYNAMIC_PORT_END) {
        warning ("not a valid port number.");

        return SERVER_PORT;
    }

    if (port <= 1 << 10)
        warning ("using ports less or equal to 1024 is not recommended.");

    return SERVER_PORT = port;
}

int get_game_port_start (void) {
    return GAME_PORT_START;
}

int set_game_port_start (int port) {
    if (!port)
        return GAME_PORT_START = DEFAULT_GAME_PORT_START;

    if (port > IANA_DYNAMIC_PORT_END) {
        warning ("not a valid port number.");

        return GAME_PORT_START;
    }

    if (port < IANA_DYNAMIC_PORT_START)
        warning ("using ports for games below the start of the AIANA dynamic port range (49152) is not recommended.");

    if (port > GAME_PORT_END) {
        warning ("the game port range start cannot be greater than the range end, the defaults will be restored.");

        GAME_PORT_END          = DEFAULT_GAME_PORT_END;
        return GAME_PORT_START = DEFAULT_GAME_PORT_START;
    }

    if (GAME_PORT_END - port < (int) get_games ())
        warning ("the amount of ports in the range is not currently enough to satistfy the amount of games.");

    return GAME_PORT_START = port;
}

int get_game_port_end (void) {
    return GAME_PORT_END;
}

int set_game_port_end (int port) {
    if (!port)
        return GAME_PORT_START = DEFAULT_GAME_PORT_END;

    if (port > IANA_DYNAMIC_PORT_END) {
        warning ("not a valid port number.");

        return GAME_PORT_START;
    }

    if (port < IANA_DYNAMIC_PORT_START)
        warning ("using ports for games below the start of the AIANA dynamic port range (49152) is not recommended.");

    if (port < GAME_PORT_START) {
        warning ("the game port range end cannot be smaller than the range start, the defaults will be restored.");

        GAME_PORT_START      = DEFAULT_GAME_PORT_START;
        return GAME_PORT_END = DEFAULT_GAME_PORT_END;
    }

    if (port - GAME_PORT_START < (int) get_games ())
        warning ("the amount of ports in the range is currently not enough to satistfy the amount of games.");

    return GAME_PORT_END = port;
}

int get_next_free_port (int start, int end) {
    if (!(start || end)) {
        start = IANA_DYNAMIC_PORT_START;
        end   = IANA_DYNAMIC_PORT_END;
    }

    if (start > end || start < 0 || end > IANA_DYNAMIC_PORT_END) {
        warning ("invalid port lookup range.");

        return -1;
    }

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

        return -1;
    }
#endif

    struct hostent *serv;
    if (!(serv = gethostbyname ("localhost"))) {
        warning ("no such host.");

        return -1;
    }

    struct sockaddr_in addr;
    memset (&addr, 0, sizeof (addr));
    addr.sin_family = AF_INET;
#ifdef _WIN32
    memcpy (serv->h_addr, &addr.sin_addr.s_addr, (size_t) serv->h_length);
#else
    memcpy (serv->h_addr, &addr.sin_addr.s_addr, (size_t) serv->h_length);
#endif

#ifdef _WIN32
    int    ret = -1;
    SOCKET sock;
    {
#endif
        for (int p = start
#ifndef _WIN32
             ,
                 sock
#endif
             ;
#ifdef _WIN32
             ret != -1 &&
#endif
             p <= end;
             p++) {
            if ((sock = socket (AF_INET, SOCK_STREAM, 0))
#ifdef _WIN32
                == INVALID_SOCKET
#else
            < 0
#endif
            ) {
                warning ("could not open socket.");

                return -1;
            }

            addr.sin_port = htons (
#ifdef _WIN32
                (u_short)
#else
            (uint16_t)
#endif
                    p
            );

            if (!connect (sock, (struct sockaddr *) &addr, sizeof (addr))) {
#ifdef _WIN32
                ret = p;
#endif

                if (
#ifdef _WIN32
                    closesocket
#else
                close
#endif
                    (sock) ==
#ifdef _WIN32
                    SOCKET_ERROR
#else
                -1
#endif
                )
                    warning ("could not close the socket");

#ifndef _WIN32
                return p;
#endif
            }
        }
#ifdef _WIN32
    }
#endif

#ifdef _WIN32
    if (WSACleanup ()) {
        int r = WSAGetLastError ();

        if (r == WSANOTINITIALISED)
            warning ("a successful WSAStartup call must occur before using this function.");

        else if (r == WSAENETDOWN)
            warning ("the network subsystem has failed.");

        else if (r == WSAEINPROGRESS)
            warning (
                "a blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function."
            );
    }
#endif

    return
#ifdef _WIN32
        ret
#else
        -1
#endif
        ;
}