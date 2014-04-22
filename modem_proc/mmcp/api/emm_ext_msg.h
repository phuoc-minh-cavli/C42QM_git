/*===========================================================================

  Copyright (c) 2010 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //components/rel/mmcp.mpss/6.1.10/api/emm_ext_msg.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/03/2011 sar     moved to API folder and removed the ifdef
===========================================================================*/
#ifndef _EMM_EXT_MSG_H_
#define _EMM_EXT_MSG_H_

#include <customer.h>

#include "comdef.h"
#include "msgr_umid.h"
#include "msgr_types.h"
#include "msgr_lte.h"
#include "msgr_nas.h"
#include "sys.h"
#include "lte_nas_common.h"
#include "lte_rrc_ext_msg.h"

#include "lte_nas_esm_message_description.h"
#include "cm_msgr_msg.h"
#include "lte_nas_local_cause.h"
#include "cm_gw.h"    // for eps_bearer_id_T
//#include "esm_cm_msg_if.h"
#include "emm_irat_if_msg.h"
#include "sm_common.h"
#include "nas_exp.h"


/*===========================================================================

                        MACROS DEFINITION

===========================================================================*/

#define NAS_CIOT_DEFAULT_CONFIG  0x0320


/*===========================================================================

                        TYPES DEFINITION

===========================================================================*/

enum
{
  MSGR_DEFINE_UMID(NAS, EMM, IND, T3402_CHANGED, 0x0C, emm_t3402_changed_ind_s_type),
  MSGR_DEFINE_UMID(NAS, EMM, IND, TAI_LIST, 0x0F, emm_tai_list_ind_type),
  MSGR_DEFINE_UMID(NAS, EMM, IND, TAU_COMPLETE, 0x10, emm_tau_complete_ind_s_type),
  MSGR_DEFINE_UMID(NAS, EMM, IND, SELECTED_TAI_INFO, 0x11, emm_rrc_selected_tai_info_ind_s_type)
};

typedef struct
{
  /* Message router header */
  msgr_hdr_s                    msg_hdr; 
  sys_plmn_id_s_type            plmn;
  sys_lac_type                  tac;
  boolean                       hc_config_status_exists;
  uint16                        hc_config_status;
  nas_tau_type					tau_type;
}emm_tau_complete_ind_s_type;
/* T3402 change ind */
typedef struct emm_t3402_changed_ind_s
{
  /* Message router header */
  msgr_hdr_s  msg_hdr;

  /* T3402 value */  
  dword  t3402_value;

} emm_t3402_changed_ind_s_type;



#if 0
enum
{
  MSGR_DEFINE_UMID(NAS, EMM, IND, PLMN_CHANGE, 0x05, emm_plmn_change_ind_type),
  MSGR_DEFINE_UMID(NAS, EMM, IND,ATTACH_COMPLETE, 0x06, emm_attach_complete_ind_type),
  MSGR_DEFINE_UMID(NAS, EMM, IND, T3402_CHANGED, 0x0C, emm_t3402_changed_ind_s_type),
  MSGR_DEFINE_UMID(NAS, EMM, IND, DEACT_NON_EMC_BEARER, 0x0D, emm_deact_non_emc_bearer_ind_s_type),
  MSGR_DEFINE_UMID(NAS, EMM, IND, RESET_APN_SWITCH, 0x0E, emm_reset_apn_switch_ind_type),
  MSGR_DEFINE_UMID(NAS, EMM, IND, TAU_COMPLETE, 0x10, emm_tau_complete_ind_type),
  MSGR_DEFINE_UMID(NAS, EMM, IND, DETACH, 0x11, emm_detach_ind_type)
};


/* PLMN change ind */
typedef struct
{
  /* Message router header */
  msgr_hdr_s          msg_hdr;  

  /* New PLMN */
  sys_plmn_id_s_type  plmn;

}emm_plmn_change_ind_type;

/*APN reset ind*/
typedef struct
{
  /* Message router header */
  msgr_hdr_s          msg_hdr;  
}emm_reset_apn_switch_ind_type;



typedef struct
{
  /* Message router header */
  msgr_hdr_s          msg_hdr; 
  boolean attach_without_pdn;

}emm_attach_complete_ind_type;

typedef struct
{
  /* Message router header */
  msgr_hdr_s                    msg_hdr; 
}emm_detach_ind_type;


typedef struct
{
  /* Message router header */
  msgr_hdr_s                    msg_hdr; 
  sys_plmn_id_s_type            plmn;
  sys_lac_type                  tac;
  boolean                       hc_config_status_exists;
  uint16                        hc_config_status;
}emm_tau_complete_ind_type;

/* T3402 change ind */
typedef struct emm_t3402_changed_ind_s
{
  /* Message router header */
  msgr_hdr_s  msg_hdr;

  /* T3402 value */  
  dword  t3402_value;

} emm_t3402_changed_ind_s_type;

/* Deactivate non emergency bearer ind */
typedef struct emm_deact_non_emc_bearer_ind_s
{
  /* Message router header */
  msgr_hdr_s  msg_hdr;

} emm_deact_non_emc_bearer_ind_s_type;

#endif

typedef struct emm_selected_tai_info_ind
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
  lte_rrc_plmn_s    selected_plmn; 
  word    tac;
  boolean serv_ind_reqd;
} emm_selected_tai_info_ind_s_type;


/*************************************
Function Name EMM_MO_EXCEPTION_ATTACHED_ALLOWED

Function description
Return TRUE if 
•	UE is in dereg state
•	timer 3346 running or barring timer running ? If(timer 3346 running it is not started for MO exception)
•	UE not barred for MO exception 
ELSE FALSE

Argument :- No argument for NOW
Return Type:- TRUE or FALSE
*********************************************/


extern boolean emm_mo_exception_attach_allowed (sys_modem_as_id_e_type sub_id) ;

/*===========================================================================
FUNCTION nas_command_get_scan_params

DESCRIPTION
Function to get the roaming, current camped rat and scanning rat status 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void nas_command_get_scan_params(sys_scan_params_s_type *scan_params);

/*===========================================================================
FUNCTION MM_WWAN_SET_TECH_STATUS

DESCRIPTION
This is a function used by NB1/CATM/GERAN for informing change in WWAN tech status whether tech is loaded or unloaded
This function puts the received WWAN current tech status indication into MM command queue

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern boolean mm_wwan_set_tech_status (boolean tech_loaded);

 
 
extern boolean ds_3gpp_is_apn_blocked(apn_T apn);


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

extern boolean ds3gpp_is_bearer_for_ims_pdn
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

extern boolean ds3gpp_is_bearer_for_nip_pdn
(
  byte   bearer_id
);

typedef enum 
{
  MM_PDP_ACTIVATE_REQ = 0x01,
  MM_PDP_DEACTIVATE_REQ = 0x02,
  MM_PDP_ACTIVATE_REJ_RSP = 0x03,
  MM_PDP_MODIFY_REQ = 0x04,
  MM_PDP_ACTIVATE_SEC_REQ = 0x05,
  MM_PDP_ABORT_REQ = 0x06,

  MM_PS_SIGNALING_REL_REQ = 0x07,

  MM_PDP_GRACEFUL_ABORT_REQ = 0x08,

  MM_PDP_NW_MODIFY_REJ = 0x09,
  MM_PDP_NW_MODIFY_CNF = 0x0A,

  MM_UNBLOCK_ALL_APN_IND = 0x0B,
  MM_RESTORE_PDP_CTXT_REQ = 0x0D,
  MM_PDP_SEC_ACTIVATE_REQ = 0x0E,
  MM_PDP_ACT_SEC_CNF = 0x0F,
  MM_RESTORE_PSM_CTXT_RSP = 0x10,
  MM_PS_DATA_AVAIL_REQ = 0x11,
  EMM_RAB_REESTAB_REQ = 0x13,
  EMM_PDN_CONNECTIVITY_REQ = 0x14,
  EMM_ACT_DEFAULT_BEARER_CONTEXT_ACCEPT = 0x15,
  EMM_ACT_DEFAULT_BEARER_CONTEXT_REJ = 0x16,
  EMM_ACT_DEDICATED_BEARER_CONTEXT_ACCEPT = 0x17,
  EMM_ACT_DEDICATED_BEARER_CONTEXT_REJ = 0x18,
  EMM_MODIFY_BEARER_CONTEXT_ACCEPT = 0x19,
  EMM_MODIFY_BEARER_CONTEXT_REJ = 0x1A,
  EMM_BEARER_RESRC_MOD_REQ = 0x1B,
  EMM_PDN_CONNECTIVITY_ABORT_REQ = 0x1C,
  EMM_PDN_DISCONNECT_REQ = 0x1D,
  EMM_MOD_BEARER_REQ = 0x1E,
  EMM_MOD_BEARER_RSP = 0x1F,
  EMM_BEARER_RESRC_ALLOC = 0x20,
  EMM_BEARER_RESRC_ALLOC_ABORT = 0x21,
  EMM_RESTORE_BEARER_CTXT_REQ = 0x22,
  EMM_ESM_DATA_TRANSPORT_REQ = 0x23,
  EMM_MO_EXCEPTION_DATA_IND = 0x24,

  MM_PDP_ACTIVATE_CNF = 0x81,
  MM_PDP_ACTIVATE_REJ = 0x82,
  MM_PDP_ACTIVATE_IND = 0x83,
  MM_PDP_DEACTIVATE_CNF = 0x84,
  MM_PDP_DEACTIVATE_IND = 0x85,
  MM_PDP_DEACTIVATE_PROMOTE_IND = 0x86,
  MM_PDP_DEACTIVATE_PROMOTE_CNF = 0x87,
  MM_PDP_DEACTIVATE_REJ = 0x88,
  MM_PDP_MODIFY_IND = 0x89,
  MM_PDP_MODIFY_CNF = 0x8A,
  MM_PDP_MODIFY_REJ = 0x8B,
  MM_PDP_ACTIVATE_SEC_CNF = 0x8C,  
  MM_PDP_ACTIVATE_SEC_REJ = 0x8D,
  MM_PDP_SIG_CON_REL_IND = 0x8E,
  MM_PDP_SIG_CON_REL_CNF = 0x8F,

  MM_PDP_ACTIVATE_SEC_IND = 0x90,
  MM_PDP_NW_MODIFY_REQ = 0x91,
  MM_RESTORE_CTXT_PSM_REQ = 0x92,
  MM_RESTORE_PDP_CTXT_RSP = 0x93,
  
  EMM_ACT_DEFAULT_BEARER_CONTEXT_REQUEST_IND = 0x94,
  EMM_DEACT_BEARER_CONTEXT_REQUEST_IND = 0x95,
  EMM_DRB_SETUP_IND = 0x96,
  EMM_ACT_DRB_RELEASED_IND = 0x97,
  EMM_DRB_REESTABLISH_REJECT_IND = 0x98,
  EMM_ACT_DEDICATED_BEARER_CONTEXT_REQUEST_IND = 0x99,
  EMM_BEARER_RESOURCE_ALLOC_REJECT_IND = 0x9A,
  EMM_BEARER_RESOURCE_ALLOC_FAILURE_IND = 0x9B,
  EMM_GET_PDN_CONNECTIVITY_REQUEST_IND = 0x9C,
  EMM_PDN_DISCONNECT_REJECT_IND = 0x9D,
  EMM_PDN_CONNECTIVITY_REJECT_IND = 0x9E,
  EMM_PDN_CONNECTIVITY_FAILURE_IND = 0x9F,
  EMM_MODIFY_BEARER_CONTEXT_REQUEST_IND = 0xA0,
  EMM_DATA_TRANSPORT_IND = 0xA1,
  EMM_DATA_TRANSPORT_CNF = 0xA2,
  EMM_RESTORE_BEARER_CTXT_RSP = 0xA3,

  EMM_ATTACH_COMPLETE_IND = 0xA5,
  EMM_TAU_COMPLETE_IND = 0xA6,
  EMM_PLMN_CHANGE_IND = 0xA7,
  EMM_T3402_CHANGED_IND = 0xA8,
  EMM_DEACT_NON_EMC_BEARER_IND = 0xA9,
  EMM_RESET_APN_SWITCH_IND = 0xAA,
  EMM_DETACH_IND = 0xAD,
  EMM_NOTIFY_IND = 0xAE,
  MM_WWAN_TECH_STATUS_IND = 0xAF,
  EMM_BEARER_CONTEXT_MODIFY_REJECT_IND = 0xB0,
  EMM_TIMER_T3448_STATUS_IND = 0xB1,
  EMM_BEARER_CTXT_TRANSFER_IND = 0xB2,
  EMM_CLEAN_UP_PENDING_MO_EXCEPTION_DATA_IND = 0xB3,
  EMM_PROCESS_PENDING_MO_EXCEPTION_DATA_IND = 0xB4,
  EMM_TIMER_BARRING_STATUS_IND = 0xB5,
  EMM_CPSR_STATUS_IND = 0xB6
  }ds_mm_cmd_e_type;


/* Enum for timer T3448 status 
*/
typedef enum 
{
 EMM_TIMER_T3448_STATUS_UNKNOWN    = -1,
 /**< FOR INTERNAL USE ONLY */

 EMM_TIMER_T3448_STATUS_START,
 /**< T3448 timer started */

 EMM_TIMER_T3448_STATUS_STOP,
 /**< T3448 timer stopped */


EMM_TIMER_T3448_STATUS_MAX   
 /**< FOR INTERNAL USE ONLY */

} emm_timer_t3448_status_e_type;

/* Enum for timer barring status
*/
typedef enum
{
EMM_TIMER_BARRING_STATUS_UNKNOWN = -1,
/**< FOR INTERNAL USE ONLY */

 EMM_TIMER_BARRING_STATUS_START,
/**< Barring timer started */

 EMM_TIMER_BARRING_STATUS_STOP,
/**< barringtimer stopped */

EMM_TIMER_BARRING_STATUS_MAX
/**< FOR INTERNAL USE ONLY */
} emm_timer_barring_status_e_type;

/* Enum for timer barring status 
*/
typedef enum 
{
 EMM_TIMER_BARRING_STATUS_CATEGORY_UNKNOWN    = -1,
 /**< FOR INTERNAL USE ONLY */
 
 EMM_TIMER_BARRING_MO_DATA,
 
 EMM_TIMER_BARRING_MO_EXP_DATA, 
 
 EMM_TIMER_BARRING_STATUS_CATEGORY_MAX   
 /**< FOR INTERNAL USE ONLY */
} emm_timer_barring_status_category_e_type;

typedef enum
{
EMM_CPSR_STATUS_NONE = -1,
EMM_CPSR_STATUS_FAILURE,
EMM_CPSR_STATUS_SUCCESS,
EMM_CPSR_STATUS_MAX
}
emm_cpsr_status;


/******************************************************************************/
/*                                                                            */
/*  EMM_BEARER_CTXT_TRANSFER_IND                                              */
/* Primitive to inform about rat change to DS. On this message                */
/* DS will transfer context to new rat upon NB1<>CATM1 RAT change             */
/*                                                                            */
/******************************************************************************/
typedef struct emm_bearer_ctxt_transfer_ind_s
{
  IMH_T                                               message_header;
  sys_radio_access_tech_e_type                        rat;
} emm_bearer_ctxt_transfer_ind_type;


/*  New DS _MM Header file */


/** @brief RAB re-establish request parameters for LTE
    packet switched calls.
*/
typedef struct emm_rab_reestab_req_s 
{
  IMH_T                        message_header;
  /* Connection ID of the bearer context */
  esm_header_T                 esm_hdr;
  eps_bearer_id_T    eps_bearer_id; /*!< Brief description of this member */
  sys_lte_reestab_calltype_e call_type; /*!< Establishment call type */
  boolean                mo_exception_data;
} emm_rab_reestab_req_s_type;


/** @brief Request for PDN connectivity setup.
*/
typedef struct emm_pdn_connectivity_req_s
{
  IMH_T                        message_header;
  /* Connection ID of the bearer context */
  esm_header_T                 esm_hdr;

  req_type_T              request_type;
    /**< Connection request type. */

  pdn_connection_data_T   pdn_data;
    /**< The payload of the PDN connection message. */

  boolean                     esm_dummy_flag;
    /**< dummy flag to indicate attach w/o pdn support */

} emm_pdn_connectivity_req_s_type ;


extern void nas_ul_count_and_mac_generation_for_cp_only(
    uint8* msg_data_pr,           /*! <this is cell identity proivided by LTE in data ptr */
    uint16 in_msg_bit_len, /*! < Input msg len - bit length  */
    uint32 *ul_mac, 
    uint8 *ul_count
);

extern void nas_dl_mac_generation_for_cp_only(lte_rrc_global_cell_id_t cell_identity,uint32 *dl_mac);

/** @brief Request for PDN connectivity abort.
*/
typedef struct emm_pdn_connectivity_abort_req_s
{
  IMH_T                        message_header;
    /* Connection ID of the bearer context */
  esm_header_T                 esm_hdr;

  esm_cause_T             esm_cause;
    /**< ESM cause value for the PDN connectivity message. */

  lte_nas_local_cause_T   esm_local_cause;
    /**< ESM local cause value for the PDN connectivity message. */

} emm_pdn_connectivity_abort_req_s_type;

/** @brief Request for PDN disconnect.
*/
typedef struct emm_pdn_disconnect_req_s
{
  IMH_T                        message_header;
  /* Connection ID of the bearer context */
  esm_header_T                 esm_hdr;

  eps_bearer_id_T          eps_bearer_id;
    /**< Evolved Packet System bearer ID. */

  protocol_cfg_options_T   protocol_config_options;
    /**< Protocol options for an end point. */

  lte_nas_local_cause_T    esm_local_cause;
    /**< ESM local cause value for the disconnect message. */

  ext_protocol_cfg_options_T  ext_protocol_conf_opt;
    /**< Extended protocol configuration options */

} emm_pdn_disconnect_req_s_type;


/** @brief Request for bearer resource modification.

Not Applicable - UE Initiated QoS not supported
*/
typedef struct emm_mod_bearer_req_s
{

  IMH_T                        message_header;

  esm_header_T                 esm_hdr;

  tft_type_T              ul_dl_tft ;
    /**< Traffic Flow Template filter for resource being allocated. */

  sdf_qos_T               sdf_qos;
    /**< Service Data Flow QoS parameters. */

  esm_cause_T             esm_cause;
    /**< ESM cause in the resource modification message. */

  lte_nas_local_cause_T   esm_local_cause;
    /**< ESM local cause in the resource modification message. */

  protocol_cfg_options_T  protocol_config_options;
    /**< Protocol options for an end point. */

  boolean                 low_priority_signaling;
    /**< NAS signaling low priority indicator */

  hc_config_T               hc_config;
    /**< Contains the header compression related info */

  ext_protocol_cfg_options_T  ext_protocol_conf_opt;
    /**< extended protocol configuration options */

} emm_mod_bearer_req_s_type;


/** @brief Response for bearer resource modification.
(Not Applicable - UE Initiated QoS not supported)
*/
typedef struct emm_mod_bearer_rsp_s
{

  IMH_T                        message_header;
  esm_header_T                 esm_hdr;
  eps_bearer_id_T           eps_bearer_id;
    /**< Bearer ID in the resource modification response. */

  esm_cause_T               esm_cause;
    /**< ESM cause in the resource modification message. */

  lte_nas_local_cause_T     esm_local_cause;
    /**< ESM local cause in the resource modification message. */

  protocol_cfg_options_T    protocol_config_options;
    /**< Protocol options for an end point. */

  boolean                   is_accept;
    /**< Result of resource modification request. */

  ext_protocol_cfg_options_T  ext_protocol_conf_opt;
    /**<extended protocol configuration options */

} emm_mod_bearer_rsp_s_type;


/** @brief Request for bearer resource allocation.
(Not Applicable - UE Initiated QoS not supported)
*/
typedef struct emm_bearer_resrc_alloc_req_s
{
  IMH_T                        message_header;
  /* Connection ID of the bearer context */
  esm_header_T                 esm_hdr;

  sdf_qos_T                 sdf_qos;
    /**< Service Data Flow QoS. */

  tft_type_T                ul_dl_tft;
    /**< Traffic Flow Template filter for resource being allocated. */

  protocol_cfg_options_T    protocol_config_options;
    /**< Protocol options for an end point. */

  byte                      lbi;
    /**< Linked Evolved Packet System Bearer ID. */

  boolean                  low_priority_signaling;
    /**< NAS signaling low priority indicator */
  ext_protocol_cfg_options_T  ext_protocol_conf_opt;
  /**< ext Protocol configuration options */

} emm_bearer_resrc_alloc_req_s_type ;


/** @brief Request to abort bearer resource allocation.
(Not Applicable - UE Initiated QoS not supported)
*/
typedef struct emm_bearer_resrc_alloc_abort_s
{

  IMH_T                        message_header;
    /* Connection ID of the bearer context */
  esm_header_T                 esm_hdr;

  esm_cause_T             esm_cause;
    /**< EPS Session Management (ESM) cause value in the abort message. */

  lte_nas_local_cause_T   esm_local_cause;
    /**< ESM local cause value in the abort message. */

} emm_bearer_resrc_alloc_abort_s_type ;

#define MAX_NO_OF_TAI 16  /* Refer Section: 9.9.3.29; There can be a maximum of 16 TAIs*/
#define MAX_TAI_LST_LENGTH 16



typedef enum
{
  NAS_EMM_TAI_LST0,
  NAS_EMM_TAI_LST1,
  NAS_EMM_TAI_LST2

} lte_nas_tai_lst_type ;

typedef struct
{
  sys_plmn_id_s_type      plmn;
  byte                    tac_lst_length ;
  word                    tac_lst[MAX_NO_OF_TAI]; /*(section 9.9.3.29.4) There can be a maximum of 16 elements, with two octets each*/
}lte_nas_tai_lst0_type; /*lte_nas_tai_...*/

/*Required for tai_type */
typedef struct
{
  sys_plmn_id_s_type     plmn;
  word                   tac; /*(section 9.9.3.29.4) Each TAC takes two octets*/
  boolean                t3247_specific; /*added temporarily while T3247 timer is running */
}lte_nas_tai_lst1_type; /*lte_nas_tai_list_type*/ 

/*Required for tai_type */
typedef struct{
  byte                    tai_lst_length ;
  lte_nas_tai_lst1_type   diff_plmn[LTE_RRC_MAX_NUM_TRACKING_AREAS]; /*(section 9.9.3.33) we can have 40 entries */
}lte_nas_tai_lst2_type; /*lte_nas_tai_plmns*/

/*Required for emm_tai_entry_type */
typedef union{
    lte_nas_tai_lst0_type    tai_lst0;
    lte_nas_tai_lst1_type    tai_lst1;
    lte_nas_tai_lst2_type    tai_lst2;
}tai_type;              

typedef struct  
{
  lte_nas_tai_lst_type tai_lst;
  tai_type             tai;   /* particular structure*/

} emm_tai_lst_elem_type ; 


/*Define TAI entry Type*/
typedef struct{

  byte                  tai_lst_length; /* Number of elements*/
  emm_tai_lst_elem_type tai_lst_elem[MAX_TAI_LST_LENGTH];

}lte_nas_emm_tai_lst_info_type;    

typedef struct
{
   msgr_hdr_s     msg_hdr;    
   lte_nas_emm_tai_lst_info_type  tai_lst_info;
}emm_tai_list_ind_type;


/* --------------------------------------------------------------------------
** Primitive  : MM_PDP_ACTIVATE_REQ, 
** Description: Mobile originated (MO) PDP context activation Req.
** ------------------------------------------------------------------------*/
typedef struct mm_pdp_activate_req
{
  IMH_T                  message_header;
  byte                   connection_id; //Added
  qos_T                  qos;
  qos_T                  minimum_qos;
  pdp_address_T          pdp_addr;
  apn_T                  apn_name;
  protocol_cfg_options_T config_options;

  sys_pdp_header_comp_e_type    h_comp;          /* SNDCP IP Header Compression reqd */
  sys_pdp_data_comp_e_type       d_comp;          /* SNDCP Data Compression reqd */

  llc_sapi_T             llc_sapi;
  nsapi_T                nsapi;
  byte                    trans_id;
  byte                    sm_pd;
  byte                    msg_type;
  uint16                    context_id;
  
  boolean                low_priority_signaling;   /* NAS signaling low priority indicator */

} mm_pdp_activate_req_T;


/** @brief PDP modify parameters for GSM/WCDMA/TDS packet switched calls.
       (Not Applicable - UE Initiated QoS not supported)
*/
typedef struct mm_pdp_modify_req_s {
   IMH_T                        message_header;
   byte                   connection_id; //Added
   nsapi_T                nsapi;
    /**< NSAP information. */
  qos_T                  qos;
    /**< Quality of Service information. */
  tft_type_T             tft;
    /**< Traffic Flow Template. */
  qos_T                  minimum_qos;
    /**< Minimum Quality of Service required for the PDP context. Clients must
         check for CM_API_CHG_PDP_MOD before using this macro. */
  boolean low_priority_signaling;
   /* NAS signaling low priority indicator */       
} mm_pdp_modify_req_s_type;


/* ---------------------------------------------------------------------
** Primitive  : MM_PDP_DEACTIVATE_REQ,
** Description: Mobile Originated PDP context deactivation Request.
** -------------------------------------------------------------------*/
typedef struct mm_pdp_deactivate_req
{
  IMH_T                  message_header;
  byte				   connection_id; //Added
  nsapi_T            nsapi;
  ti_T               tear_down_ind;
  sm_status_T        cause;

  protocol_cfg_options_T config_options;

} mm_pdp_deactivate_req_T;

/* ------------------------------------------------------------------------
** Primitive  : MM-PDP-ACTIVATE-SEC-REQ,
** Description: Mobile Originated Secondary PDP activation Req.

(Not Applicable - UE Initiated QoS not supported)

** ----------------------------------------------------------------------*/
typedef struct mm_pdp_activate_sec_req
{
  IMH_T                  message_header;
  byte                   connection_id; //Added
  nsapi_T                nsapi;
  qos_T                  qos;
  qos_T                  minimum_qos;
  tft_type_T             tft;
  nsapi_T                primary_nsapi;
  byte                   group_id;
  sys_pdp_header_comp_e_type    h_comp;          /* SNDCP IP Header Compression reqd */
  sys_pdp_data_comp_e_type       d_comp;          /* SNDCP Data Compression reqd */
  protocol_cfg_options_T config_options;
  llc_sapi_T             llc_sapi;
  byte                    trans_id;
  byte                    sm_pd;
  byte                    msg_type;

  uint16                    context_id;
  uint16                    assoc_context_id;

  boolean                   low_priority_signaling;   /* NAS signaling low priority indicator */
  

} mm_pdp_activate_sec_req_T;

/* --------------------------------------------------------------------------
** Primitive  : MM_PDP_ACTIVATE_REJ_RESP,
** Description: Mobile sends the PDP activation failed response to the 
                (Mobile Terminated) PDP activation request.

     (Not Applicable - UE Initiated QoS not supported)
** ------------------------------------------------------------------------*/
typedef struct mm_pdp_activate_rej_resp
{
  IMH_T                  message_header;
  byte                   connection_id; //Added
  
  sm_status_T            cause;    

  protocol_cfg_options_T config_options;

} mm_pdp_activate_rej_resp_T;


/* ---------------------------------------------------------------------
** Primitive  : MM-PDP-NW-MODIFY-CNF
** Description: DS accepting MT PDP modification request.
** -------------------------------------------------------------------*/

typedef struct mm_pdp_nw_modify_cnf
{
     IMH_T                  message_header;
     byte                   connection_id; //Added
}mm_pdp_nw_modify_cnf_T;

/* ---------------------------------------------------------------------
** Primitive  : SMREG-PDP-NW-MODIFY-REJ
** Description: DS rejected MT PDP modification request based on BCM constaints.
** -------------------------------------------------------------------*/
typedef struct mm_pdp_nw_modify_rej
{
  IMH_T                  message_header;
  byte                   connection_id; //Added
  sm_status_T            cause;
} mm_pdp_nw_modify_rej_T;


/* --------------------------------------------------------------------------
** Primitive  : MM_PDP_ACTIVATE_CNF, 
** Description: Confirmation for the MO PDP activation request message.
** ------------------------------------------------------------------------*/
typedef struct mm_pdp_activate_cnf
{ 
  IMH_T                  message_header;
  byte                   connection_id;
  nsapi_T                      nsapi;
  byte                   group_id;
  qos_T                  qos;
  pdp_address_T          pdp_addr;
  protocol_cfg_options_T config_options;
  boolean                is_net_alloc_qos_diff_from_req;

  byte                  esm_conn_id;


  cause_enum_type_T  cause_type;
  pdp_cause_T cause;

} mm_pdp_activate_cnf_T;


/* --------------------------------------------------------------------------
** Primitive  : MM_PDP_ACTIVATE_REJ, 
** Description: MO PDP activation request rejected by the network.
** ------------------------------------------------------------------------*/
typedef struct mm_pdp_activate_rej
{ 
  IMH_T                  message_header;
  byte                   connection_id;
  cause_enum_type_T  cause_type;
  pdp_cause_T            cause;
  nsapi_T                nsapi;
  protocol_cfg_options_T config_options;

  byte nas_max_ota_negotiated_len;

  timer_3gpp_type3    backoff_timer;
  
  pdp_reject_ind_params_s_type pdp_reject_param;
  reattempt_ind_T	  reattempt_indicator;
} mm_pdp_activate_rej_T;


/* --------------------------------------------------------------------------
** Primitive  : MM_PDP_ACTIVATE_IND,
** Description: Mobile Terminated (MT) PDP activation indication. Network
                has initiated a PDP activation request.
** ------------------------------------------------------------------------*/
typedef struct mm_pdp_activate_ind
{ 
  IMH_T                  message_header;
  byte                   connection_id;
  byte                   group_id;
  pdp_address_T          pdp_addr;
  apn_T                  apn_name;

  protocol_cfg_options_T config_options;


  llc_sapi_T             llc_sapi;
  //nsapi_T                nsapi;
  byte                    sm_pd;
  byte                    msg_type;

} mm_pdp_activate_ind_T;


/* ---------------------------------------------------------------------
** Primitive  : MM-PDP-DEACTIVATE-CNF
** Description: Confirmation for the MO PDP deactivation request.
** -------------------------------------------------------------------*/
typedef struct mm_pdp_deactivate_cnf
{
  IMH_T                 message_header;
  byte                 connection_id;
  nsapi_T              nsapi;
  protocol_cfg_options_T config_options;
  mbms_prot_config_options_T mbms_config_options;

} mm_pdp_deactivate_cnf_T;


/* ---------------------------------------------------------------------
** Primitive  : MM-PDP-DEACTIVATE-IND
** Description: Mobile Terminated (MT) PDP deactivation indication.
** -------------------------------------------------------------------*/
typedef struct mm_pdp_deactivate_ind
{
  IMH_T                 message_header;
  byte                  connection_id;
  nsapi_T               nsapi;
  ti_T                  tear_down_ind;
  cause_enum_type_T     cause_type;
  pdp_cause_T           cause;

  protocol_cfg_options_T config_options;


  mbms_prot_config_options_T mbms_config_options;

  timer_3gpp_type3    backoff_timer;

} mm_pdp_deactivate_ind_T;


/* ---------------------------------------------------------------------
** Primitive  : MM_PDP_DEACT_PROMOTE_IND
** Description: Mobile Terminated (MT) PDP deactivation indication
                and prmotion of secondary to primary.
** -------------------------------------------------------------------*/
typedef struct mm_pdp_deact_promote_ind
{
  IMH_T                 message_header;
  byte                  connection_id;
  nsapi_T               nsapi;
  byte                  promote_con_id;//For promoting sec to primary
  cause_enum_type_T     cause_type;
  pdp_cause_T           cause;

  protocol_cfg_options_T config_options;

}mm_pdp_deact_promote_ind_T;


/* ---------------------------------------------------------------------
** Primitive  : MM_PDP_DEACT_PROMOTE_CNF
** Description: Mobile Terminated (MT) PDP deactivation indication
                and prmotion of secondary to primary.
** -------------------------------------------------------------------*/
typedef struct mm_pdp_deact_promote_cnf
{
  IMH_T                 message_header;
  byte                  connection_id;
  nsapi_T               nsapi;
  byte                  promote_con_id;//For promoting sec to primary

  protocol_cfg_options_T config_options;
}mm_pdp_deact_promote_cnf_T;


/* ---------------------------------------------------------------------
** Primitive  : MM-PDP-MODIFY-IND
** Description: Indication of a successful MT PDP modification.
** -------------------------------------------------------------------*/
typedef struct mm_pdp_modify_ind
{
   IMH_T            message_header;
  byte              connection_id;
  nsapi_T           nsapi;
  qos_T             qos;

  protocol_cfg_options_T config_options;

  tft_type_T             tft;

} mm_pdp_modify_ind_T;


/* ---------------------------------------------------------------------
** Primitive  : MM-PDP-MODIFY-CNF,
** Description: Confirmation for MO PDP modification request, Network 
                has accepted the Mobile proposed PDP modification.

    (Not Applicable - UE Initiated QoS not supported)
** -------------------------------------------------------------------*/
typedef struct mm_pdp_modify_cnf
{
  IMH_T            message_header;
  byte              connection_id;
  nsapi_T           nsapi;
  qos_T             qos;

  protocol_cfg_options_T config_options;

} mm_pdp_modify_cnf_T;


/* ---------------------------------------------------------------------
** Primitive  : MM-PDP-MODIFY-REJ
** Description: Rejection of MO PDP modification request, Network 
                has rejected the Mobile proposed PDP modification.
    (Not Applicable - UE Initiated QoS not supported)
** -------------------------------------------------------------------*/
typedef struct mm_pdp_modify_rej
{
  IMH_T                message_header;
  byte                 connection_id;
  cause_enum_type_T    cause_type;
  pdp_cause_T          cause;
  nsapi_T                nsapi;

  protocol_cfg_options_T config_options;

  byte nas_max_ota_negotiated_len;

  timer_3gpp_type3    backoff_timer;
  reattempt_ind_T	  reattempt_indicator;

} mm_pdp_modify_rej_T;


/* ---------------------------------------------------------------------
** Primitive  : MM-PDP-DEACTIVATE-REJ
** Description: Rejection of MO PDP modification request, Network 
                has rejected the Mobile proposed PDP modification.
** -------------------------------------------------------------------*/
typedef struct mm_pdp_deactivate_rej
{
  IMH_T               message_header;
  byte                 connection_id;
  cause_enum_type_T    cause_type;
  pdp_cause_T          cause;
  nsapi_T                nsapi;

} mm_pdp_deactivate_rej_T;


/* ---------------------------------------------------------------------
** Primitive  : MM-PDP-ACTIVATE-SEC-CNF  
** Description: Confirmation for MO SEC PDP modification request, Network 
                has accepted the Mobile proposed SEC PDP modification.
** -------------------------------------------------------------------*/
typedef struct mm_pdp_activate_sec_cnf
{
  IMH_T                  message_header;
  byte                   connection_id;
  nsapi_T                nsapi;
  qos_T                  qos;
  boolean                is_net_alloc_qos_diff_from_req;

  protocol_cfg_options_T config_options;

//  byte                      esm_conn_id;

} mm_pdp_activate_sec_cnf_T;


/* ---------------------------------------------------------------------
** Primitive  : MM-PDP-ACTIVATE-SEC-REJ  
** Description: Rejection of MO SEC PDP modification request, Network 
                has rejected the Mobile proposed SEC PDP modification.
** -------------------------------------------------------------------*/
typedef struct mm_pdp_activate_sec_rej
{
  IMH_T                 message_header;
  byte                   connection_id;
  nsapi_T                nsapi;
  cause_enum_type_T      cause_type;
  pdp_cause_T            cause;

  protocol_cfg_options_T config_options;

  byte nas_max_ota_negotiated_len;

  timer_3gpp_type3    backoff_timer;   

  pdp_reject_ind_params_s_type pdp_reject_param;
  reattempt_ind_T     reattempt_indicator;

} mm_pdp_activate_sec_rej_T;


/* --------------------------------------------------------------------------
** Primitive  : MM_SEC_PDP_ACTIVATE_IND,
** Description: Mobile Terminated (MT) Secondary PDP activation indication. Network
                has initiated a Secondary PDP activation request.
** ------------------------------------------------------------------------*/
typedef struct mm_pdp_activate_sec_ind
{ 

  IMH_T				  message_header;
  byte                   connection_id;
  byte                   group_id;
  tft_type_T             tft;
  protocol_cfg_options_T config_options;
  qos_T                  qos;
  llc_sapi_T             llc_sapi;
  nsapi_T                nsapi;
  byte                    sm_pd;
  byte                    msg_type;
  
} mm_pdp_activate_sec_ind_T;


/* ---------------------------------------------------------------------
** Primitive  : MM-PDP-NW-MODIFY-REQ
** Description: Request for DS to validate TFT params.
** -------------------------------------------------------------------*/
typedef struct mm_pdp_nw_modify_req
{

  IMH_T                 message_header;
  byte              connection_id;
  nsapi_T           nsapi;   
  tft_type_T             tft;
  protocol_cfg_options_T config_options;
  qos_T              qos;
}mm_pdp_nw_modify_req_T;


typedef struct
{
  IMH_T                  message_header;
} mm_ps_data_avail_req_T ;


// Doubt MM_UNBLOCK_ALL_APN_IND (Check *)

/* ---------------------------------------------------------------------
** Primitive  : MM-UNBLOCK-ALL-APN-IND
** Description: Request for DS to unblock all APNs.
** -------------------------------------------------------------------*/
//typedef struct mm_unblock_all_apn
//{
//} mm_unblock_all_apn_ind_T;


/* ---------------------------------------------------------------------
** Primitive  : MM-BLOCKED_APN-LIST
** Description: List of all blocked APN from DS
** -------------------------------------------------------------------*/
typedef struct mm_blocked_apn_list
{
  IMH_T                  message_header;
  apn_T                  apn_name[MAX_BLOCKED_APN];
  uint16                 length;
} mm_blocked_apn_list_ind_T;

/* ---------------------------------------------------------------------
** Primitive  : MM-UNBLOCK_ALL_APN-IND
** Description: List of all blocked APN from DS
** -------------------------------------------------------------------*/
typedef struct mm_unblock_all_apn_ind
{
  IMH_T                  message_header;
} mm_unblock_all_apn_ind_T;

/* ---------------------------------------------------------------------
** Primitive : MM-RESTORE-CTXT-PSM-REQ
** Description: Request for DS (via CM) to trigger PSM context restoration.
** -------------------------------------------------------------------*/
typedef struct mm_restore_psm_ctxt_req
{
     IMH_T                        message_header;
    sys_sys_mode_e_type           sys_mode; 
    /**< Defines the current RAT. */
} mm_restore_psm_ctxt_req_T;


/* ---------------------------------------------------------------------
** Primitive  : MM-RESTORE-CTXT-PSM-RSP
** Description: DS (via CM) responds status of PSM context restore request after restoring all PDP contexts.
** -------------------------------------------------------------------*/

typedef struct mm_restore_psm_ctxt_rsp
{
  IMH_T                        message_header;
  boolean                      status;
  sys_sys_mode_e_type          sys_mode;
} mm_restore_psm_ctxt_rsp_T;


/* ---------------------------------------------------------------------
** Primitive  : EMM-RESTORE_BEARER_CTXT_RSP
** Description: NAS responds status of Bearer context restore request after restoring 
**              contexts for requested EPS bearer ID.
** -------------------------------------------------------------------*/

/*This structure is used for sending restore bearer ctxt rsp
** from NAS to DS
*/
typedef struct emm_restore_bearer_ctxt_rsp_s
{
  IMH_T                  message_header;
  eps_bearer_id_T        eps_bearer_id;
  byte                   connection_id;
} emm_restore_bearer_ctxt_rsp_s_type;


typedef struct mm_restore_pdp_ctxt_rsp
{
   IMH_T                      message_header;
   nsapi_T                    nsapi;
   byte                       connection_id;
} mm_restore_pdp_ctxt_rsp_s_type;


typedef struct mm_restore_pdp_ctxt_req {

  IMH_T                        message_header;
  nsapi_T                              nsapi;
    /**< NSAP information. */
  byte                                 sm_group_id;
    /**< Clients must check CM_API_SEC_PDP for the secondary PDP support group
         ID of the primary context that is associated with the secondary PDP
         context origination. */
         
  pdp_address_T                        pdp_addr;
    /**< Packet Data Protocol address. */
 
  apn_T                                apn_name;
    /**< Access Point Name -- A label or a fully-qualified domain name. */
 
  qos_T                                neg_qos;
    /**< Quality of Service information. */
 
  qos_T                               minimum_qos;
/**< Minimum Quality of Service required for the PDP context*/

  llc_sapi_T						   llc_sapi;
	  /**< Service Access Point Identifier (SAPI) Logical Link Control
		   (LLC). */

  sys_pdp_header_comp_e_type           h_comp;
    /**< SNDCP IP header compression required. */
 
  sys_pdp_data_comp_e_type             d_comp;
    /**< SNDCP data compression required. */
 
  bearer_ctrl_mode_T                   bcm;
 
  tft_type_T                           tft;
   /**<  Traffic Flow Template. */

  boolean                             is_primary_pdp;
  /**< Used to inform the client whether the call type is primary or
  secondary PDP.
       TRUE -- Primary_pdp.
       FALSE -- Secondry_pdp. */
       
} mm_restore_pdp_ctxt_req_s_type;


typedef struct emm_restore_bearer_ctxt_req_s{

  IMH_T                                message_header;
  eps_bearer_id_T                      eps_bearer_id;
    /**< Evolved Packet System bearer ID. */

  boolean                              default_bearer_ctxt;
    /**< Indicates if default or dedicated bearer*/

  sdf_qos_T                            sdf_qos;
    /**< Service Data Flow QoS. */

  gsm_umts_connection_id_T             ti;
    /**< ID used between GSM and UMTS to transfer call objects. */

  byte                                 sm_group_id;
    /**< Clients must check CM_API_SEC_PDP for the secondary PDP support group
         ID of the primary context that is associated with the secondary PDP
         context origination. */

  pdn_address_T                        pdn_address;
    /**< Packet Data Network address. */

  apn_T                                apn_name;
    /**< Access Point Name -- A label or a fully-qualified domain name. */

  qos_T                                neg_qos;
    /**< Quality of Service information. */

  llc_sapi_T                           neg_llc_sapi;
    /**< Service Access Point Identifier (SAPI) Logical Link Control
         (LLC). */

  apn_ambr_T                           apn_ambr;
    /**< Aggregate Maximum Bitrate (AMBR) for the APN. */

  byte                                 lbi;
  word                                 sdf_id;
    /**< Service Data Flow ID. */

  sys_pdp_header_comp_e_type           h_comp;
    /**< SNDCP IP header compression required. */

  sys_pdp_data_comp_e_type             d_comp;
    /**< SNDCP data compression required. */

  bearer_ctrl_mode_T                   bcm;
} emm_restore_bearer_ctxt_req_s_type;


/** EMM_ESM_DATA_TRANSPORT_REQ 
      @brief Request for lte data transport
*/
typedef struct emm_esm_data_transport_req_s
{
  IMH_T                        message_header;
  esm_header_T                 esm_hdr;
  
  void                        *dsm_item;
    /* DSM item containing the actual data to be trasported */
  
  eps_bearer_id_T              eps_bearer_id;
    /* EPS bearer ID for which data req is sent */    
  release_assistance_ind_type  release_assistance_ind;
    /* Release indication */

  boolean                      mo_exception_data;
    /**< pending MO exception data indicator */ 

  boolean                     empty_user_data_container;
    /**< indicates empty user data container */

  uint8                       seq_num;
    /**< indicates the sequence number of the data packet */
} emm_esm_data_transport_req_type;



/*! EMM_ACT_DEFAULT_BEARER_CONTEXT_REQUEST_IND
      @brief REQUIRED one-sentence brief description of this structure typedef
*/

typedef struct emm_act_default_bearer_context_request_ind_s
{
  IMH_T                          message_header;
  
  esm_header_T            esm_hdr;

  eps_bearer_id_T         eps_bearer_id;    
      /*!< Brief description of this member */

  sdf_qos_T               sdf_qos; 
      /*!< Brief description of this member */

  pdn_address_T           pdn_address; 
      /*!< Brief description of this member */

  apn_T                   apn_name; 
      /*!< Brief escription of this member */

  tft_type_T              ul_tft; 
      /*!< Brief description of this member */

  qos_T                   neg_qos; 
      /*!< Brief description of this member */

  llc_sapi_T              neg_llc_sapi; 
      /*!< Brief description of this member */

  protocol_cfg_options_T  protocol_config_options; 
      /*!< Brief description of this member */

  esm_cause_T             esm_cause; 

  apn_ambr_T              apn_ambr;

  gsm_umts_connection_id_T gsm_umts_connection_id;

  byte                    group_id;

  cp_only_ind_T              cp_only_opt;

  splmn_rate_ctrl_T       splmn_rate_ctrl;

  hc_config_T             hc_config;
     /* Header compression IE */
  ext_protocol_cfg_options_T  ext_protocol_conf_opt; /*!< dsm attachment containing the EPCO contents*/ 
} emm_act_default_bearer_context_request_ind_s_type;


/*! EMM_DEACT_BEARER_CONTEXT_REQUEST_IND
     @brief REQUIRED one-sentence brief description of this structure typedef
*/
typedef struct emm_deact_bearer_context_request_ind_s
{
  IMH_T                   message_header;

  esm_header_T            esm_hdr;

  eps_bearer_id_T         eps_bearer_id; 
    /*!< Brief description of this member */

  esm_cause_T             esm_cause; 
    /*!< Brief description of this member */

  protocol_cfg_options_T  protocol_config_options; 

    /*!< Brief description of this member */
  lte_nas_local_cause_T   esm_local_cause ;

  timer_3gpp_type3        backoff_timer;

  ext_protocol_cfg_options_T  ext_protocol_conf_opt;

} emm_deact_bearer_context_request_ind_s_type;


/*! EMM_ACT_DEDICATED_BEARER_CONTEXT_REQUEST_IND
      @brief REQUIRED one-sentence brief description of this structure typedef
*/
typedef struct emm_act_dedicated_bearer_context_request_ind_s
{
  IMH_T                   message_header;
  esm_header_T            esm_hdr;

  eps_bearer_id_T         eps_bearer_id;    
      /*!< Brief description of this member */

  sdf_qos_T               sdf_qos; 
      /*!< Brief description of this member */

  tft_type_T              tft; 
      /*!< Brief description of this member */

  qos_T                   neg_qos; 
      /*!< Brief description of this member */

  llc_sapi_T              neg_llc_sapi; 
      /*!< Brief description of this member */

  protocol_cfg_options_T  protocol_config_options; 
      /*!< Brief description of this member */

  gsm_umts_connection_id_T gsm_umts_connection_id;

  byte                    group_id;
  
  ext_protocol_cfg_options_T  ext_protocol_conf_opt;
}emm_act_dedicated_bearer_context_request_ind_s_type;


/*! EMM_MODIFY_BEARER_CONTEXT_REQUEST_IND
       @brief REQUIRED one-sentence brief description of this structure typedef
*/
typedef struct emm_modify_bearer_context_request_ind_s
{
  IMH_T                   message_header;
  esm_header_T            esm_hdr;

  eps_bearer_id_T         eps_bearer_id;    
      /*!< Brief description of this member */

  sdf_qos_T               sdf_qos; 
      /*!< Brief description of this member */

  tft_type_T              ul_dl_tft; 
      /*!< Brief description of this member */

  qos_T                   neg_qos; 
      /*!< Brief description of this member */

  llc_sapi_T              neg_llc_sapi; 
      /*!< Brief description of this member */

  protocol_cfg_options_T  protocol_config_options; 
      /*!< Brief description of this member */

  apn_ambr_T              apn_ambr;

  hc_config_T             hc_config;
     /* Header compression IE */

  ext_protocol_cfg_options_T  ext_protocol_conf_opt;  /*!< dsm attachment containing the EPCO contents*/

}emm_modify_bearer_context_request_ind_s_type;


/*! EMM_BEARER_RESOURCE_ALLOC_FAILURE_IND
      @brief REQUIRED one-sentence brief description of this structure typedef
*/
typedef struct emm_bearer_resource_alloc_failure_ind_s
{
  IMH_T                   message_header;
  esm_header_T            esm_hdr;

  boolean                 result;
//esm_cause_T esm_cause;
} emm_bearer_resource_alloc_failure_ind_s_type;


/*! EMM_BEARER_RESOURCE_ALLOC_REJECT_IND
      @brief REQUIRED one-sentence brief description of this structure typedef
*/
typedef struct emm_bearer_resource_alloc_reject_ind_s
{
  IMH_T                   message_header;
  esm_header_T            esm_hdr;
  esm_cause_T             esm_cause;
  lte_nas_local_cause_T   esm_local_cause;
  protocol_cfg_options_T  protocol_config_options;
  timer_3gpp_type3        backoff_timer;
  reattempt_ind_T         reattempt_indicator;
  ext_protocol_cfg_options_T  ext_protocol_conf_opt;  /*!< dsm attachment containing the EPCO contents*/
} emm_bearer_resource_alloc_reject_ind_s_type;



/*! @brief REQUIRED one-sentence brief description of this structure typedef
*/
typedef emm_bearer_resource_alloc_reject_ind_s_type emm_bearer_context_modify_reject_ind_s_type;

 /*! EMM_ACT_DEFAULT_BEARER_CONTEXT_ACCEPT
      @brief REQUIRED one-sentence brief description of this structure typedef
*/

typedef struct 
{
  IMH_T                     message_header;
  esm_header_T              esm_hdr;
  eps_bearer_id_T           eps_bearer_id; /*!< Brief description of this member */
  /* Indicates if it is emergency bearer */
  boolean                   emc_bc;
  protocol_cfg_options_T    protocol_config_options; /*!< Brief description of this member */
  sys_pdp_header_comp_e_type      h_comp;
  sys_pdp_data_comp_e_type        d_comp;
  ext_protocol_cfg_options_T  ext_protocol_conf_opt;
} emm_act_default_bearer_context_accept_T;



/*! EMM_ACT_DEFAULT_BEARER_CONTEXT_REJECT
      @brief REQUIRED one-sentence brief description of this structure typedef
*/
typedef struct 
{
  IMH_T                     message_header;
  esm_header_T              esm_hdr;
  eps_bearer_id_T           eps_bearer_id; /*!< Brief description of this member */
  esm_cause_T               esm_cause; /*!< Brief description of this member */
  lte_nas_local_cause_T      esm_local_cause; /*!< ESM local cause value for the message */
  protocol_cfg_options_T    protocol_config_options; /*!< Brief description of this member */
  /* Indicates if it is emergency bearer */
  boolean                    emc_bc;
  ext_protocol_cfg_options_T  ext_protocol_conf_opt;
} emm_act_default_bearer_context_rej_T;



/*! EMM_ACT_DEDICATED_BEARER_CONTEXT_ACCEPT
      @brief REQUIRED one-sentence brief description of this structure typedef
*/
typedef struct 
{
  IMH_T                       message_header;
  esm_header_T                esm_hdr;
  eps_bearer_id_T             eps_bearer_id; /*!< Brief description of this member */
  /* Indicates if it is emergency bearer */
  boolean                     emc_bc;
  protocol_cfg_options_T      protocol_config_options; /*!< Brief description of this member */
  sys_pdp_header_comp_e_type      h_comp;
  sys_pdp_data_comp_e_type        d_comp;
  ext_protocol_cfg_options_T  ext_protocol_conf_opt;
} emm_act_dedicated_bearer_context_accept_T; 


/*! EMM_ACT_DEDICATED_BEARER_CONTEXT_REJECT
      @brief REQUIRED one-sentence brief description of this structure typedef
*/
typedef struct 
{
  IMH_T                      message_header;
  esm_header_T               esm_hdr;
  eps_bearer_id_T            eps_bearer_id; /*!< Brief description of this member */
  esm_cause_T                esm_cause;
  lte_nas_local_cause_T      esm_local_cause; /*!< ESM local cause value for the message */
  protocol_cfg_options_T     protocol_config_options; /*!< Brief description of this member */
  ext_protocol_cfg_options_T  ext_protocol_conf_opt;
} emm_act_dedicated_bearer_context_rej_T;


/*! EMM_MODIFY_BEARER_CONTEXT_REJECT
      @brief REQUIRED one-sentence brief description of this structure typedef
*/
typedef struct 
{
  IMH_T                     message_header;
  esm_header_T              esm_hdr;
  eps_bearer_id_T           eps_bearer_id; /*!< Brief description of this member */
  esm_cause_T               esm_cause; /*!< Brief description of this member */
  lte_nas_local_cause_T     esm_local_cause; /*!< ESM local cause value for the message */
  protocol_cfg_options_T    protocol_config_options; /*!< Brief description of this member */
  ext_protocol_cfg_options_T  ext_protocol_conf_opt;
} emm_modify_bearer_context_rej_T;



/*! EMM_MODIFY_BEARER_CONTEXT_ACCEPT
      @brief REQUIRED one-sentence brief description of this structure typedef
*/
typedef struct 
{
  IMH_T                     message_header;
  esm_header_T              esm_hdr;
  eps_bearer_id_T           eps_bearer_id; /*!< Brief description of this member */
  protocol_cfg_options_T    protocol_config_options; /*!< Brief description of this member */
  ext_protocol_cfg_options_T  ext_protocol_conf_opt;
} emm_modify_bearer_context_accept_T;

/*! EMM_BEARER_RESOURCE_MODIFICATION_REQ
      @brief REQUIRED This struct represents the bearer resource allocation request
      Not Applicable
*/
typedef struct 
{
  IMH_T                     message_header;
  esm_header_T              esm_hdr;

  sdf_qos_T                 sdf_qos; /*!< Brief description of this member */
  tft_type_T                tad; /*!< Brief description of this member */
  esm_cause_T               esm_cause;
  protocol_cfg_options_T    protocol_config_options; /*!< Brief description of this member */
  lte_nas_local_cause_T     esm_local_cause;  

  boolean                   low_priority_signaling;  /*!< NAS signaling low priority indicator */

  hc_config_T               hc_config;
  /**< Contains the header compression related info */
  ext_protocol_cfg_options_T  ext_protocol_conf_opt;
} emm_bearer_resource_modification_req_T ;


/*! EMM_DRB_RELEASED_IND
      @brief REQUIRED one-sentence brief description of this structure typedef
*/
typedef struct emm_drb_released_ind_s
{
  IMH_T                   message_header;
  esm_header_T            esm_hdr;

  eps_bearer_id_T         eps_bearer_id ; 
    /*!< Brief description of this member */

  esm_cause_T             esm_cause ;
  lte_nas_local_cause_T   esm_local_cause ;

} emm_drb_released_ind_s_type;


/*! EMM_DRB_SETUP_IND
      @brief REQUIRED one-sentence brief description of this structure typedef
*/
typedef struct emm_drb_setup_ind_s
{
  IMH_T                   message_header;
  esm_header_T            esm_hdr;

  eps_bearer_id_T         eps_bearer_id ; 
    /*!< Brief description of this member */
  byte                    rb_id;
} emm_drb_setup_ind_s_type;


/*! EMM_DRB_REESTABLISH_REJECT_IND
      @brief REQUIRED one-sentence brief description of this structure typedef
*/
typedef struct emm_drb_reestablish_failed_ind_s
{
  IMH_T                   message_header;
  esm_header_T            esm_hdr;
  eps_bearer_id_T         eps_bearer_id ; 
    /*!< Brief description of this member */
  lte_nas_local_cause_T   esm_local_cause ;
  emm_failure_type        emm_failure_cause;

} emm_drb_reestablish_failed_ind_s_type;

typedef emm_drb_reestablish_failed_ind_s_type emm_drb_reestablish_reject_ind_s_type;


/*! EMM_PDN_CONNECTIVITY_REJECT_IND
      @brief REQUIRED one-sentence brief description of this structure typedef
*/
typedef struct emm_pdn_connectivity_reject_ind_s
{
  IMH_T                   message_header;
  esm_header_T            esm_hdr;

  esm_cause_T             esm_cause;
  lte_nas_local_cause_T   esm_local_cause;
  protocol_cfg_options_T  protocol_config_options;
  timer_3gpp_type3               backoff_timer;
  pdn_reject_ind_params_s_type   pdn_reject_ind;

  emm_failure_type        emm_failure_cause;
  boolean                 emc_type;
  reattempt_ind_T         reattempt_indicator;
  ext_protocol_cfg_options_T  ext_protocol_conf_opt; /*!< dsm attachment containing the EPCO contents*/
} emm_pdn_connectivity_reject_ind_s_type;


/*! EMM_PDN_CONNECTIVITY_FAILURE_IND
      @brief REQUIRED one-sentence brief description of this structure typedef
*/
typedef struct emm_pdn_connectivity_failure_ind_s
{
  IMH_T                   message_header;
  esm_header_T            esm_hdr;

  boolean                 result;
    /*!< Brief description of this member */

} emm_pdn_connectivity_failure_ind_s_type;


/*! EMM_PDN_DISCONNECT_REJECT_IND
      @brief REQUIRED one-sentence brief description of this structure typedef
*/
typedef struct emm_pdn_disconnect_reject_ind_s
{
  IMH_T                   message_header;
  esm_header_T            esm_hdr;
  eps_bearer_id_T         eps_bearer_id ;
  esm_cause_T             esm_cause ;
  lte_nas_local_cause_T   esm_local_cause ;
  protocol_cfg_options_T  protocol_config_options;
  ext_protocol_cfg_options_T  ext_protocol_conf_opt; /*!< dsm attachment containing the EPCO contents*/

} emm_pdn_disconnect_reject_ind_s_type;


 /*! EMM_PDN_DISCONNECT_REJECT_IND
      @brief REQUIRED one-sentence brief description of this structure typedef
*/
typedef struct emm_get_pdn_connectivity_request_ind_s
{
  IMH_T                   message_header;
  uint8                   seq_num;

  boolean                 responding_to_page;
  
  boolean attach_without_pdn_supported;
  
} emm_get_pdn_connectivity_request_ind_s_type;




/* EMM_NOTIFICATION_IND : This structure is used for sending ESM SRVCC cancel notification
** From NAS to DS
*/
typedef struct
{
  IMH_T                          message_header;
  esm_notification_ind_e_type    notification_ind;    /* notificationID */
} emm_notification_ind_s_type;


 
/** EMM_PDN_DISCONNECT_REQ 
       @brief Request for PDN disconnect.
*/
typedef struct
{
  IMH_T                   message_header;
  void                    *dsm_item;
  eps_bearer_id_T         eps_bearer_id;
} emm_data_transport_ind_type;


/** EMM_PDN_DISCONNECT_REQ 
       @brief Request for PDN disconnect.
*/


typedef struct
{
  IMH_T                   message_header;
  emm_failure_type        emm_failure_cause;
  boolean                 status; /* TRUE success, otherwise FALSE*/
  uint8                   seq_num;
}emm_data_transport_cnf_type;

 
/** EMM_PLMN_CHANGE_IND 
       @brief Request for PLMN change indication
*/
/* PLMN change ind */
typedef struct
{
  IMH_T               message_header;
  /* New PLMN */
  sys_plmn_id_s_type  plmn;
}emm_plmn_change_ind_type;


typedef struct
{
	IMH_T				message_header;
	/* New PLMN */
	emm_timer_t3448_status_e_type	status;

	dword                           timer_t3448_value;
}emm_timer_t3448_status_ind_type;

typedef struct
{
    IMH_T                message_header;
	
    emm_timer_barring_status_e_type		status;
	
    emm_timer_barring_status_category_e_type		barring_category; 
}emm_timer_barring_status_ind_type;

/** EMM_RESET_APN_SWITCH_IND 
       @brief Request for APN reset indication.
*/
typedef struct
{
 IMH_T                          message_header;
}emm_reset_apn_switch_ind_type;


/** EMM_ATTACH_COMPLETE_IND 
       @brief Request for attach Complete indication
*/

typedef struct
{
  IMH_T   message_header;
  boolean attach_without_pdn;
}emm_attach_complete_ind_type;


 
/** EMM_ESM_DETACH_IND 
       @brief Request for detach indication.
*/
typedef struct
{
  IMH_T                          message_header;
}emm_esm_detach_ind_type;


 
/** EMM_TAU_COMPLETE_IND 
       @brief Request for TAU Complete indication
*/
typedef struct
{
  IMH_T                         message_header;
  sys_plmn_id_s_type            plmn;
  sys_lac_type                  tac;
  boolean                       hc_config_status_exists;
  uint16                        hc_config_status;
  nas_tau_type					tau_type;
}emm_tau_complete_ind_type;

 
/** EMM_T3402_CHANGED_IND 
       @brief Request for T3402 change ind.
*/
typedef struct emm_ds_t3402_changed_ind_s
{
  IMH_T  message_header;
  /* T3402 value */  
  dword  t3402_value;

} emm_ds_t3402_changed_ind_s_type;

 
/** EMM_DEACT_NON_EMC_BEARER_IND 
      Deactivate non emergency bearer ind
*/
typedef struct emm_deact_non_emc_bearer_ind_s
{
  IMH_T                          message_header;
} emm_deact_non_emc_bearer_ind_s_type;

typedef struct emm_detach_ind_s
{
   IMH_T                         message_header;
}emm_detach_ind_type;


typedef struct emm_process_pending_mo_exception_data
{
  IMH_T                   message_header;
 boolean                is_mo_exception_to_be_cleared;
} emm_process_pending_mo_exception_data_s_type;


typedef struct emm_clean_up_mo_exception_data
{
  IMH_T                   message_header;
} emm_clean_up_mo_exception_data_s_type; 
/* DS- ATCOP/QMI-NAS interface for phone command */

typedef struct
{
 	IMH_T                message_header;
 	emm_cpsr_status      status;
}
emm_cpsr_status_ind_type;

/*************************************************************/
/* MM_SET_EDRX_CFG_REQ                                       */
/* Primitive to set/change EDRX configuration params such as */
/* eDRX enabled/disabled, paging time window, cycle length   */
/*************************************************************/
typedef struct mm_cmd_set_edrx_cfg_req_s
{
  sys_radio_access_tech_e_type rat_type;
  boolean                      edrx_enabled;
  uint8                        edrx_ptw;
  uint8                        edrx_cycle_length;
  uint16                       change_mask;
}mm_cmd_set_edrx_cfg_req_s;


/******************************************************************/
/* MM_SET_EDRX_CFG_RSP                                            */
/* Primitive to acknowledge that NAS received MM_SET_EDRX_CFG_REQ */
/******************************************************************/
typedef struct mm_cmd_set_edrx_cfg_rsp_s
{
   msgr_hdr_s                                     msg_hdr;
}mm_cmd_set_edrx_cfg_rsp_s;


/********************************************************************/
/* MM_GET_EDRX_PARAMS_REQ                                           */
/* Primitive to get eDRX paging time window & cycle length from NAS */
/********************************************************************/
typedef struct mm_cmd_get_edrx_params_req_s
{
  sys_radio_access_tech_e_type rat_type;
}mm_cmd_get_edrx_params_req_s;


/********************************************************************/
/* MM_GET_EDRX_PARAMS_RSP                                           */
/* Primitive to send eDRX paging time window & cycle length to CM   */
/********************************************************************/
typedef struct mm_cmd_get_edrx_params_rsp_s
{
  msgr_hdr_s                   msg_hdr;
  sys_radio_access_tech_e_type rat_type;
  boolean                      edrx_enabled;
  uint8                        edrx_ptw;
  uint8                        edrx_cycle_length;
}mm_cmd_get_edrx_params_rsp_s;



/******************************************************************************/
/*                                                                            */
/*  MM_SET_DRX_REQ                                                       */
/*                                                                            */
/******************************************************************************/
typedef struct mm_cmd_set_drx_req_s
{
  sys_drx_cn_coefficient_s1_e_type                    drx_coefficient;
} mm_cmd_set_drx_req_s;


/******************************************************************************/
/*                                                                            */
/*  MM_CMD-GET_DRX_REQ                                            */
/*                                                                            */
/******************************************************************************/

//typedef struct mm_cmd_get_drx_req_s
//{
//} mm_cmd_get_drx_req_s;


/******************************************************************************/
/*                                                                            */
/*  MM_SET_DRX_CNF                                               */
/*                                                                            */
/******************************************************************************/
typedef struct mm_cmd_set_drx_cnf_s
{
   msgr_hdr_s                                     msg_hdr;
   boolean                                         result; 
} mm_cmd_set_drx_cnf_s;

/******************************************************************************/
/*                                                                            */
/*  MM_GET_DRX_CNF                                               */
/*                                                                            */
/******************************************************************************/
typedef struct mm_cmd_get_drx_cnf_s
{
  msgr_hdr_s                     msg_hdr;
  sys_drx_cn_coefficient_s1_e_type                    drx_coefficient;
} mm_cmd_get_drx_cnf_s;


/******************************************************************************/
/*                                                                            */
/*  MM_SET_HPLMN_SEARCH_TIMER_REQ                                            */
/*                                                                            */
/******************************************************************************/
typedef struct mm_cmd_set_hplmn_search_timer_req_s
{
  /*  
  ** Value is in minutes
  **  Time value  = 0xFFFFFFFF means use SIM defined timer
  **/
  uint32                                             timer_value; 

} mm_cmd_set_hplmn_search_timer_req_s;

/*! MM_WWAN_TECH_STATUS_IND
      @brief REQUIRED one-sentence brief description of this structure typedef
*/
typedef struct mm_wwan_tech_status_ind_s
{
  IMH_T                   message_header;

  boolean                 is_suspend; 
    /*! < WWAN = GPS/NB/CATM tech is LOADED <FALSE> 
              or UNLOADED <TRUE> */
} mm_wwan_tech_status_ind_s_type;


/* ! EMM_MO_EXCEPTION_DATA_IND
      @brief REQUIRED one-sentence brief description of this structure typedef
*/
typedef struct emm_mo_exception_data_ind_s
{

  IMH_T                        message_header;
  boolean                      mo_exception_data;
      /**< pending MO exception data indicator */
} emm_mo_exception_data_ind_s_type;


enum
{
  MSGR_DEFINE_UMID(NAS, EMM, RSP, SET_EDRX_CFG,               0x01, 
                                                    mm_cmd_set_edrx_cfg_rsp_s),
  MSGR_DEFINE_UMID(NAS, EMM, RSP, GET_EDRX_PARAMS,            0x02, 
                                                    mm_cmd_get_edrx_params_rsp_s),
  MSGR_DEFINE_UMID(NAS, EMM, CNF, SET_DRX,                    0x03, 
                                                    mm_cmd_set_drx_cnf_s),
  MSGR_DEFINE_UMID(NAS, EMM, CNF, GET_DRX,                    0x04, 
                                                    mm_cmd_get_drx_cnf_s),
};




/*===========================================================================
FUNCTION nas_mm_cmd_set_edrx_req

DESCRIPTION

  Queue MM_SET_EDRX_REQ command to NAS.

DEPENDENCIES
  None

RETURN VALUE

  TRUE
  Command was queued succesfully to NAS

  FALSE
  No NAS buffer available to queue the command or incorrect parameters
  passed into the function

SIDE EFFECTS
  None

===========================================================================*/

extern boolean nas_mm_cmd_set_edrx_req(

  mm_cmd_set_edrx_cfg_req_s              *set_edrx_info_ptr
);


/*===========================================================================
FUNCTION nas_mm_cmd_set_edrx_req

DESCRIPTION

  Queue MM_GET_EDRX_REQ command to NAS.

DEPENDENCIES
  None

RETURN VALUE

  TRUE
  Command was queued succesfully to NAS

  FALSE
  No NAS buffer available to queue the command or incorrect parameters
  passed into the function

SIDE EFFECTS
  None

===========================================================================*/

extern boolean nas_mm_cmd_get_edrx_req(

  mm_cmd_get_edrx_params_req_s              *get_edrx_info_ptr
);


/*===========================================================================
FUNCTION nas_mm_cmd_set_drx_req

DESCRIPTION

  Queue MM_SET_DRX_REQ command to NAS.

DEPENDENCIES
  None

RETURN VALUE

  TRUE
  Command was queued succesfully to NAS

  FALSE
  No NAS buffer available to queue the command or incorrect parameters
  passed into the function

SIDE EFFECTS
  None

===========================================================================*/

extern boolean nas_mm_cmd_set_drx_req(

  mm_cmd_set_drx_req_s              *set_drx_info_ptr
);



/*===========================================================================
FUNCTION nas_mm_cmd_get_drx_req

DESCRIPTION

  Queue MM_SET_EDRX_REQ command to NAS.

DEPENDENCIES
  None

RETURN VALUE

  TRUE
  Command was queued succesfully to NAS

  FALSE
  No NAS buffer available to queue the command or incorrect parameters
  passed into the function

SIDE EFFECTS
  None

===========================================================================*/

extern boolean nas_mm_cmd_get_drx_req(
  void
);



/*===========================================================================
FUNCTION nas_reg_cmd_set_hplmn_search_timer

DESCRIPTION

  Queue MM_SET_HPLMN_SEARCH_REQ command to REG queue.

DEPENDENCIES
  None

RETURN VALUE

  TRUE
  Command was queued succesfully to NAS

  FALSE
  No NAS buffer available to queue the command or incorrect parameters
  passed into the function

SIDE EFFECTS
  None

===========================================================================*/

extern boolean nas_reg_cmd_set_hplmn_search_timer(

  mm_cmd_set_hplmn_search_timer_req_s              *set_hplmn_search_info_ptr
);

#ifdef FEATURE_LTE_REL14
/*===========================================================================
FUNCTION ds3gpp_check_pending_data_for_cpsr

DESCRIPTION

  Function to check whether there is any data pending in DS during paging.

DEPENDENCIES
  None

RETURN VALUE

  TRUE
  NAS will wait for DS to trigger ESM data transport

  FALSE
  NAS will start CPSR with Null ESM container.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean  ds3gpp_check_pending_data_for_cpsr(void);
#endif


/*===========================================================================
===========================================================================*/

extern boolean emm_search_tai_list
(
  /* Provided TAI */
  lte_nas_tai_lst1_type *provided_tai,
  /* Stored TAI list */
  lte_nas_emm_tai_lst_info_type *tai_list
);

/*===========================================================================
FUNCTION nas_cmd_get_hplmn_search_timer

DESCRIPTION

  Function to retrieve the update the HPLMN timer value

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/

extern uint32 nas_cmd_get_hplmn_search_timer(void);

#ifdef FEATURE_CIOT
/*===========================================================================
FUNCTION nas_cmd_get_ciot_irat_search_timer

DESCRIPTION

  Function to retrieve ciot irat search timer value

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
extern uint32 nas_cmd_get_ciot_irat_search_timer(void);
#endif

/* EMM to DS Outgoing command structure */

typedef struct ds_emm_cmd
{
   q_link_type        link; 
      /*!< Queue link. This must be the first element of the cmd_type. 
         MSGR expects q_link to be the first element and the msgr_hdr_s 
         as the second element.So, after link_type no element should be 
         added and the only element is the union below
      */
   union
   {
       IMH_T       hdr; /*!< Message router header */

          /*! @brief Messages sent by the NAS to DS. */ 

        emm_act_default_bearer_context_request_ind_s_type    emm_act_default_bearer_context_request_ind;
        emm_act_dedicated_bearer_context_request_ind_s_type  emm_act_dedicated_bearer_context_request_ind;
        emm_deact_bearer_context_request_ind_s_type          emm_deact_bearer_context_request_ind;
        emm_bearer_resource_alloc_failure_ind_s_type         emm_bearer_resource_alloc_failure_ind;
        emm_bearer_resource_alloc_reject_ind_s_type          emm_bearer_resource_alloc_reject_ind;
        emm_modify_bearer_context_request_ind_s_type         emm_modify_bearer_context_request_ind;
        emm_pdn_connectivity_reject_ind_s_type               emm_pdn_connectivity_reject_ind;
        emm_pdn_connectivity_failure_ind_s_type              emm_pdn_connectivity_failure_ind;
        emm_pdn_disconnect_reject_ind_s_type                 emm_pdn_disconnect_reject_ind;
        emm_drb_released_ind_s_type                          emm_act_drb_released_ind;
        emm_drb_setup_ind_s_type                             emm_drb_setup_ind;
        emm_drb_reestablish_failed_ind_s_type                emm_drb_reestablish_failed_ind;
        emm_drb_reestablish_reject_ind_s_type                emm_drb_reestablish_reject_ind;
        emm_get_pdn_connectivity_request_ind_s_type          emm_get_pdn_connectivity_request_ind;
        emm_bearer_context_modify_reject_ind_s_type          emm_bearer_context_modify_reject_ind;
        //emm_pdn_connectivity_packed_request_ind_s_type       emm_pdn_connectivity_packed_request_ind;
        emm_notification_ind_s_type                          emm_notify_ind;
        emm_data_transport_ind_type                          emm_data_transport_ind; 
        emm_data_transport_cnf_type                          emm_data_transport_cnf;

        emm_plmn_change_ind_type                             emm_plmn_change_ind;
        emm_reset_apn_switch_ind_type                        emm_reset_apn_switch_ind;
        emm_attach_complete_ind_type                         emm_attach_complete_ind;
        emm_detach_ind_type                                  emm_detach_ind;
        emm_tau_complete_ind_type                            emm_tau_complete_ind;
        emm_ds_t3402_changed_ind_s_type                      emm_t3402_changed_ind;
		emm_timer_t3448_status_ind_type						 emm_timer_t3448_status_ind;
        emm_deact_non_emc_bearer_ind_s_type                  emm_deact_non_emc_bearer_ind;
        emm_bearer_ctxt_transfer_ind_type                    emm_bearer_ctxt_transfer_ind;
		emm_timer_barring_status_ind_type                    emm_timer_barring_status_ind;
		emm_cpsr_status_ind_type							 emm_cpsr_status_ind;
		
        mm_pdp_activate_cnf_T                                pdp_activate_cnf;
        mm_pdp_activate_rej_T                                pdp_activate_rej;
        mm_pdp_activate_ind_T                                pdp_activate_ind;
        mm_pdp_deactivate_cnf_T                              pdp_deact_cnf;
        mm_pdp_deactivate_ind_T                              pdp_deact_ind;
        mm_pdp_modify_ind_T                                  pdp_modify_ind;
        mm_pdp_modify_cnf_T                                  pdp_modify_cnf;
        mm_pdp_modify_rej_T                                  pdp_modify_rej;
        mm_pdp_deactivate_rej_T                              pdp_deact_rej;
        mm_pdp_activate_sec_cnf_T                            pdp_act_sec_conf;
        mm_pdp_activate_sec_rej_T                            pdp_act_sec_rej;
        mm_pdp_activate_sec_ind_T                            pdp_act_sec_ind;  //Added
        mm_pdp_nw_modify_req_T                               pdp_nw_modify_req; //Added
        mm_pdp_deact_promote_ind_T                           pdp_deact_prom_ind;
        mm_pdp_deact_promote_cnf_T                           pdp_deact_prom_cnf;
        mm_restore_psm_ctxt_req_T                            mm_restore_psm_ctxt_req;
        emm_restore_bearer_ctxt_rsp_s_type                   emm_restore_bearer_ctxt_rsp;       
        mm_restore_pdp_ctxt_rsp_s_type                       mm_restore_pdp_ctxt_rsp;
        mm_unblock_all_apn_ind_T                             mm_unblock_all_apn;
        mm_wwan_tech_status_ind_s_type                       mm_wwan_tech_status_ind;

   }cmd;
  /*! @brief List all the incoming messages/commands*/ 
} ds_emm_cmd_type;

#endif /* _EMM_EXT_MSG_H_ */



