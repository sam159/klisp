/* 
 * File:   main.c
 * Author: sam
 *
 * Created on 17 May 2014, 11:49
 */

#include <stdio.h>
#include <stdlib.h>

#include <editline/readline.h>

#include "mpc.h"
#include "lang.h"
#include "main.h"
#include "util.h"

/*
 * 
 */
int main(int argc, char** argv) {
    
    printf("KLisp Version %s\n", VERSION);
    
    //Init environment
    lenv* env = lenv_new();
    lenv_add_builtin_funcs(env);
    setup_parsers();
    
    //Attempt to import/run files specified on the command line
    if (argc > 1) {
        for(int i = 1; i < argc; i++) {
            lval* loadargs = lval_add(lval_s_expr(), lval_str(argv[i]));
            
            lval* result = builtin_load(env, loadargs);
            
            if (result->type == LVAL_ERR) {
                lval_println(result);
            }
            
            lval_delete(result);
        }
    }
    
    int exitcode = EXIT_SUCCESS;
    
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
            
            BOOL exit = FALSE;
            if (result != NULL && result->type == LVAL_EXIT) {
                exit = TRUE;
                exitcode = result->data.exitcode;
            } else {
                //print the result
                lval_println(result);
            }
            
            //Cleanup
            lval_delete(result);
            mpc_ast_delete(ast_result);
            
            if (exit == TRUE) {;
                break;
            }
        }
        
    }
    
    lenv_delete(env);
    cleanup_parsers();
    
    return (exitcode);
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
        case LVAL_STR: lval_print_str(val); break;
        case LVAL_S_EXPR: lval_expr_print(val, "(", ")"); break;
        case LVAL_Q_EXPR: lval_expr_print(val, "{", "}"); break;
        case LVAL_EXIT: printf("exit"); break;
        case LVAL_OK: printf("ok"); break;
        case LVAL_FUNC: ;
            lval_func* func = val->data.func;
            if (func->builtin != NULL) {
                printf("<%s>", func->name);
            } else {
                printf("(<lambda> ");
                lval_print(func->formals);
                putchar(' ');
                lval_print(func->body);
                putchar(')');
            }
            break;
        case LVAL_ERR:
            switch(val->data.err.num) {
                case LERR_DIV_ZERO: fprintf(stderr, "Divide By Zero"); break;
                case LERR_BAD_NUM: fprintf(stderr,"Bad Number"); break;
                case LERR_BAD_OP: fprintf(stderr,"Invalid Operator"); break;
                case LERR_BAD_SYM: fprintf(stderr,"Unknown/Invalid Symbol"); break;
                case LERR_OTHER: fprintf(stderr,"Unknown/Other Error"); break;
                case LERR_SYNTAX: fprintf(stderr,"Syntax Error"); break;
                case LERR_USER: fprintf(stderr,"Runtime Error"); break;
                default: fprintf(stderr,"Unknown Error"); break;
            }
            if (val->data.err.detail != NULL) {
                fprintf(stderr,": %s", val->data.err.detail);
            }
            break;
    }
}

void lval_println(lval* val) {
    lval_print(val);
    putchar('\n');
}

void lval_print_str(lval* val) {
    char* escaped = strdup(val->data.str);
    escaped = mpcf_escape(escaped);
    printf("\"%s\"", escaped);
    free(escaped);
}