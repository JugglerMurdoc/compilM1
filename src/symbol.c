/* Thibaut Ehlinger
 * Stan Wilhelm
 */

#include "symbol.h"

#define MAXSTRING 1024
static char temp_string[MAXSTRING];

SymbolTable* addSymbol(SymbolTable *table, Symbol *symbol)
{
    SymbolTable* next = malloc(sizeof(*next));
    next->next = NULL;
    next->symbol = symbol;
    if(table != NULL)
	{
        SymbolTable* end = table;
        while(end->next != NULL)
        {
			end = end->next;
		}
        end->next = next;
        return table;
    }
    else
    {
        return next;
    }
}

void deleteSymbolTable(SymbolTable *table)
{
    SymbolTable* temp;
    while(table != NULL)
	{
        temp = table->next;
        deleteSymbol(table->symbol);
        free(table);
        table = temp;
    }
}

Symbol* searchSymbol(SymbolTable* table, char* val)
{
    int found = 0;
    while(!found && table != NULL )
	{
        int retval = strcmp(table->symbol->name, val);
        if(retval == 0)
		{
            return table->symbol;
        }
        table = table->next;
    }
    return NULL;
}

void printSymbol(FILE* out, Symbol* symbol)
{
    fprintf(out, "%s\t", symbol->name);
    switch(symbol->type)
	{
        case integer:
            printf("integer\n");
            break;
        
        case floating:
			printf("floating\n");
			break;
       
        case string:
            printf("string\n");
            break;

        case array:
			printf("array\n");
            break;

        default:
			break;
    }
}

void printSymbolTable(FILE* out, SymbolTable* table)
{
	fprintf(out, "name\ttype\n");
    while(table != NULL)
	{
        printSymbol(out, table->symbol);
        table = table->next;
    }
}

Symbol* newId(char* name, SymbolType type)
{
    Symbol* symbol = malloc(sizeof(*symbol));
    symbol->name = name;
    symbol->type = type;
    symbol->constant = false;
    return symbol;
}

Symbol* newInteger(char* name)
{ // constant
    Symbol* symbol = malloc(sizeof(*symbol));
    symbol->name = name;
    symbol->type = integer;
    symbol->value = atoi(name);
    symbol->constant = true;
    return symbol;
}
      
Symbol* newString(char* name)
{
    Symbol* symbol = malloc(sizeof(*symbol));
    symbol->name = name;
    symbol->type = string;
    symbol->value = 0;
    symbol->constant = true;
    return symbol;
}

Symbol* newFloating(char* name)
{ // constant flo
    Symbol* symbol = malloc(sizeof(*symbol));
    symbol->name = name;
    symbol->type = floating;
    symbol->f_value = atof(name);
    symbol->constant = true;
    return symbol;
}

Symbol* newTemp(SymbolType type)
{
	static int temp_number = 0;
	
    Symbol* symbol = malloc(sizeof(*symbol));
    char* temp;
    snprintf(temp_string, MAXSTRING, "temp%d", temp_number);
    temp = strdup(temp_string);
    temp_number++;
    symbol->name = temp;
    symbol->type = type;
    if(type == integer)
    {
		symbol->constant = false;
	}
    return symbol;
}

void deleteSymbol(Symbol* symbol)
{
    if(symbol->type == array)
	{
        free(symbol->array_value);
        destroy_array_dimension(symbol->dim);
    }
    free(symbol->name);
    free(symbol);
}

array_dimension* new_dimension(int size)
{
    array_dimension* ard = malloc(sizeof(*ard));
    ard->size = size;
    ard->next = NULL;
    return ard;
}

array_dimension* add_dimension(array_dimension* dim1, array_dimension* dim2)
{
    array_dimension* temp = dim1;
    if(dim1 == NULL)
	{
        return dim2;
    }
    else{
        while(temp->next != NULL)
	{
            temp = temp->next;
        }
        temp->next = dim2;
        return dim1;
    }
}

int getArraySize(array_dimension* dim)
{
    int size = 1;
    while(dim != NULL)
	{
        size *= dim->size;
        dim = dim->next;
    }
    return size;
}

void destroy_array_dimension(array_dimension* dim)
{
    array_dimension* temp;
    while(dim != NULL)
	{
        temp = dim->next;
        free(dim);
        dim = temp;
    }
}

int getDim(array_dimension* dim)
{
    int nbdim = 0;
    while(dim != NULL)
	{
        nbdim++;
        dim = dim->next;
    }
    return nbdim;
}

int getNthDimension(Symbol* symbol, int n)
{
    if(symbol->type != array || symbol->nbdim < n || n == 0)
	{
        return 0;
    }
    
    array_dimension* dim = symbol->dim;
    int i;
    for(i = 1; i < n; i++)
	{
        dim = dim->next;
    }
    return dim->size;
}

Symbol* newArray(char* name, array_dimension* dim)
{
    Symbol *symbol = malloc(sizeof(*symbol));
    symbol->type = array;
    symbol->name = name;
    symbol->dim = dim;
    symbol->nbdim = getDim(dim);
    symbol->arraySize = getArraySize(dim);
    symbol->array_value = NULL;
    return symbol;
}

char* itoa(int val)
{
    snprintf(temp_string, MAXSTRING, "%d", val);
    return strdup(temp_string);
}
