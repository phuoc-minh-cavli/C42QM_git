/*!
  @file
  lte_tlb.h

  @brief
  This file contains all External interfaces exported by Test Loopback Module.


*/

/*===========================================================================

  Copyright (c) 2009 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //components/rel/lte.mpss/4.7.2/api/lte_tlb.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/10/19   ss      CR 2522694: Add repetitions CLOSE_TL_IND
12/26/17   ss      FR 46037 : CR 2163747 NB1 loopback Modes I
08/08/17   ss      FR 44060 : CR 2105586 NB1 loopback Modes H
04/26/17   ss      FR 39221 : CR 2010498 Rel.13 LTE Cat.NB1 loopback Modes G
09/13/12   st      CR 364517: Addded TLB Mode C enhancements
11/16/10   bn      Added API for DS to inform Close TL is ready
04/26/10   bn      Added in API to inform DS testloop open
04/20/10   bn      Updated the TLB Activation Test Loop API
03/08/10   bn      Modified API to have IP delay for TLB mode B
10/21/09   bn      Fixed the order of the protocol discrimimator/security
05/06/09   bn      Initial version
===========================================================================*/

#ifndef LTE_TLB_H
#define LTE_TLB_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <comdef.h>
#include <pthread.h>
#include <msgr.h>
#include <msgr_lte.h>
#include <lte_app.h>
#include <lte_as.h>
#include "lte_tlb_msg.h"
#include <dsm_queue.h>

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief define TLB/NAS message length container
*/
#define LTE_TLB_MAX_NAS_MSG_CONTAINER_SIZE   100

/*! @brief define TLB default Protocal descriptor and security
*/
#define LTE_TLB_PRO_DES_SECURITY_MODE_DEFAULT_HDR   0x0F

/*! @brief Enumeration of TLB mode type
*/
typedef enum
{
  LTE_TLB_MODE_A,
  LTE_TLB_MODE_B,
  LTE_TLB_MODE_C,
  LTE_TLB_MODE_G=6, /* * X3=1 and X2=1 and X1=0 then UE test loop mode G */
  LTE_TLB_MODE_H=7, /* * X3=1 and X2=1 and X1=1 then UE test loop mode H */
  LTE_TLB_MODE_I,
  LTE_TLB_MODE_INVALID
} lte_tlb_mode_e;

/*! @brief Data structure for TLB<->NAS message container
*/
typedef struct
{
  uint16 msg_size; /*!< Size of message in bytes */
  uint8 msg_payload[LTE_TLB_MAX_NAS_MSG_CONTAINER_SIZE]; /*!< Message data */
} lte_tlb_nas_msg_container_s;

/*! @brief Message structure for TLB DL info Indication
*/
typedef struct
{
  msgr_hdr_s hdr; /*!< Message router header */

  lte_tlb_nas_msg_container_s nas_msg_container; /*!<message container */
} lte_tlb_dl_info_ind_s;

/*! @brief Message structure for TLB UL info Request
*/
typedef struct
{
  msgr_hdr_s hdr; /*!< Message router header */

  lte_tlb_nas_msg_container_s nas_msg_container; /*!<message container */
} lte_tlb_ul_info_req_s;

/*! @brief Message structure for TLB UL info Request
*/
typedef struct
{
  msgr_hdr_s hdr; /*!< Message router header */
  
  dsm_item_type *dsm_pkt_ptr; /*!<DSM pkt pointer */
} lte_ds_dl_enqueue_ind_s;

/*! @brief Message structure for RRC CFG request
*/
typedef struct
{
  msgr_hdr_s hdr; /*!< Message router header */

  boolean             enable;  /*! TRUE - RRC enables the eMBMS stack
                                   FALSE - RRC disabled eMBMS stack */
  lte_mbsfn_area_id_t tlb_area_id; /*! Ranges from 0 - 255*/
  lte_pmch_id_t       tlb_pmch_id; /*! Ranges from 0 - 14 */
  uint8               tlb_lc_id;    /*! Ranges from 0 - 28 */
} lte_tlb_rrc_cfg_req_s;

/*! @brief Message structure for RRC CFG confirmation
*/
typedef struct
{
  msgr_hdr_s hdr; /*!< Message router header */
  boolean status; /*!< TRUE - eMBMS stack CFG success; FALSE - CFG fail */
} lte_tlb_rrc_cfg_cnf_s;

/*! @brief Message structure for DS close TL ready indication
*/
typedef struct
{
  msgr_hdr_s hdr; /*!< Message router header */
} lte_tlb_ds_close_tl_ready_ind_s;

/*! @brief Message structure for TLB LB CLOSE TL Indication
*/
typedef struct
{
  msgr_hdr_s hdr;       /*!< Message router header */
  lte_tlb_mode_e mode;  /*!< TLB mode */
  uint8  ip_ipu_delay;  /*!< IP delay in ms. Apply for Mode B only*/
  uint8  repetitions;   /*!< repetitions. Apply for Mode G/H only*/
} lte_tlb_lb_close_tl_ind_s;

/*! @brief Message structure for TLB Activation Indication
*/
typedef struct
{
  msgr_hdr_s hdr;       /*!< Message router header */
  lte_tlb_mode_e  mode; /*!< TLB mode: Mode A or B*/
} lte_tlb_lb_activation_ind_s;

/*! @brief Message structure for TLB Open testloop Ind
*/
typedef struct
{
  msgr_hdr_s hdr;       /*!< Message router header */
  lte_tlb_mode_e  mode; /*!< TLB mode: Mode A or B*/
} lte_tlb_lb_open_ind_s;

/*! @brief structure for Update Location Info
*/
  /*! @brief Define Location Sign
  */
#define LTE_TLB_CTRL_LOCATION_INFO_DIRECTION  0x80
typedef struct
{
  msgr_hdr_s hdr;       /*!< Message router header */
  boolean valid;
  uint8 lat_sign; /* 0-north, 1-south*/
  uint32 latitude;
  int longitude;
  uint8 alt_sign; /* 0-height, 1-depth*/
  uint16 altitude;
  uint16 heading; 
  uint16 horizontal_speed;
  uint32 gnss_tod;
} lte_tlb_ctrl_update_loc_info_s;


/*! @brief structure for close test loop mode C configuration
*/
typedef struct
{
  boolean             via_rlc_am_sap;
  uint8               repetitions;
  uint8               uplink_data_delay;
} lte_tlb_ctrl_mode_g_cfg_s;

typedef lte_tlb_rrc_cfg_cnf_s nb1_tlb_rrc_cfg_cnf_s;

typedef lte_tlb_rrc_cfg_req_s nb1_tlb_rrc_cfg_req_s;

/*===========================================================================

  FUNCTION:  lte_tlb_ctrl_task_init

===========================================================================*/
/*!
    @brief
    This function is a initialization function for TLB Control Task.

    @detail
    Initialization consists of:
    - Initialization TLB control attributes
    - Initialization stack size and priority
    - Starting the TLB Control Task

    @return
    Thread ID

    @note

    @see related_function()

*/
/*=========================================================================*/
EXTERN pthread_t lte_tlb_ctrl_task_init
( 
  int priority  /*!< thread priority passed in by app mgr */
);

/*===========================================================================

  FUNCTION:  lte_tlb_mode_a_scaling_pdcp_packet

===========================================================================*/
/*!
    @brief
    This function is used by DS to scaling the DL PDCP packet to make it 
    become the UL PDCP packet according to the mapping/scaling configuration
    in the TLB Control module.

    @detail
    - input1 : DSM pointer to the PDCP packet. TLB mode A will reformat/
	scaling the packet. It will modify the current input DSM pointer
    - input2 : RB ID of the current input PDCP packet

    @return
    Status = TRUE 
	  if the operation is success which results a valid UL packet 
    Status = FALSE
      if the operation is failed due to one of the following reasons
        1) the test loop A is open or not active 
        2) The Dl packet size is zero or UL scaling size is zero
    When Status = FALSE, consider it as there is no UL packet

    @see related_function()

*/
/*=========================================================================*/
EXTERN boolean lte_tlb_mode_a_scaling_pdcp_packet
(
  dsm_item_type   **pdcp_pkt_ptr, /*!< input/output PDCP packet pointer*/
  lte_rb_id_t     rb_id          /*!< RB ID of the pdcp_pkt_ptr*/
);

/*===========================================================================

  FUNCTION:  lte_tlb_mode_c_packet_count

===========================================================================*/
/*!
    @brief
    This function increases the eMBMS packet counter and releases the DSM
    item of the packet

    @detail
    This is called by data services within their task context

    @return
    Status of the operation

    @see related_function()

*/
/*=========================================================================*/
EXTERN boolean lte_tlb_mode_c_packet_count
(
  dsm_item_type   **pdcp_pkt_ptr /*!< Input PDCP packet pointer*/
);

/*===========================================================================

  FUNCTION:  lte_tlb_ctrl_get_tlb_mode_gh_cfg_ptr

===========================================================================*/
/*!
    @brief
    Function call to get the pointer to the data structure of the TLB control

    @detail

    @return
    void

    @note

    @see related_function()
*/
/*=========================================================================*/
EXTERN lte_tlb_ctrl_mode_g_cfg_s *lte_tlb_ctrl_get_tlb_mode_gh_cfg_ptr
( 
  void
);

/*===========================================================================

  FUNCTION:  nb1_tlb_mode_gh_ds_update_wm_address

===========================================================================*/
/*!
    @brief
    This function store the DS UL watermark pointer 
    to send back loopback data in uplink

    @detail
    This is called by data services within their task context

    @return
    Status of the operation

    @see related_function()

*/
/*=========================================================================*/
EXTERN boolean nb1_tlb_mode_ghi_ds_update_wm_address
(
  dsm_watermark_type  *ds_wm_ptr /*!< Input DS water mark pointer*/
);

/*===========================================================================

  FUNCTION:  nb1_tlb_mode_gh_send_dl_packets

===========================================================================*/
/*!
    @brief
    This function buffer DL DSM item from DS to local DSM
    item of the packet

    @detail
    This is called by data services within their task context

    @return
    Status of the operation

    @see related_function()

*/
/*=========================================================================*/
EXTERN boolean nb1_tlb_mode_ghi_ds_send_dl_packets
(
  dsm_item_type *dsm_pkt_ptr /*!< Input DS packet pointer*/
);

#endif /* LTE_TLB_H */
