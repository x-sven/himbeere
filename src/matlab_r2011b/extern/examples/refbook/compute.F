#include "fintrf.h"
C=======================================================================
#if 0
C     
C     compute.F
C     .F file needs to be preprocessed to generate .for equivalent
C     
#endif
C     
C     compute.f
C
C     This subroutine doubles the input matrix. Your version of 
C     compute() may do whaveter you would like it to do.
C
C     This is a MEX-file for MATLAB.
C     Copyright 1984-2009 The MathWorks, Inc.
C     $Revision: 1.3.6.4 $
C=======================================================================
C     Computational subroutine
      subroutine compute(out_mat, in_mat, size)

C     Declarations
      implicit none

      mwSize size, i
      real*8  out_mat(*), in_mat(*)

      do 10 i=1,size
         out_mat(i) = 2*in_mat(i)
 10   continue

      return
      end
