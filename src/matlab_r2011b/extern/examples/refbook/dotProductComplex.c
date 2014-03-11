/*=========================================================
* dotProductComplex.c - Example to handle FORTRAN complex
* return type for function called from a C MEX-file.
*
* X = dotProductComplex(A,B) computes the dot product of 
* each element of two complex vectors A and B 
* using BLAS routine ZDOTU: 
* DOUBLE COMPLEX FUNCTION ZDOTU(N,ZX,INCX,ZY,INCY)
*
* where:
* A and B are COMPLEX vectors of the same size  
* X is COMPLEX scalar
*
* This is a MEX-file for MATLAB.
* Copyright 2010 The MathWorks, Inc.
*=======================================================*/
/* $Revision: */

/* Macro to define the correct function prototype */
#ifdef _WIN32
#define FORTRAN_COMPLEX_FUNCTIONS_RETURN_VOID 1
#endif

#include "mex.h"
#include "blas.h"
#include "fort.h"    /* defines complex data handling functions */

void mexFunction(int nlhs, mxArray *plhs[], 
                 int nrhs, const mxArray *prhs[])
{
  double *zinA, *zinB;  /* pointers to input matrices */
  doublecomplex result; /* ZDOTU output */
  double *zout;         /* pointer to ZDOTU output */

  /* matrix dimensions */
  size_t nrowsA, ncolsA, nrowsB, ncolsB;

  /* additional inputs to ZDOTU */
  mwSignedIndex nElements;    /* Number of elements in vector */
  mwSignedIndex one = 1; 
  mwSignedIndex incx = one;   /* Increment between elements of x */
  mwSignedIndex incy = one;   /* Increment between elements of y */

  /* Check for proper number of arguments. */
  if ( nrhs != 2) {
    mexErrMsgIdAndTxt("MATLAB:dotProductComplex:rhs",
    "This function requires 2 input matrices.");
  }
  /* Check for complex values */
  if (!mxIsComplex(prhs[0]) || !mxIsComplex(prhs[1])) {
    mexErrMsgIdAndTxt("MATLAB:dotProductComplex:real",
    "Input matrices must be complex.");
  }

  /* dimensions of input matrices A and B */
  nrowsA = mxGetM(prhs[0]);  
  ncolsA = mxGetN(prhs[0]);
  nrowsB = mxGetM(prhs[1]);  
  ncolsB = mxGetN(prhs[1]);
  
  /* Validate input arguments */
  if ((nrowsA > ncolsA ? ncolsA : nrowsA) > 1 || 
      (nrowsB > ncolsB ? ncolsB : nrowsB) > 1) {
    mexErrMsgIdAndTxt("MATLAB:dotProductComplex:matrix",
    "Input must be vectors."); 
  }
  if ((nElements = (nrowsA > ncolsA ? nrowsA : ncolsA)) != 
      (nrowsB > ncolsB ? nrowsB : ncolsB) ) {
    mexErrMsgIdAndTxt("MATLAB:dotProductComplex:unequal",
     "Input vectors must be equal size.");
  }
  
  /* Convert complex input data to Fortran format */
  zinA = mat2fort(prhs[0], nElements, one);
  zinB = mat2fort(prhs[1], nElements, one);

  /* Call BLAS function */
  /* Use a different call syntax on different platforms */
#ifdef _WIN32
  zdotu(&result, &nElements, zinA, &incx, zinB, &incy);
#else
  result = zdotu(&nElements, zinA, &incx, zinB, &incy);
#endif
  
  /* Convert result to pointer of type double */
  /* to make it compatible with mxArray */
  zout = (double *) &result;

  /* Convert Fortran output to MATLAB format */        
  plhs[0] = fort2mat(zout, one, one, one);
  /* plhs now holds a complex mxArray */

  mxFree(zinA);
  mxFree(zinB);
}
