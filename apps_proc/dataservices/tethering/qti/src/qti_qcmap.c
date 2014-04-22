/*==============================================================================

                             QTI_QCMAP.C

GENERAL DESCRIPTION
  This file checks if the USB mode is standard ECM 
  Incase of Std ECM, QCMAP task is invoked.

Copyright (c) 2019 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
==============================================================================*/

#include "qcmap.h"

int qti_check_usb_mode(sio_stream_id_type stream_id, sio_ioctl_param_type *param)
{
  int ret = 0;
#ifdef ECM_ENABLE
  sio_control_ioctl (stream_id,
                       SIO_IOCTL_IS_STANDARD_ECM, param);

  if (param->is_standard_ecm)
  {
    //pass the Periphereal streamId to QCMAP task so that the DTR CB can be changed
#ifndef MINI_QCLI
    qti_qcmap_cb(stream_id);
    ret = 1;
#endif
  }
  else
#endif
    ret = 0;

  return ret;
}

