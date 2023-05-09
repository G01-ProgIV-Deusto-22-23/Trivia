#ifndef TRIVIA_MAP_H
#define TRIVIA_MAP_H

#ifdef __cplusplus
extern "C" {
#endif

#define DEFAULT_MAP_CAPACITY 100

    extern map_t  impl_create_map (size_t);
    extern bool   impl_destroy_map (restrict map_t *const);
    extern size_t capacity_map (const restrict map_t);
    extern map_t  impl_resize_map (restrict map_t *const restrict, size_t);
    extern void  *impl_get_map (const restrict map_t, void *const, const key_type_t);
    extern bool   impl_set_map (const restrict map_t, void *const, const key_type_t, void *const);
    extern bool   impl_put_map (const restrict map_t, void *const, const key_type_t, void *const);
    extern bool   impl_remove_map (const restrict map_t, void *const, const key_type_t);
    extern void  *keys_map (const restrict map_t, size_t *const restrict);
    extern void  *values_map (const restrict map_t, size_t *const restrict);

#ifdef __cplusplus
    #define create_map(...)                                                                                            \
        (ct_error (                                                                                                    \
             NARGS (__VA_ARGS__) > 1,                                                                                  \
             "the create_map() macro accepts either a single argument or no arguments at all."                         \
         ),                                                                                                            \
         ct_error (                                                                                                    \
             !isint (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 0)),                                                            \
             "the argument passed to the create_map() macro must be of integral type."                                 \
         ),                                                                                                            \
         impl_create_map (static_cast<std::size_t> (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 0))))
#else
    #define create_map(...)                                                                                            \
        (ct_error (                                                                                                    \
             NARGS (__VA_ARGS__) > 1,                                                                                  \
             "the create_map() macro accepts either a single argument or no arguments at all."                         \
         ),                                                                                                            \
         ct_error (                                                                                                    \
             !isint (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 0)),                                                            \
             "the argument passed to the create_map() macro must be of integral type."                                 \
         ),                                                                                                            \
         impl_create_map (__builtin_choose_expr (                                                                      \
             isint (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 0)), ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 0), 0                     \
         )))
#endif

#ifdef __cplusplus
    #define destroy_map(m)                                                                                                     \
        (ct_error (                                                                                                            \
             !std::is_same<std::remove_pointer<std::decay<decltype (m)>::type>::type, map_t>::value,                           \
             "the type of the first argument passed to the destroy() macro must be compatile with map_t, map_t * or map_t []." \
         ),                                                                                                                    \
         impl_destroy_map (static_cast<map_t *> (std::is_same<decltype (l), map_t>::value ? (void *) &m : (void *) m))         \
        )
#else
    #define destroy_map(m)                                                                                                        \
        (ct_error (                                                                                                               \
             !(__builtin_types_compatible_p (typeof ((m)), map_t) ||                                                              \
               __builtin_types_compatible_p (typeof ((m)), map_t *) ||                                                            \
               __builtin_types_compatible_p (typeof ((m)), map_t [])),                                                            \
             "the type of the first argument passed to the remove_map() macro must be compatile with map_t, map_t * or map_t []." \
         ),                                                                                                                       \
         impl_destroy_map (__builtin_choose_expr (__builtin_types_compatible_p (typeof ((m)), map_t), &(m), (m))))
#endif

#ifdef __cpluplus
    #define resize_map(m, ...)                                                                                                    \
        (ct_error (                                                                                                               \
             NARGS (__VA_ARGS__) > 1,                                                                                             \
             "the resize_map() macro accepts either a single argument or no arguments at all."                                    \
         ),                                                                                                                       \
         ct_error (                                                                                                               \
             !std::is_same<std::remove_pointer<std::decay<decltype (m)>::type>::type, map_t>::value,                              \
             "the type of the first argument passed to the resize_map() macro must be compatile with map_t, map_t * or map_t []." \
         ),                                                                                                                       \
         ct_error (                                                                                                               \
             !isint (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 0)),                                                                       \
             "the argument passed to the resize_map() macro must be of integral type."                                            \
         ),                                                                                                                       \
         impl_resize_map (                                                                                                        \
             static_cast<map_t *> (std::is_same<decltype (l), map_t>::value ? (void *) &m : (void *) m),                          \
             static_cast<std::size_t> (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 0))                                                      \
         ))
#else
    #define resize_map(m, ...)                                                                                                    \
        (ct_error (                                                                                                               \
             NARGS (__VA_ARGS__) > 1,                                                                                             \
             "the resize_map() macro accepts either a single argument or no arguments at all."                                    \
         ),                                                                                                                       \
         ct_error (                                                                                                               \
             !(__builtin_types_compatible_p (typeof ((m)), map_t) ||                                                              \
               __builtin_types_compatible_p (typeof ((m)), map_t *) ||                                                            \
               __builtin_types_compatible_p (typeof ((m)), map_t [])),                                                            \
             "the type of the first argument passed to the resize_map() macro must be compatile with map_t, map_t * or map_t []." \
         ),                                                                                                                       \
         ct_error (                                                                                                               \
             !isint (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 0)),                                                                       \
             "the argument passed to the resize_map() macro must be of integral type."                                            \
         ),                                                                                                                       \
         impl_resize_map (                                                                                                        \
             __builtin_choose_expr (__builtin_types_compatible_p (typeof ((m)), map_t), &(m), (m)),                               \
             __builtin_choose_expr (                                                                                              \
                 isint (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 0)), ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 0), 0                            \
             )                                                                                                                    \
         ))
#endif

#ifdef __cpluplus
    #define get_map(m, k)                                                                                                                                \
        (ct_error (                                                                                                                                      \
             !std::is_same<std::remove_const<decltype ((m))>::type, map_t>::value,                                                                       \
             "the type of the first argument passed to the get_map() macro must be compatible with map_t."                                               \
         ),                                                                                                                                              \
         ct_error (                                                                                                                                      \
             !(isint ((k)) || (std::is_pointer<std::decay<decltype (k)>::type>::value &&                                                                 \
                               std::is_same<std::remove_const<std::decay<decltype (k)>::type>::type, char *>::value)),                                   \
             "the second argument passed to the get_map() macro must be either of integral type or compatible with (optionally cv-qualified) char */[]." \
         ),                                                                                                                                              \
         impl_get_map (                                                                                                                                  \
             (m), (void *) (uintptr_t) (k),                                                                                                              \
             isint ((k)) ? sizeof ((k)) * __CHAR_BIT__ <= 32 ? u32_key : u64_key : str_key                                                               \
         ))
#else
    #define get_map(m, k)                                                                                                              \
        (ct_error (                                                                                                                    \
             !__builtin_types_compatible_p (typeof ((m)), map_t),                                                                      \
             "the type of the first argument passed to the get_map() macro must be compatible with map_t."                             \
         ),                                                                                                                            \
         ct_error (                                                                                                                    \
             !(isint ((k)) || __builtin_types_compatible_p (typeof ((k)), char *) ||                                                   \
               __builtin_types_compatible_p (typeof ((k)), const char *) ||                                                            \
               __builtin_types_compatible_p (typeof ((k)), char []) ||                                                                 \
               __builtin_types_compatible_p (typeof ((k)), const char [])),                                                            \
             "the second argument passed to the get_map() macro must be either of integral type or compatible with (const) char */[]." \
         ),                                                                                                                            \
         impl_get_map (                                                                                                                \
             (m), (void *) (uintptr_t) (k),                                                                                            \
             __builtin_choose_expr (                                                                                                   \
                 isint ((k)), __builtin_choose_expr (sizeof ((k)) * __CHAR_BIT__ <= 32, u32_key, u64_key), str_key                     \
             )                                                                                                                         \
         ))
#endif

#ifdef __cpluplus
    #define set_map(m, k, v)                                                                                                                             \
        (ct_error (                                                                                                                                      \
             !std::is_same<std::remove_const<decltype ((m))>::type, map_t>::value,                                                                       \
             "the type of the first argument passed to the set_map() macro must be compatible with map_t."                                               \
         ),                                                                                                                                              \
         ct_error (                                                                                                                                      \
             !(isint ((k)) || (std::is_pointer<std::decay<decltype (k)>::type>::value &&                                                                 \
                               std::is_same<std::remove_const<std::decay<decltype (k)>::type>::type, char *>::value)),                                   \
             "the second argument passed to the set_map() macro must be either of integral type or compatible with (optionally cv-qualified) char */[]." \
         ),                                                                                                                                              \
         ct_error (                                                                                                                                      \
             std::is_pointer<std::decay<decltype (v)>::type>::value,                                                                                     \
             "the third argument passed to the set_map() macro must be of pointer or array type."                                                        \
         ),                                                                                                                                              \
         impl_set_map (                                                                                                                                  \
             (m), (void *) (uintptr_t) (k),                                                                                                              \
             isint ((k)) ? sizeof ((k)) * __CHAR_BIT__ <= 32 ? u32_key : u64_key : str_key, (v)                                                          \
         ))
#else
    #define set_map(m, k, v)                                                                                                           \
        (ct_error (                                                                                                                    \
             !__builtin_types_compatible_p (typeof ((m)), map_t),                                                                      \
             "the type of the first argument passed to the set_map() macro must be compatible with map_t."                             \
         ),                                                                                                                            \
         ct_error (                                                                                                                    \
             !(isint ((k)) || __builtin_types_compatible_p (typeof ((k)), char *) ||                                                   \
               __builtin_types_compatible_p (typeof ((k)), const char *) ||                                                            \
               __builtin_types_compatible_p (typeof ((k)), char []) ||                                                                 \
               __builtin_types_compatible_p (typeof ((k)), const char [])),                                                            \
             "the second argument passed to the set_map() macro must be either of integral type or compatible with (const) char */[]." \
         ),                                                                                                                            \
         ct_error (                                                                                                                    \
             __builtin_classify_type (v) != pointer_type_class,                                                                        \
             "the third argument passed to the set_map() macro must be of pointer or array type."                                      \
         ),                                                                                                                            \
         impl_set_map (                                                                                                                \
             (m), (void *) (uintptr_t) (k),                                                                                            \
             __builtin_choose_expr (                                                                                                   \
                 isint (k), __builtin_choose_expr (sizeof ((k)) * __CHAR_BIT__ <= 32, u32_key, u64_key), str_key                       \
             ),                                                                                                                        \
             (v)                                                                                                                       \
         ))
#endif

#ifdef __cpluplus
    #define put_map(m, k, v)                                                                                                                             \
        (ct_error (                                                                                                                                      \
             !std::is_same<std::remove_const<decltype ((m))>::type, map_t>::value,                                                                       \
             "the type of the first argument passed to the put_map() macro must be compatible with map_t."                                               \
         ),                                                                                                                                              \
         ct_error (                                                                                                                                      \
             !(isint ((k)) || (std::is_pointer<std::decay<decltype (k)>::type>::value &&                                                                 \
                               std::is_same<std::remove_const<std::decay<decltype (k)>::type>::type, char *>::value)),                                   \
             "the second argument passed to the put_map() macro must be either of integral type or compatible with (optionally cv-qualified) char */[]." \
         ),                                                                                                                                              \
         ct_error (                                                                                                                                      \
             std::is_pointer<std::decay<decltype (v)>::type>::value,                                                                                     \
             "the third argument passed to the put_map() macro must be of pointer or array type."                                                        \
         ),                                                                                                                                              \
         impl_put_map (                                                                                                                                  \
             (m), (void *) (uintptr_t) (k),                                                                                                              \
             isint ((k)) ? sizeof ((k)) * __CHAR_BIT__ <= 32 ? u32_key : u64_key : str_key, (v)                                                          \
         ))
#else
    #define put_map(m, k, v)                                                                                                           \
        (ct_error (                                                                                                                    \
             !__builtin_types_compatible_p (typeof ((m)), map_t),                                                                      \
             "the type of the first argument passed to the put_map() macro must be compatible with map_t."                             \
         ),                                                                                                                            \
         ct_error (                                                                                                                    \
             !(isint ((k)) || __builtin_types_compatible_p (typeof ((k)), char *) ||                                                   \
               __builtin_types_compatible_p (typeof ((k)), const char *) ||                                                            \
               __builtin_types_compatible_p (typeof ((k)), char []) ||                                                                 \
               __builtin_types_compatible_p (typeof ((k)), const char [])),                                                            \
             "the second argument passed to the put_map() macro must be either of integral type or compatible with (const) char */[]." \
         ),                                                                                                                            \
         ct_error (                                                                                                                    \
             __builtin_classify_type (v) != pointer_type_class,                                                                        \
             "the third argument passed to the put_map() macro must be of pointer or array type."                                      \
         ),                                                                                                                            \
         impl_put_map (                                                                                                                \
             (m), (void *) (uintptr_t) (k),                                                                                            \
             __builtin_choose_expr (                                                                                                   \
                 isint (k), __builtin_choose_expr (sizeof ((k)) * __CHAR_BIT__ <= 32, u32_key, u64_key), str_key                       \
             ),                                                                                                                        \
             (v)                                                                                                                       \
         ))
#endif

#ifdef __cpluplus
    #define remove_map(m, k)                                                                                                                                \
        (ct_error (                                                                                                                                         \
             !std::is_same<std::remove_const<decltype ((m))>::type, map_t>::value,                                                                          \
             "the type of the first argument passed to the remove_map() macro must be compatible with map_t."                                               \
         ),                                                                                                                                                 \
         ct_error (                                                                                                                                         \
             !(isint ((k)) || (std::is_pointer<std::decay<decltype (k)>::type>::value &&                                                                    \
                               std::is_same<std::remove_const<std::decay<decltype (k)>::type>::type, char *>::value)),                                      \
             "the second argument passed to the remove_map() macro must be either of integral type or compatible with (optionally cv-qualified) char */[]." \
         ),                                                                                                                                                 \
         impl_remove_map (                                                                                                                                  \
             (m), (void *) (uintptr_t) (k),                                                                                                                 \
             isint ((k)) ? sizeof ((k)) * __CHAR_BIT__ <= 32 ? u32_key : u64_key : str_key                                                                  \
         ))
#else
    #define remove_map(m, k)                                                                                                           \
        (ct_error (                                                                                                                    \
             !__builtin_types_compatible_p (typeof ((m)), map_t),                                                                      \
             "the type of the first argument passed to the put_map() macro must be compatible with map_t."                             \
         ),                                                                                                                            \
         ct_error (                                                                                                                    \
             !(isint ((k)) || __builtin_types_compatible_p (typeof ((k)), char *) ||                                                   \
               __builtin_types_compatible_p (typeof ((k)), const char *) ||                                                            \
               __builtin_types_compatible_p (typeof ((k)), char []) ||                                                                 \
               __builtin_types_compatible_p (typeof ((k)), const char [])),                                                            \
             "the second argument passed to the put_map() macro must be either of integral type or compatible with (const) char */[]." \
         ),                                                                                                                            \
         impl_put_map (                                                                                                                \
             (m), (void *) (uintptr_t) (k),                                                                                            \
             __builtin_choose_expr (                                                                                                   \
                 isint ((k)), __builtin_choose_expr (sizeof (k) * __CHAR_BIT__ <= 32, u32_key, u64_key), str_key                       \
             )                                                                                                                         \
         ))
#endif

#ifdef __cplusplus
}
#endif

#endif