/*===========================================================================
                          D S _ R M S M I . C 

DESCRIPTION
  The Data Services UMTS specific RM State Machine functions 
  used for pdp-ip/pdp-ppp calls which are used by the state machine(s) 
  for both pdp-ip and pdp-ppp calls.
  
  NOTE:
  This file contains functions which are private to DS_RMSM and 
  should not be used by the clients of DS_RMSM directly.
 
EXTERNALIZED FUNCTIONS
Copyright (c) 2002-2018 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/dataiot.mpss/2.0/3gpp/rmsm/src/ds_rmsm.c#7 $ 
$DateTime: 2022/10/06 21:19:58 $ $Author: pwbldsvc $

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "data_msg.h"
#define FEATURE_DSM_WM_CB
#include "memory.h"
#include "err.h"
#include "msg.h"
#include "queue.h"
#include "sys.h"
#include "rcinit_rex.h"
#include "dstask.h"
#include "dstaski.h"
#include "ds3gsiolib.h"
#include "ps_svc.h"
#include "ps_ppp.h"
#include "ds_rmsmi.h"
#include "ds_pdn_manager.h"
#include "ds_pdn_nv_manager.h"
#include "ds_rmsm_ip.h"
#include "ps_logging_defs.h"
#include "ps_meta_info.h"
#define DEFAULT_RMSM_AGG_TIMER_TIMEOUT 10
#define RMSM_AGG_TIMER_NV_INDEX 1
#include "ps_byte.h"
#include "ds_3gpp_rmsm_ip_common.h"
#include "ds_system_heap.h"
#include "ds3gsiolib_ex.h"
#include "dsati.h"
#include "ps_ppp_sig.h"
#include "ds_3gpp_rmsm_ati.h"
#include "ds_fwki.h"

/*===========================================================================

                           EXTERNAL DECLARATIONS

===========================================================================*/
/* RMSM UL watermark levels configurable */
  uint32 DS_RMSM_UL_LO        = 15000;
  uint32 DS_RMSM_UL_HI        = 25000;
  uint32 DS_RMSM_UL_DNE       = 58000;


  uint32 DS_RMSM_DL_LO         = 50000;
  uint32 DS_RMSM_DL_HI         = 75000;
  uint32 DS_RMSM_DL_DNE        = 108000;

/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                            INTERNAL DEFINITIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

static timer_type smd_notify_delay_timer[DS_RMSM_MAX_RM_DEV_INSTANCES];

/* Timeout for smd_notify_delay_timer, read from nv in 
   ds_rmsmi_setup_sio_watermarks */
static uint8 ds_smem_tx_delay = 0; 

/*---------------------------------------------------------------------------
  The control block of the Generic RM state machine.
---------------------------------------------------------------------------*/
static ds_rmsmi_info_type* ds_rmsmi_info[DS_RMSM_MAX_RM_DEV_INSTANCES] = {NULL};

dsat_mode_enum_type ds_rmsmi_at_escape_sequence_handler( void );

void ds_rmsmi_at_return_to_online_data_handler
(
  /* Pointer to the watermark for data from SIO */
  dsm_watermark_type     **from_sio_wm_ptr,   

  /* Pointer to the watermark for data to SIO. */
  dsm_watermark_type     **to_sio_wm_ptr,
  
  /* Pointer to SIO RX mode-specific handler function */
  sio_rx_func_ptr_type    *mode_hdlr_sio_rx_func_ptr,

  /* Port associated */
  ds3g_siolib_port_e_type  port_id
);

void ds_rmsmi_at_return_to_online_data_complete_handler ( void );

static void ds_rmsm_tx_cb
(
  ds_fwk_clnt_handle_type    client_handle,
  dsm_item_type              *dsm_item,
  void                      *user_data_ptr
);


ds3g_siolib_func_tbl_type ds_rmsmi_sio_func_table =
{
  ds_rmsmi_at_escape_sequence_handler, /* AT escape seq handler*/
  NULL, /* DTR changed sig handler */
  NULL, /* DTR when AT&D = 0 handler */
  ds_rmsmi_at_return_to_online_data_handler,
  /* return to online data handler */
  ds_rmsmi_at_return_to_online_data_complete_handler,
  /* return to online data complete handler */
  NULL, /* return to online cmd mode complete handler */
  NULL, /* auto answer handler */
};


/*---------------------------------------------------------------------------
  HS_USB AGGREGATION flush if packet < packet size.
---------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_USB_AGGRN_IOCTL_FLUSH
#ifdef FEATURE_HS_USB
#define HS_USB_FLUSH_PACKET_SIZE 100
static uint16 hs_usb_flush_packet_size = 0;
#endif /* FEATURE_HS_USB */
#endif/* FEATURE_DATA_USB_AGGRN_IOCTL_FLUSH */

/*---------------------------------------------------------------------------
  Enable chopping of rx packets into bite sized fragments before delivery
  to PPP/HDLC. This ensures an upper bound on the amount of data buffer 
  required on Um side before flow control takes effect.
---------------------------------------------------------------------------*/
#define SPLIT_RX_PACKETS

/*---------------------------------------------------------------------------
  Default maximum size of packet injected into PPP/HDLC
---------------------------------------------------------------------------*/
#define MAX_RX_PACKET_SZ    5000

#ifdef SPLIT_RX_PACKETS

/*---------------------------------------------------------------------------
  Variable to hold maximum allowed rx packet size
---------------------------------------------------------------------------*/
static uint16 ds_rmsmi_rx_pkt_sz = MAX_RX_PACKET_SZ;

/*---------------------------------------------------------------------------
  DSM packet pending delivery to PPP/HDLC
---------------------------------------------------------------------------*/
static dsm_item_type * ds_rmsmi_pending_rx_pkt_ptr[DS_RMSM_MAX_RM_DEV_INSTANCES] = {NULL}; 

/*---------------------------------------------------------------------------
  Helper macro to get pending rx packet ptr
---------------------------------------------------------------------------*/
#define DS_RMSMI_GET_PENDING_RX_PKT(rm_dev) (ds_rmsmi_pending_rx_pkt_ptr[rm_dev])

/*---------------------------------------------------------------------------
  Helper macro to set pending rx packet ptr
---------------------------------------------------------------------------*/
#define DS_RMSMI_SET_PENDING_RX_PKT(pkt_ptr,rm_dev) \
            (ds_rmsmi_pending_rx_pkt_ptr[rm_dev] = (pkt_ptr))

/*---------------------------------------------------------------------------
  Helper macro to free pending rx packet
---------------------------------------------------------------------------*/
#define DS_RMSMI_FREE_PENDING_RX_PKT(rm_dev) \
        { \
          if (ds_rmsmi_pending_rx_pkt_ptr[rm_dev]) \
          { \
            dsm_free_packet(&ds_rmsmi_pending_rx_pkt_ptr[rm_dev]); \
          } \
        }

/*---------------------------------------------------------------------------
  Helper macro to get max rx packet size
---------------------------------------------------------------------------*/
#define DS_RMSMI_GET_RX_PKT_SZ() (ds_rmsmi_rx_pkt_sz)

/*---------------------------------------------------------------------------
  Helper macro to set max rx packet size (currently unused)
---------------------------------------------------------------------------*/
#define DS_RMSMI_SET_RX_PKT_SZ(pkt_sz) \
            (ds_rmsmi_rx_pkt_sz = (pkt_sz))

void ds_rmsmi_set_rx_data_can_be_pulled_flag 
(
  ds_rmsmi_info_type* rmsmi_info_ptr
)
{
  DS_RMSM_ENTER_CRIT_SECTION(&rmsmi_info_ptr->rx_data_crit_sec);
  rmsmi_info_ptr->rx_data_can_be_pulled = TRUE;
  DS_RMSM_LEAVE_CRIT_SECTION(&rmsmi_info_ptr->rx_data_crit_sec);
}

void ds_rmsmi_reset_rx_data_can_be_pulled_flag 
(
  ds_rmsmi_info_type* rmsmi_info_ptr
)
{
  DS_RMSM_ENTER_CRIT_SECTION(&rmsmi_info_ptr->rx_data_crit_sec);
  rmsmi_info_ptr->rx_data_can_be_pulled = FALSE;
  DS_RMSM_LEAVE_CRIT_SECTION(&rmsmi_info_ptr->rx_data_crit_sec);
}

boolean ds_rmsmi_get_rx_data_can_be_pulled_flag 
(
  ds_rmsmi_info_type* rmsmi_info_ptr
)
{
  boolean flag;

  DS_RMSM_ENTER_CRIT_SECTION(&rmsmi_info_ptr->rx_data_crit_sec);
  flag = rmsmi_info_ptr->rx_data_can_be_pulled;
  DS_RMSM_LEAVE_CRIT_SECTION(&rmsmi_info_ptr->rx_data_crit_sec);

  return flag;
}

/*===========================================================================
  FUNCTION DS_RMSMI_NEXT_RX_PACKET()

  DESCRIPTION
    This function returns a pointer to the next rx packet for delivery to 
    the upper layer (PPP/HDLC). If necessary, packet is chopped to guarantee
    packet does not exceed maximum allowed packet size. 

  PARAMETERS
    rx_wm : DSM watermark ptr of queue holding incoming rx packets
    rm_dev: Rm Device Instance

  RETURN VALUE
    Ptr to dsm packet chain

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
dsm_item_type * ds_rmsmi_next_rx_packet
(
  dsm_watermark_type           *rx_wm,
  ds_rmsm_rm_dev_instance_type rm_dev
)
{
  dsm_item_type * pkt_ptr;
  dsm_item_type * pkt_tail_ptr = NULL;
  uint16 max_pkt_sz;

  if ((pkt_ptr = DS_RMSMI_GET_PENDING_RX_PKT(rm_dev)) == NULL) 
  {
    pkt_ptr = dsm_dequeue(rx_wm);
  }

  if ((pkt_ptr) && ((max_pkt_sz = DS_RMSMI_GET_RX_PKT_SZ()) > 0))
  {
    (void) dsm_split_packet
           (
             &pkt_ptr, 
             &pkt_tail_ptr, 
             max_pkt_sz, 
             DSM_DUP_ITEM_POOL
           );
    DS_RMSMI_SET_PENDING_RX_PKT(pkt_tail_ptr,rm_dev);
  }

  return pkt_ptr;
}

#else /* SPLIT_RX_PACKETS */

/*---------------------------------------------------------------------------
  Stub definition of macro that does nothing
---------------------------------------------------------------------------*/
#define DS_RMSMI_FREE_PENDING_RX_PKT()

/*===========================================================================
  FUNCTION DS_RMSMI_NEXT_RX_PACKET()

  DESCRIPTION
    This function returns a pointer to the next rx packet for delivery to 
    the upper layer (PPP/HDLC).

  PARAMETERS
    rx_wm : DSM watermark ptr of queue holding incoming rx packets
    rm_dev: Rm Device Instance

  RETURN VALUE
    Ptr to dsm packet chain

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
dsm_item_type * ds_rmsmi_next_rx_packet
(
  dsm_watermark_type           *rx_wm,
  ds_rmsm_rm_dev_instance_type rm_dev
)
{
  return dsm_dequeue(rx_wm);
} /*ds_rmsmi_next_rx_packet*/

#endif 

/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                        EXTERNAL FUNCTION DEFINTIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

/*===========================================================================
  FUNCTION DS_RMSM_INIT()

  DESCRIPTION
    This function initializes the UMTS RM state machine. Once initialized, the 
    state machine manages SIO iface for packet data calls on an appropriate 
    Um iface. 
   
   It also calls the pdp-ip and pdp-ppp specific sub state machine
   initializations.
   
   We just call the internal initialization function
    
  PARAMETERS
    None
    
  RETURN VALUE
    TRUE on success
    FALSE on failure

  DEPENDENCIES
    Should only be called once at powerup

  SIDE EFFECTS
    None
===========================================================================*/
boolean ds_rmsm_init( void )
{
  boolean result = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  result = ds_rmsmi_init_all_instances();
  if (FALSE == result)
  {
    DS_MSG0(MSG_LEGACY_ERROR,"RMSM initialization failed ");
  }

  /* Initialize RMSM - ATI interface */
  ds_3gpp_rmsm_ati_init_all_instances();

  return result;
} /* ds_rmsm_init() */


/*===========================================================================
  FUNCTION DS_RMSM_DIAL_STR_CB()

  DESCRIPTION
    This function handles the dial stings for pkt data calls and initiates 
    the data call over the appropriate pkt iface.  This function is 
    registered with ATCOP ATD handler table.
    
    It queries the pdp profile registry to get the profile associated with
    the context number specified in the dial strig. Based on that it sets
    the "sub-state" to either "pdp-ip" or "pdp-ppp" and kick starts the 
    state machine

  PARAMETERS
    dial_string: The dial string to be processed
    digit_mode: Flag to tell whether dial string contains digits only

  RETURN VALUE
    Function pointer to abort handler

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

dsat_call_abort_cb_type ds_rmsm_dial_str_cb
(
  const byte *dial_string,                        /* dial string           */
  boolean    digit_mode,                          /* non-digit or digits   */
  dsat_dial_modifier_info_type  *modifiers /* Dial sting modifiers  */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_MSG0(MSG_LEGACY_HIGH,"Received dial string for Pkt data call");
  
  return (ds_rmsmi_handle_atd_on_rm( ds_rmsmi_get_next_rm_dev_instance(),
                                         dial_string, digit_mode,
                                         modifiers));

} /* ds_rmsm_dial_str_cb() */


/*===========================================================================
                            Forward Declarations
===========================================================================*/

/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                          LOCAL FUNCTION DEFINTIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

 /*===========================================================================
  FUNCTION DS_RMSMI_GET_PS_PPP_RM_RX_SIGNAL

  DESCRIPTION
    This function fetches the PPP Rm Rx Signal for the corresponding Rm Device.

  PARAMETERS
    Rm Device Instance

  RETURN VALUE
    PPP Rm Rx Signal

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
ps_sig_enum_type ds_rmsmi_get_ps_ppp_rm_rx_signal
(
  ds_rmsm_rm_dev_instance_type rm_dev
)
{
  if (rm_dev == DS_RMSM_RM_DEV_INSTANCE_0)
  {
    return PPP_RM_RX_SIGNAL;
  }
  else if (rm_dev == DS_RMSM_RM_DEV_INSTANCE_1)
  {
    return PPP_RM_2_RX_SIGNAL;
  }
/*
  else if (rm_dev == DS_RMSM_RM_DEV_INSTANCE_2)
  {
    return PPP_RM_3_RX_SIGNAL;
  }
  else if (rm_dev == DS_RMSM_RM_DEV_INSTANCE_2)
  {
    return PPP_RM_4_RX_SIGNAL;
}*/
  else
  {
    return PS_MAX_SIGNALS;
  }
}
/*===========================================================================
FUNCTION DS_RMSMI_GET_RM_DEV_FROM_PORT_ID

DESCRIPTION
  This function is used to get the Rm Device instance corresponding to a Port Id

DEPENDENCIES
   None
  
PARAMETERS
  Port Id

RETURN VALUE
   Rm Dev Instance
    
SIDE EFFECTS
   None

===========================================================================*/
ds_rmsm_rm_dev_instance_type
ds_rmsmi_get_rm_dev_from_port_id(ds3g_siolib_port_e_type port_id)
{
  ds_rmsmi_info_type *rmsmi_info_ptr; 
  ds_rmsm_rm_dev_instance_type rm_dev;

  for (rm_dev=0; rm_dev<DS_RMSM_MAX_RM_DEV_INSTANCES; rm_dev++)
  {
    rmsmi_info_ptr = ds_rmsmi_get_rmsmi_info_ptr(rm_dev);
    if (rmsmi_info_ptr != NULL &&
        rmsmi_info_ptr->port_id == port_id)
    {
      return rm_dev;
    }
  }
  return DS_RMSM_INVALID_RM_DEV_INSTANCE;

}/* ds_rmsmi_get_sio_portid */

/*===========================================================================
FUNCTION SMD_NOTIFY_TIMER_EXPIRED

DESCRIPTION
  
DEPENDENCIES
   None
  
PARAMETERS
  Port Id

RETURN VALUE
   Rm Dev Instance
    
SIDE EFFECTS
   None

===========================================================================*/
void smd_notify_timer_expired(timer_cb_data_type data)
{
  sio_ioctl_param_type wait_param;
  ds_rmsmi_info_type* rmsmi_info_ptr =
    (ds_rmsmi_info_type*)data;

  //The timer expired, call the foll. ioctl in sio to call notifywrite immediately
  wait_param.wm_wait = FALSE;

  if (rmsmi_info_ptr != NULL)
  {
    ds3g_siolib_ex_ioctl(SIO_IOCTL_WM_SET_WAIT, &wait_param,
                         rmsmi_info_ptr->port_id);
  }
} /*smd_notify_timer_expired*/

/*===========================================================================
FUNCTION DS_RMSMI_TX_WM_NON_EMPTY_FUNC

DESCRIPTION
  
DEPENDENCIES
   None
  
PARAMETERS
  Port Id

RETURN VALUE
   Rm Dev Instance
    
SIDE EFFECTS
   None

===========================================================================*/
void ds_rmsmi_tx_wm_non_empty_func
(
  struct dsm_watermark_type_s *wm,
  void* callback_data
)
{
  sio_ioctl_param_type wait_param;
  ds_rmsmi_info_type *rmsmi_info_ptr = 
    (ds_rmsmi_info_type*)callback_data;

  //Call the foll. ioctl in sio to set the wait which will delay the call to notifywrite (to ARM11).
  wait_param.wm_wait = TRUE;

  if (rmsmi_info_ptr != NULL)
  {
    ds3g_siolib_ex_ioctl(SIO_IOCTL_WM_SET_WAIT, &wait_param, 
                         rmsmi_info_ptr->port_id);
    timer_set(&smd_notify_delay_timer[rmsmi_info_ptr->instance],
            ds_smem_tx_delay,  // this is the delay in MS
            0,
            T_MSEC);
  }
} /*ds_rmsmi_tx_wm_non_empty_func*/

/*===========================================================================
                            Forward Declarations
===========================================================================*/

static void ds_rmsmi_ppprm_event_handler_cb
(
  ppp_dev_enum_type   ppp_dev,
  ppp_protocol_e_type protocol,
  ppp_event_e_type    ppp_event,
  void*               user_data,
  uint32              session_handle,
  ppp_fail_reason_e_type  fail_reason
);

/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                          LOCAL FUNCTION DEFINTIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

/*===========================================================================
  FUNCTION DS_RMSMI_RX_WM_NON_EMPTY_FUNC()

  DESCRIPTION
    This function performs the 'Non Empty' event processing for the PACKET
    DATA SERVICES shared queue. This function is called when the SIO RX
    data stream watermark item gets its first item enqueued. It sets its
    idle bit which allows UM_PKT_CTRL to end cta dormancy if needed.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void ds_rmsmi_rx_wm_non_empty_func
( 
  struct dsm_watermark_type_s *wm,
  void* callback_data 
)
{
  ds_rmsmi_info_type *rmsmi_info_ptr = (ds_rmsmi_info_type*)callback_data;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(rmsmi_info_ptr == NULL)
    return; 

  if ( RX_DATA_CAN_BE_PULLED(rmsmi_info_ptr))
  {
    ps_set_signal( rmsmi_info_ptr->ps_ppp_rm_rx_signal );
  }
  return;
} /* ds_rmsmi_wm_non_empty_func() */

/*===========================================================================
  FUNCTION DS_RMSMI_RX_WM_LOWATER_FUNC()

  DESCRIPTION
   This function enables inbound serial flow, when the lowater point in the
   packet watermark is reached.

   This function is called from RLC TX processing, which occurs in the 
   decode ISR.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void ds_rmsmi_rx_wm_lowater_func
( 
  struct dsm_watermark_type_s *wm,
  void                        *callback_data 
)
{
  ds_rmsmi_info_type *rmsmi_info_ptr = 
    (ds_rmsmi_info_type*)callback_data;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(rmsmi_info_ptr == NULL)
    return;

  ds3g_siolib_ex_set_inbound_flow( DS_FLOW_PKT_WM_MASK, DS_FLOW_ENABLE,
                                   rmsmi_info_ptr->port_id );

} /* ds_rmsmi_wm_lowater_func() */

/*===========================================================================
  FUNCTION DS_RMSMI_RX_WM_HIWATER_FUNC()

  DESCRIPTION
   This function disables inbound serial flow, when the hiwater point in the
   packet watermark is reached.

   This is called from the serial RX ISR.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void ds_rmsmi_rx_wm_hiwater_func
( 
  struct dsm_watermark_type_s *wm,
  void* callback_data 
)
{
  ds_rmsmi_info_type *rmsmi_info_ptr = 
    (ds_rmsmi_info_type*)callback_data;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(rmsmi_info_ptr == NULL)
    return;

  ds3g_siolib_ex_set_inbound_flow( DS_FLOW_PKT_WM_MASK, DS_FLOW_DISABLE,
                                   rmsmi_info_ptr->port_id );
} /* ds_rmsmi_wm_hiwater_func() */

/*===========================================================================
  FUNCTION DS_RMSMI_TX_WM_LOWATER_FUNC()

  DESCRIPTION
   This function enables inbound serial flow, when the lowater point in the
   packet watermark is reached.

   This function is called from RLC TX processing, which occurs in the 
   decode ISR.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void ds_rmsmi_tx_wm_lowater_func
( 
  struct dsm_watermark_type_s *wm,
  void* callback_data
)
{
  ds_command_type       *cmd_ptr;               /* DS Task message */
  ds_rmsm_instance_type inst;
  ds_rmsmi_info_type   *rmsmi_info_ptr = (ds_rmsmi_info_type*)callback_data;
  ds_rmsm_instance_type *data_ptr = NULL;
  ds_local_err_val_e_type err_val = DS_LOCAL_ERR_VAL_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(rmsmi_info_ptr == NULL)
  {
    err_val = DS_LOCAL_ERR_VAL_0;
    goto func_exit;
  }
  /*-------------------------------------------------------------------------
    To support multiple instances later, get the instance from user
    parameter which is passed back in the callback
  -----------------------------------------------------------------------*/
  inst.rm_dev_instance = rmsmi_info_ptr->instance;
  inst.sm_instance = DS_RMSM_V4_SM_INSTANCE;

  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_rmsm_instance_type));
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    err_val = DS_LOCAL_ERR_VAL_1;
    goto func_exit;
  }

  /*-------------------------------------------------------------------------
    We enable the flow on the SIO interface. The rm event handler calls
    a function registered by um controller which signals the base station
    to enable the forward link flow on the Um link.
  -----------------------------------------------------------------------*/
  cmd_ptr->hdr.cmd_id = 0; //tbd_aks DS_CMD_3GPP_IFACE_FLOW_ENABLE;
  data_ptr = (ds_rmsm_instance_type*)cmd_ptr->cmd_payload_ptr;

  data_ptr->rm_dev_instance = (int)inst.rm_dev_instance;
  data_ptr->sm_instance = (int)inst.sm_instance;
  data_ptr->data_ptr = (void*)DS_FLOW_PKT_WM_MASK;

  ds_put_cmd(cmd_ptr);
  
func_exit:
  if(err_val != DS_LOCAL_ERR_VAL_NONE)
    DS_MSG1(MSG_LEGACY_HIGH,"ds_rmsmi_tx_wm_lowater_func err_val: %d",err_val);
  return;
} /* ds_rmsmi_wm_lowater_func() */

/*===========================================================================
  FUNCTION DS_RMSMI_TX_WM_HIWATER_FUNC()

  DESCRIPTION
   This function disables inbound Um flow, when the hiwater point in the
   tx watermark is reached.

   This is called from the serial RX ISR.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void ds_rmsmi_tx_wm_hiwater_func
( 
  struct dsm_watermark_type_s *wm,
  void                        *callback_data 
)
{
  ds_command_type         *cmd_ptr;               /* DS Task message */
  sio_ioctl_param_type    wait_param;
  ds_rmsm_instance_type   inst;
  ds_rmsmi_info_type *rmsmi_info_ptr = (ds_rmsmi_info_type*)callback_data;
  ds_rmsm_instance_type  *data_ptr = NULL;
  ds_local_err_val_e_type err_val = DS_LOCAL_ERR_VAL_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(rmsmi_info_ptr == NULL)
  {
    err_val = DS_LOCAL_ERR_VAL_0;
    goto func_exit;
  }
  /*-------------------------------------------------------------------------
    To support multiple instances later, get the instance from user 
    parameter which is passed back in the callback
  -------------------------------------------------------------------------*/
  inst.rm_dev_instance = rmsmi_info_ptr->instance;
  inst.sm_instance = DS_RMSM_V4_SM_INSTANCE;

  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_rmsm_instance_type));
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    err_val = DS_LOCAL_ERR_VAL_0;
    goto func_exit;
  }

  /*-------------------------------------------------------------------------
	We disable the flow on the SIO interface. The rm event handler calls
	a function registered by um controller which signals the base station
	to disable the forward link flow on the Um link.
  -------------------------------------------------------------------------*/
  cmd_ptr->hdr.cmd_id = 0; //tbd_aks DS_CMD_3GPP_IFACE_FLOW_DISABLE;
  data_ptr = (ds_rmsm_instance_type*)cmd_ptr->cmd_payload_ptr;

  data_ptr->rm_dev_instance = (int)inst.rm_dev_instance;
  data_ptr->sm_instance = (int)inst.sm_instance;
  data_ptr->data_ptr = (void*)DS_FLOW_PKT_WM_MASK;

  ds_put_cmd(cmd_ptr);

  /* There is no wait if the timer value is 0. So no need to clear it */
  if(ds_smem_tx_delay != 0) 
  {
    /*Since the high watermark has hit, call the foll. ioctl in sio to clear the wait 
      This will call notifywrite and send data across immediately.*/
      
    wait_param.wm_wait = FALSE;
    if (rmsmi_info_ptr != NULL)
    {
      ds3g_siolib_ex_ioctl(SIO_IOCTL_WM_SET_WAIT, &wait_param, rmsmi_info_ptr->port_id);
      timer_clr(&smd_notify_delay_timer[inst.rm_dev_instance], T_MSEC);
    }
  }

func_exit:
  if(err_val != DS_LOCAL_ERR_VAL_NONE)
    DS_MSG1(MSG_LEGACY_HIGH,"ds_rmsmi_tx_wm_hiwater_func err_val: %d",err_val);
  return;
} /* ds_rmsmi_tx_wm_hiwater_func() */

/*===========================================================================
  FUNCTION DS_RMSMI_ALLOCATE_RMSM_INSTANCE

  DESCRIPTION
   This function allocates a RmSm instance.

  PARAMETERS
    Rm Device instance which needs to be allocated

  RETURN VALUE
    Pointer to the newly allocated RmSm instance.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static ds_rmsmi_info_type* ds_rmsmi_allocate_rmsm_instance
(
  ds_rmsm_rm_dev_instance_type rm_dev
)
{
  if (rm_dev > DS_RMSM_INVALID_RM_DEV_INSTANCE &&
      rm_dev < DS_RMSM_MAX_RM_DEV_INSTANCES)
  {
    if (ds_rmsmi_info[rm_dev] == NULL)
    {
      DS_SYSTEM_HEAP_MEM_ALLOC(ds_rmsmi_info[rm_dev], sizeof(ds_rmsmi_info_type), 
                               ds_rmsmi_info_type *);
      return ds_rmsmi_info[rm_dev];
    }
  }
  return NULL;
}

/*===========================================================================
  FUNCTION DS_RMSMI_FREE_RMSM_INSTANCE

  DESCRIPTION
   This function allocates a RmSm instance.

  PARAMETERS
    Rm Device instance which needs to be allocated

  RETURN VALUE
    Pointer to the newly allocated RmSm instance.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

void ds_rmsmi_free_rmsm_instance
(
  ds_rmsm_rm_dev_instance_type rm_dev
)
{
  ds_rmsmi_info_type* rmsm_info_ptr = NULL;
  ds_rmsm_sm_instance_type sm_instance = DS_RMSM_V4_SM_INSTANCE;
  ppp_dev_enum_type ppp_dev;
  ds_3gpp_rmsm_ip_common_info_type *info_ptr = NULL;
  
  if (ds_rmsmi_info[rm_dev] == NULL)
  {
    DS_MSG0(MSG_LEGACY_HIGH,"RM device not allocated or already freed");
    return;
  }
  else
  {
    rmsm_info_ptr = ds_rmsmi_info[rm_dev];
    dsm_queue_destroy(&rmsm_info_ptr->sio_tx_wmk);
    dsm_queue_destroy(&rmsm_info_ptr->sio_rx_wmk);
	
    info_ptr = rmsm_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr;
    if(info_ptr != NULL)
    {
      /*Free the timers,critical sec and then the common info pointer*/
      PS_TIMER_FREE_HANDLE(info_ptr->v4_info.ipcp_res_del_timer);
      PS_TIMER_FREE_HANDLE(info_ptr->v6_info.ipv6cp_res_del_timer);
      DS_MSG0(MSG_LEGACY_HIGH,"Freeing the NAK delay timers !!!");
      DS_RMSM_DELETE_CRIT_SECTION(&(info_ptr->v4_info.ipi_sm_info_crit_sec));
      DS_RMSM_DELETE_CRIT_SECTION(&(info_ptr->v6_info.ipi_sm_info_crit_sec));
      DS_MSG0(MSG_LEGACY_HIGH,"Freeing the critical sections !!!");
      DS_SYSTEM_HEAP_MEM_FREE(rmsm_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr);
    }
    
    for (sm_instance = DS_RMSM_V4_SM_INSTANCE; 
          sm_instance < DS_RMSM_MAX_SM_INSTANCES; 
          sm_instance++)
    {
      DS_SYSTEM_HEAP_MEM_FREE(rmsm_info_ptr->sm_info_ptr[sm_instance]->sio_inst);
      DS_SYSTEM_HEAP_MEM_FREE(rmsm_info_ptr->sm_info_ptr[sm_instance]);
    }

    ppp_dev = ds_rmsmi_get_ppp_dev(rm_dev);
    ppp_event_dealloc(ppp_dev, &(rmsm_info_ptr->ppp_handle));

    DS_RMSM_DELETE_CRIT_SECTION(&(rmsm_info_ptr->rx_data_crit_sec));
	
    DS_SYSTEM_HEAP_MEM_FREE(ds_rmsmi_info[rm_dev]);
    ds_rmsmi_info[rm_dev] = NULL;
  }     
  return;
}

/*====================================================================
  FUNCTION ds_rmsmi_sio_cleanup ()

  DESCRIPTION
    This is used to perform SIO port related cleanup before call tear down

  PARAMETERS
    Rm Dev Instance

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
====================================================================*/
void ds_rmsmi_sio_cleanup ( ds_rmsm_rm_dev_instance_type rm_dev )
{
  ds_rmsmi_info_type *rmsmi_info_ptr = 
    ds_rmsmi_get_rmsmi_info_ptr(rm_dev);
	
  if(rmsmi_info_ptr == NULL)
  {
    DS_MSG0(MSG_LEGACY_ERROR,"rmsmi_info_ptr is NULL");
    return;
  }

  if (ds3g_siolib_ex_set_cd_state
       ( DS3G_SIOLIB_REMOTE_CARRIER_OFF, rmsmi_info_ptr->port_id ) == 
          DS3G_SIOLIB_NO_PORT_OPEN)
  {
    DS_MSG0(MSG_LEGACY_ERROR,"Error setting CD State");  
  }
#ifdef FEATURE_DATA_ATCOP
  dsat_send_result_code_ex( DSAT_NO_CARRIER,rmsmi_info_ptr->port_id );
#endif /* FEATURE_DATA_ATCOP */
  ds_rmsmi_siolib_deregister_callbacks(rmsmi_info_ptr->port_id);

  ds3g_siolib_ex_release_port (rmsmi_info_ptr->port_id);
  ds_rmsmi_set_sio_port_id(rm_dev,DS3G_SIOLIB_PORT_NONE);
}

/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                        EXTERNAL FUNCTION DEFINTIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

/*===========================================================================
  FUNCTION DS_RMSMI_INIT_ALL_INSTANCES()

  DESCRIPTION
    This function initializes all the instances of UMTS RM state machines. 
    There is one state machine per Rm interface that is enabled for Data.
    
    As part of initialization it creates SIO iface, init 
    queues/wms, allocates event buffers and resets state variables.
   
   IT also calls the pdp-ip and pdp-ppp specific sub state machine initializations
    
  PARAMETERS
    None
    
  RETURN VALUE
    TRUE on success
    FALSE on failure

  DEPENDENCIES
    Should only be called once at powerup

  SIDE EFFECTS
    None
===========================================================================*/
boolean ds_rmsmi_init_all_instances( void )
{
  ds_rmsm_rm_dev_instance_type rm_dev_instance = 
                                         DS_RMSM_INVALID_RM_DEV_INSTANCE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_rmsmi_sio_func_table.at_escape_sequence_ex_handler =
    ds_rmsmi_process_esc_seq_cb;
  ds_rmsmi_sio_func_table.dtr_changed_sig_ex_handler =
    ds_rmsmi_process_dtr_sig_cb;
  ds_rmsmi_sio_func_table.dtr_changed_sig_ex_handler_ampd0 =
    ds_rmsmi_process_dtr_sig_cb;

  if(FALSE == ds_register_cmd_hdlr(DS_MODULE_RMSM,ds_3gpp_rmsm_ip_common_process_cmd ))
  {
    DS_ERROR_LOG_0("Not able to register rmsm cmd handler with DS task..Abort!!!");
	  ASSERT(0);
    return FALSE;
  }

  /*-------------------------------------------------------------------------
  Register the DS_FWK callback function
-------------------------------------------------------------------------*/
  ds_fwk_reg_events(DS_FWK_CLIENT_RMSM,ds_rmsm_pdn_event_cback,NULL);
  ds_fwk_reg_tx_cb(DS_FWK_CLIENT_RMSM, &ds_rmsm_tx_cb ,NULL);
  
  for (rm_dev_instance = DS_RMSM_RM_DEV_INSTANCE_0; 
       rm_dev_instance < DS_RMSM_MAX_RM_DEV_INSTANCES; 
       rm_dev_instance++)
  {
    /*----------------------------------------------------------------------- 
      Initialize smd_notify_delay_timers
      ----------------------------------------------------------------------*/
    memset(&smd_notify_delay_timer[rm_dev_instance],0,sizeof(timer_type));
  }

  return TRUE;
} /* ds_rmsmi_init_all_instances() */

/*===========================================================================
  FUNCTION DS_RMSMI_INIT_RMDEV_INSTANCE()

  DESCRIPTION
    This function initializes the passed instance of UMTS RM state machine. 
    There is one state machine per Rm interface that is enabled for Data.
    As part of initialization it creates SIO iface, init 
    queues/wms, allocates event buffers and resets state variables.
   IT also calls the pdp-ip specific sub state machine initializations
    
  PARAMETERS
    
  RETURN VALUE

  DEPENDENCIES

  SIDE EFFECTS
    None
===========================================================================*/

ds_rmsmi_info_type* ds_rmsmi_init_rmdev_instance (
    ds_rmsm_rm_dev_instance_type rm_dev_instance
)
{
  ds_rmsmi_info_type       *rmsmi_info_ptr = NULL;
  ds_rmsm_sm_instance_type sm_instance = DS_RMSM_V4_SM_INSTANCE;
  ds_rmsmi_sm_info_type    *sm_info_ptr = NULL;
  ppp_dev_enum_type        ppp_dev = PPP_INVALID_DEV;
  ds_local_err_val_e_type  err_val = DS_LOCAL_ERR_VAL_NONE;
  /*------------------------------------------------------------------------*/
  
  rmsmi_info_ptr = ds_rmsmi_allocate_rmsm_instance(rm_dev_instance);
  if(rmsmi_info_ptr == NULL)
  {
    err_val = DS_LOCAL_ERR_VAL_0;
    goto func_exit;
  }

  /*Initialise SIO TX wm ie for outgoing data to TE*/
  memset(&rmsmi_info_ptr->sio_tx_wmk, 0, sizeof(dsm_watermark_type));
  memset(&rmsmi_info_ptr->sio_tx_wmk_q, 0, sizeof(q_type));
  dsm_queue_init(&rmsmi_info_ptr->sio_tx_wmk, 
                   (int)0x7FFFFFFF,
                   &rmsmi_info_ptr->sio_tx_wmk_q);

  /*Initialise SIO RX wm ie incoming data from TE*/                   
  memset(&rmsmi_info_ptr->sio_rx_wmk, 0, sizeof(dsm_watermark_type));
  memset(&rmsmi_info_ptr->sio_rx_wmk_q, 0, sizeof(q_type));
  dsm_queue_init(&rmsmi_info_ptr->sio_rx_wmk,
                   (int)0x7FFFFFFF,
                   &rmsmi_info_ptr->sio_rx_wmk_q);

  DS_RMSM_INIT_CRIT_SECTION( &rmsmi_info_ptr->rx_data_crit_sec );

  RESET_RX_DATA_CAN_BE_PULLED(rmsmi_info_ptr);

  rmsmi_info_ptr->ps_ppp_rm_rx_signal = 
      ds_rmsmi_get_ps_ppp_rm_rx_signal(rm_dev_instance);

  rmsmi_info_ptr->instance = rm_dev_instance;

  rmsmi_info_ptr->port_id = DS3G_SIOLIB_PORT_NONE;

  rmsmi_info_ptr->rm_ppp_config_ptr = NULL;

  rmsmi_info_ptr->ppp_up_delay_timer = 
	ps_timer_alloc(ds_3gpp_rmsm_ip_common_ppp_up_delay_timer_cb, 
	                                          (void*)rmsmi_info_ptr);
  rmsmi_info_ptr->config_wait_timer = 
	ps_timer_alloc(ds_3gpp_rmsm_ip_common_config_wait_timer_cb,
	                                          (void*)rmsmi_info_ptr);

  ppp_dev = ds_rmsmi_get_ppp_dev(rm_dev_instance);
  if (ds_rmsmi_validate_ppp_dev(ppp_dev) == FALSE)
  {
    err_val = DS_LOCAL_ERR_VAL_1;
    goto func_exit;
  }

  rmsmi_info_ptr->ppp_handle = ppp_event_alloc(
                                      ppp_dev,
                                      ds_rmsmi_ppprm_event_handler_cb,
                                      (void*)rmsmi_info_ptr,
                                      NULL,
                                      NULL,
                                      ds_rmsmi_ppprm_event_handler_cb,
                                      (void*)rmsmi_info_ptr,
                                      ds_rmsmi_ppprm_event_handler_cb,
                                      (void*)rmsmi_info_ptr
                                    );
  if( NULL == rmsmi_info_ptr->ppp_handle )
  {
    err_val = DS_LOCAL_ERR_VAL_2;
    goto func_exit;
  }

  if (ds_3gpp_rmsm_ip_common_init(rm_dev_instance) == FALSE)
  {
    err_val = DS_LOCAL_ERR_VAL_3;
    goto func_exit;
  }
     
  rmsmi_info_ptr->sm_type = DS_RMSM_INVALID_SM_TYPE;

  for (sm_instance = DS_RMSM_V4_SM_INSTANCE; 
        sm_instance < DS_RMSM_MAX_SM_INSTANCES; 
        sm_instance++)
  {
    DS_SYSTEM_HEAP_MEM_ALLOC(sm_info_ptr, sizeof(ds_rmsmi_sm_info_type),
                             ds_rmsmi_sm_info_type*);
    if (sm_info_ptr == NULL)
    {
      DS_MSG2(MSG_LEGACY_ERROR,"Could not allocate memory for sm instance %d,"
                               "call instance %d",sm_instance,rm_dev_instance);
      err_val = DS_LOCAL_ERR_VAL_4;
      goto func_exit;
    }

	  rmsmi_info_ptr->sm_info_ptr[sm_instance] = sm_info_ptr;
    memset(sm_info_ptr, 0x0, sizeof(ds_rmsmi_sm_info_type)); 
    sm_info_ptr->instance = sm_instance;
    sm_info_ptr->fw_inst = NULL;
    sm_info_ptr->flow_enabled = 0;
    sm_info_ptr->recd_um_down = FALSE;
  
    /*Allocate Sio instance*/
    DS_SYSTEM_HEAP_MEM_ALLOC(sm_info_ptr->sio_inst,
                        sizeof(ds_rmsm_sio_instance),ds_rmsm_sio_instance *);
  
    /*-----------------------------------------------------------------------
     Initialize the sub state machines. They initialize the sm_specific_info
     in the structure
    -----------------------------------------------------------------------*/  
    if (ds_rmsm_ip_init(sm_info_ptr) == FALSE)
    {
      err_val = DS_LOCAL_ERR_VAL_5;
      goto func_exit;
    }
  
    sm_info_ptr->vobj_data_ptr = (void*)rmsmi_info_ptr;
    sm_info_ptr->inited = TRUE;
  }
        
  return rmsmi_info_ptr;
          
func_exit:
  if(err_val != DS_LOCAL_ERR_VAL_NONE)
    DS_MSG1(MSG_LEGACY_ERROR,"ds_rmsmi_init_rmdev_instance : err %d",err_val);
  
  return NULL;
}

/*===========================================================================
  FUNCTION DS_RMSMI_HANDLE_ATD_ON_RM

  DESCRIPTION
    This function handles the dial strings for pkt data calls on a 
    specific RM instance and initiates the data call over the appropriate 
    pkt iface.
    
    It queries the pdp profile registry to get the profile associated with
    the context number specified in the dial strig. Based on that it sets
    the "sub-state" to either "pdp-ip" or "pdp-ppp" and kick starts the 
    state machine

  PARAMETERS
    Rm Device inst. on which atd
    dial_string: The dial string to be processed
    digit_mode: Flag to tell whether dial string contains digits only
    Dial sting modifiers

  RETURN VALUE
    Function pointer to abort handler

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
dsat_call_abort_cb_type ds_rmsmi_handle_atd_on_rm
(
  ds_rmsm_rm_dev_instance_type rm_dev_instance, 
  const byte                  *dial_string,                      
  boolean                      digit_mode,                          
  const dsat_dial_modifier_info_type *modifiers
)
{ 
#ifdef FEATURE_DATA_RMSM
  ds_profile_status_etype            profile_result = DS_PROFILE_RESULT_FAIL;
  ds3g_siolib_port_alloc_info_s_type alloc_info;
  ds_rmsm_instance_type              inst = {0};
  ds_rmsmi_info_type                 *rmsmi_info_ptr = NULL;
  ds3g_siolib_port_e_type            port_id = DS3G_SIOLIB_PORT_NONE;
  ds3g_siolib_port_e_type            active_port = DS3G_SIOLIB_PORT_NONE;
  ds_profile_pdp_type_e              pdp_type;
  ds_local_err_val_e_type            err_val = DS_LOCAL_ERR_VAL_NONE;
  uint32                             profile_num = INVALID_PROFILE_NUMBER;
  ds_profile_type_s                  prof_context = {{0}};
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  active_port = ds3g_siolib_get_active_port();

  inst.rm_dev_instance = rm_dev_instance;
  inst.sm_instance = DS_RMSM_V4_SM_INSTANCE;
  if((rm_dev_instance < DS_RMSM_RM_DEV_INSTANCE_0) || 
     (rm_dev_instance >= DS_RMSM_MAX_RM_DEV_INSTANCES))
  {
  #ifdef FEATURE_DATA_ATCOP
    dsat_send_result_code_ex( DSAT_NO_CARRIER,active_port);
  #endif /* FEATURE_DATA_ATCOP */
    err_val = DS_LOCAL_ERR_VAL_0;
    goto func_exit;
  }

  /*-------------------------------------------------------------------------
    Get the context id from the dial string, if available.
  -------------------------------------------------------------------------*/
  if ((modifiers->modifier_present == TRUE) && 
      (modifiers->modifier == DSAT_CID_DIAL_MODIFIER))
  {
    profile_num = modifiers->modifier_info.cid.cid_val;
  }
  else
  {
    profile_num = ds_profile_get_profile_number_for_tethered_calls();
  }

  profile_result = ds_profile_setting_from_profile_id((int8)profile_num,
                                                       &prof_context);
  if(profile_result != DS_PROFILE_RESULT_SUCCESS)
  {
  #ifdef FEATURE_DATA_ATCOP
    dsat_send_result_code_ex( DSAT_NO_CARRIER,active_port);
  #endif /* FEATURE_DATA_ATCOP */
    err_val = DS_LOCAL_ERR_VAL_1;
    goto func_exit;
  }
  else
  {
    DS_MSG1(MSG_LEGACY_HIGH,"Profile:%d valid & primary: using it",
                               profile_num);
  }

  pdp_type = prof_context.context.pdp_type;
  if(pdp_type != DS_PROFILE_PDP_IPV4 &&
     pdp_type != DS_PROFILE_PDP_IPV6 &&
     pdp_type != DS_PROFILE_PDP_IPV4V6)
  {
  #ifdef FEATURE_DATA_ATCOP
    dsat_send_result_code_ex( DSAT_NO_CARRIER,active_port);
  #endif /* FEATURE_DATA_ATCOP */
    err_val = DS_LOCAL_ERR_VAL_2;
    goto func_exit;
  }

  /*-------------------------------------------------------------------------
    Try to allocate SIO port for external data call.  Without successfully
    downing the semaphore, the call cannot proceed.
  -------------------------------------------------------------------------*/
  memset((void*)&alloc_info, 0x0, sizeof(ds3g_siolib_port_alloc_info_s_type));
  alloc_info.mode = DS3G_SIOLIB_PORT_ALLOC_MODE_AUTOMATIC;
  alloc_info.call_dir = DS3G_SIOLIB_PORT_CALL_DIR_ORIGINATED;

  if ( DS3G_SIOLIB_PORT_NONE !=
       (port_id = ds3g_siolib_ex_allocate_port( &alloc_info, DS_CALL_TYPE_PKT )) )
  {
    /*-------------------------------------------------------------------------
      Register for desired events (dtr signal) with siolib.  If the
      registration fails drop the call since siolib is already in use.
      Otherwise indicate to siolib that we are in call now.
    -------------------------------------------------------------------------*/
    if( FALSE == ds3g_siolib_ex_register_callback_func
                   (&ds_rmsmi_sio_func_table,port_id ))
    {
      if (ds3g_siolib_ex_set_cd_state
            (DS3G_SIOLIB_REMOTE_CARRIER_OFF, port_id) ==
            DS3G_SIOLIB_NO_PORT_OPEN)
      {
        DS_MSG0(MSG_LEGACY_ERROR,"Error setting CD State");  
      }
   #ifdef FEATURE_DATA_ATCOP
      dsat_send_result_code_ex( DSAT_NO_CARRIER,port_id );
   #endif /* FEATURE_DATA_ATCOP */        
      err_val = DS_LOCAL_ERR_VAL_3;
      goto func_exit;
    }
  }
  else
  {
    DS_MSG0(MSG_LEGACY_ERROR,"Unable to allocate SIOLIB port");

    if (ds3g_siolib_ex_set_cd_state (DS3G_SIOLIB_REMOTE_CARRIER_OFF,port_id) ==
          DS3G_SIOLIB_NO_PORT_OPEN)
    {
      DS_MSG0(MSG_LEGACY_ERROR,"Error setting CD State");  
    }
   #ifdef FEATURE_DATA_ATCOP
    dsat_send_result_code_ex( DSAT_NO_CARRIER,port_id );
   #endif /* FEATURE_DATA_ATCOP */       
    err_val = DS_LOCAL_ERR_VAL_4;
    goto func_exit;
  }
  
  /*Allocate and initialise RMSM instance on the assigned RM device*/
  rmsmi_info_ptr = ds_rmsmi_init_rmdev_instance(rm_dev_instance);
  if (rmsmi_info_ptr == NULL)
  {   
    if (ds3g_siolib_ex_set_cd_state
          (DS3G_SIOLIB_REMOTE_CARRIER_OFF,active_port) ==
            DS3G_SIOLIB_NO_PORT_OPEN)
  {
      DS_MSG0(MSG_LEGACY_ERROR,"Error setting CD State");  
  }
    ds_rmsmi_sio_cleanup(rm_dev_instance);
  #ifdef FEATURE_DATA_ATCOP
    dsat_send_result_code_ex( DSAT_NO_CARRIER, active_port );
  #endif /* FEATURE_DATA_ATCOP */
    err_val = DS_LOCAL_ERR_VAL_5;
    goto func_exit;
  }

  rmsmi_info_ptr->profile_id = profile_num;
  memscpy(&rmsmi_info_ptr->profile_info, sizeof(ds_profile_type_s), 
          &prof_context,sizeof(ds_profile_type_s));
  ds_rmsmi_set_sio_port_id(rmsmi_info_ptr->instance, port_id);

    rmsmi_info_ptr->sm_type = DS_RMSM_SM_TYPE_PDP_IP;
    inst.sm_instance = DS_RMSM_ANY_SM_INSTANCE;

  ds3g_siolib_ex_set_call_coming_up(rmsmi_info_ptr->port_id,TRUE );

  /*--------------------------------------------------------------------------
    Capture dial string to internal RMSM info table, 
    NV_PKT_DIAL_STRING_SIZE is size of dial string with ending NULL
  ---------------------------------------------------------------------------*/
  memscpy( rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->ppp_info.dial_string, 
           NV_PKT_DIAL_STRING_SIZE-1,
           dial_string, NV_PKT_DIAL_STRING_SIZE - 1 );
  
  rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->ppp_info. 
    dial_string[NV_PKT_DIAL_STRING_SIZE - 1] = '\0';
  
  rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->ppp_info. 
    valid_fields |= (uint32) NET_CFG_PRM_DIAL_STR_MASK;

  DS_MSG1(MSG_LEGACY_HIGH,"DS_RMSMI_DIAL_STR_CB: dial string is %s",
    rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->ppp_info.dial_string);

  ds_3gpp_rmsm_ip_common_post_event( &inst, DS_CMD_3GPP_RMSM_RM_WANTS_PKT_CALL );

  return (ds_rmsmi_call_abort_cb);

func_exit:
  if(err_val != DS_LOCAL_ERR_VAL_NONE)
    DS_MSG1(MSG_LEGACY_ERROR,"ds_rmsmi_handle_atd_on_rm : err %d",err_val);
  
  return NULL;
  
#else
  dsat_connect_rate_s_type				connect_rate = {0}; 
  ds3g_siolib_port_e_type  active_port = ds3g_siolib_get_active_port();

  connect_rate.call_type = DSAT_CALL_TYPE_PS;
  connect_rate.comm_prot = DSAT_COMM_PROT_PACKET;
  connect_rate.rate.ps_call.dl_rate = MAX_BIT_RATE_150000KBPS;
  connect_rate.rate.ps_call.ul_rate = MAX_BIT_RATE_50000KBPS;
  dsat_report_rate_on_connect( connect_rate );
  dsat_send_result_code_ex( DSAT_CONNECT,active_port);
  return (ds_rmsmi_call_abort_cb);
#endif /*FEATURE_DATA_RMSM*/
} /* ds_rmsmi_dial_str_cb() */
  
/*===========================================================================
  FUNCTION DS_RMSMI_CALL_ABORT_CB()

  DESCRIPTION
    This function is registered as abort handler with ATCOP. If ATCOP
    receives an AT cmd while processing an abortable cmd, this function gets
    called.  It is registered along with the dial string handler. It posts
    appropriate event to DS_RMSM which initiates the call termination.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void ds_rmsmi_call_abort_cb( void )
{
  ds_rmsm_sm_instance_type sm_instance = DS_RMSM_V4_SM_INSTANCE;
  ds_rmsm_instance_type inst;
  ds3g_siolib_port_e_type port_id = ds3g_siolib_get_active_port();

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (port_id <= DS3G_SIOLIB_PORT_NONE && 
      port_id >= DS3G_SIOLIB_PORT_MAX)
  {
    DS_MSG3(MSG_LEGACY_HIGH, "Invalid Port Id", 0, 0, 0);
    return;
  }

  inst.rm_dev_instance = ds_rmsmi_get_rm_dev_from_port_id(port_id);

  if (ds_rmsmi_validate_rm_dev_instance(inst.rm_dev_instance) == FALSE)
  {
    return;
  }

  inst.sm_instance = sm_instance;

  DS_MSG0(MSG_LEGACY_HIGH, "Call aborted");
#ifdef FEATURE_DATA_RMSM
  ds_3gpp_rmsm_ip_common_post_event( &inst, DS_CMD_3GPP_RMSM_RM_PPP_DOWN );
#endif 

  return;
} /* ds_rmsmi_call_abort_cb() */

/*===========================================================================
  FUNCTION DS_RMSMI_PROCESS_DTR_SIG_CB()

  DESCRIPTION
    This function processes the change in dtr state.  If DTR is deasserted
    appropriate event is posted to ds_rmsm which initiates the call 
    termination. DTR assert is ignored for now.

  PARAMETERS
    is_asserted: flag which indicates current DTR state -
                 TRUE: DTR asserted, FALSE: DTR deasserted

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void ds_rmsmi_process_dtr_sig_cb
(
  boolean                          is_asserted,
  void*                            cb_data
)
{
  ds_rmsm_sm_instance_type sm_instance = DS_RMSM_ANY_SM_INSTANCE;
  ds_rmsm_instance_type inst;
  ds_rmsm_rm_dev_instance_type rm_dev;
  ds3g_siolib_port_e_type port_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(cb_data != NULL);

  port_id = (ds3g_siolib_port_e_type)cb_data;

  rm_dev = ds_rmsmi_get_rm_dev_from_port_id(port_id);

  if (ds_rmsmi_validate_rm_dev_instance(rm_dev) == FALSE)
  {
    return;
  }

  inst.rm_dev_instance = rm_dev;
  inst.sm_instance = sm_instance;

  /*-------------------------------------------------------------------------
    Currently only dtr dropped is handled, post appropriate event
    ------------------------------------------------------------------------*/
  DS_MSG1(MSG_LEGACY_HIGH, "is_asserted = %d", is_asserted);
#ifdef FEATURE_DATA_RMSM 
  if ( is_asserted == FALSE )
  {
    ds_3gpp_rmsm_ip_common_post_event( &inst, DS_CMD_3GPP_RMSM_RM_PPP_DOWN );
  }
#endif 
  return;

} /* ds_rmsmi_process_dtr_sig_cb() */


/*===========================================================================
  FUNCTION DS_RMSMI_SETUP_SIO_WATERMARKS()

  DESCRIPTION
    This function configures the SIO packet mode watermark items
    to operate for the laptop pkt data calls handled by DS_RMSM.

  PARAMETERS
    rm_dev_instance   - RM Device Instance 
    *tx_wm_ptr - ptr to sio transmit (to laptop) watermark
    *rx_wm_ptr - ptr to sio receive (from laptop) watermark

  RETURN VALUE
    None

  DEPENDENCIES
    Hi/Lo Watermark Settings: The Lo and Hi watermark values and the don't
    exceed values for each watermark is inter-dependent with the DSM small
    and large item counts (DSM_LARGE_ITEM_CNT and DSM_SMALL_ITEM_CNT) in the
    DSM module. Any change to lo/hi watermark or don't exceed values can
    cause asserts due to insufficient memory items available.

  SIDE EFFECTS
    None
===========================================================================*/
void ds_rmsmi_setup_sio_watermarks
(
  ds_rmsm_rm_dev_instance_type rm_dev_instance,
  dsm_watermark_type           *tx_wm_ptr,
  dsm_watermark_type           *rx_wm_ptr,
  q_type                       *tx_wmk_q_ptr,
  q_type                       *rx_wmk_q_ptr 
)
{
  nv_stat_enum_type        nv_status;
  nv_item_type             ds_nv_item;
  boolean                  is_device_bt = FALSE; /* is device Bluetooth? */
  #ifdef FEATURE_DATA_ATCOP
  dsat_num_item_type       ipr_val = 0;
  #endif /* FEATURE_DATA_ATCOP */
  ds_rmsmi_info_type       *rmsmi_info_ptr = NULL;
  ds_local_err_val_e_type  err_val = DS_LOCAL_ERR_VAL_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #ifdef FEATURE_DATA_ATCOP
  ipr_val = (dsat_num_item_type)dsatutil_get_val(DSAT_EXT_IPR_IDX,0,NUM_TYPE);
  #endif /* FEATURE_DATA_ATCOP */
  if( rdm_get_device( RDM_DATA_SRVC ) == RDM_BT_NA_DEV )
  {
    is_device_bt = TRUE;
  }

  rmsmi_info_ptr = ds_rmsmi_get_rmsmi_info_ptr(rm_dev_instance);
  if(rmsmi_info_ptr == NULL)
  {
    err_val = DS_LOCAL_ERR_VAL_0;
    goto func_exit;
  }

  /*-------------------------------------------------------------------------
    Free any pending rx packet.
  -------------------------------------------------------------------------*/
  DS_RMSMI_FREE_PENDING_RX_PKT(rm_dev_instance);
  
  /*-------------------------------------------------------------------------
                             SETUP SIO TX WATERMARK

    This watermark queue is to be used for transmitting data from mobile to 
    TE2 over SIO.
  -------------------------------------------------------------------------*/
  dsm_queue_destroy(tx_wm_ptr);
  dsm_queue_init(tx_wm_ptr,DS_RMSM_DL_DNE,tx_wmk_q_ptr);
  tx_wm_ptr->lo_watermark          = DS_RMSM_DL_LO;
  tx_wm_ptr->hi_watermark          = DS_RMSM_DL_HI;
  tx_wm_ptr->dont_exceed_cnt       = DS_RMSM_DL_DNE;

  tx_wm_ptr->gone_empty_func_ptr   = NULL;
  tx_wm_ptr->non_empty_func_ptr    = NULL;
  tx_wm_ptr->lowater_func_ptr      = ds_rmsmi_tx_wm_lowater_func;
  tx_wm_ptr->lowater_func_data     = (void*) rmsmi_info_ptr;
  tx_wm_ptr->hiwater_func_ptr      = ds_rmsmi_tx_wm_hiwater_func;
  tx_wm_ptr->hiwater_func_data     = (void*) rmsmi_info_ptr;
  tx_wm_ptr->each_enqueue_func_ptr = NULL;

  /*-------------------------------------------------------------------------
                             SETUP SIO RX WATERMARK
  -------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
    This watermark queue is to be used for transmitting data from mobile to 
    TE2 over SIO.
    
    If SIO is using high speed serial card and CTS flow control is available
    then use the higher values for waternarks.  Also for bluetooth, provide
    more cushion between hi and don't exceed levels.
  -------------------------------------------------------------------------*/

  dsm_queue_destroy(rx_wm_ptr);
  dsm_queue_init(rx_wm_ptr,15000,rx_wmk_q_ptr);
  #ifdef FEATURE_DATA_ATCOP
  if( ( ds3g_siolib_is_baud_valid( (sio_bitrate_type)ipr_val ) ) &&
       ( ipr_val >= SIO_BITRATE_115200 ) &&
       ( ipr_val <  SIO_BITRATE_BEST ) 
     )
  {
    if( is_device_bt == TRUE )
    {
      rx_wm_ptr->lo_watermark        = 1500;
      rx_wm_ptr->hi_watermark        = 5000;
      rx_wm_ptr->dont_exceed_cnt     = 38000;
    }
    else
    {
      rx_wm_ptr->lo_watermark    = DS_RMSM_UL_LO;
      rx_wm_ptr->hi_watermark    = DS_RMSM_UL_HI;
      rx_wm_ptr->dont_exceed_cnt = DS_RMSM_UL_DNE;
    }
  }
  else
  #endif /* FEATURE_DATA_ATCOP */
  {
    rx_wm_ptr->lo_watermark        = 400;
    rx_wm_ptr->hi_watermark        = 1300;

    if( is_device_bt == TRUE )
    {
      rx_wm_ptr->dont_exceed_cnt   = 35000;
    }
    else
    {
      rx_wm_ptr->dont_exceed_cnt   = 35000;
    }
  }

  rx_wm_ptr->gone_empty_func_ptr   = NULL;
  rx_wm_ptr->non_empty_func_ptr    = ds_rmsmi_rx_wm_non_empty_func;
  rx_wm_ptr->non_empty_func_data   = (void*) rmsmi_info_ptr;
  rx_wm_ptr->lowater_func_ptr      = ds_rmsmi_rx_wm_lowater_func;
  rx_wm_ptr->lowater_func_data     = (void*) rmsmi_info_ptr;
  rx_wm_ptr->hiwater_func_ptr      = ds_rmsmi_rx_wm_hiwater_func;
  rx_wm_ptr->hiwater_func_data     = (void*) rmsmi_info_ptr;
  rx_wm_ptr->each_enqueue_func_ptr = NULL;

  /*-------------------------------------------------------------------------
    Reset the total_rcvd_cnt, as a packet data call can really set this
    number very high.
  -------------------------------------------------------------------------*/
  tx_wm_ptr->total_rcvd_cnt        = 0;
  rx_wm_ptr->total_rcvd_cnt        = 0;

  /*Initialize timer value to 0 before reading from NV*/
  ds_smem_tx_delay = 0;

  timer_def(&smd_notify_delay_timer[rm_dev_instance], NULL, NULL, 0, 
             smd_notify_timer_expired, (unsigned long)rmsmi_info_ptr);
  nv_status = ds_pdn_nv_manager_read_legacy_nv(NV_DATA_AGG_TIMER_I, &ds_nv_item);
  if( nv_status == NV_DONE_S)
  {
     ds_smem_tx_delay = (uint8)ds_nv_item.data_agg_timer[RMSM_AGG_TIMER_NV_INDEX];
	 
     /*read the element at index 1, because it corresponds to smd data port 1.*/
     if(ds_smem_tx_delay == 0) {
         DS_MSG0(MSG_LEGACY_HIGH,"NV read, smd_notify_delay_timer is 0. "
                  "Disabling data aggregation");
     } else {
         DS_MSG1(MSG_LEGACY_HIGH,"NV read, smd_notify_delay_timer : [%d]", 
                           ds_smem_tx_delay);
     }
  }
  else 
  {
    DS_MSG1(MSG_LEGACY_HIGH,"Bad NV read status for smd_notify_delay_timer: %d,"
                      " Disabling timer", nv_status);
    /*cannot read nv item for the timeout value for aggregation timer. So disable the timer*/
    ds_smem_tx_delay = 0;
  }

  tx_wm_ptr->non_empty_func_ptr    = ds_rmsmi_tx_wm_non_empty_func;
  tx_wm_ptr->non_empty_func_data   = (void*)rmsmi_info_ptr;
  
func_exit:
  if(err_val != DS_LOCAL_ERR_VAL_NONE)
  {
    DS_MSG1(MSG_LEGACY_ERROR,"ds_rmsmi_setup_sio_watermarks :"
                            "local err %d",err_val);
  }
  return;   
} /* ds_rmsmi_setup_sio_watermarks() */

/*===========================================================================
FUNCTION DS_RMSMI_SIO_TX_PPP_DATA

DESCRIPTION
  This function is called to transmit a packet coming on Um PPP
  interface onto Rm interface.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_rmsmi_sio_tx_ppp_data
(
  dsm_item_type  **item_head_ptr,
  void           *meta_info_ptr,
  void           *user_data_ptr
)
{
  ds_rmsmi_info_type* rmsmi_info_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ps_meta_info_type *temp_meta_info_ptr = (ps_meta_info_type *)meta_info_ptr;
  ASSERT(user_data_ptr != NULL);
  rmsmi_info_ptr = (ds_rmsmi_info_type*)user_data_ptr;

#ifdef FEATURE_DATA_USB_AGGRN_IOCTL_FLUSH
#if defined(FEATURE_HS_USB) && !defined(FEATURE_NO_USB_HW_ON_PROCESSOR)
  int length;
#endif /* FEATURE_HS_USB && !FEATURE_NO_USB_HW_ON_PROCESSOR*/
#endif /* FEATURE_DATA_USB_AGGRN_IOCTL_FLUSH */

  (void)ds3g_siolib_ex_setup_tx( *item_head_ptr, FALSE, 
                                 rmsmi_info_ptr->port_id );
#ifdef FEATURE_DATA_USB_AGGRN_IOCTL_FLUSH
#if defined(FEATURE_HS_USB) && !defined(FEATURE_NO_USB_HW_ON_PROCESSOR)
  if (rmsmi_info_ptr->port_id == DS3G_SIOLIB_SMD_DS_PORT || 
      rmsmi_info_ptr->port_id == DS3G_SIOLIB_SMD_DATA11_PORT)
  {
    length = dsm_length_packet(*item_head_ptr);
    if(length <= hs_usb_flush_packet_size)
    {
      ds3g_siolib_ex_flush_tx(NULL,rmsmi_info_ptr->port_id);
    }
  }  
#endif /* FEATURE_HS_USB && !FEATURE_NO_USB_HW_ON_PROCESSOR*/
#endif /* FEATURE_DATA_USB_AGGRN_IOCTL_FLUSH */

  *item_head_ptr = NULL;

  /*------------------------------------------------------------------------
   Free the meta_info_ptr here. meta_info_ptr would be set if filters were
   executed on Um iface (typically for GPS/LBS apps).
  ------------------------------------------------------------------------*/
  if (NULL != temp_meta_info_ptr)
  {
    PS_META_INFO_FREE(&temp_meta_info_ptr);
  }
} /* ds_rmsmi_sio_tx_ppp_data() */

/*===========================================================================
FUNCTION DS_RMSMI_SIO_RX_PPP_DATA

DESCRIPTION
  This function is called to receive the next PPP packet, coming on the Rm
  interface, for rx processing.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
dsm_item_type* ds_rmsmi_sio_rx_ppp_data( void *user_data )
{
  ds_rmsm_instance_type inst;
  ds_rmsmi_info_type *rmsmi_info_ptr;
  uint8                  i = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  inst.sm_instance = DS_RMSM_V4_SM_INSTANCE;

  rmsmi_info_ptr = (ds_rmsmi_info_type*)user_data;

  if (rmsmi_info_ptr == NULL)
  {
    for(i=0; i < DS_RMSM_MAX_RM_DEV_INSTANCES; i++)
    {
      rmsmi_info_ptr = ds_rmsmi_info[i];
      if (rmsmi_info_ptr != NULL)
      {
        DS_MSG0(MSG_LEGACY_LOW,"RmSmi Info ptr is NULL "
                          "emptying rx_wm and not doing further rx processing");
        dsm_empty_queue(&rmsmi_info_ptr->sio_rx_wmk);
      }
    }
    return NULL;
  }

  if (rmsmi_info_ptr->instance <= DS_RMSM_INVALID_RM_DEV_INSTANCE || 
      rmsmi_info_ptr->instance >= DS_RMSM_MAX_RM_DEV_INSTANCES)
  {
    DS_MSG1(MSG_LEGACY_HIGH, "rmsmi_info_ptr->instance is invalid: "
                       "rmsmi_info_ptr->instance is %d", rmsmi_info_ptr->instance);
    return NULL;
  }

  inst.rm_dev_instance = rmsmi_info_ptr->instance;

  if ( RX_DATA_CAN_BE_PULLED(rmsmi_info_ptr) )
  {
    return ds_rmsmi_next_rx_packet( &rmsmi_info_ptr->sio_rx_wmk,
                                        inst.rm_dev_instance);
  }

  return NULL;
  
} /* ds_rmsmi_sio_rx_ppp_data() */

/*===========================================================================
FUNCTION   DS_RMSMI_PPPRM_EVENT_HANDLER_CB()

DESCRIPTION
  This callback function is called when one of the registered PPP events
  occur on RM PPP device. As part of event handling appropriate event is
  posted to DSMTS_RMSM.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_rmsmi_ppprm_event_handler_cb
(
  ppp_dev_enum_type   ppp_dev,
  ppp_protocol_e_type protocol,
  ppp_event_e_type    ppp_event,
  void*               user_data,
  uint32              session_handle,
  ppp_fail_reason_e_type  fail_reason
)
{
  ds_rmsmi_info_type            *rmsmi_info_ptr = NULL;
  ds_rmsmi_sm_info_type *sm_info_ptr = NULL;
  ds_rmsm_sm_instance_type sm_instance = DS_RMSM_V4_SM_INSTANCE;
  ds_rmsm_instance_type inst;
  ds_profile_pdp_type_e         pdp_type;
 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
  DS_MSG3(MSG_LEGACY_HIGH, "Recd %d ppp event", ppp_event, 0, 0 );
    
  /*lint -e730 */
 /* ASSERT( ppp_dev == PPP_RM_DEV ||
          ppp_dev == PPP_RM_2_DEV ||
          ppp_dev == PPP_RM_3_DEV ||
          ppp_dev == PPP_RM_4_DEV);*/
  /*lint +e730 */

  inst.rm_dev_instance = ds_rmsmi_get_rm_dev_from_ppp_dev(ppp_dev);
  inst.sm_instance = sm_instance;

  rmsmi_info_ptr = ds_rmsmi_get_rmsmi_info_ptr(inst.rm_dev_instance);

  if (rmsmi_info_ptr == NULL)
  {
    DATA_ERR_FATAL("RmSmi Info ptr is NULL");
    return;
  }
  pdp_type = DS_RMSMI_GET_PDP_TYPE(rmsmi_info_ptr);

  if (protocol == PPP_PROTOCOL_IPCP || 
      (protocol == PPP_PROTOCOL_LCP && pdp_type == DS_PROFILE_PDP_IPV4) ||
      (protocol == PPP_PROTOCOL_LCP && pdp_type == DS_PROFILE_PDP_PPP))
  {
    inst.sm_instance = DS_RMSM_V4_SM_INSTANCE;
  }

  else if (protocol == PPP_PROTOCOL_IPV6CP || 
          (protocol == PPP_PROTOCOL_LCP && pdp_type == DS_PROFILE_PDP_IPV6))
  {
    inst.sm_instance = DS_RMSM_V6_SM_INSTANCE;
  }

  else if (protocol == PPP_PROTOCOL_LCP && pdp_type == DS_PROFILE_PDP_IPV4V6)
  {
    if (ppp_event != PPP_DOWN_EV)
    {
      DS_MSG3(MSG_LEGACY_HIGH,"LCP Up/Resync event received during dual IP call, ignoring",0,0,0);
      return;
    }
    inst.sm_instance =  DS_RMSM_ANY_SM_INSTANCE;
  }

  else
  {
    DS_MSG3(MSG_LEGACY_ERROR,"Unrecognized protocol: %d, pdp type: %d",
                        protocol, pdp_type, 0 );
    return;
  }

  sm_info_ptr = ds_rmsmi_get_sm_info_ptr(&inst);
  if (sm_info_ptr == NULL && inst.sm_instance != DS_RMSM_ANY_SM_INSTANCE)
  {
    DATA_ERR_FATAL("SM Info ptr is NULL");
    return;
  }
  
  switch(ppp_event)
  {
#ifdef FEATURE_DATA_RMSM
    case PPP_UP_EV:      
      ds_rmsm_rm_enable_flow(sm_info_ptr);
      ds_3gpp_rmsm_ip_common_post_event( &inst,  DS_CMD_3GPP_RMSM_RM_PPP_UP );
      break;

    case PPP_DOWN_EV:
      ds_3gpp_rmsm_ip_common_post_event( &inst, DS_CMD_3GPP_RMSM_RM_PPP_DOWN );
      break;
#endif 
    case PPP_RESYNC_EV:
      //ds_rmsm_rm_disable_flow(sm_info_ptr);
      break;

    default:
      DS_MSG1(MSG_LEGACY_ERROR, "Unsupported PPP event %d", ppp_event);
      //ASSERT(0);
  }
  return;
} /* ds_rmsmi_ppprm_event_handler_cb */

/*===========================================================================
  FUNCTION DS_RMSMI_PROCESS_ESC_SEQ_CB

  DESCRIPTION
    This function is called when DTR goes low and AT&D = 1.  This should do
    the same action as the dtr_sig_cb.
  PARAMETERS
    None
  RETURN VALUE
    Always return DSAT_ONLINE_DATA.
  DEPENDENCIES
    None
  SIDE EFFECTS
    None
 ===========================================================================*/
dsat_mode_enum_type ds_rmsmi_process_esc_seq_cb(void* cb_data)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_rmsmi_process_dtr_sig_cb(FALSE, cb_data);
  return(DSAT_ONLINE_DATA);
} /* ds_rmsmi_process_esc_seq_cb() */

/*===========================================================================
FUNCTION   DS_RMSM_GET_INSTANCE_NUM_FROM_SIO_INST()

DESCRIPTION
  Returns the instance of the UMTS_RMSM corresponding to the supplied 
  sio_inst_ptr.

NOTE: THIS IS A RESTRICTED FUNCTION AND IS INTENDED TO BE USED BY 
DS_RMSM MODULES ONLY

PARAMETERS
  sio_inst_ptr: The sio_inst_ptr which the caller wants to map to RMSM
  instance

RETURN VALUE
  DS_RMSM_INVALID_SM_INSTANCE: In case of error
  SM instance: The SM instance corr. to the supplied sio_inst_ptr, otherwise


DEPENDENCIES
  ds_rmsm_init() should have been called first.

SIDE EFFECTS
  None
===========================================================================*/
ds_rmsm_sm_instance_type  ds_rmsmi_get_instance_num_from_sio_inst
(
  ds_rmsm_sio_instance *sio_inst_ptr
)
{
  ds_rmsm_sm_instance_type i = DS_RMSM_V4_SM_INSTANCE;
  ds_rmsm_sm_instance_type instance = DS_RMSM_INVALID_SM_INSTANCE;
  ds_rmsm_rm_dev_instance_type rm_dev_instance = 
    DS_RMSM_RM_DEV_INSTANCE_0;
  ds_rmsmi_info_type* rmsmi_info_ptr = 
    ds_rmsmi_get_rmsmi_info_ptr(rm_dev_instance);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for (i = DS_RMSM_V4_SM_INSTANCE; i<DS_RMSM_MAX_SM_INSTANCES; i++)
  {
    if (rmsmi_info_ptr->sm_info_ptr[i]->sio_inst == sio_inst_ptr)
    {
      instance = i;
      break;
    }
  }

  return instance;
} /* ds_rmsmi_get_instance_num_from_sio_inst */

/*===========================================================================
FUNCTION   DS_RMSMI_GET_RMSMI_INFO_PTR

DESCRIPTION
  Returns pointer to the Rm Control Block.
  
NOTE: THIS IS A RESTRICTED FUNCTION AND IS INTENDED TO BE USED BY 
DS_RMSM MODULES ONLY

PARAMETERS
  rm_dev_instance - Rm Device Instance

RETURN VALUE
  Pointer to the RMSMI info structure,
  NULL if invalid Rm Dev instance

DEPENDENCIES
  ds_rmsm_init() should have been called first.

SIDE EFFECTS
  None
===========================================================================*/
ds_rmsmi_info_type*  ds_rmsmi_get_rmsmi_info_ptr
(
  ds_rmsm_rm_dev_instance_type rm_dev_instance
)
{
  if (rm_dev_instance <= DS_RMSM_INVALID_RM_DEV_INSTANCE ||
         rm_dev_instance >= DS_RMSM_MAX_RM_DEV_INSTANCES)
  {
    DS_MSG1(MSG_LEGACY_ERROR,"Invalid Rm Dev instance %d",rm_dev_instance);
    return NULL;
  }

  return ds_rmsmi_info[rm_dev_instance];
} /* ds_rmsmi_get_info_ptr_from_instance */


/*===========================================================================
FUNCTION   DS_RMSMI_GET_SM_INFO_PTR

DESCRIPTION
  Returns pointer to the SM specific Control Block.
  
NOTE: THIS IS A RESTRICTED FUNCTION AND IS INTENDED TO BE USED BY 
DS_RMSM MODULES ONLY

PARAMETERS 
  inst_ptr - Instance of the Rm Device and the State Machine Instance

RETURN VALUE
  Pointer to the SM info structure.
  NULL if invalid SM or Rm Dev instance

DEPENDENCIES
  ds_rmsm_init() should have been called first.

SIDE EFFECTS
  None
===========================================================================*/
ds_rmsmi_sm_info_type*  ds_rmsmi_get_sm_info_ptr
(
  ds_rmsm_instance_type* inst_ptr
)
{
  ds_rmsmi_info_type *rmsmi_info_ptr;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (inst_ptr == NULL)
  {
    DS_MSG0(MSG_LEGACY_ERROR,"Instance ptr is NULL");
    return NULL;
  }

  if (inst_ptr->sm_instance <= DS_RMSM_INVALID_SM_INSTANCE ||
         inst_ptr->sm_instance >= DS_RMSM_MAX_SM_INSTANCES)
  {
    DS_MSG1(MSG_LEGACY_ERROR,"Invalid sm instance %d",inst_ptr->sm_instance);
    return NULL;
  }

  rmsmi_info_ptr = ds_rmsmi_get_rmsmi_info_ptr(inst_ptr->rm_dev_instance);

  if (rmsmi_info_ptr == NULL)
  {
    DS_MSG0(MSG_LEGACY_ERROR,"RmSm Info ptr is NULL");
    return NULL;
  }

  return rmsmi_info_ptr->sm_info_ptr[inst_ptr->sm_instance];
}

/*===========================================================================
FUNCTION   DS_RMSMI_SIOLIB_DEREGISTER_CALLBACKS()

DESCRIPTION
  De reregisters the callbacks registered with SIO lib
  
NOTE: THIS IS A RESTRICTED FUNCTION AND IS INTENDED TO BE USED BY 
DS_RMSM MODULES ONLY

PARAMETERS
  Port Id

RETURN VALUE
  none
  
DEPENDENCIES
  ds_rmsm_init() should have been called first.

SIDE EFFECTS
  None
===========================================================================*/
void  ds_rmsmi_siolib_deregister_callbacks(ds3g_siolib_port_e_type port_id)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  (void)ds3g_siolib_ex_deregister_callback_func( &ds_rmsmi_sio_func_table, 
                                                  port_id );
}


/*===========================================================================
FUNCTION DS_RMSMI_SET_SIO_PORT_ID

DESCRIPTION
  This function is used to set the port id corresponding to an Rm Device.

DEPENDENCIES
   None
  
PARAMETERS
  Rm Dev instance
  Port Id
  
RETURN VALUE
   None
    
SIDE EFFECTS
   None

===========================================================================*/
void ds_rmsmi_set_sio_port_id
(
  ds_rmsm_rm_dev_instance_type rm_dev,
  ds3g_siolib_port_e_type          port_id
)
{
  ds_rmsmi_info_type *rmsmi_info_ptr = 
    ds_rmsmi_get_rmsmi_info_ptr(rm_dev);

  if (rmsmi_info_ptr == NULL)
  {
    return;
  }

  if (port_id < DS3G_SIOLIB_PORT_NONE ||
      port_id > DS3G_SIOLIB_PORT_MAX)
  {
    return;
  }

  rmsmi_info_ptr->port_id = port_id;

  DS_MSG1(MSG_LEGACY_HIGH,"Port Id is %d",port_id);
}

/*===========================================================================
FUNCTION DS_RMSMI_GET_SIO_PORT_ID

DESCRIPTION
  This function is used to get the port id corresponding to an Rm Device.

DEPENDENCIES
   None
  
PARAMETERS
  Rm Dev Instance

RETURN VALUE
   Port Id
    
SIDE EFFECTS
   None

===========================================================================*/
ds3g_siolib_port_e_type 
ds_rmsmi_get_sio_port_id(ds_rmsm_rm_dev_instance_type rm_dev)
{
  ds_rmsmi_info_type *rmsmi_info_ptr = 
            ds_rmsmi_get_rmsmi_info_ptr(rm_dev);

  if (rmsmi_info_ptr == NULL)
  {
    return DS3G_SIOLIB_PORT_NONE;
  }

  return rmsmi_info_ptr->port_id;
}/* ds_rmsmi_get_sio_portid */

/*===========================================================================
FUNCTION DS_RMSMI_GET_PPP_DEV

DESCRIPTION
  This function is used to map Rm Dev to the PPP Device

DEPENDENCIES
   None
  
PARAMETERS
  Rm Dev Instance

RETURN VALUE
   PPP Device
    
SIDE EFFECTS
   None

===========================================================================*/
ppp_dev_enum_type ds_rmsmi_get_ppp_dev
(
  ds_rmsm_rm_dev_instance_type rm_dev
)
{
  if (rm_dev == DS_RMSM_RM_DEV_INSTANCE_0)
  {
    return PPP_RM_DEV;
  }
  else if (rm_dev == DS_RMSM_RM_DEV_INSTANCE_1)
  {
    return PPP_RM_2_DEV;
  }
  /* else if (rm_dev == DS_RMSM_RM_DEV_INSTANCE_2)
  {
    return PPP_RM_3_DEV;
  }
   else if (rm_dev == DS_RMSM_RM_DEV_INSTANCE_3)
  {
    return PPP_RM_4_DEV;
  }*/
  else
  {
    DS_MSG1(MSG_LEGACY_ERROR,"Invalid Rm Dev %d", rm_dev);
    return PPP_INVALID_DEV;
  }
}

/*===========================================================================
  FUNCTION DS_RMSMI_GET_RM_DEV_FROM_PPP_DEV

  DESCRIPTION
    This function returns the Rm Device instance corresponding to a
    PPP device.
    
  PARAMETERS
    PPP Device

  RETURN VALUE
    Rm Device Instance    

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
ds_rmsm_rm_dev_instance_type ds_rmsmi_get_rm_dev_from_ppp_dev
(
  ppp_dev_enum_type ppp_dev
)
{
  if (ppp_dev == PPP_RM_DEV)
  {
    return DS_RMSM_RM_DEV_INSTANCE_0;
  }
  else if (ppp_dev == PPP_RM_2_DEV)
  {
    return DS_RMSM_RM_DEV_INSTANCE_1;
  }
 /* else if (ppp_dev == PPP_RM_3_DEV)
  {
    return DS_RMSM_RM_DEV_INSTANCE_2;
  }
  else if (ppp_dev == PPP_RM_4_DEV)
  {
    return DS_RMSM_RM_DEV_INSTANCE_3;
  }*/
  else
  {
    ASSERT(0);
    return DS_RMSM_INVALID_RM_DEV_INSTANCE;
  }
}

/*===========================================================================
  FUNCTION DS_RMSMI_GET_RM_DEV_FROM_FW_INST

  DESCRIPTION
    This function returns the Rm Device instance corresponding to a
    passed FWK instance.
    
  PARAMETERS
    FWK instance

  RETURN VALUE
    Rm Device Instance    

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
ds_rmsm_rm_dev_instance_type ds_rmsmi_get_rm_dev_from_fw_inst
(
  ds_fwk_s_type *fw_inst
)
{
  ds_rmsm_rm_dev_instance_type rm_dev = DS_RMSM_INVALID_RM_DEV_INSTANCE;
  ds_rmsmi_info_type           *rmsmi_info_ptr = NULL;

  if(fw_inst == NULL)
  {
    DS_MSG0(MSG_LEGACY_HIGH,"Invalid input passed");
    return rm_dev;
  }

  for (rm_dev = 0; rm_dev < DS_RMSM_MAX_RM_DEV_INSTANCES; rm_dev++ )
  {
    rmsmi_info_ptr = ds_rmsmi_get_rmsmi_info_ptr(rm_dev);
    if (rmsmi_info_ptr && 
      ((rmsmi_info_ptr->sm_info_ptr[DS_RMSM_V4_SM_INSTANCE] && 
      rmsmi_info_ptr->sm_info_ptr[DS_RMSM_V4_SM_INSTANCE]->fw_inst == fw_inst) ||
       (rmsmi_info_ptr->sm_info_ptr[DS_RMSM_V6_SM_INSTANCE] && 
      rmsmi_info_ptr->sm_info_ptr[DS_RMSM_V6_SM_INSTANCE]->fw_inst == fw_inst)))
    {
      DS_MSG1(MSG_LEGACY_HIGH,"Fwk inst matched for rm dev inst: %d",rm_dev);
      break;
    }
  }
  return rm_dev;
}

/*===========================================================================
  FUNCTION DS_RMSMI_GET_PPP_DEV_FROM_CLIENT_HANDLE

  DESCRIPTION: This function gets ppp_dev corresponding to the client handle
    
  PARAMETERS: Client handle from framework

  RETURN VALUE : ppp_dev

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/

ppp_dev_enum_type ds_rmsmi_get_ppp_dev_from_client_handle(int client_handle)
{
  ds_rmsm_instance_type inst_ptr = {0};
  ppp_dev_enum_type  ppp_dev = PPP_INVALID_DEV;
  
  ds_rmsm_get_inst_from_client_handle( client_handle, &inst_ptr);
  ppp_dev = ds_rmsmi_get_ppp_dev(inst_ptr.rm_dev_instance);

  return ppp_dev; 
}

/*===========================================================================
  FUNCTION DS_RMSMI_GET_NEXT_RM_DEV_INSTANCE

  DESCRIPTION
    This function fetches the next Rm Dev Instance to be use for DUN calls.
    
  PARAMETERS
    void

  RETURN VALUE
    Next Rm Dev Instance

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
ds_rmsm_rm_dev_instance_type ds_rmsmi_get_next_rm_dev_instance
(
  void
)
{
  ds_rmsm_rm_dev_instance_type rm_dev = DS_RMSM_INVALID_RM_DEV_INSTANCE;
  ds_rmsmi_info_type           *rmsmi_info_ptr = NULL;

  DS_MSG1(MSG_LEGACY_HIGH, "Call being attempted on port %d", 
                     ds3g_siolib_get_active_port());  
  
  for (rm_dev = 0; rm_dev < DS_RMSM_MAX_RM_DEV_INSTANCES; rm_dev++ )
  {
    rmsmi_info_ptr = ds_rmsmi_get_rmsmi_info_ptr(rm_dev);
    if (rmsmi_info_ptr == NULL)
      break;
  }   
  DS_MSG1(MSG_LEGACY_HIGH, "Rm Dev being returned: %d ", rm_dev); 
  return rm_dev;
}

/*===========================================================================
FUNCTION DS_RMSMI_VALIDATE_RM_DEV_INSTANCE

DESCRIPTION
  This function validates the given Rm Dev instance

DEPENDENCIES
   None
  
PARAMETERS
  Rm Device instance

RETURN VALUE
  TRUE or FALSE depending on whether Rm Device instance is valid
    
SIDE EFFECTS
   None

===========================================================================*/
boolean ds_rmsmi_validate_rm_dev_instance
(
  ds_rmsm_rm_dev_instance_type rm_dev
)
{
  
  if (rm_dev >= DS_RMSM_MAX_RM_DEV_INSTANCES ||
       rm_dev <= DS_RMSM_INVALID_RM_DEV_INSTANCE)
  {
    DS_MSG1(MSG_LEGACY_ERROR, "Invalid Rm Dev %d", rm_dev);
    return FALSE;
  }
  return TRUE;
}

/*===========================================================================
FUNCTION DS_RMSMI_VALIDATE_PPP_DEV

DESCRIPTION
  This function validates the given Rm Dev instance

DEPENDENCIES
   None
  
PARAMETERS
  Rm Device instance

RETURN VALUE
  TRUE or FALSE depending on whether PPP Device is valid
    
SIDE EFFECTS
   None

===========================================================================*/
boolean ds_rmsmi_validate_ppp_dev
(
  ppp_dev_enum_type ppp_dev
)
{
  if (ppp_dev == PPP_INVALID_DEV )
  {
    DS_MSG1(MSG_LEGACY_ERROR, "Invalid PPP Dev %d", ppp_dev);
    return FALSE;
  }
  
  return TRUE;
}

/*===========================================================================
FUNCTION DS_RMSMI_GET_COMPANION_SM_PTR

DESCRIPTION
  This function gets the companion instance corresponding to the input inst

DEPENDENCIES
   None
  
PARAMETERS
  Rm Device instance

RETURN VALUE
  TRUE or FALSE depending on whether PPP Device is valid
    
SIDE EFFECTS
   None

===========================================================================*/
ds_rmsmi_sm_info_type* ds_rmsmi_get_companion_sm_ptr
(
  ds_rmsm_instance_type* inst_ptr
)
{

  ds_rmsmi_info_type         *rmsmi_info_ptr = NULL;
  ds_profile_pdp_type_e       pdp_type;
  ds_rmsm_sm_instance_type   companion_sm_instance = 
    DS_RMSM_INVALID_SM_INSTANCE;
  ds_rmsmi_sm_info_type*     companion_sm_ptr = NULL;
  ds_rmsm_instance_type      companion_inst = {0};
  ds_local_err_val_e_type    err_val = DS_LOCAL_ERR_VAL_NONE;
  /*----------------------------------------------------------------------*/
  
  if (inst_ptr == NULL) 
  {
    err_val = DS_LOCAL_ERR_VAL_0;
    goto func_exit;
  }

  rmsmi_info_ptr = ds_rmsmi_get_rmsmi_info_ptr(inst_ptr->rm_dev_instance);
  if (rmsmi_info_ptr == NULL) 
  {
    err_val = DS_LOCAL_ERR_VAL_1;
    goto func_exit;
  }

  pdp_type = DS_RMSMI_GET_PDP_TYPE(rmsmi_info_ptr);

  if (pdp_type == DS_PROFILE_PDP_IPV4V6)
  {
    if (inst_ptr->sm_instance == DS_RMSM_V4_SM_INSTANCE)
    {
      companion_sm_instance = DS_RMSM_V6_SM_INSTANCE;
    }

    else if (inst_ptr->sm_instance == DS_RMSM_V6_SM_INSTANCE)
    {
      companion_sm_instance = DS_RMSM_V4_SM_INSTANCE;
    }

    companion_inst.sm_instance = companion_sm_instance;
    companion_inst.rm_dev_instance = inst_ptr->rm_dev_instance;

    companion_sm_ptr = ds_rmsmi_get_sm_info_ptr(&companion_inst);   
  }

  return companion_sm_ptr;
  
func_exit:
  if(err_val != DS_LOCAL_ERR_VAL_NONE)
    DS_MSG1(MSG_LEGACY_ERROR, "ds_rmsmi_get_companion_sm_ptr err_val %d",
                                                  err_val);
  return NULL;
}

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */

/*===========================================================================

FUNCTION ds_rmsmi_at_escape_sequence_handler

DESCRIPTION
   From the 3GDSMGR header file....
   
   This handler is invoked when the AT escape sequence is detected, in
   online data mode. The return value indicates whether online command mode
   should be entered (in this case, data transfer over the air should be
   suspended), or if we should stay in online data mode. 
   
   In our case (UCSD subtask), we simply forward this indication on to the
   UCSD protocol stack task.
         
DEPENDENCIES
   None
  
RETURN VALUE
   An indication of whether or not the MT should transition to online
   command mode or not.  In our case we will always return the indication
   that the MT should go to on-line command mode. 
   
    
SIDE EFFECTS
   None

===========================================================================*/
dsat_mode_enum_type 
ds_rmsmi_at_escape_sequence_handler( void )
{
/*-------------------------------------------------------------------------*/
  //TODO:

  /* Tell 3GDSMGR that we want to go to on-line command mode */
 return (DSAT_ONLINE_CMD);
} /* ds_ucsd_at_escape_sequence_handler() */

/*===========================================================================

FUNCTION ds_rmsmi_at_return_to_online_data_handler

DESCRIPTION
   This handler is invoked when the ATO command (return to online data mode)
   is rx'ed in online command mode. It is invoked before the transition to
   return the SIO watermarks and callback function.
         
DEPENDENCIES
   None
  
RETURN VALUE
   None
    
SIDE EFFECTS
   None

===========================================================================*/
LOCAL void 
ds_rmsmi_at_return_to_online_data_handler
(
  /* Pointer to the watermark for data from SIO */
  dsm_watermark_type     **from_sio_wm_ptr,   

  /* Pointer to the watermark for data to SIO. */
  dsm_watermark_type     **to_sio_wm_ptr,
  
  /* Pointer to SIO RX mode-specific handler function */
  sio_rx_func_ptr_type    *mode_hdlr_sio_rx_func_ptr,

  /* Port associated */
  ds3g_siolib_port_e_type  port_id
)
{
  ds_rmsmi_info_type *rmsmi_info_ptr = NULL;  
  ds_rmsm_rm_dev_instance_type rm_dev;
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (port_id <= DS3G_SIOLIB_PORT_NONE ||
      port_id >= DS3G_SIOLIB_PORT_MAX )
  {
    DS_SIO_MSG1_ERROR("Invalid Port Id %d", port_id);
    ASSERT(0);
    return;
  }

  if(NULL == from_sio_wm_ptr || NULL == to_sio_wm_ptr || NULL == mode_hdlr_sio_rx_func_ptr )
  {
    ASSERT(0);
  }

  rm_dev = ds_rmsmi_get_rm_dev_from_port_id(port_id);
  if (ds_rmsmi_validate_rm_dev_instance(rm_dev) == FALSE)
  {
    DS_MSG1(MSG_LEGACY_HIGH,"invalid rm_dev %d", rm_dev);
    return;
  }

  rmsmi_info_ptr = ds_rmsmi_get_rmsmi_info_ptr(rm_dev);

  if(rmsmi_info_ptr != NULL)
  {
    /* Switch statapath to that in call instance */
    *from_sio_wm_ptr            = &rmsmi_info_ptr->sio_rx_wmk;
    *to_sio_wm_ptr              = &rmsmi_info_ptr->sio_tx_wmk;
    mode_hdlr_sio_rx_func_ptr   = NULL;
  }
} /* ds_rmsmi_at_return_to_online_data_handler() */

/*===========================================================================

FUNCTION ds_rmsmi_at_return_to_online_data_complete_handler

DESCRIPTION
   From the 3GDSMGR header file...
   
   This handler is invoked when the ATO command (return to online data mode)
   is rx'ed in online command mode. The mode-specific handler should resume
   data transfer over the air.
   
   In our case, this simply involves sending an event notification down to
   the protocol stack.. 
   
         
DEPENDENCIES
   None
  
RETURN VALUE
   None
    
SIDE EFFECTS
   None

===========================================================================*/
LOCAL void 
ds_rmsmi_at_return_to_online_data_complete_handler ( void )
{
  //TODO:
} /* ds_ucsd_at_return_to_online_data_complete_handler() */


/*===========================================================================

FUNCTION DS_RMSM_ALLOC_CLIENT_HANDLE

DESCRIPTION
         
DEPENDENCIES
   None
  
RETURN VALUE
   None
    
SIDE EFFECTS
   None

===========================================================================*/

int8 ds_rmsm_alloc_client_handle
(
  ds_rmsm_instance_type* inst_ptr
)
{
  int8 client_hdl = DS_INVALID_HANDLE;
  if(inst_ptr == NULL)
  {
    return client_hdl;
  }

  if((inst_ptr->rm_dev_instance < DS_RMSM_RM_DEV_INSTANCE_0 || 
        inst_ptr->rm_dev_instance >= DS_RMSM_MAX_RM_DEV_INSTANCES) ||
     (inst_ptr->sm_instance < DS_RMSM_V4_SM_INSTANCE ||
     inst_ptr->sm_instance >= DS_RMSM_MAX_SM_INSTANCES))
  {
    return client_hdl;
  }

  client_hdl = ((inst_ptr->rm_dev_instance << 1)|(inst_ptr->sm_instance));
  return client_hdl;
  
}


/*===========================================================================

FUNCTION DS_RMSM_GET_INST_FROM_CLIENT_HANDLE

DESCRIPTION
         
DEPENDENCIES
   None
  
RETURN VALUE
   None
    
SIDE EFFECTS
   None

===========================================================================*/

void ds_rmsm_get_inst_from_client_handle
(
  int client_hdl,
  ds_rmsm_instance_type* inst_ptr
)
{
  if((client_hdl == DS_INVALID_HANDLE) || (client_hdl > DS_MAX_HANDLE))
    return;

  if(inst_ptr == NULL)
    return;

  inst_ptr->rm_dev_instance = (client_hdl>>1);
  inst_ptr->sm_instance = (client_hdl & 0x1);

  return;
}

/*===========================================================================

FUNCTION DS_RMSM_BRINGUP_PDN

DESCRIPTION
         
DEPENDENCIES
   None
  
RETURN VALUE
   None
    
SIDE EFFECTS
   None

===========================================================================*/

int ds_rmsm_bringup_pdn(ds_rmsmi_sm_info_type *sm_ptr)
{

  int                                result = -1;
  ds_rmsmi_info_type                 *rmsmi_info_ptr;
  ds_rmsm_instance_type              inst;
  ds_fwk_clnt_handle_type            client_handle = DS_FWK_INVALID_CLNT_HANDLE;
  ds_policy_info_type                policy_info = { 0 };
  ds_fwk_response_info_s_type        resp_info;
  ds_local_err_val_e_type            err_val = DS_LOCAL_ERR_VAL_NONE;

  if(sm_ptr == NULL)
  {
    err_val = DS_LOCAL_ERR_VAL_0;
    goto func_exit;
  }
  
  rmsmi_info_ptr = (ds_rmsmi_info_type*)sm_ptr->vobj_data_ptr;
  if(rmsmi_info_ptr == NULL)
  {
    err_val = DS_LOCAL_ERR_VAL_1;
    goto func_exit;
  }

  policy_info.length = strlen(rmsmi_info_ptr->profile_info.context.apn);
  memscpy(policy_info.name, policy_info.length ,
  	      rmsmi_info_ptr->profile_info.context.apn, policy_info.length);
  
  inst.rm_dev_instance = rmsmi_info_ptr->instance;
  inst.sm_instance = sm_ptr->instance;

  client_handle = sm_ptr->instance_hdl;
  
  policy_info.apn_type = DS_APN_TYPE_UNSPECIFIED;
  policy_info.family = ds_get_addr_family_from_sm_inst(inst.sm_instance);
  if(rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr != NULL){
    
    policy_info.user_info_ptr = (void *)&(rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->ppp_info);
    DS_MSG2(MSG_LEGACY_HIGH,"Auth valid fields passed in user_info_ptr:0x%x  auth_type: %d",
              rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->ppp_info.valid_fields,
              rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->ppp_info.auth_info.auth_type);
  }

  /*Check to see if PDN UP for particular APN */  
  if(ds_fwk_look_up_pdn_by_policy(DS_FWK_CLIENT_RMSM, policy_info)!=
  	                                              DS_FWK_INVALID_CLNT_HANDLE)
  {
    DS_MSG0(MSG_LEGACY_HIGH,"RMSM : Client Handle found for instance during lookup");
    return 1;
  }
  else
  {
    /*Bring up the data call*/
    result = ds_fwk_bringup_pdn_by_policy(DS_FWK_CLIENT_RMSM,
                                        policy_info,
                                        client_handle,
                                        &resp_info);
    if(result == -1)
    {
      err_val = DS_LOCAL_ERR_VAL_3;
      goto func_exit;
    }		
  }

func_exit:
  if(err_val != DS_LOCAL_ERR_VAL_NONE)
    DS_MSG1(MSG_LEGACY_ERROR,"ds_rmsm_bringup_pdn err_val:%d",err_val);
  return result;
}


/*===========================================================================

FUNCTION DS_BRINGDOWN_PDN

DESCRIPTION   
         
DEPENDENCIES
   None
  
RETURN VALUE
   None
    
SIDE EFFECTS
   None

===========================================================================*/
int ds_rmsm_bringdown_pdn(ds_rmsmi_sm_info_type *sm_ptr)
{
  ds_fwk_response_info_s_type        resp_info;
  ds_fwk_clnt_handle_type  client_handle = DS_FWK_INVALID_CLNT_HANDLE;
  int ret_val;

  client_handle = sm_ptr->instance_hdl;

  ret_val = ds_fwk_tear_down_pdn(DS_FWK_CLIENT_RMSM,client_handle, &resp_info);

  return ret_val;
  
}

/*===========================================================================

FUNCTION DS_RMSM_PDN_EVENT_CBACK

DESCRIPTION   
         
DEPENDENCIES
   None
  
RETURN VALUE
   None
    
SIDE EFFECTS
   None

===========================================================================*/
void ds_rmsm_pdn_event_cback
(
  ds_fwk_clnt_handle_type    client_handle,
  ds_pdn_event_enum_type     event,
  ds_pdn_event_info_u_type   event_info,
  void                      *user_data_ptr
)
{

  ds_rmsm_instance_type inst;
  ds_rmsmi_sm_info_type *sm_info_ptr = NULL;

  if (client_handle == DS_INVALID_HANDLE)
  {
    DS_ERROR_LOG_0(" ds_rmsm_pdn_event_cback : Invalid handle");
                   
    return;
  }
  memset(&inst,0,sizeof(ds_rmsm_instance_type));

  /* Populate the rmsm instance from client handle */
  ds_rmsm_get_inst_from_client_handle(client_handle, &inst);
  
  sm_info_ptr = ds_rmsmi_get_sm_info_ptr(&inst);
  if(sm_info_ptr == NULL)
  {
    DS_MSG0(MSG_LEGACY_ERROR,"SM Info ptr is NULL");
    return;
  }
  
  switch(event)
  {
#ifdef FEATURE_DATA_RMSM
    case PDN_UP_EV:
      ds_3gpp_rmsm_ip_common_post_event( &inst, DS_CMD_3GPP_RMSM_UM_PDN_UP );  
      break;

    case PDN_DOWN_EV:
      ds_3gpp_rmsm_ip_common_post_event( &inst, DS_CMD_3GPP_RMSM_UM_PDN_DOWN );  
      break; 
#endif
    case BEARER_FLOW_ENABLED_EV:
	    ds_rmsm_rm_enable_flow(sm_info_ptr);
      break;

    case BEARER_FLOW_DISABLED_EV:
      ds_rmsm_rm_disable_flow(sm_info_ptr);
      break;
    
    case PDN_WWAN_TECH_SUSPENSION_EV:
      ds_rmsm_process_tech_suspension(&inst,event_info.tech_suspension_status);
      break;
    
    default:
      break;
  }

  return;
}

/*===========================================================================

FUNCTION DS_GET_ADDR_FAMILY_FROM_SM_INST

DESCRIPTION   
         
DEPENDENCIES
   None
  
RETURN VALUE
   None
    
SIDE EFFECTS
   None

===========================================================================*/

ip_addr_enum_type ds_get_addr_family_from_sm_inst(ds_rmsm_sm_instance_type sm_instance)
{

  ip_addr_enum_type addr_family = IP_ADDR_INVALID;
  
  if(sm_instance <= DS_RMSM_INVALID_SM_INSTANCE || sm_instance > DS_RMSM_MAX_SM_INSTANCES)
    return addr_family;

  switch (sm_instance)
  {
    case DS_RMSM_V4_SM_INSTANCE:
	  addr_family = IPV4_ADDR;
	  break;

	case DS_RMSM_V6_SM_INSTANCE:
	  addr_family = IPV6_ADDR;
	  break;

	default:
	  break;
  }
  
  return addr_family;
}

/*===========================================================================

FUNCTION DS_RMSM_PROCESS_TECH_SUSPENSION

DESCRIPTION   
         
DEPENDENCIES
   None
  
RETURN VALUE
   None
    
SIDE EFFECTS
   None

===========================================================================*/
void ds_rmsm_process_tech_suspension
(
  ds_rmsm_instance_type* inst_ptr,
  boolean tech_suspension_status
)
{
  ds_rmsmi_info_type* rmsmi_info_ptr = NULL;
  ds_local_err_val_e_type err_val = DS_LOCAL_ERR_VAL_NONE;
  
  if(!inst_ptr)
  {
    err_val = DS_LOCAL_ERR_VAL_0;
    goto func_exit;
  }

  rmsmi_info_ptr = ds_rmsmi_get_rmsmi_info_ptr(inst_ptr->rm_dev_instance);
  if (rmsmi_info_ptr == NULL)
  {
    err_val = DS_LOCAL_ERR_VAL_1;
    goto func_exit;
  }
  
  if(tech_suspension_status)
  {
    ds3g_siolib_ex_set_inbound_flow( DS_FLOW_TECH_SUSP_MASK, DS_FLOW_DISABLE,
                                     rmsmi_info_ptr->port_id );
  }
  else
  {
    ds3g_siolib_ex_set_inbound_flow( DS_FLOW_TECH_SUSP_MASK, DS_FLOW_ENABLE,
                                     rmsmi_info_ptr->port_id );
  }
  
func_exit:
  if(err_val != DS_LOCAL_ERR_VAL_NONE)
    DS_MSG1(MSG_LEGACY_ERROR,"ds_rmsm_process_tech_suspension : err_val %d",err_val);
  
  return;								   
}

/*===========================================================================

FUNCTION DS_RMSM_RM_ENABLE_FLOW

DESCRIPTION   
         
DEPENDENCIES
   None
  
RETURN VALUE
   None
    
SIDE EFFECTS
   None

===========================================================================*/

void ds_rmsm_rm_enable_flow(ds_rmsmi_sm_info_type *sm_info_ptr)
{
  ds_rmsmi_info_type* rmsmi_info_ptr = NULL;
  
  rmsmi_info_ptr = (ds_rmsmi_info_type*)sm_info_ptr->vobj_data_ptr;
  sm_info_ptr->flow_enabled = 1;
  
  SET_RX_DATA_CAN_BE_PULLED(rmsmi_info_ptr);

  /*---------------------------------------------------------------------
   	Send signal to PS to resume pulling PPP packets from SIO rx_wm, 
   	laptop flow will be enabled by rx_wm lo water function.
  ---------------------------------------------------------------------*/
  ps_set_signal(rmsmi_info_ptr->ps_ppp_rm_rx_signal );
  
  return;
}

/*===========================================================================

FUNCTION DS_RMSM_RM_DISABLE_FLOW

DESCRIPTION   
         
DEPENDENCIES
   None
  
RETURN VALUE
   None
    
SIDE EFFECTS
   None

===========================================================================*/
void ds_rmsm_rm_disable_flow(ds_rmsmi_sm_info_type *sm_info_ptr)
{
  ds_rmsmi_info_type* rmsmi_info_ptr = NULL;
  
  rmsmi_info_ptr = (ds_rmsmi_info_type*)sm_info_ptr->vobj_data_ptr;
  sm_info_ptr->flow_enabled = 0;
  RESET_RX_DATA_CAN_BE_PULLED(rmsmi_info_ptr);
  return;
}

/*===========================================================================

FUNCTION DS_RMSM_TX_CB

DESCRIPTION   
         
DEPENDENCIES
   None
  
RETURN VALUE
   None
    
SIDE EFFECTS
   None

===========================================================================*/

static void ds_rmsm_tx_cb
(
  ds_fwk_clnt_handle_type    client_handle,
  dsm_item_type              *dsm_item,
  void                       *user_data_ptr
)
{
#ifdef FEATURE_DATA_RMSM	
  ppp_dev_enum_type        ppp_dev = PPP_MIN_DEV;
  ps_tx_meta_info_type     *tx_meta_info = NULL;
  ds_rmsm_instance_type    rmsm_inst = {0};
  
  ds_rmsm_get_inst_from_client_handle(client_handle, &rmsm_inst);
  ppp_dev = ds_rmsmi_get_ppp_dev_from_client_handle(client_handle);

  PS_EXT_TX_META_INFO_GET_ALL(tx_meta_info);
  if((tx_meta_info == NULL) || (tx_meta_info->rt_meta_info_ptr == NULL))
  {
    DS_MSG0(MSG_LEGACY_ERROR,"Unable to allocate TX meta info");
    return;
  }
  
  if(rmsm_inst.sm_instance == DS_RMSM_V4_SM_INSTANCE)
  {
    tx_meta_info->rt_meta_info_ptr->pkt_info.ip_vsn = IP_V4;
  }
  else if(rmsm_inst.sm_instance == DS_RMSM_V6_SM_INSTANCE)
  {
    tx_meta_info->rt_meta_info_ptr->pkt_info.ip_vsn = IP_V6;
  }    
  ppp_output(ppp_dev, &dsm_item,0,tx_meta_info);
#endif 
  return;
}

#ifndef FEATURE_DATA_RMSM
/*===========================================================================
  FUNCTION DS_3GPP_RMSM_IP_COMMON_PROCESS_CMD()

  DESCRIPTION
    This function receives a cmd posted to DS_3GPP_RMSM (for pdp-ip calls)
    in DS task and calls an appropriate function to handle it

  PARAMETERS

  RETURN VALUE
    none

  DEPENDENCIES
    none  
    
  SIDE EFFECTS
    None
===========================================================================*/
/*lint -e818*/
void ds_3gpp_rmsm_ip_common_process_cmd
(
  ds_command_type *ds_cmd_ptr
)
{
  ds_rmsm_instance_type          *inst_ptr = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( (ds_cmd_ptr == NULL) || (ds_cmd_ptr->cmd_payload_ptr == NULL) )
  {
    DATA_ERR_FATAL("NULL cmd_ptr or null payload ptr");
    return;
  }

  inst_ptr = (ds_rmsm_instance_type*)ds_cmd_ptr->cmd_payload_ptr;
    
  /*-------------------------------------------------------------------------
    Take action based on event received and which state we are currently
    in.
  -------------------------------------------------------------------------*/
  switch(ds_cmd_ptr->hdr.cmd_id)
  {
    case DS_CMD_3GPP_RMSM_AT_PDN_EV:
    case DS_CMD_3GPP_RMSM_AT_SEND_DATA:
      ds_3gpp_rmsm_at_process_cmd(ds_cmd_ptr);
      break;
    
    case DS_CMD_SIOLIB_RDM_OPEN:
    case DS_CMD_SIOLIB_RDM_CLOSE:
    case DS_CMD_SIOLIB_CLOSE_COMPLETE:
    case DS_CMD_SIOLIB_DTR_CHANGED:
#ifdef FEATURE_DSAT_MDM_FUSION
    case DS_CMD_SIOLIB_CD_CHANGED:
#endif
    case DS_CMD_SIOLIB_DSM_MEM_CTRL:
    case DS_CMD_SIOLIB_ENTER_ONLINE_CMD_TX_FLUSH:
    case DS_CMD_SIOLIB_COMPLETE_ONLINE_CMD_SWITCH:  /* Rx'd after a TX flush completes */
    case DS_CMD_SIOLIB_ENTER_ONLINE_DATA_TX_FLUSH:
    case DS_CMD_SIOLIB_ENTER_ONLINE_DATA_NO_TX_FLUSH:/* Enter online-cmd w/no TX flush */
    case DS_CMD_SIOLIB_COMPLETE_ONLINE_DATA_SWITCH:  /* Rx'd after a TX flush completes */
    case DS_CMD_SIOLIB_TIMER_EXPIRED:
      /*Dispatch siolib cmd to be handled separately*/
      ds3g_siolib_process_cmds(ds_cmd_ptr);
      break;
    
    default:
      DS_MSG3(MSG_LEGACY_ERROR,"Invalid UMTS RMSM sm instance: %d, "
                         "rm dev instance: %d, cmd %d, ignoring", 
                          inst_ptr->sm_instance, 
                          inst_ptr->rm_dev_instance, ds_cmd_ptr->hdr.cmd_id);
  } /* switch( cmd ) */
  return;
}
#endif
