//----------------------------------------------------------------------------- 
// Program code for CS 415/515 Parallel Programming, Portland State University.
//----------------------------------------------------------------------------- 

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>

int* array = NULL;
int* sieve = NULL;

int N=2, P=1;  // defaults
int scnt=0;    // sieve count
int next_up=1; // starting position in sieve
int limit = 0;
pthread_mutex_t lock;
pthread_cond_t cond;

// Find sieve primes 
void find_sieves() {
  int sieve_limit = (int) floor(sqrt((double)limit));
  int* tmp = (int *) malloc(sizeof(int)*(limit));
  for (int i=0;i<limit;i++) {
    tmp[i] = i;
  }
  for (int i=2;i<=sieve_limit;i++){
    if (tmp[i]) {
      pthread_mutex_lock(&lock);
      sieve[scnt] = tmp[i];
      scnt++;
      pthread_cond_signal(&cond);
      pthread_mutex_unlock(&lock);
      for (int j=i+i; j<=limit; j=j+i){
        tmp[j] = 0;
      }
    }
  } 
  for (int i=sieve_limit+1;i<limit;i++){
    if (tmp[i] != 0) {
      pthread_mutex_lock(&lock);
      sieve[scnt] = tmp[i];
      scnt++;
      pthread_cond_signal(&cond);
      pthread_mutex_unlock(&lock);
    }
  }
  free(tmp);
  sieve[scnt] = 0;
  pthread_mutex_lock(&lock);
  pthread_cond_signal(&cond);
  pthread_mutex_unlock(&lock);
}

// Keep getting a new sieve, and mark its multiples
void worker(long tid) {
  int done = 0;
  int p = 0;
  printf("Worker[%ld] starts ...\n", tid);
  while(1) {
    pthread_mutex_lock(&lock);
    while (sieve[next_up] < 0) {
      pthread_cond_wait(&cond, &lock);
    }
    if (sieve[next_up]) {
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
      pthread_cond_broadcast(&cond);
      return; 
    }
  }
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

  // allocate space for and initalize the limit and array global arrays
  limit = (int) floor(sqrt((double)N)); 
  array = (int *) malloc(sizeof(int)*(N+1));
  for (int i=2; i<=N; i++)
    array[i] = i;

  // sieve holds primes [1..sqrt(N)] and stop flag value
  sieve = (int *) malloc(sizeof(int)*(limit+1));
  for (int i=0;i<=limit;i++) {
    sieve[i] = -1;
  } 

  // creates P-1 worker threads
  pthread_t threads[P-1];
  pthread_mutex_init(&lock, NULL);
  pthread_cond_init(&cond, NULL);
  for (long i=1;i<P;i++){
    pthread_create(&threads[i], NULL, (void*)worker, (void*)i);
  }
  
  // sleep for 10 ms to make sure workers are ready
  usleep(10000);
  
  // master finds sieves
  find_sieves();
  printf("Master found %d sieves\n", scnt);

  // master itself becomes worker 0
  worker(0);

  // master waits for all threads to finish
  for (long i=1;i<P;i++){
    printf(" ---- joining %ld ----\n", i);
    pthread_join(threads[i], NULL);
  }

  // master counts and prints the primes
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
}

