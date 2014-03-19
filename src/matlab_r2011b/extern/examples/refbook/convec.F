#include "fintrf.h"      
C======================================================================
C
#if 0
C     
C     convec.F
C     .F file needs to be preprocessed to generate .for equivalent
C     
#endif
C     
C     convec.f
C     Example for illustrating how to pass complex data from MATLAB
C     to FORTRAN (using COMPLEX data type) and back again.
C
C     Convolves two complex input vectors
C      
C     This is a MEX-file for MATLAB.
C     Copyright 1984-2011 The MathWorks, Inc.
C     $Revision: 1.15.2.5 $
C======================================================================
C     Computational subroutine

      subroutine convec(x,y,z,nx,ny)
      complex*16 x(*), y(*), z(*)
      mwSize nx, ny

C     Initialize the output array
      do 10 i=1,nx+ny-1
         z(i) = (0.0,0.0)
 10   continue

      do 30 i=1,nx
         do 20 j=1,ny
            z(i+j-1) = z(i+j-1) + x(i) * y(j)
 20      continue
 30   continue
      return
      end

C-----------------------------------------------------------------------
C     Gateway routine
      subroutine mexFunction(nlhs, plhs, nrhs, prhs)

C     Declarations
      implicit none

C     mexFunction arguments:
      mwPointer plhs(*), prhs(*)
      integer nlhs, nrhs

C     Function declarations:
      mwPointer mxGetPr, mxGetPi
      mwPointer mxCreateDoubleMatrix
      mwPointer mxGetM, mxGetN
      integer mxIsComplex

C     Array information:
      mwPointer mx, nx, my, ny, nz
      mwSize row

      complex*16 x(100), y(100), z(199)

C-----------------------------------------------------------------------
C     Check for proper number of arguments. 
      if (nrhs .ne. 2) then
         call mexErrMsgIdAndTxt ('MATLAB:convec:nInput',
     +                           'Two inputs required.')
      elseif (nlhs .gt. 1) then
         call mexErrMsgIdAndTxt ('MATLAB:convec:nOutput',
     +                           'Too many output arguments.')
      endif

C     Validate inputs
      mx = mxGetM(prhs(1))
      nx = mxGetN(prhs(1))
      my = mxGetM(prhs(2))
      ny = mxGetN(prhs(2))

C     Check that inputs are row vectors.
      if(mx .ne. 1 .or. my .ne. 1) then
         call mexErrMsgIdAndTxt ('MATLAB:convec:NonRowVector',
     +                           'Inputs must be row vectors.')
C     Check size of the inputs.
      elseif(nx .gt. 100 .or. ny .gt. 100) then
         call mexErrMsgIdAndTxt ('MATLAB:convec:MaxElements',
     +                 'Inputs must have less than 100 elements.')
C     Check if inputs are complex.
      elseif ((mxIsComplex(prhs(1)) .ne. 1) .or.           
     +        (mxIsComplex(prhs(2)) .ne. 1)) then
         call mexErrMsgIdAndTxt ('MATLAB:convec:NonComplex',
     +                           'Inputs must be complex.')
      endif

C     Create the output array.
      row = 1
      nz = nx+ny-1
      plhs(1) = mxCreateDoubleMatrix(row, nz, 1)

C     Load the data into Fortran arrays(native COMPLEX data).
      call mxCopyPtrToComplex16(mxGetPr(prhs(1)),
     +                          mxGetPi(prhs(1)),x,nx)
      call mxCopyPtrToComplex16(mxGetPr(prhs(2)),
     +                          mxGetPi(prhs(2)),y,ny)

C     Call the computational subroutine.
      call convec(x,y,z,nx,ny)

C     Load the output into a MATLAB array.
      call mxCopyComplex16ToPtr(z,mxGetPr(plhs(1)),
     +                          mxGetPi(plhs(1)),nz)

      return
      end
