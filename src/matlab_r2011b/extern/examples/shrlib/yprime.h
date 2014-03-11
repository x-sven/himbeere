/* $Revision: 1.1.6.5 $ */
/* header file for yprime.c 
* Copyright 2002-2007 The MathWorks, Inc.
*/
#include "shrhelp.h"
#define EXTERN_C EXPORTED_FUNCTION
#include <mex.h>

EXPORTED_FUNCTION void yprimefcn(
		   double	*yp,
		   double	*t,
 		   double	*y
		   );
		   
EXPORTED_FUNCTION mxArray* better_yprime(
		   double	t,
 		   mxArray* Y_IN);

EXPORTED_FUNCTION void mexFunction( int nlhs, mxArray *plhs[], 
		  int nrhs, const mxArray *prhs[] );
		  
