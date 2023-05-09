cmd_t success_command (void) {
    return (cmd_t) {};
}

cmd_t error_command (const uint32_t error) {
    return (cmd_t) { .cmd = cmd_error, .info.arg [0] = error };
}

cmd_t kill_command (void) {
    return (cmd_t) { .cmd = cmd_kill };
}

void packet (cmd_t *const restrict dest, const char src [static 1], size_t sz) {
    if (!dest)
        return;

    if (!src)
        return memset (dest, 0, sizeof (cmd_t)), warning ("expected a non-null text source.");

    if (!sz)
        sz = strlen (src);

    if (sz >= MAX_PACKET_SZ) {
        warning ("only the first " STRINGIFY (MAX_PACKET_SZ) " bytes of the source will be copied.");
        sz = MAX_PACKET_SZ;
    }

    dest->cmd           = cmd_packet;
    dest->info.pack.len = (uint16_t) sz;
    memcpy (dest->info.pack.text, src, sz);
}