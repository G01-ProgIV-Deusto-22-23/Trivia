#include "menu.h"
#include "window.h"

#ifdef __cplusplus
extern "C" {
#endif

    MENU         *MENUS [__WORDSIZE]                               = { 0 };
    atomic_size_t MENU_CONTROL                                     = 0;
    atomic_int    FREE_MENU_ERR [sizeof (MENUS) / sizeof (*MENUS)] = { [0 ...(sizeof (MENUS) / sizeof (*MENUS) - 1)] =
                                                                           OK };

#ifdef _WIN32
    HANDLE FREE_MENU_SEMS [sizeof (MENUS) / sizeof (*MENUS)] = { 0 };
#else
sem_t            FREE_MENU_SEMS [sizeof (MENUS) / sizeof (*MENUS)];
#endif

    static char     **MENU_CHOICE_DATA [sizeof (MENUS) / sizeof (*MENUS)][2];
    static menutype_t MENU_TYPES [sizeof (MENUS) / sizeof (*MENUS)] = { [0 ...(sizeof (MENUS) / sizeof (*MENUS) - 1)] =
                                                                            actionmenu };
    static bool       MENU_LOOP [sizeof (MENUS) / sizeof (*MENUS)]  = { false };
    static size_t    *MENU_RETS [sizeof (MENUS) / sizeof (*MENUS)]  = { 0 };
    static title_color_t MENU_TITLE_COLORS [sizeof (MENUS) / sizeof (*MENUS)] = { title_no_color };

    static bool FREE_MENU_RETS [sizeof (MENUS) / sizeof (*MENUS)] = { 0 };
#ifdef _WIN32
    static HANDLE FREE_MENU_THREADS [sizeof (MENUS) / sizeof (*MENUS)];
#else
static pthread_t FREE_MENU_THREADS [sizeof (MENUS) / sizeof (*MENUS)];
#endif

    static int  MENU_EXIT_KEY              = DEFAULT_MENU_EXIT_KEY;
    static char MENU_EXIT_MESSAGE [BUFSIZ] = DEFAULT_MENU_EXIT_MESSAGE;

#ifdef _WIN32
    static unsigned long impl_trivia_free_menu (void *__menu__) {
#else
static void     *impl_trivia_free_menu (void *__menu__) {
#endif
        const size_t menu = (size_t) (uintptr_t) __menu__;
        atomic_store (FREE_MENU_ERR + menu, OK);

        if (!*(MENUS + menu))
            return
#ifdef _WIN32
                (unsigned long) (uintptr_t)
#endif
                * (MENUS + menu);

        {
            int err = unpost_menu (*(MENUS + menu));
            if (!(err == E_OK || err == E_NOT_POSTED))
                warning ("could not hide the menu.");
        }

        ITEM **const items = menu_items (*(MENUS + menu));
        size_t       n     = (size_t) item_count (*(MENUS + menu));

        set_menu_items (*(MENUS + menu), NULL);
        for (size_t i = 0; i < n;) {
            free (*(**(MENU_CHOICE_DATA + menu) + i));
            free (*(*(*(MENU_CHOICE_DATA + menu) + 1) + i));

            if (free_item (*(items + i++)) != E_OK) {
                atomic_store (FREE_MENU_ERR + menu, ERR);

                warning ("could not free menu item.");
            }
        }

        free (**(MENU_CHOICE_DATA + menu));
        free (*(*(MENU_CHOICE_DATA + menu) + 1));

        if (*(FREE_MENU_RETS + menu)) {
            free (*(MENU_RETS + menu));
            *(MENU_RETS + menu) = NULL;
        }

        if (free_menu (*(MENUS + menu)) != E_OK) {
            atomic_store (FREE_MENU_ERR + menu, ERR);

            warning ("could not free the menu.");

            return
#ifdef _WIN32
                (unsigned long) (uintptr_t)
#endif
                * (MENUS + menu);
        }

        refresh ();

        atomic_fetch_and (&MENU_CONTROL, ~(((size_t) 1) << menu));
        *(MENUS + menu) =
            (void
                 *) (*(MENU_RETS + menu) = (void *) (uintptr_t) (*(FREE_MENU_RETS + menu) = (*(MENU_TITLE_COLORS + menu) = 0)));

        if (
#ifdef _WIN32
            !ReleaseSemaphore (*(FREE_MENU_SEMS + menu), 1, NULL)
#else
        sem_post (FREE_MENU_SEMS + menu) == -1
#endif
        )
            error ("could not post the semaphore after freeing the menu.");

        return
#ifdef _WIN32
            (unsigned long) (uintptr_t)
#endif
            * (MENUS + menu);
    }

    void trivia_free_menu (const size_t menu) {
        if (menu >= sizeof (MENUS) / sizeof (*MENUS))
            error ("not a valid menu index.");

#ifdef _WIN32
        if (!(*(FREE_MENU_THREADS + menu) = CreateThread (
                  &(SECURITY_ATTRIBUTES
                  ) { .nLength = sizeof (SECURITY_ATTRIBUTES), .lpSecurityDescriptor = NULL, .bInheritHandle = TRUE },
                  0, impl_trivia_free_menu, (void *) (uintptr_t) menu, 0, NULL
              )))
#else
    if (pthread_create (FREE_MENU_THREADS + menu, NULL, impl_trivia_free_menu, (void *) (uintptr_t) menu) == -1)
#endif
        {
            warning ("could not start the menu freeing thread, falling back to manual freeing.");

            impl_trivia_free_menu ((void *) (uintptr_t) menu);
        }
    }

    void start_menu_gc (void) {
#ifdef _WIN32
        static CHAR FREE_MENU_SEM_NAMES [sizeof (MENUS) / sizeof (*MENUS)]
                                        [sizeof ("TRIVIA_FREE_MENU_SEM__") + sizeof (STRINGIFY (DWORD_MAX)) +
                                         sizeof (STRINGIFY (sizeof (MENUS) / sizeof (*MENUS))) - 2];
#endif

        for (size_t i = 0; i <
#ifdef _WIN32
                           sizeof (FREE_MENU_SEMS) / sizeof (*FREE_MENU_SEMS)
#else
                       arrsize (FREE_MENU_SEMS)
#endif
                 ;
             i++)
            if (
#ifdef _WIN32
                !(sprintf (*(FREE_MENU_SEM_NAMES + i), "TRIVIA_FREE_MENU_SEM_%lu_%" PRISZ, GetCurrentProcessId (), i) !=
                      -1 &&
                  CreateSemaphoreA (NULL, 1L, LONG_MAX, *(FREE_MENU_SEM_NAMES + i)) &&
                  (*(FREE_MENU_SEMS + i) =
                       OpenSemaphoreA (SEMAPHORE_MODIFY_STATE | SYNCHRONIZE, FALSE, *(FREE_MENU_SEM_NAMES + i))))
#else
            sem_init (FREE_MENU_SEMS + i, true, 1)
#endif
            )
                error ("could not start the menu garbage collector.");
    }

    void stop_menu_gc (void) {
        for (size_t i = 0; i <
#ifdef _WIN32
                           sizeof (FREE_MENU_SEMS) / sizeof (*FREE_MENU_SEMS)
#else
                       arrsize (FREE_MENU_SEMS)
#endif
                 ;) {
            if (
#ifdef _WIN32
                WaitForSingleObject (*(FREE_MENU_SEMS + i), INFINITE) == WAIT_FAILED
#else
            sem_wait (FREE_MENU_SEMS + i) == -1
#endif
            )
                warning ("could not wait for a semaphore.");

            if (
#ifdef _WIN32
                !CloseHandle (*(FREE_MENU_SEMS + i++))
#else
            sem_destroy (FREE_MENU_SEMS + i++)
#endif
            )
                warning ("could not destroy a semaphore.");
        }
    }

    static size_t impl_create_menu2 (
        const size_t menu, const menutype_t t, uint32_t w, uint32_t h, uint32_t x, uint32_t y, const size_t n,
        const char *const (*const choices) [2], const size_t (*const lens) [2], choicefunc_t *const *const funcs
    ) {
        if (
#ifdef _WIN32
            WaitForSingleObject (*(FREE_MENU_SEMS + menu), INFINITE) == WAIT_FAILED
#else
        sem_wait (FREE_MENU_SEMS + menu) == -1
#endif
        )
            error ("could not wait for the menu to be freed.");

        if (*(MENUS + menu) && !delete_menu (menu))
            error ("the menu index was already in use and could not be properly freed.");

        atomic_fetch_or (&MENU_CONTROL, 1 << menu);

        ITEM **items;
        if (!(**(MENU_CHOICE_DATA + menu) = malloc (n * sizeof (char *))) ||
            !(*(*(MENU_CHOICE_DATA + menu) + 1) = malloc (n * sizeof (char *))) ||
            !(items = malloc ((n + 1) * sizeof (ITEM *))))
            error ("could not allocate space for the menu data.");

        for (size_t i = 0; i < n; i++) {
            if (!(*(**(MENU_CHOICE_DATA + menu) + i) = malloc (**(lens + i) + 1)) ||
                !(*(*(*(MENU_CHOICE_DATA + menu) + 1) + i) = malloc (*(*(lens + i) + 1) + 1)))
                error ("could not allocate space for the menu data.");

            *(char *) mempcpy (*(**(MENU_CHOICE_DATA + menu) + i), **(choices + i), **(lens + i)) = '\0';
            *(char *) mempcpy (*(*(*(MENU_CHOICE_DATA + menu) + 1) + i), *(*(choices + i) + 1), *(*(lens + i) + 1)) =
                '\0';

            if (!(*(items + i) =
                      new_item (*(**(MENU_CHOICE_DATA + menu) + i), *(*(*(MENU_CHOICE_DATA + menu) + 1) + i))))
                error ("could not create menu item.");

            set_item_userptr (*(items + i), *(funcs + i));
        }
        *(items + n) = NULL;

        if (!(*(MENUS + menu) = new_menu ((ITEM **) items)))
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
                fitwin (_w, _h, _x, _y);

                if (set_menu_sub (*(MENUS + menu), derwin (win, (int) _h, (int) _w, (int) _y, (int) _x)) != E_OK)
                    error ("could not set the menu subwindow.");
            }
        }

        add_window ((void *) (uintptr_t) menu, 2);

        keypad (menu_win (*(MENUS + menu)), true);
        *(MENU_LOOP + menu) = ((*(MENU_TYPES + menu) = t) == actionmenu);
        set_menu_ret (menu, NULL);

        if (
#ifdef _WIN32
            !ReleaseSemaphore (*(FREE_MENU_SEMS + menu), 1L, 0)
#else
        sem_post (FREE_MENU_SEMS + menu) == -1
#endif
        )
            error ("could not post the semaphore after creating the menu.");

        return menu;
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
            choicefunc_t *const *const funcs
        ) {
        if (atomic_load (&MENU_CONTROL) == (size_t) -1)
            error ("no more menus can be created.");

        if (!choices)
            error ("The choices pointer cannot be null");

        if (!funcs)
            error ("The funcs pointer cannot be null.");

        size_t menu = 0;
        for (; menu <
#ifdef _WIN32
               sizeof (MENUS) / sizeof (*MENUS)
#else
           arrsize (MENUS)
#endif
                 ;)
            if (!((atomic_load (&MENU_CONTROL) >> menu++) & 1))
                break;

        return impl_create_menu2 (menu, t, w, h, x, y, n, choices, lens, funcs);
    }

    title_color_t get_menu_title_color (const size_t menu) {
        if (menu >=
#ifdef _WIN32
            sizeof (MENUS) / sizeof (*MENUS)
#else
        arrsize (MENUS)
#endif
        )
            error ("not a valid menu index.");

        return *(MENU_TITLE_COLORS + menu);
    }

    title_color_t set_menu_title_color (const size_t menu, const title_color_t color) {
        if (menu >=
#ifdef _WIN32
            sizeof (MENUS) / sizeof (*MENUS)
#else
        arrsize (MENUS)
#endif
        )
            error ("not a valid menu index.");

        if (!*(MENUS + menu)) {
            warning ("cannot change the title color of a nonexistent menu.");

            return title_no_color;
        }

        return *(MENU_TITLE_COLORS + menu) = color;
    }

    bool loop_menu (const size_t menu, const bool loop) {
        if (menu >= sizeof (MENUS) / sizeof (*MENUS))
            error ("not a valid menu index.");

        return *(MENU_LOOP + menu) = loop;
    }

#if defined(__cpp_attributes) || __STDC_VERSION__ > 201710L
    [[nonnull (2)]]
#else
__attribute__ ((nonnull (2)))
#endif
    static void
        print_menu_title (const size_t menu, const char *const restrict title) {
        if (wattron (menu_win (*(MENUS + menu)), A_BOLD) == ERR)
            warning ("could not set menu title to bold.");

        if (*(MENU_TITLE_COLORS + menu) != title_no_color &&
            ((*(MENU_TITLE_COLORS + menu) == title_red &&
              wattron (menu_win (*(MENUS + menu)), COLOR_PAIR (log_error + 1)) == ERR) ||
             (*(MENU_TITLE_COLORS + menu) == title_green &&
              wattron (menu_win (*(MENUS + menu)), COLOR_PAIR (log_message + 1)) == ERR) ||
             (*(MENU_TITLE_COLORS + menu) == title_blue &&
              wattron (menu_win (*(MENUS + menu)), COLOR_PAIR (log_error + 2)) == ERR) ||
             (*(MENU_TITLE_COLORS + menu) == title_yellow &&
              wattron (menu_win (*(MENUS + menu)), COLOR_PAIR (log_warning + 1)) == ERR) ||
             (*(MENU_TITLE_COLORS + menu) == title_magenta &&
              wattron (menu_win (*(MENUS + menu)), COLOR_PAIR (log_error + 3)) == ERR) ||
             (*(MENU_TITLE_COLORS + menu) == title_cyan &&
              wattron (menu_win (*(MENUS + menu)), COLOR_PAIR (log_error + 4)) == ERR)))
            warning ("could not set the menu color.");

        if (mvwprintw (
                menu_win (*(MENUS + menu)), 2,
                (getmaxx (menu_win (*(MENUS + menu))) - getbegx (menu_win (*(MENUS + menu))) - (int) strlen (title)) /
                    2,
                "%s", title
            ) == ERR)
            warning ("could not print the title.");

        if (mvwhline (
                menu_win (*(MENUS + menu)), 3,
                (getmaxx (menu_win (*(MENUS + menu))) - getbegx (menu_win (*(MENUS + menu))) - (int) strlen (title)) /
                    2,
                0, (int) ({
                    const char *loc = setlocale (LC_ALL, NULL);
                    setlocale (LC_ALL, "es_ES.UTF-8");
                    mbtowc (NULL, NULL, 0);
                    size_t len = mbstowcs (NULL, title, strlen (title));
                    setlocale (LC_ALL, loc);
                    len;
                })
            ) == ERR)
            warning ("could not print the title's horizontal line.");

        if (wattroff (menu_win (*(MENUS + menu)), A_BOLD) == ERR)
            warning ("could not restore the font weight of the menu window.");

        if (*(MENU_TITLE_COLORS + menu) != title_no_color &&
            ((*(MENU_TITLE_COLORS + menu) == title_red &&
              wattroff (menu_win (*(MENUS + menu)), COLOR_PAIR (log_error + 1)) == ERR) ||
             (*(MENU_TITLE_COLORS + menu) == title_green &&
              wattroff (menu_win (*(MENUS + menu)), COLOR_PAIR (log_message + 1)) == ERR) ||
             (*(MENU_TITLE_COLORS + menu) == title_blue &&
              wattroff (menu_win (*(MENUS + menu)), COLOR_PAIR (log_error + 2)) == ERR) ||
             (*(MENU_TITLE_COLORS + menu) == title_yellow &&
              wattroff (menu_win (*(MENUS + menu)), COLOR_PAIR (log_warning + 1)) == ERR) ||
             (*(MENU_TITLE_COLORS + menu) == title_magenta &&
              wattroff (menu_win (*(MENUS + menu)), COLOR_PAIR (log_error + 3)) == ERR) ||
             (*(MENU_TITLE_COLORS + menu) == title_cyan &&
              wattroff (menu_win (*(MENUS + menu)), COLOR_PAIR (log_error + 4)) == ERR)))
            warning ("could not restore the menu color.");
    }

    static size_t impl_display_menu2 (
        const size_t menu, size_t current, const char *const restrict title, const char *const restrict mark
    ) {
        if (menu >= sizeof (MENUS) / sizeof (*MENUS))
            error ("not a valid menu index.");

        if (!*(MENUS + menu))
            error ("the menu has not been created.");

        if (set_menu_mark (*(MENUS + menu), mark) != E_OK)
            warning ("could not set the menu mark.");

        if (set_menu_format (
                *(MENUS + menu), getmaxx (menu_sub (*(MENUS + menu))) - getbegx (menu_sub (*(MENUS + menu))), 1
            ) != E_OK)
            warning ("could not set the menu format.");

        box (menu_win (*(MENUS + menu)), 0, 0);
        if (*title)
            print_menu_title (menu, title);

        mvwprintw (
            menu_win (*(MENUS + menu)), getmaxy (menu_win (*(MENUS + menu))) - getbegy (menu_win (*(MENUS + menu))) - 5,
            (int) strlen (mark) + 2, "%s", get_menu_exit_message ()
        );

        int prevcurs;
        if ((prevcurs = curs_set (0)) == ERR)
            warning ("could not hide the cursor.");

        wrefresh (menu_sub (*(MENUS + menu)));
        wrefresh (menu_win (*(MENUS + menu)));
        refresh ();

        if (get_menu_ret (menu)) {
            if (*get_menu_ret (menu) != (size_t) -1)
                for (size_t i = 0; ++i < *get_menu_ret (menu); menu_driver (*(MENUS + menu), REQ_DOWN_ITEM))
                    ;

            *get_menu_ret (menu) = (size_t) -1;
        }

        if (current && set_current_item (*(MENUS + menu), *(menu_items (*(MENUS + menu)) + current)) != E_OK)
            warning ("could not change the current item.");

        if (({
                int           err = post_menu (*(MENUS + menu));
                err != E_OK &&err != E_POSTED;
            })) {
            warning ("could not show the menu.");

            return menu;
        }

        for (int c = 0; (c = tolower (wgetch (menu_win (*(MENUS + menu))))) != get_menu_exit_key ();
             wrefresh (menu_sub (*(MENUS + menu))), wrefresh (menu_win (*(MENUS + menu))), refresh ())
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
                if (*(MENU_TYPES + menu) == actionmenu) {
                    if (get_menu_ret (menu))
                        *get_menu_ret (menu) = (size_t) item_index (current_item (*(MENUS + menu)));

                    goto action;
                }

                else if (*(MENU_TYPES + menu) == choicemenu) {
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

            cont:
                continue;
            }

        if (prevcurs && prevcurs != ERR)
            if (curs_set (prevcurs) == ERR)
                warning ("could not restore the cursor to its previous state.");

        {
            int err = unpost_menu (*(MENUS + menu));
            if (err != E_OK && err != E_NOT_POSTED)
                warning ("could not hide the menu.");
        }

        return menu;

    action:
        choicefunc_t *exec;
        if (!(exec = item_userptr (*(menu_items (*(MENUS + menu)) + item_index (current_item (*(MENUS + menu)))))))
            goto cont;

        size_t n = (size_t) item_count (*(MENUS + menu));

        void *p;
        if (!(p = malloc (((sizeof (char *) + sizeof (size_t)) * 2 + sizeof (choicefunc_t *)) * n)))
            error ("could not allocate space for the recreation of menu data.");

        char **const  c = p;
        size_t *const l = (void *) ((char *) p + sizeof (char *) * 2 * n);
        for (size_t i = 0; i < n; i++) {
            if (!(*(c + i * 2) = malloc (*(l + i * 2) = strlen (item_name (*(menu_items (*(MENUS + menu)) + i))))))
                error ("could not allocate space for the recreation of menu data.");

            if (!(*(c + i * 2 + 1) =
                      malloc (*(l + i * 2 + 1) = strlen (item_description (*(menu_items (*(MENUS + menu)) + i))))))
                error ("could not allocate space for the recreation of menu data.");

            memcpy (*(c + i * 2), item_name (*(menu_items (*(MENUS + menu)) + i)), *(l + i * 2));
            memcpy (*(c + i * 2 + 1), item_description (*(menu_items (*(MENUS + menu)) + i)), *(l + i * 2 + 1));
        }

        choicefunc_t **f = (void *) ((char *) p + (sizeof (char *) + sizeof (size_t)) * 2 * n);
        for (size_t i = 0; i < n; i++)
            *(f + i) = (choicefunc_t *) item_userptr (*(menu_items (*(MENUS + menu)) + i));

        int dims [4];
        getbegyx (menu_win (*(MENUS + menu)), *(dims + 3), *(dims + 2));
        getmaxyx (menu_win (*(MENUS + menu)), *(dims + 1), *dims);
        *dims       = *dims - (*(dims + 2) -= (int) get_hor_padding ()) + (int) get_hor_padding () * 2;
        *(dims + 1) = *(dims + 1) - (*(dims + 3) -= (int) get_ver_padding ()) + (int) get_ver_padding () * 2;

        current             = (size_t) item_index (current_item (*(MENUS + menu)));
        title_color_t color = get_menu_title_color (menu);

        if (!delete_menu (menu)) {
            warning ("could not delete menu, choice function not called.");
            free (p);

            goto cont;
        }

        exec ();

        if (*(MENUS + menu) && !delete_menu (menu))
            warning ("could not delete menu, will probably crash while creating the replacement menu.");

        // There's risk of overflowing the stack, but I don't think anybody will dare to make nearly-infinite
        // invocations of menus.
        impl_create_menu2 (
            menu, actionmenu, (uint32_t) *dims, (uint32_t) * (dims + 1), (uint32_t) * (dims + 2),
            (uint32_t) * (dims + 3), n, (const char *const (*const restrict) [2]) c,
            (const size_t (*const restrict) [2]) l, f
        );
        free (p);
        set_menu_title_color (menu, color);

        return *(MENU_LOOP + menu) ? impl_display_menu2 (menu, current, title, mark) : menu;
    }

#if defined(__cpp_attributes) || __STDC_VERSION__ > 201710L
    [[nonnull (2, 3)]]
#else
__attribute__ ((nonnull (2, 3)))
#endif
    size_t
        impl_display_menu (const size_t menu, const char *const restrict title, const char *const restrict mark) {
        return impl_display_menu2 (menu, 0, title, mark);
    }

    size_t *get_menu_ret (const size_t menu) {
        if (menu >=
#ifdef _WIN32
            sizeof (MENUS) / sizeof (*MENUS)
#else
        arrsize (MENUS)
#endif
        )
            error ("not a valid menu index.");

        if (!*(MENU_RETS + menu))
            return set_menu_ret (menu, NULL);

        return *(MENU_RETS + menu);
    }

    size_t *set_menu_ret (const size_t menu, size_t *const restrict ret) {
        if (menu >=
#ifdef _WIN32
            sizeof (MENUS) / sizeof (*MENUS)
#else
        arrsize (MENUS)
#endif
        )
            error ("not a valid menu index.");

        if (!*(MENUS + menu))
            return NULL;

        if (*(FREE_MENU_RETS + menu))
            free (*(MENU_RETS + menu));

        return *(MENU_RETS + menu) = ret ? (size_t *) ret : ({
            size_t *buf;
            if (!(*(FREE_MENU_RETS + menu) =
                      !!(buf = calloc (
                             *(MENU_TYPES + menu) == multimenu ? (size_t) item_count (*(MENUS + menu)) : 1,
                             sizeof (size_t)
                         ))))
                error ("could not create a menu return buffer.");
            buf;
        });
    }

    int get_menu_exit_key (void) {
        return MENU_EXIT_KEY;
    }

    const char *get_menu_exit_message (void) {
        return MENU_EXIT_MESSAGE;
    }

    int set_menu_exit_key (const int key, const char *const restrict message) {
        if (!message)
            memcpy (MENU_EXIT_MESSAGE, DEFAULT_MENU_EXIT_MESSAGE, sizeof (DEFAULT_MENU_EXIT_MESSAGE));

        else if (message != MENU_EXIT_MESSAGE) {
            size_t l;
            if ((l = strlen (message)) < sizeof (MENU_EXIT_MESSAGE))
                memcpy (MENU_EXIT_MESSAGE, message, l + 1);

            else
                warning ("the menu exit message is too big so the previous one will be used.");
        }

        return MENU_EXIT_KEY = key ? key : DEFAULT_MENU_EXIT_KEY;
    }

#ifdef __cplusplus
}
#endif
