/*
 * File: tricopter_simulation_rtw_types.h
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

#ifndef RTW_HEADER_tricopter_simulation_rtw_types_h_
#define RTW_HEADER_tricopter_simulation_rtw_types_h_
#include "rtwtypes.h"
#ifndef _DEFINED_TYPEDEF_FOR_sChannelsKK_
#define _DEFINED_TYPEDEF_FOR_sChannelsKK_

typedef struct {
  real_T lat_us;
  real_T lon_us;
  real_T ped_us;
  real_T col_us;
} sChannelsKK;

#endif

#ifndef _DEFINED_TYPEDEF_FOR_arNAcc_T_
#define _DEFINED_TYPEDEF_FOR_arNAcc_T_

typedef struct {
  real_T ax;
  real_T ay;
  real_T az;
} arNAcc_T;

#endif

#ifndef _DEFINED_TYPEDEF_FOR_arNRate_T_
#define _DEFINED_TYPEDEF_FOR_arNRate_T_

typedef struct {
  real_T p;
  real_T q;
  real_T r;
} arNRate_T;

#endif

#ifndef _DEFINED_TYPEDEF_FOR_arNQuat_T_
#define _DEFINED_TYPEDEF_FOR_arNQuat_T_

typedef struct {
  real_T q0;
  real_T q1;
  real_T q2;
  real_T q3;
} arNQuat_T;

#endif

#ifndef _DEFINED_TYPEDEF_FOR_argNPos_T_
#define _DEFINED_TYPEDEF_FOR_argNPos_T_

typedef struct {
  real_T x;
  real_T y;
  real_T z;
} argNPos_T;

#endif

#ifndef _DEFINED_TYPEDEF_FOR_arNRateDot_T_
#define _DEFINED_TYPEDEF_FOR_arNRateDot_T_

typedef struct {
  real_T pdot;
  real_T qdot;
  real_T rdot;
} arNRateDot_T;

#endif

#ifndef _DEFINED_TYPEDEF_FOR_argNVel_T_
#define _DEFINED_TYPEDEF_FOR_argNVel_T_

typedef struct {
  real_T ug;
  real_T vg;
  real_T wg;
} argNVel_T;

#endif

#ifndef _DEFINED_TYPEDEF_FOR_arNVel_T_
#define _DEFINED_TYPEDEF_FOR_arNVel_T_

typedef struct {
  real_T ub;
  real_T vb;
  real_T wb;
} arNVel_T;

#endif

#ifndef _DEFINED_TYPEDEF_FOR_sMotion_
#define _DEFINED_TYPEDEF_FOR_sMotion_

typedef struct {
  arNAcc_T acc;
  arNRate_T rate;
  arNQuat_T quaternion;
  argNPos_T pos;
  arNRateDot_T rate_dot;
  argNVel_T vel_geo;
  arNVel_T vel_body;
} sMotion;

#endif

/* Forward declaration for rtModel */
typedef struct RT_MODEL_tricopter_simulation_r RT_MODEL_tricopter_simulation_r;

#endif                                 /* RTW_HEADER_tricopter_simulation_rtw_types_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
