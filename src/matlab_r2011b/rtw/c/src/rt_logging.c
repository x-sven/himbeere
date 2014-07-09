/* $Revision: 1.1.6.20 $
 *
 * Copyright 1994-2010 The MathWorks, Inc.
 *
 * File: rt_logging.c
 *
 * Abstract:
 *	Real-Time Workshop data logging routines using circular buffers of
 *      fixed size.  The buffers are allocated at start, filled in at each
 *      major time step and finally written to a MAT-file at the end of the
 *      simulation.
 *
 *      This file handles redefining the following standard MathWorks types
 *      (see tmwtypes.h):
 *         [u]int8_T     to be int32_T (logged as Matlab [u]int32)
 *         [u]int16_T    to be int32_T (logged as Matlab [u]int32)
 *         real_T        to be real32_T (logged as Matlab single)
 *
 *
 */


#define PUBLIC
#define BEGIN_PUBLIC
#define END_PUBLIC

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

BEGIN_PUBLIC
#include <stddef.h>                     /* size_t */
#include "rtwtypes.h"
#include "rt_mxclassid.h"
#include "rtw_matlogging.h"


#ifndef TMW_NAME_LENGTH_MAX
#define TMW_NAME_LENGTH_MAX 64
#endif
#define mxMAXNAM  TMW_NAME_LENGTH_MAX	/* maximum name length */

/*=========*
 * Defines *
 *=========*/

/*
 * Logging related functions and data structures
 */
typedef double MatReal;                /* "real" data type used in model.mat  */
typedef struct LogVar_Tag LogVar;
typedef struct StructLogVar_Tag StructLogVar;

typedef struct MatrixData_Tag {
  char_T         name[mxMAXNAM];     /* Name of the variable                  */
  int_T          nRows;              /* number of rows                        */
  int_T          nCols;              /* number of columns                     */
  int_T          nDims;              /* number of dimensions                  */
  int_T          _dims[2];           /* most cases, dimensions are 2          */
  int_T          *dims;              /* dimensions of the log variable we 
                                        write to at each simulation time step.
                                        E.g: (1) Non-frame data - 
                                                 Signal dimension = [2 X 3]
                                                 numDims = 2
                                                 dims[0] = 2, dims[1] = 3
                                             (2) Frame data - 
                                                 Signal dimension = [2 X 3]
                                                 numDims = 1
                                                 dims[0] = 3                  */
  void           *re;                /* pointer to real part of the data      */
  void           *im;                /* pointer to imaginary part, if complex */
  DTypeId        dTypeID;            /* data type id                          */
  size_t         elSize;             /* element size in bytes                 */

  RTWLogDataTypeConvert dataTypeConvertInfo;

  mxClassID      mxID;               /* mxId corresponding to this dTypeID    */
  uint32_T       logical;            /* is this a logical array ?             */
  uint32_T       complex;            /* is this a complex matrix?             */
  uint32_T       frameData;          /* is this data frame based?             */
  uint32_T       frameSize;          /* is this data frame based?             */
} MatrixData;

typedef struct ValDimsData_Tag {
  char_T         name[mxMAXNAM];     /* Name of the variable                  */
  int_T          nRows;              /* number of rows                        */
  int_T          nCols;              /* number of columns                     */
  void           **currSigDims;      /* dimensions of current output          */
  int_T          *currSigDimsSize;   /* size in bytes of current sig dims     */
  real_T         *dimsData;          /* pointer to the value of dimension     */
} ValDimsData;

struct LogVar_Tag {
    MatrixData  data;                 /* Container for name, data etc.,       */
    ValDimsData *valDims;             /* field of valueDimensions
                                       1. If all logging signals are fixed-size,
                                          then we set this field to NULL;
                                       2. If any logging signal is variable-size,
                                          then this field will be needed:
                                         1) For fixed-size signal, this field is
                                            an empty matrix;
                                         2) Otherwise, it contains the dimension
                                            information of the logging signal.
                                      */
    int_T      rowIdx;                /* current row index                    */
    int_T      wrapped;               /* number of times the circular buffer
                                       * has wrapped around                   */
    int_T     nDataPoints;            /* total number of data points logged   */
    int_T     usingDefaultBufSize;    /* used to print a message at end       */
    int_T     okayToRealloc;          /* reallocate during sim?               */
    int_T     decimation;             /* decimation factor                    */
    int_T     numHits;                /* decimation hit count                 */

    int_T     *coords;
    int_T     *strides;
    int_T     *currStrides;           /* coords, strides and currStrides will be
                                         needed when logging variable-size 
                                         signal to calculate whether the 
                                         currently logging value is in the range.
                                         If the current signal is fixed-size,
                                         these pointers will be set to NULLs;
                                         otherwise, we allocate memory for them.
                                         (the size will be nDims in this case)
                                      */

    LogVar    *next;
};

typedef struct SignalsStruct_Tag {
    int_T        numActiveFields;   /* number of active fields                */
    const char_T *fieldNames;
    int_T        numSignals;
    LogVar       *values;
    MatrixData   *dimensions;
    MatrixData   *labels;
    MatrixData   *plotStyles;
    MatrixData   *titles;
    MatrixData   *blockNames;
    MatrixData   *stateNames;
    MatrixData   *crossMdlRef;

    boolean_T    logValueDimensions; /* If there's any variable-size signal 
                                         we also should log 'valueDimensions' field
                                      */
    boolean_T    *isVarDims; /* is this signal a variable-size signal? */
} SignalsStruct;

struct StructLogVar_Tag {
    char_T        name[mxMAXNAM];   /* Name of the ML Struct variable         */
    int_T         numActiveFields;  /* number of active fields                */
    boolean_T     logTime;
    void          *time;
    SignalsStruct signals;
    MatrixData    *blockName;

    StructLogVar  *next;
};

#define matUNKNOWN                  0
#define	matINT8                     1
#define	matUINT8                    2
#define	matINT16                    3
#define	matUINT16                   4
#define	matINT32                    5
#define	matUINT32                   6
#define	matFLOAT                    7
#define	matDOUBLE                   9
#define matINT64                   12
#define matUINT64                  13
#define	matMATRIX                  14

/* status of logging "valueDimensions" field */
/* 
  NO_LOGVALDIMS: 
                 No need to log valueDimensions: 
                 All signals are fixed-sized.

  LOGVALDIMS_EMPTYMX: 
                 Signals with mixed dimension modes,
                 and the signal logged currently
                 is fixed-sized. So set valueDimensions
                 field to an empty matrix.

  LOGVALDIMS_VARDIMS:
                 Signal logged currently is variable-sized.
*/
typedef enum {
    NO_LOGVALDIMS,      
    LOGVALDIMS_EMPTYMX,
    LOGVALDIMS_VARDIMS
} LogValDimsStat;

END_PUBLIC

#define matLOGICAL_BIT          0x200
#define matCOMPLEX_BIT          0x800

#define matKEY                 0x4D49
#define matVERSION             0x0100
#define matVERSION_INFO_OFFSET   124L

#define matINT64_ALIGN(e)      ( ( ((unsigned)(e))+7 ) & (~7) )
#define matTAG_SIZE            (sizeof(int32_T) << 1)

#ifndef DEFAULT_BUFFER_SIZE
#define DEFAULT_BUFFER_SIZE      1024     /* used if maxRows=0 and Tfinal=0.0 */
#endif

#define FREE(m) if (m != NULL) free(m)

/*===========*
 * typedef's *
 *===========*/

typedef struct LogInfo_Tag {
    LogVar       *t;		       /* Time log variable                   */
    void         *x;		       /* State log variable                  */
    void         *sl;                  /* SigLog log variable                 */
    int_T        ny;	               /* Length of "y" log variables         */
    void         **y;		       /* Output log vars                     */
    void         *xFinal;              /* Final state log variable            */

    LogVar       *logVarsList;         /* Linked list of all LogVars          */
    StructLogVar *structLogVarsList;   /* Linked list of all StructLogVars    */

    boolean_T   haveLogVars;           /* Are logging one or more vars?       */
} LogInfo;

typedef struct MatItem_tag {
  int32_T    type;
  int32_T    nbytes;
  const void *data;
} MatItem;

typedef enum {
    DATA_ITEM,
    MATRIX_ITEM,
    STRUCT_LOG_VAR_ITEM,
    SIGNALS_STRUCT_ITEM
} ItemDataKind;

/*===========*
 * Constants *
 *===========*/

static const char_T rtMemAllocError[] = "Memory allocation error";

#define ZEROS32 "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

#if mxMAXNAM==32

#define ZERO_PAD

#elif mxMAXNAM==64

#define ZERO_PAD ZEROS32

#elif mxMAXNAM==128

#define ZERO_PAD   ZEROS32  ZEROS32  ZEROS32

#else

#error "Cannot Handle mxMAXNAM other than 32,64, and 128"

#endif
/* field names: for vaiable-size signal logging */
static const char_T rtStructLogVarFieldNames[] =
                  "time\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" ZERO_PAD
                  "signals\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" ZERO_PAD
                  "blockName\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" ZERO_PAD;
static const char_T rtLocalLoggingSignalsStructFieldNames[] =
                  "values\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" ZERO_PAD
		  "valueDimensions\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" ZERO_PAD
		  "dimensions\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" ZERO_PAD
                  "label\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" ZERO_PAD
                  "title\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" ZERO_PAD
                  "plotStyle\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" ZERO_PAD;
static const char_T rtGlobalLoggingSignalsStructFieldNames[] =
                  "values\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" ZERO_PAD
                  "valueDimensions\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" ZERO_PAD
		  "dimensions\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" ZERO_PAD
                  "label\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" ZERO_PAD
                  "blockName\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" ZERO_PAD
                  "stateName\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" ZERO_PAD
                  "inReferencedModel\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" ZERO_PAD;
#define TIME_FIELD_NAME      (rtStructLogVarFieldNames[0*mxMAXNAM])
#define SIGNALS_FIELD_NAME   (rtStructLogVarFieldNames[1*mxMAXNAM])
#define BLOCKNAME_FIELD_NAME (rtStructLogVarFieldNames[2*mxMAXNAM])

#define VALUES_FIELD_NAME    (rtLocalLoggingSignalsStructFieldNames[0*mxMAXNAM])
#define VALUEDIMENSIONS_FIELD_NAME (rtLocalLoggingSignalsStructFieldNames[1*mxMAXNAM])
#define DIMENSION_FIELD_NAME (rtLocalLoggingSignalsStructFieldNames[2*mxMAXNAM])
#define LABEL_FIELD_NAME     (rtLocalLoggingSignalsStructFieldNames[3*mxMAXNAM])
#define TITLE_FIELD_NAME     (rtLocalLoggingSignalsStructFieldNames[4*mxMAXNAM])
#define PLOTSTYLE_FIELD_NAME (rtLocalLoggingSignalsStructFieldNames[5*mxMAXNAM])

#define STATENAME_FIELD_NAME (rtGlobalLoggingSignalsStructFieldNames[5*mxMAXNAM])
#define CROSS_MDL_REF_FIELD_NAME (rtGlobalLoggingSignalsStructFieldNames[6*mxMAXNAM])

/* field names: for fixed-size signal logging */
static const char_T rtLocalLoggingSignalsStructFieldNames_noValDims[] =
                  "values\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" ZERO_PAD
		  "dimensions\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" ZERO_PAD
                  "label\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" ZERO_PAD
                  "title\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" ZERO_PAD
                  "plotStyle\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" ZERO_PAD;
static const char_T rtGlobalLoggingSignalsStructFieldNames_noValDims[] =
                  "values\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" ZERO_PAD
		  "dimensions\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" ZERO_PAD
                  "label\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" ZERO_PAD
                  "blockName\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" ZERO_PAD
                  "stateName\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" ZERO_PAD
                  "inReferencedModel\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" ZERO_PAD;

extern real_T rtInf; /* declared by rt_nonfinite.c */
extern real_T rtNaN;
extern real_T rtNaNF;

/*================*
 * Local routines *
 *================*/

/* Forward declarations */
LogVar *rt_CreateLogVarWithConvert(RTWLogInfo        *li,
                                   const real_T      startTime,
                                   const real_T      finalTime,
                                   const real_T      inStepSize,
                                   const char_T      **errStatus,
                                   const char_T      *varName,
                                   BuiltInDTypeId    inpDataTypeID,
                                   const RTWLogDataTypeConvert *pDataTypeConvertInfo,
                                   int_T             logical,
                                   int_T             complex,
                                   int_T             frameData,
                                   int_T             nCols,
                                   int_T             nDims,
                                   const int_T       *dims,
                                   LogValDimsStat    logVarDimsStat,
                                   void              **currSigDims,
                                   int_T             *currSigDimsSize,
                                   int_T             maxRows,
                                   int_T             decimation,
                                   real_T            sampleTime,
                                   int_T             appanedToLogVarsList);


/* Function: rt_GetSizeofDataType ==============================================
 * Abstract:
 *      Get the element size in bytes given the data type id.
 */
static size_t rt_GetSizeofDataType(BuiltInDTypeId dTypeID)
{
    size_t elSz = 0; /* unknown */

    switch (dTypeID) {
      case SS_DOUBLE:
        elSz = sizeof(real_T);
        break;
      case SS_SINGLE:
        elSz = sizeof(real32_T);
        break;
      case SS_INT8:
        elSz = sizeof(int8_T);
        break;
      case SS_UINT8:
        elSz = sizeof(uint8_T);
        break;
      case SS_INT16:
        elSz = sizeof(int16_T);
        break;
      case SS_UINT16:
        elSz = sizeof(uint16_T);
        break;
      case SS_INT32:
        elSz = sizeof(int32_T);
        break;
      case SS_UINT32:
        elSz = sizeof(uint32_T);
        break;
      case SS_BOOLEAN:
        elSz = sizeof(boolean_T);
        break;
    }
    return(elSz);

} /* end: rt_GetSizeofDataType */


/* Function: rt_GetDataTypeConvertInfo
 * Abstract:
 *     if pointer to structure is non-NULL then directly copy
 *     otherwise set to default
 */
static RTWLogDataTypeConvert rt_GetDataTypeConvertInfo(
    const RTWLogDataTypeConvert *pDataTypeConvertInfo,
    BuiltInDTypeId dTypeID
    )
{
    RTWLogDataTypeConvert dataTypeConvertInfoCopy;

    if ( pDataTypeConvertInfo == NULL )
    {
        dataTypeConvertInfoCopy.conversionNeeded = 0;
        dataTypeConvertInfoCopy.dataTypeIdLoggingTo = dTypeID;
        dataTypeConvertInfoCopy.dataTypeIdOriginal  = (DTypeId)dTypeID;
        dataTypeConvertInfoCopy.bitsPerChunk = 0;
        dataTypeConvertInfoCopy.numOfChunk = 0;
        dataTypeConvertInfoCopy.isSigned = 0;
        dataTypeConvertInfoCopy.fracSlope = 1.0;
        dataTypeConvertInfoCopy.fixedExp = 0;
        dataTypeConvertInfoCopy.bias = 0.0;
    }
    else
    {
        dataTypeConvertInfoCopy = *pDataTypeConvertInfo;
    }

    return dataTypeConvertInfoCopy;
}

static double rt_GetDblValueFromOverSizedData(
    const void *pVoid, 
    int bitsPerChunk, 
    int numOfChunk,
    unsigned int isSigned, 
    double fracSlope, 
    int fixedExp, 
    double bias)
{
    double retValue = 0;

    double *dblValue = (double *) calloc(numOfChunk, sizeof(double));

    int i;    
    double isSignedNeg;

    if(isSigned)
    {
        const chunk_T *pData = (const chunk_T *) (pVoid);
        for (i = 0; i <numOfChunk; i++)
        {
            dblValue[i] = (double)(pData[i]);
        }
    }
    else
    {
        const uchunk_T *pData = (const uchunk_T *) (pVoid);
        for (i = 0; i <numOfChunk; i++)
        {
            dblValue[i] = (double)(pData[i]);
        }
    }

    /* 
     Assuming multi chunks b_n ... b_2 b_1 b_0, and the length of each chunk is N.
     Suppose b_i is the i-th chunk's value.
     Then for unsigned data or data with one chunk: we have
     retValue = b_n * 2^(n*N) + ... + b_1 * 2^N + b_0 * 2^0;
     But for signed data, we have
     retValue = b_n * 2^(n*N) + ... + b_1 * 2^N + b_0 * 2^0+ (b_0<0) * 2^N + 
     ... (b_(n-1) <0) * 2^(n*N) 
     = (b_n + (b_(n-1)<0)) * 2^(n*N) +... + (b_1 + (b_0<0)) * 2^N + b_0 * 2^0;
     Together:
     retValue = 
     (b_n + isSigned * (b_(n-1)<0)) * 2^(n*N) +... + (b_1 + isSigned * (b_0<0)) * 2^N + b_0 * 2^0;
    */

    retValue = dblValue[numOfChunk - 1];
    
    for(i = numOfChunk - 1; i > 0; i--)
    {
        isSignedNeg = dblValue[i - 1] < 0 ? (double)isSigned : 0;
        retValue = retValue + isSignedNeg;

        retValue = ldexp(retValue, bitsPerChunk)+ dblValue[i-1];
    }
    retValue = ldexp( fracSlope * retValue, fixedExp ) + bias;

    FREE(dblValue);
    return (retValue);
}

/* Function: rt_GetNonBoolMxIdFromDTypeId =====================================
 * Abstract:
 *      Get the mx???_CLASS given the simulink buildin data type id.
 */
mxClassID rt_GetNonBoolMxIdFromDTypeId(BuiltInDTypeId dTypeID)
{
    mxClassID mxID;

    switch (dTypeID) {
      case SS_DOUBLE:
        mxID = (sizeof(real_T)==4? mxSINGLE_CLASS: mxDOUBLE_CLASS);
        break;
      case SS_SINGLE:
        mxID = mxSINGLE_CLASS;
        break;
      case SS_INT8:
        mxID = (sizeof(int8_T)==4? mxINT32_CLASS: mxINT8_CLASS);
        break;
      case SS_UINT8:
        mxID = (sizeof(uint8_T)==4? mxUINT32_CLASS: mxUINT8_CLASS);
        break;
      case SS_INT16:
        mxID = (sizeof(int16_T)==4? mxINT32_CLASS: mxINT16_CLASS);
        break;
      case SS_UINT16:
        mxID = (sizeof(uint16_T)==4? mxUINT32_CLASS: mxUINT16_CLASS);
        break;
      case SS_INT32:
        mxID = mxINT32_CLASS;
        break;
      case SS_UINT32:
        mxID = mxUINT32_CLASS;
        break;
        /*case SS_BOOLEAN:
          mxID = (sizeof(boolean_T)==4? mxUINT32_CLASS: mxLOGICAL_CLASS);
          break;*/
      default:
        mxID = mxUNKNOWN_CLASS;
        break;
    }

    return(mxID);

} /* end: rt_GetNonBoolMxIdFromDTypeId */

BEGIN_PUBLIC
#ifdef __cplusplus
extern "C" {
#endif
END_PUBLIC
/* Function: rt_GetMxIdFromDTypeIdForRSim=======================================
 * Abstract:
 *      Get the mx???_CLASS given the simulink buildin data type id.
 */
PUBLIC mxClassID rt_GetMxIdFromDTypeIdForRSim(BuiltInDTypeId dTypeID)
{
    mxClassID mxID;

    if (dTypeID == SS_BOOLEAN) {
        mxID = (sizeof(boolean_T)==4? mxUINT32_CLASS: mxLOGICAL_CLASS);
    } else {
        mxID = rt_GetNonBoolMxIdFromDTypeId(dTypeID);
    }

    return(mxID);

} /* end: rt_GetMxIdFromDTypeIdForRSim */
BEGIN_PUBLIC
#ifdef __cplusplus
}
#endif
END_PUBLIC

BEGIN_PUBLIC
#ifdef __cplusplus
extern "C" {
#endif
END_PUBLIC
/* Function: rt_GetMxIdFromDTypeId =============================================
 * Abstract:
 *      Get the mx???_CLASS given the simulink buildin data type id.
 */
PUBLIC mxClassID rt_GetMxIdFromDTypeId(BuiltInDTypeId dTypeID)
{
    mxClassID mxID;

    if (dTypeID == SS_BOOLEAN) {
        mxID = (sizeof(boolean_T)==4? mxUINT32_CLASS: mxUINT8_CLASS);
    } else {
        mxID = rt_GetNonBoolMxIdFromDTypeId(dTypeID);
    }
    return(mxID);

} /* end: rt_GetMxIdFromDTypeId */
BEGIN_PUBLIC
#ifdef __cplusplus
}
#endif
END_PUBLIC


/* Function: rt_GetMatIdFromMxId ===============================================
 * Abstract:
 *      Get the MatId given the mxClassID.
 */
static int_T rt_GetMatIdFromMxId(mxClassID mxID)
{
    int_T matID;

    switch (mxID) {
      case mxCELL_CLASS:
      case mxSTRUCT_CLASS:
      case mxOBJECT_CLASS:
        matID = -1;
        break;
      case mxCHAR_CLASS:
        matID = matUINT16;
        break;
      case mxDOUBLE_CLASS:
        matID = matDOUBLE;
        break;
      case mxSINGLE_CLASS:
        matID = matFLOAT;
        break;
      case mxINT8_CLASS:
        matID = matINT8;
        break;
      case mxUINT8_CLASS:
        matID = matUINT8;
        break;
      case mxINT16_CLASS:
        matID = matINT16;
        break;
      case mxUINT16_CLASS:
        matID = matUINT16;
        break;
      case mxINT32_CLASS:
        matID = matINT32;
        break;
      case mxUINT32_CLASS:
        matID = matUINT32;
        break;
      case mxINT64_CLASS:
        matID = matINT64;
        break;
      case mxUINT64_CLASS:
        matID = matUINT64;
        break;
      default:
        matID = matUNKNOWN;
        break;
    }
    return(matID);

} /* end: rt_GetMatIdFromMxId */


static int_T rt_WriteItemToMatFile(FILE         *fp,
                                   MatItem      *pItem,
                                   ItemDataKind dataKind);

/* Function: rt_ProcessMatItem =================================================
 * Abstract:
 *      This routine along with rt_WriteItemToMatFile() write out a specified
 *      mat-item the .mat file. Note that if the input argument
 *          cmd == 0, then this function just calculates the size of the item.
 *          cmd <> 0, this function writes the mat-item to the file.
 *      Return values is
 *           -1 : coding/logic error
 *            0 : upon success
 *          > 0 : upon write failure (1)
 */
static int_T rt_ProcessMatItem(FILE         *fp,
                               MatItem      *pItem,
                               ItemDataKind itemKind,
                               int_T        cmd)
{
    mxClassID    mxID          = mxUNKNOWN_CLASS;
    uint32_T     arrayFlags[2] = {0, 0};
    int32_T      *dims         = NULL;
    int32_T      _dims[3]      = {0, 0, 0};
    int32_T      nDims         = 2;
    int32_T      nBytesInItem  = 0;
    const char_T *itemName;
    MatItem      item;
    int_T        retStat       = 0;

    switch (itemKind) {
      case DATA_ITEM: {
          (void)fprintf(stderr,"Unexpected itemKind = DATA_ITEM in "
                               "rt_ProcessMatItem @A\n");
          retStat = -1;
          goto EXIT_POINT;
      }
      case MATRIX_ITEM: {
          const MatrixData *var = (const MatrixData *) pItem->data;

          mxID           = var->mxID;
          arrayFlags[0]  = mxID;
          arrayFlags[0] |= var->logical;
          arrayFlags[0] |= var->complex;
	  if(var->nDims < 2){
              dims         = _dims;
	      dims[0]      = var->nRows;
	      dims[1]      = var->nCols;
	      nDims        = 2;
	  }else if(var->nDims >= 2){
              int32_T k;
              dims = (int32_T*)malloc(sizeof(int32_T)*(var->nDims+1));
              for (k = 0; k < var->nDims; k++) {
                  dims[k] = var->dims[k];
              }
              dims[var->nDims] = var->nRows;
	      nDims = var->nDims + 1;
	  }
          itemName       = var->name;
          break;
      }
      case STRUCT_LOG_VAR_ITEM: {
          const StructLogVar *var = (const StructLogVar *) pItem->data;

          mxID          = mxSTRUCT_CLASS;
          arrayFlags[0] = mxID;
          dims          = _dims;
          dims[0]       = 1;
          dims[1]       = 1;
          itemName      = var->name;
          break;
      }
      case SIGNALS_STRUCT_ITEM: {
          const SignalsStruct *var = (const SignalsStruct *) pItem->data;

          mxID          = mxSTRUCT_CLASS;
          arrayFlags[0] = mxID;
          dims          = _dims;
          dims[0]       = 1;
          dims[1]       = var->numSignals;
          itemName      = &SIGNALS_FIELD_NAME;
          break;
      }
      default:
        (void)fprintf(stderr,"Unexpected itemKind=%d in rt_ProcessMatItem @B\n",
                      itemKind);
        retStat = -1;
        goto EXIT_POINT;
    }

    /* array flags */
    item.nbytes = 2*sizeof(uint32_T);
    if (cmd) {
        item.type = matUINT32;
        item.data = arrayFlags;
        if (rt_WriteItemToMatFile(fp,&item, DATA_ITEM)) {
            retStat = 1;
            goto EXIT_POINT;
        }
    } else {
        /*LINTED E_CAST_INT_TO_SMALL_INT*/
        nBytesInItem += matINT64_ALIGN(matTAG_SIZE + item.nbytes);
    }
    /* dimensions */
    /*LINTED E_ASSIGN_INT_TO_SMALL_INT*/
    item.nbytes = nDims*sizeof(int32_T);
    if (cmd) {
        item.type = matINT32;
        item.data = dims;
        if (rt_WriteItemToMatFile(fp,&item, DATA_ITEM)) {
            retStat = 1;
            goto EXIT_POINT;            
        }
    } else {
        /*LINTED E_CAST_INT_TO_SMALL_INT*/
        nBytesInItem += matINT64_ALIGN(matTAG_SIZE + item.nbytes);
    }
    /* name */
    item.nbytes = (int32_T)strlen(itemName);
    if (cmd) {
        item.type = matINT8;
        item.data = (const char_T*) itemName;
        if (rt_WriteItemToMatFile(fp,&item, DATA_ITEM)) {
            retStat = 1;
            goto EXIT_POINT;
        }
    } else {
        nBytesInItem += (item.nbytes <= 4) ? /*LINTED E_CAST_INT_TO_SMALL_INT*/
                        matTAG_SIZE : matINT64_ALIGN(matTAG_SIZE + item.nbytes);
    }

    if (itemKind == MATRIX_ITEM) {
        const MatrixData *var   = (const MatrixData*) pItem->data;
        int_T            matID  = rt_GetMatIdFromMxId(mxID);
        size_t           elSize = var->elSize;

        /* data */
        item.nbytes = (int32_T)(var->nRows * var->nCols * elSize);
        if (cmd) {
            item.type = matID;
            item.data = var->re;
            if (rt_WriteItemToMatFile(fp, &item, DATA_ITEM)) {
                retStat = 1;
                goto EXIT_POINT;
            }
        } else {
            nBytesInItem += (item.nbytes <= 4) ? /*LINTED*/
                        matTAG_SIZE : matINT64_ALIGN(matTAG_SIZE + item.nbytes);
        }
        /* imaginary part */
        if (var->complex) {
            item.nbytes = (int32_T)(var->nRows * var->nCols * elSize);
            if (cmd) {
                item.type = matID;
                item.data = var->im;
                if (rt_WriteItemToMatFile(fp, &item, DATA_ITEM)) {
                    retStat = 1;
                    goto EXIT_POINT;
                }
            } else {
                nBytesInItem += (item.nbytes <= 4) ? /*LINTED*/
                        matTAG_SIZE : matINT64_ALIGN(matTAG_SIZE + item.nbytes);
            }
        }
    } else {  /* some type of structure item */
        const char_T *fieldNames;
        int_T        sizeofFieldNames;

        /* field names */
        switch (itemKind) {
          case STRUCT_LOG_VAR_ITEM: {
              const StructLogVar *var = (const StructLogVar *) pItem->data;
              fieldNames        = rtStructLogVarFieldNames;
              sizeofFieldNames  = var->numActiveFields * mxMAXNAM;
              break;
          }
          case SIGNALS_STRUCT_ITEM: {
              const SignalsStruct *var = (const SignalsStruct *) pItem->data;
              fieldNames        = var->fieldNames;
              sizeofFieldNames  = var->numActiveFields * mxMAXNAM;
              break;
          }
          default:
            (void)fprintf(stderr, "Unexpected itemKind=%d in "
                          "rt_ProcessMatItem @C\n", itemKind);
            retStat = -1;
            goto EXIT_POINT;
        }

        /* write field names */
        if (cmd) {
            int32_T tmpInt = mxMAXNAM;

            item.nbytes = sizeof(int32_T);
            item.type   = matINT32;
            item.data   = &tmpInt;
            if (rt_WriteItemToMatFile(fp,&item, DATA_ITEM)) {
                retStat = 1;
                goto EXIT_POINT;
            }

            item.nbytes = sizeofFieldNames;
            item.type   = matINT8;
            item.data   = (const char_T*) fieldNames;
            if (rt_WriteItemToMatFile(fp,&item, DATA_ITEM)) {
                retStat = 1;
                goto EXIT_POINT;
            }
        } else {
            /*LINTED E_CAST_INT_TO_SMALL_INT*/
            nBytesInItem += matINT64_ALIGN( matTAG_SIZE + matTAG_SIZE +
                                            sizeofFieldNames );
        }

        /* process each field of the structure */
        switch (itemKind) {
          case STRUCT_LOG_VAR_ITEM: {
              const StructLogVar *var = pItem->data;

              /* time */
              {
                  const void *data = var->time;

                  if (var->logTime) { /* time is a LogVar, get the MatrixData */
                      data = &(((const LogVar*) (var->time))->data);
                  }

                  item.type = matMATRIX;
                  item.data = data;
                  if (cmd) {
                      if (rt_WriteItemToMatFile(fp,&item,MATRIX_ITEM)){
                          retStat = 1;
                          goto EXIT_POINT;
                      }
                  } else {
                      if (rt_ProcessMatItem(fp, &item, MATRIX_ITEM,0)){
                          retStat = 1;
                          goto EXIT_POINT;
                      }
                      nBytesInItem += item.nbytes + matTAG_SIZE;
                  }
              }

              /* signals */
              item.type = matMATRIX;
              item.data = &(var->signals);
              if (cmd) {
                  if (rt_WriteItemToMatFile(fp,&item,SIGNALS_STRUCT_ITEM)) {
                      retStat = 1;
                      goto EXIT_POINT;
                  }
              } else {
                  if (rt_ProcessMatItem(fp, &item, SIGNALS_STRUCT_ITEM,0)) {
                      retStat = 1;
                      goto EXIT_POINT;
                  }
                  nBytesInItem += item.nbytes + matTAG_SIZE;
              }

              /* block name */
              if (var->blockName != NULL) {
                  item.type = matMATRIX;
                  item.data = var->blockName;
                  if (cmd) {
                      if (rt_WriteItemToMatFile(fp, &item, MATRIX_ITEM)) {
                          retStat = 1;
                          goto EXIT_POINT;
                      }
                  } else {
                      if (rt_ProcessMatItem(fp, &item, MATRIX_ITEM, 0)) {
                          retStat = 1;
                          goto EXIT_POINT;
                      }
                      nBytesInItem += item.nbytes + matTAG_SIZE;
                  }
              }
              break;
          }
          case SIGNALS_STRUCT_ITEM: {
              const SignalsStruct *var        = pItem->data;
              const LogVar        *values     = var->values;
	      const MatrixData    *dimensions = var->dimensions;
              const MatrixData    *labels     = var->labels;
              const MatrixData    *plotStyles = var->plotStyles;
              const MatrixData    *titles     = var->titles;
              const MatrixData    *blockNames = var->blockNames;
              const MatrixData    *stateNames = var->stateNames;
              const MatrixData    *crossMdlRef = var->crossMdlRef;
              const boolean_T logValueDimensions = var->logValueDimensions;
              int_T               i;

              for (i = 0; i < var->numSignals; i++) {
                  /* values */
                  item.type = matMATRIX;
                  item.data = &(values->data);
                  if (cmd) {
                      if (rt_WriteItemToMatFile(fp, &item,MATRIX_ITEM)) {
                          retStat = 1;
                          goto EXIT_POINT;
                      }
                  } else {
                      if (rt_ProcessMatItem(fp, &item, MATRIX_ITEM, 0)) {
                          retStat = 1;
                          goto EXIT_POINT;
                      }
                      nBytesInItem += item.nbytes + matTAG_SIZE;
                  }

                  if(logValueDimensions)
                  {
                      /* valueDimensions */
                      /* Since the functions rt_WriteItemToMatFile and 
                         rt_ProcessMatItem deal with MatrixData, 
                         convert valDims to tempData, and fill up the
                         necessary fields.
                      */
                      MatrixData  tempData;
                      (void)memcpy(tempData.name, &VALUEDIMENSIONS_FIELD_NAME, mxMAXNAM);
                      tempData.nRows = values->valDims->nRows;
                      tempData.nCols = values->valDims->nCols;
                      tempData.nDims = 1;
                      tempData._dims[0] = values->valDims->nCols;
                      tempData.re = values->valDims->dimsData;
                      tempData.im = NULL;
                      tempData.dTypeID = SS_DOUBLE;
                      tempData.elSize =  sizeof(real_T);
                      tempData.mxID = mxDOUBLE_CLASS;
                      tempData.logical = 0;
                      tempData.complex = 0;
                      tempData.frameData = 0;
                      tempData.frameSize = 1;

                      item.type = matMATRIX;                    
                      item.data = &tempData; /*values->valDims;*/

                      if (cmd) {
                          if (rt_WriteItemToMatFile(fp, &item,MATRIX_ITEM)) {
                              retStat = 1;
                              goto EXIT_POINT;
                          }
                      } else {
                          if (rt_ProcessMatItem(fp, &item, MATRIX_ITEM, 0)) {
                              retStat = 1;
                              goto EXIT_POINT;
                          }
                          nBytesInItem += item.nbytes + matTAG_SIZE;
                      }
                  }
                  values = values->next;

		  /* dimensions */
                  if (dimensions != NULL) {
                      item.type = matMATRIX;
                      item.data = &(dimensions[i]);
                      if (cmd) {
                          if (rt_WriteItemToMatFile(fp,&item, MATRIX_ITEM)) {
                              retStat = 1;
                              goto EXIT_POINT;
                          }
                      } else {
                          if (rt_ProcessMatItem(fp, &item, MATRIX_ITEM, 0)) {
                              retStat = 1;
                              goto EXIT_POINT;
                          }
                          nBytesInItem += item.nbytes + matTAG_SIZE;
                      }
                  }

                  /* label */
                  item.type = matMATRIX;
                  item.data = &(labels[i]);
                  if (cmd) {
                      if (rt_WriteItemToMatFile(fp, &item,MATRIX_ITEM)) {
                          retStat = 1;
                          goto EXIT_POINT;
                      }
                  } else {
                      if (rt_ProcessMatItem(fp, &item, MATRIX_ITEM, 0)) {
                          retStat = 1;
                          goto EXIT_POINT;
                      }
                      nBytesInItem += item.nbytes + matTAG_SIZE;
                  }
                  /* title */
                  if (titles != NULL) {
                      item.type = matMATRIX;
                      item.data = &(titles[i]);
                      if (cmd) {
                          if (rt_WriteItemToMatFile(fp, &item, MATRIX_ITEM)) {
                              retStat = 1;
                              goto EXIT_POINT;
                          }
                      } else {
                          if (rt_ProcessMatItem(fp, &item, MATRIX_ITEM, 0)) {
                              retStat = 1;
                              goto EXIT_POINT;
                          }
                          nBytesInItem += item.nbytes + matTAG_SIZE;
                      }
                  }
                  /* plot style */
                  if (plotStyles != NULL) {
                      item.type = matMATRIX;
                      item.data = &(plotStyles[i]);
                      if (cmd) {
                          if (rt_WriteItemToMatFile(fp,&item, MATRIX_ITEM)) {
                              retStat = 1;
                              goto EXIT_POINT;
                          }
                      } else {
                          if (rt_ProcessMatItem(fp, &item, MATRIX_ITEM, 0)) {
                              retStat = 1;
                              goto EXIT_POINT;
                          }
                          nBytesInItem += item.nbytes + matTAG_SIZE;
                      }
                  }
                  /* block name */
                  if (blockNames != NULL) {
                      item.type = matMATRIX;
                      item.data = &(blockNames[i]);
                      if (cmd) {
                          if (rt_WriteItemToMatFile(fp, &item, MATRIX_ITEM)) {
                              retStat = 1;
                              goto EXIT_POINT;
                          }
                      } else {
                          if (rt_ProcessMatItem(fp, &item, MATRIX_ITEM, 0)) {
                              retStat = 1;
                              goto EXIT_POINT;
                          }
                          nBytesInItem += item.nbytes + matTAG_SIZE;
                      }
                  }
                  /* state name */
                  if (stateNames != NULL) {
                      item.type = matMATRIX;
                      item.data = &(stateNames[i]);
                      if (cmd) {
                          if (rt_WriteItemToMatFile(fp, &item, MATRIX_ITEM)) {
                              retStat = 1;
                              goto EXIT_POINT;
                          }
                      } else {
                          if (rt_ProcessMatItem(fp, &item, MATRIX_ITEM, 0)) {
                              retStat = 1;
                              goto EXIT_POINT;
                          }
                          nBytesInItem += item.nbytes + matTAG_SIZE;
                      }
                  }
                  /* crossMdlRef */
                  if (crossMdlRef != NULL) {
                      item.type = matMATRIX;
                      item.data = &(crossMdlRef[i]);
                      if (cmd) {
                          if (rt_WriteItemToMatFile(fp, &item, MATRIX_ITEM)) {
                              retStat = 1;
                              goto EXIT_POINT;
                          }
                      } else {
                          if (rt_ProcessMatItem(fp, &item, MATRIX_ITEM, 0)) {
                              retStat = 1;
                              goto EXIT_POINT;
                          }
                          nBytesInItem += item.nbytes + matTAG_SIZE;
                      }
                  }
              } /* for i=1:numSignals */
              break;
          }
          default:
            (void)fprintf(stderr, "Unexpected itemKind=%d in "
                          "rt_ProcessMatItem @D\n", itemKind);
            retStat = -1;
            goto EXIT_POINT;
        }
    } /* end struct item */

    if (!cmd) {
        pItem->nbytes = nBytesInItem;
    }

  EXIT_POINT:
    if (dims != _dims) {
        FREE(dims);
    }
    return(retStat);

} /* end: rt_ProcessMatItem */



/* Function: rt_WriteItemToMatFile =============================================
 * Abstract:
 *      Entry function for writing out a mat item to the mat file.
 *
 *      Return values is
 *          == 0 : upon success
 *          <> 0 : upon failure
 */
static int_T rt_WriteItemToMatFile(FILE         *fp,
                                   MatItem      *pItem,
                                   ItemDataKind itemKind)
{
    /* Determine the item size */
    if (pItem->type == matMATRIX) {
        if (rt_ProcessMatItem(fp, pItem, itemKind, 0)) return(1);
    }

    /* Write the item tag and data */
    if (pItem->nbytes > 4) {
        int32_T nAlignBytes;

        if (fwrite(pItem, 1, matTAG_SIZE, fp) != matTAG_SIZE) return(1);

        if (pItem->type == matMATRIX) {
            if (rt_ProcessMatItem(fp, pItem, itemKind, 1)) return(1);
        } else {
            if ( fwrite(pItem->data, 1, pItem->nbytes, fp) !=
                                                    ((size_t) pItem->nbytes) ) {
                return(1);
            }
        }

        /* Add offset for 8-byte alignment */
        nAlignBytes = matINT64_ALIGN(pItem->nbytes) - pItem->nbytes;
        if (nAlignBytes > 0) {
            int pad[2] = {0, 0};
            if ( fwrite(pad,1,nAlignBytes,fp) != ((size_t) nAlignBytes) ) {
                return(1);
            }
        }
    } else {
        MatItem item = {0, 0, NULL};
        item.type = ((uint32_T)(pItem->type))|(((uint32_T)(pItem->nbytes))<<16);
        (void)memcpy(&item.nbytes, pItem->data, pItem->nbytes);
        if (fwrite(&item, 1, matTAG_SIZE, fp) != matTAG_SIZE) return(1);
    }

    return(0);

} /* end: rt_WriteItemToMatFile */



/* Function: rt_WriteMat5FileHeader ============================================
 * Abstract:
 *      Function to write the mat file header.
 *      Return values is
 *          == 0 : upon success
 *          <> 0 : upon failure
 */
static int_T rt_WriteMat5FileHeader(FILE *fp)
{
    int_T        nbytes;
    int_T        nspaces;
    int_T        i, n;
    unsigned short ver[2];
    char_T       spaces[16];
    const char_T *matversion = "MATLAB 5.0 MAT-file";

    (void)memset(spaces, ' ', sizeof(spaces));

    n = (int_T)strlen(matversion);
    nbytes = (int_T)fwrite(matversion, 1, n, fp);
    /*LINTED E_ASSIGN_INT_TO_SMALL_INT*/
    nspaces = matVERSION_INFO_OFFSET - nbytes;
    /*LINTED E_ASSIGN_INT_TO_SMALL_INT*/
    n = nspaces % sizeof(spaces);
    nbytes += (int_T)fwrite(spaces, 1, n, fp);
    /*LINTED E_ASSIGN_INT_TO_SMALL_INT*/
    n = nspaces / sizeof(spaces);
    for (i = 0; i < n; ++i) {
        nbytes += (int_T)fwrite(spaces, 1, sizeof(spaces), fp);
    }
    if (nbytes == matVERSION_INFO_OFFSET) {
        ver[0] = matVERSION;
        ver[1] = matKEY;
        nbytes += (int_T)fwrite(ver, 1, sizeof(ver), fp);
    }
    return(nbytes != matVERSION_INFO_OFFSET + sizeof(ver));

} /* end: rt_WriteMat5FileHeader */


/* Function: rt_FixupLogVar ================================================
 * Abstract:
 *	Make the logged variable suitable for MATLAB.
 */
static const char_T *rt_FixupLogVar(LogVar *var,int verbose)
{
    int_T  nCols   = var->data.nCols;
    int_T  maxRows = var->data.nRows;
    int_T  nDims   = var->data.nDims;
    size_t elSize  = var->data.elSize;
    int_T  nRows   = (var->wrapped ?  maxRows : var->rowIdx);

    var->nDataPoints = var->rowIdx + var->wrapped * maxRows;

    if (var->wrapped > 1 || (var->wrapped == 1 && var->rowIdx != 0)) {
        /*
         * Warn the user the circular buffer has wrapped, implying that
         * some data has been lost.
         */
        if( verbose) {
            (void)fprintf(stdout,
                          "*** Log variable %s has wrapped %d times\n"
                          "    using a circular buffer of size %d\n",
                          var->data.name, var->wrapped, var->data.nRows);
        }
        if (var->usingDefaultBufSize) {
            /*
             * If wrapping occurred using the default buffer size,
             * let the user know what size buffer to use in the
             * future to avoid wrapping.  If the default buffer
             * size was not used, the user has no control to specify
             * the correct value.  Wrapping may occur when not using
             * the default buffer if we allocated too small a buffer
             * size for this logvar.  One common case is a toWorkspace
             * block inside of an iterative subsystem - we can not take
             * the number of iterations into account (they may be
             * variable) when allocating the buffer.  In this case,
             * just warn the buffer wrapped and don't tell user they
             * can override the buffer size.
             */
            if( verbose ) {
                (void)fprintf(stdout,
                              "*** To avoid wrapping, explicitly specify a\n"
                              "    buffer size of %d in your Simulink model\n"
                              "    by adding OPTS=\"-DDEFAULT_BUFFER_SIZE=%d\"\n"
                              "    as an argument to the ConfigSet MakeCommand\n"
                              "    parameter\n",
                              var->nDataPoints, var->nDataPoints);
            }
        }
    }

    if (nDims < 2 && nCols > 1) {  /* Transpose? */
        /* Don't need to transpose valueDimensions */
        int_T  nEl    = nRows*nCols;
        char   *src   = var->data.re;
        char   *pmT;
        int_T  k;

        /**********************************
         * If memory cannot be allocated, *
         * write to a temporary buffer    *
         **********************************/
        if ((pmT = malloc(nEl*elSize)) == NULL) {
            FILE  *fptr;
            char  fName[mxMAXNAM];

            (void)sprintf(fName, "%s%s", var->data.name, "_rtw_tmw.tmw");
            if ((fptr=fopen(fName,"w+b")) == NULL) {
                (void)fprintf(stderr,"*** Error opening %s",fName);
                return("unable to open data file\n");
            }

            /****************************
             * Write the data to a file *
             ****************************/
            for (k=0; k<nEl; k++) {
                int_T kT   = nCols*(k%nRows) + (k/nRows);
                char  *dst = src + kT*elSize;
                (void)fwrite(dst, elSize, 1, fptr);
            }
            if (var->data.complex) {
                char *pmiT = var->data.re;
                src  = var->data.im;
                for (k=0; k<nEl; k++) {
                    int_T kT   = nRows*(k%nCols) + (k/nCols);
                    char  *dst = pmiT + kT*elSize;
                    (void)memcpy(dst, src, elSize);
                    src += elSize;
                }
                var->data.re = var->data.im;
                var->data.im = pmiT;
            }

            /*******************************
             * Read the data from the file *
             *******************************/
            (void)rewind(fptr);
            (void)fread(var->data.re, elSize, nEl, fptr);
            (void)fclose(fptr);
            (void)remove(fName);
        }
        else {
            for (k=0; k<nEl; k++) {
                int_T kT   = nRows*(k%nCols) + (k/nCols);
                char  *dst = pmT + kT*elSize;
                (void)memcpy(dst, src, elSize);
                src += elSize;
            }
            if (var->data.complex) {
                char *pmiT = var->data.re;
                src  = var->data.im;
                for (k=0; k<nEl; k++) {
                    int_T kT   = nRows*(k%nCols) + (k/nCols);
                    char  *dst = pmiT + kT*elSize;
                    (void)memcpy(dst, src, elSize);
                    src += elSize;
                }
                var->data.re = var->data.im;
                var->data.im = pmiT;
            }
            FREE(var->data.re);
            var->data.re = pmT;
        }
    } /* Transpose? */

    if (var->wrapped > 0 && var->rowIdx != 0 ) {  /* Rotate? */
        char_T *buffer    = var->data.re;
        int_T  done       = 0; /* done: 0 (1) rotate real (imag) part. */

        do{
            char_T *col       = buffer;
            int_T  rowOffset  = (int_T)((nDims == 1) ? (elSize) : (elSize * nCols));
            int_T  colOffset  = (int_T)((nDims == 1)?  (nRows*elSize) : elSize);
            int_T  zeroIdx    = var->rowIdx;
            int_T  j;

            for( j = 0 ; j < nCols; ++j, col += colOffset){
                int_T   swapCount;
                int_T   srcIdx;
                int_T   dstIdx;
                int_T   tmpIdx;
                MatReal tmp;

                for (tmpIdx=0, swapCount=0; swapCount < nRows; tmpIdx++) {
                    (void)memcpy(&tmp, col + tmpIdx*rowOffset, elSize);

                    dstIdx=tmpIdx; 
                    srcIdx = ((dstIdx + zeroIdx) % nRows);
                    while (srcIdx != tmpIdx) {
                        (void)memcpy(col + dstIdx*rowOffset,
                                     col + srcIdx*rowOffset,
                                     elSize);
                        ++swapCount;
                        dstIdx = srcIdx;
                        srcIdx = ((dstIdx + zeroIdx) % nRows);
                        
                    }
                    (void)memcpy(col + dstIdx*rowOffset, &tmp, elSize);
                    ++swapCount;
                }
            }
            done ++;
            /* need to rotate the imaginary part */
        }while ((done == 1) && ((buffer = var->data.im) != NULL));

        var->rowIdx = 0;
    } /* Rotate? */

    /*
     * We might have allocated more number of rows than the number of data
     * points that have been logged, in which case set nRows to nDataPoints
     * so that only these values get saved.
     */
    if (var->nDataPoints < var->data.nRows) {
        var->data.nRows = var->nDataPoints;
        if(var->valDims != NULL){
            size_t elSizeValDims = sizeof(real_T);
            int_T  k;
            real_T *dimsData = var->valDims->dimsData + nRows;
            /* 
               Keep nRows of values and that of valueDimensions consistent 
               for variable-size signals.
            */
            var->valDims->nRows = var->data.nRows;
            /*
               Also need to move data when shrinking the array size,
               because valueDimensions data is stored in array format. 
               e.g. maxRows = 4; nRows = 2; nDims = 3;
               Before fixing up the logVar, the locations of data are as below:
               (x, y, z -- useful data / o -- junk)
               a[0] = x    a[4] = y    a[8] = z
               a[1] = x    a[5] = y    a[9] = z
               a[2] = o    a[6] = o    a[10]= o
               a[3] = o    a[7] = o    a[11]= o
               After fixing up the logVar, we want the data to be stored as:
               a[0] = x    a[4] = z    a[8] = o
               a[1] = x    a[5] = z    a[9] = o
               a[2] = y    a[6] = o    a[10]= o
               a[3] = y    a[7] = o    a[11]= o
            */
            for(k = 1; k < nDims; k++){
                (void) memcpy(dimsData, 
                              var->valDims->dimsData + k*maxRows,
                              elSizeValDims * nRows);
                dimsData += nRows;
            }
        }
    }

     return(NULL);

} /* end rt_FixupLogVar */


/* Function: rt_LoadModifiedLogVarName =========================================
 * Abstract:
 *      The name of the logged variable is obtaind from the input argument
 *      varName and the nameMmodifier which is obtained from the simstruct. If
 *      the nameModifier begins with an '_', then nameModifier is post-pended to
 *      varName to obtain the name of the logged variable. If the first
 *      character does not begin with an '_', then the nameModifier is
 *      pre-pended to varName.
 *
 * Examples:
 *     a)  varName = "tout" & nameModifier = "_rt"  => logVarName = "tout_rt"
 *     b)  varName = "tout" & nameModifier = "rt_"  => logVarName = "rt_tout"
 *     c)  varName = "tout" & nameModifier = "none" => logVarName = "tout"
 */
static void rt_LoadModifiedLogVarName(const RTWLogInfo *li,         /* in  */
                                      const char       *varName,    /* in  */
                                      char             *logVarName) /* out */
{
    int_T        nameLen;
    const char_T *nameModifier = rtliGetLogVarNameModifier(li);

    if (nameModifier != NULL && strcmp(nameModifier,"none")==0) {
        nameModifier = NULL;
    }

    logVarName[mxMAXNAM-1] = '\0';
    if (nameModifier == NULL) {
        (void)strncpy(logVarName, varName, mxMAXNAM-1);
    } else if (nameModifier[0] == '_') {
        (void)strncpy(logVarName, varName, mxMAXNAM-1);
        nameLen = (int_T)strlen(logVarName);
        (void)strncat(logVarName, nameModifier, mxMAXNAM-1-nameLen);
    } else {
        (void)strncpy(logVarName, nameModifier, mxMAXNAM-1);
        nameLen = (int_T)strlen(logVarName);
        (void)strncat(logVarName, varName, mxMAXNAM-1-nameLen);
    }
} /* end rt_LoadModifiedLogVarName */



/* Function: rt_GetActualDTypeID ===============================================
 * Abstract:
 *	Given a built-in data type id, return the actual data type id.
 *	The only time these are different is when real_T has been mapped
 *	to a single.
 */
static BuiltInDTypeId rt_GetActualDTypeID(BuiltInDTypeId dTypeID)
{
    /*LINTED E_FALSE_LOGICAL_EXPR*/
    if (dTypeID == SS_DOUBLE && sizeof(real_T) != 8) {
        return(SS_SINGLE);
    } else {
        return(dTypeID);
    }
} /* end rt_GetActualDTypeID */



/* Function: rt_DestroyLogVar ==================================================
 * Abstract:
 *      Destroy the log var linked list.
 *
 */
static void rt_DestroyLogVar(LogVar *head)
{
    while(head) {
        LogVar *var = head;
        head = var->next;
        FREE(var->data.re);
        FREE(var->data.im);
        if (var->data.dims != var->data._dims) {
            FREE(var->data.dims);
        }
        /* free valDims if necessary */
        if(var->valDims != NULL) {
            FREE(var->valDims->dimsData);
            FREE(var->valDims);
        }
        /* free coords, strides and currStrides if necessary */
        FREE(var->coords);
        FREE(var->strides);
        FREE(var->currStrides);

        FREE(var);
    }
} /* end rt_DestroyLogVar */


/* Function: rt_DestroyStructLogVar ============================================
 * Abstract:
 *      Destroy the struct log var linked list.
 *
 */
static void rt_DestroyStructLogVar(StructLogVar *head)
{
    while(head) {
        StructLogVar *var = head;

        head = var->next;

        if (var->logTime) { /* time is LogVar */
            rt_DestroyLogVar(var->time);
        } else {        /* time is MatrixData */
            FREE(var->time);
        }
        rt_DestroyLogVar(var->signals.values);
        FREE(var->signals.labels);
        FREE(var->signals.plotStyles);
	FREE(var->signals.dimensions);
        FREE(var->signals.titles);
        FREE(var->signals.blockNames);
        FREE(var->signals.stateNames);
        FREE(var->signals.crossMdlRef);
        FREE(var->blockName);
        FREE(var);
    }
} /* end rt_DestroyStructLogVar */


/* Function: rt_InitSignalsStruct ==============================================
 * Abstract:
 *      Initialize the signals structure in the struct log variable.
 *
 * Returns:
 *	== NULL  => success.
 *	~= NULL  => failure, the return value is a pointer to the error
 *                           message, which is also set in the simstruct.
 */
static const char_T *rt_InitSignalsStruct(RTWLogInfo             *li,
                                          const real_T           startTime,
                                          const real_T           finalTime,
                                          const real_T           inStepSize,
                                          const char_T           **errStatus,
                                          StructLogVar           *var,
                                          int_T                  maxRows,
                                          int_T                  decimation,
                                          real_T                 sampleTime,
                                          const RTWLogSignalInfo *sigInfo)
{
    int_T                i, sigIdx;
    SignalsStruct        *sig          = &(var->signals);
    int_T                nSignals      = sigInfo->numSignals;
    const int_T          *numCols      = sigInfo->numCols;
    const int_T          *numDims      = sigInfo->numDims;
    const int_T          *dims         = sigInfo->dims;
    const BuiltInDTypeId *dTypes       = sigInfo->dataTypes;
    const int_T          *cSgnls       = sigInfo->complexSignals;
    const int_T          *fData        = sigInfo->frameData;
    const char_T         **labels      = sigInfo->labels.cptr;
    const int_T          *plotStyles   = sigInfo->plotStyles;
    const char_T         *titles       = sigInfo->titles;
    const int_T          *titleLen     = sigInfo->titleLengths;
    const char_T         **blockNames  = sigInfo->blockNames.cptr;
    const char_T         **stateNames  = sigInfo->stateNames.cptr;
    const boolean_T      *crossMdlRef  = sigInfo->crossMdlRef;

    void                 **currSigDims = sigInfo->currSigDims;
    int_T                *currSigDimsSize = sigInfo->currSigDimsSize;
    LogVar               *prevValues   = NULL;
    int_T                dimsOffset    = 0;
    boolean_T            *isVarDims    = sigInfo->isVarDims;
    /* if any signal is variable-size, the field 'valueDimensions' is needed */
    boolean_T            logValueDimensions = false;
    const RTWLogDataTypeConvert *pDTConvInfo = sigInfo->dataTypeConvert;

    /* reset error status */
    *errStatus = NULL;

    sig->numActiveFields = 1;
    sig->numSignals      = nSignals;

    sig->isVarDims = isVarDims;
    /* check whether we need valueDimensions field*/
    for (i=0; i<nSignals; i++){
        if(isVarDims[i]){
            logValueDimensions = true;
            break;
        }
    }

    /* values */
    dimsOffset = 0;
    for (i = 0; i < nSignals; i++) {
        BuiltInDTypeId dt = (dTypes) ? dTypes[i] : SS_DOUBLE;
        int_T          cs = (cSgnls) ? cSgnls[i] : 0;
        int_T          fd = (fData)  ? fData[i]  : 0;
        int_T          nd = (numDims) ? numDims[i] : 1;

        const RTWLogDataTypeConvert *pDTConvInfoCur =
                       (pDTConvInfo)  ? (pDTConvInfo+i)  : 0;

        LogVar *values = NULL;
        LogValDimsStat logValDimsStat;

        if(!logValueDimensions){
            logValDimsStat = NO_LOGVALDIMS;
        }
        else{
            logValDimsStat = isVarDims[i] ? LOGVALDIMS_VARDIMS :  
                                            LOGVALDIMS_EMPTYMX;
        }

        values = rt_CreateLogVarWithConvert(li, startTime, finalTime,
                                            inStepSize, errStatus,
                                            &VALUES_FIELD_NAME,
                                            dt, 
                                            pDTConvInfoCur,
                                            0, cs, fd,
                                            numCols[i],nd,
                                            dims + dimsOffset,
                                            logValDimsStat,
                                            currSigDims + dimsOffset,
                                            currSigDimsSize + dimsOffset,
                                            maxRows,decimation,sampleTime, 0);

        if (values == NULL) goto ERROR_EXIT;

        if (sig->values == NULL) {
            sig->values = values;
        } else {
            prevValues->next = values;
        }
        prevValues = values;
	dimsOffset += nd;
    }

    if(logValueDimensions){
        ++sig->numActiveFields;
        sig->logValueDimensions = true;
    }
    else{
        sig->logValueDimensions = false;
    }

    /* Dimensions */
    {
        real_T         *data;
	size_t	       nbytes;
        int_T          dataLen    = 0;
        BuiltInDTypeId dTypeId    = rt_GetActualDTypeID(SS_DOUBLE);
	size_t         dataOffset = nSignals*sizeof(MatrixData);
        uint_T         overhang   = (uint_T)(dataOffset % sizeof(real_T));

	if (overhang) {
	    dataOffset += (sizeof(real_T) - overhang);
	}
        for (i=0; i< nSignals; i++) {
            int_T nd = (numDims) ? numDims[i] : 1;
            dataLen += nd;
        }
        nbytes = dataOffset + dataLen*sizeof(real_T);

        if ( (sig->dimensions = calloc(nbytes, 1)) == NULL ) goto ERROR_EXIT;

        data = (real_T*) (((char_T*) (sig->dimensions)) + dataOffset);

        for (i = 0; i < dataLen; i++) {
          data[i] = dims[i]; /* cannot memcpy double <- int */
        }

        for (i = 0; i < nSignals; i++) {
            MatrixData *mtxData = &(sig->dimensions[i]);
            int_T nd = (numDims) ? numDims[i] : 1;

            (void)memcpy(mtxData->name, &DIMENSION_FIELD_NAME, mxMAXNAM);

            mtxData->nRows   = 1;
            mtxData->nCols   = nd;

	    mtxData->nDims   = 1; /* assume */
            mtxData->dims    = mtxData->_dims;
	    mtxData->dims[0] = mtxData->nCols;

            mtxData->re      = data;
            mtxData->im      = NULL;
            mtxData->dTypeID = dTypeId;
            mtxData->mxID    = rt_GetMxIdFromDTypeId(dTypeId);
            mtxData->elSize  = rt_GetSizeofDataType(dTypeId);
            mtxData->logical = 0U;
            mtxData->complex = 0U;

            data += nd;
        }
        ++sig->numActiveFields;
    }

    /* labels */
    if (labels != NULL)     {
        short   *data;
        size_t  nbytes;
        int_T   dataLen    = 0;
        size_t  dataOffset = nSignals * sizeof(MatrixData);
        uint_T  overhang   = (uint_T)(dataOffset % sizeof(short));
        int_T dataIdx = 0;

        for (i=0;i<nSignals;i++) {
            if (labels[i] != NULL){
                dataLen = dataLen + (int_T)strlen(labels[i]);
            }
        }

        if (overhang) {
            dataOffset += (sizeof(short) - overhang);
        }
        nbytes = dataOffset + dataLen*sizeof(short);

        if ( (sig->labels = calloc(nbytes, 1)) == NULL ) goto ERROR_EXIT;

        data = (short*) (((char_T*) (sig->labels)) + dataOffset);
        for(sigIdx=0;sigIdx<nSignals;sigIdx++) {
            int_T labelLen = (labels[sigIdx]==NULL) ? 0 : (int_T)strlen(labels[sigIdx]);
            for (i = 0; i < labelLen; i++) {
                data[dataIdx++] = labels[sigIdx][i];
            }
        }

        for (i = 0; i < nSignals; i++) {
            MatrixData *mtxData = &(sig->labels[i]);
            int_T labelLen = (int_T)strlen(labels[i]);

            (void)memcpy(mtxData->name, &LABEL_FIELD_NAME, mxMAXNAM);
            mtxData->nRows   = (labelLen) ? 1 : 0;
            mtxData->nCols   = labelLen;

            mtxData->re      = data;
            mtxData->im      = NULL;

	    mtxData->nDims   = 1; /* assume */
            mtxData->dims    = mtxData->_dims;
	    mtxData->dims[0] = mtxData->nCols;

            mtxData->dTypeID = SS_INT16;
            mtxData->mxID    = mxCHAR_CLASS;
            mtxData->elSize  = sizeof(short);
            mtxData->logical = 0U;
            mtxData->complex = 0U;

            data += labelLen;
        }
        ++sig->numActiveFields;
    }

    /* plot styles */
    if (plotStyles != NULL) {
        real_T         *data;
	size_t	       nbytes;
        int_T          dataLen    = 0;
        BuiltInDTypeId dTypeId    = rt_GetActualDTypeID(SS_DOUBLE);
        /*LINTED E_ASSIGN_INT_TO_SMALL_INT*/
	size_t         dataOffset = nSignals*sizeof(MatrixData);
        /*LINTED E_ASSIGN_INT_TO_SMALL_INT*/
        uint_T         overhang   = (uint_T)(dataOffset % sizeof(real_T));

	if (overhang) {
	    dataOffset += (sizeof(real_T) - overhang);
	}
        for (i=0; i< nSignals; i++) {
            dataLen += numCols[i];
        }
        /*LINTED E_ASSIGN_INT_TO_SMALL_INT*/
        nbytes = dataOffset + dataLen*sizeof(real_T);

        if ( (sig->plotStyles = calloc(nbytes, 1)) == NULL ) goto ERROR_EXIT;

        /*LINTED E_BAD_PTR_CAST_ALIGN*/
        data = (real_T*) (((char_T*) (sig->plotStyles)) + dataOffset);

        for (i = 0; i < dataLen; i++) {
            data[i] = plotStyles[i];
        }

        dimsOffset = 0;
        for (i = 0; i < nSignals; i++) {
            MatrixData *mtxData = &(sig->plotStyles[i]);
            
            (void)memcpy(mtxData->name, &PLOTSTYLE_FIELD_NAME, mxMAXNAM);

            mtxData->nRows   = (numCols[i]) ? 1 : 0;
            mtxData->nCols   = numCols[i];

	    mtxData->nDims   = numDims[i];
            
            if(mtxData->nDims > 2) {
                if ((mtxData->dims = calloc(mtxData->nDims, sizeof(int_T))) == NULL) goto ERROR_EXIT;
            } else {
                mtxData->dims    = mtxData->_dims;
            }
            
	    mtxData->dims[0] = *(dims + dimsOffset);
            if(mtxData->nDims >= 2) {
                int32_T j;
                for (j=1; j<mtxData->nDims; j++) {
                    mtxData->dims[j] = *(dims + dimsOffset + j);
                }
            }

            mtxData->re      = data;
            mtxData->im      = NULL;
            mtxData->dTypeID = dTypeId;
            mtxData->mxID    = rt_GetMxIdFromDTypeId(dTypeId);
            mtxData->elSize  = rt_GetSizeofDataType(dTypeId);
            mtxData->logical = 0U;
            mtxData->complex = 0U;

            data       += numCols[i];
            dimsOffset += numDims[i];
        }
        ++sig->numActiveFields;
    }

    /* titles */
    if (titles != NULL) {
        short  *data;
        size_t nbytes;
        int_T  dataLen    = (int_T)strlen(titles);
        /*LINTED E_ASSIGN_INT_TO_SMALL_INT*/
        size_t  dataOffset = nSignals * sizeof(MatrixData);
        /*LINTED E_ASSIGN_INT_TO_SMALL_INT*/
        uint_T  overhang   = (uint_T)(dataOffset % sizeof(short));

        if (overhang) {
            dataOffset += (sizeof(short) - overhang);
        }
        /*LINTED E_ASSIGN_INT_TO_SMALL_INT*/
        nbytes = dataOffset + dataLen*sizeof(short);

        if ( (sig->titles = calloc(nbytes, 1)) == NULL ) goto ERROR_EXIT;

        /*LINTED E_BAD_PTR_CAST_ALIGN*/
        data = (short*) (((char_T*) (sig->titles)) + dataOffset);
        for (i = 0; i < dataLen; i++) {
            data[i] = titles[i];
        }

        for (i = 0; i < nSignals; i++) {
            MatrixData *mtxData = &(sig->titles[i]);

            (void)memcpy(mtxData->name, &TITLE_FIELD_NAME, mxMAXNAM);
            if (titleLen) {
                mtxData->nRows   = (titleLen[i]) ? 1 : 0;
                mtxData->nCols   = titleLen[i];
            } else {
                mtxData->nRows   = (dataLen) ? 1 : 0;
                mtxData->nCols   = dataLen;
            }

	    mtxData->nDims   = 1; /* assume */
            mtxData->dims    = mtxData->_dims;
	    mtxData->dims[0] = mtxData->nCols;

            mtxData->re      = data;
            mtxData->im      = NULL;
            mtxData->dTypeID = SS_INT16;
            mtxData->mxID    = mxCHAR_CLASS;
            mtxData->elSize  = sizeof(short);
            mtxData->logical = 0U;
            mtxData->complex = 0U;

            data += ((titleLen) ? titleLen[i] : dataLen);
        }
        ++sig->numActiveFields;
    }

    /* block names */
    if (blockNames != NULL)     {
        short  *data;
        size_t nbytes;
        int_T  dataLen = 0;
        size_t dataOffset = nSignals * sizeof(MatrixData);
        uint_T overhang   = (uint_T)(dataOffset % sizeof(short));
        int_T  dataIdx = 0;

        for (i=0;i<nSignals;i++) {
            if (blockNames[i] != NULL) {
                dataLen = dataLen + (int_T)strlen(blockNames[i]);
            }
        }

        if (overhang) {
            dataOffset += (sizeof(short) - overhang);
        }

        nbytes = dataOffset + dataLen*sizeof(short);

        if ( (sig->blockNames = calloc(nbytes, 1)) == NULL ) goto ERROR_EXIT;

        data = (short*) (((char_T*) (sig->blockNames)) + dataOffset);

        for(sigIdx=0;sigIdx<nSignals;sigIdx++) {
            int_T nameLen = (blockNames[sigIdx]==NULL) ? 0 : 
                (int_T)strlen(blockNames[sigIdx]);
            for (i = 0; i < nameLen; i++) {
                data[dataIdx++] = blockNames[sigIdx][i];
            }
        }

        for (i = 0; i < nSignals; i++) {
            MatrixData *mtxData = &(sig->blockNames[i]);
            int_T blockNameLen = (int_T)strlen(blockNames[i]);

            (void)memcpy(mtxData->name, &BLOCKNAME_FIELD_NAME, mxMAXNAM);
            mtxData->nRows   = (blockNameLen) ? 1 : 0;
            mtxData->nCols   = blockNameLen;

	    mtxData->nDims   = 1; /* assume */
            mtxData->dims    = mtxData->_dims;
	    mtxData->dims[0] = mtxData->nCols;

            mtxData->re      = data;
            mtxData->im      = NULL;
            mtxData->dTypeID = SS_INT16;
            mtxData->mxID    = mxCHAR_CLASS;
            mtxData->elSize  = sizeof(short);
            mtxData->logical = 0U;
            mtxData->complex = 0U;

            data += blockNameLen;
        }
        ++sig->numActiveFields;
        if(logValueDimensions){
            sig->fieldNames = rtGlobalLoggingSignalsStructFieldNames;
        }
        else{
            sig->fieldNames = rtGlobalLoggingSignalsStructFieldNames_noValDims;
        }

    } else {
        if(logValueDimensions){
            sig->fieldNames = rtLocalLoggingSignalsStructFieldNames;
        }
        else{
            sig->fieldNames = rtLocalLoggingSignalsStructFieldNames_noValDims;
        }

    }

    /* state names */
    if (stateNames != NULL)     {
        short  *data;
        size_t nbytes;
        int_T  dataLen = 0;
        size_t dataOffset = nSignals * sizeof(MatrixData);
        uint_T overhang   = (uint_T)(dataOffset % sizeof(short));
        int_T  dataIdx = 0;

        for (i=0;i<nSignals;i++) {
            if (stateNames[i] != NULL) {
                dataLen = dataLen + (int_T)strlen(stateNames[i]);
            }
        }

        if (overhang) {
            dataOffset += (sizeof(short) - overhang);
        }

        nbytes = dataOffset + dataLen*sizeof(short);

        if ( (sig->stateNames = calloc(nbytes, 1)) == NULL ) goto ERROR_EXIT;

        data = (short*) (((char_T*) (sig->stateNames)) + dataOffset);

        for(sigIdx=0;sigIdx<nSignals;sigIdx++) {
            int_T nameLen = (stateNames[sigIdx]==NULL) ? 0 : 
                (int_T)strlen(stateNames[sigIdx]);
            for (i = 0; i < nameLen; i++) {
                data[dataIdx++] = stateNames[sigIdx][i];
            }
        }

        for (i = 0; i < nSignals; i++) {
            MatrixData *mtxData = &(sig->stateNames[i]);
            int_T stateNameLen = (int_T)strlen(stateNames[i]);

            (void)memcpy(mtxData->name, &STATENAME_FIELD_NAME, mxMAXNAM);
            mtxData->nRows   = (stateNameLen) ? 1 : 0;
            mtxData->nCols   = stateNameLen;

	    mtxData->nDims   = 1; /* assume */
            mtxData->dims    = mtxData->_dims;
	    mtxData->dims[0] = mtxData->nCols;

            mtxData->re      = data;
            mtxData->im      = NULL;
            mtxData->dTypeID = SS_INT16;
            mtxData->mxID    = mxCHAR_CLASS;
            mtxData->elSize  = sizeof(short);
            mtxData->logical = 0U;
            mtxData->complex = 0U;

            data += stateNameLen;
        }
        ++sig->numActiveFields;

        if(logValueDimensions){
            sig->fieldNames = rtGlobalLoggingSignalsStructFieldNames;
        }
        else{
            sig->fieldNames = rtGlobalLoggingSignalsStructFieldNames_noValDims;
        }

    }

    /* CrossMdlRef */
    if (crossMdlRef != NULL) {
        real_T  *data;
        size_t nbytes;
        size_t dataOffset = nSignals * sizeof(MatrixData);
        uint_T overhang   = (uint_T)(dataOffset % sizeof(real_T));

        if (overhang) {
            dataOffset += (sizeof(real_T) - overhang);
        }

        nbytes = dataOffset + nSignals*sizeof(real_T);

        if ( (sig->crossMdlRef = calloc(nbytes, 1)) == NULL ) goto ERROR_EXIT;

        data = (real_T*) (((char_T*) (sig->crossMdlRef)) + dataOffset);

        for(sigIdx=0;sigIdx<nSignals;sigIdx++) {
            data[sigIdx] = crossMdlRef[sigIdx];
        }

        for (i = 0; i < nSignals; i++) {
            MatrixData *mtxData = &(sig->crossMdlRef[i]);

            (void)memcpy(mtxData->name, &CROSS_MDL_REF_FIELD_NAME, mxMAXNAM);
            mtxData->nRows   = 1;
            mtxData->nCols   = 1;
            mtxData->nDims   = 1; /* => matlab scalar */

            mtxData->re      = &data[i];
            mtxData->im      = NULL;
            mtxData->dTypeID = SS_DOUBLE;
            mtxData->mxID    = rt_GetMxIdFromDTypeId(SS_DOUBLE);
            mtxData->elSize  = sizeof(real_T);
            mtxData->logical = matLOGICAL_BIT;
            mtxData->complex = 0U;
            mtxData->frameData = 0;
            mtxData->frameSize = 1;
        }
        ++sig->numActiveFields;
    }
    
    return(NULL); /* NORMAL_EXIT */

 ERROR_EXIT:

    (void)fprintf(stderr, "*** Error creating signals structure "
                  "in the struct log variable %s\n", var->name);
    if (*errStatus == NULL) {
        *errStatus = rtMemAllocError;
    }
    rt_DestroyLogVar(sig->values);
    FREE(sig->labels);
    FREE(sig->plotStyles);
    FREE(sig->dimensions);
    FREE(sig->titles);
    FREE(sig->blockNames);
    FREE(sig->stateNames);
    FREE(sig->crossMdlRef);

    return(*errStatus);

} /* end rt_InitSignalsStruct */





/* Function: local_CreateStructLogVar =============================================
 * Abstract:
 *	Create a logging variable in the structure format.
 *
 * Returns:
 *	~= NULL  => success, returns the log variable created.
 *	== NULL  => failure, error message set in the simstruct.
 */
static StructLogVar *local_CreateStructLogVar(
    RTWLogInfo              *li,
    const real_T            startTime,
    const real_T            finalTime,
    const real_T            inStepSize,
    const char_T            **errStatus,
    const char_T            *varName,
    boolean_T               logTime,
    int_T                   maxRows,
    int_T                   decimation,
    real_T                  sampleTime,
    const RTWLogSignalInfo  *sigInfo,
    const char_T            *blockName)
{
    StructLogVar *var;
    LogInfo      *logInfo = rtliGetLogInfo(li);

    /* reset error status */
    *errStatus = NULL;

    if ( (var = calloc(1, sizeof(StructLogVar))) == NULL ) goto ERROR_EXIT;

    var->numActiveFields = 2;

    /* Setup the structure name using varName and nameModifier */
    rt_LoadModifiedLogVarName(li,varName,var->name);

    /* time field */
    if (logTime) {
        /* need to create a LogVar to log time */
	int_T dims = 1;
        var->time = rt_CreateLogVarWithConvert(li, startTime, finalTime,
                                               inStepSize, errStatus,
                                               &TIME_FIELD_NAME, SS_DOUBLE, 
                                               NULL,
                                               0, 0, 0, 1,
                                               1, &dims, NO_LOGVALDIMS, 
                                               NULL, NULL, maxRows,
                                               decimation, sampleTime, 0);
        if (var->time == NULL) goto ERROR_EXIT;
    } else {
        /* create a dummy MatrixData to write out time as an empty matrix */
        BuiltInDTypeId dt     = rt_GetActualDTypeID(SS_DOUBLE);
        size_t         nbytes = sizeof(MatrixData);
        MatrixData     *time;

        if ( (var->time = calloc(nbytes, 1)) == NULL ) goto ERROR_EXIT;
        time = var->time;

        (void)memcpy(time->name, &TIME_FIELD_NAME, mxMAXNAM);
        time->nRows   = 0;
        time->nCols   = 0;
	time->nDims   = 0;
        time->re      = NULL;
        time->im      = NULL;
        time->dTypeID = dt;
        time->mxID    = rt_GetMxIdFromDTypeId(dt);
        time->elSize  = rt_GetSizeofDataType(dt);
        time->logical = 0U;
        time->complex = 0U;
    }
    var->logTime = logTime;

    /* signals field */
    if (sigInfo) {
        if (rt_InitSignalsStruct(li,startTime,finalTime,inStepSize,errStatus,
                                 var,maxRows,decimation,sampleTime,sigInfo)) {
            goto ERROR_EXIT;
        }
    }

    /* blockName Field */
    if (blockName != NULL) {
        int_T  dataLen = (int_T)strlen(blockName);
        size_t nbytes;
        size_t dataOffset = sizeof(MatrixData);
        /*LINTED E_ASSIGN_INT_TO_SMALL_INT*/
        uint_T overhang   = (uint_T)(dataOffset % sizeof(short));

        if (overhang) {
            dataOffset += (sizeof(short) - overhang);
        }
        /*LINTED E_ASSIGN_INT_TO_SMALL_INT*/
        nbytes = dataOffset + dataLen*sizeof(short);

        if ( (var->blockName = calloc(nbytes, 1)) == NULL ) goto ERROR_EXIT;

        (void)memcpy(var->blockName->name, &BLOCKNAME_FIELD_NAME, mxMAXNAM);
        var->blockName->nRows   = (dataLen) ? 1 : 0;
        var->blockName->nCols   = dataLen;

	var->blockName->nDims   = 1;
	var->blockName->dims    = var->blockName->_dims;
	var->blockName->dims[0] = dataLen;
        {
            /*LINTED E_BAD_PTR_CAST_ALIGN*/
            short *data = (short*)(((char_T*) (var->blockName))+dataOffset);
            int_T   i;

            for (i=0; i<dataLen; i++) {
                data[i] = blockName[i];
            }
            var->blockName->re  = data;
        }
        var->blockName->im      = NULL;
        var->blockName->dTypeID = SS_INT16;
        var->blockName->mxID    = mxCHAR_CLASS;
        var->blockName->elSize  = sizeof(short);
        var->blockName->logical = 0U;
        var->blockName->complex = 0U;

        ++var->numActiveFields;
    }

    /* Add this struct log var to the linked list in log info */
    {
        StructLogVar *list = logInfo->structLogVarsList;

        if (list != NULL) {
            while (list->next != NULL) {
                list = list->next;
            }
            list->next = var;
        } else {
            logInfo->structLogVarsList = var;
        }
    }

    return(var); /* NORMAL_EXIT */

 ERROR_EXIT:
    (void)fprintf(stderr, "*** Error creating log variable %s\n", varName);
    if (*errStatus == NULL) {
        *errStatus = rtMemAllocError;
    }
    rt_DestroyStructLogVar(var);
    return(NULL);

} /* end local_CreateStructLogVar */


/*==================*
 * Visible routines *
 *==================*/



/* Function: rt_CreateLogVarWithConvert ======================================
 * Abstract:
 *	Create a logging variable.
 *
 * Returns:
 *	~= NULL  => success, returns the log variable created.
 *	== NULL  => failure, error message set in the simstruct.
 */

BEGIN_PUBLIC
#ifdef __cplusplus
extern "C" {
#endif
END_PUBLIC
 
PUBLIC LogVar *rt_CreateLogVarWithConvert(
    RTWLogInfo        *li,
    const real_T      startTime,
    const real_T      finalTime,
    const real_T      inStepSize,
    const char_T      **errStatus,
    const char_T      *varName,
    BuiltInDTypeId    inpDataTypeID,
    const RTWLogDataTypeConvert *pDataTypeConvertInfo,
    int_T             logical,
    int_T             complex,
    int_T             frameData,
    int_T             nCols,
    int_T             nDims,
    const int_T       *dims,
    LogValDimsStat    logValDimsStat,
    void              **currSigDims,
    int_T             *currSigDimsSize,
    int_T             maxRows,
    int_T             decimation,
    real_T            sampleTime,
    int_T             appanedToLogVarsList)
{
    int_T          usingDefaultBufSize = 0;
    int_T          okayToRealloc       = 0;
    LogVar         *var                = NULL;
    /*inpDataTypeID is the rt_LoggedOutputDataTypeId*/
    BuiltInDTypeId dTypeID             = (BuiltInDTypeId)inpDataTypeID; 
    size_t         elementSize         = rt_GetSizeofDataType(dTypeID);
    int_T          frameSize;
    int_T          nRows;
    int_T          nColumns;

    /*===================================================================*
     * Determine the frame size if the data is frame based               *
     *===================================================================*/
    frameSize = frameData ? dims[0] : 1;

    /*===================================================================*
     * Try to figure out the maximum number of rows needed in the buffer *
     *===================================================================*/

    if (finalTime > startTime && finalTime != rtInf) {
        real_T nPoints;            /* Tfinal is finite  ===>  nRows can be  */
        real_T stepSize;           /* computed since the StepSize is fixed  */

        if (sampleTime == -2.0) {  /* The signal being logged is constant,  *
                                    * Hence, only one data point is logged. */
            stepSize = finalTime;
        } else if (sampleTime == -1.0 || sampleTime == 0.0) {
                                /* Signal being logged is either inside a    *
                                 * triggered sub-system or it is continuous. */
            stepSize = inStepSize;
        } else {                /* Discrete signal */
            stepSize = sampleTime;
        }

        if (stepSize == 0.0) {
            /* small initial value, so as to exercise the realloc code */
            nRows = maxRows+1;
            okayToRealloc = 1;
        } else {
            nPoints = 1.0 + floor((finalTime-startTime)/stepSize);

            /*
             * Add one more data point if needed.
             */
            if ( stepSize*(nPoints-1.0) < (finalTime-startTime) ) {
                nPoints += 1.0;
            }

            /*
             * Actual number of points to log = nPoints * size of
             * each frame if data is frame-based
             */
            nPoints = frameData ? (nPoints * frameSize) : nPoints;

            nPoints /= decimation;
            if (nPoints != floor(nPoints)) {
                nPoints += 1.0;
            }
            nRows = (nPoints <= INT_MAX) ? ((int_T) nPoints) : INT_MAX;
        }
        /*
         * If maxRows is specified, and if this number is less
         * than the number we computed (nRows) then use maxRows.
         */
        if ((maxRows > 0) && (maxRows < nRows)) {
            nRows = maxRows;
            okayToRealloc = 0;
        }
    } else if (finalTime == startTime) {
        /*
         * Number of rows to log is equal to 1 if not frame-based and
         * equal to frame size if frame-based
         */
        nRows = frameData ? frameSize : 1;

        /*
         * If maxRows is specified, and if this number is less
         * than the number we computed (nRows) then use maxRows.
         */
        if ((maxRows > 0) && (maxRows < nRows)) {
            nRows = maxRows;
        }
    } else if (maxRows > 0) {     /* maxRows is specified => nRows=maxRows  */
        nRows = maxRows;

    } else {

        if (inStepSize == 0) {
            /* small initial value, so as to exercise the realloc code */
            nRows = maxRows+1;
            okayToRealloc = 1;
        } else {                    /* Use a default value for nRows          */
            usingDefaultBufSize = 1;
            nRows = DEFAULT_BUFFER_SIZE;
            (void)fprintf(stdout, "*** Using a default buffer of size %d for "
                          "logging variable %s\n", nRows, varName);
        }
    }

    /*
     * Figure out the number of columns that the log variable should have.
     * If the data is not frame based, then number of columns should equal
     * nCols that is provided as input to the function. If the data is
     * frame-based, then the number of columns should be equal to the
     * number of channels = nCols/frameSize = dims[1];
     */
    nColumns = frameData ? dims[1] : nCols;

    /*
     * Error out if the size of the circular buffer is absurdly large, this
     * error message is more informative than the one we get when we try to
     * malloc this many number of bytes in one fell swoop.
     */
    {
        double tmpDbl = ((double)elementSize)*((double)nRows)*
                                              ((double)nColumns);

        if (tmpDbl >= UINT_MAX) {
            (void)fprintf(stderr,
                          "\n*** Memory required to log variable '%s' is too"
                          "\n    big. Use the 'Limit rows to last:' and (or)"
                          "\n    'Decimation:' options to reduce the required"
                          "\n    memory size.\n", varName);
            (void)fprintf(stderr, "*** Details:\n"
                          "       varName         = %s\n"
                          "       nRows           = %d\n"
                          "       nCols           = %d\n"
                          "       elementSize     = %lu\n"
                          "       Bytes Required  = %.16g\n\n",
                          varName, nRows, nColumns, (unsigned long)
                          elementSize, tmpDbl);
            goto ERROR_EXIT;
        }
    }

    /* Allocate memory for the log variable */
    if ( (var = calloc(1, sizeof(LogVar))) == NULL ) {
        (void)fprintf(stderr, "*** Error allocating memory for logging %s\n",
                      varName);
        goto ERROR_EXIT;
    }

    /* Allocate memory for the circular buffer (real part) */
    if ( (var->data.re = malloc(nRows*nColumns*elementSize)) == NULL ) {
        (void)fprintf(stderr,
                      "*** Error allocating memory for the circular buffer\n");
        (void)fprintf(stderr, "*** Details:\n"
                      "       varName         = %s\n"
                      "       nRows           = %d\n"
                      "       nCols           = %d\n"
                      "       elementSize     = %lu\n"
                      "       Bytes Requested = %.16g\n\n",
                      varName, nRows, nColumns, (unsigned long) elementSize,
                      ((double)elementSize)*((double)nRows)*((double)nColumns));
        goto ERROR_EXIT;
    }

    /* Allocate memory for the circular buffer for the imaginary part */
    if (complex) {
        if ( (var->data.im = malloc(nRows*nColumns*elementSize)) == NULL ) {
            (void)fprintf(stderr,
                          "*** Error allocating memory for the circular buffer "
                          "for logging the imaginary part of %s\n", varName);
            (void)fprintf(stderr, "*** Details:\n"
                          "       varName         = %s\n"
                          "       nRows           = %d\n"
                          "       nCols           = %d\n"
                          "       elementSize     = %lu\n"
                          "       Bytes Requested = %.16g\n\n",
                          varName, nRows, nColumns, (unsigned long) elementSize,
                          ((double)elementSize)*((double)nRows)*
                                                 ((double)nColumns));
            goto ERROR_EXIT;
        }
    }
    /*
     * Initialize the fields in LogVar structure.
     */
    if (appanedToLogVarsList) {
        rt_LoadModifiedLogVarName(li,varName,var->data.name);
    } else {
        var->data.name[mxMAXNAM-1] = '\0';
        (void)strncpy(var->data.name,varName,mxMAXNAM-1);
    }
    var->data.nCols           = nColumns;
    var->data.nRows           = nRows;

    var->data.nDims           = frameData ? 1 : nDims;
    if (var->data.nDims > 2) {
        var->data.dims = (int_T*)malloc(sizeof(int_T)*var->data.nDims);
    } else {
        var->data.dims = var->data._dims;
    }
    if (frameData) {
        var->data.dims[0] = nColumns;
    } else {
        /*LINTED E_CAST_INT_TO_SMALL_INT*/
        (void)memcpy(var->data.dims, dims, (size_t)(nDims*sizeof(int_T)));
    }

    var->data.dTypeID         = dTypeID;
    var->data.elSize          = elementSize;

    var->data.dataTypeConvertInfo = rt_GetDataTypeConvertInfo(
        pDataTypeConvertInfo, dTypeID);

    var->data.mxID            = rt_GetMxIdFromDTypeId(dTypeID);
    /* over-ride logical bit if data type is boolean */
    logical = dTypeID == SS_BOOLEAN ? 1 : 0;
    var->data.logical         = (logical)   ? matLOGICAL_BIT : 0x0;
    var->data.complex         = (complex)   ? matCOMPLEX_BIT : 0x0;
    var->data.frameData       = frameData;
    var->data.frameSize       = (frameData) ? frameSize : 1;

    /* fill up valDims field */
    if(logValDimsStat == NO_LOGVALDIMS){
        /* All signals are fixed-size, no need to log valueDimensions field */
        var->valDims     = NULL;
        /* Set these pointers to NULLs in this case */
        var->coords      = NULL;
        var->strides     = NULL;
        var->currStrides = NULL;
    }
    else{
        if ( (var->valDims = calloc(1, sizeof(ValDimsData))) == NULL ) 
            goto ERROR_EXIT;
        (void)memcpy(var->valDims->name, &VALUEDIMENSIONS_FIELD_NAME, mxMAXNAM);

        if(logValDimsStat == LOGVALDIMS_EMPTYMX){
        /* At least one signal is variable-size, 
           but the current signal is fixed-size. 
           Therefore, create a dummy MatrixData to write out valueDimensions 
           as an empty matrix. 
        */
            var->valDims->nRows = 0;
            var->valDims->nCols = 0;
            var->valDims->currSigDims = NULL;
            var->valDims->currSigDimsSize = NULL;
            var->valDims->dimsData = NULL;
            /* Set these pointers to NULLs in this case */
            var->coords      = NULL;
            var->strides     = NULL;
            var->currStrides = NULL;
        }
        else{ /* The current signal is a variable-size signal. */
            /* The "valueDimensions" must be double, so re-assign element size */
            elementSize = sizeof(real_T);
            /* When signals are frame-based, 'valueDimensions' has 1 column */
            if(frameData){
               /* When signal is frame-based, the first dimension is always fixed, 
                  so we only need to record the second dimension.
                  e.g. Two frame-based signals [10x4] and [10x3], 
                  'valueDimensions' and 'currSigDims'
                  only record 4 or 3.
               */
                nColumns = 1;
                var->valDims->currSigDims = (void**) (currSigDims + 1);
                var->valDims->currSigDimsSize = (int_T*) (currSigDimsSize + 1);
            }
            else{ /* non-frame based */
                nColumns = nDims;
                var->valDims->currSigDims = (void**) currSigDims;
                var->valDims->currSigDimsSize = (int_T*) currSigDimsSize;
            }

            /* Allocate memory for the circular buffer */
            if ( (var->valDims->dimsData = malloc(nRows*nColumns*elementSize)) == NULL ) {
                (void)fprintf(stderr,
                              "*** Error allocating memory for the circular buffer\n");
                (void)fprintf(stderr, "*** Details:\n"
                              "       varName         = %s\n"
                              "       nRows           = %d\n"
                              "       nCols           = %d\n"
                              "       elementSize     = %lu\n"
                              "       Bytes Requested = %.16g\n\n",
                              var->valDims->name, nRows, nColumns, (unsigned long) elementSize,
                              ((double)elementSize)*((double)nRows)*((double)nColumns));
                goto ERROR_EXIT;
            }
            var->valDims->nRows = nRows;
            var->valDims->nCols = nColumns;

            /* Allocate memory for these dynamic arrays */
            {
                size_t nbytes = var->data.nDims*sizeof(int_T);
                if( ((var->coords = calloc(nbytes, 1)) == NULL)
                    ||((var->strides = calloc(nbytes, 1)) == NULL)
                    ||((var->currStrides = calloc(nbytes, 1)) == NULL) )
                    goto ERROR_EXIT;
            }

        }
    }


    var->rowIdx               = 0;
    var->wrapped              = 0;
    var->nDataPoints          = 0;
    var->usingDefaultBufSize  = usingDefaultBufSize;
    var->okayToRealloc        = okayToRealloc;
    var->decimation           = decimation;
    var->numHits              = -1;  /* so first point gets logged */

    /* Add this log var to list in log info, if necessary */
    if (appanedToLogVarsList) {
        LogInfo *logInfo = (LogInfo*) rtliGetLogInfo(li);
        LogVar  *varList = logInfo->logVarsList;

        if (varList != NULL) {
            while (varList->next != NULL) {
                varList = varList->next;
            }
            varList->next = var;
        } else {
            logInfo->logVarsList = var;
        }
    }

    return(var); /* NORMAL_EXIT */

 ERROR_EXIT:
    
    *errStatus = rtMemAllocError;
    rt_DestroyLogVar(var);
    return(NULL);

} /* end rt_CreateLogVarWithConvert */

BEGIN_PUBLIC
#ifdef __cplusplus
}
#endif
END_PUBLIC

/* Function: rt_CreateLogVar ===================================================
 * Abstract:
 *	Create a logging variable.
 *
 * Returns:
 *	~= NULL  => success, returns the log variable created.
 *	== NULL  => failure, error message set in the simstruct.
 */

BEGIN_PUBLIC
#ifdef __cplusplus
extern "C" {
#endif
END_PUBLIC

PUBLIC LogVar *rt_CreateLogVar(RTWLogInfo        *li,
                               const real_T      startTime,
                               const real_T      finalTime,
                               const real_T      inStepSize,
                               const char_T      **errStatus,
                               const char_T      *varName,
                               BuiltInDTypeId    inpDataTypeID,
                               int_T             logical,
                               int_T             complex,
                               int_T             frameData,
                               int_T             nCols,
                               int_T             nDims,
                               const int_T       *dims,
                               LogValDimsStat    logValDimsStat,
                               void              **currSigDims,
                               int_T             *currSigDimsSize,
                               int_T             maxRows,
                               int_T             decimation,
                               real_T            sampleTime,
                               int_T             appanedToLogVarsList)
{
    const RTWLogDataTypeConvert *pDataTypeConvertInfo = NULL;

    return rt_CreateLogVarWithConvert(li,
                                      startTime,
                                      finalTime,
                                      inStepSize,
                                      errStatus,
                                      varName,
                                      inpDataTypeID,
                                      pDataTypeConvertInfo,
                                      logical,
                                      complex,
                                      frameData,
                                      nCols,
                                      nDims,
                                      dims,
                                      logValDimsStat,
                                      currSigDims,
                                      currSigDimsSize,
                                      maxRows,
                                      decimation,
                                      sampleTime,
                                      appanedToLogVarsList);

} /* end rt_CreateLogVar */

BEGIN_PUBLIC
#ifdef __cplusplus
}
#endif
END_PUBLIC

/* Function: rt_CreateStructLogVar =============================================
 * Abstract:
 *	Create a logging variable in the structure format.
 *
 * Returns:
 *	~= NULL  => success, returns the log variable created.
 *	== NULL  => failure, error message set in the simstruct.
 */

BEGIN_PUBLIC
#ifdef __cplusplus
extern "C" {
#endif
END_PUBLIC

PUBLIC StructLogVar *rt_CreateStructLogVar(RTWLogInfo              *li,
                                           const real_T            startTime,
                                           const real_T            finalTime,
                                           const real_T            inStepSize,
                                           const char_T            **errStatus,
                                           const char_T            *varName,
                                           boolean_T               logTime,
                                           int_T                   maxRows,
                                           int_T                   decimation,
                                           real_T                  sampleTime,
                                           const RTWLogSignalInfo  *sigInfo,
                                           const char_T            *blockName)
{

    return( local_CreateStructLogVar(li,
                                     startTime,
                                     finalTime,
                                     inStepSize,
                                     errStatus,
                                     varName,
                                     logTime,
                                     maxRows,
                                     decimation,
                                     sampleTime,
                                     sigInfo,
                                     blockName));

} /* end rt_CreateStructLogVar */

BEGIN_PUBLIC
#ifdef __cplusplus
}
#endif
END_PUBLIC

const char_T *rt_StartDataLoggingForOutput(RTWLogInfo   *li,
                                           const real_T startTime,
                                           const real_T finalTime,
                                           const real_T stepSize,
                                           const char_T **errStatus)
{
    const char_T   *varName;
    real_T         sampleTime = stepSize;
    int_T          maxRows    = rtliGetLogMaxRows(li);
    int_T          decimation = rtliGetLogDecimation(li);
    int_T          logFormat  = rtliGetLogFormat(li);
    boolean_T      logTime    = (logFormat==2) ? 1 : 0;

    LogInfo *       logInfo;
    logInfo = rtliGetLogInfo(li);

    /* reset error status */
    *errStatus = NULL;


    /* outputs */
    varName = rtliGetLogY(li);
    if (varName[0] != '\0') {
        int_T                  i;
        int_T                  ny;
        int_T                  yIdx;
        char_T                 name[mxMAXNAM];
        const char_T           *cp        = strchr(varName,',');
        LogSignalPtrsType      ySigPtrs   = rtliGetLogYSignalPtrs(li);
        const RTWLogSignalInfo *yInfo     = rtliGetLogYSignalInfo(li);

        /* count the number of variables (matrices or structures) to create */
        for (ny=1; cp != NULL; ny++) {
            cp = strchr(cp+1,',');
        }
        logInfo->ny = ny;

        if (logFormat==0) {
            if ( (logInfo->y = calloc(ny,sizeof(LogVar*))) == NULL ) {
                *errStatus = rtMemAllocError;
                goto ERROR_EXIT;
            }
        } else {
            if ( (logInfo->y = calloc(ny,sizeof(StructLogVar*))) == NULL ) {
                *errStatus = rtMemAllocError;
                goto ERROR_EXIT;
            }
        }

        for (i = yIdx = 0, cp = varName; i < ny; i++) {
            int_T        len;
            const char_T *cp1 = strchr(cp+1,',');

            if (cp1 != NULL) {
                /*LINTED E_ASSIGN_INT_TO_SMALL_INT*/
                len = (int_T)(cp1 - cp);
                if (len >= mxMAXNAM) len = mxMAXNAM - 1;
            } else {
                len = mxMAXNAM - 1;
            }
            (void)strncpy(name, cp, len);
            name[len] = '\0';

            if (ny > 1 && ySigPtrs[i] == NULL) {
                goto NEXT_NAME;
            }

            if (logFormat == 0) {
                int            numCols;
		int            nDims;
		const int      *dims;
                BuiltInDTypeId dataType;
                int            isComplex;

                if (ny == 1) {
                    int_T op;

                    numCols = yInfo[0].numCols[0];
                    for (op = 1; op < yInfo[0].numSignals; op++) {
                        numCols += yInfo[0].numCols[op];
                    }
                    /*
                     * If we have only one "matrix" outport,
                     * we can still log it as a matrix
                     */
		    if(yInfo[0].numSignals == 1){
			nDims     = yInfo[0].numDims[0];
			dims      = yInfo[0].dims;
		    }else{
			nDims     = 1;
			dims      = &numCols;
		    }
                    dataType  = yInfo[0].dataTypes[0];
                    isComplex = yInfo[0].complexSignals[0];

                } else {
                    numCols   = yInfo[yIdx].numCols[0];
		    nDims     = yInfo[yIdx].numDims[0];
		    dims      = yInfo[yIdx].dims;
                    dataType  = yInfo[yIdx].dataTypes[0];
                    isComplex = yInfo[yIdx].complexSignals[0];
                }

                logInfo->y[yIdx] = rt_CreateLogVarWithConvert(
                    li, startTime, finalTime,
                    stepSize, errStatus,
                    name,
                    dataType,
                    yInfo[yIdx].dataTypeConvert,
                    0,isComplex,
                    0,numCols,nDims,dims,
                    NO_LOGVALDIMS, NULL, NULL,
                    maxRows,decimation,
                    sampleTime,1);
                if (logInfo->y[yIdx] == NULL)  goto ERROR_EXIT;
            } else {
                logInfo->y[yIdx] = local_CreateStructLogVar(li, startTime,
                                                            finalTime, stepSize,
                                                            errStatus, name,
                                                            logTime, maxRows,
                                                            decimation, sampleTime,
                                                            &yInfo[yIdx], NULL);
                if (logInfo->y[yIdx] == NULL) goto ERROR_EXIT;
            }
            ++yIdx;
        NEXT_NAME:
            cp = cp1;
            if (cp != NULL && *cp == ',') cp++;
        }
    }

    return(NULL); /* NORMAL_EXIT */

 ERROR_EXIT:
    (void)fprintf(stderr, "*** Errors occurred when starting data logging.\n");
    if (*errStatus == NULL) {
        *errStatus = rtMemAllocError;
    }
    if (logInfo) {
        rt_DestroyLogVar(logInfo->logVarsList);
        logInfo->logVarsList = NULL;
        rt_DestroyStructLogVar(logInfo->structLogVarsList);
        logInfo->structLogVarsList = NULL;
        FREE(logInfo->y);
        logInfo->y = NULL;
    }
    return(*errStatus);

}



/* Function: rt_StartDataLogging ===============================================
 * Abstract:
 *      Initialize data logging info based upon the following settings cached
 *      in the RTWLogging data structure of the SimStruct.
 *
 * Return value is:
 *	== NULL  => success
 *	!= NULL  => failure (the return value is a pointer that points to the
 *                           error message, which is also set in the simstruct)
 */

BEGIN_PUBLIC
#ifdef __cplusplus
extern "C" {
#endif
END_PUBLIC
 
PUBLIC const char_T *rt_StartDataLoggingWithStartTime(RTWLogInfo   *li,
                                               const real_T startTime,
                                               const real_T finalTime,
                                               const real_T stepSize,
                                               const char_T **errStatus)
{
    const char_T   *varName;
    LogInfo        *logInfo;
    real_T         sampleTime = stepSize;
    int_T          maxRows    = rtliGetLogMaxRows(li);
    int_T          decimation = rtliGetLogDecimation(li);
    int_T          logFormat  = rtliGetLogFormat(li);
    boolean_T      logTime    = (logFormat==2) ? 1 : 0;

    /* reset error status */
    *errStatus = NULL;

    if ((logInfo=calloc(1,sizeof(LogInfo))) == NULL) {
        *errStatus = rtMemAllocError;
        goto ERROR_EXIT;
    }
    rtliSetLogInfo(li, (void*)logInfo);

    /* time */
    varName = rtliGetLogT(li);
    if (varName[0] != '\0') {
        int_T dims = 1;
        logInfo->t = rt_CreateLogVarWithConvert(li, startTime, finalTime,
                                                stepSize, errStatus,
                                                varName,SS_DOUBLE,
                                                NULL,
                                                0,0,0,1,1,
                                                &dims, NO_LOGVALDIMS, NULL, NULL,
                                                maxRows,decimation,
                                                sampleTime,1);
        if (logInfo->t == NULL) goto ERROR_EXIT;
    }

    /* states */
    if ( rtliGetLogX(li)[0] != '\0' ||  rtliGetLogXFinal(li)[0] != '\0' ) {
        const RTWLogSignalInfo  *xInfo = rtliGetLogXSignalInfo(li);

        if (logFormat == 0) {                                /* Matrix Format */
            int            numCols;
            int            nDims;
            const int      *dims;
            BuiltInDTypeId dataType;
            int            isComplex;
            int_T          sIdx;

            const RTWLogDataTypeConvert *pDTConvInfo;

            numCols = xInfo[0].numCols[0];
            for (sIdx = 1; sIdx < xInfo[0].numSignals; sIdx++) {
                numCols += xInfo[0].numCols[sIdx];
            }
            /* If we have only one "matrix" state, we can log as a matrix */
            if (xInfo[0].numSignals == 1) {
                nDims     = xInfo[0].numDims[0];
                dims      = xInfo[0].dims;
            } else {
                nDims     = 1;
                dims      = &numCols;
            }
            dataType  = xInfo[0].dataTypes[0];
            isComplex = xInfo[0].complexSignals[0];

            pDTConvInfo = xInfo[0].dataTypeConvert;

            if (rtliGetLogX(li)[0] != '\0') {
                logInfo->x = rt_CreateLogVarWithConvert(li, startTime, finalTime,
                                                        stepSize, errStatus,
                                                        rtliGetLogX(li),dataType,
                                                        pDTConvInfo,
                                                        0,
                                                        isComplex,0,numCols,nDims,dims,
                                                        NO_LOGVALDIMS, NULL, NULL,
                                                        maxRows,decimation,sampleTime,1);
                if (logInfo->x == NULL)  goto ERROR_EXIT;
            }
            if (rtliGetLogXFinal(li)[0] != '\0') {
                logInfo->xFinal = rt_CreateLogVarWithConvert(li, startTime, finalTime,
                                                             stepSize, errStatus,
                                                             rtliGetLogXFinal(li),dataType,
                                                             pDTConvInfo,
                                                             0,isComplex,0,numCols,nDims,
                                                             dims, NO_LOGVALDIMS, NULL, 
                                                             NULL, 1,decimation,
                                                             sampleTime,1);
                if (logInfo->xFinal == NULL)  goto ERROR_EXIT;
            }
        } else {                                          /* Structure Format */
            if (rtliGetLogX(li)[0] != '\0') {
                logInfo->x = local_CreateStructLogVar(li, startTime, finalTime,
                                                      stepSize, errStatus,
                                                      rtliGetLogX(li), logTime,
                                                      maxRows, decimation,
                                                      sampleTime, xInfo, NULL);
                if (logInfo->x == NULL) goto ERROR_EXIT;
            }
            if (rtliGetLogXFinal(li)[0] != '\0') {
                logInfo->xFinal = local_CreateStructLogVar(li, startTime, finalTime,
                                                           stepSize, errStatus,
                                                           rtliGetLogXFinal(li),
                                                           logTime,1,decimation,
                                                           sampleTime,xInfo,NULL);
                if (logInfo->xFinal == NULL) goto ERROR_EXIT;
            }
        }
    }

    /* signal logging */
    if ( rtliGetSigLog(li)[0] != '\0' ) {
        const RTWLogSignalInfo  *slInfo = rtliGetSigLogSignalInfo(li);

        /* log using structure format */
        logInfo->sl = local_CreateStructLogVar(li, startTime, finalTime, 
                                               stepSize, errStatus,
                                               rtliGetSigLog(li), logTime,
                                               maxRows, decimation,
                                               sampleTime, slInfo, NULL);
        if (logInfo->sl == NULL) goto ERROR_EXIT;
    }

    /* outputs */
    *errStatus = rt_StartDataLoggingForOutput(li,startTime,finalTime,
                                              stepSize,errStatus);
    if (*errStatus != NULL)  goto ERROR_EXIT;

    return(NULL); /* NORMAL_EXIT */

 ERROR_EXIT:
    (void)fprintf(stderr, "*** Errors occurred when starting data logging.\n");
    if (*errStatus == NULL) {
        *errStatus = rtMemAllocError;
    }
    if (logInfo) {
        rt_DestroyLogVar(logInfo->logVarsList);
        logInfo->logVarsList = NULL;
        rt_DestroyStructLogVar(logInfo->structLogVarsList);
        logInfo->structLogVarsList = NULL;
    }
    return(*errStatus);

} /* end rt_StartDataLogging */

BEGIN_PUBLIC
#ifdef __cplusplus
}
#endif
END_PUBLIC



BEGIN_PUBLIC
#ifdef __cplusplus
extern "C" {
#endif
END_PUBLIC

BEGIN_PUBLIC
#if MAT_FILE == 1
END_PUBLIC
PUBLIC const char_T *rt_StartDataLogging(RTWLogInfo   *li,
                                         const real_T finalTime,
                                         const real_T stepSize,
                                         const char_T **errStatus)
{
    return rt_StartDataLoggingWithStartTime(li,
                                            0.0,
                                            finalTime,
                                            stepSize,
                                            errStatus);
}
BEGIN_PUBLIC
#else
#define rt_StartDataLogging(li, finalTime, stepSize, errStatus) NULL /* do nothing */
#endif
END_PUBLIC

BEGIN_PUBLIC
#ifdef __cplusplus
}
#endif
END_PUBLIC

/* Function: rt_ReallocLogVar ==================================================
 * Abstract:
 *   Allocate more memory for the data buffers in the log variable.
 *   Exit if unable to allocate more memory.
 */
void rt_ReallocLogVar(LogVar *var, boolean_T isVarDims)
{
    void *tmp;
    int_T nCols = var->data.nCols;
    int_T nRows = var->data.nRows + DEFAULT_BUFFER_SIZE;
    size_t elSize = var->data.elSize;
    
    tmp = realloc(var->data.re, nRows*nCols*elSize);
    if (tmp == NULL) {
        (void)fprintf(stderr,
                      "*** Memory allocation error.\n");
        (void)fprintf(stderr, ""
                      "    varName          = %s%s\n"
                      "    nRows            = %d\n"
                      "    nCols            = %d\n"
                      "    elementSize      = %lu\n"
                      "    Current Size     = %.16g\n"
                      "    Failed resize    = %.16g\n\n",
                      var->data.name,
                      var->data.complex ? " (real part)" : "",
                      var->data.nRows,
                      var->data.nCols,
                      (long)  var->data.elSize,
                      (double)nRows*nCols*elSize,
                      (double)(nRows+DEFAULT_BUFFER_SIZE)*nCols*elSize);
        exit(1);
    }
    var->data.re = tmp;

    if (var->data.complex) {
        tmp = realloc(var->data.im, nRows*nCols*elSize);
        if (tmp == NULL) {
            (void)fprintf(stderr,
                          "*** Memory allocation error.\n");
            (void)fprintf(stderr, ""
                          "    varName          = %s (complex part)\n"
                          "    nRows            = %d\n"
                          "    nCols            = %d\n"
                          "    elementSize      = %lu\n"
                          "    Current Size     = %.16g\n"
                          "    Failed resize    = %.16g\n\n",
                          var->data.name,
                          var->data.nRows,
                          var->data.nCols,
                          (long)  var->data.elSize,
                          (double)nRows*nCols*elSize,
                          (double)(nRows+DEFAULT_BUFFER_SIZE)*nCols*elSize);
            exit(1);
        }
        var->data.im = tmp;
    }
    var->data.nRows = nRows;

    /* Also reallocate memory for "valueDimensions" 
       when logging the variable-size signal
    */
    if(isVarDims){
        nCols = var->valDims->nCols;
        nRows = var->valDims->nRows + DEFAULT_BUFFER_SIZE;
        elSize = sizeof(real_T);
        tmp = realloc(var->valDims->dimsData, nRows*nCols*elSize);
        if (tmp == NULL) {
            (void)fprintf(stderr,
                          "*** Memory allocation error.\n");
            (void)fprintf(stderr, ""
                          "    varName          = %s\n"
                          "    nRows            = %d\n"
                          "    nCols            = %d\n"
                          "    elementSize      = %lu\n"
                          "    Current Size     = %.16g\n"
                          "    Failed resize    = %.16g\n\n",
                          var->valDims->name,
                          var->valDims->nRows,
                          var->valDims->nCols,
                          (long)  elSize,
                          (double)nRows*nCols*elSize,
                          (double)(nRows+DEFAULT_BUFFER_SIZE)*nCols*elSize);
            exit(1);
        }
        var->valDims->dimsData = tmp;
        var->valDims->nRows = nRows;
    }

} /* end rt_ReallocLogVar */


/* Function: rt_UpdateLogVar ===================================================
 * Abstract:
 *	Called to log data for a log variable.
 */
 
BEGIN_PUBLIC
#ifdef __cplusplus
extern "C" {
#endif
END_PUBLIC
 
PUBLIC void rt_UpdateLogVar(LogVar *var, const void *data, boolean_T isVarDims)
{
    size_t        elSize    = var->data.elSize;
    const  char_T *cData    = data;
    const  int_T  frameData = var->data.frameData;
    const  int_T  frameSize = frameData ? (var->data.frameSize) : 1;
    const  int_T  logWidth  = var->data.nCols;
    BuiltInDTypeId dTypeID  = var->data.dTypeID;

    int_T  offset        = 0;
    char_T *currRealRow  = NULL;
    char_T *currImagRow  = NULL;
    int_T  pointSize     = (int_T)((var->data.complex) ? 2*elSize : elSize);

    int    i, j, k;

    /* The following variables will be used for 
       logging variable-size signals */
    const  int_T  nDims = var->data.nDims;
    const  int_T  *dims = var->data.dims;
    const  void   **currDimsPtr = NULL;
    const  int_T  *currDimsSizePtr = NULL;

    /* The following variables will be used for 
       logging "valueDimensions" field */
    int_T  offset_valDims   = 0;
    char_T *currValDimsRow  = NULL;
    size_t elSize_valDims   = sizeof(real_T);
    real_T currentSigDims   = 0;
    int_T  nRows_valDims    = 0;
    int_T  logWidth_valDims = 0;

    if(isVarDims){
        currDimsPtr = (const void**) var->valDims->currSigDims;
        currDimsSizePtr = (const int_T*) var->valDims->currSigDimsSize;
        logWidth_valDims = frameData ? 1 : var->valDims->nCols;
        nRows_valDims = var->valDims->nRows;

        var->strides[0] = 1;
        var->currStrides[0] = 1;

        for (k = 1; k < nDims; k++){
            int32_T currDimsVal=0;
            switch (currDimsSizePtr[k-1]) {
              case 1:
                currDimsVal = (**(((const uint8_T**) currDimsPtr)+(k-1)));
                break;
              case 2:
                currDimsVal = (**(((const uint16_T**) currDimsPtr)+(k-1)));
                break;
              case 4:
                currDimsVal = (**(((const uint32_T**) currDimsPtr)+(k-1)));
                break;
            }
            var->strides[k] = var->strides[k-1] * dims[k-1];
            var->currStrides[k] = var->currStrides[k-1] * currDimsVal;
        }
    }


    for (i = 0; i < frameSize; i++) {
        if (++var->numHits % var->decimation) continue;
        var->numHits = 0;

        offset       = (int_T)(elSize * var->rowIdx * logWidth);
        currRealRow  = ((char_T*) (var->data.re)) + offset;
        currImagRow  = (var->data.complex) ?
                       ((char_T*) (var->data.im)) + offset :  NULL;

        /* update logging data */
        for (j = 0; j < logWidth; j++) {

            boolean_T inRange = true;
            int idx = j;

            /* Check whether the currently logged value is in range or not.
               For fixed-size signal logging, always inRange = true; idx = j;
               For variable-size signal logging, use strides, coordinates
               and current strides to decide whether the currently logged
               data is in range or not and its location in the logging 
               matrix.
             */
            if(isVarDims){
                int rem = j;
                idx = 0;
                for(k = nDims-1; k>=0; k--){
                    int32_T currDimsVal=0;
                    switch (currDimsSizePtr[k]) {
                      case 1:
                        currDimsVal = (**(((const uint8_T**) currDimsPtr)+k));
                        break;
                      case 2:
                        currDimsVal = (**(((const uint16_T**) currDimsPtr)+k));
                        break;
                      case 4:
                        currDimsVal = (**(((const uint32_T**) currDimsPtr)+k));
                        break;
                    }
                    var->coords[k] = rem / var->strides[k];
                    if( var->coords[k] >= currDimsVal ){
                        inRange = false;
                        break;
                    }
                    rem = rem - var->coords[k] * var->strides[k];
                }
                if(inRange){
                    idx = var->coords[0];
                    for (k = 1; k < nDims; k++){
                        idx += var->coords[k] * var->currStrides[k];
                    }
                }
            }
            
            if ( ! var->data.dataTypeConvertInfo.conversionNeeded )
            {
                /* NO  conversion needed
                 */ 
                if(inRange){
                    /* If in range, fill in data */
                    const char *cDataPoint = cData + (i+frameSize*idx) * pointSize;

                    (void) memcpy(currRealRow, cDataPoint, elSize);
                    currRealRow += elSize;
                    if (var->data.complex) {
                        (void) memcpy(currImagRow, cDataPoint + elSize, elSize);
                        currImagRow += elSize;
                    }
                }
                else{
                    /* If out of range, fill in NaN or 0:
                      1) For bool, int32, uint32, int16, uint16, etc,
                         memset to zeros;
                      2) For fixed-point data type, NaN conversion is not
                         allowed, memset to zeros.
                    */
                    if(dTypeID == SS_DOUBLE){
                        (void) memcpy(currRealRow, &rtNaN, elSize);
                    }
                    else if(dTypeID == SS_SINGLE){
                        (void) memcpy(currRealRow, &rtNaNF, elSize);
                    }
                    else{
                        (void) memset(currRealRow, 0, elSize);
                    }
                    
                    currRealRow += elSize;
                    if (var->data.complex) {
                        /* For imaginary part, fill in 0 */
                        (void) memset(currImagRow, 0, elSize);
                        currImagRow += elSize;
                    }
                }
            }
            else
            {
                /* YES conversion needed
                 */ 
                DTypeId dataTypeIdOriginal = 
                    var->data.dataTypeConvertInfo.dataTypeIdOriginal;

                DTypeId dataTypeIdLoggingTo = 
                    var->data.dataTypeConvertInfo.dataTypeIdLoggingTo;

                int bitsPerChunk = var->data.dataTypeConvertInfo.bitsPerChunk;
                int numOfChunk =  var->data.dataTypeConvertInfo.numOfChunk;
                unsigned int isSigned = var->data.dataTypeConvertInfo.isSigned;

                double fracSlope = var->data.dataTypeConvertInfo.fracSlope;
                int    fixedExp  = var->data.dataTypeConvertInfo.fixedExp;
                double bias      = var->data.dataTypeConvertInfo.bias;

                double curRealValue = -0.12345678987654;
                double curImagValue = -0.12345678987654;

                int_T adjIndexIfComplex = (var->data.complex) ? 2 : 1;

                if(inRange){
                    if(numOfChunk > 1)
                    {
                        /* For multiword */
                        const char *pInData = (const char *)(cData);
                        int dtSize = bitsPerChunk*numOfChunk/8;
                        pInData += ((i+frameSize*idx) * adjIndexIfComplex) * dtSize;
                        
                        curRealValue = rt_GetDblValueFromOverSizedData(pInData, bitsPerChunk, numOfChunk, 
                                                                       isSigned, fracSlope, fixedExp, bias);
                        if (var->data.complex) {
                            curImagValue = rt_GetDblValueFromOverSizedData((pInData+dtSize), bitsPerChunk, numOfChunk, 
                                                                           isSigned, fracSlope, fixedExp, bias);
                        }
                    }
                    else
                    {
                        /* if in range, fill in data that is converted first */
                        switch ( dataTypeIdOriginal )
                        {
                          case SS_DOUBLE:
                            {
                                const real_T *pInData = ((const real_T *)(cData));
                                
                                pInData += (i+frameSize*idx) * adjIndexIfComplex;
                                
                                curRealValue = ldexp( fracSlope * (double)pInData[0], fixedExp ) + bias;
                                if (var->data.complex) {
                                    curImagValue = ldexp( fracSlope * (double)pInData[1], fixedExp ) + bias;
                                }
                            }
                            break;
                          case SS_SINGLE:
                            {
                                const real32_T *pInData = ((const real32_T *)(cData));

                                pInData += (i+frameSize*idx) * adjIndexIfComplex;
                                
                                curRealValue = ldexp( fracSlope * (double)pInData[0], fixedExp ) + bias;
                                if (var->data.complex) {
                                    curImagValue = ldexp( fracSlope * (double)pInData[1], fixedExp ) + bias;
                                }
                            }
                            break;
                          case SS_INT8:
                            {
                                const int8_T *pInData = ((const int8_T *)(cData));
                                
                                pInData += (i+frameSize*idx) * adjIndexIfComplex;
                                
                                curRealValue = ldexp( fracSlope * (double)pInData[0], fixedExp ) + bias;
                                if (var->data.complex) {
                                    curImagValue = ldexp( fracSlope * (double)pInData[1], fixedExp ) + bias;
                                }
                            }
                            break;
                          case SS_UINT8:
                            {
                                const uint8_T *pInData = ((const uint8_T *)(cData));
                                
                                pInData += (i+frameSize*idx) * adjIndexIfComplex;
                                
                                curRealValue = ldexp( fracSlope * (double)pInData[0], fixedExp ) + bias;
                                if (var->data.complex) {
                                    curImagValue = ldexp( fracSlope * (double)pInData[1], fixedExp ) + bias;
                                }
                            }
                            break;
                          case SS_INT16:
                            {
                                const int16_T *pInData = ((const int16_T *)(cData));
                                
                                pInData += (i+frameSize*idx) * adjIndexIfComplex;
                                
                                curRealValue = ldexp( fracSlope * (double)pInData[0], fixedExp ) + bias;
                                if (var->data.complex) {
                                    curImagValue = ldexp( fracSlope * (double)pInData[1], fixedExp ) + bias;
                                }
                            }
                            break;
                          case SS_UINT16:
                            {
                                const uint16_T *pInData = ((const uint16_T *)(cData));
                                
                                pInData += (i+frameSize*idx) * adjIndexIfComplex;
                                
                                curRealValue = ldexp( fracSlope * (double)pInData[0], fixedExp ) + bias;
                                if (var->data.complex) {
                                    curImagValue = ldexp( fracSlope * (double)pInData[1], fixedExp ) + bias;
                                }
                            }
                            break;
                          case SS_INT32:
                            {
                                const int32_T *pInData = ((const int32_T *)(cData));
                                
                                pInData += (i+frameSize*idx) * adjIndexIfComplex;

                                curRealValue = ldexp( fracSlope * (double)pInData[0], fixedExp ) + bias;
                                if (var->data.complex) {
                                    curImagValue = ldexp( fracSlope * (double)pInData[1], fixedExp ) + bias;
                                }
                            }
                            break;
                          case SS_UINT32:
                            {
                                const uint32_T *pInData = ((const uint32_T *)(cData));
                                
                                pInData += (i+frameSize*idx) * adjIndexIfComplex;
                                
                                curRealValue = ldexp( fracSlope * (double)pInData[0], fixedExp ) + bias;
                                if (var->data.complex) {
                                    curImagValue = ldexp( fracSlope * (double)pInData[1], fixedExp ) + bias;
                                }
                            }
                            break;
                          case SS_BOOLEAN:
                            {
                                const boolean_T *pInData = ((const boolean_T *)(cData));
                                
                                pInData += (i+frameSize*idx) * adjIndexIfComplex;
                                
                                curRealValue = ldexp( fracSlope * (double)pInData[0], fixedExp ) + bias;
                                if (var->data.complex) {
                                    curImagValue = ldexp( fracSlope * (double)pInData[1], fixedExp ) + bias;
                                }
                            }
                            break;
                          default: 
                            {
                                /* For biglong */
                                const char *pInData = (const char *)(cData);
                                int dtSize = bitsPerChunk*numOfChunk/8;
                                pInData += ((i+frameSize*idx) * adjIndexIfComplex) * dtSize;
                                
                                curRealValue = rt_GetDblValueFromOverSizedData(pInData, bitsPerChunk, numOfChunk, 
                                                                               isSigned, fracSlope, fixedExp, bias);
                                if (var->data.complex) {
                                    curImagValue = rt_GetDblValueFromOverSizedData((pInData+dtSize), bitsPerChunk, numOfChunk, 
                                                                                   isSigned, fracSlope, fixedExp, bias);
                                }
                            }
                            break;
                        } /* -- end of swith -- */
                    }
                }
                else{
                    /* if out of range, just fill NaN or 0 */
                    if(dTypeID == SS_DOUBLE || dTypeID == SS_SINGLE){
                        curRealValue = ldexp( rtNaN, fixedExp ) + bias;
                    }
                    else{
                        curRealValue = ldexp( 0, fixedExp ) + bias;
                    }
                    if (var->data.complex) {
                        /* fill 0 in imaginary part*/
                        curImagValue = ldexp( 0, fixedExp ) + bias;
                    }
                }

                switch ( dataTypeIdLoggingTo )
                {
                  case SS_DOUBLE:
                    {
                        *((real_T *)currRealRow) = (real_T)curRealValue;

                        if (var->data.complex) {

                            *((real_T *)currImagRow) = (real_T)curImagValue;
                        }
                    }
                    break;
                  case SS_SINGLE:
                    {
                        *((real32_T *)currRealRow) = (real32_T)curRealValue;

                        if (var->data.complex) {

                            *((real32_T *)currImagRow) = (real32_T)curImagValue;
                        }
                    }
                    break;
                  case SS_INT8:
                    {
                        *((int8_T *)currRealRow) = (int8_T)curRealValue;

                        if (var->data.complex) {

                            *((int8_T *)currImagRow) = (int8_T)curImagValue;
                        }
                    }
                    break;
                  case SS_UINT8:
                    {
                        *((uint8_T *)currRealRow) = (uint8_T)curRealValue;

                        if (var->data.complex) {

                            *((uint8_T *)currImagRow) = (uint8_T)curImagValue;
                        }
                    }
                    break;
                  case SS_INT16:
                    {
                        *((int16_T *)currRealRow) = (int16_T)curRealValue;

                        if (var->data.complex) {

                            *((int16_T *)currImagRow) = (int16_T)curImagValue;
                        }
                    }
                    break;
                  case SS_UINT16:
                    {
                        *((uint16_T *)currRealRow) = (uint16_T)curRealValue;

                        if (var->data.complex) {

                            *((uint16_T *)currImagRow) = (uint16_T)curImagValue;
                        }
                    }
                    break;
                  case SS_INT32:
                    {
                        *((int32_T *)currRealRow) = (int32_T)curRealValue;

                        if (var->data.complex) {

                            *((int32_T *)currImagRow) = (int32_T)curImagValue;
                        }
                    }
                    break;
                  case SS_UINT32:
                    {
                        *((uint32_T *)currRealRow) = (uint32_T)curRealValue;

                        if (var->data.complex) {

                            *((uint32_T *)currImagRow) = (uint32_T)curImagValue;
                        }
                    }
                    break;
                  case SS_BOOLEAN:
                    {
                        *((boolean_T *)currRealRow) = (boolean_T)(curRealValue != 0.0);

                        if (var->data.complex) {

                            *((boolean_T *)currImagRow) = (boolean_T)(curImagValue != 0.0);
                        }
                    }
                    break;
                } /* -- end of swith -- */

                currRealRow += elSize;
                if (var->data.complex) {
                    currImagRow += elSize;
                }
            }
        }

        if(isVarDims){ /* update "valueDimensions" field */
            for(j = 0; j < logWidth_valDims; j ++){
                int32_T currDimsVal=0;
                switch (currDimsSizePtr[j]) {
                  case 1:
                    currDimsVal = (**(((const uint8_T**) currDimsPtr)+j));
                    break;
                  case 2:
                    currDimsVal = (**(((const uint16_T**) currDimsPtr)+j));
                    break;
                  case 4:
                    currDimsVal = (**(((const uint32_T**) currDimsPtr)+j));
                    break;
                }
                offset_valDims  = (int_T)(elSize_valDims *( var->rowIdx + nRows_valDims * j));
                currValDimsRow  = ((char_T*) (var->valDims->dimsData)) + offset_valDims;

                /* convert int_T to real_T */
                currentSigDims = (real_T) currDimsVal;
                (void) memcpy(currValDimsRow, &currentSigDims, elSize_valDims);
                currValDimsRow += elSize_valDims;
           }
        }

        if (++var->rowIdx == var->data.nRows) {
            if (var->okayToRealloc == 1) {
                rt_ReallocLogVar(var, isVarDims);
            }  else {
                /* Circular buffer */
                var->rowIdx = 0;
                ++(var->wrapped); /* increment the wrap around counter */
            }
        }
    }

    return;

} /* end rt_UpdateLogVar */

BEGIN_PUBLIC
#ifdef __cplusplus
}
#endif
END_PUBLIC

/* Function: rt_UpdateLogVarWithDiscontiguousData ==============================
 * Abstract:
 *      Log one row of the LogVar with data that is not contiguous.
 */
void rt_UpdateLogVarWithDiscontiguousData(LogVar            *var,
                                          LogSignalPtrsType data,
                                          const int_T       *segmentLengths,
                                          int_T             nSegments)
{
    /* This function is only used to log states, there's no var-dims issue. */
    size_t elSize = var->data.elSize;
    int_T  offset = (int_T)(elSize * var->rowIdx * var->data.nCols);
    int    segIdx;

    if (++var->numHits % var->decimation) return;
    var->numHits = 0;

    if (var->data.complex) {
        int8_T *dstRe = (int8_T*)(var->data.re) + offset;
        int8_T *dstIm = (int8_T*)(var->data.im) + offset;

        for (segIdx = 0; segIdx < nSegments; segIdx++) {
            int_T         nEl  = segmentLengths[segIdx];
            const  int8_T *src = data[segIdx];
            int_T         el;

            for (el = 0; el < nEl; el++) {
                (void)memcpy(dstRe, src, elSize);
                dstRe += elSize;   src += elSize;
                (void)memcpy(dstIm, src, elSize);
                dstIm += elSize;   src += elSize;
            }
        }
    } else {
        int8_T *dst = (int8_T*)(var->data.re) + offset;

        for (segIdx = 0; segIdx < nSegments; segIdx++) {
            size_t         segSize = elSize*segmentLengths[segIdx];
            const  int8_T  *src    = data[segIdx];

            (void)memcpy(dst, src, segSize);
            dst += segSize;
        }
    }

    if (++var->rowIdx == var->data.nRows) {
        if (var->okayToRealloc == 1) {
            rt_ReallocLogVar(var, false);
        } else {
            /* Circular buffer */
            var->rowIdx = 0;
            ++(var->wrapped); /* increment the wrap around counter */
        }
    }
    return;

} /* end rt_UpdateLogVarWithDiscontiguousData */



/* Function: rt_UpdateStructLogVar =============================================
 * Abstract:
 *      Called to log data for a structure log variable.
 */

BEGIN_PUBLIC
#ifdef __cplusplus
extern "C" {
#endif
END_PUBLIC 
 
PUBLIC void rt_UpdateStructLogVar(StructLogVar *var, const real_T *t, const void *data)
{
    LogVar       *values = var->signals.values;
    const char_T *signal = data;
    boolean_T *isVarDims = var->signals.isVarDims;
    int i = 0;

    /* time */
    if (var->logTime) {
        rt_UpdateLogVar(var->time, t, false);
    }

    /* signals */
    while (values) {
        size_t elSz = values->data.elSize;

        rt_UpdateLogVar(values, signal, isVarDims[i]);

        if (values->data.complex) elSz *= 2;
        signal += elSz * values->data.nCols;

        values = values->next;
        i++;
    }
} /* end rt_UpdateStructLogVar */

BEGIN_PUBLIC
#ifdef __cplusplus
}
#endif
END_PUBLIC

/* Function: rt_UpdateTXYLogVars ===============================================
 * Abstract:
 *	Update the T,X,Y variables that are being logged.
 */

BEGIN_PUBLIC
#ifdef __cplusplus
extern "C" {
#endif
END_PUBLIC 
 
BEGIN_PUBLIC
#if MAT_FILE == 1
END_PUBLIC          
PUBLIC const char_T *rt_UpdateTXYLogVars(RTWLogInfo *li, time_T *tPtr)
{
    LogInfo *logInfo     = rtliGetLogInfo(li);
    int_T   matrixFormat = (rtliGetLogFormat(li) == 0);

    /* time */
    if (logInfo->t != NULL) {
        rt_UpdateLogVar(logInfo->t, tPtr, false);
    }

    if (matrixFormat) {                                      /* MATRIX_FORMAT */
        /* states */
        if (logInfo->x != NULL || logInfo->xFinal != NULL) {
            const RTWLogSignalInfo *xInfo      = rtliGetLogXSignalInfo(li);
            LogSignalPtrsType      segAddr     = rtliGetLogXSignalPtrs(li);
            const int_T            *segLengths = xInfo->numCols;
            int_T                  nSegments   = xInfo->numSignals;
            if (logInfo->x != NULL) {
                rt_UpdateLogVarWithDiscontiguousData(logInfo->x, segAddr,
                                                     segLengths, nSegments);
            }
            if (logInfo->xFinal != NULL) {
                rt_UpdateLogVarWithDiscontiguousData(logInfo->xFinal, segAddr,
                                                     segLengths, nSegments);
            }
        }
        /* outputs */
        if (logInfo->y != NULL) {
            LogVar **var = (LogVar**) (logInfo->y);
            int_T  ny    = logInfo->ny;
            int_T  i;
            int    yIdx;
            LogSignalPtrsType data = rtliGetLogYSignalPtrs(li);

            for (i = 0, yIdx = 0; i < ny; i++) {
                if (data[i] != NULL) {
                    /* 
                       When outputs are logged in Matrix format, 
                       no variable-size signal logging is allowed.
                    */
                    rt_UpdateLogVar(var[yIdx++], data[i], false);
                }
            }
        }
    } else {                                              /* STRUCTURE_FORMAT */
        /* states */
        if (logInfo->x != NULL) {
            int_T             i;
            StructLogVar      *var = logInfo->x;
            LogVar            *val = var->signals.values;
            int_T             nsig = var->signals.numSignals;
            LogSignalPtrsType data = rtliGetLogXSignalPtrs(li);

            /* time */
            if (var->logTime) {
                rt_UpdateLogVar(var->time, tPtr, false);
            }

            /* signals */
            for (i = 0; i < nsig; i++) {
                rt_UpdateLogVar(val, data[i], false);
                val = val->next;
            }
        }

        /* outputs */
        if (logInfo->y != NULL) {
            int_T             ny      = logInfo->ny;
            LogSignalPtrsType data    = rtliGetLogYSignalPtrs(li);
            StructLogVar      **var   = (StructLogVar**) (logInfo->y);

            if (ny == 1) {
                int_T  i;
                int_T  dataIdx;
                LogVar *val = var[0]->signals.values;
                int_T  nsig = var[0]->signals.numSignals;
                boolean_T   *isVarDims = var[0]->signals.isVarDims;

                /* time */
                if (var[0]->logTime) {
                    rt_UpdateLogVar(var[0]->time, tPtr, false);
                }

                /* signals */
                for (i = 0, dataIdx = 0; i < nsig; i++) {
                    while (data[dataIdx] == NULL) {
                        ++dataIdx;
                    }
                    rt_UpdateLogVar(val,  data[dataIdx], isVarDims[i]);
                    val = val->next;
                    ++dataIdx;
                }
            } else {
                int_T  i;
                int_T  dataIdx;

                for (i = 0, dataIdx = 0; i < ny && var[i] != NULL; i++) {
                    LogVar *val = var[i]->signals.values;
                    boolean_T   *isVarDims = var[i]->signals.isVarDims;

                    /* time */
                    if (var[i]->logTime) {
                        rt_UpdateLogVar(var[i]->time, tPtr, false);
                    }

                    /* signals */
                    while (data[dataIdx] == NULL) {
                        ++dataIdx;
                    }
                    rt_UpdateLogVar(val,  data[dataIdx], isVarDims[0]);
                    val = val->next;
                    ++dataIdx;
                }
            }
        }
        /* final state */
        if (logInfo->xFinal != NULL) {
            StructLogVar *xf  = logInfo->xFinal;
            LogVar       *val = xf->signals.values;
            int_T        nsig = xf->signals.numSignals;
            int_T        i;

            /* time */
            if (xf->logTime) {
                rt_UpdateLogVar(xf->time, tPtr, false);
            }

            /* signals */
            for (i = 0; i < nsig; i++) {
                LogSignalPtrsType data = rtliGetLogXSignalPtrs(li);

                rt_UpdateLogVar(val, data[i], false);
                val = val->next;
            }
        }
    }

    return(NULL);

} /* end rt_UpdateTXYLogVars */
BEGIN_PUBLIC
#else
#define rt_UpdateTXYLogVars(li, tPtr) NULL /* do nothing */
#endif
END_PUBLIC          

BEGIN_PUBLIC
#ifdef __cplusplus
}
#endif
END_PUBLIC          
          
/* Function: rt_UpdateSigLogVars ===============================================
 * Abstract:
 *	Update the Signal Logging testpoint variables that are being logged.
 *
 */

BEGIN_PUBLIC
#ifdef __cplusplus
extern "C" {
#endif
END_PUBLIC

BEGIN_PUBLIC
#if MAT_FILE == 1
END_PUBLIC
PUBLIC void rt_UpdateSigLogVars(RTWLogInfo *li, time_T *tPtr)
{
    LogInfo   *logInfo     = rtliGetLogInfo(li);

    if (logInfo->sl != NULL) {
        int_T             i;
        StructLogVar      *var = logInfo->sl;
        LogVar            *val = var->signals.values;
        int_T             nsig = var->signals.numSignals;
        LogSignalPtrsType data = rtliGetSigLogSignalPtrs(li);

        /* time */
        if (var->logTime) {
            rt_UpdateLogVar(var->time, tPtr, false);
        }

        /* signals */
        for (i = 0; i < nsig; i++) {
            /* currently, SigLog doesn't support variable-size signals */
            rt_UpdateLogVar(val, data[i], false);
            val = val->next;
        }
    }

} /* end rt_UpdateSigLogVars */
BEGIN_PUBLIC
#else
#define rt_UpdateSigLogVars(li, tPtr); /* do nothing */
#endif
END_PUBLIC          

BEGIN_PUBLIC
#ifdef __cplusplus
}
#endif
END_PUBLIC

/* Function: rt_StopDataLoggingForRaccel ============================================
 * Abstract:
 *	Write logged data to model.mat and free memory.
 */

BEGIN_PUBLIC
#ifdef __cplusplus
extern "C" {
#endif
END_PUBLIC

PUBLIC void rt_StopDataLoggingForRaccel(const char_T *file, RTWLogInfo *li, int verbose)
{
    FILE          *fptr;
    LogInfo       *logInfo     = (LogInfo*) rtliGetLogInfo(li);
    LogVar        *var         = logInfo->logVarsList;
    StructLogVar  *svar        = logInfo->structLogVarsList;

    boolean_T     emptyFile    = 1; /* assume */
    boolean_T     errFlag      = 0;
    const char_T  *msg;

    /*******************************
     * Create MAT file with header *
     *******************************/
    if ((fptr=fopen(file,"w+b")) == NULL) {
        (void)fprintf(stderr,"*** Error opening %s",file);
        goto EXIT_POINT;
    }
    if (rt_WriteMat5FileHeader(fptr)) {
        (void)fprintf(stderr,"*** Error writing to %s",file);
        goto EXIT_POINT;
    }

    /**************************************************
     * First log all the variables in the LogVar list *
     **************************************************/
    while (var != NULL) {
        if ( (msg = rt_FixupLogVar(var,verbose)) != NULL ) {
            (void)fprintf(stderr,"*** Error writing %s due to: %s\n",file,msg);
            errFlag = 1;
            break;
        }
        if (var->nDataPoints > 0) {
            MatItem item;

            item.type   = matMATRIX;
            item.nbytes = 0; /* not yet known */
            item.data   = &(var->data);
            if (rt_WriteItemToMatFile(fptr, &item, MATRIX_ITEM)) {
                (void)fprintf(stderr,"*** Error writing log variable %s to "
                              "file %s",var->data.name, file);
                errFlag = 1;
                break;
            }
            emptyFile = 0;
        }
        var = var->next;
    }
    /* free up some memory by destroying the log var list here */
    rt_DestroyLogVar(logInfo->logVarsList);
    logInfo->logVarsList = NULL;

    /*******************************************************
     * Next log all the variables in the StrcutLogVar list *
     *******************************************************/
    while (svar != NULL) {
        MatItem item;

        if (svar->logTime) {
            var = svar->time;
            if ( (msg = rt_FixupLogVar(var,verbose)) != NULL ) {
                (void)fprintf(stderr, "*** Error writing %s due to: %s\n",
                              file, msg);
                errFlag = 1;
                break;
            }
        }

        var = svar->signals.values;
        while (var) {
            if ( (msg = rt_FixupLogVar(var,verbose)) != NULL ) {
                (void)fprintf(stderr, "*** Error writing %s due to: %s\n",
                              file, msg);
                errFlag = 1;
                break;
            }
            var = var->next;
        }

        item.type   = matMATRIX;
        item.nbytes = 0; /* not yet known */
        item.data   = svar;

        if (rt_WriteItemToMatFile(fptr, &item, STRUCT_LOG_VAR_ITEM)) {
            (void)fprintf(stderr,"*** Error writing structure log variable "
                          "%s to file %s",svar->name, file);
            errFlag = 1;
            break;
        }
        emptyFile = 0;

        svar = svar->next;
    }

    /******************
     * Close the file *
     ******************/
    (void)fclose(fptr);
    if (emptyFile || errFlag) {
        (void)remove(file);
    } else {
        if( verbose ) {
            (void)printf("** created %s **\n\n", file);
        }
    }

 EXIT_POINT:

    /****************
     * free logInfo *
     ****************/
    rt_DestroyLogVar(logInfo->logVarsList);
    logInfo->logVarsList = NULL;
    rt_DestroyStructLogVar(logInfo->structLogVarsList);
    logInfo->structLogVarsList = NULL;
    FREE(logInfo);
    rtliSetLogInfo(li,NULL);

} /* end rt_StopDataLogging */

BEGIN_PUBLIC
#ifdef __cplusplus
}
#endif
END_PUBLIC

/* Function: rt_StopDataLogging ================================================
 * Abstract:
 *	Write logged data to model.mat and free memory.
 */

BEGIN_PUBLIC
#ifdef __cplusplus
extern "C" {
#endif
END_PUBLIC

BEGIN_PUBLIC
#if MAT_FILE == 1
END_PUBLIC
PUBLIC void rt_StopDataLogging(const char_T *file, RTWLogInfo *li)
{
    rt_StopDataLoggingForRaccel(file,li,1);
} /* end rt_StopDataLogging */
BEGIN_PUBLIC
#else
#define rt_StopDataLogging(file, li); /* do nothing */
#endif
END_PUBLIC

BEGIN_PUBLIC
#ifdef __cplusplus
}
#endif
END_PUBLIC


/* [eof] rt_logging.c */