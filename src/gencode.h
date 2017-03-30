/* Thibaut Ehlinger
 * Stan Wilhelm
 */

#ifndef __GENCODE_H__
#define __GENCODE_H__

#include "quad.h"

void gencodeData(FILE* out, SymbolTable* symbolTable);
void gencodeText(FILE* out, QuadList *quadList);

void gencodeLoadSymbol(FILE* out, Symbol* symbol, char* reg);
void gencodeLoadSymbolAddress(FILE* out, Symbol* symbol, char* reg);
void gencodeLoadRegisterAddress(FILE* out, const char* regAddr, const char* dest);

void gencodeStoreSymbol(FILE* out, Symbol* symbol, char* reg);
void gencodeStoreRegisterAddress(FILE* out, const char* val, const char* addr);

char* generateStringName(char* string);

#endif
