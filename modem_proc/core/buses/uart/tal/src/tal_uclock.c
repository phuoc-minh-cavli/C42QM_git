/*==================================================================================================

FILE: tal_uclock.c

DESCRIPTION: This module defines the "stock" implementation of the clock APIs
             for the Target Abstraction Layer.  This implementation selects
             the correct clock for the UART device and invokes the DAL Clock
             layer API to enable or disable the clock and set the clock
             frequency.

                           Copyright (c) 2019 Qualcomm Technologies Incorporated
                                        All Rights Reserved
                                     QUALCOMM Proprietary/GTDR

==================================================================================================*/
/*==================================================================================================

$Header: //components/rel/core.mpss/3.10/buses/uart/tal/src/tal_uclock.c#1 $

==================================================================================================*/
/*==================================================================================================
                                            DESCRIPTION
====================================================================================================

GLOBAL FUNCTIONS:
   tal_clock_close
   tal_clock_disable
   tal_clock_enable
   tal_clock_open
   tal_clock_set_frequency

==================================================================================================*/

/*==================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
	  

#if ( defined(_WIN32) && !defined(_WIN32_WCE) )
#include "dal.h"
#else
#include "comdef.h"
#include "DALSys.h"
#include "DALDeviceId.h"
#include "ClockDefs.h"
#endif

#include "tal.h"
#include "uClock.h"
/*==================================================================================================
                                              MACROS
==================================================================================================*/
#define UART_ICB_CLIENT_CNT          1
#define UART_PNOC_MSTRSLV_PAIRS_NUM  1
/*==================================================================================================
                                              TYPEDEFS
==================================================================================================*/



typedef struct tal_clock_context
{
   /** The DAL device id of the UART device.
    *
    * This id is used to select the correct clocks, and to print any log
    * messages originating in this interface.
    */
   DALDEVICEID dal_id;
   /** The ID for the core clock.
    *
    * This is the ID for the core clock and is associated with a
    * string provided in the XML file.
    */
   ClockIdType core_clk_id;

   /** The ID for the peripheral clock.
    *
    * This is the ID for the peripheral clock and is associated with 
    * a string provided in the XML file.
    */
   ClockIdType periph_clk_id;

   /** Flag indicating whether this interface is responsible for
    * enabling and disabling the peripheral clock.
    *
    * This flag is stored in the XML configuration under the property
    * "ManagePCLK".
    */
   uint32      manage_pclk;

   /** The fundamental frequency of the clock providing the bit clock used by
    * the UART core.
    *
    * If this value is non-zero, it indicates that the UART bit clock is
    * driven by a simple integer divider. We can call
    * DalClock_SetClockDivider() to set the divider.
    *
    * If this value is zero, it indicates that the UART bit clock is driven by
    * an M/N:D clock divider, so we can call DalClock_SetClockFrequency() to
    * request nearly any frequency.
    *
    * This value is stored in the XML configuration under the property
    * "ClockFundFreq".
    */
   uint32      fundamental_frequency;
   boolean     core_clk_configured;
} TAL_CLOCK_CONTEXT;

/*==================================================================================================
                                     LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

static DALResult get_property_string(DALDEVICEID id, DALSYSPropertyHandle handle,
                                const char *property_name, char** property_value,
                                char* default_value);

static void get_property_dword(DALDEVICEID id, DALSYSPropertyHandle handle,
                                const char *property_name,uint32 *property_value,
                                uint32 default_value);

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/
/*==================================================================================================

FUNCTION: get_property_string

DESCRIPTION:
   Retrieve a string property.

==================================================================================================*/
static DALResult get_property_string(DALDEVICEID id, DALSYSPropertyHandle handle,
                                const char *property_name, char** property_value,
                                char* default_value)
{
   DALSYSPropertyVar property_variable;
   DALResult result;
   const char *defaulted_str;

   result = DALSYS_GetPropertyValue(handle, property_name, 0, &property_variable);

   if (result == DAL_SUCCESS)
   {
      *property_value = property_variable.Val.pszVal;
      defaulted_str = "";
   }
   else
   {
      *property_value = default_value;
      defaulted_str = " (defaulted)";
   }

   DALSYS_LogEvent(id, 
                   DALSYS_LOGEVENT_INFO, 
                   "get_property_string: %s = %s%s", 
                   property_name, 
                   ((*property_value == NULL) ? "NULL" : *property_value ),
                   defaulted_str);

   return result;
}
/*==================================================================================================

FUNCTION: get_property_dword

DESCRIPTION:
   Retrieve a dword (32-bit integer) property.

==================================================================================================*/
static void get_property_dword(DALDEVICEID id, DALSYSPropertyHandle handle,
                                const char *property_name,uint32 *property_value,
                                uint32 default_value)
{
   DALSYSPropertyVar property_variable;
   DALResult result;
   const char *defaulted_str;

   result = DALSYS_GetPropertyValue(handle, property_name, 0, &property_variable);

   if (result == DAL_SUCCESS)
   {
      *property_value = property_variable.Val.dwVal;
      defaulted_str = "";
   }
   else
   {
      *property_value = default_value;
      defaulted_str = " (defaulted)";
   }

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "get_property_dword: %s = 0x%08X%s",
                                             property_name, *property_value, defaulted_str);
}

/*==================================================================================================
                                          GLOBAL FUNCTIONS
==================================================================================================*/

/*==================================================================================================

FUNCTION: tal_clock_close

DESCRIPTION:
   Close the clock handle, freeing any resources that were allocated when opening the clock handle.

==================================================================================================*/
void tal_clock_close(TAL_CLOCK_HANDLE handle)
{
   DALSYS_Free(handle);
   handle = NULL;
}

/*==================================================================================================

FUNCTION: tal_clock_disable

DESCRIPTION:
   Disable all clocks used by the UART core specified when opening the clock handle.

==================================================================================================*/
TAL_RESULT tal_clock_disable(TAL_CLOCK_HANDLE handle)
{
   boolean result;
   TAL_RESULT tal_result = TAL_SUCCESS;

   result = uClock_DisableClock(handle->core_clk_id);
   if (result != TRUE) { tal_result = TAL_ERROR; }

   if ( handle->manage_pclk && (handle->periph_clk_id != 0) )
   {
      result = uClock_DisableClock(handle->periph_clk_id);
      if (result != TRUE) { tal_result = TAL_ERROR; }
   }
   return tal_result;
}

/*==================================================================================================

FUNCTION: tal_clock_enable

DESCRIPTION:
   Enable all clocks used by the UART core specified when opening the clock handle.

==================================================================================================*/
TAL_RESULT tal_clock_enable(TAL_CLOCK_HANDLE handle)
{
   boolean result;
   
   handle->core_clk_configured = TRUE;

   result = uClock_EnableClock(handle->core_clk_id);
   if (result != TRUE) { goto error; }

   if ( handle->manage_pclk && (handle->periph_clk_id != 0) )
   {
      result = uClock_EnableClock(handle->periph_clk_id);
      if (result != TRUE) { goto error; }
   }
   return TAL_SUCCESS;
error:
   DALSYS_LogEvent(handle->dal_id, DALSYS_LOGEVENT_INFO, "Unable to enable the required clocks");
   return TAL_ERROR;
}

/*==================================================================================================

FUNCTION: tal_clock_open

DESCRIPTION:
   Open a clock handle for the specified UART device. If successful, this function will return a
   clock handle, and may acquire other resources, which must be released by calling
   tal_clock_close().

==================================================================================================*/
TAL_RESULT tal_clock_open(TAL_CLOCK_HANDLE *phandle, uint32 client_id)
{
   DALSYS_PROPERTY_HANDLE_DECLARE(property_handle);
   TAL_CLOCK_HANDLE handle = NULL;
   
   char *clock_name = NULL;
   
   DALResult result;
   boolean res;

   if (DALSYS_Malloc(sizeof(TAL_CLOCK_CONTEXT), (void **)&handle) != DAL_SUCCESS) { goto error; }
   memset(handle, 0, sizeof(TAL_CLOCK_CONTEXT));

   handle->dal_id = client_id;
   handle->manage_pclk = FALSE;

   // Read this clock's Clock Regime-specific parameters from the UART configuration:
   //
   // UartClockName: This is the name of the core clock.  It must be present.
   //
   // ManagePCLK: In recent chips there are two clock inputs to the UART HW block: the
   //             core clock and the peripheral bus clock.  The peripheral bus clock
   //             operates the bus interface (it must be enabled when reading/writing
   //             registers).  On some builds CLKREGIM manages the peripheral bus clock
   //             for us and turns it on/off along with the core clock.  On other builds
   //             we must manage the peripheral bus clock ourselves.  This property is
   //             used to specify whether we must manage it ourselves.
   //
   // PClockName: This is the name of the peripheral clock (if present)
   //
   // ResetClock: Flag indicating whether the clock block must be reset before
   //             it can be used.
   //
   // ClockFundFreq: The fundamental frequency of the bit clock, to which an
   //                integer divider can be applied. If unspecified, the clock
   //                has an M/N:D divider and the clock frequency can be
   //                specified directly.

   result = DALSYS_GetDALPropertyHandle(handle->dal_id, property_handle);
   if (result != DAL_SUCCESS) { goto error; }

   // Read the name of the UART clock
   result = get_property_string(handle->dal_id, property_handle, "UartClockName", &clock_name, NULL);
   if (result != DAL_SUCCESS || clock_name == 0) { goto error; }

   // Look up the clock ID for the core clock
   res = uClock_GetClockId(clock_name, &handle->core_clk_id);
   if (res != TRUE)
   {
      DALSYS_LogEvent(handle->dal_id, DALSYS_LOGEVENT_ERROR, "Core GetClockId( %s ) = %d", clock_name, result);
      goto error;
   }

   handle->periph_clk_id = 0;
   
   get_property_dword(handle->dal_id,property_handle,"ManagePCLK", &handle->manage_pclk,FALSE);

   if (handle->manage_pclk)
   {
      // Read the name of the peripheral clock 
      result = get_property_string(handle->dal_id, property_handle, "PClockName", &clock_name, NULL);

      // Look up the clock ID for the peripheral clock
      if (result == DAL_SUCCESS && clock_name)
      {
         res = uClock_GetClockId(clock_name, &handle->periph_clk_id);
         if (res != TRUE)
         {
            DALSYS_LogEvent(handle->dal_id, DALSYS_LOGEVENT_ERROR, "Periph GetClockId(%s) = %d", clock_name, result);
            goto error;
         }
      }
   }

   
  *phandle = handle;
   return TAL_SUCCESS;

error:
   
   if (handle)
   { 
      DALSYS_Free(handle); 
   }
   return TAL_ERROR;
}

/*==================================================================================================

FUNCTION: tal_clock_set_frequency

DESCRIPTION:
   Set the clock input frequency to the UART core.

==================================================================================================*/
TAL_RESULT tal_clock_set_frequency(TAL_CLOCK_HANDLE handle, uint32 input_freq)
{
   DALDEVICEID id = handle->dal_id;
   uint32 result_freq;
   boolean res;

    res = uClock_SetClockFrequency(handle->core_clk_id, 
                                           input_freq,  
                                           &result_freq);

      if (res != TRUE || result_freq != input_freq)
      {
         DALSYS_LogEvent(id, DALSYS_LOGEVENT_ERROR, "tal_clock_set_frequency: unsupported freq");
         return TAL_ERROR;
      }
   
    handle->core_clk_configured = TRUE;

    return TAL_SUCCESS;
}
