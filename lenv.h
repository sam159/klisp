/* 
 * File:   lenv.h
 * Author: sam
 *
 * Created on 18 May 2014, 21:36
 */

#ifndef LENV_H
#define	LENV_H

#ifdef	__cplusplus
extern "C" {
#endif
    
    struct lenv;
    typedef struct lenv lenv;
    
    struct symtab;
    typedef struct symtab symtab;

    struct symtab {
        char* sym;
        lval* lval;
    };
    
    struct lenv {
        size_t count;
        struct symtab** syms;
    };

    lenv* lenv_new();
    void lenv_delete(lenv* env);
    
    int lenv_compare_symtabs(const void *a, const void *b);
    void lenv_sort(lenv* env);
    symtab* lenv_search(lenv* env, char* sym);
    
    lval* lenv_get(lenv* env, lval* sym);
    void lenv_put(lenv* env, lval* key, lval* val);
    
    symtab* symtab_new(char* sym, lval* lval);
    void symtab_delete(symtab* symtab);

#ifdef	__cplusplus
}
#endif

#endif	/* LENV_H */

