#ifndef TRIVIA_OS_LINUX_H
#define TRIVIA_OS_LINUX_H

#include <sys/ioctl.h>
#include <fcntl.h>
#include <byteswap.h>

#ifdef __cplusplus
    #include <cunistd>
#else
    #include <unistd.h>
#endif

#include <pthread.h>
#include <semaphore.h>

#endif