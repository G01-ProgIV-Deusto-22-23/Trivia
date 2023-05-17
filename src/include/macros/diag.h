#ifndef TRIVIA_MACROS_DIAG_H
#define TRIVIA_MACROS_DIAG_H

#ifdef __cplusplus

static constexpr int EVAL_CT_ERROR_HELPER_FUNC (int x) noexcept {
    return !!x;
}

    #define EVAL_CT_ERROR_HELPER(x) EVAL_CT_ERROR_HELPER_FUNC (x)

template <std::size_t N> static constexpr bool IS_STRING_LITERAL_FUNC (const char (&) [N]) {
    return true;
}

template <std::size_t N> static constexpr bool IS_STRING_LITERAL_FUNC (char (&) [N]) {
    return false;
}

template <typename T> static constexpr bool IS_STRING_LITERAL_FUNC (T) {
    return false;
}

    #define IS_STRING_LITERAL(x) IS_STRING_LITERAL_FUNC (x)

    #define ct_error(...)                                                                                              \
        ({                                                                                                             \
            static_assert (                                                                                            \
                EVAL_CT_ERROR_HELPER (NARGS (__VA_ARGS__) <= 2),                                                       \
                "the ct_error() macro accepts between zero and two arguments."                                         \
            );                                                                                                         \
            static_assert (                                                                                            \
                EVAL_CT_ERROR_HELPER (                                                                                 \
                    NARGS (__VA_ARGS__) != 2 ||                                                                        \
                    (NARGS (__VA_ARGS__) == 2 && IS_STRING_LITERAL (ARG2 (__VA_ARGS__ __VA_OPT__ (, ) "", "")))        \
                ),                                                                                                     \
                "when passing two arguments to the ct_error() macro, the second argument must be a string literal."    \
            );                                                                                                         \
            static_assert (                                                                                            \
                EVAL_CT_ERROR_HELPER (__builtin_constant_p (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 0))),                    \
                "the first argument passed to the ct_error() macro must be a compile-time constant expression."        \
            );                                                                                                         \
            static_assert (                                                                                            \
                EVAL_CT_ERROR_HELPER (__builtin_constant_p (ARG2 (__VA_ARGS__ __VA_OPT__ (, ) 0, 0))),                 \
                "the second argument passed to the ct_error() macro must be a compile-time constant expression."       \
            );                                                                                                         \
            static_assert (NARGS (__VA_ARGS__));                                                                       \
            static_assert (                                                                                            \
                EVAL_CT_ERROR_HELPER (                                                                                 \
                    NARGS (__VA_ARGS__) != 2 || (NARGS (__VA_ARGS__) == 2 && !(ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 1)))  \
                ),                                                                                                     \
                ARG2 (__VA_ARGS__ __VA_OPT__ (, ) "", "")                                                              \
            );                                                                                                         \
            static_assert (EVAL_CT_ERROR_HELPER (                                                                      \
                NARGS (__VA_ARGS__) != 1 || IS_STRING_LITERAL (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) ||               \
                (NARGS (__VA_ARGS__) == 1 && !(ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 1)))                                  \
            ));                                                                                                        \
            static_assert (EVAL_CT_ERROR_HELPER (                                                                      \
                !(NARGS (__VA_ARGS__) == 1 && IS_STRING_LITERAL (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")))               \
            ) __VA_OPT__ (, stringify (ARG1 (__VA_ARGS__))));                                                          \
        })

#else

    #define EVAL_CT_ERROR_HELPER2(x) __builtin_choose_expr (x, 1, 0)
    #define EVAL_CT_ERROR_HELPER(x)                                                                                    \
        (EVAL_CT_ERROR_HELPER2 (__builtin_choose_expr (__builtin_constant_p (x), x, !"not a compile-time constant.")))

    #define ct_error(...)                                                                                              \
        ({                                                                                                             \
            static_assert (                                                                                            \
                EVAL_CT_ERROR_HELPER (NARGS (__VA_ARGS__) <= 2),                                                       \
                "the ct_error() macro accepts between zero and two arguments."                                         \
            );                                                                                                         \
            static_assert (                                                                                            \
                EVAL_CT_ERROR_HELPER (__builtin_choose_expr (                                                          \
                    NARGS (__VA_ARGS__) == 2,                                                                          \
                    __builtin_types_compatible_p (typeof (ARG2 (__VA_ARGS__ __VA_OPT__ (, ) "", "")), char []) &&      \
                        __builtin_constant_p (ARG2 (__VA_ARGS__ __VA_OPT__ (, ) "", "")),                              \
                    1                                                                                                  \
                )),                                                                                                    \
                "when passing two arguments to the ct_error() macro, the second argument must be a string literal."    \
            );                                                                                                         \
            static_assert (                                                                                            \
                EVAL_CT_ERROR_HELPER (__builtin_constant_p (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 0))),                    \
                "the first argument passed to the ct_error() macro must be a compile-time constant expression."        \
            );                                                                                                         \
            static_assert (                                                                                            \
                EVAL_CT_ERROR_HELPER (__builtin_constant_p (ARG2 (__VA_ARGS__ __VA_OPT__ (, ) 0, 0))),                 \
                "the second argument passed to the ct_error() macro must be a compile-time constant expression."       \
            );                                                                                                         \
            static_assert (NARGS (__VA_ARGS__));                                                                       \
            static_assert (                                                                                            \
                EVAL_CT_ERROR_HELPER (__builtin_choose_expr (                                                          \
                    __builtin_constant_p (                                                                             \
                        __builtin_choose_expr (NARGS (__VA_ARGS__) == 2, !(ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 0)), 1)   \
                    ),                                                                                                 \
                    __builtin_choose_expr (NARGS (__VA_ARGS__) == 2, !(ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 0)), 1), 0    \
                )),                                                                                                    \
                ARG2 (__VA_ARGS__ __VA_OPT__ (, ) "", "")                                                              \
            );                                                                                                         \
            static_assert (EVAL_CT_ERROR_HELPER (__builtin_choose_expr (                                               \
                NARGS (__VA_ARGS__) == 1 &&                                                                            \
                    !(__builtin_types_compatible_p (typeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")), char []) &&        \
                      __builtin_constant_p (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) ""))),                                   \
                !(ARG1 (__VA_ARGS__ __VA_OPT__ (, ) 0)), 1                                                             \
            )));                                                                                                       \
            static_assert (EVAL_CT_ERROR_HELPER (__builtin_choose_expr (                                               \
                NARGS (__VA_ARGS__) == 1 &&                                                                            \
                    __builtin_types_compatible_p (typeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")), char []) &&          \
                    __builtin_constant_p (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")),                                      \
                0, 1                                                                                                   \
            )) __VA_OPT__ (, stringify (ARG1 (__VA_ARGS__))));                                                         \
        })

#endif

#ifdef DISABLE_RUNTIME_DIAGS
    #define message(...)    ((void) 0)
    #define warning(...)    ((void) 0)
    #define error(...)      ((void) 0)
    #define log(level, ...) ((void) 0)
#else
    #ifdef _WIN32
        #ifdef __cpluplus
            #define message(...)                                                                                                                            \
                (ct_error (                                                                                                                                 \
                     NARGS (__VA_ARGS__) > 1,                                                                                                               \
                     "the message() function-like macro must be passed between zero and one arguments."                                                     \
                 ),                                                                                                                                         \
                 ({                                                                                                                                         \
                     HANDLE __message_stderr_handle__ = GetStdHandle (STD_ERROR_HANDLE);                                                                    \
                     SetConsoleMode (__message_stderr_handle__, ({                                                                                          \
                                         DWORD __message_consolemode__;                                                                                     \
                                         GetConsoleMode (__message_stderr_handle__, &__message_consolemode__);                                              \
                                         __message_consolemode__ | DISABLE_NEWLINE_AUTO_RETURN |                                                            \
                                             ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING;                                                  \
                                     }));                                                                                                                   \
                     ({                                                                                                                                     \
                         if (get_log_file () == -1)                                                                                                         \
                             WriteFile (                                                                                                                    \
                                 __message_stderr_handle__, "\033[1m\033[32m", sizeof ("\033[1m\033[32m") - 1, NULL,                                        \
                                 NULL                                                                                                                       \
                             );                                                                                                                             \
                     }),                                                                                                                                    \
                         WriteFile (                                                                                                                        \
                             __message_stderr_handle__, "Message in function ", sizeof ("Message in function ") - 1,                                        \
                             NULL, NULL                                                                                                                     \
                         ),                                                                                                                                 \
                         WriteFile (__message_stderr_handle__, __func__, sizeof (__func__) - 1, NULL, NULL),                                                \
                         WriteFile (__message_stderr_handle__, " (", sizeof (" (") - 1, NULL, NULL),                                                        \
                         WriteFile (__message_stderr_handle__, __FILE__, sizeof (__FILE__) - 1, NULL, NULL),                                                \
                         WriteFile (__message_stderr_handle__, ": line ", sizeof (": line ") - 1, NULL, NULL), ({                                           \
                             char     __message_line_digits__ [10];                                                                                         \
                             uint32_t __message_line__      = (uint32_t) __LINE__;                                                                          \
                             uint32_t __message_decplaces__ = (uint32_t) decplaces (__LINE__);                                                              \
                             for (uint32_t __message_iter__ = 1; __message_line__; __message_line__ /= 10)                                                  \
                                 *(__message_line_digits__ + __message_decplaces__ - __message_iter__++) =                                                  \
                                     (char) (__message_line__ % 10) + '0';                                                                                  \
                             WriteFile (                                                                                                                    \
                                 __message_stderr_handle__, __message_line_digits__, __message_decplaces__, NULL, NULL                                      \
                             );                                                                                                                             \
                         }),                                                                                                                                \
                         WriteFile (__message_stderr_handle__, ")", sizeof (")") - 1, NULL, NULL), ({                                                       \
                             if (get_log_file () == -1)                                                                                                     \
                                 WriteFile (                                                                                                                \
                                     __message_stderr_handle__, ([] () {                                                                                    \
                                         if (sizeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) == 1)                                                           \
                                             return ".\033[0m";                                                                                             \
                                         return ":\033[0m " ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "");                                                          \
                                     }) (),                                                                                                                 \
                                     ([] () {                                                                                                               \
                                         if (sizeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) == 1)                                                           \
                                             return sizeof (".\033[0m") - 1;                                                                                \
                                         return sizeof (":\033[0m " ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) - 1;                                             \
                                     }) (),                                                                                                                 \
                                     NULL, NULL                                                                                                             \
                                 );                                                                                                                         \
                             else                                                                                                                           \
                                 WriteFile (                                                                                                                \
                                     __message_stderr_handle__, ([] () {                                                                                    \
                                         if (sizeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) == 1)                                                           \
                                             return ".";                                                                                                    \
                                         return ": " ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "");                                                                 \
                                     }) (),                                                                                                                 \
                                     ([] () {                                                                                                               \
                                         if (sizeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) == 1)                                                           \
                                             return sizeof (".") - 1;                                                                                       \
                                         return sizeof (": " ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) - 1;                                                    \
                                     }) (),                                                                                                                 \
                                     NULL, NULL                                                                                                             \
                                 );                                                                                                                         \
                         }),                                                                                                                                \
                         WriteFile (__message_stderr_handle__, "\n", sizeof ("\n") - 1, NULL, NULL);                                                        \
                     (void) ({                                                                                                                              \
                         if (is_log_window ()) {                                                                                                            \
                             int line;                                                                                                                      \
                             if ((line = inc_last_log_line ()) >= (int) get_log_window_height () - 3) {                                                     \
                                 clear_log_window ();                                                                                                       \
                                 line = inc_last_log_line ();                                                                                               \
                             }                                                                                                                              \
                             if (has_colors ())                                                                                                             \
                                 wattron (get_log_window (), COLOR_PAIR (log_message + 1) | A_BOLD);                                                        \
                             mvwprintw (                                                                                                                    \
                                 get_log_window (), line, 1, "Message in function %s (%s: line %d)%s", __func__,                                            \
                                 __FILE__, __LINE__, sizeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) == 1 ? "." : ": "                                       \
                             );                                                                                                                             \
                             if (has_colors ())                                                                                                             \
                                 wattroff (get_log_window (), COLOR_PAIR (log_message + 1) | A_BOLD);                                                       \
                             if (sizeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) > 1)                                                                        \
                                 (ct_error (                                                                                                                \
                                      __builtin_classify_type (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) !=                                                    \
                                          pointer_type_class,                                                                                               \
                                      "the message() function-like macro must be passed either no arguments or a single argument of pointer or array type." \
                                  ),                                                                                                                        \
                                  waddstr (get_log_window (), ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")));                                                      \
                             refresh_log_window ();                                                                                                         \
                         }                                                                                                                                  \
                     });                                                                                                                                    \
                 }))
        #else
            #define message(...)                                                                                                                            \
                (ct_error (                                                                                                                                 \
                     NARGS (__VA_ARGS__) > 1,                                                                                                               \
                     "the message() function-like macro must be passed between zero and one arguments."                                                     \
                 ),                                                                                                                                         \
                 ({                                                                                                                                         \
                     HANDLE __message_stderr_handle__ = GetStdHandle (STD_ERROR_HANDLE);                                                                    \
                     SetConsoleMode (__message_stderr_handle__, ({                                                                                          \
                                         DWORD __message_consolemode__;                                                                                     \
                                         GetConsoleMode (__message_stderr_handle__, &__message_consolemode__);                                              \
                                         __message_consolemode__ | DISABLE_NEWLINE_AUTO_RETURN |                                                            \
                                             ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING;                                                  \
                                     }));                                                                                                                   \
                     ({                                                                                                                                     \
                         if (get_log_file () == -1)                                                                                                         \
                             WriteFile (                                                                                                                    \
                                 __message_stderr_handle__, "\033[1m\033[32m", sizeof ("\033[1m\033[32m") - 1, NULL,                                        \
                                 NULL                                                                                                                       \
                             );                                                                                                                             \
                     }),                                                                                                                                    \
                         WriteFile (                                                                                                                        \
                             __message_stderr_handle__, "Message in function ", sizeof ("Message in function ") - 1,                                        \
                             NULL, NULL                                                                                                                     \
                         ),                                                                                                                                 \
                         WriteFile (__message_stderr_handle__, __func__, sizeof (__func__) - 1, NULL, NULL),                                                \
                         WriteFile (__message_stderr_handle__, " (", sizeof (" (") - 1, NULL, NULL),                                                        \
                         WriteFile (__message_stderr_handle__, __FILE__, sizeof (__FILE__) - 1, NULL, NULL),                                                \
                         WriteFile (__message_stderr_handle__, ": line ", sizeof (": line ") - 1, NULL, NULL), ({                                           \
                             char     __message_line_digits__ [10];                                                                                         \
                             uint32_t __message_line__      = (uint32_t) __LINE__;                                                                          \
                             uint32_t __message_decplaces__ = (uint32_t) decplaces (__LINE__);                                                              \
                             for (uint32_t __message_iter__ = 1; __message_line__; __message_line__ /= 10)                                                  \
                                 *(__message_line_digits__ + __message_decplaces__ - __message_iter__++) =                                                  \
                                     (char) (__message_line__ % 10) + '0';                                                                                  \
                             WriteFile (                                                                                                                    \
                                 __message_stderr_handle__, __message_line_digits__, __message_decplaces__, NULL, NULL                                      \
                             );                                                                                                                             \
                         }),                                                                                                                                \
                         WriteFile (__message_stderr_handle__, ")", sizeof (")") - 1, NULL, NULL), ({                                                       \
                             if (get_log_file () == -1)                                                                                                     \
                                 WriteFile (                                                                                                                \
                                     __message_stderr_handle__,                                                                                             \
                                     __builtin_choose_expr (                                                                                                \
                                         sizeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) == 1, ".\033[0m",                                                   \
                                         ":\033[0m " ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")                                                                  \
                                     ),                                                                                                                     \
                                     sizeof (__builtin_choose_expr (                                                                                        \
                                         sizeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) == 1, ".\033[0m",                                                   \
                                         ":\033[0m " ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")                                                                  \
                                     )) - 1,                                                                                                                \
                                     NULL, NULL                                                                                                             \
                                 );                                                                                                                         \
                             else                                                                                                                           \
                                 WriteFile (                                                                                                                \
                                     __message_stderr_handle__,                                                                                             \
                                     __builtin_choose_expr (                                                                                                \
                                         sizeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) == 1, ".",                                                          \
                                         ": " ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")                                                                         \
                                     ),                                                                                                                     \
                                     sizeof (__builtin_choose_expr (                                                                                        \
                                         sizeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) == 1, ".",                                                          \
                                         ": " ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")                                                                         \
                                     )) - 1,                                                                                                                \
                                     NULL, NULL                                                                                                             \
                                 );                                                                                                                         \
                         }),                                                                                                                                \
                         WriteFile (__message_stderr_handle__, "\n", sizeof ("\n") - 1, NULL, NULL);                                                        \
                     (void) ({                                                                                                                              \
                         if (is_log_window ()) {                                                                                                            \
                             int line;                                                                                                                      \
                             if ((line = inc_last_log_line ()) >= (int) get_log_window_height () - 3) {                                                     \
                                 clear_log_window ();                                                                                                       \
                                 line = inc_last_log_line ();                                                                                               \
                             }                                                                                                                              \
                             if (has_colors ())                                                                                                             \
                                 wattron (get_log_window (), COLOR_PAIR (log_message + 1) | A_BOLD);                                                        \
                             mvwprintw (                                                                                                                    \
                                 get_log_window (), line, 1, "Message in function %s (%s: line %d)%s", __func__,                                            \
                                 __FILE__, __LINE__, sizeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) == 1 ? "." : ": "                                       \
                             );                                                                                                                             \
                             if (has_colors ())                                                                                                             \
                                 wattroff (get_log_window (), COLOR_PAIR (log_message + 1) | A_BOLD);                                                       \
                             __builtin_choose_expr (                                                                                                        \
                                 sizeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) == 1, (void) 0,                                                             \
                                 (ct_error (                                                                                                                \
                                      __builtin_classify_type (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) !=                                                    \
                                          pointer_type_class,                                                                                               \
                                      "the message() function-like macro must be passed either no arguments or a single argument of pointer or array type." \
                                  ),                                                                                                                        \
                                  waddstr (get_log_window (), ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")))                                                       \
                             );                                                                                                                             \
                             refresh_log_window ();                                                                                                         \
                         }                                                                                                                                  \
                     });                                                                                                                                    \
                 }))
        #endif
    #else
        #ifdef __cplusplus
            #define message(...)                                                                                                                           \
                (ct_error (                                                                                                                                \
                     NARGS (__VA_ARGS__) > 1,                                                                                                              \
                     "the message() function-like macro must be passed between zero and three arguments."                                                  \
                 ),                                                                                                                                        \
                 fprintf (                                                                                                                                 \
                     stderr, "%sMessage in function %s (%s: line %d)%s\n",                                                                                 \
                     get_log_file () == -1 ? "\033[1m\033[32m" : "", __func__, __FILE__, __LINE__, ([] () {                                                \
                         if (sizeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) == 1)                                                                          \
                             return get_log_file () == -1 ? ".\033[0m" : ".";                                                                              \
                         ct_error (                                                                                                                        \
                             !std::is_pointer<                                                                                                             \
                                 std::decay<decltype (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) ""))>::type>::value,                                               \
                             "the message() function-like macro must be passed either no arguments or a single argument of pointer or array type."         \
                         );                                                                                                                                \
                         return get_log_file () == -1 ? (":\033[0m " ARG1 (__VA_ARGS__ __VA_OPT__ (, ) ""))                                                \
                                                      : (": " ARG1 (__VA_ARGS__ __VA_OPT__ (, ) ""));                                                      \
                     }) ()                                                                                                                                 \
                 ),                                                                                                                                        \
                 fflush (stderr), (void) ({                                                                                                                \
                     if (is_log_window ()) {                                                                                                               \
                         int line;                                                                                                                         \
                         if ((line = inc_last_log_line ()) >= (int) get_log_window_height () - 3) {                                                        \
                             clear_log_window ();                                                                                                          \
                             line = inc_last_log_line ();                                                                                                  \
                         }                                                                                                                                 \
                         if (has_colors ())                                                                                                                \
                             wattron (get_log_window (), COLOR_PAIR (log_message + 1) | A_BOLD);                                                           \
                         mvwprintw (                                                                                                                       \
                             get_log_window (), line, 1, "Message in function %s (%s: line %d)%s", __func__, __FILE__,                                     \
                             __LINE__, sizeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) == 1 ? "." : ": "                                                    \
                         );                                                                                                                                \
                         if (has_colors ())                                                                                                                \
                             wattroff (get_log_window (), COLOR_PAIR (log_message + 1) | A_BOLD);                                                          \
                         ([] () {                                                                                                                          \
                             if (sizeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) > 1) {                                                                     \
                                 ct_error (                                                                                                                \
                                     !std::is_pointer<                                                                                                     \
                                         std::decay<decltype (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) ""))>::type>::value,                                       \
                                     "the message() function-like macro must be passed either no arguments or a single argument of pointer or array type." \
                                 );                                                                                                                        \
                                 waddstr (get_log_window (), ARG1 (__VA_ARGS__ __VA_OPT__ (, ) ""));                                                       \
                             }                                                                                                                             \
                         }) ();                                                                                                                            \
                         refresh_log_window ();                                                                                                            \
                     }                                                                                                                                     \
                 }))
        #else
            #define message(...)                                                                                                                        \
                (ct_error (                                                                                                                             \
                     NARGS (__VA_ARGS__) > 1,                                                                                                           \
                     "the message() function-like macro must be passed between zero and three arguments."                                               \
                 ),                                                                                                                                     \
                 fprintf (                                                                                                                              \
                     stderr, "%sMessage in function %s (%s: line %d)%s\n",                                                                              \
                     get_log_file () == -1 ? "\033[1m\033[32m" : "", __func__, __FILE__, __LINE__,                                                      \
                     __builtin_choose_expr (                                                                                                            \
                         sizeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) == 1,                                                                           \
                         get_log_file () == -1 ? ".\033[0m" : ".",                                                                                      \
                         (ct_error (                                                                                                                    \
                              __builtin_classify_type (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) != pointer_type_class,                                    \
                              "the message() function-like macro must be passed either no arguments or a single argument of pointer or array type."     \
                          ),                                                                                                                            \
                          get_log_file () == -1 ? (":\033[0m " ARG1 (__VA_ARGS__ __VA_OPT__ (, ) ""))                                                   \
                                                : (": " ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")))                                                         \
                     )                                                                                                                                  \
                 ),                                                                                                                                     \
                 fflush (stderr), (void) ({                                                                                                             \
                     if (is_log_window ()) {                                                                                                            \
                         int line;                                                                                                                      \
                         if ((line = inc_last_log_line ()) >= (int) get_log_window_height () - 3) {                                                     \
                             clear_log_window ();                                                                                                       \
                             line = inc_last_log_line ();                                                                                               \
                         }                                                                                                                              \
                         if (has_colors ())                                                                                                             \
                             wattron (get_log_window (), COLOR_PAIR (log_message + 1) | A_BOLD);                                                        \
                         mvwprintw (                                                                                                                    \
                             get_log_window (), line, 1, "Message in function %s (%s: line %d)%s", __func__, __FILE__,                                  \
                             __LINE__, sizeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) == 1 ? "." : ": "                                                 \
                         );                                                                                                                             \
                         if (has_colors ())                                                                                                             \
                             wattroff (get_log_window (), COLOR_PAIR (log_message + 1) | A_BOLD);                                                       \
                         __builtin_choose_expr (                                                                                                        \
                             sizeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) == 1, (void) 0,                                                             \
                             (ct_error (                                                                                                                \
                                  __builtin_classify_type (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) !=                                                    \
                                      pointer_type_class,                                                                                               \
                                  "the message() function-like macro must be passed either no arguments or a single argument of pointer or array type." \
                              ),                                                                                                                        \
                              waddstr (get_log_window (), ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")))                                                       \
                         );                                                                                                                             \
                         refresh_log_window ();                                                                                                         \
                     }                                                                                                                                  \
                 }))
        #endif
    #endif

    #ifdef _WIN32
        #ifdef __cplusplus
            #define warning(...)                                                                                                                            \
                (ct_error (                                                                                                                                 \
                     NARGS (__VA_ARGS__) > 1,                                                                                                               \
                     "the warning() function-like macro must be passed between zero and one arguments."                                                     \
                 ),                                                                                                                                         \
                 ({                                                                                                                                         \
                     HANDLE __warning_stderr_handle__ = GetStdHandle (STD_ERROR_HANDLE);                                                                    \
                     SetConsoleMode (__warning_stderr_handle__, ({                                                                                          \
                                         DWORD __warning_consolemode__;                                                                                     \
                                         GetConsoleMode (__warning_stderr_handle__, &__warning_consolemode__);                                              \
                                         __warning_consolemode__ | DISABLE_NEWLINE_AUTO_RETURN |                                                            \
                                             ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING;                                                  \
                                     }));                                                                                                                   \
                     ({                                                                                                                                     \
                         if (get_log_file () == -1)                                                                                                         \
                             WriteFile (                                                                                                                    \
                                 __warning_stderr_handle__, "\033[1m\033[32m", sizeof ("\033[1m\033[33m") - 1, NULL,                                        \
                                 NULL                                                                                                                       \
                             );                                                                                                                             \
                     }),                                                                                                                                    \
                         WriteFile (                                                                                                                        \
                             __warning_stderr_handle__, "Warning in function ", sizeof ("Warning in function ") - 1,                                        \
                             NULL, NULL                                                                                                                     \
                         ),                                                                                                                                 \
                         WriteFile (__warning_stderr_handle__, __func__, sizeof (__func__) - 1, NULL, NULL),                                                \
                         WriteFile (__warning_stderr_handle__, " (", sizeof (" (") - 1, NULL, NULL),                                                        \
                         WriteFile (__warning_stderr_handle__, __FILE__, sizeof (__FILE__) - 1, NULL, NULL),                                                \
                         WriteFile (__warning_stderr_handle__, ": line ", sizeof (": line ") - 1, NULL, NULL), ({                                           \
                             char     __warning_line_digits__ [10];                                                                                         \
                             uint32_t __warning_line__      = (uint32_t) __LINE__;                                                                          \
                             uint32_t __warning_decplaces__ = (uint32_t) decplaces (__LINE__);                                                              \
                             for (uint32_t __warning_iter__ = 1; __warning_line__; __warning_line__ /= 10)                                                  \
                                 *(__warning_line_digits__ + __warning_decplaces__ - __warning_iter__++) =                                                  \
                                     (char) (__warning_line__ % 10) + '0';                                                                                  \
                             WriteFile (                                                                                                                    \
                                 __warning_stderr_handle__, __warning_line_digits__, __warning_decplaces__, NULL, NULL                                      \
                             );                                                                                                                             \
                         }),                                                                                                                                \
                         WriteFile (__warning_stderr_handle__, ")", sizeof (")") - 1, NULL, NULL), ({                                                       \
                             if (get_log_file () == -1)                                                                                                     \
                                 WriteFile (                                                                                                                \
                                     __warning_stderr_handle__, ([] () {                                                                                    \
                                         if (sizeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) == 1)                                                           \
                                             return ".\033[0m";                                                                                             \
                                         return ":\033[0m " ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "");                                                          \
                                     }) (),                                                                                                                 \
                                     ([] () {                                                                                                               \
                                         if (sizeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) == 1)                                                           \
                                             return (DWORD) sizeof (".\033[0m") - 1;                                                                        \
                                         return (DWORD) sizeof (":\033[0m " ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) -                                        \
                                                1;                                                                                                          \
                                     }) (),                                                                                                                 \
                                     NULL, NULL                                                                                                             \
                                 );                                                                                                                         \
                             else                                                                                                                           \
                                 WriteFile (                                                                                                                \
                                     __warning_stderr_handle__, ([] () {                                                                                    \
                                         if (sizeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) == 1)                                                           \
                                             return ".";                                                                                                    \
                                         return ": " ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "");                                                                 \
                                     }) (),                                                                                                                 \
                                     ([] () {                                                                                                               \
                                         if (sizeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) == 1)                                                           \
                                             return (DWORD) sizeof (".") - 1;                                                                               \
                                         return (DWORD) sizeof (": " ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) - 1;                                            \
                                     }) (),                                                                                                                 \
                                     NULL, NULL                                                                                                             \
                                 );                                                                                                                         \
                         }),                                                                                                                                \
                         WriteFile (__warning_stderr_handle__, "\n", sizeof ("\n") - 1, NULL, NULL);                                                        \
                     (void) ({                                                                                                                              \
                         if (is_log_window ()) {                                                                                                            \
                             int line;                                                                                                                      \
                             if ((line = inc_last_log_line ()) >= (int) get_log_window_height () - 3) {                                                     \
                                 clear_log_window ();                                                                                                       \
                                 line = inc_last_log_line ();                                                                                               \
                             }                                                                                                                              \
                             if (has_colors ())                                                                                                             \
                                 wattron (get_log_window (), COLOR_PAIR (log_warning + 1) | A_BOLD);                                                        \
                             mvwprintw (                                                                                                                    \
                                 get_log_window (), line, 1, "Warning in function %s (%s: line %d)%s", __func__,                                            \
                                 __FILE__, __LINE__, sizeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) == 1 ? "." : ": "                                       \
                             );                                                                                                                             \
                             if (has_colors ())                                                                                                             \
                                 wattroff (get_log_window (), COLOR_PAIR (log_warning + 1) | A_BOLD);                                                       \
                             if (sizeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) > 1)                                                                        \
                                 (ct_error (                                                                                                                \
                                      !std::is_pointer<                                                                                                     \
                                          std::decay<decltype (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) ""))>::type>::value,                                       \
                                      "the warning() function-like macro must be passed either no arguments or a single argument of pointer or array type." \
                                  ),                                                                                                                        \
                                  waddstr (get_log_window (), ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")));                                                      \
                             refresh_log_window ();                                                                                                         \
                         }                                                                                                                                  \
                     });                                                                                                                                    \
                 }))
        #else
            #define warning(...)                                                                                                                            \
                (ct_error (                                                                                                                                 \
                     NARGS (__VA_ARGS__) > 1,                                                                                                               \
                     "the warning() function-like macro must be passed between zero and one arguments."                                                     \
                 ),                                                                                                                                         \
                 ({                                                                                                                                         \
                     HANDLE __warning_stderr_handle__ = GetStdHandle (STD_ERROR_HANDLE);                                                                    \
                     SetConsoleMode (__warning_stderr_handle__, ({                                                                                          \
                                         DWORD __warning_consolemode__;                                                                                     \
                                         GetConsoleMode (__warning_stderr_handle__, &__warning_consolemode__);                                              \
                                         __warning_consolemode__ | DISABLE_NEWLINE_AUTO_RETURN |                                                            \
                                             ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING;                                                  \
                                     }));                                                                                                                   \
                     ({                                                                                                                                     \
                         if (get_log_file () == -1)                                                                                                         \
                             WriteFile (                                                                                                                    \
                                 __warning_stderr_handle__, "\033[1m\033[33m", sizeof ("\033[1m\033[33m") - 1, NULL,                                        \
                                 NULL                                                                                                                       \
                             );                                                                                                                             \
                     }),                                                                                                                                    \
                         WriteFile (                                                                                                                        \
                             __warning_stderr_handle__, "Warning in function ", sizeof ("Warning in function ") - 1,                                        \
                             NULL, NULL                                                                                                                     \
                         ),                                                                                                                                 \
                         WriteFile (__warning_stderr_handle__, __func__, sizeof (__func__) - 1, NULL, NULL),                                                \
                         WriteFile (__warning_stderr_handle__, " (", sizeof (" (") - 1, NULL, NULL),                                                        \
                         WriteFile (__warning_stderr_handle__, __FILE__, sizeof (__FILE__) - 1, NULL, NULL),                                                \
                         WriteFile (__warning_stderr_handle__, ": line ", sizeof (": line ") - 1, NULL, NULL), ({                                           \
                             char     __warning_line_digits__ [10];                                                                                         \
                             uint32_t __warning_line__      = (uint32_t) __LINE__;                                                                          \
                             uint32_t __warning_decplaces__ = (uint32_t) decplaces (__LINE__);                                                              \
                             for (uint32_t __warning_iter__ = 1; __warning_line__; __warning_line__ /= 10)                                                  \
                                 *(__warning_line_digits__ + __warning_decplaces__ - __warning_iter__++) =                                                  \
                                     (char) (__warning_line__ % 10) + '0';                                                                                  \
                             WriteFile (                                                                                                                    \
                                 __warning_stderr_handle__, __warning_line_digits__, __warning_decplaces__, NULL, NULL                                      \
                             );                                                                                                                             \
                         }),                                                                                                                                \
                         WriteFile (__warning_stderr_handle__, ")", sizeof (")") - 1, NULL, NULL), ({                                                       \
                             if (get_log_file () == -1)                                                                                                     \
                                 WriteFile (                                                                                                                \
                                     __warning_stderr_handle__,                                                                                             \
                                     __builtin_choose_expr (                                                                                                \
                                         sizeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) == 1, ".\033[0m",                                                   \
                                         ":\033[0m " ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")                                                                  \
                                     ),                                                                                                                     \
                                     sizeof (__builtin_choose_expr (                                                                                        \
                                         sizeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) == 1, ".\033[0m",                                                   \
                                         ":\033[0m " ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")                                                                  \
                                     )) - 1,                                                                                                                \
                                     NULL, NULL                                                                                                             \
                                 );                                                                                                                         \
                             else                                                                                                                           \
                                 WriteFile (                                                                                                                \
                                     __warning_stderr_handle__,                                                                                             \
                                     __builtin_choose_expr (                                                                                                \
                                         sizeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) == 1, ".",                                                          \
                                         ": " ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")                                                                         \
                                     ),                                                                                                                     \
                                     sizeof (__builtin_choose_expr (                                                                                        \
                                         sizeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) == 1, ".",                                                          \
                                         ": " ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")                                                                         \
                                     )) - 1,                                                                                                                \
                                     NULL, NULL                                                                                                             \
                                 );                                                                                                                         \
                         }),                                                                                                                                \
                         WriteFile (__warning_stderr_handle__, "\n", sizeof ("\n") - 1, NULL, NULL);                                                        \
                     (void) ({                                                                                                                              \
                         if (is_log_window ()) {                                                                                                            \
                             int line;                                                                                                                      \
                             if ((line = inc_last_log_line ()) >= (int) get_log_window_height () - 3) {                                                     \
                                 clear_log_window ();                                                                                                       \
                                 line = inc_last_log_line ();                                                                                               \
                             }                                                                                                                              \
                             if (has_colors ())                                                                                                             \
                                 wattron (get_log_window (), COLOR_PAIR (log_warning + 1) | A_BOLD);                                                        \
                             mvwprintw (                                                                                                                    \
                                 get_log_window (), line, 1, "Warning in function %s (%s: line %d)%s", __func__,                                            \
                                 __FILE__, __LINE__, sizeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) == 1 ? "." : ": "                                       \
                             );                                                                                                                             \
                             if (has_colors ())                                                                                                             \
                                 wattroff (get_log_window (), COLOR_PAIR (log_warning + 1) | A_BOLD);                                                       \
                             __builtin_choose_expr (                                                                                                        \
                                 sizeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) == 1, (void) 0,                                                             \
                                 (ct_error (                                                                                                                \
                                      __builtin_classify_type (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) !=                                                    \
                                          pointer_type_class,                                                                                               \
                                      "the warning() function-like macro must be passed either no arguments or a single argument of pointer or array type." \
                                  ),                                                                                                                        \
                                  waddstr (get_log_window (), ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")))                                                       \
                             );                                                                                                                             \
                             refresh_log_window ();                                                                                                         \
                         }                                                                                                                                  \
                     });                                                                                                                                    \
                 }))
        #endif
    #else
        #ifdef __cplusplus
            #define warning(...)                                                                                                                           \
                (ct_error (                                                                                                                                \
                     NARGS (__VA_ARGS__) > 1,                                                                                                              \
                     "the warning() function-like macro must be passed between zero and three arguments."                                                  \
                 ),                                                                                                                                        \
                 fprintf (                                                                                                                                 \
                     stderr, "%sWarning in function %s (%s: line %d)%s\n",                                                                                 \
                     get_log_file () == -1 ? "\033[1m\033[33m" : "", __func__, __FILE__, __LINE__, ([] () {                                                \
                         if (sizeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) == 1)                                                                          \
                             return get_log_file () == -1 ? ".\033[0m" : ".";                                                                              \
                         ct_error (                                                                                                                        \
                             !std::is_pointer<                                                                                                             \
                                 std::decay<decltype (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) ""))>::type>::value,                                               \
                             "the warning() function-like macro must be passed either no arguments or a single argument of pointer or array type."         \
                         );                                                                                                                                \
                         return get_log_file () == -1 ? (":\033[0m " ARG1 (__VA_ARGS__ __VA_OPT__ (, ) ""))                                                \
                                                      : (": " ARG1 (__VA_ARGS__ __VA_OPT__ (, ) ""));                                                      \
                     }) ()                                                                                                                                 \
                 ),                                                                                                                                        \
                 fflush (stderr), (void) ({                                                                                                                \
                     if (is_log_window ()) {                                                                                                               \
                         int line;                                                                                                                         \
                         if ((line = inc_last_log_line ()) >= (int) get_log_window_height () - 3) {                                                        \
                             clear_log_window ();                                                                                                          \
                             line = inc_last_log_line ();                                                                                                  \
                         }                                                                                                                                 \
                         if (has_colors ())                                                                                                                \
                             wattron (get_log_window (), COLOR_PAIR (log_warning + 1) | A_BOLD);                                                           \
                         mvwprintw (                                                                                                                       \
                             get_log_window (), line, 1, "Warning in function %s (%s: line %d)%s", __func__, __FILE__,                                     \
                             __LINE__, sizeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) == 1 ? "." : ": "                                                    \
                         );                                                                                                                                \
                         if (has_colors ())                                                                                                                \
                             wattroff (get_log_window (), COLOR_PAIR (log_warning + 1) | A_BOLD);                                                          \
                         ([] () {                                                                                                                          \
                             if (sizeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) > 1) {                                                                     \
                                 ct_error (                                                                                                                \
                                     !std::is_pointer<                                                                                                     \
                                         std::decay<decltype (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) ""))>::type>::value,                                       \
                                     "the warning() function-like macro must be passed either no arguments or a single argument of pointer or array type." \
                                 );                                                                                                                        \
                                 waddstr (get_log_window (), ARG1 (__VA_ARGS__ __VA_OPT__ (, ) ""));                                                       \
                             }                                                                                                                             \
                         }) ();                                                                                                                            \
                         refresh_log_window ();                                                                                                            \
                     }                                                                                                                                     \
                 }))
        #else
            #define warning(...)                                                                                                                        \
                (ct_error (                                                                                                                             \
                     NARGS (__VA_ARGS__) > 1,                                                                                                           \
                     "the warning() function-like macro must be passed between zero and one arguments."                                                 \
                 ),                                                                                                                                     \
                 fprintf (                                                                                                                              \
                     stderr, "%sWarning in function %s (%s: line %d)%s\n",                                                                              \
                     get_log_file () == -1 ? "\033[1m\033[33m" : "", __func__, __FILE__, __LINE__,                                                      \
                     __builtin_choose_expr (                                                                                                            \
                         sizeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) == 1,                                                                           \
                         get_log_file () == -1 ? ".\033[0m" : ".",                                                                                      \
                         (ct_error (                                                                                                                    \
                              __builtin_classify_type (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) != pointer_type_class,                                    \
                              "the warning() function-like macro must be passed either no arguments or a single argument of pointer or array type."     \
                          ),                                                                                                                            \
                          get_log_file () == -1 ? (":\033[0m " ARG1 (__VA_ARGS__ __VA_OPT__ (, ) ""))                                                   \
                                                : (": " ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")))                                                         \
                     )                                                                                                                                  \
                 ),                                                                                                                                     \
                 fflush (stderr), (void) ({                                                                                                             \
                     if (is_log_window ()) {                                                                                                            \
                         int line;                                                                                                                      \
                         if ((line = inc_last_log_line ()) >= (int) get_log_window_height () - 3) {                                                     \
                             clear_log_window ();                                                                                                       \
                             line = inc_last_log_line ();                                                                                               \
                         }                                                                                                                              \
                         if (has_colors ())                                                                                                             \
                             wattron (get_log_window (), COLOR_PAIR (log_warning + 1) | A_BOLD);                                                        \
                         mvwprintw (                                                                                                                    \
                             get_log_window (), line, 1, "Warning in function %s (%s: line %d)%s", __func__, __FILE__,                                  \
                             __LINE__, sizeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) == 1 ? "." : ": "                                                 \
                         );                                                                                                                             \
                         if (has_colors ())                                                                                                             \
                             wattroff (get_log_window (), COLOR_PAIR (log_warning + 1) | A_BOLD);                                                       \
                         __builtin_choose_expr (                                                                                                        \
                             sizeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) == 1, (void) 0,                                                             \
                             (ct_error (                                                                                                                \
                                  __builtin_classify_type (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) !=                                                    \
                                      pointer_type_class,                                                                                               \
                                  "the warning() function-like macro must be passed either no arguments or a single argument of pointer or array type." \
                              ),                                                                                                                        \
                              waddstr (get_log_window (), ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")))                                                       \
                         );                                                                                                                             \
                         refresh_log_window ();                                                                                                         \
                     }                                                                                                                                  \
                 }))
        #endif
    #endif

    #ifdef _WIN32
        #ifdef __cplusplus
            #define error(...)                                                                                                  \
                (ct_error (                                                                                                     \
                     NARGS (__VA_ARGS__) > 3,                                                                                   \
                     "the error() function-like macro must be passed between zero and three arguments."                         \
                 ),                                                                                                             \
                 ct_error (                                                                                                     \
                     !(std::is_same<                                                                                            \
                           decltype (ARG2 (__VA_ARGS__ __VA_OPT__ (, ) ((errorfunc_t *) NULL), ((errorfunc_t *) NULL))          \
                           ),                                                                                                   \
                           errorfunc_t>::value ||                                                                               \
                       std::is_same<                                                                                            \
                           decltype (ARG2 (__VA_ARGS__ __VA_OPT__ (, ) ((errorfunc_t *) NULL), ((errorfunc_t *) NULL))          \
                           ),                                                                                                   \
                           errorfunc_t *>::value),                                                                              \
                     "the second argument passed to the error() function-like macro must be of type errorfunc_t *."             \
                 ),                                                                                                             \
                 ct_error (                                                                                                     \
                     !std::is_pointer<std::decay<decltype (ARG3 (                                                               \
                         __VA_ARGS__ __VA_OPT__ (, ) ((void *) NULL), ((void *) NULL), ((void *) NULL)                          \
                     ))>::type>::value,                                                                                         \
                     "the third argument passed to the error() function-like macro must be of a type that decays to a pointer." \
                 ),                                                                                                             \
                 ({                                                                                                             \
                     HANDLE __error_stderr_handle__ = GetStdHandle (STD_ERROR_HANDLE);                                          \
                     SetConsoleMode (__error_stderr_handle__, ({                                                                \
                                         DWORD __error_consolemode__;                                                           \
                                         GetConsoleMode (__error_stderr_handle__, &__error_consolemode__);                      \
                                         __error_consolemode__ | DISABLE_NEWLINE_AUTO_RETURN |                                  \
                                             ENABLE_VIRTUAL_TERMINAL_PROCESSING;                                                \
                                     })),                                                                                       \
                         ({                                                                                                     \
                             if (get_log_file () == -1)                                                                         \
                                 WriteFile (                                                                                    \
                                     __error_stderr_handle__, "\033[1m\033[31m", sizeof ("\033[1m\033[31m") - 1, NULL,          \
                                     NULL                                                                                       \
                                 );                                                                                             \
                         }),                                                                                                    \
                         WriteFile (                                                                                            \
                             __error_stderr_handle__, "Error in function ", sizeof ("Error in function ") - 1, NULL,            \
                             NULL                                                                                               \
                         ),                                                                                                     \
                         WriteFile (__error_stderr_handle__, __func__, sizeof (__func__) - 1, NULL, NULL),                      \
                         WriteFile (__error_stderr_handle__, " (", sizeof (" (") - 1, NULL, NULL),                              \
                         WriteFile (__error_stderr_handle__, __FILE__, sizeof (__FILE__) - 1, NULL, NULL),                      \
                         WriteFile (__error_stderr_handle__, ": line ", sizeof (": line ") - 1, NULL, NULL), ({                 \
                             char     __error_line_digits__ [10];                                                               \
                             uint32_t __error_line__      = (uint32_t) __LINE__;                                                \
                             uint32_t __error_decplaces__ = (uint32_t) decplaces ((uint32_t) __LINE__);                                    \
                             for (uint32_t __error_iter__ = 1; __error_line__; __error_line__ /= 10)                            \
                                 *(__error_line_digits__ + __error_decplaces__ - __error_iter__++) =                            \
                                     (char) (__error_line__ % 10) + '0';                                                        \
                             WriteFile (                                                                                        \
                                 __error_stderr_handle__, __error_line_digits__, __error_decplaces__, NULL, NULL                \
                             );                                                                                                 \
                         }),                                                                                                    \
                         WriteFile (__error_stderr_handle__, ")", sizeof (")") - 1, NULL, NULL), ({                             \
                             if (get_log_file () == -1)                                                                         \
                                 WriteFile (                                                                                    \
                                     __error_stderr_handle__, ([] () {                                                          \
                                         if (sizeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) == 1)                               \
                                             return ".\033[0m";                                                                 \
                                         return ":\033[0m " ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "");                              \
                                     }) (),                                                                                     \
                                     ([] () {                                                                                   \
                                         if (sizeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) == 1)                               \
                                             return (DWORD) sizeof (".\033[0m") - 1;                                            \
                                         return (DWORD) sizeof (":\033[0m " ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) -            \
                                                1;                                                                              \
                                     }) (),                                                                                     \
                                     NULL, NULL                                                                                 \
                                 );                                                                                             \
                             else                                                                                               \
                                 WriteFile (                                                                                    \
                                     __error_stderr_handle__, ([] () {                                                          \
                                         if (sizeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) == 1)                               \
                                             return ".";                                                                        \
                                         return ": " ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "");                                     \
                                     }) (),                                                                                     \
                                     ([] () {                                                                                   \
                                         if (sizeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) == 1)                               \
                                             return (DWORD) sizeof (".") - 1;                                                   \
                                         return (DWORD) sizeof (": " ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) - 1;                \
                                     }) (),                                                                                     \
                                     NULL, NULL                                                                                 \
                                 );                                                                                             \
                         }),                                                                                                    \
                         WriteFile (__error_stderr_handle__, "\n", sizeof ("\n") - 1, NULL, NULL), close_log_file (),           \
                         print_backtrace (), ({                                                                                 \
                             _Pragma ("GCC diagnostic push");                                                                   \
                             _Pragma ("GCC diagnostic ignored \"-Waddress\"");                                                  \
                             _Pragma ("GCC diagnostic ignored \"-Wcast-function-type\"");                                       \
                             if (ARG2 (__VA_ARGS__ __VA_OPT__ (, ) NULL, NULL))                                                 \
                                 (ARG2 (__VA_ARGS__ __VA_OPT__ (, ) ((errorfunc_t *) NULL), ((errorfunc_t *) NULL))             \
                                 ) (ARG3 (__VA_ARGS__ __VA_OPT__ (, ) NULL, NULL, NULL));                                       \
                             _Pragma ("GCC diagnostic pop");                                                                    \
                         }),                                                                                                    \
                         ([] () {                                                                                               \
                             if (__builtin_has_attribute (                                                                      \
                                     ARG2 (                                                                                     \
                                         __VA_ARGS__ __VA_OPT__ (, ) ((errorfunc_t *) NULL), ((errorfunc_t *) NULL)             \
                                     ),                                                                                         \
                                     __noreturn__                                                                               \
                                 ) ||                                                                                           \
                                 __builtin_has_attribute (                                                                      \
                                     ARG2 (                                                                                     \
                                         __VA_ARGS__ __VA_OPT__ (, ) ((errorfunc_t *) NULL), ((errorfunc_t *) NULL)             \
                                     ),                                                                                         \
                                     noreturn                                                                                   \
                                 ))                                                                                             \
                                 return trap (), unreachable ();                                                                \
                             if (is_log_window ()) {                                                                            \
                                 int line;                                                                                      \
                                 if ((line = inc_last_log_line ()) >= (int) get_log_window_height () - 3) {                     \
                                     clear_log_window ();                                                                       \
                                     line = inc_last_log_line ();                                                               \
                                 }                                                                                              \
                                 if (has_colors ())                                                                             \
                                     wattron (get_log_window (), COLOR_PAIR (log_error + 1) | A_BOLD);                          \
                                 mvwprintw (                                                                                    \
                                     get_log_window (), line, 1, "Error in function %s (%s: line %d)%s", __func__,              \
                                     __FILE__, __LINE__,                                                                        \
                                     sizeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) == 1 ? "." : ": "                           \
                                 );                                                                                             \
                                 if (has_colors ())                                                                             \
                                     wattroff (get_log_window (), COLOR_PAIR (log_error + 1) | A_BOLD);                         \
                                 if (sizeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) != 1)                                       \
                                     waddstr (get_log_window (), ARG1 (__VA_ARGS__ __VA_OPT__ (, ) ""));                        \
                                 refresh_log_window ();                                                                         \
                             }                                                                                                  \
                             if (!isendwin ()) {                                                                                \
                                 refresh_log_window ();                                                                         \
                                 def_prog_mode ();                                                                              \
                                 end_ui ();                                                                                     \
                                 reset_prog_mode ();                                                                            \
                                 int __error_ignore__;                                                                          \
                                 for (int __error_iter__ =                                                                      \
                                          ({                                                                                    \
                                              int __error_h__;                                                                  \
                                              getmaxyx (get_log_window (), __error_h__, __error_ignore__);                      \
                                              __error_h__;                                                                      \
                                          }) *                                                                                  \
                                          2;                                                                                    \
                                      __error_iter__-- > 0; WriteFile (                                                         \
                                          GetStdHandle (STD_OUTPUT_HANDLE), "\n", sizeof ("\n") - 1, NULL, NULL                 \
                                      ))                                                                                        \
                                     ;                                                                                          \
                             }                                                                                                  \
                             trap ();                                                                                           \
                             unreachable ();                                                                                    \
                         }) ();                                                                                                 \
                 }))
        #else
            #define error(...)                                                                                                                                                     \
                (ct_error (                                                                                                                                                        \
                     NARGS (__VA_ARGS__) > 3,                                                                                                                                      \
                     "the error() function-like macro must be passed between zero and three arguments."                                                                            \
                 ),                                                                                                                                                                \
                 ct_error (                                                                                                                                                        \
                     !(__builtin_types_compatible_p (                                                                                                                              \
                           typeof (ARG2 (__VA_ARGS__ __VA_OPT__ (, ) ((errorfunc_t *) NULL), ((errorfunc_t *) NULL))),                                                             \
                           errorfunc_t                                                                                                                                             \
                       ) ||                                                                                                                                                        \
                       __builtin_types_compatible_p (typeof (ARG2 (__VA_ARGS__ __VA_OPT__ (, ) ((errorfunc_t *) NULL), ((errorfunc_t *) NULL))), errorfunc_t *)                    \
                     ),                                                                                                                                                            \
                     "the second argument passed to the error() function-like macro must be of type errorfunc_t *."                                                                \
                 ),                                                                                                                                                                \
                 ct_error (                                                                                                                                                        \
                     __builtin_classify_type (ARG3 (__VA_ARGS__ __VA_OPT__ (, ) NULL, NULL, NULL)) !=                                                                              \
                         pointer_type_class,                                                                                                                                       \
                     "the third argument passed to the error() function-like macro must be of a type that decays to a pointer."                                                    \
                 ),                                                                                                                                                                \
                 ({                                                                                                                                                                \
                     HANDLE __error_stderr_handle__ = GetStdHandle (STD_ERROR_HANDLE);                                                                                             \
                     SetConsoleMode (__error_stderr_handle__, ({                                                                                                                   \
                                         DWORD __error_consolemode__;                                                                                                              \
                                         GetConsoleMode (__error_stderr_handle__, &__error_consolemode__);                                                                         \
                                         __error_consolemode__ | DISABLE_NEWLINE_AUTO_RETURN |                                                                                     \
                                             ENABLE_VIRTUAL_TERMINAL_PROCESSING;                                                                                                   \
                                     })),                                                                                                                                          \
                         ({                                                                                                                                                        \
                             if (get_log_file () == -1)                                                                                                                            \
                                 WriteFile (                                                                                                                                       \
                                     __error_stderr_handle__, "\033[1m\033[31m", sizeof ("\033[1m\033[31m") - 1, NULL,                                                             \
                                     NULL                                                                                                                                          \
                                 );                                                                                                                                                \
                         }),                                                                                                                                                       \
                         WriteFile (                                                                                                                                               \
                             __error_stderr_handle__, "Error in function ", sizeof ("Error in function ") - 1, NULL,                                                               \
                             NULL                                                                                                                                                  \
                         ),                                                                                                                                                        \
                         WriteFile (__error_stderr_handle__, __func__, sizeof (__func__) - 1, NULL, NULL),                                                                         \
                         WriteFile (__error_stderr_handle__, " (", sizeof (" (") - 1, NULL, NULL),                                                                                 \
                         WriteFile (__error_stderr_handle__, __FILE__, sizeof (__FILE__) - 1, NULL, NULL),                                                                         \
                         WriteFile (__error_stderr_handle__, ": line ", sizeof (": line ") - 1, NULL, NULL), ({                                                                    \
                             char     __error_line_digits__ [10];                                                                                                                  \
                             uint32_t __error_line__      = (uint32_t) __LINE__;                                                                                                   \
                             uint32_t __error_decplaces__ = (uint32_t) decplaces (__LINE__);                                                                                       \
                             for (uint32_t __error_iter__ = 1; __error_line__; __error_line__ /= 10)                                                                               \
                                 *(__error_line_digits__ + __error_decplaces__ - __error_iter__++) =                                                                               \
                                     (char) (__error_line__ % 10) + '0';                                                                                                           \
                             WriteFile (                                                                                                                                           \
                                 __error_stderr_handle__, __error_line_digits__, __error_decplaces__, NULL, NULL                                                                   \
                             );                                                                                                                                                    \
                         }),                                                                                                                                                       \
                         WriteFile (__error_stderr_handle__, ")", sizeof (")") - 1, NULL, NULL), ({                                                                                \
                             if (get_log_file () == -1)                                                                                                                            \
                                 WriteFile (                                                                                                                                       \
                                     __error_stderr_handle__,                                                                                                                      \
                                     __builtin_choose_expr (                                                                                                                       \
                                         sizeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) == 1, ".\033[0m",                                                                          \
                                         ":\033[0m " ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")                                                                                         \
                                     ),                                                                                                                                            \
                                     sizeof (__builtin_choose_expr (                                                                                                               \
                                         sizeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) == 1, ".\033[0m",                                                                          \
                                         ":\033[0m " ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")                                                                                         \
                                     )) - 1,                                                                                                                                       \
                                     NULL, NULL                                                                                                                                    \
                                 );                                                                                                                                                \
                             else                                                                                                                                                  \
                                 WriteFile (                                                                                                                                       \
                                     __error_stderr_handle__,                                                                                                                      \
                                     __builtin_choose_expr (                                                                                                                       \
                                         sizeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) == 1, ".",                                                                                 \
                                         ": " ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")                                                                                                \
                                     ),                                                                                                                                            \
                                     sizeof (__builtin_choose_expr (                                                                                                               \
                                         sizeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) == 1, ".",                                                                                 \
                                         ": " ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")                                                                                                \
                                     )) - 1,                                                                                                                                       \
                                     NULL, NULL                                                                                                                                    \
                                 );                                                                                                                                                \
                         }),                                                                                                                                                       \
                         WriteFile (__error_stderr_handle__, "\n", sizeof ("\n") - 1, NULL, NULL), close_log_file (),                                                              \
                         print_backtrace (), ({                                                                                                                                    \
                             _Pragma ("GCC diagnostic push");                                                                                                                      \
                             _Pragma ("GCC diagnostic ignored \"-Waddress\"");                                                                                                     \
                             _Pragma ("GCC diagnostic ignored \"-Wincompatible-pointer-types\"");                                                                                  \
                             _Pragma ("GCC diagnostic ignored \"-Wcast-function-type\"");                                                                                          \
                             _Pragma ("GCC diagnostic ignored \"-Wbad-function-cast\"");                                                                                           \
                             if (__builtin_choose_expr (                                                                                                                           \
                                     __builtin_classify_type (ARG2 (__VA_ARGS__ __VA_OPT__ (, ) NULL, NULL)) ==                                                                    \
                                         pointer_type_class,                                                                                                                       \
                                     ARG2 (__VA_ARGS__ __VA_OPT__ (, ) NULL, NULL), NULL                                                                                           \
                                 ))                                                                                                                                                \
                                 (__builtin_choose_expr (                                                                                                                          \
                                     __builtin_types_compatible_p (                                                                                                                \
                                         typeof (ARG2 (                                                                                                                            \
                                             __VA_ARGS__ __VA_OPT__ (, ) ((errorfunc_t *) NULL),                                                                                   \
                                             ((errorfunc_t *) NULL)                                                                                                                \
                                         )),                                                                                                                                       \
                                         errorfunc_t                                                                                                                               \
                                     ) ||                                                                                                                                          \
                                         __builtin_types_compatible_p (typeof (ARG2 (__VA_ARGS__ __VA_OPT__ (, ) ((errorfunc_t *) NULL), ((errorfunc_t *) NULL))), errorfunc_t *), \
                                     ARG2 (                                                                                                                                        \
                                         __VA_ARGS__ __VA_OPT__ (, ) ((errorfunc_t *) NULL), ((errorfunc_t *) NULL)                                                                \
                                     ),                                                                                                                                            \
                                     (errorfunc_t *) NULL                                                                                                                          \
                                 )                                                                                                                                                 \
                                 ) (__builtin_choose_expr (                                                                                                                        \
                                     __builtin_classify_type (ARG3 (__VA_ARGS__ __VA_OPT__ (, ) NULL, NULL, NULL)) ==                                                              \
                                         pointer_type_class,                                                                                                                       \
                                     ARG3 (__VA_ARGS__ __VA_OPT__ (, ) NULL, NULL, NULL), NULL                                                                                     \
                                 ));                                                                                                                                               \
                             _Pragma ("GCC diagnostic pop");                                                                                                                       \
                         }),                                                                                                                                                       \
                         __builtin_choose_expr (                                                                                                                                   \
                             __builtin_has_attribute (                                                                                                                             \
                                 ARG2 (__VA_ARGS__ __VA_OPT__ (, ) ((errorfunc_t *) NULL), ((errorfunc_t *) NULL)),                                                                \
                                 __noreturn__                                                                                                                                      \
                             ) ||                                                                                                                                                  \
                                 __builtin_has_attribute (                                                                                                                         \
                                     ARG2 (                                                                                                                                        \
                                         __VA_ARGS__ __VA_OPT__ (, ) ((errorfunc_t *) NULL), ((errorfunc_t *) NULL)                                                                \
                                     ),                                                                                                                                            \
                                     noreturn                                                                                                                                      \
                                 ),                                                                                                                                                \
                             (trap (), unreachable ()),                                                                                                                            \
                             (({                                                                                                                                                   \
                                  if (is_log_window ()) {                                                                                                                          \
                                      int line;                                                                                                                                    \
                                      if ((line = inc_last_log_line ()) >= (int) get_log_window_height () - 3) {                                                                   \
                                          clear_log_window ();                                                                                                                     \
                                          line = inc_last_log_line ();                                                                                                             \
                                      }                                                                                                                                            \
                                      if (has_colors ())                                                                                                                           \
                                          wattron (get_log_window (), COLOR_PAIR (log_error + 1) | A_BOLD);                                                                        \
                                      mvwprintw (                                                                                                                                  \
                                          get_log_window (), line, 1, "Error in function %s (%s: line %d)%s",                                                                      \
                                          __func__, __FILE__, __LINE__,                                                                                                            \
                                          sizeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) == 1 ? "." : ": "                                                                         \
                                      );                                                                                                                                           \
                                      if (has_colors ())                                                                                                                           \
                                          wattroff (get_log_window (), COLOR_PAIR (log_error + 1) | A_BOLD);                                                                       \
                                      __builtin_choose_expr (                                                                                                                      \
                                          sizeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) == 1, (void) 0,                                                                           \
                                          waddstr (get_log_window (), ARG1 (__VA_ARGS__ __VA_OPT__ (, ) ""))                                                                       \
                                      );                                                                                                                                           \
                                      refresh_log_window ();                                                                                                                       \
                                  }                                                                                                                                                \
                                  if (!isendwin ()) {                                                                                                                              \
                                      refresh_log_window ();                                                                                                                       \
                                      def_prog_mode ();                                                                                                                            \
                                      end_ui ();                                                                                                                                   \
                                      reset_prog_mode ();                                                                                                                          \
                                      for (int __error_iter__ =                                                                                                                    \
                                               ({                                                                                                                                  \
                                                   int __error_h__;                                                                                                                \
                                                   getmaxyx (get_log_window (), __error_h__, (int) { 0 });                                                                         \
                                                   __error_h__;                                                                                                                    \
                                               }) *                                                                                                                                \
                                               2;                                                                                                                                  \
                                           __error_iter__-- > 0; WriteFile (                                                                                                       \
                                               GetStdHandle (STD_OUTPUT_HANDLE), "\n", sizeof ("\n") - 1, NULL, NULL                                                               \
                                           ))                                                                                                                                      \
                                          ;                                                                                                                                        \
                                  }                                                                                                                                                \
                              }),                                                                                                                                                  \
                              trap (), unreachable ())                                                                                                                             \
                         );                                                                                                                                                        \
                 }))
        #endif
    #else
        #ifdef __cplusplus
            #define error(...)                                                                                                  \
                (ct_error (                                                                                                     \
                     NARGS (__VA_ARGS__) > 3,                                                                                   \
                     "the error() function-like macro must be passed between zero and three arguments."                         \
                 ),                                                                                                             \
                 ct_error (                                                                                                     \
                     !(std::is_same<                                                                                            \
                           decltype (ARG2 (__VA_ARGS__ __VA_OPT__ (, ) ((errorfunc_t *) NULL), ((errorfunc_t *) NULL))          \
                           ),                                                                                                   \
                           errorfunc_t>::value ||                                                                               \
                       std::is_same<                                                                                            \
                           decltype (ARG2 (__VA_ARGS__ __VA_OPT__ (, ) ((errorfunc_t *) NULL), ((errorfunc_t *) NULL))          \
                           ),                                                                                                   \
                           errorfunc_t *>::value),                                                                              \
                     "the second argument passed to the error() function-like macro must be of type errorfunc_t *."             \
                 ),                                                                                                             \
                 ct_error (                                                                                                     \
                     !std::is_pointer<std::decay<decltype (ARG3 (                                                               \
                         __VA_ARGS__ __VA_OPT__ (, ) ((void *) NULL), ((void *) NULL), ((void *) NULL)                          \
                     ))>::type>::value,                                                                                         \
                     "the third argument passed to the error() function-like macro must be of a type that decays to a pointer." \
                 ),                                                                                                             \
                 ({                                                                                                             \
                     if (get_log_file () == -1)                                                                                 \
                         write (STDERR_FILENO, "\033[1m\033[31m", sizeof ("\033[1m\033[31m") - 1);                              \
                 }), /* error() will be using async-signal safe functions until calling the provided function in                \
                        order to serve as a signal-handling function. */                                                        \
                 write (STDERR_FILENO, "Error in function ", sizeof ("Error in function ") - 1),                                \
                 write (STDERR_FILENO, __func__, sizeof (__func__) - 1),                                                        \
                 write (STDERR_FILENO, " (", sizeof (" (") - 1),                                                                \
                 write (STDERR_FILENO, __FILE__, sizeof (__FILE__) - 1),                                                        \
                 write (STDERR_FILENO, ": line ", sizeof (": line ") - 1), ({                                                   \
                     char     __error_line_digits__ [10];                                                                       \
                     uint32_t __error_line__      = (uint32_t) __LINE__;                                                        \
                     uint32_t __error_decplaces__ = (uint32_t) decplaces ((uint32_t) __LINE__);                                            \
                     for (uint32_t __error_iter__ = 1; __error_line__; __error_line__ /= 10)                                    \
                         *(__error_line_digits__ + __error_decplaces__ - __error_iter__++) =                                    \
                             (char) (__error_line__ % 10) + '0';                                                                \
                     write (STDERR_FILENO, __error_line_digits__, __error_decplaces__);                                         \
                 }),                                                                                                            \
                 write (STDERR_FILENO, ")", sizeof (")") - 1), ({                                                               \
                     if (get_log_file () == -1)                                                                                 \
                         write (                                                                                                \
                             STDERR_FILENO, ([] () {                                                                            \
                                 if (sizeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) == 1)                                       \
                                     return ".\033[0m";                                                                         \
                                 return ":\033[0m " ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "");                                      \
                             }) (),                                                                                             \
                             ([] () {                                                                                           \
                                 if (sizeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) == 1)                                       \
                                     return sizeof (".\033[0m") - 1;                                                            \
                                 return sizeof (":\033[0m " ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) - 1;                         \
                             }) ()                                                                                              \
                         );                                                                                                     \
                     else                                                                                                       \
                         write (                                                                                                \
                             STDERR_FILENO, ([] () {                                                                            \
                                 if (sizeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) == 1)                                       \
                                     return ".";                                                                                \
                                 return ": " ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "");                                             \
                             }) (),                                                                                             \
                             ([] () {                                                                                           \
                                 if (sizeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) == 1)                                       \
                                     return sizeof (".") - 1;                                                                   \
                                 return sizeof (": " ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) - 1;                                \
                             }) ()                                                                                              \
                         );                                                                                                     \
                 }),                                                                                                            \
                 write (STDERR_FILENO, "\n", sizeof ("\n") - 1), close_log_file (),                                             \
                 print_backtrace (), /* I know this is not an async-safe approach and that I should not be doing                \
                    this but its not as if errors while displaying mattered too much at this point */                           \
                 ({                                                                                                             \
                     _Pragma ("GCC diagnostic push");                                                                           \
                     _Pragma ("GCC diagnostic ignored \"-Waddress\"");                                                          \
                     _Pragma ("GCC diagnostic ignored \"-Wcast-function-type\"");                                               \
                     if (ARG2 (__VA_ARGS__ __VA_OPT__ (, ) NULL, NULL))                                                         \
                         (ARG2 (__VA_ARGS__ __VA_OPT__ (, ) ((errorfunc_t *) NULL), ((errorfunc_t *) NULL))                     \
                         ) (ARG3 (__VA_ARGS__ __VA_OPT__ (, ) NULL, NULL, NULL));                                               \
                     _Pragma ("GCC diagnostic pop");                                                                            \
                 }),                                                                                                            \
                 ([] () {                                                                                                       \
                     if (__builtin_has_attribute (                                                                              \
                             ARG2 (__VA_ARGS__ __VA_OPT__ (, ) ((errorfunc_t *) NULL), ((errorfunc_t *) NULL)),                 \
                             __noreturn__                                                                                       \
                         ) ||                                                                                                   \
                         __builtin_has_attribute (                                                                              \
                             ARG2 (__VA_ARGS__ __VA_OPT__ (, ) ((errorfunc_t *) NULL), ((errorfunc_t *) NULL)),                 \
                             noreturn                                                                                           \
                         ))                                                                                                     \
                         return trap (), unreachable ();                                                                        \
                     if (is_log_window ()) {                                                                                    \
                         int line;                                                                                              \
                         if ((line = inc_last_log_line ()) >= (int) get_log_window_height () - 3) {                             \
                             clear_log_window ();                                                                               \
                             line = inc_last_log_line ();                                                                       \
                         }                                                                                                      \
                         if (has_colors ())                                                                                     \
                             wattron (get_log_window (), COLOR_PAIR (log_error + 1) | A_BOLD);                                  \
                         mvwprintw (                                                                                            \
                             get_log_window (), line, 1, "Error in function %s (%s: line %d)%s", __func__, __FILE__,            \
                             __LINE__, sizeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) == 1 ? "." : ": "                         \
                         );                                                                                                     \
                         if (has_colors ())                                                                                     \
                             wattroff (get_log_window (), COLOR_PAIR (log_error + 1) | A_BOLD);                                 \
                         if (sizeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) == 1)                                               \
                             waddstr (get_log_window (), ARG1 (__VA_ARGS__ __VA_OPT__ (, ) ""));                                \
                         refresh_log_window ();                                                                                 \
                     }                                                                                                          \
                     if (!isendwin ()) {                                                                                        \
                         refresh_log_window ();                                                                                 \
                         def_prog_mode ();                                                                                      \
                         end_ui ();                                                                                             \
                         reset_prog_mode ();                                                                                    \
                         int __error_ignore__;                                                                                  \
                         for (int __error_iter__ = ({                                                                           \
                                                       int __error_h__;                                                         \
                                                       getmaxyx (get_log_window (), __error_h__, __error_ignore__);             \
                                                       __error_h__;                                                             \
                                                   }) *                                                                         \
                                                   2;                                                                           \
                              __error_iter__-- > 0; write (STDOUT_FILENO, "\n", sizeof ("\n") - 1))                             \
                             ;                                                                                                  \
                     }                                                                                                          \
                     return trap (), unreachable ();                                                                            \
                 }) ())
        #else
            #define error(...)                                                                                                                                             \
                (ct_error (                                                                                                                                                \
                     NARGS (__VA_ARGS__) > 3,                                                                                                                              \
                     "the error() function-like macro must be passed between zero and three arguments."                                                                    \
                 ),                                                                                                                                                        \
                 ct_error (                                                                                                                                                \
                     !(__builtin_types_compatible_p (                                                                                                                      \
                           typeof (ARG2 (__VA_ARGS__ __VA_OPT__ (, ) ((errorfunc_t *) NULL), ((errorfunc_t *) NULL))),                                                     \
                           errorfunc_t                                                                                                                                     \
                       ) ||                                                                                                                                                \
                       __builtin_types_compatible_p (typeof (ARG2 (__VA_ARGS__ __VA_OPT__ (, ) ((errorfunc_t *) NULL), ((errorfunc_t *) NULL))), errorfunc_t *)            \
                     ),                                                                                                                                                    \
                     "the second argument passed to the error() function-like macro must be of type errorfunc_t *."                                                        \
                 ),                                                                                                                                                        \
                 ct_error (                                                                                                                                                \
                     __builtin_classify_type (ARG3 (__VA_ARGS__ __VA_OPT__ (, ) NULL, NULL, NULL)) !=                                                                      \
                         pointer_type_class,                                                                                                                               \
                     "the third argument passed to the error() function-like macro must be of a type that decays to a pointer."                                            \
                 ),                                                                                                                                                        \
                 ({                                                                                                                                                        \
                     if (get_log_file () == -1)                                                                                                                            \
                         write (STDERR_FILENO, "\033[1m\033[31m", sizeof ("\033[1m\033[31m") - 1);                                                                         \
                 }), /* error() will be using async-signal safe functions until calling the provided function in                                                           \
                        order to serve as a signal-handling function. */                                                                                                   \
                 write (STDERR_FILENO, "Error in function ", sizeof ("Error in function ") - 1),                                                                           \
                 write (STDERR_FILENO, __func__, sizeof (__func__) - 1),                                                                                                   \
                 write (STDERR_FILENO, " (", sizeof (" (") - 1),                                                                                                           \
                 write (STDERR_FILENO, __FILE__, sizeof (__FILE__) - 1),                                                                                                   \
                 write (STDERR_FILENO, ": line ", sizeof (": line ") - 1), ({                                                                                              \
                     char     __error_line_digits__ [10];                                                                                                                  \
                     uint32_t __error_line__      = (uint32_t) __LINE__;                                                                                                   \
                     uint32_t __error_decplaces__ = (uint32_t) decplaces (__LINE__);                                                                                       \
                     for (uint32_t __error_iter__ = 1; __error_line__; __error_line__ /= 10)                                                                               \
                         *(__error_line_digits__ + __error_decplaces__ - __error_iter__++) =                                                                               \
                             (char) (__error_line__ % 10) + '0';                                                                                                           \
                     write (STDERR_FILENO, __error_line_digits__, __error_decplaces__);                                                                                    \
                 }),                                                                                                                                                       \
                 write (STDERR_FILENO, ")", sizeof (")") - 1), ({                                                                                                          \
                     if (get_log_file () == -1)                                                                                                                            \
                         write (                                                                                                                                           \
                             STDERR_FILENO,                                                                                                                                \
                             __builtin_choose_expr (                                                                                                                       \
                                 sizeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) == 1, ".\033[0m",                                                                          \
                                 ":\033[0m " ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")                                                                                         \
                             ),                                                                                                                                            \
                             sizeof (__builtin_choose_expr (                                                                                                               \
                                 sizeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) == 1, ".\033[0m",                                                                          \
                                 ":\033[0m " ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")                                                                                         \
                             )) - 1                                                                                                                                        \
                         );                                                                                                                                                \
                     else                                                                                                                                                  \
                         write (                                                                                                                                           \
                             STDERR_FILENO,                                                                                                                                \
                             __builtin_choose_expr (                                                                                                                       \
                                 sizeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) == 1, ".",                                                                                 \
                                 ": " ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")                                                                                                \
                             ),                                                                                                                                            \
                             sizeof (__builtin_choose_expr (                                                                                                               \
                                 sizeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) == 1, ".",                                                                                 \
                                 ": " ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")                                                                                                \
                             )) - 1                                                                                                                                        \
                         );                                                                                                                                                \
                 }),                                                                                                                                                       \
                 write (STDERR_FILENO, "\n", sizeof ("\n") - 1), close_log_file (),                                                                                        \
                 print_backtrace (), /* I know this is not an async-safe approach and that I should not be doing                                                           \
                    this but its not as if errors while displaying mattered too much at this point */                                                                      \
                 ({                                                                                                                                                        \
                     _Pragma ("GCC diagnostic push");                                                                                                                      \
                     _Pragma ("GCC diagnostic ignored \"-Waddress\"");                                                                                                     \
                     _Pragma ("GCC diagnostic ignored \"-Wincompatible-pointer-types\"");                                                                                  \
                     _Pragma ("GCC diagnostic ignored \"-Wcast-function-type\"");                                                                                          \
                     _Pragma ("GCC diagnostic ignored \"-Wbad-function-cast\"");                                                                                           \
                     if (__builtin_choose_expr (                                                                                                                           \
                             __builtin_classify_type (ARG2 (__VA_ARGS__ __VA_OPT__ (, ) NULL, NULL)) ==                                                                    \
                                 pointer_type_class,                                                                                                                       \
                             ARG2 (__VA_ARGS__ __VA_OPT__ (, ) NULL, NULL), NULL                                                                                           \
                         ))                                                                                                                                                \
                         (__builtin_choose_expr (                                                                                                                          \
                             __builtin_types_compatible_p (                                                                                                                \
                                 typeof (ARG2 (                                                                                                                            \
                                     __VA_ARGS__ __VA_OPT__ (, ) ((errorfunc_t *) NULL), ((errorfunc_t *) NULL)                                                            \
                                 )),                                                                                                                                       \
                                 errorfunc_t                                                                                                                               \
                             ) ||                                                                                                                                          \
                                 __builtin_types_compatible_p (typeof (ARG2 (__VA_ARGS__ __VA_OPT__ (, ) ((errorfunc_t *) NULL), ((errorfunc_t *) NULL))), errorfunc_t *), \
                             ARG2 (__VA_ARGS__ __VA_OPT__ (, ) ((errorfunc_t *) NULL), ((errorfunc_t *) NULL)),                                                            \
                             (errorfunc_t *) NULL                                                                                                                          \
                         )                                                                                                                                                 \
                         ) (__builtin_choose_expr (                                                                                                                        \
                             __builtin_classify_type (ARG3 (__VA_ARGS__ __VA_OPT__ (, ) NULL, NULL, NULL)) ==                                                              \
                                 pointer_type_class,                                                                                                                       \
                             ARG3 (__VA_ARGS__ __VA_OPT__ (, ) NULL, NULL, NULL), NULL                                                                                     \
                         ));                                                                                                                                               \
                     _Pragma ("GCC diagnostic pop");                                                                                                                       \
                 }),                                                                                                                                                       \
                 __builtin_choose_expr (                                                                                                                                   \
                     __builtin_has_attribute (                                                                                                                             \
                         ARG2 (__VA_ARGS__ __VA_OPT__ (, ) ((errorfunc_t *) NULL), ((errorfunc_t *) NULL)),                                                                \
                         __noreturn__                                                                                                                                      \
                     ) ||                                                                                                                                                  \
                         __builtin_has_attribute (                                                                                                                         \
                             ARG2 (__VA_ARGS__ __VA_OPT__ (, ) ((errorfunc_t *) NULL), ((errorfunc_t *) NULL)),                                                            \
                             noreturn                                                                                                                                      \
                         ),                                                                                                                                                \
                     (trap (), unreachable ()),                                                                                                                            \
                     (({                                                                                                                                                   \
                          if (is_log_window ()) {                                                                                                                          \
                              int line;                                                                                                                                    \
                              if ((line = inc_last_log_line ()) >= (int) get_log_window_height () - 3) {                                                                   \
                                  clear_log_window ();                                                                                                                     \
                                  line = inc_last_log_line ();                                                                                                             \
                              }                                                                                                                                            \
                              if (has_colors ())                                                                                                                           \
                                  wattron (get_log_window (), COLOR_PAIR (log_error + 1) | A_BOLD);                                                                        \
                              mvwprintw (                                                                                                                                  \
                                  get_log_window (), line, 1, "Error in function %s (%s: line %d)%s", __func__,                                                            \
                                  __FILE__, __LINE__, sizeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) == 1 ? "." : ": "                                                     \
                              );                                                                                                                                           \
                              if (has_colors ())                                                                                                                           \
                                  wattroff (get_log_window (), COLOR_PAIR (log_error + 1) | A_BOLD);                                                                       \
                              __builtin_choose_expr (                                                                                                                      \
                                  sizeof (ARG1 (__VA_ARGS__ __VA_OPT__ (, ) "")) == 1, (void) 0,                                                                           \
                                  waddstr (get_log_window (), ARG1 (__VA_ARGS__ __VA_OPT__ (, ) ""))                                                                       \
                              );                                                                                                                                           \
                              refresh_log_window ();                                                                                                                       \
                          }                                                                                                                                                \
                          if (!isendwin ()) {                                                                                                                              \
                              refresh_log_window ();                                                                                                                       \
                              def_prog_mode ();                                                                                                                            \
                              end_ui ();                                                                                                                                   \
                              reset_prog_mode ();                                                                                                                          \
                              for (int __error_iter__ = ({                                                                                                                 \
                                                            int __error_h__;                                                                                               \
                                                            getmaxyx (get_log_window (), __error_h__, (int) { 0 });                                                        \
                                                            __error_h__;                                                                                                   \
                                                        }) *                                                                                                               \
                                                        2;                                                                                                                 \
                                   __error_iter__-- > 0; write (STDOUT_FILENO, "\n", sizeof ("\n") - 1))                                                                   \
                                  ;                                                                                                                                        \
                          }                                                                                                                                                \
                      }),                                                                                                                                                  \
                      trap (), unreachable ())                                                                                                                             \
                 ))
        #endif
    #endif

    #ifdef __cplusplus
        #define log(level, ...)                                                                                        \
            (ct_error (                                                                                                \
                 !(std::is_same<decltype (level), log_level_t>::value),                                                \
                 "the first argument passed to the log() macro must be a log_level_t."                                 \
             ),                                                                                                        \
             level == log_message   ? message (__VA_ARGS__)                                                            \
             : level == log_warning ? warning (__VA_ARGS__)                                                            \
                                    : error (__VA_ARGS__))
    #else
        #define log(level, ...)                                                                                        \
            (ct_error (                                                                                                \
                 !__builtin_types_compatible_p (typeof (level), log_level_t),                                          \
                 "the first argument passed to the log() macro must be a log_level_t."                                 \
             ),                                                                                                        \
             __builtin_choose_expr (                                                                                   \
                 __builtin_constant_p (level),                                                                         \
                 __builtin_choose_expr (                                                                               \
                     __builtin_choose_expr (__builtin_constant_p (level), level, log_message) == log_message,          \
                     message (__VA_ARGS__),                                                                            \
                     __builtin_choose_expr (                                                                           \
                         __builtin_choose_expr (__builtin_constant_p (level), level, log_warning) == log_warning,      \
                         warning (__VA_ARGS__), error (__VA_ARGS__)                                                    \
                     )                                                                                                 \
                 ),                                                                                                    \
                 level == log_message   ? message (__VA_ARGS__)                                                        \
                 : level == log_warning ? warning (__VA_ARGS__)                                                        \
                                        : error (__VA_ARGS__)                                                          \
             ))
    #endif

#endif

#endif
