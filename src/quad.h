/* Thibaut Ehlinger
 * Stan Wilhelm
 */

#ifndef __QUAD_H__
#define __QUAD_H__

#include <stdlib.h>
#include "symbol.h"

typedef enum {
	/* unary */
    print, // print integer
    printmat, // print matrix
    prints,// print string
    /* binary */
    assign, // assign
    assign_addr, // assign address
    load_addr, // load from address
    store_addr, // store at address
    load_and_store_addr, // load and store
    negative, // negative
    /* ternary */
    add, // +
    substract, // -
    divide, // /
    multiply, // *
    lt, // <
    gt, // <
    le, // <=
    ge, // >=
    eq, // ==
    ne, // !=
} QuadOp;

typedef enum {
    unary,
    binary,
    ternary,
    testJump,
    goTo,
    end,
} QuadType;

typedef struct {
    int label;
    int gotoLabel;
    QuadOp op;
    QuadType type;
    Symbol* arg1;
    Symbol* arg2;
    Symbol* res;
} Quad;

typedef struct QuadList {
    Quad* quad;
    struct QuadList* next;
} QuadList;

Quad* gencodeUnary(QuadOp op, Symbol* res);
Quad* gencodeBinary(QuadOp op, Symbol* arg1, Symbol* res);
Quad* gencodeTernary(QuadOp op, Symbol* arg1, Symbol* arg2, Symbol* res);
Quad* gencodeGoTo(int gotoLabel);
Quad* gencodeTestJump(QuadOp op, Symbol* arg1, Symbol* arg2, int gotoLabel);
Quad* gencodeEnd();

QuadList* addQuad(QuadList* quadList, Quad* Quad);
QuadList* concat(QuadList* quadList1, QuadList* quadList2);
void complete(QuadList* list, int label);

void deleteQuad(Quad* quad);
void deleteQuadList(QuadList* quadList);

int nextQuad();

void printQuad(FILE* out, Quad* quad);
void printQuadList(FILE* out, QuadList* list);

#endif
