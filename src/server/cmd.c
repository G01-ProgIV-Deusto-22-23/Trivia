cmd_t success_command (void) {
    return (cmd_t) {};
}

cmd_t error_command (const uint32_t error) {
    return (cmd_t) { .cmd = cmd_error, .info.arg [0] = error };
}

cmd_t kill_command (void) {
    return (cmd_t) { .cmd = cmd_kill };
}

cmd_t game_command (const game_attr_t game) {
    return (cmd_t) { .cmd = cmd_game_create, .info.game = game };
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
    memcpy (dest->info.pack.text, src, sz);
}