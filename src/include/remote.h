#ifndef TRIVIA_REMOTE_H
#define TRIVIA_REMOTE_H

extern
#ifdef _WIN32
    SOCKET
#else
    int
#endif
    locate_server (void);

extern cmd_t login_menu (
#ifdef _WIN32
    SOCKET
#else
    int
#endif
);

#endif