#ifndef TRIVIA_ARRSIZE_H
#define TRIVIA_ARRSIZE_H

#ifdef __cplusplus

template <typename T, size_t N2, size_t N3, size_t N4, size_t N5>
static constexpr size_t ARRSIZE_HELPER_FUNC (T arr [][N2][N3][N4][N5], size_t N1, int n) {
    return n == 1 ? N1 : n == 2 ? N1 * N2 : n == 3 ? N1 * N2 * N3 : n == 4 ? N1 * N2 * N3 * N4 : N1 * N2 * N3 * N4 * N5;
}

template <typename T, size_t N2, size_t N3, size_t N4>
static constexpr size_t ARRSIZE_HELPER_FUNC (T arr [][N2][N3][N4], size_t N1, int n) {
    return n == 1 ? N1 : n == 2 ? N1 * N2 : n == 3 ? N1 * N2 * N3 : N1 * N2 * N3 * N4;
}

template <typename T, size_t N2, size_t N3>
static constexpr size_t ARRSIZE_HELPER_FUNC (T arr [][N2][N3], size_t N1, int n) {
    return n == 1 ? N1 : n == 2 ? N1 * N2 : N1 * N2 * N3;
}

template <typename T, size_t N2> static constexpr size_t ARRSIZE_HELPER_FUNC (T arr [][N2], size_t N1, int n) {
    return n == 1 ? N1 : N1 * N2;
}

template <typename T> static constexpr size_t ARRSIZE_HELPER_FUNC (T arr [], size_t N1, int n) {
    return N1;
}

template <typename T> static constexpr size_t ARRSIZE_HELPER_FUNC (T, size_t N1, int n) {
    return 0;
}

    #define ARRSIZE_HELPER(x, n) ARRSIZE_HELPER_FUNC (x, std::extent<decltype (x)>::value, n)

    #define arrsize(x, ...)                                                                                            \
        (ct_error (                                                                                                    \
             !isint (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 1)),                                                            \
             "the second argument passed to the arrsize() macro must be an integer between 1 and 5."                   \
         ),                                                                                                            \
         ct_error (                                                                                                    \
             !__builtin_constant_p (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 1)),                                             \
             "the second argument passed to the arrsize() macro must be known at compile-time."                        \
         ),                                                                                                            \
         ct_error (                                                                                                    \
             ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 1) < 1 || ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 1) > 5,                     \
             "the second argument passed to the arrsize() macro must be an integer between 1 and 5."                   \
         ),                                                                                                            \
         ct_error (                                                                                                    \
             !isarr (x, 1) && ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 1) >= 1,                                               \
             "the object passed to the arrsize() macro is not an array."                                               \
         ),                                                                                                            \
         ct_error (                                                                                                    \
             !isarr (x, 2) && ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 1) >= 2,                                               \
             "the object passed to the arrsize() macro is not an array of arrays."                                     \
         ),                                                                                                            \
         ct_error (                                                                                                    \
             !isarr (x, 3) && ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 1) >= 3,                                               \
             "the object passed to the arrsize() macro is not an array of arrays of arrays."                           \
         ),                                                                                                            \
         ct_error (                                                                                                    \
             !isarr (x, 4) && ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 1) >= 4,                                               \
             "the object passed to the arrsize() macro is not an array of arrays of arrays of arrays."                 \
         ),                                                                                                            \
         ct_error (                                                                                                    \
             !isarr (x, 5) && ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 1) == 5,                                               \
             "the object passed to the arrsize() macro is not an array of arrays of arrays of arrays of arrays."       \
         ),                                                                                                            \
         ARRSIZE_HELPER (x, ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 1)))

#else
    #ifdef _WIN32
        #define arrsize(x) (sizeof (x) / sizeof (*x))
    #else
        #define arrsize(x, ...)                                                                                        \
            (ct_error (                                                                                                \
                 !isint (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 1)),                                                        \
                 "the second argument passed to the arrsize() macro must be an integer between 1 and 5."               \
             ),                                                                                                        \
             ct_error (                                                                                                \
                 !__builtin_constant_p (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 1)),                                         \
                 "the second argument passed to the arrsize() macro must be known at compile-time."                    \
             ),                                                                                                        \
             ct_error (                                                                                                \
                 __builtin_choose_expr (                                                                               \
                     __builtin_constant_p (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 1)) &&                                    \
                         isint (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 1)),                                                 \
                     ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 1), 0                                                           \
                 ) < 1 ||                                                                                              \
                     __builtin_choose_expr (                                                                           \
                         __builtin_constant_p (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 1)) &&                                \
                             isint (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 1)),                                             \
                         ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 1), 0                                                       \
                     ) > 5,                                                                                            \
                 "the second argument passed to the arrsize() macro must be an integer between 1 and 5."               \
             ),                                                                                                        \
             ct_error (                                                                                                \
                 !isarr_eval (x, 1) && __builtin_choose_expr (                                                         \
                                           __builtin_constant_p (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 1)) &&              \
                                               isint (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 1)),                           \
                                           ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 1), 0                                     \
                                       ) >= 1,                                                                         \
                 "the object passed to the arrsize() macro is not an array."                                           \
             ),                                                                                                        \
             ct_error (                                                                                                \
                 !isarr_eval (x, 2) && __builtin_choose_expr (                                                         \
                                           __builtin_constant_p (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 1)) &&              \
                                               isint (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 1)),                           \
                                           ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 1), 0                                     \
                                       ) >= 2,                                                                         \
                 "the object passed to the arrsize() macro is not an array of arrays."                                 \
             ),                                                                                                        \
             ct_error (                                                                                                \
                 !isarr_eval (x, 3) && __builtin_choose_expr (                                                         \
                                           __builtin_constant_p (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 1)) &&              \
                                               isint (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 1)),                           \
                                           ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 1), 0                                     \
                                       ) >= 3,                                                                         \
                 "the object passed to the arrsize() macro is not an array of arrays of arrays."                       \
             ),                                                                                                        \
             ct_error (                                                                                                \
                 !isarr_eval (x, 4) && __builtin_choose_expr (                                                         \
                                           __builtin_constant_p (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 1)) &&              \
                                               isint (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 1)),                           \
                                           ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 1), 0                                     \
                                       ) >= 4,                                                                         \
                 "the object passed to the arrsize() macro is not an array of arrays of arrays of arrays."             \
             ),                                                                                                        \
             ct_error (                                                                                                \
                 !isarr_eval (x, 5) && __builtin_choose_expr (                                                         \
                                           __builtin_constant_p (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 1)) &&              \
                                               isint (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 1)),                           \
                                           ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 1), 0                                     \
                                       ) == 5,                                                                         \
                 "the object passed to the arrsize() macro is not an array of arrays of arrays of arrays of arrays."   \
             ),                                                                                                        \
             __builtin_choose_expr (                                                                                   \
                 ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 1) == 1, sizeof (x) / (sizeof (*x)),                                \
                 __builtin_choose_expr (                                                                               \
                     ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 1) == 2,                                                        \
                     sizeof (x) / (sizeof (*__builtin_choose_expr (                                                    \
                                      __builtin_classify_type (*x) == pointer_type_class, *x, ((char [1]) {})          \
                                  ))),                                                                                 \
                     __builtin_choose_expr (                                                                           \
                         ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 1) == 3,                                                    \
                         sizeof (x) / (sizeof (*__builtin_choose_expr (                                                \
                                          __builtin_classify_type (*__builtin_choose_expr (                            \
                                              __builtin_classify_type (*x) == pointer_type_class, *x, ((char [1]) {})  \
                                          )) == pointer_type_class,                                                    \
                                          *__builtin_choose_expr (                                                     \
                                              __builtin_classify_type (*x) == pointer_type_class, *x, ((char [1]) {})  \
                                          ),                                                                           \
                                          ((char [1]) {})                                                              \
                                      ))),                                                                             \
                         __builtin_choose_expr (                                                                       \
                             ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 1) == 4,                                                \
                             sizeof (x) /                                                                              \
                                 (sizeof (*__builtin_choose_expr (                                                     \
                                     __builtin_classify_type (*__builtin_choose_expr (                                 \
                                         __builtin_classify_type (*__builtin_choose_expr (                             \
                                             __builtin_classify_type (*x) == pointer_type_class, *x, ((char [1]) {})   \
                                         )) == pointer_type_class,                                                     \
                                         *__builtin_choose_expr (                                                      \
                                             __builtin_classify_type (*x) == pointer_type_class, *x, ((char [1]) {})   \
                                         ),                                                                            \
                                         ((char [1]) {})                                                               \
                                     )) == pointer_type_class,                                                         \
                                     *__builtin_choose_expr (                                                          \
                                         __builtin_classify_type (*__builtin_choose_expr (                             \
                                             __builtin_classify_type (*x) == pointer_type_class, *x, ((char [1]) {})   \
                                         )) == pointer_type_class,                                                     \
                                         *__builtin_choose_expr (                                                      \
                                             __builtin_classify_type (*x) == pointer_type_class, *x, ((char [1]) {})   \
                                         ),                                                                            \
                                         ((char [1]) {})                                                               \
                                     ),                                                                                \
                                     ((char [1]) {})                                                                   \
                                 ))),                                                                                  \
                             sizeof (x) / (sizeof (*__builtin_choose_expr (                                            \
                                              __builtin_classify_type (*__builtin_choose_expr (                        \
                                                  __builtin_classify_type (*__builtin_choose_expr (                    \
                                                      __builtin_classify_type (*__builtin_choose_expr (                \
                                                          __builtin_classify_type (*x) == pointer_type_class, *x,      \
                                                          ((char [1]) {})                                              \
                                                      )) == pointer_type_class,                                        \
                                                      *__builtin_choose_expr (                                         \
                                                          __builtin_classify_type (*x) == pointer_type_class, *x,      \
                                                          ((char [1]) {})                                              \
                                                      ),                                                               \
                                                      ((char [1]) {})                                                  \
                                                  )) == pointer_type_class,                                            \
                                                  *__builtin_choose_expr (                                             \
                                                      __builtin_classify_type (*__builtin_choose_expr (                \
                                                          __builtin_classify_type (*x) == pointer_type_class, *x,      \
                                                          ((char [1]) {})                                              \
                                                      )) == pointer_type_class,                                        \
                                                      *__builtin_choose_expr (                                         \
                                                          __builtin_classify_type (*x) == pointer_type_class, *x,      \
                                                          ((char [1]) {})                                              \
                                                      ),                                                               \
                                                      ((char [1]) {})                                                  \
                                                  ),                                                                   \
                                                  ((char [1]) {})                                                      \
                                              )) == pointer_type_class,                                                \
                                              *__builtin_choose_expr (                                                 \
                                                  __builtin_classify_type (*__builtin_choose_expr (                    \
                                                      __builtin_classify_type (*__builtin_choose_expr (                \
                                                          __builtin_classify_type (*x) == pointer_type_class, *x,      \
                                                          ((char [1]) {})                                              \
                                                      )) == pointer_type_class,                                        \
                                                      *__builtin_choose_expr (                                         \
                                                          __builtin_classify_type (*x) == pointer_type_class, *x,      \
                                                          ((char [1]) {})                                              \
                                                      ),                                                               \
                                                      ((char [1]) {})                                                  \
                                                  )) == pointer_type_class,                                            \
                                                  *__builtin_choose_expr (                                             \
                                                      __builtin_classify_type (*__builtin_choose_expr (                \
                                                          __builtin_classify_type (*x) == pointer_type_class, *x,      \
                                                          ((char [1]) {})                                              \
                                                      )) == pointer_type_class,                                        \
                                                      *__builtin_choose_expr (                                         \
                                                          __builtin_classify_type (*x) == pointer_type_class, *x,      \
                                                          ((char [1]) {})                                              \
                                                      ),                                                               \
                                                      ((char [1]) {})                                                  \
                                                  ),                                                                   \
                                                  ((char [1]) {})                                                      \
                                              ),                                                                       \
                                              ((char [1]) {})                                                          \
                                          )))                                                                          \
                         )                                                                                             \
                     )                                                                                                 \
                 )                                                                                                     \
             ))
    #endif

#endif

#endif
