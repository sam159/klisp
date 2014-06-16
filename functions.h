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
    
    typedef enum BUILTIN_OP_TYPE BUILTIN_OP_TYPE;
    
    enum BUILTIN_OP_TYPE {
        BUILTIN_OP_ADD, BUILTIN_OP_SUB, BUILTIN_OP_DIV, BUILTIN_OP_MUL, BUILTIN_OP_POW,
        BUILTIN_COMP_GT, BUILTIN_COMP_LT, BUILTIN_COMP_GE, BUILTIN_COMP_LE,
        BUILTIN_COMP_EQ, BUILTIN_COMP_NEQ,
        BUILTIN_LOGICAL_OR, BUILTIN_LOGICAL_AND, BUILTIN_LOGICAL_NOT
    };
    
    char* builtin_op_strname(BUILTIN_OP_TYPE op);
    
    void lenv_add_builtin(lenv* env, char* sym, lbuiltin func);
    
    void lenv_add_builtin_funcs(lenv* env);
    
    //Math functions
    lval* builtin_op(lenv* env, lval* val, BUILTIN_OP_TYPE op);
    lval* builtin_add(lenv* env, lval* val);
    lval* builtin_sub(lenv* env, lval* val);
    lval* builtin_div(lenv* env, lval* val);
    lval* builtin_mul(lenv* env, lval* val);
    lval* builtin_pow(lenv* env, lval* val);
    
    //Comparison Functions
    lval* builtin_comp_num(lenv* env, lval* val, BUILTIN_OP_TYPE op);
    lval* builtin_comp_gt(lenv* env, lval* val);
    lval* builtin_comp_lt(lenv* env, lval* val);
    lval* builtin_comp_ge(lenv* env, lval* val);
    lval* builtin_comp_le(lenv* env, lval* val);
    lval* builtin_comp_value(lenv* env, lval* val, BUILTIN_OP_TYPE op);
    lval* builtin_comp_eq(lenv* env, lval* val);
    lval* builtin_comp_neq(lenv* env, lval* val);
    lval* builtin_logical(lenv* env, lval* val, BUILTIN_OP_TYPE op);
    lval* builtin_logical_and(lenv* env, lval* val);
    lval* builtin_logical_or(lenv* env, lval* val);
    lval* builtin_logical_not(lenv* env, lval* val);
    
    //List/Util functions
    lval* builtin_list(lenv* env, lval* val);
    lval* builtin_eval(lenv* env, lval* val);
    lval* builtin_join(lenv* env, lval* val);
    lval* builtin_head(lenv* env, lval* val);
    lval* builtin_tail(lenv* env, lval* val);
    lval* builtin_if(lenv* env, lval* val);
    
    //ENV Functions
    lval* builtin_envdef(lenv* env, lval* val, char* type);
    lval* builtin_def(lenv* env, lval* val);
    lval* builtin_var(lenv* env, lval* val);
    lval* builtin_listenv(lenv* env, lval* val);
    lval* builtin_exit(lenv* env, lval* val);
    lval* builtin_lambda(lenv* env, lval* val);
    lval* builtin_load(lenv* env, lval* val);
    lval* builtin_loadonce(lenv* env, lval* val);
    lval* builtin_do_load(lenv* env, lval* val, BOOL loadonce);
    
#ifdef	__cplusplus
}
#endif

#endif	/* FUNCTIONS_H */

