#ifndef TRIVIA_OS_LINUX_H
#define TRIVIA_OS_LINUX_H

#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <byteswap.h>

#include <bits/wordsize.h>

#ifndef __WORDSIZE
    #define __WORDSIZE BITS_PER_LONG
#endif

#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <linux/futex.h>
#include <sys/syscall.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>

#endif