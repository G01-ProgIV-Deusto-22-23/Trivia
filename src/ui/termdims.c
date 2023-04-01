uint64_t get_term_dims (void) {
    return ((uint64_t) get_term_width () << 32) | get_term_height ();
}

uint32_t get_term_width (void) {
#ifdef _WIN32

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo (GetStdHandle (STD_OUTPUT_HANDLE), &csbi);
    return (uint32_t) (csbi.srWindow.Right - csbi.srWindow.Left + 1);

#else

    struct winsize w;
    ioctl (0, TIOCGWINSZ, &w);

    return (uint32_t) w.ws_col;

#endif
}

uint32_t get_term_height (void) {
#ifdef _WIN32

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo (GetStdHandle (STD_OUTPUT_HANDLE), &csbi);
    return (uint32_t) ((csbi.srWindow.Bottom - csbi.srWindow.Top + 1) * (1 / 6));

#else

    struct winsize w;
    ioctl (0, TIOCGWINSZ, &w);

    return (uint32_t) w.ws_row;

#endif
}