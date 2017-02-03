//------------------------------------------------------------------------- 
// This is supporting software for CS415/515 Parallel Programming.
// Copyright (c) Portland State University.
//------------------------------------------------------------------------- 

// Jacobi method for solving a Laplace equation.  
// (shared-memory version)
//



config const epsilon = 0.001;	// convergence tolerance
config const n = 8; 	        // mesh size (including boundary)
config const verb=0;
config const gs:int =1,
             rb:int =1,
             jb:int =1;

// Jacobi iteration -- return the iteration count.
// 
proc jacobi(D: domain(2), x: [D] real, epsilon: real) { 
  var cnt = 0;			// iteration counter
  var delta=0.0;
  const D2 = D[1..n-2,1..n-2] ;
  

  var xnew: [D2] real;
  do{
    forall (i,j) in D2 do {
      xnew[i,j] = (x[i-1,j] + x[i+1,j] + x[i,j-1] + x[i,j+1])/4.0;
    }
       delta = max reduce abs(x[D2] - xnew[D2]);
      x[D2] = xnew[D2];
    
    cnt = cnt+1;

  }while(delta>epsilon);
   //writeln("delta = %f", delta);
  

  return cnt;
}

proc gauss_seidel(D: domain(2), x: [D] real, epsilon: real) { 
  var cnt = 0;			// iteration counter
  var delta = 0.0;
  
  const D2 = D[1..n-2,1..n-2];

  var xnew: [D2] real;
  do{
    forall (i,j) in D2 do {
      xnew[i,j] = x[i,j];
      x[i,j] = (x[i-1,j] + x[i+1,j] + x[i,j-1] + x[i,j+1])/4.0;
    }
       delta = max reduce abs(x[D2] - xnew[D2]);
      //x[D2] = xnew[D2];
    
    cnt = cnt+1;

  }while(delta>epsilon);
  //writeln("delta = ", delta);

  return cnt;
}


proc red_black(D: domain(2), x: [D] real, epsilon: real) { 
  var cnt = 0;			// iteration counter
  var delta = 0.0;

  const D1= D[1..n-2,1..n-2];
  
  var xnew: [D1] real;
var diff:[D] int;
        var diff2:[D] int;    
      
  do{
      
    forall (i,j) in D1 do {
      if( (i+j) %2 ==0){
      xnew[i,j] = x[i,j];
      x[i,j] = (x[i-1,j] + x[i+1,j] + x[i,j-1] + x[i,j+1])/4.0;
        if((verb==1 ) && (cnt==0)){
          diff[i,j] = 1;    
        }

      }
    }
    
    forall (i,j) in D1 do {
      if( (i+j) %2 ==1){
      xnew[i,j] = x[i,j];
      x[i,j] = (x[i-1,j] + x[i+1,j] + x[i,j-1] + x[i,j+1])/4.0;

      if((verb==1 ) && (cnt==0)){
          diff2[i,j] = 1;    
      }


      }
    }
     
      if((verb==1 ) && (cnt==0)){
          writeln("red update diff");
          writeln(diff);    
          writeln("black update diff");
          writeln(diff2);
        }



       delta = max reduce abs(x[D1] - xnew[D1]);
      //x[D2] = xnew[D2];
    
    cnt = cnt+1;

  }while(delta>epsilon);
  //writeln("delta = %f", delta);


  return cnt;
}



// Main routine.
//
proc main() {
  
  const D = {0..n-1, 0..n-1};   // domain including boundary points
  var a,b,c: [D] real = 0.0;	// mesh array
  if(jb ==1){
    //writeln("jb=1");
    a[n-1, 0..n-1] = 1.0;         // - setting boundary values
    a[0..n-1, n-1] = 1.0;
    var cnt = jacobi(D, a, epsilon);
    writeln("Mesh size: ", n, " x ", n, ", epsilon=", epsilon, 
            ", total Jacobi iterations: ", cnt);
  }

  if(gs==1){
  //writeln("gs=1");
  b[n-1, 0..n-1] = 1.0;         // - setting boundary values
  b[0..n-1, n-1] = 1.0;
  
  var cnt2 = gauss_seidel(D, b, epsilon);
  writeln("Mesh size: ", n, " x ", n, ", epsilon=", epsilon, 
            ", total Gauss-seidel iterations: ", cnt2);
  }

  if(rb==1){
  //writeln("rb=1");
  c[n-1, 0..n-1] = 1.0;         // - setting boundary values
  c[0..n-1, n-1] = 1.0;
  
  var cnt3 = red_black(D, c, epsilon);
  writeln("Mesh size: ", n, " x ", n, ", epsilon=", epsilon, 
            ", total red_black iterations: ", cnt3);
  }
}
