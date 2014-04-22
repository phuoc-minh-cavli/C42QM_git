#ifndef __CSD_VOICE_DEBUG_PRIVATE_H__
#define __CSD_VOICE_DEBUG_PRIVATE_H__

/*-----------------------------------------------------------------------------
     Copyright (c) 2009 - 2011 Qualcomm Technologies Incorporated.
	 All rights reserved.
	 Qualcomm Confidential and Proprietary
-----------------------------------------------------------------------------*/
/*=============================================================================
                      EDIT HISTORY FOR MODULE
	This section contains comments describing changes made to this module.
	Notice that changes are listed in reverse chronological order.

	$Header: //components/rel/audio.tx/2.0/audio/csd/voice/inc/csd_voice_debug_private.h#1 $
	$DateTime: 2018/12/25 22:39:07 $
	$Author: pwbldsvc $
	$Change: 17929171 $
	$Revision: #1 $
	=========================================================================*/

#include "csd_intf.h"
#include "csd_vcommon.h"
#include "csd.h"

/*****************************************************************************
 * Voice Debug Internal API                                    *
 ****************************************************************************/
CSD_INTERNAL int32_t csd_voice_debug_init (
  struct csd_vtable** ret_vtable
);

CSD_INTERNAL int32_t csd_voice_debug_deinit ( void );

CSD_INTERNAL int32_t csd_voice_debug_open (
  enum csd_open_code open_id,
  void* params,
  uint32_t size
);

CSD_INTERNAL int32_t csd_voice_debug_close (
  uint32_t handle
);

CSD_INTERNAL int32_t csd_voice_debug_write (
  uint32_t handle,
  void* params,
  uint32_t size
);

CSD_INTERNAL int32_t csd_voice_debug_read (
  uint32_t handle,
  void* params,
  uint32_t size
);

CSD_INTERNAL int32_t csd_voice_debug_ioctl (
  uint32_t handle,
  uint32_t code,
  void* params,
  uint32_t size
);
#endif /* __CSD_VOICE_DEBUG_PRIVATE_H__ */

