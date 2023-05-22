#ifndef TRIVIA_OS_WINDOWS_H
#define TRIVIA_OS_WINDOWS_H

#include <winsock2.h>
#include <ws2tcpip.h>
#include <wincrypt.h>
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <share.h>
#include <process.h>

#ifdef _WIN64
    #define __WORDSIZE 64
#else
    #define __WORDSIZE 32
#endif

#undef KEY_DOWN
#define KEY_DOWN 2

#undef KEY_UP
#define KEY_UP 3

#undef KEY_LEFT
#define KEY_LEFT 4

#undef KEY_RIGHT
#define KEY_RIGHT 5

#undef KEY_F0
#define KEY_F0 8

#endif