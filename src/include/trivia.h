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
    #include <cinttypes>
    #include <csignal>
    #include <cassert>
    #include <cerrno>
    #include <ctime>
    #include <atomic>
    #include <climits>
#else
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <ctype.h>
    #include <wchar.h>
    #include <locale.h>
    #include <stdbool.h>
    #include <inttypes.h>
    #include <signal.h>
    #include <assert.h>
    #include <errno.h>
    #include <time.h>
    #include <stdatomic.h>
#endif

#include <libgen.h>

#include <backtrace.h>

#include <ncursesw/curses.h>
#include <ncursesw/form.h>
#include <ncursesw/menu.h>

#include <sqlite3.h>
#include <sqlite3rc.h>

#include "types.h"
#include "macros.h"
#include "ui.h"
#include "os.h"
#include "server.h"
#include "bd.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wattributes"

static const int
#if defined(__cpp_attributes) || __STDC_VERSION__ >= 201710L
    [[unused]]
#else
    __attribute__ ((unused))
#endif
    argc;

static const char
#if defined(__cpp_attributes) || __STDC_VERSION__ >= 201710L
    [[unused]]
#else
    __attribute__ ((unused))
#endif
    * argv [1];

#ifdef _WIN32
static LPWSTR
    #if defined(__cpp_attributes) || __STDC_VERSION__ >= 201710L
    [[unused]]
    #else
    __attribute__ ((unused))
    #endif
    pCmdLine;
#endif

#pragma GCC diagnostic pop

#endif
