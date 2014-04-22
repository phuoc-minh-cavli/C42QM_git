/*!
  @file lte_ml1_static_task.h

  @brief
  Externalizes the ML1 thread init/de-init routines.

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

$Header: //components/rel/lte.mpss/4.7.2/api/lte_static_ml1_mgr_task.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------

===========================================================================*/

#ifndef LTE_ML1_STATIC_TASK_H
#define LTE_ML1_STATIC_TASK_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include <msgr.h>
#include <msgr_types.h>
#include <msgr_lte.h>
#include <lte_static_as_msg.h>
#include <pthread.h>
#include <lte_app_msg.h>
#include <lte_static_ml1_ext_api.h>
#include "lte_static_ml1_common.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/
/* ML1 Manager stack size is 8k for now */
#ifdef TEST_FRAMEWORK
#error code not present
#else
#define LTE_ML1_MGR_TASK_STACK_SIZE  2048
#endif

/*! @brief Union of all external msgs that ML1 MGR can receive 
     in Static code before NB DLL is loaded
*/
typedef union
{
 /* Supervisor Requests */
  msgr_spr_loopback_s          msgr_spr_loopback;
  msgr_spr_loopback_reply_s    msgr_spr_loopback_reply;
} lte_static_ml1_mgr_msg_u;

typedef void* (*mgr_task_init_msg_fptr) (void);

typedef struct
{
  boolean                  task_is_initialized; /*!< Flag set when the task gets initialized */
  pthread_t                thread_id;           /*!< task thread id */
  pthread_attr_t           attr;                /*!< task attr */
  mgr_task_init_msg_fptr   mgr_init_msg_fptr;    /*!< Function ptr of INIT Handle msg */
}lte_ml1_mgr_task_s;

typedef struct 
{
  /* The client for ML1 MGR, the value is set by MSGR
     and returned as a result of msgr_client_create */
  msgr_client_t               lte_static_ml1_mgr_msgr_client;
  
  lte_ml1_mgr_task_s          lte_ml1_mgr_task;
  
  /*! Supervisory messages mailbox */
  msgr_id_t sup_mb;

  lte_static_ml1_mgr_msg_u    static_rcv_msg;

  /* Modules with pending cnf*/
  lte_ml1_task_mask_t         mod_pend_mask;

  /* Track Init/Deinit proc */
  boolean                     init_deinit_in_progress;

 /* Track system deinit proc */
  boolean                     sys_deinit_done;
}lte_static_ml1_mgr_client_data_s;


typedef struct 
{
  //thread id
  pthread_t                             thread_id;

  //task data
  lte_static_ml1_mgr_client_data_s     *client_ptr;
}lte_ml1_manager_static_task_data_s;


/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/
/*===========================================================================

  FUNCTION:  lte_ml1_mgr_task_init

===========================================================================*/
pthread_t lte_ml1_mgr_task_init
(
  int priority  /*!< thread priority passed in by app mgr */
);
/*===========================================================================

  FUNCTION:  lte_static_ml1_mgr_task_main

===========================================================================*/
void* lte_static_ml1_mgr_task_main
(
  void *arg             /*!< argument passed in during thread creation */
);
/*===========================================================================

  FUNCTION:  lte_static_ml1_mgr_deinit

===========================================================================*/
void lte_static_ml1_mgr_deinit
(
  /*! Pointer to right instance */
  lte_static_ml1_mgr_client_data_s  *mgr_client_ptr
);
/*===========================================================================

  FUNCTION:  lte_ml1_task_is_initialized

===========================================================================*/
boolean lte_ml1_task_is_initialized
(
  void
);
/*===========================================================================

  FUNCTION:  lte_static_ml1_mgr_handle_cphy_init_req

===========================================================================*/
void lte_static_ml1_mgr_handle_cphy_init_req
(
  /*! Pointer to right instance */
  lte_static_ml1_mgr_client_data_s *mgr_client_ptr,
  uint8* msg_ptr,
  uint32 msg_size,
  boolean is_tech_initialized
);
/*===========================================================================

  FUNCTION:  lte_static_ml1_mgr_handle_load_unload_cnf

===========================================================================*/
boolean lte_static_ml1_mgr_handle_load_unload_cnf
(
  lte_static_ml1_mgr_client_data_s *mgr_client_ptr,
  boolean load_cnf,
  uint8* msg_ptr,
  uint32 msg_size
);

/*===========================================================================

  FUNCTION:  lte_ml1_mgr_get_task_info

===========================================================================*/
/*!
  @brief
  Retrun ML1 mgr task info ptr

  @return
  ML1 mgr task info ptr
*/
/*=========================================================================*/
lte_static_ml1_mgr_client_data_s* lte_ml1_mgr_get_task_info( void );

/*===========================================================================

  FUNCTION:  lte_static_ml1_mgr_trigger_ce_reporting

===========================================================================*/
/*!
    @brief
    Handles LTE_CPHY_CE_INFO_REPORT_REQ

    @return
    None.
*/
/*=========================================================================*/
void lte_static_ml1_mgr_trigger_ce_reporting
(
  lte_cphy_ce_info_rpt_req_s *lte_cphy_ce_info_rpt_req,
  uint32                     dmm_intf_id
 );

/*===========================================================================

  FUNCTION:  lte_static_ml1_mgr_handle_cphy_ce_rpt_req

===========================================================================*/
/*!
    @brief
    Handles LTE_CPHY_CE_INFO_REPORT_REQ

    @return
    None.
*/
/*=========================================================================*/
void lte_static_ml1_mgr_handle_cphy_ce_rpt_req
(
  uint8  *msg_ptr,
  uint32 msg_size
 );

/*===========================================================================

  FUNCTION:  lte_ml1_mgr_is_init_deinit_in_progress

===========================================================================*/
/*!
  @brief
  Return if init or deinit is in progress

  @return
  boolean
*/
/*=========================================================================*/
boolean lte_ml1_mgr_is_init_deinit_in_progress( void );

/*===========================================================================

  FUNCTION:  lte_ml1_mgr_set_init_deinit_in_progress

===========================================================================*/
/*!
  @brief
  Set init deinit is in progress

  @return
  boolean
*/
/*=========================================================================*/
void lte_ml1_mgr_set_init_deinit_in_progress
(
  boolean in_progress
);

/*===========================================================================

  FUNCTION:  lte_ml1_mgr_is_sys_deinit_done

===========================================================================*/
/*!
  @brief
  Return True, if sys deinit is done, and none of the system(LTE/NB1) is active

  @return
  boolean
*/
/*=========================================================================*/
boolean lte_ml1_mgr_is_sys_deinit_done( void );

/*===========================================================================

  FUNCTION:  lte_ml1_mgr_set_sys_deinit_done

===========================================================================*/
/*!
  @brief
  Set sys deinit is done, and none of the system(LTE/NB1) is active

  @return
  None
*/
/*=========================================================================*/
void lte_ml1_mgr_set_sys_deinit_done
(
  boolean deinit_done
);

/*===========================================================================

  FUNCTION: lte_static_ml1_mgr_handle_get_next_sfn_req

===========================================================================*/
/*!
    @brief
    Handles LTE_CPHY_GET_NEXT_SFN_REQ

    @return
    None.
*/
/*=========================================================================*/
void lte_static_ml1_mgr_handle_get_next_sfn_req
(
  uint8* msg_ptr,
  uint32 msg_size
);

/*===========================================================================

  FUNCTION: lte_static_ml1_mgr_handle_set_sfn_config_req

===========================================================================*/
/*!
    @brief
    Handles LTE_CPHY_SET_SFN_CONFIG_REQ

    @return
    None.
*/
/*=========================================================================*/
void lte_static_ml1_mgr_handle_set_sfn_config_req
(
  uint8* msg_ptr,
  uint32 msg_size
 );

/*===========================================================================

  FUNCTION:  lte_static_ml1_mgr_handle_cphy_ce_level_rpt_req

===========================================================================*/
/*!
    @brief
    Handles LTE_CPHY_CE_LEVEL_REPORT_REQ

    @return
    None.
*/
/*=========================================================================*/
void lte_static_ml1_mgr_handle_cphy_ce_level_rpt_req
(
  uint8* msg_ptr,
  uint32 msg_size
);

/* Supervisory UMIDs */
enum{
  MSGR_DEFINE_UMID(LTE, ML1, SPR, LOOPBACK,   MSGR_ID_LOOPBACK,     msgr_spr_loopback_s),
  MSGR_DEFINE_UMID(LTE, ML1, SPR, LOOPBACK_REPLY, MSGR_ID_LOOPBACK_REPLY,   msgr_spr_loopback_reply_s),
  MSGR_DEFINE_UMID(LTE, ML1, SPR, THREAD_READY, LTE_APP_ID_THREAD_READY,  none),
  MSGR_DEFINE_UMID(LTE, ML1, SPR, THREAD_KILL,  LTE_APP_ID_THREAD_KILL,   none),
};
#endif
