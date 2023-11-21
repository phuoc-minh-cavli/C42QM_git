/*============================================================================
  FILE:         uimdrv_clk.c

  OVERVIEW:     FIle conatins the functions to intilalize and control the clk interface.

  DEPENDENCIES: N/A

                Copyright (c) 2012-2014, 2016 QUALCOMM Technologies, Inc.
                All Rights Reserved.
                QUALCOMM Technologies Confidential and Proprietary
============================================================================*/

/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

$Header: //components/rel/uim.mpss/3.3.1/uimdrv/src/hw/clkregime/uimdrv_clk.c#1 $
$DateTime: 2020/07/22 10:53:27 $
$Author: pwbldsvc $

when        who        what, where, why
------      ----       -----------------------------------------------------------
06/13/16    gm         Calculate UART SRC CLK frequency instead of lookup table
06/07/16    gm         F3 optimization
05/30/16    gm         F3 message removal
11/25/14    sam        Check for return value on dal APIs for clock enable and disable
08/25/14    ll         Fix UIM_ATR_FI_VALUE_IS_ZERO_OR_INVALID type mismatch
08/20/14    nmb        9x45 Bring up changes
02/20/14    ak         Maximize the UART clock frequency by setting it directly
02/18/14    sam        Usage of newer style diag macros
02/08/14    sam        Fix for Klocwork issues
01/09/14    akv        Handle to vote for mss bus freq made uim instance specific
10/16/13    akv        Fixed KW errors
10/08/13    nmb        HW header file cleanup
09/19/13    akv        Reference CLK changes to support 4.8MHz
09/16/13    yk         Removed the code for old targets and for old features
07/12/13    nmb        Global Reorganization
02/22/13    js         8x26 bring up changes
02/15/13    ak         Replace MCPM calls with NPA calls to /clk/mss/config_bus
02/12/13    js         General F3 macro cleanup
12/05/12    js         UIM Parallel Processing changes
10/26/12    akv/ssr    HW Enumeration changes
============================================================================*/
#include "uimdrv_clk.h"
#include "uimdrv_uartdm.h"
#include "uimdrv_main.h"
#include "uimglobals.h"
#include "uim_msg.h"


  /* These strings are used to get the clock ids from the clock DAL API*/
#if defined(FEATURE_BOLT_MODEM) || defined(FEATURE_THOR_MODEM)
#define UIM1_DAL_SIM_CLK_NAME    "clk_card_src_uim0"
#define UIM1_DAL_UART_CLK_NAME   "clk_uart_bit_uim0"
#define UIM1_DAL_HCLK_NAME       "clk_bus_uim0"
#define UIM2_DAL_SIM_CLK_NAME    "clk_card_src_uim1"
#define UIM2_DAL_UART_CLK_NAME   "clk_uart_bit_uim1"
#define UIM2_DAL_HCLK_NAME       "clk_bus_uim1"
#define UIM3_DAL_SIM_CLK_NAME    "clk_card_src_uim2"
#define UIM3_DAL_UART_CLK_NAME   "clk_uart_bit_uim2"
#define UIM3_DAL_HCLK_NAME       "clk_bus_uim2"

#else

#define UIM1_DAL_SIM_CLK_NAME    "clk_uim0"
#define UIM1_DAL_UART_CLK_NAME   "clk_xo_uim0"
#define UIM1_DAL_HCLK_NAME       "clk_bus_uim0"
#define UIM2_DAL_SIM_CLK_NAME    "clk_uim1"
#define UIM2_DAL_UART_CLK_NAME   "clk_xo_uim1"
#define UIM2_DAL_HCLK_NAME       "clk_bus_uim1"
#define UIM3_DAL_SIM_CLK_NAME    "clk_uim2"
#define UIM3_DAL_UART_CLK_NAME   "clk_xo_uim2"
#define UIM3_DAL_HCLK_NAME       "clk_bus_uim2"

#endif /* FEATURE_BOLT_MODEM || FEATURE_THOR_MODEM */

#define CLK_NAME_SIZE   20

static char dal_sim_clk_name [][CLK_NAME_SIZE]   = {UIM1_DAL_SIM_CLK_NAME, UIM2_DAL_SIM_CLK_NAME, UIM3_DAL_SIM_CLK_NAME};
static char dal_uart_clk_name [][CLK_NAME_SIZE]  = {UIM1_DAL_UART_CLK_NAME, UIM2_DAL_UART_CLK_NAME, UIM3_DAL_UART_CLK_NAME};
static char dal_hclk_name [][CLK_NAME_SIZE]      = {UIM1_DAL_HCLK_NAME, UIM2_DAL_HCLK_NAME, UIM3_DAL_HCLK_NAME};

/* List of uim clock frequencies that we could use */
const dword uim_clock_frequency[]={
  3840000,   /* UIMDRV_CLK_FREQ_3_84_MHZ */
  4800000,   /* UIMDRV_CLK_FREQ_4_8_MHZ  */
};

/* The following table holds the Baud Rate Adjustment Factors defined
   for the DI value of an ATR or PPS response */
const byte braf_values[UIM_BRAF_SIZE] =
{
   0,        /* DI = 0 - Reserved for future use  */
   1,        /* DI = 1                            */
   2,        /* DI = 2                            */
   4,        /* DI = 3                            */
   8,        /* DI = 4                            */
  16,        /* DI = 5                            */
  32,        /* DI = 6                            */
  64,        /* DI = 7                            */
  12,        /* DI = 8                            */
  20,        /* DI = 9                            */
   0,        /* DI = 10 - Reserved for future use  */
   0,        /* DI = 11 - Reserved for future use  */
   0,        /* DI = 12 - Reserved for future use  */
   0,        /* DI = 13 - Reserved for future use  */
   0,        /* DI = 14 - Reserved for future use  */
   0,        /* DI = 15 - Reserved for future use  */
};


/* The following table holds the Clock Rate Conversion Factors defined
   for the FI value of an ATR or PPS response */
const dword crcf_values[UIM_CRCF_SIZE] =
{
  372,        /* FI = 0                             */
  372,        /* FI = 1                             */
  558,        /* FI = 2                             */
  744,        /* FI = 3                             */
 1116,        /* FI = 4                             */
 1488,        /* FI = 5                             */
 1860,        /* FI = 6                             */
    0,        /* FI = 7  - Reserved for future use  */
    0,        /* FI = 8  - Reserved for future use  */
  512,        /* FI = 9                             */
  768,        /* FI = 10                            */
 1024,        /* FI = 11                            */
 1536,        /* FI = 12                            */
 2048,        /* FI = 13                            */
    0,        /* FI = 14 - Reserved for future use  */
    0,        /* FI = 15 - Reserved for future use  */
};


/* The following table holds the avaliable CSR register value */
const dword csr_values[UIM_CSR_SIZE] =
{
  24576,        /* CSR = 0                             */
  12288,        /* CSR = 1                             */
   6144,        /* CSR = 2                             */
   3072,        /* CSR = 3                             */
   1536,        /* CSR = 4                             */
    768,        /* CSR = 5                             */
    512,        /* CSR = 6                             */
    384,        /* CSR = 7                             */
    256,        /* CSR = 8                             */
    192,        /* CSR = 9                             */
    128,        /* CSR = 10                            */
     96,        /* CSR = 11                            */
     64,        /* CSR = 12                            */
     48,        /* CSR = 13                            */
     32,        /* CSR = 14                            */
     16,        /* CSR = 15                            */
};


extern uim_clock_rate_conversion_factor_type uim_FI_current[UIM_MAX_INSTANCES];
extern uim_baud_rate_adjustment_factor_type  uim_DI_current[UIM_MAX_INSTANCES];


/**
  This function returns a uart clock frequency and a CSR divider
  value for the given fi and di values.  The uart clock
  frequency that is returned is the maximum value given the
  constraints.
  @param  fi:- Clock rate conversion factor.
  @param  di:- Baud rate adjustment factor.
  @param  sim_clk_freq_hz:- The SIM clock frequency.
  @param  uim_pre_mnd_source_clk_freq_hz: - The UART source clk
                                        frequency
  @param  uart_freq_hz_ptr:- Returns the calculated maximum uart
                             freq in hertz
  @param  bit_clk_div_ptr:-  Returns the calculated UART
                         CSRdivider factor

*/
void uim_calc_uart_clk_freq(uint32 fi,
                            uint32 di,
                            const uint32 sim_clk_freq_hz,
                            uint32 uim_pre_mnd_source_clk_freq_hz,
                            uint32 *uart_freq_hz_ptr,
                            uint32 *bit_clk_div_ptr)
{
  boolean b_success = FALSE;
  int     i = 0;
  uint64  uim_data_baudrate_64;
  uint64  uart_bit_gen_limit_64;
  uint64  uim_sim_clk_freq_hz_64  = (uint64)sim_clk_freq_hz;

  /* 
  The purpose of this table is to find the maximum supported 
  UART clock frequency based on a given fi/di pair. 
  
  1. uart_clk = uart_src_clk * (M/N)
  
  Notes:
  a) M/N is external to UARTDM
  b) uart_src_clk is TCXO = 19.2MHz
  c) Due to M/N <= 1/2 restriction in the hardware,
  the uart_clk cannot go beyond uart_src_clk/2
  
  2. uim_bit_rate = sim_clk / (F/D) 
     sim_clk = 3.84MHz or 4.8MHz
     
  3. uart_bit_rate = uart_clk / csr_div_val
  
  The goal is to maximize "uart_clk" with constraint:
  uart_bit_rate * csr_div_val <= uart_src_clk/2.
  
  For example:
  for F= 372, D=1, sim_clk = 3.84MHz, uart_src_clk = 19.2MHz
  
  uim_bit_rate = 3.84MHz/ (372/1) = 10.32 KBps = 10320 bps.
  
  the max "uart_clk" hz = max ( (uim_bit_rate * csr_div) ) <= uart_src_clk/2
  
  max ( (uim_bit_rate, csr_div) ) <= 9600000
  max (
  10320 * 16,
  10320 * 32,
  10320 * 48,
  10320 * 64,
  10320 * 128,
  10320 * 192,
  10320 * 256,
  10320 * 384,
  10320 * 512,
  10320 * 768,
  10320 * 1536,
  10320 * 3072,
  10320 * 6144,
  10320 * 12288,
  10320 * 24576
  ) <= 9600000
  
  Out of this, only 10320 * 768 <= 9600000.
  Therefore, the csr_div_val = 768, and uart_clk = 10320 * 768 = 7925760 Hz.
  */
  uart_bit_gen_limit_64 = (uint64)uim_pre_mnd_source_clk_freq_hz >> 1;

  uim_data_baudrate_64 = ((uim_sim_clk_freq_hz_64*di)<< 20)/fi;

  /* Looping through CSR values from largest to smallest
     to find the largest uart_src_clock with the constrain
     of the SIM data baudrate and pre-mnd clk source for
     UART_SRC_CLK
  */
  for (i=0; i<UIM_CSR_SIZE; i++)
  {
    /* Get the optimized(largest) UART_SRC_CLK frequency and corresponding CSR value
       under the constrain of MND out put should be less than 1/2 of input frequency
    */
    if ( ( uim_data_baudrate_64 * csr_values[i])<= (uart_bit_gen_limit_64 << 20))
    {
      *uart_freq_hz_ptr = ((uim_data_baudrate_64 * csr_values[i]) >> 20);
      *bit_clk_div_ptr  = csr_values[i];
      b_success = TRUE;
      break;
    }
  }/* for */

  /* uim_data_baudrate is too fast. Even with smallest CSR, UART_SRC_CLK frequency
     is exceeding the MND output limit
  */
  if (FALSE == b_success)
  {
    *uart_freq_hz_ptr = ((uim_data_baudrate_64 * csr_values[UIM_MIN_CSR_INDEX]) >> 20);
    *bit_clk_div_ptr  = csr_values[UIM_MIN_CSR_INDEX];
  }
} /* uim_calc_uart_clk_freq */


/**
 @brief   Initializes the CLK Regime.
 @param   uim_instance:- Current UIM task instance.
 @return  None
*/
void initialize_clkregime(uim_instance_enum_type uim_instance)
{
  DALResult                 eRes              = 0;
  uint32                    nFreqHz           = 0;
  uint32                    nUartFreqHz       = 0;
  uint32                    nBitClkDiv        = 0;
  uimUartEnumList           uart_ref;
  uim_instance_global_type *uim_ptr           = uim_get_instance_ptr(uim_instance);

  if (uim_ptr == NULL)
  {
    return;
  }

  uim_hw_if.clkreg[uim_instance].m_pDalClockHandle_ptr = NULL;

  if ((uimdrv_hw_nv_config_info.number_of_active_interfaces == 0x1) &&
      uim_nv_is_feature_enabled(UIMDRV_FEATURE_UIM_SET_CLK_FREQ_AT_4_8_MHZ, uim_ptr) == TRUE )
  {
    uim_hw_if.clkreg[uim_instance].m_simClkSourceFreq            = UIM_TCXO_FREQ;
    uim_hw_if.clkreg[uim_instance].m_simClkFreq                  = uim_clock_frequency[UIMDRV_CLK_FREQ_4_8_MHZ];
    uim_hw_if.clkreg[uim_instance].m_simMaxFreqSupported         = UIMDRV_CLK_FREQ_4_8_MHZ;
    uim_hw_if.clkreg[uim_instance].m_preferred_clock_stop_ctl    = UART_SIM_CFG__SIM_CLK_OFF ;
    uim_hw_if.clkreg[uim_instance].m_preferred_clock_ctl         = UART_SIM_CFG__SIM_CLK_TD8_SEL_MASK;
  }
  else
  {
    uim_hw_if.clkreg[uim_instance].m_simClkSourceFreq            = UIM_TCXO_FREQ;
    uim_hw_if.clkreg[uim_instance].m_simClkFreq                  = uim_clock_frequency[UIMDRV_CLK_FREQ_3_84_MHZ];
    uim_hw_if.clkreg[uim_instance].m_simMaxFreqSupported         = UIMDRV_CLK_FREQ_3_84_MHZ;
    uim_hw_if.clkreg[uim_instance].m_preferred_clock_stop_ctl    = UART_SIM_CFG__SIM_CLK_OFF ;
    uim_hw_if.clkreg[uim_instance].m_preferred_clock_ctl         = UART_SIM_CFG__SIM_CLK_TD8_SEL_MASK;
  }

  /* UART and CLK are mutually dependent on the selected UART of the NV config */
  /* First get the UART reference for given uim_instance */

  /* In case of UIM3 test we set uimUART to BADGER_MSS_UIM2_UART_DM = 2 */
  uart_ref = m_HWConfig.uim_slot_config[uim_instance].uimUart;

  eRes = DAL_DeviceAttach(DALDEVICEID_CLOCK,
                          &(uim_hw_if.clkreg[uim_instance].m_pDalClockHandle_ptr));
  if (DAL_SUCCESS != eRes)
  {
    UIMDRV_MSG_ERR_1(uim_instance,"!!FAILED TO GET CLOCK HANDLE: %d!!", eRes);
    return;
  }
  /* Read all the configuration */
  eRes = DalClock_GetClockId(uim_hw_if.clkreg[uim_instance].m_pDalClockHandle_ptr,
                             dal_sim_clk_name[uart_ref],
                             &(uim_hw_if.clkreg[uim_instance].m_SimClkInfo.nClkId) );

  if (DAL_SUCCESS != eRes)
  {
    UIMDRV_MSG_ERR_2(uim_instance,"!!FAILED TO GET SIM CLOCK ID FOR SLOT %d: %d!!",
                     uart_ref, eRes);
  }
  eRes = DalClock_ResetClock(uim_hw_if.clkreg[uim_instance].m_pDalClockHandle_ptr,
                             uim_hw_if.clkreg[uim_instance].m_SimClkInfo.nClkId,
                             CLOCK_RESET_DEASSERT );

  if (DAL_SUCCESS != eRes)
  {
    UIMDRV_MSG_ERR_2(uim_instance,"!!FAILED TO RESET SIM CLOCK ID FOR SLOT %d: %d!!",
                     uart_ref, eRes);
  }
    /*Set the SIM clock frequency*/
  DalClock_SetClockFrequency(uim_hw_if.clkreg[uim_instance].m_pDalClockHandle_ptr,
                             uim_hw_if.clkreg[uim_instance].m_SimClkInfo.nClkId,
                             uim_hw_if.clkreg[uim_instance].m_simClkFreq,
                             CLOCK_FREQUENCY_HZ_CLOSEST, &nFreqHz);

  eRes = DalClock_GetClockId(uim_hw_if.clkreg[uim_instance].m_pDalClockHandle_ptr,
                             dal_uart_clk_name[uart_ref],
                             &(uim_hw_if.clkreg[uim_instance].m_UartClkInfo.nClkId));

  if (DAL_SUCCESS != eRes)
  {
    UIMDRV_MSG_ERR_2(uim_instance,"!!FAILED TO GET UART%d CLOCK ID: %d!!",
                     uart_ref, eRes);
  }

  eRes = DalClock_ResetClock(uim_hw_if.clkreg[uim_instance].m_pDalClockHandle_ptr,
                             uim_hw_if.clkreg[uim_instance].m_UartClkInfo.nClkId,
                             CLOCK_RESET_DEASSERT );

  if (DAL_SUCCESS != eRes)
  {
    UIMDRV_MSG_ERR_2(uim_instance,"!!FAILED TO RESET UART%d CLOCK ID: %d!!",
                     uart_ref, eRes);
  }

  /* UART Frequency at this point is considered based on default Fi(371) and Di(1) which later
     get modified in setBaudRate Function */
  uim_calc_uart_clk_freq(crcf_values[UIM_CRCF_372_1],
                         braf_values[UIM_BRAF_1],
                         uim_clock_frequency[UIMDRV_CLK_FREQ_3_84_MHZ],
                         UIM_TCXO_FREQ,
                         &nUartFreqHz,
                         &nBitClkDiv);
  /* Reason for this change is that in uim_dev_init, UART CLOCK is enabled in UIM_TCXO_MUST_BE_ON_SLOT
     and UART Frequency is set in SET_UIM_BAUD_RATE_SLOT this sequence of invocation causing
     the UART clock enablement prior to the actual frequency setting.*/
  eRes = DalClock_SetClockFrequency(uim_hw_if.clkreg[uim_instance].m_pDalClockHandle_ptr,
                                    uim_hw_if.clkreg[uim_instance].m_UartClkInfo.nClkId,
                                    nUartFreqHz,
                                    CLOCK_FREQUENCY_HZ_CLOSEST,
                                    &(uim_hw_if.clkreg[uim_instance].m_uartClkActualFreq));

  if (DAL_SUCCESS == eRes)
  {
    /* Program the UART CSR register to the divider value that will give us the right bit rate clock */
    uim_prg_uart_csr_by_div_factor(nBitClkDiv, uim_instance);
  }
  else
  {
    UIMDRV_MSG_ERR_2(uim_instance,"!!FAILED TO SET UART%d Frequency: %d!!",
                     uart_ref, eRes);
    ERR_FATAL("Set Clock Frequency is failed for UART",0,0,0);
  }

  eRes = DalClock_GetClockId(uim_hw_if.clkreg[uim_instance].m_pDalClockHandle_ptr,
                             dal_hclk_name[uart_ref],
                             &(uim_hw_if.clkreg[uim_instance].m_HclkInfo.nClkId));

  if (DAL_SUCCESS != eRes)
  {
    UIMDRV_MSG_ERR_2(uim_instance,"!!FAILED TO GET HCLK%d ID: %d!!",
                     uart_ref, eRes);
  }

  eRes = DalClock_ResetClock(uim_hw_if.clkreg[uim_instance].m_pDalClockHandle_ptr,
                             uim_hw_if.clkreg[uim_instance].m_HclkInfo.nClkId,
                             CLOCK_RESET_DEASSERT );
  if (DAL_SUCCESS != eRes)
  {
    UIMDRV_MSG_ERR_2(uim_instance,"!!FAILED TO RESET HCLK%d ID: %d!!",
                      uart_ref, eRes);
  }

  uim_ptr->hardware.mss_bus_clk_voting_handle =
    npa_create_sync_client("/clk/mss/config_bus", "uim", NPA_CLIENT_REQUIRED);
  if (NULL == uim_ptr->hardware.mss_bus_clk_voting_handle)
  {
    UIMDRV_MSG_ERR_0(uim_instance,
                     "!!FAILED TO OBTAIN NPA HANDLE FOR /CLK/MSS/CONFIG_BUS");
  }
} /* initialize_clkregime */


/**
  @brief  Programs the UIM clock for either a digital value or a frequency.
  @param  uim_instance:- Current UIM task instance.
  @param  uim_clock_control_req:- Clock control req, to set
                               clock LOW, HIGH, TD4, TD5.
  @return None
*/
void setClockControl
(
   uim_instance_enum_type uim_instance,
   uim_clock_control_type uim_clock_control_req
)
{
  switch (uim_clock_control_req)
  {
    /* Program the clock at tcxo/5 */
    case UIMDRV_CLK_FREQ_3_84_MHZ:
      uim_hw_if.clkreg[uim_instance].m_simClkFreq =
        uim_hw_if.clkreg[uim_instance].m_simClkSourceFreq/5;
      /* Evaluate the UIM clock frequency and conversion factor */
      uim_hw_if.clkreg[uim_instance].m_preferred_clock_ctl = 0;
      /* Program the UIM clock */
      prgSIMCFG(uim_instance, UART_SIM_CFG__CLK_MASK,
                   UART_SIM_CFG__SIM_CLK_ON_MASK |
                   uim_hw_if.clkreg[uim_instance].m_preferred_clock_stop_ctl |
                   uim_hw_if.clkreg[uim_instance].m_preferred_clock_ctl);
      break;

    /* Program the clock at tcxo/4 */
    case UIMDRV_CLK_FREQ_4_8_MHZ:
      /* Evaluate the UIM clock frequency and conversion factor */
      uim_hw_if.clkreg[uim_instance].m_simClkFreq =
        uim_hw_if.clkreg[uim_instance].m_simClkSourceFreq/4;
      uim_hw_if.clkreg[uim_instance].m_preferred_clock_ctl =
        UART_SIM_CFG__SIM_CLK_TD4_SEL_MASK;
      /* Program the UIM clock */
      prgSIMCFG(uim_instance, UART_SIM_CFG__CLK_MASK,
                    UART_SIM_CFG__SIM_CLK_ON_MASK |
                    uim_hw_if.clkreg[uim_instance].m_preferred_clock_stop_ctl |
                    uim_hw_if.clkreg[uim_instance].m_preferred_clock_ctl);
      break;

    /* Leave the clock signal HIGH */
    case UIMDRV_CLK_STOP_HIGH:
      uim_hw_if.clkreg[uim_instance].m_preferred_clock_stop_ctl =
        UART_SIM_CFG__SIM_CLK_STOP_HI_MASK;
      prgSIMCFG(uim_instance, UART_SIM_CFG__CLK_MASK,
                    UART_SIM_CFG__SIM_CLK_OFF |
                    uim_hw_if.clkreg[uim_instance].m_preferred_clock_stop_ctl |
                    uim_hw_if.clkreg[uim_instance].m_preferred_clock_ctl);
      break;

    /* Leave the clock signal LOW */
    case UIMDRV_CLK_STOP_LOW:
      uim_hw_if.clkreg[uim_instance].m_preferred_clock_stop_ctl =
        UART_SIM_CFG__SIM_CLK_OFF;
      prgSIMCFG(uim_instance, UART_SIM_CFG__CLK_MASK,
                   UART_SIM_CFG__SIM_CLK_OFF |
                   uim_hw_if.clkreg[uim_instance].m_preferred_clock_stop_ctl |
                   uim_hw_if.clkreg[uim_instance].m_preferred_clock_ctl);
      break;

    default:
      break;
  }
  return;
} /*  setClockControl */


/**
  @brief This procedure determines if the platform can support any {FI, DI}
  combination.
  @param  uim_instance:- Current UIM task instance.
  @param  FI:- Clock rate conversion factor.
  @param  DI:- Baud rate adjustment factor.
  @return TRUE:- If FI/DI values are supported, FALSE - if FI/DI
          values are not supported.
*/
boolean is_fi_di_supported
(
   uim_instance_enum_type    uim_instance,
   uim_clock_rate_conversion_factor_type  FI,
   uim_baud_rate_adjustment_factor_type   DI
)
{
  /*
  The FI (CRCF) and DI (BRAF) that is negotiated determines the baud rate
  that is set on the SIM card, which is:
  uim_clk / (FI / DI) = uim_clk * DI / FI.

  This baud-rate will have to be set on the UART so that UART and the SIM are
  in sync. The baud-rate that is set on the UART is a function of
  {M, N, uart_clk} which is equal to (M / N) * (uart_clk / 16).

  M/N divisor is needed to divide the input clock, and 16 division factor is
  per UART�s design that the UART�s baud-rate is 16 times less than the
  uart_clk frequency. Also, there is a constraint in the M/N counter, whether
  inside the UART or outside, that N/M ratio should be greater than or equal
  to 2.
  Therefore,

  (M/N) * (uart_clk / 16) = (uim_clk) * DI/ FI     --- equation (1)
  N/M >= 2            --- equation (2)

  Solving the above two equations (eliminating M and N terms), we get the
  following:

  ( uart_clk / uim_clk ) >=  (32 * DI / FI)            --- equation (3)

  This means that, a {FI, DI} combination is supported by a platform as long
  as the above equation (3) is satisfied.

  */

  if((FI < UIM_CRCF_SIZE) && (DI < UIM_BRAF_SIZE))
  {
    if( (uim_hw_if.clkreg[uim_instance].m_simClkFreq > 0) &&
        (uim_hw_if.clkreg[uim_instance].m_simClkSourceFreq > 0) &&
        (crcf_values[FI] > 0) && (braf_values[DI] > 0) )
    {
      if ( (uim_hw_if.clkreg[uim_instance].m_simClkSourceFreq / uim_hw_if.clkreg[uim_instance].m_simClkFreq) >=
           ((32 * braf_values[DI]) / crcf_values[FI]) )
      {
        UIMDRV_MSG_HIGH_0(uim_instance,"FI and DI are supported");
        return TRUE;
      }
      else
      {
        return FALSE;
      }
    }
    else
    {
      return FALSE;
    }
  }
  UIMDRV_MSG_HIGH_0(uim_instance,"FI and DI are NOT supported");
  return FALSE;
} /* is_fi_di_supported */


/**
  @brief enables the UIMclocks: SIM, UART, HCLK.
  @param  uim_instance:- Current UIM task instance.
  @return TRUE if operation is a SUCCESS, FALSE if operation
          FAILS.
*/
boolean enableClock(uim_instance_enum_type uim_instance)
{
  DALResult eRes = 0;
  if(uim_hw_if.clkreg[uim_instance].m_pDalClockHandle_ptr == NULL)
  {
    return FALSE;
  }
  if (!(uim_hw_if.clkreg[uim_instance].m_SimClkInfo.bEnabled) )
  {
    eRes = DalClock_EnableClock(uim_hw_if.clkreg[uim_instance].m_pDalClockHandle_ptr,
                             uim_hw_if.clkreg[uim_instance].m_SimClkInfo.nClkId);
    if (DAL_SUCCESS == eRes)
    {
      uim_hw_if.clkreg[uim_instance].m_SimClkInfo.bEnabled = TRUE;
    }
    else
    {
      UIMDRV_MSG_ERR_1(uim_instance,"Enable sim clock failed with error %d", eRes);
    }
  }
  if (!(uim_hw_if.clkreg[uim_instance].m_UartClkInfo.bEnabled) )
  {
    eRes = DalClock_EnableClock(uim_hw_if.clkreg[uim_instance].m_pDalClockHandle_ptr,
                             uim_hw_if.clkreg[uim_instance].m_UartClkInfo.nClkId);
    if (DAL_SUCCESS == eRes)
    {
      uim_hw_if.clkreg[uim_instance].m_UartClkInfo.bEnabled = TRUE;
    }
    else
    {
      UIMDRV_MSG_ERR_1(uim_instance,"Enable UART clock failed with error %d", eRes);
    }
  }
  if (!(uim_hw_if.clkreg[uim_instance].m_HclkInfo.bEnabled) )
  {
    eRes = DalClock_EnableClock(uim_hw_if.clkreg[uim_instance].m_pDalClockHandle_ptr,
                             uim_hw_if.clkreg[uim_instance].m_HclkInfo.nClkId);
    if (DAL_SUCCESS == eRes)
    {
      uim_hw_if.clkreg[uim_instance].m_HclkInfo.bEnabled = TRUE;
    }
    else
    {
      UIMDRV_MSG_ERR_1(uim_instance,"Enable hclock failed with error %d", eRes);
    }
  }
  return TRUE;
} /* enableClock */


/**
  @brief disabled the UIMclocks: SIM, UART, HCLK.
  @param  uim_instance:- Current UIM task instance.
  @return TRUE if operation is a SUCCESS, FALSE if operation
          FAILS.
*/
boolean disableClock(uim_instance_enum_type uim_instance)
{
  DALResult eRes = 0;
  if(uim_hw_if.clkreg[uim_instance].m_pDalClockHandle_ptr == NULL)
  {
    return FALSE;
  }
  if (uim_hw_if.clkreg[uim_instance].m_SimClkInfo.bEnabled)
  {
    eRes = DalClock_DisableClock(uim_hw_if.clkreg[uim_instance].m_pDalClockHandle_ptr,
                              uim_hw_if.clkreg[uim_instance].m_SimClkInfo.nClkId);
    if (DAL_SUCCESS == eRes)
    {
      uim_hw_if.clkreg[uim_instance].m_SimClkInfo.bEnabled = FALSE;
    }
    else
    {
      UIMDRV_MSG_ERR_1(uim_instance,"Disable sim clock failed with error %d", eRes);
    }
  }
  if (uim_hw_if.clkreg[uim_instance].m_UartClkInfo.bEnabled)
  {
    eRes = DalClock_DisableClock(uim_hw_if.clkreg[uim_instance].m_pDalClockHandle_ptr,
                              uim_hw_if.clkreg[uim_instance].m_UartClkInfo.nClkId);
    if (DAL_SUCCESS == eRes)
    {
      uim_hw_if.clkreg[uim_instance].m_UartClkInfo.bEnabled = FALSE;
    }
    else
    {
      UIMDRV_MSG_ERR_1(uim_instance,"Disable UART clock failed with error %d", eRes);
    }
  }
  return TRUE;
} /* disableClock */


/**
  @brief Set the UART clock frequency bases on the UIM baud rate adjustment factor
         nDI, and the UIM clock rate conversion factor, nFI.
  @param  FI , DI
  @result SUCCESS or FAIL
*/
boolean setBaudRate(uim_instance_enum_type uim_instance, uint32  nFI,  uint32  nDI)
{
  uint32    nUartFreqHz       = 0;
  uint32    nBitClkDiv        = 0;
  DALResult eRes              = 0;
  boolean   b_uart_clk_is_set = FALSE;

  if(uim_hw_if.clkreg[uim_instance].m_pDalClockHandle_ptr == NULL)
  {
    return FALSE;
  }

  uim_calc_uart_clk_freq(crcf_values[nFI],
                         braf_values[nDI],
                         uim_hw_if.clkreg[uim_instance].m_simClkFreq,
                         UIM_TCXO_FREQ,
                         &nUartFreqHz,
                         &nBitClkDiv);

  eRes = DalClock_SetClockFrequency(uim_hw_if.clkreg[uim_instance].m_pDalClockHandle_ptr,
                                    uim_hw_if.clkreg[uim_instance].m_UartClkInfo.nClkId,
                                    nUartFreqHz,
                                    CLOCK_FREQUENCY_HZ_CLOSEST,
                                    &(uim_hw_if.clkreg[uim_instance].m_uartClkActualFreq));

  if (DAL_SUCCESS == eRes)
  {
    b_uart_clk_is_set = TRUE;
    /* Program the UART CSR register to the divider value that will give us the right bit rate clock */
    uim_prg_uart_csr_by_div_factor(nBitClkDiv, uim_instance);
  }

  if (FALSE == b_uart_clk_is_set)
  {
    /* If setting the UART clock and divider failed, then let DAL set the UART clock */
    eRes = DalClock_SetUARTBaudClock(uim_hw_if.clkreg[uim_instance].m_pDalClockHandle_ptr,
                                     uim_hw_if.clkreg[uim_instance].m_UartClkInfo.nClkId,
                                     uim_hw_if.clkreg[uim_instance].m_SimClkInfo.nClkId,
                                     braf_values[nDI],
                                     crcf_values[nFI],
                                     &(uim_hw_if.clkreg[uim_instance].m_uartClkActualFreq));

    if (DAL_SUCCESS == eRes)
    {
      b_uart_clk_is_set = TRUE;
      /* SetUARTBaudClock assumes a CSR divider value of 16 */
      prgCSR(uim_instance, CSR_VALUE);
    }
  }
  if(!b_uart_clk_is_set)
  {
    UIM_MSG_ERR_0("setBaudRate: clock is not set");
  }
  return b_uart_clk_is_set;
} /* setBaudRate */


/**
  @brief uim_mss_bus_clk_config
  @param nFreqKhz:- Clock frequency
  @result None
*/
void uim_mss_bus_clk_config(npa_client_handle mss_bus_clk_handle, uint32 nFreqKhz)
{
  if (mss_bus_clk_handle)
  {
    npa_issue_required_request(mss_bus_clk_handle, nFreqKhz);
  }
  else
  {
    UIM_MSG_ERR_0("uim_mss_bus_clk_config: NULL NPA handle");
  }
}/* uim_mss_bus_clk_config */


/*===========================================================================
FUNCTION SET_UIM_BAUD_RATE_SLOT

DESCRIPTION
  Set the UART clock frequency bases on the UIM baud rate adjustment factor
  nDI, and the UIM clock rate conversion factor, nFI.
===========================================================================*/
void SET_UIM_BAUD_RATE_SLOT
(
   uim_instance_enum_type  uim_instance,
   uim_clock_rate_conversion_factor_type  nFI,
   uim_baud_rate_adjustment_factor_type   nDI
)
{
  uim_instance_global_type *uim_ptr = uim_get_instance_ptr(uim_instance);

  if(uim_ptr == NULL)
  {
    UIM_MSG_ERR_0("SET_UIM_BAUD_RATE_SLOT: uim_ptr is NULL");
    return;
  }

  /* Store the current FI and DI */
  uim_ptr->state.FI_current = nFI;
  uim_ptr->state.DI_current = nDI;
  setBaudRate(uim_ptr->id, nFI, nDI);
  return;
} /* SET_UIM_BAUD_RATE_SLOT */


/*===========================================================================
FUNCTION UIM_CLOCK_REGIME_DISABLE_SLOT

DESCRIPTION
  Disables the UIM clocks: SIM, UART, HCLK.
===========================================================================*/
void UIM_CLOCK_REGIME_DISABLE_SLOT(uim_instance_enum_type uim_instance)
{
  disableClock(uim_instance);
  return;
} /* UIM_CLOCK_REGIME_DISABLE_SLOT */


/*===========================================================================
FUNCTION UIM_CLOCK_REGIME_ENABLE_SLOT

DESCRIPTION
  Enables the UIM clocks: SIM, UART, HCLK.
===========================================================================*/
void UIM_CLOCK_REGIME_ENABLE_SLOT(uim_instance_enum_type uim_instance)
{
  enableClock(uim_instance);
  return;
} /* UIM_CLOCK_REGIME_ENABLE_SLOT */


/*===========================================================================

FUNCTION UIM_CLOCK_CONTROL

DESCRIPTION
  Programs the UIM clock for either a digital value or a frequency.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  This changes the clock signal to the UIM.

===========================================================================*/
void uim_clock_control( uim_instance_enum_type uim_instance,
                        uim_clock_control_type uim_clock_control_req )

{
  setClockControl(uim_instance, uim_clock_control_req);
  return;
} /* uim_clock_control */


/*===========================================================================

FUNCTION UIM_ATR_FI_VALUE_IS_ZERO_OR_INVALID

DESCRIPTION
  Checks if ATR FI parameter is 0 or invalid.

DEPENDENCIES
  None

RETURN VALUE
  Boolean  TRUE  - Indicates ATR FI parameter is 0 or invalid.
           FALSE - Indicates FI parameter is greater than 0 and valid.

SIDE EFFECTS
  None

===========================================================================*/
boolean UIM_ATR_FI_VALUE_IS_ZERO_OR_INVALID(uim_instance_enum_type uim_instance)
{
  boolean uim_atr_fi_value_zero_or_invalid = FALSE;
  uim_atr_FI_value_type   FI;
  uint8  uim_atr_TA1_index = 0x2;
  uim_instance_global_type *uim_ptr = uim_get_instance_ptr(uim_instance);
  
  if(uim_ptr == NULL)
  {
    uim_atr_fi_value_zero_or_invalid = TRUE;
    return uim_atr_fi_value_zero_or_invalid;
  }

  /* Get the FI values from the ATR response */
  FI = (uim_atr_FI_value_type)(uim_ptr->atr.atr_buf.data[uim_atr_TA1_index] >> 4);
  UIMDRV_MSG_MED_1(uim_ptr->id,"FI value is %d", FI);  
  switch (FI)
  {
    case FI_0:
    {  /* UIM_CARD_MAX_FREQ_SUPPORTED_4MHZ */
      uim_atr_fi_value_zero_or_invalid = TRUE;
      break;
    }

    case FI_1:  /* UIM_CARD_MAX_FREQ_SUPPORTED_5MHZ */
    case FI_2:  /* UIM_CARD_MAX_FREQ_SUPPORTED_6MHZ */
    case FI_3:  /* UIM_CARD_MAX_FREQ_SUPPORTED_8MHZ */
    case FI_4:  /* UIM_CARD_MAX_FREQ_SUPPORTED_12MHZ */
    case FI_5:  /* UIM_CARD_MAX_FREQ_SUPPORTED_16MHZ */
    case FI_6:  /* UIM_CARD_MAX_FREQ_SUPPORTED_20MHZ */
    case FI_9:  /* UIM_CARD_MAX_FREQ_SUPPORTED_5MHZ */
    case FI_10: /* UIM_CARD_MAX_FREQ_SUPPORTED_7_5_MHZ */
    case FI_11: /* UIM_CARD_MAX_FREQ_SUPPORTED_10MHZ */
    case FI_12: /* UIM_CARD_MAX_FREQ_SUPPORTED_15MHZ */
    case FI_13: /* UIM_CARD_MAX_FREQ_SUPPORTED_20MHZ */
    {
      uim_atr_fi_value_zero_or_invalid = FALSE;
      break;
    }

    case FI_7: /* UIM_CARD_MAX_FREQ_SUPPORTED_INVALID */
    case FI_8:
    case FI_14:
    case FI_15:
    default:
    {
      uim_atr_fi_value_zero_or_invalid = TRUE;
      break;
    }
  }
  return uim_atr_fi_value_zero_or_invalid;
} /* UIM_ATR_FI_VALUE_IS_ZERO_OR_INVALID */


/*===========================================================================

FUNCTION UIM_SWITCH_CLK_FREQ_TO_3_8_MHZ

DESCRIPTION
  Changes the SIM clk frequency to 3.84MHz.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  This changes the clock frequency to the UIM.

===========================================================================*/
void UIM_SWITCH_CLK_FREQ_TO_3_8_MHZ(uim_instance_enum_type uim_instance)
{
  uint32 nFreqHz = 0;
  UIMDRV_MSG_HIGH_0(uim_instance,"UIM_SWITCH_CLK_FREQ_TO_3_8_MHZ");
  uim_hw_if.clkreg[uim_instance].m_simClkFreq  = uim_clock_frequency[UIMDRV_CLK_FREQ_3_84_MHZ];
  /*Set the SIM clock frequency*/
  DalClock_SetClockFrequency(uim_hw_if.clkreg[uim_instance].m_pDalClockHandle_ptr,
                             uim_hw_if.clkreg[uim_instance].m_SimClkInfo.nClkId,
                             uim_hw_if.clkreg[uim_instance].m_simClkFreq,
                             CLOCK_FREQUENCY_HZ_CLOSEST, &nFreqHz);
}/* UIM_SWITCH_CLK_FREQ_TO_3_8_MHZ */


/*===========================================================================

FUNCTION UIM_RESET_CLK_FREQ_TO_DEFAULT_4_8_MHZ

DESCRIPTION
  Resets the SIM clk frequency to 4.8MHz.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Resets the SIM clk frequency to the default 4.8MHz.

===========================================================================*/
void UIM_RESET_CLK_FREQ_TO_DEFAULT_4_8_MHZ(uim_instance_enum_type uim_instance)
{
  uint32 nFreqHz = 0;
  UIMDRV_MSG_HIGH_0(uim_instance,"UIM_SWITCH_CLK_FREQ_TO_DEFAULT_4_8_MHZ");
  uim_hw_if.clkreg[uim_instance].m_simClkFreq  = uim_clock_frequency[UIMDRV_CLK_FREQ_4_8_MHZ];
  /*Set the SIM clock frequency*/
  DalClock_SetClockFrequency(uim_hw_if.clkreg[uim_instance].m_pDalClockHandle_ptr,
                             uim_hw_if.clkreg[uim_instance].m_SimClkInfo.nClkId,
                             uim_hw_if.clkreg[uim_instance].m_simClkFreq,
                             CLOCK_FREQUENCY_HZ_CLOSEST, &nFreqHz);
}/* UIM_RESET_CLK_FREQ_TO_DEFAULT_4_8_MHZ */


/*===========================================================================

FUNCTION UIM_IS_CLK_FREQ_AT_4_8_MHZ

DESCRIPTION
  Checks if the SIM clk frequency is at 4.8MHz.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - UIM CLK at 4.8MHz
  FALSE - UIM CLK at 3.8MHz

SIDE EFFECTS
  None.

===========================================================================*/
boolean UIM_IS_CLK_FREQ_AT_4_8_MHZ(uim_instance_enum_type uim_instance)
{
  boolean uim_clk_freq_at_4_8_mhz = FALSE;
  if (uim_hw_if.clkreg[uim_instance].m_simClkFreq ==
        uim_clock_frequency[UIMDRV_CLK_FREQ_3_84_MHZ])
  {
    uim_clk_freq_at_4_8_mhz = FALSE;
  }
  else if(uim_hw_if.clkreg[uim_instance].m_simClkFreq ==
            uim_clock_frequency[UIMDRV_CLK_FREQ_4_8_MHZ])
  {
    uim_clk_freq_at_4_8_mhz = TRUE;
  }
  return uim_clk_freq_at_4_8_mhz;
}/* UIM_IS_CLK_FREQ_AT_4_8_MHZ */

