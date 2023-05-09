#ifndef TRIVIA_OS_BACKTRACE_H
#define TRIVIA_OS_BACKTRACE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

    extern size_t get_backtrace_max (void);
    extern int    get_backtrace_fd (void);
    extern void   print_backtrace (void);
    extern void   init_backtrace (void);
    extern void   end_backtrace (void);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wundef"
#pragma GCC diagnostic ignored "-Wattributes"

    __attribute__ ((constructor (101))) static void init_backtrace_constructor (void) {
        init_backtrace ();
    }

    __attribute__ ((destructor (101))) static void delete_backtrace_destructor (void) {
        end_backtrace ();
    }

#pragma GCC diagnostic pop

#ifdef __cplusplus
}
#endif

#endif
