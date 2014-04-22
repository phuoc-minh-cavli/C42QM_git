#ifndef REMOTEFS_SAHARA_PKT_H
#define REMOTEFS_SAHARA_PKT_H

/*==============================================================================

                      Remote Sahara Prorocol Interface Packet Definitions

DESCRIPTION
   This file contains RMTS Sahara Procotol Interface Packet Definitions.

Copyright (c) 2014 by QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
==============================================================================*/
/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.tx/6.0/storage/remotefs/src/remotefs_sahara_pkt.h#1 $
  $DateTime: 2020/01/30 22:49:35 $
  $Author: pwbldsvc $

YYYY-MM-DD       who     what, where, why
--------------   ---     -------------------------------------------------------
2014-12-05       dks     Create.
==============================================================================*/

#include "comdef.h"
#include "remotefs_sahara.h"

enum remotefs_sahara_status remotefs_sahara_interpret_pkt (
              struct remotefs_sahara_packet *sahara_pkt, uint32 pkt_len);

void remotefs_sahara_form_hello_pkt (struct remotefs_sahara_packet *sahara_pkt,
                                     uint32 mode, uint32 seq_num);


void remotefs_sahara_form_mem_dbg_pkt (
                                     struct remotefs_sahara_packet *sahara_pkt,
                                     uint32 mem_tbl_addr, uint32 mem_tbl_size);


void remotefs_sahara_form_cmd_ready_pkt (
                                    struct remotefs_sahara_packet *sahara_pkt);

void remotefs_sahara_form_cmd_exec_resp_pkt (
                                    struct remotefs_sahara_packet *sahara_pkt,
                                    uint32 cmd_id, uint32 resp_len);

void remotefs_sahara_form_reset_resp_pkt (
                                    struct remotefs_sahara_packet *sahara_pkt);


void remotefs_sahara_form_end_of_img_transfer_pkt (
                                    struct remotefs_sahara_packet *sahara_pkt,
                                    uint32 image_id, uint32 status);

#endif /* REMOTEFS_SAHARA_PKT_H */
