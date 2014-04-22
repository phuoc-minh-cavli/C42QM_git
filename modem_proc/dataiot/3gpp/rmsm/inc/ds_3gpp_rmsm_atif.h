#ifndef DS_3GPP_RMSM_ATIF_H
#define DS_3GPP_RMSM_ATIF_H

/*===========================================================================

                          D S _ 3 G P P _ R M S M  _A T I F  H

DESCRIPTION
  This file contains the definitions for RM state machine for ATCoP 
  interactions for PS related commands. The functions in this files are
  used by ATCoP for +CGACT, +CGPADDR ,$QCDGEN commands.
  
  
EXTERNALIZED FUNCTIONS

Copyright (c) 2018 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/


/*===========================================================================

                            EDIT HISTORY FOR FILE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/dataiot.mpss/2.0/3gpp/rmsm/inc/ds_3gpp_rmsm_atif.h#1 $ 
$DateTime: 2020/03/17 08:03:16 $ $Author: pwbldsvc $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
11/12/18    aks    3GPP First revision of file
===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "ds_headers.h"
#include "dsat_v.h"
#include "ds_profile.h"
#include "ds_pdn_pco.h"

/*===========================================================================

                         REGIONAL DATA DECLARATIONS

===========================================================================*/

typedef int ds_3gpp_rmsm_at_instance_type;

#define  DS_ATIF_MAX_PDP_PROFILE_NUM   DS_PROFILE_MAX_PROFILE_NUM

/*---------------------------------------------------------------------------
  Type for +CGACT modes of operation
---------------------------------------------------------------------------*/
typedef enum ds_3gpp_atif_cgact_act_mode
{
  DS_3GPP_ATIF_DEACTIVATION    = 0,         /* +CGACT activation     */
  DS_3GPP_ATIF_ACTIVATION      = 1,         /* +CGACT deactivation   */
  DS_3GPP_ATIF_MAX             = 0xFF       /* Internal use only !   */
} ds_3gpp_atif_cgact_act_mode_e_type;


/*---------------------------------------------------------------------------
  Type for PDP connection status
---------------------------------------------------------------------------*/
typedef enum ds_3gpp_atif_pdp_connect_state
{
  DS_3GPP_ATIF_PDP_DEACTIVATED   = 0,
  DS_3GPP_ATIF_PDP_DEACTIVATING  = 1,
  DS_3GPP_ATIF_PDP_ACTIVATING    = 2,
  DS_3GPP_ATIF_PDP_ACTIVATED     = 3,
  DS_3GPP_ATIF_PDP_MAX           = 0xFF      /* Internal use only ! */
} ds_3gpp_atif_pdp_connect_state_e_type;


/*---------------------------------------------------------------------------
  Type for Profile status and validity related information
---------------------------------------------------------------------------*/
typedef struct
{
  ds_3gpp_atif_pdp_connect_state_e_type  act_status;
  uint32                                 profile_num;
}ds_3gpp_atif_profile_status_type;


/*---------------------------------------------------------------------------
  Type for Profile status and validity for all contexts
---------------------------------------------------------------------------*/
typedef struct
{
  uint8                             profile_count;
  ds_3gpp_atif_profile_status_type  list[DS_ATIF_MAX_PDP_PROFILE_NUM];
}ds_3gpp_atif_profile_status_info_type;

/*---------------------------------------------------------------------------
  Structure to hold the dynamic params    
---------------------------------------------------------------------------*/

typedef struct
{
  uint32   cid;
  uint32   p_cid;
  byte     bearer_id;
}ds_3gpp_atif_secondary_dynamic_info_type;

typedef enum
{
  DS_3GPP_ATIF_PRIM_INFO = 0x0,
  DS_3GPP_ATIF_SEC_INFO,
  DS_3GPP_ATIF_QOS_INFO,
  DS_3GPP_ATIF_TFT_INFO,
  DS_3GPP_ATIF_DYNAMIC_MAX =0xFF
} ds_3gpp_atif_dynamic_e_type;

typedef struct
{
  byte                       bearer_id;
  char                       *apn;
  ds_profile_addr_type       ip_addr;
  ds_ipcp_options_s          v4_ipcp;
  ds_ipcp_options_s          v6_ipcp;
  ds_profile_addr_type       prim_pcscf_addr;
  ds_profile_addr_type       sec_pcscf_addr;
}ds_3gpp_atif_primary_dynamic_info_type;

#define DS_3GPP_ATIF_MAX_PCSCF_SERVERS (2)
/* no of rx filters max + number of tx filters max */

typedef struct
{
  ds_3gpp_atif_dynamic_e_type info_type;
  union
  {
    ds_3gpp_atif_primary_dynamic_info_type   prim_dynamic_info;
  }u;
}ds_3gpp_atif_dynamic_params_info_type;

struct ds_rmsm_ati_prim_profile_list_type{
  uint8     data_len;
  uint32    data_ptr[DS_3GPP_MAX_PDN_CONTEXT];
};

typedef void (*downlink_data_cback_type)
(
  uint32                         profile_number,
  dsm_item_type                  **data_frame
);

/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

                        PUBLIC FUNCTION DECLARATIONS

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
);

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
  uint32                                  profile_number,
  ds_3gpp_atif_cgact_act_mode_e_type      act_state,
  void *                                  user_info_ptr
);

/*===========================================================================
  FUNCTION   DS_3GPP_RMSM_ATIF_QCDGEN_HANDLER
  
  DESCRIPTION
    This Function is called by ATCoP for sending uplink packets of data len as 
    requested by $QCDGEN Command. Only the PDP-IP profile activated by +CGACT 
    context can be used for data transfer. DSAT_NO_RSP will returned if
    data transfer is initiated. Once the data transfer is complete, 
    result code is send using dsat_send_result_code().
    
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
);

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
  ds_profile_addr_type                    *pdp_addr_ptr
);

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
);

/*===========================================================================
  FUNCTION   DS_3GPP_RMSM_READ_DYNAMIC_PARAMS
  
  DESCRIPTION
    This Function is called by ATCoP for reading the dynamic parameters 
    associated with the primary /secondary activated profiles .
    This function returns FALSE if the profile is not activated .

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
  ds_3gpp_atif_dynamic_params_info_type       *dynamic_param_ptr
);

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
  ds_prim_profile_list_type  *profile_list_info_ptr
);

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
);

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
);


#endif /* DS_3GPP_RMSM_ATIF_H */
