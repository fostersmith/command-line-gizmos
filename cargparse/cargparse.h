#ifndef cargparse_h
#define cargparse_h
    #include <stdio.h>
    #include <stdint.h>
    #include <stdlib.h>
    #include <string.h>

    extern char *CARGPARSE_MOTD;

    typedef enum {
        ARG_TYPE_STRING,
        ARG_TYPE_INT,
        ARG_TYPE_BOOL
    } ARG_TYPE;

    // Warning! If type == ARG_TYPE_BOOL, target must point to 4 bytes
    typedef struct {
        const void *target;
        const char *name;
        const char *short_name;
        const char *description;
        const ARG_TYPE type;
        const void *def;
        const int required;
        const size_t buffer_size;
    } ArgSpec;

    int parse_args(int argc, char *argv[], int specc, ArgSpec specs[]);

#endif