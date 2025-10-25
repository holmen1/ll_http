#include <stdio.h>
#include <stdlib.h>

void debug_log(const char *message) {
    fprintf(stderr, "DEBUG: %s\n", message);
}

