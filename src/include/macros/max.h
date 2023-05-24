#ifndef TRIVIA_MACROS_MAX_H
#define TRIVIA_MACROS_MAX_H

#ifdef __cplusplus
    #define max(x, y) std::max (x, y)
#else
    #define ct_max(x, y)                                                                                               \
        __builtin_choose_expr (                                                                                        \
            __builtin_choose_expr (__builtin_constant_p (x), x, 0) >                                                   \
                __builtin_choose_expr (__builtin_constant_p (x), x, 0),                                                \
            x, y                                                                                                       \
        )

    #define max(x, y)                                                                                                  \
        __builtin_choose_expr (__builtin_constant_p (x) && __builtin_constant_p (y), ct_max (x, y), ({                 \
                                   _Pragma ("GCC diagnostic push");                                                    \
                                   _Pragma ("GCC diagnostic ignored \"-Wshadow=local\"");                              \
                                   _Pragma ("GCC diagnostic ignored \"-Wshadow=compatible-local\"");                   \
                                   auto __max_temp_x__ = (__extension__ x);                                            \
                                   auto __max_temp_y__ = (__extension__ y);                                            \
                                   _Pragma ("GCC diagnostic pop");                                                     \
                                   __max_temp_x__ > __max_temp_y__ ? __max_temp_x__ : __max_temp_y__;                  \
                               }))
#endif

#endif
