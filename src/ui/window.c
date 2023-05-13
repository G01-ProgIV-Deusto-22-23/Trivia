#include "form.h"
#include "log_window.h"
#include "menu.h"
#include "window.h"

typedef struct __window_list_struct {
        void                        *win;
        delwinfunc_t                *delfunc;
        struct __window_list_struct *next;
} *window_list_t;

static window_list_t TRIVIA_WINDOW_LIST = NULL;
static bool          IS_DELETE_WINDOWS  = false;

static window_list_t create_window_list (void);
static bool          remove_window (window_list_t);
static delwinfunc_t  dellogfunc, delwinfunc, delmenufunc, delformfunc;

static window_list_t create_window_list (void) {
    if (TRIVIA_WINDOW_LIST)
        return TRIVIA_WINDOW_LIST;

    if (!(TRIVIA_WINDOW_LIST = calloc (1, sizeof (struct __window_list_struct))))
        error ("could not initialize the window list.");

    start_menu_gc ();
    start_form_gc ();

    return TRIVIA_WINDOW_LIST;
}

bool delete_windows (void) {
    if (!TRIVIA_WINDOW_LIST)
        return false;

    bool fine = IS_DELETE_WINDOWS = true;
    for (; TRIVIA_WINDOW_LIST; fine |= remove_window (TRIVIA_WINDOW_LIST))
        ;

    IS_DELETE_WINDOWS = false;

    stop_menu_gc ();
    stop_form_gc ();

    return fine;
}

void *add_window (void *const restrict win, const int wintype) {
    window_list_t head;
    for (head = create_window_list (); head->next; head = head->next)
        if (head->win == win)
            return win;

    if (!(head->next = calloc (1, sizeof (struct __window_list_struct))))
        error ("could not add a node to the window list");

    head->next->delfunc =
        *(((delwinfunc_t *const []) { (delwinfunc_t *const) dellogfunc, delwinfunc, delmenufunc, delformfunc }) +
          wintype);

    return head->next->win = win;
}

static bool remove_window (window_list_t node) {
    if (!node)
        return false;

    for (window_list_t *head = &TRIVIA_WINDOW_LIST, temp = NULL; *head;) {
        temp  = *head;
        *head = (*head)->next;

        if (temp == node) {
            int ret = OK;
            if (temp->delfunc && (ret = temp->delfunc (temp->win)) == ERR) {
                if (temp->delfunc == delwinfunc)
                    warning ("could not delete window.");

                else if (temp->delfunc == dellogfunc)
                    warning ("could not delete the log window");

                else if (temp->delfunc == delmenufunc)
                    warning ("could not delete menu.");

                else
                    warning ("could not delete form.");
            }

            free (temp);

            return ret != ERR;
        }
    }

    return false;
}

WINDOW *
    impl_create_window (const uint32_t w, const uint32_t h, const uint32_t x, const uint32_t y, const int notlogwin) {
    WINDOW *win;
    if (!(win = newwin (
              (int) (h <= get_ver_padding () * 2 ? h : h - get_ver_padding () * 2),
              (int) (w <= get_hor_padding () * 2 ? w : w - get_hor_padding () * 2), (int) (y + get_ver_padding ()),
              (int) (x + get_hor_padding ())
          )))
        error ("could not create a window.");

    return add_window (win, notlogwin);
}

bool delete_window (WINDOW *const restrict win) {
    if (!(TRIVIA_WINDOW_LIST && win))
        return false;

    for (window_list_t head = TRIVIA_WINDOW_LIST->next; head; head = head->next)
        if (head->win == win && (head->delfunc == delwinfunc || head->delfunc == dellogfunc))
            return remove_window (head);

    return false;
}

bool delete_menu (const size_t menu) {
    if (!TRIVIA_WINDOW_LIST)
        return false;

    for (window_list_t head = TRIVIA_WINDOW_LIST->next; head; head = head->next)
        if (head->win == (void *) (uintptr_t) menu && (head->delfunc == delmenufunc))
            return remove_window (head);

    return false;
}

bool delete_form (const size_t form) {
    if (!TRIVIA_WINDOW_LIST)
        return false;

    for (window_list_t head = TRIVIA_WINDOW_LIST->next; head; head = head->next)
        if (head->win == (void *) (uintptr_t) form && (head->delfunc == delformfunc))
            return remove_window (head);

    return false;
}

static int dellogfunc (void *const restrict win) {
    clear_log_window ();
    int del        = delwin (win);
    LOG_WINDOW_VAR = NULL;

    return del;
}

static int delwinfunc (void *const restrict win) {
    return delwin (win);
}

static int delmenufunc (void *const restrict menu) {
    trivia_free_menu ((size_t) (uintptr_t) menu);

    if (!IS_DELETE_WINDOWS) {
        nanosleep (&(struct timespec) { 0, 5000000L }, &(struct timespec) {});

        if (
#ifdef _WIN32
            WaitForSingleObject (*(FREE_MENU_SEMS + (size_t) (uintptr_t) menu), INFINITE) == WAIT_FAILED
#else
            sem_wait (FREE_MENU_SEMS + (uintptr_t) menu) == -1
#endif
        )
            error ("could not wait for the menu to be freed.");
    }

    return atomic_load (FREE_MENU_ERR + (uintptr_t) menu);
}

static int delformfunc (void *const restrict form) {
    trivia_free_form ((size_t) (uintptr_t) form);

    if (!IS_DELETE_WINDOWS) {
        if (
#ifdef _WIN32
            WaitForSingleObject (*(FREE_FORM_SEMS + (size_t) (uintptr_t) form), INFINITE) == WAIT_FAILED
#else
            sem_wait (FREE_FORM_SEMS + (uintptr_t) form) == -1
#endif
        )
            error ("could not wait for the form to be freed.");
    }

    return atomic_load (&FREE_FORM_ERR);
}