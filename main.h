/* 
 * File:   main.h
 * Author: sam
 *
 * Created on 17 May 2014, 16:15
 */

#ifndef MAIN_H
#define	MAIN_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#define VERSION "0.1"

int main(int argc, char** argv);

void lval_expr_print(lval* val, char* open, char* close);
void lval_print(lval* val);
void lval_println(lval* val);

#ifdef	__cplusplus
}
#endif

#endif	/* MAIN_H */

