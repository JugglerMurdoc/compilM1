/* Thibaut Ehlinger
 * Stan Wilhelm
 */

#ifndef __SYMBOL_H__
#define __SYMBOL_H__

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

typedef enum
{      
	string,
    integer,
    floating,
    array
} SymbolType;

typedef struct array_dimension {
    int size;
    struct array_dimension* next;
} array_dimension;

typedef struct {
    SymbolType type;
    char* name;
    bool constant;
    int value;
    float f_value;
    int* array_value;
    array_dimension* dim;
    int nbdim;
    int arraySize;
} Symbol;

typedef struct SymbolTable {
	Symbol* symbol;
    struct SymbolTable* next;
} SymbolTable;

SymbolTable* addSymbol(SymbolTable *table, Symbol *symbol);
void deleteSymbolTable(SymbolTable *table);
Symbol* searchSymbol(SymbolTable* table, char* val);
Symbol* newId(char* name, SymbolType type); // constant
Symbol* newInteger(char* name);
       
Symbol* newString(char* name);
       
Symbol* newFloating(char* name);
void deleteSymbol(Symbol* Symbol);
Symbol* newTemp(SymbolType type);
array_dimension* new_dimension(int size);
array_dimension* add_dimension(array_dimension* dim1, array_dimension* dim2);
int getArraySize(array_dimension* dim);
void destroy_array_dimension(array_dimension* dim);
int getDim(array_dimension* dim);
int getNthDimension(Symbol *array, int n);
Symbol* newArray(char* name, array_dimension* dim);
char* itoa(int val);
void printSymbol(FILE* out, Symbol* symbol);
void printSymbolTable(FILE* out, SymbolTable* table);

#endif
