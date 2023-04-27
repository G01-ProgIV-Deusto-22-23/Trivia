#ifndef TRIVIA_SERVER_H
#define TRIVIA_SERVER_H

#define MAX_GAMES     DEFAULT_MAP_CAPACITY
#define DEFAULT_GAMES (MAX_GAMES / 2)

#define IANA_DYNAMIC_PORT_START ((1 << 14) + (1 << 16))
#define IANA_DYNAMIC_PORT_END   (1 << 16)

#define DEFAULT_SERVER_PORT     21023
#define DEFAULT_GAME_PORT_START IANA_DYNAMIC_PORT_START
#define DEFAULT_GAME_PORT_END   IANA_DYNAMIC_PORT_END

extern server_status_t get_server_status (void);
extern choicefunc_t    start_server;
extern choicefunc_t    stop_server;
extern choicefunc_t    restart_server;

#endif
