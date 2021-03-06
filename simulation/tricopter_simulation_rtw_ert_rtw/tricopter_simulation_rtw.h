/*
 * File: tricopter_simulation_rtw.h
 *
 * Code generated for Simulink model 'tricopter_simulation_rtw'.
 *
 * Model version                  : 1.246
 * Simulink Coder version         : 8.1 (R2011b) 08-Jul-2011
 * TLC version                    : 8.1 (Aug  6 2011)
 * C/C++ source code generated on : Sun Apr 20 14:49:51 2014
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: 32-bit Generic
 * Emulation hardware selection:
 *    Differs from embedded hardware (MATLAB Host)
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#ifndef RTW_HEADER_tricopter_simulation_rtw_h_
#define RTW_HEADER_tricopter_simulation_rtw_h_
#include "rtw_modelmap.h"
#ifndef tricopter_simulation_rtw_COMMON_INCLUDES_
# define tricopter_simulation_rtw_COMMON_INCLUDES_
#include <math.h>
#include <string.h>
#include <stddef.h>
#include <float.h>
#include "rtwtypes.h"
#include "rt_nonfinite.h"
#include "rtGetInf.h"
#include "rtGetNaN.h"
#include "rt_defines.h"
#endif                                 /* tricopter_simulation_rtw_COMMON_INCLUDES_ */

#include "tricopter_simulation_rtw_types.h"
#include <stddef.h>

/* Macros for accessing real-time model data structure */
#ifndef rtmGetDataMapInfo
# define rtmGetDataMapInfo(rtm)        ((rtm)->DataMapInfo)
#endif

#ifndef rtmSetDataMapInfo
# define rtmSetDataMapInfo(rtm, val)   ((rtm)->DataMapInfo = (val))
#endif

#ifndef rtmGetErrorStatus
# define rtmGetErrorStatus(rtm)        ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
# define rtmSetErrorStatus(rtm, val)   ((rtm)->errorStatus = (val))
#endif

/* Block signals (auto storage) */
typedef struct {
  real_T DLookupTable;                 /* '<S36>/1-D Lookup Table' */
  real_T DLookupTable_b;               /* '<S37>/1-D Lookup Table' */
  real_T DLookupTable_h;               /* '<S38>/1-D Lookup Table' */
} BlockIO_tricopter_simulation_rt;

/* Block states (auto storage) for system '<Root>' */
typedef struct {
  real_T Delay_DSTATE;                 /* '<S38>/Delay' */
  real_T UnitDelay_DSTATE;             /* '<S50>/Unit Delay' */
  real_T UnitDelay1_DSTATE;            /* '<S50>/Unit Delay1' */
  real_T DiscreteTimeIntegrator1_DSTATE;/* '<S50>/Discrete-Time Integrator1' */
  real_T DiscreteTimeIntegrator_DSTATE;/* '<S50>/Discrete-Time Integrator' */
  real_T Rates_DSTATE[3];              /* '<S4>/Rates' */
  real_T UnitDelay_DSTATE_g;           /* '<S51>/Unit Delay' */
  real_T UnitDelay1_DSTATE_n;          /* '<S51>/Unit Delay1' */
  real_T DiscreteTimeIntegrator1_DSTAT_m;/* '<S51>/Discrete-Time Integrator1' */
  real_T DiscreteTimeIntegrator_DSTATE_m;/* '<S51>/Discrete-Time Integrator' */
  real_T Delay_DSTATE_j;               /* '<S36>/Delay' */
  real_T UnitDelay_DSTATE_n;           /* '<S48>/Unit Delay' */
  real_T UnitDelay1_DSTATE_j;          /* '<S48>/Unit Delay1' */
  real_T DiscreteTimeIntegrator1_DSTAT_i;/* '<S48>/Discrete-Time Integrator1' */
  real_T DiscreteTimeIntegrator_DSTATE_h;/* '<S48>/Discrete-Time Integrator' */
  real_T Delay_DSTATE_c;               /* '<S37>/Delay' */
  real_T UnitDelay_DSTATE_i;           /* '<S49>/Unit Delay' */
  real_T UnitDelay1_DSTATE_nn;         /* '<S49>/Unit Delay1' */
  real_T DiscreteTimeIntegrator1_DSTAT_f;/* '<S49>/Discrete-Time Integrator1' */
  real_T DiscreteTimeIntegrator_DSTATE_e;/* '<S49>/Discrete-Time Integrator' */
  real_T FixPtUnitDelay1_DSTATE[4];    /* '<S24>/FixPt Unit Delay1' */
  real_T Velocity_DSTATE[3];           /* '<S4>/Velocity' */
  real_T Position_DSTATE[3];           /* '<S4>/Position' */
  real_T Memory1_PreviousInput[3];     /* '<S2>/Memory1' */
  real_T Memory_PreviousInput[3];      /* '<S2>/Memory' */
  uint8_T FixPtUnitDelay2_DSTATE;      /* '<S24>/FixPt Unit Delay2' */
  uint8_T DiscreteTimeIntegrator1_SYSTEM_;/* '<S50>/Discrete-Time Integrator1' */
  uint8_T DiscreteTimeIntegrator_SYSTEM_E;/* '<S50>/Discrete-Time Integrator' */
  uint8_T DiscreteTimeIntegrator1_SYSTE_a;/* '<S51>/Discrete-Time Integrator1' */
  uint8_T DiscreteTimeIntegrator_SYSTEM_i;/* '<S51>/Discrete-Time Integrator' */
  uint8_T DiscreteTimeIntegrator1_SYSTE_o;/* '<S48>/Discrete-Time Integrator1' */
  uint8_T DiscreteTimeIntegrator_SYSTEM_k;/* '<S48>/Discrete-Time Integrator' */
  uint8_T DiscreteTimeIntegrator1_SYSTE_m;/* '<S49>/Discrete-Time Integrator1' */
  uint8_T DiscreteTimeIntegrator_SYSTE_ka;/* '<S49>/Discrete-Time Integrator' */
} D_Work_tricopter_simulation_rtw;

/* Invariant block signals (auto storage) */
typedef const struct tag_ConstBlockIO_tricopter_simu {
  real_T u[3];                         /* '<S22>/1//2' */
  real_T cosphi2;                      /* '<S22>/cos(phi//2)' */
  real_T costheta2;                    /* '<S22>/cos(theta//2)' */
  real_T cospsi2;                      /* '<S22>/cos(psi//2)' */
  real_T CCC;                          /* '<S22>/CCC' */
  real_T sinphi2;                      /* '<S22>/sin(phi//2)' */
  real_T sintheta2;                    /* '<S22>/sin(theta//2)' */
  real_T sinpsi2;                      /* '<S22>/sin(psi//2)' */
  real_T SSS;                          /* '<S22>/SSS' */
  real_T Sum9;                         /* '<S22>/Sum9' */
  real_T SCC;                          /* '<S22>/SCC' */
  real_T CSS;                          /* '<S22>/CSS' */
  real_T Sum2;                         /* '<S22>/Sum2' */
  real_T CSC;                          /* '<S22>/CSC' */
  real_T SCS;                          /* '<S22>/SCS' */
  real_T Sum1;                         /* '<S22>/Sum1' */
  real_T CCS;                          /* '<S22>/CCS' */
  real_T SSC;                          /* '<S22>/SSC' */
  real_T Sum3;                         /* '<S22>/Sum3' */
  real_T MatrixConcatenation[18];      /* '<S14>/Matrix Concatenation' */
  real_T Selector[9];                  /* '<S12>/Selector' */
  real_T Gain;                         /* '<S42>/Gain' */
  real_T Sqrt;                         /* '<S42>/Sqrt' */
  real_T Gain_a;                       /* '<S44>/Gain' */
  real_T Selector1[9];                 /* '<S12>/Selector1' */
  real_T Selector2[9];                 /* '<S12>/Selector2' */
} ConstBlockIO_tricopter_simulati;

/* Constant parameters (auto storage) */
typedef struct {
  /* Expression: [-60 60]*pi/180
   * Referenced by: '<S45>/1-D Lookup Table'
   */
  real_T DLookupTable_tableDat[2];

  /* Pooled Parameter (Expression: [900 2100])
   * Referenced by:
   *   '<S36>/1-D Lookup Table'
   *   '<S37>/1-D Lookup Table'
   *   '<S38>/1-D Lookup Table'
   *   '<S45>/1-D Lookup Table'
   */
  real_T pooled16[2];

  /* Pooled Parameter (Expression: [min_rpm max_rpm]*pi/30)
   * Referenced by:
   *   '<S36>/1-D Lookup Table'
   *   '<S37>/1-D Lookup Table'
   *   '<S38>/1-D Lookup Table'
   */
  real_T pooled21[2];
} ConstParam_tricopter_simulation;

/* External inputs (root inport signals with auto storage) */
typedef struct {
  sChannelsKK deflections;             /* '<Root>/deflections' */
} ExternalInputs_tricopter_simula;

/* External outputs (root outports fed by signals with auto storage) */
typedef struct {
  sMotion motion;                      /* '<Root>/motion' */
} ExternalOutputs_tricopter_simul;

/* Real-time Model Data Structure */
struct RT_MODEL_tricopter_simulation_r {
  const char_T * volatile errorStatus;

  /*
   * DataMapInfo:
   * The following substructure contains information regarding
   * structures generated in the model's C API.
   */
  struct {
    rtwCAPI_ModelMappingInfo mmi;
    void* dataAddress[29];
    int32_T* vardimsAddress[29];
  } DataMapInfo;
};

extern const ConstBlockIO_tricopter_simulati tricopter_simulation_rtw_ConstB;/* constant block i/o */

/* Constant parameters (auto storage) */
extern const ConstParam_tricopter_simulation tricopter_simulation_rtw_ConstP;

/* External data declarations for dependent source files */
extern const sChannelsKK tricopter_simulation_rtw_rtZsChannelsKK;/* sChannelsKK ground */
extern const sMotion tricopter_simulation_rtw_rtZsMotion;/* sMotion ground */
extern const sChannelsKK tricopter_simulation_rtw_rtZsChannelsKK;/* sChannelsKK ground */
extern const sMotion tricopter_simulation_rtw_rtZsMotion;/* sMotion ground */

/* Function to get C API Model Mapping Static Info */
extern const rtwCAPI_ModelMappingStaticInfo*
  tricopter_simulation_rtw_GetCAPIStaticMap(void);

/* Class declaration for model tricopter_simulation_rtw */
class tricopter_simulation_rtwModelClass {
  /* public data and function members */
 public:
  /* External inputs */
  ExternalInputs_tricopter_simula tricopter_simulation_rtw_U;

  /* External outputs */
  ExternalOutputs_tricopter_simul tricopter_simulation_rtw_Y;

  /* Model entry point functions */

  /* model initialize function */
  void initialize();

  /* model step function */
  void step();

  /* model terminate function */
  void terminate();

  /* Constructor */
  tricopter_simulation_rtwModelClass();

  /* Destructor */
  ~tricopter_simulation_rtwModelClass();

  /* Real-Time Model get method */
  RT_MODEL_tricopter_simulation_r * getRTM() const;

  /* private data and function members */
 private:
  /* Block signals */
  BlockIO_tricopter_simulation_rt tricopter_simulation_rtw_B;

  /* Block states */
  D_Work_tricopter_simulation_rtw tricopter_simulation_rtw_DWork;

  /* Real-Time Model */
  RT_MODEL_tricopter_simulation_r tricopter_simulation_rtw_M_;

  /* Real-Time Model pointer */
  RT_MODEL_tricopter_simulation_r *tricopter_simulation_rtw_M;
};

/*-
 * The generated code includes comments that allow you to trace directly
 * back to the appropriate location in the model.  The basic format
 * is <system>/block_name, where system is the system number (uniquely
 * assigned by Simulink) and block_name is the name of the block.
 *
 * Use the MATLAB hilite_system command to trace the generated code back
 * to the model.  For example,
 *
 * hilite_system('<S3>')    - opens system 3
 * hilite_system('<S3>/Kp') - opens and selects block Kp which resides in S3
 *
 * Here is the system hierarchy for this model
 *
 * '<Root>' : 'tricopter_simulation_rtw'
 * '<S1>'   : 'tricopter_simulation_rtw/EQM'
 * '<S2>'   : 'tricopter_simulation_rtw/KK-Controller'
 * '<S3>'   : 'tricopter_simulation_rtw/Tricopter'
 * '<S4>'   : 'tricopter_simulation_rtw/EQM/Variable Mass 6DoF (Quaternion)'
 * '<S5>'   : 'tricopter_simulation_rtw/EQM/vec2bus'
 * '<S6>'   : 'tricopter_simulation_rtw/EQM/vec2bus1'
 * '<S7>'   : 'tricopter_simulation_rtw/EQM/vec2bus2'
 * '<S8>'   : 'tricopter_simulation_rtw/EQM/vec2bus3'
 * '<S9>'   : 'tricopter_simulation_rtw/EQM/vec2bus4'
 * '<S10>'  : 'tricopter_simulation_rtw/EQM/vec2bus5'
 * '<S11>'  : 'tricopter_simulation_rtw/EQM/vec2bus6'
 * '<S12>'  : 'tricopter_simulation_rtw/EQM/Variable Mass 6DoF (Quaternion)/Calulate omega_dot'
 * '<S13>'  : 'tricopter_simulation_rtw/EQM/Variable Mass 6DoF (Quaternion)/Cross'
 * '<S14>'  : 'tricopter_simulation_rtw/EQM/Variable Mass 6DoF (Quaternion)/Detremine Force,  Mass & Inertia'
 * '<S15>'  : 'tricopter_simulation_rtw/EQM/Variable Mass 6DoF (Quaternion)/Subsystem'
 * '<S16>'  : 'tricopter_simulation_rtw/EQM/Variable Mass 6DoF (Quaternion)/Calulate omega_dot/Cross'
 * '<S17>'  : 'tricopter_simulation_rtw/EQM/Variable Mass 6DoF (Quaternion)/Calulate omega_dot/Subsystem'
 * '<S18>'  : 'tricopter_simulation_rtw/EQM/Variable Mass 6DoF (Quaternion)/Calulate omega_dot/Subsystem1'
 * '<S19>'  : 'tricopter_simulation_rtw/EQM/Variable Mass 6DoF (Quaternion)/Subsystem/Body Rates Integration'
 * '<S20>'  : 'tricopter_simulation_rtw/EQM/Variable Mass 6DoF (Quaternion)/Subsystem/Quadternions to Direct Cosine Matrix'
 * '<S21>'  : 'tricopter_simulation_rtw/EQM/Variable Mass 6DoF (Quaternion)/Subsystem/Quaternions to Euler Angles '
 * '<S22>'  : 'tricopter_simulation_rtw/EQM/Variable Mass 6DoF (Quaternion)/Subsystem/euler2quad'
 * '<S23>'  : 'tricopter_simulation_rtw/EQM/Variable Mass 6DoF (Quaternion)/Subsystem/Body Rates Integration/Rate Integration'
 * '<S24>'  : 'tricopter_simulation_rtw/EQM/Variable Mass 6DoF (Quaternion)/Subsystem/Body Rates Integration/Unit Delay Resettable External IC'
 * '<S25>'  : 'tricopter_simulation_rtw/EQM/Variable Mass 6DoF (Quaternion)/Subsystem/Body Rates Integration/Rate Integration/Magnitude 3d'
 * '<S26>'  : 'tricopter_simulation_rtw/EQM/Variable Mass 6DoF (Quaternion)/Subsystem/Body Rates Integration/Rate Integration/Normalization'
 * '<S27>'  : 'tricopter_simulation_rtw/EQM/Variable Mass 6DoF (Quaternion)/Subsystem/Body Rates Integration/Rate Integration/Quaternion Product'
 * '<S28>'  : 'tricopter_simulation_rtw/EQM/Variable Mass 6DoF (Quaternion)/Subsystem/Body Rates Integration/Rate Integration/Normalization/Vector norm'
 * '<S29>'  : 'tricopter_simulation_rtw/EQM/Variable Mass 6DoF (Quaternion)/Subsystem/Quaternions to Euler Angles /Modulo 2pi [-pi;pi]'
 * '<S30>'  : 'tricopter_simulation_rtw/EQM/Variable Mass 6DoF (Quaternion)/Subsystem/Quaternions to Euler Angles /Modulo 2pi [-pi;pi]1'
 * '<S31>'  : 'tricopter_simulation_rtw/EQM/Variable Mass 6DoF (Quaternion)/Subsystem/Quaternions to Euler Angles /Modulo 2pi [-pi;pi]2'
 * '<S32>'  : 'tricopter_simulation_rtw/KK-Controller/MATLAB Function'
 * '<S33>'  : 'tricopter_simulation_rtw/Tricopter/Gravitation'
 * '<S34>'  : 'tricopter_simulation_rtw/Tricopter/Luftwiderstand'
 * '<S35>'  : 'tricopter_simulation_rtw/Tricopter/Luftwiderstand (Drehung)'
 * '<S36>'  : 'tricopter_simulation_rtw/Tricopter/Motordynamics M1'
 * '<S37>'  : 'tricopter_simulation_rtw/Tricopter/Motordynamics M2'
 * '<S38>'  : 'tricopter_simulation_rtw/Tricopter/Motordynamics M3'
 * '<S39>'  : 'tricopter_simulation_rtw/Tricopter/Propeller M1'
 * '<S40>'  : 'tricopter_simulation_rtw/Tricopter/Propeller M2'
 * '<S41>'  : 'tricopter_simulation_rtw/Tricopter/Propeller M3'
 * '<S42>'  : 'tricopter_simulation_rtw/Tricopter/X-Moment'
 * '<S43>'  : 'tricopter_simulation_rtw/Tricopter/Y-Force'
 * '<S44>'  : 'tricopter_simulation_rtw/Tricopter/Y-Moment'
 * '<S45>'  : 'tricopter_simulation_rtw/Tricopter/Yaw-Servodynamics'
 * '<S46>'  : 'tricopter_simulation_rtw/Tricopter/Z-Force'
 * '<S47>'  : 'tricopter_simulation_rtw/Tricopter/Z-Moment'
 * '<S48>'  : 'tricopter_simulation_rtw/Tricopter/Motordynamics M1/Nonlinear Actuator (Second Order, discrete)'
 * '<S49>'  : 'tricopter_simulation_rtw/Tricopter/Motordynamics M2/Nonlinear Actuator (Second Order, discrete)'
 * '<S50>'  : 'tricopter_simulation_rtw/Tricopter/Motordynamics M3/Nonlinear Actuator (Second Order, discrete)'
 * '<S51>'  : 'tricopter_simulation_rtw/Tricopter/Yaw-Servodynamics/Nonlinear Actuator (Second Order, discrete)'
 */
#endif                                 /* RTW_HEADER_tricopter_simulation_rtw_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
