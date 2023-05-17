#ifndef TRIVIA_CONFIG_H
#define TRIVIA_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#define DEFAULT_CONFIG_FILE "config.json"
#define CONFIG_FILE_LOCATOR ".config.path"

    extern const char *get_database_file (void);
    extern const char *set_database_file (const char *const restrict);
    extern const char *get_config_file (void);
    extern const char *set_config_file (const char *const restrict);
    extern cJSON      *get_config (const char *const restrict);
    extern char       *get_config_database (const char *const restrict);
    extern int         get_config_port (const char *const restrict);

#ifdef __cplusplus
}
#endif

#endif