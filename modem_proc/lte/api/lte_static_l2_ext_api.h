/*!
  @file lte_static_l2_ext_api.h

  @brief
  Pending queue implementation for RRC dispatcher.

  @detail
  - This code uses the lockless circular queue implementation in lte. Since
    lockless circular queues are not a good match for the needs of the pending
    queue, the implementation is not optimized for speed. Depending on need,
    we will have to re-visit this design at a later point. The best implementation
    might be to use qube style queues.

*/

/*===========================================================================

  Copyright (c) 2008-2022 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //components/rel/lte.mpss/4.7.2/api/lte_static_l2_ext_api.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/27/22   ve      CR 3072213: API to share current CE level
09/28/21   ss      CR 3045224 : FR71043:API to disable the Non-zero BSR feature
04/07/21   ve      CR 2909375 : Conn sus/resume fixes for OOS, MARB and PSM
02/02/18   ss      Initial Revision

===========================================================================*/

#ifndef LTE_STATIC_L2_EXT_API_H
#define LTE_STATIC_L2_EXT_API_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <lte_pdcp_ext_api.h>
#include <nb1_pdcp_ext_api.h>
#include <lte_rlc_ext_api.h>

typedef enum
{
  L2_SYS_TYPE_LTE,
  L2_SYS_TYPE_NB1,
} lte_static_l2_sys_type_e;

typedef uint64 (*lte_pdcpul_get_pdu_bytes_tx_t)
(
  lte_eps_id_t
);

typedef void (*lte_pdcpul_tag_each_pkt_t)
(
  dsm_item_type *
);

typedef void (*lte_pdcpul_tag_and_enqueue_comp_pkt_t)
(
  dsm_watermark_type *,
  dsm_item_type *
);

typedef void (*lte_rlc_get_delay_metric_t)
(
  lte_rlc_delay_metric_s *,
  lte_rlc_delay_metric_s *,
  uint8
);

typedef void (*lte_rlc_1x_page_wakeup_notification_t)
(
  uint16
);

typedef boolean (*lte_pdcpdl_get_activity_statistics_t)
(
  lte_eps_id_t,
  lte_pdcpdl_activity_stats_s *
);

typedef boolean (*nb1_pdcpdl_get_activity_statistics_t)
(
  nb1_eps_id_t,
  nb1_pdcpdl_activity_stats_s *
);

typedef void (*lte_mac_ul_data_set_gps_padding_status_t)
(
  boolean
);

typedef uint32 (*lte_mac_qos_get_allowed_tput_t)
(
  void
);

typedef uint8 (*nb1_mac_get_curr_celevel_t)
(
  void
);

typedef uint8 (*mac_get_curr_celevel_t)
(
  void
);

typedef void (*l2_init_req_t)
(
  void
);

typedef void (*l2_deinit_req_t)
(
  void
);

typedef void (*lte_nb_static_enable_rai_t)
(
  boolean
);


typedef struct
{
  
  lte_pdcpul_tag_each_pkt_t                pdcpul_tag_each_pkt;
  lte_pdcpul_get_pdu_bytes_tx_t            pdcpul_get_pdu_bytes;
  lte_pdcpdl_get_activity_statistics_t     pdcpdl_get_activity_statistics;
  nb1_pdcpdl_get_activity_statistics_t     nb1_pdcpdl_get_activity_statistics;
  lte_mac_ul_data_set_gps_padding_status_t mac_ul_data_set_gps_padding_status;
  lte_mac_qos_get_allowed_tput_t           mac_qos_get_allowed_tput;
  mac_get_curr_celevel_t                   mac_get_curr_celevel;
  l2_init_req_t                            l2_init_req;
  l2_deinit_req_t                          l2_deinit_req;
  lte_nb_static_enable_rai_t               lte_nb_static_enable_rai;
} lte_static_l2_ext_api_s;

typedef struct
{
  boolean context_save_valid;
  
  // TODO:
  //lte_rb_rohc_ctx_s       nb1_rb_rohc_ctx; /*!< TODO: ROHC */
  // TODO:
  //lte_rb_rohc_ctx_s       nb1_rb_rohc_ctx; /*!< TODO: ROHC */

} lte_pdcp_shim_db_s;

typedef struct
{
  boolean context_save_valid;
  
  // TODO:
  //lte_rb_rohc_ctx_s       nb1_rb_rohc_ctx; /*!< TODO: ROHC */
  // TODO:
  //lte_rb_rohc_ctx_s       nb1_rb_rohc_ctx; /*!< TODO: ROHC */

} nb1_pdcp_shim_db_s;

extern lte_static_l2_ext_api_s * lte_l2_get_lte_vtbl
(
  void
);


extern lte_static_l2_ext_api_s * nb1_l2_get_nb1_vtbl
(
  void
);

extern void lte_static_l2_init_req
(
  void
);

void lte_static_l2_set_active_sys_type
(
  lte_static_l2_sys_type_e sys_type
);

extern uint8 lte_nb1_mac_get_curr_celevel
(
  void
);

/*===========================================================================

  FUNCTION:  lte_get_pdcp_shim_context

===========================================================================*/
/*!
  @brief
  returns pdcp and rohc context

  @detail
  

  @return
  returns shim db.
*/
/*=========================================================================*/
lte_pdcp_shim_db_s * lte_get_pdcp_shim_context
(
  void 
);

/*===========================================================================

  FUNCTION:  nb1_get_pdcp_shim_context

===========================================================================*/
/*!
  @brief
  returns pdcp and rohc context

  @detail
  

  @return
  returns shim db.
*/
/*=========================================================================*/
nb1_pdcp_shim_db_s * nb1_get_pdcp_shim_context
(
  void
);

/*===========================================================================

  FUNCTION:  lte_nb_static_enable_rai

===========================================================================*/
/*!
    @brief
    External Call from Upper layer to enable the RAI

    @return
    boolean.

*/
/*=========================================================================*/
EXTERN void l2_static_enable_rai
(
  boolean enable
);


#endif /* LTE_STATIC_RRC_EXT_API_H */

