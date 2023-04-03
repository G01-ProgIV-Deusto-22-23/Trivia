#ifndef TRIVIA_MACROS_FITWIN_H
#define TRIVIA_MACROS_FITWIN_H

#define fitwin(w, h, x, y)                                                                                             \
    (ct_error (                                                                                                        \
         !(isint (w) && isint (h) && isint (x) && isint (y)),                                                          \
         "all the arguments passed to the fitwin() macro must be of integral type."                                    \
     ),                                                                                                                \
     (void) ({                                                                                                         \
         if (!w)                                                                                                       \
             w = get_term_width () - get_log_window_width () * is_log_window () - 1;                                   \
         w = min (w, get_term_width ());                                                                               \
         w = w <= get_hor_padding () * 2 ? w : w - get_hor_padding () * 2;                                             \
                                                                                                                       \
         if (!h)                                                                                                       \
             h = get_term_height () - get_log_window_height () * is_log_window ();                                     \
         h = min (h, get_term_height ());                                                                              \
         h = h <= get_ver_padding () * 2 ? h : h - get_ver_padding () * 2;                                             \
                                                                                                                       \
         x = max (x, (uint32_t) getbegx (stdscr)) + get_hor_padding ();                                                \
         x = x + w > get_term_width () ? x + get_term_width () - w : x;                                                \
                                                                                                                       \
         y = max (y, (uint32_t) getbegy (stdscr)) + get_ver_padding ();                                                \
         y = y + h > get_term_height () ? y + get_term_height () - h : y;                                              \
     }))

#endif