#include "cargparse.c"

int main(int argc, char **argv)
{
    int boolarg = 0;
    char stringarg[64];
    int intarg = 0;

    ArgSpec specs[] = {
        {&boolarg, "--boolarg", "-b", "Bool Arg", ARG_TYPE_BOOL, (void *)0, 0},
        {stringarg, "--strarg", "-s", "String Arg", ARG_TYPE_STRING, (void *)"STRING!", 0, sizeof(stringarg)},
        {&intarg, "--intarg", "-i", "Int Arg", ARG_TYPE_INT, (void *)10, 0},
    };
    if(parse_args(argc, argv, 3, specs) != 0) return 1;

    printf("Bool Arg: %d\n",boolarg);
    printf("String Arg: %s\n",stringarg);
    printf("Int Arg: %d\n",intarg);
}
