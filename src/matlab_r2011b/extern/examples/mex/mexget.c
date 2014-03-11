/*=================================================================
 * mexget.c 
 *
 * This example demonstrates how to use mexGet and mexSet.  The input
 * to this function is a handle graphics handle.  mexget.c gets the
 * Color property of the handle that was passed into the function. It
 * then changes the colors, and uses mexSet to set the Color property
 * of the handle to the new color values.
 *
 *
 * This is a MEX-file for MATLAB.  
 * Copyright 1984-2011 The MathWorks, Inc.
 * All rights reserved.
 *=================================================================*/

/* $Revision: 1.6.6.4 $ */
#include "mex.h"

#define RED   0
#define GREEN 1
#define BLUE  2

void mexFunction(int nlhs, mxArray *plhs[], int nrhs,
		 const mxArray *prhs[])
{
    double         handle; 
    const mxArray *color_array_ptr;
    mxArray       *value;
    double        *color; 

    (void) plhs;      /* unused parameters */

    /* Assume that the first input argument is a graphics
       handle. Check to make sure the input is a double and that only
       one input is specified.*/
    if(nrhs != 1 || !mxIsDouble(prhs[0])){
        mexErrMsgIdAndTxt( "MATLAB:mexget:invalidInput",
                "Must be called with a valid handle");
    }    
    
    /* Check for the correct number of outputs */
    if(nlhs > 1){
        mexErrMsgIdAndTxt( "MATLAB:mexget:maxlhs",
                "Too many output arguments.");
    }
    /* Check to make sure input argument is a scalar */

    if (mxGetN(prhs[0]) != 1 || mxGetM(prhs[0]) !=1){
      mexErrMsgIdAndTxt( "MATLAB:mexget:inputMustBeScalar",
              "Input must be a scalar handle value.\n");
    }
    /* Get the handle */
    handle = mxGetScalar(prhs[0]);
    
    /* Get the "Color" property associated with this handle. */
    color_array_ptr = mexGet(handle, "Color");
    if (color_array_ptr == NULL)
      mexErrMsgIdAndTxt( "MATLAB:mexget:errGettingHandleProperty",
              "Could not get this handle property");
    
   /* Make copy of "Color" propery */
    value = mxDuplicateArray(color_array_ptr);
    
    /* The returned "Color" property is a 1-by-3 matrix of 
       primary colors. */ 
    color = mxGetPr(value);
    
    /* Change the color values */
    color[RED] = (1 + color[RED]) /2;
    color[GREEN] = color[GREEN]/2;
    color[BLUE] = color[BLUE]/2;
    
    /* Reset the "Color" property to use the new color. */
	if(mexSet(handle, "Color", value))
	  mexErrMsgIdAndTxt( "MATLAB:mexget:errorSettingColor",
              "Could not set a new 'Color' property.");
}
