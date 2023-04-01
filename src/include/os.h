#ifndef TRIVIA_OS_H
#define TRIVIA_OS_H

#ifdef _WIN32
    #include "os/windows.h"
#else
    #ifdef __linux__
        #include "os/linux.h"
    #else
        #error "This project can only be compiled for Linux and Windows."
    #endif
#endif

#include "os/signals.h"
#include "os/backtrace.h"

#endif
