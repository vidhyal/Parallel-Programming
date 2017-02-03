//------------------------------------------------------------------------- 
// This is supporting software for CS415/515 Parallel Programming.
// Copyright (c) Portland State University.
//------------------------------------------------------------------------- 

// Jacobi method for solving a Laplace equation.  
//
// Usage: ./jacobi [N]
// 
//
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define EPSILON 0.001 	// convergence tolerance
#define VERBOSE 0	// printing control

// Initialize the mesh with a fixed set of boundary conditions.
// 
void init_array(int n, double **a)  {
  int i, j;
  for (i = 0; i < n; i++) {
    for (j = 0; j < n; j++) 
      a[i][j] = 0;
  }
  for (i = 1; i < n-1; i++) {
    a[n-1][i] = 1.0;
    a[i][n-1] = 1.0;
  }
}

// Display the whole mesh.
// 
void print_array(int n, double **a)  {
  int i, j;
  for (i = 0; i < n; i++) {
    for (j = 0; j < n; j++)
      printf("%8.4f ", a[i][j]);
    printf("\n");
  }
}

// Jacobi iteration -- return the iteration count.
// 
int jacobi(int n, double **x, double epsilon) {
  //double **xnew = malloc(sizeof(double)*n*n);//[n][n];	// buffer for new values    
  double delta;		// measure of convergence   
  int cnt = 0;		// iteration counter              
  int i, j;


  double **xnew = (double**)malloc(sizeof(double*)*n);
  for (i=0; i<n; i++){
    xnew[i] = (double*) malloc(n*sizeof(double));
  }
  do {	
    delta = 0.0;
    for (i = 1; i < n-1; i++) {
      for (j = 1; j < n-1; j++) {
	xnew[i][j] = (x[i-1][j] + x[i][j-1] + x[i+1][j] + x[i][j+1]) / 4.0;
	delta = fmax(delta, fabs(xnew[i][j] - x[i][j]));
      }
    }	
    for (i = 1; i < n-1; i++) {
      for (j = 1; j < n-1; j++) {
	x[i][j] = xnew[i][j];
      }
    }
    cnt++;
    if (VERBOSE) {
      printf("Iter %d: (delta=%6.4f)\n", cnt, delta);
      print_array(n, x);
    }
  } while (delta > epsilon);
   if (VERBOSE) printf("delta in jacobi =%f\n", delta);
  return cnt;
}

// Gauss-Seidel iteration -- return the iteration count.
// 
int red_black(int n, double **x, double epsilon) {
  //double xnew[n][n];	// buffer for new values    
 double delta;		// measure of convergence   
  int cnt = 0;		// iteration counter              
  int i, j;
  int rb=0;
  do {	
    delta = 0.0;
    for( int rb=0; rb<2; rb++){
    for (i = 1; i < n-1; i++) {
      for (j = 1; j < n-1; j++) {
        if((i+j)%2==rb){
	double temp = x[i][j];
        x[i][j] = (x[i-1][j] + x[i][j-1] + x[i+1][j] + x[i][j+1]) / 4.0;
	delta = fmax(delta, fabs(temp - x[i][j]));
        //printf("delta= %f\n", delta);
         //rb=1;
        }
        
      }
    }	
    }
    cnt++;
    if (VERBOSE) {
      printf("Iter %d: (delta=%6.4f)\n", cnt, delta);
      print_array(n, x);
    }
    //if(rb==0) rb=1;
    //else rb=0;
  } while (delta > epsilon);
   if (VERBOSE) printf("delta in red-black =%f\n", delta);
  return cnt;
}

int gaussSeidel(int n, double **x, double epsilon) {
 // double xnew[n][n];	// buffer for new values    
  double delta;		// measure of convergence   
  int cnt = 0;		// iteration counter              
  int i, j;

  do {	
    delta = 0.0;
    for (i = 1; i < n-1; i++) {
      for (j = 1; j < n-1; j++) {
	double temp = x[i][j];
        x[i][j] = (x[i-1][j] + x[i][j-1] + x[i+1][j] + x[i][j+1]) / 4.0;
	delta = fmax(delta, fabs(temp - x[i][j]));
      }
    }	
    cnt++;
    if (VERBOSE) {
      printf("Iter %d: (delta=%6.4f)\n", cnt, delta);
      print_array(n, x);
    }
  } while (delta > epsilon);
   if (VERBOSE)  printf("delta in gauss-seidel =%f\n", delta);
  return cnt;
}


// Main routine.
//
int main(int argc, char **argv) {

  int n = 8;  	   	// mesh size, default 8 x 8
  if (argc > 1) {  	// check command line for overwrite
    if ((n = atoi(argv[1])) < 2) {
      printf("Mesh size must must be greater than 2, use default\n");
      n = 8;
    }
  }
int i=0;
  double **a = (double**)malloc(sizeof(double*)*n);
  for (i=0; i<n; i++){
    a[i] = (double*) malloc(n*sizeof(double));
  }
//printf("declared\n");
  init_array(n, a);
if (VERBOSE) 
    print_array(n, a);

  // Jacobi iteration, return value is the total iteration number
  int cnt = jacobi(n, a, EPSILON);
  printf("Mesh size: %d x %d, epsilon=%6.4f, total Jacobi iterations: %d\n", 
	 n, n, EPSILON, cnt);
init_array(n, a);
if (VERBOSE) 
    print_array(n, a);

int cnt2 = gaussSeidel(n, a, EPSILON);
  printf("Mesh size: %d x %d, epsilon=%6.4f, total Gauss-Seidel iterations: %d\n", 
	 n, n, EPSILON, cnt2);

init_array(n, a);
if (VERBOSE) 
    print_array(n, a);

int cnt3 = red_black(n, a, EPSILON);
  printf("Mesh size: %d x %d, epsilon=%6.4f, total red_black iterations: %d\n", 
	 n, n, EPSILON, cnt3);

  if (VERBOSE) 
    print_array(n, a);
}
