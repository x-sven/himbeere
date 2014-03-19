/*
 * File: tricopter_simulation_rtw.h
 *
 * Code generated for Simulink model 'tricopter_simulation_rtw'.
 *
 * Model version                  : 1.241
 * Simulink Coder version         : 8.1 (R2011b) 08-Jul-2011
 * TLC version                    : 8.1 (Jul  9 2011)
 * C/C++ source code generated on : Sun Mar 02 13:59:27 2014
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
#include "rtwtypes.h"
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
  real_T DLookupTable;                 /* '<S37>/1-D Lookup Table' */
  real_T DLookupTable_f;               /* '<S38>/1-D Lookup Table' */
  real_T DLookupTable_e;               /* '<S39>/1-D Lookup Table' */
} BlockIO_tricopter_simulation_rt;

/* Block states (auto storage) for system '<Root>' */
typedef struct {
  real_T Delay_DSTATE;                 /* '<S39>/Delay' */
  real_T UnitDelay_DSTATE;             /* '<S51>/Unit Delay' */
  real_T UnitDelay1_DSTATE;            /* '<S51>/Unit Delay1' */
  real_T DiscreteTimeIntegrator1_DSTATE;/* '<S51>/Discrete-Time Integrator1' */
  real_T DiscreteTimeIntegrator_DSTATE;/* '<S51>/Discrete-Time Integrator' */
  real_T Rates_DSTATE[3];              /* '<S4>/Rates' */
  real_T Delay_DSTATE_d;               /* '<S37>/Delay' */
  real_T UnitDelay_DSTATE_f;           /* '<S49>/Unit Delay' */
  real_T UnitDelay1_DSTATE_p;          /* '<S49>/Unit Delay1' */
  real_T DiscreteTimeIntegrator1_DSTAT_m;/* '<S49>/Discrete-Time Integrator1' */
  real_T DiscreteTimeIntegrator_DSTATE_e;/* '<S49>/Discrete-Time Integrator' */
  real_T Delay_DSTATE_l;               /* '<S38>/Delay' */
  real_T UnitDelay_DSTATE_i;           /* '<S50>/Unit Delay' */
  real_T UnitDelay1_DSTATE_l;          /* '<S50>/Unit Delay1' */
  real_T DiscreteTimeIntegrator1_DSTAT_n;/* '<S50>/Discrete-Time Integrator1' */
  real_T DiscreteTimeIntegrator_DSTATE_a;/* '<S50>/Discrete-Time Integrator' */
  real_T FixPtUnitDelay1_DSTATE[4];    /* '<S25>/FixPt Unit Delay1' */
  real_T Velocity_DSTATE[3];           /* '<S4>/Velocity' */
  real_T Position_DSTATE[3];           /* '<S4>/Position' */
  real_T Memory1_PreviousInput[3];     /* '<S2>/Memory1' */
  real_T Memory_PreviousInput[3];      /* '<S2>/Memory' */
  uint8_T FixPtUnitDelay2_DSTATE;      /* '<S25>/FixPt Unit Delay2' */
  uint8_T DiscreteTimeIntegrator1_SYSTEM_;/* '<S51>/Discrete-Time Integrator1' */
  uint8_T DiscreteTimeIntegrator_SYSTEM_E;/* '<S51>/Discrete-Time Integrator' */
  uint8_T DiscreteTimeIntegrator1_SYSTE_b;/* '<S49>/Discrete-Time Integrator1' */
  uint8_T DiscreteTimeIntegrator_SYSTEM_c;/* '<S49>/Discrete-Time Integrator' */
  uint8_T DiscreteTimeIntegrator1_SYSTE_d;/* '<S50>/Discrete-Time Integrator1' */
  uint8_T DiscreteTimeIntegrator_SYSTEM_o;/* '<S50>/Discrete-Time Integrator' */
} D_Work_tricopter_simulation_rtw;

/* Invariant block signals (auto storage) */
typedef const struct tag_ConstBlockIO_tricopter_simu {
  real_T u[3];                         /* '<S23>/1//2' */
  real_T cosphi2;                      /* '<S23>/cos(phi//2)' */
  real_T costheta2;                    /* '<S23>/cos(theta//2)' */
  real_T cospsi2;                      /* '<S23>/cos(psi//2)' */
  real_T CCC;                          /* '<S23>/CCC' */
  real_T sinphi2;                      /* '<S23>/sin(phi//2)' */
  real_T sintheta2;                    /* '<S23>/sin(theta//2)' */
  real_T sinpsi2;                      /* '<S23>/sin(psi//2)' */
  real_T SSS;                          /* '<S23>/SSS' */
  real_T Sum9;                         /* '<S23>/Sum9' */
  real_T SCC;                          /* '<S23>/SCC' */
  real_T CSS;                          /* '<S23>/CSS' */
  real_T Sum2;                         /* '<S23>/Sum2' */
  real_T CSC;                          /* '<S23>/CSC' */
  real_T SCS;                          /* '<S23>/SCS' */
  real_T Sum1;                         /* '<S23>/Sum1' */
  real_T CCS;                          /* '<S23>/CCS' */
  real_T SSC;                          /* '<S23>/SSC' */
  real_T Sum3;                         /* '<S23>/Sum3' */
  real_T MatrixConcatenation[18];      /* '<S15>/Matrix Concatenation' */
  real_T Selector[9];                  /* '<S13>/Selector' */
  real_T Gain;                         /* '<S43>/Gain' */
  real_T Sqrt;                         /* '<S43>/Sqrt' */
  real_T Gain_a;                       /* '<S45>/Gain' */
  real_T Gain1;                        /* '<S45>/Gain1' */
  real_T Sqrt_n;                       /* '<S45>/Sqrt' */
  real_T Selector1[9];                 /* '<S13>/Selector1' */
  real_T Selector2[9];                 /* '<S13>/Selector2' */
} ConstBlockIO_tricopter_simulati;

/* Constant parameters (auto storage) */
typedef struct {
  /* Expression: [-60 60]*pi/180
   * Referenced by: '<S46>/1-D Lookup Table'
   */
  real_T DLookupTable_tableDat[2];

  /* Pooled Parameter (Expression: [900 2100])
   * Referenced by:
   *   '<S37>/1-D Lookup Table'
   *   '<S38>/1-D Lookup Table'
   *   '<S39>/1-D Lookup Table'
   *   '<S46>/1-D Lookup Table'
   */
  real_T pooled12[2];

  /* Pooled Parameter (Expression: [0 7000]*pi/30)
   * Referenced by:
   *   '<S37>/1-D Lookup Table'
   *   '<S38>/1-D Lookup Table'
   *   '<S39>/1-D Lookup Table'
   */
  real_T pooled17[2];
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
    void* dataAddress[31];
    int32_T* vardimsAddress[31];
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
 * '<S12>'  : 'tricopter_simulation_rtw/EQM/vec2bus7'
 * '<S13>'  : 'tricopter_simulation_rtw/EQM/Variable Mass 6DoF (Quaternion)/Calulate omega_dot'
 * '<S14>'  : 'tricopter_simulation_rtw/EQM/Variable Mass 6DoF (Quaternion)/Cross'
 * '<S15>'  : 'tricopter_simulation_rtw/EQM/Variable Mass 6DoF (Quaternion)/Detremine Force,  Mass & Inertia'
 * '<S16>'  : 'tricopter_simulation_rtw/EQM/Variable Mass 6DoF (Quaternion)/Subsystem'
 * '<S17>'  : 'tricopter_simulation_rtw/EQM/Variable Mass 6DoF (Quaternion)/Calulate omega_dot/Cross'
 * '<S18>'  : 'tricopter_simulation_rtw/EQM/Variable Mass 6DoF (Quaternion)/Calulate omega_dot/Subsystem'
 * '<S19>'  : 'tricopter_simulation_rtw/EQM/Variable Mass 6DoF (Quaternion)/Calulate omega_dot/Subsystem1'
 * '<S20>'  : 'tricopter_simulation_rtw/EQM/Variable Mass 6DoF (Quaternion)/Subsystem/Body Rates Integration'
 * '<S21>'  : 'tricopter_simulation_rtw/EQM/Variable Mass 6DoF (Quaternion)/Subsystem/Quadternions to Direct Cosine Matrix'
 * '<S22>'  : 'tricopter_simulation_rtw/EQM/Variable Mass 6DoF (Quaternion)/Subsystem/Quaternions to Euler Angles '
 * '<S23>'  : 'tricopter_simulation_rtw/EQM/Variable Mass 6DoF (Quaternion)/Subsystem/euler2quad'
 * '<S24>'  : 'tricopter_simulation_rtw/EQM/Variable Mass 6DoF (Quaternion)/Subsystem/Body Rates Integration/Rate Integration'
 * '<S25>'  : 'tricopter_simulation_rtw/EQM/Variable Mass 6DoF (Quaternion)/Subsystem/Body Rates Integration/Unit Delay Resettable External IC'
 * '<S26>'  : 'tricopter_simulation_rtw/EQM/Variable Mass 6DoF (Quaternion)/Subsystem/Body Rates Integration/Rate Integration/Magnitude 3d'
 * '<S27>'  : 'tricopter_simulation_rtw/EQM/Variable Mass 6DoF (Quaternion)/Subsystem/Body Rates Integration/Rate Integration/Normalization'
 * '<S28>'  : 'tricopter_simulation_rtw/EQM/Variable Mass 6DoF (Quaternion)/Subsystem/Body Rates Integration/Rate Integration/Quaternion Product'
 * '<S29>'  : 'tricopter_simulation_rtw/EQM/Variable Mass 6DoF (Quaternion)/Subsystem/Body Rates Integration/Rate Integration/Normalization/Vector norm'
 * '<S30>'  : 'tricopter_simulation_rtw/EQM/Variable Mass 6DoF (Quaternion)/Subsystem/Quaternions to Euler Angles /Modulo 2pi [-pi;pi]'
 * '<S31>'  : 'tricopter_simulation_rtw/EQM/Variable Mass 6DoF (Quaternion)/Subsystem/Quaternions to Euler Angles /Modulo 2pi [-pi;pi]1'
 * '<S32>'  : 'tricopter_simulation_rtw/EQM/Variable Mass 6DoF (Quaternion)/Subsystem/Quaternions to Euler Angles /Modulo 2pi [-pi;pi]2'
 * '<S33>'  : 'tricopter_simulation_rtw/KK-Controller/MATLAB Function'
 * '<S34>'  : 'tricopter_simulation_rtw/Tricopter/Gravitation'
 * '<S35>'  : 'tricopter_simulation_rtw/Tricopter/Luftwiderstand'
 * '<S36>'  : 'tricopter_simulation_rtw/Tricopter/Luftwiderstand (Drehung)'
 * '<S37>'  : 'tricopter_simulation_rtw/Tricopter/Motordynamics 1'
 * '<S38>'  : 'tricopter_simulation_rtw/Tricopter/Motordynamics 2'
 * '<S39>'  : 'tricopter_simulation_rtw/Tricopter/Motordynamics 3'
 * '<S40>'  : 'tricopter_simulation_rtw/Tricopter/Thrust 1'
 * '<S41>'  : 'tricopter_simulation_rtw/Tricopter/Thrust 2'
 * '<S42>'  : 'tricopter_simulation_rtw/Tricopter/Thrust 3'
 * '<S43>'  : 'tricopter_simulation_rtw/Tricopter/X-Moment'
 * '<S44>'  : 'tricopter_simulation_rtw/Tricopter/Y-Force'
 * '<S45>'  : 'tricopter_simulation_rtw/Tricopter/Y-Moment'
 * '<S46>'  : 'tricopter_simulation_rtw/Tricopter/Yaw-Servodynamics'
 * '<S47>'  : 'tricopter_simulation_rtw/Tricopter/Z-Force'
 * '<S48>'  : 'tricopter_simulation_rtw/Tricopter/Z-Moment'
 * '<S49>'  : 'tricopter_simulation_rtw/Tricopter/Motordynamics 1/Nonlinear Actuator (Second Order, discrete)'
 * '<S50>'  : 'tricopter_simulation_rtw/Tricopter/Motordynamics 2/Nonlinear Actuator (Second Order, discrete)'
 * '<S51>'  : 'tricopter_simulation_rtw/Tricopter/Motordynamics 3/Nonlinear Actuator (Second Order, discrete)'
 */
#endif                                 /* RTW_HEADER_tricopter_simulation_rtw_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
