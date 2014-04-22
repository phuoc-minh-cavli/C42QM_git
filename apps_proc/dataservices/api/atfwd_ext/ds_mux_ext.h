#ifndef DS_MUX_EXT_H
#define DS_MUX_EXT_H
/*===========================================================================
                         DS_MUX_EXT.H

DESCRIPTION
  This file exposes the DS MUX Logical init API to SIO
  
  Copyright (c)2016 by Qualcomm Technologies INCORPORATED.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary.
===========================================================================*/


/*==============================================================================

                            EDIT HISTORY FOR FILE
when        who    what, where, why
--------    ---    -------------------------------------------------------------
09/16/2016   sr     created file
==============================================================================*/
#include "sio.h"

uint32 ds_mux_ext_sio_init
(
  sio_register_device_driver_cb_type register_driver_cb_fn
);

#endif
