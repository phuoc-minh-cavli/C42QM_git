/*==================================================================================================

FILE: tal_clock_legacy.c

DESCRIPTION: This module defines the "legacy" implementation of the clock APIs
             for the Target Abstraction Layer.  This implementation selects
             the correct clock for the UART device and invokes the Clock
             Regime API to enable or disable the clock and set the clock
             frequency. It differs from the "stock" implementation in that 
             the stock implementation uses the DAL Clock layer.

                              Copyright (c) 2012 Qualcomm Technologies Incorporated
                                        All Rights Reserved
                                     QUALCOMM Proprietary/GTDR

==================================================================================================*/
/*==================================================================================================

$Header: //components/rel/core.tx/6.0/buses/uart/tal/src/tal_clock_legacy.c#1 $

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
#include "tal.h"
#include "clkrgm_msm.h"
#include "msmhwioreg.h"

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
   DALDEVICEID          dal_id;

   /** Flag indicating whether this interface is responsible for
    * enabling and disabling the peripheral clock.
    *
    * This flag is stored in the XML configuration under the property
    * "ManagePCLK".
    */
   uint32               manage_pclk;
} TAL_CLOCK_CONTEXT;

/*==================================================================================================
                                     LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

static clkrgm_msm_clk_type device_core_clock(DALDEVICEID device);
static clkrgm_msm_clk_type device_peripheral_clock(DALDEVICEID device);

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/

/*==================================================================================================

FUNCTION: device_core_clock

DESCRIPTION:
   Specifies the core clock source for a DAL device.

==================================================================================================*/
static clkrgm_msm_clk_type device_core_clock(DALDEVICEID device)
{
   switch (device)
   {
      case DALDEVICEID_UARTLG_DEVICE_1:    return CLKRGM_UART1_CLK;
      case DALDEVICEID_UARTLG_DEVICE_2:    return CLKRGM_UART2_CLK;
      case DALDEVICEID_UARTLG_DEVICE_3:    return CLKRGM_UART3_CLK;
      case DALDEVICEID_UARTDM_DEVICE_1:    return CLKRGM_UART1DM_CLK;
      case DALDEVICEID_UARTDM_DEVICE_2:    return CLKRGM_UART2DM_CLK;
#if (defined (GSBI0_UART_DM_BASE_PHYS) || defined(GSBI0_UART_REG_BASE_PHYS))
      case DALDEVICEID_UARTGSBI_DEVICE_0:  return CLKRGM_GSBI0_UART_CLK;
#endif
#if (defined (GSBI1_UART_DM_BASE_PHYS) || defined(GSBI1_UART_REG_BASE_PHYS))
      case DALDEVICEID_UARTGSBI_DEVICE_1:  return CLKRGM_GSBI1_UART_CLK;
      case DALDEVICEID_UARTGSBI_DEVICE_2:  return CLKRGM_GSBI2_UART_CLK;
      case DALDEVICEID_UARTGSBI_DEVICE_3:  return CLKRGM_GSBI3_UART_CLK;
      case DALDEVICEID_UARTGSBI_DEVICE_4:  return CLKRGM_GSBI4_UART_CLK;
#endif
#ifdef GSBI5_UART_DM_BASE_PHYS
      case DALDEVICEID_UARTGSBI_DEVICE_5:  return CLKRGM_GSBI5_UART_CLK;
      case DALDEVICEID_UARTGSBI_DEVICE_6:  return CLKRGM_GSBI6_UART_CLK;
      case DALDEVICEID_UARTGSBI_DEVICE_7:  return CLKRGM_GSBI7_UART_CLK;
      case DALDEVICEID_UARTGSBI_DEVICE_8:  return CLKRGM_GSBI8_UART_CLK;
      case DALDEVICEID_UARTGSBI_DEVICE_9:  return CLKRGM_GSBI9_UART_CLK;
      case DALDEVICEID_UARTGSBI_DEVICE_10: return CLKRGM_GSBI10_UART_CLK;
      case DALDEVICEID_UARTGSBI_DEVICE_11: return CLKRGM_GSBI11_UART_CLK;
      case DALDEVICEID_UARTGSBI_DEVICE_12: return CLKRGM_GSBI12_UART_CLK;
#endif
      default:                             return CLKRGM_NOT_SUPPORTED_CLK;
   }
}

/*==================================================================================================

FUNCTION: device_peripheral_clock

DESCRIPTION:
   Specifies the peripheral bus clock source for a DAL device.

==================================================================================================*/
static clkrgm_msm_clk_type device_peripheral_clock(DALDEVICEID device)
{
#ifdef _WIN32_WCE
   // The peripheral clocks use different enumerations on 7x30 (ie. CLKRGM_UART1DM_HCLK_CLK
   // on M7630AAABQMWSA13502040) versus 8x50 (ie. CLKRGM_UART1DM_P_CLK on Q8650BSDCANWSA2233).
   // CLKRGM needs to reconcile this.  For now, since they are not needed on WM, we will disable
   // them to prevent compile errors.
   //
   // The peripheral clocks for GSBI5 through GSBI12 are not enumerated on WM (even on
   // 8x60 where those GSBIs are physically present).  Again, we will disable them for WM
   // to prevent compile errors.

   #define CLKRGM_UART1DM_HCLK_CLK  CLKRGM_NOT_SUPPORTED_CLK
   #define CLKRGM_UART2DM_HCLK_CLK  CLKRGM_NOT_SUPPORTED_CLK

   #define CLKRGM_GSBI5_HCLK_CLK    CLKRGM_NOT_SUPPORTED_CLK
   #define CLKRGM_GSBI6_HCLK_CLK    CLKRGM_NOT_SUPPORTED_CLK
   #define CLKRGM_GSBI7_HCLK_CLK    CLKRGM_NOT_SUPPORTED_CLK
   #define CLKRGM_GSBI8_HCLK_CLK    CLKRGM_NOT_SUPPORTED_CLK
   #define CLKRGM_GSBI9_HCLK_CLK    CLKRGM_NOT_SUPPORTED_CLK
   #define CLKRGM_GSBI10_HCLK_CLK   CLKRGM_NOT_SUPPORTED_CLK
   #define CLKRGM_GSBI11_HCLK_CLK   CLKRGM_NOT_SUPPORTED_CLK
   #define CLKRGM_GSBI12_HCLK_CLK   CLKRGM_NOT_SUPPORTED_CLK
#endif

   switch (device)
   {
      case DALDEVICEID_UARTLG_DEVICE_1:    return CLKRGM_NOT_SUPPORTED_CLK;
      case DALDEVICEID_UARTLG_DEVICE_2:    return CLKRGM_NOT_SUPPORTED_CLK;
      case DALDEVICEID_UARTLG_DEVICE_3:    return CLKRGM_NOT_SUPPORTED_CLK;
      case DALDEVICEID_UARTDM_DEVICE_1:    return CLKRGM_UART1DM_HCLK_CLK;
      case DALDEVICEID_UARTDM_DEVICE_2:    return CLKRGM_UART2DM_HCLK_CLK;
#if (defined (GSBI0_UART_DM_BASE_PHYS) || defined(GSBI0_UART_REG_BASE_PHYS))
      case DALDEVICEID_UARTGSBI_DEVICE_0:  return CLKRGM_GSBI0_HCLK_CLK;
#endif
#if (defined (GSBI1_UART_DM_BASE_PHYS) || defined(GSBI1_UART_REG_BASE_PHYS))
      case DALDEVICEID_UARTGSBI_DEVICE_1:  return CLKRGM_GSBI1_HCLK_CLK;
      case DALDEVICEID_UARTGSBI_DEVICE_2:  return CLKRGM_GSBI2_HCLK_CLK;
      case DALDEVICEID_UARTGSBI_DEVICE_3:  return CLKRGM_GSBI3_HCLK_CLK;
      case DALDEVICEID_UARTGSBI_DEVICE_4:  return CLKRGM_GSBI4_HCLK_CLK;
#endif
#ifdef GSBI5_UART_DM_BASE_PHYS
      case DALDEVICEID_UARTGSBI_DEVICE_5:  return CLKRGM_GSBI5_HCLK_CLK;
      case DALDEVICEID_UARTGSBI_DEVICE_6:  return CLKRGM_GSBI6_HCLK_CLK;
      case DALDEVICEID_UARTGSBI_DEVICE_7:  return CLKRGM_GSBI7_HCLK_CLK;
      case DALDEVICEID_UARTGSBI_DEVICE_8:  return CLKRGM_GSBI8_HCLK_CLK;
      case DALDEVICEID_UARTGSBI_DEVICE_9:  return CLKRGM_GSBI9_HCLK_CLK;
      case DALDEVICEID_UARTGSBI_DEVICE_10: return CLKRGM_GSBI10_HCLK_CLK;
      case DALDEVICEID_UARTGSBI_DEVICE_11: return CLKRGM_GSBI11_HCLK_CLK;
      case DALDEVICEID_UARTGSBI_DEVICE_12: return CLKRGM_GSBI12_HCLK_CLK;
#endif
      default:                             return CLKRGM_NOT_SUPPORTED_CLK;
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
   DALSYS_Free(handle);
}

/*==================================================================================================

FUNCTION: tal_clock_disable

DESCRIPTION:
   Disable all clocks used by the UART core specified when opening the clock handle.

==================================================================================================*/
TAL_RESULT tal_clock_disable(TAL_CLOCK_HANDLE handle)
{
   clkrgm_msm_clk_type core_clock = device_core_clock(handle->dal_id);
   clkrgm_msm_clk_type periph_clock = device_peripheral_clock(handle->dal_id);

   clk_regime_msm_disable(core_clock);

   if ( handle->manage_pclk && (periph_clock != CLKRGM_NOT_SUPPORTED_CLK) )
   {
      clk_regime_msm_disable(periph_clock);
   }

   return TAL_SUCCESS;
}

/*==================================================================================================

FUNCTION: tal_clock_enable

DESCRIPTION:
   Enable all clocks used by the UART core specified when opening the clock handle.

==================================================================================================*/
TAL_RESULT tal_clock_enable(TAL_CLOCK_HANDLE handle)
{
   clkrgm_msm_clk_type core_clock = device_core_clock(handle->dal_id);
   clkrgm_msm_clk_type periph_clock = device_peripheral_clock(handle->dal_id);

   clk_regime_msm_enable(core_clock);

   if ( handle->manage_pclk && (periph_clock != CLKRGM_NOT_SUPPORTED_CLK) )
   {
      clk_regime_msm_enable(periph_clock);
   }

   return TAL_SUCCESS;
}

/*==================================================================================================

FUNCTION: tal_clock_open

DESCRIPTION:
   Open a clock handle for the specified UART device. If successful, this function will return a
   clock handle, and may acquire other resources, which must be released by calling
   tal_clock_close().

==================================================================================================*/
TAL_RESULT tal_clock_open(TAL_CLOCK_HANDLE *handle_ptr, void *device_id)
{
   DALSYS_PROPERTY_HANDLE_DECLARE(property_handle);
   DALSYSPropertyVar property_variable;
   DALResult result;
   TAL_CLOCK_HANDLE handle = NULL;

   if(DALSYS_Malloc(sizeof(TAL_CLOCK_CONTEXT), (void **)&handle) != DAL_SUCCESS)
   {
      goto error;
   }

   handle->dal_id = (DALDEVICEID)client_id;
   handle->manage_pclk = FALSE;

   // Read this Clock Regime-specific parameter from the UART configuration:
   //
   // ManagePCLK: In recent chips there are two clock inputs to the UART HW block: the
   //             core clock and the peripheral bus clock.  The peripheral bus clock
   //             operates the bus interface (it must be enabled when reading/writing
   //             registers).  On some builds CLKREGIM manages the peripheral bus clock
   //             for us and turns it on/off along with the core clock.  On other builds
   //             we must manage the peripheral bus clock ourselves.  This property is
   //             used to specify whether we must manage it ourselves.

   result = DALSYS_GetDALPropertyHandle(handle->dal_id, property_handle);
   if (result != DAL_SUCCESS) { goto error; }

   result = DALSYS_GetPropertyValue(property_handle, "ManagePCLK", 0, &property_variable);
   if (result == DAL_SUCCESS)
   {
      handle->manage_pclk = property_variable.Val.dwVal;
   }

   *handle_ptr = handle;

   return TAL_SUCCESS;

error:

   if (handle != NULL)
   {
      tal_clock_close(handle);
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
   clkrgm_msm_clk_type core_clock = device_core_clock(handle->dal_id);
   clkrgm_uart_speed_type lg_freq = CLKRGM_UART_NUM_SPEEDS;
   clkrgm_uartdm_speed_type dm_freq = CLKRGM_UARTDM_NUM_SPEEDS;
   uint32 result = 0;

#if( defined(_WIN32) || defined(T_QSC6695) )
   // These APIs are currently broken (and cause a linker error) on WM builds.  We will stub
   // them out until CLKRGM releases a fix.  Note that these input clocks are currently hard
   // coded as follows (on Q8650BSDCANWSA2172): UART1 = 19.2MHz, UART2 = 19.2MHz, UART3 = 4.8MHz

   #define clk_regime_sel_uart1_clk(new_clk) {}
   #define clk_regime_sel_uart2_clk(new_clk) {}
   #define clk_regime_sel_uart3_clk(new_clk) {}
#endif

#if (!defined (GSBI1_UART_DM_BASE_PHYS) && !defined(GSBI1_UART_REG_BASE_PHYS))
   #define clk_regime_msm_sel_clk_freq_hz(clk, freq, match) (0)
#endif

   // map frequency to clkrgm enumeration
   switch (core_clock)
   {
      case CLKRGM_UART1_CLK:
      case CLKRGM_UART2_CLK:
      case CLKRGM_UART3_CLK:
         switch (input_freq)
         {
            case 19200000: lg_freq = CLKRGM_UART_SPEED_SIO; break;
            default:
               DALSYS_LogEvent(id, DALSYS_LOGEVENT_ERROR, "set_input_freq: unsupported freq");
               return TAL_ERROR;
         }
         break;
      case CLKRGM_UART1DM_CLK:
      case CLKRGM_UART2DM_CLK:
         switch (input_freq)
         {
            case  7372800: dm_freq = CLKRGM_UARTDM_SPEED_7_3728_MHZ;  break;
            case 14745600: dm_freq = CLKRGM_UARTDM_SPEED_14_7456_MHZ; break;
            case 16000000: dm_freq = CLKRGM_UARTDM_SPEED_16_MHZ;      break;
            case 24000000: dm_freq = CLKRGM_UARTDM_SPEED_24_MHZ;      break;
            case 32000000: dm_freq = CLKRGM_UARTDM_SPEED_32_MHZ;      break;
            case 40000000: dm_freq = CLKRGM_UARTDM_SPEED_40_MHZ;      break;
            case 48000000: dm_freq = CLKRGM_UARTDM_SPEED_48_MHZ;      break;
            case 51200000: dm_freq = CLKRGM_UARTDM_SPEED_51_2_MHZ;    break;
            case 56000000: dm_freq = CLKRGM_UARTDM_SPEED_56_MHZ;      break;
            case 58982400: dm_freq = CLKRGM_UARTDM_SPEED_58_9824_MHZ; break;
            case 64000000: dm_freq = CLKRGM_UARTDM_SPEED_64_MHZ;      break;
            default:
               DALSYS_LogEvent(id, DALSYS_LOGEVENT_ERROR, "set_input_freq: unsupported freq");
               return TAL_ERROR;
         }
         break;
      default:
         break;
   }

   // make clkrgm request to change the input frequency
   switch (core_clock)
   {
      case CLKRGM_UART1_CLK:
         clk_regime_sel_uart1_clk(lg_freq);
         break;
      case CLKRGM_UART2_CLK:
         clk_regime_sel_uart2_clk(lg_freq);
         break;
      case CLKRGM_UART3_CLK:
         clk_regime_sel_uart3_clk(lg_freq);
         break;
      case CLKRGM_UART1DM_CLK:
         clk_regime_sel_uart1dm_clk(dm_freq);
         break;
      case CLKRGM_UART2DM_CLK:
         clk_regime_sel_uart2dm_clk(dm_freq);
         break;
      default:  // GSBI UARTs will use the new generic API
         result = clk_regime_msm_sel_clk_freq_hz(core_clock, input_freq, CLKRGM_FREQ_EXACT);

         if (result != input_freq)
         {
            DALSYS_LogEvent(id, DALSYS_LOGEVENT_ERROR, "set_input_freq: unsupported freq");
            return TAL_ERROR;
         }

         break;
   }

   return TAL_SUCCESS;
}
