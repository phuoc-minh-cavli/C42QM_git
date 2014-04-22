#ifndef UART_COMPAT_H
#define UART_COMPAT_H     1    
/*============================================================================

GENERAL DESCRIPTION

  This module contains code for compatability between different versions
  of the UART driver and the code it integrates with.

EXTERNALIZED FUNCTIONS
    
INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 1999-2000, 2007-2008, 2012-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.  Qualcomm Confidential and Proprietary
============================================================================*/

/*============================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.tx/6.0/buses/uart/mdd/inc/uart_compat.h#1 $

when         user      what, where, why
-----------  --------  -------------------------------------------------------
15-Jan-2009  cmihalik  Initial revision.
22-Jul-2009  cmihalik  Expand the compatibility layer to include a lot more things.
============================================================================*/

#include "comdef.h"
#include "sio.h"
#include "DALStdDef.h"
#include "uart_device_props.h"

//----------------------------------------------------------------------------
//  Count the number of UARTs.
//----------------------------------------------------------------------------

#define UART_FORCE_SIZE_TO_32_BITS     0x12345678

#ifdef FEATURE_CXM_UART
  #define UART_NUM_DEVICES    4
#elif defined FEATURE_THIRD_UART
  #define UART_NUM_DEVICES    3
#elif defined FEATURE_SECOND_UART
  #define UART_NUM_DEVICES    2
#elif defined FEATURE_FIRST_UART
  #define UART_NUM_DEVICES    1
#else
  #define UART_NUM_DEVICES    0
#endif

//----------------------------------------------------------------------------
//  uart_device_id_type
//----------------------------------------------------------------------------

typedef enum 
{
  //  UART devices.

  UART_DEVICE_INVALID = -1,
  UART_DEVICE_1 = 0,
  UART_DEVICE_2,
  UART_DEVICE_3,
  UART_DEVICE_CXM,

  //  Aliases used for enumeration.
  //  Enumeration is ( i = FIRST; i < LAST; i++ ), i.e. LAST is not included.

  UART_DEVICE_FIRST = UART_DEVICE_1,
  UART_DEVICE_LAST  = UART_NUM_DEVICES,

  //  Force size. 

  UART_DEVICE_FORCE_SIZE = UART_FORCE_SIZE_TO_32_BITS

} uart_device_id_type;

//----------------------------------------------------------------------------
//  uart_device_info
//----------------------------------------------------------------------------

//  The purpose of this structure is to convert all of the compile
//  time configuration into something that can be accessed at runtime.

typedef struct uart_device_info
{
  boolean                  is_present;
  boolean                  is_valid;
  boolean                  enable_dma;
  boolean                  can_wake_on_rx_event;
  DALDEVICEID              dal_device_id;

} uart_device_info;

//----------------------------------------------------------------------------
//  FUNCTION 
//    uart_sio_port_id_to_device_id
//
//  DESCRIPTION
//    This function converts the input SIO port id to the corresponding 
//    device id.
//
//  DEPENDENCIES
//    None
//
//  RETURN VALUE
//    device_id if input port_id is valid.
//    UART_DEVICE_INVALID otherwise.
//
//  SIDE EFFECTS
//    None
//----------------------------------------------------------------------------

uart_device_id_type      uart_sio_port_id_to_device_id( sio_port_id_type port_id );
boolean                  uart_get_device_info( uart_device_id_type device_id, uart_device_info* pInfo );

#endif

