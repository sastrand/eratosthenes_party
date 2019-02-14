//----------------------------------------------------------------------------- 
// Program code for CS 415/515 Parallel Programming, Portland State University.
//----------------------------------------------------------------------------- 

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <math.h>

int* array = NULL;
int* sieve = NULL;

int N=2, P=1;  // defaults
int scnt=0;    // sieve count
int next_up=1; // starting position in sieve
int limit = 0;
int* composites_found;
pthread_mutex_t lock;

// Find sieve primes 
void find_sieves() {
  int sieve_limit = (int) floor(sqrt((double)limit));
  for (int i=2;i<=sieve_limit;i++){
    if (array[i]) {
      for (int j=i+i; j<=limit; j=j+i){
        array[j] = 0;
      }
    }
  }
  for (int i=0;i<limit;i++){
    if (array[i] != 0) {
      sieve[scnt] = array[i];
      scnt++;
    } else {
      array[i] = i;
    }
  }
  sieve[scnt+1] = 0;
}

// Keep getting a new sieve, and mark its multiples
void worker(long tid) {
  int done = 0;
  int p = 0;
  printf("Worker[%ld] starts ...\n", tid);
  while(1) {
    pthread_mutex_lock(&lock);
    if (sieve[next_up]) {
      printf("       next up = %d\n", next_up);
      p = sieve[next_up];
      next_up++;
    } else {
      done = 1;
    }
    pthread_mutex_unlock(&lock);
    if (!done) {
      printf("-- W[%ld] working on prime %d\n", tid, p);
      for (int i=2; i<=limit; i++) {
        if (array[i]) {
          for (int j=i+i; j<=N; j+=i) {
            array[j] = 0;
            composites_found[tid]++;
          }
        }
      }
    } else {
      printf("---- W[%ld] done ----\n", tid);
      return; 
    }
  }
  printf("Worker[%ld] done\n", tid);
}  

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

  printf("Seaching primes in [1..%d] with %d thread(s)\n", N, P);

  // allocate space for and initalize the limit and array global arrays
  limit = (int) floor(sqrt((double)N)); 
  array = (int *) malloc(sizeof(int)*(N+1));
  for (int i=2; i<=N; i++)
    array[i] = i;

  // sieve holds primes [1..sqrt(N)] and stop flag value
  sieve = (int *) malloc(sizeof(int)*(limit+1));

  // master finds sieves
  find_sieves();
  printf("Master found %d sieves\n", scnt);

  printf("array\n");
  for (int i=0;i<N;i++) {
    printf("%d ", array[i]);
  }
  printf("sieve\n");
  for (int i=0;i<=limit;i++) {
    printf("%d ", sieve[i]);
  }

  // Create P-1 worker threads
  pthread_t threads[P-1];
  composites_found = (int *) malloc(sizeof(int)*P);
  pthread_mutex_init(&lock, NULL);
  for (long i=1;i<P;i++){
    pthread_create(&threads[i], NULL, (void*)worker, (void*)i);
    composites_found[i]=0;
  }

  // master itself becomes worker 0
  worker(0);

  // master waits for all threads to finish
  for (long i=1;i<P;i++){
    printf(" ---- joining %ld ----\n", i);
    pthread_join(threads[i], NULL);
  }

  // Count and print the primes
  #define min(x,y) (((x)<(y)) ? (x) : (y))
  int cnt = 0;
  for (int i=2; i<=N; i++)
    if (array[i])
      cnt++;
  printf("Found %d primes in [1..%d]: ", cnt, N);
  for (int i=2; i<=min(N,100); i++)
    if (array[i])
      printf("%d, ", i);
  printf("...\n");
  int sum = 0;
  for (int i=0;i<P;i++){
    printf("Thread[%d]:%d\n", i, composites_found[i]);
    sum = sum + composites_found[i];
  }
  printf("Total: %d\n", sum);
}

