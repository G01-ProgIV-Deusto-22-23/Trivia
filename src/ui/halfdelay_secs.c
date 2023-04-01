volatile int HALFDELAY_SECS_VAR;

int get_halfdelay_secs (void) {
    return HALFDELAY_SECS_VAR;
}

int set_halfdelay_secs (const int secs) {
    return HALFDELAY_SECS_VAR = DEFAULT_HALFDELAY_SECS * (secs < 1 || secs > 255) + secs * !(secs < 1 || secs > 255);
}
