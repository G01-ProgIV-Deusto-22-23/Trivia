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

#include <bits/wordsize.h>

#ifndef __WORDSIZE
    #define __WORDSIZE BITS_PER_LONG
#endif

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>

#endif