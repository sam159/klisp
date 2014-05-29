/* 
 * File:   lval.h
 * Author: sam
 *
 * Created on 18 May 2014, 21:22
 */

#ifndef LVAL_H
#define	LVAL_H

#ifdef	__cplusplus
extern "C" {
#endif

//Forward declarations (if thats what they are called)
struct lval;
typedef struct lval lval;

struct lval_func;
typedef struct lval_func lval_func;

#include "main.h"
#include "lenv.h"

enum VAL_TYPE { LVAL_ERR, LVAL_NUM, LVAL_SYM, LVAL_FUNC, LVAL_S_EXPR, LVAL_Q_EXPR, LVAL_EXIT };
enum VAL_ERROR { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM, LERR_BAD_SYM, LERR_OTHER, LERR_SYNTAX };
typedef enum VAL_TYPE VAL_TYPE;
typedef enum VAL_ERROR VAL_ERROR;

typedef lval*(*lbuiltin)(lenv*, lval*);

struct lval_func {
    char* name;
    lbuiltin builtin;
    lenv* env;
    lval* formals;
    lval* body;
    lval* va;
};

struct lval {
  enum VAL_TYPE type;
  union {
     double_t num;
     char* sym;
     struct {
        enum VAL_ERROR num;
        char* detail;
     } err;
     struct lval_func* func;
  } data;
  
  int cell_count;
  struct lval** cell_list;
  
};
    
lval* lval_new(int type);
lval* lval_num(double_t x);
lval* lval_sym(char* x);
lval* lval_s_expr();
lval* lval_q_expr();
lval* lval_builtin(lbuiltin func, char* name);
lval* lval_lambda(lval* formals, lval* body);
lval* lval_exit();

lval* lval_add(lval* val, lval* x);
lval* lval_pop(lval* val, int i);
lval* lval_take(lval* val, int i);
lval* lval_join(lval* a, lval* b);

lval* lval_call(lenv* env, lval* function, lval* args);
BOOL lval_equal(lval* a, lval* b);

void lval_delete(lval* val);
lval* lval_copy(lval* current);

lval* lval_err(VAL_ERROR x);
lval* lval_err_detail(VAL_ERROR x, char* format, ...);
char* lval_str_name(VAL_TYPE type);

#ifdef	__cplusplus
}
#endif

#endif	/* LVAL_H */

