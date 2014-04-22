/*!
  @file
  ds_pdn_mgr_psm_hdlr.h

  @brief
  Manages NV/EFS items related to 3GPP MH

  @detail
  OPTIONAL detailed description of this C header file.
  - DELETE this section if unused.

*/

/*===========================================================================

  Copyright (c) 2009-2017 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //components/rel/dataiot.mpss/2.0/3gpp/pdnmgr/inc/ds_pdn_psm_hdlr.h#3 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/01/14   sb      Initial version.
===========================================================================*/

#ifndef DS_PDN_MGR_PSM_HDLR_H
#define DS_PDN_MGR_PSM_HDLR_H
/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "ds_pdn_defs.h"
#include "ds_pdn_manager.h"
#include "ds_pdn_flow_manager.h"
#include "ds_pdn_apn_rate_ctrl.h"
#include "sys.h"
#include "ULogFront.h"
#include "ps_rohc.h"




#define PSM_VALID_COOKIE 0xaeabeabe

typedef enum
{
  DS_PDN_PSM_INFO_NONE = 0,
  DS_PDN_PSM_INFO_IN_PROGRESS,
  DS_PDN_PSM_INFO_DONE
} ds_pdn_psm_info_state_type;

typedef struct{

  uint32                              num_ipv6_extended_address;
  q_type                              ps_v6_addr_list;
  hc_config_T                         hc_config;
  ds_ip_config_type                   ipv4_config;
  ds_ip_config_type                   ipv6_config;
  
}ip_config_s;

typedef struct
{
  /*! Cookie to ensure dyn mem is valid */
  uint32                           cookie; 
  sys_sys_mode_e_type              call_mode; 
  uint8                            bearer_id;
  /* eps bearer id or nsapi*/
  boolean                          default_bearer;
  boolean                          default_pdn;
  ds_pdn_bcm_info_e                bcm_info;
  uint16                           v4_mtu_len;
  uint16                           v6_mtu_len;
  uint16                           non_ip_mtu_len;
  byte                             apn[DS_PROFILE_APN_STRING_LEN+1];
  byte                             resolved_apn[DS_PROFILE_APN_STRING_LEN+1];  
  ds_profile_pdp_type_e            pdp_type;
  ds_apn_ip_support_type_e         ip_support;
  uint8                            pdp_profile_num;
  uint32                           pdp_group_id;  
  byte                             pdp_type_org;
  byte                             pdp_type_num;
  byte                             pdp_addr_len;
  uint8                            lbi;      /* linked bearer id */
  sdf_qos_T                        sdf_qos;
  qos_T                            neg_qos; 
  qos_T                            minimum_qos;
  apn_ambr_T                       apn_ambr;
  sys_pdp_header_comp_e_type       h_comp;
  sys_pdp_data_comp_e_type         d_comp;
  tft_type_T                       tft;
  uint8                            prefix_len;
  ds_pdn_psm_info_state_type       psm_state;
  cp_only_ind_T                    cp_only;
  splmn_rate_ctrl_T                splmn_rate_ctrl;
  sys_plmn_id_s_type               current_plmn;
  ds_apn_rate_ctrl_config_s        apn_rate_ctrl_config;
  int                              apn_rate_ctrl_apn_index;
  ds_pdn_op_reserved_pco_type      operator_pco_info;
  ds_pdn_mgr_modem_reserved_pco_type  modem_pco_info;

  /* IP_CONFIG structure */
  ip_config_s                     ipconfig;

}ds_pdn_psm_info_s;

typedef struct
{
/*! Cookie to ensure dyn mem is valid */
  uint32                           cookie; 
  rohc_psm_context_s_type          rohc_param;
}ds_pdn_psm_rohc_info_s;

/*===========================================================================
FUNCTION DS_3GPP_PSM_READ_PSM_INFO_FROM_BUFFER

DESCRIPTION
  This function reads the psm information from the buffer and fills the
  psm information array during PSM Exit Scenario
  
PARAMETERS   : 
  
 
DEPENDENCIES
  None.

RETURN VALUE
  True/False
 
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_psm_read_psm_info_from_buffer();

/*===========================================================================
FUNCTION  ds_pdn_psm_info_find_next_index

DESCRIPTION
 This functions is used to provide the next index in the psm info table
 which has the same subscription id as the one passed and is PSM_NONE
 state

PARAMETERS 
  subs_id - subscription id 

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
uint8 ds_pdn_psm_info_find_next_index();

/*===========================================================================
FUNCTION  ds_pdn_psm_info_find_current_index

DESCRIPTION
 This functions is used to provide the next current in the psm info table
 which has the same subscription id as the one passed and is
 DS_PDN_PSM_INFO_IN_PROGRESS  state

PARAMETERS 
  subs_id - subscription id 

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
uint8 ds_pdn_psm_info_find_current_index
(
  
);

/*===========================================================================
FUNCTION  ds_pdn_psm_info_find_index_by_eps_id

DESCRIPTION
 This functions is used to find the index in the psm info table
 which has the same subscription id as the one passed and has
 matching eps id

PARAMETERS 
  subs_id - subscription id 

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
uint8 ds_pdn_psm_info_find_index_by_bearer_id
(
   uint8                    bearer_id
  
);

/*===========================================================================
FUNCTION  ds_pdn_psm_info_fill_ipv6_config

DESCRIPTION
 This functions is used to fill the ipv6 config from the psm index indicated
 

PARAMETERS 
  psm_index               Index in the psm info array
  ipv6_config_info_ptr    IPV6 Config Ptr

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTSc
  None.

===========================================================================*/
void  ds_pdn_psm_info_fill_ipv6_conifg
(
  uint8                psm_index,
  ds_ip_config_type   *ipv6_config_info_ptr,
  uint8               *prefix_len
);

/*===========================================================================
FUNCTION  ds_pdn_psm_info_apply_extended_ipv6_conifg

DESCRIPTION
 This functions is used to apply the extebded ipv6 config from the 
 psm index indicated
 

PARAMETERS 
  psm_index               Index in the psm info array
  ps_iface_ptr            ipv6 pointer

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTSc
  None.

===========================================================================*/

void  ds_pdn_psm_info_apply_extended_ipv6_conifg
(
  uint8                   psm_index,
  ds_pdn_mgr_s           *pdn_mgr_p
);


/*===========================================================================
FUNCTION  ds_pdn_psm_get_elapsed_time

DESCRIPTION
 This functions is used to get the elapsed time from the last psm
 restoration
 

PARAMETERS 

DEPENDENCIES
  None

RETURN VALUE
  uint64- elapsed time in  milli seconds

SIDE EFFECTSc
  None.

===========================================================================*/
uint64 ds_pdn_psm_get_elapsed_time();

/*===========================================================================
FUNCTION  ds_pdn_psm_ready_cmd

DESCRIPTION
 This functions is invoked a part of PSM READY command processing

PARAMETERS
  cmd_ptr

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_psm_ready_cmd
(
  const ds_command_type      *cmd_ptr
);

/*===========================================================================
FUNCTION  ds_pdn_psm_enter_cmd

DESCRIPTION
 This functions is invoked a part of PSM READY command processing

PARAMETERS
  cmd_ptr

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_psm_enter_cmd
(
  const ds_command_type      *cmd_ptr
);

/*===========================================================================
FUNCTION ds_pdn_PSM_POPULATE_INFO_FROM_BEARERS

DESCRIPTION
  This function is used to populated the psm info
  table from then active bearers

PARAMETERS   : 
  
 
DEPENDENCIES
  None.

RETURN VALUE
  None
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_psm_populate_info_from_bearers
(
#ifdef FEATURE_NBIOT_NTN
  boolean           is_ntn_ind
#endif /* FEATURE_NBIOT_NTN */
);


/*===========================================================================
FUNCTION DS_PDN_PSM_FILL_BEARER_RESPONSE_INFO_LTE

DESCRIPTION
  This function is used to fill the bearer response info based
  on the current psm index
 
PARAMETERS   : 
  restore_bearer_req_ptr poiter to activate_default_bearer_request msg from NAS

DEPENDENCIES
  None.

RETURN VALUE
  None
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_psm_fill_bearer_response_info_lte
(
  emm_act_default_bearer_context_request_ind_s_type    *restore_bearer_req_ptr
);



/*===========================================================================
FUNCTION ds_pdn_PSM_FILL_BEARER_RESPONSE_INFO_UMTS

DESCRIPTION
  This function is used to fill the bearer response info based
  on the current psm index
 
PARAMETERS   : 
  restore_bearer_req_ptr  mm_restore_pdp_ctxt_req_s_type     from NAS
 
DEPENDENCIES
  None.

RETURN VALUE
  None
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_psm_fill_bearer_response_info_gsm
(
  mm_pdp_activate_cnf_T    *restore_bearer_req_ptr
);


/*===========================================================================
FUNCTION ds_pdn_PSM_FREE_INFO

DESCRIPTION
  This function is used to free the psm info table
 
 
PARAMETERS   : 
  
 
DEPENDENCIES
  None.

RETURN VALUE
  None
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_psm_free_info();


/*===========================================================================
FUNCTION  ds_pdn_psm_bringup_pdn_connection

DESCRIPTION
 This functions is used to bring up the pdn connection  for the specified
 profile id.
 
 
PARAMETERS 
  psm_index          PSM index

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

boolean ds_pdn_psm_bringup_pdn_connection
(
  uint8                    psm_index
);

/*===========================================================================
FUNCTION  ds_pdn_psm_set_psm_index_state

DESCRIPTION
 This functions is used to the set the state of psm info table for a particular
 index.
 
 
PARAMETERS 
 index             PSM index
 psm_info_stste    PSM state
 
DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_psm_set_psm_index_state
(
  uint8                         index,  
  ds_pdn_psm_info_state_type    psm_info_state
);


/*===========================================================================
FUNCTION  ds_pdn_psm_fill_PdnAddress

DESCRIPTION
 This functions is used to the fill the pdn address pointer based on the
 pdn index 
 
 
PARAMETERS 
 pdn_addr_ptr- pdn address pointer
 psm_index - psm index 

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_psm_fill_PdnAddress
(
   pdn_address_T     *pdn_addr_ptr,
   uint8             psm_index

);

/*===========================================================================
FUNCTION  ds_pdn_psm_fill_PdpAddress

DESCRIPTION
 This functions is used to the fill the pdn address pointer based on the
 pdn index in UMTS mode
 
 
PARAMETERS 
 pdn_addr_ptr- pdn address pointer
 psm_index - psm index 

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_psm_fill_PdpAddress
(
   pdp_address_T     *pdp_addr_ptr,
   uint8             psm_index

);

/*===========================================================================
FUNCTION  ds_pdn_psm_fill_restore_bearer_ctxt_req

DESCRIPTION
 This functions is used to the fill the restore bearer context request 
 pointer based on the current psm index which is being processed 
 
 
PARAMETERS 
  restore_bearer_req_ptr : restore bearer context request pointer
 
DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

boolean ds_pdn_psm_fill_bearer_restore_bearer_ctxt_req
(
  emm_restore_bearer_ctxt_req_s_type    *restore_bearer_req_ptr
);

/*===========================================================================
FUNCTION  ds_pdn_psm_restore_context_request

DESCRIPTION
 This functions is invoked as part of the command processing request from
 cm to restore context
 
PARAMETERS 
  sys_mode - network mode

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_psm_restore_context_request
(
   sys_sys_mode_e_type      sys_mode
);

/*===========================================================================
FUNCTION  ds_pdn_psm_restore_context_request

DESCRIPTION
 This functions is invoked as part of the command processing request  to
 restore bearer from particular psm index
 
PARAMETERS 
  index   - psm index in the psm info array 

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_psm_restore_bearer_context_request
(
   uint8                    index
  
);

/*===========================================================================
FUNCTION  ds_pdn_psm_set_restoration_state

DESCRIPTION
 This functions is invoked to set the psm restoration state

 
PARAMETERS
  state - TRUE/FALSE

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_psm_set_restoration_state
(
  boolean                 state
);

/*===========================================================================
FUNCTION  ds_pdn_psm_get_restoration_state

DESCRIPTION
 This functions is invoked to get the psm restoration state

 
PARAMETERS 

DEPENDENCIES
  None

RETURN VALUE
  state - TRUE/FALSE

SIDE EFFECTS
  None.

===========================================================================*/

boolean ds_pdn_psm_get_restoration_state();

/*===========================================================================
FUNCTION  ds_pdn_psm_get_cached_plmn

DESCRIPTION
 This functions is invoked to get the plmn saved into PSM cache

 
PARAMETERS 
  index - psm info index which can be mapped to a bearer id

DEPENDENCIES
  None

RETURN VALUE
  state - TRUE/FALSE

SIDE EFFECTS
  None.

===========================================================================*/

sys_plmn_id_s_type ds_pdn_psm_get_cached_plmn
(
  uint8 index
);

/*===========================================================================
FUNCTION  ds_pdn_psm_send_attach_complete_indication

8DESCRIPTION
 This functions is invoked to indicate attach complete in psm mode after
 all the contexts are restored
 
 
PARAMETERS 

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void  ds_pdn_psm_send_attach_complete_indication();

/*===========================================================================
FUNCTION  ds_pdn_psm_set_pco_parameters

DESCRIPTION
 This functions is invoked to set the pco parameters as part of restoring the
 bearer
 
 
PARAMETERS 
  pdn_mgr-p      pdn_manager_ptr

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_psm_set_pco_parameters
(
  ds_pdn_mgr_s      *pdn_mgr_p
);

/*===========================================================================
FUNCTION  ds_pdn_psm_update_network_mode

DESCRIPTION
 This functions is used to update the network to LTE during psm restoration
 
 
PARAMETERS 
  network_mode : sys_mode
 
DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void ds_pdn_psm_update_network_mode
(

  sys_sys_mode_e_type   network_mode
);

/*===========================================================================
FUNCTION ds_pdn_PSM_WRITE_PSM_INFO_FROM_FILE

DESCRIPTION
  This function writes the psm information array to the efs file for
  restoration during PSM Enter scenario
  
PARAMETERS   : 
  
 
DEPENDENCIES
  None.

RETURN VALUE
  True/False
 
SIDE EFFECTS
  None.

===========================================================================*/

boolean ds_pdn_psm_write_psm_info_to_file();

/*===========================================================================
FUNCTION ds_pdn_PSM_READ_PSM_INFO_FROM_FILE

DESCRIPTION
  This function reads the psm information from the file and fills the
  psm information array during PSM Exit Scenario
  
PARAMETERS   : 
  
 
DEPENDENCIES
  None.

RETURN VALUE
  True/False
 
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_psm_read_psm_info_from_file();

/*===========================================================================
FUNCTION ds_pdn_PSM_DELETE_FILE_FROM_DIRECTORY

DESCRIPTION
  This function is used to delete the psm info from the directory after the 
  file is read at boot up
  
PARAMETERS   : 
  
 
DEPENDENCIES
  None.

RETURN VALUE
  None
 
SIDE EFFECTS
  None.

===========================================================================*/

void  ds_pdn_psm_delete_file_from_directory();

/*===========================================================================
FUNCTION  ds_pdn_psm_get_info_ptr

DESCRIPTION
 This functions is used to provide PSM info ptr for the passed index

PARAMETERS 
  Index

DEPENDENCIES
  None

RETURN VALUE
  PSM infor ptr.

SIDE EFFECTS
  None.

===========================================================================*/
ds_pdn_psm_info_s* ds_pdn_psm_get_info_ptr(uint8 index);

/*===========================================================================
FUNCTION ds_pdn_PSM_INFO_VALIDATE

DESCRIPTION
  This function validates the Psm Info Pointer
  
PARAMETERS   : 
  psm_info_ptr      - pointer to PSM Info
 
DEPENDENCIES
  None.

RETURN VALUE
  True/False
 
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_psm_info_validate
(
  ds_pdn_psm_info_s *psm_info_ptr
);

/*===========================================================================
FUNCTION ds_pdn_psm_print_bearer_info

DESCRIPTION
  This function is used to print all the bearers and PSM related info
 
PARAMETERS
 None
 
DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void ds_pdn_psm_print_bearer_info();

/*===========================================================================
FUNCTION ds_pdn_PSM_INFO_ALLOCATE_NEW_INSTANCE

DESCRIPTION
  This function is used to return a psm instance from the pool of available
  psm context The allocation and deallocation details are completely hidden from
  the caller.
 
PARAMETERS
 
 
DEPENDENCIES
  None.

RETURN VALUE
  ds_pdn_psm_info_s*

SIDE EFFECTS
  None.
===========================================================================*/
ds_pdn_psm_info_s*  ds_pdn_psm_info_allocate_new_instance();

/*===========================================================================
FUNCTION  ds_pdn_psm_ready_ind

DESCRIPTION
 This functions is invoked a part of PSM READY indicatiom processing

PARAMETERS

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_psm_ready_ind();

/*===========================================================================
  FUNCTION ds_pdn_PSM_HANDLE_BEARER_ACTIVATION

  DESCRIPTION
    This function ensures that when bearer activate is called, special
    call flow for when PSM is enabled for bringing up the bearer is executed
  
  PARAMETERS   
   bearer context    beare context pointer
 
  DEPENDENCIES
    None.
  
  RETURN VALUE 
    boolean for success or failure
  
  SIDE EFFECTS 
    None.
  
===========================================================================*/
boolean ds_pdn_psm_handle_bearer_activation
(
  ds_bearer_mgr_s                   *bearer_context_p
);

/*===========================================================================
FUNCTION  ds_pdn_psm_fill_lte_bearer_restore_params

DESCRIPTION
 This functions is used to the fill the restore bearer context request 
 pointer for LTE based on the current psm index which is being processed 
 
 
PARAMETERS 
  lte_restore_bearer_req_ptr : restore bearer context request pointer
  index                      : PSM index

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_psm_fill_lte_bearer_restore_params
(
  emm_restore_bearer_ctxt_req_s_type    *lte_restore_bearer_req_ptr,
  uint8                                   index
);


/*===========================================================================
FUNCTION  ds_pdn_psm_fill_umts_bearer_restore_params

DESCRIPTION
 This functions is used to the fill the restore bearer context request 
 pointer for UMTS based on the current psm index which is being processed 
 
 
PARAMETERS 
  gsm_restore_bearer_req_ptr : restore bearer context request pointer
  index                      : PSM index
 
DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_psm_fill_gsm_bearer_restore_params
(
  mm_restore_pdp_ctxt_req_s_type   *gsm_restore_bearer_req_ptr,
  uint8                               index 
);

/*===========================================================================
FUNCTION ds_pdn_PSM_POPULATE_UMTS_SPECIFIC_INFO_FROM_BEARERS

DESCRIPTION
  This function is used to populated the psm info
  table from then active bearers

PARAMETERS   : 
  
 
DEPENDENCIES
  None.

RETURN VALUE
  None
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_psm_populate_gsm_info_from_bearers(
  ds_bearer_mgr_s        *bearer_context_p,
  ds_pdn_mgr_s           *pdn_context_p,
  ds_pdn_psm_info_s      *psm_info_p
);


/*===========================================================================
FUNCTION ds_pdn_PSM_POPULATE_LTE_SPECIFIC_INFO_FROM_BEARERS

DESCRIPTION
  This function is used to populated the psm info
  table from then active bearers

PARAMETERS   : 
  
 
DEPENDENCIES
  None.

RETURN VALUE
  None
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_psm_populate_lte_info_from_bearers(  
  ds_bearer_mgr_s          *bearer_context_p,
  ds_pdn_psm_info_s        *psm_info_p,
  ds_pdn_psm_rohc_info_s   *psm_rohc_info_p
);

/*===========================================================================
FUNCTION ds_bearer_psm_resume_umts

DESCRIPTION
  This function is used to resume in UMTS after PSM is complete
 
PARAMETERS
 sys_modem_as_id_e_type  subs_id
 
DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void ds_pdn_psm_activate_umts_bearers();

/*===========================================================================
FUNCTION DS_PDN_PSM_HANDLE_ATTACH_WO_PDN_PSM_RESTORE

DESCRIPTION
  This function is used to handle PSM restore for attach w/o pdn
 
PARAMETERS
 sys_sys_mode_e_type     sys_mode
 
DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_pdn_psm_handle_attach_wo_pdn_psm_restore
(
  sys_sys_mode_e_type     sys_mode
);

/*===========================================================================
FUNCTION  ds_pdn_psm_set_V4PdnAddress

DESCRIPTION
  This functions is used to fill the pdn address pointer with the v4
  ipaddress passed
 

PARAMETERS 
   pdn_address_ptr               Pdn Address Pointer
   ipv4_addr                     IPV4 Address

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_psm_set_V4PdnAddress
(
  pdn_address_T *pdn_addr_ptr,
  uint32        ipv4_addr
);

/*===========================================================================
FUNCTION  ds_pdn_psm_set_V6PdnAddress

DESCRIPTION
  This functions is used to fill the pdn address pointer with the v6
  ipaddress passed
 

PARAMETERS 
   pdn_address_ptr               Pdn Address Pointer
   iid                           V6 IID

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_psm_set_V6PdnAddress
(
  pdn_address_T *pdn_addr_ptr,
  uint64         iid
);
/*===========================================================================
FUNCTION  ds_pdn_psm_set_V4V6PdnAddress

DESCRIPTION
  This functions is used to fill the pdn address pointer with the v6
  ipaddress passed
 

PARAMETERS 
   pdn_address_ptr               Pdn Address Pointer
   ipv4_addr                     IPV4 Address
   iid                           V6 IID

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_psm_set_V4V6PdnAddress
(
  pdn_address_T *pdn_addr_ptr,
  uint32         ipv4_addr,
  uint64         iid 
);
/*===========================================================================
FUNCTION  ds_pdn_psm_set_nonIPPdnAddress

DESCRIPTION
  This functions is used to fill the pdn address pointer with the non IP
  ipaddress passed
 

PARAMETERS 
   pdn_address_ptr               Pdn Address Pointer

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_psm_set_nonIPPdnAddress
(
  pdn_address_T *pdn_addr_ptr
);

/*===========================================================================
FUNCTION  ds_pdn_psm_set_V4PdpAddress

DESCRIPTION
  This functions is used to fill the pdn address pointer with the v4
  ipaddress passed
 

PARAMETERS 
   pdn_address_ptr               Pdn Address Pointer
   ipv4_addr                     IPV4 Address

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_psm_set_V4PdpAddress
(
  pdp_address_T *pdp_addr_ptr,
  uint32         ipv4_addr
);
/*===========================================================================
FUNCTION  ds_pdn_psm_set_V6PdpAddress

DESCRIPTION
  This functions is used to fill the pdn address pointer with the v6
  ipaddress passed
 

PARAMETERS 
   pdn_address_ptr               Pdn Address Pointer
   iid                           V6 IID

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_psm_set_V6PdpAddress
(
  pdp_address_T *pdp_addr_ptr,
  uint64         iid
);
/*===========================================================================
FUNCTION  ds_pdn_psm_set_V4V6PdpAddress

DESCRIPTION
  This functions is used to fill the pdn address pointer with the v6
  ipaddress passed
 

PARAMETERS 
   pdn_address_ptr               Pdn Address Pointer
   ipv4_addr                     IPV4 Address
   iid                           V6 IID

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_psm_set_V4V6PdpAddress
(
  pdp_address_T *pdp_addr_ptr,
  uint32        ipv4_addr,
  uint64         iid 
);

/*===========================================================================
FUNCTION ds_pdn_PSM_WRITE_PSM_INFO_TO_BUFFER

DESCRIPTION
  This function writes the psm information array to the buffer for
  restoration during PSM Enter scenario
  
PARAMETERS   : 
OUTPUT :
  Data ptr that needs to be returned with the buffer content.
  Length of the buffer.
 
DEPENDENCIES
  None.

RETURN VALUE
  True/False
 
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_psm_write_psm_info_to_buffer
(
  char    **ptr,
  uint32   *len
);


/*===========================================================================
FUNCTION  ds_pdn_psm_fill_restore_bearer_ctxt_req

DESCRIPTION
 This functions is used to the fill the restore bearer context request 
 pointer based on the current psm index which is being processed 
 
 
PARAMETERS 
  subs_id : subscription id
  restore_bearer_req_ptr : restore bearer context request pointer
 
DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_psm_fill_pdp_restore_ctxt_req
(
  mm_restore_pdp_ctxt_req_s_type    *restore_bearer_req_ptr,
  sys_sys_mode_e_type                bearer_call_mode
);


/*===========================================================================
FUNCTION      ds_pdn_psm_ready_response

DESCRIPTION   This function notifies CM that DS is ready to enter PSM Mode
              and provide the activate timer    
              
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_pdn_psm_ready_response
(
  boolean        status,
  uint32         activate_timer
);

/*===========================================================================
FUNCTION ds_pdn_psm_get_ulog_handle

DESCRIPTION
  Returns ULOG Handle for REG task
  
DEPENDENCIES
  None

RETURN VALUE
  reg_ulog_handle

SIDE EFFECTS
  None
===========================================================================*/
ULogHandle ds_pdn_psm_get_ulog_handle();

/*===========================================================================
FUNCTION ds_pdn_psm_ulog_register

DESCRIPTION
  This function call registers DS Task with ULOG
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_pdn_psm_ulog_register(void);

/*===========================================================================
FUNCTION ds_pdn_process_psm_restore_cntx_req

DESCRIPTION
  This function is to process the psm restoration req from NAS
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/


void ds_pdn_process_psm_restore_cntx_req
(
  mm_restore_psm_ctxt_req_T  *payload_ptr
);

/*===========================================================================
FUNCTION  ds_pdn_psm_allocate_pdn_and_bringup_ifaces

DESCRIPTION
  This functions allocates a new PDN context and new DS ifaces and brings up
  the ifaces in psm mode.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  DS3G_SUCCESS - if all operations went fine
  DS3G_FAILURE - on error

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_psm_allocate_pdn_and_bringup_pdn
(
  uint8                     psm_index,
  ds_profile_type_s        *prof_context,
  ds_apn_ip_support_type_e  ip_support
);

/*===========================================================================
FUNCTION      DS3G_CM_PSM_READY_IND

DESCRIPTION   This function notifies CM that DS is ready to enter PSM Mode
              and provide the activate timer in the indication
              
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_pdn_cm_psm_ready_ind
(
  uint32          activate_timer
);

/*===========================================================================

FUNCTION DS3G_MSH_RESTORE_CONTEXT_RESPONSE

DESCRIPTION
  This function sends response to cm to indicate restore context response
  with status set to success or 'failure only in PSM Mode
  
PARAMETERS
  status        - Restoration success (TRUE/FALSE) is sent to CM
DEPENDENCIES
  

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void  ds_pdn_process_restore_context_response
(
  boolean                       status,
  sys_sys_mode_e_type          sys_mode
#ifdef FEATURE_NBIOT_NTN
  ,boolean                      is_ntn_ind
#endif /* FEATURE_NBIOT_NTN */
  );

/*===========================================================================
FUNCTION DS_PDN_PSM_FREE_ROHC_INFO

DESCRIPTION
  This function is used to free the psm info table
 
 
PARAMETERS   : 
  
 
DEPENDENCIES
  None.

RETURN VALUE
  None
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_psm_free_rohc_info();

/*===========================================================================
FUNCTION ds_pdn_psm_rohc_info_validate

DESCRIPTION
  This function validates the Psm Info Pointer
  
PARAMETERS   : 
  psm_info_ptr      - pointer to PSM Info
 
DEPENDENCIES
  None.

RETURN VALUE
  True/False
 
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_psm_rohc_info_validate
(
  ds_pdn_psm_rohc_info_s *psm_rohc_info_ptr
);
/*===========================================================================
FUNCTION DS_3GPP_PSM_INFO_ALLOCATE_NEW_ROHC_INSTANCE

DESCRIPTION
  This function is used to return a psm instance from the pool of available
  psm context The allocation and deallocation details are completely hidden
  from the caller.
 
PARAMETERS
 
 
DEPENDENCIES
  None.

RETURN VALUE
  ds_3gpp_psm_info_s*

SIDE EFFECTS
  None.
===========================================================================*/
ds_pdn_psm_rohc_info_s*  ds_pdn_psm_info_allocate_new_rohc_instance();
/*===========================================================================
FUNCTION  ds_pdn_psm_get_rohc_info_ptr

DESCRIPTION
 This functions is used to provide PSM rohc info ptr for the passed index

PARAMETERS 
  Index

DEPENDENCIES
  None

RETURN VALUE
  PSM info ptr.

SIDE EFFECTS
  None.

===========================================================================*/
ds_pdn_psm_rohc_info_s* ds_pdn_psm_get_rohc_info_ptr (uint8 index);

/*===========================================================================
FUNCTION DS_PDN_PSM_FIND_PSM_NTN_INFO_PTR

DESCRIPTION
  This function is used to return a psm or ntn info ptr
 
PARAMETERS
 
 
DEPENDENCIES
  None.

RETURN VALUE
  ds_3gpp_psm_info_s*

SIDE EFFECTS
  None.
===========================================================================*/
ds_pdn_psm_info_s*  ds_pdn_find_psm_ntn_info_ptr(uint8  index);

#ifdef FEATURE_NBIOT_NTN

/*===========================================================================
FUNCTION      DS_PDN_STORE_RESET_NTN_CNTXT_CMD

DESCRIPTION   This function processes the MM_STORE_DS_CTXT_IND
              from NAS. This command is notification to store or reset NTN 
              context by network.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_pdn_process_store_reset_ntn_cntxt_cmd
(
  mm_ntn_ds_context_ind_T  *payload_ptr
);

/*===========================================================================
FUNCTION  ds_pdn_ntn_info_find_next_index

DESCRIPTION
 This functions is used to provide the next index in the ntn info table
 which has the same subscription id as the one passed and is PSM_NONE
 state

PARAMETERS 


DEPENDENCIES
  None

RETURN VALUE
  Index.

SIDE EFFECTS
  None.

===========================================================================*/
uint8 ds_pdn_ntn_info_find_next_index();

/*===========================================================================
FUNCTION  ds_pdn_ntn_restore_context_request

DESCRIPTION
 This functions is invoked as part of the command processing request from
 cm to restore context
 
PARAMETERS 
 None

DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE.

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_ntn_restore_context_request
(
  boolean         is_ntn_ind
);

/*===========================================================================
FUNCTION DS_PDN_NTN_FREE_INFO

DESCRIPTION
  This function is used to free the ntn info table
 
 
PARAMETERS   : 
  
 
DEPENDENCIES
  None.

RETURN VALUE
  None
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_ntn_free_info();

/*===========================================================================
FUNCTION DS_3GPP_NTN_INFO_ALLOCATE_NEW_INSTANCE

DESCRIPTION
  This function is used to return a ntn instance from the pool of available
  ntn context The allocation and deallocation details are completely hidden
  from the caller.
 
PARAMETERS
 
 
DEPENDENCIES
  None.

RETURN VALUE
  ds_3gpp_psm_info_s*

SIDE EFFECTS
  None.
===========================================================================*/
ds_pdn_psm_info_s*  ds_pdn_ntn_info_allocate_new_instance();

/*===========================================================================
FUNCTION DS_PDN_WRITE_NTN_INFO_TO_BUFFER

DESCRIPTION
  This function writes the ntn information array to the buffer for
  restoration
  
PARAMETERS   : 
OUTPUT :
  Nonr
 
DEPENDENCIES
  None.

RETURN VALUE
  True/False
 
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_write_ntn_info_to_buffer();

/*===========================================================================
FUNCTION DS_PDN_READ_NTN_INFO_FROM_EFS

DESCRIPTION
  This function reads the ntn information from the efs and fills the
  psm information array during PSM Exit Scenario
  
PARAMETERS   : 
  
 
DEPENDENCIES
  None.

RETURN VALUE
  True/False
 
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_read_ntn_info_from_efs();

/*===========================================================================
FUNCTION DS_PDN_DELETE_NTN_INFO_FROM_EFS

DESCRIPTION
  This function deletes the efs file
  
PARAMETERS   : 
  
DEPENDENCIES
  None.

RETURN VALUE
  NONE
 
SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_delete_ntn_info_from_efs();

/*===========================================================================
FUNCTION DS_3GPP_WRITE_NTN_CONTEXT_TO_FILE

DESCRIPTION
  This function writes the NTN information array to the efs file for
  restoration
  
PARAMETERS   : 
  
 
DEPENDENCIES
  None.

RETURN VALUE
  True/False
 
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_write_ntn_context_to_efs
(
  char     *buf,
  uint32    buf_len
);

#endif /* FEATURE_NBIOT_NTN */
#endif /* DS3GPPPSMHANDLER_H*/


