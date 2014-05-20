/* 
 * File:   main.c
 * Author: sam
 *
 * Created on 17 May 2014, 11:49
 */

#include <stdio.h>
#include <stdlib.h>

#include <editline/readline.h>
#include <editline/history.h>

#include "mpc.h"
#include "lang.h"
#include "main.h"

/*
 * 
 */
int main(int argc, char** argv) {
    
    printf("KLisp Version %s\n", VERSION);
    
    //Init environment
    lenv* env = lenv_new();
    lenv_add_builtin_funcs(env);
    
    while(1) {
        char *input = readline("> ");
        if (NULL == input) {
            break;
        }
        add_history(input);
        
        mpc_ast_t* ast_result = tokenize(input);
        
        free(input);
        
        if (ast_result != NULL) {
            
            //Parse the ast
            lval* result = parse(ast_result);
            if (result == NULL) {
                result = lval_err(LERR_OTHER);
            }
            
            //Evaluate
            result = eval(env, result);
            
            int exit = 0;
            if (result != NULL && result->type == LVAL_EXIT) {
                exit = 1;
            } else {
                //print the result
                lval_println(result);
            }
            
            //Cleanup
            lval_delete(result);
            mpc_ast_delete(ast_result);
            
            if (exit == 1) {
                printf("Program Terminated: ");
                
                lval* sym = lval_sym("exitcode");
                lval* exitcode = lenv_get(env, sym);
                lval_println(exitcode);
                lval_delete(exitcode);
                lval_delete(sym);
                break;
            }
        }
        
    }
    
    lenv_delete(env);
    
    return (EXIT_SUCCESS);
}

void lval_expr_print(lval* val, char* open, char* close) {
    printf("%s", open);
    
    for(int i = 0; i < val->cell_count ; i++) {
        lval_print(val->cell_list[i]);
        
        if (i != val->cell_count-1) {
            putchar(' ');
        }
    }
    
    printf("%s", close);
}

void lval_print(lval* val) {
    switch(val->type) {
        case LVAL_NUM: printf("%g", val->data.num); break;
        case LVAL_SYM: printf("%s", val->data.sym); break;
        case LVAL_S_EXPR: lval_expr_print(val, "(", ")"); break;
        case LVAL_Q_EXPR: lval_expr_print(val, "{", "}"); break;
        case LVAL_FUNC: printf("<%s>", val->data.func.name); break;
        case LVAL_EXIT: printf("exit"); break;
        case LVAL_ERR:
            printf("Error: ");
            switch(val->data.err.num) {
                case LERR_DIV_ZERO: printf("Divide By Zero"); break;
                case LERR_BAD_NUM: printf("Bad Number"); break;
                case LERR_BAD_OP: printf("Invalid Operator"); break;
                case LERR_BAD_SYM: printf("Unknown/Invalid Symbol"); break;
                case LERR_OTHER: printf("Unknown Error"); break;
                case LERR_SYNTAX: printf("Syntax Error"); break;
                default: printf("Unknown Error"); break;
            }
            if (val->data.err.detail != NULL) {
                printf(", %s", val->data.err.detail);
            }
            break;
    }
}
void lval_println(lval* val) {
    lval_print(val);
    putchar('\n');
}