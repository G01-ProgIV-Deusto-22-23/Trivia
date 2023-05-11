#ifndef TRIVIA_UI_MENU_H
#define TRIVIA_UI_MENU_H

extern MENU         *MENUS [__WORDSIZE];
extern atomic_size_t MENU_CONTROL;
extern atomic_int    FREE_MENU_ERR [sizeof (MENUS) / sizeof (*MENUS)];
#ifdef _WIN32
extern HANDLE FREE_MENU_SEMS [sizeof (MENUS) / sizeof (*MENUS)];
#else
extern sem_t FREE_MENU_SEMS [sizeof (MENUS) / sizeof (*MENUS)];
#endif

extern void trivia_free_menu (const size_t);
extern void start_menu_gc (void);
extern void stop_menu_gc (void);

#endif