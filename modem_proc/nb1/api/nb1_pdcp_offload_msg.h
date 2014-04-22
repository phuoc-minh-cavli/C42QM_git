/*!
  @file
  nb1_pdcp_offload_msg.h

  @brief
  This file contains all External message data structure and IDs interfaces
  exported by PDCP OFFLOAD task.


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

$Header: //components/rel/nb1.mpss/2.2/api/nb1_pdcp_offload_msg.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/15/18   ss      CR 2278167 : FR 50565: L2 task merge for NB
===========================================================================*/

#ifndef NB1_PDCPOFFLOAD_MSG_H
#define NB1_PDCPOFFLOAD_MSG_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <comdef.h>
#include <msgr.h>
#include <msgr_nb1.h>
#include <lte_app.h>
#include "nb1_l2_comdef.h"
#include <nb1_security.h>
#include "nb1_pdcp_msg.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/
/*-----------------------------------------------------------------------*/
/*     NB1 PDCP/RRC External message definitions                          */
/*-----------------------------------------------------------------------*/
/*! @brief ROHC seq len in bits
*/
#define NB1_PDCPOFFLOAD_SEQ_LEN 5

/*! @brief
  ROHC profile mask that indicates what ROHC profiles are supported
*/
typedef uint32 nb1_rohc_profile_mask_t;

/*! @brief configuration request from PDCPDL to PDCP OFFLOAD */

typedef struct
{
  nb1_rb_id_t                         rb_id;      /*!< RB ID */
  nb1_rb_cfg_idx_t                    rb_cfg_idx;
  nb1_eps_id_t                        eps_id;     /*!< EPS ID */
  uint16  max_cid;         /* Max value of context identifier              */
  nb1_pdcp_rohc_profile_mask_t  rohc_mask;    /*!< ROHC mask */

} nb1_pdcpdl_decomp_cfg_req_msg_s;

/*! @brief ROHC RB info definition for UL Data
*/
typedef struct
{
  nb1_rb_id_t      rb_id;   /*!< RB ID */
  nb1_rb_cfg_idx_t rb_cfg_idx; /*!< RB cfg index */
  nb1_eps_id_t   eps_id;     /*!< EPS ID */

  uint16          rohc_max_cid; /*!< ROHC max context ID */
  nb1_rohc_profile_mask_t  rohc_mask;    /*!< ROHC mask */

} nb1_pdcp_offload_comp_info_s;

/*! @brief ROHC info definition for DL Data
*/
typedef struct
{
  nb1_rb_id_t      rb_id;    /*!< RB ID */
  nb1_rb_cfg_idx_t rb_cfg_idx; /*!< RB cfg index */
  nb1_eps_id_t   eps_id;     /*!< EPS ID */

  uint16          rohc_max_cid; /*!< ROHC max context ID */
  nb1_pdcp_rohc_profile_mask_t  rohc_mask;    /*!< ROHC mask */

} nb1_pdcp_offload_decomp_info_s;


/*-----------------------------------------------------------------------*/
/*     NB1 PDCP/PS External message definitions                          */
/*-----------------------------------------------------------------------*/

/*! @brief Compressor packet indication from OFFLOAD to PDCPUL
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;         /*!< message header */
  /*message payload*/
  nb1_rb_cfg_idx_t      rb_cfg_idx;
  nb1_l2_cfg_reason_e                cfg_reason;
}nb1_pdcp_offload_comp_done_ind_msg_s;

/*! @brief RAB register request from PS to PDCP OFFLOAD for UL Data
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;         /*!< message header */
  /* Message payload */
  nb1_rb_cfg_idx_t                    rb_cfg_idx; /*!< RB cfg index */
  nb1_eps_id_t                        eps_id;     /*!< eps_id */
  nb1_pdcp_bearer_type_e              curr_state; /*!< Current cfg state of RB */
  nb1_pdcp_cfg_act_e                  cfg_act;    /*!< cfg_act */
} nb1_pdcp_offload_rab_dereg_ind_msg_s;

/*! @brief indication that is triggered when data to watermark is enqueued*/

typedef struct
{
  msgr_hdr_struct_type                hdr;  /*!< message header */
  nb1_rb_cfg_idx_t                    rb_cfg_idx; /*!< RB Cfg Index */
} nb1_pdcp_offload_wm_enqueue_ind_msg_s;

/*! @brief indication that is triggered when low watermark is crosssed
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;  /*!< message header */
  nb1_rb_cfg_idx_t                    rb_cfg_idx; /*!< RB Cfg Index */
} nb1_pdcp_offload_wm_low_ind_msg_s;


/*! @brief NB1_PDCPOFFLOAD_START_REQ request message definition
*/
typedef struct
{
  msgr_hdr_struct_type   msg_hdr;    /*!< common message router header */
} nb1_pdcp_offload_start_req_msg_s;


/*! @brief NB1_PDCPOFFLOAD_STOP_REQ request message definition
*/
typedef struct
{
  msgr_hdr_struct_type   msg_hdr;  /*!< common message router header */
} nb1_pdcp_offload_stop_req_msg_s;

/*-----------------------------------------------------------------------*/
/*     NB1 PDCP OFFLOAD External Indication messages                           */
/*-----------------------------------------------------------------------*/
/*! @brief external Indications that the NB1 PDCP OFFLOAD module sends  */
enum
{
  NB1_PDCPOFFLOAD_FIRST_IND = MSGR_UMID_BASE(MSGR_NB1_PDCPOFFLOAD, MSGR_TYPE_IND),

  MSGR_DEFINE_UMID( NB1,PDCPOFFLOAD,IND,WM_ENQUEUE,0x00,
                   nb1_pdcp_offload_wm_enqueue_ind_msg_s),
  MSGR_DEFINE_UMID( NB1,PDCPOFFLOAD,IND,WM_LOW,0x01,
                   nb1_pdcp_offload_wm_low_ind_msg_s),
  MSGR_DEFINE_UMID( NB1,PDCPOFFLOAD,IND,COMP_DONE,0x02,
                   nb1_pdcp_offload_comp_done_ind_msg_s),
  NB1_PDCPOFFLOAD_MAX_IND,
  NB1_PDCPOFFLOAD_LAST_IND = NB1_PDCPOFFLOAD_MAX_IND - 1
};

#endif /* NB1_PDCPOFFLOAD_MSG_H */
