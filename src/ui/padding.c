volatile uint64_t TRIVIA_PADDING_VAR = u64comp (DEFAULT_PADDING, DEFAULT_PADDING);

uint64_t get_padding (void) {
    return TRIVIA_PADDING_VAR;
}

uint64_t impl_set_padding (uint32_t w, uint32_t h) {
    return TRIVIA_PADDING_VAR = u64comp (w, h);
}