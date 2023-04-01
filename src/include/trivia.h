#ifndef TRIVIA_H
#define TRIVIA_H

#ifdef __cplusplus
    #include <cstdio>
    #include <type_traits>
    #include <cstdlib>
    #include <cstring>
    #include <cctype>
    #include <cwchar>
    #include <clocale>
    #include <cunistd>
    #include <csignal>
    #include <cassert>
    #include <cerrno>
    #include <ctime>
#else
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <ctype.h>
    #include <wchar.h>
    #include <locale.h>
    #include <stdbool.h>
    #include <stdint.h>
    #include <unistd.h>
    #include <signal.h>
    #include <assert.h>
    #include <errno.h>
    #include <time.h>
#endif
#include <pthread.h>

#include <backtrace.h>
#include <curses.h>
#include <form.h>
#include <menu.h>

#ifdef _WIN32
    #include <ncurses_dll.h>
    #include <ncurses_mingw.h>
#endif

#include "types.h"
#include "macros.h"
#include "ui.h"
#include "os.h"

#endif
