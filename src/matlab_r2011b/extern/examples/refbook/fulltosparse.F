#include "fintrf.h"
C======================================================================
C
#if 0
C     
C     fulltosparse.F
C     .F file needs to be preprocessed to generate .for equivalent
C     
#endif
C     
C     fulltosparse.f
C     example for illustrating how to populate a sparse matrix.
C     For the purpose of this example, you must pass in a
C     non-sparse 2-dimensional argument of type real double.
C
C     NOTE: The subroutine loadsparse() is in the file called 
C     loadsparse.f. 
C
C     This is a MEX-file for MATLAB.
C     Copyright 1984-2011 The MathWorks, Inc.
C     $Revision: 1.6.6.6 $
C======================================================================
C     Gateway routine
      subroutine mexFunction(nlhs, plhs, nrhs, prhs)

C     Declarations
      implicit none

C     mexFunction arguments:
      mwPointer plhs(*), prhs(*)
      integer nlhs, nrhs

C     Function declarations:
      mwPointer mxGetPr
      mwPointer mxCreateSparse
      mwPointer mxGetIr, mxGetJc
      mwPointer mxGetM, mxGetN
      integer mxIsComplex
      integer mxIsDouble
      integer loadsparse

C     Pointers to input/output mxArrays:
      mwPointer pr, sr, irs, jcs

C     Array information:
      mwPointer m, n
      mwSize nzmax

C-----------------------------------------------------------------------
C     Check for proper number of arguments. 
      if (nrhs .ne. 1) then
         call mexErrMsgIdAndTxt ('MATLAB:fulltosparse:nInput',
     +                           'One input argument required.')
      endif
      if (nlhs .gt. 1) then
         call mexErrMsgIdAndTxt ('MATLAB:fulltosparse:nOutput',
     +                           'Too many output arguments.')
      endif

C     Validate inputs
C     Check data type of input argument
      if (mxIsDouble(prhs(1)) .eq. 0) then
         call mexErrMsgIdAndTxt ('MATLAB:fulltosparse:NonDouble',
     +                      'Input argument must be of type double.')
      endif
      if (mxIsComplex(prhs(1)) .eq. 1) then
         call mexErrMsgIdAndTxt ('MATLAB:fulltosparse:NonReal',
     +                           'Input argument must be real only')
      endif

C     Get the size and pointers to input data
      m = mxGetM(prhs(1))
      n = mxGetN(prhs(1))
      pr = mxGetPr(prhs(1))

C     Allocate space
C     NOTE: Assume at most 20% of the data is sparse.
      nzmax = dble(m*n) *.20 + .5

C     NOTE: The maximum number of non-zero elements cannot be less
C     than the number of columns in the matrix
      if (n .gt. nzmax) then
         nzmax = n
      endif

      plhs(1) = mxCreateSparse(m,n,nzmax,0)
      sr = mxGetPr(plhs(1))
      irs = mxGetIr(plhs(1))
      jcs = mxGetJc(plhs(1))

C     Load the sparse data
      if (loadsparse(%VAL(pr),%VAL(sr),%VAL(irs),%VAL(jcs),m,n,nzmax)
     +     .eq. 1) then
         call mexPrintf('Truncating output, input is > 20%% sparse')
      endif

      return
      end
