/*===========================================================================

  device_actions.c

  DESCRIPTION
  Thermal mitigation action implementations, excluding modem actions which
  are implemented in modem mitigation files.

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  If present, action _init functions (e.g. vdd_min_init) should be called
  before the action functions. Each action function should keep its own
  state to avoid undesired repeating of mitigation actions at the same
  mitigation level.

  TODO
  It is possible to share different device functions with a little bit
  restructuring.

  Copyright (c) 2013-18 QUALCOMM Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

===========================================================================*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <sys/types.h>

#include "npa_resource.h"
#include "therm_log.h"
#include "thermal.h"
#include "devices_actions.h"
#include "devices-qmi.h"
#include "pmapp_npa.h"
#include "kvp.h"
#include <stdbool.h>
#include "rpm.h"
#include "rpmclient.h"
#include "VCSDefs.h"
#include "voltage_level.h"

/* Variables related to vdd_restrict device action */
static qurt_mutex_t 	 vdd_restrict_mtx;    		/* Mutex for concurrent requests */
static npa_client_handle vdd_restrict_cx_clnt; 	/* NPA client for vdd restrict cx request */
static npa_client_handle vdd_restrict_mx_clnt; 	/* NPA client for vdd restrict mx request */
static uint8_t 			 vdd_restrict_init_npa;	/* Indicates if NPA request can be made */
static int 				 vdd_rstr_enabled;    	/* vdd restrict state */

/* Variables related to mx_min_request device action */
static qurt_mutex_t 	 mx_min_req_mtx;      	/* Mutex for concurrent requests */
static npa_client_handle mx_min_req_clnt;   	/* NPA client for mx min request */
static uint8_t 			 mx_min_req_init_npa;   /* Indicates if NPA request can be made */
static int 				 mx_min_req_enabled;    /* mx min request state */
static uint8_t 			 mx_min_request_used;   /* Indicates if mx min request is used */

/* Variables related to cx_min_request device action */
static qurt_mutex_t 	 cx_min_req_mtx;      	/* Mutex for concurrent requests */
static npa_client_handle cx_min_req_clnt;   	/* NPA client for cx min request */
static uint8_t 			 cx_min_req_init_npa;   /* Indicates if NPA request can be made */
static int 				 cx_min_req_enabled;    /* cx min request state */
static uint8_t 			 cx_min_request_used;   /* Indicates if cx min request is used */

#ifdef CPR_COLD_REQUEST
/* Variables related to cpr_cold_request device action */
static qurt_mutex_t 	cpr_cold_req_mtx;      /* Mutex for concurrent requests */
static int 				cpr_cold_req_state;    /* cpr cold request state */
#endif

static qurt_mutex_t 	 cpu_req_mtx;			/* Mutex for concurrent requests */
static npa_client_handle cpu_req_clnt;			/* NPA client for cpu min request */
static uint8_t 			 cpu_req_init_npa;    	/* Indicates if NPA request can be made */
static int 				 cpu_req_enabled;   	/* cpu min request state */
static uint8_t 			 cpu_request_used;   	/* Indicates if cpu min request is used */


static const char* resources[2] = {VCS_NPA_RESOURCE_VDD_MX_ACTIVE_FLOOR, VCS_NPA_RESOURCE_VDD_CX_ACTIVE_FLOOR};

enum {
  CPR_COLD_NORMAL = 0,
  CPR_COLD_COOL,
  CPR_COLD_COLD,
  CPR_COLD_CRITICAL_COLD,
};

enum {
  CPR_RPM_NO_INFO = 0,
  CPR_RPM_CRITICAL_COLD,
  CPR_RPM_COLD,
  CPR_RPM_COOL,
  CPR_RPM_NORMAL,
  CPR_RPM_WARM,
  CPR_RPM_HOT,
  CPR_RPM_CRITICAL_HOT,
};

/*===========================================================================
FUNCTION vdd_rstr_npa_avail_cb

Callback function which will be invoked when associated npa resource (cx rail)
becomes available.

ARGUMENTS
  context: Unused - to comply with prototype
  event_type: Unused - to comply with prototype
  data: Unused - to comply with prototype
  data_size: Unused - to comply with prototype

RETURN VALUE
  None
============================================================================*/
static void vdd_rstr_npa_avail_cb(void         *context,
                                  unsigned int  event_type,
                                  void         *data,
                                  unsigned int  data_size)
{
  int request;
  therm_log_printf(THERM_LOG_LEVEL_DEBUG, 1, "%s", __func__);

  qurt_mutex_lock(&vdd_restrict_mtx);
  vdd_restrict_cx_clnt = npa_create_sync_client(VCS_NPA_RESOURCE_VDD_CX_ACTIVE_FLOOR,
                                             "THERMAL_DEV_CX_MGR",
                                             NPA_CLIENT_REQUIRED);

  vdd_restrict_mx_clnt = npa_create_sync_client(VCS_NPA_RESOURCE_VDD_MX_ACTIVE_FLOOR,
                                             "THERMAL_DEV_MX_MGR",
                                             NPA_CLIENT_REQUIRED);
											 
  if (vdd_restrict_cx_clnt == NULL)
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 2,
                     "%s: NPA client create error for %s", __func__,
                     VCS_NPA_RESOURCE_VDD_CX_ACTIVE_FLOOR);
    goto handle_return;
  }

  if (vdd_restrict_mx_clnt == NULL)
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 2,
                     "%s: NPA client create error for %s", __func__,
                     VCS_NPA_RESOURCE_VDD_MX_ACTIVE_FLOOR);
    goto handle_return;
  }
  if (vdd_rstr_enabled)
  {
    request = RAIL_VOLTAGE_LEVEL_NOMINAL;
    npa_issue_required_request(vdd_restrict_cx_clnt, request);
    npa_issue_required_request(vdd_restrict_mx_clnt, request);
  }
  else
  {
    npa_cancel_request(vdd_restrict_cx_clnt);
    npa_cancel_request(vdd_restrict_mx_clnt);
  }

  vdd_restrict_init_npa = 1;
handle_return:
  qurt_mutex_unlock(&vdd_restrict_mtx);
}

/*===========================================================================
FUNCTION mx_min_req_npa_avail_cb

Callback function which will be invoked when associated npa resource (mx rail)
becomes available.

ARGUMENTS
  context: Unused - to comply with prototype
  event_type: Unused - to comply with prototype
  data: Unused - to comply with prototype
  data_size: Unused - to comply with prototype

RETURN VALUE
  None
===========================================================================*/
static void mx_min_req_npa_avail_cb( void *context,
                                     unsigned int event_type,
                                     void *data,
                                     unsigned int data_size )
{
  therm_log_printf(THERM_LOG_LEVEL_DEBUG, 1, "%s", __func__);

  qurt_mutex_lock(&mx_min_req_mtx);
  mx_min_req_clnt = npa_create_sync_client(VCS_NPA_RESOURCE_VDD_MX,
                                           "THERM_MX_MIN_DEV_MGR",
                                           NPA_CLIENT_REQUIRED);

  if( mx_min_req_clnt == NULL )
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 2,
                     "%s: NPA client create error for %s", __func__,
                     VCS_NPA_RESOURCE_VDD_MX_ACTIVE_FLOOR);
    goto handle_return;
  }

  if( mx_min_req_enabled )
  {
    npa_issue_required_request(mx_min_req_clnt,
                               VCS_CORNER_RETENTION_PLUS);
  }

  mx_min_req_init_npa = 1;
handle_return:
  qurt_mutex_unlock(&mx_min_req_mtx);
}

/*===========================================================================
FUNCTION vdd_rstr_init


Function for reading initial VDD RESTRICTION parameter.

ARGUMENTS
  request

RETURN VALUE
  none
===========================================================================*/
void vdd_rstr_init(void)
{
  qurt_mutex_init(&vdd_restrict_mtx);
  therm_log_printf(THERM_LOG_LEVEL_STATS, 1, "Device ( vdd_restriction:%d )",
                   vdd_rstr_enabled);
  if (vdd_restrict_qmi_request(vdd_rstr_enabled) != 0)
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 1,
                     "%s: Error setting init QMI req", __func__);
  }
  npa_resources_available_cb(2, resources, vdd_rstr_npa_avail_cb,
                            NULL);
}

/*===========================================================================
FUNCTION vdd_restriction_request


Function for enabling/disabling VDD RESTRICTION.

ARGUMENTS
  request


RETURN VALUE
  0 on success, -1 on failure.
===========================================================================*/
int vdd_restriction_request(int request)
{
  int ret = -1;

  request = (request <= 0) ? 0 : 1;

  qurt_mutex_lock(&vdd_restrict_mtx);
  vdd_rstr_enabled = request;

  if (vdd_restrict_qmi_request(vdd_rstr_enabled) != 0)
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 1,
                     "%s: Error making QMI req", __func__);
  }

  if (vdd_restrict_init_npa)
  {
    if (vdd_rstr_enabled)
    {
      npa_issue_required_request(vdd_restrict_cx_clnt,
                                 RAIL_VOLTAGE_LEVEL_NOMINAL);
      npa_issue_required_request(vdd_restrict_mx_clnt,
                                 RAIL_VOLTAGE_LEVEL_NOMINAL);
    }
    else
    {
      npa_cancel_request(vdd_restrict_cx_clnt);
      npa_cancel_request(vdd_restrict_mx_clnt);
    }
    therm_log_printf(THERM_LOG_LEVEL_INFO, 1,
                     "ACTION: VDD_RESTRICTION - Setting to %d",
                     request);
    ret = 0;
  }

  therm_log_printf(THERM_LOG_LEVEL_STATS, 1, "Device ( vdd_restriction:%d )",
                   vdd_rstr_enabled);
  qurt_mutex_unlock(&vdd_restrict_mtx);
  return ret;
}

/*===========================================================================
FUNCTION mx_min_request_init
  Initializes parameters for Mx Min request device

ARGUMENT
  None

RETURN VALUE
  None
===========================================================================*/
void mx_min_request_init(void)
{
  enum therm_msm_id msm_id = therm_get_msm_id();

  if (( msm_id != THERM_MDM_9x35 ) &&
      ( msm_id != THERM_MDM_9x45 ) &&
      ( msm_id != THERM_MDM_9x55 ) &&
      ( msm_id != THERM_MDM_9x06 ) &&
      ( msm_id != THERM_MDM_9205 ))
    return;

  /* Currently this device is used only on 9x35 and 9x45 */
  mx_min_request_used = 1;

  qurt_mutex_init(&mx_min_req_mtx);
  therm_log_printf(THERM_LOG_LEVEL_STATS, 1, "Device ( mx_min_request:%d )",
                     mx_min_req_enabled);

  /* Registering npa callback when mx rail resource becomes available */
  npa_resource_available_cb(VCS_NPA_RESOURCE_VDD_MX, mx_min_req_npa_avail_cb,
                            NULL);
}

/*===========================================================================
FUNCTION  mx_min_request
  Function to enable/disable mx min request device action.

ARGUMENT
  request: Indicates mx min request (1 enable, 0 disable)

RETURN VALUE
  0 on success, -1 on failure
===========================================================================*/
int mx_min_request(int request)
{
  int ret = -1;

  if (!mx_min_request_used)
    return ret;

  request = (request <= 0) ? 0 : 1;

  qurt_mutex_lock(&mx_min_req_mtx);
  mx_min_req_enabled = request;

  if (mx_min_req_init_npa)
  {
    if (mx_min_req_enabled)
    {
      /* Issue mx min request */
      npa_issue_required_request(mx_min_req_clnt,
                                 VCS_CORNER_RETENTION_PLUS);
    }
    else
    {
      npa_cancel_request(mx_min_req_clnt);
    }

    therm_log_printf(THERM_LOG_LEVEL_INFO, 1,
                     "ACTION: MX_MIN_REQUEST - Setting to %d",
                     request);
    ret = 0;
  }

  therm_log_printf(THERM_LOG_LEVEL_STATS, 1, "Device ( mx_min_request:%d )",
                   mx_min_req_enabled);
  qurt_mutex_unlock(&mx_min_req_mtx);

  return ret;
}

/*===========================================================================
FUNCTION cx_min_req_npa_avail_cb

Callback function which will be invoked when associated npa resource (cx rail)
becomes available.

ARGUMENTS
  context: Unused - to comply with prototype
  event_type: Unused - to comply with prototype
  data: Unused - to comply with prototype
  data_size: Unused - to comply with prototype

RETURN VALUE
  None
===========================================================================*/
static void cx_min_req_npa_avail_cb( void *context,
                                     unsigned int event_type,
                                     void *data,
                                     unsigned int data_size )
{
  therm_log_printf(THERM_LOG_LEVEL_DEBUG, 1, "%s", __func__);

  qurt_mutex_lock(&cx_min_req_mtx);
  cx_min_req_clnt = npa_create_sync_client(VCS_NPA_RESOURCE_VDD_CX,
                                           "THERM_CX_MIN_DEV_MGR",
                                           NPA_CLIENT_REQUIRED);

  if( cx_min_req_clnt == NULL )
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 2,
                     "%s: NPA client create error for %s", __func__,
                     VCS_NPA_RESOURCE_VDD_CX);
    goto handle_return;
  }

  if( cx_min_req_enabled )
  {
    npa_issue_required_request(cx_min_req_clnt,
                               VCS_CORNER_RETENTION_PLUS);
  }

  cx_min_req_init_npa = 1;
handle_return:
  qurt_mutex_unlock(&cx_min_req_mtx);
}

/*===========================================================================
FUNCTION cx_min_request_init
  Initializes parameters for Cx Min request device

ARGUMENT
  None

RETURN VALUE
  None
===========================================================================*/
void cx_min_request_init(void)
{
  enum therm_msm_id msm_id = therm_get_msm_id();

  if ( (msm_id != THERM_MDM_9x55) )
    return;

  /* Currently this device is used only on 9x45 */
  cx_min_request_used = 1;

  qurt_mutex_init(&cx_min_req_mtx);
  therm_log_printf(THERM_LOG_LEVEL_STATS, 1, "Device ( cx_min_request:%d )",
                     cx_min_req_enabled);

  /* Registering npa callback when cx rail resource becomes available */
  npa_resource_available_cb(VCS_NPA_RESOURCE_VDD_CX, cx_min_req_npa_avail_cb,
                            NULL);
}

/*===========================================================================
FUNCTION  cx_min_request
  Function to enable/disable cx min request device action.

ARGUMENT
  request: Indicates cx min request (1 enable, 0 disable)

RETURN VALUE
  0 on success, -1 on failure
===========================================================================*/
int cx_min_request(int request)
{
  int ret = -1;

  if (!cx_min_request_used)
    return ret;

  request = (request <= 0) ? 0 : 1;

  qurt_mutex_lock(&cx_min_req_mtx);
  cx_min_req_enabled = request;

  if (cx_min_req_init_npa)
  {
    if (cx_min_req_enabled)
    {
      /* Issue cx min request */
      npa_issue_required_request(cx_min_req_clnt,
                                 VCS_CORNER_RETENTION_PLUS);
    }
    else
    {
      npa_cancel_request(cx_min_req_clnt);
    }

    therm_log_printf(THERM_LOG_LEVEL_INFO, 1,
                     "ACTION: CX_MIN_REQUEST - Setting to %d",
                     request);
    ret = 0;
  }

  therm_log_printf(THERM_LOG_LEVEL_STATS, 1, "Device ( cx_min_request:%d )",
                   cx_min_req_enabled);
  qurt_mutex_unlock(&cx_min_req_mtx);

  return ret;
}


#ifdef CPR_COLD_REQUEST
/*===========================================================================
FUNCTION cpr_cold_request_init


Function for setting initial cpr_cold state.

ARGUMENTS
  request

RETURN VALUE
  none
===========================================================================*/
void cpr_cold_request_init(void)
{
  cpr_cold_req_state = CPR_COLD_COOL; /* Cool state */
  qurt_mutex_init(&cpr_cold_req_mtx);
  cpr_cold_request(cpr_cold_req_state);
}

/*===========================================================================
FUNCTION cpr_cold_request


Function for relaying cpr cold state.

ARGUMENTS
  request


RETURN VALUE
  0 on success, -1 on failure.
===========================================================================*/
int cpr_cold_request(int request)
{
  int ret = -1;
  static bool cpr_cold_init = false;
  static kvp_t *kvp;
  uint32 key =  0x706d6574; // = "temp"
  uint32 value;
  unsigned msg_id;


  if (!kvp)
  {
    /* Allocate once and there is no valid code path
       requiring it be destroyed */
    kvp = kvp_create(12);
    if (!kvp)
    {
      therm_log_printf(THERM_LOG_LEVEL_ERROR, 1,
                     "%s: Error kvp_create", __func__);
      return ret;
    }
    cpr_cold_init = true;
  }

  request = (request <= 0) ? 0 : request;

  qurt_mutex_lock(&cpr_cold_req_mtx);
  cpr_cold_req_state = request;

  if (cpr_cold_qmi_request(cpr_cold_req_state) != 0)
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 1,
                     "%s: Error making QMI req", __func__);
  }

  if (cpr_cold_init)
  {
    switch (cpr_cold_req_state)
    {
    default:
    case CPR_COLD_NORMAL:
      value = CPR_RPM_NORMAL;
      break;
    case CPR_COLD_COOL:
      value = CPR_RPM_COOL;
      break;
    case CPR_COLD_COLD:
      value = CPR_RPM_COLD;
      break;
    case CPR_COLD_CRITICAL_COLD:
      value = CPR_RPM_CRITICAL_COLD;
    }
    kvp_put(kvp, key, sizeof(value), (void*)&value);

    msg_id = rpm_post_request(RPM_ACTIVE_SET, RPM_RBCPR_REQ, 0, kvp);

    therm_log_printf(THERM_LOG_LEVEL_DEBUG, 2, "%s msg_id %d",
                      __func__, msg_id);

    therm_log_printf(THERM_LOG_LEVEL_INFO, 1,
                     "ACTION: CPR_COLD - Setting to %d",
                     request);
    ret = 0;
  }

  therm_log_printf(THERM_LOG_LEVEL_STATS, 1, "Device ( cpr_cold:%d )",
                   cpr_cold_req_state);
  qurt_mutex_unlock(&cpr_cold_req_mtx);
  return ret;
}
#endif
/*===========================================================================
FUNCTION cpu_req_npa_avail_cb

Callback function which will be invoked when associated npa resource 
becomes available.

ARGUMENTS
  context: 		Unused - to comply with prototype
  event_type: 	Unused - to comply with prototype
  data: 		Unused - to comply with prototype
  data_size: 	Unused - to comply with prototype

RETURN VALUE
  None
===========================================================================*/
static void cpu_req_npa_avail_cb( void *context,
   										 unsigned int event_type,
   										 void *data,
   										 unsigned int data_size )
{
  therm_log_printf(THERM_LOG_LEVEL_DEBUG, 1, "%s", __func__);
   	
  qurt_mutex_lock(&cpu_req_mtx);
  cpu_req_clnt = npa_create_sync_client("/clk/cpu",
   									    "THERM_CPU_FREQ_DEV_MGR",
   										 NPA_CLIENT_LIMIT_MAX);
   	
  if( cpu_req_clnt == NULL )
  {
   		therm_log_printf(THERM_LOG_LEVEL_ERROR, 2,
   						 "%s: NPA client create error for %s", __func__,
   						 "/clk/cpu");
   		goto handle_return;
  }
  cpu_req_init_npa = 1;
  handle_return:
  qurt_mutex_unlock(&cpu_req_mtx);
}

void cpu_request_init()
{
   	
  qurt_mutex_init(&cpu_req_mtx);
  therm_log_printf(THERM_LOG_LEVEL_STATS, 1, "Device (cpu_request_init:%d )",
   						 cpu_req_enabled);
   	
  /* Registering npa callback when cx rail resource becomes available */
  npa_resource_available_cb("/clk/cpu", cpu_req_npa_avail_cb,NULL);		  	
}

int cpu_request( int request)
{
  int ret = -1;
  therm_log_printf(THERM_LOG_LEVEL_STATS, 1, "Device (cpu_request: %d )",request);
  if(cpu_req_clnt && cpu_req_init_npa) {
		qurt_mutex_lock (&cpu_req_mtx);
		if(request){
			npa_issue_limit_max_request (cpu_req_clnt, request);
		}else{
			npa_complete_request(cpu_req_clnt);
		}
		qurt_mutex_unlock(&cpu_req_mtx);
		ret = 0;
  }
  return ret;
}

