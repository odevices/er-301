/*
 * Copyright (C) 2005-2017 Texas Instruments Incorporated.
 *
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
/*
 *  ======== tistdtypes.h ========
 */

/*
 * These types are also defined by other TI components.  They are bracketed
 * with _TI_STD_TYPES to avoid warnings for duplicate definition.
 *
 * You may get warnings about duplicate type definitions when using this
 * header file with earlier versions of DSP/BIOS and CSL.
 *
 * You can use the '-pds303' compiler option to suppress these warnings.
 */
#ifndef _TI_STD_TYPES
#define _TI_STD_TYPES
#include <stdint.h>

/* If the CSL header files are getting included through the MAKEFILE builds we don't want
 * to include XDC TARGETS at all. */

#ifdef MAKEFILE_BUILD
#undef xdc_target_types__
#endif

#ifndef xdc_target_types__
/*
 * This '#ifndef STD_' is needed to protect from duplicate definitions
 * of Int, Uns, etc. in DSP/BIOS v4.x (e.g. 4.90, 4.80) since these versions
 * of DSP/BIOS or linux/include/ti/ipc/Std.h did not contain the
 * '#ifndef_TI_STD_TYPES' logic.
 */
#if !defined(STD_) && !defined(STD_H)

/*
 * Aliases for standard C types
 */
#ifndef Int
typedef int			Int;
#endif

#ifndef Uns
typedef unsigned		Uns;
#endif

#ifndef Char
typedef char			Char;
#endif

/* pointer to null-terminated character sequence */
#ifndef String
typedef char			*String;
#endif

#ifndef Ptr
typedef void			*Ptr;		/* pointer to arbitrary type */
#endif

#ifndef Bool
typedef unsigned short		Bool;		/* boolean */
#endif

/* Define TRUE/FALSE to go with Bool */
#ifndef TRUE

#define TRUE		((Bool) 1)
#define FALSE		((Bool) 0)

#endif

/* Define CSL_TRUE/CSL_FALSE to go with uint32_t */
#ifndef CSL_TRUE
#define CSL_TRUE    (1U)
#define CSL_FALSE   (0U)
#endif

#endif /* STD_ */

#ifndef NULL
#define NULL (0)
#endif

#ifndef NULL_PTR
#if defined (HOST_EMULATION)
/* Host emulation compilation throws below error when void * is used. So
 * retain as normal 0 comparision.
 *
 * error: invalid conversion from 'void*' to 'xxx *' [-fpermissive]
 */
#define NULL_PTR (NULL)
#else
#define NULL_PTR ((void *)0x0)
#endif
#endif

/* Unsigned integer definitions (32bit, 16bit, 8bit) follow... */
#ifndef Uint32
typedef uint32_t    Uint32;
#endif
#ifndef Uint16
typedef uint16_t    Uint16;
#endif
#ifndef Uint8
typedef uint8_t     Uint8;
#endif

/* Signed integer definitions (32bit, 16bit, 8bit) follow... */
#ifndef Int32
typedef int32_t     Int32;
#endif
#ifndef Int16
typedef int16_t     Int16;
#endif
#ifndef Int8
typedef int8_t      Int8;
#endif

/* Below typedefs aliases are needed for VPS Lib */
#ifndef UInt32
typedef uint32_t      UInt32;
#endif
#ifndef UInt16
typedef uint16_t      UInt16;
#endif
#ifndef UInt8
typedef uint8_t       UInt8;
#endif
#ifndef UInt64
typedef uint64_t      UInt64;
#endif
#ifndef Float32
typedef float         Float32;
#endif
#ifndef Double
typedef double        Double;
#endif
#ifndef Float64
typedef double        Float64;
#endif

#else
    #include <xdc/std.h>
#endif /* xdc_target_types__ */

#endif  /* _TI_STD_TYPES */

#ifndef TI_TYPES_SOC_ALIASES
#define TI_TYPES_SOC_ALIASES
    /* SOC Alias Defines for backward compatibility */
    #if defined(DEVICE_K2K)
    #ifndef SOC_K2K
    #define SOC_K2K
    #endif
    #elif defined(DEVICE_K2H)
    #ifndef SOC_K2H
    #define SOC_K2H
    #endif
    #elif defined(DEVICE_K2E)
    #ifndef SOC_K2E
    #define SOC_K2E
    #endif
    #elif defined(DEVICE_K2L)
    #ifndef SOC_K2L
    #define SOC_K2L
    #endif
    /* Temporary to be removed */
    #elif defined(DEVICE_K2G)
    #ifndef SOC_K2G
    #define SOC_K2G
    #endif
    #elif defined(DEVICE_AM574x)
    #ifndef SOC_AM574x
    #define SOC_AM574x
    #endif
    #elif defined(DEVICE_AM572x)
    #ifndef SOC_AM572x
    #define SOC_AM572x
    #endif
    #elif defined(DEVICE_AM571x)
    #ifndef SOC_AM571x
    #define SOC_AM571x
    #endif
    #elif defined(DEVICE_C6678)
    #ifndef SOC_C6678
    #define SOC_C6678
    #endif
    #elif defined(DEVICE_C6657)
    #ifndef SOC_C6657
    #define SOC_C6657
    #endif

    #endif /* DEVICE_XXXXX */
#endif
