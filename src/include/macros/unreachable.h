#ifndef TRIVIA_MACROS_UNREACHABLE_H
#define TRIVIA_MACROS_UNREACHABLE_H

#if __GNUC__ * 1000 < 13 || !(defined(unreachable) || defined(DISABLE_UNREACHABLE_OVERRIDE))
    #define unreachable() __builtin_unreachable ()
#endif

#endif
