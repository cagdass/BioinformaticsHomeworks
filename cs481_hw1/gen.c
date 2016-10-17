#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

int main(int argc, char** argv){

	if( argc != 3){ return -1;}

	int i;
	char alphabet[5]={'A','T','C','G','A'};
	size_t size = atoi(argv[1]);
	int repeatsize = 14;
	char repeat[14];
	for(i=0;i<repeatsize;i++){
		repeat[i]=alphabet[(int)(rand()%4)];
	}
	size_t patternloc = atoi(argv[2]);
	srand(time(NULL));
	FILE *fptr = fopen("data","a+");
	for(i=0;i<patternloc;i++){
		fprintf(fptr,"%c",repeat[i%repeatsize]);
	}
	char pattern[1000];
	FILE *pptr = fopen("pattern","r");
	fread(pattern,sizeof(char),1000,pptr);
	fprintf(fptr,"%s",pattern);
	for(;i<size;i++){
		fprintf(fptr,"%c",alphabet[(int)(rand()%4)]);
	}
	fclose(fptr);
	fclose(pptr);
	return 0;
}
