/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                             U A R T   I N I T              
                             
            Initialization of UART device driver

GENERAL DESCRIPTION

  This module contains the code used by the device manager (SIO) to initialize
  the UART driver.

EXTERNALIZED FUNCTIONS
    
INITIALIZATION AND SEQUENCING REQUIREMENTS

  The only external function, sio_uart_driver_init, is called by the device 
  manager (SIO) to perform device driver initialization.  The name of this 
  function cannot change since SIO depends on it.

Copyright (c) 1999-2000, 2007-2008 Qualcomm Technologies Incorporated.  All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.tx/6.0/buses/uart/mdd/src/uart_init.c#1 $

when         who       what, where, why
-----------  --------  ------------------------------------------------------
11-Nov-2008  cmihalik  Initial revision.

===========================================================================*/

#include "comdef.h"
#include "assert.h"

#include "sio.h"
#include "sio_priv.h"
#include "siors232_mdd.h"
#include "uart_init.h"
#include "DALSys.h"
#include "uart_device_props.h"

/*===========================================================================

FUNCTION       uart_close_wrapper

DESCRIPTION    A simple wrapper function to convert the signature of the
               existing close function to the signature expected by
               the device manager (SIO).

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.
===========================================================================*/

static void 
uart_close_wrapper( sio_stream_id_type stream_id, sio_port_id_type port_id, void (*close_func_ptr) (void))
{
  siors_mdd_api_close_stream( stream_id, close_func_ptr ); 
}

/*===========================================================================

FUNCTION       register_UART_driver

DESCRIPTION    Register the UART device functions with the device manager (SIO).

DEPENDENCIES   None.

RETURN VALUE   SUCCESS if all devices were successfully registered.
               Error code otherwise.

SIDE EFFECTS   None.
===========================================================================*/

static void 
register_UART_driver( sio_register_device_driver_cb_type register_driver_fn )
{
     sio_driver_interface driver_iface;
     sio_device_interafce device_iface;
     uint32 ret;
     DALResult result;
     boolean prop_result;
     DALSYS_PROPERTY_HANDLE_DECLARE( handle );

     result = DALSYS_GetDALPropertyHandleStr( "uart_build_props", handle );
     ASSERT( result == DAL_SUCCESS );

     driver_iface.HW_open             = siors_mdd_api_open_stream;
     driver_iface.HW_close            = uart_close_wrapper;  // wrapper function
     driver_iface.HW_ioctl            = siors_mdd_api_ioctl;
     driver_iface.HW_transmit         = siors_mdd_api_dev_transmit;
     driver_iface.HW_flush_tx         = siors_mdd_api_flush_tx;
     driver_iface.HW_disable_device   = siors_mdd_api_disable_device;

     driver_iface.HW_control_open     = NULL;
     driver_iface.HW_control_close    = NULL;
     driver_iface.HW_control_ioctl    = NULL;
     driver_iface.HW_control_transmit = NULL;
     device_iface.forward_port_id     = SIO_PORT_NULL;

     device_iface.driver_interface    = &driver_iface;

     // UART1 
     device_iface.device_id        = DEVICE_ID( SIO_MAJOR_UART, SIO_MINOR_UART_1 );
     get_bool_property( handle, "FEATURE_FIRST_UART", &prop_result, FALSE );
     if ( prop_result )
     {
        device_iface.forward_port     = FALSE;
        device_iface.driver_available = TRUE;
     }
     else
     {
        device_iface.forward_port     = TRUE;
        device_iface.driver_available = FALSE;
     }
     ret = register_driver_fn( &device_iface );
     ASSERT( ret );

     // UART2 
     device_iface.device_id        = DEVICE_ID( SIO_MAJOR_UART, SIO_MINOR_UART_2 );
     get_bool_property( handle, "FEATURE_SECOND_UART", &prop_result, FALSE );
     if ( prop_result )
     {
        device_iface.forward_port     = FALSE;
        device_iface.driver_available = TRUE;
     }
     else
     {
        device_iface.forward_port     = TRUE;
        device_iface.driver_available = FALSE;
     }
     ret = register_driver_fn( &device_iface );
     ASSERT( ret );

     // UART3 
     device_iface.device_id        = DEVICE_ID( SIO_MAJOR_UART, SIO_MINOR_UART_3 );
     get_bool_property( handle, "FEATURE_THIRD_UART", &prop_result, FALSE );
     if ( &prop_result)
     {
        device_iface.forward_port     = FALSE;
        device_iface.driver_available = TRUE;
     }
     else
     {
        device_iface.forward_port     = TRUE;
        device_iface.driver_available = FALSE;
     }
     ret = register_driver_fn( &device_iface );
     ASSERT( ret );

     // UART_CXM
     device_iface.device_id        = DEVICE_ID( SIO_MAJOR_UART, SIO_MINOR_UART_CXM );
     get_bool_property( handle, "FEATURE_CXM_UART", &prop_result, FALSE );
     if ( prop_result)
     {
        device_iface.forward_port     = FALSE;
        device_iface.driver_available = TRUE;
     }
     else
     {
        device_iface.forward_port     = TRUE;
        device_iface.driver_available = FALSE;
     }
     ret = register_driver_fn( &device_iface );
     ASSERT( ret );
}

/*===========================================================================

FUNCTION       sio_uart_driver_init

DESCRIPTION    This is the UART device driver initialization function.  It's
               called by the device manager (SIO) to initialize the 
               UART driver.

DEPENDENCIES   None.

RETURN VALUE   1.

SIDE EFFECTS   None.
===========================================================================*/

uint32 
sio_uart_driver_init( sio_register_device_driver_cb_type register_driver_cb_fn )
{
  siors_mdd_api_dev_init();
      
  register_UART_driver( register_driver_cb_fn );  // now register UART driver with SIO.

  return(1);
}

