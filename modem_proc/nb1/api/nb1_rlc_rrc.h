/*!
  @file
  nb1_rlc_rrc.h

  @brief
  RLC header file related to RRC.

  @author
  gxiao, araina

*/

/*==============================================================================

  Copyright (c) 2016 QUALCOMM Technologies Incorporated. All Rights Reserved

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

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/nb1.mpss/2.2/api/nb1_rlc_rrc.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
03/18/19   ss      CR 2407883 : DMM suspend with RLF stop all timer and restart
                   request and confirm messages
                   pdu logging
                   nb1_rlcul_am_cfg_s
==============================================================================*/

#ifndef NB1_RLC_RRC_H
#define NB1_RLC_RRC_H

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include <comdef.h>
#include <msgr.h>
#include <nb1_as.h>
#include "nb1_l2_comdef.h"
#include "nb1_l1_types.h"
#include "nb1_rrc_ext_msg.h"
/*==============================================================================

                   EXTERNAL CONSTANT/MACRO DEFINITIONS

==============================================================================*/


/*==============================================================================

                   EXTERNAL ENUMERATION TYPES

==============================================================================*/

#define NB1_RLC_MAX_MUID_SIZE_IN_MSG 16

/*! @brief action on the RB
*/
typedef enum
{
  NB1_RLC_RB_ACTION_INVALID      = 0x0, /*!< invalid - for initialization
                                              purpose */
  NB1_RLC_RB_ACTION_ADD          = 0x1, /*!< add RB */
  NB1_RLC_RB_ACTION_MODIFY       = 0x2, /*!< modify RB */
  NB1_RLC_RB_ACTION_RE_EST       = 0x4, /*!< re-establish RB, for future use */
  NB1_RLC_RB_ACTION_RE_EST_MODIFY= 0x6, /*!< re-establish and modify RB,
                                             for future use  */
  NB1_RLC_RB_ACTION_REMOVE       = 0x8  /*!< remove RB  */
} nb1_rlc_rb_action_e;

/*==============================================================================

                   EXTERNAL STRUCTURE/UNION TYPES

==============================================================================*/

/*! @brief UL AM configuration definition
*/
typedef struct
{
  uint32 t_poll_retransmit_new;  /*!< Indicates the value of timer T_poll_retransmit
                                  in ms */
  uint8  max_retx_threshold; /*!< Maximum number of re-transmissions */
} nb1_rlcul_am_cfg_s;

/*! @brief DL AM configuration definition
*/
typedef struct
{
  boolean enable_status_rpt;  /*!Boolean to Indicate if status reporting due to
                                  detection of reception failure is enabled */
} nb1_rlcdl_am_cfg_s;

/*! @brief union of UL configuration
*/
typedef union
{
  nb1_rlcul_am_cfg_s  am_cfg; /*!< configuration for AM */
} nb1_rlcul_cfg_u;

/*! @brief union of DL cfg
*/
typedef union
{
  nb1_rlcdl_am_cfg_s  am_cfg; /*!< configuration for AM */
} nb1_rlcdl_cfg_u;

/*! @brief UL RB info definition
*/
typedef struct
{
  nb1_lc_id_t        lc_id;               /*!< logical channel ID */
  nb1_rb_id_t        rb_id;               /*!< radio bearer ID, not unique */
  nb1_eps_id_t       eps_id;              /*!< EPS bearer ID */
  nb1_rb_cfg_idx_t   rb_cfg_idx;          /*!< RB configuration index: unique */

  nb1_rb_type_e      rb_type;             /*!< SRB/DRB */
  uint8              pdcp_seq_len;        /*!< PDCP seq len in bits(5,7,12 bits)
                                               only used for PDCP pdu logging */
  nb1_rlcul_cfg_u    cfg;                 /*!< configuration for the RB */
} nb1_rlcul_rb_info_s;

/*! @brief DL RB info definition
*/
typedef struct
{
  nb1_lc_id_t         lc_id;      /*!< logical channel ID */
  nb1_rb_id_t         rb_id;      /*!< radio bearer ID: not unique */
  nb1_eps_id_t        eps_id;     /*!< EPS bearer ID */
  nb1_rb_cfg_idx_t    rb_cfg_idx; /*!< RB configuration index: unique
                                       RB cfg idx 1 - 32 for DRBs
                                       RB cfg idx 33-34 for SRBs */
  nb1_rb_type_e       rb_type;    /*!< SRB/DRB */
  nb1_rlcdl_cfg_u     cfg;        /*!< configuration for the RB */
  boolean             t_reordering_present;/*!< whether t_reordering is present*/
  uint16              t_reordering;        /*!< T_reordering which is madantory if twoHARQ is TRUE*/
} nb1_rlcdl_rb_info_s;

/*! @brief DL MRB info definition
*/
typedef struct
{
  nb1_mbsfn_area_id_t area_id;    /*!< MBSFN area id */
  nb1_pmch_id_t       pmch_id;    /*!< PMCH id */
  nb1_rb_id_t         mrb_id;      /*!< MRB id, generated by RRC,
                                        MRB id 35 used for MCCH by default,
                                        No RRC configuration for MCCH,
                                        MTCH valid range is 36 to
                                        (36 + NB1_MAX_ACTIVE_MRB - 1) */
  nb1_lc_id_t         lc_id;      /*!< logical channel ID */
  nb1_rb_cfg_idx_t    mrb_cfg_idx;     /*!< MRB cfg idx: currently
                                            0 to (NB1_MAX_ACTIVE_MRB - 1)
                                            is the valid index range*/
  nb1_rb_type_e       rb_type;    /*!< Only MTCH is applicable */
  lte_rrc_embms_session_info_s session_info; /*!< TMGI/Session id for the MRB id */

  /*! Indicates the MCH scheduling period i.e. the periodicity used for providing
    MCH scheduling information at lower layers (MAC) applicable for an MCH. Value rf8
    corresponds to 8 radio frames, rf16 corresponds to 16 radio frames and so on.
  */
  nb1_l1_mbsfn_mch_schdl_period_e    msi_schdl_period;
} nb1_rlcdl_mrb_info_s;

/*! @brief definition of modified RB info
*/
typedef struct
{
  nb1_rb_cfg_idx_t     rb_cfg_idx; /*!< RB configuration index: unique */
  nb1_rlc_rb_action_e  action;     /*!< action on the RB */
} nb1_rlc_modified_rb_s;

/*! @brief DL MRB info definition
*/
typedef struct
{
  uint8                  num_released_mrb;  /*!< number of released MRBs */
  nb1_rb_cfg_idx_t       released_mrb_cfg_idx[NB1_MAX_ACTIVE_MRB];
                                          /*!< Released MRB cfg index
                                            0 to (NB1_MAX_ACTIVE_MRB - 1)
                                            is the valid index range */
  uint8                  num_added_mrb; /*!< number of added MRBs */
  nb1_rlc_modified_rb_s  added_mrb_cfg_idx[NB1_MAX_ACTIVE_MRB];
                                           /*!< added MRB cfg idx,
                                            0 to (NB1_MAX_ACTIVE_MRB - 1)
                                             is the valid index range */

  nb1_rlcdl_mrb_info_s*  mrb_cfg_base_ptr;  /*!< base pointer to DL MRB
                                               configuration database
                                               of size NB1_MAX_ACTIVE_MRB */
} nb1_rlcdl_embms_cfg_s;


/*! @brief configuration request message definition for UL
*/
typedef struct
{
  msgr_hdr_struct_type   msg_hdr;          /*!< common message router header */
  nb1_l2_cfg_reason_e    cfg_reason;         /*!< configuration reason */
  uint8                  num_released_rb;  /*!< number of released RBs */
  nb1_rb_cfg_idx_t       released_rb_cfg_idx[NB1_MAX_ACTIVE_RB]; /*!< Released
                                                             RB cfg index */
  uint8                  num_modified_rb;  /*!< number of add/modified RBs */
  nb1_rlc_modified_rb_s  modified_rb[NB1_MAX_ACTIVE_RB]; /*!< add/modified RB
                                                          configuration index */
  nb1_rlcul_rb_info_s*   rb_cfg_base_ptr;   /*!< base pointer to UL Radio Bearer
                                                configuration database
                                                of size NB1_MAX_RB_CFG_IDX */
  boolean                dmm_suspend;
} nb1_rlcul_cfg_req_s;


/*! @brief configuration request message definition for DL
*/
typedef struct
{
  msgr_hdr_struct_type   msg_hdr;          /*!< common message router header */
  nb1_l2_cfg_reason_e    cfg_reason;       /*!< configuration reason */
  uint8                  num_released_rb;  /*!< number of released RBs,
                                              will be 0 if cfg_reason is
                                              NB1_L2_CFG_EMBMS */
  nb1_rb_cfg_idx_t       released_rb_cfg_idx[NB1_MAX_ACTIVE_RB]; /*!< Released
                                                             RB cfg index */
  uint8                  num_modified_rb;  /*!< number of add/modified RBs,
                                                will be 0 if cfg_reason is
                                                NB1_L2_CFG_EMBMS */
  nb1_rlc_modified_rb_s  modified_rb[NB1_MAX_ACTIVE_RB]; /*!< add/modified RB
                                                          configuration index */
  nb1_rlcdl_rb_info_s*   rb_cfg_base_ptr;  /*!< base pointer to DL Radio Bearer
                                                configuration database
                                                of size NB1_MAX_RB_CFG_IDX */
  nb1_rlcdl_embms_cfg_s  embms_cfg;        /*!< eMBMS config, applicable only
                                                if cfg_reason is
                                                NB1_L2_CFG_EMBMS */
  boolean                dmm_suspend;
} nb1_rlcdl_cfg_req_s;

/*! @brief configuration confirmation message definition
*/
typedef struct
{
  msgr_hdr_struct_type   msg_hdr;          /*!< common message router header */
  nb1_errno_e            cfg_status;       /*!< status of configuration */
} nb1_rlc_cfg_cnf_s;

/*! @brief NB1_RLCXX_START_REQ request message definition
*/
typedef struct
{
  msgr_hdr_struct_type   msg_hdr;    /*!< common message router header */
} nb1_rlc_start_req_s;

/*! @brief NB1_RLCXX_START_CNF request message definition
*/
typedef struct
{
  msgr_hdr_struct_type   msg_hdr;    /*!< common message router header */
  nb1_errno_e            cnf_status; /*!< confimation status message */
} nb1_rlc_start_cnf_s;

/*! @brief NB1_RLCXX_STOP_REQ request message definition
*/
typedef struct
{
  msgr_hdr_struct_type   msg_hdr;  /*!< common message router header */
} nb1_rlc_stop_req_s;

/*! @brief NB1_RLCXX_STOP_CNF request message definition
*/
typedef struct
{
  msgr_hdr_struct_type   msg_hdr;    /*!< common message router header */
  nb1_errno_e            cnf_status; /*!< confimation status message */
} nb1_rlc_stop_cnf_s;

/*! @brief NB1_RLCUL_MAX_RETX_IND message payload definition
*/
typedef struct
{
  msgr_hdr_struct_type   msg_hdr;          /*!< common message router header */
  nb1_rb_cfg_idx_t       rb_cfg_idx;       /*!< Unique RB configuration index */
} nb1_rlcul_max_retx_ind_s;

/*! @brief signal message MCCH (PDU) indication from RLC DL to RRC
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;  /*!< message header */
  msgr_attach_s                       dsm_attach; /*!< dsm attachment contained signal message */

  nb1_mbsfn_area_id_t                 area_id; /*!< Area id of MCCH */

  nb1_earfcn_t                        dl_cell_frequency;  /*!< The DL freq of the cell
                                                  in which the MCCH PDU was received */

  nb1_sfn_s                           sfn;  /*!< SFN in which the MCCH PDU was received */

} nb1_rlcdl_mcch_pdu_ind_s;

/*! @brief NB1_RLCUL_ACK_TX_STS_REQ request message definition
*/
typedef struct
{
  msgr_hdr_struct_type   msg_hdr;  /*!< common message router header */
  nb1_lc_id_t        lc_id;               /*!< logical channel ID */
} nb1_rlcul_ack_tx_sts_req_s;

/*! @brief NB1_RLCUL_ACK_TX_STS_CNF request message definition
*/
typedef struct
{
  msgr_hdr_struct_type   msg_hdr;    /*!< common message router header */
  nb1_errno_e            cnf_status; /*!< confimation status message */
} nb1_rlcul_ack_tx_sts_cnf_s;

/*! @brief SDU request confirmation from RLC UL to RRC for SRB1 BiS
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;  /*!< message header */
  /* Message payload */
  uint8                               num_ack_muid; /*!< number of ack MU ID */
  uint8                               ack_muid[NB1_RLC_MAX_MUID_SIZE_IN_MSG]; /*!< ack MU ID array */
  uint8                               num_nack_muid; /*!< number of nack MU ID */
  uint8                               nack_muid[NB1_RLC_MAX_MUID_SIZE_IN_MSG]; /*!< nack MU ID array */
  uint8                               num_maybe_muid; /*!< number of maybe transmitted MU ID */
  uint8                               maybe_muid[NB1_RLC_MAX_MUID_SIZE_IN_MSG]; /*!< maybe transmitted MU ID array */
} nb1_rlcul_sdu_cnf_msg_s;

#endif /* NB1_RLC_RRC_H */
