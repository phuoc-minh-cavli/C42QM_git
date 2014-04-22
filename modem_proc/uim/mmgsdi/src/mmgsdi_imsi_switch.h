#ifndef MMGSDI_IMSI_SWITCH_H
#define MMGSDI_IMSI_SWITCH_H
/*===========================================================================


           M M G S D I   I M S I   S W I T C H   H E A D E R S


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2023  QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.  
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/dev/uim.mpss/3.3.1/tqidwai.uim.mpss.3.3.1.3454742_3_3_1/mmgsdi/src/mmgsdi_refresh.h#1 $$ $DateTime: 2023/04/04 00:54:00 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
05/05/23   tq      Initial version

=============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/


#include "mmgsdilib.h"
#include "cm.h"
#include "mmgsdi.h"
#include "sys.h"
#include "mmgsdilib_common.h"

/*=============================================================================

                       DATA DECLARATIONS

=============================================================================*/


/*=============================================================================

                       STRUCTURE DECLARATIONS

=============================================================================*/


extern boolean                                    mmgsdi_imsi_switch_timer_running; 
extern mmgsdi_imsi_switch_nw_enum_type            mmgsdi_imsi_switch_crnt_nw_type;
extern mmgsdi_imsi_switch_profile_id_enum_type    mmgsdi_imsi_switch_crnt_active_profile;
extern uint32                                     mmgsdi_imsi_switch_timer_tick; 
extern uint32                                     mmgsdi_imsi_switch_ping_pong_tick;
extern uint8                                      mmgsdi_switch_imsi_enable;
extern sys_srv_status_e_type                      mmgsdi_imsi_switch_srv_status;

/*=============================================================================

                                   MMGSDI MACROS
                        MACRO Definitions used through MMGSDI
=============================================================================*/
#define MMGSDI_IMSI_SWITCH_TIMER_VAL                60000 * 2
#define MMGSDI_IMSI_SWITCH_PING_PONG_TIMER_VAL      60000 * 20
#define MMGSDI_IMSI_SWITCH_MAX_PING_PONG_CNT        6

#define MMGSDIIMSI_SWITCH_SRV_STATUS_CHANGED(x)                                      \
          ((x == SYS_SRV_STATUS_SRV) || (x == SYS_SRV_STATUS_LIMITED) ||  \
           (x == SYS_SRV_STATUS_LIMITED_REGIONAL) || (x == SYS_SRV_STATUS_NO_SRV))

/*=============================================================================

                          FUNCTIONS

=============================================================================*/

/*===========================================================================
  FUNCTION MMGSDI_IMSI_SWITCH_CLEAR_TIMER

  DESCRIPTION
    This function clear the timer

  DEPENDENCIES
    None

  LIMITATIONS
    NONE

  RETURN VALUE
    None

  SIDE EFFECTS
    None
===========================================================================*/
void mmgsdi_imsi_switch_timer_clear(void);

/*================================================================
FUNCTION  MMGSDI_SWITCH_IMSI_PROFILE

DESCRIPTION:
  This function is called to to switch the imsi profile .

  INPUT PARAMETERS:
    client_id:                       Client ID of the caller.
    slot:                            Specific slot
    sys_srv_status_e_type:           srv, 
    sys_plmn_id_s_type:              plmn,
    mmgsdi_imsi_switch_nw_enum_type: switch_network,
    response_cb_ptr:                 Pointer to the response callback.
    src:                             Source of API trigger
    client_ref:                      User data returned upon completion of this command.

DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN VALUE:
  mmgsdi_return_enum_type:

  MMGSDI_SUCCESS          -- Command structure was properly generated and sent
                             to the MMGSDI command queue.\n
  MMGSDI_INCORRECT_PARAMS -- Parameters are not within the appropriate ranges.
  MMGSDI_CMD_QUEUE_FULL   -- Command was not queued to the MMGSDI task because 
                             the maximum number of commands are already queued.

SIDE EFFECTS:
  None
===============================================================*/
mmgsdi_return_enum_type mmgsdi_switch_imsi_profile(
  mmgsdi_client_id_type            client_id,
  mmgsdi_slot_id_enum_type         slot,
  sys_srv_status_e_type            srv,
  sys_plmn_id_s_type               plmn,
  mmgsdi_imsi_switch_nw_enum_type  switch_network,
  mmgsdi_callback_type             response_cb_ptr,
  mmgsdi_imsi_switch_src_enum_type src,
  mmgsdi_client_data_type          client_ref
);


/*===========================================================================
FUNCTION MMGSDI_SWITCH_IMSI_GET_CURRENT_PROFILE

DESCRIPTION
  This function is called to get the active IMSI profile 

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_return_enum_type
  MMGSDI_SUCCESS          - Command processed,
  MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
  MMGSDI_ERROR            - Other unknown failure

SIDE EFFECTS
  NONE
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_switch_imsi_get_current_profile
(
  mmgsdi_imsi_switch_profile_id_enum_type *profile_id_ptr,
  mmgsdi_imsi_switch_nw_enum_type         *nw_type_ptr
);
/*===========================================================================
  FUNCTION: MMGSDI_IMSI_SWITCH_CM_SS_EVT_CB

  DESCRIPTION:
    Handler for CM_SS_EVENT_SRV_CHANGED. It decides whether it is required to
    SWITCH the IMSI profile 

  PARAMETERS:
    ss_event         -> cm ss event
    mm_ss_info_ptr   -> more information regarding the ss event
===========================================================================*/
void mmgsdi_imsi_switch_cm_ss_evt_cb(
  cm_ss_event_e_type                ss_event,
  const cm_mm_ss_info_s_type      * mm_ss_info_ptr
);

/*===========================================================================
  FUNCTION MMGSDI_IMSI_SWITCH_TIMER_EXPIRY

  DESCRIPTION
    This function got called when srv timer got expired 

  DEPENDENCIES
    None

  LIMITATIONS
    NONE

  RETURN VALUE
    None

  SIDE EFFECTS
    None
===========================================================================*/
void mmgsdi_imsi_switch_timer_expiry (uint32 dummy);

/*===========================================================================
  FUNCTION MMGSDI_IMSI_SWITCH_TIMER
  
  DESCRIPTION
    This function starts the timer based on the service

  DEPENDENCIES
    None

  LIMITATIONS
    NONE

  RETURN VALUE
    None

  SIDE EFFECTS
    Builds an Error Response to MMGSDI Client
===========================================================================*/
 void mmgsdi_imsi_switch_timer_start(void);

#endif /* MMGSDI_IMSI_SWITCH_H */
