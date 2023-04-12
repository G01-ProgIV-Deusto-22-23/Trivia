#ifndef TRIVIA_OS_WINDOWS_H
#define TRIVIA_OS_WINDOWS_H

#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <share.h>
#include <process.h>

#undef KEY_DOWN
#define KEY_DOWN 2

#undef KEY_UP
#define KEY_UP 3

#undef KEY_LEFT
#define KEY_LEFT 4

#undef KEY_RIGHT
#define KEY_RIGHT 5

#endif