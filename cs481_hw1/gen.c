#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

int main(int argc, char** argv){

//if( argc != 3){ return -1;}

	int i;
	char alphabet[5]={'A','T','C','G','A'};
	size_t size = atoi(argv[1]);
	int repeatsize = atoi(argv[3]);
	char repeat[1000];

	for(i=0;i<repeatsize;i++){
		repeat[i]=alphabet[(int)(rand()%4)];
	}
	size_t patternloc = atoi(argv[2]);
	srand(time(NULL));
	FILE *fptr = fopen(argv[5],"a+");
	for(i=0;i<patternloc;i++){
		fprintf(fptr,"%c",repeat[i%repeatsize]);
	}
	char pattern[1000];
	FILE *pptr;

	pptr = fopen(argv[4],"r");


	
	size_t patsize=	fread(pattern,sizeof(char),1000,pptr) - 1 ;
	int j;
	for(j=0;j<patsize;j++){
		fprintf(fptr,"%c",pattern[j]);
	}
	for(;i<size-1;i++){
		fprintf(fptr,"%c",alphabet[(int)(rand()%4)]);
	}
	fclose(fptr);
	fclose(pptr);
	return 0;
}
