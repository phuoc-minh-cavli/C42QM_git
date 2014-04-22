/*===========================================================================
                        D S _ 3 G P P _ R M S M _ A T I F . C 

DESCRIPTION
  This file contains the interface between ATCoP and RMSM interface.
  
EXTERNALIZED FUNCTIONS

Copyright (c) 2018 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/dataiot.mpss/2.0/3gpp/rmsm/src/ds_3gpp_rmsm_atif.c#1 $ 
  $DateTime: 2020/03/17 08:03:16 $ $Author: pwbldsvc $

when        who    what, where, why
--------    ---   ----------------------------------------------------------
10/12/18    ss    3GPP MH First revision checkin
===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "ds_headers.h"
#include "ds_3gpp_rmsm_atif.h"
#include "ds_3gpp_rmsm_ati.h"
#include "ds_pdn_manager.h"
#include "ds_profile.h"
#include "dsati.h"

/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                        EXTERNAL FUNCTION DEFINTIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

/*===========================================================================
  FUNCTION   DS_3GPP_RMSM_ATIF_CGACT_QUERY
  
  DESCRIPTION
    This Function is called by ATCoP for Querying the Status of PDP
    profiles during +CGACT command execution. The function returns the
    activation status of all the valid profiles. 
    
  DEPENDENCIES
    None.
  
  RETURN VALUE
    DSAT_ERROR: in case of Failure
    DSAT_OK: in case of SUCCESS
  
  SIDE EFFECTS
    None
===========================================================================*/
dsat_result_enum_type ds_3gpp_rmsm_atif_cgact_query
(
  ds_3gpp_atif_profile_status_info_type  *profile_status_info_ptr
)
{
  DS_MSG0(MSG_LEGACY_HIGH,"In ds_3gpp_rmsm_atif_cgact_query");
  
  return ds_3gpp_rmsm_ati_cgact_query(profile_status_info_ptr);
}/* ds_3gpp_rmsm_atif_cgact_query */

/*===========================================================================
  FUNCTION   DS_3GPP_RMSM_ATIF_CGACT_HANDLER
  
  DESCRIPTION
    This Function is called by ATCoP for activation or deactivation of profiles
    requested by +CGACT Command. This function returns DSAT_OK if the profile 
    is already activated by other applications or by +CGACT. Only the profile 
    activated by +CGACT context can only be de-activated from activated state.
    
  DEPENDENCIES
    None.
  
  RETURN VALUE
    DSAT_ERROR: in case of Failure
    DSAT_OK: if the activation state and current state of the profile is same.
    DSAT_ASYNC_CMD: Activation or de-activation is in process.
  
  SIDE EFFECTS
    None
===========================================================================*/
dsat_result_enum_type ds_3gpp_rmsm_atif_cgact_handler
(
  uint32                                profile_number,
  ds_3gpp_atif_cgact_act_mode_e_type    act_state,
  void *                                user_info_ptr
)
{
  dsat_result_enum_type result= DSAT_ASYNC_CMD;
  ds_3gpp_rmsm_at_instance_type instance = DS_3GPP_RMSM_AT_UM_MAX_INSTANCES;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_MSG2(MSG_LEGACY_HIGH,"In ds_3gpp_rmsm_atif_cgact_handler profile_number %d, act state %d ",
                      profile_number,(int)act_state);

  /* Profile validation */
  if(ds_profile_number_is_valid(profile_number) != TRUE)
  {
    DS_MSG1(MSG_LEGACY_ERROR,"Error on PDP profile valid query: %d",profile_number); 
    return (dsat_result_enum_type)DSAT_INVALID_PROFILE; 
  }

  /* state validation */
  switch (act_state)
  {
   case DS_3GPP_ATIF_ACTIVATION:
      result = ds_3gpp_rmsm_ati_cgact_activation( profile_number,
                                                  user_info_ptr);
      if(result == DSAT_ERROR)
      {
        /* If instance is available, de-allocate it in case of ERROR */
        instance = ds_3gpp_rmsm_ati_get_inst_num_from_profile(profile_number);
        if ( DS_3GPP_RMSM_AT_UM_MAX_INSTANCES != instance)
        {
          ds_3gpp_rmsm_ati_deallocate_instance(instance);
        }
      }
      break;
   case DS_3GPP_ATIF_DEACTIVATION:
     result = ds_3gpp_rmsm_ati_cgact_deactivation( profile_number,
                                                   user_info_ptr);
     break;
   default:
     DS_MSG0(MSG_LEGACY_ERROR,"Invalid activation state");
     result = DSAT_ERROR;
     break;
  }
  return result;
}/* ds_3gpp_rmsm_atif_cgact_handler */

/*===========================================================================
  FUNCTION   DS_3GPP_RMSM_ATIF_QCDGEN_HANDLER
  
  DESCRIPTION
    This Function is called by ATCoP for sending uplink packets of data len as 
    requested by $QCDGEN Command. Only the PDP-IP profile activated by +CGACT 
    context can be used for data transfer. DSAT_ASYNC_CMD will returned if
    data transfer is initiated. Once the data transfer is complete, 
    result code is send using dsat_rmsm_done_handler().
    
  DEPENDENCIES
    None.
  
  RETURN VALUE
    DSAT_ERROR: in case of Failure
    DSAT_OK/DSAT_ASYNC_CMD: in case of Success

  SIDE EFFECTS
    None
===========================================================================*/
dsat_result_enum_type ds_3gpp_rmsm_atif_qcdgen_handler
(
  uint32                                  profile_number,
  uint32                                  total_data_len,
  void *                                  user_info_ptr
)
{
  DS_MSG2(MSG_LEGACY_HIGH,"In ds_3gpp_rmsm_atif_qcdgen_handler for profile %d and data len %d",
                profile_number,total_data_len);

  if(ds_profile_number_is_valid((int)profile_number) == FALSE)
  {
    DS_MSG1(MSG_LEGACY_ERROR,"Profile_number:%d is Invalid",profile_number);
    return (dsat_result_enum_type)DSAT_INVALID_PROFILE;
  }


  return ds_3gpp_rmsm_ati_process_qcdgen(profile_number,
                                         total_data_len,
                                         user_info_ptr);
}/* ds_3gpp_rmsm_atif_qcdgen_handler */

/*===========================================================================
  FUNCTION   DS_3GPP_RMSM_ATIF_GET_PDP_ADDR
  
  DESCRIPTION
    This Function is called by ATCoP for Querying the PDP address
    of the activated profile. 
    
  DEPENDENCIES
    None.
  
  RETURN VALUE
    DSAT_ERROR: in case of Failure
    DSAT_OK: in case of SUCCESS
  
  SIDE EFFECTS
    None
===========================================================================*/
dsat_result_enum_type ds_3gpp_rmsm_atif_get_pdp_addr
(
  uint32                                  profile_number,
  ds_profile_addr_type                   *pdp_addr_ptr
)
{
  DS_MSG1(MSG_LEGACY_HIGH,"In ds_3gpp_rmsm_atif_get_pdp_addr for profile num %d",
    profile_number);
  
    /* Profile validation */
  if(ds_profile_number_is_valid((int)profile_number) == FALSE)
  {
    DS_MSG1(MSG_LEGACY_ERROR,"Profile_number:%d is Invalid",profile_number);
    return (dsat_result_enum_type)DSAT_INVALID_PROFILE;
  }
  
  return ds_3gpp_rmsm_ati_get_pdp_addr(profile_number,
                                       pdp_addr_ptr);

}/* ds_3gpp_rmsm_atif_get_pdp_addr */

/*===========================================================================
  FUNCTION   DS_3GPP_RMSM_ATIF_CGCMOD_HANDLER
  
  DESCRIPTION
    This Function is called by ATCoP for modification of profiles
    requested by +CGCMOD Command. This function returns DSAT_ERROR if the 
    profile is not activated by +CGACT. Only the profile.

  DEPENDENCIES
    None.
  
  RETURN VALUE
    DSAT_ERROR: in case of Failure
    DSAT_ASYNC_CMD: Modification is in process.
  
  SIDE EFFECTS
    None
===========================================================================*/
dsat_result_enum_type ds_3gpp_rmsm_atif_cgcmod_handler
(
  uint32                                profile_number,
  void *                                user_info_ptr
)
{
  return DSAT_OK; // to remove once the corresponding changes from ATCOP is cleared 
}/* ds_3gpp_rmsm_atif_cgcmod_handler */

/*===========================================================================
  FUNCTION   DS_3GPP_RMSM_ATIF_READ_DYNAMIC_PARAMS
  
  DESCRIPTION
    This Function is called by ATCoP for reading the dynamic parameters 
    associated with the primary /secondary activated profiles .
    This function returns DSAT_ERROR if the profile is not activated .
    Secondary profile details can be provided only if the context 
    is activated using +CGACT.

  DEPENDENCIES
    None.
  
  RETURN VALUE
    FALSE: in case of Failure
    TRUE: If retrieval of dynamic data is success.
  
  SIDE EFFECTS
    None
===========================================================================*/
boolean ds_3gpp_rmsm_atif_read_dynamic_params
(
  uint32                                      profile_number,
  ds_3gpp_atif_dynamic_e_type                 info_type,
  ds_3gpp_atif_dynamic_params_info_type      *dynamic_param_ptr
)
{
  DS_MSG2(MSG_LEGACY_HIGH,"In ds_3gpp_rmsm_atif_read_dynamic_params for profile %d, info_type %d",
            profile_number,(int)info_type);

      /* Profile validation */
  if((DS_3GPP_ATIF_PRIM_INFO != info_type)
     || (ds_profile_number_is_valid((int)profile_number) == FALSE))
  {
    DS_MSG2(MSG_LEGACY_ERROR,"Profile_number:%d not valid or Wrong info type: %d queried ",
              profile_number,info_type);
    return (dsat_result_enum_type)DSAT_INVALID_PROFILE;
  }
  return ds_3gpp_rmsm_ati_read_prim_dynamic_params(profile_number,
                                                         dynamic_param_ptr);

}/* ds_3gpp_rmsm_atif_read_dynamic_params */


/*===========================================================================
  FUNCTION   DS_3GPP_RMSM_ATIF_GET_ALL_ACTIVE_PRIM_PROFILES
  
  DESCRIPTION
    This Function is called by ATCoP for Querying the list of all 
    active primary profiles.
    
  DEPENDENCIES
    None.
  
  RETURN VALUE
    None
  
  SIDE EFFECTS
    None
===========================================================================*/
void ds_3gpp_rmsm_atif_get_all_active_prim_profiles
(
  ds_prim_profile_list_type * profile_list_info_ptr
)
{
  if(profile_list_info_ptr == NULL)
  {
    DS_MSG0(MSG_LEGACY_ERROR,"Invalid output profile_list info ptr");
    return;
  }
  
  ds_pdn_mgr_get_all_pdn_context_profiles(profile_list_info_ptr);

}/* ds_3gpp_rmsm_atif_get_all_active_pdn_profiles */

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_ATIF_CSODCP_HDLR

  DESCRIPTION
    This Function is called by ATCoP for sending uplink packets
    data over control plane to the network.

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
dsat_result_enum_type ds_3gpp_rmsm_atif_csodcp_hdlr
(
  uint32                      profile_number,
  uint32                      cpdata_length ,
  dsm_item_type             **data_frame,
  uint8                       rai_ind,
  boolean                     is_MOX_data
)
{
  DS_MSG1(MSG_LEGACY_HIGH,"In ds_3gpp_rmsm_atif_csodcp_hdlr for profile %d",
                     profile_number);

  if(ds_profile_number_is_valid((int)profile_number) == FALSE)
  {
    DS_MSG1(MSG_LEGACY_ERROR,"Profile_number:%d is not valid",profile_number);
    return DSAT_ERROR;
  }

  return ds_3gpp_rmsm_ati_process_csodcp(profile_number,
                                         cpdata_length,
                                         data_frame,
                                         rai_ind,
                                         is_MOX_data);
}/* ds_3gpp_rmsm_atif_csodcp_hdlr */

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_ATIF_CRTDCP_HDLR

  DESCRIPTION
    This function is used to indicate that AT is interested in
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
dsat_result_enum_type ds_3gpp_rmsm_atif_crtdcp_hdlr
(
  boolean                            enable_data_rx,
  downlink_data_cback_type           cb_fptr
)
{
  return ds_3gpp_rmsm_ati_crtdcp_handler(enable_data_rx,
                                         cb_fptr);
} /* ds_3gpp_rmsm_atif_crtdcp_hdlr */


