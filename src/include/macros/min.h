#ifndef TRIVIA_MACROS_MIN_H
#define TRIVIA_MACROS_MIN_H

#ifdef __cplusplus
    #define min(x, y) std::min (x, y)
#else
    #define ct_min(x, y)                                                                                               \
        __builtin_choose_expr (                                                                                        \
            __builtin_choose_expr (__builtin_constant_p (x), x, 0) <                                                   \
                __builtin_choose_expr (__builtin_constant_p (x), x, 0),                                                \
            x, y                                                                                                       \
        )

    #define min(x, y)                                                                                                  \
        __builtin_choose_expr (__builtin_constant_p (x) && __builtin_constant_p (y), ct_min (x, y), ({                 \
                                   _Pragma ("GCC diagnostic push");                                                    \
                                   _Pragma ("GCC diagnostic ignored \"-Wshadow=local\"");                              \
                                   _Pragma ("GCC diagnostic ignored \"-Wshadow=compatible-local\"");                   \
                                   auto __min_temp_x__ = (__extension__ x);                                            \
                                   auto __min_temp_y__ = (__extension__ y);                                            \
                                   _Pragma ("GCC diagnostic pop");                                                     \
                                   __min_temp_x__ < __min_temp_y__ ? __min_temp_x__ : __min_temp_y__;                  \
                               }))
#endif

#endif
