/*
 * File: tricopter_simulation_rtw.cpp
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

#include "tricopter_simulation_rtw_capi.h"
#include "tricopter_simulation_rtw.h"
#include "tricopter_simulation_rtw_private.h"

const sChannelsKK tricopter_simulation_rtw_rtZsChannelsKK = {
  0.0,                                 /* lat_us */
  0.0,                                 /* lon_us */
  0.0,                                 /* ped_us */
  0.0                                  /* col_us */
} ;                                    /* sChannelsKK ground */

const sMotion tricopter_simulation_rtw_rtZsMotion = {
  {
    0.0,                               /* ax */
    0.0,                               /* ay */
    0.0                                /* az */
  },                                   /* acc */

  {
    0.0,                               /* p */
    0.0,                               /* q */
    0.0                                /* r */
  },                                   /* rate */

  {
    0.0,                               /* q0 */
    0.0,                               /* q1 */
    0.0,                               /* q2 */
    0.0                                /* q3 */
  },                                   /* quaternion */

  {
    0.0,                               /* x */
    0.0,                               /* y */
    0.0                                /* z */
  },                                   /* pos */

  {
    0.0,                               /* pdot */
    0.0,                               /* qdot */
    0.0                                /* rdot */
  },                                   /* rate_dot */

  {
    0.0,                               /* ug */
    0.0,                               /* vg */
    0.0                                /* wg */
  },                                   /* vel_geo */

  {
    0.0,                               /* ub */
    0.0,                               /* vb */
    0.0                                /* wb */
  }                                    /* vel_body */
} ;                                    /* sMotion ground */

real_T look1_binlxpw(real_T u0, const real_T bp0[], const real_T table[],
                     uint32_T maxIndex)
{
  real_T frac;
  uint32_T iRght;
  uint32_T iLeft;
  uint32_T bpIdx;

  /* Lookup 1-D
     Search method: 'binary'
     Use previous index: 'off'
     Interpolation method: 'Linear'
     Extrapolation method: 'Linear'
     Use last breakpoint for index at or above upper limit: 'off'
     Remove protection against out-of-range input in generated code: 'off'
   */
  /* Prelookup - Index and Fraction
     Index Search method: 'binary'
     Extrapolation method: 'Linear'
     Use previous index: 'off'
     Use last breakpoint for index at or above upper limit: 'off'
     Remove protection against out-of-range input in generated code: 'off'
   */
  if (u0 <= bp0[0U]) {
    iLeft = 0U;
    frac = (u0 - bp0[0U]) / (bp0[1U] - bp0[0U]);
  } else if (u0 < bp0[maxIndex]) {
    /* Binary Search */
    bpIdx = (uint32_T)((int32_T)maxIndex >> 1U);
    iLeft = 0U;
    iRght = maxIndex;
    while (iRght - iLeft > 1U) {
      if (u0 < bp0[bpIdx]) {
        iRght = bpIdx;
      } else {
        iLeft = bpIdx;
      }

      bpIdx = (uint32_T)((int32_T)(iRght + iLeft) >> 1U);
    }

    frac = (u0 - bp0[iLeft]) / (bp0[iLeft + 1U] - bp0[iLeft]);
  } else {
    iLeft = maxIndex - 1U;
    frac = (u0 - bp0[maxIndex - 1U]) / (bp0[maxIndex] - bp0[maxIndex - 1U]);
  }

  /* Interpolation 1-D
     Interpolation method: 'Linear'
     Use last breakpoint for index at or above upper limit: 'off'
     Overflow mode: 'portable wrapping'
   */
  return (table[iLeft + 1U] - table[iLeft]) * frac + table[iLeft];
}

void rt_mldivide_U1d3x3_U2d3_Yd3x1_snf(const real_T u0[9], const real_T u1[3],
  real_T y[3])
{
  real_T A[9];
  int32_T TWO;
  int32_T THREE;
  int32_T r;
  int32_T r_0;
  real_T maxval;
  real_T a;
  memcpy((void *)&A[0], (void *)&u0[0], 9U * sizeof(real_T));
  THREE = 2;
  r = 0;
  r_0 = 1;
  maxval = fabs(u0[0]);
  a = fabs(u0[1]);
  if (a > maxval) {
    maxval = a;
    r = 1;
    r_0 = 0;
  }

  if (fabs(u0[2]) > maxval) {
    r = 2;
    r_0 = 1;
    THREE = 0;
  }

  A[r_0] = u0[r_0] / u0[r];
  A[THREE] /= A[r];
  A[3 + r_0] -= A[3 + r] * A[r_0];
  A[3 + THREE] -= A[3 + r] * A[THREE];
  A[6 + r_0] -= A[6 + r] * A[r_0];
  A[6 + THREE] -= A[6 + r] * A[THREE];
  if (fabs(A[3 + THREE]) > fabs(A[3 + r_0])) {
    TWO = r_0 + 1;
    r_0 = THREE;
    THREE = TWO - 1;
  }

  A[3 + THREE] /= A[3 + r_0];
  A[6 + THREE] -= A[3 + THREE] * A[6 + r_0];
  maxval = u1[r_0] - u1[r] * A[r_0];
  a = ((u1[THREE] - u1[r] * A[THREE]) - A[3 + THREE] * maxval) / A[6 + THREE];
  maxval -= A[6 + r_0] * a;
  maxval /= A[3 + r_0];
  y[0] = ((u1[r] - A[6 + r] * a) - A[3 + r] * maxval) / A[r];
  y[1] = maxval;
  y[2] = a;
}

/* Model step function */
void tricopter_simulation_rtwModelClass::step()
{
  /* local block i/o variables */
  real_T rtb_Sum2;
  real_T rtb_Sum2_e;
  real_T rtb_Sum2_i;
  real_T MotorOut;
  real_T MotorOut_0;
  real_T MotorOut_1;
  real_T RxInRoll;
  real_T RxInCollective;
  real_T error;
  real_T imax;
  real_T rtb_Direction;
  real_T rtb_Saturation1;
  real_T rtb_Saturation2;
  real_T rtb_Saturation4;
  real_T rtb_DLookupTable;
  real_T rtb_Direction_d;
  real_T rtb_Direction_o;
  real_T rtb_delta2;
  real_T rtb_TrigonometricFunction1;
  real_T rtb_TrigonometricFunction_h;
  real_T rtb_Sqrt;
  real_T rtb_q_0;
  real_T rtb_q_1;
  real_T rtb_q_2;
  real_T rtb_q_3;
  real_T rtb_RbiMatrix[9];
  real_T rtb_exey;
  real_T rtb_eze0;
  real_T rtb_exez;
  real_T rtb_eye0;
  real_T rtb_eyez;
  real_T rtb_exe0;
  real_T rtb_Product_l[3];
  real_T rtb_Product2_m[3];
  real_T rtb_GetDCM[9];
  real_T du;
  real_T u;
  real_T rtb_Direction_o_0[3];
  int32_T i;
  real_T rtb_GetDCM_0[3];
  real_T tmp[3];
  real_T tmp_0[3];
  real_T rtb_Product2_m_0[3];
  real_T tmp_1[3];
  real_T rtb_Product2_m_1[3];
  real_T rtb_integral_out_idx;
  real_T rtb_last_error_out_idx;
  real_T rtb_integral_out_idx_0;
  real_T rtb_last_error_out_idx_0;
  real_T rtb_integral_out_idx_1;
  real_T rtb_last_error_out_idx_1;
  real_T rtb_Init_idx;
  real_T rtb_Init_idx_0;
  real_T rtb_Init_idx_1;
  real_T rtb_Init_idx_2;
  real_T rtb_Product_e_idx;
  real_T rtb_Product1_h_idx;
  real_T rtb_Product2_a_idx;
  real_T rtb_Product2_a_idx_0;
  real_T rtb_Product2_a_idx_1;
  real_T rtb_rad2deg_s_idx;
  real_T rtb_rad2deg_s_idx_0;
  real_T rtb_rad2deg_s_idx_1;

  /* Sum: '<S51>/Sum2' incorporates:
   *  Delay: '<S39>/Delay'
   *  Gain: '<S51>/2*zeta * wn'
   *  Gain: '<S51>/wn^2'
   *  Saturate: '<S51>/Demand limits'
   *  Sum: '<S51>/Sum3'
   *  UnitDelay: '<S51>/Unit Delay'
   *  UnitDelay: '<S51>/Unit Delay1'
   */
  rtb_Sum2 = ((tricopter_simulation_rtw_DWork.Delay_DSTATE >= 733.03828583761833
               ? 733.03828583761833 :
               tricopter_simulation_rtw_DWork.Delay_DSTATE <= 0.0 ? 0.0 :
               tricopter_simulation_rtw_DWork.Delay_DSTATE) -
              tricopter_simulation_rtw_DWork.UnitDelay_DSTATE) * 289.0 - 32.3 *
    tricopter_simulation_rtw_DWork.UnitDelay1_DSTATE;

  /* DiscreteIntegrator: '<S51>/Discrete-Time Integrator1' */
  if (tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator1_SYSTEM_ != 0) {
    du = tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator1_DSTATE;
  } else {
    du = 0.005 * rtb_Sum2 +
      tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator1_DSTATE;
  }

  /* End of DiscreteIntegrator: '<S51>/Discrete-Time Integrator1' */

  /* DiscreteIntegrator: '<S51>/Discrete-Time Integrator' */
  if (tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator_SYSTEM_E != 0) {
    u = tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator_DSTATE;
  } else {
    u = 0.005 * du +
      tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator_DSTATE;
  }

  if (u >= 733.03828583761833) {
    u = 733.03828583761833;
  } else {
    if (u <= 0.0) {
      u = 0.0;
    }
  }

  /* End of DiscreteIntegrator: '<S51>/Discrete-Time Integrator' */

  /* Gain: '<S42>/Direction' incorporates:
   *  Gain: '<S42>/k'
   *  Product: '<S42>/omega^2'
   *  Sum: '<S51>/Sum'
   */
  rtb_Direction = -(u * u * 1.92E-5);

  /* Saturate: '<S2>/Saturation1' incorporates:
   *  BusSelector: '<Root>/BusConversion_InsertedFor_deflections_at_outport_0'
   *  Inport: '<Root>/deflections'
   */
  rtb_Saturation1 = tricopter_simulation_rtw_U.deflections.lat_us >= 2000.0 ?
    2000.0 : tricopter_simulation_rtw_U.deflections.lat_us <= 1000.0 ? 1000.0 :
    tricopter_simulation_rtw_U.deflections.lat_us;

  /* Saturate: '<S2>/Saturation2' incorporates:
   *  BusSelector: '<Root>/BusConversion_InsertedFor_deflections_at_outport_0'
   *  Inport: '<Root>/deflections'
   */
  rtb_Saturation2 = tricopter_simulation_rtw_U.deflections.lon_us >= 2000.0 ?
    2000.0 : tricopter_simulation_rtw_U.deflections.lon_us <= 1000.0 ? 1000.0 :
    tricopter_simulation_rtw_U.deflections.lon_us;

  /* Saturate: '<S2>/Saturation4' incorporates:
   *  BusSelector: '<Root>/BusConversion_InsertedFor_deflections_at_outport_0'
   *  Inport: '<Root>/deflections'
   */
  rtb_Saturation4 = tricopter_simulation_rtw_U.deflections.ped_us >= 2000.0 ?
    2000.0 : tricopter_simulation_rtw_U.deflections.ped_us <= 1000.0 ? 1000.0 :
    tricopter_simulation_rtw_U.deflections.ped_us;

  /* Gain: '<S2>/rad2deg_s' incorporates:
   *  DiscreteIntegrator: '<S4>/Rates'
   */
  rtb_rad2deg_s_idx = 57.295779513082323 *
    tricopter_simulation_rtw_DWork.Rates_DSTATE[0];
  rtb_rad2deg_s_idx_0 = 57.295779513082323 *
    tricopter_simulation_rtw_DWork.Rates_DSTATE[1];
  rtb_rad2deg_s_idx_1 = 57.295779513082323 *
    tricopter_simulation_rtw_DWork.Rates_DSTATE[2];

  /* MATLAB Function: '<S2>/MATLAB Function' incorporates:
   *  BusSelector: '<Root>/BusConversion_InsertedFor_deflections_at_outport_0'
   *  Constant: '<S2>/Gain-Poti'
   *  Constant: '<S2>/Gain-Poti1'
   *  Constant: '<S2>/Gain-Poti2'
   *  Gain: '<S2>/+//- 300 -> 16bit'
   *  Inport: '<Root>/deflections'
   *  Memory: '<S2>/Memory1'
   *  Saturate: '<S2>/Saturation'
   *  Saturate: '<S2>/Saturation3'
   */
  /* MATLAB Function 'KK-Controller/MATLAB Function': '<S33>:1' */
  /* https://github.com/sim-/kk/blob/master/kk.c */
  /*  roll index definition */
  /*  pitch index definition */
  /*  yaw index definition */
  /* '<S33>:1:8' */
  /* '<S33>:1:11' */
  /* '<S33>:1:12' */
  /* '<S33>:1:14' */
  /* '<S33>:1:15' */
  /* '<S33>:1:16' */
  /* '<S33>:1:17' */
  /* ------------------------------------------------------- */
  /* --- Start mixing by setting collective to motor outputs */
  /* ------------------------------------------------------- */
  /* '<S33>:1:22' */
  RxInCollective = ((tricopter_simulation_rtw_U.deflections.col_us >= 2000.0 ?
                     2000.0 : tricopter_simulation_rtw_U.deflections.col_us <=
                     1000.0 ? 1000.0 :
                     tricopter_simulation_rtw_U.deflections.col_us) - 1100.0) *
    10.0 / 8.0;

  /*  0-800 -> 0-1000 */
  /* '<S33>:1:26' */
  /* ------------------------------------------------------- */
  /*  Roll control loop */
  /* ------------------------------------------------------- */
  /* '<S33>:1:31' */
  error = ((rtb_Saturation1 - 1500.0) - (rtb_rad2deg_s_idx >= 300.0 ? 300.0 :
            rtb_rad2deg_s_idx <= -300.0 ? -300.0 : rtb_rad2deg_s_idx) *
           109.22666666666667) * 0.005;

  /* '<S33>:1:38' */
  rtb_integral_out_idx = tricopter_simulation_rtw_DWork.Memory1_PreviousInput[0]
    + error;

  /* '<S33>:1:40' */
  imax = RxInCollective;
  if (RxInCollective < 0.0) {
    /* '<S33>:1:41' */
    /* '<S33>:1:42' */
    imax = 0.0;
  }

  /* '<S33>:1:45' */
  imax /= 10.0;

  /*  1000 -> 200 */
  if (rtb_integral_out_idx > imax) {
    /* '<S33>:1:46' */
    /* '<S33>:1:47' */
    rtb_integral_out_idx = imax;
  } else {
    if (rtb_integral_out_idx < -imax) {
      /* '<S33>:1:48' */
      /* '<S33>:1:49' */
      rtb_integral_out_idx = -imax;
    }
  }

  /* '<S33>:1:53' */
  rtb_last_error_out_idx = error;

  /* '<S33>:1:54' */
  /*  + (integral(ROLL) / 4) + (derivative / 4); */
  /*  [Tricopter] */
  /* '<S33>:1:57' */
  RxInRoll = ((rtb_Saturation1 - 1500.0) + error) * 0.875;

  /*  (RxInRoll * 7) >> 3 = RxInRoll*0.875 Approximation of sin(60)=(.875 versus .86602540) without div */
  /* '<S33>:1:59' */
  /* '<S33>:1:60' */
  /* ------------------------------------------------------- */
  /*  Pitch control loop */
  /* ------------------------------------------------------- */
  /* '<S33>:1:65' */
  error = ((rtb_Saturation2 - 1500.0) - (rtb_rad2deg_s_idx_0 >= 300.0 ? 300.0 :
            rtb_rad2deg_s_idx_0 <= -300.0 ? -300.0 : rtb_rad2deg_s_idx_0) *
           109.22666666666667) * 0.005;

  /* '<S33>:1:72' */
  rtb_integral_out_idx_0 = tricopter_simulation_rtw_DWork.Memory1_PreviousInput
    [1] + error;
  if (rtb_integral_out_idx_0 > imax) {
    /* '<S33>:1:74' */
    /* '<S33>:1:75' */
    rtb_integral_out_idx_0 = imax;
  } else {
    if (rtb_integral_out_idx_0 < -imax) {
      /* '<S33>:1:76' */
      /* '<S33>:1:77' */
      rtb_integral_out_idx_0 = -imax;
    }
  }

  /* '<S33>:1:81' */
  rtb_last_error_out_idx_0 = error;

  /* '<S33>:1:82' */
  error += rtb_Saturation2 - 1500.0;

  /*  + (integral(PITCH) / 4) + (derivative / 4); */
  /*  [Tricopter] */
  /* '<S33>:1:85' */
  MotorOut_1 = RxInCollective - error;

  /* '<S33>:1:86' */
  error /= 2.0;

  /*  cosine of 60 */
  /* '<S33>:1:87' */
  MotorOut = (RxInCollective + RxInRoll) + error;

  /* '<S33>:1:88' */
  MotorOut_0 = (RxInCollective - RxInRoll) + error;

  /* ------------------------------------------------------- */
  /*  Yaw control loop */
  /* ------------------------------------------------------- */
  /* '<S33>:1:93' */
  error = ((rtb_Saturation4 - 1500.0) - (rtb_rad2deg_s_idx_1 >= 300.0 ? 300.0 :
            rtb_rad2deg_s_idx_1 <= -300.0 ? -300.0 : rtb_rad2deg_s_idx_1) *
           109.22666666666667) * 0.005;

  /* '<S33>:1:100' */
  rtb_integral_out_idx_1 = tricopter_simulation_rtw_DWork.Memory1_PreviousInput
    [2] + error;
  if (rtb_integral_out_idx_1 > imax) {
    /* '<S33>:1:102' */
    /* '<S33>:1:103' */
    rtb_integral_out_idx_1 = imax;
  } else {
    if (rtb_integral_out_idx_1 < -imax) {
      /* '<S33>:1:104' */
      /* '<S33>:1:105' */
      rtb_integral_out_idx_1 = -imax;
    }
  }

  /* '<S33>:1:109' */
  rtb_last_error_out_idx_1 = error;

  /* '<S33>:1:110' */
  /*  + (integral(YAW) / 4) + (derivative / 4); */
  /*  [Tricopter] */
  /* '<S33>:1:113' */
  /*  [Tricopter] */
  /*  Rather than clipping the motor outputs and causing instability */
  /*  at throttle saturation, we pull down the throttle of the other */
  /*  motors. This gives priority to stabilization without a fixed */
  /*  collective limit. */
  /* '<S33>:1:122' */
  imax = MotorOut;
  if (MotorOut_0 > MotorOut) {
    /* '<S33>:1:123' */
    /* '<S33>:1:124' */
    imax = MotorOut_0;
  }

  if (MotorOut_1 > imax) {
    /* '<S33>:1:126' */
    /* '<S33>:1:127' */
    imax = MotorOut_1;
  }

  /* '<S33>:1:129' */
  imax -= 1000.0;
  if (imax > 0.0) {
    /* '<S33>:1:130' */
    /* '<S33>:1:131' */
    MotorOut -= imax;

    /* '<S33>:1:132' */
    MotorOut_0 -= imax;

    /* '<S33>:1:133' */
    MotorOut_1 -= imax;
  }

  /* --- Limit the lowest value to avoid stopping of motor if motor value is under-saturated --- */
  /* '<S33>:1:137' */
  /*  this is the motor idle level */
  if (MotorOut < 114.0) {
    /* '<S33>:1:138' */
    /* '<S33>:1:139' */
    MotorOut = 114.0;
  }

  if (MotorOut_0 < 114.0) {
    /* '<S33>:1:141' */
    /* '<S33>:1:142' */
    MotorOut_0 = 114.0;
  }

  /* #if defined(TRI_COPTER) || defined(QUAD_COPTER) || defined(QUAD_X_COPTER) || defined(Y4_COPTER) */
  if (MotorOut_1 < 114.0) {
    /* '<S33>:1:146' */
    /* '<S33>:1:147' */
    MotorOut_1 = 114.0;
  }

  /*  Bound pulse length to 1ms <= pulse <= 2ms */
  /* '<S33>:1:151' */
  MotorOut += 1000.0;

  /* '<S33>:1:152' */
  MotorOut_0 += 1000.0;

  /* '<S33>:1:153' */
  MotorOut_1 += 1000.0;

  /* Lookup_n-D: '<S46>/1-D Lookup Table' incorporates:
   *  MATLAB Function: '<S2>/MATLAB Function'
   */
  /* '<S33>:1:154' */
  /* '<S33>:1:156' */
  /* '<S33>:1:157' */
  rtb_DLookupTable = look1_binlxpw((((rtb_Saturation4 - 1500.0) + error) + 500.0)
    + 1000.0, tricopter_simulation_rtw_ConstP.pooled12,
    tricopter_simulation_rtw_ConstP.DLookupTable_tableDat, 1U);

  /* Sum: '<S49>/Sum2' incorporates:
   *  Delay: '<S37>/Delay'
   *  Gain: '<S49>/2*zeta * wn'
   *  Gain: '<S49>/wn^2'
   *  Saturate: '<S49>/Demand limits'
   *  Sum: '<S49>/Sum3'
   *  UnitDelay: '<S49>/Unit Delay'
   *  UnitDelay: '<S49>/Unit Delay1'
   */
  rtb_Sum2_e = ((tricopter_simulation_rtw_DWork.Delay_DSTATE_d >=
                 733.03828583761833 ? 733.03828583761833 :
                 tricopter_simulation_rtw_DWork.Delay_DSTATE_d <= 0.0 ? 0.0 :
                 tricopter_simulation_rtw_DWork.Delay_DSTATE_d) -
                tricopter_simulation_rtw_DWork.UnitDelay_DSTATE_f) * 289.0 -
    32.3 * tricopter_simulation_rtw_DWork.UnitDelay1_DSTATE_p;

  /* DiscreteIntegrator: '<S49>/Discrete-Time Integrator1' */
  if (tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator1_SYSTE_b != 0) {
    RxInCollective =
      tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator1_DSTAT_m;
  } else {
    RxInCollective = 0.005 * rtb_Sum2_e +
      tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator1_DSTAT_m;
  }

  /* End of DiscreteIntegrator: '<S49>/Discrete-Time Integrator1' */

  /* DiscreteIntegrator: '<S49>/Discrete-Time Integrator' */
  if (tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator_SYSTEM_c != 0) {
    error = tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator_DSTATE_e;
  } else {
    error = 0.005 * RxInCollective +
      tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator_DSTATE_e;
  }

  if (error >= 733.03828583761833) {
    error = 733.03828583761833;
  } else {
    if (error <= 0.0) {
      error = 0.0;
    }
  }

  /* End of DiscreteIntegrator: '<S49>/Discrete-Time Integrator' */

  /* Gain: '<S40>/Direction' incorporates:
   *  Gain: '<S40>/k'
   *  Product: '<S40>/omega^2'
   *  Sum: '<S49>/Sum'
   */
  rtb_Direction_d = -(error * error * 1.92E-5);

  /* Sum: '<S50>/Sum2' incorporates:
   *  Delay: '<S38>/Delay'
   *  Gain: '<S50>/2*zeta * wn'
   *  Gain: '<S50>/wn^2'
   *  Saturate: '<S50>/Demand limits'
   *  Sum: '<S50>/Sum3'
   *  UnitDelay: '<S50>/Unit Delay'
   *  UnitDelay: '<S50>/Unit Delay1'
   */
  rtb_Sum2_i = ((tricopter_simulation_rtw_DWork.Delay_DSTATE_l >=
                 733.03828583761833 ? 733.03828583761833 :
                 tricopter_simulation_rtw_DWork.Delay_DSTATE_l <= 0.0 ? 0.0 :
                 tricopter_simulation_rtw_DWork.Delay_DSTATE_l) -
                tricopter_simulation_rtw_DWork.UnitDelay_DSTATE_i) * 289.0 -
    32.3 * tricopter_simulation_rtw_DWork.UnitDelay1_DSTATE_l;

  /* DiscreteIntegrator: '<S50>/Discrete-Time Integrator1' */
  if (tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator1_SYSTE_d != 0) {
    RxInRoll = tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator1_DSTAT_n;
  } else {
    RxInRoll = 0.005 * rtb_Sum2_i +
      tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator1_DSTAT_n;
  }

  /* End of DiscreteIntegrator: '<S50>/Discrete-Time Integrator1' */

  /* DiscreteIntegrator: '<S50>/Discrete-Time Integrator' */
  if (tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator_SYSTEM_o != 0) {
    imax = tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator_DSTATE_a;
  } else {
    imax = 0.005 * RxInRoll +
      tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator_DSTATE_a;
  }

  if (imax >= 733.03828583761833) {
    imax = 733.03828583761833;
  } else {
    if (imax <= 0.0) {
      imax = 0.0;
    }
  }

  /* End of DiscreteIntegrator: '<S50>/Discrete-Time Integrator' */

  /* Gain: '<S41>/Direction' incorporates:
   *  Gain: '<S41>/k'
   *  Product: '<S41>/omega^2'
   *  Sum: '<S50>/Sum'
   */
  rtb_Direction_o = -(imax * imax * 1.92E-5);

  /* Switch: '<S25>/Init' incorporates:
   *  Logic: '<S25>/FixPt Logical Operator'
   *  UnitDelay: '<S25>/FixPt Unit Delay1'
   *  UnitDelay: '<S25>/FixPt Unit Delay2'
   */
  if (tricopter_simulation_rtw_DWork.FixPtUnitDelay2_DSTATE != 0) {
    rtb_Init_idx = tricopter_simulation_rtw_ConstB.Sum9;
    rtb_Init_idx_0 = tricopter_simulation_rtw_ConstB.Sum2;
    rtb_Init_idx_1 = tricopter_simulation_rtw_ConstB.Sum1;
    rtb_Init_idx_2 = tricopter_simulation_rtw_ConstB.Sum3;
  } else {
    rtb_Init_idx = tricopter_simulation_rtw_DWork.FixPtUnitDelay1_DSTATE[0];
    rtb_Init_idx_0 = tricopter_simulation_rtw_DWork.FixPtUnitDelay1_DSTATE[1];
    rtb_Init_idx_1 = tricopter_simulation_rtw_DWork.FixPtUnitDelay1_DSTATE[2];
    rtb_Init_idx_2 = tricopter_simulation_rtw_DWork.FixPtUnitDelay1_DSTATE[3];
  }

  /* End of Switch: '<S25>/Init' */

  /* Gain: '<S24>/Gain' incorporates:
   *  DiscreteIntegrator: '<S4>/Rates'
   */
  rtb_rad2deg_s_idx = 0.01 * tricopter_simulation_rtw_DWork.Rates_DSTATE[0];
  rtb_rad2deg_s_idx_0 = 0.01 * tricopter_simulation_rtw_DWork.Rates_DSTATE[1];
  rtb_rad2deg_s_idx_1 = 0.01 * tricopter_simulation_rtw_DWork.Rates_DSTATE[2];

  /* Fcn: '<S26>/magnitude square' */
  rtb_rad2deg_s_idx = (rtb_rad2deg_s_idx * rtb_rad2deg_s_idx +
                       rtb_rad2deg_s_idx_0 * rtb_rad2deg_s_idx_0) +
    rtb_rad2deg_s_idx_1 * rtb_rad2deg_s_idx_1;
  if (rtb_rad2deg_s_idx < 0.0) {
    rtb_rad2deg_s_idx = -sqrt(-rtb_rad2deg_s_idx);
  } else {
    rtb_rad2deg_s_idx = sqrt(rtb_rad2deg_s_idx);
  }

  /* End of Fcn: '<S26>/magnitude square' */

  /* Gain: '<S24>/Gain2' */
  rtb_delta2 = 0.5 * rtb_rad2deg_s_idx;

  /* Trigonometry: '<S24>/Trigonometric Function1' */
  rtb_TrigonometricFunction1 = cos(rtb_delta2);

  /* Trigonometry: '<S24>/Trigonometric Function' */
  rtb_TrigonometricFunction_h = sin(rtb_delta2);

  /* Sqrt: '<S29>/Sqrt' incorporates:
   *  DiscreteIntegrator: '<S4>/Rates'
   *  DotProduct: '<S29>/Dot Product'
   */
  rtb_Sqrt = sqrt((tricopter_simulation_rtw_DWork.Rates_DSTATE[0] *
                   tricopter_simulation_rtw_DWork.Rates_DSTATE[0] +
                   tricopter_simulation_rtw_DWork.Rates_DSTATE[1] *
                   tricopter_simulation_rtw_DWork.Rates_DSTATE[1]) +
                  tricopter_simulation_rtw_DWork.Rates_DSTATE[2] *
                  tricopter_simulation_rtw_DWork.Rates_DSTATE[2]);

  /* Switch: '<S27>/Switch' incorporates:
   *  DiscreteIntegrator: '<S4>/Rates'
   *  Product: '<S27>/e//|e|'
   */
  if (rtb_Sqrt > 0.0001) {
    rtb_rad2deg_s_idx = tricopter_simulation_rtw_DWork.Rates_DSTATE[0] /
      rtb_Sqrt;
    rtb_rad2deg_s_idx_0 = tricopter_simulation_rtw_DWork.Rates_DSTATE[1] /
      rtb_Sqrt;
    rtb_rad2deg_s_idx_1 = tricopter_simulation_rtw_DWork.Rates_DSTATE[2] /
      rtb_Sqrt;
  } else {
    rtb_rad2deg_s_idx = tricopter_simulation_rtw_DWork.Rates_DSTATE[0];
    rtb_rad2deg_s_idx_0 = tricopter_simulation_rtw_DWork.Rates_DSTATE[1];
    rtb_rad2deg_s_idx_1 = tricopter_simulation_rtw_DWork.Rates_DSTATE[2];
  }

  /* End of Switch: '<S27>/Switch' */

  /* Product: '<S24>/Product' */
  rtb_Product_e_idx = rtb_TrigonometricFunction_h * rtb_rad2deg_s_idx;
  rtb_rad2deg_s_idx_0 *= rtb_TrigonometricFunction_h;
  rtb_rad2deg_s_idx = rtb_TrigonometricFunction_h * rtb_rad2deg_s_idx_1;

  /* Fcn: '<S28>/q_0' */
  rtb_q_0 = ((rtb_Init_idx * rtb_TrigonometricFunction1 - rtb_Init_idx_0 *
              rtb_Product_e_idx) - rtb_Init_idx_1 * rtb_rad2deg_s_idx_0) -
    rtb_Init_idx_2 * rtb_rad2deg_s_idx;

  /* Fcn: '<S28>/q_1' */
  rtb_q_1 = ((rtb_Init_idx * rtb_Product_e_idx + rtb_Init_idx_0 *
              rtb_TrigonometricFunction1) + rtb_Init_idx_1 * rtb_rad2deg_s_idx)
    - rtb_Init_idx_2 * rtb_rad2deg_s_idx_0;

  /* Fcn: '<S28>/q_2' */
  rtb_q_2 = ((rtb_Init_idx * rtb_rad2deg_s_idx_0 - rtb_Init_idx_0 *
              rtb_rad2deg_s_idx) + rtb_Init_idx_1 * rtb_TrigonometricFunction1)
    + rtb_Init_idx_2 * rtb_Product_e_idx;

  /* Fcn: '<S28>/q_3' */
  rtb_q_3 = ((rtb_Init_idx * rtb_rad2deg_s_idx + rtb_Init_idx_0 *
              rtb_rad2deg_s_idx_0) - rtb_Init_idx_1 * rtb_Product_e_idx) +
    rtb_Init_idx_2 * rtb_TrigonometricFunction1;

  /* Math: '<S21>/Math Function' */
  rtb_rad2deg_s_idx = rtb_q_0 * rtb_q_0;
  rtb_rad2deg_s_idx_0 = rtb_q_1 * rtb_q_1;
  rtb_rad2deg_s_idx_1 = rtb_q_2 * rtb_q_2;
  rtb_Product_e_idx = rtb_q_3 * rtb_q_3;

  /* Sum: '<S21>/Sum' */
  rtb_RbiMatrix[0] = ((rtb_rad2deg_s_idx + rtb_rad2deg_s_idx_0) -
                      rtb_rad2deg_s_idx_1) - rtb_Product_e_idx;

  /* Product: '<S21>/exey' */
  rtb_exey = rtb_q_1 * rtb_q_2;

  /* Product: '<S21>/eze0' */
  rtb_eze0 = rtb_q_3 * rtb_q_0;

  /* Gain: '<S21>/Gain' incorporates:
   *  Sum: '<S21>/Sum1'
   */
  rtb_RbiMatrix[1] = (rtb_exey + rtb_eze0) * 2.0;

  /* Product: '<S21>/exez' */
  rtb_exez = rtb_q_1 * rtb_q_3;

  /* Product: '<S21>/eye0' */
  rtb_eye0 = rtb_q_2 * rtb_q_0;

  /* Gain: '<S21>/Gain1' incorporates:
   *  Sum: '<S21>/Sum2'
   */
  rtb_RbiMatrix[2] = (rtb_exez - rtb_eye0) * 2.0;

  /* Gain: '<S21>/Gain2' incorporates:
   *  Sum: '<S21>/Sum3'
   */
  rtb_RbiMatrix[3] = (rtb_exey - rtb_eze0) * 2.0;

  /* Sum: '<S21>/Sum4' */
  rtb_RbiMatrix[4] = ((rtb_rad2deg_s_idx - rtb_rad2deg_s_idx_0) +
                      rtb_rad2deg_s_idx_1) - rtb_Product_e_idx;

  /* Product: '<S21>/eyez' */
  rtb_eyez = rtb_q_2 * rtb_q_3;

  /* Product: '<S21>/exe0' */
  rtb_exe0 = rtb_q_1 * rtb_q_0;

  /* Gain: '<S21>/Gain3' incorporates:
   *  Sum: '<S21>/Sum5'
   */
  rtb_RbiMatrix[5] = (rtb_eyez + rtb_exe0) * 2.0;

  /* Gain: '<S21>/Gain4' incorporates:
   *  Sum: '<S21>/Sum6'
   */
  rtb_RbiMatrix[6] = (rtb_exez + rtb_eye0) * 2.0;

  /* Gain: '<S21>/Gain5' incorporates:
   *  Sum: '<S21>/Sum7'
   */
  rtb_RbiMatrix[7] = (rtb_eyez - rtb_exe0) * 2.0;

  /* Sum: '<S21>/Sum8' */
  rtb_RbiMatrix[8] = ((rtb_rad2deg_s_idx - rtb_rad2deg_s_idx_0) -
                      rtb_rad2deg_s_idx_1) + rtb_Product_e_idx;

  /* Math: '<S21>/Get DCM' */
  for (i = 0; i < 3; i++) {
    rtb_GetDCM[3 * i] = rtb_RbiMatrix[i];
    rtb_GetDCM[1 + 3 * i] = rtb_RbiMatrix[i + 3];
    rtb_GetDCM[2 + 3 * i] = rtb_RbiMatrix[i + 6];
  }

  /* End of Math: '<S21>/Get DCM' */

  /* Product: '<S34>/T_i->b' incorporates:
   *  Constant: '<S34>/G'
   *  Sum: '<S3>/Summe der  Kräfte (body)'
   */
  for (i = 0; i < 3; i++) {
    rtb_GetDCM_0[i] = rtb_GetDCM[i + 6] * 9.81 + (rtb_GetDCM[i + 3] * 0.0 +
      rtb_GetDCM[i] * 0.0);
  }

  /* End of Product: '<S34>/T_i->b' */

  /* Product: '<S4>/enable_acc ' incorporates:
   *  Constant: '<S1>/0'
   *  Constant: '<S35>/A=pi*r^2'
   *  Constant: '<S35>/G'
   *  Constant: '<S4>/run_vel'
   *  DiscreteIntegrator: '<S4>/Velocity'
   *  Gain: '<S35>/Direction'
   *  Product: '<S15>/Product'
   *  Product: '<S35>/V^2'
   *  Product: '<S35>/Wz'
   *  Product: '<S44>/Product'
   *  Product: '<S47>/Product'
   *  Signum: '<S35>/Sign'
   *  Sum: '<S15>/Sum'
   *  Sum: '<S3>/Summe der  Kräfte (body)'
   *  Sum: '<S47>/Add'
   *  Trigonometry: '<S44>/Trigonometric Function'
   *  Trigonometry: '<S47>/Trigonometric Function'
   */
  rtb_rad2deg_s_idx = (tricopter_simulation_rtw_DWork.Velocity_DSTATE[0] * 0.0 +
                       rtb_GetDCM_0[0]) * 0.0;
  rtb_rad2deg_s_idx_0 = ((rtb_Direction * sin(rtb_DLookupTable) + rtb_GetDCM_0[1])
    + tricopter_simulation_rtw_DWork.Velocity_DSTATE[1] * 0.0) * 0.0;
  rtb_rad2deg_s_idx_1 = ((tricopter_simulation_rtw_DWork.Velocity_DSTATE[2] *
    tricopter_simulation_rtw_DWork.Velocity_DSTATE[2] * 0.63 *
    0.78539816339744828 * -(tricopter_simulation_rtw_DWork.Velocity_DSTATE[2] <
    0.0 ? -1.0 : tricopter_simulation_rtw_DWork.Velocity_DSTATE[2] > 0.0 ? 1.0 :
    tricopter_simulation_rtw_DWork.Velocity_DSTATE[2] == 0.0 ? 0.0 :
    tricopter_simulation_rtw_DWork.Velocity_DSTATE[2]) + (((rtb_Direction_d +
    rtb_Direction_o) + rtb_Direction * cos(rtb_DLookupTable)) + rtb_GetDCM_0[2]))
    + tricopter_simulation_rtw_DWork.Velocity_DSTATE[2] * 0.0) * 0.0;

  /* Product: '<S18>/Product2' incorporates:
   *  DiscreteIntegrator: '<S4>/Rates'
   */
  for (i = 0; i < 3; i++) {
    rtb_Product2_m[i] = tricopter_simulation_rtw_ConstB.Selector2[i + 6] *
      tricopter_simulation_rtw_DWork.Rates_DSTATE[2] +
      (tricopter_simulation_rtw_ConstB.Selector2[i + 3] *
       tricopter_simulation_rtw_DWork.Rates_DSTATE[1] +
       tricopter_simulation_rtw_ConstB.Selector2[i] *
       tricopter_simulation_rtw_DWork.Rates_DSTATE[0]);
  }

  /* End of Product: '<S18>/Product2' */

  /* Sum: '<S3>/Summe der Momente (body)' incorporates:
   *  Constant: '<S36>/0'
   *  Constant: '<S36>/A=pi*r^2'
   *  Constant: '<S48>/Laenge Arm1'
   *  DiscreteIntegrator: '<S4>/Rates'
   *  Gain: '<S36>/Direction'
   *  Product: '<S36>/Mr'
   *  Product: '<S36>/r^2'
   *  Product: '<S43>/Product'
   *  Product: '<S45>/Product'
   *  Product: '<S45>/Product1'
   *  Product: '<S48>/Product1'
   *  Signum: '<S36>/Sign'
   *  Sum: '<S43>/Add'
   *  Sum: '<S45>/Add'
   *  Sum: '<S45>/Add1'
   *  Trigonometry: '<S45>/Trigonometric Function'
   *  Trigonometry: '<S48>/Trigonometric Function'
   */
  rtb_GetDCM_0[0] = (rtb_Direction_o - rtb_Direction_d) *
    tricopter_simulation_rtw_ConstB.Sqrt;
  rtb_GetDCM_0[1] = (rtb_Direction_o - rtb_Direction_d) *
    tricopter_simulation_rtw_ConstB.Gain_a + rtb_Direction * sin
    (rtb_DLookupTable) * tricopter_simulation_rtw_ConstB.Sqrt_n;
  rtb_GetDCM_0[2] = rtb_Direction * sin(rtb_DLookupTable) * 0.5;
  tmp[0] = 0.0;
  tmp[1] = 0.0;
  tmp[2] = tricopter_simulation_rtw_DWork.Rates_DSTATE[2] *
    tricopter_simulation_rtw_DWork.Rates_DSTATE[2] * 0.0003 *
    -(tricopter_simulation_rtw_DWork.Rates_DSTATE[2] < 0.0 ? -1.0 :
      tricopter_simulation_rtw_DWork.Rates_DSTATE[2] > 0.0 ? 1.0 :
      tricopter_simulation_rtw_DWork.Rates_DSTATE[2] == 0.0 ? 0.0 :
      tricopter_simulation_rtw_DWork.Rates_DSTATE[2]);

  /* Product: '<S17>/Product1' incorporates:
   *  DiscreteIntegrator: '<S4>/Rates'
   */
  tmp_0[0] = tricopter_simulation_rtw_DWork.Rates_DSTATE[1];
  tmp_0[1] = tricopter_simulation_rtw_DWork.Rates_DSTATE[2];
  tmp_0[2] = tricopter_simulation_rtw_DWork.Rates_DSTATE[0];
  rtb_Product2_m_0[0] = rtb_Product2_m[2];
  rtb_Product2_m_0[1] = rtb_Product2_m[0];
  rtb_Product2_m_0[2] = rtb_Product2_m[1];

  /* Product: '<S17>/Product2' incorporates:
   *  DiscreteIntegrator: '<S4>/Rates'
   */
  tmp_1[0] = tricopter_simulation_rtw_DWork.Rates_DSTATE[2];
  tmp_1[1] = tricopter_simulation_rtw_DWork.Rates_DSTATE[0];
  tmp_1[2] = tricopter_simulation_rtw_DWork.Rates_DSTATE[1];
  rtb_Product2_m_1[0] = rtb_Product2_m[1];
  rtb_Product2_m_1[1] = rtb_Product2_m[2];
  rtb_Product2_m_1[2] = rtb_Product2_m[0];

  /* Sum: '<S13>/Sum' incorporates:
   *  DiscreteIntegrator: '<S4>/Rates'
   *  Product: '<S17>/Product1'
   *  Product: '<S17>/Product2'
   *  Product: '<S19>/Product2'
   *  Sum: '<S17>/Sum1'
   *  Sum: '<S3>/Summe der Momente (body)'
   */
  for (i = 0; i < 3; i++) {
    rtb_Direction_o_0[i] = ((rtb_GetDCM_0[i] + tmp[i]) -
      ((tricopter_simulation_rtw_ConstB.Selector1[i + 3] *
        tricopter_simulation_rtw_DWork.Rates_DSTATE[1] +
        tricopter_simulation_rtw_ConstB.Selector1[i] *
        tricopter_simulation_rtw_DWork.Rates_DSTATE[0]) +
       tricopter_simulation_rtw_ConstB.Selector1[i + 6] *
       tricopter_simulation_rtw_DWork.Rates_DSTATE[2])) - (tmp_0[i] *
      rtb_Product2_m_0[i] - tmp_1[i] * rtb_Product2_m_1[i]);
  }

  /* End of Sum: '<S13>/Sum' */

  /* Product: '<S13>/Product' */
  rt_mldivide_U1d3x3_U2d3_Yd3x1_snf(&tricopter_simulation_rtw_ConstB.Selector[0],
    rtb_Direction_o_0, rtb_Product_l);

  /* Product: '<S4>/Product ' incorporates:
   *  DiscreteIntegrator: '<S4>/Velocity'
   *  Math: '<S4>/Math Function'
   *  Product: '<S4>/enable_vel'
   */
  for (i = 0; i < 3; i++) {
    tmp[i] = rtb_GetDCM[i + 6] * tricopter_simulation_rtw_DWork.Velocity_DSTATE
      [2] + (rtb_GetDCM[i + 3] * tricopter_simulation_rtw_DWork.Velocity_DSTATE
             [1] + tricopter_simulation_rtw_DWork.Velocity_DSTATE[0] *
             rtb_GetDCM[i]);
  }

  /* End of Product: '<S4>/Product ' */

  /* Product: '<S4>/enable_vel' incorporates:
   *  Constant: '<S4>/run_pos'
   */
  rtb_Product_e_idx = 0.0 * tmp[0];
  rtb_Init_idx = 0.0 * tmp[1];
  rtb_Init_idx_0 = 0.0 * tmp[2];

  /* Outport: '<Root>/motion' incorporates:
   *  BusCreator: '<Root>/BusConversion_InsertedFor_motion_at_inport_0'
   *  DiscreteIntegrator: '<S4>/Position'
   *  DiscreteIntegrator: '<S4>/Rates'
   *  DiscreteIntegrator: '<S4>/Velocity'
   */
  tricopter_simulation_rtw_Y.motion.acc.ax = rtb_rad2deg_s_idx;
  tricopter_simulation_rtw_Y.motion.acc.ay = rtb_rad2deg_s_idx_0;
  tricopter_simulation_rtw_Y.motion.acc.az = rtb_rad2deg_s_idx_1;
  tricopter_simulation_rtw_Y.motion.rate.p =
    tricopter_simulation_rtw_DWork.Rates_DSTATE[0];
  tricopter_simulation_rtw_Y.motion.rate.q =
    tricopter_simulation_rtw_DWork.Rates_DSTATE[1];
  tricopter_simulation_rtw_Y.motion.rate.r =
    tricopter_simulation_rtw_DWork.Rates_DSTATE[2];
  tricopter_simulation_rtw_Y.motion.quaternion.q0 = rtb_q_0;
  tricopter_simulation_rtw_Y.motion.quaternion.q1 = rtb_q_1;
  tricopter_simulation_rtw_Y.motion.quaternion.q2 = rtb_q_2;
  tricopter_simulation_rtw_Y.motion.quaternion.q3 = rtb_q_3;
  tricopter_simulation_rtw_Y.motion.pos.x =
    tricopter_simulation_rtw_DWork.Position_DSTATE[0];
  tricopter_simulation_rtw_Y.motion.pos.y =
    tricopter_simulation_rtw_DWork.Position_DSTATE[1];
  tricopter_simulation_rtw_Y.motion.pos.z =
    tricopter_simulation_rtw_DWork.Position_DSTATE[2];
  tricopter_simulation_rtw_Y.motion.rate_dot.pdot = rtb_Product_l[0];
  tricopter_simulation_rtw_Y.motion.rate_dot.qdot = rtb_Product_l[1];
  tricopter_simulation_rtw_Y.motion.rate_dot.rdot = rtb_Product_l[2];
  tricopter_simulation_rtw_Y.motion.vel_geo.ug = rtb_Product_e_idx;
  tricopter_simulation_rtw_Y.motion.vel_geo.vg = rtb_Init_idx;
  tricopter_simulation_rtw_Y.motion.vel_geo.wg = rtb_Init_idx_0;
  tricopter_simulation_rtw_Y.motion.vel_body.ub =
    tricopter_simulation_rtw_DWork.Velocity_DSTATE[0];
  tricopter_simulation_rtw_Y.motion.vel_body.vb =
    tricopter_simulation_rtw_DWork.Velocity_DSTATE[1];
  tricopter_simulation_rtw_Y.motion.vel_body.wb =
    tricopter_simulation_rtw_DWork.Velocity_DSTATE[2];

  /* Product: '<S14>/Product1' incorporates:
   *  DiscreteIntegrator: '<S4>/Rates'
   *  DiscreteIntegrator: '<S4>/Velocity'
   */
  rtb_Init_idx_1 = tricopter_simulation_rtw_DWork.Velocity_DSTATE[1] *
    tricopter_simulation_rtw_DWork.Rates_DSTATE[2];
  rtb_Init_idx_2 = tricopter_simulation_rtw_DWork.Velocity_DSTATE[2] *
    tricopter_simulation_rtw_DWork.Rates_DSTATE[0];
  rtb_Product1_h_idx = tricopter_simulation_rtw_DWork.Velocity_DSTATE[0] *
    tricopter_simulation_rtw_DWork.Rates_DSTATE[1];

  /* Product: '<S14>/Product2' incorporates:
   *  DiscreteIntegrator: '<S4>/Rates'
   *  DiscreteIntegrator: '<S4>/Velocity'
   */
  rtb_Product2_a_idx = tricopter_simulation_rtw_DWork.Velocity_DSTATE[2] *
    tricopter_simulation_rtw_DWork.Rates_DSTATE[1];
  rtb_Product2_a_idx_0 = tricopter_simulation_rtw_DWork.Velocity_DSTATE[0] *
    tricopter_simulation_rtw_DWork.Rates_DSTATE[2];
  rtb_Product2_a_idx_1 = tricopter_simulation_rtw_DWork.Velocity_DSTATE[1] *
    tricopter_simulation_rtw_DWork.Rates_DSTATE[0];

  /* Lookup_n-D: '<S37>/1-D Lookup Table' incorporates:
   *  MATLAB Function: '<S2>/MATLAB Function'
   */
  tricopter_simulation_rtw_B.DLookupTable = look1_binlxpw(MotorOut,
    tricopter_simulation_rtw_ConstP.pooled12,
    tricopter_simulation_rtw_ConstP.pooled17, 1U);

  /* Lookup_n-D: '<S38>/1-D Lookup Table' incorporates:
   *  MATLAB Function: '<S2>/MATLAB Function'
   */
  tricopter_simulation_rtw_B.DLookupTable_f = look1_binlxpw(MotorOut_0,
    tricopter_simulation_rtw_ConstP.pooled12,
    tricopter_simulation_rtw_ConstP.pooled17, 1U);

  /* Lookup_n-D: '<S39>/1-D Lookup Table' incorporates:
   *  MATLAB Function: '<S2>/MATLAB Function'
   */
  tricopter_simulation_rtw_B.DLookupTable_e = look1_binlxpw(MotorOut_1,
    tricopter_simulation_rtw_ConstP.pooled12,
    tricopter_simulation_rtw_ConstP.pooled17, 1U);

  /* Update for Delay: '<S39>/Delay' */
  tricopter_simulation_rtw_DWork.Delay_DSTATE =
    tricopter_simulation_rtw_B.DLookupTable_e;

  /* Update for UnitDelay: '<S51>/Unit Delay' */
  tricopter_simulation_rtw_DWork.UnitDelay_DSTATE = u;

  /* Update for UnitDelay: '<S51>/Unit Delay1' */
  tricopter_simulation_rtw_DWork.UnitDelay1_DSTATE = du;

  /* Update for DiscreteIntegrator: '<S51>/Discrete-Time Integrator1' */
  tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator1_SYSTEM_ = 0U;
  tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator1_DSTATE = 0.005 *
    rtb_Sum2 + du;

  /* Update for DiscreteIntegrator: '<S51>/Discrete-Time Integrator' */
  tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator_SYSTEM_E = 0U;
  tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator_DSTATE = 0.005 * du + u;
  if (tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator_DSTATE >=
      733.03828583761833) {
    tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator_DSTATE =
      733.03828583761833;
  } else {
    if (tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator_DSTATE <= 0.0) {
      tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator_DSTATE = 0.0;
    }
  }

  /* End of Update for DiscreteIntegrator: '<S51>/Discrete-Time Integrator' */

  /* Update for DiscreteIntegrator: '<S4>/Rates' */
  tricopter_simulation_rtw_DWork.Rates_DSTATE[0] = 0.01 * rtb_Product_l[0] +
    tricopter_simulation_rtw_DWork.Rates_DSTATE[0];
  tricopter_simulation_rtw_DWork.Rates_DSTATE[1] = 0.01 * rtb_Product_l[1] +
    tricopter_simulation_rtw_DWork.Rates_DSTATE[1];
  tricopter_simulation_rtw_DWork.Rates_DSTATE[2] = 0.01 * rtb_Product_l[2] +
    tricopter_simulation_rtw_DWork.Rates_DSTATE[2];

  /* Update for Memory: '<S2>/Memory1' */
  tricopter_simulation_rtw_DWork.Memory1_PreviousInput[0] = rtb_integral_out_idx;
  tricopter_simulation_rtw_DWork.Memory1_PreviousInput[1] =
    rtb_integral_out_idx_0;
  tricopter_simulation_rtw_DWork.Memory1_PreviousInput[2] =
    rtb_integral_out_idx_1;

  /* Update for Memory: '<S2>/Memory' */
  tricopter_simulation_rtw_DWork.Memory_PreviousInput[0] =
    rtb_last_error_out_idx;
  tricopter_simulation_rtw_DWork.Memory_PreviousInput[1] =
    rtb_last_error_out_idx_0;
  tricopter_simulation_rtw_DWork.Memory_PreviousInput[2] =
    rtb_last_error_out_idx_1;

  /* Update for Delay: '<S37>/Delay' */
  tricopter_simulation_rtw_DWork.Delay_DSTATE_d =
    tricopter_simulation_rtw_B.DLookupTable;

  /* Update for UnitDelay: '<S49>/Unit Delay' */
  tricopter_simulation_rtw_DWork.UnitDelay_DSTATE_f = error;

  /* Update for UnitDelay: '<S49>/Unit Delay1' */
  tricopter_simulation_rtw_DWork.UnitDelay1_DSTATE_p = RxInCollective;

  /* Update for DiscreteIntegrator: '<S49>/Discrete-Time Integrator1' */
  tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator1_SYSTE_b = 0U;
  tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator1_DSTAT_m = 0.005 *
    rtb_Sum2_e + RxInCollective;

  /* Update for DiscreteIntegrator: '<S49>/Discrete-Time Integrator' */
  tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator_SYSTEM_c = 0U;
  tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator_DSTATE_e = 0.005 *
    RxInCollective + error;
  if (tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator_DSTATE_e >=
      733.03828583761833) {
    tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator_DSTATE_e =
      733.03828583761833;
  } else {
    if (tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator_DSTATE_e <= 0.0) {
      tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator_DSTATE_e = 0.0;
    }
  }

  /* End of Update for DiscreteIntegrator: '<S49>/Discrete-Time Integrator' */

  /* Update for Delay: '<S38>/Delay' */
  tricopter_simulation_rtw_DWork.Delay_DSTATE_l =
    tricopter_simulation_rtw_B.DLookupTable_f;

  /* Update for UnitDelay: '<S50>/Unit Delay' */
  tricopter_simulation_rtw_DWork.UnitDelay_DSTATE_i = imax;

  /* Update for UnitDelay: '<S50>/Unit Delay1' */
  tricopter_simulation_rtw_DWork.UnitDelay1_DSTATE_l = RxInRoll;

  /* Update for DiscreteIntegrator: '<S50>/Discrete-Time Integrator1' */
  tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator1_SYSTE_d = 0U;
  tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator1_DSTAT_n = 0.005 *
    rtb_Sum2_i + RxInRoll;

  /* Update for DiscreteIntegrator: '<S50>/Discrete-Time Integrator' */
  tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator_SYSTEM_o = 0U;
  tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator_DSTATE_a = 0.005 *
    RxInRoll + imax;
  if (tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator_DSTATE_a >=
      733.03828583761833) {
    tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator_DSTATE_a =
      733.03828583761833;
  } else {
    if (tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator_DSTATE_a <= 0.0) {
      tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator_DSTATE_a = 0.0;
    }
  }

  /* End of Update for DiscreteIntegrator: '<S50>/Discrete-Time Integrator' */

  /* Update for UnitDelay: '<S25>/FixPt Unit Delay2' incorporates:
   *  Constant: '<S25>/FixPt Constant'
   */
  tricopter_simulation_rtw_DWork.FixPtUnitDelay2_DSTATE = 0U;

  /* Update for UnitDelay: '<S25>/FixPt Unit Delay1' */
  tricopter_simulation_rtw_DWork.FixPtUnitDelay1_DSTATE[0] = rtb_q_0;
  tricopter_simulation_rtw_DWork.FixPtUnitDelay1_DSTATE[1] = rtb_q_1;
  tricopter_simulation_rtw_DWork.FixPtUnitDelay1_DSTATE[2] = rtb_q_2;
  tricopter_simulation_rtw_DWork.FixPtUnitDelay1_DSTATE[3] = rtb_q_3;

  /* Update for DiscreteIntegrator: '<S4>/Velocity' incorporates:
   *  Constant: '<S4>/run_vel'
   *  Product: '<S4>/enable_acc'
   *  Sum: '<S14>/Sum1'
   *  Sum: '<S4>/Sum'
   */
  tricopter_simulation_rtw_DWork.Velocity_DSTATE[0] = ((rtb_Init_idx_1 -
    rtb_Product2_a_idx) + rtb_rad2deg_s_idx) * 0.0 * 0.01 +
    tricopter_simulation_rtw_DWork.Velocity_DSTATE[0];
  tricopter_simulation_rtw_DWork.Velocity_DSTATE[1] = ((rtb_Init_idx_2 -
    rtb_Product2_a_idx_0) + rtb_rad2deg_s_idx_0) * 0.0 * 0.01 +
    tricopter_simulation_rtw_DWork.Velocity_DSTATE[1];
  tricopter_simulation_rtw_DWork.Velocity_DSTATE[2] = ((rtb_Product1_h_idx -
    rtb_Product2_a_idx_1) + rtb_rad2deg_s_idx_1) * 0.0 * 0.01 +
    tricopter_simulation_rtw_DWork.Velocity_DSTATE[2];

  /* Update for DiscreteIntegrator: '<S4>/Position' */
  tricopter_simulation_rtw_DWork.Position_DSTATE[0] = 0.01 * rtb_Product_e_idx +
    tricopter_simulation_rtw_DWork.Position_DSTATE[0];
  tricopter_simulation_rtw_DWork.Position_DSTATE[1] = 0.01 * rtb_Init_idx +
    tricopter_simulation_rtw_DWork.Position_DSTATE[1];
  tricopter_simulation_rtw_DWork.Position_DSTATE[2] = 0.01 * rtb_Init_idx_0 +
    tricopter_simulation_rtw_DWork.Position_DSTATE[2];
}

/* Model initialize function */
void tricopter_simulation_rtwModelClass::initialize()
{
  /* Registration code */

  /* initialize real-time model */
  (void) memset((void *)tricopter_simulation_rtw_M, 0,
                sizeof(RT_MODEL_tricopter_simulation_r));

  /* block I/O */
  {
    tricopter_simulation_rtw_B.DLookupTable = 0.0;
    tricopter_simulation_rtw_B.DLookupTable_f = 0.0;
    tricopter_simulation_rtw_B.DLookupTable_e = 0.0;
  }

  /* states (dwork) */
  (void) memset((void *)&tricopter_simulation_rtw_DWork, 0,
                sizeof(D_Work_tricopter_simulation_rtw));
  tricopter_simulation_rtw_DWork.Delay_DSTATE = 0.0;
  tricopter_simulation_rtw_DWork.UnitDelay_DSTATE = 0.0;
  tricopter_simulation_rtw_DWork.UnitDelay1_DSTATE = 0.0;
  tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator1_DSTATE = 0.0;
  tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator_DSTATE = 0.0;
  tricopter_simulation_rtw_DWork.Rates_DSTATE[0] = 0.0;
  tricopter_simulation_rtw_DWork.Rates_DSTATE[1] = 0.0;
  tricopter_simulation_rtw_DWork.Rates_DSTATE[2] = 0.0;
  tricopter_simulation_rtw_DWork.Delay_DSTATE_d = 0.0;
  tricopter_simulation_rtw_DWork.UnitDelay_DSTATE_f = 0.0;
  tricopter_simulation_rtw_DWork.UnitDelay1_DSTATE_p = 0.0;
  tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator1_DSTAT_m = 0.0;
  tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator_DSTATE_e = 0.0;
  tricopter_simulation_rtw_DWork.Delay_DSTATE_l = 0.0;
  tricopter_simulation_rtw_DWork.UnitDelay_DSTATE_i = 0.0;
  tricopter_simulation_rtw_DWork.UnitDelay1_DSTATE_l = 0.0;
  tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator1_DSTAT_n = 0.0;
  tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator_DSTATE_a = 0.0;
  tricopter_simulation_rtw_DWork.FixPtUnitDelay1_DSTATE[0] = 0.0;
  tricopter_simulation_rtw_DWork.FixPtUnitDelay1_DSTATE[1] = 0.0;
  tricopter_simulation_rtw_DWork.FixPtUnitDelay1_DSTATE[2] = 0.0;
  tricopter_simulation_rtw_DWork.FixPtUnitDelay1_DSTATE[3] = 0.0;
  tricopter_simulation_rtw_DWork.Velocity_DSTATE[0] = 0.0;
  tricopter_simulation_rtw_DWork.Velocity_DSTATE[1] = 0.0;
  tricopter_simulation_rtw_DWork.Velocity_DSTATE[2] = 0.0;
  tricopter_simulation_rtw_DWork.Position_DSTATE[0] = 0.0;
  tricopter_simulation_rtw_DWork.Position_DSTATE[1] = 0.0;
  tricopter_simulation_rtw_DWork.Position_DSTATE[2] = 0.0;
  tricopter_simulation_rtw_DWork.Memory1_PreviousInput[0] = 0.0;
  tricopter_simulation_rtw_DWork.Memory1_PreviousInput[1] = 0.0;
  tricopter_simulation_rtw_DWork.Memory1_PreviousInput[2] = 0.0;
  tricopter_simulation_rtw_DWork.Memory_PreviousInput[0] = 0.0;
  tricopter_simulation_rtw_DWork.Memory_PreviousInput[1] = 0.0;
  tricopter_simulation_rtw_DWork.Memory_PreviousInput[2] = 0.0;

  /* external inputs */
  tricopter_simulation_rtw_U.deflections =
    tricopter_simulation_rtw_rtZsChannelsKK;

  /* external outputs */
  tricopter_simulation_rtw_Y.motion = tricopter_simulation_rtw_rtZsMotion;

  /* Initialize DataMapInfo substructure containing ModelMap for C API */
  tricopter_simulation_rtw_InitializeDataMapInfo(tricopter_simulation_rtw_M,
    &tricopter_simulation_rtw_B);

  /* InitializeConditions for Delay: '<S39>/Delay' */
  tricopter_simulation_rtw_DWork.Delay_DSTATE = 1.0;

  /* InitializeConditions for DiscreteIntegrator: '<S4>/Rates' */
  tricopter_simulation_rtw_DWork.Rates_DSTATE[0] = 0.17453292519943295;
  tricopter_simulation_rtw_DWork.Rates_DSTATE[1] = 0.0;
  tricopter_simulation_rtw_DWork.Rates_DSTATE[2] = 0.0;

  /* InitializeConditions for Delay: '<S37>/Delay' */
  tricopter_simulation_rtw_DWork.Delay_DSTATE_d = 1.0;

  /* InitializeConditions for Delay: '<S38>/Delay' */
  tricopter_simulation_rtw_DWork.Delay_DSTATE_l = 1.0;

  /* InitializeConditions for UnitDelay: '<S25>/FixPt Unit Delay2' */
  tricopter_simulation_rtw_DWork.FixPtUnitDelay2_DSTATE = 1U;

  /* Enable for DiscreteIntegrator: '<S51>/Discrete-Time Integrator1' */
  tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator1_SYSTEM_ = 1U;

  /* Enable for DiscreteIntegrator: '<S51>/Discrete-Time Integrator' */
  tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator_SYSTEM_E = 1U;

  /* Enable for DiscreteIntegrator: '<S49>/Discrete-Time Integrator1' */
  tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator1_SYSTE_b = 1U;

  /* Enable for DiscreteIntegrator: '<S49>/Discrete-Time Integrator' */
  tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator_SYSTEM_c = 1U;

  /* Enable for DiscreteIntegrator: '<S50>/Discrete-Time Integrator1' */
  tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator1_SYSTE_d = 1U;

  /* Enable for DiscreteIntegrator: '<S50>/Discrete-Time Integrator' */
  tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator_SYSTEM_o = 1U;
}

/* Model terminate function */
void tricopter_simulation_rtwModelClass::terminate()
{
  /* (no terminate code required) */
}

/* Constructor */
tricopter_simulation_rtwModelClass::tricopter_simulation_rtwModelClass()
{
  /* Real-Time Model */
  tricopter_simulation_rtw_M = &tricopter_simulation_rtw_M_;
}

/* Destructor */
tricopter_simulation_rtwModelClass::~tricopter_simulation_rtwModelClass()
{
  /* Currently there is no destructor body generated.*/
}

/* Real-Time Model get method */
RT_MODEL_tricopter_simulation_r * tricopter_simulation_rtwModelClass::getRTM()
  const
{
  return tricopter_simulation_rtw_M;
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
