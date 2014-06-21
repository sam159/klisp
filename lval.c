#include <stdlib.h>
#include <float.h>

#include "lang.h"
#include "util.h"

lval* lval_new(int type) {
    lval* val = calloc(1,sizeof(lval));
    val->type = type;
    return val;
}

lval* lval_num(double_t x) {
    lval* val = lval_new(LVAL_NUM);
    val->data.num = x;
    return val;
}

lval* lval_sym(char* x) {
    lval* val = lval_new(LVAL_SYM);
    val->data.sym = strdup(x);
    return val;
}

lval* lval_s_expr() {
    lval* val = lval_new(LVAL_S_EXPR);
    val->cell_count = 0;
    val->cell_list = NULL;
    return val;
}

lval* lval_q_expr() {
    lval* val = lval_new(LVAL_Q_EXPR);
    val->cell_count = 0;
    val->cell_list = NULL;
    return val;
}

lval* lval_builtin(lbuiltin func, char* name) {
    lval* val = lval_new(LVAL_FUNC);
    val->data.func = calloc(1, sizeof(lval_func));
    val->data.func->builtin = func;
    val->data.func->name = strdup(name);
    return val;
}
lval* lval_lambda(lval* formals, lval* body) {
    lval* val = lval_new(LVAL_FUNC);
    val->data.func = calloc(1, sizeof(lval_func));
    val->data.func->builtin = NULL;
    val->data.func->name = NULL;
    val->data.func->env = lenv_new();
    val->data.func->formals = formals;
    val->data.func->body = body;
    return val;
}
lval* lval_exit(short exitcode) {
    lval* val = lval_new(LVAL_EXIT);
    val->data.exitcode = exitcode;
    return val;
}
lval* lval_str(char* str) {
    lval* val = lval_new(LVAL_STR);
    val->data.str = strdup(str);
    return val;
}
lval* lval_ok() {
    lval* val = lval_new(LVAL_OK);
    return val;
}

lval* lval_add(lval* val, lval* x) {
    val->cell_count++;
    val->cell_list = realloc(val->cell_list, sizeof(lval*)*val->cell_count);
    val->cell_list[val->cell_count-1] = x;
    return val;
}

lval* lval_pop(lval* val, int index) {
    //Get the item
    lval* x = val->cell_list[index];
    
    //Re-create the list ignoring the index we are extracting
    lval** newList = NULL;
    if (val->cell_count-1 > 0) {
        newList = calloc(val->cell_count-1, sizeof(lval*));
        int k = 0;
        for(int i=0; i < val->cell_count; i++) {
            if (i == index) {
                continue;
            }
            newList[k++] = val->cell_list[i];
        }
    }
    
    val->cell_count--;
    free(val->cell_list);
    val->cell_list = newList;
    
    return x;
}

lval* lval_take(lval* val, int i) {
    lval* x = lval_pop(val, i);
    lval_delete(val);
    return x;
}

lval* lval_join(lval* a, lval* b) {
    while(b->cell_count > 0) {
        lval_add(a, lval_pop(b,0));
    }
    lval_delete(b);
    return a;
}

lval* lval_call(lenv* env, lval* function, lval* args) {
    lval_func* func = function->data.func;
    
    if (func->builtin != NULL) {
        return func->builtin(env, args);
    }
    
    //Check arg counts
    LASSERT(args, func->formals->cell_count <= args->cell_count, LERR_SYNTAX,
            "lambda: insufficient arguments. Expected %ld got %ld", func->formals->cell_count, args->cell_count);
    
    for(int i = 0; i < func->formals->cell_count; i++) {
        lenv_put(func->env, func->formals->cell_list[i], args->cell_list[i]);
    }
    
    if (func->va != NULL) {
        lval* vaArgs = lval_q_expr();
        
        for(int i = func->formals->cell_count; i < args->cell_count; i ++ ) {
            lval_add(vaArgs, lval_copy(args->cell_list[i]));
        }
        
        lenv_put(func->env, func->va, vaArgs);
        lval_delete(vaArgs);
    }
    
    lval_delete(args);
    
    func->env->parent = env;
    
    return eval(func->env, lval_add(lval_s_expr(), lval_copy(func->body)));
}

BOOL lval_equal(lval* a, lval* b) {
    if (a->type != b->type) {
        return FALSE;
    }
    
    switch(a->type) {
        case LVAL_ERR:  return a->data.err.num == b->data.err.num;
        case LVAL_NUM:  return fabs(a->data.num - b->data.num) <= DBL_EPSILON;
        case LVAL_SYM:  return strcmp(a->data.sym, b->data.sym) == 0;
        case LVAL_STR:  return strcmp(a->data.str, b->data.str) == 0;
        case LVAL_OK:
        case LVAL_EXIT:
            return TRUE;
        case LVAL_FUNC:
            if (a->data.func->builtin != NULL) {
                if (b->data.func->builtin != NULL) {
                    return a->data.func->builtin == a->data.func->builtin;
                } else {
                    return FALSE;
                }
            } else {
                if (b->data.func->builtin == NULL) {
                    return FALSE;
                } else {
                    return lval_equal(b->data.func->formals, b->data.func->formals)
                        && lval_equal(b->data.func->body, b->data.func->body);
                }
            }
        case LVAL_Q_EXPR:
        case LVAL_S_EXPR:
            if (a->cell_count != b->cell_count) { return 0; }
            for (int i = 0; i < a->cell_count; i++) {
                if (!lval_equal(a->cell_list[i], b->cell_list[i])) {
                    return FALSE;
                }
            }
            return TRUE;
            
        default: return FALSE;
    }
}

void lval_delete(lval* val) {
    switch(val->type) {
        case LVAL_NUM: break;
        case LVAL_EXIT: break;
        case LVAL_OK: break;
        case LVAL_FUNC: 
            if (val->data.func != NULL) {
                if (val->data.func->builtin == NULL) {
                    lenv_delete(val->data.func->env);
                    lval_delete(val->data.func->formals);
                    lval_delete(val->data.func->body);
                } else {
                    free(val->data.func->name);
                }
            }
            break;
        
        case LVAL_SYM: free(val->data.sym); break;
        case LVAL_STR: free(val->data.str); break;
        case LVAL_ERR: 
            if (val->data.err.detail != NULL) {
                free(val->data.err.detail);
            }
            break;
        case LVAL_Q_EXPR:
        case LVAL_S_EXPR:
            for (int i = 0; i < val->cell_count; i++) {
                lval_delete(val->cell_list[i]);
            }
            if (val->cell_count > 0) {
                free(val->cell_list);
            }
            break;
    }
    free(val);
}

lval* lval_copy(lval* current) {
    lval* new = lval_new(current->type);
    
    switch(current->type) {
        case LVAL_FUNC:
            new->data.func = calloc(1, sizeof(lval_func));
            lval_func* funcNew = new->data.func;
            lval_func* funcCurrent = current->data.func;
            
            if (funcCurrent->builtin == NULL) {
                funcNew->env = lenv_copy(funcCurrent->env);
                funcNew->body = lval_copy(funcCurrent->body);
                funcNew->formals = lval_copy(funcCurrent->formals);
            } else {
                funcNew->builtin = funcCurrent->builtin;
                funcNew->name = strdup(funcCurrent->name);
            }
            break;
        case LVAL_NUM: new->data.num = current->data.num; break;
        case LVAL_EXIT: break;
        case LVAL_OK: break;
        
        case LVAL_SYM: new->data.sym = strdup(current->data.sym); break;
        case LVAL_STR: new->data.str = strdup(current->data.str); break;
        case LVAL_ERR:
            new->data.err.num = current->data.err.num;
            new->data.err.detail = current->data.err.detail == NULL ? NULL : strdup(current->data.err.detail); 
            break;
        
        case LVAL_Q_EXPR:
        case LVAL_S_EXPR:
            new->cell_count = current->cell_count;
            if (new->cell_count == 0) {
                new->cell_list = NULL;
            } else {
                new->cell_list = calloc(new->cell_count, sizeof(lval*));
                for(int i=0; i < new->cell_count; i++) {
                    new->cell_list[i] = lval_copy(current->cell_list[i]);
                }
            }
            break;
    }
    
    return new;
}

lval* lval_err(VAL_ERROR err){
    return lval_err_detail(err, NULL);
}

lval* lval_err_detail(VAL_ERROR err, char* format, ...){
    lval* val = lval_new(LVAL_ERR);
    val->data.err.num = err;
    
    va_list va;
    va_start(va, format);
    
    val->data.err.detail = calloc(512, sizeof(char));
    
    vsnprintf(val->data.err.detail, 511, format, va);
    
    if (strlen(val->data.err.detail) == 0) {
        free(val->data.err.detail);
        val->data.err.detail = NULL;
    } else {
        val->data.err.detail = realloc(val->data.err.detail, strlen(val->data.err.detail)+1);
    }
    
    va_end(va);
    
    return val;
}

char* lval_str_name(VAL_TYPE type) {
    switch(type) {
        case LVAL_FUNC: return "Function";
        case LVAL_NUM: return "Numeric";
        case LVAL_STR: return "String";
        case LVAL_SYM: return "Symbol";
        case LVAL_Q_EXPR: return "Q-Expression";
        case LVAL_S_EXPR: return "S-Expression";
        case LVAL_EXIT: return "Exit";
        case LVAL_ERR: return "Error";
        case LVAL_OK: return "Ok/Success";
        default: return "UNKNOWN";
    }
}