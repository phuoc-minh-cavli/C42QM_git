#ifndef __VOICEINIT_H__
#define __VOICEINIT_H__

/*
   Copyright (C) 2016 QUALCOMM Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.

   $Header: //components/rel/avs.mpss/8.0.c12/vsd/voiceinit/inc/private/voiceinit.h#1 $
   $Author: pwbldsvc $
*/


//#include "customer.h"
//#include "comdef.h"
#include "mmdefs.h"

/***************************************************************************
  VOICEINIT CALL FUNCTION.
****************************************************************************/

/**
 * Provides a single API entry point into voice drivers intialization module.
 *
 * @param[in] event_id event identifier to execute.
 * @param[in] params event parameters.
 * @param[in] size Size of the event parameters in bytes.V
 *
 * @return
 * None
 */

int32_t voiceinit_call (
  uint32_t cmd_id,
  void* params,
  uint32_t size
);

/**
 * This is the voiceinit task entry-point.
 *
 * \param[in] params The parameters supplied by TMC.
 *
 * \pre
 *   TBD
 *
 * \post
 *   TBD
 *
 * \remarks
 *   This API is intended to be called only once by the TMC during boot-up
 *   initialization.
 */
void voiceinit_task (
  dword params
);

#endif  /* !__VOICEINIT_H__ */

