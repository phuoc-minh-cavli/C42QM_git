/*****************************************************************************/
/*
    D Y N A M I C    P O R T    S W I T C H 

GENERAL DESCRIPTION
  This file contains types and declarations associated with the
  Dynamic Port Switch (DPS).

Copyright (c) 2016 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Qualcomm Confidential and Proprietary
*/
/*****************************************************************************/

#include "dps.h"
#include "comdef.h"
#include "err.h"
#include "dsm.h"
#include "diagdsm_v.h"           /* For DSM_DIAG_SIO_RX_ITEM_POOL, etc */
#include "sio.h"
#include "rdevmap.h"
#include "assert.h"
#include "rex.h"
#include "mhi.h"
#include "timer.h"

#define DPS_ULOG(s)                 ULOG_RT_PRINTF_0(dps_ulog_vars.dps_log,s);
#define DPS_ULOG_1(s,a)             ULOG_RT_PRINTF_1(dps_ulog_vars.dps_log,s,a);
#define DPS_ULOG_2(s,a,b)           ULOG_RT_PRINTF_2(dps_ulog_vars.dps_log,s,a,b);
#define DPS_ULOG_3(s,a,b,c)         ULOG_RT_PRINTF_3(dps_ulog_vars.dps_log,s,a,b,c);
#define DPS_ERR_FATAL(a,b,c,d)      ERR_FATAL(a,b,c,d)

 //2k ulog buffer allocated
#define DPS_MAX_ULOG_SIZE           2048

// packets threshold required to trigger switch
#define DPS_TRIGGER_MAX_PACKETS     5
#define DIAG_MAGIC_PACKET_LEN       4
#define DUN_MAGIC_PACKET_LEN        2

//diag magic packet sequence 7B 2C 3D 7E - hdlc encoded
static const uint8 diag_magic_packet[4] = {0x7B,0x2C,0x3D,0x7E};
//any AT command is dun magic packet : sequence 41 54 - AT
static const uint8 dun_magic_packet[2] = {0x41,0x54};

//wait for rdm status check - 10ms
#define DPS_RDM_READY_TIMEOUT 10
#define ASSIGN_SVC_STATUS(s,a,b,c) s.status=a;s.service=b;s.device=c;

// DPS states
typedef enum
{
  DPS_STATE_READY,
  DPS_STATE_BUSY,
} dps_state_type;

// DPS available for services - diag and dun
typedef enum
{
  DPS_SVC_FIRST = 0,
  DPS_DIAG_SVC = DPS_SVC_FIRST,
  DPS_DUN_SVC,
  DPS_SVC_LAST,
} dps_svc_type;

// switch counter-types for each service DIAG/DUN
typedef enum
{
  SVC_FIRST = 0,
  SVC_MAIN = SVC_FIRST,
  SVC_ALT,
  SVC_LAST,
} switch_svc_type;

// save service switch state
typedef struct switch_status_s {
  rdm_assign_status_type status;
  rdm_service_enum_type  service;
  rdm_device_enum_type   device;
} switch_status_t;

// main structure type to carry debug info
typedef struct dps_info_s {
  dps_state_type        status;                 // DPS current state
  sio_stream_id_type    alt_svc_sio_handle;     // SVC_ALT SIO handle
  sio_port_id_type      alt_svc_port;           // SVC_ALT port in use
  sio_port_id_type      svc_port;               // SVC port in use
  dsm_watermark_type    dps_tx_wm_queue;        // TX DSM queue req for MHI
  q_type                dps_tx_q;               // TX queue structure
  uint32                packet_count;           // packet counter 
  boolean               magic_pkt_rcvd;         // track magic pkt received
  switch_status_t       svc_status[SVC_LAST];   // switch resp
} dps_info_t;

// DPS bookkeeping
dps_info_t dps_info[DPS_SVC_LAST];

// DPS ulog handle
dps_ulog_vars_type dps_ulog_vars;
// TCB pointer
static rex_tcb_type* dps_tcb = NULL;

//timer used for re-initialization
static rex_timer_type dps_timer = {0};
static timer_group_type dps_timer_group = {0};

//======================================================================================================
// forward declarations
//======================================================================================================
void dps_fallback_switch_cb (rdm_assign_status_type status,
                             rdm_service_enum_type  service,
                             rdm_device_enum_type   device);

void dps_svc_switch_cb (rdm_assign_status_type status,
                         rdm_service_enum_type  service,
                         rdm_device_enum_type   device);

void dps_alt_svc_switch_cb (rdm_assign_status_type status,
                             rdm_service_enum_type  service,
                             rdm_device_enum_type   device);

static void dps_rdm_ready_cb (boolean ready);
//======================================================================================================
// functions definitions
//======================================================================================================

dps_svc_type dps_map_rdm_to_dps_svc (rdm_service_enum_type rdm_svc)
{
  return (RDM_ALT_DIAG_SRVC == rdm_svc || RDM_DIAG_SRVC == rdm_svc) ? DPS_DIAG_SVC : DPS_DUN_SVC;
}

rdm_service_enum_type dps_map_dps_to_rdm_alt_svc (dps_svc_type svc)
{
  return (DPS_DIAG_SVC == svc) ? RDM_ALT_DIAG_SRVC : RDM_ALT_DUN_SRVC;
}

rdm_service_enum_type dps_map_dps_to_rdm_svc (dps_svc_type svc)
{
  return (DPS_DIAG_SVC == svc) ? RDM_DIAG_SRVC : RDM_DUN_SRVC;
}

rdm_device_enum_type dps_swap_rdm_device (dps_svc_type svc, rdm_device_enum_type device)
{
  if (DPS_DIAG_SVC == svc)
    return (RDM_USB_DIAG_DEV == device) ? RDM_MHI_DIAG_DEV : RDM_USB_DIAG_DEV;
  else
    return (RDM_USB_MDM_DEV == device) ? RDM_MHI_DUN_DEV : RDM_USB_MDM_DEV;
}

sio_port_id_type dps_swap_sio_port (dps_svc_type svc, sio_port_id_type sio_port)
{
  if (DPS_DIAG_SVC == svc)
    return (SIO_PORT_MHI_DIAG == sio_port) ? SIO_PORT_USB_DIAG : SIO_PORT_MHI_DIAG;
  else
    return (SIO_PORT_MHI_DUN == sio_port) ? SIO_PORT_USB_MDM : SIO_PORT_MHI_DUN;
}

rdm_device_enum_type dps_swap_to_rdm_device (dps_svc_type svc, sio_port_id_type sio_port)
{
  if (DPS_DIAG_SVC == svc)
    return (SIO_PORT_MHI_DIAG == sio_port) ? RDM_USB_DIAG_DEV : RDM_MHI_DIAG_DEV;
  else
    return (SIO_PORT_MHI_DUN == sio_port) ? RDM_USB_MDM_DEV : RDM_MHI_DUN_DEV;
}

boolean dps_check_if_valid_port (dps_svc_type svc, sio_port_id_type sio_port)
{
  if (DPS_DIAG_SVC == svc)
    return (SIO_PORT_MHI_DIAG == sio_port || SIO_PORT_USB_DIAG == sio_port) ? TRUE : FALSE;
  else
    return (SIO_PORT_MHI_DUN == sio_port || SIO_PORT_USB_MDM == sio_port) ? TRUE : FALSE;
}

void dps_set_rex_sigs (dps_svc_type svc, switch_svc_type switch_svc, boolean result)
{
  if (DPS_DIAG_SVC == svc)
  {
    switch (switch_svc)
    {
      case SVC_ALT:
      if (result)
        (void) rex_set_sigs (dps_tcb, DPS_ALT_DIAG_SWITCH_SIG);
      else
        (void) rex_set_sigs (dps_tcb, DPS_ALT_DIAG_SWITCH_FAIL_SIG);
      break;
      case SVC_MAIN:
        if (result)
          (void) rex_set_sigs (dps_tcb, DPS_DIAG_SWITCH_SIG);
        else
          (void) rex_set_sigs (dps_tcb, DPS_DIAG_SWITCH_FAIL_SIG);
      break;
    }
  }
  else
  {  //(DPS_DUN_SVC == service)
    switch (switch_svc)
    {
      case SVC_ALT:
        if (result)
          (void) rex_set_sigs (dps_tcb, DPS_ALT_DUN_SWITCH_SIG);
        else
          (void) rex_set_sigs (dps_tcb, DPS_ALT_DUN_SWITCH_FAIL_SIG);
      break;
      case SVC_MAIN:
        if (result)
          (void) rex_set_sigs (dps_tcb, DPS_DUN_SWITCH_SIG);
        else
          (void) rex_set_sigs (dps_tcb, DPS_DUN_SWITCH_FAIL_SIG);
      break;
    }
  }
}

//---------------------------------------------------------------------------------------------------
// ulogs init
//---------------------------------------------------------------------------------------------------
void dps_ulog_init(void)
{
  dps_ulog_vars.log_size = DPS_MAX_ULOG_SIZE;
  ULogFront_RealTimeInit (&(dps_ulog_vars.dps_log),
                          "DPS",
                          dps_ulog_vars.log_size,
                          ULOG_MEMORY_LOCAL,
                          ULOG_LOCK_OS);
  ULogCore_HeaderSet (dps_ulog_vars.dps_log, "Content-Type: text/tagged-log-1.0;");
}

//---------------------------------------------------------------------------------------------------
// fallback switch completed
// update status
//---------------------------------------------------------------------------------------------------
void dps_fallback_switch_cb
(
  rdm_assign_status_type status,
  rdm_service_enum_type  service,
  rdm_device_enum_type   device
)
{
  dps_svc_type svc = dps_map_rdm_to_dps_svc (service);
  // set DPS to ready state again
  dps_info[svc].packet_count = 0;
  dps_info[svc].status = DPS_STATE_READY;

  //don't make it recursive
  if (service == dps_info[svc].svc_status[SVC_ALT].service)
  {
    ASSIGN_SVC_STATUS (dps_info[svc].svc_status[SVC_ALT], status, service, device);
  }
  else
  {
    ASSIGN_SVC_STATUS (dps_info[svc].svc_status[SVC_MAIN], status, service, device);
  }
}

//---------------------------------------------------------------------------------------------------
// fallback if switch fails for any service
// swap port and re-assign service to old port
//---------------------------------------------------------------------------------------------------
void dps_fallback_switch_handle (dps_svc_type svc)
{
  int i = (SVC_LAST-1);
  for (;i >= SVC_FIRST; i--)
  {
    if (RDM_DONE_S != dps_info[svc].svc_status[i].status)
    {
      DPS_ULOG_3 ("DPS: SWITCH FAILURE, in Fallback service: %u, device: %u, status: %u", 
                                                    dps_info[svc].svc_status[i].service,
                                                    dps_info[svc].svc_status[i].device,
                                                    dps_info[svc].svc_status[i].status);
      rdm_assign_port (dps_info[svc].svc_status[i].service,
                       dps_swap_rdm_device (svc, dps_info[svc].svc_status[i].device),
                       dps_fallback_switch_cb);
    }
  }
}

//---------------------------------------------------------------------------------------------------
// switch completed
//---------------------------------------------------------------------------------------------------
void dps_svc_switch_handle (dps_svc_type svc)
{
  DPS_ULOG_2 ("DPS: SVC SWITCH completed, service: %d, status: %u", svc,
                           (dps_info[svc].svc_status[SVC_MAIN].status));

  dps_info[svc].packet_count = 0;
  dps_info[svc].status       = DPS_STATE_READY;
  dps_info[svc].svc_port     = dps_swap_sio_port(svc, dps_info[svc].alt_svc_port);

  DPS_ULOG_1 ("DPS: SWITCH SUCCESSFUL, service: %d", svc);
}

//---------------------------------------------------------------------------------------------------
// check switch for SVC_MAIN and 
// if failure, fallback to previous port for both SVC_MAIN / SVC_ALT
//---------------------------------------------------------------------------------------------------
void dps_svc_switch_cb
(
  rdm_assign_status_type status,
  rdm_service_enum_type  service,
  rdm_device_enum_type   device
)
{
  dps_svc_type svc = dps_map_rdm_to_dps_svc (service);
  //save the svc status for fallback if required
  ASSIGN_SVC_STATUS (dps_info[svc].svc_status[SVC_MAIN], status, service, device);
  if (RDM_DONE_S == status)
  {
    //diag switch is successful
    dps_set_rex_sigs (svc, SVC_MAIN, TRUE);
  }
  else
  {
    //if diag service fails to switch, push fallback on ALT DIAG as well
    dps_info[svc].svc_status[SVC_ALT].status = status;

    //fallback to last known good state
    dps_set_rex_sigs (svc, SVC_MAIN, FALSE);
  }
}

//---------------------------------------------------------------------------------------------------
// trigger switch diag svc to non-active interface on RDM
//---------------------------------------------------------------------------------------------------
void dps_alt_svc_switch_handle (dps_svc_type svc)
{
  DPS_ULOG_2 ("DPS: SVC_ALT switch status=%u, service=%d swap SVC_MAIN --> ",
                             dps_info[svc].svc_status[SVC_ALT].status, svc);
  //assign diag to non active interface
  if (TRUE != dps_check_if_valid_port (svc, dps_info[svc].svc_port))
  {
    DPS_ULOG_2 ("DPS: no valid active interface for service: %d, port: %u",
                                             svc, dps_info[svc].svc_port);
  }
  else
  {
    rdm_assign_port (dps_map_dps_to_rdm_svc (svc),
                     dps_swap_to_rdm_device (svc, dps_info[svc].svc_port),
                     dps_svc_switch_cb);
  }
}

//---------------------------------------------------------------------------------------------------
// check switch for alt_svc and 
// if failure, fallback to previous port
//---------------------------------------------------------------------------------------------------
void dps_alt_svc_switch_cb
(
  rdm_assign_status_type status,
  rdm_service_enum_type  service,
  rdm_device_enum_type   device
)
{
  dps_svc_type svc = dps_map_rdm_to_dps_svc (service);
  //save the svc status for fallback if required
  ASSIGN_SVC_STATUS (dps_info[svc].svc_status[SVC_ALT], status, service, device);
  // ignore failure for standalone where MHI interface is not available
  if (RDM_DONE_S == status ||
     (RDM_DONE_S != status && MHI_SUCCESS != mhi_is_enumerated()))
  {
    //alt svc switch is successful
    dps_set_rex_sigs (svc, SVC_ALT, TRUE);
  }
  else
  {
    //fallback to last known good state
    dps_set_rex_sigs (svc, SVC_ALT, FALSE);
  }
}

//---------------------------------------------------------------------------------------------------
// triggers the switch logic
// 1) assign alt_diag/alt_dun svc to active interface
//     a) close running active interface on RDM
//     b) assign alt_diag/alt_dun on active interface
// 2) if successful, switch svc to non active interface
//---------------------------------------------------------------------------------------------------
void dps_start_switch (dps_svc_type svc)
{
  DPS_ULOG_1 ("DPS: Switch Started service: %d", svc);
  if (dps_check_if_valid_port (svc, dps_info[svc].alt_svc_port) != TRUE)
  {
    DPS_ULOG_2 ("DPS: no valid active interface for SVC_ALT service: %d, port: %u",
                                                  svc, dps_info[svc].alt_svc_port);
  }
  else
  {
    //save diag port for switch
    dps_info[svc].svc_port = dps_swap_sio_port (svc, dps_info[svc].alt_svc_port);
    //switch started - change state
    dps_info[svc].status = DPS_STATE_BUSY;
    //signal rdm to close current SVC_ALT and assign to active interface
    rdm_assign_port (dps_map_dps_to_rdm_alt_svc (svc), 
                     dps_swap_to_rdm_device (svc, dps_info[svc].alt_svc_port),
                     dps_alt_svc_switch_cb);
  }
}

//---------------------------------------------------------------------------------------------------
// counts packets / look for magic packet for trigger logic
// don't entertain requests if RDM/DPS busy
//---------------------------------------------------------------------------------------------------
void dps_alt_svc_msg_notify (dps_svc_type svc)
{
  DPS_ULOG_1("DPS: SVC_ALT RX service: %d", svc);
  dps_info[svc].packet_count++;

  if (TRUE == rdm_is_busy() || DPS_STATE_BUSY == dps_info[svc].status)
  {
    DPS_ULOG("DPS: RDM/DPS busy, ignoring switch packet!");
  }
  else if (TRUE == dps_info[svc].magic_pkt_rcvd)
  {
    DPS_ULOG_1 ("DPS: MAGIC PACKET RECEIVED on service: %d", svc);
    //trigger a switch immediately
    dps_info[svc].magic_pkt_rcvd = FALSE;
    dps_info[svc].packet_count = 0;
    dps_start_switch (svc);
  }
  else if (DPS_TRIGGER_MAX_PACKETS <= dps_info[svc].packet_count)
  {
    DPS_ULOG_1 ("DPS: SWITCH PACKET THRESHOLD on service: %d", svc);
    //trigger a switch - open ALT DIAG on non active interface
    dps_info[svc].packet_count = 0;
    dps_start_switch (svc);
  }

  if (DPS_TRIGGER_MAX_PACKETS < dps_info[svc].packet_count)
  { //comes here for RDM busy and packet count increase above threshold
    dps_info[svc].packet_count = 0;
  }
}

//---------------------------------------------------------------------------------------------------
// parse packet to check if magic pattern received
//---------------------------------------------------------------------------------------------------
boolean dps_check_if_magic_packet (dsm_item_type* item_ptr, dps_svc_type svc)
{
  boolean len_check = TRUE;
  int len = dsm_length_packet (item_ptr);
  int len_max = DIAG_MAGIC_PACKET_LEN;
  const uint8 * magic_packet = diag_magic_packet;

  if (DPS_DUN_SVC == svc)
  {
    len_check = FALSE;
    magic_packet = dun_magic_packet;
    len_max = DUN_MAGIC_PACKET_LEN;
  }

  //for DUN, magic packet is any sequence starting with AT, ignore length check here
  if (len_max != len && len_check)
    return FALSE;
  for (len = 0; len < len_max; len++)
  {
    if (magic_packet[len] != item_ptr->data_ptr[len])
      break;
  }
  if (len_max == len)
    return TRUE;
  return FALSE;
}

//---------------------------------------------------------------------------------------------------
// packet notification on inactive interface 
// enqueue request if applicable for switching
//---------------------------------------------------------------------------------------------------
void dps_alt_diag_rx_notify_cb (dsm_item_type ** item_ptr)
{
  if (TRUE != rdm_is_busy() && 
      DPS_STATE_BUSY != dps_info[DPS_DIAG_SVC].status &&
      TRUE == dps_check_if_magic_packet (*item_ptr, DPS_DIAG_SVC))
  {
    //magic packet received, trigger switch immediately
    dps_info[DPS_DIAG_SVC].magic_pkt_rcvd = TRUE;
  }

  //free DSM packet
  dsm_free_packet (item_ptr);
  (void) rex_set_sigs (dps_tcb, DPS_ALT_DIAG_SIO_RX_SIG);
}

//---------------------------------------------------------------------------------------------------
// packet notification on inactive interface 
// enqueue request if applicable for switching
//---------------------------------------------------------------------------------------------------
void dps_alt_dun_rx_notify_cb (dsm_item_type ** item_ptr)
{
  if (TRUE != rdm_is_busy() && 
      DPS_STATE_BUSY != dps_info[DPS_DUN_SVC].status &&
      TRUE == dps_check_if_magic_packet (*item_ptr, DPS_DUN_SVC))
  {
    //magic packet received, trigger switch immediately
    dps_info[DPS_DUN_SVC].magic_pkt_rcvd = TRUE;
  }

  //free DSM packet
  dsm_free_packet (item_ptr);
  (void) rex_set_sigs (dps_tcb, DPS_ALT_DUN_SIO_RX_SIG);
}

//---------------------------------------------------------------------------------------------------
// close request handling for alt_svc DIAG\DUN service
// notify RDM for result
//---------------------------------------------------------------------------------------------------
void dps_alt_svc_sio_close (dps_svc_type svc)
{
  DPS_ULOG_2 ("DPS: dps_alt_svc_sio_close, service: %d, port: %u", svc, dps_info[svc].alt_svc_port);
  if (dps_info[svc].alt_svc_sio_handle != SIO_NO_STREAM_ID)
  {
    /* Call SIO close with NULL close_func_ptr to discard the TX data
    ** immediately
    */
    sio_close(dps_info[svc].alt_svc_sio_handle, NULL );
    dps_info[svc].alt_svc_sio_handle = SIO_NO_STREAM_ID;

    rdm_notify (dps_map_dps_to_rdm_alt_svc (svc), RDM_DONE_S);
  }
}

//---------------------------------------------------------------------------------------------------
// open request handling for alt_svc DIAG/DUN service
// notify RDM for result
//---------------------------------------------------------------------------------------------------
void dps_alt_svc_sio_open (dps_svc_type svc)
{
  sio_ioctl_param_type  param           = {0};
  sio_open_type         open_properties = {0};
  sio_open_param_type   open_param      = {0};

  /*
  ** SIO OPEN logic:
  ** 1. ALT DIAG port should be closed before trying to open a new one
  ** 2. if sio open fails, wait for the next trigger
  */
  DPS_ULOG_2 ("DPS: dps_alt_svc_sio_open, service: %d, port: %u", svc, dps_info[svc].alt_svc_port);
  if (SIO_NO_STREAM_ID != dps_info[svc].alt_svc_sio_handle)
  {
    //we shouldn't land here
    DPS_ULOG_1 ("DPS: rdm_open_cb() invoked without close for service: %d", svc);
    return;
  }
  // Initialize watermark queue for SIO
  dsm_queue_init (&dps_info[svc].dps_tx_wm_queue,
                  DSM_DIAG_SIO_TX_ITEM_CNT * DSM_DIAG_SIO_TX_ITEM_SIZ,
                  &dps_info[svc].dps_tx_q);
  dsm_set_low_wm (&dps_info[svc].dps_tx_wm_queue,
                  DSM_DIAG_SIO_TX_ITEM_CNT * DSM_DIAG_SIO_TX_ITEM_SIZ);
  dsm_set_hi_wm (&dps_info[svc].dps_tx_wm_queue,
                 DSM_DIAG_SIO_TX_ITEM_CNT * DSM_DIAG_SIO_TX_ITEM_SIZ);

  open_param.dsm_pool_param.dsm_mempool_id = DSM_DIAG_SIO_RX_ITEM_POOL;

  open_properties.tx_queue    = &dps_info[svc].dps_tx_wm_queue;
  open_properties.port_id     = dps_info[svc].alt_svc_port;
  open_properties.open_param  = &open_param;
  if (DPS_DIAG_SVC == svc)
  {
    open_properties.stream_mode = SIO_DM_MODE;
    open_properties.rx_func_ptr = dps_alt_diag_rx_notify_cb;
  }
  else
  {
    open_properties.stream_mode = SIO_GENERIC_MODE;
    open_properties.rx_func_ptr = dps_alt_dun_rx_notify_cb;
  }

  // open interface
  dps_info[svc].alt_svc_sio_handle = sio_open (&open_properties);

  if (SIO_NO_STREAM_ID != dps_info[svc].alt_svc_sio_handle)
  {
    /* RDM issue open succeeded. Notify to RDM, so RDM can commit the current
    ** mapping to config file in EFS
    */
    rdm_notify (dps_map_dps_to_rdm_alt_svc(svc), RDM_DONE_S);
    // Set flow control to allow flow
    sio_ioctl (dps_info[svc].alt_svc_sio_handle, SIO_IOCTL_INBOUND_FLOW_ENABLE, &param);
    // Set CD line
    sio_ioctl (dps_info[svc].alt_svc_sio_handle, SIO_IOCTL_CD_ASSERT, &param);
    // Set DSR line
    sio_ioctl (dps_info[svc].alt_svc_sio_handle, SIO_IOCTL_DSR_ASSERT, &param);
    // Clear RI line
    sio_ioctl (dps_info[svc].alt_svc_sio_handle, SIO_IOCTL_RI_DEASSERT, &param);
  }
  else
  {
    //don't error fatal as this would fail for standalone
    DPS_ULOG_2 ("DPS: SVC_ALT SIO open failed on port, service: %d, port: %u", svc, dps_info[svc].alt_svc_port);
    rdm_notify (dps_map_dps_to_rdm_alt_svc(svc), RDM_DONE_S);
  }
}

//---------------------------------------------------------------------------------------------------
// dps task handling
//---------------------------------------------------------------------------------------------------
static rex_sigs_type dps_handle_sigs (rex_sigs_type mask)
{
  boolean found = FALSE;
  rex_sigs_type sigs = 0;    // Signals currently set for this task.
  rex_sigs_type composite_mask =  DPS_ALT_DIAG_OPEN_SIG |
                                  DPS_ALT_DIAG_CLOSE_SIG |
                                  DPS_ALT_DIAG_SWITCH_SIG |
                                  DPS_DIAG_SWITCH_SIG |
                                  DPS_ALT_DIAG_SWITCH_FAIL_SIG |
                                  DPS_DIAG_SWITCH_FAIL_SIG |
                                  DPS_ALT_DIAG_SIO_RX_SIG |
                                  DPS_ALT_DUN_SIO_RX_SIG |
                                  DPS_ALT_DUN_OPEN_SIG |
                                  DPS_ALT_DUN_CLOSE_SIG |
                                  DPS_ALT_DUN_SWITCH_SIG |
                                  DPS_DUN_SWITCH_SIG |
                                  DPS_ALT_DUN_SWITCH_FAIL_SIG |
                                  DPS_DUN_SWITCH_FAIL_SIG |
                                  DPS_TIMER_SIG;
  while (!found)
  {
    sigs = rex_wait (composite_mask);
    //DPS_ULOG_1 ("DPS: dps_handle_sigs sig = %u", sigs);
    if (sigs & mask)
    {
      found = TRUE;
    }
    else if (sigs & DPS_ALT_DIAG_OPEN_SIG)
    {
      (void) rex_clr_sigs (dps_tcb, DPS_ALT_DIAG_OPEN_SIG);
      dps_alt_svc_sio_open (DPS_DIAG_SVC);
    }
    else if (sigs & DPS_ALT_DIAG_CLOSE_SIG)
    {
      (void) rex_clr_sigs (dps_tcb, DPS_ALT_DIAG_CLOSE_SIG);
      dps_alt_svc_sio_close (DPS_DIAG_SVC);
    }
    else if (sigs & DPS_ALT_DIAG_SWITCH_SIG)
    {
      (void) rex_clr_sigs (dps_tcb, DPS_ALT_DIAG_SWITCH_SIG);
      dps_alt_svc_switch_handle (DPS_DIAG_SVC);
    }
    else if (sigs & DPS_DIAG_SWITCH_SIG)
    {
      (void) rex_clr_sigs (dps_tcb, DPS_DIAG_SWITCH_SIG);
      dps_svc_switch_handle (DPS_DIAG_SVC);
    }
    else if (sigs & DPS_ALT_DIAG_SWITCH_FAIL_SIG)
    {
      (void) rex_clr_sigs (dps_tcb, DPS_ALT_DIAG_SWITCH_FAIL_SIG);
      dps_fallback_switch_handle (DPS_DIAG_SVC);
    }
    else if (sigs & DPS_DIAG_SWITCH_FAIL_SIG)
    {
      (void) rex_clr_sigs (dps_tcb, DPS_DIAG_SWITCH_FAIL_SIG);
      dps_fallback_switch_handle (DPS_DIAG_SVC);
    }
    else if (sigs & DPS_ALT_DIAG_SIO_RX_SIG)
    {
      (void) rex_clr_sigs (dps_tcb, DPS_ALT_DIAG_SIO_RX_SIG);
      dps_alt_svc_msg_notify (DPS_DIAG_SVC);
    }

    //DUN service
    else if (sigs & DPS_ALT_DUN_OPEN_SIG)
    {
      (void) rex_clr_sigs (dps_tcb, DPS_ALT_DUN_OPEN_SIG);
      dps_alt_svc_sio_open (DPS_DUN_SVC);
    }
    else if (sigs & DPS_ALT_DUN_CLOSE_SIG)
    {
      (void) rex_clr_sigs (dps_tcb, DPS_ALT_DUN_CLOSE_SIG);
      dps_alt_svc_sio_close (DPS_DUN_SVC);
    }
    else if (sigs & DPS_ALT_DUN_SWITCH_SIG)
    {
      (void) rex_clr_sigs (dps_tcb, DPS_ALT_DUN_SWITCH_SIG);
      dps_alt_svc_switch_handle (DPS_DUN_SVC);
    }
    else if (sigs & DPS_DUN_SWITCH_SIG)
    {
      (void) rex_clr_sigs (dps_tcb, DPS_DUN_SWITCH_SIG);
      dps_svc_switch_handle (DPS_DUN_SVC);
    }
    else if (sigs & DPS_ALT_DUN_SWITCH_FAIL_SIG)
    {
      (void) rex_clr_sigs (dps_tcb, DPS_ALT_DUN_SWITCH_FAIL_SIG);
      dps_fallback_switch_handle (DPS_DUN_SVC);
    }
    else if (sigs & DPS_DUN_SWITCH_FAIL_SIG)
    {
      (void) rex_clr_sigs (dps_tcb, DPS_DUN_SWITCH_FAIL_SIG);
      dps_fallback_switch_handle (DPS_DUN_SVC);
    }
    else if (sigs & DPS_ALT_DUN_SIO_RX_SIG)
    {
      (void) rex_clr_sigs (dps_tcb, DPS_ALT_DUN_SIO_RX_SIG);
      dps_alt_svc_msg_notify (DPS_DUN_SVC);
    }
    else if (sigs & DPS_TIMER_SIG)
    {
      (void) rex_clr_sigs (dps_tcb, DPS_TIMER_SIG);
      dps_rdm_ready_cb (FALSE);
    }
  }
  return sigs;
}

//---------------------------------------------------------------------------------------------------
// requesting alt diag to open up port with new interface
//---------------------------------------------------------------------------------------------------
void dps_alt_svc_open_cb (sio_port_id_type port_id)
{
  switch (port_id)
  {
    case SIO_PORT_MHI_DIAG: // SIO_PORT_MHI_ALT_DIAG:
    case SIO_PORT_USB_DIAG: // SIO_PORT_USB_ALT_DIAG:
    {
      dps_info[DPS_DIAG_SVC].alt_svc_port = port_id;
      (void) rex_set_sigs (dps_tcb, DPS_ALT_DIAG_OPEN_SIG);
    }
    break;
    case SIO_PORT_MHI_DUN:
    case SIO_PORT_USB_MDM:
    {
      dps_info[DPS_DUN_SVC].alt_svc_port = port_id;
      (void) rex_set_sigs (dps_tcb, DPS_ALT_DUN_OPEN_SIG);
    }
    break;
    default:
    {
      // Unsupported PORT ID
      DPS_ULOG_1 ("DPS: ALT DIAG/DUN, invalid port requested %u", port_id);
    }
    break;
  }
}

//---------------------------------------------------------------------------------------------------
// request for closing svc_alt_diag interface
//---------------------------------------------------------------------------------------------------
void dps_alt_diag_close_cb (void)
{
  (void) rex_set_sigs (dps_tcb, DPS_ALT_DIAG_CLOSE_SIG);
}

//---------------------------------------------------------------------------------------------------
// request for closing alt_dun interface
//---------------------------------------------------------------------------------------------------
void dps_alt_dun_close_cb (void)
{
  (void) rex_set_sigs (dps_tcb, DPS_ALT_DUN_CLOSE_SIG);
}

//---------------------------------------------------------------------------------------------------
// initialize the task
// register for open/close cb for svc_alt
//---------------------------------------------------------------------------------------------------
void dps_task (uint32 params)
{
  //Retrieve tcb pointer from REX
  dps_tcb = rex_self();

  //init ulogs
  dps_ulog_init();
  memset (&dps_info, 0, sizeof (dps_info_t));
  dps_info[DPS_DIAG_SVC].alt_svc_sio_handle = SIO_NO_STREAM_ID;
  dps_info[DPS_DUN_SVC].alt_svc_sio_handle  = SIO_NO_STREAM_ID;

  DPS_ULOG ("DPS: dps_task cb registered");

  /* Register the open/close callbacks with RDM for DIAG */
  rdm_register_open_func (RDM_ALT_DIAG_SRVC, dps_alt_svc_open_cb);
  rdm_register_close_func (RDM_ALT_DIAG_SRVC, dps_alt_diag_close_cb);

  /* Register for DUN */
  rdm_register_open_func (RDM_ALT_DUN_SRVC, dps_alt_svc_open_cb);
  rdm_register_close_func (RDM_ALT_DUN_SRVC, dps_alt_dun_close_cb);

  // return to continue rcinit framework
  rcinit_handshake_startup ();

  // Begin task operations
  while (1)
  {
    (void) dps_handle_sigs (0);
  }
}

//---------------------------------------------------------------------------------------------------
// dps reinitialize cb
// update status
//---------------------------------------------------------------------------------------------------
void dps_reinit_svc_cb
(
  rdm_assign_status_type status,
  rdm_service_enum_type  service,
  rdm_device_enum_type   device
)
{
  dps_svc_type svc = dps_map_rdm_to_dps_svc (service);
  DPS_ULOG_3 ("DPS: DPS re-init CB, service: %u, device: %u, status: %u", service, device, status);
  ASSIGN_SVC_STATUS (dps_info[svc].svc_status[SVC_ALT], status, service, device);
}

//---------------------------------------------------------------------------------------------------
// check the RDM failures to re-initialize port switching service
// timer callback being invoked for monitoring RDM ready state
//---------------------------------------------------------------------------------------------------
static void dps_rdm_ready_cb (boolean ready)
{
  if (FALSE == ready && TRUE == rdm_is_busy ())
  {
    DPS_ULOG ("DPS: RDM busy, wait for another 10ms");
    timer_set (&dps_timer, DPS_RDM_READY_TIMEOUT, 0, T_MSEC);
  }
  else
  {
    //check current device setting on diag service
    rdm_device_enum_type pDiag = rdm_get_device (RDM_DIAG_SRVC);
    rdm_device_enum_type pAltDiag = rdm_get_device (RDM_ALT_DIAG_SRVC);

    //check current device setting on dun service
    rdm_device_enum_type pDun = rdm_get_device (RDM_DUN_SRVC);
    rdm_device_enum_type pAltDun = rdm_get_device (RDM_ALT_DUN_SRVC);

    if (dps_timer.timer_handle)
    {
      //rdm is ready, cancel the timer if ever set
      DPS_ULOG ("DPS: Cancel DPS timer");
      (void) timer_undef (&dps_timer);
    }

    if (RDM_NULL_DEV >= pAltDiag && RDM_NULL_DEV < pDiag)
    {
      //rdm map partially corrupted, assign port to ALT DIAG service
      DPS_ULOG ("DPS: RDM map fail, re-init ALT DIAG");
      rdm_assign_port (RDM_ALT_DIAG_SRVC,
                       dps_swap_rdm_device (DPS_DIAG_SVC, pDiag),
                       dps_reinit_svc_cb);
    }

    if (RDM_NULL_DEV >= pAltDun && RDM_NULL_DEV < pDun)
    {
      //rdm map partially corrupted, assign port to ALT DUN service
      DPS_ULOG ("DPS: RDM map fail, re-init ALT DUN");
      rdm_assign_port (RDM_ALT_DUN_SRVC,
                       dps_swap_rdm_device (DPS_DUN_SVC, pDun),
                       dps_reinit_svc_cb);
    }

    if (RDM_NULL_DEV >= pDiag && RDM_NULL_DEV >= pDun)
    {
      //rdm map fully corrupted, log an event
      DPS_ULOG ("DPS: WARNING! RDM map is fully corrupted!!!");
    }
  }
}

//---------------------------------------------------------------------------------------------------
// dps error handling for RDM failure cases
// timer impl for checking RDM readiness
//---------------------------------------------------------------------------------------------------
void dps_reinitialize (uint32 params)
{
  DPS_ULOG ("DPS: Re-INIT invoked");
  // if RDM busy, wait and recheck after some time 
  if (TRUE == rdm_is_busy())
  {
    /* Creating a timer with a call back function in debug group */
    timer_def (&dps_timer,               /* timer instance to set */
               &dps_timer_group,         /* timer group */
               dps_tcb,                  /* task TCB */
               DPS_TIMER_SIG,            /* REX signal set on timer expiry */
               0,                        /* cb to register */
               0                         /* data to cb */
              );

    // Set timer to keep firing every 10 ms
    timer_set (&dps_timer, DPS_RDM_READY_TIMEOUT, 0, T_MSEC);
  }
  else
  {
    //rdm is ready
    dps_rdm_ready_cb (TRUE);
  }
}
//======================================================================================================
//======================================================================================================

