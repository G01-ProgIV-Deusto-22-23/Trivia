#ifndef TRIVIA_MACROS_ISARR_H
#define TRIVIA_MACROS_ISARR_H

#ifdef __cplusplus
    #define isarr(x, ...)                                                                                              \
        (ct_error (                                                                                                    \
             !isint (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 1)),                                                            \
             "the second argument passed to the isarr() macro must be an integer between 1 and 5."                     \
         ),                                                                                                            \
         ct_error (                                                                                                    \
             !__builtin_constant_p (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 1)),                                             \
             "the second argument passed to the isarr() macro must be known at compile-time."                          \
         ),                                                                                                            \
         ct_error (                                                                                                    \
             ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 1) < 1 || ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 1) > 5,                     \
             "the second argument passed to the isarr() macro must be an integer between 1 and 5."                     \
         ),                                                                                                            \
         std::rank<decltype (x)>::value >= ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 1))
#else
    #ifdef _WIN32
        #define isarr_helper(x)                                                                                        \
            ({                                                                                                         \
                size_t __is_arr_var__ = __builtin_choose_expr (                                                        \
                    __builtin_classify_type (x) == pointer_type_class,                                                 \
                    __builtin_types_compatible_p (                                                                     \
                        typeof (*(__builtin_choose_expr (                                                              \
                            __builtin_classify_type (x) == pointer_type_class &&                                       \
                                !__builtin_types_compatible_p (typeof (x), void *),                                    \
                            x, ((char []) {})                                                                          \
                        ))) [],                                                                                        \
                        typeof (x)                                                                                     \
                    ),                                                                                                 \
                    0                                                                                                  \
                );                                                                                                     \
                __is_arr_var__;                                                                                        \
            })
    #else
        #define isarr_helper(x)                                                                                        \
            __builtin_choose_expr (                                                                                    \
                __builtin_classify_type (x) == pointer_type_class,                                                     \
                __builtin_types_compatible_p (                                                                         \
                    typeof (*(__builtin_choose_expr (                                                                  \
                        __builtin_classify_type (x) == pointer_type_class &&                                           \
                            !__builtin_types_compatible_p (typeof (x), void *),                                        \
                        x, ((char []) {})                                                                              \
                    ))) [],                                                                                            \
                    typeof (x)                                                                                         \
                ),                                                                                                     \
                0                                                                                                      \
            )
    #endif

    #define isarr_eval(x, ...)                                                                                         \
        __builtin_choose_expr (                                                                                        \
            __builtin_choose_expr (                                                                                    \
                isint (__builtin_choose_expr (                                                                         \
                    __builtin_constant_p (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 1)), ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 1), \
                    1                                                                                                  \
                )),                                                                                                    \
                ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 1), 1                                                                \
            ) == 1,                                                                                                    \
            /* Level 1 */ isarr_helper (x),                                                                            \
            __builtin_choose_expr (                                                                                    \
                __builtin_choose_expr (                                                                                \
                    isint (__builtin_choose_expr (                                                                     \
                        __builtin_constant_p (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 1)),                                   \
                        ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 1), 1                                                        \
                    )),                                                                                                \
                    ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 1), 1                                                            \
                ) == 2,                                                                                                \
                /* Level 2 */ isarr_helper (x) &&                                                                      \
                    isarr_helper (                                                                                     \
                        *__builtin_choose_expr (__builtin_classify_type (x) == pointer_type_class, x, ((char []) {}))  \
                    ),                                                                                                 \
                __builtin_choose_expr (                                                                                \
                    __builtin_choose_expr (                                                                            \
                        isint (__builtin_choose_expr (                                                                 \
                            __builtin_constant_p (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 1)),                               \
                            ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 1), 1                                                    \
                        )),                                                                                            \
                        ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 1), 1                                                        \
                    ) == 3,                                                                                            \
                    /* Level 3 */ isarr_helper (x) &&                                                                  \
                        isarr_helper (*__builtin_choose_expr (                                                         \
                            __builtin_classify_type (x) == pointer_type_class, x, ((char []) {})                       \
                        )) &&                                                                                          \
                        isarr_helper (*__builtin_choose_expr (                                                         \
                            __builtin_classify_type (*__builtin_choose_expr (                                          \
                                __builtin_classify_type (x) == pointer_type_class, x, ((char []) {})                   \
                            )) == pointer_type_class,                                                                  \
                            *__builtin_choose_expr (                                                                   \
                                __builtin_classify_type (x) == pointer_type_class, x, ((char []) {})                   \
                            ),                                                                                         \
                            ((char []) {})                                                                             \
                        )),                                                                                            \
                    __builtin_choose_expr (                                                                            \
                        __builtin_choose_expr (                                                                        \
                            isint (__builtin_choose_expr (                                                             \
                                __builtin_constant_p (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 1)),                           \
                                ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 1), 1                                                \
                            )),                                                                                        \
                            ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 1), 1                                                    \
                        ) == 4,                                                                                        \
                        /* Level 4 */ isarr_helper (x) &&                                                              \
                            isarr_helper (*__builtin_choose_expr (                                                     \
                                __builtin_classify_type (x) == pointer_type_class, x, ((char []) {})                   \
                            )) &&                                                                                      \
                            isarr_helper (*__builtin_choose_expr (                                                     \
                                __builtin_classify_type (*__builtin_choose_expr (                                      \
                                    __builtin_classify_type (x) == pointer_type_class, x, ((char []) {})               \
                                )) == pointer_type_class,                                                              \
                                *__builtin_choose_expr (                                                               \
                                    __builtin_classify_type (x) == pointer_type_class, x, ((char []) {})               \
                                ),                                                                                     \
                                ((char []) {})                                                                         \
                            )) &&                                                                                      \
                            isarr_helper (*__builtin_choose_expr (                                                     \
                                __builtin_classify_type (*__builtin_choose_expr (                                      \
                                    __builtin_classify_type (*__builtin_choose_expr (                                  \
                                        __builtin_classify_type (x) == pointer_type_class, x, ((char []) {})           \
                                    )) == pointer_type_class,                                                          \
                                    *__builtin_choose_expr (                                                           \
                                        __builtin_classify_type (x) == pointer_type_class, x, ((char []) {})           \
                                    ),                                                                                 \
                                    ((char []) {})                                                                     \
                                )) == pointer_type_class,                                                              \
                                *__builtin_choose_expr (                                                               \
                                    __builtin_classify_type (*__builtin_choose_expr (                                  \
                                        __builtin_classify_type (x) == pointer_type_class, x, ((char []) {})           \
                                    )) == pointer_type_class,                                                          \
                                    *__builtin_choose_expr (                                                           \
                                        __builtin_classify_type (x) == pointer_type_class, x, ((char []) {})           \
                                    ),                                                                                 \
                                    ((char []) {})                                                                     \
                                ),                                                                                     \
                                ((char []) {})                                                                         \
                            )),                                                                                        \
                        /* Level 5 */ isarr_helper (x) &&                                                              \
                            isarr_helper (*__builtin_choose_expr (                                                     \
                                __builtin_classify_type (x) == pointer_type_class, x, ((char []) {})                   \
                            )) &&                                                                                      \
                            isarr_helper (*__builtin_choose_expr (                                                     \
                                __builtin_classify_type (*__builtin_choose_expr (                                      \
                                    __builtin_classify_type (x) == pointer_type_class, x, ((char []) {})               \
                                )) == pointer_type_class,                                                              \
                                *__builtin_choose_expr (                                                               \
                                    __builtin_classify_type (x) == pointer_type_class, x, ((char []) {})               \
                                ),                                                                                     \
                                ((char []) {})                                                                         \
                            )) &&                                                                                      \
                            isarr_helper (*__builtin_choose_expr (                                                     \
                                __builtin_classify_type (*__builtin_choose_expr (                                      \
                                    __builtin_classify_type (*__builtin_choose_expr (                                  \
                                        __builtin_classify_type (x) == pointer_type_class, x, ((char []) {})           \
                                    )) == pointer_type_class,                                                          \
                                    *__builtin_choose_expr (                                                           \
                                        __builtin_classify_type (x) == pointer_type_class, x, ((char []) {})           \
                                    ),                                                                                 \
                                    ((char []) {})                                                                     \
                                )) == pointer_type_class,                                                              \
                                *__builtin_choose_expr (                                                               \
                                    __builtin_classify_type (*__builtin_choose_expr (                                  \
                                        __builtin_classify_type (x) == pointer_type_class, x, ((char []) {})           \
                                    )) == pointer_type_class,                                                          \
                                    *__builtin_choose_expr (                                                           \
                                        __builtin_classify_type (x) == pointer_type_class, x, ((char []) {})           \
                                    ),                                                                                 \
                                    ((char []) {})                                                                     \
                                ),                                                                                     \
                                ((char []) {})                                                                         \
                            )) &&                                                                                      \
                            isarr_helper (*__builtin_choose_expr (                                                     \
                                __builtin_classify_type (*__builtin_choose_expr (                                      \
                                    __builtin_classify_type (*__builtin_choose_expr (                                  \
                                        __builtin_classify_type (*__builtin_choose_expr (                              \
                                            __builtin_classify_type (x) == pointer_type_class, x, ((char []) {})       \
                                        )) == pointer_type_class,                                                      \
                                        *__builtin_choose_expr (                                                       \
                                            __builtin_classify_type (x) == pointer_type_class, x, ((char []) {})       \
                                        ),                                                                             \
                                        ((char []) {})                                                                 \
                                    )) == pointer_type_class,                                                          \
                                    *__builtin_choose_expr (                                                           \
                                        __builtin_classify_type (*__builtin_choose_expr (                              \
                                            __builtin_classify_type (x) == pointer_type_class, x, ((char []) {})       \
                                        )) == pointer_type_class,                                                      \
                                        *__builtin_choose_expr (                                                       \
                                            __builtin_classify_type (x) == pointer_type_class, x, ((char []) {})       \
                                        ),                                                                             \
                                        ((char []) {})                                                                 \
                                    ),                                                                                 \
                                    ((char []) {})                                                                     \
                                )) == pointer_type_class,                                                              \
                                *__builtin_choose_expr (                                                               \
                                    __builtin_classify_type (*__builtin_choose_expr (                                  \
                                        __builtin_classify_type (*__builtin_choose_expr (                              \
                                            __builtin_classify_type (x) == pointer_type_class, x, ((char []) {})       \
                                        )) == pointer_type_class,                                                      \
                                        *__builtin_choose_expr (                                                       \
                                            __builtin_classify_type (x) == pointer_type_class, x, ((char []) {})       \
                                        ),                                                                             \
                                        ((char []) {})                                                                 \
                                    )) == pointer_type_class,                                                          \
                                    *__builtin_choose_expr (                                                           \
                                        __builtin_classify_type (*__builtin_choose_expr (                              \
                                            __builtin_classify_type (x) == pointer_type_class, x, ((char []) {})       \
                                        )) == pointer_type_class,                                                      \
                                        *__builtin_choose_expr (                                                       \
                                            __builtin_classify_type (x) == pointer_type_class, x, ((char []) {})       \
                                        ),                                                                             \
                                        ((char []) {})                                                                 \
                                    ),                                                                                 \
                                    ((char []) {})                                                                     \
                                ),                                                                                     \
                                ((char []) {})                                                                         \
                            ))                                                                                         \
                    )                                                                                                  \
                )                                                                                                      \
            )                                                                                                          \
        )

    #define isarr(x, ...)                                                                                              \
        (ct_error (                                                                                                    \
             !isint (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 1)),                                                            \
             "the second argument passed to the isarr() macro must be an integer between 1 and 5."                     \
         ),                                                                                                            \
         ct_error (                                                                                                    \
             !__builtin_constant_p (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 1)),                                             \
             "the second argument passed to the isarr() macro must be known at compile-time."                          \
         ),                                                                                                            \
         ct_error (                                                                                                    \
             __builtin_choose_expr (                                                                                   \
                 __builtin_constant_p (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 1)) &&                                        \
                     isint (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 1)),                                                     \
                 ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 1), 0                                                               \
             ) < 1 ||                                                                                                  \
                 __builtin_choose_expr (                                                                               \
                     __builtin_constant_p (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 1)) &&                                    \
                         isint (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 1)),                                                 \
                     ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 1), 0                                                           \
                 ) > 5,                                                                                                \
             "the second argument passed to the isarr() macro must be an integer between 1 and 5."                     \
         ),                                                                                                            \
         __builtin_choose_expr (                                                                                       \
             ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 1) == 1, isarr_helper (x), isarr_eval (x __VA_OPT__ (, ) __VA_ARGS__)   \
         ))
#endif

#endif