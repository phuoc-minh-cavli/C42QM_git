
/*===========================================================================


      R F C  C o m m o n  G S M  c o n t r o l   A P I

                            S o u r c e  F i l e

DESCRIPTION
  This file contains common functions for the RFC.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2011 - 2019 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/rfa.mpss/3.10/rfc/common/src/rfc_common.cpp#2 $
  $DateTime: 2020/09/07 22:27:40 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/02/20   gk      mpll early clock source changes
07/23/19   as      Fixing USTMR Syncup issue
07/05/19   as      Add delay before WTR Chip Init sequence
06/19/19   bm      removed debug changse in rfcommon_bootup_init
04/18/19   bm      Updated rfcommon_bootup_init with chip_init_3 sequence changes
03/06/19   up      Updated rfcommon_bootup_init() to switch to 38.4M RFFE clk
02/28/19   up      reverted switch to 38.4M RFFE clk in  rfcommon_bootup_init()
02/22/19   bm      Updated rfcommon_bootup_init() to switch to 38.4M RFFE clk
02/15/19   up      Updates for bootup init function for 9205 modem.
10/25/18   sk      TRACK B Bringup changes
10/05/18   mns     device snapsot feature.
10/01/18   bm      Removed trim bypass register write in rfcommon_bootup_init()
09/28/18   bm      Updated pwr_ref_clk_req register write in rfcommon_bootup_init() 
09/25/18   bm      Re-ordered the register writes in the function
                   rfcommon_grfc_toggle_ext_registers()
09/24/18   mns     device snapsot feature.
09/24/18   bm      chip_init_v2 sequence updates for boorup_init_sequence
09/20/18   up      Modified for SDR105, renamed rfdevice_rxtx_common_class to 
                   rfdevice_trx_common_intf_class
06/11/18   sk      OFT Fixes
03/03/15   vv      Added support for ADC VCM cal feature
11/18/14   px      Support for sending rffe scan efs dump to rf apps task
11/07/14   ag      Removed redundant message in rfc_common_generate_grfc_script()
10/20/14   vb      ASDiv immediate script support
10/13/14   dr      Adding Featurization for Tabasco
08/15/14   kg      Framework to read RFC EEPROM and populate local structure.
07/18/14   Saul    Convert to using GPIO protection APIs.
05/23/14   vrb     Calling TLMM API to set TLMM_RFFE_CTL Slew Rates as prescribed
12/13/13   cc      Unify gpio type to rfc_sig_cfg_type for FDD and TDD tech
11/26/13   nrk     Fixed compiler warnings 
06/24/13   sar     Replaced memcpy calls with safe memscpy. 
05/29/13   spa     Added delay param to APIs for getting GRFC scripts
04/24/13   APU     If delay is of the special value of 0x7FFF, then just 
                   append the current GRFC transaction to the previous 
                   GRFC transaction with 0 relative delay.
04/02/13   cri     Remove unnecessary error messages
02/27/13   sr      GNSS concurrency fix with critical section lock for sleep and wakeup.
01/11/13   APU     If GRFCs are in config-only mode, skip appending the GRFC
                   scripts.
12/21/12   sr      Put the GRFC/GPIOs to rf-card init_state during wakeup.
12/11/12   hdz     Enabled timing support in rfc_common_get_grfc_overwritten_buffer 
12/09/12   zhw     Make GRFC overwritten API take RFC logic instead of boolean
11/07/12   sbm     API to extract timing information.
10/30/12   sr      Warnings fix
10/24/12   zhw     Support for generating GRFC script with init state
10/16/12   zhw     RF On/Off grfc scripting using overwritten rfc signal
10/11/12   dw      SVVZ bringup updates
10/02/12   vss     Add support for getting grfc script buffer based on signal type 
09/05/12   sr      fixed compiler warnings.
07/21/12   sr      removed the grfc rf engine references.
07/11/12   shb     Updated script_buffer to be of rf_buffer_intf* type 
                   instead of void ptr
07/09/12   sr      changes to use the rfc_data object instead of rfc_ag function ptr table.
07/05/12   sr      Added Err message if the signal type is GPIO while generating scripts
06/29/12   sr      RFC changes : use rf_hal_buffer to generate scripts 
06/29/12   sr      rfc api name change.
06/28/12   sr      dime rfc changes.
05/18/12   sb      made APIs to get grfc number from msm signal and signal type
05/16/12   zw      Refactored grfc script appending API
05/15/12   zw      Added API for appending grfc scripts. 
                   The API will memcopy the entries of source_script into dest_script
                   starting from the first empty slot. GRFC counts in dest_script
                   will also be updated
04/24/12   ac      Added support for rfc binary data
04/05/12   sr      made changes to put all the GPIOs into low power mode during sleep.
04/01/12   lcl     Removed empty mdsp interface header file 
03/26/12   vss     Add API to retrive GRFC engine output type 
03/16/12   aak     Change to base voltage scaling on WTR1605 process  
03/17/12   jfc     Remove deprecated mdsp_intf_v.h include
12/15/11   hdz     Configure GFRC in rfccommon_ini()
11/17/11   aak     Critical section lock for RFC 
11/17/11   sr      Add rfc_common_get_grfc_info()
10/17/11    sr     changed to initialize the grfc signal to default state based 
                   on the rfc-card init state setting info.
09/19/11   aro     Added callback function for NV write
08/26/11   sr      RFC V1.3.35 SS release changes. SS format has changed.
08/20/09   sr      added function to check for the rfc common init status.
07/29/11   Saul    IRAT update. Skip setting GPIOs during IRAT.
07/26/11   Saul    Changed common GRFC script gen API to skip processing GPIOs.
07/15/11   whc     IRAT GRFC script support for autogen RFC  
07/08/11   sr      Added new API to get the GRFC number from list of signals
06/23/11   vb      Fix the infinite loop in rfc_common_init_default()
03/25/11   sr      Initial version
============================================================================*/


/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include <stringl/stringl.h>

extern "C" {
#include "rflm_diag.h"
#include "comdef.h"
#include "rfc_common.h" 
#include "msg.h"
#include "rfcommon_msg.h"
#include "rf_hal_grfc.h"
#include "rf_buffer_intf.h"
#include "rfdevice_cmn_intf.h"
#include "rf_apps_task.h"
#include "rfc_class.h"
#include "rfm_umid_list.h"
#include "rf_hal_rffe.h"
#include "rf_hal_bus_types.h"

/*Includes to read and populate 
the eeprom structure from smem*/
#include "smem.h"
#include "rfc_class.h"
#include "rfdevice_physical_device.h"
#include "rfcommon_data.h"
#include "modem_mem.h"
#include "mcpm_api.h"
}

rf_hal_bus_rffe_type script_bitbang[8];
extern "C" {

static DalDeviceHandle * rfc_tlmm_dal_handle = NULL;

/* rfc Signal info table for the specific RF CARD get initialized once during the init. */
rfc_signal_info_type *rfc_signal_info_table = NULL;
uint32 rfc_signals_num = 0;

/* MSM Signal info table for the Specific MSM, get initializd once during init */ 
rfc_msm_signal_info_type *rfc_msm_signal_info_table = NULL; 
uint32 rfc_msm_signals_num = 0;

} /* extern "C" */

/* Device process */
uint8 device_process[2] = {0,0};

/* this is to keep track of which tech's are active and which rf_path. 
 if all the techs in sleep state, we put all the rf signals into low power state.
 if any of the tech becomes active we need to put all the signals into enable state.
**/ 

typedef struct 
{
  int8 active_techs; /* this is to keep track of how many techs are active */ 
  uint32 vote_mask[RF_PATH_MAX];
} rfc_common_vote_type;

static rfc_common_vote_type rfc_common_vote_info = { 0, {0} };

static rf_rfc_eeprom_data_type rf_rfc_eeprom_data;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Maintains the information on the Common RFC Lock.
 
  @details
  This variable will be referenced through RFC code to acquire and release the locks.
  */ 
static rf_lock_data_type rfc_lock_data;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Callback function for NV Write

  @param wait_sig
  Signal to Wait
*/
void
rfc_common_write_nv_cb
(
  rex_sigs_type wait_sig
)
{
  /* Do rex_wait() on a signal */
  (void) rex_wait(wait_sig);

  return;
} /* rfc_common_write_nv_cb */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function initializes the DAL handle.

  @details
  This function initializes the DAL handle.

  @param None
*/
DALResult rfc_common_init_dal_handle(void)
{
  DALResult dal_status = DAL_SUCCESS;

  /* Replacing bio with DAL equivalent*/
  dal_status = DAL_DeviceAttach(DALDEVICEID_TLMM, &rfc_tlmm_dal_handle);
  if(dal_status == DAL_SUCCESS)
  {
    (void) DalDevice_Open(rfc_tlmm_dal_handle,DAL_OPEN_SHARED);
  }

  return dal_status;
}



/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function actiavtes/de-activates the specified GRFC based on the specified signal logic.

  @details
  This function actiavtes/de-activates the specified GRFC based on the specified signal logic.

  @param grfc:  GRFC to be activated/de-activated.
  @param logic: Signal logic ( HIGH or LOW)
*/
boolean rfc_common_set_grfc(uint8 grfc, rfc_logic_type logic)
{
  rf_hal_bus_result_type result = RF_HAL_BUS_SUCCESS;
  boolean ret_status = TRUE;

  if(RFC_HIGH == logic)
  {
    result = rfhal_grfc_write_single(grfc, TRUE, NULL);
  }
  else if(RFC_LOW == logic)
  {
    result = rfhal_grfc_write_single(grfc, FALSE, NULL);
  }
  else if(logic != RFC_CONFIG_ONLY)
  {
    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfc_common_set_grfc(%d, %d) Invalid GRFC logic!", grfc, logic);
    ret_status = FALSE;
  }

  if (result != RF_HAL_BUS_SUCCESS)
  {
    MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfc_common_set_grfc(%d, %d) failed!, result = %d", grfc, logic, result);
    ret_status = FALSE;
  }

  return ret_status;
}

#if defined(FEATURE_DIMEPM_MODEM) || defined(FEATURE_JOLOKIA_MODEM) || defined(FEATURE_TABASCO_MODEM)
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function sets the specified GPIO signals based on the specified signal logic. 

  @details
  This function sets the specified GPIO signals based on the specified signal logic. 

  @param dal_signal:  DAL Signal (GPIO) to be set.
  @param logic: Signal logic ( HIGH or LOW)
*/
boolean rfc_common_set_gpio(DALGpioSignalType dal_signal, rfc_logic_type logic)
{
  boolean ret_status = FALSE;

  if (rfc_tlmm_dal_handle == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfc_common_tlmm_dal_handle==NULL", 0);
    return FALSE;
  }

  if(RFC_HIGH == logic)
  {
    (void) DalTlmm_GpioOut(rfc_tlmm_dal_handle, dal_signal, DAL_GPIO_HIGH_VALUE);
    ret_status = TRUE;
  }
  else if(RFC_LOW == logic)
  {
    (void) DalTlmm_GpioOut(rfc_tlmm_dal_handle, dal_signal, DAL_GPIO_LOW_VALUE);
    ret_status = TRUE;
  }

  return ret_status;
}
#else
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function sets the specified GPIO signals based on the specified signal logic. 

  @details
  This function sets the specified GPIO signals based on the specified signal logic. 

  @param gpio_id:  GPIO ID.
  @param logic: Signal logic ( HIGH or LOW)
*/
boolean rfc_common_set_gpio( DALGpioIdType gpio_id, rfc_logic_type logic)
{
  boolean ret_status = FALSE;
  DALResult dal_status = DAL_SUCCESS;
  DALGpioValueType dal_gpio_io_val;

  if (rfc_tlmm_dal_handle == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfc_common_tlmm_dal_handle==NULL", 0);
    return FALSE;
  }

  if(RFC_HIGH == logic)
  {
    dal_gpio_io_val = DAL_GPIO_HIGH_VALUE;
    ret_status = TRUE;
  }
  else if(RFC_LOW == logic)
  {
    dal_gpio_io_val = DAL_GPIO_LOW_VALUE;
    ret_status = TRUE;
  }

  /* Config GPIO output state */
  if (ret_status == TRUE)
  {
    dal_status = DalTlmm_GpioIdOut( rfc_tlmm_dal_handle, gpio_id, dal_gpio_io_val );
    if(dal_status != DAL_SUCCESS)
    {
      ERR_FATAL( "rfc_common_set_gpio - DalTlmm_GpioIdOut api failed for GPIO id %d.",
                 gpio_id, 0, 0 );
      ret_status &= FALSE;
    }
  }

  return ret_status;
}
#endif

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function initilizes the requested rf signal into its default state. 

  @details
  This function initilizes the requested rf signal into its default state.

  @param rf_sig_name:  rfc signal to be set to default state. 
*/
void rfc_common_init_rfc_signal(int32 rf_sig_name)
{
  rfc_msm_signal_type msm_signal = RFC_MSM_SIG_NUM;
  rfc_logic_type init_state = RFC_LOGIC_NUM;

  if (rf_sig_name >= (int32)rfc_signals_num)
  {
    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,"Invalid RF signal (%d, %d)", rf_sig_name, rfc_signals_num);
    return;
  }

  /* make sure that the RF-CARD Signal info table has been initialized. which is done during init. */
  if (rfc_signal_info_table == NULL || rfc_signals_num == 0) 
  {
    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,"empty rfc_card_signals table (0x%x, %d)", rfc_signal_info_table, rfc_signals_num);
    return;
  }

  /* make sure that the MSM Signal info table has been initialized. which is done during init. */ 
  if (rfc_msm_signal_info_table == NULL || rfc_msm_signals_num == 0) {
    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,"Empty rfc_msm_mapping_table (0x%x, %d)", rfc_msm_signal_info_table, rfc_msm_signals_num);
    return;
  }

  msm_signal = rfc_signal_info_table[rf_sig_name].msm_sig_name; /* get the MSM Signal name */
  init_state = rfc_signal_info_table[rf_sig_name].init_state;

  if (rfc_msm_signal_info_table[msm_signal].output_type == RFC_GRFC)
  {
    rfc_common_set_grfc(rfc_msm_signal_info_table[msm_signal].grfc_num, init_state);
  }
  else if (rfc_msm_signal_info_table[msm_signal].output_type == RFC_GPIO)
  {
    #if defined(FEATURE_DIMEPM_MODEM) || defined(FEATURE_JOLOKIA_MODEM) || defined(FEATURE_TABASCO_MODEM)
    rfc_common_set_gpio(rfc_signal_info_table[rf_sig_name].dal_signal, init_state);
    #else
    rfc_common_set_gpio(rfc_signal_info_table[rf_sig_name].gpio_id, init_state);
    #endif
  }

}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  returns the GRFC Engine/RF Engine/GPIO signal number  for the corresponding rf_signal and signal
  type.

  @details
  returns the GRFC Engine/RF Engine/GPIO signal number  for the corresponding rf_signal and signal
  type.

  @param rf_sig_name: RF Signal for which GRFC Engine/RF Engine/GPIO number is requested.
  @param sig_type: Signal type for which GRFC Engine/RF Engine/GPIO number should be returned.
*/
int32 rfc_common_get_signal_num(int32 rf_sig_name, rfc_signal_type sig_type)
{
  boolean status = TRUE;
  rfc_msm_signal_type msm_signal = RFC_MSM_SIG_NUM;
  int32 signal_num = -1;

  /* make sure that the rfc common has been initialized */
  status = rfc_common_init_status();
  if (!status) 
  {
    return signal_num;
  }

  msm_signal = rfc_signal_info_table[rf_sig_name].msm_sig_name; /* get the MSM Signal name */

  if (rfc_msm_signal_info_table[msm_signal].signal_type == sig_type)
  {
    signal_num = rfc_common_get_grfc_num(rf_sig_name);
  }

  return signal_num;
}
/*----------------------------------------------------------------------------*/
/*!
  @brief
  returns the grfc number from msm signal

  @details
  returns the grfc number from msm signal

  @param rfc_msm_signal_type: MSM Signal for which GRFC/GPIO number is requested.
*/
int32 rfc_common_get_grfc_num_of_msm_sig (rfc_msm_signal_type msm_signal) 
{

  int32 signal_num = -1;
  switch (rfc_msm_signal_info_table[msm_signal].output_type)
  {
  case RFC_GRFC:
    signal_num = rfc_msm_signal_info_table[msm_signal].grfc_num;
    break;

  case RFC_GPIO:
    /* configure the GPIO to the specified logic */
    signal_num = rfc_msm_signal_info_table[msm_signal].msm_gpio;
    break;

  default:
    break;
  }

  return signal_num;
}
/*----------------------------------------------------------------------------*/
/*!
  @brief
  returns the grfc number from signal type

  @details
  returns the grfc number from signal type

  @param rfc_signal_type: RFC Signal type for which GRFC Engine/RF Engine/GPIO number is requested.
*/

int32 rfc_common_get_grfc_num_of_output_type(rfc_signal_type sig_type)
{
  int32 grfc_num = -1;
  int32 msm_sig_name;
  int8 success = 0;
  for ( msm_sig_name = 0; msm_sig_name < (int32)RFC_MSM_SIG_NUM; msm_sig_name++)
  {
    if (rfc_msm_signal_info_table[msm_sig_name].signal_type == sig_type )
    {
      success = 1;
      grfc_num = rfc_common_get_grfc_num_of_msm_sig ( (rfc_msm_signal_type) msm_sig_name );
    }
  }

  if (success == 0)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC Signal type: %d not found ", sig_type);
  }


  return grfc_num;  //get_grfc_num_of_msm_sig (sig_name)
}

/*----------------------------------------------------------------------------*/

/*!
  @brief
  returns the GRFC Engine/RF Engine/GPIO signal number  for the corresponding rf_signal.
  @details
  returns the GRFC Engine/RF Engine/GPIO signal number  for the corresponding rf_signal.

  @param rf_sig_name: RF Signal for which GRFC Engine/RF Engine/GPIO number is requested.

*/
int32 rfc_common_get_grfc_num(int32 rf_sig_name)
{
  boolean status = FALSE;
  rfc_msm_signal_type msm_signal = RFC_MSM_SIG_NUM;
  int32 signal_num = -1;

  /* make sure that the rfc common has been initialized */
  status = rfc_common_init_status();
  if (!status) 
  {
    return signal_num;
  }

  msm_signal = rfc_signal_info_table[rf_sig_name].msm_sig_name; /* get the MSM Signal name */
  signal_num = rfc_common_get_grfc_num_of_msm_sig (msm_signal);
  return signal_num;
}
/*----------------------------------------------------------------------------*/

/*!
  @brief
  returns the GRFC Engine output type  for the corresponding rf_signal.
  @details
  returns the GRFC Engine output type  for the corresponding rf_signal.

  @param rf_sig_name: RF Signal for which GRFC Engine/RF Engine/GPIO output type is requested.

*/
rfc_gpio_grfc_type rfc_common_get_grfc_output_type(int32 rf_sig_name)
{
  boolean status = FALSE;
  rfc_msm_signal_type msm_signal = RFC_MSM_SIG_NUM;

  /* make sure that the rfc common has been initialized */
  status = rfc_common_init_status();
  if (!status) 
  {
    return RFC_GPIO_GRFC_INVALID;
  }

  msm_signal = rfc_signal_info_table[rf_sig_name].msm_sig_name; /* get the MSM Signal name */

  return rfc_msm_signal_info_table[msm_signal].output_type;
}
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function checks whether the rfc common has ben initialized or not. If initialized returns
  TRUE.

  @details
  This function checks whether the rfc common has ben initialized or not. If initialized returns
  TRUE.

  @param None.
*/
boolean rfc_common_init_status()
{
  /* make sure that the RF-CARD Signal info table has been initialized. which is done during init. */
  if (rfc_signal_info_table == NULL || rfc_signals_num == 0) 
  {
    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,"empty rfc_card_signals table (0x%x, %d)", rfc_signal_info_table, rfc_signals_num);
    return FALSE;
  }

  /* make sure that the MSM Signal info table has been initialized. which is done during init. */ 
  if (rfc_msm_signal_info_table == NULL || rfc_msm_signals_num == 0) {
    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,"Empty rfc_msm_mapping_table (0x%x, %d)", rfc_msm_signal_info_table, rfc_msm_signals_num);
    return FALSE;
  }

  return TRUE;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initializes the RFC common module. 

  @details
  This function configures the specified GRFC engines, GRFC RF engines and GPIO signals to their
  default state. Enables the specified GPIOs using DAL interface. This function will be called once
  per power-up.

  @param rfc_sig_table: RFC Signal info table, which will be used for the specific RF CARD.
  @param num_signals: Number of RF Signals used in the specified RF CARD.
*/
#if defined(FEATURE_DIMEPM_MODEM) || defined(FEATURE_JOLOKIA_MODEM) || defined(FEATURE_TABASCO_MODEM)
boolean rfc_common_init(rfc_signal_info_type *rfc_sig_table, int32 num_signals)
{
  DALResult dal_status = DAL_SUCCESS;
  rfc_msm_signal_type msm_signal = RFC_MSM_SIG_NUM;
  uint16 i = 0;

  rfc_signal_info_table = rfc_sig_table; /* initialize the rfc_card_table and which will be used later */ 
  rfc_signals_num = num_signals;

  if (rfc_signal_info_table == NULL || rfc_signals_num == 0)
  {
    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,"empty rfc_card_signals table (0x%x, %d)", rfc_signal_info_table, rfc_signals_num);
    return FALSE;
  }

  rfc_msm_signals_num = rfc_msm_sig_info_table_get(&rfc_msm_signal_info_table);
  if (rfc_msm_signal_info_table == NULL || rfc_msm_signals_num == 0)
  {
    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,"Empty rfc_msm_mapping_table (0x%x, %d)", rfc_msm_signal_info_table, rfc_msm_signals_num);
    return FALSE;
  }

  dal_status = rfc_common_init_dal_handle();
  if(dal_status != DAL_SUCCESS)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"DAL Device failed to attach!!!", 0);
    return FALSE;
  }

  DalTlmm_SetPort(rfc_tlmm_dal_handle, DAL_TLMM_RFFE_CTL, 0x1);

  #ifndef TEST_FRAMEWORK
  for (i = 0; i < rfc_signals_num ; i++)
  {

    msm_signal = rfc_signal_info_table[i].msm_sig_name;

    if (rfc_msm_signal_info_table[msm_signal].output_type == RFC_GRFC)
    {
      rfc_common_set_grfc(rfc_msm_signal_info_table[msm_signal].grfc_num,
                                 rfc_signal_info_table[i].init_state);
    }

    if (rfc_msm_signal_info_table[msm_signal].msm_gpio != RFC_GPIO_INVALID)
    {
      /* construct the DAL Signal */
      rfc_signal_info_table[i].dal_signal = DAL_GPIO_CFG(rfc_msm_signal_info_table[msm_signal].msm_gpio, \
                                                         rfc_msm_signal_info_table[msm_signal].function_select, \
                                                         rfc_msm_signal_info_table[msm_signal].direction, \
                                                         rfc_signal_info_table[i].gpio_pull, \
                                                         rfc_signal_info_table[i].gpio_drive);
      /* Disable the DAL Signal  */
      (void) DalTlmm_ConfigGpio(rfc_tlmm_dal_handle, rfc_signal_info_table[i].dal_signal, DAL_TLMM_GPIO_DISABLE);
      }
  } /* end of for() */

  #endif

  return TRUE;
}
#else
boolean rfc_common_init(rfc_signal_info_type *rfc_sig_table, int32 num_signals)
{
  boolean api_status = TRUE;
  DALResult dal_status = DAL_SUCCESS;
  rfc_msm_signal_type msm_signal = RFC_MSM_SIG_NUM;
  uint16 i = 0;
  DalTlmm_GpioConfigIdType dal_gpio_settings;

  rfc_signal_info_table = rfc_sig_table; /* initialize the rfc_card_table and which will be used later */ 
  rfc_signals_num = num_signals;

  if (rfc_signal_info_table == NULL || rfc_signals_num == 0)
  {
    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,"empty rfc_card_signals table (0x%x, %d)", rfc_signal_info_table, rfc_signals_num);
    return FALSE;
  }

  rfc_msm_signals_num = rfc_msm_sig_info_table_get(&rfc_msm_signal_info_table);
  if (rfc_msm_signal_info_table == NULL || rfc_msm_signals_num == 0)
  {
    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,"Empty rfc_msm_mapping_table (0x%x, %d)", rfc_msm_signal_info_table, rfc_msm_signals_num);
    return FALSE;
  }

  dal_status = rfc_common_init_dal_handle();
  if(dal_status != DAL_SUCCESS)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"DAL Device failed to attach!!!", 0);
    return FALSE;
  }

  DalTlmm_SetPort(rfc_tlmm_dal_handle, DAL_TLMM_RFFE_CTL, 0x1);

  for (i = 0; (i < rfc_signals_num) && (api_status == TRUE) ; i++)
  {

    msm_signal = rfc_signal_info_table[i].msm_sig_name;

    if (rfc_msm_signal_info_table[msm_signal].output_type == RFC_GRFC)
    {
      rfc_common_set_grfc(rfc_msm_signal_info_table[msm_signal].grfc_num,
                                 rfc_signal_info_table[i].init_state);
    }

    if (rfc_msm_signal_info_table[msm_signal].msm_gpio != RFC_GPIO_INVALID)
    {
      /* Get GPIO ID. Store GPIO ID. Also locks GPIO */
      if (api_status == TRUE)
      {
        dal_status = DalTlmm_GetGpioId( rfc_tlmm_dal_handle, 
                                        rfc_msm_signal_info_table[msm_signal].tlmm_gpio_name, 
                                        &rfc_signal_info_table[i].gpio_id );
        if(dal_status != DAL_SUCCESS)
        {
          ERR_FATAL( "rfc_common_init - DalTlmm_GetGpioId api failed for GPIO [num %d, id %d]. "
                     "Check GPIO Name validity in signal info.",
                     rfc_msm_signal_info_table[msm_signal].msm_gpio, rfc_signal_info_table[i].gpio_id, 0);
          api_status &= FALSE;
        }
      }

      /* Config GPIO Settings */
      if (api_status == TRUE)
      {
        dal_gpio_settings.eDirection = rfc_msm_signal_info_table[msm_signal].direction;
        dal_gpio_settings.eDriveStrength = rfc_signal_info_table[i].gpio_drive;
        dal_gpio_settings.ePull = rfc_signal_info_table[i].gpio_pull;

        dal_status = DalTlmm_ConfigGpioId( rfc_tlmm_dal_handle, rfc_signal_info_table[i].gpio_id, 
                                           &dal_gpio_settings );
        if(dal_status != DAL_SUCCESS)
        {
          ERR_FATAL( "rfc_common_init - DalTlmm_ConfigGpioId api failed for GPIO  [num %d, id %d]. "
                     "Check GPIO settings.",
                     rfc_msm_signal_info_table[msm_signal].msm_gpio, rfc_signal_info_table[i].gpio_id, 0);
          api_status &= FALSE;
        }
      }

      /* Config GPIO to Disabled state */
      if (api_status == TRUE)
      {
        dal_status = DalTlmm_ConfigGpioIdInactive( rfc_tlmm_dal_handle, rfc_signal_info_table[i].gpio_id );
        if(dal_status != DAL_SUCCESS)
        {
          ERR_FATAL( "rfc_common_init - DalTlmm_ConfigGpioIdInactive api failed for GPIO [num %d, id %d].",
                     rfc_msm_signal_info_table[msm_signal].msm_gpio, rfc_signal_info_table[i].gpio_id, 0);
          api_status &= FALSE;
        }
      }
      }
  } /* end of for() */

  return api_status;
}
#endif

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function Initializes the specified RF signals into their default state.

  @details
  This function Initializes the specified RF signals into their default state.

  @param rfc_signals: List of RF Signals which need to placed into their default state.
*/
boolean rfc_common_init_default(rfc_sig_cfg_type *rfc_signals)
{
  boolean status = TRUE;
  uint16 i = 0;
  int32 rf_sig_name = RFC_SIG_LIST_END;

  if (rfc_signals == NULL) /* tech specific rf signals to be configured */
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfc_signals==NULL", 0);
    return FALSE;
  }

  /* loop through the signal list and configure them to specified default state */
  while (rfc_signals[i].sig_name != RFC_SIG_LIST_END)
  {
    rf_sig_name = rfc_signals[i].sig_name; /* get the RF-CARD Signal name */

    rfc_common_init_rfc_signal(rf_sig_name);

    i++;
  }

  return status;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function configuers the specified RF signals to the specified state(HIGH/LOW).

  @details
  This function configuers the specified RF signals to the specified state(HIGH/LOW). It also setup
  the RFD routing table for the GRFC and RF GRFC engines.

  @param rf_signals: List of RF signals and the logic information to be configured.
*/
boolean rfc_common_config_signal(rfc_sig_cfg_type *rf_signals)
{
  uint16 i = 0;
  boolean status = TRUE;
  rfc_msm_signal_type msm_signal = RFC_MSM_SIG_NUM;
  int32 rf_sig_name = RFC_SIG_LIST_END;

  if (rf_signals == NULL) /* tech specific rf signals to be configured */
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rf_signals==NULL", 0);
    return FALSE;
  }

  /* make sure that the rfc common has been initialized */
  status = rfc_common_init_status();
  MSG_1(MSG_SSID_RF, MSG_LEGACY_LOW,"rfc_common_init_status status :%d", status);
  if (!status) 
  {
    return status;
  }

  /* loop through the signal list and configure them based on their type to specified logic  */
  while (rf_signals[i].sig_name != RFC_SIG_LIST_END)
  {
    rf_sig_name = rf_signals[i].sig_name; /* get the RF-CARD Signal name */
    msm_signal = rfc_signal_info_table[rf_sig_name].msm_sig_name; /* get the MSM Signal name */
    MSG_2(MSG_SSID_RF, MSG_LEGACY_LOW,"rf_signals[i].sig_name i:%d signal Name:%d", i, rf_signals[i].sig_name);
    switch(rfc_msm_signal_info_table[msm_signal].output_type)
    {
    case RFC_GRFC:

      /* Set the specified GRFC Engine logic */
      status &= rfc_common_set_grfc(rfc_msm_signal_info_table[msm_signal].grfc_num, 
                                           rf_signals[i].start.logic);
      MSG_1(MSG_SSID_RF, MSG_LEGACY_LOW,"rfc_common_set_grfc status :%d", status);
      break;

    case RFC_GPIO:
      /* configure the GPIO to the specified logic */
      #if defined(FEATURE_DIMEPM_MODEM) || defined(FEATURE_JOLOKIA_MODEM) || defined(FEATURE_TABASCO_MODEM)
      status &= rfc_common_set_gpio(rfc_signal_info_table[rf_sig_name].dal_signal, rf_signals[i].start.logic);
      #else
      status &= rfc_common_set_gpio(rfc_signal_info_table[rf_sig_name].gpio_id, rf_signals[i].start.logic);
      #endif
      break;

    case RFC_SSBI: /* do nothing for now */
      break;

    default:
      break;
    }

    i++;
  }

  return status;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function finds the particular signal in the list of signals and return the correspoding
  signal number (GRFC ENGINE NUM, GRFC RF _ENGINE NUM or GPIO NUM).

  @details
  This function finds the particular signal in the list of signals and return the correspoding
  signal number (GRFC ENGINE NUM, GRFC RF _ENGINE NUM or GPIO NUM).

  @param
  rf_signals: List of RF signals.
  sig_type : the Signal type for which the Signal number need to be returned.
*/
int32 rfc_common_get_grfc_engine_num(rfc_sig_cfg_type *rf_signals, rfc_signal_type sig_type)
{
  uint16 i = 0;
  int32 rf_sig_name = RFC_SIG_LIST_END;
  int32 signal_num = -1;

  if (rf_signals == NULL) /* tech specific rf signals to be configured */
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rf_signals==NULL", 0);
    return signal_num;
  }

  /* loop through the signal list and configure them based on their type to specified logic  */
  while (rf_signals[i].sig_name != RFC_SIG_LIST_END)
  {
    rf_sig_name = rf_signals[i].sig_name; /* get the RF-CARD Signal name */

    signal_num = rfc_common_get_signal_num(rf_sig_name, sig_type);
    if (signal_num >= 0)
    {
      break;
    }

    i++;
  }

  return signal_num;
}


/*! 
  @brief
  Maintains the information on the Common RFC Lock.
 
  @details
  Referenced through RFC code to acquire and release the locks.
  */ 
rf_lock_data_type* rfc_get_lock_data(void)
{
  return &rfc_lock_data;
} /* rfc_get_lock_data */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function finds the particular signal in the list of signals and return the correspoding
  signal information (GRFC ENGINE NUM, GRFC RF _ENGINE NUM or GPIO NUM).

  @details
  This function finds the particular signal in the list of signals and return the correspoding
  signal information (GRFC ENGINE NUM, GRFC RF _ENGINE NUM or GPIO NUM).

  @param
  rf_signals: List of RF signals.
  sig_type : the Signal type for which the Signal number need to be returned.
  rfc_grfc_list : signal information like GRFC Number, start/stop delta and logic, grfc output type.
*/
int32 rfc_common_get_grfc_info(rfc_sig_cfg_type *rf_signals, rfc_signal_type sig_type,
                               rfc_sig_cfg_type *rfc_grfc_list)
{
  uint16 i = 0;
  int32 rf_sig_name = RFC_SIG_LIST_END;
  int32 grfc_num = -1;
  int32 num_signals = 0;
  rfc_gpio_grfc_type rf_grfc_out_type;

  if (rf_signals == NULL) /* tech specific rf signals to be configured */
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rf_signals==NULL", 0);
    return num_signals;
  }

  if (rfc_grfc_list == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfc_grfc_list==NULL", 0);
    return num_signals;
  }

  /* loop through the signal list and configure them based on their type to specified logic  */
  while (rf_signals[i].sig_name != RFC_SIG_LIST_END)
  {
    rf_sig_name = rf_signals[i].sig_name; /* get the RF-CARD Signal name */

    grfc_num = rfc_common_get_signal_num(rf_sig_name, sig_type);

    /*get GRFC signal output type*/
    rf_grfc_out_type = rfc_common_get_grfc_output_type(rf_sig_name);

    if(rf_grfc_out_type >= RFC_GPIO_GRFC_INVALID)
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_LOW, 
            "Invalid GPIO_GRFC type: %d",rf_grfc_out_type);
    }	

    if (grfc_num >= 0)
    {
      rfc_grfc_list[num_signals].sig_name = grfc_num;
      rfc_grfc_list[num_signals].start.logic = rf_signals[i].start.logic;
      num_signals++;

      if(num_signals >= RFC_MAX_NUM_OF_GRFC)
      {
        MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, 
              "GRFC script exceed MAX count. count: %d, MAX: %d",num_signals,RFC_MAX_NUM_OF_GRFC);
        break;
      }

    }

    i++;
  }

  return num_signals;
}

/*----------------------------------------------------------------------------*/


/*!
  @brief
  This function finds the particular timing information from the timing table.

  @details
  This function finds the particular timing information from the timing table.

  @param
  device: device type.
  rf_band : Rf band 
  timing_type : tming signa
  rfc_timing : pointer to store the timing informatin for specific timing_type.
*/
boolean rfc_common_get_timing_info(rfc_timing_info_type *timing_info_tbl,
                                   rfc_timing_param_type timing_type,int32 *rfc_timing)
{
  uint16 i = 0;
 
  if (timing_type >= RFC_TIMING_PARAMS_NUM)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"timing_type Invalid", timing_type);
    return FALSE;
  }

  if (timing_info_tbl == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"timing info tbl is NULL");
    return FALSE;
  }

  /* loop through the signal list and configure them based on their type to specified logic  */
  while (timing_info_tbl[i].timing_param != RFC_TIMING_PARAMS_NUM)
  {
    if (timing_info_tbl[i].timing_param == timing_type)
    {
      *rfc_timing = timing_info_tbl[i].timing;
      break;
    }
    i++;
  }

  return TRUE;
}

/*----------------------------------------------------------------------------*/

/*!
  @brief
  This function populates the rf_buffer with the specified rfc signal list.

  @details
  This function populates the rf_buffer with the specified rfc signal list.

  @param rfc_signal_list: List of RF signals.
  rf_buf_ptr : ptr to rf_buffer to store the rfc signals.
*/
boolean rfc_common_generate_grfc_script(rfc_sig_cfg_type *rfc_signal_list, 
                                        rf_buffer_intf *rf_buffer, int16 delay ) 
{
    uint8 sig_idx = 0;
    rfc_msm_signal_type msm_signal = RFC_MSM_SIG_NUM;
    int32 rf_sig_name = RFC_SIG_LIST_END;

    if(rfc_signal_list == NULL || rf_buffer == NULL)
    {
      MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, 
            "rfc_common_generate_grfc_script got NULL ptr. rfc_signal_list= %x, rf_buffer = %x",rfc_signal_list,rf_buffer);
      return FALSE;
    }

    /* For each signal in rfc_signal_list, look up GRFC engine number and then fill in grfc_buf_ptr.  */ 
    for(sig_idx = 0; rfc_signal_list[sig_idx].sig_name != RFC_SIG_LIST_END; sig_idx++ )
    {
      rf_sig_name = rfc_signal_list[sig_idx].sig_name; /* get the RF-CARD Signal name */      
      msm_signal = rfc_signal_info_table[rf_sig_name].msm_sig_name; /* get the MSM Signal name */
	  
      /* MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR," rfc_common_generate_grfc_script for loop entered",0); */ 
      
      if (rfc_signal_list[sig_idx].start.logic == RFC_LOW ||  rfc_signal_list[sig_idx].start.logic == RFC_HIGH)
      {
        if (rfc_msm_signal_info_table[msm_signal].output_type == RFC_GPIO)
        {
          /* configure the GPIO to the specified logic */
          #if defined(FEATURE_DIMEPM_MODEM) || defined(FEATURE_JOLOKIA_MODEM) || defined(FEATURE_TABASCO_MODEM)
          rfc_common_set_gpio(rfc_signal_info_table[rf_sig_name].dal_signal, rfc_signal_list[sig_idx].start.logic);
          #else
          rfc_common_set_gpio(rfc_signal_info_table[rf_sig_name].gpio_id, rfc_signal_list[sig_idx].start.logic);
          #endif
          MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR," Creating GRFC scripts. GPIO signal not expected!: Configuring the GPIO(%d) to %d", 
                rfc_msm_signal_info_table[msm_signal].msm_gpio, rfc_signal_list[sig_idx].start.logic);
        }
        else
        { /* for GRFCs put into to the script buffer */
          rf_buffer->append_grfc(rfc_msm_signal_info_table[msm_signal].grfc_num, 
                                 rfc_signal_list[sig_idx].start.logic, delay ); 
        /*  MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR," rfc_common_generate_grfc_script append GRFC is called",0);  */
        }
      }
    }

    return TRUE;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function populates the rf_buffer with the specified rfc signal list,
  in addition, the signals are all at default state.

  @details
  This function populates the rf_buffer with the specified rfc signal list.
  In addition, the signals are all at default state.

  @param rfc_signal_list
  List of RF signals.  
 
  @param rf_buf_ptr
  Ptr to rf_buffer to store the rfc signals.
*/
boolean rfc_common_generate_init_grfc_script(rfc_sig_cfg_type *rfc_signal_list, 
                                             rf_buffer_intf *rf_buffer, int16 delay) 
{
    uint8 sig_idx = 0;
    rfc_msm_signal_type msm_signal = RFC_MSM_SIG_NUM;
    int32 rf_sig_name = RFC_SIG_LIST_END;
    rfc_logic_type init_state = RFC_LOGIC_NUM;

    if(rfc_signal_list == NULL || rf_buffer == NULL)
    {
      MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, 
            "rfc_common_generate_init_grfc_script got NULL ptr. rfc_signal_list= %x, rf_buffer = %x",rfc_signal_list,rf_buffer);
      return FALSE;
    }

    /* For each signal in rfc_signal_list, look up GRFC engine number and then fill in grfc_buf_ptr.  */ 
    for(sig_idx = 0; rfc_signal_list[sig_idx].sig_name != RFC_SIG_LIST_END; sig_idx++ )
    {
      rf_sig_name = rfc_signal_list[sig_idx].sig_name; /* get the RF-CARD Signal name */      
      msm_signal = rfc_signal_info_table[rf_sig_name].msm_sig_name; /* get the MSM Signal name */
      init_state = rfc_signal_info_table[rf_sig_name].init_state;

      if (rfc_signal_list[sig_idx].start.logic == RFC_LOW ||  rfc_signal_list[sig_idx].start.logic == RFC_HIGH)
      {
        if (rfc_msm_signal_info_table[msm_signal].output_type == RFC_GPIO)
        {
          /* configure the GPIO to the specified logic */
          #if defined(FEATURE_DIMEPM_MODEM) || defined(FEATURE_JOLOKIA_MODEM) || defined(FEATURE_TABASCO_MODEM)
          rfc_common_set_gpio(rfc_signal_info_table[rf_sig_name].dal_signal, init_state);
          #else
          rfc_common_set_gpio(rfc_signal_info_table[rf_sig_name].gpio_id, init_state);
          #endif
          MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR," Creating GRFC scripts. GPIO signal not expected!: Configuring the GPIO(%d) to %d", 
                rfc_msm_signal_info_table[msm_signal].msm_gpio, rfc_signal_list[sig_idx].start.logic);
        }
        else
        { /* for GRFCs put into to the script buffer */
          if (init_state == RFC_LOW || init_state == RFC_HIGH)
          {
            rf_buffer->append_grfc(rfc_msm_signal_info_table[msm_signal].grfc_num, 
                                   init_state, delay ); 
          }
        }
      }
    }

    return TRUE;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function keep track of the rfc voting based on the path and technologies.

  @details
  This function keep track of the votes based on path and techs. This voting information will be
  used to decide whethere to put all the RF Signals into low power mode or to be enabled (active
  mode).

  @param
  path: RF_PATH for which voting need to be done.
  mode : technology which is voting.
  vote : TRUE - cast vote; FALSE - decast vote.
  active_techs : number of techs active.
*/
int8 rfc_common_vote(rf_path_enum_type path, rfm_mode_enum_type mode, boolean vote, int8 active_techs)
{ 
  uint32 vote_mask = 0x0;

  vote_mask = rfc_common_vote_info.vote_mask[path];
  if (vote)
  {
    rfc_common_vote_info.vote_mask[path] |= (1U << mode);
    if (vote_mask != rfc_common_vote_info.vote_mask[path])
    {
      active_techs++;
    }
  }
  else
  {
    rfc_common_vote_info.vote_mask[path] &= ~(1U << mode);
    if (vote_mask != rfc_common_vote_info.vote_mask[path])
    {
      active_techs--;
    }
  }

  MSG_4(MSG_SSID_RF, MSG_LEGACY_HIGH," RFC vote : path=%d mode=%d vote=%d active_techs=%d", path, mode, vote, active_techs);

  return active_techs;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function puts the rf signals(GPIOs) into active mode of operation if the signals are in low
  power mode.

  @details
  This function puts the rf signals(GPIOs) into active mode of operation if the signals are in low
  power mode. If the signals are already in active mode of operation then nothing will be done.
  It will also cast the RFC Vote for the requested path and tech.
 
  @param
  path: RF_PATH which is requesting the signals to be enabled.
  mode : technology which is requesting the signals to be enabled.
*/
#if defined(FEATURE_DIMEPM_MODEM) || defined(FEATURE_JOLOKIA_MODEM) || defined(FEATURE_TABASCO_MODEM)
boolean rfc_common_wakeup(rf_path_enum_type path, rfm_mode_enum_type mode)
{
  uint16 i = 0;
  boolean status = TRUE;
  DALResult result = DAL_SUCCESS;
  uint32 msm_gpio = 0;
  rfc_msm_signal_type msm_signal;
  int8 new_active_techs = 0;

  /* Protect the rfc sleep/wakeup functions with critical section */
  rf_common_enter_critical_section(rfc_get_lock_data());

  /* make sure that the rfc common has been initialized */
  status = rfc_common_init_status();
  if (!status) 
  {
    rf_common_leave_critical_section(rfc_get_lock_data());
    return status;
  }

  /* cast the rfc vote */
  new_active_techs = rfc_common_vote(path, mode, TRUE, rfc_common_vote_info.active_techs);

  if (rfc_common_vote_info.active_techs != 0)
  {
    /* one of the tech is already active so, there is no need to enable the rf signals, so just
       account the new tech's vote and return */
    rfc_common_vote_info.active_techs = new_active_techs;
    rf_common_leave_critical_section(rfc_get_lock_data());
    return status;
  }

  /* no tech was active before, so, we have to enable all the rf signals. */
  rfc_common_vote_info.active_techs = new_active_techs; /* active techs shoule be ONE here */

  MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH,"Enabling the rf signals from low power mode (%d)", rfc_common_vote_info.active_techs);

  /* configure all the rf signals into active mode of operation */
  for (i = 0; i < rfc_signals_num ; i++)
  {

    msm_signal = rfc_signal_info_table[i].msm_sig_name;
    msm_gpio = rfc_msm_signal_info_table[msm_signal].msm_gpio;
#ifndef TEST_FRAMEWORK
    if ( msm_gpio != RFC_GPIO_INVALID )
    {
      /* Disable the DAL Signal, which will put the signal into inactive state.  */
      result = DalTlmm_ConfigGpio(rfc_tlmm_dal_handle, rfc_signal_info_table[i].dal_signal, DAL_TLMM_GPIO_ENABLE);
      if (result != DAL_SUCCESS)
      {
        MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,"Error while configuring dal_signal(0x%x) to wakeup: error %d", rfc_signal_info_table[i].dal_signal, result);
      }
      else
      {
        /* initialize the GRFC/GPIO state according to the default init_state*/
        rfc_common_init_rfc_signal((int32)i);
      }
    }
#endif     
  }

  rf_common_leave_critical_section(rfc_get_lock_data());
  return status;
}
#else
boolean rfc_common_wakeup(rf_path_enum_type path, rfm_mode_enum_type mode)
{
  uint16 i = 0;
  boolean status = TRUE;
  DALResult result = DAL_SUCCESS;
  uint32 msm_gpio = 0;
  rfc_msm_signal_type msm_signal;
  int8 new_active_techs = 0;
  DalTlmm_GpioConfigIdType dal_gpio_settings;

  /* Protect the rfc sleep/wakeup functions with critical section */
  rf_common_enter_critical_section(rfc_get_lock_data());

  /* make sure that the rfc common has been initialized */
  status = rfc_common_init_status();
  if (!status) 
  {
    rf_common_leave_critical_section(rfc_get_lock_data());
    return status;
  }

  /* cast the rfc vote */
  new_active_techs = rfc_common_vote(path, mode, TRUE, rfc_common_vote_info.active_techs);

  if (rfc_common_vote_info.active_techs != 0)
  {
    /* one of the tech is already active so, there is no need to enable the rf signals, so just
       account the new tech's vote and return */
    rfc_common_vote_info.active_techs = new_active_techs;
    rf_common_leave_critical_section(rfc_get_lock_data());
    return status;
  }

  /* no tech was active before, so, we have to enable all the rf signals. */
  rfc_common_vote_info.active_techs = new_active_techs; /* active techs shoule be ONE here */

  MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH,"Enabling the rf signals from low power mode (%d)", rfc_common_vote_info.active_techs);

  /* configure all the rf signals into active mode of operation */
  for (i = 0; i < rfc_signals_num ; i++)
  {

    msm_signal = rfc_signal_info_table[i].msm_sig_name;
    msm_gpio = rfc_msm_signal_info_table[msm_signal].msm_gpio;

    if ( msm_gpio != RFC_GPIO_INVALID )
    {
      /* Config GPIO Settings */
      if (status == TRUE)
      {
        dal_gpio_settings.eDirection = rfc_msm_signal_info_table[msm_signal].direction;
        dal_gpio_settings.eDriveStrength = rfc_signal_info_table[i].gpio_drive;
        dal_gpio_settings.ePull = rfc_signal_info_table[i].gpio_pull;

        result = DalTlmm_ConfigGpioId( rfc_tlmm_dal_handle, rfc_signal_info_table[i].gpio_id, &dal_gpio_settings );
        if(result != DAL_SUCCESS)
        {
          ERR_FATAL( "rfc_common_wakeup - DalTlmm_ConfigGpioId api failed for GPIO  [num %d, id %d]. "
                     "Check GPIO settings.",
                     rfc_msm_signal_info_table[msm_signal].msm_gpio, rfc_signal_info_table[i].gpio_id, 0);
          status &= FALSE;
        }
      }

      if (result != DAL_SUCCESS)
      {
        MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,"Error while configuring gpio_id(0x%x) to wakeup: error %d", rfc_signal_info_table[i].gpio_id, result);
      }
      else
      {
        /* initialize the GRFC/GPIO state according to the default init_state*/
        rfc_common_init_rfc_signal((int32)i);
      }
    }
  }

  rf_common_leave_critical_section(rfc_get_lock_data());
  return status;
}
#endif

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function puts the rf signals(GPIOs) into low power mode of operation if the signals are in
  active mode and none of the other techs are active.

  @details
  This function puts the rf signals(GPIOs) into low power mode of operation if the signals are in
  active mode and no other techs are active, then all the rf signals will be configured to low power
  mode of operation. If the signals are already in low power mode of operation then nothing will be
  done. It will also de-cast the RFC Vote for the requested path and tech.
 
  @param
  path: RF_PATH which is requesting the signals to be put into active mode.
  mode : technology which is requesting the signals to be put into active mode.
*/
boolean rfc_common_sleep(rf_path_enum_type path, rfm_mode_enum_type mode)
{
  boolean status = TRUE;
  uint16 i = 0;
  DALResult result = DAL_SUCCESS;
  uint32 msm_gpio;
  rfc_msm_signal_type msm_signal;
  int8 new_active_techs = 0;

  /* Protect the rfc sleep/wakeup functions with critical section */
  rf_common_enter_critical_section(rfc_get_lock_data());

  /* make sure that the rfc common has been initialized */
  status = rfc_common_init_status();
  if (!status) 
  {
    rf_common_leave_critical_section(rfc_get_lock_data());
    return status;
  }

  /* de-cast the rfc-vote */
  new_active_techs = rfc_common_vote(path, mode, FALSE, rfc_common_vote_info.active_techs);


  if (rfc_common_vote_info.active_techs == new_active_techs || new_active_techs != 0)
  {
    // if there is no change in state,  nothing todo. OR, new_active_techs is NOT ZERO,
    // means another tech is still active so, don't disable rf signals.
    rfc_common_vote_info.active_techs = new_active_techs;
    rf_common_leave_critical_section(rfc_get_lock_data());
    return status;
  }

  /* if we are here means that none of the other techs are active so, it is safe to put all the rf
     signals into low power mode */

  rfc_common_vote_info.active_techs = new_active_techs; /* the active techs should be ZERO here */

  MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH,"Putting rf signals to low power mode (%d)", rfc_common_vote_info.active_techs);

  #if defined(FEATURE_DIMEPM_MODEM) || defined(FEATURE_JOLOKIA_MODEM) || defined(FEATURE_TABASCO_MODEM)
  #ifndef TEST_FRAMEWORK
  for (i = 0; i < rfc_signals_num ; i++)
  {
    msm_signal = rfc_signal_info_table[i].msm_sig_name;
    msm_gpio = rfc_msm_signal_info_table[msm_signal].msm_gpio;

    if ( msm_gpio != RFC_GPIO_INVALID)
    {
      /* Disable the DAL Signal, which will put the signal into inactive state.  */
      result = DalTlmm_ConfigGpio(rfc_tlmm_dal_handle, rfc_signal_info_table[i].dal_signal, DAL_TLMM_GPIO_DISABLE);
      if (result != DAL_SUCCESS)
      {
        MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,"Error while configuring dal_signal(0x%x) into sleep: error %d", rfc_signal_info_table[i].dal_signal, result);
      }
    }
  }
  #endif
  #else
  /* put all the rf signals into low power mode */
  for (i = 0; i < rfc_signals_num ; i++)
  {
    msm_signal = rfc_signal_info_table[i].msm_sig_name;
    msm_gpio = rfc_msm_signal_info_table[msm_signal].msm_gpio;

    if ( msm_gpio != RFC_GPIO_INVALID)
    {
      /* Disable the DAL Signal, which will put the signal into inactive state.  */
      
      /* Config GPIO to Disabled state */
      if (status == TRUE)
      {
        result = DalTlmm_ConfigGpioIdInactive( rfc_tlmm_dal_handle, rfc_signal_info_table[i].gpio_id );
        if(result != DAL_SUCCESS)
        {
          ERR_FATAL( "rfc_common_sleep - DalTlmm_ConfigGpioIdInactive api failed for GPIO [num %d, id %d].",
                     rfc_msm_signal_info_table[msm_signal].msm_gpio, rfc_signal_info_table[i].gpio_id, 0);
          status &= FALSE;
        }
      }

      if (result != DAL_SUCCESS)
      {
        MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,"Error while configuring gpio_id(0x%x) into sleep: error %d", rfc_signal_info_table[i].gpio_id, result);
      }
    }
  }
  #endif

  rf_common_leave_critical_section(rfc_get_lock_data());
  return status;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Append dest_script to source_script if there is enough space available in
  source_script
  
  @param *dest_script
  Destination script that stores the final appending result
  @param *source_script
  Source script that all the contents are copied from

  @return
  TRUE if append operation is successful. Otherwise return FALSE.
*/
boolean
rfc_common_append_grfc_scripts
(
  rfc_grfc_script_type *dest_script,
  rfc_grfc_script_type *source_script
)
{
  boolean success;
  if( source_script == NULL || dest_script == NULL )
  {
    MSG( MSG_SSID_RF, MSG_LEGACY_ERROR, "rfc_common_append_grfc_scripts: "
         "Null pointer passed in" );
    success = FALSE;
  }
  else if( (dest_script->num_grfc + source_script->num_grfc) > RFC_MAX_NUM_OF_GRFC )
  {
    MSG_1( MSG_SSID_RF, MSG_LEGACY_ERROR, "rfc_common_append_grfc_scripts: "
           "Exceeded max GRFC num: ", dest_script->num_grfc + source_script->num_grfc );
    success = FALSE;
  }
  else
  {
    MSG_1( MSG_SSID_RF, MSG_LEGACY_LOW, "rfc_common_append_grfc_scripts: "
           "GRFC num in script: ", dest_script->num_grfc + source_script->num_grfc );
    memscpy( &(dest_script->grfc_buffer[ dest_script->num_grfc ]),
            source_script->num_grfc * sizeof( rfc_grfc_script_entry_type ),
            &(source_script->grfc_buffer[0]),
            source_script->num_grfc * sizeof( rfc_grfc_script_entry_type ) );

    dest_script->num_grfc += source_script->num_grfc;
    success = TRUE;
  }

  return success;
} /* rfc_common_append_grfc_scripts() */


boolean rfc_read_eeprom()
{
  uint32 rf_eeprom_data_size = 0;
  void* rf_eeprom_info_smem_ptr = NULL; 
  boolean rf_eeprom_data_integrity_success = FALSE;
  
  rf_eeprom_info_smem_ptr = (void *)smem_get_addr( SMEM_RF_EEPROM_DATA, 
                                                    &rf_eeprom_data_size );
  if( rf_eeprom_info_smem_ptr != NULL)
  {
     //We are ignoring rf_eeprom_data_size since we want to copy only upto struct size
     memscpy(&rf_rfc_eeprom_data, sizeof(rf_rfc_eeprom_data_type),
              rf_eeprom_info_smem_ptr, sizeof(rf_rfc_eeprom_data_type));

     if (rf_rfc_eeprom_data.rf_rfc_eeprom_magic_num == RFC_EEPROM_MAGIC_NUM) {
        rf_eeprom_data_integrity_success = TRUE;
     } else {
        MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"EEPROM integrity check failed", 0);
     }

  } else {
     MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rf_eeprom_info_smem_ptr==NULL. EEPROM not present or couldn't read", 0);
  }

  return rf_eeprom_data_integrity_success;
} /* rfc_read_eeprom()*/


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function finds the particular signal in the list of signals and populates corresponding
  signals in the script_buffer

  @details
  This function finds the particular signal in the list of signals and populates corresponding
  signals in the script_buffer
 
  @param
  rf_signals: List of RF signals
  sig_type : the Signal type for which the Signal number need to be returned
  script : buffer to populate the grfc signals
 
  @return
  num_signals: number of signals of the selected type present in the sig list
*/
int32 
rfc_common_get_grfc_buffer
(
  rfc_sig_cfg_type *rf_signals, 
  rfc_signal_type sig_type,
  rf_buffer_intf* script
)
{
  uint16 i = 0;
  int32 rf_sig_name = RFC_SIG_LIST_END;
  int32 grfc_num = -1;
  int32 num_signals = 0;
  rfc_gpio_grfc_type rf_grfc_out_type;

  if (rf_signals == NULL) /* tech specific rf signals to be configured */
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rf_signals==NULL", 0);
    return num_signals;
  }

  if (script == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR," grfc buffer script==NULL", 0);
    return num_signals;
  }

  /* loop through the signal list and configure them based on their type to specified logic  */
  while (rf_signals[i].sig_name != RFC_SIG_LIST_END)
  {
    rf_sig_name = rf_signals[i].sig_name; /* get the RF-CARD Signal name */

    grfc_num = rfc_common_get_signal_num(rf_sig_name, sig_type);

    /*get GRFC signal output type*/
    rf_grfc_out_type = rfc_common_get_grfc_output_type(rf_sig_name);

    if(rf_grfc_out_type != RFC_GRFC)
    {
      MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR," Expecting signal of GRFC output type. "
                    "found output type: %d, signal name: %d ", rf_grfc_out_type, rf_sig_name);
    }
    else
    {
      if (grfc_num >= 0 )
      {
        if (script->append_grfc(grfc_num, rf_signals[i].start.logic, 0, FALSE))
        {
          num_signals++;
        }
        else
        {
          MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"populate script buffer failed:"
                                               " sig_type: %d", sig_type);
        }        
      }
    }
    i++;
  }

  return num_signals;
}
rfc_signal_type rfc_common_get_signal_type(int rfc_signal_name)
{
  boolean status = FALSE;
  rfc_msm_signal_type msm_signal = RFC_MSM_SIG_NUM;

  /* make sure that the rfc common has been initialized */
  status = rfc_common_init_status();
  if (!status) 
  {
    return RFC_SIG_TYPE_INVALID;
  }

  msm_signal = rfc_signal_info_table[rfc_signal_name].msm_sig_name; /* get the MSM Signal name */

  return rfc_msm_signal_info_table[msm_signal].signal_type;
}

/*----------------------------------------------------------------------------*/
/*!
  @details
  This function accepts a table of signals (rf_signals), a signal_type 
  (sig_type), the logic for the signal type (HIGH / LOW), HAL buffer (script)  
  and a time delay in micro-secs.
 
  The functionality of this API is to update the HAL buffer with entries 
  (GRFC_number) from the rf_signals table that match a given signal_type. In 
  additon, the time field for the first signal that matches the sig_type will be 
  updated in the HAL buffer (with time set to "delay" arg)
 
  For Ex:
  if rf_signals[] = {
                     PA_ON           , HIGH,
                     TX_ON           , LOW ,
                     Ant_sel_00      , HIGH,
                     Ant_sel_01      , HIGH,
                     Ant_sel_02      , LOW ,
                     RFC_SIG_LIST_END, LOW
                    }
 
  if the arg "sig_type" is Ant_sel, and arg "delay" is -90us
 
  This API looks for the GRFC numbers associated with Ant_sel_xx - say 
  GRFC_num(ant_sel_xx)

  The HAL buffer will be updated as follows: << GRFC area of the buffer >>
  {
    GRFC_num (Ant_sel_00), -90
    GRFC_num (Ant_sel_00),   0 <<--- note that the "delay" needs to be added
    GRFC_num (Ant_sel_00),   0       only for the first occurence of sig_type..
    GRFC_num (Ant_sel_00),   0       the timings for the rest are relative to 
                                     the first one
  }
 
  @param rf_signals
  List of RF signals
 
  @param sig_type
  The Signal type for which the Signal number need to be returned (Ant_sel, 
  TX_ON, PA_ON, etc)
 
  @param logic
  The logic to be overwritten (HIGH/LOW)
 
  @param script
  HAL Buffer to populate the grfc signals and the timings associated with them
 
  @param delay
  time delay in mirosecs
 
  @return
  num_signals: number of signals of the selected type present in the sig list
*/
int32 
rfc_common_get_grfc_overwritten_buffer
(
  rfc_sig_cfg_type *rf_signals, 
  rfc_signal_type sig_type,
  rfc_logic_type logic,
  rf_buffer_intf* script,
  int16 delay
)
{
  uint16 i = 0;
  int32 rf_sig_name = RFC_SIG_LIST_END;
  int32 grfc_num = -1;
  int32 num_signals = 0;
  rfc_gpio_grfc_type rf_grfc_out_type;

  if (rf_signals == NULL) /* tech specific rf signals to be configured */
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR," rf_signals==NULL", 0);
    return num_signals;
  }

  if (script == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR," grfc buffer script==NULL", 0);
    return num_signals;
  }

  /* loop through the signal list and configure them based on their type to specified logic  */
  while (rf_signals[i].sig_name != RFC_SIG_LIST_END)
  {
    rf_sig_name = rf_signals[i].sig_name; /* get the RF-CARD Signal name */

    grfc_num = rfc_common_get_signal_num(rf_sig_name, sig_type);

    /*get GRFC signal output type*/
    rf_grfc_out_type = rfc_common_get_grfc_output_type(rf_sig_name);

    if(rf_grfc_out_type != RFC_GRFC)
    {
      MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR," Expecting signal of GRFC output type. "
                    "found output type: %d, signal name: %d ", rf_grfc_out_type, rf_sig_name);
    }
    else
    {
      if (grfc_num >= 0 )
      {
        if ( ( num_signals == 0 ) &&                /* The first transaction appends non-zero delay as new time ref*/   
             ( delay != RFDEVICE_NO_TIME_OFFSET ) ) /* AND if delay is not RFDEVICE_NO_TIME_OFFSET */
        {
          if (script->append_grfc(grfc_num, logic, delay, TRUE))
          {
            num_signals++;
          }
          else
          {
            MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"populate script buffer failed:"
                  " sig_type: %d", sig_type);
          }      
        }
        else  /* The rest of transactions append zero delay relative to previous transaction*/
        {
          if (script->append_grfc(grfc_num, logic, 0, FALSE))
          {
            num_signals++;
          }
          else
          {
            MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"populate script buffer failed:"
                                                 " sig_type: %d", sig_type);
          }               
        }
      } /* if (grfc_num >= 0 ) */
                 
    } /* if(rf_grfc_out_type != RFC_GRFC) */
    i++;

  } /* while (rf_signals[i].sig_name != RFC_SIG_LIST_END) */

  return num_signals;
}

void rfc_common_rffe_scan_efs_dump_cmd_put(void)
{
  /*Holds details about rf apps task command*/
  rf_apps_cmd_type* rffe_scan_cmd = NULL;

  /* Add command to RF Apps task to dump device info to EFS */
  /* Get the pointer to the free buffer of RF command queue. */
  if ( ( rffe_scan_cmd = rf_apps_get_buf() ) == NULL )
  {
    RF_MSG( RF_ERROR, "rfc_rffe_bus_scan: Cmd queue is full" );
  }
  else
  {
    rffe_scan_cmd->msgr_hdr.id = RFA_RF_COMMON_MSG_ID_RFFE_SCAN_EFS_DUMP;
    rffe_scan_cmd->rex_hdr.cmd_hdr.tech_id = RF_APPS_COMMON_ID;
    

    /* place RFC_APPS_TASK_RFFE_SCAN_CMD to RF command queue */
    rf_apps_cmd_put( rffe_scan_cmd );
  }
}

void rfc_common_rffe_scan_efs_dump(void* cmd_ptr, rf_dispatch_cid_info_type* cid_info, void* cb_data)
{
  rfc_intf* rfc_ptr = rfc_intf::get_instance();
  
  if ( rfc_ptr == NULL )
  {
    RF_MSG( RF_ERROR, "rfc_apps_task_cmd_dispatcher: NULL rfc ptr!");
    return ;
  }

  rfc_ptr->rfc_rffe_bus_scan();
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function is used to enable/disable  trx rx chain mapped to the requested modem chain(ADC)

  @details
  This function is used to enable/disable  wtr rx chain mapped to the requested modem chain(ADC)
 
  @param rx_chain_enable
  TRUE to enable and FALSE to disable the rx chain
 
  @param rx_modem_chain
  to enable the requested modem chain
 
  @return
  TRUE for success and FALSE for failure
*/
boolean rfc_config_adc_dc_cal(
                                 boolean rx_chain_enable,       /*Enable/disable trx rx chain*/
                                 uint32  rx_modem_chain        /*modem chain to map the ADC*/                        
                               )
{
  uint32 trx_logical_instance = 0xFF;
  uint32 phy_path_num = 0xFF;
  rfdevice_class* trx_obj_p = NULL;
  rfc_intf *rfc_cmn = rfc_intf::get_instance();
  rfdevice_rx_vco_path_enum_type vco_path = WTR_RX_INVALID_VCO_PATH;

  if (rfc_cmn == NULL)
  {
    RF_MSG(RF_ERROR,"rfc_config_adc_dc_cal(): RFC CMN Object is NOT Created");
    return FALSE;
  }

  /*parameter validity check*/
  if (rx_modem_chain >= RXLM_CHAIN_MAX)
  {
    RF_MSG_2(RF_ERROR, "rfc_config_adc_dc_cal(): invalid parameters passed.  rx_modem_chain=%d, rx_chain_enable= %d", 
                        rx_modem_chain,rx_chain_enable);
    return FALSE;  
  }
 
  /*get the phy path and trx logical instance for the requested modem_chain*/
  if(!rfc_cmn->get_trx_phy_path_adc_mapping(
                                            rx_modem_chain, 
                                            &phy_path_num,
                                            &trx_logical_instance,
                                            &vco_path
                                           )
    )
  {

    RF_MSG(RF_ERROR,"rfc_config_adc_dc_cal(): failed to get trx to phy_path,vco, adc mapping ");
    return FALSE;

  }

  /*Get the device object*/
  trx_obj_p = rfc_cmn->get_cmn_rf_device_object(RFDEVICE_TRANSCEIVER, trx_logical_instance);

  if (trx_obj_p == NULL)
  {
    RF_MSG_2(RF_ERROR,"rfc_config_adc_dc_cal(): returned NULL for device type = %d, trx_logical instance = %d",RFDEVICE_TRANSCEIVER,trx_logical_instance);
    return FALSE;
  }

  if (rx_chain_enable)
  {  /*!Turn ON the WTR*/

    if(rfdevice_cmn_power_up ((rfdevice_trx_common_intf_class *)trx_obj_p,
                       RFDEV_VOTE_ON,
                       RFDEV_CMN_NORMAL_VOTING) != RFDEV_ON)
     {
       RF_MSG_2(RF_ERROR,"rfc_config_adc_dc_cal(): Failed to turn ON the WTR "
                         "device type = %d, trx_logical instance = %d",RFDEVICE_TRANSCEIVER,trx_logical_instance);
       return FALSE;
     }
     
     if(!rfdevice_cmn_rx_adc_vcm_cal_setup((rfdevice_trx_common_intf_class*)trx_obj_p,vco_path ))/*Configures  WTR Rx chain for BBRx DC Cal */
     {
       RF_MSG_3(RF_ERROR,"rfc_config_adc_dc_cal(): Failed to setup Rx chain for BBrx DC cal "
                           "device type = %d, trx_logical instance = %d, vco=%d",RFDEVICE_TRANSCEIVER,trx_logical_instance,vco_path);
       return FALSE;
     }

  } 
  else
  {  /*!Turn OFF the WTR*/
     
     if(rfdevice_cmn_power_up ((rfdevice_trx_common_intf_class *)trx_obj_p,
                       RFDEV_VOTE_OFF,
                       RFDEV_CMN_NORMAL_VOTING) != RFDEV_OFF)
     {
       RF_MSG_2(RF_ERROR,"rfc_config_adc_dc_cal(): Failed to turn OFF the WTR "
                         "device type = %d, trx_logical instance = %d",RFDEVICE_TRANSCEIVER,trx_logical_instance);
       return FALSE;
     }
   }


  return TRUE;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function is used to get the list of valid ADCs

  @details
  This function is used to get the list of valid ADCs
 
  @param rx_chain_enable
  TRUE to enable and FALSE to disable the rx chain
 
  @param rx_modem_chain
  to enable the requested modem chain
 
  @return
  TRUE for success and FALSE for failure
*/
boolean rfc_get_valid_adc_list( uint32* adc_list )
{

  rfc_intf *rfc_cmn = rfc_intf::get_instance();

  if (adc_list == NULL)
  {
    RF_MSG(RF_ERROR,"rfc_get_valid_adc_list(): adc_list pointer is NULL. returning FALSE");
    return FALSE;
  }

  if (rfc_cmn == NULL)
  {
    RF_MSG(RF_ERROR,"rfc_get_valid_adc_list(): RFC CMN Object is NOT Created");
    return FALSE;
  }

  /*get the valid adc list mapping*/
  if(!rfc_cmn->get_valid_adc_list(adc_list))
  {

    RF_MSG(RF_ERROR,"rfc_get_valid_adc_list(): failed to get valid adc list mapping ");
    return FALSE;

  }
  return TRUE;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initializes RF device snapshot data structure. 
 
  return
  ptr to corresponding 
*/
/*----------------------------------------------------------------------------*/
#define BUS_INDEX 0
#define DEV_INSTANCE 0

void 
rfc_common_snapshot_init
(
   void
)
{
  rfc_physical_device_struct_type* phys_devices_array = NULL;
  rfcommon_device_snapshot_type* snapshot_data = rfcommon_get_snapshot_data();
  // Get pointer to rfc instance (rfc_p)
  rfc_intf *rfc_ptr = rfc_intf::get_instance();
  int invalid_reg_count=0;
  int phys_devices_count=0;
  if (NULL != rfc_ptr && snapshot_data != NULL )
  {    
    // Initialize all fields to 0
    memset(snapshot_data, 0x0, sizeof(rfcommon_device_snapshot_type));
    // Get physical devices array from rfc
    phys_devices_array = rfc_ptr->get_phys_devices_array();
    phys_devices_count = rfc_ptr->get_phys_devices_count();
    snapshot_data->hw_id = rfc_ptr->get_rfc_hardware_id();
    if ((NULL != phys_devices_array) && (phys_devices_count > 0)) 
    { 
      uint8 device_index = 0;
      for (device_index = 0; device_index < phys_devices_count; device_index++ )
      { 
        rfc_phy_device_info_type* phy_dev_cfg = phys_devices_array[device_index].phy_dev_cfg;
        rfdevice_physical_device* phy_device = phys_devices_array[device_index].device_obj;

        if ( phy_device != NULL && phy_dev_cfg != NULL && 
             phy_dev_cfg->rf_device_comm_protocol == RFDEVICE_COMM_PROTO_RFFE)
        {
          if( snapshot_data->reg_list_phy_rffe_device_count < RFA_SNAPSHOT_MAX_CUSTOM_DEVICE_NUM )
          {
            uint16 reg_list_size = (uint16)(phy_device->rfdevice_phy_dev_get_reg_list_size());   
            uint16 reg_list_device_idx = snapshot_data->reg_list_phy_rffe_device_count;
            /* double check reg_list_size, if invalid size, do not allocate */
            if ( reg_list_size > 0 )
            {
              snapshot_data->reg_list_phy_rffe_device_count++; 
              snapshot_data->reg_list_rffe_dump[reg_list_device_idx] = 
              (rfcommon_rffe_snapshot_addr_list_data_type*)modem_mem_alloc(
                        sizeof(rfcommon_rffe_snapshot_addr_list_data_type), MODEM_MEM_CLIENT_RFA);
              memset(snapshot_data->reg_list_rffe_dump[reg_list_device_idx], 0,sizeof(rfcommon_rffe_snapshot_addr_list_data_type) );

              snapshot_data->reg_list_rffe_dump[reg_list_device_idx]->bus = phy_dev_cfg->bus[BUS_INDEX];              
              snapshot_data->reg_list_rffe_dump[reg_list_device_idx]->slave_id = phy_dev_cfg->assigned_usid;
              snapshot_data->reg_list_rffe_dump[reg_list_device_idx]->data_dump_size = reg_list_size;
              snapshot_data->reg_list_rffe_dump[reg_list_device_idx]->addr_list =  
                     (uint16*)modem_mem_alloc( sizeof(uint16)*reg_list_size, MODEM_MEM_CLIENT_RFA);
              memset(snapshot_data->reg_list_rffe_dump[reg_list_device_idx]->addr_list, 0, sizeof(uint16)*reg_list_size );
              snapshot_data->reg_list_rffe_dump[reg_list_device_idx]->data_dump = 
                     (uint32*)modem_mem_alloc( sizeof(uint32)*reg_list_size, MODEM_MEM_CLIENT_RFA);
              memset(snapshot_data->reg_list_rffe_dump[reg_list_device_idx]->data_dump, 0, sizeof(uint32)*reg_list_size );
              phy_device->rfdevice_phy_dev_get_reg_list(snapshot_data->reg_list_rffe_dump[reg_list_device_idx]->addr_list, reg_list_size);
            }
            else
            {
              uint8 max_reg_list_size=255;
              snapshot_data->reg_list_phy_rffe_device_count++; 
              snapshot_data->reg_list_rffe_dump[reg_list_device_idx] = 
                    (rfcommon_rffe_snapshot_addr_list_data_type*)modem_mem_alloc(
                       sizeof(rfcommon_rffe_snapshot_addr_list_data_type), MODEM_MEM_CLIENT_RFA);
              memset(snapshot_data->reg_list_rffe_dump[reg_list_device_idx], 0, sizeof(rfcommon_rffe_snapshot_addr_list_data_type) );
              snapshot_data->reg_list_rffe_dump[reg_list_device_idx]->bus = phy_dev_cfg->bus[BUS_INDEX];              
              snapshot_data->reg_list_rffe_dump[reg_list_device_idx]->slave_id = phy_dev_cfg->assigned_usid;
              snapshot_data->reg_list_rffe_dump[reg_list_device_idx]->data_dump_size = max_reg_list_size;
              snapshot_data->reg_list_rffe_dump[reg_list_device_idx]->addr_list =  
                     (uint16*)modem_mem_alloc( sizeof(uint16)*max_reg_list_size, MODEM_MEM_CLIENT_RFA);
              memset(snapshot_data->reg_list_rffe_dump[reg_list_device_idx]->addr_list, 0, sizeof(uint16)*max_reg_list_size );
              snapshot_data->reg_list_rffe_dump[reg_list_device_idx]->data_dump = 
                     (uint32*)modem_mem_alloc( sizeof(uint32)*max_reg_list_size, MODEM_MEM_CLIENT_RFA);
              memset(snapshot_data->reg_list_rffe_dump[reg_list_device_idx]->data_dump, 0, sizeof(uint32)*max_reg_list_size );
           }
           phy_device->rfdevice_phy_dev_free_reg_addr_buffer();
        }  
      }/* if ( phy_dev_cfg != NULL && phy_dev_cfg->rf_device_comm_protocol == RFDEVICE_COMM_PROTO_RFFE ) */
    }/* for (device_index = 0; device_idx < phys_devices_count; device_idx++ ) */

    snapshot_data->initialized = TRUE;
  }/* if ((NULL != phys_devices_array) && (phys_devices_count > 0))  */
  else
  {
    RF_MSG(RF_ERROR, "rfc_common_rffe_logging_init: RFC physical devices array not available.");
  }
  }
  else
  {
    RF_MSG(RF_ERROR, "rfc_common_rffe_logging_init: rfc_p was not created." );
    //ERR_FATAL("RFC not initialized!!",0,0,0);
  }
}
/*GRFC bit banging varaibles*/
#define EXT_CLK_COUNT 82
#define EXT_DATA_COUNT 42
#define EXT_LONG_CLK_COUNT 154
#define EXT_LONG_ONE_BYTE_CLK 100
#define EXTL_DATA_COUNT 78

/* Tranceiver Bus ID */
#define TRANSCEIVER_RFFE_BUS_ID 0

/****************************************************************
                                             Extended writes 
****************************************************************/

/* SCLK for extended writes */
rfc_logic_type GRFC_EXT_CLOCK[EXT_CLK_COUNT] = {
RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,
RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,
RFC_LOW,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,
RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,
RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,
RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,
RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,
RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,
RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,
RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,
RFC_HIGH,RFC_LOW
};

/*DATA:0xFF to ADDR: 0x0044*/
rfc_logic_type EXT_DATA_44[EXT_DATA_COUNT] = {
RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW, //initial 0s to make it multiple of 4
RFC_HIGH,RFC_LOW, //SSC
RFC_HIGH,RFC_LOW,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_HIGH, //CF
RFC_LOW,RFC_HIGH,RFC_LOW,RFC_LOW,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_LOW,RFC_HIGH, //AF 0x44 (with parity 1)
RFC_HIGH,RFC_HIGH,RFC_HIGH,RFC_HIGH,RFC_HIGH,RFC_HIGH,RFC_HIGH,RFC_HIGH,RFC_HIGH, //DF 0xFF (with parity 1)
RFC_LOW,RFC_LOW //bpc
};

/*0x0A to 0x004A*/
rfc_logic_type EXT_DATA_4A[EXT_DATA_COUNT] = {
RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW, //initial 0s to make it multiple of 4
RFC_HIGH,RFC_LOW, //SSC
RFC_HIGH,RFC_LOW,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_HIGH, //CF
RFC_LOW,RFC_HIGH,RFC_LOW,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_LOW, //AF 0x4A (with parity 0)
RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_HIGH, //DF 0x0A (with parity 1)
RFC_LOW,RFC_LOW //bpc
};

/*0x01 to 0x0042*/
rfc_logic_type EXT_DATA_42[EXT_DATA_COUNT] = {
RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW, //initial 0s to make it multiple of 4
RFC_HIGH,RFC_LOW, //SSC
RFC_HIGH,RFC_LOW,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_HIGH, //CF
RFC_LOW,RFC_HIGH,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_HIGH, //AF 0x42 (with parity 1)
RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_HIGH,RFC_LOW, //DF 0x01 (with parity 0)
RFC_LOW,RFC_LOW //bpc
};

/*0x06 to 0x0055*/
rfc_logic_type EXT_DATA_55[EXT_DATA_COUNT] = {
RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW, //initial 0s to make it multiple of 4
RFC_HIGH,RFC_LOW, //SSC
RFC_HIGH,RFC_LOW,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_HIGH, //CF
RFC_LOW,RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_HIGH, //AF 0x55 (with parity 1)
RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_HIGH,RFC_HIGH,RFC_LOW,RFC_HIGH, //DF 0x06 (with parity 1)
RFC_LOW,RFC_LOW //bpc
};

/*0x01 to 0x0054*/
rfc_logic_type EXT_DATA_54[EXT_DATA_COUNT] = {
RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW, //initial 0s to make it multiple of 4
RFC_HIGH,RFC_LOW, //SSC
RFC_HIGH,RFC_LOW,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_HIGH, //CF
RFC_LOW,RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_LOW,RFC_LOW, //AF 0x54 (with parity 0)
RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_HIGH,RFC_LOW, //DF 0x01 (with parity 0)
RFC_LOW,RFC_LOW //bpc
};

/****************************************************************
                                             Extended long writes 
****************************************************************/
/* SCLK for extended long writes */
rfc_logic_type EXT_LONG_CLOCK[EXT_LONG_CLK_COUNT] = {
RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,
RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,
RFC_LOW,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,
RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,
RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,
RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,
RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,
RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,
RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,
RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,
RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,
RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,
RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,
RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,
RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,
RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,
RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,
RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,
RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,
RFC_HIGH,RFC_LOW
};

/*0x04A0 to 0xF048*/
rfc_logic_type EXTL_DATA_F048[EXTL_DATA_COUNT] = {
RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,
RFC_HIGH, RFC_LOW, // SSC
RFC_HIGH,RFC_LOW,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_LOW,RFC_HIGH, RFC_HIGH,RFC_LOW,RFC_LOW,RFC_HIGH,RFC_HIGH,RFC_HIGH, //CF
RFC_HIGH,RFC_HIGH, RFC_HIGH,RFC_HIGH,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_HIGH, // AF1:0xF0 (with parity 1)
RFC_LOW,RFC_HIGH,RFC_LOW,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_LOW,RFC_LOW,RFC_HIGH, // AF2:0x48 (with parity 1)
RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_LOW,RFC_LOW, // DF1:0x04 (with parity 0)
RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_HIGH, // DF2:0xA0 (with parity 1)
RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_HIGH,// DF3:0x00 (with parity 1)
RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_HIGH,// DF4:0x00 (with parity 1)
RFC_LOW,RFC_LOW //bpc
};

/*0x0400 to 0xF030*/
rfc_logic_type EXTL_DATA_F030[EXTL_DATA_COUNT] = {
RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,
RFC_HIGH, RFC_LOW, // SSC
RFC_HIGH,RFC_LOW,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_LOW,RFC_HIGH, RFC_HIGH,RFC_LOW,RFC_LOW,RFC_HIGH,RFC_HIGH,RFC_HIGH, //CF
RFC_HIGH,RFC_HIGH, RFC_HIGH,RFC_HIGH,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_HIGH, // AF1:0xF0 (with parity 1)
RFC_LOW,RFC_LOW,RFC_HIGH,RFC_HIGH,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_HIGH, // AF2:0x30 (with parity 1)
RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_LOW,RFC_LOW, // DF1:0x04	(with parity 0)
RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_HIGH, // DF2:0x00 (with parity 1)
RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_HIGH,// DF3:0x00 (with parity 1)
RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_HIGH,// DF4:0x00 (with parity 1)
RFC_LOW,RFC_LOW //bpc
};

/*0x0400 to 0xF008*/
rfc_logic_type EXTL_DATA_F008[EXTL_DATA_COUNT] = {
RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,
RFC_HIGH, RFC_LOW, // SSC
RFC_HIGH,RFC_LOW,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_LOW,RFC_HIGH, RFC_HIGH,RFC_LOW,RFC_LOW,RFC_HIGH,RFC_HIGH,RFC_HIGH, //CF
RFC_HIGH,RFC_HIGH, RFC_HIGH,RFC_HIGH,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_HIGH, // AF1:0xF0 (with parity 1)
RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW, // AF2:0x08 (with parity 1)
RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_LOW,RFC_LOW, // DF1:0x04 (with parity 0)
RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_HIGH, // DF2:0x00 (with parity 1)
RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_HIGH,// DF3:0x00 (with parity 1)
RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_HIGH,// DF4:0x00 (with parity 1)
RFC_LOW,RFC_LOW //bpc
};

/*0x003 to 0xF478*/
rfc_logic_type EXTL_DATA_F478[EXTL_DATA_COUNT] = {
RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,
RFC_HIGH, RFC_LOW, // SSC
RFC_HIGH,RFC_LOW,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_LOW,RFC_HIGH, RFC_HIGH,RFC_LOW,RFC_LOW,RFC_HIGH,RFC_HIGH,RFC_HIGH, //CF
RFC_HIGH,RFC_HIGH, RFC_HIGH,RFC_HIGH,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_LOW,RFC_LOW, // AF1:0xF4 (with parity 0)
RFC_LOW,RFC_HIGH,RFC_HIGH,RFC_HIGH,RFC_HIGH,RFC_LOW,RFC_LOW,RFC_LOW,RFC_HIGH, // AF2:                                                 0x78 (with parity 1)
RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_HIGH,RFC_HIGH,RFC_HIGH, // DF1:0x03 (with parity 1)
RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_HIGH, // DF1:0x00  (with parity 1)
RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_HIGH, // DF3:0x00 (with parity 1)
RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_HIGH, // DF4:0x00 (with parity 1)
RFC_LOW,RFC_LOW //bpc
};

/*0xFF10 to 0x8000*/
rfc_logic_type EXTL_DATA_8000[EXTL_DATA_COUNT] = {
RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,
RFC_HIGH, RFC_LOW, // SSC
RFC_HIGH,RFC_LOW,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_LOW,RFC_HIGH, RFC_HIGH,RFC_LOW,RFC_LOW,RFC_HIGH,RFC_HIGH,RFC_HIGH, //CF
RFC_HIGH,RFC_LOW, RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW, // AF1:0x80 (with parity 0)
RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_HIGH, // AF2:0x00 (with parity 1)
RFC_HIGH,RFC_HIGH,RFC_HIGH,RFC_HIGH,RFC_HIGH,RFC_HIGH,RFC_HIGH,RFC_HIGH,RFC_HIGH, // DF1:0xFF (with parity 1)
RFC_LOW,RFC_LOW,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW, // DF2:0x10 (with parity 0)
RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_HIGH, // DF3:0x00 (with parity 1)
RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_HIGH, // DF4:0x00 (with parity 1)
RFC_LOW,RFC_LOW //bpc
};

/*0x017F to 0x8010*/
rfc_logic_type EXTL_DATA_8010[EXTL_DATA_COUNT] = {
RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,
RFC_HIGH, RFC_LOW, // SSC
RFC_HIGH,RFC_LOW,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_LOW,RFC_HIGH, RFC_HIGH,RFC_LOW,RFC_LOW,RFC_HIGH,RFC_HIGH,RFC_HIGH, //CF
RFC_HIGH,RFC_LOW, RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW, // AF1:0x80 (with parity 0)
RFC_LOW,RFC_LOW,RFC_LOW,RFC_HIGH,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW, // AF2:0x10 (with parity 0)
 RFC_LOW,RFC_HIGH,RFC_HIGH,RFC_HIGH,RFC_HIGH,RFC_HIGH,RFC_HIGH,RFC_HIGH,RFC_LOW, // DF2:0x7F (with parity 0)
RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_HIGH,RFC_LOW, // DF1:0x01 (with parity 0)
RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_HIGH, // DF3:0x00 (with parity 1)
RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_LOW,RFC_HIGH, // DF4:0x00 (with parity 1)
RFC_LOW,RFC_LOW //bpc
};

/*********************************************************
   SDR-105 bootup init sequence 
1. Set the RFFE clk to 9.6MHz
2. Execute boot_up_init sequence at a low clock rate 
   the XOGEN module of SDR and enable 4XO clock  to the AHB bus in RDSS.
3. Switch the speed back to 38.4 MHz
4. Continue programming bootup sequence at high speed

**********************************************************/
void rfcommon_bootup_init()
{
 rf_hal_bus_resource_script_settings_type settings;
 rf_hal_bus_rffe_type script;
 uint8 script_data_2[4] = {0x7F,0x01,0x00,0x00};  
 rf_hal_bus_result_type err;
 rffe_clk_info_type channel_clk_info;

 /*Delay before Chip Init sequence*/
 //DALSYS_BusyWait(3000);

 // Switch RFFE clk speed to 9.6MHz
 channel_clk_info.rffe_channel = TRANSCEIVER_RFFE_BUS_ID;
 channel_clk_info.rffe_clk = RF_HAL_RFFE_9_p_6;
 rflm_ccs_set_dynamic_rffe_clk(&channel_clk_info);

 MCPM_CLK_RFFE_Resrc_Update(0);
 DALSYS_BusyWait(10); 
  /* Register writes for bootup init (1 byte data) */
  rfcommon_write_ext_registers();
#ifndef TEST_FRAMEWORK
 /*DALSysWait 200 usec*/
 //DALSYS_BusyWait(200); 
#endif
  /* Register writes for bootup init (2 byte data) */
  rfcommon_write_ext_long_registers();
  
  DALSYS_BusyWait(10);
  // MCPM call to re program the RCGR input:
  MCPM_CLK_RFFE_Resrc_Update(1);

  DALSYS_BusyWait(20);

  // Switch RFFE clk to 19.2MHz
  channel_clk_info.rffe_channel = TRANSCEIVER_RFFE_BUS_ID;
  channel_clk_info.rffe_clk = RF_HAL_RFFE_38_p_4;
  rflm_ccs_set_dynamic_rffe_clk(&channel_clk_info);

  /* Set the extra settings */
  memset(&settings, 0, sizeof(rf_hal_bus_resource_script_settings_type));
  settings.half_rate = FALSE;
  settings.rffe_force_ext = TRUE;

  /* Set the RFFE transaction settings, addr=0x8024, data=0x2828 */
  script.addr = 0x8024;
  script.channel = TRANSCEIVER_RFFE_BUS_ID;
  script.data[0] = 0x28;
  script.data[1] = 0x28;
  script.data[2] = 0x00;
  script.data[3] = 0x00;
  script.extended_cmd = 1;
  script.half_rate = FALSE;
  script.num_bytes = 4;
  script.rd_delay = 0;
  script.slave_id = 9;
  script.start_delta = 0;
  script.rd_wr = RF_HAL_BUS_WRITE;
  
  err = rfhal_rffe_write_bytes_ext(&script, TRANSCEIVER_RFFE_BUS_ID, 1, &settings);

  /* Set the extra settings */
  memset(&settings, 0, sizeof(rf_hal_bus_resource_script_settings_type));
  settings.half_rate = FALSE;
  settings.rffe_force_ext = TRUE;

  /* Set the RFFE transaction settings, addr=0x8038, data=0x029F */
  script.addr = 0x8038;
  script.channel = TRANSCEIVER_RFFE_BUS_ID;
  script.data[0] = 0x9f;
  script.data[1] = 0x02;
  script.data[2] = 0x00;
  script.data[3] = 0x00;
  script.extended_cmd = 1;
  script.half_rate = FALSE;
  script.num_bytes = 4;
  script.rd_delay = 0;
  script.slave_id = 9;
  script.start_delta = 0;
  script.rd_wr = RF_HAL_BUS_WRITE;
  
  err = rfhal_rffe_write_bytes_ext(&script, TRANSCEIVER_RFFE_BUS_ID, 1, &settings);
  //DALSYS_BusyWait(100);
  
  //rffe_read_data();

  //MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfcommon_bootup_init is done %d", 0);   
  
}

/* API to Invoke the TLMM API to configure the RFFE bus connecting SDR from modem to a GRFC line */
void rfcommon_convert_gpio_to_grfc()
{
   rfc_msm_signal_type msm_signal = RFC_MSM_SIG_NUM;
   boolean result_gpio = FALSE;
   uint32 msm_gpio;
   uint16 i = 0;
  // MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH,"execute config_gpio_to_grfc rfc_signals_num[%d]",rfc_signals_num); 

   /*Disable the GPIOs*/
   for (i = 0; i < rfc_signals_num ; i++)
   {
     msm_signal = rfc_signal_info_table[i].msm_sig_name;
     msm_gpio = rfc_msm_signal_info_table[msm_signal].msm_gpio;

     if( msm_gpio != RFC_GPIO_INVALID)
     {
       /* Disable the DAL Signal, which will put the signal into inactive state.  */
       #ifndef TEST_FRAMEWORK
       result_gpio = DalTlmm_ConfigGpio(rfc_tlmm_dal_handle, rfc_signal_info_table[i].dal_signal, DAL_TLMM_GPIO_DISABLE);
       #endif
	   MSG_2(MSG_SSID_RF, MSG_LEGACY_HIGH,"Configure dal_signal(0x%x) into enable: %d", rfc_signal_info_table[i].dal_signal, 0);
	   if (result_gpio != DAL_SUCCESS)
       {
         MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,"Error while configuring dal_signal(0x%x) into enable: error %d", rfc_signal_info_table[i].dal_signal, result_gpio);
       }
      }
    }

    /*configuring GPIO 43 & 45 to GRFC*/
	 for (i = 0; i < rfc_signals_num ; i++)
	 {
	   msm_signal = rfc_signal_info_table[i].msm_sig_name;		 
	
	   if (rfc_msm_signal_info_table[msm_signal].msm_gpio != RFC_GPIO_INVALID)
	   {
	
		 if((rfc_msm_signal_info_table[msm_signal].signal_type == RFC_RFFE3_CLK)||(rfc_msm_signal_info_table[msm_signal].signal_type == RFC_RFFE3_DATA))
		 {		  
			rfc_msm_signal_info_table[msm_signal].function_select = 1;	 
			rfc_msm_signal_info_table[msm_signal].output_type = RFC_GRFC;	  
	
		   /* construct the DAL Signal */
		   rfc_signal_info_table[i].dal_signal = DAL_GPIO_CFG(rfc_msm_signal_info_table[msm_signal].msm_gpio, \
														  rfc_msm_signal_info_table[msm_signal].function_select, \
														  rfc_msm_signal_info_table[msm_signal].direction, \
														  rfc_signal_info_table[i].gpio_pull, \
														  rfc_signal_info_table[i].gpio_drive); 
          #ifndef TEST_FRAMEWORK
		   /* Disable the DAL Signal, which will put the signal into inactive state.  */
		  result_gpio = DalTlmm_ConfigGpio(rfc_tlmm_dal_handle, rfc_signal_info_table[i].dal_signal, DAL_TLMM_GPIO_DISABLE);
		  #endif
		  
		  MSG_2(MSG_SSID_RF, MSG_LEGACY_HIGH,"Configure dal_signal(0x%x) into enable: %d", rfc_signal_info_table[i].dal_signal, 0);
		  if (result_gpio != DAL_SUCCESS)
		  {
			MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,"Error while configuring dal_signal(0x%x) into enable: error %d", rfc_signal_info_table[i].dal_signal, result_gpio);
		  }
		  else
		  {
			
			/*MSG_3(MSG_SSID_RF, MSG_LEGACY_HIGH,"msm_gpio:%d,func_select:%d,out_put_type:%d",rfc_msm_signal_info_table[msm_signal].msm_gpio,\
			rfc_msm_signal_info_table[msm_signal].function_select,rfc_msm_signal_info_table[msm_signal].output_type);*/
		  }
	   }
	 }	
	}
	
	/*Enable the GPIOs*/
	for (i = 0; i < rfc_signals_num ; i++)
	{
	  msm_signal = rfc_signal_info_table[i].msm_sig_name;
	  msm_gpio = rfc_msm_signal_info_table[msm_signal].msm_gpio;
	
	  if ( msm_gpio != RFC_GPIO_INVALID)
	  {
	  
        #ifndef TEST_FRAMEWORK
	    /* Disable the DAL Signal, which will put the signal into inactive state.  */
        result_gpio = DalTlmm_ConfigGpio(rfc_tlmm_dal_handle, rfc_signal_info_table[i].dal_signal, DAL_TLMM_GPIO_ENABLE);
		#endif
		
		MSG_2(MSG_SSID_RF, MSG_LEGACY_HIGH,"Configure dal_signal(0x%x) into enable: %d", rfc_signal_info_table[i].dal_signal, 1);
        if (result_gpio != DAL_SUCCESS)
		{
		   MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,"Error while configuring dal_signal(0x%x) into enable: error %d", rfc_signal_info_table[i].dal_signal, result_gpio);
		}
	   }
	} 
}

/*********************************************************
API to for extended register writes(1 data byte)

**********************************************************/
void rfcommon_write_ext_registers()
{

 rf_hal_bus_resource_script_settings_type settings;
 rf_hal_bus_rffe_type script;
 rf_hal_bus_result_type err;

/********************************************************
 1. writing 0xFF to 0x0044
********************************************************/
  /* Set the extra settings */
  memset(&settings, 0, sizeof(rf_hal_bus_resource_script_settings_type));
  settings.half_rate = FALSE;
  settings.rffe_force_ext = TRUE;
  
  /* Set the RFFE transaction settings */
  script.addr = 0x44;
  script.channel = TRANSCEIVER_RFFE_BUS_ID;
  script.data[0] = 0xff;
  script.data[1] = 0x00;
  script.data[2] = 0x00;
  script.data[3] = 0x00;
  script.extended_cmd = 1;
  script.half_rate = FALSE;
  script.num_bytes = 1;
  script.rd_delay = 0;
  script.slave_id = 9;
  script.start_delta = 0;
  script.rd_wr = RF_HAL_BUS_WRITE;
  
  err = rfhal_rffe_write_bytes_ext(&script, TRANSCEIVER_RFFE_BUS_ID, 1, &settings);



/********************************************************
   2. writing 0x0A to 0x004A
********************************************************/

/* Set the extra settings */
memset(&settings, 0, sizeof(rf_hal_bus_resource_script_settings_type));
settings.half_rate = FALSE;
settings.rffe_force_ext = TRUE;

/* Set the RFFE transaction settings */
script.addr = 0x4A;
script.channel = TRANSCEIVER_RFFE_BUS_ID;
script.data[0] = 0x0A;
script.data[1] = 0x00;
script.data[2] = 0x00;
script.data[3] = 0x00;
script.extended_cmd = 1;
script.half_rate = FALSE;
script.num_bytes = 1;
script.rd_delay = 0;
script.slave_id = 9;
script.start_delta = 0;
script.rd_wr = RF_HAL_BUS_WRITE;

err = rfhal_rffe_write_bytes_ext(&script, TRANSCEIVER_RFFE_BUS_ID, 1, &settings);

DALSYS_BusyWait(200);


/********************************************************
   3. writing 0x06 to 0x0055
********************************************************/

/* Set the extra settings */
memset(&settings, 0, sizeof(rf_hal_bus_resource_script_settings_type));
settings.half_rate = FALSE;
settings.rffe_force_ext = TRUE;

/* Set the RFFE transaction settings */
script.addr = 0x55;
script.channel = TRANSCEIVER_RFFE_BUS_ID;
script.data[0] = 0x06;
script.data[1] = 0x00;
script.data[2] = 0x00;
script.data[3] = 0x00;
script.extended_cmd = 1;
script.half_rate = FALSE;
script.num_bytes = 1;
script.rd_delay = 0;
script.slave_id = 9;
script.start_delta = 0;
script.rd_wr = RF_HAL_BUS_WRITE;

err = rfhal_rffe_write_bytes_ext(&script, TRANSCEIVER_RFFE_BUS_ID, 1, &settings);

}

/*********************************************************
API extended register long writes
register ddress is byte aligned and total 4 data bytes in which 
last two(LSB) bytes of data is zeros

**********************************************************/
void rfcommon_write_ext_long_registers()
{
 rf_hal_bus_resource_script_settings_type settings;
 rf_hal_bus_rffe_type script;
 rf_hal_bus_result_type err;

/********************************************************
   1. writing 0x0003 to 0xF478
********************************************************/

 /* Set the extra settings */
 memset(&settings, 0, sizeof(rf_hal_bus_resource_script_settings_type));
 settings.half_rate = FALSE;
 settings.rffe_force_ext = TRUE;
 
 /* Set the RFFE transaction settings */
 script.addr = 0xf478;
 script.channel = TRANSCEIVER_RFFE_BUS_ID;
 script.data[0] = 0x03;
 script.data[1] = 0x00;
 script.data[2] = 0x00;
 script.data[3] = 0x00;
 script.extended_cmd = 1;
 script.half_rate = FALSE;
 script.num_bytes = 4;
 script.rd_delay = 0;
 script.slave_id = 9;
 script.start_delta = 0;
 script.rd_wr = RF_HAL_BUS_WRITE;
 
 err = rfhal_rffe_write_bytes_ext(&script, TRANSCEIVER_RFFE_BUS_ID, 1, &settings);


}

/*********************************************************
 API to Invoke the TLMM API to configure the GRFC line back to RFFE functionality

**********************************************************/
void rfcommon_convert_gpio_to_rffe()
{
   rfc_msm_signal_type msm_signal = RFC_MSM_SIG_NUM;
   uint32 msm_gpio;
   uint16 i = 0;
   boolean result_gpio = FALSE;
 
   int32 rf_sig_name = 0;
   // MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH,"execute config_gpio_rffe:rfc_signals_num[%d]",rfc_signals_num);

   for (i = 0; i < rfc_signals_num ; i++)
   {
     msm_signal = rfc_signal_info_table[i].msm_sig_name;
     msm_gpio = rfc_msm_signal_info_table[msm_signal].msm_gpio;

     if ( msm_gpio != RFC_GPIO_INVALID)
     {
       /* Disable the DAL Signal, which will put the signal into inactive state.  */
       #ifndef TEST_FRAMEWORK
       result_gpio = DalTlmm_ConfigGpio(rfc_tlmm_dal_handle, rfc_signal_info_table[i].dal_signal, DAL_TLMM_GPIO_DISABLE);
       #endif
	   MSG_2(MSG_SSID_RF, MSG_LEGACY_HIGH,"Configure dal_signal(0x%x) into enable: %d", rfc_signal_info_table[i].dal_signal, 0);
       if (result_gpio != DAL_SUCCESS)
       {
         MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,"Error while configuring dal_signal(0x%x) into enable: error %d", rfc_signal_info_table[i].dal_signal, result_gpio);
       }
     }
   }

   for (i = 0; i < rfc_signals_num ; i++)
   {
	 msm_signal = rfc_signal_info_table[i].msm_sig_name;	    

     if (rfc_msm_signal_info_table[msm_signal].msm_gpio != RFC_GPIO_INVALID)
     {

	   if((rfc_msm_signal_info_table[msm_signal].signal_type == RFC_RFFE3_CLK)||(rfc_msm_signal_info_table[msm_signal].signal_type == RFC_RFFE3_DATA))
	   {		 
	     rfc_msm_signal_info_table[msm_signal].function_select = 2;	
		rfc_msm_signal_info_table[msm_signal].output_type = RFC_GPIO;	 

		/* construct the DAL Signal */
        rfc_signal_info_table[i].dal_signal = DAL_GPIO_CFG(rfc_msm_signal_info_table[msm_signal].msm_gpio, \
                                                         rfc_msm_signal_info_table[msm_signal].function_select, \
                                                         rfc_msm_signal_info_table[msm_signal].direction, \
                                                         rfc_signal_info_table[i].gpio_pull, \
                                                         rfc_signal_info_table[i].gpio_drive);		  
        #ifndef TEST_FRAMEWORK  
		/* Disable the DAL Signal, which will put the signal into inactive state.  */
	     result_gpio = DalTlmm_ConfigGpio(rfc_tlmm_dal_handle, rfc_signal_info_table[i].dal_signal, DAL_TLMM_GPIO_DISABLE);
        #endif
		MSG_2(MSG_SSID_RF, MSG_LEGACY_HIGH,"Configure dal_signal(0x%x) into enable: %d", rfc_signal_info_table[i].dal_signal, 0);

		 if (result_gpio == DAL_SUCCESS)
	     {
	      /*  MSG_3(MSG_SSID_RF, MSG_LEGACY_HIGH,"msm_gpio:%d,func_select:%d,out_put_type:%d",rfc_msm_signal_info_table[msm_signal].msm_gpio,\
		    rfc_msm_signal_info_table[msm_signal].function_select,rfc_msm_signal_info_table[msm_signal].output_type);*/
	     }
		 else
		 {
            MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,"Error while configuring dal_signal(0x%x) into enable: error %d", rfc_signal_info_table[i].dal_signal, result_gpio);
		 }
	    }
      }
   }	

   for (i = 0; i < rfc_signals_num ; i++)
   {
     msm_signal = rfc_signal_info_table[i].msm_sig_name;
     msm_gpio = rfc_msm_signal_info_table[msm_signal].msm_gpio;

     if ( msm_gpio != RFC_GPIO_INVALID)
     {
       #ifndef TEST_FRAMEWORK
       /* Disable the DAL Signal, which will put the signal into inactive state.  */
       result_gpio = DalTlmm_ConfigGpio(rfc_tlmm_dal_handle, rfc_signal_info_table[i].dal_signal, DAL_TLMM_GPIO_ENABLE);
       #endif
	   
	   MSG_2(MSG_SSID_RF, MSG_LEGACY_HIGH,"Configure dal_signal(0x%x) into enable: %d", rfc_signal_info_table[i].dal_signal, 1);
       if (result_gpio != DAL_SUCCESS)
       {
         MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,"Error while configuring dal_signal(0x%x) into enable: error %d", rfc_signal_info_table[i].dal_signal, result_gpio);
       }
	  else
	  {
	     /*MSG_3(MSG_SSID_RF, MSG_LEGACY_HIGH,"msm_gpio:%d,func_select:%d,out_put_type:%d",rfc_msm_signal_info_table[msm_signal].msm_gpio,\
		                             rfc_msm_signal_info_table[msm_signal].function_select,rfc_msm_signal_info_table[msm_signal].output_type);*/
	  }
     }
   } 
  MSG_3(MSG_SSID_RF, MSG_LEGACY_HIGH,"config_gpio_to_rffe result:%d",result_gpio,0,0);
		                             
}

void rffe_read_data()
{
 rf_hal_bus_result_type err;
 rf_hal_bus_resource_script_settings_type settings;


  /* Set the extra settings */
  memset(&settings, 0, sizeof(rf_hal_bus_resource_script_settings_type));
  settings.half_rate = 1;
  settings.rffe_force_ext = 1;

  /* Set the RFFE transaction settings */
  script_bitbang[0].addr = 0x004A;
  script_bitbang[0].channel = 2;
  script_bitbang[0].data[0] = 0;
  script_bitbang[0].extended_cmd = 1;  
  script_bitbang[0].num_bytes = 1;
  script_bitbang[0].rd_delay = 0;
  script_bitbang[0].slave_id = 9;
  script_bitbang[0].start_delta = 0;
 
  /* Issue the command to the HAL. Always use the 2 word ("ext") CCS format since we're doing single transactions */
   script_bitbang[0].rd_wr = (rf_hal_bus_rd_wr_type)1;
   err = rfhal_rffe_read_bytes_ext(&script_bitbang[0], script_bitbang[0].channel, 1, &settings);
   err = rfhal_rffe_read_bytes_ext(&script_bitbang[0], script_bitbang[0].channel, 1, &settings);
    
  /* Record the read value if applicable */
  if (err == RF_HAL_BUS_SUCCESS) 
  {
    //debug_data_00 = script_bitbang.data[0];
//	 MSG_HIGH("rffe_read_data channel[%d],data[0x%x]",script_bitbang[0].addr,script_bitbang[0].data[0],0);
  }
  else if (err != RF_HAL_BUS_SUCCESS)
  {
	 MSG_HIGH("rffe_read_data:Failed!!",0,0,0);
  }

  #if 0
  /* Set the RFFE transaction settings */
  script_bitbang[1].addr = 0x0044;
  script_bitbang[1].channel = 2;
  script_bitbang[1].data[0] = 0;
  script_bitbang[1].extended_cmd = 1;  
  script_bitbang[1].num_bytes = 1;
  script_bitbang[1].rd_delay = 0;
  script_bitbang[1].slave_id = 9;
  script_bitbang[1].start_delta = 0;
 
  /* Issue the command to the HAL. Always use the 2 word ("ext") CCS format since we're doing single transactions */
   script_bitbang[1].rd_wr = (rf_hal_bus_rd_wr_type)1;
   err = rfhal_rffe_read_bytes_ext(&script_bitbang[1], script_bitbang[1].channel, 1, &settings);
   err = rfhal_rffe_read_bytes_ext(&script_bitbang[1], script_bitbang[1].channel, 1, &settings);
    
  /* Record the read value if applicable */
  if (err == RF_HAL_BUS_SUCCESS) 
  {
    //debug_data_00 = script_bitbang.data[0];
	 MSG_HIGH("rffe_read_data channel[%d],data[0x%x]",script_bitbang[1].addr,script_bitbang[1].data[0],0);
  }
  else if (err != RF_HAL_BUS_SUCCESS)
  {
	 MSG_HIGH("rffe_read_data:Failed!!",0,0,0);
  }




/* Set the RFFE transaction settings */
  script_bitbang[2].addr = 0x0042;
script_bitbang[2].channel = 2;
script_bitbang[2].data[0] = 0;
script_bitbang[2].extended_cmd = 1;  
  script_bitbang[2].num_bytes = 1;
script_bitbang[2].rd_delay = 0;
script_bitbang[2].slave_id = 9;
script_bitbang[2].start_delta = 0;

/* Issue the command to the HAL. Always use the 2 word ("ext") CCS format since we're doing single transactions */
 script_bitbang[2].rd_wr = (rf_hal_bus_rd_wr_type)1;
 err = rfhal_rffe_read_bytes_ext(&script_bitbang[2], script_bitbang[2].channel, 1, &settings);
 err = rfhal_rffe_read_bytes_ext(&script_bitbang[2], script_bitbang[2].channel, 1, &settings);
  
/* Record the read value if applicable */
if (err == RF_HAL_BUS_SUCCESS) 
{
  //debug_data_00 = script_bitbang.data[0];
	 MSG_HIGH("rffe_read_data channel[%d],data[0x%x]",script_bitbang[2].addr,script_bitbang[2].data[0],0);
  }
  else if (err != RF_HAL_BUS_SUCCESS)
  {
	 MSG_HIGH("rffe_read_data:Failed!!",0,0,0);
  }




    /* Set the RFFE transaction settings */
  script_bitbang[3].addr = 0x0054;
  script_bitbang[3].channel = 2;
  script_bitbang[3].data[0] = 0;
  script_bitbang[3].extended_cmd = 1;  
  script_bitbang[3].num_bytes = 1;
  script_bitbang[3].rd_delay = 0;
  script_bitbang[3].slave_id = 9;
  script_bitbang[3].start_delta = 0;
 
  /* Issue the command to the HAL. Always use the 2 word ("ext") CCS format since we're doing single transactions */
   script_bitbang[3].rd_wr = (rf_hal_bus_rd_wr_type)1;
   err = rfhal_rffe_read_bytes_ext(&script_bitbang[3], script_bitbang[3].channel, 1, &settings);
   err = rfhal_rffe_read_bytes_ext(&script_bitbang[3], script_bitbang[3].channel, 1, &settings);
    
  /* Record the read value if applicable */
  if (err == RF_HAL_BUS_SUCCESS) 
  {
    //debug_data_00 = script_bitbang.data[0];
	 MSG_HIGH("rffe_read_data channel[%d],data[0x%x]",script_bitbang[3].addr,script_bitbang[3].data[0],0);
  }
  else if (err != RF_HAL_BUS_SUCCESS)
  {
	 MSG_HIGH("rffe_read_data:Failed!!",0,0,0);
  }


    /* Set the RFFE transaction settings */
  script_bitbang[4].addr = 0x0055;
  script_bitbang[4].channel = 2;
  script_bitbang[4].data[0] = 0;
  script_bitbang[4].extended_cmd = 1;  
  script_bitbang[4].num_bytes = 1;
  script_bitbang[4].rd_delay = 0;
  script_bitbang[4].slave_id = 9;
  script_bitbang[4].start_delta = 0;
 
  /* Issue the command to the HAL. Always use the 2 word ("ext") CCS format since we're doing single transactions */
   script_bitbang[4].rd_wr = (rf_hal_bus_rd_wr_type)1;
   err = rfhal_rffe_read_bytes_ext(&script_bitbang[4], script_bitbang[4].channel, 1, &settings);
   err = rfhal_rffe_read_bytes_ext(&script_bitbang[4], script_bitbang[4].channel, 1, &settings);
    
  /* Record the read value if applicable */
  if (err == RF_HAL_BUS_SUCCESS) 
  {
    //debug_data_00 = script_bitbang.data[0];
	 MSG_HIGH("rffe_read_data channel[%d],data[0x%x]",script_bitbang[4].addr,script_bitbang[4].data[0],0);
  }
  else if (err != RF_HAL_BUS_SUCCESS)
  {
	 MSG_HIGH("rffe_read_data:Failed!!",0,0,0);
  }


  


  /* Set the RFFE transaction settings */
  script_bitbang[5].addr = 0x8010;
  script_bitbang[5].channel = 2;
  script_bitbang[5].data[0] = 0;
  script_bitbang[5].data[1] = 0;
  script_bitbang[5].extended_cmd = 1;  
  script_bitbang[5].num_bytes = 4;
  script_bitbang[5].rd_delay = 0;
  script_bitbang[5].slave_id = 9;
  script_bitbang[5].start_delta = 0;
 
  /* Issue the command to the HAL. Always use the 2 word ("ext") CCS format since we're doing single transactions */
   script_bitbang[5].rd_wr = (rf_hal_bus_rd_wr_type)1;
   err = rfhal_rffe_read_bytes_ext(&script_bitbang[5], script_bitbang[5].channel, 1, &settings);
   err = rfhal_rffe_read_bytes_ext(&script_bitbang[5], script_bitbang[5].channel, 1, &settings);
   err = rfhal_rffe_read_bytes_ext(&script_bitbang[5], script_bitbang[5].channel, 1, &settings);
   err = rfhal_rffe_read_bytes_ext(&script_bitbang[5], script_bitbang[5].channel, 1, &settings);
   err = rfhal_rffe_read_bytes_ext(&script_bitbang[5], script_bitbang[5].channel, 1, &settings);
   err = rfhal_rffe_read_bytes_ext(&script_bitbang[5], script_bitbang[5].channel, 1, &settings);    
  /* Record the read value if applicable */
  if (err == RF_HAL_BUS_SUCCESS) 
  {
    //debug_data_00 = script_bitbang.data[0];
	 MSG_HIGH("rffe_read_data addr[%d],data[0x%x],data[0x%x]",script_bitbang[5].addr,script_bitbang[5].data[0],script_bitbang[5].data[1]);
	 MSG_HIGH("rffe_read_data data[0x%x],data[0x%x]",script_bitbang[5].data[2],script_bitbang[5].data[3],0);
  }
  else if (err != RF_HAL_BUS_SUCCESS)
  {
	 MSG_HIGH("rffe_read_data:Failed!!",0,0,0);
  }



/* Set the RFFE transaction settings */
script_bitbang[6].addr = 0xF478;
script_bitbang[6].channel = 2;
script_bitbang[6].data[0] = 0;
script_bitbang[6].data[1] = 0;
script_bitbang[6].extended_cmd = 1;  
script_bitbang[6].num_bytes = 4;
script_bitbang[6].rd_delay = 0;
script_bitbang[6].slave_id = 9;
script_bitbang[6].start_delta = 0;

/* Issue the command to the HAL. Always use the 2 word ("ext") CCS format since we're doing single transactions */
 script_bitbang[6].rd_wr = (rf_hal_bus_rd_wr_type)1;
 err = rfhal_rffe_read_bytes_ext(&script_bitbang[6], script_bitbang[6].channel, 1, &settings);
 err = rfhal_rffe_read_bytes_ext(&script_bitbang[6], script_bitbang[6].channel, 1, &settings);
 err = rfhal_rffe_read_bytes_ext(&script_bitbang[6], script_bitbang[6].channel, 1, &settings);
 err = rfhal_rffe_read_bytes_ext(&script_bitbang[6], script_bitbang[6].channel, 1, &settings);
 err = rfhal_rffe_read_bytes_ext(&script_bitbang[6], script_bitbang[6].channel, 1, &settings);
 err = rfhal_rffe_read_bytes_ext(&script_bitbang[6], script_bitbang[6].channel, 1, &settings);
  
/* Record the read value if applicable */
if (err == RF_HAL_BUS_SUCCESS) 
{
  //debug_data_00 = script_bitbang.data[0];
MSG_HIGH("rffe_read_data addr[%d],data[0x%x],data[0x%x]",script_bitbang[6].addr,script_bitbang[6].data[0],script_bitbang[6].data[1]);
MSG_HIGH("rffe_read_data data[0x%x],data[0x%x]",script_bitbang[6].data[2],script_bitbang[6].data[3],0);
}
else if (err != RF_HAL_BUS_SUCCESS)
{
MSG_HIGH("rffe_read_data:Failed!!",0,0,0);
}


  /* Set the RFFE transaction settings */
  script_bitbang[7].addr = 0x8000;
  script_bitbang[7].channel = 2;
  script_bitbang[7].data[0] = 0;
  script_bitbang[7].data[1] = 0;
  script_bitbang[7].extended_cmd = 1;  
  script_bitbang[7].num_bytes = 4;
  script_bitbang[7].rd_delay = 0;
  script_bitbang[7].slave_id = 9;
  script_bitbang[7].start_delta = 0;
  
  /* Issue the command to the HAL. Always use the 2 word ("ext") CCS format since we're doing single transactions */
   script_bitbang[7].rd_wr = (rf_hal_bus_rd_wr_type)1;
   err = rfhal_rffe_read_bytes_ext(&script_bitbang[7], script_bitbang[7].channel, 1, &settings);
   err = rfhal_rffe_read_bytes_ext(&script_bitbang[7], script_bitbang[7].channel, 1, &settings);
   err = rfhal_rffe_read_bytes_ext(&script_bitbang[7], script_bitbang[7].channel, 1, &settings);
   err = rfhal_rffe_read_bytes_ext(&script_bitbang[7], script_bitbang[7].channel, 1, &settings);
   err = rfhal_rffe_read_bytes_ext(&script_bitbang[7], script_bitbang[7].channel, 1, &settings);
   err = rfhal_rffe_read_bytes_ext(&script_bitbang[7], script_bitbang[7].channel, 1, &settings);    
  /* Record the read value if applicable */
  if (err == RF_HAL_BUS_SUCCESS) 
  {
    //debug_data_00 = script_bitbang.data[0];
  MSG_HIGH("rffe_read_data addr[%d],data[0x%x],data[0x%x]",script_bitbang[7].addr,script_bitbang[7].data[0],script_bitbang[7].data[1]);
  MSG_HIGH("rffe_read_data data[0x%x],data[0x%x]",script_bitbang[7].data[2],script_bitbang[7].data[3],0);
  }
  else if (err != RF_HAL_BUS_SUCCESS)
  {
  MSG_HIGH("rffe_read_data:Failed!!",0,0,0);
  }



  memset(&settings, 0, sizeof(rf_hal_bus_resource_script_settings_type));
  settings.half_rate = 1;
  settings.rffe_force_ext = 1;

  /* Set the RFFE transaction settings */
  script.addr = 0x0003;
  script.channel = 2;
  script.data[0] = 0;
  script.extended_cmd = 1;  
  script.num_bytes = 1;
  script.rd_delay = 0;
  script.slave_id = 1;
  script.start_delta = 0;
 
  /* Issue the command to the HAL. Always use the 2 word ("ext") CCS format since we're doing single transactions */
   script.rd_wr = (rf_hal_bus_rd_wr_type)1;
   err = rfhal_rffe_read_bytes_ext(&script, script.channel, 1, &settings);
    
  /* Record the read value if applicable */
  if (err == RF_HAL_BUS_SUCCESS) 
  {
   debug_data_01 = script.data[0];
	 MSG_HIGH("rffe_read_data channel[%d],data[0x%x]",script.channel,script.data[0],0);
  }
  else if (err != RF_HAL_BUS_SUCCESS)
  {
	 MSG_HIGH("rffe_read_data:Failed!!",0,0,0);
  }


  /* Set the RFFE transaction settings */
  script.addr = 0x002F;
  script.channel = 2;
  script.data[0] = 0;
  script.extended_cmd = 1;  
  script.num_bytes = 1;
  script.rd_delay = 0;
  script.slave_id = 1;
  script.start_delta = 0;
 
  /* Issue the command to the HAL. Always use the 2 word ("ext") CCS format since we're doing single transactions */
   script.rd_wr = (rf_hal_bus_rd_wr_type)1;
   err = rfhal_rffe_read_bytes_ext(&script, script.channel, 1, &settings);
    
  /* Record the read value if applicable */
  if (err == RF_HAL_BUS_SUCCESS) 
  {
    debug_data_02 = script.data[0];
	 MSG_HIGH("rffe_read_data channel[%d],data[0x%x]",script.channel,script.data[0],0);
  }
  else if (err != RF_HAL_BUS_SUCCESS)
  {
	 MSG_HIGH("rffe_read_data:Failed!!",0,0,0);
  }

  /* Set the RFFE transaction settings */
  script.addr = 0x0030;
  script.channel = 2;
  script.data[0] = 0;
  script.extended_cmd = 1;  
  script.num_bytes = 1;
  script.rd_delay = 0;
  script.slave_id = 1;
  script.start_delta = 0;
 
  /* Issue the command to the HAL. Always use the 2 word ("ext") CCS format since we're doing single transactions */
   script.rd_wr = (rf_hal_bus_rd_wr_type)1;
   err = rfhal_rffe_read_bytes_ext(&script, script.channel, 1, &settings);
    
  /* Record the read value if applicable */
  if (err == RF_HAL_BUS_SUCCESS) 
  {
   debug_data_03 = script.data[0];
	 MSG_HIGH("rffe_read_data channel[%d],data[0x%x]",script.channel,script.data[0],0);
  }
  else if (err != RF_HAL_BUS_SUCCESS)
  {
	 MSG_HIGH("rffe_read_data:Failed!!",0,0,0);
  }
  #endif
}  
