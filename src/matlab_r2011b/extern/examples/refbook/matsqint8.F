#include "fintrf.h"
C======================================================================
#if 0
C     
C     matsqint8.F
C     .F file needs to be preprocessed to generate .for equivalent
C     
#endif
C     
C     matsqint8.f
C
C     Squares the input matrix. Data is MATLAB type int8, 
C     equivalent to Fortran type integer*1
    
C     This is a MEX-file for MATLAB.
C     Copyright 1984-2011 The MathWorks, Inc.
C     $Revision: 1.1.6.3 $
C======================================================================
C     Gateway routine
      subroutine mexFunction(nlhs, plhs, nrhs, prhs)
 
C     Declarations
      implicit none
      
C     mexFunction arguments:
      mwPointer plhs(*), prhs(*)
      integer nlhs, nrhs
 
C     Function declarations:
      mwPointer mxCreateNumericArray
      integer*4 mxClassIDFromClassName
      mwPointer mxIsInt8 
      mwPointer mxGetPr
      mwPointer mxGetM, mxGetN
 
C     Pointers to input/output mxArrays:
      mwPointer x_pr, y_pr
 
C     Array information:
      mwPointer m, n
      mwSize size

C     Arguments for mxCreateNumericArray
      integer*4 classid
      integer*4 complexflag
      mwSize ndim
      mwSize dims(2)
 
C     Arguments for computational routine:
C     Input and output Fortran arrays
C     Maximum size = numel
      integer*4 numel
      parameter(numel = 1000)
      integer*1  x(numel), y(numel)
 
C-----------------------------------------------------------------------
C     Check for proper number of arguments. 
      if(nrhs .ne. 1) then
         call mexErrMsgIdAndTxt ('MATLAB:matsqint8:nInput',
     +     'One input required.')
      elseif(nlhs .ne. 1) then
         call mexErrMsgIdAndTxt ('MATLAB:matsqint8:nOutput',
     +     'One output required.')
      endif

C     Validate inputs
C     Check for proper input type
      if(mxIsInt8(prhs(1)) .eq. 0) then
         call mexErrMsgIdAndTxt ('MATLAB:matsqint8:WrongType',
     +     'Input must be int8 array.')
      endif

C     Column * row must be smaller than numel
C     numel is limit set by x, y declaration.
C     Get the size of the input array.
      m = mxGetM(prhs(1))
      n = mxGetN(prhs(1))
      size = m*n
      if(size .gt. numel) then
         call mexErrMsgIdAndTxt ('MATLAB:matsqint8:mSize',
     +     'Row * Column must be <= 1000.')
      endif
      
C     Create matrix for the return argument.
      classid = mxClassIDFromClassName('int8')
      complexflag = 0
      ndim = 2
      dims(1) = m
      dims(2) = n
      plhs(1) = mxCreateNumericArray(ndim, dims, 
     +          classid, 
     +          complexflag)

      x_pr = mxGetPr(prhs(1))
      y_pr = mxGetPr(plhs(1))

C     Create a Fortran array x from the input,
C     which is the mxArray x_pr.
      call mxCopyPtrToInteger1(x_pr,x,size)

C     Call the computational subroutine.
      call matsq(y, x, m, n)

C     Load the Fortran data y into y_pr, 
C     which is the output to MATLAB.
      call mxCopyInteger1ToPtr(y,y_pr,size)     

      return
      end

C-----------------------------------------------------------------------
C     Computational routine

      subroutine matsq(y, x, m, n)
      mwsize m, n, i, j
      integer*1 x(m,n), y(m,n)
C     
      do 20 i=1,m
         do 10 j=1,n
            y(i,j)= x(i,j)**2
 10      continue
 20   continue
      return
      end
