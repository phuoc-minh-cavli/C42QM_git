#ifndef __FLASH_LOG_H__
#define __FLASH_LOG_H__

/*=======================================================================
* FILE:        flash_log.h
*
* SERVICES:    Routines to support flash logging
*
* DESCRIPTION :
*
* Copyright (c) 2010 Qualcomm Technologies Incorporated.
* All Rights Reserved.
* QUALCOMM Confidential and Proprietary
*=======================================================================*/

/*===================================================================
*
*                       EDIT HISTORY FOR FILE
*
*   This section contains comments describing changes made to the
*   module. Notice that changes are listed in reverse chronological
*   order.
*
*
* when         who     what, where, why
* ----------   ---     ----------------------------------------------
* 07/12/10     nr      Initial version
*==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

/* Include in <stdio.h> when compiling for off target tool */
#ifdef BUILD_FOR_X86
  #include <stdio.h>
  #include <stdlib.h>
#endif

void jprintf(const char* format, ...);

/* -----------------------------------------------------------------------
** Macros for debug print output
** ----------------------------------------------------------------------- */
#if !defined(BUILD_NAND_WITH_OEMSBL) && !defined(BUILD_FOR_X86)
  /* When compiled with TNOISY, these statements will print depending
   * on level of TNOISY.  Only changeable at compile time
   */
  #ifdef TNOISY
    #define FLASHLOG(l, x) if (TNOISY >= l) {jprintf x;}
  #else
    #define FLASHLOG(l, x) /**/
  #endif /* NOISY */

#elif defined(BUILD_FOR_X86)

  #define FLASHLOG(l, x) {printf x;}

#else
  #define FLASHLOG(l, x)
#endif

#endif /* __FLASH_LOG_H__ */
