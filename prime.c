//----------------------------------------------------------------------------- 
// Program code for CS 415/515 Parallel Programming, Portland State University.
//----------------------------------------------------------------------------- 

// A prime-finding program (Sequential version).
//
// Usage: 
//   linux> prime N
//
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

int main(int argc, char **argv) {
  int N=2;

  if (argc < 2) {
    printf ("Usage: prime N\n");
    exit(0);
  }
  if ((N=atoi(argv[1])) < 2) {
    printf ("N must be greater than 2\n");
    exit(0);
  }

  printf("Seaching primes in [1..%d]\n", N);

  // Init candidate array 
  int array[N+1];
  for (int i=2; i<=N; i++)
    array[i] = 1;

  // Main loop:
  // 1. find sieves within [2..sqrt(N)]
  // 2. for each sieve, mark its multiples
  int limit = (int) sqrt((double) N);
  for (int i=2; i<=limit; i++)
    if (array[i]) 
      for (int j=i+i; j<=N; j+=i) 
	array[j] = 0;

  // Count and print the primes
  #define min(x,y) (((x)<(y)) ? (x) : (y))
  int cnt = 0;
  for (int i=2; i<=N; i++)
    if (array[i]==1)
      cnt++;
  printf("Found %d primes in [1..%d]: ", cnt, N);
  for (int i=2; i<=min(N,100); i++)
    if (array[i]==1)
      printf("%d, ", i);
  printf("...\n");

}

