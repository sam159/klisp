/* 
 * File:   shared.h
 * Author: sam
 *
 * Created on 31 May 2014, 15:50
 */

#ifndef SHARED_H
#define	SHARED_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include "mpc.h"
    
    extern mpc_parser_t* gLispy;
    extern size_t gParserCount;
    extern mpc_parser_t** gParsers;

#ifdef	__cplusplus
}
#endif

#endif	/* SHARED_H */

