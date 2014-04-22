/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     D S   M U X   T A S K

GENERAL DESCRIPTION
  This file contains all the functions, definitions and data types needed 
  for other tasks to interface to the MUX Task.

  Signal and command definitions for the MUX Task, and functions
  that should be used by other tasks to send commands to the MUX 
  Task are defined here.

  Copyright (c) 2016 by Qualcomm Technologies, Incorporated.  All Rights Reserved.  
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/dataservices.tx/6.0/tethering/qti_bridge/src/ds_mux_stubs.c#1 $ 
  $DateTime: 2019/11/26 00:18:17 $ 
  $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/01/16   ad     Initial version
===========================================================================*/


/*===========================================================================

                      INCLUDE FILES

===========================================================================*/

#ifdef TEST_FRAMEWORK

#include "ds_mux_phys.h"
#include "ds_mux.h"
#include "ds_mux_api.h"
#include "ds_mux_stub.h"


rex_stack_word_type ds_mux_stack [DS_MUX_STACK_SIZ];
rex_tcb_type        ds_mux_tcb;

void (*test_open_func_ptr_type) (sio_stream_id_type) ;
void (*test_close_func_ptr_type)(sio_stream_id_type);
void (*test_dtr_func_ptr_type)  (sio_stream_id_type, boolean);
void (*test_non_emp_wm_func_ptr_type)(sio_stream_id_type, dsm_watermark_type  *);

static ds_mux_stub_logical_conn_info  ds_mux_stub_log_conn_info;
static dsm_watermark_type             ds_mux_stub_tx_wmk[DS_MUX_MAX_LOGICAL_CONNECTION];
/* Transmit Watermark, to write to the SIO Port */
static q_type                         ds_mux_stub_tx_q[DS_MUX_MAX_LOGICAL_CONNECTION];
/* Queue for the tx_wmk */
static dsm_watermark_type             ds_mux_stub_rx_wmk[DS_MUX_MAX_LOGICAL_CONNECTION];
/* Receive Watermark, to write to the SIO Port */
static q_type                         ds_mux_stub_rx_q[DS_MUX_MAX_LOGICAL_CONNECTION];
/* Queue for the tx_wmk */
static boolean                        ds_mux_stub_dte_ready_asserted[DS_MUX_MAX_LOGICAL_CONNECTION];

static uint8                          ds_mux_stub_curr_stream_id_index = 0;

sio_open_type                         phys_port_open_param;

typedef struct
{
  uint8                     total_serv;
  rdm_service_enum_type     service_type[DS_MUX_MAX_SERV];
  rdm_service_params_type   service_params[DS_MUX_MAX_SERV];
}ds_mux_stub_registered_rdm_srv_info;

typedef struct
{
  uint8                     total_device;
  rdm_device_enum_type      device[DS_MUX_MAX_SERV];
  rdm_service_enum_type     service_type[DS_MUX_MAX_SERV];
  sio_port_id_type          sio_port[DS_MUX_MAX_SERV];
}ds_mux_stub_registered_rdm_device_info;

typedef struct
{
  uint8                     total_sio_dev;
  uint32                    device_id[DS_MUX_MAX_SERV]; 
  sio_stream_id_type        stream_id[DS_MUX_MAX_SERV];
  sio_driver_interface      driver_interface[DS_MUX_MAX_SERV];

}ds_mux_stub_registered_sio_device_info;


ds_mux_stub_registered_rdm_srv_info        ds_mux_reged_rdm_srv;
ds_mux_stub_registered_rdm_device_info     ds_mux_reged_rdm_dev;
ds_mux_stub_registered_sio_device_info     ds_mux_reged_sio_dev;

//Function pointer those testcases can register

uint32 ds_mux_stub_sio_register_device_driver
(
  sio_device_interafce * device_iface_ptr
)
{

  uint8  index = ds_mux_reged_sio_dev.total_sio_dev;
  uint32 res = 1;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( index < DS_MUX_MAX_SERV )
  {
    ds_mux_reged_sio_dev.device_id[index] = device_iface_ptr->device_id;

    ds_mux_reged_sio_dev.stream_id[index] = (sio_stream_id_type) device_iface_ptr->device_id;
    
    memscpy(&ds_mux_reged_sio_dev.driver_interface[index],
        sizeof(sio_driver_interface), 
        device_iface_ptr->driver_interface, 
        sizeof(sio_driver_interface));

    ds_mux_reged_sio_dev.total_sio_dev++;
  }
  return res;
}/* ds_mux_stub_sio_register_device_driver */

void  ds_mux_stub_reg_physical_sio
(
  sio_device_interafce * device_iface_ptr
)
{
  ds_mux_stub_sio_register_device_driver(device_iface_ptr);
}/* ds_mux_stub_reg_physical_sio */

static sio_status_type ds_mux_stub_phy_drv_sio_open
(
  sio_open_type *open_ptr
)
{
  memscpy(&phys_port_open_param, sizeof(sio_open_type), open_ptr, sizeof(sio_open_type));
  
  return SIO_DONE_S;

}/* ds_mux_stub_phy_drv_sio_open */

void ds_mux_stub_phy_drv_sio_close
(
  sio_stream_id_type   stream_id,
  sio_port_id_type     port_id,
  void                (*close_func_ptr)(void) 
)
{

}/* ds_mux_stub_phy_drv_sio_close */

void ds_mux_stub_phy_drv_sio_ioctl 
(
  sio_stream_id_type    stream_id,
  sio_port_id_type      port_id,
  sio_ioctl_cmd_type    cmd,
  sio_ioctl_param_type *param_ptr
)
{
    
  if (SIO_IOCTL_DTE_READY_ASSERTED == cmd )
  {
    *param_ptr->dte_ready_asserted = TRUE;
  }

}/*ds_mux_stub_phy_drv_sio_ioctl */

void ds_mux_stub_phy_drv_sio_transmit
(
  sio_stream_id_type stream_id,
  sio_port_id_type   port_id,
  dsm_item_type     *tx_ptr
)
{
}/* ds_mux_stub_phy_drv_sio_transmit */

void ds_mux_stub_phy_drv_sio_flush_transmit
(
  sio_stream_id_type stream_id,
  sio_port_id_type   port_id,
  void (*flush_func_ptr)(void)
)
{

}/*ds_mux_stub_phy_drv_sio_flush_transmit*/

static void register_phy_sio_drv
(
  sio_driver_interface       *test_phy_driver_iface
)
{
  sio_device_interafce                   device_iface;
  sio_driver_interface                   driver_iface;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  //Set default values
  driver_iface.HW_open              = ds_mux_stub_phy_drv_sio_open;
  driver_iface.HW_close             = ds_mux_stub_phy_drv_sio_close;
  driver_iface.HW_ioctl             = ds_mux_stub_phy_drv_sio_ioctl;
  driver_iface.HW_transmit          = ds_mux_stub_phy_drv_sio_transmit; 
  driver_iface.HW_flush_tx          = ds_mux_stub_phy_drv_sio_flush_transmit;
  driver_iface.HW_disable_device    = NULL;
  //Control function pointers are same as regular
  driver_iface.HW_control_open      = ds_mux_stub_phy_drv_sio_open;
  driver_iface.HW_control_close     = ds_mux_stub_phy_drv_sio_close;
  driver_iface.HW_control_ioctl     = ds_mux_stub_phy_drv_sio_ioctl;
  driver_iface.HW_control_transmit  = ds_mux_stub_phy_drv_sio_transmit;

  //Overwrite with specific function pointer whatever testcase want to set
  if ( NULL != test_phy_driver_iface && NULL != test_phy_driver_iface->HW_open)
  {
   driver_iface.HW_open              = test_phy_driver_iface->HW_open;
   driver_iface.HW_control_open      = test_phy_driver_iface->HW_open;
  }

  if ( NULL != test_phy_driver_iface && NULL != test_phy_driver_iface->HW_close)
  {
   driver_iface.HW_close              = test_phy_driver_iface->HW_close;
   driver_iface.HW_control_close      = test_phy_driver_iface->HW_close;
  }

  if ( NULL != test_phy_driver_iface && NULL != test_phy_driver_iface->HW_ioctl)
  {
   driver_iface.HW_ioctl              = test_phy_driver_iface->HW_ioctl;
   driver_iface.HW_control_ioctl      = test_phy_driver_iface->HW_ioctl;
  }

  if ( NULL != test_phy_driver_iface && NULL != test_phy_driver_iface->HW_transmit)
  {
   driver_iface.HW_transmit              = test_phy_driver_iface->HW_transmit;
   driver_iface.HW_control_transmit      = test_phy_driver_iface->HW_transmit;
  }

  if ( NULL != test_phy_driver_iface && NULL != test_phy_driver_iface->HW_flush_tx)
  {
   driver_iface.HW_flush_tx              = test_phy_driver_iface->HW_flush_tx;
  }


  /* Step 2: Device Interface */
  device_iface.driver_interface     = &driver_iface;
  device_iface.forward_port         = FALSE;
  device_iface.forward_port_id      = SIO_PORT_NULL;
  device_iface.driver_available     = TRUE;
  device_iface.device_id            = DS_MUX_PHY_DEV;

  ds_mux_stub_reg_physical_sio( &device_iface);

}/* register_phy_sio_drv */



void ds_mux_stub_init
(
  boolean                 is_static_init,
  sio_driver_interface    *test_phy_driver_iface
)
{
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  memset( &ds_mux_reged_rdm_srv, 0, sizeof(ds_mux_stub_registered_rdm_srv_info));
  memset( &ds_mux_reged_rdm_dev, 0, sizeof(ds_mux_stub_registered_rdm_device_info));
  memset( &ds_mux_reged_sio_dev, 0, sizeof(ds_mux_stub_registered_sio_device_info));
  memset( &ds_mux_stub_log_conn_info, 0x0, sizeof(ds_mux_stub_log_conn_info));
  memset( &phys_port_open_param, 0x0, sizeof(phys_port_open_param));
  
  /*-----------------------------------------------------------------------
    1. Initialize DS MUX as driver to serve logical connection
  -----------------------------------------------------------------------*/ 
#ifndef MOB_TX
  ds_mux_logical_driver_init( ds_mux_stub_sio_register_device_driver );
#endif
  /*---------------------------------------------------------------------
  2 . When Static initialization is true then we have to register device 
  -----------------------------------------------------------------------*/ 
  if ( TRUE == is_static_init)
  {
    ds_mux_reged_rdm_dev.device[ds_mux_reged_rdm_dev.total_device]       = DS_MUX_PHY_DEV;
    ds_mux_reged_rdm_dev.sio_port[ds_mux_reged_rdm_dev.total_device]     = DS_MUX_PHY_PORT;
    ds_mux_reged_rdm_dev.service_type[ds_mux_reged_rdm_dev.total_device] = DS_MUX_SERVICE;
    ds_mux_reged_rdm_dev.total_device++;
  }

/*-----------------------------------------------------------------------
  3. Register physical SIO interface so that SIO layer invoke registered APIs for physical port
    operation DS MUX ->SIO -> Test case
-----------------------------------------------------------------------*/ 
  register_phy_sio_drv( test_phy_driver_iface );


  
}


void rdm_register_service
(
  rdm_service_enum_type srvc,
  rdm_service_params_type *srvc_params
)
{

  uint8  index = ds_mux_reged_rdm_srv.total_serv;
  uint8  dev_index             = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( index < DS_MUX_MAX_SERV )
  {
    ds_mux_reged_rdm_srv.service_type[index] = srvc;
    memscpy(&ds_mux_reged_rdm_srv.service_params[index],
        sizeof(rdm_service_params_type), srvc_params, sizeof(rdm_service_params_type));
    
    ds_mux_reged_rdm_srv.total_serv++;
  }

  //Check if device is already registered

  for ( dev_index  = 0;  dev_index < ds_mux_reged_rdm_dev.total_device ; dev_index++)
  {
    if ( ds_mux_reged_rdm_dev.service_type[dev_index] == srvc )
    {
      srvc_params->open_func( ds_mux_reged_rdm_dev.sio_port[dev_index] );
      break;
    }
  }

}/*rdm_register_service*/

void ds_mux_rdm_register_service
(
  rdm_service_enum_type srvc,
  rdm_service_params_type *srvc_params
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  rdm_register_service( srvc, srvc_params);

}/* ds_mux_rdm_register_service */


void rdm_register_device
(
  rdm_device_info_type *dev_info
)
{
  uint8  index = ds_mux_reged_rdm_dev.total_device;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( index < DS_MUX_MAX_SERV )
  {

    ds_mux_reged_rdm_dev.device[index]       = dev_info->device;
    ds_mux_reged_rdm_dev.sio_port[index]     = dev_info->sio_port;
    ds_mux_reged_rdm_dev.total_device++;
  }
}

void rdm_set_compatibility
(
  rdm_service_enum_type service,
  rdm_device_enum_type device
)
{

}

boolean rdm_assign_port
(
  /* Service Task desiring a device                     */
  rdm_service_enum_type         service,

  /* Device to give to service task                     */
  rdm_device_enum_type          device,

  /* Optional callback function to notify the status of
     the assign action                                  */
  rdm_assign_port_cb_type       cbptr
)
{
  boolean             device_registered = FALSE;
  uint8               index             = 0;
  sio_port_id_type    port_id           = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for ( index  =0; index < DS_MUX_MAX_SERV && index < ds_mux_reged_rdm_dev.total_device ; index++)
  {
    if ( ds_mux_reged_rdm_dev.device[index] == device)
    {
      device_registered = TRUE;
      port_id = ds_mux_reged_rdm_dev.sio_port[index];
      ds_mux_reged_rdm_dev.service_type[index]  = service;
      break;
    }
  }
  //Device is registered 
  if ( TRUE == device_registered)
  {
    //Notify Assign successful
    cbptr(RDM_DONE_S, service, device);

    //Check if Service is registered: then call service open call back function
    for ( index  =0; index < DS_MUX_MAX_SERV && index < ds_mux_reged_rdm_srv.total_serv ; index++)
    {
      if ( ds_mux_reged_rdm_srv.service_type[index] == service )
      {
        ds_mux_reged_rdm_srv.service_params[index].open_func(port_id );
        break;
      }
    }
  }
  else
  {
    cbptr(RDM_DEVMAP_BUSY_S, service, device);
  }
  


  return TRUE;
}

boolean rdm_close_device
(
  /* Service Task request closing of the device         */
  rdm_service_enum_type         service,

  /* Device/port to be closed                           */
  rdm_device_enum_type          device,

  /* Optional callback function to inform the result of
     the closing action                                 */
  rdm_assign_port_cb_type       cbptr
)
{
  return TRUE;
}


sio_stream_id_type ds_mux_stub_sio_open
( 
  sio_open_type *open_ptr                      /* Configurations of stream */
)
{
  uint8                    index = 0;
  sio_status_type          ret_val             = SIO_DONE_S;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for (index =  0; index < ds_mux_reged_sio_dev.total_sio_dev; index++)
  {
    if ( ds_mux_reged_sio_dev.device_id[index] == (uint32)open_ptr->port_id)
    {
      ret_val = ds_mux_reged_sio_dev.driver_interface[index].HW_open(open_ptr);
      break;
    }
  }
  if (  SIO_DONE_S == ret_val && index <= ds_mux_reged_sio_dev.total_sio_dev)
  {
    return ds_mux_reged_sio_dev.stream_id[index];
  }
  return SIO_NO_STREAM_ID;
}

void ds_mux_stub_sio_ioctl
(
  sio_stream_id_type    stream_id,
  sio_ioctl_cmd_type    cmd,
  sio_ioctl_param_type *param
)
{
  uint8                    index = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  for (index =  0; index < ds_mux_reged_sio_dev.total_sio_dev; index++)
  {
    if ( ds_mux_reged_sio_dev.stream_id[index] == (uint32)stream_id)
    {
       ds_mux_reged_sio_dev.driver_interface[index].HW_ioctl(stream_id, 
        ds_mux_reged_sio_dev.device_id[index], cmd, param);
      break;
    }
  }

  return;
}


void ds_mux_stub_sio_transmit
( 
  sio_stream_id_type stream_id,    /* Stream ID                            */
  dsm_item_type *tx_ptr            /* Packet which needs to be transmitted */
)
{
  uint8                    index = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  for (index =  0; index < ds_mux_reged_sio_dev.total_sio_dev; index++)
  {
    if ( ds_mux_reged_sio_dev.stream_id[index] == (uint32)stream_id)
    {
      ds_mux_reged_sio_dev.driver_interface[index].HW_transmit(stream_id, 
        ds_mux_reged_sio_dev.device_id[index], 
        tx_ptr);
      break;
    }
  }

  return;
}

void ds_mux_stub_sio_close
( 
  sio_stream_id_type stream_id,               /* Stream ID                 */
  void             (*close_func_ptr)(void)    /* Function to call when 
                                                 transmission is complete. */
)
{
  return;
}

boolean ds_mux_stub_phy_transmit_to_mux
(
  dsm_item_type *tx_ptr
)
{
  boolean    is_transmitted= FALSE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(  NULL != phys_port_open_param.rx_queue )
  {
    dsm_enqueue(phys_port_open_param.rx_queue , &tx_ptr);
    is_transmitted= TRUE;
  }
  else
  {
    dsm_free_packet(&tx_ptr);
    
  }
  
  return is_transmitted;
  
}/* ds_mux_stub_phy_transmit_to_mux */

void ds_mux_stub_rdm_close_cb
(
  void
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

} /* ds_mux_phys_rdm_close_cb() */

static void ds_mux_stub_dtr_changed_cb
(
  void
)
{
  sio_ioctl_param_type          ioctl_param;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  memset( &ioctl_param, 0x0, sizeof(sio_ioctl_param_type));
  
  ioctl_param.dte_ready_asserted = &ds_mux_stub_dte_ready_asserted[ds_mux_stub_curr_stream_id_index];
  
  sio_ioctl( ds_mux_stub_log_conn_info.stream_id[ds_mux_stub_curr_stream_id_index],
            SIO_IOCTL_DTE_READY_ASSERTED,
            &ioctl_param );

  //Notify DTR status to the testcase
  if ( NULL != test_dtr_func_ptr_type)
  {
    test_dtr_func_ptr_type( ds_mux_stub_log_conn_info.stream_id[ds_mux_stub_curr_stream_id_index],
                            ds_mux_stub_dte_ready_asserted[ds_mux_stub_curr_stream_id_index]);
  }

  
}/*ds_mux_stub_dtr_changed_cb*/

static void  ds_mux_stub_tx_wmk_low_func
(
  dsm_watermark_type  * ds_mux_tx_wmk,
  void                * user_data_ptr
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

}/* ds_mux_stub_tx_wmk_low_func */

static void  ds_mux_stub_tx_wmk_hi_func
(
  dsm_watermark_type  * ds_mux_tx_wmk,
  void                * user_data_ptr
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

}/* ds_mux_stub_tx_wmk_hi_func */


static void  ds_mux_stub_rx_wmk_low_func
(
  dsm_watermark_type  * ds_mux_rx_wmk,
  void                * user_data_ptr
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

}/* ds_mux_ds_mux_test_rx_wmk_low_func */

static void  ds_mux_stub_rx_wmk_hi_func
(
  dsm_watermark_type  * ds_mux_rx_wmk,
  void                * user_data_ptr
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

}/* ds_mux_stub_rx_wmk_hi_func */


static void  ds_mux_stub_rx_wmk_non_empty_func
(
  dsm_watermark_type  * ds_mux_rx_wmk,
  void                * user_data_ptr
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( NULL != test_non_emp_wm_func_ptr_type)
  {
    test_non_emp_wm_func_ptr_type( ds_mux_stub_log_conn_info.stream_id[ds_mux_stub_curr_stream_id_index], ds_mux_rx_wmk);
  }

  
}/* ds_mux_stub_rx_wmk_non_empty_func */

static void ds_mux_stub_rdm_open_cb
(
  sio_port_id_type port_id
)
{
  uint8                index = ds_mux_stub_log_conn_info.conn_count;
  sio_open_type        open_param;
  sio_ioctl_param_type ioctl_param;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  memset( &open_param, 0x0, sizeof(sio_open_type));
  memset( &ioctl_param, 0x0, sizeof(sio_ioctl_param_type));
  
  if ( index < DS_MUX_MAX_LOGICAL_CONNECTION)
  {
    ds_mux_stub_log_conn_info.sio_port[index] = port_id;
    //WMs initialization
    /* Step 1 */
    dsm_queue_init ( &ds_mux_stub_tx_wmk[index],
                     0x7FFFFFFF,
                     &ds_mux_stub_tx_q[index] );
    
    dsm_set_low_wm( &ds_mux_stub_tx_wmk[index], DS_MUX_TEST_PHYS_WM_DEFAULT_LO );
    dsm_set_hi_wm( &ds_mux_stub_tx_wmk[index], DS_MUX_TEST_PHYS_WM_DEFAULT_HI );
    dsm_set_dne( &ds_mux_stub_tx_wmk[index], DS_MUX_TEST_PHYS_WM_DEFAULT_DNE );

    ds_mux_stub_tx_wmk[index].lowater_func_ptr = ds_mux_stub_tx_wmk_low_func;
    ds_mux_stub_tx_wmk[index].hiwater_func_ptr = ds_mux_stub_tx_wmk_hi_func;

    
    /* Step 2 */
    dsm_queue_init ( &ds_mux_stub_rx_wmk[index],
                     0x7FFFFFFF,
                     &ds_mux_stub_rx_q[index] );
    
    dsm_set_low_wm( &ds_mux_stub_rx_wmk[index], DS_MUX_TEST_PHYS_WM_DEFAULT_LO );
    dsm_set_hi_wm( &ds_mux_stub_rx_wmk[index], DS_MUX_TEST_PHYS_WM_DEFAULT_HI );
    dsm_set_dne( &ds_mux_stub_rx_wmk[index], DS_MUX_TEST_PHYS_WM_DEFAULT_DNE );
    
    ds_mux_stub_rx_wmk[index].non_empty_func_ptr = ds_mux_stub_rx_wmk_non_empty_func;
    ds_mux_stub_rx_wmk[index].lowater_func_ptr = ds_mux_stub_rx_wmk_low_func;
    ds_mux_stub_rx_wmk[index].hiwater_func_ptr = ds_mux_stub_rx_wmk_hi_func;
    /* Step 3*/
    open_param.rx_queue    = &ds_mux_stub_rx_wmk[index];
    open_param.tx_queue    = &ds_mux_stub_tx_wmk[index];
    open_param.rx_func_ptr = NULL;
    open_param.stream_mode = SIO_GENERIC_MODE;
    open_param.stream_type = SIO_STREAM_RXTX;
    open_param.port_id     = port_id;
    open_param.rx_bitrate  = SIO_BITRATE_9600;
    open_param.tx_bitrate  = SIO_BITRATE_9600;
    ds_mux_stub_log_conn_info.stream_id[index] = ds_mux_stub_sio_open(&open_param);

    ASSERT(SIO_NO_STREAM_ID != ds_mux_stub_log_conn_info.stream_id[index]);

    /*Step 4*/
       /* Step 3 */
    ioctl_param.enable_dte_ready_event = ds_mux_stub_dtr_changed_cb;
    
    ds_mux_stub_sio_ioctl( ds_mux_stub_log_conn_info.stream_id[index],
               SIO_IOCTL_ENABLE_DTR_EVENT,
               &ioctl_param );
    
    ioctl_param.dte_ready_asserted = &ds_mux_stub_dte_ready_asserted[index];
    
    ds_mux_stub_sio_ioctl( ds_mux_stub_log_conn_info.stream_id[index],
            SIO_IOCTL_DTE_READY_ASSERTED,
            &ioctl_param );
    
    //Notify port open status to the testcase
    if ( NULL != test_open_func_ptr_type)
    {
      test_open_func_ptr_type( ds_mux_stub_log_conn_info.stream_id[index] );
    }
    //Notify DTR status to the testcase
    if ( NULL != test_dtr_func_ptr_type)
    {
      test_dtr_func_ptr_type( ds_mux_stub_log_conn_info.stream_id[index],
                              ds_mux_stub_dte_ready_asserted[index]);
    }
    
    ds_mux_stub_log_conn_info.conn_count++;
  }


}/*ds_mux_test_rdm_open_cb*/

 void ds_mux_stub_register_service_with_rdm
(
  void (*open_func_ptr_type) (sio_stream_id_type),
  void (*close_func_ptr_type)(sio_stream_id_type),
  void (*dtr_func_ptr_type)  (sio_stream_id_type, boolean),
  void (*non_emp_wm_func_ptr_type)(sio_stream_id_type, dsm_watermark_type  *)
)
{
  rdm_service_params_type  rdm_params;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Step 1 */

  test_open_func_ptr_type       = open_func_ptr_type;
  test_close_func_ptr_type      = close_func_ptr_type;
  test_dtr_func_ptr_type        = dtr_func_ptr_type;
  test_non_emp_wm_func_ptr_type = non_emp_wm_func_ptr_type;
  
  /* Step 2 */
  rdm_params.device_support   = RDM_SINGLE_DEV_SUPPORT;
  rdm_params.open_multi_func  = NULL;
  rdm_params.close_multi_func = NULL;
  rdm_params.open_func        = ds_mux_stub_rdm_open_cb;
  rdm_params.close_func       = ds_mux_stub_rdm_close_cb;
  rdm_params.data             = NULL;

  /* Step 3 */
  ds_mux_rdm_register_service ( DS_MUX_TEST_LOGICAL_SERVICE, &rdm_params );

}/* ds_mux_test_register_service_with_rdm */

 void ds_mux_stub_send_data_over_logical
(
  uint8                     dlci_Id,
  dsm_item_type           **tx_ptr
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_mux_stub_sio_transmit( ds_mux_stub_log_conn_info.stream_id[dlci_Id - 1], *tx_ptr);


}/*ds_mux_stub_send_data_over_logical*/


void ds_mux_stub_send_ioctl_over_logical
(
  uint8                     dlci_Id,
  sio_ioctl_cmd_type        cmd,
  sio_ioctl_param_type     *param
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_mux_stub_sio_ioctl( ds_mux_stub_log_conn_info.stream_id[dlci_Id - 1], cmd, param);

}/* ds_mux_stub_send_ioctl_over_logical */

#endif  /* TEST_FRAMEWORK */


