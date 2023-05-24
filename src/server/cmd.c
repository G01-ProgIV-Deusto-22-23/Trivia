cmd_t success_command (void) {
    return (cmd_t) {};
}

cmd_t error_command (const uint32_t error) {
    return (cmd_t) { .cmd = cmd_error, .info.arg [0] = error };
}

cmd_t kill_command (void) {
    return (cmd_t) { .cmd = cmd_kill };
}

cmd_t game_list_command (void) {
    return (cmd_t) { .cmd = cmd_game_list };
}

__attribute__ ((nonnull (1))) cmd_t connect_game_command (const char id [sizeof ("XXXX")]) {
    cmd_t c = { .cmd = cmd_game_connect };
    memcpy (c.info.pack.text, id, sizeof ("XXXX"));

    return c;
}

cmd_t create_game_command (const game_attr_t game) {
    return (cmd_t) { .cmd = cmd_game_create, .info.game = game };
}

cmd_t user_fetch_command (const Usuario u) {
    return (cmd_t) { .cmd = cmd_user_fetch, .info.user = u };
}

cmd_t user_creds_command (const Usuario u) {
    return (cmd_t) { .cmd = cmd_user_creds, .info.user = u };
}

cmd_t insert_user_command (const Usuario u) {
    return (cmd_t) { .cmd = cmd_user_insert, .info.user = u };
}

cmd_t update_user_command (const Usuario u) {
    return (cmd_t) { .cmd = cmd_user_update, .info.user = u };
}

cmd_t delete_user_command (const Usuario u) {
    cmd_t cmd;
    packet (&cmd, u.username, sizeof (u.username) - 1, false);
    cmd.cmd = cmd_user_delete;

    return cmd;
}

void packet (cmd_t *const restrict dest, const char src [static 1], size_t sz, const bool cont) {
    if (!dest)
        return;

    if (!src)
        return memset (dest, 0, sizeof (cmd_t)), warning ("expected a non-null text source.");

    if (!sz)
        sz = strlen (src);

    if (sz >= MAX_PACKET_SZ) {
        warning ("only the first " stringify (MAX_PACKET_SZ) " bytes of the source will be copied.");
        sz = MAX_PACKET_SZ;
    }

    dest->cmd           = cmd_packet + cont;
    dest->info.pack.len = (uint16_t) sz;
    memccpy (dest->info.pack.text, src, '\0', sz);
}