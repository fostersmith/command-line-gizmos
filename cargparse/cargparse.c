#include "cargparse.h"

char *CARGPARSE_MOTD = "Set me with CARGPARSE_MOTD!";
// static void print_spec_val(ArgSpec *spec){
//     if(spec->target == NULL){
//         printf("Spec %s: NULL\n", spec->name);
//     }
//     else {
//         switch(spec->type){
//             case ARG_TYPE_BOOL:
//             case ARG_TYPE_INT:
//                 int *vali = (int*)spec->target;
//                 printf("Spec %s: %d\n", spec->name, *vali);
//                 break;
//             case ARG_TYPE_STRING:
//                 char *val = (char*)spec->target;
//                 printf("Spec %s: %s\n", spec->name, val);
//         }
//     }
// }

// static void print_spec_vals(int specc, ArgSpec specs[]){
//     for(int s = 0; s < specc; ++s){
//         ArgSpec spec = specs[s];
//         print_spec_val(&spec);
//     }
// }

static int check_spec(ArgSpec *spec){
    if(spec->name == NULL){
        printf("Must provide name for all specs.\n");
        return 1;
    } 
    if(spec->target == NULL){
        printf("Must provide target for spec '%s'.\n", spec->name);
        return 1;
    } 
    if(spec->type!=ARG_TYPE_BOOL && spec->type!=ARG_TYPE_INT && spec->type!=ARG_TYPE_STRING){
        printf("Invalid arg type for spec '%s'.\n", spec->name);
        return 1;
    }
    if(spec->required != 0 && spec->required != 1){
        printf("'required' should be 1 or 0 for spec '%s'.\n", spec->name);
        return 1;
    } 
    if(spec->type == ARG_TYPE_STRING && spec->buffer_size == 0){
        printf("Must to specify buffer size for string spec '%s'.\n", spec->name);
        return 1;
    }
    if(spec->short_name != NULL) {
        if(strlen(spec->short_name) != 2 || spec->short_name[0] != '-'){
            printf("Invalid short name for spec '%s': should have one dash and one character.\n", spec->name);
            return 1;
        }
    }
    return 0;
}

static void print_help(int specc, ArgSpec specs[]){
    if(CARGPARSE_MOTD != NULL){
        printf("%s\n\n", CARGPARSE_MOTD);
    }
    printf("Options:\n");
    printf("  %-20s %s\n", "--help, -h", "Show this screen");
    char name_buffer[100];
    for(int s = 0; s < specc; ++s){
        ArgSpec spec = specs[s];
        const char *name = spec.name;
        const char *short_name = (spec.short_name == NULL) ? "" : spec.short_name;
        const char *desc = (spec.description == NULL) ? "" : spec.description;
        if(short_name != NULL)
            snprintf(name_buffer, sizeof(name_buffer), "%s, %s", name, short_name);
        else
            snprintf(name_buffer, sizeof(name_buffer), "%s", name);
        printf("  %-20s %s\n", name_buffer, desc);
    }
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

static int findspec(int specc, ArgSpec specs[], char *arg){
    for(int s = 0; s < specc; ++s){
        ArgSpec spec = specs[s];
        if(strcmp(arg, spec.name) == 0) {
            return s;
        }
        if(spec.short_name != NULL){
            if(strcmp(arg, spec.short_name) == 0){
                return s;
            }
        }
    }
    return -1;
}

static int process_flag_string(int specc, ArgSpec specs[], int found_l[], char *arg){
    int arglen = strlen(arg);
    int specn;
    int res;

    int i = arg[0] == '-' ? 1 : 0;
    for(; i < arglen; ++i){
        char arg_ver[3] = {'-', arg[i], '\0'};
        specn = findspec(specc, specs, arg_ver);
        if(specn != -1){
            res = process_arg(&specs[specn], NULL);
            if(res != 0 && res != 2){
                printf("Can't process option '%s' as boolean flag.\n", specs[specn].name);
                return res;
            } else {
                found_l[specn] = 1;
            }
        } else {
            printf("Unrecognized flag '%s'\n", arg_ver);
            return 1;
        }
    }

    return 0;
}

int parse_args(int argc, char *argv[], int specc, ArgSpec specs[]) {

    for(int s = 0; s < specc; ++s){
        int valid = check_spec(&specs[s]);
        if(valid != 0) return 1;
    }

    int found_l[specc];
    memset(found_l, 0, sizeof(found_l));

    for(int a = 1; a < argc; ++a){
        char *arg = argv[a];
        int arglen = strlen(arg);
        char *val;
        int res;
        int specn;

        if(arglen < 2){
            printf("Couldn't parse arg %s: Arguments should be at least 2 characters.\n", arg);
            return 1;
        }

        if( strcmp(arg, "--help") == 0 || strcmp(arg, "-h") == 0 ){
            print_help(specc, specs);
            return 1;
        }

        specn = findspec(specc, specs, arg);

        if(specn != -1){
            // Standard
            found_l[specn] = 1;

            if(a+1 < argc)
                val = argv[a+1];
            else
                val = NULL;
            
            res = process_arg(&specs[specn], val);
            switch(res) {
                case 0:
                    ++a;
                    break;
                case 2:
                    break;
                default:
                    printf("Failed to process arg %s\n", arg);
                    return 1;
            }
        } else {
            // Flag string (e.g. -sao)
            res = process_flag_string(specc, specs, found_l, arg);
            if(res != 0) return res;
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