#ifndef TRIVIA_FORM_H
#define TRIVIA_FORM_H

typedef struct __form_struct {
        FORM  *form;
        size_t nfields;
        int    dims [4];
        bool   saveexit;
        char   data [
#ifdef __cplusplus
            0
#endif
        ]; // Flexible array member (cache+++)
} form_t;

extern form_t                *FORMS [__WORDSIZE];
extern volatile atomic_size_t FORM_CONTROL;
extern volatile atomic_int    FREE_FORM_ERR;
#ifdef _WIN32
extern HANDLE FREE_FORM_SEMS [sizeof (FORMS) / sizeof (*FORMS)];
#else
extern sem_t FREE_FORM_SEMS [sizeof (FORMS) / sizeof (*FORMS)];
#endif

extern void trivia_free_form (const size_t);
extern void start_form_gc (void);
extern void stop_form_gc (void);

#endif