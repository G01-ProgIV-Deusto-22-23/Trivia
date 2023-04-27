#ifndef TRIVIA_UI_H
#define TRIVIA_UI_H

#define USE_WIDEC_SUPPORT

// end_ui.c
extern int end_ui (void);

// form.c

#define DEFAULT_FORM_EXIT_KEY     KEY_F (1)
#define DEFAULT_FORM_EXIT_MESSAGE "Pulsa F1 para salir."

#define DEFAULT_FORM_ERASE_KEY     KEY_F (5)
#define DEFAULT_FORM_ERASE_MESSAGE "Pulsa F5 para borrar el contenido de todos los campos."

#define DEFAULT_FORM_SAVE_KEY     '\n'
#define DEFAULT_FORM_SAVE_MESSAGE "Pulsa Intro para guardar los cambios."

#define MAX_FORM_FIELDS    __WORDSIZE
#define MAX_FORM_FIELD_LEN ((size_t) (1 << 12) - 1)

extern field_attr_t impl_field_attrs (const size_t, const FIELDTYPE *const);

#define generic_field(len, ...)                                                                                        \
    (ct_error (NARGS (__VA_ARGS__) > 1, "the generic_field() macro accepts either one or two arguments."),             \
     ct_error (                                                                                                        \
         !(isint (len) && isint (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 0))),                                               \
         "both arguments passed to the generic_field() macro must be integers."                                        \
     ),                                                                                                                \
     ({                                                                                                                \
         field_attr_t __generic_field_fa__        = impl_field_attrs (len, NULL);                                      \
         __generic_field_fa__.type_args.alnum.min = __builtin_choose_expr (                                            \
             isint (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 0)), ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 0), 0                     \
         );                                                                                                            \
         __generic_field_fa__;                                                                                         \
     }))

#define alnum_field(len, ...)                                                                                          \
    (ct_error (NARGS (__VA_ARGS__) > 1, "the alnum_field() macro accepts either one or two arguments."),               \
     ct_error (                                                                                                        \
         !(isint (len) && isint (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 0))),                                               \
         "both arguments passed to the alnum_field() macro must be integers."                                          \
     ),                                                                                                                \
     ({                                                                                                                \
         field_attr_t __alnum_field_fa__        = impl_field_attrs (len, TYPE_ALNUM);                                  \
         __alnum_field_fa__.type_args.alnum.min = __builtin_choose_expr (                                              \
             isint (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 0)), ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 0), 0                     \
         );                                                                                                            \
         __alnum_field_fa__;                                                                                           \
     }))

#define alpha_field(len, ...)                                                                                          \
    (ct_error (NARGS (__VA_ARGS__) > 1, "the alpha_field() macro accepts either one or two arguments."),               \
     ct_error (                                                                                                        \
         !(isint (len) && isint (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 0))),                                               \
         "both arguments passed to the alpha_field() macro must be integers."                                          \
     ),                                                                                                                \
     ({                                                                                                                \
         field_attr_t __alpha_field_fa__        = impl_field_attrs (len, TYPE_ALPHA);                                  \
         __alpha_field_fa__.type_args.alnum.min = __builtin_choose_expr (                                              \
             isint (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 0)), ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 0), 0                     \
         );                                                                                                            \
         __alpha_field_fa__;                                                                                           \
     }))

#define passwd_field(len, ...)                                                                                         \
    (ct_error (NARGS (__VA_ARGS__) > 1, "the passwd_field() macro accepts either one or two arguments."),              \
     ct_error (                                                                                                        \
         !(isint (len) && isint (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 0))),                                               \
         "both arguments passed to the passwd_field() macro must be integers."                                         \
     ),                                                                                                                \
     ({                                                                                                                \
         field_attr_t __passwd_field_fa__           = impl_field_attrs (len, NULL);                                    \
         __passwd_field_fa__.type_args.alnum.passwd = true;                                                            \
         __passwd_field_fa__.type_args.alnum.min    = __builtin_choose_expr (                                          \
             isint (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 0)), ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 0), 0                  \
         );                                                                                                         \
         __passwd_field_fa__;                                                                                          \
     }))

#define int_field(len, ...)                                                                                            \
    (ct_error (NARGS (__VA_ARGS__) > 2, "the int_field() macro accepts between one and three arguments."),             \
     ct_error (                                                                                                        \
         !(isint (len) && isint (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 0)) &&                                              \
           isint (ARG2 (__VA_ARGS__ __VA_OPT__ (, ) 0, 0))),                                                           \
         "all arguments passed to the int_field() macro must be integers."                                             \
     ),                                                                                                                \
     ({                                                                                                                \
         _Pragma ("GCC diagnostic push");                                                                              \
         _Pragma ("GCC diagnostic ignored \"-Wshadow=local\"");                                                        \
         field_attr_t __int_field_fa__ = impl_field_attrs (len, TYPE_INTEGER);                                         \
         _Pragma ("GCC diagnostic pop");                                                                               \
         __builtin_choose_expr (                                                                                       \
             NARGS (__VA_ARGS__) >= 2,                                                                                 \
             (__int_field_fa__.type_args.integer.min = __builtin_choose_expr (                                         \
                  isint (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 0)), ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 0), 0                \
              ),                                                                                                       \
              __int_field_fa__.type_args.integer.max = __builtin_choose_expr (                                         \
                  isint (ARG2 (__VA_ARGS__ __VA_OPT__ (, ) LONG_MAX, LONG_MAX)),                                       \
                  ARG2 (__VA_ARGS__ __VA_OPT__ (, ) LONG_MAX, LONG_MAX), LONG_MAX                                      \
              )),                                                                                                      \
             (void) 0                                                                                                  \
         );                                                                                                            \
         __int_field_fa__;                                                                                             \
     }))

#define ipv4_field() impl_field_attrs (15, TYPE_IPV4)

#if defined(__cpp_attributes) || __STDC_VERSION__ > 201710L
[[nodiscard]]
#else
__attribute__ ((warn_unused_result))
#endif
size_t
    impl_create_form (
        uint32_t, uint32_t, uint32_t, uint32_t, const size_t, const field_attr_t *const,
        const char *const *const restrict
    );

#define create_form(w, h, x, y, attrs, ...)                                                                            \
    (ct_error (NARGS (__VA_ARGS__) > 1, "the create_form() macro admits either 5 or 6 arguments."),                    \
     ct_error (                                                                                                        \
         !(isint (w) && isint (h) && isint (x) && isint (y)),                                                          \
         "the first four arguments passed to the create_form() macro must be of integer type."                         \
     ),                                                                                                                \
     ct_error (                                                                                                        \
         !(__builtin_types_compatible_p (typeof (attrs), field_attr_t []) ||                                           \
           __builtin_types_compatible_p (typeof (attrs), const field_attr_t [])),                                      \
         "the fifth argument passed to the create_form() macro must be a (const) field_attr_t []."                     \
     ),                                                                                                                \
     ({                                                                                                                \
         _Pragma ("GCC diagnostic push");                                                                              \
         _Pragma ("GCC diagnostic ignored \"-Wdiscarded-qualifiers\"");                                                \
         ct_error (                                                                                                    \
             __builtin_classify_type (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) NULL)) != pointer_type_class,                  \
             "the sixth argument passed to the create_form() macro must be of pointer type."                           \
         );                                                                                                            \
         _Pragma ("GCC diagnostic pop");                                                                               \
     }),                                                                                                               \
     impl_create_form (                                                                                                \
         __builtin_choose_expr (isint (w), w, 0), __builtin_choose_expr (isint (h), h, 0),                             \
         __builtin_choose_expr (isint (x), x, 0), __builtin_choose_expr (isint (y), y, 0),                             \
         arrsize (__builtin_choose_expr (                                                                              \
             __builtin_types_compatible_p (typeof (attrs), field_attr_t []) ||                                         \
                 __builtin_types_compatible_p (typeof (attrs), const field_attr_t []),                                 \
             attrs, ((char []) {})                                                                                     \
         )),                                                                                                           \
         (const field_attr_t *) __builtin_choose_expr (                                                                \
             __builtin_classify_type (attrs) == pointer_type_class, attrs, NULL                                        \
         ),                                                                                                            \
         (const char *const *) __builtin_choose_expr (                                                                 \
             __builtin_classify_type (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) NULL)) == pointer_type_class,                  \
             ARG1 (__VA_ARGS__ __VA_OPT__ (, ) NULL), NULL                                                             \
         )                                                                                                             \
     ))

#if defined(__cpp_attributes) || __STDC_VERSION__ > 201710L
[[nonnull (2)]]
#else
__attribute__ ((nonnull (2)))
#endif
size_t
    impl_display_form (const size_t, const char *const restrict);

#define display_form(form, ...)                                                                                        \
    (ct_error (NARGS (__VA_ARGS__) > 1, "the display_form() macro admits either one or two arguments."),               \
     ct_error (!isint (form), "the first argument passed to the display_form() macro must be of integer type."),       \
     ct_error (                                                                                                        \
         __builtin_classify_type (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) != pointer_type_class,                        \
         "the second argument passed to the display_form() macro must be of pointer or array type."                    \
     ),                                                                                                                \
     impl_display_form (                                                                                               \
         form, __builtin_choose_expr (                                                                                 \
                   __builtin_classify_type (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) == pointer_type_class,              \
                   ARG1 (__VA_ARGS__ __VA_OPT__ (, ) ""), ""                                                           \
               )                                                                                                       \
                   ? __builtin_choose_expr (                                                                           \
                         __builtin_classify_type (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) == pointer_type_class,        \
                         ARG1 (__VA_ARGS__ __VA_OPT__ (, ) ""), ""                                                     \
                     )                                                                                                 \
                   : ""                                                                                                \
     ))

#define form(w, h, x, y, attrs, ...)                                                                                   \
    (ct_error (NARGS (__VA_ARGS__) > 2, "the form() macro admits between 5 and 7 arguments."), ({                      \
         _Pragma ("GCC diagnostic push");                                                                              \
         _Pragma ("GCC diagnostic ignored \"-Wdiscarded-qualifiers\"");                                                \
         size_t __form_form__ = create_form (w, h, x, y, attrs, ARG1 (__VA_ARGS__ __VA_OPT__ (, ) NULL));              \
         _Pragma ("GCC diagnostic pop");                                                                               \
         display_form (__form_form__, ARG2 (__VA_ARGS__ __VA_OPT__ (, ) "", ""));                                      \
     }))

// extern const char *const *get_form_data (const size_t);
extern const char (*get_form_data (const size_t)) [MAX_FORM_FIELD_LEN + 1];
extern void set_form_data (const size_t, const char *const *const restrict);

extern int         get_form_exit_key (void);
extern const char *get_form_exit_message (void);
extern int         set_form_exit_key (const int, const char *const restrict);

extern int         get_form_erase_key (void);
extern const char *get_form_erase_message (void);
extern int         set_form_erase_key (const int, const char *const restrict);

extern int         get_form_save_key (void);
extern const char *get_form_save_message (void);
extern int         set_form_save_key (const int, const char *const restrict);

extern FORM_EXPORT (wchar_t *) _nc_Widen_String (char *, int *);

#define Check_CTYPE_Field(result, buffer, width, ccheck)                                                               \
    while (*buffer && *buffer == ' ')                                                                                  \
        buffer++;                                                                                                      \
    if (*buffer) {                                                                                                     \
        bool     blank = FALSE;                                                                                        \
        int      len;                                                                                                  \
        int      n;                                                                                                    \
        wchar_t *list = _nc_Widen_String ((char *) buffer, &len);                                                      \
        if (list != 0) {                                                                                               \
            result = TRUE;                                                                                             \
            for (n = 0; n < len; ++n) {                                                                                \
                if (blank) {                                                                                           \
                    if (list [n] != ' ') {                                                                             \
                        result = FALSE;                                                                                \
                        break;                                                                                         \
                    }                                                                                                  \
                } else if (list [n] == ' ') {                                                                          \
                    blank  = TRUE;                                                                                     \
                    result = (n + 1 >= width);                                                                         \
                } else if (!ccheck (list [n], NULL)) {                                                                 \
                    result = FALSE;                                                                                    \
                    break;                                                                                             \
                }                                                                                                      \
            }                                                                                                          \
            free (list);                                                                                               \
        }                                                                                                              \
    }

// halfdelay_secs.c

#define DEFAULT_HALFDELAY_SECS 2

extern int get_halfdelay_secs (void);
extern int set_halfdelay_secs (const int);

// log_window.c

extern int get_log_file (void);
extern int impl_set_log_file (const int, const bool);
extern int temp_log_file (void);
extern int open_log_file (void);
extern int close_log_file (void);

#ifdef __cplusplus
#else
    #ifdef _WIN32
        #define set_log_file(x)                                                                                                                                           \
            (ct_error (                                                                                                                                                   \
                 !(__builtin_types_compatible_p (typeof (x), char *) ||                                                                                                   \
                   __builtin_types_compatible_p (typeof (x), const char *) ||                                                                                             \
                   __builtin_types_compatible_p (typeof (x), char []) ||                                                                                                  \
                   __builtin_types_compatible_p (typeof (x), const char []) || isint (x)),                                                                                \
                 "the set_log_file() macro must receive either a (const) char pointer/array that represents a file path or an integer that represents a file descriptor." \
             ),                                                                                                                                                           \
             impl_set_log_file (                                                                                                                                          \
                 ({                                                                                                                                                       \
                     _Pragma ("GCC diagnostic push");                                                                                                                     \
                     _Pragma ("GCC diagnostic ignored \"-Wshadow=local\"");                                                                                               \
                     _Pragma ("GCC diagnostic ignored \"-Wshadow=compatible-local\"");                                                                                    \
                     FILE *const __set_log_file_file__ = __builtin_choose_expr (                                                                                          \
                         isint (x), _fdopen (__builtin_choose_expr (isint (x), x, -1), "w+"),                                                                             \
                         fopen (                                                                                                                                          \
                             __builtin_choose_expr (                                                                                                                      \
                                 __builtin_types_compatible_p (typeof (x), char *) ||                                                                                     \
                                     __builtin_types_compatible_p (typeof (x), const char *) ||                                                                           \
                                     __builtin_types_compatible_p (typeof (x), char []) ||                                                                                \
                                     __builtin_types_compatible_p (typeof (x), const char []),                                                                            \
                                 x, ""                                                                                                                                    \
                             ),                                                                                                                                           \
                             "w+"                                                                                                                                         \
                         )                                                                                                                                                \
                     );                                                                                                                                                   \
                     _Pragma ("GCC diagnostic pop");                                                                                                                      \
                     __set_log_file_file__ ? _fileno (__set_log_file_file__) : _fileno (stderr);                                                                          \
                 }),                                                                                                                                                      \
                 !(x || strlen (                                                                                                                                          \
                            __builtin_choose_expr (                                                                                                                       \
                                __builtin_types_compatible_p (typeof (x), char *) ||                                                                                      \
                                    __builtin_types_compatible_p (typeof (x), const char *) ||                                                                            \
                                    __builtin_types_compatible_p (typeof (x), char []) ||                                                                                 \
                                    __builtin_types_compatible_p (typeof (x), const char []),                                                                             \
                                x, ""                                                                                                                                     \
                            )                                                                                                                                             \
                                ? __builtin_choose_expr (                                                                                                                 \
                                      __builtin_types_compatible_p (typeof (x), char *) ||                                                                                \
                                          __builtin_types_compatible_p (typeof (x), const char *) ||                                                                      \
                                          __builtin_types_compatible_p (typeof (x), char []) ||                                                                           \
                                          __builtin_types_compatible_p (typeof (x), const char []),                                                                       \
                                      x, ""                                                                                                                               \
                                  )                                                                                                                                       \
                                : ""                                                                                                                                      \
                        ))                                                                                                                                                \
             ))
    #else
        #define set_log_file(x)                                                                                                                                           \
            (ct_error (                                                                                                                                                   \
                 !(__builtin_types_compatible_p (typeof (x), char *) ||                                                                                                   \
                   __builtin_types_compatible_p (typeof (x), const char *) ||                                                                                             \
                   __builtin_types_compatible_p (typeof (x), char []) ||                                                                                                  \
                   __builtin_types_compatible_p (typeof (x), const char []) || isint (x)),                                                                                \
                 "the set_log_file() macro must receive either a (const) char pointer/array that represents a file path or an integer that represents a file descriptor." \
             ),                                                                                                                                                           \
             impl_set_log_file (                                                                                                                                          \
                 ({                                                                                                                                                       \
                     _Pragma ("GCC diagnostic push");                                                                                                                     \
                     _Pragma ("GCC diagnostic ignored \"-Wshadow=local\"");                                                                                               \
                     _Pragma ("GCC diagnostic ignored \"-Wshadow=compatible-local\"");                                                                                    \
                     FILE *const __impl_set_log_file__ = __builtin_choose_expr (                                                                                          \
                         isint (x), fdopen (__builtin_choose_expr (isint (x), x, -1), "w+"),                                                                              \
                         fopen (                                                                                                                                          \
                             __builtin_choose_expr (                                                                                                                      \
                                 __builtin_types_compatible_p (typeof (x), char *) ||                                                                                     \
                                     __builtin_types_compatible_p (typeof (x), const char *) ||                                                                           \
                                     __builtin_types_compatible_p (typeof (x), char []) ||                                                                                \
                                     __builtin_types_compatible_p (typeof (x), const char []),                                                                            \
                                 x, ""                                                                                                                                    \
                             ),                                                                                                                                           \
                             "w+"                                                                                                                                         \
                         )                                                                                                                                                \
                     );                                                                                                                                                   \
                     _Pragma ("GCC diagnostic pop");                                                                                                                      \
                     __impl_set_log_file__ ? fileno (__impl_set_log_file__) : -1;                                                                                         \
                 }),                                                                                                                                                      \
                 !(x || strlen (                                                                                                                                          \
                            __builtin_choose_expr (                                                                                                                       \
                                __builtin_types_compatible_p (typeof (x), char *) ||                                                                                      \
                                    __builtin_types_compatible_p (typeof (x), const char *) ||                                                                            \
                                    __builtin_types_compatible_p (typeof (x), char []) ||                                                                                 \
                                    __builtin_types_compatible_p (typeof (x), const char []),                                                                             \
                                x, ""                                                                                                                                     \
                            )                                                                                                                                             \
                                ? __builtin_choose_expr (                                                                                                                 \
                                      __builtin_types_compatible_p (typeof (x), char *) ||                                                                                \
                                          __builtin_types_compatible_p (typeof (x), const char *) ||                                                                      \
                                          __builtin_types_compatible_p (typeof (x), char []) ||                                                                           \
                                          __builtin_types_compatible_p (typeof (x), const char []),                                                                       \
                                      x, ""                                                                                                                               \
                                  )                                                                                                                                       \
                                : ""                                                                                                                                      \
                        ))                                                                                                                                                \
             ))
    #endif
#endif

#define DEFAULT_LOG_WINDOW_WIDTH  get_term_width ()
#define DEFAULT_LOG_WINDOW_HEIGHT ((uint32_t) __builtin_ceil (get_term_height () / 5.0))

extern bool is_log_window (void);

extern WINDOW *get_log_window (void);
extern int     refresh_log_window (void);
extern int     clear_log_window (void);
extern WINDOW *create_log_window (void);
extern bool    delete_log_window (void);

extern getdimsfunc_t get_log_window_dims;

#define get_log_window_width()  ((uint32_t) (get_log_window_dims () >> 32))
#define get_log_window_height() ((uint32_t) get_log_window_dims ())

extern setdimsfunc_t impl_set_log_window_dims;
#define set_log_window_dims(...)                                                                                       \
    (ct_error (NARGS (__VA_ARGS__) > 2, "the set_log_window_dims() macro accepts between zero and two arguments."),    \
     setdims (                                                                                                         \
         impl_set_log_window_dims, DEFAULT_LOG_WINDOW_WIDTH, DEFAULT_LOG_WINDOW_HEIGHT __VA_OPT__ (, ) __VA_ARGS__     \
     ))
#define set_log_window_width(...)                                                                                      \
    (ct_error (                                                                                                        \
         NARGS (__VA_ARGS__) > 1, "the set_log_window_width() macro accepts either no arguments or a single argument." \
     ),                                                                                                                \
     setwidth (                                                                                                        \
         impl_set_log_window_dims, DEFAULT_LOG_WINDOW_WIDTH, get_log_window_height () __VA_OPT__ (, __VA_ARGS__)       \
     ))
#define set_log_window_height(...)                                                                                     \
    (ct_error (                                                                                                        \
         NARGS (__VA_ARGS__) > 1,                                                                                      \
         "the set_log_window_height() macro accepts either no arguments or a single argument."                         \
     ),                                                                                                                \
     setheight (                                                                                                       \
         impl_set_log_window_dims, get_log_window_width (), DEFAULT_LOG_WINDOW_HEIGHT __VA_OPT__ (, __VA_ARGS__)       \
     ))

extern int inc_last_log_line (void);

// menu.c

#define DEFAULT_MENUMARK "> "

extern size_t impl_create_menu (
    const menutype_t, uint32_t, uint32_t, uint32_t, uint32_t, size_t, const char *const (*const restrict) [2],
    const size_t (*const restrict) [2], choicefunc_t *const *const restrict
);
#ifdef __cplusplus
#else
    #define create_menu(t, w, h, x, y, c, ...)                                                                                                          \
        /* NOLINT */                                                                                                                                    \
        (ct_error (NARGS (__VA_ARGS__) > 1, "the create_menu() macro accepts between 5 and 6 arguments."),                                              \
         ct_error (                                                                                                                                     \
             !(isint (t) && isint (w) && isint (h) && isint (x) && isint (y)),                                                                          \
             "the first four arguments passed to the create_menu() macro must be of integral type."                                                     \
         ),                                                                                                                                             \
         ct_error (                                                                                                                                     \
             __builtin_choose_expr (__builtin_constant_p (t), t, actionmenu) < actionmenu ||                                                            \
                 __builtin_choose_expr (__builtin_constant_p (t), t, actionmenu) > multimenu,                                                           \
             "the type of menu specified is not a valid menu type."                                                                                     \
         ),                                                                                                                                             \
         ct_error (                                                                                                                                     \
             !(__builtin_types_compatible_p (typeof (c), char *[]) ||                                                                                   \
               __builtin_types_compatible_p (typeof (c), char *const []) ||                                                                             \
               __builtin_types_compatible_p (typeof (c), const char *[]) ||                                                                             \
               __builtin_types_compatible_p (typeof (c), const char *const []) ||                                                                       \
               __builtin_types_compatible_p (typeof (c), char *[][2]) ||                                                                                \
               __builtin_types_compatible_p (typeof (c), char *const [][2]) ||                                                                          \
               __builtin_types_compatible_p (typeof (c), const char *[][2]) ||                                                                          \
               __builtin_types_compatible_p (typeof (c), const char *const [][2])),                                                                     \
             "the fifth argument passed to the create_menu() macro must be an array of (const) char* (const) or an array of (const) char* (const) [2]." \
         ),                                                                                                                                             \
         ct_error (                                                                                                                                     \
             !(__builtin_types_compatible_p (typeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) ((choicefunc_t **) NULL))), choicefunc_t **) ||                  \
               __builtin_types_compatible_p (typeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) ((choicefunc_t **) NULL))), choicefunc_t *const *) ||            \
               __builtin_types_compatible_p (                                                                                                           \
                   typeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) ((choicefunc_t **) NULL))), choicefunc_t *[]                                               \
               ) ||                                                                                                                                     \
               __builtin_types_compatible_p (                                                                                                           \
                   typeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) ((choicefunc_t **) NULL))), choicefunc_t *const []                                         \
               )),                                                                                                                                      \
             "the sixth argument passed to the create_menu() macro must be a pointer or array to choicefunc_t *(const). "                               \
         ),                                                                                                                                             \
         ({                                                                                                                                             \
             _Pragma ("GCC diagnostic push");                                                                                                           \
             _Pragma ("GCC diagnostic ignored \"-Wshadow=local\"");                                                                                     \
             _Pragma ("GCC diagnostic ignored \"-Wshadow=compatible-local\"");                                                                          \
             __builtin_choose_expr (__builtin_constant_p (t), (void) 0, ({                                                                              \
                                        if (__builtin_choose_expr (isint (t), t, actionmenu) < actionmenu ||                                            \
                                            __builtin_choose_expr (isint (t), t, actionmenu) > multimenu)                                               \
                                            error ("the type of menu specified is not a valid menu type.");                                             \
                                    }));                                                                                                                \
             const size_t __create_menu_arrsz__ = sizeof (c) / sizeof (*c);                                                                             \
             if (!__create_menu_arrsz__)                                                                                                                \
                 error ("menus must have at least one item.");                                                                                          \
             size_t *const restrict __create_menu_lens__ = alloca (__create_menu_arrsz__ * 2 * sizeof (size_t));                                        \
             const char *const *const restrict __create_menu_choices__ = (const char *const *) __builtin_choose_expr (                                  \
                 __builtin_types_compatible_p (typeof (c), char *[][2]) ||                                                                              \
                     __builtin_types_compatible_p (typeof (c), char *const [][2]) ||                                                                    \
                     __builtin_types_compatible_p (typeof (c), const char *[][2]) ||                                                                    \
                     __builtin_types_compatible_p (typeof (c), const char *const [][2]),                                                                \
                 ({                                                                                                                                     \
                     for (size_t __create_menu_iter__ = 0; __create_menu_iter__ < __create_menu_arrsz__;                                                \
                          __create_menu_iter__++) {                                                                                                     \
                         *(__create_menu_lens__ + __create_menu_iter__ * 2) = strlen (                                                                  \
                             *((const char *const *) __builtin_choose_expr (                                                                            \
                                   __builtin_classify_type (c) == pointer_type_class, c, NULL                                                           \
                               ) +                                                                                                                      \
                               __create_menu_iter__ * 2)                                                                                                \
                         );                                                                                                                             \
                         *(__create_menu_lens__ + __create_menu_iter__ * 2 + 1) = strlen (                                                              \
                             *((const char *const *) __builtin_choose_expr (                                                                            \
                                   __builtin_classify_type (c) == pointer_type_class, c, NULL                                                           \
                               ) +                                                                                                                      \
                               __create_menu_iter__ * 2 + 1)                                                                                            \
                         );                                                                                                                             \
                     }                                                                                                                                  \
                     memcpy (                                                                                                                           \
                         alloca (__create_menu_arrsz__ * 2 * sizeof (const char *)),                                                                    \
                         __builtin_choose_expr (__builtin_classify_type (c) == pointer_type_class, c, NULL),                                            \
                         __create_menu_arrsz__ * 2 * sizeof (const char *)                                                                              \
                     );                                                                                                                                 \
                 }),                                                                                                                                    \
                 ({                                                                                                                                     \
                     char **__create_menu_choices__ = alloca (__create_menu_arrsz__ * 2 * sizeof (const char *));                                       \
                     for (size_t __create_menu_iter__ = 0; __create_menu_iter__ < __create_menu_arrsz__;                                                \
                          __create_menu_iter__++) {                                                                                                     \
                         sprintf (                                                                                                                      \
                             *(__create_menu_choices__ + __create_menu_iter__ * 2) = alloca (                                                           \
                                 *(__create_menu_lens__ + __create_menu_iter__ * 2) =                                                                   \
                                     (decplaces (__create_menu_iter__ + 1)) + 1                                                                         \
                             ),                                                                                                                         \
                             "%" PRISZ, __create_menu_iter__ + 1                                                                                        \
                         );                                                                                                                             \
                         *(__create_menu_lens__ + __create_menu_iter__ * 2 + 1) = strlen (                                                              \
                             *((const char **) __create_menu_choices__ + __create_menu_iter__ * 2 + 1) =                                                \
                                 (const char *) *(                                                                                                      \
                                     __builtin_choose_expr (                                                                                            \
                                         __builtin_classify_type (c) == pointer_type_class, c, ((char *[0]) {})                                         \
                                     ) +                                                                                                                \
                                     __create_menu_iter__                                                                                               \
                                 )                                                                                                                      \
                         );                                                                                                                             \
                     }                                                                                                                                  \
                     __create_menu_choices__;                                                                                                           \
                 })                                                                                                                                     \
             );                                                                                                                                         \
             size_t __create_menu_ret__ = impl_create_menu (                                                                                            \
                 __builtin_choose_expr (isint (t), t, actionmenu), w, h, x, y, __create_menu_arrsz__,                                                   \
                 (const char *const (*const restrict) [2]) __create_menu_choices__,                                                                     \
                 (const size_t (*const restrict) [2]) __create_menu_lens__,                                                                             \
                 (choicefunc_t *const *const restrict) __builtin_choose_expr (                                                                          \
                     NARGS (__VA_ARGS__), ARG1 (__VA_ARGS__ __VA_OPT__ (, ) ((choicefunc_t **) NULL)), ({                                               \
                         choicefunc_t **const restrict __create_menu_f__ =                                                                              \
                             alloca (__create_menu_arrsz__ * sizeof (choicefunc_t *));                                                                  \
                         memset (__create_menu_f__, 0, __create_menu_arrsz__ * sizeof (choicefunc_t *));                                                \
                         __create_menu_f__;                                                                                                             \
                     })                                                                                                                                 \
                 )                                                                                                                                      \
             );                                                                                                                                         \
             _Pragma ("GCC diagnostic pop");                                                                                                            \
             __create_menu_ret__;                                                                                                                       \
         }))
#endif

extern size_t impl_display_menu (const size_t, const char *const restrict, const char *const restrict);
#define display_menu(m, ...)                                                                                           \
    (ct_error (NARGS (__VA_ARGS__) > 2, "the display_menu() macro must be passed between one and three arguments."),   \
     ct_error (!isint (m), "the first argument passed to the display_menu() macro must be of integral_type."),         \
     ct_error (                                                                                                        \
         __builtin_classify_type (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) != pointer_type_class,                        \
         "the second argument passed to the display_menu() macro must be of pointer or array type."                    \
     ),                                                                                                                \
     ct_error (                                                                                                        \
         __builtin_classify_type (ARG2 (__VA_ARGS__ __VA_OPT__ (, ) DEFAULT_MENUMARK, DEFAULT_MENUMARK)) !=            \
             pointer_type_class,                                                                                       \
         "the third argument passed to the display_menu() macro must be of pointer or array type."                     \
     ),                                                                                                                \
     impl_display_menu (                                                                                               \
         m, (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) ? (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) : "",                    \
         (ARG2 (__VA_ARGS__ __VA_OPT__ (, ) DEFAULT_MENUMARK, DEFAULT_MENUMARK))                                       \
             ? (ARG2 (__VA_ARGS__ __VA_OPT__ (, ) DEFAULT_MENUMARK, DEFAULT_MENUMARK))                                 \
             : DEFAULT_MENUMARK                                                                                        \
     ))

#ifdef __cplusplus
#else
    #define impl_create_display_menu(macro, t, w, h, x, y, c, ...)                                                                                                                                                                                                                                                                                                                                                                                                                               \
        /* NOLINT */ (                                                                                                                                                                                                                                                                                                                                                                                                                                                                           \
            ct_error (NARGS (__VA_ARGS__) > 3, "the " macro "() macro accepts between 5 and 8 arguments."),                                                                                                                                                                                                                                                                                                                                                                                      \
            ct_error (                                                                                                                                                                                                                                                                                                                                                                                                                                                                           \
                !(((__builtin_types_compatible_p (typeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")), char *) ||                                                                                                                                                                                                                                                                                                                                                                                     \
                    __builtin_types_compatible_p (typeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")), const char *) ||                                                                                                                                                                                                                                                                                                                                                                               \
                    __builtin_types_compatible_p (typeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")), char []) ||                                                                                                                                                                                                                                                                                                                                                                                    \
                    __builtin_types_compatible_p (typeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")), const char [])) &&                                                                                                                                                                                                                                                                                                                                                                             \
                   (__builtin_types_compatible_p (typeof (ARG2 (__VA_ARGS__ __VA_OPT__ (, ) "", "")), char *) ||                                                                                                                                                                                                                                                                                                                                                                                 \
                    __builtin_types_compatible_p (typeof (ARG2 (__VA_ARGS__ __VA_OPT__ (, ) "", "")), const char *) ||                                                                                                                                                                                                                                                                                                                                                                           \
                    __builtin_types_compatible_p (typeof (ARG2 (__VA_ARGS__ __VA_OPT__ (, ) "", "")), char []) ||                                                                                                                                                                                                                                                                                                                                                                                \
                    __builtin_types_compatible_p (typeof (ARG2 (__VA_ARGS__ __VA_OPT__ (, ) "", "")), const char []))                                                                                                                                                                                                                                                                                                                                                                            \
                  ) ||                                                                                                                                                                                                                                                                                                                                                                                                                                                                           \
                  ((__builtin_types_compatible_p (typeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) ((choicefunc_t **) NULL))), choicefunc_t **) ||                                                                                                                                                                                                                                                                                                                                                      \
                    __builtin_types_compatible_p (typeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) ((choicefunc_t **) NULL))), choicefunc_t *const *) ||                                                                                                                                                                                                                                                                                                                                                \
                    __builtin_types_compatible_p (                                                                                                                                                                                                                                                                                                                                                                                                                                               \
                        typeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) ((choicefunc_t **) NULL))), choicefunc_t *[]                                                                                                                                                                                                                                                                                                                                                                                   \
                    ) ||                                                                                                                                                                                                                                                                                                                                                                                                                                                                         \
                    __builtin_types_compatible_p (                                                                                                                                                                                                                                                                                                                                                                                                                                               \
                        typeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) ((choicefunc_t **) NULL))), choicefunc_t *const []                                                                                                                                                                                                                                                                                                                                                                             \
                    )) &&                                                                                                                                                                                                                                                                                                                                                                                                                                                                        \
                   (__builtin_types_compatible_p (typeof (ARG2 (__VA_ARGS__ __VA_OPT__ (, ) "", "")), char *) ||                                                                                                                                                                                                                                                                                                                                                                                 \
                    __builtin_types_compatible_p (typeof (ARG2 (__VA_ARGS__ __VA_OPT__ (, ) "", "")), const char *) ||                                                                                                                                                                                                                                                                                                                                                                           \
                    __builtin_types_compatible_p (typeof (ARG2 (__VA_ARGS__ __VA_OPT__ (, ) "", "")), char []) ||                                                                                                                                                                                                                                                                                                                                                                                \
                    __builtin_types_compatible_p (typeof (ARG2 (__VA_ARGS__ __VA_OPT__ (, ) "", "")), const char [])                                                                                                                                                                                                                                                                                                                                                                             \
                   ) &&                                                                                                                                                                                                                                                                                                                                                                                                                                                                          \
                   (__builtin_types_compatible_p (typeof (ARG3 (__VA_ARGS__ __VA_OPT__ (, ) "", "", "")), char *) ||                                                                                                                                                                                                                                                                                                                                                                             \
                    __builtin_types_compatible_p (typeof (ARG3 (__VA_ARGS__ __VA_OPT__ (, ) "", "", "")), const char *) ||                                                                                                                                                                                                                                                                                                                                                                       \
                    __builtin_types_compatible_p (typeof (ARG3 (__VA_ARGS__ __VA_OPT__ (, ) "", "", "")), char []) ||                                                                                                                                                                                                                                                                                                                                                                            \
                    __builtin_types_compatible_p (                                                                                                                                                                                                                                                                                                                                                                                                                                               \
                        typeof (ARG3 (__VA_ARGS__ __VA_OPT__ (, ) "", "", "")), const char []                                                                                                                                                                                                                                                                                                                                                                                                    \
                    )))),                                                                                                                                                                                                                                                                                                                                                                                                                                                                        \
                "the final arguments passed to the " macro                                                                                                                                                                                                                                                                                                                                                                                                                                       \
                "() macro must be either (..., funcs, title), (..., funcs, title, menumark), (..., title) or (..., title, menumark), being title and menumark (const) char */[] and funcs choicefunc_t *(const) */[]."                                                                                                                                                                                                                                                                           \
            ),                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   \
            (                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    \
                {                                                                                                                                                                                                                                                                                                                                                                                                                                                                                \
                    const size_t __impl_create_display_menu_var__ = create_menu (                                                                                                                                                                                                                                                                                                                                                                                                                \
                        t, w, h, x, y,                                                                                                                                                                                                                                                                                                                                                                                                                                                           \
                        c, (choicefunc_t *const *const) __builtin_choose_expr (__builtin_types_compatible_p (typeof (*__builtin_choose_expr (__builtin_classify_type (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) ((char *) NULL))) == pointer_type_class, ARG1 (__VA_ARGS__ __VA_OPT__ (, ) ((char *) NULL)), ((char *) NULL))), choicefunc_t *), ARG1 (__VA_ARGS__ __VA_OPT__ (, ) ((choicefunc_t *const *) NULL)), ({                                                                                   \
                                                                                   const size_t                                                                                                                                                                                                                                                                                                                                                                                                  \
                                                                                       __create_menu_arrsz__ =                                                                                                                                                                                                                                                                                                                                                                                   \
                                                                                           sizeof (c) / sizeof (*c);                                                                                                                                                                                                                                                                                                                                                                             \
                                                                                   choicefunc_t *                                                                                                                                                                                                                                                                                                                                                                                                \
                                                                                       *const restrict __create_menu_f__ =                                                                                                                                                                                                                                                                                                                                                                       \
                                                                                           alloca (                                                                                                                                                                                                                                                                                                                                                                                              \
                                                                                               __create_menu_arrsz__ *                                                                                                                                                                                                                                                                                                                                                                           \
                                                                                               sizeof (choicefunc_t *)                                                                                                                                                                                                                                                                                                                                                                           \
                                                                                           );                                                                                                                                                                                                                                                                                                                                                                                                    \
                                                                                   memset (                                                                                                                                                                                                                                                                                                                                                                                                      \
                                                                                       __create_menu_f__, 0,                                                                                                                                                                                                                                                                                                                                                                                     \
                                                                                       __create_menu_arrsz__ *                                                                                                                                                                                                                                                                                                                                                                                   \
                                                                                           sizeof (choicefunc_t *)                                                                                                                                                                                                                                                                                                                                                                               \
                                                                                   );                                                                                                                                                                                                                                                                                                                                                                                                            \
                                                                               }))                                                                                                                                                                                                                                                                                                                                                                                                               \
                    );                                                                                                                                                                                                                                                                                                                                                                                                                                                                           \
                    const char *const *const __impl_create_display_menu_format__ = __builtin_choose_expr (                                                                                                                                                                                                                                                                                                                                                                                       \
                        __builtin_types_compatible_p (typeof (*__builtin_choose_expr (__builtin_classify_type (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) ((char *) NULL))) == pointer_type_class, ARG1 (__VA_ARGS__ __VA_OPT__ (, ) ((char *) NULL)), ((char *) NULL))), choicefunc_t *), ((const char *[]) { (const char *) (void *) (ARG2 (__VA_ARGS__ __VA_OPT__ (, ) "", "")), (const char *) (void *) (ARG3 (__VA_ARGS__ __VA_OPT__ (, ) DEFAULT_MENUMARK, DEFAULT_MENUMARK, DEFAULT_MENUMARK)) }), \
                        ((const char *[]) { (const char *) (void *) (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")),                                                                                                                                                                                                                                                                                                                                                                                     \
                                            (const char *) (void *) ARG2 (                                                                                                                                                                                                                                                                                                                                                                                                                       \
                                                __VA_ARGS__ __VA_OPT__ (, ) DEFAULT_MENUMARK, DEFAULT_MENUMARK                                                                                                                                                                                                                                                                                                                                                                                   \
                                            ) })                                                                                                                                                                                                                                                                                                                                                                                                                                                 \
                    );                                                                                                                                                                                                                                                                                                                                                                                                                                                                           \
                    impl_display_menu (                                                                                                                                                                                                                                                                                                                                                                                                                                                          \
                        __impl_create_display_menu_var__, *__impl_create_display_menu_format__,                                                                                                                                                                                                                                                                                                                                                                                                  \
                        *(__impl_create_display_menu_format__ + 1)                                                                                                                                                                                                                                                                                                                                                                                                                               \
                    );                                                                                                                                                                                                                                                                                                                                                                                                                                                                           \
                }                                                                                                                                                                                                                                                                                                                                                                                                                                                                                \
            )                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    \
        )

#endif

#define actionmenu(w, h, x, y, c, ...)                                                                                 \
    impl_create_display_menu ("actionmenu", actionmenu, w, h, x, y, c __VA_OPT__ (, ) __VA_ARGS__)
#define choicemenu(w, h, x, y, c, ...)                                                                                 \
    impl_create_display_menu ("choicemenu", choicemenu, w, h, x, y, c __VA_OPT__ (, ) __VA_ARGS__)
#define multimenu(w, h, x, y, c, ...)                                                                                  \
    impl_create_display_menu ("multimenu", multimenu, w, h, x, y, c __VA_OPT__ (, ) __VA_ARGS__)

extern size_t *get_menu_ret (const size_t);
extern size_t *set_menu_ret (const size_t, size_t *const restrict);

extern int         get_menu_exit_key (void);
extern const char *get_menu_exit_message (void);
extern int         set_menu_exit_key (const int, const char *const restrict);

#define DEFAULT_MENU_EXIT_KEY     KEY_F (1)
#define DEFAULT_MENU_EXIT_MESSAGE "Pulsa F1 para salir."

// padding.c

#define DEFAULT_PADDING UINT32_C (1)

extern getdimsfunc_t get_padding;

#define get_hor_padding() ((uint32_t) (get_padding () >> 32))
#define get_ver_padding() ((uint32_t) get_padding ())

extern setdimsfunc_t impl_set_padding;
#define set_padding(...)                                                                                               \
    (ct_error (NARGS (__VA_ARGS__) > 2, "the set_padding() macro accepts between zero and two arguments."),            \
     setdims (impl_set_padding, DEFAULT_PADDING, DEFAULT_PADDING __VA_OPT__ (, ) __VA_ARGS__))
#define set_hor_padding(...)                                                                                           \
    (ct_error (                                                                                                        \
         NARGS (__VA_ARGS__) > 1, "the set_hor_padding() macro accepts either no arguments or a single argument."      \
     ),                                                                                                                \
     setwidth (impl_set_padding, DEFAULT_PADDING, get_ver_padding () __VA_OPT__ (, __VA_ARGS__)))
#define set_ver_padding(...)                                                                                           \
    (ct_error (                                                                                                        \
         NARGS (__VA_ARGS__) > 1, "the set_ver_padding() macro accepts either no arguments or a single argument."      \
     ),                                                                                                                \
     setheight (impl_set_padding, get_hor_padding (), DEFAULT_PADDING __VA_OPT__ (, __VA_ARGS__)))

// termdims.c

#define MIN_TERM_WIDTH  UINT32_C (120)
#define MIN_TERM_HEIGHT UINT32_C (50)

extern getdimsfunc_t get_term_dims;
#ifdef _WIN32
extern setdimsfunc_t set_term_dims;
#endif
extern getwidthfunc_t  get_term_width;
extern getheightfunc_t get_term_height;

// setup_ui.c

extern int
    impl_setup_ui (const int, const char *const *const restrict, int, const int, const int, const int, const int);
#ifdef _WIN32
    #define setup_ui(...)                                                                                                                 \
        (ct_error (NARGS (__VA_ARGS__) > 5, "the setup_ui() macro must be passed between 0 and 5 arguments."),                            \
         ct_error (                                                                                                                       \
             !(isint (argc) && __builtin_classify_type (argv) == pointer_type_class),                                                     \
             "the argc and argv variables must be of integral and pointer/array type, respectively."                                      \
         ),                                                                                                                               \
         ct_error (                                                                                                                       \
             !(isint (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 0)) && isint (ARG2 (__VA_ARGS__ __VA_OPT__ (, ) 0, 0)) &&                         \
               isint (ARG3 (__VA_ARGS__ __VA_OPT__ (, ) 0, 0, 0)) &&                                                                      \
               isint (ARG4 (__VA_ARGS__ __VA_OPT__ (, ) 0, 0, 0, 0)) &&                                                                   \
               isint (ARG5 (__VA_ARGS__ __VA_OPT__ (, ) 0, 0, 0, 0, 0))),                                                                 \
             "all the arguments passed to the set_ui_settings () must be of integral type."                                               \
         ),                                                                                                                               \
         ({                                                                                                                               \
             _Pragma ("GCC diagnostic push");                                                                                             \
             _Pragma ("GCC diagnostic ignored \"-Wshadow=local\"");                                                                       \
             _Pragma ("GCC diagnostic ignored \"-Wdiscarded-qualifiers\"");                                                               \
             FreeConsole ();                                                                                                              \
             if (!AttachConsole (ATTACH_PARENT_PROCESS))                                                                                  \
                 if (!AllocConsole ())                                                                                                    \
                     trap (), unreachable ();                                                                                             \
             {                                                                                                                            \
                 FILE *__setup_ui_f__;                                                                                                    \
                 freopen_s (&__setup_ui_f__, "CONIN$", "r", stdin);                                                                       \
                 fclose (__setup_ui_f__);                                                                                                 \
                 freopen_s (&__setup_ui_f__, "CONOUT$", "w", stdout);                                                                     \
                 fclose (__setup_ui_f__);                                                                                                 \
                 freopen_s (&__setup_ui_f__, "CONOUT$", "w", stderr);                                                                     \
                 fclose (__setup_ui_f__);                                                                                                 \
             }                                                                                                                            \
             if (_dup2 (                                                                                                                  \
                     _fileno (_fdopen (_open_osfhandle ((intptr_t) GetStdHandle (STD_INPUT_HANDLE), _O_U8TEXT), "r")),                    \
                     _fileno (stdin)                                                                                                      \
                 ) == -1 ||                                                                                                               \
                 _dup2 (                                                                                                                  \
                     _fileno (_fdopen (_open_osfhandle ((intptr_t) GetStdHandle (STD_OUTPUT_HANDLE), _O_U8TEXT), "w")                     \
                     ),                                                                                                                   \
                     _fileno (stdout)                                                                                                     \
                 ) == -1 ||                                                                                                               \
                 _dup2 (                                                                                                                  \
                     _fileno (_fdopen (_open_osfhandle ((intptr_t) GetStdHandle (STD_ERROR_HANDLE), _O_U8TEXT), "w")),                    \
                     _fileno (stderr)                                                                                                     \
                 ) == -1)                                                                                                                 \
                 trap (), unreachable ();                                                                                                 \
             setvbuf (stdin, NULL, _IONBF, 0);                                                                                            \
             setvbuf (stdout, NULL, _IONBF, 0);                                                                                           \
             setvbuf (stderr, NULL, _IONBF, 0);                                                                                           \
             SetConsoleCP (CP_UTF8);                                                                                                      \
             SetConsoleOutputCP (CP_UTF8);                                                                                                \
             int       __setup_ui_argc__  = 0;                                                                                            \
             char    **__setup_ui_argv__  = NULL;                                                                                         \
             wchar_t **__setup_ui_wargv__ = NULL;                                                                                         \
             int      *__setup_ui_wargl__ = NULL;                                                                                         \
             if (!strcmp (__func__, "main")) {                                                                                            \
                 __setup_ui_argc__ = __builtin_choose_expr (isint (argc), argc, 1) - 1;                                                   \
                 __setup_ui_argv__ =                                                                                                      \
                     (void                                                                                                                \
                          *) (__builtin_choose_expr (__builtin_classify_type (argv) == pointer_type_class, argv, (char *) NULL - 1) + 1); \
             } else if (!strcmp (__func__, "wmain")) {                                                                                    \
                 __setup_ui_argc__ = __builtin_choose_expr (isint (argc), argc, 1);                                                       \
                 __setup_ui_wargv__ =                                                                                                     \
                     (void                                                                                                                \
                          *) (__builtin_choose_expr (__builtin_classify_type (argv) == pointer_type_class, argv, (char *) NULL - 1) + 1); \
                 goto __setup_ui_label__;                                                                                                 \
             } else if (!(strcmp (__func__, "WinMain") && strcmp (__func__, "wWinMain"))) {                                               \
                 if (!(__setup_ui_wargv__ = CommandLineToArgvW (GetCommandLineW (), &__setup_ui_argc__)))                                 \
                     error ("failed to retrieve the program arguments.");                                                                 \
             __setup_ui_label__:                                                                                                          \
                 if (--__setup_ui_argc__) {                                                                                               \
                     if (!(__setup_ui_argv__ = _malloca (sizeof (char *) * (size_t) __setup_ui_argc__)))                                  \
                         error ("failed to allocate memory for the program arguments.");                                                  \
                     if (!(__setup_ui_wargl__ = _malloca (sizeof (size_t) * (size_t) __setup_ui_argc__)))                                 \
                         error ("failed to allocate memory for the program arguments.");                                                  \
                     for (int __setup_ui_iter__ = 0; __setup_ui_iter__ < __setup_ui_argc__; __setup_ui_iter__++) {                        \
                         if (!(*(__setup_ui_argv__ + __setup_ui_iter__) = _malloca (                                                      \
                                   ((size_t                                                                                               \
                                    ) (*(__setup_ui_wargl__ + __setup_ui_iter__) =                                                        \
                                           lstrlenW (*(__setup_ui_wargv__ + __setup_ui_iter__ + 1))) +                                    \
                                    1) *                                                                                                  \
                                   sizeof (wchar_t)                                                                                       \
                               )))                                                                                                        \
                             error ("failed to allocate memory for the program arguments.");                                              \
                     }                                                                                                                    \
                     for (int __setup_ui_iter__ = 0; __setup_ui_iter__ < __setup_ui_argc__; __setup_ui_iter__++)                          \
                         if (!WideCharToMultiByte (                                                                                       \
                                 CP_UTF8, 0, *(__setup_ui_wargv__ + __setup_ui_iter__ + 1),                                               \
                                 *(__setup_ui_wargl__ + __setup_ui_iter__) + 1,                                                           \
                                 *(__setup_ui_argv__ + __setup_ui_iter__),                                                                \
                                 (*(__setup_ui_wargl__ + __setup_ui_iter__) + 1) * (int) sizeof (wchar_t), NULL, NULL                     \
                             ))                                                                                                           \
                             error ("failed to retrieve the program arguments.");                                                         \
                 }                                                                                                                        \
             }                                                                                                                            \
             {                                                                                                                            \
                 char __setup_ui_path__ [MAX_PATH + 1];                                                                                   \
                 if (!strcmp (                                                                                                            \
                         basename (({                                                                                                     \
                             if (!GetModuleFileNameA (NULL, __setup_ui_path__, MAX_PATH + 1))                                             \
                                 error ("could not get the executable filename.");                                                        \
                             __setup_ui_path__;                                                                                           \
                         })),                                                                                                             \
                         "local.exe"                                                                                                      \
                     ))                                                                                                                   \
                     if (!SetWindowTextW (                                                                                                \
                             ({                                                                                                           \
                                 HWND __setup_ui_window_handle__ = GetConsoleWindow ();                                                   \
                                 if (!__setup_ui_window_handle__)                                                                         \
                                     error ("could not retrieve a handle to the console window.");                                        \
                                 __setup_ui_window_handle__;                                                                              \
                             }),                                                                                                          \
                             L"Trivia: servidor local"                                                                                    \
                         ))                                                                                                               \
                         warning ("could not change the window's name");                                                                  \
             }                                                                                                                            \
             int __setup_ui_ret__ = impl_setup_ui (                                                                                       \
                 __setup_ui_argc__, (const char *const *) __setup_ui_argv__,                                                              \
                 __builtin_choose_expr (                                                                                                  \
                     isint (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 0)), ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 0), 0                                \
                 ),                                                                                                                       \
                 __builtin_choose_expr (                                                                                                  \
                     isint (ARG2 (__VA_ARGS__ __VA_OPT__ (, ) 0, 0)), ARG2 (__VA_ARGS__ __VA_OPT__ (, ) 0, 0), 0                          \
                 ),                                                                                                                       \
                 __builtin_choose_expr (                                                                                                  \
                     isint (ARG3 (__VA_ARGS__ __VA_OPT__ (, ) 0, 0, 0)), ARG3 (__VA_ARGS__ __VA_OPT__ (, ) 0, 0, 0), 0                    \
                 ),                                                                                                                       \
                 __builtin_choose_expr (                                                                                                  \
                     isint (ARG4 (__VA_ARGS__ __VA_OPT__ (, ) 0, 0, 0, 0)),                                                               \
                     ARG4 (__VA_ARGS__ __VA_OPT__ (, ) 0, 0, 0, 0), 0                                                                     \
                 ),                                                                                                                       \
                 __builtin_choose_expr (                                                                                                  \
                     isint (ARG5 (__VA_ARGS__ __VA_OPT__ (, ) 0, 0, 0, 0, 0)),                                                            \
                     ARG5 (__VA_ARGS__ __VA_OPT__ (, ) 0, 0, 0, 0, 0), 0                                                                  \
                 )                                                                                                                        \
             );                                                                                                                           \
             _Pragma ("GCC diagnostic pop");                                                                                              \
             if (!(strcmp (__func__, "wmain") && strcmp (__func__, "WinMain") && strcmp (__func__, "wWinMain"))) {                        \
                 for (int __setup_ui_iter__ = 0; __setup_ui_iter__ < __setup_ui_argc__;                                                   \
                      _freea (*(__setup_ui_argv__ + __setup_ui_iter__++)))                                                                \
                     ;                                                                                                                    \
                 _freea (__setup_ui_argv__);                                                                                              \
                 _freea (__setup_ui_wargl__);                                                                                             \
                 if (strcmp (__func__, "wmain"))                                                                                          \
                     LocalFree (__setup_ui_wargv__);                                                                                      \
             }                                                                                                                            \
             __setup_ui_ret__;                                                                                                            \
         }))
#else
    #define setup_ui(...)                                                                                                 \
        (ct_error (NARGS (__VA_ARGS__) > 5, "the setup_ui() macro must be passed between 0 and 5 arguments."),            \
         ct_error (                                                                                                       \
             !(isint (argc) && __builtin_classify_type (argv) == pointer_type_class),                                     \
             "the argc and argv variables must be of integral and pointer/array type, respectively."                      \
         ),                                                                                                               \
         ct_error (                                                                                                       \
             !(isint (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 0)) && isint (ARG2 (__VA_ARGS__ __VA_OPT__ (, ) 0, 0)) &&         \
               isint (ARG3 (__VA_ARGS__ __VA_OPT__ (, ) 0, 0, 0)) &&                                                      \
               isint (ARG4 (__VA_ARGS__ __VA_OPT__ (, ) 0, 0, 0, 0)) &&                                                   \
               isint (ARG5 (__VA_ARGS__ __VA_OPT__ (, ) 0, 0, 0, 0, 0))),                                                 \
             "all the arguments passed to the set_ui_settings () must be of integral type."                               \
         ),                                                                                                               \
         impl_setup_ui (                                                                                                  \
             !strcmp (__func__, "main") ? __builtin_choose_expr (isint (argc), argc, 1) - 1 : 0,                          \
             !strcmp (__func__, "main")                                                                                   \
                 ? (void                                                                                                  \
                        *) (__builtin_choose_expr (__builtin_classify_type (argv) == pointer_type_class, argv, NULL) + 1) \
                 : NULL,                                                                                                  \
             __builtin_choose_expr (                                                                                      \
                 isint (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 0)), ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 0), 0                    \
             ),                                                                                                           \
             __builtin_choose_expr (                                                                                      \
                 isint (ARG2 (__VA_ARGS__ __VA_OPT__ (, ) 0, 0)), ARG2 (__VA_ARGS__ __VA_OPT__ (, ) 0, 0), 0              \
             ),                                                                                                           \
             __builtin_choose_expr (                                                                                      \
                 isint (ARG3 (__VA_ARGS__ __VA_OPT__ (, ) 0, 0, 0)), ARG3 (__VA_ARGS__ __VA_OPT__ (, ) 0, 0, 0), 0        \
             ),                                                                                                           \
             __builtin_choose_expr (                                                                                      \
                 isint (ARG4 (__VA_ARGS__ __VA_OPT__ (, ) 0, 0, 0, 0)), ARG4 (__VA_ARGS__ __VA_OPT__ (, ) 0, 0, 0, 0),    \
                 0                                                                                                        \
             ),                                                                                                           \
             __builtin_choose_expr (                                                                                      \
                 isint (ARG5 (__VA_ARGS__ __VA_OPT__ (, ) 0, 0, 0, 0, 0)),                                                \
                 ARG5 (__VA_ARGS__ __VA_OPT__ (, ) 0, 0, 0, 0, 0), 0                                                      \
             )                                                                                                            \
         ))
#endif

extern int initial_cursor_mode (void);

// ui_settings.c

extern int get_ui_settings (void);

extern int impl_set_ui_settings (const int, const int, const int, const int, const int);
#define set_ui_settings(...)                                                                                           \
    (ct_error (NARGS (__VA_ARGS__) > 5, "the set_ui_settings() macro must be passed between 0 and 5 arguments."),      \
     ct_error (                                                                                                        \
         !(isint ((ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 0))) && isint ((ARG2 (__VA_ARGS__ __VA_OPT__ (, ) 0, 0))) &&      \
           isint ((ARG3 (__VA_ARGS__ __VA_OPT__ (, ) 0, 0, 0))) &&                                                     \
           isint ((ARG4 (__VA_ARGS__ __VA_OPT__ (, ) 0, 0, 0, 0))) &&                                                  \
           isint ((ARG5 (__VA_ARGS__ __VA_OPT__ (, ) 0, 0, 0, 0, 0)))),                                                \
         "all the arguments passed to the set_ui_settings () must be of integral type."                                \
     ),                                                                                                                \
     impl_set_ui_settings (                                                                                            \
         __builtin_choose_expr (                                                                                       \
             isint ((ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 0))), (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 0)), 0                 \
         ),                                                                                                            \
         __builtin_choose_expr (                                                                                       \
             isint ((ARG2 (__VA_ARGS__ __VA_OPT__ (, ) 0, 0))), (ARG2 (__VA_ARGS__ __VA_OPT__ (, ) 0, 0)), 0           \
         ),                                                                                                            \
         __builtin_choose_expr (                                                                                       \
             isint ((ARG3 (__VA_ARGS__ __VA_OPT__ (, ) 0, 0, 0))), (ARG3 (__VA_ARGS__ __VA_OPT__ (, ) 0, 0, 0)), 0     \
         ),                                                                                                            \
         __builtin_choose_expr (                                                                                       \
             isint ((ARG4 (__VA_ARGS__ __VA_OPT__ (, ) 0, 0, 0, 0))), (ARG4 (__VA_ARGS__ __VA_OPT__ (, ) 0, 0, 0, 0)), \
             0                                                                                                         \
         ),                                                                                                            \
         __builtin_choose_expr (                                                                                       \
             isint ((ARG5 (__VA_ARGS__ __VA_OPT__ (, ) 0, 0, 0, 0, 0))),                                               \
             (ARG5 (__VA_ARGS__ __VA_OPT__ (, ) 0, 0, 0, 0, 0)), 0                                                     \
         )                                                                                                             \
     ))

#define ui_settings_list(settings)                                                                                     \
    settings & (3 << 5), settings & (1 << 3), settings & (1 << 2), settings & (1 << 1), settings & 1

#define compact_ui_settings(...)                                                                                       \
    (ct_error (NARGS (__VA_ARGS__) > 5, "the compact_ui_settings() macro must be passed between 0 and 5 arguments."),  \
     ct_error (                                                                                                        \
         !(isint (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 0)) && isint (ARG2 (__VA_ARGS__ __VA_OPT__ (, ) 0, 0)) &&          \
           isint (ARG3 (__VA_ARGS__ __VA_OPT__ (, ) 0, 0, 0)) &&                                                       \
           isint (ARG4 (__VA_ARGS__ __VA_OPT__ (, ) 0, 0, 0, 0)) &&                                                    \
           isint (ARG5 (__VA_ARGS__ __VA_OPT__ (, ) 0, 0, 0, 0, 0))),                                                  \
         "all the arguments passed to the set_ui_settings () must be of integral type."                                \
     ),                                                                                                                \
     (((__builtin_choose_expr (                                                                                        \
            isint (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 0)), ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 0), 0                      \
        ) &                                                                                                            \
        3)                                                                                                             \
       << 5) |                                                                                                         \
      (!!(__builtin_choose_expr (                                                                                      \
           isint (ARG2 (__VA_ARGS__ __VA_OPT__ (, ) 0, 0)), ARG2 (__VA_ARGS__ __VA_OPT__ (, ) 0, 0), 0                 \
       ))                                                                                                              \
       << 3) |                                                                                                         \
      (!!(__builtin_choose_expr (                                                                                      \
           isint (ARG3 (__VA_ARGS__ __VA_OPT__ (, ) 0, 0, 0)), ARG3 (__VA_ARGS__ __VA_OPT__ (, ) 0, 0, 0), 0           \
       ))                                                                                                              \
       << 2) |                                                                                                         \
      (!!(__builtin_choose_expr (                                                                                      \
           isint (ARG4 (__VA_ARGS__ __VA_OPT__ (, ) 0, 0, 0, 0)), ARG4 (__VA_ARGS__ __VA_OPT__ (, ) 0, 0, 0, 0), 0     \
       ))                                                                                                              \
       << 1) |                                                                                                         \
      !!(__builtin_choose_expr (                                                                                       \
          isint (ARG5 (__VA_ARGS__ __VA_OPT__ (, ) 0, 0, 0, 0, 0)), ARG5 (__VA_ARGS__ __VA_OPT__ (, ) 0, 0, 0, 0, 0),  \
          0                                                                                                            \
      ))))

// window.c

extern WINDOW *impl_create_window (uint32_t, uint32_t, uint32_t, uint32_t, const int);
#define create_window(w, h, x, y) impl_create_window (w, h, x, y, !!strcmp (__func__, "create_log_window"))

extern bool delete_window (WINDOW *const restrict);
extern bool delete_menu (const size_t);
extern bool delete_form (const size_t);
extern bool delete_windows (void);

#endif
