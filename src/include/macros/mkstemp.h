#ifndef TRIVIA_MKSTEMP_H
#define TRIVIA_MKSTEMP_H

#ifdef _WIN32
    #define mkstemp(template_name)                                                                                     \
        ({                                                                                                             \
            _Pragma ("GCC diagnostic push");                                                                           \
            _Pragma ("GCC diagnostic ignored \"-Wshadow\"");                                                           \
            _Pragma ("GCC diagnostic ignored \"-Wimplicit-function-declaration\"");                                    \
            struct timeval __mkstemp_t__;                                                                              \
            gettimeofday (&__mkstemp_t__, NULL);                                                                       \
            _Pragma ("GCC diagnostic pop");                                                                            \
            srand ((unsigned long) __mkstemp_t__.tv_usec *(unsigned long) __mkstemp_t__.tv_sec *GetCurrentProcessId () \
            );                                                                                                         \
            mkstemp (template_name);                                                                                   \
        })
#endif

#endif