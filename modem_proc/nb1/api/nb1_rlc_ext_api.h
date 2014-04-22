/*!
  @file
  nb1_rlc_ext_api.h

  @brief
  NB1 RLC API C header file.
*/

/*==============================================================================

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

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/nb1.mpss/2.2/api/nb1_rlc_ext_api.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
07/15/18   ss      CR 2278167 : FR 50565: L2 task merge for NB
==============================================================================*/

#ifndef NB1_RLC_EXT_API_H
#define NB1_RLC_EXT_API_H

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include <comdef.h>
#include <dsm_item.h>
#include <mutils_circ_q.h>
#include "nb1_as.h"

/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/
/*! @brief Macro to enqueue the DSM item to the passed PDCP-RLC watermark queue.
 *  Following are the details of function which this macro calls internally.

    FUNCTION: nb1_dsmi_enqueue_with_length_in_app_field()

    This function enqueues the DSM item to the passed watermark queue., then
    check for and trigger relevant 'put' events.

    This function doesn't check for the priority and will NOT enqueue an item
    to the front of the queue.

    @note
    Caller of this function is expected to make sure that app_field is not
    used as part of the watermark interface because this function will over-
    write the same with 'item_length'.

    'item_length' can possibly be set to user defined length which may or may
    not be equal to the enqueued dsm packet length.

    If enqueue fails then this API will release the passed DSM item and set the
    passed *pkt_ptr to NULL.

    @see related_function()
    nb1_dsmi_dequeue_with_length_in_app_field() - "app_field" of the first
    dsm item of the dequeued packet will be used for updating the watermark
    count whenever an item is dequeued from the watermark

    @return
    TRUE or FALSE based on enqueue status
    This function also sets the passed *pkt_ptr to NULL.
*/
#define NB1_RLC_DSM_ENQUEUE(wm_ptr, pkt_head_ptr, item_length) \
  nb1_dsm_enqueue_with_length_in_app_field(wm_ptr, pkt_head_ptr, item_length)


/*! @brief Macro to dequeue the DSM item to the passed PDCP-RLC watermark queue.
 * Following are the details of function which this macro calls internally.

    FUNCTION: nb1_dsmi_dequeue_with_length_in_app_field()

    This function dequeues and return a pointer to the next item on the
    watermark queue associated with the passed watermark item.

    This function will also update the 'current_cnt' field using "app_field" of
    the first item of the dequeued dsm packet.

    After updating the count, it checks for and perform any relevent 'get'
    events.

    @note
    The parameter must NOT be NULL.

    @return
    A pointer to a 'dsm_item_type' or NULL if no item_array available.
*/
#define NB1_RLC_DSM_DEQUEUE(wm_ptr) \
  dsm_dequeue(wm_ptr)

/*! @brief definition of triggers for the RLC SDU delivery to PDCP
*/
typedef enum
{
  NB1_RLCDL_SDU_DEL_REASON_HO = 0,   /*!< SDU delievery due to handover */
  NB1_RLCDL_SDU_DEL_REASON_NORMAL,   /*!< Delievery not due to handover */
  NB1_RLCDL_SDU_DEL_REASON_MAX       /*!< MAX */
} nb1_rlcdl_sdu_del_reason_e;

/*! @brief RLC-PDCP DL circular RLC SDU buffer element structure
*/
typedef struct
{
  nb1_rb_cfg_idx_t rb_cfg_idx;       /*!< unique RB configuration index */
  nb1_rlcdl_sdu_del_reason_e reason; /*!< trigger for the SDU delivery */
  nb1_sfn_s                  sfn;       /*!< sfn at which SDUs delivered */
  dsm_item_type*   head_ptr;         /*!< dsm pointer to the 1st RLC SDU */
} nb1_rlcdl_sdu_buffer_element_s;

/*! @brief definition of delay metrics */
typedef struct
{
  uint16 max;          /*!< Maximal average value */
  uint16 min;          /*!< Minimal average value */
  uint16 avg;          /*!< current average value */
  uint32 num_samples;  /*!< number of samples */
}nb1_rlc_delay_metric_s;

/*==============================================================================

  FUNCTION:  nb1_rlc_reset_harq_2_pdu_fields

==============================================================================*/
/*!
    @brief
    This function resets harq_2_pdu_map data structure.

    @detail
    It is only required to reset num_pdus field. If num_pdus field is zero
    we ignore rest of the fields. Reset should be done for every first
    PDU built for that particular HARQ.

    @return
    void

*/
/*============================================================================*/
extern void nb1_rlc_reset_harq_2_pdu_fields
(
  uint8 harq_id
);

/*==============================================================================

  FUNCTION:  nb1_rlc_reset_scratch_harq_info

==============================================================================*/
/*!
    @brief
    This function resets num_pdu entry of scratch harq info.

    @detail
    This function is called before a packet build. This will reset the num_pdus
    field of scratch harq to 0.

    @return
    void

*/
/*============================================================================*/
void nb1_rlc_reset_scratch_harq_info
(
  uint8 harq_id
);

/*==============================================================================

  FUNCTION:  nb1_rlc_switch_curr_index

==============================================================================*/
/*!
    @brief
    This function switches the ping pong buffer index.

    @detail
    After the harq_2_pdu_map data structure for particular harq_id is updated
    for one of the ping pong buffer, this function is called to switch the
    index and make this buffer available for retx enqueue operation.

    @return
    void

*/
/*============================================================================*/
void nb1_rlc_switch_curr_index
(
  uint8 harq_id
);

/*==============================================================================

  FUNCTION:  nb1_rlc_get_delay_metric

==============================================================================*/
/*!
    @brief
    This function get the delay metrics including rtt and queue delay

    @detail
    This will be called by DS module and return with both rtt and queue delay

    @return
    void

*/
/*============================================================================*/
EXTERN void nb1_rlc_get_delay_metric
(
  nb1_rlc_delay_metric_s* rtt,         /*!< Pointer to rtt metric structure */
  nb1_rlc_delay_metric_s* queue_delay, /*!< Pointer to delay queue metric structure */
  uint8 eps_id                         /*!< eps ID */
);

/*==============================================================================

  FUNCTION:  nb1_rlc_1x_page_wakeup_notification

==============================================================================*/
/*!
    @brief
    This function is called to indciate NB1 L2 about the 1x page notification.

    @return
    void

*/
/*============================================================================*/
EXTERN void nb1_rlc_1x_page_wakeup_notification
(
  uint16 onex_page_cycle_in_ms
);

#endif /* NB1_RLC_EXT_API_H */
