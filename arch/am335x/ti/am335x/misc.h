/**
 * \file   misc.h
 *
 * \brief  This file contains the prototypes of the functions present in
 *         utils/misc.c
 *
 * 		   Also it has commonly used miscellaneous macros.
 *
 *  \copyright Copyright (C) 2013 Texas Instruments Incorporated - 
 *             http://www.ti.com/
 */

/*
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef MISC_H_
#define MISC_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/** \brief Round-off division value to upper integer. The parameters passed 
 *         for this macro should be greater than 1. 
 */
#define DIV_CEIL(a, b) (((a) + (b)-1) / (b))

/** \brief Deprecated function - Compiler message */
#ifdef __GNUC__
/**  \brief Deprecated function macro for gnuc compiler */
#define DEPRECATED(func) func __attribute__((deprecated))
#else
/**  \brief Deprecated function macro for non gnuc compilers */
#define DEPRECATED(func) func
#endif

/** \brief Expect condition result for code optimization (by compiler). */
#ifdef __GNUC__
/** \brief Expect pass macro for gnuc compiler */
#define EXPECT_PASS(cond) __builtin_expect(!!(cond), 1)
/** \brief Expect fail macro for gnuc compiler */
#define EXPECT_FAIL(cond) __builtin_expect(!!(cond), 0)
#else
/** \brief Expect pass macro for non gnuc compilers */
#define EXPECT_PASS(cond) cond
/** \brief Expect fail macro for non gnuc compilers */
#define EXPECT_FAIL(cond) cond
#endif

/** \brief Compiler independent macro to align the placement of variable in 
  *        memory to the specified value in bytes. This macro is to be used 
  *        only for global variables and not for local variables. 
  *
  * Usage:
  *       ALIGN_VAR(demoVar, 100)
  *       uint32_t demoVar = 6;
  */
#if defined(__GNUC__)
#define ALIGN_VAR(var, align_val) __attribute__((aligned(align_val)))
#elif defined(__IAR_SYSTEMS_ICC__)
#define ALIGN_VAR(var, align_val) #pragma data_alignment = align_val
#elif defined(__TI_COMPILER_VERSION__)
#define ALIGN_VAR_PRAGMA(str) _Pragma(#str)
#define ALIGN_VAR(x, y) ALIGN_VAR_PRAGMA(DATA_ALIGN(x, y))
#else
#error "Alignment is not supported in this compiler"
#endif

/** ARM wait-for-interrupt instruction.
 *  Note: Need to be moved to arch specific file.
 */
#define wfi() asm("   WFI");

/**
 * @defgroup MEMORY_SIZE Commonly used memory size macros
 *
 * @{
 */
/** Memory size 1 byte */
#define MEM_SIZE_1B (1U)
/** Memory size 1 kilo-byte */
#define MEM_SIZE_KB (1024U * MEM_SIZE_1B)
/** Memory size 1 mega-byte */
#define MEM_SIZE_MB (1024U * MEM_SIZE_KB)
/** Memory size 1 gega-byte */
#define MEM_SIZE_GB (1024U * MEM_SIZE_MB)
/** @} */

/**
 * @defgroup FREQUENCY Commonly used frequency macros
 *
 * @{
 */
/** Frequency macros */
/** Frequency in kilo-hertz */
#define FREQ_KHZ (1000U)
/** Frequency in mega-hertz */
#define FREQ_MHZ (1000U * FREQ_KHZ)
/** Frequency in giga-hertz */
#define FREQ_GHZ (1000U * FREQ_MHZ)
  /** @} */

  /* ========================================================================== */
  /*                          Function Declarations                             */
  /* ========================================================================== */

  /**
 * \brief   This function adds two bcd numbers
 *
 * \param    bcd1 bcd number to be added
 *
 * \param    bcd2 bcd number to be added
 *
 * \retval   bcd sum value
 **/
  uint16_t bcdAdd(uint8_t bcd1, uint8_t bcd2);

  /**
 * \brief   This function adds two time values
 *
 * \param   time1 Time value at instance1
 *
 * \param   time2 Time value at instance2
 *
 * \param   pDate Date value to capture the time addition overflow 
 *
 * \retval  time sum value
 **/
  uint32_t addTime(uint32_t time1, uint32_t time2, uint32_t *pDate);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* MISC_H_ */
