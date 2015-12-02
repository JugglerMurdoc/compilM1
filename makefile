all : 
	yacc -d test.y 
	lex test.l	
	gcc y.tab.c lex.yy.c -ly -lfl
	
clean :
	rm -f a.out lex.yy.c y.tab.c y.tab.h
