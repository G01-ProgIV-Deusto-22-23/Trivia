#ifndef TRIVIA_OS_SIGNALS_H
#define TRIVIA_OS_SIGNALS_H

extern void handle_signals (void);

#if defined(__cplusplus) || __STDC_VERSION__ > 201710L
[[constructor (101)]]
#else
__attribute__ ((constructor (101)))
#endif
static inline void
    handle_signals_constructor (void) {
    handle_signals ();
}

#endif
