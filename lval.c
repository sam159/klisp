#include <stdlib.h>

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

lval* lval_func(lbuiltin func, char* name) {
    lval* val = lval_new(LVAL_FUNC);
    val->data.func.call = func;
    val->data.func.name = strdup(name);
    return val;
}
lval* lval_exit() {
    lval* val = lval_new(LVAL_EXIT);
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

void lval_delete(lval* val) {
    switch(val->type) {
        case LVAL_NUM: break;
        case LVAL_EXIT: break;
        case LVAL_FUNC: free(val->data.func.name); break;
        
        case LVAL_SYM: free(val->data.sym); break;
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
            new->data.func.call = current->data.func.call; 
            new->data.func.name = strdup(current->data.func.name);
            break;
        case LVAL_NUM: new->data.num = current->data.num; break;
        case LVAL_EXIT: break;
        
        case LVAL_SYM: new->data.sym = strdup(current->data.sym); break;
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
        case LVAL_ERR: return "Error";
        case LVAL_FUNC: return "Function";
        case LVAL_NUM: return "Numeric";
        case LVAL_Q_EXPR: return "Q-Expression";
        case LVAL_SYM: return "Symbol";
        case LVAL_S_EXPR: return "S-Expression";
        default: return "UNKNOWN";
    }
}