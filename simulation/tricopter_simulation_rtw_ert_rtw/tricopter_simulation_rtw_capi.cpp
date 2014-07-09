/*
 * File: tricopter_simulation_rtw_capi.cpp
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

#include "tricopter_simulation_rtw.h"
#include "rtw_capi.h"
#include "tricopter_simulation_rtw_private.h"

/* Block output signal information */
static rtwCAPI_Signals rtBlockSignals[] = {
  /* addrMapIndex, sysNum, blockPath,
   * signalName, portNumber, dataTypeIndex, dimIndex, fxpIndex, sTimeIndex
   */
  { 0, 0, "tricopter_simulation_rtw/Tricopter/Motordynamics M1/1-D Lookup Table",
    "", 0, 0, 0, 0, 0 },

  { 1, 0, "tricopter_simulation_rtw/Tricopter/Motordynamics M2/1-D Lookup Table",
    "", 0, 0, 0, 0, 0 },

  { 2, 0, "tricopter_simulation_rtw/Tricopter/Motordynamics M3/1-D Lookup Table",
    "", 0, 0, 0, 0, 0 },

  { 3, 0, "tricopter_simulation_rtw/Tricopter/X-Moment/Gain",
    "", 0, 0, 0, 0, 1 },

  { 4, 0, "tricopter_simulation_rtw/Tricopter/X-Moment/Sqrt",
    "", 0, 0, 0, 0, 1 },

  { 5, 0, "tricopter_simulation_rtw/Tricopter/Y-Moment/Gain",
    "", 0, 0, 0, 0, 1 },

  { 6, 0,
    "tricopter_simulation_rtw/EQM/Variable Mass 6DoF (Quaternion)/Calulate omega_dot/Selector",
    "", 0, 0, 1, 0, 1 },

  { 7, 0,
    "tricopter_simulation_rtw/EQM/Variable Mass 6DoF (Quaternion)/Calulate omega_dot/Selector1",
    "", 0, 0, 1, 0, 1 },

  { 8, 0,
    "tricopter_simulation_rtw/EQM/Variable Mass 6DoF (Quaternion)/Calulate omega_dot/Selector2",
    "", 0, 0, 1, 0, 1 },

  { 9, 0,
    "tricopter_simulation_rtw/EQM/Variable Mass 6DoF (Quaternion)/Detremine Force,  Mass & Inertia/Matrix Concatenation",
    "", 0, 0, 2, 0, 1 },

  { 10, 0,
    "tricopter_simulation_rtw/EQM/Variable Mass 6DoF (Quaternion)/Subsystem/euler2quad/1//2",
    "", 0, 0, 3, 0, 1 },

  { 11, 0,
    "tricopter_simulation_rtw/EQM/Variable Mass 6DoF (Quaternion)/Subsystem/euler2quad/CCC",
    "", 0, 0, 0, 0, 1 },

  { 12, 0,
    "tricopter_simulation_rtw/EQM/Variable Mass 6DoF (Quaternion)/Subsystem/euler2quad/CCS",
    "", 0, 0, 0, 0, 1 },

  { 13, 0,
    "tricopter_simulation_rtw/EQM/Variable Mass 6DoF (Quaternion)/Subsystem/euler2quad/CSC",
    "", 0, 0, 0, 0, 1 },

  { 14, 0,
    "tricopter_simulation_rtw/EQM/Variable Mass 6DoF (Quaternion)/Subsystem/euler2quad/CSS",
    "", 0, 0, 0, 0, 1 },

  { 15, 0,
    "tricopter_simulation_rtw/EQM/Variable Mass 6DoF (Quaternion)/Subsystem/euler2quad/SCC",
    "", 0, 0, 0, 0, 1 },

  { 16, 0,
    "tricopter_simulation_rtw/EQM/Variable Mass 6DoF (Quaternion)/Subsystem/euler2quad/SCS",
    "", 0, 0, 0, 0, 1 },

  { 17, 0,
    "tricopter_simulation_rtw/EQM/Variable Mass 6DoF (Quaternion)/Subsystem/euler2quad/SSC",
    "", 0, 0, 0, 0, 1 },

  { 18, 0,
    "tricopter_simulation_rtw/EQM/Variable Mass 6DoF (Quaternion)/Subsystem/euler2quad/SSS",
    "", 0, 0, 0, 0, 1 },

  { 19, 0,
    "tricopter_simulation_rtw/EQM/Variable Mass 6DoF (Quaternion)/Subsystem/euler2quad/Sum1",
    "", 0, 0, 0, 0, 1 },

  { 20, 0,
    "tricopter_simulation_rtw/EQM/Variable Mass 6DoF (Quaternion)/Subsystem/euler2quad/Sum2",
    "", 0, 0, 0, 0, 1 },

  { 21, 0,
    "tricopter_simulation_rtw/EQM/Variable Mass 6DoF (Quaternion)/Subsystem/euler2quad/Sum3",
    "", 0, 0, 0, 0, 1 },

  { 22, 0,
    "tricopter_simulation_rtw/EQM/Variable Mass 6DoF (Quaternion)/Subsystem/euler2quad/Sum9",
    "", 0, 0, 0, 0, 1 },

  { 23, 0,
    "tricopter_simulation_rtw/EQM/Variable Mass 6DoF (Quaternion)/Subsystem/euler2quad/cos(phi//2)",
    "", 0, 0, 0, 0, 1 },

  { 24, 0,
    "tricopter_simulation_rtw/EQM/Variable Mass 6DoF (Quaternion)/Subsystem/euler2quad/cos(psi//2)",
    "", 0, 0, 0, 0, 1 },

  { 25, 0,
    "tricopter_simulation_rtw/EQM/Variable Mass 6DoF (Quaternion)/Subsystem/euler2quad/cos(theta//2)",
    "", 0, 0, 0, 0, 1 },

  { 26, 0,
    "tricopter_simulation_rtw/EQM/Variable Mass 6DoF (Quaternion)/Subsystem/euler2quad/sin(phi//2)",
    "", 0, 0, 0, 0, 1 },

  { 27, 0,
    "tricopter_simulation_rtw/EQM/Variable Mass 6DoF (Quaternion)/Subsystem/euler2quad/sin(psi//2)",
    "", 0, 0, 0, 0, 1 },

  { 28, 0,
    "tricopter_simulation_rtw/EQM/Variable Mass 6DoF (Quaternion)/Subsystem/euler2quad/sin(theta//2)",
    "", 0, 0, 0, 0, 1 },

  {
    0, 0, (NULL), (NULL), 0, 0, 0, 0, 0
  }
};

/* Individual block tuning is not valid when inline parameters is *
 * selected. An empty map is produced to provide a consistent     *
 * interface independent  of inlining parameters.                 *
 */
static rtwCAPI_BlockParameters rtBlockParameters[] = {
  /* addrMapIndex, blockPath,
   * paramName, dataTypeIndex, dimIndex, fixPtIdx
   */
  {
    0, (NULL), (NULL), 0, 0, 0
  }
};

/* Tunable variable parameters */
static rtwCAPI_ModelParameters rtModelParameters[] = {
  /* addrMapIndex, varName, dataTypeIndex, dimIndex, fixPtIndex */
  { 0, (NULL), 0, 0, 0 }
};

/* Initialize Data Address */
static void tricopter_simulation_rtw_InitializeDataAddr(void* dataAddr[],
  BlockIO_tricopter_simulation_rt *tricopter_simulation_rtw_B)
{
  dataAddr[0] = (void*) (&tricopter_simulation_rtw_B->DLookupTable);
  dataAddr[1] = (void*) (&tricopter_simulation_rtw_B->DLookupTable_b);
  dataAddr[2] = (void*) (&tricopter_simulation_rtw_B->DLookupTable_h);
  dataAddr[3] = (void*) (&tricopter_simulation_rtw_ConstB.Gain);
  dataAddr[4] = (void*) (&tricopter_simulation_rtw_ConstB.Sqrt);
  dataAddr[5] = (void*) (&tricopter_simulation_rtw_ConstB.Gain_a);
  dataAddr[6] = (void*) (&tricopter_simulation_rtw_ConstB.Selector[0]);
  dataAddr[7] = (void*) (&tricopter_simulation_rtw_ConstB.Selector1[0]);
  dataAddr[8] = (void*) (&tricopter_simulation_rtw_ConstB.Selector2[0]);
  dataAddr[9] = (void*) (&tricopter_simulation_rtw_ConstB.MatrixConcatenation[0]);
  dataAddr[10] = (void*) (&tricopter_simulation_rtw_ConstB.u[0]);
  dataAddr[11] = (void*) (&tricopter_simulation_rtw_ConstB.CCC);
  dataAddr[12] = (void*) (&tricopter_simulation_rtw_ConstB.CCS);
  dataAddr[13] = (void*) (&tricopter_simulation_rtw_ConstB.CSC);
  dataAddr[14] = (void*) (&tricopter_simulation_rtw_ConstB.CSS);
  dataAddr[15] = (void*) (&tricopter_simulation_rtw_ConstB.SCC);
  dataAddr[16] = (void*) (&tricopter_simulation_rtw_ConstB.SCS);
  dataAddr[17] = (void*) (&tricopter_simulation_rtw_ConstB.SSC);
  dataAddr[18] = (void*) (&tricopter_simulation_rtw_ConstB.SSS);
  dataAddr[19] = (void*) (&tricopter_simulation_rtw_ConstB.Sum1);
  dataAddr[20] = (void*) (&tricopter_simulation_rtw_ConstB.Sum2);
  dataAddr[21] = (void*) (&tricopter_simulation_rtw_ConstB.Sum3);
  dataAddr[22] = (void*) (&tricopter_simulation_rtw_ConstB.Sum9);
  dataAddr[23] = (void*) (&tricopter_simulation_rtw_ConstB.cosphi2);
  dataAddr[24] = (void*) (&tricopter_simulation_rtw_ConstB.cospsi2);
  dataAddr[25] = (void*) (&tricopter_simulation_rtw_ConstB.costheta2);
  dataAddr[26] = (void*) (&tricopter_simulation_rtw_ConstB.sinphi2);
  dataAddr[27] = (void*) (&tricopter_simulation_rtw_ConstB.sinpsi2);
  dataAddr[28] = (void*) (&tricopter_simulation_rtw_ConstB.sintheta2);
}

/* Initialize Data Run-Time Dimension Buffer Address */
static void tricopter_simulation_rtw_InitializeVarDimsAddr(int32_T* vardimsAddr[])
{
  vardimsAddr[0] = (NULL);
}

/* Data Type Map - use dataTypeMapIndex to access this structure */
static rtwCAPI_DataTypeMap rtDataTypeMap[] = {
  /* cName, mwName, numElements, elemMapIndex, dataSize, slDataId, *
   * isComplex, isPointer */
  { "double", "real_T", 0, 0, sizeof(real_T), SS_DOUBLE, 0, 0 }
};

/* Structure Element Map - use elemMapIndex to access this structure */
static rtwCAPI_ElementMap rtElementMap[] = {
  /* elementName, elementOffset, dataTypeIndex, dimIndex, fxpIndex */
  { (NULL), 0, 0, 0, 0 },
};

/* Dimension Map - use dimensionMapIndex to access elements of ths structure*/
static rtwCAPI_DimensionMap rtDimensionMap[] = {
  /* dataOrientation, dimArrayIndex, numDims, vardimsIndex */
  { rtwCAPI_SCALAR, 0, 2, 0 },

  { rtwCAPI_MATRIX_COL_MAJOR, 2, 2, 0 },

  { rtwCAPI_MATRIX_COL_MAJOR, 4, 2, 0 },

  { rtwCAPI_MATRIX_COL_MAJOR, 6, 2, 0 }
};

/* Dimension Array- use dimArrayIndex to access elements of this array */
static uint_T rtDimensionArray[] = {
  1,                                   /* 0 */
  1,                                   /* 1 */
  3,                                   /* 2 */
  3,                                   /* 3 */
  6,                                   /* 4 */
  3,                                   /* 5 */
  3,                                   /* 6 */
  1                                    /* 7 */
};

/* C-API stores floating point values in an array. The elements of this  *
 * are unique. This ensures that values which are shared across the model*
 * are stored in the most efficient way. These values are referenced by  *
 *           - rtwCAPI_FixPtMap.fracSlopePtr,                            *
 *           - rtwCAPI_FixPtMap.biasPtr,                                 *
 *           - rtwCAPI_SampleTimeMap.samplePeriodPtr,                    *
 *           - rtwCAPI_SampleTimeMap.sampleOffsetPtr                     */
static const real_T rtcapiStoredFloats[] = {
  0.005, 0.0
};

/* Fixed Point Map */
static rtwCAPI_FixPtMap rtFixPtMap[] = {
  /* fracSlopePtr, biasPtr, scaleType, wordLength, exponent, isSigned */
  { (NULL), (NULL), rtwCAPI_FIX_RESERVED, 0, 0, 0 },
};

/* Sample Time Map - use sTimeIndex to access elements of ths structure */
static rtwCAPI_SampleTimeMap rtSampleTimeMap[] = {
  /* samplePeriodPtr, sampleOffsetPtr, tid, samplingMode */
  { (const void *) &rtcapiStoredFloats[0], (const void *) &rtcapiStoredFloats[1],
    0, 0 },

  { (NULL), (NULL), -2, 0 }
};

static rtwCAPI_ModelMappingStaticInfo mmiStatic = {
  /* Signals:{signals, numSignals},
   * Params: {blockParameters, numBlockParameters,
   *          modelParameters, numModelParameters},
   * States: {states, numStates},
   * Root Inputs: {rootInputs, numRootInputs}
   * Root Outputs: {rootOutputs, numRootOutputs}
   * Maps:   {dataTypeMap, dimensionMap, fixPtMap,
   *          elementMap, sampleTimeMap, dimensionArray},
   * TargetType: targetType
   */
  { rtBlockSignals, 29,
    (NULL), 0,
    (NULL), 0 },

  { rtBlockParameters, 0,
    rtModelParameters, 0 },

  { (NULL), 0 },

  { rtDataTypeMap, rtDimensionMap, rtFixPtMap,
    rtElementMap, rtSampleTimeMap, rtDimensionArray },
  "float", (NULL)
};

/* Function to get C API Model Mapping Static Info */
const rtwCAPI_ModelMappingStaticInfo*
  tricopter_simulation_rtw_GetCAPIStaticMap()
{
  return &mmiStatic;
}

/* Cache pointers into DataMapInfo substructure of RTModel */
void tricopter_simulation_rtw_InitializeDataMapInfo
  (RT_MODEL_tricopter_simulation_r *const tricopter_simulation_rtw_M,
   BlockIO_tricopter_simulation_rt *tricopter_simulation_rtw_B)
{
  /* Set C-API version */
  rtwCAPI_SetVersion(tricopter_simulation_rtw_M->DataMapInfo.mmi, 1);

  /* Cache static C-API data into the Real-time Model Data structure */
  rtwCAPI_SetStaticMap(tricopter_simulation_rtw_M->DataMapInfo.mmi, &mmiStatic);

  /* Cache static C-API logging data into the Real-time Model Data structure */
  rtwCAPI_SetLoggingStaticMap(tricopter_simulation_rtw_M->DataMapInfo.mmi, (NULL));

  /* Cache C-API Data Addresses into the Real-Time Model Data structure */
  tricopter_simulation_rtw_InitializeDataAddr
    (tricopter_simulation_rtw_M->DataMapInfo.dataAddress,
     tricopter_simulation_rtw_B);
  rtwCAPI_SetDataAddressMap(tricopter_simulation_rtw_M->DataMapInfo.mmi,
    tricopter_simulation_rtw_M->DataMapInfo.dataAddress);

  /* Cache C-API Data Run-Time Dimension Buffer Addresses into the Real-Time Model Data structure */
  tricopter_simulation_rtw_InitializeVarDimsAddr
    (tricopter_simulation_rtw_M->DataMapInfo.vardimsAddress);
  rtwCAPI_SetVarDimsAddressMap(tricopter_simulation_rtw_M->DataMapInfo.mmi,
    tricopter_simulation_rtw_M->DataMapInfo.vardimsAddress);

  /* Set Instance specific path */
  rtwCAPI_SetPath(tricopter_simulation_rtw_M->DataMapInfo.mmi, (NULL));
  rtwCAPI_SetFullPath(tricopter_simulation_rtw_M->DataMapInfo.mmi, (NULL));

  /* Cache the instance C-API logging pointer */
  rtwCAPI_SetInstanceLoggingInfo(tricopter_simulation_rtw_M->DataMapInfo.mmi,
    (NULL));

  /* Set reference to submodels */
  rtwCAPI_SetChildMMIArray(tricopter_simulation_rtw_M->DataMapInfo.mmi, (NULL));
  rtwCAPI_SetChildMMIArrayLen(tricopter_simulation_rtw_M->DataMapInfo.mmi, 0);
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
