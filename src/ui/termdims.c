#include <stdint.h>

uint64_t get_term_dims (void) {
    return ((uint64_t) get_term_width () << 32) | get_term_height ();
}

uint32_t get_term_width (void) {
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo (GetStdHandle (STD_OUTPUT_HANDLE), &csbi);
    return GetConsoleScreenBufferInfo (GetStdHandle (STD_OUTPUT_HANDLE), &csbi)
               ? (uint32_t) (csbi.srWindow.Right - csbi.srWindow.Left + 1)
               : (warning ("could not get the terminal width."), UINT32_C (0));
#else
    struct winsize w;
    return ioctl (0, TIOCGWINSZ, &w) == -1 ? (warning ("could not get the terminal width."), UINT32_C (0))
                                           : (uint32_t) w.ws_col;
#endif
}

uint32_t get_term_height (void) {
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    return GetConsoleScreenBufferInfo (GetStdHandle (STD_OUTPUT_HANDLE), &csbi)
               ? (uint32_t) (csbi.srWindow.Bottom - csbi.srWindow.Top + 1)
               : (warning ("could not get the terminal height."), UINT32_C (0));
#else
    struct winsize w;
    return ioctl (0, TIOCGWINSZ, &w) == -1 ? (warning ("could not get the terminal height."), UINT32_C (0))
                                           : (uint32_t) w.ws_row;
#endif
}

#ifdef _WIN32
uint64_t set_term_dims (uint32_t w, uint32_t h) {
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wmaybe-uninitialized"

    HWND                       win;
    HANDLE                     out;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!((win = GetConsoleWindow ()) && (out = GetStdHandle (STD_OUTPUT_HANDLE)) &&
          GetConsoleScreenBufferInfo (GetStdHandle (STD_OUTPUT_HANDLE), &csbi) &&
          SetConsoleScreenBufferSize (
              GetStdHandle (STD_OUTPUT_HANDLE),
              (COORD) { (SHORT
                        ) (w =
                               (w ? ((uint32_t) csbi.srWindow.Left + w)
                                  : (uint32_t) (csbi.srWindow.Right - csbi.srWindow.Left + 1 + csbi.srWindow.Left))),
                        (SHORT
                        ) (h = h ? ((uint32_t) csbi.srWindow.Top + h)
                                 : (uint32_t) (csbi.srWindow.Bottom - csbi.srWindow.Top + 1 + csbi.srWindow.Top)) }
          ) &&
          GetConsoleScreenBufferInfo (GetStdHandle (STD_OUTPUT_HANDLE), &csbi) &&
          SetWindowPos (
              win, 0, 0, 0, (int) (w * 800.0 / 95.0), (int) (h * 1600.0 / 95.0),
              SWP_DRAWFRAME | SWP_FRAMECHANGED | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOCOPYBITS | SWP_SHOWWINDOW
          )
          /* SetConsoleWindowInfo (out, FALSE, &(SMALL_RECT) { 0, 0, csbi.srWindow.Right - 1, csbi.srWindow.Bottom - 1
             }) */
        ) &&
        SetConsoleActiveScreenBuffer (out))
        warning ("could not resize the console window.");

    #pragma GCC diagnostic push

    return get_term_dims ();
}
#endif