#ifndef TRIVIA_STRINGIFY_H
#define TRIVIA_STRINGIFY_H

#define stringify_helper(x) #x
#define stringify(x)        stringify_helper (x)

#endif