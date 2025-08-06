#include <time.h>
#include "util.h"

time_t clock_gettime_ms() {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    
    return (t.tv_sec * 1000) + (t.tv_nsec / (1000 * 1000));
}

