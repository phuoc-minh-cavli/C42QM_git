#ifndef FTM_COMMAND_ID_H
#define FTM_COMMAND_ID_H

/*!
  @file
  Common FTM Command IDs

  @brief
  This module contains list of common FTM commands.
*/

/*==============================================================================

  Copyright (c) 2013 - 2023 Qualcomm Technologies, Inc.  All Rights Reserved.

  Qualcomm Technologies Proprietary and Confidential.

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/rffactory.mpss/1.0/intf/ftm_command_id.h#318 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
05/16/23   ytak    add FTM_DO_RFFE_CMN_DEBUG_CMD
03/13/23   ytak    add MASKED WRITE
02/28/23   chaoxu  add extractor mode updating cmd
02/17/23   xs      add RFFE validation FTM cmd id
01/17/23   jiaai   [FR83775]  support GSM RX V4 delayed response
12/19/22   rmk     Add new FTM command id for Kernel char override in xPT
12/09/21   lzh     add new get papm screen cmd
05/25/21   prab    [CR2952591] FTM command for microsleep
03/09/21   smd     fixed compile error
02/05/21   hz      add qlink internal FTM cmd support
11/13/20   fxd     [CR2817970]FTM command to support selftest customized crash
09/11/20   vm      [CR2770642]FTM command to crash phone in IRAT case
08/04/20   vm      [CR2747691]FTM command for CLPC disable/enable in FTM mode
06/16/20   bp      [CR2715986] FR63838:DSDA: Parallel/concurrent IRAT gaps support changes
01/16/19   krk     DBG framework to dynamically change update_sample_index and NSAMPLE
12/18/19   ritt    FR 60515: [MAV20][RFFTM] RFFE device scan support for Beamer
11/25/19   gz      add new diag cmd: FTM_SET/GET_TRM_TECH_ANT_PORT_STATE
10/23/19   sas     Adding new command id for spur-notch injection framework
10/17/19   pu      Add new FTM command ID for Reflog Debug feature
10/15/19   ani     CV2X Fast AGC FTM support
08/26/19   mz      add vswr mav diag cmd: FTM_GET_VSWR
08/13/19   dpk     FR57525: Return FBRx Meas Power
08/02/19   ddeepak Adding new command in FTM mode for Power Config
07/26/19   tik     Adding new commands in FTM to support TAU injection framework
05/27/19   kk      Add new FTM command for GSM RF DC Offset Cal
04/01/19   spa     Add gear switch as new command and keep BER for other targets
03/28/19   spa     Replace Qlink BER with gear switch test
03/15/19   zhuoxi  Add FTM CDRX Sleep cmd
01/11/19   am      [FR47255]:FTM mode support to trigger LPM switch
12/11/18   rj      Rx toggle support in ftm mode
11/30/17   aen     Add FTM Update RFC cal NV table
11/29/18   hxx     Add ENDC AMPR custom API
11/20/18   vs      Add FTM cmd to support scheduling of Tx info from Q6 to Modem IU
11/13/18   sn      Add XPT debug commands
09/27/18   ani     Add enable/disable cmd for LTE TxAGC EFS
08/16/18   Shirley Add FTM_RFC_DEBUG_COMMON_DISPATCH
06/22/18   zhh     Add the support to set/get GRFC status
04/10/18   rj      FTM support for Usleep
03/14/18   aen     Add internal NV tree write FTM cmd id
03/02/18   jmf     FTM support to set RFFE x1 clock debug mode
02/21/18   krk     New FTM cmd to support pllid for GSM cal
02/15/18   ap      CR2182759: Add FTM Command for LLC Write Tests
01/12/18   wxl     RFCal opt for LTE
11/22/17   mg      Add HANDOVER_ENH command to specify num Rx chains for handover
11/15/17    id     Add FTM command for querying ECM timestamp
10/23/17   nf      Added FTM_COMMON_RFDEVICE_NR5G_MMW_CONTROL custom GUI cmd ID
10/20/17   dpk     Add switch_band_settings cmd id
10/18/17   dyc     Added antsw cmd id
09/30/17   ahm     Enhance qsleep testing methodology
09/29/17   vv      Added FTM RFC signals control custom GUI command id
09/06/17   vv      Added FTM RFC custom GUI command IDs
08/22/17   zhw     Added support for PLL ID in ARA command
08/10/17   zhw     Added support for Tx MTPL override
07/18/17   tt      FR 37697: FTM NS LTE Downlink-only acquisition
04/26/17   shk     Added internal cmd id for ftm exit mode
03/31/17   spa     Support to read CDR codes
03/23/17   vv      Added RFC FTM command enums for beam info queries
02/27/17   qzh     Add LTE NB FTM RF mode support for Tx/Rx test.
02/27/17   qzh     Add LTE NB support for set NB mode and FS in FTM RF mode.
02/26/17   qzh     Add ftm command id related support for CATM1 FTM RF mode
02/24/17   qzh     Add NBIOT FTM NS support
02/13/17   aro     Addition NR5G radio commands
02/09/17   aro     NR5G radio commands
01/30/17   zhw     Added support for TCXOMGR therm read
01/20/17   ska     Conflict in the FTM command ID 1021 is fixed
01/11/17    vb     IIP2 cal single trigger FTM command
12/14/16   xsh     Add ftm_vswr_test
12/13/16   jz      Added live debug flag and PA_CTL/ASM_CTL override for TDS
12/08/16   smd     Added new FTM command IDs for eye plot infinite mode
12/06/16   zhw     Added FTM SET/GET Sawless state support
11/03/16   aa      Added FTM cmd to enable crash during therm read
10/24/16   smd     Added a new FTM command ID to get Qlink termination setting
10/21/16   krk     FR35886: Support different RFM Context/Path for different bands
10/17/16   mg      Added support for GET_DL_LEVEL_ENH command in FTM Non-Signaling.
10/10/16   stw     Sawless Control
10/10/16   mg      Added support for LTE Start Data Path Enh in FTM Non-Signaling
09/30/16   xsh     Add ftm_get_s11 ftm_msg_id
08/18/16   ljz     Add CW and Tx_freq overwrite FR
08/04/16   lm      Added FTM support for WLAN configure and de-configure
07/26/16   zhh     Added API to get unique path number
07/14/16   shk     Added a new FTM command ID for internal ASDIV switch.
07/06/16   sn      Add a new FTM command ID for internal NV write.
06/16/06   smd     Added FTM commands for QLINK
06/10/16   mg      Added a new command for RX chain select in FTM NS mode.
06/08/16   jfc     Add FTM_SET_TRM_ANT_PORT_STATE command
06/02/16   wy/qzh  Add a new command for setting HD FDD mode about CAT M1
05/16/16   sku     Added command for setting FTM NLIC mode
05/16/16   ck      Remove word
04/29/16   kma     Added a new command to query HW index
04/28/16   sbm     New XPT override command.
04/28/16   zhh     Diag Command to program XSW devices to specific settings
04/12/16   sn      Add support for Autodelay override
04/04/16   bsh     New ftm command to trigger asdiv reload LNA scripts
03/29/16   px      Added FTM_FEM_COMMON_DISPATCH for Front-End Module commands
03/26/16   zc      FTM GSM Set Ant Switch Config Command
03/24/16   aro     W CA info query command
03/21/16   Saul    [FTM_TRM] Antenna Path support.
03/16/16   Saul    [FTM_COMMON] Assigned Radio Allocations (ARA) API
03/08/16   ljz     Added FTM_WCDMA_RADIO_TX_CONFIGURE_DAC command
03/03/16   stl     [selftest] FBRX Gain Stage automated cal algorithm
03/01/16   ugs     Added new FTM command for reloading LNA scripts
03/02/16   hm      correcting the enum FTM_GET_TUNER_STATUS value by one
03/02/16   hm      increasing the enum FTM_GET_TUNER_STATUS value by one
02/24/16   sc      Added support for FTM_GET_UNIFIED_ENH_THERM_V2
02/18/16   zhh     Add FTM_INT_CALV3_DELTA_PIN_POUT, command
02/01/16   br      Update the FTM_STG_COMMON_CONTROL command ID.
01/27/16   aro     [1] Retune bandwidth Radio Test support
                   [2] Radio Test DS testing bug fix
01/08/16   mpt     added support for FTM_LTE_GET_HORXD_BAND_SUPPORT_STATUS command
12/16/15   kr      Add Multi cluster FTM support
12/15/15   spa     Support for 1x LNA override
12/15/15   st      LTE_FTM_RF: Configurable UL Duty Cycle Support
12/11/15   jr      RF TDSCDMA device status read
12/10/15   ars     Adding new FTM command to test NLIC functionality in FTM mode.
                   and to enable debug F3s
12/10/15   tks     Added support for rxagc timer enable/disable
12/08/15   ck      update FTM command id for fbrx gain cal
12/02/15   sml     Phase 2 device status read
12/01/15   ars     Adding FTM NLIC char support.
11/25/15   hzh     Added ftm_rfc_get_fem_directory_info for FEM EFS support
11/24/15   ck      Add FTM command id for fbrx gain cal
11/24/15   sc      Added support for FTM_GET_RFFE_DEVICE_INFO_V2
11/19/15   zbz     Added FTM command FTM_GSM_SET_NS_BAND
11/10/15   zc      FTM dispatch command to set Prx/Drx/Tx devices
10/15/15   br      Add FTM command ID for enabling STG
11/05/15   al      Add command FTM_COMMON_PRINT_MESSAGE to print out generic message
10/22/15   php     Add support for LTE 5DLCA FTM Test Framework
10/08/15   cd      Add support for XPT radio APIs
10/06/15   sb      Added new cmd to support device to carrier mapping for LTE
10/06/15   kma     Added new cmd to support autopin PAR algo
10/04/15   aro     Interface to unpack event script
10/01/15   awl     Added FTM case for one WTR read/write
09/28/15   ska     Adding support for DL active mask bitmask for NS ULCA
09/21/15   swl     Added Command ID for QLNK FTM test case
09/21/15    aa     Added suport for FBRx debug ftm command
08/19/15   ska     Adding support for ULCA Non-Signaling FTM Support
08/18/15   sn      Add FTM Command ID to get RF path information for Calibration
08/07/15   vv      Add FTM RFC command support get rfm_device and rx/tx rfm_path info
07/21/15   krg     Add FTM LTE cmd for enabling/disabling various debug flags
07/21/15   ck      add FTM GSM cmd to override QPOET mode and bias voltage
07/20/15   fhuo    Add FTM_DO_RFFE_MULTI_CMD=622 for RFFE multi-bytes(up to 16) read and write
07/15/15   zhw     Added FTM device manager Bypass mode support
07/15/15   cdb     Add support to override Autopin operation
06/24/15   st      ELNA Control Command
06/23/15   aro     AGC Log configure command
06/21/15   dbc     Added FTM_SET_PA_TUNABLE_CAP command ID
06/17/15   aro     Added commands for FTM W Tx radio test
06/16/15   yb      Added support for get tune code API
05/20/15   nv      Added command to log RXAGC during FTM IRAT testing
05/19/15   sdu     Added support for FTM_SET_PATH_SEL_VERIFICATION_INDEX command
05/19/15   br      Add new command  for FTM  Response Packet Compression
05/08/15   adk     Added FTM_GET_TUNER_CL_ALGO_STATE to query CL algo state
04/14/15   nv      Added new commands for half scripts
03/31/15   nv      Added new command for CRAT support
03/30/15   rp      Code changes to support FTM SET TX CARRIER INDEX command.
03/30/15   aro     WCDMA Tx deinit/retune commands
03/29/15   aro     Fed event execute command
03/29/15   aro     Radio Test command to override LNA state
03/26/15   kma     Added cmd to support CW tone generation
03/25/15   aro     Added WCDMA Radio tear down command
03/13/15   aro     Added WCDMA NS commands
03/13/15   aro     Added Debug error flush
03/12/15   ska     Added support to set DGLNA flag and attributes in FTM RF mode
03/10/15   aro     Added RX_INIT command for WCDMA Radio Test
03/09/15   ck      Add command to support ftm drds reconfiguration
02/23/15   al      Added command to get rffe device IDs with max device number attached
02/11/15    br     Added support for simultaneous PRx/DRx IQ capture.
01/29/15   kg      Added command to retrieve FBRX Sample Captures
01/21/15   lm      Added TX DAC msb cal code dump and reload command enum
12/30/14   jps     Add support for FTM_TDSCDMA_FBRX_SET_RX_TX_DELAY
12/19/14   ck      add ftm command to return nv container
12/12/14   stw     Add Multiple Scell Support in NS mode
12/09/14   ck      Add dispatch command to overwrite alt_path
12/10/14    ska     Added FTM API for NLIC characterization
12/10/14   zbz     Add command to return both GSM PRx DRx RSSI
11/24/14   ars     Chaning the command ID for enable/disable DRx chain API to 533.
11/13/14   ars     Added FTM API to enable/disable DRx chain
11/13/14   vbh     Add command for FTM RF scenario setup
11/10/14   kg      Add command to configure concurrency manager debug mode
10/30/14   zbz     Add command to return GSM DRx RSSI
10/29/14   stw     Added command FTM_QUERY_HW_PATH
10/24/14   kma     Add a command to override TX freq
10/14/14   jps     Add GET_STATIC_INFO command
10/13/14   al      Add support for FTM_TDSCDMA_SET_DEVICES
09/23/14   piy     Reading device status in between GSM Rx/Tx burst
09/18/14   ska     Add support for FTM_TXRX_AGC_RELOAD command
09/11/14   bsh     Support FTM EXIT command
08/15/14   st      Alt Path Override Command
08/11/14    aa     Update run continuous cmd to control fbrx enable/disable
08/06/14   aro     Support to override ILPC mode of operation
07/17/14  jmf     Add new command to Set Self Test Mode
07/10/14  sbo     Add new command to override TRM device mapping for CDMA
07/07/14  xsh     Add Internal commands for RSB cal
07/08/14   ec      Add commmand ids for SAWLess linearity mode control
05/15/14   aro     Command to override TxPlim override mode
05/16/14   cri     IQ capture segment reduction changes
04/24/14   gvn     S-LTE Support
04/16/14   kma     Added TDS FTM cmd to trigger front end port switch
04/15/14  ska     Add a new FTM command to get RFFE device settings
04/08/14   JJ      Add a new FTM command for timed sleep rx
03/26/14   sb      IP2 cal support for Dime PM
03/25/14   rmb     Add a new FTM command for get rxagc API during X2W measurements.
03/20/14   spa     Added FTM_CDMA_RADIO_FORCE_RX_ONOFF command
02/20/14   vbh     Add a command to get RxAGC info
02/13/14   ac      test send script in FTM cmd
02/12/14   ka      Added FBRx continuous run cmd
01/30/14   sdu/pv  Added FTM_SET_PATH_SEL_OVERRIDE_INDEX command
01/30/14   npi     Support for configuring/apply/reset LTE dedicated notches
01/29/14   sdu     Added FTM_LTE_RFFE_DEVICE_STATUS_UPDATE command
01/28/14   zhh     Update WCDMA Radio Test Interface
01/23/14   rp      Added code change to support FTM TDD CONFIG Command.
01/17/14   ka      Added FTM_FBRX_SET_PARAMS
01/10/14   npi     Added FTM cmd to disable an SCELL
01/08/14   kma     Added FTM cmd to return ET Pin compensation
12/16/13   ms      RF alternate path support
12/12/13   rmb     Add FTM commands for W2W IRAT
12/12/13   aro     W Radio command to enable or disable HDET
12/11/13   php     Added Get DL BLER support for all carriers
12/05/13   aj      Added FTM_FEEDBACK_SETUP command
12/02/13   php     Added RF-ML1 interface and support for NS CA
11/25/13    ka     Added fbrx store results
11/15/13   ck      Add FTM command ID to set PA BIAS NV items
11/13/13    ka     Added FTM fbrx common test framework
11/11/13   php     Added interface changes for NS CA
10/29/13   vs      Add FTM IDs to get/set DVGA offset for Multi carrier
10/28/13   sb      Add Support for APIs to set filtered path and override MCS control in SGLTE/SGTDS
10/25/13   jmf     Added FTM_RF_MEAS_FBRX_NOISE and FTM_DO_FBRX_RSB_CAL commands
10/08/13   aro     Updated copyright information
09/16/13   cvd     Added RXLM and TXLM commands
09/10/13   aa      Added FTM_WCDMA_RADIO_SET_RX_CAL_NBEE
09/09/13   sbm     Added FTM_CONFIG_TX_RX Command.
09/06/13   ars     Added FTM_GET_MULTIPLE_RX_LEVEL_DBM command
08/26/13    jr     Added FTM_GET_EFS_VERIFICATION_DATA
08/27/13   jf      Added interface for rxlm/txlm register dump
08/21/13    dj     Added FTM_INT_CALV3_FEEDBACK_SETUP command
08/19/13   spa     Added FTM_CDMA_ENTER_MODE command
08/13/13    sk     Added support for compressed ESC Cal Command
08/05/13   bsh     Added FTM_LTE_USE_FIXED_VECTOR cmd: LTE Tx fixed vector
08/05/13   ars/rmb New FTM command to exit currently selected FTM RF mode.
07/23/13   sdu     Added FTM_SET_ASM_PORT cmd
07/30/13   aro     WCDMA Radio command to Trigger AGC Log
07/29/13   aka     Added FTM_SET_FB_PATH_STATE cmd
07/15/12   ars     Added FTM interface for self-test APIs
07/16/13   aro     Added ASDIV FTM mode testing support
07/19/13   aro     Migrated Tuner self cal command ID
07/10/13   aro     Removed header file
05/25/13   aro     Migrated FTM command ID
05/25/13   aro     Initial Release

==============================================================================*/

#include "comdef.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*/
/*!
  @name Common Command IDs

  @brief
  This section includes common FTM command IDs
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Lists the FTM Radio Control Command IDs for Diag Interface.

  @image html ftm_cdma_rfm_command.jpg
*/
typedef enum
{
  /*--------------------------------------------------------------------------*/
  /* RFM common commands (0 - 4095) */
  /*--------------------------------------------------------------------------*/
  FTM_COMMON_RADIO_CMD_START       = 0,  /*!< 0: Start of Common Command */

  FTM_COMMON_SET_SAR_STATE                = 0, /*!< 0 : Set common SAR state */
  FTM_COMMON_EPT_IQ_CAPTURE               = 1, /*!< 1 : Unused */
  FTM_COMMON_SET_QFE_SCENARIO             = 2, /*!< 2 : Set common QFE Scenarior */
  FTM_COMMON_EPT_OVERRIDE                    = 3, /*!< 3 : Unused */
  FTM_COMMON_CONFIG_SLAVE_RES                = 4, /*!< 4 : Config Slave resource */
  FTM_COMMON_HW_TIMER_TEST                   = 5, /*!< 5 : Test Hardware Timer */
  FTM_COMMON_CALV3_DBG_LVL                   = 6, /*!< 6 : Set calv3 debug level */
  FTM_COMMON_SET_ANTENNA                     = 7, /*!< 7 : Set common antenna switch
                                               position*/
  FTM_COMMON_DRIVER_VER_SANDBOX              = 8, /*!< 8: Driver Verification sandbox */
  FTM_COMMON_RADIO_ENTER_MODE                = 9, /*!< 9: Generic command to enter a
                                                       tech in FTM >*/
  FTM_COMMON_FBRX_ILPC_OVERRIDE              = 10, /*!< 10: Override ILPC mode of operation >*/
  FTM_COMMON_RADIO_EXIT_MODE                 = 11, /*!< 11: Generic command to exit a
                                                       tech in FTM >*/
  FTM_COMMON_AUTOPIN_OVERRIDE                = 12, /*!< 12: Override AutoPin operation >*/
  FTM_COMMON_RFM_DEVICE_TBL                  = 13, /*!< 13: Get RFM_DEVICE mapping table >*/
  FTM_COMMON_RX_RFM_PATH_INFO                = 14, /*!< 14: Get RX RFM PATH info for given band & tech >*/
  FTM_COMMON_TX_RFM_PATH_INFO                = 15, /*!< 15: Get TX RFM PATH info for given band & tech >*>*/
  FTM_COMMON_RADIO_XPT_LOAD_VDPD_UNITY       = 16, /*!< 16: Load unity Volterra LUT >*/
  FTM_COMMON_RADIO_XPT_LOAD_VDPD_MEM_LESS    = 17, /*!< 17: Load memoryless Volterra LUT >*/
  FTM_COMMON_RADIO_XPT_LOAD_VDPD_KERNELS     = 18, /*!< 18: Load Volterra kernels */
  FTM_COMMON_RADIO_XPT_MULTI_CAPTURE         = 19, /*!< 19: Trigger multi capture sequence for XPT */
  FTM_COMMON_RADIO_XPT_DEBUG_OVERRIDE        = 20, /*!< 20: XPT Debug params override */
  FTM_COMMON_FEM_DIRECTORY_INFO              = 21, /*!< 21: Get FEM EFS directory info from RFC */
  FTM_COMMON_AUTODELAY_OVERRIDE              = 22, /*!< 22: 0verride AutoDelay operation */
  FTM_COMMON_RFC_PROGRAM_ANT_SW_PATH         = 23, /*!< 23: Program XSW to specific settings */
  FTM_COMMON_RFC_GET_UNIQUE_PATH_NUM         = 24, /*!< 24: Get Unique path num based on Tech/Band/Sig Path */
  FTM_COMMON_RFC_GET_BAND_INFO_BASE_ON_PATH  = 25, /*!< 25: Get Unique band info based on unique path number */
  FTM_COMMON_RFC_CONFIGURE_WLAN              = 26, /*!< 26: Configure WLAN based on rfm_context */
  FTM_COMMON_RFC_DECONFIGURE_WLAN            = 27, /*!< 27: Configure WLAN based on rfm_context */

  FTM_COMMON_RFC_GET_STATIC_BEAM_LIST        = 28, /*!< 28: Get list of all beams for a given band */
  FTM_COMMON_RFC_GET_BEAM_INFO               = 29, /*!< 29: Get beam info for a given set of beams per band */
  FTM_COMMON_RFC_ALLOCATE_DYNAMIC_BEAM       = 30, /*!< 30: Allocate dynamic beam*/
  FTM_COMMON_RFC_DEALLOCATE_DYNAMIC_BEAM     = 31, /*!< 31: Deallocate dynamic beam*/
  FTM_COMMON_RFC_GET_DYNAMIC_BEAM_TABLE      = 32, /*!< 32: get dynamic beam table*/
  FTM_COMMON_RFC_GET_STATIC_BEAM_LIST_INFO   = 33, /*!< 33: Get beam list info for a given set of beams per band */
  FTM_COMMON_RFC_SIGNALS_CONTROL             = 34, /*!< 34: rfc signals control */
  FTM_COMMON_RFDEVICE_NR5G_MMW_CONTROL       = 35, /*!< 35: NR5G MMW RF Device control (e.g., read/write) */
  FTM_COMMON_RFC_TECH_CONFIGURE              = 36, /*!< 36: Configure tech based on rfm_context */
  FTM_COMMON_RFC_TECH_DECONFIGURE            = 37, /*!< 37: Configure tech based on rfm_context */
  FTM_COMMON_RFC_CMW_GET_GRFC_STATUS         = 38, /*!< 38: Get MDM or SDR GRFC Status*/
  FTM_COMMON_RFC_CMW_SET_GRFC_STATUS         = 39, /*!< 39: Get MDM or SDR GRFC Status*/
  FTM_COMMON_XPT_SNR_DBG                     = 40, /*!< 40: enable debug SNR assert condition*/
  FTM_COMMON_XPT_DELAYCAPT_DBG               = 41, /*!< 41: enable fase delay char assert condition*/
  FTM_COMMON_XPT_DELAY_CHAR_IQ_CAPTURE_SAVE  = 42, /*!< 42: enable capture saving for delay char in efs */
  FTM_COMMON_MCPM_RF_RESOURCE_CONTROL        = 43, /*!< 43: send MCPM REQ to bring up/down rf related resources */
  FTM_COMMON_XPT_DPD_KERNEL_INFO_OVERRIDE    = 44, /*!< 44: read and override Kernel information in xPT during tune */

  FTM_COMMON_RADIO_CMD_END         = 4095, /*!< 4095: End of Common Command */

  /*--------------------------------------------------------------------------*/
  /* RFM CDMA 1x commands (4096 - 8191) */
  /*--------------------------------------------------------------------------*/
  FTM_1X_RADIO_CMD_START           = 4096, /*!< 4096 : Start of  1x command*/

  FTM_1X_RADIO_ENTER               = 4096, /*!< 4096 : Enter 1x */
  FTM_1X_RADIO_EXIT                = 4097, /*!< 4097 : Exit 1x */
  FTM_1X_RADIO_ENABLE              = 4098, /*!< 4098 : Enable 1x Radio */
  FTM_1X_RADIO_DISABLE             = 4099, /*!< 4099 : Disable 1x Radio */
  FTM_1X_RADIO_WAKEUP_RX           = 4100, /*!< 4100 : Wakeup 1x Receiver */
  FTM_1X_RADIO_SLEEP_RX            = 4101, /*!< 4101 : Sleep 1x Receiver */
  FTM_1X_RADIO_WAKEUP_TX           = 4102, /*!< 4102 : Wakeup 1x Transmitter */
  FTM_1X_RADIO_SLEEP_TX            = 4103, /*!< 4103 : Sleep 1x Transmitter */
  FTM_1X_RADIO_RETUNE              = 4104, /*!< 4104 : Re-tune 1x Rx/Tx */
  FTM_1X_RADIO_ENABLE_DIV          = 4105, /*!< 4105 : Enable 1x Diversity */
  FTM_1X_RADIO_DISABLE_DIV         = 4106, /*!< 4106 : Disable 1x Diversity */
  FTM_1X_RADIO_GET_RX_AGC          = 4107, /*!< 4107 : Get 1x Rx AGC */
  FTM_1X_RADIO_GET_TX_AGC          = 4108, /*!< 4108 : Get 1x Rx AGC */
  FTM_1X_RADIO_SET_TX_POWER        = 4109, /*!< 4109 : Get 1x Rx AGC */
  FTM_1X_RADIO_CLEAR_LNA_OVERRIDE  = 4110, /*!< 4110 : Clear LNA Override */
  FTM_1X_RADIO_SET_TX_OVERRIDE     = 4111, /*!< 4111 : Set 1x Tx Override */
  FTM_1X_RADIO_SET_PM_OVERRIDE     = 4112, /*!< 4112 : Set PowerMode Override */
  FTM_1X_RADIO_CLEAR_PM_OVERRIDE   = 4113, /*!< 4113 : Clear PM Override */
  FTM_1X_RADIO_GET_PM              = 4114, /*!< 4114 : Get iC PM */
  FTM_1X_CAL_V3_CONFIG_RADIO       = 4115, /*!< 4115 : Config radio for Cal v3*/
  FTM_1X_CAL_V3_RETUNE_RADIO       = 4116, /*!< 4116 : Retune radio for Cal v3*/
  FTM_1X_CAL_V3_TEAR_DOWN_RADIO    = 4117, /*!< 4117 : Tear down RF for Cal v3*/
  FTM_1X_RADIO_TIMED_SLEEP         = 4118, /*!< 4118 : Schedule rx sleep */
  FTM_1X_RADIO_LNA_OVERRIDE        = 4119, /*!< 4119 : OVERRIDE LNA STATE */
  FTM_1X_RADIO_CMD_END             = 8191, /*!< 8191 : End of 1x command*/

  /*--------------------------------------------------------------------------*/
  /* RFM CDMA HDR commands (8192 - 12287) */
  /*--------------------------------------------------------------------------*/
  FTM_HDR_RADIO_CMD_START          = 8192, /*!< 8192 : Start of HDR command*/

  FTM_HDR_RADIO_ENTER              = 8192, /*!< 8192 : Enter HDR */
  FTM_HDR_RADIO_EXIT               = 8193, /*!< 8193 : Exit HDR */
  FTM_HDR_RADIO_ENABLE             = 8194, /*!< 8194 : Enable HDR Radio */
  FTM_HDR_RADIO_DISABLE            = 8195, /*!< 8195 : Disable HDR Radio */
  FTM_HDR_RADIO_WAKEUP_RX          = 8196, /*!< 8196 : Wakeup HDR Receiver */
  FTM_HDR_RADIO_SLEEP_RX           = 8197, /*!< 8197 : Sleep HDR Receiver */
  FTM_HDR_RADIO_WAKEUP_TX          = 8198, /*!< 8198 : Wakeup HDR Transmitter */
  FTM_HDR_RADIO_SLEEP_TX           = 8199, /*!< 8199 : Sleep HDR Transmitter */
  FTM_HDR_RADIO_RETUNE             = 8200, /*!< 8200 : Re-tune HDR Rx/Tx */
  FTM_HDR_RADIO_ENABLE_DIV         = 8201, /*!< 8201 : Enable HDR Diversity */
  FTM_HDR_RADIO_DISABLE_DIV        = 8202, /*!< 8202 : Disable HDR Diversity */
  FTM_HDR_RADIO_GET_RX_AGC         = 8203, /*!< 8203 : Get HDR Rx AGC */
  FTM_HDR_RADIO_GET_TX_AGC         = 8204, /*!< 8204 : Get HDR Rx AGC */
  FTM_HDR_RADIO_SET_TX_POWER       = 8205, /*!< 8205 : Get HDR Rx AGC */
  FTM_HDR_RADIO_LNA_OVERRIDE       = 8206, /*!< 8206 : Set LNA Override */
  FTM_HDR_RADIO_CLEAR_LNA_OVERRIDE = 8207, /*!< 8207 : Clear LNA Override */

  FTM_HDR_RADIO_CMD_END            = 12287, /*!< 12287 : End of HDR command*/

  /*--------------------------------------------------------------------------*/
  /* RFM CDMA Common commands (12288 - 16383) */
  /*--------------------------------------------------------------------------*/
  FTM_CDMA_RADIO_CMD_START         = 12288, /*!< 12288 : Start of CDMA command*/

  FTM_CDMA_RADIO_SET_PM_OVERRIDE   = 12288, /*!< 12288 : Set PM Override */
  FTM_CDMA_RADIO_CLEAR_PM_OVERRIDE = 12289, /*!< 12289 : Clear PM Override */
  FTM_CDMA_RADIO_GET_PM            = 12290, /*!< 12290 : Get iC PM */
  FTM_CDMA_DRIVER_VER_SANDBOX      = 12291, /*!< 12291 : Driver verification*/
  FTM_CDMA_CONFIG_DEBUG_MODE       = 12292, /*!< 12292 : Configure Debug Mode*/
  FTM_CDMA_CONFIG_IC_OPERATION     = 12293, /*!< 12293 : Configure iC Op*/
  FTM_CDMA_RADIO_FORCE_RX_ONOFF    = 12294, /*!< 12294 : Force Rx on/off */
  FTM_CDMA_RELOAD_LNA_SCRIPT       = 12295, /*!< 12295 : Reload LNA Script */
  FTM_CDMA_RADIO_CMD_END           = 16383, /*!< 16383 : End of CDMA command*/

  /*--------------------------------------------------------------------------*/
  /* RFM LTE commands (16384 - 20479) */
  /*--------------------------------------------------------------------------*/
  FTM_LTE_RADIO_CMD_START         = 16384, /*!< 16384 : Start of LTE command*/

  FTM_LTE_RADIO_CMD_END           = 20479, /*!< 20479 : End of LTE command*/

  /*--------------------------------------------------------------------------*/
  /* RFM GSM commands (20480 - 24575) */
  /*--------------------------------------------------------------------------*/
  FTM_GSM_RADIO_CMD_START         = 20480, /*!< 20480 : Start of GSM command*/

  FTM_GSM_RADIO_CMD_END           = 24575, /*!< 24575 : End of GSM command*/

  /*--------------------------------------------------------------------------*/
  /* RFM TD-CDMA commands (24576 - 28671) */
  /*--------------------------------------------------------------------------*/
  FTM_TDSCDMA_RADIO_CMD_START     = 24576, /*!< 24576 : Start of TDS cmd*/

  FTM_TDSCDMA_RADIO_CMD_END       = 28671, /*!< 28671 : End of TDS cmd*/

  /*--------------------------------------------------------------------------*/
  /* RFM WCDMA commands (28672 - 32767) */
  /*--------------------------------------------------------------------------*/
  FTM_WCDMA_RADIO_CMD_START       = 28672, /*!< 28672 : Start of WCDMA cmd*/

  FTM_WCDMA_RADIO_TRIGGER_AGC_LOG = 28672, /*!< 28672 : Trigger AGC Log */

  FTM_WCDMA_RADIO_SET_RX_CAL_NBEE = 28673, /*!< 28673 :  Enable NBEE RX calibration */

  FTM_WCDMA_RADIO_SET_DEBUG_FLAG  = 28674, /*!< 28674 :  Set debug flag*/

  FTM_WCDMA_RADIO_GET_MULTI_CARRIER_RXAGC = 28675, /*!< 28675 :  Get RxAGC for multiple carriers */

  FTM_WCDMA_RADIO_RX_TUNE_TO_MULTI_CHAN = 28676, /*!< 28676 :  Support Rx tune for SC/DC/3C */

  FTM_WCDMA_RADIO_CONFIG_HDET = 28677, /*!< 28677 : Configure HDET Loop */

  FTM_WCDMA_RADIO_PRE_TUNE    = 28678, /*!< 28678 : Pre Tune Channel */

  FTM_WCDMA_RADIO_RX_INIT    = FTM_WCDMA_RADIO_PRE_TUNE, /*!< 28678 : Rx Init */

  FTM_WCDMA_RADIO_TUNE        = 28679, /*!< 28679 : Tune Channel */

  FTM_WCDMA_RADIO_RX_WAKEUP   = 28680, /*!< 28680 : Rx Wakeup */

  FTM_WCDMA_RADIO_RX_SLEEP    = 28681, /*!< 28681 : Rx Sleep */

  FTM_WCDMA_RADIO_TX_INIT     = 28682, /*!< 28682 : Tx Init */

  FTM_WCDMA_RADIO_TX_ENABLE   = 28683, /*!< 28683 : TX Enable */

  FTM_WCDMA_RADIO_TX_DISABLE  = 28684, /*!< 28684 : Tx Disable */

  FTM_WCDMA_RADIO_TX_WAKEUP   = 28685, /*!< 28685 : Tx Wakeup */

  FTM_WCDMA_RADIO_GET_RXAGC   = 28686,  /*!< 28686 : Get RxAGC */

  FTM_WCDMA_RADIO_OVERRIDE_PLIM   = 28687,  /*!< 28687 : Override Tx Power limiting */

  FTM_WCDMA_RADIO_OVERRIDE_TEMPCOMP   = 28688,  /*!< 28688 : Override TempComp */

  FTM_WCDMA_RADIO_TX_SLEEP    = 28689, /*!< 28689 : Tx Sleep */

  FTM_WCDMA_RADIO_DEBUG_FLUSH    = 28690, /*!< 28690 : Flush Deug data */

  FTM_WCDMA_RADIO_RX_DEINIT    = 28691, /*!< 28691 : Rx Deinit */

  FTM_WCDMA_RADIO_TEAR_DOWN    = 28692, /*!< 28692 : Teardown radio */

  FTM_WCDMA_RADIO_LNA_OVERRIDE    = 28693, /*!< 28693 : LNA State Override */

  FTM_WCDMA_RADIO_EXECUTE_FED_EVENT    = 28694, /*!< 28694 : Execute FED event */

  FTM_WCDMA_RADIO_TX_DEINIT    = 28695, /*!< 28695 : Tx De-init */

  FTM_WCDMA_RADIO_TX_RETUNE_BW    = 28696, /*!< 28696 : Tx Retune Bandwidth */

  FTM_WCDMA_RADIO_RX_PREP_TUNE    = 28697, /*!< 28697 : rx_prep_tune */

  FTM_WCDMA_RADIO_RX_EXEC_TUNE    = 28698, /*!< 28698 : rx_exec_tune */

  FTM_WCDMA_RADIO_PWR_CLK_ON    = 28699, /*!< 28699 : power clock on */

  FTM_WCDMA_RADIO_PWR_CLK_OFF    = 28700, /*!< 28700 : power clock off */

  FTM_WCDMA_RADIO_RX_PREP_WAKEUP    = 28701, /*!< 28701 : rx_prep_wakeup */

  FTM_WCDMA_RADIO_RX_EXEC_WAKEUP    = 28702, /*!< 28702 : rx_exec_wakeup */

  FTM_WCDMA_RADIO_TX_DECFG    = 28703, /*!< 28703 : decfg_tx */

  FTM_WCDMA_RADIO_DEVICE_EXIT  = 28704, /*!< 28703 : Exit Device */

  FTM_WCDMA_RADIO_TX_REINIT    = 28705, /*!< 28705 : Tx Re-init */

  FTM_WCDMA_RADIO_TX_GET_AGC   = 28706,  /*!< 28706 : Get TxAGC */

  FTM_WCDMA_RADIO_TX_SET_POWER   = 28707,  /*!< 28707 : Set Tx Power */

  FTM_WCDMA_RADIO_CONFIG_AGC_LOG = 28708, /*!< 28708 : Configure AGC Log */

  FTM_WCDMA_RADIO_UNPACK_EVENT = 28709, /*!< 28709 : Unpack Event */

  FTM_WCDMA_RADIO_TX_LM_CONFIGURE = 28710, /*!< 28710 : LM configure */

  FTM_WCDMA_RADIO_TX_UPLINK_CONFIGURE = 28711, /*!< 28711 : Uplink configure */

  FTM_WCDMA_RADIO_TX_BANDWIDTH_RETUNE = 28712, /*!< 28712 : Tx bandwidth retune */

  FTM_WCDMA_RADIO_TX_CONFIGURE_DAC = 28713, /*!< 28713 : Tx configure dac */

  FTM_WCDMA_RADIO_GET_CA_INFO = 28714, /*!< 28714 : Get Carrier aggregation info */

  FTM_WCMDA_RADIO_TX_FREQ_OVERIDE = 28715, /*!< 28715 : Tx Freq overide */

  FTM_WCMDA_RADIO_TX_SET_CW = 28716, /*!< 28716 : Tx Set CW */

  FTM_WCDMA_RADIO_CMD_END         = 32767, /*!< 32767 : End of WCDMA cmd*/

  /*--------------------------------------------------------------------------*/
  /* NR5G (32768 - 36861) */
  /*--------------------------------------------------------------------------*/

  FTM_NR5G_RADIO_CMD_START = 32768, /*!< 32768 : Start of NR5G cmd*/

  FTM_NR5G_RADIO_ENTER = 32768, /*!< 32768 : Start NR5G mode */

  FTM_NR5G_RADIO_EXIT = 32769, /*!< 32769 : Stop NR5G mode */

  FTM_NR5G_RADIO_PREP_CONFIG = 32770, /*!< 32770 : Prepare Tune configuration */

  FTM_NR5G_RADIO_EXEC_CONFIG = 32771, /*!< 32771 : Execute Tune Configuration */

  FTM_NR5G_RADIO_WAKEUP = 32772, /*!< 32772 : Wakeup Radio */

  FTM_NR5G_RADIO_SLEEP = 32773, /*!< 32773 : Sleep Radio */

  FTM_NR5G_RADIO_CHANGE_SERVING_BEAM = 32774, /*!< 32774 : Change Serving Beam */

  FTM_NR5G_RADIO_BEAM_SWEEP = 32775, /*!< 32775 : Beam Sweep */

  FTM_NR5G_RADIO_CONFIG_COMPLETE = 32776, /*!< 32776 : Config Complete */

  FTM_NR5G_RADIO_UNPACK_EVENT = 32777, /*!< 32777 : Unpack event */

  FTM_NR5G_RADIO_POPULATE_SF_DB = 32778, /*!< 32778 : Populate Tx/Rx subframe dashboard */

  FTM_NR5G_RADIO_MISC_CMD = 32779, /*!< 32779 : Misc cmd to test radio config */

  FTM_NR5G_RADIO_ENABLE_MVT_FULL_STACK = 32780, /*!<32780 : Command to enable non-cal MVT mode */

  FTM_NR5G_PROFILING_SET_DETAIL_LEVEL    = 32781,  /*!< 32781 :  NR5G profiling detail control */

  FTM_NR5G_RADIO_DEBUG_REG_READ = 32782, /*<! 32782 : Command to schedule debug register reads */

  FTM_NR5G_RADIO_CONTROL_SF_DB_RUN = 32783, /*!< 32783 : Control RTC sequence */

  FTM_NR5G_RADIO_POPULATE_MAC_SHADOW_DB = 32784, /*!< 32784 : Fill MAC Shadow dashboard for MAC stub */

  FTM_NR5G_RADIO_STOP_MAC_STUB = 32785, /*!< 32785 : Stop the MAC stub */

  FTM_NR5G_RADIO_TXAGC_OVERRIDE = 32786, /*!< 32786 : Override TxAGC in online mode */

  FTM_NR5G_RADIO_POPULATE_TX_GEN_SLOT_DB = 32787, /*!< 32787 : Populate Tx general slot dashboard */

  FTM_NR5G_RADIO_POPULATE_TX_CAR_SLOT_DB = 32788, /*!< 32788 : Populate Tx per carrier slot dashboard */

  FTM_NR5G_RADIO_POPULATE_TXAGC_SLOT_DB = 32789, /*!< 32789 : Populate TxAGC slot dashboard */

  FTM_NR5G_RADIO_POPULATE_RX_SLOT_DB = 32790, /*!< 32790 : Populate Rx mmW or sub6 slot dashboard */

  FTM_NR5G_RADIO_POPULATE_TX_GEN_SLOT_SHADOW_DB = 32791, /*!< 32791 : Populate shadow Tx general slot dashboard */

  FTM_NR5G_RADIO_POPULATE_TX_CAR_SLOT_SHADOW_DB = 32792, /*!< 32792 : Populate shadow Tx per carrier slot dashboard */

  FTM_NR5G_RADIO_POPULATE_TXAGC_SLOT_SHADOW_DB = 32793, /*!< 32793 : Populate shadow TxAGC slot dashboard */

  FTM_NR5G_RADIO_START_MAC_STUB = 32794, /*!< 32794 : Start the MAC stub */

  FTM_NR5G_RADIO_SET_RTC_SETUP_FLAGS = 32795, /*!< 32795 Populate RTC related flags */

  FTM_NR5G_RADIO_DEBUG_RFE      = 32796, /*!< 32795 RFE debug module command */

  FTM_NR5G_RADIO_FDRSB_LB_TEST_SETUP = 32797, /*!< 32797 FDRSB LB Test setup */

  FTM_NR5G_MPE_TYPE1_GUI_FREQ_OFFSET_WRITE     = 32798, /*!< 32798 MPE type1 gui freq. offset write command*/

  FTM_NR5G_MPE_TYPE1_GUI_PARAMS_READ           = 32799, /*!< 32799 MPE type1 gui params read command */

  FTM_NR5G_MPE_TYPE1_GUI_PARAMS_WRITE          = 32800, /*!< 32800 MPE type1 gui params write command */

  FTM_NR5G_RADIO_POPULATE_TX_MDM_IU_INFO       = 32801, /*!< 32801 Populate Tx/Rx DB from RTC dashboard to be sent to MDM IU */

  FTM_NR5G_AMPR_ENDC_LTE                       = 32802, /*!< 32802 LTE freq information for AMPR ENDC */

  FTM_NR5G_REFLOG_CAPTURE_TYPE                 = 32803, /*!< 32803 REFLOG capture command for Sub6 */

  FTM_NR5G_MPE_RF_DEBUG_GUI_FREQ_OFFSET_WRITE     = 32804, /*!< 32804 MPE RF Debug GUI freq. offset write command*/

  FTM_NR5G_MPE_RF_DEBUG_GUI_PARAMS_READ           = 32805, /*!< 32805 MPE RF Debug GUI params read command */

  FTM_NR5G_MPE_RF_DEBUG_GUI_PARAMS_WRITE_0        = 32806, /*!< 32806 MPE RF Debug GUI params write command */

  FTM_NR5G_MPE_RADAR_DEBUG_GUI_PARAMS_READ        = 32807, /*!< 32807 MPE RADAR Debug GUI params read command */

  FTM_NR5G_MPE_RADAR_DEBUG_GUI_PARAMS_WRITE       = 32808, /*!< 32808 MPE RADAR Debug GUI params write command */

  FTM_NR5G_RADIO_UPDATE_MDM_IU_CONFIG             = 32809, /*!< 32809 Send new config to modem IU */

  FTM_NR5G_MPE_RF_DEBUG_GUI_PARAMS_RESET          = 32810, /*!< 32810 MPE rf Debug GUI params reset command */

  FTM_NR5G_MPE_RF_DEBUG_GUI_PARAMS_WRITE          = 32811, /*!< 32811 MPE rf Debug GUI params write command */

  FTM_NR5G_MPE_TYPE1_DEBUG_GUI_PARAMS_RESET       = 32812, /*!< 32812 MPE Type1 Debug GUI params reset command */

  FTM_NR5G_MPE_TYPE1_DEBUG_GUI_PARAMS_WRITE       = 32813, /*!< 32813 MPE Type1 Debug GUI params write command */

  FTM_NR5G_MPE_TYPE2_DEBUG_GUI_PARAMS_RESET       = 32814, /*!< 32814 MPE Type2 Debug GUI params reset command */

  FTM_NR5G_MPE_TYPE2_DEBUG_GUI_PARAMS_WRITE       = 32815, /*!< 32815 MPE Type2 Debug GUI params write command */

  FTM_NR5G_RADIO_POWER_CONFIG                     = 32816, /*!<32816 Power Config Command */

  FTM_NR5G_RX_SCHEDULED_IQ_CAPTURE                = 32817, /*!< 32817 Rx Scheduled IQ Capture command */

  FTM_NR5G_SCHEDULER_POPULATE_DB_INFO             = 32818, /*!< 32818 Populate Sub6 DB info command */

  FTM_NR5G_MMW_RX_SCHEDULER_POPULATE_DB_INFO      = 32819, /*!< 32819 Populate mmW DB info command */

  FTM_NR5G_SCHEDULER_START_STOP                   = 32820, /*!< 32820 Scheduler Start Stop for Rx Meas command */

  FTM_NR5G_MTPL_OVERRIDE                          = 32821, /*!< 32821 To dynamically override SUB6 MTPL limit */

  FTM_NR5G_RADIO_CAPTURE_MACHINE                  = 32822, /*!<32822 DFE capture machine data Config Command */

  FTM_NR5G_RADIO_FTECH_CM                         = 32823, /*!<32823 DFE capture machine data fetch Command */

  FTM_NR5G_LNA_TOGGLE                             = 32824, /*!< 32822 Toggle LNA */

  FTM_NR5G_SLEEP_DTA_FETCH                        = 32825, /*!< 32825 DTS SLEEP FETCH config */

  FTM_NR5G_RFIU_RTC_SEQ                           = 32826, /*!< 32826 riot sequence */

  FTM_NR5G_RFIU_VECTOR_SEQ                        = 32827, /*!< 32827 Vector Test Sequence*/

  FTM_NR5G_PEEK_POKE_MDM_ADDR                     = 32828, /*!< 32828 Peek/Poke Modem Side addresses*/

  FTM_NR5G_RADIO_SUB6_BWP_SWITCH                  = 32829, /*!< 32829 BWP switch comand */

  FTM_NR5G_RADIO_PREP_SUBCFG_REQ                  = 32830, /*!< 32830 PREP SUB CFG REQ */

  FTM_NR5G_MMW_L3_THERM_READ                      = 32831, /*!< 32829 L3 Therm Read */

  FTM_NR5G_RADIO_MMW_MICRO_SLEEP                  = 32832, /*!< 32832 : Micro Sleep */

  FTM_NR5G_RADIO_MMW_GET_PROCESS_ID               = 32833, /*!< 32833: MMW get chip process IDs*/

  FTM_NR5G_RADIO_SEQUENCER_TEST                   = 32834, /*!< 32834 : Sequencer Test */

  FTM_NR5G_RADIO_DYN_BEAMS_UPDATE_TEST            = 32835, /*!< 32835: test mmw dyn beams update*/

  FTM_NR5G_PSD_10MHZ_LIMIT                        = 32836, /*!< 32836 : PSD 10 MHz Limit Update */

  FTM_NR5G_RADIO_PREP_CARR_ACT_DEACT_REQ          = 32837, /*!< 32830 PREP SUB CFG REQ */

  FTM_NR5G_RADIO_NR_U_POPULATE_REQ                = 32838, /*!< 32837: NR-U populate REQ*/

  FTM_NR5G_RADIO_CMD_END = 36861, /*!< 36861 : End of NR5G cmd*/

  /*--------------------------------------------------------------------------*/
  /* Reserved (36862 - 65535) */
  /*--------------------------------------------------------------------------*/

  FTM_CDMA_RADIO_MAX /*!< Maximum number of HWTC Command */

} ftm_common_radio_cmd_enum;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Lists the FTM Command IDs for Diag Interface.
*/
typedef enum
{
  FTM_SET_PDM, /*!< 0 : Command to override Tx power using RGI */
  FTM_SET_PDM_CLOSE, /*!< 1 : TBD */
  FTM_SET_TX_ON, /*!< 2 : Command to Enable Tx */
  FTM_SET_TX_OFF, /*!< 3 : Command to Disable Tx */
  FTM_SET_MODE = 7, /*!< 7 : Command to configure the FTM of operation for
                    a given technology */
  FTM_SET_CHAN, /*!< 8 : Command to tune radio to given channel */
  FTM_GET_RSSI, /*!< 9 : Command to query the Receive Signal Strength
                Indication */
  FTM_GSM_CAPTURE_IQ = 11, /*!< 11 : Capture IQ capture in GSM mode of
                           operation */
  FTM_GSM_DO_IP2_CAL = 12,
  FTM_GSM_DISABLE_IDLE_FRAME = 13, /*!< 13 : TBD */
  FTM_GSM_GET_IP2_CAL_RESULTS = 15,
  FTM_GET_SYNTH_STATE = 16, /*!< 16 : Query the PLL synthesizer lock status */
  FTM_SET_SMPS_PA_BIAS_VAL = 17, /*!< 17 : Set the PA Bias value */
  FTM_SET_SMPS_PA_BIAS_OVERRIDE = 18, /*!< 18 : Set the flag indicating
                                     PA Bias is overridden */
  FTM_SET_PA_CURRENT = 19, /*!< 19 : Set PA current */

  FTM_GSM_SAWLESS_LIN_OVERRIDE = 25, /*!< 25: Override Rx Linearity mode on SAWless targets */
  FTM_GSM_GET_SAWLESS_LIN_MODE = 26, /*!< 26: Retrieve Rx Linearity mode on SAWless targets */

  FTM_GSM_IRAT_INPUT_DATA = 27, /*!< 27 : Input expected level for X2G FTM IRAT case */

  FTM_GSM_IRAT_OUTPUT_DATA = 28, /*!< 28 : Output measured level for X2G FTM IRAT case */

  FTM_SET_CDMA_CW_WAVEFORM = 50, /*!< 50 : Configure the type of waveform for
                                 WCDMA and CDMA */
  FTM_GET_CAGC_RX_AGC = 53,  /*!< 53 : Query the current RxAGC */
  FTM_SET_PA_RANGE = 54, /*!< 54 : Override the PA range value */
  FTM_SET_PA_TUNABLE_CAP = 55, /*!< 55 : Set the PA Tunable Cap value */
  FTM_SET_LNA_RANGE = 58, /*!< 58 : Override LNA state */
  FTM_GET_ADC_VAL = 59, /*!< 59 : Read ADC channel */
  FTM_SET_LNA_OFFSET = 81, /*!< 81 : Set the LNA offset value for the given
                           LNA state */
  FTM_RF_SET_SCENARIO = 100, /*!< 100 : Set the  right scenario for FTM-RF testing */

  FTM_SET_DVGA_OFFSET = 111, /*!< 111 : Set the DVGA offset value for the
                             given DVGA offset  */
  FTM_GET_CDMA_IM2 = 114, /*!< 114 : Query CDMA IM2 value */
  FTM_GSM_IM2 = FTM_GET_CDMA_IM2, /*!< 114 : Query GSM IM2 value */
  FTM_LTE_IM2 = FTM_GET_CDMA_IM2, /*!< 114 : Query LTE IM2 value */
  FTM_TX_SWEEP_CAL = 116, /*!< 116 : Perform Tx linearizer sweep */
  FTM_GET_DVGA_OFFSET = 117, /*!< 117 : Perform one instance of DVGA
                             calibration to return DVGA offset */
  FTM_GET_LNA_OFFSET=118, /*!< 118 : Perform one instance of LNA calibration
                          to return LNA offset */
  FTM_GET_HDET_FROM_TX_SWEEP_CAL=119, /*!< 119 : Query the HDET data pertaining
                                      to a index generated as a result of Tx
                                      Linearizer sweep */
  FTM_SET_SECONDARY_CHAIN=121, /*!< 121 : Enable the secondary chain in
                               a specific mode of operation such as
                               diversity/SHDR */
  FTM_GET_CTON = 122, /*!< 122 : Query Carrier to Noise ratio */
  FTM_SECOND_CHAIN_TEST_CALL=123, /*!< 123 : Setup a second chain test call */
  FTM_SET_HDET_TRACKING = 124, /*!< 124 : TBD */
  FTM_CONFIGURE_TX_SWEEP_CAL = 125, /*!< 125 : Push configuration data to
                                    be used for Tx linearizer sweep */
  FTM_GET_CDMA_IM2_WITH_SUPPRESSION=126, /*!< 126 : Get CDMA IM2 */
  FTM_GET_ALL_HDET_FROM_TX_SWEEP_CAL=127, /*!< 127 : Query all HDET data
                                          generated as a result of Tx
                                          Linearizer sweep */
  FTM_SET_RF_POWER_MODE=128, /*!< 128 : Configure power mode */
  FTM_SET_TX_SWEEP_STEP_SIZE=129, /*!< 129 : Configure the step size to be used
                                  for Tx linearizer sweep */
  FTM_TX_RX_FREQ_CAL_SWEEP=130, /*!< 130 : Start Composite calibration */
  FTM_LOAD_RF_NV=131, /*!< 131 : Reload RFNV items from EFS to local data
                      structure */
  FTM_DO_INTERNAL_DEVICE_CAL=132, /*!< 132 : Perform internal device
                                  calibration */
  FTM_DO_DC_CAL=133, /*!< 133 : Perform DC calibration */
  FTM_DO_XO_DC_CAL = 134, /*!< 134 : Perfrom XO DC calibration */
  FTM_DO_XO_FT_CURVE_CAL = 135, /*!< 135 : Perform XO curve fitting */
  FTM_SET_TX_WAVEFORM =136, /*!< 136 : Set the Tx waveform */
  FTM_SET_CALIBRATION_STATE = 137, /*!< 137 : Configure the calibration state */
  FTM_GET_THERM = 138, /*!< 138 : Replicating get therm in common so GSM
                       only builds can do therm cal */
  FTM_GET_AGC_RESOLUTION_TX = 139, /*!< 139 : TBD */
  FTM_GET_AGC_RESOLUTION_RX = 140, /*!< 140 : TBD */
  FTM_SELECT_RX_ANTENNA = 141, /*!< 141 : TBD */
  FTM_GET_ENH_THERM = 142, /*!< 142 : Perform Therm Read */
  FTM_DO_IQ_MISMATCH_CAL = 143, /*!< 143 : Perform IQ mismatch calibration */
  FTM_GET_ALL_HDET_FROM_TX_SWEEP_CAL_V2=144, /*!< 144 : Get all HDET reads
                                             in UINT16 generated during
                                             Tx Linearizer sweep */
  FTM_DO_ICI_CAL = 145, /*!< 145 : Perform ICI calibration */
  FTM_SET_DIGITAL_IQ_GAIN = 146, /*!< 146 : For setting TX IQ gain value */
  FTM_GET_DIGITAL_IQ_GAIN = 147, /*!< 147 : For getting TX IQ gain value */
  FTM_ENABLE_RF_TXPLIM_DEBUG_MSG = 150, /*!< 150 : FTM command to enable RF
                                        Txplim debug messages */
  FTM_SET_BLOCK_MASK = 153, /*!< 153 : Set sub-class block mask */
  FTM_DO_ENH_XO_DC_CAL=154, /*!< 154 : Perform XO DC calibration */
  FTM_DO_ENH_XO_FT_CURVE_CAL = 155, /*!< 155 : Perform XO curve fitting */
  FTM_DO_ENH_XO_FT_COLLECT_SAMPLE = 156, /*!< 156 : Collect samples generated
                                         during curve fitting */
  FTM_DO_ENH_XO_DC_CAL_V2=157, /*!< 157 : Perform XO DC calibration */
  FTM_DO_ENH_XO_FT_CURVE_CAL_V2 = 158, /*!< 158 : Perform XO curve fitting */
  FTM_DO_ENH_XO_FT_COLLECT_SAMPLE_V2 = 159, /*!< 159 : Collect samples generated
                                         during curve fitting */
  FTM_DO_PSEUDO_ILPC_SWEEP = 160, /*!< 160 : TBD */
  FTM_ENABLE_LPM_HDET = 161, /*!< 161 : Enable low power mode HDET */
  FTM_GET_LPM_HDET_FROM_TX_SWEEP_CAL=162, /*!< 162 : Get one LPM HDET read
                                          generated by Tx Lin sweep */
  FTM_GET_ALL_LPM_HDET_FROM_TX_SWEEP_CAL=163, /*!< 163 : Get all LPM HDET
                                              generated by Tx Lin Sweep */

  FTM_SAR_POWER_LIMIT_SIMULATION = 164, /*!< 164 :  test SAR max power limit*/
  FTM_GET_UNIFIED_ENH_THERM = 165, /*!< 165 : Generic command to get Therm read
                                   for a given tech on a device*/
  FTM_COMMON_SAVE_DATA_TO_FILE = 168, /*!< 168 : TBD */
  FTM_TRIGGER_EPT_IQ_CAP = 169, /*!< 169 : Triiger EPT IQ capture */
  FTM_ENABLE_EUL_WAVEFORM = 170, /*!< 170 : Enable EUL waveform */
  FTM_DISABLE_EUL_WAVEFORM = 172, /*!< 172 : Disable EUL waveform */
  FTM_WCDMA_SET_TXAGC = 173, /*!< 173 : Set WCDMA TxAGC */
  FTM_WCDMA_TXAGC_SWEEP = 174, /*!< 174 : Perform WCDMA TxAGC sweep */
  FTM_WCDMA_TPC_CONTROL = 175, /*!< 175 : TBD */
  FTM_SET_TX_POWER_DBM = 176, /*!< 176 : Set Tx Power in dBm */
  FTM_LTE_SET_NET_SIG_VALUE = 177, /*!< 177 : TBD */
  FTM_GET_RX_LEVEL_DBM = 179, /*!< 179 : Get RxAGC in dBm */
  FTM_SET_OFFSET_CW = 180, /*!< 180 : Set the offset Continuous waveform */
  FTM_PA_SMPS = 181, /*!< 181 : TBD */
  FTM_SET_PA_STATE = 182, /*!< 182 : Override PA state */
  FTM_SET_PA_SMPS_LEVEL = 183, /*!< 183 : Set PA SMPS level */
  FTM_DO_WCDMA_IQ_LOGGING = 184, /*!< 184 : Enable WCDMA IQ logging */
  FTM_LTE_SET_LNA_STATE_DBM = 185,  /*!< 185 : Set LNA state based on the
                                    expected DL power*/
  FTM_SET_FREQ_ADJUST = 186, /*!< 186 : TBD */
  FTM_GET_MULTIPLE_RX_LEVEL_DBM = 187, /*!< 187 : Gets RxAGC in AGC and dBm
                                       units for the selected receivers */
  FTM_FETCH_ET_DELAY = 188, /*!< 188 : DPD cal - Get ET Path Delay in units
                               of 0.1ns */
  FTM_CALCULATE_XPT_PIN_COMPRESSION = 189, /*!< 189 : DPD cal - Get ET Pin target
                                               compression point */
  FTM_SET_TX_ENH_CAL_ACTION = 190, /*!< 190 : Sets enhanced cal actions
                                   for transmitter */
  FTM_LOAD_TX_LINEARIZER_CAL_DATA = 191, /*!< 191 : Loads the Tx linearizer
                                         calibration data for a given band */
  FTM_LOAD_TX_FREQCOMP_CAL_DATA = 192, /*!< ¤192 : Loads Tx Freq comp Cal data
                                       for a given band*/
  FTM_LOAD_DPD_TBLS = 197, /*!< 197 : Loads AMAM and AMPM tables to be
                           effective at next BPG */
  FTM_SET_SAPT_STATE = 198, /*!< 198 : Sets SAPT state. */
  FTM_MULTIMODE_HANDOVER = 199, /*!< 199 : TBD */

  FTM_PEEK_DSP_INPORT = 200,/*!< 200 : retrieving the content of
                            the QDSP3 MEMB */
  FTM_POKE_DSP_OUTPORT,     /*!< 201 : send user data to the QDSP3 MEMB */
  FTM_SET_GRFC_ALL,         /*!< 202 : set all GRFCs (configured as output)*/
  FTM_GET_GRFC_ALL,         /*!< 203 : sense all GRFCs
                            (configured as input/output) */
  FTM_GRFC_CONFIG_ALL,      /*!< 204 : configure the GRFC (timing, direction,
                            polarity, mode) */
  FTM_SET_GRFC_DATA,        /*!< 205 : set a specific GRFC to the specified
                            state (HI/LOW) */
  FTM_SET_GRFC_OMODE,       /*!< 206 : configure the output mode (static/timed)
                            for a GRFC */
  FTM_SET_GRFC_TRISTATE,    /*!< 207 : tristate a specific GRFC */
  FTM_SET_GRFC_POLARITY,    /*!< 208 : set a specific GRFC polarity
                            (active HI/active LOW) */
  FTM_SET_SYNTH_ON,         /*!< 209 : turn RF synthesizer circuitry on */

  FTM_SET_SYNTH_OFF,        /*!< 210 : turn RF synthesizer circuitry off */
  FTM_SET_ANTENNA_CONFIG,   /*!< 211 : configure the antenna switch/duplexer
                            for RX/TX operation (band)        */
  FTM_SET_PA_PROFILE,       /*!< 212 : download GSM PA power profile for
                            specified power level                 */
  FTM_GET_PA_PROFILE,       /*!< 213 : retrieve GSM PA power profile for
                            specified power level                 */
  FTM_GET_PA_CONFIG,        /*!< 214 : retrieve GSM PA configuartion  */
  FTM_CONF_MODULATE_DATA,   /*!< 215 : configure GMSK modulator */
  FTM_SET_TRANSMIT_CONT,    /*!< 216 : transmit a tone (all 1's to
                            GMSK modulator). Causes RF controls setup   */
  FTM_SET_TRANSMIT_BURST,   /*!< 217 : transmit user defined data pattern.
                            Causes RF controls setup            */
  FTM_SET_VGA_GAIN_VAL,     /*!< 218 : program a gain value onto the VGA in
                            RF module                          */
  FTM_SET_LNA_ON,           /*!< 219 : turn LNA On */

  FTM_SET_LNA_OFF,          /*!< 220 : turns LNA off */
  FTM_SET_RX_ON,            /*!< 221 : perform all operations required to
                            turn on RX circuitry                 */
  FTM_SET_RX_OFF,           /*!< 222 : perform all operations required to
                            turn off RX circuitry                */
  FTM_GET_RX_DC_OFFSET,     /*!< 223 : get the residual DC reading from
                            the RX samples                         */
  FTM_GET_RX_IQ_DATA,       /*!< 224 : receive a specified number of
                            raw RX samples                            */
  FTM_SET_RX_BURST,         /*!< 225 : set up Rx burst type */
  FTM_CHFILT_CONFIG,        /*!< 226 : configure the channel filter */
  FTM_SET_TX_POWER_LEVEL,   /*!< 227 : select TX power level by choosing a
                            pre-configured PA LUT values        */
  FTM_GET_PHONE_STATE,      /*!< 228 : query the state of the phone, such as
                            current channel, power level, etc */
  FTM_SET_GRFC_TIMING,      /*!< 229 : set the timing information for a GRFC
                            (as well as TX/RX/General)        */

  FTM_GET_GSM_RSSI,        /*!< 230 : get raw RSSI value */
  FTM_SET_PA_START_DELTA,  /*!< 231 : move the Pa ramp start time by the
                           specified delta */
  FTM_SET_PA_STOP_DELTA,   /*!< 232 : move the Pa ramp stop time by the
                           specified delta */
  FTM_SET_PA_DAC_INPUT,    /*!< 233 : set pa dac input */
  FTM_SET_RX_CONTINUOUS,   /*!< 234 : set Rx continuous */
  FTM_DO_GSM_TX_CAL_SWEEP, /*!< 235 : Tx cal sweep */
  FTM_DO_GSM_AUTOCAL,      /*!< 236 : Do autocal */
  FTM_SET_PATH_DELAY,      /*!< 237 : Set the path delay for AMAM/AMPM */
  FTM_SET_AMAM_TABLE_GAIN, /*!< 238 : Set the gain for the AMAM table */
  FTM_SET_AMAM_TABLE_DC,   /*!< 239 : Set the DC offset for the AMAM table */

  FTM_SET_TX_FRAME_MATRIX,      /*!< 240 : Set the TX frame matrix */
  FTM_ENABLE_POLAR_REF_CAL,     /*!< 241 : Enable polar path reference
                                calibration */
  FTM_SET_TWOTONE_FREQ_OFFSET,  /*!< 242 : Sets the frequency offset for the
                                two tone transmit */
  FTM_DO_CALPATH_RSB,           /*!< 243 : Measure calpath RSB */
  FTM_DO_RTR6250_RSB,           /*!< 244 : Load VI firmware for RSB */
  FTM_SET_OPLL_BW,              /*!< 245 : set Chargepump current registers */
  FTM_RX_GAIN_RANGE_CAL,        /*!< 246 : FTM GSM RX GAIN RANGE Calibration */
  FTM_GSM_SET_LINEAR_RGI,       /*!< 247 : set tx RGI */
  FTM_GSM_SET_TX_SLOT_CONFIG,   /*!< 248 : set PA state,RGI and env_gain
                                for a slot */
  FTM_GSM_SET_PREDIST_OVERRIDE, /*!< 249 : Enable or disable GSM predist in
                                FTM mode*/

  FTM_BASEBAND_BW_CAL,            /*!< 250 : Baseband Bandwidth calibration */
  FTM_TX_KV_CAL,                  /*!< 251 : KV calibration */
  FTM_SET_GSM_LINEAR_PA_RANGE,    /*!< 252 : Used to set LINEAR PA GSM range */

  FTM_GNSS_EXTERNAL_LNA,   /*!< 253 : FTM_GNSS_EXTERNAL_LNA - for external LNA */
  FTM_GSM_TX_KV_CAL_V2,    /*!< 254 : Kv v2 */
  FTM_GNSS_PGA_BACKOFF,    /*!< 255 : FTM_GNSS_PGA_BACKOFF- for PGA backoff */
  FTM_GNSS_PGA_RESTORE,    /*!< 256 : FTM_GNSS_PGA_RESTORE- for PGA restore */

  FTM_FREQ_ADJUST = 258, /*!< 258 : Set Frequency Adjust */

  FTM_SET_TX_PWR_LIMIT = 259, /*!< 259 : Set Tx Power Limit */
  FTM_XPT_CONFIG = 260, /*!< 260 :Perform XPT config */

  FTM_RESET_MULTI_SYNTH_STATE = 261, /*!< 261 : TBD */

  FTM_GET_MULTI_SYNTH_STATE = 262, /*!< 262 : TBD */

  FTM_GET_GSM_PRX_DRX_RSSI = 268, /*!< 268 : Get PRX & DRX RSSI */

  FTM_RX_GAIN_RANGE_PRX_DRX_CAL = 269,     /*!< 269 : GSM RX cal for PRX & DRX */

  FTM_TX_FREQ_OVERRIDE = 270,     /*!< 270 : Override TX freq to a RX channel */
  FTM_TX_ARB_CW_TONE = 271,     /*!< 271 : Generate CW tone from a predefined waveform */
  FTM_AUTOPIN_TX_PAR_CAPTURE = 272,     /*!< 272 : Capture TX and compute PAR */

  FTM_GSM_POLAR_TX_SWEEP_CAL = 300, /*!< 300 : FTM GSM TX Sweep calibration */
  FTM_TX_RSB_DC_SWEEP,              /*!< 301 : FTM GSM TX RSB and DC Sweep
                                    calibration */
  FTM_GSM_DETECTOR_CAL,             /*!< 302 : FTM GSM TX Detector
                                    calibration */
  FTM_TX_4xFMOD_SWEEP,              /*!< 303 : FTM GSM 4xf MOD calibration */
  FTM_TX_CS_SWEEP,                  /*!< 304 : FTM GSM TX carrier suppression
                                    calibration */
  FTM_SET_SMPS_PDM_TBL,             /*!< 305 : FTM GSM set pdm for external
                                    SMPS on RF target */
  FTM_TX_RSB_DC_SWEEP_GEN,   /*!< 306 : FTM RSB and DC CAL - uses the
                             generic ftm_sweep func */
  FTM_TX_4xFMOD_SWEEP_GEN,   /*!< 307 : FTM 4xf MOD CAL - uses the
                             generic ftm_sweep func */
  FTM_GSM_TX_ENVDC_CS_SWEEP, /*!< 308 : TBD     */
  FTM_TX_CFG2_AMAM_SWEEP,    /*!< 309 : TBD */
  FTM_GSM_TX_GAIN_SWEEP,     /*!< 310 : TBD */
  FTM_GSM_SETUP_RX_BURST_FOR_EXPECTED_PWR = 329,/*!< 329 : TBD */
  FTM_GSM_GET_MEAS_PWR = 330, /*!< 330 : TBD */


  FTM_GSM_SET_RX_TIMING = 315, /*!< 315 : debug facility to update the GSM
                               Rx timing delta */
  FTM_GSM_SET_TX_TIMING, /*!< 316 : debug facility to update the GSM Tx timing
                         delta */

  FTM_MEAS_WTOW_TEST, /*!< 317 - IRAT WTOW operation, test WCDMA */

  FTM_GSM_SET_RX_MULTISLOT, /*!< 318 : FTMmultislot RX */

  FTM_MEAS_GTOW_TEST_WCDMA = 319, /*!< 319 : IRAT GTOW operation, test WCDMA */
  FTM_MEAS_GTOW_TEST_GSM = 320, /*!< 320 : IRAT GTOW operation, test GSM */
  FTM_MEAS_WTOG_TEST_GSM = 321, /*!< 321 : IRAT WTOG operation, test GSM */
  FTM_MEAS_WTOG_TEST_WCDMA = 322, /*!< 322 : IRAT WTOG operation, test WCDMA */
  FTM_GSM_RSSI_TO_DB, /*!< 323 : convert RSSI to dBm*/

  FTM_TX_CFG2_AMAM_SWEEP_V2 = 324, /*!< 324 : Enhanced EDGE predistortion cal */
  FTM_DO_SELF_CAL = 325,           /*!< 325 : For all the self calibration
                                   routines of RF Devices & MSM */
  FTM_MEAS_XTOY_TEST_Y = 326,      /*!< 326 : IRAT XTOY operation, test Y */
  FTM_MEAS_XTOY_TEST_X = 327,      /*!< 327 : IRAT XTOY operation, test X */

  FTM_MEAS_LTOW_TEST_WCDMA,        /*!< 319 : IRAT LTOW operation, test WCDMA*/
  FTM_MEAS_XTOL_TEST_LTE,          /*!< 320 : IRAT xTOL operation, test LTE */
  FTM_MEAS_XTODO_TEST_DO,          /*!< 321 : IRAT xTODO operation, test DO */

  FTM_DO_DAC_CAL = 331, /*!< 331 : Command to do DAC cal only. This is a
                        temporary command added for customers who are facing DAC
                        Cal issues and need to be removed later */
  FTM_MEAS_WRITE_WTOW, /*!< 332 : TBD */

  FTM_GSM_GET_MEAS_PWR_DRX = 333,

  FTM_GSM_GET_MEAS_PWR_PRX_DRX = 334,

  FTM_GSM_SET_ALT_PATH = 335,

  FTM_GSM_GET_NV_CONTAINER = 336,

  FTM_MEAS_NULL2X_SCRIPT_START = 337,              /*!< 337 : IRAT NULL2X start */
  FTM_MEAS_NULL2X_SCRIPT_EXIT = 338,               /*!< 338 : IRAT NULL2X exit */

  FTM_MEAS_HALF_SCRIPT_HOMEFREQ_2_NBR_X = 339,    /*!< 339 : Home Freq to Neighbour X */
  FTM_MEAS_HALF_SCRIPT_HOMEFREQ_2_NBR_Y = 340,    /*!< 340 : Home Freq to Neighbour Y */

  FTM_MEAS_HALF_SCRIPT_NBR_X_2_NBR_Y   = 341,    /*!< 341 : Neighbour X to Neighbour Y */
  FTM_MEAS_HALF_SCRIPT_NBR_X_2_HOMEFREQ = 342,    /*!< 342 : Neighbour X to Home Freq*/

  FTM_MEAS_HALF_SCRIPT_NBR_Y_2_NBR_X   = 343,    /*!< 343 : Neighbour Y to Neighbour X */
  FTM_MEAS_HALF_SCRIPT_NBR_Y_2_HOMEFREQ = 344,    /*!< 344 : Neighbour Y to Home Freq */

  FTM_MEAS_GET_RXAGC = 345,  /*!< 345 : Command to get RXAGC during FTM IRAT */

  FTM_GSM_SET_DEVICE_AND_SIG_PATH = 346, /*!< 346 Set the GSM RF device and sig path*/

  #ifdef FEAT_DIFF_SIG_PATH_PER_GSM_BAND
  FTM_GSM_SET_RFM_PATH = 75, /*!< 346 Set the GSM RFM PATH*/
  FTM_GSM_SET_RFM_PATH_V2 = 76,
  #endif

  FTM_GSM_SET_NS_BAND = 347,  /*!< 347 : Command to set GSM NS band */

  FTM_GSM_SET_ASDIV_PATH = 348,
  FTM_GSM_SET_FBRX_REQUEST_FLAG = 349, /*!< 346 Set the GSM RF device and sig path*/
  FTM_INT_GSM_SET_ASDIV_PATH = 350, /*!< 350 : Internal version of FTM_GSM_SET_ASDIV_PATH */

  FTM_SET_TX_BW = 400, /*!< 400 : Sets the transmit bandwidth.
                       In LTE the TX BW can be 1.4,3,5,10,15,20MHz*/
  FTM_SET_RX_BW = 401, /*!< 401 : Sets the receive bandwidth.
                       In LTE the RX BW can be 1.4,3,5,10,15,20MHz*/
  FTM_SET_TX_GAIN_INDEX = 402, /*!< 402 : This command replaces the SET_PDM.
                               In the newer targets PDM is no longer used.*/
  FTM_SET_LTE_WAVEFORM = 403, /*!< 403 : Configure LTE waveform */
  FTM_STOP_LTE_WAVEFORM = 404, /*!< 404 : Stop LTE confirm */
  FTM_LTE_TEST_TONE_GEN = 405, /*!< 405 : Generate Test Tone */
  FTM_LTE_TEST_SET_TXAGC = 406, /*!< 406 : Set TxAGC  */
  FTM_LTE_CONFIGURE_RX = 407, /*!< 407 : Configure LTE Rx */
  FTM_LTE_CONFIGURE_TX = 408, /*!< 408 : Configure LTE Tx */
  FTM_LTE_SET_RX_ON = 409, /*!< 409 : Set Rx ON signal */
  FTM_LTE_DO_RX_IQ_MISMATCH_CAL = 410, /*!< 410 : Perform LTE IQ mismatch cal */
  FTM_LTE_SET_RX_IQ_MISMATCH_COMP_COEFFTS = 411, /*!< 411 : Set IQ mismatch
                                                 compensation coeff */
  FTM_LTE_GET_RX_IQ_MISMATCH_COMP_COEFFTS = 412, /*!< 412 : Get IQ mismatch
                                                 compensation coeff */
  FTM_LTE_DO_TX_IQ_MISMATCH_CAL = 413, /*!< 413 : Perform LTE TX IQ mismatch */
  FTM_LTE_SET_TX_IQ_MISMATCH_COMP_COEFFTS = 414, /*!< 414: Set Tx IQ MC coeff */
  FTM_LTE_GET_TX_IQ_MISMATCH_COMP_COEFFTS = 415, /*!< 415 :  Get Tx IQ MC
                                                 coeff */
  FTM_OVERRIDE_TXAGC_OUTPUT_POWER = 416, /*!< 416 : override TxAGC output */
  FTM_OVERRIDE_TXAGC_OUTPUT_POWER_DISABLE = 417, /*!< 417 : Disable
                                                 TxAGC Override */
  FTM_OVERRIDE_TXAGC_BACKOFF = 418, /*!< 418 : Override TxAGC backoff */
  FTM_OVERRIDE_TXAGC_BACKOFF_DISABLE = 419, /*!< 419 : Clear txAGC backoff
                                            override */
  FTM_LTE_SLEEP  = 420, /*!< 420 : Perform LTE sleep */
  FTM_LTE_WAKEUP = 421, /*!< 421 : Perform LTE wakeup */
  FTM_LTE_CAL_V3_CONFIG_RADIO     = 422, /*!< 422 : perform calv3 configure
                                         radio */
  FTM_LTE_CAL_V3_RETUNE_RADIO     = 423,/*!< 423 : Perform calv3 retune */
  FTM_LTE_CAL_V3_TEAR_DOWN_RADIO  = 424, /*!< 424 : perform Radio teardown */

  FTM_LTE_SET_TX_CARRIER_INDEX = 425, /* Command to set Carrier index in FTM state machine.*/
  FTM_LTE_CONFIGURE_TX_DUTY_CYCLE = 426, /* Command to set Carrier index in FTM state machine.*/
  FTM_LTE_ASDIV_RELOAD_LNA_DATA = 427,   /* Command to trigger asdiv reload lna scripts.*/
  FTM_LTE_SET_HD_FDD = 428,   /* Command to set HD FDD mode for CAT M1.*/
  FTM_LTE_SET_RX_SYS_BW = 429, /*Command to set LTE Rx System BW for CAT M1*/
  FTM_LTE_RX_NB_RETUNE = 430,/*Command to retune Rx to new NB region for CAT M1*/
  FTM_LTE_SET_TX_SYS_BW = 431, /*Command to set LTE Tx System BW for CAT M1*/
  FTM_LTE_SET_TX_NB_REGION = 432, /*Command to set LTE UL Hop nb region for CAT M1*/
  FTM_LTE_SET_NB_MODE = 433,   /* Command to set LTE NB mode.*/
  FTM_LTE_CONFIG_FS_CHAN_LIST = 434,   /* Command to configure FS channel list in LTE NB mode.*/
  FTM_LTE_NB_FS_RETUNE = 435,   /* Command to retune FS channel in LTE NB mode.*/
  FTM_LTE_NB_SET_WAVEFORM = 436,   /* Configure LTE NB waveform .*/
  FTM_LTE_NB_STOP_WAVEFORM = 437,   /* Stop LTE NB waveform .*/
  FTM_LTE_NB_SET_UL_CARRIER_FREQ_OFFSET = 438,   /* Set UL carrier freq offset in LTE NB mode.*/
  FTM_LTE_NB_ENABLE_RX = 439,   /* Command to enable/disable Rx in LTE NB mode .*/
  FTM_LTE_CAL_V1_EXEC = 440,    /* Command to exec scripts when do calibration for version 1 .*/

  FTM_LTE_NS_START = 500,          /*!< 500 : Enter LTE Non-signaling mode */
  FTM_LTE_NS_ACQ,                  /*!< 501 : Start LTE Acquisition in
                                   Non-signaling mode */
  FTM_LTE_NS_START_DP,             /*!< 502 : Start LTE data path in
                                   Non-sginaling mode */
  FTM_LTE_NS_IDLE,                 /*!< 503 : Enter LTE IDLE mode in
                                   Non-signaling mode */
  FTM_LTE_NS_STOP,                 /*!< 504 : Exit LTE Non-signaling mode */
  FTM_LTE_NS_IS_CONN,              /*!< 505 : Check if Phone is connected in
                                   Non-signaling mode */
  FTM_LTE_NS_CONFIG_UL_WAVEFORM,   /*!< 506 : Explicity configure UL grant
                                   overriding DL DCI1*/
  FTM_LTE_NS_CONFIG_UL_POWER,      /*!< 507 : Explicity configure UL Power
                                   overriding TxAGC */
  FTM_LTE_NS_GET_DL_LEVEL,         /*!< 508 : Return Rx AGC and Rx Level in
                                   dB/10 for a specified reciever*/
  FTM_LTE_NS_RESET_DL_BLER_REPORT, /*!< 509 : Reset the DL BLER variables for a
                                   specified reciever*/
  FTM_LTE_NS_GET_DL_BLER_REPORT,   /*!< 510 : Reports DL BLER value for a
                                   specified reciever */
  FTM_LTE_NS_HANDOVER,             /*!< 511 : LTE NS Band/channel handover */
  FTM_LTE_CHAN_MODULATION,            /*!< 512 : LTE Set modulation type */
  FTM_LTE_SET_DAC_SETPOINT,        /*!< 513 : LTE Set DAC set point */
  FTM_LTE_GET_DAC_SETPOINT,        /*!< 514 : LTE Get DAC set point */
  /* Correcting mismatch in enum number for TDD config to what the DLL
     expects */
  FTM_LTE_NS_TDD_CONFIG = 516,           /*!< 516 : LTE NS Configure TDD SF Params*/
  FTM_LTE_GET_RX_AGC_DATA,         /*!< 517 : LTE Get RX LNA and AGC*/
  FTM_LTE_USE_FIXED_VECTOR = 518,  /*!< 518 : LTE Command to use fixed vector
                                              for tx waveform*/
  FTM_LTE_NS_ENABLE_SCELL,         /*!< 519 : LTE NS Enable SCell */
  FTM_LTE_NS_DISABLE_SCELL,        /*!< 520 : LTE NS Disable SCell */
  FTM_LTE_NS_GET_ALL_CARR_DL_BLER, /*!< 521 : LTE NS Get DL BLER for all carriers */
  FTM_LTE_NS_ENABLE_MULTI_SCELL = 522,/*!< 522 : LTE NS Enable Multiple Scell*/
  FTM_LTE_NS_DISABLE_MULTI_SCELL = 523,/*!< 523 : LTE NS Enable Multiple Scell*/

  FTM_RF_LTE_NLIC_ON_OFF = 654, /*!NLIC on off command*/

  FTM_SET_ASM_PORT = 525,          /*!< 525 : LTE Set ASM port*/
  FTM_LTE_DISABLE_SCELL = 526,     /*!< 526 : LTE Disable specified SCELL */

  FTM_LTE_RFFE_DEVICE_STATUS_UPDATE = 527,/*!< 527 : Update RFFE device registers for LTE*/
  FTM_SET_PATH_SEL_OVERRIDE_INDEX = 528,  /*!< 528 : LTE Set Split path select override index*/
  FTM_LTE_TEST_CONFIG_RX_CHAIN    = 529,
  FTM_SET_ALT_PATH_OVERRIDE_INDEX = 530,
  FTM_QUERY_HW_PATH = 531,/*!< 531 : Get HW Path Info*/

  FTM_CNCRY_MGR_CONFIGURE_DEBUG_MODE = 532, /*! < 532 - Set the debug mode flags for concurrency manager*/
  FTM_LTE_CONFIG_DECONFIG_RX_CHAIN =  533, /*!< 533 : This command is used to
                                               enable/disable one or more Rx chains
                                               for a carrier*/
  FTM_SET_ALT_PATH_VERIFICATION_INDEX = 534,

  FTM_LTE_NS_ENABLE_MULTI_SCELL_RX_TX = 535,/*!< 535 : LTE NS Enable Multiple Scell for Rx and Tx*/
  FTM_LTE_NS_DISABLE_MULTI_SCELL_RX_TX = 536,/*!< 536 : LTE NS Disable Multiple Scell for Rx and Tx*/
  FTM_LTE_NS_ACTIVE_DL_CARRIER_MASK = 537, /*!< 537 : LTE NS Active DL carrier mask*/

  FTM_LTE_SET_DEVICES = 538, /*!< 538: set device to carrier mapping for LTE in FTM mode*/
  FTM_SET_LTE_MC_WAVEFORM = 539, /*!< 539 : Configure LTE multicluster waveform */
  FTM_LTE_GET_HORXD_BAND_SUPPORT_STATUS = 540, /*!< 540: Get HORXD band support status */

  FTM_LTE_NS_RX_SELECT_CHAIN = 541, /*!< 541: LTE NS Select Rx chain request */
  FTM_LTE_NS_START_DP_ENH = 542,   /*!< 542: Start LTE data path in NS mode for HORXD */
  FTM_LTE_NS_START_DL_DP = 543,   /*!< 543: Start LTE data path in NS mode for DL only */
  FTM_LTE_NS_ENABLE_MULTI_SCELL_RX_TX_V2 = 544, /*!< 544 : LTE NS Enable MULTI SCell RX TX V2 for unequal bandwidth and HORxD */

  FTM_LTE_NS_GET_DL_LEVEL_ENH = 545, /*!< 545 : Return Rx AGC and Rx Level in dB/10 for a specified reciever*/

  FTM_LTE_NB_NS_START = 546,          /*!< 546 : Enter LTE NB Non-signaling mode */
  FTM_LTE_NB_NS_ACQ,                  /*!< 547 : Start LTE NB Acquisition in
                                                 Non-signaling mode */
  FTM_LTE_NB_NS_START_DP,             /*!< 548 : Start LTE NB data path in
                                                 Non-sginaling mode */
  FTM_LTE_NB_NS_IDLE,                 /*!< 549 : Enter LTE NB IDLE mode in
                                                 Non-signaling mode */
  FTM_LTE_NB_NS_STOP,                 /*!< 550 : Exit LTE NB Non-signaling mode */
  FTM_LTE_NB_NS_IS_CONN,              /*!< 551 : Check if Phone is connected in
                                                 LTE NB Non-signaling mode */
  FTM_LTE_NB_NS_CONFIG_UL_WAVEFORM,   /*!< 552 : Explicity configure LTE NB UL grant
                                                 overriding DL DCI1*/
  FTM_LTE_NB_NS_CONFIG_UL_POWER,      /*!< 553 : Explicity configure LTE NB UL Power
                                                 overriding TxAGC */
  FTM_LTE_NB_NS_GET_DL_LEVEL,         /*!< 554 : Return Rx AGC and Rx Level in
                                                 dB/10 for a specified reciever in LTE NB mode*/
  FTM_LTE_NB_NS_RESET_DL_BLER_REPORT, /*!< 555 : Reset the DL BLER variables for a
                                                 specified reciever in LTE NB mode*/
  FTM_LTE_NB_NS_GET_DL_BLER_REPORT,   /*!< 556 : Reports DL BLER value for a
                                                  specified reciever in LTE NB mode*/
  FTM_LTE_NS_HANDOVER_ENH = 557,      /*!< 557 : LTE NS Band/channel handover for HORXD */

  FTM_CARR_TA = 558,                  /*!< 558 : LTE NS Band/channel handover for HORXD */
  FTM_CARR_TB = 559,                  /*!< 559 : LTE NS Band/channel handover for HORXD */

  FTM_RTT_CAL = 560,                  /*!< 560 : NR5G RTT CAL */

  FTM_GET_CAGC_DUAL_CARRIER_RX_AGC = 600, /*!< 600: Get dual carrier RxAGC */
  FTM_SET_DUAL_CARRIER_DVGA_OFFSET = 601, /*!< 601 : Set DVGA offset for
                                          Dual carrier */
  FTM_GET_DUAL_CARRIER_DVGA_OFFSET = 602, /*!< 602 : Get DVGA offset for
                                          Dual carrier */
  FTM_SET_MULTI_CHAN = 603, /*!< 603 : TBD */
  FTM_RFNV_READ = 604, /*!< 604 : Perform NV read */
  FTM_RFNV_WRITE = 605, /*!< 605 : Perform RFNV write */
  FTM_DO_ICI_CAL_MULTI_CARRIER = 606, /*!< 606 : ICI calibration */

  FTM_CALIBRATION_SWEEP = 608, /*!< 608 : Calibration v3 Sweep */
  FTM_CALIBRATION_SWEEP_GET_RESULTS = 609, /*!< 609 : Calibration Sweep Get
                                           results*/
  FTM_ANT_TUNER_OVERWRITE_CMD = 610, /*!< 610 : send overwrite codeword to
                                     ant tuner device*/
  FTM_RX_WAIT_DBM = 611, /*!< 611 : Wait for RxAGC to reach the threshold */
  FTM_SET_MULTI_CARRIER_TX = 612, /*!< 612 : Enable multi-carrier Tx */
  FTM_SET_MULTI_CARRIER_TX_GAINS = 613, /*!< 613 : Set Multi-carrier Tx gain */
  FTM_RF_MODE_EXIT  = 614,          /* Forces RF driver to disable any activities and exit the current mode*/

  FTM_SET_FILTERED_PATH = 615,   /*!< 615: Forces GSM/TDS/LTE to use filtered or un-filtered path in FTM/online mode for SGLTE*/
  FTM_SET_FILTERED_PATH_CTL= 616, /*!< 616: Enables or disables MCS control for setting filtered or unfiltered path in online mode for SGLTE*/
   FTM_RF_ANT_DIV_SWITCH  = 617,

  FTM_DO_RFFE_VALIDATION = 618, /*!< 618: RFFE validation in PHY test> */
  FTM_DO_MUTIL_PATH_RFFE_VALIDATION = 619, /*!< 619: MUTIL PATH RFFE validation in PHY test> */

  FTM_DO_TX_INT_DEV_CAL = 621, /*!< 621 : Tx Internal device calibration */
  FTM_DO_RFFE_MULTI_CMD = 622, /*!< 622 : Send RFFE command for multi bytes read and write */
  FTM_DO_RFFE_CMD = 623, /*!< 623 : Send RFFE command */
  FTM_XO_CALIBRATION = 624, /*!< 624 : Perform XO calibration */
  FTM_SET_XPT_TXAGC_OVERRIDE = 625, /*!< 625 : Set TxAGC override */
  FTM_XPT_MLINE_COMPUTE = 626, /*!< 626 : Perform Mline computation */
  FTM_DO_RSB_TXLO_CAL = 627, /*!< 627 : Perform RSB TxLO cal */
  FTM_DO_GPIO_CMD = 628, /*!< 628 : Send GPIO command */
  FTM_SET_XPT_CAL_CONFIG = 629, /*!< 629 : Perform XPT calibration config */
  FTM_SET_XPT_MODE_OVERRIDE = 630, /*!< 630 : Perform XPT mode override */
  FTM_RFNV_READ_V2 = 631, /*!< 631 : Read NV */
  FTM_RFNV_WRITE_V2 = 632, /*!< 632 : Write NV */
  FTM_RFNV_GET_MAX_TRANSFER_SIZE = 633, /*!< 633 : TBD */
  FTM_RX_WAIT_DBM_V2 = 634, /*!< 634 : Wait for RxAGC to reach the threshold  */
  FTM_CALIBRATION_SWEEP_COMPRESSED = 635, /*!< Calibration Sweep - Compressed Command */
  FTM_GET_EFS_VERIFICATION_DATA = 636, /*!< RF CARD data for EFS validation */
  FTM_DO_FBRX_LM_TEST = 637, /*!< FBRx Link Manager Test */
  FTM_GET_MULTI_CAR_DVGA_OFFSET = 638, /*!< 637 : Perform DVGA calibration to return DVGA offset per carrier */
  FTM_SET_MULTI_CAR_DVGA_OFFSET = 639, /*!< 638 : Set the DVGA offset value for the  given carrier */
  FTM_FBRX_ENTER_MODE = 640, /*!< 640: FBRx Enter mode Test */
  FTM_FBRX_INIT_TX = 641, /*!< 641: FBRx Init Tx Test */
  FTM_FBRX_ENABLE = 642, /*!< 642: FBRx Enable Test */
  FTM_FBRX_EXIT = 643, /*!< 643: FBRx Exit mode Test */
  FTM_FBRX_ACTIVATE_CHAIN = 644, /*!< 644: FBRx Activate Chain */
  FTM_FBRX_BRIDGE_CONFIG = 645, /*!< 645: FBRx Bridge Config */
  FTM_FBRX_SINGLE_TRIGGER = 646, /*!< 646: FBRx Trigger single capture and processing */
  FTM_FBRX_ENABLE_RF = 647, /*!< 647: FBRx Enable RF Device */
  FTM_FBRX_STORE_RESULTS = 648, /*!< 648: FBRx Store Results */
  FTM_DO_ENH_INTERNAL_DEVICE_CAL = 649, /*!<Enhanced INTERNAL CAL, including FB Droop, FB Gain, FB Group Delay, DC Cal, IP2, FB RSB Cal */
  FTM_FBRX_SET_PARAMS = 650, /*!< 650: FBRx Set Params */
  FTM_FBRX_RUN_CONTROL = 651, /*!< 650: FBRx Run Continuous Operation */
  FTM_RF_GET_TXLUT_INFO_FOR_RED_CAL = 652, /*!< 652: Get TX LUT info for Reduced cal.*/
  FTM_GET_STATIC_INFO = 653, /*!< 653: Return PA switchpoints for a given tech*/
  FTM_FBRX_DEBUG_GET_SAMPLE_CAPTURES = 654, /*!< FBRx Debug Get Samples*/
  FTM_GET_RF_PATH_INFORMATION = 655, /*!< Get RF Path information for calibration */
  FTM_FBRX_SET_DEBUG_CONTROL = 656, /*!< 656: FBRx debug control */
  FTM_STG_COMMON_CONTROL = 657, /*! < 657 - Enable/Disable the STG */
  FTM_NLIC_CHAR_PARAMS_OVERRIDE = 658, /*!< 658: Param overrides for NLIC char */
  FTM_NLIC_CHAR_GET_MEAS_RESULTS = 659, /*!< 659: Fetch NLIC char results */
  FTM_NLIC_TEST = 660, /*!< 660: Test NLIC RF implementation in FTM mode */
  FTM_SET_XPT_TXAGC_OVERRIDE_V2 = 661, /*!< 661 : Set TxAGC override v2*/
  FTM_NLIC_CHAR_MODE_ENTER = 662, /*!< 662: Set NLIC CHAR FTM mode */
  FTM_FBRX_ENABLE_CRASH_TO_DEBUG_THERM = 663, /*!< 663: FBRx Therm debug */
  FTM_IIP2_CAL_SINGLE_TRIG = 664, /*!< 664: IIP2 Single Trigger API for LTE */
  FTM_LTE_IDC_CAL = 665, /*!< 665: IDC CAL for LTE */
  FTM_REFLOG_DEBUG_CAPTURE = 666, /*!< 666: Reflog Debug Captures */
  FTM_V2X_TX_FAULT_DETECTION =667,
  FTM_RFLM_DBG_CMD = 690, /*!< 690: RFLM debug command */
  FTM_LTE_EXIT     = 691,
  FTM_LTE_ELNA_CTRL = 692,
  FTM_LTE_CONFIGURE_DEBUG_MODE = 693, /*!< 693: RF LTE configure debug flags command */
  FTM_LTE_RF_CONFIG = 694, /*!< 694: RF LTE configure all carriers and paths to requested configuration */
  FTM_LTE_DEVICE_STATUS_READ = 695, /*!< 695: RF LTE device status read */
  FTM_LTE_DEVICE_STATUS_READ_ENABLE = 696, /*!< 696: RF LTE device status read enable/disable */
  FTM_LTE_ENABLE_RXAGC_TIMER = 697,        /*!< 697: Enable RxAGC timer for FTM mode */
  FTM_LTE_DISABLE_RXAGC_TIMER = 698,       /*!< 698: Disable RxAGC timer for FTM mode */
  FTM_LTE_RF_SAWLESS_CONTROL = 699, /*!< 694: RF LTE Sawless Control */

  /*TDSCDMA cmds, owns 700 to 799*/
  FTM_TDSCDMA_SET_TXAGC = 700, /*!< 700 : Set TxAGC */
  FTM_TDSCDMA_TXAGC_SWEEP = 701, /*!< 701 : Perform TxAGC Sweep */
  FTM_TDSCDMA_TPC_CONTROL = 702, /*!< 702 : TBD */
  FTM_TDSCDMA_IQ_LOGGING = 703, /*!< 703 : Perform IQ logging */
  FTM_TDSCDMA_SET_TX_CONTINOUS = 704, /*!< 704 : TBD */
  FTM_TDSCDMA_GET_TX_SYNTH_STATE = 705, /*!< 705 : Get Tx synth state */
  FTM_TDSCDMA_GET_RX_SYNTH_WORD = 706, /*!< 706 : Get Rx synth word */
  FTM_TDSCDMA_GET_TX_SYNTH_WORD = 707, /*!< 707 : Get Tx synth word */
  FTM_TDSCDMA_SLOT_CONFIG_OVERRIDE = 708, /*!< 708 : TBD */
  FTM_TDSCDMA_CALV3_CONFIGURE_RADIO = 709, /*!< 709 : Configure Radio */
  FTM_TDSCDMA_CALV3_RETUNE_RADIO   = 710, /*!< 710 : Retune radio */
  FTM_TDSCDMA_CALV3_TEAR_DOWN_RADIO = 711, /*!< 711 : Tear down radio */
  FTM_TDSCDMA_SET_BURST_TX_PARAMS = 712, /*!< 712 : TBD */
  FTM_TDSCDMA_BURST_TX = 713, /*!< 713 :  */
  FTM_TDSCDMA_BURST_RX = 714, /*!< 714 :  */
  FTM_TDSCDMA_SWITCH_TX_WAVEFORM = 715, /*!< 715 : */
  FTM_TDSCDMA_TX_SELF_CAL_ENABLE = 716, /*!< 716 : */
  FTM_TDSCDMA_TX_SELF_CAL_DISABLE = 717, /*!< 717 : */
  FTM_DO_FBRX_RSB_CAL = 718, /*!< 718 : Perform RSB cal for FB Rx */
  FTM_TDSCDMA_SET_ALT_PATH = 719,/*!< 719 : */
  FTM_TDSCDMA_TRIGGER_PORT_SWITCH = 720, /*!< 719 : Trigger front end port switch */
  FTM_TDSCDMA_SET_RX_LIN_STATE = 721, /*set sawless HL/LL RX lin state*/
  FTM_TDSCDMA_SET_DEVICES = 722, /*!< 722 : Dynamically set devices */
  FTM_TDSCDMA_FBRX_SET_RX_TX_DELAY = 723, /*!< 723: Dynamically change FBRx RX TX relative delay for TDS */
  FTM_TDSCDMA_DEVICE_STATUS_READ = 724, /*!< 724: RF TDSCDDMA device status read */
  FTM_TDSCDMA_DEVICE_STATUS_READ_ENABLE = 725, /*!< 725: RF TDSCDMA device status read enable/disable */
  FTM_TDSCDMA_DEG_PA_CTL = 726, /*!< 726 : PA_CTL override */
  FTM_TDSCDMA_DEG_ASM_CTL = 727, /*!< 727 : ASM_CTL override */
  FTM_TDSCDMA_LIVE_DEG = 728, /*!< 728 : Live debug flag */

  #ifdef FTM_ERROR_FATAL_FEATURE_DEFINED
  FTM_ERR_FATAL_COMMAND = 610, /*!< To crash the phone when received*/
  #endif

  FTM_GET_ENH_XO_THERM = 800,  /*!< Command to read thermistor for XO,
                              PMIC or extern PA */

  /*--------------------------------------------------------------------------*/
  /*Qtuner FTM commands*/
  FTM_PROGRAM_QFE_SCENARIO = 849,        /*!< 849 : Program Qtuner scenario */
  FTM_SET_QFE_SCENARIO = 850,            /*!< 850 : Set Qtuner scenario */
  FTM_SET_QFE_HDET_GAIN_STATE = 851,     /*!< 851 : Set Qtuner HDET Gain
                                         State */
  FTM_GET_QFE_HDET_DC_OFFSET_CAL = 852,  /*!< 852 : Do Qtuner HDET DC offset
                                         Cal */
  FTM_GET_QFE_K0_SENSOR_CAL = 853,       /*!< 853 : Do Qtuner K0 sensor Cal */
  FTM_DO_QFE_R_TUNER_CAL = 854,          /*!< 854 : Do Qtuner R_tuner Cal */
  FTM_GET_QFE_HDET_INCIDENT_PWR = 855,   /*!< 855 : Get Qtuner HDET incident
                                         power */
  FTM_GET_QFE_HDET_REFLECTED_PWR = 856,  /*!< 856 : Get Qtuner HDET reflected
                                         power */
  FTM_GET_QFE_K_SENSOR_VAL = 857,        /*!< 857 : Get Qtuner K sensor value */

  FTM_ACTIVATE_FTM_MODE = 858,           /*!< 858 : Activate FTM mode */
  FTM_DEACTIVATE_FTM_MODE = 859,         /*!< 859 : Deactivate FTM mode */

  FTM_GSM_GET_ENV_GAIN_NV_DATA = 860,    /*!< 860 : Get Value of GSM env
                                         gain NV */

  FTM_SET_TUNER_TUNE_CODE_OVERRIDE = 861,   /*!< 861 : */

  FTM_GET_QFE_HDET_POWER = 862,   /*!< 862 : */

  FTM_GET_RFFE_DEVICE_INFO = 863, /*!< 863 : Get RFFE info: mfg_id, prd_id and
                                  rev_id*/

  FTM_GSM_FRAME_SWEEP_CFG_CMD = 864, /*!< 864 : */

  FTM_GSM_FRAME_SWEEP_EXEC_CMD = 865, /*!< 865 : */

  FTM_GSM_FRAME_SWEEP_GET_RESULTS_CMD = 866, /*!< 866 : */

  FTM_GSM_FRAME_SWEEP_CLEANUP_CMD = 867,  /*!< 867 */

  FTM_WCDMA_SLEEP = 868,               /*!< 868 : Call RFWCDMA MC Wakeup */
  FTM_WCDMA_WAKEUP = 869,              /*!< 869 : Call RFWCDMA MC Sleep */
  FTM_WCDMA_PRETUNE_TO_CHAN = 870,     /*!< 870 :Call RFWCDMA MC Pretune
                                       to Chan */

  FTM_MEAS_WTOW_TEST_BUILD_SCRIPTS_V2 = 871, /*!< 871 - IRAT WTOW Build scripts for
                                             Preload and Trigger */

  FTM_MEAS_WTOW_TEST_WRITE_SCRIPTS_V2 = 872, /*!< 872 - Tune either home or neighbour by
                                             writing scripts */
  FTM_ENABLE_SEND_SCRIPT = 873,/*!< enable the scripts for various events */

  FTM_GSM_FRAME_SWEEP_CFG_EXEC_CMD = 874, /*!< 874 : */


  FTM_IQ_CAPTURE_GET_CONFIG = 900 , /*!< 900:  */
  FTM_IQ_CAPTURE_ACQUIRE_SAMPLES = 901, /*!< 901 :  */
  FTM_IQ_CAPTURE_GET_SAMPLES = 902, /*!< 902 : */
  FTM_IQ_CAPTURE_ANALYZE_SAMPLES = 903, /*!< 903 : */
  FTM_DO_TUNER_SELF_CAL = 904, /*!< 904 : */

  FTM_TUNER_RL_CMD = 905, /*!< 905 : */
  FTM_CONFIG_TX_RX = 910,
  FTM_RF_MEAS_TX_PARAMS = 911,            /*!< 911 : */
  FTM_RF_MEAS_FBRX_NOISE = 912,            /*!< 912 : */
  FTM_FBRX_IQ_CAPTURE_TRIGGER = 913,      /*!< 913 : */
  FTM_FBRX_IQ_CAPTURE_GET_SAMPLES = 914,  /*!< 914 : */
  FTM_SET_FB_PATH_STATE  = 915,  /*!< 915 : */

  FTM_RXLM_EXECUTE_REG_DUMP = 916,  /*!< 916 : */
  FTM_TXLM_EXECUTE_REG_DUMP = 917,  /*!< 917 : */

  FTM_RXLM_ALLOCATE_BUFFER = 918,  /*!< 918 : */
  FTM_RXLM_UPDATE_STATIC_SETTINGS = 919,  /*!< 919 : */
  FTM_RXLM_UPDATE_DYNAMIC_SETTINGS = 920,  /*!< 920 : */
  FTM_RXLM_CONFIGURE_CHAIN = 921,  /*!< 921 : */
  FTM_RXLM_DEALLOCATE_BUFFER = 922,  /*!< 922 : */

  FTM_TXLM_ALLOCATE_BUFFER = 923,  /*!< 923 : */
  FTM_TXLM_UPDATE_STATIC_SETTINGS = 924,  /*!< 924 : */
  FTM_TXLM_UPDATE_DYNAMIC_SETTINGS = 925,  /*!< 925 : */
  FTM_TXLM_CONFIGURE_CHAIN = 926,  /*!< 926 : */
  FTM_TXLM_DEALLOCATE_BUFFER = 927,  /*!< 927 : */

  FTM_CONFIGURE_BRIDGE = 928,  /*!< 928 : */
  FTM_STREAM_DATA = 929,  /*!< 929 : */
  FTM_GSM_SET_PA_BIAS_NV = 930, /*!< 930: */
  FTM_FEEDBACK_SETUP = 931, /*!< 931: Set FB Path for RX AGC*/

  FTM_SET_MSM_NOTCH_FILTER            = 932, /*!< 932: Configure RXFE Notch
                                               filter settings in FTM SM*/
  FTM_APPLY_MSM_NOTCH_FILTER_SETTINGS = 933, /*!< 932: Apply RXFE Notch filter
                                               Settings to modem registers */
  FTM_RESET_MSM_NOTCH_FILTER          = 934, /*!< 932: Reset RXFE Notch
                                               filter settings in FTM SM*/
  FTM_WCDMA_GET_RX_LEVEL_DBM_X2W_MEAS = 935, /*!< 935 : Get RxAGC in dBm during X2W measurement*/

  FTM_GET_RFFE_DEVICE_SETTING = 936, /*!< 936 : Get RFFE device settings */

  FTM_OVERRIDE_TRM_DEVICE_MAPPING = 937, /*! <937: Override TRM device mapping for SV/Non-SV in CDMA */

  FTM_RF_SET_SELFTEST_MODE = 938, /*!< 938: Set Self Test Mode before running any Self-Test Processing,
                                            required for FW to load a different DTR-IB map */
  FTM_TXRX_AGC_RELOAD = 939,/*TX-RX AGC Reload*/

  FTM_READ_DEVICE_STATUS = 940, /*940: Command to read Device Status registers*/

  FTM_READ_DEVICE_STATUS_ENABLE = 941, /*941: Command to ENABLE read Device Status*/

  FTM_READ_DEVICE_STATUS_DISABLE = 942, /*942: Command to DISABLE read Device Status*/

  FTM_READ_GRFC_DEVICE_STATUS_ENABLE = 943, /*943: Command to ENABLE Read GRFC Device Status*/

  FTM_READ_GRFC_DEVICE_STATUS_DISABLE = 944, /*944: Command to DISABLE Read GRFC Device Status*/

  FTM_TXLM_MSB_CAL_CODE_DUMP = 945, /*Command to dump REGARRAY containing tx msb cal code*/

  FTM_TXLM_MSB_CAL_CODE_RELOAD = 946, /*Command to re-load REGARRAY by reading back from NV*/

  FTM_IQ_CAPTURE_ACQUIRE_PRXDRX_SAMPLES = 947,

  FTM_IQ_CAPTURE_GET_PRXDRX_SAMPLES = 948,

  FTM_GET_RFFE_DEVICE_IDS = 949, /*Ehanced command to get RFFE info: mfg_id, prd_id and rev_id */

  FTM_DRDS_RECONFIG_ENABLE = 950,

  FTM_SET_DGLNA_ATTRIBUTES = 951,/*Settings the DGLNA flag and other parameters*/

  FTM_GET_TUNER_CL_ALGO_STATE = 952,   /*!< 952 : */

  FTM_GET_TUNE_CODE = 953,   /*!< 952 : */

  FTM_DEVICE_MANAGER_MODE = 954, /*Bypass FTM Device Manager mapping*/

  FTM_COMMON_PRINT_MESSAGE = 969, /*Command to print generic message*/

  FTM_CALIBRATION_SWEEP_GET_RESULTS_COMPRESSED = 970,  /*Command to compress response packet sent back to PC */

  FTM_GSM_SET_QPOET_MODE_BIAS, /*!< 971 : */

  /* QLNK FTM cmds, from 972 to 980*/
  FTM_QLNK_SELF_TEST        = 972,  /*!< 972 : QLNK self loop back */
  FTM_QLNK_COLD_START       = 973,  /*!< 973 : QLNK COLD start to LS/HS mode */
  FTM_QLNK_WARM_START       = 974,  /*!< 974 : QLNK WARM start */
  FTM_QLNK_HIBERNATE        = 975,  /*!< 975 : QLNK Hibernate/Stop */
  FTM_QLNK_PING_TEST        = 976,  /*!< 976 : QLNK PING Test */
  FTM_QLNK_REG_RW           = 977,  /*!< 977 : QLNK Register Read/Write */
  FTM_QLNK_BER_TEST         = 978,  /*!< 978 : QLNK error statistics */
  FTM_QLNK_WTR_RW           = 979,  /*!< 979 : QLNK WTR single Write/Read */
  FTM_QLNK_DO_WTR_SCRIPT    = 980,  /*!< 980 : QLNK WTR Script Write/Read */

  FTM_GSM_FBRX_CAL          = 981,  /*!< 981 : To be consistent with customer Docs on branch*/
  FTM_SELFTEST_OVERRIDE_GAIN_STAGE   = 982, /*!< 982 :To set Selftest FBRX gain switch point*/
  FTM_QLNK_READ_CDR_CODES    = 983,  /*!< 983 : QLNK CRD codesRead */
  FTM_QLNK_GET_POLICY        = 984,  /*!< 984 : QLINK get current policy applied*/
  FTM_QLNK_SET_POLICY        = 985,  /*!< 985 : QLINK update qsleep policy setting*/
  FTM_QLNK_FETCH_POLICY      = 986,  /*!< 986 : QLINK fetch qsleep policy setting*/
  FTM_QLNK_LLC_WRITE         = 987,  /*!< 987 : QLINK Comprehensive LLC tests */
  FTM_QLNK_GEARSW_TEST         = 988,  /*!< 988 : QLNK GEAR SWITCH TEST */

  FTM_QLNK_CMD_END           = 999,  /*!< 979 : End of QLNK cmd*/

  FTM_GET_RFFE_DEVICE_INFO_V2 = 1000, /*!< 1000 : Get RFFE info: mfg_id, prd_id and
                                   rev_id*/
  /*Detune FTM commands*/
  FTM_PROGRAM_TUNER      = 1001, /*!< 1001 : To program the tuners */
  FTM_GET_UNIFIED_ENH_THERM_V2 = 1002, /*!< 165 : Generic command to get Therm read
                                            for a given tech on a sig_path and rfm_device*/
  FTM_GET_TUNER_STATUS         = 1003, /*!< 1003 : To get the tune status for all active & inactive tuners */
  FTM_ASSIGN_RADIO_ALLOCATION  = 1004, /*!< 1004 : ASSIGN RADIO ALLOCATIONS command */
  FTM_FEM_COMMON_DISPATCH      = 1005, /*!< 1005 : Dispatch Front-End Module Common Commands */
  FTM_GET_HW_INDEX             = 1006, /*!< 1006 : GET HW INDEX */

  FTM_QLNK_TEST_SETUP          = 1007,  /*!< 1007 : QLNK test setup */
  FTM_QLNK_TEST_END            = 1008,  /*!< 1008 : QLNK test end */
  FTM_QLNK_ENUM_CYCLE          = 1009,  /*!< 1009 : QLNK enum cycle */
  FTM_QLNK_STATE_TRANSIT       = 1010,  /*!< 1010 : QLNK state transit */
  FTM_QLNK_PEAK_TPUT           = 1011,  /*!< 1011 : QLNK peak througput */
  FTM_QLNK_ENUM_COLD_START     = 1012,  /*!< 1012 : QLNK cold start */
  FTM_QLNK_GET_TERM_REG        = 1013,  /*!< 1013 : QLNK get register for termination settings  */
  FTM_QLNK_INTERNAL_TEST       = 1013,  /*!< 1013 : QLNK FTM internal test. It doesn't co-exist with GET_TERM_TEST on the same target. Reuse it */
  FTM_QLNK_EYE_PLOT_INFINITE_START   = 1014,  /*!< 1014 : QLNK eye plot infinite mode start */
  FTM_QLNK_EYE_PLOT_INFINITE_STOP    = 1015,  /*!< 1015 : QLNK eye plot infinite mode stop */
  FTM_QLNK_RESERVED_4          = 1016,  /*!< 1016 : QLNK reserved */
  FTM_QLNK_RESERVED_5          = 1017,  /*!< 1017 : QLNK reserved */
  FTM_QLNK_RESERVED_6          = 1018,  /*!< 1019 : QLNK reserved */

  FTM_SET_TRM_ANT_PORT_STATE   = 1019, /*!< 1019 : Set the antenna port state in TRM layer */
  FTM_GET_TRM_ANT_PORT_STATE   = 1020, /*!< 1020 : Get the antenna port state in TRM layer */

  FTM_GET_S11_MAG              = 1021, /*!< 1021:  Get s11_mag vswr info */
  FTM_LTE_RF_CONFIG_SUB2       = 1022, /*!< 1022 : Perform LTE RF CONFIG on SUB2 */
  FTM_LTE_SLEEP_SUB2           = 1023, /*!< 1023 : Perform LTE sleep on SUB2     */
  FTM_LTE_WAKEUP_SUB2          = 1024, /*!< 1024 : Perform LTE wakeup on SUB2    */
  FTM_LTE_EXIT_SUB2            = 1025, /*!< 1025 : Perform LTE EXIT on SUB2      */

  FTM_SET_SAWLESS_STATE        = 1026, /*!< 1026 : Set the SAWLESS state in TRM layer */
  FTM_GET_SAWLESS_STATE        = 1027, /*!< 1027 : Get the SAWLESS state in TRM layer */
  FTM_TRIG_VSWR_TEST           = 1028, /*!<1028: trigger vswr test */

  FTM_TCXOMGR_THERM_READ       = 1029, /*!< 1029 : Read TCXOMGR Therm */

  FTM_OVERRIDE_TX_MTPL_PWR     = 1030, /*!< 1030 : Override TX MTPL PWR */

  FTM_ANTSW_DISPATCH           = 1031, /*!< 1031 : Antenna Switch Command dispatcher */

  FTM_SWITCH_BAND_SETTINGS     = 1032, /*!< 1032 : Switch the band settings */

  FTM_GET_RTC_ECM_TIMESTAMP    = 1033, /*!< 1033 : GET ECM Timestamp */

  FTM_SET_RFFE_X1_DEBUG_CLOCK_MODE = 1034, /*!< 1034 : SET RFFE X1 DEBUG CLOCK MODE */

  FTM_GET_UNIFIED_ENH_THERM_V3 = 1035,  /*!< 1035 : NEW THERM V3 API to read thermistor */

  FTM_LTE_USLEEP_ENTER         = 1036, /*!< 1036 : LTE Usleep Enter Command*/

  FTM_LTE_USLEEP_EXIT          = 1037, /*!< 1037 : LTE Usleep Exit Command*/

  FTM_LTE_USLEEP_SEQ           = 1038, /*!< 1038 : LTE Usleep Seq Command*/

  FTM_COMMON_IDC_CAL           = 1039, /*!< 1039: Tech Independent IDC CAL Command*/

  FTM_RFC_DEBUG_COMMON_DISPATCH = 1040, /*!< 1040 : Dispatch RFC DEBUG common Commands*/

  FTM_LTE_TX_FED_DBG_FLAG_CTRL  = 1041, /*!< 1041 : Dispatch Control Cmd to en/dis LTE TxAGC DbgEFS Flag  */

  FTM_GET_QFPROM_REV            = 1042, /*!< 1042 : Get QFPROM Revision */

  FTM_SET_LTE_ENDC_AMPR_PARAMS  = 1043, /*!< 1043 : Set LTE ENDC ampr pramas */

  FTM_SET_LTE_ARD_CMD  = 1044, /*!< 1044 : Set LTE ARD Cmd*/

  FTM_LTE_LPM_ENTER            = 1045, /*!< 1045 : LTE LPM Enter Command*/

  FTM_LTE_LPM_EXIT             = 1046, /*!< 1046 : LTE LPM Enter Command*/

  FTM_REFLOG_CAPTURE           = 1047, /*!< 1046 : Reflog Capture Command*/

  //CDRX_SLEEP
  FTM_LTE_CDRX_SLEEP            = 1048, /*!< 1048 : LTE 1ST SUB CDRX SLEEP Cmd*/
  FTM_LTE_CDRX_WAKEUP           = 1049, /*!< 1049 : LTE 1ST SUB CDRX WAKEUP Cmd*/
  FTM_LTE_CDRX_SLEEP_SUB2       = 1050, /*!< 1050 : LTE 2ST SUB CDRX SLEEP Cmd*/
  FTM_LTE_CDRX_WAKEUP_SUB2      = 1051, /*!< 1051 : LTE 2ST SUB CDRX WAKEUP Cmd*/

  FTM_QLINK_QXI_READ_VALUE      = 1052,/*!< 1052 : LTE read qlink qxi address Cmd*/
  FTM_RX_GAIN_RANGE_CAL_V2      = 1053, /*!< 1053 : FTM GSM RX GAIN RANGE and WBDC Calibration */

  FTM_GSM_REFLOG_ENABLE         = 1054, /*!< 1054 : GSM standalone reflog Cmd*/
  FTM_ENABLE_DPD_LUT_READBACK   = 1055, /*!< 1055 : GSM DPD lut read back Cmd*/

  FTM_TEST_DATAMOVER            = 1056, /*!< 1056 : Datamover test command */

  FTM_RFGSM_INJECT_COARSE_TAU   = 1057, /*!< 1057 : COARSE tau injection command */
  FTM_RFGSM_INJECT_FINE_TAU_i   = 1058, /*!< 1058 : Fine tau i injection command */
  FTM_RFGSM_INJECT_FINE_TAU_q	  = 1059, /*!< 1059 : Fine tau i injection command */

  FTM_GET_FBRX_MEAS_PWR         = 1060, /*!< 1060 : Get FBRx Power for all techs command */
  FTM_GET_VSWR                  = 1061, /*!< 1061: Get VSWR Measurements */

//CV2X FAST AGC
  FTM_LTE_GET_CV2X_FAST_AGC_LOGGING = 1062, /*!< 1062 : CV2X GET DEBUG LOGGING Command*/
  FTM_LTE_START_CV2X_FAST_AGC       = 1063,   /*!< 1063 : CV2X INIT FAST AGC Command*/

//GSM spur notch
  FTM_RFGSM_INJECT_SPUR_FREQ_INJECTION_ENABLE  = 1064, /*!< 1064 : GSM spur-notch injection enable*/
  FTM_RFGSM_SPUR_OFFSET_FREQ_IN_KHZ            = 1065, /*!< 1065 : GSM spur offset in KHz*/
  FTM_RFGSM_SPUR_NOTCH_DEPTH                   = 1066, /*!< 1066 : GSM notch depth */
  FTM_RFGSM_SPUR_OFFSET_POLARITY               = 1067, /*!< 1067 : GSM spur frequency offset polarity*/
  FTM_RFGSM_INJECT_SPUR_FREQ_INJECTION_DISABLE = 1068, /*!< 1068 : GSM spur-notch injection disable*/

  FTM_OVERRIDE_TX_MTPL_PWR_V2       = 1069, /*!< 1069 : Override TX MTPL PWR V2 */

  FTM_SET_TRM_TECH_ANT_PORT_STATE   = 1070, /*!< 1070 : Set the antenna port state for each tech in TRM layer */
  FTM_GET_TRM_TECH_ANT_PORT_STATE   = 1071, /*!< 1071 : Get the antenna port state for each tech in TRM layer */

  FTM_LOAD_RF_NV_V2 = 1072, /*!< 1072 : Reload RFNV items from EFS to local data structure */

  FTM_SET_GSM_SAMPLE_UPDATE 	= 1073, /*!< 1073 : Overwrite update sample index and NSAMPLE */

  FTM_M_SF_DBG = 1074, /*!< 1074 : */

  FTM_GSM_FBRX_DISABLE = 1075, /*< 1075 FTM command to disbale/enable the GSM FBRX on TX path */

  FTM_DEBUG_DUMP_RF_MEM = 1076, /*!< 1076 :  Dump RFM init and NV heap usage log */

  FTM_SET_RFMEAS_PARAMS_CRASH	  = 1077, /*!< 1077 : Update rfmeas params to crash phone in rfmeas layer for debug*/

  FTM_LTE_CDTA_CONFIG  = 1078, /*!< 1078 : CDTA Command */

  FTM_SELFTEST_SET_DEBUG_CONFIG = 1079, /*< 1079 FTM command to support selftest customized crash */

  FTM_OVERRIDE_TX_MTPL_PWR_SRS     = 1080, /*!< 1080 : Override TX MTPL PWR SRS */

  FTM_LTE_RX_DTR_CONFIG = 1081, /*< 1081 FTM command to support LTE Rx DTR activate/deactivate */

  FTM_TRIGGER_SAR_SET_STATE        = 1082, /*!< ftm trigger sar state update*/

  FTM_DEVICE_PAPM_GET_SCRN_RESULT  = 1083, /*!< ftm cmd to get device papm screen result*/

  FTM_RF_CAL_DEBUG_CONFIG = 1084, /*!< ftm cmd to config RF cal crash conditions*/

  FTM_RFCARD_HW_INFO_READ = 1085, /*!< ftm cmd to read hwid and bid from eeprom*/

  FTM_UPDATE_SYM_LEVEL_CFG  = 1086,  /*!< ftm cmd to update symbol level SRS configuration*/

  FTM_CHANGE_EXTRACTOR_MODE = 1087, /*!< ftm cmd to inform RFLM to change the extractor mode */

  FTM_LTE_APM_CMD  = 1088, /*!< 1044 : LTE APM Cmd for APM mode switch*/

  FTM_MMW_IF_LOOPBACK_TEST  = 1089, /*!< ftm cmd to perform IF loopback test*/

  /* Note: 1090 to 1098 are free to use */
  FTM_TRIGGER_LNA_PROTECTION = 1099, /* FR76564 -- Internal Use Only */

//NR5G commands 1100 above

  FTM_NR5G_RF_TRX_MMW_MODULE_CHECK  = 1100,     /*!< 1100 : mmw beamer stats */

  FTM_NR5G_RF_TRX_MMW_WIF_CRC_STATS  = 1101,    /*!< 1101 : mmW RF TRX WIF CRC relability stats */

  FTM_NR5G_RF_TRX_MMW_IF_PLL_LOCK_CHECK = 1102, /*!< 1102 : mmW RF TRX IF PLL LOCK check */

    /*!< 1103 : Set mmW RF TRX PER Test Input Params */
  FTM_NR5G_RF_TRX_MMW_SET_COLD_WARM_BOOT_PER_TEST_INPUT_PARAMS  = 1103,

  /*!< 1104 : GET mmW RF TRX COLDBOOT/WARMBOOT PER Test Result */
  FTM_NR5G_RF_TRX_MMW_GET_COLD_WARM_BOOT_PER_TEST_RESULTS  = 1104,

  /*!< 1105 : GET mmW RF TRX Execute steady state PER Test Internal*/
  FTM_NR5G_RF_TRX_MMW_EXECUTE_STEADY_STATE_PER_TEST = 1105,

  /*!< 1106 : GET mmW RF TRX Execute WIF RELIABILITY PER Test for Customer */
  FTM_NR5G_RF_TRX_MMW_EXECUTE_WIF_RELIABILITY_PER_TEST = 1106,
  
  /*!< 1107 : Enter FW<->RF test mode for internal use */
  FW_RF_TEST_MODE_ENTER = 1107,
  
  /*!< 1108 : Exit FW<->RF test mode for internal use */
  FW_RF_TEST_MODE_EXIT = 1108,

  /*!< 1109 : Get WTR info: WTR number, each WTR version*/
  FTM_GET_WTR_INFO = 1109,  
   
  /*!< 1109 : ftm q6 flag for activating Q6,IU and FW<->RF test mode call flows */
  FTM_Q6_FLAG_SET = 1110,
   
  /*1200 - 1209 reserved for RFCAL*/
  FTM_RFCAL_CONFIG = 1200,

  /*Serialize Interface using protobuffers*/
  FTM_RFCAL_CONFIG_MMW_PROTOBUFFER   = 1201,
  FTM_RFCAL_CONFIG_SUB6_PROTOBUFFER  = 1202,
  
  FTM_NVMGR_TEST = 1203,
  FTM_RFCAL_CONFIG_SUB6 = 1204,
  
  /*1250 - 1270 reserved for FTM VM*/
  FTM_VM_DAP_STATUS = 1250,
  FTM_VM_LOAD_PGM = 1251,
  FTM_VM_DAP_BKPT = 1252,
  FTM_VM_DAP_CTRL = 1253,
  FTM_VM_DAP_MEM = 1254,
  FTM_VM_DAP_REG = 1255,
  

  // FTM_DO_RFFE_MASKED_WRITE_CMD
  FTM_DO_RFFE_MASKED_WRITE_CMD = 1271,
  FTM_DO_RFFE_DEBUG_GUI_CMD = 1272,
  FTM_DO_RFFE_DEBUG_MSG_CMD = 1273,
  FTM_DO_RFFE_CMN_DEBUG_CMD = 1274,
  

  FTM_DIAG_CMD_ID_MAX, /*!< Maximum Diag command ID */

  /*--------------------------------------------------------------------------*/
  /* Following are all "Internal Commands" Any PC commands should be above
  FTM_DIAG_CMD_ID_MAX */

  FTM_INT_TX_SWEEP_CAL_TRIGGER, /*!< Trigger one TxSweep Cal Instance */
  FTM_INT_TX_RX_FREQ_CAL_TRIGGER, /*!< Trigger one Composite Cal Instance */
  FTM_INT_SET_MODE, /*!< Command to perform Set Mode internally bypassing Diag
                    This is requried, as there should be no response to
                    Diag when this command is used. Under-lying funtionality is
                    same as FTM_SET_CMODE */
  FTM_INT_SET_CHAN, /*!< Command to perform Set Chan internally bypassing Diag
                    This is requried, as there should be no response to
                    Diag when this command is used. Under-lying funtionality is
                    same as FTM_SET_CHAN */
  FTM_INT_SET_TX_ON, /*!< Command to Enable Tx internally bypassing Diag
                    This is requried, as there should be no response to
                    Diag when this command is used. Under-lying funtionality is
                    same as FTM_SET_TX_OFF */
  FTM_INT_SET_TX_OFF, /*!< Command to Disable Tx internally bypassing Diag
                    This is requried, as there should be no response to
                    Diag when this command is used. Under-lying funtionality is
                    same as FTM_SET_TX_OFF */
  FTM_INT_COMP_CAL_LOG_COMMIT, /*!< Trigger Log Commit for CompositeCal */
  FTM_INT_SET_LOWEST_TX_PWR, /*!< Set Tx Power to lowest */

  FTM_INT_CALIBRATION_SWEEP, /*!< Calibration Sweep */
  FTM_INT_CALIBRATION_SWEEP_GET_RESULTS, /*!< Calibration Sweep Results */
  FTM_INT_PERFORM_INT_DEV_CAL,
  FTM_INT_WCDMA_CALV3_CONFIGURE_RADIO, /*!< WCDMA Cal v3 configure radio */
  FTM_INT_CALV3_RADIO_SETUP, /*!< Command sent during cal v3 sweep to
                    perform radio setup opeartion */
  FTM_INT_CALV3_DATA_PROCESS_MLINE, /*!< Process Mline data */
  FTM_INT_CALV3_FEEDBACK_SETUP, /*!< Process feedback_Setup segment */
   /* Do NOT add PC cmds here, only "Internal Commands" should be added here.
  Add PC cmds above FTM_DIAG_CMD_ID_MAX*/
  FTM_INT_CALV3_XPT_IQ_CAPTURE,
  FTM_INT_CALV3_RSB_CAL,
  FTM_INT_CALV3_DELTA_PIN_POUT,
  FTM_INT_RFNV_WRITE,/* Used by NV manager to write NV to phone. Command does not produce any response */
  FTM_INT_VSWR_TEST, /*Internal vswr_test */
  FTM_INT_RF_MODE_EXIT, /* Common mode exit */
  FTM_INT_CALV3_SEQ_CAL_PROCESS_EXEC, /* Dispatch Seq class process execute to FTm task on sequences that take too long.*/
  FTM_INT_WRITE_NV_TREE,
  FTM_INT_UPDATE_RFC_CAL_NV_TBL,
  FTM_INT_RELOAD_RFNV_V2_DELAY_RES,

  /*--------------------------------------------------------------------------*/

  FTM_ID_MAX /*!< Maximum command ID */
} ftm_common_cmd_id_type;


/*! @} */


/*============================================================================*/
/*!
  @name CDMA Command IDs

  @brief
  This section includes command IDs specific to CDMA.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Lists the FTM CDMA Calibration v2 Commands IDs for Diag Interface.
*/
typedef enum
{

  FTM_CDMA_CAL_COMMIT_NV      = 0, /*!< Comit NV */
  FTM_CDMA_CAL_DVGA           = 1, /*!< Calibrate DVGA */
  FTM_CDMA_CAL_LNA            = 2, /*!< Calibrate LNA */
  FTM_CDMA_CAL_IM2            = 3, /*!< Calibrate IM2 */
  FTM_CDMA_CAL_INTELLICEIVER  = 4  /*!< Calibrate Intelliceiver */

} ftm_cdma_cal_v2_cmd_enum;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Lists the FTM HWTC CDMA Radio Control Command IDs for Diag Interface.
*/
typedef enum
{
  /* Debug commands */
  FTM_HWTC_CDMA_DEBUG_BUS_OUT  = 0, /*!< Command to Config Debug Bus for RXF */

  FTM_HWTC_1X_SET_TX_DP_IQ_GAIN = 1, /*!< Set 1x Tx Datapath IQ Gain */

  FTM_HWTC_HDR_SET_TX_DP_IQ_GAIN = 2, /*!< Set HDR Tx Datapath IQ Gain */

  FTM_HWTC_1X_UPDATE_ICIFIR_ITEMS = 3, /*!< Update ICIFIR items for 1x */

  FTM_HWTC_HDR_UPDATE_ICIFIR_ITEMS = 4, /*!< Update ICIFIR items for HDR */

  FTM_HWTC_HDR_UPDATE_DVGA_ITEMS = 5, /*!< Update DVGA items for HDR */

  FTM_HWTC_1X_UPDATE_DPD_DEBUG_ITEMS = 6, /*!< Update DPD Debug items for 1x */

  FTM_HWTC_RXLM_UPDATE_GDA_ITEMS = 7, /*!< Update WB and NB GDA registers */

  FTM_HWTC_CDMA_MAX /*!< Maximum number of HWTC Command */

} ftm_hwtc_cdma_cmd_enum;


/*! @} */

/*============================================================================*/
/*!
  @name WCDMA Command IDs

  @brief
  This section includes command IDs specific to WCDMA.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Lists the FTM WCDMA non signalling Command IDs for Diag Interface.
*/
typedef enum
{
  FTM_WCDMA_NS_START_WCDMA_MODE_REQ = 0, /*!< Start WCDMA protocol */

  FTM_WCDMA_NS_STOP_WCDMA_MODE_REQ  = 1, /*!< Stop WCDMA protocol */

  FTM_WCDMA_NS_IDLE_REQ = 2, /*!< Move WCDMA to IDLE mode */

  FTM_WCDMA_NS_ACQUIRE_REQ = 3, /*!< Move WCDMA to ACQ mode */

  FTM_WCDMA_NS_RMC_DCH_SETUP = 4, /*!< Setup DCH channel */

  FTM_WCDMA_NS_RMC_DCH_SETUP_V2 = 5, /*!< Setup DCH channel */

  FTM_WCDMA_NS_RMC_DCH_TFCS_CONFIG_REQ = 6, /*!< COnfigure DCH TFCS */

  FTM_WCDMA_HS_CMD_BASE = 256, /*!< Starting command ID for HS NS */

  FTM_WCDMA_HS_CMD_START_HS = FTM_WCDMA_HS_CMD_BASE, /*!< Start HS */

  FTM_WCDMA_HS_CMD_STOP_HS = 257, /*!< Stop HS */

  FTM_WCDMA_HS_CMD_RECONFIG_HS = 258, /*!< Reconfig HS */

  FTM_WCDMA_HS_CMD_CONFIG_HS_PDSCH = 259, /*!< Config HS PDSCH */

  FTM_WCDMA_HS_CMD_CONFIG_HS_SCCH = 260, /*!< Config HS SCCH */

  FTM_WCDMA_HS_CMD_CONFIG_HS_DSCH = 261, /*!< Config HS DSCH */

  FTM_WCDMA_HS_CMD_CONFIG_HS_DPCCH = 262, /*!< Config HS DPCCH */

  FTM_WCDMA_HS_EVENT_STATUS_CLEAR_REQ = 263, /*!< Clear status */

  FTM_WCDMA_HS_EVENT_STATUS_REQ = 264 /*!< Get Status */

} ftm_wcdma_ns_cmd_enum;

/*! @} */

/*====================================================================================================================*/
/*!
  @name Assigned Radio Allocations Property ID

  @brief
  Assigned Radio Allocations Property ID list
*/
/*! @{ */

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Enumeration to indicate the type of properties for Assigned Radio Allocations Command.
  These enumeration are used to define the content of test command packet.
  property_names[] Must be updated when this list is updated.*/
typedef enum
{
  FTM_TRM_ARA_PROP_UNASSIGNED   = 0,  /*!< Unassigned property */
  FTM_TRM_ARA_PROP_CARRIER_IDX  = 1,  /*!< Carrier Index 0/1/2/n */
  FTM_TRM_ARA_PROP_BAND         = 2,  /*!< Band from sys_band_class_e_type values */
  FTM_TRM_ARA_PROP_RXTX         = 3,  /*!< Indicates if request is for Rx or Tx path from ftm_rf_test_device_radio_trx_t*/
  FTM_TRM_ARA_PROP_CHAIN        = 4,  /*!< Chain 0/1/2/3/n */
  FTM_TRM_ARA_PROP_DEVICE       = 5, /*!< Device from rfm_device_enum_type values */
  FTM_TRM_ARA_PROP_SIGNAL_PATH  = 6, /*!< Signal */
  FTM_TRM_ARA_PROP_ANTENNA_PATH = 7, /*!< Antenna Path */
  FTM_TRM_ARA_PROP_NUM          = 8,  /*!< DEPRECATED: This enum is kept here for legacy PL compilation and is not
                                          to be changed any more. Changing this enum cause compilation failure in
                                          legacy PL. Incrementally add new properties as usual but do not change this
                                          enum */
  FTM_TRM_ARA_PROP_PLL_ID       = 8, /*!< Pll ID */


  /* ADD MORE ITEMS ABOVE THIS LINE */
  FTM_TRM_ARA_PROP_NUM_V2  /*!< Max : Defines maximum number of properties */

} ftm_trm_ara_property_type;

/*! @} */

#ifdef __cplusplus
}
#endif

#endif /* FTM_COMMAND_ID_H */
