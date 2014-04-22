/*!
  @file mutils_common.h

  @brief
  The commmon header file for MODEM UTILS

*/

/*==============================================================================

  Copyright (c) 2009 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/16/16   cx      Initial version  
==============================================================================*/

#ifndef MUTILS_COMMON_H
#define MUTILS_COMMON_H

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/
#include <comdef.h>
#include <sys.h>
#include <err.h>
/*==============================================================================

                   MACROS 

==============================================================================*/  
#if defined(__GNUC__) && !defined(_lint)
  #define MUTILS_LIKELY_TRUE(x) __builtin_expect((x), 1)
  #define MUTILS_UNLIKELY_TRUE(x) __builtin_expect((x), 0)
#else
  #define MUTILS_LIKELY_TRUE(x) (x)
  #define MUTILS_UNLIKELY_TRUE(x) (x)
#endif /* ! (__GNUC__) */

#define MUTILS_ASSERT(exp)  \
  /*lint -save -e506 -e774 */ \
  if(MUTILS_UNLIKELY_TRUE(!(exp)) ) \
  /*lint -restore */ \
  { \
     ERR_FATAL( "Assert failed: " #exp, 0, 0, 0 ); \
  } 

/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/

/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

#endif /* MUTILS_COMMON_H */

