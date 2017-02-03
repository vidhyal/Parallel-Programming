#include <stdio.h>
#include <mpi.h>
#include<stdlib.h>
#include <unistd.h>
#include "qsort.c"
#include "LinkedList.h"

#define TAG 1001

int * Partition(int *array, int n, int P) ;
void make_buckets(int array[], Bucket* buckets, int n, int modN, int P);
void ToArray(Bucket , int[]);
void print_array(int* arr, int n);
void print_bucket(Node * head) ;


int main(int argc, char* argv[]){
  if (argc<3){
    printf("Usage: exsort <infile> <outfile>\n");
    exit(1);
  }
  char* inFile = argv[1];
  char* outFile = argv[2];

  MPI_File iFp, oFp ;
  MPI_Offset size;  
  int N; 
  int rank, P;
  MPI_Status st;
 
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &P);
  int tot = 10*P;


  if (P<2){
    printf("Need at least 2 processes.\n");
    MPI_Finalize();
    return(1);
  }

  printf("P%d/%d started ...\n", rank, P);
  if (rank ==0){

   
    MPI_File_open(MPI_COMM_SELF, argv[1], MPI_MODE_RDONLY, MPI_INFO_NULL, &iFp);
    MPI_File_get_size(iFp, &size);
    N = size/sizeof(int);
    #ifdef DEBUG
      printf(" N = %d\n", N);
    #endif
    MPI_File_set_view(iFp, 0, MPI_INT, MPI_INT, "native", MPI_INFO_NULL);
  
    // read N integers from the file
    int *buf = (int*) malloc(N*sizeof(int));
    MPI_File_read(iFp, buf, N, MPI_INT, &st);
    int count;
    MPI_Get_count(&st,MPI_INT, &count);
   // printf("%d", count);

    #ifdef DEBUG
      printf("%d", count);
      printf("file read\n\n");
      print_array(buf,N);
    #endif
    
    MPI_File_close(&iFp);
    //printf("file closed\n");
    quicksort(buf, 0, 10*P-1);
    #ifdef DEBUG
      print_array(buf,N);
    #endif
    Bucket *buckets = malloc(sizeof(Bucket) * (P));
    make_buckets(buf, buckets, N, N/P, P);
    
    int off=buckets[0].size;
    
    for (int i=1; i<P; i++){
        int leng= buckets[i].size;
        int *array= (int*) malloc(leng* sizeof(int));
	    ToArray(buckets[i], array);
	    
		#ifdef DEBUG   
	      printf("off = %d, leng = %d\n", off, leng);
	      print_array(array, leng);
	    #endif
	    
		MPI_Send(&leng, 1, MPI_INT, i, TAG, MPI_COMM_WORLD);
	    MPI_Send(&off, 1, MPI_INT, i, TAG, MPI_COMM_WORLD);
	    MPI_Send(array, leng, MPI_INT, i, TAG, MPI_COMM_WORLD);
	    off +=leng;
	    free(array);
    }
 
    int leng= buckets[0].size;
    int *array= (int*)malloc(leng* sizeof(int));
    ToArray(buckets[0], array);
    quicksort(array,0, leng-1);
    #ifdef DEBUG
 	  printf("sorted array\n");
      print_array(array, leng);
    #endif
    MPI_File_open(MPI_COMM_WORLD, argv[2], MPI_MODE_CREATE|MPI_MODE_RDWR, MPI_INFO_NULL, &oFp);
    MPI_File_set_view(oFp, 0 , MPI_INT, MPI_INT, "native", MPI_INFO_NULL);
 
    MPI_File_write(oFp, array, leng, MPI_INT, &st);
    MPI_File_close(&oFp);
    free(array);
    
       
   } 
   else{
      int len, off;
      MPI_Recv(&len, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, &st);
      MPI_Recv(&off, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, &st);
      #ifdef DEBUG
	    printf("off = %d, len = %d\n", off, len);
      #endif
   
      int *arr = malloc(sizeof(int)*len);
      MPI_Recv(arr, len, MPI_INT, 0, TAG, MPI_COMM_WORLD, &st); 
      #ifdef DEBUG
	    print_array(arr,len);
      #endif
      
      quicksort(arr,0, len-1);
      #ifdef DEBUG
        printf("sorted arr\n");
        print_array(arr, len);
      #endif
      
      MPI_Offset offst = (off)* sizeof(int);
      MPI_File_open(MPI_COMM_WORLD, argv[2], MPI_MODE_CREATE|MPI_MODE_RDWR, MPI_INFO_NULL, &oFp);
     
      MPI_File_set_view(oFp, offst , MPI_INT, MPI_INT, "native", MPI_INFO_NULL);
      MPI_File_write(oFp, arr, len, MPI_INT, &st);
  	  MPI_File_close(&oFp);
      free(arr);
    
   }
   MPI_Finalize();
  
  return 0;

}


void make_buckets(int array[],Bucket *buckets, int n, int modN, int P){
  int i, j, bucket;
  MPI_Status status;
  
  int pivot[P-1];
  int num = modN;
  
  
  for( i=0; i<P-1; i++){
    pivot[i] = array[(i+1)*10-1];
  }
  #ifdef DEBUG  
    for( i=0; i<P-1; i++){
      printf("pivot[%d] = %d\t", i, pivot[i]);
    }
    printf("\n");
  #endif
    for (i=0; i<P; i++){
      buckets[i].size =0;
      buckets[i]. head = NULL;
	}
  
    for (j =0; j< n; j++){
      for (i = 0; i<P-1; i++){
        if (i ==0 ){
          if (array[j] < pivot[i]){
            Node *temp = buckets[i].head;
            Node *temp2 = malloc(sizeof(Node));
            temp2->data = array[j];
            temp2->next = temp;
            buckets[i].head = temp2;
            buckets[i].size++;
          }
       }
       else if (array[j] < pivot[i] && array[j] >= pivot[i-1]){
          Node *temp = buckets[i].head;
          Node *temp2 = malloc(sizeof(Node));
          temp2->data = array[j];
          temp2->next = temp;
          buckets[i].head = temp2;
          buckets[i].size++;
        }
        if (i==P-2 ){
          if(array[j]>=pivot[i]){
            Node *temp = buckets[i+1].head;
            Node *temp2 = malloc(sizeof(Node));
            temp2->data = array[j];
            temp2->next = temp;
            buckets[i+1].head = temp2;
            buckets[i+1].size++;
          }
        }
      
      }
    }
    #ifdef DEBUG
      for (i =0; i<P; i++){
        printf("bucket size = %d\n",buckets[i].size);
        print_bucket(buckets[i].head);
      } 
    #endif
   
}

void ToArray(Bucket bucket, int* arr){
    int n = bucket.size;
    int i;
    Node *temp = bucket.head;
    for (i=0; i<n; i++){
       arr[i] = temp->data;
       temp = temp->next;
    }
}


void print_array(int* arr, int n){
  for (int i=0; i<n; i++)
    printf(" %d\t", arr[i]);
  printf("\n");
}

void print_bucket(Node * head) {
    Node * current = head;
    while (current != NULL) { 
        printf("%d->", current->data);
        current = current->next;
    }
    printf("null\n");
    printf("\n");
}
