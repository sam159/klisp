/* 
 * File:   lang.h
 * Author: sam
 *
 * Created on 18 May 2014, 21:19
 */

#ifndef LANG_H
#define	LANG_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "mpc.h"
#include "lval.h"
#include "lenv.h"
#include "functions.h"
    
#define LASSERT(val, cond, errnum, errdetail, ...) \
    if (!(cond)) { \
        lval * err = lval_err_detail(errnum, errdetail, ##__VA_ARGS__); \
        lval_delete(val); \
        return err; \
    }
    
#define LASSERT_TYPE(name, val, subject, expectedType) \
    LASSERT(val, subject->type == expectedType, \
            LERR_SYNTAX, "%s Expected type %s got %s", name, lval_str_name(expectedType), lval_str_name(subject->type))
#define LASSERT_ARG_COUNT(name, val, subject, expectedNum) \
    LASSERT(val, subject->cell_count == expectedNum, \
            LERR_SYNTAX, "%s Expected %ld arguments got %ld", name, expectedNum, subject->cell_count )
#define LASSERT_MIN_ARG_COUNT(name, val, subject, expectedNum) \
    LASSERT(val, subject->cell_count >= expectedNum, \
            LERR_SYNTAX, "%s Expected %ld or more arguments got %ld", name, expectedNum, subject->cell_count )
    
    
    
    mpc_ast_t* tokenize(char *input);
    lval* parse(mpc_ast_t *t);
    lval* eval_builtin_op(lval* val, char* op);
    lval* eval_s_expr(lenv* env, lval* val);
    lval* eval(lenv* env, lval* val);


#ifdef	__cplusplus
}
#endif

#endif	/* LANG_H */

