/*!
   @file rfgsm_core_tx.c

   @brief

   @details

*/

/*! @todo satheesha 10/30/08 do we need to keep the edit history??? */

/*===========================================================================
Copyright (c) 2008 - 2023 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/rftech_gsm.mpss/2.10/rf/core/src/rfgsm_core_tx.c#6 $
$DateTime: 2023/07/21 00:57:44 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/21/23   vta     FSK enhancement changes.
07/20/23   vta	   FSK shift issue: phase path disable through SDR api
03/15/21   aad     To FSK Frequency truncation issue
09/09/20   mpa     To disable first therm read to avoid uninitialized wxe trigger
09/02/20   bpa     Fixing CW and KW
05/18/20   mpa     To support gp_sync feature
05/31/19   Krk     Delay cal for various PLL options
04/05/19   mpa     F3 reduction on Twizy
01/28/19   mpa     Changes to support PATH_DELAY
12/18/18   mpa     Fix CW error
08/02/18   hkm     Double buffering and Token changes
05/21/18   hkm    RGI API update 
05/18/18   hkm    Supporting GSM SDR105
04/23/15   par     Fix incorrect AFC being applied in GBTA cases
02/23/15   sml     Fixing OFT complier errors
02/23/15   sml     Passing band info while generating GSM Tx/Rx 
                   SBI scripts to support band split across 2 WTRs
12/15/14   sml     Enable DAC VREF only during Tx enable
12/02/14   sml     Read WTR or RF front end devices status in between Rx/Tx 
                   burst along with GRFC status
11/20/14   hoh     Apply temp comp to originally calculated frequency compensated AMAM/AMPM table
11/19/14   sc      Read and calculate delay match DM1-3 in RFLM GSM TxFE config
11/18/14   hoh     Set amam_ampm_recalc_required flag before ping-pong buffer switch
10/17/14   rr      Separate rx/tx alternate path core handles
10/07/14   sc      Program all delay match settings in RFLM DRIF
09/23/14   zbz     changes to support GSM and LTE/TDS coexistence related RFC changes
09/22/14   sc      Update band specific Tx timing adjust data when Tx band change
08/28/14   sc      Clean up Tx data initialisation to reduce wakeup timeline
08/11/14   shb     Fixed offtarget compiler error
08/11/14   cj      Added debug prints for Tx
08/11/14   sc      Ensure chain update with alternate path
07/24/14   tsr     DR-DSDS: Add support for device driver to handle worst case rx alpha for IDLE mode RX
07/01/14   jr      Enable therm read on sub-2
06/24/14   av      Compiler warning fix
06/18/14   tsr     RXD diversity call flow changes to enable DRX path along with PRX path
06/02/14    av     Fix memory corruption/writing to out of bounds.
05/20/14   zbz     Fix OFT error
06/05/14   ggs     AMAM/AMPM tables should only be recalculated 
                   for GSM when modulation type is 8PSK
04/29/14   sc      Scale the Tx PA ramps based on subscription
04/29/14   tsr     Mainline GSM RFC config v2 apis 
04/25/14   zbz     Up sample pa ramp table from NV using spline interpolation algorithm
04/16/14   sb      IP2 cal Interface and functionality changes
03/06/14   tsr     Program device scripts dynamically through CCS
03/06/14   sc      Correct the PA ramp up table for accurate PA ramp adjust
03/06/14   ggs     Ensure that rfgsm_core_init_master_pa_ramp_tables does not return
                   FALSE if the ramp tables are initialised for one of the supported 
                   bands.
03/04/14   tsr     Separate RX burst scripts and TX burst scripts retrieval from device driver
03/04/14   tsr     Move TX specific initialisation into Tx enter mode
02/28/14   sc      Pass core data through mc to core layers
02/25/14   tsr     Deprecate Feature D3925
02/18/14   tsr     GSM RX Diversity 
02/17/14   sc      Clean up mDSP APIs
01/30/14   sc      Manage data table independent of device
01/21/14   tsr     WTR TXAGC support for Multislot TX
01/21/14   ggs     Initial changes for TXC DCOC and DPD calibration
01/15/14   sc      Remove unused APIs using FW shared memory
01/14/14   av      Delay Calibration and npler based delay support
12/10/13   sc      Add set envelope gain API functionality
11/01/13   tsr     Append APT script to TASKQA(Non-atomic event) TX start script
10/31/13   sc      Feature-guard FW shared mem buffer population
10/28/13   ggs     Downsample base AMAM temp comp LUT by a factor of 4 
10/28/13   ggs     Add function to switch amam/ampm buffer index
10/28/13   ggs     Copy AMAM and AMPM tables to DM memory
10/25/13   tsr     Fix for appending APT Script to TX Non-sbi start script
10/23/13   tsr     Mainline D3295 changes for BOLT
10/21/13   sc      Modify dynamic event building for scenario-specific wrappers
10/21/13   sc      Set the PA ramp tables to RFLM GSM DM
10/14/13   tsr     Temporarily append new APT script to TX start script
10/14/13   sc      Mainline core DM and RFLM code
10/10/13   sc      Use RFLM band information to initialise PA ramp tables
10/09/13   tsr     Dynamic RF CCS event changes for WTR3925
10/08/13   sc      Correct RFLM table population PA ramp band loop
10/01/13   tsr     Added support retrieve to individual device scripts
10/03/13   sb      Add changes to receive MSGR cmd from MCS to switch to filtered/unfiltered 
                   path for SGLTE coexistence (GSM DCS +LTE B39/TDS B39/B34)
10/03/13   sc      RFLM DM interface data and dynamic event development
10/03/13   sg      Enabling GSM channel 1 phase data for QRD SGLTE card
10/01/13   sb      Disable SGLTE Coexistence filtered path cal data
09/27/13   sc      Modify RFLM GSM interface data programming functionality
09/25/13   sb      Added changes to support GSM TX calibration/online mode operation in coexistence
09/17/13   sb      Support for SGLTE/SGTDS coexistence
09/12/13   sc      RFLM interface encompassed by core DM
09/09/13   kab     Add temporary featurized changes to enable GSM phase data on chain 1 for 1625 SGLTE RF card
09/04/13   sb      Add temporary featurized changes to enable GSM phase data on chain 1 for SGLTE DSDA RF card
08/19/13   sc      Rename default num PA ramp entries
08/16/13   ndb     Clean-up the SPI based antenna tuner support
08/13/13   sc      Rename definitions ready for RFLM GSM merging
08/08/13   sc      Modify the ramp scaling to scale from a stored master table
08/08/13   sc      Add PA ramp initialisation storing NV tables in core DM
08/05/13   sc      Add PA ramp data management functions (featurised)
07/23/13   sc      Update RFLM data information (featurised)
07/22/13   ec	   Use new F3s/Null pointer checks
07/16/13   sc      Move mc/core functionality out of dispatcher and into mc/core layer
06/03/13   sb      Included right header file to enable ftm gsm logging
05/03/13   sb      Fixes for vbatt read taking too long
04/09/13   tsr     Added RF APPS->RF Task MSGR framework for temp comp to make temp comp data thread safe
04/09/13   sr      removed legacy antenna tuner code which is not used.
03/28/13   aka     GSM frame sweep feature changes.
02/07/13   tsr     Ensure that Temp comp calculation is thread safe
02/05/13   sb      Created separate APIs to configure RF front end devices based on whether
                   they are called every TX/RX burst or only when the band is changed.
01/22/12   sb      Changes to enable QPOET transactions to go out every gsm frame
01/09/13   sr      Made changes to run vbatt/temp comp in rf app task context.
01/3/13    av      Changes to support chain2 for GSM
12/19/12   sc      Remove Tx band change check as L1 monitor Tx band now
12/10/12   tws     Use core ftm_path delay value.
12/05/12   aka     changes to make path delay rfm device dependent
12/03/12   aka     Changes to use NV ptr from rfgsm_core_handle_ptr
11/27/12   tws     Add device id & as_id to FTM.
11/26/12   aka     Compiler warning fixes
11/26/12   aka     Changes to use a path specific gsm handle
11/21/12   av      Added rfm_device parameters to mc/core/mdsp/msm layers.
11/19/12   sc      RF-Only mDSP update shared variables cleanup
11/16/12   aka     Added RFDEVICE_GSM_SET_FREQ_ERROR to set freq err in WTR driver
11/15/12   ec      Using new GL1 test api for Triton compatibility
11/07/12   sb      Made changes to correctly apply path delay
10/18/12   sb      Made changes to include ssbis for PA/ASM devices in TX burst script (SVVZ bringup)
10/11/12   sc      Cleanup up unrequired Tx burst & SBI/Non-SBI API burst buffer params
10/01/12   sb      Made changes to add rfm device to rf device interface and uppler layer APIs 
09/25/12   tc      Mainline RF_HAS_RFA_GSM_TASK.  
09/24/12   sb      Made changes to update TXLM,RXLM modem chain
09/18/12   tc      Remove FEATURE_RF_HAS_QFE1320-related functionality.  
09/06/12   ggs     Removed ftm_tx_cal_set_band. Set Tx band should always 
                   be called from GL1 and not from ftm directly 
08/09/12   ggs     Added ftm_tx_cal_set_band and rearranged call flow for rfgsm_set_tx_band
08/07/12   sc      Using seperate Rx and Tx scripts for CCS buffer tracability
08/06/12   sc      Move rf_buffer_clear for scripts to the dispatcher APIs
08/02/12   sc      Add num slots to setup Tx burst API
07/26/12   sr      Changes to APIS to take rfm_dev as parameter.
07/20/12   sr      Made changes to pass the RFM_DEVICE and rf_buffer to rfc_gsm
07/12/12   sc      Cast to rf_buffer_intf type for device calls
07/06/12   sc      Edit API parameters to support CCS buffer format
07/02/12   sr      renamed rfc_common_gsmctl.h to rfc_common_tdd.h as it is shared with TDS.
06/27/12   sc      Passing correct pointer to device for tx band CCS script populating
06/21/12   sc      Enable shared memory support
06/20/12   shb     Updated GSM device APIs to accept script buffer
07/02/12   sn      PA/ASM device interface and scripting support
06/21/12   vb/rsr  Added support for QFE1510 
04/09/12   sb      Add changes to fix frequency error adjustment in FTM mode
02/28/12   dej     Hooked up PA start/stop delta adjustments in FTM mode. 
                   ( Added call to reconstruct grfc scripts in setup_tx_burst if tx_timing_adjust flag is set ) 
02/10/12   sb      Changes to support FTM logging for GSM
01/20/12   sn      Updated call flow for QFE1320, calling rfgsm_mdsp_set_qpa_tx_params
12/20/11   sn      Added support for QFE1320
01/10/12   sr      changes to enable vbatt comp.
10/28/11    sr     made changes use the tx_nv_tbl ptr for any tx NV items
                   and rx_nv_tbl for any rx NV items.
10/25/11   sr      Made changes to configure rfc signals to tx enable/disable mode.
10/19/11   sb      Apply therm read in idle frame and enable FTM temp comp and vbatt comp during idle frame
10/13/11   lcl     Added antenna tuner support for TX.
13/11/11    sr     made changes add F3 msgs via DPC task. 
                   and made changes to remove rfc calls from enable/disable tx.
10/11/11    sr     added rfc tx disable/enable calls to configure RFC signals 
                   for tx disable/enable state.
09/20/11    sr     made changes to update the tx_band variable correctly.
09/19/11   av      set_tx_band needs to be called by L1 before tuning for TX. 
                   Removing the assumption that set_rx_band is called and updating
                   from rx_band.
09/15/11    sr     added antenna tuner support code.
09/07/11   sb/rsr  Changes for Delay match setting to use new FW interface.
08/26/11   sr      made changes to call the rfm_set_tx_band() for cal.
08/06/11   sr      RFC Cleanup: removed un-used code.
07/11/11    sr     removed un-used code.
06/08/11    sr     Send the PA RANGE information fw with band change.
07/01/11   vrb     Replacing direct calls to rfgsm_ds with wrapper functions
03/23/11    sr     temp comp code cleanup.
03/21/11    sr     Added LM support.
03/10/11   sr      Moved Tx power related functions to new rfgsm_core_tx_power.c file
03/04/11    sr     featurization for nikel build
03/03/11    sr     merge from mdm9k branch
01/27/11   tsr     Enhanced Temp Comp Scheme for Predistorted PCL       
01/21/11    sc      Corrected VBATT compensation calculation arithmetic (+ to -)
                    CR258210 (Checked in before in gsmctl file on 12/08/10)
01/17/11   dej     Mainline RF_DEVICE_MERGE_INTERIM_FEATURE
01/12/11   lcl     Added hook to init TX indicator buffer
12/15/10   av      RFGSMCTL files cleanup
12/03/10   tsr     Changes to ensure that temp comp is not performed if in FTM mode       
10/18/10  av/lcl   Clean up common RFC support
10/19/10   lcl     Removed not useful F3 message
10/11/10   lcl     Changed MSG_LVL to MSG_LEGACY to avoid too many F3 messages
09/01/10   av      GSM RFC Rearch
08/19/10   lcl     Pick up temp comp from QSC6x95: 
                   Make sure we perform temp comp at the very first instance of set tx band 
07/27/10   lcl     Compiler warning fix.
06/15/10   rsr     Picked up Temp comp changes from QSC6x95...
                   [
                    04/13/10 jps  Adding support to add slope to AMAM table based on temp bin 
                    02/09/10 jps  Picked up LCU fixes. Fix to match TX slot number sent down 
                                  from GL1 
                    01/31/10 jps  Added check for modulation type change in set_tx_power API
                    01/18/10 sr   Setup msm and rfc for tx_band.
                    01/18/10 jps  Added support for path delay scaling based on NPLER / DIV mode  
                   ]
03/19/10   lcl     Pull in changes from QSC6295
03/18/10   lcl     Added declaration of rfgsm_core_scaled_path_delay
03/09/10   tsr     Defined and initialised rfgsm_mdsp_tx_pow_dbm
12/22/09   sr      changed the debug msg macro from legacy to new one.
12/22/09   sr/lp   Moved rfgsm_mdsp_set_delay_value from set band to tune tx to 
                   avoid crash DA Cal
12/06/09   sr      removed un-used code.
12/05/09   sr      removed un-used code. Moved the ramp table load function to
                   rfgsm_mdsp file.
12/02/09   sr      modified the use of rfgsm_card.tx_band.
10/21/09   sr      new device driver interface changes.
10/12/09   sr      Fixed warnings.
07/27/09   rsr     Cleanup
07/08/09   av      Added calls to rfc to configure the band data.
07/06/09   sr      changes to make the operating band to rfcom_gsm_band type
07/04/009  sr      changed the code to use tx_band instead of band.
06/26/009  sr      code cleanup.
06/06/009  sr      unused code cleanup.
04/02/009  sr      replaced the direct mdsp write with rfgsm_mdsp interface
03/27/009  sr      rfgsm_core_ctl name changed to rfc_card_gsmctl
03/18/09   sr      Changed the rfgsm_mdsp__() name to reflect new name
03/16/09   sr      removed the code which is not needed.
03/13/09   sr      code cleanup for SCMM build
10/30/08   sr      Initial revision.

============================================================================*/

#include "rfa_variation.h"
#include "rfcom.h"
#include "msg.h"
#include "ftm.h"
#include "ftm_common.h"
#include "rfdevice_gsm_intf.h"
#include "rfdevice_type_defs.h"
#include "rfgsm_core_types.h"
#include "rfgsm_nv_cmn.h"
#include "rfgsm_mdsp.h"
#include "rfgsm_msm.h"
#include "rfgsm_core_timing_consts.h"
#include "rfgsm_core_tx.h"
#include "rfc_card.h"
#include "rfgsm_core.h"
#include "rfc_common_tdd.h" 
#include "mdsp_intf.h"
#include "rfgsm_core_util.h"
#include "rfc_card_gsm.h"
#include "rf_hal_buffer.h"
#include "ftm_gsm_rfctl.h"
#include "geran_test_mode_api.h"
#include "rfgsm_core_temp_comp.h"
#include "rfgsm_core_vbatt_comp.h"
#include "stringl.h"
#include "rfcommon_core.h"
#include "rflm_gsm_dm.h"

#ifdef FEATURE_SGLTE
#include "rfcommon_nv_mm.h"
#endif
#include "rflm_rfdevice_typedefs.h"

#include "rfgsm_data_manager.h"

#include "rfm_internal.h"


/*----------------------------------------------------------------------------*/
/*EXTERNAL FUNCTIONS*/
void rfgsm_core_compute_linear_freq_comp (rfm_device_enum_type rfm_dev, rfcom_gsm_band_type band, uint16 arfcn);
void rfgsm_core_compute_pcl_vs_power (rfm_device_enum_type rfm_dev, rfcom_gsm_band_type band, uint16 arfcn);
void rfgsm_core_log_send_to_ftm_task(rfm_device_enum_type rfm_dev);

/*----------------------------------------------------------------------------*/
/*!
  @details
  This function sets up the GRFC and SBI/uwire buffer required for the
  Tx burst.

  @retval
  tx_alpha - Tx alpha value for the burst set up
*/
uint16 rfgsm_core_setup_tx_burst(rfgsm_core_data_type *core_data, uint8 num_tx_slots, void *tx_burst_event_ptr )
{
  uint16 tx_alpha= 0;
  sys_modem_as_id_e_type as_id = SYS_MODEM_AS_ID_NONE;
  uint16 delay_backoff;
  int16 final_delay_val;
  rfdevice_trx_gsm_env_delay_type path_delay;
  
  core_data->rfgsm_core_handle_ptr->tx_log.freq_error = rfgsm_core_get_tx_freq_error(core_data->rfgsm_core_handle_ptr);
  core_data->rfgsm_core_handle_ptr->tx_log.fn = geran_test_mode_api_get_FN(as_id);
  core_data->rfgsm_core_handle_ptr->tx_log.temp_comp_index = rfgsm_core_temp_comp_bin_index_get(core_data->rfm_dev);

   /* Set PA start/stop time in rf scripts. */
  if(core_data->rfgsm_core_handle_ptr->tx_timing_adjust_flag == TRUE)
  {
      /*If requested, recalculate timing based on the current tx band*/
      rfcom_gsm_band_type rfgsm_tx_band = core_data->rfgsm_core_handle_ptr->tx_band;
      rfc_gsm_tx_timing_adjust(core_data->rfm_dev, rfgsm_tx_band);

      /*Clear Flag*/
      core_data->rfgsm_core_handle_ptr->tx_timing_adjust_flag = FALSE;
  }

  /*Clear the TX script buffers first*/
  if(!rfgsm_core_clear_tx_burst_device_scripts(core_data->rfgsm_core_handle_ptr))
  {
    RF_MSG_1(RF_ERROR,"RF GSM WTR TX START/STOP script buffer was unsuccessfully cleared for rfm_dev|%d|",
                              core_data->rfm_dev);
  }
  /* Retrieve WTR device scripts for TX Burst */
  if( !rfgsm_core_populate_tx_burst_device_buffers(core_data,
  	                                           num_tx_slots,&path_delay) )
  {
    RF_MSG_1(RF_ERROR,"RF GSM Tx Burst WTR script buffers are not retrieved for rfm_dev|%d|",
                      core_data->rfm_dev);
  }

  /* Populate the Tx RFLM event data table */
  if( !rfgsm_core_dm_dynamic_tx_event_wrapper(core_data, 
                                              num_tx_slots) )
  {
    RF_MSG(MSG_LEGACY_ERROR,"RF GSM Tx RFLM dynamic event prog failed!");
  }

  /* Populate the Tx burst RFLM data information */
  rfgsm_core_populate_burst_rflm_data_info( core_data, RF_TX_BURST, num_tx_slots, tx_burst_event_ptr );

  delay_backoff = get_delay_backoff(core_data->rfgsm_core_handle_ptr->tx_band,core_data->rfm_dev);

  
  
  rfgsm_nv_calc_path_delay( core_data->rfm_dev, 
                            delay_backoff, 
                            &final_delay_val );

  #if 0
  rfgsm_core_dm_set_tx_path_delay( core_data->sub_id,
                                   core_data->triple_buffer_id,
                                   path_delay );
  #endif
//  RF_MSG_1(RF_ERROR,"final_delay_val|%d|",final_delay_val);
  
  rfgsm_core_dm_set_tx_path_delay( core_data->sub_id,
                                   core_data->triple_buffer_id,
                                   (int64)final_delay_val,
                                   core_data->rfgsm_core_handle_ptr);
  /* log the path delay value in Tx log packet*/
  core_data->rfgsm_core_handle_ptr->tx_log.delay_val = final_delay_val;

  #ifdef FTM_HAS_LOGGING
  rfgsm_core_log_send_to_ftm_task(core_data->rfm_dev);
  #endif
  
  return( tx_alpha );
}

/*----------------------------------------------------------------------------*/
/*!
  @details

*/
void rfgsm_core_set_tx_band( rfgsm_core_data_type *core_data,
                             rfcom_gsm_band_type band, 
                             void *shared_mem_ptr )
{
  core_data->rfgsm_core_handle_ptr->tx_log.tx_band = band;
  core_data->rfgsm_core_handle_ptr->tx_log.log_flag = TRUE;

  /* setup tx nv table pointer */
  core_data->rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl = rfgsm_nv_get_tbl_ptr(core_data->rfm_dev, band, RFGSM_NV_TX);

  RF_NULL_CHECK_RTN_V( core_data->rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl, "rfgsm_core_set_tx_band NULL Nv ptr!");
  RF_NULL_CHECK_RTN_V( core_data->rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl->tx_static_nv_data_ptr, "rfgsm_core_set_tx_band NULL Nv ptr!");

  /* Reset the previous tx channel when changing band. */
  core_data->rfgsm_core_handle_ptr->rf_tx_arfcn = RF_INVALID_VALUE_16BIT;

  /* msm_set_tx_band() need to be called before the rfc_tx_band_config, because we need to switch
  the PM mux from tcxo to PA_ON before we turn on the pmic mux */
  rfgsm_msm_set_tx_band(band);

  /* Clear the script buffer before use */
  if(!rf_buffer_clear((rf_buffer_intf*)core_data->rfgsm_core_handle_ptr->scripts.tx_band_script))
  {
    RF_MSG( MSG_LEGACY_ERROR, "RF GSM card Tx Band script buffer was unsuccessfully cleared" );
  }

  /* explicitly typecasting the rfcom_gsm_band_type to rf_card_band_type to
     fix  warnings. When the band parameter is used in rfc_<target>_gsm.c file
     it will be used as rfcom_gsm_band_type. */
  rfc_gsm_tx_band_config(core_data->rfm_dev, 
                         band, 
                         (rf_buffer_intf *)core_data->rfgsm_core_handle_ptr->scripts.tx_band_script); 

  /* Update the RFLM DM with the pa start and stop offsets */
  if( !rfgsm_core_dm_set_ramp_deltas( core_data->sub_id,
                                      core_data->rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl->tx_static_nv_data_ptr->tx_timing_data.pa_start_offset_adj,
                                      core_data->rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl->tx_static_nv_data_ptr->tx_timing_data.pa_stop_offset_adj,
                                      core_data->rfgsm_core_handle_ptr) )
  {
    RF_MSG(MSG_LEGACY_ERROR, "Tx ramp deltas are NOT updated");
    return;
  }

  /* Even at band change ensure that the mDSP GL1 vars updated for burst offset */
  rfgsm_mdsp_set_tx_band(core_data);

  /* Setup band dependent settings */
  /* For FTM TX calibration, WTR device need to perform set tx band sbi writes via SW CCS cmd interface instead
     of via FW CCS interface */
  rfdevice_gsm_set_tx_band( core_data->rfm_dev, band, RFDEVICE_CREATE_SCRIPT,(rf_buffer_intf*)core_data->rfgsm_core_handle_ptr->scripts.tx_band_script);

  /* Populate the Tx band RFLM event data table */
  if( !rfgsm_core_dm_dynamic_tx_band_event_wrapper(core_data) )
  {
    RF_MSG(MSG_LEGACY_ERROR,"RF GSM Tx band RFLM dynamic event prog failed!");
  }

  /* Populate the Tx band RFLM data information */
  if( !rfgsm_core_dm_populate_tx_band_intf_data( core_data, (void*)shared_mem_ptr ) )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"RF GSM Tx band RFLM intf data update failed!");
  }

  /* Calculate the vbatt compensation value based on new band. */ 
  rfgsm_core_vbatt_comp_trigger(core_data->rfm_dev);

  /* Update tx profile due to tx band change */
  rfgsm_core_set_tx_profile_update_flag(core_data->rfgsm_core_handle_ptr);

  core_data->rfgsm_core_handle_ptr->tx_band = band;   
} /* end of rfgsm_set_tx_band */

#ifdef RFGSM_FSK_ENABLE

/*----------------------------------------------------------------------------*/
/*!
  @details
  This function sets up the GRFC and SBI/uwire buffer required for the
  Tx burst.

  @retval
  tx_alpha - Tx alpha value for the burst set up
*/
uint16 rfgsm_core_setup_fsk_tx_burst(rfgsm_core_data_type *core_data, uint8 num_tx_slots, void *tx_burst_event_ptr )
{
  uint16 tx_alpha= 0;
  sys_modem_as_id_e_type as_id = SYS_MODEM_AS_ID_NONE;
  uint16 delay_backoff;
  int16 final_delay_val;
  rfdevice_trx_gsm_env_delay_type path_delay;
  
  core_data->rfgsm_core_handle_ptr->tx_log.freq_error = rfgsm_core_get_tx_freq_error(core_data->rfgsm_core_handle_ptr);
  core_data->rfgsm_core_handle_ptr->tx_log.fn = geran_test_mode_api_get_FN(as_id);
  core_data->rfgsm_core_handle_ptr->tx_log.temp_comp_index = rfgsm_core_temp_comp_bin_index_get(core_data->rfm_dev);

   /* Set PA start/stop time in rf scripts. */
  if(core_data->rfgsm_core_handle_ptr->tx_timing_adjust_flag == TRUE)
  {
      /*If requested, recalculate timing based on the current tx band*/
      rfcom_gsm_band_type rfgsm_tx_band = core_data->rfgsm_core_handle_ptr->tx_band;
      rfc_gsm_tx_timing_adjust(core_data->rfm_dev, rfgsm_tx_band);

      /*Clear Flag*/
      core_data->rfgsm_core_handle_ptr->tx_timing_adjust_flag = FALSE;
  }

  /*Clear the TX script buffers first*/
  if(!rfgsm_core_clear_fsk_tx_burst_device_scripts(core_data->rfgsm_core_handle_ptr))
  {
    RF_MSG_1(RF_ERROR,"RF GSM FSK WTR TX START/STOP script buffer was unsuccessfully cleared for rfm_dev|%d|",
                              core_data->rfm_dev);
  }
  /* Retrieve WTR device scripts for TX Burst */
  if( !rfgsm_core_populate_fsk_tx_burst_device_buffers(core_data,
  	                                           num_tx_slots,&path_delay) )
  {
    RF_MSG_1(RF_ERROR,"RF GSM fsk Tx Burst WTR script buffers are not retrieved for rfm_dev|%d|",
                      core_data->rfm_dev);
  }
  /*populate the wtr afc scripts*/

  RF_MSG_3(RF_HIGH,"rfgsm_core_setup_fsk_tx_burst(): rf_fc_in_khz = %d, rf_freq_fc_residual_freq_in_hz = %d, rf_freq_offset = %d",
                      core_data->rfgsm_core_handle_ptr->rf_fsk_tx_fc, core_data->rfgsm_core_handle_ptr->rf_fsk_tx_fc_residual_freq_in_hz, core_data->rfgsm_core_handle_ptr->rf_fsk_offset_hz);
  
  if( !rfdevice_gsm_get_fsk_afc_script(core_data->rfm_dev,
		                                 core_data->rfgsm_core_handle_ptr->tx_band,
		                                 core_data->rfgsm_core_handle_ptr->rf_fsk_tx_fc,
		                                 (core_data->rfgsm_core_handle_ptr->rf_fsk_offset_hz + core_data->rfgsm_core_handle_ptr->rf_fsk_tx_fc_residual_freq_in_hz), // add the residual frequency in Hz to freq offset as the SDR accepts the Fc in KHz
  	                                     core_data->rfgsm_core_handle_ptr->scripts.fsk_tx_afc_pos_script,
  	                                     RFDEVICE_CREATE_SCRIPT) )
  {
    RF_MSG_1(RF_ERROR,"RF GSM fsk Tx Burst WTR POS script buffers are not retrieved for rfm_dev|%d|",
                      core_data->rfm_dev);
  }
  if( !rfdevice_gsm_get_fsk_afc_script(core_data->rfm_dev,
		                                 core_data->rfgsm_core_handle_ptr->tx_band,
		                                 core_data->rfgsm_core_handle_ptr->rf_fsk_tx_fc,
		                                ((-1)* core_data->rfgsm_core_handle_ptr->rf_fsk_offset_hz + core_data->rfgsm_core_handle_ptr->rf_fsk_tx_fc_residual_freq_in_hz), // add the residual frequency in Hz to freq offset as the SDR accepts the Fc in KHz
  	                                     core_data->rfgsm_core_handle_ptr->scripts.fsk_tx_afc_neg_script,
  	                                     RFDEVICE_CREATE_SCRIPT) )
  {
    RF_MSG_1(RF_ERROR,"RF GSM fsk Tx Burst WTR NEG script buffers are not retrieved for rfm_dev|%d|",
                      core_data->rfm_dev);
  }										   
  /* Populate the Tx RFLM event data table */
  if( !rfgsm_core_dm_dynamic_fsk_tx_event_wrapper(core_data, 
                                              num_tx_slots) )
  {
    RF_MSG(MSG_LEGACY_ERROR,"RF GSM FSK Tx RFLM dynamic event prog failed!");
  }
  if( !rfgsm_core_dm_dynamic_fsk_tx_afc_pos_event_wrapper(core_data, 
											   num_tx_slots) )
   {
	 RF_MSG(MSG_LEGACY_ERROR,"RF GSM FSK Tx AFC POS RFLM dynamic event prog failed!");
   }
  if( !rfgsm_core_dm_dynamic_fsk_tx_afc_neg_event_wrapper(core_data, 
												 num_tx_slots) )
	 {
	   RF_MSG(MSG_LEGACY_ERROR,"RF GSM FSK Tx AFC NEG RFLM dynamic event prog failed!");
	 }

  /* Populate the Tx burst RFLM data information */
  rfgsm_core_populate_fsk_burst_rflm_data_info( core_data, RF_TX_BURST, num_tx_slots, tx_burst_event_ptr );

  delay_backoff = get_delay_backoff(core_data->rfgsm_core_handle_ptr->tx_band,core_data->rfm_dev);

  
  
  rfgsm_nv_calc_path_delay( core_data->rfm_dev, 
                            delay_backoff, 
                            &final_delay_val );

  #if 0
  rfgsm_core_dm_set_tx_path_delay( core_data->sub_id,
                                   core_data->triple_buffer_id,
                                   path_delay );
  #endif
//  RF_MSG_1(RF_ERROR,"final_delay_val|%d|",final_delay_val);
  
  rfgsm_core_dm_set_fsk_tx_path_delay( core_data->sub_id,
                                   core_data->triple_buffer_id,
                                   (int64)final_delay_val );
  /* log the path delay value in Tx log packet*/
  core_data->rfgsm_core_handle_ptr->tx_log.delay_val = final_delay_val;

  #ifdef FTM_HAS_LOGGING
  rfgsm_core_log_send_to_ftm_task(core_data->rfm_dev);
  #endif
  
  return( tx_alpha );
}

/*----------------------------------------------------------------------------*/
/*!
  @details

*/
void rfgsm_core_fsk_tx_band( rfgsm_core_data_type *core_data,
                             rfcom_gsm_band_type band, 
                             void *shared_mem_ptr )
{
  core_data->rfgsm_core_handle_ptr->tx_log.tx_band = band;
  core_data->rfgsm_core_handle_ptr->tx_log.log_flag = TRUE;

  /* setup tx nv table pointer */
  core_data->rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl = rfgsm_nv_get_tbl_ptr(core_data->rfm_dev, band, RFGSM_NV_TX);

  RF_NULL_CHECK_RTN_V( core_data->rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl, "rfgsm_core_set_tx_band NULL Nv ptr!");
  RF_NULL_CHECK_RTN_V( core_data->rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl->tx_static_nv_data_ptr, "rfgsm_core_set_tx_band NULL Nv ptr!");

  /* Reset the previous tx channel when changing band. */
  core_data->rfgsm_core_handle_ptr->rf_tx_arfcn = RF_INVALID_VALUE_16BIT;

  /* msm_set_tx_band() need to be called before the rfc_tx_band_config, because we need to switch
  the PM mux from tcxo to PA_ON before we turn on the pmic mux */
  rfgsm_msm_set_tx_band(band);

  /* Clear the script buffer before use */
  if(!rf_buffer_clear((rf_buffer_intf*)core_data->rfgsm_core_handle_ptr->scripts.fsk_tx_band_script))
  {
    RF_MSG( MSG_LEGACY_ERROR, "RF GSM card FSK Tx enable script buffer was unsuccessfully cleared" );
  }

  /* explicitly typecasting the rfcom_gsm_band_type to rf_card_band_type to
     fix  warnings. When the band parameter is used in rfc_<target>_gsm.c file
     it will be used as rfcom_gsm_band_type. */
  rfc_gsm_tx_band_config(core_data->rfm_dev, 
                         band, 
                         (rf_buffer_intf *)core_data->rfgsm_core_handle_ptr->scripts.fsk_tx_band_script); 

  /* Update the RFLM DM with the pa start and stop offsets */
  if( !rfgsm_core_dm_set_ramp_deltas( core_data->sub_id,
                                      core_data->rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl->tx_static_nv_data_ptr->tx_timing_data.pa_start_offset_adj,
                                      core_data->rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl->tx_static_nv_data_ptr->tx_timing_data.pa_stop_offset_adj,
                                      core_data->rfgsm_core_handle_ptr) )
  {
    RF_MSG(MSG_LEGACY_ERROR, "Tx ramp deltas are NOT updated");
    return;
  }

  /* Even at band change ensure that the mDSP GL1 vars updated for burst offset */
  rfgsm_mdsp_set_tx_band(core_data);

  /* Setup band dependent settings */
  /* For FTM TX calibration, WTR device need to perform set tx band sbi writes via SW CCS cmd interface instead
     of via FW CCS interface */
  rfdevice_gsm_set_tx_band( core_data->rfm_dev, band, RFDEVICE_CREATE_SCRIPT,(rf_buffer_intf*)core_data->rfgsm_core_handle_ptr->scripts.fsk_tx_band_script);

  /* Populate the Tx band RFLM event data table */
  if( !rfgsm_core_dm_dynamic_fsk_tx_band_event_wrapper(core_data) )
  {
    RF_MSG(MSG_LEGACY_ERROR,"RF GSM Tx band RFLM dynamic event prog failed!");
  }

  /* Populate the Tx band RFLM data information */
  //TODO_MANI
  if( !rfgsm_core_dm_populate_fsk_tx_band_intf_data( core_data, (void*)shared_mem_ptr ) )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"RF GSM FSK Tx ENABLE RFLM intf data update failed!");
  }

  /* Calculate the vbatt compensation value based on new band. */ 
  rfgsm_core_vbatt_comp_trigger(core_data->rfm_dev);

  /* Update tx profile due to tx band change */
  rfgsm_core_set_tx_profile_update_flag(core_data->rfgsm_core_handle_ptr);

  core_data->rfgsm_core_handle_ptr->tx_band = band;   
} /* end of rfgsm_set_tx_band */




/*----------------------------------------------------------------------------*/
/*!
  @details
  This function sets up the Tx scripts to program the synthesizer to the
  requested channel.

  rfgsm_set_band must be called before this function or the 
  FEATURE_SET_BAND_IN_GL1 must be turned off so it may be called in this 
  function before proceseding

  @retval
  TRUE/FALSE - sucessful/unsuccessful tune.
*/

boolean rfgsm_core_tune_fsk_tx(rfgsm_core_data_type *core_data, uint32 rf_fc)
{
  rfcom_gsm_band_type tx_band = RFCOM_NUM_GSM_BANDS;
  boolean use_coex_filtered_path = FALSE;
  boolean flag = FALSE;
  uint16 arfcn=0;
  rfc_gsm_coex_band_info_type band_split_info;

  rfgsm_nv_band_data_type *rfgsm_tx_nv_table = NULL;

  tx_band = core_data->rfgsm_core_handle_ptr->tx_band;
  band_split_info.band = core_data->rfgsm_core_handle_ptr->tx_band;
  band_split_info.status = RFC_FILTERED_PATH_FALSE;
 
  if ( (core_data->rfgsm_core_handle_ptr->rf_fsk_tx_fc != rf_fc) || (core_data->rfgsm_core_handle_ptr->temp_comp.temp_comp_updated == TRUE) )
  {
    /* Set the amam_ampm recalc required flag so it will be done during next tx_power update */
    core_data->rfgsm_core_handle_ptr->amam_ampm_recalc_required = TRUE;    

    if( !rfgsm_core_tx_amam_ampm_data_buffer_switch( core_data->rfm_dev ) )
    {
      RF_MSG(MSG_LEGACY_ERROR, "RF GSM AMAM AMPM table ping-pong failed!");
      return FALSE;
    }
  }

  /* Recompute freq comp for amam table , if frequency is changing */
  if ( core_data->rfgsm_core_handle_ptr->rf_fsk_tx_fc != rf_fc )
  { 
    arfcn=rfgsm_core_convert_tx_freq_to_arfcn(core_data->rfgsm_core_handle_ptr->tx_band,rf_fc);
	core_data->rfgsm_core_handle_ptr->tx_log.tx_chan = arfcn;
  core_data->rfgsm_core_handle_ptr->tx_log.log_flag = TRUE;
    rfgsm_core_compute_linear_freq_comp(core_data->rfm_dev, core_data->rfgsm_core_handle_ptr->tx_band,arfcn);

  /*This API retrieves RF configuration for all FE devices including GRFCs,ASM,PA,ATUNER etc. for a give TX burst*/
  rfc_gsm_fsk_tx_burst_config(core_data->rfm_dev,
                          tx_band, 
                          arfcn);

    /* Compensate pcl vs power curve with calibrated err data */
    rfgsm_core_compute_pcl_vs_power(core_data->rfm_dev, core_data->rfgsm_core_handle_ptr->tx_band, arfcn);

    core_data->rfgsm_core_handle_ptr->ftm_path_delay_cal_override = FALSE;

    rfgsm_core_set_tx_profile_update_flag(core_data->rfgsm_core_handle_ptr);
	  /* Update the tx arfcn */
    core_data->rfgsm_core_handle_ptr->rf_tx_arfcn = arfcn;
	 core_data->rfgsm_core_handle_ptr->rf_fsk_tx_fc = rf_fc;
    RF_MSG_2(MSG_LEGACY_ERROR, "core_data->rfgsm_core_handle_ptr->rf_tx_arfcn %d rf_freq %d ",core_data->rfgsm_core_handle_ptr->rf_tx_arfcn,rf_fc);

  } /* rf_tx_arfcn != arfcn */



  return( TRUE );
} /* end of rfgsm_tune_tx */



/*----------------------------------------------------------------------------*/
/*!
   @brief
   Enable/Disable the Tx
   
*/
void rfgsm_core_fsk_enable_tx(boolean enable, uint32 txlm_buf_idx, rfgsm_core_data_type *core_data)
{
  rf_path_enum_type rf_path;
  uint16 rf_dev_sub_mask = ((uint16)core_data->rfm_dev | (uint16) (core_data->sub_id << 8));

  if (enable)
  {
    rfgsm_core_enter_mode_tx_init(core_data);
#if 1
    if( rfgsm_debug_flags.disable_temp_comp == FALSE )
    {
       /* trigger thermistor read */
       rfgsm_core_therm_read_trigger(0, rf_dev_sub_mask);
    }
#endif
    /* trigger vbatt read */
    rfgsm_core_vbatt_read_trigger(0, core_data->rfm_dev, FALSE);

    /* Start Vbatt Tx compensation timer. */
    rfgsm_core_vbatt_comp_start(core_data->rfm_dev);
#if 1
    if( rfgsm_debug_flags.disable_temp_comp == FALSE )
    {
       /* Start PA Temp Tx compensation timer.*/
       rfgsm_core_temp_comp_start(core_data->rfm_dev, core_data->sub_id);
    }
#endif
    //rfgsm_msm_enable_tx(txlm_buf_idx, core_data->rfm_dev, FALSE, core_data->rfgsm_core_handle_ptr->tx_alternate_path);

    rfc_gsm_fsk_tx_enable(core_data->rfm_dev);

    //core_data->rfgsm_core_handle_ptr->txlm_buf_index = txlm_buf_idx;

    rf_path = rfcommon_core_device_to_path(core_data->rfm_dev);
    //This enable the TX DAC VREF 	  
    rfgsm_core_turn_on_vregs(rf_path, core_data->sub_id, RF_PATH_RXTX_STATE);
	  
  }
  else
  {
    /* Stop Vbatt Tx pwr compensation.*/
    rfgsm_core_vbatt_comp_stop(core_data->rfm_dev);

    /* Stop PA Temp pwr compensation.*/
    rfgsm_core_temp_comp_stop(core_data->rfm_dev);

    rfgsm_msm_disable_tx();
    rfc_gsm_fsk_tx_disable(core_data->rfm_dev);
  }
}


boolean rfgsm_core_clear_fsk_tx_burst_device_scripts( rfgsm_core_handle_type *rfgsm_core_handle_ptr)
{
  int i=0;
  if( !rf_buffer_clear_script( (rf_buffer_intf *)rfgsm_core_handle_ptr->scripts.fsk_tx_start_script[TX_WTR_SCRIPT], RF_BUFFER_ALL)||
      !rf_buffer_clear_script( (rf_buffer_intf *)rfgsm_core_handle_ptr->scripts.fsk_tx_stop_script[TX_WTR_SCRIPT], RF_BUFFER_ALL) ||
      !rf_buffer_clear_script( (rf_buffer_intf *)rfgsm_core_handle_ptr->scripts.fsk_tx_afc_pos_script, RF_BUFFER_ALL) ||
      !rf_buffer_clear_script( (rf_buffer_intf *)rfgsm_core_handle_ptr->scripts.fsk_tx_afc_neg_script, RF_BUFFER_ALL)||
      !rf_buffer_clear_script( (rf_buffer_intf *)rfgsm_core_handle_ptr->scripts.fsk_tx_trans_script[TX_TRANS_WTR_SCRIPT][0], RF_BUFFER_ALL))
      
  {
    return FALSE;
  }
  
  return TRUE;
}

/*----------------------------------------------------------------------------*/
/*!
   @fn rfgsm_core_populate_tx_burst_device_buffers 
   @brief This function retrieves wtr device driver scripts for GSM Burst and stores them in rfgsm_core_handles pointer
   @param  rfm_dev - RFM device processing for
   @param  burst - TX
   @retval boolean - success/failure indication
*/

boolean rfgsm_core_populate_fsk_tx_burst_device_buffers( rfgsm_core_data_type *core_data,uint8 num_tx_slots, rfdevice_trx_gsm_env_delay_type *env_delay)
{

  uint16 buffer_mask;
  void* script_dm_ptr = NULL;
  #ifdef FEATURE_TX_SHARING
  uint16 temp_tx_num_trans = 0;
  #endif

  boolean build_trans_script = TRUE;
  uint8 tx_slot_index = 0;
  uint64 final_lo_freq_hz=0;
  uint64 req_rf_freq_hz=0;
  int64 freq_error=0;
  rfdevice_trx_timer_fixup_type trx_timer_fixup_array[RFDEVICE_TRX_MAX_TIMER_FIXUPS_PER_SCRIPT];
  uint8 timer_fixup_arr_size = 0;
  /* Initialise variables */
  rflm_fsk_data_type *table_ptr = NULL;

  boolean phase_path_on_off = FALSE;

  /* Get the data table pointer from DM */
  table_ptr = rflm_fsk_dm_get_table_ptr();
  if( table_ptr == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, 
          "RF GSM rfgsm_core_populate_tx_burst_device_buffers for sub %d",
          core_data->sub_id );
    return FALSE;
  }

  
  req_rf_freq_hz=(uint64)((core_data->rfgsm_core_handle_ptr->rf_fsk_tx_fc)*1000);
  final_lo_freq_hz =  req_rf_freq_hz;
  freq_error=core_data->rfgsm_core_handle_ptr->rfgsm_curr_freq_error_tx;
  freq_error = freq_error*(final_lo_freq_hz >> 15);
  freq_error = freq_error/31250;

  rfgsm_core_dm_set_freq_error( core_data->sub_id, (int16)freq_error,core_data->rfgsm_core_handle_ptr );
  MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, 
			"RF GSM rfgsm_core_populate_tx_burst_device_buffers for rf_freq %d",
			req_rf_freq_hz );

  if(!rfdevice_gsm_tune_fsk_tx(core_data->rfm_dev,
                           core_data->rfgsm_core_handle_ptr->tx_band,
                           req_rf_freq_hz,
                           freq_error,
                           (rf_buffer_intf*)core_data->rfgsm_core_handle_ptr->scripts.fsk_tx_start_script[TX_WTR_SCRIPT],
                           &trx_timer_fixup_array,
                           &timer_fixup_arr_size)
                           )
  {
    RF_MSG_1( MSG_LEGACY_HIGH,"Building RF device script failed for TX start script",0);
  }

// to disable the phase path during FSK Tx".

if(!rfdevice_gsm_set_tx_phase_const(core_data->rfm_dev,
						 core_data->rfgsm_core_handle_ptr->tx_band,
						   phase_path_on_off,
						   (rf_buffer_intf*)core_data->rfgsm_core_handle_ptr->scripts.fsk_tx_start_script[TX_WTR_SCRIPT])
						 )
{
  RF_MSG_1( MSG_LEGACY_HIGH,"failed to build rfdevice phase path toggle script ",0);
}

//TODO_MANI 
//write the afc update driver function

    rfdevice_gsm_set_rgi(  core_data->rfm_dev, 
                           core_data->rfgsm_core_handle_ptr->tx_band,
                           core_data->rfgsm_core_handle_ptr->rgi_slot,
                           core_data->rfgsm_core_handle_ptr->last_mod_type,
                           (rf_buffer_intf **)core_data->rfgsm_core_handle_ptr->scripts.fsk_tx_trans_script[TX_TRANS_WTR_SCRIPT],
                           1, /* num_slots */
                           TRUE,
                           core_data->rfgsm_core_handle_ptr->rf_tx_arfcn,
                           &trx_timer_fixup_array,
                           &timer_fixup_arr_size,
                           RFDEVICE_TRX_RX_WXE_BUF_0);  /* Update the device driver burst script */
  
if( !rfdevice_gsm_tx_disable( core_data->rfm_dev,
                               core_data->rfgsm_core_handle_ptr->tx_band,
                              (rf_buffer_intf*)core_data->rfgsm_core_handle_ptr->scripts.fsk_tx_stop_script[TX_WTR_SCRIPT]) )
  {
    RF_MSG_1( MSG_LEGACY_HIGH,"Building script for RF device failed for TX stop script",0);
  }

  /*Populating Device Status Read Event Scripts During Tx Burst */
  //TODO_MANI
  /*if(core_data->rfgsm_core_handle_ptr->device_config_to_read.params[RF_TX_BURST].enable)
  {
    rfgsm_core_populate_device_status_read_buffers(core_data, RF_TX_BURST);
  }*/
  
  rfgsm_core_log(core_data->rfm_dev);

  return TRUE;
} /* rfgsm_core_populate_device_tx_burst_buffer */

#endif

/*----------------------------------------------------------------------------*/
/*!
  @details
  This function sets up the Tx scripts to program the synthesizer to the
  requested channel.

  rfgsm_set_band must be called before this function or the 
  FEATURE_SET_BAND_IN_GL1 must be turned off so it may be called in this 
  function before proceseding

  @retval
  TRUE/FALSE - sucessful/unsuccessful tune.
*/

boolean rfgsm_core_tune_tx(rfgsm_core_data_type *core_data, uint16 arfcn)
{
  rfcom_gsm_band_type tx_band = RFCOM_NUM_GSM_BANDS;
  boolean use_coex_filtered_path = FALSE;
  boolean flag = FALSE;
  rfc_gsm_coex_band_info_type band_split_info;

  rfgsm_nv_band_data_type *rfgsm_tx_nv_table = NULL;

  tx_band = core_data->rfgsm_core_handle_ptr->tx_band;
  band_split_info.band = core_data->rfgsm_core_handle_ptr->tx_band;
  band_split_info.status = RFC_FILTERED_PATH_FALSE;


  /* Set or clear bypass_filtered path flag*/
  core_data->rfgsm_core_handle_ptr->use_coex_filtered_path_tx = rfcommon_core_get_tx_filter_status();

  use_coex_filtered_path = core_data->rfgsm_core_handle_ptr->use_coex_filtered_path_tx;


  if (use_coex_filtered_path == TRUE) /* Filtered path needs to be use based on the channel*/
  {
    band_split_info.status = RFC_FILTERED_PATH_TRUE;
    flag = rfc_get_split_band_type(core_data->rfm_dev,RFC_CONFIG_TX,core_data->rfgsm_core_handle_ptr->tx_band, arfcn, &band_split_info );

    if (flag == TRUE)
    {

        /* updated band enum info*/
        tx_band = band_split_info.band;

        if(core_data->rfgsm_core_handle_ptr->coex_filtered_path_cal_flag == TRUE) 
        /* If filtered path has been calibrated then use the data for filtered path by using the split band enum type*/
        {
          rfgsm_tx_nv_table = rfgsm_nv_get_tbl_ptr(core_data->rfm_dev, tx_band, RFGSM_NV_TX);
          if (rfgsm_tx_nv_table != NULL)
          {
           /*Update the TX NV band data buffer pointer(Static + cal data) in the core handle for the split band. 
            Though the band data buffer datatype contains both TX and RX static and cal data for a band enum, 
            separate band data buffer pointers are maintained for TX and RX in the GSM core handles.
          */
            core_data->rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl = rfgsm_tx_nv_table;
          }
          else
          {
            RF_MSG(MSG_LEGACY_ERROR,"rfgsm_core_tune_tx, TX NV table is NULL for SPLIT band. Update TX cal data failed");
          }
        }
    }
    else
    {
      tx_band = rfc_get_unfiltered_band_type(core_data->rfm_dev,RFC_CONFIG_TX,core_data->rfgsm_core_handle_ptr->tx_band);

    }
  }
  else
  {

    tx_band = rfc_get_unfiltered_band_type(core_data->rfm_dev, RFC_CONFIG_TX, core_data->rfgsm_core_handle_ptr->tx_band);
  }
  /*This API retrieves RF configuration for all FE devices including GRFCs,ASM,PA,ATUNER etc. for a give TX burst*/
  rfc_gsm_tx_burst_config(core_data->rfm_dev,
                          tx_band, 
                          arfcn);

 
  if ( (core_data->rfgsm_core_handle_ptr->rf_tx_arfcn != arfcn) || (core_data->rfgsm_core_handle_ptr->temp_comp.temp_comp_updated == TRUE) )
  {
    /* Set the amam_ampm recalc required flag so it will be done during next tx_power update */
    core_data->rfgsm_core_handle_ptr->amam_ampm_recalc_required = TRUE;    

    if( !rfgsm_core_tx_amam_ampm_data_buffer_switch( core_data->rfm_dev ) )
    {
      RF_MSG(MSG_LEGACY_ERROR, "RF GSM AMAM AMPM table ping-pong failed!");
      return FALSE;
    }
  }

  /* Recompute freq comp for amam table , if frequency is changing */
  if ( core_data->rfgsm_core_handle_ptr->rf_tx_arfcn != arfcn )
  { 
    core_data->rfgsm_core_handle_ptr->tx_log.tx_chan = arfcn;
    core_data->rfgsm_core_handle_ptr->tx_log.log_flag = TRUE;

    rfgsm_core_compute_linear_freq_comp(core_data->rfm_dev, core_data->rfgsm_core_handle_ptr->tx_band,arfcn);

    /* Compensate pcl vs power curve with calibrated err data */
    rfgsm_core_compute_pcl_vs_power(core_data->rfm_dev, core_data->rfgsm_core_handle_ptr->tx_band, arfcn);

    core_data->rfgsm_core_handle_ptr->ftm_path_delay_cal_override = FALSE;

    rfgsm_core_set_tx_profile_update_flag(core_data->rfgsm_core_handle_ptr);

  } /* rf_tx_arfcn != arfcn */

  /* Update the tx arfcn */
  core_data->rfgsm_core_handle_ptr->rf_tx_arfcn = arfcn;

  return( TRUE );
} /* end of rfgsm_tune_tx */

/*----------------------------------------------------------------------------*/
/*!
   @brief
  This function updates the frequency error (in ppb).
  For example, if user passes a value of 2054  (ppb) it corresponds to 2054/1024 = 2.005 ppm. The resolution of frequency error is hence .001 ppm.
  @details

*/
void rfgsm_core_update_tx_freq_error(rfgsm_core_data_type *core_data, int32  freq_err)
{
  core_data->rfgsm_core_handle_ptr->rfgsm_curr_freq_error_tx= freq_err;
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function returns the value of frequency error (in ppb)
   @details

   @param 

   @retval 
   
*/
int32 rfgsm_core_get_tx_freq_error(rfgsm_core_handle_type *rfgsm_core_handle_ptr)
{
  return rfgsm_core_handle_ptr->rfgsm_curr_freq_error_tx;
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
   Enable/Disable the Tx
   
*/
void rfgsm_core_enable_tx(boolean enable, uint32 txlm_buf_idx, rfgsm_core_data_type *core_data)
{
  rf_path_enum_type rf_path;
  uint16 rf_dev_sub_mask = ((uint16)core_data->rfm_dev | (uint16) (core_data->sub_id << 8));

  if (enable)
  {
    rfgsm_core_enter_mode_tx_init(core_data);
#if 1
    if( rfgsm_debug_flags.disable_temp_comp == FALSE )
    {
       /* trigger thermistor read */
       //rfgsm_core_therm_read_trigger(0, rf_dev_sub_mask);
    }
#endif
    /* trigger vbatt read */
    rfgsm_core_vbatt_read_trigger(0, core_data->rfm_dev, FALSE);

    /* Start Vbatt Tx compensation timer. */
    rfgsm_core_vbatt_comp_start(core_data->rfm_dev);
#if 1
    if( rfgsm_debug_flags.disable_temp_comp == FALSE )
    {
       /* Start PA Temp Tx compensation timer.*/
       rfgsm_core_temp_comp_start(core_data->rfm_dev, core_data->sub_id);
    }
#endif
    rfgsm_msm_enable_tx(txlm_buf_idx, core_data->rfm_dev, FALSE, core_data->rfgsm_core_handle_ptr->tx_alternate_path);

    rfc_gsm_tx_enable(core_data->rfm_dev);

    core_data->rfgsm_core_handle_ptr->txlm_buf_index = txlm_buf_idx;

    rf_path = rfcommon_core_device_to_path(core_data->rfm_dev);
    //This enable the TX DAC VREF 	  
    rfgsm_core_turn_on_vregs(rf_path, core_data->sub_id, RF_PATH_RXTX_STATE);
	  
  }
  else
  {
    /* Stop Vbatt Tx pwr compensation.*/
    rfgsm_core_vbatt_comp_stop(core_data->rfm_dev);

    /* Stop PA Temp pwr compensation.*/
    rfgsm_core_temp_comp_stop(core_data->rfm_dev);

    rfgsm_msm_disable_tx();
    rfc_gsm_tx_disable(core_data->rfm_dev);
  }
}


void rfgsm_core_set_gl1_freq_error_flag ( rfm_device_enum_type rfm_dev, boolean pass_flag)
{

	 rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;
	 
	 rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);
	 
     RF_NULL_CHECK_RTN_V(rfgsm_core_handle_ptr,"rfgsm_core_handle_ptr is NULL!");
    
     rfgsm_core_handle_ptr->freq_error_flag_from_api = pass_flag;
     //MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH," FLAG set by FREQ ADJUST command : :%d", freq_error_flag_from_api);

   
}

boolean rfgsm_core_get_gl1_freq_error_flag(rfgsm_core_handle_type *rfgsm_core_handle_ptr)
{
  return rfgsm_core_handle_ptr->freq_error_flag_from_api;
}



/*---------------------------------------------------------------------------*/
/*! @brief This function populates the default master PA tables to the DM MEM*/
/*! @details This function populates the band-specific tables for the maximum*/
/*!          possible number of slots.                                       */
/*! @param rfm_dev - which RFM device to program event for                   */
/*! @retval boolean - success status                                         */
boolean rfgsm_core_init_master_pa_ramp_tables( rfgsm_core_data_type *core_data )
{
  uint16 *master_ramp_up_tbl_ptr = NULL;
  uint16 *master_ramp_down_tbl_ptr = NULL;
  rfgsm_nv_band_data_type *rfgsm_temp_tx_nv_tbl = NULL;
  gsm_pa_ramp_lut_type *polar_ramp_ptr = NULL;
  rflm_gsm_pa_ramp_type temp_ramps_var; //used for memscpy
  rflm_gsm_band_type tx_band = RFLM_GSM_NUM_GSM_BANDS;
  boolean ret_val = FALSE;
  uint16 rfgsm_pa_ramp_up_76p8[RFLM_GSM_MAX_NUM_RAMP_SAMPLES] = {0};
  uint16 rfgsm_pa_ramp_down_76p8[RFLM_GSM_MAX_NUM_RAMP_SAMPLES] = {0};

  /*-------------------------------------------------------------------------*/
  /*@TODO As the NV PA ramp tables are only for one WTR sampling frequency we*/
  /*      need to modify the table if the WTR is of another freq. Therefore, */
  /*      based on device, the ramp may or may not need up-sampling and      */
  /*      spline interpolation. A call to device will return the sampling    */
  /*      freq of that WTR and then we can make the decision and interpolate */
  /*      here if required. The result will be based on polar_ramp_ptr but   */
  /*      assigned to a seperate temporary ramp table local variable which   */
  /*      will then be copied to the master core DM table.                   */
  /*      NOTE: Currently only 52MHz (24 entries per table) is supported here*/

  /* Initialise the number of ramp entries */
  /* @TODO Replace the assignment with an actual retrieval function based on WTR */
  core_data->rfgsm_core_handle_ptr->num_pa_ramp_table_entries = RFLM_GSM_MAX_NUM_RAMP_SAMPLES;

  /* Copy all of the band tables */
  for ( tx_band = 0; tx_band < RFLM_GSM_NUM_GSM_BANDS; tx_band++ )
  {
    /* Get the band-specific Tx NV ptr */
    rfgsm_temp_tx_nv_tbl = rfgsm_nv_get_tbl_ptr(core_data->rfm_dev, (rfcom_gsm_band_type)tx_band, RFGSM_NV_TX);

    if ( (rfgsm_temp_tx_nv_tbl != NULL) && (rfgsm_temp_tx_nv_tbl->tx_static_nv_data_ptr != NULL) )
    {
      /* Get NV ramp data address */
      polar_ramp_ptr = &rfgsm_temp_tx_nv_tbl->tx_static_nv_data_ptr->polar_paramp_lut;

      /* Get band-specific master core DM ramp tables */
      master_ramp_up_tbl_ptr = rfgsm_core_dm_get_master_pa_ramp_up_table_addr( core_data->sub_id, tx_band,core_data->rfgsm_core_handle_ptr );
      master_ramp_down_tbl_ptr = rfgsm_core_dm_get_master_pa_ramp_down_table_addr( core_data->sub_id, tx_band,core_data->rfgsm_core_handle_ptr );
      RF_NULL_CHECK_RTN( master_ramp_up_tbl_ptr, "RF GSM init ramps master_ramp_up_tbl_ptr is NULL!", FALSE );
      RF_NULL_CHECK_RTN( master_ramp_down_tbl_ptr, "RF GSM init ramps master_ramp_down_tbl_ptr is NULL!", FALSE );

      #ifdef FEATURE_BOLT_MODEM
      rfgsm_mdsp_upsample_pa_ramp(polar_ramp_ptr->ramp_up, rfgsm_pa_ramp_up_76p8, RFGSM_MDSP_SCALE_3P25_TO_4P8MHZ);
      rfgsm_mdsp_upsample_pa_ramp(polar_ramp_ptr->ramp_down, rfgsm_pa_ramp_down_76p8, RFGSM_MDSP_SCALE_3P25_TO_4P8MHZ);
      #else
      memscpy(rfgsm_pa_ramp_up_76p8,sizeof(rfgsm_pa_ramp_up_76p8),polar_ramp_ptr->ramp_up,sizeof(rfgsm_pa_ramp_up_76p8));
      memscpy(rfgsm_pa_ramp_down_76p8,sizeof(rfgsm_pa_ramp_down_76p8),polar_ramp_ptr->ramp_down,sizeof(rfgsm_pa_ramp_down_76p8));
      #endif


      /* Copy the master NV ramp tables to master core DM table memory */
      memscpy( master_ramp_up_tbl_ptr,
               sizeof(temp_ramps_var.pa_ramp_up_table),
               rfgsm_pa_ramp_up_76p8,
               sizeof(rfgsm_pa_ramp_up_76p8) );

      memscpy( master_ramp_down_tbl_ptr,
               sizeof(temp_ramps_var.pa_ramp_down_table),
               rfgsm_pa_ramp_down_76p8,
               sizeof(rfgsm_pa_ramp_down_76p8) );
      ret_val = TRUE;
    }
  }

  return ret_val;
}



/*---------------------------------------------------------------------------*/
/*! @brief This function scales the ramps                                    */
/*! @details This function scales the band-specific tables based on scale    */
/*!          num_pa_ramp_table_entries can be:                               */
/*!          24=DIME52MHz, 26=BOLT57p6MHz, or 36=BOLT76p8MHz                 */
/*! @param rfm_dev - which RFM device to program event for                   */
/*! @param tx_slot - which Tx slot's ramp tables to scale                    */
/*! @param pa_scale - scale factor                                           */
/*! @param ramp_table_entries - 24=DIME52MHz, 26=BOLT57p6MHz, 36=BOLT76p8MHz */
/*! @retval boolean - success status                                         */
#define RFGSM_SCALE_FACTOR_SHIFT 15
#define RFGSM_SATURATE(val, mn,mx) MIN(mx, MAX((val),(mn)) )

boolean rfgsm_core_scale_tx_pa_ramp_data( rfm_device_enum_type rfm_dev,
                                          uint8 sub_id,
                                          uint8 tx_slot,
                                          uint16 pa_scale )
{
  /* Initialise local variables */
  uint16 *ramp_up_tbl_ptr = NULL;
  uint16 *ramp_down_tbl_ptr = NULL;
  uint16 *master_ramp_up_tbl_ptr = NULL;
  uint16 *master_ramp_down_tbl_ptr = NULL;
  uint32 pa_ramp_up_lut_entry;
  uint32 pa_ramp_down_lut_entry;
  uint32 index;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;

  /* Get the device-specific core handle data */
  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  /* Check the core handle pointer is not NULL for rfm_dev ID */
  RF_NULL_CHECK_RTN(rfgsm_core_handle_ptr, 
                    "rfgsm_core_set_tx_ramp_data core handle is NULL!", 
                    FALSE);

  /* Check the Tx slot number is valid */
  if ( tx_slot >= RFGSM_MAX_TX_SLOTS_PER_FRAME )
  {
    RFGC_MSG_1(MSG_LEGACY_ERROR,"rfgsm_core_scale_tx_ramp_data Tx slot val %d invalid", tx_slot);
    return FALSE;
  }

  /* Get the master PA ramp table pointers to GSM core DM memory */
  master_ramp_up_tbl_ptr = rfgsm_core_dm_get_master_pa_ramp_up_table_addr( sub_id, (rflm_gsm_band_type)rfgsm_core_handle_ptr->tx_band,rfgsm_core_handle_ptr );
  master_ramp_down_tbl_ptr = rfgsm_core_dm_get_master_pa_ramp_down_table_addr( sub_id, (rflm_gsm_band_type)rfgsm_core_handle_ptr->tx_band,rfgsm_core_handle_ptr );

  RF_NULL_CHECK_RTN(master_ramp_up_tbl_ptr, "master_ramp_up_tbl_ptr is NULL!", FALSE);
  RF_NULL_CHECK_RTN(master_ramp_down_tbl_ptr, "master_ramp_down_tbl_ptr is NULL!", FALSE);

  /* Get the PA ramp table pointers to GSM core DM memory */
  ramp_up_tbl_ptr = rfgsm_core_dm_get_pa_ramp_up_table_addr( sub_id, rfgsm_core_handle_ptr->pa_ramp_buffer_id, tx_slot,rfgsm_core_handle_ptr );
  ramp_down_tbl_ptr = rfgsm_core_dm_get_pa_ramp_down_table_addr( sub_id, rfgsm_core_handle_ptr->pa_ramp_buffer_id, tx_slot,rfgsm_core_handle_ptr );

  RF_NULL_CHECK_RTN(ramp_up_tbl_ptr, "ramp_up_tbl_ptr is NULL!", FALSE);
  RF_NULL_CHECK_RTN(ramp_down_tbl_ptr, "ramp_down_tbl_ptr is NULL!", FALSE);

  rfgsm_core_handle_ptr->num_pa_ramp_table_entries = RFLM_GSM_MAX_NUM_RAMP_SAMPLES;
  /* Check the size of the ramp is not bigger than the ramp in GSM core DM */
  if( rfgsm_core_handle_ptr->num_pa_ramp_table_entries > RFLM_GSM_MAX_NUM_RAMP_SAMPLES )
  {
    RFGC_MSG_1(MSG_LEGACY_ERROR,"rfgsm_core_scale_tx_ramp_data number or ramp entries %d to large", 
               rfgsm_core_handle_ptr->num_pa_ramp_table_entries);
    return FALSE;
  }

  /* Overwrite the GSM core DM ramp entries with the scaled values */
  for( index = 0; index < rfgsm_core_handle_ptr->num_pa_ramp_table_entries; index++ )
  {
    /* Assign temp var with scaled master ramp entry for up and down table */
    pa_ramp_up_lut_entry = ( pa_scale * ((uint32)master_ramp_up_tbl_ptr[index]) );
    pa_ramp_down_lut_entry = ( pa_scale * ((uint32)master_ramp_down_tbl_ptr[index]) );

    /* Shift the value back by unity to get the scaled ramp values */
    pa_ramp_up_lut_entry >>= RFGSM_SCALE_FACTOR_SHIFT;
    pa_ramp_down_lut_entry >>= RFGSM_SCALE_FACTOR_SHIFT;

    /* Ensure the ramp entry is not greater than the maximum allowed value or less than the minimum */
    ramp_up_tbl_ptr[index] = (uint16)RFGSM_SATURATE( pa_ramp_up_lut_entry,
                                                     rfgsm_core_handle_ptr->pa_ramp_saturation_low, 
                                                     rfgsm_core_handle_ptr->pa_ramp_saturation_high);
    ramp_down_tbl_ptr[index] = (uint16)RFGSM_SATURATE( pa_ramp_down_lut_entry,
                                                       rfgsm_core_handle_ptr->pa_ramp_saturation_low, 
                                                       rfgsm_core_handle_ptr->pa_ramp_saturation_high);
  }

  /* NOTE: Fractional interpolation due to unmatched samples/rates drift is  */
  /*       executed in FW context in RFLM as this may require knowledge from */
  /*       FW.                                                               */

  return TRUE;
}



/*---------------------------------------------------------------------------*/
/*! @brief This function switches the ping-pong buffer for Tx pa ramps       */
/*! @details When the ramp is programmed the ping-pong id needs switching to */
/*!          ensure that the next frame's programming is happening on the    */
/*!          other ramp table memory to prevent read/write collisions        */
/*! @param rfm_dev - which RFM device to program event for                   */
/*! @retval boolean - success status                                         */
boolean rfgsm_core_tx_pa_ramp_data_buffer_switch( rfm_device_enum_type rfm_dev )
{
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;

  /* Get the device-specific core handle data */
  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  /* Check the core handle pointer is not NULL for rfm_dev ID */
  RF_NULL_CHECK_RTN(rfgsm_core_handle_ptr, 
                    "rfgsm_core_tx_ramp_data_buffer_switch core handle is NULL!", 
                    FALSE);

  /* Switch buffers for next iteration */
  /* PA ramp ping-pong'ing needs to be handled by core handle to ensure seperate ping-pong ID per device */
  rfgsm_core_handle_ptr->pa_ramp_buffer_id = 
     ( rfgsm_core_handle_ptr->pa_ramp_buffer_id == RFGSM_CORE_DOUBLE_BUF0 ) ? RFGSM_CORE_DOUBLE_BUF1 : RFGSM_CORE_DOUBLE_BUF0;

  return TRUE;
}

/*---------------------------------------------------------------------------*/
/*! @brief This function sets the ping-pong buffer for Tx pa ramps           */
/*! @details During Tx calibration we force the PA ramp buffer to use        */
/*!          ping-pong buffer index 0                                        */
/*! @param rfm_dev - which RFM device to program event for                   */
/*! @param buffer_index - Which ping-pong buffer index to use                */
/*! @retval boolean - success status                                         */

boolean rfgsm_core_tx_set_pa_ramp_data_buffer( rfm_device_enum_type rfm_dev, uint8 buffer_index )
{
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;

  /* Get the device-specific core handle data */
  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  /* Check the core handle pointer is not NULL for rfm_dev ID */
  RF_NULL_CHECK_RTN(rfgsm_core_handle_ptr, 
                    "rfgsm_core_tx_ramp_data_buffer_switch core handle is NULL!", 
                    FALSE);

  /* Switch buffers for next iteration */
  /* PA ramp ping-pong'ing needs to be handled by core handle to ensure seperate ping-pong ID per device */
  rfgsm_core_handle_ptr->pa_ramp_buffer_id = buffer_index;

  return TRUE;
}

/*---------------------------------------------------------------------------*/
/*! @brief This function switches the ping-pong buffer for AMAM and AMPM tables */
/*! @details When the amam and ampm tables are programmed the ping-pong id      */ 
/*!           needs switching to                                                */
/*!          ensure that the next frame's programming is happening on the       */
/*!          other amam/ampm table memory to prevent read/write collisions           */
/*! @param rfm_dev - which RFM device to program event for                      */
/*! @retval boolean - success status                                            */
boolean rfgsm_core_tx_amam_ampm_data_buffer_switch( rfm_device_enum_type rfm_dev )
{
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;

  /* Get the device-specific core handle data */
  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  /* Check the core handle pointer is not NULL for rfm_dev ID */
  RF_NULL_CHECK_RTN(rfgsm_core_handle_ptr, 
                    "rfgsm_core_tx_ramp_data_buffer_switch core handle is NULL!", 
                    FALSE);

  /* Switch buffers for next iteration */
  /* AMAM AMPM ramp ping-pong'ing needs to be handled by core handle to ensure seperate ping-pong ID per device */
  rfgsm_core_handle_ptr->amam_ampm_buffer_id = 
     ( rfgsm_core_handle_ptr->amam_ampm_buffer_id == RFGSM_CORE_DOUBLE_BUF0 ) ? RFGSM_CORE_DOUBLE_BUF1 : RFGSM_CORE_DOUBLE_BUF0;

   //RFGC_MSG_1(MSG_LEGACY_ERROR,"AMAM AMPM Ping-Pong buffer switched. New ID = %d", rfgsm_core_handle_ptr->amam_ampm_buffer_id);

  return TRUE;
}

/*----------------------------------------------------------------------------*/
/*!
   @fn rfgsm_core_set_tx_transition_bufferr 
   @brief This function sets the TX transition buffer to TXAGC trigger scripts based on modulation type
   @param  rfm_dev - RFM device 
   @param  tx_slot - TX slot
   @param  mod_type - Modulation type 
   @retval None
*/
void rfgsm_core_set_tx_transition_buffer( rfm_device_enum_type rfm_dev,
                                          uint8 tx_slot,
                                          rfgsm_modulation_type mod_type )
{

  rfgsm_core_handle_type *rfgsm_core_handle_ptr;

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  RF_NULL_CHECK_RTN_V(rfgsm_core_handle_ptr,"rfgsm_core_handle_ptr is NULL!");

  /*update the TX Trans buffer with approroate TXAGC trigger based modulation*/
  if (mod_type== RF_MOD_GMSK)
  {
    rfgsm_core_handle_ptr->scripts.tx_trans_script[TX_TRANS_WTR_SCRIPT][tx_slot] = rfgsm_core_handle_ptr->scripts.tx_agc_trigger_gmsk_script[tx_slot];
  }
  else
  {
    rfgsm_core_handle_ptr->scripts.tx_trans_script[TX_TRANS_WTR_SCRIPT][tx_slot] = rfgsm_core_handle_ptr->scripts.tx_agc_trigger_8psk_script[tx_slot];
  }

}

/*----------------------------------------------------------------------------*/
/*!
  @details
  This function clears the WTR TX burst scripts
 
  @param rfm_dev : rfm device
 
  @retval True/False
*/

boolean rfgsm_core_clear_tx_burst_device_scripts( rfgsm_core_handle_type *rfgsm_core_handle_ptr)
{
  int i=0;
  if( !rf_buffer_clear_script( (rf_buffer_intf *)rfgsm_core_handle_ptr->scripts.tx_start_script[TX_WTR_SCRIPT], RF_BUFFER_ALL)||
      !rf_buffer_clear_script( (rf_buffer_intf *)rfgsm_core_handle_ptr->scripts.tx_stop_script[TX_WTR_SCRIPT], RF_BUFFER_ALL) )
  {
    return FALSE;
  }
  for ( i=0; i < RFGSM_MAX_TX_SLOTS_PER_FRAME; i++)
  {
    rf_buffer_clear_script((rf_buffer_intf *)rfgsm_core_handle_ptr->scripts.tx_trans_script[TX_TRANS_WTR_SCRIPT][i], RF_BUFFER_ALL);
  }
  return TRUE;
}

/*----------------------------------------------------------------------------*/
/*!
   @fn rfgsm_core_populate_tx_burst_device_buffers 
   @brief This function retrieves wtr device driver scripts for GSM Burst and stores them in rfgsm_core_handles pointer
   @param  rfm_dev - RFM device processing for
   @param  burst - TX
   @retval boolean - success/failure indication
*/

boolean rfgsm_core_populate_tx_burst_device_buffers( rfgsm_core_data_type *core_data,uint8 num_tx_slots, rfdevice_trx_gsm_env_delay_type *env_delay)
{

  uint16 buffer_mask;
  void* script_dm_ptr = NULL;
  #ifdef FEATURE_TX_SHARING
  uint16 temp_tx_num_trans = 0;
  #endif

  boolean build_trans_script = TRUE;
  uint8 tx_slot_index = 0;
  uint64 final_lo_freq_hz=0;
  int64 freq_error=0;
  rfdevice_trx_timer_fixup_type trx_timer_fixup_array[RFDEVICE_TRX_MAX_TIMER_FIXUPS_PER_SCRIPT];
  uint8 timer_fixup_arr_size = 0;
  /* Initialise variables */
  rflm_gsm_data_type *table_ptr = NULL;

  /* Get the data table pointer from DM */
  table_ptr = rflm_gsm_dm_get_table_ptr( core_data->sub_id );
  if( table_ptr == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, 
          "RF GSM rfgsm_core_populate_tx_burst_device_buffers for sub %d",
          core_data->sub_id );
    return FALSE;
  }

  

  final_lo_freq_hz =  (uint64)rfgsm_core_convert_arfcn_to_tx_freq(core_data->rfgsm_core_handle_ptr->tx_band, core_data->rfgsm_core_handle_ptr->rf_tx_arfcn );
  freq_error=core_data->rfgsm_core_handle_ptr->rfgsm_curr_freq_error_tx;
  freq_error = freq_error*(final_lo_freq_hz >> 15);
  freq_error = freq_error/31250;

  rfgsm_core_dm_set_freq_error( core_data->sub_id, (int16)freq_error,core_data->rfgsm_core_handle_ptr );

  if(!rfdevice_gsm_tune_tx(core_data->rfm_dev,
                           core_data->rfgsm_core_handle_ptr->tx_band,
                           core_data->rfgsm_core_handle_ptr->rf_tx_arfcn,
                           freq_error,
                           (rf_buffer_intf*)core_data->rfgsm_core_handle_ptr->scripts.tx_start_script[TX_WTR_SCRIPT],
                           &trx_timer_fixup_array,
                           &timer_fixup_arr_size)
                           )
  {
    RF_MSG_1( MSG_LEGACY_HIGH,"Building RF device script failed for TX start script",0);
  }


  for (tx_slot_index=0;tx_slot_index<num_tx_slots; tx_slot_index++)
  {
     script_dm_ptr = rfgsm_core_get_dm_script_ptr( core_data->sub_id,
                                                   RFLM_GSM_TX_TRANS_SCRIPT,
                                                   core_data->triple_buffer_id,
                                                   tx_slot_index, 
                                                   RFLM_GSM_CCS_TASK_Q_A );

     if (script_dm_ptr == NULL) {
        RF_MSG_1( MSG_LEGACY_HIGH,"DM script ptr is NULL for TX trans Script for slot%d",tx_slot_index);
        build_trans_script = FALSE;
        break;
     }
  }
  if (build_trans_script == TRUE)
  {
    rfdevice_gsm_set_rgi(  core_data->rfm_dev, 
                           core_data->rfgsm_core_handle_ptr->tx_band,
                           core_data->rfgsm_core_handle_ptr->rgi_slot,
                           core_data->rfgsm_core_handle_ptr->last_mod_type,
                           (rf_buffer_intf **)core_data->rfgsm_core_handle_ptr->scripts.tx_trans_script[TX_TRANS_WTR_SCRIPT],
                           num_tx_slots, /* num_slots */
                           TRUE,
                           core_data->rfgsm_core_handle_ptr->rf_tx_arfcn,
                           &trx_timer_fixup_array,
                           &timer_fixup_arr_size,
                           RFDEVICE_TRX_RX_WXE_BUF_0);  /* Update the device driver burst script */
  }
if( !rfdevice_gsm_tx_disable( core_data->rfm_dev,
                               core_data->rfgsm_core_handle_ptr->tx_band,
                              (rf_buffer_intf*)core_data->rfgsm_core_handle_ptr->scripts.tx_stop_script[TX_WTR_SCRIPT]) )
  {
    RF_MSG_1( MSG_LEGACY_HIGH,"Building script for RF device failed for TX stop script",0);
  }

  /*Populating Device Status Read Event Scripts During Tx Burst */
  if(core_data->rfgsm_core_handle_ptr->device_config_to_read.params[RF_TX_BURST].enable)
  {
    rfgsm_core_populate_device_status_read_buffers(core_data, RF_TX_BURST);
  }
  
  rfgsm_core_log(core_data->rfm_dev);

  return TRUE;
} /* rfgsm_core_populate_device_tx_burst_buffer */

/*----------------------------------------------------------------------------*/

/*!
  @brief update tx_alpha mdsp timing parameters.

  @details

  @param 

*/


boolean rfgsm_core_update_tx_timing(rfgsm_core_data_type *core_data)
  {
   uint16 tx_alpha = 0;
   uint32 num_gb = 0;
   uint32 num_pll_trans = 0; //absorbed in clock recovery sequence
   int32 tx_start_delta = 0;
   rfgsm_core_handle_type * rfgsm_core_handle_ptr = NULL;
   rfc_gsm_tx_timing_info_type * rfgsm_core_tx_timing = NULL;
   rfc_gsm_core_config_type * rfc_gsm_core_ptr = NULL;
   
   int tx_tune_exec_time =0,tx_rgi_exec_time=0;
   int tx_tune_trans =0,tx_rgi_trans=0;
   rf_buffer_intf *dummy_script_tx = NULL ;
   int i;
   rf_buffer_intf *dummy_script_tx_trans[TX_TRANS_MAX_SCRIPT][RFGSM_MAX_TX_SLOTS_PER_FRAME];
   rfdevice_trx_timer_fixup_type trx_timer_fixup_array[RFDEVICE_TRX_MAX_TIMER_FIXUPS_PER_SCRIPT];
   uint8 timer_fixup_arr_size = 0;
   rfgsm_modulation_type mod_type[RFDEVICE_TRX_GSM_MAX_TX_SLOTS];
   
   rfdevice_trx_gsm_env_delay_type env_delay;
   /* Get the core handle and RFC data */
   rfgsm_core_handle_ptr = rfgsm_core_handle_get( core_data->rfm_dev );
   RF_NULL_CHECK_RTN(rfgsm_core_handle_ptr, "rfgsm_core_tx_wakeup_timing_update rfgsm_core_handle_ptr is NULL!", FALSE);
   RF_NULL_CHECK_RTN(rfgsm_core_handle_ptr->rfc_gsm_core_ptr, "rfgsm_core_tx_wakeup_timing_update rfc_gsm_core_ptr is NULL!", FALSE);
 
   /* Get the tx timing data */
   rfgsm_core_tx_timing = rfgsm_core_handle_ptr->rfc_gsm_core_ptr->tx_timing;
   RF_NULL_CHECK_RTN(rfgsm_core_handle_ptr->rfc_gsm_core_ptr->tx_timing, "rfgsm_core_tx_wakeup_timing_update tx_timing is NULL!", FALSE);
 
		 rfgsm_core_handle_ptr->rgi_slot[0] = 15;
		 for (i=0; i < RFGSM_MAX_TX_SLOTS_PER_FRAME; i++)
		 {
		   dummy_script_tx_trans[TX_TRANS_WTR_SCRIPT][i] =NULL; 
		   dummy_script_tx_trans[TX_TRANS_FE_SCRIPT][i] =NULL; 
  }
		 mod_type[0]=RF_MOD_GMSK;
		 
		 if (dummy_script_tx == NULL)
  {
		   dummy_script_tx = rf_buffer_create_tech_scaling(0, 
		                             40, 
		                             20, /*GRFCs transactions are very small*/
		                             50,
		                             400,
		                             NUM_QS_PER_100_USEC);
  }
		 if(dummy_script_tx_trans[TX_TRANS_WTR_SCRIPT][0] ==NULL)
		 {
		   dummy_script_tx_trans[TX_TRANS_WTR_SCRIPT][0] = rf_buffer_create_tech_scaling(0, 
		                                                   10,
		                                                   10,
		                                                   50,
		                                                   50,
		                                                   NUM_QS_PER_100_USEC);
 
		 }
   rf_buffer_clear_script((rf_buffer_intf *)dummy_script_tx,RF_BUFFER_ALL);
   rf_buffer_clear_script((rf_buffer_intf *)dummy_script_tx_trans[TX_TRANS_WTR_SCRIPT][0],RF_BUFFER_ALL);

	  rfdevice_gsm_tune_tx( core_data->rfm_dev ,
                       RFCOM_BAND_GSM850,
                       128,
                       0,
                       (rf_buffer_intf*)dummy_script_tx,
                       &trx_timer_fixup_array,
                       &timer_fixup_arr_size);

   rf_buffer_get_script_execution_time(dummy_script_tx, RF_BUFFER_RFFE_GROUP,(uint32*) &tx_tune_exec_time);
   tx_tune_trans = rf_buffer_get_num_trans(dummy_script_tx,RF_BUFFER_RFFE_GROUP);

   MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfdevice_gsm_tune_tx, trans: %d: tx_tune_exec_time:%d " ,tx_tune_trans,tx_tune_exec_time);
   
   rfdevice_gsm_set_rgi( core_data->rfm_dev,
  	                     RFCOM_BAND_GSM850,
  	                     rfgsm_core_handle_ptr->rgi_slot,
  	                     mod_type,
  	                     (rf_buffer_intf**)dummy_script_tx_trans[TX_TRANS_WTR_SCRIPT],
                        1,  /* num_slots */
                        TRUE,
                        128,
                        &trx_timer_fixup_array,
                        &timer_fixup_arr_size,
                        RFDEVICE_TRX_RX_WXE_BUF_0);

   rf_buffer_get_script_execution_time(dummy_script_tx_trans[TX_TRANS_WTR_SCRIPT][0], RF_BUFFER_RFFE_GROUP,(uint32*) &tx_rgi_exec_time);  
 
   tx_rgi_trans = rf_buffer_get_num_trans(dummy_script_tx_trans[TX_TRANS_WTR_SCRIPT][0],RF_BUFFER_RFFE_GROUP);
 
   MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfdevice_gsm_set_rgi %d! trans %d tx_rgi_exec_time", tx_rgi_trans,tx_rgi_exec_time);
 
   //TODOMANI
	 /* Ensure that the number of guard bits is no larger than RFGSM_MAX_GUARD_BITS */
	// num_gb = dpll_seq.num_guard_bits > RFGSM_MAX_DPLL_GUARD_BITS ? RFGSM_MAX_DPLL_GUARD_BITS : dpll_seq.num_guard_bits;
    num_gb = RFGSM_MAX_DPLL_GUARD_BITS;

   //END
   rfdevice_trx_gsm_tx_get_timing_info(core_data->rfm_dev, &rfgsm_core_handle_ptr->tx_timing_info);
   /* Calculate the Tx start delta */
   tx_start_delta = rfgsm_core_handle_ptr->tx_timing_info.tune_fixup_rel_qs;

   /* Set the Tx alpha */
   tx_alpha = (uint16)(-tx_start_delta) + RFGSM_US_TO_QS(tx_tune_exec_time);

   MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,"tx_alpha %d! trans %d tx_start_delta", tx_alpha,tx_start_delta);

   // To enable KV frame for the duration of PLL lock time, to support any KV CAL sequence, WTR doesnt need zeros fed from DTR
   /* Update RFC copies of the TX timing info */
   //TODOMANI
   //rfgsm_core_tx_timing->kv_length = 0; 
   rfgsm_core_tx_timing->kv_length=(uint16)num_gb;
   rfgsm_core_tx_timing->num_pll_sbis = (uint16)num_pll_trans;
   rfgsm_core_tx_timing->tx_sbi_start_delta_qs = (int)tx_start_delta;
   rfgsm_core_tx_timing->tx_alpha_qs = (int)tx_alpha;
   /* Program the KV data into RFLM DM */
   if( !rfgsm_core_dm_set_kv_data( core_data->sub_id, num_gb,core_data->rfgsm_core_handle_ptr ) )
   {
     RF_MSG_2(MSG_LEGACY_ERROR, "Tx KV data NOT updated for sub %d, device %d", core_data->sub_id, core_data->rfm_dev);
     return FALSE;
   }
 
   /* Write Tx alpha value to RFLM DM */
   if( !rfgsm_core_dm_set_tx_alpha( core_data->sub_id, tx_alpha,core_data->rfgsm_core_handle_ptr ) )
   {
     RF_MSG_2(MSG_LEGACY_ERROR, "Tx timings are NOT updated for sub %d, device %d", core_data->sub_id, core_data->rfm_dev);
     return FALSE;
   }
 
   rfgsm_core_dm_set_tx_timing_fixup(core_data->sub_id, 
                                     core_data->rfgsm_core_handle_ptr->tx_timing_info.tune_fixup_rel_qs,
                                     core_data->rfgsm_core_handle_ptr->tx_timing_info.disable_fixup_rel_qs,
                                     core_data->rfgsm_core_handle_ptr->tx_timing_info.agc_fixup_rel_qs,
                                     core_data->rfgsm_core_handle_ptr); 

  rf_buffer_delete(dummy_script_tx);
  
  for (i=0; i < RFGSM_MAX_TX_SLOTS_PER_FRAME; i++)
  {
    rf_buffer_delete(dummy_script_tx_trans[TX_TRANS_WTR_SCRIPT][i]);
    rf_buffer_delete(dummy_script_tx_trans[TX_TRANS_FE_SCRIPT][i]);
  }
  return TRUE;
  }




/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function initializes GSM TX Enter mode

  @details
  
  @todo add details.

*/
void rfgsm_core_enter_mode_tx_init(rfgsm_core_data_type *core_data)
{
  uint32 i;

  /* Initialise the master PA ramp tables in DM from NV */
  if( !rfgsm_core_init_master_pa_ramp_tables( core_data ) )
  {
    RF_MSG(MSG_LEGACY_ERROR, "RF GSM core DM init master PA ramp tables failed!");
  }

  /* Update the Tx timing data to WTR and RFLM DM */
  rfgsm_core_update_tx_timing(core_data);

  rfgsm_core_set_slot_override_flag(core_data->rfgsm_core_handle_ptr, FALSE);
}



/*----------------------------------------------------------------------------*/
/*!
   @fn rfgsm_core_update_linear_amam_ampm_profiles
   @brief Performs frequency and temperature compensation of amam and ampm tables
   @details
   @param
   @retval None
*/

void rfgsm_core_update_linear_amam_ampm_profiles(rfgsm_core_data_type *core_data)
{
  uint16* dm_amam_table_ptr = NULL; 
  uint16* dm_ampm_table_ptr = NULL;

  uint16* final_amam_table_ptr = NULL;
  uint16* final_ampm_table_ptr = NULL;

  /* Compute the freq comp AMAM/AMPM tables */
  rfgsm_mdsp_compute_linear_amam_freq_comp(core_data->rfm_dev, core_data->rfgsm_core_handle_ptr->tx_band, core_data->rfgsm_core_handle_ptr->rf_tx_arfcn);

  /* Add slope to AMAM table based on temp bin */
  rfgsm_mdsp_compute_linear_amam_temp_comp(core_data->rfm_dev, TRUE);

  /******** AMAM Table Data manager updates ********/
  /* Get AMAM table location in Data Manager */
  dm_amam_table_ptr = rfgsm_core_dm_get_amam_table_addr( core_data->sub_id, 
                                                         core_data->rfgsm_core_handle_ptr->amam_ampm_buffer_id,
                                                         core_data->rfgsm_core_handle_ptr);

  /* Get the Final AMAM Table location from rfgsm_mdsp layer table */
  final_amam_table_ptr = rfgsm_mdsp_get_final_amam_ampm_table( core_data->rfm_dev, AMAM_TABLE );

  /* Copy the contents of the final AMAM table location to memory space in Data manager */
  if ( (final_amam_table_ptr != NULL) && (dm_amam_table_ptr != NULL) )
     {
        memscpy (dm_amam_table_ptr,
                 RFLM_GSM_MAX_NUM_AMAM_SAMPLES*sizeof(uint16),
                 final_amam_table_ptr,
                 RFLM_GSM_MAX_NUM_AMAM_SAMPLES*(sizeof(uint16))); /* Table size is reduced by a factor of 4 */
     }
     else
     {
        RF_MSG_2(MSG_LEGACY_ERROR,"Final AMAM Table or DM AMAM Table ptr is NULL: 0x%x, 0x%x",final_amam_table_ptr,dm_amam_table_ptr);
        return;
     }

  /******* AMPM Table Data Manager updates *******/
  /* Get AMPM table location in Data Manager */
  dm_ampm_table_ptr = rfgsm_core_dm_get_ampm_table_addr( core_data->sub_id, 
                                                          core_data->rfgsm_core_handle_ptr->amam_ampm_buffer_id,
                                                          core_data->rfgsm_core_handle_ptr);

  /* Get the Final AMPM Table location from rfgsm_mdsp layer table */
  final_ampm_table_ptr = rfgsm_mdsp_get_final_amam_ampm_table( core_data->rfm_dev, AMPM_TABLE );

  /* Copy the contents of the final AMPM table location to memory space in Data manager */
  if ( (final_ampm_table_ptr != NULL) && (dm_ampm_table_ptr != NULL) )
     {
         memscpy (dm_ampm_table_ptr,
                 RFLM_GSM_MAX_NUM_AMPM_SAMPLES*sizeof(uint16),
                 final_ampm_table_ptr,
                 RFLM_GSM_MAX_NUM_AMAM_SAMPLES*(sizeof(uint16))); /* Table size is reduced by a factor of 4 */
     }
     else
     {
       RF_MSG_2(MSG_LEGACY_ERROR,"Final AMPM Table or DM AMPM Table ptr is NULL: 0x%x, 0x%x",final_ampm_table_ptr,dm_ampm_table_ptr);
        return;
     }

  return;
}

/*----------------------------------------------------------------------------*/
/*! 
   @fn rfgsm_core_update_linear_amam_temp_comp
   @brief Performs temperature compensation of amam table
   @details
   @param
   @retval None
*/

void rfgsm_core_update_linear_amam_temp_comp(rfgsm_core_data_type *core_data)
{
  uint16* dm_amam_table_ptr = NULL; 
  uint16* final_amam_table_ptr = NULL;

  /* Add slope to AMAM table based on temp bin */
  rfgsm_mdsp_compute_linear_amam_temp_comp(core_data->rfm_dev, TRUE);

  /******** AMAM Table Data manager updates ********/
  /* Get AMAM table location in Data Manager */
  dm_amam_table_ptr = rfgsm_core_dm_get_amam_table_addr( core_data->sub_id, 
                                                         core_data->rfgsm_core_handle_ptr->amam_ampm_buffer_id,
                                                         core_data->rfgsm_core_handle_ptr);

  /* Get the Final AMAM Table location from rfgsm_mdsp layer table */
  final_amam_table_ptr = rfgsm_mdsp_get_final_amam_ampm_table( core_data->rfm_dev, AMAM_TABLE );

  /* Copy the contents of the final AMAM table location to memory space in Data manager */
  if ( (final_amam_table_ptr != NULL) && (dm_amam_table_ptr != NULL) )
  {
     memscpy (dm_amam_table_ptr,
              RFLM_GSM_MAX_NUM_AMAM_SAMPLES*sizeof(uint16),
              final_amam_table_ptr,
              RFLM_GSM_MAX_NUM_AMAM_SAMPLES*(sizeof(uint16))); /* Table size is reduced by a factor of 4 */
  }
  else
  {
     RF_MSG_2(MSG_LEGACY_ERROR,"Final AMAM Table or DM AMAM Table ptr is NULL: 0x%x, 0x%x",final_amam_table_ptr,dm_amam_table_ptr);
     return;
  }

  return;
}
#ifdef TEST_FRAMEWORK
#error code not present
#endif
