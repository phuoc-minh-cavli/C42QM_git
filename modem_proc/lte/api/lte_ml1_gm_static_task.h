/*!
  @file lte_ml1_gm_static_task.h

  @brief
  The source file implementing the ML1 Grant Manager startup as well as
  common data

*/

/*===========================================================================

  Copyright (c) 2008 - 2015 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //components/rel/lte.mpss/4.7.2/api/lte_ml1_gm_static_task.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------

===========================================================================*/

#ifndef LTE_ML1_GM_STATIC_TASK_H
#define LTE_ML1_GM_STATIC_TASK_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/


#include <comdef.h>
#include <msgr.h>
#include <msgr_types.h>
#include <msgr_lte.h>

#include <pthread.h>
#include <lte_app_msg.h>
#include "lte_static_as_msg.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/
/* ML1 GM stack size is 5.5k for now */
#ifdef TEST_FRAMEWORK
#error code not present
#else
#define LTE_ML1_GM_TASK_STACK_SIZE  2048
#endif

msgr_client_t lte_ml1_gm_static_get_client_ptr(void);

typedef union
{
 /* Supervisor Requests */
  msgr_spr_loopback_s          msgr_spr_loopback;
  msgr_spr_loopback_reply_s    msgr_spr_loopback_reply;
} gm_static_msg_u;

typedef struct 
{
  boolean                      task_is_initialized;    /*!< Flag set when the task gets initialized */
  pthread_t                    thread_id;              /*!< task thread id */
  pthread_attr_t               attr;                   /*!< task attr */
  msgr_client_t                gm_msgr_client;         /*!< MSGR client pointer */
  msgr_id_t                    sup_mb;                 /*!< Supervisory messages mailbox */
  gm_static_msg_u              static_rcv_msg;         /*!< Static message buffer */  
}lte_ml1_gm_task_context_s;

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/
/*===========================================================================

  FUNCTION:  lte_ml1_gm_static_dispatch

===========================================================================*/
/*!
  @brief
  ML1 Grant Manager Dispatcher

  @return
  None
*/
/*=========================================================================*/
void lte_ml1_gm_static_dispatch
(
  lte_ml1_gm_task_context_s *gm_task_ptr,
  uint8* msg_ptr,
  uint32 msg_size
);

/*===========================================================================

  FUNCTION:  lte_ml1_gm_create_mailboxes

===========================================================================*/
/*!
  @brief
  Creates the ml1 grant manager mailboxes

  @return
  True if all mailboxes are created successfully
*/
/*=========================================================================*/
STATIC boolean lte_ml1_gm_static_create_sup_mailbox
(
  lte_ml1_gm_task_context_s *gm_task_ptr
);

/*===========================================================================

  FUNCTION:  lte_ml1_gm_msgr_register

===========================================================================*/
/*!
  @brief
  Registers ml1 grant manager with the message router

  @return
  TRUE if all registrations were successful, FALSE otherwise.
*/
/*=========================================================================*/
boolean lte_ml1_gm_static_msgr_add_and_register_sup_mb
(
  msgr_client_t    *gm_msgr_client,
  msgr_id_t        *sup_mb 
);

/*===========================================================================

  FUNCTION:  lte_ml1_gm_task_main

===========================================================================*/
/*!
  @brief
  ML1 Grant Manager main

  @return
  NULL pointer
*/
/*=========================================================================*/
void* lte_ml1_gm_static_task_main
(
  void* arg       /*!< argument passed during thread creation, in ML1 case we
                       have passed NULL above */
);

/*===========================================================================

  FUNCTION:  lte_ml1_gm_static_task_init

===========================================================================*/
/*!
  @brief
  ML1 Grant Manager Initialization

  @return
  thread ID
*/
/*=========================================================================*/
pthread_t lte_ml1_gm_static_task_init
(
  int priority
);

/*===========================================================================

  FUNCTION:  lte_ml1_gm_task_deinit

===========================================================================*/
/*!
  @brief
  ML1 Grant Manager Task Tear down

  @return
  void
*/
/*=========================================================================*/
void lte_ml1_gm_static_deinit
( 
  boolean kill_task
);

/*===========================================================================

  FUNCTION:  lte_ml1_gm_static_task_get_thread_id

===========================================================================*/
/*!
  @brief
  The API provides the thread id of GM task, if the task is intialized

  @return
  TRUE - if GM task_is_initialized
  FALSE - otherwise
*/
/*=========================================================================*/
boolean lte_ml1_gm_static_task_get_thread_id
(
  /*!< pointer to task thread id */
  pthread_t *thread_id
);

/* Supervisory UMIDs */
typedef enum
{
  MSGR_DEFINE_UMID(LTE, ML1_GM, SPR, LOOPBACK, MSGR_ID_LOOPBACK, msgr_spr_loopback_s),
  MSGR_DEFINE_UMID(LTE, ML1_GM, SPR, LOOPBACK_REPLY, MSGR_ID_LOOPBACK_REPLY, msgr_spr_loopback_reply_s),
  MSGR_DEFINE_UMID(LTE, ML1_GM, SPR, THREAD_READY, LTE_APP_ID_THREAD_READY, none),
  MSGR_DEFINE_UMID(LTE, ML1_GM, SPR, THREAD_KILL, LTE_APP_ID_THREAD_KILL, none)
}lte_ml1_gm_sup_e;

#endif /* __LTE_ML1_GM_MAIN_H__ */


