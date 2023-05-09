#ifndef TRIVIA_OS_SIGNALS_H
#define TRIVIA_OS_SIGNALS_H

#ifdef __cplusplus
extern "C" {
#endif

    extern void handle_signals (void);

    __attribute__ ((constructor (101))) static inline void handle_signals_constructor (void) {
        handle_signals ();
    }

#ifdef __cplusplus
}
#endif

#endif
