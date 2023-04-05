#ifndef TRIVIA_MACROS_MAX_H
#define TRIVIA_MACROS_MAX_H

#define max(x, y)                                                                                                      \
    ({                                                                                                                 \
        _Pragma ("GCC diagnostic push");                                                                               \
        _Pragma ("GCC diagnostic ignored \"-Wshadow=local\"");                                                         \
        _Pragma ("GCC diagnostic ignored \"-Wshadow=compatible-local\"");                                              \
        auto __max_temp_x__ = (__extension__ x);                                                                       \
        auto __max_temp_y__ = (__extension__ y);                                                                       \
        _Pragma ("GCC diagnostic pop");                                                                                \
        __max_temp_x__ > __max_temp_y__ ? __max_temp_x__ : __max_temp_y__;                                             \
    })

#endif
