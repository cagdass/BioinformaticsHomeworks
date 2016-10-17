#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define _SUPER_MAGIC_NUMBER 8
#define _MAGICAL_MYSTERY_MASK 0X07
#define _ALPHABET_SIZE 4
#define _SIZE 400000000
#define _PATTERN_SIZE 100
char buffer[_SIZE+1] = {0};
char pattern[_PATTERN_SIZE+1] = {0};

char alphabet[_ALPHABET_SIZE] = {'A','T','C','G'};
char alphabetun[_ALPHABET_SIZE] = {'a','t','c','g'};
// HMM I need some kind of hashing for char->index
// I dont want to create an array of size 90 for 4 chars
// Lets see...
// Our chars are the following: 65 84 67 71 or for lc  97 116 99 103 
// we cant use % 4 because C%4==G%4
// UC % 8 -> 1 4 3 7, lc % 8 -> 1 4 3 7
// WOW how convenient
int indexer[_SUPER_MAGIC_NUMBER] = { -37, 0, -37, 2, 1, -37, -37, 3};
//int nukleohash(char in){ return (int)in%_SUPER_MAGIC_NUMBER;}
//So alphabet[index[nukleohash(character)]] = character I think
//it is case insensitive ofc
//however modulus op is expensive, and I am not sure if compiler
//optimizes % 8 to logic and I am too lazy to check.
//so we need a mask to do same job.
//which is 0X0007 I think
static inline int fasthash(char in){ return in&_MAGICAL_MYSTERY_MASK;}
//Yep, it works just fine



int brute_force(size_t dsize, size_t psize){
	int i,j;
	for(i=0;i<dsize;i++){
		for(j=0;j<psize;j++){
			if(buffer[i+j]!=pattern[j]){break;}
		}
		if(j==psize){return i;}
	}
	return -1;
}

int KMP(size_t dsize, size_t psize){
	int _fail[_PATTERN_SIZE];
	_fail[0]=0;
	int i=1;
	int j=0;
	while(i<psize){
		if(pattern[i]==pattern[j]){
			_fail[i]=i+1;
			i++;
			j++;
		}else if(j>0){
			j=_fail[j-1];
		}
		else{
			_fail[i]=0;
			i++;
		}
	}
//..
	i = 0;
	j = 0;
	while(i<dsize){
		if(buffer[i]==pattern[j]){
			if(j==psize-1){
				return i-j;
			}
			else{
				i++;
				j++;
			}
		}
		else{
			if(j>0){
				j=_fail[j-1];
			}
			else{
				i++;
			}
		}
	}
	return -1;
}

int BM(size_t dsize, size_t psize){
	int _loc[_ALPHABET_SIZE];		//Last Occurance
	int _gsx[_PATTERN_SIZE];	//Good Suffix
	int i;
	char tmp;
	//Init bad character here
	for(i=0;i<_ALPHABET_SIZE;i++){
		_loc[i]=psize;
	}
	for(i=0;i<psize-1;i++){
		_loc[indexer[fasthash(pattern[i])]] = psize - i - 1;
	}

	//Init suffixes here
	_gsx[psize-1]=psize;
	
		

	return -1;
}

int RK(size_t dsize, size_t psize){

	return -1;
}
/*
int maintester(){
	int i;
	for(i=0;i<_ALPHABET_SIZE;i++){
		printf("%c -> %d ?= %d",alphabet[i],fasthash(alphabet[i]),nukleohash(alphabet[i]));		
	}

	return 0;
}
*/
int main(int argc, char** argv){
	if(argc!=3){
		printf("Something went wrong you need 2 arguments not %d\n",argc-1);
		return -1;
	}
	
	clock_t begin = clock();
	FILE* fptr = fopen(argv[1], "r");


	size_t data_size = fread(buffer, sizeof(char), _SIZE , fptr );


	fclose(fptr);
	fptr = fopen(argv[2],"r");

	size_t pattern_size = fread(pattern,sizeof(char), _PATTERN_SIZE,fptr) -1;
		
	printf( "Size is %lu\nPattern is: %s\nPattern size is %lu\n",data_size,pattern, pattern_size);


	clock_t end = clock();


	double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	
	printf("Time spent for reading data and stuff: %f\n",time_spent);
	begin = clock();
	int index = brute_force(data_size,pattern_size);

	end = clock();

	time_spent = (double)(end - begin) / CLOCKS_PER_SEC;


	printf("With Brute Force, Pattern found at index %d, %f time spent\n", index+1, time_spent);


	begin = clock();
	index = KMP(data_size,pattern_size);
	end = clock();
	time_spent = (double)(end - begin) / CLOCKS_PER_SEC;	
	printf("With KMP, Pattern found at index %d, %f time spent\n", index+1, time_spent);




	return 0;
}
