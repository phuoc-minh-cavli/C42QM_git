/*!
  @file
  qcmap_qti.c

  @brief
  Mobile AP functionalities implementation
*/

/*===========================================================================

  Copyright (c) 2017 - 2023 Qualcomm Technologies, Inc. All Rights Reserved

  Qualcomm Technologies Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

===========================================================================*/


#include "qcmap.h"
#include "common_v01.h"
#include "network_access_service_v01.h"
#include "qcmap_api.h"
#include "qmi_client.h"
#include "qurt_signal.h"
#include "fs_public.h"
#include "fs_errno.h"
#include "data_utils_common.h"
#include "qapi_socket.h"
#include "qapi_ssl.h"
#include "qapi.h"
#include "qapi_netservices.h"
#include "qcmap_port.h"

#ifdef MOB_TX
#include "offtarget_stubs.h"
#endif

#define QCMAP_V4_DEFAULT_DEVICE_NAME "rmnet_data0"
#define QCMAP_V6_DEFAULT_DEVICE_NAME "rmnet_data0"
#define AUTO_CONNECT_TIMER 1
#define DSS_PROFILE_3GPP2_OFFSET  1000
#define QCMAP_CONFIG_FILE        "/datatx/qcmap_config"

static boolean read_file=false;
unsigned int modem_loopback_mode = 0 ;
qcmap_connection_manager *qc_mgr;
qcmap_client_info_type qcmap_client_info[QCMAP_MAX_CLIENT_HANDLES];
qcmap_softap_handle_type qcmap_handle;
qcmap_backhaul_mgr *qc_bck;
qcmap_backhaul_wwan *qc_bck_wan;
client_data_s *client_data_t;
extern qcmap_file qcmap_file_t;
mobile_ap_resp mobile_ap_qti_type;

extern void qcmap_set_v4_dns_server_added( boolean state);
extern void qcmap_set_v6_dns_server_added( boolean state);

extern void nat_init_params(int nat_en, int l_if, int i_if);
extern void nat_disable(void);
extern int _dhs_init_from_config_file(ip_addr ipaddr,ip_addr snmask,ip_addr start_ip, ip_addr end_ip, uint32_t lease);
extern void set_nat_tcp_timeout(unsigned short tcp_tmo);
extern void set_nat_udp_timeout(unsigned short udp_tmo);

extern void ip6_set_req_params(void);
extern void ip6_clear_req_params(void);
extern void ip6_clear_addresses(void);
extern boolean std_ecm_enabled;
extern boolean qcmap_v6_connected;
extern boolean   stdecm_publicIP;
extern uint16_t stdecm_apps_port_start;
extern uint16_t stdecm_apps_port_range;
extern boolean AppsSourceFilterEnabled;

typedef  struct net *NET;
extern NET if_getbyname(const char *);
extern int nd_rtsolicit(NET);

extern int ns_is_valid_ip_address(int af, void *ipaddr);

extern void et_inittimer(void);

extern void ecm_set_mac(uint8 hwaddr[]);

extern void nat_enable_dmz(unsigned long dmz_addr, boolean enable);

extern qcmap_ip qcmap_ip_t; 
extern qcmap_file qcmap_file_t;
extern int nd_rtadvert_deprecated(NET);
boolean qti_client = FALSE;
extern void* ds_memcpy(void *d, const void *s, size_t n);

extern void dhs_enable(void);
extern void dhs_disable(void);
extern void qcmap_set_dhcp_public_address_range(int leasetime);
extern void dhs_add_dns_server(ip_addr addr);

struct qcmap_event_strings_s
{
  qapi_DSS_Net_Evt_t evt;
  char * str;
};

struct qcmap_event_strings_s qcmap_event_string_tbl[QAPI_DSS_EVT_MAX_E] =
{
  { QAPI_DSS_EVT_INVALID_E, " INVALID" },
  { QAPI_DSS_EVT_NET_IS_CONN_E, "NET_IS_CONN" },
  { QAPI_DSS_EVT_NET_NO_NET_E, "NET_NO_NET" },
  { QAPI_DSS_EVT_NET_RECONFIGURED_E, "NET_RECONFIGURED" },
  { QAPI_DSS_EVT_NET_NEWADDR_E, "NET_NEWADDR" },
  { QAPI_DSS_EVT_NET_DELADDR_E, "NET_DELADDR" }
};

void qcmap_dss_net_cb_fcn( qapi_DSS_Hndl_t hndl,
                     void * user_data,
                     qapi_DSS_Net_Evt_t evt,
                     qapi_DSS_Evt_Payload_t *payload_ptr )
{

  qcmap_cmd_t*             cmd_ptr = NULL;
  qcmap_dss_buffer_t       *qcmap_dss_buffer;

  qcmap_dss_buffer = (qcmap_dss_buffer_t *)malloc(sizeof(qcmap_dss_buffer_t));

  if(qcmap_dss_buffer == NULL)
  {
    QCMAP_LOG_MSG_ERROR("Cannot assign memory for dss buffer Assert", 0, 0, 0);
    ASSERT(0);
  }

  memset(qcmap_dss_buffer, 0, sizeof(qcmap_dss_buffer_t));

  qcmap_dss_buffer->dss_nethandle = hndl;
  qcmap_dss_buffer->user_data = user_data;
  qcmap_dss_buffer->evt = evt;
  qcmap_dss_buffer->payload_ptr = payload_ptr;

  QCMAP_LOG_MSG_HIGH ("Entering: qcmap_dss_net_cb_fcn ", 0, 0, 0 );

  if((cmd_ptr = qcmap_get_cmd_buffer()) == NULL)
  {
    QCMAP_LOG_MSG_ERROR("Cannot obtain command buffer. Assert", 0, 0, 0);
    ASSERT(0);
  }
  else
  {
    cmd_ptr->cmd_hdr.cmd_id              = QCMAP_DSS_IND_EVT;
    cmd_ptr->cmd_data.data               = (void *)qcmap_dss_buffer;

    qcmap_enqueue_cmd(cmd_ptr);
  }

  qcmap_set_signal();
  return;

}

/*===========================================================================

FUNCTION QCMAP_GET_DEV_NAME()

DESCRIPTION
  This Function get RMNET device name.

DEPENDENCIES
  None.

RETURN VALUE
  On success, returns QCMAP_SUCCESS.
  On error, return QCMAP_ERROR and places the error condition value in
  *qcmap_errno.

SIDE EFFECTS

===========================================================================*/
int qcmap_get_dev_name
(
  int     qcmap_handle,                     /* Handle for Mobile AP CM  */
  qcmap_msgr_ip_family_enum_v01
          qcmap_dev_type,                      /* Dev type                 */
  char   *qcmap_dev_name,                      /* Device name              */
  int    *qcmap_errno                       /* Error condition value    */
)
{
  char device[QAPI_DSS_CALL_INFO_DEVICE_NAME_MAX_LEN + 2];
  int rval;
  ds_assert(qcmap_dev_name != NULL);
  ds_assert(qcmap_errno != NULL);

  QCMAP_LOG_MSG_MED("qcmap_get_dev_name: enter",0,0,0);

  memset(device,0,sizeof(device));
  if (qcmap_handle != qcmap_cb.handle)
  {
    *qcmap_errno = QCMAP_EBADAPP;
    QCMAP_LOG_MSG_ERROR( "Wrong QCMAP CM Handle, qcmap_cm_handle:%d, qcmap_cm_cb.handle:%d",qcmap_handle, qcmap_cb.handle,0);
    return QCMAP_ERROR;
  }
  if (qcmap_dev_type == QCMAP_MSGR_IP_FAMILY_V4_V01)
  {
    if (qcmap_cb.state <= QCMAP_ENABLE)
    {
      *qcmap_errno = QCMAP_EOPNOTSUPP;

      QCMAP_LOG_MSG_ERROR( "QCMAP CM V4 WAN not connected. %d", qcmap_cb.state,0,0);
      return QCMAP_ERROR;
    }
    else if (qcmap_cb.dss_net_hndl.handle != NULL)
    {
      rval = qapi_DSS_Get_Device_Name((qapi_DSS_Hndl_t)qcmap_cb.dss_net_hndl.handle, 
	  	device, QAPI_DSS_CALL_INFO_DEVICE_NAME_MAX_LEN+2);
      if(rval != QAPI_DSS_SUCCESS)
      {
        QCMAP_LOG_MSG_ERROR ("Couldn't get ipv4 rmnet name. rval %d . Using saved device",rval, 0, 0);
        memcpy(device, qcmap_cb.dss_device_name, QAPI_DSS_CALL_INFO_DEVICE_NAME_MAX_LEN+2);
        *qcmap_errno = rval;
      }
      memcpy(qcmap_dev_name, device, QAPI_DSS_CALL_INFO_DEVICE_NAME_MAX_LEN+2);
    }else
    {
      *qcmap_errno = QCMAP_EOPNOTSUPP;
      QCMAP_LOG_MSG_ERROR("Error during get ipv4 dev_name state =%d, handle=%d ",
                     qcmap_cb.state,qcmap_cb.dss_net_hndl.handle,0);
      return QCMAP_ERROR;
    }
  }
  else if (qcmap_dev_type == QCMAP_MSGR_IP_FAMILY_V6_V01)
  {
    if (qcmap_cb.ipv6_state <= QCMAP_V6_ENABLE)
    {
      *qcmap_errno = QCMAP_EOPNOTSUPP;
      QCMAP_LOG_MSG_ERROR( "QCMAP V6 WAN not connected. %d", qcmap_cb.ipv6_state,0,0);
      return QCMAP_ERROR;
    }
    else if ( qcmap_cb.ipv6_dss_net_hndl.handle != NULL)
    {
      rval = qapi_DSS_Get_Device_Name((qapi_DSS_Hndl_t)qcmap_cb.ipv6_dss_net_hndl.handle, device, QAPI_DSS_CALL_INFO_DEVICE_NAME_MAX_LEN+2);
      if(rval != QAPI_DSS_SUCCESS)
      {
        QCMAP_LOG_MSG_ERROR ("Couldn't get ipv6 rmnet name. rval %d . Using saved device",rval, 0, 0);
        memcpy(device, qcmap_cb.ipv6_dss_device_name, QAPI_DSS_CALL_INFO_DEVICE_NAME_MAX_LEN+2);
        *qcmap_errno = rval;
      }

      memcpy(qcmap_dev_name, device, QAPI_DSS_CALL_INFO_DEVICE_NAME_MAX_LEN+2);
    }else
    {
      *qcmap_errno = QCMAP_EOPNOTSUPP;
      QCMAP_LOG_MSG_ERROR("Error during get ipv6 dev_name state =%d, handle=%d ",
                     qcmap_cb.ipv6_state,qcmap_cb.ipv6_dss_net_hndl.handle,0);
      return QCMAP_ERROR;
    }
  }
  QCMAP_LOG_MSG_MED_SPRINT( "qcmap_get_dev_name:: %s", qcmap_dev_name,0,0);
  return QCMAP_SUCCESS;
}

/*=====================================================
  FUNCTION QCMAP_ConnectionManager_callback
======================================================*/
/*!
@brief
  Handles the mobile ap events and invokes the callback function
  for the respective indications to be sent.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*====================================================*/
void qcmap_connectionmanager_callback
(
  int              handle,                   /* MobileAP Application id */
  qcmap_event_e    event,                    /* Type of MobileAP Event  */
  void             *qcmap_msgr_cb_user_data, /* Call back user data     */
  qapi_DSS_CE_Reason_t  *callend_reason
)
{
  qcmap_status_cb_data *cbPtr;
  qcmap_softap_handle_type *softApHandle =
                  (qcmap_softap_handle_type *) qcmap_msgr_cb_user_data;

  qcmap_msgr_wwan_call_end_type_enum_v01 call_end_type =
                                      QCMAP_MSGR_WWAN_CALL_END_TYPE_INVALID_V01;
  int call_end_reason_code=0;
  char devname[QAPI_DSS_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  int qcmap_error;

  if (softApHandle != NULL)
  {
    cbPtr = (qcmap_status_cb_data *) softApHandle->cb_ptr;
  }
  else
  {
     return;
  }

  QCMAP_LOG_MSG_MED("QCMAP AP Handle %04X Event %04X", handle, event, 0);

  memset ( devname, 0, sizeof(devname));

  /*If connect to backhaul failed then callend_reason would be filled
    with reason and call type, same is sent to clients in failure response*/
  if( callend_reason != NULL )
  {
    if( callend_reason->reason_type == QAPI_DSS_CE_TYPE_MOBILE_IP_E )
      call_end_type = QCMAP_MSGR_WWAN_CALL_END_TYPE_MOBILE_IP_V01;
    else if( callend_reason->reason_type == QAPI_DSS_CE_TYPE_INTERNAL_E )
      call_end_type = QCMAP_MSGR_WWAN_CALL_END_TYPE_INTERNAL_V01;
    else if( callend_reason->reason_type == QAPI_DSS_CE_TYPE_CALL_MANAGER_DEFINED_E )
      call_end_type = QCMAP_MSGR_WWAN_CALL_END_TYPE_CALL_MANAGER_DEFINED_V01;
    else if( callend_reason->reason_type == QAPI_DSS_CE_TYPE_3GPP_SPEC_DEFINED_E )
      call_end_type = QCMAP_MSGR_WWAN_CALL_END_TYPE_3GPP_SPEC_DEFINED_V01;
    else if( callend_reason->reason_type == QAPI_DSS_CE_TYPE_PPP_E )
      call_end_type = QCMAP_MSGR_WWAN_CALL_END_TYPE_PPP_V01;
    else if( callend_reason->reason_type == QAPI_DSS_CE_TYPE_EHRPD_E )
      call_end_type = QCMAP_MSGR_WWAN_CALL_END_TYPE_EHRPD_V01;
    else if( callend_reason->reason_type == QAPI_DSS_CE_TYPE_IPV6_E )
      call_end_type = QCMAP_MSGR_WWAN_CALL_END_TYPE_IPV6_V01;
    call_end_reason_code = callend_reason->reason_code;
  }

  switch ( event )
  {
  case QCMAP_EVENT_ENABLED:
    if (cbPtr && cbPtr->qcmap_cb)
    {
      cbPtr->qcmap_cb(cbPtr, QCMAP_MSGR_MOBILE_AP_STATUS_CONNECTED_V01);
    }
    break;
  case QCMAP_EVENT_WAN_CONNECTING:
    break;
  case QCMAP_EVENT_WAN_CONNECTING_FAIL:
    if (cbPtr && cbPtr->wwan_cb)
    {
     cbPtr->wwan_cb(cbPtr, QCMAP_MSGR_WWAN_STATUS_CONNECTING_FAIL_V01,
                     call_end_type,call_end_reason_code);
    }
    break;
    case QCMAP_EVENT_WAN_IPv6_CONNECTING_FAIL:
    if (cbPtr && cbPtr->wwan_cb)
    {
      cbPtr->wwan_cb(cbPtr, QCMAP_MSGR_WWAN_STATUS_IPV6_CONNECTING_FAIL_V01,
                     call_end_type,call_end_reason_code);
    }
    break;
  case QCMAP_EVENT_WAN_CONNECTED:

    if ( qcmap_get_dev_name(qc_mgr->qcmap_cm_handle,
                              QCMAP_MSGR_IP_FAMILY_V4_V01,
                              devname, &qcmap_error) != QCMAP_SUCCESS )
    {
      QCMAP_LOG_MSG_ERROR("Couldn't get rmnet name. error %d", 0,0,0);
    }
    if(stdecm_publicIP)
    {
        uint8_t cmd = TRUE;
        sio_ioctl_param_type ioctl_param;
        ioctl_param.link_up = 1;
        sio_control_ioctl(qc_mgr->qcmap_ph_stream_id, SIO_IOCTL_NETWORK_CONNECTION_NOTIF, &ioctl_param);
        qcmap_set_dhcp_public_address_range(qcmap_file_t.dhcp_lease);
        set_dhcp_command(cmd);
    }
    else
    {
        qcmap_enable_nat_on_apps();
    }

    if (cbPtr && cbPtr->wwan_cb)
    {
      cbPtr->wwan_cb(cbPtr, QCMAP_MSGR_WWAN_STATUS_CONNECTED_V01,
                     call_end_type,call_end_reason_code);
    }
    break;
  case QCMAP_EVENT_WAN_IPv6_CONNECTED:

    qcmap_v6_connected = TRUE;
    if(std_ecm_enabled)
    {
      NET ifp;
      char *ifp_name = QCMAP_V6_DEFAULT_DEVICE_NAME;

      ifp = if_getbyname(ifp_name);

      if(ifp)
      {
        nd_rtsolicit(ifp);
      }
    }

    if ( qcmap_get_dev_name(qc_mgr->qcmap_cm_handle,
                              QCMAP_MSGR_IP_FAMILY_V6_V01,
                              devname, &qcmap_error) != QCMAP_SUCCESS )
    {
      QCMAP_LOG_MSG_ERROR("Couldn't get rmnet name. error %d", 0,0,0);
    }


    if (cbPtr && cbPtr->wwan_cb)
    {
      cbPtr->wwan_cb(cbPtr, QCMAP_MSGR_WWAN_STATUS_IPV6_CONNECTED_V01,
                     call_end_type,call_end_reason_code);
    }
    break;
  case QCMAP_EVENT_WAN_DISCONNECTED:
    if (cbPtr && cbPtr->wwan_cb)
    {
      cbPtr->wwan_cb(cbPtr, QCMAP_MSGR_WWAN_STATUS_DISCONNECTED_V01,
                     call_end_type,call_end_reason_code);
    }
    if(stdecm_publicIP)
    {
        uint8_t cmd = FALSE;
        sio_ioctl_param_type ioctl_param;
        ioctl_param.link_up = 0;
        sio_control_ioctl(qc_mgr->qcmap_ph_stream_id, SIO_IOCTL_NETWORK_CONNECTION_NOTIF, &ioctl_param);
        set_dhcp_command(cmd);
    }
    else
    {
        qcmap_disable_nat_on_apps();
    }
    break;

  case QCMAP_EVENT_WAN_IPv6_DISCONNECTED:

    qcmap_v6_connected = FALSE;
    if(std_ecm_enabled)
    {
      NET ifp;
      char *ifp_name = "ecm5";

      ifp = if_getbyname(ifp_name);

      if(ifp)
      {
        nd_rtadvert_deprecated(ifp);    /* Sends out the deprecated RA packet */
      }
    }

    ip6_clear_addresses();

    if (cbPtr && cbPtr->wwan_cb)
    {
      cbPtr->wwan_cb(cbPtr, QCMAP_MSGR_WWAN_STATUS_IPV6_DISCONNECTED_V01,
                     call_end_type,call_end_reason_code);
    }
  break;
  case QCMAP_EVENT_WAN_ADDR_RECONF:

    if(stdecm_publicIP)
    {
        sio_ioctl_param_type ioctl_param_disc, ioctl_param_conn;
        ioctl_param_disc.link_up = 0;
        sio_control_ioctl(qc_mgr->qcmap_ph_stream_id, SIO_IOCTL_NETWORK_CONNECTION_NOTIF, &ioctl_param_disc);
        ioctl_param_conn.link_up = 1;
        sio_control_ioctl(qc_mgr->qcmap_ph_stream_id, SIO_IOCTL_NETWORK_CONNECTION_NOTIF, &ioctl_param_conn);
    }

  break;
  case QCMAP_EVENT_WAN_IPv6_ADDR_RECONF:
  break;
  case QCMAP_EVENT_DISABLED:
    if (cbPtr && cbPtr->qcmap_cb)
    {
      cbPtr->qcmap_cb(cbPtr, QCMAP_MSGR_MOBILE_AP_STATUS_DISCONNECTED_V01);
    }
    qurt_mutex_lock(&qc_mgr->cm_mutex);
    qc_mgr->qcmap_tear_down_in_progress = false;
    DisableHandle();
    qurt_mutex_unlock(&qc_mgr->cm_mutex);
    QCMAP_LOG_MSG_MED("Teardown flag=false",0,0,0);
    break;
  default:
    QCMAP_LOG_MSG_MED("QCMAP_EVENT_UNKNOWN %08X", event,0,0);
    break;
  }

  return;
}

/*================================================================

FUNCTION QCMAP_READ_DEFAULT_CONFIG_FILE

DESCRIPTION
  Default config file in case of missing or corrupted EFS file.
  
DEPENDENCIES
  None.

RETURN VALUE
  Return SUCCESS if we are able to create new file and write data to it

SIDE EFFECTS
  None

==================================================================*/

int qcmap_read_default_config_file(void)
{

  int fp = INVALID_FILE_HANDLE;
  int dir_ret = INVALID_FILE_HANDLE;
  char  str[128];
  int ret = QCMAP_FAILURE;

  dir_ret = efs_mkdir("/datatx/", 0777);
  if ((dir_ret < 0) && (efs_errno != EEXIST))
  {
    QCMAP_LOG_MSG_ERROR("Unable to create dir /datatx/ in Alt EFS ",
                        0, 0, 0);
    return QCMAP_FAILURE;
  }

  fp = efs_open(QCMAP_CONFIG_FILE,O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
  if (fp < 0)
  {
    QCMAP_LOG_MSG_ERROR("Error in opening file to write default config ",
                        0, 0, 0);
    return QCMAP_FAILURE;
  }

  if ( fp != INVALID_FILE_HANDLE )
  {
    snprintf(str, sizeof(str), "%s=%s\n", "Tech", "Lte");
    efs_write( fp , str, strlen(str));
    snprintf(str, sizeof(str), "%s=%d\n", "Profile", 1);
    efs_write( fp , str, strlen(str));
    snprintf(str, sizeof(str), "%s=%s\n", "Gateway_IP", "192.168.225.1");
    efs_write( fp , str, strlen(str));
    snprintf(str, sizeof(str), "%s=%s\n", "Subnet", "255.255.255.0");
    efs_write( fp , str, strlen(str));
    snprintf(str, sizeof(str), "%s=%s\n", "DHCP_Start", "192.168.225.20");
    efs_write( fp , str, strlen(str));
    snprintf(str, sizeof(str), "%s=%s\n", "DHCP_End", "192.168.225.60");
    efs_write( fp , str, strlen(str));
    snprintf(str, sizeof(str), "%s=%d\n", "DHCP_Lease", 43200);
    efs_write( fp , str, strlen(str));
    snprintf(str, sizeof(str), "%s=%d\n", "Autoconnect", 1);
    efs_write( fp , str, strlen(str));
    snprintf(str, sizeof(str), "%s=%d\n", "Roaming", 0);
    efs_write( fp , str, strlen(str));
    snprintf(str, sizeof(str), "%s=%d\n", "DHCP_Enable", 1);
    efs_write( fp , str, strlen(str));
    snprintf(str, sizeof(str), "%s=%d\n", "STDECM_PublicIP", 0);
    efs_write( fp , str, strlen(str));
    snprintf(str, sizeof(str), "%s=%d\n", "APPS_Port_Start", 5000);
    efs_write( fp , str, strlen(str));
    snprintf(str, sizeof(str), "%s=%d\n", "APPS_Port_Range", 50);
    efs_write( fp , str, strlen(str));
    snprintf(str, sizeof(str), "%s=%d\n", "NAT_TCP_Timeout", 0);
    efs_write( fp , str, strlen(str));
    snprintf(str, sizeof(str), "%s=%d\n", "NAT_UDP_Timeout", 0);
    efs_write( fp , str, strlen(str));
    efs_close(fp);
    fp = INVALID_FILE_HANDLE;
  }

  ret = qcmap_read_config_file();

  if(ret == QCMAP_SUCCESS)
  {
    QCMAP_LOG_MSG_HIGH("File read successful ", 0, 0, 0);
    return QCMAP_SUCCESS;
  }
  else
  {
    QCMAP_LOG_MSG_ERROR("File read un-successful ", 0, 0, 0);
    return QCMAP_FAILURE;
  }
}

/*=============================================================================

FUNCTION QCMAP_CLIENT_REG_CB

DESCRIPTION
  Callback Exposed for QCMAP Clients to register a Function which will be used
  for QCMAP Task -> Client Communication
DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None

==============================================================================*/
void qcmap_client_reg_cb
(
  qcmap_client_resp_ind_cb cb_ptr,
  uint8                    *connection_handle
)
{

  int idx = 0;
  if(read_file == false)
  {
    if(qcmap_read_config_file() == QCMAP_FAILURE)
    {
      if (qcmap_read_default_config_file() == QCMAP_FAILURE)
      {
        QCMAP_LOG_MSG_ERROR("Unable to update default config file ", 0, 0, 0);
        return;
      }
    }

    if((qcmap_file_t.stdecm_publicIP == 1) && (AppsSourceFilterEnabled == FALSE))
    {
      qcmap_file_t.dhcp_enable = 0;
      qcmap_file_t.autoconnect = 1;
      stdecm_publicIP = TRUE;
      stdecm_apps_port_start = qcmap_file_t.apps_port_start;
      stdecm_apps_port_range = qcmap_file_t.apps_port_range;
    }
    else
    {
      if(qcmap_set_dhcp_address_range() == QCMAP_DHCP_ASSIGN_FAIL)
      {
        QCMAP_LOG_MSG_ERROR("QCMAP_DHCP_ASSIGN_FAIL ", 0, 0, 0);
        return;
      }
    }

    set_dhcp_command(qcmap_file_t.dhcp_enable);
    read_file = true;
  }

  qc_mgr->auto_connect = qcmap_file_t.autoconnect;
  qc_bck_wan->wwan_cfg.roaming = qcmap_file_t.roaming;

  QCMAP_LOG_MSG_HIGH("Entering: qcmap_client_reg_cb", 0, 0, 0);
  if (cb_ptr ==  NULL)
  {
    QCMAP_LOG_MSG_MED("Invalid callback function from client. Please check", 0, 0, 0);
    return;
  }

  for (idx = 0; idx < QCMAP_MAX_CLIENT_HANDLES; idx++)
  {
    if (qc_mgr->cb_arr[idx] == NULL)
    {
      QCMAP_LOG_MSG_MED("Found free index ", 0, 0, 0);
      qc_mgr->cb_arr[idx] = cb_ptr;
      break;
    }
  }

  if (idx  == QCMAP_MAX_CLIENT_HANDLES)
  {
    QCMAP_LOG_MSG_ERROR("MAX QCMAP CLIENTS REACHED ", 0, 0, 0);
    return;
  }

  *connection_handle = (uint8)idx;
  return;
}


/*=============================================================================

FUNCTION QTI_QCMAP_CB

DESCRIPTION
  Callback Exposed for QTI to send the peripheral stream ID


DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None

==============================================================================*/
void qti_qcmap_cb
(
  sio_stream_id_type          qti_ph_stream_id
)
{
  qcmap_cmd_t*             cmd_ptr = NULL;

  QCMAP_LOG_MSG_HIGH ("Entering: qti_qcmap_cb ", 0, 0, 0 );

  if((cmd_ptr = qcmap_get_cmd_buffer()) == NULL)
  {
    QCMAP_LOG_MSG_ERROR("Cannot obtain command buffer. Assert", 0, 0, 0);
    ASSERT(0);
  }
  else
  {
    cmd_ptr->cmd_hdr.cmd_id              = QCMAP_QTI_EVT;
    cmd_ptr->cmd_data.connection_handle  = (uint8)qti_ph_stream_id;

    qcmap_enqueue_cmd(cmd_ptr);
  }

  qcmap_set_signal();
  return;
}

/*=============================================================================

FUNCTION QCMAP_CLIENT_REG_CB

DESCRIPTION
  Callback Exposed for QCMAP Clients to send various event for MobileAP 
  functionality

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None

==============================================================================*/
void qcmap_evt_handler_cb
(
  qcmap_cmd_id_e evt,
  void * data,
  uint8  connection_handle
)
{
  qcmap_cmd_t*             cmd_ptr = NULL;

  QCMAP_LOG_MSG_HIGH ("qcmap_evt_handler_cb, evt:%d ", evt, 0, 0 );

  if((cmd_ptr = qcmap_get_cmd_buffer()) == NULL)
  {
    QCMAP_LOG_MSG_ERROR("Cannot obtain command buffer. Assert", 0, 0, 0);
    ASSERT(0);
  }
  else
  {
    cmd_ptr->cmd_hdr.cmd_id              = evt;
    cmd_ptr->cmd_data.data               = data;
    cmd_ptr->cmd_data.connection_handle  = connection_handle;

    qcmap_enqueue_cmd(cmd_ptr);
  }

  qcmap_set_signal();
  return;
}


/*===========================================================================
  FUNCTION QCMAP_WWAN_STATUS_IND()

  DESCRIPTION
    Dispatches a unicast indication to notify registered QCMobileAP client
      about WWAN status.

  RETURN VALUE
    None

  DEPENDENCIES
    qcmap_mobile_ap_enable() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
void qcmap_wwan_status_ind
(
  void                           *user_data,
  qcmap_msgr_wwan_status_enum_v01  conn_status,
  qcmap_msgr_wwan_call_end_type_enum_v01 call_end_type,
  int call_end_reason_code
)
{

  qcmap_client_info_type      *qcmap_cp;
  qcmap_msgr_wwan_status_ind_msg_v01     *ind_msg;
  qcmap_status_cb_data   *cb_data;
  unsigned int                      client_index=0;
  union
  {
    qcmap_msgr_bring_up_wwan_ind_msg_v01   bring_up_ind;
    qcmap_msgr_tear_down_wwan_ind_msg_v01  tear_down_ind;
  }*wwan_op_ind_msg;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(user_data != NULL);
  QCMAP_LOG_MSG_MED(" qcmap_wwan_status_ind ", 0, 0, 0);
  cb_data = (qcmap_status_cb_data*)user_data;
  wwan_op_ind_msg = malloc(sizeof(qcmap_msgr_bring_up_wwan_ind_msg_v01));

  if (wwan_op_ind_msg == NULL)
  {
    QCMAP_LOG_MSG_ERROR("Out of mem for WWAN Ind msg", 0, 0, 0);
    ASSERT(0);
  }

  ind_msg          = (qcmap_msgr_wwan_status_ind_msg_v01 *)malloc(sizeof(qcmap_msgr_wwan_status_ind_msg_v01));

  if (ind_msg == NULL)
  {
    QCMAP_LOG_MSG_ERROR("Out of mem for WWAN Ind status msg", 0, 0, 0);
    ASSERT(0);
  }

  qcmap_cp = (qcmap_client_info_type * )malloc(sizeof(qcmap_client_info_type));

  if (qcmap_cp == NULL)
  {
    QCMAP_LOG_MSG_ERROR("Out of mem for QCMAP client info ", 0, 0, 0);
    ASSERT(0);
  }

  *qcmap_cp = (qcmap_client_info[0]);

  //go through all the handles and see which client is waiting for wwan status indication
  for (client_index=0;client_index < QCMAP_MAX_CLIENT_HANDLES; client_index++)
  {
    *qcmap_cp = (qcmap_client_info[client_index]);
    if ( qcmap_cp && (qcmap_cp->wwan_op_ind_pending || qcmap_cp->wwan_status_ind_regd) )
    {
      if (qcmap_cp->wwan_op_ind_pending)
      {
        if (qcmap_cp->wwan_op_ind_msg_id == QCMAP_BRING_UP_WWAN_IND_V01)
        {
          wwan_op_ind_msg->bring_up_ind.conn_status = conn_status;
          wwan_op_ind_msg->bring_up_ind.mobile_ap_handle = cb_data->map_instance;

          if ( conn_status == QCMAP_MSGR_WWAN_STATUS_CONNECTING_FAIL_V01 ||
               conn_status == QCMAP_MSGR_WWAN_STATUS_IPV6_CONNECTING_FAIL_V01)
          {
            wwan_op_ind_msg->bring_up_ind.wwan_call_end_reason_valid = TRUE;
            wwan_op_ind_msg->bring_up_ind.wwan_call_end_reason.wwan_call_end_reason_type = call_end_type;
            wwan_op_ind_msg->bring_up_ind.wwan_call_end_reason.wwan_call_end_reason_code = call_end_reason_code;
            QCMAP_LOG_MSG_MED("Sending WWAN status ind_msg.wwan_call_end_reason_type =%d ind_msg.wwan_call_end_reason_code=%d ",
                              call_end_type,call_end_reason_code,0);
          }
          QCMAP_LOG_MSG_MED("Sending WWAN status %d to client %p", conn_status, qcmap_cp, 0);
          qcmap_send_msg(QCMAP_MOBILEAP_BRING_UP_WWAN_IND_EVT,(void *) &wwan_op_ind_msg->bring_up_ind, client_index);
        }
        else if (qcmap_cp->wwan_op_ind_msg_id == QCMAP_TEAR_DOWN_WWAN_IND_V01)
        {
          wwan_op_ind_msg->tear_down_ind.conn_status = conn_status;
          wwan_op_ind_msg->tear_down_ind.mobile_ap_handle = cb_data->map_instance;

          if ( conn_status == QCMAP_MSGR_WWAN_STATUS_DISCONNECTED_V01 ||
               conn_status == QCMAP_MSGR_WWAN_STATUS_IPV6_DISCONNECTED_V01)
          {
            wwan_op_ind_msg->tear_down_ind.wwan_call_end_reason_valid = TRUE;
            wwan_op_ind_msg->tear_down_ind.wwan_call_end_reason.wwan_call_end_reason_type = call_end_type;
            wwan_op_ind_msg->tear_down_ind.wwan_call_end_reason.wwan_call_end_reason_code = call_end_reason_code;
            QCMAP_LOG_MSG_MED("Sending WWAN status call_end_reason_type =%d call_end_reason_code=%d ",
                            call_end_type,call_end_reason_code,0);
          }
          QCMAP_LOG_MSG_MED("Sending WWAN status %d to client %p", conn_status, qcmap_cp,0);
          qcmap_send_msg(QCMAP_MOBILEAP_TEAR_DOWN_WWAN_IND_EVT,(void *) &wwan_op_ind_msg->tear_down_ind, client_index);
        }

        qcmap_cp->wwan_op_ind_pending = false;
      }
      else if (qcmap_cp->wwan_status_ind_regd)
      {
        memset(ind_msg, 0, sizeof(qcmap_msgr_wwan_status_ind_msg_v01));

        ind_msg->wwan_status = conn_status;
        if ( conn_status == QCMAP_MSGR_WWAN_STATUS_CONNECTING_FAIL_V01 ||
             conn_status == QCMAP_MSGR_WWAN_STATUS_IPV6_CONNECTING_FAIL_V01 ||
             conn_status == QCMAP_MSGR_WWAN_STATUS_DISCONNECTED_V01 ||
             conn_status == QCMAP_MSGR_WWAN_STATUS_IPV6_DISCONNECTED_V01)
        {
          ind_msg->wwan_call_end_reason_valid = TRUE;
          ind_msg->wwan_call_end_reason.wwan_call_end_reason_type = call_end_type;
          ind_msg->wwan_call_end_reason.wwan_call_end_reason_code = call_end_reason_code;
        }
        QCMAP_LOG_MSG_MED("Sending WWAN status %d to client %d", conn_status, qcmap_cp, 0);
        qcmap_send_msg(QCMAP_MOBILEAP_BRING_UP_WWAN_IND_EVT,(void *) ind_msg, client_index); 
      }
    }
  }

  if(qcmap_cp != NULL)
  {
    free(qcmap_cp);
    qcmap_cp = NULL;
  }
  if(ind_msg != NULL)
  {
    free(ind_msg);
    ind_msg = NULL;
  }
  if(wwan_op_ind_msg != NULL)
  {
    free(wwan_op_ind_msg);
    wwan_op_ind_msg = NULL;
  }

}

/*===========================================================================
  FUNCTION QCMAP_Mobile_AP_STATUS_IND()

  DESCRIPTION
    Dispatches a unicast indication to notify registered QCMobileAP client
      about Mobile AP status.

  RETURN VALUE
    None

  DEPENDENCIES
    qcmap_mobile_ap_enable() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
void qcmap_mobile_ap_status_ind
(
  void                           *user_data,
  qcmap_msgr_mobile_ap_status_enum_v01  conn_status
)
{

  qcmap_msgr_mobile_ap_status_ind_msg_v01     *ind_msg;
  qcmap_status_cb_data                        *cb_data;
  uint8                                        conn_handle;


  ds_assert(user_data != NULL);

  cb_data = (qcmap_status_cb_data*)user_data;

  conn_handle = cb_data->conn_handle;

  ind_msg = (qcmap_msgr_mobile_ap_status_ind_msg_v01*)malloc(sizeof(qcmap_msgr_mobile_ap_status_ind_msg_v01));

  if(ind_msg == NULL)
  {
    QCMAP_LOG_MSG_ERROR ("ind_msg == NULL ",
                      0, 0, 0);
    ASSERT(0);
  }
  	
  memset(ind_msg, 0, sizeof(qcmap_msgr_mobile_ap_status_ind_msg_v01));

  ind_msg->mobile_ap_status = conn_status;

  QCMAP_LOG_MSG_HIGH ("qcmap_mobile_ap_status_ind - Sending IND evt to client ",
                      0, 0, 0);

  qcmap_send_msg(QCMAP_MOBILEAP_ENABLE_IND_EVT,(void *) ind_msg,conn_handle);

}

/*===========================================================================
  FUNCTION QCMAP_MOBILE_AP_ENABLE()

  DESCRIPTION
    Enable SoftAP functionality on modem.

  PARAMETERS
    data - Data from client
    conn_handle: Used to identify client

  RETURN VALUE
    None

  DEPENDENCIES
    qcmap_client_reg_cb should be called

  SIDE EFFECTS
    None
===========================================================================*/
void  qcmap_mobile_ap_enable
(
  void* data,
  uint8 conn_handle
)
{
  qcmap_msgr_mobile_ap_enable_resp_msg_v01   *resp_msg;
  qcmap_status_cb_data    *cb_data_ptr = NULL;

  int              handle;
  unsigned int     index;
  qcmap_error_type qcmap_err_num;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  QCMAP_LOG_MSG_MED ("Client %d enabling Mobile AP", conn_handle, 0, 0);

  resp_msg = (qcmap_msgr_mobile_ap_enable_resp_msg_v01*)malloc(sizeof(qcmap_msgr_mobile_ap_enable_resp_msg_v01));

  if (resp_msg == NULL)
  {
    QCMAP_LOG_MSG_ERROR("Out of mem for resp msg", 0, 0, 0);
    ASSERT(0);
  }

  memset(resp_msg, 0, sizeof(qcmap_msgr_mobile_ap_enable_resp_msg_v01));

  /*---------------------------------------------------------------------
    Enable SoftAP
  ---------------------------------------------------------------------*/

  if (qcmap_handle.cb_ptr == NULL)
  {
    cb_data_ptr = (qcmap_status_cb_data*)malloc(sizeof(qcmap_status_cb_data));

    if (cb_data_ptr == NULL)
    {
      QCMAP_LOG_MSG_ERROR("Out of mem for WWAN cb data", 0, 0, 0);
      ASSERT(0);
    }

    cb_data_ptr->map_instance = QCMAP_SOFTAP_HANDLE;
    cb_data_ptr->wwan_cb  = qcmap_wwan_status_ind;
    cb_data_ptr->qcmap_cb = qcmap_mobile_ap_status_ind;
    cb_data_ptr->conn_handle = conn_handle;

    qcmap_handle.cb_ptr = cb_data_ptr;
  }

  if(!qcmap_client_info[conn_handle].client_enabled)
  {
    if(qc_mgr->client_ref_count > 0)
    {
      qcmap_client_info[conn_handle].client_enabled = TRUE;
      qc_mgr->client_ref_count++;                //no need
      index = QCMAP_SOFTAP_HANDLE;
      QCMAP_LOG_MSG_MED("Enabled MobileAP inst %d", index, 0, 0);
      resp_msg->mobile_ap_handle_valid = TRUE;
      resp_msg->mobile_ap_handle = index;
      qcmap_send_msg(QCMAP_MOBILEAP_ENABLE_RESP_EVT,(void *) resp_msg,conn_handle);

      if(resp_msg != NULL)
      {
        free(resp_msg);
        resp_msg = NULL;
      }

      return;
    }
    else if(qc_mgr->client_ref_count == 0)
    {

      if (Enable(&handle, &qcmap_handle, &qcmap_err_num))
      {
        qcmap_client_info[conn_handle].client_enabled = TRUE;
        qc_mgr->client_ref_count++;
      }
      else
      {
        QCMAP_LOG_MSG_MED("MobileAP Enable fails.", 0, 0, 0);

        resp_msg->mobile_ap_handle_valid = false;
        resp_msg->resp.result = QCMAP_RESULT_FAILURE;
        resp_msg->resp.error = qcmap_err_num;

        qcmap_send_msg(QCMAP_MOBILEAP_ENABLE_RESP_EVT,(void *) resp_msg,conn_handle);

        if(resp_msg != NULL)
        {
          free(resp_msg);
          resp_msg = NULL;
        }

        return;
      }


      qcmap_handle.handle = handle;

      index = QCMAP_SOFTAP_HANDLE;
      QCMAP_LOG_MSG_MED("Enabled MobileAP inst %d", index, 0, 0);
      resp_msg->mobile_ap_handle_valid = TRUE;
      resp_msg->mobile_ap_handle = index;
      qcmap_send_msg(QCMAP_MOBILEAP_ENABLE_RESP_EVT,(void *) resp_msg,conn_handle);

      if(resp_msg != NULL)
      {
        free(resp_msg);
        resp_msg = NULL;
      }

      return;
    }
  }
  else
  {
    QCMAP_LOG_MSG_ERROR("The client has already enabled MobileAP", 0, 0, 0);
    resp_msg->mobile_ap_handle_valid = false;
    resp_msg->resp.result = QCMAP_RESULT_FAILURE;
    resp_msg->resp.error = QCMAP_ERR_NO_EFFECT;
    qcmap_send_msg(QCMAP_MOBILEAP_ENABLE_RESP_EVT,(void *) resp_msg,conn_handle);

    if(resp_msg != NULL)
    {
      free(resp_msg);
      resp_msg = NULL;
    }

    return;
  }

}

/*=====================================================
  FUNCTION Enable
======================================================*/
/*!
@brief
  Enables MobileAP based on the configuration.
  As a part of enable mobileap, registers mobileap event callback.
  It will bring up RmNet.
  If AutoConnect is enabled then initiates connect to backhaul.

@return
  true  - on success
  false - on failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=====================================================*/
boolean Enable
(
  int *handle,
  void *cb_user_data,
  qcmap_error_type *qcmap_err_num
)
{
  int qcmap_errno;
  QCMAP_LOG_FUNC_ENTRY();

  qc_mgr->qcmap_cm_handle = *handle =qcmap_enable(qcmap_connectionmanager_callback,
                                                 cb_user_data,
                                                 &qcmap_errno,
                                                 qcmap_err_num);


  if (qc_mgr->qcmap_cm_handle <= 0)
  {
    QCMAP_LOG_MSG_ERROR("QCMAP Connection failed ",0,0,0);
    return false;
  }

  /* Store all the Client/CB related information */
  qc_mgr->qcmap_cb_handle =
         (void *)((qcmap_softap_handle_type *)cb_user_data)->cb_ptr;

  QCMAP_LOG_MSG_MED("QCMAP Enabled this->qcmap_cm_handle=%x ",
                qc_mgr->qcmap_cm_handle,0,0);
  qc_mgr->qcmap_enable = true;
  return true;
}

/*===========================================================================

FUNCTION QCMAP_ENABLE()

DESCRIPTION

  Enable MobileAP based on the user input.
  It will register MobileAP event callback.
  It will configure Modem in MobileAP Mode and bring up RmNet.

DEPENDENCIES
  None.

RETURN VALUE
  Returns MobileAP application ID on success.

  On error, return 0 and places the error condition value in
  *qcmap_errno.

SIDE EFFECTS

===========================================================================*/
int qcmap_enable
(
  qcmap_cb_fcn     qcmap_callback,             /* Callback function     */
  void             *qcmap_callback_user_data,   /* Callback user data    */
  int              *qcmap_errno,                /* Error condition value */
  qcmap_error_type *qcmap_err_num
)
{
  int qcmap_handle = 0;
  qurt_thread_attr_t attr;


  ds_assert(qcmap_callback != NULL);
  ds_assert(qcmap_errno != NULL);
  ds_assert(qcmap_err_num != NULL);

  if (qcmap_cb.init == false)
  {
    QCMAP_LOG_MSG_MED("qcmap_enable: init", 0, 0, 0);

    /* If it is the first time call to QCMAP CM */
    qurt_thread_attr_init(&attr);
    qurt_mutex_init(&qcmap_cb.qcmap_mutex);
    qcmap_cb.init = TRUE;
  }


  if (qcmap_cb.disable_in_process)
  {
    *qcmap_errno = QCMAP_EOPNOTSUPP;
    QCMAP_LOG_MSG_ERROR( "qcmap_enable fail because disable still in progress", 0, 0, 0);
    *qcmap_err_num = QCMAP_ERR_NO_EFFECT;
    return QCMAP_ERROR;
  }

  if ( ((qcmap_cb.dss_net_hndl.handle != NULL) &&
       (qcmap_cb.state > QCMAP_DISABLE)) ||
       ((qcmap_cb.ipv6_dss_net_hndl.handle != NULL) &&
       (qcmap_cb.ipv6_state > QCMAP_V6_DISABLE)))
  {
    *qcmap_errno = QCMAP_ENOERROR;
    QCMAP_LOG_MSG_MED( "qcmap_enable : already enabled",0,0,0);
    *qcmap_err_num = QCMAP_ERR_NO_EFFECT;
    return qcmap_cb.handle;
  }

  qcmap_cb.qcmap_callback = qcmap_callback;
  qcmap_cb.qcmap_callback_user_data = qcmap_callback_user_data;

  memset( &qcmap_cb.dss_net_hndl, 0x0, sizeof(qcmap_cb.dss_net_hndl) );
  memset( &qcmap_cb.ipv6_dss_net_hndl, 0x0, sizeof(qcmap_cb.ipv6_dss_net_hndl) );

  if (qcmap_nas_init() == QCMAP_ERROR)
  {
    *qcmap_errno = QCMAP_EOPNOTSUPP;
    QCMAP_LOG_MSG_ERROR("Unable to initialize QMI NAS service in QCMAP.", 0, 0, 0);
    *qcmap_err_num = QCMAP_ERR_INTERNAL;
    return qcmap_handle;
  }

  qcmap_cb.state = QCMAP_ENABLE;
  qcmap_cb.ipv6_state = QCMAP_V6_ENABLE;

  QCMAP_LOG_MSG_MED("Enable: STATES V4 %d V6 %d", qcmap_cb.state, qcmap_cb.ipv6_state,0);
  qcmap_handle = qcmap_cb.handle;

  QCMAP_LOG_MSG_MED("QCMAP AP Handle 0x%04x Enabled", qcmap_handle, 0, 0);
  qcmap_cb.qcmap_callback( qcmap_cb.handle,
                           QCMAP_EVENT_ENABLED,
                           qcmap_cb.qcmap_callback_user_data,NULL);

  return qcmap_handle;
}


/*============================================================
  FUNCTION qcmap_nas_init
==============================================================
@brief
  API to register QMI NAS Service available call back

@return
  QCMAP_ERROR - Could not register QMI NAS service callback
  QCMAP_SUCCESS - Successfully registered QMI NAS service call back
@note

  - Dependencies
    - None

  - Side Effects
    - None
==========================================================*/
int qcmap_nas_init()
{
  qmi_idl_service_object_type nas_qmi_idl_service_object;
  qmi_client_error_type qmi_error;

  nas_qmi_idl_service_object = nas_get_service_object_v01();
  if (nas_qmi_idl_service_object == NULL)
  {
    QCMAP_LOG_MSG_MED("qcmap service object(nas) not available.",0,0,0);
    return QCMAP_ERROR;
  }

  qmi_error = qmi_client_notifier_init(nas_qmi_idl_service_object,
                                       &qcmap_cb.qmi_nas_os_params,
                                       &qcmap_cb.qmi_nas_notifier);
  if (qmi_error < 0)
  {
    QCMAP_LOG_MSG_MED("qmi_client_notifier_init(nas) returned %d",
                  qmi_error,0,0);
    return QCMAP_ERROR;
  }

  qmi_error =  qmi_client_register_notify_cb(qcmap_cb.qmi_nas_notifier,
                                             qmi_nas_service_available_cb,
                                             NULL);

  if (qmi_error < 0)
  {
    QCMAP_LOG_MSG_MED("qmi_client_register_notify_cb(nas) returned %d",
                  qmi_error,0,0);
    return QCMAP_ERROR;
  }

  QCMAP_LOG_MSG_MED(" qcmap_nas_init : QMI NAS init successful", 0, 0, 0);
  return QCMAP_SUCCESS;

}

/*============================================================
  FUNCTION qmi_nas_service_available_cb
==============================================================
@brief
 QMI NAS Service Available callback handler

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
==========================================================*/
void qmi_nas_service_available_cb
(
  qmi_client_type                user_handle,
  qmi_idl_service_object_type    service_obj,
  qmi_client_notify_event_type   service_event,
  void                           *notify_cb_data
)
{

  qcmap_cmd_t*             cmd_ptr = NULL;
  qcmap_qmi_service_buffer_t  *qcmap_qmi_service_buffer;

  qcmap_qmi_service_buffer = (qcmap_qmi_service_buffer_t  *)malloc(sizeof(qcmap_qmi_service_buffer_t));

  if(qcmap_qmi_service_buffer == NULL)
  {
    QCMAP_LOG_MSG_ERROR("Cannot assign memory for qmi service buffer. Assert", 0, 0, 0);
    ASSERT(0);
  }

  switch(service_event)
  {
    case QMI_CLIENT_SERVICE_COUNT_INC:
		qcmap_qmi_service_buffer->qmi_service_id = QCMAP_QMI_NAS_IN_SERVICE;
		break;
    default:
		qcmap_qmi_service_buffer->qmi_service_id = QCMAP_QMI_NAS_NOT_IN_SERVICE;
		break;
  }
  
  if((cmd_ptr = qcmap_get_cmd_buffer()) == NULL)
  {
    QCMAP_LOG_MSG_ERROR("Cannot obtain command buffer. Assert", 0, 0, 0);
    ASSERT(0);
  }
       
  else
  {
       
    cmd_ptr->cmd_hdr.cmd_id = QCMAP_QMI_NAS_IN_SERVICE_EVT;
    cmd_ptr->cmd_data.data = (void *)qcmap_qmi_service_buffer;
    //Enqueue the command into the command queue.
    qcmap_enqueue_cmd(cmd_ptr);
  }
       
  qcmap_set_signal();

  return;

}

/*=====================================================
  FUNCTION Disable
======================================================*/
/*!
@brief
  Disable Mobile ap will bring down the backhaul.

@return
  true  - on success
  false - on failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=====================================================*/

boolean Disable
(
  int *err_num,
  qcmap_error_type *qcmap_err_num
)
{
  int ret = 0, qcmap_errno;
  boolean retval;
  QCMAP_LOG_FUNC_ENTRY();

  if (!qc_mgr->qcmap_enable)
  {
    /* QCMAP is not enabled */
    QCMAP_LOG_MSG_ERROR("QCMAP not enabled", 0, 0, 0);
    *qcmap_err_num = QCMAP_ERR_INVALID_HANDLE;
    return false;
  }

  qc_mgr->qcmap_tear_down_in_progress = true;

  ret = qcmap_disable(qc_mgr->qcmap_cm_handle, &qcmap_errno, qcmap_err_num);

  if (ret < 0)
  {
    QCMAP_LOG_MSG_ERROR("QCMAP Disable fails: %d", qcmap_errno, 0, 0);
    retval = false;
  }
  else
  {
    *err_num = qcmap_errno;
    QCMAP_LOG_MSG_ERROR("QCMAP Disable in progress", 0, 0, 0);
    retval = true;
  }
  qc_mgr->qcmap_enable = false;
  return retval;
}

/*=====================================================
  FUNCTION DisableHandle
======================================================*/
/*!
@brief
 Initialize mobileap handle to zero.

@return
  None.

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=====================================================*/
void DisableHandle()
{
  QCMAP_LOG_MSG_MED("DisableHandle", 0, 0, 0);
  qc_mgr->qcmap_cm_handle = 0;
}

/*===========================================================================
  FUNCTION QCMAP_MOBILE_AP_DISABLE()

  DESCRIPTION
    Disable SoftAP functionality on modem.

  PARAMETERS
    data - Data from client
    conn_handle: Used to identify client

  RETURN VALUE
    None

  DEPENDENCIES
    qcmap_client_reg_cb should be called

  SIDE EFFECTS
    None
===========================================================================*/

void  qcmap_mobile_ap_disable(
  void* data,
  uint8 conn_handle
)
{

  qcmap_msgr_mobile_ap_disable_req_msg_v01   *req_ptr = NULL;
  qcmap_msgr_mobile_ap_disable_resp_msg_v01  *resp_msg;
  qcmap_softap_handle_type                   *map_handle;
  int                                        return_val, err_num=0;
  unsigned int                               index;
  qcmap_error_type                           qcmap_err_num;
  int                                        client_count = 1;

  ds_assert(data != NULL);

  resp_msg = (qcmap_msgr_mobile_ap_disable_resp_msg_v01 *)malloc(sizeof(qcmap_msgr_mobile_ap_disable_resp_msg_v01));
  if (resp_msg == NULL)
  {
    QCMAP_LOG_MSG_ERROR("Out of mem for resp msg", 0, 0, 0);
    ASSERT(0);
  }
  memset(resp_msg, 0, sizeof(qcmap_msgr_mobile_ap_disable_resp_msg_v01));

  req_ptr =  (qcmap_msgr_mobile_ap_disable_req_msg_v01*)data;

  /*---------------------------------------------------------------------
    Disable SOftAP. This would trigger all RmNet calls to stop.
  ---------------------------------------------------------------------*/
  index = req_ptr->mobile_ap_handle;

  if (index != QCMAP_SOFTAP_HANDLE)
  {
    QCMAP_LOG_MSG_ERROR("Incorrect handle passed %d", req_ptr->mobile_ap_handle, 0, 0);
    resp_msg->resp.result = QCMAP_RESULT_FAILURE;
    resp_msg->resp.error = QCMAP_ERR_INVALID_HANDLE;
    qcmap_send_msg(QCMAP_MOBILEAP_DISABLE_RESP_EVT, (void *)resp_msg, conn_handle);

    if(resp_msg != NULL)
    {
      free(resp_msg);
      resp_msg = NULL;
    }
    if(req_ptr != NULL)
    {
      free(req_ptr);
      req_ptr = NULL;
    }

    return;
  }

  map_handle = &qcmap_handle;

  if (map_handle->handle == 0)
  {
    QCMAP_LOG_MSG_ERROR("MobileAP inst %d already disabled", index, 0, 0);
    resp_msg->resp.result = QCMAP_RESULT_FAILURE;
    resp_msg->resp.error = QCMAP_ERR_NO_EFFECT;
    qcmap_send_msg(QCMAP_MOBILEAP_DISABLE_RESP_EVT, (void *)resp_msg, conn_handle);

    if(resp_msg != NULL)
    {
      free(resp_msg);
      resp_msg = NULL;
    }
    if(req_ptr != NULL)
    {
      free(req_ptr);
      req_ptr = NULL;
    }

    return;
  }

  QCMAP_LOG_MSG_MED ("Client %d disabling MobileAP inst %d", conn_handle, index, 0);

  // If client is enabled but ref count = 0, implies mobileap disable is already called on the server side
  if ((qcmap_client_info[conn_handle].client_enabled) && (qc_mgr->client_ref_count == 0))
  {
    qcmap_client_info[conn_handle].client_enabled = false;
    resp_msg->resp.result = QCMAP_RESULT_SUCCESS;
    resp_msg->resp.error = QCMAP_ERR_NONE;
    qcmap_send_msg(QCMAP_MOBILEAP_DISABLE_RESP_EVT, (void *)resp_msg, conn_handle);

    if(resp_msg != NULL)
    {
      free(resp_msg);
      resp_msg = NULL;
    }
    if(req_ptr != NULL)
    {
      free(req_ptr);
      req_ptr = NULL;
    }

    return ;
  }

  if(qcmap_client_info[conn_handle].client_enabled)
  {
    if(qc_mgr->client_ref_count > client_count)
    {
      qc_mgr->client_ref_count--;
      qcmap_client_info[conn_handle].client_enabled = false;
      resp_msg->resp.result = QCMAP_RESULT_SUCCESS;
      resp_msg->resp.error = QCMAP_ERR_NO_EFFECT;
      qcmap_send_msg(QCMAP_MOBILEAP_DISABLE_RESP_EVT, (void *)resp_msg, conn_handle);
 
      if(resp_msg != NULL)
      {
        free(resp_msg);
        resp_msg = NULL;
      }
      if(req_ptr != NULL)
      {
        free(req_ptr);
        req_ptr = NULL;
      }

      return ;
    }
    else if(qc_mgr->client_ref_count == client_count)
    {
      return_val= Disable(&err_num, &qcmap_err_num);

      if (return_val)
      {
        if (err_num == QCMAP_EALDDISCONN)
        {
          qc_mgr->client_ref_count = qc_mgr->client_ref_count - client_count;
          QCMAP_LOG_MSG_MED("Client reference count %d", qc_mgr->client_ref_count, 0, 0);
          qcmap_client_info[conn_handle].client_enabled = false;
          resp_msg->resp.result = QCMAP_RESULT_SUCCESS;
          resp_msg->resp.error = QCMAP_ERR_NONE;
          qcmap_send_msg(QCMAP_MOBILEAP_DISABLE_RESP_EVT, (void *)resp_msg, conn_handle);
          map_handle->handle = 0;
 
          if(resp_msg != NULL)
          {
            free(resp_msg);
            resp_msg = NULL;
          }
          if(req_ptr != NULL)
          {
            free(req_ptr);
            req_ptr = NULL;
          }
          return ;
        }
        QCMAP_LOG_MSG_MED("MobileAP Disable in progress." ,0, 0, 0);
        resp_msg->resp.result = QCMAP_RESULT_SUCCESS;
        resp_msg->resp.error = QCMAP_ERR_NONE;

        /*.If backhaul is not connected, Mobileap will be disabled instantly.
           And since call back function is being called much before the response
           pending flag is set to TRUE, responses are not sent to the client.
           Hence, we set qcmap_disable_resp_msg_v01.resp.error to
           QMI_ERR_NO_EFFECT_V01, so that the caller of this function sends a
           response back to the client. Used for IoE 9x25.
          */
        if (err_num == QCMAP_DISCONNECTED)
          resp_msg->resp.error = QCMAP_ERR_NO_EFFECT;
      }
      else
      {
        QCMAP_LOG_MSG_ERROR("MobileAP Disable request fails." ,0, 0, 0);
        resp_msg->resp.result = QCMAP_RESULT_FAILURE;
        resp_msg->resp.error = qcmap_err_num;
        qcmap_send_msg(QCMAP_MOBILEAP_DISABLE_RESP_EVT, (void *)resp_msg, conn_handle);
        
        if(resp_msg != NULL)
        {
          free(resp_msg);
          resp_msg = NULL;
        }
        if(req_ptr != NULL)
        {
          free(req_ptr);
          req_ptr = NULL;
        }
        return ;
      }

      qc_mgr->client_ref_count = qc_mgr->client_ref_count - client_count;

      QCMAP_LOG_MSG_MED("Client reference count %d", qc_mgr->client_ref_count, 0, 0);
      qcmap_client_info[conn_handle].client_enabled = false;
      qcmap_send_msg(QCMAP_MOBILEAP_DISABLE_RESP_EVT, (void *)resp_msg, conn_handle);
      map_handle->handle = 0;
 
      if(resp_msg != NULL)
      {
        free(resp_msg);
        resp_msg = NULL;
      }
      if(req_ptr != NULL)
      {
        free(req_ptr);
        req_ptr = NULL;
      }
      return;
    }
  }
  else
  {
    QCMAP_LOG_MSG_ERROR("MobileAP not enabled by client, but trying to disable!!", 0, 0, 0);
    resp_msg->resp.result = QCMAP_RESULT_FAILURE;
    resp_msg->resp.error = QCMAP_ERR_INVALID_HANDLE;
    qcmap_send_msg(QCMAP_MOBILEAP_DISABLE_RESP_EVT, (void *)resp_msg, conn_handle);

    if(resp_msg != NULL)
    {
      free(resp_msg);
      resp_msg = NULL;
    }
    if(req_ptr != NULL)
    {
      free(req_ptr);
      req_ptr = NULL;
    }

    return ;
  }
}

/*===========================================================================

FUNCTION QCMAP_DISABLE()

DESCRIPTION

  Disable MobileAP.
  It will configure Modem in non-MobileAP mode.

DEPENDENCIES
  None.

RETURN VALUE

  qcmap_errno Values
  ----------------
  QCMAP_EBADAPP           invalid application ID specified

SIDE EFFECTS

===========================================================================*/
int qcmap_disable
(
  int  qcmap_handle,                          /* Handle for MobileAP CM */
  int *qcmap_errno,                           /* Error condition value  */
  qcmap_error_type *qcmap_err_num
)
{
  int ret = QCMAP_SUCCESS;
  int ret_val;
  qmi_client_error_type qmi_error;

  QCMAP_LOG_MSG_MED("qcmap_disable: enter", 0, 0, 0);
  ds_assert(qcmap_errno != NULL);

  if (qcmap_handle != qcmap_cb.handle)
  {
    *qcmap_errno = QCMAP_EBADAPP;
    ret = QCMAP_ERROR;
    *qcmap_err_num = QCMAP_ERR_INVALID_HANDLE;
    QCMAP_LOG_MSG_MED( "Wrong QCMAP CM Handle", 0, 0, 0);
    return ret;
  }

  if (qcmap_cb.state == QCMAP_DISABLE && qcmap_cb.ipv6_state == QCMAP_V6_DISABLE)
  {
    *qcmap_errno = QCMAP_EALDDISCONN;
    ret = QCMAP_SUCCESS;
    *qcmap_err_num = QCMAP_ERR_NO_EFFECT;
    QCMAP_LOG_MSG_ERROR( "Already disconnected...", 0, 0, 0);
    return ret;
  }

  if(qcmap_cb.state > QCMAP_ENABLE || qcmap_cb.ipv6_state > QCMAP_V6_ENABLE)
  {
    qcmap_cb.disable_in_process = TRUE;
    if ( qcmap_cb.state > QCMAP_ENABLE )
    {
      ret_val = qcmap_disconnect_backhaul(qcmap_cb.handle, QCMAP_MSGR_WWAN_CALL_TYPE_V4_V01, qcmap_errno, qcmap_err_num);
      if (ret_val == QCMAP_ERROR && *qcmap_errno != QCMAP_EWOULDBLOCK)
      {
        /* IPV4 Backhaul disconnecting failed. */
        QCMAP_LOG_MSG_ERROR("IPV4 backhaul disconnecting failed: Error %x", *qcmap_err_num, 0, 0);
        return ret_val;
      }
    }
    if ( qcmap_cb.ipv6_state > QCMAP_V6_ENABLE )
    {
      ret_val = qcmap_disconnect_backhaul(qcmap_cb.handle, QCMAP_MSGR_WWAN_CALL_TYPE_V6_V01, qcmap_errno, qcmap_err_num);
      if (ret_val == QCMAP_ERROR && *qcmap_errno != QCMAP_EWOULDBLOCK)
      {
        /* IPV6 Backhaul disconnecting failed. */
        QCMAP_LOG_MSG_ERROR("IPV6 backhaul disconnecting failed: Error %x", *qcmap_err_num, 0, 0);
        return ret_val;
      }
    }
  }
  else if(qcmap_cb.state <= QCMAP_ENABLE && qcmap_cb.ipv6_state <= QCMAP_V6_ENABLE)
  {
    qcmap_cb.state = QCMAP_DISABLE;
    qcmap_cb.ipv6_state = QCMAP_V6_DISABLE;
    QCMAP_LOG_MSG_MED("Disable: STATES V4 %d V6 %d", qcmap_cb.state, qcmap_cb.ipv6_state, 0);

    qmi_error = qmi_client_release(qcmap_cb.qmi_nas_notifier);
    qcmap_cb.qmi_nas_notifier = NULL;

    if (qmi_error != QMI_NO_ERR)
    {
      QCMAP_LOG_MSG_ERROR("Can not release client nas notifier %d", qmi_error, 0, 0);
    }

    qmi_error = qmi_client_release(qcmap_cb.qmi_nas_handle);
    qcmap_cb.qmi_nas_handle = NULL;

    if (qmi_error != QMI_NO_ERR)
    {
      QCMAP_LOG_MSG_ERROR("Can not release client nas handle %d", qmi_error, 0, 0);
    }

    if (qcmap_cb.ipv6_dss_net_hndl.handle != NULL)
    {
      qapi_DSS_Rel_Data_Srvc_Hndl((qapi_DSS_Hndl_t)qcmap_cb.ipv6_dss_net_hndl.handle);
      qcmap_cb.ipv6_dss_net_hndl.handle = NULL;
      memset(&qcmap_cb.ipv6_dss_net_hndl, 0x0, sizeof(qcmap_cb.ipv6_dss_net_hndl));
    }

    if ( qcmap_cb.dss_net_hndl.handle != NULL )
    {
      qapi_DSS_Rel_Data_Srvc_Hndl((qapi_DSS_Hndl_t)qcmap_cb.dss_net_hndl.handle);
      qcmap_cb.dss_net_hndl.handle = NULL;
      memset(&qcmap_cb.dss_net_hndl, 0x0, sizeof(qcmap_cb.dss_net_hndl));
    }

    qcmap_cb.disable_in_process = false;

    /* qcmap_err_num is set to QCMAP_DISCONNECTED to indicate clients that MobileAP is
    is disabled. */
    *qcmap_errno = QCMAP_DISCONNECTED;

    qcmap_cb.qcmap_callback(qcmap_cb.handle, QCMAP_EVENT_DISABLED, qcmap_cb.qcmap_callback_user_data,NULL);

    QCMAP_LOG_MSG_MED("QCMAP AP Handle 0x%04x Disabled", qcmap_handle, 0, 0);

  }

  return ret;
}

/*===========================================================================

FUNCTION BRING_UP_IPV4_WWAN()

DESCRIPTION

  It will bring up IPV4 WWAN.

DEPENDENCIES
  None.

RETURN VALUE
  Conncets WAN for IPV4 and returns QCMAP_SUCCESS if sucessful
  return QCMAP_ERROR and places the error condition value in
  *qcmap_errno.

  qcmap_errno Values
  ----------------
  QCMAP_ERR_INVALID_HANDLE           invalid handle received
  QCMAP_EWOULDBLOCK                  the operation would block
  QCMAP_EOPNOTSUPP                   backhaul bringup/teardown in progress


SIDE EFFECTS

===========================================================================*/

int bring_up_ipv4_wwan
(
  int *qcmap_errno,
  qcmap_error_type *qcmap_err_num
)
{
  int ret = QCMAP_SUCCESS, rval;
  qapi_DSS_Call_Param_Value_t param_info, param_info_profile;
  boolean enable_v4= 1;

  if (enable_v4)
  {
    if (qcmap_cb.state == QCMAP_WAN_CONNECTED)
    {
      ret = QCMAP_SUCCESS;
      *qcmap_errno = QCMAP_EALDCONN;
      QCMAP_LOG_MSG_MED("QCMAP Backhaul already connected", 0, 0, 0);
      *qcmap_err_num = QCMAP_ERR_NO_EFFECT;
      return ret;
    }
    else if ((qcmap_cb.state == QCMAP_WAN_CONNECTING) || (qcmap_cb.state == QCMAP_WAN_DISCONNECTING))
    {
      /* We have some outstanding WAN request */
      *qcmap_errno = QCMAP_EOPNOTSUPP;
      ret = QCMAP_ERROR;
      QCMAP_LOG_MSG_MED("QCMAP has outstanding backhaul request", 0, 0, 0);
      *qcmap_err_num = QCMAP_ERR_NO_EFFECT;
      return ret;
    }

  /* If we already have an handle release it. */
  if ( qcmap_cb.dss_net_hndl.handle != NULL )
  {
    qapi_DSS_Rel_Data_Srvc_Hndl((qapi_DSS_Hndl_t)qcmap_cb.dss_net_hndl.handle);
    qcmap_cb.dss_net_hndl.handle = NULL;
    memset(&qcmap_cb.dss_net_hndl, 0x0, sizeof(qcmap_cb.dss_net_hndl));
  }

  /* Acquire the handle. */
  qapi_DSS_Get_Data_Srvc_Hndl((qapi_DSS_Net_Ev_CB_t)qcmap_dss_net_cb_fcn, (void*) &qcmap_cb.dss_net_hndl, &qcmap_cb.dss_net_hndl.handle);
  
  if (qcmap_cb.dss_net_hndl.handle == NULL)
  {
    *qcmap_errno = QCMAP_EOPNOTSUPP;
    ret = QCMAP_ERROR;
    QCMAP_LOG_MSG_MED("bring_up_ipv4_wwan: Can not get dss net handle", 0, 0, 0);
    *qcmap_err_num = QCMAP_ERR_INVALID_HANDLE;
    return ret;
  }

  param_info_profile.buf_val = NULL;
  param_info_profile.num_val = qcmap_file_t.profile;

  if(qcmap_file_t.profile > 0)
  {
    if(qcmap_file_t.profile > DSS_PROFILE_3GPP2_OFFSET)
    {
      qcmap_cb.dss_net_hndl.profile = qcmap_file_t.profile - DSS_PROFILE_3GPP2_OFFSET;
      param_info_profile.num_val = qcmap_file_t.profile - DSS_PROFILE_3GPP2_OFFSET;
      qapi_DSS_Set_Data_Call_Param((qapi_DSS_Hndl_t)qcmap_cb.dss_net_hndl.handle,
	  	(qapi_DSS_Call_Param_Identifier_t)QAPI_DSS_CALL_INFO_CDMA_PROFILE_IDX_E ,(qapi_DSS_Call_Param_Value_t *)&param_info_profile);
    }

    else
    {
      qcmap_cb.dss_net_hndl.profile = qcmap_file_t.profile;
      qapi_DSS_Set_Data_Call_Param((qapi_DSS_Hndl_t)qcmap_cb.dss_net_hndl.handle,
	  	(qapi_DSS_Call_Param_Identifier_t)QAPI_DSS_CALL_INFO_UMTS_PROFILE_IDX_E ,(qapi_DSS_Call_Param_Value_t *)&param_info_profile);
    }
  }

  /* set data call param */
  param_info.buf_val = NULL;

  if(strcasecmp(qcmap_file_t.call_tech ,"auto") == 0)
  {
   strlcpy(qcmap_cb.dss_net_hndl.tech, "AUTO", sizeof(qcmap_cb.dss_net_hndl.tech));
   param_info.num_val = QAPI_DSS_RADIO_TECH_UNKNOWN;
   qapi_DSS_Set_Data_Call_Param((qapi_DSS_Hndl_t)qcmap_cb.dss_net_hndl.handle, 
   	(qapi_DSS_Call_Param_Identifier_t)QAPI_DSS_CALL_INFO_TECH_PREF_E, (qapi_DSS_Call_Param_Value_t *)&param_info);
  }

  else if(strcasecmp(qcmap_file_t.call_tech ,"umts") == 0)
  {
   strlcpy(qcmap_cb.dss_net_hndl.tech, "UMTS", sizeof(qcmap_cb.dss_net_hndl.tech));
   param_info.num_val = QAPI_DSS_RADIO_TECH_UMTS;
   qapi_DSS_Set_Data_Call_Param((qapi_DSS_Hndl_t)qcmap_cb.dss_net_hndl.handle, 
   	(qapi_DSS_Call_Param_Identifier_t)QAPI_DSS_CALL_INFO_TECH_PREF_E, (qapi_DSS_Call_Param_Value_t *)&param_info);
  }

  else if(strcasecmp(qcmap_file_t.call_tech ,"lte") == 0)
  {
   strlcpy(qcmap_cb.dss_net_hndl.tech, "LTE", sizeof(qcmap_cb.dss_net_hndl.tech));
   param_info.num_val = QAPI_DSS_RADIO_TECH_LTE;
   qapi_DSS_Set_Data_Call_Param((qapi_DSS_Hndl_t)qcmap_cb.dss_net_hndl.handle, 
   	(qapi_DSS_Call_Param_Identifier_t)QAPI_DSS_CALL_INFO_TECH_PREF_E, (qapi_DSS_Call_Param_Value_t *)&param_info);
  }

  else if(strcasecmp(qcmap_file_t.call_tech ,"cdma") == 0)
  {
   strlcpy(qcmap_cb.dss_net_hndl.tech, "CDMA", sizeof(qcmap_cb.dss_net_hndl.tech));
   param_info.num_val = QAPI_DSS_RADIO_TECH_CDMA;
   qapi_DSS_Set_Data_Call_Param((qapi_DSS_Hndl_t)qcmap_cb.dss_net_hndl.handle, 
   	(qapi_DSS_Call_Param_Identifier_t)QAPI_DSS_CALL_INFO_TECH_PREF_E, (qapi_DSS_Call_Param_Value_t *)&param_info);
  }

  strlcpy(qcmap_cb.dss_net_hndl.family, "0" , sizeof(qcmap_cb.dss_net_hndl.family));
  param_info.buf_val = NULL;
  param_info.num_val = QAPI_DSS_IP_VERSION_4;
  qapi_DSS_Set_Data_Call_Param((qapi_DSS_Hndl_t)qcmap_cb.dss_net_hndl.handle, 
  	(qapi_DSS_Call_Param_Identifier_t)QAPI_DSS_CALL_INFO_IP_VERSION_E, (qapi_DSS_Call_Param_Value_t *)&param_info);

  QCMAP_LOG_MSG_MED("bring_up_ipv4_wwan: dss_net_handle %X", qcmap_cb.dss_net_hndl.handle, 0, 0);

    /* Bring up the data call. */
    /* Connecting WWAN */

    rval = qapi_DSS_Start_Data_Call((qapi_DSS_Hndl_t)qcmap_cb.dss_net_hndl.handle);

    if (rval != QAPI_DSS_SUCCESS)
    {
      //Handle Error condition gracefully
    }
    else
    {
      *qcmap_errno = QCMAP_EWOULDBLOCK;
      qcmap_cb.state = QCMAP_WAN_CONNECTING;
    }
  }
  else
  {
    ret = QCMAP_ERROR;
    *qcmap_errno = QCMAP_EOPNOTSUPP;
    *qcmap_err_num = QCMAP_ERR_INVALID_HANDLE;
    QCMAP_LOG_MSG_ERROR("Cannot bring up IPV4 wwan:-  dss handle 0x%x, IPv4 enabled:- %d", qcmap_cb.dss_net_hndl.handle, enable_v4, 0);
  }

  return ret;
}

/*===========================================================================

FUNCTION BRING_UP_IPV6_WWAN()

DESCRIPTION

  It will bring up IPV4 WWAN.

DEPENDENCIES
  None.

RETURN VALUE
  Conncets WAN for IPV6 and returns QCMAP_SUCCESS if sucessful
  return QCMAP_ERROR and places the error condition value in
  *qcmap_errno.

  qcmap_errno Values
  ----------------
  QCMAP_ERR_INVALID_HANDLE           invalid handle received
  QCMAP_EWOULDBLOCK                  the operation would block
  QCMAP_EOPNOTSUPP                   backhaul bringup/teardown in progress


SIDE EFFECTS

===========================================================================*/

int bring_up_ipv6_wwan
(
  int *qcmap_errno,
  qcmap_error_type *qcmap_err_num
)
{

  int ret = QCMAP_SUCCESS, rval;
  qapi_DSS_Call_Param_Value_t param_info, param_info_profile;
  boolean enable_v6= 1 ;


  if (enable_v6)
  {
    if(qcmap_cb.ipv6_state == QCMAP_V6_WAN_CONNECTED)
    {
      ret = QCMAP_SUCCESS;
      *qcmap_errno = QCMAP_EALDCONN;
      QCMAP_LOG_MSG_MED("QCMAP Backhaul already connected", 0, 0, 0);
      *qcmap_err_num = QCMAP_ERR_NO_EFFECT;
      return ret;
    }
    else if (qcmap_cb.ipv6_state == QCMAP_V6_WAN_CONNECTING)
    {
      /* We have some outstanding WAN request */
      *qcmap_errno = QCMAP_EOPNOTSUPP;
      ret = QCMAP_ERROR;
      QCMAP_LOG_MSG_MED("QCMAP has outstanding backhaul request", 0, 0, 0);
      *qcmap_err_num = QCMAP_ERR_NO_EFFECT;
      return ret;
    }

    /* Release the previously acquired handle. */
    if (qcmap_cb.ipv6_dss_net_hndl.handle != NULL)
    {
       qapi_DSS_Rel_Data_Srvc_Hndl((qapi_DSS_Hndl_t)qcmap_cb.ipv6_dss_net_hndl.handle);
      qcmap_cb.ipv6_dss_net_hndl.handle = NULL;
      memset(&qcmap_cb.ipv6_dss_net_hndl, 0x0, sizeof(qcmap_cb.ipv6_dss_net_hndl));
    }

    /* Acquire the handle. */
	qapi_DSS_Get_Data_Srvc_Hndl((qapi_DSS_Net_Ev_CB_t)qcmap_dss_net_cb_fcn, (void*) &qcmap_cb.ipv6_dss_net_hndl, &qcmap_cb.ipv6_dss_net_hndl.handle);
    
    if (qcmap_cb.ipv6_dss_net_hndl.handle == NULL)
    {
      *qcmap_errno = QCMAP_EOPNOTSUPP;
      ret = QCMAP_ERROR;
      QCMAP_LOG_MSG_MED("bring_up_ipv6_wwan: Can not get ipv6 dss net handle", 0, 0, 0);
      *qcmap_err_num = QCMAP_ERR_INVALID_HANDLE;
      return ret;
    }

  param_info_profile.buf_val = NULL;
  param_info_profile.num_val = qcmap_file_t.profile;

  if(qcmap_file_t.profile > 0)
  {
    if(qcmap_file_t.profile > DSS_PROFILE_3GPP2_OFFSET)
    {
      qcmap_cb.ipv6_dss_net_hndl.profile = qcmap_file_t.profile - DSS_PROFILE_3GPP2_OFFSET;
      param_info_profile.num_val = qcmap_file_t.profile - DSS_PROFILE_3GPP2_OFFSET;
      qapi_DSS_Set_Data_Call_Param((qapi_DSS_Hndl_t)qcmap_cb.ipv6_dss_net_hndl.handle, 
	  	(qapi_DSS_Call_Param_Identifier_t)QAPI_DSS_CALL_INFO_CDMA_PROFILE_IDX_E,(qapi_DSS_Call_Param_Value_t *)&param_info_profile);
    }

    else
    {
      qcmap_cb.ipv6_dss_net_hndl.profile = qcmap_file_t.profile;
      qapi_DSS_Set_Data_Call_Param((qapi_DSS_Hndl_t)qcmap_cb.ipv6_dss_net_hndl.handle,
	  	(qapi_DSS_Call_Param_Identifier_t)QAPI_DSS_CALL_INFO_UMTS_PROFILE_IDX_E, (qapi_DSS_Call_Param_Value_t *)&param_info_profile);
    }
  }

  /* set data call param */
  param_info.buf_val = NULL;

  if(strcasecmp(qcmap_file_t.call_tech ,"auto") == 0)
  {
   strlcpy(qcmap_cb.ipv6_dss_net_hndl.tech, "AUTO", sizeof(qcmap_file_t.call_tech));
   param_info.num_val = QAPI_DSS_RADIO_TECH_UNKNOWN;
   qapi_DSS_Set_Data_Call_Param((qapi_DSS_Hndl_t)qcmap_cb.ipv6_dss_net_hndl.handle, 
   	(qapi_DSS_Call_Param_Identifier_t)QAPI_DSS_CALL_INFO_TECH_PREF_E, (qapi_DSS_Call_Param_Value_t *)&param_info);
  }

  else if(strcasecmp(qcmap_file_t.call_tech ,"umts") == 0)
  {
   strlcpy(qcmap_cb.ipv6_dss_net_hndl.tech, "UMTS", sizeof(qcmap_file_t.call_tech));
   param_info.num_val = QAPI_DSS_RADIO_TECH_UMTS;
   qapi_DSS_Set_Data_Call_Param((qapi_DSS_Hndl_t)qcmap_cb.ipv6_dss_net_hndl.handle, 
   	(qapi_DSS_Call_Param_Identifier_t)QAPI_DSS_CALL_INFO_TECH_PREF_E, (qapi_DSS_Call_Param_Value_t *)&param_info);
  }

  else if(strcasecmp(qcmap_file_t.call_tech ,"lte") == 0)
  {
   strlcpy(qcmap_cb.ipv6_dss_net_hndl.tech, "LTE", sizeof(qcmap_file_t.call_tech));
   param_info.num_val = QAPI_DSS_RADIO_TECH_LTE;
   qapi_DSS_Set_Data_Call_Param((qapi_DSS_Hndl_t)qcmap_cb.ipv6_dss_net_hndl.handle, 
   	(qapi_DSS_Call_Param_Identifier_t)QAPI_DSS_CALL_INFO_TECH_PREF_E, (qapi_DSS_Call_Param_Value_t *)&param_info);
  }

  else if(strcasecmp(qcmap_file_t.call_tech ,"cdma") == 0)
  {
   strlcpy(qcmap_cb.ipv6_dss_net_hndl.tech, "CDMA", sizeof(qcmap_file_t.call_tech));
   param_info.num_val = QAPI_DSS_RADIO_TECH_CDMA;
   qapi_DSS_Set_Data_Call_Param((qapi_DSS_Hndl_t)qcmap_cb.ipv6_dss_net_hndl.handle, 
   	(qapi_DSS_Call_Param_Identifier_t)QAPI_DSS_CALL_INFO_TECH_PREF_E, (qapi_DSS_Call_Param_Value_t *)&param_info);
  }

  strlcpy(qcmap_cb.dss_net_hndl.family, "1", sizeof(qcmap_cb.dss_net_hndl.family));
  param_info.buf_val = NULL;
  param_info.num_val = QAPI_DSS_IP_VERSION_6;
  qapi_DSS_Set_Data_Call_Param((qapi_DSS_Hndl_t)qcmap_cb.ipv6_dss_net_hndl.handle, 
  	(qapi_DSS_Call_Param_Identifier_t)QAPI_DSS_CALL_INFO_IP_VERSION_E, (qapi_DSS_Call_Param_Value_t *)&param_info);

    QCMAP_LOG_MSG_MED("bring_up_ipv6_wwan: ipv6_dss_net_handle %X", qcmap_cb.ipv6_dss_net_hndl.handle,0,0);


    /* Connecting IPv6 WAN */
    rval = qapi_DSS_Start_Data_Call((qapi_DSS_Hndl_t)qcmap_cb.ipv6_dss_net_hndl.handle);

    if (rval != QAPI_DSS_SUCCESS )
    {
      //Handle Error condition gracefully
    }
    else
    {
      *qcmap_errno = QCMAP_EWOULDBLOCK;
      qcmap_cb.ipv6_state = QCMAP_V6_WAN_CONNECTING;
    }
  }
  else
  {
    ret = QCMAP_ERROR;
    *qcmap_errno = QCMAP_EOPNOTSUPP;
    *qcmap_err_num = QCMAP_ERR_INVALID_HANDLE;
  }

  return ret;
}

/*===========================================================================

FUNCTION BRING_DOWN_IPV4_WWAN()

DESCRIPTION

  It will teardown IPV4 WWAN.

DEPENDENCIES
  None.

RETURN VALUE
  If WAN is already disconnected, returns QCMAP_SUCCESS.
  return QCMAP_ERROR and places the error condition value in
  *qcmap_errno.

  qcmap_errno Values
  ----------------
  QCMAP_EBADAPP           invalid application ID specified
  QCMAP_EWOULDBLOCK       the operation would block
  QCMAP_EOPNOTSUPP        backhaul bringup/teardown in progress


SIDE EFFECTS

===========================================================================*/
int bring_down_ipv4_wwan
(
  int    qcmap_handle,                      /* Handle for Mobile AP CM  */
  int    *qcmap_errno,                      /* error condition value    */
  qcmap_error_type *qcmap_err_num
)
{
  int ret = QCMAP_SUCCESS;

  ds_assert(qcmap_errno != NULL);



  if (qcmap_handle != qcmap_cb.handle)
  {
    *qcmap_errno = QCMAP_EBADAPP;
    ret = QCMAP_ERROR;
    *qcmap_err_num = QCMAP_ERR_INVALID_HANDLE;
    QCMAP_LOG_MSG_ERROR("Wrong QCMAP Handle", 0, 0, 0);
    return ret;
  }

  if (qcmap_cb.state <= QCMAP_ENABLE)
  {
    /* WAN is not connected */
    *qcmap_errno = QCMAP_EALDDISCONN;
    ret = QCMAP_SUCCESS;
    *qcmap_err_num = QCMAP_ERR_NO_EFFECT;
    QCMAP_LOG_MSG_ERROR("QCMAP IPV4 WAN is not connected", 0, 0, 0);
    return ret;
  }
  /* Handle IPV4 disconnect backhaul request gracefully when
     mobile ap disable is processing */
  /* These changes are made to support consecutive usb plug unplug */

  if ((qcmap_cb.state == QCMAP_WAN_CONNECTING || qcmap_cb.state == QCMAP_WAN_DISCONNECTING)&& (qcmap_cb.disable_in_process == false))
  {
    /* We can some outstanding WAN request */
    *qcmap_errno = QCMAP_EOPNOTSUPP;
    ret = QCMAP_ERROR;
    *qcmap_err_num = QCMAP_ERR_NO_EFFECT;
    QCMAP_LOG_MSG_ERROR("QCMAP has outstanding backhaul request", 0, 0, 0);
    return ret;
  }
  /* Checks are added to make sure wan disconnected when mobile ap
     disable is triggered */
  if(qcmap_cb.state == QCMAP_WAN_CONNECTED || ((qcmap_cb.state == QCMAP_WAN_CONNECTING)&& (qcmap_cb.disable_in_process == TRUE)))
  {
    if (qcmap_cb.dss_net_hndl.handle != NULL)
    {
      QCMAP_LOG_MSG_MED("Bring down IPv4 WAN", 0, 0, 0);
      qcmap_cb.state = QCMAP_WAN_DISCONNECTING;
      if(qapi_DSS_Stop_Data_Call((qapi_DSS_Hndl_t)qcmap_cb.dss_net_hndl.handle) == 0)
      QCMAP_LOG_MSG_MED("Successful dss_stop_data_call", 0, 0, 0);
      *qcmap_errno = QCMAP_EWOULDBLOCK;
      ret = QCMAP_ERROR;
    }
    else
    {
      QCMAP_LOG_MSG_ERROR("Invalid Handle: Cannot bring down IPv4" , 0, 0, 0);
      *qcmap_errno = QCMAP_EINVAL;
      ret = QCMAP_ERROR;
      *qcmap_err_num = QCMAP_ERR_INVALID_HANDLE;
    }
  }
  /* return QCMAP_CM_EWOULDBLOCK when disable in process to ensure
     mobile ap disable return success  */
  else if((qcmap_cb.state == QCMAP_WAN_DISCONNECTING)&& (qcmap_cb.disable_in_process == TRUE))
  {
     QCMAP_LOG_MSG_MED("IPV4 call in disconnecting state: Disable process", 0, 0, 0);
     *qcmap_errno = QCMAP_EWOULDBLOCK;
     ret = QCMAP_ERROR;
  }
  else
  {
    *qcmap_errno = QCMAP_EINVAL;
    ret = QCMAP_ERROR;
    *qcmap_err_num = QCMAP_ERR_NO_EFFECT;
    QCMAP_LOG_MSG_ERROR("Cannot bring down IPV4 wwan because of invalid state %d", qcmap_cb.state, 0, 0);
  }

  QCMAP_LOG_MSG_MED("QCMAP AP Handle %04x tear down backhaul.", qcmap_handle, 0, 0);

  return ret;
}

/*===========================================================================

FUNCTION bring_down_ipv6_wwan()

DESCRIPTION

  It will teardown IPV6 WWAN.

DEPENDENCIES
  None.

RETURN VALUE
  If WAN is already disconnected, returns QCMAP_SUCCESS.
  return QCMAP_ERROR and places the error condition value in
  *qcmap_errno.

  qcmap_errno Values
  ----------------
  QCMAP_EBADAPP           invalid application ID specified
  QCMAP_EWOULDBLOCK       the operation would block
  QCMAP_EOPNOTSUPP        backhaul bringup/teardown in progress


SIDE EFFECTS

===========================================================================*/

int bring_down_ipv6_wwan
(
  int    qcmap_handle,                      /* Handle for Mobile AP CM  */
  int    *qcmap_errno,                      /* error condition value    */
  qcmap_error_type *qcmap_err_num
)
{
  int ret = QCMAP_SUCCESS;

  ds_assert(qcmap_errno != NULL);


  if (qcmap_handle != qcmap_cb.handle)
  {
    *qcmap_errno = QCMAP_EBADAPP;
    ret = QCMAP_ERROR;
    *qcmap_err_num = QCMAP_ERR_INVALID_HANDLE;
    QCMAP_LOG_MSG_ERROR("Wrong QCMAP Handle", 0, 0, 0);
    return ret;
  }

  if (qcmap_cb.ipv6_state <= QCMAP_V6_ENABLE)
  {
    /* WAN is not connected */
    *qcmap_errno = QCMAP_EALDDISCONN;
    ret = QCMAP_SUCCESS;
    *qcmap_err_num = QCMAP_ERR_NO_EFFECT;
    QCMAP_LOG_MSG_ERROR("QCMAP IPV6 WAN is not connected", 0, 0, 0);
    return ret;
  }
  /* Handle IPV6 disconnect backhaul request gracefully when
     mobile ap disable is processing */
  /* These changes are made to support consecutive usb plug unplug */

  if ((qcmap_cb.ipv6_state == QCMAP_V6_WAN_CONNECTING || qcmap_cb.ipv6_state == QCMAP_V6_WAN_DISCONNECTING)&& (qcmap_cb.disable_in_process == false))
  {
    /* We can some outstanding WAN request */
    *qcmap_errno = QCMAP_EOPNOTSUPP;
    ret = QCMAP_ERROR;
    *qcmap_err_num = QCMAP_ERR_NO_EFFECT;
    QCMAP_LOG_MSG_ERROR("QCMAP has outstanding backhaul request", 0, 0, 0);
    return ret;
  }
  /* Checks are added to make sure wan disconnected when mobile ap
     disable is triggered */
  if((qcmap_cb.ipv6_state == QCMAP_V6_WAN_CONNECTED) || ((qcmap_cb.ipv6_state == QCMAP_V6_WAN_CONNECTING)&& (qcmap_cb.disable_in_process == TRUE)))
  {
    if (qcmap_cb.ipv6_dss_net_hndl.handle != NULL)
    {
      qcmap_cb.ipv6_state = QCMAP_V6_WAN_DISCONNECTING;
      qapi_DSS_Stop_Data_Call((qapi_DSS_Hndl_t)qcmap_cb.ipv6_dss_net_hndl.handle);
      QCMAP_LOG_MSG_MED("After dss_stop_data_call", 0, 0, 0);
      *qcmap_errno = QCMAP_EWOULDBLOCK;
      ret = QCMAP_ERROR;
    }
    else
    {
      QCMAP_LOG_MSG_MED("Invalid Handle: Cannot bring down IPv6", 0, 0, 0);
      *qcmap_errno = QCMAP_EINVAL;
      ret = QCMAP_ERROR;
      *qcmap_err_num = QCMAP_ERR_INVALID_HANDLE;
    }
  }
  /* return QCMAP_CM_EWOULDBLOCK when disable in process to ensure
     mobile ap disable return success  */
  else if((qcmap_cb.state == QCMAP_V6_WAN_DISCONNECTING)&& (qcmap_cb.disable_in_process == TRUE))
  {
     QCMAP_LOG_MSG_MED("IPV6 call in disconnecting state: Disable process", 0, 0,0 );
     *qcmap_errno = QCMAP_EWOULDBLOCK;
     ret = QCMAP_ERROR;
  }
  else
  {
    *qcmap_errno = QCMAP_EINVAL;
    ret = QCMAP_ERROR;
    *qcmap_err_num = QCMAP_ERR_NO_EFFECT;
    QCMAP_LOG_MSG_ERROR("Cannot bring down IPV6 wwan because of invalid state %d", qcmap_cb.ipv6_state, 0, 0);
  }

  QCMAP_LOG_MSG_MED("QCMAP AP Handle %04x tear down backhaul.", qcmap_handle, 0, 0);

  return ret;
}

/*===========================================================================

FUNCTION QCMAP_CONNECT_BACKHAUL()

DESCRIPTION

  It will bringup WWAN.

DEPENDENCIES
  None.

RETURN VALUE
  If WAN is already connected, returns QCMAP_SUCCESS.
  Otherwise, return QCMAP_ERROR and places the error condition value in
  *qcmap_errno.

  qcmap_errno Values
  ----------------
  QCMAP_EBADAPP           invalid application ID specified
  QCMAP_EWOULDBLOCK       the operation would block
  QCMAP_EOPNOTSUPP        backhaul bringup/teardown in progress

SIDE EFFECTS
===========================================================================*/
int qcmap_connect_backhaul
(
  int  qcmap_handle,                          /* Handle for MobileAP CM */
  qcmap_msgr_wwan_call_type_v01    call_type, /* Call type to be brought UP. */
  int *qcmap_errno,                            /* Error condition value  */
  qcmap_error_type *qcmap_err_num
)
{
  int ret = QCMAP_SUCCESS;

  ds_assert(qcmap_errno != NULL);
  ds_assert(qcmap_err_num != NULL);



  if (qcmap_handle != qcmap_cb.handle)
  {
    *qcmap_errno = QCMAP_EBADAPP;
    ret = QCMAP_ERROR;
    *qcmap_err_num = QCMAP_ERR_INVALID_HANDLE;
    QCMAP_LOG_MSG_ERROR("Wrong QCMAP CM Handle", 0, 0, 0);
    return ret;
  }

  /* Skip NAS check for modem loopback call. */
  if ( !modem_loopback_mode )
  {
    if (get_nas_config(qcmap_handle, qcmap_errno, qcmap_err_num) != QCMAP_SUCCESS)
    {
      ret = QCMAP_ERROR;

      QCMAP_LOG_MSG_ERROR("get_nas_config() returned error!!", 0, 0, 0);
      return ret;
    }
  }

  *qcmap_errno = QCMAP_EWOULDBLOCK;

  switch ( call_type )
  {
    case QCMAP_MSGR_WWAN_CALL_TYPE_V4_V01:
    {
      if (bring_up_ipv4_wwan(qcmap_errno, qcmap_err_num) != QCMAP_SUCCESS)
      {
        ret = QCMAP_ERROR;
        QCMAP_LOG_MSG_ERROR("bring_up_ipv4_wwan() returned error!!", 0, 0, 0);
      }
      break;
    }
    case QCMAP_MSGR_WWAN_CALL_TYPE_V6_V01:
    {
      if (bring_up_ipv6_wwan(qcmap_errno, qcmap_err_num) != QCMAP_SUCCESS)
      {
        ret = QCMAP_ERROR;
        QCMAP_LOG_MSG_ERROR("bring_up_ipv6_wwan() returned error!!", 0, 0, 0);
      }
      break;
    }
    default:
      *qcmap_errno = QCMAP_EINVAL;
      ret = QCMAP_ERROR;
      *qcmap_err_num = QCMAP_ERR_INVALID_IP_FAMILY_PREF;
      QCMAP_LOG_MSG_ERROR("ConnectBackhaul() Invalid Call Type!!", 0, 0, 0);
  }

  QCMAP_LOG_MSG_MED("QCMAP AP Handle 0x%04x Connecting Backhaul.", qcmap_handle, 0, 0);

  return ret;
}

/*===========================================================================

FUNCTION QCMAP_DISCONNECT_BACKHAUL()

DESCRIPTION

  It will teardown WWAN.

DEPENDENCIES
  None.

RETURN VALUE
  If WAN is already disconnected, returns QCMAP_SUCCESS.
  return QCMAP_ERROR and places the error condition value in
  *qcmap_errno.

  qcmap_errno Values
  ----------------
  QCMAP_EBADAPP           invalid application ID specified
  QCMAP_EWOULDBLOCK       the operation would block
  QCMAP_EOPNOTSUPP        backhaul bringup/teardown in progress


SIDE EFFECTS

===========================================================================*/
int qcmap_disconnect_backhaul
(
  int  qcmap_handle,                          /* Handle for MobileAP CM */
  qcmap_msgr_wwan_call_type_v01    call_type, /* Call type to be brought down. */
  int *qcmap_errno,                           /* Error condition value  */
  qcmap_error_type *qcmap_err_num
)
{
  int ret = QCMAP_SUCCESS;

  ds_assert(qcmap_errno != NULL);
  ds_assert(qcmap_err_num != NULL);



  if (qcmap_handle != qcmap_cb.handle)
  {
    *qcmap_errno = QCMAP_EBADAPP;
    ret = QCMAP_ERROR;
    *qcmap_err_num = QCMAP_ERR_INVALID_HANDLE;
    QCMAP_LOG_MSG_ERROR("Wrong QCMAP Handle", 0, 0, 0);
    return ret;
  }

  if (qcmap_cb.state <= QCMAP_ENABLE && qcmap_cb.ipv6_state <= QCMAP_V6_ENABLE)
  {
    /* WAN is not connected */
    *qcmap_errno = QCMAP_EALDDISCONN;
    ret = QCMAP_SUCCESS;
    *qcmap_err_num = QCMAP_ERR_NO_EFFECT;
    QCMAP_LOG_MSG_ERROR("QCMAP WAN is not connected", 0, 0, 0);
    return ret;
  }

  switch ( call_type )
  {
    case QCMAP_MSGR_WWAN_CALL_TYPE_V4_V01:
    {
      if (bring_down_ipv4_wwan(qcmap_handle, qcmap_errno, qcmap_err_num) != QCMAP_SUCCESS)
      {
        ret = QCMAP_ERROR;
        QCMAP_LOG_MSG_ERROR("bring_down_ipv4_wwan() returned error!!", 0, 0, 0);
      }
      break;
    }
    case QCMAP_MSGR_WWAN_CALL_TYPE_V6_V01:
    {
      if (bring_down_ipv6_wwan(qcmap_handle, qcmap_errno, qcmap_err_num) != QCMAP_SUCCESS)
      {
        ret = QCMAP_ERROR;
        QCMAP_LOG_MSG_ERROR("bring_down_ipv6_wwan() returned error!!", 0, 0, 0);
      }
      break;
    }
    default:
      *qcmap_errno = QCMAP_EINVAL;
      ret = QCMAP_ERROR;
      *qcmap_err_num = QCMAP_ERR_INVALID_IP_FAMILY_PREF;
      QCMAP_LOG_MSG_ERROR("DisconnectBackhaul() Invalid Call Type!!", 0, 0, 0);
  }

  QCMAP_LOG_MSG_MED("QCMAP AP Handle %04x tear down backhaul.", qcmap_handle, 0, 0);

  return ret;
}



int get_nas_config
(
  int  qcmap_handle,   /* Handle for MobileAP CM */
  int *qcmap_errno,     /* Error condition value  */
  qcmap_error_type *qcmap_err_num
)
{
  int ret = QCMAP_SUCCESS;
  qmi_client_error_type qmi_error;
  nas_get_sys_info_resp_msg_v01 get_sys_info_resp_msg;
  nas_roam_status_enum_type_v01 roam_status = NAS_ROAM_STATUS_ENUM_TYPE_MIN_ENUM_VAL_V01;
  nas_roam_status_enum_type_v01 roam_status_hdr  = NAS_ROAM_STATUS_ENUM_TYPE_MIN_ENUM_VAL_V01;
  nas_roam_status_enum_type_v01 roam_status_cdma = NAS_ROAM_STATUS_ENUM_TYPE_MIN_ENUM_VAL_V01;
  // check whether had backhaul service

  /* Call QMI NAS service to get system info. */
  nas_service_domain_enum_type_v01 srv_domain         = NAS_SERVICE_DOMAIN_ENUM_TYPE_MIN_ENUM_VAL_V01;
  nas_service_domain_enum_type_v01 srv_domain_lte     = NAS_SERVICE_DOMAIN_ENUM_TYPE_MIN_ENUM_VAL_V01;
  nas_service_domain_enum_type_v01 srv_domain_hdr     = NAS_SERVICE_DOMAIN_ENUM_TYPE_MIN_ENUM_VAL_V01;
  nas_service_domain_enum_type_v01 srv_domain_cdma    = NAS_SERVICE_DOMAIN_ENUM_TYPE_MIN_ENUM_VAL_V01;
  nas_service_domain_enum_type_v01 srv_domain_wcdma   = NAS_SERVICE_DOMAIN_ENUM_TYPE_MIN_ENUM_VAL_V01;
  nas_service_domain_enum_type_v01 srv_domain_gsm     = NAS_SERVICE_DOMAIN_ENUM_TYPE_MIN_ENUM_VAL_V01;
  nas_service_domain_enum_type_v01 srv_domain_tdscdma = NAS_SERVICE_DOMAIN_ENUM_TYPE_MIN_ENUM_VAL_V01;
  nas_service_status_enum_type_v01 srv_status         = NAS_SERVICE_STATUS_ENUM_TYPE_MIN_ENUM_VAL_V01;
  nas_service_status_enum_type_v01 srv_status_lte     = NAS_SERVICE_STATUS_ENUM_TYPE_MIN_ENUM_VAL_V01;
  nas_service_status_enum_type_v01 srv_status_hdr     = NAS_SERVICE_STATUS_ENUM_TYPE_MIN_ENUM_VAL_V01;
  nas_service_status_enum_type_v01 srv_status_cdma    = NAS_SERVICE_STATUS_ENUM_TYPE_MIN_ENUM_VAL_V01;
  nas_service_status_enum_type_v01 srv_status_wcdma   = NAS_SERVICE_STATUS_ENUM_TYPE_MIN_ENUM_VAL_V01;
  nas_service_status_enum_type_v01 srv_status_gsm     = NAS_SERVICE_STATUS_ENUM_TYPE_MIN_ENUM_VAL_V01;
  nas_service_status_enum_type_v01 srv_status_tdscdma = NAS_SERVICE_STATUS_ENUM_TYPE_MIN_ENUM_VAL_V01;

  memset(&get_sys_info_resp_msg, 0 , sizeof(nas_get_sys_info_resp_msg_v01));
  qmi_error = qmi_client_send_msg_sync( qcmap_cb.qmi_nas_handle,
                                        QMI_NAS_GET_SYS_INFO_REQ_MSG_V01,
                                        NULL,
                                        0,
                                        &get_sys_info_resp_msg,
                                        sizeof(get_sys_info_resp_msg),
                                        QCMAP_QMI_TIMEOUT_VALUE);
  if (qmi_error != QMI_NO_ERR)
  {
    *qcmap_errno = QCMAP_EOPNOTSUPP;
    ret = QCMAP_ERROR;
    *qcmap_err_num = QCMAP_ERR_INTERNAL;
    QCMAP_LOG_MSG_ERROR( "QCMAP nas get sys info req msg fail %d", qmi_error, 0,0 );
    return ret;
  }
  else
  {
    /* Local domain and status variables */
    srv_domain_lte     = get_sys_info_resp_msg.lte_sys_info.common_sys_info.srv_domain;
    srv_domain_hdr     = get_sys_info_resp_msg.hdr_sys_info.common_sys_info.srv_domain;
    srv_domain_cdma    = get_sys_info_resp_msg.cdma_sys_info.common_sys_info.srv_domain;
    srv_domain_wcdma   = get_sys_info_resp_msg.wcdma_sys_info.common_sys_info.srv_domain;
    srv_domain_gsm     = get_sys_info_resp_msg.gsm_sys_info.common_sys_info.srv_domain;
    srv_domain_tdscdma = get_sys_info_resp_msg.tdscdma_sys_info.common_sys_info.srv_domain;
    srv_status_lte     = get_sys_info_resp_msg.lte_srv_status_info.srv_status;
    srv_status_hdr     = get_sys_info_resp_msg.hdr_srv_status_info.srv_status;
    srv_status_cdma    = get_sys_info_resp_msg.cdma_srv_status_info.srv_status;
    srv_status_wcdma   = get_sys_info_resp_msg.wcdma_srv_status_info.srv_status;
    srv_status_gsm     = get_sys_info_resp_msg.gsm_srv_status_info.srv_status;
    srv_status_tdscdma = get_sys_info_resp_msg.tdscdma_srv_status_info.srv_status;
    /* First Get the Service Domain. */
    /* If the LTE System Info is valid, check the LTE Service Domain. */
    if (get_sys_info_resp_msg.lte_sys_info_valid == TRUE &&
        get_sys_info_resp_msg.lte_sys_info.common_sys_info.srv_domain_valid == TRUE &&
        srv_status_lte == NAS_SYS_SRV_STATUS_SRV_V01 &&
        (srv_domain_lte == SYS_SRV_DOMAIN_PS_ONLY_V01 || srv_domain_lte == SYS_SRV_DOMAIN_CS_PS_V01))
    {
      srv_domain = get_sys_info_resp_msg.lte_sys_info.common_sys_info.srv_domain;
      srv_status = get_sys_info_resp_msg.lte_srv_status_info.srv_status;
      if (get_sys_info_resp_msg.lte_sys_info.common_sys_info.roam_status_valid == TRUE)
      {
        roam_status = get_sys_info_resp_msg.lte_sys_info.common_sys_info.roam_status;
      }
      QCMAP_LOG_MSG_MED("lte Service Domain %d Status %d, roam status %d", srv_domain, srv_status, roam_status);
    }
    /* If the HDR System Info is valid, check the HDR Service Domain. */
    else if (get_sys_info_resp_msg.hdr_sys_info_valid == TRUE &&
        get_sys_info_resp_msg.hdr_sys_info.common_sys_info.srv_domain_valid == TRUE &&
        srv_status_hdr == NAS_SYS_SRV_STATUS_SRV_V01 &&
        (srv_domain_hdr == SYS_SRV_DOMAIN_PS_ONLY_V01 || srv_domain_hdr == SYS_SRV_DOMAIN_CS_PS_V01))
    {
      srv_domain = get_sys_info_resp_msg.hdr_sys_info.common_sys_info.srv_domain;
      srv_status = get_sys_info_resp_msg.hdr_srv_status_info.srv_status;
      if (get_sys_info_resp_msg.hdr_sys_info.common_sys_info.roam_status_valid == TRUE)
      {
        roam_status = get_sys_info_resp_msg.hdr_sys_info.common_sys_info.roam_status;
        roam_status_hdr = get_sys_info_resp_msg.hdr_sys_info.common_sys_info.roam_status;
        QCMAP_LOG_MSG_MED("roam_status_hdr  %d", roam_status_hdr, 0, 0);
      }
      QCMAP_LOG_MSG_MED("HDR Service Domain %d Status %d, roam status %d", srv_domain, srv_status, roam_status);
    }
    /* If the CDMA System Info is valid, check the CDMA Service Domain. */
    else if (get_sys_info_resp_msg.cdma_sys_info_valid == TRUE &&
        get_sys_info_resp_msg.cdma_sys_info.common_sys_info.srv_domain_valid == TRUE &&
        srv_status_cdma == NAS_SYS_SRV_STATUS_SRV_V01 &&
        (srv_domain_cdma == SYS_SRV_DOMAIN_PS_ONLY_V01 || srv_domain_cdma == SYS_SRV_DOMAIN_CS_PS_V01))
    {
      srv_domain = get_sys_info_resp_msg.cdma_sys_info.common_sys_info.srv_domain;
      srv_status = get_sys_info_resp_msg.cdma_srv_status_info.srv_status;
      if (get_sys_info_resp_msg.cdma_sys_info.common_sys_info.roam_status_valid == TRUE)
      {
        roam_status = get_sys_info_resp_msg.cdma_sys_info.common_sys_info.roam_status;
        roam_status_cdma = get_sys_info_resp_msg.cdma_sys_info.common_sys_info.roam_status;
        QCMAP_LOG_MSG_MED("roam_status_cdma  %d", roam_status_cdma, 0, 0);
      }
      QCMAP_LOG_MSG_MED("CDMA Service Domain %d Status %d, roam status %d", srv_domain, srv_status, roam_status);
    }
    /* If the WCDMA System Info is valid, check the WCDMA Service Domain. */
    else if (get_sys_info_resp_msg.wcdma_sys_info_valid == TRUE &&
        get_sys_info_resp_msg.wcdma_sys_info.common_sys_info.srv_domain_valid == TRUE &&
        srv_status_wcdma == NAS_SYS_SRV_STATUS_SRV_V01 &&
        (srv_domain_wcdma == SYS_SRV_DOMAIN_PS_ONLY_V01 || srv_domain_wcdma == SYS_SRV_DOMAIN_CS_PS_V01))
    {
      srv_domain = get_sys_info_resp_msg.wcdma_sys_info.common_sys_info.srv_domain;
      srv_status = get_sys_info_resp_msg.wcdma_srv_status_info.srv_status;
      if (get_sys_info_resp_msg.wcdma_sys_info.common_sys_info.roam_status_valid == TRUE)
      {
        roam_status = get_sys_info_resp_msg.wcdma_sys_info.common_sys_info.roam_status;
      }
      QCMAP_LOG_MSG_MED("WCDMA Service Domain %d Status %d, roam status %d", srv_domain, srv_status, roam_status);
    }
    /* If the GSM System Info is valid, check the GSM Service Domain. */
    else if (get_sys_info_resp_msg.gsm_sys_info_valid == TRUE &&
        get_sys_info_resp_msg.gsm_sys_info.common_sys_info.srv_domain_valid == TRUE &&
        srv_status_gsm == NAS_SYS_SRV_STATUS_SRV_V01 &&
        (srv_domain_gsm == SYS_SRV_DOMAIN_PS_ONLY_V01 || srv_domain_gsm == SYS_SRV_DOMAIN_CS_PS_V01))
    {
      srv_domain = get_sys_info_resp_msg.gsm_sys_info.common_sys_info.srv_domain;
      srv_status = get_sys_info_resp_msg.gsm_srv_status_info.srv_status;
      if (get_sys_info_resp_msg.gsm_sys_info.common_sys_info.roam_status_valid == TRUE)
      {
        roam_status = get_sys_info_resp_msg.gsm_sys_info.common_sys_info.roam_status;
      }
      QCMAP_LOG_MSG_MED("GSM Service Domain %d Status %d, roam status %d", srv_domain, srv_status, roam_status);
    }
    /* If the TDSCDMA System Info is valid, check the TDSCDMA Service Domain. */
    else if (get_sys_info_resp_msg.tdscdma_sys_info_valid == TRUE &&
        get_sys_info_resp_msg.tdscdma_sys_info.common_sys_info.srv_domain_valid == TRUE &&
        srv_status_tdscdma == NAS_SYS_SRV_STATUS_SRV_V01 &&
        (srv_domain_tdscdma == SYS_SRV_DOMAIN_PS_ONLY_V01 || srv_domain_tdscdma == SYS_SRV_DOMAIN_CS_PS_V01))
    {
      srv_domain = get_sys_info_resp_msg.tdscdma_sys_info.common_sys_info.srv_domain;
      srv_status = get_sys_info_resp_msg.tdscdma_srv_status_info.srv_status;
      if (get_sys_info_resp_msg.tdscdma_sys_info.common_sys_info.roam_status_valid == TRUE)
      {
        roam_status = get_sys_info_resp_msg.tdscdma_sys_info.common_sys_info.roam_status;
      }
      QCMAP_LOG_MSG_MED("TDSCDMA Service Domain %d Status %d, roam status %d", srv_domain, srv_status, roam_status);
    }

    if ((srv_status != NAS_SYS_SRV_STATUS_SRV_V01) ||
        (srv_domain != SYS_SRV_DOMAIN_PS_ONLY_V01 && srv_domain != SYS_SRV_DOMAIN_CS_PS_V01))
    {
      *qcmap_errno = QCMAP_EOPNOTSUPP;
      ret = QCMAP_ERROR;
      *qcmap_err_num = QCMAP_ERR_NO_NETWORK_FOUND;
      QCMAP_LOG_MSG_ERROR("QCMAP has no backhaul service", 0, 0, 0);
      return ret;
    }

    /* QCMAP is on an active network. */
    qcmap_cb.backhaul_service = TRUE;

    if(roam_status != NAS_ROAM_STATUS_ENUM_TYPE_MIN_ENUM_VAL_V01)
    {
      qcmap_cb.roaming_indicator = roam_status;
    }
    else
    {
      qcmap_cb.roaming_indicator = 0;
    }
  }

    if(qcmap_cb.backhaul_service)
    {
      if((qc_bck_wan->wwan_cfg.roaming == FALSE) &&
         (qcmap_cb.roaming_indicator >= NAS_SYS_ROAM_STATUS_ON_V01))
      {
        *qcmap_errno = QCMAP_EOPNOTSUPP;
        QCMAP_LOG_MSG_ERROR("QCMAP can not backhaul while it is in roaming", 0, 0, 0);
        *qcmap_err_num = QCMAP_ERR_NO_NETWORK_FOUND;
        return QCMAP_FAILURE;
      }
    }
  return QCMAP_SUCCESS;
}

/*===========================================================================
  FUNCTION QCMAP_BRING_UP_WWAN()

  DESCRIPTION
    Bring up WWAN connection in MobileAP mode.

  PARAMETERS
    data - Data from client
    conn_handle: Used to identify client

  RETURN VALUE
    None

  DEPENDENCIES
    qcmap_client_reg_cb should be called

  SIDE EFFECTS
    None
===========================================================================*/

void qcmap_bring_up_wwan
(
  void* data,
  uint8 conn_handle
)
{

  qcmap_msgr_bring_up_wwan_req_msg_v01   *req_ptr = NULL;
  qcmap_msgr_bring_up_wwan_resp_msg_v01  *resp_msg;
  qcmap_softap_handle_type               *map_handle;
  qcmap_msgr_wwan_call_type_v01 call_type = QCMAP_MSGR_WWAN_CALL_TYPE_V4_V01;
  boolean            retval;
  unsigned int       index;
  int                err_num=0;
  qcmap_error_type qcmap_err_num;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(data != NULL);

  resp_msg = (qcmap_msgr_bring_up_wwan_resp_msg_v01 *)malloc(sizeof(qcmap_msgr_bring_up_wwan_resp_msg_v01));
  if (resp_msg == NULL)
  {
    QCMAP_LOG_MSG_ERROR("Out of mem for resp msg", 0, 0, 0);
    ASSERT(0);
  }
  memset(resp_msg, 0, sizeof(qcmap_msgr_bring_up_wwan_resp_msg_v01));

  req_ptr = (qcmap_msgr_bring_up_wwan_req_msg_v01 *)data;

  /*---------------------------------------------------------------------
        Start the network interface for this client
  ---------------------------------------------------------------------*/
  index = req_ptr->mobile_ap_handle;
  map_handle = &qcmap_handle;

  if (index != QCMAP_SOFTAP_HANDLE || map_handle->handle == 0)
  {
    QCMAP_LOG_MSG_ERROR("Incorrect handle %d passed", req_ptr->mobile_ap_handle, 0, 0);
    resp_msg->resp.result = QCMAP_RESULT_FAILURE;
    resp_msg->resp.error = QCMAP_ERR_INVALID_HANDLE;
    qcmap_send_msg(QCMAP_MOBILEAP_BRING_UP_WWAN_RESP_EVT,(void *) resp_msg,conn_handle);

    if(resp_msg != NULL)
    {
      free(resp_msg);
      resp_msg = NULL;
    }
    if(req_ptr != NULL)
    {
      free(req_ptr);
      req_ptr = NULL;
    }
    return ;
  }

  if ( req_ptr->call_type_valid )
  {
    call_type = req_ptr->call_type;
  }

  QCMAP_LOG_MSG_MED ("Client %d starting WWAN call in MobileAP inst %d", conn_handle, index, 0);

  retval = ConnectBackHaul(call_type, &err_num, &qcmap_err_num);
  if (!retval)
  {
    QCMAP_LOG_MSG_ERROR("WWAN bringup failed calltype %d ret %d qmierr %d", call_type,retval,qcmap_err_num);
    if ( call_type == QCMAP_MSGR_WWAN_CALL_TYPE_V6_V01 )
    {
      resp_msg->conn_status = QCMAP_MSGR_WWAN_STATUS_IPV6_CONNECTING_FAIL_V01;
    }
    else
    {
      resp_msg->conn_status = QCMAP_MSGR_WWAN_STATUS_CONNECTING_FAIL_V01;
    }
    resp_msg->resp.result = QCMAP_RESULT_FAILURE;
    resp_msg->resp.error = qcmap_err_num;
    qcmap_send_msg(QCMAP_MOBILEAP_BRING_UP_WWAN_RESP_EVT,(void *) resp_msg,conn_handle);

    if(resp_msg != NULL)
    {
      free(resp_msg);
      resp_msg = NULL;
    }
    if(req_ptr != NULL)
    {
      free(req_ptr);
      req_ptr = NULL;
    }
    return ;
  }

  if (err_num == QCMAP_EALDCONN)
  {
    QCMAP_LOG_MSG_MED("WWAN Already connected!!", 0, 0, 0);
    if ( call_type == QCMAP_MSGR_WWAN_CALL_TYPE_V6_V01 )
    {
      resp_msg->conn_status = QCMAP_MSGR_WWAN_STATUS_IPV6_CONNECTED_V01;
    }
    else
    {
      resp_msg->conn_status = QCMAP_MSGR_WWAN_STATUS_CONNECTED_V01;
    }
    resp_msg->conn_status_valid = TRUE;
    qcmap_send_msg(QCMAP_MOBILEAP_BRING_UP_WWAN_RESP_EVT,(void *) resp_msg,conn_handle);

    if(resp_msg != NULL)
    {
      free(resp_msg);
      resp_msg = NULL;
    }
    if(req_ptr != NULL)
    {
      free(req_ptr);
      req_ptr = NULL;
    }
    return;
  }

  if ( call_type == QCMAP_MSGR_WWAN_CALL_TYPE_V6_V01 )
  {
    resp_msg->conn_status = QCMAP_MSGR_WWAN_STATUS_IPV6_CONNECTING_V01;
  }
  else
  {
    resp_msg->conn_status = QCMAP_MSGR_WWAN_STATUS_CONNECTING_V01;
  }
  qcmap_send_msg(QCMAP_MOBILEAP_BRING_UP_WWAN_RESP_EVT,(void *) resp_msg,conn_handle);

  /* Set pending to TRUE to notify client with indication
   * once final WWAN state is received
   *  */
  qcmap_client_info[conn_handle].wwan_op_ind_pending = TRUE;
  qcmap_client_info[conn_handle].wwan_op_ind_msg_id = QCMAP_BRING_UP_WWAN_IND_V01;
 
  if(resp_msg != NULL)
  {
    free(resp_msg);
    resp_msg = NULL;
  }
  if(req_ptr != NULL)
  {
    free(req_ptr);
    req_ptr = NULL;
  }
  return ;

}

/*===========================================================================
  FUNCTION QCMAP_TEAR_DOWN_WWAN()

  DESCRIPTION
    Tear down currently active WWAN connection.

  PARAMETERS
    data - Data from client
    conn_handle: Used to identify client

  RETURN VALUE
    None

  DEPENDENCIES
    qcmap_client_reg_cb should be called

  SIDE EFFECTS
    None
===========================================================================*/

void qcmap_tear_down_wwan
(
  void* data,
  uint8 conn_handle
)
{
  qcmap_msgr_tear_down_wwan_req_msg_v01  *req_ptr = NULL;
  qcmap_msgr_tear_down_wwan_resp_msg_v01 *resp_msg;
  qcmap_softap_handle_type               *map_handle;
  qcmap_msgr_wwan_call_type_v01 call_type = QCMAP_MSGR_WWAN_CALL_TYPE_V4_V01;
  int                err_num=0;
  unsigned int             index;
  qcmap_error_type qcmap_err_num;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(data != NULL);

  resp_msg = (qcmap_msgr_tear_down_wwan_resp_msg_v01 *)malloc(sizeof(qcmap_msgr_tear_down_wwan_resp_msg_v01));
  if (resp_msg == NULL)
  {
    QCMAP_LOG_MSG_ERROR("Out of mem for resp msg", 0, 0, 0);
    ASSERT(0);
  }
  memset(resp_msg, 0, sizeof(qcmap_msgr_tear_down_wwan_resp_msg_v01));

  req_ptr = (qcmap_msgr_tear_down_wwan_req_msg_v01 *)data;

  /*---------------------------------------------------------------------
      Tear down WWAN for this client
  ---------------------------------------------------------------------*/
  index = (req_ptr->mobile_ap_handle);
  map_handle = &qcmap_handle;

  if (index != QCMAP_SOFTAP_HANDLE || map_handle->handle == 0)
  {
    QCMAP_LOG_MSG_MED("Incorrect handle %d passed", req_ptr->mobile_ap_handle, 0, 0);
    resp_msg->resp.result = QCMAP_RESULT_FAILURE;
    resp_msg->resp.error = QCMAP_ERR_INVALID_HANDLE;
    qcmap_send_msg(QCMAP_MOBILEAP_TEAR_DOWN_WWAN_RESP_EVT,(void *) resp_msg,conn_handle);

    if(resp_msg != NULL)
    {
      free(resp_msg);
      resp_msg = NULL;
    }
    if(req_ptr != NULL)
    {
      free(req_ptr);
      req_ptr = NULL;
    }
    return ;
  }

  if ( req_ptr->call_type_valid )
  {
    call_type = req_ptr->call_type;
  }

  QCMAP_LOG_MSG_MED("Client %d ending WWAN call in MobileAP inst %d", conn_handle, index, 0);

  if (!DisconnectBackHaul(call_type, &err_num, &qcmap_err_num))
  {
    QCMAP_LOG_MSG_ERROR("WWAN teardown failed", 0, 0, 0);
    if ( call_type == QCMAP_MSGR_WWAN_CALL_TYPE_V6_V01 )
    {
      resp_msg->conn_status = QCMAP_MSGR_WWAN_STATUS_IPV6_DISCONNECTING_FAIL_V01;
    }
    else
    {
      resp_msg->conn_status = QCMAP_MSGR_WWAN_STATUS_DISCONNECTING_FAIL_V01;
    }
    resp_msg->resp.result = QCMAP_RESULT_FAILURE;
    resp_msg->resp.error = qcmap_err_num;
    qcmap_send_msg(QCMAP_MOBILEAP_TEAR_DOWN_WWAN_RESP_EVT,(void *) resp_msg,conn_handle);

    if(resp_msg != NULL)
    {
      free(resp_msg);
      resp_msg = NULL;
    }
    if(req_ptr != NULL)
    {
      free(req_ptr);
      req_ptr = NULL;
    }
    return;
  }

  if (err_num == QCMAP_EALDDISCONN)
  {
    QCMAP_LOG_MSG_MED("WWAN Already disconnected!!", 0, 0, 0);
    if ( call_type == QCMAP_MSGR_WWAN_CALL_TYPE_V6_V01 )
    {
      resp_msg->conn_status = QCMAP_MSGR_WWAN_STATUS_IPV6_DISCONNECTED_V01;
    }
    else
    {
      resp_msg->conn_status = QCMAP_MSGR_WWAN_STATUS_DISCONNECTED_V01;
    }
    resp_msg->conn_status_valid = TRUE;
    qcmap_send_msg(QCMAP_MOBILEAP_TEAR_DOWN_WWAN_RESP_EVT,(void *) resp_msg,conn_handle);

    if(resp_msg != NULL)
    {
      free(resp_msg);
      resp_msg = NULL;
    }
    if(req_ptr != NULL)
    {
      free(req_ptr);
      req_ptr = NULL;
    }
    return;
  }
  if ( call_type == QCMAP_MSGR_WWAN_CALL_TYPE_V6_V01 )
  {
    resp_msg->conn_status = QCMAP_MSGR_WWAN_STATUS_IPV6_DISCONNECTING_V01;
  }
  else
  {
    resp_msg->conn_status = QCMAP_MSGR_WWAN_STATUS_DISCONNECTING_V01;
  }
  qcmap_send_msg(QCMAP_MOBILEAP_TEAR_DOWN_WWAN_RESP_EVT,(void *) resp_msg,conn_handle);

  /* Set pending to TRUE to notify client with indication
   * once final WWAN state is received
   *  */
  qcmap_client_info[conn_handle].wwan_op_ind_pending = TRUE;
  qcmap_client_info[conn_handle].wwan_op_ind_msg_id = QCMAP_TEAR_DOWN_WWAN_IND_V01;
 
  if(resp_msg != NULL)
  {
    free(resp_msg);
    resp_msg = NULL;
  }
  if(req_ptr != NULL)
  {
    free(req_ptr);
    req_ptr = NULL;
  }
  return ;

}

/*===========================================================================
  FUNCTION ConnectBackHaul
==========================================================================*/
/*!
@brief
  Tries to connect to backhaul. As a part of connect to backhaul checks for the
  network service and nas configuration.
  If the network is in roaming mode,  connects to backhaul based on the roaming flag configuration.

@parameters
  qcmap_msgr_wwan_call_type_v01 call_type
  int                           *err_num

@return
  true  - on success
  false - on failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/

boolean ConnectBackHaul
(
  qcmap_msgr_wwan_call_type_v01 call_type,
  int *err_num,
  qcmap_error_type *qcmap_err_num
)
{
  int qcmap_errno;
  int ret_val;


  QCMAP_LOG_MSG_MED("In ConnectBackHaul %x Call Type:%d", qc_mgr->qcmap_cm_handle, call_type, 0);
  ret_val = qcmap_connect_backhaul(qc_mgr->qcmap_cm_handle, call_type, &qcmap_errno, qcmap_err_num);

  if (ret_val == QCMAP_SUCCESS)
  {
    *err_num = qcmap_errno;
    return true;
  }

  if (ret_val == QCMAP_ERROR && qcmap_errno == QCMAP_EWOULDBLOCK)
  {
    QCMAP_LOG_MSG_MED("QCMAP WAN Connecting Inprogress ", 0, 0, 0);
    return true;
  }

  else
  {
    QCMAP_LOG_MSG_ERROR( "QCMAP WAN Connecting Fail, ret_val %d qcmap_errno %d", ret_val, qcmap_errno, 0 );
    return false;
  }
}


/*===========================================================================
  FUNCTION DisconnectBackHaul
==========================================================================*/
/*!
@brief
  Brings down the bachaul service.

@parameters
  qcmap_msgr_wwan_call_type_v01 call_type
  int                           *err_num

@return
  true  - on success
  false - on failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/

boolean DisconnectBackHaul
(
  qcmap_msgr_wwan_call_type_v01 call_type,
  int *err_num,
  qcmap_error_type *qcmap_err_num
)
{
  int qcmap_errno;
  int ret_val;

  QCMAP_LOG_FUNC_ENTRY();

  ret_val = qcmap_disconnect_backhaul(qc_mgr->qcmap_cm_handle, call_type, &qcmap_errno, qcmap_err_num);

  if (ret_val == QCMAP_SUCCESS)
  {
    *err_num = qcmap_errno;
    return true;
  }

  if (ret_val == QCMAP_ERROR && qcmap_errno == QCMAP_EWOULDBLOCK)
  {
    return true;
  }

  else
  {
    return false;
  }
}
/*===========================================================================
  FUNCTION  qcmap_process_dss_net_evt
===========================================================================*/
/*!
@brief
    QCMAP DSS net callback

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
void qcmap_process_dss_net_evt
(
qcmap_dss_buffer_t *qcmap_dss_buffer
)
{
  qcmap_event_e  event;
  boolean           callback = false;
  int               err;
  int              *qcmap_errno = &err;
  qmi_client_error_type                qmi_error;
  nas_indication_register_req_msg_v01  qcmap_nas_indication_register_req_msg_v01;
  nas_indication_register_resp_msg_v01 qcmap_nas_indication_register_resp_msg_v01;
  qapi_DSS_CE_Reason_t   dsscallend;
  qcmap_error_type qcmap_err_num;

  qapi_DSS_Hndl_t         hndl;
  qapi_DSS_Net_Evt_t      evt;

  ds_assert(qcmap_dss_buffer->dss_nethandle);

  hndl        = qcmap_dss_buffer->dss_nethandle;
  evt         = qcmap_dss_buffer->evt;

  memset(&dsscallend, 0, sizeof(dsscallend));

  QCMAP_LOG_MSG_MED("qcmap_process_dss_net_evt: dss_nethandle 0x%x evt %d qcmap_cb.state (V4) %d ",\
                  hndl, evt, qcmap_cb.state);

  QCMAP_LOG_MSG_MED("qcmap_cb.ipv6_state (V6) %d",qcmap_cb.ipv6_state, 0,0 );

  if(hndl == qcmap_cb.ipv6_dss_net_hndl.handle)
  {
    if (qcmap_cb.ipv6_state == QCMAP_V6_ENABLE)
    {
      if (evt == QAPI_DSS_EVT_NET_IS_CONN_E)
      {
        qcmap_cb.ipv6_state = QCMAP_V6_WAN_CONNECTED;
        event = QCMAP_EVENT_WAN_IPv6_CONNECTED;
        callback = TRUE;
        QCMAP_LOG_MSG_MED("Received QAPI_DSS_EVT_NET_IS_CONN_E in state V6_ENABLE", 0, 0, 0);
        /*deregister from serving system NAS indication to save power*/
        memset(&qcmap_nas_indication_register_req_msg_v01, 0, sizeof(nas_indication_register_req_msg_v01));

        qcmap_nas_indication_register_req_msg_v01.sys_info_valid = TRUE;
        qcmap_nas_indication_register_req_msg_v01.sys_info = 0x00;

        QCMAP_LOG_MSG_MED("Deregistering from serving system NAS indications", 0, 0, 0);

        qmi_error = qmi_client_send_msg_sync(qcmap_cb.qmi_nas_handle,
                                       QMI_NAS_INDICATION_REGISTER_REQ_MSG_V01,
                                       &qcmap_nas_indication_register_req_msg_v01,
                                       sizeof(qcmap_nas_indication_register_req_msg_v01),
                                       &qcmap_nas_indication_register_resp_msg_v01,
                                       sizeof(qcmap_nas_indication_register_resp_msg_v01),
                                       QCMAP_QMI_TIMEOUT_VALUE);
        if(qmi_error != QMI_NO_ERR)
        {
          QCMAP_LOG_MSG_ERROR("Failed to deregister from NAS indications %d", qmi_error, 0, 0);
          if(qcmap_dss_buffer != NULL)
          {
            free(qcmap_dss_buffer);
            qcmap_dss_buffer = NULL;
          }
          return;
        }
      }
      else if(evt == QAPI_DSS_EVT_NET_NO_NET_E)
      {
        QCMAP_LOG_MSG_MED("Received event DSS_EVT_NET_NO_NET in state V6_ENABLE", 0, 0, 0);
        qcmap_cb.ipv6_state = QCMAP_V6_ENABLE;
        event = QCMAP_EVENT_WAN_IPv6_CONNECTING_FAIL;
        callback = TRUE;
      }
    }
    else if(qcmap_cb.ipv6_state == QCMAP_V6_WAN_CONNECTING)
    {
      if(evt == QAPI_DSS_EVT_NET_IS_CONN_E)
      {
          char device[QAPI_DSS_CALL_INFO_DEVICE_NAME_MAX_LEN + 2];
          qapi_DSS_Addr_Info_t v6addr;
          int rval;

          memset(device, 0, QAPI_DSS_CALL_INFO_DEVICE_NAME_MAX_LEN+2);
          qcmap_cb.ipv6_state = QCMAP_V6_WAN_CONNECTED;
          event = QCMAP_EVENT_WAN_IPv6_CONNECTED;
          callback = TRUE;
          QCMAP_LOG_MSG_MED("Received DS_ENETISCONN in state V6_WAN_CONNECTING", 0, 0, 0);
            stop_auto_timer( QCMAP_MSGR_IP_FAMILY_V6_V01 );
          rval = qapi_DSS_Get_Device_Name((qapi_DSS_Hndl_t)qcmap_cb.ipv6_dss_net_hndl.handle, device, QAPI_DSS_CALL_INFO_DEVICE_NAME_MAX_LEN+2);

          if(rval != QAPI_DSS_SUCCESS)
          {
            QCMAP_LOG_MSG_ERROR ("Couldn't get ipv6 rmnet name. rval %d", rval, 0, 0);
            strlcpy((char *)device, QCMAP_V6_DEFAULT_DEVICE_NAME, QAPI_DSS_CALL_INFO_DEVICE_NAME_MAX_LEN+2);
          }

          QCMAP_LOG_MSG_MED_SPRINT("device_name:%s",device, 0, 0);
          memcpy(&qcmap_cb.ipv6_dss_device_name, device, QAPI_DSS_CALL_INFO_DEVICE_NAME_MAX_LEN+2);

          rval = qapi_DSS_Get_IP_Addr((qapi_DSS_Hndl_t)qcmap_cb.ipv6_dss_net_hndl.handle, 
		  	(qapi_DSS_Addr_Info_t  *)&v6addr,1);
		
          if(rval != QAPI_DSS_SUCCESS)
          {
            QCMAP_LOG_MSG_ERROR ("Couldn't get ipv6 ip_addr. rval %d",
                         rval, 0, 0);
          }
          rval = qcmap_add_v6_dns_svr(&v6addr);
          if(rval != QAPI_DSS_SUCCESS)
          {
            QCMAP_LOG_MSG_ERROR ("Couldn't add v6 dns srvr. rval %d",
                         rval, 0, 0);
          }

          QCMAP_LOG_MSG_MED("QCMAP AP Handle 0x%04x IPv6 WAN Connected",qcmap_cb.handle, 0, 0);
          /*deregister from serving system NAS indication to save power*/
          memset(&qcmap_nas_indication_register_req_msg_v01, 0, sizeof(nas_indication_register_req_msg_v01));

          qcmap_nas_indication_register_req_msg_v01.sys_info_valid = TRUE;
          qcmap_nas_indication_register_req_msg_v01.sys_info = 0x00;

          QCMAP_LOG_MSG_MED("Deregistering from serving system NAS indications", 0, 0, 0);

          qmi_error = qmi_client_send_msg_sync(qcmap_cb.qmi_nas_handle,
                                          QMI_NAS_INDICATION_REGISTER_REQ_MSG_V01,
                                          &qcmap_nas_indication_register_req_msg_v01,
                                          sizeof(qcmap_nas_indication_register_req_msg_v01),
                                          &qcmap_nas_indication_register_resp_msg_v01,
                                          sizeof(qcmap_nas_indication_register_resp_msg_v01),
                                          QCMAP_QMI_TIMEOUT_VALUE);
          if(qmi_error != QMI_NO_ERR)
          {
            QCMAP_LOG_MSG_ERROR("Failed to deregister from NAS indications %d", qmi_error, 0, 0);
          if(qcmap_dss_buffer != NULL)
          {
            free(qcmap_dss_buffer);
            qcmap_dss_buffer = NULL;
          }
            return;
          }

      }
      else if(evt == QAPI_DSS_EVT_NET_NO_NET_E)
      {
        qcmap_cb.ipv6_state = QCMAP_V6_ENABLE;
        event = QCMAP_EVENT_WAN_IPv6_CONNECTING_FAIL;
        callback = TRUE;
        QCMAP_LOG_MSG_MED("Received event QAPI_DSS_EVT_NET_NO_NET_E in V6_WAN_CONNECTING", 0, 0, 0);
        QCMAP_LOG_MSG_MED("Transition from V6_WAN_CONNECTING to V6_ENABLE", 0, 0, 0);
        if(qapi_DSS_Get_Call_End_Reason((qapi_DSS_Hndl_t)qcmap_cb.ipv6_dss_net_hndl.handle,
			(qapi_DSS_CE_Reason_t *)&dsscallend,(qapi_DSS_IP_Family_t)QAPI_DSS_IP_FAMILY_V6_E) == QAPI_DSS_SUCCESS)
        {
          QCMAP_LOG_MSG_ERROR("Call Disconnected reason type=%d reason code =%d ",dsscallend.reason_type, dsscallend.reason_code, 0);
        }
        if(qcmap_cb.disable_in_process)
        {
          qcmap_disable(qcmap_cb.handle, qcmap_errno, &qcmap_err_num);
        }
        else if (qc_mgr->auto_connect)
        {
          /*register for serving system NAS indication*/
          memset(&qcmap_nas_indication_register_req_msg_v01, 0, sizeof(nas_indication_register_req_msg_v01));
          qcmap_nas_indication_register_req_msg_v01.sys_info_valid = TRUE;
          qcmap_nas_indication_register_req_msg_v01.sys_info = 0x01;

          QCMAP_LOG_MSG_MED("Registering for serving system NAS indications", 0, 0, 0);

          qmi_error = qmi_client_send_msg_sync(qcmap_cb.qmi_nas_handle,
                                               QMI_NAS_INDICATION_REGISTER_REQ_MSG_V01,
                                               &qcmap_nas_indication_register_req_msg_v01,
                                               sizeof(qcmap_nas_indication_register_req_msg_v01),
                                               &qcmap_nas_indication_register_resp_msg_v01,
                                               sizeof(qcmap_nas_indication_register_resp_msg_v01),
                                               QCMAP_QMI_TIMEOUT_VALUE);
          if(qmi_error != QMI_NO_ERR)
          {
            QCMAP_LOG_MSG_ERROR("Failed to register to NAS indications %d", qmi_error, 0, 0);
          if(qcmap_dss_buffer != NULL)
          {
            free(qcmap_dss_buffer);
            qcmap_dss_buffer = NULL;
          }
            return;
          }

          QCMAP_LOG_MSG_MED("Start autoconnect", 0, 0, 0);
          /* Set backhual service status to false. We don't know the reason
             for the disconnect. Try to reconnect the backhaul. */
          qcmap_cb.backhaul_service = false;
          start_auto_connect( QCMAP_MSGR_IP_FAMILY_V6_V01 );
        }
      }

      if (callback)
        qcmap_cb.qcmap_callback(qcmap_cb.handle, event, qcmap_cb.qcmap_callback_user_data,&dsscallend);
          if(qcmap_dss_buffer != NULL)
          {
            free(qcmap_dss_buffer);
            qcmap_dss_buffer = NULL;
          }

      return;
    }
    else if(qcmap_cb.ipv6_state == QCMAP_V6_WAN_CONNECTED)
    {
      if(evt == QAPI_DSS_EVT_NET_IS_CONN_E)
      {
        QCMAP_LOG_MSG_MED("Received event QAPI_DSS_EVT_NET_IS_CONN_E in V6_WAN_CONNECTED", 0, 0, 0);
        QCMAP_LOG_MSG_MED("Ignore event QAPI_DSS_EVT_NET_IS_CONN_E in state V6_WAN_CONNECTED", 0, 0, 0);
      }
      else if(evt == QAPI_DSS_EVT_NET_NO_NET_E)
      {
        qcmap_cb.ipv6_state = QCMAP_V6_ENABLE;
        event = QCMAP_EVENT_WAN_IPv6_DISCONNECTED;
        callback = TRUE;
        QCMAP_LOG_MSG_MED("Received event QAPI_DSS_EVT_NET_NO_NET_E in V6_WAN_CONNECTED", 0, 0, 0);

        if(qapi_DSS_Get_Call_End_Reason((qapi_DSS_Hndl_t)qcmap_cb.ipv6_dss_net_hndl.handle,
			(qapi_DSS_CE_Reason_t *)&dsscallend,(qapi_DSS_IP_Family_t)QAPI_DSS_IP_FAMILY_V6_E) == QAPI_DSS_SUCCESS)
        {
          QCMAP_LOG_MSG_ERROR("Call Disconnected reason type=%d reason code =%d ", dsscallend.reason_type, dsscallend.reason_code, 0);
        }
        qcmap_cb.qcmap_callback(qcmap_cb.handle, event,
                                      qcmap_cb.qcmap_callback_user_data,&dsscallend);

        if(qcmap_cb.disable_in_process)
        {
          qcmap_disable(qcmap_cb.handle, qcmap_errno, &qcmap_err_num);
        }
        else if (qc_mgr->auto_connect)
        {
          /*register for serving system NAS indication*/
          memset(&qcmap_nas_indication_register_req_msg_v01, 0, sizeof(nas_indication_register_req_msg_v01));
          qcmap_nas_indication_register_req_msg_v01.sys_info_valid = TRUE;
          qcmap_nas_indication_register_req_msg_v01.sys_info = 0x01;

          QCMAP_LOG_MSG_MED("Registering for serving system NAS indications", 0, 0, 0);

          qmi_error = qmi_client_send_msg_sync(qcmap_cb.qmi_nas_handle,
                                               QMI_NAS_INDICATION_REGISTER_REQ_MSG_V01,
                                               &qcmap_nas_indication_register_req_msg_v01,
                                               sizeof(qcmap_nas_indication_register_req_msg_v01),
                                               &qcmap_nas_indication_register_resp_msg_v01,
                                               sizeof(qcmap_nas_indication_register_resp_msg_v01),
                                               QCMAP_QMI_TIMEOUT_VALUE);
          if(qmi_error != QMI_NO_ERR)
          {
            QCMAP_LOG_MSG_ERROR("Failed to register to NAS indications %d", qmi_error, 0, 0);
          if(qcmap_dss_buffer != NULL)
          {
            free(qcmap_dss_buffer);
            qcmap_dss_buffer = NULL;
          }
            return;
          }


          QCMAP_LOG_MSG_MED("Start autoconnect", 0, 0, 0);
          /* Set backhual service status to false. We don't know the reason
             for the disconnect. Try to reconnect the backhaul. */
          qcmap_cb.backhaul_service = false;
          start_auto_connect( QCMAP_MSGR_IP_FAMILY_V6_V01 );
          if(qcmap_dss_buffer != NULL)
          {
            free(qcmap_dss_buffer);
            qcmap_dss_buffer = NULL;
          }
        }
        return;
      }
      else if ( evt == QAPI_DSS_EVT_NET_RECONFIGURED_E)
      {
        event = QCMAP_EVENT_WAN_IPv6_ADDR_RECONF;
        callback = TRUE;
        QCMAP_LOG_MSG_MED("Received event DSS_EVT_NET_RECONFIGURED "
                      "in V6_WAN_CONNECTED", 0, 0, 0);
      }
    }
    else if(qcmap_cb.ipv6_state == QCMAP_V6_WAN_DISCONNECTING)
    {
      if(evt == QAPI_DSS_EVT_NET_IS_CONN_E)
      {
        QCMAP_LOG_MSG_MED("Received event QAPI_DSS_EVT_NET_IS_CONN_E in V6_WAN_DISCONNECTING", 0, 0, 0);
      }
      else if(evt == QAPI_DSS_EVT_NET_NO_NET_E)
      {

        qcmap_cb.ipv6_state = QCMAP_V6_ENABLE;
        event = QCMAP_EVENT_WAN_IPv6_DISCONNECTED;

        /* Resetting timer value after disconnect*/
            stop_auto_timer( QCMAP_MSGR_IP_FAMILY_V6_V01 );

        QCMAP_LOG_MSG_MED("Received event QAPI_DSS_EVT_NET_NO_NET_E in V6_WAN_DISCONNECTING", 0, 0, 0);
        callback = TRUE;
        if(qapi_DSS_Get_Call_End_Reason((qapi_DSS_Hndl_t)qcmap_cb.ipv6_dss_net_hndl.handle,
			(qapi_DSS_CE_Reason_t *)&dsscallend,(qapi_DSS_IP_Family_t)QAPI_DSS_IP_FAMILY_V6_E) == QAPI_DSS_SUCCESS)
        {
          QCMAP_LOG_MSG_MED("Call Disconnected reason type=%d reason code =%d ", dsscallend.reason_type, dsscallend.reason_code, 0);
        }
        qcmap_cb.qcmap_callback(qcmap_cb.handle, event,
                                      qcmap_cb.qcmap_callback_user_data,&dsscallend);
       if(qcmap_cb.disable_in_process)
        {
          qcmap_disable(qcmap_cb.handle, qcmap_errno, &qcmap_err_num);
          if(qcmap_dss_buffer != NULL)
          {
            free(qcmap_dss_buffer);
            qcmap_dss_buffer = NULL;
          }
        }
        return;
      }
    }
  }

  if (hndl == qcmap_cb.dss_net_hndl.handle)
  {
    if (qcmap_cb.state == QCMAP_ENABLE)
    {
      if (evt == QAPI_DSS_EVT_NET_IS_CONN_E)
      {

        qcmap_cb.state = QCMAP_WAN_CONNECTED;
        event = QCMAP_EVENT_WAN_CONNECTED;
        callback = TRUE;
        QCMAP_LOG_MSG_MED("Received event QAPI_DSS_EVT_NET_IS_CONN_E in state ENABLE", 0, 0, 0);
        /*deregister from serving system NAS indication to save power*/
        memset(&qcmap_nas_indication_register_req_msg_v01, 0, sizeof(nas_indication_register_req_msg_v01));

        qcmap_nas_indication_register_req_msg_v01.sys_info_valid = TRUE;
        qcmap_nas_indication_register_req_msg_v01.sys_info = 0x00;

        QCMAP_LOG_MSG_MED("Deregistering from serving system NAS indications",0,0,0);

        qmi_error = qmi_client_send_msg_sync(qcmap_cb.qmi_nas_handle,
                                             QMI_NAS_INDICATION_REGISTER_REQ_MSG_V01,
                                             &qcmap_nas_indication_register_req_msg_v01,
                                             sizeof(qcmap_nas_indication_register_req_msg_v01),
                                             &qcmap_nas_indication_register_resp_msg_v01,
                                             sizeof(qcmap_nas_indication_register_resp_msg_v01),
                                             QCMAP_QMI_TIMEOUT_VALUE);
        if(qmi_error != QMI_NO_ERR)
        {
          QCMAP_LOG_MSG_ERROR("Failed to deregister from NAS indications %d", qmi_error, 0, 0);
          if(qcmap_dss_buffer != NULL)
          {
            free(qcmap_dss_buffer);
            qcmap_dss_buffer = NULL;
          }
          return;
        }

      }
      else if (evt == QAPI_DSS_EVT_NET_NO_NET_E)
      {
        QCMAP_LOG_MSG_MED("Received event QAPI_DSS_EVT_NET_NO_NET_E in state ENABLE", 0, 0, 0);
        qcmap_cb.state = QCMAP_ENABLE;
        event = QCMAP_EVENT_WAN_CONNECTING_FAIL;
        QCMAP_LOG_MSG_MED("QCMAP AP Handle 0x%04x IPv4 Backhaul Connecting Fail",
                      qcmap_cb.handle, 0, 0);
        callback = TRUE;
      }
    }
    else if (qcmap_cb.state == QCMAP_WAN_CONNECTING)
    {
      if (evt == QAPI_DSS_EVT_NET_IS_CONN_E)
      {
        char device[QAPI_DSS_CALL_INFO_DEVICE_NAME_MAX_LEN + 2];
        qapi_DSS_Addr_Info_t v4addr;
        int rval;

        qcmap_cb.state = QCMAP_WAN_CONNECTED;
        event = QCMAP_EVENT_WAN_CONNECTED;
        callback = TRUE;

        QCMAP_LOG_MSG_MED("Received DS_ENETISCONN in state WAN_CONNECTING", 0, 0, 0);
            stop_auto_timer( QCMAP_MSGR_IP_FAMILY_V4_V01 );
        memset(device, 0, QAPI_DSS_CALL_INFO_DEVICE_NAME_MAX_LEN+2);

        rval = qapi_DSS_Get_Device_Name((qapi_DSS_Hndl_t)qcmap_cb.dss_net_hndl.handle, device, QAPI_DSS_CALL_INFO_DEVICE_NAME_MAX_LEN+2);

        if(rval != QAPI_DSS_SUCCESS)
        {
          QCMAP_LOG_MSG_ERROR ("Couldn't get ipv4 rmnet name. rval %d",
                         rval, 0, 0);
          strlcpy((char *)device, QCMAP_V4_DEFAULT_DEVICE_NAME, QAPI_DSS_CALL_INFO_DEVICE_NAME_MAX_LEN+2);
        }

        QCMAP_LOG_MSG_MED_SPRINT("device_name:%s", device, 0, 0);
        memcpy(&qcmap_cb.dss_device_name, device, QAPI_DSS_CALL_INFO_DEVICE_NAME_MAX_LEN+2);

        rval = qapi_DSS_Get_IP_Addr((qapi_DSS_Hndl_t)qcmap_cb.dss_net_hndl.handle, 
			(qapi_DSS_Addr_Info_t  *)&v4addr,1);
		
        if(rval != QAPI_DSS_SUCCESS)
        {
          QCMAP_LOG_MSG_ERROR ("Couldn't get ipv4 ip_addr. rval %d",
                         rval, 0, 0);
        }
        rval = qcmap_add_v4_dns_svr(&v4addr);
         if(rval != QAPI_DSS_SUCCESS)
        {
          QCMAP_LOG_MSG_ERROR ("Couldn't add v4 dns srvr. rval %d",
                         rval, 0, 0);
        }

        QCMAP_LOG_MSG_MED("QCMAP AP Handle 0x%04x IPv4 WAN Connected", qcmap_cb.handle, 0, 0);
        /*deregister from serving system NAS indication to save power*/
        memset(&qcmap_nas_indication_register_req_msg_v01, 0, sizeof(nas_indication_register_req_msg_v01));

        qcmap_nas_indication_register_req_msg_v01.sys_info_valid = TRUE;
        qcmap_nas_indication_register_req_msg_v01.sys_info = 0x00;

        QCMAP_LOG_MSG_MED("Deregistering from serving system NAS indications", 0, 0, 0);

        qmi_error = qmi_client_send_msg_sync(qcmap_cb.qmi_nas_handle,
                                             QMI_NAS_INDICATION_REGISTER_REQ_MSG_V01,
                                             &qcmap_nas_indication_register_req_msg_v01,
                                             sizeof(qcmap_nas_indication_register_req_msg_v01),
                                             &qcmap_nas_indication_register_resp_msg_v01,
                                             sizeof(qcmap_nas_indication_register_resp_msg_v01),
                                             QCMAP_QMI_TIMEOUT_VALUE);
        if(qmi_error != QMI_NO_ERR)
        {
          QCMAP_LOG_MSG_MED("Failed to deregister from NAS indications %d", qmi_error, 0, 0);
          if(qcmap_dss_buffer != NULL)
          {
            free(qcmap_dss_buffer);
            qcmap_dss_buffer = NULL;
          }
          return;
        }
      }
      else if (evt == QAPI_DSS_EVT_NET_NO_NET_E)
      {

        qcmap_cb.state = QCMAP_ENABLE;
        event = QCMAP_EVENT_WAN_CONNECTING_FAIL;
        QCMAP_LOG_MSG_MED("QCMAP AP Handle 0x%04x IPv4 Backhaul Connecting Fail",
                      qcmap_cb.handle, 0, 0);
        callback = TRUE;

        if(qapi_DSS_Get_Call_End_Reason((qapi_DSS_Hndl_t)qcmap_cb.dss_net_hndl.handle,
			(qapi_DSS_CE_Reason_t *)&dsscallend,(qapi_DSS_IP_Family_t)QAPI_DSS_IP_FAMILY_V4_E) == QAPI_DSS_SUCCESS)
        {
          QCMAP_LOG_MSG_MED("Call Disconnected reason type=%d reason code =%d ",
                             dsscallend.reason_type, dsscallend.reason_code, 0);
        }
        if (callback)
          qcmap_cb.qcmap_callback(qcmap_cb.handle, event,
              qcmap_cb.qcmap_callback_user_data,&dsscallend);

        if(qcmap_cb.disable_in_process)
        {
          qcmap_disable(qcmap_cb.handle, qcmap_errno, &qcmap_err_num);
        }
        else if (qc_mgr->auto_connect)
        {
          /*register for serving system NAS indication*/
          memset(&qcmap_nas_indication_register_req_msg_v01, 0, sizeof(nas_indication_register_req_msg_v01));
          qcmap_nas_indication_register_req_msg_v01.sys_info_valid = TRUE;
          qcmap_nas_indication_register_req_msg_v01.sys_info = 0x01;

          QCMAP_LOG_MSG_MED("Registering for serving system NAS indications", 0, 0, 0);

          qmi_error = qmi_client_send_msg_sync(qcmap_cb.qmi_nas_handle,
                                               QMI_NAS_INDICATION_REGISTER_REQ_MSG_V01,
                                               &qcmap_nas_indication_register_req_msg_v01,
                                               sizeof(qcmap_nas_indication_register_req_msg_v01),
                                               &qcmap_nas_indication_register_resp_msg_v01,
                                               sizeof(qcmap_nas_indication_register_resp_msg_v01),
                                               QCMAP_QMI_TIMEOUT_VALUE);
          if(qmi_error != QMI_NO_ERR)
          {
            QCMAP_LOG_MSG_ERROR("Failed to register to NAS indications %d", qmi_error, 0, 0);
          if(qcmap_dss_buffer != NULL)
          {
            free(qcmap_dss_buffer);
            qcmap_dss_buffer = NULL;
          }
            return;
          }

          QCMAP_LOG_MSG_MED("Start autoconnect", 0, 0, 0);
          /* Set backhual service status to false. We don't know the reason
             for the disconnect. Try to reconnect the backhaul. */
          qcmap_cb.backhaul_service = false;
          start_auto_connect( QCMAP_MSGR_IP_FAMILY_V4_V01 );
        QCMAP_LOG_MSG_MED("Received event QAPI_DSS_EVT_NET_NO_NET_E in WAN_CONNECTING", 0, 0, 0);
      }
          if(qcmap_dss_buffer != NULL)
          {
            free(qcmap_dss_buffer);
            qcmap_dss_buffer = NULL;
          }
      return;
    }
   }
    else if (qcmap_cb.state == QCMAP_WAN_CONNECTED)
    {
      if(evt == QAPI_DSS_EVT_NET_IS_CONN_E)
      {
        QCMAP_LOG_MSG_MED("Received event DSI_EVT_NET_IS_CONN in WAN_CONNECTED", 0, 0, 0);
      }
      else if(evt == QAPI_DSS_EVT_NET_NO_NET_E)
      {

        qcmap_cb.state = QCMAP_ENABLE;
        event = QCMAP_EVENT_WAN_DISCONNECTED;
        QCMAP_LOG_MSG_MED("QCMAP AP Handle 0x%04x IPv4 Backhaul Disconnected",qcmap_cb.handle, 0, 0);
        callback = TRUE;

        if(qapi_DSS_Get_Call_End_Reason((qapi_DSS_Hndl_t)qcmap_cb.dss_net_hndl.handle, 
			(qapi_DSS_CE_Reason_t *)&dsscallend, (qapi_DSS_IP_Family_t)QAPI_DSS_IP_FAMILY_V4_E) == QAPI_DSS_SUCCESS)
        {
          QCMAP_LOG_MSG_MED("Call Disconnected reason type=%d reason code =%d ",
                             dsscallend.reason_type, dsscallend.reason_code, 0);
        }
        QCMAP_LOG_MSG_MED("Received event QAPI_DSS_EVT_NET_NO_NET_E in WAN_CONNECTED", 0, 0, 0);
        if (callback)
          qcmap_cb.qcmap_callback(qcmap_cb.handle, event,
                                        qcmap_cb.qcmap_callback_user_data,&dsscallend);
        if(qcmap_cb.disable_in_process)
        {
          qcmap_disable(qcmap_cb.handle, qcmap_errno, &qcmap_err_num);
        }
        else if (qc_mgr->auto_connect)
        {
          /*register for serving system NAS indication*/
          memset(&qcmap_nas_indication_register_req_msg_v01, 0, sizeof(nas_indication_register_req_msg_v01));
          qcmap_nas_indication_register_req_msg_v01.sys_info_valid = TRUE;
          qcmap_nas_indication_register_req_msg_v01.sys_info = 0x01;

          QCMAP_LOG_MSG_MED("Registering for serving system NAS indications", 0, 0, 0);

          qmi_error = qmi_client_send_msg_sync(qcmap_cb.qmi_nas_handle,
                                               QMI_NAS_INDICATION_REGISTER_REQ_MSG_V01,
                                               &qcmap_nas_indication_register_req_msg_v01,
                                               sizeof(qcmap_nas_indication_register_req_msg_v01),
                                               &qcmap_nas_indication_register_resp_msg_v01,
                                               sizeof(qcmap_nas_indication_register_resp_msg_v01),
                                               QCMAP_QMI_TIMEOUT_VALUE);
          if(qmi_error != QMI_NO_ERR)
          {
            QCMAP_LOG_MSG_ERROR("Failed to register to NAS indications %d", qmi_error, 0, 0);
          if(qcmap_dss_buffer != NULL)
          {
            free(qcmap_dss_buffer);
            qcmap_dss_buffer = NULL;
          }
            return;
          }

          QCMAP_LOG_MSG_MED("Start autoconnect", 0, 0, 0);
          /* Set backhual service status to false. We don't know the reason
             for the disconnect. Try to reconnect the backhaul. */
          qcmap_cb.backhaul_service = false;
          start_auto_connect( QCMAP_MSGR_IP_FAMILY_V4_V01 );
        }
          if(qcmap_dss_buffer != NULL)
          {
            free(qcmap_dss_buffer);
            qcmap_dss_buffer = NULL;
          }
        return;
      }
      else if ( evt == QAPI_DSS_EVT_NET_RECONFIGURED_E )
      {
        event = QCMAP_EVENT_WAN_ADDR_RECONF;
        callback = true;
        QCMAP_LOG_MSG_MED("Received event DSI_EVT_NET_RECONFIGURED "
                      "in WAN_CONNECTED", 0, 0, 0);
      }
    }
    else if (qcmap_cb.state == QCMAP_WAN_DISCONNECTING)
    {
      if(evt == QAPI_DSS_EVT_NET_IS_CONN_E)
      {
        QCMAP_LOG_MSG_MED("Received event DSI_EVT_NET_IS_CONN in WAN_DISCONNECTING", 0, 0, 0);
      }
      else if(evt == QAPI_DSS_EVT_NET_NO_NET_E)
      {
        qcmap_cb.state = QCMAP_ENABLE;
        event = QCMAP_EVENT_WAN_DISCONNECTED;
            stop_auto_timer( QCMAP_MSGR_IP_FAMILY_V4_V01 );

        QCMAP_LOG_MSG_MED("QCMAP AP Handle 0x%04x IPv4 Backhaul Disconnected",
                      qcmap_cb.handle, 0, 0);
        callback = TRUE;
        if(qapi_DSS_Get_Call_End_Reason((qapi_DSS_Hndl_t)qcmap_cb.dss_net_hndl.handle,
			(qapi_DSS_CE_Reason_t *)&dsscallend,(qapi_DSS_IP_Family_t)QAPI_DSS_IP_FAMILY_V4_E) == QAPI_DSS_SUCCESS)
        {
          QCMAP_LOG_MSG_MED("Call Disconnected reason type=%d reason code =%d ",
                             dsscallend.reason_type, dsscallend.reason_code, 0);
        }
        QCMAP_LOG_MSG_MED("Received event DS_ENETNONET in WAN_DISCONNECTING", 0, 0, 0);
        if (callback)
          qcmap_cb.qcmap_callback(qcmap_cb.handle, event,
                                        qcmap_cb.qcmap_callback_user_data,&dsscallend);
        if(qcmap_cb.disable_in_process)
        {
          qcmap_disable(qcmap_cb.handle, qcmap_errno, &qcmap_err_num);
        }
          if(qcmap_dss_buffer != NULL)
          {
            free(qcmap_dss_buffer);
            qcmap_dss_buffer = NULL;
          }
        return;
      }
    }
  }
  if (callback)
    qcmap_cb.qcmap_callback(qcmap_cb.handle, event,
                              qcmap_cb.qcmap_callback_user_data,NULL);
          if(qcmap_dss_buffer != NULL)
          {
            free(qcmap_dss_buffer);
            qcmap_dss_buffer = NULL;
          }
  return;
}

/*===========================================================================
  FUNCTION start_auto_connect
===========================================================================*/
/*!
@brief
  This Function is used to send connect to backhaul request incase the
  previous connect to backhal request failed.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
void start_auto_connect(qcmap_msgr_ip_family_enum_v01 family)
{
  qurt_time_t reload_time;

  if( family == QCMAP_MSGR_IP_FAMILY_V4_V01)
  {
    QCMAP_LOG_MSG_MED("qcmap_cb.state =%d ",qcmap_cb.state, 0, 0);

    qc_mgr->auto_connect_timer_running_v4 = true;
    reload_time = qc_mgr->auto_timer_value_v4;

    QCMAP_LOG_MSG_MED("V4 New Connect to backhaul request with start after %d sec ",
                      qc_mgr->auto_timer_value_v4, 0, 0);

    timer_set_64(&qc_mgr->ipv4_auto_connect_timer, reload_time, 0, T_SEC);

    if(qc_mgr->auto_timer_value_v4 < MAX_WAN_CON_TIMEOUT)
    {
      qc_mgr->auto_timer_value_v4 = qc_mgr->auto_timer_value_v4 * 2;
    }
  }
  else
  {
    QCMAP_LOG_MSG_MED("qcmap_cb.state =%d ",qcmap_cb.ipv6_state, 0, 0);

    qc_mgr->auto_connect_timer_running_v6 = true;
    reload_time = qc_mgr->auto_timer_value_v6;

    QCMAP_LOG_MSG_MED("V6 New Connect to backhaul request with start after %d sec ",
                      qc_mgr->auto_timer_value_v6, 0, 0);

    timer_set_64(&qc_mgr->ipv6_auto_connect_timer, reload_time, 0, T_SEC);

    if(qc_mgr->auto_timer_value_v6 < MAX_WAN_CON_TIMEOUT)
    {
      qc_mgr->auto_timer_value_v6 = qc_mgr->auto_timer_value_v6 * 2;
    }
  }

}

/*===========================================================================
  FUNCTION stop_auto_timer
===========================================================================*/
/*!
@brief
 This Fuction will stop the running timer and reset the timer vaules.
@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
void stop_auto_timer(qcmap_msgr_ip_family_enum_v01 family)
{


  {
    if(family == QCMAP_MSGR_IP_FAMILY_V4_V01)
    {
      QCMAP_LOG_MSG_MED("stopping auto timer for v4 ", 0, 0, 0);
      if(qc_mgr->auto_connect_timer_running_v4==true)
      {
        qc_mgr->auto_connect_timer_running_v4=false;
        timer_stop(&qc_mgr->ipv4_auto_connect_timer,T_SEC,NULL);
        qc_mgr->auto_timer_value_v4 = AUTO_CONNECT_TIMER;
      }
      else
      {
        QCMAP_LOG_MSG_HIGH("auto timer not running v4 ",0, 0, 0);
      }
    }
    else
    {
      QCMAP_LOG_MSG_MED("stopping auto timer for v6 ", 0, 0, 0);
      if(qc_mgr->auto_connect_timer_running_v6==true)
      {
        qc_mgr->auto_connect_timer_running_v6=false;
        timer_stop(&qc_mgr->ipv6_auto_connect_timer,T_SEC,NULL);
        qc_mgr->auto_timer_value_v6 =  AUTO_CONNECT_TIMER;
      }
      else
      {
        QCMAP_LOG_MSG_HIGH("auto timer not running v6 ", 0, 0, 0);
      }
    }
  }
}

/*=============================================================================

FUNCTION QCMAP_SEND_MSG

DESCRIPTION
  API to send response/indication back to the QCMAP CLIENT
DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None

==============================================================================*/
void qcmap_send_msg
(
  qcmap_cmd_id_e evt,
  void * data,
  uint8  connection_handle
)
{
  uint8 idx;
  qcmap_client_resp_ind_cb qcmap_resp_handler = NULL;

  ds_assert(data != NULL);

  QCMAP_LOG_MSG_MED("qcmap_send_msg: enter", 0, 0, 0);

  for (idx = 0 ; idx < QCMAP_MAX_CLIENT_HANDLES; idx++)
  {
    if (connection_handle == idx)
    {
      qcmap_resp_handler = qc_mgr->cb_arr[idx];
      break;
    }
  }

  if (idx == QCMAP_MAX_CLIENT_HANDLES)
  {
    QCMAP_LOG_MSG_ERROR("Unable to find Client Call back!! Please check!!", 0, 0, 0);
    return;
  }

  if(qcmap_resp_handler != NULL)
  {
    qcmap_resp_handler((uint8)evt, data, connection_handle);
  }

  return;
}


/*===========================================================================
  FUNCTION QCMAP_GET_AUTO_CONNECT()

  DESCRIPTION
    Get Autoconnect flag value on Apps.

  PARAMETERS
    data - Data from client
    conn_handle: Used to identify client

  RETURN VALUE
    None

  DEPENDENCIES
    qcmap_client_reg_cb should be called

  SIDE EFFECTS
    None
===========================================================================*/
void  qcmap_get_auto_connect(
  void* data,
  uint8 conn_handle
)
{
  qcmap_msgr_get_auto_connect_req_msg_v01  *req_ptr = NULL;
  qcmap_msgr_get_auto_connect_resp_msg_v01 *resp_msg;
  boolean    auto_connect_flag;

  ds_assert(data != NULL);

  resp_msg = (qcmap_msgr_get_auto_connect_resp_msg_v01 *)malloc(sizeof(qcmap_msgr_get_auto_connect_resp_msg_v01));
  if (resp_msg == NULL)
  {
    QCMAP_LOG_MSG_ERROR("Out of mem for resp msg", 0, 0, 0);
    ASSERT(0);
  }
  memset(resp_msg, 0, sizeof(qcmap_msgr_get_auto_connect_resp_msg_v01));

  req_ptr =  (qcmap_msgr_get_auto_connect_req_msg_v01*)data;

  QCMAP_LOG_MSG_MED ("Client %d get auto_connect flag", conn_handle, 0, 0);

  auto_connect_flag = GetAutoconnect();

  resp_msg->auto_conn_flag_valid = TRUE;
  resp_msg->auto_conn_flag = auto_connect_flag;

  qcmap_send_msg(QCMAP_MOBILEAP_GET_AUTOCONNECT_RESP_EVT, (void *)resp_msg, conn_handle);
 
  if(resp_msg != NULL)
  {
    free(resp_msg);
    resp_msg = NULL;
  }
  if(req_ptr != NULL)
  {
    free(req_ptr);
    req_ptr = NULL;
  }
  return ;
}

/*===========================================================================
  FUNCTION QCMAP_SET_AUTO_CONNECT()

  DESCRIPTION
    Set Autoconnect flag value on Apps.

  PARAMETERS
    data - Data from client
    conn_handle: Used to identify client

  RETURN VALUE
    None

  DEPENDENCIES
    qcmap_client_reg_cb should be called

  SIDE EFFECTS
    None
===========================================================================*/
void  qcmap_set_auto_connect(
  void* data,
  uint8 conn_handle
)
{
  qcmap_msgr_set_auto_connect_req_msg_v01  *req_ptr = NULL;
  qcmap_msgr_set_auto_connect_resp_msg_v01 *resp_msg;
  qcmap_softap_handle_type                 *map_handle;
  unsigned int index;
  qcmap_error_type qcmap_err_num;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(data != NULL);

  resp_msg = (qcmap_msgr_set_auto_connect_resp_msg_v01 *)malloc(sizeof(qcmap_msgr_set_auto_connect_resp_msg_v01));
  if (resp_msg == NULL)
  {
    QCMAP_LOG_MSG_ERROR("Out of mem for resp msg", 0, 0, 0);
    ASSERT(0);
  }
  memset(resp_msg, 0, sizeof(qcmap_msgr_set_auto_connect_resp_msg_v01));

  req_ptr =  (qcmap_msgr_set_auto_connect_req_msg_v01*)data;

  QCMAP_LOG_MSG_MED("Client %d set auto_connect flag", conn_handle, 0, 0);

  /*---------------------------------------------------------------------

  ---------------------------------------------------------------------*/
  index = (req_ptr->mobile_ap_handle);
  map_handle = &qcmap_handle;

  if (index != QCMAP_SOFTAP_HANDLE ||map_handle->handle == 0)
  {
    QCMAP_LOG_MSG_MED("Incorrect handle %d passed", req_ptr->mobile_ap_handle, 0, 0);
    resp_msg->resp.result = QCMAP_RESULT_FAILURE;
    resp_msg->resp.error = QCMAP_ERR_INVALID_HANDLE;
    qcmap_send_msg(QCMAP_MOBILEAP_SET_AUTOCONNECT_RESP_EVT, (void *)resp_msg, conn_handle);

    if(resp_msg != NULL)
    {
      free(resp_msg);
      resp_msg = NULL;
    }
    if(req_ptr != NULL)
    {
      free(req_ptr);
      req_ptr = NULL;
    }
    return ;
  }
  if (SetAutoconnect(req_ptr->enable, &qcmap_err_num))
  {
    QCMAP_LOG_MSG_MED("Set Auto connect flag successfully.", 0, 0, 0);
  }
  else
  {
    QCMAP_LOG_MSG_ERROR("Set Auto connect failed!!", 0, 0, 0);
    resp_msg->resp.result = QCMAP_RESULT_FAILURE;
    resp_msg->resp.error = qcmap_err_num;
    qcmap_send_msg(QCMAP_MOBILEAP_SET_AUTOCONNECT_RESP_EVT, (void *)resp_msg, conn_handle);

    if(resp_msg != NULL)
    {
      free(resp_msg);
      resp_msg = NULL;
    }
    if(req_ptr != NULL)
    {
      free(req_ptr);
      req_ptr = NULL;
    }
    return ;
  }

  qcmap_send_msg(QCMAP_MOBILEAP_SET_AUTOCONNECT_RESP_EVT, (void *)resp_msg, conn_handle);
 
  if(resp_msg != NULL)
  {
    free(resp_msg);
    resp_msg = NULL;
  }
  if(req_ptr != NULL)
  {
    free(req_ptr);
    req_ptr = NULL;
  }
  return;
}

/*===========================================================================
  FUNCTION QCMAP_GET_ROAMING()

  DESCRIPTION
    Gets Roaming flag value on Apps.

  PARAMETERS
    data - Data from client
    conn_handle: Used to identify client

  RETURN VALUE
    None

  DEPENDENCIES
    qcmap_client_reg_cb should be called

  SIDE EFFECTS
    None
===========================================================================*/
void qcmap_get_roaming
(
  void* data,
  uint8 conn_handle
)
{

  qcmap_msgr_get_roaming_pref_req_msg_v01  *req_ptr = NULL;
  qcmap_msgr_get_roaming_pref_resp_msg_v01 *resp_msg;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(data != NULL);

  resp_msg = (qcmap_msgr_get_roaming_pref_resp_msg_v01 *)malloc(sizeof(qcmap_msgr_get_roaming_pref_resp_msg_v01));
  if (resp_msg == NULL)
  {
    QCMAP_LOG_MSG_ERROR("Out of mem for resp msg", 0, 0, 0);
    ASSERT(0);
  }
  memset(resp_msg, 0, sizeof(qcmap_msgr_get_roaming_pref_resp_msg_v01));

  req_ptr =  (qcmap_msgr_get_roaming_pref_req_msg_v01*)data;

  QCMAP_LOG_MSG_MED("Client %d get roaming flag", conn_handle, 0, 0);


  /*---------------------------------------------------------------------
       Get Roaming flag
  ---------------------------------------------------------------------*/
  resp_msg->allow_wwan_calls_while_roaming_valid = TRUE;
  resp_msg->allow_wwan_calls_while_roaming = GetRoaming();
  qcmap_send_msg(QCMAP_MOBILEAP_GET_ROAMING_RESP_EVT, (void *)resp_msg, conn_handle);
 
  if(resp_msg != NULL)
  {
    free(resp_msg);
    resp_msg = NULL;
  }
  if(req_ptr != NULL)
  {
    free(req_ptr);
    req_ptr = NULL;
  }
  return;
}

/*===========================================================================
  FUNCTION QCMAP_SET_ROAMING()

  DESCRIPTION
    Set roaming flag value on Apps.  PARAMETERS
    data - Data from client
    conn_handle: Used to identify client

  RETURN VALUE
    None

  DEPENDENCIES
    qcmap_client_reg_cb should be called

  SIDE EFFECTS
    None
===========================================================================*/
void qcmap_set_roaming
(
  void* data,
  uint8 conn_handle
)
{
  qcmap_msgr_set_roaming_pref_req_msg_v01  *req_ptr = NULL;
  qcmap_msgr_set_roaming_pref_resp_msg_v01 *resp_msg;
  unsigned int     index;
  qcmap_softap_handle_type                 *map_handle;
  qcmap_error_type qcmap_err_num;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(data != NULL);

  QCMAP_LOG_MSG_MED("Client %d set roaming flag", conn_handle, 0, 0);

  resp_msg = (qcmap_msgr_set_roaming_pref_resp_msg_v01 *)malloc(sizeof(qcmap_msgr_set_roaming_pref_resp_msg_v01));
  if (resp_msg == NULL)
  {
    QCMAP_LOG_MSG_ERROR("Out of mem for resp msg", 0, 0, 0);
    ASSERT(0);
  }
  memset(resp_msg, 0, sizeof(qcmap_msgr_set_roaming_pref_resp_msg_v01));

  req_ptr =  (qcmap_msgr_set_roaming_pref_req_msg_v01*)data;

  /*---------------------------------------------------------------------

  ---------------------------------------------------------------------*/
  index = (req_ptr->mobile_ap_handle);
  map_handle = &qcmap_handle;

  if (index != QCMAP_SOFTAP_HANDLE || map_handle ->handle == 0)
  {
    QCMAP_LOG_MSG_MED("Incorrect handle %d passed", req_ptr->mobile_ap_handle, 0, 0);
    resp_msg->resp.result = QCMAP_RESULT_FAILURE;
    resp_msg->resp.error = QCMAP_ERR_INVALID_HANDLE;
    qcmap_send_msg(QCMAP_MOBILEAP_SET_ROAMING_RESP_EVT, (void *)resp_msg, conn_handle);

    if(resp_msg != NULL)
    {
      free(resp_msg);
      resp_msg = NULL;
    }
    if(req_ptr != NULL)
    {
      free(req_ptr);
      req_ptr = NULL;
    }
    return;
  }
  if (SetRoaming(req_ptr->allow_wwan_calls_while_roaming, &qcmap_err_num))
  {
    QCMAP_LOG_MSG_MED("Set Roaming flag successfully.", 0, 0, 0);
  }
  else
  {
    QCMAP_LOG_MSG_ERROR("Set Roaming flag failed!!", 0, 0, 0);
    resp_msg->resp.result = QCMAP_RESULT_FAILURE;
    resp_msg->resp.error = qcmap_err_num;
    qcmap_send_msg(QCMAP_MOBILEAP_SET_ROAMING_RESP_EVT, (void *)resp_msg, conn_handle);

    if(resp_msg != NULL)
    {
      free(resp_msg);
      resp_msg = NULL;
    }
    if(req_ptr != NULL)
    {
      free(req_ptr);
      req_ptr = NULL;
    }
    return;
  }

  qcmap_send_msg(QCMAP_MOBILEAP_SET_ROAMING_RESP_EVT, (void *)resp_msg, conn_handle);
 
  if(resp_msg != NULL)
  {
    free(resp_msg);
    resp_msg = NULL;
  }
  if(req_ptr != NULL)
  {
    free(req_ptr);
    req_ptr = NULL;
  }
  return;
}

/*===========================================================================
  FUNCTION QCMAP_SET_DMZIP()

  DESCRIPTION
    Set DMZ IP

  PARAMETERS
    data - Data from client
    conn_handle: Used to identify client

  RETURN VALUE
    None

  DEPENDENCIES
    qcmap_client_reg_cb should be called

  SIDE EFFECTS
    None
===========================================================================*/
void qcmap_set_dmzip
(
  void* data,
  uint8 conn_handle
)
{
  qcmap_msgr_set_dmz_req_msg_v01             *req_ptr = NULL;
  qcmap_msgr_set_dmz_resp_msg_v01            *resp_msg;
  unsigned int                               index;
  qcmap_softap_handle_type                   *map_handle;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(data != NULL);

  QCMAP_LOG_MSG_MED("Client %d set DMZ IP", conn_handle, 0, 0);

  resp_msg = (qcmap_msgr_set_dmz_resp_msg_v01 *)malloc(sizeof(qcmap_msgr_set_dmz_resp_msg_v01));
  if (resp_msg == NULL)
  {
    QCMAP_LOG_MSG_ERROR("Out of mem for resp msg", 0, 0, 0);
    ASSERT(0);
  }
  memset(resp_msg, 0, sizeof(qcmap_msgr_set_dmz_resp_msg_v01));

  req_ptr =  (qcmap_msgr_set_dmz_req_msg_v01*)data;

  /*---------------------------------------------------------------------*/

  index = (req_ptr->mobile_ap_handle);
  map_handle = &qcmap_handle;

  if (index != QCMAP_SOFTAP_HANDLE || map_handle ->handle == 0)
  {
    QCMAP_LOG_MSG_MED("Incorrect handle %d passed", req_ptr->mobile_ap_handle, 0, 0);
    resp_msg->resp.result = QCMAP_RESULT_FAILURE;
    resp_msg->resp.error = QCMAP_ERR_INVALID_HANDLE;
    qcmap_send_msg(QCMAP_MOBILEAP_SET_DMZIP_RESP_EVT, (void *)resp_msg, conn_handle);

    if(resp_msg != NULL)
    {
      free(resp_msg);
      resp_msg = NULL;
    }
    if(req_ptr != NULL)
    {
      free(req_ptr);
      req_ptr = NULL;
    }
    return;
  }

  // 1. Check DMZ IP is in DHCP range
  // 2. Store DMZ IP
  // 3. Invoke Networking stack API to turn on DMZ and also share IP address

  if(((qcmap_ip_t.dhcp_start_addr.a.addr4) <= (req_ptr->dmz_ip_addr)) &&
    ((req_ptr->dmz_ip_addr) <= (qcmap_ip_t.dhcp_end_addr.a.addr4)))
  {
    qc_mgr->dmz_addr = req_ptr->dmz_ip_addr;
    nat_enable_dmz(qc_mgr->dmz_addr, TRUE);
  }
  else
  {
    QCMAP_LOG_MSG_ERROR("Set DMZ IP failed!!", 0, 0, 0);
    resp_msg->resp.result = QCMAP_RESULT_FAILURE;
    resp_msg->resp.error = QCMAP_ERR_INTERNAL;
    qcmap_send_msg(QCMAP_MOBILEAP_SET_DMZIP_RESP_EVT, (void *)resp_msg, conn_handle);

    if(resp_msg != NULL)
    {
      free(resp_msg);
      resp_msg = NULL;
    }
    if(req_ptr != NULL)
    {
      free(req_ptr);
      req_ptr = NULL;
    }
    return;
  }

  qcmap_send_msg(QCMAP_MOBILEAP_SET_DMZIP_RESP_EVT, (void *)resp_msg, conn_handle);

  if(resp_msg != NULL)
  {
    free(resp_msg);
    resp_msg = NULL;
  }
  if(req_ptr != NULL)
  {
    free(req_ptr);
    req_ptr = NULL;
  }
  return;
  
}


/*===========================================================================
  FUNCTION QCMAP_DELETE_DMZIP()

  DESCRIPTION
    Set DMZ IP

  PARAMETERS
    data - Data from client
    conn_handle: Used to identify client

  RETURN VALUE
    None

  DEPENDENCIES
    qcmap_client_reg_cb should be called

  SIDE EFFECTS
    None
===========================================================================*/
void qcmap_delete_dmzip
(
  void* data,
  uint8 conn_handle
)
{
  qcmap_msgr_delete_dmz_req_msg_v01          *req_ptr = NULL;
  qcmap_msgr_delete_dmz_resp_msg_v01         *resp_msg;
  unsigned int                               index;
  qcmap_softap_handle_type                   *map_handle;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(data != NULL);

  QCMAP_LOG_MSG_MED("Client %d Delete DMZ IP", conn_handle, 0, 0);

  resp_msg = (qcmap_msgr_delete_dmz_resp_msg_v01 *)malloc(sizeof(qcmap_msgr_delete_dmz_resp_msg_v01));
  if (resp_msg == NULL)
  {
    QCMAP_LOG_MSG_ERROR("Out of mem for resp msg", 0, 0, 0);
    ASSERT(0);
  }
  memset(resp_msg, 0, sizeof(qcmap_msgr_delete_dmz_resp_msg_v01));

  req_ptr =  (qcmap_msgr_delete_dmz_req_msg_v01*)data;
  index = (req_ptr->mobile_ap_handle);
  map_handle = &qcmap_handle;

  if (index != QCMAP_SOFTAP_HANDLE || map_handle ->handle == 0)
  {
    QCMAP_LOG_MSG_MED("Incorrect handle %d passed", req_ptr->mobile_ap_handle, 0, 0);
    resp_msg->resp.result = QCMAP_RESULT_FAILURE;
    resp_msg->resp.error = QCMAP_ERR_INVALID_HANDLE;
    qcmap_send_msg(QCMAP_MOBILEAP_DELETE_DMZIP_RESP_EVT, (void *)resp_msg, conn_handle);
    if(resp_msg != NULL)
    {
      free(resp_msg);
      resp_msg = NULL;
    }
    if(req_ptr != NULL)
    {
      free(req_ptr);
      req_ptr = NULL;
    }
    return;
  }

  // 1. Match incoming IP with stored DMZ IP
  // 2. If same disable DMZ from nw stack
  // 3. Else return error

  if (req_ptr->dmz_ip_addr == qc_mgr->dmz_addr)
  {
    qc_mgr->dmz_addr = 0;
    nat_enable_dmz(qc_mgr->dmz_addr, FALSE);
  }
  else
  {
    QCMAP_LOG_MSG_ERROR("Delete DMZ IP failed!!", 0, 0, 0);
    resp_msg->resp.result = QCMAP_RESULT_FAILURE;
    resp_msg->resp.error = QCMAP_ERR_INTERNAL;
    qcmap_send_msg(QCMAP_MOBILEAP_DELETE_DMZIP_RESP_EVT, (void *)resp_msg, conn_handle);

    if(resp_msg != NULL)
    {
      free(resp_msg);
      resp_msg = NULL;
    }
    if(req_ptr != NULL)
    {
      free(req_ptr);
      req_ptr = NULL;
    }
    return;
  }

  qcmap_send_msg(QCMAP_MOBILEAP_DELETE_DMZIP_RESP_EVT, (void *)resp_msg, conn_handle);

  if(resp_msg != NULL)
  {
    free(resp_msg);
    resp_msg = NULL;
  }
  if(req_ptr != NULL)
  {
    free(req_ptr);
    req_ptr = NULL;
  }
  return;
}

void qcmap_set_dhcp_command
(
  void* data,
  uint8 conn_handle
)
{
  qcmap_msgr_set_dhcp_req_msg_v01  *req_ptr = NULL;
  qcmap_msgr_set_dhcp_resp_msg_v01 *resp_msg;
  qcmap_softap_handle_type                 *map_handle;
  unsigned int index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_assert(data != NULL);

  resp_msg = (qcmap_msgr_set_dhcp_resp_msg_v01 *)malloc(sizeof(qcmap_msgr_set_dhcp_resp_msg_v01));
  if (resp_msg == NULL)
  {
    QCMAP_LOG_MSG_ERROR("Out of mem for resp msg", 0, 0, 0);
    ASSERT(0);
  }
  memset(resp_msg, 0, sizeof(qcmap_msgr_set_dhcp_resp_msg_v01));

  req_ptr =  (qcmap_msgr_set_dhcp_req_msg_v01*)data;

  QCMAP_LOG_MSG_MED("Client %d set dhcp flag", conn_handle, 0, 0);

  /*---------------------------------------------------------------------

  ---------------------------------------------------------------------*/
  index = (req_ptr->mobile_ap_handle);
  map_handle = &qcmap_handle;

  if (index != QCMAP_SOFTAP_HANDLE ||map_handle->handle == 0)
  {
    QCMAP_LOG_MSG_MED("Incorrect handle %d passed", req_ptr->mobile_ap_handle, 0, 0);
    resp_msg->resp.result = QCMAP_RESULT_FAILURE;
    resp_msg->resp.error = QCMAP_ERR_INVALID_HANDLE;
    qcmap_send_msg(QCMAP_MOBILEAP_SET_DHCP_RESP_EVT, (void *)resp_msg, conn_handle);

    if(resp_msg != NULL)
    {
      free(resp_msg);
      resp_msg = NULL;
    }
    if(req_ptr != NULL)
    {
      free(req_ptr);
      req_ptr = NULL;
    }
    return ;
  }
  if (req_ptr->dhcp_enable == TRUE || req_ptr->dhcp_enable == FALSE)
  {
    set_dhcp_command(req_ptr->dhcp_enable);
    QCMAP_LOG_MSG_MED("Set dhcp flag successfully.", 0, 0, 0);
  }
  else
  {
    QCMAP_LOG_MSG_ERROR("Set dhcp failed!!Accepted values 0 or 1 only", 0, 0, 0);
    resp_msg->resp.result = QCMAP_RESULT_FAILURE;
    resp_msg->resp.error = QCMAP_ERR_INTERNAL;
    qcmap_send_msg(QCMAP_MOBILEAP_SET_DHCP_RESP_EVT, (void *)resp_msg, conn_handle);

    if(resp_msg != NULL)
    {
      free(resp_msg);
      resp_msg = NULL;
    }
    if(req_ptr != NULL)
    {
      free(req_ptr);
      req_ptr = NULL;
    }
    return ;
  }

  qcmap_send_msg(QCMAP_MOBILEAP_SET_DHCP_RESP_EVT, (void *)resp_msg, conn_handle);
 
  if(resp_msg != NULL)
  {
    free(resp_msg);
    resp_msg = NULL;
  }
  if(req_ptr != NULL)
  {
    free(req_ptr);
    req_ptr = NULL;
  }
  return;
}



/*===========================================================================
  FUNCTION SetAutoconnect
==========================================================================*/
/*!
@brief
  This function will enable the autoconnect flag.
  If the mobileap and auto connect is enabled, connect to backhaul is triggered.
  In case connect to backhaul fails, then a autoconnect timer is triggered. After the expiry of
  autoconnect timer a new request to connect to backhaul is initiated.

@parameters
  boolean enable

@return
  true  - on success
  false - on failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean SetAutoconnect
(
  boolean enable,
  qcmap_error_type *qcmap_err_num
)
{
  int qcmap_errno;
  int ret_val = QCMAP_FAILURE;
  char data[MAX_STRING_LENGTH] = {0};
  int fp = INVALID_FILE_HANDLE;
  char  str[128];

  QCMAP_LOG_MSG_LOW("SetAutoConnect() entered", 0, 0, 0);

  snprintf(data, sizeof(data), "%d", enable);
 if(qc_mgr != NULL)
 {
    /* Config Auto Connect */
    if( qc_mgr->qcmap_cm_handle != NULL)
    {
      ret_val = qcmap_cm_set_auto_connect(qc_mgr->qcmap_cm_handle, &qcmap_errno, enable, qcmap_err_num);
    }
  
    if (ret_val != QCMAP_SUCCESS)
    {
     return false;
    }
  
    qc_mgr->auto_connect = enable;
    qcmap_file_t.autoconnect = enable;
    efs_unlink(QCMAP_CONFIG_FILE);
    /* Update Autoconect config status to EFS file directly. */
  
    fp = efs_open(QCMAP_CONFIG_FILE, O_WRONLY|O_CREAT);
  
    if ( fp != INVALID_FILE_HANDLE )
    {
      snprintf(str, sizeof(str), "%s=%s\n", "Tech", qcmap_file_t.call_tech);
      efs_write( fp , str, strlen(str));
      snprintf(str, sizeof(str), "%s=%d\n", "Profile", qcmap_file_t.profile);
      efs_write( fp , str, strlen(str));
      snprintf(str, sizeof(str), "%s=%s\n", "Gateway_IP", qcmap_file_t.gateway_ip);
      efs_write( fp , str, strlen(str));
      snprintf(str, sizeof(str), "%s=%s\n", "Subnet", qcmap_file_t.subnet_mask);
      efs_write( fp , str, strlen(str));
      snprintf(str, sizeof(str), "%s=%s\n", "DHCP_Start", qcmap_file_t.dhcp_start);
      efs_write( fp , str, strlen(str));
      snprintf(str, sizeof(str), "%s=%s\n", "DHCP_End", qcmap_file_t.dhcp_end);
      efs_write( fp , str, strlen(str));
      snprintf(str, sizeof(str), "%s=%d\n", "DHCP_Lease", qcmap_file_t.dhcp_lease);
      efs_write( fp , str, strlen(str));
      snprintf(str, sizeof(str), "%s=%d\n", "Autoconnect", qcmap_file_t.autoconnect);
      efs_write( fp , str, strlen(str));
      snprintf(str, sizeof(str), "%s=%d\n", "Roaming", qcmap_file_t.roaming);
      efs_write( fp , str, strlen(str));
      snprintf(str, sizeof(str), "%s=%d\n", "DHCP_Enable", qcmap_file_t.dhcp_enable);
      efs_write( fp , str, strlen(str));
      snprintf(str, sizeof(str), "%s=%d\n", "STDECM_PublicIP", qcmap_file_t.stdecm_publicIP);
      efs_write( fp , str, strlen(str));
      snprintf(str, sizeof(str), "%s=%d\n", "APPS_Port_Start", qcmap_file_t.apps_port_start);
      efs_write( fp , str, strlen(str));
      snprintf(str, sizeof(str), "%s=%d\n", "APPS_Port_Range", qcmap_file_t.apps_port_range);
      efs_write( fp , str, strlen(str));
      snprintf(str, sizeof(str), "%s=%d\n", "NAT_TCP_Timeout", qcmap_file_t.nat_tcp_tmo);
      efs_write( fp , str, strlen(str));
      snprintf(str, sizeof(str), "%s=%d\n", "NAT_UDP_Timeout", qcmap_file_t.nat_udp_tmo);
      efs_write( fp , str, strlen(str));
      efs_close(fp);
      fp = INVALID_FILE_HANDLE;
    }
  
  
    return true;
 }
 return false;
}

/*===========================================================================
  FUNCTION GetAutoconnect
==========================================================================*/
/*!
@brief
  Returns autoconnect flag value.

@parameters
  value

@return
  true  - on success
  false - on failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/

boolean GetAutoconnect()
{

  int auto_flag;
  int val= false;

    if(qcmap_file_t.autoconnect != NULL)
    {
      auto_flag = qcmap_file_t.autoconnect;
      qc_mgr->auto_connect = auto_flag;
      val = true;
    }
    else
    {
     QCMAP_LOG_MSG_ERROR("Could not read value from EFS file: Getautoconnect()", 0, 0, 0);
     val = false;
    }

   return val;
}

/*===========================================================================
  FUNCTION SetRoaming
==========================================================================*/
/*!
@brief
  This function will set the roaming flag value. Enabling the roaming flag
  will enable connecting to a Network in roaming mode.

@parameters
  boolean enable

@return
  true  - on success
  false - on failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/

boolean SetRoaming
(
 boolean enable,
 qcmap_error_type *qcmap_err_num
)
{

  int ret_val;
  int qcmap_errno;
  char roaming_state[MAX_STRING_LENGTH]={0};
  int fp = INVALID_FILE_HANDLE;
  char  str[128];
  snprintf(roaming_state, sizeof(roaming_state), "%d", enable);

  if(qc_bck_wan)
  {
    /* Config Roaming flag */
    ret_val = qcmap_cm_set_roaming(qc_mgr->qcmap_cm_handle, qcmap_err_num, enable);

    if (ret_val != QCMAP_SUCCESS)
    {
      return false;
    }

    qc_bck_wan->wwan_cfg.roaming = enable;
  }

  /* if it is OK, Update Roaming config flag  to EFS file directly. */
  qcmap_file_t.roaming = enable;
  efs_unlink(QCMAP_CONFIG_FILE);
  fp = efs_open(QCMAP_CONFIG_FILE, O_WRONLY|O_CREAT);

  if ( fp != INVALID_FILE_HANDLE )
  {
    snprintf(str, sizeof(str), "%s=%s\n", "Tech", qcmap_file_t.call_tech);
    efs_write( fp , str, strlen(str));
    snprintf(str, sizeof(str), "%s=%d\n", "Profile", qcmap_file_t.profile);
    efs_write( fp , str, strlen(str));
    snprintf(str, sizeof(str), "%s=%s\n", "Gateway_IP", qcmap_file_t.gateway_ip);
    efs_write( fp , str, strlen(str));
    snprintf(str, sizeof(str), "%s=%s\n", "Subnet", qcmap_file_t.subnet_mask);
    efs_write( fp , str, strlen(str));
    snprintf(str, sizeof(str), "%s=%s\n", "DHCP_Start", qcmap_file_t.dhcp_start);
    efs_write( fp , str, strlen(str));
    snprintf(str, sizeof(str), "%s=%s\n", "DHCP_End", qcmap_file_t.dhcp_end);
    efs_write( fp , str, strlen(str));
    snprintf(str, sizeof(str), "%s=%d\n", "DHCP_Lease", qcmap_file_t.dhcp_lease);
    efs_write( fp , str, strlen(str));
    snprintf(str, sizeof(str), "%s=%d\n", "Autoconnect", qcmap_file_t.autoconnect);
    efs_write( fp , str, strlen(str));
    snprintf(str, sizeof(str), "%s=%d\n", "Roaming", qcmap_file_t.roaming);
    efs_write( fp , str, strlen(str));
    snprintf(str, sizeof(str), "%s=%d\n", "DHCP_Enable", qcmap_file_t.dhcp_enable);
    efs_write( fp , str, strlen(str));
    snprintf(str, sizeof(str), "%s=%d\n", "STDECM_PublicIP", qcmap_file_t.stdecm_publicIP);
    efs_write( fp , str, strlen(str));
    snprintf(str, sizeof(str), "%s=%d\n", "APPS_Port_Start", qcmap_file_t.apps_port_start);
    efs_write( fp , str, strlen(str));
    snprintf(str, sizeof(str), "%s=%d\n", "APPS_Port_Range", qcmap_file_t.apps_port_range);
    efs_write( fp , str, strlen(str));
    snprintf(str, sizeof(str), "%s=%d\n", "NAT_TCP_Timeout", qcmap_file_t.nat_tcp_tmo);
    efs_write( fp , str, strlen(str));
    snprintf(str, sizeof(str), "%s=%d\n", "NAT_UDP_Timeout", qcmap_file_t.nat_udp_tmo);
    efs_write( fp , str, strlen(str));

    efs_close(fp);
    fp = INVALID_FILE_HANDLE;
  }


  if (qc_mgr->auto_connect)
  {
    /* Config Auto Connect */
    ret_val = qcmap_cm_set_auto_connect(qc_mgr->qcmap_cm_handle, &qcmap_errno, true, qcmap_err_num);

    if (ret_val != QCMAP_SUCCESS)
      return false;
  }

  return true;
}


/*===========================================================================
  FUNCTION GetRoaming
==========================================================================*/
/*!
@brief
  Returns roaming flag value.

@parameters
  void

@return
  true  - on success
  false - on failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/

boolean GetRoaming()
{
  int roaming_flag;
  int val= false;

    if(qcmap_file_t.roaming != NULL)
    {
      roaming_flag = qcmap_file_t.roaming;
      qc_bck_wan->wwan_cfg.roaming = roaming_flag;
      val = true;
    }
    else
    {
     QCMAP_LOG_MSG_ERROR("Could not read value from EFS file: GetRoaming()", 0, 0, 0);
     val = false;
    }

   return val;
}

/*===========================================================================

FUNCTION QCMAP_CM_SET_AUTO_CONNECT()

DESCRIPTION

  It will set autoconnect.

DEPENDENCIES
  None.

RETURN VALUE

  qcmap_errno Values
  ----------------
  On success, returns QCMAP_SUCCESS.
  On error, return QCMAP_ERROR and places the error condition value in
  *qcmap_errno.


SIDE EFFECTS

===========================================================================*/
int qcmap_cm_set_auto_connect
(
 int      qcmap_handle,                   /* Handle for MobileAP CM    */
 int     *qcmap_errno,                    /* Error condition value     */
 boolean  auto_connect,                      /* Autoconnect Enable or Not */
 qcmap_error_type *qcmap_err_num
 )
{
  int ret = QCMAP_SUCCESS;
  nas_indication_register_req_msg_v01 qcmap_nas_indication_register_req_msg_v01;
  nas_indication_register_resp_msg_v01 qcmap_nas_indication_register_resp_msg_v01;
  qmi_client_error_type qmi_error;

  ds_assert(qcmap_errno != NULL);
  ds_assert(qcmap_err_num != NULL);

  QCMAP_LOG_MSG_MED("qcmap_cm_set_auto_connect: enter", 0, 0, 0);


  if (qcmap_handle != qcmap_cb.handle)
  {
    *qcmap_errno = QCMAP_EBADAPP;
    ret = QCMAP_ERROR;
    *qcmap_err_num = QCMAP_ERR_INVALID_HANDLE;
    QCMAP_LOG_MSG_ERROR( "Wrong QCMAP CM Handle", 0, 0, 0);
    return ret;
  }

  if (qcmap_cb.state == QCMAP_DISABLE && qcmap_cb.ipv6_state == QCMAP_V6_DISABLE)
  {
    *qcmap_errno = QCMAP_EOPNOTSUPP;
    ret = QCMAP_ERROR;
    *qcmap_err_num = QCMAP_ERR_NO_EFFECT;
    QCMAP_LOG_MSG_ERROR( "QCMAP CM disable state", 0, 0, 0);
    return ret;
  }

  qc_mgr->auto_connect = auto_connect;


  /*if autoconnect is enabled register to serving system NAS indication and connect backhaul*/
  if(qc_mgr->auto_connect)
  {
    memset(&qcmap_nas_indication_register_req_msg_v01, 0, sizeof(nas_indication_register_req_msg_v01));
    qcmap_nas_indication_register_req_msg_v01.sys_info_valid = TRUE;
    qcmap_nas_indication_register_req_msg_v01.sys_info = 0x01;
    QCMAP_LOG_MSG_MED("Registering for serving system NAS indications", 0, 0, 0);
    qmi_error = qmi_client_send_msg_sync(qcmap_cb.qmi_nas_handle,
                                         QMI_NAS_INDICATION_REGISTER_REQ_MSG_V01,
                                         &qcmap_nas_indication_register_req_msg_v01,
                                         sizeof(qcmap_nas_indication_register_req_msg_v01),
                                         &qcmap_nas_indication_register_resp_msg_v01,
                                         sizeof(qcmap_nas_indication_register_resp_msg_v01),
                                         QCMAP_QMI_TIMEOUT_VALUE);

    if(qmi_error != QMI_NO_ERR)
    {
      QCMAP_LOG_MSG_ERROR("Failed to register to NAS indications %d", qmi_error, 0, 0);
      return QCMAP_ERROR;
    }

    QCMAP_LOG_MSG_MED("Starting backhaul connection due to autoconnect", 0, 0, 0);
    qcmap_cb.backhaul_service = false;

    if (qcmap_cb.state == QCMAP_ENABLE)
    {
      if( qc_mgr->auto_connect_timer_running_v4 )
      {
        stop_auto_timer(QCMAP_MSGR_IP_FAMILY_V4_V01);
      }
      qcmap_connect_backhaul(qcmap_cb.handle, QCMAP_MSGR_WWAN_CALL_TYPE_V4_V01, qcmap_errno, qcmap_err_num);
    }
    if (qcmap_cb.ipv6_state == QCMAP_V6_ENABLE)
    {
      if( qc_mgr->auto_connect_timer_running_v6 )
      {
        stop_auto_timer(QCMAP_MSGR_IP_FAMILY_V6_V01);
      }
      qcmap_connect_backhaul(qcmap_cb.handle, QCMAP_MSGR_WWAN_CALL_TYPE_V6_V01, qcmap_errno, qcmap_err_num);
    }
  }
  return QCMAP_SUCCESS;

}


/*===========================================================================

FUNCTION QCMAP_CM_SET_ROAMING()

DESCRIPTION

  It will set roaming.

DEPENDENCIES
  None.

RETURN VALUE

  qcmap_errno Values
  ----------------
  On success, returns QCMAP_SUCCESS.
  On error, return QCMAP_ERROR and places the error condition value in
  *qcmap_errno.


SIDE EFFECTS

===========================================================================*/
int qcmap_cm_set_roaming
(
  int      qcmap_handle,                      /* Handle for MobileAP CM */
  qcmap_error_type *qcmap_err_num,               /* Error condition value  */
  boolean  roaming                               /* Roaming Enable or Not  */
)
{
  int ret = QCMAP_SUCCESS;

  ds_assert(qcmap_err_num != NULL);

  QCMAP_LOG_MSG_MED("qcmap_cm_set_roaming: enter", 0, 0, 0);

  if (qcmap_handle != qcmap_cb.handle)
  {
    *qcmap_err_num = QCMAP_ERR_INVALID_HANDLE;
    ret = QCMAP_ERROR;
    QCMAP_LOG_MSG_ERROR( "Wrong QCMAP CM Handle", 0, 0, 0);
    return ret;
  }

  if (qcmap_cb.state == QCMAP_DISABLE)
  {
    *qcmap_err_num = QCMAP_ERR_NO_EFFECT;
    ret = QCMAP_ERROR;
    QCMAP_LOG_MSG_ERROR( "QCMAP CM disable state", 0, 0, 0);
    return ret;
  }

  qc_bck_wan->wwan_cfg.roaming = roaming;
  return ret;
}

/*================================================================

FUNCTION qcmap_ph_dtr_changed_cb

DESCRIPTION
  Callback function to handle DTR signal notification from peripheral

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None

==================================================================*/
void qcmap_ph_dtr_changed_cb (void)
{
  qcmap_cmd_t*             cmd_ptr = NULL;

  QCMAP_LOG_MSG_HIGH ("Entering: qcmap_ph_dtr_changed_cb ", 0, 0, 0 );

  if((cmd_ptr = qcmap_get_cmd_buffer()) == NULL)
  {
    QCMAP_LOG_MSG_ERROR("Cannot obtain command buffer. Assert", 0, 0, 0);
    ASSERT(0);
  }
  else
  {
    cmd_ptr->cmd_hdr.cmd_id              = QCMAP_DTR_CHANGE_EVT;

    qcmap_enqueue_cmd(cmd_ptr);
  }

  qcmap_set_signal();
  return;
}

/*===========================================================================
  FUNCTION QCMAP_UPDATE_DTR_REG()

  DESCRIPTION
    The function updates the DTR callback registration with the peripheral

  RETURN VALUE
    None

  DEPENDENCIES
    qcmap_client_reg_cb should be called

  SIDE EFFECTS
    None
===========================================================================*/
void qcmap_update_dtr_reg(uint8 stream_id)
{
  sio_ioctl_param_type ioctl_param;

  QCMAP_LOG_MSG_MED("qcmap_update_dtr_reg: enter", 0, 0, 0);

  qc_mgr->qcmap_ph_stream_id = stream_id;
/*===================================================
  Register DTR Call back
====================================================*/
  ioctl_param.enable_dte_ready_event = qcmap_ph_dtr_changed_cb;

  sio_control_ioctl (qc_mgr->qcmap_ph_stream_id ,
                     SIO_IOCTL_ENABLE_DTR_EVENT, &ioctl_param);

}

/*===========================================================================
  FUNCTION QCMAP_GET_LINE_STATE()

  DESCRIPTION
    The function gets the Line state from the peripheral

  RETURN VALUE
    boolean with the line state value

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qcmap_get_line_state(void)
{
  sio_ioctl_param_type ioctl_param;
  boolean line_state = FALSE;


  ioctl_param.dte_ready_asserted = &line_state;

  sio_control_ioctl (qc_mgr->qcmap_ph_stream_id ,
             SIO_IOCTL_DTE_READY_ASSERTED,
             (void*)&ioctl_param);

  QCMAP_LOG_MSG_HIGH ("qti_get_line_state %d ",line_state, 0, 0 );

  qc_mgr->dtr_enabled = line_state;

  return line_state;
}

/*===========================================================================
  FUNCTION ()

  DESCRIPTION
   Function to get the mac address for ecm interface

  RETURN VALUE
    boolean with the line state value

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qcmap_get_mac_address(void)
{
  sio_ioctl_param_type ioctl_param;
  int idx, i,temp1, temp2;
  uint8 mac_addr[13];
  uint8 res_mac_addr[QCMAP_MSGR_MAC_ADDR_LEN];

  QCMAP_LOG_MSG_HIGH ("qcmap_get_mac_address %d ",0, 0, 0 );

  memset(res_mac_addr,0,QCMAP_MSGR_MAC_ADDR_LEN);

  sio_control_ioctl (qc_mgr->qcmap_ph_stream_id ,
             SIO_IOCTL_GET_MAC_ADDRESS,
             (void*)&ioctl_param);
  ds_memcpy(mac_addr,ioctl_param.mac_address,13);

  for (idx = 0; idx<QCMAP_MSGR_MAC_ADDR_LEN*2 ;idx++)
  {
    if (0x41 <= (mac_addr[idx]) &&  (mac_addr[idx]) <= 0x46)
    {
      mac_addr[idx] = (mac_addr[idx] - 'A')+10;
    }
    else if (0x61 <= (mac_addr[idx]) &&  (mac_addr[idx]) <= 0x66)
    {
      mac_addr[idx] = (mac_addr[idx] - 'a')+10;
    }
    else if (0x30<= (mac_addr[idx]) && (mac_addr[idx]) <=0x39)
    {
      mac_addr[idx] = (mac_addr[idx] - '0');
    }
  }


  for (idx = 0,i=0; idx < QCMAP_MSGR_MAC_ADDR_LEN ;idx++)
  {
    temp1 = mac_addr[i];
    temp2 = mac_addr[i+1];
    res_mac_addr[idx] = (temp1<<4)|temp2;
    i=i+2;
  }

  ecm_set_mac(res_mac_addr);
  return ;
}


/*============================================================
  FUNCTION  qcmap_cm_process_qmi_service_availabilty
=============================================================
@brief
 Initiate QMI Client for the QMI service which is available

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None

 ==========================================================*/
void qcmap_cm_process_qmi_service_availabilty
(
  qcmap_qmi_service_t  qmi_service_id
)
{
  switch (qmi_service_id)
  {
    case QCMAP_QMI_NAS_IN_SERVICE:
      if ( qmi_nas_client_init() == QCMAP_ERROR )
      {
        QCMAP_LOG_MSG_ERROR("Not able to Init QMI NAS client", 0, 0, 0);
      }
      break;

    case QCMAP_QMI_NAS_NOT_IN_SERVICE:
      if ( qmi_nas_not_in_service() == QCMAP_ERROR )
      {
        QCMAP_LOG_MSG_ERROR("Not able to clean up QMI NAS context during SSR", 0, 0, 0);
      }
      break;

    default:
     QCMAP_LOG_MSG_ERROR("Invalid QMI service =%d", qmi_service_id, 0, 0);
     break;
  }
  return;
}

/*============================================================
  FUNCTION qmi_nas_client_init
==============================================================
@brief
 QMI NAS Client Init

@return
  QCMAP_ERROR in case of error
  QCMAP_SUCCESS for sucess

@note

  - Dependencies
    - None

  - Side Effects
    - None
==========================================================*/

int qmi_nas_client_init()
{
  int ret_val;
  qmi_client_error_type qmi_error;
  qcmap_error_type qcmap_err_num;
  int qcmap_errno;
  qmi_idl_service_object_type nas_qmi_idl_service_object;
  nas_indication_register_req_msg_v01 qcmap_nas_indication_register_req_msg_v01;
  nas_indication_register_resp_msg_v01 qcmap_nas_indication_register_resp_msg_v01;

  nas_qmi_idl_service_object = nas_get_service_object_v01();
  if (nas_qmi_idl_service_object == NULL)
  {
    QCMAP_LOG_MSG_ERROR("QCMAP service object(nas) not available.", 0, 0, 0);
    return QCMAP_ERROR;
  }


  qmi_error = qmi_client_release(qcmap_cb.qmi_nas_notifier);
  qcmap_cb.qmi_nas_notifier = NULL;

  if (qmi_error != QMI_NO_ERR)
  {
    QCMAP_LOG_MSG_ERROR("Cannot release client nas notifier %d",
                   qmi_error, 0, 0);
    return QCMAP_ERROR;
  }

  qmi_error = qmi_client_init_instance(nas_qmi_idl_service_object,
                                       QMI_CLIENT_INSTANCE_ANY,
                                       qcmap_cm_qmi_nas_ind,
                                       NULL,
                                       &qcmap_cb.qmi_nas_os_params,
                                       QCMAP_QMI_TIMEOUT_VALUE,
                                       &qcmap_cb.qmi_nas_handle);

  if(qmi_error != QMI_NO_ERR)
  {
    QCMAP_LOG_MSG_ERROR("Failed to init NAS qmi client %d", qmi_error, 0, 0);
    if(qcmap_cb.qmi_nas_handle != NULL)
    {
      qmi_client_release(qcmap_cb.qmi_nas_handle);
      qcmap_cb.qmi_nas_handle = NULL;
    }
    return QCMAP_ERROR;
  }

  QCMAP_LOG_MSG_MED("QCMAP: Nas client inited %d", qcmap_cb.qmi_nas_handle, 0, 0);
  /*deregister from all NAS indications*/
  memset(&qcmap_nas_indication_register_req_msg_v01, 0, sizeof(nas_indication_register_req_msg_v01));

  qcmap_nas_indication_register_req_msg_v01.dual_standby_pref_valid = TRUE;
  qcmap_nas_indication_register_req_msg_v01.dual_standby_pref = 0x00;
  qcmap_nas_indication_register_req_msg_v01.err_rate_valid = TRUE;
  qcmap_nas_indication_register_req_msg_v01.err_rate = 0x00;
  qcmap_nas_indication_register_req_msg_v01.network_reject_valid = TRUE;
  qcmap_nas_indication_register_req_msg_v01.network_reject.reg_network_reject = 0x00;
  qcmap_nas_indication_register_req_msg_v01.reg_csp_plmn_mode_bit_valid = TRUE;
  qcmap_nas_indication_register_req_msg_v01.reg_csp_plmn_mode_bit =0x00;
  qcmap_nas_indication_register_req_msg_v01.reg_current_plmn_name_valid = TRUE;
  qcmap_nas_indication_register_req_msg_v01.reg_current_plmn_name = 0x00;
  qcmap_nas_indication_register_req_msg_v01.reg_ddtm_events_valid = TRUE;
  qcmap_nas_indication_register_req_msg_v01.reg_ddtm_events = 0x00;
  qcmap_nas_indication_register_req_msg_v01.reg_embms_status_valid = TRUE;
  qcmap_nas_indication_register_req_msg_v01.reg_embms_status = 0x00;
  qcmap_nas_indication_register_req_msg_v01.reg_hdr_session_close_valid = TRUE;
  qcmap_nas_indication_register_req_msg_v01.reg_hdr_session_close = 0x00;
  qcmap_nas_indication_register_req_msg_v01.reg_hdr_uati_valid = TRUE;
  qcmap_nas_indication_register_req_msg_v01.reg_hdr_uati = 0x00;
  qcmap_nas_indication_register_req_msg_v01.reg_managed_roaming_valid = TRUE;
  qcmap_nas_indication_register_req_msg_v01.reg_managed_roaming = 0x00;
  qcmap_nas_indication_register_req_msg_v01.reg_network_time_valid = TRUE;
  qcmap_nas_indication_register_req_msg_v01.reg_network_time = 0x00;
  qcmap_nas_indication_register_req_msg_v01.reg_operator_name_data_valid = TRUE;
  qcmap_nas_indication_register_req_msg_v01.reg_operator_name_data = 0x00;
  qcmap_nas_indication_register_req_msg_v01.reg_rf_band_info_valid = TRUE;
  qcmap_nas_indication_register_req_msg_v01.reg_rf_band_info = 0x00;
  qcmap_nas_indication_register_req_msg_v01.reg_rtre_cfg_valid = TRUE;
  qcmap_nas_indication_register_req_msg_v01.reg_rtre_cfg = 0x00;
  qcmap_nas_indication_register_req_msg_v01.reg_sys_sel_pref_valid = TRUE;
  qcmap_nas_indication_register_req_msg_v01.reg_sys_sel_pref = 0x00;
  qcmap_nas_indication_register_req_msg_v01.sig_info_valid = TRUE;
  qcmap_nas_indication_register_req_msg_v01.sig_info = 0x00;
  qcmap_nas_indication_register_req_msg_v01.subscription_info_valid = TRUE;
  qcmap_nas_indication_register_req_msg_v01.subscription_info = 0x00;
  qcmap_nas_indication_register_req_msg_v01.req_serving_system_valid = TRUE;
  qcmap_nas_indication_register_req_msg_v01.req_serving_system = 0x00;

  /*register for serving system NAS indication if autoconnect is enabled*/
  if(qc_mgr->auto_connect)
  {
    QCMAP_LOG_MSG_MED("Registering for NAS Sys Info indications", 0, 0, 0);
    qcmap_nas_indication_register_req_msg_v01.sys_info_valid = TRUE;
    qcmap_nas_indication_register_req_msg_v01.sys_info = 0x01;
  }
  else
  {
    QCMAP_LOG_MSG_MED("Deregistering from NAS Sys info indications", 0, 0, 0);
    qcmap_nas_indication_register_req_msg_v01.sys_info_valid = TRUE;
    qcmap_nas_indication_register_req_msg_v01.sys_info = 0x00;
  }

  qmi_error = qmi_client_send_msg_sync(qcmap_cb.qmi_nas_handle,
                                       QMI_NAS_INDICATION_REGISTER_REQ_MSG_V01,
                                       &qcmap_nas_indication_register_req_msg_v01,
                                       sizeof(qcmap_nas_indication_register_req_msg_v01),
                                       &qcmap_nas_indication_register_resp_msg_v01,
                                       sizeof(qcmap_nas_indication_register_resp_msg_v01),
                                       QCMAP_QMI_TIMEOUT_VALUE);
  if (qmi_error != QMI_NO_ERR)
  {
    qmi_client_release(qcmap_cb.qmi_nas_handle);
    qcmap_cb.qmi_nas_handle = NULL;
    QCMAP_LOG_MSG_ERROR( "Can not perform NAS indication register %d",
                  qmi_error, 0, 0);
    return QCMAP_ERROR;
  }

  QCMAP_LOG_MSG_MED("NAS indication registration success", 0, 0, 0);

  if( qc_mgr->auto_connect )
  {
    {
      ret_val = qcmap_connect_backhaul(qcmap_cb.handle,
                            QCMAP_MSGR_WWAN_CALL_TYPE_V4_V01, &qcmap_errno, &qcmap_err_num);
      if (ret_val == QCMAP_ERROR && qcmap_errno == QCMAP_EWOULDBLOCK)
      {
        QCMAP_LOG_MSG_MED("QCMAP WAN Connecting Inprogress ", 0, 0, 0);
      }
      else
      {
        QCMAP_LOG_MSG_MED( "QCMAP WAN Connecting Fail, ret_val %d qcmap_errno %d",
            ret_val, qcmap_errno,0 );
      }
    }

    {
      ret_val = qcmap_connect_backhaul(qcmap_cb.handle,
                            QCMAP_MSGR_WWAN_CALL_TYPE_V6_V01, &qcmap_errno, &qcmap_err_num);

      if (ret_val == QCMAP_ERROR && qcmap_errno == QCMAP_EWOULDBLOCK)

      {
        QCMAP_LOG_MSG_MED("QCMAP IPV6 WAN Connecting Inprogress ", 0, 0, 0);
      }
      else
      {
        QCMAP_LOG_MSG_MED( "QCMAP IPV6 WAN Connecting Fail, ret_val %d qcmap_errno %d",
            ret_val, qcmap_errno,0 );
      }
    }
  }
  return QCMAP_SUCCESS;
}

/*============================================================
  FUNCTION qmi_nas_not_in_service
=============================================================

@brief
   Function invoked during SSR functionality, when modem is not in service.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
==========================================================*/
int qmi_nas_not_in_service()
{
  qmi_client_error_type                 qmi_error;
  qmi_idl_service_object_type nas_qmi_idl_service_object;

  nas_qmi_idl_service_object = nas_get_service_object_v01();
  if (nas_qmi_idl_service_object == NULL)
  {
    QCMAP_LOG_MSG_ERROR("qcpmap service object(nas) not available.", 0, 0, 0);
    return QCMAP_ERROR;
  }

  qmi_error = qmi_client_release(qcmap_cb.qmi_nas_handle);
  qcmap_cb.qmi_nas_handle = NULL;
  if (qmi_error != QMI_NO_ERR)
  {
    QCMAP_LOG_MSG_ERROR("Cannot release NAS client %d", qmi_error, 0, 0);
    return QCMAP_ERROR;
  }


/*----------------------------------------------------------------------------
  Invoke dss_release
---------------------------------------------------------------------------- */
  if(qapi_DSS_Release((int) QAPI_DSS_MODE_GENERAL) != QAPI_DSS_SUCCESS)
  {
    QCMAP_LOG_MSG_ERROR("Error in dss_release", 0, 0, 0);
    return QCMAP_ERROR;
  }
  QCMAP_LOG_MSG_MED("DSS Release Success during SSR", 0, 0, 0);

/*----------------------------------------------------------------------------
    Register callback to get notified when modem is in service
---------------------------------------------------------------------------- */
    (void) qmi_client_notifier_init(nas_qmi_idl_service_object,
                                    &qcmap_cb.qmi_nas_os_params,
                                    &qcmap_cb.qmi_nas_notifier);

    (void) qmi_client_register_notify_cb(qcmap_cb.qmi_nas_notifier,
                                         qmi_nas_service_available_cb,
                                         NULL);
  return QCMAP_SUCCESS;
}


/*===========================================================================
  FUNCTION  qcmap_cm_qmi_nas_ind
===========================================================================*/
/*!
@brief
 Receives an incoming QMI NAS Indication.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
void
qcmap_cm_qmi_nas_ind
(
  qmi_client_type user_handle,                    /* QMI user handle       */
  unsigned int    msg_id,                         /* Indicator message ID  */
  void           *ind_buf,                        /* Raw indication data   */
  unsigned int    ind_buf_len,                    /* Raw data length       */
  void           *ind_cb_data                     /* User call back handle */
)
{
  qcmap_cmd_t*             cmd_ptr = NULL;
  qcmap_nas_buffer_t       *qcmap_nas_buffer;

  qcmap_nas_buffer = (qcmap_nas_buffer_t *)malloc(sizeof(qcmap_nas_buffer_t));

  if(qcmap_nas_buffer == NULL)
  {
    QCMAP_LOG_MSG_ERROR("Cannot assign memory for nas buffer. Assert", 0, 0, 0);
    ASSERT(0);
  }
   memset(qcmap_nas_buffer, 0, sizeof(qcmap_nas_buffer_t));
   
  qcmap_nas_buffer->ind_buf = ( void *)malloc(ind_buf_len);
  
  if(qcmap_nas_buffer->ind_buf == NULL)
  {
    QCMAP_LOG_MSG_ERROR("Cannot assign memory for ind_buf buffer. Assert", 0, 0, 0);
    ASSERT(0);
  }
  	
 

  qcmap_nas_buffer->user_handle = user_handle;
  qcmap_nas_buffer->msg_id      = msg_id;
  ds_memcpy(qcmap_nas_buffer->ind_buf,ind_buf,ind_buf_len);
  qcmap_nas_buffer->ind_buf_len = ind_buf_len;

  QCMAP_LOG_MSG_HIGH ("Entering: qcmap_cm_qmi_nas_ind ", 0, 0, 0 );

  if((cmd_ptr = qcmap_get_cmd_buffer()) == NULL)
  {
    QCMAP_LOG_MSG_ERROR("Cannot obtain command buffer. Assert", 0, 0, 0);
    ASSERT(0);
  }
  else
  {
    cmd_ptr->cmd_hdr.cmd_id              = QCMAP_QMI_NAS_IND_EVT;
    cmd_ptr->cmd_data.data               = (void *)qcmap_nas_buffer;

    qcmap_enqueue_cmd(cmd_ptr);
  }

  qcmap_set_signal();

  return;

}

/*===========================================================================
  FUNCTION  qcmap_cm_process_qmi_nas_ind
===========================================================================*/
/*!
@brief
 Processes an incoming QMI NAS Indication.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
void
qcmap_cm_process_qmi_nas_ind
(
  qcmap_nas_buffer_t *qcmap_nas_buffer
)
{
  qmi_client_error_type qmi_error = QMI_NO_ERR;
  nas_sys_info_ind_msg_v01 sys_info_ind;
  nas_service_domain_enum_type_v01 srv_domain         = NAS_SERVICE_DOMAIN_ENUM_TYPE_MIN_ENUM_VAL_V01;
  nas_service_domain_enum_type_v01 srv_domain_lte     = NAS_SERVICE_DOMAIN_ENUM_TYPE_MIN_ENUM_VAL_V01;
  nas_service_domain_enum_type_v01 srv_domain_hdr     = NAS_SERVICE_DOMAIN_ENUM_TYPE_MIN_ENUM_VAL_V01;
  nas_service_domain_enum_type_v01 srv_domain_cdma    = NAS_SERVICE_DOMAIN_ENUM_TYPE_MIN_ENUM_VAL_V01;
  nas_service_domain_enum_type_v01 srv_domain_wcdma   = NAS_SERVICE_DOMAIN_ENUM_TYPE_MIN_ENUM_VAL_V01;
  nas_service_domain_enum_type_v01 srv_domain_gsm     = NAS_SERVICE_DOMAIN_ENUM_TYPE_MIN_ENUM_VAL_V01;
  nas_service_domain_enum_type_v01 srv_domain_tdscdma = NAS_SERVICE_DOMAIN_ENUM_TYPE_MIN_ENUM_VAL_V01;
  nas_service_status_enum_type_v01 srv_status         = NAS_SERVICE_STATUS_ENUM_TYPE_MIN_ENUM_VAL_V01;
  nas_service_status_enum_type_v01 srv_status_lte     = NAS_SERVICE_STATUS_ENUM_TYPE_MIN_ENUM_VAL_V01;
  nas_service_status_enum_type_v01 srv_status_hdr     = NAS_SERVICE_STATUS_ENUM_TYPE_MIN_ENUM_VAL_V01;
  nas_service_status_enum_type_v01 srv_status_cdma    = NAS_SERVICE_STATUS_ENUM_TYPE_MIN_ENUM_VAL_V01;
  nas_service_status_enum_type_v01 srv_status_wcdma   = NAS_SERVICE_STATUS_ENUM_TYPE_MIN_ENUM_VAL_V01;
  nas_service_status_enum_type_v01 srv_status_gsm     = NAS_SERVICE_STATUS_ENUM_TYPE_MIN_ENUM_VAL_V01;
  nas_service_status_enum_type_v01 srv_status_tdscdma = NAS_SERVICE_STATUS_ENUM_TYPE_MIN_ENUM_VAL_V01;
  nas_roam_status_enum_type_v01 roam_status = NAS_ROAM_STATUS_ENUM_TYPE_MIN_ENUM_VAL_V01;
  int               err;
  int              *qcmap_errno = &err;
  qcmap_error_type qcmap_err_num;

  qmi_client_type user_handle;
  unsigned int    msg_id;
  void           *ind_buf;
  unsigned int    ind_buf_len;

  ds_assert(qcmap_nas_buffer);
  ds_assert(qcmap_nas_buffer->user_handle);
  ds_assert(qcmap_nas_buffer->ind_buf);


  user_handle = qcmap_nas_buffer->user_handle;
  msg_id      = qcmap_nas_buffer->msg_id;
  ind_buf     = qcmap_nas_buffer->ind_buf;
  ind_buf_len = qcmap_nas_buffer->ind_buf_len;


  QCMAP_LOG_MSG_MED("qcmap_cm_process_qmi_nas_ind: user_handle %d msg_id %d ind_buf_len %d.",
                user_handle,
                msg_id,
                ind_buf_len);

  memset(&sys_info_ind, 0, sizeof(nas_sys_info_ind_msg_v01));

  /* Process based on indication type */
  switch (msg_id)
  {
    case QMI_NAS_SYS_INFO_IND_MSG_V01:
      qmi_error = qmi_client_message_decode(user_handle,
                                            QMI_IDL_INDICATION,
                                            msg_id,
                                            ind_buf,
                                            ind_buf_len,
                                            &sys_info_ind,
                                            sizeof(nas_sys_info_ind_msg_v01));
      if (qmi_error == QMI_NO_ERR)
      {
        /* Local domain and status variables */
        srv_domain_lte     = sys_info_ind.lte_sys_info.common_sys_info.srv_domain;
        srv_domain_hdr     = sys_info_ind.hdr_sys_info.common_sys_info.srv_domain;
        srv_domain_cdma    = sys_info_ind.cdma_sys_info.common_sys_info.srv_domain;
        srv_domain_wcdma   = sys_info_ind.wcdma_sys_info.common_sys_info.srv_domain;
        srv_domain_gsm     = sys_info_ind.gsm_sys_info.common_sys_info.srv_domain;
        srv_domain_tdscdma = sys_info_ind.tdscdma_sys_info.common_sys_info.srv_domain;
        srv_status_lte     = sys_info_ind.lte_srv_status_info.srv_status;
        srv_status_hdr     = sys_info_ind.hdr_srv_status_info.srv_status;
        srv_status_cdma    = sys_info_ind.cdma_srv_status_info.srv_status;
        srv_status_wcdma   = sys_info_ind.wcdma_srv_status_info.srv_status;
        srv_status_gsm     = sys_info_ind.gsm_srv_status_info.srv_status;
        srv_status_tdscdma = sys_info_ind.tdscdma_srv_status_info.srv_status;

        /* First Get the Service Domain. */
        /* If the LTE System Info is valid, check the LTE Service Domain. */
        if (sys_info_ind.lte_sys_info_valid == TRUE &&
            sys_info_ind.lte_sys_info.common_sys_info.srv_domain_valid == TRUE &&
            srv_status_lte == NAS_SYS_SRV_STATUS_SRV_V01 &&
            (srv_domain_lte == SYS_SRV_DOMAIN_PS_ONLY_V01 || srv_domain_lte == SYS_SRV_DOMAIN_CS_PS_V01))
        {
          srv_domain = sys_info_ind.lte_sys_info.common_sys_info.srv_domain;
          srv_status = sys_info_ind.lte_srv_status_info.srv_status;
          if (sys_info_ind.lte_sys_info.common_sys_info.roam_status_valid == TRUE)
          {
            roam_status = sys_info_ind.lte_sys_info.common_sys_info.roam_status;
          }
          QCMAP_LOG_MSG_MED("lte Service Domain %d Status %d, roam status %d", srv_domain, srv_status, roam_status);
        }
        /* If the HDR System Info is valid, check the HDR Service Domain. */
        else if (sys_info_ind.hdr_sys_info_valid == TRUE &&
            sys_info_ind.hdr_sys_info.common_sys_info.srv_domain_valid == TRUE &&
            srv_status_hdr == NAS_SYS_SRV_STATUS_SRV_V01 &&
            (srv_domain_hdr == SYS_SRV_DOMAIN_PS_ONLY_V01 || srv_domain_hdr == SYS_SRV_DOMAIN_CS_PS_V01))
        {
          srv_domain = sys_info_ind.hdr_sys_info.common_sys_info.srv_domain;
          srv_status = sys_info_ind.hdr_srv_status_info.srv_status;
          if (sys_info_ind.hdr_sys_info.common_sys_info.roam_status_valid == TRUE)
          {
            roam_status = sys_info_ind.hdr_sys_info.common_sys_info.roam_status;
          }
          QCMAP_LOG_MSG_MED("HDR Service Domain %d Status %d, roam status %d", srv_domain, srv_status, roam_status);
        }
        /* If the CDMA System Info is valid, check the CDMA Service Domain. */
        else if (sys_info_ind.cdma_sys_info_valid == TRUE &&
            sys_info_ind.cdma_sys_info.common_sys_info.srv_domain_valid == TRUE &&
            srv_status_cdma == NAS_SYS_SRV_STATUS_SRV_V01 &&
            (srv_domain_cdma == SYS_SRV_DOMAIN_PS_ONLY_V01 || srv_domain_cdma == SYS_SRV_DOMAIN_CS_PS_V01))
        {
          srv_domain = sys_info_ind.cdma_sys_info.common_sys_info.srv_domain;
          srv_status = sys_info_ind.cdma_srv_status_info.srv_status;
          if (sys_info_ind.cdma_sys_info.common_sys_info.roam_status_valid == TRUE)
          {
            roam_status = sys_info_ind.cdma_sys_info.common_sys_info.roam_status;
          }
          QCMAP_LOG_MSG_MED("CDMA Service Domain %d Status %d, roam status %d", srv_domain, srv_status, roam_status);
        }
        /* If the WCDMA System Info is valid, check the WCDMA Service Domain. */
        else if (sys_info_ind.wcdma_sys_info_valid == TRUE &&
            sys_info_ind.wcdma_sys_info.common_sys_info.srv_domain_valid == TRUE &&
            srv_status_wcdma == NAS_SYS_SRV_STATUS_SRV_V01 &&
            (srv_domain_wcdma == SYS_SRV_DOMAIN_PS_ONLY_V01 || srv_domain_wcdma == SYS_SRV_DOMAIN_CS_PS_V01))
        {
          srv_domain = sys_info_ind.wcdma_sys_info.common_sys_info.srv_domain;
          srv_status = sys_info_ind.wcdma_srv_status_info.srv_status;
          if (sys_info_ind.wcdma_sys_info.common_sys_info.roam_status_valid == TRUE)
          {
            roam_status = sys_info_ind.wcdma_sys_info.common_sys_info.roam_status;
          }
          QCMAP_LOG_MSG_MED("WCDMA Service Domain %d Status %d, roam status %d", srv_domain, srv_status, roam_status);
        }
        /* If the GSM System Info is valid, check the GSM Service Domain. */
        else if (sys_info_ind.gsm_sys_info_valid == TRUE &&
            sys_info_ind.gsm_sys_info.common_sys_info.srv_domain_valid == TRUE &&
            srv_status_gsm == NAS_SYS_SRV_STATUS_SRV_V01 &&
            (srv_domain_gsm == SYS_SRV_DOMAIN_PS_ONLY_V01 || srv_domain_gsm == SYS_SRV_DOMAIN_CS_PS_V01))
        {
          srv_domain = sys_info_ind.gsm_sys_info.common_sys_info.srv_domain;
          srv_status = sys_info_ind.gsm_srv_status_info.srv_status;
          if (sys_info_ind.gsm_sys_info.common_sys_info.roam_status_valid == TRUE)
          {
            roam_status = sys_info_ind.gsm_sys_info.common_sys_info.roam_status;
          }
          QCMAP_LOG_MSG_MED("GSM Service Domain %d Status %d, roam status %d", srv_domain, srv_status, roam_status);
        }
        /* If the TDSCDMA System Info is valid, check the TDSCDMA Service Domain. */
        else if (sys_info_ind.tdscdma_sys_info_valid == TRUE &&
            sys_info_ind.tdscdma_sys_info.common_sys_info.srv_domain_valid == TRUE &&
            srv_status_tdscdma == NAS_SYS_SRV_STATUS_SRV_V01 &&
            (srv_domain_tdscdma == SYS_SRV_DOMAIN_PS_ONLY_V01 || srv_domain_tdscdma == SYS_SRV_DOMAIN_CS_PS_V01))
        {
          srv_domain = sys_info_ind.tdscdma_sys_info.common_sys_info.srv_domain;
          srv_status = sys_info_ind.tdscdma_srv_status_info.srv_status;
          if (sys_info_ind.tdscdma_sys_info.common_sys_info.roam_status_valid == TRUE)
          {
            roam_status = sys_info_ind.tdscdma_sys_info.common_sys_info.roam_status;
          }
          QCMAP_LOG_MSG_MED("TDSCDMA Service Domain %d Status %d, roam status %d", srv_domain, srv_status, roam_status);
        }

        if ((srv_status == NAS_SYS_SRV_STATUS_SRV_V01) &&
            (srv_domain == SYS_SRV_DOMAIN_PS_ONLY_V01 || srv_domain == SYS_SRV_DOMAIN_CS_PS_V01))
        {
          if(roam_status != NAS_ROAM_STATUS_ENUM_TYPE_MIN_ENUM_VAL_V01)
          {
            qcmap_cb.roaming_indicator = roam_status;
          } 
          else
          {
            qcmap_cb.roaming_indicator = 0;
          }
          if (qc_mgr->auto_connect && qcmap_cb.state == QCMAP_ENABLE &&
              !qcmap_cb.disable_in_process)
          {
            QCMAP_LOG_MSG_MED("Auto connect start", 0, 0, 0);
            qcmap_cb.backhaul_service = TRUE;
            qc_mgr->auto_timer_value_v4 = AUTO_CONNECT_TIMER;
            if(qc_mgr->auto_connect_timer_running_v4 )
            {
              stop_auto_timer(QCMAP_MSGR_IP_FAMILY_V4_V01);
            }
            qcmap_connect_backhaul(qcmap_cb.handle, QCMAP_MSGR_WWAN_CALL_TYPE_V4_V01, qcmap_errno, &qcmap_err_num);
          }
          if(qc_mgr->auto_connect && qcmap_cb.ipv6_state == QCMAP_V6_ENABLE &&
             !qcmap_cb.disable_in_process)
          {
            QCMAP_LOG_MSG_MED("Auto connect start", 0, 0, 0);
            qcmap_cb.backhaul_service = TRUE;
            qc_mgr->auto_timer_value_v6 = AUTO_CONNECT_TIMER;
            if(qc_mgr->auto_connect_timer_running_v6 )
            {
              stop_auto_timer(QCMAP_MSGR_IP_FAMILY_V6_V01);
            }
            qcmap_connect_backhaul(qcmap_cb.handle, QCMAP_MSGR_WWAN_CALL_TYPE_V6_V01,  qcmap_errno, &qcmap_err_num);
          }
        }
      }
      else
      {
        QCMAP_LOG_MSG_MED("could not decode message %d", qmi_error, 0, 0);
      }
      break;
    default:
      /* Ignore all other indications */
      break;
   }

   if(qcmap_nas_buffer != NULL)
   {
     if(qcmap_nas_buffer->ind_buf != NULL)
     {
       free(qcmap_nas_buffer->ind_buf);
       qcmap_nas_buffer->ind_buf = NULL;   
     }
     free(qcmap_nas_buffer);
     qcmap_nas_buffer = NULL;
   }

}

#ifdef SSR_SUPPORT
/*============================================================
  FUNCTION qmi_nas_error_cb
=============================================================

@brief
 QMI NAS Error callback handler

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
==========================================================*/
void qmi_nas_error_cb()
{
  qcmap_qmi_service_buffer_t qcmap_qmi_service_buffer;
  int numBytes=0, len;
  struct sockaddr_un qmi_service_qcmap;

  LOG_MSG_INFO1(" qmi_nas_error_cb", 0, 0, 0);

  qmi_service_qcmap.sun_family = AF_UNIX;
  strlcpy(qmi_service_qcmap.sun_path, QCMAP_QMI_SERVICE_UDS_FILE, sizeof(QCMAP_QMI_SERVICE_UDS_FILE));
  len = strlen(qmi_service_qcmap.sun_path) + sizeof(qmi_service_qcmap.sun_family);

  qcmap_qmi_service_buffer.qmi_service_id = QCMAP_QMI_NAS_NOT_IN_SERVICE;

  if ((numBytes = sendto(qmi_service_qcmap_sockfd, (void *)&qcmap_qmi_service_buffer,
                          sizeof(qcmap_qmi_service_buffer_t), 0,
                         (struct sockaddr *)&qmi_service_qcmap, len)) == -1)
  {
    LOG_MSG_ERROR("Send Failed from qmi_nas_error_cb context", 0, 0, 0);
    return;
  }
  return;
}
#endif


/*===========================================================================
  FUNCTION  qcmap_create_timers
===========================================================================*/
/*!
@brief
 Creates Timers for Autoconnect functionality

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
void qcmap_create_timers(void)
{
  timer_error_type timer_ret;

  // Create v4 Autoconnect timer
  qc_mgr->auto_timer_value_v4 = 1;

  /* Define a Non-defferable timer for Ipv4 backhaul connect */
  timer_ret = timer_def_osal(&qc_mgr->ipv4_auto_connect_timer,
                             &qc_mgr->ipv4_timer_group, TIMER_FUNC1_CB_TYPE,
                             qcmap_v4_timer_cb, NULL);

  if (timer_ret != TE_SUCCESS)
  {
    QCMAP_LOG_MSG_HIGH("Unable to create v4 autoconnect timer %d",timer_ret,0,0);
    return;
  }

  else
  {
    QCMAP_LOG_MSG_HIGH("Successfully created v4 autoconnect timer %d",timer_ret,0,0);
  }

  // Create v6 Autoconnect timer
  qc_mgr->auto_timer_value_v6 = 1;
  /* Define a Non-defferable timer for Ipv6 backhaul connect */
  timer_ret = timer_def_osal(&qc_mgr->ipv6_auto_connect_timer,
                             &qc_mgr->ipv6_timer_group, TIMER_FUNC1_CB_TYPE,
                             qcmap_v6_timer_cb, NULL);

  if (timer_ret != TE_SUCCESS)
  {
    QCMAP_LOG_MSG_HIGH("Unable to create v6 autoconnect timer %d",timer_ret,0,0);
    return;
  }

  else
  {
    QCMAP_LOG_MSG_HIGH("Successfully created v6 autoconnect timer %d",timer_ret,0,0);
  }
  return;
}


/*===========================================================================
  FUNCTION  qcmap_v4_timer_cb
===========================================================================*/
/*!
@brief
 Callback invoked when v4 timer expires

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
void qcmap_v4_timer_cb(void* unused)
{
  qcmap_cmd_t*             cmd_ptr = NULL;

  QCMAP_LOG_MSG_HIGH ("Entering: qcmap_v4_timer_cb ", 0, 0, 0 );

  if((cmd_ptr = qcmap_get_cmd_buffer()) == NULL)
  {
    QCMAP_LOG_MSG_ERROR("Cannot obtain command buffer. Assert", 0, 0, 0);
    ASSERT(0);
  }
  else
  {
    cmd_ptr->cmd_hdr.cmd_id              = QCMAP_V4_TIMEOUT_EVT;

    qcmap_enqueue_cmd(cmd_ptr);
  }

  qcmap_set_signal();
  return;
}

/*===========================================================================
  FUNCTION  qcmap_v6_timer_cb
===========================================================================*/
/*!
@brief
 Callback invoked when v6 timer expires

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
void qcmap_v6_timer_cb(void* unused)
{
  qcmap_cmd_t*             cmd_ptr = NULL;

  QCMAP_LOG_MSG_HIGH ("Entering: qcmap_v6_timer_cb ", 0, 0, 0 );

  if((cmd_ptr = qcmap_get_cmd_buffer()) == NULL)
  {
    QCMAP_LOG_MSG_ERROR("Cannot obtain command buffer. Assert", 0, 0, 0);
    ASSERT(0);
  }
  else
  {
    cmd_ptr->cmd_hdr.cmd_id              = QCMAP_V6_TIMEOUT_EVT;

    qcmap_enqueue_cmd(cmd_ptr);
  }

  qcmap_set_signal();
  return;
}

/*===========================================================================
  FUNCTION  QCMAP_AUTO_CONNECT_TIMEOUT
===========================================================================*/
/*!
@brief
  Function to handle the auto connect timeout
@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
void qcmap_auto_connect_timeout(qcmap_cmd_id_e  cmd_id)
{
  int qcmap_errno;
  int ret_val;
  qcmap_error_type qcmap_err_num;


  if (cmd_id == QCMAP_V4_TIMEOUT_EVT && qc_mgr->auto_connect_timer_running_v4 == true)
  {
    ret_val = qcmap_connect_backhaul(qc_mgr->qcmap_cm_handle, QCMAP_MSGR_WWAN_CALL_TYPE_V4_V01, &qcmap_errno, &qcmap_err_num);

    if (ret_val == QCMAP_SUCCESS)
    {
       QCMAP_LOG_MSG_HIGH("Call to V4 Connect backhaul Success ", 0, 0, 0);
    }

    if (ret_val == QCMAP_ERROR && qcmap_errno == QCMAP_EWOULDBLOCK)
    {
      QCMAP_LOG_MSG_HIGH("QCMAP WAN Connecting Inprogress ", 0, 0, 0);
    }
    else
    {
      QCMAP_LOG_MSG_ERROR( "QCMAP WAN Connecting Fail, ret_val %d qcmap_errno %d", ret_val, qcmap_errno, 0 );
    }
  }
  else if (cmd_id == QCMAP_V6_TIMEOUT_EVT && qc_mgr->auto_connect_timer_running_v6 == true)
  {
      ret_val = qcmap_connect_backhaul(qc_mgr->qcmap_cm_handle, QCMAP_MSGR_WWAN_CALL_TYPE_V6_V01, &qcmap_errno, &qcmap_err_num);

    if (ret_val == QCMAP_SUCCESS)
    {
       QCMAP_LOG_MSG_HIGH("Call to V6 Connect backhaul Success ", 0, 0, 0);
    }

    if (ret_val == QCMAP_ERROR && qcmap_errno == QCMAP_EWOULDBLOCK)
    {
      QCMAP_LOG_MSG_HIGH("QCMAP V6 WAN Connecting Inprogress ", 0, 0, 0);
    }
    else
    {
      QCMAP_LOG_MSG_ERROR( "QCMAP V6 WAN Connecting Fail, ret_val %d qcmap_errno %d", ret_val, qcmap_errno, 0 );
    }
  }
  return;
}


/*===========================================================================
  FUNCTION  qcmap_qti_client_cb
===========================================================================*/
/*!
@brief
 Callback function to process different response events received from qcmap server

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/

void qcmap_qti_client_cb(uint8 evt, void* data, uint8 idx)
{
  qcmap_msgr_mobile_ap_enable_resp_msg_v01 *enable_resp;
  qcmap_msgr_mobile_ap_status_ind_msg_v01  *enable_ind;
  qcmap_msgr_mobile_ap_disable_resp_msg_v01 *disable_resp;
  qcmap_msgr_bring_up_wwan_resp_msg_v01 *wwan_bringup;
  qcmap_msgr_tear_down_wwan_resp_msg_v01 *wwan_teardown;
  qcmap_msgr_bring_up_wwan_ind_msg_v01  *wwan_bringup_ind;
  qcmap_msgr_tear_down_wwan_ind_msg_v01  *wwan_teardown_ind;
  qcmap_msgr_set_roaming_pref_resp_msg_v01 *set_roaming;
  qcmap_msgr_get_roaming_pref_resp_msg_v01 *get_roaming;
  qcmap_msgr_get_auto_connect_resp_msg_v01 *get_autoconnect;
  qcmap_msgr_set_auto_connect_resp_msg_v01 *set_autoconnect;

  switch(evt)
  {
   case QCMAP_MOBILEAP_ENABLE_RESP_EVT:
        enable_resp = (qcmap_msgr_mobile_ap_enable_resp_msg_v01 *)data;
        mobile_ap_qti_type.resp.result = enable_resp->resp.result;
        mobile_ap_qti_type.resp.error  = enable_resp->resp.error;
        mobile_ap_qti_type.mobile_ap_handle_valid = TRUE;
        mobile_ap_qti_type.mobile_ap_handle = enable_resp->mobile_ap_handle;

        break;

   case QCMAP_MOBILEAP_ENABLE_IND_EVT:
         enable_ind = (qcmap_msgr_mobile_ap_status_ind_msg_v01 *)data;
         mobile_ap_qti_type.mobile_ap_handle = enable_ind->mobile_ap_handle;
         mobile_ap_qti_type.mobile_ap_status = enable_ind->mobile_ap_status;

        break;

   case QCMAP_MOBILEAP_DISABLE_RESP_EVT:
        disable_resp = (qcmap_msgr_mobile_ap_disable_resp_msg_v01 *)data;
        mobile_ap_qti_type.resp.result = disable_resp->resp.result;
        mobile_ap_qti_type.resp.error  = disable_resp->resp.error;

       break;

   case QCMAP_MOBILEAP_BRING_UP_WWAN_RESP_EVT:
        wwan_bringup = (qcmap_msgr_bring_up_wwan_resp_msg_v01 *)data;
        mobile_ap_qti_type.resp.result = wwan_bringup->resp.result;
        mobile_ap_qti_type.resp.error  = wwan_bringup->resp.error;
        mobile_ap_qti_type.conn_status_valid = TRUE;
        mobile_ap_qti_type.conn_status = wwan_bringup->conn_status;

       break;

   case QCMAP_MOBILEAP_TEAR_DOWN_WWAN_RESP_EVT:
        wwan_teardown = (qcmap_msgr_tear_down_wwan_resp_msg_v01 *)data;
        mobile_ap_qti_type.resp.result = wwan_teardown->resp.result;
        mobile_ap_qti_type.resp.error  = wwan_teardown->resp.error;
        mobile_ap_qti_type.conn_status_valid = TRUE;
        mobile_ap_qti_type.conn_status = wwan_teardown->conn_status;

       break;

   case QCMAP_MOBILEAP_BRING_UP_WWAN_IND_EVT:
        wwan_bringup_ind = (qcmap_msgr_bring_up_wwan_ind_msg_v01 *)data;
        mobile_ap_qti_type.mobile_ap_handle = wwan_bringup_ind->mobile_ap_handle;
        mobile_ap_qti_type.conn_status = wwan_bringup_ind->conn_status;
        mobile_ap_qti_type.wwan_call_end_reason_valid = TRUE;
        mobile_ap_qti_type.wwan_call_end_reason = wwan_bringup_ind->wwan_call_end_reason;

       break;

   case QCMAP_MOBILEAP_TEAR_DOWN_WWAN_IND_EVT:
        wwan_teardown_ind = (qcmap_msgr_tear_down_wwan_ind_msg_v01 *)data;
        mobile_ap_qti_type.mobile_ap_handle = wwan_teardown_ind->mobile_ap_handle;
        mobile_ap_qti_type.conn_status = wwan_teardown_ind->conn_status;
        mobile_ap_qti_type.wwan_call_end_reason_valid = TRUE;
        mobile_ap_qti_type.wwan_call_end_reason = wwan_teardown_ind->wwan_call_end_reason;

       break;

   case QCMAP_MOBILEAP_SET_AUTOCONNECT_RESP_EVT:
        set_autoconnect = (qcmap_msgr_set_auto_connect_resp_msg_v01 *)data;
        mobile_ap_qti_type.resp.result = set_autoconnect->resp.result;
        mobile_ap_qti_type.resp.error  = set_autoconnect->resp.error;

      break;

   case QCMAP_MOBILEAP_GET_AUTOCONNECT_RESP_EVT:
        get_autoconnect = (qcmap_msgr_get_auto_connect_resp_msg_v01 *)data;
        mobile_ap_qti_type.resp.result = get_autoconnect->resp.result;
        mobile_ap_qti_type.resp.error  = get_autoconnect->resp.error;

       break;

   case QCMAP_MOBILEAP_SET_ROAMING_RESP_EVT:
        set_roaming= (qcmap_msgr_set_roaming_pref_resp_msg_v01 *)data;
        mobile_ap_qti_type.resp.result = set_roaming->resp.result;
        mobile_ap_qti_type.resp.error  = set_roaming->resp.error;

      break;

   case QCMAP_MOBILEAP_GET_ROAMING_RESP_EVT:
        get_roaming = (qcmap_msgr_get_roaming_pref_resp_msg_v01 *)data;
        mobile_ap_qti_type.resp.result = get_roaming->resp.result;
        mobile_ap_qti_type.resp.error  = get_roaming->resp.error;

       break;

  }
}

/*===========================================================================
  FUNCTION  qcmap_bring_up_std_ecm
===========================================================================*/
/*!
@brief
 Function for registering qti client and enabling mobile ap functionality.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/

void qcmap_bring_up_std_ecm( void)
{
  uint8 connection_handle = 255; // set to high value.


  if(qti_client == FALSE)
  {    
    qcmap_client_reg_cb((qcmap_client_resp_ind_cb )qcmap_qti_client_cb, &connection_handle);
  
    if(connection_handle <= QCMAP_MAX_CLIENT_HANDLES)
    {
      mobile_ap_qti_type.conn_handle = connection_handle;
      qti_client = TRUE;
      QCMAP_LOG_MSG_MED("CLient registered with connection handle %d ",connection_handle, 0, 0);
    }
    else
    {
     QCMAP_LOG_MSG_ERROR("CLient not registered Try again ", 0, 0, 0);
     return;
    }
  }

  else
  {
    QCMAP_LOG_MSG_HIGH("CLient already registered ", 0, 0, 0);
  }
  QCMAP_LOG_MSG_HIGH("qti enabling mobileap ", 0, 0, 0);
  
  qcmap_mobile_ap_enable(mobile_ap_qti_type.data, mobile_ap_qti_type.conn_handle);
  
  ip6_set_req_params();

  return;

}

/*===========================================================================
  FUNCTION  qcmap_bring_down_std_ecm
===========================================================================*/
/*!
@brief
 Function for disabling mobile ap functionality for qti client.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/

void qcmap_bring_down_std_ecm(void )
{
  qcmap_msgr_mobile_ap_disable_req_msg_v01 *data;
  

  data = (qcmap_msgr_mobile_ap_disable_req_msg_v01 *)malloc(sizeof(qcmap_msgr_mobile_ap_disable_req_msg_v01));
  if (data == NULL)
  {
        QCMAP_LOG_MSG_ERROR("data == NULL", 0, 0, 0);
        return ;
  }
  memset(data, 0, sizeof(qcmap_msgr_mobile_ap_disable_req_msg_v01));
  data->mobile_ap_handle = mobile_ap_qti_type.mobile_ap_handle;

  QCMAP_LOG_MSG_HIGH("qti disabling mobileap ", 0, 0, 0);

  qcmap_mobile_ap_disable((void *) data, mobile_ap_qti_type.conn_handle);

  ip6_clear_req_params();
  ip6_clear_addresses();
  return;
}

/*===========================================================================
  FUNCTION  qcmap_enable_nat_on_apps
===========================================================================*/
/*!
@brief
 Function for enabling nat module functionality.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/

void qcmap_enable_nat_on_apps()
{
  //int private_inf, public_inf,i;
  /*for(i=0;i<MAXNETS;i++)
  {
    if(strncmp(nets[i]->name,"rmnet_data0",11)==0)
		public_inf=i;
    if(strncmp(nets[i]->name,"ecm",3)==0)
		private_inf=i;
  }
  nat_init_params(1, private_inf, public_inf);*/
  
  QCMAP_LOG_MSG_MED("qcmap_enable_nat_on_apps: enter", 0, 0, 0);
  nat_init_params(1, 5, 0);

  if(qcmap_file_t.nat_tcp_tmo)
  {
      set_nat_tcp_timeout((unsigned short)qcmap_file_t.nat_tcp_tmo);
  }
  if(qcmap_file_t.nat_udp_tmo)
  {
      set_nat_udp_timeout((unsigned short)qcmap_file_t.nat_udp_tmo);
  }
  	
}

/*===========================================================================
  FUNCTION  qcmap_disable_nat_on_apps
===========================================================================*/
/*!
@brief
 Function for disabling nat module functionality when call gets disconnected.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/

void qcmap_disable_nat_on_apps()
{
  QCMAP_LOG_MSG_MED("qcmap_disable_nat_on_apps: enter", 0, 0, 0);
  nat_disable();
}

/*===========================================================================
  FUNCTION  qcmap_set_dhcp_address_range
===========================================================================*/
/*!
@brief
 Function to set address pool to be used by DHCP clients.

@return
  QCMAP_DHCP_ASSIGN_SUCCESS -Successful in setting up DHCP address pool
  QCMAP_DHCP_ASSIGN_FAIL - Failed to set DHCP address pool

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/

int32_t qcmap_set_dhcp_address_range()
{
  struct ip46addr ipaddr = qcmap_ip_t.gateway_addr;
  struct ip46addr start_ip = qcmap_ip_t.dhcp_start_addr;
  struct ip46addr end_ip = qcmap_ip_t.dhcp_end_addr;
  struct ip46addr snmask = qcmap_ip_t.subnet_addr;
  int leasetime=qcmap_file_t.dhcp_lease;

  QCMAP_LOG_MSG_MED("qcmap_set_dhcp_address_range: enter", 0, 0, 0);
  
  if (ipaddr.type == AF_INET && start_ip.type == AF_INET && end_ip.type == AF_INET && snmask.type == AF_INET)
  {
    if (ipaddr.a.addr4 == NULL || (!ns_is_valid_ip_address(ipaddr.type, &(ipaddr.a.addr4))))
    {
      return QCMAP_DHCP_ASSIGN_FAIL ;
    }
    if (start_ip.a.addr4 == NULL || (!ns_is_valid_ip_address(start_ip.type, &(start_ip.a.addr4))))
    {
      return QCMAP_DHCP_ASSIGN_FAIL;
    }
    if (end_ip.a.addr4 == NULL || (!ns_is_valid_ip_address(end_ip.type, &(end_ip.a.addr4))))
    {
      return QCMAP_DHCP_ASSIGN_FAIL;
    }
    if (snmask.a.addr4 == NULL || (!ns_is_valid_ip_address(snmask.type, &(snmask.a.addr4))))
    {
      return QCMAP_DHCP_ASSIGN_FAIL;
    }
    if(_dhs_init_from_config_file( ipaddr.a.addr4, snmask.a.addr4, start_ip.a.addr4,  end_ip.a.addr4,  leasetime) == 0)
      return QCMAP_DHCP_ASSIGN_SUCCESS;
  }
  
  return QCMAP_DHCP_ASSIGN_FAIL;

}

/*===========================================================================
  FUNCTION  qcmap_start_dnsc
===========================================================================*/
/*!
@brief
 Function to start the DNSc
@return
  0 -Successful in starting dnsc
  -1 - Failed starting dnsc

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/

int qcmap_start_dnsc(void)
{
  return qapi_Net_DNSc_Command(QAPI_NET_DNS_START_E);
}


/*=========================================================================
  FUNCTION  qcmap_add_v4_dns_svr
===========================================================================*/
/*!
@brief
 Function to add v4 DNS servers
 @param
 qapi_DSS_Addr_Info_t 
@return
  0 -Successful adding v4 DNS servers
  -1 - Failed adding v4 DNS servers

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*====================================================*/

int qcmap_add_v4_dns_svr(qapi_DSS_Addr_Info_t *v4addr)
{
  uint32_t p_dns,s_dns;
  uint32_t id;
  int ret = QCMAP_FAILURE;
  int ret1 = QCMAP_FAILURE;
  int ret2 = QCMAP_FAILURE;
  char svr_addr[16];
  id = QAPI_NET_DNS_ANY_SERVER_ID;
  if(v4addr->dnsp_addr_s.valid_addr)
  {
    p_dns = v4addr->dnsp_addr_s.addr.v4;
    inet_ntop(AF_INET,&p_dns,svr_addr,sizeof(svr_addr));
    ret1 = qapi_Net_DNSc_Add_Server(svr_addr, id);
    dhs_add_dns_server(p_dns);
  }
  if(v4addr->dnss_addr_s.valid_addr)
  {
    s_dns = v4addr->dnss_addr_s.addr.v4;
    inet_ntop(AF_INET,&s_dns,svr_addr,sizeof(svr_addr));
    ret2 = qapi_Net_DNSc_Add_Server(svr_addr, id);
  }
  ret = ((ret1 == 0) || (ret2 == 0))?0:-1;
  if(ret == 0)
    qcmap_set_v4_dns_server_added(true);
  return ret;
}

/*===========================================================================
  FUNCTION  qcmap_add_v6_dns_svr
===========================================================================*/
/*!
@brief
 Function to add v6 DNS servers
 @param
 qapi_DSS_Addr_Info_t 
@return
  0 -Successful adding v6 DNS servers
  -1 - Failed adding v6 DNS servers

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=====================*/

int qcmap_add_v6_dns_svr(qapi_DSS_Addr_Info_t *v6addr)
{
  uint8_t   p_dns6[16], s_dns6[16];
  uint32_t id;
  int ret = QCMAP_FAILURE;
  int ret1 = QCMAP_FAILURE;
  int ret2 = QCMAP_FAILURE;
  char svr_addr[48];
  id = QAPI_NET_DNS_ANY_SERVER_ID;
  if(v6addr->dnsp_addr_s.valid_addr)
  {
      ds_memcpy(p_dns6, v6addr->dnsp_addr_s.addr.v6_addr8,16);
      inet_ntop(AF_INET6,p_dns6,svr_addr,sizeof(svr_addr));
      ret1 =  qapi_Net_DNSc_Add_Server(svr_addr, id);
  }
  if(v6addr->dnss_addr_s.valid_addr)
  {
       ds_memcpy(s_dns6, v6addr->dnss_addr_s.addr.v6_addr8,16);
       inet_ntop(AF_INET6,s_dns6,svr_addr,sizeof(svr_addr));
       ret2 = qapi_Net_DNSc_Add_Server(svr_addr, id);
  }
  ret = ((ret1 == 0) || (ret2 == 0))?0:-1;
  if(ret == 0)
    qcmap_set_v6_dns_server_added(true);
  return ret;
}

void set_dhcp_command(uint8_t cmd)
{
    if(cmd)
	dhs_enable();
    else
	dhs_disable();
}



