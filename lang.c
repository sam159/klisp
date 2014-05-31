#include <float.h>
#include <stdio.h>

#include "mpc.h"
#include "lang.h"

mpc_ast_t* tokenize(char *input) {
    mpc_result_t result;
    int success = 1;
    
    if (!mpc_parse("<stdin>", input,gLispy, &result)) {
        success = 0;
        mpc_err_print(result.error);
        mpc_err_delete(result.error);
    }
    
    return success ? (mpc_ast_t*)result.output : NULL;
}
void setup_parsers() {
    mpc_parser_t* Number = mpc_new("number");
    mpc_parser_t* Symbol = mpc_new("symbol");
    mpc_parser_t* String = mpc_new("string");
    mpc_parser_t* Comment = mpc_new("comment");
    mpc_parser_t* S_Expr = mpc_new("s_expr");
    mpc_parser_t* Q_Expr = mpc_new("q_expr");
    mpc_parser_t* Expr = mpc_new("expr");
    mpc_parser_t* Lispy = mpc_new("lispy");
    
    mpca_lang(MPCA_LANG_DEFAULT,
        "                                                        \
          number   : /-?[0-9]+(\\.[0-9]+)?/ ;                    \
          symbol   : /[a-zA-Z0-9_+\\-*\\/\\\\=<>!&\\|]+/ ;       \
          string   : /\"(\\\\.|[^\"])*\"/ ;                      \
          comment  : /;[^\\r?\\n]*/ ;                            \
          s_expr   : '(' <expr>* ')' ;                           \
          q_expr   : '{' <expr>* '}' ;                           \
          expr     : <number> | <symbol> | <string>              \
                   | <comment> | <s_expr> | <q_expr> ;           \
          lispy    : /^/ <expr>+ /$/ ;                           \
        ",
        Number, Symbol, String, Comment, Expr, S_Expr, Q_Expr, Lispy);
    
    gLispy = Lispy;
    gParsers = calloc(20, sizeof(mpc_parser_t*));
    int i = 0;
    gParsers[i++] = Number;
    gParsers[i++] = Symbol;
    gParsers[i++] = String;
    gParsers[i++] = Comment;
    gParsers[i++] = Q_Expr;
    gParsers[i++] = S_Expr;
    gParsers[i++] = Expr;
    gParsers[i++] = Lispy;
    gParsers = realloc(gParsers, sizeof(mpc_parser_t*)*i);
    gParserCount = i;
}
void cleanup_parsers() {
    for (int i = 0; i < gParserCount; i++) { mpc_undefine(gParsers[i]); }
    for (int i = 0; i < gParserCount; i++) { mpc_delete(gParsers[i]); } 
    free(gParsers);
    gParsers = NULL;
    gParserCount = 0;
    gLispy = NULL;
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
    if (strstr(t->tag, "string")) {
        return parse_read_string(t);
    }
    
    lval* result = NULL;
    if (strcmp(t->tag, ">") == 0) { result = lval_s_expr(); } 
    if (result == NULL && strstr(t->tag, "s_expr") != NULL)  { result = lval_s_expr(); }
    if (result == NULL && strstr(t->tag, "q_expr") != NULL)  { result = lval_q_expr(); }

    for (int i = 0; i < t->children_num; i++) {
      if (strcmp(t->children[i]->contents, "(") == 0) { continue; }
      if (strcmp(t->children[i]->contents, ")") == 0) { continue; }
      if (strcmp(t->children[i]->contents, "}") == 0) { continue; }
      if (strcmp(t->children[i]->contents, "{") == 0) { continue; }
      if (strcmp(t->children[i]->tag,  "regex") == 0) { continue; }
      if (strstr(t->children[i]->tag,  "comment") != NULL) { continue; }
      result = lval_add(result, parse(t->children[i]));
    }
    return result;
}
lval* parse_read_string(mpc_ast_t* t) {
    size_t tLen = strlen(t->contents);
    //Copy string, excluding quote marks
    char* unescaped = calloc(tLen, sizeof(char));
    memcpy(unescaped, t->contents+sizeof(char), (tLen-2) * sizeof(char));
    //evaluate entered escape characters. ie \n -> new line
    unescaped = mpcf_unescape(unescaped);
    
    //return new string
    lval* val = lval_str(unescaped);
    free(unescaped);
    return val;
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