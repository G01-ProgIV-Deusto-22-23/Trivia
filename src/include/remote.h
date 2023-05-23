#ifndef TRIVIA_REMOTE_H
#define TRIVIA_REMOTE_H

#include "../remote/questionhandler/questionhandler.h"

extern void local_game (void);

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

extern void user_menu (
    const
#ifdef _WIN32
    SOCKET
#else
    int
#endif
    ,
    Usuario
);

#endif