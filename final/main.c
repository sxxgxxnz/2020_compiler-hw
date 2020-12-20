#include <stdio.h>
#include <stdlib.h>
#include "type.h"
extern int syntax_err;
extern int semantic_err;
extern A_NODE *root;
FILE *fout;
extern FILE *yyin;

void initialize();
void print_ast();
void print_sem_ast();
void code_generation();
void semantic_analysis();
void main(int argc, char *argv[])
{
	if(argc<2){
		printf("source file not given\n");
		exit(1);
	}
	if(strcmp(argv[1],"-o")==0){
		if(argc>3)
			if((fout=fopen(argv[2],"w"))==NULL){
				printf("can not open output file: %s\n",argv[3]);
				exit(1);
			}
			else ;
		else{
			printf("out file not given\n");
			exit(1);
		}	
	}
	else if(argc==2){
		if((fout=fopen("a.asm","w"))==NULL){
			printf("can not open output file : a.asm\n");
			exit(1);
		}
	}
	if((yyin=fopen(argv[argc-1],"r"))==NULL){
		printf("can  not open input file: %s\n",argv[argc-1]);
		exit(1);
	}
	printf("\nstart syntax analysis\n");
	initialize();
	yyparse();
	if(syntax_err)
		exit(1);
	//print_ast(root);

	//printf("\n\n======================================\n\n");

	printf("\nstart semantic analysis\n");
	semantic_analysis(root);
	//print_sem_ast(root);
	if(semantic_err)
		exit(1);
	printf("start code generation\n");
	code_generation(root);
	fclose(fout);
	exit(0);
}
