#ifndef __MATC_H__
#define __MATC_H__

#include "quad.h"

int yyparse(void);
int yylex_destroy(void);
extern FILE *yyin;

QuadList *quadList = NULL;
SymbolTable *symbolTable = NULL;

#endif
