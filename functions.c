#include <stdlib.h>
#include <float.h>

#include "lang.h"
#include "main.h"

void lenv_add_builtin(lenv* env, char* sym, lbuiltin func) {
    lval* symval = lval_sym(sym);
    lval* funcval = lval_builtin(func, sym);
    
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
    
    //Comparison functions
    lenv_add_builtin(env, ">", builtin_comp_gt);
    lenv_add_builtin(env, "<", builtin_comp_lt);
    lenv_add_builtin(env, ">=", builtin_comp_ge);
    lenv_add_builtin(env, "<=", builtin_comp_le);
    lenv_add_builtin(env, "==", builtin_comp_eq);
    lenv_add_builtin(env, "!=", builtin_comp_neq);
    lenv_add_builtin(env, "&&", builtin_logical_and);
    lenv_add_builtin(env, "||", builtin_logical_or);
    lenv_add_builtin(env, "!", builtin_logical_not);
    
    //List/Util functions
    lenv_add_builtin(env, "list", builtin_list);
    lenv_add_builtin(env, "eval", builtin_eval);
    lenv_add_builtin(env, "join", builtin_join);
    lenv_add_builtin(env, "head", builtin_head);
    lenv_add_builtin(env, "tail", builtin_tail);
    lenv_add_builtin(env, "if", builtin_if);
    
    //ENV Functions
    lenv_add_builtin(env, "def", builtin_def);
    lenv_add_builtin(env, "var", builtin_var);
    lenv_add_builtin(env, "listenv", builtin_listenv);
    lenv_add_builtin(env, "exit", builtin_exit);
    lenv_add_builtin(env, "lambda", builtin_lambda);
    lenv_add_builtin(env, "\\", builtin_lambda);
}

char* builtin_op_strname(BUILTIN_OP_TYPE op) {
    switch(op) {
        // Operators
        case BUILTIN_OP_ADD:    return "+";
        case BUILTIN_OP_SUB:    return "-";
        case BUILTIN_OP_MUL:    return "*";
        case BUILTIN_OP_DIV:    return "/";
        case BUILTIN_OP_POW:    return "^";
        // Comparisons
        case BUILTIN_COMP_GT:   return ">";
        case BUILTIN_COMP_LT:   return "<";
        case BUILTIN_COMP_GE:   return ">=";
        case BUILTIN_COMP_LE:   return "<=";
        case BUILTIN_COMP_EQ:   return "==";
        case BUILTIN_COMP_NEQ:  return "!=";
        //Logical Operators
        case BUILTIN_LOGICAL_AND:   return "&&";
        case BUILTIN_LOGICAL_OR:    return "||";
        case BUILTIN_LOGICAL_NOT:   return "!";
        default:                return "UNKNOWN";
    }
}

//Start Math Functions
lval* builtin_op(lenv* env, lval* val, BUILTIN_OP_TYPE op) {
    
    //Ensure numbers only
    for(int i = 0; i < val->cell_count; i++) {
        LASSERT_TYPE(builtin_op_strname(op), val, val->cell_list[i], LVAL_NUM);
    }
    
    //Get the first element
    lval* x = lval_pop(val, 0);
    
    if (op == BUILTIN_OP_SUB && val->cell_count == 0) {
        x->data.num = -x->data.num;
    }
    
    while(val->cell_count > 0) {
        //Get next to process
        lval* y = lval_pop(val, 0);
        
        switch(op) {
            case BUILTIN_OP_ADD: x->data.num += y->data.num; break;
            case BUILTIN_OP_SUB: x->data.num -= y->data.num; break;
            case BUILTIN_OP_MUL: x->data.num *= y->data.num; break;
            case BUILTIN_OP_POW: x->data.num = pow(x->data.num,y->data.num); break;
            case BUILTIN_OP_DIV: ;
                short divZero = 0;
                if (y->type == LVAL_NUM && LVAL_IS_FALSE(y)) {divZero = 1;}

                if (divZero) {
                    lval_delete(x);
                    lval_delete(y);
                    x = lval_err(LERR_DIV_ZERO);
                    break;
                } else {
                    x->data.num /= y->data.num;
                }
                break;
            default:
                lval_delete(val);
                return lval_err_detail(LERR_BAD_OP, "expected operator got %s", builtin_op_strname(op));
                break;
        }
        
        lval_delete(y);
    }
    
    lval_delete(val);
    return x;
}
lval* builtin_add(lenv* env, lval* val) {
    return builtin_op(env, val, BUILTIN_OP_ADD);
}
lval* builtin_sub(lenv* env, lval* val) {
    return builtin_op(env, val, BUILTIN_OP_SUB);
}
lval* builtin_div(lenv* env, lval* val) {
    return builtin_op(env, val, BUILTIN_OP_DIV);
}
lval* builtin_mul(lenv* env, lval* val) {
    return builtin_op(env, val, BUILTIN_OP_MUL);
}
lval* builtin_pow(lenv* env, lval* val){
    return builtin_op(env, val, BUILTIN_OP_POW);
}
//End Math Functions

//Start Comparison Functions
lval* builtin_comp_num(lenv* env, lval* val, BUILTIN_OP_TYPE op) {
    char* opName = builtin_op_strname(op);
    LASSERT_ARG_COUNT(opName, val, val, 2);
    LASSERT_TYPE(opName, val, val->cell_list[0], LVAL_NUM);
    LASSERT_TYPE(opName, val, val->cell_list[1], LVAL_NUM);
    
    int r = 0;
    switch(op) {
        case BUILTIN_COMP_GT: r = val->cell_list[0]->data.num > val->cell_list[1]->data.num; break;
        case BUILTIN_COMP_LT: r = val->cell_list[0]->data.num < val->cell_list[1]->data.num; break;
        case BUILTIN_COMP_GE: r = val->cell_list[0]->data.num >= val->cell_list[1]->data.num; break;
        case BUILTIN_COMP_LE: r = val->cell_list[0]->data.num <= val->cell_list[1]->data.num; break;
        default:
            lval_delete(val);
            return lval_err_detail(LERR_BAD_OP, "Invalid comparison got %s", builtin_op_strname(op));
            break;
    }
    lval_delete(val);
    return lval_num(r);
}
lval* builtin_comp_gt(lenv* env, lval* val) {
    return builtin_comp_num(env, val, BUILTIN_COMP_GT);
}
lval* builtin_comp_lt(lenv* env, lval* val) {
    return builtin_comp_num(env, val, BUILTIN_COMP_LT);
}
lval* builtin_comp_ge(lenv* env, lval* val) {
    return builtin_comp_num(env, val, BUILTIN_COMP_GE);
}
lval* builtin_comp_le(lenv* env, lval* val) {
    return builtin_comp_num(env, val, BUILTIN_COMP_LE);
}

lval* builtin_comp_value(lenv* env, lval* val, BUILTIN_OP_TYPE op) {
    LASSERT_ARG_COUNT(builtin_op_strname(op), val, val, 2);
    BOOL result = FALSE;
    result = lval_equal(val->cell_list[0], val->cell_list[1]);
    if (op == BUILTIN_COMP_NEQ) {
        result = !result;
    }
    return lval_num((int)result);
}
lval* builtin_comp_eq(lenv* env, lval* val) {
    return builtin_comp_value(env, val, BUILTIN_COMP_EQ);
}
lval* builtin_comp_neq(lenv* env, lval* val) {
    return builtin_comp_value(env, val, BUILTIN_COMP_NEQ);
}
lval* builtin_logical(lenv* env, lval* val, BUILTIN_OP_TYPE op) {
    int expectedArgs = op == BUILTIN_LOGICAL_NOT ? 1 : 2;
    char* opName = builtin_op_strname(op);
    
    LASSERT_ARG_COUNT(opName, val, val, expectedArgs);
    LASSERT_TYPE(opName, val, val->cell_list[0], LVAL_NUM);
    if (expectedArgs == 2) {
        LASSERT_TYPE(opName, val, val->cell_list[1], LVAL_NUM);
    }
    
    BOOL result = FALSE;
    
    switch(op) {
        case BUILTIN_LOGICAL_AND:
            if (LVAL_IS_TRUE(val->cell_list[0]) && LVAL_IS_TRUE(val->cell_list[1])) {
                result = TRUE;
            }
            break;
        case BUILTIN_LOGICAL_OR:
            if (LVAL_IS_TRUE(val->cell_list[0]) || LVAL_IS_TRUE(val->cell_list[1])) {
                result = TRUE;
            }
            break;
        case BUILTIN_LOGICAL_NOT:
            if (LVAL_IS_TRUE(val->cell_list[0])) {
                result = FALSE;
            } else {
                result = TRUE;
            }
            break;
        default:
            lval_delete(val);
            return lval_err_detail(LERR_BAD_OP, "Expected logical operator, got %s", opName);
    }
    
    lval_delete(val);
    return lval_num((int)result);
    
}
lval* builtin_logical_and(lenv* env, lval* val) {
    return builtin_logical(env, val, BUILTIN_LOGICAL_AND);
}
lval* builtin_logical_or(lenv* env, lval* val) {
    return builtin_logical(env, val, BUILTIN_LOGICAL_OR);
}
lval* builtin_logical_not(lenv* env, lval* val) {
    return builtin_logical(env, val, BUILTIN_LOGICAL_NOT);
}
//End Comparison Functions

//Start List/Util functions
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
    LASSERT_ARG_COUNT("head", val, val, 1);
    LASSERT_TYPE("head", val, val->cell_list[0], LVAL_Q_EXPR);
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
lval* builtin_if(lenv* env, lval* val) {
    LASSERT_ARG_COUNT("if", val, val, 3);
    LASSERT_TYPE("if", val, val->cell_list[0], LVAL_NUM);
    LASSERT_TYPE("if", val, val->cell_list[1], LVAL_Q_EXPR);
    LASSERT_TYPE("if", val, val->cell_list[2], LVAL_Q_EXPR);
    
    lval* result = NULL;
    
    val->cell_list[1]->type = LVAL_S_EXPR;
    val->cell_list[2]->type = LVAL_S_EXPR;
    
    if (LVAL_IS_TRUE(val->cell_list[0])) {
        result = eval(env, lval_pop(val, 1));
    } else {
        result = eval(env, lval_pop(val, 2));
    }
    lval_delete(val);
    return result;
    
}
//End List/Util functions

//Start ENV Functions
lval* builtin_envdef(lenv* env, lval* val, char* type){
    LASSERT_MIN_ARG_COUNT(type, val, val, 1);
    LASSERT_TYPE(type, val, val->cell_list[0], LVAL_Q_EXPR);
    
    lval* symbols = val->cell_list[0];
    
    for(int i = 0; i < symbols->cell_count; i++) {
        LASSERT_TYPE(type, val, symbols->cell_list[i], LVAL_SYM);
    }
    
    LASSERT(val, symbols->cell_count == val->cell_count -1,
            LERR_OTHER, "%s: incorrect number of definitions for symbols", type);
    
    for(int i = 0; i < symbols->cell_count; i++) {
        if (strcmp(type, "def") == 0) {
            lenv_def(env, symbols->cell_list[i], val->cell_list[i+1]);
        } else if (strcmp(type, "var") == 0) {
            lenv_put(env, symbols->cell_list[i], val->cell_list[i+1]);
        }
    }
    
    lval_delete(val);
    return lval_s_expr();
}
lval* builtin_var(lenv* env, lval* val) {
    return builtin_envdef(env, val, "var");
}
lval* builtin_def(lenv* env, lval* val) {
    return builtin_envdef(env, val, "def");
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

lval* builtin_lambda(lenv* env, lval* val) {
    LASSERT_ARG_COUNT("lambda", val, val, 2);
    LASSERT_TYPE("lambda", val, val->cell_list[0], LVAL_Q_EXPR);
    LASSERT_TYPE("lambda", val, val->cell_list[1], LVAL_Q_EXPR);
    
    lval* symbols = val->cell_list[0];
    lval* va = NULL;
    
    for(int i = 0; i < symbols->cell_count; i++) {
        LASSERT_TYPE("lambda args", val, symbols->cell_list[i], LVAL_SYM);
        if (strcmp("&", symbols->cell_list[i]->data.sym) == 0 && i+1 == symbols->cell_count) {
            va = lval_pop(symbols, i);
            break;
        }
    }
    
    lval* formals = lval_pop(val, 0);
    lval* body = lval_pop(val, 0);
    body->type = LVAL_S_EXPR;
    
    lval* lambda = lval_lambda(formals, body);
    lambda->data.func->va = va;
    lval_delete(val);
    return lambda;
}
//End ENV Functions