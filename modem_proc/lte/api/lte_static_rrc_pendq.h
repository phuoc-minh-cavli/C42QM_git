/*!
  @file lte_static_rrc_pendq.h

  @brief
  Header defs for RRC pending queue implementation.

*/

/*===========================================================================

  Copyright (c) 2008 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //components/rel/lte.mpss/4.7.2/api/lte_static_rrc_pendq.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/17/08   ask     Initial Revision

===========================================================================*/

#ifndef LTE_STATIC_RRC_PENDQ_H
#define LTE_STATIC_RRC_PENDQ_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <lte.h>
#include <queue.h>
#include <msgr.h>

#include "lte_static_rrc_dispatcher.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief Callback for checking if pended message is ready to be processed
*/
typedef boolean (*lte_static_rrc_pendq_cb)(byte *msg_ptr, uint32 msg_len);

/*! @brief  Compare function to check if a particular msg exists in the queue or not
*/
typedef int (*lte_static_rrc_pendq_cmp)(void * item_ptr, void * compare_val);


/*! @brief Structure defn for RRC pending queue item
*/
typedef struct
{
  q_link_type       link;       /*!< Queue link */

  lte_static_rrc_msg_u     *msg_ptr;        /*!< Message payload */
  uint32            msg_len;    /*!< Message length */
  lte_static_rrc_pendq_cb  cb_fn;      /*!< Ptr to callback func */

} lte_static_rrc_pendq_queue_item_s;

typedef lte_static_rrc_pending_msg_indi_s lte_rrc_pending_msg_indi_s;

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/


void lte_static_rrc_pendq_init
(
  void
);

lte_static_rrc_pendq_queue_item_s *lte_static_rrc_pendq_put
(
  msgr_hdr_s *msg_ptr,            /*!< Ptr to the message buffer */
  uint32 msg_len,           /*!< Length of the message */
  lte_static_rrc_pendq_cb cb_fn    /*!< Callback function to check if pended msg 
                                 is ready to be processed */
);

lte_static_rrc_pendq_queue_item_s *lte_static_rrc_pendq_get
(
  msgr_hdr_s **msg_ptr,       /*!< Msg ptr to return */
  uint32 *msg_len_ptr   /*!< Length of the returned message */
);

void lte_static_rrc_pendq_free
(
  lte_static_rrc_pendq_queue_item_s *item_ptr  /*!< Ptr to a previously pended queue item */
);

void lte_static_rrc_pendq_reset
(
  void
);

void lte_static_rrc_pendq_clear_non_shim_cmds
(
  void
);

boolean lte_static_rrc_pendq_free_umid
(
  msgr_umid_type msg_id     /*!< UMID */
);

boolean lte_static_rrc_pendq_find_umid
(
  msgr_umid_type msg_id     /*!< UMID */
);

uint8 lte_static_rrc_pendq_get_umid_cnt
(
  msgr_umid_type msg_id     /*!< UMID */
);

boolean lte_static_rrc_pendq_search_and_insert
(
  msgr_hdr_s *msg_ptr,            /*!< Ptr to the message buffer */
  uint32 msg_len,           /*!< Length of the message */
  lte_static_rrc_pendq_cmp cmp_fn,    /*!< Compare function to check if the msg is in the queue or not */
  lte_static_rrc_pendq_cb cb_fn,    /*!< Callback function to check if pended msg 
                                 is ready to be processed */
  boolean insert /*!< Boolean to indicate whether insertion is necessary */
);

boolean lte_static_rrc_pendq_search_and_delete
(
  lte_static_rrc_pendq_cmp cmp_fn    /*!< Compare function to check if the msg is in the queue or not */
);

boolean lte_static_rrc_pendq_get_umid
(
  msgr_umid_type msg_id,     /*!< UMID */
  lte_static_rrc_pendq_queue_item_s **item_ptr_ptr  /*!< Item ptr reference to continue search */
);

#endif /* LTE_STATIC_RRC_PENDQ_H */
