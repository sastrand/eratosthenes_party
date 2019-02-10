/*----------------------------------------------------------------------------- 
 * Sieve of Eratosthenes parallelized with OpenMP
 * Sascha Strand
 * 2019
 *---------------------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <omp.h>

int main(int argc, char **argv) {
  int N=2, P=1;

  if (argc < 2) {
    printf ("Usage: prime <upper bound> <quant threads>\n");
    exit(0);
  } else if (argc < 3) {
    printf ("Using default number of threds 1\n"); 
  }
  if ((N=atoi(argv[1])) < 2) {
    printf ("N must be greater than 2\n");
    exit(0);
  }
  if ((P=atoi(argv[2])) < 0) {
    printf("The quantity of threads must be greater than 0\n");
    exit(0);
  }

  printf("Seaching primes in [1..%d] with %d thread(s)\n", N, P);

  // Init candidate array 
  int array[N+1];
  for (int i=2; i<=N; i++)
    array[i] = 1;

  int limit = (int) sqrt((double) N);
  #pragma omp parallel num_threads(P)
  for (int i=2; i<=limit; i++)
    if (array[i]) 
      #pragma omp for schedule(static,N/(P*2))
      for (int j=i+i; j<=N; j+=i) {
        if (fst) {
          printf("W[%2d] working on prime %i (1st composite:%d)\n",
                  omp_get_thread_num(), i, j);
        }
	    array[j] = 0;
      }

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

