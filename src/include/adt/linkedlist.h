#ifndef TRIVIA_LINKEDLIST_H
#define TRIVIA_LINKEDLIST_H

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

#define destroy_linkedlist(l)                                                                                                                                \
    ({                                                                                                                                                       \
        _Pragma ("GCC diagnostic push");                                                                                                                     \
        _Pragma ("GCC diagnostic ignored \"-Wshadow=local\"");                                                                                               \
        _Pragma ("GCC diagnostic ignored \"-Wshadow=compatible-local\"");                                                                                    \
        auto __destroy_linkedlist_l__ = l;                                                                                                                   \
        _Pragma ("GCC diagnostic pop");                                                                                                                      \
        ct_error (                                                                                                                                           \
            !(__builtin_types_compatible_p (typeof (__destroy_linkedlist_l__), linkedlist_t) ||                                                              \
              __builtin_types_compatible_p (typeof (__destroy_linkedlist_l__), linkedlist_t *) ||                                                            \
              __builtin_types_compatible_p (typeof (__destroy_linkedlist_l__), linkedlist_t [])),                                                            \
            "the type of the first argument passed to the remove_linkedlist() macro must be compatile with linkedlist_t, linkedlist_t * or linkedlist_t []." \
        );                                                                                                                                                   \
        impl_destroy_linkedlist (__builtin_choose_expr (                                                                                                     \
            __builtin_types_compatible_p (typeof (__destroy_linkedlist_l__), linkedlist_t), &__destroy_linkedlist_l__,                                       \
            __destroy_linkedlist_l__                                                                                                                         \
        ));                                                                                                                                                  \
    })

#define find_linkedlist(l, d, ...)                                                                                      \
    (ct_error (NARGS (__VA_ARGS__) > 2, "the find_linkedlist() macro must be passed between 2 and 4 arguments."),       \
     ct_error (                                                                                                         \
         !__builtin_types_compatible_p (typeof (l), linkedlist_t),                                                      \
         "the type of the first argument passed to the remove_linkedlist() macro must be compatile with linkedlist_t."  \
     ),                                                                                                                 \
     ct_error (                                                                                                         \
         !__builtin_types_compatible_p (typeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) ((cmpfunc_t *) NULL))), cmpfunc_t *), \
         "the type of the third argument passed to the find_linkedlist() macro must be compatible with cmpfunc_t *."    \
     ),                                                                                                                 \
     ct_error (                                                                                                         \
         __builtin_classify_type (ARG3 (__VA_ARGS__ __VA_OPT__ (, ) NULL, NULL)) != pointer_type_class,                 \
         "the type of the fourth argument passed to the find_linkedlist() macro must be of pointer or array type."      \
     ),                                                                                                                 \
     impl_find_linkedlist (                                                                                             \
         l, d, ARG1 (__VA_ARGS__ __VA_OPT__ (, ) ((cmpfunc_t *) NULL)),                                                 \
         ARG2 (__VA_ARGS__ __VA_OPT__ (, ) ((size_t *) NULL), ((size_t *) NULL))                                        \
     ))

#define indexof_linkedlist(l, d, ...)                                                                                   \
    (ct_error (NARGS (__VA_ARGS__) > 1, "the indexof_linkedlist() macro must be passed between 2 and 3 arguments."),    \
     ct_error (                                                                                                         \
         !__builtin_types_compatible_p (typeof (l), linkedlist_t),                                                      \
         "the type of the first argument passed to the indexof_linkedlist() macro must be compatile with linkedlist_t." \
     ),                                                                                                                 \
     ct_error (                                                                                                         \
         !__builtin_types_compatible_p (typeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) ((cmpfunc_t *) NULL))), cmpfunc_t *), \
         "the type of the third argument passed to the indexof_linkedlist() macro must be compatible with cmpfunc_t *." \
     ),                                                                                                                 \
     impl_indexof_linkedlist (l, d, ARG1 (__VA_ARGS__ __VA_OPT__ (, ) ((cmpfunc_t *) NULL))))

#define pop_linkedlist(l, i)                                                                                                                              \
    ({                                                                                                                                                    \
        _Pragma ("GCC diagnostic push");                                                                                                                  \
        _Pragma ("GCC diagnostic ignored \"-Wshadow=local\"");                                                                                            \
        _Pragma ("GCC diagnostic ignored \"-Wshadow=compatible-local\"");                                                                                 \
        auto __pop_linkedlist_l__ = l;                                                                                                                    \
        _Pragma ("GCC diagnostic pop");                                                                                                                   \
        ct_error (                                                                                                                                        \
            !(__builtin_types_compatible_p (typeof (__pop_linkedlist_l__), linkedlist_t) ||                                                               \
              __builtin_types_compatible_p (typeof (__pop_linkedlist_l__), linkedlist_t *) ||                                                             \
              __builtin_types_compatible_p (typeof (__pop_linkedlist_l__), linkedlist_t [])),                                                             \
            "the type of the first argument passed to the pop_linkedlist() macro must be compatile with linkedlist_t, linkedlist_t * or linkedlist_t []." \
        );                                                                                                                                                \
        ct_error (!isint (i), "the second argument passed to the pop_linkedlist() macro must be of integral type.");                                      \
        impl_pop_linkedlist (                                                                                                                             \
            __builtin_choose_expr (                                                                                                                       \
                __builtin_types_compatible_p (typeof (__pop_linkedlist_l__), linkedlist_t), &__pop_linkedlist_l__,                                        \
                __pop_linkedlist_l__                                                                                                                      \
            ),                                                                                                                                            \
            i                                                                                                                                             \
        );                                                                                                                                                \
    })

#define remove_linkedlist(l, i)                                                                                                                              \
    ({                                                                                                                                                       \
        _Pragma ("GCC diagnostic push");                                                                                                                     \
        _Pragma ("GCC diagnostic ignored \"-Wshadow=local\"");                                                                                               \
        _Pragma ("GCC diagnostic ignored \"-Wshadow=compatible-local\"");                                                                                    \
        auto __remove_linkedlist_l__ = l;                                                                                                                    \
        _Pragma ("GCC diagnostic pop");                                                                                                                      \
        ct_error (                                                                                                                                           \
            !(__builtin_types_compatible_p (typeof (__remove_linkedlist_l__), linkedlist_t) ||                                                               \
              __builtin_types_compatible_p (typeof (__remove_linkedlist_l__), linkedlist_t *) ||                                                             \
              __builtin_types_compatible_p (typeof (__remove_linkedlist_l__), linkedlist_t [])),                                                             \
            "the type of the first argument passed to the remove_linkedlist() macro must be compatile with linkedlist_t, linkedlist_t * or linkedlist_t []." \
        );                                                                                                                                                   \
        ct_error (                                                                                                                                           \
            !isint (i), "the second argument passed to the remove_linkedlist() macro must be of integral type."                                              \
        );                                                                                                                                                   \
        impl_remove_linkedlist (                                                                                                                             \
            __builtin_choose_expr (                                                                                                                          \
                __builtin_types_compatible_p (typeof (__remove_linkedlist_l__), linkedlist_t),                                                               \
                &__remove_linkedlist_l__, __remove_linkedlist_l__                                                                                            \
            ),                                                                                                                                               \
            i                                                                                                                                                \
        );                                                                                                                                                   \
    })

#endif