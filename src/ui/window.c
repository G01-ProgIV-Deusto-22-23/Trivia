#include "menu.h"

typedef struct __window_list_struct {
        void                        *win;
        delwinfunc_t                *delfunc;
        struct __window_list_struct *next;
} *restrict window_list_t;

static window_list_t TRIVIA_WINDOW_LIST = NULL;

static void         create_window_list (void);
static bool         remove_window (window_list_t);
static delwinfunc_t delwinfunc, delmenufunc;

static void create_window_list (void) {
    if (!(TRIVIA_WINDOW_LIST = calloc (1, sizeof (struct __window_list_struct))))
        error ("could not initialize the window list.");

    start_menu_gc ();
}

bool delete_windows (void) {
    if (!TRIVIA_WINDOW_LIST)
        return false;

    bool fine = true;
    for (; TRIVIA_WINDOW_LIST; remove_window (TRIVIA_WINDOW_LIST));

    return fine;
}

void *add_window (void *const restrict win, const int ismenu) {
    if (!TRIVIA_WINDOW_LIST)
        create_window_list ();

    window_list_t head;
    for (head = TRIVIA_WINDOW_LIST; head->next; head = head->next)
        if (head->win == win)
            return win;

    if (!(head->next = calloc (1, sizeof (struct __window_list_struct))))
        error ("could not add a node to the window list");

    head->delfunc =
        *(((delwinfunc_t *const []) { (delwinfunc_t *const) (void *) delete_log_window, delwinfunc, delmenufunc }) +
          (win == get_log_window () ? 0 : (ismenu + 1)));
    return head->next->win = win;
}

static bool remove_window (window_list_t node) {
    if (!node)
        return false;

    for (window_list_t *head = &TRIVIA_WINDOW_LIST, temp = NULL; *head;) {
        temp = *head;
        *head = (*head)->next;

        if (temp == node) {
            int ret;
            if (!temp->delfunc || (ret = temp->delfunc (temp->win)) == ERR) {
                if (temp->delfunc == delwinfunc)
                    warning ("could not delete window.");

                else if (temp->delfunc == (delwinfunc_t *) (void *) delete_log_window)
                    warning ("could not delete the log window");

                else
                    warning ("could not delete menu.");
            }


            free (temp);

            return ret != ERR;
        }
    }

    return false;
}

WINDOW *impl_create_window (const uint32_t w, const uint32_t h, const uint32_t x, const uint32_t y, const int ismenu) {
    WINDOW *win;
    if (!(win = newwin (
              (int) (h <= get_ver_padding () * 2 ? h : h - get_ver_padding () * 2),
              (int) (w <= get_hor_padding () * 2 ? w : w - get_hor_padding () * 2), (int) (y + get_ver_padding ()),
              (int) (x + get_hor_padding ())
          )))
        error ("could not create a window.");

    return add_window (win, win == get_log_window () ? 0 : (ismenu + 1));
}

bool delete_window (WINDOW *const restrict win) {
    if (!win)
        return false;

    for (window_list_t head = TRIVIA_WINDOW_LIST; head->next; head = head->next)
        if (head->win == win && (head->delfunc == delwinfunc || head->delfunc == (void *) delete_log_window))
            return remove_window (head);

    return false;
}

bool delete_menu (const size_t menu) {
    for (window_list_t head = TRIVIA_WINDOW_LIST; head->next; head = head->next)
        if (head->win == (void *) (uintptr_t) menu && (head->delfunc == delmenufunc))
            return remove_window (head);

    return false;
}

static int delwinfunc (void *const restrict win) {
    return delwin (win);
}

static int delmenufunc (void *const restrict menu) {
    trivia_free_menu ((size_t) (uintptr_t) menu);

#ifdef _WIN32
    WaitForSingleObject (*(FREE_MENU_SEMS + (uintptr_t) menu), 0L);
#else
    sem_wait (FREE_MENU_SEMS + (uintptr_t) menu);
#endif

    return FREE_MENU_ERR;
}