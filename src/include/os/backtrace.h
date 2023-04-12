#ifndef TRIVIA_OS_BACKTRACE_H
#define TRIVIA_OS_BACKTRACE_H

#include <stddef.h>

extern size_t get_backtrace_max (void);
extern int    get_backtrace_fd (void);
extern void   print_backtrace (void);
extern void   init_backtrace (void);
extern void   end_backtrace (void);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wundef"
#pragma GCC diagnostic ignored "-Wattributes"

#if defined(__cpp_attributes) || __STDC_VERSION__ >= 201710L
[[constructor (101)]]
#else
__attribute__ ((constructor (101)))
#endif
static void
    init_backtrace_constructor (void) {
    init_backtrace ();
}

#if defined(__cpp_attributes) || __STDC_VERSION__ >= 201710L
[[destructor (101)]]
#else
__attribute__ ((destructor (101)))
#endif
static void
    delete_backtrace_destructor (void) {
    end_backtrace ();
}

#pragma GCC diagnostic pop

#endif
