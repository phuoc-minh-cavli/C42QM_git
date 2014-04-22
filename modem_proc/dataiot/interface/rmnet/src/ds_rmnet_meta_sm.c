/*===========================================================================

                     D S _ R M N E T _ M E T A _ S M . C
DESCRIPTION

  The Data Services Rm Network interface State Machine source file.

  This state machine manages the Rm network interface and coordinates
  wireless WAN connectivity between the Rm and Um interfaces over all
  supported radio technologies.

Copyright (c) 2017-2021 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===============================================  =============*/
/*==========================================================
 
                            EDIT HISTORY FOR FILE

  $Header:  
  $DateTime: 2023/01/11 22:11:46 $ 

when        who    what, where, why 
--------    ---    ----------------------------------------------------------
12/29/17    mkumarsa    First version of the  RMNET  module
===========================================================================*/


/*==========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "ds_headers.h"
#include "ds_crit_sect.h"
#include "dsm.h"
#include "ds_rmnet_smi.h"
#include "ds_rmnet_xport.h"
#include "ds_qmi_nv_util.h"
#include "ds_system_heap.h"
#include "rmnet_opt_defs.h"
#include <stringl/stringl.h>
#include "ds_qmap.h"
#include "ds_pdn_defs.h"
#include "ds_fwk.h"
#include "dstaski.h"
#include "ps_logging.h"
#include "ds_fwki.h"
#include "ds_qmi_nipd.h"
#include "a2_sio.h"
#include "ds_rmnet_shim.h"
#include "ds_pdn_cp_data_hdlr.h"

/*============================================================================

                                 DATA TYPES                        

=============================================================================*/

ds_crit_sect_type rmnet_crit_section;
ds_crit_sect_type rmnet_rx_pkt_hdlr_crit_section;

#ifdef TEST_FRAMEWORK
 #error code not present
#endif

/*-----------------------------------------------------------------------------
      EFS Configuration and parsing definitions
 -----------------------------------------------------------------------------*/
rmnet_efs_global_config_s_type rmnet_efs_global_config_params;

#ifdef FEATURE_DATA_QCRMCALL
/*-----------------------------------------------------------------------------
   RMNET $QCRMCALL
------------------------------------------------------------------------------*/
typedef enum
{
  RMNET_QCRMCALL_STATE_NONE   = 0,
  RMNET_QCRMCALL_STATE_START  = 1,
  RMNET_QCRMCALL_STATE_UP     = 2,
  RMNET_QCRMCALL_STATE_STOP   = 3
} rmnet_qcrmcall_state_enum;

typedef struct rmnet_qcrmcall_inst
{
  uint32                     ep_id;
  uint8                      mux_id;
  rmnet_qcrmcall_state_enum  v4_state;
  rmnet_qcrmcall_state_enum  v6_state; 
  rmnet_qcrmcall_cb_type     v4_cb;
  rmnet_qcrmcall_cb_type     v6_cb;
} rmnet_qcrmcall_inst_type;

static rmnet_qcrmcall_inst_type rmnet_qcrmcall_inst[RMNET_QCRMCALL_MAX];

static void rmnet_meta_smi_process_qcrmcall_cmd
(
  rmnet_qcrmcall_action_e_type    action,
  rmnet_qcrmcall_request_type   * request,
  rmnet_qcrmcall_cb_type          callback
);

static void rmnet_meta_smi_update_qcrmcall_state
(
  rmnet_smi_info_type * info
);
#endif /* FEATURE_DATA_QCRMCALL */

/*========================================================================

                           FORWARD DECLARATIONS

=========================================================================*/
/*=======================================================================
 FUNCTION RMNET_META_SM_INIT()
    
 DESCRIPTION
   This function inits   a RMNET inst

    
  PARAMETERS
   instance          :  RmNet inst which needs to be inits

  RETURN VALUE
   Bool
  
  DEPENDENCIES
   RMNET module must already have been initialized

  SIDE EFFECTS
   None
 =========================================================================*/

static boolean rmnet_meta_smi_init
(
   rmnet_instance_e_type  instance
);

/*========================================================================
 FUNCTION RMNET_META_SMI_PROCESS_DTR_CHANGED_CMD()
    
 DESCRIPTION
   This function process DTR state chane high or low on an rmnet inst

    
  PARAMETERS
   info          :  RmNet inst pointer which needs to prcess DTR command
  dtr_asserted :  DTR state high or low


  RETURN VALUE
   Bool
  
  DEPENDENCIES
   RMNET module must already have been initialized

  SIDE EFFECTS
   None
 ========================================================================*/

static void rmnet_meta_smi_process_dtr_changed_cmd
(
  rmnet_smi_info_type  * info,
  boolean                dtr_asserted
);


/*-------------------------------------------------------------
  RMNET_META_SM state management and event processing functions
-------------------------------------------------------------*/

/*=======================================================================
 FUNCTION RMNET_META_SMI_TRANSITION_STATE())
    
 DESCRIPTION
   This function process the meta state chane an rmnet inst

    
  PARAMETERS
   info          :  RmNet inst pointer which needs to prcess DTR command
  new_state      :  rmnet state  to move on


  RETURN VALUE
   None
  
  DEPENDENCIES
   RMNET module must already have been initialized

  SIDE EFFECTS
   None
 ======================================================================*/
                   
static void rmnet_meta_smi_transition_state
(
  rmnet_smi_info_type       * info,
  rmnet_meta_smi_state_type   new_state              /* state to tranistion to */
);


/*=====================================================================
 FUNCTION RMNET_META_SMI_HANDLE_RM_DEV_START_PKT_EV()
    
 DESCRIPTION
   This function starts the data call on an rmnet inst

    
  PARAMETERS
   info          :  RmNet inst pointer which needs to start the data call


  RETURN VALUE
   None
  
  DEPENDENCIES
   RMNET module must already have been initialized

  SIDE EFFECTS
   None
 ====================================================================*/
static void rmnet_meta_smi_handle_rm_dev_start_pkt_ev
(
  rmnet_smi_info_type *  info
);

/*===================================================================
 FUNCTION RMNET_META_SMI_HANDLE_RM_LINK_DOWN_EV()
    
 DESCRIPTION
   This function stops the data call on an rmnet inst

    
  PARAMETERS
   info          :  RmNet inst pointer which needs to stop the data call


  RETURN VALUE
   None
  
  DEPENDENCIES
   RMNET module must already have been initialized

  SIDE EFFECTS
   None
 =====================================================================*/
static void rmnet_meta_smi_handle_rm_link_down_ev
(
  rmnet_smi_info_type *  info
);

/*====================================================================
 FUNCTION RMNET_META_SMI_HANDLE_PDN_DOWN_EV()
    
 DESCRIPTION
   This function process the PDN DOWN event on an rmnet inst

    
  PARAMETERS
   info          :  RmNet inst pointer which needs to process the event


  RETURN VALUE
   None
  
  DEPENDENCIES
   RMNET module must already have been initialized

  SIDE EFFECTS
   None
 ====================================================================*/
static void rmnet_meta_smi_handle_pdn_down_ev
(
  rmnet_smi_info_type *  info
);

/*==================================================================
 FUNCTION RMNET_META_SMI_HANDLE_PDN_UP_EV()
    
 DESCRIPTION
   This function processes the PDN UP evnt on an rmnet inst

    
  PARAMETERS
   info          :  RmNet inst pointer which needs to process the event


  RETURN VALUE
   None
  
  DEPENDENCIES
   RMNET module must already have been initialized

  SIDE EFFECTS
   None
 ====================================================================*/
static void rmnet_meta_smi_handle_pdn_up_ev
(
  rmnet_smi_info_type *  info
);

/*====================================================================
 FUNCTION RMNET_META_SMI_HANDLE_ADDR_CFG_CHANGED_EV()
    
 DESCRIPTION
   This function processes the adrress changed event on an rmnet inst

    
  PARAMETERS
   info          :  RmNet inst pointer which needs to process the event


  RETURN VALUE
   None
  
  DEPENDENCIES
   RMNET module must already have been initialized

  SIDE EFFECTS
   None
 ====================================================================*/

static void rmnet_meta_smi_handle_addr_cfg_changed_ev
(
  rmnet_smi_info_type *  info
);

/*===========================================================================
  FUNCTION RMNET_META_SMI_HANDLE_EP_CFG_DONE_EV

  DESCRIPTION
    This function processes RMNET_SM_PORT_EP_CFG_DONE_EV event.

  PARAMETERS
    info - pointer to the interface controller's control block

  RETURN VALUE
    None

  DEPENDENCIES
    must !ONLY! be called from command dispatcher
    - state and initialization of the control block is verified there

  SIDE EFFECTS
    None
===========================================================================*/
static void rmnet_meta_smi_handle_ep_cfg_done_ev
(
    rmnet_smi_info_type *  info
);

/*-------------------------------------------------------------------
  RMNET_META_SM callback functions registered 
  with various external modules
--------------------------------------------------------------------*/

/*===================================================================
 FUNCTION RMNET_META_SMI_PDN_EV_HDLR_CB()
    
 DESCRIPTION
   This function is called by the framework when any 
      PDN events occur a particular rmnet inst

    
  PARAMETERS
   client_handle          :  RmNet inst  which needs to process the event
   event                  : PDN event 
  event_info              : PDN event info passed by the framework
  user_data_ptr           : User data register with th eframework

  RETURN VALUE
   None
  
  DEPENDENCIES
   RMNET module must already have been initialized

  SIDE EFFECTS
   None
 ============================================================*/

static void rmnet_meta_smi_pdn_ev_hdlr_cb
(
  ds_fwk_clnt_handle_type    client_handle,
  ds_pdn_event_enum_type 	event,
  ds_pdn_event_info_u_type	event_info,
 void					   *user_data_ptr
);


static void rmnet_meta_tx_cb
(
  ds_fwk_clnt_handle_type    client_handle,
  dsm_item_type             *dsm_item,
  void                      *user_data_ptr
);

static void rmnet_meta_tx_nipd_cb
(
  ds_fwk_clnt_handle_type    client_handle,
  dsm_item_type             *dsm_item,
  void                      *user_data_ptr
);

/*============================================================

                     EFS Configuration and Parsing Function

=============================================================*/
static void rmnet_meta_sm_get_efs_config_params
(
  void
);

/*============================================================
 FUNCTION RMNET_META_SMI_IN_CALL_CB()
    
 DESCRIPTION
   This function checks if the data call is present on rmnet inst

    
  PARAMETERS
   user_info              :  RmNet inst   on which needs to check the 
                             data call is present

  RETURN VALUE
   None
  
  DEPENDENCIES
   RMNET module must already have been initialized

  SIDE EFFECTS
   None
 ============================================================*/

static boolean rmnet_meta_smi_in_call_cb
(
  void  * user_info
);

/*============================================================
 FUNCTION RMNET_META_SMI_DTR_CB()
    
 DESCRIPTION
   This function processes the DTR events on physical ports

    
  PARAMETERS
   user_info              :  RmNet phys port   on which needs to check the 
                             DTR event

   dtr_asserted           :  DTR state high or low

  RETURN VALUE
   None
  
  DEPENDENCIES
   RMNET module must already have been initialized

  SIDE EFFECTS
   None
 ============================================================*/

static void rmnet_meta_smi_dtr_cb
(
  void   * user_info,
  boolean  dtr_asserted
);

/*============================================================
 FUNCTION RMNET_SET_MANUAL_DNS_SETTINGS()
    
 DESCRIPTION
   This function  sets the manula DNS addresses from NV to rmnet inst

    
  PARAMETERS
   info                  :  RmNet ip info    on which DNS info is been set

  RETURN VALUE
   None
  
  DEPENDENCIES
   RMNET module must already have been initialized

  SIDE EFFECTS
   None
 ============================================================*/

static boolean rmnet_set_manual_dns_settings
(
  rmnet_smi_ip_info_type *info
);

/*============================================================
 FUNCTION RMNET_META_SMI_RX_HIWATER_FUNC()
    
 DESCRIPTION
   This function processes the high wateramrk event on the RX watermark

    
  PARAMETERS
   wm_ptr               :  Sio Rx wm on which high event occured

   user_data_ptr        :  Rmnet  phys xport pointer

  RETURN VALUE
   None
  
  DEPENDENCIES
   RMNET module must already have been initialized

  SIDE EFFECTS
   None
 ============================================================*/

static void rmnet_meta_smi_rx_hiwater_func
(
  struct dsm_watermark_type_s * wm_ptr,
  void                        * user_data_ptr
);

/*============================================================
 FUNCTION RMNET_META_SMI_RX_LOWATER_FUNC()
    
 DESCRIPTION
   This function processes the low wateramrk event on the RX watermark

    
  PARAMETERS

   wm_ptr               :  Sio Rx wm on which high event occured
   user_data_ptr        :  Rmnet  phys xport pointer

  RETURN VALUE
   None
  
  DEPENDENCIES
   RMNET module must already have been initialized

  SIDE EFFECTS
   None
 ============================================================*/

static void rmnet_meta_smi_rx_lowater_func
(
  struct dsm_watermark_type_s * wm_ptr,
  void                        * user_data_ptr

);

/*============================================================
 FUNCTION RMNET_META_SMI_TX_HIWATER_FUNC()
    
 DESCRIPTION
   This function processes the high wateramrk event on the TX watermark

    
  PARAMETERS
   wm_ptr               :  Sio Tx wm on which high event occured
   user_data_ptr        :  Rmnet  phys xport pointer

  RETURN VALUE
   None
  
  DEPENDENCIES
   RMNET module must already have been initialized

  SIDE EFFECTS
   None
 ============================================================*/

static void rmnet_meta_smi_tx_hiwater_func
(
  struct dsm_watermark_type_s * wm_ptr,
  void                        * user_data_ptr

);

/*============================================================
 FUNCTION RMNET_META_SMI_TX_LOWATER_FUNC()
    
 DESCRIPTION
   This function processes the low wateramrk event on the TX watermark

    
  PARAMETERS
   wm_ptr               :  Sio Tx wm on which high event occured
   user_data_ptr        :  Rmnet  phys xport pointer


  RETURN VALUE
   None
  
  DEPENDENCIES
   RMNET module must already have been initialized

  SIDE EFFECTS
   None
 ============================================================*/

static void rmnet_meta_smi_tx_lowater_func
(
  struct dsm_watermark_type_s * wm_ptr,
  void                        * user_data_ptr
);
/*============================================================
 FUNCTION RMNET_RESET_SIO_WM()
    
 DESCRIPTION
   This function   clears the Sio Rx and Tx wm

    
  PARAMETERS
   ip_info               :  RmNet ip_info 
   xport_info           : Rmnet xport containing the wm

  RETURN VALUE
   None
  
  DEPENDENCIES
   RMNET module must already have been initialized

  SIDE EFFECTS
   None
 ============================================================*/

static void rmnet_reset_sio_wm
(
  rmnet_smi_ip_info_type  *ip_info,
  rmnet_xport_type  *xport_info
);
/*============================================================
 FUNCTION RMNET_SETUP_SIO_WM()
    
 DESCRIPTION
   This function   inits the Sio Rx and Tx wm

    
  PARAMETERS
   ip_info               :  RmNet ip_info 
   info                  :  Rmnet state info
       
  RETURN VALUE
   None
  
  DEPENDENCIES
   RMNET module must already have been initialized

  SIDE EFFECTS
   None
 ============================================================*/

static int32 rmnet_setup_sio_wm
(
  rmnet_smi_ip_info_type     *ip_info,
  rmnet_smi_info_type        *info
);
/*============================================================
 FUNCTION RMNET_INVOKE_WDS_CB()
    
 DESCRIPTION
   This function invokes    the WDS callaback when any PDN  events 
   occur  on rmnet info

    
  PARAMETERS
   rmnet_inst              :  RmNet inst on hich PDN event happen
   pdn_event               :  event to be processed
   
   event_info              :PDN info 

  RETURN VALUE
   None
  
  DEPENDENCIES
   RMNET module must already have been initialized

  SIDE EFFECTS
   None
 ============================================================*/

static void rmnet_invoke_wds_cb
(
  rmnet_instance_e_type   rmnet_inst,
  ds_pdn_event_enum_type  pdn_event,
  ds_pdn_event_info_u_type event_info
);

/*===========================================================

                               INTERNAL DATA

============================================================*/

/*---------------------------------------------------------------------------
  RmNet state machine info block
---------------------------------------------------------------------------*/
rmnet_smi_info_type    *rmnet_smi_info[RMNET_INSTANCE_MAX];

rmnet_smi_ip_info_type *rmnet_smi_ip_info[RMNET_IP_MAX];

/* Mux id pool for the chiltepin*/
rmnet_mux_call_info  rmnet_call_info[MAX_MUX_ID_POOL_SIZE];

/*----------------------------------------------------------
  Table holds in use flags for all uplink signals
-------------------------------------------------------------*/
static boolean   rmnet_ul_sig_in_use_tbl[RMNET_UL_MAX_SIGNALS];

static rmnet_wds_event_buf_type  *rmnet_wds_evt_buf;

/*----------------------------------------------------------
  RMNET command handler array
-----------------------------------------------------------*/
typedef void (*rmnet_meta_smi_cmd_hdlr_type)( rmnet_smi_info_type *);
static rmnet_meta_smi_cmd_hdlr_type rmnet_meta_smi_cmd_hdlr[RMNET_META_SM_MAX_EV] =
{
  rmnet_meta_smi_handle_rm_dev_start_pkt_ev,                       /* event 0 */
  rmnet_meta_smi_handle_rm_link_down_ev,                           /* event 1 */
  rmnet_meta_smi_handle_pdn_down_ev,                          /* event 2 */
  rmnet_meta_smi_handle_pdn_up_ev,                             /* event 3 */
  rmnet_meta_smi_handle_addr_cfg_changed_ev,                       /* event 4 */
  rmnet_meta_smi_handle_ep_cfg_done_ev                            /*event 5*/
};

/*===========================================================

                        EXTERNAL FUNCTION DEFINITIONS

===========================================================*/
static void rmnet_reset_wm_counters
(
  dsm_watermark_type  * dsm_wm_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  dsm_wm_ptr->highest_cnt           = 0;
  dsm_wm_ptr->total_rcvd_cnt        = 0;
  dsm_wm_ptr->dne_discard_pkt_cnt   = 0;
  dsm_wm_ptr->dne_discard_bytes_cnt = 0;

  return;
} /* rmnet_reset_wm_counters() */

/*============================================================
  FUNCTION RMNET_PROCESS_CMD()

  DESCRIPTION
    This function processes a RMNET cmd.

    It is called by the serving task main loop and will dispatch 
    the  approprate cmd handler function.

  PARAMETERS
    cmd          :  RmNet command to be processed
    user_data_ptr:  private data buffer containing the Rmnet 
                     command   information.

  RETURN VALUE
    None

  DEPENDENCIES
    RMNET module must already have been initialized

  SIDE EFFECTS
    None
============================================================*/
void rmnet_sm_process_cmd
(
  ds_command_enum_type cmd,
  void              *user_data_ptr
)
{
  rmnet_smi_cmd_type * cmd_ptr;
/* - - - - - - - - - - - - - - - - - - - - - -- - - - - - - - */
  cmd_ptr = (rmnet_smi_cmd_type *)user_data_ptr;
  DS_ASSERT (cmd_ptr && (cmd == DS_QMI_RMNET_SM_CMD) && 
             (cmd_ptr->sm_id == RMNET_META_SM));

  switch (cmd_ptr->sm_id)
  {
    case RMNET_META_SM:
      rmnet_meta_sm_process_cmd(cmd, user_data_ptr); 
      break;
    default:
      DS_MSG1(MSG_LEGACY_HIGH,"Ignoring RMNET cmd for SM %d", cmd_ptr->sm_id);
      break;
  }
} /* rmnet_process_cmd() */

/*============================================================
  FUNCTION RMNET_SM_IS_VALID()

  DESCRIPTION
    This function validates the RMNET info.

    It is called by the serving task main loop and will dispatch 
    the  approprate cmd handler function.

  PARAMETERS
    info          :  RmNet info
    
  RETURN VALUE
    None

  DEPENDENCIES
    RMNET module must already have been initialized

  SIDE EFFECTS
    None
============================================================*/
static boolean rmnet_sm_is_valid
(
  rmnet_smi_info_type     *info
)
{
  uint8                 i = 0;  
  rmnet_smi_info_type  *info_ptr = NULL;
/*------------------------------------------------------*/
  if(info == NULL)
  {
    DS_MSG1(MSG_LEGACY_HIGH,"rmnet_sm_is_valid	%x", info);
    return FALSE;
  }
  for(i =0; i< RMNET_INSTANCE_MAX; i++) 
  {
    info_ptr = rmnet_smi_info[i];
    if((info_ptr != NULL) && (info_ptr == info))
    {
      DS_MSG2(MSG_LEGACY_HIGH,"rmnet_sm_is_valid  %x %x", info, info_ptr);
      return TRUE;
    }
  }
  return FALSE;
}


/*=============================================================
  FUNCTION RMNET_SM_PROCESS_EV_CMD()

  DESCRIPTION
    This function processes a RmNet event cmd.

    It is called by the serving task main loop and will dispatch the
    appropriate event handler function.

  PARAMETERS
    cmd:  serving task command buffer containing the RmNet sm
              event information
    user_data_ptr     Information buffer

  RETURN VALUE
    None

  DEPENDENCIES
    RM State Machine should have already been initialized

  SIDE EFFECTS
    None
============================================================*/
void rmnet_sm_process_ev_cmd
(
  ds_command_enum_type cmd,
  void              *user_data_ptr
)
{
  rmnet_smi_ev_cmd_type * cmd_ptr;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  cmd_ptr = (rmnet_smi_ev_cmd_type *)user_data_ptr;
  DS_ASSERT (cmd_ptr && (cmd == DS_QMI_RMNET_SM_EV_CMD) && 
            (cmd_ptr->sm_id == RMNET_META_SM));

  switch (cmd_ptr->sm_id)
  {
    case RMNET_META_SM:
      rmnet_meta_sm_process_ev_cmd(cmd, user_data_ptr); 
      break;
    default:
      DS_MSG1(MSG_LEGACY_HIGH,"Ignoring RMNET event cmd for SM %d",
              cmd_ptr->sm_id);
      break;
  }
} /* rmnet_v4_sm_process_ev_cmd() */

/*============================================================
  FUNCTION RMNET_SM_INIT()

  DESCRIPTION
    This function initializes the RMNET state machine.
    Called from serving task's task init function.

  PARAMETERS
    None

  RETURN VALUE
    Bool

  DEPENDENCIES
    Should only be called once at powerup

  SIDE EFFECTS
    None
============================================================*/
boolean rmnet_sm_init
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_INIT_CRIT_SECTION(&rmnet_crit_section);
  DS_INIT_CRIT_SECTION(&rmnet_rx_pkt_hdlr_crit_section);

  rmnet_wds_evt_buf = NULL;

  /*----------------------------------------------------------
    Check if QOS header type size is even else assert 
  ----------------------------------------------------------*/
  if(((sizeof(rmnet_meta_smi_qos_hdr_type)) % 2 ) != 0)
  {
    DS_ERROR_FATAL("rmnet_meta_smi_qos_hdr_type not even");
  }

  /* Read the EFS configuration at bootup and cache the settings. */
  rmnet_meta_sm_get_efs_config_params();
  
  ds_fwk_reg_events(DS_FWK_CLIENT_RMNET,&rmnet_meta_smi_pdn_ev_hdlr_cb,NULL);
  ds_fwk_reg_tx_cb(DS_FWK_CLIENT_RMNET, &rmnet_meta_tx_cb ,NULL);
  
  memset(rmnet_smi_ip_info, 0, sizeof(rmnet_smi_ip_info));  
  memset(rmnet_smi_info, 0, sizeof(rmnet_smi_info));
  ds_fwk_reg_nipd_tx_cb(DS_FWK_CLIENT_RMNET, &rmnet_meta_tx_nipd_cb ,NULL);
  
  ds_qmap_init();
#ifndef TEST_FRAMEWORK
  a2_sio_register_port_cfg_cb(&ds_rmnet_port_open_cnf_cb);
#endif
  return TRUE;
} /* rmnet_sm_init() */

/*===========================================================================
  FUNCTION DS_RMNET_PORT_OPEN_CNF_CB

  DESCRIPTION
    This function gets called by A2 once logical port is opened 
    on which call is brought up

  PARAMETERS
    port_id: port on which the call is brought up

  RETURN VALUE
    None 

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void ds_rmnet_port_open_cnf_cb
(
  sio_port_id_type	port_id
)
{
  uint8                      i = 0;
  rmnet_smi_info_type       *info = NULL;
  
  //Check for the logical port only.
 
  DS_MSG1(MSG_LEGACY_HIGH,"port_id %d", port_id);
  if(rmnet_shim_check_logical_port(port_id) == TRUE)
  {
     //Find out the rmnet instance and post for bring up 
     //for all V4,V6 and NIPD calls if triggered from APPS
     for(i =0; i< RMNET_INSTANCE_MAX; i++)
     {
        info = rmnet_smi_info[i];
        if((info != NULL) &&
           (info->xport_info->sio_port == port_id))
        {
           rmnet_smi_ip_info[info->constants.ip_inst]->ep_cfg_done = TRUE;
           if(info->meta_sm.state == RMNET_META_SM_EP_CONFIGURING_STATE)
           {
             //Move the state to UM_CONFIG here             
             DS_MSG1(MSG_LEGACY_HIGH,"instance %d", RMNET_META_SM_INFO_TO_INSTANCE(info));
             rmnet_meta_sm_post_event(RMNET_META_SM_INFO_TO_INSTANCE(info),
                                       RMNET_META_SM_EP_CFG_DONE);           
           }
        }
     }
  }
}

/*==========================================================
  FUNCTION RMNET_META_SM_POST_EVENT()

  DESCRIPTION
    This function posts an event to RMNetSM and enqueues the
    corresponding command to be processed in the DS task.

  PARAMETERS
    instance:  Instance of RmNetSm to post event to
    event: The event that is being posted to the state machine.

  RETURN VALUE
    Boolean

  DEPENDENCIES
    RM State Machine should have already been initialized.

  SIDE EFFECTS
    None
===========================================================================*/
boolean rmnet_meta_sm_post_event
(
  rmnet_instance_e_type      instance,
  rmnet_meta_sm_event_type   event
)
{
  rmnet_smi_ev_cmd_type     * cmd_payload;
  rmnet_smi_info_type       * info = NULL;
  ds_command_type           * cmd_ptr; 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (instance < RMNET_INSTANCE_MAX)
  {
    info = rmnet_smi_info[instance];
  }
  if (info == NULL)
  {  
    DS_MSG1(MSG_LEGACY_ERROR,"Invalid RMNET_META_SM NULL state event %d, ", event);
    return FALSE;
  }

  if (event >= RMNET_META_SM_MAX_EV)
  {
    DS_MSG1(MSG_LEGACY_ERROR,"Invalid RMNET_META_SM event %d, ignored", event);
    DS_ASSERT (0);
    return FALSE;
  }

  DS_MSG3(MSG_LEGACY_HIGH,"RMNET inst %d, Event %d posted to "
          "RMNET_META_SM in state %d", instance, event, info->meta_sm.state );

  /*-----------------------------------------------------------
    Get a DS command buffer
  ------------------------------------------------------------*/
  cmd_ptr = ds_allocate_cmd_buf(sizeof(rmnet_smi_ev_cmd_type));
  if ((cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL))
  {
     DS_ERR_FATAL("Out of memory");
     return FALSE;
  }
  cmd_ptr->hdr.cmd_id = DS_QMI_RMNET_SM_EV_CMD;
  cmd_payload = (rmnet_smi_ev_cmd_type *)cmd_ptr->cmd_payload_ptr;

  /*-----------------------------------------------------------
    Post cmd to DS task corresponding to each event
  -----------------------------------------------------------*/
  cmd_payload->sm_id             = RMNET_META_SM;
  cmd_payload->event             = event;
  cmd_payload->rmnet_sm_info_ptr = (void *) info;

  ds_put_cmd(cmd_ptr);
  return TRUE;
} /* rmnet_meta_sm_post_event() */


/*============================================================
  FUNCTION RMNET_META_SM_PROCESS_EV_CMD()

  DESCRIPTION
    This function processes a RmNet RmSM event cmd.

    It is called by the serving task main loop and will dispatch the
    associated event handler function.

  PARAMETERS
    cmd :  serving task command buffer containing the RmNet sm
              event information
              
    user_data_ptr:  private data buffer containing the Rmnet 
                        command   information.
    user_data_ptr  :           

  RETURN VALUE
    None

  DEPENDENCIES
    RM State Machine should have already been initialized

  SIDE EFFECTS
    None
=============================================================*/
void rmnet_meta_sm_process_ev_cmd
(
  ds_command_enum_type cmd,
  void * user_data_ptr
)
{
  rmnet_smi_info_type      * info;
  rmnet_meta_sm_event_type   event;
  rmnet_smi_ev_cmd_type    * cmd_ptr;
  rmnet_instance_e_type      rmnet_inst;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  cmd_ptr = (rmnet_smi_ev_cmd_type *)user_data_ptr;
  DS_ASSERT (cmd_ptr && (cmd == DS_QMI_RMNET_SM_EV_CMD) && 
                      (cmd_ptr->sm_id == RMNET_META_SM));

  event = (rmnet_meta_sm_event_type) cmd_ptr->event;
  info  = (rmnet_smi_info_type *)
          cmd_ptr->rmnet_sm_info_ptr;

  /*-------------------------------------------------------------------------
    Verify that RMNET_META_SM is initialized
  -------------------------------------------------------------------------*/
  if (info == NULL)
  {
    DS_MSG1(MSG_LEGACY_ERROR,"Can't process event %d, RMNET_META_SM not inited", event);
    DS_ASSERT(0);
    return;
  }

  rmnet_inst = RMNET_META_SM_INFO_TO_INSTANCE(info);

  DS_MSG3(MSG_LEGACY_ERROR,"RmNetSm event %d in state %d, RMNET inst %d",
                   event, info->meta_sm.state, rmnet_inst);

  /*-------------------------------------------------------------------------
    Validate posted event before processing
  -------------------------------------------------------------------------*/
  if (event >= RMNET_META_SM_MAX_EV)
  {
    DS_MSG1(MSG_LEGACY_ERROR,"Invald RMNET_META_SM event %d, ignoring", event );
    DS_ASSERT(0);
    return;
  }

  /*-------------------------------------------------------------------------
    Validate RMNET_META_SM state before processing event -
    this allows event handler 
    functions to assume that state is valid.
  -------------------------------------------------------------------------*/
  if (info->meta_sm.state >= RMNET_META_SM_MAX_STATE)
  {
    DS_MSG1(MSG_LEGACY_ERROR,"Invalid RMNET_META_SM state %d", info->meta_sm.state);
    DS_ASSERT( 0 );
    return;
  }

  /*-------------------------------------------------------------------------
    Invoke associated command handler function
  -------------------------------------------------------------------------*/
  (*(rmnet_meta_smi_cmd_hdlr[event]))(info);

} /* rmnet_meta_sm_process_ev_cmd() */

/*=============================================================
  FUNCTION RMNET_META_SM_PROCESS_CMD()

  DESCRIPTION
    This function processes a RmNet Meta SM cmd.

  PARAMETERS
    cmd_ptr:  serving task command buffer containing the RmNet sm
              event information

  RETURN VALUE
    None

  DEPENDENCIES
    RM State Machine should have already been initialized

  SIDE EFFECTS
    None
=============================================================*/
void rmnet_meta_sm_process_cmd
(
  ds_command_enum_type    cmd,
  void                *user_data_ptr
)
{
  rmnet_smi_cmd_type      *   cmd_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_ptr = (  rmnet_smi_cmd_type *)user_data_ptr;
  DS_ASSERT (cmd_ptr && (cmd == DS_QMI_RMNET_SM_CMD) && 
                (cmd_ptr->sm_id == RMNET_META_SM));

  switch(cmd_ptr->cmd_type)
  {
#ifdef FEATURE_DATA_QCRMCALL
    case RMNET_SM_CMD_QCRMCALL:
      rmnet_meta_smi_process_qcrmcall_cmd(
                          cmd_ptr->data.qcrmcall.action,
                          &cmd_ptr->data.qcrmcall.request,
                          cmd_ptr->data.qcrmcall.callback);
      break;
#endif /* FEATURE_DATA_QCRMCALL */
    default:
      DS_MSG1(MSG_LEGACY_ERROR,"Received Unhandled rmnet_meta_sm cmd %d",
                      cmd_ptr->cmd_type);
      break;
  }
} /* rmnet_meta_sm_process_cmd() */


/*===========================================================================
  FUNCTION RMNET_META_SM_IN_CALL()

  DESCRIPTION
    Returns true if we're in a packet call.  Called by ws_if.c

  PARAMETERS
    instance - the RMNET Rm interface to query

  RETURN VALUE
    TRUE  - Rm interface 'instance' is starting/in/stopping a packet call
    FALSE - Rm interface 'instance' is idle

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean rmnet_meta_sm_in_call
(
  rmnet_instance_e_type  instance
)
{
  rmnet_smi_info_type *  info = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (instance < RMNET_INSTANCE_MAX)
  {
    info = rmnet_smi_info[instance];
  }

  /*-------------------------------------------------------------------------
    We are in a packet call if rmsm state is non-null
  -------------------------------------------------------------------------*/
  return (info == NULL ||
      info->meta_sm.state == RMNET_META_SM_NULL_STATE) ? FALSE : TRUE;

} /* rmnet_meta_sm_in_call() */


/*===========================================================================
FUNCTION RMNET_META_SMET_NETWORK_CFG_PARAMS

DESCRIPTION
  This function is called to set the parameters received in Start Network
  Interface for network configuration in the rmnet info structure.

PARAMETERS
  instance       : RmNet instance
  net_cfg_params : Network Configuration parameters

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void rmnet_meta_sm_set_network_cfg_params
(
  rmnet_instance_e_type      instance,
  rmnet_meta_sm_network_info_type   *net_cfg_params
)
{
  rmnet_smi_info_type *  info = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (instance < RMNET_INSTANCE_MAX)
  {
    info = rmnet_smi_info[instance];
  }

  if (info == NULL)
  {
    DS_ASSERT(0);
    return;
  }

  memscpy(&(info->meta_sm.net_params),
          sizeof(rmnet_meta_sm_network_info_type),
          net_cfg_params,
          sizeof(rmnet_meta_sm_network_info_type));

} /* rmnet_meta_sm_set_network_cfg_params() */

/*===========================================================================
  FUNCTION RMNET_META_SM_GET_IPV4_MANUAL_DNS_ADDRS()

  DESCRIPTION
    Gets the manual DNS settings for this instance.  Each address that has not
    been set, the type will be returned as IP_ADDR_INVALID.

  PARAMETERS
    instance    - the RmNet instance
    pri_dns_ptr - primary DNS address pointer
    sec_dns_ptr - secondary DNS address pointer

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void rmnet_meta_sm_get_ipv4_manual_dns_addrs
(
  rmnet_instance_e_type  instance,
  ip_addr_type           *pri_dns_ptr,
  ip_addr_type           *sec_dns_ptr
)
{
  rmnet_smi_info_type *  info = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (instance < RMNET_INSTANCE_MAX)
  {
    info = rmnet_smi_info[instance];
  }
  
  if (info == NULL)
  {
    DS_ASSERT(0);
    return;
  }

  if (NULL != pri_dns_ptr)
  {
    memscpy(pri_dns_ptr,
            sizeof(ip_addr_type),
           &info->meta_sm.settings.pri_dns_addr,
           sizeof(ip_addr_type));
  }
  if (NULL != sec_dns_ptr)
  {
    memscpy(sec_dns_ptr,
            sizeof(ip_addr_type),
           &info->meta_sm.settings.sec_dns_addr,
           sizeof(ip_addr_type));
  }
} /* rmnet_meta_sm_get_ipv4_manual_dns_addrs */

/*===========================================================================
  FUNCTION RMNET_META_SM_GET_IPV6_MANUAL_DNS_ADDRS()

  DESCRIPTION
    Gets the manual DNS settings for this instance.  Each address that has not
    been set, the type will be returned as IP_ADDR_INVALID.

  PARAMETERS
    instance    - the RmNet instance
    pri_dns_ptr - primary DNS address pointer
    sec_dns_ptr - secondary DNS address pointer

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void rmnet_meta_sm_get_ipv6_manual_dns_addrs
(
  rmnet_instance_e_type   instance,
  ip_addr_type           *pri_dns_ptr,
  ip_addr_type           *sec_dns_ptr
)
{
  rmnet_smi_info_type *  info = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (instance < RMNET_INSTANCE_MAX)
  {
    info = rmnet_smi_info[instance];
  }

  if (info == NULL)
  {
    DS_ASSERT(0);
    return;
  }

  if (NULL != pri_dns_ptr)
  {
    memscpy(pri_dns_ptr,
            sizeof(ip_addr_type),
           &info->meta_sm.settings.ipv6_pri_dns_addr,
           sizeof(ip_addr_type));
  }
  if (NULL != sec_dns_ptr)
  {
    memscpy(sec_dns_ptr,
            sizeof(ip_addr_type),
           &info->meta_sm.settings.ipv6_sec_dns_addr,
           sizeof(ip_addr_type));
  }
} /* rmnet_meta_sm_get_ipv6_manual_dns_addrs */

/*===========================================================================
  FUNCTION RMNET_META_SMI_XPORT_INIT()

  DESCRIPTION
    Initialize the rmnet xport structure

  PARAMETERS
    xport_info:  pointer to rmnet xport structure
    user_info:   user info ptr

  RETURN VALUE
    TRUE/FALSE

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean rmnet_meta_smi_xport_init
(
  rmnet_xport_type        * xport_info
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (xport_info == NULL)
  {
    return FALSE;
  }

  memset(xport_info, 0, sizeof(rmnet_xport_type));
  /*-------------------------------------------------------------------------
    Watermark queues
  -------------------------------------------------------------------------*/
  dsm_queue_init( &xport_info->tx_wmk,
                  RMNET_TX_WM_DNE,
                  &xport_info->tx_wmk_q );

  dsm_queue_init( &xport_info->rx_wmk,
                  RMNET_RX_WM_DNE,
                  &xport_info->rx_wmk_q );

  /*-------------------------------------------------------------------------
    TX watermark
  -------------------------------------------------------------------------*/
  xport_info->tx_wmk.lo_watermark    = 8192;
  xport_info->tx_wmk.hi_watermark    = 65536;

  xport_info->tx_wmk.dont_exceed_cnt = RMNET_TX_WM_DNE;
  xport_info->tx_wmk.dne_q_cnt       = RMNET_TX_WM_Q_CNT_DNE;

  xport_info->tx_wmk.lowater_func_ptr    = rmnet_meta_smi_tx_lowater_func;
  xport_info->tx_wmk.lowater_func_data   = xport_info;
  xport_info->tx_wmk.hiwater_func_ptr    = rmnet_meta_smi_tx_hiwater_func;
  xport_info->tx_wmk.hiwater_func_data   = xport_info;

  /*-------------------------------------------------------------------------
    RX watermark
  -------------------------------------------------------------------------*/
  xport_info->rx_wmk.lo_watermark    = RMNET_RX_WM_LOW;
  xport_info->rx_wmk.hi_watermark    = RMNET_RX_WM_HIGH;
  xport_info->rx_wmk.dont_exceed_cnt = RMNET_RX_WM_DNE;
  xport_info->rx_wmk.dne_q_cnt       = RMNET_RX_WM_Q_CNT_DNE;

  xport_info->rx_wmk.lowater_func_ptr    = rmnet_meta_smi_rx_lowater_func;
  xport_info->rx_wmk.lowater_func_data   = xport_info;
  xport_info->rx_wmk.hiwater_func_ptr    = rmnet_meta_smi_rx_hiwater_func;
  xport_info->rx_wmk.hiwater_func_data   = xport_info;
  xport_info->rx_wmk.non_empty_func_ptr  = rmnet_xport_default_rx_cb;

  /*-------------------------------------------------------------------------
    Callback functions
  -------------------------------------------------------------------------*/
  xport_info->in_call_cb = rmnet_meta_smi_in_call_cb;
  xport_info->ep_cfg_cb = NULL;
  xport_info->dtr_cb = rmnet_meta_smi_dtr_cb;

  return TRUE;

} /* rmnet_meta_smi_xport_init() */

/*===========================================================================
  FUNCTION RMNET_META_SMI_INIT()

  DESCRIPTION
    This function initializes the state machine.

  PARAMETERS
    instance: Instance      which is been    initialised.

  RETURN VALUE
    TRUE on success
    FALSE on failure

  DEPENDENCIES
    Should only be called once at powerup

  SIDE EFFECTS
    None
===========================================================================*/
static boolean rmnet_meta_smi_init
(
  rmnet_instance_e_type  instance
)
{
  rmnet_smi_info_type   *info;
  uint8                  icmp_outreq_count = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( instance >= RMNET_INSTANCE_MAX ||
              rmnet_smi_info[instance] != NULL )
  {
    DS_ASSERT(0);
    return FALSE;
  }

  DS_SYSTEM_HEAP_MEM_ALLOC(info, sizeof(rmnet_smi_info_type),
                           rmnet_smi_info_type*);
  if (info == NULL)
  {
    return FALSE;
  }

  rmnet_smi_info[instance] = info;
  memset (info, 0, sizeof(rmnet_smi_info_type));
  info->meta_sm.state = RMNET_META_SM_NULL_STATE;

  /*-------------------------------------------------------------------------
    populate the constant values in rmnet_meta_sm_info based on the instance
  -------------------------------------------------------------------------*/
  info->constants.ip_inst = instance / 3;
  info->constants.rmnet_inst = instance;
  
  /*-------------------------------------------------------------------------
    Initialize manual DNS settings (IPv4 and IPv6)
  -------------------------------------------------------------------------*/
  info->meta_sm.settings.pri_dns_addr.type = IP_ADDR_INVALID;
  info->meta_sm.settings.pri_dns_addr.addr.v4 = 0;
  info->meta_sm.settings.sec_dns_addr.type = IP_ADDR_INVALID;
  info->meta_sm.settings.sec_dns_addr.addr.v4 = 0;

  info->meta_sm.settings.ipv6_pri_dns_addr.type = IP_ADDR_INVALID;
  info->meta_sm.settings.ipv6_pri_dns_addr.addr.v6[0] = 0;
  info->meta_sm.settings.ipv6_pri_dns_addr.addr.v6[1] = 0;
  info->meta_sm.settings.ipv6_sec_dns_addr.type = IP_ADDR_INVALID;
  info->meta_sm.settings.ipv6_sec_dns_addr.addr.v6[0] = 0;
  info->meta_sm.settings.ipv6_sec_dns_addr.addr.v6[1] = 0;
  for(icmp_outreq_count = 0; icmp_outreq_count < RMNET_MAX_ICMP_OUTSTANDING_REQ; 
      icmp_outreq_count++)
  {
    info->icmp_data[icmp_outreq_count].identifier = -1;
    info->icmp_data[icmp_outreq_count].sequence_num = -1;
  }
  /*-------------------------------------------------------------------------
    Initialize call end reason
  -------------------------------------------------------------------------*/
  info->meta_sm.call_end_reason = DS_NET_DOWN_REASON_NOT_SPECIFIED;
  
  info->meta_sm.proc_id                = RMNET_PROC_ID_MAX;
  return TRUE;
} /* rmnet_meta_smi_init() */

/*===========================================================================
  FUNCTION RMNET_META_SMI_HANDLE_RM_DEV_START_PKT_EV()
  
  DESCRIPTION
    This function processes RMNET_META_SM_RM_DEV_START_PKT_EV event.

    This event indicates that the UE wants to start a packet
    data session.

  PARAMETERS
    info - pointer to the interface controller's control block

  RETURN VALUE
    None

  DEPENDENCIES
    must !ONLY! be called from command dispatcher
    - state and initialization of the control block is verified there

  SIDE EFFECTS
    None
===========================================================================*/
static void rmnet_meta_smi_handle_rm_dev_start_pkt_ev
(
  rmnet_smi_info_type *  info
)
{
  rmnet_instance_e_type  rmnet_inst;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  rmnet_inst = RMNET_META_SM_INFO_TO_INSTANCE(info);

  DS_MSG2(MSG_LEGACY_ERROR,"Recvd RM_DEV_START_PKT_EV, RMNET inst %d, state %d",
                   rmnet_inst, info->meta_sm.state);

  switch( info->meta_sm.state )
  {
    case RMNET_META_SM_NULL_STATE:
    {
      /*---------------------------------------------------------------------
        Reset flow control status.
      ---------------------------------------------------------------------*/
      DS_ENTER_CRIT_SECTION(&rmnet_crit_section);
      info->rx_fc_mask = 0;
      DS_LEAVE_CRIT_SECTION(&rmnet_crit_section);

      /*---------------------------------------------------------------------
        Transition to UM_CONFIGURING state.
      ---------------------------------------------------------------------*/
      #ifndef TEST_FRAMEWORK
      if(rmnet_smi_ip_info[info->constants.ip_inst]->ep_cfg_done == FALSE)
      {
        rmnet_meta_smi_transition_state(info, RMNET_META_SM_EP_CONFIGURING_STATE);
      }
      else 
      {
        rmnet_meta_smi_transition_state(info, RMNET_META_SM_UM_CONFIGURING_STATE);
      }
      #else
      #error code not present
#endif
      break;
    }
    case RMNET_META_SM_UM_CONFIGURING_STATE:
    case RMNET_META_SM_RM_CONFIGURING_STATE:
    case RMNET_META_SM_NET_IF_UP_STATE:
    case RMNET_META_SM_RM_RECONFIGURING_STATE:
    case RMNET_META_SM_WAITING_FOR_UM_DOWN_STATE:
    default:
    {
      /*---------------------------------------------------------------------
        Its error to get this event in all other states.
      ---------------------------------------------------------------------*/
      DS_MSG1(MSG_LEGACY_ERROR,"Unexpected RM_DEV_START_PKT_EV in %d state",
                      info->meta_sm.state);
      break;
    }
  } /* switch( info->meta_sm.state ) */
} /* rmnet_meta_smi_handle_rm_dev_start_pkt_ev() */


/*===========================================================================
  FUNCTION RMNET_META_SMI_HANDLE_RM_LINK_DOWN_EV

  DESCRIPTION
    This function processes RMNET_META_SM_RM_LINK_DOWN_EV event.

    This event indicates that the tethered device is closing the packet data
    session, and so the RMNET_META_SM should release associated packet sessions.

  PARAMETERS
    info - pointer to the interface controller's control block

  RETURN VALUE
    None

  DEPENDENCIES
    must !ONLY! be called from command dispatcher
    - state and initialization of the control block is verified there

  SIDE EFFECTS
    None
===========================================================================*/
static void rmnet_meta_smi_handle_rm_link_down_ev
(
  rmnet_smi_info_type *  info
)
{
  rmnet_instance_e_type  rmnet_inst;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  rmnet_inst = RMNET_META_SM_INFO_TO_INSTANCE(info);

  DS_MSG2(MSG_LEGACY_ERROR,"Recvd RM_LINK_DOWN_EV, RMNET inst %d, state %d",
                   rmnet_inst, info->meta_sm.state);
  
  switch( info->meta_sm.state )
  {
    case RMNET_META_SM_UM_CONFIGURING_STATE:
    case RMNET_META_SM_RM_CONFIGURING_STATE:
    case RMNET_META_SM_RM_RECONFIGURING_STATE:
    case RMNET_META_SM_NET_IF_UP_STATE:
    {
      rmnet_meta_smi_transition_state(info, RMNET_META_SM_WAITING_FOR_UM_DOWN_STATE);
      break;
    }
    case RMNET_META_SM_EP_CONFIGURING_STATE:
    {
      rmnet_meta_smi_transition_state(info, RMNET_META_SM_NULL_STATE);
      break;
    }

    case RMNET_META_SM_NULL_STATE:
    case RMNET_META_SM_WAITING_FOR_UM_DOWN_STATE:
    {
      /*---------------------------------------------------------------------
        Its error to get this event in all other states.
      ---------------------------------------------------------------------*/
      DS_MSG1(MSG_LEGACY_ERROR,"Ignoring RM_LINK_DOWN_EV in %d state",
                      info->meta_sm.state);
      break;
    }
    default:
    {
      DS_ERROR_FATAL("Bad RmNetSM state!");
      break;
    }
  } /* switch( info->meta_sm.state ) */
} /* rmnet_meta_smi_handle_rm_link_down_ev() */


/*===========================================================================
  FUNCTION RMNET_META_SMI_HANDLE_PDN_DOWN_EV

  DESCRIPTION
    This function processes RMNET_META_SM_UM_IFACE_DOWN_EV event.

    This event indicates that network has closed the packet data session,
    and so the RMNET_META_SM should release the tethered packet session.

  PARAMETERS
    info - pointer to the interface controller's control block

  RETURN VALUE
    None

  DEPENDENCIES
    must !ONLY! be called from command dispatcher
    - state and initialization of the control block is verified there

  SIDE EFFECTS
    None
===========================================================================*/
static void rmnet_meta_smi_handle_pdn_down_ev
(
  rmnet_smi_info_type *  info
)
{
  rmnet_instance_e_type             rmnet_inst;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  rmnet_inst = RMNET_META_SM_INFO_TO_INSTANCE(info);

  DS_MSG2(MSG_LEGACY_ERROR,"Recvd UM_IFACE_DOWN_EV, RMNET inst %d, state %d",
                   rmnet_inst, info->meta_sm.state);
  
  switch( info->meta_sm.state )
  {
    case RMNET_META_SM_RM_CONFIGURING_STATE:
    case RMNET_META_SM_NET_IF_UP_STATE:
    case RMNET_META_SM_RM_RECONFIGURING_STATE:
    case RMNET_META_SM_WAITING_FOR_UM_DOWN_STATE:
    case RMNET_META_SM_UM_CONFIGURING_STATE:
    case RMNET_META_SM_EP_CONFIGURING_STATE:
      rmnet_meta_smi_transition_state(info, RMNET_META_SM_NULL_STATE);
      break;
    case RMNET_META_SM_NULL_STATE:
    {
      /*---------------------------------------------------------------------
        Its error to get this event in all other states.
      ---------------------------------------------------------------------*/
     DS_MSG1(MSG_LEGACY_ERROR,"Unexpected PDN_DOWN_EV in %d state",
                      info->meta_sm.state);
      break;
    }
    default:
    {
      DS_ERROR_FATAL("Bad RmNetSM state!");
      break;
    }
  } /* switch( info->meta_sm.state ) */
} /* rmnet_meta_smi_handle_um_iface_down_ev() */


/*===========================================================================
  FUNCTION RMNET_META_SMI_HANDLE_PDN_UP_EV

  DESCRIPTION
    This function processes RMNET_META_SM_UM_IFACE_UP_EV event.

    This event indicates that network has accepted the packet data session,
    and so the RMNET_META_SM should release the tethered packet session.

  PARAMETERS
    info - pointer to the interface controller's control block

  RETURN VALUE
    None

  DEPENDENCIES
    must !ONLY! be called from command dispatcher
    - state and initialization of the control block is verified there

  SIDE EFFECTS
    None
===========================================================================*/
static void rmnet_meta_smi_handle_pdn_up_ev
(
  rmnet_smi_info_type *  info
)
{
  rmnet_instance_e_type  rmnet_inst;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  rmnet_inst = RMNET_META_SM_INFO_TO_INSTANCE(info);

  DS_MSG2(MSG_LEGACY_ERROR,"Recvd pdn_UP, RMNET inst %d, state %d", 
                  rmnet_inst, info->meta_sm.state );

  /*-------------------------------------------------------------------------
    Um interface controller has [re]established Um interface and put it
    in up state.
  -------------------------------------------------------------------------*/
  switch( info->meta_sm.state )
  {
    case RMNET_META_SM_UM_CONFIGURING_STATE:    
      DS_MSG0(MSG_LEGACY_ERROR,"pdn up ev, transition to RM_CONFIGURING_STATE" );
       rmnet_meta_smi_transition_state (info,
                                       RMNET_META_SM_RM_CONFIGURING_STATE);
   	  break;
    case RMNET_META_SM_RM_CONFIGURING_STATE:
    case RMNET_META_SM_NET_IF_UP_STATE:
    case RMNET_META_SM_RM_RECONFIGURING_STATE:
      DS_MSG0(MSG_LEGACY_ERROR," UM is UP, Ignore UP_EV!");
      break;

    case RMNET_META_SM_WAITING_FOR_UM_DOWN_STATE:  /* Um out of sync w/ RmSm */
     DS_MSG0(MSG_LEGACY_ERROR,"Um interface restored to UP as before rmnet call");
     break;

    /* fall through */
    case RMNET_META_SM_NULL_STATE:         /* event should be deregistered */
    default:
    {
      DS_MSG1(MSG_LEGACY_ERROR,"Invalid RmNETSM state (%d)!", info->meta_sm.state);
      DS_ASSERT(0);
      break;

    }
 } /* rmnet_meta_smi_handle_um_iface_up_ev() */
}

/*===========================================================================
  FUNCTION RMNET_META_SMI_HANDLE_ADDR_CFG_CHANGED_EV

  DESCRIPTION
    This function processes RMNET_META_SM_ADDR_CFG_CHANGED_EV event.

    This event indicates that Um has changed addresses and now the Rm
    is in the process of reconfiguring its address.

  PARAMETERS
    info - pointer to the interface controller's control block

  RETURN VALUE
    None

  DEPENDENCIES
    must !ONLY! be called from command dispatcher
    - state and initialization of the control block is verified there

  SIDE EFFECTS
    None
===========================================================================*/
static void rmnet_meta_smi_handle_addr_cfg_changed_ev
(
  rmnet_smi_info_type *  info
)
{
  rmnet_instance_e_type  rmnet_inst;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  rmnet_inst = RMNET_META_SM_INFO_TO_INSTANCE(info);

  DS_MSG2(MSG_LEGACY_ERROR,"Recvd ADDR_CFG_CHANGED_EV, RMNET inst %d, state %d",
                   rmnet_inst, info->meta_sm.state);
  switch( info->meta_sm.state )
  {
    case RMNET_META_SM_NET_IF_UP_STATE:
    case RMNET_META_SM_RM_CONFIGURING_STATE:
    case RMNET_META_SM_RM_RECONFIGURING_STATE:
    {
      rmnet_meta_smi_transition_state(info,
                                      RMNET_META_SM_RM_RECONFIGURING_STATE);
      break;
    }
    case RMNET_META_SM_UM_CONFIGURING_STATE:
    case RMNET_META_SM_WAITING_FOR_UM_DOWN_STATE:
    case RMNET_META_SM_NULL_STATE:
    {
    /*---------------------------------------------------------------------
        Its error to get this event in all other states.
      ---------------------------------------------------------------------*/
      DS_MSG1(MSG_LEGACY_ERROR,"Unexpected ADDR_CFG_CHANGED_EV in %d state",
                      info->meta_sm.state);
      break;
    }
    default:
    {
      DS_ERROR_FATAL("Bad RmNetSM state!");
      break;
    }
  } /* switch( info->meta_sm.state ) */
} /* rmnet_meta_smi_handle_addr_cfg_changed_ev() */

/*===========================================================================
FUNCTION   RMNET_META_TX_CB()

DESCRIPTION
  This callback function is called when one tehpackest are destined to Rmnet

DEPENDENCIES
  None

 PARAMETERS:
  client_handle:          Rm instance
  dsm_item:               Dsm item
  user_data_prtr         :User data registered during the init with fwk

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void rmnet_meta_tx_cb
(
  ds_fwk_clnt_handle_type    client_handle,
  dsm_item_type              *dsm_item,
  void                      *user_data_ptr
)
{
#ifndef TEST_FRAMEWORK
  sio_stream_id_type    stream_id = SIO_NO_STREAM_ID;
/*-------------------------------------------------------*/  
  DS_ENTER_CRIT_SECTION(&rmnet_rx_pkt_hdlr_crit_section);
  stream_id = rmnet_get_stream_id_from_client(client_handle);
  if (stream_id == SIO_NO_STREAM_ID)
  {
    dsm_free_packet(&dsm_item );
    DS_LEAVE_CRIT_SECTION(&rmnet_rx_pkt_hdlr_crit_section);
    return ;
  }

  sio_transmit(stream_id,dsm_item);
#else
    #error code not present
#endif /* TEST_FRAMEWORK */
  DS_LEAVE_CRIT_SECTION(&rmnet_rx_pkt_hdlr_crit_section);
}

/*===========================================================================
FUNCTION   RMNET_META_TX_NIPD_CB()

DESCRIPTION
  This callback function is called when one the packest are destined to nipd Rmnet

DEPENDENCIES
  None

 PARAMETERS:
  client_handle:          Rm instance
  dsm_item:               Dsm item
  user_data_prtr         :User data registered during the init with fwk

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

static void rmnet_meta_tx_nipd_cb
(
  ds_fwk_clnt_handle_type    client_handle,
  dsm_item_type              *dsm_item,
  void                      *user_data_ptr
)
{
  if(dsm_item != NULL)
  {
    qmi_nipdi_down_link_data_indication( &dsm_item, client_handle );
  }
}

/*===========================================================================
FUNCTION   RMNET_META_SMI_PDN_EV_HDLR_CB()

DESCRIPTION
  This callback function is called when one of the registered ps_pdn events
  occur on Um interface.  Various Um pdn events are registered based on
  call type during the call setup type.  Each pdn event generates
  appropriate rmnet_meta_sm event .

DEPENDENCIES
  None

 PARAMETERS:
  client_handle:          Rm instance
  event           :       PDN event
  event_info      :       PDN info according to the event
  user_data_prtr:    User data registered during the init with fwk

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void rmnet_meta_smi_pdn_ev_hdlr_cb
(  
  ds_fwk_clnt_handle_type    client_handle,
  ds_pdn_event_enum_type     event,
  ds_pdn_event_info_u_type   event_info,
  void                      *user_data_ptr
)
{
  rmnet_smi_info_type         *info = NULL;
  rmnet_smi_ip_info_type      *ip_info = NULL;
  rmnet_instance_e_type        rmnet_inst;
  ds_stats_type               *stats = NULL;
  ds_fwk_s_type               *fwk_inst   = NULL;
  ds_fwk_clnt_info_s_type      clnt_info_ptr ={0};  
  sint15                       stats_return = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  //Check if the call was initiated or not by RMNET
  if(rmnet_meta_sm_in_call(client_handle) == FALSE)
  {
   return;
  }
  info = rmnet_smi_info[client_handle];

  if (info == NULL)
  {
    DS_MSG2(MSG_LEGACY_ERROR,"Recvd iface ev %d from %d",
                   event, client_handle);
    DS_ASSERT(0);
    return;
  }
 
  rmnet_inst = RMNET_META_SM_INFO_TO_INSTANCE(info);
  ip_info = rmnet_get_ip_info_from_inst(info->constants.ip_inst);
  DS_MSG3(MSG_LEGACY_ERROR,"Recvd iface ev %d from %d, RMNET inst %d",
                   event, client_handle, rmnet_inst );
  
  info->last_event = event;
  info->event_info = event_info;
  
  switch( event )
  {
    case PDN_DOWN_EV:
    {
       info->meta_sm.call_end_reason = event_info.pdn_down_info.netdown_reason;
       ds_fwk_get_clnt_info_from_handle(DS_FWK_CLIENT_RMNET,rmnet_inst,
                                                      &clnt_info_ptr);
       if(clnt_info_ptr.fw_inst == NULL)
       {   
          DS_MSG0(MSG_LEGACY_ERROR,"No framework "
              "instance could be found for this call");
       }
       else
       {
         fwk_inst = clnt_info_ptr.fw_inst;
         if(event_info.pdn_down_info.addr_family == IPV4_ADDR)
            stats = DS_FWK_GET_STATS_PTR(fwk_inst,IP_V4,DS_STATS_CLIENT_UM);
         else if(event_info.pdn_down_info.addr_family == IPV6_ADDR)
            stats = DS_FWK_GET_STATS_PTR(fwk_inst,IP_V6,DS_STATS_CLIENT_UM);
         ds_fwk_delete_pdn_sharing_filters(clnt_info_ptr.fw_inst->ds_fwk_index, event_info.pdn_down_info.addr_family);
      }
      if(stats)
      {
         memscpy(&(info->meta_sm.call_stats), sizeof(ds_stats_type),
                    stats,sizeof(ds_stats_type));
         ds_fwk_reset_pdn_stats(fwk_inst,event_info.pdn_down_info.addr_family,&stats_return);
      }

      DS_MSG1(MSG_LEGACY_ERROR,"pdn down, reason [%d], posting RMNET_META_SM_PDN_DOWN_EV",
              event_info.pdn_down_info.netdown_reason );
     (void)rmnet_meta_sm_post_event( RMNET_META_SM_INFO_TO_INSTANCE(info),
                                  RMNET_META_SM_PDN_DOWN_EV );
      break;
    }
    case PDN_UP_EV:
    {
      /*---------------------------------------------------------------------
        Traffic channel on UM has come up. This event is used for Relay and
        network model call setup.
      ---------------------------------------------------------------------*/
      (void)rmnet_meta_sm_post_event( RMNET_META_SM_INFO_TO_INSTANCE(info),
                                  RMNET_META_SM_PDN_UP_EV );
      break;
    }
    case PDN_ADDR_CHANGED_EV:
    case PDN_EXTENDED_IP_CONFIG_EX_EV:
    {
       (void)rmnet_meta_sm_post_event( RMNET_META_SM_INFO_TO_INSTANCE(info),
                                 RMNET_META_SM_ADDR_CFG_CHANGED_EV
                                );
       break;
    }
    case PDN_PREFIX_UPDATE_EV:
    {
      if(event_info.prefix_info.kind == DS_PREFIX_ADDED)
      {
        (void)rmnet_meta_sm_post_event( RMNET_META_SM_INFO_TO_INSTANCE(info),
                                  RMNET_META_SM_ADDR_CFG_CHANGED_EV
                                );
      }
      break;
    }
    case BEARER_FLOW_ENABLED_EV:
    {
      DS_MSG1(MSG_LEGACY_ERROR,"Recieved flow enabled event enable the ps_rm signal %d", event);
      if(ip_info != NULL)
      {
        PS_RM_SET_SIGNAL(ip_info->rmnet_ul_sig);
      }
      break;
    }
    case BEARER_FLOW_DISABLED_EV:
    {
      DS_MSG2(MSG_LEGACY_HIGH,"Recieved flow disabled event %d, flow mask 0x%x ",
              event,event_info.bearer_flow_mask);
      if((event_info.bearer_flow_mask & DS_BEARER_MEM_FLOW_CTL_MASK) != 0)
      {
          DS_MSG2(MSG_LEGACY_HIGH,"Recieved flow disabled event %d, flow mask 0x%x ",
          event,event_info.bearer_flow_mask);
          dsm_empty_queue(&info->xport_info->rx_wmk);
      }
      break;
    }
    case PDN_WWAN_TECH_SUSPENSION_EV:
    {
      if(ip_info != NULL)
      {  
        rmnet_logical_xport_set_rx_flow(&(ip_info->xport_info), event_info.tech_suspension_status);
      }
      break;
    }
    default:
    {
      DS_MSG1(MSG_LEGACY_ERROR,"Unregistered event %d recvd, ignoring", event);
      break;
    }
  } /* switch( event ) */
} /* rmnet_meta_smi_um_iface_ev_hdlr_cb() */

/*===========================================================================
  FUNCTION RMNET_META_SMI_TRANSITION_STATE()

  DESCRIPTION
    This function performs state transition actions.

  PARAMETERS
    new_state: the state to transition to

  RETURN VALUE
    None

  DEPENDENCIES
    Its the caller's responsibility that it initiates only valid state
    transitions. This function does not check if a state transition is
    valid or not.

  SIDE EFFECTS
    None
===========================================================================*/
static void rmnet_meta_smi_transition_state
(
  rmnet_smi_info_type         *info,
  rmnet_meta_smi_state_type   new_state
)
{
  int retval;
  rmnet_instance_e_type  rmnet_inst;
  rmnet_smi_ip_info_type        *ip_info = NULL;
  ds_policy_info_type            policy_info ={0};  
  ds_fwk_response_info_s_type    resp_info={0};
  ds_pdn_event_info_u_type       event_info;
  uint8                          ip_inst;  
  ds_fwk_clnt_info_s_type        clnt_info_ptr ={0};   
  ds_bearer_mgr_s               *def_bearer = NULL;	  
  ds_fwk_s_type		        *fwk_inst = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  rmnet_inst = RMNET_META_SM_INFO_TO_INSTANCE(info);
  ip_info = rmnet_smi_ip_info[info->constants.ip_inst];
  ip_inst = info->constants.ip_inst;
  DS_ASSERT((int)ip_info); 
  memset(&event_info,0,sizeof(ds_pdn_event_info_u_type));
  DS_MSG3(MSG_LEGACY_ERROR,"RMNET_META_SM inst %d, state goes %d to %d",
                   rmnet_inst, info->meta_sm.state, new_state);

  info->meta_sm.state = new_state;
  event_info = info->event_info;
  /*-------------------------------------------------------------------------
    Perform action associated with entering into new state.
  -------------------------------------------------------------------------*/
  switch( new_state )
  {
    /*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                        RMNET_META_SM_NULL_STATE
    = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/
    case RMNET_META_SM_NULL_STATE:
    {    
      DS_ENTER_CRIT_SECTION(&rmnet_rx_pkt_hdlr_crit_section);
      ip_info->call_count--;
      DS_MSG1(MSG_LEGACY_ERROR,"RMNET ip_info call_count in NULL state %d ",
             ip_info->call_count);

      info->has_flow = FALSE;

      ip_info->rx_flow_count =
             ip_info->info_v4->has_flow +
             ip_info->info_v6->has_flow +
             ip_info->info_nipd->has_flow;      
      /*---------------------------------------------------------------------
        Pkt call is terminating, Perform all the necessary cleanup
      ---------------------------------------------------------------------*/

      /*clear WM queue only if this is last SM to go down*/
      if(( 0 == ip_info->rx_flow_count )&&(ip_info->call_count ==0))
      {
        rmnet_logical_xport_clear_wmk_queues(info->xport_info);
		rmnet_reset_sio_wm(ip_info, &(ip_info->xport_info));
      }
  /* deregister RX function only if this is last SM to go down*/
   if(( 0 == ip_info->rx_flow_count )&&(ip_info->call_count == 0))
   {
      /*---------------------------------------------------------------------
        Deregister rmnet_sio rx function
      ---------------------------------------------------------------------*/
      rmnet_logical_xport_set_rx_cb( info->xport_info, NULL, info );  //To check

     if (info->meta_sm.proc_id == RMNET_PROC_ID_LAPTOP1)
     {
       rmnet_phys_xport_dec_teth_call_cnt(ip_info->xport_info.phys_xport);
     }
     /*---------------------------------------------------------------------
       Close RmNet xport if no call is active
     ---------------------------------------------------------------------*/
     if (rmnet_meta_smi_in_call_cb(ip_info) == FALSE)
     {
       ds_qmap_clean_cmd(info->xport_info);
       rmnet_logical_xport_close(&ip_info->xport_info);
     }
   
     if(ip_info->rmnet_ul_sig < RMNET_UL_MAX_SIGNALS)
      {
         rmnet_ul_sig_in_use_tbl[ip_info->rmnet_ul_sig] = FALSE;
      }

     ip_info->rmnet_ul_sig = PS_RM_MAX_SIGNALS;

    if(ip_info->xport_info.mux_id == TETHERED_CALL_MUXID)
       rmnet_call_info[MAX_MUX_ID_POOL_SIZE -1].in_use = FALSE;
    else if(ip_info->xport_info.mux_id < MAX_MUX_ID_POOL_SIZE)
    {
      rmnet_call_info[ip_info->xport_info.mux_id].in_use = FALSE;      
      memset(rmnet_call_info[ip_info->xport_info.mux_id].apn ,0,
             sizeof(rmnet_call_info[ip_info->xport_info.mux_id].apn)); 
      rmnet_call_info[ip_info->xport_info.mux_id].null_apn = FALSE;      

    }
    ip_info->ep_cfg_done = FALSE;
   }

  info->meta_sm.proc_id      = RMNET_PROC_ID_MAX;
#ifdef FEATURE_DATA_QCRMCALL
      // Update qcrmcall state
      rmnet_meta_smi_update_qcrmcall_state(info);
#endif /* FEATURE_DATA_QCRMCALL */
      event_info.pdn_down_info.netdown_reason = info->meta_sm.call_end_reason;
      memset(&(info->meta_sm.net_params),
               0x0,
              sizeof(rmnet_meta_sm_network_info_type) );
      
      rmnet_invoke_wds_cb(rmnet_inst,PDN_DOWN_EV,event_info);
      //Free all the rmnet instances bound to this ip_info as all the SM are down
      if((0 == ip_info->rx_flow_count)&&(ip_info->call_count == 0))
      {
        rmnet_logical_xport_unbind(info->xport_info);		
        dsm_queue_destroy(&info->xport_info->rx_wmk);		
        dsm_queue_destroy(&info->xport_info->tx_wmk);
        DS_SYSTEM_HEAP_MEM_FREE(ip_info->info_v4);
        DS_SYSTEM_HEAP_MEM_FREE(ip_info->info_v6);
        DS_SYSTEM_HEAP_MEM_FREE(ip_info->info_nipd);
        DS_SYSTEM_HEAP_MEM_FREE(ip_info);
        rmnet_smi_ip_info[ip_inst] =NULL;
        rmnet_smi_info[ip_inst*3] = NULL;
        rmnet_smi_info[(ip_inst*3)+1] = NULL;
        rmnet_smi_info[(ip_inst*3)+2] = NULL;
      }	  
      DS_LEAVE_CRIT_SECTION(&rmnet_rx_pkt_hdlr_crit_section);
      break;
    }

    /*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                        RMNET_META_SM_UM_CONFIGURING_STATE
    = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/
    case RMNET_META_SM_EP_CONFIGURING_STATE:
    {
      if(ip_info->ep_cfg_done == FALSE)
      {
	    /* Open RmNet xport */
        if (!rmnet_logical_xport_open(&ip_info->xport_info))
        {
          (void)rmnet_meta_sm_post_event( RMNET_META_SM_INFO_TO_INSTANCE(info),
                                      RMNET_META_SM_PDN_DOWN_EV );
        }
      }
      else
      {	  	
        rmnet_meta_smi_transition_state(info, RMNET_META_SM_UM_CONFIGURING_STATE);
      }
      break;  
    }
    case RMNET_META_SM_UM_CONFIGURING_STATE:
    {
      /*---------------------------------------------------------------------
        Bring up the Um interface
      ---------------------------------------------------------------------*/
#ifdef TEST_FRAMEWORK
      #error code not present
#endif
      /* Check if it is the first tethered call coming up */
      if (info->meta_sm.net_params.call_type == RMNET_CALL_TYPE_LAPTOP) 
      {
        rmnet_phys_xport_inc_teth_call_cnt(ip_info->xport_info.phys_xport);
      }
      DS_MSG2(MSG_LEGACY_HIGH,"RMNET data format UL [%d] DL [%d]",
                   RMNET_UL_DATA_AGG(info), RMNET_DL_DATA_AGG(info));

      memscpy(policy_info.name, DS_SYS_MAX_APN_LEN,
	  	       info->meta_sm.net_params.apn.name,
	  	       strlen(info->meta_sm.net_params.apn.name));

     /*---------------------------------------------------------------------
      Formulate the Rm network interface's 802.3 MAC address
  
      Vendor ID (most significant 3 bytes) is set to the IEEE assigned
      Vendor ID for the customer.
 
      TODO - add an NV item for this.

      Device ID (least significant 3 bytes) is set based on the media
      type and underlying QMI link ID.
 
      byte	   description
      ----	   -----------
      05 MSB   \
      04	   +  Vendor ID = hard coded MSM Net device VID (TBD)
      03	   /
      02	   media type => see header file rmnet_mac_media_e_type
      01	   rmnet dual ip instance
      00	   mux id
     ---------------------------------------------------------------------*/
#define QMI_ETH_VENDOR_ID  (0x0250F3) /* To be assigned */

#define QMI_ETH_DEVICE_ID_USB  (0x00)
      info->hw_addr[0] = (QMI_ETH_VENDOR_ID >> 16) & 0xFF;
      info->hw_addr[1] = (QMI_ETH_VENDOR_ID >> 8 ) & 0xFF;
      info->hw_addr[2] = (QMI_ETH_VENDOR_ID		) & 0xFF;
      info->hw_addr[3] = QMI_ETH_DEVICE_ID_USB;
      info->hw_addr[4] = (info->constants.ip_inst) & 0xFF;
      info->hw_addr[5] = (RMNET_MUX_ID(info)) & 0xFF;
      DS_MSG2(MSG_LEGACY_HIGH,"Set Rm MAC address to 0x%x:0x%x",
              QMI_ETH_VENDOR_ID, 
              QMI_ETH_DEVICE_ID_USB << 16 | 
              (info->constants.ip_inst << 8 | RMNET_MUX_ID(info)));

      policy_info.length = strlen(info->meta_sm.net_params.apn.name);
      policy_info.family = info->meta_sm.net_params.ip_family_pref;
      policy_info.mo_excp_data_cap = info->meta_sm.net_params.mo_excp_data_cap;
      if (info->meta_sm.net_params.auth_param_valid)
      {
        policy_info.authentication_params.auth_param_valid = \
             info->meta_sm.net_params.auth_param_valid;

        policy_info.authentication_params.auth_params.auth_type = \
                     info->meta_sm.net_params.auth_params.auth_type;

        strlcpy(policy_info.authentication_params.auth_params.username,
                info->meta_sm.net_params.auth_params.username,
                DS_PROFILE_MAX_AUTH_STRING_LEN+1);

        strlcpy(policy_info.authentication_params.auth_params.password,
                info->meta_sm.net_params.auth_params.password,
                DS_PROFILE_MAX_AUTH_STRING_LEN+1);
      }
      ip_info->call_count++;
      DS_MSG1(MSG_LEGACY_HIGH,"RMNET ip_info call_count %d ",
                   ip_info->call_count);
      
      retval = ds_fwk_bringup_pdn_by_policy(DS_FWK_CLIENT_RMNET,
                                            policy_info,
                                            RMNET_META_SM_INFO_TO_INSTANCE(info),
                                            &resp_info
                                            );
     
        /*-------------------------------------------------------------------
          Bringup in progress.
        -------------------------------------------------------------------*/
        DS_MSG1(MSG_LEGACY_HIGH,"pdn bringup api returned retval is  %d ",
                        retval);
      break;
    }

	/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
	                RMNET_META_SM_RM_CONFIGURING_STATE
    = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

	case RMNET_META_SM_RM_CONFIGURING_STATE:
	{
	  info->has_flow = TRUE;
 
     ip_info->rx_flow_count =
        ip_info->info_v4->has_flow +
        ip_info->info_v6->has_flow;

    /* if this is the first SM to start call then register call backs and enable flow*/
    /*---------------------------------------------------------------------
        Setup the SIO wm
     ---------------------------------------------------------------------*/
      if(rmnet_setup_sio_wm(ip_info , info) !=0 )
      {
       (void)rmnet_meta_sm_post_event(RMNET_META_SM_INFO_TO_INSTANCE(info),
                                 RMNET_META_SM_RM_LINK_DOWN_EV);
       break;
      }
	//Post an event to Rmnet Meta SM to move to RMNET_META_SM_NET_IF_UP_STATE 
      rmnet_meta_smi_transition_state(info, RMNET_META_SM_NET_IF_UP_STATE);
      break;
    }

    /*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                        RMNET_META_SM_NET_IF_UP_STATE
    = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/
    case RMNET_META_SM_NET_IF_UP_STATE:
    {
      /*---------------------------------------------------------------------
        This is the common state for data connection up.  TE has configured
        its IP address at this point
      ---------------------------------------------------------------------*/
       DS_MSG0(MSG_LEGACY_HIGH,"Host has configured IP address");
#ifdef FEATURE_DATA_QCRMCALL
      // Update qcrmcall state
      rmnet_meta_smi_update_qcrmcall_state(info);
#endif /* FEATURE_DATA_QCRMCALL */

     rmnet_invoke_wds_cb(rmnet_inst,PDN_UP_EV,event_info);
     //Send flow enable to A2 by checking with MH if flow is enabled 
     ds_fwk_get_clnt_info_from_handle(
                            DS_FWK_CLIENT_RMNET,rmnet_inst,&clnt_info_ptr);

     if(clnt_info_ptr.fw_inst == NULL)
     {   
       DS_MSG0(MSG_LEGACY_ERROR,"No framework instance could be found for this call");
     }
     else
     {
       fwk_inst = clnt_info_ptr.fw_inst;
       
       def_bearer = ds_fwk_get_default_bearer(fwk_inst,(ip_addr_enum_type)info->meta_sm.net_params.ip_family_pref);
       if(def_bearer != NULL)
       {
         if(!ds_bearer_mgr_is_tx_flow_enabled_ex(def_bearer,FALSE))
         {
           DS_MSG0(MSG_LEGACY_HIGH,"Moving to UP state but flow is disabled at MH " 
                "state");
         }
         else
         {
           rmnet_logical_xport_set_rx_flow(&(ip_info->xport_info), FALSE);
         }
       }
     }
     break;
    }

    /*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                        RMNET_META_SM_RM_RECONFIGURING_STATE
    = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/
    case RMNET_META_SM_RM_RECONFIGURING_STATE:
    {    
      rmnet_invoke_wds_cb(rmnet_inst,info->last_event,event_info);
      DS_MSG0(MSG_LEGACY_HIGH,"Moving to UP state after calling wds callback in RM RECONFIGURING" 
              "state");
      info->meta_sm.state = RMNET_META_SM_NET_IF_UP_STATE;
      ds_fwk_get_clnt_info_from_handle(
                              DS_FWK_CLIENT_RMNET,rmnet_inst,&clnt_info_ptr);
     if(clnt_info_ptr.fw_inst == NULL)

     {
       DS_MSG0(MSG_LEGACY_ERROR,"No framework instance could be found for this call");
     }
     else
     {
       fwk_inst = clnt_info_ptr.fw_inst;
        
       def_bearer = ds_fwk_get_default_bearer(fwk_inst,(ip_addr_enum_type)info->meta_sm.net_params.ip_family_pref);
       if(def_bearer != NULL)
       {
         if(!ds_bearer_mgr_is_tx_flow_enabled_ex(def_bearer,FALSE))
         {
           DS_MSG0(MSG_LEGACY_HIGH,"Moving to UP state but flow is disabled at MH " 
                "state");
         }
         else
         {
           rmnet_logical_xport_set_rx_flow(&(ip_info->xport_info), FALSE);
         }
       }
     }
     break;
    }

    /*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                        RMNET_META_SM_WAITING_FOR_UM_DOWN_STATE
    = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/
    case RMNET_META_SM_WAITING_FOR_UM_DOWN_STATE:
    {
      /*---------------------------------------------------------------------
          Since TE has terminated the call, we need to cleanup Um before
          completely terminating the call.  Tear it down now.
          If tear down cmd fails, Um is already be down. 
      -----------------------------------------------------------------------*/
      resp_info.ip_type = info->meta_sm.net_params.ip_family_pref;
	  retval = ds_fwk_tear_down_pdn(DS_FWK_CLIENT_RMNET,
                           RMNET_META_SM_INFO_TO_INSTANCE(info),
                           &resp_info); 
       
      DS_MSG1(MSG_LEGACY_HIGH,"Fwk Teardown returned retval %d",retval);
        
      break;
    }
    default:
    {
      DS_MSG1(MSG_LEGACY_ERROR,"Invalid state %d", new_state);
      DS_ASSERT(0);
    }
  } /* switch(new state) */
} /* rmnet_meta_smi_transition_state() */
/*===========================================================================
  FUNCTION RMNET_META_SM_PROCESS_DTR_CHANGED_CMD()

  DESCRIPTION
    This function processes a RmNet RmSM DTR event cmd.

  PARAMETERS
    cmd_ptr:  serving task command buffer containing the RmNet sm
              event information

  RETURN VALUE
    None

  DEPENDENCIES
    RM State Machine should have already been initialized

  SIDE EFFECTS
    None
===========================================================================*/
static void rmnet_meta_smi_process_dtr_changed_cmd
(
  rmnet_smi_info_type  * info,
  boolean                dtr_asserted
)
{
  rmnet_instance_e_type  rmnet_instance;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_ASSERT ((int)info);
  rmnet_instance = RMNET_META_SM_INFO_TO_INSTANCE(info);
  DS_ASSERT(rmnet_instance < RMNET_INSTANCE_MAX);

  DS_MSG2(MSG_LEGACY_ERROR,"DTR or binding change for RMNET inst [%d] DTR [%d]",
                   rmnet_instance, dtr_asserted);

  /*-------------------------------------------------------------------------
    Handle DTR state change.

    If DTR drops, end the call by posting RMNET_META_SM_LINK_DOWN_EV, and tell
    the QMI module that the link is down.
    If DTR goes high, check if autoconnect is enabled.  If so, post
    RM_DEV_START_PKT_EV to start a new call.
  -------------------------------------------------------------------------*/
  if ( FALSE == dtr_asserted )
  {
    if(info->meta_sm.dtr_is_high == FALSE)
    {
      DS_MSG1(MSG_LEGACY_ERROR,"Ignoring DTR low, RMNET inst %d", rmnet_instance);
      return;
    }

    info->meta_sm.dtr_is_high = FALSE;
    if (info->meta_sm.state != RMNET_META_SM_NULL_STATE)
    {
      DS_MSG1(MSG_LEGACY_ERROR,"DTR dropped - end packet call, RMNET inst %d",
                       rmnet_instance );
      (void)rmnet_meta_sm_post_event(rmnet_instance, RMNET_META_SM_RM_LINK_DOWN_EV);
    }
  }
  else
  {
    if(info->meta_sm.dtr_is_high == TRUE)
    {
      DS_MSG0(MSG_LEGACY_ERROR,"Ignoring DTR High");
      return;
    }
    info->meta_sm.dtr_is_high = TRUE;
  }
} /* rmnet_meta_smi_process_dtr_changed_cmd() */

/*===========================================================================
FUNCTION RMNET_META_SMI_PROCESS_QOS_HDR

DESCRIPTION
  Processes QOS header

PARAMETERS
  user_data     -  user data passed to lan_llc_start(),
                   contains rmnet_meta_sm info ptr

RETURN VALUE


DEPENDENCIES


SIDE EFFECTS
  None.

===========================================================================*/
#if 0
void rmnet_meta_smi_process_qos_hdr
(
  void *                user_data,
  dsm_item_type *       pkt_ptr,
  ps_meta_info_type_ex  ** meta_info_ex_ptr_ptr,
  rmnet_meta_smi_qos_hdr_type * qos_hdr
)
{
  rmnet_smi_info_type    * info = NULL;
  ps_tx_meta_info_type * tx_meta_info_ptr;
  ps_meta_info_type_ex * meta_info_ex_ptr;
  uint8                  octet_val = 0;
  struct ip              pkt_ip4_hdr = {0,};
  rmnet_smi_info_type  rmnet_inst;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(NULL == user_data)
  {
    LOG_MSG_ERROR_0("rmnet_meta_smi_process_qos_hdr(): user_data ptr is NULL");
    return ;
  }

  info = (rmnet_smi_info_type *) user_data;
  rmnet_inst = RMNET_META_SM_INFO_TO_INSTANCE(info);

  /*-------------------------------------------------------------------------
    QoS enabled
  -------------------------------------------------------------------------*/

  if(qos_hdr->flow_ptr != NULL)
  {
    /* don't assert because of race condition */
    //DS_ASSERT(PS_FLOW_IS_VALID(qos_hdr.flow_ptr));

    LOG_MSG_INFO3_2("got pkt on qos flow 0x%x, RMNET inst %d",
                    qos_hdr->flow_ptr, rmnet_inst);

    /* always fwd on default if flow is !activated */
    if(PS_FLOW_GET_STATE(qos_hdr->flow_ptr) != FLOW_ACTIVATED)
    {
      LOG_MSG_INFO3_0("fwding pkt on default flow");
      qos_hdr->flow_ptr = PS_IFACEI_GET_DEFAULT_FLOW(info->um_iface_ptr);
    }
  }
  else
  {
    LOG_MSG_INFO3_1("got pkt on default flow, RMNET inst %d", rmnet_inst);
    /* if 0 - then get default flow */
    qos_hdr->flow_ptr = PS_IFACEI_GET_DEFAULT_FLOW(info->um_iface_ptr);
  }

  /* NOT checking whether TX flow is ENABLED for sec/pri..
      assumption: if any flow is TX_DISABLED, driver gets the ind and stops
      sending on that flow. Any packets driver had already sent before
      processing the ind can be queued on flow wmk
      verify HW and DNE values ok for the above */

  /* generate meta_info - TO DO caching */

  /* Since packet needs to be routed to Um Iface, its better we create TX meta info
     directly here */
  PS_TX_META_INFO_GET_ALL(tx_meta_info_ptr);

  if ((tx_meta_info_ptr == NULL ) ||
      (PS_TX_META_GET_RT_META_INFO_PTR(tx_meta_info_ptr) == NULL))
  {
    DS_ERROR_FATAL("Cannot allocate meta_info");
    return ;
  }

  PS_TX_META_SET_ROUTING_CACHE(tx_meta_info_ptr, info->um_iface_ptr);

  if ( FALSE != RMNET_QOS_ENABLED(info) )
  {
    PS_TX_META_SET_FILTER_RESULT(tx_meta_info_ptr,
                               IP_FLTR_CLIENT_QOS_OUTPUT,
                               qos_hdr->flow_ptr);
  }

  /* If the DOS flag is set, propagate the information to the PS layer
   * by setting the appropriate flag in meta info. Use the IP_ID from
   * the packet as the DOS ack handle. */
  if (qos_hdr->flags & ((uint8)RMNET_QOS_FLAG_TYPE_DOS))
  {
    /* Extract the IP version. DOS is supported only for IPv4 packets. */
    if ( (sizeof(uint8) == dsm_extract( pkt_ptr,
                                        0,
                                        &octet_val,
                                        sizeof(uint8))) &&
         (IP_V4 == (ip_version_enum_type)(octet_val >> 4)) )
    {
      /* Peek and extract the IP_ID from the IPv4 header. */
      if (IPLEN == dsm_extract( pkt_ptr,
                                0,
                                (void*)&pkt_ip4_hdr,
                                IPLEN ))
      {
        DS_ASSERT(tx_meta_info_ptr->pkt_meta_info_ptr != NULL);
        tx_meta_info_ptr->pkt_meta_info_ptr->tx_flags |= MSG_FAST_EXPEDITE;
        tx_meta_info_ptr->pkt_meta_info_ptr->dos_ack_handle = pkt_ip4_hdr.id;
        LOG_MSG_INFO2_1( "DOS enabled on pkt, dos_ack_handle:0x%x",
                         tx_meta_info_ptr->pkt_meta_info_ptr->dos_ack_handle);
      }
      else
      {
        LOG_MSG_INFO1_0("Invalid IP header, DOS not enabled");
      }
    }
    else
    {
      LOG_MSG_INFO1_0("Invalid IP version, DOS not enabled");
    }
  }

  /* Update meta_info_ex_ptr : create meta_info_ex_ptr and embed
  tx_meta_info_ptr inside */
  PS_META_INFO_GET_EX(meta_info_ex_ptr);
  if (meta_info_ex_ptr == NULL )
  {
    DS_ERROR_FATAL("Cannot allocate ps_meta_info_type_ex");
    return ;
  }

  meta_info_ex_ptr->tx_meta_info_ptr = tx_meta_info_ptr;
  (*meta_info_ex_ptr_ptr) = meta_info_ex_ptr;
} /* rmnet_meta_smi_process_qos_hdr() */

#endif

/*===========================================================================
  FUNCTION RMNET_META_SM_INIT_DUAL_IP_INSTANCE

  DESCRIPTION
    Open the specified RmNet instance. This function runs in DCC task and posts
    the result to QMUX that runs in qmi_modem task.

  PARAMETERS
    instance : instance of RmNET to open
    port_id :  SIO port identifier

  RETURN VALUE
    void

  SIDE EFFECTS
    none
===========================================================================*/
static boolean rmnet_meta_sm_init_ip_instance
(
  uint8                  ip_inst
)
{
  rmnet_smi_ip_info_type * ip_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( ip_inst >= RMNET_IP_MAX ||
          rmnet_smi_ip_info[ip_inst] != NULL )
  {
    DS_ASSERT(0);
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Allocate the dual IP info structure
  -------------------------------------------------------------------------*/
  DS_SYSTEM_HEAP_MEM_ALLOC(ip_info, sizeof(rmnet_smi_ip_info_type),
                             rmnet_smi_ip_info_type*);
  if (ip_info == NULL)
  {
    return FALSE;
  }

  memset(ip_info, 0, sizeof(rmnet_smi_ip_info_type));

  /*-------------------------------------------------------------------------
    Init rmnet xport
  -------------------------------------------------------------------------*/
  if (!rmnet_logical_xport_init(&ip_info->xport_info,
                    rmnet_meta_smi_xport_init, ip_info))
  {
    DS_MSG0(MSG_LEGACY_ERROR,"Failed to init rmnet xport");
    DS_SYSTEM_HEAP_MEM_FREE(ip_info);
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Init rmnet instances
  -------------------------------------------------------------------------*/
  if ( ! rmnet_meta_smi_init((ip_inst * 3)) ||
       ! rmnet_meta_smi_init((ip_inst * 3) + 1) ||
       ! rmnet_meta_smi_init((ip_inst * 3) + 2 ))
  {
    DS_MSG1(MSG_LEGACY_ERROR,"Failed to open RMNET ip insts %d", ip_inst);
    DS_SYSTEM_HEAP_MEM_FREE(ip_info);
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Initialize other structures
  -------------------------------------------------------------------------*/
  ip_info->info_v4 = rmnet_smi_info[ip_inst * 3];
  ip_info->info_v6 = rmnet_smi_info[(ip_inst * 3) + 1];  
  ip_info->info_nipd = rmnet_smi_info[(ip_inst * 3) + 2];

  if (ip_info->info_v4 == NULL || ip_info->info_v6 == NULL || (ip_info->info_nipd == NULL))
  {
    DS_ASSERT(0);
    DS_SYSTEM_HEAP_MEM_FREE(ip_info);
    return FALSE;
  }

  ip_info->info_v4->xport_info = &ip_info->xport_info;
  ip_info->info_v6->xport_info = &ip_info->xport_info;  
  ip_info->info_nipd->xport_info = &ip_info->xport_info;

  ip_info->info_v4->has_flow = FALSE;
  ip_info->info_v6->has_flow = FALSE;  
  ip_info->info_nipd->has_flow = FALSE;
  ip_info->rmnet_ul_sig  = PS_RM_MAX_SIGNALS;
  
  ip_info->rx_flow_count = 0;
  ip_info->rx_pkt        =NULL;
  rmnet_smi_ip_info[ip_inst] = ip_info;

 return TRUE;
} /* rmnet_meta_sm_init_dual_ip_instance() */


/**************************************************************************
            EFS Configuration and Parsing Functions and Utilities
***************************************************************************/

/* ===========================================================================
FUNCTION RMNET_META_SM_GET_EFS_CONFIG_PARAMS

DESCRIPTION
  This function reads the file rmnet_config.txt from the efs root directory and
  parses it and saves the information. If the file is not present,
  or the content of the file is corrupted, creates a default file with
  just autoconnect to be INVALID so that the Autoconnect can be used from NV
if
  present.

PARAMETERS
  config_params : Output parameters containing the parsed EFS configuration.

RETURN VALUE
  None.

DEPENDENCIES
  None

SIDE EFFECTS
  None.

===========================================================================*/
static void rmnet_meta_sm_get_efs_config_params
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*------------------------------------------------------------------------
   * Global config
  -------------------------------------------------------------------------*/
  memset(&rmnet_efs_global_config_params,
                     0, sizeof(rmnet_efs_global_config_params));

  if (qmi_nv_read(QMI_NV_ITEM_RMNET_GLOBAL_CONFIG, 0, 0,
                  &rmnet_efs_global_config_params,
                  sizeof(rmnet_efs_global_config_params)) != QMI_NV_STATUS_OK)
  {
    /* Initialize global config params to defaults */
    rmnet_efs_global_config_params.ul_agg_prot_mask =
               (RMNET_GLOBAL_EFS_DATA_FORMAT_MBIM_MASK |
                RMNET_GLOBAL_EFS_DATA_FORMAT_QMAP_MASK |
                RMNET_GLOBAL_EFS_DATA_FORMAT_QMAP_V3_MASK);

    rmnet_efs_global_config_params.dl_agg_prot_mask =
               (RMNET_GLOBAL_EFS_DATA_FORMAT_MBIM_MASK |
                RMNET_GLOBAL_EFS_DATA_FORMAT_QMAP_MASK |
                RMNET_GLOBAL_EFS_DATA_FORMAT_QMAP_V3_MASK);
  }
#ifdef FEATURE_DATA_QCRMCALL
  /*------------------------------------------------------------------------
   * $QCRMCALL table
  -------------------------------------------------------------------------*/
  memset(rmnet_qcrmcall_inst, 0, sizeof(rmnet_qcrmcall_inst));
#endif /* FEATURE_DATA_QCRMCALL */


} /* rmnet_meta_sm_get_efs_config_params() */

/*===========================================================================
FUNCTION RMNET_META_SM_BIND

DESCRIPTION
  This function binds a logical rmnet instance to a physical port

PARAMETERS
  ep_id:       physical port end point id
  mux_id:      logical rmnet mux id

DEPENDENCIES
  None

RETURN VALUE
  TRUE : success
  FALSE: failure

SIDE EFFECTS
  None.
===========================================================================*/
boolean rmnet_meta_sm_bind
(
  uint32          ep_id,
  uint8           mux_id
)
{
  rmnet_phys_xport_type  * phys_xport;
  uint8                      i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (ep_id == 0)
  {
    return FALSE;
  }
  /*-------------------------------------------------------------------------
   * Get the phys xport
  -------------------------------------------------------------------------*/
  phys_xport = rmnet_phys_xport_from_ep_id(ep_id);
  if (phys_xport == NULL)
  {
    DS_MSG1(MSG_LEGACY_ERROR,"Unknown EP 0x%x", ep_id);
    return  FALSE;
  }

  /*------------------------------------------------------------------------
   * Find existing rmnet instance
  -------------------------------------------------------------------------*/
  for (i = 0; i < RMNET_IP_MAX; i++)
  {
    if (rmnet_smi_ip_info[i] != NULL &&
        rmnet_smi_ip_info[i]->xport_info.mux_id == mux_id)
    {
      if(rmnet_smi_ip_info[i]->xport_info.ep_id != ep_id)
	  	return FALSE;
	  
      DS_MSG2(MSG_LEGACY_ERROR,"Rmnet EP 0x%x mux_id %d already bound", ep_id, mux_id);
      return TRUE;
    }
  }
  
  /*-------------------------------------------------------------------------
   * Allocate a new rmnet instance in the mux section
  -------------------------------------------------------------------------*/
 
   if(mux_id == TETHERED_CALL_MUXID)
   {
     if(rmnet_smi_ip_info[MAX_MUX_ID_POOL_SIZE -1] == NULL)
      {
	 	i = (RMNET_IP_MAX -1);
    }
    }
   else 
     i = (mux_id -1);

  if (i >= RMNET_IP_MAX)
  {
    DS_MSG0(MSG_LEGACY_ERROR,"No available RMNET instance");
    return FALSE;
  }

  if (rmnet_smi_ip_info[i] == NULL)
  {
    if ( !rmnet_meta_sm_init_ip_instance(i) )
    {
      DS_MSG1(MSG_LEGACY_ERROR,"RMNET dual IP inst %d open failed", i);
      return FALSE;
    }
  }

  /*-------------------------------------------------------------------------
   * Bind to phys xport
  -------------------------------------------------------------------------*/
  if ( !rmnet_logical_xport_bind(
                  &rmnet_smi_ip_info[i]->xport_info,
                  phys_xport,
                  mux_id) )
  {
    DS_MSG2(MSG_LEGACY_ERROR,"Unable to bind RMNET EP 0x%x mux_id %d", ep_id, mux_id);
    return FALSE;
  }
  else
  {
    DS_MSG3(MSG_LEGACY_ERROR,"Rmnet EP 0x%x mux_id %d bound to dual IP inst %d",
                    ep_id, mux_id, i);
  }

  /*-------------------------------------------------------------------------
   * Get manual DNS settings
  -------------------------------------------------------------------------*/
  if (rmnet_set_manual_dns_settings(rmnet_smi_ip_info[i]) == FALSE)
  {
    DS_MSG0(MSG_LEGACY_ERROR,"Error setting manual dns settings");
  }
  return TRUE;

} /* rmnet_meta_sm_bind() */

/*===========================================================================

FUNCTION RMNET_INST_FROM_EP_AND_MUX_ID

DESCRIPTION
  This function retrieves rmnet instance from ep_id and mux_id

PARAMETERS
  ep_id:       physical port end point id
  mux_id:      logical rmnet mux id
  is_default:  is default instance

DEPENDENCIES
  None

RETURN VALUE
  rmnet instance number
  RMNET_INSTANCE_MAX if failure

SIDE EFFECTS
  None.
===========================================================================*/
rmnet_instance_e_type rmnet_inst_from_ep_and_mux_id
(
  uint32     ep_id,
  uint8      mux_id,
  boolean    is_default,
  uint8      ip_type
)
{
  rmnet_smi_info_type          *info;
  rmnet_instance_e_type         rmnet_inst = RMNET_INSTANCE_MAX;
  int                            i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  for (i = 0; i < RMNET_INSTANCE_MAX; i++)
  {
    info = rmnet_smi_info[i];
    if ( info != NULL &&
         info->xport_info->mux_id == mux_id &&
         info->xport_info->ep_id == ep_id &&
         info->meta_sm.net_params.ip_family_pref == ip_type)
    {
      rmnet_inst = info->constants.rmnet_inst;      
      DS_MSG1(MSG_LEGACY_HIGH,"rmnet_inst_from_ep_and_mux_id rmnet inst %d", rmnet_inst);
      break;
    }
  }
  DS_MSG1(MSG_LEGACY_HIGH,"rmnet_inst_from_ep_and_mux_id rmnet inst %d", rmnet_inst);

  return rmnet_inst;

} /* rmnet_inst_from_ep_and_mux_id() */

/*===========================================================================

FUNCTION RMNET_GET_IP_INFO_FROM_INST

DESCRIPTION
  This function retrieves rmnet ip info from instance

PARAMETERS
  ip_inst:    ip_info instnace

DEPENDENCIES
  None

RETURN VALUE
  rmnet_smi_ip_info_type pointer

SIDE EFFECTS
  None.
===========================================================================*/

rmnet_smi_ip_info_type *rmnet_get_ip_info_from_inst
(
  uint8 ip_inst
)
{
  rmnet_smi_ip_info_type  * ip_info;
   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
   
   ip_info = rmnet_smi_ip_info[ip_inst];
   return ip_info;
}

/*===========================================================================
FUNCTION RMNET_META_SMI_IN_CALL_CB

DESCRIPTION
  This is rmnet callback function to check in call status

PARAMETERS
  user_info:  pointer to rmnet_smi_dual_ip_info structure

DEPENDENCIES
  None

RETURN VALUE
  TRUE: rmnet is in call
  FALSE: rmnet is not in call

SIDE EFFECTS
  None.
===========================================================================*/
static boolean rmnet_meta_smi_in_call_cb
(
  void  * user_info
)
{
  rmnet_smi_ip_info_type  * ip_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ip_info = (rmnet_smi_ip_info_type *)user_info;
  if (ip_info != NULL)
  {
    if ( rmnet_meta_sm_in_call(
               RMNET_META_SM_INFO_TO_INSTANCE(ip_info->info_v4)) ||
         rmnet_meta_sm_in_call(
               RMNET_META_SM_INFO_TO_INSTANCE(ip_info->info_v6))||
         rmnet_meta_sm_in_call(		
               RMNET_META_SM_INFO_TO_INSTANCE(ip_info->info_nipd))
       )
    {
      return TRUE;
    }
  }
  return FALSE;
} /* rmnet_meta_smi_in_call_cb() */

/*===========================================================================
FUNCTION RMNET_META_SMI_DTR_CB

DESCRIPTION
  This is rmnet DTR callback function

PARAMETERS
  user_info:  pointer to rmnet_smi_dual_ip_info structure
  dtr_assert: is DTR asserted

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
static void rmnet_meta_smi_dtr_cb
(
  void   * user_info,
  boolean  dtr_asserted
)
{
  rmnet_smi_ip_info_type  * ip_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ip_info = (rmnet_smi_ip_info_type *)user_info;
  if (ip_info != NULL)
  {
    rmnet_meta_smi_process_dtr_changed_cmd(
                         ip_info->info_v4, dtr_asserted);
    rmnet_meta_smi_process_dtr_changed_cmd(
                         ip_info->info_v6, dtr_asserted);	
    rmnet_meta_smi_process_dtr_changed_cmd(
                         ip_info->info_nipd,dtr_asserted);
  }
} /* rmnet_meta_smi_dtr_cb() */

/*===========================================================================
  FUNCTION RMNET_META_SMI_DO_RX_WMK_FC()

  DESCRIPTION
    This function handles RX watermark flow control.

  PARAMETERS
    dual_ip_info:  rmnet dual ip info pointer
    disable:       flow disable

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void rmnet_meta_smi_do_rx_wmk_fc
(
  rmnet_smi_ip_info_type      * ip_info,
  boolean                        disable
)
{
  rmnet_data_agg_enum_type       ul_agg;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (ip_info == NULL || 
      ip_info->info_v4 == NULL || ip_info->info_v6 == NULL)
  {
    return;
  }

  ul_agg = RMNET_XPORT_UL_DATA_AGG(&ip_info->xport_info);

  /*------------------------------------------------------------------------
   * If QMAP is used, don't do SIO flow control because this will
   * flow control all PDNs. AP is supposed to do either WDS/QoS or QMAP
   * in-band flow control if QMAP is enabled.
  -------------------------------------------------------------------------*/
  #if 0
  if (RMNET_DATA_AGG_IS_QMAP(ul_agg) &&
      RMNET_XPORT_IN_BAND_FC(&ip_info->xport_info)) // XXX Remove this check
  {
    if (rmnet_meta_smi_qos_or_te_fc_enabled(ip_info->info_v4))
    {
      // XXX Convert to QOS/QMAP flow control
    }
    else
    {
      // Convert to WDS/QMAP flow control
      rmnet_meta_smi_do_rx_fc(
                     ip_info->info_v4,
                     RMNET_FC_MASK_SIO,
                     disable);

      rmnet_meta_smi_do_rx_fc(
                     ip_info->info_v6,
                     RMNET_FC_MASK_SIO,
                     disable);
    }
  }
/*
  else if (ul_agg == RMNET_ENABLE_DATA_AGG_MBIM &&
           ip_info->xport_info.data_format_cache.mbim_sw_deagg)
  {
    // MBIM specific
    rmnet_phys_xport_set_flow_mask(
               ip_info->xport_info.phys_xport,
               1 << ip_info->xport_info.mux_id,
               disable);
  }
  else*/
  #endif
  {
    // Do SIO flow control
    rmnet_logical_xport_set_rx_flow(&ip_info->xport_info, disable);
  }

} /* rmnet_meta_smi_do_rx_wmk_fc() */

/*===========================================================================
  FUNCTION RMNET_META_SMI_RX_HIWATER_FUNC()

  DESCRIPTION
    Receive watermark high callback.
    Disable rx flow on the data link specified by user_data

  PARAMETERS
    wm_ptr          -  receive watermark on which rx data is enqueued
    user_data_ptr   -  rmnet_xport_type ptr

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void rmnet_meta_smi_rx_hiwater_func
(
  struct dsm_watermark_type_s * wm_ptr,
  void                        * user_data_ptr
)
{
  rmnet_xport_type  * xport_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_ENTER_CRIT_SECTION(&rmnet_crit_section);

  xport_info = (rmnet_xport_type *)user_data_ptr;
  if (xport_info != NULL && !xport_info->rx_wmk_fc)
  {
    xport_info->rx_wmk_fc = TRUE;
    rmnet_meta_smi_do_rx_wmk_fc(
           (rmnet_smi_ip_info_type *)xport_info->user_info, TRUE);
  }

  DS_LEAVE_CRIT_SECTION(&rmnet_crit_section);

} /* rmnet_meta_smi_rx_hiwater_func() */

/*===========================================================================
  FUNCTION RMNET_META_SMI_RX_LOWATER_FUNC()

  DESCRIPTION
    Receive watermark low callback.
    Enable rx flow on the data link specified by user_data

  PARAMETERS
    wm_ptr           -  receive watermark on which rx data is enqueued
    user_data_ptr    -  rmnet_xport_type ptr

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void rmnet_meta_smi_rx_lowater_func
(
  struct dsm_watermark_type_s * wm_ptr,
  void                        * user_data_ptr
)
{
  rmnet_xport_type  * xport_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_ENTER_CRIT_SECTION(&rmnet_crit_section);

  xport_info = (rmnet_xport_type *)user_data_ptr;
  if (xport_info != NULL && xport_info->rx_wmk_fc)
  {
    xport_info->rx_wmk_fc = FALSE;
    rmnet_meta_smi_do_rx_wmk_fc(
           (rmnet_smi_ip_info_type *)xport_info->user_info, FALSE);
  }

  DS_LEAVE_CRIT_SECTION(&rmnet_crit_section);

} /* rmnet_meta_smi_rx_lowater_func() */

/*===========================================================================
  FUNCTION RMNET_META_SMI_HANDLE_EP_CFG_DONE_EV

  DESCRIPTION
    This function processes RMNET_SM_PORT_EP_CFG_DONE_EV event.

  PARAMETERS
    info - pointer to the interface controller's control block

  RETURN VALUE
    None

  DEPENDENCIES
    must !ONLY! be called from command dispatcher
    - state and initialization of the control block is verified there

  SIDE EFFECTS
    None
===========================================================================*/
static void rmnet_meta_smi_handle_ep_cfg_done_ev
(
  rmnet_smi_info_type *  info
)
{
  rmnet_instance_e_type  rmnet_inst;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 /*Validate the info pointer with the global Rmnet SMI INFO*/
  if(TRUE == rmnet_sm_is_valid(info))
  {
    rmnet_inst = RMNET_META_SM_INFO_TO_INSTANCE(info);

    DS_MSG2(MSG_LEGACY_HIGH,"Recvd EP_CFG_DONE_EV, RMNET inst %d, state %d",
                     rmnet_inst, info->meta_sm.state );

    switch( info->meta_sm.state )
    {
      case RMNET_META_SM_EP_CONFIGURING_STATE:
        rmnet_meta_smi_transition_state(
                       info, RMNET_META_SM_UM_CONFIGURING_STATE);
        break;

      default:
        break;
    } 
  }
} /* rmnet_meta_smi_handle_ep_cfg_done_ev() */

/*===========================================================================
  FUNCTION RMNET_META_SMI_DO_TX_WMK_FC()

  DESCRIPTION
    This function handles RX watermark flow control.

  PARAMETERS
    dual_ip_info:  rmnet dual ip info pointer
    disable:       flow disable

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
//Check if this api is needed
static void rmnet_meta_smi_do_tx_wmk_fc
(
  rmnet_smi_ip_info_type     * ip_info,
  boolean                        disable
)
{
  rmnet_smi_cmd_type  * cmd_payload;
  ds_command_type     *cmd_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (ip_info == NULL)
  {
    return;
  }

  /*-----------------------------------------------------------------------
  	 Get a DS command buffer
   -----------------------------------------------------------------------*/
   cmd_ptr = ds_allocate_cmd_buf(sizeof(rmnet_smi_cmd_type));
   if ((cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL))
   {
     DS_ERR_FATAL("Out of memory"); 
	 return;
   }
  cmd_ptr->hdr.cmd_id = DS_QMI_RMNET_SM_CMD;
  cmd_payload = (rmnet_smi_cmd_type *)cmd_ptr->cmd_payload_ptr;

  cmd_payload->sm_id	  = RMNET_META_SM;
  cmd_payload->info_ptr = ip_info;

  if (disable)
  {
    cmd_payload->cmd_type = RMNET_SM_CMD_TX_FLOW_DISABLE;
  }
  else /* enable */
  {
    cmd_payload->cmd_type = RMNET_SM_CMD_TX_FLOW_ENABLE;
  }
	
  ds_put_cmd(cmd_ptr);

  DS_MSG1(MSG_LEGACY_ERROR,"TX FLOW CTRL: %c", disable ? '+' : '-');

} /* rmnet_meta_smi_do_tx_wmk_fc() */

/*===========================================================================
  FUNCTION RMNET_META_SMI_TX_HIWATER_FUNC()

  DESCRIPTION
    Receive watermark high callback.
    Disable tx flow on the data link specified by user_data

  PARAMETERS
    tx_wm_ptr       -  receive watermark on which tx data is enqueued
    user_data_ptr   -  rmnet_xport_type ptr

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void rmnet_meta_smi_tx_hiwater_func
(
  struct dsm_watermark_type_s * wm_ptr,
  void                        * user_data_ptr
)
{
  rmnet_xport_type  * xport_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_ENTER_CRIT_SECTION(&rmnet_crit_section);

  xport_info = (rmnet_xport_type *)user_data_ptr;
  if (xport_info != NULL && !xport_info->tx_wmk_fc)
  {
    xport_info->tx_wmk_fc = TRUE;
    rmnet_meta_smi_do_tx_wmk_fc(
           (rmnet_smi_ip_info_type *)xport_info->user_info, TRUE);
  }

  DS_LEAVE_CRIT_SECTION(&rmnet_crit_section);

} /* rmnet_meta_smi_tx_hiwater_func() */

#ifdef FEATURE_DATA_QMI_QOS
void rmnet_process_qos_hdr
(
  dsm_item_type          * pkt_ptr,
  ds_fwk_s_type          * ds_fwk_inst,
  ip_addr_enum_type       ip_ver,
  boolean                  is_opt,
  ps_tx_meta_info_type   * tx_meta_info_ptr,
  void                   * qos_hdr,
  rmnet_qos_format_e_type  qos_hdr_format,
  ds_flow_type           **flow_ptr_ret  // J-F-C
)
{
  uint16          ip_id;
  ds_flow_type            *flow_ptr = NULL;
  ps_rmnet_qos_hdr_type   *qos_6_byte_hdr = NULL;
  ps_rmnet_qos2_hdr_type  *qos_8_byte_hdr = NULL;
  uint32                qos_handle = 0;
  uint8                 version = 0;
  uint8                 flags = 0 ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (qos_hdr == NULL )
  {
    DS_MSG1(MSG_LEGACY_ERROR,"QOS header is NULL or um iface 0x%p is NULL", qos_hdr);
    return;
  }
  if (qos_hdr_format == RMNET_QOS_FORMAT_8_BYTE)
  {
    qos_8_byte_hdr = (ps_rmnet_qos2_hdr_type*)qos_hdr;
    qos_handle     = qos_8_byte_hdr->hdr.qos_handle;
    version        = qos_8_byte_hdr->hdr.version;
    flags          = qos_8_byte_hdr->hdr.flags; 
  }
  else if (qos_hdr_format == RMNET_QOS_FORMAT_6_BYTE)
  {
    qos_6_byte_hdr = (ps_rmnet_qos_hdr_type*)qos_hdr;
    qos_handle     = qos_6_byte_hdr->qos_handle;
    version        = qos_6_byte_hdr->version;
    flags          = qos_6_byte_hdr->flags; 
  }
  else 
  {
    DS_MSG1(MSG_LEGACY_ERROR,"Invalid QOS Format %d", qos_hdr_format);
  }

  /* Replace qos handle with flow_ptr if valid */
  #if 0
  if (0 == qmi_qos_get_flow_ptr_from_handle(qos_handle, &flow_ptr ))
  {
    LOG_MSG_ERROR_1("Flow handle 0x%x passed in is invalid, forwarding onto default flow", qos_handle);
    flow_ptr = NULL;
  }
  else
  {
    LOG_MSG_DATA_PATH_INFO_2("Replacing qos handle 0x%x with flow ptr 0x%x", 
                              qos_hdr->qos_handle, 
                              flow_ptr);
  }

  /*-------------------------------------------------------------------------
    Update flow in QoS header based on follwing steps
      1. If flow in QoS header is valid, go to step 2. Else set it to
         default flow of Um iface
      2. If it is optimized path, fetch assoc flow and go to step 3. Else, exit
      3. If assoc flow is valid, update flow in QoS header to assoc flow. Else,
         set flow in QoS header to default flow of Um iface
  -------------------------------------------------------------------------*/
  DS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  if (!PS_FLOW_IS_VALID(flow_ptr))
  {
    flow_ptr = PS_IFACEI_GET_DEFAULT_FLOW(um_iface_ptr);
  }
  
  /*-------------------------------------------------------------------------
    Always forward on default if flow is not ACTIVATED
  -------------------------------------------------------------------------*/
  if (FLOW_ACTIVATED != PS_FLOW_GET_STATE(flow_ptr))
  {
    /* Resume flow if it is SUSPENDED, this is needed for NW init QOS on eHRPD*/
    if (PS_FLOW_IS_NW_INITIATED(flow_ptr) &&
        PS_FLOW_GET_STATE(flow_ptr) == FLOW_SUSPENDED)
    {
      LOG_MSG_INFO2_1("Resuming SUSPENDED NW init QOS flow 0x%x", flow_ptr);
      return_val =  ps_flow_ioctl(flow_ptr,
                                  PS_FLOW_IOCTL_QOS_RESUME,
                                  NULL,
                                  &ps_errno);
      if (return_val != 0)
      {
        LOG_MSG_ERROR_1("Error while trying to resume flow %d", ps_errno);
      }
    }
    flow_ptr = PS_IFACEI_GET_DEFAULT_FLOW(um_iface_ptr);
  }
  DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
#endif

  /*-------------------------------------------------------------------------
    Update Tx meta info with routing cache and QoS filter result
  -------------------------------------------------------------------------*/
  PS_TX_META_SET_ROUTING_CACHE(tx_meta_info_ptr, ds_fwk_inst);
  
  PS_TX_META_SET_FILTER_RESULT(tx_meta_info_ptr,
                               FLTR_CLIENT_QOS_OUTPUT,
                               flow_ptr);
  PS_TX_META_SET_TX_FLAGS(tx_meta_info_ptr, 0);

  /*-------------------------------------------------------------------------
    Handle DoS. Use the IP_ID from the packet as the DOS ack handle
  -------------------------------------------------------------------------*/
  if (flags & ((uint8) 0x1))
  {
    if (IP_V4 == ip_ver)
    {
      (void) dsm_extract(pkt_ptr,
                         RMNET_OPT_V4_ID_OFFSET,
                         &ip_id,
                         RMNET_OPT_V4_ID_LEN);
      PS_TX_META_SET_TX_FLAGS(tx_meta_info_ptr, MSG_FAST_EXPEDITE);
      PS_TX_META_SET_DOS_ACK_HANDLE(tx_meta_info_ptr, ip_id);

      DS_MSG1(MSG_LEGACY_ERROR,"ps_dpm_process_qos_hdr():"
                               "Set DoS Ack handle to %d", ip_id);
    }
    else
    {
      DS_MSG0(MSG_LEGACY_HIGH,"ps_dpm_process_qos_hdr(): "
                                "DOS not enabled for IPv6");
    }
  }
  
  if (NULL != flow_ptr_ret)
  {
    *flow_ptr_ret = flow_ptr;
  }

  return;
}

#endif
/*===========================================================================
  FUNCTION RMNET_META_SMI_TX_LOWATER_FUNC()

  DESCRIPTION
    Receive watermark low callback.
    Enable tx flow on the data link specified by user_data

  PARAMETERS
    wm_ptr           -  receive watermark on which tx data is enqueued
    user_data_ptr    -  rmnet_xport_type ptr

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void rmnet_meta_smi_tx_lowater_func
(
  struct dsm_watermark_type_s * wm_ptr,
  void                        * user_data_ptr
)
{
  rmnet_xport_type  * xport_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_ENTER_CRIT_SECTION(&rmnet_crit_section);

  xport_info = (rmnet_xport_type *)user_data_ptr;
  if (xport_info != NULL && xport_info->tx_wmk_fc)
  {
    xport_info->tx_wmk_fc = FALSE;
    rmnet_meta_smi_do_tx_wmk_fc(
           (rmnet_smi_ip_info_type *)xport_info->user_info, FALSE);
  }

  DS_LEAVE_CRIT_SECTION(&rmnet_crit_section);

} /* rmnet_meta_smi_tx_lowater_func() */

/*===========================================================================
  FUNCTION RMNET_SET_MANUAL_DNS_SETTINGS()

  DESCRIPTION
    Sets the manual DNS settings used for the instance
    associated with ep_id and mux_id.

  PARAMETERS
    ep_id - End point id associated with the instance

  RETURN VALUE
    TRUE  - update successful
    FALSE - udpate failed

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean rmnet_set_manual_dns_settings
(
  rmnet_smi_ip_info_type *info
)
{
  qmi_nv_dns_addr_type  qmi_ipv4_dns_nv_item;
  qmi_nv_ipv6_dns_addr_type qmi_ipv6_dns_nv_item;
  qmi_nv_status_e_type qmi_nv_status = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*Setting Primary IPv4 Dns*/
  memset(&qmi_ipv4_dns_nv_item, 0, sizeof(qmi_ipv4_dns_nv_item));
  qmi_nv_status = qmi_nv_read(QMI_NV_ITEM_MANUAL_DNS_IPV4_PRI,
                              info->xport_info.ep_id,
                              info->xport_info.mux_id,
                              &qmi_ipv4_dns_nv_item,
                              sizeof(qmi_ipv4_dns_nv_item));
  if (QMI_NV_STATUS_OK == qmi_nv_status)
  {
      DS_MSG1(MSG_LEGACY_ERROR,"Manual IPv4 DNS setting (pri) read [%x]",
              qmi_ipv4_dns_nv_item.dns_addr);
      info->info_v4->meta_sm.settings.pri_dns_addr.type = IPV4_ADDR;
      info->info_v4->meta_sm.settings.pri_dns_addr.addr.v4 = qmi_ipv4_dns_nv_item.dns_addr;
    
  }
  else
  {
    DS_MSG0(MSG_LEGACY_ERROR,"No Manual Primary Dns set");
  }

  /*Setting Secondary DNS*/
  memset(&qmi_ipv4_dns_nv_item, 0, sizeof(qmi_ipv4_dns_nv_item));
  qmi_nv_status = qmi_nv_read(QMI_NV_ITEM_MANUAL_DNS_IPV4_SEC,
                              info->xport_info.ep_id,
                              info->xport_info.mux_id,
                              &qmi_ipv4_dns_nv_item,
                              sizeof(qmi_ipv4_dns_nv_item));
  if (QMI_NV_STATUS_OK == qmi_nv_status)
  {
      DS_MSG1(MSG_LEGACY_ERROR,"Manual IPv4 DNS setting (sec) read [%x]",
              qmi_ipv4_dns_nv_item.dns_addr);
      info->info_v4->meta_sm.settings.sec_dns_addr.type = IPV4_ADDR;
      info->info_v4->meta_sm.settings.sec_dns_addr.addr.v4 = qmi_ipv4_dns_nv_item.dns_addr;
  }
  else
  {
    DS_MSG0(MSG_LEGACY_ERROR,"No Manual Secondary Dns set");
  }

  /*Setting Secondary Ipv6 DNS*/
  memset(&qmi_ipv6_dns_nv_item, 0, sizeof(qmi_ipv6_dns_nv_item));
  qmi_nv_status = qmi_nv_read(QMI_NV_ITEM_MANUAL_DNS_IPV6_PRI,
                              info->xport_info.ep_id,
                              info->xport_info.mux_id,
                              &qmi_ipv6_dns_nv_item,
                              sizeof(qmi_ipv6_dns_nv_item));
  if (QMI_NV_STATUS_OK == qmi_nv_status)
  {
      DS_MSG2(MSG_LEGACY_ERROR,"Manual IPv6 primary DNS setting (pri) read [%x], [%x]",
              qmi_ipv6_dns_nv_item.dns_addr[0],qmi_ipv6_dns_nv_item.dns_addr[1]);
      info->info_v6->meta_sm.settings.ipv6_pri_dns_addr.type = IPV6_ADDR;
      info->info_v6->meta_sm.settings.ipv6_pri_dns_addr.addr.v6[0] = qmi_ipv6_dns_nv_item.dns_addr[0];
      info->info_v6->meta_sm.settings.ipv6_pri_dns_addr.addr.v6[1] = qmi_ipv6_dns_nv_item.dns_addr[1];
  }
  else
  {
    DS_MSG0(MSG_LEGACY_ERROR,"No Manual Ipv6 Primary Dns set");
  }

  /*Setting Secondary Ipv6 DNS*/
  memset(&qmi_ipv6_dns_nv_item, 0, sizeof(qmi_ipv6_dns_nv_item));
  qmi_nv_status = qmi_nv_read(QMI_NV_ITEM_MANUAL_DNS_IPV6_SEC,
                              info->xport_info.ep_id,
                              info->xport_info.mux_id,
                              &qmi_ipv6_dns_nv_item,
                              sizeof(qmi_ipv6_dns_nv_item));
  if (QMI_NV_STATUS_OK == qmi_nv_status)
  {
      DS_MSG2(MSG_LEGACY_ERROR,"Manual IPv6 secondary DNS setting (sec) read [%x], [%x]",
              qmi_ipv6_dns_nv_item.dns_addr[0],qmi_ipv6_dns_nv_item.dns_addr[1]);
      info->info_v6->meta_sm.settings.ipv6_sec_dns_addr.type = IPV6_ADDR;
      info->info_v6->meta_sm.settings.ipv6_sec_dns_addr.addr.v6[0] = qmi_ipv6_dns_nv_item.dns_addr[0];
      info->info_v6->meta_sm.settings.ipv6_sec_dns_addr.addr.v6[1] = qmi_ipv6_dns_nv_item.dns_addr[1];
  }
  else
  {
    DS_MSG0(MSG_LEGACY_ERROR,"No Manual Ipv6 Secondary Dns set");
  }
  /* need to set IPv6 addresses on both instances (IPv4+6 and IPv6-only) to keep
     them in sync */
  memset(&qmi_ipv4_dns_nv_item, 0, sizeof(qmi_ipv4_dns_nv_item));
  qmi_nv_status = qmi_nv_read(QMI_NV_ITEM_MANUAL_DNS_IPV4_PRI,
                              info->xport_info.ep_id,
                              info->xport_info.mux_id,
                              &qmi_ipv4_dns_nv_item,
                              sizeof(qmi_ipv4_dns_nv_item));
  if (QMI_NV_STATUS_OK == qmi_nv_status)
  {
      DS_MSG1(MSG_LEGACY_ERROR,"Manual IPv4 DNS setting (pri) read [%x]",
                    qmi_ipv4_dns_nv_item.dns_addr);
      info->info_v6->meta_sm.settings.pri_dns_addr.type = IPV4_ADDR;
      info->info_v6->meta_sm.settings.pri_dns_addr.addr.v4 = qmi_ipv4_dns_nv_item.dns_addr;
    
  }
  else
  {
    DS_MSG0(MSG_LEGACY_ERROR,"No Manual Primary Dns set");
  }

  /*Setting Secondary DNS*/
  memset(&qmi_ipv4_dns_nv_item, 0, sizeof(qmi_ipv4_dns_nv_item));
  qmi_nv_status = qmi_nv_read(QMI_NV_ITEM_MANUAL_DNS_IPV4_SEC,
                              info->xport_info.ep_id,
                              info->xport_info.mux_id,
                              &qmi_ipv4_dns_nv_item,
                              sizeof(qmi_ipv4_dns_nv_item));
  if (QMI_NV_STATUS_OK == qmi_nv_status)
  {
      DS_MSG1(MSG_LEGACY_ERROR,"Manual IPv4 DNS setting (sec) read [%x]",
                     qmi_ipv4_dns_nv_item.dns_addr);
      info->info_v6->meta_sm.settings.sec_dns_addr.type = IPV4_ADDR;
      info->info_v6->meta_sm.settings.sec_dns_addr.addr.v4 = qmi_ipv4_dns_nv_item.dns_addr;
  }
  else
  {
    DS_MSG0(MSG_LEGACY_ERROR,"No Manual Secondary Dns set");
  }

  /*Setting Secondary Ipv6 DNS*/
  memset(&qmi_ipv6_dns_nv_item, 0, sizeof(qmi_ipv6_dns_nv_item));
  qmi_nv_status = qmi_nv_read(QMI_NV_ITEM_MANUAL_DNS_IPV6_PRI,
                              info->xport_info.ep_id,
                              info->xport_info.mux_id,
                              &qmi_ipv6_dns_nv_item,
                              sizeof(qmi_ipv6_dns_nv_item));
  if (QMI_NV_STATUS_OK == qmi_nv_status)
  {
      DS_MSG2(MSG_LEGACY_ERROR,"Manual IPv6 primary DNS setting (pri) read [%x], [%x]",
              qmi_ipv6_dns_nv_item.dns_addr[0],qmi_ipv6_dns_nv_item.dns_addr[1]);
      info->info_v4->meta_sm.settings.ipv6_pri_dns_addr.type = IPV6_ADDR;
      info->info_v4->meta_sm.settings.ipv6_pri_dns_addr.addr.v6[0] = qmi_ipv6_dns_nv_item.dns_addr[0];
      info->info_v4->meta_sm.settings.ipv6_pri_dns_addr.addr.v6[1] = qmi_ipv6_dns_nv_item.dns_addr[1];
  }
  else
  {
    DS_MSG0(MSG_LEGACY_ERROR,"No Manual Ipv6 Primary Dns set");
  }

  /*Setting Secondary Ipv6 DNS*/
  memset(&qmi_ipv6_dns_nv_item, 0, sizeof(qmi_ipv6_dns_nv_item));
  qmi_nv_status = qmi_nv_read(QMI_NV_ITEM_MANUAL_DNS_IPV6_SEC,
                              info->xport_info.ep_id,
                              info->xport_info.mux_id,
                              &qmi_ipv6_dns_nv_item,
                              sizeof(qmi_ipv6_dns_nv_item));
  if (QMI_NV_STATUS_OK == qmi_nv_status)
  {
      DS_MSG2(MSG_LEGACY_ERROR,"Manual IPv6 secondary DNS setting (sec) read [%x], [%x]",
              qmi_ipv6_dns_nv_item.dns_addr[0],qmi_ipv6_dns_nv_item.dns_addr[1]);
      info->info_v4->meta_sm.settings.ipv6_sec_dns_addr.type = IPV6_ADDR;
      info->info_v4->meta_sm.settings.ipv6_sec_dns_addr.addr.v6[0] = qmi_ipv6_dns_nv_item.dns_addr[0];
      info->info_v4->meta_sm.settings.ipv6_sec_dns_addr.addr.v6[1] = qmi_ipv6_dns_nv_item.dns_addr[1];
  }
  else
  {
    DS_MSG0(MSG_LEGACY_ERROR,"No Manual Ipv6 Secondary Dns set");
  }

  return TRUE;
} /* rmnet_set_manual_dns_settings() */

#ifdef FEATURE_DATA_QCRMCALL

/*===========================================================================
  FUNCTION RMNET_QCRMCALL_INST_ADD()

  DESCRIPTION
    This function adds the rmnet instance to the qcrmcall table

  PARAMETERS
    ep_id:     End point ID
    mux_id:    Mux ID

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void rmnet_qcrmcall_inst_add
(
  uint32   ep_id,
  uint8    mux_id
)
{
  int  i;
  int  err = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (rmnet_inst_from_ep_and_mux_id(
            ep_id, mux_id, TRUE,IPV4_ADDR) >= RMNET_INSTANCE_MAX)
  {
    err = 1;
    goto send_result;
  }

  // Check if ep_id, mux_id is already in the tbl
  for (i = 0; i < RMNET_QCRMCALL_MAX; i++)
  {
    if (rmnet_qcrmcall_inst[i].ep_id == ep_id &&
        rmnet_qcrmcall_inst[i].mux_id == mux_id)
    {
      goto send_result;
    }
  }

  // Find an available slot
  for (i = 0; i < RMNET_QCRMCALL_MAX; i++)
  {
    if (rmnet_qcrmcall_inst[i].ep_id == 0)
    {
      memset(&rmnet_qcrmcall_inst[i], 0, sizeof(rmnet_qcrmcall_inst_type));
      rmnet_qcrmcall_inst[i].ep_id = ep_id;
      rmnet_qcrmcall_inst[i].mux_id = mux_id;
      goto send_result;
    }
  }

  if (i >= RMNET_QCRMCALL_MAX)
  {
    err = 2;
    goto send_result;
  }

send_result:
  if (err != 0)
  {
    DS_MSG3(MSG_LEGACY_ERROR,"Failed to add qcrmcall inst for "
               "ep_id 0x%x, mux_id %d, err %d",
               ep_id, mux_id, err);
  }
  else
  {
    DS_MSG2(MSG_LEGACY_ERROR,"Added qcrmcall inst ep_id 0x%x mux_id %d",
              ep_id, mux_id);
  }

} /* rmnet_qcrmcall_inst_add() */

/*===========================================================================
  FUNCTION RMNET_QCRMCALL_INST_REMOVE()

  DESCRIPTION
    This function removes qcrmcall instance

  PARAMETERS
    ep_id:     End point ID

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void rmnet_qcrmcall_inst_remove
(
  uint32   ep_id
)
{
  int  i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (ep_id == 0)
  {
    return;
  }

  for (i = 0; i < RMNET_QCRMCALL_MAX; i++)
  {
    if (rmnet_qcrmcall_inst[i].ep_id == ep_id)
    {
      memset(&rmnet_qcrmcall_inst[i], 0, sizeof(rmnet_qcrmcall_inst_type));
    }
  }

} /* rmnet_qcrmcall_inst_remove() */

/*===========================================================================
  FUNCTION RMNET_SEND_QCRMCALL_REQUEST

  DESCRIPTION
    This function sends a qcrmcall request to RMNET

  PARAMETERS
    action:        qcrmcall reqeust type
    request:       request info
    callback:      Callback to be invoked for sending result 

  RETURN VALUE
    TRUE: success
    FALSE: failed

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean rmnet_send_qcrmcall_request
(
  rmnet_qcrmcall_action_e_type    action,
  rmnet_qcrmcall_request_type   * request,
  rmnet_qcrmcall_cb_type          callback
)
{
  rmnet_smi_cmd_type               * cmd_payload;
  rmnet_meta_sm_network_info_type  * sni_params_ptr = NULL;
  int                                err = 0;  
  ds_command_type                   *cmd_ptr; 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  do
  {
    /*-----------------------------------------------------------------------
      Sanity checks
    -----------------------------------------------------------------------*/
    if (action != QCRMCALL_ACTION_STOP &&
        action != QCRMCALL_ACTION_START &&
        action != QCRMCALL_ACTION_QUERY)
    {
      err = 1;
      break;
    }

    if (request == NULL)
    {
      err = 2;
      break;
    }

    if (action == QCRMCALL_ACTION_START)
    {
      if (request->start.sni_params == NULL)
      {
        err = 3;
        break;
      }

      DS_SYSTEM_HEAP_MEM_ALLOC(sni_params_ptr,
                      sizeof(rmnet_meta_sm_network_info_type),
                      rmnet_meta_sm_network_info_type*);
      if (sni_params_ptr == NULL)
      {
        err = 4;
        break;
      }

      memscpy(sni_params_ptr, sizeof(rmnet_meta_sm_network_info_type),
            request->start.sni_params, sizeof(rmnet_meta_sm_network_info_type));
    }

    /*-----------------------------------------------------------------------
     Get a DS command buffer
     -----------------------------------------------------------------------*/
    cmd_ptr = ds_allocate_cmd_buf(sizeof(rmnet_smi_cmd_type));
    if ((cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL))
    {
      if (sni_params_ptr != NULL)
      {
        DS_SYSTEM_HEAP_MEM_FREE(sni_params_ptr);
      }
      err = 5;
      break;
      
    }
    cmd_ptr->hdr.cmd_id = DS_QMI_RMNET_SM_CMD;
    cmd_payload = (rmnet_smi_cmd_type *)cmd_ptr->cmd_payload_ptr;

    /*-----------------------------------------------------------------------
       Post cmd to DS task.
    -----------------------------------------------------------------------*/
    cmd_payload->sm_id                  = RMNET_META_SM;
    cmd_payload->cmd_type               = RMNET_SM_CMD_QCRMCALL;
    cmd_payload->data.qcrmcall.action   = action;
    cmd_payload->data.qcrmcall.request  = * request;
    cmd_payload->data.qcrmcall.callback = callback;

    if (action == QCRMCALL_ACTION_START)
    {
      cmd_payload->data.qcrmcall.request.start.sni_params = sni_params_ptr;
    }
    ds_put_cmd(cmd_ptr);
  } while (0);

  if (err)
  {
    DS_MSG2(MSG_LEGACY_ERROR,"Failed to send qcrmcall action [%d] err %d",
                       action, err);
    return FALSE;
  }

  return TRUE;

} /* rmnet_send_qcrmcall_request() */

/*===========================================================================
  FUNCTION RMNET_QCRMCALL_INST_TO_RMNET_INST()

  DESCRIPTION
    This function returns rmnet instance from qcrmcall instance

  PARAMETERS
    qcrmcall_inst: qcrmcall instance
    ip_family:     IP family

  RETURN VALUE
    rmnet instance

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static uint8 rmnet_qcrmcall_inst_to_rmnet_inst
(
  uint8  qcrmcall_inst,
  uint8  ip_family
)
{
  uint8  rmnet_inst;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (qcrmcall_inst >= RMNET_QCRMCALL_MAX ||
      rmnet_qcrmcall_inst[qcrmcall_inst].ep_id == 0)
  {
    return RMNET_INSTANCE_MAX;
  }

  rmnet_inst = rmnet_inst_from_ep_and_mux_id(
                   rmnet_qcrmcall_inst[qcrmcall_inst].ep_id,
                   rmnet_qcrmcall_inst[qcrmcall_inst].mux_id,
                   ip_family != IPV6_ADDR,ip_family);

  return rmnet_inst;

} /* rmnet_qcrmcall_inst_to_rmnet_inst() */

/*===========================================================================
  FUNCTION RMNET_META_SMI_PROCESS_QCRMALL_CMD

  DESCRIPTION
    This function handles a qcrmcall request

  PARAMETERS
    action:        qcrmcall reqeust type
    request:       request info
    callback:      Callback to be invoked for sending result 

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void rmnet_meta_smi_process_qcrmcall_cmd
(
  rmnet_qcrmcall_action_e_type    action,
  rmnet_qcrmcall_request_type   * request,
  rmnet_qcrmcall_cb_type          callback
)
{
  uint8                       rmnet_inst;
  uint8                       qcrmcall_inst;
  rmnet_qcrmcall_result_type  result;
  int                         err = 0;
  int                         i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (request == NULL)
  {
    DS_ASSERT(0);
    return;
  }

  DS_MSG1(MSG_LEGACY_ERROR,"$QCRMCALL action [%d]", action);

  memset(&result, 0, sizeof(result));

  if (action == QCRMCALL_ACTION_START)
  {
    qcrmcall_inst = request->start.qcrmcall_inst;

    result.sni.qcrmcall_inst = qcrmcall_inst;
    if (request->start.sni_params != NULL)
    {
      result.sni.ip_family = request->start.sni_params->ip_family_pref;
    }
    else
    {
      result.sni.ip_family = 0;
      result.sni.status = FALSE;
      err = 1;
      goto send_result;
    }

    if (qcrmcall_inst >= RMNET_QCRMCALL_MAX)
    {
      DS_SYSTEM_HEAP_MEM_FREE(request->start.sni_params);
      result.sni.status = FALSE;
      err = 2;
      goto send_result;
    }

    rmnet_inst = rmnet_qcrmcall_inst_to_rmnet_inst(
                   request->start.qcrmcall_inst,
                   request->start.sni_params->ip_family_pref);
    if (rmnet_inst >= RMNET_INSTANCE_MAX)
    {
      DS_SYSTEM_HEAP_MEM_FREE(request->start.sni_params);
      result.sni.status = FALSE;
      err = 3;
      goto send_result;
    }

    if (rmnet_meta_sm_in_call(rmnet_inst))
    {
      DS_SYSTEM_HEAP_MEM_FREE(request->start.sni_params);
      result.sni.status = FALSE;
      err = 4;
      goto send_result;
    }

    if (request->start.sni_params->ip_family_pref == IPV6_ADDR)
    {
      rmnet_qcrmcall_inst[qcrmcall_inst].v6_state = RMNET_QCRMCALL_STATE_START;
      rmnet_qcrmcall_inst[qcrmcall_inst].v6_cb = callback;
    }
    else
    {
      rmnet_qcrmcall_inst[qcrmcall_inst].v4_state = RMNET_QCRMCALL_STATE_START;
      rmnet_qcrmcall_inst[qcrmcall_inst].v4_cb = callback;
    }

    // Start the call
    rmnet_meta_sm_set_network_cfg_params(
                  rmnet_inst, request->start.sni_params);
    rmnet_meta_smi_handle_rm_dev_start_pkt_ev(rmnet_smi_info[rmnet_inst]);

    DS_SYSTEM_HEAP_MEM_FREE(request->start.sni_params);
    return;
  }
  else if (action == QCRMCALL_ACTION_STOP)
  {
    qcrmcall_inst = request->stop.qcrmcall_inst;

    result.sni.qcrmcall_inst = qcrmcall_inst;
    result.sni.ip_family = request->stop.ip_family;

    if (request->stop.qcrmcall_inst >= RMNET_QCRMCALL_MAX)
    {
      result.sni.status = FALSE;
      err = 5;
      goto send_result;
    }

    rmnet_inst = rmnet_qcrmcall_inst_to_rmnet_inst(
                   request->stop.qcrmcall_inst,
                   request->stop.ip_family);
    if (rmnet_inst >= RMNET_INSTANCE_MAX)
    {
      result.sni.status = FALSE;
      err = 6;
      goto send_result;
    }

    if (!rmnet_meta_sm_in_call(rmnet_inst))
    {
      result.sni.status = FALSE;
      err = 7;
      goto send_result;
    }

    if (request->stop.ip_family == IPV6_ADDR)
    {
      if (rmnet_qcrmcall_inst[qcrmcall_inst].v6_state != RMNET_QCRMCALL_STATE_START &&
          rmnet_qcrmcall_inst[qcrmcall_inst].v6_state != RMNET_QCRMCALL_STATE_UP)
      {
        result.sni.status = FALSE;
        err = 8;
        goto send_result;
      }
      // If there is a pending start, send failure response
      if (rmnet_qcrmcall_inst[qcrmcall_inst].v6_state == RMNET_QCRMCALL_STATE_START)
      {
        if (rmnet_qcrmcall_inst[qcrmcall_inst].v6_cb)
        {
          result.sni.status = FALSE;
          rmnet_qcrmcall_inst[qcrmcall_inst].v6_cb(QCRMCALL_ACTION_START, &result);
        }
      }
      rmnet_qcrmcall_inst[qcrmcall_inst].v6_state = RMNET_QCRMCALL_STATE_STOP;
      rmnet_qcrmcall_inst[qcrmcall_inst].v6_cb = callback;
    }
    else
    {
      if (rmnet_qcrmcall_inst[qcrmcall_inst].v4_state != RMNET_QCRMCALL_STATE_START &&
          rmnet_qcrmcall_inst[qcrmcall_inst].v4_state != RMNET_QCRMCALL_STATE_UP)
      {
        result.sni.status = FALSE;
        err = 9;
        goto send_result;
      }
      // If there is a pending start, send failure response
      if (rmnet_qcrmcall_inst[qcrmcall_inst].v4_state == RMNET_QCRMCALL_STATE_START)
      {
        if (rmnet_qcrmcall_inst[qcrmcall_inst].v4_cb)
        {
          result.sni.status = FALSE;
          rmnet_qcrmcall_inst[qcrmcall_inst].v4_cb(QCRMCALL_ACTION_START, &result);
        }
      }
      rmnet_qcrmcall_inst[qcrmcall_inst].v4_state = RMNET_QCRMCALL_STATE_STOP;
      rmnet_qcrmcall_inst[qcrmcall_inst].v4_cb = callback;
    }

    // Stop the call
    rmnet_meta_smi_handle_rm_link_down_ev(rmnet_smi_info[rmnet_inst]);
    return;
  }
  else if (action == QCRMCALL_ACTION_QUERY)
  {
    for (i = 0; i < RMNET_QCRMCALL_MAX; i++)
    {
      if (rmnet_qcrmcall_inst[i].ep_id != 0)
      {
        if (rmnet_qcrmcall_inst[i].v4_state == RMNET_QCRMCALL_STATE_UP)
        {
          result.list.qcrmcall[result.list.num_qcrmcall].qcrmcall_inst = i;
          result.list.qcrmcall[result.list.num_qcrmcall++].ip_family = IPV4_ADDR;
        }
        if (rmnet_qcrmcall_inst[i].v6_state == RMNET_QCRMCALL_STATE_UP)
        {
          result.list.qcrmcall[result.list.num_qcrmcall].qcrmcall_inst = i;
          result.list.qcrmcall[result.list.num_qcrmcall++].ip_family = IPV6_ADDR;
        }
      }
    }
  }

send_result:
  if (action == QCRMCALL_ACTION_START || action == QCRMCALL_ACTION_STOP)
  {
    DS_MSG2(MSG_LEGACY_ERROR,"$QCRMCALL action [%d] failed err %d", action, err);

  }

  if (callback)
  {
    callback(action, &result);
  }

} /* rmnet_meta_smi_process_qcrmcall_cmd() */

/*===========================================================================
  FUNCTION RMNET_META_SMI_UPDATE_QCRMALL_STATE

  DESCRIPTION
    This function updates qcrmcall state

  PARAMETERS
    info: rmnet smi info

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void rmnet_meta_smi_update_qcrmcall_state
(
  rmnet_smi_info_type * info
)
{
  rmnet_smi_ip_info_type  * ip_info;
  rmnet_qcrmcall_action_e_type   action = QCRMCALL_ACTION_NONE;
  rmnet_qcrmcall_result_type     result;
  int                            i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (info == NULL)
  {
    DS_ASSERT(0);
    return;
  }

  ip_info = rmnet_smi_ip_info[info->constants.ip_inst];
  if (ip_info == NULL)
  {
    DS_ASSERT(0);
    return;
  }

  // Check if this is a qcrmcall instance
  for (i = 0; i < RMNET_QCRMCALL_MAX; i++)
  {
    if (rmnet_qcrmcall_inst[i].ep_id == RMNET_EP_ID(info) &&
        rmnet_qcrmcall_inst[i].mux_id == RMNET_MUX_ID(info))
    {
      break;
    }
  }

  if (i >= RMNET_QCRMCALL_MAX)
  {
    return;
  }

  // Send qcrmcall result if triggered by atcop
  memset(&result, 0, sizeof(result));
  result.sni.qcrmcall_inst = i;

  if (info == ip_info->info_v4)
  {
    if (info->meta_sm.state == RMNET_META_SM_NET_IF_UP_STATE && 
        rmnet_qcrmcall_inst[i].v4_state == RMNET_QCRMCALL_STATE_START)
    {
      rmnet_qcrmcall_inst[i].v4_state = RMNET_QCRMCALL_STATE_UP;
      action = QCRMCALL_ACTION_START;
      result.sni.status = TRUE;
    }
    else if (info->meta_sm.state == RMNET_META_SM_NULL_STATE && 
             rmnet_qcrmcall_inst[i].v4_state == RMNET_QCRMCALL_STATE_START)
    {
      rmnet_qcrmcall_inst[i].v4_state = RMNET_QCRMCALL_STATE_NONE;
      action = QCRMCALL_ACTION_START;
      result.sni.status = FALSE;
    }
    else if (info->meta_sm.state == RMNET_META_SM_NULL_STATE && 
             rmnet_qcrmcall_inst[i].v4_state == RMNET_QCRMCALL_STATE_STOP)
    {
      rmnet_qcrmcall_inst[i].v4_state = RMNET_QCRMCALL_STATE_NONE;
      action = QCRMCALL_ACTION_STOP;
      result.sni.status = TRUE;
    }
    else if (info->meta_sm.state == RMNET_META_SM_NULL_STATE && 
             rmnet_qcrmcall_inst[i].v4_state == RMNET_QCRMCALL_STATE_UP)
    {
      rmnet_qcrmcall_inst[i].v4_state = RMNET_QCRMCALL_STATE_NONE;
    }

    // Send result and reset qcrmcall state
    if (action != QCRMCALL_ACTION_NONE && rmnet_qcrmcall_inst[i].v4_cb)
    {
      result.sni.ip_family = IPV4_ADDR;
      rmnet_qcrmcall_inst[i].v4_cb(action, &result);
      rmnet_qcrmcall_inst[i].v4_cb = NULL;
      return;
    }
  }

  if (info == ip_info->info_v6)
  {
    if (info->meta_sm.state == RMNET_META_SM_NET_IF_UP_STATE && 
        rmnet_qcrmcall_inst[i].v6_state == RMNET_QCRMCALL_STATE_START)
    {
      rmnet_qcrmcall_inst[i].v6_state = RMNET_QCRMCALL_STATE_UP;
      action = QCRMCALL_ACTION_START;
      result.sni.status = TRUE;
    }
    else if (info->meta_sm.state == RMNET_META_SM_NULL_STATE && 
             rmnet_qcrmcall_inst[i].v6_state == RMNET_QCRMCALL_STATE_START)
    {
      rmnet_qcrmcall_inst[i].v6_state = RMNET_QCRMCALL_STATE_NONE;
      action = QCRMCALL_ACTION_START;
      result.sni.status = FALSE;
    }
    else if (info->meta_sm.state == RMNET_META_SM_NULL_STATE && 
             rmnet_qcrmcall_inst[i].v6_state == RMNET_QCRMCALL_STATE_STOP)
    {
      rmnet_qcrmcall_inst[i].v6_state = RMNET_QCRMCALL_STATE_NONE;
      action = QCRMCALL_ACTION_STOP;
      result.sni.status = TRUE;
    }
    else if (info->meta_sm.state == RMNET_META_SM_NULL_STATE && 
             rmnet_qcrmcall_inst[i].v6_state == RMNET_QCRMCALL_STATE_UP)
    {
      rmnet_qcrmcall_inst[i].v6_state = RMNET_QCRMCALL_STATE_NONE;
    }

    // Send result and reset qcrmcall state
    if (action != QCRMCALL_ACTION_NONE && rmnet_qcrmcall_inst[i].v6_cb)
    {
      result.sni.ip_family = IPV6_ADDR;
      rmnet_qcrmcall_inst[i].v6_cb(action, &result);
      rmnet_qcrmcall_inst[i].v6_cb = NULL;
      return;
    }
  }

} /* rmnet_meta_smi_update_qcrmcall_state() */

#endif /* FEATURE_DATA_QCRMCALL */

/*===========================================================================
FUNCTION RMNET_WDS_REG_EVENTS

DESCRIPTION
  This is rmnet registation function for PDN events

PARAMETERS
  callback_type:    function pointer
  user_data_ptr : data pointer to be passed

DEPENDENCIES
  None

RETURN VALUE
  0             success

SIDE EFFECTS
  None.
===========================================================================*/

int rmnet_wds_reg_events
(
 rmnet_wds_event_cback_type  callback_type,
 void                    *user_data_ptr
)
{
  /*allocate the memory for client*/
  DS_SYSTEM_HEAP_MEM_ALLOC(rmnet_wds_evt_buf , sizeof(rmnet_wds_event_buf_type),
                           rmnet_wds_event_buf_type *);
  
  if( rmnet_wds_evt_buf == NULL)
  {  
    DS_MSG0(MSG_LEGACY_ERROR,"Memory not available for allocating the wds callback");
    return -1;
  }
  rmnet_wds_evt_buf->event_cback_f_ptr = callback_type;

  rmnet_wds_evt_buf->user_data_ptr = user_data_ptr;
  return 0;
}

/*===========================================================================
FUNCTION RMNET_GET_ADDR_FAMILY

DESCRIPTION
  This is   function to get the addr family

PARAMETERS
  info:     Rmnet state info pointer

DEPENDENCIES
  None

RETURN VALUE
  ip_version
  
SIDE EFFECTS
  None.
===========================================================================*/

ip_addr_enum_type rmnet_get_addr_family
(
  rmnet_smi_info_type *info
)
{
  switch(info->meta_sm.net_params.ip_family_pref)
  {
   case IPV4_ADDR:
   	return IPV4_ADDR;

   case IPV6_ADDR:
   	return IPV6_ADDR;

   case NON_IP_ADDR:
     return NON_IP_ADDR; 	
  }
  return  IP_ADDR_INVALID;
}

/*===========================================================================
FUNCTION RMNET_META_SM_GET_FREE_MUX_ID

DESCRIPTION
  This is   function to get the free muxid

PARAMETERS
  call_type:     Call type Embedded or Tethered
  apn      :    Apn name

DEPENDENCIES
  None

RETURN VALUE
 Muxid
 
SIDE EFFECTS
  None.
===========================================================================*/

uint8 rmnet_meta_sm_get_free_mux_id
(
  rmnet_meta_sm_call_e_type  call_type,
  char                      *apn
)
{
  uint8 i =0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(call_type == RMNET_CALL_TYPE_LAPTOP)
  {
    return TETHERED_CALL_MUXID;
  }
  for(i = 1; i < MAX_MUX_ID_POOL_SIZE ; i++)
  {
     if(rmnet_call_info[i].in_use == TRUE)
     {
       DS_MSG_SPRINTF_2(MSG_LEGACY_ERROR,"rmnet_meta_sm_get_free_mux_id apn is %s stored apn %s", 
                         apn ,rmnet_call_info[i].apn);
       if((apn == NULL )||(apn[0] == '\0'))
       {
         if(rmnet_call_info[i].null_apn == TRUE)
            return i;
       }
       else
       {
       if(strcmp(apn, rmnet_call_info[i].apn) == 0)
         return i;
     }
  }
  }
  for(i = 1; i < MAX_MUX_ID_POOL_SIZE ; i++)
  {
     if(rmnet_call_info[i].in_use == FALSE)
     {
        rmnet_call_info[i].in_use = TRUE;
        if((apn == NULL )||(apn[0] == '\0'))
        {
           rmnet_call_info[i].null_apn = TRUE;
        }
        else
        {
          strlcpy(rmnet_call_info[i].apn , apn ,sizeof(rmnet_call_info[i].apn));
        }
        return i;
     }
  }
  return 0;

}

/*===========================================================================
FUNCTION RMNET_GET_MUX_ID_FROM_INST

DESCRIPTION
  This is   function to get the rm inst from muxid

PARAMETERS
  clnt_inst:     Rmnet inst

DEPENDENCIES
  None

RETURN VALUE
  ip_version
  
SIDE EFFECTS
  None.
===========================================================================*/

uint8 rmnet_get_mux_id_from_inst
( 
  ds_fwk_clnt_handle_type  client_inst
)
{
  rmnet_smi_info_type *  info = NULL;
/*----------------------------------------------------------------------------------*/
  if (client_inst != DS_FWK_INVALID_CLNT_HANDLE)
  {
    info = rmnet_smi_info[client_inst];
  }

  if ((info != NULL)&&(info->xport_info !=NULL))
  {
    DS_MSG2(MSG_LEGACY_ERROR,"Returning mux_id %d, RMNET inst %d",

	info->xport_info->mux_id, client_inst);	
	return info->xport_info->mux_id;
  }
  return FALSE;

}

/*===========================================================================
FUNCTION RMNET_BIND_MUX_ID_WITH_RM_INST

DESCRIPTION
  This is   function to bind muxid with the rm inst

PARAMETERS
  muxid:      muxid
  rmnet_inst : RM inst

DEPENDENCIES
  None

RETURN VALUE
  ip_version
  
SIDE EFFECTS
  None.
===========================================================================*/

void rmnet_bind_mux_id_with_rm_inst
(
  uint8 mux_id,
  rmnet_instance_e_type rmnet_inst
)
{
  rmnet_smi_info_type *  info = NULL;
/*-------------------------------------------------------------------------------------*/
  
  if (rmnet_inst < RMNET_INSTANCE_MAX)
  {
    info = rmnet_smi_info[rmnet_inst];
  }
  if((info!=NULL)&&(info->xport_info!=NULL))
  {
    info->xport_info->mux_id = mux_id;
  }
}

/*===========================================================================
FUNCTION RMNET_GET_STERAM_ID_FROM_CLIENT

DESCRIPTION
  This is   function to get the sio stream id 

PARAMETERS
  cl_inst:      Rm inst

DEPENDENCIES
  None

RETURN VALUE
sio stream    

SIDE EFFECTS
  None.
===========================================================================*/

sio_stream_id_type rmnet_get_stream_id_from_client
(
 ds_fwk_clnt_handle_type cl_inst
)
{ 

 rmnet_smi_info_type *  info = NULL;
/*--------------------------------------------------------------------------------------*/

  if((rmnet_meta_sm_in_call(cl_inst) == FALSE) ||(cl_inst >= RMNET_INSTANCE_MAX))
  {
   return SIO_NO_STREAM_ID;
  }
  else  
  {
    info = rmnet_smi_info[cl_inst];
  }  
  return info->xport_info->stream_id;
}

static void rmnet_set_ul_signal_handler
(
  rmnet_smi_ip_info_type      *ip_info,
  rmnet_smi_info_type         *info 
)
{
#ifdef FEATURE_DATA_PS_464XLAT
  boolean                is_clat_capable  = FALSE;
  uint8                    rmnet_client =  info->constants.rmnet_inst;
  ds_fwk_s_type           *ds_fwk_ptr = NULL;
  ds_fwk_clnt_info_s_type  client_info;
  ds_fwk_get_clnt_info_from_handle(DS_FWK_CLIENT_RMNET, 
                                   rmnet_client, &client_info);
  ds_fwk_ptr = client_info.fw_inst;
#endif /* FEATURE_DATA_PS_464XLAT */  

  uint8 data_format  = ip_info->xport_info.data_format_cache.qos;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
       
#ifdef FEATURE_DATA_PS_464XLAT
  if((ds_fwk_ptr)&&(ds_fwk_ptr->clat_pdn_cntxt != NULL))
  {
    is_clat_capable = TRUE;
  }
  DS_MSG2(MSG_LEGACY_HIGH,"rmnet_set_ul_signal_handler is_clat_capable %d,"
          "data_format %d",is_clat_capable, data_format);	

  if (is_clat_capable == TRUE)
  {
    if (PS_RMNET_QOS_DISABLED == data_format)
    {
     (void) ps_rm_set_sig_handler(ip_info->rmnet_ul_sig,
                rmnet_ul_opt_clat_ip_no_qos_rx_sig_hdlr,
               (void *)ip_info);
    }
#ifdef FEATURE_DATA_QMI_QOS
    else
    {
      (void) ps_rm_set_sig_handler(ip_info->rmnet_ul_sig,
             rmnet_ul_opt_clat_ip_no_qos_rx_sig_hdlr,
             (void *)ip_info);
    }
#endif
  }
  else 
#endif  /* FEATURE_DATA_PS_464XLAT */  
    {
     if (PS_RMNET_QOS_DISABLED == data_format)
     {
       (void) ps_rm_set_sig_handler(ip_info->rmnet_ul_sig,
                                    rmnet_ul_opt_ip_no_qos_rx_sig_hdlr,
                                    (void *)ip_info);
     }
#ifdef FEATURE_DATA_QMI_QOS
     else
     {
       (void) ps_rm_set_sig_handler(ip_info->rmnet_ul_sig,
                                   rmnet_ul_opt_ip_qos_rx_sig_hdlr,
                                   (void *)ip_info);
     }
#endif /* FEATURE_DATA_QMI_QOS */
 
  }    
}

static void rmnet_sio_rx_non_empty_cback
(
  dsm_watermark_type  * dsm_wm_ptr,
  void                * user_data_ptr
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  PS_RM_SET_SIGNAL((ps_rm_sig_enum_type)user_data_ptr);

} /* ps_dpmi_sio_rx_non_empty_cback() */

/*===========================================================================
  FUNCTION RMNET_GET_SMI_INFO_PTR

  DESCRIPTION
    This function gets the rmnet info on which the call is brought

  PARAMETERS
    cl_inst: FWK Instance on which rmnet instance is bind and call is brought up

  RETURN VALUE
    rmnet smi_info ptr

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
rmnet_smi_info_type * rmnet_get_smi_info_ptr(uint8 rm_inst)
{
  rmnet_smi_info_type *  info = NULL;
 /*---------------------------------------------*/
  
   if((rm_inst >= RMNET_INSTANCE_MAX) ||(rmnet_meta_sm_in_call(rm_inst) == FALSE))
   {
     return NULL;
   }
   else  
   {
     info = rmnet_smi_info[rm_inst];
   } 
  return info;
}

/*===========================================================================
  FUNCTION RMNET_GET_SMI_PTR

  DESCRIPTION
    This function gets the rmnet info on which the call is brought

  PARAMETERS
    cl_inst: FWK Instance on which rmnet instance is bind and call is brought up

  RETURN VALUE
    rmnet smi_info ptr

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
rmnet_smi_info_type * rmnet_get_smi_ptr(uint8 rm_inst)
{
  rmnet_smi_info_type *  info = NULL;
 /*---------------------------------------------*/
  
   if(rm_inst >= RMNET_INSTANCE_MAX)
   {
     return NULL;
   }
   else  
   {
     info = rmnet_smi_info[rm_inst];
   } 
  return info;
}

static int32 rmnet_setup_sio_wm
(
  rmnet_smi_ip_info_type       *ip_info,
  rmnet_smi_info_type          *info
)
{
  int32                 ul_sig_index = 0;
  rmnet_xport_type     *xport_info   = &(ip_info->xport_info);  
  ps_rm_sig_enum_type   rmnet_ul_sig = ip_info->rmnet_ul_sig;  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Setup UL sig for DPM Rm info
  -------------------------------------------------------------------------*/
  if(rmnet_ul_sig == PS_RM_MAX_SIGNALS)
  {
  for (ul_sig_index = 0; ul_sig_index < RMNET_UL_MAX_SIGNALS; ul_sig_index++)
  {
    if (FALSE == rmnet_ul_sig_in_use_tbl[ul_sig_index])
    {
      ip_info->rmnet_ul_sig =
        (ps_rm_sig_enum_type) (PS_RM_RMNET_RX_DATA_Q_SIGNAL_1 + ul_sig_index);
       rmnet_ul_sig_in_use_tbl[ul_sig_index] = TRUE;

      ps_rm_enable_sig(ip_info->rmnet_ul_sig);
      break;
    }
  }
  }
  if (RMNET_UL_MAX_SIGNALS <= ul_sig_index)
  {
    DS_MSG0(MSG_LEGACY_ERROR,"rmnet_setup_sio_wm(): Couldn't find UL sig");
    return -1;
  }

 DS_MSG2(MSG_LEGACY_HIGH,"rmnet_setup_sio_wm(): Assigned UL sig %d for ip_info %p",
                         ip_info->rmnet_ul_sig, ip_info);

  rmnet_set_ul_signal_handler(ip_info,info);
  /*-------------------------------------------------------------------------
    Reset WM counters
  -------------------------------------------------------------------------*/
  if(rmnet_ul_sig == PS_RM_MAX_SIGNALS)
  {
  rmnet_reset_wm_counters(&(xport_info->tx_wmk));
  rmnet_reset_wm_counters(&(xport_info->rx_wmk));
  /*-------------------------------------------------------------------------
    Update DPM Rm info with SIO WM and setup non empty cb
  -------------------------------------------------------------------------*/

   xport_info->rx_wmk.non_empty_func_ptr  = rmnet_sio_rx_non_empty_cback;
   xport_info->rx_wmk.non_empty_func_data = (void *) (ip_info->rmnet_ul_sig);
  }
  return 0;
}

static void rmnet_reset_sio_wm
(
  rmnet_smi_ip_info_type  *ip_info,  
  rmnet_xport_type        *xport_info
)
{
  int32  ul_sig_index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Reset UL sig hdlr and UL sig.

    It may not be needed to reset UL sig hdlr as UL sig is cleared but doing
    it as a safe side. If sig hdlr gets called, and if DPM Rm info is already
    freed, then sig hdlr would access already freed memory
  ------------------------------------------------------------------------*/
  if (ip_info->rmnet_ul_sig < PS_RM_MAX_SIGNALS &&
      ip_info->rmnet_ul_sig > 0)
  {
    (void) ps_rm_set_sig_handler(ip_info->rmnet_ul_sig, NULL, NULL);
    PS_RM_CLR_SIGNAL(ip_info->rmnet_ul_sig);

    ul_sig_index = ip_info->rmnet_ul_sig - PS_RM_RMNET_RX_DATA_Q_SIGNAL_1;
    
    if (ul_sig_index < 0 || ul_sig_index >= RMNET_UL_MAX_SIGNALS)
    {
      DS_MSG1(MSG_LEGACY_ERROR,"ps_dpmi_reset_sio_wm(): Invalid sig index  %d",
                      ul_sig_index); 
      DS_ASSERT(0); 
      return;                      
    }

    rmnet_ul_sig_in_use_tbl[ul_sig_index] = FALSE;

    DS_MSG1(MSG_LEGACY_HIGH,"ps_dpmi_reset_sio_wm(): Reclaimed UL sig %d",
                    ip_info->rmnet_ul_sig);
  }

  /*-------------------------------------------------------------------------
    Reset SIO Tx and Rx WM
  -------------------------------------------------------------------------*/
  if (NULL != xport_info)
  {
    dsm_empty_queue(&(xport_info->rx_wmk));
    xport_info->rx_wmk.non_empty_func_ptr = NULL;
    xport_info->rx_wmk.non_empty_func_data = NULL;

    dsm_empty_queue(&(xport_info->tx_wmk));
    xport_info->tx_wmk.non_empty_func_ptr = NULL;
    xport_info->tx_wmk.non_empty_func_data = NULL;
  }

  return;
}


/*===========================================================================
FUNCTION RMNET_GET_EP_ID_FROM_RMNET_INST

DESCRIPTION
  This is   function to get the EP ID from Rmnet instance

PARAMETERS
  cl_inst:      Rm inst

DEPENDENCIES
  None

RETURN VALUE
  EP_ID
  
SIDE EFFECTS
  None.
===========================================================================*/
uint32 rmnet_get_ep_id_from_rmnet_inst
(
  ds_fwk_clnt_handle_type  client_inst
)
{
   rmnet_smi_info_type *  info = NULL;
 /*----------------------------------------------------------------------------------*/
  if (client_inst != DS_FWK_INVALID_CLNT_HANDLE)
  {
	info = rmnet_smi_info[client_inst];
  }

  if ((info != NULL)&&(info->xport_info !=NULL))
  {
	DS_MSG2(MSG_LEGACY_ERROR,"Returning mux_id %d, RMNET inst %d",

	info->xport_info->mux_id, client_inst); 
	return info->xport_info->ep_id;
  }
  return 0;
}

static void rmnet_invoke_wds_cb
(
  rmnet_instance_e_type   rmnet_inst,
  ds_pdn_event_enum_type  pdn_event,
  ds_pdn_event_info_u_type event_info
)
{
  if((rmnet_wds_evt_buf != NULL)&& (rmnet_wds_evt_buf->event_cback_f_ptr != NULL))
  {
     rmnet_wds_evt_buf->event_cback_f_ptr(rmnet_inst,
                                          pdn_event,
                                          event_info,
                                          rmnet_wds_evt_buf->user_data_ptr);
  }
}

/*===========================================================================
FUNCTION RMNET_GET_RMNET_INST_FROM_STREAM_ID

DESCRIPTION
  This is function to get the Rmnet instance from stream ID

PARAMETERS
  stream_id -  stream id of RMNET call
  ip_type -    IP Type of the call

DEPENDENCIES
  None

RETURN VALUE
  EP_ID
  
SIDE EFFECTS
  None.
===========================================================================*/
rmnet_instance_e_type rmnet_get_rmnet_inst_from_stream_id
(
  sio_stream_id_type  stream_id,
  ip_addr_enum_type   ip_type,
  void                **rmnet_sm_info 
)
{
  rmnet_smi_info_type          *info = NULL;
  rmnet_instance_e_type         rmnet_inst = RMNET_INSTANCE_MAX;
  int                            i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  for (i = 0; i < RMNET_INSTANCE_MAX; i++)
  {
    info = rmnet_smi_info[i];
    if ( info != NULL &&
         info->xport_info->stream_id == stream_id &&
         info->meta_sm.net_params.ip_family_pref == ip_type)
    {
      rmnet_inst = info->constants.rmnet_inst;
      *rmnet_sm_info = info;
      break;
    }
  }
  DS_MSG2(MSG_LEGACY_HIGH,"rmnet_inst_from_ep_and_mux_id rmnet inst %d,"
          "rmnet_sm_info 0x%x", rmnet_inst, *rmnet_sm_info);

  return rmnet_inst;
}

#ifdef TEST_FRAMEWORK
#error code not present
#endif
