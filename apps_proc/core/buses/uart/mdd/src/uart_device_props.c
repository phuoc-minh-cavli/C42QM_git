/*==================================================================================================

FILE: uart_device_props.c

DESCRIPTION: Defines the common properties needed for the UART driver used by SIO

                     Copyright (c) 2015 Qualcomm Technologies Incorporated
                                        All Rights Reserved
                                     QUALCOMM Proprietary/GTDR

==================================================================================================*/

/*==================================================================================================
                                           INCLUDE FILES
==================================================================================================*/

#include "uart_device_props.h"

/*==================================================================================================
                                         GLOBAL FUNCTIONS
==================================================================================================*/
/*==================================================================================================

FUNCTION: get_bool_property

DESCRIPTION: Read property from XML file as a boolean value. If successful, prop_handle will contain 
the property. Otherwise, the default_value will be written there instead 

==================================================================================================*/
void 
get_bool_property( DALSYSPropertyHandle handle, const char *name, boolean *prop_handle,
                   boolean default_value )
{
   DALSYSPropertyVar prop;
   DALResult result;
   
   result = DALSYS_GetPropertyValue( handle, name, 0, &prop );
   if ( result == DAL_SUCCESS )
   {
      *prop_handle = prop.Val.dwVal ? TRUE : FALSE;
   }
   else
   {
      *prop_handle = default_value;
   }
}

/*==================================================================================================

FUNCTION: get_dword_property

DESCRIPTION: Read property from XML file as a uint32 value. If successful, prop_handle will contain 
the property. Otherwise, the default_value will be written there instead 

==================================================================================================*/
void 
get_dword_property( DALSYSPropertyHandle handle, const char *property_name,
                    uint32 *property_value, uint32 default_value )
{
   DALSYSPropertyVar property_variable;
   DALResult result;

   result = DALSYS_GetPropertyValue( handle, property_name, 0, &property_variable );

   if( result == DAL_SUCCESS )
   {
      *property_value = property_variable.Val.dwVal;
   }
   else
   {
      *property_value = default_value;
   }

}
