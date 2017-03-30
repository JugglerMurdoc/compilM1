/* Thibaut Ehlinger
 * Stan Wilhelm
 */

#include <getopt.h>
#include "matC.h"
#include "gencode.h"

int main(int argc, char *argv[])
{
	if(argc < 2)
	{
		fprintf(stderr, "usage: ./matC -h\n");
		exit(1);
	}
	
	int option = 0, correct = 0;
	
	char *filename = (char*) malloc(sizeof(char)*100);
	int opt[3];
	
	int i;
	for(i = 0; i < 3; i++)
	{
		opt[i] = 0;
	}
	i = 0;
	
	while((option = getopt(argc, argv, "f:hqs")) != -1)
	{
		switch(option)
		{
			case 'f': // fichier matC (obligatoire)
				strcpy(filename, optarg);
				correct = 1;
			break;
			
			case 'h': // help
				opt[0] = 1;
			break;
			
			case 'q': // print quadList
				opt[1] = 1;
			break;
			
			case 's': // print symbolTable
				opt[2] = 1;
			break;
			
			default:
				free(filename);
				fprintf(stderr, "Wrong option\n");
				exit(1);
		}
	}
	
	if(opt[0]) // help
	{
		printf("Utilisation : ./matC -f matC_file [OPTIONS]\n\n");
		printf("[OPTIONS] :\n");
		printf("-h\tAide\n");
		printf("-f F\tAnalyse du fichier F\n");
		printf("-q \tAfficher la liste des quad\n");
		printf("-s \tAfficher la table des symboles\n");
	}
	else if(correct) // utilisation correcte
	{
		yyin = fopen(filename, "r");
		
		if(yyin == NULL)
		{
			fprintf(stderr, "Can't open file %s\n", filename);
			free(filename);
			exit(1);
		}
		
		correct = yyparse();
		fclose(yyin);
		
		if(correct == 0)
		{
			if(opt[1])
			{
				printQuadList(stdout, quadList);
			}
			
			if(opt[2])
			{
				printSymbolTable(stdout, symbolTable);
			}	
							
			FILE *file = NULL;
			file = fopen("matC.s", "w");
			if(file == NULL)
			{
				fprintf(stderr, "Can't open file matC.s\n");
				free(filename);
				exit(1);
			}
			
			gencodeData(file, symbolTable);
			gencodeText(file, quadList);
			fclose(file);
			
			yylex_destroy();
			deleteQuadList(quadList);
			deleteSymbolTable(symbolTable);
		}
		else
		{
			fprintf(stderr, "Erreur yacc\n");
		}
	}
	else
	{
		free(filename);
		fprintf(stderr, "usage: ./matC -h\n");
		exit(1);
	}
		
	free(filename);
    
    return 0;
}
