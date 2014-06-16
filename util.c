#include <string.h>
#include <stdlib.h>

#include "util.h"

char * strdup(char* s) {
    if (s == NULL) return NULL;
    char *d = calloc(strlen (s) + 1, sizeof(char));
    if (d == NULL) return NULL;
    strcpy (d,s);
    return d;
}
