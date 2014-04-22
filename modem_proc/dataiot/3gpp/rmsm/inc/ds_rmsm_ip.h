#ifndef DS_RMSM_IP_H
#define DS_RMSM_IP_H
/*===========================================================================

                      D S U M T S _ R M S M _ I P  . H

DESCRIPTION
  The Data Services RM State Machine(PDP-IP) specific header file. 
  This contains the state machine specific function definitions and 
  data definitions 
  
EXTERNALIZED FUNCTIONS
Copyright (c) 2002 - 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                            EDIT HISTORY FOR FILE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/dataiot.mpss/2.0/3gpp/rmsm/inc/ds_rmsm_ip.h#2 $ 
$DateTime: 2020/04/27 07:41:34 $ $Author: pwbldsvc $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/08/12    vs      Added support for 3GPP IPv6 over PPP
03/04/09    sa      AU level CMI modifications.
04/22/03    vsk     code review cleanup. updated function headers
04/18/03    vsk     Created module
===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "dstaski.h"
#include "ps_svc.h"
#include "dsat_v.h"
#include "ds_rmsmi.h"

#if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS)|| defined (FEATURE_DATA_LTE)
/*===========================================================================

                         REGIONAL DATA DECLARATIONS

===========================================================================*/
  
   

/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

                        PUBLIC FUNCTION DECLARATIONS

= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

/*===========================================================================
  FUNCTION DS_RMSM_IP_SM_INIT 
    This function initializes the PDP-IP specific state machine information
    pointed by sm_info

  DESCRIPTION
    
  PARAMETERS
   sm_ptr: Pointer to the RmSm IP State Machine

  RETURN VALUE
   TRUE,  if success
   FALSE, if failure

  DEPENDENCIES
    none  
    
  SIDE EFFECTS
    None
===========================================================================*/  
boolean ds_rmsm_ip_init
(
  ds_rmsmi_sm_info_type *sm_ptr
);

/*===========================================================================
  FUNCTION DS_RMSM_IP_PROCESS_CMD()

  DESCRIPTION
    This function receives a cmd posted to DS_RMSM (for pdp-ip calls)
    in DS task and calls an appropriate function to handle it.

  PARAMETERS
    inst_ptr: Pointer to the Rm Dev Instance and SM Instance
    cmd id to be processed
    
  RETURN VALUE
    none

  DEPENDENCIES
    none  

  SIDE EFFECTS
    None
===========================================================================*/
void ds_rmsm_ip_process_cmd
(
  ds_rmsm_instance_type* inst_ptr,
  ds_command_enum_type cmd_id
);


/*===========================================================================
  FUNCTION DS_RMSM_IP_PROCESS_RM_PPP_UP_EV()

  DESCRIPTION
    This function processes RM_PPP_UP_EV event. 
    
  PARAMETERS
    sm_ptr: Pointer to the RmSm IP State Machine

  RETURN VALUE
    None

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
void ds_rmsm_ip_process_rm_ppp_up_ev
(
  ds_rmsmi_sm_info_type *sm_ptr
);

/*===========================================================================
  FUNCTION DS_RMSM_IP_PROCESS_RM_PPP_DOWN_EV()

  DESCRIPTION
    This function processes RM_PPP_DOWN_EV event. 

  PARAMETERS
    sm_ptr: Pointer to the RmSm IP State Machine

  RETURN VALUE
    None

  DEPENDENCIES
    none  
    
  SIDE EFFECTS
    None
===========================================================================*/
void ds_rmsm_ip_process_rm_ppp_down_ev
( 
  ds_rmsmi_sm_info_type *sm_ptr
);

/*===========================================================================
  FUNCTION DS_RMSM_IP_PROCESS_ADDR_CFG_FAILURE_EV()

  DESCRIPTION
    This function processes ADDR_CFG_FAILURE_EV event. 
    
  PARAMETERS
    sm_ptr: Pointer to the RmSm IP State Machine

  RETURN VALUE
    None

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
void ds_rmsm_ip_process_addr_cfg_failure_ev
(
  ds_rmsmi_sm_info_type *sm_ptr
);

/*===========================================================================
  FUNCTION DS_RMSM_IP_PROCESS_ADDR_CFG_CHANGED_EV()

  DESCRIPTION
    This function processes ADDR_CFG_CHANGED_EV event. 
    
  PARAMETERS
    sm_ptr: Pointer to the RmSm IP State Machine

  RETURN VALUE
    None

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
void ds_rmsm_ip_process_addr_cfg_changed_ev
(
  ds_rmsmi_sm_info_type *sm_ptr
);

/*===========================================================================
  FUNCTION DS_RMSM_IP_PROCESS_ADDR_CFG_COMPLETE_EV()

  DESCRIPTION
    This function processes ADDR_CFG_COMPLETE_EV event. 
    
  PARAMETERS
    sm_ptr: Pointer to the RmSm IP State Machine

  RETURN VALUE
    None

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
void ds_rmsm_ip_process_addr_cfg_complete_ev
(
  ds_rmsmi_sm_info_type *sm_ptr
);

/*===========================================================================
  FUNCTION DS_RMSM_IP_PROCESS_ADDR_CFG_END_EV()

  DESCRIPTION
    This function processes ADDR_CFG_END_EV event. 
    
  PARAMETERS
    sm_ptr: Pointer to the RmSm IP State Machine

  RETURN VALUE
    None

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
void ds_rmsm_ip_process_addr_cfg_end_ev
(
  ds_rmsmi_sm_info_type *sm_ptr
);


/*===========================================================================
  FUNCTION DS_RMSM_IP_PROCESS_BRING_UP_UM_IFACE_EV()

  DESCRIPTION
    This function processes BRING_UP_UM_IFACE_EV event. 
    
  PARAMETERS
    sm_ptr: Pointer to the RmSm IP State Machine

  RETURN VALUE
    None

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
void ds_rmsm_ip_process_bring_up_pdn_ev
(
  ds_rmsmi_sm_info_type *sm_ptr
);

/*===========================================================================
  FUNCTION DS_RMSM_IP_PROCESS_BRING_UP_UM_IFACE_EV()

  DESCRIPTION
    This function processes BRING_UP_UM_IFACE_EV event. 
    
  PARAMETERS
    sm_ptr: Pointer to the RmSm IP State Machine

  RETURN VALUE
    None

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/

ds_rmsm_ipi_state_type ds_rmsm_ip_get_state
(
  ds_rmsmi_sm_info_type *sm_ptr
);


/*===========================================================================
  FUNCTION DS_RMSM_IP_PROCESS_BRING_UP_UM_IFACE_EV()

  DESCRIPTION
    This function processes BRING_UP_UM_IFACE_EV event. 
    
  PARAMETERS
    sm_ptr: Pointer to the RmSm IP State Machine

  RETURN VALUE
    None

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
int ds_rmsm_set_addr_family
(
  ds_rmsm_sio_instance* sio_inst,
  ip_addr_enum_type ip_addr_family 
);

/*===========================================================================
   FUNCTION DS_RMSM_IP_PROCESS_PDN_UP_EV()
 
   DESCRIPTION
     This function processes UM_PDN_DOWN_EV event. 
     
   PARAMETERS
     sm_ptr: Pointer to the RmSm IP State Machine
 
   RETURN VALUE
     None
 
   DEPENDENCIES
     none
 
   SIDE EFFECTS
     None
===========================================================================*/
void ds_rmsm_ip_process_pdn_up_ev
(
  ds_rmsmi_sm_info_type *sm_ptr
);

/*===========================================================================
   FUNCTION DS_RMSM_IP_PROCESS_PDN_DOWN_EV()
 
   DESCRIPTION
     This function processes UM_PDN_DOWN_EV event. 
     
   PARAMETERS
     sm_ptr: Pointer to the RmSm IP State Machine
 
   RETURN VALUE
     None
 
   DEPENDENCIES
     none
 
   SIDE EFFECTS
     None
===========================================================================*/
void ds_rmsm_ip_process_pdn_down_ev
(
  ds_rmsmi_sm_info_type *sm_ptr
);

/*===========================================================================
 FUNCTION DS_3GPP_RMSM_IP_COMMON_INIT

 DESCRIPTION
   This function initializes the RmSm IP Common Data Structure
   
 PARAMETERS
   Rm Device Instance
   
 RETURN VALUE
   TRUE on success
   FALSE on failure

 DEPENDENCIES
   Should only be called once at powerup

 SIDE EFFECTS
   None
===========================================================================*/
boolean ds_3gpp_rmsm_ip_common_init
(
 ds_rmsm_rm_dev_instance_type rm_dev_instance
);

/*===========================================================================
 FUNCTION DS_3GPP_RMSM_IP_COMMON_POST_EVENT()

 DESCRIPTION
   This function receives an event posted to PDP-IP type RMSM and enqueues the
   corresponding command to be processed in the DS task.

 PARAMETERS
 inst_ptr: Pointer to the Rm Dev Instance and SM Instance
   ds_command_enum
 RETURN VALUE
   None

 DEPENDENCIES
   ds_rmsm_ip_sm_init() should have been called first.
   The top level state machine post function checks if we the state machine
   is initialized in the first place

 SIDE EFFECTS
   None
===========================================================================*/
void ds_3gpp_rmsm_ip_common_post_event
(
 ds_rmsm_instance_type* inst_ptr,
 ds_command_enum_type       cmd_id
);

/*===========================================================================
 FUNCTION DS_3GPP_RMSM_IP_COMMON_PROCESS_CMD()

 DESCRIPTION
   This function receives a cmd posted to DS_3GPP_RMSM (for pdp-ip calls)
   in DS task and calls an appropriate function to handle it

 PARAMETERS
   inst_ptr: Pointer to the Rm Dev Instance and SM Instance
   cmd id to be processed

 RETURN VALUE
   none

 DEPENDENCIES
   none  
   
 SIDE EFFECTS
   None
===========================================================================*/
void ds_3gpp_rmsm_ip_common_process_cmd
(
 ds_command_type *ds_cmd_ptr
);

/*===========================================================================
 FUNCTION DS_3GPP_RMSM_IP_COMMON_GET_STATE()

 DESCRIPTION
   This function retrieves the RMSM IP common state.

 PARAMETERS
   inst: Rm Dev Instance

 RETURN VALUE
   ds_3gpp_rmsm_ip_common_state_type

 DEPENDENCIES
   none  
   
 SIDE EFFECTS
   None
===========================================================================*/
ds_3gpp_rmsm_ip_common_state_type ds_3gpp_rmsm_ip_common_get_state
(
 ds_rmsm_rm_dev_instance_type  inst
);

/*===========================================================================
FUNCTION   DS_3GPP_RMSM_IP_COMMON_PPP_UP_DELAY_TIMER_CB

DESCRIPTION
  
  This function will be called when the PPP_UP_DELAY_TIMER expires

DEPENDENCIES
  none

RETURN VALUE
  none
  
SIDE EFFECTS
  
===========================================================================*/
void ds_3gpp_rmsm_ip_common_ppp_up_delay_timer_cb
( 
 void*  callback_data
);

/*===========================================================================
FUNCTION   DS_3GPP_RMSM_IP_COMMON_CONFIG_WAIT_TIMER_CB

DESCRIPTION
  
  This function will be called when the PPP_UP_DELAY_TIMER expires

DEPENDENCIES
  none

RETURN VALUE
  none
  
SIDE EFFECTS
  
===========================================================================*/
void ds_3gpp_rmsm_ip_common_config_wait_timer_cb
(
  void* callback_data
);

/*===========================================================================
FUNCTION   DS_3GPP_RMSM_IP_COMMON_PROCESS_UM_PDN_UP_EV

DESCRIPTION

DEPENDENCIES
  none

RETURN VALUE
  none
  
SIDE EFFECTS
  
===========================================================================*/

LOCAL void ds_3gpp_rmsm_ip_common_process_um_pdn_up_ev
(
ds_rmsm_instance_type* inst_ptr
);


/*===========================================================================
FUNCTION   DS_3GPP_RMSM_IP_COMMON_PROCESS_UM_PDN_DOWN_EV

DESCRIPTION

DEPENDENCIES
  none

RETURN VALUE
  none
  
SIDE EFFECTS
  
===========================================================================*/

void ds_3gpp_rmsm_ip_common_process_um_pdn_down_ev
(
 ds_rmsm_instance_type* inst_ptr
);

/*===========================================================================
FUNCTION   DS_3GPP_RMSM_IP_BRINGUP_COMPANION_INST
DESCRIPTION

DEPENDENCIES
  none

RETURN VALUE
  none
  
SIDE EFFECTS
  
===========================================================================*/

int ds_3gpp_rmsm_ip_bringup_companion_inst
(
  ds_rmsm_instance_type* inst_ptr
);


#endif /*defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS)
         || defined (FEATURE_DATA_LTE) */
#endif /* DS_RMSM_IP_H */
