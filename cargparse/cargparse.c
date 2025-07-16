#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    ARG_TYPE_STRING,
    ARG_TYPE_INT,
    ARG_TYPE_BOOL
} ARG_TYPE;

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

static void arg_error(const char *opt, char *msg){
    printf("Error: Cannot parse argument '%s': %s\n", opt, msg);
}

/*
Returns
    0 for success
    1 for failure
    2 for ignore input (i.e. don't increment pointer)
*/
static int process_arg(ArgSpec *spec, char *in){
    switch(spec->type){
        case ARG_TYPE_STRING:
            if(in == NULL){
                printf("Must provide value for string parameter %s\n", spec->name);
                return 1;
            }

            char *buffer = (char*)spec->target;
            strncpy(buffer, in, spec->buffer_size);
            buffer[spec->buffer_size-1] = '\0';

            return 0;
        case ARG_TYPE_INT:
            if(in == NULL){ 
                printf("Must provide value for int parameter %s\n", spec->name);                
                return 1;
            }

            *(int*)spec->target = atoi(in);

            return 0;
        case ARG_TYPE_BOOL:
            if(in == NULL){
                *(int*)spec->target = 1;
                return 2;
            }

            switch(in[0]){
                case 'y':
                case 'Y':
                case 't':
                case 'T':
                case '1':
                    *(int*)spec->target = 1;
                    return 0;
                case 'n':
                case 'N':
                case 'f':
                case 'F':
                case '0':
                    *(int*)spec->target = 0;
                    return 0;
                default:
                    *(int*)spec->target = 1;
                    return 2;
            }
            
            return 1;
        default:
            return 1;
    }
}

static int parse_long_arg(int argc, char *argv[], int specc, ArgSpec specs[], char *arg, int *a, int found_l[]){
    // Find ArgSpec
    int found = 0;
    int res;

    for(int s = 0; s < specc; ++s){
        ArgSpec spec = specs[s];
        if( strcmp(arg, spec.name) == 0 ){
            found = 1;
            found_l[s] = 1;

            char *in = NULL;
            if(*a + 1 < argc){
                in = argv[*a+1];
            }

            res = process_arg(&spec, in);
            switch(res) {
                case 0:
                    (*a)++;
                    break;
                case 2:
                    break;
                default:
                    return res;
            }
        }
    }

    if(found == 0){
        arg_error(arg, "Unrecognized argument.");
        return 1;
    }

    return 0;
}

static int parse_short_arg(int argc, char *argv[], int specc, ArgSpec specs[], char *arg, int *a, int found_l[]){
    // int found = 0;
    // for(int c = 1; c < sizeof(arg); ++c){
    //     char copt = arg[c];
    //     for(int s = 0; s < specc; ++s){
    //         ArgSpec spec = specs[s];
    //         if(spec.short_name == copt){
    //             found = 1;
    //             if(spec.type == )
    //         }
    //     }
    // }

    printf("Short args aren't implemented yet.\n");
    return 1;
}

int parse_args(int argc, char *argv[], int specc, ArgSpec specs[]) {
    int found_l[specc];
    memset(found_l, 0, sizeof(found_l));

    for(int a = 1; a < argc; ++a){
        char *arg = argv[a];
        int res;

        if(sizeof(arg) < 2){
            arg_error(arg, "Arguments should be at least 2 characters.");
            return 1;
        }
        if(arg[0] == '-' && arg[1] == '-'){
            res = parse_long_arg(argc, argv, specc, specs, arg, &a, found_l);
        }
        else if(arg[0] == '-'){
            res = parse_short_arg(argc, argv, specc, specs, arg, &a, found_l);
        } else {
            arg_error(arg, "Unrecognized format.");
            return 1;
        }

        if(res != 0){
            return res;
        }
    }

    for(int s = 0; s < specc; ++s){
        ArgSpec spec = specs[s];
        int found = found_l[s];
        if(found == 0){
            if(spec.required != 0){
                printf("Missing required arg %s\n", spec.name);
                return 1;
            }

            if(spec.def != NULL){
                switch (spec.type) {
                    case ARG_TYPE_STRING:
                        strncpy((char*)spec.target, (char*)spec.def, spec.buffer_size - 1);
                        ((char*)spec.target)[spec.buffer_size-1] = '\0';
                        break;
                    case ARG_TYPE_INT:
                        *(int *)spec.target = (int)(intptr_t)spec.def;
                        break;
                    case ARG_TYPE_BOOL:
                        *(int *)spec.target = (int)(intptr_t)spec.def;
                        break;
                }
            }
        }
    }

    return 0;
}