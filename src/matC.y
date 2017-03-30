%{
/* Thibaut Ehlinger
 * Stan Wilhelm
 */
 
#include <stdio.h>
#include "quad.h"
#include "symbol.h"
#include "int_list.h"

int yylex();
void yyerror(const char *s) 
{
    fprintf(stderr, "%s\n", s);
}

extern QuadList *quadList;
extern SymbolTable *symbolTable;
extern FILE *yyin;

%}

%token IF ELSE FOR WHILE MAIN	
%token <string> ID 
%token INTEGER_TYPE FLOATING_TYPE
       
%token <string> INTEGER FLOATING STRING
%token LE LT GE GT EQ NE NOT AND OR
%token PRINT PRINTF PRINTMAT

       
%left OR AND EQ NE LE LT GE GT
%left '+' '-' '*' '/' ','
%right '='
%right NOT

%union{
    char* string;
    int next_label;
    Symbol *symbol;
    struct{
        QuadList *true_list;
        QuadList *false_list;
    } condition;
    QuadList *next;
    SymbolType declaration_type;
    struct{
        Symbol* symbol;
        Symbol* depl;
    } address_access;
    struct{
        Symbol* array;
        int nbdim;
        Symbol* depl;
    } array_access;
    array_dimension* dim;
    struct {
        intlist* int_list;
        int depth;
    } array_decl;
}

%type <next_label> nextQuad
%type <next> statement_list statement goTo
%type <symbol> expr
%type <condition> cond
%type <declaration_type> var_declaration
%type <address_access> address assign
%type <array_access> array
%type <dim> array_dims
%type <array_decl> init_list array_init

%%

       
main : INTEGER_TYPE MAIN '(' ')' '{' axiom '}' 
		{}

axiom
    : 
    | nextQuad statement_list
        {
            complete($2, nextQuad());
            Quad *end = gencodeEnd();
            quadList = addQuad(quadList, end);
        }
    ;
       
statement_list
    : 
     statement
        {
            $$ = $1;
        }
    | statement nextQuad statement_list
        {
            complete($1, $2);
            $$ = $3;
        }
    | '{' statement_list '}'
        {
            $$ = $2;
        }
    ;

statement
    : assign_list ';'
        {
            $$ = NULL;
        }
        
    | PRINT '(' expr ')' ';'
        {
            Quad* printQuad = gencodeUnary(print, $3);
            quadList = addQuad(quadList, printQuad);
            $$ = NULL;
        }
        
    | PRINTF '(' STRING ')' ';'
        {
			Symbol* n_string = newString($3);
			symbolTable = addSymbol(symbolTable, n_string);
			Quad* printQuad = gencodeUnary(prints, n_string);
            quadList = addQuad(quadList, printQuad);
            $$ = NULL;    
        }
       
    | PRINTMAT '(' expr ')' ';'
        {
            Quad* printQuad = gencodeUnary(printmat, $3);
            quadList = addQuad(quadList, printQuad);
            $$ = NULL;
        }
        
    | parameter_declaration ';'
        {
            $$ = NULL;
        }
        
    | WHILE nextQuad '(' cond ')' '{' nextQuad statement_list '}' goTo
        {
            $$ = $4.false_list;
            complete($4.true_list, $7);
            complete($8, $2);
            complete($10, $2);
        }
        
    | FOR '(' assign_list ';' nextQuad cond ';' nextQuad assign_list goTo ')' nextQuad '{' statement_list '}' goTo
        {
            $$ = $6.false_list;
            complete($6.true_list, $12);
            complete($10, $5);
            complete($16, $8);
            complete($14, $8);
        }
        
    | IF '(' cond ')' nextQuad '{' statement_list '}'
        {
            $$ = concat($3.false_list, $7);
            complete($3.true_list, $5);
        }
        
    | IF '(' cond ')' nextQuad '{' statement_list '}' goTo ELSE nextQuad '{' statement_list '}'
        {
            $$ = concat(concat($7, $9), $13);
            complete($3.true_list, $5);
            complete($3.false_list, $11);
        }
    ;

assign_list
    : assign
    | assign ',' assign_list
    ;

assign
    : address '=' expr
        {
            if($1.depl == NULL)
			{
                Quad *affect = gencodeBinary(assign, $3, $1.symbol);
                quadList = addQuad(quadList, affect);
                $$.symbol = $1.symbol;
                $$.depl = NULL;
            }
            else
            {
                Symbol* addr_ptr = newTemp(integer);
                symbolTable = addSymbol(symbolTable, addr_ptr);
                Quad *add_ptr = gencodeTernary(add, $1.symbol, $1.depl, addr_ptr);
                quadList = addQuad(quadList, add_ptr);
                Quad *store_ptr = gencodeBinary(store_addr, $3, addr_ptr);
                quadList = addQuad(quadList, store_ptr);
                $$.symbol = $1.symbol;
                $$.depl = $1.depl;
            }
        }
    | address '=' assign
        {
            if($3.depl == NULL)
			{
                if($1.depl == NULL)
				{
                    Quad *affect = gencodeBinary(assign, $3.symbol, $1.symbol);
                    quadList = addQuad(quadList, affect);
                    $$.symbol = $1.symbol;
                    $$.depl = NULL;
                }
                else
                {
                    Symbol* addr_ptr = newTemp(integer);
                    symbolTable = addSymbol(symbolTable, addr_ptr);
                    Quad *add_ptr = gencodeTernary(add, $1.symbol, $1.depl,
                    addr_ptr);
                    quadList = addQuad(quadList, add_ptr);
                    Quad *store_ptr = gencodeBinary(store_addr, $3.symbol, addr_ptr);
                    quadList = addQuad(quadList, store_ptr);
                    $$.symbol = $1.symbol;
                    $$.depl = $1.depl;
                }
            }
            else
            {
                if($1.depl == NULL)
				{
                    Symbol* addr_ptr = newTemp(integer);
                    symbolTable = addSymbol(symbolTable, addr_ptr);
                    Quad *add_ptr = gencodeTernary(add, $3.symbol, $3.depl,
                    addr_ptr);
                    quadList = addQuad(quadList, add_ptr);
                    Symbol* ptr_val = newTemp(integer);
                    symbolTable = addSymbol(symbolTable, ptr_val);
                    Quad *val_ptr = gencodeBinary(load_addr, addr_ptr, ptr_val);
                    quadList = addQuad(quadList, val_ptr);
                    Quad *affect = gencodeBinary(assign, ptr_val, $1.symbol);
                    quadList = addQuad(quadList, affect);
                    $$.symbol = $1.symbol;
                    $$.depl = NULL;
                }
                else{
                    Symbol* addr_ptr1 = newTemp(integer);
                    symbolTable = addSymbol(symbolTable, addr_ptr1);
                    Quad *add_ptr = gencodeTernary(add, $1.symbol, $1.depl,
                    addr_ptr1);
                    quadList = addQuad(quadList, add_ptr);
                    Symbol* addr_ptr2 = newTemp(integer);
                    symbolTable = addSymbol(symbolTable, addr_ptr2);
                    Quad *add_ptr2 = gencodeTernary(add, $3.symbol, $3.depl, addr_ptr2);
                    quadList = addQuad(quadList, add_ptr2);
                    Quad *sandl = gencodeBinary(load_and_store_addr, addr_ptr2, addr_ptr1);
                    quadList = addQuad(quadList, sandl);
                    $$.symbol = $1.symbol;
                    $$.depl = $1.depl;
                }
            }
        }
    ;

address
    : ID
        {
            Symbol *id;
            id = searchSymbol(symbolTable, $1);
            if(id == NULL)
	{
                fprintf(stderr, "Error, %s not declared but used\n", $1);
                free($1);
                return EXIT_FAILURE;
            }
            free($1);
            $$.symbol = id;
            $$.depl = NULL;
        }
    | array ']'
        {
            Symbol *four = searchSymbol(symbolTable, "4");
            if(four == NULL)
	{
                four = newInteger(strdup("4"));
                symbolTable = addSymbol(symbolTable, four);
            }

            Symbol *tab_addr = newTemp(integer);
            symbolTable = addSymbol(symbolTable, tab_addr);
            Quad *toutterrain = gencodeBinary(assign_addr, $1.array, tab_addr);
            quadList = addQuad(quadList, toutterrain);
            $$.symbol = tab_addr;

            Symbol *depl = newTemp(integer);
            symbolTable = addSymbol(symbolTable, depl);
            Quad * depl_Quad = gencodeTernary(multiply, four, $1.depl, depl);
            quadList = addQuad(quadList, depl_Quad);
            $$.depl = depl;
        }
    ;

array
    : ID '[' expr
        {
            Symbol* id;
            id = searchSymbol(symbolTable, $1);
            if(id == NULL)
	{
                fprintf(stderr, "Error, %s not declared but used\n", $1);
                free($1);
                return EXIT_FAILURE;
            }
            if(id->type != array)
	{
                fprintf(stderr, "Error, %s is not an array\n", $1);
                free($1);
                return EXIT_FAILURE;
            }
            free($1);
            $$.array = id;
            $$.nbdim = 1;
            $$.depl = $3;
        }
    | array ']' '[' expr
        {
            $$.array = $1.array;
            $$.nbdim = $1.nbdim + 1;

            int array_size = getNthDimension($$.array, $$.nbdim);
            char* array_size_char = itoa(array_size);
            Symbol *size_array = searchSymbol(symbolTable, array_size_char);
            if(size_array == NULL)
	{
                size_array = newInteger(array_size_char);
                symbolTable = addSymbol(symbolTable, size_array);
            }
            else{
                free(array_size_char);
            }

            Symbol *temp = newTemp(integer);
            symbolTable = addSymbol(symbolTable, temp);
            Symbol *retval = newTemp(integer);
            symbolTable = addSymbol(symbolTable, retval);
            Quad *tempmul = gencodeTernary(multiply, size_array, $1.depl, temp);
            quadList = addQuad(quadList, tempmul);
            Quad *depl = gencodeTernary(add, temp, $4, retval);
            quadList = addQuad(quadList, depl);
            $$.depl = retval;
        }
    ;

parameter_declaration
    : var_declaration
    | array_declaration
    ;

var_declaration
    : INTEGER_TYPE ID
        {
            Symbol* id;
            id = searchSymbol(symbolTable, $2);
            if(id == NULL)
	{
                id = newId($2, integer);
                symbolTable = addSymbol(symbolTable, id);
            }
            else{
                free($2);
            }
            $$ = integer;
        }
    |FLOATING_TYPE ID
        {
            Symbol* id;
            id = searchSymbol(symbolTable, $2);
            if(id == NULL)
	{
                id = newId($2, floating);
                symbolTable = addSymbol(symbolTable, id);
            }
            else{
                free($2);
            }
            $$ = floating;
        }
    | var_declaration ',' ID
        {
            Symbol* id;
            id = searchSymbol(symbolTable, $3);
            if(id == NULL)
	{
                id = newId($3, $1);
                symbolTable = addSymbol(symbolTable, id);
            }
            else{
                free($3);
            }
            $$ = $1;
        }
    ;

array_declaration
    : INTEGER_TYPE ID array_dims ']'
        {
            Symbol* symbol = searchSymbol(symbolTable, $2);
            if(symbol != NULL)
	{
                fprintf(stderr, "Error, redefinition of %s\n", $2);
                free($2);
                return EXIT_FAILURE;
            }
            Symbol* new_tab = newArray($2, $3);
            symbolTable = addSymbol(symbolTable, new_tab);
        }
    | INTEGER_TYPE ID array_dims ']' '=' '{' init_list '}'
        {
            Symbol* symbol = searchSymbol(symbolTable, $2);
            if(symbol != NULL)
	{
                fprintf(stderr, "Error, redefinition of %s\n", $2);
                free($2);
                delete_intlist($7.int_list);
                return EXIT_FAILURE;
            }
            Symbol* new_tab = newArray($2, $3);
            symbolTable = addSymbol(symbolTable, new_tab);
            int size = size_intlist($7.int_list);
            if(size != new_tab->arraySize)
	{
                fprintf(stderr, "Error, array initialization with bad size\n");
                delete_intlist($7.int_list);
                return EXIT_FAILURE;
            }
            new_tab->array_value = malloc(size*sizeof(int));
            int i;
            for(i = 0; i < size; i++)
	{
                new_tab->array_value[i] = get_intlist_element($7.int_list, i);
            }
            delete_intlist($7.int_list);
        }
    ;

init_list
    : array_init
        {
            $$.int_list = $1.int_list;
            $$.depth = $1.depth;
        }
    | init_list ',' array_init
        {
            $$.int_list = add_intlist($1.int_list, $3.int_list);
            $$.depth = $1.depth > $3.depth ? $1.depth : $3.depth;
        }
    ;

array_init
    : INTEGER
        {
            $$.int_list = new_intlist(atoi($1));
            free($1);
            $$.depth = 0;
        }
    | '-' INTEGER
        {
            $$.int_list = new_intlist(-atoi($2));
            free($2);
            $$.depth = 0;
        }
    | '{' init_list '}'
        {
            $$.int_list = $2.int_list;
            $$.depth = $2.depth + 1;
        }
    ;

array_dims
    : '[' INTEGER
        {
            if(atoi($2) <= 0)
	{
                free($2);
                fprintf(stderr, "Array dimension must be >= 1\n");
                return EXIT_FAILURE;
            }
            $$ = new_dimension(atoi($2));
            free($2);
        }
    | array_dims ']' '[' INTEGER
        {
            if(atoi($4) <= 0)
	{
                free($4);
                fprintf(stderr, "Array dimension must be >= 1\n");
                return EXIT_FAILURE;
            }
            $$ = new_dimension(atoi($4));
            $$ = add_dimension($1, $$);
            free($4);
        }
    ;

expr
    : address
        {
            if($1.depl == NULL)
	{
                $$ = $1.symbol;
            }
            else{
                Symbol* addr_ptr = newTemp(integer);
                symbolTable = addSymbol(symbolTable, addr_ptr);
                Quad *add_ptr = gencodeTernary(add, $1.symbol, $1.depl, addr_ptr);
                quadList = addQuad(quadList, add_ptr);
                Symbol* val = newTemp(integer);
                symbolTable = addSymbol(symbolTable, val);
                Quad *valQuad = gencodeBinary(load_addr, addr_ptr, val);
                quadList = addQuad(quadList, valQuad);
                $$ = val;
            }
        }
    | INTEGER
        {
            Symbol *id;
            id = searchSymbol(symbolTable, $1);
            if(id == NULL)
	{
                $$ = newInteger($1);
                symbolTable = addSymbol(symbolTable, $$);
            }
            else{
                $$ = id;
                free($1);
            }
        }
    | FLOATING
        {
			Symbol *id;
            id = searchSymbol(symbolTable, $1);
            if(id == NULL)
	{
                $$ = newFloating($1);
                symbolTable = addSymbol(symbolTable, $$);
            }
            else{
                $$ = id;
                free($1);
            }
        }
    | '-' expr
        {
            Symbol *retval = newTemp($2->type);
            symbolTable = addSymbol(symbolTable, retval);
            Quad *affect = gencodeBinary(negative, $2, retval);
            quadList = addQuad(quadList, affect);
            $$ = retval;
        }
    | expr '+' expr
        {
            Symbol *retval = newTemp($1->type);
            symbolTable = addSymbol(symbolTable, retval);
            Quad *affect = gencodeTernary(add, $1, $3, retval);
            quadList = addQuad(quadList, affect);
            $$ = retval;
        }
    | expr '-' expr
        {
            Symbol *retval = newTemp($1->type);
            symbolTable = addSymbol(symbolTable, retval);
            Quad *affect = gencodeTernary(substract, $1, $3, retval);
            quadList = addQuad(quadList, affect);
            $$ = retval;
        }
    | expr '*' expr
        {
            Symbol *retval = newTemp($1->type);
            symbolTable = addSymbol(symbolTable, retval);
            Quad *affect = gencodeTernary(multiply, $1, $3, retval);
            quadList = addQuad(quadList, affect);
            $$ = retval;
        }
    | expr '/' expr
        {
            Symbol *retval = newTemp($1->type);
            symbolTable = addSymbol(symbolTable, retval);
            Quad *affect = gencodeTernary(divide, $1, $3, retval);
            quadList = addQuad(quadList, affect);
            $$ = retval;
        }
    ;

cond
    : expr EQ expr
        {
            Quad* true_one = gencodeTestJump(eq, $1, $3, 0);
            quadList = addQuad(quadList, true_one);
            Quad* false_one = gencodeGoTo(0);
            quadList = addQuad(quadList, false_one);
            $$.true_list = addQuad(NULL, true_one);
            $$.false_list = addQuad(NULL, false_one);
        }
    | expr LT expr
        {
            Quad* true_one = gencodeTestJump(lt, $1, $3, 0);
            quadList = addQuad(quadList, true_one);
            Quad* false_one = gencodeGoTo(0);
            quadList = addQuad(quadList, false_one);
            $$.true_list = addQuad(NULL, true_one);
            $$.false_list = addQuad(NULL, false_one);
        }
    | expr GT expr
        {
            Quad* true_one = gencodeTestJump(gt, $1, $3, 0);
            quadList = addQuad(quadList, true_one);
            Quad* false_one = gencodeGoTo(0);
            quadList = addQuad(quadList, false_one);
            $$.true_list = addQuad(NULL, true_one);
            $$.false_list = addQuad(NULL, false_one);
        }
    | expr LE expr
        {
            Quad* true_one = gencodeTestJump(le, $1, $3, 0);
            quadList = addQuad(quadList, true_one);
            Quad* false_one = gencodeGoTo(0);
            quadList = addQuad(quadList, false_one);
            $$.true_list = addQuad(NULL, true_one);
            $$.false_list = addQuad(NULL, false_one);
        }
    | expr GE expr
        {
            Quad* true_one = gencodeTestJump(ge, $1, $3, 0);
            quadList = addQuad(quadList, true_one);
            Quad* false_one = gencodeGoTo(0);
            quadList = addQuad(quadList, false_one);
            $$.true_list = addQuad(NULL, true_one);
            $$.false_list = addQuad(NULL, false_one);
        }
    | expr NE expr
        {
            Quad* true_one = gencodeTestJump(ne, $1, $3, 0);
            quadList = addQuad(quadList, true_one);
            Quad* false_one = gencodeGoTo(0);
            quadList = addQuad(quadList, false_one);
            $$.true_list = addQuad(NULL, true_one);
            $$.false_list = addQuad(NULL, false_one);
        }
    | cond OR nextQuad cond
        {
            $$.true_list = concat($1.true_list, $4.true_list);
            complete($1.false_list, $3);
               $$.false_list = $4.false_list;
        }
    | cond AND nextQuad cond
        {
            $$.true_list = $4.true_list;
            complete($1.true_list, $3);
            $$.false_list = concat($1.false_list, $4.false_list);
        }
    | NOT cond
        {
            $$.true_list = $2.false_list;
            $$.false_list = $2.true_list;
        }
    | '(' cond ')'
        {
            $$.true_list = $2.true_list;
            $$.false_list = $2.false_list;
        }
    ;

goTo
    :
        {
            Quad* goTo = gencodeGoTo(0);
            quadList = addQuad(quadList, goTo);
            $$ = addQuad(NULL, goTo);
        }
    ;
    
nextQuad
    :
        {
            $$ = nextQuad();
        }
    ;

%%

