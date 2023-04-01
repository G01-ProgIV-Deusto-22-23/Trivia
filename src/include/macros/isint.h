#ifndef TRIVIA_MACROS_ISINT_H
#define TRIVIA_MACROS_ISINT_H

#ifdef __cplusplus

    #define isint(x) std::is_integral<decltype (x)>::value

#else

    #define isint(x) (__builtin_classify_type (x) == integer_type_class)

#endif

#endif