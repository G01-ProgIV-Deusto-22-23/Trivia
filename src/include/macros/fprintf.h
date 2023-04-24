#ifndef TRIVIA_FPRINTF_H
#define TRIVIA_FPRINTF_H

#ifdef _WIN32
    #ifdef _WIN64
        #define PRI_SZ PRIu64
    #else
        #define PR_SZ PRIu32
    #endif
#else
    #define PRI_SZ "zu"
#endif

#ifdef _WIN32
    #define fprintf(stream, format, ...)                                                                               \
        ({                                                                                                             \
            stdin->_file  = _open_osfhandle ((intptr_t) GetStdHandle (STD_INPUT_HANDLE), _O_U8TEXT);                   \
            stdout->_file = _open_osfhandle ((intptr_t) GetStdHandle (STD_OUTPUT_HANDLE), _O_U8TEXT);                  \
            stderr->_file = _open_osfhandle ((intptr_t) GetStdHandle (STD_ERROR_HANDLE), _O_U8TEXT);                   \
            fprintf (stream, format __VA_OPT__ (, ) __VA_ARGS__);                                                      \
        })

    #define fflush(stream)                                                                                             \
        ({                                                                                                             \
            stdin->_file  = _open_osfhandle ((intptr_t) GetStdHandle (STD_INPUT_HANDLE), _O_U8TEXT);                   \
            stdout->_file = _open_osfhandle ((intptr_t) GetStdHandle (STD_OUTPUT_HANDLE), _O_U8TEXT);                  \
            stderr->_file = _open_osfhandle ((intptr_t) GetStdHandle (STD_ERROR_HANDLE), _O_U8TEXT);                   \
            fflush (stream);                                                                                           \
        })
#endif

#endif