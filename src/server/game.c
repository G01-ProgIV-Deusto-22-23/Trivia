static size_t NGAMES = DEFAULT_GAMES;

size_t get_games (void) {
    return NGAMES;
}

size_t set_games (size_t n) {
    return NGAMES = n ? min (MAX_GAMES, n) : DEFAULT_GAMES;
}