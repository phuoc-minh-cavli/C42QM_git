/*!
  @file
  fws.h

  @brief
  Modem FW application interface.

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

$Header: //components/rel/mcs.mpss/5.1/api/fws.h#2 $

===========================================================================*/

#ifndef FWS_H
#define FWS_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <comdef.h>
#include <pthread.h>
#include "fws_types.h"
#include "msgr_types.h"


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/* Allow external access to FW error info in shared memory */
extern volatile fws_smem_intf_t *fws_smem_intf;

/* IPC callback types */
typedef enum
{
  FWS_IPC_CALLBACK__NULL,
  FWS_IPC_CALLBACK__THRU_TASK,
  FWS_IPC_CALLBACK__DIRECT_CALL
}fws_ipc_callback_type;

/* Function pointer type for IPC callbacks */
typedef void (*FWS_IPC_FPTR)(void);

/* Legacy SMEM interface */
#define fws_get_smem_region(phys_addr,size)  ((volatile void *) (phys_addr))


/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

extern pthread_t fws_task_init
(
  int priority  
);


extern volatile void * fws_smem_get_interface
(
  uint32 intf_id
);

extern void fws_smem_register_interface
(
  uint32 intf_id,
  volatile void *virt_addr,
  uint64 phys_addr,
  uint32 size
);


extern uint32 fws_app_enable
(
  uint32 enable_mask  
);

extern uint32 fws_get_app_enabled
(
  void
);

extern uint32 fws_app_disable
(
  uint32 disable_mask  
);

extern boolean fws_diag_submit_log
(
  msgr_hdr_struct_type *  msgr_hdr,
  uint32                  length
);

extern uint32 fws_app_config
(
  uint32 disable_mask, 
  uint32 enable_mask  
);


extern uint32 fws_suspend
(
  uint32 app_mask
);

extern void fws_resume
(
  uint32 app_mask
);

extern uint32 fws_sleep_request
(
  fw_sleep_mode_e mode
);

extern void fws_sleep_finalize
(
  void
);

extern void fws_get_sleep_cfg
(
  fws_sleep_cfg_t *sleep_cfg 
);

extern void fws_wakeup_start
(
  void
);

extern void fws_wakeup_finalize
(
  void
);

extern void fws_ipc_init
(
  uint32                id,
  fws_ipc_callback_type type,
  FWS_IPC_FPTR          callback
);

extern void fws_ipc_deinit
(
  uint32 id
);

extern void fws_ipc_send
(
  uint32 id
);

extern void fws_ipc_task
(
  uint32 id
);

extern void fws_config_set
(
  uint32 config_ofs, 
  uint8 value     
);

extern uint8 fws_config_get
(
  uint32 config_ofs  
);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif /* FWS_H */
