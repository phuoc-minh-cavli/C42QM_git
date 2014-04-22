/*!
  @file
  nb1_tlb.h

  @brief
  This file contains all External interfaces exported by Test Loopback Module.


*/

/*===========================================================================

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

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/nb1.mpss/2.2/api/nb1_tlb.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/08/17   ss      FR 44060 : CR 2070117 NB1 loopback Modes H
04/26/17   ss      FR 39221 : CR 2038743 Rel.13 LTE Cat.NB1 loopback Modes G

===========================================================================*/

#ifndef NB1_TLB_H
#define NB1_TLB_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include <dsm_item.h>
#include <dsm_init.h>
#include <dsm_queue.h>
#include <dsm_lock.h>

#include <comdef.h>
#include <pthread.h>
#include <msgr.h>
#include <msgr_nb1.h>
#include <lte_app.h>
#include <nb1_as.h>
/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*===========================================================================

  FUNCTION:  nb1_tlb_mode_gh_data_init

===========================================================================*/
/*!
    @brief
    This function is initialized all the TLB mode G data structures.

    @detail

    @return
    None.

    @see related_function()

*/
/*=========================================================================*/
EXTERN void nb1_tlb_mode_gh_data_init
(
  void
);

/*===========================================================================

  FUNCTION:  nb1_tlb_mode_gh_data_deinit

===========================================================================*/
/*!
    @brief
    This function is deinitialized all the TLB mode G data structures.

    @detail

    @return
    None.

    @see related_function()

*/
/*=========================================================================*/
EXTERN void nb1_tlb_mode_gh_data_deinit
(
  void
);

/*===========================================================================

  FUNCTION:  nb1_tlb_mode_gh_handle_ul_delay_timer_expiry_ind

===========================================================================*/
/*!
    @brief
    This function stores the DSM packet from DS
    item of the packet

    @detail
    This is called by tld ctrl task context

    @return
    Status of the operation

    @see related_function()

*/
/*=========================================================================*/
EXTERN void nb1_tlb_mode_gh_handle_ul_delay_timer_expiry_ind
(
);

/*===========================================================================

  FUNCTION:  nb1_tlb_mode_gh_handle_ds_dl_packet_msg

===========================================================================*/
/*!
    @brief
    This function stores the DSM packet from DS
    item of the packet

    @detail
    This is called by tld ctrl task context

    @return
    Status of the operation

    @see related_function()

*/
/*=========================================================================*/
EXTERN void nb1_tlb_mode_gh_handle_ds_dl_packet_msg
(
  dsm_item_type *dsm_pkt_ptr /*!< Input DS packet pointer*/
);

/*===========================================================================

  FUNCTION:  nb1_tlb_mode_gh_ds_send_dl_packets_wrapper

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
EXTERN boolean nb1_tlb_mode_gh_ds_send_dl_packets_wrapper
(
  dsm_item_type *dsm_pkt_ptr /*!< Input DS packet pointer*/
);
/*===========================================================================

  FUNCTION:  nb1_tlb_mode_gh_ds_update_wm_address_wrapper

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
EXTERN boolean nb1_tlb_mode_gh_ds_update_wm_address_wrapper
(
  dsm_watermark_type  *ds_wm_ptr /*!< Input DS water mark pointer*/
);

#endif /* NB1_TLB_H */
