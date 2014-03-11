/*=================================================================
 * mxmalloc.c
 * 
 * This function takes a MATLAB string as an argument and copies it in 
 * NULL terminated ANSI C string.
 *
 * This is a MEX-file for MATLAB.  
 * Copyright 1984-2011 The MathWorks, Inc.
 *=================================================================*/

/* $Revision: 1.3.6.4 $ */
#include "mex.h"
   
void
mexFunction(int nlhs,mxArray *plhs[],int nrhs,const mxArray *prhs[])
{
    char *buf;
    size_t buflen;
    int status;

    (void) plhs;    /* unused parameters */
    
    /* Check for proper number of input and output arguments */
    if (nrhs != 1) { 
	    mexErrMsgIdAndTxt( "MATLAB:mxmalloc:invalidNumInputs", 
                "One input argument required.");
    } 
    if (nlhs > 1) {
	    mexErrMsgIdAndTxt( "MATLAB:MXMALLOC:maxlhs",
                "Too many output arguments.");
    }
    
    /* Check for proper input type */
    if (!mxIsChar(prhs[0]) || (mxGetM(prhs[0]) != 1 ) )  {
	    mexErrMsgIdAndTxt( "MATLAB:mxmalloc:invalidInput", 
                "Input argument must be a string.");
    }
    
    /* Find out how long the input string is.  Allocate enough memory
       to hold the converted string.  NOTE: MATLAB stores characters
       as 2 byte Unicode ( 16 bit ASCII) on machines with multi-byte
       character sets.  You should use mxChar to ensure enough space
       is allocated to hold the string */
    
    buflen = mxGetN(prhs[0])*sizeof(mxChar)+1;
    buf = mxMalloc(buflen);
    
    /* Copy the string data into buf. */ 
    status = mxGetString(prhs[0], buf, (mwSize)buflen);   
    mexPrintf("The input string is:  %s\n", buf);
    /* NOTE: You could add your own code here to manipulate 
       the string */
    
    /* When finished using the string, deallocate it. */
    mxFree(buf);
    
}
