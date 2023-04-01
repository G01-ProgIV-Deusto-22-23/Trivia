#ifndef TRIVIA_UI_MENU_H
#define TRIVIA_UI_MENU_H

extern MENU          *MENUS [sizeof (size_t) * __CHAR_BIT__];
extern _Atomic size_t MENU_CONTROL;
extern int            FREE_MENU_ERR;

extern void start_menu_gc (void);
extern void stop_menu_gc (void);

extern void *add_window (void *const restrict, const int);

#endif