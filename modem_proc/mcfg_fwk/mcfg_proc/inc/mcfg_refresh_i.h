/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

	mcfg_refresh_i.h

GENERAL DESCRIPTION

  MCFG header file for auto refrsh
  
Copyright (c) 2014 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                      EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/mcfg_fwk.mpss/7.10.1.1/mcfg_proc/inc/mcfg_refresh_i.h#1 $
  $DateTime: 2020/02/19 23:07:19 $


  when        who     what, where, why
  ---------   ---     ---------------------------------------------------------------------------
 2014/04/11   cys     Create API file

===========================================================================*/
#ifndef _MCFG_REFRESH_I_H_
#define _MCFG_REFRESH_I_H_

#include "mcfg_common.h"
#include "mcfg_task.h"
#include "mcfg_refresh.h"
#include "mcfg_client.h"

#include "cm.h"
#include "cm_v.h"

/*==============================================================================

                PUBLIC DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/

/*==============================================================================
  Typedefs
==============================================================================*/
typedef enum mcfg_refresh_cmd_e {
  MCFG_REFRESH_CMD_REGISTER_EXT = 0,
  MCFG_REFRESH_CMD_DONE,
  MCFG_REFRESH_CMD_UIMDATA,
  MCFG_REFRESH_CMD_MMCP_DEACT,
  MCFG_REFRESH_CMD_MMCP_TEAR_DOWN_ACK,
  MCFG_REFRESH_CMD_CM_CALL_END,
  MCFG_REFRESH_CMD_CM_LPM,
  MCFG_REFRESH_CMD_TIMEOUT,
  MCFG_REFRESH_CMD_CM_PH_STATE,
  MCFG_REFRESH_CMD_DEREGISTER,
} mcfg_refresh_cmd_e_type;

typedef struct
{
  mcfg_refresh_index_type     sub_index;
  cm_ph_state_e_type          ph_state;
} mcfg_refresh_cm_ph_state_s_type;

typedef struct
{
  mcfg_refresh_index_type     sub_index;
  cm_ph_cmd_err_e_type        ph_cmd_err;
} mcfg_refresh_mmcp_cb_s_type;

typedef struct mcfg_refresh_cmd_data_s {
  mcfg_refresh_cmd_e_type   cmd_id;
  mcfg_client_event_e_type  event_reg_type;
  mcfg_client_id_type       client_id;

  union {
    mcfg_refresh_registration_s_type register_req;
    mcfg_refresh_event_reg_s_type    refresh_event_reg;

    mcfg_refresh_info_s_type     info;
    mcfg_refresh_uimdata_s_type  uimdata;
    mcfg_refresh_index_mask_type submask;
    mcfg_refresh_mmcp_cb_s_type  mmcpdata;
    mcfg_refresh_index_type      slot_index;
    mcfg_refresh_cm_ph_state_s_type cmphstate;
  } data;

  mcfg_refresh_status_e_type status;
} mcfg_refresh_cmd_data_s_type;

typedef enum mcfg_refresh_state_e {
  MCFG_REFRESH_STATE_MIN = 0,
  MCFG_REFRESH_STATE_IDLE = MCFG_REFRESH_STATE_MIN,
  MCFG_REFRESH_STATE_ACTIVATING_SLOT,
  MCFG_REFRESH_STATE_UPDATING_SLOT,
  MCFG_REFRESH_STATE_WAIT_FOR_SESSION,
  MCFG_REFRESH_STATE_ONHOLD,
  MCFG_REFRESH_STATE_WAIT_CM_INIT,
  MCFG_REFRESH_STATE_TEAR_DOWN_REQ,
  MCFG_REFRESH_STATE_TEAR_DOWN_ACK,
  MCFG_REFRESH_STATE_TEAR_DOWN_NACK,
  MCFG_REFRESH_STATE_ACTIVATING_SUBS,
  MCFG_REFRESH_STATE_UPDATING_SUBS,
  MCFG_REFRESH_STATE_INVALID,
  MCFG_REFRESH_STATE_MAX = MCFG_REFRESH_STATE_INVALID,
} mcfg_refresh_state_e_type;

typedef enum mcfg_refresh_orig_e {
  MCFG_REFRESH_ORIG_INVALID = 0,   
  MCFG_REFRESH_ORIG_UIM,      //Originated by uim hot swap
  MCFG_REFRESH_ORIG_PDC,      //Originated by MCFG PDC activation
  MCFG_REFRESH_ORIG_APP,      //Forced by APP centric refresh
  MCFG_REFRESH_ORIG_PDC_UPDATE_ONLY, //Originated by MCFG PDC activation for MBN update only
  MCFG_REFRESH_ORIG_NOCHANGE, //There's no configuration change
  MCFG_REFRESH_ORIG_FLEX,     //Originated by uim flex mapping
} mcfg_refresh_orig_e_type;


/*==============================================================================

                    PUBLIC FUNCTION DECLARATIONS FOR MODULE

==============================================================================*/

void mcfg_refresh_cmd_proc
(
  mcfg_task_cmd_s_type *cmd
);

void mcfg_refresh_init
(
  void
);

mcfg_refresh_state_e_type mcfg_refresh_get_state
(
  mcfg_refresh_index_type sub_index
);

boolean mcfg_refresh_set_state
(
  mcfg_refresh_index_type slot_index, 
  mcfg_refresh_state_e_type state
);

boolean mcfg_refresh_set_orig
(
  mcfg_refresh_index_type sub_index, 
  mcfg_refresh_orig_e_type orig
);

mcfg_refresh_orig_e_type mcfg_refresh_get_orig
(
  mcfg_refresh_index_type sub_index
);

mcfg_error_e_type mcfg_refresh_clients
(
  mcfg_refresh_type_e_type  type,
  mcfg_refresh_index_type   slot
);

mcfg_refresh_index_type mcfg_refresh_sub_to_slot_index
(
  mcfg_refresh_index_type sub_index
);

mcfg_refresh_index_type mcfg_refresh_slot_to_sub_index
(
  mcfg_refresh_index_type slot_index
);

mcfg_error_e_type mcfg_refresh_tear_down
(
  mcfg_refresh_index_type sub_index
);

boolean mcfg_refresh_enabled
(
  mcfg_refresh_index_type slot_index
);

boolean mcfg_refresh_is_ongoing
(
  mcfg_refresh_index_mask_type slot_mask
);

boolean mcfg_refresh_is_ongoing_by_type
(
  mcfg_refresh_index_mask_type slot_mask,
  mcfg_refresh_type_e_type refresh_type
);

void mcfg_refresh_cm_init
(
  void
);

mcfg_error_e_type mcfg_refresh_all_by_apps
(
  void
);

#endif /* _MCFG_REFRESH_I_H_ */