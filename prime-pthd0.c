//----------------------------------------------------------------------------- 
// Program code for CS 415/515 Parallel Programming, Portland State University.
//----------------------------------------------------------------------------- 

// A prime-finding program (a starting template).
//
// Usage: 
//   linux> prime-pthd0 N [P]   // P = #threads (including master)
//
// This is a naive version.
//
// 1. Master initializes a candidate array, array[N]
// 2. Master finds all sieves and saves them in an array, sieve[]
//    (Note: For this step, all markings should be confined within the sieve
//     range [2..sqrt(N)].)
// 3. Master creates P-1 threads, worker[1],...,worker[P-1]; master itself 
//    becomes worker[0] 
// 4. Each worker competes to get the next sieve from sieve[], and mark
//    its multiples in array[]
// 5. Each worker terminates itself when there is no more sieve to work on
// 6. Master waits for other workers to finish, and prints out the results
//
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <math.h>

int N=2, P=1;  // default
int scnt=0;    // sieve count

// Find sieve primes 
// (Don't perform any unnecessary marking operations)
//
void find_sieves() {

  // ... add code ...

}

// Keep getting a new sieve, and mark its multiples
//
void worker(long tid) {
  printf("Worker[%ld] starts ...\n", tid);

  // ... add code ...

  printf("-- W[%ld] working on prime %d\n", tid, p);

  // ... add code ...

  printf("Worker[%ld] done\n", tid);
}  

// The main routine
//
int main(int argc, char **argv) {
  if (argc < 2) {
    printf ("Usage: prime-pthd0 N [P]\n");
    exit(0);
  }
  if ((N=atoi(argv[1])) < 2) {
    printf ("N must be greater than 2\n");
    exit(0);
  }
  if ((argc == 3) && (P = atoi(argv[2])) < 1) {
    printf ("P must be greater than 0\n");
    exit(0);
  }

  printf("Seaching primes in [1..%d] with %d threads\n", N, P);

  // ... add code ...

  limit = (int) sqrt((double)N);

  array = (int *) malloc(sizeof(int)*(N+1));
  for (int i=2; i<=N; i++)
    array[i] = 1;

  sieve = (int *) malloc(sizeof(int)*limit);

  // Master find sieves
  find_sieves();
  printf("Master found %d sieves\n", scnt);

  // Create P-1 worker threads

  // ... add code ...

  // Master becomes worker[0]
  worker(0);

  // Wait for other P-1 worker threads to complete

  // ... add code ...

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

