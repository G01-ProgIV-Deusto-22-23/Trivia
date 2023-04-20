#include "form.h"
#include "ncursesw/curses.h"
#include "ncursesw/eti.h"
#include "ncursesw/form.h"
#include "window.h"

form_t                *FORMS [MAX_FORM_FIELDS] = { 0 };
volatile atomic_size_t FORM_CONTROL            = 0;
volatile atomic_int    FREE_FORM_ERR           = OK;

#ifdef _WIN32
HANDLE FREE_FORM_SEMS [sizeof (FORMS) / sizeof (*FORMS)] = { 0 };
#else
sem_t            FREE_FORM_SEMS [sizeof (FORMS) / sizeof (*FORMS)];
#endif

static char        FORM_FIELD_DATA [sizeof (FORMS) / sizeof (*FORMS)][MAX_FORM_FIELDS][MAX_FORM_FIELD_LEN + 1];
static const char *FORM_FIELD_SAVEBUF [sizeof (FORMS) / sizeof (*FORMS)][MAX_FORM_FIELDS];
static char        FORM_FIELD_TEMPBUF [sizeof (FORMS) / sizeof (*(FORMS))][MAX_FORM_FIELD_LEN + 1];

#ifdef _WIN32
static HANDLE FREE_FORM_THREADS [sizeof (FORMS) / sizeof (*FORMS)];
#else
static pthread_t FREE_FORM_THREADS [sizeof (FORMS) / sizeof (*FORMS)];
#endif

static int  FORM_EXIT_KEY                   = DEFAULT_FORM_EXIT_KEY;
static char FORM_EXIT_MESSAGE [BUFSIZ >> 2] = DEFAULT_FORM_EXIT_MESSAGE;

static int  FORM_ERASE_KEY                   = DEFAULT_FORM_ERASE_KEY;
static char FORM_ERASE_MESSAGE [BUFSIZ >> 2] = DEFAULT_FORM_ERASE_MESSAGE;

static int  FORM_SAVE_KEY                   = DEFAULT_FORM_SAVE_KEY;
static char FORM_SAVE_MESSAGE [BUFSIZ >> 2] = DEFAULT_FORM_SAVE_MESSAGE;

#ifdef _WIN32
static unsigned long impl_trivia_free_form (void *__form__) {
#else
static void     *impl_trivia_free_form (void *__form__) {
#endif
    size_t form = (size_t) (uintptr_t) __form__;
    atomic_store (&FREE_FORM_ERR, OK);

    if (!*(FORMS + form))
        return
#ifdef _WIN32
            (unsigned long) (uintptr_t)
#endif
            * (FORMS + form);

    {
        int err = unpost_form ((*(FORMS + form))->form);
        if (err != E_OK && err != E_NOT_POSTED)
            warning ("could not hide the form.");
    }

    FIELD **const fields = form_fields ((*(FORMS + form))->form);

    if (free_form ((*(FORMS + form))->form) != E_OK) {
        atomic_store (&FREE_FORM_ERR, ERR);

        warning ("could not free the form.");
    }

    for (short i = 0; i < field_count ((*(FORMS + form))->form);) {
        if (free_field (*(fields + i++)) != E_OK) {
            atomic_store (&FREE_FORM_ERR, ERR);

            warning ("could not free form field.");
        }
    }

    free (*(FORMS + form));

#ifdef _WIN32
    SecureZeroMemory
#else
    explicit_bzero
#endif
        (*(FORM_FIELD_DATA + form), sizeof (FORM_FIELD_DATA) * sizeof (*FORMS) / sizeof (FORMS));

    refresh ();

    FORM_CONTROL ^= ((size_t) 1) << form;

#ifdef _WIN32
    ReleaseSemaphore (*(FREE_FORM_SEMS + form), 1, NULL);
#else
    sem_post (FREE_FORM_SEMS + form);
#endif

    return
#ifdef _WIN32
        (unsigned long) (uintptr_t)
#endif
            (*(FORMS + form) = NULL);
}

void trivia_free_form (const size_t form) {
    if (form >= sizeof (FORMS) / sizeof (*FORMS))
        error ("not a valid forms index.");

#ifdef _WIN32
    if (!(*(FREE_FORM_THREADS + form) = CreateThread (
              &(SECURITY_ATTRIBUTES
              ) { .nLength = sizeof (SECURITY_ATTRIBUTES), .lpSecurityDescriptor = NULL, .bInheritHandle = TRUE },
              0, impl_trivia_free_form, (void *) (uintptr_t) form, 0, NULL
          )))
#else
    if (pthread_create (FREE_FORM_THREADS + form, NULL, impl_trivia_free_form, (void *) (uintptr_t) form))
#endif
        impl_trivia_free_form ((void *) (uintptr_t) form);
}

void start_form_gc (void) {
    for (size_t i = 0; i <
#ifdef _WIN32
                       sizeof (FREE_FORM_SEMS) / sizeof (*FREE_FORM_SEMS)
#else
                       arrsize (FREE_FORM_SEMS)
#endif
             ;)
        if (
#ifdef _WIN32
            !(*(FREE_FORM_SEMS + i++) = CreateSemaphore (NULL, 0, 1, NULL))
#else
            sem_init (FREE_FORM_SEMS + i++, 0, 1)
#endif
        )
            error ("could not start the form garbage collector.");
}

void stop_form_gc (void) {
    for (size_t i = 0; i <
#ifdef _WIN32
                       sizeof (FREE_FORM_SEMS) / sizeof (*FREE_FORM_SEMS)
#else
                       arrsize (FREE_FORM_SEMS)
#endif
             ;) {
#ifdef _WIN32
        WaitForSingleObject (*(FREE_FORM_SEMS + i), 0L);
#else
        sem_wait (FREE_FORM_SEMS + i);
#endif

        if (
#ifdef _WIN32
            !CloseHandle (*(FREE_FORM_SEMS + i++))
#else
            sem_destroy (FREE_FORM_SEMS + i++)
#endif
        )
            warning ("could not destroy a semaphore.");
    }
}

field_attr_t impl_field_attrs (const size_t len, const FIELDTYPE *const type) {
    return (field_attr_t
    ) { .len =
            (len ? (len > MAX_FORM_FIELD_LEN
                        ? (error ("the field cannot be longer than " STRINGIFY (MAX_FORM_FIELD_LEN)), (size_t) 0)
                        : len)
                 : (error ("the field cannot have a length of zero."), (size_t) 0)),
        .type = (FIELDTYPE *) type };
}

#if defined(__cpp_attributes) || __STDC_VERSION__ > 201710L
[[nodiscard]]
#else
__attribute__ ((warn_unused_result))
#endif
size_t
    impl_create_form (
        uint32_t w, uint32_t h, uint32_t x, uint32_t y, const size_t n, const field_attr_t *const attrs,
        const char *const *const restrict names
    ) {
    if (atomic_load (&FORM_CONTROL) == (size_t) -1)
        error ("no more forms can be created.");

    if (!n)
        error ("forms cannot have no fields.");

    if (n > sizeof (FORMS) / sizeof (*FORMS))
        error ("maximum of form fields for a single form exceeded.");

    if (!attrs)
        error ("The attrs pointer cannot be null.");

    size_t form = 0;
    for (; form < sizeof (FORMS) / sizeof (*FORMS);)
        if (!((atomic_load (&FORM_CONTROL) >> form++) & 1))
            break;

    atomic_fetch_or (&FORM_CONTROL, 1 << form);

#ifdef _WIN32
    WaitForSingleObject (*(FREE_FORM_SEMS + form), 0L);
#else
    sem_wait (FREE_FORM_SEMS + form);
#endif

    FIELD **fields;
    if (!(fields = malloc (sizeof (FIELD *) * (n + 1))))
        error ("could not allocate space for the form fields.");
    *(fields + n) = NULL;

    size_t  tl    = 0;
    size_t *strls = alloca ((n + 1) * sizeof (size_t));
    memset (strls, 0, n * sizeof (size_t));
    if (names) {
        const char *loc = setlocale (LC_ALL, NULL);
        setlocale (LC_ALL, "es_ES.UTF-8");

        for (size_t i = 0; i < n; *strls = max (*strls, mbstowcs (NULL, *(names + i), *(strls + i + 1))), i++)
            if (*(names + i))
                tl += *(strls + i + 1) = strlen (*(names + i));

        setlocale (LC_ALL, loc);
    }

    for (size_t i = 0; i < n; field_opts_off (*(fields + i++), O_AUTOSKIP | O_BLANK)) {
        if (!((attrs + i)->type == TYPE_ALNUM || (attrs + i)->type == TYPE_ALPHA || (attrs + i)->type == TYPE_INTEGER ||
              (attrs + i)->type == TYPE_IPV4))
            error ("only alphabetical, alphanumeric, integer and IPv4 fields are supported.");

        *(fields + i) = new_field (1, (int) (attrs + i)->len, 5 + (int) i, (int) *strls, 0, 0);

        set_field_back (*(fields + i), A_UNDERLINE);

        if ((attrs + i)->type == TYPE_ALNUM && (attrs + i)->type_args.alnum.passwd)
            field_opts_off (*(fields + i), O_PUBLIC);
    }

    if (!(*(FORMS + form) = malloc (
              sizeof (struct __form_struct) + (sizeof (size_t) + sizeof (field_attr_t) + 1) * n + sizeof (size_t) + tl
          )))
        error ("could not allocate space for the form.");

    if (!((*(FORMS + form))->form = new_form (fields)))
        error ("could not create a new form.");
    (*(FORMS + form))->nfields  = n;
    (*(FORMS + form))->saveexit = false;

    char *pos =
        mempcpy (mempcpy ((*(FORMS + form))->data, strls, (n + 1) * sizeof (size_t)), attrs, sizeof (field_attr_t) * n);

    if (names)
        for (size_t i = 0; i < n; i++)
            pos = mempcpy (pos, *(names + i), *(strls + i + 1) + 1);

    fitwin (w, h, x, y);
    {
        WINDOW *win = newwin (
            *(*(FORMS + form))->dims = (int) h, *((*(FORMS + form))->dims + 1) = (int) w,
            *((*(FORMS + form))->dims + 2) = (int) y, *((*(FORMS + form))->dims + 3) = (int) x
        );
        if (set_form_win ((*(FORMS + form))->form, win) != E_OK)
            error ("could not set the form window.");

        {
            uint32_t _w = (uint32_t) (getmaxx (win) - getbegx (win) - 3);
            uint32_t _h = (uint32_t) (getmaxy (win) - getbegy (win)) / 2;
            if (_h > 5)
                _h -= 5;
            uint32_t _x = 3;
            uint32_t _y = (uint32_t) _h;
            fitwin (_w, _h, _x, _y);

            if (set_form_sub ((*(FORMS + form))->form, derwin (win, (int) _h, (int) _w, (int) _y, (int) _x)) != E_OK)
                error ("could not set the form subwindow.");
        }
    }

#ifdef _WIN32
    ReleaseSemaphore (*(FREE_FORM_SEMS + form), 1L, NULL);
#else
    sem_post (FREE_FORM_SEMS + form);
#endif

    add_window ((void *) (uintptr_t) form, 3);

    keypad (form_win ((*(FORMS + form))->form), true);

    return form;
}

#if defined(__cpp_attributes) || __STDC_VERSION__ > 201710L
[[nonnull (2)]]
#else
__attribute__ ((nonnull (2)))
#endif
static void
    print_form_title (const size_t form, const char *const restrict title) {
    WINDOW *w = form_win ((*(FORMS + form))->form);
    if (mvwprintw (w, 2, (getmaxx (w) - getbegx (w) - (int) strlen (title)) / 2, "%s", title) == ERR)
        warning ("could not print the title.");

    if (mvwhline (w, 3, (getmaxx (w) - getbegx (w) - (int) strlen (title)) / 2, 0, (int) ({
                      const char *loc = setlocale (LC_ALL, NULL);
                      setlocale (LC_ALL, "es_ES.UTF-8");
                      mbtowc (NULL, NULL, 0);
                      size_t len = mbstowcs (NULL, title, strlen (title));
                      setlocale (LC_ALL, loc);
                      len;
                  })) == ERR)
        warning ("could not print the title's horizontal line.");
}

#if defined(__cpp_attributes) || __STDC_VERSION__ > 201710L
[[nonnull (2)]]
#else
__attribute__ ((nonnull (2)))
#endif
size_t
    impl_display_form (const size_t form, const char *const restrict title) {
    if (form >= sizeof (FORMS) / sizeof (*FORMS))
        error ("not a valid form index.");

    if (!*(FORMS + form))
        error ("the form has not been created.");

    if (!form_win ((*(FORMS + form))->form)) {
        if (set_form_win (
                (*(FORMS + form))->form, newwin (
                                             *(*(FORMS + form))->dims, *((*(FORMS + form))->dims + 1),
                                             *((*(FORMS + form))->dims + 2), *((*(FORMS + form))->dims + 3)
                                         )
            ) != E_OK)
            error ("could not set the form window.");
    }

    if (!form_sub ((*(FORMS + form))->form)) {
        uint32_t _w = (uint32_t
        ) (getmaxx (form_win ((*(FORMS + form))->form)) - getbegx (form_win ((*(FORMS + form))->form)) - 3);
        uint32_t _h =
            (uint32_t) (getmaxy (form_win ((*(FORMS + form))->form)) - getbegy (form_win ((*(FORMS + form))->form))) /
            2;
        if (_h > 5)
            _h -= 5;
        uint32_t _x = 3;
        uint32_t _y = _h;
        fitwin (_w, _h, _x, _y);

        if (set_form_sub (
                (*(FORMS + form))->form,
                derwin (form_win ((*(FORMS + form))->form), (int) _h, (int) _w, (int) _y, (int) _x)
            ) != E_OK)
            error ("could not set the form subwindow.");
    }

    for (size_t i = 0; i < (*(FORMS + form))->nfields;)
        if (set_field_type (*(form_fields ((*(FORMS + form))->form) + i++), NULL) != E_OK)
            warning ("could not set field type.");

    if (({
            int           err = post_form ((*(FORMS + form))->form);
            err != E_OK &&err != E_POSTED;
        })) {
        warning ("could not show the form.");

        return form;
    }

    if (*title)
        print_form_title (form, title);

    else {
        if (wmove (form_win ((*(FORMS + form))->form), 2, 0) == ERR ||
            wclrtoeol (form_win ((*(FORMS + form))->form)) == ERR)
            warning ("could not clear line 2.");

        if (wmove (form_win ((*(FORMS + form))->form), 3, 0) == ERR ||
            wclrtoeol (form_win ((*(FORMS + form))->form)) == ERR)
            warning ("could not clear line 3.");
    }

    box (form_win ((*(FORMS + form))->form), 0, 0);

    {
        int   r;
        char *pos = (*(FORMS + form))->data + (sizeof (size_t) + sizeof (field_attr_t)) * (*(FORMS + form))->nfields +
                    sizeof (size_t);
        for (size_t i = 0; i < (*(FORMS + form))->nfields;
             pos      += *((size_t *) (void *) (*(FORMS + form))->data + i++ + 1) + 1)
            mvwprintw (
                form_win ((*(FORMS + form))->form),
                (field_info (
                     *(form_fields ((*(FORMS + form))->form) + i), &(int) { 0 }, &(int) { 0 }, &r, &(int) { 0 },
                     &(int) { 0 }, &(int) { 0 }
                 ),
                 r) +
                    getpary (form_sub ((*(FORMS + form))->form)),
                3, "%s", pos
            );
    }

    mvwprintw (
        form_win ((*(FORMS + form))->form),
        getmaxy (form_win ((*(FORMS + form))->form)) - getbegy (form_win ((*(FORMS + form))->form)) - 7, 3, "%s",
        get_form_save_message ()
    );

    mvwprintw (
        form_win ((*(FORMS + form))->form),
        getmaxy (form_win ((*(FORMS + form))->form)) - getbegy (form_win ((*(FORMS + form))->form)) - 6, 3, "%s",
        get_form_erase_message ()
    );

    mvwprintw (
        form_win ((*(FORMS + form))->form),
        getmaxy (form_win ((*(FORMS + form))->form)) - getbegy (form_win ((*(FORMS + form))->form)) - 5, 3, "%s",
        get_form_exit_message ()
    );

    if (set_current_field ((*(FORMS + form))->form, *form_fields ((*(FORMS + form))->form)) != E_OK)
        warning ("could not set the current field to the first field.");

    else {
        if (set_field_back (*form_fields ((*(FORMS + form))->form), A_STANDOUT) != E_OK)
            warning ("could not set the field background.");

        if (set_field_fore (*form_fields ((*(FORMS + form))->form), A_REVERSE) != E_OK)
            warning ("could not set the field foreground.");
    }

    wrefresh (form_win ((*(FORMS + form))->form));
    wrefresh (form_sub ((*(FORMS + form))->form));

    bool insert = false;
    bool del    = false;
    for (int c = 0, i = field_index (current_field ((*(FORMS + form))->form)),
             pos = (int) ({
                 char *buf = field_buffer (*form_fields ((*(FORMS + form))->form), 0);
                 buf       += ({
                     size_t j = 0;
                     for (; isspace (*(buf + j)); j++)
                         ;
                     j;
                 });
                 size_t j  = strlen (buf);
                 for (; j && isspace (*(buf + j - 1)); j--)
                     ;
                 j;
             });
         (c = tolower (wgetch (form_win ((*(FORMS + form))->form)))) != get_form_exit_key ();
         del = false, wrefresh (form_sub ((*(FORMS + form))->form)) == ERR ||
                              wrefresh (form_win ((*(FORMS + form))->form)) == ERR
                          ? warning ("could not update form display.")
                          : (void) 0)
        if (c == KEY_UP) {
            i = field_index (current_field ((*(FORMS + form))->form));

            if (set_field_back (current_field ((*(FORMS + form))->form), A_UNDERLINE) != E_OK)
                warning ("could not set the field background.");

            if (set_field_fore (current_field ((*(FORMS + form))->form), A_NORMAL) != E_OK)
                warning ("could not set the field foreground.");

            set_current_field (
                (*(FORMS + form))->form,
                *(form_fields ((*(FORMS + form))->form) + (i = i ? i - 1 : (int) (*(FORMS + form))->nfields - 1))
            );

            if (form_driver ((*(FORMS + form))->form, REQ_BEG_LINE) == E_OK)
                pos = 0;

            else
                warning ("could not move the cursor to the beginning of the field.");

            if (set_field_back (*(form_fields ((*(FORMS + form))->form) + i), A_STANDOUT) != E_OK)
                warning ("could not set the field background.");

            if (set_field_fore (*(form_fields ((*(FORMS + form))->form) + i), A_REVERSE) != E_OK)
                warning ("could not set the field foreground.");
        }

        else if (c == KEY_DOWN) {
            i = field_index (current_field ((*(FORMS + form))->form));

            if (set_field_back (current_field ((*(FORMS + form))->form), A_UNDERLINE) != E_OK)
                warning ("could not set the field background.");

            if (set_field_fore (current_field ((*(FORMS + form))->form), A_NORMAL) != E_OK)
                warning ("could not set the field foreground");

            set_current_field (
                (*(FORMS + form))->form,
                *(form_fields ((*(FORMS + form))->form) + (i = i == (int) (*(FORMS + form))->nfields - 1 ? 0 : i + 1))
            );

            if (form_driver ((*(FORMS + form))->form, REQ_BEG_LINE) == E_OK)
                pos = 0;

            else
                warning ("could not move the cursor to the beginning of the field.");

            if (set_field_back (*(form_fields ((*(FORMS + form))->form) + i), A_STANDOUT) != E_OK)
                warning ("could not set the field background.");

            if (set_field_fore (*(form_fields ((*(FORMS + form))->form) + i), A_REVERSE))
                warning ("could not set the field foreground.");
        }

        else if (c == KEY_LEFT) {
        move_left:
            bool jmp = false;
            if (({
                    int err = form_driver ((*(FORMS + form))->form, REQ_LEFT_CHAR);
                    pos     -= (jmp = (pos && (err == E_OK || err == E_REQUEST_DENIED)));
                    !(err == E_OK || err == E_REQUEST_DENIED);
                }))
                warning ("could not move left.");

            if (jmp && del)
                goto del_char;
        }

        else if (c == KEY_RIGHT) {
            if (({
                    int err = form_driver ((*(FORMS + form))->form, REQ_RIGHT_CHAR);
                    pos     += pos < ({
                               int fl;
                               field_info (
                                   current_field ((*(FORMS + form))->form), &(int) { 0 }, &fl, &(int) { 0 },
                                   &(int) { 0 }, &(int) { 0 }, &(int) { 0 }
                               );
                               fl - 1;
                           }) &&
                           (err == E_OK || err == E_REQUEST_DENIED);
                    !(err == E_OK || err == E_REQUEST_DENIED);
                }))
                warning ("could not move right.");
        }

        else if (c == get_form_save_key ()) {
            for (size_t j = 0; j < (*(FORMS + form))->nfields; j++)
                *(*(FORM_FIELD_SAVEBUF + form) + j) = field_buffer (*(form_fields ((*(FORMS + form))->form) + j), 0);

            set_form_data (form, *(FORM_FIELD_SAVEBUF + form));
        }

        else if (c == get_form_erase_key ()) {
            for (size_t j = (size_t) (pos = 0); j < (*(FORMS + form))->nfields;)
                if (set_field_buffer (*(form_fields ((*(FORMS + form))->form) + j++), 0, "") != E_OK)
                    warning ("could not erase field.");
        }

        else if ((del = (c == KEY_BACKSPACE || c == '\b' || c == 127)))
            goto move_left;

        else if (c == KEY_DC) {
        del_char:
            if (({
                    int err = form_driver ((*(FORMS + form))->form, REQ_DEL_CHAR);
                    if (err == E_OK) {
                        FIELD *f;
                        char  *buf = field_buffer (f = current_field ((*(FORMS + form))->form), 0);
                        buf        += ({
                            size_t j = 0;
                            for (; isspace (*(buf + j)); j++)
                                ;
                            j;
                        });
                        *(char *) mempcpy (*(FORM_FIELD_TEMPBUF + form), buf, ({
                            size_t j = strlen (buf);
                            for (; j && isspace (*(buf + j - 1)); j--)
                                ;
                            j;
                        }))        = '\0';
                        memmove (
                            *(FORM_FIELD_TEMPBUF + form) + pos, *(FORM_FIELD_TEMPBUF + form) + pos + 1,
                            strlen (*(FORM_FIELD_TEMPBUF + form) + pos + 1) + 1
                        );
                        set_field_buffer (f, 0, *(FORM_FIELD_TEMPBUF + form));
                        pos -= !del;
                    }
                    !(err == E_OK || err == E_REQUEST_DENIED);
                }))
                warning ("could not delete character.");
        }

        else if (c == KEY_IC) {
            if (form_driver ((*(FORMS + form))->form, insert ? REQ_INS_MODE : REQ_OVL_MODE))
                warning ("could not toggle insert mode");
        }

        else if (({
                     int err = form_driver ((*(FORMS + form))->form, c);
                     if (err == E_OK) {
                         FIELD *f;
                         char  *buf = field_buffer (f = current_field ((*(FORMS + form))->form), 0);
                         for (; isspace (*(buf)); buf++)
                             ;
                         *(char *) mempcpy (*(FORM_FIELD_TEMPBUF + form), buf, ({
                             size_t j = strlen (buf);
                             for (; j && isspace (*(buf + j - 1)); j--)
                                 ;
                             j;
                         }))  = '\0';
                         *((char *) memmove (
                               *(FORM_FIELD_TEMPBUF + form) + pos + 1, *(FORM_FIELD_TEMPBUF + form) + pos,
                               strlen (*(FORM_FIELD_TEMPBUF + form) + pos) + 1
                           ) -
                           1) = (char) c;
                         set_field_buffer (f, 0, *(FORM_FIELD_TEMPBUF + form));
                         pos++;
                     }
                     !(err == E_OK || err == E_REQUEST_DENIED);
                 }))
            warning ("could not update the field with the entered character.");

    {
        size_t        invalid = 0;
        field_attr_t *attrs =
            (field_attr_t *) (void *) ((*(FORMS + form))->data + sizeof (size_t) * ((*(FORMS + form))->nfields + 1));
        form_driver ((*(FORMS + form))->form, REQ_FIRST_FIELD);
        for (size_t i                                = 0;
             i < (*(FORMS + form))->nfields; invalid |= (
#if __WORDSIZE == 64
                                                            UINT64_C (1)
#else
                                                            UINT32_C (1)
#endif
                                                            << (i++)
                                                        ) *
                                                        (form_driver ((*(FORMS + form))->form, REQ_VALIDATION) != E_OK),
                    form_driver ((*(FORMS + form))->form, REQ_NEXT_FIELD)) {
            if (((attrs + i)->type == TYPE_ALNUM || (attrs + i)->type == TYPE_ALPHA) &&
                set_field_type (
                    *(form_fields ((*(FORMS + form))->form) + i), (attrs + i)->type, (attrs + i)->type_args.alnum.min
                ) != E_OK)
                warning ("could not set field type.");

            else if ((attrs + i)->type == TYPE_INTEGER && 
                set_field_type (
                    *(form_fields ((*(FORMS + form))->form) + i), TYPE_INTEGER, 0, (attrs + i)->type_args.integer.min,
                    (attrs + i)->type_args.integer.max
                ) != E_OK)
                warning ("could not set field type.");

            else if (set_field_type (*(form_fields ((*(FORMS + form))->form) + i), TYPE_IPV4) != E_OK)
                warning ("could not set field type.");

            set_field_back (*(form_fields ((*(FORMS + form))->form) + i), A_UNDERLINE);
            set_field_fore (*(form_fields ((*(FORMS + form))->form) + i), A_NORMAL);

            if (set_field_buffer (*(form_fields ((*(FORMS + form))->form) + i), 0, *(*(FORM_FIELD_DATA + form) + i)))
                warning ("could not set contents of field buffer.");
        }

        {
            int err = unpost_form ((*(FORMS + form))->form);
            if (err != E_OK && err != E_NOT_POSTED)
                warning ("could not hide the form.");
        }

        if (invalid) {
            WINDOW *win;
            if (!(win = newwin (
                      *(*(FORMS + form))->dims, *((*(FORMS + form))->dims + 1), *((*(FORMS + form))->dims + 2),
                      *((*(FORMS + form))->dims + 3)
                  )))
                error ("could not create the menu deletion window.");

            box (win, 0, 0);

            int r = 2;
            for (size_t i = 0; i < (*(FORMS + form))->nfields;
                 set_field_type (*(form_fields ((*(FORMS + form))->form) + i++), NULL))
                if (invalid & (UINT64_C (1) << i)) {
                    if ((attrs + i)->type == TYPE_ALNUM || (attrs + i)->type == TYPE_ALPHA)
                        mvwprintw (
                            win, r, 2, "Debe introducirse un mínimo de %d carácteres en el campo %zu.",
                            (attrs + i)->type_args.alnum.min, i + 1
                        );

                    else if ((attrs + i)->type == TYPE_INTEGER)
                        mvwprintw (
                            win, r, 2, "Debe introducirse un número entero entre %ld y %ld en el campo %zu.",
                            (attrs + i)->type_args.integer.min, (attrs + i)->type_args.integer.max, i + 1
                        );

                    else
                        mvwprintw (win, r, 2, "IP no válida en el campo %zu.", i + 1);

                    r++;
                }

            for (int c = 0; !((c = wgetch (win)) == KEY_ENTER || c == '\r' || c == '\n');)
                ;

            if (delwin (win) == ERR)
                warning ("could not delete the invalid fields window.");

            return impl_display_form (form, title);
        }
    }

    {
        int err = unpost_form ((*(FORMS + form))->form);
        if (err != E_OK && err != E_NOT_POSTED)
            warning ("could not hide the form.");
    }

    if ((*(FORMS + form))->saveexit) {
        for (size_t i = 0; i < (*(FORMS + form))->nfields; i++)
            *(*(FORM_FIELD_SAVEBUF + form) + i) = field_buffer (*(form_fields ((*(FORMS + form))->form) + i), 0);

        set_form_data (form, *(FORM_FIELD_SAVEBUF + form));
    }

    return form;
}

const char (*get_form_data (const size_t form)) [MAX_FORM_FIELD_LEN + 1] {
    if (form >= sizeof (FORMS) / sizeof (*FORMS))
        error ("not a valid form index.");

    if (!*(FORMS + form))
        return (warning ("the requested data belongs to a form that does not exist."), NULL);

    char *buf;
    for (size_t i                                                       = 0, l; i < (*(FORMS + form))->nfields;
         *(char *) mempcpy (*(*(FORM_FIELD_DATA + form) + i++), buf, l) = '\0') {
        buf = field_buffer (*(form_fields ((*(FORMS + form))->form) + i), 0);

        for (; isspace (*buf); buf++)
            ;

        l = strlen (buf);
        for (; l && isspace (*(buf + l - 1)); l--)
            ;
    }

    return *(FORM_FIELD_DATA + form);
}

void set_form_data (const size_t form, const char *const *const restrict data) {
    if (form >= sizeof (FORMS) / sizeof (*FORMS))
        error ("not a valid form index.");

    if (!*(FORMS + form))
        return warning ("cannot assign data to a form that does not exist.");

    if (!data) {
        for (size_t i = 0; i < (*(FORMS + form))->nfields; i++)
            set_field_buffer (
                *(form_fields ((*(FORMS + form))->form) + i), 0,
                memcpy (*(*(FORM_FIELD_DATA + form) + i), "", sizeof (""))
            );

        return;
    }

    for (size_t i = 0; i < (*(FORMS + form))->nfields; i++)
        set_field_buffer (
            *(form_fields ((*(FORMS + form))->form) + i), 0,
            *(data + i) ? memcpy (*(*(FORM_FIELD_DATA + form) + i), *(data + i), strlen (*(data + i)) + 1)
                        : memcpy (*(*(FORM_FIELD_DATA + form) + i), "", sizeof (""))
        );
}

int get_form_exit_key (void) {
    return FORM_EXIT_KEY;
}

const char *get_form_exit_message (void) {
    return FORM_EXIT_MESSAGE;
}

int set_form_exit_key (const int key, const char *const restrict message) {
    if (!message)
        memcpy (FORM_EXIT_MESSAGE, DEFAULT_FORM_EXIT_MESSAGE, sizeof (DEFAULT_FORM_EXIT_MESSAGE));

    else if (message != FORM_EXIT_MESSAGE) {
        size_t l;
        if ((l = strlen (message)) < sizeof (FORM_EXIT_MESSAGE))
            memcpy (FORM_EXIT_MESSAGE, message, l + 1);

        else
            warning ("the form exit message is too big so the previous one will be used.");
    }

    return FORM_EXIT_KEY = key ? key : DEFAULT_FORM_EXIT_KEY;
}

int get_form_erase_key (void) {
    return FORM_ERASE_KEY;
}

const char *get_form_erase_message (void) {
    return FORM_ERASE_MESSAGE;
}

int set_form_erase_key (const int key, const char *const restrict message) {
    if (!message)
        memcpy (FORM_ERASE_MESSAGE, DEFAULT_FORM_ERASE_MESSAGE, sizeof (DEFAULT_FORM_ERASE_MESSAGE));

    else if (message != FORM_ERASE_MESSAGE) {
        size_t l;
        if ((l = strlen (message)) < sizeof (FORM_ERASE_MESSAGE))
            memcpy (FORM_ERASE_MESSAGE, message, l + 1);

        else
            warning ("the form save message is too big so the previous one will be used.");
    }

    return FORM_ERASE_KEY = key ? key : DEFAULT_FORM_ERASE_KEY;
}

int get_form_save_key (void) {
    return FORM_SAVE_KEY;
}

const char *get_form_save_message (void) {
    return FORM_SAVE_MESSAGE;
}

int set_form_save_key (const int key, const char *const restrict message) {
    if (!message)
        memcpy (FORM_SAVE_MESSAGE, DEFAULT_FORM_SAVE_MESSAGE, sizeof (DEFAULT_FORM_SAVE_MESSAGE));

    else if (message != FORM_SAVE_MESSAGE) {
        size_t l;
        if ((l = strlen (message)) < sizeof (FORM_SAVE_MESSAGE))
            memcpy (FORM_SAVE_MESSAGE, message, l + 1);

        else
            warning ("the form save message is too big so the previous one will be used.");
    }

    return FORM_SAVE_KEY = key ? key : DEFAULT_FORM_SAVE_KEY;
}
