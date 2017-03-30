/* Thibaut Ehlinger
 * Stan Wilhelm
 */

#include "gencode.h"

void gencodeData(FILE* out, SymbolTable* symbolTable)
{
    fprintf(out, ".data\n");
    fprintf(out, "newline: .asciiz \"\\n\"\n");
    fprintf(out, "space: .asciiz \" \"\n");
    while(symbolTable != NULL)
	{
        Symbol* symbol = symbolTable->symbol;
       
        char* data_name;
       
        switch(symbol->type)
		{
            case integer:
				if(symbol->constant)
				{
					fprintf(out, "const%s:\t.word %d\n", symbol->name, symbol->value);
                }
                else
                {
                    fprintf(out, "_%s:\t.word %d\n", symbol->name, 0);
                }
                break;
                
            case floating:
                if(symbol->constant)
				{
                      fprintf(out, "const%s:\t.float %f\n", symbol->name, symbol->f_value);
                }
                else
                {
                    fprintf(out, "_%s:\t.word %d\n", symbol->name, 0);
                }
                break;
                
            case array:
                fprintf(out, "_%s:\t.word ", symbol->name);
                int i;
                for(i = 0; i < symbol->arraySize; i++)
				{
                    if(symbol->array_value == NULL)
					{
                        fprintf(out, "0 ");
                    }
                    else
                    {
                        fprintf(out, "%d ", symbol->array_value[i]);
                    }
                }
                fprintf(out, "\n");
                break;
            
            case string:
				 data_name = generateStringName(symbol->name);			 
				 fprintf(out, "%s:\t.asciiz %s\n", data_name, symbol->name);
				 free(data_name);
				break;
				
            default:
                break;
        }
        symbolTable = symbolTable->next;
    }
}

void gencodeLoadSymbolAddress(FILE* out, Symbol* symbol, char* reg)
{
	char* data_name;
    switch(symbol->type)
	{
        case integer:
            if(symbol->constant)
			{        
                    fprintf(out, "la %s, const%s\n", reg, symbol->name);   
            }
            else
            {
                fprintf(out, "la %s, _%s\n", reg, symbol->name);
            }
            break;
        
        case array:
            fprintf(out, "la %s, _%s\n", reg, symbol->name);
            break;
       
        case string:
			data_name = generateStringName(symbol->name);
			fprintf(out, "la %s, %s\n", reg, data_name);
			free(data_name);
			break;
			
        default:
            break;
    }
}

void gencodeLoadSymbol(FILE* out, Symbol* symbol, char* reg)
{
    int is_integer = (reg[1] != 'f');
    switch(symbol->type)
	{
        case integer:
			if(is_integer)
			{
				if(symbol->constant)
				{					
					fprintf(out, "lw %s, const%s\n", reg, symbol->name);
				}
				else
				{
					fprintf(out, "lw %s, _%s\n", reg, symbol->name);
				}
            }
            else
            {
				if(symbol->constant)
				{	
					fprintf(out, "l.s %s, const%s\n", reg, symbol->name);			
				}
				else
				{
					if(is_integer)
					{
						fprintf(out, "lw %s, _%s\n", reg, symbol->name);
					}
					else
					{
						fprintf(out, "l.s %s, _%s\n", reg,  symbol->name);
					}
				}
			}
            break;
        
        case floating:
            if(symbol->constant)
			{
				fprintf(out, "l.s %s, const%s\n", reg, symbol->name); 
            }
            else
            {
				if(is_integer)
				{
					fprintf(out, "lw %s, _%s\n", reg, symbol->name);
				}
				else
				{
					fprintf(out, "l.s %s, _%s\n", reg,  symbol->name);
				}
            }
            break;
        
        default:
            break;
    }
}

void gencodeStoreSymbol(FILE* out, Symbol* symbol, char* reg)
{
	int is_integer = (reg[1] != 'f');
	 
    switch(symbol->type)
	{
		case integer:
        if(is_integer)
		{
            if(symbol->constant)
			{
				fprintf(out, "sw %s, const%s\n", reg, symbol->name);
            }
            else
            {
                fprintf(out, "sw %s, _%s\n", reg, symbol->name);
            }
        }
        else
        {
			if(symbol->constant)
			{
				fprintf(out, "s.s %s, const%s\n", reg, symbol->name);
			}
            else
            {
                fprintf(out, "s.s %s, _%s\n", reg, symbol->name);
            }
		}
            break;
        case floating:
            if(symbol->constant)
			{
				fprintf(out, "s.s %s, const%s\n", reg, symbol->name);
            }
            else
            {
                fprintf(out, "s.s %s, _%s\n", reg, symbol->name);
            }
            break;
        
        default:
            break;
    }
}

void gencodeLoadRegisterAddress(FILE* out, const char* regAddr, const char* dest)
{
	fprintf(out, "lw %s, 0(%s)\n", dest, regAddr);
}

void gencodeStoreRegisterAddress(FILE* out, const char* val, const char* addr)
{
    fprintf(out, "sw %s, 0(%s)\n", val, addr);
}

void gencodeText(FILE* out, QuadList *quadList)
{
	int i, j, is_integer;
    fprintf(out, ".text\nmain:\n");
    
    while(quadList != NULL)
	{
        Quad* quad = quadList->quad;
        fprintf(out, "_%d: ", quad->label);
        is_integer = (((quad->arg1 == NULL) || (quad->arg1->type != floating)) && ((quad->arg2 == NULL) || (quad->arg2->type != floating)));

        switch(quad->type)
		{
            case unary:
                switch(quad->op)
				{
                    case print:
                        if(quad->res->type == integer)
						{
							gencodeLoadSymbol(out, quad->res, "$a0");
							fprintf(out, "li $v0, 1\nsyscall\n");
                        }
                        else if(quad->res->type == floating)
						{
							gencodeLoadSymbol(out, quad->res, "$f12");
							fprintf(out, "li $v0, 2\nsyscall\n");
                        }
                        
                        fprintf(out, "li $v0, 4\nla $a0, newline\nsyscall\n"); // \n
                        break;
       
                    case prints:
						gencodeLoadSymbolAddress(out, quad->res, "$a0");
						fprintf(out, "li $v0, 4\nsyscall\n");
                       
                        fprintf(out, "li $v0, 4\nla $a0, newline\nsyscall\n");
                        break;
                        
       
                    case printmat:
						for(i = 0; i < quad->res->arraySize; i++)
						{
							for(j = 0; j < quad->res->arraySize/quad->res->dim->size; i++, j++)
							{
								gencodeLoadSymbolAddress(out, quad->res, "$t0");
								fprintf(out, "li $t1, %d\n", i*4);
								fprintf(out, "addu $t2, $t0, $t1\n");
								gencodeLoadRegisterAddress(out, "$t2", "$a0");
								
								fprintf(out, "li $v0, 1\nsyscall\n");
								fprintf(out, "li $v0, 4\nla $a0, space\nsyscall\n");
							}
							i--;
							fprintf(out, "li $v0, 4\nla $a0, newline\nsyscall\n");
						}
                        break;
                    
                    default:
                        break;
                }
                break;
            
            case binary:
				if(is_integer)
				{
					switch(quad->op)
					{
						case assign:
							gencodeLoadSymbol(out, quad->arg1, "$t0");
							gencodeStoreSymbol(out, quad->res, "$t0");
							break;
						
						case assign_addr:
							gencodeLoadSymbolAddress(out, quad->arg1, "$t0");
							gencodeStoreSymbol(out, quad->res, "$t0");
							break;
						
						case load_addr:
							gencodeLoadSymbol(out, quad->arg1, "$t0");
							gencodeLoadRegisterAddress(out, "$t0", "$t1");
							gencodeStoreSymbol(out, quad->res, "$t1");
							break;
						
						case store_addr:
							gencodeLoadSymbol(out, quad->arg1, "$t0");
							gencodeLoadSymbol(out, quad->res, "$t1");
							gencodeStoreRegisterAddress(out, "$t0", "$t1");
							break;
						
						case load_and_store_addr:
							gencodeLoadSymbol(out, quad->arg1, "$t0");
							gencodeLoadRegisterAddress(out, "$t0", "$t1");
							gencodeLoadSymbol(out, quad->res, "$t0");
							gencodeStoreRegisterAddress(out, "$t1", "$t0");
						
						case negative:
							gencodeLoadSymbol(out, quad->arg1, "$t0");
							fprintf(out, "negu $t1, $t0\n");
							gencodeStoreSymbol(out, quad->res, "$t1");
							break;
						
						default:
							break;
					}
			}else
			{
				switch(quad->op)
				{
					case assign:
						gencodeLoadSymbol(out, quad->arg1, "$f0");
						gencodeStoreSymbol(out, quad->res, "$f0");
						break;
						
					case assign_addr:
						gencodeLoadSymbolAddress(out, quad->arg1, "$f0");
						gencodeStoreSymbol(out, quad->res, "$f0");
						break;
					
					case load_addr:
						gencodeLoadSymbol(out, quad->arg1, "$f0");
						gencodeLoadRegisterAddress(out, "$f0", "$f2");
						gencodeStoreSymbol(out, quad->res, "$f2");
						break;
					
					case store_addr:
						gencodeLoadSymbol(out, quad->arg1, "$f0");
						gencodeLoadSymbol(out, quad->res, "$f2");
						gencodeStoreRegisterAddress(out, "$f0", "$f2");
						break;
					
					case load_and_store_addr:
						gencodeLoadSymbol(out, quad->arg1, "$f0");
						gencodeLoadRegisterAddress(out, "$f0", "$f2");
						gencodeLoadSymbol(out, quad->res, "$f0");
						gencodeStoreRegisterAddress(out, "$f2", "$f0");
						break;
					
					case negative:
						gencodeLoadSymbol(out, quad->arg1, "$f0");
						fprintf(out, "negu $f2, $f0\n");
						gencodeStoreSymbol(out, quad->res, "$f2");
						break;
							
					default:
						break;
					}
				}
                break;

            case ternary:
                if(is_integer)
				{
					gencodeLoadSymbol(out, quad->arg1, "$t0");
					gencodeLoadSymbol(out, quad->arg2, "$t1");
					switch(quad->op)
					{
						case add :
							fprintf(out, "addu $t2, $t0, $t1\n"); break;
						case substract :
							fprintf(out, "subu $t2, $t0, $t1\n"); break;
						case divide:
							fprintf(out, "div $t2, $t0, $t1\n"); break;
						case multiply :
							fprintf(out, "mul $t2, $t0, $t1\n"); break;
						case lt :
							fprintf(out, "slt $t2, $t0, $t1\n"); break;
						case gt :
							fprintf(out, "sgt $t2, $t0, $t1\n"); break;
						case le :
							fprintf(out, "sle $t2, $t0, $t1\n"); break;
						case ge :
							fprintf(out, "sge $t2, $t0, $t1\n"); break;
						case eq :
							fprintf(out, "seq $t2, $t0, $t1\n"); break;
						case ne :
							fprintf(out, "sne $t2, $t0, $t1\n"); break;
						default :
							break;
                }
                gencodeStoreSymbol(out, quad->res, "$t2");
			}
			else
			{
                gencodeLoadSymbol(out, quad->arg1, "$f0");
                gencodeLoadSymbol(out, quad->arg2, "$f2");
                switch(quad->op)
				{
                    case add :
                        fprintf(out, "add.s $f4, $f0, $f2\n"); break;
                    case substract :
                        fprintf(out, "sub.s $f4, $f0, $f2\n"); break;
                    case divide:
                        fprintf(out, "div.s $f4, $f0, $f2\n"); break;
                    case multiply :
                        fprintf(out, "mul.s $f4, $f0, $f2\n"); break;
                    case lt :
                        fprintf(out, "slt.s $f4, $f0, $f2\n"); break;
                    case gt :
                        fprintf(out, "sgt $f4, $f0, $f2\n"); break;
                    case le :
                        fprintf(out, "sle $f4, $f0, $f2\n"); break;
                    case ge :
                        fprintf(out, "sge $f4, $f0, $f2\n"); break;
                    case eq :
                        fprintf(out, "seq $f4, $f0, $f2\n"); break;
                    case ne :
                        fprintf(out, "sne $f4, $f0, $f2\n"); break;
                    default :
                        break;
                }
                gencodeStoreSymbol(out, quad->res, "$f4");
			}
                break;
            
            case testJump:
				if(is_integer)
				{
					gencodeLoadSymbol(out, quad->arg1, "$t0");
					gencodeLoadSymbol(out, quad->arg2, "$t1");
					switch(quad->op)
					{
						case lt :
							fprintf(out, "blt $t0, $t1, _%d\n", quad->gotoLabel); break;
						case gt :
							fprintf(out, "bgt $t0, $t1, _%d\n", quad->gotoLabel); break;
						case le :
							fprintf(out, "ble $t0, $t1, _%d\n", quad->gotoLabel); break;
						case ge :
							fprintf(out, "bge $t0, $t1, _%d\n", quad->gotoLabel); break;
						case eq :
							fprintf(out, "beq $t0, $t1, _%d\n", quad->gotoLabel); break;
						case ne :
							fprintf(out, "bne $t0, $t1, _%d\n", quad->gotoLabel); break;
						default:
							break;
					}
				}
			break;

            case goTo:
                fprintf(out, "b _%d\n", quad->gotoLabel);
                break;

            case end:
                fprintf(out, "li $v0, 10\nsyscall\n");
                break;

            default:
                break;
        }
        quadList = quadList->next;
    }
}

char* generateStringName(char* string)
{
	char char_to_test;
	char *result = (char*) malloc (256*sizeof(char));
		
	strcpy(result, string);
	int i = 0;
	while(string[i] != '\0' && i < 256)
	{
		char_to_test = string[i];
		if ((char_to_test == ' ') ||
			(char_to_test == ',') ||
			(char_to_test == '"') ||
			(char_to_test == ';') ||
			(char_to_test == '!') ||
			(char_to_test == '\n') ||
			(char_to_test == '\t') ||
			(char_to_test == '-'))
		{
			result[i] = '_';
					
		}
			i++;
	}
	
	return result;
}
