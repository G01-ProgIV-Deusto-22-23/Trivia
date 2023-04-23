static int             SERVER_PORT     = DEFAULT_SERVER_PORT;
static server_status_t SERVER_STATUS   = server_off;
static int             GAME_PORT_START = DEFAULT_GAME_PORT_START;
static int             GAME_PORT_END   = DEFAULT_GAME_PORT_END;

server_status_t get_server_status (void) {
    return SERVER_STATUS;
}

server_status_t set_server_status (server_status_t status) {
    return SERVER_STATUS = status;
}

int get_server_port (void) {
    return SERVER_PORT;
}

int set_server_port (int port) {
    if (port > UINT16_MAX) {
        warning ("not a valid port number.");

        return SERVER_PORT;
    }

    if (port <= 1 << 10)
        warning ("using ports less or equal to 1024 is not recommended.");

    if (!port)
        port = DEFAULT_SERVER_PORT;

    return SERVER_PORT = port;
}

void start_server (void) {
    if (get_server_status () == server_on)
        return;

    set_server_status (server_on);
}

void stop_server (void) {
    if (get_server_status () == server_off)
        return;

    set_server_status (server_off);
}

void restart_server (void) {
    stop_server ();
    start_server ();
}