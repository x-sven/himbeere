/*
 * File: tricopter_simulation_rtw.cpp
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
    0.0,                               /* phi */
    0.0,                               /* theta */
    0.0,                               /* psi */
    0.0,                               /* q0 */
    0.0,                               /* q1 */
    0.0,                               /* q2 */
    0.0                                /* q3 */
  },                                   /* attitude */

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

real_T rt_atan2d_snf(real_T u0, real_T u1)
{
  real_T y;
  if (rtIsNaN(u0) || rtIsNaN(u1)) {
    y = (rtNaN);
  } else if (rtIsInf(u0) && rtIsInf(u1)) {
    y = atan2(u0 > 0.0 ? 1.0 : -1.0, u1 > 0.0 ? 1.0 : -1.0);
  } else if (u1 == 0.0) {
    if (u0 > 0.0) {
      y = RT_PI / 2.0;
    } else if (u0 < 0.0) {
      y = -(RT_PI / 2.0);
    } else {
      y = 0.0;
    }
  } else {
    y = atan2(u0, u1);
  }

  return y;
}

real_T rt_roundd_snf(real_T u)
{
  real_T y;
  if (fabs(u) < 4.503599627370496E+15) {
    y = u >= 0.5 ? floor(u + 0.5) : u > -0.5 ? u * 0.0 : ceil(u - 0.5);
  } else {
    y = u;
  }

  return y;
}

real_T rt_modd_snf(real_T u0, real_T u1)
{
  real_T y;
  real_T tmp;
  if (u1 == 0.0) {
    y = u0;
  } else if (!((!rtIsNaN(u0)) && (!rtIsInf(u0)) && ((!rtIsNaN(u1)) && (!rtIsInf
                (u1))))) {
    y = (rtNaN);
  } else {
    tmp = u0 / u1;
    if (u1 <= floor(u1)) {
      y = u0 - floor(tmp) * u1;
    } else if (fabs(tmp - rt_roundd_snf(tmp)) <= DBL_EPSILON * fabs(tmp)) {
      y = 0.0;
    } else {
      y = (tmp - floor(tmp)) * u1;
    }
  }

  return y;
}

/* Model step function */
void tricopter_simulation_rtwModelClass::step()
{
  /* local block i/o variables */
  real_T rtb_Sum2;
  real_T rtb_Sum2_k;
  real_T rtb_Sum2_j;
  real_T rtb_Sum2_l;
  real_T MotorOut;
  real_T MotorOut_0;
  real_T MotorOut_1;
  real_T RxInRoll;
  real_T RxInCollective;
  real_T imax;
  real_T error;
  real_T derivative;
  real_T rtb_Direction;
  real_T rtb_DLookupTable;
  real_T rtb_Direction_m;
  real_T rtb_Direction_i;
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
  real_T u_e;
  real_T rtb_Direction_i_0[3];
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
  real_T rtb_enable_acc_idx;
  real_T rtb_rad2deg_s_idx;
  real_T rtb_rad2deg_s_idx_0;
  real_T rtb_rad2deg_s_idx_1;

  /* Sum: '<S50>/Sum2' incorporates:
   *  Delay: '<S38>/Delay'
   *  Gain: '<S50>/2*zeta * wn'
   *  Gain: '<S50>/wn^2'
   *  Saturate: '<S50>/Demand limits'
   *  Sum: '<S50>/Sum3'
   *  UnitDelay: '<S50>/Unit Delay'
   *  UnitDelay: '<S50>/Unit Delay1'
   */
  rtb_Sum2 = ((tricopter_simulation_rtw_DWork.Delay_DSTATE >= 7000.0 ? 7000.0 :
               tricopter_simulation_rtw_DWork.Delay_DSTATE <= 0.0 ? 0.0 :
               tricopter_simulation_rtw_DWork.Delay_DSTATE) -
              tricopter_simulation_rtw_DWork.UnitDelay_DSTATE) * 289.0 - 32.3 *
    tricopter_simulation_rtw_DWork.UnitDelay1_DSTATE;

  /* DiscreteIntegrator: '<S50>/Discrete-Time Integrator1' */
  if (tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator1_SYSTEM_ != 0) {
    du = tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator1_DSTATE;
  } else {
    du = 0.0025 * rtb_Sum2 +
      tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator1_DSTATE;
  }

  /* End of DiscreteIntegrator: '<S50>/Discrete-Time Integrator1' */

  /* DiscreteIntegrator: '<S50>/Discrete-Time Integrator' */
  if (tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator_SYSTEM_E != 0) {
    u = tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator_DSTATE;
  } else {
    u = 0.0025 * du +
      tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator_DSTATE;
  }

  if (u >= 7000.0) {
    u = 7000.0;
  } else {
    if (u <= 0.0) {
      u = 0.0;
    }
  }

  /* End of DiscreteIntegrator: '<S50>/Discrete-Time Integrator' */

  /* Gain: '<S41>/Direction' incorporates:
   *  Gain: '<S41>/k'
   *  Product: '<S41>/omega^2'
   *  Sum: '<S50>/Sum'
   */
  rtb_Direction = -(u * u * 1.92E-5);

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
   *  Memory: '<S2>/Memory'
   *  Memory: '<S2>/Memory1'
   *  Saturate: '<S2>/Saturation'
   *  Saturate: '<S2>/Saturation1'
   *  Saturate: '<S2>/Saturation2'
   *  Saturate: '<S2>/Saturation3'
   *  Saturate: '<S2>/Saturation4'
   */
  /* MATLAB Function 'KK-Controller/MATLAB Function': '<S32>:1' */
  /* https://github.com/sim-/kk/blob/master/kk.c */
  /*  roll index definition */
  /*  pitch index definition */
  /*  yaw index definition */
  /* '<S32>:1:8' */
  /*  maximal error limit */
  /* '<S32>:1:11' */
  /* '<S32>:1:12' */
  /* '<S32>:1:14' */
  /* '<S32>:1:15' */
  /* '<S32>:1:16' */
  /* '<S32>:1:17' */
  /* ------------------------------------------------------- */
  /* --- Start mixing by setting collective to motor outputs */
  /* ------------------------------------------------------- */
  /* '<S32>:1:22' */
  RxInCollective = ((tricopter_simulation_rtw_U.deflections.col_us >= 2000.0 ?
                     2000.0 : tricopter_simulation_rtw_U.deflections.col_us <=
                     1000.0 ? 1000.0 :
                     tricopter_simulation_rtw_U.deflections.col_us) - 1100.0) *
    10.0 / 8.0;

  /*  0-800 -> 0-1000 */
  /* '<S32>:1:26' */
  /* '<S32>:1:28' */
  imax = RxInCollective;
  if (RxInCollective < 0.0) {
    /* '<S32>:1:29' */
    /* '<S32>:1:30' */
    imax = 0.0;
  }

  /* '<S32>:1:32' */
  imax /= 10.0;

  /*  1000 -> 200 */
  /* ------------------------------------------------------- */
  /*  Roll control loop */
  /* ------------------------------------------------------- */
  /* '<S32>:1:37' */
  error = (((tricopter_simulation_rtw_U.deflections.lat_us >= 2000.0 ? 2000.0 :
             tricopter_simulation_rtw_U.deflections.lat_us <= 1000.0 ? 1000.0 :
             tricopter_simulation_rtw_U.deflections.lat_us) - 1500.0) * 32768.0 /
           500.0 - (rtb_rad2deg_s_idx >= 300.0 ? 300.0 : rtb_rad2deg_s_idx <=
                    -300.0 ? -300.0 : rtb_rad2deg_s_idx) * 109.22666666666667) *
    0.005;

  /* '<S32>:1:44' */
  rtb_integral_out_idx = tricopter_simulation_rtw_DWork.Memory1_PreviousInput[0]
    + error;
  if (rtb_integral_out_idx > imax) {
    /* '<S32>:1:46' */
    /* '<S32>:1:47' */
    rtb_integral_out_idx = imax;
  } else {
    if (rtb_integral_out_idx < -imax) {
      /* '<S32>:1:48' */
      /* '<S32>:1:49' */
      rtb_integral_out_idx = -imax;
    }
  }

  /* '<S32>:1:52' */
  /* '<S32>:1:53' */
  rtb_last_error_out_idx = error;

  /* '<S32>:1:54' */
  /* + RxInRoll; */
  /*  [Tricopter] */
  /* '<S32>:1:57' */
  RxInRoll = ((rtb_integral_out_idx / 100.0 + error) + (error -
    tricopter_simulation_rtw_DWork.Memory_PreviousInput[0]) / 100.0) * 0.875;

  /*  (RxInRoll * 7) >> 3 = RxInRoll*0.875 Approximation of sin(60)=(.875 versus .86602540) without div */
  /* '<S32>:1:59' */
  /* '<S32>:1:60' */
  /* ------------------------------------------------------- */
  /*  Pitch control loop */
  /* ------------------------------------------------------- */
  /* '<S32>:1:65' */
  error = (((tricopter_simulation_rtw_U.deflections.lon_us >= 2000.0 ? 2000.0 :
             tricopter_simulation_rtw_U.deflections.lon_us <= 1000.0 ? 1000.0 :
             tricopter_simulation_rtw_U.deflections.lon_us) - 1500.0) * 32768.0 /
           500.0 - (rtb_rad2deg_s_idx_0 >= 300.0 ? 300.0 : rtb_rad2deg_s_idx_0 <=
                    -300.0 ? -300.0 : rtb_rad2deg_s_idx_0) * 109.22666666666667)
    * 0.01;

  /* '<S32>:1:72' */
  rtb_integral_out_idx_0 = tricopter_simulation_rtw_DWork.Memory1_PreviousInput
    [1] + error;
  if (rtb_integral_out_idx_0 > imax) {
    /* '<S32>:1:74' */
    /* '<S32>:1:75' */
    rtb_integral_out_idx_0 = imax;
  } else {
    if (rtb_integral_out_idx_0 < -imax) {
      /* '<S32>:1:76' */
      /* '<S32>:1:77' */
      rtb_integral_out_idx_0 = -imax;
    }
  }

  /* '<S32>:1:80' */
  /* '<S32>:1:81' */
  rtb_last_error_out_idx_0 = error;

  /* '<S32>:1:82' */
  error = (rtb_integral_out_idx_0 / 100.0 + error) + (error -
    tricopter_simulation_rtw_DWork.Memory_PreviousInput[1]) / 100.0;

  /*  + RxInPitch; */
  /*  [Tricopter] */
  /* '<S32>:1:85' */
  MotorOut_1 = RxInCollective + error;

  /* '<S32>:1:86' */
  error /= 2.0;

  /*  cosine of 60 */
  /* '<S32>:1:87' */
  MotorOut = (RxInCollective + RxInRoll) - error;

  /* '<S32>:1:88' */
  MotorOut_0 = (RxInCollective - RxInRoll) - error;

  /* ------------------------------------------------------- */
  /*  Yaw control loop */
  /* ------------------------------------------------------- */
  /* '<S32>:1:93' */
  error = (((tricopter_simulation_rtw_U.deflections.ped_us >= 2000.0 ? 2000.0 :
             tricopter_simulation_rtw_U.deflections.ped_us <= 1000.0 ? 1000.0 :
             tricopter_simulation_rtw_U.deflections.ped_us) - 1500.0) * 32768.0 /
           500.0 - (rtb_rad2deg_s_idx_1 >= 300.0 ? 300.0 : rtb_rad2deg_s_idx_1 <=
                    -300.0 ? -300.0 : rtb_rad2deg_s_idx_1) * 109.22666666666667)
    * 0.05;
  if (error > 1023.0) {
    /* '<S32>:1:94' */
    /* '<S32>:1:95' */
    error = 1023.0;
  } else {
    if (error < -1023.0) {
      /* '<S32>:1:96' */
      /* '<S32>:1:97' */
      error = -1023.0;
    }
  }

  /* '<S32>:1:100' */
  rtb_integral_out_idx_1 = tricopter_simulation_rtw_DWork.Memory1_PreviousInput
    [2] + error;
  if (rtb_integral_out_idx_1 > imax) {
    /* '<S32>:1:102' */
    /* '<S32>:1:103' */
    rtb_integral_out_idx_1 = imax;
  } else {
    if (rtb_integral_out_idx_1 < -imax) {
      /* '<S32>:1:104' */
      /* '<S32>:1:105' */
      rtb_integral_out_idx_1 = -imax;
    }
  }

  /* '<S32>:1:108' */
  /* '<S32>:1:109' */
  rtb_last_error_out_idx_1 = error;

  /* '<S32>:1:110' */
  /*  + RxInYaw; */
  /*  [Tricopter] */
  /* '<S32>:1:113' */
  /*  [Tricopter] */
  /*  Rather than clipping the motor outputs and causing instability */
  /*  at throttle saturation, we pull down the throttle of the other */
  /*  motors. This gives priority to stabilization without a fixed */
  /*  collective limit. */
  /* '<S32>:1:122' */
  imax = MotorOut;
  if (MotorOut_0 > MotorOut) {
    /* '<S32>:1:123' */
    /* '<S32>:1:124' */
    imax = MotorOut_0;
  }

  if (MotorOut_1 > imax) {
    /* '<S32>:1:126' */
    /* '<S32>:1:127' */
    imax = MotorOut_1;
  }

  /* '<S32>:1:129' */
  imax -= 1000.0;
  if (imax > 0.0) {
    /* '<S32>:1:130' */
    /* '<S32>:1:131' */
    MotorOut -= imax;

    /* '<S32>:1:132' */
    MotorOut_0 -= imax;

    /* '<S32>:1:133' */
    MotorOut_1 -= imax;
  }

  /* --- Limit the lowest value to avoid stopping of motor if motor value is under-saturated --- */
  /* '<S32>:1:137' */
  /*  this is the motor idle level */
  if (MotorOut < 114.0) {
    /* '<S32>:1:138' */
    /* '<S32>:1:139' */
    MotorOut = 114.0;
  }

  if (MotorOut_0 < 114.0) {
    /* '<S32>:1:141' */
    /* '<S32>:1:142' */
    MotorOut_0 = 114.0;
  }

  /* #if defined(TRI_COPTER) || defined(QUAD_COPTER) || defined(QUAD_X_COPTER) || defined(Y4_COPTER) */
  if (MotorOut_1 < 114.0) {
    /* '<S32>:1:146' */
    /* '<S32>:1:147' */
    MotorOut_1 = 114.0;
  }

  /*  Bound pulse length to 1ms <= pulse <= 2ms */
  /* '<S32>:1:151' */
  MotorOut += 1000.0;

  /* '<S32>:1:152' */
  MotorOut_0 += 1000.0;

  /* '<S32>:1:153' */
  MotorOut_1 += 1000.0;

  /* '<S32>:1:154' */
  /* '<S32>:1:156' */
  /* '<S32>:1:157' */
  rtb_rad2deg_s_idx = (500.0 - ((rtb_integral_out_idx_1 / 100.0 + error) +
    (error - tricopter_simulation_rtw_DWork.Memory_PreviousInput[2]) / 100.0)) +
    1000.0;

  /* Sum: '<S51>/Sum2' incorporates:
   *  Gain: '<S51>/2*zeta * wn'
   *  Gain: '<S51>/wn^2'
   *  Saturate: '<S51>/Demand limits'
   *  Sum: '<S51>/Sum3'
   *  UnitDelay: '<S51>/Unit Delay'
   *  UnitDelay: '<S51>/Unit Delay1'
   */
  rtb_Sum2_k = ((rtb_rad2deg_s_idx >= 2100.0 ? 2100.0 : rtb_rad2deg_s_idx <=
                 900.0 ? 900.0 : rtb_rad2deg_s_idx) -
                tricopter_simulation_rtw_DWork.UnitDelay_DSTATE_g) * 400.0 -
    38.0 * tricopter_simulation_rtw_DWork.UnitDelay1_DSTATE_n;

  /* DiscreteIntegrator: '<S51>/Discrete-Time Integrator1' */
  if (tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator1_SYSTE_a != 0) {
    RxInCollective =
      tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator1_DSTAT_m;
  } else {
    RxInCollective = 0.0025 * rtb_Sum2_k +
      tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator1_DSTAT_m;
  }

  /* End of DiscreteIntegrator: '<S51>/Discrete-Time Integrator1' */

  /* DiscreteIntegrator: '<S51>/Discrete-Time Integrator' */
  if (tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator_SYSTEM_i != 0) {
    error = tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator_DSTATE_m;
  } else {
    error = 0.0025 * RxInCollective +
      tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator_DSTATE_m;
  }

  if (error >= 2100.0) {
    error = 2100.0;
  } else {
    if (error <= 900.0) {
      error = 900.0;
    }
  }

  /* End of DiscreteIntegrator: '<S51>/Discrete-Time Integrator' */

  /* Lookup_n-D: '<S45>/1-D Lookup Table' incorporates:
   *  Sum: '<S51>/Sum'
   */
  rtb_DLookupTable = look1_binlxpw(error,
    tricopter_simulation_rtw_ConstP.pooled16,
    tricopter_simulation_rtw_ConstP.DLookupTable_tableDat, 1U);

  /* Sum: '<S48>/Sum2' incorporates:
   *  Delay: '<S36>/Delay'
   *  Gain: '<S48>/2*zeta * wn'
   *  Gain: '<S48>/wn^2'
   *  Saturate: '<S48>/Demand limits'
   *  Sum: '<S48>/Sum3'
   *  UnitDelay: '<S48>/Unit Delay'
   *  UnitDelay: '<S48>/Unit Delay1'
   */
  rtb_Sum2_j = ((tricopter_simulation_rtw_DWork.Delay_DSTATE_j >= 7000.0 ?
                 7000.0 : tricopter_simulation_rtw_DWork.Delay_DSTATE_j <= 0.0 ?
                 0.0 : tricopter_simulation_rtw_DWork.Delay_DSTATE_j) -
                tricopter_simulation_rtw_DWork.UnitDelay_DSTATE_n) * 289.0 -
    32.3 * tricopter_simulation_rtw_DWork.UnitDelay1_DSTATE_j;

  /* DiscreteIntegrator: '<S48>/Discrete-Time Integrator1' */
  if (tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator1_SYSTE_o != 0) {
    RxInRoll = tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator1_DSTAT_i;
  } else {
    RxInRoll = 0.0025 * rtb_Sum2_j +
      tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator1_DSTAT_i;
  }

  /* End of DiscreteIntegrator: '<S48>/Discrete-Time Integrator1' */

  /* DiscreteIntegrator: '<S48>/Discrete-Time Integrator' */
  if (tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator_SYSTEM_k != 0) {
    derivative = tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator_DSTATE_h;
  } else {
    derivative = 0.0025 * RxInRoll +
      tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator_DSTATE_h;
  }

  if (derivative >= 7000.0) {
    derivative = 7000.0;
  } else {
    if (derivative <= 0.0) {
      derivative = 0.0;
    }
  }

  /* End of DiscreteIntegrator: '<S48>/Discrete-Time Integrator' */

  /* Gain: '<S39>/Direction' incorporates:
   *  Gain: '<S39>/k'
   *  Product: '<S39>/omega^2'
   *  Sum: '<S48>/Sum'
   */
  rtb_Direction_m = -(derivative * derivative * 1.92E-5);

  /* Sum: '<S49>/Sum2' incorporates:
   *  Delay: '<S37>/Delay'
   *  Gain: '<S49>/2*zeta * wn'
   *  Gain: '<S49>/wn^2'
   *  Saturate: '<S49>/Demand limits'
   *  Sum: '<S49>/Sum3'
   *  UnitDelay: '<S49>/Unit Delay'
   *  UnitDelay: '<S49>/Unit Delay1'
   */
  rtb_Sum2_l = ((tricopter_simulation_rtw_DWork.Delay_DSTATE_c >= 7000.0 ?
                 7000.0 : tricopter_simulation_rtw_DWork.Delay_DSTATE_c <= 0.0 ?
                 0.0 : tricopter_simulation_rtw_DWork.Delay_DSTATE_c) -
                tricopter_simulation_rtw_DWork.UnitDelay_DSTATE_i) * 289.0 -
    32.3 * tricopter_simulation_rtw_DWork.UnitDelay1_DSTATE_nn;

  /* DiscreteIntegrator: '<S49>/Discrete-Time Integrator1' */
  if (tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator1_SYSTE_m != 0) {
    imax = tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator1_DSTAT_f;
  } else {
    imax = 0.0025 * rtb_Sum2_l +
      tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator1_DSTAT_f;
  }

  /* End of DiscreteIntegrator: '<S49>/Discrete-Time Integrator1' */

  /* DiscreteIntegrator: '<S49>/Discrete-Time Integrator' */
  if (tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator_SYSTE_ka != 0) {
    u_e = tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator_DSTATE_e;
  } else {
    u_e = 0.0025 * imax +
      tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator_DSTATE_e;
  }

  if (u_e >= 7000.0) {
    u_e = 7000.0;
  } else {
    if (u_e <= 0.0) {
      u_e = 0.0;
    }
  }

  /* End of DiscreteIntegrator: '<S49>/Discrete-Time Integrator' */

  /* Gain: '<S40>/Direction' incorporates:
   *  Gain: '<S40>/k'
   *  Product: '<S40>/omega^2'
   *  Sum: '<S49>/Sum'
   */
  rtb_Direction_i = -(u_e * u_e * 1.92E-5);

  /* Switch: '<S24>/Init' incorporates:
   *  Logic: '<S24>/FixPt Logical Operator'
   *  UnitDelay: '<S24>/FixPt Unit Delay1'
   *  UnitDelay: '<S24>/FixPt Unit Delay2'
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

  /* End of Switch: '<S24>/Init' */

  /* Gain: '<S23>/Gain' incorporates:
   *  DiscreteIntegrator: '<S4>/Rates'
   */
  rtb_rad2deg_s_idx = 0.005 * tricopter_simulation_rtw_DWork.Rates_DSTATE[0];
  rtb_rad2deg_s_idx_0 = 0.005 * tricopter_simulation_rtw_DWork.Rates_DSTATE[1];
  rtb_rad2deg_s_idx_1 = 0.005 * tricopter_simulation_rtw_DWork.Rates_DSTATE[2];

  /* Fcn: '<S25>/magnitude square' */
  rtb_rad2deg_s_idx = (rtb_rad2deg_s_idx * rtb_rad2deg_s_idx +
                       rtb_rad2deg_s_idx_0 * rtb_rad2deg_s_idx_0) +
    rtb_rad2deg_s_idx_1 * rtb_rad2deg_s_idx_1;
  if (rtb_rad2deg_s_idx < 0.0) {
    rtb_rad2deg_s_idx = -sqrt(-rtb_rad2deg_s_idx);
  } else {
    rtb_rad2deg_s_idx = sqrt(rtb_rad2deg_s_idx);
  }

  /* End of Fcn: '<S25>/magnitude square' */

  /* Gain: '<S23>/Gain2' */
  rtb_delta2 = 0.5 * rtb_rad2deg_s_idx;

  /* Trigonometry: '<S23>/Trigonometric Function1' */
  rtb_TrigonometricFunction1 = cos(rtb_delta2);

  /* Trigonometry: '<S23>/Trigonometric Function' */
  rtb_TrigonometricFunction_h = sin(rtb_delta2);

  /* Sqrt: '<S28>/Sqrt' incorporates:
   *  DiscreteIntegrator: '<S4>/Rates'
   *  DotProduct: '<S28>/Dot Product'
   */
  rtb_Sqrt = sqrt((tricopter_simulation_rtw_DWork.Rates_DSTATE[0] *
                   tricopter_simulation_rtw_DWork.Rates_DSTATE[0] +
                   tricopter_simulation_rtw_DWork.Rates_DSTATE[1] *
                   tricopter_simulation_rtw_DWork.Rates_DSTATE[1]) +
                  tricopter_simulation_rtw_DWork.Rates_DSTATE[2] *
                  tricopter_simulation_rtw_DWork.Rates_DSTATE[2]);

  /* Switch: '<S26>/Switch' incorporates:
   *  DiscreteIntegrator: '<S4>/Rates'
   *  Product: '<S26>/e//|e|'
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

  /* End of Switch: '<S26>/Switch' */

  /* Product: '<S23>/Product' */
  rtb_Product_e_idx = rtb_TrigonometricFunction_h * rtb_rad2deg_s_idx;
  rtb_rad2deg_s_idx_0 *= rtb_TrigonometricFunction_h;
  rtb_rad2deg_s_idx = rtb_TrigonometricFunction_h * rtb_rad2deg_s_idx_1;

  /* Fcn: '<S27>/q_0' */
  rtb_q_0 = ((rtb_Init_idx * rtb_TrigonometricFunction1 - rtb_Init_idx_0 *
              rtb_Product_e_idx) - rtb_Init_idx_1 * rtb_rad2deg_s_idx_0) -
    rtb_Init_idx_2 * rtb_rad2deg_s_idx;

  /* Fcn: '<S27>/q_1' */
  rtb_q_1 = ((rtb_Init_idx * rtb_Product_e_idx + rtb_Init_idx_0 *
              rtb_TrigonometricFunction1) + rtb_Init_idx_1 * rtb_rad2deg_s_idx)
    - rtb_Init_idx_2 * rtb_rad2deg_s_idx_0;

  /* Fcn: '<S27>/q_2' */
  rtb_q_2 = ((rtb_Init_idx * rtb_rad2deg_s_idx_0 - rtb_Init_idx_0 *
              rtb_rad2deg_s_idx) + rtb_Init_idx_1 * rtb_TrigonometricFunction1)
    + rtb_Init_idx_2 * rtb_Product_e_idx;

  /* Fcn: '<S27>/q_3' */
  rtb_q_3 = ((rtb_Init_idx * rtb_rad2deg_s_idx + rtb_Init_idx_0 *
              rtb_rad2deg_s_idx_0) - rtb_Init_idx_1 * rtb_Product_e_idx) +
    rtb_Init_idx_2 * rtb_TrigonometricFunction1;

  /* Math: '<S20>/Math Function' */
  rtb_rad2deg_s_idx = rtb_q_0 * rtb_q_0;
  rtb_rad2deg_s_idx_0 = rtb_q_1 * rtb_q_1;
  rtb_rad2deg_s_idx_1 = rtb_q_2 * rtb_q_2;
  rtb_Product_e_idx = rtb_q_3 * rtb_q_3;

  /* Sum: '<S20>/Sum' */
  rtb_RbiMatrix[0] = ((rtb_rad2deg_s_idx + rtb_rad2deg_s_idx_0) -
                      rtb_rad2deg_s_idx_1) - rtb_Product_e_idx;

  /* Product: '<S20>/exey' */
  rtb_exey = rtb_q_1 * rtb_q_2;

  /* Product: '<S20>/eze0' */
  rtb_eze0 = rtb_q_3 * rtb_q_0;

  /* Gain: '<S20>/Gain' incorporates:
   *  Sum: '<S20>/Sum1'
   */
  rtb_RbiMatrix[1] = (rtb_exey + rtb_eze0) * 2.0;

  /* Product: '<S20>/exez' */
  rtb_exez = rtb_q_1 * rtb_q_3;

  /* Product: '<S20>/eye0' */
  rtb_eye0 = rtb_q_2 * rtb_q_0;

  /* Gain: '<S20>/Gain1' incorporates:
   *  Sum: '<S20>/Sum2'
   */
  rtb_RbiMatrix[2] = (rtb_exez - rtb_eye0) * 2.0;

  /* Gain: '<S20>/Gain2' incorporates:
   *  Sum: '<S20>/Sum3'
   */
  rtb_RbiMatrix[3] = (rtb_exey - rtb_eze0) * 2.0;

  /* Sum: '<S20>/Sum4' */
  rtb_RbiMatrix[4] = ((rtb_rad2deg_s_idx - rtb_rad2deg_s_idx_0) +
                      rtb_rad2deg_s_idx_1) - rtb_Product_e_idx;

  /* Product: '<S20>/eyez' */
  rtb_eyez = rtb_q_2 * rtb_q_3;

  /* Product: '<S20>/exe0' */
  rtb_exe0 = rtb_q_1 * rtb_q_0;

  /* Gain: '<S20>/Gain3' incorporates:
   *  Sum: '<S20>/Sum5'
   */
  rtb_RbiMatrix[5] = (rtb_eyez + rtb_exe0) * 2.0;

  /* Gain: '<S20>/Gain4' incorporates:
   *  Sum: '<S20>/Sum6'
   */
  rtb_RbiMatrix[6] = (rtb_exez + rtb_eye0) * 2.0;

  /* Gain: '<S20>/Gain5' incorporates:
   *  Sum: '<S20>/Sum7'
   */
  rtb_RbiMatrix[7] = (rtb_eyez - rtb_exe0) * 2.0;

  /* Sum: '<S20>/Sum8' */
  rtb_RbiMatrix[8] = ((rtb_rad2deg_s_idx - rtb_rad2deg_s_idx_0) -
                      rtb_rad2deg_s_idx_1) + rtb_Product_e_idx;

  /* Math: '<S20>/Get DCM' */
  for (i = 0; i < 3; i++) {
    rtb_GetDCM[3 * i] = rtb_RbiMatrix[i];
    rtb_GetDCM[1 + 3 * i] = rtb_RbiMatrix[i + 3];
    rtb_GetDCM[2 + 3 * i] = rtb_RbiMatrix[i + 6];
  }

  /* End of Math: '<S20>/Get DCM' */

  /* Product: '<S33>/T_i->b' incorporates:
   *  Constant: '<S33>/G'
   *  Sum: '<S3>/Summe der  Kräfte (body)'
   */
  for (i = 0; i < 3; i++) {
    rtb_GetDCM_0[i] = rtb_GetDCM[i + 6] * 9.81 + (rtb_GetDCM[i + 3] * 0.0 +
      rtb_GetDCM[i] * 0.0);
  }

  /* End of Product: '<S33>/T_i->b' */

  /* Product: '<S4>/enable_acc ' incorporates:
   *  Constant: '<S1>/0'
   *  Constant: '<S34>/A=pi*r^2'
   *  Constant: '<S34>/G'
   *  DiscreteIntegrator: '<S4>/Velocity'
   *  Gain: '<S34>/Direction'
   *  Product: '<S14>/Product'
   *  Product: '<S34>/V^2'
   *  Product: '<S34>/Wz'
   *  Product: '<S43>/Product'
   *  Product: '<S46>/Product'
   *  Signum: '<S34>/Sign'
   *  Sum: '<S14>/Sum'
   *  Sum: '<S3>/Summe der  Kräfte (body)'
   *  Sum: '<S46>/Add'
   *  Trigonometry: '<S43>/Trigonometric Function'
   *  Trigonometry: '<S46>/Trigonometric Function'
   */
  rtb_Init_idx_1 = tricopter_simulation_rtw_DWork.Velocity_DSTATE[0] * 0.0 +
    rtb_GetDCM_0[0];
  rtb_Init_idx_2 = (rtb_Direction * sin(rtb_DLookupTable) + rtb_GetDCM_0[1]) +
    tricopter_simulation_rtw_DWork.Velocity_DSTATE[1] * 0.0;
  rtb_enable_acc_idx = (tricopter_simulation_rtw_DWork.Velocity_DSTATE[2] *
                        tricopter_simulation_rtw_DWork.Velocity_DSTATE[2] * 0.63
                        * 0.78539816339744828 *
                        -(tricopter_simulation_rtw_DWork.Velocity_DSTATE[2] <
    0.0 ? -1.0 : tricopter_simulation_rtw_DWork.Velocity_DSTATE[2] > 0.0 ? 1.0 :
    tricopter_simulation_rtw_DWork.Velocity_DSTATE[2] == 0.0 ? 0.0 :
    tricopter_simulation_rtw_DWork.Velocity_DSTATE[2]) + (((rtb_Direction_m +
    rtb_Direction_i) + rtb_Direction * cos(rtb_DLookupTable)) + rtb_GetDCM_0[2]))
    + tricopter_simulation_rtw_DWork.Velocity_DSTATE[2] * 0.0;

  /* Math: '<S21>/Math Function' */
  rtb_rad2deg_s_idx_0 = rtb_q_0 * rtb_q_0;
  rtb_rad2deg_s_idx_1 = rtb_q_1 * rtb_q_1;
  rtb_Product_e_idx = rtb_q_2 * rtb_q_2;
  rtb_Init_idx = rtb_q_3 * rtb_q_3;

  /* Product: '<S17>/Product2' incorporates:
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

  /* End of Product: '<S17>/Product2' */

  /* Sum: '<S3>/Summe der Momente (body)' incorporates:
   *  Constant: '<S35>/0'
   *  Constant: '<S35>/A=pi*r^2'
   *  Constant: '<S44>/Laenge Arm1'
   *  Constant: '<S47>/Laenge Arm1'
   *  DiscreteIntegrator: '<S4>/Rates'
   *  Gain: '<S35>/Direction'
   *  Product: '<S35>/Mr'
   *  Product: '<S35>/r^2'
   *  Product: '<S42>/Product'
   *  Product: '<S44>/Product'
   *  Product: '<S44>/Product1'
   *  Product: '<S47>/Product1'
   *  Signum: '<S35>/Sign'
   *  Sum: '<S42>/Add'
   *  Sum: '<S44>/Add'
   *  Sum: '<S44>/Add1'
   *  Trigonometry: '<S44>/Trigonometric Function'
   *  Trigonometry: '<S47>/Trigonometric Function'
   */
  rtb_GetDCM_0[0] = (rtb_Direction_i - rtb_Direction_m) *
    tricopter_simulation_rtw_ConstB.Sqrt;
  rtb_GetDCM_0[1] = (rtb_Direction_m + rtb_Direction_i) *
    tricopter_simulation_rtw_ConstB.Gain_a - rtb_Direction * cos
    (rtb_DLookupTable) * 0.5;
  rtb_GetDCM_0[2] = rtb_Direction * sin(rtb_DLookupTable) * 0.5;
  tmp[0] = 0.0;
  tmp[1] = 0.0;
  tmp[2] = tricopter_simulation_rtw_DWork.Rates_DSTATE[2] *
    tricopter_simulation_rtw_DWork.Rates_DSTATE[2] * 0.0003 *
    -(tricopter_simulation_rtw_DWork.Rates_DSTATE[2] < 0.0 ? -1.0 :
      tricopter_simulation_rtw_DWork.Rates_DSTATE[2] > 0.0 ? 1.0 :
      tricopter_simulation_rtw_DWork.Rates_DSTATE[2] == 0.0 ? 0.0 :
      tricopter_simulation_rtw_DWork.Rates_DSTATE[2]);

  /* Product: '<S16>/Product1' incorporates:
   *  DiscreteIntegrator: '<S4>/Rates'
   */
  tmp_0[0] = tricopter_simulation_rtw_DWork.Rates_DSTATE[1];
  tmp_0[1] = tricopter_simulation_rtw_DWork.Rates_DSTATE[2];
  tmp_0[2] = tricopter_simulation_rtw_DWork.Rates_DSTATE[0];
  rtb_Product2_m_0[0] = rtb_Product2_m[2];
  rtb_Product2_m_0[1] = rtb_Product2_m[0];
  rtb_Product2_m_0[2] = rtb_Product2_m[1];

  /* Product: '<S16>/Product2' incorporates:
   *  DiscreteIntegrator: '<S4>/Rates'
   */
  tmp_1[0] = tricopter_simulation_rtw_DWork.Rates_DSTATE[2];
  tmp_1[1] = tricopter_simulation_rtw_DWork.Rates_DSTATE[0];
  tmp_1[2] = tricopter_simulation_rtw_DWork.Rates_DSTATE[1];
  rtb_Product2_m_1[0] = rtb_Product2_m[1];
  rtb_Product2_m_1[1] = rtb_Product2_m[2];
  rtb_Product2_m_1[2] = rtb_Product2_m[0];

  /* Sum: '<S12>/Sum' incorporates:
   *  DiscreteIntegrator: '<S4>/Rates'
   *  Product: '<S16>/Product1'
   *  Product: '<S16>/Product2'
   *  Product: '<S18>/Product2'
   *  Sum: '<S16>/Sum1'
   *  Sum: '<S3>/Summe der Momente (body)'
   */
  for (i = 0; i < 3; i++) {
    rtb_Direction_i_0[i] = ((rtb_GetDCM_0[i] + tmp[i]) -
      ((tricopter_simulation_rtw_ConstB.Selector1[i + 3] *
        tricopter_simulation_rtw_DWork.Rates_DSTATE[1] +
        tricopter_simulation_rtw_ConstB.Selector1[i] *
        tricopter_simulation_rtw_DWork.Rates_DSTATE[0]) +
       tricopter_simulation_rtw_ConstB.Selector1[i + 6] *
       tricopter_simulation_rtw_DWork.Rates_DSTATE[2])) - (tmp_0[i] *
      rtb_Product2_m_0[i] - tmp_1[i] * rtb_Product2_m_1[i]);
  }

  /* End of Sum: '<S12>/Sum' */

  /* Product: '<S12>/Product' */
  rt_mldivide_U1d3x3_U2d3_Yd3x1_snf(&tricopter_simulation_rtw_ConstB.Selector[0],
    rtb_Direction_i_0, rtb_Product_l);

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

  /* Outport: '<Root>/motion' incorporates:
   *  BusCreator: '<Root>/BusConversion_InsertedFor_motion_at_inport_0'
   *  Constant: '<S29>/ '
   *  Constant: '<S29>/  '
   *  Constant: '<S29>/     '
   *  DiscreteIntegrator: '<S4>/Rates'
   *  Gain: '<S21>/Gain6'
   *  Math: '<S29>/Math Function'
   *  Product: '<S21>/exe0'
   *  Product: '<S21>/eyez'
   *  Sum: '<S21>/Sum10'
   *  Sum: '<S21>/Sum9'
   *  Sum: '<S29>/Sum2'
   *  Sum: '<S29>/Sum3'
   *  Trigonometry: '<S21>/Trigonometric Function'
   */
  tricopter_simulation_rtw_Y.motion.acc.ax = rtb_Init_idx_1;
  tricopter_simulation_rtw_Y.motion.acc.ay = rtb_Init_idx_2;
  tricopter_simulation_rtw_Y.motion.acc.az = rtb_enable_acc_idx;
  tricopter_simulation_rtw_Y.motion.rate.p =
    tricopter_simulation_rtw_DWork.Rates_DSTATE[0];
  tricopter_simulation_rtw_Y.motion.rate.q =
    tricopter_simulation_rtw_DWork.Rates_DSTATE[1];
  tricopter_simulation_rtw_Y.motion.rate.r =
    tricopter_simulation_rtw_DWork.Rates_DSTATE[2];
  tricopter_simulation_rtw_Y.motion.attitude.phi = rt_modd_snf(rt_atan2d_snf
    ((rtb_q_2 * rtb_q_3 + rtb_q_1 * rtb_q_0) * 2.0, ((rtb_rad2deg_s_idx_0 -
    rtb_rad2deg_s_idx_1) - rtb_Product_e_idx) + rtb_Init_idx) +
    3.1415926535897931, 6.2831853071795862) - 3.1415926535897931;

  /* Gain: '<S21>/Gain7' incorporates:
   *  Product: '<S21>/exez'
   *  Product: '<S21>/eye0'
   *  Sum: '<S21>/Sum11'
   */
  rtb_rad2deg_s_idx = (rtb_q_2 * rtb_q_0 - rtb_q_1 * rtb_q_3) * 2.0;

  /* Outport: '<Root>/motion' incorporates:
   *  BusCreator: '<Root>/BusConversion_InsertedFor_motion_at_inport_0'
   *  Constant: '<S30>/ '
   *  Constant: '<S30>/  '
   *  Constant: '<S30>/     '
   *  Constant: '<S31>/ '
   *  Constant: '<S31>/  '
   *  Constant: '<S31>/     '
   *  DiscreteIntegrator: '<S4>/Position'
   *  DiscreteIntegrator: '<S4>/Velocity'
   *  Gain: '<S21>/Gain8'
   *  Math: '<S30>/Math Function'
   *  Math: '<S31>/Math Function'
   *  Product: '<S21>/exey'
   *  Product: '<S21>/eze0'
   *  Product: '<S4>/enable_vel'
   *  Sum: '<S21>/Sum12'
   *  Sum: '<S21>/Sum13'
   *  Sum: '<S30>/Sum2'
   *  Sum: '<S30>/Sum3'
   *  Sum: '<S31>/Sum2'
   *  Sum: '<S31>/Sum3'
   *  Trigonometry: '<S21>/Trigonometric Function1'
   *  Trigonometry: '<S21>/Trigonometric Function2'
   */
  tricopter_simulation_rtw_Y.motion.attitude.theta = rt_modd_snf(asin
    (rtb_rad2deg_s_idx >= 1.0 ? 1.0 : rtb_rad2deg_s_idx <= -1.0 ? -1.0 :
     rtb_rad2deg_s_idx) + 3.1415926535897931, 6.2831853071795862) -
    3.1415926535897931;
  tricopter_simulation_rtw_Y.motion.attitude.psi = rt_modd_snf(rt_atan2d_snf
    ((rtb_q_1 * rtb_q_2 + rtb_q_3 * rtb_q_0) * 2.0, ((rtb_rad2deg_s_idx_0 +
    rtb_rad2deg_s_idx_1) - rtb_Product_e_idx) - rtb_Init_idx) +
    3.1415926535897931, 6.2831853071795862) - 3.1415926535897931;
  tricopter_simulation_rtw_Y.motion.attitude.q0 = rtb_q_0;
  tricopter_simulation_rtw_Y.motion.attitude.q1 = rtb_q_1;
  tricopter_simulation_rtw_Y.motion.attitude.q2 = rtb_q_2;
  tricopter_simulation_rtw_Y.motion.attitude.q3 = rtb_q_3;
  tricopter_simulation_rtw_Y.motion.pos.x =
    tricopter_simulation_rtw_DWork.Position_DSTATE[0];
  tricopter_simulation_rtw_Y.motion.pos.y =
    tricopter_simulation_rtw_DWork.Position_DSTATE[1];
  tricopter_simulation_rtw_Y.motion.pos.z =
    tricopter_simulation_rtw_DWork.Position_DSTATE[2];
  tricopter_simulation_rtw_Y.motion.rate_dot.pdot = rtb_Product_l[0];
  tricopter_simulation_rtw_Y.motion.rate_dot.qdot = rtb_Product_l[1];
  tricopter_simulation_rtw_Y.motion.rate_dot.rdot = rtb_Product_l[2];
  tricopter_simulation_rtw_Y.motion.vel_geo.ug = tmp[0];
  tricopter_simulation_rtw_Y.motion.vel_geo.vg = tmp[1];
  tricopter_simulation_rtw_Y.motion.vel_geo.wg = tmp[2];
  tricopter_simulation_rtw_Y.motion.vel_body.ub =
    tricopter_simulation_rtw_DWork.Velocity_DSTATE[0];
  tricopter_simulation_rtw_Y.motion.vel_body.vb =
    tricopter_simulation_rtw_DWork.Velocity_DSTATE[1];
  tricopter_simulation_rtw_Y.motion.vel_body.wb =
    tricopter_simulation_rtw_DWork.Velocity_DSTATE[2];

  /* Product: '<S13>/Product1' incorporates:
   *  DiscreteIntegrator: '<S4>/Rates'
   *  DiscreteIntegrator: '<S4>/Velocity'
   */
  rtb_rad2deg_s_idx = tricopter_simulation_rtw_DWork.Velocity_DSTATE[1] *
    tricopter_simulation_rtw_DWork.Rates_DSTATE[2];
  rtb_rad2deg_s_idx_0 = tricopter_simulation_rtw_DWork.Velocity_DSTATE[2] *
    tricopter_simulation_rtw_DWork.Rates_DSTATE[0];
  rtb_rad2deg_s_idx_1 = tricopter_simulation_rtw_DWork.Velocity_DSTATE[0] *
    tricopter_simulation_rtw_DWork.Rates_DSTATE[1];

  /* Product: '<S13>/Product2' incorporates:
   *  DiscreteIntegrator: '<S4>/Rates'
   *  DiscreteIntegrator: '<S4>/Velocity'
   */
  rtb_Init_idx = tricopter_simulation_rtw_DWork.Velocity_DSTATE[2] *
    tricopter_simulation_rtw_DWork.Rates_DSTATE[1];
  rtb_Product_e_idx = tricopter_simulation_rtw_DWork.Velocity_DSTATE[0] *
    tricopter_simulation_rtw_DWork.Rates_DSTATE[2];
  rtb_Init_idx_0 = tricopter_simulation_rtw_DWork.Velocity_DSTATE[1] *
    tricopter_simulation_rtw_DWork.Rates_DSTATE[0];

  /* Lookup_n-D: '<S36>/1-D Lookup Table' incorporates:
   *  MATLAB Function: '<S2>/MATLAB Function'
   */
  tricopter_simulation_rtw_B.DLookupTable = look1_binlxpw(MotorOut,
    tricopter_simulation_rtw_ConstP.pooled16,
    tricopter_simulation_rtw_ConstP.pooled21, 1U);

  /* Lookup_n-D: '<S37>/1-D Lookup Table' incorporates:
   *  MATLAB Function: '<S2>/MATLAB Function'
   */
  tricopter_simulation_rtw_B.DLookupTable_b = look1_binlxpw(MotorOut_0,
    tricopter_simulation_rtw_ConstP.pooled16,
    tricopter_simulation_rtw_ConstP.pooled21, 1U);

  /* Lookup_n-D: '<S38>/1-D Lookup Table' incorporates:
   *  MATLAB Function: '<S2>/MATLAB Function'
   */
  tricopter_simulation_rtw_B.DLookupTable_h = look1_binlxpw(MotorOut_1,
    tricopter_simulation_rtw_ConstP.pooled16,
    tricopter_simulation_rtw_ConstP.pooled21, 1U);

  /* Update for Delay: '<S38>/Delay' */
  tricopter_simulation_rtw_DWork.Delay_DSTATE =
    tricopter_simulation_rtw_B.DLookupTable_h;

  /* Update for UnitDelay: '<S50>/Unit Delay' */
  tricopter_simulation_rtw_DWork.UnitDelay_DSTATE = u;

  /* Update for UnitDelay: '<S50>/Unit Delay1' */
  tricopter_simulation_rtw_DWork.UnitDelay1_DSTATE = du;

  /* Update for DiscreteIntegrator: '<S50>/Discrete-Time Integrator1' */
  tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator1_SYSTEM_ = 0U;
  tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator1_DSTATE = 0.0025 *
    rtb_Sum2 + du;

  /* Update for DiscreteIntegrator: '<S50>/Discrete-Time Integrator' */
  tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator_SYSTEM_E = 0U;
  tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator_DSTATE = 0.0025 * du + u;
  if (tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator_DSTATE >= 7000.0) {
    tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator_DSTATE = 7000.0;
  } else {
    if (tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator_DSTATE <= 0.0) {
      tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator_DSTATE = 0.0;
    }
  }

  /* End of Update for DiscreteIntegrator: '<S50>/Discrete-Time Integrator' */

  /* Update for DiscreteIntegrator: '<S4>/Rates' */
  tricopter_simulation_rtw_DWork.Rates_DSTATE[0] = 0.005 * rtb_Product_l[0] +
    tricopter_simulation_rtw_DWork.Rates_DSTATE[0];
  tricopter_simulation_rtw_DWork.Rates_DSTATE[1] = 0.005 * rtb_Product_l[1] +
    tricopter_simulation_rtw_DWork.Rates_DSTATE[1];
  tricopter_simulation_rtw_DWork.Rates_DSTATE[2] = 0.005 * rtb_Product_l[2] +
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

  /* Update for UnitDelay: '<S51>/Unit Delay' */
  tricopter_simulation_rtw_DWork.UnitDelay_DSTATE_g = error;

  /* Update for UnitDelay: '<S51>/Unit Delay1' */
  tricopter_simulation_rtw_DWork.UnitDelay1_DSTATE_n = RxInCollective;

  /* Update for DiscreteIntegrator: '<S51>/Discrete-Time Integrator1' */
  tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator1_SYSTE_a = 0U;
  tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator1_DSTAT_m = 0.0025 *
    rtb_Sum2_k + RxInCollective;

  /* Update for DiscreteIntegrator: '<S51>/Discrete-Time Integrator' */
  tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator_SYSTEM_i = 0U;
  tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator_DSTATE_m = 0.0025 *
    RxInCollective + error;
  if (tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator_DSTATE_m >= 2100.0)
  {
    tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator_DSTATE_m = 2100.0;
  } else {
    if (tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator_DSTATE_m <= 900.0)
    {
      tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator_DSTATE_m = 900.0;
    }
  }

  /* End of Update for DiscreteIntegrator: '<S51>/Discrete-Time Integrator' */

  /* Update for Delay: '<S36>/Delay' */
  tricopter_simulation_rtw_DWork.Delay_DSTATE_j =
    tricopter_simulation_rtw_B.DLookupTable;

  /* Update for UnitDelay: '<S48>/Unit Delay' */
  tricopter_simulation_rtw_DWork.UnitDelay_DSTATE_n = derivative;

  /* Update for UnitDelay: '<S48>/Unit Delay1' */
  tricopter_simulation_rtw_DWork.UnitDelay1_DSTATE_j = RxInRoll;

  /* Update for DiscreteIntegrator: '<S48>/Discrete-Time Integrator1' */
  tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator1_SYSTE_o = 0U;
  tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator1_DSTAT_i = 0.0025 *
    rtb_Sum2_j + RxInRoll;

  /* Update for DiscreteIntegrator: '<S48>/Discrete-Time Integrator' */
  tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator_SYSTEM_k = 0U;
  tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator_DSTATE_h = 0.0025 *
    RxInRoll + derivative;
  if (tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator_DSTATE_h >= 7000.0)
  {
    tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator_DSTATE_h = 7000.0;
  } else {
    if (tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator_DSTATE_h <= 0.0) {
      tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator_DSTATE_h = 0.0;
    }
  }

  /* End of Update for DiscreteIntegrator: '<S48>/Discrete-Time Integrator' */

  /* Update for Delay: '<S37>/Delay' */
  tricopter_simulation_rtw_DWork.Delay_DSTATE_c =
    tricopter_simulation_rtw_B.DLookupTable_b;

  /* Update for UnitDelay: '<S49>/Unit Delay' */
  tricopter_simulation_rtw_DWork.UnitDelay_DSTATE_i = u_e;

  /* Update for UnitDelay: '<S49>/Unit Delay1' */
  tricopter_simulation_rtw_DWork.UnitDelay1_DSTATE_nn = imax;

  /* Update for DiscreteIntegrator: '<S49>/Discrete-Time Integrator1' */
  tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator1_SYSTE_m = 0U;
  tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator1_DSTAT_f = 0.0025 *
    rtb_Sum2_l + imax;

  /* Update for DiscreteIntegrator: '<S49>/Discrete-Time Integrator' */
  tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator_SYSTE_ka = 0U;
  tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator_DSTATE_e = 0.0025 * imax
    + u_e;
  if (tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator_DSTATE_e >= 7000.0)
  {
    tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator_DSTATE_e = 7000.0;
  } else {
    if (tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator_DSTATE_e <= 0.0) {
      tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator_DSTATE_e = 0.0;
    }
  }

  /* End of Update for DiscreteIntegrator: '<S49>/Discrete-Time Integrator' */

  /* Update for UnitDelay: '<S24>/FixPt Unit Delay2' incorporates:
   *  Constant: '<S24>/FixPt Constant'
   */
  tricopter_simulation_rtw_DWork.FixPtUnitDelay2_DSTATE = 0U;

  /* Update for UnitDelay: '<S24>/FixPt Unit Delay1' */
  tricopter_simulation_rtw_DWork.FixPtUnitDelay1_DSTATE[0] = rtb_q_0;
  tricopter_simulation_rtw_DWork.FixPtUnitDelay1_DSTATE[1] = rtb_q_1;
  tricopter_simulation_rtw_DWork.FixPtUnitDelay1_DSTATE[2] = rtb_q_2;
  tricopter_simulation_rtw_DWork.FixPtUnitDelay1_DSTATE[3] = rtb_q_3;

  /* Update for DiscreteIntegrator: '<S4>/Velocity' incorporates:
   *  Sum: '<S13>/Sum1'
   *  Sum: '<S4>/Sum'
   */
  tricopter_simulation_rtw_DWork.Velocity_DSTATE[0] = ((rtb_rad2deg_s_idx -
    rtb_Init_idx) + rtb_Init_idx_1) * 0.005 +
    tricopter_simulation_rtw_DWork.Velocity_DSTATE[0];
  tricopter_simulation_rtw_DWork.Velocity_DSTATE[1] = ((rtb_rad2deg_s_idx_0 -
    rtb_Product_e_idx) + rtb_Init_idx_2) * 0.005 +
    tricopter_simulation_rtw_DWork.Velocity_DSTATE[1];
  tricopter_simulation_rtw_DWork.Velocity_DSTATE[2] = ((rtb_rad2deg_s_idx_1 -
    rtb_Init_idx_0) + rtb_enable_acc_idx) * 0.005 +
    tricopter_simulation_rtw_DWork.Velocity_DSTATE[2];

  /* Update for DiscreteIntegrator: '<S4>/Position' incorporates:
   *  Product: '<S4>/enable_vel'
   */
  tricopter_simulation_rtw_DWork.Position_DSTATE[0] = 0.005 * tmp[0] +
    tricopter_simulation_rtw_DWork.Position_DSTATE[0];
  tricopter_simulation_rtw_DWork.Position_DSTATE[1] = 0.005 * tmp[1] +
    tricopter_simulation_rtw_DWork.Position_DSTATE[1];
  tricopter_simulation_rtw_DWork.Position_DSTATE[2] = 0.005 * tmp[2] +
    tricopter_simulation_rtw_DWork.Position_DSTATE[2];
}

/* Model initialize function */
void tricopter_simulation_rtwModelClass::initialize()
{
  /* Registration code */

  /* initialize non-finites */
  rt_InitInfAndNaN(sizeof(real_T));

  /* initialize real-time model */
  (void) memset((void *)tricopter_simulation_rtw_M, 0,
                sizeof(RT_MODEL_tricopter_simulation_r));

  /* block I/O */
  {
    tricopter_simulation_rtw_B.DLookupTable = 0.0;
    tricopter_simulation_rtw_B.DLookupTable_b = 0.0;
    tricopter_simulation_rtw_B.DLookupTable_h = 0.0;
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
  tricopter_simulation_rtw_DWork.UnitDelay_DSTATE_g = 0.0;
  tricopter_simulation_rtw_DWork.UnitDelay1_DSTATE_n = 0.0;
  tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator1_DSTAT_m = 0.0;
  tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator_DSTATE_m = 0.0;
  tricopter_simulation_rtw_DWork.Delay_DSTATE_j = 0.0;
  tricopter_simulation_rtw_DWork.UnitDelay_DSTATE_n = 0.0;
  tricopter_simulation_rtw_DWork.UnitDelay1_DSTATE_j = 0.0;
  tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator1_DSTAT_i = 0.0;
  tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator_DSTATE_h = 0.0;
  tricopter_simulation_rtw_DWork.Delay_DSTATE_c = 0.0;
  tricopter_simulation_rtw_DWork.UnitDelay_DSTATE_i = 0.0;
  tricopter_simulation_rtw_DWork.UnitDelay1_DSTATE_nn = 0.0;
  tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator1_DSTAT_f = 0.0;
  tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator_DSTATE_e = 0.0;
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

  /* InitializeConditions for Delay: '<S38>/Delay' */
  tricopter_simulation_rtw_DWork.Delay_DSTATE = 1.0;

  /* InitializeConditions for UnitDelay: '<S51>/Unit Delay' */
  tricopter_simulation_rtw_DWork.UnitDelay_DSTATE_g = 1500.0;

  /* InitializeConditions for DiscreteIntegrator: '<S51>/Discrete-Time Integrator' */
  tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator_DSTATE_m = 1500.0;

  /* InitializeConditions for Delay: '<S36>/Delay' */
  tricopter_simulation_rtw_DWork.Delay_DSTATE_j = 1.0;

  /* InitializeConditions for Delay: '<S37>/Delay' */
  tricopter_simulation_rtw_DWork.Delay_DSTATE_c = 1.0;

  /* InitializeConditions for UnitDelay: '<S24>/FixPt Unit Delay2' */
  tricopter_simulation_rtw_DWork.FixPtUnitDelay2_DSTATE = 1U;

  /* Enable for DiscreteIntegrator: '<S50>/Discrete-Time Integrator1' */
  tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator1_SYSTEM_ = 1U;

  /* Enable for DiscreteIntegrator: '<S50>/Discrete-Time Integrator' */
  tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator_SYSTEM_E = 1U;

  /* Enable for DiscreteIntegrator: '<S51>/Discrete-Time Integrator1' */
  tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator1_SYSTE_a = 1U;

  /* Enable for DiscreteIntegrator: '<S51>/Discrete-Time Integrator' */
  tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator_SYSTEM_i = 1U;

  /* Enable for DiscreteIntegrator: '<S48>/Discrete-Time Integrator1' */
  tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator1_SYSTE_o = 1U;

  /* Enable for DiscreteIntegrator: '<S48>/Discrete-Time Integrator' */
  tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator_SYSTEM_k = 1U;

  /* Enable for DiscreteIntegrator: '<S49>/Discrete-Time Integrator1' */
  tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator1_SYSTE_m = 1U;

  /* Enable for DiscreteIntegrator: '<S49>/Discrete-Time Integrator' */
  tricopter_simulation_rtw_DWork.DiscreteTimeIntegrator_SYSTE_ka = 1U;
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
