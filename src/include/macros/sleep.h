#ifndef TRIVIA_SLEEP_H
#define TRIVIA_SLEEP_H

#ifdef _WIN32
    #define sleep(x) Sleep (x * 1000)
#endif

#endif