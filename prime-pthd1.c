//----------------------------------------------------------------------------- 
// Program code for CS 415/515 Parallel Programming, Portland State University.
//----------------------------------------------------------------------------- 

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <math.h>

/*
struct arg_struct{
  long tid;
  int* sieve;
  int* array;
};
*/

int* array = NULL;
int* sieve = NULL;

int N=2, P=1;  // defaults
int scnt=0;    // sieve count
int next_up=1; // starting position in sieve
int limit = 0;
//int limit = (int) floor(sqrt((double)N)); 
pthread_mutex_t lock;
pthread_cond_t cond;

// Find sieve primes 
void find_sieves(int limit, int* array, int* sieve) {
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
  sieve[limit] = -1;
}

// Keep getting a new sieve, and mark its multiples
void worker(long tid) {
/*
  struct arg_struct *args = arguments;
  long tid = args->tid;
  int* sieve = args->sieve;
  int* array = args->array;
*/
  int done = 0;
  int p = 0;
  printf("Worker[%ld] starts ...\n", tid);
  while(1) {
    pthread_mutex_lock(&lock);
    if (next_up != -1) {
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
          }
        }
      }
    } else {
      printf("---- W[%ld] done ----\n", tid);
      return; 
    }
  }


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

  printf("Seaching primes in [1..%d] with %d thread(s)\n", N, P);

  // ... add code ...
  //
  limit = (int) floor(sqrt((double)N)); 
  array = (int *) malloc(sizeof(int)*(N+1));
  for (int i=2; i<=N; i++)
    array[i] = i;

  // sieve holds primes [1..sqrt(N)] and stop flag value
  sieve = (int *) malloc(sizeof(int)*(limit+1));

  // Master find sieves
  find_sieves(limit, array, sieve);
  printf("Master found %d sieves\n", scnt);

  // Create arguments to pass to worker threads
  /*
  struct arg_struct all_args[P-1];
  for (int i=0;i<P;i++) {
    all_args[i].tid = i;
    all_args[i].sieve = sieve;
    all_args[i].array = array;
  }
  */

  // Create P-1 worker threads
  pthread_t threads[P-1];
  pthread_mutex_init(&lock, NULL);
  pthread_cond_init(&cond, NULL);
  for (long i=1;i<P;i++){
    //pthread_create(&threads[i], NULL, (void*)worker, (void*)&all_args[i]);
    pthread_create(&threads[i], NULL, (void*)worker, (void*)i);
  }

  // master itself becomes worker 0
  worker(0);

  
  

/*
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
*/
}

