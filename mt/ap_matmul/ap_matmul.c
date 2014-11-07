//**************************************************************************
// Multi-threaded Matrix Multiply benchmark
//--------------------------------------------------------------------------
// TA     : Christopher Celio
// Student: ME STEPHANIE TUNG
//
//
// This benchmark multiplies two 2-D arrays together and writes the results to
// a third vector. The input data (and reference data) should be generated
// using the matmul_gendata.pl perl script and dumped to a file named
// dataset.h. 


// print out arrays, etc.
//#define DEBUG

//--------------------------------------------------------------------------
// Includes 

#include <string.h>
#include <stdlib.h>
#include <stdio.h>


//--------------------------------------------------------------------------
// Input/Reference Data

typedef float data_t;
#include "dataset.h"
 
  
//--------------------------------------------------------------------------
// Basic Utilities and Multi-thread Support

__thread unsigned long coreid;
unsigned long ncores;

#include "util.h"
   
#define stringify_1(s) #s
#define stringify(s) stringify_1(s)
#define stats(code) do { \
    unsigned long _c = -rdcycle(), _i = -rdinstret(); \
    code; \
    _c += rdcycle(), _i += rdinstret(); \
    if (coreid == 0) \
      printf("%s: %ld cycles, %ld.%ld cycles/iter, %ld.%ld CPI\n", \
             stringify(code), _c, _c/DIM_SIZE/DIM_SIZE/DIM_SIZE, 10*_c/DIM_SIZE/DIM_SIZE/DIM_SIZE%10, _c/_i, 10*_c/_i%10); \
  } while(0)
 

//--------------------------------------------------------------------------
// Helper functions
    
void printArrayMT( char name[], int n, data_t arr[] )
{
   int i;
   if (coreid != 0)
      return;
  
   printf( " %10s :", name );
   for ( i = 0; i < n; i++ )
      printf( " %3ld ", (long) arr[i] );
   printf( "\n" );
}
      
void __attribute__((noinline)) verifyMT(size_t n, const data_t* test, const data_t* correct)
{
   if (coreid != 0)
      return;

   size_t i;
   for (i = 0; i < n; i++)
   {
      if (test[i] != correct[i])
      {
         printf("FAILED test[%d]= %3ld, correct[%d]= %3ld\n", 
            i, (long)test[i], i, (long)correct[i]);
         exit(-1);
      }
   }
   
   return;
}
 
//--------------------------------------------------------------------------
// matmul function
 
// single-thread, naive version
void __attribute__((noinline)) matmul_naive(const int lda,  const data_t A[], const data_t B[], data_t C[] )
{
   int i, j, k;

   if (coreid > 0)
      return;
  
   for ( i = 0; i < lda; i++ )
      for ( j = 0; j < lda; j++ )  
      {
         for ( k = 0; k < lda; k++ ) 
         {
            C[i + j*lda] += A[j*lda + k] * B[k*lda + i];
         }
      }

}
 


void __attribute__((noinline)) matmul(const int lda,  const data_t A[], const data_t B[], data_t C[] )
{
   
   // ***************************** //
   // **** ADD YOUR CODE HERE ***** //
   // ***************************** //
   //
   // feel free to make a separate function for MI and MSI versions.
   int i, j, k, ii, jj, kk;
   int block = lda / ncores;
   int leftover = lda % ncores;
   int start = block * coreid;


 
   for ( j = start; j < (start+block); j++ )
      for ( k = 0; k < lda; k++ )  
      {
         for ( i = 0; i < lda; i++ ) 
         {
            C[i + j*lda] += A[j*lda + k] * B[k*lda + i];
         }
      }
/*


   for ( j = coreid; j < lda; j += ncores )
      for ( k = 0; k < lda; k++ )  
      {
         for ( i = 0; i < lda; i++ ) 
         {
            C[i + j*lda] += A[j*lda + k] * B[k*lda + i];
         }
      }

/*
   if (coreid > 0) {
      return;
   }
   
   for ( j = (lda - leftover); j < lda; j++ )
      for ( i = 0; i < lda; i++ )  
      {
         for ( k = 0; k < lda; k++ ) 
         {
            C[i + j*lda] += A[j*lda + k] * B[k*lda + i];
         }
      }



if (coreid > 0) {
	return;
}



for (jj = start; jj < start+block; jj += 4) {
   for (kk = 0; kk < lda; kk += 4) {
      for (ii = 0; ii < lda; ii += 4) {
         for (i = ii; i < ii+4; i += 4) {
            //float * p = B + i;
            for (j = jj; j < jj+4; j++) {
               for (k = kk; k < kk+4; k++) {

                  float a = A[k + j*lda];

                  C[i + j*lda] += a * B[k*lda + i];
                  C[i + j*lda + 1] += a * B[k*lda + i + 1];
                  C[i + j*lda + 2] += a * B[k*lda + i + 2];
                  C[i + j*lda + 3] += a * B[k*lda + i + 3];
               }
            }
         }
      }
   }
}

*/

}

//--------------------------------------------------------------------------
// Main
//
// all threads start executing thread_entry(). Use their "coreid" to
// differentiate between threads (each thread is running on a separate core).
  
void thread_entry(int cid, int nc)
{
   coreid = cid;
   ncores = nc;

   // static allocates data in the binary, which is visible to both threads
   static data_t results_data[ARRAY_SIZE];


//   // Execute the provided, naive matmul
//   barrier(nc);
//   stats(matmul_naive(DIM_SIZE, input1_data, input2_data, results_data); barrier(nc));
// 
//   
//   // verify
//   verifyMT(ARRAY_SIZE, results_data, verify_data);
//   
//   // clear results from the first trial
//   size_t i;
//   if (coreid == 0) 
//      for (i=0; i < ARRAY_SIZE; i++)
//         results_data[i] = 0;
//   barrier(nc);

   
   // Execute your faster matmul
   barrier(nc);
   stats(matmul(DIM_SIZE, input1_data, input2_data, results_data); barrier(nc));
 
#ifdef DEBUG
   printArrayMT("results:", ARRAY_SIZE, results_data);
   printArrayMT("verify :", ARRAY_SIZE, verify_data);
#endif
   
   // verify
   verifyMT(ARRAY_SIZE, results_data, verify_data);
   barrier(nc);

   exit(0);
}
