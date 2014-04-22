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

  $Header: //components/rel/core.tx/6.0/buses/uart/mdd/src/uart_compat.c#1 $

when         user      what, where, why
-----------  --------  -------------------------------------------------------
15-Jan-2009  cmihalik  Initial revision.
22-Jul-2009  cmihalik  Greatly expand the scope to the compat layer.

============================================================================*/

#include "comdef.h"
#include "err.h"
#include "uart_compat.h"
#include "DALDeviceId.h"
#include "uart_log.h"
#include "DALSys.h"
#include "uart_device_props.h"

#define UART_TRACE_BUFFER_FILE_ID      7

//  Something not likely to conflict with any port id.
#ifndef SIO_PORT_ILLEGAL
#define SIO_PORT_ILLEGAL   (0x7fffffff)
#endif

//----------------------------------------------------------------------------
//  uart_sio_port_id_to_device_id
//----------------------------------------------------------------------------

uart_device_id_type
uart_sio_port_id_to_device_id( sio_port_id_type port_id )
{
   uart_device_id_type device_id;
   boolean             bool_prop;
   DALSYS_PROPERTY_HANDLE_DECLARE( handle );

   //  This code assumes the client and the driver are compiled
   //  with the same version of the sio.h header file.  If not,
   //  something smarter needs to be done here.
   //
   //  Cannot use a simple switch statement.  Why?  The SIO port
   //  definitions may be replaced with SIO_PORT_ILLEGAL, indicating
   //  the port definition is not valid for the given set of 
   //  feature defines.  You can end up with a given case listed 
   //  multiple times -- a compile error.  The nested if doesn't
   //  have that problem.

   if( DALSYS_GetDALPropertyHandleStr( "uart_build_props", handle ) != DAL_SUCCESS )
   {
      return UART_DEVICE_INVALID;
   }

   if( SIO_PORT_UART_MAIN == port_id )
   {
      get_bool_property( handle, "FEATURE_FIRST_UART", &bool_prop, FALSE );
      if( bool_prop )
      {
         device_id = UART_DEVICE_1;
      }
      else
      {
         device_id = UART_DEVICE_INVALID;
      }
   }
   else if( SIO_PORT_UART_AUX == port_id )
   {
      get_bool_property( handle, "FEATURE_SECOND_UART", &bool_prop, FALSE );
      if( bool_prop )
      {
         device_id = UART_DEVICE_2;
      }
      else
      {
         device_id = UART_DEVICE_INVALID;
      }
   }
   else if( SIO_PORT_UART_THIRD == port_id )
   {
      get_bool_property( handle, "FEATURE_THIRD_UART", &bool_prop, FALSE );
      if( bool_prop )
      {
         device_id = UART_DEVICE_3;
      }
      else
      {
         device_id = UART_DEVICE_INVALID;
      }
   }
   else if( SIO_PORT_UART_CXM == port_id )
   {
      get_bool_property( handle, "FEATURE_CXM_UART", &bool_prop, FALSE );
      if( bool_prop )
      {
         device_id = UART_DEVICE_CXM;
   }
   else
   {
      device_id = UART_DEVICE_INVALID;
   }
}
   else
{
      device_id = UART_DEVICE_INVALID;
   }

   return( device_id );
}

//----------------------------------------------------------------------------
//  uart_get_device_info
//
//  Description:
//  Isolate all featurizations in this one function.  This function
//  turns all of the compile time switches (featurization) into a run 
//  time accessible structure.  That way, all of the ugliness of featurization 
//  can be hidden in this function and not pollute the rest of the code.
//
//  Returns TRUE if device is valid, FALSE if the device is not valid
//
//----------------------------------------------------------------------------

boolean                  
uart_get_device_info( uart_device_id_type device_id, uart_device_info* pInfo )
{
   int              driver_type;
   boolean           bool_prop;
   DALDEVICEID      dal_device_id;
   DALSYS_PROPERTY_HANDLE_DECLARE( handle );

   UART_LOG_1( TRACE, "[%d] +uart_get_device_info", device_id );

   pInfo->is_present = FALSE;

   //  Isolate all featurizations in this one function.  This function
   //  turns all of the compile time switches (featurization) into a run 
   //  time accessible structure.  That way, all of the ugliness of featurization 
   //  can be hidden in this function and not pollute the rest of the code.

   if( !(( UART_DEVICE_FIRST <= device_id) && ( device_id < UART_DEVICE_LAST )))
   {
      UART_LOG_1( ERROR, "[%d] -uart_get_device_info: unrecognized device ID", device_id );
      goto error;
   }

   if( DALSYS_GetDALPropertyHandleStr( "uart_build_props", handle ) != DAL_SUCCESS )
   {
      UART_LOG_0( ERROR, "-uart_get_device_info: couldn't get uart_build_props property handle" );
      goto error;
   }

   //  Set the device and driver type.

   driver_type = NO_DRIVER;
   dal_device_id = UART_INVALID_DALDEVICEID;

   switch( device_id )
   {
      case UART_DEVICE_1:
         get_bool_property( handle, "FEATURE_FIRST_UART", &bool_prop, FALSE );
         if( bool_prop == TRUE )
         {
            get_dword_property( handle, "UART_FIRST_DALDEVICEID", &dal_device_id,
                                UART_INVALID_DALDEVICEID );
            get_dword_property( handle, "UART_FIRST_DRIVER_TYPE", (uint32 *)&driver_type,
                                NO_DRIVER );
            get_bool_property( handle, "UART_FIRST_PD_WAKE_ON_RX",
                               &( pInfo->can_wake_on_rx_event ), FALSE );
         }
         break;

      case UART_DEVICE_2:
         get_bool_property( handle, "FEATURE_SECOND_UART", &bool_prop, FALSE );
         if( bool_prop == TRUE )
         {
            get_dword_property( handle, "UART_SECOND_DALDEVICEID", &dal_device_id,
                                UART_INVALID_DALDEVICEID );
            get_dword_property( handle, "UART_SECOND_DRIVER_TYPE", (uint32 *)&driver_type,
                                NO_DRIVER );
            get_bool_property( handle, "UART_SECOND_PD_WAKE_ON_RX",
                               &( pInfo->can_wake_on_rx_event ), FALSE );
         }
         break;

      case UART_DEVICE_3:
         get_bool_property( handle, "FEATURE_THIRD_UART", &bool_prop, FALSE );
         if( bool_prop == TRUE )
         {
            get_dword_property( handle, "UART_THIRD_DALDEVICEID", &dal_device_id,
                                UART_INVALID_DALDEVICEID );
            get_dword_property( handle, "UART_THIRD_DRIVER_TYPE", (uint32 *)&driver_type,
                                NO_DRIVER );
            get_bool_property( handle, "UART_THIRD_PD_WAKE_ON_RX",
                               &( pInfo->can_wake_on_rx_event ), FALSE );
         }
         break;

      case UART_DEVICE_CXM:
         get_bool_property( handle, "FEATURE_CXM_UART", &bool_prop, FALSE );
         if( bool_prop == TRUE )
         {
            get_dword_property( handle, "UART_CXM_DALDEVICEID", &dal_device_id,
                                UART_INVALID_DALDEVICEID );
            get_dword_property( handle, "UART_CXM_DRIVER_TYPE", (uint32 *)&driver_type,
                                NO_DRIVER );
            pInfo->can_wake_on_rx_event = FALSE;
         }
         break;

      default:

         UART_LOG_1( ERROR, "[%d] is not first, second, third, or cxm UART", device_id );
         goto error;
   }

   UART_LOG_2( TRACE, "[%d] driver_type: %d", device_id, driver_type );
   UART_LOG_2( TRACE, "[%d] can_wake_on_rx_event: %d", device_id, pInfo->can_wake_on_rx_event );

   //  Set the rest of the data in the info structure based
   //  on the driver type.

   if( dal_device_id == UART_INVALID_DALDEVICEID )
   {
      pInfo->is_present = FALSE;
      goto error;
   }
   else
   {
      pInfo->is_present = TRUE;
      pInfo->dal_device_id = dal_device_id;
   }

   UART_LOG_2( TRACE, "[%d] dal_device_id: 0x%08x", device_id, dal_device_id );

   pInfo->enable_dma = FALSE;
   switch( driver_type )
   {
      case BAM_DRIVER:
      case DM_DRIVER:

         pInfo->enable_dma = TRUE;
         break;
   }

   if( driver_type == NO_DRIVER )
   {
      UART_LOG_1( ERROR, "[%d] NO DRIVER", device_id );
      goto error;
   }

   UART_LOG_1( TRACE, "[%d] -uart_get_device_info", device_id );
   pInfo->is_valid = TRUE;
   return ( TRUE );

error:
   UART_LOG_1( TRACE, "[%d] -uart_get_device_info (error)", device_id );
   pInfo->is_valid = FALSE;
   return( FALSE );
}

