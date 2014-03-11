/*
 * File: tricopter_simulation_rtw_data.cpp
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

#include "tricopter_simulation_rtw.h"
#include "tricopter_simulation_rtw_private.h"

/* Invariant block signals (auto storage) */
const ConstBlockIO_tricopter_simulati tricopter_simulation_rtw_ConstB = {
  {
    0.0,
    0.0,
    0.0
  }
  ,                                    /* '<S23>/1//2' */
  1.0
  ,                                    /* '<S23>/cos(phi//2)' */
  1.0
  ,                                    /* '<S23>/cos(theta//2)' */
  1.0
  ,                                    /* '<S23>/cos(psi//2)' */
  1.0
  ,                                    /* '<S23>/CCC' */
  0.0
  ,                                    /* '<S23>/sin(phi//2)' */
  0.0
  ,                                    /* '<S23>/sin(theta//2)' */
  0.0
  ,                                    /* '<S23>/sin(psi//2)' */
  0.0
  ,                                    /* '<S23>/SSS' */
  1.0
  ,                                    /* '<S23>/Sum9' */
  0.0
  ,                                    /* '<S23>/SCC' */
  0.0
  ,                                    /* '<S23>/CSS' */
  0.0
  ,                                    /* '<S23>/Sum2' */
  0.0
  ,                                    /* '<S23>/CSC' */
  0.0
  ,                                    /* '<S23>/SCS' */
  0.0
  ,                                    /* '<S23>/Sum1' */
  0.0
  ,                                    /* '<S23>/CCS' */
  0.0
  ,                                    /* '<S23>/SSC' */
  0.0
  ,                                    /* '<S23>/Sum3' */

  {
    0.0625,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0625,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.125,
    0.0,
    0.0,
    0.0
  }
  ,                                    /* '<S15>/Matrix Concatenation' */

  {
    0.0625,
    0.0,
    0.0,
    0.0,
    0.0625,
    0.0,
    0.0,
    0.0,
    0.125
  }
  ,                                    /* '<S13>/Selector' */
  0.75
  ,                                    /* '<S43>/Gain' */
  0.8660254037844386
  ,                                    /* '<S43>/Sqrt' */
  0.25
  ,                                    /* '<S45>/Gain' */
  0.75
  ,                                    /* '<S45>/Gain1' */
  0.8660254037844386
  ,                                    /* '<S45>/Sqrt' */

  {
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0
  }
  ,                                    /* '<S13>/Selector1' */

  {
    0.0625,
    0.0,
    0.0,
    0.0,
    0.0625,
    0.0,
    0.0,
    0.0,
    0.125
  }
  /* '<S13>/Selector2' */
};

/* Constant parameters (auto storage) */
const ConstParam_tricopter_simulation tricopter_simulation_rtw_ConstP = {
  /* Expression: [-60 60]*pi/180
   * Referenced by: '<S46>/1-D Lookup Table'
   */
  { -1.0471975511965976, 1.0471975511965976 },

  /* Pooled Parameter (Expression: [900 2100])
   * Referenced by:
   *   '<S37>/1-D Lookup Table'
   *   '<S38>/1-D Lookup Table'
   *   '<S39>/1-D Lookup Table'
   *   '<S46>/1-D Lookup Table'
   */
  { 900.0, 2100.0 },

  /* Pooled Parameter (Expression: [0 7000]*pi/30)
   * Referenced by:
   *   '<S37>/1-D Lookup Table'
   *   '<S38>/1-D Lookup Table'
   *   '<S39>/1-D Lookup Table'
   */
  { 0.0, 733.03828583761833 }
};

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
