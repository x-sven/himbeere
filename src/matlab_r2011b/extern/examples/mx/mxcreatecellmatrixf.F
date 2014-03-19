C===================================================================
#include "fintrf.h"
#if 0
C     
C     mxcreatecellmatrixf.F
C     .F file needs to be preprocessed to generate .for equivalent
C     
#endif
C     mxcreatecellmatrixf.f 
C 
C     mxcreatecellmatrix takes the input arguments and places them in a
C     cell. It then displays the contents of the cell through a
C     mexCallMATLAB call.
C 
C 
C     This is a MEX-file for MATLAB.  
C     Copyright 1984-2011 The MathWorks, Inc.
C     All rights reserved.
C     $Revision: 1.2.2.4 $
C===================================================================

      subroutine mexFunction(nlhs, plhs, nrhs, prhs)

C     Declarations
      implicit none

      mwPointer plhs(*), prhs(*)
      integer nlhs, nrhs
      mwPointer mxCreateCellMatrix, mxDuplicateArray
      mwPointer cell_array_ptr, rhs(1)
      mwSize i, m, n
      mwPointer NULL

C     Check for proper number of input and output arguments    
      if (nrhs .lt. 1) then
         call mexErrMsgIdAndTxt( 'MATLAB:mxcreatecellmatrixf:minrhs',
     +    'One input argument required.')
      end if
      if (nlhs .gt. 1) then
         call mexErrMsgIdAndTxt( 'MATLAB:mxcreatecellmatrixf:maxlhs',
     +     'Too many output arguments.')
      end if

C     Create a nrhs x 1 cell mxArray.
      m = nrhs
      n = 1
      cell_array_ptr = mxCreateCellMatrix(m, n)
    
C     Fill cell matrix with input arguments
      do 10 i=1,m
        call mxSetCell(cell_array_ptr,i,
     +                     mxDuplicateArray(prhs(i)))
   10 continue  
           
      rhs(1) = cell_array_ptr
    
C     Call mexCallMATLAB to display the cell 
      call mexPrintf('The contents of the created cell is:')
      call mexPrintf(char(10))
      call mexCallMATLAB(0,NULL,1,rhs,'disp')

C     Cleanup the un-freed memory after calling mexCallMATLAB.
      call mxDestroyArray(rhs(1))
    
      return
      end

