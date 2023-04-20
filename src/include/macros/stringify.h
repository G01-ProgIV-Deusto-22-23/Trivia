#ifndef TRIVIA_STRINGIFY_H
#define TRIVIA_STRINGIFY_H

#define STRINGIFY_HELPER(x) #x
#define STRINGIFY(x)        STRINGIFY_HELPER (x)

#endif