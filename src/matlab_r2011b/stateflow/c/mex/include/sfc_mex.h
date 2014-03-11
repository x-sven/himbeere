/*
 *   SFC_MEX.H  Stateflow mex header file.
 *
 *   Copyright 1995-2011 The MathWorks, Inc.
 *
 *     
 */

#ifndef SFC_MEX_H

#define SFC_MEX_H

#include "matrix.h"
#include "mex.h"
#include <setjmp.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "tmwtypes.h"
#include "simstruc.h"
#include "emlrt.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_DIMENSIONS (16)
#define SFC_STRBUF_LEN (1024)
#define SF_MEX_ERROR_BUFFER_LEN (2048*2)
#define SF_MEX_ERROR_PREFIX_LEN 256

typedef enum {
    SF_DOUBLE  = 0,
    SF_SINGLE,
    SF_INT8,
    SF_UINT8,
    SF_INT16,
    SF_UINT16,
    SF_INT32,
    SF_UINT32,
    SF_INT64,
    SF_UINT64,
    SF_CHAR,
    SF_BOOL,
    SF_MATLAB,
    SF_STRUCT,
    SF_UNKNOWN_ZZ, /* G338679: tweak this so it doesn't collide with a system header file */
    SF_STRING,
    SF_ENUM,
    SF_TOTAL_DATA_TYPES
}SfDataType;

typedef enum {
     SF_CONTIGUOUS
    ,SF_NON_CONTIGUOUS
}SfArrayType;

typedef enum {
    SF_ROW_MAJOR,
    SF_COLUMN_MAJOR
} SfIndexScheme;

#define MATLAB_CLASSES(X) \
    X(LOGICAL,boolean_T,int) \
    X(CHAR,char_T,int)    \
    X(DOUBLE,real_T,double)  \
    X(SINGLE,real32_T,double)  \
    X(INT8,int8_T,int)    \
    X(UINT8,uint8_T,int)   \
    X(INT16,int16_T,int)   \
    X(UINT16,uint16_T,int)  \
    X(INT32,int32_T,int)   \
    X(UINT32,uint32_T,unsigned int)

typedef enum {
#define DEFINE_ENUM(NAME,CTYPE,CTYPE2) ML_##NAME = mx##NAME##_CLASS,
   MATLAB_CLASSES(DEFINE_ENUM)
#undef DEFINE_ENUM
   ML_MX_ARRAY,
   ML_STRING,
   ML_VOID,
   ML_LAST_DATA_TYPE
} MatlabDataType; /* useful for passing arguments to sf_mex_call*/

#ifdef __cplusplus
#define MatlabWorkspaceType int
#define BASE_WORKSPACE   0
#define CALLER_WORKSPACE 1
#define GLOBAL_WORKSPACE 2
#define ALL_WORKSPACES   3
#else
typedef enum {
    BASE_WORKSPACE,
    CALLER_WORKSPACE,
    GLOBAL_WORKSPACE,
    ALL_WORKSPACES} MatlabWorkspaceType;
#endif

typedef void (*DebuggerSimStatusListenerFcn)(unsigned int);

extern int strcmp_ignore_ws( const char *string1, const char *string2 );
extern bool sf_mex_get_halt_simulation(void);
extern void sf_mex_clear_error_message(void);
extern const char* sf_mex_get_error_message_for_read(void);
extern unsigned int sf_mex_get_error_message_buffer_length(void);
void sf_mex_printf(const char *msg, ...);
bool sf_mex_mx_is_numeric(const mxArray *mxArrayPtr);

/*
 * Is array temporary
 */

/*
#ifndef mxIsTemp
extern int mxIsTemp(const mxArray *pa);
#endif

#if defined(ARRAY_ACCESS_INLINING)
#define mxIsTemp(pa)     (mxGetArrayScope(pa) == mxTEMPORARY_SCOPE)
#endif */ /* defined(ARRAY_ACCESS_INLINING) */


#define SF_DUPLICATE_ARRAY


/*
 * Calls to the following functions and macros should always be followed immediately by either RETURN_ON_ERROR
 * or RETURN_VALUE_ON_ERROR to propagate errors up the call stack.
 * Technically, these are not necessary if the call is followed immediately by an
 * unconditional return.  However, any failure to follow this regime is likely to produce
 * very subtle and difficult bugs so I urge rigid adherence to the discipline of adding
 * the macros.  It's also good documentation that you know what you're doing.
 *
 * Sometimes these macros don't work well with sf_mex_call_matlab() because you need to
 * clean up mxArrays etc before returning (to avoid leaks).  So sf_mex_call_matlab traps the error
 * and also returns it.  Test the result for non-zero and do the right thing.
 */
extern int sf_mex_call_matlab(int nlhs, mxArray* plhs[], int nrhs, mxArray* prhs[], const char* fcnName);
extern const mxArray *sf_mex_evalin(MatlabWorkspaceType workspaceType, unsigned int numReturn, const char *evalFormatStr, ...);

/* Difference between these two function is in error handling. In case of error in MATLAB function, sf_mex_call() 
   longjumps into Simulink, while sf_mex_call_debug() pops up Stateflow/EML debugger.
 */
extern const mxArray *sf_mex_call(const char *functionName, unsigned int numReturn, unsigned int numArguments,...);
extern const mxArray *sf_mex_call_debug(const char *functionName, unsigned int numReturn, unsigned int numArguments,...);

extern const mxArray *sf_mex_get_ml_var_no_error(const char *variableName, MatlabWorkspaceType workspaceType);
extern const mxArray *sf_mex_get_ml_var(const char *variableName, MatlabWorkspaceType workspaceType);


extern void sf_mex_v_errbuf_createf(char *errorBuffer, const char* fmt, va_list argPtr);
extern void sf_mex_errbuf_createf(char *errorBuffer, const char* fmt, ... );
extern void sf_mex_v_errbuf_catf(char *errorBuffer, const char* fmt, va_list argPtr);
extern void sf_mex_errbuf_catf(char *errorBuffer, const char* fmt, ... );
extern void sf_mex_errbuf_copy(char *errorBuffer, const char* inStr);
extern void sf_mex_errbuf_clear(char *errorBuffer);
extern bool sf_mex_errbuf_isempty(char *errorBuffer);

extern void sf_mex_enter_parallel_region(void);
extern void sf_mex_exit_parallel_region(void);
extern void sf_mex_set_error_status(SimStruct *S, const char *fmt, ...);
extern void sf_mex_set_halt_simulation(bool halt);
extern void sf_mex_error_message(const char *msg, ...);
extern void sf_mex_error_message_no_long_jump(const char *errMsg);
extern void sf_mex_error_direct_call(SimStruct *s, const char *fcnName, const char *fileName);
extern void sf_mex_stop_simulation_abruptly_wo_error(void);
extern void sf_mex_warning_message(const char *msg, ...);
extern void sf_mex_long_jump_on_error(unsigned int longJumpType);
extern void sf_assertion_failed(const char *expr, const char *in_file, int at_line, const char *msg);
extern void sf_clear_error_manager_info(void);
extern void sf_set_error_prefix_string(const char *errorPrefixString);
extern void sf_mex_set_debugger_sim_status_listener_fcn(DebuggerSimStatusListenerFcn listenerFcn);

extern void sf_set_error_debugger_context_message_fcn(void (*debuggerContextMessageFcn)(char *));
extern void sf_set_debugger_matlab_error_fcn(void (*debuggerMatlabErrorFcn)(char*));

extern char * sf_get_error_prefix_string(void);
extern void sf_echo_expression(const char *expr, real_T value);
extern const mxArray *sf_mex_getfield(const mxArray *mxStruct, /* src: structure containing field */
                             const char *fldName,       /* name: field name */
                             const char *errName,       /* name: name to use for errors */
                             int index);                /* src: array index */
extern const mxArray *sf_mex_addfield(const mxArray *mxStruct, /* src: structure to update */
                             const mxArray *mxField,    /* src: field to add */
                             const char *fldName,       /* name: field name */
                             const char *errName,       /* name: name to use for errors */
                             int index);                /* src: array index */
extern void sf_mex_check_struct(const struct emlrtMsgIdentifier *aMsgId, const mxArray *s, int32_T nFields, const char **fldNames,
                                uint32_T nDims, const void *pDims);
extern void sf_mex_check_struct_vs(const struct emlrtMsgIdentifier *aMsgId, const mxArray *aSrcMx, int32_T aNFields, const char **aFldNames,
                              int aNumDims,           /* dest: desired number of dimensions */
                              boolean_T *aDynamic,    /* src: dynamic dimensions flags */
                              const void *aDimsMax,   /* src: dimensions maximima */
                              void *aDimsActual);     /* dest: actual dimensions */
extern void sf_mex_check_fi(const struct emlrtMsgIdentifier *aMsgId, const mxArray *fi, boolean_T complex, uint32_T nDims,
                            const void *vDims, const mxArray *fimath, const mxArray *numericType);
extern void sf_mex_check_fi_vs(const struct emlrtMsgIdentifier *aMsgId, const mxArray *aSrcFI, boolean_T aComplex, 
                              int aNumDims,               /* dest: desired number of dimensions */
                              const boolean_T *aDynamic,  /* src: dynamic dimensions flags */
                              const void *aDimsMax,       /* src: dimensions maximima */
                              void *aDimsActual,          /* dest: actual dimensions */
                              const mxArray *aFiMath, const mxArray *aNumericType);

extern void sf_mex_check_builtin(const struct emlrtMsgIdentifier *aMsgId, const mxArray *mx, const char *clsName, boolean_T complex, uint32_T nDims, const void *vDims);

extern void sf_mex_check_builtin_vs(const struct emlrtMsgIdentifier *aMsgId, const mxArray *mx, const char *clsName, boolean_T complex, uint32_T nDims, const void *pDims, const boolean_T *aDynamic, void *aDimsOut);

extern void sf_mex_check_enum(const char *enumName, int numFields, const char **elNames, const int32_T *elValues);
extern const mxArray *sf_mex_create_enum(const char *enumName, const mxArray *mxValues);
extern int32_T sf_mex_get_enum_element(const mxArray *mxValues, int index);

extern const mxArray *sf_mex_createstruct(const char *nameStr,      /* name: name to use for errors */
                             int numDims,              /* src: number of dimensions */
                             ...);                     /* src: dimension sizes */
extern const mxArray *sf_mex_createstructarray(const char *aNameStr,      /* name: name to use for errors */
                             uint32_T aNumDims,        /* src: number of dimensions */
                             const void *aDims);       /* src: dimension sizes */
extern const mxArray *sf_mex_create( const char *nameStr,       /* name: name to use for errors */
                                     const void *dataPtr,       /* src: data to create mxArray */
                                     int dataType,              /* src: input data type */
                                     unsigned int isComplex,    /* src: is complex data? */
                                     int indexScheme,           /* src: index scheme for input data */
                                     unsigned int aElementSize, /* src: element size, in bytes */
                                     int numDims,               /* src: number of dimensions */
                                     ...);                      /* src: dimension sizes */

/* Creates a 1 by sz cellarray */
extern const mxArray *sf_mex_createcellarray(int sz);

extern void sf_mex_setcell(const mxArray* array, int index, const mxArray* value);
extern const mxArray* sf_mex_getcell(const mxArray* array, int index);

extern double sf_mex_ml_var_sub(const char *wsDataName,         /* workspace data name */
                                 MatlabWorkspaceType workspaceType, /* workspace type */
                                 int numDims,                       /* desired number of dimensions */
                                 ...);                              /* desired index for each dimension */
extern void sf_mex_ml_var_subsasgn(const char *wsDataName,
                               MatlabWorkspaceType workspaceType,
                               double value,
                               int numDims,
                               ...);
extern void sf_mex_assign(const mxArray **sfMatlabDataPtr,
                                               const mxArray *srcMxArray, bool makePersistent);
extern const mxArray *sf_mex_dup(const mxArray *srcMxArray);
extern void sf_mex_destroy(const mxArray **sfMatlabDataPtr);
extern mxClassID sf_mex_get_mx_datatype_from_sf_datatype(SfDataType dataType);
extern void sf_mex_export(const char *vectName,
                          const mxArray *mxArrayPtr,
                          MatlabWorkspaceType workspaceType);
extern void sf_mex_import(const struct emlrtMsgIdentifier *aMsgId, /* aMsgId: name to use for errors */
                          const mxArray *mxArrayPtr,  /* src:  mxArray to copy from */
                          void *dataPtr,              /* dest: memory place to put array data */
                          int isStrict,               /* isStrict: Enforce type/size match */
                          int dataType,               /* dest: array data type */
                          unsigned int isComplex,     /* dest: complex data or not */
                          int indexScheme,            /* dest: index scheme, row-major or column major */
                          unsigned int aElementSize,  /* dest: element size, in bytes */
                          int numDims,                /* dest: desired number of dimensions */
                          ...);                       /* dest: desired length for each dimension */
extern void sf_mex_import_named(const char *nameStr,  /* name: name to use for errors */
                          const mxArray *mxArrayPtr,  /* src:  mxArray to copy from */
                          void *dataPtr,              /* dest: memory place to put array data */
                          int isStrict,               /* isStrict: Enforce type/size match */
                          int dataType,               /* dest: array data type */
                          unsigned int isComplex,     /* dest: complex data or not */
                          int indexScheme,            /* dest: index scheme, row-major or column major */
                          unsigned int aElementSize,  /* dest: element size, in bytes */
                          int numDims,                /* dest: desired number of dimensions */
                          ...);                       /* dest: desired length for each dimension */
extern void sf_mex_import_vs(const struct emlrtMsgIdentifier *aMsgId, /* aMsgId: name to use for errors */    
                              const mxArray *aSrcMx,      /* src:  mxArray to copy from */
                              void *aDstData,             /* dest: memory place to put array data */
                              int aStrict,                /* aStrict: should we be strict about type/size matching.*/
                              int aDataType,              /* dest: array data type */
                              unsigned int aComplex,      /* dest: complex data or not */
                              int aIndexScheme,           /* dest: index scheme*/
                              unsigned int aElementSz,    /* dest: element size, in bytes */
                              int aNumDims,               /* dest: desired number of dimensions */
                              const boolean_T *aDynamic,  /* src: dynamic dimensions flags */
                              const void *aDimsMax,       /* src: dimensions maximima */
                              void *aDimsActual);         /* dest: actual dimensions */
extern void sf_mex_import_fi_vs(
                              const mxArray *aFiMx,     /* src: original FI mxArray to copy from */
                              const mxArray *aIntMx,    /* src: simulinkarray representation of aFiMx */
                              void *aOutData,           /* dst: memory place to put array data */
                              int32_T aElementSize,     /* src: element size, in bytes */
                              uint32_T aDimsMax,        /* src: number of elements in aDimsOut */
                              void *aDimsActual);       /* dst: actual dimensions */
const mxArray* sf_mex_convert_from_fi(const mxArray* mxArrayPtr, SfDataType dataType);
const mxArray* sf_mex_convert_to_double(const mxArray* mxArrayPtr);
bool mx_array_has_fi_type(const mxArray *mxArrayPtr);
extern const mxArray *sf_mex_lower_fixpt_mx_array(const mxArray *mxArrayPtr,
                                           int fixptExponent,
                                           double fixptSlope,
                                           double fixPtBias,
                                           unsigned int nBits,
                                           unsigned int isSigned);
extern double sf_mex_sub(const mxArray *mxArrayPtr, /* pointer to SF MATLAB data */
                                               const char *nameStr,       /* name: name to use for errors */
                                               int numDims,               /* desired number of dimensions */
                                               ...);                      /* desired index for each dimension */
extern void sf_mex_subsasgn(const mxArray *mxArrayPtr,  /* pointer to SF MATLAB data */
                                             const char *nameStr,       /* name: name to use for errors */
                                            double value,
                                            int numDims,                /* desired number of dimensions */
                                            ...);                       /* desired index for each dimension */
extern mxArray *sf_mex_decode_encoded_mx_struct_array(const char *encStr[], int len, int segSize);

/* If the *ppMxArray is not NULL, dispatches to mxDuplicateArray; otherwise returns empty matrix. */
extern const mxArray *sf_mex_duplicatearraysafe(const mxArray** ppMxArray);

extern void ml_set_element_value_in_array(const mxArray *mxArrayPtr,
										  const char *nameStr,
										  unsigned int index,
										  double value,
										  bool setImaginary);

extern jmp_buf *sf_get_current_jmp_buf_ptr(void);
extern void sf_set_current_jmp_buf_ptr(jmp_buf *jumpbufPtr);
int sf_mex_lw_bounds_check(int indexValue,int firstIndex,int lastIndex);

void sf_mex_lw_size_eq_check_1d(int dim1, int dim2);
void sf_mex_lw_size_eq_check_nd(const int* dims1, const int* dims2, int nDims);
void sf_mex_lw_dim_size_eq_check(int dim1, int dim2, int index);
void sf_mex_lw_dim_size_geq_check(int dim1, int dim2, int index);
void sf_mex_lw_sub_assign_size_check_nd(const int* dims1, int nDims1, const int* dims2, int nDims2);
void sf_mex_lw_matrix_matrix_index_check(const int* dims1, int nDims1, const int* dims2, int nDims2);
void sf_mex_lw_vector_vector_index_check(int sizeL_0, int sizeL_1, int sizeR_0, int sizeR_1);
void sf_mex_lw_for_loop_vector_check(double start, double step, double end, mxClassID classID, int n);
void sf_mex_lw_runtime_error_msgid(const char* msgID);

double sf_mex_lw_non_negative_check(double val);

/* Called from within sfdebug.c to avoid code duplication. */

#define EML_MATRIX_MATRIX_ERROR_MSG "Matrix(matrix) subscript turned out to be a vector(vector) subscript with shape different from compiled assumption."
#define EML_VECTOR_VECTOR_ERROR_MSG "Vector(vector) subscript turned out to be a scalar(vector) subscript with shape different from compiled assumption."
#define EML_MATRIX_MATRIX_DOC "See <a href=\"matlab:emlhelp('eml_error_ertMatrixMatrixIndexing');\">documentation</a> for details.\n"
#define EML_FOR_LOOP_VECTOR_MSG "Cannot determine the exact number of iterations for a loop with range %g:%g:%g."
int sf_matrix_matrix_index_ok(const int32_T* dims1, int32_T nDims1, const int32_T* dims2, int32_T nDims2);
int sf_vector_vector_index_ok(int sizeL_0, int sizeL_1, int sizeR_0, int sizeR_1);

#define sf_assertion(expr) ((expr)?(void)0:sf_assertion_failed(#expr,__FILE__,__LINE__,NULL))
#define sf_assertion_msg(expr,msg) ((expr)?(void)0:sf_assertion_failed(#expr,__FILE__,__LINE__,msg))

extern void sf_mex_overflow_error();
extern void sf_mex_dividebyzero_error();
extern void sf_mex_unsafe_enum_error();
int *ssGetCurrentInputPortDimensions_wrapper(SimStruct *S,int portNumber);
int *ssGetCurrentOutputPortDimensions_wrapper(SimStruct *S,int portNumber);

bool sf_mex_emlrt_context_is_debug_mode();
void sf_mex_emlrt_context_set_debug_mode( bool b );

#ifndef TRUE
#define TRUE (1U)
#endif

#ifndef FALSE
#define FALSE (0U)
#endif

#define mexPrintf sf_mex_printf
#ifdef printf
#undef printf
#endif
#define printf sf_mex_printf

#ifndef sf_MIN
#define sf_MIN(a,b)    (((a) < (b)) ? (a) : (b))
#endif
#ifndef sf_MAX
#define sf_MAX(a,b)    (((a) > (b)) ? (a) : (b))
#endif

#ifndef RT_PI
#define RT_PI          3.14159265358979323846
#endif

#ifndef RT_PIF
#define RT_PIF         3.1415927F
#endif

#ifndef RT_LN_10
#define RT_LN_10       2.30258509299404568402
#endif

#ifndef RT_LN_10F
#define RT_LN_10F      2.3025851F
#endif

#ifndef RT_LOG10E
#define RT_LOG10E      0.43429448190325182765
#endif

#ifndef RT_LOG10EF
#define RT_LOG10EF     0.43429449F
#endif

#ifndef RT_E
#define RT_E           2.7182818284590452354
#endif

#ifndef RT_EF
#define RT_EF          2.7182817F
#endif

#ifdef __cplusplus
}
#endif

#endif /*SF_MEX_H */

