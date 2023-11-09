#ifndef DS_3GPP_RMSM_ATI_H
#define DS_3GPP_RMSM_ATI_H
/*===========================================================================

                          D S _ 3 G P P _ R M S M _A T I  H

DESCRIPTION
  This file contains the definitions for RM state machine for ATCoP 
  interactions for PS related commands.
  
  
EXTERNALIZED FUNCTIONS

Copyright (c) 2018 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/


/*===========================================================================

                            EDIT HISTORY FOR FILE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/dataiot.mpss/2.0/3gpp/rmsm/inc/ds_3gpp_rmsm_ati.h#2 $ 
  $DateTime: 2023/08/03 09:33:47 $ $Author: pwbldsvc $

when        who    what, where, why
--------    ---   ----------------------------------------------------------
10/12/18    aks    First revision checkin 
===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "ds_3gpp_rmsm_atif.h"
#include "ds_pdn_hdlr.h"
#include "ds_sys.h"
#include "ds_pdn_manager.h"

/*===========================================================================

                         REGIONAL DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
    The number of instances of the state machine. The total instances
    that can be supported is equal to the MAX calls.
---------------------------------------------------------------------------*/

#define DS_3GPP_RMSM_AT_UM_MAX_INSTANCES  DS_3GPP_MAX_PDN_CONTEXT
typedef enum
{
  DS_RMSM_ATI_MIN_INST = 0,
  DS_RMSM_ATI_V4_INST  = DS_RMSM_ATI_MIN_INST,
  DS_RMSM_ATI_V6_INST,
  DS_RMSM_ATI_NONIP_INST,
  DS_RMSM_ATI_MAX_INST
}ds_rmsm_ati_inst_type_e;

typedef struct
{
  ds_pdn_mgr_instance_state_e     state;
  boolean                         in_use;
  ds_fwk_s_type                   *fw_inst;
} ds_at_pdn_instance;

typedef struct
{
  uint8          profile_num;
  uint8          rai_ind;
  boolean        is_mox_data_pending;
  uint32         pkt_data_len;
  dsm_item_type  **pkt_ptr;
} ds_mo_ex_user_info_s;

typedef struct 
{
  uint16                                profile_mask; 
  ds_3gpp_atif_pdp_connect_state_e_type connect_state;           
  uint32                                pending_data;/* pending data for transfer */
  boolean                               um_flow_enabled;
  ds_at_pdn_instance*                   um_inst_ptr[DS_RMSM_ATI_MAX_INST];          
  void*                                 user_info_ptr;        /*user data     */
  ds_mo_ex_user_info_s*                 mo_ex_data_info;
  /*Keeping client handles outside pdn instances as they are static and are 
    valid even in the absence of calls */
  ds_fwk_clnt_handle_type               v4_clt_hdl;
  ds_fwk_clnt_handle_type               v6_clt_hdl;
  ds_fwk_clnt_handle_type               nonip_clt_hdl;
} ds_3gpp_ati_pdp_state_type_dyn_s;


typedef struct 
{
  boolean                            in_use;/* flag to denote whether it is in use */
  ds_3gpp_ati_pdp_state_type_dyn_s*  ds_3gpp_ati_pdp_state_type_dyn_p;
} ds_3gpp_ati_pdp_state_type;

typedef struct 
{
  boolean                    data_rx_enabled;
  /* Flag to indicate if AT is interested for Rx non-IP data */
  downlink_data_cback_type   data_rx_cb_func;
  /* cb function to send RX non-IP data to AT */
} ds_3gpp_ati_rx_data_info_type;

/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

                        FUNCTION DECLARATIONS
  These are to be used by DS_3GPP_RMSM_AT internally and should not be used by 
  the clients of DS_3GPP_RMSM_AT

= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_GET_V4_INST_PTR
  
  DESCRIPTION   
    Given the call instance, returns the v4 instance ptr.
  
  DEPENDENCIES  
    None.
  
  RETURN VALUE  
    ds_at_pdn_instance
    
  SIDE EFFECTS  
    None.
  ===========================================================================*/
ds_at_pdn_instance* ds_3gpp_rmsm_get_v4_inst_ptr
(
  ds_3gpp_rmsm_at_instance_type instance
);

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_GET_V6_IFACE_PTR
  
  DESCRIPTION   
    Given the call instance, returns the v6 instance ptr.
  
  DEPENDENCIES  
    None.
  
  RETURN VALUE  
    ds_at_pdn_instance.
    
  SIDE EFFECTS  
    None.
  ===========================================================================*/
ds_at_pdn_instance* ds_3gpp_rmsm_get_v6_inst_ptr
(
  ds_3gpp_rmsm_at_instance_type instance
);

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_ATI_CGACT_ACTIVATION

  DESCRIPTION
    This function activates the Profile, if not already activated .
    If the profile is already activated return SUCCESS immediately.

  RETURN VALUE
    DSAT_OK -- Profile is already activated
    DSAT_ERROR -- In case of any FAILURE
    DSAT_ASYNC_CMD -- Profile is being activated.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
dsat_result_enum_type  ds_3gpp_rmsm_ati_cgact_activation
(
  uint32                  profile_number,
  void *                  user_info_ptr
);

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_ATI_CGACT_DEACTIVATION

  DESCRIPTION
    This function de-activates the Profile activated by this module,
    if not already de-activated .
    If the profile is already de-activated return SUCCESS immediately.

  RETURN VALUE
    DSAT_OK -- Profile is already deactivated
    DSAT_ERROR -- In case of any FAILURE
    DSAT_ASYNC_CMD -- Profile is being deactivated.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
dsat_result_enum_type  ds_3gpp_rmsm_ati_cgact_deactivation
(
  uint32                profile_number,
  void *                user_info_ptr
);

/*===========================================================================
FUNCTION   DS_3GPP_RMSM_ATI_INIT_ALL_INSTANCES

DESCRIPTION
  This function initializes all RMSM AT instances 
  
NOTE: THIS IS A RESTRICTED FUNCTION AND IS INTENDED TO BE USED BY 
DS_3GPP_AT MODULES ONLY


RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_3gpp_rmsm_ati_init_all_instances
( 
  void
);

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_ATI_CGACT_QUERY

  DESCRIPTION
    This function returns teh profile status of all PDP contexts. It 
    retrieves data from DS_3GPP_RMSM_ST interface and Mode handler for
    valid PDP profiles.

  RETURN VALUE
    DSAT_OK -- if SUCCESS
    DSAT_ERROR -- if FAILURE

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
dsat_result_enum_type ds_3gpp_rmsm_ati_cgact_query
(
  ds_3gpp_atif_profile_status_info_type * profile_status_info_ptr
);

/*===========================================================================
FUNCTION   DS_3GPP_RMSM_ATI_GET_V4_UM_INST_STATE

DESCRIPTION
  This function returns the RMSM AT instance from  V4 Um Iface pointer if present 
  
NOTE: THIS IS A RESTRICTED FUNCTION AND IS INTENDED TO BE USED BY 
DS_AT MODULES ONLY

RETURN VALUE
  PS IFACE STATE

DEPENDENCIES
  ds_3gpp_ati_init_all_instances() should have been called first.

SIDE EFFECTS
  None
===========================================================================*/
ds_pdn_mgr_instance_state_e ds_3gpp_rmsm_ati_get_v4_um_inst_state
(
  ds_3gpp_rmsm_at_instance_type instance
);

/*===========================================================================
FUNCTION   DS_3GPP_RMSM_ATI_GET_V6_UM_INST_STATE

DESCRIPTION
  This function returns the RMSM AT instance from  V6 Um Iface pointer if present 
  
NOTE: THIS IS A RESTRICTED FUNCTION AND IS INTENDED TO BE USED BY 
DS_AT MODULES ONLY

RETURN VALUE
  PS IFACE STATE

DEPENDENCIES
  ds_3gpp_ati_init_all_instances() should have been called first.

SIDE EFFECTS
  None
===========================================================================*/
ds_pdn_mgr_instance_state_e ds_3gpp_rmsm_ati_get_v6_um_inst_state
(
  ds_3gpp_rmsm_at_instance_type instance
);

/*===========================================================================
FUNCTION   DS_3GPP_RMSM_ATI_GET_INSTANCE_NUM_FROM_PROFILE

DESCRIPTION
  This function returns the RMSM AT instance from profile number if present 
  
NOTE: THIS IS A RESTRICTED FUNCTION AND IS INTENDED TO BE USED BY 
DS_AT MODULES ONLY

RETURN VALUE
  The instance number:
     valid DS_3GPP_RMSM instance  if Success
     DS_3GPP_RMSM_AT_UM_MAX_INSTANCES if failure to allocate an instance

DEPENDENCIES
  ds_3gpp_ati_init_all_instances() should have been called first.

SIDE EFFECTS
  None
===========================================================================*/

ds_3gpp_rmsm_at_instance_type ds_3gpp_rmsm_ati_get_inst_num_from_profile
(
  uint32 profile_number
);

/*===========================================================================
FUNCTION   DS_3GPP_RMSM_ATI_GET_PROFILE_LIST

DESCRIPTION
  This function returns list of active profiles 
  
NOTE: THIS IS A RESTRICTED FUNCTION AND IS INTENDED TO BE USED BY 
DSUMTS_AT MODULES ONLY

RETURN VALUE


DEPENDENCIES
  ds_3gpp_ati_init_all_instances() should have been called first.

SIDE EFFECTS
  None
===========================================================================*/

void ds_3gpp_rmsm_ati_get_profile_list
(
  ds_prim_profile_list_type*  profile_list_info_ptr
);

/*===========================================================================
  FUNCTION   DS_3GPP_RMSM_GET_INFO_PTR_FROM_INSTANCE
  
  DESCRIPTION
    Returns the pointer to 3GPP RMSM_AT instance for the specified instance 
    
  NOTE: THIS IS A RESTRICTED FUNCTION AND IS INTENDED TO BE USED BY 
  DS_RMSM MODULES ONLY
  
  RETURN VALUE
    Pointer to the RMSM AT info structure if a valid instance is passed, else
    NULL
  
  
  DEPENDENCIES
    None
  
  SIDE EFFECTS
    None
  ===========================================================================*/
ds_3gpp_ati_pdp_state_type *  ds_3gpp_rmsm_get_info_ptr_from_instance
(
  ds_3gpp_rmsm_at_instance_type instance
);

/*===========================================================================
FUNCTION   DS_3GPP_RMSM_ATI_DEALLOCATE_INSTANCE

DESCRIPTION
  This function de-allocates RMSM AT instance if present 
  
NOTE: THIS IS A RESTRICTED FUNCTION AND IS INTENDED TO BE USED BY 
DS_3GPP_AT MODULES ONLY

RETURN VALUE
  None

DEPENDENCIES
  ds_3gpp_ati_init_all_instances() should have been called first.

SIDE EFFECTS
  None
===========================================================================*/
void ds_3gpp_rmsm_ati_deallocate_instance
(
  ds_3gpp_rmsm_at_instance_type  instance
);

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_ATI_PROCESS_QCDGEN

  DESCRIPTION
    This function process data to a context activated
    using CGACT PDP IP profile.

  RETURN VALUE
    DSAT_ERROR -- In case of any FAILURE
    DSAT_NO_RSP -- data transfer is halted
                   due to flow control/ pending data 
    DSAT_OK   -- complete data is transferred

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

dsat_result_enum_type ds_3gpp_rmsm_ati_process_qcdgen
(
  uint32                                  profile_number,
  uint32                                  total_data_len,
  void *                                  user_info_ptr
);

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_ATI_GET_PDP_ADDR

  DESCRIPTION
  
  This function returns the PDP addr of the PDN connection for which 
  the Profile number is passed.

  RETURN VALUE
    DSAT_OK -- if SUCCESS
    DSAT_ERROR -- if FAILURE

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
dsat_result_enum_type ds_3gpp_rmsm_ati_get_pdp_addr
(
  uint32                    profile_num,
  ds_profile_addr_type      *pdp_addr_ptr
);

/*===========================================================================
  FUNCTION   DS_3GPP_RMSM_ATI_GET_PROFILELIST_FROM_PROF_MASK
  
  DESCRIPTION
    This function returns list of profiles from input profile mask

  DEPENDENCIES
    None.
  
  RETURN VALUE
    Count of the profiles present in the profile mask
  
  SIDE EFFECTS
    None
===========================================================================*/
uint8 ds_3gpp_rmsm_ati_get_profilelist_from_prof_mask
(
  uint16 prof_mask,
  uint8* prof_num_list
);

/*===========================================================================
FUNCTION   DS_3GPP_RMSM_ATI_GET_NUM_ACTIVE_PDP

DESCRIPTION
  This function returns the number of active calls, activated using AT.

RETURN VALUE
  None

DEPENDENCIES
  ds_3gpp_ati_init_all_instances() should have been called first.

SIDE EFFECTS
  None
===========================================================================*/
ds_3gpp_rmsm_at_instance_type ds_3gpp_rmsm_ati_get_num_active_pdp
(
  void
);

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_ATI_PROCESS_CSODCP

  DESCRIPTION
    This function is used to transmit data over
    control plane to the network from AT terminal.

  PARAMETERS
    profile_number - profile no
    cpdata_length  - number of octets in the data payload
    data_frame     - data payload
    is_MOX_data    â€“ to indicate MO exception data

  RETURN VALUE
    DSAT_ERROR -- In case of any FAILURE
    DSAT_ASYNC_CMD -- in case of SUCCESS

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
dsat_result_enum_type ds_3gpp_rmsm_ati_process_csodcp
(
  uint32                      profile_number,
  uint32                      cpdata_length ,
  dsm_item_type             **data_frame,
  uint8                       rai_ind,
  boolean                     is_MOX_data
);

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_ATI_CRTDCP_HANDLER

  DESCRIPTION
    This function is used to indicate MH that AT is interested in
    listening incoming cp data from network.

  PARAMETERS


  RETURN VALUE
    DSAT_ERROR -- In case of any FAILURE
    DSAT_ASYNC_CMD -- in case of SUCCESS

  DEPENDENCIES
    None

  SIDE EFFECTS
    None

===========================================================================*/
dsat_result_enum_type ds_3gpp_rmsm_ati_crtdcp_handler
(
  boolean                            enable_data_rx,
  downlink_data_cback_type           cb_fptr
);

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_ATI_RX_DATA_CB

  DESCRIPTION
    This function is used to indicate MH that AT is interested in
    listening incoming non-ip data from network.

  PARAMETERS
  wm_p -> ptr to wm
  call_back_data -> will contain bearer_context_p

  RETURN VALUE

  DEPENDENCIES
    None

  SIDE EFFECTS
    None

===========================================================================*/
void ds_3gpp_rmsm_ati_rx_data_cb
(
  ds_fwk_clnt_handle_type    client_handle,
  dsm_item_type              *dsm_item,
  void                       *user_data_ptr
);

/*===========================================================================
  FUNCTION   DS_3GPP_RMSM_ATI_READ_PRIM_DYNAMIC_PARAMS
  
  DESCRIPTION
    This function returns the primary dynamic params requested 
    by ATCoP for LTE.

  DEPENDENCIES
    None.
  
  RETURN VALUE
    FALSE: in case of Failure
    TRUE: If retrieval of dynamic data is success.
  
  SIDE EFFECTS
    None
===========================================================================*/
boolean ds_3gpp_rmsm_ati_read_prim_dynamic_params
(
  uint32                                      profile_number,
  ds_3gpp_atif_dynamic_params_info_type*      dynamic_param_ptr
);

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_ATI_SETUP_RX_DATA_PATH

  DESCRIPTION
    When AT is interested in Rx data, dereg DPM and setup data path to AT.
    When AT is not interested in Rx data, register to DPM.

  PARAMETERS
    enable_AT_data_rx -> indicates if AT is interested in Rx data

  RETURN VALUE

  DEPENDENCIES
    None

  SIDE EFFECTS
    None

===========================================================================*/
void ds_3gpp_rmsm_ati_setup_rx_data_path
(
  boolean                         enable_AT_data_rx
);

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_ATI_SEND_USER_DATA

  DESCRIPTION
    This function is used to indicate MH that AT is interested in
    listening incoming non-ip data from network.

  PARAMETERS
  wm_p -> ptr to wm
  call_back_data -> will contain bearer_context_p

  RETURN VALUE

  DEPENDENCIES
    None

  SIDE EFFECTS
    None

===========================================================================*/
boolean ds_3gpp_rmsm_ati_send_user_data
(
  ds_3gpp_rmsm_at_instance_type instance,
  uint32                        profile_num,
  uint32                        cpdata_length,
  dsm_item_type                 **data_frame,
  uint8                         rai_ind,
  boolean                       is_mox_data
);

#endif /* DS_3GPP_RMSM_ATI_H */
