#ifndef TRIVIA_LINKEDLIST_H
#define TRIVIA_LINKEDLIST_H

#ifdef __cplusplus
extern "C" {
#endif

    extern linkedlist_t create_linkedlist (void *const);
    extern size_t       length_linkedlist (const linkedlist_t);
    extern bool         impl_destroy_linkedlist (linkedlist_t *const restrict);
    extern void         freedata_linkedlist (const restrict linkedlist_t, freefunc_t *const);
    extern bool         insert_linkedlist (const linkedlist_t, void *const);
    extern void        *impl_find_linkedlist (linkedlist_t, void *const, cmpfunc_t *const, size_t *const restrict);
    extern size_t       impl_indexof_linkedlist (linkedlist_t, void *const, cmpfunc_t *const cmp);
    extern void        *impl_pop_linkedlist (linkedlist_t *const restrict, const size_t);
    extern bool         impl_remove_linkedlist (linkedlist_t *const restrict, const size_t);
    extern void        *get_linkedlist (linkedlist_t, const size_t);
    extern void        *set_linkedlist (linkedlist_t, const size_t, void *const);
    extern void        *toarray_linkedlist (linkedlist_t, size_t *const restrict);

#ifdef __cplusplus
    #define destroy_linkedlist(l)                                                                                                                              \
        (ct_error (                                                                                                                                            \
             !std::is_same<                                                                                                                                    \
                 std::remove_const<std::remove_pointer<std::decay<decltype (l)>::type>::type>::type,                                                           \
                 linkedlist_t>::value,                                                                                                                         \
             "the type of the first argument passed to the destroy_linkedlist() macro must be compatile with linkedlist_t, linkedlist_t * or linkedlist_t []." \
         ),                                                                                                                                                    \
         impl_destroy_linkedlist (                                                                                                                             \
             static_cast<linkedlist_t *> (std::is_same<decltype (l), linkedlist_t>::value ? (void *) &l : (void *) l)                                          \
         ))
#else
    #define destroy_linkedlist(l)                                                                                                                              \
        (ct_error (                                                                                                                                            \
             !(__builtin_types_compatible_p (typeof ((l)), linkedlist_t) ||                                                                                    \
               __builtin_types_compatible_p (typeof ((l)), linkedlist_t *) ||                                                                                  \
               __builtin_types_compatible_p (typeof ((l)), linkedlist_t [])),                                                                                  \
             "the type of the first argument passed to the destroy_linkedlist() macro must be compatile with linkedlist_t, linkedlist_t * or linkedlist_t []." \
         ),                                                                                                                                                    \
         impl_destroy_linkedlist (                                                                                                                             \
             __builtin_choose_expr (__builtin_types_compatible_p (typeof ((l)), linkedlist_t), &(l), (l))                                                      \
         ))
#endif

#ifdef __cplusplus
    #define find_linkedlist(l, d, ...)                                                                                      \
        (ct_error (NARGS (__VA_ARGS__) > 2, "the find_linkedlist() macro must be passed between 2 and 4 arguments."),       \
         ct_error (                                                                                                         \
             !std::is_same<decltype ((l)), linkedlist_t>::value,                                                            \
             "the type of the first argument passed to the indexof_linkedlist() macro must be compatile with linkedlist_t." \
         ),                                                                                                                 \
         ct_error (                                                                                                         \
             !std::is_pointer<std::decay<decltype (d)>::type>::value,                                                       \
             "the second argument passed to the find_linkedlist() macro must be of pointer or array type."                  \
         ),                                                                                                                 \
         ct_error (                                                                                                         \
             !std::is_same<decltype (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) ((cmpfunc_t *) NULL))), cmpfunc_t *>::value,         \
             "the type of the third argument passed to the find_linkedlist() macro must be compatible with cmpfunc_t *."    \
         ),                                                                                                                 \
         ct_error (                                                                                                         \
             !std::is_pointer<std::decay<decltype (ARG2 (__VA_ARGS__ __VA_OPT__ (, ) NULL, NULL))>::type>::value,           \
             "the type of the fourth argument passed to the find_linkedlist() macro must be of pointer or array type."      \
         ),                                                                                                                 \
         impl_find_linkedlist (                                                                                             \
             (l), (d), ARG1 (__VA_ARGS__ __VA_OPT__ (, ) ((cmpfunc_t *) NULL)),                                             \
             ARG2 (__VA_ARGS__ __VA_OPT__ (, ) ((size_t *) NULL), ((size_t *) NULL))                                        \
         ))
#else
    #define find_linkedlist(l, d, ...)                                                                                      \
        (ct_error (NARGS (__VA_ARGS__) > 2, "the find_linkedlist() macro must be passed between 2 and 4 arguments."),       \
         ct_error (                                                                                                         \
             !__builtin_types_compatible_p (typeof ((l)), linkedlist_t),                                                    \
             "the type of the first argument passed to the indexof_linkedlist() macro must be compatile with linkedlist_t." \
         ),                                                                                                                 \
         ct_error (                                                                                                         \
             __builtin_classify_type ((d)) != pointer_type_class,                                                           \
             "the second argument passed to the find_linkedlist() macro must be of pointer or array type."                  \
         ),                                                                                                                 \
         ct_error (                                                                                                         \
             !__builtin_types_compatible_p (typeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) ((cmpfunc_t *) NULL))), cmpfunc_t *), \
             "the type of the third argument passed to the find_linkedlist() macro must be compatible with cmpfunc_t *."    \
         ),                                                                                                                 \
         ct_error (                                                                                                         \
             __builtin_classify_type (ARG2 (__VA_ARGS__ __VA_OPT__ (, ) NULL, NULL)) != pointer_type_class,                 \
             "the type of the fourth argument passed to the find_linkedlist() macro must be of pointer or array type."      \
         ),                                                                                                                 \
         impl_find_linkedlist (                                                                                             \
             (l), (d), ARG1 (__VA_ARGS__ __VA_OPT__ (, ) ((cmpfunc_t *) NULL)),                                             \
             ARG2 (__VA_ARGS__ __VA_OPT__ (, ) ((size_t *) NULL), ((size_t *) NULL))                                        \
         ))
#endif

#ifdef __cplusplus
    #define indexof_linkedlist(l, d, ...)                                                                                   \
        (ct_error (                                                                                                         \
             NARGS (__VA_ARGS__) > 1, "the indexof_linkedlist() macro must be passed between 2 and 4 arguments."            \
         ),                                                                                                                 \
         ct_error (                                                                                                         \
             !std::is_same<decltype ((l)), linkedlist_t>::value,                                                            \
             "the type of the first argument passed to the indexof_linkedlist() macro must be compatile with linkedlist_t." \
         ),                                                                                                                 \
         ct_error (                                                                                                         \
             !std::is_pointer<std::decay<decltype (d)>::type>::value,                                                       \
             "the second argument passed to the indexof_linkedlist() macro must be of pointer or array type."               \
         ),                                                                                                                 \
         ct_error (                                                                                                         \
             !std::is_same<decltype (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) ((cmpfunc_t *) NULL))), cmpfunc_t *>::value,         \
             "the type of the third argument passed to the indexof_linkedlist() macro must be compatible with cmpfunc_t *." \
         ),                                                                                                                 \
         impl_indexof_linkedlist ((l), (d), ARG1 (__VA_ARGS__ __VA_OPT__ (, ) ((cmpfunc_t *) NULL))))
#else
    #define indexof_linkedlist(l, d, ...)                                                                                   \
        (ct_error (                                                                                                         \
             NARGS (__VA_ARGS__) > 1, "the indexof_linkedlist() macro must be passed between 2 and 3 arguments."            \
         ),                                                                                                                 \
         ct_error (                                                                                                         \
             !__builtin_types_compatible_p (typeof ((l)), linkedlist_t),                                                    \
             "the type of the first argument passed to the indexof_linkedlist() macro must be compatile with linkedlist_t." \
         ),                                                                                                                 \
         ct_error (                                                                                                         \
             __builtin_classify_type ((d)) != pointer_type_class,                                                           \
             "the second argument passed to the indexof_linkedlist() macro must be of pointer or array type."               \
         ),                                                                                                                 \
         ct_error (                                                                                                         \
             !__builtin_types_compatible_p (typeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) ((cmpfunc_t *) NULL))), cmpfunc_t *), \
             "the type of the third argument passed to the indexof_linkedlist() macro must be compatible with cmpfunc_t *." \
         ),                                                                                                                 \
         impl_indexof_linkedlist ((l), (d), ARG1 (__VA_ARGS__ __VA_OPT__ (, ) ((cmpfunc_t *) NULL))))
#endif

#ifdef __cplusplus
    #define pop_linkedlist(l, i, ...)                                                                                   \
        (ct_error (NARGS (__VA_ARGS__) > 1, "the pop_linkedlist() macro must be passed between 2 and 4 arguments."),    \
         ct_error (                                                                                                     \
             !std::is_same<decltype ((l)), linkedlist_t>::value,                                                        \
             "the type of the first argument passed to the pop_linkedlist() macro must be compatile with linkedlist_t." \
         ),                                                                                                             \
         ct_error (                                                                                                     \
             !isint ((i)), "the second argument passed to the pop_linkedlist() macro must be of integral type."         \
         ),                                                                                                             \
         ct_error (                                                                                                     \
             !std::is_same<decltype (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) ((cmpfunc_t *) NULL))), cmpfunc_t *>::value,     \
             "the type of the third argument passed to the pop_linkedlist() macro must be compatible with cmpfunc_t *." \
         ),                                                                                                             \
         impl_pop_linkedlist ((l), (i), ARG1 (__VA_ARGS__ __VA_OPT__ (, ) ((cmpfunc_t *) NULL))))
#else
    #define pop_linkedlist(l, i)                                                                                                                           \
        (ct_error (                                                                                                                                        \
             !(__builtin_types_compatible_p (typeof ((l)), linkedlist_t) ||                                                                                \
               __builtin_types_compatible_p (typeof ((l)), linkedlist_t *) ||                                                                              \
               __builtin_types_compatible_p (typeof ((l)), linkedlist_t [])),                                                                              \
             "the type of the first argument passed to the pop_linkedlist() macro must be compatile with linkedlist_t, linkedlist_t * or linkedlist_t []." \
         ),                                                                                                                                                \
         ct_error (                                                                                                                                        \
             !isint ((i)), "the second argument passed to the pop_linkedlist() macro must be of integral type."                                            \
         ),                                                                                                                                                \
         impl_pop_linkedlist (                                                                                                                             \
             __builtin_choose_expr (__builtin_types_compatible_p (typeof ((l)), linkedlist_t), &(l), (l)), (i)                                             \
         ))
#endif

#ifdef __cplusplus
    #define remove_linkedlist(l, i, ...)                                                                                   \
        (ct_error (                                                                                                        \
             NARGS (__VA_ARGS__) > 1, "the remove_linkedlist() macro must be passed between 2 and 4 arguments."            \
         ),                                                                                                                \
         ct_error (                                                                                                        \
             !std::is_same<decltype ((l)), linkedlist_t>::value,                                                           \
             "the type of the first argument passed to the remove_linkedlist() macro must be compatile with linkedlist_t." \
         ),                                                                                                                \
         ct_error (                                                                                                        \
             !isint ((i)), "the second argument passed to the remove_linkedlist() macro must be of integral type."         \
         ),                                                                                                                \
         ct_error (                                                                                                        \
             !std::is_same<decltype (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) ((cmpfunc_t *) NULL))), cmpfunc_t *>::value,        \
             "the type of the third argument passed to the remove_linkedlist() macro must be compatible with cmpfunc_t *." \
         ),                                                                                                                \
         impl_remove_linkedlist ((l), (i), ARG1 (__VA_ARGS__ __VA_OPT__ (, ) ((cmpfunc_t *) NULL))))
#else
    #define remove_linkedlist(l, i)                                                                                                                           \
        (ct_error (                                                                                                                                           \
             !(__builtin_types_compatible_p (typeof ((l)), linkedlist_t) ||                                                                                   \
               __builtin_types_compatible_p (typeof ((l)), linkedlist_t *) ||                                                                                 \
               __builtin_types_compatible_p (typeof ((l)), linkedlist_t [])),                                                                                 \
             "the type of the first argument passed to the remove_linkedlist() macro must be compatile with linkedlist_t, linkedlist_t * or linkedlist_t []." \
         ),                                                                                                                                                   \
         ct_error (                                                                                                                                           \
             !isint ((i)), "the second argument passed to the remove_linkedlist() macro must be of integral type."                                            \
         ),                                                                                                                                                   \
         impl_remove_linkedlist (                                                                                                                             \
             __builtin_choose_expr (__builtin_types_compatible_p (typeof ((l)), linkedlist_t), &(l), (l)), (i)                                                \
         ))
#endif

#ifdef __cplusplus
}
#endif

#endif