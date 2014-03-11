/*
 *   SFC_SF.H  Stateflow S-Function header file.
 *
 *   Copyright 1995-2011 The MathWorks, Inc.
 *
 *     
 */

#ifndef _SFC_SF_H_
#define _SFC_SF_H_


#ifndef FORCE_S_FUNCTION_LEVEL_ONE
#define S_FUNCTION_LEVEL 2
#endif

#if S_FUNCTION_LEVEL==2
#define MDL_RTW
#define MDL_INITIALIZE_CONDITIONS
#define MDL_SET_WORK_WIDTHS   /* Change to #undef to remove function */
#define MDL_DISABLE
#define MDL_ENABLE
#define MDL_START
#define MDL_EXT_MODE_EXEC
#define MDL_PROCESS_PARAMETERS
#define MDL_ZERO_CROSSINGS
#define MDL_DERIVATIVES
#define MDL_SIM_STATE
#endif



#include "mwmathutil.h"
#include "simstruc.h"

#include "mex.h"
#define real_T double
#define int_T int

#ifdef __cplusplus
extern "C" {
#endif


typedef struct {
    unsigned int isEMLChart;
    void				*chartInstance;
    char sFunctionName[mxMAXNAM];
    unsigned int chartInitialized;
    void (*sFunctionGateway)(void *chartInstance);
    void (*extModeExec)(void *chartInstance);
    void (*restoreLastMajorStepConfiguration)  (void *chartInstance);
    void (*restoreBeforeLastMajorStepConfiguration)  (void *chartInstance);
    void (*storeCurrentConfiguration)  (void *chartInstance);
    void (*initializeChart) (void* chartInstance);
    void (*terminateChart) (void *chartInstance);
    void (*enableChart) (void *chartInstance);
    void (*disableChart) (void *chartInstance);
    const mxArray* (*getSimState) (SimStruct* S);
    const mxArray* (*getSimStateInfo) (void);
    void (*setSimState) (SimStruct* S, const mxArray *st);
    void (*zeroCrossings) (void *chartInstance);
    void (*outputs)(void *chartInstance);
    void (*derivatives)(void *chartInstance);
    void (*mdlRTW)(SimStruct *S);
    void (*mdlSetWorkWidths)(SimStruct *S);
    void (*mdlStart)(SimStruct *S);
} ChartInfoStruct; 

extern BuiltInDTypeId sf_get_sl_type_from_ml_type(mxClassID mlType);

extern void mdlInitializeSizes(SimStruct *S);
extern void mdlInitializeSampleTimes(SimStruct *S);
extern void mdlTerminate(SimStruct * S);
extern void mdlInitializeConditions(SimStruct *S);
extern void mdlOutputs(SimStruct *S, int_T tid);
extern void mdlExtModeExec(SimStruct *S);
extern void mdlRTW(SimStruct *S);
extern void mdlSetWorkWidths(SimStruct *S);
extern void mdlEnable(SimStruct *S);
extern void mdlDisable(SimStruct *S);
extern void mdlStart(SimStruct *S);
extern void mdlProcessParameters(SimStruct *S);
extern mxArray* mdlGetSimState(SimStruct *S);
extern void mdlSetSimState(SimStruct *S, const mxArray *st);

#ifdef MDL_ZERO_CROSSINGS
extern void mdlZeroCrossings(SimStruct *S);
#endif
#ifdef MDL_DERIVATIVES
extern void mdlDerivatives(SimStruct *S);
#endif

extern void *sf_get_chart_instance_ptr(SimStruct *S);
unsigned int sf_rtw_info_uint_prop(SimStruct *S,
                                   mxArray *infoStruct,
								   unsigned int chartFileNumber,
								   const char *uintPropName);
unsigned int sf_is_chart_inlinable(SimStruct *S,
                                   mxArray *infoStruct,
								   unsigned int chartFileNumber);
void sf_mark_output_events_with_multiple_callers(SimStruct *S,
                                                 mxArray *infoStruct,
								                 unsigned int chartFileNumber,
								                 unsigned numOutputFcnCalls);
void sf_mark_chart_reusable_outputs(SimStruct *S,
                                    mxArray *infoStruct,
								    unsigned int chartFileNumber,
								    unsigned numOutputData);
void sf_mark_chart_expressionable_inputs(SimStruct *S,
                                         mxArray *infoStruct, 
										unsigned int chartFileNumber,
										unsigned numInputData);
extern unsigned int sim_mode_is_rtw_gen(SimStruct *S);
extern unsigned int sim_mode_is_external(SimStruct *S);
extern unsigned int sim_mode_is_modelref_sim(SimStruct *S);

extern DTypeId sf_get_fixpt_data_type_id(SimStruct *S,
                                  unsigned int nBits,
                                  bool isSigned,
                                  int exponent,
                                  double slope,
                                  double bias);
extern DTypeId sf_get_enum_data_type_id(SimStruct *S,
                                  const char *enumTypeName);
extern DTypeId sf_get_param_data_type_id(SimStruct *S,
					 int idx);
extern unsigned int sf_mex_listen_for_ctrl_c(SimStruct *S);
extern unsigned int sf_mex_listen_for_ctrl_c_force(SimStruct *S);
extern const mxArray *sf_mex_get_sfun_param(SimStruct *S, int_T paramIndex, int_T bStrict);
extern void *sf_get_runtime_param_data(void *SVoid, int_T paramIndex);
extern void sf_call_output_fcn_call(SimStruct *S, 
							 int eventIndex, 
							 const char *eventName,
							 int checkForInitialization);
extern mxArray *sf_load_rtw_optimization_info(const char *machineName,
                                       const char *mainMachineName);
extern void sf_clear_rtw_identifier(SimStruct *S);
void sf_write_symbol_mapping(SimStruct *S,
                             mxArray *infoStruct, 
							 unsigned int chartFileNumber);
extern void sf_call_output_fcn_enable(SimStruct *S, 
							 int eventIndex, 
							 const char *eventName,
                             int checkForInitialization);
extern void sf_call_output_fcn_disable(SimStruct *S, 
							 int eventIndex, 
							 const char *eventName,
                             int checkForInitialization);
void sf_set_rtw_dwork_info(SimStruct *S, mxArray *infoStruct,unsigned chartFileNumber);
mxArray* sf_get_dwork_info_from_mat_file(SimStruct *S,
                                         mxArray *infoStruct, 
                                         unsigned chartFileNumber,
                                         const char* propName);
void sf_set_encoded_dwork_info(SimStruct *S,
                               const char *encodedDWorkInfo[],
                               int len,
                               int segSize);
SimStruct* sf_get_subchart_simstruct(SimStruct *S, const char* blockName);
extern mxArray* sf_get_sim_state_info(SimStruct *S);
extern void sf_set_sim_state(SimStruct *S, const mxArray *st);

void sf_send_message_wrapper(SimStruct* S, int i);
unsigned int sf_receive_message_wrapper(SimStruct* S, int i);
unsigned int sf_check_out_message_wrapper(SimStruct* S, int i);
unsigned int sf_check_in_message_wrapper(SimStruct* S, int i);
unsigned int sf_no_message_wrapper(SimStruct* S);

#define SF_SIM_RUNNING 0
#define SF_SIM_STOPPED 1
#define SF_SIM_PAUSED 2

/* work around the buggy macro in simstruc.h until it is fixed */
#define cdrGetOutputPortReusable(S,port) \
  ( (S)->portInfo.outputs[(port)].attributes.optimOpts != \
   SS_NOT_REUSABLE_AND_GLOBAL )


#ifdef __cplusplus
}
#endif

#endif

