#ifndef __RTPD_H__
#define __RTPD_H__

/* ! \file rtpd.h
    \brief Contains Real-Time Proxy Driver's API
*/
/*-----------------------------------------------------------------------------
     Copyright (C) 2014 QUALCOMM Technologies Incorporated.
     All rights reserved.
     Qualcomm Confidential and Proprietary.
-----------------------------------------------------------------------------*/
/*===========================================================================
                      EDIT HISTORY FOR MODULE

This section contains comments describing changes made to this module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/audio.tx/2.0/audio/api/rtpd/inc/rtpd.h#1 $
$DateTime: 2018/12/25 22:39:07 $
$Author: pwbldsvc $
$Change: 17929171 $
$Revision: #1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/12/14    vk     Initial version.
===========================================================================*/

#include "mmdefs.h"  
#include "rtpd_status.h"

/**
  Defines the open type for the rtpd_open() function.
*/
enum rtpd_open_code
{
  RTPD_OPEN_NOT_USED          = 0,/**< Not used. */
  RTPD_OPEN_REAL_TIME_STREAM  = 1,
    /**< Instructs RTPD to open a real-time stream handle. */
  RTPD_OPEN_MAX_NUM
    /**< Maximum number. */
};


/*=============================================================================
FUNCTION       rtpd_init

DESCRIPTION    This function initializes the real time proxy driver. 
               The second call only adds ref count. When adding new module/s,
               need to maintain the same ordering in init and deInit functions
               Account to the count of new module in the RTPD_INIT_STAGE_MAX defn/enum.

DEPENDENCIES   none

PARAMETERS     none

RETURN VALUE   rtpd return status @see rtpd_status.h

SIDE EFFECTS   none
=============================================================================*/
uint32_t rtpd_init(void);


/*=============================================================================
FUNCTION       rtpd_deinit

DESCRIPTION    This function first reduce the ref count. If ref count reaches zero 
               it de-initializes the rt proxy driver.When adding new module/s,
               need to maintain the same sequence in init and deInit functions
               Account to the count of new module in the RTPD_INIT_STAGE_MAX defn/enum.

DEPENDENCIES   none

PARAMETERS     none

RETURN VALUE   rtpd return status @see rtpd_status.h

SIDE EFFECTS   none
=============================================================================*/
uint32_t rtpd_deinit(void);


/*=============================================================================
FUNCTION       rtpd_open

DESCRIPTION    This function opens a handle for the client to communicate with the RTPD

DEPENDENCIES   none

PARAMETERS     @param code : rtpd_open_code
               @param params : payload
               @param size : payload size

RETURN VALUE   real time stream handle

SIDE EFFECTS   none
=============================================================================*/
uint32_t rtpd_open(enum rtpd_open_code code, void* params, uint32_t size);


/*=============================================================================
FUNCTION       rtpd_close

DESCRIPTION    This function closes the stream handle

DEPENDENCIES   none

PARAMETERS     @param handle: real time stream handle

RETURN VALUE   rtpd return status @see rtpd_status.h

SIDE EFFECTS   none
=============================================================================*/
uint32_t rtpd_close(uint32_t handle);


/*=============================================================================
FUNCTION       rtpd_write

DESCRIPTION    This function is used to send pcm data to rtpd

DEPENDENCIES   none

PARAMETERS     @param handle: real time stream handle
               @param params: payload
               @param size: payload size

RETURN VALUE   rtpd return status @see rtpd_status.h

SIDE EFFECTS   none
=============================================================================*/
uint32_t rtpd_write(uint32_t handle, void* params, uint32_t size);


/*=============================================================================
FUNCTION       rtpd_read

DESCRIPTION    This function is used to retrieve pcm data from rtpd

DEPENDENCIES   none

PARAMETERS     @param handle: real time stream handle
               @param params: payload
               @param size: payload size

RETURN VALUE   rtpd return status @see rtpd_status.h

SIDE EFFECTS   none
=============================================================================*/
uint32_t rtpd_read(uint32_t handle, void* params, uint32_t size);


/*=============================================================================
FUNCTION       rtpd_ioctl

DESCRIPTION    This function issues a control command from the client 
               to the RTPD for a specific handle.

DEPENDENCIES   none

PARAMETERS     @param handle: real time stream handle
               @param cmd: type of ioctl command
               @param params: payload
               @param size: payload size

RETURN VALUE   rtpd return status @see rtpd_status.h

SIDE EFFECTS   none
=============================================================================*/
uint32_t rtpd_ioctl(uint32_t handle, uint32_t cmd, void* params, uint32_t size);


/** @} */  /* end_addtogroup rtpd_api */

#endif /* __RTPD_H__ */
