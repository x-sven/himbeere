/*=================================================================
 * mexevalstring.c
 *
 * mexevalstring takes no input arguments. It uses mexEvalString to
 * assign a couple of variables in MATLAB.
 *
 * Copyright 1984-2009 The MathWorks, Inc.
 *================================================================*/

/* $Revision: 1.8.6.3 $ */
#include "mex.h"

void
mexFunction(int nlhs,mxArray *plhs[],int nrhs,const mxArray *prhs[])
{
    (void) plhs;      /* unused parameters */
    (void) prhs;

    /* Check for proper number of input and output arguments */    
    if (nrhs != 0) {
        mexErrMsgIdAndTxt("MATLAB:mexevalstring:nInput", "No input arguments required.");
    } 
    if(nlhs !=0){
        mexErrMsgIdAndTxt("MATLAB:mexevalstring:nOutput", "Too many output arguments.");
    } 

    /* Assign a value to the variable a. */
    mexEvalString("a=10");
   
    /* Assign a value to the variable b. */
    mexEvalString("b='Hello World'");
}
