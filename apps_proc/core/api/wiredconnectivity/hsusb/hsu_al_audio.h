#ifndef _HSU_AL_AUDIO_H_
#define _HSU_AL_AUDIO_H_

/*==============================================================================

            High Speed USB Adaptation Layer for Audio Data Transfers

  GENERAL DESCRIPTION
   Provides High-Speed USB Audio Data Transfers.

  Copyright (c) 2014 - 2019 by Qualcomm Technologies, Inc. All Rights Reserved.
==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: 
  $DateTime: 

 
  when     who  what, where, why
  -------- ---  ---------------------------------------------------------
 06/12/11  sm   Intial version   
==============================================================================*/
#include "customer.h"
#include "comdef.h"
#include "sio.h"
#include "dsm.h"

/*==============================================================================
  Macros
==============================================================================*/

#define HSU_AUDIO_STREAM_DISABLE 0
#define HSU_AUDIO_STREAM_ENABLE 1


/*==============================================================================
  Typedefs
==============================================================================*/

/*===========================================================================

FUNCTION hsu_al_audio_open

DESCRIPTION
This function opens and interface for Audio over usb.

DEPENDENCIES
None

RETURN VALUE
int

SIDE EFFECTS
None
===========================================================================*/

sio_status_type hsu_al_audio_open(sio_open_type * open_ptr);

/*===========================================================================

FUNCTION hsu_al_audio_close

DESCRIPTION
This function closes the interface for Audio over usb.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/

void hsu_al_audio_close (
  sio_stream_id_type stream_id,
  sio_port_id_type   port_id,
  sio_vv_func_ptr_type close_done_cb
);

/*===========================================================================

FUNCTION hsu_al_audio_ioctl

DESCRIPTION
This function works on the icotl parameters passed.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
void hsu_al_audio_ioctl (
  sio_stream_id_type stream_id,
  sio_port_id_type   port_id,
  sio_ioctl_cmd_type      cmd,
  sio_ioctl_param_type *  param
);

/*===========================================================================

FUNCTION hsu_al_audio_write

DESCRIPTION
This function writes a buffer to the write stream.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
void hsu_al_audio_write
(
  sio_stream_id_type stream_id,
  sio_port_id_type   port_id,
  dsm_item_type *tx_ptr
); 

#endif /* _HSU_AL_AUDIO_H_ */
