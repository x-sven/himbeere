// Copyright 2007-2010 The MathWorks, Inc.
/* Generated by genpubheader from export/include/cgir_construct/rtw_api/impl/codeconstructor_c_api.hpp. */

#ifdef SUPPORTS_PRAGMA_ONCE
#pragma once
#endif

#ifndef SFUN_CODECONSTRUCTOR_C_API_HPP
#define SFUN_CODECONSTRUCTOR_C_API_HPP

#include "../libmwcgir_construct.hpp"
#include "../fwd.hpp"
#include "simstruc.h" 
#include "fixedpoint.h"

namespace RTW
{
   
    extern "C"
    {
       
        CGIR_CONSTRUCT_API void SFun_Block_dtor(SFun_CodeCtor_Impl* pThis);

        CGIR_CONSTRUCT_API SFun_Reference_Impl* SFun_Block_createLocal_T_c(
                                SFun_CodeCtor_Impl* pThis,
                                const SFun_Type_Impl* pType,
                                const char* name);

        CGIR_CONSTRUCT_API SFun_Value_Impl* SFun_Block_input_i(
                                SFun_CodeCtor_Impl* pThis,
                                int index);

        CGIR_CONSTRUCT_API SFun_Reference_Impl* SFun_Block_output_i(
                                SFun_CodeCtor_Impl* pThis,
                                int index);

        CGIR_CONSTRUCT_API int SFun_CodeCtor_numInputs(
                                int* pResult,
                                const SFun_CodeCtor_Impl* pThis);
       
        CGIR_CONSTRUCT_API int SFun_CodeCtor_numOutputs(
                                int* pResult,
                                const SFun_CodeCtor_Impl* pThis);
    }
}

#endif
