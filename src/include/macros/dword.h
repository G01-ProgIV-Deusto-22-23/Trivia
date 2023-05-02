#ifndef TRIVIA_DWORD_H
#define TRIVIA_DWORD_H

#ifndef _WIN32
    #define DWORD_MIN ((DWORD) 0)
    #define DWORD_MAX (((DWORD) 1) << (sizeof (DWORD) * __CHAR_BIT__ - 1))
#endif

#endif