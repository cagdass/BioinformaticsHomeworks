#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <limits.h>

#include "primetable.h"

#define MAX(a,b) (((a)>(b))?(a):(b))

#define _SUPER_MAGIC_NUMBER 8
#define _MAGICAL_MYSTERY_MASK 0X07
#define _ALPHABET_SIZE 4
#define _SIZE 400000000
#define _PATTERN_SIZE 10000

char buffer[_SIZE+1];
char pattern[_PATTERN_SIZE+1] = {0};

char alphabet[_ALPHABET_SIZE] = {'A','C','G','T'};
char alphabetun[_ALPHABET_SIZE] = {'a','c','g','t'};
// HMM I need some kind of hashing for char->index
// I dont want to create an array of size 90 for 4 chars
// Lets see...
// Our chars are the following: 65 84 67 71 or for lc  97 116 99 103 
// we cant use % 4 because C%4==G%4
// UC % 8 -> 1 3 7 4, lc % 8 -> 1 3 7 4
// WOW how convenient
int indexer[_SUPER_MAGIC_NUMBER] = { -37, 0, -37, 1, 3, -37, -37, 2};
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
		if(j==psize)return i;
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
			_fail[i]=j+1;
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

int _badchr[_ALPHABET_SIZE];	//Last Occurance
int _goodsfx0[_PATTERN_SIZE+1];	//Good Suffix
int _goodsfx1[_PATTERN_SIZE+1];

int BM(size_t dsize, size_t psize){

	int i;
	char tmp;
	//Init bad character here
	for(i=0;i<_ALPHABET_SIZE;i++){
		_badchr[i]=-1;
	}
	for(i=0;i<psize-1;i++){
		_badchr[indexer[fasthash(pattern[i])]] = i;
	}
		
	//Init suffixes here
	i=psize;
	int j = psize+1;
	_goodsfx0[i]=j;
	
	while(i>0){
		while(j<=psize && pattern[i-1]!=pattern[j-1]){
			if(_goodsfx1[j]==0) _goodsfx1[j]=j-i;
			j=_goodsfx0[j];
		}
		i--;
		j--;
		_goodsfx0[i]=j;
	}
	
	j=_goodsfx0[0];

	for(i=0;i<psize+1;i++){
		if(_goodsfx1[i]==0) _goodsfx1[i]=j;
		if(i==j) j=_goodsfx0[j];
	}

	i=0;
	while(i<=dsize-psize){
		j=psize-1;
		while(j>=0 && pattern[j]==buffer[i+j])j--;
		if(j<0){
			return i;
		}
		else{
			i+=MAX(_goodsfx1[j+1], j-_badchr[indexer[fasthash(buffer[i+j])]]);
		}
	}
		

	return -1;
}


int overflowpow( unsigned int num, int power){
	if(power==1)return num;
	if(power==2)return (num*num);
	unsigned int p = overflowpow(num,power/2);
	if(power&0X0001){
		return (p*p*num);
	}
	return (p*p);


}

int modpow(int num, int power, int mod){

	if(power==1)return num;
	if(power==2)return (num*num)%mod;
	int p = modpow(num,power/2,mod);
	if(power&0X0001){
		return (p*p*num)%mod;
	}
	return (p*p)%mod;

}

int findprime(int num){
	
	double root;
	int limit0 = 0;
	int limit1 = PRIME_COUNT;
	int current = PRIME_COUNT/2;
	while(1){
		
		if(primetable[current]>num)
		{
			
			root = sqrt((double)primetable[current]);
			if(primetable[current]-num<=root){return primetable[current];}
			limit1 = current;
			current= (current+limit0)/2;

		}
		else{
			limit0 = current;
			current = (current+limit1)/2;
		}
	}
}


#define _RKSHIFT_AMOUNT 2 //(log2(_ALPHABET_SIZE))
int RK(size_t dsize, size_t psize){
//	int q = findprime(psize);//O(logn) prime finder
	unsigned int c = overflowpow(_ALPHABET_SIZE, psize-1);
//	printf("q=%d,c=%d\n",q,c);
	unsigned int fp = 0;
	unsigned int ft = 0;
	int i;
	for( i=0; i<psize;i++){
		fp=((fp<<_RKSHIFT_AMOUNT)+indexer[fasthash(pattern[i])]);
		ft=((fp<<_RKSHIFT_AMOUNT)+indexer[fasthash(buffer[i])]);
	}	

	for( i=0; i<dsize-psize+1;i++){
		if(fp==ft){
	//		printf("%d\n",i);
			if(memcmp(&pattern,&buffer[i],psize)==0){
				return i;
			}
		}
		ft= (((ft - indexer[fasthash(buffer[i])]*c)<<_RKSHIFT_AMOUNT) + indexer[fasthash(buffer[i+psize])]);
	}
	return -1;
}

int maintest(){
	int i;

	
	printf("for %d, prime is %d\n",2396,findprime(2396));

	return 0;
}

void callSearch( char* name, int (*search)(size_t, size_t), int dsize, int psize){
	clock_t begin = clock();
	clock_t end = clock();
	

	double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	

	begin = clock();
	int index = (*search)(dsize,psize) + 1;

	end = clock();

	time_spent = (double)(end - begin) / CLOCKS_PER_SEC;


	printf( "With %s: \n",name);
	if(index)
		printf("\tPattern found at index %d, %f time spent\n", index, time_spent);
	else
		printf("\tPattern does not exist in the text. %f secs wasted\n", time_spent);

}


int main(int argc, char** argv){
	if(argc!=3){
		printf("Something went wrong you need 2 arguments not %d\n",argc-1);
		return -1;
	}
	
	clock_t begin = clock();
	FILE* fptr = fopen(argv[1], "r");


	size_t data_size = fread(buffer, sizeof(char), _SIZE , fptr );


	if(buffer[data_size-1]=='\n'){data_size--;}//I cant produce data without \n with text editors	
	fclose(fptr);
	fptr = fopen(argv[2],"r");

	size_t pattern_size = fread(pattern,sizeof(char), _PATTERN_SIZE,fptr);

	if(pattern[pattern_size-1]=='\n'){pattern_size--;}	
	printf( "Size is %lu\nPattern is: %s\nPattern size is %lu\n",data_size,pattern, pattern_size);


	clock_t end = clock();


	double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	
	printf("Time spent for reading data and stuff: %f\n",time_spent);
	begin = clock();

	callSearch("Brute Force",brute_force,data_size,pattern_size);
	callSearch("KMP",KMP,data_size,pattern_size);
	callSearch("Boyer Moore",BM,data_size,pattern_size);
	callSearch("Rabin Karp",RK,data_size,pattern_size);
/*
	int index = brute_force(data_size,pattern_size);

	end = clock();

	time_spent = (double)(end - begin) / CLOCKS_PER_SEC;


	printf( "With Brute Force: \n");
	if(index)
		printf("With Brute Force, Pattern found at index %d, %f time spent\n", index+1, time_spent);
	else
		printf("With Brute Force

	begin = clock();
	index = KMP(data_size,pattern_size);
	end = clock();
	time_spent = (double)(end - begin) / CLOCKS_PER_SEC;	
	printf("With KMP, Pattern found at index %d, %f time spent\n", index+1, time_spent);

	begin = clock();
	index = BM(data_size,pattern_size);
	end = clock();
	time_spent = (double)(end - begin) / CLOCKS_PER_SEC;	
	printf("With BM, Pattern found at index %d, %f time spent\n", index+1, time_spent);
	
	begin = clock();
	index = RK(data_size,pattern_size);
	end = clock();
	time_spent = (double)(end - begin) / CLOCKS_PER_SEC;	
	printf("With RK, Pattern found at index %d, %f time spent\n", index+1, time_spent);
	

*/
	return 0;
}
