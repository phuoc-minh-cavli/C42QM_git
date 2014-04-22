/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     D S   M U X    L O G I C A L

GENERAL DESCRIPTION
  This file contains all the functions, definitions and data types needed 
  for other tasks to interface to the MUX Ports.



  Copyright (c) 2016 by Qualcomm Technologies, Incorporated.  All Rights Reserved.  
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/dataservices.tx/6.0/tethering/qti_bridge/src/ds_mux_logical.c#1 $ 
  $DateTime: 2019/11/26 00:18:17 $ 
  $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/01/16   ad     Initial version
===========================================================================*/


/*===========================================================================

                      INCLUDE FILES

===========================================================================*/

#include "ds_mux_logical.h"
#include "ds_mux_taski.h"
#include "ds_mux.h"
#include "ds_mux_logging.h"

ds_mux_logical_sio_iface_info_type ds_mux_logical_conn_info;

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

void ds_mux_logical_init
(
)
{
  uint8  index  = 0 ;
  static boolean is_initialized = FALSE;
    
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_MUX_LOG_MSG_INFO2_1( "ds_mux_logical_init is_initialized %d ", is_initialized);

  /*-----------------------------------------------------------------------
    This layer can be initialized from SIO (when DS MUX do driver init) or from DS MUX task init. 
    Static flag will set to true at first initialization and sub sequent Init will be ignored 
  -----------------------------------------------------------------------*/
  DS_MUX_ENTER_CRIT_SECTION(&global_ds_mux_crit_section);
  
  if ( FALSE == is_initialized )
  {
    is_initialized = TRUE;
    
    memset( &ds_mux_logical_conn_info, 0x0, sizeof(ds_mux_logical_sio_iface_info_type));
  
    for ( index  = 0; index < DS_MUX_MAX_LOGICAL_CONNECTION; index ++)
    {
  
      ds_mux_logical_conn_info.logical_cons[index].dlci_id        = 0xFF;
  
      ds_mux_logical_conn_info.logical_cons[index].port_id        = SIO_PORT_NULL;
          
      ds_mux_logical_conn_info.logical_cons[index].rdm_oprt_retry = 0;
      
      ds_mux_logical_conn_info.logical_cons[index].timer_handle   = DS_MUX_TIMER_INVALID_HANDLE;
  
      ds_mux_logical_conn_info.logical_cons[index].port_status = 
        DS_MUX_LOGICAL_PORT_MIN;
  
      ds_mux_logical_conn_info.logical_cons[index].rdm_device  = 
        (rdm_device_enum_type)(DS_MUX_BASE_LOGICAL_RDM_DEVICE + index);
  
      ds_mux_logical_conn_info.logical_cons[index].service     = 
        DS_MUX_LOGICAL_SERVICE;
    }
  }
  DS_MUX_LEAVE_CRIT_SECTION(&global_ds_mux_crit_section);

}/* ds_mux_logical_init */

uint8   ds_mux_get_idx_from_sio_port
(
  sio_port_id_type   port_id
)
{
  uint8    index = 0xFF;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    loop through all logical connection and find valid index for given port
  -----------------------------------------------------------------------*/
  for ( index  = 0; index < DS_MUX_MAX_LOGICAL_CONNECTION; index ++)
  {
     if ( port_id == ds_mux_logical_conn_info.logical_cons[index].port_id )
     {
       break;
     }
  }
  /*-----------------------------------------------------------------------
    Unable to find index
  -----------------------------------------------------------------------*/
  if( DS_MUX_MAX_LOGICAL_CONNECTION == index)
  {
    index = 0xFF;
  }
  
  DS_MUX_LOG_MSG_INFO2_2( "ds_mux_get_idx_from_sio_port index %d port_id %d ",
    index, port_id);
  
  return index;
} /* ds_mux_get_idx_from_sio_port */

uint8   ds_mux_get_idx_from_dlci_id
(
  uint8   dlci_id
)
{
  uint8    index = 0xFF;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  

  /*-----------------------------------------------------------------------
    loop through all logical connection and find valid index for given DLCI ID
  -----------------------------------------------------------------------*/
  for ( index  = 0; index < DS_MUX_MAX_LOGICAL_CONNECTION; index ++)
  {
    if ( dlci_id == ds_mux_logical_conn_info.logical_cons[index].dlci_id )
    {
      break;
    }
  }
  /*-----------------------------------------------------------------------
    Unable to find index
  -----------------------------------------------------------------------*/
  if( DS_MUX_MAX_LOGICAL_CONNECTION == index)
   {
     index = 0xFF;
   }

  DS_MUX_LOG_MSG_INFO2_2( "ds_mux_get_idx_from_dlci_id index %d port_id %d ",
    index, dlci_id);
  
  return index;

} /* ds_mux_get_idx_from_dlci_id */

uint8   ds_mux_get_idx_from_rdm_dev
(
  rdm_device_enum_type device
)
{
  uint8    index = 0xFF;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  

  /*-----------------------------------------------------------------------
    loop through all logical connection and find valid index for given Device ID
  -----------------------------------------------------------------------*/
  for ( index  = 0; index < DS_MUX_MAX_LOGICAL_CONNECTION; index ++)
  {
     if ( device == ds_mux_logical_conn_info.logical_cons[index].rdm_device )
     {
       break;
     }
  }
  /*-----------------------------------------------------------------------
    Unable to find index
  -----------------------------------------------------------------------*/
  if( DS_MUX_MAX_LOGICAL_CONNECTION == index)
  {
    index = 0xFF;
  }

  DS_MUX_LOG_MSG_INFO2_2( "ds_mux_get_idx_from_rdm_dev index %d device %d ",
    index, device);
  
  return index;
}/* ds_mux_get_idx_from_rdm_dev*/

sio_status_type ds_mux_logical_drv_sio_open
(
  sio_open_type *open_ptr
)
{
  uint8                      index               = 0xFF;
  sio_status_type            ret_val             = SIO_DONE_S;
  ds_muxi_cmd_info_type     *ds_mux_cmd_info_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_MUX_LOG_MSG_INFO2_1( "ds_mux_logical_drv_sio_open recv port open 0x%x",
    open_ptr);

  /*-------------------------------------------------------------------------
    This function will be invoked when Client call SIO open on MUX ports
    1. Store the Rx and Tx WMs
      1.1 Flow would be disabled and client has to enable the flow
    2. Send Port open command to command to DS MUX 
  -------------------------------------------------------------------------*/
  if ( NULL == open_ptr)
  {
    return SIO_BADP_S;
  }

  if ( 0xFF == (index = ds_mux_get_idx_from_sio_port( open_ptr->port_id )) )
  {
    DS_MUX_LOG_MSG_ERROR_1( "ds_mux_logical_drv_sio_open index is invalid "
                            " index %d  ", index );
    return SIO_UNAVAIL_S;
  }

  /*-------------------------------------------------------------------------
    Allocate DS MUX cmd buf, populate it and post cmd to DS MUX task
  -------------------------------------------------------------------------*/
  ds_mux_cmd_info_ptr = (ds_muxi_cmd_info_type*)malloc(sizeof(ds_muxi_cmd_info_type));
  if (NULL == ds_mux_cmd_info_ptr)
  {
    /*-----------------------------------------------------------------------
      No free command buffers available. Notify RDM that the open failed.
    -----------------------------------------------------------------------*/
    return SIO_UNAVAIL_S;
  }
  
  DS_MUX_ENTER_CRIT_SECTION(&global_ds_mux_crit_section);
    
  /*  Step 1*/
  ds_mux_logical_conn_info.logical_cons[index].rx_queue = open_ptr->rx_queue;
  ds_mux_logical_conn_info.logical_cons[index].tx_queue = open_ptr->tx_queue;
  
  ds_mux_logical_conn_info.logical_cons[index].port_status = 
    DS_MUX_LOGICAL_PORT_OPEN;

  ds_mux_logical_conn_info.logical_cons[index].ioctl_data.in_bound_flow_disabled = TRUE;

  DS_MUX_LEAVE_CRIT_SECTION(&global_ds_mux_crit_section);
    
  ds_mux_cmd_info_ptr->cmd_id  = DS_MUXI_LOGICAL_PORT_OPEN;
  
  ds_mux_cmd_info_ptr->cmd_info.logical_port_open_info.dlci_id = 
    ds_mux_logical_conn_info.logical_cons[index].dlci_id;
  
  /*  Step 2*/
  ds_mux_send_cmd(DS_MUX_CMD, ds_mux_cmd_info_ptr);

  
  return ret_val;

} /* ds_mux_logical_drv_sio_open() */

void ds_mux_logical_drv_sio_close
(
  sio_stream_id_type   stream_id,
  sio_port_id_type     port_id,
  void                (*close_func_ptr)(void) 
)
{
  uint8                      index = 0xFF;
  ds_muxi_cmd_info_type     *ds_mux_cmd_info_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_MUX_LOG_MSG_INFO2_1( "ds_mux_logical_drv_sio_close recv port "
                          " close stream id %d", stream_id );

  /*-------------------------------------------------------------------------
    This function will be invoked when Client call SIO close on MUX ports
    1. Store close  call back function. Close call back will be invoked once DLCI is closed
    2. Send Port close command to command to DS MUX 
    Note : Client supposed to call dsm_queue_destroy on each RX and TX wm to discard packets(If Any)
  -------------------------------------------------------------------------*/
  if ( 0xFF == (index = ds_mux_get_idx_from_sio_port( port_id )))
  {
    DS_MUX_LOG_MSG_ERROR_1( " ds_mux_logical_drv_sio_close index is invalid "
                            " index %d  ", index );
    return ;
  }

  /*-------------------------------------------------------------------------
    Allocate DS MUX cmd buf, populate it and post cmd to DS MUX task
  -------------------------------------------------------------------------*/
  ds_mux_cmd_info_ptr = (ds_muxi_cmd_info_type*)malloc(sizeof(ds_muxi_cmd_info_type));
  if (NULL == ds_mux_cmd_info_ptr)
  {
    /*-----------------------------------------------------------------------
      No free command buffers available. Notify RDM that the open failed.
    -----------------------------------------------------------------------*/
    return ;
  }
  DS_MUX_ENTER_CRIT_SECTION(&global_ds_mux_crit_section);
  
  /*  Step 1*/
  ds_mux_logical_conn_info.logical_cons[index].close_cb_ptr = close_func_ptr;

  ds_mux_logical_conn_info.logical_cons[index].port_status =  DS_MUX_LOGICAL_PORT_CLOSED; 

  /*-----------------------------------------------------------------------
    RESET all signals . DS MUX just send MSC message to host and keep logical channel open
  -----------------------------------------------------------------------*/
  ds_mux_logical_conn_info.logical_cons[index].ioctl_data.cd_asserted            = FALSE;
  ds_mux_logical_conn_info.logical_cons[index].ioctl_data.rtc_asserted           = FALSE;
  ds_mux_logical_conn_info.logical_cons[index].ioctl_data.ri_asserted            = FALSE;
  ds_mux_logical_conn_info.logical_cons[index].ioctl_data.in_bound_flow_disabled = TRUE;

  DS_MUX_LEAVE_CRIT_SECTION(&global_ds_mux_crit_section);

  ds_mux_cmd_info_ptr->cmd_id  = DS_MUXI_LOGICAL_PORT_CLOSE;
  
  ds_mux_cmd_info_ptr->cmd_info.logical_port_close_info.dlci_id = 
    ds_mux_logical_conn_info.logical_cons[index].dlci_id;
  
  /*  Step 2*/
  ds_mux_send_cmd(DS_MUX_CMD, ds_mux_cmd_info_ptr);

}/* ds_mux_logical_drv_sio_close */

void ds_mux_logical_relay_v24_sig
(
  uint8                   index
)
{
  ds_muxi_cmd_info_type     *ds_mux_cmd_info_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Allocate DS MUX cmd buf, populate it and post cmd to DS MUX task
  -------------------------------------------------------------------------*/
  ds_mux_cmd_info_ptr = (ds_muxi_cmd_info_type*)malloc(sizeof(ds_muxi_cmd_info_type));
  if (NULL == ds_mux_cmd_info_ptr)
  {
    /*-----------------------------------------------------------------------
      No free command buffers available. Notify RDM that the open failed.
    -----------------------------------------------------------------------*/
    return ;
  }

  ds_mux_cmd_info_ptr->cmd_id  = DS_MUXI_LOGICAL_PORT_IOCTL;

  /*-----------------------------------------------------------------------
    Post a command to MUX to send all signal to the host for this logical port
    (MSC message will be send to host)
  -----------------------------------------------------------------------*/
  ds_mux_cmd_info_ptr->cmd_info.logical_port_ioctl_info.dlci_id = 
    ds_mux_logical_conn_info.logical_cons[index].dlci_id;

  ds_mux_cmd_info_ptr->cmd_info.logical_port_ioctl_info.data_valid = 
    ds_mux_logical_conn_info.logical_cons[index].ioctl_data.cd_asserted;

  ds_mux_cmd_info_ptr->cmd_info.logical_port_ioctl_info.rtc = 
    ds_mux_logical_conn_info.logical_cons[index].ioctl_data.rtc_asserted;

  ds_mux_cmd_info_ptr->cmd_info.logical_port_ioctl_info.fc = 
    ds_mux_logical_conn_info.logical_cons[index].ioctl_data.in_bound_flow_disabled;

  ds_mux_cmd_info_ptr->cmd_info.logical_port_ioctl_info.ic = 
    ds_mux_logical_conn_info.logical_cons[index].ioctl_data.ri_asserted;
  

  ds_mux_send_cmd(DS_MUX_CMD, ds_mux_cmd_info_ptr);

  DS_MUX_LOG_MSG_INFO2_6( " ds_mux_logical_relay_v24_sig signal status "
                          " index %d  dlci Id %d cd %d rtc%d fc %d ri %d", index, 
                          ds_mux_logical_conn_info.logical_cons[index].dlci_id,
                          ds_mux_logical_conn_info.logical_cons[index].ioctl_data.cd_asserted,
                          ds_mux_logical_conn_info.logical_cons[index].ioctl_data.rtc_asserted,
                          ds_mux_logical_conn_info.logical_cons[index].ioctl_data.in_bound_flow_disabled,
                          ds_mux_logical_conn_info.logical_cons[index].ioctl_data.ri_asserted);
  
}/*ds_mux_logical_relay_v24_sig*/

void ds_mux_logical_drv_sio_ioctl 
(
  sio_stream_id_type    stream_id,
  sio_port_id_type      port_id,
  sio_ioctl_cmd_type    cmd,
  sio_ioctl_param_type *param_ptr
)
{
  uint8                      index = 0xFF;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_MUX_LOG_MSG_INFO2_3( " ds_mux_logical_drv_sio_ioctl stream id %d "
                          " port id %d  cmd %d", stream_id, port_id, cmd );

  /*-------------------------------------------------------------------------
    This function will be invoked when Client call SIO IOCTL on MUX ports
    1. Client has to Assert the signal once they open the port (DSR, CD, Flow Enable)
    2. DS MUX will relay the Asserted/De-Asserted signal to host/remote MUX via MSC command
  -------------------------------------------------------------------------*/
  if ( 0xFF == (index = ds_mux_get_idx_from_sio_port( port_id )) )
  {
    return ;
  }
  
  DS_MUX_ENTER_CRIT_SECTION(&global_ds_mux_crit_section);
    
  /*-------------------------------------------------------------------------
    Process SIO IOCTL
  -------------------------------------------------------------------------*/
  switch( cmd )
  {
    case SIO_IOCTL_ENABLE_DTR_EVENT_EXT:
    {
      if ( NULL != param_ptr )
      {
        if  ( param_ptr->enable_dte_ready_event_ext.cb_func != NULL )
        {
          ds_mux_logical_conn_info.logical_cons[index].ioctl_data.dtr_evt_info.cb_func =
          param_ptr->enable_dte_ready_event_ext.cb_func;
          
          ds_mux_logical_conn_info.logical_cons[index].ioctl_data.dtr_evt_info.cb_data =
          param_ptr->enable_dte_ready_event_ext.cb_data;
        }
        
        DS_MUX_LOG_MSG_INFO2_2( "ds_mux_ioctl ENABLE DTR EVT EXT cb 0x%x data 0x%x ",
          param_ptr->enable_dte_ready_event_ext.cb_func, 
          param_ptr->enable_dte_ready_event_ext.cb_data);
      }
      break;
    }
    case SIO_IOCTL_DISABLE_DTR_EVENT_EXT:
    {
      ds_mux_logical_conn_info.logical_cons[index].ioctl_data.dtr_evt_info.cb_func = NULL;
      ds_mux_logical_conn_info.logical_cons[index].ioctl_data.dtr_evt_info.cb_data = NULL;
      break;
    }

    case SIO_IOCTL_INBOUND_FLOW_DISABLE:
    {
      if ( TRUE != ds_mux_logical_conn_info.logical_cons[index].ioctl_data.in_bound_flow_disabled)
      {
        ds_mux_logical_conn_info.logical_cons[index].ioctl_data.in_bound_flow_disabled = TRUE;
        ds_mux_logical_relay_v24_sig(index);
      }
      break;
    }

    case SIO_IOCTL_INBOUND_FLOW_ENABLE:
    {
      if ( FALSE != ds_mux_logical_conn_info.logical_cons[index].ioctl_data.in_bound_flow_disabled )
      {
        ds_mux_logical_conn_info.logical_cons[index].ioctl_data.in_bound_flow_disabled = FALSE;
        ds_mux_logical_relay_v24_sig(index);
      }
      
      break;
    }

    case SIO_IOCTL_DTE_READY_ASSERTED:
    {
      if ( NULL != param_ptr )
      {
        /*--------------------------------------------------------------------
          RTC signal will be updated as part of MSC message processing received from peer
       -------------------------------------------------------------------------*/
        *(param_ptr->dte_ready_asserted) = 
            ds_mux_logical_conn_info.logical_cons[index].ioctl_data.remote_sig_state.rtc;
      }
      break;
    }
    
    case SIO_IOCTL_ENABLE_CD_EVENT_EXT:
    {
      if ( NULL != param_ptr )
      {
        if  ( param_ptr->enable_dte_ready_event_ext.cb_func != NULL )
        {
          ds_mux_logical_conn_info.logical_cons[index].ioctl_data.cd_evt_info.cb_func =
            param_ptr->enable_cd_ready_event_ext.cb_func;
            
          ds_mux_logical_conn_info.logical_cons[index].ioctl_data.cd_evt_info.cb_data =
            param_ptr->enable_cd_ready_event_ext.cb_data;
        }
          
        DS_MUX_LOG_MSG_INFO2_2( "ds_mux_ioctl ENABLE CD EVT EXT cb 0x%x data 0x%x ",
          param_ptr->enable_cd_ready_event_ext.cb_func, 
          param_ptr->enable_cd_ready_event_ext.cb_data);
      }
      break;
    }
    
    case SIO_IOCTL_DISABLE_CD_EVENT_EXT:
    {
      ds_mux_logical_conn_info.logical_cons[index].ioctl_data.cd_evt_info.cb_func = NULL;
      ds_mux_logical_conn_info.logical_cons[index].ioctl_data.cd_evt_info.cb_data = NULL;
      break;
    }

    case SIO_IOCTL_CD_ASSERT:
    {
      if (TRUE != ds_mux_logical_conn_info.logical_cons[index].ioctl_data.cd_asserted)
      {
        ds_mux_logical_conn_info.logical_cons[index].ioctl_data.cd_asserted = TRUE;
        ds_mux_logical_relay_v24_sig(index);
      }
      break;
    }

    case SIO_IOCTL_CD_DEASSERT:
    {
      if ( FALSE != ds_mux_logical_conn_info.logical_cons[index].ioctl_data.cd_asserted )
      {
        ds_mux_logical_conn_info.logical_cons[index].ioctl_data.cd_asserted = FALSE;
        ds_mux_logical_relay_v24_sig(index);
      }
      break;
    }

    case SIO_IOCTL_GET_CURRENT_CD:
    {
      if ( NULL != param_ptr  )
      {  
        /*--------------------------------------------------------------------
          Carrier Detect (CD) mapped to Data Valid (DV). signal will be updated as part of MSC
          message processing received from peer
          -------------------------------------------------------------------------*/
        *(param_ptr->cd_asserted) = ds_mux_logical_conn_info.logical_cons[index].ioctl_data.remote_sig_state.data_valid; 
      }
      break;
    }
    
    case SIO_IOCTL_DSR_ASSERT:
    {
      if ( TRUE != ds_mux_logical_conn_info.logical_cons[index].ioctl_data.rtc_asserted )
      {
        ds_mux_logical_conn_info.logical_cons[index].ioctl_data.rtc_asserted = TRUE;
        ds_mux_logical_relay_v24_sig(index);
      }
      break;
    }
    
    case SIO_IOCTL_DSR_DEASSERT:
    {
      if ( FALSE != ds_mux_logical_conn_info.logical_cons[index].ioctl_data.rtc_asserted )
      {
        ds_mux_logical_conn_info.logical_cons[index].ioctl_data.rtc_asserted = FALSE;
        ds_mux_logical_relay_v24_sig(index);
      }
      break;
    }
    
    case SIO_IOCTL_CHANGE_MODE:
    {
      if ( NULL != param_ptr  )
      {
        ds_mux_logical_conn_info.logical_cons[index].rx_queue = param_ptr->open_ptr->rx_queue;
        ds_mux_logical_conn_info.logical_cons[index].tx_queue = param_ptr->open_ptr->tx_queue;
      }
      break;
    }
    case SIO_IOCTL_GET_CURRENT_RI:
    {
      if ( NULL != param_ptr  )
      {  
        /*--------------------------------------------------------------------
          Incoming call indicator (IC).  mapped to RI. signal will be updated as part of MSC
          message processing received from peer
          -------------------------------------------------------------------------*/
        *(param_ptr->ri_asserted) = ds_mux_logical_conn_info.logical_cons[index].ioctl_data.remote_sig_state.ic; 
      }
    }
    break;
    case SIO_IOCTL_RI_ASSERT:
    {
      if (TRUE != ds_mux_logical_conn_info.logical_cons[index].ioctl_data.ri_asserted)
      {
        ds_mux_logical_conn_info.logical_cons[index].ioctl_data.ri_asserted = TRUE;
        ds_mux_logical_relay_v24_sig(index);
      }
      break;
    }
    
    case SIO_IOCTL_RI_DEASSERT:
    {
      if (FALSE != ds_mux_logical_conn_info.logical_cons[index].ioctl_data.ri_asserted)
      {
        ds_mux_logical_conn_info.logical_cons[index].ioctl_data.ri_asserted = FALSE;
        ds_mux_logical_relay_v24_sig(index);
      }
      break;
    }
    case SIO_IOCTL_ENABLE_RI_EVENT_EXT:
    {
      if ( NULL != param_ptr )
      {
        if  ( param_ptr->enable_ri_ready_event_ext.cb_func != NULL )
        {
          ds_mux_logical_conn_info.logical_cons[index].ioctl_data.ri_evt_info.cb_func =
            param_ptr->enable_ri_ready_event_ext.cb_func;
           
          ds_mux_logical_conn_info.logical_cons[index].ioctl_data.ri_evt_info.cb_data =
            param_ptr->enable_ri_ready_event_ext.cb_data;
        }
         
        DS_MUX_LOG_MSG_INFO2_2( "ds_mux_ioctl ENABLE RI EVT EXT cb 0x%x data 0x%x ",
          param_ptr->enable_ri_ready_event_ext.cb_func, 
          param_ptr->enable_ri_ready_event_ext.cb_data);
      }
    }
    break;

    case SIO_IOCTL_DISABLE_RI_EVENT_EXT:
    {
      ds_mux_logical_conn_info.logical_cons[index].ioctl_data.ri_evt_info.cb_func = NULL;
      ds_mux_logical_conn_info.logical_cons[index].ioctl_data.ri_evt_info.cb_data = NULL;
    }
    break;
    
    default :
      break;
  }

  DS_MUX_LEAVE_CRIT_SECTION(&global_ds_mux_crit_section);
    
} /* ds_mux_logical_drv_sio_ioctl() */

boolean  ds_mux_logical_is_transmission_allowed
(
  uint8              index,
  dsm_item_type     *tx_ptr
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (( (0xFF != index) && (index < DS_MUX_MAX_LOGICAL_CONNECTION)) &&
     (  DS_MUX_LOGICAL_PORT_OPEN  == ds_mux_logical_conn_info.logical_cons[index].port_status) &&
     (  NULL != ds_mux_logical_conn_info.logical_cons[index].tx_queue) &&
     (  NULL != tx_ptr && 0 != dsm_length_packet(tx_ptr) ))
  {
    return TRUE;
  }
  
  return FALSE;
}/* ds_mux_logical_is_transmission_allowed */
void ds_mux_logical_drv_sio_transmit
(
  sio_stream_id_type stream_id,
  sio_port_id_type   port_id,
  dsm_item_type     *tx_ptr
)
{
  uint8                      index = 0xFF;
  boolean                     is_transmission_allowed = FALSE;
  ds_mux_result_enum_type    result = DS_MUX_SUCCESS;
  dsm_item_type             **tx_ptr_ptr = &tx_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    This function will be invoked when Client call SIO Transmit on MUX ports. 

    Flow validation responsibility distributed as mentioned below 
    
    1. Logical Transmit Func  : - Verify logical connection status and other perform other validation on packet
    2. DS MUX Transmit Func :- Verify DLCI status corresponding to logical port
    3  DS MUX IO Transmit Func :- Verify Covergence layer, Prepare data frame and en-queue into client Tx queue
    4  DS MUX IO Tx Sig handler :- Verify flow status. if flow is enabled then only de-queue the pkt and transmit over physical
                                               otherwise keep packet into Client TX Queue
  -------------------------------------------------------------------------*/
  DS_MUX_ENTER_CRIT_SECTION(&global_ds_mux_crit_section);

  index = ds_mux_get_idx_from_sio_port( port_id );
    
  is_transmission_allowed = ds_mux_logical_is_transmission_allowed( index, tx_ptr);

  DS_MUX_LEAVE_CRIT_SECTION(&global_ds_mux_crit_section);

  if ( TRUE == is_transmission_allowed)
  {
    DS_MUX_LOG_APP_TX_PACKET(ds_mux_logical_conn_info.logical_cons[index].dlci_id, tx_ptr);
    result = ds_mux_io_transmit_data( ds_mux_logical_conn_info.logical_cons[index].dlci_id,
                                      ds_mux_logical_conn_info.logical_cons[index].ioctl_data.in_bound_flow_disabled,
                                      ds_mux_logical_conn_info.logical_cons[index].ioctl_data.rtc_asserted,
                                      ds_mux_logical_conn_info.logical_cons[index].ioctl_data.cd_asserted,
                                      tx_ptr_ptr);
  }
  else
  {
    result = DS_MUX_FAILURE;
  }
  /*-------------------------------------------------------------------------
    Couldn't transmit the data: Free DSM item
    -------------------------------------------------------------------------*/
  if ( DS_MUX_FAILURE == result && NULL != tx_ptr_ptr && NULL != *tx_ptr_ptr)
  {
    DS_MUX_LOG_MSG_ERROR_3( " ds_mux_logical_drv_sio_transmit couldn't transmit index %d "
                            " tx ptr %x  len  %d", index, tx_ptr, dsm_length_packet(tx_ptr)  );
    
    if ( index < DS_MUX_MAX_LOGICAL_CONNECTION )
    {
      DS_MUX_LOG_MSG_ERROR_2(" ds_mux_logical_drv_sio_transmit couldn't transmit port status %d "
                             " tx queue %x ", ds_mux_logical_conn_info.logical_cons[index].port_status,
                             ds_mux_logical_conn_info.logical_cons[index].tx_queue );
    }
    
    dsm_free_packet( tx_ptr_ptr );
  }
  
} /* ds_mux_logical_drv_sio_transmit() */

void ds_mux_logical_drv_sio_flush_transmit
(
  sio_stream_id_type stream_id,
  sio_port_id_type   port_id,
  void (*flush_func_ptr)(void)
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  uint8 index;

  index = ds_mux_get_idx_from_sio_port( port_id );
  /*-------------------------------------------------------------------------
    Not Supported
    -------------------------------------------------------------------------*/
  if ( 0xFF == index)
  {
    return ;
  }

} /* ds_mux_logical_drv_sio_flush_transmit()*/

uint32 ds_mux_logicali_driver_init
( 
  sio_register_device_driver_cb_type register_driver_cb_fn
)
{
  uint32                 ret_val = 1;
  uint8                  index   = 0;
  sio_device_interafce   device_iface;
  sio_driver_interface   driver_iface;
  uint32                 device_id;
  rdm_device_info_type   rdm_device_info;
  sio_port_id_type       sio_port_id[SIO_MINOR_DATA_MUX_MAX] = {
                                    SIO_PORT_DATA_MUX_1,
                                    SIO_PORT_DATA_MUX_2,
                                    SIO_PORT_DATA_MUX_3,
                                    SIO_PORT_DATA_MUX_4,
                                    SIO_PORT_DATA_MUX_5,
                                    SIO_PORT_DATA_MUX_6,
                                    SIO_PORT_DATA_MUX_7,
                                    SIO_PORT_DATA_MUX_8,
                                    SIO_PORT_DATA_MUX_9,
                                    SIO_PORT_DATA_MUX_10,
                                    SIO_PORT_DATA_MUX_11,
                                    SIO_PORT_DATA_MUX_12,
                                    SIO_PORT_DATA_MUX_13,
                                    SIO_PORT_DATA_MUX_14,
                                    SIO_PORT_DATA_MUX_15,
                                    SIO_PORT_DATA_MUX_16
                                  };
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( NULL == register_driver_cb_fn)
  {
    DS_MUX_LOG_MSG_ERROR_0( "ds_mux_logicali_driver_init: NULL register driver call back ");
    return 0;
  }
  /*-----------------------------------------------------------------------
    1.  Initialize logical interface layer
    2.  Initialize driver and device interface 
    3.  Register devices with SIO
         ( Invoke register device call back because Sio task is higher prioirty task and DS MUX may not initialize by then)
    4. Register devices with RDM as well
    -----------------------------------------------------------------------*/
  /* Step 1*/
  ds_mux_logical_init();

  /* Step 2:  Driver Interface */
  driver_iface.HW_open              = ds_mux_logical_drv_sio_open;
  driver_iface.HW_close             = ds_mux_logical_drv_sio_close;
  driver_iface.HW_ioctl             = ds_mux_logical_drv_sio_ioctl;
  driver_iface.HW_transmit          = ds_mux_logical_drv_sio_transmit; 
  driver_iface.HW_flush_tx          = ds_mux_logical_drv_sio_flush_transmit;
  driver_iface.HW_disable_device    = NULL;
  //Control function pointers are same as regular
  driver_iface.HW_control_open      = ds_mux_logical_drv_sio_open;
  driver_iface.HW_control_close     = ds_mux_logical_drv_sio_close;
  driver_iface.HW_control_ioctl     = ds_mux_logical_drv_sio_ioctl;
  driver_iface.HW_control_transmit  = ds_mux_logical_drv_sio_transmit;

  /* Step 2: Device Interface */
  device_iface.driver_interface     = &driver_iface;
  device_iface.forward_port         = FALSE;
  device_iface.forward_port_id      = SIO_PORT_NULL;
  device_iface.driver_available     = TRUE;
  
  for ( index = 0; index < DS_MUX_MAX_LOGICAL_CONNECTION; index++ )
  {
    /* Step 3: Register with SIO */
    device_id              = SIO_MINOR_DATA_MUX_1 + index;
    
    device_iface.device_id = DEVICE_ID(SIO_MAJOR_DATA_MUX, device_id );

    if ( register_driver_cb_fn ( &device_iface ) == 0 )
    {
      DS_MUX_LOG_MSG_ERROR_1( " Error in registering the device:%d",
                    device_iface.device_id);
      continue;
    }
    /* Update Device ID */
    ds_mux_logical_conn_info.logical_cons[index].dlci_id = device_id;
    /* Update SIO Port ID */
    ds_mux_logical_conn_info.logical_cons[index].port_id = (sio_port_id_type) device_iface.device_id;
        
    /* Step 4: Register with RDM */
    rdm_device_info.device = (rdm_device_enum_type)(DS_MUX_BASE_LOGICAL_RDM_DEVICE + index);
    
    snprintf(&rdm_device_info.device_name[0], 
             RDM_MAX_DEV_NAME_LEN, "RDM_MUX_DEV_%d", index);
    
    rdm_device_info.available = TRUE;
    rdm_device_info.sio_port  = sio_port_id[index];
    rdm_register_device(&rdm_device_info);
  }

#if 0
  /* Step 2 */ //Can be moved to logical driver init
  ds_mux_logical_notify_port_open();
#endif

  return ret_val;
}/* ds_mux_logicali_driver_init */

static void ds_mux_logical_rdm_assign_port_cb
(
  rdm_assign_status_type status,
  rdm_service_enum_type  service,
  rdm_device_enum_type   device
)
{
  boolean    retry_rdm_oprt = FALSE;
  uint8      inst           = 0xFF;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  DS_MUX_LOG_MSG_INFO2_3( "ds_mux_logical_rdm_assign_port_cb status %d device %d service %d",
    status, service, device);

  if(0xFF == (inst = ds_mux_get_idx_from_rdm_dev(device)) )
  {
    return;
  }
  /*-----------------------------------------------------------------------
    Retry port assignment again after sometimes if RDM is busy. 
    Otherwise ignore and "don't retry" in case of success(RDM_DONE_S) or other failures 
    (RDM_NOT_ALLOWED_S, RDM_RESTRICTED_S, RDM_APP_BUSY_S)
    -----------------------------------------------------------------------*/
  if ( RDM_DEVMAP_BUSY_S == status )
  {
    if( ds_mux_logical_conn_info.logical_cons[inst].rdm_oprt_retry  < DS_MUX_MAX_RDM_OPR_RETRY)
    {
      retry_rdm_oprt = TRUE;
      ds_mux_logical_conn_info.logical_cons[inst].rdm_oprt_retry++;
      
      if( DS_MUX_TIMER_FAILURE == ds_mux_timer_start( ds_mux_logical_conn_info.logical_cons[inst].timer_handle,
                                          &ds_mux_logical_conn_info.logical_cons[inst].timer_attr,
                                          DS_MUX_RDM_OPR_RETRY_TIMER ) )
      {
        retry_rdm_oprt = FALSE;
      }
      
    }
  }
  else if ( RDM_DONE_S == status)
  {
    ds_mux_logical_conn_info.logical_cons[inst].port_status = 
        DS_MUX_LOGICAL_PORT_ASSIGNED;
  }
  else
  {
     ds_mux_logical_conn_info.logical_cons[inst].port_status = 
        DS_MUX_LOGICAL_PORT_MIN;
  }
  if (FALSE == retry_rdm_oprt)
  {
    if ( DS_MUX_TIMER_INVALID_HANDLE != ds_mux_logical_conn_info.logical_cons[inst].timer_handle)
    {
      (void)ds_mux_timer_free( ds_mux_logical_conn_info.logical_cons[inst].timer_handle );
      
      ds_mux_logical_conn_info.logical_cons[inst].rdm_oprt_retry = 0;
      ds_mux_logical_conn_info.logical_cons[inst].timer_handle   = DS_MUX_TIMER_INVALID_HANDLE;
    }
  }
  
  DS_MUX_LOG_MSG_INFO2_3( "ds_mux_logical_rdm_assign_port_cb retry flag %d "
                          " rdm operation retried %d timer handle %d",
                          retry_rdm_oprt, 
                          ds_mux_logical_conn_info.logical_cons[inst].rdm_oprt_retry ,
                          ds_mux_logical_conn_info.logical_cons[inst].timer_handle);

}/* ds_mux_logical_rdm_assign_port_cb */

static void ds_mux_logical_rdm_oprt_timer_cb
(
  void *  data
)
{
  boolean    result = FALSE;
  uint32      inst  = (uint32 ) data;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( inst < DS_MUX_MAX_LOGICAL_CONNECTION )
  {
    result = rdm_assign_port( ds_mux_logical_conn_info.logical_cons[inst].service,
                              ds_mux_logical_conn_info.logical_cons[inst].rdm_device,
                              ds_mux_logical_rdm_assign_port_cb );
  }
  DS_MUX_LOG_MSG_INFO2_2( "ds_mux_logical_rdm_oprt_timer_cb instance %d result %d", inst, result);
  
}/* ds_mux_logical_rdm_oprt_timer_cb */

void ds_mux_logical_notify_port_open
(

)
{
  uint32   inst = 0;
  boolean  res = FALSE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
/*-----------------------------------------------------------------------
  1.  Reset the retry counter
  2.  Allocate timer for retry
  3.  Set compatibility and assign the device to servce so that clients can get
       RDM open call back function
  -----------------------------------------------------------------------*/ 

  for (inst = 0; inst < DS_MUX_MAX_LOGICAL_CONNECTION; inst++) 
  {

    ds_mux_logical_conn_info.logical_cons[inst].rdm_oprt_retry = 0;

    ds_mux_timer_alloc( &ds_mux_logical_conn_info.logical_cons[inst].timer_handle,
                        &ds_mux_logical_conn_info.logical_cons[inst].timer_attr,
                        ds_mux_logical_rdm_oprt_timer_cb, 
                       (void *)inst );
  
    rdm_set_compatibility( ds_mux_logical_conn_info.logical_cons[inst].service,
                           ds_mux_logical_conn_info.logical_cons[inst].rdm_device);

    ds_mux_logical_conn_info.logical_cons[inst].port_status = 
      DS_MUX_LOGICAL_PORT_ASSIGN;

    res = rdm_assign_port( ds_mux_logical_conn_info.logical_cons[inst].service,
                           ds_mux_logical_conn_info.logical_cons[inst].rdm_device,
                           ds_mux_logical_rdm_assign_port_cb );
    if (FALSE == res)
    {
      ds_mux_logical_conn_info.logical_cons[inst].port_status = 
        DS_MUX_LOGICAL_PORT_MIN;
      ds_mux_timer_free(ds_mux_logical_conn_info.logical_cons[inst].timer_handle);
    }
    
    DS_MUX_LOG_MSG_INFO2_3( "rdm_assign_port result %d timer handle %d inst %d",
        res, ds_mux_logical_conn_info.logical_cons[inst].timer_handle, inst);
    
  }
}/* ds_mux_logical_notify_port_open */

ds_mux_result_enum_type ds_mux_logical_handle_remote_fc_enable
(
  uint8                         dlci_id
)
{
  ds_mux_result_enum_type       result          = DS_MUX_SUCCESS;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  /*-----------------------------------------------------------------------
    This Function will be invoked when remote mux/host send flow control enable in MSC message or 
    flow control enable in first Byte of data(CL- 2.). 
    Bit 2.Flow Control (FC). The bit is set to 1(one) when the device is unable to accept frames.
    -----------------------------------------------------------------------*/ 
  ds_mux_disable_flow_mask( dlci_id, DS_MUX_FLOW_DISABLE_MSC );

  return result;
}/* ds_mux_logical_handle_remote_fc_enable */


ds_mux_result_enum_type ds_mux_logical_handle_remote_fc_disable
(
  uint8                         dlci_id
)
{
  ds_mux_result_enum_type       result          = DS_MUX_SUCCESS;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  /*-------------------------------------------------------------------------
    This Function will be invoked when remote mux/host send flow controldisaable in MSC message or 
    flow control disable in first Byte of data(CL- 2.). 
    Bit 2.Flow Control (FC). The bit is set to 1(one) when the device is unable to accept frames.
    -------------------------------------------------------------------------*/
  ds_mux_enable_flow_mask( dlci_id, DS_MUX_FLOW_DISABLE_MSC );
  
  return result;
}/* ds_mux_logical_handle_remote_fc_disable */


ds_mux_result_enum_type  ds_mux_logical_process_v24_signal
(
  uint8                  dlci_id,
  uint8                  v24_sig
)
{
    
  ds_mux_result_enum_type       result          = DS_MUX_SUCCESS;
  boolean                       recv_enable_fc  = FALSE;
  boolean                       recv_enable_rtc = FALSE;
  boolean                       recv_enable_dv  = FALSE;
  boolean                       recv_enable_ic  = FALSE;

  boolean                       notify_dtr_change = FALSE;
  boolean                       notify_cd_change  = FALSE;
  boolean                       notify_ri_change  = FALSE;

  uint8                         index = 0xFF;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    1.  Parse V24 signal byte and check signal bit
    2.  If signal changed then update the signal status and invoke client call back (if registered  )

        Supported Bits:- 
        
        Bit 2.Flow Control (FC). The bit is set to 1(one) when the device is unable to accept frames.
        Bit 3. Ready To Communicate (RTC). The bit is set to 1 when the device is ready to communicate.
        Bit 7. Incoming call indicator (IC). The bit is set to 1 to indicate an incoming call.
        Bit 8. Data Valid (DV). The bit is set to 1 to indicate that valid data is being sent
    -----------------------------------------------------------------------*/
  do
  {
    if ( 0 == (v24_sig & DS_MUX_DLCI_MSG_TYPE_EA_MASK))
     DS_MUX_SET_ERROR_AND_BREAK(result)

    index = ds_mux_get_idx_from_dlci_id( dlci_id );
        
     if ( 0xFF == index )
      DS_MUX_SET_ERROR_AND_BREAK(result)
      
    /* Step 1 */
    if ( (v24_sig & DS_MUX_DLCI_MSC_FC_MASK) )
    {
      recv_enable_fc = TRUE;
    }
    if ( (v24_sig & DS_MUX_DLCI_MSC_RTC_MASK) )
    {
      recv_enable_rtc = TRUE;
    }
    if ( (v24_sig & DS_MUX_DLCI_MSC_IC_MASK) )
    {
      recv_enable_ic = TRUE;
    }
    if ( (v24_sig & DS_MUX_DLCI_MSC_DV_MASK) )
    {
      recv_enable_dv = TRUE;
    }
    /* Step 2 */

    DS_MUX_ENTER_CRIT_SECTION(&global_ds_mux_crit_section);
    
    /*-----------------------------------------------------------------------
      DTR changed
    -----------------------------------------------------------------------*/
    if ( recv_enable_rtc != ds_mux_logical_conn_info.logical_cons[index].ioctl_data.remote_sig_state.rtc )
    {
      ds_mux_logical_conn_info.logical_cons[index].ioctl_data.remote_sig_state.rtc = recv_enable_rtc;
      notify_dtr_change = TRUE;
      
    }
    /*-----------------------------------------------------------------------
      CD or DV changed
    -----------------------------------------------------------------------*/
    if ( recv_enable_dv != ds_mux_logical_conn_info.logical_cons[index].ioctl_data.remote_sig_state.data_valid )
    {
      ds_mux_logical_conn_info.logical_cons[index].ioctl_data.remote_sig_state.data_valid = recv_enable_dv;
      notify_cd_change = TRUE;
      }
    /*-----------------------------------------------------------------------
      RI changed
    -----------------------------------------------------------------------*/
    if ( recv_enable_ic != ds_mux_logical_conn_info.logical_cons[index].ioctl_data.remote_sig_state.ic )
    {
      ds_mux_logical_conn_info.logical_cons[index].ioctl_data.remote_sig_state.ic = recv_enable_ic;
      notify_ri_change = TRUE;
    }

  /*-----------------------------------------------------------------------
      FC changed
    -----------------------------------------------------------------------*/
    if ( recv_enable_fc != ds_mux_logical_conn_info.logical_cons[index].ioctl_data.remote_sig_state.fc )
    {
      if ( TRUE == recv_enable_fc)
      {
        ds_mux_logical_handle_remote_fc_enable( dlci_id);
      }
      else
      {
        ds_mux_logical_handle_remote_fc_disable( dlci_id);
      }
      ds_mux_logical_conn_info.logical_cons[index].ioctl_data.remote_sig_state.fc = recv_enable_fc;
    }
    
    DS_MUX_LEAVE_CRIT_SECTION(&global_ds_mux_crit_section);

    /*-----------------------------------------------------------------------
       Invoke call back after critical section, client may enter into their critical section in call back
       Avoid dead lock
      -----------------------------------------------------------------------*/
    if ( TRUE == notify_dtr_change)
    {
      if ( NULL != ds_mux_logical_conn_info.logical_cons[index].ioctl_data.dtr_evt_info.cb_func)
      {
        ds_mux_logical_conn_info.logical_cons[index].ioctl_data.dtr_evt_info.cb_func (
          ds_mux_logical_conn_info.logical_cons[index].ioctl_data.dtr_evt_info.cb_data );
      }
    }

    if ( TRUE == notify_cd_change)
    {
      if ( NULL != ds_mux_logical_conn_info.logical_cons[index].ioctl_data.cd_evt_info.cb_func)
      {
        ds_mux_logical_conn_info.logical_cons[index].ioctl_data.cd_evt_info.cb_func (
          ds_mux_logical_conn_info.logical_cons[index].ioctl_data.cd_evt_info.cb_data );
      }
    }

    if ( TRUE == notify_ri_change)
    {
      if ( NULL != ds_mux_logical_conn_info.logical_cons[index].ioctl_data.ri_evt_info.cb_func)
      {
        ds_mux_logical_conn_info.logical_cons[index].ioctl_data.ri_evt_info.cb_func (
          ds_mux_logical_conn_info.logical_cons[index].ioctl_data.ri_evt_info.cb_data );
      }
    }
    /*-----------------------------------------------------------------------
       Avoid printing multiple control F3s. In case of CL 2. every data packet has control byte.
       printing only when some thing has changed
      -----------------------------------------------------------------------*/
    if( TRUE == notify_dtr_change || TRUE == notify_cd_change || TRUE == notify_ri_change)
    {
      DS_MUX_LOG_MSG_INFO2_6( " ds_mux_logical_process_v24_signal changed  "
                              " index %d  dlci Id %d cd %d rtc %d fc %d ic %d", index, 
                                ds_mux_logical_conn_info.logical_cons[index].dlci_id,
                                recv_enable_dv,
                                recv_enable_rtc,
                                recv_enable_fc,
                                recv_enable_ic);
      DS_MUX_LOG_MSG_INFO2_3( " ds_mux_logical_process_v24_signal  changed  "
                              " DTR change %d  CD change %d RI change %d",
                                notify_dtr_change, 
                                notify_cd_change,
                                notify_ri_change);
    }
    
  }while(0);
 
  return result;
}/*ds_mux_logical_process_v24_signal*/


ds_mux_result_enum_type ds_mux_logical_input_rx_data
(
  uint8                  dlci_id,
  dsm_item_type         *item_ptr
)
{
  ds_mux_result_enum_type       result = DS_MUX_SUCCESS;
  uint8                         index = 0xFF;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  index = ds_mux_get_idx_from_dlci_id( dlci_id );
  
  /*-----------------------------------------------------------------------
    1.  Validate the DSM item
    2.  En-queue the packet into client RX queue.
  -----------------------------------------------------------------------*/
  if ( 0xFF == index || NULL == item_ptr || 0 == dsm_length_packet(item_ptr))
  {
    DS_MUX_LOG_MSG_ERROR_2( "ds_mux_logical_input_rx_data input failed index %d item_ptr %x",
      index, item_ptr);
        
    if ( NULL != item_ptr)
    {
      dsm_free_packet(&item_ptr);
    }
    return DS_MUX_FAILURE;
  }
  /*Step 2*/
  DS_MUX_ENTER_CRIT_SECTION(&global_ds_mux_crit_section);
  
  if ( NULL != ds_mux_logical_conn_info.logical_cons[index].rx_queue &&
       DS_MUX_LOGICAL_PORT_OPEN  == ds_mux_logical_conn_info.logical_cons[index].port_status )
  {

    DS_MUX_LEAVE_CRIT_SECTION(&global_ds_mux_crit_section);
   
    DS_MUX_LOG_APP_RX_PACKET(dlci_id, item_ptr);

    dsm_enqueue(ds_mux_logical_conn_info.logical_cons[index].rx_queue, &item_ptr);
  }
  else
  {
    DS_MUX_LEAVE_CRIT_SECTION(&global_ds_mux_crit_section);
    
   DS_MUX_LOG_MSG_ERROR_2( "ds_mux_logical_input_rx_data input failed rx queue %x port status %d",
    ds_mux_logical_conn_info.logical_cons[index].rx_queue, 
    ds_mux_logical_conn_info.logical_cons[index].port_status);
      
   dsm_free_packet(&item_ptr);
   result = DS_MUX_FAILURE;
  }

  return result;
}/*ds_mux_logical_input_rx_data*/


ds_mux_result_enum_type ds_mux_logical_process_rx_data_frames
(
  ds_mux_io_frame_type    *frame
)
{

  ds_mux_result_enum_type       result      = DS_MUX_SUCCESS;
  ds_mux_cl_type_enum_type      cl_type     = DS_MUX_CL_TYPE_INVALID;
  dsm_item_type**               rx_data_ptr;
  uint8                         cl2_v24_val;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( NULL == frame || 
       NULL == frame->information_ptr ||
       0 == dsm_length_packet(frame->information_ptr) )
  {
     return DS_MUX_FAILURE;
  }
  /*-----------------------------------------------------------------------
    1 Get CL type
      1.1 If CL is type 2 then process signal byte and remove it from the data
    2 Enque the data into Rx WM
  -----------------------------------------------------------------------*/ 
  rx_data_ptr = &frame->information_ptr;
  
  cl_type = ds_mux_get_cl_type( frame->dlci );
  
  if ( DS_MUX_CL_TYPE_2 == cl_type)
  {
    if ( 1 != dsm_pullup(rx_data_ptr, &cl2_v24_val, 1))
    {
      DS_MUX_LOG_MSG_ERROR_0( "ds_mux_logical_process_rx_data_frames unable to pull up signal octet ");
      dsm_free_packet(rx_data_ptr);
      return DS_MUX_FAILURE;
    }
    
    ds_mux_logical_process_v24_signal( frame->dlci, cl2_v24_val);
    
  }
  
  result = ds_mux_logical_input_rx_data( frame->dlci, *rx_data_ptr);
 
  return result;
}/* ds_mux_logical_process_rx_data_frames*/


void ds_mux_logical_notify_disconneted_dlci 
(
  uint8                   dlci_id
)
{
  uint8                      index = 0xFF;
  dsm_item_type            * item_ptr;
  dsm_watermark_type       * tx_wm;
  boolean                    notify_dtr_change = FALSE;
  boolean                    notify_cd_change  = FALSE;
  boolean                    notify_ri_change  = FALSE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_MUX_LOG_MSG_INFO2_1( "ds_mux_logical_notify_disconneted_dlci dlci ID %d ", dlci_id);
  
  /*-----------------------------------------------------------------------
    1 Port status remain same
    2  Reset all remote signals
      2.1 Notify CD DE_ASSERTED to the client
      2.2 Notify DTR LOW to the client
    3.Flush TX watermark only. 
  -----------------------------------------------------------------------*/ 
  if (0xFF != ( index = ds_mux_get_idx_from_dlci_id( dlci_id )))
  {
      
    DS_MUX_ENTER_CRIT_SECTION(&global_ds_mux_crit_section);
  
    if ( FALSE != ds_mux_logical_conn_info.logical_cons[index].ioctl_data.remote_sig_state.data_valid)
    {
      ds_mux_logical_conn_info.logical_cons[index].ioctl_data.remote_sig_state.data_valid = FALSE;
      notify_cd_change = TRUE;
    }
    
    if ( FALSE != ds_mux_logical_conn_info.logical_cons[index].ioctl_data.remote_sig_state.rtc)
    {
      ds_mux_logical_conn_info.logical_cons[index].ioctl_data.remote_sig_state.rtc = FALSE;
      notify_dtr_change = TRUE;
    }

    if ( FALSE != ds_mux_logical_conn_info.logical_cons[index].ioctl_data.remote_sig_state.ic)
      {
      ds_mux_logical_conn_info.logical_cons[index].ioctl_data.remote_sig_state.ic = FALSE;
      notify_ri_change = TRUE;
    }

    ds_mux_logical_conn_info.logical_cons[index].ioctl_data.remote_sig_state.fc = FALSE;
    
    /*Step 3*/
     tx_wm = ds_mux_logical_conn_info.logical_cons[index].tx_queue;

    DS_MUX_LEAVE_CRIT_SECTION(&global_ds_mux_crit_section);
    
    while (( NULL != tx_wm) && ( NULL != (item_ptr = dsm_dequeue( tx_wm ))))
    {
      (void)dsm_free_packet(&item_ptr);
    }

    /*-----------------------------------------------------------------------
       Invoke call back after critical section, client may enter into their critical section in call back
       Avoid dead lock
      -----------------------------------------------------------------------*/
    if ( TRUE == notify_dtr_change)
    {
      if ( NULL != ds_mux_logical_conn_info.logical_cons[index].ioctl_data.dtr_evt_info.cb_func)
      {
        ds_mux_logical_conn_info.logical_cons[index].ioctl_data.dtr_evt_info.cb_func (
          ds_mux_logical_conn_info.logical_cons[index].ioctl_data.dtr_evt_info.cb_data );
      }
    }
    
    if ( TRUE == notify_cd_change)
    {
      if ( NULL != ds_mux_logical_conn_info.logical_cons[index].ioctl_data.cd_evt_info.cb_func)
      {
        ds_mux_logical_conn_info.logical_cons[index].ioctl_data.cd_evt_info.cb_func (
          ds_mux_logical_conn_info.logical_cons[index].ioctl_data.cd_evt_info.cb_data );
      }
    }
    
    if ( TRUE == notify_ri_change)
    {
      if ( NULL != ds_mux_logical_conn_info.logical_cons[index].ioctl_data.ri_evt_info.cb_func)
      {
        ds_mux_logical_conn_info.logical_cons[index].ioctl_data.ri_evt_info.cb_func (
          ds_mux_logical_conn_info.logical_cons[index].ioctl_data.ri_evt_info.cb_data );
      }
    }
  }
  
}/* ds_mux_logical_notify_disconneted_dlci*/

void ds_mux_logical_notify_conneted_dlci 
(
  uint8                   dlci_id
)
{
    uint8                      index = 0xFF;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_MUX_LOG_MSG_INFO2_1( "ds_mux_logical_notify_conneted_dlci dlci ID %d ", dlci_id);
  
  /*-----------------------------------------------------------------------
    1 We will wait for MSC messgae for DTR high update to logical client
    2.There may be chance where Application has already opened the port and
       setup v24 signals. Check if Any v24 signal are set then send MSC message on
       behalf on application
  -----------------------------------------------------------------------*/ 
  if ( 0xFF != ( index = ds_mux_get_idx_from_dlci_id( dlci_id )) )
  {
    if ( TRUE == ds_mux_logical_conn_info.logical_cons[index].ioctl_data.cd_asserted ||
         TRUE == ds_mux_logical_conn_info.logical_cons[index].ioctl_data.rtc_asserted ||
         FALSE == ds_mux_logical_conn_info.logical_cons[index].ioctl_data.in_bound_flow_disabled)
  {
      ds_mux_logical_relay_v24_sig( index );
    }
  }
  
}/* ds_mux_logical_notify_conneted_dlci*/

dsm_watermark_type*      ds_mux_logical_get_tx_wmk
(
  uint8                   dlci_id
)
{
  dsm_watermark_type*     tx_wm = NULL;
  uint8                   index = 0xFF;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0xFF != ( index = ds_mux_get_idx_from_dlci_id( dlci_id )))
  {
    tx_wm = ds_mux_logical_conn_info.logical_cons[index].tx_queue ; 
  }

  return tx_wm;
}


dsm_watermark_type*      ds_mux_logical_get_rx_wmk
(
  uint8                   dlci_id
)
{
  dsm_watermark_type*     rx_wm = NULL;
  uint8                   index = 0xFF;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0xFF != ( index = ds_mux_get_idx_from_dlci_id( dlci_id )))
  {
    rx_wm = ds_mux_logical_conn_info.logical_cons[index].rx_queue ; 
  }

  return rx_wm;
}/* ds_mux_logical_get_tx_wmk*/

dsm_item_type* ds_mux_logical_get_dsm_item_from_wmk
(
  uint8                   dlci_id
)
{
  dsm_watermark_type     *rx_wm = NULL;
  dsm_item_type          *dsm_item_ptr = NULL;
  
  rx_wm = ds_mux_logical_get_rx_wmk(dlci_id);

  dsm_item_ptr = dsm_dequeue(rx_wm);

  return dsm_item_ptr;
}




