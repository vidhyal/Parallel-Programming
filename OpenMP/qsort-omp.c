//------------------------------------------------------------------------- 
// This is supporting software for CS415/515 Parallel Programming.
// Copyright (c) Portland State University.
//------------------------------------------------------------------------- 

// A sequential quicksort program.
//
// Usage: ./qsort <N>
// 
//
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <omp.h>

#define MINSIZE   2		// threshold for switching to bubblesort
int numThreads=200;

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
  if (low >= high) 
    return;
  for (int i = low; i <= high; i++)
    for (int j = i+1; j <= high; j++) 
      if (array[i] > array[j])
	swap(array, i, j);
}


// code for parallel partition
/*
//par Scan
 
int parScanUp(int *arr, int *t, int i, int j){
  if (i==j)
     return arr[i];
  else{
    int k = (int)((i+j)/2);
    int right=0;
    #pragma omp parallel sections num_threads(numThreads)
    {
    #pragma omp section
    t[k] = parScanUp(arr, t, i, k);
    #pragma omp section
    right = parScanUp(arr, t, k+1, j);
    }   
    return t[k] + right;
    
  }
}

void parScanDown(int v, int * arr, int * t, int *y, int i, int j){
  if(i==j){
    y[i] = v+arr[i];
  }
  else{
    int k= (int)((i+j)/2);
    #pragma omp parallel num_threads(numThreads)
    #pragma omp sections
    {
      #pragma omp section
        parScanDown(v,arr,t,y,i,k);
      
      #pragma omp section
        parScanDown(v+t[k], arr, t, y, k+1,j);
      
    }
  }
}




void parScan(int *arr, int n, int * y){
  int t[n];
  y[0] = arr[0];
  if (n>1){
    parScanUp(arr, t, 1, n-1);
    parScanDown(arr[0], arr, t, y, 1, n-1);
  }
}

int partition(int *array, int low, int high){
  int n = high-low+1;
  if (n==1){
     return low;}
  int pivot = array[high];
  int less[n], great[n], aux[n], y1[n], y2[n];
  //int less_n = 0, great_n =0;
  int i, j;

  #pragma omp parallel for num_threads(numThreads)
    for (i=0; i<n; i++){
      aux[i] = array[low+i];
      if (aux[i] <= pivot){
        less[i] =1;
        great[i] =0;
      }
      else{
        less[i] =0;
        great[i] =1;
      }
    }
    parScan(less, n, y1);
    parScan(great, n, y2);
    int k= low +y1[n-2];
    array[k] = pivot;

  #pragma omp parallel for num_threads(numThreads)
    for(i =0; i<n; i++){
      if (aux[i] <=pivot){
        array[low+ y1[i]-1] = aux[i];
      }
      else{
        array[k+y2[i]] = aux[i];
      }
    }
    return k;
 }
*/

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




// QuickSort an array range
// 
void quicksort(int *array, int low, int high) {
  if (high - low < MINSIZE) {
    bubblesort(array, low, high);
    return;
  }
  int middle = partition(array, low, high);
  #pragma omp parallel sections num_threads(numThreads)
{
  #pragma omp section
  if (low < middle)
    quicksort(array, low, middle-1);

  #pragma omp section
  if (middle < high)
    quicksort(array, middle+1, high);
  }
}
 
// Main routine for testing quicksort
// 
int main(int argc, char **argv) {
  int *array, N;
  
  // check command line first 
  if (argc < 2) {
    printf ("Usage: ./qsort <N>\n");
    exit(0);
  }
  if ((N = atoi(argv[1])) < 2) {
    printf ("<N> must be greater than 2\n");
    exit(0);
  }
  if (argc>2){
     numThreads = atoi(argv[2]);
  }
 // N =8;
  clock_t start,finish;
  array = init_array(N);
//int numThreads;// =2;
//omp_set_num_threads(numThreads);

  //  printf("Sorting started ...\n");
  quicksort(array, 0, N-1);
//numThreads = omp_get_num_threads();
  //  printf("... completed.\n");
  verify_array(array, N);
  
  
}

