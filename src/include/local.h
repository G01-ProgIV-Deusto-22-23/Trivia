#ifndef TRIVIA_LOCAL_H
#define TRIVIA_LOCAL_H

#ifdef __cplusplus
extern "C" {
#endif

    __attribute__ ((noreturn)) extern void welcome (void);
    extern choicefunc_t                    server_status_menu;
    extern choicefunc_t                    configurarServidor;
    extern choicefunc_t                    usuarioMenu;

#ifdef __cplusplus
}
#endif

#endif
