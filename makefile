# Thibaut Ehlinger
# Stan Wilhelm

SRCDIR = src/

all :
	yacc -d $(SRCDIR)matC.y 
	lex $(SRCDIR)matC.l
	mv lex.yy.c y.tab.c y.tab.h $(SRCDIR)
	gcc -Wall -g -o matC $(SRCDIR)y.tab.c $(SRCDIR)lex.yy.c $(SRCDIR)matC.c $(SRCDIR)quad.c $(SRCDIR)symbol.c $(SRCDIR)gencode.c $(SRCDIR)int_list.c -ly -lfl
	
clean :
	rm -f matC matC.s $(SRCDIR)lex.yy.c $(SRCDIR)y.tab.c $(SRCDIR)y.tab.h
