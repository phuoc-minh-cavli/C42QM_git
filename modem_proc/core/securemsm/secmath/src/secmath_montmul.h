/*===========================================================================

                         S E C U R I T Y    S E R V I C E S

                S E C M A T H   R S A   I M P L E M E N T A T I O N
                              
FILE:  secmath_montmul.h

DESCRIPTION:

DEPENDENCIES/ASSUMPTIONS

EXTERNALIZED FUNCTIONS             

    Copyright (c) 2011 Qualcomm Technologies, Inc.
    All rights reserved.
    Qualcomm Technologies, Inc. Confidential and Proprietary.
===========================================================================*/

/*=========================================================================

                            EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.10/securemsm/secmath/src/secmath_montmul.h#2 $
  $DateTime: 2018/11/08 03:29:06 $
  $Author: pwbldsvc $

when       who            what, where, why
--------   ----           ---------------------------------------------------
05/11/11   vg             Initial Revision

===========================================================================*/



#ifndef _MONTMUL_H
#define _MONTMUL_H

/*-conditional code compilation for Montgomery multiplication are defined here -*/

//select only one of the 4 below algorithms (only REF is provided to QCT)
//#define USE_MONTMUL_NEON
#define USE_MONTMUL_REF
//#define USE_MONTMUL_SEP
//#define USE_MONTMUL_HYB


// commented out as inline assembly has been deprecated by ARM #define USE_ARMASM   
#ifdef USE_ARMASM
//ensure the code is built in arm mode as you can't use inline assembly in
//thumb mode
#pragma arm
#endif
//#define DEBUG_MONTMUL
/*- end of conditional code compilation ---------------------------------------*/


#ifdef __GNUC__
#define restrict __restrict__
#else
#define restrict
#endif

#include "comdef.h"
#include <stddef.h> //for size_t

#ifdef  __cplusplus
extern "C" {
#endif
#if 0
} /* keep emacs from indenting all the stuff below */
#endif

int mult_R_mod_m(uint32 * restrict z, const uint32 *a, const uint32 *m, const size_t N);
int montmul_one_ref(uint32 * restrict z, const uint32 *a, const uint32 *m, size_t N, uint32 m0_1);
int montmul_ref(uint32 * restrict z, const uint32 *a, const uint32 *b, const uint32 *m, const size_t N, uint32 m0_1);

#ifdef __cplusplus
}
#endif

//the minimum length of data arrays
#define MINIMUM_N 2

#ifdef DEBUG_MONTMUL
    #define DEBUG_PRINT printf
    #define ASSERT assert
    #include <assert.h>
#else 
    #define DEBUG_PRINT(...) 
    //#define ASSERT(...)
#endif //DEBUG_MONTMUL


#endif //_MONTMUL_H
