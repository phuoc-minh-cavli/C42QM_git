/******************************************************************************

  @file    iotapp_qmi_internal.h
  @brief   QMI Interface header file for all service

  ---------------------------------------------------------------------------
  Copyright (c) 2017-2021 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------

******************************************************************************/

 
#ifndef QMI_INTERNAL_H
#define QMI_INTERNAL_H


/** Header files */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stringl/stringl.h>
#include "qapi_device_info.h"
#include "wireless_data_service_v01.h"
#include "qurt_thread.h"
#include "qurt_signal.h"



#define EFAILURE 	-1
#define ESUCCESS	 0
#define SEND_TIMEOUT 	50000
#define SEND_CELL_SCAN_TIMEOUT  500000
#define PLMN_SCAN_TIMEOUT  3600000

/** Macro for  Network Bearer values*/
#define LTE_TDD         5
#define LTE_FDD         6
#define GSM_NW_BEARER         0
#define WCDMA_NW_BEARER         2

/** Macro for Roaming status */
#define ROAM_STATUS "RoamStatus"
/** Macro for MNC Unused bytes */
#define MNC_UNUSED_BYTE 0xFF
/** Max Length for IMSI */
#define MAX_IMSI_LEN    15

#define PARAM_CELL_ID                   "Base Station ID"   // to get cell id information
#define PARAM_MCC                       "MCC"       // to get mcc information
#define PARAM_MNC                       "MNC"       // to get mnc information
#define PARAM_SIGNAL_STRENGTH           "4G Signal Strength"  // to get the signal strength information
#define IOTAPP_QMI_WDS_APN_NAME_MAX_V01 150
#define IOTAPP_QMI_WDS_USER_NAME_MAX_V01 127
#define IOTAPP_QMI_WDS_PASSWORD_MAX_V01  127

/* Indication ID's */
#define QAPI_DEVICE_INFO_ANY_IND                0x01
#define QAPI_DEVICE_INFO_NETWORK_IND            0x02
#define QAPI_DEVICE_INFO_NETWORK_BEARER_IND     0x04
#define QAPI_DEVICE_INFO_MCC_IND                0x08
#define QAPI_DEVICE_INFO_MNC_IND                0x10
#define QAPI_DEVICE_INFO_CELL_ID_IND            0x20
#define QAPI_DEVICE_INFO_4G_SIG_STRENGTH_IND    0x40
#define QAPI_DEVICE_INFO_LINK_QUALITY_IND       0x80
#define QAPI_DEVICE_INFO_TX_BYTES_IND          0x100
#define QAPI_DEVICE_INFO_RX_BYTES_IND          0x200
#define QAPI_DEVICE_INFO_ROAMING_IND           0x400
#define QAPI_DEVICE_INFO_LTE_OP_MODE_IND       0x800
#define QAPI_DEVICE_INFO_SIM_STATE_IND        0x1000
#define QAPI_DEVICE_INFO_EDRX_IND             0x2000
#define QAPI_DEVICE_INFO_JAMMER_IND           0x4000
#define QAPI_DEVICE_INFO_ACTIVE_TIMER_IND     0x8000
#define QAPI_DEVICE_INFO_PSM_TIMER_IND       0x10000
#define QAPI_DEVICE_INFO_WMS_EVENT_IND       0x20000
#define QAPI_DEVICE_INFO_IMS_IOT_REGISTRATION_STATUS_IND 0x40000
#define QAPI_DEVICE_INFO_VOICE_ALL_CALL_STATUS_IND       0x80000
#define QAPI_DEVICE_INFO_MODEM_SERVICE_STATE_IND         0x100000
#define QAPI_DEVICE_INFO_WWAN_SLEEP_INFO_IND             0x200000
#define QAPI_DEVICE_INFO_INCOMING_SMS_IND                0x400000
#define QAPI_DEVICE_INFO_ROUTE_INCOMING_SMS_IND          0x800000
#define QAPI_DEVICE_INFO_SEND_SMS_RSP_IND                0x1000000
#define QAPI_DEVICE_INFO_SMS_STATUS_REPORT_IND           0x2000000

extern char* devInfoMallocError;
extern char* qmi_syn_failure;


#define NAS_SERVICE_SIGNAL_MASK      0x00000001



typedef qurt_thread_t device_info_pthread_t;
typedef qurt_signal_t device_info_cond_t;

/* Signaling related macros  */
#define DEVICE_INFO_INIT_COND(cond)          qurt_signal_init(&cond)
#define DEVICE_INFO_WAIT_COND(cond, mask)    qurt_signal_wait(&cond, mask, QURT_SIGNAL_ATTR_WAIT_ALL)
#define DEVICE_INFO_SIGNAL_COND(cond, mask)  qurt_signal_set(&cond, mask)   
#define DEVICE_INFO_DESTROY_COND(cond)       qurt_signal_destroy(&cond)   




typedef enum {
  IOTAPP_NO_SVC = 0,
  IOTAPP_LTE = 1,
  IOTAPP_GSM,
  IOTAPP_WCDMA
} nw_srvc_type;

typedef enum {
  IOTAPP_LTE_NO_SVC = 0,
  IOTAPP_LTE_TDD = 5,
  IOTAPP_LTE_FDD,
  IOTAPP_LTE_NB_IOT
} lte_mode_pref;

typedef enum {
  IOTAPP_CIOT_MODE_NO_SVC = 0,
  IOTAPP_CIOT_MODE_CATM1,
  IOTAPP_CIOT_MODE_NB1
} lte_ciot_mode;

typedef enum {
  IOTAPP_WDS_PDP_TYPE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  IOTAPP_WDS_PDP_TYPE_PDP_IPV4_V01 = 0x00, /**<  PDP-IP (IPv4) \n  */
  IOTAPP_WDS_PDP_TYPE_PDP_PPP_V01 = 0x01, /**<  PDP-PPP \n  */
  IOTAPP_WDS_PDP_TYPE_PDP_IPV6_V01 = 0x02, /**<  PDP-IPv6 \n  */
  IOTAPP_WDS_PDP_TYPE_PDP_IPV4V6_V01 = 0x03, /**<  PDP-IPv4 and IPv6 \n  */
  IOTAPP_WDS_PDP_TYPE_PDP_NON_IP_V01 = 0x04, /**<  PDP-NON IP   */
  IOTAPP_WDS_PDP_TYPE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}iotapp_wds_pdp_type_enum_v01;

typedef struct {

  nw_srvc_type network_type;

  lte_mode_pref lte_mode;

  lte_ciot_mode ciot_mode;

} network_bearer_t;

/**
 * This enum defines the signals exchanges between QMI_WMS and M2M
 */
/*enum wms_signal_events
{
  WMS_MT_MESSAGE_IND = 0x10000
};*/

/*
 * This enum defines the different types of storage types
 */
typedef enum {
  WMS_STORAGE_TYPE_ENUM_MIN_ENUM_VAL = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  WMS_STORAGE_TYPE_UIM = 0x00, 
  WMS_STORAGE_TYPE_NV1 = 0x01, 
  WMS_STORAGE_TYPE_NONE = -1, 
  WMS_STORAGE_TYPE_ENUM_MAX_ENUM_VAL = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}wms_storage_type_enum;

typedef enum {
  PERSO_PF  = 0, /**< lock Phone to the very First inserted SIM/UICC card */
  PERSO_PN,      /**< Network personalization */
  PERSO_PU,      /**< network sUbset personalization */
  PERSO_PP,      /**< service Provider personalization */
  PERSO_PC,      /**< Corporate personalization */  	
}uim_personalize_type_enum;

/* 
 * This struct stores mt message details related to storage 
 */

typedef struct {

  wms_storage_type_enum storage_type;

  uint32_t storage_index;
  /**<   MT message index. */
}wms_mt_message_type_t;  /* Type */

typedef struct 
{
  int buf[MAX_LEN_VAL];
  uint32_t len;
}dev_info;

struct c_band
{
  uint64_t gsm_band;
  uint64_t m1_band;
  uint64_t nb1_band;
};
typedef struct c_band cell_info_band;


/* Service type */ 
extern network_bearer_t srvc_type;

/* WDS profile setting parameters bitmask */

#define WDS_PROFILE_APN_NAME                               0x1
/** APN name */
#define WDS_PROFILE_TYPE                                   0x2
/** Home PDP type */
#define WDS_PROFILE_CONTEXT_H_COMP                         0x4
/** PDP header compression support */
#define WDS_PROFILE_CONTEXT_D_COMP                         0x8
/** PDP data compression support */
#define WDS_PROFILE_DNS_ADDR_V4_PRIMARY                    0x10
/** Primary DNS IPV6 address */
#define WDS_PROFILE_DNS_ADDR_V4_SECONDARY                  0x20
/** Secondary DNS IPV4 address */
#define WDS_PROFILE_AUTH_USERNAME                          0x40
/** User name */
#define WDS_PROFILE_AUTH_PASSWORD                          0x80
/** password */
#define WDS_PROFILE_AUTH_TYPE                              0x100
/** Aunthentication Preferencre */
#define WDS_PROFILE_DNS_ADDR_V6_PRIMARY                    0x200
/** Priimary DNS IPV6 address */
#define WDS_PROFILE_DNS_ADDR_V6_SECONDARY                  0x400
/** Secondary DNS IPV6 address */
#define WDS_PROFILE_PCFCH_ADDRESS_USING_DHCP_FLAG          0x800
/* PCFCH address using dhcp */
#define WDS_PROFILE_ACCESS_CTRL_FLAG                       0x1000
/**Acess control flag */
#define WDS_PROFILE_IM_CN_FLAG                             0x2000
/** im_cn_flag */
#define WDS_PROFILE_APN_DISABLE_FLAG                       0x4000
/** APN disable flag */
#define WDS_PROFILE_PDN_INACTIVITY_TIMEOUT                 0x8000
/** PDN inactivity time out */
#define WDS_PROFILE_APN_CLASS                              0x10000
/**APN calss */
#define WDS_PROFILE_APN_BEARER                             0x20000
/** APN bearer */
#define WDS_PROFILE_OPERATOR_RESERVED_PCO                  0x40000
/** OPERATOR RESERVED PCO */
#define WDS_PROFILE_MCC                                    0x80000
/** MCC */
#define WDS_PROFILE_MNC                                    0x100000
/** MNC */
#define WDS_PROFILE_MAX_PDN_CONN_PER_BLOCK                 0x200000
/** Max PDN Connections per Time Block */
#define WDS_PROFILE_PDN_DISCON_WAIT_TIME                   0x400000
/** Max pdn wait time */
#define WDS_PROFILE_MAX_PDN_CONN_TIMER                     0x800000
/** Max PDN Connection Time */
#define WDS_PROFILE_PDN_REQ_WAIT_TIMER                     0x1000000
/** PDN Request Wait Time */
#define WDS_PROFILE_ROAMING_DISALLOWED                     0x2000000
/** Roaming disallowed*/
#define WDS_PROFILE_PARAM_LTE_ROAMING_PDP_TYPE             0x4000000
/** LTE Roaming PDP type*/
#define WDS_PROFILE_PARAM_UMTS_ROAMING_PDP_TYPE            0x8000000
/** GSM Roaming PDP type*/
#define WDS_PROFILE_PARAM_OVERRIDE_HOME_PDP_TYPE           0x10000000
/** Override IP type of the default home pdp type*/
#define WDS_PROFILE_PARAM_ATTACH_WO_PDN                    0x20000000
/** Attach without PDN*/
#define WDS_PROFILE_CLAT_ENABLED                           0x40000000
/** Clat enabled*/
#define WDS_PROFILE_PDP_ADDR                               0x80000000
/* PDP address */
#define WDS_PROFILE_FAILURE_TIMERS                         0x100000000
/* Failure timer */
#define WDS_PROFILE_APN_NAME_CHANGE                        0x200000000
/* APN name change */

/* Request source */
#define REQ_TYPE_QCPDPP          1
#define REQ_TYPE_CGDCONT         2
#define REQ_TYPE_QCPDPIMSCFGE    3
#define REQ_TYPE_QCPDPCFGE       4
#define REQ_TYPE_OBJ_11          5

/* Profile action required based on search result.*/
typedef enum
{
  PROFILE_ACTION_ENUM_MIN_ENUM_VAL = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  PROFILE_NO_ACTION = 0x00, 
  PROFILE_MODIFY = 0x01, 
  PROFILE_CREATE = 0x02, 
  PROFILE_ACTION_ENUM_MAX_ENUM_VAL = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
} profile_action_enum;

typedef struct profile_params_s
{
  uint16_t instanceID;                                /* Object 11 instance id */
  profile_action_enum type;                           /* Profile creation/ modification  */
  char apn_name[IOTAPP_QMI_WDS_APN_NAME_MAX_V01 + 1]; /* APN name passed */
  bool enable_status;                                 /* APN is enabled or not*/
  uint8_t username_valid;                             /* Username is passed or not */
  char username[IOTAPP_QMI_WDS_USER_NAME_MAX_V01 + 1];/* Username passed */
  uint8_t password_valid;                             /* Password is passed or not */
  char password[IOTAPP_QMI_WDS_PASSWORD_MAX_V01 + 1]; /* Password passed */
  uint8_t authentication_preference_valid;            /* Auth type is passed or not */
  uint8_t authentication_preference;                  /* Auth type passed*/
  iotapp_wds_pdp_type_enum_v01 pdp_type;              /* PDP type */
  bool modified_pdp;                                  /* Modified pdp field is set or not */
  iotapp_wds_pdp_type_enum_v01 old_pdp_type;          /* Old pdp type */
  uint8_t req_type;                                   /* Request from Create/Write/Set operation */
  bool delay_timer;                                   /* Delay */
  uint16_t ssid;                                      /* SSID for create/write operation */
  uint8_t token_len;                                  /* Token length for sending message response */
  uint8_t token[8];                                   /* Token  for sending message response */
}profile_params_t;

/* Structure for mandatory fields for Data profiles */
typedef struct{
  qapi_Device_Info_PDP_Type_e pdp_type;
  bool apn_name_valid;
  char apn_name[QAPI_DEVICE_INFO_APN_NAME_MAX + 1];
} wds_mandatory_res_t;

/* Structure for mandatory fields change for Data profiles.*/
typedef struct{
  bool pdp_type_valid;
  qapi_Device_Info_PDP_Type_e pdp_type;
  bool apn_name_valid;
  char apn_name[QAPI_DEVICE_INFO_APN_NAME_MAX + 1];
} wds_mandatory_res_changed_t;

/*Structure for clubbing cell info and plmn List */
typedef struct
{	
	//uint8_t cell_info_valid;
	qapi_Cell_Info* cell_info;
	//uint8_t plmn_list_valid;
	qapi_Device_Info_PLMN_Info_List_t* plmn_list;	
	qapi_Device_Info_ID_t info_id;
}Device_Info_NW_Scan_t;


/* NAS callback notification payload information */
typedef struct device_info_signal_s
{
  unsigned char            is_service_up;
  device_info_pthread_t    thread_id;
  device_info_cond_t       cnd_var;
} device_info_signal_t;



/* Profile search response structure.*/
typedef struct{
  qapi_Status_t rc_code;    /* Return code QAPI_OK for success, QAPI_ERR_* for failure */
  uint8_t search_result;    /* No Action, Create or Modify */
  bool old_pdp_type_valid;  /* Old PDP type field is set or not.*/
  uint8_t old_pdp_type;     /* If search result is Modify and PDP type is changed then 
                               this value is set.*/
} profile_search_response_t;

/* Data profile internal structure.*/
typedef struct {
  wds_pdp_type_enum_v01 pdp_type;
  char apn_name[QMI_WDS_APN_NAME_MAX_V01 + 1];
  wds_pdp_hdr_compr_type_enum_v01 pdp_hdr_compression_type;
  wds_pdp_data_compr_type_enum_v01 pdp_data_compression_type;
  char username[QMI_WDS_USER_NAME_MAX_V01 + 1];
  char password[QMI_WDS_PASSWORD_MAX_V01 + 1];
  wds_auth_pref_mask_v01 authentication_preference;
  uint8_t pcscf_addr_using_pco;
  uint8_t im_cn_flag;
  uint8_t apn_disabled_flag;
  uint32_t pdn_inactivity_timeout;
  uint8_t apn_class;
  wds_apn_bearer_mask_v01 apn_bearer;
  uint16_t op_pco_id;
  uint16_t pco_mcc;
  wds_mnc_pcs_digit_include_status_type_v01 pco_mnc_includes_pcs_digit;
  uint16_t max_pdn_conn_per_block;
  uint16_t max_pdn_conn_timer;
  uint16_t pdn_req_wait_interval;
  wds_common_pdp_type_enum_v01 override_home_pdp_type;
  char changed_apn_name[QMI_WDS_APN_NAME_MAX_V01 + 1];
  uint64_t flag;
}prf_params_t;

#define INT_STR_SIZE 16

/** Declaring function prototypes */
void iotapp_qmi_dms_init(void);
int iotapp_qmi_nas_init_blocking(void);
void iotapp_qmi_uim_init(void);
void iotapp_qmi_dsd_init(void);
void iotapp_qmi_wds_init(void);
void iotapp_qmi_wms_init(void);
void iotapp_qmi_ims_iot_init(void);
void iotapp_qmi_voice_init(void);
void iotapp_qmi_psm_init(void);
void iotapp_qmi_dins_init(void);


void iotapp_qmi_wms_release(void);

int qmi_wms_set_smsc_address(char *sms_addr);
char * qmi_wms_get_smsc_address(void);

void iotapp_qmi_dins_release(void);
void iotapp_qmi_pdc_init(void);
void iotapp_qmi_dms_release(void);
void iotapp_qmi_nas_release(void);
void iotapp_qmi_uim_release(void);
void iotapp_qmi_pdc_release(void);
void lh_location_deinit(void);
void iotapp_qmi_dsd_release(void);
void iotapp_qmi_wds_release(void);
void iotapp_qmi_ims_iot_release(void);
void iotapp_qmi_voice_release(void);
int iotapp_qmi_wds_find_and_update_profile(char *changed_apn_name, char *apn_name);
int iotapp_qmi_wds_create_profile(profile_params_t *param);
int iotapp_qmi_wds_delete_profile(uint8_t pdp_type, char *apn_name);
int iotapp_qmi_wds_modify_profile(profile_params_t *param);
int iotapp_qmi_wds_search_profile(uint8_t pdp_type, char *apn_name, uint8_t *old_pdp_type);
bool iotapp_qmi_wds_get_apn_disable_status(uint8_t pdp_type, char *apn_name);


uint8_t* get_device_manufacturer(void);
uint8_t* get_device_model_number(void);
uint8_t* get_device_hw_version(void);
uint8_t* get_device_sw_info(qapi_Device_Info_ID_t id);
uint8_t* get_device_fw_version(void);
uint8_t* get_device_serial_number(void);
int16_t get_device_battery_percent(uint8_t * );
uint8_t * get_msisdn_frm_uim(void);
uint8_t* get_iccid_frm_physical_slot(void);
uint8_t* get_imsi_frm_physical_slot(void);

qapi_Status_t iotapp_qmi_uim_send_apdu(uint8_t* req, uint16_t req_length, uint8_t** rsp, uint16_t* rsp_length, qapi_Device_Info_UIM_Response_t* uim_resp);
qapi_Status_t iotapp_qmi_uim_write_plmn(uint16_t file_id, uint8_t index_id, uint8_t* data, uint16_t length, qapi_Device_Info_UIM_Response_t* uim_resp);
qapi_Status_t iotapp_qmi_uim_read_plmn (uint16_t file_id, uint8_t** data, uint16_t* length, qapi_Device_Info_UIM_Response_t* uim_resp);
qapi_Status_t iotapp_qmi_uim_get_records_number(uint16_t file_id, uint8_t*  path, uint8_t  pathlen, uint16_t* record_number, qapi_Device_Info_UIM_Response_t* uim_resp);
qapi_Status_t iotapp_qmi_uim_read_record       (uint16_t file_id, uint8_t*  path, uint8_t  pathlen, uint16_t  record_no, uint8_t** data, uint32_t* datasize, qapi_Device_Info_UIM_Response_t* uim_resp);
qapi_Status_t iotapp_qmi_uim_unblock_pin(qapi_Device_Info_Pin_Id_t pin_id, const char* puk_pin, const char* new_pin, qapi_Device_Info_Pin_Response_t* response);
qapi_Status_t iotapp_qmi_uim_change_pin (qapi_Device_Info_Pin_Id_t pin_id, const char* old_pin, const char* new_pin, qapi_Device_Info_Pin_Response_t* response);
qapi_Status_t iotapp_qmi_uim_verify_pin (qapi_Device_Info_Pin_Id_t pin_id, const char* pin, qapi_Device_Info_Pin_Response_t* response);
qapi_Status_t iotapp_qmi_uim_protect_pin(qapi_Device_Info_Pin_Id_t pin_id, const char* pin, bool isEnable, qapi_Device_Info_Pin_Response_t* response);
qapi_Status_t iotapp_qmi_uim_set_fdn_service_status(bool isEnable);
qapi_Status_t iotapp_qmi_uim_get_fdn_service_status(qapi_Device_Info_FDN_Status_t* fdn_status);
qapi_Status_t iotapp_uim_set_sub_enable(uint8_t isEnable);

int set_psm_params(void *cfg_params);
void *get_psm_params(void);
int register_psm_cfg_change(void);
void * get_operating_mode(void);
int set_operating_mode(uint8_t op_mode);

int iotapp_qmi_get_service_status(bool is_lte_op_mode);
int iotapp_qmi_get_mode_pref(void);
void iot_dss_signal_nas_state(bool state);
void _intr_qapi_device_info_network_status_cb(bool state);
void _intr_qapi_device_info_network_status_cb2(bool state);

int iotapp_get_pdc_config_info(void);
int iotapp_get_selected_config_request(void);
int16_t nas_get_signal_strength(char *required);
int64_t nas_get_ue_info(char *required, bool get_buf, uint8_t** buff);
int16_t qmi_nas_get_network_time(bool *daylight_save);
int64_t iotapp_qmi_get_cell_loc_info(char *required);
int64_t nas_get_neighbour_intra_cell_measurement_info(char *required);
void _intr_qapi_device_info_change_cb(qapi_Device_Info_t *dev_info, uint64_t ind_id);
void _intr_qapi_device_info_change_cb2(qapi_Device_Info_t *dev_info, uint64_t ind_id, qapi_Device_Info_Hndl_t device_info_hndl);
bool util_info_id_registered(uint64_t ind_id);
bool util_info_id_registered2(uint64_t ind_id);

int iotapp_qmi_get_sim_state(void);

int qmi_wms_set_event_report_req(void);
int get_sms_payload(const unsigned char *pdu_buf, int pdu_buf_len, char **response, boolean routed_msg);
int qmi_wms_send_msg_ack(void);
int qmi_wms_raw_read_msg(char **response);

dev_info* qmi_nas_get_sys_sel_pref(qapi_Device_Info_ID_t id);
int qmi_nas_set_sys_sel_pref(qapi_Device_Info_ID_t id, int len, int *val);
int qmi_nas_set_edrx_params(void *cfg_params);
void * qmi_nas_get_edrx_params(void *cfg_params);
int iotapp_qmi_nas_set_erdx_params(uint8_t edrx_params, lte_ciot_mode mode);
int8_t iotapp_qmi_nas_get_erdx_params(uint8_t *edrx_params, lte_ciot_mode mode);

int qmi_dsd_get_network_bearer(void);
int set_app_priority(int group);
dev_info* get_app_priority_and_loaded_tech(void);
int iotapp_nas_qmi_get_jamming_status(qapi_Device_Info_RAT_Type_t rat_type, qapi_Device_Info_Jamming_Status_Get_Rsp_t *jamming_status);
qapi_Status_t ims_iot_set_ind_register_req(bool srv_status_change_ind);
qapi_Status_t ims_iot_get_register_status(qapi_Device_Info_IMS_Registration_Status_Rsp_t *ims_registration_rsp);
qapi_Status_t voice_dial_call_req(char *calling_number, uint8_t call_num_length, qapi_Device_Info_Call_Status_Rsp_t *call_resp);
qapi_Status_t voice_end_call_req(uint8_t call_id,
                                uint8_t end_cause_valid,
                                qapi_Device_Info_Call_Reject_Cause_t end_cause);
qapi_Status_t voice_answer_call_req(uint8_t call_id, bool reject_call,
                                              uint8_t reject_cause_valid,
                                              qapi_Device_Info_Call_Reject_Cause_t reject_cause);
void*iotapp_qmi_wds_get_profile_list(profile_search_response_t *response, uint8_t *count);
int iotapp_qmi_wds_search_profile_lite(void *request, profile_search_response_t *response);
prf_params_t* profile_params_set_util(void *req, uint8_t type, wds_mandatory_res_changed_t info);
uint64_t profile_params_get_util(uint8_t cmd_id);
int profile_params_fill_get_util(void *request, uint8_t cmd_id, void **response, uint8_t curr_cnt);
void* profile_params_fill_set_util(void *req, uint8_t cmd_id, uint8_t req_type,
                                         wds_mandatory_res_changed_t info);
int iotapp_qmi_wds_create_profile_lite(void *param, uint8_t cmd_id);
int iotapp_qmi_wds_modify_profile_lite(void *param, uint8_t cmd_id, wds_mandatory_res_changed_t c_info);
qapi_Status_t iotapp_qmi_wds_set_profile_action(void *request, uint8_t cmd_id);
qapi_Status_t iotapp_qmi_wds_get_profile_action(void *req, uint8_t cmd_id, void **response);
qapi_Status_t iotapp_nas_get_scan_config(qapi_Device_Info_Scan_Config_Rsp_t *scan_config_rsp);
int qmi_nas_get_cell_info(Device_Info_NW_Scan_t *nw_scan_info);
int iotapp_nas_fsk_start(qapi_Device_Info_FSK_Start_Req_t *request);
int iotapp_nas_fsk_stop(void);
int iotapp_nas_fsk_data(qapi_Device_Info_FSK_Data_Req_t *request);
int iotapp_nas_set_fsk_debug(qapi_Device_Info_Set_FSK_Debug_Req_t *request);
int iotapp_nas_set_fsk_hoptable(qapi_Device_Info_Set_FSK_Hoptable_Req_t *request);
int iotapp_nas_set_fsk_pcl(qapi_Device_Info_Set_FSK_PCL_Req_t *request);
int iotapp_nas_get_fsk_debug(qapi_Device_Info_Get_FSK_Debug_Rsp_t *response);
int iotapp_nas_get_fsk_pcl(qapi_Device_Info_Get_FSK_PCL_Rsp_t *response);
int iotapp_nas_get_fsk_hoptable(qapi_Device_Info_Get_FSK_Hoptable_Rsp_t *response);
qapi_Status_t iotapp_nas_get_wwan_sleep_threshold(uint32_t *sleep_threshold);
qapi_Status_t iotapp_nas_set_wwan_sleep_threshold(uint32_t sleep_threshold);
int iotapp_nas_get_qRxlevmin(qapi_Device_Info_Get_QRXLEVMIN_Rsp_t *response);
int iotapp_nas_set_sig_strength_threshold(qapi_Device_Info_Config_Signal_strength_Req_t *sig_config_req);
int16_t iotapp_nas_get_sinr_info (void);
int16_t iotapp_nas_get_ce_level_info (void);
int16_t iotapp_nas_get_srxlev_info(qapi_Device_Info_RAT_Type_t rat_type,qapi_Device_Info_Get_Srxlev_Rsp_t *response);

int set_nz_bsr(int enable);
dev_info* get_nz_bsr(void);

qapi_Status_t iotapp_qmi_wms_send_sms(qapi_Device_Info_Send_SMS_Req_t *req,
                                                 qapi_Device_Info_Hndl_t device_info_hndl);

qapi_Status_t iotapp_qmi_wms_write_sms(qapi_Device_Info_Write_SMS_Req_t *req,
                                                  qapi_Device_Info_Write_SMS_Rsp_t *resp);

qapi_Status_t iotapp_qmi_wms_read_sms(qapi_Device_Info_Read_SMS_Req_t *req,
                                                qapi_Device_Info_Read_SMS_Rsp_t *resp);

qapi_Status_t iotapp_qmi_wms_delete_sms(qapi_Device_Info_Delete_SMS_Req_t *req);

qapi_Status_t iotapp_qmi_wms_list_sms(qapi_Device_Info_List_SMS_Req_t *req,
                                                 qapi_Device_Info_List_SMS_Rsp_t *resp);

qapi_Status_t iotapp_qmi_wms_send_sms_ack(qapi_Device_Info_Send_SMS_Ack_Req_t *req,
                                                      qapi_Device_Info_Send_SMS_Ack_Rsp_t *resp);


qapi_Status_t iotapp_qmi_wms_get_sms_routes(qapi_Device_Info_Get_SMS_Routes_Rsp_t *resp);

qapi_Status_t iotapp_qmi_wms_set_sms_routes(qapi_Device_Info_Set_SMS_Routes_Req_t *req);

qapi_Status_t iotapp_qmi_wms_get_smsc_address(qapi_Device_Info_Get_SMSC_Req_t *req,
                                                            qapi_Device_Info_Get_SMSC_Rsp_t *resp);

qapi_Status_t iotapp_qmi_wms_set_smsc_address(qapi_Device_Info_Set_SMSC_Req_t *req);

#endif
