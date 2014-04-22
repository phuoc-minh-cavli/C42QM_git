/*!
  @file
  nb1_rlc_msg.h

  @brief
  This file contains all External message IDs interfaces exported by RLC layer.


*/

/*===========================================================================

  Copyright (c) 2018 QUALCOMM Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/nb1.mpss/2.2/api/nb1_rlc_msg.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/15/18   ss      CR 2278167 : FR 50565: L2 task merge for NB
08/01/08   ax      initial version
===========================================================================*/

#ifndef NB1_RLC_MSG_H
#define NB1_RLC_MSG_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "dsm_queue.h"
#include <msgr_nb1.h>
#include <lte_app.h>
#include "nb1.h"
#include "nb1_as.h"


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief RLCDL statistics per radio bearer
*/
typedef struct
{
  nb1_lc_id_t        lc_id;      /*!< logical channel ID */
  nb1_rb_id_t        rb_id;      /*!< radio bearer ID: not unique */
  nb1_rb_cfg_idx_t   rb_cfg_idx; /*!< RB configuration index: unique */
  nb1_rb_type_e      rb_type;    /*!< SRB/DRB */
  nb1_eps_id_t       eps_id;     /*!< EPS bearer ID, PDCP param */
  uint32  num_data_pdu;       /*!< number of RLC data pdu received */
  uint64  data_pdu_bytes;     /*!< RLC data PDU in bytes received */
  uint32  num_sdu;            /*!< number of RLC sdu reassembled */
  uint64  sdu_bytes;          /*!< RLC SDUs in bytes reassembled */
  uint32  num_comp_nack;      /*!< number of complete PDU NACKs sent in status */
  uint32  num_segm_nack;      /*!< number of segmented PDU NACKs sent */
} nb1_rlcdl_rb_stats_s;

/*! @brief RLCDL statistics
*/
typedef struct
{
  uint32                  num_active_rb;          /*!< number of active RB's */
  nb1_rlcdl_rb_stats_s    rb_stats[NB1_MAX_ACTIVE_RB]; /*!< stats per RB, only
            the first num_active_rb elements in the array are valid */
} nb1_rlcdl_stats_s;

/*! @brief statistics request message for RLCDL
*/
typedef struct
{
  msgr_hdr_struct_type   msg_hdr;          /*!< common message router header */
  nb1_rlcdl_stats_s*     stats_ptr;        /*!< pointer to the statistics */
} nb1_rlcdl_stats_req_s;

/*! @brief RLCUL statistics per radio bearer
*/
typedef struct
{
  nb1_rb_cfg_idx_t   rb_cfg_idx; /*!< RB configuration index: unique */
  uint32 num_new_data_pdus_txed;      /*!< Total number of new RLC PDUs txed */
  uint32 new_data_pdu_bytes_txed;     /*!< Total new data pdu bytes transmitted
                                           including header */
  uint32 num_sdus_txed;               /*!< Total number of new RLC SDUs txed */
  uint32 sdu_bytes_txed;              /*!< Total number of new RLC SDUs txed in
                                           bytes */
  uint32 num_pdus_re_txed;            /*!< Total number of re-txed RLC PDUs */
  uint32 pdu_bytes_re_txed;           /*!< Total pdu bytes re-transmitted */
} nb1_rlcul_rb_stats_s;

/*! @brief RLCUL statistics
*/
typedef struct
{
  uint32                  num_active_rb;          /*!< number of active RB's */
  nb1_rlcul_rb_stats_s    rb_stats[NB1_MAX_ACTIVE_RB]; /*!< stats per RB only
            the first num_active_rb elements in the array are valid */
} nb1_rlcul_stats_s;

/*! @brief statistics request message for RLCUL
*/
typedef struct
{
  msgr_hdr_struct_type   msg_hdr;          /*!< common message router header */
  nb1_rlcul_stats_s*     stats_ptr;        /*!< pointer to the statistics */
} nb1_rlcul_stats_req_s;


/*! @brief EMBMS watermark registration request message for RLCDL
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;          /*!< common message router header */
  /* Message payload */
  uint8                               id;           /*!< Watermark id*/
  dsm_watermark_type*                 embms_wm_ptr; /*!< EMBMS WM ptr provided by DS */
} nb1_rlcdl_embms_wm_register_req_msg_s;

/*! @brief EMBMS watermark deregistration request message for RLCDL
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;          /*!< common message router header */
  /* Message payload */
  uint8                               id;           /*!< Watermark id*/
} nb1_rlcdl_embms_wm_deregister_req_msg_s;

/*! @brief EMBMS watermark registration confirmation message for RLCDL
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;          /*!< common message router header */
  /* Message payload */
  uint8                               id;           /*!< Watermark id*/
  nb1_errno_e                         cnf_status;   /*!< confimation status message */
} nb1_rlcdl_embms_wm_register_cnf_msg_s;

/*! @brief EMBMS watermark deregistration confirmation message for RLCDL
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;          /*!< common message router header */
  /* Message payload */
  uint8                               id;           /*!< Watermark id*/
  nb1_errno_e                         cnf_status;   /*!< confimation status message */
} nb1_rlcdl_embms_wm_deregister_cnf_msg_s;

/*! @brief QSH analysis process request message for RLCDL
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;          /*!< common message router header */
  /*! whether to run analysis, reset stats ... */
  uint32 action;
  /*! category mask. Use enum qsh_cat_e values to define mask e.g.
      QSH_CAT_CFG|QSH_CAT_PERF (since the values are bit mask simple
      "or" will work) */
  uint32 category_mask;
  /*! whether the registered CB can run in sync/async mode.
      When called with async_allowed = false the clients MUST RUN
      the registered CB in the same task context which invoked their call back
      (e.g. error fatal). When called with async_allowed = true
      the clients have an option to run their registered CB in async mode. */
  boolean async_allowed;
} nb1_rlcdl_qsh_req_msg_s;

/*! @brief QSH analysis process request message for RLCUL
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;          /*!< common message router header */
  /*! whether to run analysis, reset stats ... */
  uint32 action;
  /*! category mask. Use enum qsh_cat_e values to define mask e.g.
      QSH_CAT_CFG|QSH_CAT_PERF (since the values are bit mask simple
      "or" will work) */
  uint32 category_mask;
  /*! whether the registered CB can run in sync/async mode.
      When called with async_allowed = false the clients MUST RUN
      the registered CB in the same task context which invoked their call back
      (e.g. error fatal). When called with async_allowed = true
      the clients have an option to run their registered CB in async mode. */
  boolean async_allowed;
} nb1_rlcul_qsh_req_msg_s;


/*! @brief signal data (SDU) request from RRC to RLC UL for SRB1 Bis
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;  /*!< message header */
  msgr_attach_s                       dsm_attach;
                                      /*!< dsm attachment
                                      contained signal message */
  /* Message payload */
  nb1_rb_cfg_idx_t                    rb_cfg_idx; /*!< RB cfg index */
  uint8                               mu_id; /*!< message unit identifier */
  boolean esm_data_transport; /*!< TRUE if data is for ESM (Extended Session Mgmt) from NAS */       
} nb1_rlcul_sdu_req_msg_s;

/*! @brief signal message (SDU) indication from RLC DL to RRC for SRB1 Bis
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;  /*!< message header */
  msgr_attach_s                       dsm_attach; /*!< dsm attachment contained signal message */
  /* Message payload */
  nb1_rb_cfg_idx_t                    rb_cfg_idx; /*!< RB Cfg index */
  nb1_sfn_s                           sfn;        /*!< The system and subframe number */
} nb1_rlcdl_sdu_ind_msg_s;


/*=============================================================================

                   EXTERNAL ENUMERATION TYPES

=============================================================================*/

/*! @brief UMID for all REQUEST accepted by RLCUL
*/
enum
{
  MSGR_DEFINE_UMID(NB1, RLCUL, REQ, CFG, 0x0, nb1_rlcul_cfg_req_s),
  MSGR_DEFINE_UMID(NB1, RLCUL, REQ, START, 0x1, nb1_rlc_start_req_s),
  MSGR_DEFINE_UMID(NB1, RLCUL, REQ, STOP, 0x2, nb1_rlc_stop_req_s),
  MSGR_DEFINE_UMID(NB1, RLCUL, REQ, STATS, 0x3, nb1_rlcul_stats_req_s),
  MSGR_DEFINE_UMID(NB1, RLCUL, REQ, QSH, 0x4, nb1_rlcul_qsh_req_msg_s),
  MSGR_DEFINE_UMID(NB1, RLCUL, REQ, ACK_TX_STS, 0x5, nb1_rlcul_ack_tx_sts_req_s),
  MSGR_DEFINE_UMID(NB1, RLCUL, REQ, SDU, 0x6, nb1_rlcul_sdu_req_msg_s),
  NB1_RLCUL_MAX_REQ,
  NB1_RLCUL_LAST_REQ = NB1_RLCUL_MAX_REQ - 1
} ;

/*! @brief UMID for all CONFIRMATION sent by RLCUL
*/
enum
{
  MSGR_DEFINE_UMID(NB1, RLCUL, CNF, CFG, 0x0, nb1_rlc_cfg_cnf_s),
  MSGR_DEFINE_UMID(NB1, RLCUL, CNF, START, 0x1, nb1_rlc_start_cnf_s),
  MSGR_DEFINE_UMID(NB1, RLCUL, CNF, STOP, 0x2, nb1_rlc_stop_cnf_s),
  MSGR_DEFINE_UMID(NB1, RLCUL, CNF, STATS, 0x3, msgr_hdr_s),
  MSGR_DEFINE_UMID(NB1, RLCUL, CNF, ACK_TX_STS, 0x4, nb1_rlcul_ack_tx_sts_cnf_s),
  MSGR_DEFINE_UMID(NB1, RLCUL, CNF, SDU, 0x5, nb1_rlcul_sdu_cnf_msg_s),
  NB1_RLCUL_MAX_CNF,
  NB1_RLCUL_LAST_CNF = NB1_RLCUL_MAX_CNF - 1
} ;

/*! @brief UMID for all INDICATIONS sent by RLCUL
*/
enum
{
  MSGR_DEFINE_UMID(NB1, RLCUL, IND, ACK, 0x0,
                   nb1_rlcul_ack_ind_s),
  MSGR_DEFINE_UMID(NB1, RLCUL, IND, FREE_PDU, 0x1,
                   nb1_rlcul_free_pdu_ind_s),
  MSGR_DEFINE_UMID(NB1, RLCUL, IND, RE_EST_STATUS, 0x2,
                   nb1_rlcul_re_est_status_ind_s),
  MSGR_DEFINE_UMID(NB1, RLCUL, IND, MAX_RETX, 0x3,
                   nb1_rlcul_max_retx_ind_s),
  MSGR_DEFINE_UMID(NB1, RLCUL, IND, FLUSH_RLCUL_LOG_BUF, 0x4,
                   nb1_rlcul_flush_rlcul_log_buf_ind_s),
  MSGR_DEFINE_UMID(NB1, RLCUL, IND, FLUSH_PDCPUL_LOG_BUF, 0x5,
                   nb1_rlcul_flush_pdcpul_log_buf_ind_s),
  MSGR_DEFINE_UMID(NB1, RLCUL, IND, STOP_STAT_PROHIBIT, 0x6,
                   nb1_rlcul_am_stop_stat_prohibit_ind_s),
  NB1_RLCUL_MAX_IND,
  NB1_RLCUL_LAST_IND = NB1_RLCUL_MAX_IND - 1
} ;

/*! @brief UMID for RLCUL periodic timer expiry
*/
enum
{
  MSGR_DEFINE_UMID(NB1, RLCUL, TMRI, PERIODIC_TIMER_EXP, 0x0, msgr_hdr_s),
  MSGR_DEFINE_UMID(NB1, RLCUL, TMRI, WDOG_TIMER_EXPIRED, 0x1, msgr_hdr_s),
  NB1_RLCUL_MAX_TMRI,
  NB1_RLCUL_LAST_TMRI = NB1_RLCUL_MAX_TMRI - 1
} ;

/*! @brief UMID for all REQUEST accepted by RLCDL
*/
enum
{
  MSGR_DEFINE_UMID(NB1, RLCDL, REQ, CFG, 0x0, nb1_rlcdl_cfg_req_s),
  MSGR_DEFINE_UMID(NB1, RLCDL, REQ, START, 0x1, nb1_rlc_start_req_s),
  MSGR_DEFINE_UMID(NB1, RLCDL, REQ, STOP, 0x2, nb1_rlc_stop_req_s),
  MSGR_DEFINE_UMID(NB1, RLCDL, REQ, FC, 0x3, cfm_fc_cmd_msg_s),
  MSGR_DEFINE_UMID(NB1, RLCDL, REQ, STATS, 0x4, nb1_rlcdl_stats_req_s),
  MSGR_DEFINE_UMID(NB1, RLCDL, REQ, EMBMS_REG, 0x5, nb1_rlcdl_embms_wm_register_req_msg_s),
  MSGR_DEFINE_UMID(NB1, RLCDL, REQ, EMBMS_DEREG, 0x6, nb1_rlcdl_embms_wm_deregister_req_msg_s),
  MSGR_DEFINE_UMID(NB1, RLCDL, REQ, QSH, 0x7, nb1_rlcdl_qsh_req_msg_s),
  MSGR_DEFINE_UMID(NB1, RLCDL, REQ, FC_LEVEL1, 0x8, cfm_fc_cmd_msg_s),
  NB1_RLCDL_MAX_REQ,
  NB1_RLCDL_LAST_REQ = NB1_RLCDL_MAX_REQ - 1
} ;

/*! @brief UMID for all CONFIRMATION sent by RLCDL
*/
enum
{
  MSGR_DEFINE_UMID(NB1, RLCDL, CNF, CFG, 0x0, nb1_rlc_cfg_cnf_s),
  MSGR_DEFINE_UMID(NB1, RLCDL, CNF, START, 0x1, nb1_rlc_start_cnf_s),
  MSGR_DEFINE_UMID(NB1, RLCDL, CNF, STOP, 0x2, nb1_rlc_stop_cnf_s),
  MSGR_DEFINE_UMID(NB1, RLCDL, CNF, STATS, 0x3, msgr_hdr_s),
  MSGR_DEFINE_UMID(NB1, RLCDL, CNF, EMBMS_REG, 0x4, nb1_rlcdl_embms_wm_register_cnf_msg_s),
  MSGR_DEFINE_UMID(NB1, RLCDL, CNF, EMBMS_DEREG, 0x5, nb1_rlcdl_embms_wm_deregister_cnf_msg_s),
  NB1_RLCDL_MAX_CNF,
  NB1_RLCDL_LAST_CNF = NB1_RLCDL_MAX_CNF - 1
} ;

/*! @brief UMID for all indications sent by RLCDL
*/
enum
{
  MSGR_DEFINE_UMID(NB1, RLCDL, IND, CTRL_PDU, 0x1, nb1_rlcdl_ctrl_pdu_ind_s),
  MSGR_DEFINE_UMID(NB1, RLCDL, IND, MCCH_PDU, 0x2, nb1_rlcdl_mcch_pdu_ind_s),
  MSGR_DEFINE_UMID(NB1, RLCDL, IND, SDU, 0x3, nb1_rlcdl_sdu_ind_msg_s),
  NB1_RLCDL_MAX_IND,
  NB1_RLCDL_LAST_IND = NB1_RLCDL_MAX_IND - 1
} ;

/*! @brief UMID for RLCDL timer
*/
enum
{
  MSGR_DEFINE_UMID(NB1, RLCDL, TMRI, EXPIRE, 0x0, msgr_hdr_s),
  MSGR_DEFINE_UMID(NB1, RLCDL, TMRI, EMBMS_SEG_DUR_TIMER_EXPIRED, 0x2, msgr_hdr_s),
  MSGR_DEFINE_UMID(NB1, RLCDL, TMRI, EMBMS_BURST_END_TIMER_EXPIRED, 0x3, msgr_hdr_s),
  MSGR_DEFINE_UMID(NB1, RLCDL, TMRI, EMBMS_ONEX_PAGE_DECODE_TIMER_EXPIRED, 0x4, msgr_hdr_s),
  NB1_RLCDL_MAX_TMRI,
  NB1_RLCDL_LAST_TMRI = NB1_RLCDL_MAX_TMRI - 1
} ;

/*! @brief Supervisory messages that RLC UL receives or throws
*/
enum
{
  MSGR_DEFINE_UMID(NB1, RLCUL, SPR, LOOPBACK, MSGR_ID_LOOPBACK,
                   msgr_spr_loopback_s),
  MSGR_DEFINE_UMID(NB1, RLCUL, SPR, LOOPBACK_REPLY, MSGR_ID_LOOPBACK_REPLY,
                   msgr_spr_loopback_reply_s),
  MSGR_DEFINE_UMID(NB1, RLCUL, SPR, THREAD_READY, LTE_APP_ID_THREAD_READY,
                   none),
  MSGR_DEFINE_UMID(NB1, RLCUL, SPR, THREAD_KILL, LTE_APP_ID_THREAD_KILL,
                   none)
};

#endif /* NB1_RLC_MSG_H */
