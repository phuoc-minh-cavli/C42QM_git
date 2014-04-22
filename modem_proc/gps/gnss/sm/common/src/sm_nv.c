/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                      Session Manager NV Items Module

GENERAL DESCRIPTION
  This module contains the SM interface routines for the Non-Volatile Memory
  module.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2006, 2011-2012 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Copyright (c) 2013 - 2014 QUALCOMM Atheros, Inc.
All Rights Reserved.
QCA Proprietary and Confidential.
 
Copyright (c) 2015 - 2019 Qualcomm Technologies, Inc. 
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

Version Control

  $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/sm/common/src/sm_nv.c#1 $
  $DateTime: 2020/07/21 21:23:44 $
  $Author: pwbldsvc $

=============================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
07/24/19   py      Reinstating code to Pass SUB as param to nv_cmd_ext
08/04/15   js      Default Enablement of XTRA download 
03/14/15   rh      Added support for MultiSIM NV 
04/17/14   skm     NV Async Read changes 
06/11/10   gk      Enable XTRA and time injection by default
02/10/10   vp      OEM DR Data Enablement
06/10/09   gk      Added Get UTC offset from NV
01/15/09    atien  XTRA-T support
01/25/09   ns      Added support for QWiP NV item
08/22/08   jlp     Added support of emergency call simulation for simulating
                   E911 A-GPS
09/14/07    gk     NMEA CONFIG set parameters support
08/01/07    rw     Add support for XTRA feature
07/31/07    ank    Added Dynamic QoS and Override logic.
05/01/07    lt     Restore the default for cgps_mo_method back to MO_CP.
04/17/07    lt     Changed default for cgps_mo_method to MO_CP_LOC_EST.
04/12/07    ah     Added default read values for all SM-related NV items.
12/22/06    ank    Added default values for various NV items.
11/14/06    ah     Initial Release.

=============================================================================

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*
 * Include files
*/
#include "gps_variation.h"
#include "comdef.h"
//#include "customer.h"

#include "sm_nv.h"

#include "msg.h"
#include "pdapibuf.h"
#include "tm_api.h"
#include "gnss_common.h"
#include "aries_os_api.h"

/*
 * Constant definitions
*/
#define C_SM_NV_READ_SIGNAL   0x01

/*
 * Structure definitions
*/
typedef struct
{
  /*XTRA download interval*/
  uint16   gps1_xtra_download_interval;
  /*XTRA Auto Download Enabled*/
  boolean  gps1_xtra_auto_download_enabled;
  /* MO Method */
  uint8  cgps_mo_method;
 /* Rate of security updates sent to client */
  uint8 gps1_sec_update_rate; 
 /* Configures the On-Demand Positioning support */
  uint8 cgps_on_demand_enabled; 

/* Below are carrier-specific NVs (one copy per sub) */
  /* GPS Lock */
  dword   gps1_lock;

} sm_nv_cached_struct_type;

#ifdef FEATURE_GNSS_NO_NV_WRITES
#error code not present
#endif
static boolean           sm_nv_initialized = FALSE;
static nv_cmd_type       sm_nv_cmd;
static q_type            sm_nv_cmd_q;
static sm_nv_clnt_s_type sm_nv_clnt_tbl[SM_NV_CLIENT_HANDLE_MAX];

  /*===========================================================================
  
  FUNCTION sm_nv_load_default_values_to_cache
  
  DESCRIPTION
    This function is used to load default values to cache
    
  RETURN VALUE
    None
    
  DEPENDENCIES
   
  ===========================================================================*/
void sm_nv_load_default_values_to_cache(void)
{

#ifdef FEATURE_GNSS_NO_NV_WRITES
  #error code not present
#endif /*FEATURE_GNSS_NO_NV_WRITES*/   
}

/*===========================================================================

FUNCTION sm_nv_init

DESCRIPTION
  This function is used to initialize internal SM-NV data structure.
  
RETURN VALUE
  None
  
DEPENDENCIES
 
===========================================================================*/

void sm_nv_init (void)
{
  int i = 0;

  if (sm_nv_initialized == FALSE)
  {
    (void)q_init(&sm_nv_cmd_q);

    for (i = 0; i < SM_NV_CLIENT_HANDLE_MAX; i++)
    {
      sm_nv_clnt_tbl[i].active    = FALSE;
      sm_nv_clnt_tbl[i].thread_id = (uint32)THREAD_ID_ERROR;
      sm_nv_clnt_tbl[i].nv_write_cb_fn  = NULL;
      sm_nv_clnt_tbl[i].nv_read_cb_fn = NULL;
    }

    sm_nv_initialized = TRUE;
    sm_nv_load_default_values_to_cache();
  }
  else
  {
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SM_NV_INIT: already initialized");
  }
}

/*===========================================================================

FUNCTION sm_nv_rdrw_reg

DESCRIPTION
  This function is used to register a client that will use this NV module.
  It will register the client's callback function after an Async Read/NV Write
  command has completed, and return a client handle that is needed for future
  read/writes.
  
RETURN VALUE
  Client handle value (or 0xFF if error)

DEPENDENCIES
 
===========================================================================*/

uint32 sm_nv_rdrw_reg (uint32 thread_id, sm_nv_cmd_cb_f_type nv_clnt_write_cb_fn,
                       sm_nv_cmd_cb_f_type nv_clnt_read_cb_fn)
{
  uint32 i = 0;

  /* SM NV Module Init sanity check */
  if (sm_nv_initialized == FALSE)
  {
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "SM_NV_INIT: Initializing SM NV module");
    sm_nv_init();
  }

  /* Passed argument sanity check */
  if ((os_ThreadIdEnumType)thread_id >= THREAD_ID_MAX)
  {
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SM_NV_INIT: FAILED - Unknown parameter");
    return (uint32)SM_NV_CLIENT_HANDLE_ERROR;
  }

  /* Passed argument sanity check */
  if ((nv_clnt_write_cb_fn == NULL) || (nv_clnt_read_cb_fn == NULL))
  {
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SM_NV_INIT: FAILED - NULL parameter");
    return (uint32)SM_NV_CLIENT_HANDLE_ERROR;
  }

  /* Look for room in the client table to register NV write cmd callback */
  for (i = 0; i < SM_NV_CLIENT_HANDLE_MAX; i++)
  {
    if (sm_nv_clnt_tbl[i].active == TRUE)
    {
      if (sm_nv_clnt_tbl[i].thread_id == thread_id)
      {
        MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SM_NV_INIT: already initialized");
        return i;
      }
    }
    else /* sm_nv_clnt_tbl[i].active == FALSE */
    {
      sm_nv_clnt_tbl[i].active    = TRUE;
      sm_nv_clnt_tbl[i].thread_id = thread_id;
      sm_nv_clnt_tbl[i].nv_write_cb_fn  = nv_clnt_write_cb_fn;
      sm_nv_clnt_tbl[i].nv_read_cb_fn = nv_clnt_read_cb_fn;
      return i;
    }
  }

  /* Can't initialize since maximum number of clients initialized */
  MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SM_NV_INIT: FAILED - max # of clients reached");
  return (uint32)SM_NV_CLIENT_HANDLE_ERROR;
}

/*===========================================================================

FUNCTION sm_nv_write_to_cache_sub

DESCRIPTION
  This function is used to write NV to cache for a subscription

RETURN VALUE
  TRUE  - NV Cache write command successful
  FALSE - NV Cache write command unsuccessful

DEPENDENCIES

===========================================================================*/
static boolean sm_nv_write_to_cache_sub(nv_items_enum_type nv_item, void* item_ptr)
{
#ifdef FEATURE_GNSS_NO_NV_WRITES
  #error code not present
#endif
  
  /* Passed argument sanity check */
  if (item_ptr == NULL)
  {
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SM NV Write TO Cache: FAILED - NULL parameter");
    return FALSE;
  }
  

#ifdef FEATURE_GNSS_NO_NV_WRITES
   #error code not present
#else /*FEATURE_GNSS_NO_NV_WRITES*/
  return FALSE;
#endif /*FEATURE_GNSS_NO_NV_WRITES*/
  return TRUE;
}


/*===========================================================================

FUNCTION sm_nv_write_to_cache

DESCRIPTION
  This function is used to write NV to cache for current DD sub

RETURN VALUE
  TRUE  - NV Cache write command successful
  FALSE - NV Cache write command unsuccessful

DEPENDENCIES

===========================================================================*/
static boolean sm_nv_write_to_cache(nv_items_enum_type nv_item, void* item_ptr)
{
  return sm_nv_write_to_cache_sub(nv_item, item_ptr);
}

/*===========================================================================

FUNCTION sm_nv_write_to_hw

DESCRIPTION
  This function is used to write NV to Hw (for sub-independent NV)

RETURN VALUE
  TRUE  - NV Hw write command successfully sent
  FALSE - NV Hw write command unsuccessful

DEPENDENCIES

===========================================================================*/
static boolean sm_nv_write_to_hw(nv_items_enum_type nv_item, void* item_ptr)
{


  #ifndef FEATURE_GNSS_LEAN_BUILD

  /* Passed argument sanity check */
  if (item_ptr == NULL)
  {
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SM NV Write TO Hw: FAILED - NULL parameter");
    return FALSE;
  }

  /* Populate NV write command */
  sm_nv_cmd.item       = nv_item;
  sm_nv_cmd.cmd        = NV_WRITE_F;
  sm_nv_cmd.data_ptr   = (nv_item_type*)item_ptr;
  sm_nv_cmd.tcb_ptr    = rex_self();
  sm_nv_cmd.sigs       = C_OS_FLAG_NV_WRITE;
  sm_nv_cmd.done_q_ptr = NULL;

  
  MSG_1( MSG_SSID_GPSSM, MSG_LEGACY_MED, "SM NV Hw Write: %d", nv_item);
  nv_cmd(&sm_nv_cmd);

  return TRUE;


  #else
  return FALSE;
  #endif
}


/*===========================================================================

FUNCTION sm_nv_write_to_hw_sub

DESCRIPTION
  This function is used to write NV to Hw for a specific sub

RETURN VALUE
  TRUE  - NV Hw write command successfully sent
  FALSE - NV Hw write command unsuccessful

DEPENDENCIES

===========================================================================*/
static boolean sm_nv_write_to_hw_sub(nv_items_enum_type nv_item, void* item_ptr, sys_modem_as_id_e_type sub)
{
  /* Extended NV command struct for dual sim */
  nv_cmd_ext_type nvcmd_ext;

  /* Passed argument sanity check */
  if (item_ptr == NULL)
  {
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SM NV Write TO Hw sub: FAILED - NULL parameter");
    return FALSE;
  }


  /* Populate NV write command */
  sm_nv_cmd.item       = nv_item;
  sm_nv_cmd.cmd        = NV_WRITE_F;
  sm_nv_cmd.data_ptr   = (nv_item_type*)item_ptr;
  sm_nv_cmd.tcb_ptr    = rex_self();
  sm_nv_cmd.sigs       = C_OS_FLAG_NV_WRITE;
  sm_nv_cmd.done_q_ptr = NULL;

  nvcmd_ext.nvcmd   = &sm_nv_cmd;

  MSG_2( MSG_SSID_GPSSM, MSG_LEGACY_MED, "SM NV Hw Write: %d (sub=%d)", nv_item, 0);
  nv_cmd_ext(&nvcmd_ext);

  return TRUE;
}


/*===========================================================================

FUNCTION sm_nv_async_read_hw

DESCRIPTION
  This function is used for Async NV Read's from Hw 

RETURN VALUE
  TRUE  - NV Hw write command successfully sent
  FALSE - NV Hw write command unsuccessful

DEPENDENCIES

===========================================================================*/
boolean sm_nv_async_read_hw(nv_items_enum_type nv_item, void* item_ptr)
{


  #ifndef FEATURE_GNSS_LEAN_BUILD

  /* Passed argument sanity check */
  if (item_ptr == NULL)
  {
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SM NV async read Hw: FAILED - NULL parameter");
    return FALSE;
  }

  /* Populate NV READ command */
  sm_nv_cmd.item       = nv_item;
  sm_nv_cmd.cmd        = NV_READ_F;
  sm_nv_cmd.data_ptr   = (nv_item_type*)item_ptr;
  sm_nv_cmd.tcb_ptr    = rex_self();
  sm_nv_cmd.sigs       = C_OS_FLAG_NV_READ;
  sm_nv_cmd.done_q_ptr = NULL;

  
  MSG_1( MSG_SSID_GPSSM, MSG_LEGACY_MED, "SM NV Async hw read: %d", nv_item);
  nv_cmd(&sm_nv_cmd);

  return TRUE;


  #else
  return FALSE;
  #endif
}

/*===========================================================================

FUNCTION sm_nv_handle_rdwr_cmd

DESCRIPTION
  This function is used to handle a NV HW Async-Read/Write command for NV.
  Function services the request at the head of the NV Read/Write command queue
  and waits until NV returns with a response.Function checks the CMD type
  "nv-ptr->cmd" to call appropriate Asyns-read/Write function.
  
RETURN VALUE
  None

DEPENDENCIES
  SM NV async-read/write callback needs to be registered via call to
  sm_nv_write_reg/sm_nv_async_read_reg

===========================================================================*/

static void sm_nv_handle_rdwr_cmd (void)
{
  sm_nv_item_s_type* nv_ptr;
#ifndef FEATURE_GNSS_NO_NV_WRITES  
  boolean nv_hw_write_needed = FALSE;
#endif /* ! FEATURE_GNSS_NO_NV_WRITES */

  if (sm_nv_initialized)
  {
    nv_ptr = (sm_nv_item_s_type*)q_check(&sm_nv_cmd_q);

    if ( NULL == nv_ptr )
    {
      MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"sm_nv_handle_rdwr_cmd NULL Ptr");
      return;
    }

    if ( nv_ptr->cmd == NV_READ_F ) 
    {
       /* Send NV Aync Read command to NV module
          Function used for reading RUIM NV's*/
       sm_nv_async_read_hw(nv_ptr->item, (nv_item_type*)&(nv_ptr->data));
               
    }
    /* Send NV write command to NV module only if a request exists in queue */
    else if ( nv_ptr->cmd == NV_WRITE_F )
    {      
      sm_nv_cmd.item = nv_ptr->item;
#ifdef FEATURE_GNSS_NO_NV_WRITES
      #error code not present
#else
      nv_hw_write_needed = TRUE;
      if(sm_nv_write_to_cache_sub(nv_ptr->item, 
                                  (nv_item_type*)&(nv_ptr->data)))
      {
        MSG_2( MSG_SSID_GPSSM, MSG_LEGACY_MED, "SM NV Cache Write: %d (sub=%d)", nv_ptr->item, 0); 
        nv_hw_write_needed = FALSE;
      }

      if(nv_hw_write_needed)
      {
        /* Send NV write command to NV module */
          sm_nv_write_to_hw(nv_ptr->item, (nv_item_type*)&(nv_ptr->data));        
        
        MSG_2( MSG_SSID_GPSSM, MSG_LEGACY_MED, "SM NV Hw Write: %d (sub=%d)", nv_ptr->item, 0);
      }
      else
      {
        /* If the write was sent to the NV module, once the data was written to 
          * hw, rex signal 'C_OS_FLAG_NV_WRITE' would be sent to tm task, which 
          * is handled in sm_nv_write_done(). As the data is not flushed to hw, 
          * at this point, the NV write is done. Simply call sm_nv_write_done()
          * to invoke the client cb */
      
        sm_nv_cmd.status = NV_DONE_S;
        sm_nv_rdrw_done();   
      }
#endif
    }
    else
    {
       MSG_1( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SM_NV_HANDLE_WR_CMD: INVALID cmd : %d",nv_ptr->cmd);
    }
  }
  else
  {
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SM_NV_HANDLE_WR_CMD: SM NV not initialized");
  }
}

/*===========================================================================

FUNCTION sm_nv_write

DESCRIPTION
  This function is used to queue NV Write commands to NV. The writes to NV
  are performed in asynchronous fashion.

RETURN VALUE
  TRUE  - NV write command successfully sent to NV
  FALSE - NV write command failed to be sent to NV

DEPENDENCIES
  SM NV write callback needs to be registered via call to sm_nv_write_reg

===========================================================================*/

boolean sm_nv_write (nv_items_enum_type nv_item, const nv_item_type* item_ptr, uint32 handle)
{
  return sm_nv_write_to_sub(nv_item, item_ptr, handle);
}

/*===========================================================================

FUNCTION sm_nv_read_default

DESCRIPTION
  This function is used to return the default value for the NV item to be
  read. This should be used only when failing to read from NV.

RETURN VALUE
  None

DEPENDENCIES
 
===========================================================================*/

void sm_nv_read_default (nv_items_enum_type nv_item, void *item_ptr)
{
  if (item_ptr == NULL)
  {
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SM_NV_READ_Default - NULL parameter");
    return;
  }

  MSG_1( MSG_SSID_GPSSM, MSG_LEGACY_MED, "SM_NV_READ: Default NV item(%d) returned", nv_item);

  switch (nv_item)
  {
    /*******************************************/
    /* Transport Manager (TM) Related NV Items */
    /*******************************************/

    case NV_GPS1_LOCK_I:
      #ifdef FEATURE_GNSS_LOCK_AT_BOOTUP
      /* Default value: GPS Lock = LOCK_ALL if the feature is defined */
      ((nv_item_type*)item_ptr)->gps1_lock = (dword)PDSM_GPS_LOCK_ALL;
      #else
      /* Default value: GPS Lock = LOCK_NONE if the feature is not defined */
      ((nv_item_type*)item_ptr)->gps1_lock = (dword)PDSM_GPS_LOCK_NONE;
      #endif
      MSG_1( MSG_SSID_GPSSM, MSG_LEGACY_HIGH ,"Inside default NV_GPS1_LOCK_I, Value: %d", ((nv_item_type*)item_ptr)->gps1_lock);
      break;
      
    /**************************************/
    /* Link Manager (LM) Related NV Items */
    /**************************************/

    case NV_CGPS_QOS_OVERRIDE_ACC_THRESHOLD_I:
      /* Default value: QoS Override Accuracy Threshold [meters] = 0 */
      ((nv_item_type*)item_ptr)->cgps_qos_override_acc_threshold = 0; //uint16
      break;

    case NV_CGPS_QOS_OVERRIDE_TIME_I:
      /* Default value: QoS Override Time [seconds] = 0 */
      ((nv_item_type*)item_ptr)->cgps_qos_override_time = 0; //uint8
      break;

    /*************************************************************************/
    /* Protocol Submodule: UMTS Related NV Item */
    /*************************************************************************/

     
    /******************************************************/
    /* Transceiver Resource Manager (TRM) Related NV Item */
    /******************************************************/

    case NV_TRM_CONFIG_I:
      /* Default value:  Limit RF assignments for simulating reduced h/w capability (nv_trm_config_type) */
      /* Limit RF assignments for simulating reduced h/w capability */
      ((nv_item_type*)item_ptr)->trm_config.rf_config = NV_TRM_RF_DUAL_RX; //nv_trm_rf_enum
      /* Bit field for disallowing certain mode combinations */
      ((nv_item_type*)item_ptr)->trm_config.mask = 0; //uint32
      break;

    /*******************************************************/
    /* Extended Receiver Assistance (XTRA) Related NV Item */
    /*******************************************************/     
       
    case NV_GPS1_XTRA_ENABLED_I:                 
      ((nv_item_type*)item_ptr)->gps1_xtra_enabled = GNSS_NV_DEFAULT_XTRA_ENABLED;
      break;    
      
    case NV_GPS1_XTRA_DOWNLOAD_INTERVAL_I:
      ((nv_item_type*)item_ptr)->gps1_xtra_download_interval = NV_AAGPS_DEFAULT_XTRA_DOWNLOAD_INTERVAL;
      break;    
      
    case NV_GPS1_XTRA_NUM_DOWNLOAD_ATTEMPTS_I:   
      ((nv_item_type*)item_ptr)->gps1_xtra_num_download_attempts = NV_AAGPS_DEFAULT_XTRA_NUM_DOWNLOAD_ATTEMPTS;
      break;    
      
    case NV_GPS1_XTRA_TIME_BETWEEN_ATTEMPTS_I:    
      ((nv_item_type*)item_ptr)->gps1_xtra_time_between_attempts = NV_AAGPS_DEFAULT_XTRA_TIME_BETWEEN_ATTEMPTS;
      break;    
      
    case NV_GPS1_XTRA_AUTO_DOWNLOAD_ENABLED_I:    
      ((nv_item_type*)item_ptr)->gps1_xtra_auto_download_enabled = GNSS_NV_DEFAULT_XTRA_AUTO_DL_DISABLED;
      break;    
      
    case NV_GPS1_XTRA_PRIMARY_SERVER_URL_I:       
      (void) GNSS_STRLCPY((char *) &(((nv_item_type *)item_ptr)->gps1_xtra_primary_server_url[0]), 
                  (const char *) NV_AAGPS_DEFAULT_XTRA_PRIMARY_SERVER_URL, 
                  sizeof(((nv_item_type *)item_ptr)->gps1_xtra_primary_server_url));
      break;    
      
    case NV_GPS1_XTRA_SECONDARY_SERVER_URL_I:     
      (void) GNSS_STRLCPY((char *) &(((nv_item_type *)item_ptr)->gps1_xtra_secondary_server_url[0]), 
                  (const char *) NV_AAGPS_DEFAULT_XTRA_SECONDARY_SERVER_URL, 
                  sizeof(((nv_item_type *)item_ptr)->gps1_xtra_secondary_server_url));
      break;    
      
    case NV_GPS1_XTRA_TERTIARY_SERVER_URL_I:      
      (void) GNSS_STRLCPY((char *) &(((nv_item_type *)item_ptr)->gps1_xtra_tertiary_server_url[0]), 
                  (const char *) NV_AAGPS_DEFAULT_XTRA_TERTIARY_SERVER_URL, 
                  sizeof(((nv_item_type *)item_ptr)->gps1_xtra_tertiary_server_url));
      break;    

    /* XTRA time injection NV items */
    case NV_GPS1_XTRA_TIME_INFO_ENABLED_I:
      ((nv_item_type*)item_ptr)->gps1_xtra_time_info_enabled  = NV_AAGPS_DEFAULT_XTRA_TIME_INFO_ENABLED;
      break;
  
    case NV_GPS1_XTRA_TIME_INFO_UNC_THRESH_I:
      ((nv_item_type*)item_ptr)->gps1_xtra_time_info_unc_thresh  = NV_AAGPS_DEFAULT_XTRA_TIME_INFO_UNC_THRESH;
      break;
  
    case NV_GPS1_XTRA_TIME_INFO_DELAY_THRESH_I:      
      ((nv_item_type*)item_ptr)->gps1_xtra_time_info_delay_thresh = NV_AAGPS_DEFAULT_XTRA_TIME_INFO_DELAY_THRESH;
      break;
  
    case NV_GPS1_XTRA_PRIMARY_SNTP_SERVER_URL_I:
      (void) GNSS_STRLCPY((char *) &(((nv_item_type *)item_ptr)->gps1_xtra_primary_sntp_server_url[0]), 
                  (const char *) NV_AAGPS_DEFAULT_XTRA_PRIMARY_SNTP_SERVER_URL, 
                  sizeof(((nv_item_type *)item_ptr)->gps1_xtra_primary_sntp_server_url));
      break;    
  
    case NV_GPS1_XTRA_SECONDARY_SNTP_SERVER_URL_I:
      (void) GNSS_STRLCPY((char *) &(((nv_item_type *)item_ptr)->gps1_xtra_secondary_sntp_server_url[0]), 
                  (const char *) NV_AAGPS_DEFAULT_XTRA_SECONDARY_SNTP_SERVER_URL, 
                  sizeof(((nv_item_type *)item_ptr)->gps1_xtra_secondary_sntp_server_url));
      break;    
  
    case NV_GPS1_XTRA_TERTIARY_SNTP_SERVER_URL_I:
      (void) GNSS_STRLCPY((char *) &(((nv_item_type *)item_ptr)->gps1_xtra_tertiary_sntp_server_url[0]), 
                  (const char *) NV_AAGPS_DEFAULT_XTRA_TERTIARY_SNTP_SERVER_URL, 
                  sizeof((nv_item_type *)item_ptr)->gps1_xtra_tertiary_sntp_server_url);
      break;

    case NV_GPS1_DYNAMIC_MODE_I:
      /* default is to enable data demod */
      ((nv_item_type*)item_ptr)->gps1_dynamic_mode = 1;
      break;
      

    case NV_CGPS_ON_DEMAND_ENABLED_I:
      /* default value should be enabled. for now disable it */
      ((nv_item_type*)item_ptr)->cgps_on_demand_enabled = FALSE;
      break;
     /* QWiP related NV items*/
    case NV_CGPS_XTRA_T_CONTROL_I:
      /* default should be disabled. */
      ((nv_item_type*)item_ptr)->cgps_xtra_t_control = FALSE;
      break;

    default:
       break;
  }
}


/*===========================================================================

FUNCTION sm_nv_read_from_cache_sub

DESCRIPTION
  This function is used to perform a read from cached value for a SUB

RETURN VALUE
  TRUE  - NV read success
  FALSE - NV read failure (read default value instead)

DEPENDENCIES
 
===========================================================================*/
static boolean sm_nv_read_from_cache_sub(nv_items_enum_type nv_item, void* item_ptr)
{
#ifdef FEATURE_GNSS_NO_NV_WRITES
  #error code not present
#endif

  /* Passed argument sanity check */
  if (item_ptr == NULL)
  {
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"SM_NV_READ_from_cache_sub: FAILED - NULL parameter");
    return FALSE;
  }


#ifdef FEATURE_GNSS_NO_NV_WRITES
    #error code not present
#else
    return FALSE;
#endif /*FEATURE_GNSS_NO_NV_WRITES*/

  return TRUE;
}


/*===========================================================================

FUNCTION sm_nv_read_from_cache

DESCRIPTION
  This function is used to perform a read from cached value from current DD sub

RETURN VALUE
  TRUE  - NV read success
  FALSE - NV read failure (read default value instead)

DEPENDENCIES
 
===========================================================================*/
static boolean sm_nv_read_from_cache(nv_items_enum_type nv_item, void* item_ptr)
{
  if (item_ptr == NULL)
  {
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SM_NV_READ_Cache - NULL parameter");
    return FALSE;
  }

  return sm_nv_read_from_cache_sub(nv_item, item_ptr);
}

/*===========================================================================

FUNCTION sm_nv_read_from_hw

DESCRIPTION
  This function is used to perform a synchronous read from NV (sub-independent)

RETURN VALUE
  TRUE  - NV read success
  FALSE - NV read failure (read default value instead)

DEPENDENCIES
 
===========================================================================*/
static boolean sm_nv_read_from_hw(nv_items_enum_type nv_item, void* item_ptr)
{


  #ifndef FEATURE_GNSS_LEAN_BUILD

  nv_cmd_type* cmd_ptr = NULL;
  rex_sigs_type sigs = 0;

  /* Passed argument sanity check */
  if (NULL == item_ptr)
  {
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SM_NV_READ_HW - NULL parameter");
    return FALSE;
  }

  /* Since NV read and writes should normally be done in GPS idle state
   * (no sessions), no separate allocation of space is memory is done.
   * The buffers will be used from a dynamic buffer pool.
   */
  cmd_ptr = (nv_cmd_type*)pdsm_getbuf(sizeof(nv_cmd_type));

  /* Dynamic buffer allocation sanity check */
  if (NULL == cmd_ptr)
  {
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SM_NV_READ failed: no buffer allocated");
    sm_nv_read_default(nv_item, item_ptr);
    return FALSE;
  }

  /* Populate NV read command */
  cmd_ptr->item       = nv_item;
  cmd_ptr->cmd        = NV_READ_F;
  cmd_ptr->data_ptr   = (nv_item_type*)item_ptr;
  cmd_ptr->tcb_ptr    = os_Self();
  cmd_ptr->sigs       = (uint32) C_SM_NV_READ_SIGNAL;
  cmd_ptr->done_q_ptr = NULL;
  
  /* Clear NV Read signal before waiting on it */
  (void) os_ClrLocalSignal( C_SM_NV_READ_SIGNAL );
  
  /* Send NV read command to NV module */
  nv_cmd(cmd_ptr);
    
  /* Block until NV Read signal is set
     When in block state, respont to dog heartbeat*/  
  while ((sigs & C_SM_NV_READ_SIGNAL)==0)
  {
     sigs =  rex_wait( C_SM_NV_READ_SIGNAL | C_OS_FLAG_DOG_HB );
     /*If the SIG Set when Wait returns is not SM_NV_READ. 
       It must be DOG Heart beat, Send Heartbeat report.
       to ensure DOG timeout does not occur*/
     if ((sigs & C_SM_NV_READ_SIGNAL)==0)
     {
        os_DogHeartbeatReport();
        MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SM_NV_READ_HW: DOG PAT");
     }
  }
  /* Clear NV Read signal after it is read out */
  (void) os_ClrLocalSignal( C_SM_NV_READ_SIGNAL );

    /* Check status of NV Read */
  if (cmd_ptr->status != NV_DONE_S)
  {
    /* NV read failed. Return default value for NV item. */
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SM_NV_READ: FAILED - Unknown Status");
    sm_nv_read_default(nv_item, item_ptr);

    (void)pdsm_freebuf((char*)cmd_ptr);
    return FALSE;
  }

  (void)pdsm_freebuf((char*)cmd_ptr);
  return TRUE;

  #else
  return FALSE;
  #endif
}

/*===========================================================================

FUNCTION sm_nv_read_from_hw_sub

DESCRIPTION
  This function is used to perform a synchronous read from NV (for a
  specific subscription)

RETURN VALUE
  TRUE  - NV read success
  FALSE - NV read failure (read default value instead)

DEPENDENCIES
 
===========================================================================*/
static boolean sm_nv_read_from_hw_sub(nv_items_enum_type nv_item, void* item_ptr, sys_modem_as_id_e_type sub)
{

#ifndef FEATURE_GNSS_LEAN_BUILD
  nv_cmd_type* cmd_ptr = NULL;
  /* Extended NV command struct for dual sim */
  nv_cmd_ext_type nvcmd_ext;

  rex_sigs_type sigs = 0;

  /* Passed argument sanity check */
  if (NULL == item_ptr)
  {
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SM_NV_READ_HW_Sub failed: NULL parameter");
    return FALSE;
  }

  /* Since NV read and writes should normally be done in GPS idle state
   * (no sessions), no separate allocation of space is memory is done.
   * The buffers will be used from a dynamic buffer pool.
   */
  cmd_ptr = (nv_cmd_type*)pdsm_getbuf(sizeof(nv_cmd_type));

  /* Dynamic buffer allocation sanity check */
  if (NULL == cmd_ptr)
  {
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SM_NV_READ failed: no buffer allocated");
    sm_nv_read_default(nv_item, item_ptr);
    return FALSE;
  }

  /* Populate NV read command */
  cmd_ptr->item       = nv_item;
  cmd_ptr->cmd        = NV_READ_F;
  cmd_ptr->data_ptr   = (nv_item_type*)item_ptr;
  cmd_ptr->tcb_ptr    = os_Self();
  cmd_ptr->sigs       = (uint32) C_SM_NV_READ_SIGNAL;
  cmd_ptr->done_q_ptr = NULL;
  nvcmd_ext.nvcmd   = cmd_ptr;
  nvcmd_ext.context = (uint16)sub;


  /* Clear NV Read signal before waiting on it */
  (void) os_ClrLocalSignal( C_SM_NV_READ_SIGNAL );
  
  /* Send NV read command to NV module */
  nv_cmd_ext(&nvcmd_ext);
    
  /* Block until NV Read signal is set
     When in block state, respont to dog heartbeat*/  
  while ((sigs & C_SM_NV_READ_SIGNAL)==0)
  {
     sigs =  rex_wait( C_SM_NV_READ_SIGNAL | C_OS_FLAG_DOG_HB );
     /*If the SIG Set when Wait returns is not SM_NV_READ. 
       It must be DOG Heart beat, Send Heartbeat report.
       to ensure DOG timeout does not occur*/
     if ((sigs & C_SM_NV_READ_SIGNAL)==0)
     {
        os_DogHeartbeatReport();
        MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SM_NV_READ_HW: DOG PAT");
     }
  }
  /* Clear NV Read signal after it is read out */
  (void) os_ClrLocalSignal( C_SM_NV_READ_SIGNAL );

    /* Check status of NV Read */
  if (cmd_ptr->status != NV_DONE_S)
  {
    /* NV read failed. Return default value for NV item. */
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SM_NV_READ: FAILED - Unknown Status");
    sm_nv_read_default(nv_item, item_ptr);

    (void)pdsm_freebuf((char*)cmd_ptr);
    return FALSE;
  }

  (void)pdsm_freebuf((char*)cmd_ptr);
  return TRUE;
#else
  return FALSE;
#endif
}


/*===========================================================================

FUNCTION sm_nv_read

DESCRIPTION
  This function is used to perform a synchronous read from NV.
  GPS SHALL CALL THIS ONLY DURING BOOTUP INITIALIZATION

RETURN VALUE
  TRUE  - NV read success
  FALSE - NV read failure (read default value instead)

DEPENDENCIES
 
===========================================================================*/
boolean sm_nv_read (nv_items_enum_type nv_item, void* item_ptr)
{
  /* Since NV read and writes should normally be done in GPS idle state
   * (no sessions), no separate allocation of space is memory is done.
   * The buffers will be used from a dynamic buffer pool.
   */
  boolean ret_val = FALSE;

  /* Passed argument sanity check */
  if (item_ptr == NULL)
  {
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SM_NV_READ: FAILED - NULL parameter");
    return FALSE;
  }
    /* sub-indepdent NV or Sub1 NV */
    ret_val = sm_nv_read_from_hw(nv_item, item_ptr);
  

  MSG_3( MSG_SSID_GPSSM, MSG_LEGACY_MED, "SM NV Hw Read: %d (sub=%d) ret=%d", nv_item, 0, ret_val);

  (void)sm_nv_write_to_cache_sub(nv_item, item_ptr);

  return ret_val;
}

/*===========================================================================

FUNCTION sm_nv_read_ex

DESCRIPTION
  This function is used to perform a read from cache. And if cached value not present, read from Hw.
  GPS SHALL CALL THIS API INSTEAD OF sm_nv_read AFTER BOOT UP INITIALIZATION

RETURN VALUE
  TRUE  - NV read success
  FALSE - NV read failure (read default value instead)

DEPENDENCIES
 
===========================================================================*/
boolean sm_nv_read_ex (nv_items_enum_type nv_item, void* item_ptr)
{
  /* Since NV read and writes should normally be done in GPS idle state
   * (no sessions), no separate allocation of space is memory is done.
   * The buffers will be used from a dynamic buffer pool.
   */
  boolean ret_val = FALSE;

  /* Passed argument sanity check */
  if (item_ptr == NULL)
  {
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SM_NV_READ: FAILED - NULL parameter");
    return FALSE;
  }
      /* sub-indepdent NV */
      ret_val = sm_nv_read_from_hw(nv_item, item_ptr);
    
      
    MSG_3( MSG_SSID_GPSSM, MSG_LEGACY_MED, "SM NV Hw Read: %d (sub=%d) ret=%d", nv_item, 0, ret_val);

  return ret_val;
}


/*===========================================================================

FUNCTION   sm_nv_rdrw_done

DESCRIPTION
  This function is used to handle NV HW read/write callback status. If there are any
  clients who issued read/write requests, they will be notified and any pending
  read/write requests will be subsequently handled.

RETURN VALUE
  TRUE  - NV write success
  FALSE - NV write failure

DEPENDENCIES
 
===========================================================================*/

boolean sm_nv_rdrw_done (void)
{
  boolean v_retval = TRUE;

  /* First dequeue the async-read/write request at the head */
  sm_nv_item_s_type* nv_ptr = ((sm_nv_item_s_type*)q_get(&sm_nv_cmd_q));

  if (nv_ptr == NULL)
  {
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SM_NV_RDRW: No Read/Write requested");
    v_retval = FALSE;
  }
  else
  {
    /* Make sure first dequeued element is same as current NV async-read/write element */
    if (sm_nv_cmd.item != nv_ptr->item)
    {
      MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SM_NV_RDRW: FAILED - Non-matching item");
      (void)pdsm_freebuf((char*)nv_ptr);
      v_retval = FALSE;
    }
    else
    {
      /* Check NV command result status */
      if (sm_nv_cmd.status != NV_DONE_S)
      {
        /* NV read/write failed. */
        MSG_2( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SM_NV_RDRW: FAILED - Error Status %d : %d ", sm_nv_cmd.status, sm_nv_cmd.item);
        (void)pdsm_freebuf((char*)nv_ptr);
        v_retval = FALSE;
      }

      /* Out-of-bound array index sanity check */
      else if (nv_ptr->handle >= SM_NV_CLIENT_HANDLE_MAX)
      {
        MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SM_NV_RDRW: FAILED - bad parameter %d",
          nv_ptr->handle);
        (void)pdsm_freebuf((char*)nv_ptr);
        v_retval = FALSE;
      }
      else
      {
         /* Call appropriate client's registered callback function.
            async-read/write function callback's are differentiated
            based on "nv_ptr->handle", currently in tm_core
            async-read = tm_nv_async_read_handle
            write      = tm_nv_write_handle*/
         if (nv_ptr->cmd == NV_WRITE_F) 
         {
            sm_nv_clnt_tbl[nv_ptr->handle].nv_write_cb_fn(nv_ptr->item, (void*)&nv_ptr->data, sm_nv_cmd.status);
         }
         else if (nv_ptr->cmd == NV_READ_F) 
         {
            /*Update cache Async NV read successful*/
            (void)sm_nv_write_to_cache(nv_ptr->item, &(nv_ptr->data));
            sm_nv_clnt_tbl[nv_ptr->handle].nv_read_cb_fn(nv_ptr->item, (void*)&nv_ptr->data, sm_nv_cmd.status);
         }
         else
         {
            MSG_1( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SM_NV_RDRW: FAILED - CMD not supported: %d",(int)nv_ptr->cmd);
         }

         /*Free the nv_ptr buffer*/
         (void)pdsm_freebuf((char*)nv_ptr);
      }
    }

    /*Handle next read/write command*/
	  //CR2000925 - CASE02787833
    if (q_cnt(&sm_nv_cmd_q) != 0)
    {
	    /* If NV Read/Write command queue has items in queue item, handle it*/
      MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SM_NV_RDRW: SS_DEBUG : dequeue/process next NV");
      sm_nv_handle_rdwr_cmd();
    }
  }

  return ( v_retval );
}

/*===========================================================================

FUNCTION sm_nv_read_async

DESCRIPTION
  This function is used to queue NV Async Read commands to NV. The READ's to NV
  are performed in asynchronous fashion.

RETURN VALUE
  TRUE  - NV read command successfully sent to NV
  FALSE - NV read command failed to be sent to NV

DEPENDENCIES
  SM NV read callback needs to be registered via call to sm_nv_read_async_reg

===========================================================================*/

boolean sm_nv_async_read (nv_items_enum_type nv_item, const nv_item_type* item_ptr, uint32 handle)
{
  sm_nv_item_s_type* nv_ptr = NULL;

  /* Passed argument sanity check */
  if ((NULL == item_ptr) || (handle >= SM_NV_CLIENT_HANDLE_MAX))
  {
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SM_NV_READ_ASYNC failed: bad parameters");
    return FALSE;
  }

  if (sm_nv_initialized)
  {
    /* Since NV read and writes should normally be done in GPS idle state
     * (no sessions), no separate allocation of space is memory is done.
     * The buffers will be used from a dynamic buffer pool.
     */
    nv_ptr = (sm_nv_item_s_type*)pdsm_getbuf(sizeof(sm_nv_item_s_type));

    /* Dynamic buffer allocation sanity check */
    if (NULL == nv_ptr)
    {
      MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SM_NV_WRITE failed: no buffer allocated");
      return FALSE;
    }

    /* Populate NV write command queue element */
    nv_ptr->item   = nv_item;
    nv_ptr->data   = *item_ptr;
    nv_ptr->handle = handle;
    nv_ptr->cmd    = NV_READ_F;

    /* Queue a request for read to NV. Put the command into the
     * NV command queue and services them one at a time.
     */
    q_put(&sm_nv_cmd_q, &nv_ptr->link);

    if (q_cnt(&sm_nv_cmd_q) == 1)
    {
      /* If NV command queue has only one item, handle it immediately */
      sm_nv_handle_rdwr_cmd();
    }
    return TRUE;
  }
  else
  {
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SM_NV_WRITE: SM NV not initialized");
    return FALSE;
  }
}

/*===========================================================================

FUNCTION sm_nv_read_from_sub

DESCRIPTION
  This function is used to perform a synchronous read from a subscription-specific NV.
  GPS SHALL CALL THIS ONLY DURING BOOTUP INITIALIZATION

RETURN VALUE
  TRUE  - NV read success
  FALSE - NV read failure (read default value instead)

DEPENDENCIES
 
===========================================================================*/

boolean sm_nv_read_from_sub (nv_items_enum_type nv_item, void* item_ptr, sys_modem_as_id_e_type sub)
{
  /* Since NV read and writes should normally be done in GPS idle state
   * (no sessions), no separate allocation of space in memory is done.
   * The buffers will be used from a dynamic buffer pool.
   */
  boolean ret_val = FALSE;

  /* Passed argument sanity check */
  if (item_ptr == NULL)
  {
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SM_NV_READ: FAILED - NULL parameter");
    return FALSE;
  }

  if ((sub < SM_CONFIG_MIN_SUB_ID) || (sub > SM_CONFIG_MAX_SUB_ID))
  {
    MSG_2( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Invalid Subs ID %d (Max=%d)", sub, SM_CONFIG_MAX_SUB_ID);
    return FALSE;
  }
  
  ret_val = sm_nv_read_from_hw_sub(nv_item, item_ptr, sub);

  MSG_3( MSG_SSID_GPSSM, MSG_LEGACY_MED, "SM NV Hw Read: %d (sub=%d) ret=%d", nv_item, 0, ret_val);

  (void)sm_nv_write_to_cache_sub(nv_item, item_ptr);

  return ret_val;
}



/*===========================================================================

FUNCTION sm_nv_write_to_sub 
 
DESCRIPTION
  This function is used to queue NV Write commands to a subscription-specific NV.
  The writes to NV are performed in asynchronous fashion.

RETURN VALUE
  TRUE  - NV write command successfully sent to NV
  FALSE - NV write command failed to be sent to NV

DEPENDENCIES
  SM NV write callback needs to be registered via call to sm_nv_write_reg

===========================================================================*/

boolean sm_nv_write_to_sub (nv_items_enum_type nv_item, const nv_item_type* item_ptr, uint32 handle)
{
  sm_nv_item_s_type* nv_ptr = NULL;

  /* Passed argument sanity check */
  if ((NULL == item_ptr) || (handle >= SM_NV_CLIENT_HANDLE_MAX))
  {
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SM_NV_WRITE failed: bad parameters");
    return FALSE;
  }


  if (sm_nv_initialized)
  {
    /* Since NV read and writes should normally be done in GPS idle state
     * (no sessions), no separate allocation of space is memory is done.
     * The buffers will be used from a dynamic buffer pool.
     */
    nv_ptr = (sm_nv_item_s_type*)pdsm_getbuf(sizeof(sm_nv_item_s_type));

    /* Dynamic buffer allocation sanity check */
    if (NULL == nv_ptr)
    {
      MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SM_NV_WRITE failed: no buffer allocated");
      return FALSE;
    }

    /* Populate NV write command queue element */
    nv_ptr->item   = nv_item;
    nv_ptr->data   = *item_ptr;
    nv_ptr->handle = handle;
    nv_ptr->cmd    = NV_WRITE_F;

    /* Queue a request for writing to NV. Put the command into the
     * NV Write command queue and services them one at a time.
     */
    q_put(&sm_nv_cmd_q, &nv_ptr->link);

    if (q_cnt(&sm_nv_cmd_q) == 1)
    {
      /* If NV Write command queue has only one item, handle it immediately */
      sm_nv_handle_rdwr_cmd();
    }
    return TRUE;
  }
  else
  {
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SM_NV_WRITE: SM NV not initialized");
    return FALSE;
  }
}


