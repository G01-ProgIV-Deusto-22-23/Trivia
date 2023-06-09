#ifndef TRIVIA_DECPLACES_H
#define TRIVIA_DECPLACES_H

#ifdef __cplusplus
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wunused-function"

template <typename T> static auto constexpr decplaces (T x) -> decltype (x) {
    return (x = x >= 0 ? x : (x < INT64_MAX && ((int64_t) x) == INT64_MIN ? INT64_MAX : -x)) < 10 ? 1
           : x < 100                                                                              ? 2
           : x < 1000                                                                             ? 3
           : x < 10000                                                                            ? 4
           : x < 100000                                                                           ? 5
           : x < 1000000                                                                          ? 6
           : x < 10000000                                                                         ? 7
           : x < 100000000                                                                        ? 8
           : x < 1000000000                                                                       ? 9
           : x < 10000000000                                                                      ? 10
           : x < 100000000000                                                                     ? 11
           : x < 1000000000000                                                                    ? 12
           : x < 10000000000000                                                                   ? 13
           : x < 100000000000000                                                                  ? 14
           : x < 1000000000000000                                                                 ? 15
           : x < 10000000000000000                                                                ? 16
           : x < 100000000000000000                                                               ? 17
           : x < 1000000000000000000                                                              ? 18
                                                                                                  : 19;
}

    #pragma GCC diagnostic pop
#else
    #define decplaces(x)                                                                                               \
        __builtin_choose_expr (__builtin_constant_p (x), sizeof (stringify (x)) - 1, ({                                \
                                   _Pragma ("GCC diagnostic push");                                                    \
                                   _Pragma ("GCC diagnostic ignored \"-Wshadow=local\"");                              \
                                   _Pragma ("GCC diagnostic ignored \"-Wshadow=compatible-local\"");                   \
                                   _Pragma ("GCC diagnostic ignored \"-Wshadow=compatible-local\"");                   \
                                   int64_t __deplaces_temp__ = (int64_t) (x);                                          \
                                   _Pragma ("GCC diagnostic pop");                                                     \
                                   if (__deplaces_temp__ < 0)                                                          \
                                       __deplaces_temp__ =                                                             \
                                           __deplaces_temp__ == INT64_MIN ? INT64_MAX : -__deplaces_temp__;            \
                                   (size_t                                                                             \
                                   ) (__deplaces_temp__ < 10                    ? 1                                    \
                                      : __deplaces_temp__ < 100                 ? 2                                    \
                                      : __deplaces_temp__ < 1000                ? 3                                    \
                                      : __deplaces_temp__ < 10000               ? 4                                    \
                                      : __deplaces_temp__ < 100000              ? 5                                    \
                                      : __deplaces_temp__ < 1000000             ? 6                                    \
                                      : __deplaces_temp__ < 10000000            ? 7                                    \
                                      : __deplaces_temp__ < 100000000           ? 8                                    \
                                      : __deplaces_temp__ < 1000000000          ? 9                                    \
                                      : __deplaces_temp__ < 10000000000         ? 10                                   \
                                      : __deplaces_temp__ < 100000000000        ? 11                                   \
                                      : __deplaces_temp__ < 1000000000000       ? 12                                   \
                                      : __deplaces_temp__ < 10000000000000      ? 13                                   \
                                      : __deplaces_temp__ < 100000000000000     ? 14                                   \
                                      : __deplaces_temp__ < 1000000000000000    ? 15                                   \
                                      : __deplaces_temp__ < 10000000000000000   ? 16                                   \
                                      : __deplaces_temp__ < 100000000000000000  ? 17                                   \
                                      : __deplaces_temp__ < 1000000000000000000 ? 18                                   \
                                                                                : 19);                                 \
                               }))
#endif

#endif