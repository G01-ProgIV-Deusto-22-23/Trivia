#ifndef TRIVIA_MAIN_H
#define TRIVIA_MAIN_H

#if defined(_WIN32) && defined(UNICODE)
    #define trivia_main                                                                                                \
        int WINAPI wWinMain (                                                                                          \
            HINSTANCE __attribute__ ((unused)) hInstance, HINSTANCE __attribute__ ((unused)) hPrevInstance,            \
            PWSTR __attribute__ ((unused)) pCmdLine, int __attribute__ ((unused)) nCmdShow                             \
        )
#else
    #define trivia_main int main (int __attribute__ ((unused)) argc, char __attribute__ ((unused)) * *argv)
#endif

#endif