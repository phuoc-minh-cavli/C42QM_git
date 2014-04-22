/*!
  @file
  intf_nb1fw_ul_msg.h

  @brief
  FW-SW interface UL message UMIDs for NB1 FW

  @detail
  FW-SW interface UL message UMIDs for NB1 FW

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

$Header: //source/qcom/qct/modem/fw/components/fw_nb.mpss/rel/15.2/nb1/api/intf_nb1fw_ul_msg.h#2 $
$DateTime: 2023/04/27 07:48:28 $
$Author: aakaniti $

when       who     what, where, why
--------   ---     ----------------------------------------------------------

===========================================================================*/

#ifndef INTF_NB1FW_UL_MSG_H
#define INTF_NB1FW_UL_MSG_H

#include "intf_nb1fw_ul.h"

typedef enum
{
  MSGR_DEFINE_UMID(NB1, FW_UL,  REQ, PUSCH_CONFIG,              0x0, nb1fw_ul_pusch_config_req_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_UL,  REQ, PUSCH_ENCODE,              0x1, nb1fw_ul_pusch_encode_req_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_UL,  REQ, PUSCH_SCHEDULE_DATA,       0x2, nb1fw_ul_pusch_schedule_data_req_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_UL,  REQ, PUSCH_SCHEDULE_ACK_NACK,   0x3, nb1fw_ul_pusch_schedule_ack_nack_req_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_UL,  REQ, PRACH_SCHEDULE,            0x4, nb1fw_ul_prach_schedule_req_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_UL,  REQ, PUSCH_TA_UPDATE,           0x5, nb1fw_ul_pusch_ta_update_req_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_UL,  REQ, PUSCH_DECONFIG,            0x6, nb1fw_ul_pusch_deconfig_req_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_UL,  REQ, PUSCH_ABORT,               0x7, nb1fw_ul_pusch_abort_req_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_UL,  REQ, PRACH_ABORT,               0x8, nb1fw_ul_prach_abort_req_msg_s),
  NB1FW_UL_REQ_COUNT=0x9,

  MSGR_DEFINE_UMID(NB1, FW_UL,  CNF, PUSCH_CONFIG,             0x0, nb1fw_ul_pusch_config_cnf_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_UL,  CNF, PUSCH_ENCODE,             0x1, nb1fw_ul_pusch_encode_cnf_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_UL,  CNF, PUSCH_SCHEDULE_DATA,      0x2, nb1fw_ul_pusch_schedule_data_cnf_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_UL,  CNF, PUSCH_SCHEDULE_ACK_NACK,  0x3, nb1fw_ul_pusch_schedule_ack_nack_cnf_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_UL,  CNF, PRACH_SCHEDULE,           0x4, nb1fw_ul_prach_schedule_cnf_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_UL,  CNF, PUSCH_TA_UPDATE,          0x5, nb1fw_ul_pusch_ta_update_cnf_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_UL,  CNF, PUSCH_DECONFIG,           0x6, nb1fw_ul_pusch_deconfig_cnf_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_UL,  CNF, PUSCH_ABORT,              0x7, nb1fw_ul_pusch_abort_cnf_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_UL,  CNF, PRACH_ABORT,              0x8, nb1fw_ul_prach_abort_cnf_msg_s),
  NB1FW_UL_CNF_COUNT=0x9,

  MSGR_DEFINE_UMID(NB1, FW_UL,  IND, FREQ_ERROR_UPDATE,   0x0, nb1fw_ul_freq_error_update_ind_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_UL,  IND, CHAN_END,            0x1, nb1fw_ul_chan_end_ind_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_UL,  IND, HDET_UPDATE,         0x2, nb1fw_ul_hdet_update_ind_msg_s),
  NB1FW_UL_IND_COUNT=0x3
} nb1fw_ul_msg_e;

#endif /* INTF_NB1FW_UL_MSG_H */

