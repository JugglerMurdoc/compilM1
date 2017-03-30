/* Thibaut Ehlinger
 * Stan Wilhelm
 */

%option noinput 
%option nounput

%{
 
#include "int_list.h"
#include "quad.h"
#include "symbol.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "y.tab.h"

%}

comment "/*"([^*]|(\*+[^*/]))*"*"+"/"
identifier [a-zA-Z_][a-zA-Z0-9_]*
integer [1-9][0-9]*|0
floating [0-9]*\.[0-9]+|[0-9]+
string_delimiters \"(\\.|[^"])*\"

%%

{comment} { }

"//".* { }
    
{string_delimiters}	{
		yylval.string = strdup(yytext);
		printf("\n");
        return STRING;
}

{integer} {
        yylval.string = strdup(yytext);
        return INTEGER;
}
    
{floating} {
		yylval.string = strdup(yytext);
        return FLOATING;
}

"main" {
        return MAIN;
}

"if" {
        return IF;
}

"else" {
        return ELSE;
}

"for" {
        return FOR;
}

"while" {
        return WHILE;
}

"print" {
        return PRINT;
}

"printf" {
        return PRINTF;
}

"printmat" {
        return PRINTMAT;
}

"int"|"matrix" {
        return INTEGER_TYPE;
}
    
"float" {
        return FLOATING_TYPE;
}

"<=" {
        return LE;
}

"<" {
        return LT;
}

">=" {
        return GE;
}

">" {
        return GT;
}

"==" {
        return EQ;
}

"!=" {
        return NE;
}

"!" {
        return NOT;
}

"&&" {
        return AND;
}

"||" {
        return OR;
}

{identifier} {
        yylval.string = strdup(yytext);
        return ID;
}

[(){}\[\],=;+\-*/] {
        return yytext[0];
}

[\n\t ] {
}

. {
}

%%
