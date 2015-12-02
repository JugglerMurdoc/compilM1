%{
	#include <stdio.h>
	#include <stdlib.h>
	#include "y.tab.h"
%}

ASSIGN :=
WHILE "while"
IF "if"
THEN "then"
DO "do"
DONE "done"
ENDIF "endif"
ELSE "else"

ID [a-zA-Z_][0-9a-zA-Z_]*
ENTIER [0-9]+

%%
{ASSIGN} { return ASSIGN; }
{WHILE} { return WHILE; }
{IF} { return IF; }
{THEN} { return THEN; }
{DO} { return DO; }
{DONE} { return DONE; }
{ENDIF} { return ENDIF; }
{ELSE} { return ELSE; }
{ID} { return ID; }
{ENTIER} { yylval.valeur = atoi(yytext); return ENTIER; }
%%
