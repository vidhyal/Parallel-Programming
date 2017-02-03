
// A parallel quicksort program.
//
// Usage: ./qsort-pthd <N> <numThreads>
// 
//
#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "task-queue.h"
#include <pthread.h>
#include <stdbool.h>
#include <sched.h>		// for getting cpu id
#include <unistd.h>             // for getting nprocs



#define MINSIZE   4 

        queue_t *task_queue;
        pthread_mutex_t queueLock, countLock;
        pthread_cond_t cond;
	int *array;
	
       int count, N;

// Swap two array elements 
//
void swap(int *array, int i, int j) {
  if (i == j) return;
  int tmp = array[i];
  array[i] = array[j];
  array[j] = tmp;
}



// Initialize array.
// - first generate [1,2,...,N]
// - then perform a random permutation
//
int *init_array(int N)  {
  int *array = (int *) malloc(sizeof(int) * N);
  for (int i = 0; i < N; i++) {
    array[i] = i + 1;
  }
  srand(time(NULL));
  for (int i = 0; i < N; i++) {
    int j = (rand()*1./RAND_MAX) * (N-1);
    
    swap(array, i, j);
  }
  printf("Initialized array to a random permutation of [1..%d]\n", N);
  return array;
}


// Verify that array is sorted
//
void verify_array(int *array, int N) {
  for (int i = 0; i < N-1; i++)
    if (array[i]>array[i+1]) {
      printf("FAILED: array[%d]=%d, array[%d]=%d\n", 
	     i, array[i], i+1, array[i+1]);
      return;
    }
  printf("Result verified!\n");
}



// Bubble sort for the base cases
//
void bubblesort(int *array, int low, int high) {
  if (low > high) {
  printf("returning from bubble!\n"); 
   return;
}
  for (int i = low; i <= high; i++)
    for (int j = i+1; j <= high; j++) 
      if (array[i] > array[j])
	swap(array, i, j);

pthread_mutex_lock(&countLock);
count = count + high - low+1;
//if (count ==N)
#ifdef DEBUG
printf("count = %d\n", count);
#endif
pthread_mutex_unlock(&countLock);
}

// Pick an arbitrary element as pivot. Rearrange array 
// elements into [smaller one, pivot, larger ones].
// Return pivot's index.
//
int partition(int *array, int low, int high) {
  int pivot = array[high]; 	// use highest element as pivot 
  int middle = low;
  for(int i = low; i < high; i++)
    if(array[i] < pivot) {
      swap(array, i, middle);
      middle++;
    }
  swap(array, high, middle);
  return middle;
}


void quicksort(int *array, int low, int high, long int wid) {
  if (high - low < MINSIZE) {
    bubblesort(array, low, high);
    return;
  }
  int middle = partition(array, low, high);
	pthread_mutex_lock(&countLock);
	count = count + 1;
	
	#ifdef DEBUG
		printf("count = %d\n", count);
	#endif
  pthread_mutex_unlock(&countLock);
  if (low < middle){
    task_t *newTask = create_task(low, middle-1);
    pthread_mutex_lock(&queueLock);
    add_task(task_queue, newTask);
    if (task_queue->length ==1)
	pthread_cond_signal(&cond);
    pthread_mutex_unlock(&queueLock);
	}
  if (middle < high){
    quicksort(array, middle+1, high, wid);}
}

 


void worker(long wid) {

// if queue is empty, wait, else pick the task, partition, put first half in queue , signal and process 2nd portion
	printf(" Worker %ld has started\n", wid);
        pthread_mutex_lock(&countLock);
	while (count != N) {
		#ifdef DEBUG
			printf(" not yet done %ld\n", wid);
			printf("trying to lock\n");
		#endif
                pthread_mutex_lock(&queueLock);
                pthread_mutex_unlock(&countLock);
                while (task_queue->length ==0 && count != N){
		  #ifdef DEBUG
                  printf("task queue empty\n");
                  printf("going to wait %ld\n", wid);
		  #endif
                  pthread_cond_wait(&cond, &queueLock);
		  #ifdef DEBUG
                 	printf("woke up from wait %ld\n", wid);
                  	printf(" length = %d\n", task_queue->length);
		  #endif
		}
                
		task_t *task = remove_task(task_queue);

        pthread_mutex_unlock(&queueLock);
        if (task)
			quicksort(array, task->low, task->high, wid);
		pthread_mutex_lock(&countLock);
	}
	#ifdef DEBUG
		printf("count is N\n");
	#endif
	pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&countLock);
}


// A global array of size N contains the integers to be sorted.
// A global task queue is initialized with the sort range [0,N-1].
int main(int argc, char **argv) {
	// read in command-line arguments, N and numThreads;
        int  numThreads;

	if (argc < 3) {
	    printf ("Usage: ./qsort <N> <numThreads>\n");
	    exit(0);
	}
	if ((N = atoi(argv[1])) < 2) {
	    printf ("<N> must be greater than 2\n");
	    exit(0);
	}

         numThreads = atoi(argv[2]);


	// initialize array, queue, and other shared variables
        array = init_array(N);

        // intialize task queue and add the first task
        task_queue = init_queue(0);
        pthread_mutex_init(&queueLock, NULL);
        pthread_cond_init(&cond, NULL);
	    task_t *newTask = create_task(0, N-1);
        pthread_mutex_lock(&queueLock);
        add_task(task_queue, newTask);
        pthread_mutex_unlock(&queueLock);

	// create numThreads-1 worker threads, each executes a copy
	// of the worker() routine; each copy has an integer id,
	// ranging from 0 to numThreads-2.
	//
    pthread_t thread[numThreads];
	int nprocs = sysconf(_SC_NPROCESSORS_ONLN);
	cpu_set_t cpuset;
	int cid = 0;

	for (long k = 0; k < numThreads-1; k++){
		pthread_create(&thread[k], NULL, (void*)worker, (void*)k);
        CPU_ZERO(&cpuset);
    	CPU_SET(cid++ % nprocs, &cpuset);
        pthread_setaffinity_np(thread[k], sizeof(cpu_set_t), &cpuset);

    }
	
	// the main thread also runs a copy of the worker() routine;
	// its copy has the last id, numThreads-1
	//quicksort(array, 0, N-1, numThreads-1) ;
     
	worker(numThreads-1);
	
	// the main thread waits for worker threads to join back
	for (long k = 0; k < numThreads-1; k++)
		pthread_join(thread[k], NULL);
	
	// verify the result
	verify_array(array, N);
}
