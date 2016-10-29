/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.c
 * Author: baraaorabi
 *
 * Created on October 27, 2016, 1:36 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <stdio.h>
#include <math.h>
#include <stdio.h>

char* pattern;
int pattern_length;
char* text;
int text_length;
int primes[] = {101, 1009, 10007, 100003, 1000003, 10000019, 100000007, 10000000019};
int baseToNum(char n);

int initialize(char*,char*);
void brute_force();
void KNP();
void BM();
void get_suffixes(char *x, int m, int *suff);
void get_Z(char* x, int m, int *Z);
void RK ();


int main(int argc, char** argv) {
    char* text_file_path = argv[1];
    char* pattern_file_path = argv[2];
    printf("%s : %s\n", text_file_path, pattern_file_path);
    if (initialize(text_file_path, pattern_file_path) == -1){
        return -1;
    }
    
    brute_force();
    //KNP();
    //BM();
    //RK();
    return (EXIT_SUCCESS);
}

void RK(){
    int q;
    q = 2;
    int i;
    i = 0;
    while (pattern_length > primes[i]) i++;
    q = primes[i];
    int c;
    c = 1;
    
    for (i = 0; i < pattern_length-1; i++)
        c = (c*4)%q;
    
    int Fp, Ft;
    Fp = 0;
    Ft = 0;
    for (i = 0; i < pattern_length; i++){
        Fp = (4*Fp + baseToNum(pattern[i])) % q ;
        Ft = (4*Ft + baseToNum(text[i])) % q ;
    }

    int j;
    for (j = 0; j <= text_length - pattern_length; j++){
        if (Fp == Ft) {
            for (i = 0; i < pattern_length && pattern[i]==text[j+i]; i++);
            if (i == pattern_length) {
                printf("RK found match at: %d\n", j +1);
                return;
            }            
        }
        if(i < text_length - pattern_length){
            Ft = (4*(Ft - baseToNum(text[j])*c)+ baseToNum(text[j + pattern_length])) % q;
            if (Ft < 0) Ft = Ft +q;
        }
        
    }
    
    printf("RK: no match found\n");
    
}

int baseToNum(char n){
    switch (n){
        case 'A':
            return 0;
            break;
        case 'C':
            return 1;
            break;
        case 'G':
            return 2;
            break;
        case 'T':
            return 3;
            break;
    }
    return -1;
}

void BM(){
    int* B[4];
    
    B[0] = malloc(sizeof(int)*pattern_length);
    B[1] = malloc(sizeof(int)*pattern_length);
    B[2] = malloc(sizeof(int)*pattern_length);
    B[3] = malloc(sizeof(int)*pattern_length);
    
    switch (pattern[0]){
        case 'A':
            B[0][0] = 0;
            B[1][0] = -1;
            B[2][0] = -1;
            B[3][0] = -1;
            break;
        case 'C':
            B[0][0] = -1;
            B[1][0] = 0;
            B[2][0] = -1;
            B[3][0] = -1;
            break;
        case 'G':
            B[0][0] = -1;
            B[1][0] = -1;
            B[2][0] = 0;
            B[3][0] = -1;
            break;
        case 'T':
            B[0][0] = -1;
            B[1][0] = -1;
            B[2][0] = -1;
            B[3][0] = 0;
            break;
    }

    
    int i;
    for (i = 1; i < pattern_length; i++){
        switch (pattern[i]){
            case 'A':
                B[0][i] = i;
                B[1][i] = B[1][i-1];
                B[2][i] = B[1][i-1];
                B[3][i] = B[1][i-1];
                break;
            case 'C':
                B[0][i] = B[1][i-1];
                B[1][i] = i;
                B[2][i] = B[1][i-1];
                B[3][i] = B[1][i-1];
                break;
            case 'G':
                B[0][i] = B[1][i-1];
                B[1][i] = B[1][i-1];
                B[2][i] = i;
                B[3][i] = B[1][i-1];
                break;
            case 'T':
                B[0][i] = B[1][i-1];
                B[1][i] = B[1][i-1];
                B[2][i] = B[1][i-1];
                B[3][i] = i;
                break;
        }
    }
    
    
    
    int* suff;
    suff = malloc(sizeof(int)*pattern_length);
    get_suffixes(pattern, pattern_length, suff);
    
    
    int* bmGs;
    bmGs = malloc(sizeof(int)*pattern_length);
    
    for (i = 0; i < pattern_length; i++){
        bmGs[i] = pattern_length;
    }
    
    int j;
    j = 0;
    for (i = 0; i < pattern_length; ++i)
        bmGs[i] = pattern_length;
    j = 0;
    for (i = pattern_length - 1; i >= 0; --i){
        if (suff[i] == i + 1){
            for (; j < pattern_length - 1 - i; ++j){
                if (bmGs[j] == pattern_length){
                    bmGs[j] = pattern_length - 1 - i;
                }
            }
        }
    }
    
    for (i = 0; i <= pattern_length - 2; ++i){
        bmGs[pattern_length - 1 - suff[i]] = pattern_length - 1 - i;   
    }
    
    

    j = 0; i = pattern_length - 1;
    
    while (j <= text_length - pattern_length) {        
        for (i = pattern_length - 1; i >= 0 && text[j + i] == pattern[i]; --i) ;
        if (i < 0) {
            printf("BM found match at : %d\n", j+1);
            return;
        }
        else{
            int Bc_shift = 0;
            switch(text[j]){
                case 'A':
                    Bc_shift = B[0][i];
                    break;
                case 'C':
                    Bc_shift = B[1][i];
                    break;
                case 'G':
                    Bc_shift = B[2][i];
                    break;
                case 'T':
                    Bc_shift = B[3][i];
                    break;
            }
            j += (bmGs[i] > Bc_shift ? bmGs[i] : bmGs[i] );
        }
    }
    
    printf("BM: No match found\n");

            
}

void get_suffixes(char *x, int m, int *suff) {
    char* x_rev;
    x_rev = malloc(sizeof(char)*m);
    
    int* Z;
    Z = malloc(sizeof(int)*m);
    int i;
    for (i=0;i < m; i++){
        x_rev[i] = x [m-1-i];
    }
    
    
    get_Z(x_rev, m, Z);
    
    for(i = 0; i < m; i++){
        suff[i] = Z[m - 1 - i];;
    }
    
}

void get_Z(char* x, int m, int *Z) {
    int L, R, k;
    
    
    L = R = 0;
    for (int i = 1; i < m; ++i) {
        if (i > R) {
            L = R = i;

            while (R<m && x[R-L] == x[R])
                R++;
            Z[i] = R-L;
            R--;
        }
        else {
            k = i-L;
            if (Z[k] < R-i+1)
                 Z[i] = Z[k];

            else {
                L = i;
                while (R<m && x[R-L] == x[R])
                    R++;
                Z[i] = R-L;
                R--;
            }
        }
    }
    Z[0] = m;
}

void KNP(){
    int* F;
    
    F = malloc(sizeof(int)*pattern_length);
    F[0] = 0;
   
    int i,j;
    i = 1;
    j = 0;
    
    while(i < pattern_length){
        if (pattern[i]==pattern[j]){
            F[i] = j+1;
            i++;
            j++;
        } else if (j > 0){
            j = F[j-1];
        } else {
            F[i] = 0;
            i++;
        }
        
    }
    
    //for (i = 0; i < pattern_length; i++) printf("%d |", F[i]);
    
    i = 0;
    j = 0;
    int iter;
    while (i<text_length ){
        if (text[i] == pattern[j]){
            if (j == pattern_length -1){
                printf("KNP found a match at: %d\n", i-j+1);
                return;
            } else {
                j++;
                i++;
            }
            
        } else {
            if (j>0) {
                j = F[j-1];
            } else {
                i++;
            }
        }
    }
    
    printf("KNP: No match found\n");
}

void brute_force(){
    int i, j;

    for(i = 0; i < text_length ; i++){
        
        for (j = 0; j < pattern_length; j++){
            if (text[i + j] != pattern[j])
                break;
            else if (j == pattern_length-1){
                printf("Brute Force found a match at: %d\n", i+1);
                return;
            }
                
        }
        
    }
    
    printf("Brute Force: No match found.\n");
}

int initialize(char* text_file_path, char* pattern_file_path){
    struct stat st1;
    
    if (stat(text_file_path, &st1) != 0)
        return -1;
    text_length = st1.st_size;
    
    
    if (stat(pattern_file_path, &st1) != 0)
        return -1;
    pattern_length = st1.st_size;
    
    printf("%d : %d\n", text_length, pattern_length);
    FILE* pattern_file;
    FILE* text_file; 
    
    text_file = fopen(text_file_path, "r");
    pattern_file = fopen(pattern_file_path, "r");

    text = malloc(text_length);
    pattern = malloc(pattern_length);
    
    
    printf("%d :", (int)fread(text, 1, text_length, text_file));
    printf("%d\n", (int)fread(pattern, 1, pattern_length, pattern_file));
    
    
    
    if (text[text_length-1] < 'A')
        printf("%d\n", --text_length);
    if (pattern[pattern_length-1] < 'A')
        printf("%d\n", --pattern_length);
    
    /*int i;
    
    for(i = 0; i < text_length; i++)
        printf("%c", text[i]);
    printf(".\n");
            
    for(i = 0; i < pattern_length; i++)
        printf("%c", pattern[i]);
    printf(".\n");
*/
    return 1;
}