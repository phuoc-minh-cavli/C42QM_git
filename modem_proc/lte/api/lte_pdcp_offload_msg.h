/*!
  @file
  lte_pdcp_offload_msg.h

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

$Header: //components/rel/lte.mpss/4.7.2/api/lte_pdcp_offload_msg.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/13/18   ss      CR2278166: FR 50565: L2 task merge for CATM
06/26/15   sb      CR862079: Change to upgrade PDCP support to
                   ELS spec ver D and UDC ver F
05/13/15   sb      CR 803880: modified PDCP interface for UDC feature
11/11/14   sb      CR748751: For RLF do not enable UL flow after recompression
09/19/14   sb      CR706535: Defense mechanism in UE to counter HFN mismatch
09/04/14   sb      CR694693: Fix to ensure UL PDUs are not built after RAB
                   is released
08/05/14   sb      CR696130: Reset Ciphering parameter in OFFLOAD when security 
                   parameter changes
04/02/14   sb      CR642230: REL11 RoHC context continuity during HO 
                   within same eNB
06/19/13   sb      CR458155: Set the reason for RoHC Reset (HO/Discard) so that
                   OFFLOAD calls the correct RoHC IOCTL
05/24/13   sb      CR491559: Free metainfo for packets sent for Re-RoHC and reallocate
                   them on recompression of packets
12/19/12   sb      CR431445: Have common A2 watermark for both PDCP and OFFLOAD tasks.
                   Remove  OFFLOAD-A2 warermark and use only PDCP-A2 watermark
06/16/12   sb      CR365196 and CR365004: Fix to handle 
                   scenarios where RB switches back and forth with RoHC enabled and 
                   disabled. Also to reduce the delay in UL after HO
04/22/12   sb      Initial version

===========================================================================*/

#ifndef LTE_PDCPOFFLOAD_MSG_H
#define LTE_PDCPOFFLOAD_MSG_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <comdef.h>
#include <msgr.h>
#include <msgr_lte.h>
#include <lte_app.h>
#include "lte_l2_comdef.h"
#include <lte_security.h>
#include "lte_pdcp_msg.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/
/*-----------------------------------------------------------------------*/
/*     LTE PDCP/RRC External message definitions                          */
/*-----------------------------------------------------------------------*/
/*! @brief ROHC seq len in bits
*/
#define LTE_PDCPOFFLOAD_SEQ_LEN 5

/*! @brief
  ROHC profile mask that indicates what ROHC profiles are supported
*/
typedef uint32 lte_rohc_profile_mask_t;

/*! @brief configuration request from PDCPUL to PDCP OFFLOAD */

typedef struct
{
  msgr_hdr_struct_type                hdr;  /*!< message header */
  /* Message payload */
  lte_rb_id_t                         rb_id;      /*!< RB ID */
  lte_rb_cfg_idx_t                    rb_cfg_idx;
  lte_eps_id_t                        eps_id;     /*!< EPS ID */
  uint16  max_cid;         /* Max value of context identifier              */
  lte_pdcp_rohc_profile_mask_t  rohc_mask;    /*!< ROHC mask */

} lte_pdcpul_comp_cfg_req_msg_s;


/*! @brief configuration request from PDCPDL to PDCP OFFLOAD */

typedef struct
{
  lte_rb_id_t                         rb_id;      /*!< RB ID */
  lte_rb_cfg_idx_t                    rb_cfg_idx;
  lte_eps_id_t                        eps_id;     /*!< EPS ID */
  uint16  max_cid;         /* Max value of context identifier              */
  lte_pdcp_rohc_profile_mask_t  rohc_mask;    /*!< ROHC mask */

} lte_pdcpdl_decomp_cfg_req_msg_s;

/*! @brief ROHC RB info definition for UL Data
*/
typedef struct
{
  lte_rb_id_t      rb_id;   /*!< RB ID */
  lte_rb_cfg_idx_t rb_cfg_idx; /*!< RB cfg index */
  lte_eps_id_t   eps_id;     /*!< EPS ID */

  uint16          rohc_max_cid; /*!< ROHC max context ID */
  lte_rohc_profile_mask_t  rohc_mask;    /*!< ROHC mask */

} lte_pdcp_offload_comp_info_s;

/*! @brief ROHC info definition for DL Data
*/
typedef struct
{
  lte_rb_id_t      rb_id;    /*!< RB ID */
  lte_rb_cfg_idx_t rb_cfg_idx; /*!< RB cfg index */
  lte_eps_id_t   eps_id;     /*!< EPS ID */

  uint16          rohc_max_cid; /*!< ROHC max context ID */
  lte_pdcp_rohc_profile_mask_t  rohc_mask;    /*!< ROHC mask */

} lte_pdcp_offload_decomp_info_s;


/*-----------------------------------------------------------------------*/
/*     LTE PDCP/PS External message definitions                          */
/*-----------------------------------------------------------------------*/

/*! @brief indication that is triggered when data to watermark is enqueued*/

typedef struct
{
  msgr_hdr_struct_type                hdr;  /*!< message header */
  lte_rb_cfg_idx_t                    rb_cfg_idx; /*!< RB Cfg Index */
} lte_pdcp_offload_wm_enqueue_ind_msg_s;

/*! @brief indication that is triggered when low watermark is crosssed
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;  /*!< message header */
  lte_rb_cfg_idx_t                    rb_cfg_idx; /*!< RB Cfg Index */
} lte_pdcp_offload_wm_low_ind_msg_s;

/*-----------------------------------------------------------------------*/
/*     LTE PDCP OFFLOAD External Indication messages                           */
/*-----------------------------------------------------------------------*/
/*! @brief external Indications that the LTE PDCP OFFLOAD module sends  */
enum
{
  LTE_PDCPOFFLOAD_FIRST_IND = MSGR_UMID_BASE(MSGR_LTE_PDCPOFFLOAD, MSGR_TYPE_IND),
  	
  MSGR_DEFINE_UMID(LTE,PDCPOFFLOAD,IND,WM_ENQUEUE,0x00,
                   lte_pdcp_offload_wm_enqueue_ind_msg_s),
  MSGR_DEFINE_UMID(LTE,PDCPOFFLOAD,IND,WM_LOW,0x01,
                   lte_pdcp_offload_wm_low_ind_msg_s),  
  LTE_PDCPOFFLOAD_MAX_IND,
  LTE_PDCPOFFLOAD_LAST_IND = LTE_PDCPOFFLOAD_MAX_IND - 1
};

#endif /* LTE_PDCPOFFLOAD_MSG_H */
