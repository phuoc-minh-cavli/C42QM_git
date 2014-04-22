/*==================================================================================================

FILE: tal_clock.c

DESCRIPTION: This module defines the "stock" implementation of the clock APIs
             for the Target Abstraction Layer.  This implementation selects
             the correct clock for the UART device and invokes the DAL Clock
             layer API to enable or disable the clock and set the clock
             frequency.

                           Copyright (c) 2012, 2014-2017 Qualcomm Technologies Incorporated
                                        All Rights Reserved
                                     QUALCOMM Proprietary/GTDR

==================================================================================================*/
/*==================================================================================================

$Header: //components/rel/core.tx/6.0/buses/uart/tal/src/tal_clock.c#1 $

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

#include "comdef.h"
#include "DALSys.h"
#include "DALDeviceId.h"
#include "ClockDefs.h"
#include "DDIClock.h"
#include "tal.h"
#include "icbarb.h"
/*==================================================================================================
                                              MACROS
==================================================================================================*/
#define UART_ICB_CLIENT_CNT          1
#define UART_PNOC_MSTRSLV_PAIRS_NUM  1
#define FREQ_EQUAL_TO_1_MBPS         16000000
#define MIN_BW_FOR_UART_PROTOCL      500000

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

   /** Handle to DAL device for clock
    */
   DalDeviceHandle *daldev_clock;

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

   /** Flag indicating whether this interface is responsible for resetting the
    * clock block before enabling the clock.
    *
    * This flag is stored in the XML configuration under the property
    * "ResetClock".
    */
   uint32       reset_clock;

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
   uint32      pnoc_enable;  
   uint32      pnoc_ib_val;
   uint32      pnoc_ab_val;
   uint32      pnoc_master;
   uint32      pnoc_slave;
   uint32      noc_ap_ib_val;
   uint32      noc_ap_ab_val;
   uint32      noc_ap_master;
   uint32      noc_ap_slave;
   /** NPA client for BLSP->BIMC voting  */
   npa_client_handle  npa_pnoc_client;
   /** NPA client for APPS->BIMC voting  */
   npa_client_handle  npa_ap_noc_client;
  
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
   //const char *defaulted_str;

   result = DALSYS_GetPropertyValue(handle, property_name, 0, &property_variable);

   if (result == DAL_SUCCESS)
   {
      *property_value = property_variable.Val.pszVal;
      //defaulted_str = "";
   }
   else
   {
      *property_value = default_value;
      //defaulted_str = " (defaulted)";
   }

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
   //const char *defaulted_str;

   result = DALSYS_GetPropertyValue(handle, property_name, 0, &property_variable);

   if (result == DAL_SUCCESS)
   {
      *property_value = property_variable.Val.dwVal;
      //defaulted_str = "";
   }
   else
   {
      *property_value = default_value;
      //defaulted_str = " (defaulted)";
   }
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
   // No close needed for clock
   if(handle->npa_pnoc_client)
   {
      icbarb_complete_request(handle->npa_pnoc_client);    
      icbarb_destroy_client(handle->npa_pnoc_client);
   }
   if(handle->npa_ap_noc_client)
   {
      icbarb_complete_request(handle->npa_ap_noc_client);    
      icbarb_destroy_client(handle->npa_ap_noc_client);
   }
   DAL_DeviceDetach(handle->daldev_clock);
   free(handle);
   handle = NULL;
   
}

/*==================================================================================================

FUNCTION: tal_clock_disable

DESCRIPTION:
   Disable all clocks used by the UART core specified when opening the clock handle.

==================================================================================================*/
TAL_RESULT tal_clock_disable(TAL_CLOCK_HANDLE handle)
{
   DALResult dal_result;
   TAL_RESULT tal_result = TAL_SUCCESS;

   dal_result = DalClock_DisableClock(handle->daldev_clock, handle->core_clk_id);
   if (dal_result != DAL_SUCCESS) { tal_result = TAL_ERROR; }

   if ( handle->manage_pclk && (handle->periph_clk_id != 0) )
   {
      dal_result = DalClock_DisableClock(handle->daldev_clock, handle->periph_clk_id);
      if (dal_result != DAL_SUCCESS) { tal_result = TAL_ERROR; }
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
   DALResult result;
   uint32 result_freq;

   if(handle->reset_clock)
   {
      result = DalClock_ResetClock(handle->daldev_clock, handle->core_clk_id, CLOCK_RESET_DEASSERT);
      if (result != DAL_SUCCESS) { goto error; }
   }

   // Core clock frequency must be configured to a valid frequency before calling enable.
   // Since at the first enable call the frequency will not be configured we can call 
   // DalClock_SetClockFrequency with a request for 1MHZ and HZ_ATLEAST attribute to 
   // get lowest valid frequency in MHz.

   if (!handle->core_clk_configured)
   {
      result = DalClock_SetClockFrequency( handle->daldev_clock,
                                           handle->core_clk_id,
                                           1000000,
                                           CLOCK_FREQUENCY_HZ_AT_LEAST,
                                           &result_freq);

      if (result != DAL_SUCCESS)
      {
         
         goto error; 
      }

      handle->core_clk_configured = TRUE;
   }

   result = DalClock_EnableClock(handle->daldev_clock, handle->core_clk_id);
   if (result != DAL_SUCCESS) { goto error; }

   if ( handle->manage_pclk && (handle->periph_clk_id != 0) )
   {
      result = DalClock_EnableClock(handle->daldev_clock, handle->periph_clk_id);
      if (result != DAL_SUCCESS) { goto error; }
   }
   return TAL_SUCCESS;
error:
   return TAL_ERROR;
}

/*==================================================================================================

FUNCTION: tal_clock_open

DESCRIPTION:
   Open a clock handle for the specified UART device. If successful, this function will return a
   clock handle, and may acquire other resources, which must be released by calling
   tal_clock_close().

==================================================================================================*/
TAL_RESULT tal_clock_open(TAL_CLOCK_HANDLE *phandle, void *device_id)
{
   DALSYS_PROPERTY_HANDLE_DECLARE(property_handle);
   TAL_CLOCK_HANDLE handle = NULL;
   DalDeviceHandle *clock_dal = NULL;
   char *clock_name = NULL;
   ICBArb_MasterSlaveType uart_arb_master_slave, uart_arb_ap_master_slave;
   DALResult result;
	handle = malloc(sizeof(TAL_CLOCK_CONTEXT));
   if ((handle == NULL) ){ goto error; }
   memset(handle, 0, sizeof(TAL_CLOCK_CONTEXT));

   if (DAL_DeviceAttach(DALDEVICEID_CLOCK, &clock_dal) != DAL_SUCCESS) { goto error; }

   handle->daldev_clock = clock_dal;

   // No open needed for clock

   handle->dal_id = (DALDEVICEID)device_id;
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
   result = DalClock_GetClockId(handle->daldev_clock, clock_name, &handle->core_clk_id);
   if (result != DAL_SUCCESS)
   {
      
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
         result = DalClock_GetClockId(handle->daldev_clock, clock_name, &handle->periph_clk_id);
         if (result != DAL_SUCCESS)
         {
            
            goto error;
         }
      }
   }

   get_property_dword(handle->dal_id,property_handle,"ResetClock",     &handle->reset_clock,0);
   get_property_dword(handle->dal_id,property_handle,"ClockFundFreq",  &handle->fundamental_frequency,0);
   get_property_dword(handle->dal_id,property_handle,"PnocVoteEnable", &handle->pnoc_enable, 0);

   if(handle->pnoc_enable)
   {
      get_property_dword(handle->dal_id,property_handle,"PnocArbMaster", &handle->pnoc_master,0);
      get_property_dword(handle->dal_id,property_handle,"PnocArbSlave",  &handle->pnoc_slave, 0);
      if(!(handle->pnoc_master)){ goto error;}

      get_property_dword(handle->dal_id,property_handle,"PnocIBval",     &handle->pnoc_ib_val,0);
      get_property_dword(handle->dal_id,property_handle,"PnocABval",     &handle->pnoc_ab_val,0);
      if(!(handle->pnoc_ib_val && handle->pnoc_ab_val)){ goto error;}

 
      
      uart_arb_master_slave.eMaster =  (ICBId_MasterType) handle->pnoc_master;
      uart_arb_master_slave.eSlave  =  (ICBId_SlaveType ) handle->pnoc_slave; 
      
      //Create a NPA client for the "/node/core/bus/uart/pnocclk" client to make PNOC voting request
      handle->npa_pnoc_client = icbarb_create_suppressible_client_ex
                                                           ("/node/core/bus/uart/pnocclk",
                                                            &uart_arb_master_slave,
                                                            UART_ICB_CLIENT_CNT, NULL);
      if (NULL == handle->npa_pnoc_client)
      {
         
         goto error;
      }
      

	  
      get_property_dword(handle->dal_id,property_handle,"NocapArbMaster", &handle->noc_ap_master,0);
      get_property_dword(handle->dal_id,property_handle,"NocapArbSlave",  &handle->noc_ap_slave, 0);
      
	  
      get_property_dword(handle->dal_id,property_handle,"NocapIBval",     &handle->noc_ap_ib_val,0);
      get_property_dword(handle->dal_id,property_handle,"NocapABval",     &handle->noc_ap_ab_val,0);
      if((handle->noc_ap_ib_val && handle->noc_ap_ab_val))
	  { 
        uart_arb_ap_master_slave.eMaster =  (ICBId_MasterType) handle->noc_ap_master;
        uart_arb_ap_master_slave.eSlave  =  (ICBId_SlaveType ) handle->noc_ap_slave; 
      
        //Create a NPA client for the "/node/core/bus/uart/pnocclk" client to make PNOC voting request
        handle->npa_ap_noc_client = icbarb_create_suppressible_client_ex
                                                           ("/node/core/bus/uart/bimcclk",
                                                            &uart_arb_ap_master_slave,
                                                            UART_ICB_CLIENT_CNT, NULL);
        if (NULL == handle->npa_ap_noc_client)
        {
         
          goto error;
        }
	  }
   }
   else
   {
      handle->pnoc_master = 0;
      handle->pnoc_slave  = 0;
      handle->pnoc_ib_val = 0;
      handle->pnoc_ab_val = 0;
	  handle->noc_ap_master = 0;
      handle->noc_ap_slave  = 0;
      handle->noc_ap_ib_val = 0;
      handle->noc_ap_ab_val = 0;
      handle->npa_pnoc_client = NULL;
	  handle->npa_ap_noc_client = NULL;
      
   }

   *phandle = handle;
   return TAL_SUCCESS;

error:
   if (clock_dal) { DAL_DeviceDetach(clock_dal); }
   if (handle)
   { 
      free(handle);
	  handle = NULL;
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
   //LDEVICEID id = handle->dal_id;
   uint32 result_freq;
   DALResult result;
   uint32 divider = handle->fundamental_frequency / input_freq;
   ICBArb_RequestType pnoc_request, noc_ap_request;
   ICBArb_ErrorType   arb_error;
   
   if(handle->pnoc_enable)
   {   
     
	 pnoc_request.arbType =    ICBARB_REQUEST_TYPE_3;     /* Ib/Ab pair */

     if(input_freq >= FREQ_EQUAL_TO_1_MBPS)
	 {
       /* Bandwidth in Bytes/second*/
       pnoc_request.arbData.type3.uIb = (uint64)  handle->pnoc_ib_val;      
       pnoc_request.arbData.type3.uAb = (uint64)  handle->pnoc_ab_val;    
	 }
	 else
	 {
        /* Bandwidth in Bytes/second*/
       pnoc_request.arbData.type3.uIb = MIN_BW_FOR_UART_PROTOCL;      
       pnoc_request.arbData.type3.uAb = MIN_BW_FOR_UART_PROTOCL;    
	 }
	 if(handle->npa_pnoc_client)
	 {
       arb_error = icbarb_issue_request(handle->npa_pnoc_client, &pnoc_request,
                                       UART_PNOC_MSTRSLV_PAIRS_NUM);
   
       if(arb_error != ICBARB_ERROR_SUCCESS)
       {
         
         return TAL_ERROR;
       }
	 }
	 else
	 {
	    return TAL_ERROR;
	 }
     noc_ap_request.arbType =    ICBARB_REQUEST_TYPE_3;     /* Ib/Ab pair */

	 if(input_freq >= FREQ_EQUAL_TO_1_MBPS)
	 {
	   if((handle->noc_ap_ib_val != 0) && (handle->noc_ap_ib_val != 0))
	   {
         /* Bandwidth in Bytes/second*/
         noc_ap_request.arbData.type3.uIb = (uint64)  handle->noc_ap_ib_val;      
         noc_ap_request.arbData.type3.uAb = (uint64)  handle->noc_ap_ab_val;   
     
         if(handle->npa_ap_noc_client)
	     {
           arb_error = icbarb_issue_request(handle->npa_ap_noc_client, &noc_ap_request,
                                            UART_PNOC_MSTRSLV_PAIRS_NUM);
   
           if(arb_error != ICBARB_ERROR_SUCCESS)
           {
             return TAL_ERROR;  
           }
         }
	     else
	     {
		   return TAL_ERROR;  
	     }
	   }	   
	 }	   
   }
   if(handle->fundamental_frequency)
   {
      if((handle->fundamental_frequency % input_freq) >= divider)
      {
         /* If the requested frequency does not divide evenly into the clock's
          * fundamental frequency, we can't set the requested frequency using
          * a simple clock divider. The clock frequency may not be
          * representable using integers, so allow the closest integer to be
          * used. */
         
         return TAL_ERROR;
      }
      else
      {
         

         result = DalClock_SetClockDivider(handle->daldev_clock, handle->core_clk_id, divider);
         if(result != DAL_SUCCESS)
         {
            
            return TAL_ERROR;
         }
      }
   }
   else
   {
      result = DalClock_SetClockFrequency( handle->daldev_clock, 
                                           handle->core_clk_id, 
                                           input_freq, 
                                           CLOCK_FREQUENCY_HZ_EXACT, 
                                           &result_freq);

      if (result != DAL_SUCCESS || result_freq != input_freq)
      {
         
         return TAL_ERROR;
      }
   }

   handle->core_clk_configured = TRUE;

   return TAL_SUCCESS;
}
