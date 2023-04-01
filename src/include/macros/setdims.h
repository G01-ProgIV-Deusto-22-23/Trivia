#ifndef TRIVIA_MACROS_SETDIMS_H
#define TRIVIA_MACROS_SETDIMS_H

#ifdef __cplusplus

template <setdimsfunc_t *F, uint64_t D> static constexpr uint64_t SETDIMS_FUNC (setdimsfunc_t *F, uint64_t D) {
    return f (u64decomp (D));
}

template <setdimsfunc_t *F, uint32_t W, uint32_t H>
static constexpr uint64_t SETDIMS_FUNC (setdimsfunc_t *F, uint32_t W, uint32_t H) {
    return F (W, H);
}

    #define setdims(f, dw, dh, ...)                                                                                    \
        (ct_error (NARGS (__VA_ARGS__) > 2, "the setdims() macro must be passed between 3 and 5 arguments."),          \
         ct_error (                                                                                                    \
             !(std::is_same<decltype (f), setdimsfunc_t>::value || std::is_same<decltype (f), setdimsfunc_t *>::value  \
             ),                                                                                                        \
             "the first argument passed to the setdims() macro must a setdimsfunc_t function (pointer)."               \
         ),                                                                                                            \
         ct_error (                                                                                                    \
             !(isint (dw) && isint (dh) && isint (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 0)) &&                             \
               isint (ARG2 (__VA_ARGS__ __VA_OPT__ (, ) 0, 0))),                                                       \
             "the second, third, fourth and fifth arguments passed to the setdims() macro must be of integral type."   \
         ),                                                                                                            \
         !NARGS (__VA_ARGS__)       ? SETDIMS_FUNC (f, dw, dh)                                                         \
         : NARGS (__VA_ARGS__) == 1 ? SETDIMS_FUNC (f, ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 0))                           \
                                    : SETDIMS_FUNC (f __VA_OPT__ (, ) __VA_ARGS__));

    #define setwidth(f, dw, dh, ...)                                                                                   \
        (ct_error (NARGS (__VA_ARGS__) > 1, "the setwidth() macro accepts either three or four arguments."),           \
         ct_error (                                                                                                    \
             !(std::is_same<decltype (f), setdimsfunc_t>::value || std::is_same<decltype (f), setdimsfunc_t *>::value  \
             ),                                                                                                        \
             "the first argument passed to the setwidth() macro must a setdimsfunc_t function (pointer)."              \
         ),                                                                                                            \
         ct_error (                                                                                                    \
             !(isint (dw) && isint (dh) && isint (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 0))),                              \
             "the second, third and fourth arguments passed to the setwidth() macro must be of integral type."         \
         ),                                                                                                            \
         setdims (f, dw, dh, ARG1 (__VA_ARGS__ __VA_OPT__ (, ) dw), dh))

    #define setheight(f, dw, dh, ...)                                                                                  \
        (ct_error (NARGS (__VA_ARGS__) > 1, "the setheight() macro accepts either three or four arguments."),          \
         ct_error (                                                                                                    \
             !(std::is_same<decltype (f), setdimsfunc_t>::value || std::is_same<decltype (f), setdimsfunc_t *>::value  \
             ),                                                                                                        \
             "the first argument passed to the setheight() macro must a setdimsfunc_t function (pointer)."             \
         ),                                                                                                            \
         ct_error (                                                                                                    \
             !(isint (dw) && isint (dh) && isint (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 0))),                              \
             "the second, third and fourth arguments passed to the setheight() macro must be of integral type."        \
         ),                                                                                                            \
         setdims (f, dw, dh, dw, ARG1 (__VA_ARGS__ __VA_OPT__ (, ) dh)))

#else

    #define setdims(f, dw, dh, ...)                                                                                    \
        (ct_error (NARGS (__VA_ARGS__) > 2, "the setdims() macro must be passed between 3 and 5 arguments."),          \
         ct_error (                                                                                                    \
             !(__builtin_types_compatible_p (typeof (f), setdimsfunc_t) ||                                             \
               __builtin_types_compatible_p (typeof (f), setdimsfunc_t *)),                                            \
             "the first argument passed to the setdims() macro must a setdimsfunc_t function (pointer)."               \
         ),                                                                                                            \
         ct_error (                                                                                                    \
             !(isint (dw) && isint (dh) && isint (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 0)) &&                             \
               isint (ARG2 (__VA_ARGS__ __VA_OPT__ (, ) 0, 0))),                                                       \
             "the second, third, fourth and fifth arguments passed to the setdims() macro must be of integral type."   \
         ),                                                                                                            \
         __builtin_choose_expr (                                                                                       \
             !NARGS (__VA_ARGS__),                                                                                     \
             ((setdimsfunc_t *) __builtin_choose_expr (__builtin_classify_type (f) == pointer_type_class, f, NULL)     \
             ) (dw, dh),                                                                                               \
             __builtin_choose_expr (                                                                                   \
                 NARGS (__VA_ARGS__) == 1,                                                                             \
                 ((setdimsfunc_t *) __builtin_choose_expr (__builtin_classify_type (f) == pointer_type_class, f, NULL) \
                 ) (u64decomp (__builtin_choose_expr (                                                                 \
                     isint (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 0)), ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 0), 0             \
                 ))),                                                                                                  \
                 ((setdimsfunc_t *) __builtin_choose_expr (__builtin_classify_type (f) == pointer_type_class, f, NULL) \
                 ) (__builtin_choose_expr (                                                                            \
                        isint (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 0)), ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 0), 0          \
                    ),                                                                                                 \
                    __builtin_choose_expr (                                                                            \
                        isint (ARG2 (__VA_ARGS__ __VA_OPT__ (, ) 0)), ARG2 (__VA_ARGS__ __VA_OPT__ (, ) 0), 0          \
                    ))                                                                                                 \
             )                                                                                                         \
         ))

    #define setwidth(f, dw, dh, ...)                                                                                   \
        (ct_error (NARGS (__VA_ARGS__) > 1, "the setwidth() macro accepts either three or four arguments."),           \
         ct_error (                                                                                                    \
             !(__builtin_types_compatible_p (typeof (f), setdimsfunc_t) ||                                             \
               __builtin_types_compatible_p (typeof (f), setdimsfunc_t *)),                                            \
             "the first argument passed to the setwidth() macro must a setdimsfunc_t function (pointer)."              \
         ),                                                                                                            \
         ct_error (                                                                                                    \
             !(isint (dw) && isint (dh) && isint (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 0))),                              \
             "the second, third and fourth arguments passed to the setwidth() macro must be of integral type."         \
         ),                                                                                                            \
         setdims (f, dw, dh, ARG1 (__VA_ARGS__ __VA_OPT__ (, ) dw), dh))

    #define setheight(f, dw, dh, ...)                                                                                  \
        (ct_error (NARGS (__VA_ARGS__) > 1, "the setheight() macro accepts either three or four arguments."),          \
         ct_error (                                                                                                    \
             !(__builtin_types_compatible_p (typeof (f), setdimsfunc_t) ||                                             \
               __builtin_types_compatible_p (typeof (f), setdimsfunc_t *)),                                            \
             "the first argument passed to the setheight() macro must a setdimsfunc_t function (pointer)."             \
         ),                                                                                                            \
         ct_error (                                                                                                    \
             !(isint (dw) && isint (dh) && isint (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 0))),                              \
             "the second, third and fourth arguments passed to the setheight() macro must be of integral type."        \
         ),                                                                                                            \
         setdims (f, dw, dh, dw, ARG1 (__VA_ARGS__ __VA_OPT__ (, ) dh)))

#endif

#endif