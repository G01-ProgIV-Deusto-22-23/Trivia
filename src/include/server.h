#ifndef TRIVIA_SERVER_H
#define TRIVIA_SERVER_H

#define MAX_GAMES     100
#define DEFAULT_GAMES (MAX_GAMES / 2)

#define DEFAULT_SERVER_PORT 210203

extern server_status_t get_server_status (void);
extern choicefunc_t    start_server;
extern choicefunc_t    stop_server;
extern choicefunc_t    restart_server;

#endif
