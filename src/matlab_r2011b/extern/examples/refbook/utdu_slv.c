/* $Revision: 1.2.6.8 $ $Date: 2011/05/16 22:56:10 $ */
/*=========================================================
 * utdu_slv.c
 * example for illustrating how to use LAPACK within a C
 * MEX-file on Windows or Linux.  This differs from the
 * other platforms in that the LAPACK symbols are not
 * exported with underscores e.g., dsysvx instead of dsysvx_
 *
 * UTDU_SLV Solves the symmetric indefinite system of linear 
 * equations A*X=B for X.
 * X = UTDU_SLV(A,B) computes a symmetric (Hermitian) indefinite 
 * factorization of A and returns the result X such that A*X is B. 
 * B must have as many rows as A.
 *
 * This is a MEX-file for MATLAB.
 * Copyright 1984-2011 The MathWorks, Inc.
 *=======================================================*/

#if !defined(_WIN32)
#define zhesvx zhesvx_
#define dsysvx dsysvx_
#endif

#include "mex.h"
#include "fort.h"
#include "stddef.h"

extern void zhesvx(
    char   *fact,
    char   *uplo,
    ptrdiff_t    *n,
    ptrdiff_t    *nrhs,
    double *a,
    ptrdiff_t    *lda,
    double *af,
    ptrdiff_t    *ldaf,
    ptrdiff_t    *ipiv,
    double *b,
    ptrdiff_t    *ldb,
    double *x,
    ptrdiff_t    *ldx,
    double *rcond,
    double *ferr,
    double *berr,
    double *work,
    ptrdiff_t    *lwork,
    double *rwork,
    ptrdiff_t    *info
);

extern void dsysvx(
    char   *fact,
    char   *uplo,
    ptrdiff_t    *n,
    ptrdiff_t    *nrhs,
    double *a,
    ptrdiff_t    *lda,
    double *af,
    ptrdiff_t    *ldaf,
    ptrdiff_t    *ipiv,
    double *b,
    ptrdiff_t    *ldb,
    double *x,
    ptrdiff_t    *ldx,
    double *rcond,
    double *ferr,
    double *berr,
    double *work,
    ptrdiff_t    *lwork,
    ptrdiff_t    *iwork,
    ptrdiff_t    *info
);

void mexFunction( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] )
{

	/* mex interface to LAPACK functions dsysvx and zhesvx */

    char fact[2] = {'N','\0'}, uplo[2] = {'U','\0'};
    char msg[101];
    int cplx;
    ptrdiff_t info, ldaf, ldx, lwork;
    ptrdiff_t *ipiv, *iwork=NULL;
    size_t n, nrhsb, lda, ldb;
    double *A, *AF, *b, *x, rcond, *ferr, *berr, *work1, *work, *rwork=NULL;
	
    if ((nlhs > 1) || (nrhs != 2)) {
      mexErrMsgIdAndTxt( "MATLAB:utdu_slv:invalidNumInputOutput",
              "Expect 2 input arguments and return 1 output argument");
    }

    n = mxGetN(prhs[0]);
    nrhsb = mxGetN(prhs[1]);
    lda = mxGetM(prhs[0]);
    if (lda != n) {
      mexErrMsgIdAndTxt( "MATLAB:utdu_slv:inputNotSquareSymmetric",
              "Matrix must be square and symmetric");
    }
    cplx = (mxGetPi(prhs[0]) || mxGetPi(prhs[1]));
    if (cplx) {
      A = mat2fort(prhs[0],lda,n);
      AF = (double *)mxCalloc(2*lda*n,sizeof(double));
    } else {
      A = mxGetPr(prhs[0]);
      AF = (double *)mxCalloc(lda*n,sizeof(double));
    }
    ldaf = lda;
    ipiv = (ptrdiff_t *)mxCalloc(n,sizeof(ptrdiff_t));
    ldb = mxGetM(prhs[1]);
    if (lda != ldb) {
      mexErrMsgIdAndTxt( "MATLAB:utdu_slv:inputSizeMismatch",
              "A and b must have the same number of rows");
    }
    ldx = ldb;
    ferr = (double *)mxCalloc(nrhsb,sizeof(double));
    berr = (double *)mxCalloc(nrhsb,sizeof(double));
    lwork = -1;
    info = 0;
    if (cplx) {
      b = mat2fort(prhs[1],ldb,nrhsb);
      x = (double *)mxCalloc(2*ldb*nrhsb,sizeof(double));
      work1 = (double *)mxCalloc(2,sizeof(double));
      rwork = (double *)mxCalloc(n,sizeof(double));
      /* Query zhesvx on the value of lwork */
      zhesvx ( fact, uplo, &n, &nrhsb, A, &lda, AF, &ldaf, ipiv, b, &ldb,
        x, &ldx, &rcond, ferr, berr, work1, &lwork, rwork, &info );
        if (info < 0) {
          sprintf(msg, "Input %d to zhesvx had an illegal value",-info);
          mexErrMsgIdAndTxt( "MATLAB:utdu_slv:illegelInputTozhesvx", msg);
        }
      lwork = (ptrdiff_t)(work1[0]);
      work = (double *)mxCalloc(2*lwork,sizeof(double));
        zhesvx ( fact, uplo, &n, &nrhsb, A, &lda, AF, &ldaf, ipiv, b, &ldb,
          x, &ldx, &rcond, ferr, berr, work, &lwork, rwork, &info );
        if (info < 0) {
          sprintf(msg, "Input %d to zhesvx had an illegal value",-info);
          mexErrMsgIdAndTxt( "MATLAB:utdu_slv:illegelInputTozhesvx", msg);
        }
    } else {
      b = mxGetPr(prhs[1]);
      x = (double *)mxCalloc(ldb*nrhsb,sizeof(double));
      work1 = (double *)mxCalloc(1,sizeof(double));
      iwork = (ptrdiff_t *)mxCalloc(n,sizeof(ptrdiff_t));
      /* Query dsysvx on the value of lwork */
      dsysvx ( fact, uplo, &n, &nrhsb, A, &lda, AF, &ldaf, ipiv, b, &ldb,
              x, &ldx, &rcond, ferr, berr, work1, &lwork, iwork, &info );
      if (info < 0) {
          sprintf(msg, "Input %d to dsysvx had an illegal value",-info);
          mexErrMsgIdAndTxt( "MATLAB:utdu_slv:illegelInputTodhesvx", msg);
      }
      lwork = (ptrdiff_t)(work1[0]);
      work = (double *)mxCalloc(lwork,sizeof(double));
      dsysvx ( fact, uplo, &n, &nrhsb, A, &lda, AF, &ldaf, ipiv, b, &ldb,
              x, &ldx, &rcond, ferr, berr, work, &lwork, iwork, &info );
      if (info < 0) {
          sprintf(msg, "Input %d to dsysvx had an illegal value",-info);
          mexErrMsgIdAndTxt( "MATLAB:utdu_slv:illegelInputTodhesvx", msg);
      }
    }

    if (rcond == 0) {
      sprintf(msg,"Matrix is singular to working precision.");
      mexErrMsgIdAndTxt( "MATLAB:utdu_slv:inputSingular", msg);
    } else if (rcond < mxGetEps()) {
      sprintf(msg,"Matrix is close to singular or badly scaled.\n"
        "         Results may be inaccurate. RCOND = %g",rcond);
      mexWarnMsgIdAndTxt( "MATLAB:utdu_slv:inputBadlyScaled", msg);
    }

    if (cplx) {
      plhs[0] = fort2mat(x,ldx,ldx,nrhsb);
      mxFree(A);
      mxFree(b);
      mxFree(rwork);
    } else {
      plhs[0] = mxCreateDoubleMatrix(0,0,0);
      mxSetPr(plhs[0],x);
      mxSetM(plhs[0], ldx);
      mxSetN(plhs[0], nrhsb);
      mxFree(iwork);
    }

    mxFree(AF);
    mxFree(ipiv);
    mxFree(ferr);
    mxFree(berr);
    mxFree(work1);
    mxFree(work);
}
