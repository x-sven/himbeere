#include "fintrf.h"
C======================================================================
#if 0
C     
C     xtimesy.F
C     .F file needs to be preprocessed to generate .for equivalent
C     
#endif
C     
C     xtimesy.f
C
C     Multiple the first input by the second input
      
C     This is a MEX file for MATLAB.
C     Copyright 1984-2011 The MathWorks, Inc.
C     $Revision: 1.12.2.6 $
C======================================================================
C     Gateway routine
      subroutine mexFunction(nlhs, plhs, nrhs, prhs)

C     Declarations
      implicit none

C     mexFunction arguments:
      mwPointer plhs(*), prhs(*)
      integer nlhs, nrhs
 
C     Function declarations:
      mwPointer mxCreateDoubleMatrix
      mwPointer mxGetPr
      mwPointer mxGetM, mxGetN
      integer mxIsNumeric 
 
C     Pointers to input/output mxArrays:
      mwPointer x_pr, y_pr, z_pr
 
C     Array information:
      mwPointer m, n
      mwSize size, row
 
C     Arguments for computational routine:
      real*8  x, y(100), z(100)

C-----------------------------------------------------------------------
C     Check for proper number of arguments. 
      if (nrhs .ne. 2) then
         call mexErrMsgIdAndTxt ('MATLAB:xtimesy:nInput',
     +                           'Two inputs required.')
      elseif (nlhs .ne. 1) then
         call mexErrMsgIdAndTxt ('MATLAB:xtimesy:nOutput',
     +                           'One output required.')
      endif

C     Validate inputs
C     Check to see both inputs are numeric.
      if (mxIsNumeric(prhs(1)) .ne. 1) then
         call mexErrMsgIdAndTxt ('MATLAB:xtimesy:NonNumeric1',
     +                           'Input # 1 is not a numeric.')
      elseif (mxIsNumeric(prhs(2)) .ne. 1) then
         call mexErrMsgIdAndTxt ('MATLAB:xtimesy:NonNumeric2',
     +                           'Input #2 is not a numeric array.')
      endif
      
C     Check that input #1 is a scalar.
      m = mxGetM(prhs(1))
      n = mxGetN(prhs(1))
      if(n .ne. 1 .or. m .ne. 1) then
         call mexErrMsgIdAndTxt ('MATLAB:xtimesy:NonScalar',
     +                           'Input #1 is not a scalar.')
      endif

C     Get the size of the input matrix.
      m = mxGetM(prhs(2))
      n = mxGetN(prhs(2))
      size = m*n
      if(size .gt. 100) then
         call mexErrMsgIdAndTxt ('MATLAB:xtimesy:InputTooBig',
     +              'Input #2 number of elements exceeds buffer')
      endif

C     Create matrix for the return argument.
      plhs(1) = mxCreateDoubleMatrix(m,n,0)
      x_pr = mxGetPr(prhs(1))
      y_pr = mxGetPr(prhs(2))
      z_pr = mxGetPr(plhs(1))

C     Load the data into Fortran arrays.
      row = 1
      call mxCopyPtrToReal8(x_pr,x,row)
      call mxCopyPtrToReal8(y_pr,y,size)

C     Call the computational subroutine.
      call xtimesy(x,y,z,m,n)

C     Load the output into a MATLAB array.
      call mxCopyReal8ToPtr(z,z_pr,size)

      return
      end

C-----------------------------------------------------------------------
C     Computational routine

      subroutine xtimesy(x,y,z,m,n)
      mwSize m, n
      real*8  x, y(m,n), z(m,n)
      mwSize i, j

      do 20 i=1,m
         do 10 j=1,n
            z(i,j)=x*y(i,j)
 10      continue
 20   continue
      return
      end

      
