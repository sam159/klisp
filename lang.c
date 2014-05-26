#include <float.h>
#include <stdio.h>

#include "mpc.h"
#include "lang.h"

mpc_ast_t* tokenize(char *input) {
    
    mpc_parser_t* Number = mpc_new("number");
    mpc_parser_t* Symbol = mpc_new("symbol");
    mpc_parser_t* S_Expr = mpc_new("s_expr");
    mpc_parser_t* Q_Expr = mpc_new("q_expr");
    mpc_parser_t* Expr = mpc_new("expr");
    mpc_parser_t* Lispy = mpc_new("lispy");
    
    mpca_lang(MPCA_LANG_DEFAULT,
        "                                                        \
          number   : /-?[0-9]+(\\.[0-9]+)?/ ;                    \
          symbol : /[a-zA-Z0-9_+\\-*\\/\\\\=<>!&]+/ ;         \
          s_expr   : '(' <expr>* ')' ;                           \
          q_expr   : '{' <expr>* '}' ;                           \
          expr     : <number> | <symbol> | <s_expr> | <q_expr> ; \
          lispy    : /^/ <expr>+ /$/ ;                           \
        ",
        Number, Symbol, Expr, S_Expr, Q_Expr, Lispy);
    
    mpc_result_t result;
    int success = 1;
    
    if (!mpc_parse("<stdin>", input,Lispy, &result)) {
        success = 0;
        mpc_err_print(result.error);
        mpc_err_delete(result.error);
    }
    
    mpc_cleanup(6, Number, Symbol, S_Expr, Expr, Lispy, Q_Expr);
    
    return success ? (mpc_ast_t*)result.output : NULL;
}

lval* parse(mpc_ast_t *t) {
    if (strstr(t->tag, "number")) {
        errno = 0;
        double_t d = strtod(t->contents, NULL);
        return errno != 0 ? lval_err_detail(LERR_BAD_NUM, strerror(errno)) : lval_num(d);
    }
    if (strstr(t->tag, "symbol")) {
        return lval_sym(t->contents);
    }
    
    lval* result = NULL;
    if (strcmp(t->tag, ">") == 0) { result = lval_s_expr(); } 
    if (result == NULL && strstr(t->tag, "s_expr"))  { result = lval_s_expr(); }
    if (result == NULL && strstr(t->tag, "q_expr"))  { result = lval_q_expr(); }

    for (int i = 0; i < t->children_num; i++) {
      if (strcmp(t->children[i]->contents, "(") == 0) { continue; }
      if (strcmp(t->children[i]->contents, ")") == 0) { continue; }
      if (strcmp(t->children[i]->contents, "}") == 0) { continue; }
      if (strcmp(t->children[i]->contents, "{") == 0) { continue; }
      if (strcmp(t->children[i]->tag,  "regex") == 0) { continue; }
      result = lval_add(result, parse(t->children[i]));
    }
    return result;
}

lval* eval_s_expr(lenv* env, lval* val) {
    
    //Eval children
    for(int i = 0; i < val->cell_count; i++) {
        val->cell_list[i] = eval(env, val->cell_list[i]);
    }
    
    //Check for errors or exit
    for(int i = 0; i < val->cell_count; i++) {
        if (val->cell_list[i]->type == LVAL_ERR) {
            return lval_take(val, i);
        }
        if (val->cell_list[i]->type == LVAL_EXIT) {
            lval* exit = lval_copy(val->cell_list[i]);
            lval_delete(val);
            return exit;
        }
    }
    
    //empty
    if (val->cell_count == 0) {
        return val;
    }
    
    //single
    if (val->cell_count == 1) {
        return lval_take(val, 0);
    }
    
    //Ensure first is symbol
    lval* func = lval_pop(val, 0);
    if (func->type != LVAL_FUNC) {
        VAL_TYPE type = func->type;
        lval_delete(func); 
        lval_delete(val);
        return lval_err_detail(LERR_BAD_OP, "First element is not %s got %s", lval_str_name(LVAL_FUNC), lval_str_name(type));
    }
    
    //Call builtin
    lval* result = lval_call(env, func, val);
    lval_delete(func);
    return result;
    
}
lval* eval(lenv* env, lval* val) {
    if (val->type == LVAL_SYM) {
        lval* x = lenv_get(env, val);
        lval_delete(val);
        return x;
    }
    if (val->type == LVAL_S_EXPR) {
        return eval_s_expr(env, val);
    }
    return val;
}