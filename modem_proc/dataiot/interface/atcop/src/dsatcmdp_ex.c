/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        D A T A   S E R V I C E S
                A T   C O M M A N D   P R O C E S S O R

GENERAL DESCRIPTION
  This module provides the table driven AT modem command processor.

EXTERNALIZED FUNCTIONS
  dsat_init
    Initializes ATCOP queues and internal variables, as well as all AT
    parameters to their default values.

  dsat_init_service_mode
    Initializes the AT command processor to new operating mode.  AT
    command processing functions are selected from table based on
    new operating mode.

  dsat_process_cmd_line
    Entry point to all of the AT command parsing routines.  Accepts
    a buffer containing a complete command line, and parses and
    executes the commands.

  dsat_process_async_cmd
    Asynchronous ATCOP command handler.  Processes asynchronous ATCOP
    commands received by data services task dispatcher.

  dsat_process_async_signal
    Asynchronous ATCOP REX signal handler.  Processes asynchronous ATCOP
    signals received by data services task dispatcher.

  dsat_nv_sync  
    Initializes the AT parameter manufacturing default values from NV.

  dsat_get_sreg_val
    This function gets the S register value set.

  dsat_get_rlp_params
    Return the parameters which control the UMTS circuit-switched
    non-transparent RLP

  dsat_change_rlp_params
    Set the parameters which control the UMTS circuit-switched
    non-transparent RLP

EXTERNALIZED FUNCTIONS INTERNAL TO DSAT UNIT    
  dsatcmdp_reg_unrec_cmd_handler
    This function registers a handler that is called when an unrecognized
    AT command is received.

  dsatcmdp_init_config
    Initializes the AT parameters to the manufacturing default values.

  dsatcmdp_get_operating_cmd_mode
    This function gets the current AT command processor operating service
    mode.
    
  dsatcmdp_get_current_mode
    This function gets the current AT command etsi mode, GSM or WCDMA or CDMA.

  dsatcmdp_set_restricted_mode
    This function sets the ATCOP restricted command mode based on events
    from GSDI.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Call the function dsat_init on initial power-up and call
  dsat_init_service_mode each time a new operating service mode
  is entered.

Copyright (c) 2000-2021 by Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/dataiot.mpss/2.0/interface/atcop/src/dsatcmdp_ex.c#5 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/01/21   ks      PSM URC cache should be reset when all comamnds are 
                   initialized to default values.
01/25/21   ks      Added support for FSK related AT commands.
01/11/18   rk      Added support of URC for CGREG=4.
11/27/17   rk      Fixed issue with CEREG urc.
05/01/17   skc     Featurizing PBM Code.
04/20/17   rk      Added support of URC for CEREG=4.
04/13/17   rg      Added rau and gprs ready timer for CPSMS
01/19/17   skc     Added enhancement on CHLD and CLCC for VoLTE MPTY Call.
09/26/16   skc     Fixed psm indication issue related to AT+CPSMS
06/13/16   skc     Added support for AT+CPSMS.
10/24/14   tk      ATCoP changes for dynamic memory optimizations.
07/29/14   sc      Added support for NV Refresh.
06/27/14   tk      Initial revision (created file for Dynamic ATCoP).

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include "target.h"
#include "err.h"
#include "amssassert.h"
#include "ds3gsiolib.h"
#include "ds3gsiolib_ex.h"
#include "dsati.h"
#include "dstaski.h"
#include "dsatctab.h"
#include "dsatdl.h"
#include "dsatvend.h"
#include "dsatcmif.h"
#include "dsatvoice.h"
#include "dsatact.h"
#include "dsatme.h"
#include "dsatclient.h"
#include "dsat_ext_api.h"
#include "msg.h"
#include "sio.h"
#include "sys.h"
#include "nv.h"
#include "mmgsdilib_v.h"

#ifdef FEATURE_DSAT_ATCOP_VEND
#if defined(FEATURE_GSM)
#include "l1_ds.h"
#include "geran_grr_msg.h"
#endif
#include "lte_ml1_ext_api.h"
#include "nb1_ml1_ext_api.h"
#include "lte_rrc_ext_msg.h"
#ifdef FEATURE_FSK_TX
#include "msgr_geran.h"
#endif /* FEATURE_FSK_TX */

#endif
#ifdef FEATURE_ETSI_SMS
#include "dsatsms.h"
#endif /*  defined(FEATURE_ETSI_SMS) */

#ifdef FEATURE_DSAT_ETSI_MODE
#include "dsatetsictab.h"
#include "dsatetsime.h"
#include "dsatetsicmif.h"
#include "dsatetsicall.h"
#include "lte_cphy_msg.h"

#ifdef FEATURE_DSAT_ETSI_DATA
#include "dsatetsipkt.h"
#endif /* FEATURE_DSAT_ETSI_DATA */
#endif /* FEATURE_DSAT_ETSI_MODE */

#ifdef FEATURE_DSAT_MDM_FUSION
#include "ds707_rmsm_proxy.h"
#include "ds3gdsdif.h"
#endif /* FEATURE_DSAT_MDM_FUSION */

#include "dsmsgr.h"
#include "cm_msgr_msg.h"
#ifdef FEATURE_DATA_LTE
#include "lte_cphy_msg.h"
#endif /* FEATURE_DATA_LTE */
#include "msgr_nas.h"
#include "emm_ext_msg.h"




/*===========================================================================

                    REGIONAL DEFINITIONS AND DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Current AT command state.
---------------------------------------------------------------------------*/
dsat_mode_enum_type dsatcmdp_at_state;

/*---------------------------------------------------------------------------
  Flag that indicates whether or not an abortable command is in progress.
---------------------------------------------------------------------------*/
dsati_abort_type dsatcmdp_abortable_state = NOT_ABORTABLE;

/*---------------------------------------------------------------------------
  Flag that indicates whether or not calling the function to send a
  result code to the TE ends the abort handling for that command.
---------------------------------------------------------------------------*/
boolean dsatcmdp_end_abort_in_send_rsp = TRUE;

/*---------------------------------------------------------------------------
    Flag that indicates status of ATCoP Initialization.
---------------------------------------------------------------------------*/
boolean dsat_init_completed = FALSE;

/*---------------------------------------------------------------------------
    Flag that indicates whether or not ME is operating in restricted
    command mode due to authorization requirement.
---------------------------------------------------------------------------*/
boolean dsatcmdp_restricted_commands[DSAT_APPS_ID_MAX] = {TRUE,TRUE,TRUE};
/*---------------------------------------------------------------------------
    Flag that indicates a async cmd is under processing,
    Atcop preprocessor does not process any incoming char when it is TRUE.
---------------------------------------------------------------------------*/
boolean dsatcmdp_processing_async_cmd = FALSE;

/*---------------------------------------------------------------------------
  ds_nv_baud_cmd_buf: Give command to NV to change the Baud Rate.
  This buffer is used to send commands to NV for changing the Baud rate.  A
  new buffer has been used instead of using the DS command buffer because
  ds_change_baud function can be called from any task. The existing nv
  command type expects only DS to call it and blocks the DS task. Since, we
  donot want it to happen, a new command buffer has been used.
  Note that this new command buffer is used by Non-Data tasks only.
---------------------------------------------------------------------------*/
extern nv_cmd_type ds_nv_baud_cmd_buf;

uint32 curr_cmd_attrib = ATTRIB_NONE;

dsat_num_item_type dsatcmdp_dds_qcsimapp_val = 0;


#ifdef IMAGE_QDSP6_PROC
extern rex_crit_sect_type dsat_cm_wm_crit_sect;
extern rex_crit_sect_type dsat_rssi_crit_sect;
#endif /* IMAGE_QDSP6_PROC*/

extern creg_cmd_type creg_cmd_pending;

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/

/*--------------------------------------------------------------------------
  Debug macro for this module.
---------------------------------------------------------------------------*/

#define DEBUG( x )

/*--------------------------------------------------------------------------
  Array of pointers to unrecognized command handlers for each AT command
  operating mode supported.
---------------------------------------------------------------------------*/
dsati_unrec_cmd_handler_ptr_type
  unrec_cmd_handler[NUM_OPER_CMD_MODES] = { NULL };

/* Based on mode this variable will be changed.
   If it is ETSI then we can send it to TE as we are ready to send 
   However in IS707 we need to hold 5 items before we flush 
*/
int max_queued_dsm_items;

/*-------------------------------------------------------------------------
   Operating mode for selection of different AT command sets from
   command table.
-------------------------------------------------------------------------*/
LOCAL dsati_operating_cmd_mode_type operating_mode;
LOCAL dsati_mode_e_type present_mode;

LOCAL dsat_sio_info_s_type *dsatcmdp_sio_info_ptr[DS3G_SIOLIB_PORT_MAX] = {NULL};

#ifdef FEATURE_DATA_LTE
/* ATCoP <--> Message router initialization function  */
LOCAL void dsat_msgr_init(void);
#endif /* FEATURE_DATA_LTE */

#ifdef FEATURE_MODEM_CONFIG_REFRESH
uint16 dsat_ds3g_client_id = 0;

LOCAL void dsatcmdp_register_nv_refresh_cb_func( void );

LOCAL void dsatcmdp_nv_refresh_evt_cb_func
(
  ds3geventmgr_event_type        event_id,
  ds3geventmgr_filter_type      *filter_info_ptr,
  void                          *event_info_ptr,
  void                          *data_ptr
);

LOCAL void dsatcmdp_refresh_subs_based_nv_ex
( 
  void
);
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

/*===========================================================================

FUNCTION DSAT_POWERUP_INIT

DESCRIPTION
  This funtion performs power up initialization of ATCoP varaibles.
  This typically would be run in RcINIT worker thread at Bootup. 
DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsat_powerup_init(void)
{
  uint32                 i;
  dsat_apps_id_e_type    apps_id;

  if(FALSE == dsat_is_atcop_allowed())
  {
     DS_AT_MSG0_ERROR(" ATcop is not initialized");
     return ;
  }

  /*------------------------------------------------------------------------
    Initialize all internal ATCOP variables.
  -------------------------------------------------------------------------*/
  dsatcmdp_at_state                = DSAT_CMD;
  dsatcmdp_abortable_state         = NOT_ABORTABLE;
  dsatcmdp_end_abort_in_send_rsp   = TRUE;
  max_queued_dsm_items             = 1;
  dsat_rsp_call_mode               = DSAT_CALL_MODE_NONE;
  /* Initilize Critical section*/

  rex_init_crit_sect(&dsat_cm_wm_crit_sect);
  //TODO: sanjaykc: rssi
  rex_init_crit_sect(&dsat_rssi_crit_sect);

  //TODO: sanjaykc
  for ( i = 0; i < (uint32)NUM_OPER_CMD_MODES; i++ )
  {
    /* Initialize unrecognized command handler for each mode. */
    unrec_cmd_handler[i] = NULL;
  }
/*------------------------------------------------------------------------
  Initialize all the registered call handlers to NULL
-------------------------------------------------------------------------*/
  dsat_init_cb_handlers();
/*------------------------------------------------------------------------
  Initialize voice call interface
-------------------------------------------------------------------------*/
  dsatvoice_init( );
  /* Update the ATCoP initialization flag */
  dsat_init_completed = TRUE;

  for (apps_id = DSAT_APPS_ID_PRIMARY; apps_id < DSAT_APPS_ID_MAX; apps_id++)
  {
    dsat_qcsimapp_info[apps_id].app_type   = MMGSDI_APP_NONE;
    dsat_qcsimapp_info[apps_id].session_id = MMGSDI_INVALID_SESSION_ID;
    dsat_qcsimapp_info[apps_id].active     = FALSE;
  }

  /* Intialize qcsimapp table */
  dsat_init_qcsimapp_table();

}/*dsat_powerup_init*/

/*===========================================================================

FUNCTION  DSAT_COMPLETE_ATCOP_INITIALIZATION

DESCRIPTION
This function is mimic(equivalent) old dsat_init function. It will initilize complete ATcop 
  Initializes ATCOP queues and internal variables, as well as all AT
 parameters to their default values

DEPENDENCIES
  

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS

===========================================================================*/
void  dsat_complete_atcop_initialization( void )
{
  ds3g_siolib_port_e_type port_id = DS3G_SIOLIB_PORT_NONE ;

  /*------------------------------------------------------------------------
    Register AtCop command processing function with DS task
  -------------------------------------------------------------------------*/
  ds_register_cmd_hdlr(DS_MODULE_ATCOP, dsat_process_async_cmd);

  /*------------------------------------------------------------------------
    Initialize all AT parameters to their default values.
  -------------------------------------------------------------------------*/
  dsatcmdp_init_config( );
  /*-------------------------------------------------------------------------
    Initialize the operating system timers.
  -------------------------------------------------------------------------*/
  dsatutil_init_timers_ex();

  /*------------------------------------------------------------------------
    Initialize ATCoP/Call Manager interface 
  -------------------------------------------------------------------------*/
  dsatcmif_init_cmif_ex( );
#ifdef FEATURE_DSAT_ETSI_MODE
  DSATUTIL_SET_VAL(DSATETSI_EXT_ACT_COPS_ETSI_IDX,0,0,
                         (dsat_num_item_type)DSAT_COPS_MODE_MAX,MIX_NUM_TYPE)

  /*------------------------------------------------------------------------
    Initialize Mobile Equipment interface
  -------------------------------------------------------------------------*/
  dsatetsime_init_me( );

#endif /* FEATURE_DSAT_ETSI_MODE */

//TODO: sanjaykc
#ifdef FEATURE_MODEM_CONFIG_REFRESH
  dsatcmdp_register_nv_refresh_cb_func();
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

  /*------------------------------------------------------------------------
    Initialize ATCoP/PBM interface
  -------------------------------------------------------------------------*/
#if defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM)
  dsatme_init_pbm();
#endif /* defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM) */

#ifdef FEATURE_ETSI_SMS
  /*------------------------------------------------------------------------
    Initialize SMS (Initialize and register Call Back functions)
  -------------------------------------------------------------------------*/
  dsatsms_init_sms_ex( );
#endif /* FEATURE_ETSI_SMS  */

//TODO:sanjaykc
#ifdef FEATURE_DATA_MUX
  dsat_init_port();
#endif /* FEATURE_DATA_MUX */
  
#ifdef FEATURE_DATA_LTE
  /* Initalize msgr router related  */
  dsat_msgr_init();
#endif /* FEATURE_DATA_LTE */
#ifdef FEATURE_DSAT_EXT_CLIENT_SUPPORT

  dsatclient_init_client_info();

#endif /*FEATURE_DSAT_EXT_CLIENT_SUPPORT*/

#ifndef FEATURE_DSAT_DYNAMIC_LOADING
  dsatcmdp_init();
#endif /* FEATURE_DSAT_DYNAMIC_LOADING */
#ifdef FEATURE_DSAT_ATCOP_VEND
#if defined(FEATURE_GSM)
 if(ds_gl1_jammer_detect_cb_register(dsat_jammer_detect_cb)!=TRUE)
 {
    DS_AT_MSG0_HIGH("Can not register cback_type with GSM");
 }
#endif
 ds_lte_jammer_detect_cb_register(dsat_jammer_detect_cb);
#endif   

  for(port_id = DS3G_SIOLIB_DATA_PORT; port_id < DS3G_SIOLIB_PORT_MAX; port_id++)
  {
    dsati_sio_rts_change[port_id].is_rts_asserted = TRUE;
    dsati_sio_rts_change[port_id].rts_high_handler = NULL;
    dsati_sio_rts_change[port_id].is_rts_change_cb_recvd = FALSE;
  }

  /* wait for ATCOP subtask signals */ 
  return ;
  
} /* dsat_complete_atcop_initialization */
/*===========================================================================

FUNCTION  DSAT_INIT 

DESCRIPTION
 This Function will intiate Partial/Complete ATcop intialization.

DEPENDENCIES
  This function must be called once on data services task startup.

RETURN VALUE
  No return

SIDE EFFECTS
  Initializes ATCOP internal variables and queues.

===========================================================================*/
void dsat_init( void )
{
  
  /*------------------------------------------------------------------------- 
    ATCoP Power up Init
  -------------------------------------------------------------------------*/
  dsat_powerup_init();
  /*------------------------------------------------------------------------
    Initialize dynamic loading
  -------------------------------------------------------------------------*/
  dsatdl_init();

  /*------------------------------------------------------------------------
    Allocate memory for all commands value structure .
  -------------------------------------------------------------------------*/
  if(dsat_is_atcop_allowed())
  {
    dsatutil_default_constructor();
    dsat_complete_atcop_initialization();
  }
} /* dsat_init( ) */

/*===========================================================================

FUNCTION DSAT_INIT_SERVICE_MODE

DESCRIPTION
  Initializes the AT command processor to new operating mode. AT
  command processing functions are selected from table based on
  new operating mode. This setting is for a particular subscription.

DEPENDENCIES
  Must be called each time operating service mode changes.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
//TODO:sanjaykc:  who is going to call this to update service mode
void dsat_init_service_mode
(
  sys_sys_mode_e_type    service_mode /* Service mode - GSM, LTE */
)
{
  DS_AT_MSG1_MED("dsat_init_service_mode(): %d", service_mode );

  /* Remap service mode to internal AT command processor service mode
     so proper commmand processing tables are used. */
  switch ( service_mode )
  {
#ifdef FEATURE_GSM    
    case SYS_SYS_MODE_GSM:
      operating_mode = ETSI_CMD_MODE;
      present_mode = DSAT_MODE_GSM;
      max_queued_dsm_items = 1;
      break;
#endif /* FEATURE_GSM */

#ifdef FEATURE_DATA_LTE
    case SYS_SYS_MODE_LTE:
      operating_mode = ETSI_CMD_MODE;
      present_mode = DSAT_MODE_LTE;
      max_queued_dsm_items = 1;
      break;
#endif /* FEATURE_DATA_LTE */
    default:
      break;
  }
} /* dsat_init_service_mode( ) */

/*===========================================================================

FUNCTION DSAT_GET_CURRENT_GW_APPS_ID

DESCRIPTION
  Returns the currently selected GW apps ID.

DEPENDENCIES
  None

RETURN VALUE
  GW apps ID

SIDE EFFECTS
  None

===========================================================================*/
dsat_apps_id_e_type dsat_get_current_gw_apps_id
(
  boolean graceful
)
{
  dsat_apps_id_e_type apps_id = 0;

  if (!IS_VALID_APPS_ID(apps_id))
  {
    DS_AT_MSG1_ERROR("Invalid current GW apps ID %d", apps_id);

    if (FALSE == graceful)
    {
     DS_ASSERT(0);
    }
  }

  return apps_id;
} /* dsat_get_current_gw_apps_id */

/*===========================================================================

FUNCTION DSAT_GET_CURRENT_APPS_ID

DESCRIPTION
  Returns the currently selected apps ID.

DEPENDENCIES
  None

RETURN VALUE
  Apps ID

SIDE EFFECTS
  None

===========================================================================*/
dsat_apps_id_e_type dsat_get_current_apps_id
(
  boolean graceful
)
{
  dsat_apps_id_e_type apps_id = DSAT_APPS_ID_PRIMARY;

  if (IS_ETSI_MODE(dsatcmdp_get_current_mode()))
  {
    apps_id = dsat_get_current_gw_apps_id(graceful);
  }
  return apps_id;
} /* dsat_get_current_apps_id */

/*===========================================================================

FUNCTION DSAT_GET_CURRENT_GW_APP_TYPE

DESCRIPTION
  Returns the currently selected GW app type.

DEPENDENCIES
  None

RETURN VALUE
  GW app type

SIDE EFFECTS
  None

===========================================================================*/
mmgsdi_app_enum_type dsat_get_current_gw_app_type
(
  boolean graceful
)
{
  dsat_apps_id_e_type  apps_id;
  mmgsdi_app_enum_type app_type;

  apps_id = dsat_get_current_gw_apps_id(graceful);

  if (IS_VALID_APPS_ID(apps_id))
  {
    app_type = dsat_qcsimapp_info[apps_id].app_type;
  }
  else
  {
    app_type = MMGSDI_APP_NONE;
  }

  DS_AT_MSG1_MED("Current GW app type = %d", app_type);

  return app_type;
} /* dsat_get_current_gw_app_type */

/*===========================================================================

FUNCTION DSAT_GET_CURRENT_APP_TYPE

DESCRIPTION
  Returns the currently selected app type.

DEPENDENCIES
  None

RETURN VALUE
  App type

SIDE EFFECTS
  None

===========================================================================*/
mmgsdi_app_enum_type dsat_get_current_app_type
(
  boolean graceful
)
{
  dsat_apps_id_e_type  apps_id;
  mmgsdi_app_enum_type app_type;

  apps_id = dsat_get_current_apps_id(graceful);

  if (IS_VALID_APPS_ID(apps_id))
  {
    app_type = dsat_qcsimapp_info[apps_id].app_type;
  }
  else
  {
    app_type = MMGSDI_APP_NONE;
  }

  return app_type;
} /* dsat_get_current_app_type */

/*===========================================================================

FUNCTION dsat_get_current_gw_session_id

DESCRIPTION
  Returns the currently selected GW session ID.

DEPENDENCIES
  None

RETURN VALUE
  GW session ID

SIDE EFFECTS
  None

===========================================================================*/
mmgsdi_session_id_type dsat_get_current_gw_session_id
(
  boolean graceful
)
{
  dsat_apps_id_e_type    apps_id;
  mmgsdi_session_id_type session_id;

  apps_id = dsat_get_current_gw_apps_id(graceful);

  if (IS_VALID_APPS_ID(apps_id))
  {
    session_id = dsat_qcsimapp_info[apps_id].session_id;
  }
  else
  {
    session_id = MMGSDI_INVALID_SESSION_ID;
  }

  DS_AT_MSG1_MED("Current GW session ID = %d", session_id);

  return session_id;
} /* dsat_get_current_gw_session_id */

/*===========================================================================

FUNCTION  DSAT_MSGR_INIT 

DESCRIPTION
  Initializes ATCOP MSGR Router UIM ID's to send and receive any messages. 

DEPENDENCIES
  This function must be called once on data services task startup.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsat_msgr_init( void )
{

  dsmsgrrcv_reg_hdlr( LTE_CPHY_RFCHAIN_CNF ,
                      (dsmsgrcv_msg_hdlr_f)dsat_msgr_hdlr );
  dsmsgrrcv_reg_hdlr( NAS_EMM_GET_EDRX_PARAMS_RSP ,
      (dsmsgrcv_msg_hdlr_f)dsat_msgr_hdlr );

  dsmsgrrcv_reg_hdlr( NAS_EMM_SET_EDRX_CFG_RSP ,
      (dsmsgrcv_msg_hdlr_f)dsat_msgr_hdlr );

#ifdef FEATURE_DSAT_ATCOP_VEND  
  dsmsgrrcv_reg_hdlr( MM_CM_PSM_IMAGE_BOOT_TYPE_IND,
      (dsmsgrcv_msg_hdlr_f)dsat_msgr_hdlr );
  dsmsgrrcv_reg_hdlr( LTE_CPHY_CE_INFO_REPORT_RSP,
      (dsmsgrcv_msg_hdlr_f)dsat_msgr_hdlr ); 
 dsmsgrrcv_reg_hdlr( LTE_RRC_DS_SERVICE_IND,
      (dsmsgrcv_msg_hdlr_f)dsat_msgr_hdlr );
  dsmsgrrcv_reg_hdlr( LTE_CPHY_GET_NEXT_SFN_RSP,
      (dsmsgrcv_msg_hdlr_f)dsat_msgr_hdlr );
  dsmsgrrcv_reg_hdlr( LTE_CPHY_SET_SFN_CONFIG_RSP,
      (dsmsgrcv_msg_hdlr_f)dsat_msgr_hdlr );
  dsmsgrrcv_reg_hdlr( LTE_CPHY_SFN_THRESHOLD_REACHED_IND,
      (dsmsgrcv_msg_hdlr_f)dsat_msgr_hdlr );
  #if defined(FEATURE_GSM)
  dsmsgrrcv_reg_hdlr( GERAN_GRR_CELL_SELECT_INFO_RSP,
      (dsmsgrcv_msg_hdlr_f)dsat_msgr_hdlr );
  #endif
  dsmsgrrcv_reg_hdlr( LTE_RRC_CELL_SELECT_INFO_RSP,
      (dsmsgrcv_msg_hdlr_f)dsat_msgr_hdlr );
#ifdef FEATURE_FSK_TX
  dsmsgrrcv_reg_hdlr( GERAN_GL1_FSK_DEBUG_RSP,
      (dsmsgrcv_msg_hdlr_f)dsat_msgr_hdlr );
  dsmsgrrcv_reg_hdlr( GERAN_GL1_FSK_DEBUG_QUERY_RSP,
      (dsmsgrcv_msg_hdlr_f)dsat_msgr_hdlr );
  dsmsgrrcv_reg_hdlr( GERAN_GL1_FSK_DATA_RSP,
      (dsmsgrcv_msg_hdlr_f)dsat_msgr_hdlr );
  dsmsgrrcv_reg_hdlr( GERAN_GL1_FSK_HOP_TABLE_RSP,
      (dsmsgrcv_msg_hdlr_f)dsat_msgr_hdlr );
  dsmsgrrcv_reg_hdlr( GERAN_GL1_FSK_HOP_TABLE_QUERY_RSP,
      (dsmsgrcv_msg_hdlr_f)dsat_msgr_hdlr );
  dsmsgrrcv_reg_hdlr( GERAN_GL1_FSK_START_RSP,
      (dsmsgrcv_msg_hdlr_f)dsat_msgr_hdlr );
  dsmsgrrcv_reg_hdlr( GERAN_GL1_FSK_STOP_RSP,
      (dsmsgrcv_msg_hdlr_f)dsat_msgr_hdlr );
  dsmsgrrcv_reg_hdlr( GERAN_GL1_FSK_PCL_RSP,
      (dsmsgrcv_msg_hdlr_f)dsat_msgr_hdlr );
  dsmsgrrcv_reg_hdlr( GERAN_GL1_FSK_PCL_QUERY_RSP,
      (dsmsgrcv_msg_hdlr_f)dsat_msgr_hdlr );
#endif /* FEATURE_FSK_TX */
#endif
    
}/* dsat_msgr_init */

/*===========================================================================
FUNCTION  DSAT_MSGR_HDLR
DESCRIPTION
  This function handles messages coming to ATCoP from the Message router. 
  
DEPENDENCIES
  None.

RETURN VALUE
  boolean.

SIDE EFFECTS
  None.

===========================================================================*/
/* ARGSUSED */
boolean dsat_msgr_hdlr 
(
  msgr_umid_type             msgrtype,
  const msgr_hdr_struct_type *dsmsg
)
{
  boolean                              result = FALSE;
  ds_command_type                     *cmd_buf  = NULL;
  dsat_psm_cfg_params_s_type          *psm_config_info = NULL;
  cm_psm_get_cfg_params_rsp_s_type    *cm_psm_get_cfg_params_resp = NULL;
  cm_mm_psm_cfg_change_ind_s_type     *cm_psm_cfg_change_ind;
  mm_cmd_get_edrx_params_rsp_s        *edrx_get_params = NULL;
  dsat_edrx_ph_pref_s_type            *edrx_info = NULL;
  sys_sys_mode_e_type                  current_mode = SYS_SYS_MODE_NONE;
  dsati_mode_e_type                    mode = DSAT_MODE_MAX;
  mm_cmd_set_edrx_cfg_rsp_s           *edrx_set_cnfr = NULL;   
#ifdef FEATURE_DSAT_ATCOP_VEND
  cm_psm_image_boot_type_ind_s_type   *psm_image_ind = NULL;  
  dsat_psm_image_boot_e_type          *psm_image     = NULL;
  lte_cphy_ce_info_rpt_rsp_s          *lte_ceinfo_param_resp = NULL;
  dsat_ceinfo_s_type                  *ceinfo = NULL;
  lte_rrc_ds_service_ind_s            *service_indication = NULL;
  dsat_jd_service_status_ind_s_type   *ds_service_ind = NULL; 
  lte_cphy_get_next_sfn_rsp_s         *lte_get_sfn_param_resp = NULL;
  lte_cphy_set_sfn_config_rsp_s       *lte_set_sfn_param_resp = NULL;
  lte_cphy_sfn_threshold_reached_ind_s *lte_sfn_reached_ind = NULL;
  dsat_sfn_s_type                      *sfn = NULL;
  lte_rrc_cell_select_info_rsp_s       *lte_cell_select_info_rsp = NULL;
  #if defined(FEATURE_GSM)
  geran_grr_cell_select_info_rsp_t     *geran_cell_select_info_rsp = NULL;
  #endif
  dsat_cell_select_info_s              *cell_select = NULL;
#ifdef FEATURE_FSK_TX
  geran_fsk_debug_rsp_type             *fsk_dbg_rsp              = NULL;
  geran_fsk_debug_query_rsp_type       *fsk_dbg_query_rsp        = NULL;
  geran_fsk_data_rsp_type              *fsk_data_rsp             = NULL;
  geran_fsk_hop_table_rsp_type         *fsk_hop_table_rsp        = NULL;
  geran_fsk_hop_table_query_rsp_type   *fsk_hop_table_query_rsp  = NULL;
  geran_fsk_start_rsp_type             *fsk_start_rsp            = NULL;
  geran_fsk_stop_rsp_type              *fsk_stop_rsp             = NULL;
  geran_fsk_pcl_rsp_type               *fsk_pcl_rsp              = NULL;
  geran_fsk_pcl_query_rsp_type         *fsk_pcl_query_rsp        = NULL;
  dsat_fsk_s_type                      *fsk                      = NULL;
#endif /* FEATURE_FSK_TX */
#endif
  
  /*NOTE: When dsmsg is being used, check for NULL  */

  DS_AT_MSG1_HIGH(" dsat_msgr_hdlr with msgr type = [%d]",msgrtype);
  /* check the current network mode and indicate the timers as per the network mode */

  mode = dsatcmdp_get_current_mode();
  if(mode == DSAT_MODE_LTE)
  {
    current_mode = SYS_SYS_MODE_LTE;
  }
  else if(mode == DSAT_MODE_GSM)
  {
    current_mode = SYS_SYS_MODE_GSM;
  }

  switch (msgrtype) 
  {
    case LTE_CPHY_RFCHAIN_CNF :
      result = TRUE;
      break;
    case MM_CM_PSM_GET_CFG_PARAMS_RSP:    
      if(!CHECK_PENDING(DSATETSI_EXT_ACT_CPSMS_ETSI_IDX, 0, DSAT_PENDING_CPSMS_READ) &&
         !CHECK_PENDING(DSATETSI_EXT_ACT_CPSMS_ETSI_IDX, 0, DSAT_PENDING_CPSMS_WRITE) &&
         !CHECK_PENDING(DSATETSI_EXT_CEREG_IDX, 0, DSAT_PENDING_CEREG_READ) &&
         !CHECK_PENDING(DSATETSI_EXT_CGREG_IDX, 0, DSAT_PENDING_CGREG_READ) &&
         !(creg_cmd_pending & CMD_CEREG_4_URC)&&
         !(creg_cmd_pending & CMD_CGREG_4_URC))
      {
        return FALSE;
      }	  

      if( NULL == (cm_psm_get_cfg_params_resp = (cm_psm_get_cfg_params_rsp_s_type *)dsmsg))
      {
        result = FALSE;
        break;
      }
      
      cmd_buf = dsat_get_cmd_buf(sizeof(dsat_psm_cfg_params_s_type), FALSE);
      if (cmd_buf == NULL)
      {
        result = FALSE;
        break;
      }
   
      psm_config_info = cmd_buf->cmd_payload_ptr;

      /* send the message to ATCOP */
      cmd_buf->hdr.cmd_id = DS_CMD_ATCOP_PSM_GET_CONFIG_CMD;
      psm_config_info->psm_enabled  = cm_psm_get_cfg_params_resp->enabled;

      DS_AT_MSG1_HIGH(" dsat_msgr_hdlr with msgr type = [%d]",cmd_buf->hdr.cmd_id);

      if (current_mode == SYS_SYS_MODE_LTE)
      {
        psm_config_info->periodic_tau_timer_value = cm_psm_get_cfg_params_resp->periodic_tau_timer_value;
        psm_config_info->active_timer_value = cm_psm_get_cfg_params_resp->active_timer_value;
      }
      else if (current_mode == SYS_SYS_MODE_GSM || current_mode == SYS_SYS_MODE_WCDMA)
      {
        psm_config_info->periodic_rau_timer_value = cm_psm_get_cfg_params_resp->periodic_rau_timer_value;
        psm_config_info->gprs_ready_timer_value = cm_psm_get_cfg_params_resp->gprs_ready_timer_value;
        psm_config_info->active_timer_value = cm_psm_get_cfg_params_resp->active_timer_value;

      }
      else
      {
        ds_release_cmd_buf(&cmd_buf);
        result = FALSE;
        break;
      }
      
      ds_put_cmd(cmd_buf);
      result = TRUE;
      break;
    case MM_CM_PSM_CFG_CHANGE_IND:
      if( NULL == (cm_psm_cfg_change_ind = (cm_mm_psm_cfg_change_ind_s_type *)dsmsg))
      {
        result = FALSE;
        break;
      }
	  
      cmd_buf = dsat_get_cmd_buf(sizeof(dsat_psm_cfg_params_s_type), FALSE);
      if (cmd_buf == NULL)
      {
        result = FALSE;
        break;
      }
   
      psm_config_info = cmd_buf->cmd_payload_ptr;

      /* send the message to ATCOP */
      cmd_buf->hdr.cmd_id = DS_CMD_ATCOP_PSM_CFG_CHANGE_IND_CMD;
      psm_config_info->change_mask = cm_psm_cfg_change_ind->change_mask;
      psm_config_info->psm_enabled  = cm_psm_cfg_change_ind->psm_enabled;

      if (current_mode == SYS_SYS_MODE_LTE)
      {
        psm_config_info->periodic_tau_timer_value = 
                       cm_psm_cfg_change_ind->periodic_update_timer_value;
      psm_config_info->active_timer_value = 
                       cm_psm_cfg_change_ind->active_timer_value;
      }
      else if (current_mode == SYS_SYS_MODE_GSM || current_mode == SYS_SYS_MODE_WCDMA)
      {
        psm_config_info->periodic_rau_timer_value = 
                      cm_psm_cfg_change_ind->periodic_update_timer_value;
        psm_config_info->active_timer_value = 
                     cm_psm_cfg_change_ind->active_timer_value;		
        psm_config_info->gprs_ready_timer_value = 
                      cm_psm_cfg_change_ind->gprs_ready_timer_value;
      }
      else
      {
		ds_release_cmd_buf(&cmd_buf);
        result = FALSE;
        break;
      }
      
      ds_put_cmd(cmd_buf);
      result = TRUE;
      break;
    case NAS_EMM_GET_EDRX_PARAMS_RSP:
      if( NULL == (edrx_get_params = (mm_cmd_get_edrx_params_rsp_s*)dsmsg))
      {
        result = FALSE;
        break;
      }
      DS_AT_MSG4_HIGH("Pedrx Info edrx_enabled %d rat_type_edrx %d edrx_cycle_length %d edrx_ptw %d", 
                             edrx_get_params->edrx_enabled,
                             edrx_get_params->rat_type,
                             edrx_get_params->edrx_cycle_length,
                             edrx_get_params->edrx_ptw);
      cmd_buf = dsat_get_cmd_buf(sizeof(dsat_edrx_ph_pref_s_type), FALSE);
      if (cmd_buf == NULL)
      {
        result = FALSE;
        break;
      }
      cmd_buf->hdr.cmd_id = DS_CMD_ATCOP_EDRX_CONFIG_GET_CNF_CMD;
      
      edrx_info = cmd_buf->cmd_payload_ptr;
      edrx_info->edrx_enabled = edrx_get_params->edrx_enabled;
      edrx_info->rat_type_edrx = edrx_get_params->rat_type;
      edrx_info->edrx_cycle_length = edrx_get_params->edrx_cycle_length;
      edrx_info->edrx_ptw = edrx_get_params->edrx_ptw;
      DS_AT_MSG4_HIGH("Pedrx Info edrx_enabled %d rat_type_edrx %d edrx_cycle_length %d edrx_ptw %d", 
                             edrx_info->edrx_enabled,
                             edrx_info->rat_type_edrx,
                             edrx_info->edrx_cycle_length,
                             edrx_info->edrx_ptw);

      ds_put_cmd(cmd_buf);      
      result = TRUE;
      break;
        
    case NAS_EMM_SET_EDRX_CFG_RSP:
      if( NULL == (edrx_set_cnfr = (mm_cmd_set_edrx_cfg_rsp_s *)dsmsg))
      {
        result = FALSE;
        break;
      }
      cmd_buf = dsat_get_cmd_buf(sizeof(dsat_edrx_ph_pref_s_type), FALSE);
      if (cmd_buf == NULL)
      {
        result = FALSE;
        break;
      }
      cmd_buf->hdr.cmd_id = DS_CMD_ATCOP_EDRX_CONFIG_SET_CNF_CMD;
      ds_put_cmd(cmd_buf);            
      result = TRUE;
      break;
#ifdef FEATURE_DSAT_ATCOP_VEND
    case MM_CM_PSM_IMAGE_BOOT_TYPE_IND:
      if( NULL == (psm_image_ind = (cm_psm_image_boot_type_ind_s_type *)dsmsg))
      {      
        result = FALSE;
        break;
      }          
      cmd_buf = dsat_get_cmd_buf(sizeof(dsat_psm_image_boot_e_type), FALSE);
      if (cmd_buf == NULL)
      {      
        result = FALSE;
        break;
      }                

      cmd_buf->hdr.cmd_id = DS_CMD_ATCOP_PSM_BOOT_IMAGE_TYPE_IND_CMD;
      psm_image = cmd_buf->cmd_payload_ptr;
     *psm_image = (dsat_psm_image_boot_e_type)psm_image_ind->psm_boot_type;
	  
      ds_put_cmd(cmd_buf);			  
      result = TRUE;
      break;

   case LTE_CPHY_CE_INFO_REPORT_RSP:
      if( NULL == (lte_ceinfo_param_resp = (lte_cphy_ce_info_rpt_rsp_s *)dsmsg))
      {
        result = FALSE;
        break;	  
	  }
	  cmd_buf = dsat_get_cmd_buf(sizeof(dsat_ceinfo_s_type), FALSE);
      if (cmd_buf == NULL)
      {
        result = FALSE;
        break;
      }
      ceinfo = cmd_buf->cmd_payload_ptr;
     //copy the data in to the ds payload

    ceinfo->active_rat           = lte_ceinfo_param_resp->active_rat;
    ceinfo->dl_num_rep           = lte_ceinfo_param_resp->dl_rep_factor;
    ceinfo->rsrp                 = lte_ceinfo_param_resp->rsrp;
    ceinfo->enh_cov_mode         = (dsat_ceinfo_enh_cov_mode_e)lte_ceinfo_param_resp->enh_cov_mode;
    ceinfo->enh_cov_mode_enabled = lte_ceinfo_param_resp->enh_cov_mode_enabled;
    ceinfo->state                = (dsat_ce_info_state_e)lte_ceinfo_param_resp->state;
    ceinfo->cinr                 = lte_ceinfo_param_resp->cinr;
    ceinfo->ul_num_rep           = lte_ceinfo_param_resp->ul_rep_factor;
    cmd_buf->hdr.cmd_id = DS_CMD_ATCOP_CEINFO_CMD;	
    ds_put_cmd(cmd_buf);            
    result = TRUE;
    break;

    case LTE_CPHY_GET_NEXT_SFN_RSP:
      if( NULL == (lte_get_sfn_param_resp = (lte_cphy_get_next_sfn_rsp_s *)dsmsg))
      {
        result = FALSE;
        break;	  
      }

      cmd_buf = dsat_get_cmd_buf(sizeof(dsat_sfn_s_type), FALSE);
      if (cmd_buf == NULL)
      {
        result = FALSE;
        break;
      }

      sfn = cmd_buf->cmd_payload_ptr;

      sfn->min_hsfn_sfn_nbr = lte_get_sfn_param_resp->min_hsfn_sfn_nbr;
      sfn->max_hsfn_sfn_nbr = lte_get_sfn_param_resp->max_hsfn_sfn_nbr;
      sfn->hsfn_sfn_nbr     = lte_get_sfn_param_resp->hsfn_sfn_nbr;
      cmd_buf->hdr.cmd_id = DS_CMD_ATCOP_GET_SFN_CMD;	
      ds_put_cmd(cmd_buf);
      result = TRUE;
      break;

    case LTE_CPHY_SET_SFN_CONFIG_RSP:
      if( NULL == (lte_set_sfn_param_resp = (lte_cphy_set_sfn_config_rsp_s *)dsmsg))
      {
        result = FALSE;
        break;	  
      }

      cmd_buf = dsat_get_cmd_buf(sizeof(dsat_sfn_s_type), FALSE);
      if (cmd_buf == NULL)
      {
        result = FALSE;
        break;
      }

      sfn = cmd_buf->cmd_payload_ptr;
      sfn->req_accepted = lte_set_sfn_param_resp->req_accepted;

      cmd_buf->hdr.cmd_id = DS_CMD_ATCOP_SET_SFN_RESP_CMD;	
      ds_put_cmd(cmd_buf);
      result = TRUE;
      break;

    case LTE_CPHY_SFN_THRESHOLD_REACHED_IND:
      if( NULL == (lte_sfn_reached_ind = (lte_cphy_sfn_threshold_reached_ind_s *)dsmsg))
      {
        result = FALSE;
        break;	  
      }

      cmd_buf = dsat_get_cmd_buf(sizeof(dsat_sfn_s_type), FALSE);
      if (cmd_buf == NULL)
      {
        result = FALSE;
        break;
      }

      sfn = cmd_buf->cmd_payload_ptr;
      sfn->hsfn_sfn_nbr = lte_sfn_reached_ind->hsfn_sfn_nbr;

      cmd_buf->hdr.cmd_id = DS_CMD_ATCOP_SFN_REACHED_IND_CMD;	
      ds_put_cmd(cmd_buf);
      result = TRUE;
      break;

   case LTE_RRC_DS_SERVICE_IND:
      if( NULL == (service_indication = (lte_rrc_ds_service_ind_s *)dsmsg))
      {      
        result = FALSE;
        break;
      }          
      cmd_buf = dsat_get_cmd_buf(sizeof(dsat_jd_service_status_ind_s_type), FALSE);
      if (cmd_buf == NULL)
      {      
        result = FALSE;
        break;
      }                

      cmd_buf->hdr.cmd_id          = DS_CMD_ATCOP_JD_SERVICE_STATUS_CMD;
      ds_service_ind               = cmd_buf->cmd_payload_ptr;
      ds_service_ind->act_rat_type = (dsat_soft_jam_rat_type_e)service_indication->act_rat_type;      
      ds_service_ind->svc_status   = (dsat_jam_det_service_status_e)service_indication->svc_status;
      ds_put_cmd(cmd_buf);			  
      result = TRUE;      
      break;

      #if defined(FEATURE_GSM)
      case GERAN_GRR_CELL_SELECT_INFO_RSP:
      if( NULL == (geran_cell_select_info_rsp = (geran_grr_cell_select_info_rsp_t*)dsmsg))
      {
        result = FALSE;
        break;	  
      }

      cmd_buf = dsat_get_cmd_buf(sizeof(dsat_cell_select_info_s), FALSE);
      if (cmd_buf == NULL)
      {
        result = FALSE;
        break;
      }

      cell_select = cmd_buf->cmd_payload_ptr;
      cell_select->is_ue_camped =  geran_cell_select_info_rsp->is_ue_camped;

      if(cell_select->is_ue_camped == TRUE)
      { 
         cell_select->rat = DSAT_COPS_ACT_GSM;
         cell_select->rxlev_access_min = geran_cell_select_info_rsp->rxlev_access_min;
      }
      cmd_buf->hdr.cmd_id = DS_CMD_ATCOP_CELL_SELECT_INFO_CMD;
      ds_put_cmd(cmd_buf);
      result = TRUE;
      break;
      #endif
	  
      case LTE_RRC_CELL_SELECT_INFO_RSP:
      if( NULL == (lte_cell_select_info_rsp = (lte_rrc_cell_select_info_rsp_s*)dsmsg))
      {
        result = FALSE;
        break;  
      }

      cmd_buf = dsat_get_cmd_buf(sizeof(dsat_cell_select_info_s), FALSE);
      if (cmd_buf == NULL)
      {
        result = FALSE;
        break;
      }

      cell_select = cmd_buf->cmd_payload_ptr;
      cell_select->is_ue_camped =  lte_cell_select_info_rsp->is_ue_camped;

      if(cell_select->is_ue_camped == TRUE)
      { 
         if(lte_cell_select_info_rsp->act_rat_type == LTE_ACTIVE_RAT_TYPE_M1)
         {
           cell_select->rat = DSAT_COPS_ACT_EUTRAN;
         }
         else if(lte_cell_select_info_rsp->act_rat_type == LTE_ACTIVE_RAT_TYPE_NB1)
         {
           cell_select->rat = DSAT_COPS_ACT_EUTRAN_NB_S1;
         }
         cell_select->qrxlevmin = lte_cell_select_info_rsp->q_rxlevmin;
         cell_select->qrxlevmin_ce = lte_cell_select_info_rsp->qrxlevmin_ce;
         cell_select->qrxlevmin_ce1 = lte_cell_select_info_rsp->qrxlevmin_ce1;
      }
      cmd_buf->hdr.cmd_id = DS_CMD_ATCOP_CELL_SELECT_INFO_CMD;
      ds_put_cmd(cmd_buf);
      result = TRUE;
      break;

#ifdef FEATURE_FSK_TX
    case GERAN_GL1_FSK_DEBUG_RSP:
      if( NULL == (fsk_dbg_rsp = (geran_fsk_debug_rsp_type *)dsmsg))
      {
        result = FALSE;
        break;	  
      }

      cmd_buf = dsat_get_cmd_buf(sizeof(dsat_fsk_s_type), FALSE);
      if (cmd_buf == NULL)
      {
        result = FALSE;
        break;
      }

      fsk = cmd_buf->cmd_payload_ptr;
      fsk->status = fsk_dbg_rsp->status;

      cmd_buf->hdr.cmd_id = DS_CMD_ATCOP_FSK_DEBUG_RSP_CMD;	
      ds_put_cmd(cmd_buf);
      result = TRUE;
      break;

    case GERAN_GL1_FSK_DEBUG_QUERY_RSP:
      if( NULL == (fsk_dbg_query_rsp = (geran_fsk_debug_query_rsp_type *)dsmsg))
      {
        result = FALSE;
        break;	  
      }

      cmd_buf = dsat_get_cmd_buf(sizeof(dsat_fsk_s_type), FALSE);
      if (cmd_buf == NULL)
      {
        result = FALSE;
        break;
      }

      fsk = cmd_buf->cmd_payload_ptr;
      fsk->status = fsk_dbg_query_rsp->status;
      if(fsk->status)
      {
        fsk->freq = fsk_dbg_query_rsp->freq;
        fsk->delta = fsk_dbg_query_rsp->delta;
        fsk->baudrate = fsk_dbg_query_rsp->baudrate;
      }

      cmd_buf->hdr.cmd_id = DS_CMD_ATCOP_FSK_DEBUG_QUERY_RSP_CMD;	
      ds_put_cmd(cmd_buf);
      result = TRUE;
      break;

    case GERAN_GL1_FSK_DATA_RSP:
      if( NULL == (fsk_data_rsp = (geran_fsk_data_rsp_type *)dsmsg))
      {
        result = FALSE;
        break;	  
      }

      cmd_buf = dsat_get_cmd_buf(sizeof(dsat_fsk_s_type), FALSE);
      if (cmd_buf == NULL)
      {
        result = FALSE;
        break;
      }

      fsk = cmd_buf->cmd_payload_ptr;
      fsk->status = fsk_data_rsp->status;

      cmd_buf->hdr.cmd_id = DS_CMD_ATCOP_FSK_DATA_RSP_CMD;	
      ds_put_cmd(cmd_buf);
      result = TRUE;
      break;

    case GERAN_GL1_FSK_HOP_TABLE_RSP:
      if( NULL == (fsk_hop_table_rsp = (geran_fsk_hop_table_rsp_type *)dsmsg))
      {
        result = FALSE;
        break;	  
      }

      cmd_buf = dsat_get_cmd_buf(sizeof(dsat_fsk_s_type), FALSE);
      if (cmd_buf == NULL)
      {
        result = FALSE;
        break;
      }

      fsk = cmd_buf->cmd_payload_ptr;
      fsk->status = fsk_hop_table_rsp->status;

      cmd_buf->hdr.cmd_id = DS_CMD_ATCOP_FSK_HOP_TABLE_RSP_CMD;	
      ds_put_cmd(cmd_buf);
      result = TRUE;
      break;

    case GERAN_GL1_FSK_HOP_TABLE_QUERY_RSP:
      if( NULL == (fsk_hop_table_query_rsp = (geran_fsk_hop_table_query_rsp_type *)dsmsg))
      {
        result = FALSE;
        break;	  
      }

      cmd_buf = dsat_get_cmd_buf(sizeof(dsat_fsk_s_type), FALSE);
      if (cmd_buf == NULL)
      {
        result = FALSE;
        break;
      }

      fsk = cmd_buf->cmd_payload_ptr;
      fsk->status = fsk_hop_table_query_rsp->status;
      if(fsk->status)
      {
        fsk->hop_table = dsat_alloc_memory(
            sizeof(geran_fsk_hop_table_type) * 3,
            TRUE
            );

        if (fsk->hop_table == NULL)
        {
          ds_release_cmd_buf(&cmd_buf);
          result = FALSE;
          break;
        }
        (void) dsatutil_memscpy((void*)fsk->hop_table,
                                 sizeof(geran_fsk_hop_table_type) * 3,
                                (void*)fsk_hop_table_query_rsp->hop_table,
                                 sizeof(geran_fsk_hop_table_type) * 3);
      }

      cmd_buf->hdr.cmd_id = DS_CMD_ATCOP_FSK_HOP_TABLE_QUERY_RSP_CMD;	
      ds_put_cmd(cmd_buf);
      result = TRUE;
      break;

    case GERAN_GL1_FSK_START_RSP:
      if( NULL == (fsk_start_rsp = (geran_fsk_start_rsp_type *)dsmsg))
      {
        result = FALSE;
        break;	  
      }

      cmd_buf = dsat_get_cmd_buf(sizeof(dsat_fsk_s_type), FALSE);
      if (cmd_buf == NULL)
      {
        result = FALSE;
        break;
      }

      fsk = cmd_buf->cmd_payload_ptr;
      fsk->status = fsk_start_rsp->status;

      cmd_buf->hdr.cmd_id = DS_CMD_ATCOP_FSK_START_RSP_CMD;	
      ds_put_cmd(cmd_buf);
      result = TRUE;
      break;

    case GERAN_GL1_FSK_STOP_RSP:
      if( NULL == (fsk_stop_rsp = (geran_fsk_stop_rsp_type *)dsmsg))
      {
        result = FALSE;
        break;	  
      }

      cmd_buf = dsat_get_cmd_buf(sizeof(dsat_fsk_s_type), FALSE);
      if (cmd_buf == NULL)
      {
        result = FALSE;
        break;
      }

      fsk = cmd_buf->cmd_payload_ptr;
      fsk->status = fsk_stop_rsp->status;

      cmd_buf->hdr.cmd_id = DS_CMD_ATCOP_FSK_STOP_RSP_CMD;	
      ds_put_cmd(cmd_buf);
      result = TRUE;
      break;

    case GERAN_GL1_FSK_PCL_RSP:
      if( NULL == (fsk_pcl_rsp = (geran_fsk_pcl_rsp_type *)dsmsg))
      {
        result = FALSE;
        break;	  
      }

      cmd_buf = dsat_get_cmd_buf(sizeof(dsat_fsk_s_type), FALSE);
      if (cmd_buf == NULL)
      {
        result = FALSE;
        break;
      }

      fsk = cmd_buf->cmd_payload_ptr;
      fsk->status = fsk_pcl_rsp->status;

      cmd_buf->hdr.cmd_id = DS_CMD_ATCOP_FSK_PCL_RSP_CMD;	
      ds_put_cmd(cmd_buf);
      result = TRUE;
      break;

    case GERAN_GL1_FSK_PCL_QUERY_RSP:
      if( NULL == (fsk_pcl_query_rsp = (geran_fsk_pcl_query_rsp_type *)dsmsg))
      {
        result = FALSE;
        break;	  
      }

      cmd_buf = dsat_get_cmd_buf(sizeof(dsat_fsk_s_type), FALSE);
      if (cmd_buf == NULL)
      {
        result = FALSE;
        break;
      }

      fsk = cmd_buf->cmd_payload_ptr;
      fsk->status = fsk_pcl_query_rsp->status;
      if(fsk->status)
      {
        fsk->pcl_gsm = fsk_pcl_query_rsp->pcl_gsm;
        fsk->pcl_dcs = fsk_pcl_query_rsp->pcl_dcs;
        fsk->pcl_pcs = fsk_pcl_query_rsp->pcl_pcs;
      }

      cmd_buf->hdr.cmd_id = DS_CMD_ATCOP_FSK_PCL_QUERY_RSP_CMD;	
      ds_put_cmd(cmd_buf);
      result = TRUE;
      break;
#endif /* FEATURE_FSK_TX */
#endif
    default:
      DS_AT_MSG1_ERROR("Not supported msgr type %d", (int)msgrtype);
      break;
  }

  if( (msgrtype == MM_CM_PSM_GET_CFG_PARAMS_RSP ||
       msgrtype == MM_CM_PSM_CFG_CHANGE_IND ) && 
       result == TRUE )
  {
    
	DS_AT_MSG3_HIGH("PSM Info change mask %d psm_enabled %d current sys mode %d", 
	  	               psm_config_info->change_mask,
	  	               psm_config_info->psm_enabled,
					   current_mode);
    if (current_mode == SYS_SYS_MODE_LTE)
    {
      DS_AT_MSG2_HIGH("PSM Info tau timer %d active_time %d", 
                         psm_config_info->periodic_tau_timer_value,
                         psm_config_info->active_timer_value);

    }
    else if (current_mode == SYS_SYS_MODE_GSM || current_mode == SYS_SYS_MODE_WCDMA)
    {
      DS_AT_MSG3_HIGH("PSM Info rau timer %d gprs_ready_time %d active_time %d", 
	  	                 psm_config_info->periodic_rau_timer_value,
	  	                 psm_config_info->gprs_ready_timer_value,
	  	                 psm_config_info->active_timer_value);
    }
  }
return result;
}/* dsat_msgr_hdlr */

/*===========================================================================

FUNCTION DSAT_PROCESS_CMD_LINE

DESCRIPTION
  This function parses the AT command line, which may contain multiple
  commands, and processes the commands. Command line does not include
  leading "AT" and is NULL terminated.  Command processing is dependent on
  AT mode: command, online data, or online commmand.  

  Command response and command line result codes are generated.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Token queue is emptied.
  Verifies no DSM items are allocated by referencing formatted_rsp_ptr 
  and will free item if one is allocated.
  
===========================================================================*/
void dsat_process_cmd_line
(
  dsat_mode_enum_type at_state,         /* Command, online data, or online
                                           command */
  byte *cmd_line_ptr                    /* Pointer to NULL terminated
                                           command line. */
)
{
  DSAT_DL_CHECK_SYMBOL_ADDR(dsatdl_vtable.dsatcmdp_process_cmd_line_fp);

  dsatdl_vtable.dsatcmdp_process_cmd_line_fp(at_state, cmd_line_ptr);

  return;
} /* dsat_process_cmd_line( ) */

/*===========================================================================

FUNCTION DSATCMDP_REG_UNREC_CMD_HANDLER

DESCRIPTION
  This function registers a handler that is called when an unrecognized
  AT command is received.  The handler is registered for the AT command
  operating mode specified.  If no handler is registered for a command
  operating mode, an error result code is returned by default when an
  unrecognized command is encountered.

  This functionality is intended to provide support for IS-707 unrecognized
  command handling.

  The handler may be deregistered for the AT command mode specified by
  passing in a NULL pointer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsatcmdp_reg_unrec_cmd_handler
(
  dsati_unrec_cmd_handler_ptr_type func_ptr,    /* Pointer to mode specific
                                                   unrecognized command
                                                   handler */
  dsati_operating_cmd_mode_type oper_mode       /* AT command set operating
                                                   mode */
)
{
  /* Verify command operating mode is valid and then initialize pointer
     to handler. */
  DS_ASSERT( oper_mode < NUM_OPER_CMD_MODES );

  /* Assures lint we won't access array out of bounds. */
  if ( oper_mode < NUM_OPER_CMD_MODES )
  { 
    unrec_cmd_handler[oper_mode] = func_ptr;
  }

} /* dsatcmdp_reg_unrec_cmd_handler( ) */

/*===========================================================================

FUNCTION DSATCMDP_QUEUE_CMD_RESULTS

DESCRIPTION
  This function places the results of AT commands in a queue,
  so they can be output (or discarded) at the end of the command
  line. It will output the contents of the queue if more than a
  defined number of DSM items are on the queue.  This is done
  to control DSM usage, as some responses can be large.

  Note: The queue count threshold may require adjustment for IS-707
  mode of operation, to insure the entire command line response
  can be buffered.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatcmdp_queue_cmd_results
(
  dsm_item_type *item_ptr       /* Pointer to command results to queue */
)
{
  DSAT_DL_CHECK_SYMBOL_ADDR(dsatdl_vtable.dsatcmdpi_queue_cmd_results_fp);

  dsatdl_vtable.dsatcmdpi_queue_cmd_results_fp(item_ptr);

  return;
} /* dsatcmdp_queue_cmd_results( ) */

/*===========================================================================

FUNCTION DSATCMDP_TRACK_ASYNC_CMD

DESCRIPTION
  Asynchronous ATCOP command tracking. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatcmdp_track_async_cmd(void)
{
  static boolean processing_async_cmd_prev = FALSE;

  if (processing_async_cmd_prev != dsatcmdp_processing_async_cmd)
  {
    if (TRUE == dsatcmdp_processing_async_cmd)
    {
      DSAT_DL_CMD_HANDLER(DSAT_DL_CMD_BLOCK_UNLOAD);
    }
    else
    {
      DSAT_DL_CMD_HANDLER(DSAT_DL_CMD_UNBLOCK_UNLOAD);
    }

    processing_async_cmd_prev = dsatcmdp_processing_async_cmd;
  }

  return;
} /* dsatcmdp_track_async_cmd( ) */

/*===========================================================================

FUNCTION DSAT_PROCESS_ASYNC_CMD

DESCRIPTION
  Asynchronous ATCOP command handler.  Processes asynchronous ATCOP commands
  received by data services task dispatcher.  Function looks up commands in
  mode specific command table.  Command processing function corresponding to
  current command and operating mode is called from table. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsat_process_async_cmd
(
  ds_command_type *cmd_ptr          /* Pointer to command */
)
{
  dsati_async_event_handler_type event_handler_func_ptr = NULL;
  dsat_result_enum_type result_code = DSAT_ASYNC_CMD;
  int i;

  DSAT_DL_CHECK_SYMBOL_ADDR(dsatdl_vtable.async_event_table_ptr);

  /* Search for event match in asynchronous event table. */
  for ( i = 0; i < (int) dsatdl_vtable.async_event_table_ptr->size; i++ )
  {
    if ( dsatdl_vtable.async_event_table_ptr->base_ptr[i].event == cmd_ptr->hdr.cmd_id )
    {
      /* An event match was found in table. */
      event_handler_func_ptr =
              dsatdl_vtable.async_event_table_ptr->base_ptr[i].event_handler;
      break;
    }
  }

  /* Call the asynchronous event handler if a valid handler exists. */
  if ( event_handler_func_ptr != NULL )
  {
    result_code = event_handler_func_ptr(cmd_ptr);
  }
  else
  {
    DS_AT_MSG1_HIGH("Unexpected ATCoP asynchronous event %d received!",
         cmd_ptr->hdr.cmd_id);
    result_code = DSAT_ASYNC_EVENT;
  }

  if (DSAT_DL_VALIDATE_SYMBOL_ADDR(dsatdl_vtable.dsatcmdp_process_async_cmd_fp))
  {
    dsatdl_vtable.dsatcmdp_process_async_cmd_fp(cmd_ptr, result_code);

    dsatcmdp_track_async_cmd();
  }

  return;
} /* dsat_process_async_cmd( ) */

/*===========================================================================

FUNCTION DSAT_PROCESS_ASYNC_SIGNAL

DESCRIPTION
  Asynchronous ATCOP REX signal handler.  Processes asynchronous ATCOP
  signals received by data services task dispatcher.  Function looks up
  signals in mode specific signal table.  Signal processing function
  corresponding to current signal and operating mode is called from table. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsat_process_async_signal
(
  rex_sigs_type set_sigs            /* ATCOP signals that are set */
)
{
  dsat_result_enum_type result_code = DSAT_ASYNC_EVENT;
  rex_sigs_type         sigs_remaining = set_sigs;

  if( (set_sigs & GET_SIG(DS_AT_MT_MSG_SIG)) ||
      (set_sigs & GET_SIG(DS_AT_SMS_SIG)))
  {  
#ifdef FEATURE_MODEM_RCINIT_PHASE2
    (void)rex_clr_sigs( rcinit_lookup_rextask("ds"), GET_SIG(DS_AT_SMS_SIG) | GET_SIG(DS_AT_MT_MSG_SIG) );
#else
    (void)rex_clr_sigs( &ds_tcb, GET_SIG(DS_AT_SMS_SIG) | GET_SIG(DS_AT_MT_MSG_SIG) );
#endif /* FEATURE_MODEM_RCINIT_PHASE2 */

#ifdef FEATURE_ETSI_SMS
    /* Signal handler will attempt to process all items in both MT message 
       watermark dsat_mt_msg_wm and SMS watermark dsat_sms_wm. It will
       return before all items are processed if an event handler result
       other than DSAT_ASYNC_CMD or DSAT_ASYNC_EVENT is encountered */
    result_code = dsatsms_signal_handler(dsatcmdp_at_state);
#endif /* FEATURE_ETSI_SMS  */

    sigs_remaining &= ~(GET_SIG(DS_AT_SMS_SIG) | GET_SIG(DS_AT_MT_MSG_SIG));
  }    

  /* If no SMS signals processed or SMS signals processed didn't generate a
     result code that would be interfered with... */
  if( (result_code == DSAT_ASYNC_EVENT) &&
      ( set_sigs & GET_SIG(DS_AT_CM_MSG_SIG) ))
  {  
#ifdef FEATURE_MODEM_RCINIT_PHASE2
    (void)rex_clr_sigs( rcinit_lookup_rextask("ds"), GET_SIG(DS_AT_CM_MSG_SIG) );
#else
    (void)rex_clr_sigs( &ds_tcb, GET_SIG(DS_AT_CM_MSG_SIG) );
#endif /* FEATURE_MODEM_RCINIT_PHASE2 */
    /* Signal handler will attempt to process all items in CM event message 
       watermark. It will return before all items are processed if an 
       event handler result other than DSAT_ASYNC_CMD or DSAT_ASYNC_EVENT 
       is encountered */
    result_code = dsatcmif_signal_handler(dsatcmdp_at_state);

    sigs_remaining &= ~GET_SIG(DS_AT_CM_MSG_SIG) ;
  }

  /* If any signals remain unprocessed set off another round of processing:
     this needed as multiple signals can be received but not all of them 
     processed if they would overwrite a significant result code, and WM's 
     callback called on enqueue will only get called on the transition from 
     empty to non-empty. */
  if ((sigs_remaining & GET_SIG(DS_AT_MT_MSG_SIG)) != 0 ||
      (sigs_remaining & GET_SIG(DS_AT_SMS_SIG))    != 0 ||
      (sigs_remaining & GET_SIG(DS_AT_CM_MSG_SIG)) != 0)
  {
#ifdef FEATURE_MODEM_RCINIT_PHASE2
    (void)rex_set_sigs( rcinit_lookup_rextask("ds"), sigs_remaining );
#else
    (void)rex_set_sigs( &ds_tcb, sigs_remaining );
#endif /* FEATURE_MODEM_RCINIT_PHASE2 */
  }

  if (DSAT_DL_VALIDATE_SYMBOL_ADDR(dsatdl_vtable.dsatcmdp_process_async_signal_fp))
  {
    dsatdl_vtable.dsatcmdp_process_async_signal_fp(result_code);

    dsatcmdp_track_async_cmd();
  }

  return;
} /* dsat_process_async_signal( ) */

/*===========================================================================

FUNCTION DSAT_IS_ATCOP_NV_READ_ALLOWED

DESCRIPTION
  TThis function will return TRUE by default. 
  This is a place holder for OEM's to choose to not read specific NV items which might improve bootup time.

DEPENDENCIES


RETURN VALUE
  TRUE    - Allowed for reading  in production environment  
  FALSE  -  Blocked for reading  in production environment 

SIDE EFFECTS
  None

===========================================================================*/

dsat_num_item_type dsat_is_atcop_allowed
( 
void
)
{
  /*Always return TRUE if  bootup time optimization is disabled */
  return TRUE;
}
/*===========================================================================

FUNCTION DSAT_ETSI_NV_SYNC

DESCRIPTION
  This function will read all ETSI related NV items.

DEPENDENCIES
  

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void dsat_etsi_nv_sync( void )
{
 /* Below NV items which might be skipped in case AT commands are not used in production environment.*/
  /*-------------------------------------------------------------------------
    Initialize hardware specific items 
  -------------------------------------------------------------------------*/
    dsatctab_data_init( );
#ifdef FEATURE_DSAT_ETSI_DATA
  /*-------------------------------------------------------------------------
    Initialize the PDP call setup delay tolerance from the NV.
  -------------------------------------------------------------------------*/
#endif /* FEATURE_DSAT_ETSI_DATA */

#ifdef FEATURE_DSAT_ETSI_DATA
    dsatetsipkt_init_service_preference_from_nv();
#endif /* FEATURE_DSAT_ETSI_DATA */

#if defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM)
    dsatme_init_pbm_mode_from_nv();
#endif /*(FEATURE_ETSI_PBM) || (FEATURE_DSAT_CDMA_PBM) */

}/*dsat_etsi_nv_sync*/

/*===========================================================================

FUNCTION DSAT_NV_SYNC

DESCRIPTION
  This function initializes from NV the AT parameter defaults for those
  stored in NV.  (If the NV item is uninitialized, it first writes the
  current, statically initialized AT parameter default to NV.)

  This should be called in at least the following two situtations:

  1) From dsatcmdp_init_config() (e.g. at boot time, ATZ, AT&F)
  2) Whenever any NV-based parameter is changed

DEPENDENCIES
  All NV-based AT parameter defaults must also have been statically
  initialized to proper values.  The statically-initialized values will be
  written to NV if the NV items are found to be uninitialized.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsat_nv_sync( void )
{
  dsat_etsi_nv_sync();
} /* dsat_nv_sync( ) */

#ifdef FEATURE_MODEM_CONFIG_REFRESH
/*===========================================================================

FUNCTION DSATCMDP_REGISTER_NV_REFRESH_CB_FUNC

DESCRIPTION
  This function registers for NV refresh event.. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void dsatcmdp_register_nv_refresh_cb_func( void )
{
  ds3geventmgr_filter_type     filter_info;
  boolean status = FALSE;
  sys_modem_as_id_e_type       subs_id;

  memset(&filter_info, 0, sizeof(ds3geventmgr_filter_type));
  filter_info.tech = PS_SYS_TECH_ALL;

  for(subs_id = SYS_MODEM_AS_ID_1; subs_id < DSAT_SYS_MODEM_AS_ID_MAX; subs_id++)
  {
    filter_info.ps_subs_id = (ps_sys_subscription_enum_type)ds3gsubsmgr_subs_id_cm_to_ds(subs_id);

    status = ds3geventmgr_event_reg( DS3GEVENTMGR_NV_REFRESH_EV,
                                     DS3GEVENTMGR_CLIENT_ID_ATCOP,
                                     &filter_info,
                                     dsatcmdp_nv_refresh_evt_cb_func,
                                     NULL);
    if(!status)
    {
      DS_AT_MSG0_ERROR(" NV Refresh Registration Failed.");
    }
  }
}
#endif /* FEATURE_MODEM_CONFIG_REFRESH */
/*===========================================================================
FUNCTION dsatcmdp_init_parse_table

DESCRIPTION
  This function initializes the parameters in a specific parse table
  to their default values.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void dsatcmdp_init_parse_table
(
  const dsati_cmd_type *parse_table,    /*  Pointer to parse table  */
  unsigned int table_size               /*  Size of parse table       */
)
{
  uint32                i, j;
  uint32                range_offset;

  for ( i = 0; i < table_size; ++i )
  {
    /*--------------------------------------------------------------------
      A Table entry flagged CONFIG must have a value allocated
    --------------------------------------------------------------------*/
      if ( (parse_table[i].attrib & (CONFIG | LIST | STRING | MIXED_PARAM | 
                                     NO_RESET)) ==
           CONFIG )
      {
        dsat_num_item_type * val_list = (dsat_num_item_type *)dsatutil_get_val_from_cmd_id(parse_table[i].cmd_id);
        dflm_type * def_list = (dflm_type *)parse_table[i].def_lim_ptr;

        /* Ensure defaults provided */
        if ( NULL == def_list || val_list == NULL)
        {
          ERR_FATAL ("Command default limits or val pointers NULL index: %d command id %d",
                     i,parse_table[i].cmd_id,0);
        }
    
        /*---------------------------------------------------------------------
            If the entry is numeric, use the value in the default entry
          ---------------------------------------------------------------------*/
        range_offset = 0;
        for ( j = 0; j < parse_table[i].compound; ++j )
        {
          val_list[j] = (dsat_num_item_type)def_list[j + range_offset].
                    default_v;

          /* For BROKEN_RANGE, advance to next range segment */
          if (0 != (parse_table[i].attrib & BROKEN_RANGE))
          {
            while (MAX_BROKEN_RANGE !=
                   (dsat_num_item_type)def_list[j + range_offset].default_v)
            {
              range_offset++;
            }
          }
        }
      }
      else if ( (parse_table[i].attrib &
                 (CONFIG | LIST | STRING | MIXED_PARAM | 
                  NO_RESET)) == (CONFIG | LIST) )
      {
        dsat_num_item_type * val_list = (dsat_num_item_type *)dsatutil_get_val_from_cmd_id(parse_table[i].cmd_id);
        dflm_type * def_list = (dflm_type *)parse_table[i].def_lim_ptr;
  
        /* Ensure defaults provided */
        if (NULL == def_list || val_list == NULL)
        {
          ERR_FATAL ("Command  default limits or val pointers NULL index: %d command id %d: %d",
                     i,parse_table[i].cmd_id,0);
        }
  
        /*---------------------------------------------------------------------
          If the entry is list, use the value in the default entry
        ---------------------------------------------------------------------*/
        for ( j = 0; j < parse_table[i].compound; ++j )
        {
          val_list[j] = (dsat_num_item_type)def_list[j].default_v;
        }
      }
      else if ( (parse_table[i].attrib & (CONFIG | STRING | MIXED_PARAM | 
                                          NO_RESET)) ==
                (CONFIG | STRING) )
      {
          dsat_string_item_type *val_list = (dsat_string_item_type *)dsatutil_get_val_from_cmd_id(parse_table[i].cmd_id);
        /*-------------------------------------------------------------------
        If the entry is a string, set it to the null string
        -------------------------------------------------------------------*/
         if(NULL != val_list)
         {
           *val_list = '\0';
         }
      }

      /*-------------------------------------------------------------------
        Handle MIXED_PARAM parameters 
      ---------------------------------------------------------------------*/
      else if ( (parse_table[i].attrib & (CONFIG | MIXED_PARAM | LIST
                                          | NO_RESET)) ==
                (CONFIG | MIXED_PARAM | LIST) )
      {
        /* if the entry is mixed parms, init them one by one */
        int index;
        dsat_mixed_param_val_type * val_list = 
              (dsat_mixed_param_val_type *) dsatutil_get_val_from_cmd_id(parse_table[i].cmd_id);
        
        mixed_def_s_type ** mixed_def_list = 
          (mixed_def_s_type **) parse_table[i].def_lim_ptr;
          if(val_list == NULL)
          {
            ERR_FATAL ("Command table parameter val pointers is NULL : command id %d",
                       parse_table[i].cmd_id,0,0);
          }
        for (index = 0; index < parse_table[i].compound; index++)
        {
          if (NULL == mixed_def_list[index])
          {
              ERR_FATAL ("Command table parameter default limits undefined: i= %d,index =%d",
                       i,index,0);
          }
    
          /* The argument is an index into string list, use the default
             in the default entry */
          if( (mixed_def_list[index]->attrib & (CONFIG | STRING | LIST)) == 
              (CONFIG | LIST) )
          {
            val_list[index].num_item = 
              mixed_def_list[index]->def->def_list.default_v;
          }

            /* The argument is an integer with range */
          else if( (mixed_def_list[index]->attrib &
                    (CONFIG | STRING | LIST)) == (CONFIG) )
          {
            val_list[index].num_item =
              mixed_def_list[index]->def->dflm.default_v;
          }

          /* the argument is a string, init it as NULL */
          if( (mixed_def_list[index]->attrib & (CONFIG | STRING | LIST)) == 
              (CONFIG | STRING) )
          {
            val_list[index].string_item[0] = '\0';
          }
        }
      }
  }
} /*  dsatcmdp_init_parse_table( ) */

  
/*===========================================================================

FUNCTION DSATCMDP_INIT_CONFIG

DESCRIPTION
  This function initializes the complete set of AT command parameters
  to their default values.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatcmdp_init_config
(
  void
)
{
  uint32 i, r;          /* Count indices */
  dsati_at_cmd_table_entry_type *array_ptr;  /* Pointer to array of cmd 
                                                table entries */
  /*-----------------------------------------------------------------------
    Set defaults from NV 
  ------------------------------------------------------------------------*/
  dsat_nv_sync( );        

  /*-----------------------------------------------------------------------
    Reset PSM URC cache when all commands are initialized to default values.
  ------------------------------------------------------------------------*/
  dsatme_initialize_to_default( DSAT_PSM_URC_VALS, 
                                dsat_get_base_addr(DSAT_PSM_URC_VALS, FALSE));

  /*-----------------------------------------------------------------------
    Initialize all AT command table parameters to their default values.
  ------------------------------------------------------------------------*/
  {
    for ( r = 0; r < (uint32)NUM_AT_CMD_CATEGORIES; r++ )
    {
      /* Get a pointer to array of command table entries for operating 
         mode from command table. */
      array_ptr = at_cmd_table[r];  

      if ( array_ptr != NULL )
      {
        /* An array of command table entries exists for this element of
           the table, now look for command tables within the array. */

        for ( i = 0; array_ptr[i].table_ptr != NULL; i++ )
        {
          /* Initialize all parameters in each parse table found in the
             array. */
          dsatcmdp_init_parse_table( array_ptr[i].table_ptr, 
                            *(array_ptr[i].table_size) );
        }
      }
    }
  }

#ifdef FEATURE_DSAT_ETSI_MODE
  /*-------------------------------------------------------------------------
    Initialize ETSI call handling
  -------------------------------------------------------------------------*/
  dsatetsicall_init_call();
#endif /* FEATURE_DSAT_ETSI_MODE */

#if defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM)
  dsatme_reset_pb();
#endif /* defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM) */

#ifdef FEATURE_ATCOP_ECHO_CTL_MULTI_PORTS
  /*initialize multi port default echo value*/
  for (i=0; i < DSAT_MODEM_PORT_MAX; i++) 
  {
    DSATUTIL_SET_VAL(DSAT_BASIC_E_IDX,i,0,1,NUM_TYPE)
  }
#endif /*FEATURE_ATCOP_ECHO_CTL_MULTI_PORTS*/

} /*  dsatcmdp_init_config( ) */

/*===========================================================================

FUNCTION DSATCMDP_GET_OPERATING_CMD_MODE

DESCRIPTION
  Gets the current AT command processor operating service mode used for
  selection of different AT command sets from command table.

DEPENDENCIES
  None

RETURN VALUE
  Current operating service mode.

SIDE EFFECTS
  None

===========================================================================*/
dsati_operating_cmd_mode_type dsatcmdp_get_operating_cmd_mode( void )
{
  return operating_mode;
} /* dsatcmdp_get_operating_cmd_mode( ) */

/*===========================================================================

FUNCTION DSATCMDP_GET_CURRENT_MODE

DESCRIPTION
  Gets the current AT command mode, GSM or WCDMA or CDMA.

DEPENDENCIES
  None

RETURN VALUE
  Current service mode.

SIDE EFFECTS
  None

===========================================================================*/
dsati_mode_e_type dsatcmdp_get_current_mode( void )
{
  return present_mode;
} /* dsatcmdp_get_current_mode */

/*===========================================================

FUNCTION DSATCMDP_CALCULATE_CURRENT_MODE

DESCRIPTION
  Gets the current AT command mode, GSM or LTE

DEPENDENCIES
  None

RETURN VALUE
  Current service mode.

SIDE EFFECTS
  None

=============================================================*/
dsati_mode_e_type dsatcmdp_calculate_curr_mode
(
  void
)
{
   sys_sys_mode_e_type  sysmode = SYS_SYS_MODE_NO_SRV;
   dsatcmif_servs_state_info  *ph_val = NULL;

   ph_val = dsat_get_base_addr(DSAT_PH_VALS, TRUE);
   if(ph_val == NULL)
   {
     DS_AT_MSG0_HIGH("dsatcmdp_calculate_curr_mode ph_val is NULL ");     
     sysmode = ds_pdn_hdlr_get_current_network_mode();
     
     if(sysmode == SYS_SYS_MODE_LTE)
     {
        DS_AT_MSG0_HIGH("dsatcmdp_calculate_curr_mode sys mode is LTE");     
        present_mode = DSAT_MODE_LTE;
     }
     else if(sysmode == SYS_SYS_MODE_GSM)
     {     
       DS_AT_MSG0_HIGH("dsatcmdp_calculate_curr_mode sys mode is GSM");     
       present_mode = DSAT_MODE_GSM;         
     }
   }
   else
   {
     //Check from Ph on what rat device camped on 
      if((ph_val->dsat_net_reg_state.sys_mode == SYS_SYS_MODE_NONE) ||
          (ph_val->dsat_net_reg_state.sys_mode == SYS_SYS_MODE_NO_SRV))
      {
        //Check the previous sys mode 
        //Might happen in LPM the original sysmode got NO service but check the cached sysmode
        if((ph_val->dsat_net_reg_state.prev_sys_mode == SYS_SYS_MODE_NONE)||
           (ph_val->dsat_net_reg_state.prev_sys_mode == SYS_SYS_MODE_NO_SRV))
         {        
           DS_AT_MSG0_HIGH("dsatcmdp_calculate_curr_mode ph_val the cached rat is not LTE not GSM");
         }
         else
         {
           if(ph_val->dsat_net_reg_state.prev_sys_mode == SYS_SYS_MODE_LTE)
           {
              DS_AT_MSG0_HIGH("dsatcmdp_calculate_curr_mode ph_val the cached rat is LTE ");
              present_mode = DSAT_MODE_LTE;
           }
           else if(ph_val->dsat_net_reg_state.prev_sys_mode ==          SYS_SYS_MODE_GSM)
           {         
             DS_AT_MSG0_HIGH("dsatcmdp_calculate_curr_mode ph_val the cached rat is GSM ");
             present_mode = DSAT_MODE_GSM;
           }
          else 
          {        
            DS_AT_MSG1_HIGH("dsatcmdp_calculate_curr_mode ph_val the cached rat is not LTE not GSM " 
                          "%d",ph_val->dsat_net_reg_state.prev_sys_mode);
          } 
        }    
      }
      else
      {
         if(ph_val->dsat_net_reg_state.sys_mode == SYS_SYS_MODE_LTE)
         {
            DS_AT_MSG0_HIGH("dsatcmdp_calculate_curr_mode ph_val the current rat is LTE ");
            present_mode = DSAT_MODE_LTE;
         }
         else if(ph_val->dsat_net_reg_state.sys_mode ==          SYS_SYS_MODE_GSM)
         {         
            DS_AT_MSG0_HIGH("dsatcmdp_calculate_curr_mode ph_val the current rat is GSM ");
            present_mode = DSAT_MODE_GSM;
         }
         else 
         {        
           DS_AT_MSG1_HIGH("dsatcmdp_calculate_curr_mode ph_val the cached rat is not LTE not GSM " 
                        "%d",ph_val->dsat_net_reg_state.prev_sys_mode);
        } 
      }    
    }  
   return present_mode; 
}
/*===========================================================================

FUNCTION DSATCMDP_BLOCK_INDICATIONS_EX

DESCRIPTION
  Returns value to indicate whether or not indications sent to the TE
  should be blocked.  No indications should be sent to TE from the
  time entering an AT command line begins to the time command line
  processing completes.  Command line processing is considered completed
  after the final result code or response is sent to the TE.  Also,
  indications should not be sent to the TE while in a data call.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if sending of indications to TE should be blocked or FALSE
  otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean dsatcmdp_block_indications_ex( void )
{
  boolean block_indications_flag = FALSE;

  if (DSAT_DL_VALIDATE_SYMBOL_ADDR(dsatdl_vtable.dsatcmdp_block_indications_fp))
  {
    block_indications_flag = dsatdl_vtable.dsatcmdp_block_indications_fp();
  }

  return block_indications_flag;
} /* dsatcmdp_block_indications_ex( ) */

/*===========================================================================

FUNCTION DSAT_SIO_IS_BUSY

DESCRIPTION
  This function is used by SIOLib to tell ATCoP that SIO is busy now and 
  Mode Specific Handlers are in charge of SIO.

  This is called when Mode Specific Handlers register their SIO Handlers.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  ATCOP stops sending the AT command results to the TE. It will resume
  its normal operatioon only when dsat_sio_is_free() is called.

===========================================================================*/
void dsat_sio_is_busy
(
  ds3g_siolib_port_e_type port_id
)
{
  DSAT_DL_CHECK_SYMBOL_ADDR(dsatdl_vtable.dsatcmdp_sio_is_busy_fp);

  dsatdl_vtable.dsatcmdp_sio_is_busy_fp(port_id);

  DSAT_DL_CMD_HANDLER(DSAT_DL_CMD_BLOCK_UNLOAD);

  return;
}


/*===========================================================================

FUNCTION DSAT_SIO_IS_FREE

DESCRIPTION
  This function is used by SIOLib to tell ATCoP that SIO is free now. ATCoP
  Can now resume its normal operation.

  This is called when Mode Specific Handlers de-register their SIO Handlers.
  i.e. when the serial port is no longer in a call.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  In ETSI mode, the queued SMS indications are flushed.

===========================================================================*/
void dsat_sio_is_free
(
  ds3g_siolib_port_e_type port_id
)
{
  DSAT_DL_CHECK_SYMBOL_ADDR(dsatdl_vtable.dsatcmdp_sio_is_free_fp);

  dsatdl_vtable.dsatcmdp_sio_is_free_fp(port_id);

  DSAT_DL_CMD_HANDLER(DSAT_DL_CMD_UNBLOCK_UNLOAD);

  return;
}

/*===========================================================================

FUNCTION DSATCMDP_SET_RESTRICTED_MODE

DESCRIPTION
  This function sets the ATCOP restricted command mode based on events
  from GSDI.  While in restricted command mode, the UE responds to a
  limitied set of comamnds indicated by the RESTRICTED attribute on
  the command table.  Restricted mode applies only to ETSI mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  May trigger events to ATCOP clients.

===========================================================================*/
void dsatcmdp_set_restricted_mode
(
  boolean             mode,  /* new mode setting */
  dsat_apps_id_e_type apps_id
)
{
  /* Check for mode change */
  if( dsatcmdp_restricted_commands[apps_id] != mode )
  {
    /* Set the new mode */
    dsatcmdp_restricted_commands[apps_id] = mode;
  }
}


/*===========================================================================
FUNCTION DSAT_GET_SREG_VAL

DESCRIPTION
  This function is called by other modules to get  
  any S register value set by S register command.

DEPENDENCIES
  Whenever new S register is added, corresponding entry should be made
  in this function as well.  

RETURN VALUE
  SUCCESS : S register value if the corresponding S register is present
  FAILURE : (-1) if S register is not present.

SIDE EFFECTS
  None

===========================================================================*/
int32 dsat_get_sreg_val
(
  uint8 sreg_num
)
{
#define ERR_RET_VAL  -1
  int32 ret_val = 0 ;

  switch(sreg_num)
  {
    case 0:
      ret_val = (int32)dsatutil_get_val(DSAT_SREG_S0_IDX,0,NUM_TYPE);
      break;
    case 2:
      ret_val = (int32)dsatutil_get_val(DSAT_SREG_S2_IDX,0,NUM_TYPE);
      break;
    case 3:
      ret_val = (int32)dsatutil_get_val(
                                       DSAT_SREG_S3_IDX,0,NUM_TYPE);
      break;
    case 4:
      ret_val = (int32)dsatutil_get_val(
                                       DSAT_SREG_S4_IDX,0,NUM_TYPE);
      break;
    case 5:
      ret_val = (int32)dsatutil_get_val(
                                       DSAT_SREG_S5_IDX,0,NUM_TYPE);
      break;
    case 6:
      ret_val = (int32)dsatutil_get_val(
                                       DSAT_SREG_S6_IDX,0,NUM_TYPE);
      break;
    case 7:
      ret_val = (int32)dsatutil_get_val(
                                       DSAT_SREG_S7_IDX,0,NUM_TYPE);
      break;
    case 8:
      ret_val = (int32)dsatutil_get_val(
                                       DSAT_SREG_S8_IDX,0,NUM_TYPE);
      break;

    case 10:
      ret_val = (int32)dsatutil_get_val(
                                      DSAT_SREG_S10_IDX,0,NUM_TYPE);
      break;
    default:
      ret_val = ERR_RET_VAL;
      break;
  }
  return ret_val;
}/* dsat_get_sreg_val */

/*===========================================================================

FUNCTION DSAT_CHANGE_SIO_PARAMS

DESCRIPTION
  This function is called by other modules to write the value to the current 
  baud rate variable, dsat_ipr_val, and set the flow control method for the.
  specified port. The baud rate change is also written to the NV.

  The third parameter in this function is a boolean type. It informs 
  the function if the Baud rate should be changed NOW or later (later
  implies, after sending acknowledgement). This is needed because, when 
  UI changes the baud rate, the baud rate should take effect immediately. 
  But when an AT command changes the baud rate, the baud rate should be 
  changed after an "OK" is sent to the TE2. 

DEPENDENCIES
  This code does not check for auto baud enabled or not. So, it is the 
  responsibility of the calling function to ascertain that if 
  FEATURE_AUTOBAUD is not enabled then value 0 should not be passed.

  This code can execute in non-DS task context.

RETURN VALUE
  None

SIDE EFFECTS
  Modifies the current values of IPR and the corresponding
  value ds_default_baud in the NV.    

===========================================================================*/
void dsat_change_sio_params
(
    sio_port_id_type port_id,                      /* SIO port_id            */
    sio_flow_ctl_type tx_flow,                    /* TX flow control method */
    sio_flow_ctl_type rx_flow,                    /* RX flow control method */
    sio_bitrate_type new_baud_rate,               /* Requested baud rate    */
    boolean change_baud_now                       /* Immediate or post ack  */
)
{
  dsat_num_item_type temp_val;
  /*--------------------------------------------------------------------------
  Check if the Baud rate is with in the maximum permissible value. If yes 
  update the variable and write to NV. Else return without modifying.

  If Autobaud not allowed, UI gives an "ERROR" to the user. 
  --------------------------------------------------------------------------*/
  if (new_baud_rate >= SIO_BITRATE_BEST ) 
  {
    DS_AT_MSG0_HIGH("Unknown Baud rate requested");
    return;
  }

  /*-------------------------------------------------------------------------
  Copy the new_baud_rate into the ipr variables.
  -------------------------------------------------------------------------*/
  DSATUTIL_SET_VAL(DSAT_EXT_IPR_IDX,0,0,new_baud_rate,NUM_TYPE)

  /*-----------------------------------------------------------------------
  Check if the baud rate should be changed NOW or later and change the
  baud rate accordingly, by informing the SIO. 
  -----------------------------------------------------------------------*/
  
  temp_val = (dsat_num_item_type)dsatutil_get_val(DSAT_EXT_IPR_IDX,0,NUM_TYPE);
  ds3g_siolib_change_sio_params( port_id,
                                 tx_flow,
                                 rx_flow,
                                 (sio_bitrate_type) temp_val,
                                 change_baud_now );

  /*-------------------------------------------------------------------------
  Write the new value of the baud_rate to the NV. 
  -------------------------------------------------------------------------*/
  temp_val = (dsat_num_item_type)dsatutil_get_val(DSAT_EXT_IPR_IDX,0,NUM_TYPE);
  ds_nv_item.ds_default_baudrate = (nv_sio_baudrate_type)temp_val;

  /*-------------------------------------------------------------------------
    Write the new value of the baud_rate to the NV.
  -------------------------------------------------------------------------*/
  temp_val = (dsat_num_item_type)dsatutil_get_val(DSAT_EXT_IPR_IDX,0,NUM_TYPE);
  ds_nv_item.ds_default_baudrate = (nv_sio_baudrate_type)temp_val;
  /*-------------------------------------------------------------------------
    Prepare command buffer to write the item to NV.
        tcb_ptr:    We donot have to inform current task when the write is
                    complete. Note that after the information is written NV,
                    NV task need not inform us. This is done because, baud
                    rate is not very critical and so we donot want to confirm
                    if it is writtena also, we donot want to block current
                    task.
        sigs:       Make it NULL. We donot want any signal to be set because
                    we donot want to block on this action.
        done_q_ptr: Command does not have to go to any queue when done.
        item:       The item to be written is NV_DS_DEFAULT_BAUDRATE_I.
        cmd:        Inform NV to write the item.
        data_ptr:   Give the address of the data to be written as ds_nv_item.
                    Note that this uses the DS space and has a potential to
                    clash between the DS data and UI data. But this is okay
                    because Baud rate given by either UI or AT command will
                    be stored here @ NV_DS_DEFAULT_BAUDRATE_I location.
                    In a racing condition, we leave it for latest entry to
                    be used, which will be done.

        Give the command to the NV task to write the data in the NV.
  -------------------------------------------------------------------------*/
  ds_nv_baud_cmd_buf.tcb_ptr = NULL;
  ds_nv_baud_cmd_buf.sigs = 0;
  ds_nv_baud_cmd_buf.done_q_ptr = NULL;

  ds_nv_baud_cmd_buf.item = NV_DS_DEFAULT_BAUDRATE_I;
  ds_nv_baud_cmd_buf.cmd = NV_WRITE_F;

  ds_nv_baud_cmd_buf.data_ptr =  &ds_nv_item;

  /*-------------------------------------------------------------------------
   Issue the command to NV. Note that we donot wait for the response.
  -------------------------------------------------------------------------*/
  nv_cmd( &ds_nv_baud_cmd_buf );
} /* dsat_change_sio_params() */

/*===========================================================================
FUNCTION DSAT_RMSM_DONE_HANDLER

DESCRIPTION
  This function is called by Mode Handler when commands processing
  is complete.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None
  
======================================================================*/
void dsat_rmsm_done_handler
(
  dsat_rmsm_cmd_type    dsat_rmsm_cmd,
  dsat_rmsm_info_s_type dsat_rmsm_info
)
{
  DSAT_DL_CHECK_SYMBOL_ADDR(dsatdl_vtable.dsatcmdp_rmsm_done_handler_fp);

  dsatdl_vtable.dsatcmdp_rmsm_done_handler_fp(dsat_rmsm_cmd, dsat_rmsm_info);

  return;
} /* dsat_rmsm_done_handler */

/*===========================================================================

FUNCTION DSAT_RMSM_PKT_EVENT_RPT_HANDLER

DESCRIPTION
  This function is called by Mode Handler when commands processing
  is complete.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None
  
======================================================================*/
void dsat_rmsm_pkt_event_rpt_handler
(
  dsat_cgerep_e_type            evt_type,
  dsat_cgerep_evt_info_s_type   *evt_info_p
)
{
  DS_ASSERT(evt_info_p != NULL);
  dsatetsipkt_report_gprs_event_to_te(evt_type,(void*)evt_info_p);
}/* dsat_rmsm_pkt_event_rpt_handler */



/*===========================================================================
FUNCTION DSATETSIPKT_CRTDCP_DONE_HANDLER

DESCRIPTION
  This function is called by Mode Handler when downlink data received over
  control plane to report data to the TE.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None
  
======================================================================*/
void dsatetsipkt_rmsm_crtdcp_done_handler
(
  uint32 profile_number,
  dsm_item_type **data_frame
)
{
  DSAT_DL_CHECK_SYMBOL_ADDR(dsatdl_vtable.dsatcmdp_rmsm_crtdcp_done_handler_fp);

  dsatdl_vtable.dsatcmdp_rmsm_crtdcp_done_handler_fp(profile_number, data_frame);

  return;
} /* dsat_rmsm_crtdcp_done_handler */

#ifdef FEATURE_MODEM_CONFIG_REFRESH
/*===========================================================================

FUNCTION DSATCMDP_NV_REFRESH_EVT_CB_FUNC

DESCRIPTION
  This function is a call back function for DS3GEVENTMGR_NV_REFRESH_EV 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void dsatcmdp_nv_refresh_evt_cb_func
(
  ds3geventmgr_event_type        event_id,
  ds3geventmgr_filter_type      *filter_info_ptr,
  void                          *event_info_ptr,
  void                          *data_ptr
)
{
  sys_modem_as_id_e_type subs_id;
  if ( event_id == DS3GEVENTMGR_NV_REFRESH_EV )
  {
    if ( NULL == event_info_ptr ||
         NULL == filter_info_ptr)
    {
      return;
    }

    if((filter_info_ptr->ps_subs_id < PS_SYS_PRIMARY_SUBS || 
       filter_info_ptr->ps_subs_id > PS_SYS_TERTIARY_SUBS))
    {
      DS_AT_MSG1_ERROR("Invalid Subs Mask: %d",filter_info_ptr->ps_subs_id);
      return;
    }

    /* Re-sync all the subscription based NVs */
    dsatcmdp_refresh_subs_based_nv_ex ();

    if (DSAT_DL_VALIDATE_SYMBOL_ADDR(dsatdl_vtable.dsatcmdp_refresh_subs_based_nv_fp))
    {
      dsatdl_vtable.dsatcmdp_refresh_subs_based_nv_fp();
    }

    if (!ds3geventmgr_set_and_notify_nv_refresh_status( 
                 DS3GEVENTMGR_CLIENT_ID_ATCOP ) )
    {
      DS_AT_MSG0_ERROR("Failed to notify nv refresh status");
    }
  }
}/* dsatcmdp_nv_refresh_evt_cb_func */

/*===========================================================================

FUNCTION DSATCMDP_REFRESH_SUBS_BASED_NV_EX

DESCRIPTION
  This function refreshes all the subscription based NVs 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void dsatcmdp_refresh_subs_based_nv_ex()
{
  dsatetsipkt_init_service_preference_from_nv ();
  dsatsms_sync_nv ();
}/* dsatcmdp_refresh_subs_based_nv_ex */

#endif /* FEATURE_MODEM_CONFIG_REFRESH */

