#include <string.h>
#include <stdlib.h>

#include "util.h"

char * strdup(char* s) {
    char *d = calloc(1, strlen (s) + 1);
    if (d == NULL) return NULL;
    strcpy (d,s);
    return d;
}
