#include "fintrf.h"
C=====================================================================
#if 0
C     
C     passstr.F
C     .F file needs to be preprocessed to generate .for equivalent
C     
#endif
C     
C     passstr.F is an example for illustrating passing a character 
C     matrix from FORTRAN to MATLAB.
C 
C     It passes a string array/character matrix into MATLAB as output
C     arguments rather than placing it directly into the workspace. 
C      
C     Copyright 1984-2009 The MathWorks, Inc.
C     $Revision: 1.11.6.8 $
C=====================================================================
C     Gateway routine
      subroutine mexFunction(nlhs, plhs, nrhs, prhs)

C     Declarations
      implicit none

C     mexFunction arguments:
      mwPointer plhs(*), prhs(*)
      integer nlhs, nrhs
 
C     Function declarations:
      mwPointer mxCreateString
      integer mexCallMATLAB
 
C     Pointers to input/output mxArrays:
      mwPointer input(1)
 
      integer status
      mwSize i, m, n
      character*75 thestring
      character*15 string(5)

C-----------------------------------------------------------------------
C     Create the string to be passed into MATLAB.
      string(1) = 'MATLAB         '
      string(2) = 'The Scientific '
      string(3) = 'Computing      '
      string(4) = 'Environment    '
      string(5) = '   by TMW, Inc.'
      
C     Concatenate the set of 5 strings into a long string.
      thestring = string(1)
      do 10 i=2,5
         thestring = thestring(:((i-1)*15)) // string(i)
 10   continue
      
C     Create the string matrix to be passed into MATLAB. 
      input(1) = mxCreateString(thestring)

C     Set the matrix size to be M=15 and N=5.
      m = 15
      call mxSetM(input(1), m)
      n = 5
      call mxSetN(input(1), n)
      
C     Transpose the resulting matrix in MATLAB because 
C     Fortran stores arrays as column major.
      status = mexCallMATLAB(1, plhs, 1, input, 'transpose')

C     Cleanup the un-freed memory after calling mexCallMATLAB.
      call mxDestroyArray(input(1))

      return
      end
