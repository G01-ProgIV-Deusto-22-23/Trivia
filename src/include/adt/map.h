#ifndef TRIVIA_MAP_H
#define TRIVIA_MAP_H

#define DEFAULT_MAP_CAPACITY 100UL

extern map_t  impl_create_map (size_t);
extern bool   impl_destroy_map (restrict map_t *const);
extern size_t capacity_map (const restrict map_t);
extern map_t  impl_resize_map (map_t *const restrict, size_t);
extern void  *impl_get_map (const restrict map_t, void *const, const key_type_t);
extern bool   impl_set_map (const restrict map_t, void *const, const key_type_t, void *const);
extern bool   impl_put_map (const restrict map_t, void *const, const key_type_t, void *const);
extern bool   impl_remove_map (const restrict map_t, void *const, const key_type_t);

#define create_map(...)                                                                                                \
    (ct_error (                                                                                                        \
         NARGS (__VA_ARGS__) > 1, "the create_map() macro accepts either a single argument or no arguments at all."    \
     ),                                                                                                                \
     ct_error (                                                                                                        \
         !isint (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 0)),                                                                \
         "the argument passed to the create_map() macro must be of integral type."                                     \
     ),                                                                                                                \
     impl_create_map (                                                                                                 \
         __builtin_choose_expr (isint (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 0)), ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 0), 0) \
     ))

#define destroy_map(m)                                                                                                           \
    ({                                                                                                                           \
        _Pragma ("GCC diagnostic push");                                                                                         \
        _Pragma ("GCC diagnostic ignored \"-Wshadow=local\"");                                                                   \
        _Pragma ("GCC diagnostic ignored \"-Wshadow=compatible-local\"");                                                        \
        auto __destroy_map_m__ = m;                                                                                              \
        _Pragma ("GCC diagnostic pop");                                                                                          \
        ct_error (                                                                                                               \
            !(__builtin_types_compatible_p (typeof (__destroy_map_m__), map_t) ||                                                \
              __builtin_types_compatible_p (typeof (__destroy_map_m__), map_t *) ||                                              \
              __builtin_types_compatible_p (typeof (__destroy_map_m__), map_t [])),                                              \
            "the type of the first argument passed to the remove_map() macro must be compatile with map_t, map_t * or map_t []." \
        );                                                                                                                       \
        impl_destroy_map (__builtin_choose_expr (                                                                                \
            __builtin_types_compatible_p (typeof (__destroy_map_m__), map_t), &__destroy_map_m__, __destroy_map_m__              \
        ));                                                                                                                      \
    })

#define resize_map(m, ...)                                                                                                       \
    ({                                                                                                                           \
        _Pragma ("GCC diagnostic push");                                                                                         \
        _Pragma ("GCC diagnostic ignored \"-Wshadow=local\"");                                                                   \
        _Pragma ("GCC diagnostic ignored \"-Wshadow=compatible-local\"");                                                        \
        auto __resize_map_m__ = m;                                                                                               \
        _Pragma ("GCC diagnostic pop");                                                                                          \
        ct_error (                                                                                                               \
            NARGS (__VA_ARGS__) > 1, "the create_map() macro accepts either a single argument or no arguments at all."           \
        );                                                                                                                       \
        ct_error (                                                                                                               \
            !(__builtin_types_compatible_p (typeof (__resize_map_m__), map_t) ||                                                 \
              __builtin_types_compatible_p (typeof (__resize_map_m__), map_t *) ||                                               \
              __builtin_types_compatible_p (typeof (__resize_map_m__), map_t [])),                                               \
            "the type of the first argument passed to the remove_map() macro must be compatile with map_t, map_t * or map_t []." \
        );                                                                                                                       \
        ct_error (                                                                                                               \
            !isint (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 0)),                                                                       \
            "the argument passed to the create_map() macro must be of integral type."                                            \
        );                                                                                                                       \
        impl_resize_map (                                                                                                        \
            __builtin_choose_expr (                                                                                              \
                __builtin_types_compatible_p (typeof (__resize_map_m__), map_t), &__resize_map_m__, __resize_map_m__             \
            ),                                                                                                                   \
            __builtin_choose_expr (                                                                                              \
                isint (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 0)), ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 0), 0                            \
            )                                                                                                                    \
        );                                                                                                                       \
    })

#define get_map(m, k)                                                                                                              \
    (ct_error (                                                                                                                    \
         !__builtin_types_compatible_p (typeof (m), map_t),                                                                        \
         "the type of the first argument passed to the put_map() macro must be compatible with map_t."                             \
     ),                                                                                                                            \
     ct_error (                                                                                                                    \
         !(isint (k) || __builtin_types_compatible_p (typeof (k), char *) ||                                                       \
           __builtin_types_compatible_p (typeof (k), const char *) ||                                                              \
           __builtin_types_compatible_p (typeof (k), char []) ||                                                                   \
           __builtin_types_compatible_p (typeof (k), const char [])),                                                              \
         "the second argument passed to the put_map() macro must be either of integral type or compatible with (const) char */[]." \
     ),                                                                                                                            \
     impl_get_map (                                                                                                                \
         m, (void *) (uintptr_t) k,                                                                                                \
         __builtin_choose_expr (                                                                                                   \
             isint (k), __builtin_choose_expr (sizeof (k) * __CHAR_BIT__ <= 32, u32_key, u64_key), str_key                         \
         )                                                                                                                         \
     ))

#define put_map(m, k, v)                                                                                                           \
    (ct_error (                                                                                                                    \
         !__builtin_types_compatible_p (typeof (m), map_t),                                                                        \
         "the type of the first argument passed to the put_map() macro must be compatible with map_t."                             \
     ),                                                                                                                            \
     ct_error (                                                                                                                    \
         !(isint (k) || __builtin_types_compatible_p (typeof (k), char *) ||                                                       \
           __builtin_types_compatible_p (typeof (k), const char *) ||                                                              \
           __builtin_types_compatible_p (typeof (k), char []) ||                                                                   \
           __builtin_types_compatible_p (typeof (k), const char [])),                                                              \
         "the second argument passed to the put_map() macro must be either of integral type or compatible with (const) char */[]." \
     ),                                                                                                                            \
     ct_error (                                                                                                                    \
         __builtin_classify_type (v) != pointer_type_class,                                                                        \
         "the third argument passed to the put_map() macro must be of pointer or array type."                                      \
     ),                                                                                                                            \
     impl_put_map (                                                                                                                \
         m, (void *) (uintptr_t) k,                                                                                                \
         __builtin_choose_expr (                                                                                                   \
             isint (k), __builtin_choose_expr (sizeof (k) * __CHAR_BIT__ <= 32, u32_key, u64_key), str_key                         \
         ),                                                                                                                        \
         v                                                                                                                         \
     ))

#define remove_map(m, k)                                                                                                           \
    (ct_error (                                                                                                                    \
         !__builtin_types_compatible_p (typeof (m), map_t),                                                                        \
         "the type of the first argument passed to the put_map() macro must be compatible with map_t."                             \
     ),                                                                                                                            \
     ct_error (                                                                                                                    \
         !(isint (k) || __builtin_types_compatible_p (typeof (k), char *) ||                                                       \
           __builtin_types_compatible_p (typeof (k), const char *) ||                                                              \
           __builtin_types_compatible_p (typeof (k), char []) ||                                                                   \
           __builtin_types_compatible_p (typeof (k), const char [])),                                                              \
         "the second argument passed to the put_map() macro must be either of integral type or compatible with (const) char */[]." \
     ),                                                                                                                            \
     impl_put_map (                                                                                                                \
         m, (void *) (uintptr_t) k,                                                                                                \
         __builtin_choose_expr (                                                                                                   \
             isint (k), __builtin_choose_expr (sizeof (k) * __CHAR_BIT__ <= 32, u32_key, u64_key), str_key                         \
         )                                                                                                                         \
     ))

#endif