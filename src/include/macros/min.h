#ifndef TRIVIA_MACROS_MIN_H
#define TRIVIA_MACROS_MIN_H

#define min(x, y)                                                                                                      \
    ({                                                                                                                 \
        _Pragma ("GCC diagnostic push");                                                                               \
        _Pragma ("GCC diagnostic ignored \"-Wshadow=local\"");                                                         \
        _Pragma ("GCC diagnostic ignored \"-Wshadow=compatible-local\"");                                              \
        auto __min_temp_x__ = (__extension__ x);                                                                       \
        auto __min_temp_y__ = (__extension__ y);                                                                       \
        _Pragma ("GCC diagnostic pop");                                                                                \
        __min_temp_x__ < __min_temp_y__ ? __min_temp_x__ : __min_temp_y__;                                             \
    })

#endif
