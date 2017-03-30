/* Thibaut Ehlinger
 * Stan Wilhelm
 */

#include "quad.h"

static int currentQuad = 0;

Quad* gencodeUnary(QuadOp op, Symbol* res)
{
    Quad* newQuad = malloc(sizeof(*newQuad));
    newQuad->label = currentQuad;
    currentQuad++;
    newQuad->op = op;
    newQuad->type = unary;
    newQuad->res = res;
    return newQuad;
}

Quad* gencodeBinary(QuadOp op, Symbol* arg1, Symbol* res)
{
    Quad* newQuad = malloc(sizeof(*newQuad));
    newQuad->label = currentQuad;
    currentQuad++;
    newQuad->op = op;
    newQuad->type = binary;
    newQuad->arg1 = arg1;
    newQuad->res = res;
    return newQuad;
}

Quad* gencodeTernary(QuadOp type, Symbol* arg1, Symbol* arg2, Symbol* res)
{
    Quad* newQuad = malloc(sizeof(*newQuad));
    newQuad->label = currentQuad;
    currentQuad++;
    newQuad->op = type;
    newQuad->type = ternary;
    newQuad->arg1 = arg1;
    newQuad->arg2 = arg2;
    newQuad->res = res;
    return newQuad;
}

Quad* gencodeGoTo(int gotoLabel)
{
    Quad* newQuad = malloc(sizeof(*newQuad));
    newQuad->label = currentQuad;
    currentQuad++;
    newQuad->type = goTo;
    newQuad->gotoLabel = gotoLabel;
    return newQuad;
}

Quad* gencodeTestJump(QuadOp op, Symbol* arg1, Symbol* arg2, int gotoLabel)
{
    Quad* newQuad = malloc(sizeof(*newQuad));
    newQuad->label = currentQuad;
    currentQuad++;
    newQuad->op = op;
    newQuad->type = testJump;
    newQuad->arg1 = arg1;
    newQuad->arg2 = arg2;
    newQuad->gotoLabel = gotoLabel;
    return newQuad;
}

Quad* gencodeEnd(void)
{
    Quad* newQuad = malloc(sizeof(*newQuad));
    newQuad->type = end;
    newQuad->label = currentQuad;
    currentQuad++;
    return newQuad;
}

void deleteQuad(Quad* quad)
{
    free(quad);
}

QuadList* addQuad(QuadList* quadList, Quad* newQuad)
{
    QuadList* newQuadList = malloc(sizeof(*newQuadList));
    newQuadList->quad = newQuad;
    newQuadList->next = NULL;
    if(quadList == NULL)
	{
        return newQuadList;
    }
    else
    {
        QuadList *end = quadList;
        while(end->next != NULL)
        {
			end = end->next;
		}
        end->next = newQuadList;
        return quadList;
    }
}

QuadList* concat(QuadList* quadList1, QuadList* quadList2)
{
    if(quadList1 == NULL)
	{
        return quadList2;
    }
    else
    {
        if( quadList2 == NULL )
		{
            return quadList1;
        }
        else
        {
            QuadList *end = quadList1;
            while(end->next != NULL)
                end = end->next;
            end->next = quadList2;
            return quadList1;
        }
    }
}

void deleteQuadList(QuadList* quadList)
{
    QuadList* temp;
    while(quadList != NULL)
	{
        temp = quadList->next;
        deleteQuad(quadList->quad);
        free(quadList);
        quadList = temp;
    }
}

int nextQuad()
{
    return currentQuad;
}

void complete(QuadList* list, int label)
{
    while(list != NULL)
	{
        Quad* quad = list->quad;
        quad->gotoLabel = label;
        list = list->next;
    }
}

void printQuad(FILE* out, Quad* quad)
{
    fprintf(out, "%d\t", quad->label);
    switch(quad->type)
	{
        case unary:
            switch(quad->op)
			{
                case print:
                    fprintf(out, "print\t\t");
                    break;
                    
                case printmat:
                    fprintf(out, "printmat\t");
                    break;
       
                case prints:
                    fprintf(out, "printstring\t");
                    break;
       
                default:
                  fprintf(out, "ERROR\t"); break;
            }
            fprintf(out, "-\t-\t%s\n", quad->res->name);
            break;

        case binary:
            switch(quad->op)
			{
                case assign:
                    fprintf(out, "assign\t\t"); break;
                case assign_addr:
                    fprintf(out, "assign_addr\t"); break;
                case load_addr:
                    fprintf(out, "load_addr\t"); break;
                case store_addr:
                    fprintf(out, "store_addr\t"); break;
                case load_and_store_addr:
                    fprintf(out, "load_and_store_addr\t"); break;
                case negative:
                    fprintf(out, "negation\t\t"); break;
                default :
					break;
            }
            fprintf(out, "%s\t-\t%s\n", quad->arg1->name, quad->res->name);
            break;

        case ternary:
            switch(quad->op)
			{
                case add :
                    fprintf(out, "add\t\t"); break;
                case substract :
                    fprintf(out, "substract\t\t"); break;
                case divide:
                    fprintf(out, "divide\t\t"); break;
                case multiply :
                    fprintf(out, "multiply\t"); break;
                case lt :
                    fprintf(out, "lower_than\t"); break;
                case gt :
                    fprintf(out, "greater_than\t"); break;
                case le :
                    fprintf(out, "lower_equal\t"); break;
                case ge :
                    fprintf(out, "greater_equal\t"); break;
                case eq :
                    fprintf(out, "equal\t\t"); break;
                default :
					break;
            }
            fprintf(out, "%s\t%s\t%s\n", quad->arg1->name, quad->arg2->name, quad->res->name);
            break;

        case goTo:
            fprintf(out, "goto\t\t-\t-\t%d\n", quad->gotoLabel);
            break;

        case testJump:
            switch(quad->op)
			{
                case lt :
                    fprintf(out, "jump_lower_than\t"); break;
                case gt :
                    fprintf(out, "jump_greater_than\t"); break;
                case le :
                    fprintf(out, "jump_lower_equal\t"); break;
                case ge :
                    fprintf(out, "jump_greater_equal\t"); break;
                case eq :
                    fprintf(out, "jump_equal\t"); break;
                case ne :
                    fprintf(out, "jump_not_equal\t"); break;
                default :
					break;
            }
            fprintf(out, "%s\t%s\t%d\n", quad->arg1->name, quad->arg2->name, quad->gotoLabel);
            break;

        case end:
            fprintf(out, "end\t\t-\t-\t-\n");
            break;

        default:
			break;
    }
}

void printQuadList(FILE* out, QuadList* list)
{
	fprintf(out, "label\top\t\targ1\targ2\tres\n");
    while(list != NULL)
	{
        printQuad(out, list->quad);
        list = list->next;
    }
}
