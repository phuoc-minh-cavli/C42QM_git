/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GFC_QMI module which is the interface between QMI and GM

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


  Copyright (c) 2008 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2015 Qualcomm Atheros, Inc.
  All Rights Reserved.
  Qualcomm Atheros Confidential and Proprietary.
  Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

Version Control

$Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/sm/tm/gfc_qmi/src/gfc_qmi_internal.c#2 $
$DateTime: 2020/12/21 22:57:28 $

=============================================================================

EDIT HISTORY FOR FILE

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
12/07/16   ss     Configure to allow Cell CPI Fix
1/18/16    ss       Supporting back Max Pos Unc in GF config and fixing leak in txIds
10/26/15   ss       Supporting Geofence Client Config indication
10/20/15   ssu     Adding default geofence confidence to HIGH
06/09/15   ssu     Adding NV for GM filtering of WLAN freq.  
05/06/15   sjk    Adding a default configuration for Max GFs required by GFC_QMI
04/02/15    ssu   Batch Dwell notification for geofence.
02/26/15   ssu      Adding new NV items for GM LOWI integration.
01/26/15    ssu   Adding geofence custom responsiveness.
11/12/14   sj     Make changes for GF Config items.
08/25/14   jv    16-bit SVID Support
05/07/14   ssu   SMART and KW compilation errors fixed.
04/03/14   ssu   Additional Fix details parameters for Geofence Breach reports
03/24/12   ss       Supporting operation mode settting to effect Geofencing operation mode
03/10/14   ssu    Addressing a few KW errors .
02/21/14   ssu     Adding NV for configuring motion sensing distance in case of challenging GPS env.
02/20/14   ssu     Adding a NV for modifying the medium responsiveness backoff value and gps challenging env backoff config.
02/21/14   ssu     Adding GM CPI request rate at bootup and resolving a bug in validation in purge resp handler.
02/10/14   ssu     Adding a NV for modifying the rate at which GM requests CPI injection.
02/10/14   ssu    Updating GFC request queue after receiving purge response from GM.
02/06/14   ss    Supporting Medium Confidence responsivenes value config
01/31/14    ss    Supporting Max acceptable GF PUNC confguration from QMI
1/29/14    ssu     Populate the option pointer while requesting for pos fixes through GFC.
01/24/14   ssu     Setting the height uncertainty flag in GFC breach report when included.
02/29/12   ssu     Adding geofence purge request API.
02/08/12   ssu     Accepting transaction ID value of 0.
02/06/12   ssu     Adding error checks for cases where the QMI does not register
                   with GFC_QMI
11/15/11   ssu     Initial Release.

=============================================================================

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#include "gfc_qmi_api.h"
#include "pdapi_lite.h"
#include "gfc_qmi.h"
#include "gm_api.h"
#include "tm_prtl_iface.h"
#include "tm_pdapi_iface.h"
#include "tm_common.h"
#include "gfc_qmi_internal.h"
#include "tm_data.h"
#include "tm_pdapi_client.h"
#include "math.h"
//#include "tech_sel_api.h"
#include "gnss_calendar_util.h"

/* The maximum number of concurrent geofence requests */
#define GFC_QMI_GM_MAX_GF_DEFAULT (200)

#define MAX_CONCUR_GEOFENCE_NUM 32
#define GFC_QMI_INVALID_TRXN_ID 0xFFFFFFFF
/* Use this transaction ID when QMI indication notification not desired */

/* offset (in meters) that should be subtracted from altitude sent
   in the GM position report to calculate the altitude w.r.t
   WGS-84 ellipsoid  */
#define GFC_QMI_ALTITUDE_WRT_WGS84_ELLIPSOID_OFFSET (500)

/* Horizontal unc confidence pct */
#define GFC_QMI_HOR_UNC_CONF_PCT 39

#define GFC_QMI_INVALID_CLIENT_HANDLE 0xFF


typedef struct
{
  /* Function pointers registered for callbacks from GFC_QMI */
  gfc_qmi_add_geofence_rsp_f_type       *p_add_rsp_func;
  gfc_qmi_delete_geofence_rsp_f_type    *p_delete_rsp_func;
  gfc_qmi_purge_geofence_rsp_f_type     *p_purge_rsp_func;
  gfc_qmi_breach_geofence_rsp_f_type    *p_breach_rsp_func;
  gfc_qmi_general_notify_rsp_f_type     *p_general_notify_rsp_func;
  gfc_qmi_query_geofence_ids_rsp_f_type *p_geofence_ids_rsp_func;
  gfc_qmi_set_geofence_engine_config_rsp_f_type *p_set_gf_eng_conf_rsp_func;
}gfc_qmi_cb_s_type;

typedef struct
{
  uint32 q_qmi_trxn_id;
  uint32 q_gm_trxn_id;
  boolean b_used;
}gfc_qmi_trxn_id_s_type;

typedef struct
{
  /* Identifier for the GFC_QMI client for GM*/
  sm_gm_client_type z_client;

  /*Identifier for the CPI client for GM*/
  sm_gm_client_type z_cpi_client;
  /* Handle to the GM core for GFC_QMI*/
  sm_gm_client_handle u_gm_handle;

  /* Handle to the GM core for GFC_QMI*/
  sm_gm_client_handle u_gm_cpi_handle;

  /* Handle to the GM core for GTP1.0 client*/
  sm_gm_client_handle u_gm_gtp_1_0_handle;

  /* Handle to the GM core for GFC_QMI*/
  sm_gm_client_handle u_gm_gtp_2_0_handle;

  /* The callback functions registered by QMI
  for Geofencing operations. */
  gfc_qmi_cb_s_type z_gfc_qmi_cb_table;

  /* The table which maintains the mapping for the transaction Ids from QMI
     with the transaction id for GM */
  gfc_qmi_trxn_id_s_type trxn_id_table[MAX_CONCUR_GEOFENCE_NUM];

  /* The currently used GM transaction ID */
  uint32 q_curr_gm_trxn_id;

  /* A structure that contains pointer to API functions exposed by TmCore to
     handle its registerd clients for callbacks */
  tm_prtl_cb_s_type z_prtlIfaceTbl;
  /* Structure to obtain the position fix */
  tm_pdapi_get_pos_param_s_type z_get_pos_param;

  /* Optional parameters for TM core position fix. */
  pdsm_lite_pd_option_s_type z_get_pos_options;        /* PD option type */
  pdsm_lite_pd_qos_type z_get_pos_qos;                 /* QOS for this pos fix request */

  /* QOS parameters for TM core position fix. */

  /* Flag to indicate whether QMI has registered with GFC_QMI */
  boolean b_qmi_registered;

  /*Flag to indicate whether CPI was requested*/
  boolean b_cpi_requested;

  /*Request Id for CPI request*/
  //q_TselRequestId q_cpi_request_id;

  /*Timer to wait for cpi position request*/
  os_TimerMsgType* p_cpi_wait_timer;

}gfc_qmi_internal_info_s_type;

gfc_qmi_internal_info_s_type *pz_gfc_qmi_info = NULL;


boolean gfc_qmi_internal_register_cbs(
  const gfc_qmi_cb_rsp_s_type* p_geofence_func_cbs
)
{
  if(NULL == pz_gfc_qmi_info)
  {
    return FALSE;
  }
  if(pz_gfc_qmi_info->b_qmi_registered == FALSE)
  {
    /* Initialise the function pointers to point to the callbacks supplied */
    pz_gfc_qmi_info->z_gfc_qmi_cb_table.p_add_rsp_func = p_geofence_func_cbs->add_rsp_func;
    pz_gfc_qmi_info->z_gfc_qmi_cb_table.p_delete_rsp_func = p_geofence_func_cbs->delete_rsp_func;
    pz_gfc_qmi_info->z_gfc_qmi_cb_table.p_purge_rsp_func = p_geofence_func_cbs->purge_rsp_func;
    pz_gfc_qmi_info->z_gfc_qmi_cb_table.p_breach_rsp_func = p_geofence_func_cbs->breach_rsp_func;
    pz_gfc_qmi_info->z_gfc_qmi_cb_table.p_general_notify_rsp_func = p_geofence_func_cbs->general_notify_rsp_func;
    pz_gfc_qmi_info->z_gfc_qmi_cb_table.p_geofence_ids_rsp_func = p_geofence_func_cbs->geofence_ids_rsp_func;
    pz_gfc_qmi_info->z_gfc_qmi_cb_table.p_set_gf_eng_conf_rsp_func = p_geofence_func_cbs->set_gf_eng_conf_rsp_func;
    
    pz_gfc_qmi_info->b_qmi_registered = TRUE;
  }
  return TRUE;
}



/*
 ******************************************************************************
 *
 * gfc_qmi_getnext_gm_trxn_id
 *
 * Function description:
 *  Get next transaction id to send request to Geofence module. A total of
 *  MAX_CONCUR_GEOFENCE_NUM requests are supported at the same time.
 *  The valid GM transaction ID is returned from this function.
 *
 * Parameters:
 *    qmi_trxn_id: The QMI transaction ID which is passed in from QMI .
 *    This transaction ID is mapped with the GM transaction ID by the GFC_QMI.
 * Return value:
 *    The GM transaction ID mapped to the passed in QMI transaction ID .
 *
 ******************************************************************************
*/
uint32 gfc_qmi_getnext_gm_trxn_id(uint32 qmi_trxn_id)
{
  uint32 i, gm_base_trxn_id;

  if(NULL == pz_gfc_qmi_info)
  { 
    return GFC_QMI_INVALID_TRXN_ID;
  }

  /* Note that the base transaction ID starts from TM_GFC_QMI_GM_TRANSACTION_ID_BASE + 2 .
      The transaction ID TM_GFC_QMI_GM_TRANSACTION_ID_BASE is reserved for unsolicited messages from GM to GFC_QMI.
      The transaction ID TM_GFC_QMI_GM_TRANSACTION_ID_BASE+1 is reserved for non qmi txn id PASSED FROM gfc AT BOOT UP. .
  */
  if(qmi_trxn_id == NON_QMI_TRXN_ID)
  {
    gm_base_trxn_id = TM_GFC_QMI_GM_TRANSACTION_ID_BASE + 1;
  }
  else
  {
    gm_base_trxn_id = TM_GFC_QMI_GM_TRANSACTION_ID_BASE + 2;
  }

  if ( (pz_gfc_qmi_info->q_curr_gm_trxn_id < gm_base_trxn_id) ||
       ( pz_gfc_qmi_info->q_curr_gm_trxn_id >=
         ( TM_GFC_QMI_GM_TRANSACTION_ID_BASE + TM_UMTS_UP_GM_TRANSACTION_ID_RANGE )
       )
     )
  {
    pz_gfc_qmi_info->q_curr_gm_trxn_id = gm_base_trxn_id;
  }
  else
  {
    pz_gfc_qmi_info->q_curr_gm_trxn_id++;
  }
  /* Store the transaction Id mappings of GM and QMI in the table */
  for (i = 0 ; i < MAX_CONCUR_GEOFENCE_NUM; i++)
  {
    // Search for an unused entry
    if(pz_gfc_qmi_info->trxn_id_table[i].b_used == FALSE)
    {
      pz_gfc_qmi_info->trxn_id_table[i].q_gm_trxn_id = pz_gfc_qmi_info->q_curr_gm_trxn_id;
      pz_gfc_qmi_info->trxn_id_table[i].q_qmi_trxn_id = qmi_trxn_id;
      pz_gfc_qmi_info->trxn_id_table[i].b_used = TRUE;
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"gfc_qmi_getnext_gm_trxn_id :: i %d GM TID %d , QMI TID %d",
              i,
              pz_gfc_qmi_info->trxn_id_table[i].q_gm_trxn_id,
              pz_gfc_qmi_info->trxn_id_table[i].q_qmi_trxn_id);
      return pz_gfc_qmi_info->trxn_id_table[i].q_gm_trxn_id;
    }
  }
  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"gfc_qmi_getnext_gm_trxn_id :: Entry not found return %d ",
          GFC_QMI_INVALID_TRXN_ID,0,0);
  return GFC_QMI_INVALID_TRXN_ID;
}



/*
 ******************************************************************************
 *
 * gfc_qmi_get_qmi_trxn_id
 *
 * Function description:
 *  Get the QMI transaction ID corresponding to the mapped GM transaction ID.
 *
 * Parameters:
 *    gm_trxn_id: The transaction ID passed in from the GM core as a result of its response
 *    to GFC_QMI
 *    This transaction ID is mapped with the QMI transaction ID by the GFC_QMI.
 * Return value:
 *    The QMI transaction ID corresponding to the passed in GM transaction ID .
 *
 ******************************************************************************
*/
uint32 gfc_qmi_get_qmi_trxn_id(uint32 gm_trxn_id)
{
  uint32 i;

  if(NULL == pz_gfc_qmi_info)
  {
	return GFC_QMI_INVALID_TRXN_ID;
  }

  /* Search for the corresponding QMI transaction ID from GM transaction ID */
  for (i = 0 ; i < MAX_CONCUR_GEOFENCE_NUM; i++)
  {
    if(pz_gfc_qmi_info->trxn_id_table[i].q_gm_trxn_id == gm_trxn_id)
    {
      /* Indicate that this entry in the table is no longer used and available for reuse */
      pz_gfc_qmi_info->trxn_id_table[i].b_used = FALSE;
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"gfc_qmi_get_qmi_trxn_id i %d GM TID :: %d , QMI TID :: %d",
              i,
              pz_gfc_qmi_info->trxn_id_table[i].q_gm_trxn_id,
              pz_gfc_qmi_info->trxn_id_table[i].q_qmi_trxn_id);
      return pz_gfc_qmi_info->trxn_id_table[i].q_qmi_trxn_id;
    }
  }
  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"gfc_qmi_get_qmi_trxn_id No entry found return %d",GFC_QMI_INVALID_TRXN_ID,0,0);
  return GFC_QMI_INVALID_TRXN_ID;
}


/*===========================================================================

FUNCTION gfc_qmi_internal_map_gm_error_code_to_loc

DESCRIPTION
  This function maps the return error codes of GM to error codes of QMI .
  The error codes may arise from any operation ( add, delete, purge etc. )

PARAMETERS
  gm_error_code: The error code from the GM .
  p_gfc_qmi_error_code: The error code which is to be converted to.

DEPENDENCIES: none.

RETURN VALUE: none.

SIDE EFFECTS: none.

===========================================================================*/
void gfc_qmi_internal_map_gm_error_code_to_loc(uint8 gm_ret_code, qmiLocStatusEnumT_v02* p_loc_ret_code)
{
  // Map the error code returned from GM.
  switch(gm_ret_code)
  {
	 case GEOFENCE_ERROR_NONE:
	 case GEOFENCE_DELETE_ON_CLIENT_REQUEST:
	 case GEOFENCE_DELETE_ON_STOP_TIME:
	 case GEOFENCE_DELETE_ON_MAX_REPORTS:
      *p_loc_ret_code = eQMI_LOC_SUCCESS_V02;
      break;
    case GEOFENCE_ERROR_INVALID_CLIENT_HANDLE:
      *p_loc_ret_code = eQMI_LOC_INVALID_PARAMETER_V02;
      break;
    case GEOFENCE_ERROR_MAX_GEOFENCE_PROGRAMED:
      *p_loc_ret_code = eQMI_LOC_MAX_GEOFENCE_PROGRAMMED_V02;
      break;
    case GEOFENCE_ERROR_GEOFENCE_NOT_FOUND:
	case GEOFENCE_ERROR_GEOFENCE_WRONG_CLIENT:
	case GEOFENCE_ERROR_INCORRECT_PARAMS:
      *p_loc_ret_code = eQMI_LOC_INVALID_PARAMETER_V02;
      break;
    case GEOFENCE_ERROR_MEMORY_ALLOC_FAIL:
      *p_loc_ret_code = eQMI_LOC_INSUFFICIENT_MEMORY_V02;
      break;
	case GEOFENCE_ERROR_IPC_SEND_FAIL:
      *p_loc_ret_code = eQMI_LOC_GENERAL_FAILURE_V02;
      break;
    default:
      *p_loc_ret_code = eQMI_LOC_GENERAL_FAILURE_V02;
      break;
  }
  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"gfc_qmi_internal_map_gm_error_code_to_loc GM error code:: %d , loc :: : %d", gm_ret_code, *p_loc_ret_code, 0);
}

/*===========================================================================

FUNCTION gfc_qmi_internal_add_gf_rsp_handler

DESCRIPTION
  This function notifies the QMI of the result of geofence add operation

PARAMETERS
  p_gm_notify_data: The data passed from the GM core.

DEPENDENCIES: none.

RETURN VALUE: none.

SIDE EFFECTS: none.

===========================================================================*/
static void gfc_qmi_internal_add_gf_rsp_handler(sm_gm_client_notification_type* p_gm_notify_data)
{
  
  qmiLocAddCircularGeofenceIndMsgT_v02 z_add_rsp = {0};

  if((NULL == pz_gfc_qmi_info)||
	  (NULL == p_gm_notify_data))
  {
	return;
  }

  if(GEOFENCE_TYPE_MO != p_gm_notify_data->notification.geofence_type)
  {
    /* We do not supported NI geofences */
	return;
  }


  // Map the error code returned from GM.
  gfc_qmi_internal_map_gm_error_code_to_loc(p_gm_notify_data->notification.return_code,
	                                        &z_add_rsp.status);
  /* Retrieve the QMI transaction ID */
  z_add_rsp.transactionId = gfc_qmi_get_qmi_trxn_id(p_gm_notify_data->q_transaction_id);
  z_add_rsp.transactionId_valid = 1;

  /** if status is success, populate geofence Id */
  if(eQMI_LOC_SUCCESS_V02 == z_add_rsp.status )
  {
    z_add_rsp.geofenceId_valid = 1;
    z_add_rsp.geofenceId = p_gm_notify_data->notification.geofence_id;
  }

  if(z_add_rsp.status == eQMI_LOC_SUCCESS_V02)
  {
    MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"GFC_QMI::Geofence Add event SUCCESS :: TID=%d GID=%d\n",
             z_add_rsp.transactionId,
             z_add_rsp.geofenceId);
  }
  else
  {
    MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"GFC_QMI::Geofence add event FAILURE :: TID=%d Error_code=%d \n",
             z_add_rsp.transactionId,
             z_add_rsp.status);
  }

  // Notify the QMI
  if (pz_gfc_qmi_info->z_gfc_qmi_cb_table.p_add_rsp_func != NULL)
  {
    pz_gfc_qmi_info->z_gfc_qmi_cb_table.p_add_rsp_func(&z_add_rsp);
  }

}

/*===========================================================================

FUNCTION gfc_qmi_internal_delete_gf_rsp_handler

DESCRIPTION
  This function notifies the QMI of the result of the geofence delete operation

PARAMETERS
  p_gm_notify_data: The data passed from the GM core.

DEPENDENCIES: none.

RETURN VALUE: none.

SIDE EFFECTS: none.

===========================================================================*/
static void gfc_qmi_internal_delete_gf_rsp_handler(sm_gm_client_notification_type* p_gm_notify_data)
{
  qmiLocDeleteGeofenceIndMsgT_v02 z_del_rsp = {0};

  if((NULL == pz_gfc_qmi_info)||
	  (NULL == p_gm_notify_data))
  {
	return;
  }

  if(GEOFENCE_TYPE_MO != p_gm_notify_data->notification.geofence_type)
  {
    /* We do not supported NI geofences */
	return;
  }

  // Map the error code returned from GM.
  gfc_qmi_internal_map_gm_error_code_to_loc(p_gm_notify_data->notification.return_code,
	                                        &z_del_rsp.status);

  /* Retrieve the QMI transaction ID */
  z_del_rsp.transactionId = gfc_qmi_get_qmi_trxn_id(p_gm_notify_data->q_transaction_id);
  z_del_rsp.transactionId_valid = 1;

  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"GFC_QMI::Geofence delete rsp :: TID from GM %d\n", p_gm_notify_data->q_transaction_id, 0, 0);

  z_del_rsp.geofenceId = p_gm_notify_data->notification.geofence_id;
  z_del_rsp.geofenceId_valid = 1;

  /* Check whether the deletion happened correctly */
  if(z_del_rsp.status == eQMI_LOC_SUCCESS_V02)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"GFC_QMI::Geofence delete SUCCESS :: GID=%d Reason=%d TID=%d\n", z_del_rsp.geofenceId, p_gm_notify_data->notification.return_code, z_del_rsp.transactionId);
  }
  else
  {
    MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"GFC_QMI::Geofence delete FAILURE ::  GID=%d Error_code=%d \n",
             z_del_rsp.geofenceId,
             z_del_rsp.status);
  }
  if(pz_gfc_qmi_info->z_gfc_qmi_cb_table.p_delete_rsp_func != NULL)
  {
    pz_gfc_qmi_info->z_gfc_qmi_cb_table.p_delete_rsp_func(&z_del_rsp);
  }
}


/*===========================================================================

FUNCTION gfc_qmi_internal_purge_gf_rsp_handler

DESCRIPTION
  This function handles the purge geofence response from GM .

PARAMETERS
  p_gm_notify_data: The data passed from the GM core.

DEPENDENCIES: none.

RETURN VALUE: none.

SIDE EFFECTS: none.

===========================================================================*/
static void gfc_qmi_internal_purge_gf_rsp_handler(sm_gm_client_notification_type* p_gm_notify_data)
{
  if(NULL == pz_gfc_qmi_info)
  {    
	return;
  }

  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"GFC_QMI::Purge geofence response code %d received from GM \n",
             p_gm_notify_data->notification.return_code,
             0,
             0);

  /* Retrieve the QMI transaction ID */
  (void)gfc_qmi_get_qmi_trxn_id(p_gm_notify_data->q_transaction_id);

  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"GFC_QMI::Geofence purge rsp :: TID from GM %d\n", p_gm_notify_data->q_transaction_id, 0, 0);
  if(pz_gfc_qmi_info->z_gfc_qmi_cb_table.p_purge_rsp_func != NULL)
  {
    pz_gfc_qmi_info->z_gfc_qmi_cb_table.p_purge_rsp_func(p_gm_notify_data->notification.return_code);
  }
}


/*===========================================================================

FUNCTION gfc_qmi_construct_utc_time_from_lr

DESCRIPTION
  This function constructs UTC time form LR

PARAMETERS
  p_gm_notify_data: The data passed from the GM core.

DEPENDENCIES: none.

RETURN VALUE: none.

SIDE EFFECTS: none.

===========================================================================*/
static boolean gfc_qmi_construct_utc_time_from_lr(tm_lr_resp_type *pos_fix_p,
                                                  pdsm_lite_utc_time_s_type *utc_time_p)
{
  qword  milliseconds;
  me_Clock z_Clock;
  tm_CalendarTimeT z_UtcTime;
  uint8 q_LeapSeconds = 0;
  uint32 q_UtcFixTime = 0;
  uint32 q_TotalDays = 0;

  if(pos_fix_p == NULL || utc_time_p == NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"Null pointers passed to gfc_qmi_construct_utc_time_from_lr",0,0,0);
    return FALSE;
  }

  /* Ext pos data*/
  gps_total_msec(pos_fix_p->timestamp, milliseconds );

  gps_week_ms( milliseconds, &(z_Clock.z_Time.w_GpsWeek),
               &(z_Clock.z_Time.q_GpsMsec) );
  /* The above gps time conversion routines may cause some rounding errors
   * that cause the millisecond value not to match what had originally been
   * sent in the PE structure.  The following code checks the value and makes
   * an adjustment if needed
   */
  if (z_Clock.z_Time.q_GpsMsec % SEC_MSECS != pos_fix_p->timestamp_msec)
  {
      uint32 q_TempMS = z_Clock.z_Time.q_GpsMsec / SEC_MSECS;
      q_TempMS *= SEC_MSECS;
      z_Clock.z_Time.q_GpsMsec = q_TempMS + pos_fix_p->timestamp_msec;
  }
  /* Check for week rollover */
  gps_MsecWeekLimit((int32 *) &z_Clock.z_Time.q_GpsMsec, &z_Clock.z_Time.w_GpsWeek );

  /* Compute UTC time of fix, adjusting for the GPS leap seconds. */
  q_LeapSeconds = cgps_GetNvUtcOffset(); /*Hardcode this for now, call a PE provided API when available*/

  if (z_Clock.z_Time.q_GpsMsec/SEC_MSECS < q_LeapSeconds)
  {
    q_UtcFixTime = z_Clock.z_Time.q_GpsMsec + (WEEK_SECS - q_LeapSeconds) * SEC_MSECS;
  }
  else
  {
    q_UtcFixTime = z_Clock.z_Time.q_GpsMsec - q_LeapSeconds * SEC_MSECS;
  }

  tm_util_construct_gps_time(z_Clock.z_Time.w_GpsWeek,
                             z_Clock.z_Time.q_GpsMsec,
                             q_UtcFixTime,
                             &z_UtcTime,
                             &q_TotalDays);
  tm_util_time_to_pdsm_time(&z_UtcTime, utc_time_p);

  return TRUE;
}
/*===========================================================================

FUNCTION gfc_qmi_internal_breach_gf_rsp_handler

DESCRIPTION
  This function notifies the QMI when the geofence breach occurs.

PARAMETERS
  p_gm_notify_data: The data passed from the GM core.

DEPENDENCIES: none.

RETURN VALUE: none.

SIDE EFFECTS: none.

===========================================================================*/
static void gfc_qmi_internal_breach_gf_rsp_handler(sm_gm_client_breach_notify_type *p_gm_notify_data)
{
  tm_lr_resp_type breach_pos_fix;
  float f_vel_east, f_vel_north, f_tmp;
  pdsm_lite_utc_time_s_type z_utc_time = {0};
  uint64_t timeInSec = 0;

  qmiLocEventGeofenceBreachIndMsgT_v02 z_breach_rsp = {0};

  if(NULL == pz_gfc_qmi_info)
  {
	return;
  }
  if(GEOFENCE_TYPE_MO != p_gm_notify_data->geofence_type)
  {
    return;
  }

  z_breach_rsp.geofenceId = p_gm_notify_data->geofence_id;

  /* Fill in the breach type. */
  switch(p_gm_notify_data->breach_type)
  {
    case GEOFENCE_EVENT_ENTERING_AREA:
      z_breach_rsp.breachType = eQMI_LOC_GEOFENCE_BREACH_TYPE_ENTERING_V02;
      break;
    case GEOFENCE_EVENT_LEAVING_AREA:
      z_breach_rsp.breachType = eQMI_LOC_GEOFENCE_BREACH_TYPE_LEAVING_V02;
      break;
    default:
	  MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"Unknown breach type");
      return;
      
  }

  z_breach_rsp.breachConfidence_valid = 1;
  switch(p_gm_notify_data->breach_confidence)
  {
    case GEOFENCE_CONFIDENCE_HIGH:
      z_breach_rsp.breachConfidence = eQMI_LOC_GEOFENCE_CONFIDENCE_HIGH_V02;
      break;
    case GEOFENCE_CONFIDENCE_MEDIUM:
      z_breach_rsp.breachConfidence = eQMI_LOC_GEOFENCE_CONFIDENCE_MED_V02;
      break;
    case GEOFENCE_CONFIDENCE_LOW:
      z_breach_rsp.breachConfidence = eQMI_LOC_GEOFENCE_CONFIDENCE_LOW_V02;
      break;
    default:
	  z_breach_rsp.breachConfidence_valid = 0;
      break;
  }

  if(p_gm_notify_data->fix_included == TRUE)
  {
	z_breach_rsp.geofencePosition_valid = 1;
    tm_util_convert_lr(&p_gm_notify_data->pos_fix_rpt, &breach_pos_fix);

    /* populate the position fix information for the breach */
    // Convert the heading angle
    z_breach_rsp.geofencePosition.heading = (uint16) tm_util_nearest_long( (double)breach_pos_fix.heading / (double)HEADING_SCALE );
    z_breach_rsp.geofencePosition.heading_valid = 1;
    // Convert to altitude With Respect to Ellipsoid
    z_breach_rsp.geofencePosition.altitudeWrtEllipsoid = (float)(breach_pos_fix.height - GFC_QMI_ALTITUDE_WRT_WGS84_ELLIPSOID_OFFSET);
    z_breach_rsp.geofencePosition.altitudeWrtEllipsoid_valid = 1;;


    if(breach_pos_fix.loc_uncrtnty_a < MAX_POS_UNC_IND)
    {
      z_breach_rsp.geofencePosition.horUncEllipseSemiMajor = f_PosUncArr[breach_pos_fix.loc_uncrtnty_a];
    }
    if(breach_pos_fix.loc_uncrtnty_p < MAX_POS_UNC_IND)
    {
      z_breach_rsp.geofencePosition.horUncEllipseSemiMinor = f_PosUncArr[breach_pos_fix.loc_uncrtnty_p];
    }
    if(breach_pos_fix.loc_uncrtnty_v < MAX_POS_UNC_IND)
    {
      z_breach_rsp.geofencePosition.vertUnc = f_PosUncArr[breach_pos_fix.loc_uncrtnty_v];
      z_breach_rsp.geofencePosition.vertUnc_valid = 1;
    }
    z_breach_rsp.geofencePosition.horUncEllipseOrientAzimuth = breach_pos_fix.loc_uncrtnty_ang*5.625F;

    // Convert the latitude and longitude values from the GM engine in radians to degrees before sending it to QMI_LOC
	z_breach_rsp.geofencePosition.latitude = ((double)breach_pos_fix.latitude*(double)180)/(double)(1<<25);
	z_breach_rsp.geofencePosition.longitude = ((double)breach_pos_fix.longitude*(double)180)/(double)(1<<25);

    /* Compute the velocity component of the fix */
    if (p_gm_notify_data->pos_fix_rpt.z_NavPos.z_PosFlags.b_IsVelValid)
    {
      z_breach_rsp.geofencePosition.speedHorizontal_valid = 1;
      /* Compute the horizontal velocity component */
      /* Compute horizontal velocity from components and scale
       * to units of m/s.
      */
      f_vel_east = p_gm_notify_data->pos_fix_rpt.z_NavPos.f_VelEnu[0];
      f_vel_north = p_gm_notify_data->pos_fix_rpt.z_NavPos.f_VelEnu[1];
      f_tmp = sqrt( f_vel_east * f_vel_east + f_vel_north * f_vel_north );
      if (f_tmp < TM_CORE_MIN_VELOCTY)
      {
	    z_breach_rsp.geofencePosition.speedHorizontal = 0;
      }
      else
      {
        z_breach_rsp.geofencePosition.speedHorizontal = f_tmp;
      }
      /* Compute the vertical velocity component */
      /* Compute vertical velocity from components and scale
       * to units of m/s.
      */
      /* If fix_type = 1, set the vertical velocity in units of m/s. */
      if ( breach_pos_fix.fix_type )
      {
        z_breach_rsp.geofencePosition.speedVertical = p_gm_notify_data->pos_fix_rpt.z_NavPos.f_VelEnu[2];
		z_breach_rsp.geofencePosition.speedVertical_valid = 1;
      }
    }// end of if (p_gm_notify_data->pos_fix_rpt.z_NavPos.z_PosFlags.b_IsVelValid)
	
    if(gfc_qmi_construct_utc_time_from_lr(&(breach_pos_fix), &z_utc_time) == FALSE)
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"Error in constructing UTC time in breach report",0,0,0);
    }
	MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"UTC Date: %d",z_utc_time.date,0,0 );
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"UTC Hour: %d Min: %d Sec: %d",
            z_utc_time.hour,
            z_utc_time.minute,
            z_utc_time.sec);

	gnss_GetUnixEpochFromUTC((pdsm_utc_time_s_type*)&z_utc_time, &timeInSec);
    z_breach_rsp.geofencePosition.timestampUtc = (timeInSec*1000);
  }// end of if(p_gm_notify_data->fix_included == TRUE)
  if(pz_gfc_qmi_info->z_gfc_qmi_cb_table.p_breach_rsp_func != NULL)
  {
    pz_gfc_qmi_info->z_gfc_qmi_cb_table.p_breach_rsp_func(&z_breach_rsp);
  }
  // Since this is a QMI based geofence, the breach receipt is self acked by GM .
  // So no need to send a breach ack separately .

}

/*===========================================================================

FUNCTION gfc_qmi_internal_start_cpi_fix

DESCRIPTION
  This function is used to perform a CPI fix

PARAMETERS
  p_gm_notify_data: The data passed from the GM core.

DEPENDENCIES: none.

RETURN VALUE: none.

SIDE EFFECTS: none.

===========================================================================*/
static void gfc_qmi_internal_start_cpi_fix(void)
{
  tm_core_get_avail_wwan_pos_param_s_type z_get_avail_wwan_pos_param ;
  if(NULL == pz_gfc_qmi_info)
  {
    return;
  }
  memset(&z_get_avail_wwan_pos_param, 0, sizeof(z_get_avail_wwan_pos_param));
  z_get_avail_wwan_pos_param.source = SM_GPS_STATE_INFO_REQ_TYPE_GET_TLE_AVAIL_POS_GFC;
  z_get_avail_wwan_pos_param.cmd.p_cmd_type = NULL;
        
  (void)tm_core_handle_avail_wwan_pos(&z_get_avail_wwan_pos_param);

}

#if 0
/*===========================================================================

FUNCTION gfc_qmi_internal_stop_cpi_fix

DESCRIPTION
  This function is used to abort a CPI fix
Commented to make sm work #9x05
PARAMETERS
 void

DEPENDENCIES: none.

RETURN VALUE: none.

SIDE EFFECTS: none.

===========================================================================*/
static void gfc_qmi_internal_stop_cpi_fix(void)
{
  if(NULL == pz_gfc_qmi_info)
  {
    return;
  }

  if(pz_gfc_qmi_info->b_cpi_requested)
  {
    //if(techSel_StopCpi(pz_gfc_qmi_info->q_cpi_request_id) == TRUE)
    {
      pz_gfc_qmi_info->b_cpi_requested = FALSE;
    }
  }
}


#endif

void gfc_qmi_internal_start_gtp_1_0_fix(void)
{  
  tm_core_get_avail_wwan_pos_param_s_type z_get_avail_wwan_pos_param ;
  
  z_get_avail_wwan_pos_param.source = SM_GPS_STATE_INFO_REQ_TYPE_GET_TLE_AVAIL_POS_GFC;
  z_get_avail_wwan_pos_param.cmd.p_cmd_type = NULL;
  
  (void)tm_core_handle_avail_wwan_pos(&z_get_avail_wwan_pos_param);
}


void gfc_qmi_internal_start_gtp_2_0_fix(void)
{  
  tm_core_get_avail_wwan_pos_param_s_type z_get_avail_wwan_pos_param ;
  pdsm_lite_cmd_s_type s_cmd ={0};
  
  z_get_avail_wwan_pos_param.source = SM_GPS_STATE_INFO_REQ_TYPE_GET_TLE_AVAIL_POS_GFC;

  // time out for GTP 2.0 fix in seconds 
  s_cmd.cmd.pd.info.qos.gps_session_timeout = 20;

  z_get_avail_wwan_pos_param.cmd.p_cmd_type = &s_cmd;
  
  (void)tm_core_handle_avail_wwan_pos(&z_get_avail_wwan_pos_param);
}

/*===========================================================================
FUNCTION gfc_qmi_internal_handle_gtp_1_0_fix

DESCRIPTION
This function handles GTP 1.0 fix from TM CORE

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE

===========================================================================*/
void gfc_qmi_internal_handle_gtp_1_0_fix(const sm_FixInfoStructType *p_gnss_fix_rpt)
{
  sm_gm_pos_fix_info fix_info;

  /*Send the position to GM core*/
  fix_info.fix_src = GEOFENCE_POS_FIX_SRC_GTP_1_0;
  fix_info.pos_fix = *p_gnss_fix_rpt;      
  fix_info.fix_type = GEOFENCE_POS_FIX_TYPE_FINAL;

  (void)gm_position_update(&fix_info); 
}

/*===========================================================================
FUNCTION gfc_qmi_internal_handle_gtp_2_0_fix

DESCRIPTION
This function handles GTP 2.0 fix from TM CORE

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE

===========================================================================*/
void gfc_qmi_internal_handle_gtp_2_0_fix(const sm_FixInfoStructType *p_gnss_fix_rpt)
{
  sm_gm_pos_fix_info fix_info;

  /*Send the position to GM core*/
  fix_info.fix_src = GEOFENCE_POS_FIX_SRC_GTP_2_0;
  fix_info.pos_fix = *p_gnss_fix_rpt;      
  fix_info.fix_type = GEOFENCE_POS_FIX_TYPE_FINAL;

  (void)gm_position_update(&fix_info); 
}

/*===========================================================================

FUNCTION gfc_qmi_internal_pos_request_handler

DESCRIPTION
  This function is used to perform the standalone positon request of the GFC_QMI

PARAMETERS
  p_gm_notify_data: The data passed from the GM core.

DEPENDENCIES: none.

RETURN VALUE: none.

SIDE EFFECTS: none.

===========================================================================*/
static void gfc_qmi_internal_pos_request_handler(void *p_gm_notify_data)
{
  sm_gm_client_pos_req_type* pos_request = (sm_gm_client_pos_req_type*)p_gm_notify_data;

  if(pos_request == NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"Null Pos Request pointer",0,0,0);
    return;
  }
  if(NULL == pz_gfc_qmi_info)
  {
	return;
  }


  if(pos_request->pos_fix_src == GEOFENCE_POS_FIX_SRC_GNSS)
  {
    pdsm_lite_pd_session_operation_e_type pd_operation_mode = ((sm_gm_client_pos_req_type*)p_gm_notify_data)->pos_operation_mode;

    /* Start a position fix request from the TM core. */
#if 0
    pz_gfc_qmi_info->z_get_pos_param.client_type = PDSM_CLIENT_LITE_TYPE_NI;
    pz_gfc_qmi_info->z_get_pos_param.client_id = pdsmclient_lite_get_client_id_map(PDSM_CLIENT_LITE_TYPE_NI);
#endif
    pz_gfc_qmi_info->z_get_pos_param.mt_initiator = MT_UP;
    pz_gfc_qmi_info->z_get_pos_param.internal_session = TRUE;

    /* The QOS needed for this position fix. */
    /* suds84. fill this in from the GM core info */
    pz_gfc_qmi_info->z_get_pos_options.fix_rate.num_fixes = ((sm_gm_client_pos_req_type*)p_gm_notify_data)->num_fixes;
    pz_gfc_qmi_info->z_get_pos_options.fix_rate.time_between_fixes_ms = ((sm_gm_client_pos_req_type*)p_gm_notify_data)->time_between_fixes * 1000;
    pz_gfc_qmi_info->z_get_pos_qos.gps_session_timeout = ((sm_gm_client_pos_req_type*)p_gm_notify_data)->session_timeout;
    pz_gfc_qmi_info->z_get_pos_qos.accuracy_threshold = ((sm_gm_client_pos_req_type*)p_gm_notify_data)->horizontal_accuracy;
    pz_gfc_qmi_info->z_get_pos_options.session = PDSM_LITE_PD_SESS_TYPE_NEW;
    pz_gfc_qmi_info->z_get_pos_options.operation_mode = pd_operation_mode;
    pz_gfc_qmi_info->z_get_pos_param.qos_ptr = &pz_gfc_qmi_info->z_get_pos_qos;
    pz_gfc_qmi_info->z_get_pos_param.option_ptr = &pz_gfc_qmi_info->z_get_pos_options;

    /* Start the position fix with the TM Core. Note that the GM would be notified when the fix becomes available . */
    if(tm_core_handle_get_pos(&pz_gfc_qmi_info->z_get_pos_param) == FALSE)
    {
      /*Send notification to GM that pos request failed*/
      gm_position_fix_request_fail(pos_request->pos_fix_src);
    }
  }
  else if(pos_request->pos_fix_src == GEOFENCE_POS_FIX_SRC_CPI)
  {
    gfc_qmi_internal_start_cpi_fix();
  }
  else if (pos_request->pos_fix_src == GEOFENCE_POS_FIX_SRC_GTP_1_0)
  {
    gfc_qmi_internal_start_gtp_1_0_fix();
  }
  else if (pos_request->pos_fix_src == GEOFENCE_POS_FIX_SRC_GTP_2_0)
  {
    gfc_qmi_internal_start_gtp_2_0_fix();
  }

  else
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"GFC_QMI can not handle fix request for src = %d",pos_request->pos_fix_src,0,0);
  }
}


/*===========================================================================

FUNCTION gfc_qmi_internal_pos_abort_handler

DESCRIPTION
  This function is used to perform position abort

PARAMETERS
  p_gm_notify_data: The data passed from the GM core.

DEPENDENCIES: none.

RETURN VALUE: none.

SIDE EFFECTS: none.

===========================================================================*/
static void gfc_qmi_internal_pos_abort_handler(void *p_gm_notify_data)
{
  sm_gm_client_abort_pos_req_type* pos_abort_request = (sm_gm_client_abort_pos_req_type*)p_gm_notify_data;


  if(pos_abort_request == NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"Null Pos Abort pointer",0,0,0);
    return;
  }

  if(pos_abort_request->pos_fix_src == GEOFENCE_POS_FIX_SRC_CPI)
  {
   // gfc_qmi_internal_stop_cpi_fix();
  }
  else if(pos_abort_request->pos_fix_src == GEOFENCE_POS_FIX_SRC_GNSS)
  {
    if(tm_core_is_internal_session_on())
    {
      tm_core_abort_recover(TRUE, TM_STOP_REASON_COMPLETED);
    }
  }

}
/*===========================================================================

FUNCTION gfc_qmi_internal_unsol_event_handler

DESCRIPTION
  This function handles the unsolicited events from GM .

PARAMETERS
  p_gm_notify_data: The data passed from the GM core.

DEPENDENCIES: none.

RETURN VALUE: none.

SIDE EFFECTS: none.

===========================================================================*/
static void gfc_qmi_internal_unsol_event_handler(sm_gm_client_unsol_event_notify_type* p_gm_notify_data)
{

  qmiLocEventGeofenceGenAlertIndMsgT_v02 genAlertInd;

  if(NULL == pz_gfc_qmi_info)
  {
	return;
  }
  switch(p_gm_notify_data->unsol_event)
  {
    case GEOFENCE_UNSOL_EVENT_GPS_LOST:
      genAlertInd.geofenceAlert =
            eQMI_LOC_GEOFENCE_GEN_ALERT_GNSS_UNAVAILABLE_V02;
      break;
    case GEOFENCE_UNSOL_EVENT_GPS_AVAIL:
      genAlertInd.geofenceAlert =
          eQMI_LOC_GEOFENCE_GEN_ALERT_GNSS_AVAILABLE_V02;
      break;
    case GEOFENCE_UNSOL_EVENT_OUT_OF_SERVICE:
      genAlertInd.geofenceAlert =
            eQMI_LOC_GEOFENCE_GEN_ALERT_OOS_V02;
      break;
    case GEOFENCE_UNSOL_EVENT_TIME_INVALID:
      genAlertInd.geofenceAlert =
            eQMI_LOC_GEOFENCE_GEN_ALERT_TIME_INVALID_V02;
      break;
  case GEOFENCE_UNSOL_EVENT_MGP_UNLOADED:
      genAlertInd.geofenceAlert =
            eQMI_LOC_GEOFENCE_GEN_ALERT_MGP_UNAVAILABLE_V02;
      break;
  default:
      MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"GFC_QMI::Unhandled Unsolicited event %d received from GM \n",
             p_gm_notify_data->unsol_event);
      return;
  }
  MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"GFC_QMI::Unsolicited event %d received from GM \n",
        genAlertInd.geofenceAlert);
  if(pz_gfc_qmi_info->z_gfc_qmi_cb_table.p_general_notify_rsp_func != NULL)
  {
    pz_gfc_qmi_info->z_gfc_qmi_cb_table.p_general_notify_rsp_func(&genAlertInd);
  }

}


/*===========================================================================

FUNCTION gfc_qmi_internal_set_gf_engine_config_rsp_handler

DESCRIPTION
  This function notifies the QMI of the result of set geofence engine config request

PARAMETERS
  p_gm_notify_data: The data passed from the GM core.

DEPENDENCIES: none.

RETURN VALUE: none.

SIDE EFFECTS: none.

===========================================================================*/
static void gfc_qmi_internal_set_gf_engine_config_rsp_handler(sm_gm_client_notification_type* p_gm_notify_data)
{
  qmiLocSetGeofenceEngineConfigIndMsgT_v02 geofenceSetEngineConfigInd = {0};

  if(NULL == pz_gfc_qmi_info)
  {
    return;
  }

  gfc_qmi_internal_map_gm_error_code_to_loc( p_gm_notify_data->notification.return_code, 
	                                         &geofenceSetEngineConfigInd.status);

  /* Retrieve the QMI transaction ID */
  geofenceSetEngineConfigInd.transactionId = gfc_qmi_get_qmi_trxn_id(p_gm_notify_data->q_transaction_id);
  geofenceSetEngineConfigInd.transactionId_valid = 1;

  if(eQMI_LOC_SUCCESS_V02 == geofenceSetEngineConfigInd.status)
  {
    MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"GFC_QMI::Geofence Set GF engine config SUCCESS :: TID=0x%x \n",
           geofenceSetEngineConfigInd.transactionId);
  }
  else
  {
    MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"GFC_QMI::Geofence Set GF engine config FAILURE :: TID=0x%x \n",
          geofenceSetEngineConfigInd.transactionId);
  }
  /* Check whether it is a non-qmi transaction */
  if(geofenceSetEngineConfigInd.transactionId == NON_QMI_TRXN_ID)
  {
    MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"GFC_QMI::Non-QMI txn 0x%x dropping the GM notification\n",
           geofenceSetEngineConfigInd.transactionId);
  }
  else
  {
    // Notify the QMI
    if (pz_gfc_qmi_info->z_gfc_qmi_cb_table.p_set_gf_eng_conf_rsp_func != NULL)
    {
      pz_gfc_qmi_info->z_gfc_qmi_cb_table.p_set_gf_eng_conf_rsp_func(&geofenceSetEngineConfigInd);
    }
  }

}

/*===========================================================================

FUNCTION gfc_qmi_internal_gm_event_handler

DESCRIPTION
  This function handles the GM events passed from TM-Core.

PARAMETERS
  q_msg_type: The type of geofence message.
  p_gm_notify_data: The data passed from the GM core.

DEPENDENCIES: none.

RETURN VALUE: none.

SIDE EFFECTS: none.

===========================================================================*/
boolean gfc_qmi_internal_gm_event_handler
(
  uint32    q_msg_type,
  void      *p_msg_data
)
{
  boolean ret_val = TRUE;

  if(NULL == pz_gfc_qmi_info)
  {
    return FALSE;
  }

  if(pz_gfc_qmi_info->b_qmi_registered == FALSE)
  {
    /* The QMI is not registered with GFC_QMI. So drop the notifications which
       are sent to the QMI */
    return ret_val;

  }
  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"GFC_QMI: gm_event_handler event type : %d", q_msg_type, 0, 0);
  switch(q_msg_type)
  {
    case GEOFENCE_ADD_NOTIFY:
      /*GM notification for Add request*/
      gfc_qmi_internal_add_gf_rsp_handler((sm_gm_client_notification_type*)p_msg_data);
      break;

    case GEOFENCE_DELETE_NOTIFY:
      /*GM notification for Delete request*/
      gfc_qmi_internal_delete_gf_rsp_handler(p_msg_data);
      break;

    case GEOFENCE_PURGE_NOTIFY:
      /*GM notification for Delete request*/
      gfc_qmi_internal_purge_gf_rsp_handler(p_msg_data);
      break;

    case GEOFENCE_BREACH_NOTIFY:
      /*GM notification for Geofence Breach*/
      gfc_qmi_internal_breach_gf_rsp_handler((sm_gm_client_breach_notify_type*)p_msg_data);
      break;

    case GEOFENCE_POS_REQUEST_NOTIFY:
      /*GM notification for starting pos fix*/
      gfc_qmi_internal_pos_request_handler(p_msg_data);
      break;

    case GEOFENCE_POS_ABORT_NOTIFY:
      /*GM notification for starting pos fix*/
      gfc_qmi_internal_pos_abort_handler(p_msg_data);
      break;

    case GEOFENCE_UNSOL_EVENT_NOTIFY:
      /* GM notification for unsolicited events */
      gfc_qmi_internal_unsol_event_handler((sm_gm_client_unsol_event_notify_type*)p_msg_data);
      break;

    case GEOFENCE_SET_ENGINE_CONFIG_NOTIFY:
    case GEOFENCE_SET_CLIENT_CONFIG_NOTIFY:
      /* GM notifications for set engine config request */
      gfc_qmi_internal_set_gf_engine_config_rsp_handler((sm_gm_client_notification_type*)p_msg_data);
      break;

    default:
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"GFC_QMI: gm_event_handler unknown event type : %d", q_msg_type, 0, 0);
      break;

  }//end switch
  return ret_val;
}

/*
******************************************************************************
* gfc_qmi_internal_gm_init
*
* Function description:
*
*   This function initialises the GFC_QMI module . This sets up the internal
*   states of the GFC_QMI and registers with GM core.
*
* Parameters: None.
*
* Return value: None.
*
******************************************************************************
*/

static void gfc_qmi_internal_gm_init(void)
{
  uint8 error_code;  
  sm_gm_set_client_config_type gfClientConfig = {0};
  
  if(NULL == pz_gfc_qmi_info)
  {
    return;
  }

  sm_gm_client_type z_gtp_1_0_client;
  sm_gm_client_type z_gtp_2_0_client;

  memset(&z_gtp_1_0_client, 0, sizeof(z_gtp_1_0_client));
  memset(&z_gtp_2_0_client, 0, sizeof(z_gtp_2_0_client));

  // ToDo:: ID for GFC_QMI
  pz_gfc_qmi_info->z_client.client_id = GEOFENCE_CLIENT_QMI;
  pz_gfc_qmi_info->z_client.q_base_msg_id = TM_CORE_GM_MSG_ID_BASE;
  // ToDo:: Need to change this transaction ID since GFC_QMI is not a protocol.
  pz_gfc_qmi_info->z_client.q_base_transaction_id = TM_GFC_QMI_GM_TRANSACTION_ID_BASE;
  pz_gfc_qmi_info->z_client.q_client_thread_id = THREAD_ID_SM_TM;
  memset(&pz_gfc_qmi_info->trxn_id_table, 0, sizeof(pz_gfc_qmi_info->trxn_id_table));
  /* The currently used GM transaction ID */
  pz_gfc_qmi_info->z_client.client_cap.client_capabilites = 
    GEOFENCE_CLIENT_QMI_NOTIFICATION_CAPABLE|GEOFENCE_CLIENT_POS_FIX_CAPABLE|GEOFENCE_CLIENT_GEOFENCE_SERVICE_REQ;
  pz_gfc_qmi_info->z_client.client_cap.pos_fix_capability.pos_fix_method_cap = GEOFENCE_CLIENT_POS_FIX_STANDALONE_CAPABLE;
  pz_gfc_qmi_info->z_client.client_cap.pos_fix_capability.pos_fix_accuracy_cap = GEOFENCE_CLIENT_POS_FIX_ACCURACY_HIGH;
  pz_gfc_qmi_info->z_client.client_cap.pos_fix_capability.pos_fix_accuracy = 2; /*Minimum accuracy can be as low as 2 meters*/
  pz_gfc_qmi_info->z_client.client_cap.pos_fix_capability.pos_fix_power_cap = GEOFENCE_CLIENT_POS_FIX_POWER_PROFILE_2;
  /*AP is not woken up during standalone fix*/
  pz_gfc_qmi_info->z_client.client_cap.pos_fix_capability.high_power_proc_wakeup_cap = GEOFENCE_CLIENT_HIGH_POWER_PROC_WAKEUP_PROFILE_0;  
  /*Nw is not used during standalone fix*/  
  pz_gfc_qmi_info->z_client.client_cap.pos_fix_capability.nw_usage_cap = GEOFENCE_CLIENT_NW_USAGE_PROFILE_0;
  
  pz_gfc_qmi_info->z_client.client_cap.pos_fix_capability.is_imp_location_notification_required = FALSE;
  pz_gfc_qmi_info->z_client.client_cap.pos_fix_capability.pos_fix_wait_secs = 60; /*Geofence core shall wait for 60 seconds for the fix*/
  pz_gfc_qmi_info->z_client.client_cap.pos_fix_capability.is_prior_fix_request_indication_required = FALSE; /*Prior fix request indication required*/
  pz_gfc_qmi_info->z_client.client_cap.pos_fix_capability.prior_fix_request_indication_secs = 0; /*Prior fix request indication required 2mins beforehand*/

  gfClientConfig.q_mask = (SM_GM_CLIENT_CONFIG_MAX_GEOFENCES_MASK|
                           SM_GM_CLIENT_CONFIG_GNSS_POS_QOS_SESS_TIMEOUT_MASK |
                           SM_GM_CLIENT_CONFIG_CPI_REQUEST_RATE_MASK |
                           SM_GM_CLIENT_CHAL_GPS_ENV_BO_MIN_MASK |
                           SM_GM_CLIENT_CHAL_GPS_ENV_BO_MAX_MASK |
                           SM_GM_CLIENT_CHAL_GPS_ENV_MS_DIST_GPS_MASK |
                           SM_GM_CLIENT_CHAL_GPS_ENV_MS_DIST_CPI_MASK |
                           SM_GM_CLIENT_CONFIG_GNSS_UNAVAIL_AGGR_EXIT_MASK);
  
  gfClientConfig.q_max_geofences = GFC_QMI_GM_MAX_GF_DEFAULT;
  gfClientConfig.q_gnss_pos_qos_sess_timeout = GNSS_NV_EFS_SM_GM_GNSS_POS_QOS_SESS_TIMEOUT_DEFAULT;
  gfClientConfig.q_cpi_request_rate = GNSS_NV_EFS_SM_GM_CPI_REQUEST_RATE_DEFAULT;
  gfClientConfig.q_chal_gps_env_bo_min = GNSS_NV_EFS_SM_GM_CHAL_GPS_ENV_BO_MIN_DEFAULT;
  gfClientConfig.q_chal_gps_env_bo_max = GNSS_NV_EFS_SM_GM_CHAL_GPS_ENV_BO_MAX_DEFAULT;
  gfClientConfig.q_chal_gps_env_ms_dist_gps = GNSS_NV_EFS_SM_GM_CHAL_GPS_ENV_MS_DIST_GPS_DEFAULT;
  gfClientConfig.q_chal_gps_env_ms_dist_cpi = GNSS_NV_EFS_SM_GM_CHAL_GPS_ENV_MS_DIST_CPI_DEFAULT;   
  gfClientConfig.gnss_unavail_aggresive_exit = FALSE;
  

  if(gm_client_register(&pz_gfc_qmi_info->z_client,&pz_gfc_qmi_info->u_gm_handle,&gfClientConfig,&error_code) == FAILED)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"Not able to add gfc_qmi as Geofence client to GM, error_code %d", error_code,0,0 );
  }
  gfc_qmi_internal_set_geofence_engine_config_at_bootup();

  /*Also register as Coarse Position Injection Client*/

  pz_gfc_qmi_info->z_cpi_client.client_id = GEOFENCE_CLIENT_CPI;
  pz_gfc_qmi_info->z_cpi_client.q_base_msg_id = TM_CORE_GM_MSG_ID_BASE;
  // ToDo:: Need to change this transaction ID since GFC_QMI is not a protocol.
  pz_gfc_qmi_info->z_cpi_client.q_base_transaction_id = TM_GFC_QMI_GM_TRANSACTION_ID_BASE;
  pz_gfc_qmi_info->z_cpi_client.q_client_thread_id = THREAD_ID_SM_TM;
  /* The currently used GM transaction ID */
  pz_gfc_qmi_info->z_cpi_client.client_cap.client_capabilites = GEOFENCE_CLIENT_POS_FIX_CAPABLE;
  pz_gfc_qmi_info->z_cpi_client.client_cap.pos_fix_capability.pos_fix_method_cap = GEOFENCE_CLIENT_POS_FIX_CPI_CAPABLE;
  pz_gfc_qmi_info->z_cpi_client.client_cap.pos_fix_capability.pos_fix_accuracy_cap = GEOFENCE_CLIENT_POS_FIX_ACCURACY_MEDIUM;
  pz_gfc_qmi_info->z_cpi_client.client_cap.pos_fix_capability.pos_fix_accuracy = 50; /*Minimum accuracy can be as low as 50 meters*/
  pz_gfc_qmi_info->z_cpi_client.client_cap.pos_fix_capability.pos_fix_power_cap = GEOFENCE_CLIENT_POS_FIX_POWER_PROFILE_4;
  /*CPI always wakes up AP*/
  pz_gfc_qmi_info->z_cpi_client.client_cap.pos_fix_capability.high_power_proc_wakeup_cap = GEOFENCE_CLIENT_HIGH_POWER_PROC_WAKEUP_PROFILE_2;
  /*CPI may use Nw if assistance data is required*/
  pz_gfc_qmi_info->z_cpi_client.client_cap.pos_fix_capability.nw_usage_cap = GEOFENCE_CLIENT_NW_USAGE_PROFILE_1;
  
  pz_gfc_qmi_info->z_cpi_client.client_cap.pos_fix_capability.is_imp_location_notification_required = FALSE;
  pz_gfc_qmi_info->z_cpi_client.client_cap.pos_fix_capability.pos_fix_wait_secs = 5; /*Geofence core shall wait for 5 seconds for the fix*/
  pz_gfc_qmi_info->z_cpi_client.client_cap.pos_fix_capability.is_prior_fix_request_indication_required = FALSE; /*Prior fix request indication required*/
  pz_gfc_qmi_info->z_cpi_client.client_cap.pos_fix_capability.prior_fix_request_indication_secs = 0; /*Prior fix request indication  not required*/

  if(gm_client_register(&pz_gfc_qmi_info->z_cpi_client,&pz_gfc_qmi_info->u_gm_cpi_handle,NULL,&error_code) == FAILED)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"Not able to add gfc_qmi as CPI Geofence client to GM, error_code %d", error_code,0,0 );
  }


  /*Also register as GTP1.0 Client*/

  z_gtp_1_0_client.client_id = GEOFENCE_CLIENT_GTP_1_0;
  z_gtp_1_0_client.q_base_msg_id = TM_CORE_GM_MSG_ID_BASE;
  z_gtp_1_0_client.q_base_transaction_id = TM_GFC_QMI_GM_TRANSACTION_ID_BASE;
  z_gtp_1_0_client.q_client_thread_id = THREAD_ID_SM_TM;

  z_gtp_1_0_client.client_cap.client_capabilites = GEOFENCE_CLIENT_POS_FIX_CAPABLE;
  z_gtp_1_0_client.client_cap.pos_fix_capability.pos_fix_method_cap = GEOFENCE_CLIENT_POS_FIX_GTP_1_0_CAPABLE;
  z_gtp_1_0_client.client_cap.pos_fix_capability.pos_fix_accuracy_cap = GEOFENCE_CLIENT_POS_FIX_ACCURACY_LOW;
  z_gtp_1_0_client.client_cap.pos_fix_capability.pos_fix_accuracy = 25000; // 25000m
  z_gtp_1_0_client.client_cap.pos_fix_capability.pos_fix_power_cap = GEOFENCE_CLIENT_POS_FIX_POWER_PROFILE_0;
  z_gtp_1_0_client.client_cap.pos_fix_capability.high_power_proc_wakeup_cap = GEOFENCE_CLIENT_HIGH_POWER_PROC_WAKEUP_PROFILE_0;  
  z_gtp_1_0_client.client_cap.pos_fix_capability.nw_usage_cap = GEOFENCE_CLIENT_NW_USAGE_PROFILE_0;
  
  z_gtp_1_0_client.client_cap.pos_fix_capability.is_imp_location_notification_required = FALSE;
  z_gtp_1_0_client.client_cap.pos_fix_capability.pos_fix_wait_secs = 20; /*Geofence core shall wait for 20 seconds for the fix*/
  z_gtp_1_0_client.client_cap.pos_fix_capability.is_prior_fix_request_indication_required = TRUE; /*Prior fix request indication required*/
  z_gtp_1_0_client.client_cap.pos_fix_capability.prior_fix_request_indication_secs = 2 * 60; /*Prior fix request indication required 2mins beforehand*/

  if(gm_client_register(&z_gtp_1_0_client,&pz_gfc_qmi_info->u_gm_gtp_1_0_handle,NULL,&error_code) == FAILED)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"Not able to add GTP 1.0 as Geofence client, error_code %d", error_code,0,0 );
  }

  /*Also register as GTP2.0 Client*/

  z_gtp_2_0_client.client_id = GEOFENCE_CLIENT_GTP_2_0;
  z_gtp_2_0_client.q_base_msg_id = TM_CORE_GM_MSG_ID_BASE;
  z_gtp_2_0_client.q_base_transaction_id = TM_GFC_QMI_GM_TRANSACTION_ID_BASE;
  z_gtp_2_0_client.q_client_thread_id = THREAD_ID_SM_TM;

  z_gtp_2_0_client.client_cap.client_capabilites = GEOFENCE_CLIENT_POS_FIX_CAPABLE;
  z_gtp_2_0_client.client_cap.pos_fix_capability.pos_fix_method_cap = GEOFENCE_CLIENT_POS_FIX_GTP_2_0_CAPABLE ;
  z_gtp_2_0_client.client_cap.pos_fix_capability.pos_fix_accuracy_cap = GEOFENCE_CLIENT_POS_FIX_ACCURACY_MEDIUM;
  z_gtp_2_0_client.client_cap.pos_fix_capability.pos_fix_accuracy = 3000; // 1000m 
  z_gtp_2_0_client.client_cap.pos_fix_capability.pos_fix_power_cap = GEOFENCE_CLIENT_POS_FIX_POWER_PROFILE_1;
  z_gtp_2_0_client.client_cap.pos_fix_capability.high_power_proc_wakeup_cap = GEOFENCE_CLIENT_HIGH_POWER_PROC_WAKEUP_PROFILE_0;  
  z_gtp_2_0_client.client_cap.pos_fix_capability.nw_usage_cap = GEOFENCE_CLIENT_NW_USAGE_PROFILE_0;
  
  z_gtp_2_0_client.client_cap.pos_fix_capability.is_imp_location_notification_required = FALSE;
  z_gtp_2_0_client.client_cap.pos_fix_capability.pos_fix_wait_secs = 20; /*Geofence core shall wait for 20 seconds for the fix*/
  z_gtp_2_0_client.client_cap.pos_fix_capability.is_prior_fix_request_indication_required = TRUE; /*Prior fix request indication required*/
  z_gtp_2_0_client.client_cap.pos_fix_capability.prior_fix_request_indication_secs = 2 * 60; /*Prior fix request indication required 2mins beforehand*/

  if(gm_client_register(&z_gtp_2_0_client,&pz_gfc_qmi_info->u_gm_gtp_2_0_handle,NULL,&error_code) == FAILED)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"Not able to add GTP2.0 as Geofence client, error_code %d", error_code,0,0 );
  }

}



/*===========================================================================

FUNCTION gfc_qmi_internal_tm_prtl_cb_init

DESCRIPTION
  Initializes function callback table for TM-Core.
  The GM core sends the geofence events to the TM core.
  The TM-Core uses these callbacks to send those events to GFC_QMI .

DEPENDENCIES
  None.

RETURN VALUE
  None.
SIDE EFFECTS
  None.

===========================================================================*/
void gfc_qmi_internal_tm_prtl_cb_init(void)
{
  if(NULL != pz_gfc_qmi_info)
  {
  /* Register the event handler for GM core related events for GFC_QMI */
    pz_gfc_qmi_info->z_prtlIfaceTbl.gm_event_handle_fp  = gfc_qmi_internal_gm_event_handler;
  }
}

/*
******************************************************************************
* tm_gfc_qmi_prtl_init
*
* Function description:
*
*   This function initialises the GFC_QMI module . This sets up the internal
*   states of the GFC_QMI and registers with GM core.
*
* Parameters: None.
*
* Return value: None.
*
******************************************************************************
*/

void tm_gfc_qmi_prtl_init(void)
{
  pz_gfc_qmi_info = (gfc_qmi_internal_info_s_type*)os_MemAlloc(sizeof(*pz_gfc_qmi_info),OS_MEM_SCOPE_TASK);

  if(NULL == pz_gfc_qmi_info)
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"No memory allocated for GFC info");
	return;
  }

  memset(pz_gfc_qmi_info, 0, sizeof(*pz_gfc_qmi_info));

  
  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"tm_gfc_qmi_prtl_init", 0, 0, 0);

  /* Initialize the function callbacks of GFC_QMI with TM Core. */
  gfc_qmi_internal_tm_prtl_cb_init();

  (void)tm_prtl_reg(TM_PRTL_TYPE_GFC_QMI, &pz_gfc_qmi_info->z_prtlIfaceTbl);

  /* Init with the GM core. */
  gfc_qmi_internal_gm_init();

}


/*
******************************************************************************
* tm_gfc_qmi_prtl_deinit
*
* Function description:
*
*   This function de-initialises the GFC_QMI module . This is used to free up the 
*   allocated memory and clean up the states
* Parameters: None.
*
* Return value: None.
*
******************************************************************************
*/

void tm_gfc_qmi_prtl_deinit(void)
{
  /*Free away the pointer to fix info*/
  os_MemFree((void**)&pz_gfc_qmi_info);
}
/*
******************************************************************************
* gfc_qmi_internal_set_engine_config_at_bootup
*
* Function description:
*
*   This function is used to set the geofence engine configuration
*   at boot up time , by reading the required configuration items from NV.
*
* Parameters: None.
*
* Return value: None.
*
******************************************************************************
*/
void gfc_qmi_internal_set_geofence_engine_config_at_bootup(void)
{
  
  geofence_engine_config_s_type *gm_config = NULL;

  sm_gm_set_geofence_engine_config_request_type z_gm_engine_config_req = {0};
  sm_gm_set_geofence_client_config_request_type z_gm_client_config_req = {0};
  uint8 u_error_code;

  tm_core_get_gm_engine_config(&gm_config);

  if(gm_config == NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"NULL GM Config pointer",0,0,0);
    return;
  }

  /***********Fill in the GM engine config parameters **********************/
  /* Set the GNSS unavailable timeout parameter. This parameter is read from the EFS NV by TM */
  
  z_gm_engine_config_req.q_mask |= SM_GM_ENGINE_CONFIG_GNSS_UNAVAIL_TIMEOUT_MASK;
  z_gm_engine_config_req.q_gnss_unavailable_indication_timeout = gm_config->sm_gm_gnss_unavail_ind_timeout;
  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"GFC_QMI: Setting GNSS unavail ind timeout as %d from NV",z_gm_engine_config_req.q_gnss_unavailable_indication_timeout,0,0);

  /* Set Sesnor usage for motion detection parameter. This parameter is read from the EFS NV by TM */
  z_gm_engine_config_req.q_mask |= SM_GM_ENGINE_CONFIG_ENABLE_SENSOR_MOTION_DETECTION_MASK;

#ifdef FEATURE_REF_TRACKER_LOC
  if (gm_config->sm_gm_motion_detection_sources & GM_MOTION_DETECTOR_SENSORS)
  {
    z_gm_engine_config_req.q_enable_sensor_motion_detection = TRUE;
  }
  else
  {
    z_gm_engine_config_req.q_enable_sensor_motion_detection = FALSE;
  }
#else
  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"GFC_QMI: Non-Sensor Build , disabling sensor motion detection",
        0,0,0);
  z_gm_engine_config_req.q_enable_sensor_motion_detection = FALSE;
#endif
  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"GFC_QMI: Setting GM Enable Sensors as %d ",
    z_gm_engine_config_req.q_enable_sensor_motion_detection,0,0);


  /* Set the CPI usage parameterr. This parameter is read from the EFS NV by TM */
  
  z_gm_engine_config_req.q_mask |= SM_GM_ENGINE_CONFIG_ENABLE_POSITION_INJECTION_MASK;

  if(gm_config->sm_gm_position_sources & GEOFENCE_POS_SOURCE_NV_CPI_MASK)
  {
    z_gm_engine_config_req.q_enable_position_injection  = TRUE;
  }
  else
  {
    z_gm_engine_config_req.q_enable_position_injection  = FALSE;
  }

  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"GFC_QMI: Setting GM Enable CPI as %d from NV",
    z_gm_engine_config_req.q_enable_position_injection,0,0);


  /* Set the GTP 2.0 usage parameterr. This parameter is read from the EFS NV by TM */
  
  z_gm_engine_config_req.q_mask |= SM_GM_ENGINE_CONFIG_ENABLE_GTP_2_0_MASK;

  if(gm_config->sm_gm_position_sources & GEOFENCE_POS_SOURCE_NV_GTP_2_0_MASK)
  {
    z_gm_engine_config_req.q_enable_gtp_2_0 = TRUE;
  }


  /* Set the value for max pos unc accepted by GM  */
  z_gm_engine_config_req.q_mask |= SM_GM_ENGINE_CONFIG_GNSS_MAX_POS_UNC_ACCEPTED_MASK;
  z_gm_engine_config_req.q_gnss_max_pos_unc_accepted = gm_config->sm_gm_gnss_max_pos_unc_accepted;
  
  
  /* Set the NV value for changing the CPI request rate by GM */
  z_gm_engine_config_req.q_mask |= SM_GM_ENGINE_CONFIG_POS_OPERATION_MODE_MASK;
  z_gm_engine_config_req.q_gm_pos_operation_mode = PDSM_LITE_SESSION_OPERATION_STANDALONE_ONLY;

  /* Set Comprehensive logging rate*/
  z_gm_engine_config_req.q_mask |= SM_GM_ENGINE_CONFIG_COMP_LOGGING_RATE_MASK;
  z_gm_engine_config_req.q_gm_comp_logging_rate = gm_config->q_gm_comp_logging_rate;

  z_gm_engine_config_req.q_mask |= SM_GM_ENGINE_CONFIG_ACCEPT_CELL_CPI_FIXES_MASK;
  z_gm_engine_config_req.v_accept_cell_cpi_fixes = gm_config->v_sm_gm_accept_cell_cpi_fixes;


  /* Get the transaction ID for geofence engine set config  operation */
  if ((z_gm_engine_config_req.q_transaction_id = gfc_qmi_getnext_gm_trxn_id(NON_QMI_TRXN_ID))
      == GFC_QMI_INVALID_TRXN_ID)
  {
    /* The mandatory parametrs are not specified , report error. */
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "gfc_qmi_internal_conv_set_gf_engine_config_req_to_gm Invalid transaction ID \n",
          0, 0, 0);
    return;
  }

  gfc_qmi_get_gm_client_handle(&z_gm_engine_config_req.client_handle);


  /***********Fill in the GM client config parameters **********************/
  z_gm_client_config_req.config_request.q_mask |= SM_GM_CLIENT_CONFIG_GNSS_POS_QOS_SESS_TIMEOUT_MASK;
  z_gm_client_config_req.config_request.q_gnss_pos_qos_sess_timeout = gm_config->sm_gm_gnss_pos_qos_sess_timeout;
  
  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"GFC_QMI: Setting GNSS Pos Qos Sess timeout as %d from NV",
    z_gm_client_config_req.config_request.q_gnss_pos_qos_sess_timeout,0,0);
  
  /* Set the NV value for changing the CPI request rate by GM */
  z_gm_client_config_req.config_request.q_mask |= SM_GM_CLIENT_CONFIG_CPI_REQUEST_RATE_MASK;
  z_gm_client_config_req.config_request.q_cpi_request_rate = gm_config->sm_gm_cpi_request_rate;
  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"GFC_QMI: Setting GM CPI request rate %d from NV",
    z_gm_client_config_req.config_request.q_cpi_request_rate,0,0);


  /* Set the Minimum backoff when in Challenging GPS Env parameter. This parameter is read from the EFS NV by TM */
  z_gm_client_config_req.config_request.q_mask |= SM_GM_CLIENT_CHAL_GPS_ENV_BO_MIN_MASK;
  z_gm_client_config_req.config_request.q_chal_gps_env_bo_min = gm_config->sm_gm_chal_gps_env_bo_config.q_backoff_min;


  /* Set maximum backoff when in Challenging GPS Env parameter. This parameter is read from the EFS NV by TM */
  z_gm_client_config_req.config_request.q_mask |= SM_GM_CLIENT_CHAL_GPS_ENV_BO_MAX_MASK;
  z_gm_client_config_req.config_request.q_chal_gps_env_bo_max = gm_config->sm_gm_chal_gps_env_bo_config.q_backoff_max;

  /* Set the motion sensing distance for GPS in case of challenging GPS env. This parameter is read from the EFS NV by TM */
  z_gm_client_config_req.config_request.q_mask |= SM_GM_CLIENT_CHAL_GPS_ENV_MS_DIST_GPS_MASK;
  z_gm_client_config_req.config_request.q_chal_gps_env_ms_dist_gps = gm_config->sm_gm_chal_gps_env_ms_dist_config.q_dist_used_for_gps;

  /* Set the motion sensing distance for GPS in case of challenging GPS env. This parameter is read from the EFS NV by TM */
  z_gm_client_config_req.config_request.q_mask |= SM_GM_CLIENT_CHAL_GPS_ENV_MS_DIST_CPI_MASK;
  z_gm_client_config_req.config_request.q_chal_gps_env_ms_dist_cpi = gm_config->sm_gm_chal_gps_env_ms_dist_config.q_dist_used_for_cpi;

 
  /* Set up the default maximum number of geofences for GFC QMI */
  z_gm_client_config_req.config_request.q_mask |= SM_GM_CLIENT_CONFIG_MAX_GEOFENCES_MASK;
  z_gm_client_config_req.config_request.q_max_geofences = GFC_QMI_GM_MAX_GF_DEFAULT;


  /* Get the transaction ID for geofence client config  operation */
  if ((z_gm_client_config_req.q_transaction_id = gfc_qmi_getnext_gm_trxn_id(NON_QMI_TRXN_ID))
      == GFC_QMI_INVALID_TRXN_ID)
  {
    /* The mandatory parametrs are not specified , report error. */
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "gfc_qmi_internal_conv_set_gf_engine_config_req_to_gm Invalid transaction ID \n",
          0, 0, 0);
    return;
  }
  gfc_qmi_get_gm_client_handle(&z_gm_client_config_req.config_request.client_handle);


  MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"GFC_QMI: Set GM engine config mask-0x%x GM TID-%d",
	                                    z_gm_engine_config_req.q_mask,
										z_gm_engine_config_req.q_transaction_id);

  MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"GFC_QMI: Set GM client config mask-0x%x GM TID-%d",
	                                     z_gm_client_config_req.config_request.q_mask,
					                     z_gm_client_config_req.q_transaction_id);

  /* Set the geofence engine config now */
  gm_set_geofence_engine_config(&z_gm_engine_config_req, &u_error_code);
  /* Set the geofence client config now */
  gm_set_geofence_client_config(&z_gm_client_config_req, &u_error_code);
 
}

/*
******************************************************************************
* tm_gfc_qmi_inject_ext_pos
*
* Function description:
*
*   This function handles Ext position injection
*
* Parameters: None.
*
* Return value: None.
*
******************************************************************************
*/
void tm_gfc_qmi_inject_ext_pos(sm_FixInfoStructType* p_fix_rpt)
{
  sm_gm_pos_fix_info *p_fix_info = NULL;

  if(p_fix_rpt == NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"Null Fix Rpt Ptr",0,0,0);
    return;
  }
  if(TRUE == p_fix_rpt->z_NavPos.z_SmPosFlags.b_IsCellId)
  {
    // its a GTP cell based fix 
    if(FALSE == p_fix_rpt->z_NavPos.z_SmPosFlags.b_IsWwanRanging)
    { 
      // Its a GTP 1.0 fix   
      gfc_qmi_internal_handle_gtp_1_0_fix(p_fix_rpt);
    }
    else
    {
      // Its a GTP 2.0 fix
      gfc_qmi_internal_handle_gtp_2_0_fix(p_fix_rpt);
    }
    return;
  }//if(TRUE == p_fix_rpt->z_NavPos.z_SmPosFlags.b_IsCellId)
  

  p_fix_info = (sm_gm_pos_fix_info*)os_MemAlloc(sizeof(sm_gm_pos_fix_info),OS_MEM_SCOPE_TASK);

  if(p_fix_info != NULL)
  {
    p_fix_info->fix_type = GEOFENCE_POS_FIX_TYPE_FINAL; /*Dont care*/
    p_fix_info->fix_src = GEOFENCE_POS_FIX_SRC_CPI;
    p_fix_info->pos_fix = *p_fix_rpt;

    gm_position_update(p_fix_info);

    /*Free away the pointer to fix info*/
    os_MemFree((void**)&p_fix_info);
  }

  //gfc_qmi_internal_stop_cpi_fix();

}

/*===========================================================================
gfc_qmi_get_gm_client_handle

Description:
   This function is used to get the GFC QMI client handle registered with GM

Parameters:
  None

Return value:
  Client handle

=============================================================================*/
boolean gfc_qmi_get_gm_client_handle(uint8 *pu_gm_handle)
{
  if((NULL == pu_gm_handle)||(NULL == pz_gfc_qmi_info))
  {
    return FALSE;
  }
  *pu_gm_handle = pz_gfc_qmi_info->u_gm_handle;
  return TRUE;
}

