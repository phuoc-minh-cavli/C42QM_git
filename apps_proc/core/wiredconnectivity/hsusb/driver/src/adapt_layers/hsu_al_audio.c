/*==============================================================================
Audio adaptation layers.

GENERAL DESCRIPTION
High Speed USB Audio adaptation layer.

EXTERNALIZED FUNCTIONS
hsu_al_audio_open
hsu_al_audio_close
hsu_al_audio_write
hsu_al_audio_ioctl

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2014 - 2019 by Qualcomm Technologies, Inc. All Rights Reserved.
==============================================================================*/

/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

  $Header: 
  $DateTime: 

when      who     what, where, why
--------  ---     ------------------------------------------------------
06/30/14  rt      modified for VOU
06/12/11  sm      Initial version

==============================================================================*/

/*==============================================================================

INCLUDE FILES FOR MODULE

==============================================================================*/
#include "hsu_al_audio.h"
#include "hsu_common_int.h"

sio_status_type hsu_al_audio_open(sio_open_type * open_ptr)
{
  return SIO_UNAVAIL_S;
}

void hsu_al_audio_close (
  sio_stream_id_type stream_id,
  sio_port_id_type   port_id,
  sio_vv_func_ptr_type close_done_cb
)
{
  if (close_done_cb)
  {
    close_done_cb();
  }
}

void hsu_al_audio_ioctl (
  sio_stream_id_type stream_id,
  sio_port_id_type   port_id,
  sio_ioctl_cmd_type      cmd,
  sio_ioctl_param_type *  param
)
{
  HSU_ERR_FATAL("hsu_al_audio_ioctl not supported",0,0,0);
}

void hsu_al_audio_write
(
  sio_stream_id_type stream_id,
  sio_port_id_type   port_id,
  dsm_item_type *tx_ptr
)
{
  if (tx_ptr)
  {
    dsm_free_packet(&tx_ptr);
  }
  HSU_ERR_FATAL("hsu_al_audio_write not supported",0,0,0);
}

