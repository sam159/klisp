#include <stdlib.h>
#include <float.h>

#include "lang.h"
#include "main.h"

void lenv_add_builtin(lenv* env, char* sym, lbuiltin func) {
    lval* symval = lval_sym(sym);
    lval* funcval = lval_func(func, sym);
    
    lenv_put(env, symval, funcval);
    
    lval_delete(symval);
    lval_delete(funcval);
}

void lenv_add_builtin_funcs(lenv* env) {
    //Math functions
    lenv_add_builtin(env, "+", builtin_add);
    lenv_add_builtin(env, "-", builtin_sub);
    lenv_add_builtin(env, "/", builtin_div);
    lenv_add_builtin(env, "*", builtin_mul);
    lenv_add_builtin(env, "^", builtin_pow);
    
    //List/Util functions
    lenv_add_builtin(env, "list", builtin_list);
    lenv_add_builtin(env, "eval", builtin_eval);
    lenv_add_builtin(env, "join", builtin_join);
    lenv_add_builtin(env, "head", builtin_head);
    lenv_add_builtin(env, "tail", builtin_tail);
    
    //ENV Functions
    lenv_add_builtin(env, "def", builtin_def);
    lenv_add_builtin(env, "listenv", builtin_listenv);
    lenv_add_builtin(env, "exit", builtin_exit);
}

lval* builtin_add(lenv* env, lval* val) {
    return builtin_op(env, val, "+");
}
lval* builtin_sub(lenv* env, lval* val) {
    return builtin_op(env, val, "-");
}
lval* builtin_div(lenv* env, lval* val) {
    return builtin_op(env, val, "+");
}
lval* builtin_mul(lenv* env, lval* val) {
    return builtin_op(env, val, "*");
}
lval* builtin_pow(lenv* env, lval* val){
    return builtin_op(env, val, "^");
}
lval* builtin_op(lenv* env, lval* val, char* op) {
    //Ensure numbers only
    for(int i = 0; i < val->cell_count; i++) {
        if (val->cell_list[i]->type != LVAL_NUM) {
            lval_delete(val);
            return lval_err(LERR_BAD_NUM);
        }
    }
    
    //Get the first element
    lval* x = lval_pop(val, 0);
    
    if (strcmp(op, "-") == 0 && val->cell_count == 0) {
        x->data.num = -x->data.num;
    } 
    
    while(val->cell_count > 0) {
        //Get next to process
        lval* y = lval_pop(val, 0);
        
        if (strcmp(op, "+") == 0) { x->data.num += y->data.num; }
        if (strcmp(op, "-") == 0) { x->data.num -= y->data.num; }
        if (strcmp(op, "*") == 0) { x->data.num *= y->data.num; }
        if (strcmp(op, "^") == 0) { x->data.num = pow(x->data.num,y->data.num); }
        if (strcmp(op, "/") == 0) {
            int zero = 0;
            if (y->type == LVAL_NUM && fabs(y->data.num) <= DBL_EPSILON) {zero = 1;}

            if (zero) {
                lval_delete(x);
                lval_delete(y);
                x = lval_err(LERR_DIV_ZERO);
                break;
            }
            x->data.num /= y->data.num;
        }
        
        lval_delete(y);
    }
    
    lval_delete(val);
    return x;
}

lval* builtin_list(lenv* env, lval* val){
    val->type = LVAL_Q_EXPR;
    return val;
}
lval* builtin_eval(lenv* env, lval* val){
    LASSERT_ARG_COUNT("eval", val, val, 1);
    LASSERT_TYPE("eval", val, val->cell_list[0], LVAL_Q_EXPR);
    
    lval* x = lval_take(val, 0);
    x->type = LVAL_S_EXPR;
    return eval(env, x);
}
lval* builtin_join(lenv* env, lval* val){
    LASSERT_MIN_ARG_COUNT("join", val, val, 1);
    for(int i = 0; i < val->cell_count; i++) {
        LASSERT_TYPE("join", val, val->cell_list[i], LVAL_Q_EXPR);
    }
    
    lval* x = lval_pop(val,0);
    
    while(val->cell_count > 0) {
        x = lval_join(x, lval_pop(val, 0));
    }
    
    return x;
}
lval* builtin_head(lenv* env, lval* val){
    LASSERT_ARG_COUNT("head",val , val, 1);
    LASSERT_TYPE("head",val , val->cell_list[0], LVAL_Q_EXPR);
    LASSERT_MIN_ARG_COUNT("head", val, val->cell_list[0], 1);
    
    lval* x = lval_take(val, 0);
    while(x->cell_count > 1) { lval_delete(lval_pop(x, 1)); }
    return x;
}
lval* builtin_tail(lenv* env, lval* val){
    LASSERT_ARG_COUNT("tail",val , val, 1);
    LASSERT_TYPE("tail", val, val->cell_list[0], LVAL_Q_EXPR);
    LASSERT_MIN_ARG_COUNT("tail", val, val->cell_list[0], 1);
    
    lval* x = lval_take(val, 0);
    lval_delete(lval_pop(x, 0));
    return x;
}

lval* builtin_def(lenv* env, lval* val) {
    LASSERT_MIN_ARG_COUNT("def", val, val, 1);
    LASSERT_TYPE("def", val, val->cell_list[0], LVAL_Q_EXPR);
    
    lval* symbols = val->cell_list[0];
    
    for(int i = 0; i < symbols->cell_count; i++) {
        LASSERT_TYPE("def", val, symbols->cell_list[i], LVAL_SYM);
    }
    
    LASSERT(val, symbols->cell_count == val->cell_count -1,
            LERR_OTHER, "def: incorrect number of definitions for symbols");
    
    for(int i = 0; i < symbols->cell_count; i++) {
        lenv_put(env, symbols->cell_list[i], val->cell_list[i+1]);
    }
    
    lval_delete(val);
    return lval_s_expr();
}

lval* builtin_listenv(lenv* env, lval* val) {
    for(int i=0; i< env->count; i++) {
        printf("%s: ", env->syms[i]->sym);
        lval_println(env->syms[i]->lval);
    }
    lval_delete(val);
    return lval_s_expr();
}

lval* builtin_exit(lenv* env, lval* val) {
    lval* args = lval_q_expr();
    for(int i=0; i<val->cell_count; i++) {
        lval_add(args, lval_copy(val->cell_list[i]));
    }
    lval* sym = lval_sym("exitcode");
    lenv_put(env, sym, args);
    lval_delete(args);
    lval_delete(sym);
    
    lval_delete(val);
    return lval_exit();
}