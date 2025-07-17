#define _POSIX_C_SOURCE 199309L
#include <time.h>
#include <stdio.h>

int main() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    printf("Time: %ld.%09ld\n", ts.tv_sec, ts.tv_nsec);
    return 0;
}
