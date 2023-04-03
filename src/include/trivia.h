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
    #include <csignal>
    #include <cassert>
    #include <cerrno>
    #include <ctime>

    #ifndef _WIN32
        #include <cunistd>
    #endif
#else
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <ctype.h>
    #include <wchar.h>
    #include <locale.h>
    #include <stdbool.h>
    #include <stdint.h>
    #include <signal.h>
    #include <assert.h>
    #include <errno.h>
    #include <time.h>

    #ifndef _WIN32
        #include <unistd.h>
    #endif
#endif
#include <pthread.h>

#include <backtrace.h>

#include <ncursesw/curses.h>
#include <ncursesw/form.h>
#include <ncursesw/menu.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wattributes"

static const int
#if defined(__cpp_attributes) || __STDC_VERSION__ >= 201710L
    [[unused]]
#else
    __attribute__ ((unused))
#endif
    argc = 0;

static const char
#if defined(__cpp_attributes) || __STDC_VERSION__ >= 201710L
    [[unused]]
#else
    __attribute__ ((unused))
#endif
    * argv [] = {};

#pragma GCC diagnostic pop

#include "types.h"
#include "macros.h"
#include "ui.h"
#include "os.h"

#endif
