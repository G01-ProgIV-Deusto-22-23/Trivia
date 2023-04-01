#include "menu.h"

MENU          *MENUS [sizeof (size_t) * __CHAR_BIT__] = { 0 };
_Atomic size_t MENU_CONTROL                           = 0;
int            FREE_MENU_ERR                          = OK;

static char     **MENU_CHOICE_DATA [sizeof (MENUS) / sizeof (*MENUS)][2];
static menutype_t MENU_TYPE [sizeof (MENUS) / sizeof (*MENUS)]      = { [0 ...(sizeof (MENUS) / sizeof (*MENUS) - 1)] =
                                                                            actionmenu };
static void      *MENU_RETS [sizeof (MENUS) / sizeof (*MENUS)]      = { 0 };
static bool       FREE_MENU_RETS [sizeof (MENUS) / sizeof (*MENUS)] = { 0 };

static bool           MENU_GC_FLAG = false;
static pthread_t      MENU_GC_THREAD [1 + sizeof (MENUS) / sizeof (*MENUS)];
static pthread_once_t MENU_GC_ONCE_CONTROL = PTHREAD_ONCE_INIT;

static int         MENU_EXIT_KEY     = DEFAULT_MENU_EXIT_KEY;
static const char *MENU_EXIT_MESSAGE = DEFAULT_MENU_EXIT_MESSAGE;

static void *impl_trivia_free_menu (void *__menu__) {
    size_t menu   = (size_t) (uintptr_t) __menu__;
    FREE_MENU_ERR = OK;

    if (!*(MENUS + menu))
        return *(MENUS + menu);

    waddch (menu_win (*(MENUS + menu)), 'q');

    {
        int err = unpost_menu (*(MENUS + menu));
        if (err != E_OK && err != E_NOT_POSTED)
            warning ("could not hide the menu.");
    }

    delete_window (menu_win (*(MENUS + menu)));
    delete_window (menu_sub (*(MENUS + menu)));

    for (short i       = 0; i < item_count (*(MENUS + menu));
         FREE_MENU_ERR &= free_item (*(menu_items (*(MENUS + menu)) + i++))) {
        free (*(**(MENU_CHOICE_DATA + menu) + i));
        free (*(*(*(MENU_CHOICE_DATA + menu) + 1) + i));
    }

    free (**(MENU_CHOICE_DATA + menu));
    free (*(*(MENU_CHOICE_DATA + menu) + 1));

    if (*(FREE_MENU_RETS + menu))
        free (*(MENU_RETS + menu));

    FREE_MENU_ERR &= free_menu (*(MENUS + menu));

    MENU_CONTROL           ^= 1 << menu;
    return *(MENUS + menu) = *(MENU_RETS + menu) = (void *) (intptr_t) (*(FREE_MENU_RETS + menu) = 0);
}

static void trivia_free_menu (const size_t menu) {
    if (pthread_create (MENU_GC_THREAD + menu, NULL, impl_trivia_free_menu, (void *) (uintptr_t) menu))
        FREE_MENU_ERR = (warning ("could not start the menu freeing function."), ERR);
}

static void impl_menu_gc2 (void) {
    MENU_GC_FLAG = true;
    for (size_t CHOICEMENU_CONTROL_PREV = MENU_CONTROL; MENU_GC_FLAG; CHOICEMENU_CONTROL_PREV = MENU_CONTROL) {
        if (MENU_CONTROL == CHOICEMENU_CONTROL_PREV)
            continue;

        for (size_t i = 0; i < arrsize (MENUS); i++)
            if ((CHOICEMENU_CONTROL_PREV ^ (MENU_CONTROL & (1 << i))))
                trivia_free_menu (i);
    }
}

#if defined(__cplusplus) || __STDC_VERSION__ >= 201710L
static void *impl_menu_gc (void *) {
#else
static void *impl_menu_gc (__attribute__ ((unused)) void *nothing) {
#endif
    if (pthread_once (&MENU_GC_ONCE_CONTROL, impl_menu_gc2))
        error ("could not start the menu garbage collector.");

    return NULL;
}

void start_menu_gc (void) {
    if (pthread_create (MENU_GC_THREAD, NULL, impl_menu_gc, NULL))
        error ("could not start the menu garbage collector.");
}

void stop_menu_gc (void) {
    MENU_GC_FLAG         = false;
    MENU_GC_ONCE_CONTROL = PTHREAD_ONCE_INIT;
}

#if defined(__cpp_attributes) || __STDC_VERSION__ > 201710L
[[nonnull (8), nodiscard]]
#else
__attribute__ ((nonnull (8), warn_unused_result))
#endif
size_t
    impl_create_menu (
        const menutype_t t, uint32_t w, uint32_t h, uint32_t x, uint32_t y, const size_t n,
        const char *const (*const restrict choices) [2], const size_t (*const restrict lens) [2],
        choicefunc_t *const *const restrict funcs
    ) {
    if (MENU_CONTROL == (size_t) -1)
        error ("no more menus can be created.");

    if (!choices)
        error ("The choices pointer cannot be null");

    if (!funcs)
        error ("The funcs pointer cannot be null.");

    size_t menu = 0;
    for (; menu < arrsize (MENUS); menu++)
        if (!(MENU_CONTROL & (1 << menu)))
            break;

    ITEM **items;
    if (!(**(MENU_CHOICE_DATA + menu) = malloc (n * sizeof (char *))) ||
        !(*(*(MENU_CHOICE_DATA + menu) + 1) = malloc (n * sizeof (char *))) ||
        !(items = malloc ((n + 1) * sizeof (ITEM *))))
        error ("could not allocate space for the menu data.");

    for (size_t i = 0; i < n; i++) {
        if (!(*(**(MENU_CHOICE_DATA + menu) + i) = malloc (**(lens + i) + 1)) ||
            !(*(*(*(MENU_CHOICE_DATA + menu) + 1) + i) = malloc (*(*(lens + i) + 1) + 1)))
            error ("could not allocate space for the menu data.");

        *(char *) mempcpy (*(**(MENU_CHOICE_DATA + menu) + i), **(choices + i), **(lens + i))                   = '\0';
        *(char *) mempcpy (*(*(*(MENU_CHOICE_DATA + menu) + 1) + i), *(*(choices + i) + 1), *(*(lens + i) + 1)) = '\0';

        if (!(*(items + i) = new_item (*(**(MENU_CHOICE_DATA + menu) + i), *(*(*(MENU_CHOICE_DATA + menu) + 1) + i))))
            error ("could not create menu item.");

        set_item_userptr (*(items + i), *(funcs + i));
    }
    *(items + n) = NULL;

    if (!(*(MENUS + menu) = new_menu (items)))
        error ("could not create menu");

    fitwin (w, h, x, y);
    {
        WINDOW *win = newwin ((int) h, (int) w, (int) y, (int) x);
        if (set_menu_win (*(MENUS + menu), win) != E_OK)
            error ("could not set the menu window.");

        {
            uint32_t _w = (uint32_t) (getmaxx (win) - getbegx (win) - 2);
            uint32_t _h = (uint32_t) (getmaxy (win) - getbegy (win)) / 2;
            if (_h > 5)
                _h -= 5;
            uint32_t _x = 2;
            uint32_t _y = _h;

            if (set_menu_sub (*(MENUS + menu), derwin (win, (int) _h, (int) _w, (int) _y, (int) _x)) != E_OK)
                error ("could not set the menu subwindow.");
        }
    }
    keypad (menu_win (*(MENUS + menu)), true);
    add_window ((void *) (uintptr_t) menu, true);
    *(MENU_TYPE + menu) = t;
    set_menu_ret (menu, NULL);

    return menu;
}

#if defined(__cpp_attributes) || __STDC_VERSION__ > 201710L
[[nonnull (2)]]
#else
__attribute__ ((nonnull (2)))
#endif
static void
    print_menu_title (const size_t menu, const char *const restrict title) {
    if (mvwprintw (
            menu_win (*(MENUS + menu)), 2,
            (getmaxx (menu_win (*(MENUS + menu))) - getbegx (menu_win (*(MENUS + menu))) - (int) strlen (title)) / 2,
            "%s", title
        ) == ERR)
        warning ("could not print the title.");

    if (mvwhline (
            menu_win (*(MENUS + menu)), 3,
            (getmaxx (menu_win (*(MENUS + menu))) - getbegx (menu_win (*(MENUS + menu))) - (int) strlen (title)) / 2, 0,
            (int) ({
                const char *loc = setlocale (LC_ALL, NULL);
                setlocale (LC_ALL, "es_ES.UTF-8");
                mbtowc (NULL, NULL, 0);
                size_t len = mbstowcs (NULL, title, strlen (title));
                setlocale (LC_ALL, loc);
                len;
            })
        ) == ERR)
        warning ("could not print the title's horizontal line.");
}

#if defined(__cpp_attributes) || __STDC_VERSION__ > 201710L
[[nonnull (2, 3)]]
#else
__attribute__ ((nonnull (2, 3)))
#endif
size_t
    impl_display_menu (const size_t menu, const char *const restrict title, const char *const restrict mark) {
    if (menu >= arrsize (MENUS))
        error ("not a valid menu index.");

    if (!*(MENUS + menu))
        warning ("the menu has not been created.");

    if (set_menu_mark (*(MENUS + menu), mark) != E_OK)
        warning ("could not set the menu mark.");

    if (set_menu_format (
            *(MENUS + menu), getmaxx (menu_sub (*(MENUS + menu))) - getbegx (menu_sub (*(MENUS + menu))), 1
        ) != E_OK)
        warning ("could not set the menu format.");

display:
    box (menu_win (*(MENUS + menu)), 0, 0);
    if (*title)
        print_menu_title (menu, title);

    mvwprintw (
        menu_win (*(MENUS + menu)), getmaxy (menu_win (*(MENUS + menu))) - getbegy (menu_win (*(MENUS + menu))) - 5,
        (int) strlen (mark) + 2, "%s", get_menu_exit_message ()
    );
    refresh ();

    int prevcurs;
    if ((prevcurs = curs_set (0)) == ERR)
        warning ("could not hide the cursor.");

    if (({
            int           err = post_menu (*(MENUS + menu));
            err != E_OK &&err != E_POSTED;
        }))
        warning ("could not show the menu.");

    wrefresh (menu_win (*(MENUS + menu)));
    refresh ();

    if (*(MENU_TYPE + menu) == choicemenu && get_menu_ret (menu))
        *get_menu_ret (menu) = (size_t) -1;

    for (int c = 0; (c = tolower (wgetch (menu_win (*(MENUS + menu))))) != get_menu_exit_key ();
         wrefresh (menu_win (*(MENUS + menu))))
        if (c == KEY_DOWN) {
            if (item_index (current_item (*(MENUS + menu))) == item_count (*(MENUS + menu)) - 1)
                menu_driver (*(MENUS + menu), REQ_FIRST_ITEM);

            else
                menu_driver (*(MENUS + menu), REQ_DOWN_ITEM);

            continue;
        }

        else if (c == KEY_UP) {
            if (!item_index (current_item (*(MENUS + menu))))
                menu_driver (*(MENUS + menu), REQ_LAST_ITEM);

            else
                menu_driver (*(MENUS + menu), REQ_UP_ITEM);

            continue;
        }

        else if (c == ' ' || c == '\r' || c == '\n' || c == KEY_ENTER) {
            if (*(MENU_TYPE + menu) == actionmenu)
                goto action;

            else if (*(MENU_TYPE + menu) == choicemenu) {
                if (get_menu_ret (menu))
                    *get_menu_ret (menu) = (size_t) item_index (current_item (*(MENUS + menu)));

                break;
            }

            else {
                menu_driver (*(MENUS + menu), REQ_TOGGLE_ITEM);

                if (get_menu_ret (menu))
                    *(get_menu_ret (menu) + item_index (current_item (*(MENUS + menu)))) =
                        !*(get_menu_ret (menu) + item_index (current_item (*(MENUS + menu))));
            }

            continue;
        }

    if (prevcurs && prevcurs != ERR)
        if (curs_set (prevcurs) == ERR)
            warning ("could not restore the cursor to its previous state.");

    unpost_menu (*(MENUS + menu));

    return menu;

action:
    unpost_menu (*(MENUS + menu));

    if (item_userptr (current_item (*(MENUS + menu))))
        ((choicefunc_t *) item_userptr (current_item (*(MENUS + menu)))) ();

    goto display;
}

size_t *get_menu_ret (const size_t menu) {
    if (menu >= arrsize (MENUS))
        error ("not a valid menu index.");

    if (!(*(MENU_TYPE + menu) == actionmenu || *(MENU_RETS + menu)))
        return set_menu_ret (menu, NULL);

    return *(MENU_RETS + menu);
}

size_t *set_menu_ret (const size_t menu, size_t *const restrict ret) {
    if (menu >= arrsize (MENUS))
        error ("not a valid menu index.");

    if (!*(MENUS + menu))
        return NULL;

    if (*(FREE_MENU_RETS + menu))
        free (*(MENU_RETS + menu));

    return *(MENU_RETS + menu) = ret ? (size_t *) ret : (*(MENU_TYPE + menu) == actionmenu ? NULL : ({
        size_t *buf;
        if (!(*(FREE_MENU_RETS + menu) =
                  !!(buf = calloc (
                         *(MENU_TYPE + menu) == choicemenu ? 1 : (size_t) item_count (*(MENUS + menu)), sizeof (size_t)
                     ))))
            error ("could not create a menu return buffer.");
        buf;
    }));
}

int get_menu_exit_key (void) {
    return MENU_EXIT_KEY;
}

const char *get_menu_exit_message (void) {
    return MENU_EXIT_MESSAGE;
}

int set_menu_exit_key (const int key, const char *const restrict message) {
    MENU_EXIT_MESSAGE = message ? message : DEFAULT_MENU_EXIT_MESSAGE;

    return MENU_EXIT_KEY = key ? key : DEFAULT_MENU_EXIT_KEY;
}