#ifndef LTE_ML1_OFFLOAD_STATIC_TASK_H
#define LTE_ML1_OFFLOAD_STATIC_TASK_H
/*!
  @file lte_ml1_mclk_msg.h

  @brief
  A header describing the messages to the mclk driver.

  Details...
  
*/

/*==============================================================================

  Copyright (c) 2009-2018 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //components/rel/lte.mpss/4.7.2/api/lte_ml1_offload_static_task.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
12/18/12   nsh      Initial Checkin

==============================================================================*/

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include <comdef.h>
#include <msgr.h>
#include <msgr_types.h>
#include <msgr_lte.h>

#include <pthread.h>

/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/

/*! @brief Holds ML1 related information like mailboxes, CS environment
        pointer and ML1 context.
*/

typedef union {
  /* Supervisor Requests */
  msgr_spr_loopback_s                   msgr_spr_loopback;
  msgr_spr_loopback_reply_s             msgr_spr_loopback_reply;
} offload_static_msg_u;

typedef struct 
{
  boolean                      task_is_initialized;    /*!< Flag set when the task gets initialized */
  pthread_t                    thread_id;              /*!< task thread id */
  pthread_attr_t               attr;                   /*!< task attr */
  msgr_client_t                offload_msgr_client;    /*!< MSGR client pointer */
  msgr_id_t                    sup_mb;                 /*!< Supervisory messages mailbox */
  offload_static_msg_u         static_rcv_msg;         /*!< Static message buffer */  
}lte_ml1_offload_task_context_s;


/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================

  FUNCTION:  lte_ml1_offload_static_get_client_ptr

===========================================================================*/
/*!
  @brief
  offload task msgr client attached to offload static task.

  @return
  returns client ptr attached to offload static task.
*/
/*=========================================================================*/
msgr_client_t lte_ml1_offload_static_get_client_ptr(void);

/*===========================================================================

  FUNCTION:  lte_ml1_offload_task_init

===========================================================================*/
/*!
  @brief
  Initializes ml1 offload task

  @return
  True if the ml1 offload task and the sub-modules are initialized successfully
*/
/*=========================================================================*/
pthread_t lte_ml1_offload_task_init
( 
  int priority /*!< thread priority from app mgr */ 
);

/*===========================================================================

  FUNCTION:  lte_ml1_offload_static_task_main

===========================================================================*/
/*!
  @brief
  Main look for offload task

  @return
  None (should not return)
*/
/*=========================================================================*/

void* lte_ml1_offload_static_task_main
(
  void *arg             /*!< argument passed in during thread creation */  
);

/*===========================================================================

  FUNCTION:  lte_ml1_offload_static_task_deinit

===========================================================================*/
/*!
  @brief
  Deinitializes ml1 offload task 

  @return
  TRUE if the ml1 task is deinitialized successfully
*/
/*=========================================================================*/
void lte_ml1_offload_static_task_deinit
( 
 void 
);

/*===========================================================================

  FUNCTION:  lte_ml1_offload_handle_init_req

===========================================================================*/
/*!
  @brief
  Main look for offload task

  @return
  None (should not return)
*/
/*=========================================================================*/

void lte_ml1_offload_handle_init_req
(
  void
);

/*===========================================================================

  FUNCTION:  lte_ml1_offload_static_msgr_register

===========================================================================*/
/*!
  @brief
  Registers ml1 offload task with the message router

  @return
  TRUE if all registrations were successful, FALSE otherwise.
*/
/*=========================================================================*/
extern boolean lte_ml1_offload_static_msgr_add_and_register_sup_mb
(
  msgr_client_t	 *offload_msgr_client,
  msgr_id_t		 *sup_mb
);

/*===========================================================================

  FUNCTION:  lte_ml1_offload_static_deinit

===========================================================================*/
/*!
  @brief
  ML1 offload Task Tear down

  @return
  None
*/
/*=========================================================================*/
extern void lte_ml1_offload_static_deinit(boolean);

/*===========================================================================

  FUNCTION:  lte_ml1_offload_static_init

===========================================================================*/
/*!
  @brief
  ML1 offload init task.

  @return
  None
*/
/*=========================================================================*/
extern void lte_ml1_offload_static_init(void);

#endif /* LTE_ML1_OFFLOAD_STATIC_TASK_H */
