#ifndef _POLICYMAN_DBG_H_
#define _POLICYMAN_DBG_H_

/**
  @file policyman_dbg.c

  @brief Policy Manager debug macros.
*/

/*
    Copyright (c) 2013-2015 QUALCOMM Technologies Incorporated.
    All Rights Reserved.
    Qualcomm Technologies Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by this
  document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

  $Header: //components/rel/mmcp.mpss/6.1.10/policyman/inc/policyman_dbg.h#1 $
  $DateTime: 2023/04/25 14:39:18 $
  $Author: pwbldsvc $
*/

#include "comdef.h"    /* Definition for basic types and macros */
#include <sys/types.h>
#include "err.h"       /* Interface to error log services */
#include "msg.h"       /* Interface to F3 messages services */
#include "policyman_i.h"
#include "policyman_util.h"


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
      Compile PM with debug information, asserts and messages.
---------------------------------------------------------------------------*/

#define POLICYMAN_MSG_SEPARATOR(which)    policyman_util_msg_separator(which)

/*---------------------------------------------------------------------------
                  POLICYMAN MSG macros
---------------------------------------------------------------------------*/

#if defined(TEST_FRAMEWORK)
#error code not present
#else /* +TEST_FRAMEWORK- */

// Leave message levels unchanged
#define PM_MSG_LVL_LOW     MSG_LEGACY_LOW
#define PM_MSG_LVL_MED     MSG_LEGACY_MED

// Disable extra messages on target
#define POLICYMAN_UT_MSG_0(str)
#define POLICYMAN_UT_MSG_1(str, a)
#define POLICYMAN_UT_MSG_2(str, a, b)
#define POLICYMAN_UT_MSG_3(str, a, b, c)
#define POLICYMAN_UT_MSG_4(str, a, b, c, d)
#define POLICYMAN_UT_MSG_5(str, a, b, c, d, e)
#define POLICYMAN_UT_MSG_6(str, a, b, c, d, e, f)
#define POLICYMAN_UT_MSG(str, a, b, c)

#define POLICYMAN_UT_MSG_S1(str, a)
#define POLICYMAN_UT_MSG_S2(str, a, b)
#define POLICYMAN_UT_MSG_S3(str, a, b, c)
#define POLICYMAN_UT_MSG_S4(str, a, b, c, d)
#define POLICYMAN_UT_MSG_S5(str, a, b, c, d, e)
#define POLICYMAN_UT_MSG_S6(str, a, b, c, d, e, f)

#endif /* -TEST_FRAMEWORK. */


#define POLICYMAN_MSG_LOW_0(str)                      MSG(MSG_SSID_POLICYMAN, PM_MSG_LVL_LOW, str)
#define POLICYMAN_MSG_MED_0(str)                      MSG(MSG_SSID_POLICYMAN, PM_MSG_LVL_MED, str)
#define POLICYMAN_MSG_HIGH_0(str)                     MSG(MSG_SSID_POLICYMAN, MSG_LEGACY_HIGH, str)
#define POLICYMAN_MSG_ERROR_0(str)                    MSG(MSG_SSID_POLICYMAN, MSG_LEGACY_ERROR, str)

#define POLICYMAN_MSG_LOW_1(str, a)                   MSG_1(MSG_SSID_POLICYMAN, PM_MSG_LVL_LOW, str, a)
#define POLICYMAN_MSG_MED_1(str, a)                   MSG_1(MSG_SSID_POLICYMAN, PM_MSG_LVL_MED, str, a)
#define POLICYMAN_MSG_HIGH_1(str, a)                  MSG_1(MSG_SSID_POLICYMAN, MSG_LEGACY_HIGH, str, a)
#define POLICYMAN_MSG_ERROR_1(str, a)                 MSG_1(MSG_SSID_POLICYMAN, MSG_LEGACY_ERROR, str, a)

#define POLICYMAN_MSG_LOW_2(str, a, b)                MSG_2(MSG_SSID_POLICYMAN, PM_MSG_LVL_LOW, str, a, b)
#define POLICYMAN_MSG_MED_2(str, a, b)                MSG_2(MSG_SSID_POLICYMAN, PM_MSG_LVL_MED, str, a, b)
#define POLICYMAN_MSG_HIGH_2(str, a, b)               MSG_2(MSG_SSID_POLICYMAN, MSG_LEGACY_HIGH, str, a, b)
#define POLICYMAN_MSG_ERROR_2(str, a, b)              MSG_2(MSG_SSID_POLICYMAN, MSG_LEGACY_ERROR, str, a, b)

#define POLICYMAN_MSG_LOW_3(str, a, b, c)             MSG_3(MSG_SSID_POLICYMAN, PM_MSG_LVL_LOW, str, a, b, c)
#define POLICYMAN_MSG_MED_3(str, a, b, c)             MSG_3(MSG_SSID_POLICYMAN, PM_MSG_LVL_MED, str, a, b, c)
#define POLICYMAN_MSG_HIGH_3(str, a, b, c)            MSG_3(MSG_SSID_POLICYMAN, MSG_LEGACY_HIGH, str, a, b, c)
#define POLICYMAN_MSG_ERROR_3(str, a, b, c)           MSG_3(MSG_SSID_POLICYMAN, MSG_LEGACY_ERROR, str, a, b, c)

#define POLICYMAN_MSG_LOW_4(str, a, b, c, d)          MSG_4(MSG_SSID_POLICYMAN, PM_MSG_LVL_LOW, str, a, b, c, d)
#define POLICYMAN_MSG_MED_4(str, a, b, c, d)          MSG_4(MSG_SSID_POLICYMAN, PM_MSG_LVL_MED, str, a, b, c, d)
#define POLICYMAN_MSG_HIGH_4(str, a, b, c, d)         MSG_4(MSG_SSID_POLICYMAN, MSG_LEGACY_HIGH, str, a, b, c, d)
#define POLICYMAN_MSG_ERROR_4(str, a, b, c, d)        MSG_4(MSG_SSID_POLICYMAN, MSG_LEGACY_ERROR, str, a, b, c, d)

#define POLICYMAN_MSG_LOW_5(str, a, b, c, d, e)       MSG_5(MSG_SSID_POLICYMAN, PM_MSG_LVL_LOW, str, a, b, c, d, e)
#define POLICYMAN_MSG_MED_5(str, a, b, c, d, e)       MSG_5(MSG_SSID_POLICYMAN, PM_MSG_LVL_MED, str, a, b, c, d, e)
#define POLICYMAN_MSG_HIGH_5(str, a, b, c, d, e)      MSG_5(MSG_SSID_POLICYMAN, MSG_LEGACY_HIGH, str, a, b, c, d, e)
#define POLICYMAN_MSG_ERROR_5(str, a, b, c, d, e)     MSG_5(MSG_SSID_POLICYMAN, MSG_LEGACY_ERROR, str, a, b, c, d, e)

#define POLICYMAN_MSG_LOW_6(str, a, b, c, d, e, f)    MSG_6(MSG_SSID_POLICYMAN, PM_MSG_LVL_LOW, str, a, b, c, d, e, f)
#define POLICYMAN_MSG_MED_6(str, a, b, c, d, e, f)    MSG_6(MSG_SSID_POLICYMAN, PM_MSG_LVL_MED, str, a, b, c, d, e, f)
#define POLICYMAN_MSG_HIGH_6(str, a, b, c, d, e, f)   MSG_6(MSG_SSID_POLICYMAN, MSG_LEGACY_HIGH, str, a, b, c, d, e, f)
#define POLICYMAN_MSG_ERROR_6(str, a, b, c, d, e, f)  MSG_6(MSG_SSID_POLICYMAN, MSG_LEGACY_ERROR, str, a, b, c, d, e, f)

#define POLICYMAN_MSG_LOW(str, a, b, c)               MSG_3(MSG_SSID_POLICYMAN, PM_MSG_LVL_LOW, str, a, b, c)
#define POLICYMAN_MSG_MED(str, a, b, c)               MSG_3(MSG_SSID_POLICYMAN, PM_MSG_LVL_MED, str, a, b, c)
#define POLICYMAN_MSG_HIGH(str, a, b, c)              MSG_3(MSG_SSID_POLICYMAN, MSG_LEGACY_HIGH, str, a, b, c)
#define POLICYMAN_MSG_ERROR(str, a, b, c)             MSG_3(MSG_SSID_POLICYMAN, MSG_LEGACY_ERROR, str, a, b, c)

#define POLICYMAN_ERR_FATAL(str, a, b, c)             ERR_FATAL("=POLICYMAN= " str, a, b, c)

#define POLICYMAN_MSG_S1(str, a)                 MSG_SPRINTF_1( MSG_SSID_POLICYMAN, MSG_LEGACY_HIGH,  str, a )
#define POLICYMAN_MSG_S2(str, a, b)              MSG_SPRINTF_2( MSG_SSID_POLICYMAN, MSG_LEGACY_HIGH,  str, a, b )
#define POLICYMAN_MSG_S3(str, a, b, c)           MSG_SPRINTF_3( MSG_SSID_POLICYMAN, MSG_LEGACY_HIGH,  str, a, b, c )
#define POLICYMAN_MSG_S4(str, a, b, c, d)        MSG_SPRINTF_4( MSG_SSID_POLICYMAN, MSG_LEGACY_HIGH,  str, a, b, c, d )
#define POLICYMAN_MSG_S5(str, a, b, c, d, e)     MSG_SPRINTF_5( MSG_SSID_POLICYMAN, MSG_LEGACY_HIGH,  str, a, b, c, d, e )
#define POLICYMAN_MSG_S6(str, a, b, c, d, e, f)  MSG_SPRINTF_6( MSG_SSID_POLICYMAN, MSG_LEGACY_HIGH,  str, a, b, c, d, e, f )


/*---------------------------------------------------------------------------
                  Asserts and verifications macros
---------------------------------------------------------------------------*/

#define POLICYMAN_ASSERT( exp )     \
  do                                \
  {                                 \
    if (!(exp))                     \
    {                               \
      POLICYMAN_ERR_FATAL("ASSERTION FAILED! " #exp, 0, 0, 0);  \
    }                               \
  } while (0)

#endif /* _POLICYMAN_DBG_H_ */
