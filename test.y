%{
	#include <stdio.h>
	#include <stdlib.h>
	void yyerror(char*);
%}

%union{
  int valeur;
}

%token <valeur> ENTIER
%token AND
%token IF
%token WHILE
%token DO
%token DONE
%token ELSE
%token ENDIF
%token THEN
%token TRUE
%token FALSE
%token OR
%token OPAR
%token CPAR
%token NOT
%token ASSIGN
%token EQUAL
%token ID
%token NUM

%%

stmt : ID ASSIGN expr {  }
	  | WHILE condition DO stmtlist DONE {  }
	  | IF condition THEN stmtlist ENDIF {  }
	  | IF condition THEN stmtlist ELSE stmtlist ENDIF {  };
	  
stmtlist : stmtlist stmt {  }
		  | stmt {  };
		  
expr : ID {  }
	  | NUM {  };

condition : ID EQUAL NUM {  }
		   | TRUE {  }
		   | FALSE {  }
		   | condition OR condition {  }
		   | condition AND condition {  }
		   | NOT condition {  }
		   | OPAR condition CPAR {  };

%%

void yyerror(char *s)
{
	fprintf(stderr, "%s\n", s);
}

int main()
{
	FILE *file = NULL;
	file = fopen("test.txt", "r");
	
	if(file == NULL)
	{
		fprintf(stderr, "Fuck you\n");
		exit(1);
	}
	
	stdin = file;
	
	while(1)
	{
		yyparse();
	}
	return 0; 
}

