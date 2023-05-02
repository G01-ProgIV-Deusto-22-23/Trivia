#ifndef TRIVIA_SERVER_H
#define TRIVIA_SERVER_H

#define MAX_GAMES         DEFAULT_MAP_CAPACITY
#define DEFAULT_GAMES     (MAX_GAMES / 2)
#define DEFAULT_PUB_GAMES 50

#define IANA_DYNAMIC_PORT_START ((1 << 16) - (1 << 14))
#define IANA_DYNAMIC_PORT_END   (1 << 16)

#define DEFAULT_SERVER_PORT     21023
#define DEFAULT_GAME_PORT_START IANA_DYNAMIC_PORT_START
#define DEFAULT_GAME_PORT_END   IANA_DYNAMIC_PORT_END

#define MAX_PLAYERS 20

#ifdef _WIN32
extern const char *get_game_arg (void);
#endif

extern size_t get_games (void);
extern size_t set_games (const size_t);
extern void   gen_game_ids (void);
extern bool   init_game (const size_t, const bool);
extern bool   init_games (void);
extern bool   end_games (void);
extern void   game_server (const int, const size_t);

extern server_status_t get_server_status (void);
extern choicefunc_t    start_server;
extern choicefunc_t    stop_server;
extern choicefunc_t    restart_server;

extern int get_server_port (void);
extern int set_server_port (int);
extern int get_game_port_start (void);
extern int set_game_port_start (int);
extern int get_game_port_end (void);
extern int set_game_port_end (int);
extern int get_next_free_port (int, int);
extern int get_next_free_port (int, int);

#endif
