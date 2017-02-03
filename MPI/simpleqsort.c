#include<stdio.h>
#include "qsort.c"

void print_array(int* arr, int n){
  //int n = sizeof(arr)/sizeof(int);
  for (int i=0; i<n; i++)
    printf(" %d\t", arr[i]);
 printf("\n");
}

int main(int argc, char **argv) {
  int N;
  
  // check command line first 
  if (argc < 3) {
    printf ("Usage: ./qsort <datafile> <output file>\n");
    exit(0);
  }
  FILE *iFp, *oFp ;
  iFp=fopen(argv[1], "rb");
  if (iFp){
  fseek(iFp, 0L, SEEK_END);
  N = ftell(iFp)/sizeof(int);
   //printf("N= %d\n", N); 
  fseek(iFp, 0L, SEEK_SET);
  int * array;
  //int array[N];
 array= (int*)malloc( N* sizeof(int));

  int n= fread(array, N*sizeof(int), 1, iFp);
    //print_array(array, N);
  fclose(iFp);
  //array = init_array(N);
   
//print_array(array, N);
  //  printf("Sorting started ...\n");
  quicksort(array, 0, N-1);
    printf("... completed.\n");

  //verify_array(array, N);
  oFp = fopen(argv[2],"wb");
  fwrite(array, N*sizeof(int), 1, oFp);
  fclose(oFp);
}
}