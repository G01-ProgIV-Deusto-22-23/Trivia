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

typedef enum { server_off, server_on, server_starting } server_status_t;

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

#endif
