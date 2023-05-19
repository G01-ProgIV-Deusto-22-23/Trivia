

#ifndef TRIVIA_H
#define TRIVIA_H

#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-fpermissive"
#pragma GCC diagnostic warning "-Wpragmas"

#ifdef __cplusplus
    #include <cstddef>
    #include <cstdio>
    #include <type_traits>
    #include <cstdlib>
    #include <cstring>
    #include <cctype>
    #include <cwchar>
    #include <cwctype>
    #include <clocale>
    #include <cinttypes>
    #include <csignal>
    #include <cassert>
    #include <cerrno>
    #include <ctime>
    #include <atomic>
    #include <climits>
    #include <memory>
    #include <vector>
    #include <algorithm>
    #include <iostream>
    #include <random>
#else
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <ctype.h>
    #include <wchar.h>
    #include <wctype.h>
    #include <locale.h>
    #include <stdbool.h>
    #include <inttypes.h>
    #include <signal.h>
    #include <assert.h>
    #include <errno.h>
    #include <time.h>
    #include <stdatomic.h>
    #include <limits.h>
#endif

#include <sys/time.h>
#include <libgen.h>

#include <backtrace.h>

#include <ncursesw/curses.h>
#include <ncursesw/form.h>
#include <ncursesw/menu.h>

#include <sqlite3.h>
#include <sqlite3rc.h>

#include <cJSON_Utils.h>

/*
    Por si necesitamos división con macros y tal (https://gustedt.gitlabpages.inria.fr/p99/p99-html/)
    Desactivado por el momento porque seguramente se rompan cosas.
*/

#ifdef TRIVIA_USE_P99
    #include <p99/p99.h>
#endif

#include "types.h"
#include "macros.h"
#include "adt.h"
#include "config.h"
#include "ui.h"
#include "os.h"
#include "server.h"
#include "bd.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wattributes"

static const int __attribute__ ((unused)) argc = 0;

static const char __attribute__ ((unused)) * argv [1];

#ifdef _WIN32
static LPWSTR __attribute__ ((unused)) pCmdLine;
#endif

#pragma GCC diagnostic pop

#endif
