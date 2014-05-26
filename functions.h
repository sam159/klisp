/* 
 * File:   functions.h
 * Author: sam
 *
 * Created on 18 May 2014, 22:41
 */

#ifndef FUNCTIONS_H
#define	FUNCTIONS_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "lang.h"
    
    void lenv_add_builtin(lenv* env, char* sym, lbuiltin func);
    
    void lenv_add_builtin_funcs(lenv* env);
    
    //Math functions
    lval* builtin_add(lenv* env, lval* val);
    lval* builtin_sub(lenv* env, lval* val);
    lval* builtin_div(lenv* env, lval* val);
    lval* builtin_mul(lenv* env, lval* val);
    lval* builtin_pow(lenv* env, lval* val);
    lval* builtin_op(lenv* env, lval* val, char* op);
    
    //List/Util functions
    lval* builtin_list(lenv* env, lval* val);
    lval* builtin_eval(lenv* env, lval* val);
    lval* builtin_join(lenv* env, lval* val);
    lval* builtin_head(lenv* env, lval* val);
    lval* builtin_tail(lenv* env, lval* val);
    
    //ENV Functions
    lval* builtin_envdef(lenv* env, lval* val, char* type);
    lval* builtin_def(lenv* env, lval* val);
    lval* builtin_var(lenv* env, lval* val);
    lval* builtin_listenv(lenv* env, lval* val);
    lval* builtin_exit(lenv* env, lval* val);
    lval* builtin_lambda(lenv* env, lval* val);
    
#ifdef	__cplusplus
}
#endif

#endif	/* FUNCTIONS_H */

