/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     D S   M U X   P H Y S

GENERAL DESCRIPTION
  This file contains all the functions, definitions and data types needed
  for MUX task to interface to the SIO for physical port connection.


  Copyright (c) 2016,2021 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/dataservices.tx/6.0/tethering/qti_bridge/src/ds_mux_phys.c#3 $
  $DateTime: 2021/05/05 04:35:02 $
  $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/01/16   ad     Initial version
===========================================================================*/


/*===========================================================================

                      INCLUDE FILES

===========================================================================*/

#include "ds_mux_phys.h"
#include "ds_mux.h"
#include "ds_mux_logging.h"

#ifdef TEST_FRAMEWORK
  #include "ds_mux_stub.h"
#endif /*  TEST_FRAMEWORK */

#ifdef QTI_LOGGING_ENABLED
#include "ULogFront.h"
#endif

extern qti_bridge_qurt_t  qti_bridge_qurt;
#ifdef QTI_LOGGING_ENABLED
extern ULogHandle qti_ulog;
#endif


ds_mux_phys_sio_conn_info_type    *ds_mux_phys_conn_info = NULL;

/*===========================================================================
                            LOCAL DEFINITIONS
===========================================================================*/
#define DS_MUX_PHYS_WM_DEFAULT_LO   175000  //TBD
#define DS_MUX_PHYS_WM_DEFAULT_HI   350000  //TBD
#define DS_MUX_PHYS_WM_DEFAULT_DNE  400000  //TBD

#define DS_MUX_SERVICE               RDM_DATA_MUX_SRVC
#define DS_MUX_PHY_DEV               RDM_MUX_PHYSICAL_DEV
#define DS_MUX_PHY_DEV_NAME          "RDM_MUX_PHYSICAL_DEV"
#define DS_MUX_MAX_RDM_OPR_RETRY     10  //TBD
#define DS_MUX_RDM_OPR_RETRY_TIMER   100 //TBD

/*===========================================================================
                      PUBLIC FUNCTION DEFINATIONS
===========================================================================*/
void ds_mux_phys_init
(

)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_mux_phys_conn_info = NULL;


}/* ds_mux_phys_init */
void ds_mux_phys_deinit
(

)
{
  dsm_item_type                  *item_ptr           = NULL;
  dsm_watermark_type             *phy_rx_wmk_ptr     = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  phy_rx_wmk_ptr = &ds_mux_phys_conn_info->rx_wmk;

  if (NULL != phy_rx_wmk_ptr)
  {
    /*-----------------------------------------------------------------------
     Flush out all the packets
    -----------------------------------------------------------------------*/
    while( NULL != (item_ptr = dsm_dequeue( phy_rx_wmk_ptr )))
      dsm_free_packet(&item_ptr);
  }

  /*-----------------------------------------------------------------------
    1. Disable Uplink/Transmit data signal
  -----------------------------------------------------------------------*/
  (void) ds_mux_set_sig_handler(DS_MUX_RX_DATA_SIGNAL, NULL, NULL);
   DS_MUX_CLR_SIGNAL(DS_MUX_RX_DATA_SIGNAL);

  /*-----------------------------------------------------------------------
    2. Destroy queue assosiated with Rx/Tx WMs so that any DSM item stuck in queue will be freed
  -----------------------------------------------------------------------*/
  dsm_queue_destroy( &ds_mux_phys_conn_info->tx_wmk );
  #ifndef FEATURE_DSM_Q_DESTROY
  q_destroy( &ds_mux_phys_conn_info->tx_q );
  #endif

  dsm_queue_destroy( &ds_mux_phys_conn_info->rx_wmk );
  #ifndef FEATURE_DSM_Q_DESTROY
  q_destroy( &ds_mux_phys_conn_info->rx_q );
  #endif

  if ( NULL != ds_mux_phys_conn_info)
  {
/*-----------------------------------------------------------------------
    3. Delete physical connection
  -----------------------------------------------------------------------*/

     DS_MUX_MEM_FREE(ds_mux_phys_conn_info);
  }
}/* ds_mux_phys_deinit */


static void  ds_mux_phys_tx_wmk_low_func
(
  dsm_watermark_type  * ds_mux_tx_wmk,
  void                * user_data_ptr
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

}/* ds_mux_phys_tx_wmk_low_func */

static void  ds_mux_phys_tx_wmk_hi_func
(
  dsm_watermark_type  * ds_mux_tx_wmk,
  void                * user_data_ptr
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

}/* ds_mux_phys_tx_wmk_hi_func */


static void  ds_mux_phys_rx_wmk_low_func
(
  dsm_watermark_type  * ds_mux_rx_wmk,
  void                * user_data_ptr
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

}/* ds_mux_phys_rx_wmk_low_func */

static void  ds_mux_phys_rx_wmk_hi_func
(
  dsm_watermark_type  * ds_mux_rx_wmk,
  void                * user_data_ptr
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

}/* ds_mux_phys_rx_wmk_hi_func */


static void  ds_mux_phys_rx_wmk_non_empty_func
(
  dsm_watermark_type  * ds_mux_rx_wmk,
  void                * user_data_ptr
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
     Set Rx data signal. DS MUX IO will dequeue the packet in ds_mux_io_rx_sig_hdlr
     and will process the packet
  -----------------------------------------------------------------------*/
  DS_MUX_SET_SIGNAL(DS_MUX_RX_DATA_SIGNAL);

}/* ds_mux_phys_rx_wmk_non_empty_func */

void ds_mux_phys_dtr_changed_cb
(
  void
)
{
  sio_ioctl_param_type          ioctl_param;
  uint8                         dlci_id = DS_MUX_DLCI_ID_1;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  memset( &ioctl_param, 0x0, sizeof(sio_ioctl_param_type));

  ioctl_param.dte_ready_asserted = &ds_mux_phys_conn_info->dte_ready_asserted;

  #ifndef TEST_FRAMEWORK

  sio_ioctl( ds_mux_phys_conn_info->stream_id,
            SIO_IOCTL_DTE_READY_ASSERTED,
            &ioctl_param );
  #else

  ds_mux_stub_sio_ioctl( ds_mux_phys_conn_info->stream_id,
            SIO_IOCTL_DTE_READY_ASSERTED,
            &ioctl_param );

  #endif /* TEST_FRAMEWORK */

    for(dlci_id = DS_MUX_DLCI_ID_0;  dlci_id <= DS_MUX_MAX_LOGICAL_CONNECTION; dlci_id++  )
    {
      if( FALSE ==  ds_mux_phys_conn_info->dte_ready_asserted)
      {
        ds_mux_disable_flow_mask( dlci_id, DS_MUX_FLOW_DISABLE_PHY_DTR_LOW );
      }
      else
      {
        ds_mux_enable_flow_mask( dlci_id, DS_MUX_FLOW_DISABLE_PHY_DTR_LOW );
      }
    }


}/*ds_mux_phys_dtr_changed_cb*/

ds_mux_result_enum_type ds_mux_phys_conn_init
(
)
{
  ds_mux_result_enum_type  result  = DS_MUX_SUCCESS;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_mux_phys_conn_info = (ds_mux_phys_sio_conn_info_type*)malloc(sizeof(ds_mux_phys_sio_conn_info_type));

  if ( NULL == ds_mux_phys_conn_info)
  {
    return DS_MUX_FAILURE;
  }

  memset((ds_mux_phys_sio_conn_info_type*)ds_mux_phys_conn_info, 0, sizeof(ds_mux_phys_sio_conn_info_type));

  do
  {
   /*-----------------------------------------------------------------------
      1.  Initialize the TX Watermark
      2.  Initialize the RX Watermark
      3.  Set flow control flag as false
      4.  Set RDM operation retry values to 0
    -----------------------------------------------------------------------*/
    /* Step 1 */
    dsm_queue_init ( &ds_mux_phys_conn_info->tx_wmk,
                     0x7FFFFFFF,
                     &ds_mux_phys_conn_info->tx_q );

    dsm_set_low_wm( &ds_mux_phys_conn_info->tx_wmk, DS_MUX_PHYS_WM_DEFAULT_LO );
    dsm_set_hi_wm( &ds_mux_phys_conn_info->tx_wmk, DS_MUX_PHYS_WM_DEFAULT_HI );
    dsm_set_dne( &ds_mux_phys_conn_info->tx_wmk, DS_MUX_PHYS_WM_DEFAULT_DNE );

    ds_mux_phys_conn_info->tx_wmk.lowater_func_ptr = (wm_cb_type)ds_mux_phys_tx_wmk_low_func;
    ds_mux_phys_conn_info->tx_wmk.hiwater_func_ptr = (wm_cb_type)ds_mux_phys_tx_wmk_hi_func;


    /* Step 2 */
    dsm_queue_init ( &ds_mux_phys_conn_info->rx_wmk,
                     0x7FFFFFFF,
                     &ds_mux_phys_conn_info->rx_q );

    dsm_set_low_wm( &ds_mux_phys_conn_info->rx_wmk, DS_MUX_PHYS_WM_DEFAULT_LO );
    dsm_set_hi_wm( &ds_mux_phys_conn_info->rx_wmk, DS_MUX_PHYS_WM_DEFAULT_HI );
    dsm_set_dne( &ds_mux_phys_conn_info->rx_wmk, DS_MUX_PHYS_WM_DEFAULT_DNE );

    ds_mux_phys_conn_info->rx_wmk.non_empty_func_ptr = (wm_cb_type)ds_mux_phys_rx_wmk_non_empty_func;
    ds_mux_phys_conn_info->rx_wmk.lowater_func_ptr = (wm_cb_type)ds_mux_phys_rx_wmk_low_func;
    ds_mux_phys_conn_info->rx_wmk.hiwater_func_ptr = (wm_cb_type)ds_mux_phys_rx_wmk_hi_func;

    ds_mux_enable_sig( DS_MUX_RX_DATA_SIGNAL );

    ds_mux_set_sig_handler(DS_MUX_RX_DATA_SIGNAL, ds_mux_io_rx_sig_hdlr, NULL);

#if 0
    /* Step 3*/
    ds_mux_phys_conn_info->is_tx_flow_controlled   = FALSE;
    ds_mux_phys_conn_info->is_rx_flow_controlled   = FALSE;
    ds_mux_phys_conn_info->stream_id               = SIO_NO_STREAM_ID;
    ds_mux_phys_conn_info->dte_ready_asserted      = FALSE;
    ds_mux_phys_conn_info->sio_port                = SIO_PORT_NULL;

     /* Step 4*/
    ds_mux_phys_conn_info->rdm_oprt_retry = 0;
    ds_mux_phys_conn_info->timer_handle   = DS_MUX_TIMER_INVALID_HANDLE;
#endif
  }while(0);

  return result;
}/*ds_mux_phys_conn_init*/


void ds_mux_phys_rdm_open_handler
(
  sio_port_id_type port_id
)
{
  #if 0
  ds_mux_result_enum_type  result  = DS_MUX_SUCCESS;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  sio_open_type        open_param;
  sio_ioctl_param_type ioctl_param;
  if ( NULL == ds_mux_phys_conn_info)
  {
      return;
  }
  memset( &open_param, 0x0, sizeof(sio_open_type));
  memset( &ioctl_param, 0x0, sizeof(sio_ioctl_param_type));

  do
  {
    /*-----------------------------------------------------------------------
      1.  Initialize SIO parameters
      2.  Open port with SIO
      3.  Register for DTR change and also check current DTR status
      4   Set Rx Signal handler
      5   Notify DS MUX about port open successful or failure so that APP can be notified
      -----------------------------------------------------------------------*/
    /* Step 1 */
    open_param.rx_queue    = &ds_mux_phys_conn_info->rx_wmk;
    open_param.tx_queue    = &ds_mux_phys_conn_info->tx_wmk;
    open_param.rx_func_ptr = NULL;
    open_param.stream_mode = SIO_GENERIC_MODE;
    open_param.stream_type = SIO_STREAM_RXTX;
    open_param.port_id     = port_id;
    open_param.rx_bitrate  = ds_mux_get_baud_rate();
    open_param.tx_bitrate  = ds_mux_get_baud_rate();

    /* Step 2 */
    #ifndef TEST_FRAMEWORK
      ds_mux_phys_conn_info->stream_id = sio_open(&open_param);
    #else
      ds_mux_phys_conn_info->stream_id = ds_mux_stub_sio_open(&open_param);
    #endif /* TEST_FRAMEWORK */

    if ( SIO_NO_STREAM_ID == ds_mux_phys_conn_info->stream_id)
    {
      result  = DS_MUX_FAILURE;
      rdm_notify( RDM_DATA_MUX_SRVC, RDM_APP_BUSY_S );
      /*-----------------------------------------------------------------------
        Close down devices so that in next init we do register with device again
        -----------------------------------------------------------------------*/
      rdm_close_device(DS_MUX_SERVICE, DS_MUX_PHY_DEV, NULL);
      break;
    }
    else
    {
      rdm_notify( RDM_DATA_MUX_SRVC, RDM_DONE_S );
    }

   /* Step 3 */
    ioctl_param.enable_dte_ready_event = ds_mux_phys_dtr_changed_cb;

    #ifndef TEST_FRAMEWORK
      sio_ioctl( ds_mux_phys_conn_info->stream_id,
                 SIO_IOCTL_ENABLE_DTR_EVENT,
                 &ioctl_param );

      ioctl_param.dte_ready_asserted = &ds_mux_phys_conn_info->dte_ready_asserted;

      sio_ioctl( ds_mux_phys_conn_info->stream_id,
            SIO_IOCTL_DTE_READY_ASSERTED,
            &ioctl_param );

    #else
      ds_mux_stub_sio_ioctl( ds_mux_phys_conn_info->stream_id,
                 SIO_IOCTL_ENABLE_DTR_EVENT,
                 &ioctl_param );

      ioctl_param.dte_ready_asserted = &ds_mux_phys_conn_info->dte_ready_asserted;

      ds_mux_stub_sio_ioctl( ds_mux_phys_conn_info->stream_id,
              SIO_IOCTL_DTE_READY_ASSERTED,
              &ioctl_param );
    #endif/* TEST_FRAMEWORK*/


  }while(0);

#endif
  /* Step 4 -  Currently moved next 2 statements as part of powerup & initialization */

// Above 2 stm move to phy_conn_iinit


}/* ds_mux_phys_rdm_open_cb */


ds_mux_result_enum_type ds_mux_phys_port_connect
(
  boolean                is_cmux_supported,
  sio_stream_id_type     stream_id
)
{
  ds_mux_result_enum_type  result  = DS_MUX_SUCCESS;
  sio_ioctl_param_type         ioctl_param;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (SIO_NO_STREAM_ID == stream_id || NULL == ds_mux_phys_conn_info)
  {
#ifdef QTI_LOGGING_ENABLED
    ULogFront_RealTimePrintf(qti_ulog, 0, "p2q");
#endif
    return DS_MUX_FAILURE;
  }
  /*-----------------------------------------------------------------------
    Store SIO port
    -----------------------------------------------------------------------*/
  ds_mux_phys_conn_info->stream_id= stream_id;

//Register for DTR call back
//CHE//code copy from open handleer to ck the DTE status

  ioctl_param.enable_dte_ready_event = ds_mux_phys_dtr_changed_cb;

  sio_ioctl( ds_mux_phys_conn_info->stream_id,
             SIO_IOCTL_ENABLE_DTR_EVENT,
             &ioctl_param );

  ioctl_param.dte_ready_asserted = &ds_mux_phys_conn_info->dte_ready_asserted;

  sio_ioctl( ds_mux_phys_conn_info->stream_id,
             SIO_IOCTL_DTE_READY_ASSERTED,
             &ioctl_param );

  ds_mux_update_phy_port_open_status( result );

//Remove below code
#if 0
  do
  {
    /*-----------------------------------------------------------------------
       1.  Initialize RDM service params
       2.  Register service with RDM and set compatibility
            Step 3 to 5 is required only when DS MUX share same port
            with other service
       3.  Initialize RDM device params  and register device with RDM.
       4.  Assign port to service
       5. Allocate the timer and Start timer if RDM is busy else free the timer
     -----------------------------------------------------------------------*/
    /* Step 1 */
    rdm_params.device_support   = RDM_SINGLE_DEV_SUPPORT;
    rdm_params.open_multi_func  = NULL;
    rdm_params.close_multi_func = NULL;
    rdm_params.open_func        = ds_mux_phys_rdm_open_cb;
    rdm_params.close_func       = ds_mux_phys_rdm_close_cb;
    rdm_params.data             = NULL;

    /* Step 2 */
    rdm_register_service ( DS_MUX_SERVICE, &rdm_params );

    if ( TRUE == is_cmux_supported)
    {
      rdm_set_compatibility ( DS_MUX_SERVICE, DS_MUX_PHY_DEV);

      /* Step 3 */
      rdm_device_info.device    = DS_MUX_PHY_DEV;
      rdm_device_info.sio_port  = sio_port_id;
      rdm_device_info.available = TRUE;


      (void) ds_mux_memscpy( &rdm_device_info.device_name[0],
                             RDM_MAX_DEV_NAME_LEN,
                             DS_MUX_PHY_DEV_NAME,
                             strlen(DS_MUX_PHY_DEV_NAME) );

      rdm_register_device( &rdm_device_info );

      /* Step 4*/
      if ( rdm_assign_port( DS_MUX_SERVICE,
                            DS_MUX_PHY_DEV,
                            ds_mux_phys_rdm_assign_port_cb ) )
      {
        ds_mux_phys_conn_info->timer_handle = ds_mux_timer_alloc( &ds_mux_phys_conn_info->timer_handle,
                                                                  &ds_mux_phys_conn_info->timer_attr,
                                                                  ds_mux_phys_rdm_oprt_timer_cb,
                                                                  (void *)DS_MUX_PHYS_RDM_ASSIGN_PORT );

        DS_MUX_LOG_MSG_INFO2_1( "RDM assign port request successful timer handle %d",
            ds_mux_phys_conn_info->timer_handle);
      }
      else
      {
        result  = DS_MUX_FAILURE;
        DS_MUX_LOG_MSG_INFO2_0("RDM assign port request failed");
        break;
      }
    }
    return result;

  }while(0);

  result  = DS_MUX_FAILURE;
#endif

  return result;
}/*ds_mux_result_enum_type*/



dsm_watermark_type* ds_mux_phy_get_rx_wmk
(

)
{
  dsm_watermark_type*   rx_wmk_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( NULL != ds_mux_phys_conn_info )
  {
    rx_wmk_ptr = &ds_mux_phys_conn_info->rx_wmk;
  }

  if( NULL == rx_wmk_ptr)
  {
#ifdef QTI_LOGGING_ENABLED
    ULogFront_RealTimePrintf(qti_ulog, 0, "p2q");
#endif
    LOG_MSG_ERROR_0("ds_mux_phy_get_rx_wmk(): Invalid rx wmk");
  }

  return rx_wmk_ptr;
}/* ds_mux_phy_get_rx_wmk */

ds_mux_result_enum_type ds_mux_phys_transmit
(
  dsm_item_type            *item_ptr
)
{
  ds_mux_result_enum_type       result   = DS_MUX_SUCCESS;
  sio_ioctl_param_type          ioctl_param;

  dsm_item_type *temp_dsm = NULL;
  dsm_item_type *temp_dsm2 = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-----------------------------------------------------------------------
    1 Check the DTR status
      1.1 DTR is low then get real time status
    2. DTR is HIGH then transmit the data to SIO
  -----------------------------------------------------------------------*/
  if ( NULL == ds_mux_phys_conn_info)
  {
#ifdef QTI_LOGGING_ENABLED
    ULogFront_RealTimePrintf(qti_ulog, 0, "p2q");
#endif
    return DS_MUX_FAILURE;
  }
  /*Step 1*/
  if ( FALSE == ds_mux_phys_conn_info->dte_ready_asserted )
  {
    memset( &ioctl_param, 0x0, sizeof(sio_ioctl_param_type));

    ioctl_param.rx_line_state = &ds_mux_phys_conn_info->dte_ready_asserted;

   #ifndef TEST_FRAMEWORK
    sio_ioctl( ds_mux_phys_conn_info->stream_id,
              SIO_IOCTL_CHECK_RX_STATE,
              &ioctl_param );
   #else

    ds_mux_stub_sio_ioctl( ds_mux_phys_conn_info->stream_id,
             SIO_IOCTL_CHECK_RX_STATE,
             &ioctl_param );

   #endif/* TEST_FRAMEWORK */
  }
  /*Step 2*/
  if ( ds_mux_phys_conn_info->dte_ready_asserted )
  {
    DS_MUX_LOG_PHY_TX_PACKET( item_ptr);

    #ifndef TEST_FRAMEWORK
      //De-chain the DSM Item before transmitting
      for (temp_dsm = item_ptr; temp_dsm;)
      {
          temp_dsm2 = temp_dsm;
          temp_dsm = temp_dsm->pkt_ptr;
          temp_dsm2->pkt_ptr = NULL;
          sio_transmit( ds_mux_phys_conn_info->stream_id, temp_dsm2 );
      }
    #else
    ds_mux_stub_sio_transmit( ds_mux_phys_conn_info->stream_id,
                item_ptr );

    #endif/* TEST_FRAMEWORK */
  }
  else
  {
    result   = DS_MUX_FAILURE;
  }
  return result;
}/* ds_mux_phys_transmit */


/*==============================================================================
FUNCTION DS_MUX_UPDATE_PHYS_RX_WMK()

DESCRIPTION
  This function will fill the DSM Item received from Physical interconnect
  into the DS MUX PHYS RX water mark

PARAMETERS
  NONE

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
==============================================================================*/
void ds_mux_update_phys_rx_wmk(dsm_item_type *dsm_item_ptr)
{
  dsm_watermark_type             *phy_rx_wmk_ptr     = NULL;

  //Avoid logging here since it will be printed for each DSM item

  phy_rx_wmk_ptr = ds_mux_phy_get_rx_wmk();

  if ( phy_rx_wmk_ptr ==NULL )
  {
#ifdef QTI_LOGGING_ENABLED
    ULogFront_RealTimePrintf(qti_ulog, 0, "p2q");
#endif
    DS_MUX_LOG_MSG_INFO1_0( "ds_mux_update_phys_rx_wmk, Phys RX WMK Ptr is NULL");
    return;
  }

  //Enqueue the DSM item into the Phys RX WMK

  dsm_enqueue(phy_rx_wmk_ptr,&dsm_item_ptr);
  return;
}

