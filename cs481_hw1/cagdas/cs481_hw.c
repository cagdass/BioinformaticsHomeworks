#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

// Pattern size and text size respectively. I'm doomed if they are not sufficient.
#define PSIZE 1024*1024*10
#define TSIZE 1024*1024*200

char pattern[PSIZE];
char text[TSIZE];

/* Function signature for Bruteforce */
int bruteforce(char* t, char* p);

/* Function signatures for Knuth-Morris-Pratt */
void failure(char* pattern, int* f, int m);
int kmp(char* t, char* p);

/* Function signatures for Boyer-Moore and other requirements */
void bad_character(int* bc, char* pat, int patlen);
int is_prefix(char* word, int length, int pos);
void good_suffix(int* gs, char* p, int pl);
int boyer_moore (char* t, int tl, char* p, int pl);
// ARRAY_LENGTH for the bad character table in Boyer-Moore
#define ARRAY_LENGTH 256
#define max(a, b) ((a < b) ? b : a)
int bc[ARRAY_LENGTH];

/* Function signatures for Rabin-Karp and other requirements */
// One hash function when pattern length <= 32, the other when not.
long long hash1 (char* s, int slen);
int rabin_karp(char* t, int tl, char* p, int pl);
// Lookup table for the values of
/*
  'A' = 0,
  'C' = 1,
  'G' = 2,
  'T' = 3 */
char lookup[ARRAY_LENGTH];
// Bit shifting when finding the hash of the shifted substring of the text.
#define shift1(x, cur, and) (((x << 2) & and) | cur)
#define shift2(x, cur) ((x << 2) | cur)

int main(int argc, char const *argv[]) {
  if (argc < 3) {
    printf("\n<usage>: ./cs481_hw1 [text_file_path] [pattern_file_path]\n");
    return -1;
  }

  // File locations.
  char* tf = (char*)argv[1];
  char* pf = (char*)argv[2];

  FILE *pfile;
  FILE *tfile;
  size_t nread;
  pfile = fopen(pf, "r");
  if (pfile) {
      while ((nread = fread(pattern, 1, sizeof pattern, pfile)) > 0)
      if (ferror(pfile)) {
          printf("Error reading pattern file\n");
      }
      fclose(pfile);
  }

  tfile = fopen(tf, "r");
  if (tfile) {
      while ((nread = fread(text, 1, sizeof text, tfile)) > 0)
      if (ferror(tfile)) {
          printf("Error reading text file\n");
      }
      fclose(tfile);
  }

  // Replace the new line character if there is one at the end of the files.
  /*
  int tlen = strlen(text);
  if (text[tlen-1] == '\n')
    text[tlen-1] = '\0';
  int plen = strlen(pattern);
  if (pattern[plen-1] == '\n')
    pattern[plen-1] = '\0';
  */

  clock_t starts[4];
  clock_t ends[4];
  int match = -1;
  double time_spent;

  starts[0] = clock();
  match = bruteforce(text, pattern);
  ends[0] = clock();
  printf("##### Bruteforce #####\n");
  if (match != -1) {
    printf("Match at position %d\n", match);
  }
  else {
    printf("Not found\n");
  }

  time_spent = (double)(ends[0] - starts[0]) / CLOCKS_PER_SEC;
  printf("Runtime: %f\n", time_spent);

  starts[1] = clock();
  match = kmp(text, pattern);
  ends[1] = clock();
  printf("##### Knuth-Morris-Pratt #####\n");
  if (match != -1) {
    printf("Match at position %d\n", match);
  }
  else {
    printf("Not found\n");
  }

  time_spent = (double)(ends[1] - starts[1]) / CLOCKS_PER_SEC;
  printf("Runtime: %f\n", time_spent);

  starts[2] = clock();
  match = boyer_moore(text, strlen(text), pattern, strlen(pattern));
  ends[2] = clock();
  printf("##### Boyer-Moore #####\n");
  if (match != -1) {
    printf("Match at position %d\n", match);
  }
  else {
    printf("Not found\n");
  }

  time_spent = (double)(ends[2] - starts[2]) / CLOCKS_PER_SEC;
  printf("Runtime: %f\n", time_spent);

  starts[3] = clock();
  match = rabin_karp(text, strlen(text), pattern, strlen(pattern));
  ends[3] = clock();
  printf("##### Rabin-Karp #####\n");
  if (match != -1) {
    printf("Match at position %d\n", match);
  }
  else {
    printf("Not found\n");
  }

  time_spent = (double)(ends[3] - starts[3]) / CLOCKS_PER_SEC;
  printf("Runtime: %f\n", time_spent);

  return 0;
}

//////////////////// BRUTEFORCE ////////////////////
int bruteforce(char* t, char* p) {
  int i;

  // Pattern length.
  int pl = strlen(p);

  // Run while null terminator has not been encountered.
  for (i = 0; t[i] != '\0'; i++) {
    // Start alignment with the pattern if pattern's first character is a match.
    if (t[i] == p[0]) {
      int j;
      char found = 1;
      for (j = 1; j < pl; j++) {
        if (t[i+j] != p[j]) {
          found = 0;
          break;
        }
      }

      // Return position if the inner loop terminated without breaking.
      if (found == 1) {
        return i + 1;
      }
    }
  }

  // Return not found.
  return -1;
}
////////////////////////////////////////////////////////////

//////////////////// Knuth-Morris-Pratt ////////////////////
int kmp(char* t, char* p) {
  int m = strlen(p);
  int* f = (int*)malloc(sizeof(int) * m);
  failure(p, f, m); // Failure function values.

  int i = 0;
  int j = 0;
  // Stop when null terminator found, no strlen this way.
  while (t[i] != '\0') {
    if (t[i] == p[j]) {
      // Match found.
      if (j == m - 1) {
        free(f);
        return i - j + 1;
      }
      // Continue aligning the pattern and the text.
      else {
        i += 1;
        j += 1;
      }
    }
    else {
      // Look at the character that came before, shift the pattern relative to the text.
      if (j > 0) {
        j = f[j-1];
      }
      else {
        // Shift the text in this case.
        i += 1;
      }
    }
  }

  free(f);
  return -1;
}

void failure(char* p, int* f, int m) {
  f[0] = 0;
  int i = 1;
  int j = 0;

  while (i < m) {
    if (p[i] == p[j]) {
      f[i] = j + 1; // j+1 matches up to the current character.
      i += 1;
      j += 1;
    }
    else if (j > 0) {
      j = f[j - 1];
    }
    else {
      f[i] = 0;
      i += 1;
    }
  }
}
////////////////////////////////////////////////////////////

//////////////////// Boyer-Moore ////////////////////
// Since the alphabet is {A, C, G, T}, when they are found the search is broken.
void bad_character(int bc[], char* pat, int patlen) {
  int i;
  // How many letters of our alphabet have been encountered so far.
  int count = 0;

  // Initialize the array indices for the letters of our alphabet.
  // Being A, C, G, and T respectively.
  bc[65] = 0;
  bc[67] = 0;
  bc[71] = 0;
  bc[84] = 0;

  for (i = patlen - 1; count < 3 && i > 0; i--) {
    if (bc[(int)pat[i]] == 0) {
      bc[(int)pat[i]] = patlen - 1 - i;
      ++count;
    }
  }
}

void find_suffix(char *p, int pl, int *suffixes) {
   int i, j, k;

   // Initialize to no suffix.
   suffixes[pl - 1] = pl;
   j = pl - 1;

   for (i = pl - 2; i >= 0; --i) {
      if (i > j && suffixes[i + pl - 1 - k] < i - j)
         // There is not a longer suffix than the previous suffix.
         suffixes[i] = suffixes[i + pl - 1 - k];
      else {
         // Suffixes do not necessarily end at the last position of the pattern.
         if (i < j)
            j = i;
         k = i;
         // Increase the suffix length while the characters keep matching.
         while (j >= 0 && p[j] == p[j + pl - 1 - k])
            --j;
         suffixes[i] = k - j;
      }
   }
}

void good_suffix(int* gs, char *p, int pl) {
  int i, j;
  int* suffixes = (int*)malloc(sizeof(int) * pl);

  find_suffix(p, pl, suffixes);
  // Initialize all positions with minimum shift amount.
  for (i = 0; i < pl; ++i) {
  gs[i] = pl;
  }

  j = 0;
  for (i = pl - 1; i >= 0; --i) {
    if (suffixes[i] == i + 1) {
      for (; j < pl - 1 - i; ++j) {
        if (gs[j] == pl){
          // Update to a larger shift amount.
          gs[j] = pl - 1 - i;
        }
      }
    }
  }

  // Update according to the
  for (i = 0; i <= pl - 2; ++i){
    gs[pl - 1 - suffixes[i]] = pl - 1 - i;
  }

  free(suffixes);
}


int boyer_moore(char* t, int tl, char* p, int pl) {
   int i, j, bc[ARRAY_LENGTH];
   int* gs = (int*)malloc(sizeof(int) * pl);

   /* Preprocessing */
   good_suffix(gs, p, pl);
   bad_character(bc, p, pl);

   /* Searching */
   j = 0;
   while (j <= tl - pl) {
      for (i = pl - 1; i >= 0 && p[i] == t[i + j]; --i);
      if (i < 0) {
        // Free dynamically allocated array.
        free(gs);
        return j + 1;
      }
      else
        // Shift j according to which table has the maximum shift amount.
         j += max(gs[i], bc[(int)t[i + j]] - pl + 1 + i);
   }

   // Free dynamically allocated array.
   free(gs);
   return -1;
}
////////////////////////////////////////////////////////////

//////////////////// Rabin-Karp ////////////////////
long long hash1 (char* s, int slen) {
  int i;
  long long result = 0;
  for (i = 0; i < slen; i++) {
    result = (result << 2) | lookup[(int)s[i]];
  }

  return result;
}

/* Could have made this type-inspecific such as
  if (pattern length <= 8) { type is int } or long for 16
  But
 */
int rabin_karp(char* t, int tl, char* p, int pl) {
  lookup[65] = 0;
  lookup[67] = 1;
  lookup[71] = 2;
  lookup[84] = 3;

  if (pl <= 32) {
    // Hash of the pattern.
    long long hash_p = hash1(p, pl);
    // Hash of the text.
    long long hash_tt = hash1(t, pl);

    // Value consisting of all 1's, to and the shifted value with.
    unsigned long int and = (unsigned long int) -1;
    // Shift the and value to have only as many 1's as twice the pattern length.
    and = and >> (64 - 2 * pl);

    int i;

    for (i = 0; i < tl - pl + 1; i++) {
      if (hash_tt == hash_p) {
        return i + 1;
      }

      // short c = getIntEq(t[i + pl]);
      hash_tt = shift1(hash_tt, lookup[(int)t[i + pl]], and);
    }
  }
  else {
    long long hash_p = hash1(p, pl);
    long long hash_tt = hash1(t, pl);
    int i;
    for (i = 0; i < tl - pl + 1; i++) {

      if (hash_tt == hash_p) {
        int j;
        int f = 1;
        // Bruteforce checking if pl > 32 and hashes match.
        // Could not be arsed to do a better implementation.
        // Attempt to do loop unrolling
        for (j = 0; j < pl; j++) {
          if (t[i + j] != p[j]) {
            f = 0;
            break;
          }
        }
        if (f == 1) {
          return i + 1;
        }
      }

      hash_tt = shift2(hash_tt, lookup[(int)t[i + pl]]);
    }
  }

  return -1;
}
