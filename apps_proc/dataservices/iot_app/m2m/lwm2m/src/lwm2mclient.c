/******************************************************************************

  @file    lwm2mclient.c

  ---------------------------------------------------------------------------
  Copyright (c) 2016-2023 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------

 *****************************************************************************/

/*******************************************************************************
 *
 * Copyright (c) 2013, 2014 Intel Corporation and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * The Eclipse Distribution License is available at
 *    http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    David Navarro, Intel Corporation - initial API and implementation
 *    Benjamin Cabé - Please refer to git log
 *    Fabien Fleutot - Please refer to git log
 *    Simon Bernard - Please refer to git log
 *    Julien Vermillard - Please refer to git log      
 *    Axel Lorente - Please refer to git log
 *    Toby Jaffey - Please refer to git log
 *    Bosch Software Innovations GmbH - Please refer to git log
 *    Pascal Rieux - Please refer to git log
 *    Christian Renz - Please refer to git log
 *    Ricky Liu - Please refer to git log
 *
 *******************************************************************************/

/*
   Copyright (c) 2013, 2014 Intel Corporation

   Redistribution and use in source and binary forms, with or without modification,
   are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice,
 this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.
 * Neither the name of Intel Corporation nor the names of its contributors
 may be used to endorse or promote products derived from this software
 without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 THE POSSIBILITY OF SUCH DAMAGE.

 David Navarro <david.navarro@intel.com>
 Bosch Software Innovations GmbH - Please refer to git log

*/

#include "liblwm2m.h"
#include "lwm2mclient.h"
#include "internals.h"


#include "object_security.h"
#include "object_server.h"
#include "object_access_control.h"
#include "bootstrap_info.h"
#include "lwm2m_signals.h"
#include "obj_connectivity_moni.h"
#include "object_access_control.h"
#include "qapi_pmapp_vbatt.h"
#include "qapi_device_info.h"
#include "lwm2m_carrier.h"
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <locale.h>
#include "iotapp_qmi_internal.h"
#include <errno.h>
#include <signal.h>
#include "qapi_socket.h"
#include "qapi_types.h"
#include "qapi_dnsc.h"
#include "qurt/qurt_thread.h"
#include "timer.h"
#include "iotapp_dss_util.h"
#include "fs_public.h"
#include "fs_lib.h"
#include "qapi_fs.h"
#include "qapi_coap.h"
#include "wireless_messaging_service_v01.h"

#ifndef MOB_TX
#include "qurt_txm_qapi_fwk.h"
#else
#include "offtarget_stubs.h"
#define QCLI_Printf(...)
#endif

#include "qapi_lwm2m.h"
#include "qapi_coap.h"
#include "object_apn_connection.h"
#include "object_cell_nw_connectivity.h"
#include "iotapp_log_util.h"
#include "ds_log.h"
#include "lwm2m_rx.h"




#ifdef ENABLE_INTERNAL_BUILD
#include "pal.h"
#include "qcli.h"
#include "qcli_api.h"
#endif

#ifdef UNIT_TEST
#include "unit_test.h"
#endif //UNIT_TEST
#ifdef DTLS_SUPPORT
#include "qapi_ssl.h"
#define LWM2M_SERVERS_SSL_MAX_FRAG_LEN 2048
#endif

#define CFG_VAL_LEN 512
#define DEFAULT_RETRY_TIMEOUT 30
#define DEFAULT_EXPONENT_VAL 2
#define DEFAULT_MAX_TIMEOUT 640 
#define DEFAULT_MAX_RETRY_VAL 5
#define DEFAULT_BATTERY_LEVEL_THRESHOLD 20
#define DEFAULT_BOOT_UP_SLEEP_TIME 5
#define DEFAULT_SESSION_RESUMPTION_TIME 60
#define DEFAULT_WAIT_TIME_FOR_REREG 86400
#define DEFAULT_FACTORY_RESET_RATE_LIMIT 1
#define DEFAULT_REG_UPDATE_RATE_LIMIT 1
#define DEFAULT_LIFETIME_DELTA_PERCENTAGE  5
#define DEFAULT_PMIN_DELTA_PERCENTAGE    100
#define DEFAULT_PMAX_DELTA_PERCENTAGE     10
#define DEFAULT_TRANSPORT_GUARD           30
#define DEFAULT_PSM_DELTA                 60
#define DEFAULT_STORE_NOTIFICATION        5
#define DEFAULT_CONNECTION_COUNT          5
#define DEFAULT_BLOCK_MAX_SIZE            1024
#define DEFAULT_BLOCK_MIN_SIZE            16


#define DEFAULT_MAX_BOOTSTRAP_WAIT_SEC 0
#define MAX_BATTERY_LEVEL 100
#define LWM2M_CFG_FILE_PATH "/datatx/lwm2m_cfg"
#define OBJECTS_CFG_FILE_PATH "/datatx/objects_cfg"

#define IFACE_NAME_MAX_LEN 14
#define CARRIER_APN_CFG "/datatx/carrier_apn_cfg"

#define PDU_TYPE_PUSH 0x06
#define LWM2M_PAYLOAD_PUSH_ID 0x9A

static int g_reboot = 0;
static int g_quit = 0;
static int g_restart = 0;
int gFwUpdateInProgress = 0;
bool glwm2m_context = false;
bool roaming_status;
bool g_carrier_cfg = false; /* flag kept default as true assuming carrier cfg is enable , once pdc interface is ready , need to consider based on that */
bool lwm2m_psm_enable     = LWM2M_FALSE;
qapi_Device_Info_Hndl_t lwm2m_dev_info_hdl;

#ifdef PSM_LWM2M
extern bool psm_inited;
extern void lwm2m_psm_cb(psm_status_msg_type *psm_status);
#endif
static qapi_Coap_Session_Hdl_t lwm2m_coap_hdl;

#ifdef ENABLE_CONN_MONITOR
extern char class3_apn[CARRIER_APN_LEN];
extern char class6_apn[CARRIER_APN_LEN];
extern char class7_apn[CARRIER_APN_LEN];
extern char class3_apn_prev[CARRIER_APN_LEN];
extern char class6_apn_prev[CARRIER_APN_LEN];
extern char class7_apn_prev[CARRIER_APN_LEN];
#endif

extern wms_ack_indicator_enum_v01 trans_ack_indicator;

// Registration Retry Information
extern time_t gRegRetryInitialTimeout;
extern time_t gRegRetrytimeoutExponent;
extern time_t gRegRetryMaxTimeout;
extern boolean routed_msg;
extern uint8_t trans_msg_data[255];
extern uint16_t trans_msg_datalen;

 uint8_t sms_msg_data_number1[] =
                  { 
                     68, 12, 145, 25, 0, 9, 0, 9, 
                     0, 0, 245, 113, 17, 3, 0, 35, 
                     130, 74, 28, 6, 5, 4, 11, 132, 
                     192, 2, 1, 6, 3, 196, 175, 154, 
                     85, 2, 27, 83, 69, 174, 37, 199, 
                     242, 177, 49, 1, 49, 1, 56
                  } ;

uint16_t sms_msg_datalen = 47;


/* SMS message */ 
lwm2m_sms_msg_t sms_msg;

extern void devicecap_init(void);

int lwm2m_client_start(void);
#ifdef LWM2M_SERVER_OBJECT
uint8_t updt_disabl_flag; // !<used to set the update or disable flag into 
//   Server list>
uint16_t short_serverId; // ! <used to update the sever list>
#endif
static bool factory_reset_req = false; //! <used to reset the device>

#define OBJ_COUNT 12

lwm2m_object_t * objArray[OBJ_COUNT];

#define MIN_INTERVAL    3000  // Minimum interval value for location init function
#define MIN_DISTANCE    1       // minimum distance value for location init function

int gps_minDistance;
int gps_minInterval;
bool enable_location 		   = LWM2M_TRUE;
bool enable_location_after_lte = LWM2M_FALSE;
bool enable_dev_cap_obj 	   = LWM2M_TRUE;
bool enable_conn_m_obj 		   = LWM2M_TRUE;
bool enable_soft_mgnt_obj 	   = LWM2M_TRUE;
bool enable_firmware_obj 	   = LWM2M_TRUE;
bool enable_conn_stat_obj 	   = LWM2M_TRUE;
bool enable_cell_nw_conn_obj   = LWM2M_TRUE;
bool enable_apn_conn_obj 	   = LWM2M_TRUE;
bool enable_host_dev_obj 	   = LWM2M_TRUE;
bool enable_acl_obj 		   = LWM2M_TRUE;
bool enable_bs_server_replace  = LWM2M_FALSE; //enable bootstrap server replacement feature or no

extern bool location_init_status;
extern bool fetch_best_avail_loc;

bool   isLwm2mStartedByApp  = false;

const char* ssl_path = "/datatx/ssl/";

extern bool lh_location_init(void); // GPS init function
extern uint32_t lh_location_single_shot(void);
extern uint32_t lh_location_best_available(void);
extern lwm2m_watcher_t * prv_getWatcher(lwm2m_context_t * contextP, lwm2m_uri_t * uriP, lwm2m_server_t * serverP);

q_type lwm2m_location_pending_read_observe;
uint32_t location_min_interval = 0x7FFFFFFF;
extern uint32 trackingSessionId;

// only backup security and server objects
#define DEFAULT_BS_SSID 100

qurt_thread_t lwm2m_thread_Handle;
extern qurt_signal_t lwm2m_signal_Handle;
qurt_thread_t loc_update_Handle;
qurt_signal_t loc_update_sem;
bool active_single_shot;
bool location_thread_inited = FALSE;
#ifdef ENABLE_INTERNAL_BUILD
QCLI_Group_Handle_t qcli_m2m_handle;
#endif
static int m2m_thread_started = -1;
uint16_t bs_ssid;

#define ACK_TIMEOUT 60

bool lwm2m_in_sleep_state;
uint32_t active_time;
uint32_t curr_time;
bool roaming_feat_enabled;
bool persis_regstatus_feat_enabled = LWM2M_TRUE; 
bool persis_bootstrap_feat_enabled = LWM2M_TRUE;

uint32_t gMemThreshold;
int gSignalThreshold = -80;

char sms_number_from_wms[32];

//!this is the internal data of lwm2m client
client_data_t data;
//! This bitmask is used to identify whether or not to trigger datacall for a apn
//! This is a toimer group for all timers used by lwm2m 
timer_group_type lwm2mTimerGroup;

static timer_type signalWaitTimeout;

//! this is the timeout value for sleep 
time_t sleepAckTimeout;

qurt_thread_t get_lwm2m_thread_Handle(void);

typedef struct _sms_rate_info_
{
  uint16_t sms_count_per_hour;
  time_t expiry_time;
  bool isValid;
}sms_rate_info_t;

typedef struct _lwm2m_cfg_item_str
{
  const char*    cfg_str;
  char*          cfg_value_ptr;
  uint8_t        cfg_value_len;
  const char*    cfg_default_value;
}lwm2m_cfg_item_str ;

typedef struct _lwm2m_cfg_item_bool
{
  const char*    cfg_str;
  bool*          cfg_value_ptr;
  bool           cfg_default_value;
}lwm2m_cfg_item_bool ;

typedef struct _lwm2m_cfg_item_integer 
{
  const char*    cfg_str;
  uint8_t*       cfg_value_ptr;
  uint8_t        size; /*to indicate integer is 8 or 16 or 32*/
  uint32_t       cfg_default_value;
  uint32_t       min_value;
  uint32_t       max_value;
  
}lwm2m_cfg_item_integer ;

#define MAX_INT_32 0xFFFFFFFF
#define CFG_ITEM_INT(str, variable, default, min, max)  {str,(uint8_t*)&variable, sizeof(variable), default, min, max},
#define CFG_ITEM_STR(str, variable, default) {str, variable, sizeof(variable), default},
  


extern void sys_m_reset(void);
void lwm2m_location_thread_start(void);
void lwm2m_location_thread_stop(void);
void lwm2m_location_thread(void *args);
void lwm2m_indicate_location_resource_value_change(const char *uriPath);
void lwm2m_datacall_handle_signals(lwm2m_context_t *lwm2mH, uint32 signals);
qapi_Status_t lwm2m_start_byApp(void);

#define LWM2M_LOCATION_PRIORITY  180
#define LWM2M_LOCATION_STACK_SIZE  1024*4

void purge_persistent_values()
{
  efs_unlink(LWM2M_VERSION_PERSISTENCE_FILE);
  efs_unlink(SECURITY_PERSISTENCE_FILE);
  efs_unlink(SERVER_PERSISTENCE_FILE);
  efs_unlink(ACL_PERSISTENCE_FILE);
  efs_unlink(DEVICE_PERSISTENCE_FILE);
  efs_unlink(FIRMWARE_PERSISTENCE_FILE);
  efs_unlink(REGISTRATION_STATUS_PERSISTENCE_FILE);
  efs_unlink(LWM2M_PERSITENT_OBSERVATION_STORAGE);
  efs_unlink(LWM2M_APN_CONN_PERSISTENCE_FILE);
  efs_unlink(LWM2M_SOFTWARE_MGNT_PERSISTENCE_FILE);
  efs_unlink(LWM2M_DEVICE_CAP_MGNT_PERSISTENCE_FILE);
  efs_unlink(LWM2M_CELL_NW_CONN_PERSISTENCE_FILE);
  efs_unlink(LWM2M_LOCATION_OBJ_PERSISTENCE_PATH);
  efs_unlink(LWM2M_DEVICE_OBJ_PERSISTENCE_PATH);
  efs_unlink(LWM2M_CONN_MON_OBJ_PERSISTENCE_PATH);
  efs_unlink(LWM2M_CONN_STAT_OBJ_PERSISTENCE_PATH);
  efs_unlink(LWM2M_FIRMWARE_OBJ_PERSISTENCE_PATH);
  efs_unlink(CONN_MON_PERSISTENCE_FILE);
  efs_unlink(CONN_STATS_PERSISTENCE_FILE);
  efs_unlink(LWM2M_REGISTERED_EXTOBJ_PERSISTENCE_FILE);  
  efs_unlink(LWM2M_CONFIG_PERSISTENCE_FILE);
}

/**
 * @fn void lwm2m_delete_server_security_credentials
 * @brief   This function is used to delete all certificates or PSK files
 *          From the secure location whenever the factory reset for bootstrapping . The new
 *          certificates would be obtained after re bootstrapping
 *          Still we need to keep the LWM2M BS security_credentials and no need to delete them
 * @param lwm2mH handle to lwm2m context
 * @return  void
 */

void lwm2m_delete_server_security_credentials(lwm2m_context_t * contextP)
{
  lwm2m_server_t *  serverList =  NULL;  
  security_instance_t * targetP = NULL;
  lwm2m_object_t * objectP = NULL;
  char cert_fname_bin[MAX_CERT_NAME_LEN];
  qapi_Net_SSL_Cert_Type_t cert_type = QAPI_NET_SSL_CERTIFICATE_E;

  if( contextP == NULL ) 
  {
    LOG_DEBUG("%s contextP",nullError); 
    return ;
  } 
  serverList =  contextP->serverList ; 
  while (serverList) 
  {
    objectP = (lwm2m_object_t *)LWM2M_LIST_FIND(contextP->objectList, LWM2M_SECURITY_OBJECT_ID);
    if (NULL == objectP) continue;

    targetP = (security_instance_t *)LWM2M_LIST_FIND(objectP->instanceList, serverList->secObjInstID);
    if (targetP != NULL) 
    {
      serverList = serverList->next;
      if(targetP->cert_source == CERT_SOURCE_PRELOAD)
      {
        ds_iot_ulog_high("cert is from local preloaded, don't delete");
	  	continue;
      }

      if ( targetP->securityMode == LWM2M_SECURITY_MODE_PRE_SHARED_KEY)
      { 
        /* Deleting old psk file*/
        memset(cert_fname_bin, 0, MAX_CERT_NAME_LEN);
        snprintf((char *)cert_fname_bin, MAX_CERT_NAME_LEN, "%hu_psk.bin", targetP->shortID);
        cert_type = QAPI_NET_SSL_PSK_TABLE_E;
      }
      else if ( targetP->securityMode == LWM2M_SECURITY_MODE_CERTIFICATE)
      {
        /* Deleting old server Domain Issued certificate file*/
        memset(cert_fname_bin, 0, MAX_CERT_NAME_LEN);
        snprintf((char *)cert_fname_bin, MAX_CERT_NAME_LEN, "%hu_server_cert.bin", targetP->shortID);
        cert_type = QAPI_NET_SSL_DI_CERT_E;

        if(qapi_Net_SSL_Cert_File_Exists(cert_fname_bin, QAPI_NET_SSL_DI_CERT_E) == QAPI_OK)
        {
          ds_iot_log_strf_high("purging cert: %s file", cert_fname_bin);
          if( 0 != qapi_Net_SSL_Cert_delete(cert_fname_bin, cert_type))
          {
            ds_iot_log_strf_high("Failed to delete cert: %s file", cert_fname_bin);
          } 
        }

        /* Deleting old client certificate file*/
        memset(cert_fname_bin, 0, MAX_CERT_NAME_LEN);
        snprintf((char *)cert_fname_bin, MAX_CERT_NAME_LEN, "%hu_cert.bin", targetP->shortID);
        cert_type = QAPI_NET_SSL_CERTIFICATE_E;
      }
      else
      {
        continue;
      }
    }
    else
    {
      continue;
    }
    /* Purging old psk or certificate files */
    if(qapi_Net_SSL_Cert_File_Exists(cert_fname_bin, cert_type) == QAPI_OK)
    {
      ds_iot_log_strf_high("purging cert: %s file", cert_fname_bin);
      if( 0 != qapi_Net_SSL_Cert_delete(cert_fname_bin, cert_type))
      {
        ds_iot_log_strf_err("Failed to delete cert: %s file", cert_fname_bin);
      } 
    }
  }
}

void set_bootstrap_context(bool rebootstrap_req)
{
  lwm2m_server_t *bs_server = data.lwm2mH->bootstrapServer;

  /* Initialize the bootstrap required - will be set according to the bootstrap server information */
  data.glwm2m_app_ctx.bs_performed = FALSE; 
  /* Configure factory bootstrapping by default - will be unset if any bootstrap servers are configured */ 
  data.glwm2m_app_ctx.factory_bs = TRUE;
    
  /* Check the bootstrap server list to see if bootstrapping needs to be performed */ 
  if (bs_server)
  {
    /* Bootstrap servers detected - unset factory bootstraping */
    data.glwm2m_app_ctx.factory_bs = FALSE;
    
    /* Re-bootstrapping needs to be performed */
    if (rebootstrap_req)
    {
      bs_server->isBootstrapped = FALSE;
      ds_iot_ulog_high("Application requested re-bootstrap needs to be performed");

      /* Remove the application re-bootstrap persistence file */ 
      if (efs_unlink(LWM2M_APP_REBOOTSTRAP_PERSISTENCE_PATH) < 0)
        ds_iot_ulog_err("QAPI_Removing the application re-bootstrap persistence file failed");

      /* Remove the application object persistence file */ 
      if (efs_unlink(LWM2M_APP_OBJ_PERSISTENCE_PATH) < 0)
        ds_iot_ulog_err("QAPI_Removing the application object persistence file failed"); 
    }
    /* Bootstrapping already performed */
    else if (bs_server->isBootstrapped)
    {
      data.glwm2m_app_ctx.bs_performed = TRUE;
    }     
  }
}


#ifdef FACTORY_BOOTSTRAPPING
/**
 * @fn bool prv_initialise_objects()
 * @brief   This function creates all implemented objects and initialises 
 *          as needed
 *
 * @param   dataP       handle to client data structure
 * @param   objArrayP   handle to Object Array
 *
 * @return  true if successful, else false
 */
static bool prv_initialise_objects(client_data_t *dataP,
    lwm2m_object_t *objArrayP[], int* objCount)
{
  uint8_t index = 0x00;
  /* Index of all objects that are part of the bootstrap.ini */ 
  uint8_t security_index = 0x00; 
  uint8_t server_index   = 0x00;
  lwm2m_object_t * acl_obj_ptr = NULL;
  lwm2m_object_t * apn_obj_ptr = NULL;
#ifdef AUTHORIZATION_SUPPORT
  uint8_t acl_index      = 0x00;
#endif
#ifdef ENABLE_APN_CONN_OBJ
  uint8_t apn_conn_index = 0x00;
#endif
#ifdef ENABLE_CELL_NW_CONN_OBJ
  uint8_t cell_nw_index  = 0x00;
#endif
#ifdef ENABLE_SOFTWARE_MGNT_OBJ
  uint8_t software_mgnt_index  = 0x00;
#endif
#ifdef ENABLE_DEVCAP_OBJ
  uint8_t device_cap_index = 0x00;
#endif
  int fd = -1;

  if((NULL == dataP) || (NULL ==objArrayP) || objCount == NULL)
  {
    LOG_DEBUG("%s",nullError);    
    return LWM2M_FALSE;
  }

  //! Initialising security object 
  objArrayP[index] = get_security_object();
  security_index = index;
  if (NULL == objArrayP[index++])
  {
    ds_iot_ulog_err("Failed to create security object");
    return LWM2M_FALSE;
  }
  dataP->securityObjP = objArrayP[security_index];

  //! Initialising server Object
  objArrayP[index] = get_server_object();
  server_index = index; 
  if (NULL == objArrayP[index++])
  {
    ds_iot_ulog_err("Failed to create server object");
    return LWM2M_FALSE;
  }
  dataP->serverObject = objArrayP[server_index];
#ifdef AUTHORIZATION_SUPPORT
  if(enable_acl_obj)
  {  //! Initialising Access control Object 
    objArrayP[index] = acc_ctrl_create_object();
    acl_index = index;   
    if (NULL == objArrayP[index++])
    {
      ds_iot_ulog_err("Failed to create access control object");
      return LWM2M_FALSE;
    }
    acl_obj_ptr = objArrayP[acl_index];
  }
#endif

#ifdef ENABLE_APN_CONN_OBJ
  if(enable_apn_conn_obj)
  {
    //! Initialising APN connection profile object   
    objArrayP[index] = get_object_apn_conn();
    apn_conn_index = index;
    if (NULL == objArrayP[index++])
    {
      ds_iot_ulog_err("Failed to create apn connection profile object");
      return LWM2M_FALSE;
    }
	apn_obj_ptr = objArrayP[apn_conn_index];
  }
#endif

  ds_iot_log_strf_med("EFS_OPT: File open %s",SECURITY_PERSISTENCE_FILE);
  fd = efs_open(SECURITY_PERSISTENCE_FILE, 00);
  if (EFAILURE != fd)
  {
    efs_close(fd);
    load_security_persistent_info(objArrayP[security_index]);
    ds_iot_log_strf_med("EFS_OPT: File open %s",SERVER_PERSISTENCE_FILE);
    fd = efs_open(SERVER_PERSISTENCE_FILE, 00);
    if (EFAILURE != fd)
    {
      efs_close(fd);
      load_server_persistent_info(objArrayP[server_index]);
    }
#ifdef AUTHORIZATION_SUPPORT
	if(enable_acl_obj)
	{
      ds_iot_log_strf_med("EFS_OPT: File open %s",ACL_PERSISTENCE_FILE);
      fd = efs_open(ACL_PERSISTENCE_FILE, 00);
      if (EFAILURE != fd)
      {
        efs_close(fd);
        load_ac_persistent_info(objArrayP[acl_index]);
      }
	}
#endif
#ifdef ENABLE_APN_CONN_OBJ
    if(enable_apn_conn_obj)
    {
      ds_iot_log_strf_med("EFS_OPT: File open %s",LWM2M_APN_CONN_PERSISTENCE_FILE);
      // Load persistent info for APN connection profile object
      fd = efs_open(LWM2M_APN_CONN_PERSISTENCE_FILE, 00);
      if (EFAILURE != fd)
      {
        efs_close(fd);
        load_apn_info_persistent_info(objArrayP[apn_conn_index]);
      }
    }
#endif
    ds_iot_ulog_high("successfully loaded Server/Security/ACL/APN persistent values");
  }
  else
  {     
    if (LWM2M_STARTUP_BOOTSTRAP_BYAPP == data.cfg_info.lwm2m_startup_mode)
    {
      ds_iot_ulog_high("startup-mode=byApp, don't load bootstrap info from ini");
    }
    else if (LWM2M_FALSE ==  get_bootstrap_info(objArrayP[security_index], 
                                                objArrayP[server_index],  
                                                acl_obj_ptr,
                                                apn_obj_ptr)) 
    {
      return LWM2M_FALSE;
    }

#if defined (LWM2M_BOOTSTRAP) 
    update_security_isbootstrap_flag(objArrayP[security_index]);
#endif
  }

  //! Initialising Device object 
  objArrayP[index] = get_object_device();
  if (NULL == objArrayP[index++])
  {
    ds_iot_ulog_err("Failed to create Device object");
    return LWM2M_FALSE;
  }
#ifdef ENABLE_CONN_MONITOR
  if(enable_conn_m_obj)
  {
  //! Initialising connectivity monitering object 
  objArrayP[index] = get_object_conn_m();
  if (NULL == objArrayP[index++])
  {
    ds_iot_ulog_err("Failed to create connectivity monitoring object");
    return LWM2M_FALSE;
  }
  }
#endif
#ifdef ENABLE_FIRMWARE_OBJ
  if(enable_firmware_obj)
  {
  //! Initialising fimware object 
  objArrayP[index] = get_object_firmware();
  if (NULL == objArrayP[index++])
  {
    ds_iot_ulog_err("Failed to create Firmware object");
    return LWM2M_FALSE;
  }
  }  
#endif
  //! Initialising location object 
  objArrayP[index] = get_object_location();
  if (NULL == objArrayP[index++])
  {
    ds_iot_ulog_err("Failed to create location object");
    return LWM2M_FALSE;
  }
#ifdef ENABLE_CONN_STAT_OBJ
  if(enable_conn_stat_obj)
  {  
  //! Initialising connectivity stats object 
  objArrayP[index] = get_object_conn_s();
  if (NULL == objArrayP[index++])
  {
    ds_iot_ulog_err("Failed to create connectivity statistics object");
    return LWM2M_FALSE;
  }
  }
#endif

#ifdef ENABLE_SOFTWARE_MGNT_OBJ
  if(enable_soft_mgnt_obj)
  {
  //! Initialising software management object   
  objArrayP[index] = get_object_software_mgnt();
  software_mgnt_index = index;
  if (NULL == objArrayP[index++])
  {
   ds_iot_ulog_err("Failed to create software management object");
    return LWM2M_FALSE;
  }

  // Load persistent info for software managemnet profile object
  ds_iot_log_strf_med("EFS_OPT: File open %s",LWM2M_SOFTWARE_MGNT_PERSISTENCE_FILE);
  fd = efs_open(LWM2M_SOFTWARE_MGNT_PERSISTENCE_FILE , 00);
  if (EFAILURE != fd)
  {
     efs_close(fd);
     load_software_mgnt_persistent_info(objArrayP[software_mgnt_index]);
  }
  }
#endif
#ifdef ENABLE_CELL_NW_CONN_OBJ
  if(enable_cell_nw_conn_obj)
  {
   //! Initialising cell network connectivity object   
   objArrayP[index] = get_object_cell_nw_conn(); 
   cell_nw_index = index;
   if (NULL == objArrayP[index++]) 
   { 
      ds_iot_ulog_err("Failed to create apn connection profile object"); 
      return LWM2M_FALSE; 
   } 
  
  // Load persistent info for cell network connectivity object 
  ds_iot_log_strf_med("EFS_OPT: File open %s",LWM2M_CELL_NW_CONN_PERSISTENCE_FILE);
   fd = efs_open(LWM2M_CELL_NW_CONN_PERSISTENCE_FILE, 00); 
   if(EFAILURE != fd) 
   { 
     efs_close(fd); 
     load_cell_nw_conn_persistent_info(objArrayP[cell_nw_index]); 
	   }
   }
#endif
  //! Initialising device capability object 
#ifdef ENABLE_DEVCAP_OBJ
	if(enable_dev_cap_obj)
	{
	  objArrayP[index] = get_object_deviceCap();
	  device_cap_index = index;
	  if (NULL == objArrayP[index++])
	  {
	    ds_iot_ulog_err("Failed to create device capability management object");
	    return LWM2M_FALSE;
	  }

	  // Load persistent info for device capability management object 
	  ds_iot_log_strf_med("EFS_OPT: File open %s",LWM2M_DEVICE_CAP_MGNT_PERSISTENCE_FILE);
	  fd = efs_open(LWM2M_DEVICE_CAP_MGNT_PERSISTENCE_FILE , 00); 
	  
	   if(EFAILURE != fd) 
	   { 
	     efs_close(fd); 
	     load_device_cap_manangement_persistent_info(objArrayP[device_cap_index]);
	   } 
	}
#endif
  /* Commenting out code for 102555*/
#if 0
  objArrayP[index] = get_object_host_dev();
  if (NULL == objArrayP[index++])
  {
    LOG_ERROR("Failed to create Host Device object");
    return LWM2M_FALSE;
  }
#endif
  *objCount = index;
  ds_iot_ulog_high("Objects Initialisation Successful");
  return LWM2M_TRUE;
}

/**
 * @fn void prv_destroy_objects()
 * @brief   This function is used to clean and destroy all the objects
 *          that were created
 *
 * @param   objArrayP   list of created objects
 *
 * @return  void
 */
void prv_destroy_objects(lwm2m_object_t *objArrayP[])
{
  unsigned int index = LWM2M_SECURITY_OBJECT_ID;

  if(NULL == objArrayP)
  {
    LOG_DEBUG("%s",nullError);      
    return;
  }     

  /* Note - Cleaning of the resources should match the order in which the object list 
   *        is populated in prv_initialise_objects. 
   * security/server/acl-ctrl/apn-conn, these objects are bootstrap-info obj, need free the object handler explicitly
   */         
  clean_security_object(objArrayP[index]);
  lwm2m_free(objArrayP[index++]);
  objArrayP[index - 1] = NULL;
  clean_server_object(objArray[index]);
  lwm2m_free(objArray[index++]);
  objArrayP[index - 1] = NULL;
#ifdef AUTHORIZATION_SUPPORT
  if(enable_acl_obj)
  {
    clean_acl_ctrl_object(objArrayP[index]);
    lwm2m_free(objArrayP[index++]);
    objArrayP[index - 1] = NULL;
  }
#endif
#ifdef ENABLE_APN_CONN_OBJ
  if(enable_apn_conn_obj)
  {
    clean_apn_conn_object(objArrayP[index]);
    lwm2m_free(objArrayP[index++]);  
  	objArrayP[index - 1] = NULL;
  }
#endif
  free_object_device(objArrayP[index++]);
  objArrayP[index - 1] = NULL;
#ifdef ENABLE_CONN_MONITOR
  if(enable_conn_m_obj)
  {
    free_object_conn_m(objArrayP[index++]);
    objArrayP[index - 1] = NULL;
  }
#endif
#ifdef ENABLE_FIRMWARE_OBJ
  if(enable_firmware_obj)
  {
    free_object_firmware(objArrayP[index++]);
    objArrayP[index - 1] = NULL;
  }
#endif
  free_object_location(objArrayP[index++]);
  objArrayP[index - 1] = NULL;
#ifdef ENABLE_CONN_STAT_OBJ
  if(enable_conn_stat_obj)
  {
    free_object_conn_s(objArrayP[index++]);
    objArrayP[index - 1] = NULL;
  }
#endif
#ifdef ENABLE_SOFTWARE_MGNT_OBJ
  if(enable_soft_mgnt_obj)
  {
    free_object_softwaremgnt(objArrayP[index++]);
    objArrayP[index - 1] = NULL;
  }
#endif
#ifdef ENABLE_CELL_NW_CONN_OBJ
  if(enable_cell_nw_conn_obj)
  {
    free_object_cell_nw_conn(objArrayP[index++]);
    objArrayP[index - 1] = NULL;
  }
#endif
#ifdef ENABLE_DEVCAP_OBJ
  if(enable_dev_cap_obj)
  {
    free_object_devicecap(objArrayP[index++]);
    objArrayP[index - 1] = NULL;
  }
#endif
/* Commenting out code for 102555*/
#if 0
  free_object_host_device(objArrayP[index++]);
  objArrayP[index - 1] = NULL;
#endif
}
#endif

/**
 * @fn void void handle_value_changed()
 * @brief   This function is used to handle the changed values
 *
 * @param lwm2mH handle to lwm2m context
 * @param uri handle to uri
 * @param value changed value
 * @param length of the value pointer
 *
 * @return  void
 */
void handle_value_changed(lwm2m_context_t * lwm2mH,
    lwm2m_uri_t * uri,
    const char * value,
    size_t valueLength)
{
  lwm2m_object_t * object = NULL;
  if( NULL == uri || NULL == value || NULL == lwm2mH )
  {
    LOG_DEBUG("%s",nullError);
    return;
  }
  object = (lwm2m_object_t *)LWM2M_LIST_FIND(lwm2mH->objectList, uri->objectId);

  if ((NULL != object) && (NULL != object->userData))
  {
    if (object->writeFunc != NULL)
    {
      lwm2m_data_t * dataP;
      int result;

      dataP = lwm2m_data_new(1);
      if (dataP == NULL)
      {
        LOG_DEBUG("%s", mallocError);
        return;
      }
      dataP->id = uri->resourceId;
      lwm2m_data_encode_nstring(value, valueLength, dataP);

      result = object->writeFunc(uri->instanceId, 1, dataP, object, INVALID_RES_INST_ID, QAPI_COAP_POST, TRUE);
      if (LWM2M_405_METHOD_NOT_ALLOWED == result)
      {
        switch (uri->objectId)
        {
          case LWM2M_DEVICE_OBJECT_ID:
            result = device_change(dataP, object);
            break;
          case LWM2M_CONN_MONITOR_OBJECT_ID:
#ifdef ENABLE_CONN_MONITOR
			if(enable_conn_m_obj)
              result = connectivity_moni_change(dataP, object);
#endif
            break;
          case LWM2M_CONN_STATS_OBJECT_ID:
#ifdef ENABLE_CONN_STAT_OBJ
			if(enable_conn_stat_obj)
              result = connectivity_stat_change(dataP, object);
#endif
			break;
		  case LWM2M_CELL_NW_CONN_OBJECT_ID:
#ifdef ENABLE_CELL_NW_CONN_OBJ
			if(enable_cell_nw_conn_obj)
              result = cell_nw_change(dataP, object);
#endif
            break;
          default:
            break;
        }
      }

      if (LWM2M_204_CHANGED != result)
      {
        ds_iot_ulog_high("Failed to change value!");
      }
      else
      {
        ds_iot_ulog_high("value changed!");
        lwm2m_resource_value_changed(lwm2mH, uri);
      }
      lwm2m_data_free(1, dataP);
      return;
    }
	
    LOG_DEBUG("handle_value_changed--NotExecute!");
    return;
  }
  else
  {
    LOG_DEBUG("Object not found !");
  }
}


/**
 * @fn  resolve_host_to_ip()
 * @brief  This function is used to resolve host name into ip
 *
 * @param host Host name to be resolved
 *
 * @return  ESUCCESS on success,
 *          EFAILURE on failure
 */
int resolve_host_to_ip(carrier_apn_cfg_t *apn_cfg, lwm2m_server_t* server_info, char *host, char *res_host)
{
  int ip_count = 0;
  struct ip46addr dnsInfo;
  char dns_svr_buf[PRV_ADDRSTRLEN];
  qapi_DSS_Addr_Info_t *ip_info = NULL;
  int ret = EFAILURE;
  struct sockaddr_in addr;
  struct sockaddr_in6 addr6;
  char ifname[IFACE_NAME_MAX_LEN];
  void *call_hndl = NULL;

  if ((apn_cfg == NULL) || 
      (server_info == NULL) || 
      (host == NULL) || 
      (res_host == NULL))
  {
    ds_iot_ulog_err4("Invalid parameters - apn_cfg[0x%x], server_info[0x%x], host[0x%x], res_host[0x%x]", apn_cfg, server_info, host, res_host); 
    return EFAILURE;
  }

  /* Retrieve the APN configuration associated with short server ID */ 
  apn_cfg = (carrier_apn_cfg_t *)LWM2M_LIST_FIND(apn_cfg, server_info->shortID);
  if (NULL == apn_cfg)
  {
    ds_iot_ulog_err1("Can't find APN config for SSID[0x%x]", server_info->shortID); 
    goto resolve_err;
  }

  /* Non-IP family there is no concept of any hostname resolution (connection already exists when the data call comes up) */
  if (AF_NONIP == apn_cfg->transport_binding)
  {
    ds_iot_ulog_err1("Non-IP doesn't need hostname resolution for SSID[0x%x]", server_info->shortID);
    return EFAILURE;
  }  

  /* Retrieve the data call handle information (IP family only) */
  call_hndl = (AF_INET6 == server_info->local_ip.sa_family) ? apn_cfg->callHdl_v6 : apn_cfg->callHdl_v4_nip; 
 
  memset(&dnsInfo, 0x00, sizeof(dnsInfo));
  dnsInfo.type = server_info->local_ip.sa_family;

  /* Retrieve the IP address information */ 
  ip_info = iot_dss_get_ip_info_by_hndl(call_hndl, &ip_count);
  if (NULL == ip_info)
  {
    ds_iot_ulog_err2("Fetching IP address information failed for call handle[0x%x] for SSID[%d]", call_hndl, server_info->shortID);
    goto resolve_err;  
  } 

  /* Fetch interface name associated with the handle */ 
  if (QAPI_OK != iotapp_dss_get_iface_name(call_hndl, ifname, IFACE_NAME_MAX_LEN))
  {
    ds_iot_ulog_err2("Fetching iface name failed for call handle[0x%x], SSID[%d]", call_hndl, server_info->shortID);
    goto resolve_err; 
  }

  ds_iot_log_strf_high("host[%s];iface name[%s] for call handle[0x%x], SSID[%d]",host, ifname, call_hndl, server_info->shortID);
  /* Resolve the host information - IPv4 */ 
  if (AF_INET == dnsInfo.type)
  {
    memset(&addr, 0x00, sizeof(addr));

    if (0 != inet_pton(AF_INET, host, &addr.sin_addr))
    {
      /* Start DNS client */
      ret = qapi_Net_DNSc_Command(QAPI_NET_DNS_START_E);
      if (ret != QAPI_OK) 
      {
        ds_iot_ulog_err1("DNS client start failed with status[%d]", ret);
        goto resolve_err;  
      }

      /* Primary DNS address */
      if (ip_info->dnsp_addr_s.valid_addr) 
      {
        inet_ntop(AF_INET, &(ip_info->dnsp_addr_s.addr.v4), dns_svr_buf, PRV_ADDRSTRLEN);
           
        /* Add the primary server on the interface */ 
        ret = qapi_Net_DNSc_Add_Server_on_iface(dns_svr_buf, QAPI_NET_DNS_ANY_SERVER_ID, ifname);  
        if (ret != QAPI_OK) 
        {
          ds_iot_ulog_err2("Adding primary DNS server failed for SSID[%d] with status[0x%x]", server_info->shortID, ret);
        }
      }

      /* Secondary DNS address */
      if (ip_info->dnss_addr_s.valid_addr) 
      {
        inet_ntop(AF_INET, &(ip_info->dnss_addr_s.addr.v4), dns_svr_buf, PRV_ADDRSTRLEN);

        /* Add the secondary server on the interface */
        ret = qapi_Net_DNSc_Add_Server_on_iface(dns_svr_buf, QAPI_NET_DNS_ANY_SERVER_ID, ifname);
        if (ret != QAPI_OK) 
        {
          ds_iot_ulog_err2("Adding secondary DNS server failed for SSID[%d] with status[0x%x]", server_info->shortID, ret);
        }
      }  
    
      ds_iot_log_strf_high("Trying DNS resolve for host[%s] on iface[%s] using IPv4 server", host, ifname);
  
      /* Resolve the host name on the iface in which we added the servers */ 
      ret = qapi_Net_DNSc_Reshost_on_iface(host, &dnsInfo, ifname);
      qapi_Net_DNSc_Command(QAPI_NET_DNS_STOP_E);  
      if (ret != QAPI_OK) 
      {
        ds_iot_ulog_err1("DNS client resolution using IPv4 server failed with status[%d]", ret);
        goto resolve_err;
      } 

      ds_iot_log_strf_high("LWM2M server URL [%s] resolved to [%s] using IPv4 server", host, inet_ntop(AF_INET, &dnsInfo.a, res_host, PRV_ADDRSTRLEN));
    }
    else
    {
      /* Resolution of host name not needed - IP address passed */ 
      memcpy(res_host, host, strlen(host));
    }
  }
  else  /* Resolve the host information - IPv6 */ 
  {
    memset(&addr6, 0x00, sizeof(addr6));

    if (0 != inet_pton(AF_INET6, host, &addr6.sin_addr))
    {
      /* Start DNS client */
      ret = qapi_Net_DNSc_Command(QAPI_NET_DNS_START_E);
      if (ret != QAPI_OK) 
      {
        ds_iot_ulog_err1("DNS client start failed with status[%d]", ret);
        goto resolve_err;  
      }

      /* Primary DNS address */
      if (ip_info->dnsp_addr_s.valid_addr) 
      {
        inet_ntop(AF_INET6, &(ip_info->dnsp_addr_s.addr.v6_addr8), dns_svr_buf, PRV_ADDRSTRLEN);
           
        /* Add the primary server on the interface */ 
        ret = qapi_Net_DNSc_Add_Server_on_iface(dns_svr_buf, QAPI_NET_DNS_ANY_SERVER_ID, ifname);  
        if (ret != QAPI_OK) 
        {
          ds_iot_ulog_err2("Adding primary DNS server failed for SSID[%d] with status[0x%x]", server_info->shortID, ret);
        }
      }

      /* Secondary DNS address */
      if (ip_info->dnss_addr_s.valid_addr) 
      {
        inet_ntop(AF_INET6, &(ip_info->dnss_addr_s.addr.v6_addr8), dns_svr_buf, PRV_ADDRSTRLEN);

        /* Add the secondary server on the interface */
        ret = qapi_Net_DNSc_Add_Server_on_iface(dns_svr_buf, QAPI_NET_DNS_ANY_SERVER_ID, ifname);
        if (ret != QAPI_OK) 
        {
          ds_iot_ulog_err2("Adding secondary DNS server failed for SSID[%d] with status[0x%x]", server_info->shortID, ret);
        }
      }
    
      ds_iot_log_strf_high("Trying DNS resolve for host[%s] on iface[%s] using IPv6 server", host, ifname);

      /* Resolve the host name on the iface in which we added the servers */ 
      ret = qapi_Net_DNSc_Reshost_on_iface(host, &dnsInfo, ifname);
      qapi_Net_DNSc_Command(QAPI_NET_DNS_STOP_E);  
      if (ret != QAPI_OK) 
      {
        ds_iot_ulog_err1("DNS client resolution using IPv6 server failed with status[%d]", ret);
        goto resolve_err;
      } 

      ds_iot_log_strf_high("LWM2M server URL [%s] resolved to [%s] using IPv6 server", host, inet_ntop(AF_INET6, &dnsInfo.a, res_host, PRV_ADDRSTRLEN));
    }
    else
    {
      /* Resolution of host name not needed - IP address passed */ 
      memcpy(res_host, host, strlen(host));
    }
  }
  
  ret = ESUCCESS;

resolve_err:  
  if (ip_info)
    lwm2m_free(ip_info);

  return ret; 
}


#ifdef DTLS_SUPPORT
/**
 * @brief Read Cert files from EFS
 * @param name File name
 * @param buffer_ptr File buffer
 * @param buffer_size Buffer size
 * @return QAPI_OK or QAPI_ERROR
 */
static int m2m_cert_read_from_EFS_file(
    const char * name,
    void ** buffer_ptr,
    size_t * buffer_size
    )
{
  uint32 bytes_read;
  int efs_fd = QAPI_ERROR;
  struct qapi_FS_Stat_Type_s stat = {0};
  uint8_t *file_buf = NULL;

  if((!name) || (!buffer_ptr) || (!buffer_size)) {
    return QAPI_ERROR;
  }

  memset( &stat, 0, sizeof(struct qapi_FS_Stat_Type_s )); 
  if(qapi_FS_Open(name, QAPI_FS_O_RDONLY_E, &efs_fd) < 0) {
    ds_iot_log_strf_err("Opening EFS file %s failed", name);
    return QAPI_ERROR;
  } else {
    ds_iot_log_strf_high("Opening EFS file %s success", name);
  }

  if(qapi_FS_Stat_With_Handle(efs_fd, &stat) < 0) {
    ds_iot_log_strf_err("Reading EFS file %s failed", name);
    return QAPI_ERROR;
  }

  file_buf = calloc(1, stat.st_size + 4); /*Added 4 bytes extra for padding*/

  if(file_buf == NULL) {
    ds_iot_ulog_err("LWM2M_LOG: Memory Allocation FAILED");
    return QAPI_ERROR;
  }

  qapi_FS_Read(efs_fd, file_buf, stat.st_size, &bytes_read);
  if((bytes_read != stat.st_size) && (bytes_read == 0)) {
    lwm2m_free(file_buf);
    ds_iot_ulog_err("Reading EFS file error");
    return QAPI_ERROR;
  }

  *buffer_ptr = file_buf;
  *buffer_size = stat.st_size;

  qapi_FS_Close(efs_fd);

  return QAPI_OK;
}


/**
 * @brief      convert/store from cert_info
 * @param[in] cert_name certificate name(without extension)
 * @param[in] cert_info QAPI Cert Info; the buffer in cert_info will be free in the function;
 * @return    QAPI_OK or QAPI_ERROR
 */
static int lwm2m_ssl_convert_and_store(const char* cert_name, qapi_Net_SSL_Cert_Info_t* cert_info)
{
   char cert_path[MAX_CERT_NAME_LEN];
   qapi_Status_t ret = QAPI_OK; 

   if(cert_name == NULL || cert_info == NULL)
   	 return QAPI_ERROR;

   memset(cert_path, 0, MAX_CERT_NAME_LEN);
   snprintf(cert_path, MAX_CERT_NAME_LEN, "%s.bin", cert_name);
   /* Calling SSL QAPI to covert, encrypt and store in the secure location */

   ds_iot_log_strf_high("%s %s",__FUNCTION__, cert_path);

   ret = qapi_Net_SSL_Cert_Convert_And_Store(cert_info, (uint8_t*)cert_path);

  /*Freeing allocated memory*/
  if(cert_info->cert_Type == QAPI_NET_SSL_DI_CERT_E && 
  	 cert_info->info.di_cert.di_Cert_Buf  != NULL)
  {
    lwm2m_free(cert_info->info.di_cert.di_Cert_Buf);
	cert_info->info.di_cert.di_Cert_Buf  = 0;
    cert_info->info.di_cert.di_Cert_Size = 0;
  }
  else if(cert_info->cert_Type == QAPI_NET_SSL_PSK_TABLE_E && 
  	      cert_info->info.psk_Tbl.psk_Buf != NULL)
  {
    lwm2m_free(cert_info->info.psk_Tbl.psk_Buf);
    cert_info->info.psk_Tbl.psk_Buf = NULL;
    cert_info->info.psk_Tbl.psk_Size = 0;
  }
  else if(cert_info->cert_Type == QAPI_NET_SSL_CA_LIST_E && 
  	      cert_info->info.ca_List.ca_Info[0] != NULL && cert_info->info.ca_List.ca_Info[0]->ca_Buf != NULL)
  {
    lwm2m_free(cert_info->info.ca_List.ca_Info[0]->ca_Buf);
	cert_info->info.ca_List.ca_Info[0]->ca_Buf  = 0;
    cert_info->info.ca_List.ca_Info[0]->ca_Size = 0;  
  }
  else if(cert_info->cert_Type == QAPI_NET_SSL_CERTIFICATE_E && 
  	      cert_info->info.cert.cert_Buf != NULL && cert_info->info.cert.key_Buf != NULL)
  {
    lwm2m_free(cert_info->info.cert.cert_Buf);
    lwm2m_free(cert_info->info.cert.key_Buf);
    cert_info->info.cert.cert_Buf = NULL;
    cert_info->info.cert.key_Buf  = NULL;
    cert_info->info.cert.cert_Size = 0;
    cert_info->info.cert.key_Size  = 0;
  }
  
  if(ret != QAPI_OK)
  {
	ds_iot_ulog_high1("lwm2m_ssl_convert_and_store: failied ret=%d", ret);
	return QAPI_ERROR;
  }

  return ret;

}

/**
 * @brief     lwm2m_ssl_convert_and_store_from_file:
 *            read cert/key from given path, and convert/store to SSL
 * @param[in] cert_type certificate type
 * @param[in] cert_path certificate file path
 * @param[in] key_path  key file path
 * @param[in] ssl_cert_name  certificate name to stored in SSL
 
 * @return    QAPI_OK or QAPI_ERROR
 */
qapi_Status_t lwm2m_ssl_convert_and_store_from_file(qapi_Net_SSL_Cert_Type_t cert_type, const char* cert_path, const char* key_path, const char* ssl_cert_name)
{
   qapi_Status_t ret = QAPI_OK;   
   void* cert_buf = 0, *key_buf  = 0;
   size_t cert_buf_len = 0, key_buf_len = 0;
   qapi_Net_SSL_Cert_Info_t cert_info ;   
   
   if(cert_path == NULL || ssl_cert_name == NULL)
   	 return QAPI_ERROR;

   if(cert_type == QAPI_NET_SSL_CERTIFICATE_E && key_path == NULL)
   	 return QAPI_ERROR;

   ds_iot_log_strf_high("Read Cert/PSK from %s", cert_path);
   if(cert_type == QAPI_NET_SSL_CERTIFICATE_E)
     ds_iot_log_strf_high("Read Key from %s", key_path);
   
   /* read from *.der file, if it is certificate then read *_key.der too*/
   if((m2m_cert_read_from_EFS_file(cert_path, &cert_buf, &cert_buf_len ) != QAPI_OK) || 
   	  ((cert_type == QAPI_NET_SSL_CERTIFICATE_E) && (m2m_cert_read_from_EFS_file(key_path, &key_buf, &key_buf_len ) != QAPI_OK)))
   {
     /* certificate or key read failure, 
        then free cert buf if is malloced*/
     if(cert_buf != NULL)
     {
   	   lwm2m_free(cert_buf);
	      cert_buf = NULL;
     }
     if(key_buf != NULL)
     {
	      lwm2m_free(key_buf);
	      key_buf = NULL;
     }	 
	    return QAPI_ERROR;
   }   

   cert_info.cert_Type = cert_type;
   switch(cert_type )
   {
     case QAPI_NET_SSL_CA_LIST_E:
	      cert_info.info.ca_List.ca_Cnt = 1;
	      cert_info.info.ca_List.ca_Info[0] = calloc(1, sizeof(qapi_NET_SSL_CA_Info_t));

  	    if (cert_info.info.ca_List.ca_Info[0] != NULL) 
  	    {
  	      cert_info.info.ca_List.ca_Info[0]->ca_Buf  = cert_buf;
  	      cert_info.info.ca_List.ca_Info[0]->ca_Size = cert_buf_len;
  	    }
       break;

     case QAPI_NET_SSL_CERTIFICATE_E:
       cert_info.info.cert.cert_Buf  = cert_buf;
       cert_info.info.cert.cert_Size = cert_buf_len;
       cert_info.info.cert.key_Buf   = key_buf;
       cert_info.info.cert.key_Size  = key_buf_len;
	   cert_info.info.cert.pass_Key  = NULL;
       break;

     case QAPI_NET_SSL_DI_CERT_E:
       cert_info.info.di_cert.di_Cert_Buf  = (uint8_t *)cert_buf;
       cert_info.info.di_cert.di_Cert_Size = cert_buf_len;
       break;

     case QAPI_NET_SSL_PSK_TABLE_E:
       cert_info.info.psk_Tbl.psk_Size = cert_buf_len;
       cert_info.info.psk_Tbl.psk_Buf  = cert_buf; 	 	
       break; 

	    default: 
	      ret = QAPI_ERROR;
	      break;
   }	

  ret = lwm2m_ssl_convert_and_store(ssl_cert_name, &cert_info);
  if(ret == QAPI_OK)
  {
    efs_unlink(cert_path);
    if(cert_type == QAPI_NET_SSL_CERTIFICATE_E)
      efs_unlink(key_path);
  }
  
  return ret;
}

/**
 * @brief function: lwm2m_ssl_read_cert_and_key:
 *
 *        Read Cert(and Key) from AFS, read priority is: *.der > *.pem 
 *        if not present then check whether it is in security location
 *         
 * @param [in] cert_name: certificate name, without extenstion name;
 * @param [in] cert_type: certificate type
 * @param [out]cert_info: certificate info need to be store/convert; if no value be set, then no need convert/store
 *
 * @return QAPI_OK or QAPI_ERROR
 */
qapi_Status_t lwm2m_ssl_read_cert_and_key(const char* cert_name, qapi_Net_SSL_Cert_Type_t cert_type)
{
   qapi_Status_t ret = QAPI_OK;   
   char cert_path[MAX_CERT_NAME_LEN];
   char key_path [MAX_CERT_NAME_LEN];

   if(cert_name == NULL)
   	 return QAPI_ERROR;
   
   memset(cert_path, 0, MAX_CERT_NAME_LEN);
   memset(key_path, 0, MAX_CERT_NAME_LEN);
   
   snprintf(cert_path, MAX_CERT_NAME_LEN, "%s%s.der",     ssl_path, cert_name);   
   snprintf(key_path , MAX_CERT_NAME_LEN, "%s%s_key.der", ssl_path, cert_name);

   if(QAPI_ERROR == lwm2m_ssl_convert_and_store_from_file(cert_type, cert_path, key_path, cert_name))
   {
     /* read from *.pem file, if it is certificate then read *_key.pem too*/
     snprintf(cert_path, MAX_CERT_NAME_LEN, "%s%s.pem",     ssl_path, cert_name);
     snprintf(key_path,  MAX_CERT_NAME_LEN, "%s%s_key.pem", ssl_path, cert_name);	 
     if(QAPI_ERROR == lwm2m_ssl_convert_and_store_from_file(cert_type, cert_path, key_path, cert_name))
     {
	   /*check whether certificate in security place*/
       memset(cert_path, 0, MAX_CERT_NAME_LEN);	   
       snprintf(cert_path, MAX_CERT_NAME_LEN, "%s.bin", cert_name);   
	   if(qapi_Net_SSL_Cert_File_Exists(cert_path, cert_type) != QAPI_OK)
	   {
         ds_iot_log_strf_high("Cert %s not found in both AFS and Security", cert_name);	   
		 return QAPI_ERROR;
	   }
	   else
	   {
         ds_iot_log_strf_high("use existing cert %s (since no Cert in AFS)", cert_path);	   
	     return QAPI_OK;
	   }       
     }
   }	

   return ret;
}


/**
 * @fn void lwm2m_ssl_ca_cert_store()
 * @brief This function stores LWM2M raw rootCA  
 *        certificates files in to secure location in encrypted
 *        form.
 * @param instance -security_instance_t *
 * @param ShortID of the server
 * @return on success QAPI_OK
 *         on failure QAPI_ERROR
 */
int lwm2m_ssl_ca_cert_store(uint16_t shortID)
{
  qapi_Status_t ret = QAPI_ERROR;  
  char ca_list_fname[MAX_CERT_NAME_LEN]; 
	
  /* Reading Root certificates(CA List) for coresponding each ShortID.
     CA List never transfered/get from bootstrapping and those are provisioned
     during the factory setup. Root certificates need to be protected. The naming
     convension for root certificates to be prvovisioned is <shortId_ca_list.pem> */
  memset(ca_list_fname, 0, MAX_CERT_NAME_LEN);
  snprintf(ca_list_fname, MAX_CERT_NAME_LEN, "%hu_ca_list", shortID);

  /*  priority:  <shortid>_ca_list.* > lwm2m_ca_list.* */   
  ret = lwm2m_ssl_read_cert_and_key(ca_list_fname, QAPI_NET_SSL_CA_LIST_E);
  if(ret != QAPI_OK) 
  {
    ret = lwm2m_ssl_read_cert_and_key("lwm2m_ca_list", QAPI_NET_SSL_CA_LIST_E);
  }
  return ret;
}

/**
 * @fn void lwm2m_ssl_di_cert_store()
 * @brief This function stores LWM2M raw domain issued 
 *        certificates files in to secure location in encrypted
 *        form.
 * @param instance -security_instance_t *
 * @param ShortID of the server
 * @return on success QAPI_OK
 *         on failure QAPI_ERROR
 */
int lwm2m_ssl_di_cert_store(security_instance_t *instance, uint16_t shortID)
{
  qapi_Status_t ret = QAPI_OK;
  char cert_name[MAX_CERT_NAME_LEN];

  if(instance == NULL )
    return QAPI_ERROR;

  /* Domain Issued Certificate Mode Support*/
  memset(cert_name, 0, MAX_CERT_NAME_LEN);
  snprintf((char *)cert_name, MAX_CERT_NAME_LEN, "%hu_server_cert", shortID);

  /* Domain issued certificate for server mode*/
  if( (instance->serverPublicKeyLen > 0) && (instance->serverPublicKey != NULL)) 
  {
    qapi_Net_SSL_Cert_Info_t cert_info ;   
    memset(&cert_info, 0x00, sizeof(qapi_Net_SSL_Cert_Info_t));

	ds_iot_ulog_high("Server Certificate Mode client Initiated bootstrapping");
	cert_info.cert_Type = QAPI_NET_SSL_DI_CERT_E;
    cert_info.info.di_cert.di_Cert_Buf  = (uint8_t *)lwm2m_calloc(1, instance->serverPublicKeyLen);
    cert_info.info.di_cert.di_Cert_Size = instance->serverPublicKeyLen;

    if( NULL == cert_info.info.di_cert.di_Cert_Buf) return QAPI_ERROR; 

    memcpy(cert_info.info.di_cert.di_Cert_Buf, instance->serverPublicKey, instance->serverPublicKeyLen);

    ret = lwm2m_ssl_convert_and_store(cert_name, &cert_info);
  }
  else 
  { 	
	ret = lwm2m_ssl_read_cert_and_key(cert_name, QAPI_NET_SSL_DI_CERT_E);
  }  

  if( ret == QAPI_ERROR)
  {
    ds_iot_log_strf_high("DI Cert(%s.bin) store failed", cert_name);
  } 
  else
  {
    ds_iot_log_strf_high("DI Cert(%s.bin) available", cert_name);
    snprintf(instance->serverCertFName, MAX_CERT_NAME_LEN, "%s.bin", cert_name);
  }  
    
  return ret;
}

int lwm2m_ssl_psk_cert_store(security_instance_t *instance, uint16_t shortID)
{
  qapi_Status_t ret = QAPI_OK;
  char cert_fname[MAX_CERT_NAME_LEN];

  if(instance == NULL )
    return QAPI_ERROR;

  memset(cert_fname, 0, MAX_CERT_NAME_LEN);
  snprintf((char *)cert_fname, MAX_CERT_NAME_LEN, "%hu_psk", shortID);

  /* Check if the secutiry instance has the valid length for the secret key and public identity
     This will happen during the bootstrapping time */
  if( (instance->publicIdLen  > 0) && (instance->publicIdentity != NULL) &&    
	  (instance->secretKeyLen > 0) && (instance->secretKey      != NULL)) 
  {
    qapi_Net_SSL_Cert_Info_t cert_info;  /*SSL Cert info holds PEM/RAW psk or cert infos */
    int i = 0, id_len = 0;
    int offset = 0;
    int buf_size = 0;
	
    memset(&cert_info, 0x00, sizeof(qapi_Net_SSL_Cert_Info_t));
	
    cert_info.cert_Type = QAPI_NET_SSL_PSK_TABLE_E;
  
    ds_iot_ulog_high1("PSK Client initiated bootstrapping for ServerID: %d", shortID);
   /* Handling PSK Mode*/
    buf_size = instance->publicIdLen + (instance->secretKeyLen *2) + 4;
    cert_info.info.psk_Tbl.psk_Buf = lwm2m_calloc(1, buf_size);
    if( NULL == cert_info.info.psk_Tbl.psk_Buf) return QAPI_ERROR;

    id_len = snprintf((char *)cert_info.info.psk_Tbl.psk_Buf, (buf_size), "|%hs|:", instance->publicIdentity) ;

    /* Copying 2byte into psk_data_buf buffer */
    for(i = 0; i < instance->secretKeyLen; i++)
    {
      offset += snprintf((char *)(cert_info.info.psk_Tbl.psk_Buf + id_len + offset), ((buf_size) - id_len - offset),"%02hx", instance->secretKey[i] ) ;
    }
    cert_info.info.psk_Tbl.psk_Buf[id_len+offset] = '\n';
    cert_info.info.psk_Tbl.psk_Size = id_len+offset;

    ret = lwm2m_ssl_convert_and_store(cert_fname, &cert_info);
  }
  else
  {
    /* PSK file name from non-secure location during the bootstrapping. */
    char psk_fname_raw[MAX_CERT_NAME_LEN]; 
    ds_iot_ulog_high1("PSK Factory Bootstrapping for ServerID: %d", shortID);

    memset(psk_fname_raw, 0, MAX_CERT_NAME_LEN);
    snprintf(psk_fname_raw, MAX_CERT_NAME_LEN, "%s%hu_server.psk", ssl_path, shortID);

    /*read from default location and encrypt and store in secure location*/
    ret = lwm2m_ssl_convert_and_store_from_file(QAPI_NET_SSL_PSK_TABLE_E, psk_fname_raw, NULL, cert_fname);
	
    if(ret < QAPI_OK)
    {
      /*if there is no file in default location, search in secure location. 
        This may be needed only factory reset bootstrapping */
      snprintf(psk_fname_raw, MAX_CERT_NAME_LEN, "%hu_psk.bin", shortID);
      ret = qapi_Net_SSL_Cert_File_Exists(psk_fname_raw, QAPI_NET_SSL_PSK_TABLE_E);
      ds_iot_log_strf_high("Reading PSK File %s from the sec location", psk_fname_raw);  
    }
  }

  if( ret != QAPI_OK)
  {
    ds_iot_log_strf_high("Client PSK(%s.bin) not available", cert_fname);
  } 
  else
  {
    ds_iot_log_strf_high("Client PSK(%s.bin) available", cert_fname);
    snprintf(instance->clientCertFName, MAX_CERT_NAME_LEN,  "%s.bin", cert_fname);  
  }  

  return ret; 
}


/**
 * @fn void lwm2m_ssl_cert_store()
 * @brief This function stores LWM2M raw psk or 
 *        certificates files in to secure location in encrypted
 *        form.
 * @param instance -security_instance_t *
 * @param ShortID of the server
 * @return on success QAPI_OK
 *         on failure QAPI_ERROR
 */
int lwm2m_ssl_cert_store(security_instance_t *instance, uint16_t shortID)
{
  qapi_Status_t ret = QAPI_OK;
  char cert_fname[MAX_CERT_NAME_LEN];

  if(instance == NULL )
    return QAPI_ERROR;

  memset(cert_fname, 0, MAX_CERT_NAME_LEN);
  snprintf((char *)cert_fname, MAX_CERT_NAME_LEN, "%hu_cert", shortID);
  
  if( (instance->publicIdLen  > 0) && (instance->publicIdentity != NULL) && 
      (instance->secretKeyLen > 0) && (instance->secretKey      != NULL) ) 
  {
    qapi_Net_SSL_Cert_Info_t cert_info;  /*SSL Cert info holds PEM/RAW psk or cert infos */
    memset(&cert_info, 0x00, sizeof(qapi_Net_SSL_Cert_Info_t));

    ds_iot_ulog_high("Client Certificate Mode on client Initiated bootstrapping ");
    /* Extract the client certificate from LWM2M Sec object and copied to
       Cert_info structure for convertion and store them in secure location*/
    cert_info.cert_Type = QAPI_NET_SSL_CERTIFICATE_E;	
    cert_info.info.cert.cert_Buf = lwm2m_calloc(1, instance->publicIdLen);
    if( NULL == cert_info.info.cert.cert_Buf) return QAPI_ERROR; 
	
    cert_info.info.cert.key_Buf  = lwm2m_calloc(1, instance->secretKeyLen);	
    if( NULL == cert_info.info.cert.key_Buf)  
	{
	  lwm2m_free(cert_info.info.cert.cert_Buf);
	  return QAPI_ERROR; 
    }
	
    memcpy(cert_info.info.cert.cert_Buf, instance->publicIdentity, instance->publicIdLen);
    memcpy(cert_info.info.cert.key_Buf,  instance->secretKey,      instance->secretKeyLen);
	
    cert_info.info.cert.cert_Size = instance->publicIdLen;
    cert_info.info.cert.key_Size  = instance->secretKeyLen;   
    cert_info.info.cert.pass_Key  = NULL; /* LWM2M sec object doesn't support the encrypted private key.So passpharse of Private key is always NULL*/
    cert_info.cert_Type           = QAPI_NET_SSL_CERTIFICATE_E;

    /* convert Client Certificate into .BIN format */
    ret = lwm2m_ssl_convert_and_store(cert_fname, &cert_info);	  
  }
  else
  {
	ret = lwm2m_ssl_read_cert_and_key(cert_fname, QAPI_NET_SSL_CERTIFICATE_E);	  
  }

  if( ret == QAPI_ERROR)
  {
    ds_iot_log_strf_high("Client Cert(%s.bin) store failed", cert_fname);
  }
  else
  {
    ds_iot_log_strf_high("Client Cert(%s.bin) available", cert_fname);
    snprintf(instance->clientCertFName, MAX_CERT_NAME_LEN,  "%s.bin", cert_fname);  
  }

  return ret;
}

/**
 * @fn void lwm2m_ssl_cert_store()
 * @brief This function stores LWM2M raw psk or 
 *        certificates files in to secure location in encrypted
 *        form.
 * @param instance -security_instance_t *
 * @param ShortID of the server
 * @return on success QAPI_OK
 *         on failure QAPI_ERROR
 */
int lwm2m_ssl_security_store(security_instance_t *instance, uint16_t shortID)
{
  qapi_Status_t ret = QAPI_ERROR;

  if(instance == NULL )
    return QAPI_ERROR;

  if ( instance->securityMode == LWM2M_SECURITY_MODE_PRE_SHARED_KEY)
  { 
    ret = lwm2m_ssl_psk_cert_store(instance, shortID);
  }
  else if ( instance->securityMode == LWM2M_SECURITY_MODE_CERTIFICATE)
  {
    /* In LWM2M certificate mode, the device would have the following certificates for each
       LwM2M Server.
     
       1. Client Certificates
       2. Client Private key assoicated with Client Certificates.
          Client keys are very important crediential and need to be protected very securely.
       3. Server Certificates(It is called Domain Issued Certificates(DI Cert))
       4. Root Certificates(RootCA).
     
       Server Certificates(DI cert) or RootCA are used in Client side which authenicates the
       Server's certificates issued during the SSL Handshake. DI Cert or Root CA are mutualy
       exclusive and can't support both for a LWM2M Server.
    */
    /* RootCA or DI support needed or not, is configured from LWM2M config file */
    switch (data.cfg_info.lwm2m_rootca_support_mode) 
    {
       case LWM2M_ROOTCA_SUPPORT_BS_SERVERS:
          /* all LWM2M server supports DI Certs and Bootstrap in RootCA mode*/
          if (instance->isBootstrap == true) 
            ret = lwm2m_ssl_ca_cert_store(shortID);
          else
            ret = lwm2m_ssl_di_cert_store(instance, shortID);
          break;
        case LWM2M_ROOTCA_SUPPORT_LWM2M_SERVERS:
          /* all LWM2M server supports RootCA and Bootstrap in DI Certs mode*/
          if (instance->isBootstrap == true) 
            ret = lwm2m_ssl_di_cert_store(instance, shortID);
         else
            ret = lwm2m_ssl_ca_cert_store(shortID);
          break;
       case LWM2M_ROOTCA_SUPPORT_BS_LWM2M_SERVERS:
        /* all LWM2M server supports RootCA*/
         ret = lwm2m_ssl_ca_cert_store(shortID);
         break;
       default:
         /* By default LWM2M supports only DI Certificate rather than RootCA*/
         ret = lwm2m_ssl_di_cert_store(instance, shortID);
         break;
    }

    if (ret == QAPI_ERROR) {
      ds_iot_ulog_high2("RootCA or DI Cert store failed Mode:%d ShortID:%d ", 
                data.cfg_info.lwm2m_rootca_support_mode, shortID);
      return ret;
    }

    /* Above, the server certificates handled and stored successfuly. Now deal with
       Client certificate */
	ret = lwm2m_ssl_cert_store(instance, shortID);
  }
  else
  {
    ds_iot_ulog_err1("Security Mode: %d is not supported", instance->securityMode);
    return QAPI_ERROR;
  }
    
  return ret;
}

/**
 * @fn void lwm2m_configure_ssl_ca_list()
 * @brief API will configure ssl ca list parameters
 * @param ssl_cfg - qapi_coap_ssl_cfg *
 * @param shortID - uint16_t *
 * @return on success QAPI_OK
 *         on failure QAPI_ERROR
 */
int lwm2m_configure_ssl_ca_list(qapi_Coap_Sec_Info_t *ssl_cfg, uint16_t shortID)
{
  qapi_Status_t ret = QAPI_OK;
  if(ssl_cfg == NULL || ssl_cfg->ca_list == NULL)
  	return QAPI_OK;
  
  memset((void*)ssl_cfg->ca_list, 0, MAX_CERT_NAME_LEN);	
  snprintf((char *)ssl_cfg->ca_list, MAX_CERT_NAME_LEN, "%hu_ca_list.bin", shortID);
	
  ret = qapi_Net_SSL_Cert_File_Exists((char *)ssl_cfg->ca_list, QAPI_NET_SSL_CA_LIST_E);
  if(ret != QAPI_OK)
  {
	memset((void*)ssl_cfg->ca_list, 0, MAX_CERT_NAME_LEN);	  
	strlcpy((char *)ssl_cfg->ca_list, "lwm2m_ca_list.bin", MAX_CERT_NAME_LEN);	  
	ret = qapi_Net_SSL_Cert_File_Exists((char *)ssl_cfg->ca_list, QAPI_NET_SSL_CA_LIST_E);
  }
  
  ds_iot_log_strf_high("Certificate with RootCA Mode: Ca_list:%s;return %d", ssl_cfg->ca_list, ret);
  
  return ret;

}

/**
 * @fn void lwmwm2_configure_ssl()
 * @brief API will configure ssl parameters
 * @param instance -security_instance_t *
 * @param cfg - qapi_Net_SSL_Config_t *
 * @return on success QAPI_OK
 *         on failure QAPI_ERROR
 */
int lwm2m_configure_ssl(security_instance_t *instance, qapi_Net_SSL_Config_t *cfg, qapi_Coap_Sec_Info_t *ssl_cfg, uint16_t shortID)
{
  qapi_Status_t ret = QAPI_OK;
  qapi_Status_t ret_ca_list     = QAPI_OK;
  qapi_Status_t ret_client_cert = QAPI_OK;

  char ssl_cert[MAX_CERT_NAME_LEN] = {0};     //efs filename .PSK or .PEM
  char ssl_key[MAX_CERT_NAME_LEN]  = {0};    //efs filename key.pem 
  char ca_list[MAX_CERT_NAME_LEN]  = {0}; 
  char di_cert[MAX_CERT_NAME_LEN]  = {0}; 

  if((instance == NULL) || (cfg == NULL))
    return QAPI_ERROR;

  memset(cfg, 0, sizeof(qapi_Net_SSL_Config_t) );
  memset(ssl_cfg, 0, sizeof(qapi_Coap_Sec_Info_t));
  memset(ssl_cert, 0, MAX_CERT_NAME_LEN);
  memset(ssl_key, 0, MAX_CERT_NAME_LEN);
  memset(ca_list, 0, MAX_CERT_NAME_LEN);
  memset(di_cert, 0, MAX_CERT_NAME_LEN);

  //ssl_cfg->sec_mode = instance->securityMode;

  /* Assigning default values */

  cfg->verify.domain =  0;
  cfg->verify.match_Name[0] = '\0';
  cfg->verify.time_Validity = 1;
  cfg->verify.send_Alert = 1;
  cfg->max_Frag_Len = LWM2M_SERVERS_SSL_MAX_FRAG_LEN;

  /* Before make Secure connection, configure the SSL parameters and see
     Secuirty files are present in secure location.
     if the certificate or PSK file is not present in the secure location,
     It will try to load from the default location.
   
     */
  if ( instance->securityMode == LWM2M_SECURITY_MODE_PRE_SHARED_KEY)
  {
    cfg->protocol = QAPI_NET_SSL_PROTOCOL_DTLS_1_2;
    cfg->cipher[0] = QAPI_NET_TLS_PSK_WITH_AES_128_CCM_8;
    ssl_cfg->psk = calloc(1, MAX_CERT_NAME_LEN);

    if(ssl_cfg->psk == NULL)return QAPI_ERROR;

    snprintf((char *)ssl_cfg->psk, MAX_CERT_NAME_LEN, "%hu_psk.bin", shortID);

    /* During the bootup up, persistence files might have the encrypted PSK files. to configure */
    if(strncmp(instance->clientCertFName, ssl_cfg->psk, MAX_CERT_NAME_LEN) == 0)
    {
       return qapi_Net_SSL_Cert_File_Exists(instance->clientCertFName, QAPI_NET_SSL_PSK_TABLE_E);
    }
    else
    { 
      ds_iot_ulog_high("PSK mode: PSK files not exist in sec location and reading from default AFS");
      ret = lwm2m_ssl_security_store(instance, shortID);
      if (ret == QAPI_OK) {
        ds_iot_ulog_high("PSK mode: storing PSK info to persistence file");    
        ret = store_security_persistent_info(instance,LWM2M_TRUE);
      }
      return ret;
    }
  } 
  else if(instance->securityMode == LWM2M_SECURITY_MODE_CERTIFICATE)  
  {
    cfg->protocol = QAPI_NET_SSL_PROTOCOL_DTLS_1_2;
    cfg->cipher[0] = QAPI_NET_TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8;    /*OMA Spec defined Cipher Suite*/

    ssl_cfg->cert = calloc(1, MAX_CERT_NAME_LEN);
    if(ssl_cfg->cert == NULL) goto Error;

    /* Check the rootCA based on configuration.*/
    if(((data.cfg_info.lwm2m_rootca_support_mode == LWM2M_ROOTCA_SUPPORT_BS_SERVERS) && (instance->isBootstrap == true)) || 
       ((data.cfg_info.lwm2m_rootca_support_mode == LWM2M_ROOTCA_SUPPORT_LWM2M_SERVERS) && (instance->isBootstrap == false)) ||
       (data.cfg_info.lwm2m_rootca_support_mode == LWM2M_ROOTCA_SUPPORT_BS_LWM2M_SERVERS)) 
    {
      ssl_cfg->ca_list = calloc(1, MAX_CERT_NAME_LEN);
      if(ssl_cfg->ca_list == NULL) goto Error;

      ret_ca_list = lwm2m_configure_ssl_ca_list(ssl_cfg, shortID);

    }
    /* RootCA and domain issued certificates are mutualy exclusive. Need to check DI cert is present or not
     * if ROOTCA is configured
     */
    else if(((data.cfg_info.lwm2m_rootca_support_mode == LWM2M_ROOTCA_SUPPORT_BS_SERVERS) && (instance->isBootstrap == false)) || 
       ((data.cfg_info.lwm2m_rootca_support_mode == LWM2M_ROOTCA_SUPPORT_LWM2M_SERVERS) && (instance->isBootstrap == true)) ||
       (data.cfg_info.lwm2m_rootca_support_mode == LWM2M_ROOTCA_NO_SUPPORT)) 
    {
      ssl_cfg->di_cert = calloc(1, MAX_CERT_NAME_LEN);
      if(ssl_cfg->di_cert == NULL) goto Error;

      snprintf((char *)ssl_cfg->di_cert, MAX_CERT_NAME_LEN, "%hu_server_cert.bin", shortID);
      ds_iot_log_strf_high("Certificate with DI Cert Mode, serverCertFName:%s di_cert:%s", instance->serverCertFName, ssl_cfg->di_cert);
      if(strncmp(instance->serverCertFName, ssl_cfg->di_cert, MAX_CERT_NAME_LEN) == 0)
      {
        ret = qapi_Net_SSL_Cert_File_Exists(instance->serverCertFName, QAPI_NET_SSL_DI_CERT_E);
        if(ret != QAPI_OK)
        {
          ds_iot_ulog_high("Server Certificate is not exist in sec location");
        }
      }
    }

    /* Client certificates would be loaded along with RootCA or DI certs */
    {
   	  ssl_cfg->cert = calloc(1, MAX_CERT_NAME_LEN);
   	  if(ssl_cfg->cert == NULL) goto Error;
     
   	  snprintf((char *)ssl_cfg->cert, MAX_CERT_NAME_LEN, "%hu_cert.bin", shortID);
   	  ds_iot_log_strf_high("Client Certificate: clientCertFName:%s cert:%s", instance->clientCertFName, ssl_cfg->cert);
   	  if(strncmp(instance->clientCertFName, ssl_cfg->cert, MAX_CERT_NAME_LEN) == 0)
   	  {
   	    ret_client_cert = qapi_Net_SSL_Cert_File_Exists(instance->clientCertFName, QAPI_NET_SSL_CERTIFICATE_E);
   	    if(ret_client_cert != QAPI_OK)
   	    {
   		  ds_iot_ulog_high("Client Certificate is not exist in sec location");
   	    }
   	  }
   	  else
   	  {
   	    ret = QAPI_ERROR;
   	  }
    }

    /* If any certificates are not present, then check default location and then covert
     * and store in secure location.
     */
    if (ret != QAPI_OK || ret_client_cert != QAPI_OK || ret_ca_list != QAPI_OK) 
    {
      ds_iot_ulog_high("Certificate mode: Cert files not exist in sec location and reading from default AFS");    
      ret = lwm2m_ssl_security_store(instance, shortID);
      if (ret == QAPI_OK) 
      {
        ds_iot_ulog_high("Certificate mode: storing cert info to persistence file");    				
        store_security_persistent_info(instance,LWM2M_TRUE);

	       //re-config CA List if store success; in case it is not RootCA mode, function will do nothing and just return due to ca_list is NULL;
	       lwm2m_configure_ssl_ca_list(ssl_cfg, shortID);        
      }
      else
      {
        goto Error;
      }
    }
  }
  else
  {
    ds_iot_ulog_err1("LWM2M_ERROR : Security Mode is not supported %d", instance->securityMode);
    return QAPI_ERROR;
  }

  if(ret == QAPI_OK) return ret;
  
Error:

  if (ssl_cfg->psk != NULL) 
  {
   	lwm2m_free((void *)ssl_cfg->psk);
	   ssl_cfg->psk = NULL;
  }

  if (ssl_cfg->cert != NULL) 
  {
    lwm2m_free((void *)ssl_cfg->cert);
    ssl_cfg->cert = NULL;
  }

  if (ssl_cfg->di_cert != NULL)
  {
    lwm2m_free((void*)ssl_cfg->di_cert);
    ssl_cfg->di_cert = NULL;
  } 

  if (ssl_cfg->ca_list != NULL) 
  {
    lwm2m_free((void *)ssl_cfg->ca_list);
    ssl_cfg->ca_list = NULL;
  }

  return QAPI_ERROR;
}
#endif


uint8_t parse_uri_info(lwm2m_object_t *security_obj, uint16_t sec_obj_id, char **host, char **port)
{
  char *portP = NULL;
  char *hostP = NULL;
  char *uri   = NULL;   
  uint8_t result = 0x01;   
  
  /* Retrieve and validate the server URI associated with the security object instance */
  uri = get_server_uri(security_obj, sec_obj_id);
  if ((uri == NULL) || (strlen(uri) < 7)) /* strlen("coap://") */
    goto parse_err;

  /* Parse URI information which is in the following formats 
   *     coap://[host]:port
   *    coaps://[host]:port
   */

  /* Host information */ 
  if (!strncmp(uri, "coap://", 7))
    hostP = uri + 7;
  else if (!strncmp(uri, "coaps://", 8))
    hostP = uri + 8;
  else 
    goto parse_err;

  /* Port information */
  portP = strrchr(hostP, ':');
  if (portP == NULL) 
    goto parse_err;

  /* Remove any optional brackets */
  if (hostP[0] == '[')
  {
    hostP++;

    if (*(portP - 1) == ']')
      *(portP - 1) = 0;
    else 
      goto parse_err;
  }

  /* Remove the ':' and split the host and port information */
 *portP = '\0';
  portP++;

  /* Allocate resources and copy the host and port information */
  *host = malloc(strlen(hostP) + 1);
  if (NULL == *host)
    goto parse_err; 

  /* Copy the host information */
  *port = malloc(strlen(portP) + 1);
  if (NULL == *port)
    goto parse_err; 

  strlcpy(*host, hostP, strlen(hostP) + 1);
  strlcpy(*port, portP, strlen(portP) + 1);

  result = 0x00; 
 
parse_err: 
  if (uri)
    lwm2m_free(uri);

  if (result)
  {
    if (*port)
       lwm2m_free(*port);

    if (*host)
       lwm2m_free(*host);

   *port = NULL;
   *host = NULL; 
  } 

  return result;
}


/**
 * @fn int lwm2m_connect_server()
 * @brief   This function is used to create connection
 *
 * @param server_info handle to server object
 * @param context handle to user data
 *
 * @return 0 for fail, 1 for success
 */
int lwm2m_connect_server(void *server_info, void *context)
{
  client_data_t * dataP = NULL;
  char *host = NULL;
  char *port = NULL;
  security_instance_t * instance = NULL;
  timer_error_type timer_result = TE_FAIL;
  int result = 0x00;
  char res_host[PRV_ADDRSTRLEN] = {0};
  qapi_Net_SSL_Config_t cfg;
  qapi_Coap_Sec_Info_t ssl_cfg;
  lwm2m_server_t server;
  carrier_apn_cfg_t *apn_cfg = NULL;
  carrier_apn_cfg_t *server_apn_cfg = NULL;  
  uint32_t mtu_size =0;
  
  /* Validate input parameters */ 
  if ((NULL == server_info) || (NULL == context))
  {
    LOG_DEBUG("Invalid input parameters - server_info[0x%x], context[0x%x]", server_info, context);
    return 0x00;
  }

  /* Copy the server info and update the copy. Only on success, update the original server info. */
  server = *((lwm2m_server_t *)server_info);

  apn_cfg = ((lwm2m_context_t *)context)->apn_cfg;
  if ((NULL == apn_cfg) ||
      (NULL == (server_apn_cfg = (carrier_apn_cfg_t *) LWM2M_LIST_FIND(apn_cfg, server.shortID)))) 
       
  {
    ds_iot_ulog_err("APN configuration is not valid");
    return 0x00; 
  } 
  
  memset(&cfg, 0x00, sizeof(qapi_Net_SSL_Config_t));
  memset(&ssl_cfg, 0x00, sizeof(qapi_Coap_Sec_Info_t));

  /* LwM2M client context */ 
  dataP = (client_data_t *)((lwm2m_context_t *)context)->userData;
 
  instance = (security_instance_t *) LWM2M_LIST_FIND(dataP->securityObjP->instanceList, server.secObjInstID);
  if (instance == NULL) 
    goto connect_err;

  timer_result = TE_FAIL;
  if (server.timer_init == false) 
  {
    timer_undef(&server.regOpTimer);
    timer_result = timer_def_osal(&server.regOpTimer, &lwm2mTimerGroup, TIMER_NATIVE_OS_SIGNAL_TYPE, &lwm2m_signal_Handle, LWM2M_TRIGGER_DISABLE);
    if (timer_result == TE_SUCCESS)
      server.timer_init = true;
  }
  
  /* Perform necessary initializations and retrieve the host and port information */
  server.conn_failed_with_v6 = LWM2M_FALSE;
  
  /* Parse URI information and retrieve the host and port information */ 
  if (parse_uri_info(dataP->securityObjP, instance->instanceId, &host, &port))
  {
    ds_iot_ulog_err1("Parsing the URI information for SSID [%d] failed", server.shortID);
    goto connect_err;
  } 
   
  ds_iot_log_strf_high("Creating COAP connection to %s : %s for SSID[%d]", host, port, server.shortID);
 
  while (true)
  {
    char server_ip[PRV_ADDRSTRLEN] = {0};
    qapi_Coap_Session_Info_t session_param;
    qapi_Coap_Connection_Cfg_t connection_params;
 
    memset(&session_param, 0x00, sizeof(qapi_Coap_Session_Info_t));
    memset(&connection_params, 0x00, sizeof(qapi_Coap_Connection_Cfg_t));  
 
    /* Retrieving the IP address information associated with the server (dummy for NON IP) */ 
    if (LWM2M_400_BAD_REQUEST == get_ip_for_carrier_apn_cfg(apn_cfg, &server, server_ip))
    {
      ds_iot_ulog_err1("Retrieving IP address associated with SSID [%d] failed", server.shortID);
      goto connect_err;
    }
    if(get_mtu_for_carrier_apn_cfg(apn_cfg, &server, &mtu_size) ==
          LWM2M_400_BAD_REQUEST)
    {
        mtu_size = DEFAULT_MTU_SIZE;
    }
    server.mtu_size = mtu_size;

    if (server.coapHandle == NULL) 
    {
      /* Update the CoAP context with the server information */ 

      session_param.coap_ack_random_factor = QAPI_COAP_ACK_RANDOM_FACTOR;
      session_param.coap_default_maxage = QAPI_COAP_DEFAULT_MAX_AGE;
      session_param.coap_max_latency = QAPI_COAP_MAX_LATENCY;
      session_param.coap_max_retransmits = QAPI_COAP_MAX_RETRANSMIT;
      session_param.coap_transaction_timeout = QAPI_COAP_RESPONSE_TIMEOUT;
      session_param.clientData  = context;
      session_param.cb          = lwm2m_handle_request_cb;
 
      /* Create CoAP session */ 
      qapi_Coap_Create_Session(&(server.coapHandle), &session_param);
      if (server.coapHandle == NULL)
      {
        LOG_DEBUG("CoAP session creation failed");
        goto connect_err;
      }

       if (qapi_Coap_Register_Event(server.coapHandle, lwm2m_Coap_Event_Callback) != QAPI_OK)
      {
	    ds_iot_ulog_high("qapi_Coap_Register_Event returned error ");
      }
	  else
	  {
          ds_iot_ulog_high("qapi_Coap_Register_Event returned success ");
	  }
    } 

    /* Resolve the host address. Skip resolution for Non IP cases as the connection exists immediately 
     * after the data call is brought-up.
     */ 
    if (AF_NONIP != server.local_ip.sa_family && server.dns_not_required == FALSE)
    {
      result = resolve_host_to_ip(apn_cfg, &server, host, res_host);
      if (result)
      {
        if ((server.ip_family_configured == AF_INET_DUAL46) && 
            (server.local_ip.sa_family == AF_INET6))
        {
          ds_iot_log_strf_err("Resolving host address %s : %s for SSID[%d] using IPv6 server failed.. retrying with IPv4 (Dual-IP)", host, port, server.shortID);
         
          /* Re-try with IPv4 */
          server.conn_failed_with_v6 = LWM2M_TRUE;
           
          /* Deregister with CoAP client */
          qapi_Coap_Destroy_Session(server.coapHandle);
          server.coapHandle = NULL;
   
          continue;
        }
        else
        {
          result = 0x00;
          ds_iot_log_strf_err("Creating CoAP connection to %s : %s for SSID[%d] failed", host, port, server.shortID);      
          goto connect_err; 
        }
      }
    }
    else if(AF_NONIP != server.local_ip.sa_family && server.dns_not_required == TRUE)
    {
      /* IPv4 family */ 
      if (AF_INET == server.dest_ip.sa_family)
      {
        inet_ntop(AF_INET, &server.dest_ip.sa_data, res_host, PRV_ADDRSTRLEN);
      }
      /* IPv6 family */ 
      else if (AF_INET6 == server.dest_ip.sa_family)
      {
        inet_ntop(AF_INET6, &server.dest_ip.sa_data, res_host, PRV_ADDRSTRLEN);
      }
      server.dns_not_required = FALSE;
    }

    connection_params.dst_host = res_host;
    connection_params.dst_port = atoi(port);
    connection_params.family_type = server.local_ip.sa_family;
    connection_params.resumption_enabled = TRUE;
    connection_params.sec_info = NULL;
    connection_params.sec_Mode = (qapi_Sec_Mode)instance->securityMode;
    connection_params.session_resumption_timeout = dataP->cfg_info.session_resumption_time;
    connection_params.src_host = server_ip;
    connection_params.src_port = 0;
    connection_params.ssl_cfg = NULL;	
    if (data.cfg_info.enable_dtls_close_notify == FALSE)
      connection_params.disable_close_notify = TRUE;
    /* Config would be considered for VzW case only.*/
    if (g_carrier_cfg)
    {
      uint8_t val = data.cfg_info.enable_dtls_epoch_0;
      qapi_Coap_Set_Extended_Config_Option(server.coapHandle, QAPI_COAP_EXTENDED_CONFIG_DTLS_HANDLE_ALERT_WITH_EPOCH_0,
                                           &val, 1);
    }
    else
    {
      uint8_t val = 0;
      qapi_Coap_Set_Extended_Config_Option(server.coapHandle, QAPI_COAP_EXTENDED_CONFIG_DTLS_HANDLE_ALERT_WITH_EPOCH_0,
                                           &val, 1);
    }

    /* Security configured */
    if (LWM2M_SECURITY_MODE_NONE != instance->securityMode)
    {
      ds_iot_ulog_high2("Configuring SSL with security mode[%d] for SSID[%d]", instance->securityMode, server.shortID);
 
      if (QAPI_ERROR == lwm2m_configure_ssl(instance, &cfg, &ssl_cfg, server.shortID))
      {
		ds_iot_ulog_err2("Configuring SSL with security mode[%d] failed for SSID[%d]", instance->securityMode, server.shortID);
		result = 0x00;    
        goto connect_err; 
      }
      else
      {
        uint32_t data_hndl = (uint32_t)get_data_call_hndl((AF_INET6 == server.local_ip.sa_family) ? server_apn_cfg->callHdl_v6 : server_apn_cfg->callHdl_v4_nip);

        /* Establish the CoAP connection with security enabled */
        connection_params.callHndl = data_hndl;
        connection_params.sec_info = &ssl_cfg;
        connection_params.ssl_cfg  = &cfg;
 
        /* Establish the CoAP connection with security enabled */ 
        result = qapi_Coap_Create_Connection(server.coapHandle, &connection_params);
        ds_iot_ulog_high3("CoAP connection with security mode[%d] and status[%d] for SSID[%d]",instance->securityMode, result, server.shortID);
      }
    }
    else
    {
      uint32_t data_hndl = (uint32_t)get_data_call_hndl((AF_INET6 == server.local_ip.sa_family) ? server_apn_cfg->callHdl_v6 : server_apn_cfg->callHdl_v4_nip);

      /* Establish the CoAP connection with security disabled */
      connection_params.callHndl = data_hndl;

      /* Creat CoAP connection */
      result = qapi_Coap_Create_Connection(server.coapHandle, &connection_params);
      ds_iot_ulog_high2("CoAP connection without security status[%d] for SSID[%d]", result, server.shortID);
    }
    
    active_time = lwm2m_gettime();
 
    /* Connecting to the server failed */
    if (result != QAPI_OK) 
    {      
      if ((server.ip_family_configured == AF_INET_DUAL46) && 
          (server.local_ip.sa_family == AF_INET6))
      {
        ds_iot_log_strf_err("Creating CoAP connection to %s : %s for SSID[%d] failed.. retrying with IPv4 (Dual-IP)", host, port, server.shortID);
       
        /* Re-try with IPv4 */
        server.conn_failed_with_v6 = LWM2M_TRUE;
         
        /* Deregister with CoAP client */
        qapi_Coap_Destroy_Session(server.coapHandle);
        server.coapHandle = NULL;
 
        continue;
      }
      else
      {
        result = 0x00; 
        ds_iot_log_strf_err("Creating CoAP connection to %s : %s for SSID[%d] failed", host, port, server.shortID);      
        goto connect_err; 
      }
    } 
    else 
    {
      ds_iot_log_strf_high("Created CoAP connection to %s : %s for SSID[%d] successfully", host, port, server.shortID);
 
      /* Cache the IP address information (would be used for comparision in case of re-connection) */
      if (AF_NONIP != server.local_ip.sa_family)
      {
        struct sockaddr current_ip;

        memset(&current_ip, 0x00, sizeof(struct sockaddr));

        /* IPv4 family */ 
        if (AF_INET == server.local_ip.sa_family)
        {
          struct sockaddr_in *sa = (struct sockaddr_in *)&current_ip;
          inet_pton(AF_INET, server_ip, &sa->sin_addr);
        }
        /* IPv6 family */ 
        else if (AF_INET6 == server.local_ip.sa_family)
        {
          struct sockaddr_in6 *sa6 = (struct sockaddr_in6 *)&current_ip;
          inet_pton(AF_INET6, server_ip, &sa6->sin_addr);
        }

        /* Update the cache with the newly acquired address */
        memset(server.local_ip.sa_data, 0x00, sizeof(server.local_ip.sa_data)); 
        memcpy(server.local_ip.sa_data, current_ip.sa_data, sizeof(server.local_ip.sa_data));

        memset(&current_ip, 0x00, sizeof(struct sockaddr));
        /* IPv4 family */ 
        if (AF_INET == server.local_ip.sa_family)
        {
          struct sockaddr_in *sa = (struct sockaddr_in *)&current_ip;
          inet_pton(AF_INET, res_host, &sa->sin_addr);
        }
        /* IPv6 family */ 
        else if (AF_INET6 == server.local_ip.sa_family)
        {
          struct sockaddr_in6 *sa6 = (struct sockaddr_in6 *)&current_ip;
          inet_pton(AF_INET6, res_host, &sa6->sin_addr);
        }

        /* Update the cache with the newly acquired address */
        memset(server.dest_ip.sa_data, 0x00, sizeof(server.dest_ip.sa_data)); 
        server.dest_ip.sa_family = server.local_ip.sa_family;
        memcpy(server.dest_ip.sa_data, current_ip.sa_data, sizeof(server.dest_ip.sa_data));
      }
      
      if((((lwm2m_server_t *)server_info)->timer_init == FALSE) && (server.timer_init == TRUE))
	  {
        memcpy(server_info, &server, sizeof(lwm2m_server_t));
	  }
	  else
      {
        /* Update the server resource with the updated information (as it is successful) */ 
        memcpy(server_info, &server, (sizeof(lwm2m_server_t) - sizeof(timer_type)));
      }
      break;
    }
  }

  result = 0x01; /* Success */ 
 
connect_err:
  {
    if(server.coapHandle != NULL && result != 0x01)
    {
	  /* Deregister with CoAP client */
      qapi_Coap_Destroy_Session(server.coapHandle);
      server.coapHandle = NULL;
    }
    /* Release the associated resources */ 
    if (ssl_cfg.psk != NULL)
    {
      lwm2m_free((void*)ssl_cfg.psk);
      ssl_cfg.psk = NULL;
    }
   
    if (ssl_cfg.cert != NULL)
    {
      lwm2m_free((void*)ssl_cfg.cert);
      ssl_cfg.cert = NULL;
    } 

    if (ssl_cfg.di_cert != NULL)
    {
      lwm2m_free((void*)ssl_cfg.di_cert);
      ssl_cfg.di_cert = NULL;
    } 
        
    if (ssl_cfg.ca_list!= NULL)
    {
      lwm2m_free((void*)ssl_cfg.ca_list);
      ssl_cfg.ca_list = NULL;
    }
   
    if (host)
      lwm2m_free(host); 
 
    if (port)
      lwm2m_free(port);   
  }
   
  return result;
}

int lwm2m_check_server_ip_change(lwm2m_server_t *server, void *context)
{
  client_data_t * dataP = NULL;
  char *host = NULL;
  char *port = NULL;
  security_instance_t * instance = NULL;
  int result = 0x00;
  char res_host[PRV_ADDRSTRLEN] = {0};
  carrier_apn_cfg_t *apn_cfg = NULL;

  /* Validate input parameters */ 
  if ((NULL == server) || (NULL == context))
  {
    LOG_DEBUG("Invalid input parameters - server_info[0x%x], context[0x%x]", server, context);
    return 0x00;
  }

  apn_cfg = ((lwm2m_context_t *)context)->apn_cfg;
  if ((NULL == apn_cfg)) 
       
  {
    ds_iot_ulog_err("APN configuration is not valid");
    return 0x00; 
  } 

  /* LwM2M client context */ 
  dataP = (client_data_t *)((lwm2m_context_t *)context)->userData;
 
  instance = (security_instance_t *)LWM2M_LIST_FIND(dataP->securityObjP->instanceList, server->secObjInstID);
  if (instance == NULL) 
    goto connect_err;

  /* Parse URI information and retrieve the host and port information */ 
  if (parse_uri_info(dataP->securityObjP, instance->instanceId, &host, &port))
  {
    ds_iot_ulog_err1("Parsing the URI information for SSID [%d] failed", server->shortID);
    goto connect_err;
  }

  while (true)
  {
    /* Resolve the host address. Skip resolution for Non IP cases as the connection exists immediately 
     * after the data call is brought-up.
     */ 
    if (AF_NONIP != server->dest_ip.sa_family)
    {
      result = resolve_host_to_ip(apn_cfg, server, host, res_host);
      if (result)
      {
        result = 0x00;
        ds_iot_log_strf_err("Creating CoAP connection to %s : %s for SSID[%d] failed", host, port, server->shortID);      
        goto connect_err; 
      }
    } 
    break;
  }

  result = 0x01; /* Success */ 
  if (AF_NONIP != server->dest_ip.sa_family && result == 0x01)
  {
    struct sockaddr current_ip;

    memset(&current_ip, 0x00, sizeof(struct sockaddr));

    /* IPv4 family */ 
    if (AF_INET == server->dest_ip.sa_family)
    {
      struct sockaddr_in *sa = (struct sockaddr_in *)&current_ip;
      inet_pton(AF_INET, res_host, &sa->sin_addr);
    }
    /* IPv6 family */ 
    else if (AF_INET6 == server->dest_ip.sa_family)
    {
      struct sockaddr_in6 *sa6 = (struct sockaddr_in6 *)&current_ip;
      inet_pton(AF_INET6, res_host, &sa6->sin_addr);
    }

    if(memcmp(current_ip.sa_data, server->dest_ip.sa_data, sizeof(server->dest_ip.sa_data)))
    {
      server->status = STATE_DEREGISTERED;
      /* Update the cache with the newly acquired address */
      memset(server->dest_ip.sa_data, 0x00, sizeof(server->dest_ip.sa_data));
      memcpy(server->dest_ip.sa_data, current_ip.sa_data, sizeof(server->dest_ip.sa_data));
      server->dns_not_required = TRUE;
      result = 0x01;
    }
    else
    {
      result = 0x00;
    }
  }
 
connect_err:
  {
    if (host)
      lwm2m_free(host); 
 
    if (port)
      lwm2m_free(port);   
  }
   
  return result;
}



/**
 * @fn static QCLI_Command_Status_t m2m_reboot()
 * @brief This function is used to reboot the device.
 *
 * @param Parameter_Count   Number of parameters passed from QCLI
 * @param Parameter_List    List of para received from QCLI
 *
 * @return On success, returns QCLI_STATUS_SUCCESS_E or QCLI_STATUS_USAGE_E
 *      On Failure, return QCLI_STATUS_ERROR_E
 */
#ifdef ENABLE_INTERNAL_BUILD
static QCLI_Command_Status_t m2m_reboot(uint32_t Parameter_Count,
    QCLI_Parameter_t* Parameter_List)
{
  sys_m_reset();
  return QCLI_STATUS_SUCCESS_E;
}


static QCLI_Command_Status_t set_thresholds(uint32_t Parameter_Count,
    QCLI_Parameter_t* Parameter_List)
{
  if (2 == Parameter_Count)
  {
    if (!strncmp((const char *)Parameter_List[0].String_Value, "-m", 2))
    {
      gMemThreshold = Parameter_List[1].Integer_Value;
    }
    else if (!strncmp((const char *)Parameter_List[0].String_Value, "-s", 2))
    {
      gSignalThreshold = Parameter_List[1].Integer_Value;
    }
  }
  else
  {
    QCLI_Printf(qcli_m2m_handle, "Invalid!!");
    return QCLI_STATUS_USAGE_E;
  }
  return QCLI_STATUS_SUCCESS_E;
}

#endif

void resource_value_change_update (lwm2m_uri_t * uriP)
{
  lwm2m_resource_value_changed(data.lwm2mH, uriP);
}

void resource_value_change(lwm2m_object_data_t *lwm2m_data)
{
  lwm2m_uri_t uriP = {0};
  if(!lwm2m_data || !(lwm2m_data->instance_info) || !(lwm2m_data->instance_info->resource_info))
    return;
  uriP.objectId   = lwm2m_data->object_ID;
  uriP.instanceId = lwm2m_data->instance_info->instance_ID ;
  uriP.resourceId = lwm2m_data->instance_info->resource_info->resource_ID;
  uriP.flag       = LWM2M_URI_FLAG_RESOURCE_ID | LWM2M_URI_FLAG_INSTANCE_ID | LWM2M_URI_FLAG_OBJECT_ID;

  resource_value_change_update(&uriP);
}
lwm2m_client_state_t lwm2m_get_client_state(void)
{
  if (data.lwm2mH != NULL)
    return data.lwm2mH->state;
  else
   	return STATE_INITIAL;
}

int32_t  lwm2m_remove_object_instance(uint16_t object_id ,uint16_t instance_id, bool api_call)
{
  lwm2m_object_t * targetP = NULL;
  lwm2m_context_t * contextP = NULL;
  int32_t result = QAPI_OK;
  lwm2m_uri_t uri;

  contextP = data.lwm2mH;
  if(contextP == NULL)
  {
    LOG_DEBUG("%s context", nullError);
    return -1;
  }
  memset(&uri, 0x00, sizeof(lwm2m_uri_t));

  /* Standard objects */
  if (api_call == true || lwm2m_is_standard_object(object_id))
  {
    targetP = (lwm2m_object_t *)LWM2M_LIST_FIND(contextP->objectList, object_id);
    if((NULL == targetP) || (!targetP->deleteFunc))
    {
      ds_iot_ulog_err("Function to delete the not registered");
      return -1;
    }
    result =  targetP->deleteFunc(instance_id,targetP, api_call);

	if(result == LWM2M_404_NOT_FOUND)
		return QAPI_ERR_NO_ENTRY;

    if(result != LWM2M_202_DELETED)
      return -1;

	result = QAPI_OK;
    uri.flag = (LWM2M_URI_FLAG_OBJECT_ID | LWM2M_URI_FLAG_INSTANCE_ID);
    uri.objectId = object_id;
    uri.instanceId = instance_id;
  
    observe_delete(contextP, &uri);
	if(api_call)
	  lwm2m_update_registration(data.lwm2mH, 0, true);
  }
  else
  {
    /* Custom objects */
    lwm2m_server_data_t lwm2m_info;

    /* Forward "delete" request to the extensible OEM application */
    memset(&lwm2m_info, 0x00, sizeof(lwm2m_server_data_t));
    lwm2m_info.msg_type = QAPI_NET_LWM2M_DELETE_REQ_E;
    
    /* Object info */
    lwm2m_info.obj_info.obj_id = object_id;
    lwm2m_info.obj_info.obj_mask |= QAPI_NET_LWM2M_OBJECT_ID_E;
    
    /* Object instance info */
    lwm2m_info.obj_info.obj_inst_id = instance_id;
    lwm2m_info.obj_info.obj_mask |= QAPI_NET_LWM2M_INSTANCE_ID_E;
    
    result = lwm2m_client_application_notification(&lwm2m_info);
    if (result)
      result = (uint8_t)LWM2M_500_INTERNAL_SERVER_ERROR;             
  }

  return result;
}

/*
 * @fn     int32_t lwm2m_create_object_instance()
 * @brief  This function is used to create instance on request by qapi interface 
 * @param  dataArrayP   - pointer to lwm2m resource information 
 *         object_id    - object id 
 instance_id  - instnace id of particular object 
 * @return on success   - 0
 on error     - -1      
 */
int32_t lwm2m_create_object_instance(lwm2m_data_t *dataArrayP ,uint16_t count , uint16_t object_id ,uint16_t instance_id)
{
  lwm2m_object_t * targetP = NULL;
  lwm2m_context_t * contextP = NULL;
  uint8_t result = 0;

  contextP = data.lwm2mH;
  if(contextP == NULL)
  {
    LOG_DEBUG("%s context", nullError);
    return -1;
  }

  if ((dataArrayP == NULL) && !lwm2m_is_standard_object(object_id))
  {
    ds_iot_ulog_err ("LWM2M_LOG: NULL Error");
    return -1;
  }

  targetP = (lwm2m_object_t *)LWM2M_LIST_FIND(contextP->objectList, object_id);
  if ((NULL == targetP) || (!targetP->createFunc))
  {
    ds_iot_ulog_err1("Create function for object ID[%d] is not registered", object_id);
    return -1;
  }

  if (NULL != lwm2m_list_find(targetP->instanceList, instance_id))
  {
    ds_iot_ulog_err2("Object instance ID[%d] already exists for object ID[%d]", instance_id, object_id);
    return QAPI_ERR_EXISTS;
  }

  result = targetP->createFunc(instance_id,count , dataArrayP ,targetP, TRUE);
  if (LWM2M_201_CREATED != result && (LWM2M_NO_ERROR != result && object_id == LWM2M_APN_CONN_PROFILE_OBJECT_ID))
  {
    ds_iot_ulog_err ("CREATE INSTANCE FAILED") ; 
    return -1;
  }   
  lwm2m_update_registration(contextP, 0, true);
  return 0;

}
/*
 * @fn     int32_t lwm2m_set_object_info()
 * @brief  This function is used to set the particular instance or resource info   
 * @param  lwm2m_data   - pointer to lwm2m object information 
 *         object_ID    - object id 
 * @return on success   - 0
 on error     - -1      
 */
int32_t lwm2m_set_object_info(lwm2m_object_data_t * lwm2m_data , uint16_t object_ID)
{
  lwm2m_object_t * targetP = NULL;
  lwm2m_context_t * contextP = data.lwm2mH;
  int32_t result = 0 ;

  if (lwm2m_data == NULL || contextP == NULL)
  {
    ds_iot_ulog_err("LWM2M_LOG: NULL Error");
    return -1;
  }

  targetP = (lwm2m_object_t *)LWM2M_LIST_FIND(contextP->objectList, object_ID);
  if((NULL == targetP) || (!targetP->setValueFunc))
  {
    ds_iot_ulog_err("Function to get the value is not registered");
    return -1;
  }
  result = targetP->setValueFunc(lwm2m_data, targetP);
  if( 1 != result)
  {
    return -1;
  }   

  return 0;
}


/**
 * @fn static QCLI_Command_Status_t roaming_signal()
 * @brief This function is used to send in_roaming and out_of_roaming signals.
 *
 * @param Parameter_Count   Number of parameters passed from QCLI
 * @param Parameter_List    List of para received from QCLI
 *
 * @return On success, returns QCLI_STATUS_SUCCESS_E or QCLI_STATUS_USAGE_E
 *      On Failure, return QCLI_STATUS_ERROR_E
 */
#ifdef ENABLE_INTERNAL_BUILD

QCLI_Command_Status_t roaming_signal(uint32_t Parameter_Count,
    QCLI_Parameter_t* Parameter_List)
{
  if (1 != Parameter_Count)
  {
    QCLI_Printf(qcli_m2m_handle, "Invalid!!");
    return QCLI_STATUS_USAGE_E;
  }
  if (0 == Parameter_List[0].Integer_Value)
  {
    QCLI_Printf(qcli_m2m_handle, "Sending in_roaming signal");
    qurt_signal_set(&lwm2m_signal_Handle, LWM2M_IN_ROAMING_IND);
  }
  else if (1 == Parameter_List[0].Integer_Value)
  {
    QCLI_Printf(qcli_m2m_handle, "Sending out_of_roaming signal");
    qurt_signal_set(&lwm2m_signal_Handle, LWM2M_OUT_OF_ROAMING_IND);
  }
  else
    return QCLI_STATUS_USAGE_E;
  return QCLI_STATUS_SUCCESS_E;
}


/**
 * @fn static QCLI_Command_Status_t simulate_smswakeup()
 * @brief This function is used to wakeup client.
 *
 * @param Parameter_Count   Number of parameters passed from QCLI
 * @param Parameter_List    List of para received from QCLI
 *
 * @return On success, returns QCLI_STATUS_SUCCESS_E or QCLI_STATUS_USAGE_E
 *      On Failure, return QCLI_STATUS_ERROR_E
 */
static QCLI_Command_Status_t simulate_smswakeup(uint32_t Parameter_Count,
    QCLI_Parameter_t* Parameter_List)
{
  if (2 < Parameter_Count || 0 == Parameter_Count)
  {
    QCLI_Printf(qcli_m2m_handle, "Invalid!!");
    return QCLI_STATUS_USAGE_E;
  }
  if (!strncmp("-s", (const char *)Parameter_List[0].String_Value, 2))
  {
    if (2 != Parameter_Count) return QCLI_STATUS_USAGE_E;
    sleepAckTimeout = Parameter_List[1].Integer_Value;
  }
  else if (!strncmp("-a", (const char *)Parameter_List[0].String_Value, 2))
  {
    if(2 < Parameter_Count) return QCLI_STATUS_USAGE_E;   
    active_time = lwm2m_gettime();
    lwm2m_in_sleep_state = false;
    ds_iot_ulog_high("Waking up from CLI");
    if(Parameter_Count == 1)
    {     
      if(data.lwm2mH && (data.lwm2mH->state == STATE_READY))
      {
        lwm2m_update_registration(data.lwm2mH, 0, FALSE);
      }
      else
      {
        QCLI_Printf(qcli_m2m_handle, "Device is not in Ready state. Ignoring Wakeup request.!!");
        return QCLI_STATUS_ERROR_E;
      }
    }  
    else
    {     
      if(data.lwm2mH && (data.lwm2mH->state == STATE_READY))
      {
        lwm2m_update_registration(data.lwm2mH, Parameter_List[1].Integer_Value, FALSE);
      }
      else
      {
        QCLI_Printf(qcli_m2m_handle, "Device is not in Ready state. Ignoring Wakeup request.!!");
        return QCLI_STATUS_ERROR_E;
      }
    }
  }
  return QCLI_STATUS_SUCCESS_E;
}


/**
 * @fn static QCLI_Command_Status_t simulate_smswakeup()
 * @brief This function is used to wakeup client.
 *
 * @param Parameter_Count   Number of parameters passed from QCLI
 * @param Parameter_List    List of para received from QCLI
 *
 * @return On success, returns QCLI_STATUS_SUCCESS_E or QCLI_STATUS_USAGE_E
 *      On Failure, return QCLI_STATUS_ERROR_E
 */
static QCLI_Command_Status_t sms_reg_update(uint32_t Parameter_Count,
    QCLI_Parameter_t* Parameter_List)
{
  uint8_t instanceId = 0;
  if (4 != Parameter_Count)
  {
    QCLI_Printf(qcli_m2m_handle, "Invalid!!");
    return QCLI_STATUS_USAGE_E;
  }
  if (!strncmp("-i", (const char *)Parameter_List[0].String_Value, 2))
  {
    instanceId = (uint8_t)Parameter_List[1].Integer_Value;
    if (!strncmp("-n", (const char *)Parameter_List[2].String_Value, 2))
    {
      memcpy(trans_msg_data, sms_msg_data_number1, sms_msg_datalen);
      trans_msg_data[sms_msg_datalen-3] = instanceId + 48;

      trans_msg_data[sms_msg_datalen] = '\0';
      trans_msg_datalen = sms_msg_datalen;
      routed_msg = true;
      trans_ack_indicator = WMS_ACK_INDICATOR_DO_NOT_SEND_ACK_V01;
      if (!strncmp("910090009001", (const char *)Parameter_List[3].String_Value, 12))
      {
        trans_msg_data[8] = 16;
      }
      if (!strncmp("910090009002", (const char *)Parameter_List[3].String_Value, 12))
      {
        trans_msg_data[8] = 32;
      }
      qurt_signal_set(&lwm2m_signal_Handle, WMS_MT_MESSAGE_IND);
    }
    return QCLI_STATUS_SUCCESS_E;
  }
  else
    return QCLI_STATUS_USAGE_E;
}

#ifdef WRITE_ATTR_TEST

static QCLI_Command_Status_t simulate_msisdn_change(uint32_t Parameter_Count,
    QCLI_Parameter_t* Parameter_List)
{
	if (1 != Parameter_Count)
	{
	  QCLI_Printf(qcli_m2m_handle, "Invalid MSISDN!!");
	  return QCLI_STATUS_USAGE_E;
	}

	
	if(0 != store_msisdn_to_persistence((char*)Parameter_List[0].String_Value))
	{
		QCLI_Printf(qcli_m2m_handle, "Invalid MSISDN!!");
		return QCLI_STATUS_USAGE_E;
	}

	return QCLI_STATUS_SUCCESS_E;
}

static int string_uri(const char * buffer,
                      size_t buffer_len,
                      lwm2m_uri_t * uriP)
{
  size_t head;
  int readNum;

  ds_iot_log_strf_high("buffer_len: %u, buffer: \"%.*s\"", buffer_len, buffer_len, buffer);

  if (buffer == NULL || buffer_len == 0 || uriP == NULL) return 0;

  memset(uriP, 0, sizeof(lwm2m_uri_t));

  // Skip any white space
  head = 0;
  while ((head < buffer_len) && lwm2m_isspace(buffer[head]&0xFF))
  {
    head++;
  }
  if (head == buffer_len) return 0;

  // Check the URI start with a '/'
  if (buffer[head] != '/') return 0;
  head++;
  if (head == buffer_len) return 0;

  // Read object ID
  readNum = prv_parseNumber((uint8_t *)buffer, buffer_len, &head);
  if (readNum < 0 || readNum > LWM2M_MAX_ID) return 0;
  uriP->objectId = (uint16_t)readNum;
  uriP->flag |= LWM2M_URI_FLAG_OBJECT_ID;

  if (buffer[head] == '/') head += 1;
  if (head >= buffer_len)
  {
    LOG_DEBUG("Parsed characters: %u", head);
    LOG_URI(uriP);
    return head;
  }

  readNum = prv_parseNumber((uint8_t *)buffer, buffer_len, &head);
  if (readNum < 0 || readNum >= LWM2M_MAX_ID) return 0;
  uriP->instanceId = (uint16_t)readNum;
  uriP->flag |= LWM2M_URI_FLAG_INSTANCE_ID;

  if (buffer[head] == '/') head += 1;
  if (head >= buffer_len)
  {
    LOG_DEBUG("Parsed characters: %u", head);
    LOG_URI(uriP);
    return head;
  }

  readNum = prv_parseNumber((uint8_t *)buffer, buffer_len, &head);
  if (readNum < 0 || readNum >= LWM2M_MAX_ID) return 0;
  uriP->resourceId = (uint16_t)readNum;
  uriP->flag |= LWM2M_URI_FLAG_RESOURCE_ID;

  if (buffer[head] == '/') head += 1;
  if (head >= buffer_len)
  {
    LOG_DEBUG("Parsed characters: %u", head);
    LOG_URI(uriP);
    return head;
  }

  readNum = prv_parseNumber((uint8_t *)buffer, buffer_len, &head);
  if (readNum < 0 || readNum >= LWM2M_MAX_ID) return 0;
  uriP->resourceInstId = (uint16_t)readNum;
  uriP->flag |= LWM2M_URI_FLAG_RESOURCE_INST_ID;

  if (head != buffer_len) return 0;

  LOG_DEBUG("Parsed characters: %u", head);
  LOG_URI(uriP);

  return head;
}

static QCLI_Command_Status_t clear_attributes(uint32_t Parameter_Count,
    QCLI_Parameter_t* Parameter_List)
{
  lwm2m_uri_t uri;
  int opt = -1;
  lwm2m_attributes_t attr;
  uint8_t result = LWM2M_NO_ERROR;

  if(Parameter_Count < 2)
  {
    QCLI_Printf(qcli_m2m_handle, "Please check usage. \n");
    return QCLI_STATUS_USAGE_E;
  }

  memset(&uri, 0x00, sizeof(lwm2m_uri_t));
  memset(&attr, 0x00, sizeof(lwm2m_attributes_t));

  if (string_uri((char *)Parameter_List[0].String_Value, strlen((void *)Parameter_List[0].String_Value ), &uri))
  {
    if(Parameter_Count > 1 && Parameter_List[1].Integer_Is_Valid)
    {
      opt = Parameter_List[1].Integer_Value;
    }

    if((opt & 0x10))
    {
      attr.toClear|= LWM2M_ATTR_FLAG_MIN_PERIOD;
    }
    if((opt & 0x08))
    {
      attr.toClear|= LWM2M_ATTR_FLAG_MAX_PERIOD;
    }
    if((opt & 0x04))
    {
      attr.toClear|= LWM2M_ATTR_FLAG_LESS_THAN;
    }
    if((opt & 0x02))
    {
      attr.toClear|= LWM2M_ATTR_FLAG_GREATER_THAN;
    }
    if((opt & 0x01))
    {
      attr.toClear|= LWM2M_ATTR_FLAG_STEP;
    }

    if(data.lwm2mH)
    {
      result = observe_setParameters(data.lwm2mH, &uri, data.lwm2mH->serverList ,&attr);
      if(result != LWM2M_204_CHANGED)
      {
        QCLI_Printf(qcli_m2m_handle,"Error in clearing attributes.\n");
        return QCLI_STATUS_ERROR_E;
      }
      else
      {
        QCLI_Printf(qcli_m2m_handle,"Attributes.cleared successfully. \n");
        return QCLI_STATUS_SUCCESS_E;
      }
    }
  }
  return QCLI_STATUS_ERROR_E;
}

static QCLI_Command_Status_t set_attributes(uint32_t Parameter_Count,
    QCLI_Parameter_t* Parameter_List)
{
  lwm2m_uri_t uri;
  int opt = -1;
  lwm2m_attributes_t attr;
  int i = 2;
  uint8_t result = LWM2M_NO_ERROR;

  if(Parameter_Count < 3)
  {
    QCLI_Printf(qcli_m2m_handle,"Please check usage. \n");
    return QCLI_STATUS_USAGE_E;
  }

  memset(&uri, 0x00, sizeof(lwm2m_uri_t));
  memset(&attr, 0x00, sizeof(lwm2m_attributes_t));

  if (string_uri((char *)Parameter_List[0].String_Value, strlen((void *)Parameter_List[0].String_Value ), &uri))
  {
    if(Parameter_Count > 1 && Parameter_List[1].Integer_Is_Valid)
    {
      opt = Parameter_List[1].Integer_Value;
    }

    if((opt & 0x10) && Parameter_Count > i && Parameter_List[i].Integer_Is_Valid)
    {
      attr.toSet |= LWM2M_ATTR_FLAG_MIN_PERIOD;
      attr.minPeriod = Parameter_List[i].Integer_Value;
      i++;
    }
    if((opt & 0x08) && Parameter_Count > i && Parameter_List[i].Integer_Is_Valid)
    {
      attr.toSet |= LWM2M_ATTR_FLAG_MAX_PERIOD;
      attr.maxPeriod= Parameter_List[i].Integer_Value;
      i++;
    }
    if((opt & 0x04) && Parameter_Count > i && Parameter_List[i].Integer_Is_Valid)
    {
      attr.toSet |= LWM2M_ATTR_FLAG_LESS_THAN;
      attr.lessThan= Parameter_List[i].Integer_Value;
      i++;
    }
    if((opt & 0x02) && Parameter_Count > i && Parameter_List[i].Integer_Is_Valid)
    {
      attr.toSet |= LWM2M_ATTR_FLAG_GREATER_THAN;
      attr.greaterThan = Parameter_List[i].Integer_Value;
      i++;
    }
    if((opt & 0x01) && Parameter_Count > i && Parameter_List[i].Integer_Is_Valid)
    {
      attr.toSet |= LWM2M_ATTR_FLAG_STEP;
      attr.step= Parameter_List[i].Integer_Value;
      i++;
    }

    if(data.lwm2mH)
    {
      result = observe_setParameters(data.lwm2mH, &uri, data.lwm2mH->serverList ,&attr);
      if(result != LWM2M_204_CHANGED)
      {
        QCLI_Printf(qcli_m2m_handle,"Error in setting attributes.\n");
        return QCLI_STATUS_ERROR_E;
      }
      else
      {
        QCLI_Printf(qcli_m2m_handle,"Attributes.set successfully. \n");
        return QCLI_STATUS_SUCCESS_E;
      }
    }
  }
  return QCLI_STATUS_ERROR_E;
}
#endif

/**
 * @fn static QCLI_Command_Status_t simulate_smswakeup()
 * @brief This function is used to wakeup client.
 *
 * @param Parameter_Count   Number of parameters passed from QCLI
 * @param Parameter_List    List of para received from QCLI
 *
 * @return On success, returns QCLI_STATUS_SUCCESS_E or QCLI_STATUS_USAGE_E
 *      On Failure, return QCLI_STATUS_ERROR_E
 */
static QCLI_Command_Status_t sms_factory_reset(uint32_t Parameter_Count,
    QCLI_Parameter_t* Parameter_List)
{
  if (2 != Parameter_Count)
  {
    QCLI_Printf(qcli_m2m_handle, "Invalid!!");
    return QCLI_STATUS_USAGE_E;
  }
  if (!strncmp("-n", (const char *)Parameter_List[0].String_Value, 2))
  {
    memcpy(trans_msg_data, sms_msg_data_number1, sms_msg_datalen);
	trans_msg_data[sms_msg_datalen-5] = (uint8_t)'3';
	trans_msg_data[sms_msg_datalen-3] = (uint8_t)'0';
    trans_msg_data[sms_msg_datalen-1] = (uint8_t)'5';
    trans_msg_data[sms_msg_datalen] = '\0';
    trans_msg_datalen = sms_msg_datalen;
    routed_msg = true;
	trans_ack_indicator = WMS_ACK_INDICATOR_DO_NOT_SEND_ACK_V01;
    if (!strncmp("910090009001", (const char *)Parameter_List[1].String_Value, 12))
    {
      trans_msg_data[8] = 16;
    }
    if (!strncmp("910090009002", (const char *)Parameter_List[1].String_Value, 12))
    {
      trans_msg_data[8] = 17;
    }
    qurt_signal_set(&lwm2m_signal_Handle, WMS_MT_MESSAGE_IND);
  }
  return QCLI_STATUS_SUCCESS_E;
}


/**
 * @fn static QCLI_Command_Status_t simulate_register_update()
 * @brief Simulates register update.
 *
 * @param Parameter_Count   Number of parameters passed from QCLI
 * @param Parameter_List    List of para received from QCLI
 *
 * @return On success, returns QCLI_STATUS_SUCCESS_E or QCLI_STATUS_USAGE_E
 *         On Failure, return QCLI_STATUS_ERROR_E
 */
static QCLI_Command_Status_t simulate_register_update(uint32_t Parameter_Count,
    QCLI_Parameter_t* Parameter_List)
{
  uint32_t size = 0x00;
  uint8_t *buffer = NULL;

  if (Parameter_Count)
  {
    QCLI_Printf(qcli_m2m_handle, "Invalid!!");
    return QCLI_STATUS_USAGE_E;
  }
  size = get_registerPayload_size(data.lwm2mH);

  if (!size)
    return QCLI_STATUS_ERROR_E;

  buffer = (uint8_t *) malloc(size);
  if(buffer == NULL)
   return QCLI_STATUS_ERROR_E;

  memset(buffer, 0x00, sizeof(buffer)/sizeof(uint8_t));
  
  if (object_getRegisterPayload(data.lwm2mH, NULL, buffer, sizeof(buffer)/sizeof(uint8_t)) == 0)
  {
    free(buffer);
    return QCLI_STATUS_ERROR_E;
  }

  QCLI_Printf(qcli_m2m_handle, "Register Update : %s\n", buffer);

  if(buffer == NULL)
     free(buffer);
   
  return QCLI_STATUS_SUCCESS_E;
}


/**
 * @fn static QCLI_Command_Status_t simulate_discover()
 * @brief This function is used to simulate discover.
 *
 * @param Parameter_Count   Number of parameters passed from QCLI
 * @param Parameter_List    List of para received from QCLI
 *
 * @return On success, returns QCLI_STATUS_SUCCESS_E or QCLI_STATUS_USAGE_E
 *         On Failure, return QCLI_STATUS_ERROR_E
 */
static QCLI_Command_Status_t simulate_discover(uint32_t Parameter_Count,
    QCLI_Parameter_t* Parameter_List)
{
  uint8_t *bufferP = NULL;
  size_t payloadLen = 0;
  lwm2m_uri_t uriP;
  uint8_t result = LWM2M_NO_ERROR;

  if (Parameter_Count)
  {
    QCLI_Printf(qcli_m2m_handle, "Invalid!!");
    return QCLI_STATUS_USAGE_E;
  }
  
  memset(&uriP, 0x00, sizeof(lwm2m_uri_t));
  uriP.objectId = LWM2M_DEVICE_OBJECT_ID;
  uriP.flag |= LWM2M_URI_FLAG_OBJECT_ID;

  result = object_discover(data.lwm2mH, &uriP, &bufferP, &payloadLen, NULL);
  if (result == LWM2M_205_CONTENT)
  {
    LOG_DEBUG("Discover : %s", bufferP);
    QCLI_Printf(qcli_m2m_handle, "Discover : %s", bufferP);

    if (bufferP)
     lwm2m_free(bufferP);
  }
  else 
  {
    LOG_DEBUG("Discover failure (%d)", result);
    QCLI_Printf(qcli_m2m_handle, "Discover failure (%d)", result);
  }

  return QCLI_STATUS_SUCCESS_E;
}


/**
 * @fn static QCLI_Command_Status_t simulate_bootstrap_discover()
 * @brief This function is used to simulate bootstrap discover.
 *
 * @param Parameter_Count   Number of parameters passed from QCLI
 * @param Parameter_List    List of para received from QCLI
 *
 * @return On success, returns QCLI_STATUS_SUCCESS_E or QCLI_STATUS_USAGE_E
 *         On Failure, return QCLI_STATUS_ERROR_E
 */
static QCLI_Command_Status_t simulate_bootstrap_discover(uint32_t Parameter_Count,
    QCLI_Parameter_t* Parameter_List)
{
  uint8_t *bufferP;
  size_t payloadLen;
  uint8_t result = LWM2M_NO_ERROR;

  simulate_register_update(0, NULL);
  simulate_discover(0, NULL);

  if (2 < Parameter_Count)
  {
    QCLI_Printf(qcli_m2m_handle, "Invalid!!");
    return QCLI_STATUS_USAGE_E;
  }

  if (Parameter_Count == 0)
    result = object_bs_discover(data.lwm2mH,INVALID_OBJECT_ID,&bufferP, &payloadLen);
  else
    result = object_bs_discover(data.lwm2mH,Parameter_List[0].Integer_Value,&bufferP, &payloadLen);
  
  if(result == LWM2M_205_CONTENT)
  {
    LOG_DEBUG("Bootstrap Discover : %s", bufferP);
    QCLI_Printf(qcli_m2m_handle, "Bootstrap Discover : %s", bufferP);

    if(bufferP)
     lwm2m_free(bufferP);
  }
  else if (result == LWM2M_404_NOT_FOUND)
  {
    LOG_DEBUG("LWM2M_LOG :Not Found ");
    QCLI_Printf(qcli_m2m_handle, "Not Found");
  }

  return QCLI_STATUS_SUCCESS_E;
}

const QCLI_Command_t m2m_cmd_list[] = {
  // Command_Function, Start_Thread, Command_String, Usage_String, Description 
#if 0 
  // commented as auto start feature is enabled 
  {m2m_start, false, "m2m_client_start", "Usage : m2m_client_start", "Start M2M Client"},
#endif  
  {m2m_reboot, false, "system reboot", "Usage : system reboot", "Reboots the Device"},
  {simulate_smswakeup,false,"simulate_smswakeup","Usage: simulate_smswakeup -a | -s <timeout>", 
      "simulate sms wake up -s options sets the timeout value and -a wakes up the client(simulate sms awaken)"},
  {simulate_bootstrap_discover,false,"bootstrap_discover","Usage: bootstrap_discover <objectId>", "bootstrap_discover"},
  {roaming_signal,false,"roaming_signal","Usage: roaming_signal <0 | 1>", "Select signal for roaming, 0 for in_roaming and 1 for out_of_roaming"},
  {set_thresholds, false, "set_threshold", "Usage: set_threshold [-m <memory threshold>] | [-s <signal threshold>]", "helps set thresholds"},
  {sms_reg_update, false, "sms_reg_update", "Usage: sms_reg_update [-i <instance_id>] -n [910090009000|910090009001|910090009002]", " Reg update through SMS"},
  {sms_factory_reset, false, "sms_factory_reset", "Usage: sms_factory_reset -n [910090009000|910090009001|910090009002]", "Factory Reset through SMS"},
#ifdef WRITE_ATTR_TEST
  {set_attributes, false, "set_attributes", "Usage: set_attributes uri [/x/y/z] [0 - 31] pmin pmax lt gt stp Ex:- /3/0/4 24 pmin 10 pmax 20", "Set resource attributes."},
  {clear_attributes, false, "clear_attributes", "Usage: clear_attributes uri [/x/y/z] [0 - 31] Ex:- /3/0/4 24 ", "Clear resource attributes."},
  {simulate_msisdn_change, false, "msisdn_change", "Usage : msisdn_change <NewMSISDNVal>", "Simulates msisdn change"}
#endif
};

const QCLI_Command_Group_t m2m_cmd_group = {
  "LWM2M", /* Group_String: will display cmd prompt as "PSM> " */
  sizeof(m2m_cmd_list) / sizeof(m2m_cmd_list[0]), /* Command_Count */
  m2m_cmd_list                                   /* Command_List */
};

/*****************************************************************************
 * This function is used to register the LWM2M Demo Group with QCLI.
 *****************************************************************************/
void Initialize_LwM2M_Demo(void)
{
  // Attempt to reqister the Command Groups with the qcli framework.
  qcli_m2m_handle = QCLI_Register_Command_Group(NULL, &m2m_cmd_group);
  if(qcli_m2m_handle)
  {
    QCLI_Printf(qcli_m2m_handle, "CLI registered\n");
  }

  return;
}

#endif


static char buffer[PRINT_SIZE];
/**
 * @fn int m2m_log_printf()
 * @brief This function is used to print the input buffer
 * 
 * @param fmt format to be printed
 *
 * @returns 1
 */
int m2m_log_printf(const char* fmt, ...)
{
  va_list arg_list;
  int len;

  if(m2m_thread_started != 1)
    return 1;

  va_start(arg_list, fmt);
  len = vsnprintf((char*)(buffer), sizeof(buffer), (char*)fmt, arg_list);
  va_end(arg_list);

  if(len >= PRINT_SIZE)
    len = PRINT_SIZE - 1;
  buffer[len] = 0;
#ifdef ENABLE_INTERNAL_BUILD
  QCLI_Printf(qcli_m2m_handle, (const uint8_t*)buffer);
  QCLI_Printf(qcli_m2m_handle, "\r\n");
#endif
  return 1;
}


#ifdef M2M_DEBUG
/**
 * @brief This function is used to print lwm2m state
 * @param lwm2mH pointer to handle to lwm2m context
 */
static void lwm2m_print_status(lwm2m_context_t *lwm2mH)
{
  switch (lwm2mH->state)
  {
    case STATE_INITIAL:
      ds_iot_ulog_high("State: STATE_INITIAL");
      break;
    case STATE_BOOTSTRAP_REQUIRED:
      ds_iot_ulog_high("State: STATE_BOOTSTRAP_REQUIRED");
      break;
    case STATE_BOOTSTRAPPING:
      ds_iot_ulog_high("State: STATE_BOOTSTRAPPING");
      break;
    case STATE_REGISTER_REQUIRED:
      ds_iot_ulog_high("State: STATE_REGISTER_REQUIRED");
      break;
    case STATE_REGISTERING:
      ds_iot_ulog_high("State: STATE_REGISTERING");
      break;
    case STATE_READY:
      ds_iot_ulog_high("State: STATE_READY");
      break;
    default:
      ds_iot_ulog_high("Unknown...");
      break;
  }
}
#endif
/**
 * @fn     int parse_wms_message_received ()
 * @brief  This function is used to parse the  message 
 * @param  buffer     - pointer to message buffer  
 * @param  buffer_len - size of buffer
 * @return on success - 0
 *         on error   - -1
 */

  void  parse_wms_message_received (char *in_buffer , int in_buffer_len)
{
  qapi_Status_t coap_error_code = QAPI_OK;
  static qapi_Coap_Packet_t *message = NULL;
  int user_data_hdr_len = 0;
  int pdu_type = 0;
  int wsp_hdr_len = 0;
#ifdef UNIT_TEST
  char buffer[] = {0x44, 0x02, 0x76, 0x61, 0xc4, 0xf6, 0x4c, 0xba, 0xb1, 0x31, 0x01, 0x33, 0x01, 0x38};
  int buffer_len = 14;
#else
  char * buffer = in_buffer;
  int buffer_len = in_buffer_len; 
#endif

  if(buffer == NULL)
  {
    ds_iot_ulog_err ("Parsed message is not sms-wakeup ") ;
    return;
  }
  user_data_hdr_len = buffer[0];

  if (((user_data_hdr_len + 3) >= buffer_len) || (user_data_hdr_len + 3) < 0)
  	return;
  
  pdu_type = buffer[user_data_hdr_len + 2];
  if(pdu_type != PDU_TYPE_PUSH)
  {
    LOG_DEBUG(" Message is not PUSH type ");
    return;
  }

  wsp_hdr_len = buffer[user_data_hdr_len + 3];

  if (((user_data_hdr_len + wsp_hdr_len + 4) >= buffer_len) || (user_data_hdr_len + wsp_hdr_len + 4) < 0)
  	return;
  
  if(buffer[user_data_hdr_len + wsp_hdr_len + 3] != LWM2M_PAYLOAD_PUSH_ID)
  {
    LOG_DEBUG("Application id URN is not x-wap-application:lwm2m.dm ");
  }

  buffer += (user_data_hdr_len + wsp_hdr_len + 4);

  buffer_len -= (user_data_hdr_len + wsp_hdr_len + 4);

  if(buffer_len <= 0 || (buffer == NULL))
  {
    ds_iot_ulog_err (" Size of CoAP message is not valid or message is NULL ");
    return;
  }

  qapi_Coap_Init_Message(lwm2m_coap_hdl, &message , QAPI_COAP_TYPE_NON , QAPI_CONTENT_2_05);
  if(message == NULL)
  { 
     return;
  }
  memset(message, 0, sizeof(qapi_Coap_Packet_t));
  coap_error_code = qapi_Coap_Parse_Message(lwm2m_coap_hdl, &message, (uint8_t *)buffer, (uint16_t)buffer_len);

  if(coap_error_code != QAPI_OK)
  {
    ds_iot_ulog_err1(" CoAP error code  %d ",coap_error_code) ;
    qapi_Coap_Free_Message(lwm2m_coap_hdl , message);
    return ;
  }
  if (message->version != 1) 
  {
    LOG_DEBUG(" The message have version of %d ",message->version ) ;
    qapi_Coap_Free_Message(lwm2m_coap_hdl , message);
    return ;
  }

  if (message->type != QAPI_COAP_TYPE_CON) 
  {
    LOG_DEBUG(" The message type recieved is NOT CON MSG TYPE  ") ;
  }

  if (message->code != QAPI_COAP_POST ) 
  {
    LOG_DEBUG(" The passed message is not of POST type");
    qapi_Coap_Free_Message(lwm2m_coap_hdl , message);
    return ;
  }

  sms_msg.uri = uri_decode(NULL, message->uri_path);

  if (message->payload_len)
  {
    /* Supporting "Register Update Trigger with Parameter" for now */ 
    if ((NULL != sms_msg.uri) &&
        (sms_msg.uri->flag & LWM2M_URI_FLAG_RESOURCE_ID) &&
        (LWM2M_SERVER_UPDATE_ID == sms_msg.uri->resourceId))
    {
      sms_msg.payload_len = message->payload_len;
      sms_msg.payload = (uint8_t *) malloc (message->payload_len);
      if(sms_msg.payload == NULL)
      {
         ds_iot_ulog_err("LWM2M_LOG: Memory Allocation FAILED SMS payload");
      }
      else
      {
        memcpy(sms_msg.payload, message->payload, message->payload_len);
    }
   }  
  }  

  qapi_Coap_Free_Message(lwm2m_coap_hdl , message);
  return;
}

carrier_apn_cfg_t *find_dup_apn_entry(carrier_apn_cfg_t* head_apn_cfg, carrier_apn_cfg_t *apn_cfg)
{
  if ((apn_cfg == NULL) || (head_apn_cfg == NULL))
    return NULL;

  while (head_apn_cfg) 
  {
    /* Detect any duplicate entry */ 
    if (head_apn_cfg != apn_cfg)
    {
      if (!strncmp(head_apn_cfg->apn_name, apn_cfg->apn_name, CARRIER_APN_LEN))
        return head_apn_cfg;
    }

    head_apn_cfg = head_apn_cfg->next;
  }

  return NULL;
}

void update_bs_apn_info(lwm2m_context_t * context, uint16_t old_ssid, uint16_t new_ssid)
{
  carrier_apn_cfg_t *apn_cfg = NULL;

  if (NULL == context) 
  {
    LOG_DEBUG("Invalid LwM2M context information - context[0x%x]", context);
    return;
  }

  if(old_ssid == new_ssid)
  {
    LOG_DEBUG("the same ssid %d, no need to update", old_ssid);
    return;
  }

  apn_cfg = context->apn_cfg;
  while(apn_cfg != NULL)
  {
    if(apn_cfg->bootstrap_apn && apn_cfg->short_server_id == old_ssid)
    {
      apn_cfg->short_server_id = new_ssid;
      bs_ssid = new_ssid;
      break;
    }
  }
  return;
}

carrier_apn_cfg_t *populate_default_apn_info(uint16_t short_server_id, bool bs_server)
{
  carrier_apn_cfg_t *apn_cfg = NULL;
  lwm2m_context_t * context = get_lwm2m_context();

  if (NULL == context) 
  {
    LOG_DEBUG("%s context", nullError);
    return NULL; 
  }  

  apn_cfg = (carrier_apn_cfg_t *)lwm2m_malloc(sizeof(carrier_apn_cfg_t));
  if (NULL == apn_cfg)
  {
    ds_iot_ulog_err("LWM2M_LOG: Memory Allocation FAILED for carrier APN config");
    return NULL; 
  }
  
  memset(apn_cfg, 0x00, sizeof(carrier_apn_cfg_t));
  
  apn_cfg->short_server_id        = short_server_id;
  /* Default to NULL APN link */
  apn_cfg->apn_link               = 0xFFFFFFFF;
  apn_cfg->apn_class              = LWM2M_APN_CLASS2;  
  apn_cfg->retry_timeout_lst      = data.cfg_info.retry_timeout_lst;
  apn_cfg->retry_timeout_lst_len  = data.cfg_info.retry_timeout_lst_len;
  strlcpy(apn_cfg->apn_name, data.cfg_info.apn_info, CARRIER_APN_LEN);
  apn_cfg->bootstrap_apn          = (bs_server) ? true : false; 
  apn_cfg->registration_apn       = (bs_server) ? false : true;
  apn_cfg->ip_family              = data.cfg_info.ip_family;
  apn_cfg->ip_support             = data.cfg_info.ip_support;  
  apn_cfg->non_ip_support         = data.cfg_info.non_ip_support;
  ds_iot_ulog_high("Populated Default APN");
  return apn_cfg; 
}


int8_t populate_apn_info_lwm2m_cfg(lwm2m_context_t * context)
{
  void *reg_server_cfg = NULL;
  carrier_apn_cfg_t *apn_cfg = NULL;
  carrier_apn_cfg_t *bs_apn_cfg = NULL; 

  if (NULL == context) 
  {
    LOG_DEBUG("%s context", nullError);
    return -1; 
  }  

  apn_cfg = (carrier_apn_cfg_t *)lwm2m_malloc(sizeof(carrier_apn_cfg_t));
  if (NULL == apn_cfg)
  {
    ds_iot_ulog_err("LWM2M_LOG: Memory Allocation FAILED for carrier APN config");
    goto cfg_error;
  }
  
  memset(apn_cfg, 0, sizeof(carrier_apn_cfg_t));
  
  /* Update the BS SSID from the bootstrap server list */ 
  if (context->bootstrapServer != NULL)
    bs_ssid = context->bootstrapServer->shortID;
    
  apn_cfg->short_server_id        = bs_ssid;
  /* Default bootstrap server APN configuration to class 2 to ensure that data call bring-up is successful */  
  apn_cfg->apn_class              = LWM2M_APN_CLASS2;  
  apn_cfg->retry_timeout_lst      = data.cfg_info.retry_timeout_lst;
  apn_cfg->retry_timeout_lst_len  = data.cfg_info.retry_timeout_lst_len;
  strlcpy(apn_cfg->apn_name, data.cfg_info.apn_info, CARRIER_APN_LEN);
  apn_cfg->bootstrap_apn          = true;
  /* Default the link to NULL link (gets updated later) */
  apn_cfg->apn_link               = 0xFFFFFFFF;        
  apn_cfg->ip_family              = data.cfg_info.ip_family;
  apn_cfg->ip_support             = data.cfg_info.ip_support;  
  apn_cfg->non_ip_support         = data.cfg_info.non_ip_support;
  
  /* Add the bootstrap APN configuration to the config list */
  reg_server_cfg = context->c_apn_cfg;
  bs_apn_cfg = apn_cfg;
  context->c_apn_cfg = (carrier_apn_cfg_t *)LWM2M_LIST_ADD(context->c_apn_cfg, apn_cfg);

  /* Carrier APN config is not present - LWM2M config file needs to be used for registration servers */
  if (NULL == reg_server_cfg)
  {
    lwm2m_server_t *srv_lst = context->serverList;
  
    while (srv_lst)
    {
      apn_cfg = (carrier_apn_cfg_t *)lwm2m_malloc(sizeof(carrier_apn_cfg_t));
      if (NULL == apn_cfg)
      {
        ds_iot_ulog_err("LWM2M_LOG: Memory Allocation FAILED for carrier APN config");
        goto cfg_error;
      }
      
      memset(apn_cfg, 0, sizeof(carrier_apn_cfg_t));
      
      /* Copy the bootstrap configuration to the registration servers */
      *apn_cfg                        = *bs_apn_cfg;
      /* Update the registration server specific parameters */
      apn_cfg->short_server_id        = srv_lst->shortID;
      apn_cfg->registration_apn       = true; 
      apn_cfg->bootstrap_apn          = false;

      /* Add the registration APN configuration to the config list */
      context->c_apn_cfg = (carrier_apn_cfg_t *)LWM2M_LIST_ADD(context->c_apn_cfg, apn_cfg);
     
      srv_lst = srv_lst->next;      
    }
  }

  return 0; 

cfg_error:
  /* Clean-up the populated APN config information */
  if (context->c_apn_cfg)
  {
    LWM2M_LIST_FREE(context->c_apn_cfg);
    context->c_apn_cfg = NULL; 
  }
  
  return -1;
}

char* process_cfg_utility_open_file(const char* cfg_file, int* bytes_size_ret, int* cfg_fd_ret)
{
  struct fs_stat lwm2m_config_fileStat;
  char* buf = NULL; 
  int read_bytes = 0;
  int cfg_fd = -1;

  //check whether the file exist and size>0
  if((EFAILURE == efs_stat(cfg_file, &lwm2m_config_fileStat)) || 
  	  (lwm2m_config_fileStat.st_size == 0))
  {
    ds_iot_log_strf_err("access %s failure", cfg_file);
  	return NULL;
  }

  buf = lwm2m_malloc(lwm2m_config_fileStat.st_size + 1);
  if(buf == NULL)
  {
    ds_iot_ulog_err("LWM2M_LOG: Memory Allocation FAILED");
  	return NULL;
  }
  
  cfg_fd = efs_open(cfg_file, O_RDONLY);

  if(cfg_fd == EFAILURE)
  {
    ds_iot_ulog_err("Failed to open config file!!");
    goto error_cleanup;
  }
  else 
  { 
    memset (buf , 0 , lwm2m_config_fileStat.st_size + 1) ;
    read_bytes = efs_read(cfg_fd, buf, lwm2m_config_fileStat.st_size);
    if(read_bytes <= 0)
    {
      ds_iot_ulog_err("Failed to read from config file!!");
	  goto error_cleanup;
    }
  } 

  if(bytes_size_ret != NULL && cfg_fd_ret != NULL)
  {
    *bytes_size_ret = read_bytes;
    *cfg_fd_ret = cfg_fd;
  }
  return buf;

  error_cleanup:
  	if(cfg_fd != -1)
  	{
  	  efs_close(cfg_fd);
	  cfg_fd = -1;
  	}
	if(buf != NULL)
	{
	  lwm2m_free(buf);
	  buf = NULL;
	}
    return NULL;
  	
}

void process_cfg_utility_set_value(const char* key, const char* value, 
                                               lwm2m_cfg_item_str*     str_table,     uint8_t str_table_size, 
                                               lwm2m_cfg_item_bool*    bool_table,    uint8_t bool_table_size, 
                                               lwm2m_cfg_item_integer* integer_table, uint8_t integer_table_size )
{
  int val = 0;
  int i_table = 0; 
  
  if(key == NULL || value == NULL)
  	return;

  for(i_table = 0; str_table != NULL && i_table < str_table_size; i_table++)
  {
    if(!strcmp(str_table[i_table].cfg_str, key))
    {
      strlcpy(str_table[i_table].cfg_value_ptr, value, str_table[i_table].cfg_value_len);
      return;
    }
  }
	
  for(i_table = 0; bool_table != NULL && i_table < bool_table_size; i_table++)
  {
    if(!strcmp(bool_table[i_table].cfg_str, key))
    {
      val = atoi(value);
      if(0 == val) 
         *(bool_table[i_table].cfg_value_ptr) = LWM2M_FALSE;
	  else 
	  	 *(bool_table[i_table].cfg_value_ptr) = LWM2M_TRUE;
	  	
      return;
    }
  }
	
  for(i_table = 0; integer_table != NULL && i_table < integer_table_size; i_table++)
  {
    if(!strcmp(integer_table[i_table].cfg_str, key))
    {			      
      if(value[0] == '-')
	  	val = integer_table[i_table].cfg_default_value;
	  else
        val = atoi(value);

      if(val < integer_table[i_table].min_value || val > integer_table[i_table].max_value)
        val = integer_table[i_table].cfg_default_value;
	     
	  if(integer_table[i_table].size == 1)	   	
	    *((uint8*)integer_table[i_table].cfg_value_ptr)  = val;
	  else if (integer_table[i_table].size == 2)	   		   	
	    *((uint16*)integer_table[i_table].cfg_value_ptr) = val;
	  else if (integer_table[i_table].size == 4)	   	
	    *((int32*)integer_table[i_table].cfg_value_ptr)  = val;
	  	
      return;
    }
  }
}	

void process_cfg_utility_set_as_default(lwm2m_cfg_item_str* str_table,         uint8_t str_table_size, 
	                                                   lwm2m_cfg_item_bool* bool_table,       uint8_t bool_table_size, 
	                                                   lwm2m_cfg_item_integer* integer_table, uint8_t integer_table_size)
{
  int i_table = 0; 

  for(i_table = 0; str_table != NULL && i_table < str_table_size; i_table++)
  {
    strlcpy(str_table[i_table].cfg_value_ptr, str_table[i_table].cfg_default_value, str_table[i_table].cfg_value_len);
  }
  for(i_table = 0; bool_table != NULL && i_table < bool_table_size; i_table++)
  {
    *(bool_table[i_table].cfg_value_ptr) = bool_table[i_table].cfg_default_value;
  }
  for(i_table = 0; integer_table != NULL && i_table < integer_table_size; i_table++)
  {
     if(integer_table[i_table].size == 1)	   	
       *((uint8*)integer_table[i_table].cfg_value_ptr)  = integer_table[i_table].cfg_default_value;
     else if (integer_table[i_table].size == 2)	   		   	
       *((uint16*)integer_table[i_table].cfg_value_ptr) = integer_table[i_table].cfg_default_value;
     else if (integer_table[i_table].size == 4)	   	
       *((int32*)integer_table[i_table].cfg_value_ptr)  = integer_table[i_table].cfg_default_value;
  }		
	
}	


/**
 * @fn     int process_carrier_apn_cfg()
 * @brief  This function is used to fetch the lwm2m configuration params
 * @param  context    - lwm2m context handle 
 * @return on success - 0
 *         on error   - -1
 */
int process_carrier_apn_cfg(lwm2m_context_t * context)
{
  int read_bytes = 0;
  char* buf = NULL;
  int cfg_fd = -1;
  carrier_apn_cfg_t *apn_cfg = NULL;
  int ret = -1;   

  if (context == NULL) 
  {
    LOG_DEBUG ("%s context", nullError);
    return -1;
  }

  /* Carrier APN configuration is optional. It is needed only in cases where there are multiple registration 
   * servers with varying configurations. Alternatively, LwM2M config file which is mandatory is used always
   * for the bootstrap server configuration and also cases where there are multiple registration servers which 
   * have the same configuration. 
   *
   * If both carrier APN and LwM2M configuration files are present, in that case carrier APN config takes the
   * precedence over the LwM2M configuration only for the registration servers.
   *
   * LwM2M configuration file is recommended only for cases where there is only one bootstrap and registration 
   * server using the same APN. For multi PDN cases, always use carrier_apn confiuration which gives flexibility.
   */ 

  buf = process_cfg_utility_open_file(CARRIER_APN_CFG, &read_bytes, &cfg_fd);

  if(buf == NULL)
  	goto end_cleanup;
  
  {
      int line_start_index = 0;
      int equal_index = -1;
      int line = 0, i = 0, j = 0, k = 0, index = 0;
      char key[64];
      char value[30];
      char apn_arr[CFG_VAL_LEN];

      LOG_DEBUG(" Number of bytes read for config file :%d", read_bytes);
      buf[read_bytes] = '\n';
      read_bytes++;

      if(read_bytes > CFG_VAL_LEN)
      {
        goto end_cleanup;
      }

      /* Loop through the APN config buffer */
      for (index = 0; index < read_bytes; index++)
      {
        if (buf[index] != ';')
        {
          apn_arr[k++] = buf[index];
          continue;
        }

        apn_arr[k] = '\n';
        apn_cfg = (carrier_apn_cfg_t *)lwm2m_malloc(sizeof(carrier_apn_cfg_t));
        if(NULL == apn_cfg)
        {
          ds_iot_ulog_err("LWM2M_LOG: Memory Allocation FAILED");
          goto end_cleanup;
        }

        memset(apn_cfg, 0, sizeof(carrier_apn_cfg_t));

        /* Parse APN config */ 
        for ( ; i < k; i++)
        {
          if (apn_arr[i] == '=')
          {
            equal_index = i;
          }
          else if (apn_arr[i] == '\r')
          {
            apn_arr[i] = 0;
          }
          else if (apn_arr[i] == '\n')
          {
            if (equal_index > 0)
            {
              int k_i = 0;
              int v_i = 0;
              memset(key, 0, sizeof(key));
              memset(value, 0, sizeof(value));

              // found an equals
              // Loop is used to extract the config parameter name
              for (j = line_start_index; j < equal_index; j++)
              {
                key[k_i] = apn_arr[j];
                k_i++;
              }

              key[k_i] = '\0';
              // Loop is used to extract the config parameter value
              for (j = equal_index + 1; j < i; j++)
              {
                value[v_i] = apn_arr[j];
                v_i++;
              }

              value[v_i] = '\0';
              LOG_DEBUG(" value parsed :%s", value);

              /* Identifies the APN name (mandatory) */
              if (!strcmp("APN_NAME", key))
              {
                strlcpy(apn_cfg->apn_name, value, CARRIER_APN_LEN);
                LOG_DEBUG(" in config file; apn_name =%s", apn_cfg->apn_name);
              }
              /* Indicates short server ID (mandatory) */ 
              else if (!strcmp("SHORT_SERVER_ID1", key))
              {
                apn_cfg->short_server_id = (uint16_t)atoi(value);                            

                /* Mark that the parsed APN configuration is associated with registration server */  
                apn_cfg->registration_apn = true;                
                m2m_log_printf(" In carrier_apn config file; short_server_id = %d", apn_cfg->short_server_id);
              }
              /* Identifies the APN uniquely (optional) - specific to Verizon (backward-compatibility) */
              else if (!strcmp("APN_CLASS", key))
              {
                apn_cfg->apn_class = atoi(value);
                LOG_DEBUG(" in config file; apn_class = %d", apn_cfg->apn_class);
              }              
              /* Indicates if bootstrapping needs to be performed if the registration fails (optional) */ 
              else if (!strcmp("BS_IF_REG_FAILS", key))
              {
                apn_cfg->bs_if_reg_fails = atoi(value);
                LOG_DEBUG(" in config file; bs_if_reg_fails = %d", apn_cfg->bs_if_reg_fails);
              }
              /* IP family associated with the APN (optional) - default to IPv4 */
              else if (!strcmp("IP_FAMILY", key))
              {
                if (!strcasecmp("v4", value))
                  apn_cfg->ip_family = AF_INET;
                else if (!strcasecmp("v6",value))
                  apn_cfg->ip_family = AF_INET6;
                else if (!strcasecmp("v4v6",value))
                  apn_cfg->ip_family = AF_INET_DUAL46;
                else 
                  apn_cfg->ip_family = AF_INET;

                m2m_log_printf(" In carrier_apn config file; ip_family = %d\n", apn_cfg->ip_family);
              }
              else if(!strcmp("BINDINGS", key))
              {
                if (!strcasecmp("ip", value))
                  apn_cfg->ip_support = true;
                else if (!strcasecmp("nonip", value))
                  apn_cfg->non_ip_support = true;
                else if (!strcasecmp("ip_nonip", value))
                {
                  apn_cfg->non_ip_support = true;
                  apn_cfg->ip_support     = true;
                }

                m2m_log_printf(" In carrier_apn config file; BINDINGS = IP(%d)_NONIP(%d)\n", data.cfg_info.ip_support, data.cfg_info.non_ip_support);
              }
            }

            line++;
            equal_index = -1;
            line_start_index = i + 1;
          }
        }

        /* Add the APN configuration to the list */ 
        context->c_apn_cfg = (carrier_apn_cfg_t *)LWM2M_LIST_ADD(context->c_apn_cfg, apn_cfg);

        /* APN information and Short Server ID are mandatory */
        if ((0x00 == apn_cfg->short_server_id))
          goto end_cleanup;      
        /* If APN name field is not configured default it to NULL */
        if(!strlen(apn_cfg->apn_name))
          strlcpy(apn_cfg->apn_name, "", CARRIER_APN_LEN);       

        /* Non-IP family - Duplicates are not allowed */
        if (apn_cfg->non_ip_support)
        {
          if (NULL != find_dup_apn_entry(context->c_apn_cfg, apn_cfg))
          {
            ds_iot_ulog_err("Invalid configuration - duplicates for Non-IP APN found");
            goto end_cleanup;   
          }
        }
   
        /* Default the link to NULL link (gets updated later) */
        apn_cfg->apn_link = 0xFFFFFFFF;        
        /* IP family - IPv4 (default if not configured)*/
        apn_cfg->ip_family = apn_cfg->ip_family ? apn_cfg->ip_family : AF_INET;
        /* Default both IP (IPv4) and NON-IP are supported if the PDN type configuration is not specified */ 
        if ((!apn_cfg->non_ip_support) && (!apn_cfg->ip_support))
        {
          /* IP family supported by default */ 
          apn_cfg->ip_support = true; 
          /* Non-IP family supported by default */ 
          apn_cfg->non_ip_support = true;
        }
        /* APN class - CLASS 3 (default) */  
        apn_cfg->apn_class = apn_cfg->apn_class ? apn_cfg->apn_class : LWM2M_APN_CLASS3;
        /* Assign the retry timeout intervals */ 
        apn_cfg->retry_timeout_lst      = data.cfg_info.retry_timeout_lst;
        apn_cfg->retry_timeout_lst_len  = data.cfg_info.retry_timeout_lst_len;       

        apn_cfg = NULL;
        memset(apn_arr, '\0', 128);
        j = 0;  
      }    

      ret = 0; /*mark process success*/
    }
  
end_cleanup:
  {
    if (cfg_fd >= 0)
    {
      efs_close(cfg_fd);
      cfg_fd = -1;
    }

    if (buf != NULL)
    {
      lwm2m_free(buf);
      buf = NULL;
    }

    /* Clean-up the parsed configuration if any error is detected in parsing "carrier_apn" config file. 
     * Alternatively, the LwM2M configuration is used to populate the default server configurations. 
     */
    if ((ret == -1) &&
        (context->c_apn_cfg))
    {
      LWM2M_LIST_FREE(context->c_apn_cfg);
      context->c_apn_cfg = NULL; 
    }
  
    /* Populate the boostrap server configuration from the LwM2M configuration. In case, there is is no 
     * carrier APN configuration then the registration servers are also populated from the LwM2M config. 
     *
     * LwM2M configuration file is recommended only for cases where there is only one bootstrap serve and 
     * registration server using the same APN. For multi PDN cases, always use carrier_apn configuration 
     * which gives flexibility in configuring the servers with varying configurations. 
     */
    ret = populate_apn_info_lwm2m_cfg(context);
  }  
     
  return ret;
}


int8_t calculate_retry_counter_values
(
  uint32_t retry_timeout, 
  uint32_t retry_exponent, 
  uint32_t max_retry_timeout, 
  uint32_t max_retry_entries, 
  uint32_t **retry_count_lst,
  uint32_t *retry_count_lst_len
)
{
  int8_t  items_cntr          = 0x00;
  uint32_t curr_retry_timeout = 0x00;

  /* Validate the input parameters */
  if ((NULL == retry_count_lst) ||
      (NULL == retry_count_lst_len) ||
      (!retry_timeout) ||
      (!retry_exponent) ||
      (!max_retry_timeout) ||
      (!max_retry_entries))
    return -1; 

  *retry_count_lst = (uint32_t *)malloc(max_retry_entries * sizeof(uint32_t));
  if (NULL == *retry_count_lst)
    return -1;

  *retry_count_lst_len = max_retry_entries;

  items_cntr = 0x00;
  curr_retry_timeout = retry_timeout;

  while(items_cntr < max_retry_entries)
  {
    if(curr_retry_timeout > max_retry_timeout)
      curr_retry_timeout = max_retry_timeout;

    *(*retry_count_lst + items_cntr) = curr_retry_timeout;

    if(curr_retry_timeout < max_retry_timeout)
      curr_retry_timeout *= retry_exponent;

    items_cntr++;
  }

  return 0;
}


/**
 * @fn     int8_t process_lwm2m_cfg()
 * @brief  This function is used to fetch the lwm2m configuration params
 * @param  void
 * @return void
 */
int8_t process_lwm2m_cfg(void)
{
  int read_bytes = 0;
  int read_bytes2 = 0;
  char* buf = NULL; 
  char* buf1 = NULL;
  char* buf2 = NULL;
  int cfg_fd = -1;
  int cfg_fd2 = -1;
  char* temp = NULL;
  char* temp2 = NULL;

  uint8_t bool_table_size=0, integer_table_size=0, str_table_size=0;
  
  lwm2m_cfg_item_bool lwm2m_cfg_bool_table[] = 
  {
    /* REG related */
    {"REG_UPDATE_ON_RECONNECT",   &data.cfg_info.reg_update_on_reconnect, LWM2M_FALSE},		
    {"ROAMING_FEAT",              &roaming_feat_enabled,                  LWM2M_TRUE},
    {"PER_REGSTATUS_FEAT",        &persis_regstatus_feat_enabled,         LWM2M_TRUE},
    {"PER_BOOTSTRAP_FEAT",        &persis_bootstrap_feat_enabled,         LWM2M_TRUE},

    /* Location related */
    {"ENABLE_LOCATION",           &enable_location,                       LWM2M_TRUE},
    {"ENABLE_LOCATION_AFTER_LTE", &enable_location_after_lte,             LWM2M_FALSE},

    /* Objects related */
    {"ENABLE_DEVCAP_OBJ",         &enable_dev_cap_obj,             LWM2M_TRUE},
    {"ENABLE_CONN_M_OBJ",         &enable_conn_m_obj,              LWM2M_TRUE},
    {"ENABLE_SOFT_MGNT_OBJ",      &enable_soft_mgnt_obj,           LWM2M_TRUE},	
    {"ENABLE_FIRMWARE_OBJ",       &enable_firmware_obj,            LWM2M_TRUE},
    {"ENABLE_CONN_STAT_OBJ",      &enable_conn_stat_obj,           LWM2M_TRUE},
    {"ENABLE_CELL_NW_CONN_OBJ",   &enable_cell_nw_conn_obj,        LWM2M_TRUE},
    {"ENABLE_APN_CONN_OBJ",       &enable_apn_conn_obj,            LWM2M_TRUE},
    {"ENABLE_HOST_DEV_OBJ",       &enable_host_dev_obj,            LWM2M_TRUE},
    {"ENABLE_ACL_OBJ",            &enable_acl_obj,                 LWM2M_TRUE},
    {"ENABLE_BS_SERVER_REPLACE",  &enable_bs_server_replace,       LWM2M_FALSE},

    /* Connection related*/
    {"ENABLE_DTLS_CLOSE_NOTIFY",  &data.cfg_info.enable_dtls_close_notify, LWM2M_FALSE},
    {"ENABLE_DTLS_EPOCH_0",       &data.cfg_info.enable_dtls_epoch_0,      LWM2M_FALSE},
    /* PSM related*/
    {"ENABLE_PSM",                &data.cfg_info.psm_enable,               LWM2M_TRUE},
    {"AWAKE_AFTER_PMIN",          &data.cfg_info.pmin_pmax_awake,          LWM2M_FALSE},

    /* MSISDN related*/
    {"USE_10_DIGIT_MSISDN",       &data.cfg_info.truncate_msisdn,          LWM2M_FALSE}
  };


  lwm2m_cfg_item_integer lwm2m_cfg_integer_table[] = 
  {
    /* Overall control related configutation */
    CFG_ITEM_INT("STARTUP_MODE",  data.cfg_info.lwm2m_startup_mode, LWM2M_STARTUP_BOOTSTRAP_PRELOADED, 
                                                                    LWM2M_STARTUP_BOOTSTRAP_PRELOADED,
                                                                    LWM2M_STARTUP_BOOTSTRAP_BYAPP)
    /* REG related */
    CFG_ITEM_INT("CARRIER_TYPE",  data.cfg_info.carrier_type, LWM2M_CARRIER_OPEN_MARKET,  
                                                              LWM2M_CARRIER_OPEN_MARKET, 
                                                              LWM2M_CARRIER_VZW)
    CFG_ITEM_INT("REG_EP_NAME",       data.cfg_info.reg_endpoint_name, LWM2M_IMEI_URN,        
                                                                       LWM2M_IMEI_URN, 
                                                                       LWM2M_URN_END)
    CFG_ITEM_INT("BOOTSTRAP_EP_NAME", data.cfg_info.bootstrap_endpoint_name, LWM2M_IMEI_MSISDN_URN, 
                                                                             LWM2M_IMEI_URN, 
                                                                             LWM2M_URN_END)
  
    /* REG retry related */  
    CFG_ITEM_INT("RETRY_TIMEOUT",       data.cfg_info.retry_timeout,      DEFAULT_RETRY_TIMEOUT,  0, MAX_INT_32)
    CFG_ITEM_INT("RETRY_EXPONENT_VAL",  data.cfg_info.retry_exponent_val, DEFAULT_EXPONENT_VAL,   0, MAX_INT_32)
    CFG_ITEM_INT("MAX_RETRY_TIMEOUT",   data.cfg_info.max_retry_timeout,  DEFAULT_MAX_TIMEOUT,    0, MAX_INT_32)
    CFG_ITEM_INT("MAX_NO_RETRIES",      data.cfg_info.max_retries ,       DEFAULT_MAX_RETRY_VAL , 0, MAX_INT_32)
    CFG_ITEM_INT("REG_RETRY_TIMEOUT",   gRegRetryInitialTimeout,  60, 0, MAX_INT_32)
    CFG_ITEM_INT("REG_RETRY_EXPONENT",  gRegRetrytimeoutExponent,  2, 0, MAX_INT_32)
    CFG_ITEM_INT("REG_RETRY_MAXTIMEOUT",gRegRetryMaxTimeout,     960, 0, MAX_INT_32)

    /* Timer related configuration */
   	CFG_ITEM_INT("ACK_TIMEOUT",             data.cfg_info.ack_timeout,        ACK_TIMEOUT,                    0,  MAX_INT_32)
   	CFG_ITEM_INT("BOOT_UP_SLEEP_TIME",      data.cfg_info.boot_up_sleep,      DEFAULT_BOOT_UP_SLEEP_TIME,     1,  MAX_INT_32)
   	CFG_ITEM_INT("MAX_BOOTSTRAP_WAIT_TIME", data.cfg_info.max_bootstrap_wait, DEFAULT_MAX_BOOTSTRAP_WAIT_SEC, 0,  MAX_INT_32)
    CFG_ITEM_INT("WAIT_TIME_TO_TRIGGER_NEXT_REG_CYCLE", data.cfg_info.wait_time_for_rereg, DEFAULT_WAIT_TIME_FOR_REREG,1, MAX_INT_32)
    CFG_ITEM_INT("STARTUP_WAIT_TIME_FOR_BYAPP",         data.cfg_info.startup_wait_time_for_byapp_mode, 0, 0, 60)
    CFG_ITEM_INT("REG_RETRY_WAIT_ON_ERROR", data.cfg_info.wait_time_for_error, 0,0, MAX_INT_32)

    /* Connection related */
    CFG_ITEM_INT("ROOTCA_SUPPORT_MODE", data.cfg_info.lwm2m_rootca_support_mode, LWM2M_ROOTCA_NO_SUPPORT,
                                                                                 LWM2M_ROOTCA_NO_SUPPORT,
                                                                                 LWM2M_ROOTCA_SUPPORT_BS_LWM2M_SERVERS)
    CFG_ITEM_INT("SESSION_TIMEOUT",     data.cfg_info.session_resumption_time,   DEFAULT_SESSION_RESUMPTION_TIME,
                                                                                 60,
                                                                                 MAX_INT_32)

    /* GPS related configuration*/
    CFG_ITEM_INT("GPS_MIN_INTERVAL",  gps_minInterval,  MIN_INTERVAL, 0, MAX_INT_32)
    CFG_ITEM_INT("GPS_MIN_DISTANCE",  gps_minDistance,  MIN_DISTANCE, 0, MAX_INT_32)

    /* PSM related configuration*/
    CFG_ITEM_INT("LIFETIME_DELTA",     data.cfg_info.lifetime_percentage, DEFAULT_LIFETIME_DELTA_PERCENTAGE, 5, 100)
    CFG_ITEM_INT("PMIN_DELTA",         data.cfg_info.pmin_percentage,     DEFAULT_PMIN_DELTA_PERCENTAGE,     0, 100)
    CFG_ITEM_INT("PMAX_DELTA",         data.cfg_info.pmax_percentage,     DEFAULT_PMAX_DELTA_PERCENTAGE,     0, 100)
    CFG_ITEM_INT("TRANSPORT_GUARD",    data.cfg_info.transport_guard,     DEFAULT_TRANSPORT_GUARD,           0, MAX_INT_32)
    CFG_ITEM_INT("DEFAULT_PSM_DELTA",  data.cfg_info.default_psm_delta,   DEFAULT_PSM_DELTA,                 0, MAX_INT_32)

    /* Max count related configuration*/
    CFG_ITEM_INT("MAX_NO_OF_FACTORY_RESET_PER_HOUR_BY_SMS", data.cfg_info.factory_reset_limit,      DEFAULT_FACTORY_RESET_RATE_LIMIT, 0, MAX_INT_32)
    CFG_ITEM_INT("MAX_NO_OF_REG_UPDATE_PER_HOUR_BY_SMS",    data.cfg_info.reg_update_limit,         DEFAULT_REG_UPDATE_RATE_LIMIT,    0, MAX_INT_32)
    CFG_ITEM_INT("REBOOTSTRAP_REG_FAILS_MAX_NO_ATTEMPTS",   data.cfg_info.max_rebootstrap_attempts, 0,                                0, MAX_INT_32)
  	
    /* Other configuration */
    CFG_ITEM_INT("BATTERY_LEVEL_THRESHOLD", data.cfg_info.battery_level_threshold, DEFAULT_BATTERY_LEVEL_THRESHOLD, 0, MAX_BATTERY_LEVEL)
    CFG_ITEM_INT("NOTIFICATIONS_STORE",     data.cfg_info.store_notifications,     DEFAULT_STORE_NOTIFICATION,      0, 0xFF)
    CFG_ITEM_INT("CONNECTION_COUNT",        data.cfg_info.connection_count,        DEFAULT_CONNECTION_COUNT,      0, 0xFF)

    CFG_ITEM_INT("BLOCKWISE_BLOCK_SIZE",     data.cfg_info.blockSize,              DEFAULT_BLOCK_MAX_SIZE,      DEFAULT_BLOCK_MIN_SIZE, DEFAULT_BLOCK_MAX_SIZE)
  };

  lwm2m_cfg_item_str lwm2m_cfg_str_table[] = 
  {
    CFG_ITEM_STR("APN",       data.cfg_info.apn_info, "")
    CFG_ITEM_STR("IP_FAMILY", data.cfg_info.ipFamily_str, "v4")
    CFG_ITEM_STR("VERSION",   data.cfg_info.version_str, "1.0")
    CFG_ITEM_STR("BINDINGS",  data.cfg_info.binding_str, "ip_nonip")
    CFG_ITEM_STR("SKIP_DATACALL_RETRY_CE_REASON",   data.cfg_info.skip_call_retry_ce_reason_str, "none")
  };

  setlocale(LC_ALL,"C");

  bool_table_size    = sizeof(lwm2m_cfg_bool_table)/sizeof(lwm2m_cfg_item_bool);
  str_table_size     = sizeof(lwm2m_cfg_str_table)/sizeof(lwm2m_cfg_item_str);
  integer_table_size = sizeof(lwm2m_cfg_integer_table)/sizeof(lwm2m_cfg_item_integer);

  process_cfg_utility_set_as_default(lwm2m_cfg_str_table,  
                                     str_table_size, 
                                     lwm2m_cfg_bool_table, 
                                     bool_table_size, 
                                     lwm2m_cfg_integer_table, 
                                     integer_table_size);

  buf1 = process_cfg_utility_open_file(LWM2M_CFG_FILE_PATH, &read_bytes, &cfg_fd);
  buf2 = process_cfg_utility_open_file(OBJECTS_CFG_FILE_PATH, &read_bytes2, &cfg_fd2);
  
  if (buf1 == NULL && buf2 == NULL)
  {
    ds_iot_log_strf_high(" can't read config files :%s %s", LWM2M_CFG_FILE_PATH, OBJECTS_CFG_FILE_PATH);
    goto end_cleanup;
  }
  
  buf = lwm2m_malloc(read_bytes+read_bytes2+1);
  if (buf == NULL)
  {
    ds_iot_ulog_err("LWM2M_LOG: Memory Allocation FAILED");
    goto end_cleanup;
  }

  memset(buf, 0x00, read_bytes+read_bytes2+1);

  if (buf1 == NULL)
  	 strlcpy(buf, buf2, read_bytes2);
  else if (buf2 == NULL)
  	 strlcpy(buf,buf1, read_bytes);
  else
  {
    strlcpy(buf, buf1, read_bytes);
    temp = strstr(buf,"}");
    temp2 = strstr(buf2,"{");

    if (temp)
    {
      if (temp2)
        strlcpy(temp,temp2+1,read_bytes2 - (temp2 +1 - buf2));
      else
        strlcpy(buf, buf1, read_bytes);
    }
    else
    {
      strlcpy(buf,buf2,read_bytes2);
    }
  }
  
  /* Handle config content */
  {
    char key[64];
    char value[64];
    char *buf_to_process = strstr(buf, "{"); 
    char *buf_end        = strstr(buf, "}");

    LOG_DEBUG(" Number of bytes read for config files :%d", read_bytes+read_bytes2);

   	if ((buf_to_process == NULL) || 
        (buf_end == NULL) ||                /* Wrong cfg format scenario */
        ((buf_end - buf_to_process) == 1))  /* Empty content scenario */
    {
      ds_iot_ulog_high("parsing of lwm2m cfg/objects cfg files failed load proper cfg file ");
      goto end_cleanup;
   	}

   	*buf_end = 0; /* Ignore all content after end-mark "}" */

    buf_to_process++;
    while (buf_to_process != NULL && *buf_to_process != 0)
   	{
      /* Ignore the \t and \n\r and empty-space */
      if(*buf_to_process == '\n' || *buf_to_process == '\r' || *buf_to_process == '\t' || *buf_to_process == ' ')
      {
        buf_to_process++;
        continue;
      }
      else
      {
        char* end_of_item = NULL;
        char* equal_pos = NULL;

        end_of_item = strstr(buf_to_process, ";");
        equal_pos   = strstr(buf_to_process, "=");

        if (end_of_item == NULL || equal_pos == NULL || end_of_item < equal_pos)
        {
          ds_iot_log_strf_err("Can't find \";\" or \"=\" after charater[%c](%d) ", *buf_to_process, *buf_to_process);
          ds_iot_ulog_err("Parsing of LwM2M cfg file failed, reload default value!");
		   
          process_cfg_utility_set_as_default(lwm2m_cfg_str_table,
                                             str_table_size,
                                             lwm2m_cfg_bool_table,
                                             bool_table_size,
                                             lwm2m_cfg_integer_table,
                                             integer_table_size);
           goto end_cleanup;
         }

         memset (key,   0, sizeof(key));
         memset (value, 0, sizeof(value));
         strlcpy(key, buf_to_process, equal_pos - buf_to_process + 1);
         strlcpy(value, equal_pos + 1, end_of_item - equal_pos);

         process_cfg_utility_set_value(key, 
                                       value,
                                       lwm2m_cfg_str_table,
                                       str_table_size, 
                                       lwm2m_cfg_bool_table,
                                       bool_table_size,
                                       lwm2m_cfg_integer_table,
                                       integer_table_size);
			
        buf_to_process = end_of_item + 1;
      }
    }  
  }

  end_cleanup:
  {
    if (cfg_fd != -1)
    {
      efs_close(cfg_fd);
      cfg_fd = -1;
    }

    if (cfg_fd2 != -1)
    {
      efs_close(cfg_fd2);
      cfg_fd2 = -1;
    }
	
    if (buf1 != NULL)
    {
      lwm2m_free(buf1);
      buf1 = NULL;
    }

    if (buf2 != NULL)
    {
      lwm2m_free(buf2);
      buf2 = NULL;
    }

    if (buf != NULL)
    {
      lwm2m_free(buf);
      buf = NULL;
    }
  }

  {
    /* Calculate the retry timeout values */ 
    if (calculate_retry_counter_values(data.cfg_info.retry_timeout, 
                                       data.cfg_info.retry_exponent_val,
                                       data.cfg_info.max_retry_timeout,
                                       data.cfg_info.max_retries, 
                                       &data.cfg_info.retry_timeout_lst,
                                       &data.cfg_info.retry_timeout_lst_len))
    {
      ds_iot_ulog_err("Generating retry counter list from the config items failed");
      return -1;
    }

    /* 
     ** Validate cfg data 
     */
    if (!((LWM2M_IMEI_URN       == data.cfg_info.reg_endpoint_name)   || 
      	   (LWM2M_IMEI_MSISDN_URN == data.cfg_info.reg_endpoint_name)   || 
      	   (LWM2M_IMEI_IMSI       == data.cfg_info.reg_endpoint_name)   || 
      	   (LWM2M_IMEI_IMSI_URN   == data.cfg_info.reg_endpoint_name)))
    {
      data.cfg_info.reg_endpoint_name = LWM2M_IMEI_URN;
    }

    if (!((LWM2M_IMEI_URN       == data.cfg_info.bootstrap_endpoint_name)   || 
  	      (LWM2M_IMEI_MSISDN_URN == data.cfg_info.bootstrap_endpoint_name)   || 
  	      (LWM2M_IMEI_IMSI       == data.cfg_info.bootstrap_endpoint_name)   || 
  	      (LWM2M_IMEI_IMSI_URN   == data.cfg_info.bootstrap_endpoint_name)))
    {
      data.cfg_info.bootstrap_endpoint_name = LWM2M_IMEI_MSISDN_URN;
    }

    /* Derive string type configuration to other type, include ip family, version, binding, ...*/         
    {
      uint32_t cntr = 0x00; 
      float version = (float)atof(data.cfg_info.version_str);
      uint8_t version_cnt = sizeof(supported_lwm2m_ver)/sizeof(float);

      /* LwM2M version not configured - configure to the lowest version supported */ 
      data.cfg_info.lwm2m_version = LWM2M_V_1_0;
      for (cntr = 0; cntr < version_cnt; cntr++)
      {
        if (supported_lwm2m_ver[cntr] == version)
        {
          data.cfg_info.lwm2m_version = version;
          break;
        }
      }

      ds_iot_log_strf_high("LWM2M VERSION = %f", data.cfg_info.lwm2m_version);  
    }
	  
    if (!strcasecmp("v4", data.cfg_info.ipFamily_str))
      data.cfg_info.ip_family = AF_INET;
    else if(!strcasecmp("v6", data.cfg_info.ipFamily_str))
      data.cfg_info.ip_family = AF_INET6;
    else if(!strcasecmp("v4v6", data.cfg_info.ipFamily_str))
      data.cfg_info.ip_family = AF_INET_DUAL46;
    else
  	   data.cfg_info.ip_family = AF_INET;    // Default use IPv4

    ds_iot_ulog_high1("IP FAMILY = %d", data.cfg_info.ip_family);

    if (data.cfg_info.store_notifications == 0)
      data.cfg_info.store_notifications = DEFAULT_STORE_NOTIFICATION;

    if (data.cfg_info.connection_count == 0)
    {
      data.cfg_info.connection_count = DEFAULT_CONNECTION_COUNT;
    }
        
    /* Skip data call retry based on call end reason */ 
    if (!strcasecmp("v4_or_v6_call_disallowed", data.cfg_info.skip_call_retry_ce_reason_str))
      data.cfg_info.skip_call_retry_ce_reason = IOT_DSS_SKIP_CALL_RETRY_CE_REASON_V4_0R_V6_CALL_DISALLOWED;
    else if (!strcasecmp("none", data.cfg_info.skip_call_retry_ce_reason_str))
      data.cfg_info.skip_call_retry_ce_reason = IOT_DSS_SKIP_CALL_RETRY_CE_REASON_NONE;
    else 
    {
      ds_iot_log_strf_high("Call end reason [%s] to skip data call - Not supported", data.cfg_info.skip_call_retry_ce_reason_str);
      return -1;
    }

    if (!strcasecmp("ip", data.cfg_info.binding_str))
      data.cfg_info.ip_support     = true;
    else if (!strcasecmp("nonip", data.cfg_info.binding_str))
      data.cfg_info.non_ip_support = true;
    else if (!strcasecmp("ip_nonip", data.cfg_info.binding_str))
    {
      data.cfg_info.ip_support     = true;
      data.cfg_info.non_ip_support = true;
    }

    /* Bindings not configured - default to both IP and Non-IP */     
    if((false == data.cfg_info.ip_support) && 
       (false == data.cfg_info.non_ip_support))
    {
      data.cfg_info.ip_support     = true;                           
      data.cfg_info.non_ip_support = true; 
      ds_iot_ulog_high("Default BINDINGS to IP + NON_IP (BINDINGS not configured)");
    } 
  }

  return 0;
}


/**
 * @fn m2m_indicate_value_change()
 * @brief This function is used to indicate change in the values of resources
 * @param type Indication type
 * @param value New value of the resource
 * @return void
 */
void m2m_indicate_value_change(qmi_change_ind_type type, char *value)
{
  size_t len = 0;
  if(data.lwm2mH == NULL || value == NULL) return;

  len = strlen(value);
  switch (type)
  {
    case QMI_CHANGE_IND_SIGNALSTRENGTH:
      value_change("/4/0/2", value, len);
      break;
    case QMI_CHANGE_IND_LINKQUALITY:
      value_change("/4/0/3", value, len);
      break;
    case QMI_CHANGE_IND_CELLID:
      value_change("/4/0/8", value, len);
      break;
    case QMI_CHANGE_IND_MNC:
      value_change("/4/0/9", value, len);
      break;
    case QMI_CHANGE_IND_MCC:
      value_change("/4/0/10", value, len);
      break;
    case QMI_CHANGE_IND_TXBYTES:
      value_change("/7/0/2", value, len);
      value_change("/7/0/5", value, len);
      break;
    case QMI_CHANGE_IND_RXBYTES:
      value_change("/7/0/3", value, len);
      value_change("/7/0/5", value, len);
      break;
    case QMI_CHANGE_IND_NETWORKBEARER:
      value_change("/4/0/0", value, len);
      break;
    case QMI_CHANGE_IND_PSM_TIMER:
      value_change("/10/0/4", value, len);
      break;
    case QMI_CHANGE_IND_ACTIVE_TIMER:
      value_change("/10/0/5", value, len);
      break;
    default:
      return;
  }
  return;
}

void proc_lwm2m_nas_status(qapi_Device_Info_t *device_info)
{
  if(NULL == device_info){
    return;
  }
  switch(device_info->id)
  {
    case QAPI_DEVICE_INFO_NETWORK_IND_E:
    {
      if(device_info->u.valuebool == true)
      {
        qurt_signal_set(&lwm2m_signal_Handle, LWM2M_NAS_UP_SIG);
        if(enable_location && enable_location_after_lte && location_init_status == LWM2M_FALSE)
          lh_location_init(); /*GPS Initialization*/
      }
    }
    break;

    case QAPI_DEVICE_INFO_ROAMING_E:
    {    
      if(roaming_feat_enabled)
      { 
        if(device_info->u.valuebool == true)
        {
          qurt_signal_set(&lwm2m_signal_Handle, LWM2M_IN_ROAMING_IND);
        }
        else
        {
          qurt_signal_set(&lwm2m_signal_Handle, LWM2M_OUT_OF_ROAMING_IND);
        }
      }
    }
    break;

    case QAPI_DEVICE_INFO_NETWORK_BEARER_E:
    {
      m2m_indicate_value_change(QMI_CHANGE_IND_NETWORKBEARER ,(char *) (device_info->u.valuebuf.buf));
    }
    break;

    case QAPI_DEVICE_INFO_MCC_E:
    {
      m2m_indicate_value_change(QMI_CHANGE_IND_MCC,(char *) (device_info->u.valuebuf.buf));
    }
    break;

    case QAPI_DEVICE_INFO_MNC_E:
    {
      m2m_indicate_value_change(QMI_CHANGE_IND_MNC,(char *) (device_info->u.valuebuf.buf));
    }
    break;

    case QAPI_DEVICE_INFO_CELL_ID_E:
    {
      m2m_indicate_value_change(QMI_CHANGE_IND_CELLID,(char *) (device_info->u.valuebuf.buf));
    }
    break;

    case QAPI_DEVICE_INFO_4G_SIG_STRENGTH_E:
    {
      m2m_indicate_value_change(QMI_CHANGE_IND_SIGNALSTRENGTH,(char *) (device_info->u.valuebuf.buf));
    }
    break;

    case QAPI_DEVICE_INFO_LINK_QUALITY_E:
    {
      m2m_indicate_value_change(QMI_CHANGE_IND_LINKQUALITY,(char *) (device_info->u.valuebuf.buf));
    }
    break;

    case QAPI_DEVICE_INFO_TX_BYTES_E:
    {
      m2m_indicate_value_change(QMI_CHANGE_IND_TXBYTES,(char *) (device_info->u.valuebuf.buf));
    }
    break;

    case QAPI_DEVICE_INFO_RX_BYTES_E:
    {
      m2m_indicate_value_change(QMI_CHANGE_IND_RXBYTES,(char *) (device_info->u.valuebuf.buf));
    }
    break;
	case QAPI_DEVICE_INFO_PSM_TIMER_E:
    {
      m2m_indicate_value_change(QMI_CHANGE_IND_PSM_TIMER,(char *) (device_info->u.valuebuf.buf));
    }
    break;
	case QAPI_DEVICE_INFO_ACTIVE_TIMER_E:
    {
      m2m_indicate_value_change(QMI_CHANGE_IND_ACTIVE_TIMER,(char *) (device_info->u.valuebuf.buf));
    }
    break;
	case QAPI_DEVICE_INFO_WMS_EVENT_REPORT_E:
       	qurt_signal_set(&lwm2m_signal_Handle, WMS_MT_MESSAGE_IND);
    break;
    
    case QAPI_DEVICE_INFO_MODEM_SERVICE_STATE_E:
    {
      if(device_info->u.valuebool == true)
      {
         qurt_signal_set(&lwm2m_signal_Handle, LWM2M_MODEM_UP_SIG);
      }
    }
    break;

    default:
    break;
  }
  if(NULL != device_info)
  {
    free(device_info);
  }

  return;
}

void lwm2m_nas_status(qapi_Device_Info_Hndl_t hndl, const qapi_Device_Info_t *device_info)
{
   lwm2m_pkt_t *pkt = NULL;
   qapi_Device_Info_t* info = NULL;

   ds_iot_ulog_high("Debug_cb: lwm2m_nas_status start.");
   if(NULL == device_info || hndl == NULL){
    return;
  }
  if(!lwm2m_is_AppRegister_Allowed())
  {
    ds_iot_ulog_high("LwM2M clean up in progress.");
    return;
  }
  info = (qapi_Device_Info_t*)malloc(sizeof(qapi_Device_Info_t));
  if(info == NULL)
  {
    LOG_DEBUG("%s",mallocError);
    return;
  }
  memset(info, 0x00, sizeof(qapi_Device_Info_t));
  memcpy(info, device_info, sizeof(qapi_Device_Info_t));
  pkt = lwm2m_get_pkt_buffer();
  if(pkt == NULL)
  {
     free(info);
     LOG_DEBUG("%s",mallocError);
     return;
  }
  memset(pkt, 0x00, sizeof(lwm2m_pkt_t));

  pkt->cmd_hdr.cmd_id = LWM2M_DEVICE_INFO_CB;
  pkt->cmd_data.cHandle = (void *)hndl;
  pkt->cmd_data.clientData = (void *)info;

  lwm2m_enqueue_pkt(pkt);

  lwm2m_set_signal();
  ds_iot_ulog_high("Debug_cb: lwm2m_nas_status end.");
  return;
}


#define CFG_INT(str, variable)  {str,(uint8_t*)&variable, sizeof(variable)},

struct lwm2m_id_int_str {
  const char* str;
  uint8_t *value;
  uint8_t  size;
};

struct lwm2m_id_int_str lwm2m_config_id_str[] = 
{
  CFG_INT("ack_timeout", data.cfg_info.ack_timeout)
  CFG_INT("ip_family", data.cfg_info.ip_family)
  CFG_INT("ip_support", data.cfg_info.ip_support)
  CFG_INT("non_ip_support", data.cfg_info.non_ip_support)
  CFG_INT("retry_timeout_lst_len", data.cfg_info.retry_timeout_lst_len)
  CFG_INT("roaming_feat", data.cfg_info.roaming_feat)
  CFG_INT("carrier_type", data.cfg_info.carrier_type)
  CFG_INT("battery_level_threshold", data.cfg_info.battery_level_threshold)
  CFG_INT("max_rebootstrap_attempts", data.cfg_info.max_rebootstrap_attempts)
  CFG_INT("reg_endpoint_name", data.cfg_info.reg_endpoint_name)
  CFG_INT("bootstrap_endpoint_name", data.cfg_info.bootstrap_endpoint_name)
  CFG_INT("boot_up_sleep", data.cfg_info.boot_up_sleep)
  CFG_INT("session_resumption_time", data.cfg_info.session_resumption_time)
  CFG_INT("max_bootstrap_wait", data.cfg_info.max_bootstrap_wait)
  CFG_INT("lwm2m_startup_mode", data.cfg_info.lwm2m_startup_mode)
  CFG_INT("reg_update_on_reconnect", data.cfg_info.reg_update_on_reconnect)
  CFG_INT("wait_time_for_rereg", data.cfg_info.wait_time_for_rereg)
  CFG_INT("wait_time_for_error", data.cfg_info.wait_time_for_error)
  CFG_INT("startup_wait_time_for_byapp_mode", data.cfg_info.startup_wait_time_for_byapp_mode)
  CFG_INT("lwm2m_rootca_support_mode", data.cfg_info.lwm2m_rootca_support_mode)
  CFG_INT("factory_reset_limit", data.cfg_info.factory_reset_limit)
  CFG_INT("reg_update_limit", data.cfg_info.reg_update_limit)
  CFG_INT("psm_enable", data.cfg_info.psm_enable)
  CFG_INT("lifetime_percentage", data.cfg_info.lifetime_percentage)
  CFG_INT("pmin_percentage", data.cfg_info.pmin_percentage)
  CFG_INT("pmax_percentage", data.cfg_info.pmax_percentage)
  CFG_INT("transport_guard", data.cfg_info.transport_guard)
  CFG_INT("default_psm_delta", data.cfg_info.default_psm_delta)
  CFG_INT("pmin_pmax_awake", data.cfg_info.pmin_pmax_awake)
  CFG_INT("enable_dtls_close_notify", data.cfg_info.enable_dtls_close_notify)
  CFG_INT("enable_dtls_epoch_0", data.cfg_info.enable_dtls_epoch_0)
  CFG_INT("store_notifications", data.cfg_info.store_notifications)
  CFG_INT("connection_count", data.cfg_info.connection_count)
  CFG_INT("roaming_feat_enabled",roaming_feat_enabled)
  CFG_INT("persis_regstatus_feat_enabled", persis_regstatus_feat_enabled)
  CFG_INT("persis_bootstrap_feat_enabled", persis_bootstrap_feat_enabled)
  CFG_INT("enable_location", enable_location)
  CFG_INT("enable_location_after_lte", enable_location_after_lte)
  CFG_INT("enable_dev_cap_obj", enable_dev_cap_obj)
  CFG_INT("enable_conn_m_obj", enable_conn_m_obj)
  CFG_INT("enable_soft_mgnt_obj", enable_soft_mgnt_obj)
  CFG_INT("enable_firmware_obj", enable_firmware_obj)
  CFG_INT("enable_conn_stat_obj", enable_conn_stat_obj)
  CFG_INT("enable_cell_nw_conn_obj", enable_cell_nw_conn_obj)
  CFG_INT("enable_apn_conn_obj", enable_apn_conn_obj)
  CFG_INT("enable_acl_obj", enable_acl_obj)
  CFG_INT("enable_bs_server_replace", enable_bs_server_replace)
};
/**
 * @brief This function is used to initialize and configure lwm2m
 * @param hlwm2mP pointer to handle of lwm2m context
 * @return void
 */
lwm2m_context_t* prv_initialize_lwm2m(void)
{
  int              result = 0;
  lwm2m_context_t *lwm2mP = NULL;
  int     bs_server_count = 0;
  qapi_Coap_Session_Info_t session_param;
  int objCount = 0;
  int i = 0;
  qapi_Device_Info_t info;

  /* Bootstrap server list */ 

  //! Initilize the variables 
  bs_ssid = DEFAULT_BS_SSID;  

  //! Perform QMI client initilize for required services 
  /*
  iotapp_qmi_dms_init();
  iotapp_qmi_nas_init();
  iotapp_qmi_dsd_init();
  iotapp_qmi_wms_init();
  iotapp_qmi_wds_init();
  iotapp_qmi_uim_init();
  */

/* Initialize the data call management module */
    iot_dss_init();


  /* Register CoAP client to use CoAP utility API*/
  memset(&session_param, 0x00, sizeof(qapi_Coap_Session_Info_t));

  session_param.coap_ack_random_factor = QAPI_COAP_ACK_RANDOM_FACTOR;
  session_param.coap_default_maxage = QAPI_COAP_DEFAULT_MAX_AGE;
  session_param.coap_max_latency = QAPI_COAP_MAX_LATENCY;
  session_param.coap_max_retransmits = QAPI_COAP_MAX_RETRANSMIT;
  session_param.coap_transaction_timeout = QAPI_COAP_RESPONSE_TIMEOUT;
  
  if (QAPI_OK != qapi_Coap_Create_Session(&lwm2m_coap_hdl, &session_param))
  {
    LOG_DEBUG("Failed to create coap session");  
    return NULL;
  }

  /* Fetch the lwm2m configuration from lwm2m_cfg */ 
  if (process_lwm2m_cfg())
  {
    return NULL;
  } 
  
  // Setting sleep timeout
  sleepAckTimeout = data.cfg_info.ack_timeout;
  g_carrier_cfg = data.cfg_info.carrier_type;
  lwm2m_psm_enable = data.cfg_info.psm_enable;
 
  timer_sleep(data.cfg_info.boot_up_sleep, T_SEC, 1);
  ds_iot_ulog_high1("LWM2M Thread was in sleep for %d seconds", data.cfg_info.boot_up_sleep);
  ds_iot_log_strf_high("Build time %s :%s", __DATE__, __TIME__);
  ds_iot_ulog_high1("carrier config value = [%d]", g_carrier_cfg);
  
  if((qapi_Device_Info_Init_v2(&lwm2m_dev_info_hdl)) != QAPI_OK) 
  {
      ds_iot_ulog_err("Shutting down Lwm2m since Device info init failed");
      return NULL;
  }
    
  /* Register callback for QMI value change indication */
  qapi_Device_Info_Set_Callback_v2(lwm2m_dev_info_hdl, QAPI_DEVICE_INFO_ANY, lwm2m_nas_status);
  
  if(qapi_Device_Info_Get_v2(lwm2m_dev_info_hdl,QAPI_DEVICE_INFO_MODEM_SERVICE_STATE_E, &info)!=QAPI_OK)
  {
      ds_iot_ulog_err("Failed to get Modem state");
      return NULL;
  }
  if(info.u.valuebool == FALSE)
  {
     ds_iot_ulog_high("Modem is not available, wait for Modem up signal");
     qurt_signal_wait(&lwm2m_signal_Handle, LWM2M_MODEM_UP_SIG, QURT_SIGNAL_ATTR_WAIT_ALL|QURT_SIGNAL_ATTR_CLEAR_MASK);
     ds_iot_ulog_high("Modem is available, proceeding");
  }
  
  for(i = 0; i< sizeof(lwm2m_config_id_str)/sizeof(struct lwm2m_id_int_str); i++)
  {
    ds_iot_log_strf_high("config: %s = %d", lwm2m_config_id_str[i].str, *lwm2m_config_id_str[i].value);
  }

  ds_iot_log_strf_high("config: lwm2m version = %f", data.cfg_info.lwm2m_version);  
  ds_iot_log_strf_high("config: apn_info = %s", data.cfg_info.apn_info);    

  q_init(&lwm2m_location_pending_read_observe);
  if (enable_location && enable_location_after_lte == LWM2M_FALSE)
    lh_location_init(); /*GPS Initialization*/ 

  //! Initilize the objects 
  if (false == prv_initialise_objects(&data, objArray,&objCount))
  {
    ds_iot_ulog_err("Failed to initialize objects");
    return NULL;
  }

  /*
   * The liblwm2m library is now initialized with the functions that will be in
   * charge of communication
   */
  lwm2mP = lwm2m_init(&data);
  if (NULL == lwm2mP)
  {
    ds_iot_ulog_err("lwm2m_init() failed");
    return NULL;
  }

#ifdef PSM_LWM2M
  /* Initialize the LwM2M PSM structures */
  if(lwm2m_psm_enable == TRUE)
  {
    lwm2m_psm_init(lwm2mP);
  }
#endif 

  result = lwm2m_configure(lwm2mP, NULL, objCount, objArray);
  if (result != 0)
  {
    ds_iot_ulog_err1("lwm2m_configure() failed: 0x%X", result);
    lwm2m_close(lwm2mP);
    return NULL;
  }

  lwm2m_location_thread_start();

  if (0 != prv_refreshServerList(lwm2mP) || 0 != prv_refreshBSServer(lwm2mP))
  {
    lwm2m_close(lwm2mP);
    return NULL;
  }

  bs_server_count = lwm2m_get_server_count(lwm2mP, TRUE);

  if (bs_server_count > 1)
  {
    ds_iot_ulog_err("Multi-bootstrap server configuration not supported");
    lwm2m_close(lwm2mP);
    return NULL;
  }
  else if(0 == bs_server_count && LWM2M_STARTUP_BOOTSTRAP_PRELOADED == data.cfg_info.lwm2m_startup_mode)
  {
    if(!bootstrap_checkAllConsistence(lwm2mP, FALSE))
    {
      ds_iot_ulog_err("Factory bootstrap consistence check failure");
      lwm2m_close(lwm2mP);
      return NULL;
    }
  }

  if (persis_regstatus_feat_enabled == LWM2M_TRUE) 
  {
    if (ESUCCESS != lwm2m_carrier_init_regstatus(lwm2mP))
    {
      /*registered status can't recover, so delete the ext-obj-registred persitent file too*/
      efs_unlink(LWM2M_REGISTERED_EXTOBJ_PERSISTENCE_FILE);
    }
    else
    {
      loadRegisterdExtObj(lwm2mP);
    }
  }

  if (process_carrier_apn_cfg(lwm2mP) == EFAILURE) 
  {
    ds_iot_ulog_err("Failed to initialize carrier apn from cfg"); 
    lwm2m_close(lwm2mP); 
    return NULL;
  } 
  else 
  {
    if(g_carrier_cfg && enable_conn_m_obj && load_conn_mon_persistent_info(lwm2mP) == ESUCCESS)
    {
      ds_iot_ulog_err("APN values are restored from persistence file");
    }
    else if (enable_conn_m_obj && populate_carrier_apn_info_to_conn_m_obj(lwm2mP) == EFAILURE) 
    {
      ds_iot_ulog_err("FAILED TO UPDATE THE APN CFG TO CONNECTION MONITORING OBJ ");
    }
  }

  /* Restore the active server list from persistence */
  if (persis_regstatus_feat_enabled == LWM2M_TRUE) 
  {
    if (sync_activeserverlist_from_persistence(lwm2mP) == EFAILURE)
    {
      ds_iot_ulog_err("Failed to restore active serverlist");
    }
    else
    {
      LOG_DEBUG("Restored active serverlist from persistence");
    }
  }
  
  // Restore the observation info from persistent storage
  if (persis_regstatus_feat_enabled == LWM2M_TRUE) 
  {
    if (sync_observations_from_persistence(lwm2mP) == EFAILURE)
    {
      ds_iot_ulog_err("Failed to restore observations");
    }
    else
    {
      LOG_DEBUG("Restored observations from persistence");
      qurt_signal_set(&lwm2m_signal_Handle, LOCATION_PMIN_UPDATED);
    }
  }

  /* Fetching Network bearer and Available Network bearer */
  if( enable_conn_m_obj && m2m_set_network_bearer_to_conn_m_obj(lwm2mP) == EFAILURE)
  {
    LOG_DEBUG("Failed to set network bearer value");
  }

  init_value_change(lwm2mP);
  glwm2m_context = true;

  return lwm2mP;
}

/**
 * @brief This function is used to update error code (/3/0/11) resource with IP connectivity 
 *         failure error code in case of data link down
 *@param  lwm2mH handle to lwm2m context
 */
static void prv_set_ipconnectivity_errorcode(lwm2m_context_t *lwm2mH)
{
  lwm2m_uri_t uri;

  if (lwm2m_stringToUri("/3/0/11", strlen("/3/0/11"), &uri))
  {
    handle_value_changed(lwm2mH, &uri, (const char *)"7", 1);
  }
}


/**
 * @brief This function is callback to iot app dss utils for
 *        primary/ default apn  datacall. 
 * @param status  status of the datacall
 */
static void prv_primary_apn_datacall_callback_v4(iot_dss_call_hndl_t hndl, void *user_data, uint8_t call_status)
{
  uint32_t short_server_id = (uint32_t)user_data;
  carrier_apn_cfg_t *apn_conf = get_lwm2m_apn_cfg();


  ds_iot_ulog_high2("Primary APN callback for IPv4 with status[%d] for SSID[%d]", call_status, short_server_id);

  if ((IOT_DSS_NAS_DOWN_IND != call_status) &&
      (NULL == (apn_conf = (carrier_apn_cfg_t *)LWM2M_LIST_FIND(apn_conf, short_server_id))))
    return; 
  
  switch (call_status)
  {
    case IOT_DSS_DATACALL_UP:
    {
      /* Registration server shares the APN with the bootstrap server */
      if (apn_conf->bs_match_callHdl_v4_nip)
      {
        /* Tear-down the bootstrap server associated data call at this point, as the matched registration server APN 
         * is already brought-up. Modem wouldn't tear-down the call internally, but just decrement the reference count. 
         */ 
        iot_dss_stop_datacall(apn_conf->bs_match_callHdl_v4_nip);
        iot_dss_deregister(apn_conf->bs_match_callHdl_v4_nip);
        /* Reset the stored boostrap handle */
        apn_conf->bs_match_callHdl_v4_nip = NULL; 
      }   

      apn_conf->v4_nip_call_state = LWM2M_DATA_CALL_CONNECTED; 
      qurt_signal_set(&lwm2m_signal_Handle, DSS_M2M_PRIMARY_APN_CONN_v4);      
      break;
    }

    case IOT_DSS_DATACALL_FAILURE:
    case IOT_DSS_DATACALL_DOWN:
    {
      apn_conf->v4_nip_call_state = (IOT_DSS_DATACALL_DOWN == call_status) ? LWM2M_DATA_CALL_DISCONNECTED : LWM2M_DATA_CALL_FAILURE;
      qurt_signal_set(&lwm2m_signal_Handle, DSS_M2M_PRIMARY_APN_NO_NET_v4);
      break;
    }

    case IOT_DSS_NAS_DOWN_IND:
    {
      qurt_signal_set(&lwm2m_signal_Handle, DSS_M2M_WAIT_FOR_NAS_UP);
      break;
    }

    case IOT_DSS_MAX_RETRIES:
    {
      qurt_signal_set(&lwm2m_signal_Handle, DSS_M2M_APN_MAX_RETRIES);
      break;
    }
  }
}


/**
 * @brief This function is callback to iot app dss utils for
 *        secondary apn 1 datacall. 
 * @param status  status of the datacall
 */
static void prv_secondary_apns_datacall_callback_v4(iot_dss_call_hndl_t hndl, void *user_data, uint8_t call_status)
{
  uint32_t short_server_id = (uint32_t)user_data;
  carrier_apn_cfg_t *apn_conf = get_lwm2m_apn_cfg();

  ds_iot_ulog_high2("Secondary APN callback for IPv4 with status[%d] for SSID[%d]", call_status, short_server_id);

  if ((IOT_DSS_NAS_DOWN_IND != call_status) &&
      (NULL == (apn_conf = (carrier_apn_cfg_t *)LWM2M_LIST_FIND(apn_conf, short_server_id))))
    return; 

  switch (call_status)
  {
    case IOT_DSS_DATACALL_UP:
    {
      /* Registration server shares the APN with the bootstrap server */
      if (apn_conf->bs_match_callHdl_v4_nip)
      {
        /* Tear-down the bootstrap server associated data call at this point, as the matched registration server APN 
         * is already brought-up. Modem wouldn't tear-down the call internally, but just decrement the reference count. 
         */ 
        iot_dss_stop_datacall(apn_conf->bs_match_callHdl_v4_nip);
        iot_dss_deregister(apn_conf->bs_match_callHdl_v4_nip);
        /* Reset the stored boostrap handle */
        apn_conf->bs_match_callHdl_v4_nip = NULL; 
      }   

      apn_conf->v4_nip_call_state = LWM2M_DATA_CALL_CONNECTED;
      qurt_signal_set(&lwm2m_signal_Handle, DSS_M2M_SECONDARY_APN_CONN_v4);
      break;
    }

    case IOT_DSS_DATACALL_FAILURE:
    case IOT_DSS_DATACALL_DOWN:
    {
      apn_conf->v4_nip_call_state = (IOT_DSS_DATACALL_DOWN == call_status) ? LWM2M_DATA_CALL_DISCONNECTED : LWM2M_DATA_CALL_FAILURE;
      qurt_signal_set(&lwm2m_signal_Handle, DSS_M2M_SECONDARY_APN_NO_NET_v4);
      break;
    }

    case IOT_DSS_NAS_DOWN_IND:
    {
      qurt_signal_set(&lwm2m_signal_Handle, DSS_M2M_WAIT_FOR_NAS_UP);
      break;
    }

    case IOT_DSS_MAX_RETRIES:
    {
      /* Ignore the event for secondary APNs */
      break;
    }
  }
}


/**
 * @brief This function is callback to iot app dss utils for
 *        primary/ default apn  datacall. 
 * @param status  status of the datacall
 */
static void prv_primary_apn_datacall_callback_v6(iot_dss_call_hndl_t hndl, void *user_data, uint8_t call_status)
{
  uint32_t short_server_id = (uint32_t)user_data;
  carrier_apn_cfg_t *apn_conf = get_lwm2m_apn_cfg();

  ds_iot_ulog_high2("Primary APN callback for IPv6 with status[%d] for SSID[%d]", call_status, short_server_id);
  
  if ((IOT_DSS_NAS_DOWN_IND != call_status) &&
      (NULL == (apn_conf = (carrier_apn_cfg_t *)LWM2M_LIST_FIND(apn_conf, short_server_id))))
    return;

  switch (call_status)
  {
    case IOT_DSS_DATACALL_UP:
    {
      /* Registration server shares the APN with the bootstrap server */
      if (apn_conf->bs_match_callHdl_v6)
      {
        /* Tear-down the bootstrap server associated data call at this point, as the matched registration server APN 
         * is already brought-up. Modem wouldn't tear-down the call internally, but just decrement the reference count. 
         */ 
        iot_dss_stop_datacall(apn_conf->bs_match_callHdl_v6);
        iot_dss_deregister(apn_conf->bs_match_callHdl_v6);
        /* Reset the stored boostrap handle */
        apn_conf->bs_match_callHdl_v6 = NULL; 
      }

      apn_conf->v6_call_state = LWM2M_DATA_CALL_CONNECTED;
      qurt_signal_set(&lwm2m_signal_Handle, DSS_M2M_PRIMARY_APN_CONN_v6);
      break;
    }
    
    case IOT_DSS_DATACALL_FAILURE:
    case IOT_DSS_DATACALL_DOWN:
    {
      apn_conf->v6_call_state = (IOT_DSS_DATACALL_DOWN == call_status) ? LWM2M_DATA_CALL_DISCONNECTED : LWM2M_DATA_CALL_FAILURE;
      qurt_signal_set(&lwm2m_signal_Handle, DSS_M2M_PRIMARY_APN_NO_NET_v6);
      break;
    }

    case IOT_DSS_NAS_DOWN_IND:
    {
      qurt_signal_set(&lwm2m_signal_Handle, DSS_M2M_WAIT_FOR_NAS_UP);
      break;
    }

    case IOT_DSS_MAX_RETRIES:
    {
      qurt_signal_set(&lwm2m_signal_Handle, DSS_M2M_APN_MAX_RETRIES);
      break;
    }    
  }
}


/**
 * @brief This function is callback to iot app dss utils for
 *        secondary apn 1 datacall. 
 * @param status  status of the datacall
 */
static void prv_secondary_apns_datacall_callback_v6(iot_dss_call_hndl_t hndl, void *user_data, uint8_t call_status)
{
  uint32_t short_server_id = (uint32_t)user_data;
  carrier_apn_cfg_t *apn_conf = get_lwm2m_apn_cfg();
 
  ds_iot_ulog_high2("Secondary APN callback for IPv6 with status[%d] for SSID[%d]", call_status, short_server_id);
 
  if ((IOT_DSS_NAS_DOWN_IND != call_status) &&
      (NULL == (apn_conf = (carrier_apn_cfg_t *)LWM2M_LIST_FIND(apn_conf, short_server_id))))
    return;

  switch (call_status)
  {
    case IOT_DSS_DATACALL_UP: 
    {
      /* Registration server shares the APN with the bootstrap server */
      if (apn_conf->bs_match_callHdl_v6)
      {
        /* Tear-down the bootstrap server associated data call at this point, as the matched registration server APN 
         * is already brought-up. Modem wouldn't tear-down the call internally, but just decrement the reference count. 
         */ 
        iot_dss_stop_datacall(apn_conf->bs_match_callHdl_v6);
        iot_dss_deregister(apn_conf->bs_match_callHdl_v6);
        /* Reset the stored boostrap handle */
        apn_conf->bs_match_callHdl_v6 = NULL; 
      }

      apn_conf->v6_call_state = LWM2M_DATA_CALL_CONNECTED;
      qurt_signal_set(&lwm2m_signal_Handle, DSS_M2M_SECONDARY_APN_CONN_v6);
      break;
    }

    case IOT_DSS_DATACALL_FAILURE:
    case IOT_DSS_DATACALL_DOWN:
    {
      apn_conf->v6_call_state = (IOT_DSS_DATACALL_DOWN == call_status) ? LWM2M_DATA_CALL_DISCONNECTED : LWM2M_DATA_CALL_FAILURE;
      qurt_signal_set(&lwm2m_signal_Handle, DSS_M2M_SECONDARY_APN_NO_NET_v6);
      break;
    }

    case IOT_DSS_NAS_DOWN_IND:
    {
      qurt_signal_set(&lwm2m_signal_Handle, DSS_M2M_WAIT_FOR_NAS_UP);
      break;
    }

    case IOT_DSS_MAX_RETRIES:
    {
      /* Ignore the event for secondary APNs */
      break;
    }
  }
}


/*======================================================================
 * Function qmi_wms_read_msg_req
 * =====================================================================*/
/*
 * @breif : API will send mt message event report req msg
 *
 * @param[In/Out] : void
 * @return : ESUCCESS if success otherwise EFAILURE
 */

/*=====================================================================*/
void qmi_wms_read_msg_req()
{

  char *response = NULL;
  int response_len = -1;
  
  if (routed_msg)
  {
    response_len = get_sms_payload((const unsigned char *)trans_msg_data, trans_msg_datalen, &response, routed_msg);

    if (response_len != -1)
    {
      parse_wms_message_received(response ,response_len);

      if (NULL == sms_msg.uri)
      {
        LOG_DEBUG("Not valid SMS ");
      }
      else
      {
        ds_iot_ulog_high("Wakeup SMS successfully handled");
      }

      if (response)
      {
        lwm2m_free(response);
        response = NULL;
      }

      routed_msg = false;
      return;
    } 

    else
    {
      routed_msg = false;
      ds_iot_ulog_high("Error in SMS payload parsing ");

      return;
    }
  }

  response_len = qmi_wms_raw_read_msg(&response);
  
  if (response_len != -1 && response != NULL)
  {
 	  ds_iot_log_strf_high("Decoded msg is: %s",response);
 	  parse_wms_message_received(response ,response_len);
 
   	if (NULL == sms_msg.uri)
   	{
   	  LOG_DEBUG("Not valid Wakeup SMS ");
   	}
   	else
   	{
   	  ds_iot_ulog_high("Wakeup SMS received for register trigger");  
   	}
 
   	if (response)
   	{
   	  lwm2m_free(response);
   	  response = NULL;
   	}

   	return;
  } 
  else
  {
	   ds_iot_ulog_high("Error in SMS payload parsing ");
	   return ;
  }
}

bool check_if_sms_rate_exceeded_and_update(lwm2m_sms_operation_E sms_oper)
{
  int fd = 0, ret = 0;
  sms_rate_info_t info;
  time_t curr_time = lwm2m_gettime();
  uint16_t sms_cnt = 0;
  ds_iot_log_strf_med("EFS_OPT: File open %s",SMS_RATE_LIMIT_FILE);
  fd = efs_open(SMS_RATE_LIMIT_FILE, O_RDWR);
  memset(&info, 0x00, sizeof(info));
  ds_iot_ulog_high1("SMS operation : %d", sms_oper);
  GET_SMS_LIMIT(sms_oper, sms_cnt);
  
  if (EFAILURE == fd)
  {
    if(sms_cnt > 0)
    {
      ds_iot_ulog_high("No SMS rate limit File. Creating one ");
      ds_iot_log_strf_med("EFS_OPT: File open %s",SMS_RATE_LIMIT_FILE);
      fd = efs_open(SMS_RATE_LIMIT_FILE, O_CREAT | O_RDWR, 00644);
      info.sms_count_per_hour = sms_cnt - 1;
      info.expiry_time = curr_time+(60*60);
      info.isValid = LWM2M_TRUE;
      efs_lseek(fd, sms_oper * sizeof(info), SEEK_SET);
      ds_iot_log_strf_med("EFS_OPT: File write %s",SMS_RATE_LIMIT_FILE);
      if (EFAILURE == efs_write(fd, &info, sizeof(info)))
      {
        ds_iot_ulog_err("Failed to store SMS info");
        efs_close(fd);
      }
  
      efs_close(fd);
      return LWM2M_TRUE;
    }
	else
    {
      return LWM2M_FALSE;
    }
  }
  else
  {
    efs_lseek(fd, sms_oper * sizeof(info), SEEK_SET);
    ret = efs_read(fd, &info, sizeof(info));
    if (ret <= 0)
    {
      efs_close(fd);
      return LWM2M_FALSE;
    }
    ds_iot_ulog_high1("Count remaining %d ", info.sms_count_per_hour);
    if(info.isValid == LWM2M_TRUE)
    {
      if(info.sms_count_per_hour > 0)
      {
        if(curr_time <= info.expiry_time)
        {
          info.sms_count_per_hour--;
        }
        else
        {
          info.sms_count_per_hour = sms_cnt - 1;
          info.expiry_time = curr_time+(60*60);
        }
        efs_lseek(fd, sms_oper * sizeof(info), SEEK_SET);
        ds_iot_log_strf_med("EFS_OPT: File write %s",SMS_RATE_LIMIT_FILE);
        if (EFAILURE == efs_write(fd, &info, sizeof(info)))
        {
          ds_iot_ulog_err("Failed to store SMS info");
          efs_close(fd);
          return LWM2M_FALSE;
        }
        efs_close(fd);
        return LWM2M_TRUE;
      }
      else if(info.sms_count_per_hour <= 0)
      {
        if(curr_time <= info.expiry_time)
        {
          efs_close(fd);
          return LWM2M_FALSE;
        }
        else
        {
          info.sms_count_per_hour = sms_cnt - 1;
          info.expiry_time = curr_time+(60*60);
          efs_lseek(fd, sms_oper * sizeof(info), SEEK_SET);
          ds_iot_log_strf_med("EFS_OPT: File write %s",SMS_RATE_LIMIT_FILE);
          if (EFAILURE == efs_write(fd, &info, sizeof(info)))
          {
            ds_iot_ulog_err("Failed to store SMS info");
            efs_close(fd);
            return LWM2M_FALSE;
          }
          efs_close(fd);
          return LWM2M_TRUE;
        }
      }
      else
      {
        efs_close(fd);
        return LWM2M_FALSE;
      }
    }
    else
    {
      if(sms_cnt > 0)
      {
        ds_iot_ulog_high("Entry not present in the persistence, create one");
  
  	    info.sms_count_per_hour = sms_cnt - 1;
        info.expiry_time = curr_time+(60*60);
        info.isValid = LWM2M_TRUE;
        efs_lseek(fd, sms_oper * sizeof(info), SEEK_SET);
        ds_iot_log_strf_med("EFS_OPT: File write %s",SMS_RATE_LIMIT_FILE);
        if (EFAILURE == efs_write(fd, &info, sizeof(info)))
        {
          ds_iot_ulog_err("Failed to store SMS info");
          efs_close(fd);
        }
    
        efs_close(fd);
        return LWM2M_TRUE;
      }
      else
      {
        efs_close(fd);
        return LWM2M_FALSE;
      }
    }
  }
}

/**
 * @brief This function is used to handle signals to lwm2m thread
 *        (both internal and external)
 * @param hlwm2mP pointer to handle of lwm2m context
 * @param tv_secP pointer to timeout value 
 * @void  void
 */
void process_signal(lwm2m_context_t *lwm2mH, time_t tv_secP)
{
  int res = 0;
  uint32 signals = 0;

  
  if (NULL == lwm2mH)
    return;  

  timer_set(&signalWaitTimeout, tv_secP, 0, T_SEC);

  res = qurt_signal_wait_timed_ext(&lwm2m_signal_Handle, 
      (DSS_M2M_PRIMARY_APN_CONN_v4 | DSS_M2M_PRIMARY_APN_NO_NET_v4 | DSS_M2M_SECONDARY_APN_CONN_v4 |
       DSS_M2M_SECONDARY_APN_NO_NET_v4 | DSS_M2M_PRIMARY_APN_CONN_v6 | DSS_M2M_PRIMARY_APN_NO_NET_v6 | DSS_M2M_SECONDARY_APN_CONN_v6 |
       DSS_M2M_SECONDARY_APN_NO_NET_v6 | LWM2M_TRIGGER_DISABLE | LWM2M_UNBLOCK_EVENT |
       LWM2M_IN_ROAMING_IND | WMS_MT_MESSAGE_IND | LWM2M_CONN_MONI_SCND_APN_CHANGE | LWM2M_CONN_MONI_CLASS6_APN_CHANGE | LWM2M_CONN_MONI_CLASS7_APN_CHANGE |
       LWM2M_BATTERY_IND | LWM2M_REBOOT_SET |LWM2M_FACTORY_RESET_SET| DSS_M2M_WAIT_FOR_NAS_UP |
       LOCATION_SINGLE_SHOT_CALLBACK|LOCATION_PMIN_UPDATED|LWM2M_RESTART|LWM2M_PS_ATTACH | LWM2M_PS_DETACH | LOCATION_FETCH), 
       QURT_SIGNAL_ATTR_CLEAR_MASK | QURT_SIGNAL_ATTR_WAIT_ANY, &signals, qurt_timer_convert_time_to_ticks(tv_secP, QURT_TIME_SEC));

  timer_clr(&signalWaitTimeout, T_SEC);

  if (QURT_EFAILED_TIMEOUT == res)
  {
    LOG_DEBUG("Signal wait timed out...");
    return;
  }
  else if (QURT_EOK != res)
  {
    ds_iot_ulog_err("Signal wait timeout out failed...");
    return;
  }

  if (signals & LWM2M_REBOOT_SET) 
  {
    ds_iot_ulog_high("lwm2m processing Reboot signal");
    g_reboot = 1;
    qurt_signal_clear(&lwm2m_signal_Handle, LWM2M_REBOOT_SET);
    return;
  } 
  if(signals & LWM2M_RESTART) {
    ds_iot_ulog_high("lwm2m processing Restart signal");
    g_restart = 1;
    qurt_signal_clear(&lwm2m_signal_Handle, LWM2M_RESTART);
    return;  	
  }
  if(signals & LWM2M_FACTORY_RESET_SET) 
  {
    ds_iot_ulog_high("lwm2m processing factory reset signal");
    factory_reset_req = true;
    reset_global_flags(LWM2M_FALSE);
    qurt_signal_clear(&lwm2m_signal_Handle, LWM2M_FACTORY_RESET_SET);
    return;
  }

#if 0
  if (signals & LWM2M_DISABLE_TIMEDOUT)
  {
    timer_sleep(1000, T_MSEC, 1);
    qurt_signal_clear(&lwm2m_signal_Handle, LWM2M_DISABLE_TIMEDOUT);
    LOG_INFO("disable signal processing done , cleared the signal");
  }
#endif

  if (signals & WMS_MT_MESSAGE_IND)
  {
    lwm2m_uri_t *uri_sms = NULL;

    ds_iot_ulog_high1(" Process signal WMS.... sleep state %d !!! ", lwm2m_in_sleep_state);
    /* Process the received SMS message from the server */
    qmi_wms_read_msg_req();    
   
    /* Send ACK for the received SMS request */
    if (trans_ack_indicator == WMS_ACK_INDICATOR_SEND_ACK_V01)
      qmi_wms_send_msg_ack();

    uri_sms = sms_msg.uri;

    if (uri_sms == NULL)
    {
      LOG_DEBUG("Invalid SMS payload...!!! ");
      goto next_step;
    } 
    else 
    {
      uint8_t server_count = 0;
	  uint16_t shortId[10] = {0};
      lwm2m_sms_operation_E sms_oper;
      uint16_t object_id = 0x00;
      uint16_t instance_id = 0x00;
      uint16_t resource_id = 0x00;
      lwm2m_server_t *targetP = NULL;
      lwm2m_object_t *serv_obj;
      lwm2m_object_t *sec_obj = NULL;
      server_instance_t* serv_obj_inst;
#ifdef AUTHORIZATION_SUPPORT
      acc_ctrl_oi_t *acc_ctrl_oi = NULL;
      acc_ctrl_ri_t *acc_ctrl_ri = NULL;      
      bool isAuthorized = LWM2M_FALSE;
#endif
      int cntr = 0;
	  memset(shortId, 0, sizeof(shortId));
      qurt_signal_clear(&lwm2m_signal_Handle, WMS_MT_MESSAGE_IND);

      /* Object ID */ 
      if (uri_sms->flag & LWM2M_URI_FLAG_OBJECT_ID)
        object_id = uri_sms->objectId;

      /* Instance ID */ 
      if (uri_sms->flag & LWM2M_URI_FLAG_INSTANCE_ID)
        instance_id = uri_sms->instanceId;

      /* Resource ID */
      if (uri_sms->flag & LWM2M_URI_FLAG_RESOURCE_ID)
        resource_id = uri_sms->resourceId;      
        
      ds_iot_ulog_high3("Object id %x / Object instance id %x /Resource id %x,", object_id, instance_id, resource_id);
      ds_iot_log_strf_high( " SMS Number %s ", sms_number_from_wms);

      sec_obj = (lwm2m_object_t *)LWM2M_LIST_FIND(lwm2mH->objectList , LWM2M_SECURITY_OBJECT_ID);
      if (NULL != sec_obj)
        {
          security_instance_t *instanceP;
          instanceP = (security_instance_t *)sec_obj->instanceList;
          while (NULL != instanceP)
          {
            if ((instanceP->isBootstrap != LWM2M_TRUE) && 
                (instanceP->sms_number != NULL) && 
                (!strcmp(instanceP->sms_number, sms_number_from_wms)))
            {
              shortId[cntr] = instanceP->shortID;
              cntr++;
            }

            instanceP = instanceP->next;
          }
      }
    
      server_count = cntr;
      if (server_count == 0)
      {
        LOG_DEBUG("No server with matching SMS number");
        goto next_step;;
      }

      ds_iot_ulog_high1("Server count with matching SMS number %d", server_count);     
#ifdef AUTHORIZATION_SUPPORT
      acc_ctrl_oi = get_acc_cl_objectinst(lwm2mH, sms_msg.uri);
      if(!enable_acl_obj || NULL == acc_ctrl_oi || 1 == lwm2m_get_server_count(data.lwm2mH, FALSE))
      {
		ds_iot_ulog_high("no ACL object instance or single DM server, operation with full permission");
        isAuthorized = LWM2M_TRUE;
        goto next;
      } 
 
      for (cntr = 0; cntr < server_count; cntr++)
      {
        lwm2m_server_t *server = NULL;

        LOG_DEBUG("Checking ShortId %d", shortId[cntr]);

        server = data.lwm2mH->serverList;
        while (server)
        {
          /* Short server ID match - server context found */
          if (server->shortID == shortId[cntr])
            break; 

          server = server->next; 
        }

        lwm2mH->activeServerP = server;
        acc_ctrl_ri = get_acc_cl_resourceinst(lwm2mH, acc_ctrl_oi);
        lwm2mH->activeServerP = NULL;

        if (NULL == acc_ctrl_ri && shortId[cntr] == acc_ctrl_oi->accCtrlOwner)
        {
          isAuthorized = LWM2M_TRUE;
          break;
        }

        if (NULL != acc_ctrl_ri)
        {
          if (ACC_CTRL_PERM_EXECUTE == (ACC_CTRL_CAN_EXEC(acc_ctrl_ri->accCtrlValue))) 
          {
            isAuthorized = LWM2M_TRUE;
            break;
          }
        }
      }

      if (isAuthorized == LWM2M_FALSE)
      {
        ds_iot_ulog_high("Server is not authorized for the operation");
        goto next_step;
      }

next:
#endif
      if ((LWM2M_DEVICE_OBJECT_ID == object_id) && 
          (0x00 == instance_id))
      {
        /* Factory reset */ 
        if (uri_sms->resourceId == RES_O_FACTORY_RESET)
        {
          sms_oper = FACTORY_RESET;
        
          if (check_if_sms_rate_exceeded_and_update(sms_oper) == LWM2M_FALSE)
          {
            ds_iot_ulog_err("Factory Reset limit exceeded, not executing the operation");
            goto next_step;
          }
        
          ds_iot_ulog_high("Factory Reset through SMS");

          factory_reset_req = true;
          reset_global_flags(LWM2M_FALSE);
        
          goto next_step;
        }
      }


      if (LWM2M_SERVER_OBJECT_ID == object_id)
      {
        /* Register Update */ 
        if (LWM2M_SERVER_UPDATE_ID == uri_sms->resourceId)
        {
          
          serv_obj = (lwm2m_object_t *)LWM2M_LIST_FIND(lwm2mH->objectList , LWM2M_SERVER_OBJECT_ID);
          if (NULL == serv_obj)
          {
            ds_iot_ulog_err (" Server object list is not valid");
            goto next_step;
          }

          serv_obj_inst = (server_instance_t *)LWM2M_LIST_FIND(serv_obj->instanceList, uri_sms->instanceId);
          if (NULL == serv_obj_inst)
          {
            ds_iot_ulog_err1 (" Server instance for URI instance ID [0x%x] not found", uri_sms->instanceId);
            goto next_step;
          }

          targetP = (lwm2m_server_t *)LWM2M_LIST_FIND(lwm2mH->serverList, serv_obj_inst->shortServerId);
          if (NULL == targetP)
          {
            ds_iot_ulog_err1 (" Server object for URI instance ID [0x%x] not found", uri_sms->instanceId);
            goto next_step;
          }

          /* Discard the SMS command if the trigger is disabled for the servers supporting LwM2Mv1.1 or higher */ 
          if ((targetP->version > (float)LWM2M_V_1_0) &&
             (false == serv_obj_inst->trigger))
          {
            ds_iot_ulog_err1("Discarding the SMS message command - trigger set to false for SSID[%d]", targetP->shortID);
            goto next_step;
          }
          sms_oper = REG_UPDATE;

          /* SMS rate */ 
          if (check_if_sms_rate_exceeded_and_update(sms_oper) == LWM2M_FALSE)
          {
            ds_iot_ulog_err("Reg update limit exceeded, not execting the operation");
            goto next_step;
          }
            	
          /* Set the flags indicating "register update" is needed */ 
          if (targetP)
          {
            targetP->pending_reg_update  = TRUE;            
            targetP->isRegupdate_for_sms = TRUE;
          }

          /* Execute the "Register Update Trigger" associated with the server instance */ 
          if (serv_obj->executeFunc)
            serv_obj->executeFunc(instance_id, resource_id, (uint8_t *)sms_msg.payload, sms_msg.payload_len, serv_obj);
        }       
      }

      active_time = lwm2m_gettime();
      lwm2m_in_sleep_state = false;
      #ifdef PSM_LWM2M
	  lwm2m_psm_operation(lwm2mH, 0, NULL, PSM_NODE_TYPE_MIN, PSM_NODE_UPDATE_ALL, PSM_MODIFY_NODE);
      #endif

      /* Release resources associated with the SMS */
      if (uri_sms)
      {
        lwm2m_free(uri_sms);
        uri_sms = NULL;
        sms_msg.uri = NULL;
      }

      /* Release the resources associated with the SMS payload 
       *     - Register Update Trigger with Param is sent the payload
       */ 
      if (sms_msg.payload_len)
      {
        sms_msg.payload_len = 0x00;
        if (sms_msg.payload)
        {
          free(sms_msg.payload); 
          sms_msg.payload = NULL;
        } 
      }
      /* Indicate OEM applications that the device is awake */
      lwm2m_send_internal_ind_to_application(QAPI_NET_LWM2M_WAKEUP_E);
    }
  }

next_step:
  if (signals & LWM2M_IN_ROAMING_IND)
  {
    if(roaming_feat_enabled)
    {     
      LOG_DEBUG("Roaming signal handling");
      lwm2m_handle_in_roaming_signal();
      return;
    }
    else
    {
      LOG_DEBUG("Roaming feature is not enabled");
      return;      
    }     
  }

  if (g_carrier_cfg) 
  { // carrier cfg if starts
#ifdef ENABLE_CONN_MONITOR
    if(enable_conn_m_obj)
      lwm2m_datacall_handle_apn_change(lwm2mH, signals);
#endif
  } // carrier cfg if ends

  if (signals & LWM2M_BATTERY_IND) 
  {
    uint8_t battery_level = 0;
    char value[32] = {'\0'};

    if (QAPI_OK != qapi_Pmapp_Vbatt_Get_Battery_Status(&battery_level))
    {
      ds_iot_ulog_err("battery_level fetch failed");
    } 

    ds_iot_ulog_high1("battery_level: %d",battery_level);
    m2m_intToString(value, battery_level);
    value_change("/3/0/9", value, strlen(value));
    qurt_signal_clear(&lwm2m_signal_Handle, LWM2M_BATTERY_IND);
  }
  
  if (signals & LOCATION_SINGLE_SHOT_CALLBACK)
  {
     ds_iot_ulog_high("process pending location read");
     lwm2m_process_pending_location_read_observe();
     qurt_signal_clear(&lwm2m_signal_Handle, LOCATION_SINGLE_SHOT_CALLBACK);
  }

  if (signals & LOCATION_FETCH)
  {
     lwm2m_location_fetch();
     qurt_signal_clear(&lwm2m_signal_Handle, LOCATION_FETCH);
  }

  if (signals & LOCATION_PMIN_UPDATED)
  {
     lwm2m_handle_location_pmin_update();
     qurt_signal_clear(&lwm2m_signal_Handle, LOCATION_PMIN_UPDATED);
  }
  
  if(signals & LWM2M_PS_DETACH)
  {
    lwm2m_object_t *cell_nw_obj = NULL;
	cell_nw_conn_instance_t* cell_nw_obj_inst = NULL;
    cell_nw_obj = (lwm2m_object_t *)LWM2M_LIST_FIND(lwm2mH->objectList , LWM2M_CELL_NW_CONN_OBJECT_ID);
	if(cell_nw_obj != NULL)
	{
	   cell_nw_obj_inst = (cell_nw_conn_instance_t *)cell_nw_obj->instanceList;
	   if(cell_nw_obj_inst != NULL)
	   {
		  /* Adding delay so that server could recieve response before PS_DETACH.*/
		  timer_sleep(100, T_MSEC, TRUE);
	      ds_iot_ulog_high1 ("Detaching for %d minute ", cell_nw_obj_inst->disable_radio_period);
          qmi_nas_set_sys_sel_pref(QAPI_DEVICE_INFO_PS_DETACH_E, 0, NULL);
	      timer_set(&cell_nw_obj_inst->ps_attach_timer , cell_nw_obj_inst->disable_radio_period , 0 , T_MIN);
	   	}
     }
	qurt_signal_clear(&lwm2m_signal_Handle, LWM2M_PS_DETACH);
  }

  if(signals & LWM2M_PS_ATTACH)
  {
    qmi_nas_set_sys_sel_pref(QAPI_DEVICE_INFO_PS_ATTACH_E, 0, NULL);
    qurt_signal_clear(&lwm2m_signal_Handle, LWM2M_PS_ATTACH);
  }

  /* Handle data call related signals */
  lwm2m_datacall_handle_signals(lwm2mH, signals);
  return;
}

/**
 * @fn check_roaming_status()
 * @brief This function is used check the roaming status
 * @param void
 * @return  void
 */ 
void check_roaming_status()
{
  // Get the roaming status       
  if(1 <= nas_get_ue_info(ROAM_STATUS, false, NULL))
  {
    ds_iot_ulog_high("Device is in Roaming State, Waiting for Out of Roaming signal");
    roaming_status = true;
    qurt_signal_wait(&lwm2m_signal_Handle, LWM2M_OUT_OF_ROAMING_IND, 
        QURT_SIGNAL_ATTR_CLEAR_MASK | QURT_SIGNAL_ATTR_WAIT_ANY);
    ds_iot_ulog_high("Device is out of Roaming State");
    roaming_status = false;
  }
  return;
}


/**
 * @fn lwm2m_handle_in_roaming_signal()
 * @brief This function is used to handle signals when device is in roaming status
 * @param void
 * @return  void
 */
void lwm2m_handle_in_roaming_signal()
{

  unsigned int return_signal = 0;

  /* Set the roaming status to valid */ 
  roaming_status = true;
  
  while (true)
  {
    return_signal = qurt_signal_wait(&lwm2m_signal_Handle, 
        LWM2M_OUT_OF_ROAMING_IND | DSS_M2M_PRIMARY_APN_NO_NET_v4 |
        DSS_M2M_SECONDARY_APN_NO_NET_v4|DSS_M2M_PRIMARY_APN_NO_NET_v6 |
        DSS_M2M_SECONDARY_APN_NO_NET_v6, 
        QURT_SIGNAL_ATTR_CLEAR_MASK | QURT_SIGNAL_ATTR_WAIT_ANY);

    if ((return_signal & DSS_M2M_PRIMARY_APN_NO_NET_v4) || 
		      (return_signal & DSS_M2M_PRIMARY_APN_NO_NET_v6))
    {
      if (iotapp_qmi_get_service_status(false) == FALSE) 
      {
        ds_iot_ulog_high("Network service is not available, wait for service up signal");
        qurt_signal_wait(&lwm2m_signal_Handle, LWM2M_NAS_UP_SIG, QURT_SIGNAL_ATTR_WAIT_ALL|QURT_SIGNAL_ATTR_CLEAR_MASK);
        ds_iot_ulog_high("Network service is available, proceeding");
      }
    } 
    else if(return_signal & LWM2M_OUT_OF_ROAMING_IND)
    {
      ds_iot_ulog_high("NAS_OUT_OF_ROAMING_IND signal ");
      roaming_status = false;
      active_time = lwm2m_gettime();
      #ifdef PSM_LWM2M
	  lwm2m_psm_operation(data.lwm2mH, 0, NULL, PSM_NODE_TYPE_MIN, PSM_NODE_UPDATE_ALL, PSM_MODIFY_NODE);
      #endif
      break;
    }
  }
  
  return;
}


int32_t lwm2m_get_object_info(lwm2m_object_info_t *lwm2m_info_req, 
    lwm2m_object_data_t ** lwm2m_dataP)
{

  lwm2m_object_t * targetP = NULL;
  lwm2m_context_t * context = NULL;
  lwm2m_id_info_t * object_info = NULL;
  lwm2m_object_data_t * lwm2m_data = NULL;
  lwm2m_resource_info_t *resource_info = NULL;
  int32_t result = 0;
  lwm2m_id_info_t id_info;

  context = data.lwm2mH;
  if(context == NULL)
  {
    LOG_DEBUG("%s context", nullError);
    return -1;
  }
  memset(&id_info, 0, sizeof(lwm2m_id_info_t));

  /* Allocated resource clean-up in case of any error is done by the caller qapi_Net_LWM2M_Get */
  lwm2m_data = (lwm2m_object_data_t *) lwm2m_malloc(sizeof(lwm2m_object_data_t));
  if(NULL == lwm2m_data)
  {
    ds_iot_ulog_err("LWM2M_LOG: Memory Allocation FAILED");
    return -1;
  }

  *lwm2m_dataP = lwm2m_data; 
  memset(lwm2m_data , 0 , sizeof (lwm2m_object_data_t));
  object_info = lwm2m_info_req->id_info;

  if (object_info != NULL)
  {
    ds_iot_ulog_high1("lwm2m get object id set enum  value %d", object_info->id_set);

    if (!(QAPI_NET_LWM2M_OBJECT_ID_E & object_info->id_set))
      return -1;

    targetP = (lwm2m_object_t *)LWM2M_LIST_FIND(context->objectList, object_info->object_ID);
    if (targetP == NULL)
    {
      ds_iot_ulog_err ("OBJECT ID NOT EXISTS");
      return -1;
    }

    ds_iot_ulog_high1("object id set , object id is %d",targetP->objID);
    lwm2m_data->object_ID = object_info->object_ID;
    id_info.id_set = QAPI_NET_LWM2M_OBJECT_ID_E;
    id_info.object_ID = object_info->object_ID;

    if (QAPI_NET_LWM2M_INSTANCE_ID_E & object_info->id_set) //If application has set the Instance ID
    {
      lwm2m_instance_info_t * instance_info = NULL;

      instance_info = (lwm2m_instance_info_t *) lwm2m_malloc(sizeof(lwm2m_instance_info_t));
      if(NULL == instance_info)
      {
        ds_iot_ulog_err("LWM2M_LOG: Memory Allocation FAILED");
        return -1;
      }

      memset (instance_info , 0x00, sizeof (lwm2m_instance_info_t));
      lwm2m_data->instance_info = instance_info;
      lwm2m_data->instance_info->instance_ID = object_info->instance_ID;
      lwm2m_data->no_instances++;

      id_info.id_set = object_info->id_set | QAPI_NET_LWM2M_INSTANCE_ID_E;
      id_info.instance_ID = object_info->instance_ID;
      

      if (QAPI_NET_LWM2M_RESOURCE_ID_E & object_info->id_set) //If application has set the Resource ID
      {
        ds_iot_ulog_high1("Resource id, Resource id is %d",object_info->resource_ID);  
        instance_info->no_resources = 1;
        id_info.id_set = object_info->id_set | QAPI_NET_LWM2M_RESOURCE_ID_E;
        id_info.resource_ID = object_info->resource_ID;
#if 0
        resource_info = (lwm2m_resource_info_t *) lwm2m_malloc(sizeof(lwm2m_resource_info_t));
        if (resource_info == NULL)
        {
          LOG_ERROR ("%s",mallocError);
          return -1;
        }

        memset(resource_info , 0x00, sizeof (lwm2m_resource_info_t));
        instance_info->resource_info = resource_info;
        instance_info->resource_info->resource_ID = object_info->resource_ID;

        if (!targetP->get_Value_Func)
        {
          LOG_ERROR("Function to get the value is not registered");
          return -1;
        }

        result =  targetP->get_Value_Func(id_info, lwm2m_data, targetP);
        if (0 != result)
          return -1;
#endif
      }
        }

      if (!targetP->get_Value_Func)
      {
        ds_iot_ulog_err("Function to get the value is not registered");
        return -1;
      }

      result = targetP->get_Value_Func(id_info, &lwm2m_data, targetP, &resource_info);
      if (0 != result)
        return -1;
    }

  return 0;
}

#ifdef TX_DATA_UNUSED_CODE

void lwm2m_free_id_info (lwm2m_id_info_t *lwm2m_id_infoP)
{
  if (lwm2m_id_infoP != NULL)
    lwm2m_free (lwm2m_id_infoP);
}

void lwm2m_free_lwm2m_data(lwm2m_object_data_t *lwm2m_dataP)
{
  if (lwm2m_dataP != NULL)
  {
    lwm2m_free_instance_info(lwm2m_dataP->instance_info);
    lwm2m_free(lwm2m_dataP);
  }
}

#endif

void lwm2m_free_instance_info (lwm2m_instance_info_t  *lwm2m_instanceP)
{
  if (lwm2m_instanceP != NULL)
  {
    lwm2m_free_resource_info(lwm2m_instanceP->resource_info);
    lwm2m_free (lwm2m_instanceP);
  }
}

void lwm2m_free_resource_info (lwm2m_resource_info_t *lwm2m_resourceP)
{
  lwm2m_resource_info_t *lwm2m_resource_linkP = NULL;
  
  while (lwm2m_resourceP != NULL)
  {
    lwm2m_resource_linkP = lwm2m_resourceP->next;

    if (lwm2m_resourceP->value.asBuffer.buffer)
      lwm2m_free(lwm2m_resourceP->value.asBuffer.buffer);
   
    lwm2m_free(lwm2m_resourceP);
    lwm2m_resourceP = lwm2m_resource_linkP;
  }
}


uint8_t lwm2m_datacall_bring_up (
  carrier_apn_cfg_t *apn_info, 
  iot_dss_regparams_t *dss_params, 
  void **callHdl_v4_nip, 
  void **callHdl_v6, 
  bool start_call, 
  uint16_t short_server_id) 
{
  uint32 signal_attr = QURT_SIGNAL_ATTR_WAIT_ANY; 
  uint32 signal = 0x00;
  bool primary_ipv6_up = false; 

  if ((NULL == dss_params) || 
      (NULL == callHdl_v4_nip) || 
      (NULL == callHdl_v6))
  {
    LOG_DEBUG("Invalid datacall bring-up input params!");
    return 0;
  }  

  if ((AF_INET6 == dss_params->ip_family) || 
      (AF_INET_DUAL46 == dss_params->ip_family))
  { 
    uint8_t ip_family = dss_params->ip_family;
    uint32_t signal_mask = 0x00;

    ds_iot_log_strf_high("Start datacall for APN (%s) and family (%d), associated with SSID[%d]", dss_params->apn, ip_family, short_server_id);

    /* Dual-IP case we would first bring IPv6 followed by IPv4 */ 
    dss_params->ip_family = AF_INET6;

    if (LWM2M_APN_CLASS2 == dss_params->apn_class)
    {
      dss_params->dss_status_cb = prv_primary_apn_datacall_callback_v6;
          
      /* Add the signals to wait on while the IPv6 data call is being brought-up. Since the APN belongs to Class 2 ensure 
       * that the call is brought-up before we proceed further. 
       */
      signal_mask |= DSS_M2M_PRIMARY_APN_CONN_v6 | DSS_M2M_PRIMARY_APN_NO_NET_v6 |
                     DSS_M2M_APN_MAX_RETRIES | LWM2M_PS_ATTACH |
                     LWM2M_FACTORY_RESET_SET | LWM2M_RESTART;
    }
    else 
    {
      dss_params->dss_status_cb = prv_secondary_apns_datacall_callback_v6;

      /* Add the signals to wait on while the IPv4 data call is being brought-up */
      signal_mask |= DSS_M2M_SECONDARY_APN_CONN_v6 | DSS_M2M_SECONDARY_APN_NO_NET_v6;           
    }

    /* Register with the data call management module */ 
    if (NULL == (*callHdl_v6 = iot_dss_register(dss_params)))
    {
      signal_mask = 0x00;
      ds_iot_ulog_err1("IPv6 DSS register error for SSID[%d]", short_server_id);
    } 
    else 
    {
      /* Notify the "registration" event to the associated APN connection instance */
      lwm2m_apn_obj_event_notification(get_data_call_hndl(*callHdl_v6), short_server_id, LWM2M_DATA_CALL_REGISTERED, dss_params->ip_family); 

      /* Start the data call */ 
      if (start_call) 
      {
retry_primary_v6_apn:
        /* Notify the "start" event to the associated APN connection instance */
        lwm2m_apn_obj_event_notification(get_data_call_hndl(*callHdl_v6), short_server_id, LWM2M_DATA_CALL_START, dss_params->ip_family);
 
        iot_dss_start_datacall(*callHdl_v6);
       
        /* Wait for APN class-2 data calls until the calls are brought-up successfully or max re-tries are reached. In case of 
         * APN class-0 and APN class-3 data calls, exit out of wait for any event from the data call management module. In both 
         * these cases, the data call management retry logic would perform the re-tries.
         */ 
        signal = qurt_signal_wait(&lwm2m_signal_Handle, signal_mask, signal_attr);

        if ((signal & LWM2M_FACTORY_RESET_SET) || (signal & LWM2M_RESTART))
        {
          ds_iot_ulog_high("Got factor reset or restart request");
          goto factory_reset_or_restart;
        }
        /* PS Attach */ 
        if (signal & LWM2M_PS_ATTACH)
        {
           qmi_nas_set_sys_sel_pref(QAPI_DEVICE_INFO_PS_ATTACH_E, 0, NULL);
           qurt_signal_clear(&lwm2m_signal_Handle, LWM2M_PS_ATTACH);
        }

        /* Primary APN */ 
        if (LWM2M_APN_CLASS2 == dss_params->apn_class)
        {
          /* Primary Class-2 APN connected */
          if (signal & DSS_M2M_PRIMARY_APN_CONN_v6)
          {
            ds_iot_ulog_high1("Primary IPv6 (Class-2) APN connected for SSID[%d]", short_server_id);
  
            /* Clear the generated "PRIMARY V6 CONN" signal */          
            qurt_signal_clear(&lwm2m_signal_Handle, DSS_M2M_PRIMARY_APN_CONN_v6);
  
            /* Reset the data call state */
            apn_info->v6_call_state = 0x00; 

            /* Set the flag indicating the Primary Class-2 APN is connected */
            primary_ipv6_up = true; 
  
            /* Notify the "CONNECTED" event to the associated APN connection instance */
            lwm2m_apn_obj_event_notification(get_data_call_hndl(*callHdl_v6), short_server_id, LWM2M_DATA_CALL_CONNECTED, dss_params->ip_family);
          } 
          /* Maximum re-tries reached Primary Class-2 APN */ 
          else if ((signal & DSS_M2M_APN_MAX_RETRIES))
          { 
            /* Clear the generated "MAX RE-TRIES" signal */
            qurt_signal_clear(&lwm2m_signal_Handle, DSS_M2M_APN_MAX_RETRIES);

            /* Shutdown LwM2M if the Primary Class-2 APN is not Dual-IP. In case of a Dual-IP scenario, try bringing the associated 
             * IPv4 data call without terminating.
             */
            if (AF_INET_DUAL46 != ip_family)
            {
              ds_iot_ulog_err1("Shutting down LwM2M client as maximum re-tries reached in bringing-up Primary IPv6 (Class-2) APN for SSID[%d]", short_server_id);          
              return 1;
            }
  
            /* Update the IPv6 call state indicating maximum retries is reached */
            apn_info->v6_call_state = DSS_M2M_APN_MAX_RETRIES;
            ds_iot_ulog_err1("Maximum re-tries reached in bringing-up Dual-IP Primary IPv6 (Class-2) APN... trying IPv4 for SSID[%d]", short_server_id);
          }
          /* Bringing-up the Primary Class-2 APN failed. IOT DSS thread would post the "NO NET" event for the primary APN, but the 
           * event is handled in the LwM2M main thread which is blocked here and would never get processed for primary APN. Handle 
           * the event at this point and re-try. 
           */
          else if (signal & DSS_M2M_PRIMARY_APN_NO_NET_v6)
          {
            ds_iot_ulog_err1("Primary IPv6 (class-2) APN failed to get connected - retry the data call bring-up for SSID[%d]", short_server_id);
  
            /* Clear the generated "PRIMARY V6 NO NET" signal */
            qurt_signal_clear(&lwm2m_signal_Handle, DSS_M2M_PRIMARY_APN_NO_NET_v6);
  
            /* Reset the data call state */
            apn_info->v6_call_state = 0x00; 
   
            /* Notify the "failure" event to the associated APN connection instance */
            lwm2m_apn_obj_event_notification(get_data_call_hndl(*callHdl_v6), short_server_id, LWM2M_DATA_CALL_FAILURE, dss_params->ip_family);                
            goto retry_primary_v6_apn; 
          }
        }
      }
    }

    /* Restore the IP family (in dual-ip scenario we would bring-up IPv6 followed by IPv4) */
    dss_params->ip_family = ip_family;
  }

  if ((AF_INET == dss_params->ip_family)  || 
      (AF_NONIP == dss_params->ip_family) ||
      (AF_INET_DUAL46 == dss_params->ip_family))
  {
    uint8_t ip_family = dss_params->ip_family;
    uint32_t signal_mask = 0x00;

    ds_iot_log_strf_high("Start datacall for APN (%s) and family (%d), associated with SSID[%d]", dss_params->apn, ip_family, short_server_id);

    /* Dual-IP case we would first bring IPv6 followed by IPv4 */    
    if (AF_NONIP != dss_params->ip_family)
      dss_params->ip_family = AF_INET;      
    
    if (LWM2M_APN_CLASS2 == dss_params->apn_class)
    {
      dss_params->dss_status_cb = prv_primary_apn_datacall_callback_v4;
          
      /* Add the signals to wait on while the IPv4 data call is being brought-up. Since the APN belongs to Class 2 ensure 
       * that the call is brought-up before we proceed further. 
       */
      signal_mask |= DSS_M2M_PRIMARY_APN_CONN_v4 | DSS_M2M_PRIMARY_APN_NO_NET_v4 |
                     DSS_M2M_APN_MAX_RETRIES | LWM2M_PS_ATTACH |
                     LWM2M_FACTORY_RESET_SET | LWM2M_RESTART;
    }
    else 
    {
      dss_params->dss_status_cb = prv_secondary_apns_datacall_callback_v4;
      /* Add the signals to wait on while the IPv4 data call is being brought-up */
      signal_mask |= DSS_M2M_SECONDARY_APN_CONN_v4 | DSS_M2M_SECONDARY_APN_NO_NET_v4;           
    }

    /* Register with the data call management module */ 
    if (NULL == (*callHdl_v4_nip = (void *)iot_dss_register(dss_params)))
    {
      ds_iot_ulog_err1("IPv4 DSS register error for SSID[%d]", short_server_id);
    } 
    else 
    {
      /* Notify the "registration" event to the associated APN connection instance */
      lwm2m_apn_obj_event_notification(get_data_call_hndl(*callHdl_v4_nip), short_server_id, LWM2M_DATA_CALL_REGISTERED, dss_params->ip_family); 

      /* Start the data call */  
      if (start_call)
      {
retry_primary_v4_apn:
        /* Notify the "start" event to the associated APN connection instance */
        lwm2m_apn_obj_event_notification(get_data_call_hndl(*callHdl_v4_nip), short_server_id, LWM2M_DATA_CALL_START, dss_params->ip_family);  

        iot_dss_start_datacall(*callHdl_v4_nip);
   
        /* Wait for APN class-2 data calls until the calls are brought-up successfully or max re-tries are reached. In case of 
         * APN class-0 and APN class-3 data calls, exit out of wait for any event from the data call management module. In both 
         * these cases, the data call management retry logic would perform the re-tries.
         */ 
        signal = qurt_signal_wait(&lwm2m_signal_Handle, signal_mask, signal_attr);
        if ((signal & LWM2M_FACTORY_RESET_SET) || (signal & LWM2M_RESTART))
        {
          ds_iot_ulog_high("Got factor reset or restart request");
          goto factory_reset_or_restart;
        }

        /* PS Attach */ 
        if (signal & LWM2M_PS_ATTACH)
        {
          qmi_nas_set_sys_sel_pref(QAPI_DEVICE_INFO_PS_ATTACH_E, 0, NULL);
          qurt_signal_clear(&lwm2m_signal_Handle, LWM2M_PS_ATTACH);
        }

        /* Primary APN */ 
        if (LWM2M_APN_CLASS2 == dss_params->apn_class)
        {
          /* Primary Class-2 APN connected */
          if (signal & DSS_M2M_PRIMARY_APN_CONN_v4)
          {
            ds_iot_ulog_high2("Primary IPv4/NON_IP (%d) APN connected for SSID[%d]", dss_params->ip_family, short_server_id);
  
            /* Clear the generated "PRIMARY V4 CONN" signal */
            qurt_signal_clear(&lwm2m_signal_Handle, DSS_M2M_PRIMARY_APN_CONN_v4);

            /* Reset the data call state */
            apn_info->v4_nip_call_state = 0x00; 
        
            /* Notify the "CONNECTED" event to the associated APN connection instance */
            lwm2m_apn_obj_event_notification(get_data_call_hndl(*callHdl_v4_nip), short_server_id, LWM2M_DATA_CALL_CONNECTED, dss_params->ip_family);                 
          } 
          /* Maximum re-tries reached Primary Class-2 APN */  
          else if (signal & DSS_M2M_APN_MAX_RETRIES)
          {
            /* Clear the generated "MAX RE-TRIES" signal */
            qurt_signal_clear(&lwm2m_signal_Handle, DSS_M2M_APN_MAX_RETRIES);

            /* Shutdown LwM2M if the Primary Class-2 APN is not Dual-IP. In case of a Priamry Dual-IP scenario, check to see if 
             * IPv6 data call is successful or not. If the IPv6 data call is not successful, terminate LwM2M.  
             */
            if ((AF_INET_DUAL46 != ip_family) || 
                ((AF_INET_DUAL46 == ip_family) && 
                 (false == primary_ipv6_up)))
            {
              ds_iot_ulog_err2("Shutting LwM2M client as maximum re-tries reached in bringing-up Primary IPv4/NON_IP (%d) APN for SSID[%d]", dss_params->ip_family, 
                                                                                                                                         short_server_id);
              return 1;
            } 
  
            /* Update the IPv4 call state indicating maximum retries is reached */
            apn_info->v4_nip_call_state = DSS_M2M_APN_MAX_RETRIES;
            ds_iot_ulog_err1("Maximum re-tries reached in bringing-up Dual Primary IPv4 APN... Primary IPv6 APN successful for SSID[%d]", short_server_id);
          }
          /* Bringing-up the Primary Class-2 APN failed. IOT DSS thread would post the "NO NET" event for the primary APN, but 
           * the event is handled in the LwM2M main thread which is blocked here and would never get processed for primary APN. 
           * Handle the event at this point and re-try. 
           */
          else if (signal & DSS_M2M_PRIMARY_APN_NO_NET_v4)
          {
            ds_iot_ulog_err2("Primary IPv4/NON_IP [%d] APN failed to get connected - retry the bring-up for SSID[%d]", dss_params->ip_family, short_server_id);
  
            /* Clear the generated "PRIMARY V4 NO NET" signal */
            qurt_signal_clear(&lwm2m_signal_Handle, DSS_M2M_PRIMARY_APN_NO_NET_v4);

            /* Reset the data call state */
            apn_info->v4_nip_call_state = 0x00; 
   
            /* Notify the "failure" event to the associated APN connection instance */
            lwm2m_apn_obj_event_notification(get_data_call_hndl(*callHdl_v4_nip), short_server_id, LWM2M_DATA_CALL_FAILURE, dss_params->ip_family);               
            goto retry_primary_v4_apn; 
          }
        }
      }      
    }
  }

  return 0;

factory_reset_or_restart:
  if(signal & LWM2M_RESTART) {
    ds_iot_ulog_high("lwm2m processing Restart signal");
    g_restart = 1;
    qurt_signal_clear(&lwm2m_signal_Handle, LWM2M_RESTART);
  }
  if(signal & LWM2M_FACTORY_RESET_SET) 
  {
    ds_iot_ulog_high("lwm2m processing factory reset signal");
    factory_reset_req = true;
    /*deleting old certificate files from secure location*/
    lwm2m_delete_server_security_credentials(data.lwm2mH);
    prv_deregister(data.lwm2mH);
    reset_global_flags(LWM2M_FALSE);
    qurt_signal_clear(&lwm2m_signal_Handle, LWM2M_FACTORY_RESET_SET);
  }

  return 1;
}


int8_t lwm2m_datacall_start (
  lwm2m_context_t *lwm2mH, 
  lwm2m_data_call_op_t operation_type, 
  uint16_t *short_serverID, 
  bool start_call)
{
  int8_t result = -1; 
  iot_dss_regparams_t dss_params;
  carrier_apn_cfg_t *apn_info = NULL;
  uint32 signal;

  if (lwm2mH == NULL) 
  {
    LOG_DEBUG("%s context",nullError);
    return -1;
  }

  qapi_Device_Info_Set_Callback_v2(lwm2m_dev_info_hdl, QAPI_DEVICE_INFO_ANY, lwm2m_nas_status);

  /* Wait for service prior to bringing up the data calls */
  if (iotapp_qmi_get_service_status(false) == FALSE) 
  {
    ds_iot_ulog_high("Network service is not available, wait for service up signal");

    /* wait factory-reset/restart signal whenever since QAPI can trigger these events anytime */
    signal = qurt_signal_wait(&lwm2m_signal_Handle, LWM2M_NAS_UP_SIG|LWM2M_RESTART|LWM2M_FACTORY_RESET_SET|LWM2M_PS_ATTACH, QURT_SIGNAL_ATTR_WAIT_ANY|QURT_SIGNAL_ATTR_CLEAR_MASK);
   	if ((signal & LWM2M_FACTORY_RESET_SET) || (signal & LWM2M_RESTART))
    {
      ds_iot_ulog_high("Got restart/request, exit start call");

      /* Re-send the signal to handle in next process signals */
      qurt_signal_clear(&lwm2m_signal_Handle, signal);	  
      qurt_signal_set(&lwm2m_signal_Handle, signal);

      return -1;
    }
    if (signal & LWM2M_PS_ATTACH)
    {
      qmi_nas_set_sys_sel_pref(QAPI_DEVICE_INFO_PS_ATTACH_E, 0, NULL);
      qurt_signal_clear(&lwm2m_signal_Handle, LWM2M_PS_ATTACH);
    }

    ds_iot_ulog_high("Network service is available, starting data calls");
  } 
  else 
  {
    ds_iot_ulog_high("Network service is available, starting data calls");
  }

  /* Initialize the data call management module */ 
  iot_dss_init();

  /* Update the APN connection instance information associated with the servers - only registration servers */ 
  apn_info = update_apn_conn_cfg(lwm2mH, short_serverID);
  if (NULL == apn_info) 
  {
    result = -1;
    ds_iot_ulog_high("Updating APN connection configuration failed");

    goto start_exit;
  } 
  else
  {
    /* Update APN list from here */
	if(enable_conn_m_obj)
    update_carrier_apn_info_to_conn_m_obj(lwm2mH);
  }
 
  while (apn_info)
  {
    uint32_t cntr = 0x00;
    uint32_t active_calls = 0x00;
    apn_ref_cnt_t *call_sharing = NULL;  

    if (((LWM2M_START_BOOTSTRAP_DATA_CALL == operation_type) && 
         (apn_info->bootstrap_apn)) ||
        ((LWM2M_START_REGISTRATION_DATA_CALL == operation_type) && 
         (apn_info->registration_apn)))      
    {
      /* Short server ID match */
      if ((short_serverID) && (*short_serverID != apn_info->short_server_id))
      {
        apn_info = apn_info->next;
        continue;
      }      

      memset(&dss_params, 0x00, sizeof(iot_dss_regparams_t));
  
      /* Active data calls and data call sharing sceanrios */
      for (cntr = 0; cntr < MAX_DATA_CALLS_SUPPORTED; cntr++)
      {
        if (lwm2mH->call_cntr[cntr].cntr)
        {
          /* Active data calls counter */
          active_calls += 0x01;
  
          /* Data call sharing - Active data call can share the calls with others  */
          if (!strncmp(lwm2mH->call_cntr[cntr].apn_name, apn_info->apn_name, CARRIER_APN_LEN))
          {
            call_sharing = &lwm2mH->call_cntr[cntr];
            lwm2mH->call_cntr[cntr].cntr += 0x01;
          }
        }      
      }
      
      /* Check the mode of operation - M1 or NB1 
       * NB-1 - Upto two data calls are supported (APNs can support call sharing)
       *  M-1 - Upto four data calls are supported (APNs can support call sharing) 
       */
      if (((IOTAPP_CIOT_MODE_NB1 == srvc_type.ciot_mode) &&
            (active_calls >= MAX_DATA_CALLS_SUPPORTED_NB_1) &&
            (NULL == call_sharing)) ||  
          ((IOTAPP_CIOT_MODE_NB1 != srvc_type.ciot_mode) &&
            (active_calls >= MAX_DATA_CALLS_SUPPORTED_M_1) &&
            (NULL == call_sharing)))
      {
        result = 0;
        goto start_exit;
      }
  
      dss_params.user_data             = (void *)apn_info->short_server_id; /* Just need the short server ID */            
      dss_params.apn_class             = apn_info->apn_class;
      dss_params.apn                   = apn_info->apn_name;
      dss_params.user_name             = apn_info->user_name;
      dss_params.password              = apn_info->password;
      dss_params.ip_family             = apn_info->transport_binding;
      dss_params.auth_type             = (qapi_DSS_Auth_Pref_t)apn_info->auth_type;
      dss_params.retry_timeout_lst     = apn_info->retry_timeout_lst;
      dss_params.retry_timeout_lst_len = apn_info->retry_timeout_lst_len;
      dss_params.skip_retry_ce_reason  = data.cfg_info.skip_call_retry_ce_reason;
      
      /* Bring-up the data call */
      apn_info->callHdl_v4_nip = NULL; 
      apn_info->callHdl_v6     = NULL; 
      if (lwm2m_datacall_bring_up(apn_info, &dss_params, &apn_info->callHdl_v4_nip, &apn_info->callHdl_v6, start_call, apn_info->short_server_id))
      {
        /* Only error that is posted in the call bring-up is when the maximum re-tries are reached for primary APN. In this case, stop all the 
         * existing data calls that were started so far and shutdown LWM2M.
         */   
        lwm2m_datacall_stop(lwm2mH, operation_type, NULL, false);
        return -1; 
      }
        
      /* Update the information of the newly brought-up data call */
      if (NULL == call_sharing)
      {
        for (cntr = 0; cntr < MAX_DATA_CALLS_SUPPORTED; cntr++)
        {
          if (0x00 == lwm2mH->call_cntr[cntr].cntr)
          {
            strlcpy(lwm2mH->call_cntr[cntr].apn_name, apn_info->apn_name, CARRIER_APN_LEN);
            lwm2mH->call_cntr[cntr].cntr += 0x01;
     
            break; 
          }
        }  
      }
    }
    
    apn_info = apn_info->next;
  }

  result = 0; 

start_exit:
  return result;
}


void lwm2m_datacall_stop(lwm2m_context_t *lwm2mH, lwm2m_data_call_op_t operation_type, uint16_t *short_serverID, bool validate_apn)
{
  carrier_apn_cfg_t *apn_info = NULL;

  if (lwm2mH == NULL)
  {
    LOG_DEBUG("%s context",nullError);
    return;
  }

  /* Retrieve the stored APN information pertaining to the data calls */
  apn_info = lwm2mH->apn_cfg;
  while (apn_info)
  {
    uint32_t cntr = 0x00; 

    if (((LWM2M_STOP_BOOTSTRAP_DATA_CALL == operation_type) && 
         (apn_info->bootstrap_apn)) ||
        ((LWM2M_STOP_REGISTRATION_DATA_CALL == operation_type) && 
         (apn_info->registration_apn)))                 
    {

      /* Short server ID match */
      if ((short_serverID) && (*short_serverID != apn_info->short_server_id))
      {
        apn_info = apn_info->next;
        continue;
      }  

      /* Decrement the data call counter, as the call is torn-down */
      for (cntr = 0; cntr < MAX_DATA_CALLS_SUPPORTED_M_1; cntr++)
      {
        if (lwm2mH->call_cntr[cntr].cntr)
        {
          if (!strncmp(lwm2mH->call_cntr[cntr].apn_name, apn_info->apn_name, CARRIER_APN_LEN))
          {
            memset(lwm2mH->call_cntr[cntr].apn_name, 0x00, CARRIER_APN_LEN); 
            lwm2mH->call_cntr[cntr].cntr -= 0x01;
            
            break;
          }
        }
      }

      /* Bootstrapping is successful. Re-use the existing datacall without tearing down if the same APN used in bootstrapping is also 
       * used by the device for registering with the registration server(s).  
       *
       * Exceptions: 
       *    1. Only applicable when transitioning from Bootstrapping -> Registration phases for better performance. 
       *    2. NON_IP data call sharing is currently not supported in the data call management module.  
       */ 
      if ((validate_apn) &&
          (apn_info->bootstrap_apn) &&
          (apn_info->transport_binding != AF_NONIP))
      {
        ds_iot_log_strf_high("apn_name: [%s], ip_family: [%d], transport_binding: [%d]",
                apn_info->apn_name, apn_info->ip_family, apn_info->transport_binding);
        if (true == bootstrap_server_apn_info_match(lwm2mH, apn_info))
        {
          ds_iot_log_strf_high("Match found for bootstrap APN[%s] with the registration server(s) - don't stop the data call", apn_info->apn_name);

          apn_info = apn_info->next;
          continue; 
        }

        ds_iot_log_strf_high("No match found for bootstrap APN[%s] with any registration server(s) - stopping the data call", apn_info->apn_name); 
      }          
      
      if ((apn_info->ip_family         == AF_INET)        ||
          (apn_info->ip_family         == AF_INET_DUAL46) ||
          (apn_info->transport_binding == AF_NONIP))
      {
        void *call_hndl = apn_info->callHdl_v4_nip; 
        uint8_t family = (AF_NONIP == apn_info->transport_binding) ? AF_NONIP : AF_INET; /* IPv4 and Non_IP re-use the same handle */

        ds_iot_log_strf_high("Stop data call for APN[%s], ip_family[%d], handle[%u]", apn_info->apn_name, family, call_hndl);

        /* Notify the "stop" event to the associated APN connection instance */
        lwm2m_apn_obj_event_notification(get_data_call_hndl(call_hndl), apn_info->short_server_id, LWM2M_DATA_CALL_STOP, family);          
        iot_dss_stop_datacall(call_hndl);

        timer_sleep(2, T_SEC, 1);

        /* Notify the "de-register" event to the associated APN connection instance */
        lwm2m_apn_obj_event_notification(get_data_call_hndl(call_hndl), apn_info->short_server_id, LWM2M_DATA_CALL_DEREGISTERED, family);  
        iot_dss_deregister(call_hndl);        
      }
      
      if ((apn_info->ip_family == AF_INET6) || 
          (apn_info->ip_family == AF_INET_DUAL46))
      {
        void *call_hndl = apn_info->callHdl_v6;

        ds_iot_log_strf_high("Stop data call for APN[%s], ip_family[%d], handle[%u]", apn_info->apn_name, apn_info->ip_family, apn_info->callHdl_v6);
  
        /* Notify the "stop" event to the associated APN connection instance */
        lwm2m_apn_obj_event_notification(get_data_call_hndl(call_hndl), apn_info->short_server_id, LWM2M_DATA_CALL_STOP, AF_INET6);  
        iot_dss_stop_datacall(call_hndl);

        timer_sleep(2, T_SEC, 1);

        /* Notify the "de-register" event to the associated APN connection instance */
        lwm2m_apn_obj_event_notification(get_data_call_hndl(call_hndl), apn_info->short_server_id, LWM2M_DATA_CALL_DEREGISTERED, AF_INET6); 
        iot_dss_deregister(call_hndl);        
      }
    }

    apn_info = apn_info->next;
  }

  return;  
}

#ifdef ENABLE_CONN_MONITOR
void lwm2m_datacall_handle_apn_change(lwm2m_context_t *lwm2mH, uint32 signals)
{
  carrier_apn_cfg_t *apnInfo = NULL;
  iot_dss_regparams_t dcParams;

  if(lwm2mH == NULL)
    return;

  apnInfo = lwm2mH->apn_cfg;
  memset(&dcParams, 0x00, sizeof(iot_dss_regparams_t));

  if(signals & LWM2M_CONN_MONI_SCND_APN_CHANGE)
  {
    LOG_DEBUG("LWM2M_CONN_MONI_SCND_APN_CHANGE %s", class3_apn);
    while(apnInfo)
    {
      if(LWM2M_APN_CLASS3 == apnInfo->apn_class)
      {
        if (AF_NONIP == apnInfo->transport_binding || AF_INET == apnInfo->transport_binding)
        {
          iot_dss_stop_datacall(apnInfo->callHdl_v4_nip);
          iot_dss_deregister(apnInfo->callHdl_v4_nip);
        }
        /* IPv6 */ 
        else if (AF_INET6 == apnInfo->transport_binding)
        {
          iot_dss_stop_datacall(apnInfo->callHdl_v6);
          iot_dss_deregister(apnInfo->callHdl_v6);
        }
        /* Dual-IP */ 
        else if (AF_INET_DUAL46 == apnInfo->transport_binding)
        {
          iot_dss_stop_datacall(apnInfo->callHdl_v4_nip);
          iot_dss_deregister(apnInfo->callHdl_v4_nip);
          iot_dss_stop_datacall(apnInfo->callHdl_v6);
          iot_dss_deregister(apnInfo->callHdl_v6);
        }
        LWM2M_STRCPY(apnInfo->apn_name, class3_apn, CARRIER_APN_LEN);

        iotapp_qmi_wds_find_and_update_profile(class3_apn,class3_apn_prev);

        dcParams.user_data             = (void *)apnInfo->short_server_id; /* Just need the short server ID */            
        dcParams.apn_class             = apnInfo->apn_class;
        dcParams.apn                   = apnInfo->apn_name;
        dcParams.user_name             = apnInfo->user_name;
        dcParams.password              = apnInfo->password;
        dcParams.auth_type             = (qapi_DSS_Auth_Pref_t)apnInfo->auth_type;
        dcParams.retry_timeout_lst     = apnInfo->retry_timeout_lst;
        dcParams.retry_timeout_lst_len = apnInfo->retry_timeout_lst_len;
        dcParams.skip_retry_ce_reason  = data.cfg_info.skip_call_retry_ce_reason;

        if (AF_NONIP == apnInfo->transport_binding || AF_INET == apnInfo->transport_binding)
        {
          dcParams.ip_family             = apnInfo->transport_binding;
          dcParams.dss_status_cb = prv_secondary_apns_datacall_callback_v4;

          if(NULL == (apnInfo->callHdl_v4_nip = iot_dss_register(&dcParams)))
          {
            LOG_DEBUG("DSS Registration failed");
            return;
          } 
          else 
          {
            iot_dss_start_datacall(apnInfo->callHdl_v4_nip);
          }
        }
        /* IPv6 */ 
        else if (AF_INET6 == apnInfo->transport_binding)
        {
          dcParams.ip_family             = apnInfo->transport_binding;
          dcParams.dss_status_cb         = prv_secondary_apns_datacall_callback_v6;

          if(NULL == (apnInfo->callHdl_v6 = iot_dss_register(&dcParams)))
          {
            LOG_DEBUG("DSS Registration failed");
            return;
          } 
          else 
          {
            iot_dss_start_datacall(apnInfo->callHdl_v6);
          }
        }
        /* Dual-IP */ 
        else if (AF_INET_DUAL46 == apnInfo->transport_binding)
        {
          dcParams.ip_family             = AF_INET6;
          dcParams.dss_status_cb         = prv_secondary_apns_datacall_callback_v6;

          if(NULL == (apnInfo->callHdl_v6 = iot_dss_register(&dcParams)))
          {
            LOG_DEBUG("DSS Registration failed");
            return;
          } 
          else 
          {
            iot_dss_start_datacall(apnInfo->callHdl_v6);
          }

          dcParams.ip_family             = AF_INET;
          dcParams.dss_status_cb         = prv_secondary_apns_datacall_callback_v4;

          if(NULL == (apnInfo->callHdl_v4_nip = iot_dss_register(&dcParams)))
          {
            LOG_DEBUG("DSS Registration failed");
            return;
          } 
          else 
          {
            iot_dss_start_datacall(apnInfo->callHdl_v4_nip);
          }
        }
      }
      apnInfo = apnInfo->next;
    }
  }
  if(signals & LWM2M_CONN_MONI_CLASS6_APN_CHANGE)
  {
    LOG_DEBUG("LWM2M_CONN_MONI_CLASS6_APN_CHANGE %s ", class6_apn);
    iotapp_qmi_wds_find_and_update_profile(class6_apn,class6_apn_prev);
  }
  if(signals & LWM2M_CONN_MONI_CLASS7_APN_CHANGE)
  {
    LOG_DEBUG("LWM2M_CONN_MONI_CLASS6_APN_CHANGE %s ", class7_apn);
    iotapp_qmi_wds_find_and_update_profile(class7_apn,class7_apn_prev);
  }
}
#endif

void lwm2m_process_profile_change(lwm2m_context_t *contextP, lwm2m_server_t *targetP)
{
  bool reconnect_success = false; 
  char ServerIp[PRV_ADDRSTRLEN] = {0};
  security_instance_t *instance = NULL;
  client_data_t *dataP = NULL;  
  server_instance_t *server_inst = NULL;
  lwm2m_object_t *server_list = NULL;  
  uint32_t mtu_size = 0;
  bool mtu_changed = false;

  if (targetP == NULL)
    return;

  if (targetP->coapHandle != NULL)
  {
    carrier_apn_cfg_t *apn_info = NULL; 

    if (get_ip_for_carrier_apn_cfg(contextP->apn_cfg, targetP, ServerIp) == LWM2M_400_BAD_REQUEST)
      return;
    if(get_mtu_for_carrier_apn_cfg(contextP->apn_cfg, targetP, &mtu_size) != LWM2M_400_BAD_REQUEST)
    {
      if(mtu_size && mtu_size != targetP->mtu_size)
        mtu_changed = true;
    }

    /* Retrieve APN information associated with the server short ID */
    apn_info = (carrier_apn_cfg_t *)LWM2M_LIST_FIND(contextP->apn_cfg, targetP->shortID);
    if (NULL == apn_info)
    {
      ds_iot_ulog_err1("APN info not found, failure to re-connect short ID [%d] with the server", targetP->shortID);
      return;
    }
       
    /* Retrieve the server instance related inforamtion */
    server_list = (lwm2m_object_t *)LWM2M_LIST_FIND(contextP->objectList, LWM2M_SERVER_OBJECT_ID);
    if (!server_list)
    {
      ds_iot_ulog_err1("Server instance list not found, failure to re-connect short ID [%d] with the server", targetP->shortID);
      return;
    }
      
    /* Traverse the server instance list and populate the APN information required */
    server_inst = (server_instance_t *)server_list->instanceList;
    while (server_inst)
    {
      if (targetP->shortID == server_inst->shortServerId)
        break;
     
      server_inst = server_inst->next;
    }

    if (NULL == server_inst)
    {
      ds_iot_ulog_err1("Server instance not found resulting in failure to re-connect short ID [%d] with the server", targetP->shortID);
      return;
    }
  
    /* IP transport */   
    if (AF_NONIP != targetP->local_ip.sa_family)
    { 
      struct sockaddr current_ip;
  
      memset(&current_ip, 0x00, sizeof(struct sockaddr));

      /* IPv4 family */ 
      if (AF_INET == targetP->local_ip.sa_family)
      {
        struct sockaddr_in *sa = (struct sockaddr_in *)&current_ip;
        inet_pton(AF_INET, ServerIp, &sa->sin_addr);
      }
      /* IPv6 family */ 
      else if (AF_INET6 == targetP->local_ip.sa_family)
      {
        struct sockaddr_in6 *sa6 = (struct sockaddr_in6 *)&current_ip;
        inet_pton(AF_INET6, ServerIp, &sa6->sin_addr);
      }
        
      /*
       * IP Transport
       *
       * Re-connect the CoAP connection with the server 
       *     1. Re-connect flag is set when transport binding changes are detected during data call failure or register update trigger. 
       *     2. Any IP address changes are detected. 
       *
       */
      if ((apn_info->reconnect) ||
          (memcmp(current_ip.sa_data, targetP->local_ip.sa_data, sizeof(targetP->local_ip.sa_data))))
      {  
        /* Close the existing CoAP connection */
        qapi_Coap_Close_Connection(targetP->coapHandle);

        /* Create a new connection with the changed transport binding */ 
        targetP->sessionH = lwm2m_connect_server(targetP, contextP);
        if (0x00 == targetP->sessionH) 
        {
          ds_iot_ulog_err1("CoAP re-connect failed for SSID[%d]", targetP->shortID);
          goto reconnect_err;   
        }

        /* Reset the re-connect flag as the re-connection is successful */
        apn_info->reconnect = false;        
        /* Set the CoAP re-connection to be successful */
        reconnect_success = true;     
    
        ds_iot_ulog_high1("Profile/Address change for SSID[%d], re-connecting to server successful", targetP->shortID);      
      }
    }
    else 
    {
      /*
       * NON_IP Transport
       *
       * Re-connect the CoAP connection with the server. Re-connect flag is set when transport binding changes are detected 
       * during data call failure or register update trigger. 
       */
      if (apn_info->reconnect)
      {
        /* Close the existing CoAP connection */
        qapi_Coap_Close_Connection(targetP->coapHandle);

        /* Create a new connection with the changed transport binding */ 
        targetP->sessionH = lwm2m_connect_server(targetP, contextP);
        if (0x00 == targetP->sessionH) 
        {
          ds_iot_ulog_err1("CoAP re-connect failed for SSID[%d]", targetP->shortID);
          goto reconnect_err;   
        }

        /* Reset the re-connect flag as the re-connection is successful */
        apn_info->reconnect = false;
        /* Set the CoAP re-connection to be successful */
        reconnect_success = true;

        ds_iot_ulog_high1("Profile change for SSID[%d], re-connecting to server successful", targetP->shortID);  
      }
    }      
           
reconnect_err:

    /* update IF MTU changed */ 
    if(mtu_changed)
    {
        targetP->mtu_size = mtu_size;
    }
    if (reconnect_success)
    {
      ds_iot_ulog_high1("CoAP re-connect success for SSID[%d]", targetP->shortID);
  
      dataP = (client_data_t *)((lwm2m_context_t *)contextP)->userData;
      if (dataP != NULL)
      {
        instance = (security_instance_t *) LWM2M_LIST_FIND(dataP->securityObjP->instanceList, targetP->secObjInstID);
        if ((instance != NULL) && (targetP->status == STATE_REGISTERED))
        {
          if (instance->securityMode == LWM2M_SECURITY_MODE_NONE)
          {
            if (!g_carrier_cfg)
            {
              targetP->trigger_registration = true;
              qurt_signal_set(&lwm2m_signal_Handle, LWM2M_TRIGGER_DISABLE);
            }
            else if ((data.cfg_info.reg_update_on_reconnect) ||
                     (apn_info->send_update))
            {
              /* Reset the register update flag */ 
              apn_info->send_update = false;  
              ds_iot_ulog_high1("Sending the register update for SSID[%d]", targetP->shortID);

              /* Send the registration update to the server */
              lwm2m_update_registration(contextP, targetP->shortID, false);
            }
          }
          else
          {
            if ((data.cfg_info.reg_update_on_reconnect) ||
                (apn_info->send_update))
            {
              /* Reset the register update flag */ 
              apn_info->send_update = false;
              ds_iot_ulog_high1("Sending the register update for SSID[%d]", targetP->shortID); 

              /* Send the registration update to the server */
              lwm2m_update_registration(contextP, targetP->shortID, false);
            }
          }
        }            
      }
    } 
  }

  return; 
}


static void lwm2m_servers_profile_change_validation(lwm2m_context_t *lwm2mH)
{
  lwm2m_server_t * targetP = NULL ;

  if (lwm2mH == NULL)
    return;

  if (lwm2mH->state != STATE_READY)
  {
    targetP = lwm2mH->bootstrapServer;

    if(targetP != NULL)
    {
      lwm2m_process_profile_change(lwm2mH, targetP);
    }
  }

  targetP = lwm2mH->serverList;

  while (targetP != NULL)
  {
    lwm2m_process_profile_change(lwm2mH, targetP);
    targetP = targetP->next;
  }
}


void lwm2m_apn_obj_event_notification(void *hndl, uint16_t short_server_id, lwm2m_data_call_event_t event, uint32_t ip_family)
{
  lwm2m_server_t *serverP = NULL; 
  
  serverP = (lwm2m_server_t *)LWM2M_LIST_FIND(data.lwm2mH->serverList, short_server_id);
  if (NULL == serverP)
  {
    ds_iot_ulog_err1("Server instance associated with the SSID[%d] is not found", short_server_id);
    return;
  }

  /* APN object event notification is only supported if the LwM2M client version is v1.1 or higher */
  if ((float)LWM2M_V_1_0 == serverP->version)
    return; 

  /* Notify the APN connection object of the data call related event */ 
  apn_info_event_notify(hndl, serverP->apn_link, event, ip_family);

  return; 
} 


void lwm2m_datacall_handle_signals(lwm2m_context_t *lwm2mH, uint32 signals)
{
  lwm2m_server_t *targetP = lwm2mH->serverList;
    
  /* On receiving "NO NET" event for the primary APNs mark the connectivity related errors */ 
  if ((signals & DSS_M2M_PRIMARY_APN_NO_NET_v4)   ||
      (signals & DSS_M2M_SECONDARY_APN_NO_NET_v4) ||    
      (signals & DSS_M2M_PRIMARY_APN_NO_NET_v6)   ||
      (signals & DSS_M2M_SECONDARY_APN_NO_NET_v6))  
  {
    carrier_apn_cfg_t *apn_info = get_lwm2m_apn_cfg();       
    
    /* Set the IP connectivity related error code in the device object (only on primary failures) */
    if ((signals & DSS_M2M_PRIMARY_APN_NO_NET_v4)  ||
        (signals & DSS_M2M_PRIMARY_APN_NO_NET_v6))
    { 
      prv_set_ipconnectivity_errorcode(lwm2mH);
    }  
    
    /* Iterate through APN connections and process those that have call state set to disconnected/failure */
    while (apn_info)
    {
      /* Notify the "Disconnected" event to the associated APN connection instance for IPv6 */
      if (LWM2M_DATA_CALL_DISCONNECTED == apn_info->v6_call_state)
      {
        lwm2m_server_t *serverP = NULL; 
        ds_iot_ulog_high1("Data call associated with IPv6 successfully disconnected for SSID[%d]", apn_info->short_server_id);
        /* Retrieve the server information associated with the server ID and change the state if it is in STATE_REG_UPDATE_PENDING */          
        serverP = (lwm2m_server_t *)lwm2m_list_find((lwm2m_list_t *)lwm2mH->serverList, apn_info->short_server_id);
		if(serverP && serverP->status == STATE_REG_UPDATE_PENDING)
		{
		   serverP->status = STATE_REGISTERED ;
		}

        lwm2m_apn_obj_event_notification(get_data_call_hndl(apn_info->callHdl_v6), apn_info->short_server_id, LWM2M_DATA_CALL_DISCONNECTED, AF_INET6);
        apn_info->v6_call_state = 0x00;
      }
      /* Notify the "Failure" event to the associated APN connection instance for IPv6 */
      else if (LWM2M_DATA_CALL_FAILURE == apn_info->v6_call_state)
      {
        lwm2m_server_t *serverP = NULL; 
        bool profile_change = false; 

        ds_iot_ulog_err1("Data call associated with IPv6 failed for SSID[%d]", apn_info->short_server_id);
        /* Retrieve the server information associated with the server ID and change the state if it is in STATE_REG_UPDATE_PENDING */          
        serverP = (lwm2m_server_t *)lwm2m_list_find((lwm2m_list_t *)lwm2mH->serverList, apn_info->short_server_id);
		if(serverP && serverP->status == STATE_REG_UPDATE_PENDING)
		{
		   serverP->status = STATE_REGISTERED ;
		}
        
        /* Notify the "failure" event to the associated APN connection instance */              
        lwm2m_apn_obj_event_notification(get_data_call_hndl(apn_info->callHdl_v6), apn_info->short_server_id, LWM2M_DATA_CALL_FAILURE, AF_INET6);
        apn_info->v6_call_state = 0x00;

        /* On PDN disconnect due to failure, check for profile changes and use the latest configuration to bring-up the data call. Actual tear-down
         * of the exisiting connection and bringing-up a new connection is done as part of the "registration step". 
         *
         * Same path is used by "register trigger (with param)" to perform any these call management changes based on any profile change detected. 
         */
        profile_change = data_call_conn_profile_change(apn_info->short_server_id);    
        if (profile_change) 
        {
          lwm2m_server_t *serverP = NULL; 

          /* Retrieve the server information associated with the server ID and set the signal indicating that the profile has changed */          
          serverP = (lwm2m_server_t *)lwm2m_list_find((lwm2m_list_t *)data.lwm2mH->serverList, apn_info->short_server_id);
          if (NULL == serverP)
            return;
      
          LWM2M_SERVER_SET_UPDATE_PROFILE_CHANGE(serverP->update_disable_check); 
        }
        else
        {
retry_primary_v6_apn:
          /* Notify the "Start" event to the associated APN connection instance */              
          lwm2m_apn_obj_event_notification(get_data_call_hndl(apn_info->callHdl_v6), apn_info->short_server_id, LWM2M_DATA_CALL_START, AF_INET6);

          /* Perform the data call re-tries to start the data call at the scheduled retry intervals (just signaling at this point) */ 
          iot_dss_start_datacall(apn_info->callHdl_v6); 

          /* If we are not successful in bringing up the primary APN (Class-2) which has gone down due to call failure even 
           * after the configured number of re-tries then we need to shutdown LwM2M client, as per the requirement.  
           */   
          if (LWM2M_APN_CLASS2 == apn_info->apn_class)
          {
             uint32 signal_attr = QURT_SIGNAL_ATTR_WAIT_ANY; 
             uint32 signal = 0x00; 
             uint32_t signal_mask = 0x00;
             uint16_t ssid = apn_info->short_server_id;

            /* Add the signals to wait on while the IPv6 data call is being brought-up. Since the APN belongs to Class 2 ensure 
             * that the call is brought-up before we proceed further. 
             */
            signal_mask |= DSS_M2M_PRIMARY_APN_CONN_v6 | DSS_M2M_APN_MAX_RETRIES | DSS_M2M_PRIMARY_APN_NO_NET_v6 | LWM2M_PS_ATTACH;
            
            /* Wait for APN class-2 data calls until the calls are brought-up successfully or max re-tries are reached. Data call 
             * management retry logic would perform the re-tries.
             */ 
            signal = qurt_signal_wait(&lwm2m_signal_Handle, signal_mask, signal_attr);
            /* PS Attach */      
            if (signal & LWM2M_PS_ATTACH)
            {
              qmi_nas_set_sys_sel_pref(QAPI_DEVICE_INFO_PS_ATTACH_E, 0, NULL);
              qurt_signal_clear(&lwm2m_signal_Handle, LWM2M_PS_ATTACH);
            }
            /* Maximum re-tries reached Primary Class-2 APN */   
            if (signal & DSS_M2M_APN_MAX_RETRIES)
            {
              /* Clear the generated "MAX RE-TRIES" signal */
              qurt_signal_clear(&lwm2m_signal_Handle, DSS_M2M_APN_MAX_RETRIES);  

              if (AF_INET_DUAL46 != apn_info->ip_family)
              { 
                ds_iot_ulog_err1("Shutting down LwM2M client as maximum re-tries reached bringing-up Primary IPv6 for SSID[%d]", ssid);              
                g_quit = 0x01;  
              }  
              else 
              { 
                if (DSS_M2M_APN_MAX_RETRIES == apn_info->v4_nip_call_state)
                { 
                  ds_iot_ulog_err1("Shutting down LwM2M client as maximum re-tries reached in bringing-up Primary IPv4 and IPv6 for SSID[%d]", ssid);              
                  g_quit = 0x01;
                }
                else
                {
                  /* Update the IPv6 call state indicating maximum retries is reached */
                  apn_info->v6_call_state = DSS_M2M_APN_MAX_RETRIES;
                  ds_iot_ulog_err1("Maximum re-tries reached in bringing-up Dual Primary IPv6... waiting for Primary IPv4 status for SSID[%d]", ssid);
                }
              }     
            }    
            /* Bringing-up the Primary Class-2 APN failed. IOT DSS thread would post the "NO NET" event for the primary APN, but the event is handled 
             * in the LwM2M main thread which is blocked here and would never get processed for primary APN. Handle the event at this point and re-try. 
             */
            else if (signal & DSS_M2M_PRIMARY_APN_NO_NET_v6)
            {
              ds_iot_ulog_err1("Primary IPv6 (class-2) failed to get connected - retry the data call bring-up for SSID[%d]", ssid);
          
              /* Clear the generated "PRIMARY V6 NO NET" signal */
              qurt_signal_clear(&lwm2m_signal_Handle, DSS_M2M_PRIMARY_APN_NO_NET_v6);  
          
              /* Notify the "FAILURE" event to the associated APN connection instance */
              lwm2m_apn_obj_event_notification(get_data_call_hndl(apn_info->callHdl_v6), apn_info->short_server_id, LWM2M_DATA_CALL_FAILURE, AF_INET6);                
              goto retry_primary_v6_apn; 
            }
            /* Primary Class-2 APN connected */ 
            else if (signal & DSS_M2M_PRIMARY_APN_CONN_v6)
            {
              ds_iot_ulog_high1("Primary IPv6 (class-2) connected for SSID[%d]", apn_info->short_server_id);
            }              
          }          
        }
      }

      /* Notify the "Disconnected" event to the associated APN connection instance for IPv4/Non-IP */
      if (LWM2M_DATA_CALL_DISCONNECTED == apn_info->v4_nip_call_state)
      {
        uint8_t family = (AF_NONIP == apn_info->transport_binding) ? AF_NONIP : AF_INET; /* IPv4 and Non_IP re-use the same handle */
        lwm2m_server_t *serverP = NULL; 
		
        ds_iot_ulog_high2("Data call associated with IPv4/NON_IP (%d) successfully disconnected for SSID[%d]", family, apn_info->short_server_id);


        /* Retrieve the server information associated with the server ID and change the state if it is in STATE_REG_UPDATE_PENDING */          
        serverP = (lwm2m_server_t *)lwm2m_list_find((lwm2m_list_t *)data.lwm2mH->serverList, apn_info->short_server_id);
		if(serverP && serverP->status == STATE_REG_UPDATE_PENDING)
		{
		   serverP->status = STATE_REGISTERED ;
		}
          
        lwm2m_apn_obj_event_notification(get_data_call_hndl(apn_info->callHdl_v4_nip), apn_info->short_server_id, LWM2M_DATA_CALL_DISCONNECTED, family);
        apn_info->v4_nip_call_state = 0x00; 
      }
      /* Notify the "Failure" event to the associated APN connection instance for IPv4/Non-IP */
      else if (LWM2M_DATA_CALL_FAILURE == apn_info->v4_nip_call_state)
      {
        bool profile_change = false; 
        uint8_t family = (AF_NONIP == apn_info->transport_binding) ? AF_NONIP : AF_INET; /* IPv4 and Non_IP re-use the same handle */
        lwm2m_server_t *serverP = NULL; 
		
        ds_iot_ulog_err2("Data call associated with IPv4/NON_IP (%d) failed for SSID[%d]", family, apn_info->short_server_id); 

		/* Retrieve the server information associated with the server ID and change the state if it is in STATE_REG_UPDATE_PENDING */          
        serverP = (lwm2m_server_t *)lwm2m_list_find((lwm2m_list_t *)data.lwm2mH->serverList, apn_info->short_server_id);
		if(serverP && serverP->status == STATE_REG_UPDATE_PENDING)
		{
		   serverP->status = STATE_REGISTERED ;
		}
        /* Notify the "failure" event to the associated APN connection instance */              
        lwm2m_apn_obj_event_notification(get_data_call_hndl(apn_info->callHdl_v4_nip), apn_info->short_server_id, LWM2M_DATA_CALL_FAILURE, family);
        apn_info->v4_nip_call_state = 0x00; 

        /* On PDN disconnect due to failure, check for profile changes and use the latest configuration to bring-up the data call. Actual tear-down
         * of the exisiting connection and bringing-up a new connection is done as part of the "registration step". 
         *
         * Same path is used by "register trigger (with param)" to perform any these call management changes based on any profile change detected. 
         */
        profile_change = data_call_conn_profile_change(apn_info->short_server_id);    
        if (profile_change) 
        {
          lwm2m_server_t *serverP = NULL; 

          /* Retrieve the server information associated with the server ID and set the signal indicating that the profile has changed */          
          serverP = (lwm2m_server_t *)lwm2m_list_find((lwm2m_list_t *)data.lwm2mH->serverList, apn_info->short_server_id);
          if (NULL == serverP)
            return;
      
          LWM2M_SERVER_SET_UPDATE_PROFILE_CHANGE(serverP->update_disable_check); 
        }
        else
        {
retry_primary_v4_apn:
          /* Notify the "Start" event to the associated APN connection instance */              
          lwm2m_apn_obj_event_notification(get_data_call_hndl(apn_info->callHdl_v4_nip), apn_info->short_server_id, LWM2M_DATA_CALL_START, family);
          
          /* Perform the data call re-tries to start the data call at the scehduled retry intervals (just signaling at this point) */         
          iot_dss_start_datacall(apn_info->callHdl_v4_nip); 
          
          /* If we are not successful in bringing up the primary APN (Class-2) which has gone down due to call failure even after the configured 
           * number of re-tries then we need to shutdown LwM2M client, as per the requirement.  
           */   
          if (LWM2M_APN_CLASS2 == apn_info->apn_class)
          {
             uint32 signal_attr = QURT_SIGNAL_ATTR_WAIT_ANY; 
             uint32 signal = 0x00; 
             uint32_t signal_mask = 0x00;
             uint16_t ssid = apn_info->short_server_id;

            /* Add the signals to wait on while the IPv4 data call is being brought-up. Since the APN belongs to Class 2 ensure 
             * that the call is brought-up before we proceed further. 
             */
            signal_mask |= DSS_M2M_PRIMARY_APN_CONN_v4 | DSS_M2M_APN_MAX_RETRIES | DSS_M2M_PRIMARY_APN_NO_NET_v4 | LWM2M_PS_ATTACH;
            
            /* Wait for APN class-2 data calls until the calls are brought-up successfully or max re-tries are reached. Data call 
             * management retry logic would perform the re-tries.
             */ 
            signal = qurt_signal_wait(&lwm2m_signal_Handle, signal_mask, signal_attr);
            /* PS Attach */  
            if (signal & LWM2M_PS_ATTACH)
            {
               qmi_nas_set_sys_sel_pref(QAPI_DEVICE_INFO_PS_ATTACH_E, 0, NULL);
               qurt_signal_clear(&lwm2m_signal_Handle, LWM2M_PS_ATTACH);
            }
            /* Maximum re-tries reached Primary Class-2 APN */ 
            if (signal & DSS_M2M_APN_MAX_RETRIES)
            {
              /* Clear the generated "MAX RE-TRIES" signal */
              qurt_signal_clear(&lwm2m_signal_Handle, DSS_M2M_APN_MAX_RETRIES);  

              if (AF_INET_DUAL46 != apn_info->ip_family)
              { 
                ds_iot_ulog_err2("Shutting down LwM2M client as maximum re-tries reached in bringing-up Primary IPv4/NONIP (%d) for SSID[%d]", family, ssid);              
                g_quit = 0x01;  
              }  
              else 
              { 
                if (DSS_M2M_APN_MAX_RETRIES == apn_info->v6_call_state)
                { 
                  ds_iot_ulog_err1("Shutting down LwM2M client as maximum re-tries reached in bringing-up DUAL-IP Primary IPv4/NON_IP and IPv6 for SSID[%d]", ssid);              
                  g_quit = 0x01;
                }
                else
                {
                  /* Update the IPv4 call state indicating maximum retries is reached */
                  apn_info->v4_nip_call_state = DSS_M2M_APN_MAX_RETRIES;
                  ds_iot_ulog_err2("Maximum re-tries reached bringing Dual Primary IPv4/NON_IP (%d)... waiting for Primary IPv6 status for SSID[%d]", family, ssid);
                }
              }     
            }
            /* Bringing-up the Primary Class-2 APN failed. IOT DSS thread would post the "NO NET" event for the primary APN, but the event is handled 
             * in the LwM2M main thread which is blocked here and would never get processed for primary APN. Handle the event at this point and re-try. 
             */
            else if (signal & DSS_M2M_PRIMARY_APN_NO_NET_v4)
            {
              ds_iot_ulog_err2("Primary IPv4/NON_IP (%d) failed to get connected - retry the data call bring-up for SSID[%d]", family, ssid);

              /* Clear the generated "PRIMARY V4 NO NET" signal */
              qurt_signal_clear(&lwm2m_signal_Handle, DSS_M2M_PRIMARY_APN_NO_NET_v4);  
 
              /* Notify the "failure" event to the associated APN connection instance */
              lwm2m_apn_obj_event_notification(get_data_call_hndl(apn_info->callHdl_v4_nip), apn_info->short_server_id, LWM2M_DATA_CALL_FAILURE, family);
              goto retry_primary_v4_apn; 
            }
            /* Primary Class-2 APN connected */ 
            else if (signal & DSS_M2M_PRIMARY_APN_CONN_v4)
            {
              ds_iot_ulog_high2("Primary IPv4/NON_IP (%d) connected for SSID[%d]", family, ssid);
            }      
          }     
        }
      }   
    
      apn_info = apn_info->next; 
    } 
  }

  /* Wait for full service if NAS has indicated there is no signal */
  if (signals & DSS_M2M_WAIT_FOR_NAS_UP)
  {
    if (iotapp_qmi_get_service_status(false) == FALSE) 
    {
      int signals_tmp;

      ds_iot_ulog_high("Network service is not available, wait for service up signal");

      /*Wait factory-reset/restart signal whenever since QAPI can trigger these events anytime */
      wait_for_nas_up:
      signals_tmp = qurt_signal_wait(&lwm2m_signal_Handle, 
                                     LWM2M_NAS_UP_SIG | LWM2M_RESTART | LWM2M_FACTORY_RESET_SET | LWM2M_PS_ATTACH, 
                                     QURT_SIGNAL_ATTR_WAIT_ANY | QURT_SIGNAL_ATTR_CLEAR_MASK);
      if ((signals_tmp & LWM2M_FACTORY_RESET_SET) || (signals_tmp & LWM2M_RESTART))
      {
        ds_iot_ulog_high("Got restart/request, exit");

        /* Re-send the signal to handle in next process_signals */
        qurt_signal_clear(&lwm2m_signal_Handle, signals_tmp);	  
        qurt_signal_set(&lwm2m_signal_Handle, signals_tmp);		
        return;
      }	  

      if (signals_tmp & LWM2M_PS_ATTACH)      
      {   
        qmi_nas_set_sys_sel_pref(QAPI_DEVICE_INFO_PS_ATTACH_E, 0, NULL);     
      }
	  
      if ((signals_tmp & LWM2M_NAS_UP_SIG) == 0)      
      {        
        goto wait_for_nas_up;      
      }

      ds_iot_ulog_high("Network service is available, proceeding...");
    }
  }

  /* On receiving "CONNECTED" event for either primary or secondary APNs check if there is any IP address changes */
  if ((signals & DSS_M2M_PRIMARY_APN_CONN_v4)   || 
      (signals & DSS_M2M_SECONDARY_APN_CONN_v4) ||
      (signals & DSS_M2M_PRIMARY_APN_CONN_v6)   || 
      (signals & DSS_M2M_SECONDARY_APN_CONN_v6)) 
  {
    carrier_apn_cfg_t *apn_info = get_lwm2m_apn_cfg();

    /* Iterate through APN connections and process those that have call state set to connected */
    while (apn_info)
    {
      /* Notify the "Connected" event to the associated APN connection instance for IPv4/Non-IP */
      if (LWM2M_DATA_CALL_CONNECTED == apn_info->v4_nip_call_state)
      {
        uint8_t family = (AF_NONIP == apn_info->transport_binding) ? AF_NONIP : AF_INET; /* IPv4 and Non_IP re-use the same handle */

        ds_iot_ulog_high2("Data call associated with IPv4/NON_IP (%d) successfully connected for SSID[%d]", family, apn_info->short_server_id);

        lwm2m_apn_obj_event_notification(get_data_call_hndl(apn_info->callHdl_v4_nip), apn_info->short_server_id, LWM2M_DATA_CALL_CONNECTED, family);
        apn_info->v4_nip_call_state = 0x00; 
      }
       
      /* Notify the "Connected" event to the associated APN connection instance for IPv6 */
      if (LWM2M_DATA_CALL_CONNECTED == apn_info->v6_call_state)
      {
        ds_iot_ulog_high1("Data call associated with IPv6 successfully connected for SSID[%d]", apn_info->short_server_id);

        lwm2m_apn_obj_event_notification(get_data_call_hndl(apn_info->callHdl_v6), apn_info->short_server_id, LWM2M_DATA_CALL_CONNECTED, AF_INET6);
        apn_info->v6_call_state = 0x00;
      }    
    
      apn_info = apn_info->next; 
    }

    /* Perform profile change validation and reconnect if necessary */ 
    lwm2m_servers_profile_change_validation(lwm2mH);
  }

  /* Any pending register updates triggered by SMS */ 
  while (targetP)
  {
    char server_ip[PRV_ADDRSTRLEN] = {0};

    if ((targetP->isRegupdate_for_sms) && 
        (get_ip_for_carrier_apn_cfg(lwm2mH->apn_cfg, targetP, server_ip)!= LWM2M_400_BAD_REQUEST))
    {
      ds_iot_ulog_high1("Found pending \"register update\" for SSID[%d]", targetP->shortID);

      lwm2m_update_registration_for_server(lwm2mH, targetP, FALSE);
      targetP->isRegupdate_for_sms = FALSE;
    }

    targetP = targetP->next;
  }
  
  return;
}


/**
 * @fn void lwm2m_main()
 * @brief entry point to lwm2m client 
 * @param none
 * @return void
 */
void lwm2m_main(void *args)
{
  lwm2m_context_t *lwm2mH = NULL;
  time_t tv_sec = 0x05;
  int fd = 0;
  int32_t ret_value= 0;
  uint32_t rebootstrap_req = 0;
  lwm2m_status_t reg_status = STATE_DEREGISTERED ;
  uint64_t time_for_bs = 0;

thread_reinit:
  /* ensure re-init the local variable */
  lwm2mH = NULL;
  tv_sec = 0x05;
  fd = 0;
  ret_value= 0;
  rebootstrap_req = 0;
  reg_status = STATE_DEREGISTERED ;
  time_for_bs = 0;
  
  factory_reset_req = 0;
  g_restart = 0;

  if (g_quit == 0)
  {
    fd = efs_open("/datatx/lwm2m_app_autostart", O_RDONLY);
    if(fd > -1)
    {
      ds_iot_ulog_high("lwm2m client is enabled through config file");
      m2m_thread_started = 1;

      efs_close(fd);
    } 
    else
    {
      ds_iot_ulog_high("lwm2m client is not enabled through config file");
      return;      
    }

  }
  else 
  {
    g_quit = 0;
    // Enters in case factory reset 
    if(lwm2m_in_sleep_state == LWM2M_TRUE)
    {
      lwm2m_in_sleep_state = LWM2M_FALSE;
    }
    timer_undef(&signalWaitTimeout);
    timer_sleep(2, T_SEC, 1);
  }

  isLwm2mStartedByApp = false;

  timer_def_osal(&signalWaitTimeout, &lwm2mTimerGroup,
      TIMER_NATIVE_OS_SIGNAL_TYPE, &lwm2m_signal_Handle, LWM2M_UNBLOCK_EVENT);  // Do not remove this timer
  qurt_signal_init(&lwm2m_signal_Handle);
  qurt_signal_clear(&lwm2m_signal_Handle, DSS_M2M_PRIMARY_APN_CONN_v4);
  qurt_signal_clear(&lwm2m_signal_Handle, DSS_M2M_PRIMARY_APN_NO_NET_v4);
  qurt_signal_clear(&lwm2m_signal_Handle, DSS_M2M_SECONDARY_APN_CONN_v4);
  qurt_signal_clear(&lwm2m_signal_Handle, DSS_M2M_SECONDARY_APN_NO_NET_v4);
  qurt_signal_clear(&lwm2m_signal_Handle, DSS_M2M_PRIMARY_APN_CONN_v6);
  qurt_signal_clear(&lwm2m_signal_Handle, DSS_M2M_PRIMARY_APN_NO_NET_v6);
  qurt_signal_clear(&lwm2m_signal_Handle, DSS_M2M_SECONDARY_APN_CONN_v6);
  qurt_signal_clear(&lwm2m_signal_Handle, DSS_M2M_SECONDARY_APN_NO_NET_v6);
  qurt_signal_clear(&lwm2m_signal_Handle, WMS_MT_MESSAGE_IND);


   memset(&data, 0, sizeof(client_data_t));
  /* Initialize the application context */
   memset (&data.glwm2m_app_ctx, 0x00, sizeof(lwm2m_app_ctx_t));
   /* Assign a start value to the handle identity counter */   
   data.glwm2m_app_ctx.handle_id_cntr = 100;  
   /* Initialize the application global mutex */
   qurt_mutex_init(&data.glwm2m_app_ctx.lwm2m_app_global_mutex);


  lwm2mH = prv_initialize_lwm2m();
  if (NULL == lwm2mH)
  {
    ds_iot_ulog_err("Failed to initialize lwm2m");

    //! Remove all persistent info
    LOG_DEBUG("purge all persisent files for next restart");
    purge_persistent_values();

    return;
  }
  
  /* Fetch EXCHANGE_LIFETIME from CoAP as defind by RFC 7252 */

  /* LWM2M global client context */
  data.lwm2mH = lwm2mH;
  ds_iot_log_strf_med("EFS_OPT: File open %s",BOOTSTRAP_STATUS_PERSISTENCE_FILE);
  fd = efs_open(BOOTSTRAP_STATUS_PERSISTENCE_FILE, O_RDONLY);
  if (EFAILURE == fd)
  {
    LOG_DEBUG("Failed to open BS persistence file");
  }
  
  if (sizeof(time_t) != efs_read(fd, &time_for_bs, sizeof(time_t)))
  {
    LOG_DEBUG("Failed to read BS persistence file");
  }
  efs_close(fd);

  if ((time_for_bs != 0))
  {
    lwm2mH->time_for_bs = time_for_bs;
  }

  //efs_unlink(BOOTSTRAP_STATUS_PERSISTENCE_FILE);

  /* Load the application re-bootstrap persistence file */ 
  rebootstrap_req = 0x00;
  if (lwm2m_load_application_rebootstrap_info (&rebootstrap_req))
  {
    rebootstrap_req = 0x00;
    ds_iot_ulog_err("Failed to load the application re-bootstrap information");
  }
  
  /* Device state - set to active */
  data.glwm2m_app_ctx.device_active = TRUE;  
  /* Set the bootstrap context information */
  set_bootstrap_context(rebootstrap_req); 
  /* Indicate that the application context is initialized */ 
  data.glwm2m_app_ctx.lwm2m_app_inited = TRUE;

  /* Indicate applications that the LWM2M client module is initialized and they can create object instances */ 
  lwm2m_send_internal_ind_to_application(QAPI_NET_LWM2M_STATE_INITIAL_E);

  /* Indicate applications that the LWM2M client module is awake */ 
  lwm2m_send_internal_ind_to_application(QAPI_NET_LWM2M_WAKEUP_E);
 
  if (roaming_feat_enabled)
  {   
    /* Check roaming status*/
    check_roaming_status();
  }

  /* Provisioning the certificates if there are anything in AFS. */ 
  lwm2m_update_security_credentials(lwm2mH);

  /* Ensure that the network service is available */ 
  qapi_Device_Info_Set_Callback_v2(lwm2m_dev_info_hdl, QAPI_DEVICE_INFO_ANY, lwm2m_nas_status);

  /* Bootstrap-byApp mode:
   *     Need wait bootstrap-ondemand-ind which trigger by QAPI durint bootup, also handle 
   *     restart/factory-reset which can be trigger by QAPI anytime 
   */
  if (data.cfg_info.lwm2m_startup_mode == LWM2M_STARTUP_BOOTSTRAP_BYAPP)
  {
	uint8_t bWaitForByAppMode_Executed = 0;
    while(1)
    {
      uint32 signals_tmp = 0; 
	  uint32_t signals_to_wait = 0;
	  
      ds_iot_ulog_high("Waiting Client Config command");

      signals_to_wait = LWM2M_FACTORY_RESET_SET|LWM2M_RESTART|LWM2M_BOOTSTRAP_ONDEMAND_IND|LWM2M_PS_ATTACH;
      if(bWaitForByAppMode_Executed == 0 && data.cfg_info.startup_wait_time_for_byapp_mode > 0)
      {
        int signal_ret;
        bWaitForByAppMode_Executed = 1;
        signal_ret = qurt_signal_wait_timed_ext(&lwm2m_signal_Handle, 
                                             signals_to_wait, 
                                             QURT_SIGNAL_ATTR_WAIT_ANY|QURT_SIGNAL_ATTR_CLEAR_MASK, 
                                             &signals_tmp,
                                             qurt_timer_convert_time_to_ticks(data.cfg_info.startup_wait_time_for_byapp_mode, QURT_TIME_SEC));
        if (QURT_EFAILED_TIMEOUT == signal_ret)
        {
          qapi_Status_t start_status;
          LOG_DEBUG("wait application start lwm2m -- time out! start lwm2m auto now");
          start_status = lwm2m_start_byApp();
          if(start_status == QAPI_OK) /*start success*/
          {
            signals_tmp |= LWM2M_BOOTSTRAP_ONDEMAND_IND;
          }
        }
        else if (QURT_EOK != signal_ret)
        {
          ds_iot_ulog_err("Signal wait timeout out failed"); /* should not happen */
          continue;
        }
      }
      else
      {
        signals_tmp = qurt_signal_wait(&lwm2m_signal_Handle, signals_to_wait, QURT_SIGNAL_ATTR_WAIT_ANY|QURT_SIGNAL_ATTR_CLEAR_MASK);
      }
  
      factory_reset_req = (signals_tmp & LWM2M_FACTORY_RESET_SET);
      g_restart         = (signals_tmp & LWM2M_RESTART);

      if (factory_reset_req || g_restart)
      {
        ds_iot_ulog_high("Get restart request, exit");
        qurt_signal_clear(&lwm2m_signal_Handle, signals_tmp);	
        goto end;
      }

      qurt_signal_clear(&lwm2m_signal_Handle, signals_tmp);	  
      if (signals_tmp & LWM2M_BOOTSTRAP_ONDEMAND_IND)
      {
        uint8_t bs_server_count;
        ds_iot_ulog_high("Get Start command, begin LwM2M");

        bs_server_count = lwm2m_get_server_count(lwm2mH, TRUE);

        if (bs_server_count > 1)
        {
          ds_iot_ulog_err("Multi-bootstrap server configuration not supported");
          goto end;
        }
        else if(0 == bs_server_count)
        {
          if(!bootstrap_checkAllConsistence(lwm2mH, FALSE))
          {
            ds_iot_ulog_err("Factory bootstrap consistence check failure");
            goto end;
          }
        }
        break;
      }
      if(signals_tmp & LWM2M_PS_ATTACH)
      {
         qmi_nas_set_sys_sel_pref(QAPI_DEVICE_INFO_PS_ATTACH_E, 0, NULL);
         qurt_signal_clear(&lwm2m_signal_Handle, LWM2M_PS_ATTACH);
      }
    }
  }

  if (iotapp_qmi_get_service_status(false) == FALSE) 
  {
    uint32_t signals_tmp; 
    ds_iot_ulog_high("Network service is not available, wait for service up signal");

    /* Wait factory-reset/restart signal whenever since QAPI can trigger these events anytime */
    signals_tmp = qurt_signal_wait(&lwm2m_signal_Handle, LWM2M_NAS_UP_SIG|LWM2M_FACTORY_RESET_SET|LWM2M_RESTART|LWM2M_PS_ATTACH, QURT_SIGNAL_ATTR_WAIT_ANY|QURT_SIGNAL_ATTR_CLEAR_MASK);
	
    factory_reset_req = (signals_tmp & LWM2M_FACTORY_RESET_SET);
    g_restart         = (signals_tmp & LWM2M_RESTART);
    if (factory_reset_req || g_restart)
    {
      ds_iot_ulog_high("Get restart request, exit");

      qurt_signal_clear(&lwm2m_signal_Handle, signals_tmp);	
      goto end;
    } 
	if(signals_tmp & LWM2M_PS_ATTACH)
    {
       qmi_nas_set_sys_sel_pref(QAPI_DEVICE_INFO_PS_ATTACH_E, 0, NULL);
       qurt_signal_clear(&lwm2m_signal_Handle, LWM2M_PS_ATTACH);
    }

    ds_iot_ulog_high("Network service is available...");
  }	  

  if (data.cfg_info.lwm2m_startup_mode == LWM2M_STARTUP_BOOTSTRAP_PRELOADED)
    StartExtObjCreateWindow(lwm2mH);
  
  active_time = lwm2m_gettime();  
  
  while (g_quit == 0)
  {
    #ifdef PSM_LWM2M
    if (lwm2m_psm_enable == TRUE)
    {
      if (psm_inited == TRUE)
      {
        lwm2m_psm_context_t *psm_info = data.lwm2mH->lwm2m_psm_info;
        qapi_Status_t result = QAPI_ERROR;

        if (psm_info)
        {
          if (psm_info->psm_registered == FALSE)
          {
            result = qapi_PSM_Client_Register(&psm_info->client_id, lwm2m_psm_cb, &psm_info->psm_msg);
  
            if (result == QAPI_OK)
            {
              ds_iot_ulog_high1("LWM2M registered with PSM client id is %d", psm_info->client_id);
              psm_info->psm_registered = TRUE;
            }
          }  // psm_info->psm_registered == FALSE
        }  // psm_info != NULL
      }  // psm_inited == TRUE
    }  // lwm2m_psm_enable == TRUE
    #endif
    ret_value = lwm2m_step(lwm2mH, &tv_sec);
    if (ret_value == LWM2M_503_SERVICE_UNAVAILABLE)
    { 
      ds_iot_ulog_err ("503 No service availbale - exiting LwM2M");
      g_quit = 1;
	  break;
    }
    
#ifdef M2M_DEBUG          
    lwm2m_print_status(lwm2mH);
#endif

    /* Check if device needs to go to sleep */
    curr_time = lwm2m_gettime();
    reg_status = get_current_regstatus(lwm2mH);
    if (((reg_status == STATE_REGISTERED) || 
         (reg_status == STATE_REG_UPDATE_PENDING)) && 
        (lwm2m_in_sleep_state != LWM2M_TRUE)) 
    {
      if ((curr_time - active_time) >= sleepAckTimeout) 
      {
        LOG_DEBUG("Current time - %u, last active time - %u", curr_time, active_time);
        ds_iot_ulog_high("Entering sleep state due to inactivity");
        lwm2m_in_sleep_state = LWM2M_TRUE; //Device enters into sleep state
        
        /* Indicate OEM applications that the device is entering sleep state */
        lwm2m_send_internal_ind_to_application(QAPI_NET_LWM2M_SLEEP_E);
      } 
    }  

    /* Process the signals that are pending */
    tv_sec = tv_sec == 0 ? 1 : tv_sec; 
    process_signal(lwm2mH, tv_sec);
	   tv_sec = 5;

    if (g_reboot || g_restart)
    {
      ds_iot_ulog_high("lwm2m handling Reboot/Restart..........");
      g_quit = 1;
    }
	
    //! checking whether factory reset is triggerd by server or not
    if (factory_reset_req) 
    {
      /*deleting old certificate files from secure location*/
      lwm2m_delete_server_security_credentials(lwm2mH);

      prv_deregister(lwm2mH);
      //! Remove all persistent info and restart application
      purge_persistent_values();
      g_quit = 1;
	  
      ds_iot_ulog_high("lwm2m handling the factory reset");
      goto end;
    }

    ds_iot_ulog_high2 ("Current registration status %d and LWM2M sleep state %d", reg_status, lwm2m_in_sleep_state);
  }    

end:
  qurt_mutex_lock(&data.glwm2m_app_ctx.lwm2m_app_global_mutex);

#ifdef PSM_LWM2M
  if(lwm2m_psm_enable == TRUE)
  {
    if(psm_inited == TRUE)
    {
      lwm2m_psm_context_t *psm_info = data.lwm2mH->lwm2m_psm_info;
      qapi_Status_t result = QAPI_ERROR;
      if(psm_info)
      {
        if(psm_info->psm_registered == TRUE)
        {
          result = qapi_PSM_Client_Unregister(psm_info->client_id);
  
          if(result == QAPI_OK)
          {
            ds_iot_ulog_high("LWM2M client de-registered.");
          }
        }  // psm_info->psm_registered == TRUE
      }  // psm_info != NULL
    }  // psm_inited == TRUE

    /* Destroy the PSM structures */
    lwm2m_psm_deinit(lwm2mH);
  }
#endif
  if (!g_reboot)
  {
    carrier_apn_cfg_t *info = NULL;
    carrier_apn_cfg_t *apnInfo = NULL;
 
    ds_iot_ulog_high("Shutting down LWM2M...");
    /* in case just want to restart lwm2m, then don't execute de-register */
    if(g_restart != 1) 
      lwm2m_deregister(lwm2mH);
	
    /* Stop all the data calls */ 
    lwm2m_datacall_stop(lwm2mH, LWM2M_STOP_BOOTSTRAP_DATA_CALL, NULL, false);
    lwm2m_datacall_stop(lwm2mH, LWM2M_STOP_REGISTRATION_DATA_CALL, NULL, false);

    /* Release the resources associated with the APN config */
    lwm2m_free_apn_list(lwm2mH->apn_cfg);
    lwm2mH->apn_cfg = NULL;
    apnInfo = lwm2mH->c_apn_cfg;
    while(apnInfo)
    {
      info = apnInfo;
      apnInfo = apnInfo->next;
      lwm2m_free(info);
    }  
    
    /* Release the resources associated with the LwM2M config */ 
    if (data.cfg_info.retry_timeout_lst)
    {
      /* Retry connection schedule */
      lwm2m_free(data.cfg_info.retry_timeout_lst);
      data.cfg_info.retry_timeout_lst = NULL; 
      data.cfg_info.retry_timeout_lst_len = 0x00; 
    }
  
    /* Release the LwM2M global context */ 
    lwm2m_close(lwm2mH);
    lwm2mH = NULL;         
   
    /* Destroy the objects */
    prv_destroy_objects(objArray);

    //! releasing the client handler for each service 
	/*
    iotapp_qmi_dms_release();      
    iotapp_qmi_nas_release();
    */

    if(enable_location && location_init_status)
      lh_location_deinit();
	/*
    iotapp_qmi_wds_release();
    iotapp_qmi_dsd_release(); 
    iotapp_qmi_wms_release();
    iotapp_qmi_uim_release();
	*/

	qapi_Device_Info_Release_v2(lwm2m_dev_info_hdl);
    ds_iot_ulog_high("LWM2M Shutdown complete");
  }
  else
  {
    ds_iot_ulog_high("Rebooting the device...");
         
    /* Indicate the applications of the reboot */
    lwm2m_send_internal_ind_to_application(QAPI_NET_LWM2M_DEVICE_REBOOT_E);
    
    /* Stop all the data calls */
    lwm2m_datacall_stop(lwm2mH, LWM2M_STOP_BOOTSTRAP_DATA_CALL, NULL, false);
    lwm2m_datacall_stop(lwm2mH, LWM2M_STOP_REGISTRATION_DATA_CALL, NULL, false);
      
    /* Reboot the device */ 
    sys_m_reset();
  }

  if (factory_reset_req || g_restart || (data.cfg_info.lwm2m_startup_mode == LWM2M_STARTUP_BOOTSTRAP_BYAPP))     
  {
    if(factory_reset_req || g_restart)
    {
      ds_iot_ulog_high("Factory reset or Restart was recevied, restarting LWM2M");
    }
    else
    {
      ds_iot_ulog_high("restarting LWM2M");
    }

   /*Purge persistent files before indicating to applications */	
   if(factory_reset_req)
      purge_persistent_values();
        
    /* Indicate the applications of the factory reset */    
    if (factory_reset_req)
    {
      // factory reset is trigged by deconfig API
      lwm2m_send_internal_ind_to_application(QAPI_NET_LWM2M_DEVICE_FACTORY_RESET_E);
    }
    else
    {
      lwm2m_send_internal_ind_to_application(QAPI_NET_LWM2M_CLIENT_RESET_E);
    }
    
    memset(&data.lwm2mH, 0, sizeof(data.lwm2mH));
    data.lwm2mH = NULL;
    memset(&data, 0, sizeof(data));
    deinit_value_change();
    qapi_Coap_Destroy_Session(lwm2m_coap_hdl);

    qurt_signal_destroy(&lwm2m_signal_Handle);
    goto thread_reinit;
  }
  else
  {
    data.lwm2mH = NULL;
    /* Set LwM2M module to uninitialized to reject all LwM2M QAPI and exit the LwM2M thread */
    qurt_mutex_unlock(&data.glwm2m_app_ctx.lwm2m_app_global_mutex);
    data.glwm2m_app_ctx.lwm2m_app_inited = FALSE;
  }
} 


/**
 * @fn int lwm2m_client_start()
 * @brief   This function is used to start Lwm2m client thread
 * @param   void
 * @return  QURT_EOK
 */
int lwm2m_client_start()
{
  qurt_thread_attr_t Thread_Attribute;
  int res = 1;

  /* Register the LWM2M handler with kernel */  
#ifndef MOB_TX
  qurt_qapi_register(TXM_QAPI_NET_LWM2M_BASE + 1, TXM_QAPI_NET_LWM2M_NUM_IDS, qapi_LWM2Mc_Handler);
  
  qurt_qapi_register(TXM_QAPI_NET_LWM2M_BASE_V2 + 1, TXM_QAPI_NET_LWM2M_NUM_IDS_V2, qapi_LWM2Mc_Handler);
#endif
  qurt_thread_attr_init(&Thread_Attribute);
  qurt_thread_attr_set_name(&Thread_Attribute, "LWM2M");
  qurt_thread_attr_set_priority(&Thread_Attribute, LWM2M_PRIORITY);
  qurt_thread_attr_set_stack_size(&Thread_Attribute, LWM2M_STACK_SIZE); 
  res = qurt_thread_create(&lwm2m_thread_Handle, &Thread_Attribute, lwm2m_main, NULL);    
  if(res != QURT_EOK) 
  {
    ds_iot_ulog_err(" Thread failed");
  }
  return QURT_EOK;
}

uint8_t get_battery_level_threshold()
{
  return data.cfg_info.battery_level_threshold;
}

lwm2m_context_t *get_lwm2m_context()
{
  return (data.lwm2mH);
}

carrier_apn_cfg_t *get_lwm2m_apn_cfg()
{
  if(data.lwm2mH)
  return (data.lwm2mH->apn_cfg);
  else
  return NULL;
}

void lwm2m_process_pending_location_read_observe()
{
   lwm2m_location_read_observe_pending_t *read_observe_pend = NULL;
   uint16_t short_serverID = 0x00; 
   int32_t err = QAPI_ERROR;
   qapi_Coap_Packet_t *message = NULL; 
   qapi_Coap_Message_Params_t tx_message;
   lwm2m_server_t *server = NULL; 
   lwm2m_uri_t *uriP = NULL;
   uint8_t * buffer = NULL;
   size_t length = 0;
   lwm2m_data_t * dataP = NULL;
   int size = 0;
   uint8_t result;
   lwm2m_watcher_t *watcherP = NULL;

   while(q_cnt(&lwm2m_location_pending_read_observe))
   {
      size = 0;
      read_observe_pend = (lwm2m_location_read_observe_pending_t *)q_get(&lwm2m_location_pending_read_observe);

      if(read_observe_pend == NULL)
      {
        return;
      }
      short_serverID = read_observe_pend->ssid;
      uriP = &read_observe_pend->uriP;

      /* Traverse the server list to find the appropriate context associated with the message */
     server = (lwm2m_server_t *)LWM2M_LIST_FIND(data.lwm2mH->serverList, short_serverID);

      if ((NULL == server) || ((server->status != STATE_REGISTERED && server->status != STATE_REG_UPDATE_PENDING)))
      {
         ds_iot_ulog_err1("Server[0x%x] invalid/Not Registered - Failed to send LWM2M Location Pending read for ", short_serverID);
         PRINT_URI(uriP);
         goto next_pending_read;
      }

     /* Initialize the message to be sent */
     memset(&tx_message, 0x00, sizeof(qapi_Coap_Message_Params_t));
     tx_message.msgUsrData = (void *)server;

     /* Create the message to be sent to the LWM2M server */
     if (QAPI_ERROR == qapi_Coap_Init_Message(server->coapHandle,&message, QAPI_COAP_TYPE_NON, LWM2M_NO_ERROR))
     {
       goto next_pending_read;
     }

      result = object_read(data.lwm2mH, uriP, &read_observe_pend->format, &buffer, &length, &dataP, &size, server);

      if(dataP == NULL)
      {
        result = LWM2M_500_INTERNAL_SERVER_ERROR;
        goto send_message;
      }
      if (dataP && (read_observe_pend->accept == LWM2M_CONTENT_TEXT
            || read_observe_pend->accept == LWM2M_CONTENT_OPAQUE))
      {
        if (size != 1
            || !LWM2M_URI_IS_SET_RESOURCE(uriP)
            || dataP->type == LWM2M_TYPE_OBJECT
            || dataP->type == LWM2M_TYPE_OBJECT_INSTANCE
            || dataP->type == LWM2M_TYPE_MULTIPLE_RESOURCE)
        {
          result = LWM2M_406_NOT_ACCEPTABLE;
        }
      }

      if (LWM2M_205_CONTENT != result)
      {
        ds_iot_ulog_err("Object read failed for uri ");
        goto send_message;
      }

      if(read_observe_pend->observe_req)
      {
        watcherP = prv_getWatcher(data.lwm2mH,uriP, server);
        if(watcherP == NULL) 
        {
          result = LWM2M_500_INTERNAL_SERVER_ERROR;
          goto send_message;
        }
        else if(watcherP->active == false)
        goto next_pending_read;

       qapi_Coap_Set_Header(server->coapHandle,message ,QAPI_COAP_OBSERVE , &(watcherP->counter) , 0 );
       watcherP->counter++;
       watcherP->lastTime = lwm2m_gettime();
       tx_message.lastmid = &watcherP->lastMid;
       watcherP->update = false;
       watcherP->pmin_wait_count = 0;  // Reset counter
       #ifdef PSM_LWM2M
	   result = lwm2m_psm_operation(data.lwm2mH, server->shortID, uriP, PSM_NODE_OBSERVATION, PSM_NODE_UPDATE, PSM_MODIFY_NODE);
      #endif

       if(LWM2M_URI_IS_SET_RESOURCE(uriP))
       {
         // Read resource to value to check value change
         result = object_readData(data.lwm2mH, uriP, &size, &dataP, watcherP->server);
         if (LWM2M_205_CONTENT != result)
         {
           goto send_message;
         }
         if (NULL != dataP)
         {
           switch (dataP->type)
           {
             case LWM2M_TYPE_INTEGER:
               lwm2m_data_decode_int(dataP, &watcherP->lastValue.asInteger);
               break;
             case LWM2M_TYPE_FLOAT:
               lwm2m_data_decode_float(dataP, &watcherP->lastValue.asFloat);
               break;
             default:
               break;
           }
         }
       }
     }

send_message:
    qapi_Coap_Set_Header(server->coapHandle,message, QAPI_COAP_TOKEN, read_observe_pend->token, read_observe_pend->token_len);
    qapi_Coap_Set_Header(server->coapHandle,message, QAPI_COAP_SET_STATUS, &result,0);
    if(result == LWM2M_205_CONTENT)
    {
      qapi_Coap_Set_Header(server->coapHandle,message, QAPI_COAP_CONTENT_TYPE, &read_observe_pend->format, 0);
      qapi_Coap_Set_Payload(server->coapHandle,message, buffer, length);
    }
    /* Send the message to the LWM2M server */
    err = qapi_Coap_Send_Message(server->coapHandle ,message, &tx_message);
    if (err != QAPI_OK)
    {
      LOG_DEBUG("Failed to send the LWM2M location read pending message");
      lwm2m_clean_coap_session(server);
    }

    qapi_Coap_Free_Message(server->coapHandle,message);


next_pending_read:

    lwm2m_free(read_observe_pend);
    if(buffer)
      lwm2m_free(buffer);
    if(dataP)
      lwm2m_data_free(size, dataP);
    read_observe_pend = NULL;
    buffer = NULL;
    message = NULL;
    dataP = NULL;
   }
}

void lwm2m_location_fetch()
{
  if(fetch_best_avail_loc)
  {
    lh_location_best_available();
  }
  else
  {
    timer_sleep(200, T_MSEC, 1);
    if(!active_single_shot)
    {
      active_single_shot = true;
      lh_location_single_shot();
    }
    else
    {
      ds_iot_ulog_high("lwm2m_location_fetch: Location single shot session already going on");
    }
  }
}

void lwm2m_handle_location_pmin_update()
{
    lwm2m_observed_t *observedP = NULL;
    lwm2m_context_t *contextP = data.lwm2mH;
    uint32_t location_min_pmin = 0x7FFFFFFF;
    uint32_t location_min_pmax = 0x7FFFFFFF;
    uint32_t last_location_pmin = location_min_interval;

    lwm2m_watcher_t *watcherP;
    lwm2m_uri_t uri;
    uint32_t min_period;
    uint32_t max_period;
	
    observedP = contextP->observedList;
    while(observedP != NULL)
    {
      if(observedP->uri.objectId == LWM2M_LOCATION_OBJECT_ID)
      {
         for(watcherP = observedP->watcherList; watcherP != NULL && (watcherP->active) ; watcherP = watcherP->next)
          {
            min_period = max_period = 0x7FFFFFFF;
            memset(&uri, 0x00, sizeof(lwm2m_uri_t));
            memcpy(&uri, &observedP->uri, sizeof(lwm2m_uri_t));
            if(LWM2M_URI_IS_SET_RESOURCE(&(observedP->uri)))
            {
               // Get the inherited Pmin for watcher if Pmin for current watcher is not set.
               min_period = get_pmin_value(contextP, watcherP, uri);
               if(min_period < location_min_pmin)
                location_min_pmin = min_period;
            }
            else
            {
              max_period = get_pmax_value(contextP, watcherP, uri);
              if(max_period < location_min_pmax)
                location_min_pmax = max_period;
            }
        }
    }
        observedP = observedP->next;
    }

    location_min_interval = ((location_min_pmin < location_min_pmax)?location_min_pmin:location_min_pmax);

    ds_iot_ulog_high1("location_min_interval %d sec", location_min_interval);

    if(last_location_pmin == 0x7fffffff && location_min_interval != 0x7fffffff)
      qurt_signal_set(&loc_update_sem, 0x01);
}

bool lwm2m_is_AppRegister_Allowed(void)
{
  //in case factory reset ongoing, don't allow new App Register
  if(data.glwm2m_app_ctx.lwm2m_app_inited == TRUE && 
  	 factory_reset_req == FALSE && g_quit == 0 && g_reboot == 0 && g_restart == 0)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

void lwm2m_location_thread_start()
{
  qurt_thread_attr_t Thread_Attribute;
  int res = 1;

  if(!location_thread_inited)
  {
    qurt_signal_init(&loc_update_sem);
    qurt_signal_clear(&loc_update_sem, 0x01);
   
    qurt_thread_attr_init(&Thread_Attribute);
    qurt_thread_attr_set_name(&Thread_Attribute, "LWM2M_LOCATION");
    qurt_thread_attr_set_priority(&Thread_Attribute, LWM2M_LOCATION_PRIORITY);
    qurt_thread_attr_set_stack_size(&Thread_Attribute, LWM2M_LOCATION_STACK_SIZE); 
    res = qurt_thread_create(&loc_update_Handle, &Thread_Attribute, lwm2m_location_thread, NULL);    
    if(res != QURT_EOK) 
    {
      ds_iot_ulog_err("lwm2m_location_thread_start: Thread  creation failed");
    }
    location_thread_inited = TRUE;
  }
}

void lwm2m_location_thread_stop()
{
  qurt_signal_clear(&loc_update_sem, 0x01);
  qurt_signal_destroy(&loc_update_sem);
  tx_thread_terminate((TX_THREAD *)loc_update_Handle);
  tx_thread_delete((TX_THREAD *)loc_update_Handle);
  location_thread_inited = 0;
}

void lwm2m_indicate_location_resource_value_change(const char *uriPath)
{
  lwm2m_uri_t uri;
  lwm2m_stringToUri(uriPath, strlen(uriPath), &uri);
  resource_value_change_update(&uri);
}

void lwm2m_location_thread(void *args)
{
  time_t curTime;
  lwm2m_observed_t *observedP = NULL;
  lwm2m_context_t *contextP;
  lwm2m_watcher_t *watcherP;
  lwm2m_uri_t uri;
  uint32_t min_period;
  uint32_t max_period;
  uint32_t mod;

  while(1)
  {
    ds_iot_ulog_high("lwm2m_location_thread: lwm2m_location_thread Blocked because no observations on location");
	qurt_signal_wait(&loc_update_sem, 0x01, QURT_SIGNAL_ATTR_WAIT_ANY | QURT_SIGNAL_ATTR_CLEAR_MASK);
    ds_iot_ulog_high("lwm2m_location_thread: lwm2m_location_thread Unblocked");

LOCATION_OBSERVE:
    curTime = lwm2m_gettime();
    contextP = data.lwm2mH;
	if(contextP == NULL)
    {
      break;
	}
	if(!lwm2m_is_AppRegister_Allowed())
    {
      ds_iot_ulog_high("LwM2M clean up in progress.");
      return;
    }
    observedP = contextP->observedList;
    while(observedP != NULL)
    {
      if(observedP->uri.objectId == LWM2M_LOCATION_OBJECT_ID)
      {
        for(watcherP = observedP->watcherList; watcherP != NULL && watcherP->active; watcherP = watcherP->next)
        {
          uint32_t period = 0;
          memset(&uri, 0x00, sizeof(lwm2m_uri_t));
          memcpy(&uri, &observedP->uri, sizeof(lwm2m_uri_t));

          if(LWM2M_URI_IS_SET_RESOURCE(&(observedP->uri)))
          {
            min_period = get_pmin_value(contextP, watcherP, uri);
            period = min_period;
          }
          else
          {
            max_period = get_pmax_value(contextP, watcherP, uri);
            period = max_period;
          }
          mod = (curTime - watcherP->lastTime)%period;
          if(location_min_interval >= 5 && ((period - mod) > 0) && ((period - mod) <= 5))
          {
            ds_iot_ulog_high1("lwm2m_location_thread: Pmin/Pmax for watcher %p going to expire in next ~5 sec", watcherP);
            if(!active_single_shot)
            {
              active_single_shot = true;
              lh_location_single_shot();
            }
            else
            {
              ds_iot_ulog_high("lwm2m_location_thread: There is already a location single shot session going on");
            }
          }
          if(((period - mod) > 60) && ((period - mod) <= 65) && !trackingSessionId)
          {
            ds_iot_ulog_high1("lwm2m_location_thread: Pmin/Pmax for watcher %p going to expire in next ~65 sec", watcherP);
            if(!active_single_shot)
            {
              active_single_shot = true;
              lh_location_single_shot();
            }
            else
            {
              ds_iot_ulog_high("lwm2m_location_thread: There is already a location single shot session going on");
            }
          }
        }
      }
      observedP = observedP->next;
    }

    if(location_min_interval != 0x7fffffff)
    {
      timer_sleep(1, T_SEC, 1);
      goto LOCATION_OBSERVE;
    }
    else 
      continue;
  }
}

qurt_thread_t get_lwm2m_thread_Handle(void)
{
    return lwm2m_thread_Handle;
}

/**
 * @fn      lwm2m_create_bootstrap_obj()
 * @brief   create object instance for security/server/acl object;
 * 
 *          in QAPI layer, the instance data is organized as link list,
 *          while in object createFunc, the instance data is organized as array, 
 *          this function is to convert the link list to array and then create the object
 *
 * @param[in] lwm2m_object     security/server/acl object
 * @param[in] instance_info    instance to be created;
 *
 * @return  QAPI_OK: success
            other:   failure
 */
qapi_Status_t lwm2m_create_bootstrap_obj(lwm2m_object_t* lwm2m_object, qapi_Net_LWM2M_Instance_Info_v2_t* instance_info)
{
  int result = QAPI_ERROR;  
  lwm2m_data_t* data_array      = NULL;
  qapi_Net_LWM2M_Resource_Info_t *resource_info = NULL;
  uint16_t  resouce_count   = 0;
  uint16_t  objID=0, insID=0;
  uint8_t res = LWM2M_IGNORE;

  if(instance_info == NULL || lwm2m_object == NULL)
  {
    ds_iot_ulog_err("lwm2m_create_bootstrap_obj instance is empty!!");  
    return QAPI_ERROR;
  }

  objID         = lwm2m_object->objID;
  insID         = instance_info->instance_ID;
  resouce_count = instance_info->no_resources;  
  resource_info = instance_info->resource_info;  /**< Pointer to the resource information. */
  
  if(resouce_count == 0)
  {
    ds_iot_ulog_err("lwm2m_create_bootstrap_obj resouce_count=0!!");  
    return QAPI_ERROR;
  }  
  
  if (objID >= LWM2M_MAX_ID || insID >= LWM2M_MAX_ID)
  {
	  LOG_DEBUG("Invalid object/Instance id"); 
	  return QAPI_ERROR; 
  }

  result = lwm2m_parse_application_object_info(instance_info, resource_info, &data_array);
  if(QAPI_OK != result)
  {
    return result;
  }

  if(NULL != LWM2M_LIST_FIND(lwm2m_object->instanceList, insID))
  {
    ds_iot_ulog_err2("object instance /%d/%d already exist!!", objID, insID);
    goto end_handling;
  }
  res = lwm2m_object->createFunc(insID, resouce_count, data_array ,lwm2m_object, TRUE);
  if(res != LWM2M_201_CREATED && (objID == LWM2M_APN_CONN_PROFILE_OBJECT_ID && res != LWM2M_NO_ERROR))
  {
    ds_iot_ulog_err2("create object instance /%d/%d failed", objID, insID);
    goto end_handling;
  }
  
  ds_iot_ulog_high2("create object /%d/%d success", objID, insID);
  result = QAPI_OK;


  end_handling:

  /* Note: here use lwm2m_free not use lwm2m_data_free!!! because we reuse asChidren.array or asBuffer.buffer memory of the input data */
  if(data_array != NULL)
    lwm2m_data_free_ext(resouce_count, data_array); 

  return result;
      
}

qapi_Status_t lwm2m_config_BootstrapInfo(lwm2m_context_t* context, qapi_Net_LWM2M_Data_v2_t* lwm2m_data)
{
  int result = QAPI_ERROR;  
  uint8_t i = 0;
  
  qapi_Net_LWM2M_Data_v2_t* tmp_lwm2m_data = lwm2m_data;
  
  if(context == NULL)
  	return result;
  
  while(tmp_lwm2m_data != NULL)
  {
    lwm2m_object_t  *lwm2m_object = NULL;
	 qapi_Net_LWM2M_Instance_Info_v2_t *tmp_instance_info = NULL; 
	 uint8_t instance_number = 0;

    if(tmp_lwm2m_data->object_ID == LWM2M_SECURITY_OBJECT_ID || 
       tmp_lwm2m_data->object_ID == LWM2M_SERVER_OBJECT_ID || 
       tmp_lwm2m_data->object_ID == LWM2M_ACL_OBJECT_ID || 
       tmp_lwm2m_data->object_ID == LWM2M_APN_CONN_PROFILE_OBJECT_ID)
    {
      lwm2m_object = (lwm2m_object_t *)LWM2M_LIST_FIND(context->objectList, tmp_lwm2m_data->object_ID);
    }
	
	if(lwm2m_object == NULL)
	{
	  LOG_DEBUG("%s lwm2m_object",nullError);    
	  goto end_handling;
	}

    ds_iot_ulog_err2("next object %d(instance:%d)", tmp_lwm2m_data->object_ID, tmp_lwm2m_data->no_instances);	
	
	instance_number   = tmp_lwm2m_data->no_instances;
    tmp_instance_info = tmp_lwm2m_data->instance_info;
	for(i=0; i < instance_number; i++)
	{
      if(tmp_instance_info == NULL)
      {
		ds_iot_ulog_err2("instance number(%d) but No.%d is empty", instance_number, i+1);
        goto end_handling;
      }
	  ds_iot_ulog_err1("instance number(%d)", instance_number);

      if(QAPI_OK != lwm2m_create_bootstrap_obj(lwm2m_object, tmp_instance_info))
      {
        goto end_handling ;
      }
	  
	  //goto next instance;
	  tmp_instance_info = tmp_instance_info->next;
	}

	//goto next object;
    tmp_lwm2m_data = tmp_lwm2m_data->next;
  }

  result = QAPI_OK;

  end_handling: 
  	
  return result;
}

/**
 * @fn      lwm2m_config_client_refresh_server_list()
 * @brief   refresh server list after bootstrap-info loaded/set;
 *          if failure, then cleanup the bootstrap into
 * @param[in] xxObjP     security/server/acl/apn object pointer
 *
 * @return  QAPI_OK: success
            other:   failure
 */
qapi_Status_t lwm2m_config_client_refresh_server_list(
 lwm2m_object_t* secObjP, lwm2m_object_t* serverObjP,
 lwm2m_object_t* aclObjP, lwm2m_object_t* apnObjP)
{
  lwm2m_context_t   *context = data.lwm2mH;
  qapi_Status_t      result  = QAPI_ERROR;

  ds_iot_ulog_high("refresh  server list ");
  //allow no DM server since there maybe bootstrap procedure and consistence check
  object_getServers(context);
  result = object_getBSServer(context);

  if(QAPI_OK == result)
  {
    ds_iot_ulog_high("set_bootstrap_context");
    set_bootstrap_context(LWM2M_TRUE);
    return QAPI_OK;
  }
  else
  {
    ds_iot_ulog_err("server refresh failure, cleanup bootstrap-info!!");
    clean_security_object(secObjP);
    clean_server_object(serverObjP);
#ifdef AUTHORIZATION_SUPPORT
    if(enable_acl_obj)
        clean_acl_ctrl_object(aclObjP);
#endif
#ifdef ENABLE_APN_CONN_OBJ
    if(enable_apn_conn_obj)
      clean_apn_conn_object(apnObjP);
#endif

    efs_unlink(SECURITY_PERSISTENCE_FILE);
    efs_unlink(SERVER_PERSISTENCE_FILE);
    efs_unlink(ACL_PERSISTENCE_FILE);
    efs_unlink(LWM2M_APN_CONN_PERSISTENCE_FILE);
    return QAPI_ERROR;
  }
}

qapi_Status_t lwm2m_start_byApp(void)
{
   lwm2m_object_t    *secObjP    = NULL;
   lwm2m_object_t    *serverObjP = NULL;
   lwm2m_object_t    *aclObjP    = NULL;
   lwm2m_object_t    *apnObjP    = NULL;
   lwm2m_context_t   *context    = data.lwm2mH;
   qapi_Status_t result = QAPI_ERROR;

   if(context == NULL) return QAPI_ERR_NOT_INITIALIZED;

   secObjP     = (lwm2m_object_t *)LWM2M_LIST_FIND(context->objectList, LWM2M_SECURITY_OBJECT_ID);
   serverObjP  = (lwm2m_object_t *)LWM2M_LIST_FIND(context->objectList, LWM2M_SERVER_OBJECT_ID);
   aclObjP     = (lwm2m_object_t *)LWM2M_LIST_FIND(context->objectList, LWM2M_ACL_OBJECT_ID);
   apnObjP     = (lwm2m_object_t *)LWM2M_LIST_FIND(context->objectList, LWM2M_APN_CONN_PROFILE_OBJECT_ID);

   if(secObjP == NULL || serverObjP == NULL || aclObjP == NULL || apnObjP == NULL)
   {
	 ds_iot_ulog_err("LwM2M client not initilized");
	 return QAPI_ERR_NOT_INITIALIZED;
   } 

   if(isLwm2mStartedByApp == true )
   {
       ds_iot_ulog_err("LwM2M already started");
	   return QAPI_ERR_INVALID_STATE;
   }
   if(LWM2M_LIST_COUNT(secObjP->instanceList) == 0)
   {
       ds_iot_ulog_high("start LwM2M without app-configure bootstrap-info, load from bootstrap.ini now!!");
       if (LWM2M_FALSE ==  get_bootstrap_info(secObjP, serverObjP, aclObjP, apnObjP)) 
       {
         ds_iot_ulog_err("start LwM2M with bootstrap.ini failure");
         return QAPI_ERR_NO_ENTRY;
       }
       if(QAPI_OK != lwm2m_config_client_refresh_server_list(secObjP, serverObjP, aclObjP, apnObjP))
       {
         return QAPI_ERR_INVALID_PARAM; /* bootstrap create failure should due to wrong data from QAPI*/
       }
   }
  
   isLwm2mStartedByApp = true;

   result = QAPI_OK;
   ds_iot_ulog_high("Start LwM2M");	 
	 
   return result;
}

qapi_Status_t lwm2m_config_client(qapi_Net_LWM2M_Config_Data_t* config)
{  
  lwm2m_context_t   *context = data.lwm2mH;
  qapi_Status_t result = QAPI_ERROR;
  lwm2m_object_t    *secObjP    = NULL;
  lwm2m_object_t    *serverObjP = NULL;
  lwm2m_object_t    *aclObjP    = NULL;
  lwm2m_object_t    *apnObjP    = NULL;

  if(context == NULL)
    return QAPI_ERR_NOT_INITIALIZED;

  secObjP     = (lwm2m_object_t *)LWM2M_LIST_FIND(context->objectList, LWM2M_SECURITY_OBJECT_ID);
  serverObjP  = (lwm2m_object_t *)LWM2M_LIST_FIND(context->objectList, LWM2M_SERVER_OBJECT_ID);
  aclObjP     = (lwm2m_object_t *)LWM2M_LIST_FIND(context->objectList, LWM2M_ACL_OBJECT_ID);
  apnObjP     = (lwm2m_object_t *)LWM2M_LIST_FIND(context->objectList, LWM2M_APN_CONN_PROFILE_OBJECT_ID);
  
  if(NULL == context || NULL == secObjP || NULL == config || apnObjP == NULL)
  {
	ds_iot_ulog_err("LwM2M client not initilized");
	return QAPI_ERR_NOT_INITIALIZED;
  }
  
  if(LWM2M_STARTUP_BOOTSTRAP_BYAPP != data.cfg_info.lwm2m_startup_mode)
  {
	ds_iot_ulog_err1("Startup Mode is not ByApp(Mode=%d), Reject BootStrap Config", data.cfg_info.lwm2m_startup_mode);
    return QAPI_ERR_NOT_SUPPORTED;
  }
  
  switch (config->config_type)
  {
    case QAPI_NET_LWM2M_CONFIG_BOOTSTRAP_INFO:
    {
      if(LWM2M_LIST_COUNT(secObjP->instanceList) != 0)
      {
        ds_iot_ulog_err("Can't configure LwM2M with new configutation without factory reset the old ones");
        return QAPI_ERR_EXISTS;
      }
  	  
      result = lwm2m_config_BootstrapInfo(context, config->value.lwm2m_data);
      if(QAPI_OK == result)
      { 
        result = lwm2m_config_client_refresh_server_list(secObjP, serverObjP, aclObjP, apnObjP);
      }
  
      if(QAPI_OK != result)
      {
        ds_iot_ulog_high1("lwm2m_config_BootstrapInfo or object_getServers return failure %d", result);
        return QAPI_ERR_INVALID_PARAM; /* bootstrap create failure should due to wrong data from QAPI*/		
      }	  
	  break;
   }
   case QAPI_NET_LWM2M_START:
   {
   	 result = lwm2m_start_byApp();

	 if(result == QAPI_OK) 
       qurt_signal_set(&lwm2m_signal_Handle, LWM2M_BOOTSTRAP_ONDEMAND_IND);
	 break;	
   }
   case QAPI_NET_LWM2M_DEREGISTER_AND_STOP:
   	 //trigger de-register, then continue restart lwm2m
   	 lwm2m_deregister(context); 
	 /*continue without break*/
   case QAPI_NET_LWM2M_STOP:
     result = QAPI_OK;
     qurt_signal_set(&lwm2m_signal_Handle, LWM2M_RESTART);
	 break;
   case QAPI_NET_LWM2M_FACTORY_RESET:
   {	  
     result = QAPI_OK;
     qurt_signal_set(&lwm2m_signal_Handle, LWM2M_FACTORY_RESET_SET);
	 break;
   }
   default:
   {
     result = QAPI_ERR_NOT_SUPPORTED;
     ds_iot_ulog_err1("Not supported config type:%d", config->config_type);	
	 break;
   }
  }
  return result;
}

qapi_Device_Info_Hndl_t get_lwm2m_dev_info_hdl(void)
{
	return lwm2m_dev_info_hdl;
}



