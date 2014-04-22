/*!
  @file
  intf_nb1fw_sys_msg.h

  @brief
  FW-SW interface system message UMIDs for NB1 FW

  @detail
  FW-SW interface system message UMIDs for NB1 FW

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

$Header: //source/qcom/qct/modem/fw/components/fw_nb.mpss/rel/15.2/nb1/api/intf_nb1fw_sys_msg.h#2 $
$DateTime: 2023/04/27 07:48:28 $
$Author: aakaniti $

when       who     what, where, why
--------   ---     ----------------------------------------------------------

===========================================================================*/

#ifndef INTF_NB1FW_SYS_MSG_H
#define INTF_NB1FW_SYS_MSG_H

#include "intf_nb1fw_sys.h"

typedef enum
{
  MSGR_DEFINE_UMID(NB1, FW_SYS,  REQ, CONFIG_APP,        0x0, nb1fw_sys_config_app_req_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_SYS,  REQ, CELL_UPDATE,       0x1, nb1fw_sys_cell_update_req_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_SYS,  REQ, CELL_CONFIG,       0x2, nb1fw_sys_cell_config_req_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_SYS,  REQ, CELL_DECONFIG,     0x3, nb1fw_sys_cell_deconfig_req_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_SYS,  REQ, CELL_MC_CONFIG,    0x4, nb1fw_sys_cell_mc_config_req_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_SYS,  REQ, CELL_STAT,         0x5, nb1fw_sys_cell_stat_req_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_SYS,  REQ, LOG_MASK_CONFIG,   0x6, nb1fw_sys_log_mask_config_req_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_SYS,  REQ, RX_SAMPLE_CAPTURE, 0x7, nb1fw_sys_rx_sample_capture_req_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_SYS,  REQ, MC_CONFIG,         0x8, nb1fw_sys_mc_config_req_msg_s),
  NB1FW_SYS_REQ_COUNT=0x9,

  MSGR_DEFINE_UMID(NB1, FW_SYS,  CNF, CONFIG_APP,        0x0, nb1fw_sys_config_app_cnf_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_SYS,  CNF, CELL_UPDATE,       0x1, nb1fw_sys_cell_update_cnf_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_SYS,  CNF, CELL_CONFIG,       0x2, nb1fw_sys_cell_config_cnf_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_SYS,  CNF, CELL_DECONFIG,     0x3, nb1fw_sys_cell_deconfig_cnf_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_SYS,  CNF, CELL_MC_CONFIG,    0x4, nb1fw_sys_cell_mc_config_cnf_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_SYS,  CNF, CELL_STAT,         0x5, nb1fw_sys_cell_stat_cnf_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_SYS,  CNF, LOG_MASK_CONFIG,   0x6, nb1fw_sys_log_mask_config_cnf_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_SYS,  CNF, RX_SAMPLE_CAPTURE, 0x7, nb1fw_sys_rx_sample_capture_cnf_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_SYS,  CNF, MC_CONFIG,         0x8, nb1fw_sys_mc_config_cnf_msg_s),
  NB1FW_SYS_CNF_COUNT=0x9,

  MSGR_DEFINE_UMID(NB1, FW_SYS,  IND, CELL_STAT,         0x0, nb1fw_sys_cell_stat_ind_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_SYS,  IND, MC_CONFIG,         0x1, nb1fw_sys_mc_config_ind_s),
  NB1FW_SYS_IND_COUNT=0x2,
} nb1fw_sys_msg_e;

#endif /* INTF_NB1FW_SYS_MSG_H */

