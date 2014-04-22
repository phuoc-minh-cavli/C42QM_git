#ifndef DS3GPP_H
#define DS3GPP_H

/*===========================================================================
                      DS_3GPP_API.H
DESCRIPTION
This file exposes 3GPP APIs to external clients outside DS.

Copyright (c) 2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE
when           who    what, where, why
--------    ---    ----------------------------------------------------------
08/09/17    sd      First checkin
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "sys.h"
#include "cm_gw.h"
#include "dsm.h"
/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  RRC states in connected mode. PCH states are URA_PCH and CELL_PCH out of PCH
  states are CELL_FACH and CELL_DCH.
---------------------------------------------------------------------------*/
typedef enum ds_3gpp_pch_event_e{
  DS_3GPP_EVENT_OUT_OF_PCH,
             /* UTRAN moves UE out of PCH states(URA_PCH and CELL_PCH) */
  DS_3GPP_EVENT_IN_PCH
             /* UTRAN moves UE into PCH states(URA_PCH and CELL_PCH) */
} ds_3gpp_pch_event_e_type;

typedef struct
{
 ds_3gpp_pch_event_e_type  pch_event;
 sys_sys_mode_e_type       call_mode;
}ds_3gpp_rrc_pch_ind_proc_cmd_type;

typedef struct
{
  uint32 rab_id;
  uint32 ul_data_rate;
  sys_modem_as_id_e_type subs_id;
}ds_3gpp_ul_rab_info_type;

/*---------------------------------------------------------------------------
  This structure is used to read the NV item(NV:global_throttling). Other
  modem subsystems (e.g: NAS) can read this NV item using this struct.
---------------------------------------------------------------------------*/
typedef struct
{ 
  uint8   version ;          /* version of the NV item*/
  boolean global_throttling; /* NV item value */
  uint8 reserve_bytes[2];    /* reserve bytes for future use*/
}ds_3gpp_global_throttle_s;

/*---------------------------------------------------------------------------
 type of the notification other modules making to DS 
---------------------------------------------------------------------------*/
typedef enum 
{
  DS_3GPP_INVALID_IND = -1,
  DS_3GPP_RAU_IND= 0
}ds_3gpp_notify_ind_e_type;

/*---------------------------------------------------------------------------
 RAU events that other modules can notify
---------------------------------------------------------------------------*/
typedef enum 
{
DS_3GPP_INVALID_EVT = -1,
DS_3GPP_RAU_COMPLETE_EVT, //? Currently we need only this event. Added other for future cases.
DS_3GPP_RAU_STARTED,
DS_3GPP_RAU_FAILED
}ds_3gpp_rau_event_e_type;

typedef enum 
{
  DS_3GPP_INVALID_RAU_TYPE = -1,
  DS_3GPP_PERIODIC_TYPE,
  DS_3GPP_RA_CAHNGE_TYPE,
  DS_3GPP_OTHER_TYPE,
  DS_3GPP_RAU_MAX

}ds_3gpp_rau_type;

typedef struct
{
  sys_modem_as_id_e_type     subs_id;
  sys_sys_mode_e_type        sys_mode;
  ds_3gpp_rau_event_e_type   event;
  ds_3gpp_rau_type           rau_type;
}ds_3gpp_rau_event_s_type;

typedef struct 
{
  uint8	version ;		   /* version of the NV item*/
  boolean bcm_info_validation_enabled; /* NV item value */
  uint8 reserve_bytes[2];    /* reserve bytes for future use*/
}ds_3gpp_bcm_info_validation_s;

/*===========================================================================
                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION DS_3GPP_NOTIFY_IND

DESCRIPTION
   This is a generic function, used to notify DS module.

PARAMETERS
 indication type and 
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If Data is available
  FALSE - If Data is not available

SIDE EFFECTS
  None.

===========================================================================*/

void ds_3gpp_notify_ind(ds_3gpp_notify_ind_e_type indication, void *notify_ind_data_ptr);

/*===========================================================================
FUNCTION DS3GPP_PLMN_BLOCKED_FORCE_LTE_PREF

DESCRIPTION
   This is called by lower layers during powere off to see if any PLMN is blocked.
   lower layer will set their next pref sys based on the PLMN blocked status

PARAMETERS
  subs_id 
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If any PLMN is blocked
  FALSE - otherwise

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds3gpp_is_plmn_blocked_force_lte_pref
(
  sys_modem_as_id_e_type	  subs_id
);

/*===========================================================================
FUNCTION DS3GPP_IS_PS_DATA_AVAILABLE

DESCRIPTION
   This function returns TRUE if data is available in any of the uplink
   buffers, otherwise, it returns FALSE.

PARAMETERS

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If Data is available
  FALSE - If Data is not available

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds3gpp_is_ps_data_available(void);

/*===========================================================================
FUNCTION DS3GPP_IS_UE_INIT_QOS_SUPPORTED

DESCRIPTION
   This function returns TRUE if umts UE initiated QOS support
   is available, otherwise, it returns FALSE.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If UE initiated UMTS QOS support is available
  FALSE - If UE initiated UMTS QOS support is not available

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds3gpp_is_ue_init_qos_supported(void);

/*===========================================================================
FUNCTION DS3GPP_IS_SERVICE_REQ_ALLOWED_ON_PLMN

DESCRIPTION
  This function is used to check if the service request throttling timer is 
  currently running on the PLMN passed into the function.

  If the throttling algorithm has never been kicked off in this PLMN or if the
  throttling timer is not running currently for the PLMN, the UE can send the 
  service request over the air.

PARAMETERS
  plmn_id - The PLMN id in which this the service request is to be sent over air

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If it is ok to send Service request over the air for this PLMN
  FALSE - otherwise

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds3gpp_is_service_req_allowed_on_plmn(sys_plmn_id_s_type plmn_id);


/*===========================================================================
FUNCTION DS3GPP_PER_SUBS_IS_SERVICE_REQ_ALLOWED_ON_PLMN

DESCRIPTION
  This function is used to check if the service request throttling timer is 
  currently running on the PLMN passed into the function.

  If the throttling algorithm has never been kicked off in this PLMN or if the
  throttling timer is not running currently for the PLMN, the UE can send the 
  service request over the air.

PARAMETERS
  plmn_id - The PLMN id in which this the service request is to be sent over air
  subs_id - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If it is ok to send Service request over the air for this PLMN
  FALSE - otherwise

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds3gpp_per_subs_is_service_req_allowed_on_plmn
(
  sys_plmn_id_s_type          plmn_id,
  sys_modem_as_id_e_type      subs_id
);

/*==============================================================================

FUNCTION:  ds_eps_bearer_reenqueue_pkts

DESCRIPTION
  This API will be used to traverse the DSM chain(head_dsm_ptr) and
  enqueue each packet to WM (wm_ptr).It will also increase the dne
  count if required based on the num_pkt_bytes

PARAMETERS
    wm_ptr : WM to which pkts need to be re-enqueued
    num_pkt_byes: Number of bytes to be enqueued
    head_dsm_ptr: This will be a chain of DSM items

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

*/

void ds_eps_bearer_reenqueue_pkts
(
  dsm_watermark_type* wm_ptr,     
  uint32 num_pkt_bytes,               
  dsm_item_type* head_dsm_ptr    
);

/*===========================================================================

FUNCTION:  ds_eps_bearer_flush_bytes

DESCRIPTION
  This API will be used to flush the eps bearer and ensure that the wm
  number of bytes to be remaining at the end is less than or equal to the
  bytes specified
  

PARAMETERS
  eps_id : eps id
  num_pkt_byes: Number of bytes to be remaining

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.  
===========================================================================*/
void ds_eps_bearer_flush_bytes
(
  uint8   eps_id,                        /*eps bearer id which needs to be flused*/
  uint32  num_pkt_bytes               /*Total number of bytes to be remaining at end of procedure*/
);

/*===========================================================================
FUNCTION DS_3GPP_COMPARE_PDP_ADDRESS

DESCRIPTION
  This function is used by NAS to compare the pdp_address when network
  sends MODIFY_REQUEST with pdp_address included. DS will compare the
  pdp_address passed by NAS against the pdp_address which is configured on
  V4/V6 interfaces.

PARAMETERS
  identifier   - Identifier used to uniquely identify a PDP/PDN Context.
                 (For W/G/TD it would be nsapi, For LTE it would be eps_bid)
  sys_mode     - Current sys_mode.
  subs_id      - Subscription on which the PDP/PDN is active.
  *pdp_address - Structure containing the pdp_address and pdp_type_num 
                 given by network, which would be used for comparison.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE -  if PDP address sent by network is same as the one configured.
  FALSE - if PDP address sent by network is different than the one
          configured.

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_3gpp_compare_pdp_address
(
  uint8 identifier,
  sys_sys_mode_e_type sys_mode,
  sys_modem_as_id_e_type subs_id,
  pdp_address_T *pdp_address
);

/*===========================================================================
FUNCTION ds3gpp_per_subs_get_config_multi_pdn_same_apn

DESCRIPTION
  This function gets the multi pdn same apn nv value for each subscription

PARAMETERS
  subs_id subscription id

DEPENDENCIES
  None.

RETURN VALUE  
  Boolean Flag indicating whether Multi Pdn same apn nv is enabled.

SIDE EFFECTS
===========================================================================*/

boolean ds3gpp_per_subs_get_config_multi_pdn_same_apn
(
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION ds3gpp_per_subs_get_config_umts_nw_init_qos_support

DESCRIPTION
  This function gets the umts_nw_init_qos_support NV item: 70331
  modem/data/3gpp/umts_nw_initiated_qos_support

PARAMETERS
  None
  
DEPENDENCIES
  None.

RETURN VALUE  
  Boolean Flag indicating whether UMTS NW Init QoS is enabled or not

SIDE EFFECTS
  None

===========================================================================*/
boolean ds3gpp_per_subs_get_config_umts_nw_init_qos_support
(
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION ds3gpp_is_user_data_or_signalling_pending

DESCRIPTION
  This function returns the user data pending / signaling pending information

PARAMETERS
  User_data_Pending flag: Output pointer
  Signalling_pending: Output pointer
  
DEPENDENCIES
  Client of this API should pass non-NULL pointers to this function.

RETURN VALUE  
  None.

SIDE EFFECTS
  None

===========================================================================*/
void ds3gpp_is_user_data_or_signalling_pending
(
  boolean *user_data_pending,   //will be use to set active flag
  boolean *signalling_pending,   //will be use to set signaling active
  boolean *dl_data_expected     //will be use to set dl data expected in cp
);

/*===========================================================================
FUNCTION ds3gpp_is_ue_ciot_only

DESCRIPTION
  This function returns the info about device in ciot
  if any of the bearer is not in CIOT only then return false

PARAMETERS 
  None.  

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds3gpp_is_ue_ciot_only();

/*===========================================================================
FUNCTION ds3gpp_is_user_data_or_signalling_pending

DESCRIPTION
  This function returns the user data pending / signaling pending information

PARAMETERS 
  Subscription Id 
  User_data_Pending flag: Output pointer
  Signalling_pending: Output pointer
  
DEPENDENCIES
  Client of this API should pass non-NULL pointers to this function.

RETURN VALUE  
  None.

SIDE EFFECTS
  None

===========================================================================*/
void ds3gpp_per_subs_is_user_data_or_signalling_pending
(
  sys_modem_as_id_e_type  subs_id,
  boolean                *user_data_pending,   //will be use to set active flag
  boolean                *signalling_pending,  //will be use to set signaling active
  boolean                *dl_data_expected     //will be use to set dl data expected in cp
);

/*===========================================================================
FUNCTION DS3GPP_UM_WM_GET_NUM_BYTES

DESCRIPTION
  This function returns TRUE if data is available in the watermark.

PARAMETERS
  Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  uint32: Number of bytes

SIDE EFFECTS
  None.

===========================================================================*/
uint32 ds3gpp_um_wm_get_num_bytes
(
  sys_modem_as_id_e_type subs_id
);

/*=========================================================================
  FUNCTION DS_3GPP_CP_DATA_SEND_UL_PACKET

  DESCRIPTION
    This function processes the notification from L2 to send additional
    UL Packets.
  
  PARAMETERS
    max_uplink_packets - Max Uplink Packets that can be sent by DS.
                     
  DEPENDENCIES
    None.
  
  RETURN VALUE 
    None.
  
  SIDE EFFECTS 
    None.
  
===========================================================================*/
void ds_3gpp_send_ul_packet
(
  uint8                   max_uplink_packets
);

/*===========================================================================
FUNCTION      DS_3GPP_IS_APN_BLOCKED

DESCRIPTION

PARAMETERS
 
DEPENDENCIES

RETURN VALUE   Boolean TRUE: If apn is blocked
                       FALSE : If apn is not blocked

SIDE EFFECT 
  None.
===========================================================================*/
boolean ds_3gpp_is_apn_blocked
(
  apn_T  apn
);

/*===========================================================================
FUNCTION DS3GPP_CHECK_PENDING_DATA_FOR_CPSR

DESCRIPTION
   This is called by lower layers to query if there is any data pending before

   performing CPSR.If data is present then post a command to DS task so that

   so that CPSR can piggyback first packet.


PARAMETERS
  None 
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If data is pending
  FALSE - otherwise

SIDE EFFECTS
  None.
===========================================================================*/


boolean ds3gpp_check_pending_data_for_cpsr
(
  void
);

/*==============================================================================
FUNCTION DS3GPP_IS_BEARER_FOR_IMS_PDN

DESCRIPTION
  This function is used to check whether the passed bearer ID belongs
  to IMS PDN.

PARAMETERS   :
   EPS bearer ID.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if bearer belong to IMS PDN. 
  FALSE otherwise

SIDE EFFECTS
  None
==============================================================================*/
boolean ds3gpp_is_bearer_for_ims_pdn
(
  byte   bearer_id
);

/*==============================================================================
FUNCTION DS3GPP_IS_BEARER_FOR_NIP_PDN

DESCRIPTION
  This function is used to check whether the passed bearer ID belongs
  to non-IP PDN.

PARAMETERS   :
   EPS bearer ID.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if bearer belong to non-IP PDN. 
  FALSE otherwise

SIDE EFFECTS
  None
==============================================================================*/
boolean ds3gpp_is_bearer_for_nip_pdn
(
  byte   bearer_id
);

/*===========================================================================
FUNCTION DS_3GPP_IS_WWAN_TECH_SUSPENDED

DESCRIPTION
  This function return the status whether WWAN tech is suspended.

PARAMETERS
 None.

DEPENDENCIES
  None.

RETURN VALUE  
  TRUE - IF WWAN tech is suspended.
  FALSE - otherwise.

SIDE EFFECTS
  None
===========================================================================*/
boolean ds_3gpp_is_wwan_tech_suspended
(
  void
);

#endif /* DS3GPP_H */
