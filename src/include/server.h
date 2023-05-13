#ifndef TRIVIA_SERVER_H
#define TRIVIA_SERVER_H

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_GAMES         DEFAULT_MAP_CAPACITY
#define DEFAULT_GAMES     (MAX_GAMES / 2)
#define DEFAULT_PUB_GAMES 50

#define IANA_DYNAMIC_PORT_START ((1 << 16) - (1 << 14))
#define IANA_DYNAMIC_PORT_END   (1 << 16)

#define DEFAULT_SERVER_PORT IANA_DYNAMIC_PORT_START

#define DEFAULT_GAME_PORT_START DEFAULT_SERVER_PORT + 1
#define DEFAULT_GAME_PORT_END   IANA_DYNAMIC_PORT_END

#define MAX_PLAYERS 20

    cmd_t error_command (const uint32_t);
    cmd_t kill_command (void);
    cmd_t game_command (const game_attr_t);
    void  packet (
         cmd_t *const restrict,
         const char [
#ifndef __cplusplus
            static
#endif
            1],
        size_t, const bool
    );

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

    extern const char     *get_start_server_command (void);
    extern server_status_t get_server_status (void);
    extern void            impl_start_server (void);
    extern void            start_server (void);
    extern void            stop_server (int);
    extern void            restart_server (int);
    extern
#ifdef _WIN32
        SOCKET
#else
    int
#endif
                impl_connect_server (const char *, int, const char *);
    extern void disconnect_server (const
#ifdef _WIN32
                                   SOCKET
#else
                               int
#endif
    );
    extern cmd_t send_server (const char *, int, const cmd_t);

#define connect_server(a, p) impl_connect_server (a, p, __func__)

    extern int get_server_port (void);
    extern int set_server_port (int);
    extern int get_game_port_start (void);
    extern int set_game_port_start (int);
    extern int get_game_port_end (void);
    extern int set_game_port_end (int);
    extern int get_next_free_port (int, int);
    extern int get_next_free_port (int, int);

#ifdef __cplusplus
}
#endif

#endif
