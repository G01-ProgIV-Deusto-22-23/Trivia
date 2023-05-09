#ifndef TRIVIA_MACROS_AUTO_H
#define TRIVIA_MACROS_AUTO_H

#if !defined(__cplusplus) && !defined(DISABLE_AUTO_OVERRIDE) && !(__STDC__ > 201710L && __GNUC__ >= 13)
    #define auto __auto_type
#endif

#endif
