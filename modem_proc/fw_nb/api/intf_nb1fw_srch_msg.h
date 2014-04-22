/*!
  @file
  intf_nb1fw_srch_msg.h

  @brief
  FW-SW interface search message UMIDs for NB1 FW

  @detail
  FW-SW interface search message UMIDs for NB1 FW

*/

/*===========================================================================

  Copyright (c) 2016 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/fw/components/fw_nb.mpss/rel/15.2/nb1/api/intf_nb1fw_srch_msg.h#2 $
$DateTime: 2023/04/27 07:48:28 $
$Author: aakaniti $

when       who     what, where, why
--------   ---     ----------------------------------------------------------

===========================================================================*/

#ifndef INTF_NB1FW_SRCH_MSG_H
#define INTF_NB1FW_SRCH_MSG_H

#include "intf_nb1fw_srch.h"

typedef enum
{
  MSGR_DEFINE_UMID(NB1, FW_SRCH,  REQ, FULL_FREQ_SCAN,    0x0, nb1fw_srch_full_freq_scan_req_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_SRCH,  REQ, LIST_FREQ_SCAN,    0x1, nb1fw_srch_list_freq_scan_req_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_SRCH,  REQ, BLOCKED_CELL_LIST, 0x2, nb1fw_srch_blocked_cell_list_req_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_SRCH,  REQ, INIT_ACQ,          0x3, nb1fw_srch_init_acq_req_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_SRCH,  REQ, CELL_SRCH,         0x4, nb1fw_srch_cell_srch_req_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_SRCH,  REQ, ABORT,             0x5, nb1fw_srch_abort_req_msg_s),
  NB1FW_SRCH_REQ_COUNT=0x6,

  MSGR_DEFINE_UMID(NB1, FW_SRCH,  CNF, FULL_FREQ_SCAN,    0x0, nb1fw_srch_full_freq_scan_cnf_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_SRCH,  CNF, LIST_FREQ_SCAN,    0x1, nb1fw_srch_list_freq_scan_cnf_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_SRCH,  CNF, BLOCKED_CELL_LIST, 0x2, nb1fw_srch_blocked_cell_list_cnf_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_SRCH,  CNF, INIT_ACQ,          0x3, nb1fw_srch_init_acq_cnf_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_SRCH,  CNF, CELL_SRCH,         0x4, nb1fw_srch_cell_srch_cnf_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_SRCH,  CNF, ABORT,             0x5, nb1fw_srch_abort_cnf_msg_s),
  NB1FW_SRCH_CNF_COUNT=0x6,
} nb1fw_srch_msg_e;

typedef enum
{
  MSGR_DEFINE_UMID(NB1, FW_SRCH,  CMDI, TERMINATE,      0x0, nb1fw_srch_terminate_cmdi_msg_s),
  NB1FW_SRCH_CMDI_COUNT=0x1,
} nb1fw_srch_msgi_e;

#endif /* INTF_NB1FW_SRCH_MSG_H */

