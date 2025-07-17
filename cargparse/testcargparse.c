#include "cargparse.c"

int main(int argc, char **argv)
{
    CARGPARSE_MOTD = "Test case for cargparse";

    int boolarg1 = 0;
    int boolarg2 = 0;
    char stringarg[64];
    int intarg = 0;

    ArgSpec specs[] = {
        {&boolarg1, "--boolarg1", "-a", "Bool Arg", ARG_TYPE_BOOL, (void *)0, 0},
        {&boolarg2, "--boolarg2", "-b", "Bool Arg", ARG_TYPE_BOOL, (void *)0, 0},
        {stringarg, "--strarg", "-s", "String Arg", ARG_TYPE_STRING, (void *)"STRING!", 0, sizeof(stringarg)},
        {&intarg, "--intarg", "-i", "Int Arg", ARG_TYPE_INT, (void *)10, 0},
    };
    if(parse_args(argc, argv, 4, specs) != 0) return 1;

    printf("Bool Arg 1: %d\n",boolarg1);
    printf("Bool Arg 2: %d\n",boolarg2);
    printf("String Arg: %s\n",stringarg);
    printf("Int Arg: %d\n",intarg);
}
