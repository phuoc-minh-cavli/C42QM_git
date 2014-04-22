/*!
  @file
  vstmr_xo.h

  @brief
  VSTMR XO header file.

*/

/*===========================================================================

  Copyright (c) 2018 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

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

$Header: //components/rel/mcs.mpss/5.1/api/vstmr_xo.h#3 $

===========================================================================*/

#ifndef VSTMR_XO_H
#define VSTMR_XO_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <comdef.h>
#include "vstmr.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief XO event table management structure.
*/
typedef struct vstmr_xo_event_tbl_s_
{
  vstmr_event_tbl_s     event_tbl_data;     /*!< Internal event table data, stores
                                                 event expiration counts in XO
                                                 resolution. */
  uint32                period;             /*!< Period in XO resolution for the
                                                 event table. */

  void (*isr_callback)(struct vstmr_xo_event_tbl_s_ *event_tbl, void *arg, uint32 pending);
  /*!< Callback routine to be called once an event expires */

  void *isr_arg;                            /*!< Argument for the callback routine */

  atomic_word_t         service_sync_flag;  /*!< Flag to synchronize event table
                                                 servicing and deregistration. */

  atomic_word_t         isr_sync_flag;      /*!< Flag to synchronize ISR handler and
                                                 interrupt deregistration. */

} vstmr_xo_event_tbl_s;

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

extern void vstmr_xo_event_tbl_register
(
  vstmr_xo_event_tbl_s  *event_tbl,
  vstmr_int_types_e      int_type
);

extern void vstmr_xo_event_tbl_deregister
(
  vstmr_xo_event_tbl_s  *event_tbl
);

extern uint32 vstmr_xo_event_service
(
  vstmr_xo_event_tbl_s  *event_tbl
);

extern void vstmr_xo_interrupt_register
(
  vstmr_xo_event_tbl_s  *event_tbl,
  void (*cb)(vstmr_xo_event_tbl_s *event_tbl, void *arg, uint32 pending),
  void *cb_arg
);

extern void vstmr_xo_interrupt_deregister
(
  vstmr_xo_event_tbl_s  *event_tbl
);

extern void vstmr_xo_event_sched
(
  vstmr_xo_event_tbl_s    *id,
  uint32                   event_idx,
  uint32                   xo_cnt
);

extern void vstmr_xo_event_cancel
(
  vstmr_xo_event_tbl_s  *id,            
  uint32                 event_idx      
);

extern vstmr_vfr_e vstmr_xo_vfr_allocate
(
  vstmr_vfr_e            vfr_id
);

extern void vstmr_xo_vfr_start
(
  vstmr_vfr_e            vfr_id,
  uint32                 xo_cnt,
  uint32                 period
);

extern void vstmr_xo_vfr_release
(
  vstmr_vfr_e            vfr_id
);

#ifdef __cplusplus
}  // extern "C"
#endif


#endif /* VSTMR_XO_H */
