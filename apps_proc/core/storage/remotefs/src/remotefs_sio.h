#ifndef REMOTEFS_SIO_H
#define REMOTEFS_SIO_H

/*==============================================================================

                      Remote Storage Server SIO Interface

DESCRIPTION
   This file contains RMTS interface to the SIO transport layer

Copyright (c) 2014 by QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
==============================================================================*/
/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.tx/6.0/storage/remotefs/src/remotefs_sio.h#1 $
  $DateTime: 2020/01/30 22:49:35 $
  $Author: pwbldsvc $

YYYY-MM-DD       who     what, where, why
--------------   ---     -------------------------------------------------------
2014-12-05       dks     Create.
==============================================================================*/

#include "comdef.h"
#include "rex.h"
#include "remotefs_comm_api.h"

void remotefs_sio_init (rex_tcb_type *svc_tcb);

remotefs_status_type remotefs_sio_tx (uint8 *tx_buf, uint32 len,
                        uint32 flush_flag, uint32 flush_timeout, uint8 *rx_buf,
                        uint32 rx_len, boolean loop_for_data);

remotefs_status_type remotefs_sio_rx  (uint32 timeout, uint32 *rx_bytes);

#endif /* REMOTEFS_SIO_H */

