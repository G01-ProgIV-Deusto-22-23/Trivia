#ifndef TRIVIA_MAIN_H
#define TRIVIA_MAIN_H

#if defined(_WIN32) && defined(UNICODE)
    #if defined(__cpp_attributes) || __STDC_VERSION__ > 201710L
        #define trivia_main                                                                                            \
            int WINAPI wWinMain (                                                                                      \
                [[unused]] HINSTANCE hInstance, [[unused]] HINSTANCE hPrevInstance, [[unused]] PWSTR pCmdLine,         \
                [[unused]] int nCmdShow                                                                                \
            )
    #else
        #define trivia_main                                                                                            \
            int WINAPI wWinMain (                                                                                      \
                HINSTANCE __attribute__ ((unused)) hInstance, HINSTANCE __attribute__ ((unused)) hPrevInstance,        \
                PWSTR __attribute__ ((unused)) pCmdLine, int __attribute__ ((unused)) nCmdShow                         \
            )
    #endif
#else
    #if defined(__cpp_attributes) || __STDC_VERSION__ > 201710L
        #define trivia_main int main ([[maybe_unused]] int argc, [[maybe_unused]] char **argv)

    #else
        #define trivia_main int main (int __attribute__ ((unused)) argc, char __attribute__ ((unused)) * *argv)
    #endif
#endif

#endif