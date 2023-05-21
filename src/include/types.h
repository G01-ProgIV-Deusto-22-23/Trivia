#ifndef TRIVIA_TYPES_H
#define TRIVIA_TYPES_H

enum gcc_type_class {
    no_type_class = -1,
    void_type_class,
    integer_type_class,
    char_type_class,
    enumeral_type_class,
    boolean_type_class,
    pointer_type_class,
    reference_type_class,
    offset_type_class,
    real_type_class,
    complex_type_class,
    function_type_class,
    method_type_class,
    record_type_class,
    union_type_class,
    array_type_class,
    string_type_class,
    lang_type_class
};

#define log_message ((log_level_t) IMPL_LOG_LEVEL_MESSAGE)
#define log_warning ((log_level_t) IMPL_LOG_LEVEL_WARNING)
#define log_error   ((log_level_t) IMPL_LOG_LEVEL_ERROR)

typedef enum { IMPL_LOG_LEVEL_MESSAGE = 0, IMPL_LOG_LEVEL_WARNING = 1, IMPL_LOG_LEVEL_ERROR = 2 } log_level_t;

typedef void errorfunc_t (void *const restrict);

typedef enum { actionmenu, choicemenu, multimenu } menutype_t;

typedef enum {
    title_no_color = 0,
    title_red,
    title_green,
    title_blue,
    title_yellow,
    title_magenta,
    title_cyan
} title_color_t;

typedef void choicefunc_t (void);

typedef struct {
        size_t     len;
        FIELDTYPE *type;

        union {
                struct {
                        int  min;
                        bool passwd;
                } alnum;

                struct {
                        long min;
                        long max;
                } integer;
        } type_args;
} field_attr_t;

typedef int delwinfunc_t (void *);

typedef uint64_t       getdimsfunc_t (void);
typedef uint32_t       getwidthfunc_t (void);
typedef getwidthfunc_t getheightfunc_t;

typedef uint64_t       setdimsfunc_t (uint32_t, uint32_t);
typedef uint32_t       setwidthfunc_t (const uint32_t);
typedef setwidthfunc_t setheightfunc_t;

typedef enum { server_off, server_on, server_starting, server_restarting, server_error } server_status_t;

typedef int cmpfunc_t (void *, void *);

typedef struct __linkedlist_struct *linkedlist_t;
typedef struct __map_struct        *map_t;

typedef void freefunc_t (void *);

typedef enum { u32_key, u64_key, str_key } key_type_t;

#include "ui.h"
#define MAX_PACKET_SZ (MAX_FORM_FIELD_LEN + 1)

typedef struct __attribute__ ((packed)) {
        uint16_t len;
        char     text [MAX_PACKET_SZ];
} packet_t;

typedef enum __attribute__ ((packed)
) { cmd_kill = 1,
    cmd_game_list,
    cmd_game_connect,
    cmd_game_create,
    cmd_game_results,
    cmd_packet,
    cmd_packet_cont,
    cmd_user_creds,
    cmd_user_insert,
    cmd_user_update,
    cmd_success = 0,
    cmd_error   = -1 } cmdname_t;

#define MAX_CMD_ARG 4

#define CMD_ERROR_SEND        0
#define CMD_ERROR_RECV        1
#define CMD_ERROR_INVALID     2
#define CMD_ERROR_INSERT_USER 3
#define CMD_ERROR_NO_USER     4

typedef struct __attribute__ ((packed)) {
        uint8_t players;
        uint8_t round_time;
} game_attr_t;

#define MAX_QUESTIONS     91
#define MAX_QUESTION_TEXT (MAX_PACKET_SZ >> 1)
#define MAX_ANSWER_TEXT   (MAX_QUESTION_TEXT >> 3)

typedef struct __attribute__ ((packed)) {
        uint8_t correct;
        char    text [MAX_ANSWER_TEXT];
} answer_t;

typedef struct __attribute__ ((packed)) {
        uint8_t  n;
        char     text [MAX_QUESTION_TEXT];
        answer_t ans [4];
} question_t;

typedef struct {
        int  ID_Usuario;
        char nombreVisible [21];
        char username [21];
        char contrasena [21];
        int  aciertosTotales;
        int  fallosTotales;
        int  ID_Presets;
} Usuario;

typedef struct {
        char nombre [21];
        int  ID_Categoria;
} Categoria;

typedef struct {
        int  ID_Presets;
        int  nJugadores;
        int  nRondas;
        int  RoundTime;
        char Categorias [101];
        char Mecanica1 [4];
        char Mecanica2 [4];
        char Mecanica3 [4];
        char Mecanica4 [4];
} Presets;
typedef struct{
	char * respuesta;
	bool correcta;
} Respuesta;
typedef struct {
	char * tipo;
	char * enunciado;
	Respuesta opciones[4];
} Pregunta;

typedef struct __attribute__ ((packed)) {
        cmdname_t cmd;

        union {
                struct {
                        uint32_t arg [MAX_CMD_ARG];
                };

                game_attr_t game;

                Usuario user;

                packet_t pack;
        } info;
} cmd_t;

#endif
