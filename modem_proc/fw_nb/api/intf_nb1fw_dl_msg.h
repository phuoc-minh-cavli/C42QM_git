/*!
  @file
  intf_nb1fw_dl_msg.h

  @brief
  FW-SW interface DL message UMIDs for NB1 FW

  @detail
  FW-SW interface DL message UMIDs for NB1 FW

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

$Header: //source/qcom/qct/modem/fw/components/fw_nb.mpss/rel/15.2/nb1/api/intf_nb1fw_dl_msg.h#2 $
$DateTime: 2023/04/27 07:48:28 $
$Author: aakaniti $

when       who     what, where, why
--------   ---     ----------------------------------------------------------

===========================================================================*/

#ifndef INTF_NB1FW_DL_MSG_H
#define INTF_NB1FW_DL_MSG_H

#include "intf_nb1fw_dl.h"

typedef enum
{
  MSGR_DEFINE_UMID(NB1, FW_DL,  REQ, PBCH_DECODE,        0x0, nb1fw_dl_pbch_decode_req_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_DL,  REQ, PBCH_DECODE_STOP,   0x1, nb1fw_dl_pbch_decode_stop_req_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_DL,  REQ, SI_UPDATE_INFO,     0x2, nb1fw_dl_si_update_info_req_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_DL,  REQ, SI_START_COLLECT,   0x3, nb1fw_dl_si_start_collect_req_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_DL,  REQ, SI_STOP_COLLECT,    0x4, nb1fw_dl_si_stop_collect_req_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_DL,  REQ, PDCCH_CONFIG,       0x5, nb1fw_dl_pdcch_config_req_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_DL,  REQ, PDCCH_RNTI_CHANGE,  0x6, nb1fw_dl_pdcch_rnti_change_req_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_DL,  REQ, PDCCH_DECONFIG,     0x7, nb1fw_dl_pdcch_deconfig_req_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_DL,  REQ, MEAS_CELL,          0x8, nb1fw_dl_meas_cell_req_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_DL,  REQ, MEAS_CELL_ABORT,    0x9, nb1fw_dl_meas_cell_abort_req_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_DL,  REQ, DRX_STATUS_UPDATE,  0xa, nb1fw_dl_drx_status_update_req_msg_s),
  NB1FW_DL_REQ_COUNT=0xB,

  MSGR_DEFINE_UMID(NB1, FW_DL,  CNF, PBCH_DECODE,        0x0, nb1fw_dl_pbch_decode_cnf_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_DL,  CNF, PBCH_DECODE_STOP,   0x1, nb1fw_dl_pbch_decode_stop_cnf_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_DL,  CNF, SI_UPDATE_INFO,     0x2, nb1fw_dl_si_update_info_cnf_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_DL,  CNF, SI_START_COLLECT,   0x3, nb1fw_dl_si_start_collect_cnf_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_DL,  CNF, SI_STOP_COLLECT,    0x4, nb1fw_dl_si_stop_collect_cnf_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_DL,  CNF, PDCCH_CONFIG,       0x5, nb1fw_dl_pdcch_config_cnf_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_DL,  CNF, PDCCH_RNTI_CHANGE,  0x6, nb1fw_dl_pdcch_rnti_change_cnf_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_DL,  CNF, PDCCH_DECONFIG,     0x7, nb1fw_dl_pdcch_deconfig_cnf_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_DL,  CNF, MEAS_CELL,          0x8, nb1fw_dl_meas_cell_cnf_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_DL,  CNF, MEAS_CELL_ABORT,    0x9, nb1fw_dl_meas_cell_abort_cnf_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_DL,  CNF, DRX_STATUS_UPDATE,  0xa, nb1fw_dl_drx_status_update_cnf_msg_s),
  NB1FW_DL_CNF_COUNT=0xB,

  MSGR_DEFINE_UMID(NB1, FW_DL,  IND, PBCH_MIB,     0x0, nb1fw_dl_pbch_mib_ind_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_DL,  IND, SIB,          0x1, nb1fw_dl_sib_ind_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_DL,  IND, PAGE_FAILED,  0x2, nb1fw_dl_page_failed_ind_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_DL,  IND, PDCCH_DCI,    0x3, nb1fw_dl_pdcch_dci_ind_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_DL,  IND, DATA,         0x4, nb1fw_dl_data_ind_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_DL,  IND, DECODE_STAT,  0x5, nb1fw_dl_decode_stat_ind_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_DL,  IND, CHAN_END,     0x6, nb1fw_dl_chan_end_ind_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_DL,  IND, RLM,          0x7, nb1fw_dl_rlm_ind_msg_s),
  NB1FW_DL_IND_COUNT=0x8,
} nb1fw_dl_msg_e;

typedef enum
{
  MSGR_DEFINE_UMID(NB1, FW_DL,  CMDI, CELL_UPDATE,    0x0, nb1fw_dl_cell_update_cmdi_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_DL,  CMDI, CELL_CFG,       0x1, nb1fw_dl_cell_cfg_cmdi_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_DL,  CMDI, CELL_DECFG,     0x2, nb1fw_dl_cell_decfg_cmdi_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_DL,  CMDI, PDCCH_DECODE,   0x3, nb1fw_dl_pdcch_decode_cmdi_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_DL,  CMDI, DRX_UPDATE_DL,  0x4, nb1fw_dl_drx_update_dl_cmdi_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_DL,  CMDI, CELL_STAT_REQ,  0x5, nb1fw_dl_cell_stat_req_cmdi_msg_s),
  MSGR_DEFINE_UMID(NB1, FW_DL,  CMDI, CELL_MEAS_DONE, 0x6, nb1fw_dl_cell_meas_done_cmdi_msg_s),
#ifdef FEATURE_NON_ANCHOR_CARRIER_SUPPORT
  MSGR_DEFINE_UMID(NB1, FW_DL,  CMDI, MC_CFG_REQ,     0x7, nb1fw_dl_mc_cfg_cmdi_msg_s),
  NB1FW_DL_CMDI_COUNT=0x8
#else
  NB1FW_DL_CMDI_COUNT=0x7
#endif
} nb1fw_dl_msgi_e;

#endif /* INTF_NB1FW_DL_MSG_H */

