/*
 * File: sfblk_manswitch.c
 *
 *  Support utility s-function for Simulink Manual Switch.
 *  Copies input to output, ensuring that block doesn't become constant.
 *
 * Copyright 1990-2009 The MathWorks, Inc.
 */


#define S_FUNCTION_NAME sfblk_manswitch
#define S_FUNCTION_LEVEL 2

#include "simstruc.h"


/*====================*
 * S-function methods *
 *====================*/

/* Function: mdlInitializeSizes ===============================================
 * Abstract:
 *   Setup sizes of the various vectors.
 */
static void mdlInitializeSizes(SimStruct *S)
{

    ssSetNumSFcnParams(S, 0);
     if (ssGetNumSFcnParams(S) != ssGetSFcnParamsCount(S)) {
	 return; /* Parameter mismatch will be reported by Simulink */
     }

    ssSetNumContStates(S, 0);
    ssSetNumDiscStates(S, 0);

    if (!ssSetNumInputPorts(S, 1)) return;
    ssSetInputPortWidth(S, 0, 1);
    ssSetInputPortDataType(S, 0, DYNAMICALLY_TYPED );
    ssSetInputPortDirectFeedThrough(S, 0, 1);
    ssSetInputPortRequiredContiguous(S, 0, 1); /*direct input signal access*/

    if (!ssSetNumOutputPorts(S, 1)) return;
    ssSetOutputPortWidth(S, 0, 1);
    ssSetOutputPortDataType(S, 0, DYNAMICALLY_TYPED);
    ssSetOutputPortComplexSignal(S, 0, COMPLEX_NO);
    ssSetNumSampleTimes(S, 1);
    ssSetNumRWork(S, 0);
    ssSetNumIWork(S, 0);
    ssSetNumPWork(S, 0);
    ssSetNumModes(S, 0);
    ssSetNumNonsampledZCs(S, 0);
    ssSetSimStateCompliance(S, USE_DEFAULT_SIM_STATE);
  
    ssSetOptions(S, (SS_OPTION_EXCEPTION_FREE_CODE |
                     SS_OPTION_USE_TLC_WITH_ACCELERATOR |
                     SS_OPTION_DISALLOW_CONSTANT_SAMPLE_TIME |
                     SS_OPTION_NONVOLATILE |    
                     SS_OPTION_WORKS_WITH_CODE_REUSE));
    
    ssSupportsMultipleExecInstances(S, true);

}


/* Function: mdlInitializeSampleTimes =========================================
 * Abstract:
 *    Specifiy  the sample time.
 */
static void mdlInitializeSampleTimes(SimStruct *S)
{
    ssSetSampleTime(S, 0, INHERITED_SAMPLE_TIME);
    ssSetOffsetTime(S, 0, 0.0);
}

/* Function: mdlOutputs =======================================================
 *
*/
static void mdlOutputs(SimStruct *S, int_T tid)
{
  DTypeId         uType         = ssGetInputPortDataType(S,0);
  size_t          uTypeSize     = (size_t) ssGetDataTypeSize(S, uType);
  const char   *u  = (const char*) ssGetInputPortSignal(S,0);
  char         *y  = (char *)ssGetOutputPortRealSignal(S,0); 
  
  (void)memcpy(y,u,uTypeSize);  
}



/* Function: mdlTerminate =====================================================
 * Abstract:
 *    In this function, you should perform any actions that are necessary
 *    at the termination of a simulation.  For example, if memory was
 *    allocated in mdlStart, this is the place to free it.
 */
static void mdlTerminate(SimStruct *S)
{
}

#ifdef  MATLAB_MEX_FILE    /* Is this file being compiled as a MEX-file? */
#include "simulink.c"      /* MEX-file interface mechanism */
#else
#include "cg_sfun.h"       /* Code generation registration function */
#endif


