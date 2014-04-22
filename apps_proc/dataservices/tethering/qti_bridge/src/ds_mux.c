/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     D S   M U X

GENERAL DESCRIPTION
  This file contains all the functions, definitions and data types needed
  for MUX processing

  DS MUX 27.010 core protocol and logic is implemented in this file

  Copyright (c) 2016, 2018,2021 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/dataservices.tx/6.0/tethering/qti_bridge/src/ds_mux.c#2 $
  $DateTime: 2021/05/05 04:35:02 $
  $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/01/16   ad     Initial version
===========================================================================*/


/*===========================================================================

                      INCLUDE FILES

===========================================================================*/
#include "ds_mux_utils.h"
#include "ds_mux.h"
#include "ds_mux_taski.h"
#include "ds_mux_svc.h"
#include "qti_bridge.h"
#include "comdef.h"
#include "ds_mux_logging.h"


extern qti_bridge_qurt_t     qti_bridge_qurt;
ds_mux_state_info_type       ds_mux_state_info;
dlci_cmux_param_type        *ds_mux_cmux_params = NULL;
ds_mux_ext_client_info_type *ds_mux_ext_client_info;
ds_mux_dlci_param_type      *ds_mux_dlci_conn[DS_MUX_MAX_LOGICAL_CONNECTION + 1] = {NULL,};

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

ds_mux_sig_enum_type ds_mux_get_sig_by_id
(
  uint32  dlci_id
)
{
  ds_mux_sig_enum_type  sig = DS_MUX_MAX_SIGNALS;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Get signal from DLCI ID
    -------------------------------------------------------------------------*/
  switch ( dlci_id )
  {
    case 1:
      sig = DS_MUX_TX_DLCI1_DATA_SIGNAL;
      break;

    case 2:
      sig = DS_MUX_TX_DLCI2_DATA_SIGNAL;
      break;

    case 3:
      sig = DS_MUX_TX_DLCI3_DATA_SIGNAL;
      break;

    default:
     sig = DS_MUX_MAX_SIGNALS;
     DS_MUX_LOG_MSG_ERROR_1( "ds_mux_get_sig_by_id invalid DLCI %d ", dlci_id);
     break;
  }

  return sig;
}/* ds_mux_get_sig_by_id */

static void ds_mux_t1_timer_cb
(
  void *  data
)
{
  uint32                          dlci_id = (uint32) data;
  ds_mux_dlci_param_type         *dlci_conn_param = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  dlci_conn_param = ds_mux_get_dlci_conn_ptr(dlci_id);

  if ( NULL == dlci_conn_param || DS_MUX_DLCI_CONNECTED != dlci_conn_param->state)
  {
    return;
  }


}/* ds_mux_t1_timer_cb */

static void ds_mux_t2_timer_cb
(
  void *  data
)
{
  uint32                         dlci_id         = (uint32) data;
  ds_mux_dlci_param_type         *dlci_conn_param = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_MUX_LOG_MSG_INFO2_1( "ds_mux_t2_timer_cb dlci_id %d ", dlci_id);

  /*-------------------------------------------------------------------------
    1. Re-transmit the packet if curr_transmission is less then N2 (Max Re- transmission )
    2. If Flow is disabled then don't start the timer as Host would not receive the cmd due
       to flow control. DS MUX do re- transmission again once flow become enabled and
       will start the timer
  -------------------------------------------------------------------------*/
  dlci_conn_param = ds_mux_get_dlci_conn_ptr(dlci_id);

  if ( NULL == dlci_conn_param || DS_MUX_DLCI_CONNECTED != dlci_conn_param->state)
  {
    DS_MUX_LOG_MSG_ERROR_1( "ds_mux_t2_timer_cb invalid DLCI conn %x state ", dlci_conn_param);
    return;
  }
  /*Step 1*/
  if ( dlci_conn_param->curr_transmissions_N2 <= dlci_conn_param->re_transmissions_N2)
  {
    /*Step 2*/
    if ( DS_MUX_FLOW_DISABLED != ds_mux_io_transmit_msg_cmd( dlci_conn_param->curr_msg_frame))
    {
    ds_mux_timer_start( dlci_conn_param->t2_timer_handle,
                        &dlci_conn_param->t2_timer_attr,
                        dlci_conn_param->response_timer_T2 * 10 );

    dlci_conn_param->curr_transmissions_N2++;
  }
  }
  else
  {
   //We should get response by now, else raising alarm
   DS_MUX_LOG_MSG_ERROR_1( "ds_mux_t2_timer_cb unable to recv msg ack from host %d",
     dlci_conn_param->curr_transmissions_N2);
    ASSERT(0);
  }

}/* ds_mux_t2_timer_cb */

static void ds_mux_t3_timer_cb
(
  void *  data
)
{
  uint32                          dlci_id = (uint32) data;
  ds_mux_dlci_param_type         *dlci_conn_param = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  dlci_conn_param = ds_mux_get_dlci_conn_ptr(dlci_id);

  if ( NULL == dlci_conn_param || DS_MUX_DLCI_CONNECTED != dlci_conn_param->state)
  {
    return;
  }

}/* ds_mux_t3_timer_cb */

void ds_mux_update_dlci_params
(
  ds_mux_dlci_param_type     *dlci_conn,
  ds_mux_subset_enum_type     frame_type,
  ds_mux_cl_type_enum_type    cl_type,
  uint8                       priprity,
  uint16                      response_timer_T1,
  uint16                      frame_size_N1,
  uint8                       re_transmissions_N2,
  uint8                       window_size_k
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  dlci_conn->frame_type          = frame_type;
  dlci_conn->cl_type             = cl_type;
  dlci_conn->priprity            = priprity;
  dlci_conn->response_timer_T1   = response_timer_T1;
  dlci_conn->frame_size_N1       = frame_size_N1;
  dlci_conn->re_transmissions_N2 = re_transmissions_N2;
  dlci_conn->window_size_k       = window_size_k;

  DS_MUX_LOG_MSG_INFO2_6( "ds_mux_update_dlci_params frame type %d "
                          "cl type %d priority %d T1 %d N1 %d N2 %d ",
                           dlci_conn->frame_type,
                           dlci_conn->cl_type,
                           dlci_conn->priprity ,
                           dlci_conn->response_timer_T1,
                           dlci_conn->frame_size_N1,
                           dlci_conn->re_transmissions_N2 );

}/* ds_mux_update_dlci_params*/

void ds_mux_inititialize_dlci_conn
(
  ds_mux_dlci_param_type  *dlci_conn,
  uint32                   dlci_id
)
{
  ds_mux_sig_enum_type                      sig;
  int                                       timer_ret;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( NULL == dlci_conn)
  {
    DS_MUX_LOG_MSG_ERROR_0( "ds_mux_inititialize_dlci_conn  DLCI conn ptr is NULL ");
    return;
  }
  /*-----------------------------------------------------------------------
    Initialize default values if CMUX params are NULL
  -----------------------------------------------------------------------*/
  if ( NULL == ds_mux_cmux_params)
  {
    ds_mux_update_dlci_params ( dlci_conn, DS_MUX_SUBSET_UIH, DS_MUX_CL_TYPE_1, DS_MUX_DEFAULT_DLCI_PRIORITY,
        DS_MUX_CMUX_DEFAULT_CMD_TIMER_T1, DS_MUX_CMUX_DEFAULT_FRAME_N1, DS_MUX_CMUX_DEFAULT_MAX_TX_N2,
        DS_MUX_WINDOW_SIZE_2);
    dlci_conn->response_timer_T2   = DS_MUX_CMUX_DEFAULT_DLCI0_TIMER_T2;
    dlci_conn->wake_up_timer_T3    = DS_MUX_CMUX_DEFAULT_TIMER_T3;
  }
  else
  {
    ds_mux_update_dlci_params ( dlci_conn, ds_mux_cmux_params->subset, DS_MUX_CL_TYPE_1, DS_MUX_DEFAULT_DLCI_PRIORITY,
        ds_mux_cmux_params->response_timer_T1, ds_mux_cmux_params->frame_size_N1, ds_mux_cmux_params->re_transmissions_N2,
        ds_mux_cmux_params->window_size_k);

    dlci_conn->response_timer_T2   = ds_mux_cmux_params->response_timer_T2;
    dlci_conn->wake_up_timer_T3    = ds_mux_cmux_params->wake_up_timer_T3;
  }

  dlci_conn->dlci_id               = (uint8)dlci_id;
  dlci_conn->state                 = DS_MUX_DLCI_INITIALIZED;

  /*-----------------------------------------------------------------------
    Initialize outstanding command and messages queues
  -----------------------------------------------------------------------*/
  memset( &dlci_conn->outstanding_cmd_frame_q, 0x0, sizeof(q_type));
  memset( &dlci_conn->outstanding_msg_frame_q, 0x0, sizeof(q_type));

  (void) q_init( &dlci_conn->outstanding_cmd_frame_q );
  (void) q_init( &dlci_conn->outstanding_msg_frame_q );

   dlci_conn->curr_msg_frame = NULL;
   dlci_conn->curr_cmd_frame = NULL;
   dlci_conn->pending_rx_cl4_data_ptr = NULL;
  /*-----------------------------------------------------------------------
    Initialize Timers and assosiated with DLCI
  -----------------------------------------------------------------------*/
  timer_ret = ds_mux_timer_alloc( &dlci_conn->t1_timer_handle,
                      &dlci_conn->t1_timer_attr,
                      ds_mux_t1_timer_cb,
                      (void *)dlci_id );
  if (timer_ret < QURT_EOK)
  {
    DS_MUX_LOG_MSG_ERROR_1( "ds_mux_inititialize_dlci_conn: Timer T1 alloc"
                            " failed %d",timer_ret);
    return;
  }

  timer_ret = ds_mux_timer_alloc( &dlci_conn->t2_timer_handle,
                      &dlci_conn->t2_timer_attr,
                      ds_mux_t2_timer_cb,
                      (void *)dlci_id );
  if (timer_ret < QURT_EOK)
  {
    DS_MUX_LOG_MSG_ERROR_1( "ds_mux_inititialize_dlci_conn: Timer T2 alloc"
                            " failed %d",timer_ret);
    return;
  }

  timer_ret = ds_mux_timer_alloc( &dlci_conn->t3_timer_handle,
                      &dlci_conn->t3_timer_attr,
                      ds_mux_t3_timer_cb,
                     (void *)dlci_id );
  if (timer_ret < QURT_EOK)
  {
    DS_MUX_LOG_MSG_ERROR_1( "ds_mux_inititialize_dlci_conn: Timer T3 alloc"
                            " failed %d",timer_ret);
    return;
  }
  dlci_conn->flow_control_mask = DS_MUX_FLOW_DISABLE_MIN;

  /*-----------------------------------------------------------------------
     Enable Uplink/Transmit data signal
     Note: Signal enable should be done at the end only
  -----------------------------------------------------------------------*/
  if ( DS_MUX_MAX_SIGNALS != (sig = ds_mux_get_sig_by_id( dlci_id )))
  {

    ds_mux_enable_sig( sig );

    ds_mux_set_sig_handler(sig,
                           ds_mux_io_tx_sig_hdlr,
                           (void *) dlci_id);
  }

  DS_MUX_LOG_MSG_INFO2_1( "ds_mux_inititialize_dlci_conn initialized dlci_id  %d ", dlci_id);

}/* ds_mux_inititialize_dlci_conn */

ds_mux_dlci_param_type*  ds_mux_get_alloc_conn_ptr
(
  uint32                   dlci_id
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( IS_DLCI_INVALID( dlci_id) )
  {
    DS_MUX_LOG_MSG_ERROR_1( "ds_mux_get_alloc_conn_ptr Invalid DLCI ID %d", dlci_id);
    return NULL;
  }

  DS_MUX_ENTER_CRIT_SECTION(&global_ds_mux_crit_section);
  /*-----------------------------------------------------------------------
     If DLCI is not allocate then allocate otherwise return previously allocated pointer
  -----------------------------------------------------------------------*/
  if ( NULL == ds_mux_dlci_conn[dlci_id])
  {
    ds_mux_dlci_conn[dlci_id] = (ds_mux_dlci_param_type*)malloc(sizeof(ds_mux_dlci_param_type));
    ds_mux_inititialize_dlci_conn( ds_mux_dlci_conn[dlci_id],  dlci_id);

    DS_MUX_LOG_MSG_INFO2_2( " ds_mux_get_alloc_conn_ptr Allocating dlci id %d conn ptr %x ",
        dlci_id, ds_mux_dlci_conn[dlci_id]);

  }

  DS_MUX_LEAVE_CRIT_SECTION(&global_ds_mux_crit_section);

  return ds_mux_dlci_conn[dlci_id];
}/* ds_mux_get_alloc_conn_ptr */

ds_mux_dlci_param_type* ds_mux_get_dlci_conn_ptr
(
  uint32                   dlci_id
)
{
  ds_mux_dlci_param_type  *dlci_conn = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( IS_DLCI_INVALID( dlci_id) )
  {
    DS_MUX_LOG_MSG_ERROR_1( "ds_mux_get_dlci_conn_ptr unable to find dlci_id %d", dlci_id);
    return NULL;
  }

  DS_MUX_ENTER_CRIT_SECTION(&global_ds_mux_crit_section);

  dlci_conn = ds_mux_dlci_conn[dlci_id];

  DS_MUX_LEAVE_CRIT_SECTION(&global_ds_mux_crit_section);

  if (  NULL == dlci_conn || dlci_conn->dlci_id != dlci_id)
  {
    DS_MUX_LOG_MSG_ERROR_2( "ds_mux_get_dlci_conn_ptr invalid dlci_id %d dlci conn %x", dlci_id, dlci_conn);

    return NULL;
  }

  return dlci_conn;
}/* ds_mux_get_dlci_conn_ptr */


void ds_mux_de_inititialize_dlci_conn
(
  ds_mux_dlci_param_type  *dlci_conn
)
{

  ds_mux_io_frame_struct_type *msg_ptr = NULL;
  ds_mux_sig_enum_type         sig;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_MUX_LOG_MSG_INFO2_1( "ds_mux_de_inititialize_dlci_conn de-initialized dlci conn ptr %x ", dlci_conn);

  if ( NULL == dlci_conn)
  {
    DS_MUX_LOG_MSG_ERROR_0( "ds_mux_de_inititialize_dlci_conn  DLCI conn ptr is NULL ");
    return;
  }

  /*-------------------------------------------------------------------------
    Re-set into global DLCI connection array
  -------------------------------------------------------------------------*/
  ds_mux_dlci_conn[dlci_conn->dlci_id] = NULL;

  /*-------------------------------------------------------------------------
    Flush the command and messages queue and  Deinitialize queues
  -------------------------------------------------------------------------*/
  while ( (msg_ptr = (ds_mux_io_frame_struct_type *) q_get( &dlci_conn->outstanding_cmd_frame_q )) != NULL )
  {
    DS_MUX_MEM_FREE(msg_ptr);
  }
  q_destroy( &dlci_conn->outstanding_cmd_frame_q );

  while ( (msg_ptr = (ds_mux_io_frame_struct_type *) q_get( &dlci_conn->outstanding_msg_frame_q )) != NULL )
  {
    DS_MUX_MEM_FREE(msg_ptr);
  }
  q_destroy( &dlci_conn->outstanding_msg_frame_q );

  /*-------------------------------------------------------------------------
    Free all timers assosiated with DLCI
  -------------------------------------------------------------------------*/
  (void)ds_mux_timer_free( dlci_conn->t1_timer_handle );
  (void)ds_mux_timer_free( dlci_conn->t2_timer_handle );
  (void)ds_mux_timer_free( dlci_conn->t3_timer_handle );

   dlci_conn->t1_timer_handle = DS_MUX_TIMER_INVALID_HANDLE;
   dlci_conn->t2_timer_handle = DS_MUX_TIMER_INVALID_HANDLE;
   dlci_conn->t3_timer_handle = DS_MUX_TIMER_INVALID_HANDLE;



  /*-------------------------------------------------------------------------
    Free all pending messages and commands
  -------------------------------------------------------------------------*/
  if ( NULL != dlci_conn->curr_msg_frame)
  {
    DS_MUX_MEM_FREE( dlci_conn->curr_msg_frame);
  }

  if ( NULL != dlci_conn->curr_cmd_frame)
  {
    DS_MUX_MEM_FREE( dlci_conn->curr_cmd_frame);
  }
  /*-------------------------------------------------------------------------
    Free up frag packet
  -------------------------------------------------------------------------*/
  if ( NULL != dlci_conn->pending_rx_cl4_data_ptr)
  {
    dsm_free_packet( &dlci_conn->pending_rx_cl4_data_ptr);
  }

  dlci_conn->curr_msg_frame            = NULL;
  dlci_conn->curr_cmd_frame            = NULL;
  dlci_conn->pending_rx_cl4_data_ptr   = NULL;

  /*-----------------------------------------------------------------------
    Disable Uplink/Transmit data signal
  -----------------------------------------------------------------------*/
  sig = ds_mux_get_sig_by_id( dlci_conn->dlci_id);

  (void) ds_mux_set_sig_handler(sig, NULL, NULL);

  DS_MUX_CLR_SIGNAL(sig);

}/* ds_mux_de_inititialize_dlci_conn*/

void ds_mux_logical_port_open_handler
(
  ds_mux_logical_open_info_type     *logical_port_open_info
)
{
  uint32                dlci_id;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( NULL == logical_port_open_info)
  {
    DS_MUX_LOG_MSG_ERROR_0( "ds_mux_logical_port_open_handler Port info ptr is NULL ");
    return;
  }

  if ( IS_DLCI_INVALID( logical_port_open_info->dlci_id))
  {
    DS_MUX_LOG_MSG_ERROR_1( "ds_mux_logical_port_open_handler Invalid DLCI ID %d",
      logical_port_open_info->dlci_id );
    return;
  }

  dlci_id = (uint8)logical_port_open_info->dlci_id;

  DS_MUX_LOG_MSG_INFO2_1( "ds_mux_logical_port_open_handler dlci_id %d ", dlci_id);

    /*-----------------------------------------------------------------------
      DLCI0 is control logical connection, client can't open it
    -----------------------------------------------------------------------*/
  if ( ( 0 == dlci_id ) || IS_DLCI_INVALID( dlci_id) )
  {
    return;
  }

  #if 0
  /*-------------------------------------------------------------------------
    Allocate the DLCI connection ptr if it was not allocated previously

    DLCI connection ptr can be allocated when DS MUX receive SABM cmd (Whenever DS MUX recv
    SABM before client open logical port)
  -------------------------------------------------------------------------*/
  if ( NULL == ds_mux_get_alloc_conn_ptr(dlci_id))
  {
    return ;
  }
  #endif
}/* ds_mux_logical_port_open_handler*/


void ds_mux_logical_port_close_handler
(
  ds_mux_logical_close_info_type   *logical_port_close_info
)
{
  uint8                                  fc_flag           = 1;
  uint8                                  rtc_asserted_flag = 0;
  uint8                                  cd_asserted_flag  = 0;
  uint8                                  ic_asserted_flag  = 0;
  uint8                                  rtr_asserted_flag = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( NULL == logical_port_close_info)
  {
    DS_MUX_LOG_MSG_ERROR_0( "ds_mux_logical_port_close_handler Port info ptr is NULL ");
    return;
  }

  if ( IS_DLCI_INVALID( logical_port_close_info->dlci_id))
  {
    DS_MUX_LOG_MSG_ERROR_1( "ds_mux_logical_port_close_handler Invalid DLCI ID %d",
      logical_port_close_info->dlci_id );
    return;
  }

  DS_MUX_LOG_MSG_INFO2_1( "ds_mux_logical_port_close_handler dlci_id %d ",
    logical_port_close_info->dlci_id);

  /*-----------------------------------------------------------------------
      TBD implementaion (Currently DS MUX is not supporting modem initiated opening and closing)
      1. DS MUX should move DLCI state to DISCONECTING
      2. DS MUX should send DISC connect message to Host
      3. Once Host send UA DS MUX do de-Init and free the memory
  -----------------------------------------------------------------------*/

 /*-----------------------------------------------------------------------
      Currently DS MUX send MSC with all signal low so that Host mux should stop sending data
  -----------------------------------------------------------------------*/
  ds_mux_prepare_and_send_msc_message( logical_port_close_info->dlci_id,
                                       fc_flag,
                                       rtc_asserted_flag,
                                       cd_asserted_flag,
                                       ic_asserted_flag,
                                       rtr_asserted_flag);

}

void ds_mux_prepare_and_send_msc_message
(
  uint8                                  dlci_id,
  uint8                                  fc_flag,
  uint8                                  rtc_asserted_flag,
  uint8                                  cd_asserted_flag,
  uint8                                  ic_asserted_flag,
  uint8                                  rtr_asserted_flag
)
{
  ds_mux_io_frame_struct_type           *output_frame   = NULL;
  dsm_item_type*                         resp_msg       = NULL;
  dsm_item_type                        **tx_ptr         = NULL;
  uint8                                  msg_type       = DS_MUX_DLCI_FRAME_VAL_MSC | DS_MUX_DLCI_MSG_TYPE_CR_MASK;
  byte                                   msg_len        = DS_MUX_DLCI_MSC_VAL_LEN;
  uint8                                  signal_frame_value;
  ds_mux_dlci_param_type                *dlci_conn_param = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  dlci_conn_param = ds_mux_get_dlci_conn_ptr(dlci_id);

  if ( NULL == dlci_conn_param || DS_MUX_DLCI_CONNECTED != dlci_conn_param->state)
  {
    DS_MUX_LOG_MSG_ERROR_1( "ds_mux_prepare_and_send_msc_message DLCI CONN ptr is %x ", dlci_conn_param);
    return;
  }
  if ( NULL == ( resp_msg = dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL)))
  {
    DS_MUX_LOG_MSG_ERROR_0( "ds_mux_prepare_and_send_msc_message unable to allocate DSM item");
    return;
  }

  tx_ptr = &resp_msg;

  do
  {
    /*-----------------------------------------------------------------------
          1. Allocate default frame
          2. Prepare frame
          3. Insert message type (Message type already have EA and CR bit set)
          4. Insert message length (Message len is 2, 1 DLCI Byte, 1 Signal Byte)
          5. Prepare DLCI ID (Last 2 bits will be 1 (including EA bit)) and insert DLCI ID
          6. Set all signal bits based on input parameters.and insert signal byte
          7. transmit the raw frame to DS MUX IO
    -----------------------------------------------------------------------*/
    /* Step 1 */
    output_frame = ds_mux_util_alloc_frame();

    if ( NULL == output_frame)
      break;

    /* Step 2 */
    ds_mux_util_prepare_data_frame( DS_MUX_DLCI_ID_0, DS_MUX_FRAME_TYPE_UTIL_UIH, &output_frame->frame);
    /* Step 3 */
    if ( 1 != dsm_pushdown_tail( tx_ptr, &msg_type, 1, DSM_DS_SMALL_ITEM_POOL))
      break;
    /* Step 4 */
    if( 1 != dsm_pushdown_tail( tx_ptr, &msg_len, 1, DSM_DS_SMALL_ITEM_POOL))
      break;
    /* Step 5 */
    dlci_id = dlci_id << 2;
    dlci_id |= 0x3;

    if( 1 != dsm_pushdown_tail( tx_ptr, &dlci_id, 1, DSM_DS_SMALL_ITEM_POOL))
      break;
    /* Step 6 */
    signal_frame_value = (DS_MUX_DLCI_MSG_TYPE_EA_MASK |
                         (fc_flag << 1) |
                         (rtc_asserted_flag << 2) |
                         (rtr_asserted_flag << 3)|
                         (ic_asserted_flag  << 6)|
                         (cd_asserted_flag << 7));

    if ( 1 != dsm_pushdown_tail (tx_ptr, &signal_frame_value, 1, DSM_DS_SMALL_ITEM_POOL))
      break;

    output_frame->frame.length = dsm_length_packet(*tx_ptr);
    output_frame->frame.information_ptr = *tx_ptr;
    /*-----------------------------------------------------------------------
      If this is first command then send to DS MUX IO otherwise queue this command into
      outstanding msg queue.
      Start the timer once DS MUX is successfully transmitted the packet.
    -----------------------------------------------------------------------*/

  /* Step 7 */
    if(  NULL == dlci_conn_param->curr_msg_frame)
    {
      dlci_conn_param->curr_msg_frame = output_frame;

      /*-------------------------------------------------------------------------
           If Flow is disabled then don't start the timer as Host would not receive the cmd due
           to flow control. DS MUX do re- transmission again once flow become enabled and
           will start the timer
      -------------------------------------------------------------------------*/
      if ( DS_MUX_FLOW_DISABLED != ds_mux_io_transmit_msg_cmd( output_frame ))
      {

        if ( DS_MUX_TIMER_INVALID_HANDLE != dlci_conn_param->t2_timer_handle )
        {
           ds_mux_timer_start( dlci_conn_param->t2_timer_handle,
                               &dlci_conn_param->t2_timer_attr,
                               dlci_conn_param->response_timer_T2 * 10);
          dlci_conn_param->curr_transmissions_N2 = 1;
        }

      }
    }
    else
    {
      q_put( &dlci_conn_param->outstanding_msg_frame_q, &output_frame->link );
    }

    DS_MUX_LOG_MSG_INFO2_1( "ds_mux_prepare_and_send_msc_message signal byte %d ", signal_frame_value);

    return;
  }while(0);

  if ( NULL != tx_ptr && NULL != *tx_ptr)
  {
    dsm_free_packet( tx_ptr );
  }

  if (  NULL != output_frame)
  {
    DS_MUX_MEM_FREE( output_frame );
  }
  return;
}/*ds_mux_prepare_and_send_msc_message*/

void ds_mux_logical_port_ioctl_handler
(
  ds_mux_logical_ioctl_info_type   *logical_port_ioctl_info
)
{
  uint8                                  rtr_asserted_flag = 1;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( NULL == logical_port_ioctl_info)
  {
    DS_MUX_LOG_MSG_ERROR_0( "ds_mux_logical_port_ioctl_handler NULL port info ");
    return;
  }

  if ( IS_DLCI_INVALID( logical_port_ioctl_info->dlci_id))
  {
    DS_MUX_LOG_MSG_ERROR_1( "ds_mux_logical_port_ioctl_handler invalid dlci %d", logical_port_ioctl_info->dlci_id);
    return;
  }

  DS_MUX_LOG_MSG_INFO2_1( "ds_mux_logical_port_ioctl_handler dlci_id %d ",
    logical_port_ioctl_info->dlci_id);

  /*-----------------------------------------------------------------------
     Prepare and Send MSC message to Host(Remote MUX)
   -----------------------------------------------------------------------*/
  ds_mux_prepare_and_send_msc_message( logical_port_ioctl_info->dlci_id,
                                       logical_port_ioctl_info->fc,
                                       logical_port_ioctl_info->rtc,
                                       logical_port_ioctl_info->data_valid,
                                       logical_port_ioctl_info->ic,
                                       rtr_asserted_flag);

  return;
}/* ds_mux_logical_port_ioctl_handler */

static void ds_muxi_cmd_hdlr
(
  ds_mux_cmd_enum_type     cmd,
  void                  * user_data_ptr
)
{
  ds_muxi_cmd_info_type  * cmd_info_ptr;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  do
  {
    if (DS_MUX_CMD != cmd || NULL == user_data_ptr)
    {
      DS_MUX_LOG_MSG_ERROR_2("ds_muxi_cmd_hdlr(): Invalid arg, cmd %d user data 0x%p",
                        cmd, user_data_ptr);
      break;
    }

    cmd_info_ptr = (ds_muxi_cmd_info_type *) user_data_ptr;

    switch( cmd_info_ptr->cmd_id)
    {
      case DS_MUXI_PHY_PORT_OPEN:
      {
        ds_mux_phys_rdm_open_handler( cmd_info_ptr->cmd_info.phy_port_open_info.port_id);
        break;
      }

      case DS_MUXI_LOGICAL_PORT_OPEN:
      {
        ds_mux_logical_port_open_handler( &cmd_info_ptr->cmd_info.logical_port_open_info);
        break;
      }

      case DS_MUXI_LOGICAL_PORT_CLOSE:
      {
        ds_mux_logical_port_close_handler( &cmd_info_ptr->cmd_info.logical_port_close_info);
        break;
      }

      case DS_MUXI_LOGICAL_PORT_IOCTL:
      {
        ds_mux_logical_port_ioctl_handler( &cmd_info_ptr->cmd_info.logical_port_ioctl_info);
        break;
      }

      default:
      {
        break;
      }
    }
  }while(0);

  DS_MUX_MEM_FREE(user_data_ptr);

}/* ds_muxi_cmd_hdlr */


void ds_muxi_init()
{
  uint8 index = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_mux_cmux_params      = NULL;
  ds_mux_ext_client_info  = NULL;

  memset(&ds_mux_state_info, 0x0, sizeof( ds_mux_state_info));

  ds_mux_set_mux_state(DS_MUX_STATE_CLOSED);

  ds_mux_set_is_initiator(FALSE);

  for ( index = 0; index <= DS_MUX_MAX_LOGICAL_CONNECTION; index++)
  {
    ds_mux_dlci_conn[index] = NULL;
  }
  ds_mux_logical_init();

  ds_mux_phys_init();

  ds_mux_io_init();

  (void) ds_mux_set_cmd_handler(DS_MUX_CMD, ds_muxi_cmd_hdlr);

  DS_MUX_LOG_MSG_INFO2_0( "ds_muxi_init done ");

}/*ds_cmux_init*/
void ds_muxi_deinit()
{
  uint8 index = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_mux_set_mux_state(DS_MUX_STATE_CLOSED);
  ds_mux_set_is_initiator(FALSE);

  for ( index = 0; index <= DS_MUX_MAX_LOGICAL_CONNECTION; index++)
  {
    if ( NULL != ds_mux_dlci_conn[index])
    {
      ds_mux_de_inititialize_dlci_conn( ds_mux_dlci_conn[index]);
      DS_MUX_MEM_FREE( ds_mux_dlci_conn[index]);
      ds_mux_dlci_conn[index] = NULL;
    }
  }

  ds_mux_phys_deinit();

  ds_mux_io_deinit();

  if ( NULL != ds_mux_cmux_params)
  {
     DS_MUX_MEM_FREE(ds_mux_cmux_params);
  }

  if ( NULL != ds_mux_ext_client_info )
  {
    if ( NULL != ds_mux_ext_client_info->event_info )
    {
      DS_MUX_MEM_FREE(ds_mux_ext_client_info->event_info);
    }
    DS_MUX_MEM_FREE(ds_mux_ext_client_info);
  }

  DS_MUX_LOG_MSG_INFO2_0( "ds_muxi_deinit done ");

}/*ds_muxi_deinit*/

ds_mux_result_enum_type   ds_mux_initialize_cmux_param
(
  dlci_cmux_param_type    *cmux_param
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_mux_cmux_params = (dlci_cmux_param_type* )malloc(sizeof(dlci_cmux_param_type));

  if ( NULL == ds_mux_cmux_params)
  {
    return DS_MUX_FAILURE;
  }
  ds_mux_cmux_params->operating_mode = ( ( 0 != (cmux_param->mask & DS_MUX_SET_MODE)  ) ?
    cmux_param->operating_mode : DS_MUX_MODE_BASIC );

  ds_mux_cmux_params->subset = ( ( 0 != (cmux_param->mask & DS_MUX_SET_SUBSET)  ) ?
    cmux_param->subset : DS_MUX_SUBSET_UIH );

  ds_mux_cmux_params->port_speed = ( ( 0 != (cmux_param->mask & DS_MUX_SET_PORT_SPEED)  ) ?
    cmux_param->port_speed : DS_MUX_PHY_PORT_SPEED_1 );

  ds_mux_cmux_params->frame_size_N1 = ( ( 0 != (cmux_param->mask & DS_MUX_SET_FRAME_SIZE_N1)  ) ?
    cmux_param->frame_size_N1 : DS_MUX_CMUX_DEFAULT_FRAME_N1 ) ;

  ds_mux_cmux_params->response_timer_T1 = ( ( 0 != (cmux_param->mask & DS_MUX_SET_ACK_TIMER_T1)  ) ?
    cmux_param->response_timer_T1 : DS_MUX_CMUX_DEFAULT_CMD_TIMER_T1 );

  ds_mux_cmux_params->re_transmissions_N2 = ( ( 0 != (cmux_param->mask & DS_MUX_SET_RE_TRIES_N2)  ) ?
    cmux_param->re_transmissions_N2 : DS_MUX_CMUX_DEFAULT_MAX_TX_N2 );

  ds_mux_cmux_params->response_timer_T2 = ( ( 0 != (cmux_param->mask & DS_MUX_SET_RESP_TIMER_T2)  ) ?
    cmux_param->response_timer_T2 : DS_MUX_CMUX_DEFAULT_DLCI0_TIMER_T2 );

  ds_mux_cmux_params->wake_up_timer_T3 =  ( ( 0 != (cmux_param->mask & DS_MUX_SET_WAKEUP_TIMER_T3)  ) ?
    cmux_param->wake_up_timer_T3 : DS_MUX_CMUX_DEFAULT_TIMER_T3 );

  ds_mux_cmux_params->window_size_k = ( ( 0 != (cmux_param->mask & DS_MUX_SET_WINDOW_SIZE_K)  ) ?
    cmux_param->window_size_k : DS_MUX_WINDOW_SIZE_2 );

  ds_mux_cmux_params->mask = DS_MUX_SET_ALL_CMUX_FIELDS;

  return DS_MUX_SUCCESS;

}/*ds_mux_initialize_cmux_param*/


ds_mux_result_enum_type   ds_muxi_passive_init
(
  dlci_cmux_param_type    *cmux_param
)
{
  ds_mux_result_enum_type   result  = DS_MUX_SUCCESS;
  ds_mux_state_enum_type    ds_mux_prev_state = ds_mux_get_mux_state();
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_MUX_LOG_MSG_INFO2_1( "ds_muxi_passive_init cmux ptr %x ", cmux_param);

  if ( NULL == cmux_param )
  {
    return DS_MUX_NULL_PARAM;
  }
  do
  {
    /*-----------------------------------------------------------------------
      1. Initialize  CMUX param.
      2. Initialize Physical connection
      3. Create DLCI 0 control block
    -----------------------------------------------------------------------*/
    if ( DS_MUX_FAILURE == ds_mux_initialize_cmux_param( cmux_param ) )
    {
      result  = DS_MUX_FAILURE;
      break;
    }

    if ( DS_MUX_FAILURE == ds_mux_phys_conn_init( ) )
    {
      result  = DS_MUX_FAILURE;
      break;
    }

    if ( DS_MUX_FAILURE == ds_mux_io_initialize_wmk( ) )
    {
      result  = DS_MUX_FAILURE;
      break;
    }

    ds_mux_set_mux_state(DS_MUX_STATE_INITIALIZING);

    DS_MUX_LOG_MSG_INFO2_0( "ds_muxi_passive_init Success  ");

    return result;

  }while(0);

  DS_MUX_LOG_MSG_ERROR_0("ds_muxi_passive_init(): Failed ");

  /*-----------------------------------------------------------------------
    1.  De-allocate all resources
    2.  Set DS MUX state to previous state
    3.  return result error
  -----------------------------------------------------------------------*/
  if ( NULL != ds_mux_cmux_params )
  {
    DS_MUX_MEM_FREE(ds_mux_cmux_params);
  }

  ds_mux_phys_deinit();

  ds_mux_io_deinit();

  result       = DS_MUX_FAILURE;

  ds_mux_set_mux_state(ds_mux_prev_state);

  return result;
}/*ds_muxi_passive_init*/

ds_mux_result_enum_type  ds_mux_set_client_param
(
  sio_stream_id_type                  stream_id,
  ds_mux_ext_client_event_info_type  *event_info
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_mux_ext_client_info = (ds_mux_ext_client_info_type*)malloc(sizeof(ds_mux_ext_client_info_type));
  if ( NULL == ds_mux_ext_client_info)
  {
    DS_MUX_LOG_MSG_ERROR_0("ds_mux_set_client_param(): client info is NULL ");
    return DS_MUX_FAILURE;
  }

  ds_mux_ext_client_info->stream_id   = stream_id;

  if ( NULL != event_info)
  {
    ds_mux_ext_client_info->event_info = (ds_mux_ext_client_event_info_type*)malloc(sizeof(ds_mux_ext_client_event_info_type));
    if ( NULL != ds_mux_ext_client_info->event_info )
    {
      ds_mux_ext_client_info->event_info->event_cb_fn = event_info->event_cb_fn;
      ds_mux_ext_client_info->event_info->client_data = event_info->client_data;
    }
  }
  else
  {
    ds_mux_ext_client_info->event_info = NULL;
  }

  return DS_MUX_SUCCESS;
} /*ds_mux_set_client_param*/

ds_mux_result_enum_type  ds_muxi_active_init
(
  boolean                             is_cmux_supported,
  sio_stream_id_type                    stream_id,
  ds_mux_ext_client_event_info_type  *event_info
)
{

  ds_mux_result_enum_type  result  = DS_MUX_SUCCESS;
  ds_mux_state_enum_type   ds_mux_prev_state = ds_mux_get_mux_state();
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_MUX_LOG_MSG_INFO2_3( "ds_muxi_active_init is_cmux_supported %d sio port %d event info %x",
    is_cmux_supported, stream_id, event_info);

  do
  {
    /*-----------------------------------------------------------------------
     1. Store client parameters  (call back info). Client will be notified via call back when
         DS MUX completed  connect operation (Failed/Success) or CLD (close down) command
         processed and DS MUX closed down the MUX
     2. Connect to sio port provided by client
   -----------------------------------------------------------------------*/
    if ( DS_MUX_FAILURE == ds_mux_set_client_param( stream_id, event_info) )
    {
      result  = DS_MUX_FAILURE;
      break;
    }

    if ( DS_MUX_FAILURE == ds_mux_phys_port_connect(is_cmux_supported, stream_id ))
    {
      result  = DS_MUX_FAILURE;
      break;
    }
    //ds_mux_set_mux_state(DS_MUX_STATE_PHY_PORT_CONFIG);

    DS_MUX_LOG_MSG_INFO2_0( "ds_muxi_active_init Success ");

    return result;

  }while(0);

   DS_MUX_LOG_MSG_ERROR_0("ds_muxi_active_init(): Failed ");

  /*-----------------------------------------------------------------------
    1.  De-allocate all resources
    2.  Set DS MUX state to previous state
    3.  return result error
  -----------------------------------------------------------------------*/
  if ( NULL != ds_mux_ext_client_info )
  {
    if ( NULL != ds_mux_ext_client_info->event_info )
    {
      DS_MUX_MEM_FREE(ds_mux_ext_client_info->event_info);
    }
    DS_MUX_MEM_FREE(ds_mux_ext_client_info);
  }

  result       = DS_MUX_FAILURE;

  ds_mux_set_mux_state(ds_mux_prev_state);

  return result;
}/*ds_muxi_active_init*/

void ds_mux_close_down_mux
(

)
{
  uint8                        dlci_id = DS_MUX_DLCI_ID_1;
  ds_mux_dlci_param_type      *dlci_conn_param = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_MUX_LOG_MSG_INFO2_0( " ds_mux_close_down_mux Closing down MUX functionality");

  /*-----------------------------------------------------------------------
    1.  Make sure ALL DLCIs should be closed. ( Spec 27.010 5.8.2 Close-down procedure)
    2.  Close down DLCI 0 (Free all resources associated with  DLCI 0 )
    3.  Notify client about MUX closing
  -----------------------------------------------------------------------*/
  /*Step 1*/
  for(dlci_id = DS_MUX_DLCI_ID_1;  dlci_id <= DS_MUX_MAX_LOGICAL_CONNECTION; dlci_id++  )
  {
      //ALL DLCIs should be disconnected except DLCI 0
      if (NULL != (dlci_conn_param = ds_mux_get_dlci_conn_ptr( dlci_id )))
      {
          ds_mux_disconnect_dlci( dlci_conn_param );
      }
  }

  /*Steo 2*/
   dlci_conn_param = ds_mux_get_dlci_conn_ptr( DS_MUX_DLCI_ID_0 );

   if ( NULL != dlci_conn_param )
   {
     ds_mux_disconnect_dlci( dlci_conn_param );
   }

  /*Step 3*/
  if ( NULL != ds_mux_ext_client_info &&
       NULL != ds_mux_ext_client_info->event_info &&
       NULL != ds_mux_ext_client_info->event_info->event_cb_fn )
  {
    ds_mux_ext_client_info->event_info->event_cb_fn( DS_MUX_EVENT_CLOSE_COMPLETE,
                           NULL, ds_mux_ext_client_info->event_info->client_data);
  }

  ds_muxi_deinit();

  DS_MUX_LOG_MSG_INFO2_0( " ds_mux_close_down_mux MUX Closed down ");

}/*ds_mux_close_down_mux*/

void ds_mux_update_phy_port_open_status
(
  ds_mux_result_enum_type  port_open_status
)
{
  boolean  result = TRUE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_MUX_LOG_MSG_INFO2_0( " ds_mux_update_phy_port_open_status ");

  /*-----------------------------------------------------------------------
    1. Allocate resouces for DLCI0 connection and initialize them
    2  Notify Logical interface, Physical port open successful
    3   If port open fails then De-Init MUX so that client can re-initialize DS MUX again
    4.  Notify Application about physical port open (If App registered with call back)
    -----------------------------------------------------------------------*/
  do
  {
    if ( DS_MUX_SUCCESS == port_open_status )
    {
    /* Step 1 */
      if ( NULL == ds_mux_get_alloc_conn_ptr(DS_MUX_DLCI_ID_0))
      {
        result = FALSE;
        break;
      }
      ds_mux_set_mux_state(DS_MUX_STATE_OPENED);
      /* Step 2 -- Sid: Is this needed here???Check */
      ds_mux_logical_notify_port_open();
    }
    else if ( DS_MUX_FAILURE == port_open_status )
    {
       result = FALSE;
       break;
    }
  }while( 0 );

  /* Step 3 */
  if ( NULL != ds_mux_ext_client_info &&
       NULL != ds_mux_ext_client_info->event_info &&
       NULL != ds_mux_ext_client_info->event_info->event_cb_fn )
  {
    if ( TRUE == result)
    {
      ds_mux_ext_client_info->event_info->event_cb_fn( DS_MUX_EVENT_INIT_COMPLETE,
                           NULL, ds_mux_ext_client_info->event_info->client_data);
    }
    else
    {
      ds_mux_ext_client_info->event_info->event_cb_fn( DS_MUX_EVENT_INIT_FAILED,
                          NULL, ds_mux_ext_client_info->event_info->client_data);
    }
  }
  /* Step 4*/
  if ( FALSE == result )
  {
    ds_muxi_deinit();
  }
}/* ds_mux_update_phy_port_open_status */

ds_mux_state_enum_type ds_mux_get_mux_state
(

)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return ds_mux_state_info.ds_mux_state;
}/* ds_mux_get_mux_state */

boolean ds_mux_is_initiator
(

)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return ds_mux_state_info.is_initiator;
}/* ds_mux_is_initiator */

void ds_mux_set_mux_state
(
  ds_mux_state_enum_type  state
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_MUX_LOG_MSG_INFO2_2( " ds_mux_set_mux_state old state %d new state %d",
    ds_mux_state_info.ds_mux_state, state);

  DS_MUX_ENTER_CRIT_SECTION(&global_ds_mux_crit_section);
  ds_mux_state_info.ds_mux_state  = state;
  DS_MUX_LEAVE_CRIT_SECTION(&global_ds_mux_crit_section);
}/*ds_mux_set_mux_state*/

void ds_mux_set_is_initiator
(
  boolean  is_initiator
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_MUX_LOG_MSG_INFO2_1( " ds_mux_set_is_initiator  %d", is_initiator);

  DS_MUX_ENTER_CRIT_SECTION(&global_ds_mux_crit_section);
  ds_mux_state_info.is_initiator  = is_initiator;
  DS_MUX_LEAVE_CRIT_SECTION(&global_ds_mux_crit_section);

}/*ds_mux_set_is_initiator*/

ds_mux_result_enum_type ds_mux_get_msg_type
(
  byte                          msg_byte,
  ds_mux_msg_type_enum_type    *msg_type
)
{
  ds_mux_result_enum_type       result           = DS_MUX_SUCCESS;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  switch ((uint32)(msg_byte & (0xff ^ DS_MUX_DLCI_MSG_TYPE_CR_MASK) ))
   {
      case DS_MUX_DLCI_FRAME_VAL_PN:
         *msg_type =  DS_MUX_MSG_TYPE_VAL_PN;
          break;
      case DS_MUX_DLCI_FRAME_VAL_PSC:
         *msg_type = DS_MUX_MSG_TYPE_VAL_PSC;
          break;
      case DS_MUX_DLCI_FRAME_VAL_CLD:
         *msg_type = DS_MUX_MSG_TYPE_VAL_CLD;
          break;
      case DS_MUX_DLCI_FRAME_VAL_TEST:
         *msg_type = DS_MUX_MSG_TYPE_VAL_TEST;
          break;
      case DS_MUX_DLCI_FRAME_VAL_FCON:
         *msg_type = DS_MUX_MSG_TYPE_VAL_FCON;
          break;
      case DS_MUX_DLCI_FRAME_VAL_FCOFF:
         *msg_type = DS_MUX_MSG_TYPE_VAL_FCOFF;
          break;
      case DS_MUX_DLCI_FRAME_VAL_MSC:
         *msg_type = DS_MUX_MSG_TYPE_VAL_MSC;
          break;
      case DS_MUX_DLCI_FRAME_VAL_NSC:
         *msg_type = DS_MUX_MSG_TYPE_VAL_NSC;
          break;
      case DS_MUX_DLCI_FRAME_VAL_RPN:
         *msg_type = DS_MUX_MSG_TYPE_VAL_RPN;
          break;
      case DS_MUX_DLCI_FRAME_VAL_RLS:
         *msg_type = DS_MUX_MSG_TYPE_VAL_RLS;
          break;
      case DS_MUX_DLCI_FRAME_VAL_SNC:
         *msg_type = DS_MUX_MSG_TYPE_VAL_SNC;
          break;
      default:
        result           = DS_MUX_FAILURE;
   }

  DS_MUX_LOG_MSG_INFO2_2( " ds_mux_get_msg_type msg type %d msg_byte %d ", *msg_type, msg_byte);

  return result;
}/* ds_mux_get_msg_type */

void ds_mux_prepare_msg_response
(
  ds_mux_io_frame_type    *input_frame,
  ds_mux_io_frame_type    *output_frame
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
   Prepare ouput frame from input frame.Length and info field will be updated as par of message processing
  -----------------------------------------------------------------------*/
  ds_mux_util_prepare_cmd_frame( input_frame->dlci, input_frame->frame_type,
    FALSE, input_frame->poll_final, output_frame );

  return ;
}/* ds_mux_prepare_msg_response */

ds_mux_result_enum_type ds_mux_get_msg_frame_type
(
  uint8    input_frame_type,
  ds_mux_subset_enum_type*   output_fram_type
)
{
  ds_mux_result_enum_type       result           = DS_MUX_SUCCESS;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  switch (input_frame_type)
  {
     case 0:
        *output_fram_type = DS_MUX_SUBSET_UIH;
        break;
     case 1:
        *output_fram_type =  DS_MUX_SUBSET_UI;
        break;
     default:
        result   = DS_MUX_FAILURE;
  }

  DS_MUX_LOG_MSG_INFO2_2( " ds_mux_get_msg_frame_type input frame type %d output fram type %d ",
    input_frame_type, *output_fram_type);

  return result;
}/* ds_mux_get_msg_frame_type */

ds_mux_result_enum_type ds_mux_get_msg_cl_type
(
  uint8    input_cl_type,
  ds_mux_cl_type_enum_type*   output_cl_type
)
{
  ds_mux_result_enum_type       result           = DS_MUX_SUCCESS;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  switch (input_cl_type)
  {
    case 0:
      *output_cl_type = DS_MUX_CL_TYPE_1;
	  break;
    case 1:
      *output_cl_type = DS_MUX_CL_TYPE_2;
	  break;
    case 2:
      *output_cl_type = DS_MUX_CL_TYPE_3;
	  break;
    case 3:
      *output_cl_type = DS_MUX_CL_TYPE_4;
	  break;
    default:
        result   = DS_MUX_FAILURE;
  }

  DS_MUX_LOG_MSG_INFO2_2( " ds_mux_get_msg_frame_type input_cl_type %d output_cl_type %d ",
    input_cl_type, *output_cl_type);

  return result;
}/* ds_mux_get_msg_frame_type */

ds_mux_result_enum_type ds_mux_send_outstanding_msg
(
  uint8                     msg_resp_type,
  ds_mux_dlci_param_type   *dlci_conn_param
)
{
  ds_mux_result_enum_type       result           = DS_MUX_SUCCESS;
  uint8                         curr_msg_type;
  ds_mux_io_frame_struct_type   *msg_ptr = NULL;
  boolean                        send_next_msg    = TRUE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  do
  {
    /*-----------------------------------------------------------------------
    Ignore memory free and send outstanding message from queue
    -----------------------------------------------------------------------*/
    if ( NULL == dlci_conn_param->curr_msg_frame )
    {
      break;
    }
    /*-----------------------------------------------------------------------
    Ignore info ptr memory free and send outstanding message from queue
    -----------------------------------------------------------------------*/
    if ( NULL == dlci_conn_param->curr_msg_frame->frame.information_ptr )
    {
      DS_MUX_MEM_FREE(dlci_conn_param->curr_msg_frame);
      dlci_conn_param->curr_msg_frame = NULL;
      break;
    }

    /*-----------------------------------------------------------------------
    Compare current message type and  response message type type. If they are not same
    they dont free the memory. Because we could got stale response
    -----------------------------------------------------------------------*/
    if(  1 != dsm_pullup_long ( &dlci_conn_param->curr_msg_frame->frame.information_ptr, &curr_msg_type, 1) )
    {
      dsm_free_packet(&dlci_conn_param->curr_msg_frame->frame.information_ptr);
      DS_MUX_MEM_FREE(dlci_conn_param->curr_msg_frame);
      dlci_conn_param->curr_msg_frame = NULL;
      break;
    }

    DS_MUX_LOG_MSG_INFO2_2( " ds_mux_send_outstanding_msg  curr_msg_type %d msg_resp_type %d",
      curr_msg_type, msg_resp_type);
	  
    /*-----------------------------------------------------------------------
    Current message type is command type hence ignore CR mask before comparison
    -----------------------------------------------------------------------*/ 
    if ( (curr_msg_type & (~DS_MUX_DLCI_MSG_TYPE_CR_MASK)) !=  msg_resp_type )
    {
      send_next_msg    = FALSE;
      break;
    }

     /*-----------------------------------------------------------------------
    Free memory for current frame
    -----------------------------------------------------------------------*/
    dsm_free_packet(&dlci_conn_param->curr_msg_frame->frame.information_ptr);
    DS_MUX_MEM_FREE(dlci_conn_param->curr_msg_frame);
    dlci_conn_param->curr_msg_frame = NULL;

  }while( 0 );

  if ( TRUE == send_next_msg)
  {
    /*-----------------------------------------------------------------------
      Cancel current running timer
    -----------------------------------------------------------------------*/
    if ( DS_MUX_TIMER_INVALID_HANDLE != dlci_conn_param->t2_timer_handle )
    {
      ds_mux_timer_free( dlci_conn_param->t2_timer_handle);
      dlci_conn_param->curr_transmissions_N2 = 0;
    }
    /*-----------------------------------------------------------------------
      Take out frame from queue and send it
    -----------------------------------------------------------------------*/
    if ( (msg_ptr = (ds_mux_io_frame_struct_type *) q_get( &dlci_conn_param->outstanding_msg_frame_q )) != NULL )
    {
      dlci_conn_param->curr_msg_frame = msg_ptr;

      if ( DS_MUX_FLOW_DISABLED != ds_mux_io_transmit_msg_cmd( msg_ptr ) )
      {
        //Now start T2 timer
        if ( DS_MUX_TIMER_INVALID_HANDLE != dlci_conn_param->t2_timer_handle )
        {
           ds_mux_timer_start( dlci_conn_param->t2_timer_handle,
                               &dlci_conn_param->t2_timer_attr,
                               dlci_conn_param->response_timer_T2 * 10);
           dlci_conn_param->curr_transmissions_N2 = 1;
        }

        DS_MUX_LOG_MSG_INFO2_0( " ds_mux_send_outstanding_msg  sent message from queue");

      }
    }
  }

  return result;
}/* ds_mux_send_outstanding_msg */

ds_mux_result_enum_type ds_mux_process_msc_msg_resp
(
  dsm_item_type**               msg_ptr_ptr,
  byte                          msg_len
)
{
  ds_mux_result_enum_type       result           = DS_MUX_SUCCESS;
  uint8                         dlci_id          = DS_MUX_DLCI_ID_0;
  ds_mux_dlci_param_type       *dlci_conn_param  = NULL;
  uint8                         msg_info[DS_MUX_DLCI_MSC_VAL_LEN];
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  do
  {
    /*-----------------------------------------------------------------------
      Process MSC message response and send outstanding message from queue
    -----------------------------------------------------------------------*/
    //Verify message length
    if( DS_MUX_DLCI_MSC_VAL_LEN != msg_len )
      DS_MUX_SET_ERROR_AND_BREAK(result)

    //Pullout DLCI ID
    if ( 1 != dsm_pullup_long ( msg_ptr_ptr, &dlci_id, 1) )
      DS_MUX_SET_ERROR_AND_BREAK(result)

    msg_len = msg_len - 1;

    //Pull out remaining bytes( DSM lib will free the packet once last byte pulled out)
    if ( msg_len != dsm_pullup_long ( msg_ptr_ptr, &msg_info, msg_len) )
      DS_MUX_SET_ERROR_AND_BREAK(result)

    //Remove EA and CR bit
    dlci_id = dlci_id >> 2;

    dlci_conn_param = ds_mux_get_dlci_conn_ptr(dlci_id);

    if ( NULL == dlci_conn_param )
      DS_MUX_SET_ERROR_AND_BREAK(result)

    //Send out standing message
    result = ds_mux_send_outstanding_msg( DS_MUX_DLCI_FRAME_VAL_MSC, dlci_conn_param);

    return result;
  }while(0);

  DS_MUX_LOG_MSG_ERROR_2("ds_mux_process_msc_msg_resp(): Error in processing "
                         "MSC resp %d dlci_id %d", msg_len, dlci_id);

  return result;
}/*ds_mux_process_msc_msg_resp */

ds_mux_result_enum_type ds_mux_process_msc_msg_cmd
(
  dsm_item_type**               msg_ptr_ptr,
  byte                          msg_len,
  ds_mux_io_frame_type*         msg_rsp_frame,
  boolean*                      send_msg_resp
)
{
  ds_mux_result_enum_type       result           = DS_MUX_SUCCESS;
  uint8                         msg_type         = DS_MUX_DLCI_FRAME_VAL_MSC;
  dsm_item_type*                resp_msg         = NULL;
  dsm_item_type**               tx_pkt           = NULL;
  uint8                         dlci_id          = 0;
  uint8                         v24_sig          = 0;
  uint8                         break_sig        = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  do
  {
    //Allocate command buffer for MSC response
    if ( NULL == (resp_msg = dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL)))
      DS_MUX_SET_ERROR_AND_BREAK(result)

    tx_pkt = &resp_msg;

    //Insert message type
    if ( 1 != dsm_pushdown_tail( tx_pkt, &msg_type, 1, DSM_DS_SMALL_ITEM_POOL))
      DS_MUX_SET_ERROR_AND_BREAK(result)
    //Insert message length
    if ( 1 != dsm_pushdown_tail( tx_pkt, &msg_len, 1, DSM_DS_SMALL_ITEM_POOL))
      DS_MUX_SET_ERROR_AND_BREAK(result)

    //Verify length
    if ( 2 != msg_len && 3 != msg_len)
     DS_MUX_SET_ERROR_AND_BREAK(result)
    //Pullout DLCI ID
    if ( 1 != dsm_pullup_long ( msg_ptr_ptr, &dlci_id, 1) )
       DS_MUX_SET_ERROR_AND_BREAK(result)
    //Insert DLCI ID
    if ( 1 != dsm_pushdown_tail( tx_pkt, &dlci_id, 1, DSM_DS_SMALL_ITEM_POOL))
      DS_MUX_SET_ERROR_AND_BREAK(result)

    //Remove Lower two bits from DLCI ID
    dlci_id = dlci_id >> 2;

    //Pull out V24 signal byte
    if ( 2 == msg_len)
    {
      if ( 1 != dsm_pullup_long ( msg_ptr_ptr, &v24_sig, 1) )
        DS_MUX_SET_ERROR_AND_BREAK(result)

      if ( 1 != dsm_pushdown_tail( tx_pkt, &v24_sig, 1, DSM_DS_SMALL_ITEM_POOL))
        DS_MUX_SET_ERROR_AND_BREAK(result)

    }
    else if ( 3 == msg_len )
    {
      if ( 1 != dsm_pullup_long ( msg_ptr_ptr, &v24_sig, 1) )
        DS_MUX_SET_ERROR_AND_BREAK(result)

      if ( 1 != dsm_pushdown_tail( tx_pkt, &v24_sig, 1, DSM_DS_SMALL_ITEM_POOL))
        DS_MUX_SET_ERROR_AND_BREAK(result)

      if ( 1 != dsm_pullup_long ( msg_ptr_ptr, &break_sig, 1) )
        DS_MUX_SET_ERROR_AND_BREAK(result)

      if ( 1 != dsm_pushdown_tail( tx_pkt, &break_sig, 1, DSM_DS_SMALL_ITEM_POOL))
        DS_MUX_SET_ERROR_AND_BREAK(result)
    }

    //Process DLCI signal byte
    ds_mux_logical_process_v24_signal( dlci_id, v24_sig);


  }while(0);

  DS_MUX_LOG_MSG_INFO2_4( " ds_mux_process_msc_msg_cmd dlci id %d v24 signal byte %d len %d result %d",
      dlci_id, v24_sig, msg_len, result);

  DS_MUX_LOG_STATE_INFO1_2( dlci_id, DS_MUX_LOGGING_MSC_RECV,\
    "Result %d Signal byte %d", result, v24_sig)

  if ( DS_MUX_FAILURE == result)
  {
    if ( NULL != tx_pkt )
    {
      if ( NULL != *tx_pkt )
      {
        dsm_free_packet(tx_pkt);
      }
    }
    *send_msg_resp = FALSE;
  }
  else
  {
     msg_rsp_frame->length = dsm_length_packet(*tx_pkt);
     msg_rsp_frame->information_ptr = *tx_pkt;
    *send_msg_resp = TRUE;
  }

  return result;
}/*ds_mux_process_msc_msg_cmd*/

ds_mux_result_enum_type ds_mux_process_psc_msg_resp
(
)
{
  ds_mux_result_enum_type       result           = DS_MUX_SUCCESS;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  do
  {
    //TBD::  modem initiated power save is not supported

  }while(0);

  return result;
}/* ds_mux_process_psc_msg_resp */


ds_mux_result_enum_type ds_mux_process_psc_msg_cmd
(
  dsm_item_type**               msg_ptr_ptr,
  byte                          msg_len,
  ds_mux_io_frame_type*         msg_rsp_frame,
  boolean*                      send_msg_resp
)
{
  ds_mux_result_enum_type       result           = DS_MUX_SUCCESS;
  uint8                         msg_type         = DS_MUX_DLCI_FRAME_VAL_PSC;
  dsm_item_type*                resp_msg         = NULL;
  dsm_item_type**               tx_pkt           = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  do
  {
    /*-----------------------------------------------------------------------
      Process PSC message command and prepare PSC message response.
      Note: PSC processing will be done after sending PSC response in process rx message
    -----------------------------------------------------------------------*/
    if ( NULL == (resp_msg = dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL)))
      DS_MUX_SET_ERROR_AND_BREAK(result)

    tx_pkt = &resp_msg;

    if ( 1 != dsm_pushdown_tail( tx_pkt, &msg_type, 1, DSM_DS_SMALL_ITEM_POOL))
      DS_MUX_SET_ERROR_AND_BREAK(result)

    if ( 1 != dsm_pushdown_tail( tx_pkt, &msg_len, 1, DSM_DS_SMALL_ITEM_POOL))
      DS_MUX_SET_ERROR_AND_BREAK(result)

  }while(0);

  if ( DS_MUX_FAILURE == result)
  {
    if ( NULL != tx_pkt )
    {
      if ( NULL != *tx_pkt )
      {
        dsm_free_packet(tx_pkt);
      }
    }
    *send_msg_resp = FALSE;
  }
  else
  {
     msg_rsp_frame->length = dsm_length_packet(*tx_pkt);
     msg_rsp_frame->information_ptr = *tx_pkt;
    *send_msg_resp = TRUE;
  }

  return result;
}/*ds_mux_process_psc_msg_cmd*/

ds_mux_result_enum_type ds_mux_process_cld_msg_resp
(
)
{
  ds_mux_result_enum_type       result           = DS_MUX_SUCCESS;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  do
  {
    //TBD::  modem initiated power save is not supported
  }while(0);


  return result;
}/* ds_mux_process_psc_msg_resp */

ds_mux_result_enum_type ds_mux_process_cld_msg_cmd
(
  dsm_item_type**               msg_ptr_ptr,
  byte                          msg_len,
  ds_mux_io_frame_type*         msg_rsp_frame,
  boolean*                      send_msg_resp
)
{
  ds_mux_result_enum_type       result           = DS_MUX_SUCCESS;
  uint8                         msg_type         = DS_MUX_DLCI_FRAME_VAL_CLD;
  dsm_item_type*                resp_msg         = NULL;
  dsm_item_type**               tx_pkt           = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
      Process CLD message command and prepare CLD message response.
      Note: CLD processing will be done after sending CLD response in process rx message
    -----------------------------------------------------------------------*/
  do
  {
    if ( NULL == (resp_msg = dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL)))
      DS_MUX_SET_ERROR_AND_BREAK(result)

    tx_pkt = &resp_msg;

    if ( 1 != dsm_pushdown_tail( tx_pkt, &msg_type, 1, DSM_DS_SMALL_ITEM_POOL))
      DS_MUX_SET_ERROR_AND_BREAK(result)

    if ( 1 != dsm_pushdown_tail( tx_pkt, &msg_len, 1, DSM_DS_SMALL_ITEM_POOL))
      DS_MUX_SET_ERROR_AND_BREAK(result)

  }while(0);

  if ( DS_MUX_FAILURE == result)
  {
    if ( NULL != tx_pkt )
    {
      if ( NULL != *tx_pkt )
      {
        dsm_free_packet(tx_pkt);
      }
    }
    *send_msg_resp = FALSE;
  }
  else
  {
     msg_rsp_frame->length = dsm_length_packet(*tx_pkt);
     msg_rsp_frame->information_ptr = *tx_pkt;
    *send_msg_resp = TRUE;
  }

  return result;
}/*ds_mux_process_cld_msg_cmd*/

ds_mux_result_enum_type ds_mux_process_test_msg_resp
(
)
{
  ds_mux_result_enum_type       result           = DS_MUX_SUCCESS;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  do
  {
    //TBD::  modem initiated power save is not supported
  }while(0);


  return result;
}/* ds_mux_process_test_msg_resp */
ds_mux_result_enum_type ds_mux_process_test_msg_cmd
(
  dsm_item_type**               msg_ptr_ptr,
  byte                          msg_len,
  ds_mux_io_frame_type*         msg_rsp_frame,
  boolean*                      send_msg_resp
)
{
  ds_mux_result_enum_type       result           = DS_MUX_SUCCESS;
  uint8                         msg_type         = DS_MUX_DLCI_FRAME_VAL_TEST;
  dsm_item_type*                resp_msg         = NULL;
  dsm_item_type**               tx_pkt           = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  do
  {
    if ( NULL != msg_ptr_ptr && NULL != *msg_ptr_ptr && 0 != dsm_length_packet(*msg_ptr_ptr))
    {
       tx_pkt = msg_ptr_ptr;
      //Reuse same packet as this contain values
       if ( 1 != dsm_pushdown_packed( tx_pkt, &msg_len, 1, DSM_DS_SMALL_ITEM_POOL))
        DS_MUX_SET_ERROR_AND_BREAK(result)

       if ( 1 != dsm_pushdown_tail( tx_pkt, &msg_type, 1, DSM_DS_SMALL_ITEM_POOL))
         DS_MUX_SET_ERROR_AND_BREAK(result)
    }
    else
    {
      //Create a response packet
      if ( NULL == (resp_msg = dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL)))
        DS_MUX_SET_ERROR_AND_BREAK(result)

      tx_pkt = &resp_msg;

      if ( 1 != dsm_pushdown_tail( tx_pkt, &msg_type, 1, DSM_DS_SMALL_ITEM_POOL))
        DS_MUX_SET_ERROR_AND_BREAK(result)

      if ( 1 != dsm_pushdown_tail( tx_pkt, &msg_len, 1, DSM_DS_SMALL_ITEM_POOL))
        DS_MUX_SET_ERROR_AND_BREAK(result)
    }
  }while(0);

  if ( DS_MUX_FAILURE == result)
  {
    if ( NULL != tx_pkt )
    {
      if ( NULL != *tx_pkt )
      {
        dsm_free_packet(tx_pkt);
      }
    }
    *send_msg_resp = FALSE;
  }
  else
  {
     msg_rsp_frame->length = dsm_length_packet(*tx_pkt);
     msg_rsp_frame->information_ptr = *tx_pkt;
    *send_msg_resp = TRUE;
  }

  return result;
}/*ds_mux_process_test_msg_cmd*/

ds_mux_result_enum_type ds_mux_process_fc_on_msg_resp
(
)
{
  ds_mux_result_enum_type       result           = DS_MUX_SUCCESS;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  do
  {
    //TBD::  modem flow control  is not supported
  }while(0);


  return result;
}/* ds_mux_process_fc_on_msg_resp */

ds_mux_result_enum_type ds_mux_process_fc_on_msg_cmd
(
  dsm_item_type**               msg_ptr_ptr,
  byte                          msg_len,
  ds_mux_io_frame_type*         msg_rsp_frame,
  boolean*                      send_msg_resp
)
{
  ds_mux_result_enum_type       result           = DS_MUX_SUCCESS;
  uint8                         msg_type         = DS_MUX_DLCI_FRAME_VAL_FCON;
  dsm_item_type*                resp_msg         = NULL;
  dsm_item_type**               tx_pkt           = NULL;
  uint8                         dlci_id          = DS_MUX_DLCI_ID_1;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
      Process FLow Control ON  command and prepare FCON message response.
      Enable flow on all DLCIs. 5.4.6.3.5	Flow Control On Command (FCon)
      The flow control command is used to handle the aggregate flow. When either entity
      is able to receive new information it transmits this command.
    -----------------------------------------------------------------------*/
  do
  {
    if ( NULL == (resp_msg = dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL)))
      DS_MUX_SET_ERROR_AND_BREAK(result)

    tx_pkt = &resp_msg;

    if ( 1 != dsm_pushdown_tail( tx_pkt, &msg_type, 1, DSM_DS_SMALL_ITEM_POOL))
      DS_MUX_SET_ERROR_AND_BREAK(result)

    if ( 1 != dsm_pushdown_tail( tx_pkt, &msg_len, 1, DSM_DS_SMALL_ITEM_POOL))
      DS_MUX_SET_ERROR_AND_BREAK(result)

    for(dlci_id = DS_MUX_DLCI_ID_1;  dlci_id <= DS_MUX_MAX_LOGICAL_CONNECTION; dlci_id++  )
    {
      ds_mux_enable_flow_mask( dlci_id, DS_MUX_FLOW_DISABLE_FCON );
    }

  }while(0);

  if ( DS_MUX_FAILURE == result)
  {
    if ( NULL != tx_pkt )
    {
      if ( NULL != *tx_pkt )
      {
        dsm_free_packet(tx_pkt);
      }
    }
    *send_msg_resp = FALSE;
  }
  else
  {
     msg_rsp_frame->length = dsm_length_packet(*tx_pkt);
     msg_rsp_frame->information_ptr = *tx_pkt;
    *send_msg_resp = TRUE;
  }

  return result;
}/*ds_mux_process_fc_on_msg_cmd*/

ds_mux_result_enum_type ds_mux_process_fc_off_msg_resp
(
)
{
  ds_mux_result_enum_type       result           = DS_MUX_SUCCESS;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  do
  {
    //TBD::  modem flow control  is not supported
  }while(0);


  return result;
}/* ds_mux_process_fc_off_msg_resp */

ds_mux_result_enum_type ds_mux_process_fc_off_msg_cmd
(
  dsm_item_type**               msg_ptr_ptr,
  byte                          msg_len,
  ds_mux_io_frame_type*         msg_rsp_frame,
  boolean*                      send_msg_resp
)
{
  ds_mux_result_enum_type       result           = DS_MUX_SUCCESS;
  uint8                         msg_type         = DS_MUX_DLCI_FRAME_VAL_FCOFF;
  dsm_item_type*                resp_msg         = NULL;
  dsm_item_type**               tx_pkt           = NULL;
    uint8                       dlci_id          = DS_MUX_DLCI_ID_1;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-----------------------------------------------------------------------
      Process FLow Control OFF  command and prepare FCOFF message response.
      Disable flow on all DLCIs. except DLCI 0 5.4.6.3.6	Flow Control Off Command (FCoff)
      The flow control command is used to handle the aggregate flow. When either entity is
      not able to receive information it transmits the FCoff command.
    -----------------------------------------------------------------------*/
  do
  {
    if ( NULL == (resp_msg = dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL)))
      DS_MUX_SET_ERROR_AND_BREAK(result)

    tx_pkt = &resp_msg;

    if ( 1 != dsm_pushdown_tail( tx_pkt, &msg_type, 1, DSM_DS_SMALL_ITEM_POOL))
      DS_MUX_SET_ERROR_AND_BREAK(result)

    if ( 1 != dsm_pushdown_tail( tx_pkt, &msg_len, 1, DSM_DS_SMALL_ITEM_POOL))
      DS_MUX_SET_ERROR_AND_BREAK(result)


    for(dlci_id = DS_MUX_DLCI_ID_1;  dlci_id <= DS_MUX_MAX_LOGICAL_CONNECTION; dlci_id++  )
    {
      ds_mux_disable_flow_mask( dlci_id, DS_MUX_FLOW_DISABLE_FCON );
    }

  }while(0);

  if ( DS_MUX_FAILURE == result)
  {
    if ( NULL != tx_pkt )
    {
      if ( NULL != *tx_pkt )
      {
        dsm_free_packet(tx_pkt);
      }
    }
    *send_msg_resp = FALSE;
  }
  else
  {
     msg_rsp_frame->length = dsm_length_packet(*tx_pkt);
     msg_rsp_frame->information_ptr = *tx_pkt;
    *send_msg_resp = TRUE;
  }

  return result;
}/*ds_mux_process_fc_off_msg_cmd*/

ds_mux_result_enum_type ds_mux_process_pn_msg_cmd
(
  dsm_item_type**               msg_ptr_ptr,
  byte                          msg_len,
  ds_mux_io_frame_type*         msg_rsp_frame,
  boolean*                      send_msg_resp
)
{
  ds_mux_result_enum_type       result           = DS_MUX_SUCCESS;
  dsm_item_type*                resp_msg         = NULL;
  dsm_item_type**               tx_pkt           = NULL;
  ds_mux_dlci_param_type        *dlci_conn_param = NULL;
  uint8                         msg_type         = DS_MUX_DLCI_FRAME_VAL_PN;
  uint8                         dlci_id          = 0;
  uint8                         oct2             = 0;
  uint8                         priority_oct3    = 0;
  uint8                         T1_oct4          = 0;
  uint8                         N1_oct5          = 0;
  uint8                         N1_oct6          = 0;
  uint16                        N1               = 0;
  uint8                         N2_oct7          = 0;
  uint8                         K_oct8           = 0;
  ds_mux_subset_enum_type       req_frame_type   = DS_MUX_SUBSET_MIN;
  ds_mux_cl_type_enum_type      req_cl_type      = DS_MUX_CL_TYPE_INVALID;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  do
  {
    if ( NULL == (resp_msg = dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL)))
      DS_MUX_SET_ERROR_AND_BREAK(result)

    tx_pkt = &resp_msg;
    //Insert Message type
    if ( 1 != dsm_pushdown_tail( tx_pkt, &msg_type, 1, DSM_DS_SMALL_ITEM_POOL))
      DS_MUX_SET_ERROR_AND_BREAK(result)
    //Insert length
    if ( 1 != dsm_pushdown_tail( tx_pkt, &msg_len, 1, DSM_DS_SMALL_ITEM_POOL))
      DS_MUX_SET_ERROR_AND_BREAK(result)

    /* Right shift one bit to remove EA*/
    msg_len = msg_len >> 1;
    //Validate length
    if ( DS_MUX_PN_MSG_LEN != msg_len)
      DS_MUX_SET_ERROR_AND_BREAK(result)

    //Parse DLCI
    if ( 1 != dsm_pullup_long ( msg_ptr_ptr, &dlci_id, 1) )
       DS_MUX_SET_ERROR_AND_BREAK(result)
    //Insert DLCI ID
    if ( 1 != dsm_pushdown_tail( tx_pkt, &dlci_id, 1, DSM_DS_SMALL_ITEM_POOL))
      DS_MUX_SET_ERROR_AND_BREAK(result)

    //Remove Highest two bits
    dlci_id = dlci_id & 0x3F;

    dlci_conn_param = ds_mux_get_alloc_conn_ptr(dlci_id);
    //If DLCI is already connected then we should ignore parameter  negotition
    if ( NULL == dlci_conn_param || dlci_conn_param->state != DS_MUX_DLCI_INITIALIZED)
      DS_MUX_SET_ERROR_AND_BREAK(result)


    //Frame type and Covergence layer type
    if ( 1 != dsm_pullup_long ( msg_ptr_ptr, &oct2, 1) )
      DS_MUX_SET_ERROR_AND_BREAK(result)
    if ( 1 != dsm_pushdown_tail( tx_pkt, &oct2, 1, DSM_DS_SMALL_ITEM_POOL))
      DS_MUX_SET_ERROR_AND_BREAK(result)

    if ( DS_MUX_FAILURE == ds_mux_get_msg_frame_type( (oct2&0x0F), &req_frame_type))
      DS_MUX_SET_ERROR_AND_BREAK(result)
    if ( DS_MUX_FAILURE == ds_mux_get_msg_cl_type( oct2>>4, &req_cl_type))
      DS_MUX_SET_ERROR_AND_BREAK(result)

    //Priority
    if ( 1 != dsm_pullup_long ( msg_ptr_ptr, &priority_oct3, 1) )
      DS_MUX_SET_ERROR_AND_BREAK(result)
    if ( 1 != dsm_pushdown_tail( tx_pkt, &priority_oct3, 1, DSM_DS_SMALL_ITEM_POOL))
      DS_MUX_SET_ERROR_AND_BREAK(result)

    //Acknowledgement timer
    if ( 1 != dsm_pullup_long ( msg_ptr_ptr, &T1_oct4, 1) )
      DS_MUX_SET_ERROR_AND_BREAK(result)
    if ( 1 != dsm_pushdown_tail( tx_pkt, &T1_oct4, 1, DSM_DS_SMALL_ITEM_POOL))
      DS_MUX_SET_ERROR_AND_BREAK(result)

    //maximum frame size
    if ( 1 != dsm_pullup_long ( msg_ptr_ptr, &N1_oct5, 1) )
      DS_MUX_SET_ERROR_AND_BREAK(result)
    if ( 1 != dsm_pushdown_tail( tx_pkt, &N1_oct5, 1, DSM_DS_SMALL_ITEM_POOL))
      DS_MUX_SET_ERROR_AND_BREAK(result)

    if ( 1 != dsm_pullup_long ( msg_ptr_ptr, &N1_oct6, 1) )
      DS_MUX_SET_ERROR_AND_BREAK(result)
    if ( 1 != dsm_pushdown_tail( tx_pkt, &N1_oct6, 1, DSM_DS_SMALL_ITEM_POOL))
      DS_MUX_SET_ERROR_AND_BREAK(result)

    N1 = N1_oct6 << 8 | N1_oct5;

    //maximum number of retransmissions
    if ( 1 != dsm_pullup_long ( msg_ptr_ptr, &N2_oct7, 1) )
      DS_MUX_SET_ERROR_AND_BREAK(result)
    if ( 1 != dsm_pushdown_tail( tx_pkt, &N2_oct7, 1, DSM_DS_SMALL_ITEM_POOL))
      DS_MUX_SET_ERROR_AND_BREAK(result)

    //Window size
    if ( 1 != dsm_pullup_long ( msg_ptr_ptr, &K_oct8, 1) )
      DS_MUX_SET_ERROR_AND_BREAK(result)
    if ( 1 != dsm_pushdown_tail( tx_pkt, &K_oct8, 1, DSM_DS_SMALL_ITEM_POOL))
      DS_MUX_SET_ERROR_AND_BREAK(result)

    ds_mux_update_dlci_params ( dlci_conn_param, req_frame_type, req_cl_type, priority_oct3,
      T1_oct4, N1, N2_oct7, K_oct8);

  }while(0);

  DS_MUX_LOG_MSG_INFO2_2( " ds_mux_process_pn_msg_cmd result %d dlci id %d ",
    result, dlci_id);
  DS_MUX_LOG_STATE_INFO1_1( dlci_id, DS_MUX_LOGGING_PN_RECVD,\
    "Result %d ", result )

  if ( DS_MUX_FAILURE == result)
  {
    if (NULL != tx_pkt && NULL != *tx_pkt )
    {
      dsm_free_packet(tx_pkt);
    }
    *send_msg_resp = FALSE;
  }
  else
  {
     msg_rsp_frame->length = dsm_length_packet(*tx_pkt);
     msg_rsp_frame->information_ptr = *tx_pkt;
    *send_msg_resp = TRUE;
  }

  return result;

}/*ds_mux_process_pn_msg_cmd*/

ds_mux_result_enum_type ds_mux_process_pn_msg_resp
(
  dsm_item_type**               msg_ptr_ptr,
  byte                          msg_len
)
{
  ds_mux_result_enum_type       result           = DS_MUX_SUCCESS;
  uint8                         dlci_id          = 0;
  ds_mux_dlci_param_type       *dlci_conn_param  = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  msg_len = msg_len >> 1;
  do
  {
    if ( DS_MUX_PN_MSG_LEN != msg_len)
      DS_MUX_SET_ERROR_AND_BREAK(result)

    if ( 1 != dsm_pullup_long ( msg_ptr_ptr, &dlci_id, 1) )
       DS_MUX_SET_ERROR_AND_BREAK(result)

     //Reduce the count as DLCI has taken out
     msg_len = msg_len - 1;

    /*-----------------------------------------------------------------------
      Note: We are not taking care of re-negotiation and Expecting host has accpeted the requested parameters
      Take out remining params
     -----------------------------------------------------------------------*/
    if ( msg_len != dsm_pullup_long ( msg_ptr_ptr, &dlci_id, msg_len) )
      DS_MUX_SET_ERROR_AND_BREAK(result)

    //Remove Highest two bits
    dlci_id = dlci_id & 0x3F;

    dlci_conn_param = ds_mux_get_alloc_conn_ptr(dlci_id);

    if ( NULL == dlci_conn_param )
      DS_MUX_SET_ERROR_AND_BREAK(result)

    /*-----------------------------------------------------------------------
      Verify if DS MUX has recevied same msg back and send if there is any outstandig msg in the queue 0
     -----------------------------------------------------------------------*/
    result = ds_mux_send_outstanding_msg( DS_MUX_DLCI_FRAME_VAL_PN, dlci_conn_param);

  }while(0);

  return result;
}/*ds_mux_process_pn_msg_resp*/

ds_mux_result_enum_type ds_mux_process_rx_msgs_frames
(
  ds_mux_io_frame_type    *frame
)
{

  ds_mux_result_enum_type       result           = DS_MUX_SUCCESS;
  dsm_item_type**               msg_ptr_ptr      = NULL;
  boolean                       processing_msg   = TRUE;
  byte                          msg_byte         = 0;
  byte                          msg_len_with_ea  = 0;
  ds_mux_msg_type_enum_type     msg_type         = DS_MUX_MSG_TYPE_VAL_INVALID;
  boolean                       is_command       = FALSE;
  ds_mux_io_frame_type          msg_rsp_frame;
  boolean                       send_msg_resp   = FALSE;
  uint8                         dlci_id;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
     DLCI 0 is control channel, UI and UIH frames are used for sending messages over DLCI 0
   -----------------------------------------------------------------------*/
  if ( (NULL == frame )||
       (DS_MUX_DLCI_ID_0 != frame->dlci) ||
       (NULL == frame->information_ptr ))
  {
    return DS_MUX_FAILURE;
  }

  /*-----------------------------------------------------------------------
    Initialize message response frame. info field will be updated as part of message processing
   -----------------------------------------------------------------------*/
  ds_mux_prepare_msg_response( frame, &msg_rsp_frame);

  msg_ptr_ptr = &frame->information_ptr ;

  /*-----------------------------------------------------------------------
      Loop till no data left in DSM item or processing aborted due to error or processing is complete
   -----------------------------------------------------------------------*/
  while( (TRUE == processing_msg)&& ( DS_MUX_SUCCESS == result) && ( NULL != *msg_ptr_ptr) )
  {
    /*-----------------------------------------------------------------------
       1  Pull out first byte and convert message byte into DS MUX message type
          1.1 Check if this is command message or response message
       2. Pull out the length byte (All supported messgae has length 1 byte only hence not supporting extnd lenght)
       3. Info byte can be optional if length byte is 0
    -----------------------------------------------------------------------*/
    if ( 1 != dsm_pullup_long ( msg_ptr_ptr, &msg_byte, 1) ||
         DS_MUX_FAILURE == ds_mux_get_msg_type( msg_byte, &msg_type) )
    {
     result = DS_MUX_FAILURE;
     processing_msg = FALSE;
     break;
    }

    is_command = ( DS_MUX_MSG_RESP != (msg_byte & DS_MUX_DLCI_MSG_TYPE_CR_MASK)) ? TRUE : FALSE;

    if(1 != dsm_pullup_long ( msg_ptr_ptr, &msg_len_with_ea, 1))
    {
      result = DS_MUX_FAILURE;
      processing_msg = FALSE;
      break;
    }

    DS_MUX_LOG_MSG_INFO2_3( " ds_mux_process_rx_msgs_frames is_command %d msg_type %d len (with EA) %d",
      is_command, msg_type, msg_len_with_ea);

    /*-----------------------------------------------------------------------
    Message processing can be done in three ways
    1. Message can be processed Synchronously. Generate the response messages and
       send them at end of this function call
    2. Message will be processed Asynchronously(later).Generate the response messages and
       send them at end of this function call to the host.
    3. Message processing and response message generation will happen Asynchronously(later).
    -----------------------------------------------------------------------*/
    switch ( msg_type)
    {
      case DS_MUX_MSG_TYPE_VAL_PN:
      {
        if ( DS_MUX_MSG_CMD == is_command )
        {
          result = ds_mux_process_pn_msg_cmd(  msg_ptr_ptr, msg_len_with_ea, &msg_rsp_frame, &send_msg_resp);
        }
        else
        {
          result = ds_mux_process_pn_msg_resp( msg_ptr_ptr, msg_len_with_ea );
          send_msg_resp = FALSE;
        }
      }
      break;
      case DS_MUX_MSG_TYPE_VAL_PSC:
      {
        if ( DS_MUX_MSG_CMD == is_command )
        {
          result = ds_mux_process_psc_msg_cmd(  msg_ptr_ptr, msg_len_with_ea, &msg_rsp_frame, &send_msg_resp);
        }
        else
        {
          result = ds_mux_process_psc_msg_resp( );
          send_msg_resp = FALSE;
        }
      }
      break;
      case DS_MUX_MSG_TYPE_VAL_CLD:
      {
        if ( DS_MUX_MSG_CMD == is_command )
        {
          result = ds_mux_process_cld_msg_cmd(  msg_ptr_ptr, msg_len_with_ea, &msg_rsp_frame, &send_msg_resp);
        }
        else
        {
          result = ds_mux_process_cld_msg_resp();
          send_msg_resp = FALSE;
        }
      }
      break;
      case DS_MUX_MSG_TYPE_VAL_TEST:
      {
        if ( DS_MUX_MSG_CMD == is_command )
        {
          result = ds_mux_process_test_msg_cmd(  msg_ptr_ptr, msg_len_with_ea, &msg_rsp_frame, &send_msg_resp);
        }
        else
        {
        result = ds_mux_process_test_msg_resp();
          send_msg_resp = FALSE;
        }
      }
      break;
      case DS_MUX_MSG_TYPE_VAL_FCON:
      {
        if ( DS_MUX_MSG_CMD == is_command )
        {
          result = ds_mux_process_fc_on_msg_cmd( msg_ptr_ptr, msg_len_with_ea, &msg_rsp_frame, &send_msg_resp);
        }
        else
        {
          result = ds_mux_process_fc_on_msg_resp();
          send_msg_resp = FALSE;
        }
      }
      break;

      case DS_MUX_MSG_TYPE_VAL_FCOFF:
      {
        if ( DS_MUX_MSG_CMD == is_command )
        {
          result = ds_mux_process_fc_off_msg_cmd(  msg_ptr_ptr, msg_len_with_ea, &msg_rsp_frame, &send_msg_resp);
        }
        else
        {
           result = ds_mux_process_fc_off_msg_resp();
          send_msg_resp = FALSE;
        }
      }
      break;

      case DS_MUX_MSG_TYPE_VAL_MSC:
      {
        if ( DS_MUX_MSG_CMD == is_command )
        {
          result = ds_mux_process_msc_msg_cmd(  msg_ptr_ptr, msg_len_with_ea, &msg_rsp_frame, &send_msg_resp);
        }
        else
        {
          result = ds_mux_process_msc_msg_resp( msg_ptr_ptr, msg_len_with_ea );
          send_msg_resp = FALSE;
        }
      }
      break;
      //Unsupported messages
      default:
      {
        result = DS_MUX_FAILURE;
        processing_msg = FALSE;
        break;
      }
    }

    DS_MUX_LOG_MSG_INFO2_1( " ds_mux_process_rx_msgs_frames send msg resp %d ", send_msg_resp);

    if ( TRUE == send_msg_resp)
    {
      /*-----------------------------------------------------------------------
        In case of failure, Free the DSM item. Success case: SIO will free the DSM item
      -----------------------------------------------------------------------*/
      if ( DS_MUX_SUCCESS != ds_mux_io_transmit_msg_response ( &msg_rsp_frame ))
      {
        dsm_free_packet(&msg_rsp_frame.information_ptr);
      }
      else
      {
        /*-----------------------------------------------------------------------
          Flow controlling all DLCI including DLCI 0 (Control channel)  Hance disabling flow after sending msg resp
        -----------------------------------------------------------------------*/
        if ( DS_MUX_MSG_TYPE_VAL_PSC == msg_type && TRUE == is_command)
        {

          for(dlci_id = DS_MUX_DLCI_ID_0;  dlci_id <= DS_MUX_MAX_LOGICAL_CONNECTION; dlci_id++  )
          {
            ds_mux_disable_flow_mask( dlci_id, DS_MUX_FLOW_DISABLE_PSC );
            /*-----------------------------------------------------------------------
              Whenever this mask is set we should preform wakeup procedure once wake up then clear the mask
              -----------------------------------------------------------------------*/
          }

          ds_mux_set_mux_state( DS_MUX_STATE_IN_POWER_SAVE );
        }

        if( DS_MUX_MSG_TYPE_VAL_CLD == msg_type && TRUE == is_command )
        {
          ds_mux_close_down_mux();
          processing_msg = FALSE;
          result         = DS_MUX_CLD_PROCESSED;
        }

        /* For Test command, stop the processing after sending the response */
        if((DS_MUX_MSG_TYPE_VAL_TEST == msg_type) && (TRUE == is_command))
        {
            DS_MUX_LOG_MSG_INFO2_0( "ds_mux_process_rx_msgs_frames test command processing is done \n");
            processing_msg = FALSE;
        }
      }
      //Reset Message field
      msg_rsp_frame.information_ptr = NULL;
      msg_rsp_frame.length          = 0;
      send_msg_resp = FALSE;
    }

  }

  /*-----------------------------------------------------------------------
     Free the DSM item if messages parsing failed
   -----------------------------------------------------------------------*/
  if ( DS_MUX_FAILURE == result )
  {
    dsm_free_packet(msg_ptr_ptr);
  }

  return result;
}/* ds_mux_process_rx_msgs_frames*/

boolean ds_mux_is_dlci_0_connected
(

)
{
  boolean                 result          = TRUE;
  ds_mux_dlci_param_type *dlci_conn_param = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  dlci_conn_param = ds_mux_get_alloc_conn_ptr(DS_MUX_DLCI_ID_0);

  if ( NULL == dlci_conn_param ||
       DS_MUX_DLCI_CONNECTED != dlci_conn_param->state )
  {
    result = FALSE;
  }

  return result;
}/* ds_mux_is_dlci_0_connected */

ds_mux_result_enum_type ds_mux_process_sabm_cmd_frames
(
  ds_mux_io_frame_type    *frame
)
{

  ds_mux_result_enum_type       result           = DS_MUX_SUCCESS;
  ds_mux_dlci_param_type        *dlci_conn_param = NULL;
  ds_mux_io_frame_type          ua_rsp_frame;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  do
  {
    DS_MUX_LOG_STATE_INFO1_0( frame->dlci, DS_MUX_LOGGING_SABM_REQ_RECV)
    //Host should set poll bit to 1
    if (FALSE == frame->poll_final )
    {
      result = DS_MUX_FAILURE;
      break;
    }
    //Get DLCI connection ptr
    dlci_conn_param = ds_mux_get_alloc_conn_ptr(frame->dlci);

    if ( NULL == dlci_conn_param)
    {
      result = DS_MUX_FAILURE;
      break ;
    }

    //If DLCI is already connected no Action will be taken
    if (DS_MUX_DLCI_CONNECTED == dlci_conn_param->state )
    {
      result = DS_MUX_SUCCESS;
      break ;
    }

    if ( DS_MUX_DLCI_ID_0 == frame->dlci)
    {
       ds_mux_set_is_initiator(FALSE);
    }
    else
    {
      //DLCI0 should be connected before any new logical connection
      if(FALSE == ds_mux_is_dlci_0_connected() )
      {
        DS_MUX_LOG_MSG_ERROR_1("Received SABM for DLCI %d before SABM for DLCI 0",
                               frame->dlci);
        result = DS_MUX_FAILURE;
        break ;
      }
    }
    //Set state to connected
    dlci_conn_param->state = DS_MUX_DLCI_CONNECTED;
    //Notify Logical interface about connect
    if ( 0 != frame->dlci )
	{
      ds_mux_logical_notify_conneted_dlci( frame->dlci );
	}
    //Prepare UA response for SABM command
    ds_mux_prepare_ua_response(frame , &ua_rsp_frame);
    //Send UA to host
    ds_mux_io_transmit_cmd_response(&ua_rsp_frame);
  }while(0);

  DS_MUX_LOG_MSG_INFO2_2( " ds_mux_process_sabm_cmd_frames result %d dlci id %d ",
    result, frame->dlci);
  DS_MUX_LOG_STATE_INFO1_1( frame->dlci, DS_MUX_LOGGING_SABM_CONNECTED,"Result %d", result)

  return result;
}/* ds_mux_process_sabm_cmd_frames*/


void ds_mux_prepare_ua_response
(
  ds_mux_io_frame_type    *input_frame,
  ds_mux_io_frame_type    *output_frame
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_mux_util_prepare_cmd_frame( input_frame->dlci, DS_MUX_FRAME_TYPE_UTIL_UA,
    FALSE, input_frame->poll_final, output_frame );

  return ;
}/* ds_mux_prepare_ua_response */

void ds_mux_prepare_dm_response
(
  ds_mux_io_frame_type    *input_frame,
  ds_mux_io_frame_type    *output_frame
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_mux_util_prepare_cmd_frame( input_frame->dlci, DS_MUX_FRAME_TYPE_UTIL_DM,
    FALSE, input_frame->poll_final, output_frame);

  return ;
}/* ds_mux_prepare_ua_response */

void ds_mux_ua_recevied
(
  ds_mux_io_frame_struct_type              *curr_cmd_frame
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return ;
}/* ds_mux_ua_recevied */

void ds_mux_send_outstanding_cmd
(
  q_type                    *outstanding_cmd_frame_q
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

}/* ds_mux_send_outstanding_msg*/

ds_mux_result_enum_type ds_mux_process_ua_cmd_frames
(
  ds_mux_io_frame_type    *frame
)
{

  ds_mux_result_enum_type       result = DS_MUX_SUCCESS;
  ds_mux_dlci_param_type        *dlci_conn_param = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    1  Fetch DLCI connection info from DLCI ID
    2. Validate final bit (Final Bit should set to 1)
    3. Check Current pending command and complete the required action
    4. Look for pending command
    5  Send outstanding command (If there is any)
  -----------------------------------------------------------------------*/
  do
  {
    dlci_conn_param = ds_mux_get_dlci_conn_ptr(frame->dlci);

    if ( NULL == dlci_conn_param)
    {
      result = DS_MUX_FAILURE;
      break ;
    }

    if (0 == frame->poll_final)
    {
      result = DS_MUX_FAILURE;
      break ;
    }

    if(  NULL != dlci_conn_param->curr_cmd_frame)
    {
      ds_mux_ua_recevied( dlci_conn_param->curr_cmd_frame );

      DS_MUX_MEM_FREE( dlci_conn_param->curr_cmd_frame );

      dlci_conn_param->curr_cmd_frame = NULL;
    }

    ds_mux_send_outstanding_cmd( &dlci_conn_param->outstanding_cmd_frame_q );

  }while(0);

  return result;
}/* ds_mux_process_ua_cmd_frames*/

void ds_mux_disconnect_dlci
(
  ds_mux_dlci_param_type  *dlci_conn
)
{
  ds_mux_sig_enum_type  sig;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_MUX_LOG_MSG_INFO2_1( " ds_mux_disconnect_dlci dlci id %d ", dlci_conn->dlci_id);

  /*-----------------------------------------------------------------------
    1  Notify logical interface about DLCI disconnect so that logical interface can do cleanup
    2. Clear Signal assosiated with this DLCI.(Other then 0; there is no Tx Signal for DLCI 0)
    3. De-Initilize DLCI connection (Free outstanding msg and command)
    4. Free DLCI connection pointer
  -----------------------------------------------------------------------*/
  /*Step 1*/
  (void)ds_mux_logical_notify_disconneted_dlci( dlci_conn->dlci_id);

  /*Step 2*/
  if ( DS_MUX_DLCI_ID_0 != dlci_conn->dlci_id)
  {
    if ( DS_MUX_MAX_SIGNALS == (sig = ds_mux_get_sig_by_id( dlci_conn->dlci_id )))
    {
      return ;
    }

    DS_MUX_CLR_SIGNAL(sig);

    ds_mux_disable_sig( sig );
  }
  /*Step 3*/
  ds_mux_de_inititialize_dlci_conn( dlci_conn );
  /*Step 4*/
  DS_MUX_MEM_FREE( dlci_conn );

  return ;
}/* ds_mux_disconnect_dlci */

ds_mux_result_enum_type ds_mux_process_disc_cmd_frames
(
  ds_mux_io_frame_type    *frame
)
{

  ds_mux_result_enum_type        result = DS_MUX_SUCCESS;
  ds_mux_dlci_param_type        *dlci_conn_param = NULL;
  ds_mux_io_frame_type           rsp_frame;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    1  IF connection is not valid
      1.1. Send DM message to host
    2 IF connection is valid
      2.1 Prepare and Send UA to host
    . 2.2 Disconnect the DLCI if Connection is valid
  -----------------------------------------------------------------------*/
  do
  {
    dlci_conn_param = ds_mux_get_dlci_conn_ptr(frame->dlci);

    if ( NULL == dlci_conn_param)
    {
      ds_mux_prepare_dm_response(frame , &rsp_frame);
      ds_mux_io_transmit_cmd_response(&rsp_frame);
      DS_MUX_LOG_MSG_INFO2_1( " ds_mux_process_disc_cmd_frames Sending DM dlci id %d ", frame->dlci);

    }
    else
    {
      ds_mux_prepare_ua_response(frame , &rsp_frame);
      ds_mux_io_transmit_cmd_response(&rsp_frame);
      
      DS_MUX_LOG_MSG_INFO2_1( " ds_mux_process_disc_cmd_frames Sending UA dlci id %d ", frame->dlci);
      ds_mux_disconnect_dlci( dlci_conn_param );

      if ( DS_MUX_DLCI_ID_0 == frame->dlci )
      {
        DS_MUX_LOG_MSG_INFO2_1( " ds_mux_process_disc_cmd_frames: Received DISC "
                                "for DLCI%d, treating it as CLD command ", frame->dlci);
        ds_mux_close_down_mux();
        result         = DS_MUX_CLD_PROCESSED;
      }
    }
  }while(0);
  return result;
}/* ds_mux_process_disc_cmd_frames*/

ds_mux_result_enum_type ds_mux_process_dm_cmd_frames
(
  ds_mux_io_frame_type    *frame
)
{
  ds_mux_result_enum_type       result = DS_MUX_SUCCESS;
  ds_mux_dlci_param_type        *dlci_conn_param = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    1. Get DLCI connection pointer
    2. Disconnect DLCI(Free up resouces)
  -----------------------------------------------------------------------*/
  do
  {
    dlci_conn_param = ds_mux_get_dlci_conn_ptr(frame->dlci);

    if ( NULL == dlci_conn_param)
    {
      result = DS_MUX_FAILURE;
      break ;
    }
    ds_mux_disconnect_dlci( dlci_conn_param );

    DS_MUX_LOG_MSG_INFO2_1( " ds_mux_process_dm_cmd_frames dlci id %d ", frame->dlci);

  }while(0);
  return result;
}/* ds_mux_process_dm_cmd_frames*/


ds_mux_result_enum_type ds_mux_process_dlci_rx_ctl_frames
(
  ds_mux_io_frame_type    *frame
)
{

  ds_mux_result_enum_type       result = DS_MUX_SUCCESS;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  //DLCI Messages are sent as UI or UIH frames
  switch ( frame->frame_type )
  {
    case DS_MUX_FRAME_TYPE_UTIL_UIH:
    case DS_MUX_FRAME_TYPE_UTIL_UI:
    {
      result = ds_mux_process_rx_msgs_frames( frame );
    }
    break;
    case DS_MUX_FRAME_TYPE_UTIL_SABM:
    {
      result = ds_mux_process_sabm_cmd_frames( frame );
    }
	break;
    case DS_MUX_FRAME_TYPE_UTIL_UA:
    {
      result = ds_mux_process_ua_cmd_frames( frame );
    }
	break;
    case DS_MUX_FRAME_TYPE_UTIL_DM:
    {
      result = ds_mux_process_dm_cmd_frames( frame );
    }
	break;
    case DS_MUX_FRAME_TYPE_UTIL_DISC:
    {
      result = ds_mux_process_disc_cmd_frames( frame );
    }
    break;
    case DS_MUX_FRAME_TYPE_INVALID_FRAME:
    default:
    {
      result = DS_MUX_FAILURE;
    }
  }

  return result;
}/* ds_mux_process_dlci_rx_ctl_frames*/


ds_mux_cl_type_enum_type ds_mux_get_cl_type
(
  uint8                       dlci
)
{

  ds_mux_cl_type_enum_type       cl_type = DS_MUX_CL_TYPE_2;
  ds_mux_dlci_param_type        *dlci_conn_param = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  dlci_conn_param = ds_mux_get_dlci_conn_ptr(dlci);

  if (  NULL != dlci_conn_param)
  {
    cl_type = dlci_conn_param->cl_type;
  }

  return cl_type;
}/* ds_mux_get_cl_type*/

ds_mux_result_enum_type ds_mux_process_rx_frag_data_frames
(
  ds_mux_io_frame_type    *frame
)
{

  ds_mux_result_enum_type       result = DS_MUX_SUCCESS;
  uint8                         cl4_ctl_header = (uint8)CL_FOUR_CTL_HDR_ENTIRE_FRAME;
  ds_mux_dlci_param_type       *dlci_conn_param = NULL;
  dsm_item_type               **pkt_head_ptr;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( NULL == frame ||
       NULL == frame->information_ptr ||
       0 == dsm_length_packet(frame->information_ptr) )
  {
     DS_MUX_LOG_MSG_ERROR_1("ds_mux_process_rx_frag_data_frames(): Invalid pkt, frame %x ", frame);

   if( NULL != frame)
   {
     if ( NULL != frame->information_ptr)
     {
       dsm_free_packet(& frame->information_ptr);
     }
   }
     return DS_MUX_FAILURE;
  }

  pkt_head_ptr = &frame->information_ptr;
  dlci_conn_param = ds_mux_get_alloc_conn_ptr(frame->dlci);

  if ( NULL == dlci_conn_param)
  {
     DS_MUX_LOG_MSG_ERROR_0("ds_mux_process_rx_frag_data_frames(): Invalid conn ptr ");
    dsm_free_packet(pkt_head_ptr);
    return  DS_MUX_FAILURE;
  }

  if ( 1 == dsm_pullup_long ( pkt_head_ptr, &cl4_ctl_header, 1) )
  {

    switch ( cl4_ctl_header )
    {
      case CL_FOUR_CTL_HDR_ENTIRE_FRAME:
      {
        /*-----------------------------------------------------------------------
          Free old pending DSM items
        -----------------------------------------------------------------------*/
        if ( NULL != dlci_conn_param->pending_rx_cl4_data_ptr )
        {
          dsm_free_packet(&dlci_conn_param->pending_rx_cl4_data_ptr);
          dlci_conn_param->pending_rx_cl4_data_ptr = NULL;
        }
        result = ds_mux_logical_input_rx_data( frame->dlci, *pkt_head_ptr);

      }
      break;

      case CL_FOUR_CTL_HDR_FIRST_FRAME:
      {
        /*-----------------------------------------------------------------------
          Free old pending DSM items and build up new chain
          -----------------------------------------------------------------------*/
        if ( NULL != dlci_conn_param->pending_rx_cl4_data_ptr )
        {
          DS_MUX_LOG_MSG_ERROR_0("ds_mux_process_rx_frag_data_frames(): dropping dsm ptr ");

          dsm_free_packet(&dlci_conn_param->pending_rx_cl4_data_ptr);
          dlci_conn_param->pending_rx_cl4_data_ptr = NULL;
        }
        //Update the first frame
        dlci_conn_param->pending_rx_cl4_data_ptr = *pkt_head_ptr;

      }
      break;

      case CL_FOUR_CTL_HDR_MIDDLE_FRAME:
      {
        /*-----------------------------------------------------------------------
          If there is no previous frame then discard middle frame otherwise append the middle frame
          -----------------------------------------------------------------------*/
        if ( NULL == dlci_conn_param->pending_rx_cl4_data_ptr )
        {
          DS_MUX_LOG_MSG_ERROR_0("ds_mux_process_rx_frag_data_frames(): dropping dsm ptr ");
          dsm_free_packet(pkt_head_ptr);
          break;
        }

        dsm_append( &dlci_conn_param->pending_rx_cl4_data_ptr, pkt_head_ptr);

      }
      break;
      case CL_FOUR_CTL_HDR_LAST_FRAME:
      {
        /*-----------------------------------------------------------------------
          If there is no previous frame then discard last frame
        -----------------------------------------------------------------------*/
        if ( NULL == dlci_conn_param->pending_rx_cl4_data_ptr )
        {
          DS_MUX_LOG_MSG_ERROR_0("ds_mux_process_rx_frag_data_frames(): dropping dsm ptr ");
          dsm_free_packet(pkt_head_ptr);
          break;
        }

        dsm_append( &dlci_conn_param->pending_rx_cl4_data_ptr, pkt_head_ptr);

        result = ds_mux_logical_input_rx_data( frame->dlci, dlci_conn_param->pending_rx_cl4_data_ptr);

        dlci_conn_param->pending_rx_cl4_data_ptr = NULL;
      }
      break;
      default:
      {
        DS_MUX_LOG_MSG_ERROR_0("ds_mux_process_rx_frag_data_frames(): dropping dsm ptr ");
        result = DS_MUX_FAILURE;
        dsm_free_packet(pkt_head_ptr);
        break;
      }
    }
  }
  else
  {
    DS_MUX_LOG_MSG_ERROR_0("ds_mux_process_rx_frag_data_frames(): dropping dsm ptr ");
    result = DS_MUX_FAILURE;
    dsm_free_packet(pkt_head_ptr);
  }

  return result;
}/* ds_mux_process_rx_frag_data_frames*/

void  ds_mux_disable_flow_mask
(
    uint8                        dlci_id,
  ds_mux_flow_ctl_mask_enum_type fc_msk
)
{
  ds_mux_dlci_param_type        *dlci_conn_param = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  dlci_conn_param = ds_mux_get_dlci_conn_ptr(dlci_id);

  if ( NULL == dlci_conn_param )
  {
    return;
  }

  DS_MUX_LOG_MSG_INFO2_3( "ds_mux_disable_flow_mask dlci_id %d fc_msk  %x current flow mask %x",
      dlci_id, fc_msk, dlci_conn_param->flow_control_mask);

  DS_MUX_ENTER_CRIT_SECTION(&global_ds_mux_crit_section);

  dlci_conn_param->flow_control_mask  |= fc_msk;

  DS_MUX_LEAVE_CRIT_SECTION(&global_ds_mux_crit_section);

}/* ds_mux_disable_flow_mask */

void ds_mux_dlci_0_flow_enabled
(
  ds_mux_dlci_param_type        *dlci_0_conn_param
)
{
  uint8                                 dlci_id         = DS_MUX_DLCI_ID_1;
  ds_mux_dlci_param_type               *dlci_conn_param = NULL;
  ds_mux_io_frame_struct_type          *msg_ptr         = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_MUX_LOG_MSG_INFO2_0( "ds_mux_dlci_0_flow_enabled ");


  /*-----------------------------------------------------------------------
    DLCI 0 can be flow controlled only for PSC/LPM/ or DTR LOW
    In above cases we should not we getting any command and msg from host
    If there is pending UA or Resp msg then we can drop them as host suppose to send
    cmd and msg again once timers expire.
    We should start transmitting any pending command messages
  -----------------------------------------------------------------------*/
  for(dlci_id = DS_MUX_DLCI_ID_1;  dlci_id <= DS_MUX_MAX_LOGICAL_CONNECTION; dlci_id++  )
  {

    dlci_conn_param = ds_mux_get_dlci_conn_ptr(dlci_id);


    if((NULL != dlci_conn_param) && (NULL != dlci_conn_param->curr_msg_frame))
    {
      ds_mux_io_transmit_msg_cmd( dlci_conn_param->curr_msg_frame );
      // Start T2 timer and increment the count were it was left
      if ( DS_MUX_TIMER_INVALID_HANDLE != dlci_conn_param->t2_timer_handle )
      {
         ds_mux_timer_start( dlci_conn_param->t2_timer_handle,
                             &dlci_conn_param->t2_timer_attr,
                             ( dlci_conn_param->response_timer_T2 * 10 ) );
        dlci_conn_param->curr_transmissions_N2++;
      }
    }
    else
    {
      if(NULL != dlci_conn_param)
      {
        if ( (msg_ptr = (ds_mux_io_frame_struct_type *) q_get( &dlci_conn_param->outstanding_msg_frame_q )) != NULL )
        {
          ds_mux_io_transmit_msg_cmd( msg_ptr );
          dlci_conn_param->curr_msg_frame = msg_ptr;
          //Now start T2 timer
          if ( DS_MUX_TIMER_INVALID_HANDLE != dlci_conn_param->t2_timer_handle )
          {
             ds_mux_timer_start( dlci_conn_param->t2_timer_handle,
                                 &dlci_conn_param->t2_timer_attr,
                                 ( dlci_conn_param->response_timer_T2 * 10 ));
            dlci_conn_param->curr_transmissions_N2 = 1;
          }
        }
      }
    }
  }
}/* ds_mux_dlci_0_flow_enabled */

void ds_mux_dlci_n_flow_enabled
(
  ds_mux_dlci_param_type        *dlci_conn_param
)
{
  dsm_watermark_type                 *logical_tx_wmk_ptr     = NULL;
  ds_mux_sig_enum_type                sig;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_MUX_LOG_MSG_INFO2_1( "ds_mux_dlci_n_flow_enabled %d ", dlci_conn_param->dlci_id);

  logical_tx_wmk_ptr = ds_mux_logical_get_tx_wmk( dlci_conn_param->dlci_id );

  //No Action required as there is no pending data to send
  if ( ( NULL == logical_tx_wmk_ptr) || (TRUE == dsm_is_wm_empty( logical_tx_wmk_ptr )) )
  {
    return;
  }
  //enable Signal so that we can process pending data
  sig = ds_mux_get_sig_by_id( dlci_conn_param->dlci_id );

  DS_MUX_SET_SIGNAL(sig);

}/* ds_mux_dlci_n_flow_enabled */


void  ds_mux_enable_flow_mask
(
  uint8                           dlci_id,
  ds_mux_flow_ctl_mask_enum_type  fc_msk
)
{
  ds_mux_dlci_param_type        *dlci_conn_param = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  dlci_conn_param = ds_mux_get_dlci_conn_ptr(dlci_id);

  if ( NULL == dlci_conn_param )
  {
   return;
  }

  DS_MUX_LOG_MSG_INFO2_3( "ds_mux_enable_flow_mask dlci_id %d fc_msk  %x current flow mask %x",
      dlci_id, fc_msk, dlci_conn_param->flow_control_mask);

  DS_MUX_ENTER_CRIT_SECTION(&global_ds_mux_crit_section);

  dlci_conn_param->flow_control_mask  &= (~fc_msk);

  DS_MUX_LEAVE_CRIT_SECTION(&global_ds_mux_crit_section);

  //Flow is beaing enabled on this DLCI hence
  if ( DS_MUX_FLOW_DISABLE_MIN != dlci_conn_param->flow_control_mask)
  {
    if ( 0 == dlci_id)
    {
      ds_mux_dlci_0_flow_enabled( dlci_conn_param );
    }
    else
    {
      ds_mux_dlci_n_flow_enabled( dlci_conn_param );
    }

  }

}/* ds_mux_enable_flow_mask */

boolean   ds_mux_is_flow_enabled
(
  uint8                           dlci_id
)
{
  ds_mux_dlci_param_type        *dlci_conn_param = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  dlci_conn_param = ds_mux_get_dlci_conn_ptr(dlci_id);

  if ( NULL == dlci_conn_param )
  {
    return FALSE;
  }

  if ( DS_MUX_FLOW_DISABLE_MIN != dlci_conn_param->flow_control_mask)
  {
    return FALSE;
  }

  return TRUE;
}/*ds_mux_is_flow_enabled*/

dlci_cmux_param_type* ds_mux_get_cmux_params()
{
  return ds_mux_cmux_params;
}

