/*!
  @file rflte_mc.h

  @brief
  This file contains all declarations and definitions necessary to use
  the LTE Main Control.

  @details

*/

/*===========================================================================

  Copyright (c) 2009 - 2019 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

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

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/rftech_lte.mpss/2.10/api/rflte_mc.h#3 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/21/21   gk      B8_B addition(Network assigned suplex spacing)
11/11/20   gk      band mask init from bc config
06/24/19   as      MSM number of nothes reduced
06/12/19   ak      PA/ASM internal timings in RFC
03/26/19   sk      FR53265 : VBATT backoff support
03/05/19   sk      RF PAGE ONLY changes for NB/CATM
02/06/19   as      Non Anchor Rx retune support
11/01/18   as      Enable LIF in NB
10/25/18   sk      TRACK B Bringup changes
09/25/18   cv      Added Support of per-band setting of Power Class in Rel.14 Cat.M1 and NB1/NB2
07/02/18   as      Clean up LTE meas interface
09/08/17   qzh     Update RxAGC algo to use RSRP calculated RSSI for CATM1 FTM NS mode
03/28/17   as      Support for high speed ADC rate for certain EARFCNs to avoid desense due to ADC spurs 
12/21/16   qzh     Add NB1 support
12/08/16   dh      Remove 'FEATURE_RF_HAS_QTUNER' for CAT-M
12/04/16   as/qzh  Add AFC and ul hop merging related changes.
07/30/16   qzh     Add CATM1 ul hopping related changes
07/25/16   as      CATM1 Rx NB Hopping changes
06/03/16   qzh     Add CATM1 HD FDD related changes
01/07/15   kga     MC common layer API to set RX config active flag for all carriers and paths per carrier
12/15/14   ars     Added new parameter to LTS state machine to store original Pcell
                   DRx band before intraband split SCell is added
12/05/14   dw      Changes to support split bands based on split specified in RFC.
11/21/14   kab     Fix Single Rx concurrent SCC addition crash
11/17/14   kab     FTM RF Dynamic SCC Device allocation
11/14/14    pv     HORxD Feature Bring up changes.
11/12/14   bsh     Cleanup TQ assignment for all cells
11/07/14   kab     RRC BW Capability API Logic Fix
11/04/14   gvn     Re-tune Tx and Rx in case split band changes on intra scell add or delete
10/29/14   jf/dyc  Support for ASDIV xsw decoupling
10/22/14   shk     Added macro to define split band override default value
10/20/14   as/vb   Enable GRFC controlled ASDiv switch for FTM 
10/10/14   kab     Fix KW\CW 
10/08/14   kab     Changes to Skip PCC update when SCC exit from (PCC+SCC) SRx
10/03/14   kab     MC Layer Support for Single Rx 
10/02/14   kab     Support for Single Rx RXLM Config Mode Selection.
09/30/14   svi     Changes to split Rx Tune Script as separate Preload and 
                   Trigger events
08/20/14   ndb     Add Tuner_ID as part of the response in "FTM_SET_TUNER_TUNE_CODE_OVERRIDE"
09/18/14   bsh     Fix KW 
09/11/14   bsh     Support FTM EXIT command and SLEEP -> EXIT 
09/11/14   kab     Separate placeholders for PRx,Drx Bands in LTE State Machine
08/27/14   svi     Move Alternate Path State Variable to Path Specific
08/22/14   gvn     If alternate path is invalid, reset it to zero
08/21/14   svi     Added Support for Rx Config Script Creation
08/21/14   svi     Started Adding Time Profiling for MC Layer
08/18/14   kab     TxPL changes to support SGLTE Tx power backoff with Coex.
08/01/14    ndb     Added support for extended EARFCN for LTE
08/15/14    st     CA Alt Path Reconfiguration Support
08/14/14    pv     Added support for PA swp adjust mechanism.
08/04/14    pv     Added support for FBRx + GPS concurrency based scripts.
08/12/14    jf     MCL LTE support
08/01/14    ndb     Added support for extended EARFCN for LTE
08/01/14   gvn     Update state machine with correct pcell band if in split intraband case
07/30/14   svi     Notch Application based on Tx AFC - Phase 2
07/17/14   pv      Added support for call-back API for spur-mitigation based re-tune in CA mode.
07/08/14   gvn     Modify algorithm for split-intraband support 
07/02/14   jf      AsDiv support
06/20/14    pv     Added support for TDD PortA/B swap for ASM.
06/18/14   sbm     Added state machine variable to determine tx pwr ctrl mode.
06/13/14   svi     Notch Application based on Tx AFC
05/13/14   jf      Add CL/AOL support
05/08/14   svi     Assigned Values to the enums deciding if Tx FED DM is 
                   to be updated or not during Tx Config
05/07/14   svi     Tx Config Timeline optimization
05/05/14   svi     Depricated RFLTE_MC_MAX_PA_STATE
05/05/14   svi     Moved TXPLL update to Core Task
04/20/14   gvn     S-LTE Support
04/11/14   bsh     Split Band Support
03/27/14   svi     [ASDIV] Added callback function as part of antenna switch
3/26/14    kg      Added support for CL LM handle 
03/27/14   gvn     Add split band support for IRAT
03/18/14    pv     Added support for FBRx script population in DM
03/17/14   svi     Added support to get Intra Band CA support info
03/11/14   bsh     Adding NLIC support - add rx freq error in sm
01/28/14   jf      Append RGI_0 script to TX_ON off script, and trigger HW write 
                   RGI_0 at tx_config
02/04/14    pv     Added support for FBRx setup.
01/08/13   npi     Track current state of Intra-band CA
01/09/14   svi     Split Band Support : SGLTE concurrency
12/02/13   cri     Support for RFLM Tx override for LTE
11/12/13   npi     Moved cell definition to an enum
11/12/13   bsh     Delete the states related to Rx Dm
10/26/13   bsh     support for rx fed
10/09/13   nrk     Added initial DM population changes
09/16/13   bsh     Clean up state machine usage
09/13/13   svi     Code Reorganization
09/04/13   svi     Added support for Single Rx CA
08/29/13   svi     MC Layer Changes to support UL CA
08/14/13   svi     Added support for Single Tx CA
07/26/13   svi     Added support for Single Tx CA
07/29/13   as      Added support for relative backoff on SAR limit based on 
                   TDD-LTE ULDL configuration
07/24/13   gvn     RSB Cal updates on online mode, iqmc update and IRAT
07/19/13    pv     Added support for separate TDD Tx and Rx ASM scripts.
07/19/13   aro     Featurized ASDIV
07/18/13   pl      Update interface to match with common AsDIV driver
07/17/13   pl      Adding Antenna Switch Diversity Support
07/02/13   ndb     Added rfcommon_nv_ant_tuner_cl_ctrl_type() 
06/06/13    pv     Added support for multple TDD ASM scripts.
05/30/13   php     Pass only bands with Critical NVs to CM for Freq Scan
05/07/13   jf      Add multiple freq comp bins support for XPT  
05/14/13   jf      Support ET_EPT_CFG
05/02/13   vb      Tuner CA changes 
04/26/13   bar     Removed the rflte_mc_state.disable_powercollapse flag
04/23/13   kab     Enable FTM mode for NBEE in IRAT 
04/22/13   bar     Introduced MC level Pointer to store bb comp table for RB based FreqComp
03/26/13   cri     XPT cal deconfig support for LTE
03/22/13   jf      Add 2D lin_vs_temp_vs_freq_comp
03/21/13   pv      Added support for GNSS Blanking during Tx for LTE TDD only.
03/21/13   jf      Move tuner_config function to .h
03/15/13   gvn     Update sgsar, coex_WWAN, coex_WLAN limits using MSGR
03/06/13   php     Add flag to indicate NS mode  
03/06/13   gvn     L2L IRAT Half script support
03/06/13   kab     Removed rxlm_buffer_idx argument from mc_exit_mode function
02/14/13   aca     Tuner over rider support
02/11/13   jf      Download ET txlm buffer only if ET mode is calibrated
02/04/12   php     Add support for using On demand Task Queues for Wakeup
01/11/13   jf      Reuse txlm buffer for ET chain
01/04/13   gvn     Move interface to query RF LTE CA bc config from rfm to rflte_mc
12/26/12   gvn     LTE Max Pwr limiting support during WLAN Coex scenario 
                   (merge from Nikel CR401193)
12/21/12   gvn     Adding functionality for Upper layers to query LTE CA bc config
12/18/12   bsh     KW errors fixed
12/07/12   cri     Dynamic allocation of linearizer tables 
11/15/12   adk     Backed out Qtuner code
11/13/12   pl      added Intra-band CA support
11/09/12   pl      Upadte #define of MAX CELL to use common #define rflte_msg.h
11/07/12   sbm     GRFC script size to 10 and timing info in state mc.
10/24/12   ndb     Qtuner- Bug fixes
10/26/12   adk     Merged antenna tuner code from Nikel.
10/25/12   gvn     Featurize LTE to support LTE FW API removal on Triton
10/08/12   php     Move PA script update to mdsp
09/14/12   php     Populate shared memory with PA RFFE script
08/23/12   pl/sbm  remove obsolete function
08/17/12   vss     Add support for RFFE size 
08/12/12   pl      Update script buffer creation for per carrier
                   prepare for sleep/wakeup optimization
07/10/12   Saul    WCDMA. Added tune_with_lte_settings API.
05/23/12   as      LTE Tx power limit for SGLTE SAR and CoEx
05/16/12   pl      LTE Carrier Aggregation support
04/04/12   pl      Remove obsolete states and comamnds
04/15/12   jf      Return RFA_RF_LTE_INVALID_CAL_DATA to Ml1 if NV is missing 
03/20/12   aca     Implemented XO correction during Tx pll tune
02/16/12   pl      Remove offline DRX processing from RFSW. 
02/09/12   aca     Tx PLL error correction
11/17/11   aak     Changes to enable Quiet mode 
11/04/11   pl      Add interface for MC state machine query
10/17/11   pl      Support SMPS temp comp
10/12/11   pl      Support Offline DRX
06/23/11   pl      Remove obsolete functions
05/09/11   sar     Removed rf_cmd_dispatcher.h and added msgr_types.h.
04/28/11   pl      Adding support for PLL Adjustments
04/28/11   pl      Removed unused functions
02/17/11   tnt     TDD Rx/Tx LM migration
02/14/11   pl      RxLM RF/L1 Integration
02/09/11   pl      MC glabal variables clean up
01/18/11   pl      RxLM integration
01/07/11   bmg     Made header C++ compatible
01/03/11   pl      Adding State machine control helper functions
12/29/10   pl      Adding new Main Control state machine states
10/14/10   whc     Added LTE Reload RFNV functionality
07/01/10   jyu     Added support to power collapse  
06/30/10   qma     Added managing rx chain support
06/25/10   jyu     Added support to manage rx chain  
05/19/10   can     Remove unused rflte_mc_do_temp_comp function.
05/10/10    dw     Added support for LTE temp comp
04/06/10   can     Removed some functions and moved them to the MSM HAL layer. 
04/01/10   pl      Move FTM related function into rflte_ftm_mc.h/.c
03/12/10   tnt     Adding code to support GRFC for RF signals
02/18/10   qma     Added RX cal support for delta offset
02/16/10   qma     Added FTM LTE RX AGC
02/01/10   pl      update rflte_mc_init() interface to comply with rfm changes
11/20/09   can     Changes to enable L1 call exercise with Genesis 1.1. Some of these
                   changes are temporary and will need to be removed eventually.
11/17/09   qma     Added FTM enter mode error checking
11/06/09   mkv	    Moving MSGR registration and de-registration required for RF Task 
				       into LTE files.
10/14/09   can     Changes to support the L1 Interface.
10/13/09   qma     Added set tx waveform command
10/08/09   qma     Added RF LTE dynamic dispatch registration
09/08/09   qma     LTE MC TX config, disable, and set BW integration
08/28/09   qma     LTE MC RX config and set BW integration
08/19/09   qma     LTE MC enter mode, exit mode integration
07/21/09   mkv	    Added Dynamic Dispatcher registration for RF LTE L1 MC commands.
04/13/09   qma     Initial Check-in
===========================================================================*/

#ifndef RFLTE_MC_H
#define RFLTE_MC_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "comdef.h"
#include "rfcom.h"
#include "msgr_types.h"
#include "lm_types.h"
#include "rfc_common.h"
#include "rflte_msg.h"
#include "rf_hal_common.h"
#ifdef FEATURE_RF_HAS_QTUNER
#include "rfcommon_atuner_intf.h"
#include "rfcommon_asdiv_manager.h"
#endif
#include "rfcommon_core_txlin_types.h"
#include "rflte_ext_mc.h"
#include "rflte_nv.h"
#include "timetick.h"
#include "rfcommon_time_profile.h"
#include "rfdevice_lte_type_defs.h"
#include "rflm_api_lte.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/
extern boolean use_catM1_cal_data;

#define RFLTE_MC_MAX_SSBI_SCRIPT_SIZE 512
#define RFLTE_MC_MAX_RFFE_SCRIPT_SIZE 50
#define RFLTE_MC_MAX_GRFC_SCRIPT_SIZE 10
#define RFLTE_MC_MAX_RFFE_GROUP_NUM_REGS 100
#define RFLTE_MC_MAX_RFFE_GROUP_NUM_GROUPS 10

/* Number of Max MSM Notches to be applied per cell */
#define RFLTE_MC_MSM_MAX_NUM_NOTCH    1

/* Max number of split bands */
#define RFLTE_MC_MAX_NUM_SPLITS       3
/* Max number of entries in RFC SS */
#define RFLTE_MC_MAX_RFC_SPLITS (RFCOM_NUM_LTE_BWS * RFLTE_MC_MAX_NUM_SPLITS)
#define RFLTE_MC_MAX_RX_PATHS_PER_CARRIER 1

#if 0
#define RFLTE_MC_PCELL_IDX 0
#define RFLTE_MC_SCELL1_IDX 1
#define RFLTE_MC_SCELL2_IDX 2
#define RFLTE_MC_SCELL3_IDX 3
#define RFLTE_MC_SCELL4_IDX 4
#endif

/* Macro to convert us to USTMR */
#define RFLTE_MC_CONV_FACTOR_US_TO_USTMR 19.2

/* Macro to check if a particular cell index is masked in the cell mask passed */
#define RFA_LTE_IS_CARRIER_IDX_USED(idx_mask,idx) (((idx_mask & (1<<idx))>0)?TRUE:FALSE)

/* Macro to define default alternate path*/
#define RFLTE_MC_DEFAULT_ALTERNATE_PATH 0
/* Macro to define invalid Alternate Path */
#define RFLTE_MC_INVALID_ALTERNATE_PATH 255
/* Macro to define default split band path override index */
#define RFLTE_MC_DEFAULT_SPLIT_BAND_PATH_OVERRIDE 255

/* Macro to convert M_UL value to KHZ for NB1 */
#define RFLTE_MC_MUL_UINT_IN_KHZ      5

/*!
   @brief
   Enumeration for states for LTE mode.

*/
typedef enum {
  /*! @@todo Add comments here and enter/exit flow*/
  RF_STATE_INIT,
  /*! @@todo Add comments here and enter/exit flow*/
  RF_STATE_LTE_IDLE,
  /*! @@todo Add comments here and enter/exit flow*/
  RF_STATE_LTE_FDD_RX_WARMUP,
  /*! @@todo Add comments here and enter/exit flow*/
  RF_STATE_LTE_FDD_RXTX,
  /*! @@todo Add comments here and enter/exit flow*/
  RF_STATE_LTE_TDD_WARMUP,
   /*! @@todo Add comments here and enter/exit flow*/
  RF_STATE_LTE_TDD_RXTX,
  /*! @@todo Add comments here and enter/exit flow*/
  RF_STATE_LTE_SLEEP,
  /*! @@todo Add comments here and enter/exit flow*/
  RF_STATE_INVALID = 0xFF,
} rf_lte_state_enum_type;

typedef enum
{
  RFLTE_MC_CARR_TYPE_PRX,
  RFLTE_MC_CARR_TYPE_TX,
  RFLTE_MC_CARR_TYPE_RX_NUM = RFLTE_MC_CARR_TYPE_TX,
  RFLTE_MC_CARR_TYPE_NUM,
  RFLTE_MC_CARR_TYPE_MAX = RFLTE_MC_CARR_TYPE_NUM,
  RFLTE_MC_CARR_TYPE_INVALID = 0xFF,
} rflte_mc_carrier_type;

/*!
   @brief
   Enumeration for whether FED STATIC DM is to be updated
   or not during Tx Config Request.

*/
typedef enum 
{

  /*!> Enable FED STATIC DM UPDATE during Tx Config */
  RFLTE_MC_TX_FED_STATIC_DM_UPDATE_DISABLE = 0,                         /*!> FALSE */

  /*!> Disable FED STATIC DM UPDATE during Tx Config */
  RFLTE_MC_TX_FED_STATIC_DM_UPDATE_ENABLE  = 1,                          /*!> TRUE */ 

  /*!> Invalid state for FED STATIC DM UPDATE during Tx Config */
  RFLTE_MC_TX_FED_STATIC_DM_UPDATE_INVALID,  

} rf_lte_update_tx_fed_static_dm_type;

/*-----------------------------------------------------------------------*/
/*!
  @brief
  This enum denotes the script buffer that is to be requested
  from RFLTE MC layer while preparing scripts for Rx Config
  or Rx Disable
  
*/
typedef enum
{

  /*!> PRx Transceiver PRELOAD */ 
  RF_LTE_MC_PRX_TUNE_PRELOAD_TRANSCEIVER,

  /*!> PRx Transceiver TRIGGER */ 
  RF_LTE_MC_PRX_TUNE_TRIGGER_TRANSCEIVER,

  /*!> PRx Config Startup */ 
  RF_LTE_MC_PRX_TUNE_NON_TRX,

  /*!> Invalid */    
  RF_LTE_MC_RX_TUNE_INVALID,
   
} rflte_mc_rx_tune_scripts_type_s;

/*!
   @brief
   Enumeration for whether the Notch applied for a particular cell
   is config 0 or config 2.
*/
typedef enum
{

  /*! Notch was programmed for config 0 */
  RFLTE_MC_NOTCH_PROGRAMMED_CFG_0 = 0,

  /*! Notch was programmed for config 2 */
  RFLTE_MC_NOTCH_PROGRAMMED_CFG_2 = 1,

  /*! Max number of Notch configs */
  RFLTE_MC_NOTCH_PROGRAMMED_CFG_MAX,

  /*! Invalid Notch config */
  RFLTE_MC_NOTCH_PROGRAMMED_CFG_INVALID = RFLTE_MC_NOTCH_PROGRAMMED_CFG_MAX,
  
}rflte_mc_notch_programmed_config_type;

/*!
   @brief
   Enumeration for which type of Spur the Notch was applied for 
   Can be a NV Spur or a WTR spur
*/
typedef enum
{
  /*! Spur type mitigated by Notch was NV Spur */
  RFLTE_MC_NV_SPUR_MITIGATED  = 0,
  
  /*! Spur type mitigated by Notch was WTR Spur */  
  RFLTE_MC_WTR_SPUR_MITIGATED = 1,
  
  /*! Max types of Spur mitigated */  
  RFLTE_MC_MAX_TYPES_OF_SPUR_MITIGATED,

  /*! Invalid type of Spur mitigated */  
  RFLTE_MC_INVALID_SPUR_MITIGATED = RFLTE_MC_MAX_TYPES_OF_SPUR_MITIGATED,
  
} rflte_mc_spur_mitigated_type;

/*!
   @brief
   Enumeration to tell which type of power control being used.
   FBrx or hkadc
*/
typedef enum
{
   RF_LTE_POWER_CTL_MODE_HKADC,
   RF_LTE_POWER_CTL_MODE_NONE,
}  rf_lte_power_control_mode_type;

typedef enum
{
  RFLTE_NB_CARRIER_ANCHOR = 0,
  RFLTE_NB_CARRIER_NON_ANCHOR,
  RFLTE_NB_CARRIER_NUM,
  RFLTE_NB_CARRIER_INVALID = 0XFF
}rflte_mc_nb_carrier_type_e;


/*!
   @brief
   The structure defines Applied Notch Freq, Notch Depth and Spur Threshold
   per notch, per chain, per cell.
*/
typedef struct
{
  /*! Frequency(Hz) at which notch was applied */
  uint32 notch_application_freq;

  /*! Notch depth (value of K0 in the notch FSM) to be used supress the spur */
  uint8 notch_depth;

  /*! master table that contains thresholds for the corresponding spurs */
  int32 spur_thresh;

  /*! type of Spur the Notch was applied for */
  rflte_mc_spur_mitigated_type spur_type;

} rflte_mc_programmed_notches_per_path_type;

/*!
   @brief
   The structure defines applied Notch details per cell.
*/
typedef struct
{
  /*! Config at which Prx notch was programmed */
  rflte_mc_notch_programmed_config_type prx_notch_config;

  /*! Notches details for PRx */
  rflte_mc_programmed_notches_per_path_type notch_programmed_prx[RFLTE_MC_MSM_MAX_NUM_NOTCH];

  /*! DL LO Rx HZ */
  uint64 prx_dl_lo_rx_hz;

  /*! Number of Notches Programmed for PRx */
  uint32 prx_num_notches;

}rflte_mc_programmed_notches_type;

/*!
   @brief
   The structure defines state/band variables for each path for LTE.
*/
typedef struct
{
  /*! @brief Maintains state of each path */
  rf_lte_state_enum_type rf_state;

  /*! @brief RX Logical Device associated with a carrier */
  rfcom_device_enum_type prx_rfm_device;

  /*! @brief Current band for each path. Default is BAND_INVALID
  Updated during tune to channel for PRX*/     
  rfcom_lte_band_type curr_band_prx;

  /*! @brief Current RX channel for each path. Default is 0
  Updated during tune to channel for RX*/     
  rfcom_lte_earfcn_type curr_chan_rx;

  /*! @brief Current RX BW. Default is BW_INVALID */ 
  rfcom_lte_bw_type bw_rx;

  /*! @brief Current RX System BW. Default is BW_INVALID */ 
  rfcom_lte_bw_type bw_sys_rx;

  /*! @brief TX Logical Device associated with a carrier */
  rfcom_device_enum_type tx_rfm_device;

  /*! @brief Current band for each path. Default is BAND_INVALID
  Updated during tune to channel for TX*/
  /* There is possibility that RX has one band, but TX has invalid band.*/
  rfcom_lte_band_type curr_band_tx;

  /*! @brief Current TX Channel for each path. Default is 0
  Updated during tune to channel for TX*/
  /* There is possibility that RX has one band, but TX has invalid band.*/
  rfcom_lte_earfcn_type curr_chan_tx;

  /*! @brief Tx carrier freq offset for each path. Default is 0
    Updated during tune to channel for TX*/
  /* There is possibility that RX has one band, but TX has invalid band.*/
  int8 ul_carrier_freq_offset;

  /*! @brief Current TX tune BW. Default is BW_INVALID */ 
  rfcom_lte_bw_type bw_tx;

  /*! @brief Current TX system BW. Default is BW_INVALID */ 
  rfcom_lte_bw_type sys_bw_tx;

  /*! @brief NV table pointer */
  void   *nv_tbl_ptr;

  /*! @brief Flag indicating if 2D temp/freq compensation NV is in use */
  boolean lin_vs_temp_vs_freq_in_use;  

  /*! @brief Flag indicating if xpt is active, this is a flag to use in FW to isolate xpt activities. 
    If this flag is TRUE, first sweep tx_lin is available. This flag is turned TRUE only when Multilin V3
    is used. */
  boolean xpt_in_use;

  /*! @brief script data pointer for PRx, DRx and Tx */
  void *script_data_ptr[RFLTE_MC_CARR_TYPE_NUM];

  /*! @brief Flag to save ET txlm buffer information */
  boolean et_is_in_use; /* This flag is determined based on NV, it is TRUE if one of the PA state uses ET mode */
  boolean et_enable_status; /* This flag is used to track if an ET txlm buffer was reserved already */
  lm_handle_type et_txlm_buf_idx; /* This to hold the ET txlm buffer idx */

  boolean olta_enable;

  /*! @brief LM Buffer index For Tx, PRx */
  lm_handle_type rxlm_prx_buf_idx;

  lm_handle_type txlm_buf_idx;

  /*! @brief power control mode being used. HDET,FBRx etc.*/
  rf_lte_power_control_mode_type power_ctrl_mode;

  /*! To keep the state of the coex filet in the path */
  rfc_coex_filter_status_type gsm_concurrency_path;

  /*! Flag to indicate current active PA state, 
   *  flag is turned on if a valid linearizer is loaded */
  #ifndef FEATURE_RF_LTE_PAGE_ONLY
  boolean active_pa_state[RF_LTE_MAX_NUM_PA_STATES];
  #else
  boolean active_pa_state[8];
  #endif

  /*! Rx freq error from ML1*/
  int32 rx_freq_error;

  /*! NB Rx freq offset, LIF*/
  int32 nb_rx_lif_offset_hz;

  /*! NB Rx raster offset for acnhor carrier*/
  int32 nb_rx_anchor_raster_offset;

  /*! NB Rx raster offset for nonanchor carrier*/
  int32 nb_rx_nonanchor_raster_offset;

  /*! NB Anchor Rx Channel*/
  rfcom_lte_earfcn_type nb_anchor_rx_chan;
  
  /*! NB Non Anchor Rx Channel*/
  rfcom_lte_earfcn_type nb_nonanchor_rx_chan;

  /*! NB Anchor Tx Channel*/
  rfcom_lte_earfcn_type nb_anchor_tx_chan;

  /*! NB Non Anchor Tx Channel*/
  rfcom_lte_earfcn_type nb_nonanchor_tx_chan;

  /*! Path override Index to identify the unique Tx/PRx/DRx path to override 
      for each carrier */
  uint8 path_override_index[RFLTE_MC_CARR_TYPE_NUM];

  /*! @brief Flag to indicate if Rx Tune Away(TA) is in progress for this Cell */
  boolean rx_ta_in_progress;

  /*! Flag to indicate if FED is to be populated with Tx FED static
  during Tx Config */
  rf_lte_update_tx_fed_static_dm_type update_tx_fed_static_dm;

  /* AFC Rx Freq Error */
  int32 afc_rx_freq_error_in_hz;

  /*! Notches that have been programmed */
  rflte_mc_programmed_notches_type notches_programmed; 

  /*! Alternate Path for the cell */
  /* PRX = alternate_path[RFLTE_MC_CARR_TYPE_PRX] */
  /* TX  = alternate_path[RFLTE_MC_CARR_TYPE_TX] */  
  uint8 alternate_path[RFLTE_MC_CARR_TYPE_NUM]; 

  /*! True if valid TQ is assigned to cell*/
  boolean is_tq_allocated;

  /*CATM1 Rx NB Region Center Frequencies*/
  uint32 rx_nb_region_center_freq[RFLM_LTE_NB_REGION_MAX];

} rflte_mc_path_state_type; 

/*! 
  @brief
  The structure has info on IRAT state
*/
typedef struct
{
  /*! IRAT has script in progress- flag to disable non sleep requests in MC*/
  boolean lte_mc_in_suspended_Rx_state_for_IRAT;

   uint32 last_irat_band;
   /*!<  Specify target technology band */

   rfcom_lte_earfcn_type last_irat_channel;
   /*!<  Specify source technology DL channel */  
} rflte_mc_irat_state_type; 

/*!
   @brief
   The structure maintains state variables for LTE.

   @details
   The object of the structure gets updated when state or band changes.
   Rx channel, Tx channel are also maintained here. Tx channel is 
   maintained so that it provides clear distinction for RX and TX functionality 
   when band changes.

   Default for state = Init_state, curr_band = INVALID_BAND, primary_path = RF_PATH_0.

*/
typedef struct
{
  /*! @brief Maintains state and band of each path */
  rflte_mc_path_state_type  rf_path_state[RFA_RF_LTE_MAX_CELL_SUPPORTED];

  /*! @brief Flag indicating whether LTE enter mode has been called after power-up*/
  boolean init_once_flag;

  /*! @brief Flag indicating whether LTE is in TDD mode*/
  boolean tdd_mode;

  /*! @brief Flag indicate if it is in half duplex mode for CAT M1. */
  boolean hd_fdd_mode;

  /*! @brief Flag indicate if it is in LTE NBIOT. */
  boolean lte_nb_mode;

  /*! @brief Flag indicating whether LTE is in FTM mode*/
  boolean ftm_mode;

  /*! @brief Flag indicating whether LTE is in xo cal mode*/
  boolean xo_cal_mode;

  /*! @brief Flag indicating whether Critical LTE NVs are presented */
  boolean lte_rfnv_retrieve_success;

  /*! @brief Pointer which points to the table of rfc external timing information */
  rfc_timing_info_type *rfc_msm_timing_info;

  /*! @brief Pointer which points to the table of rfc internal timing information */
  rfc_timing_info_type *rfc_iotfe_timing_info;

  /*! state info to track IRAT reqests*/
  rflte_mc_irat_state_type l2l_irat_state;

  /*! @brief Flag indicating whether LTE is in FTM NS mode*/
  boolean ftm_ns_mode;
    
  /*! @brief Flag to track if GNSS Blanking is active while in LTE TDD mode */
  boolean gnss_blanking_enabled;

  /*! @brief MC level Pointer to store the bb comp table which will be used for RB based FreqComp */
  const int8 *bb_device_filter_coeff;

  /*! @bried Flag indicating if dynamic scell switch is enabled or not*/
  boolean dyn_scell_switch_on;

  /*! @brief Flag indicating if Tuner is present or not */
  boolean tuner_present;

  /*! @brief MC level array to store the Tx Rx ASM port State. */
  uint8 tx_rx_asm_port_state[RFCOM_NUM_LTE_BANDS];

  /*! @brief MC level array to store the Rx ASM port State. */
  //uint8 tx_rx_asm_port_state[RFCOM_NUM_LTE_BANDS];

  /*! @brief Flag to indicate if TQ wake up is enabled */
  boolean wakeup_tq_enable_flag;

  /*! @brief Flag indicating if RSB cal is on */
  boolean rsb_cal_on;

  /*! @brief Current NS Flag */
  #ifndef FEATURE_RF_LTE_PAGE_ONLY
  rflte_core_txpl_ns_type active_ns_flag_based_hw_ctl;
  #endif

  /* ALT Path Information */
  uint8 calibration_alt_path[RFCOM_MAX_DEVICES];

  /* UE power class info */
  rflte_core_txpl_power_class_type ue_power_class;

  boolean non_3gpp_duplex_spacing;
}rflte_mc_state_type;

extern rflte_mc_state_type rflte_mc_state;

typedef enum
{
  RF_LTE_MC_SUCCESS,
  RF_LTE_MC_SM_ERROR,
  RF_LTE_MC_STATUS_MAX,
} rflte_mc_status_type;

typedef enum
{
  RFLTE_MC_CMD_SET_QUIET_MODE,
  RFLTE_MC_CMD_MAX = 0xFF,
} rflte_mc_cmd_type;

typedef enum
{
  RFM_DEVICE_0_1_PAIR = 0,
  RFM_DEVICE_2_3_PAIR,
  RFM_DEVICE_4_5_PAIR, 
  RFM_DEVICE_MAX_PAIR,
  RFM_DEVICE_INVALID_PAIR
}rflte_mc_rfm_dev_pair_type;

typedef struct
{
  uint32 carrier_idx;
  rfcom_device_enum_type device;
  rflte_mc_carrier_type  carrier_type;
} rflte_mc_carrier_info_type;

typedef struct
{
  uint32 carrier_idx;
  rfcom_device_enum_type device;
  rflte_mc_carrier_type  carrier_type;
  rfc_lte_bw_type rflte_sys_bw;
  rfcom_lte_band_type rx_band;
  rfcom_lte_earfcn_type rflte_chan_rx;
} rflte_mc_carrier_info_rfc_type;

/*-----------------------------------------------------------------------*/
/*!
  @brief
  This structure keeps the time profiling for RX Config HW Control Layer
  
*/
typedef struct
{
  /*!> Get MSM timing from RFC time */
  rf_time_tick_type msm_timing_start;
  rf_time_type msm_timing_time;
  
  /*!> Vreg Management Time */
  rf_time_tick_type vregs_start;
  rf_time_type vregs_time;

  /*!> Manage HW and Script Build time */
  rf_time_tick_type script_hw_manage_start;
  rf_time_type script_hw_manage_start_time;

  /*!> Get RSB Info from devices Time */  
  rf_time_tick_type rsb_start;
  rf_time_type rsb_time;

  /*!> TRX and NON TRX HW cleanup Script Time */  
  /* TRX Cleanup */
  rf_time_tick_type trx_cleanup_start;
  rf_time_type trx_cleanup_time;

  /* ASM Cleanup */  
  rf_time_tick_type asm_cleanup_start;
  rf_time_type asm_cleanup_time;

  /* Tuner Cleanup */    
  rf_time_tick_type tuner_cleanup_start;
  rf_time_type tuner_cleanup_time;

  /*!> TRX and NON TRX HW Config Script Time */    
  /* TRX Config*/  
  rf_time_tick_type trx_config_start;
  rf_time_type trx_config_time;

  /* ASM Config*/    
  rf_time_tick_type asm_config_start;
  rf_time_type asm_config_time;

  /* Tuner Config*/    
  rf_time_tick_type tuner_config_start;
  rf_time_type tuner_config_time;

  /*!> MSM Config Script Time */
  rf_time_tick_type msm_config_start;
  rf_time_type msm_config_time;

  /*!> MDSP CCS event creation Time */  
  rf_time_tick_type create_mdsp_event_start;
  rf_time_type create_mdsp_event_time;

  /*!> Transceiver Power vote Time */
  rf_time_tick_type power_vote_start;
  rf_time_type power_vote_time;  

  /*!> CCS Rx Config Script enqueue Time */
  rf_time_tick_type ccs_script_schedule_start;
  rf_time_type ccs_script_schedule_time;

#ifdef FEATURE_RF_ASDIV  
  /* XSW Config*/    
  rf_time_tick_type xsw_config_start;
  rf_time_type xsw_config_time;
#endif

}rflte_mc_rx_hw_ctl_time_type;

/*-----------------------------------------------------------------------*/
/*!
  @brief
  This structure keeps the time profiling for RX Config MC Layer
  
*/
typedef struct
{
  /*!> MC Rx Config time */
  rf_time_tick_type config_start;
  rf_time_type config_start_time;

  /*!> MC Rx Config HW CTL time */  
  rflte_mc_rx_hw_ctl_time_type hw_ctl;
 
}rflte_mc_rx_time_profile_type;

/*-----------------------------------------------------------------------*/
/*!
  @brief
  This structure keeps the time profiling for entire MC Layer
  
*/
typedef struct
{
  /*!> MC Rx Config time */
  rflte_mc_rx_time_profile_type rx_time_profile[RFLTE_MC_CARR_TYPE_MAX];

}rflte_mc_time_profile_type;

/*-----------------------------------------------------------------------*/
/*!
  @brief
  Time Profile Variables for logging time profile
  
*/
extern rflte_mc_time_profile_type mc_time_prof[RFA_RF_LTE_MAX_CELL_SUPPORTED];

/*-----------------------------------------------------------------------*/
/*!
  @brief
  Flag indicating if Time profiling is enabled
  
*/
extern boolean enable_mc_time_profile;

/* Event IDs for the RF Wakeup CCS Events*/
typedef enum
{
 RFLTE_MC_CCS_WAKEUP,
 RFLTE_MC_CCS_EVENT_NUM
}rflte_mc_ccs_event_type;

/* CB data type for AsDiv */
typedef struct
{
  /*! @brief  */  
  rfm_device_enum_type device;

  /*! @brief  */  
  rfcom_lte_band_type band;

  boolean immediate;

} rflte_mc_asdiv_get_script_struct_type;

/* Rx Tune params. */
typedef struct
{
  rfcom_lte_earfcn_type  rx_channel[RFA_RF_LTE_MAX_CELL_SUPPORTED];
  rfcom_lte_bw_type rx_bw[RFA_RF_LTE_MAX_CELL_SUPPORTED];
} rflte_mc_rx_tune_params_struct_type;

/* Tuner input params required for MCL */
typedef struct
{
  uint8 scenario_val;
  rfm_tuner_handle_type tuner_handle;
  boolean is_scenario_valid;
} rflte_mc_mcl_params_s;

/*-----------------------------------------------------------------------*/
/*!
  @brief
  This structure holds BW spacing info between carriers (Single Rx scenario).
 
         A     |     D
    -----------|-----------         A    --> BW of First carrier from Left 
             |B|C|                  B+C  --> BW of First carrier from Left 
    | FIRST  | | | SECOND |         D    --> BW of Second carrier from Left 
    |CARRIER | | |CARRIER |
    | FROM   | | | FROM   |
    | LEFT   | | | LEFT   |
    ------------------------------------------------>>> Freq Domain 
*/
typedef struct
{
  /*!> Number of Carriers. */
  uint8 num_of_carriers;

  /*!> BW Info of First Carrier from Left.*/
  rfcom_lte_bw_type rflte_bw_first_carrier_from_left;

  /*!> BW Gap between First Carrier from Left.*/
  uint16 rflte_bw_gap_first_vs_second_carrier;

  /*!> BW Info of Second Carrier from Left.*/
  rfcom_lte_bw_type rflte_bw_second_carrier_from_left;

}rflte_mc_carrier_bw_spacing_s_type;

/*------structure to hold input parameters to rflte_mc_common api to set Rx config active flag-------------*/
typedef struct
{
  uint32 carrier_idx_mask;
  uint16 path_bitmask[RFA_RF_LTE_MAX_CELL_SUPPORTED];  
  rfa_rf_chain_param_type prx[RFA_RF_LTE_MAX_CELL_SUPPORTED];
} rflte_rx_config_active_flag_req_s;

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/
boolean rf_lte_msgr_register( msgr_client_t *client_id, msgr_id_t id );

/*----------------------------------------------------------------------------*/
boolean rf_lte_msgr_deregister( msgr_client_t *client_id );

/*----------------------------------------------------------------------------*/
rflte_mc_status_type rflte_mc_enter_mode( rflte_mc_carrier_info_type carrier_info );

/*----------------------------------------------------------------------------*/
rflte_mc_status_type rflte_mc_fdd_rx_config( rflte_mc_carrier_info_type carrier_info,
                                             rfcom_lte_earfcn_type rx_channel,
                                             rfcom_lte_bw_type tune_bw,
                                             rfcom_lte_bw_type sys_bw,
                                             lm_handle_type rxlm_buffer_idx,
                                             rflte_mc_mcl_params_s rflte_mc_mcl_params,
                                             rflte_mc_rx_tune_params_struct_type rx_tune_params,
                                             boolean rf_execute_tune_script);

/*----------------------------------------------------------------------------*/
rflte_mc_status_type rflte_nb_mc_fdd_rx_config( rflte_mc_carrier_info_type         carrier_info,
                                             uint8                                 num_chan,
                                             rfcom_lte_earfcn_type                 start_chan,
                                             uint8                                 step_size,
                                             rfcom_lte_bw_type                     rx_bw,
                                             lm_handle_type                        rxlm_buffer_idx,
                                             rflte_mc_mcl_params_s                 rflte_mc_mcl_params,
                                             rflte_mc_rx_tune_params_struct_type   rx_tune_params,
                                             boolean                               rf_execute_tune_script);


/*----------------------------------------------------------------------------*/
rflte_mc_status_type rflte_mc_fdd_tx_config( rflte_mc_carrier_info_type carrier_info,
                                             rfcom_lte_earfcn_type tx_channel,
                                             int8 ul_carrier_freq_offset,
                                             rfcom_lte_bw_type tx_bw,
                                             rfcom_lte_bw_type tx_sys_bw,
                                             int32 rx_freq_error,
                                             lm_handle_type txlm_buffer_idx );

/*----------------------------------------------------------------------------*/
rflte_mc_status_type rflte_mc_fdd_tx_disable( rflte_mc_carrier_info_type carrier_info, 
                                              lm_handle_type txlm_buffer_idx );

/*----------------------------------------------------------------------------*/
rflte_mc_status_type rflte_mc_tdd_rx_config( rflte_mc_carrier_info_type carrier_info,
                                             rfcom_lte_earfcn_type rx_channel,
                                             rfcom_lte_bw_type tune_bw,
                                             rfcom_lte_bw_type sys_bw,
                                             lm_handle_type rxlm_buffer_idx,
                                             rflte_mc_mcl_params_s rflte_mc_mcl_params,
                                             rflte_mc_rx_tune_params_struct_type rx_tune_params,
                                             boolean rf_execute_tune_script);

/*----------------------------------------------------------------------------*/
rflte_mc_status_type rflte_mc_tdd_tx_config( rflte_mc_carrier_info_type carrier_info,
                                             uint32 tx_channel,
                                             rfcom_lte_bw_type tx_bw,
                                             rfcom_lte_bw_type tx_sys_bw,
                                             rfa_rf_lte_tdd_ul_dl_cfg_type tdd_ul_dl_cfg,
                                             int32 rx_freq_error,
                                             lm_handle_type txlm_buffer_idx );

/*----------------------------------------------------------------------------*/
rflte_mc_status_type rflte_mc_tdd_tx_disable( rflte_mc_carrier_info_type carrier_info,
                                                 lm_handle_type txlm_buffer_idx );

/*----------------------------------------------------------------------------*/

void rflte_mc_tx_config_assign_tx_nv_ptr( rfcom_lte_band_type split_band, 
                                          rflte_mc_carrier_info_type carrier_info );

/*----------------------------------------------------------------------------*/
rflte_mc_status_type rflte_mc_sleep( rflte_mc_carrier_info_type carrier_info );

/*----------------------------------------------------------------------------*/
rflte_mc_status_type rflte_mc_wakeup( rflte_mc_carrier_info_type carrier_info,
                                      lm_handle_type rxlm_buffer_idx,
                                      boolean offline_wakeup_flag,
                                      rflte_mc_mcl_params_s rflte_mc_mcl_params);

/*----------------------------------------------------------------------------*/
int32 rflte_mc_cmd_dispatch( rfcom_device_enum_type device,int32 cmd, 
                             void *data);

/*----------------------------------------------------------------------------*/
void rf_lte_dispatch_init(void);

/*----------------------------------------------------------------------------*/
void rf_lte_dispatch_deinit(void);

/*----------------------------------------------------------------------------*/
uint32 rflte_mc_override_tuner_code(rfcom_device_enum_type device,
                                    uint8 override_flag,
                                    void *data,
                                    uint8* tuner_nv_ptr,
                                    uint8 *tuner_id);
/*----------------------------------------------------------------------------*/

rflte_mc_status_type rflte_nb_mc_rx_retune_script_req( rfcom_lte_earfcn_type nonanchor_rx_chan,
                                                       int32 raster_offset,
                                                       lm_handle_type rxlm_buffer_idx);
/*----------------------------------------------------------------------------*/

/*!
  @brief

  @details

  @param common_init_success: if common initialization was successful.
  @param device: RF device
  @param caller_tcb_ptr: Calling task TCB pointer
  @param task_nv_wait_sig: Task signal to wait for when reading NV
  @param task_wait_func_ptr: Task's wait function to be called when reading NV
*/
boolean rflte_mc_init(rfcom_device_enum_type device,
                    boolean common_init_success, 
                    rex_tcb_type *caller_tcb_ptr,
                    rex_sigs_type task_nv_wait_sig,
                    void (*task_wait_func_ptr)( rex_sigs_type ));

/*----------------------------------------------------------------------------*/
void rflte_mc_clear_rx_gain_force(void);

#ifndef FEATURE_RF_LTE_PAGE_ONLY
int32 rflte_mc_tx_update_sar_backoff(rfcom_device_enum_type device);
#endif

void rflte_mc_tx_sg_set_txplim(uint32 carrier_idx, uint32 plimit_type_mask, int16 sar_limit_dB10, int16 coex_limit_dB10, int16 wcn_limit_dB10, int16 subframe_coex_limit_dB10, int16 vbatt_limit_dB10);

/*----------------------------------------------------------------------------*/
boolean rflte_mc_reload_nv
(
  rfcom_device_enum_type device,
  rex_tcb_type *caller_tcb_ptr,
  rex_sigs_type task_nv_wait_sig,
  void (*task_wait_func_ptr)( rex_sigs_type )
);

/*----------------------------------------------------------------------------*/
boolean rflte_mc_set_pmic_quiet_mode(boolean enable);

/*----------------------------------------------------------------------------*/
rflte_mc_status_type rflte_mc_get_txpll_script
(
/* For D3925 Bring up Build, stubbing out
features dependent on other layers */
#ifdef RFLTE_BOLT_UL_CA_SUPPORTED
uint32 carrier_idx, 
#endif 
int32 rx_freq_error_in_hz, 
void *txpll_sbi_script_buf_ptr
);
#ifndef FEATURE_RF_LTE_PAGE_ONLY
boolean
rflte_mc_tx_update_pll_scripts
(
  #ifdef RFLTE_BOLT_UL_CA_SUPPORTED
  uint32 carrier_idx, 
  #endif /* RFLTE_BOLT_UL_CA_SUPPORTED */  
  int32  rx_freq_error_in_hz, 
  void  *txpll_sbi_script_buf_ptr
);
#endif
/*----------------------------------------------------------------------------*/
rflte_mc_status_type rflte_mc_get_rxpll_script
(
/* For D3925 Bring up Build, stubbing out
features dependent on other layers */
#ifdef RFLTE_BOLT_UL_CA_SUPPORTED
uint32 carrier_idx, 
#endif 
void *rxpll_script_buf_ptr
);

/*----------------------------------------------------------------------------*/
rflte_mc_status_type rflte_mc_connected_mode(boolean enable);

/*----------------------------------------------------------------------------*/

rfcom_lte_band_type rflte_mc_sm_get_band( uint8 carr_index );

/*----------------------------------------------------------------------------*/
void rflte_mc_get_rfc_timing_data(rfm_device_enum_type device, rfcom_lte_band_type rf_band);

/*----------------------------------------------------------------------------*/
rflte_mc_status_type rflte_mc_create_wakeup_events_tq(rflte_mc_carrier_info_type carrier_info, 
                                                      rf_hal_rfcmd_tq_type *wakeup_tq_ptr);

#ifdef FEATURE_RF_HAS_QTUNER
/*----------------------------------------------------------------------------*/
void 
rflte_mc_config_tx_tuner_device
(
  rfm_device_enum_type device,
  rfcom_lte_band_type band,
  rfcom_lte_earfcn_type tx_chan,
  rf_device_execution_type dev_action,
  rf_buffer_intf *script_buffer,
  int16 script_timing,
  script_token_type *script_token,
  rfcommon_atuner_cl_ctrl_info_type *cl_ctrl_info
);
/*----------------------------------------------------------------------------*/
void 
rflte_mc_disable_tx_tuner_device
(
  rfm_device_enum_type device,
  rfcom_lte_band_type band,
  rfcom_lte_earfcn_type  tx_chan,
  rf_device_execution_type dev_action,
  rf_buffer_intf *script_buffer,
  int16 script_timing,
  script_token_type *script_token
);

#endif

/*----------------------------------------------------------------------------*/
boolean 
rflte_mc_get_ftm_mode
(
void
);
/*----------------------------------------------------------------------------*/
void 
rflte_mc_set_ftm_mode
(
boolean ftm_mode
);
/*----------------------------------------------------------------------------*/
boolean 
rflte_mc_get_tuner_present
(
void
);
/*----------------------------------------------------------------------------*/
void 
rflte_mc_set_tuner_present
(
boolean status
);
/*----------------------------------------------------------------------------*/
boolean 
rflte_mc_validate_rfcom_band_using_nv_valid_data_flag
(
rfcom_lte_band_type rfcom_band
);
/*----------------------------------------------------------------------------*/
uint64 
rflte_mc_get_supported_band_config
(
void
);

uint64 
rflte_mc_get_band_mask_from_bc_config
(
uint64 band_mask
);

#ifndef FEATURE_RF_LTE_PAGE_ONLY
/*----------------------------------------------------------------------------*/
boolean
rflte_mc_tx_hw_ctl_get_device_info
(
  rfm_device_enum_type device,
  rfcom_lte_band_type band,
  rfcommon_core_txlin_agc_device_info *device_info
);
#endif
/*----------------------------------------------------------------------------*/
boolean rflte_mc_trigger_asm_update(rfcom_lte_earfcn_type tx_channel,
                                    rfcom_lte_earfcn_type rx_channel,
                                    uint8 cell_idx);
#ifdef FEATURE_RF_ASDIV
/*----------------------------------------------------------------------------*/
void rflte_mc_switch_ant(rfm_device_enum_type device,
                         rfcommon_asdiv_position_type to_pos,
                         rfm_l1_cb_type cb_func,
                         void *cb_data,
                         boolean immediate);
/*----------------------------------------------------------------------------*/
void rflte_mc_switch_ant_abort(rfm_device_enum_type device,
                               rfcommon_asdiv_position_type to_pos,
                               rfm_l1_cb_type cb_func,
                               void *cb_data);
/*----------------------------------------------------------------------------*/
void rflte_mc_send_switch_ant_msgr
(
   rfm_device_enum_type device,
   rfcommon_asdiv_position_type to_pos,
   rfm_l1_cb_type cb_func,
   void *cb_data,
   boolean immediate
);
/*----------------------------------------------------------------------------*/
void rflte_mc_send_switch_ant_abort_msgr
(
   rfm_device_enum_type device,
   rfcommon_asdiv_position_type to_pos,
   rfm_l1_cb_type cb_func,
   void *cb_data
);
/*----------------------------------------------------------------------------*/
boolean rflte_mc_common_get_asdiv_script(rfcommon_asdiv_position_type ant_sw_pos, /*!< New Antenna Position */
                                         rf_buffer_intf *rf_buffer, /*!< pointer to buffer to store Script */
                                         void *cb_data /*!< Callback data to be passed to the function */);
#endif
#ifndef FEATURE_RF_LTE_PAGE_ONLY
/*----------------------------------------------------------------------------*/
void rflte_mc_tx_pwr_ctl_script_setup 
(
  rflte_mc_carrier_info_type carrier_info,
  rfcom_lte_band_type tx_band,
  rfcom_lte_earfcn_type tx_channel
);
#endif
/*----------------------------------------------------------------------------*/

rflte_nv_tbl_type* 
rflte_mc_rx_config_assign_rx_nv_ptr
(
rfcom_lte_band_type            split_band,
rflte_mc_carrier_info_type     carrier_info
);

/*----------------------------------------------------------------------------*/
rflte_mc_status_type rflte_mc_rx_configure_chain( rflte_mc_carrier_info_type carrier_info, 
                                                   lm_handle_type rxlm_buffer_idx,
                                                  boolean enable,
                                                  rflte_mc_mcl_params_s rflte_mc_mcl_params);
/*----------------------------------------------------------------------------*/
#ifndef FEATURE_RF_LTE_PAGE_ONLY
rflte_mc_status_type 
rflte_mc_tx_disable_update_states
(
  rflte_mc_carrier_info_type     *carrier_info
);

/*----------------------------------------------------------------------------*/

rflte_mc_status_type 
rflte_mc_tx_reset_tx_fed_static_dm_update
(
  rflte_mc_carrier_info_type     *carrier_info
);
#endif
/*----------------------------------------------------------------------------*/
boolean rflte_mc_rx_spur_mitigation_cb
( 
  uint8 carrier_index,
  rfm_device_enum_type rfm_device,
  rfcom_lte_band_type rf_band,
  rfcom_lte_bw_type rf_bw,
  uint32 freq_khz,
  boolean intra_band_ca_active_device,
  boolean retune_done,
  rfdevice_lte_retune_reqd_event_type retune_event,
  rfdevice_lte_get_rx_gain_settings_type* gain_settings,
  rfdevice_meas_script_data_type* script_data_ptr
);


rflte_mc_status_type rflte_mc_exit_cleanup( rflte_mc_carrier_info_type     carrier_info );

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Release LTE TQ pair 

  @details
  Check if the TQ pair is allocated and release if valid device

  @param
  carrier_info
*/

boolean rflte_mc_common_release_tq_pair(rflte_mc_carrier_info_type *carrier_info);

/*----------------------------------------------------------------------------*/

rflte_mc_status_type rflte_mc_rx_tune_scripts_init_hal_buffer ( void );

/*----------------------------------------------------------------------------*/
rflte_mc_status_type rflte_mc_rx_tune_scripts_deinit_hal_buffer ( void );

/*----------------------------------------------------------------------------*/
rf_buffer_intf *rflte_mc_rx_tune_scripts_get_hal_buffer ( rflte_mc_rx_tune_scripts_type_s script_type );

/*----------------------------------------------------------------------------*/
void rflte_mc_rx_config_clean_time_profile ( uint32 carrier_idx );

/*----------------------------------------------------------------------------*/
rflte_mc_carrier_bw_spacing_s_type rflte_mc_rx_get_bw_spacing_info(uint32 carrier_idx, 
                                                                  rflte_mc_rx_tune_params_struct_type rx_tune_params);

/*----------------------------------------------------------------------------*/
rflte_mc_status_type rflte_mc_common_exit_mode_reset_states( rflte_mc_carrier_info_type     *carrier_info,
                                                             boolean                         skip_pcell_update);

/*----------------------------------------------------------------------------*/
boolean rflte_mc_check_nb_retune_script_requiered
(
  rflte_mc_carrier_info_type              carrier_info,
  rfcom_lte_bw_type                       rx_sys_bw,
  rfcom_lte_bw_type                       rx_tune_bw
);

/*----------------------------------------------------------------------------*/
void rflte_mc_common_rx_config_set_active_flag( rflte_rx_config_active_flag_req_s *rflte_rx_config_flag_ptr );

/*----------------------------------------------------------------------------*/
uint8 rflte_mc_rx_nb_get_fs_num_chan_capability(rfcom_lte_earfcn_type start_chan, boolean *end_adc_rate);


#ifdef FEATURE_RF_ASDIV
void rflte_mc_update_tuner_code_imm
(
  uint8 cb_data
);



/*----------------------------------------------------------------------------*/
#endif/* FEATURE_RF_ASDIV */
/*----------------------------------------------------------------------------*/

#ifdef FEATURE_RF_HAS_QTUNER
rflte_mc_status_type rflte_mc_common_tuner_config(void);
#endif /* FEATURE_RF_HAS_QTUNER */
/*----------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* RFLTE_MC_H */

