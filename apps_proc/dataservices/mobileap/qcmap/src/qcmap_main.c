/*==============================================================================

                             QCMAP_MAIN.C

GENERAL DESCRIPTION
  This file deals with the task Initialization to support various qcmap
  mechanisms
 

INITIALIZATION AND SEQUENCING REQUIREMENTS
  The task init and task start routines do the work necessary to ensure that
  all data structures are in the desired state to continue with normal
  operation.

Copyright (c) 2017 - 2023 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
==============================================================================*/

/*==============================================================================

                            EDIT HISTORY FOR FILE
when        who    what, where, why
--------    ---    -------------------------------------------------------------
11/23/2016   pv     created file
==============================================================================*/
#include "qcmap.h"
#include "qcmap_api.h"
#include "qapi_dss.h"
#include "data_utils_common.h"
#include "qapi_ns_utils.h"
#include "qapi_dnsc.h"

#define QCMAP_CONFIG_FILE        "/datatx/qcmap_config"
static qcmap_config_info_t          qcmap_param;
extern qcmap_connection_manager *qc_mgr;
extern client_data_s *client_data_t;
extern qcmap_backhaul_wwan *qc_bck_wan;

qcmap_file qcmap_file_t;
qcmap_ip   qcmap_ip_t;
/*=============================================================================

FUNCTION QCMAP_SET_SIGNAL

DESCRIPTION
  Set the qcmap signal using lock protection

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None

==============================================================================*/
void qcmap_set_signal(void)
{
  QCMAP_LOG_MSG_HIGH ("qcmap_set_signal ",0, 0, 0 );

  qurt_mutex_lock(&qcmap_param.qcmap_mutex);

  QCMAP_TASK_SET_SIG(qcmap_param.qcmap_signal,QCMAP_SIGNAL_MASK);

  qurt_mutex_unlock(&qcmap_param.qcmap_mutex);

  return;
}
/*=============================================================================

FUNCTION QCMAP_MUTEX_INIT

DESCRIPTION
  qcmap mutex Initialization

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None

==============================================================================*/
static void qcmap_mutex_init(void)
{
  qurt_mutex_init(&qcmap_param.qcmap_mutex);
  return;
}


/*=============================================================================

FUNCTION QCMAP_SIGNAL_INIT

DESCRIPTION
  QCMAP Signal Initialization
  
DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None

==============================================================================*/
static void qcmap_signal_init(void)
{
  qurt_signal_init(&qcmap_param.qcmap_signal);
  return;
}


/*=============================================================================

FUNCTION QCMAP_SIGNAL_WAIT

DESCRIPTION
  This API will wait on the QCMAP signal.
  The QCMAP signal will be set during various operations.

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None

==============================================================================*/
static void qcmap_signal_wait(void)
{
  uint32_t received_sigs = 0;

  QCMAP_LOG_MSG_HIGH ("qcmap_signal_wait ",0, 0, 0 );

  while (1)
  {
/*============================================================================
  Wait for the QCMAP Signal to be set
=============================================================================*/
    received_sigs = qurt_signal_wait(&qcmap_param.qcmap_signal, QCMAP_SIGNAL_MASK,
                                     QURT_SIGNAL_ATTR_WAIT_ANY);

    if (QCMAP_TASK_IS_SIG_SET (received_sigs, QCMAP_SIGNAL_MASK))
    {

      /*============================================================================
      Clear the QCMAP signal **Need To add mutex in the correct place**
      =============================================================================*/

      qurt_mutex_lock(&qcmap_param.qcmap_mutex);

      QCMAP_TASK_CLEAR_SIG(qcmap_param.qcmap_signal, QCMAP_SIGNAL_MASK);

      qurt_mutex_unlock(&qcmap_param.qcmap_mutex);

      // QCMAP Signal is set Dequeue the QCMAP Command queue
      qcmap_process_commands();
    }

  }
}

/*================================================================

FUNCTION QCMAP_READ_CONFIG_FILE

DESCRIPTION
  Read config file from EFS
  
DEPENDENCIES
  None.

RETURN VALUE
  Sucess/Failure

SIDE EFFECTS
  None

==================================================================*/

int qcmap_read_config_file(void )
{

  void *data_config_list = NULL;
  char ip_str_g[16], ip_str_s[16], ip_str_ds[16], ip_str_de[16];
  data_config_list = data_util_config_load(QCMAP_CONFIG_FILE);
  if (data_config_list != NULL)
  {
    data_util_config_get_value(data_config_list, "Tech", STRING, qcmap_file_t.call_tech);
    data_util_config_get_value(data_config_list, "Profile", INTEGER, &qcmap_file_t.profile);
    data_util_config_get_value(data_config_list, "Gateway_IP", STRING, qcmap_file_t.gateway_ip);
    data_util_config_get_value(data_config_list, "Subnet", STRING, qcmap_file_t.subnet_mask);
    data_util_config_get_value(data_config_list, "DHCP_Start", STRING, qcmap_file_t.dhcp_start);
    data_util_config_get_value(data_config_list, "DHCP_End", STRING, qcmap_file_t.dhcp_end);
    data_util_config_get_value(data_config_list, "DHCP_Lease", INTEGER, &qcmap_file_t.dhcp_lease);
    data_util_config_get_value(data_config_list, "Autoconnect", INTEGER, &qcmap_file_t.autoconnect);
    data_util_config_get_value(data_config_list, "Roaming", INTEGER, &qcmap_file_t.roaming);
    if(data_util_config_get_value(data_config_list, "DHCP_Enable", INTEGER, &qcmap_file_t.dhcp_enable) == FALSE)
    {
        qcmap_file_t.dhcp_enable = 1;
    }
    if(data_util_config_get_value(data_config_list, "STDECM_PublicIP", INTEGER, &qcmap_file_t.stdecm_publicIP) == FALSE)
    {
        qcmap_file_t.stdecm_publicIP = 0;
    }
    if(data_util_config_get_value(data_config_list, "APPS_Port_Start", INTEGER, &qcmap_file_t.apps_port_start) == FALSE)
    {
        qcmap_file_t.stdecm_publicIP = 0;
        qcmap_file_t.apps_port_start = 0;
    }
    if(data_util_config_get_value(data_config_list, "APPS_Port_Range", INTEGER, &qcmap_file_t.apps_port_range) == FALSE)
    {
        qcmap_file_t.stdecm_publicIP = 0;
        qcmap_file_t.apps_port_range = 0;
    }
    if(data_util_config_get_value(data_config_list, "NAT_TCP_Timeout", INTEGER, &qcmap_file_t.nat_tcp_tmo) == FALSE)
    {
        qcmap_file_t.nat_tcp_tmo= 0;
    }
    if(data_util_config_get_value(data_config_list, "NAT_UDP_Timeout", INTEGER, &qcmap_file_t.nat_udp_tmo) == FALSE)
    {
        qcmap_file_t.nat_udp_tmo = 0;
    }

    if(qcmap_file_t.apps_port_start <= QCMAP_IPPORT_RESERVED || qcmap_file_t.apps_port_range < QCMAP_MAX_NWSTACK_SOCKETS)
    {
       qcmap_file_t.stdecm_publicIP = 0;
    }
	
    data_util_config_unload(data_config_list);
  }
  else
  {
    QCMAP_LOG_MSG_HIGH(" Unable to read config file ", 0, 0, 0);
    return QCMAP_FAILURE;
  }

    if (inet_pton(AF_INET, qcmap_file_t.gateway_ip, &qcmap_ip_t.gateway_addr.a) != 0)   /* not in xxx.xxx.xxx.xxx format */
    {
        if (strlen(qcmap_file_t.gateway_ip) > 32)
        {
            QCMAP_LOG_MSG_HIGH(" Gateway ip should not be more than 32 bytes ", 0, 0, 0);
            return QCMAP_FAILURE;
        }
        else
        {
          inet_ntop(AF_INET, &qcmap_ip_t.gateway_addr.a, ip_str_g, sizeof(ip_str_g));
        }
    }
    qcmap_ip_t.gateway_addr.type = AF_INET;


    if (inet_pton(AF_INET, qcmap_file_t.subnet_mask, &qcmap_ip_t.subnet_addr.a) != 0)   /* not in xxx.xxx.xxx.xxx format */
    {
        if (strlen(qcmap_file_t.subnet_mask) > 32)
        {
            QCMAP_LOG_MSG_HIGH(" Subnet mask should not be more than 32 bytes ", 0, 0, 0);
            return QCMAP_FAILURE;
        }
        else
        {
          inet_ntop(AF_INET, &qcmap_ip_t.subnet_addr.a, ip_str_s, sizeof(ip_str_s));
        }
    }
    qcmap_ip_t.subnet_addr.type = AF_INET;


    if (inet_pton(AF_INET, qcmap_file_t.dhcp_start, &qcmap_ip_t.dhcp_start_addr.a) != 0)   /* not in xxx.xxx.xxx.xxx format */
    {
        if (strlen(qcmap_file_t.dhcp_start) > 32)
        {
            QCMAP_LOG_MSG_HIGH(" DHCP starting ip should not be more than 32 bytes ", 0, 0, 0);
            return QCMAP_FAILURE;
        }
        else
        {
          inet_ntop(AF_INET, &qcmap_ip_t.dhcp_start_addr.a, ip_str_ds, sizeof(ip_str_ds));
        }
    }
    qcmap_ip_t.dhcp_start_addr.type = AF_INET;


    if (inet_pton(AF_INET, qcmap_file_t.dhcp_end, &qcmap_ip_t.dhcp_end_addr.a) != 0)   /* not in xxx.xxx.xxx.xxx format */
    {
        if (strlen(qcmap_file_t.dhcp_end) > 32)
        {
            QCMAP_LOG_MSG_HIGH(" DHCP ending ip  should not be more than 32 bytes ", 0, 0, 0);
            return QCMAP_FAILURE;
        }
        else
        {
          inet_ntop(AF_INET, &qcmap_ip_t.dhcp_end_addr.a, ip_str_de, sizeof(ip_str_de));
        }
    }
    qcmap_ip_t.dhcp_end_addr.type = AF_INET;

    QCMAP_LOG_MSG_HIGH("qcmap_read_config_file is successful ", 0, 0, 0);
    return QCMAP_SUCCESS;
}

/*================================================================

FUNCTION QCMAP_TASK_INIT

DESCRIPTION
  QCMAP Task initialization
  
DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None

==================================================================*/
void qcmap_task_init(void)
{

  QCMAP_LOG_MSG_HIGH ("qcmap_task_init ",0, 0, 0 );
/*============================================================================
  Reseting the Global QCMAP Structure
=============================================================================*/

  memset (&qcmap_param, 0, sizeof (qcmap_config_info_t));

/*----------------------------------------------------------------------------
  Initialize the command queues and set the command queue pointers in the
   qcmap_rmnet_config_info struct to the appropriate values.
----------------------------------------------------------------------------*/
  qcmap_cmdq_init(&qcmap_param);

  qapi_DSS_Init((int) QAPI_DSS_MODE_GENERAL);
}


/*=============================================================================

FUNCTION QCMAP_TASK_ENTRY

DESCRIPTION
  This is the entry point for QCMAP task
  Handshake with RCInit
  Reset the Global QCMAP structure
  Initialize DPM Service
  Open the Peripheral SIO port
  Initalize the QCMAP Qurt signal

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None

==============================================================================*/
void qcmap_task_entry(void *thread_ctxt)
{

  QCMAP_LOG_MSG_HIGH ("qcmap_task_entry ",0, 0, 0 );
/*============================================================================
  Handshake with RCInit
    Indicate to the Task start framework that we have completed the task start
  procedures.
 
  If any synchronization is needed with other tasks during the task start time
  we should send an event indicating what we wish to communicate. 
=============================================================================*/

  rcinit_handshake_startup();

/*=============================================================================
Initialize Mutex Object
==============================================================================*/

  qcmap_mutex_init();

/*=============================================================================
Initialize Signal for QCMAP
==============================================================================*/

  qcmap_signal_init ();


  qc_mgr = (qcmap_connection_manager *)malloc(sizeof(qcmap_connection_manager));
   if (qc_mgr == NULL)
  {
     QCMAP_LOG_MSG_ERROR("qc_mgr == NULL", 0, 0, 0);
      ASSERT(0);
  }
  memset(qc_mgr, 0, sizeof(qcmap_connection_manager));
  qc_bck_wan = (qcmap_backhaul_wwan *)malloc(sizeof(qcmap_backhaul_wwan ));
   if (qc_bck_wan == NULL)
  {
     QCMAP_LOG_MSG_ERROR("qc_bck_wan == NULL", 0, 0, 0);
      ASSERT(0);
  }
  memset(qc_bck_wan, 0, sizeof(qcmap_backhaul_wwan ));
  client_data_t = (client_data_s *)malloc(sizeof(client_data_s));
   if (client_data_t == NULL)
  {
     QCMAP_LOG_MSG_ERROR("client_data_t == NULL", 0, 0, 0);
      ASSERT(0);
  }
  memset(client_data_t, 0, sizeof(client_data_s));

/*=============================================================================
Create timers for Autoconnect functionality
==============================================================================*/

  qcmap_create_timers();

/*=============================================================================
Wait on the QCMAP Signal
==============================================================================*/
  qcmap_signal_wait();

}

