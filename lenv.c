#include <stdlib.h>
#include <bits/stdlib-bsearch.h>

#include "lang.h"
#include "util.h"

lenv* lenv_new() {
    lenv* env = calloc(1, sizeof(lenv));
    env->count = 0;
    env->syms = NULL;
    return env;
}
void lenv_delete(lenv* env) {
    for(int i = 0; i < env->count; i++) {
        lval_delete(env->syms[i]->lval);
        free(env->syms[i]->sym);
        free(env->syms[i]);
    }
    free(env->syms);
    free(env);
}

int lenv_compare_symtabs(const void *lhs, const void *rhs) {
    const struct symtab* l = *(const struct symtab**)lhs;
    const struct symtab* r = *(const struct symtab**)rhs;
    
    return strcmp(l->sym, r->sym);
}

void lenv_sort(lenv* env) {
    qsort(env->syms, env->count, sizeof(symtab*), lenv_compare_symtabs);
}

symtab* lenv_search(lenv* env, char* sym) {
    if (env->count == 0) {
        return NULL;
    }
    symtab* searchElem = symtab_new(sym, NULL);
    
    symtab** searchComp = calloc(1, sizeof(symtab*));
    searchComp[0] = searchElem;
    
    symtab** result = bsearch(searchComp, env->syms, env->count, sizeof(symtab*), lenv_compare_symtabs);
    
    free(searchComp);
    symtab_delete(searchElem);
    
    if (result == NULL) {
        return NULL;
    }
    return *result;
}

lval* lenv_get(lenv* env, lval* sym) {
    LASSERT(sym, sym->type == LVAL_SYM, LERR_BAD_OP, "Expected symbol");
    
    symtab* result = lenv_search(env, sym->data.sym);
    return result != NULL ? lval_copy(result->lval) : lval_err(LERR_BAD_SYM);
}

void lenv_put(lenv* env, lval* key, lval* val) {
    symtab* result = lenv_search(env, key->data.sym);
    if (result != NULL) {
        lval_delete(result->lval);
        result->lval = lval_copy(val);
        
        lenv_sort(env);
        return;
    }
    
    env->count ++;
    
    env->syms = realloc(env->syms, sizeof(symtab*) * env->count);
    
    env->syms[env->count-1] = symtab_new(key->data.sym, val);
    
    lenv_sort(env);
}

symtab* symtab_new(char* sym, lval* lval) {
    symtab* new = calloc(1, sizeof(symtab));
    new->lval = lval == NULL ? NULL : lval_copy(lval);
    new->sym = strdup(sym);
    return new;
}
void symtab_delete(symtab* symtab) {
    if (symtab->lval != NULL) {
        lval_delete(symtab->lval);
    }
    free(symtab->sym);
    free(symtab);
}