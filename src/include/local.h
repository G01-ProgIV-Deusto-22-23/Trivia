#ifndef TRIVIA_LOCAL_H
#define TRIVIA_LOCAL_H

extern
#if defined(__cpp_attributes) || __STDC_VERSION__ > 201710L
    [[noreturn]]
#else
    __attribute__ ((noreturn))
#endif
    void
                    welcome (void);
extern choicefunc_t server_status_menu;
extern choicefunc_t configurarServidor;
extern choicefunc_t usuarioMenu;

#endif
