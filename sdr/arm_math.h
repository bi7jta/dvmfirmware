/**
* Digital Voice Modem - DSP Firmware
* GPLv2 Open Source. Use is subject to license terms.
* DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
*
* @package DVM / DSP Firmware
*
*/
//
// Based on code from the MMDVM-SDR project. (https://github.com/r4d10n/mmdvm-sdr)
// Licensed under the GPLv2 License (https://opensource.org/licenses/GPL-2.0)
//
/*
*   Copyright (C) 2018 by Adrian Musceac YO8RZZ
*
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software
*   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
#if !defined(__ARM_MATH_H__)
#define __ARM_MATH_H__

#include <cstddef>

// ---------------------------------------------------------------------------
//  Types
// ---------------------------------------------------------------------------

#ifndef _INT8_T_DECLARED
#ifndef __INT8_TYPE__
typedef signed char         int8_t;
#endif // __INT8_TYPE__
#endif // _INT8_T_DECLARED
#ifndef _INT16_T_DECLARED
#ifndef __INT16_TYPE__
typedef short               int16_t;
#endif // __INT16_TYPE__
#endif // _INT16_T_DECLARED
#ifndef _INT32_T_DECLARED
#ifndef __INT32_TYPE__
typedef int                 int32_t;
#endif // __INT32_TYPE__
#endif // _INT32_T_DECLARED
#ifndef _INT64_T_DECLARED
#ifndef __INT64_TYPE__
typedef long long           int64_t;
#endif // __INT64_TYPE__
#endif // _INT64_T_DECLARED
#ifndef _UINT8_T_DECLARED
#ifndef __UINT8_TYPE__
typedef unsigned char       uint8_t;
#endif // __UINT8_TYPE__
#endif // _UINT8_T_DECLARED
#ifndef _UINT16_T_DECLARED
#ifndef __UINT16_TYPE__
typedef unsigned short      uint16_t;
#endif // __UINT16_TYPE__
#endif // _UINT16_T_DECLARED
#ifndef _UINT32_T_DECLARED
#ifndef __UINT32_TYPE__
typedef unsigned int        uint32_t;
#endif // __UINT32_TYPE__
#endif // _UINT32_T_DECLARED
#ifndef _UINT64_T_DECLARED
#ifndef __UINT64_TYPE__
typedef unsigned long long  uint64_t;
#endif // __UINT64_TYPE__
#endif // _UINT64_T_DECLARED

#ifndef __LONG64_TYPE__
typedef long long           long64_t;
#endif // __LONG64_TYPE__
#ifndef __ULONG64_TYPE__
typedef unsigned long long  ulong64_t;
#endif // __ULONG64_TYPE__

typedef int16_t             q15_t;
typedef int32_t             q31_t;
typedef int64_t             q63_t;

// ---------------------------------------------------------------------------
//  Structures
// ---------------------------------------------------------------------------

typedef struct {
    uint16_t numTaps;               /**< number of filter coefficients in the filter. */
    q15_t* pState;                  /**< points to the state variable array. The array is of length numTaps+blockSize-1. */
    q15_t* pCoeffs;                 /**< points to the coefficient array. The array is of length numTaps.*/
} arm_fir_instance_q15;

typedef struct {
    uint8_t L;                      /**< upsample factor. */
    uint16_t phaseLength;           /**< length of each polyphase filter component. */
    q15_t* pCoeffs;                 /**< points to the coefficient array. The array is of length L*phaseLength. */
    q15_t* pState;                  /**< points to the state variable array. The array is of length blockSize+phaseLength-1. */
} arm_fir_interpolate_instance_q15;

typedef struct {
    uint32_t numStages;             /**< number of 2nd order stages in the filter.  Overall order is 2*numStages. */
    q31_t* pState;                  /**< Points to the array of state coefficients.  The array is of length 4*numStages. */
    q31_t* pCoeffs;                 /**< Points to the array of coefficients.  The array is of length 5*numStages. */
    uint8_t postShift;              /**< Additional shift, in bits, applied to each output sample. */
} arm_biquad_casd_df1_inst_q31;

// ---------------------------------------------------------------------------
//  Macros
// ---------------------------------------------------------------------------

#define __SSAT(x, y)  ((x>32767)  ? 32767 : ((x < -32768) ? -32768 : x))

// ---------------------------------------------------------------------------
//  Global Functions
// ---------------------------------------------------------------------------

/// <summary>
/// Processing function for the Q15 FIR interpolator.
/// </summary>
/// <param name="S">An instance of the Q15 FIR interpolator structure.</param>
/// <param name="pSrc">Block of input data.</param>
/// <param name="pDst">Block of output data.</param>
/// <param name="blockSize">Number of input samples to process per call.</param>
void arm_fir_interpolate_q15(const arm_fir_interpolate_instance_q15* S, q15_t* pSrc, q15_t* pDst, uint32_t blockSize);

/// <summary>
/// Processing function for the fast Q15 FIR filter for Cortex-M3 and Cortex-M4.
/// </summary>
/// <param name="S">An instance of the Q15 FIR interpolator structure.</param>
/// <param name="pSrc">Block of input data.</param>
/// <param name="pDst">Block of output data.</param>
/// <param name="blockSize">Number of input samples to process per call.</param>
void arm_fir_fast_q15(const arm_fir_instance_q15* S, q15_t* pSrc, q15_t* pDst, uint32_t blockSize);

/// <summary>
/// Processing function for the Q31 Biquad cascade filter
/// </summary>
/// <param name="S">An instance of the Q31 Biquad cascade structure.</param>
/// <param name="pSrc">Block of input data.</param>
/// <param name="pDst">Block of output data.</param>
/// <param name="blockSize">Number of input samples to process.</param>
void arm_biquad_cascade_df1_q31(const arm_biquad_casd_df1_inst_q31* S, q31_t* pSrc, q31_t* pDst, uint32_t blockSize);

/// <summary>
/// Converts the elements of the Q15 vector to Q31 vector.
/// </summary>
/// <param name="pSrc">Input pointer.</param>
/// <param name="pDst">Output pointer.</param>
/// <param name="blockSize">Number of input samples to process.</param>
void arm_q15_to_q31(q15_t* pSrc, q31_t* pDst, uint32_t blockSize);

#endif // __ARM_MATH_H__
