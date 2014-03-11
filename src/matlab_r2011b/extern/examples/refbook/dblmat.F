#include "fintrf.h"
C=======================================================================
#if 0
C     
C     dblmat.F
C     .F file needs to be preprocessed to generate .for equivalent
C     
#endif
C     
C     dblmat.f
C     example for illustrating how to use %VAL
C     doubles the input matrix. The demo only handles real part of input.
C     NOTE: if your FORTRAN compiler does not support %VAL, 
C     use mxCopy_routine.
C
C     NOTE: The subroutine compute() is in the file called compute.f. 
C     This is a MEX-file for MATLAB.
C     Copyright 1984-2011 The MathWorks, Inc.
C     $Revision: 1.13.2.7 $
C=======================================================================
C     Gateway subroutine
      subroutine mexfunction(nlhs, plhs, nrhs, prhs)

C     Declarations
      implicit none

C     mexFunction arguments:
      mwPointer plhs(*), prhs(*)
      integer nlhs, nrhs

C     Function declarations:
      mwPointer mxGetPr
      mwPointer mxCreateDoubleMatrix
      mwPointer mxGetM, mxGetN

C     Pointers to input/output mxArrays:
      mwPointer pr_in, pr_out

C     Array information:
      mwPointer m_in, n_in
      mwSize size

C-----------------------------------------------------------------------
C     Check for proper number of arguments. 
      if(nrhs .ne. 1) then
         call mexErrMsgIdAndTxt ('MATLAB:dblmat:nInput',
     +                           'One input required.')
      endif
      if(nlhs .gt. 1) then
         call mexErrMsgIdAndTxt ('MATLAB:dblmat:nOutput',
     +                           'Too many output arguments.')
      endif
         
C     Get the size of the input array.
      m_in = mxGetM(prhs(1))
      n_in = mxGetN(prhs(1))
      size = m_in * n_in

C     Create Fortran array from the input argument.
      pr_in = mxGetPr(prhs(1))

C     Create matrix for the return argument.
      plhs(1) = mxCreateDoubleMatrix(m_in, n_in, 0)
      pr_out = mxGetPr(plhs(1))

C     Call the computational routine.
      call compute(%VAL(pr_out), %VAL(pr_in), size)

      return
      end
