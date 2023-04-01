#ifndef TRIVIA_MACROS_TRAP_H
#define TRIVIA_MACROS_TRAP_H

#ifndef __builtin_trap
    #define __builtin_trap()      abort ()
    #define __builtin_debugtrap() abort ()
#endif

#if !defined(NDEBUG) && defined(DEBUG)
    #define trap() __builtin_debugtrap ()
#else
    #define trap() __builtin_trap ()
#endif

#endif