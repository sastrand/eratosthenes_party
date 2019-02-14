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
pthread_mutex_t lock;
pthread_cond_t cond;

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
      scnt++;
      sieve[scnt-1] = array[i];
    } else {
      array[i] = i;
    }
  }
  printf("-----< sieve >-----\n");
  for (int i=0;i<limit;i++) {
    printf("%d ", sieve[i]);
  }
  printf("\n-------------------\n");
}

// Keep getting a new sieve, and mark its multiples
void worker(long tid) {
  int l = tid*(N/P)+1;
  int r = l+(N/P);
  int ll = 0, rr = 0, s=0;
  printf("Worker[%ld] starts on range [%d..%d] ...\n", tid, l, r);
      for (int i=0;i<scnt;i++) {
        s = sieve[i];
      /*
        s = sieve[i];
        ll = (int) ceil(l/(double)s);
        rr = (int) floor(r/(double)s);
        printf("l  = %d\n", l);
        printf("r  = %d\n", r);
        printf("ll = %d expecting floor(%d)\n", ll, l/s);
        printf("rr = %d expecting ceil (%d)\n", rr, r/s);
        for(int j=ll*i;j<rr*i;j++){
          sieve[j] = 0;       
        }
        */

        for (int j=s+s; j<=N; j=j+s) {
          if (j >= l && j <= r) {
          array[j] = 0;
          }
        }
      }
      /*
      if (tid != 0) {
         printf("Worker[%ld] waiting\n", tid);
         pthread_mutex_lock(&lock);
         pthread_cond_wait(&cond, &lock);
         pthread_mutex_unlock(&lock);
      }
      */
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

  if (N % P != 0) {
    printf("P must evenly divide N\n");
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

  // initialize sync primitives
  pthread_t threads[P-1];
  pthread_mutex_init(&lock, NULL);
  pthread_cond_init(&cond, NULL);

  // master creates P-1 worker threads
  for (long i=1;i<P;i++){
    pthread_create(&threads[i], NULL, (void*)worker, (void*)i);
  }

  // master itself becomes worker 0
  worker(0);

  // master waits for all threads to finish
//  pthread_cond_broadcast(&cond);
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
}

