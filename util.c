#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include "util.h"

char * strdup(const char* s) {
    if (s == NULL) return NULL;
    char *d = calloc(strlen (s) + 1, sizeof(char));
    if (d == NULL) return NULL;
    strcpy (d,s);
    return d;
}

char* vstrcat(int n, ...) {
    va_list va;
    
    char** strings = calloc(n, sizeof(char*));
    size_t stringLength = 0;
    
    va_start(va, n);
    
    for(int i = 0; i<n; i++) {
        strings[i] = va_arg(va, char*);
        stringLength += strlen(strings[i]);
    }
    
    va_end(va);
    
    char* newStr = calloc(stringLength+1, sizeof(char));
    
    for(int i = 0; i<n; i++) {
        strcat(newStr, strings[i]);
    }
    
    free(strings);
    
    return newStr;
}