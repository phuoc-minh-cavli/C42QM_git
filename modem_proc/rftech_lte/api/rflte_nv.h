/*!
  @file
  rflte_nv.h

  @brief
  This header file contains definitions  to interface with RF NV for LTE.

*/

/*===========================================================================

  Copyright (c) 2009- 2023 QUALCOMM Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and inf ormation contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/rftech_lte.mpss/2.10/api/rflte_nv.h#7 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
02/08/23   tej     FR 85021 : NTN band support for B23 and B256
06/29/22   gk      Tx max pwr limit adj v3 NV support
01/25/22   gk      MPR NV V3 support changes
03/01/21   gk      B87/B88 addition
01/21/21   gk      remove unsupported band(B39)
01/21/21   gk      B8_B addition(Network assigned duplex spacing)
01/19/21   gk      remove unsupported bands(B17)
03/17/20   gk      neighbour bands init(bootup optimization)
12/04/19   mp      NBIOT Band86 enablement
03/11/19   gk      B27 CatM1/NB1 enablement 
03/11/19   gk      B31 CatM1/NB1 enablement 
03/08/19   gk      BB72/B73 CatM1/NB1 enablement
03/08/19   gk      B85 CatM1/NB1 enablement
03/05/19   sk      RF PAGE ONLY changes for NB/CATM
01/17/19   gk      Fix for crash in PSM scenario-added an API to init nv data
11/16/18   gk      Fix for wrong power class parsing for NB1-removed container idx as not needed
11/07/18   wxl     Add NV support Fast AGC mode
10/31/18   gk      TrackB BU chnages-Rx cal data parsing
10/25/18   sk      TRACK B Bringup changes
10/25/18   qzh     Add fast mode AGC support
09/25/18   cv      Added Support of per-band setting of Power Class in Rel.14 Cat.M1 and NB1/NB2
08/28/18   gk      rfm init(rc_init)optmization
07/30/18   gk      B71 CatM1/NB1 enablement changes
07/25/18   gk      B14 CatM1/NB1 enablement changes
07/10/18   gk      Bootup optimizations-Remove unwanted NVs in all lte bands & adding consolidated Tx and rx static nvs
02/15/18   sk      [Phase1] Chiltepin LTE memory optimization
10/18/17   cv      Added LTE B66 support
10/16/17   cv      Increasing the channel variable size and addition of static NV
08/10/17   cv      Added B25 support
04/25/17   pv      rflte_nv_tbl_type-removing pa_state_nv_c1 as UL CA not supported
09/12/16   cv       Removing of Non-POR bands
10/08/15   bir      Fix KW error
10/14/15   bir      Added support for AMPR backoff
09/30/15   bir      Added support for tx max pwr based EARFCN NV
09/28/15   bir      Added support for pa bias offset based on 
02/05/15   dw      Added support for LTE band specific ASDiv position dependent SAR Tx power limit values
10/21/14   kab     SW support for CA BC Band Combo Support NV
09/26/14   php     Add NV support for Pin Calibration
09/22/14   svi     Added support to update Band NV table Data Valid flag properly
08/14/14    pv     Added support for PA swp adjust mechanism.
07/31/14   ndb     Added support for extended EARFCN for LTE
07/11/14   qzh     Delete RFLTE_NV_B44 before B44 NV and operation are created to avoid potential overflow
06/13/14   kab     Backwards compatabiltiy for rx freq comp NVs
05/27/14   kab     Adding support for Common RX NV
05/18/14   kab     Legacy NV items reduction
04/30/14   cri     PA hysteresis in microseconds and switchpoints in dB10
04/29/14   svi     Fixed PA state Switch Points and active state update
04/25/14   cri     ET Delay offset NV and ET TDD Delay support
04/17/14   kab     Allocate memory(FBRx) for supported LTE bands only
04/17/14   svi     PA static NV and code redesign
01/09/14   svi     Split Band Support
08/20/13   kab     B30 Support
08/13/13   sbm     DIME FR: NS_05_RESTRICTED_REGION_BACKOFF NV control
08/08/13   cri     DPD scaling support
08/08/13   as      Added support for relative backoff on SAR limit based on 
                   TDD-LTE ULDL configuration
07/30/13   npi     Replace memcpy with memscpy
07/10/13    pv     Support for Delay vs Temp vs Freq offset for XPT.
08/02/13   nrk     MPR & AMPR backoff for UL CA
07/09/13   svi     Duplicated NV's for Tx Chain 1 for UL CA
07/08/13   php     Add NV support for Intraband RxAGC LNA threshold
06/28/13   kab     Fix for LTE all_band NV table and LTE Band Enum mismatch
06/28/13   ndb     Added ant_tuner_cl_ctrl_info support 
05/23/13   bar     Added 2D Matrix support for Tx Limiting vs Temp and vs freq
05/20/13   bar     Added support for split bands B28 for LTE
05/14/13   kab     Added LTE HDET HPM cut-off power extension and NV support.
05/02/13   cri     Store pin/pout comp in common structure for calv4
04/29/13   gvn     Fix order of bands in rflte_nv_supported_band_info_table
04/25/13   gvn     Support for Split bands for LTE
03/22/13   jf      Add 2D lin_vs_temp_vs_freq_comp
03/14/13   jf      Add XPT path delay support
03/07/13   bsh     B28/B29 support 
02/19/13   sbm     Bandedge max power backoff support in LTE(Nikel CR375880)
12/21/12   gvn     Adding LTE CA bc config
12/12/12   gvn     Band Specific Min power NV support (Merge from Nikel)
12/06/12   gvn     Add RFLTE_NV_INVALID_BAND to rflte_nv_supported_band_type
11/20/12   pl      Add support for LTE Intraband CA NV items
11/06/12   gvn     Add support for new LTE bands 14, 23, 26, 42 (Merge from Nikel)
11/06/12   gvn     Merge from Nikel. Add new AMPR NVs support (multiple NS support per band)
10/25/12   php     Remove duplicate population of the Tx multi lin data
09/07/12   sbm     Remove rflte_nv_tbl_ptr.
08/27/12   pl/sbm  Added C2 C3 for LTE CA support
06/12/12   id      Add support to get HDET Vs AGC NV Item Id per tech and band
06/12/12   id      Added define for WCDMA Max Power
06/07/12   as      Added support to check for RFC bands against BC config
05/14/12   dj      Add chan to freq conversion handler 
04/16/12   jf      Add LTE B6 and B10 
04/11/12   sbm     smps_bias nv support 
04/10/12   id      Added support for fetching Tx Multi Lin global structure 
                   and freq to chan conversion handler
03/08/12   php     Added support for TXP2 signals
02/02/12   php     Fix B9 band type ENUM order error
01/20/12   php     Added support for B34 and B39
12/22/11   sbm     Added B9 support.
12/14/11   vss     Change B41 enum order to fix Tx cal failure on B12
09/22/11   jyu     Added LTE B12 support 
09/01/11   pl      Corrected the ordering of NV table elements for B19 and B20
08/26/11   pl      Adding LTE B19 B21 support
08/18/11   pl      Support Cal V3 NV items
08/10/11   tnt     Add Ant Tuner support 
08/05/11   whc     Added B25 Support
03/22/11   whc     Fix Pack Error 
03/14/11   whc     APT SMPS RFC NV Support  
03/04/11   can     LTE SAR support based on index from rfm layer and NV
02/10/11   whc     Added APT nvs 
01/27/11   whc     Temp Comp Support for GST
01/19/11   whc     Add new max tx power DB10 NV
01/07/11   jyu     Added EFS NV support for LTE B5 and B8  
12/21/10   whc     Basic APT\GST support
11/23/10   can     NV support for AMPR PA switchpoint back-off for B13.
11/08/10   cri     Added support for global AMPR offset
08/31/10   jyu     Added EFS items for B2, B3, B18, B20, B38 
08/19/10   jyu     Fixed NV type mismatch 
08/09/10   jyu     Added A-MPR backoff NV support 
08/03/10   jyu     Fixed type bug of tx_limit_vs_freq 
07/06/10   jyu     Added EFS based NV support for B11 and B4
07/06/10   tnt     Add LTE B4 support
06/25/10   jyu     Added new items for LPM 
06/15/10   tnt     Add B11 support
05/26/09   tnt     Adding LTE NV items for B40(2300MHz)
05/15/09   tnt     Initial Check-in
===========================================================================*/
#ifndef RFLTE_NV_H
#define RFLTE_NV_H
/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "comdef.h"
#include "nv.h"
#include "rfcommon_nv.h"
#ifndef FEATURE_RF_LTE_PAGE_ONLY
#include "rfcommon_core_txlin.h"
#include "rfcommon_core_txlin_types.h"
#include "rfcommon_core_xpt.h"
#include "rflte_core_txpl.h"
#endif
#include "rfc_card_lte.h"


/*===========================================================================

                           CONSTANT DEFINITIONS

===========================================================================*/


/*--------------------------------------------------------------------------*/
/* External Macro Definitions */
/*--------------------------------------------------------------------------*/

/*!>
  @brief
  This module contains the external Macro Definitons :
  @{
*/

/*--------------------------------------------------------------------------*/
/*!>
  @brief
  This Macro defines the number of Max PA ranges in the new NV desgin
*/
#define RF_LTE_MAX_NUM_PA_STATES         8
/*!> 8 */

/*--------------------------------------------------------------------------*/
/*!>
  @brief
  This Macro defines the number of Max PA ranges in the legacy NV desgin
  
*/

#define RF_LTE_LEGACY_MAX_NUM_PA_STATES  4
/*!> 4 */

/*--------------------------------------------------------------------------*/
#define RFLTE_AMPR_SIZ                     16
#define RFLTE_MAX_SPI_VALUES               2
#define RFLTE_AMPR_OFFSET_SIZE             3
#define RFLTE_AMPR_PA_BACKOFF_VALUE_SIZE   2
#define RFLTE_SAR_BACK_OFF_SIZE            8
#define RFLTE_APT_SMPS_CHAR_TBL_NV_SIZE    RFC_APT_SMPS_CHAR_TBL_NV_SIZE
#define RFLTE_MAX_PWR 323
#define RFLTE_XPT_PATH_DELAY_ARRAY_SIZ     16
#define RFLTE_MAX_NS_SIZE                  4
#define RFLTE_MAX_RX_PATH_NV_CONTAINERS    8  
#define RFLTE_MAX_RX_NV_ELEMENTS          (RFLTE_MAX_RX_PATH_NV_CONTAINERS * RFNV_LTE_RX_MAX_SWITCH_POINT_TYPES)

#define RFLTE_MAX_LNA_GAIN_STATES          9
/* For CT total LNA gains are 9 but calibration is done for 8 gain states and for9th gain 
state char data will be used */
#define RFLTE_NUM_LNA_GAINS_FOR_CALDATA    8
/* For CT seperate calibration for CatM1 and NB1*/
#define RFLTE_MAX_TECHS_CALDATA            2
#define RFLTE_MAX_CARRIERS                 4
/* The temperature compensation uses interpolated tables of size
   NV_TEMP_TABLE_SIZ.  The temperature value from the therm services
   is scaled 0..THERM_SCALE.  The bin size between the entries in the
   table is RFWCDMA_NV_SINGLE_TEMP_BIN_SIZE.  Note that this is a scalar, and 
   the result of the division can make 1 bin smaller than 
   RFWCDMA_NV_SINGLE_TEMP_BIN_SIZE */
#ifndef FEATURE_RF_LTE_PAGE_ONLY
#define RFLTE_NV_SCALED_THERM_MIN 0
#define RFLTE_NV_SCALED_THERM_MAX 255
#define RFLTE_NV_SINGLE_TEMP_BIN_SIZE \
                       (RFLTE_NV_SCALED_THERM_MAX/(NV_TEMP_TABLE_SIZ-1))

#define RFLTE_MAX_NUM_CA_CONFIG RF_LTE_MAX_AMPR_NVS

/*----------------------------------------------------------------------------*/
/*!>
  @brief
  This Macro defines coversion factor for hysterisis timer for 
  8 PA State Static NV RF_LTE_EIGHT_PA_STATE_NV_VER_1

  For instance, 
  If Timer Hystersis = 4000 us; 
  the interval of txagc update is 1000 us; then 
  hyst_timer_cnt = 4000/1000 = 4; 
  and pass this to common TxAGC.

*/
#define RF_LTE_TIMER_HYST_CONV_FACTOR_VER_1  1000  

/*----------------------------------------------------------------------------*/
/*!
  @brief
  max. number of AMPR NVs that is expected to be supported per band.
  This should be set to MAX(# AMPR NVs for each supported band)

  e.g. LTE B26 currently requires to support a max. of 14 AMPR NV
  items.
*/
#define RF_LTE_MAX_AMPR_NVS                15

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This macro defines the NV container index for the Tx static NV
*/

#define RFLTE_NV_TX_CONTAINER_DEFAULT    0
#define RFLTE_NV_TX_CONTAINER_0          RFLTE_NV_TX_CONTAINER_DEFAULT
#define RFLTE_NV_TX_CONTAINER_1          1
#endif

/*! @} */

/*----------------------------------------------------------------------------*/

typedef enum {
  RFNV_LTE_RX_SWITCH_POINT_SINGLE_CARRIER = 0,
  RFNV_LTE_RX_SWITCH_POINT_MULTI_CARRIER_SHARED_LNA = 1,
  RFNV_LTE_RX_SWITCH_POINT_DIRECT = 2,
  RFNV_LTE_RX_MAX_SWITCH_POINT_TYPES,
}rfnv_lte_rx_switchpoint_enum_type;

/* Abstraction of rfnv_asdiv_rx_gain_offset_type structure to
 * minimize dependency when adding elements for book keeping
 */
typedef struct
{
  rfnv_asdiv_rx_gain_offset_type  asdiv_gain_offset_info;
}rfnv_rx_asdiv_gain_offset_info;

/* Abstraction of rfnv_asdiv_rx_switch_points_type structure to
 * minimize dependency when adding elements for book keeping
 */
typedef struct
{
  rfnv_asdiv_rx_switch_points_type asdiv_swpt_info;
}rfnv_rx_asdiv_swpt_info;

typedef struct
{
  uint8 nv_container;
  /* Array of gain offsets and switch point offsets 
   * of size maximum number of ASDIV switch positions
   */
  rfnv_rx_asdiv_gain_offset_info  *asdiv_gain_offsets;
  rfnv_rx_asdiv_swpt_info *asdiv_swpts;
}
rfnv_rx_asdiv_data_type;

/*------------------------------------------------------------------------------*/

typedef PACK(struct)
{
  uint8 nv_container;
  uint8 num_of_spurs;
  rfnv_rx_spur_data_type *spur_data;
}rfnv_rx_spur_list_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Default Pin versus freq value for RFNV - 
  This indicates Pin is not calibrated for that particular PA state
*/
#ifndef FEATURE_RF_LTE_PAGE_ONLY
#define RFLTE_NV_DEFAULT_PIN_CAL_VS_FREQ 32767

/*! @} */


#define RFLTE_NV_SAR_BACK_OFF_SWAP_POS_LIMIT_LIST_MAX_SIZE 50
#endif


/*===========================================================================

                       PUBLIC DATA DECLARATIONS

===========================================================================*/
/*----------------------------------------------------------------------------*/
/*!
  @brief 
  enum to define all LTE supported band

*/
typedef enum
{
    RFLTE_NV_B13,
    RFLTE_NV_B1,
    //RFLTE_NV_B17,
    RFLTE_NV_B4, 
    RFLTE_NV_B2, 
    RFLTE_NV_B3, 
    RFLTE_NV_B18, 
    RFLTE_NV_B20, 
    RFLTE_NV_B5, 
    RFLTE_NV_B8,
    RFLTE_NV_B25,
    RFLTE_NV_B19, 
    RFLTE_NV_B12,
    //RFLTE_NV_B39,
    RFLTE_NV_B14,
    RFLTE_NV_B26,
    RFLTE_NV_B28,
    RFLTE_NV_B28_B,
    RFLTE_NV_B66,
    RFLTE_NV_B71,
    RFLTE_NV_B72,
    RFLTE_NV_B73,
    RFLTE_NV_B85,
    RFLTE_NV_B27,
    RFLTE_NV_B31,
    RFLTE_NV_B86,
    RFLTE_NV_B8_B,
#ifdef FEATURE_NBIOT_NTN 
    RFLTE_NV_B23,
    RFLTE_NV_B256,
#endif
#ifdef FEATURE_RF_SUPPORT_BAND_87_88
    RFLTE_NV_B87,
    RFLTE_NV_B88,
#endif
    /* add additional mode enum field here */
    RFLTE_NV_SUPPORTED_BAND_NUM,
    RFLTE_NV_INVALID_BAND
} rflte_nv_supported_band_type;

#ifndef FEATURE_RF_LTE_PAGE_ONLY
typedef struct {

  uint16                                           enable;

  uint16                                           dB10_value;
} rfnv_lte_max_tx_power_nv_db10_type;
/* This structure will provide hdet threshold value based on NV*/
typedef struct {
  uint16                                          enable;

  uint16                                           value;
} rfnv_lte_hdet_hpm_threshold_type;

typedef struct {

  uint16                                           enable;

  int16                                            min_pwr_db10;
} rfnv_lte_min_tx_power_nv_db10_type;

/* This structure will provide bandedge max power backoff values  for both upper and lower edge  )*/
typedef struct {
  uint8                                           enable;/* 0- disable 1-  enable */

  uint8                                           lowerband_backoff;
  
  uint8                                           upperband_backoff;
} rfnv_lte_bandedge_maxpower_backoff_type;

/* This structure will provide bandedge maxpower pa backoff values  for both upper and lower edge  )*/
typedef struct {
  uint8                                           enable;/* 0- disable 1-  enable */

  uint8                                           lowerband_pa_switchpt_backoff;
  
  uint8                                           upperband_pa_switchpt_backoff;
} rfnv_lte_bandedge_pa_switchpt_backoff_type;

typedef PACK(struct)
{
  uint8 num_of_active_entries;
  uint16 upper_bound_tx_earfcn[RFLTE_XPT_PATH_DELAY_ARRAY_SIZ];
  int32 delay_value_bw5[RFLTE_XPT_PATH_DELAY_ARRAY_SIZ];
  int32 delay_value_bw10[RFLTE_XPT_PATH_DELAY_ARRAY_SIZ];
  int32 delay_value_bw20[RFLTE_XPT_PATH_DELAY_ARRAY_SIZ];
}rfnv_lte_xpt_path_delay_type;


/*!>
  @brief
  This Structure defines the additional for each BW
  Tx XPT delay to be added to the absolute Tx XPT delay
  for 10 Mhz
  
*/
typedef PACK ( struct )
{
  /* Reserved */
  int16 header[8];
  /* 1P4 Mhz */
  int32 offset_bw1p4[RFLTE_XPT_PATH_DELAY_ARRAY_SIZ];
  /* 3 Mhz */
  int32 offset_bw3[RFLTE_XPT_PATH_DELAY_ARRAY_SIZ];
  /* 5 Mhz */  
  int32 offset_bw5[RFLTE_XPT_PATH_DELAY_ARRAY_SIZ];
  /* 10 Mhz */  
  int32 offset_bw10[RFLTE_XPT_PATH_DELAY_ARRAY_SIZ];
  /* 15 Mhz */  
  int32 offset_bw15[RFLTE_XPT_PATH_DELAY_ARRAY_SIZ];
  /* 20 Mhz */  
  int32 offset_bw20[RFLTE_XPT_PATH_DELAY_ARRAY_SIZ];
  /* 40 Mhz */
  int32 offset_bw40[RFLTE_XPT_PATH_DELAY_ARRAY_SIZ];
}rfnv_lte_bw_offset_delay_type;

typedef struct
{
  /* Flag to use offset values with respect to the 10MHz
  offsets in the xpt_path_delay NV. */
  boolean offsets_in_use; 
  /* Per BW offsets */
  rfnv_lte_bw_offset_delay_type offsets;
}rfnv_lte_xpt_path_bw_offset_delay_type;

/* This structure is used to hold the different types of static 
   compensation */
typedef enum
{
  RFNV_LTE_STATIC_PIN_COMP,
  RFNV_LTE_STATIC_POUT_COMP
} rfnv_lte_static_comp_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This module contains enum definitions for the new NV structure for 8 PA states:
  @{
*/

/*!
  @brief
  This enum is kept in NV Pointer Table per LTE Band
  and dictates whether the PA state related NV's are:
  1. FOUR PA state NV's
  2. EIGHT PA state NV's VER 1
  
*/
typedef enum 
{

  /*!> NV updated to NV TLB Pointer are 4 PA State NV */
  RF_LTE_LEGACY_FOUR_PA_STATE_NV = 0,                      /*!> Legacy NV's */

  /*!> NV updated to NV TLB Pointer are 8 PA State NV */
  RF_LTE_EIGHT_PA_STATE_NV_VER_1 = 1,                      /*!> Version 1 */

  /*!> NV updated to NV TLB Pointer are 8 PA State NV */
  RF_LTE_EIGHT_PA_STATE_NV_VER_2 = 2,                      /*!> Version 1 */

  /*!> Number of types of PA State NV */
  RF_LTE_NUM_TYPES_PA_STATE_NV,

  /*!> NV updated to NV TLB Pointer are Invalid */  
  RF_LTE_PA_STATE_NV_INVALID

}rfnv_lte_pa_state_nv_type_e;

/*----------------------------------------------------------------------------*/

/*! @} */

/* This structure will provide absolute Tx limiting based on characterized data from NV */
typedef PACK(struct)
{
  /* Flag to control the usage of the 2D matrix or 1D static matrix */
  boolean matrix_enabled;

  /*  2D Matrix comprises of Lim vs Temp vs Freq characterization of Tx Chain.*/
  int16 lin_vs_temp_vs_freq[RF_LTE_LEGACY_MAX_NUM_PA_STATES][NV_TEMP_TABLE_SIZ][LTE_TX_CAL_CHAN_SIZ];

} rfnv_lte_lin_vs_temp_vs_freq_four_pa_static_type;

/* This structure will provide absolute Tx limiting based on characterized data from NV for 8 PA Static NV */
typedef PACK(struct)
{

  int16 tx_lin_pin_pout[RFCOMMON_NV_MAX_TEMP_BIN][RFCOMMON_NV_MAX_FREQ_BIN][RFCOMMON_NV_MAX_PA_STATE];

} rfnv_lte_lin_vs_temp_vs_freq_eight_pa_static_type;

/* Structure containing TXP2 signals control information*/
typedef PACK(struct)
{
  int8 active;                 /* Indicator of whether it is active or not      */
  int16 rise_value;               /* Units of dB100 for GSM and dB10 for other techs */
  int16 fall_value;               /* Units of dB100 for GSM and dB10 for other techs */
  int16 advance_time;             /* Units of us */

} rfnv_lte_tx_gtr_thresh_type;


/* This structure will provide absolute Tx limiting based on characterized data from NV */
typedef struct
{
  /* Flag to control the usage of the 2D matrix or 1D static matrix */
  boolean matrix_enabled;

  /*  2D Matrix comprises of Lim vs Temp vs Freq characterization of Tx Chain.*/
  int8 lim_vs_temp_vs_freq[LTE_TX_CAL_CHAN_SIZ][NV_TEMP_TABLE_SIZ];

} rfnv_lte_tx_limit_type;

/* This structure will provide tx power backoff value from the NV based on the DL/UL config */

typedef struct 
{
  /*Backoff value when DL/UL config is 0*/
  uint16 cfg_0_backoff_db10;

 /*Backoff value when DL/UL config is 1*/	
  uint16 cfg_1_backoff_db10;

 /*Backoff value when DL/UL config is 2*/
  uint16 cfg_2_backoff_db10;

 /*Backoff value when DL/UL config is 3*/
  uint16 cfg_3_backoff_db10;

 /*Backoff value when DL/UL config is 4*/
  uint16 cfg_4_backoff_db10;

 /*Backoff value when DL/UL config is 5*/
  uint16 cfg_5_backoff_db10;

 /*Backoff value when DL/UL config is 6*/
  uint16 cfg_6_backoff_db10;

}rfnv_lte_tdd_ul_dl_cfg_tx_backoff_db10_type;

/* This structure will keep the number of split bands for a particular band */
#endif

typedef struct
{

  /*! Split-band configuration for Rx. This field defines the total HW RF splits needed to be supported for Rx on the current band.
  If there are no splits, this value needs to be 1 suggesting 1 HW path for full band, max of 8 splits can be supported. */
  uint8 rx_split;

  /*! Split-band configuration for Tx. This field defines the total HW RF splits needed to be supported for Tx on the current band.
  If there are no splits, this value needs to be 1 suggesting 1 HW path for full band, max of 8 splits can be supported. */
  uint8 tx_split;

} rfnv_lte_hw_path_config_type;

#ifndef FEATURE_RF_LTE_PAGE_ONLY
/*----------------------------------------------------------------------------*/
/*!
  @brief 
  This module contains structure 
  definitions for the new NV structure for 8 PA states:
  @{
*/

/*!
  @brief
  This structure is kept in NV Pointer Table per LTE Band
  and will keep all the the PA state related NV's are either:
  1. FOUR PA state NV's
  2. EIGHT PA state NV's
  
*/
typedef struct 
{

  /*!> Number of PA states for the NV loaded */
  rfnv_lte_pa_state_nv_type_e nv_pa_states;

  uint16 active_pa_states;

  /* PA Range Map */
  int16 pa_range_map[RF_LTE_MAX_NUM_PA_STATES];

  /* timer hysterisis for swithcing from high to low gain state*/
  int16   timer_hysterisis;
  
 /* timer hysterisis for swithcing from high to low gain state*/
  int16   timer_hysterisis_nbiot;
  
  /* rise value for PA switch threshold from Linearizer gain state 0 to 1 */
  uint16  pa_rise_0_1;

  /* rise value for PA switch threshold from Linearizer gain state 1 to 2 */
  uint16  pa_rise_1_2;

  /* rise value for PA switch threshold from Linearizer gain state 2 to 3 */
  uint16  pa_rise_2_3;

  /* rise value for PA switch threshold from Linearizer gain state 3 to 4 */
  uint16  pa_rise_3_4;

  /* rise value for PA switch threshold from Linearizer gain state 4 to 5 */
  uint16  pa_rise_4_5;

  /* rise value for PA switch threshold from Linearizer gain state 5 to 6 */
  uint16  pa_rise_5_6;

  /* rise value for PA switch threshold from Linearizer gain state 6 to 7 */
  uint16  pa_rise_6_7;

  
  /* fall value for PA switch threshold from Linearizer gain state 1 to 0 */
  uint16  pa_fall_1_0;

  /* fall value for PA switch threshold from Linearizer gain state 2 to 1 */
  uint16  pa_fall_2_1;

  /* fall value for PA switch threshold from Linearizer gain state 3 to 2 */
  uint16  pa_fall_3_2;

  /* fall value for PA switch threshold from Linearizer gain state 4 to 3 */
  uint16  pa_fall_4_3;

  /* fall value for PA switch threshold from Linearizer gain state 5 to 4 */
  uint16  pa_fall_5_4;

  /* fall value for PA switch threshold from Linearizer gain state 6 to 5 */
  uint16  pa_fall_6_5;

  /* fall value for PA switch threshold from Linearizer gain state 7 to 6 */
  uint16  pa_fall_7_6;

  /* tx rotator angles */
  uint16 tx_rot_angle_pa_state0;
  uint16 tx_rot_angle_pa_state1;
  uint16 tx_rot_angle_pa_state2;
  uint16 tx_rot_angle_pa_state3;
  uint16 tx_rot_angle_pa_state4;
  uint16 tx_rot_angle_pa_state5;
  uint16 tx_rot_angle_pa_state6;
  uint16 tx_rot_angle_pa_state7;  

  /* PA Compensation UP for PA switch threshold from gain state 0 to 1 */
  int16   pa_compensate_up_0_1;
  /* PA Compensation UP for PA switch threshold from gain state 1 to 2 */
  int16   pa_compensate_up_1_2;
  /* PA Compensation UP for PA switch threshold from gain state 2 to 3 */
  int16   pa_compensate_up_2_3;
  /* PA Compensation UP for PA switch threshold from gain state 3 to 4 */
  int16   pa_compensate_up_3_4;
  /* PA Compensation UP for PA switch threshold from gain state 3 to 4 */
  int16   pa_compensate_up_4_5;
  /* PA Compensation UP for PA switch threshold from gain state 3 to 4 */
  int16   pa_compensate_up_5_6;
  /* PA Compensation UP for PA switch threshold from gain state 3 to 4 */
  int16   pa_compensate_up_6_7;

  /* PA Compensation DOWN for PA switch threshold from gain state 4 to 3 */
  int16   pa_compensate_down_8_7;
  /* PA Compensation DOWN for PA switch threshold from gain state 4 to 3 */
  int16   pa_compensate_down_7_6;
  /* PA Compensation DOWN for PA switch threshold from gain state 4 to 3 */
  int16   pa_compensate_down_6_5;
  /* PA Compensation DOWN for PA switch threshold from gain state 4 to 3 */
  int16   pa_compensate_down_5_4;
  /* PA Compensation DOWN for PA switch threshold from gain state 4 to 3 */
  int16   pa_compensate_down_4_3;
  /* PA Compensation DOWN for PA switch threshold from gain state 3 to 2 */
  int16   pa_compensate_down_3_2;
  /* PA Compensation DOWN for PA switch threshold from gain state 2 to 1 */
  int16   pa_compensate_down_2_1;
  /* PA Compensation DOWN for PA switch threshold from gain state 1 to 0 */
  int16   pa_compensate_down_1_0;

  
  /* rise value for PA switch threshold from Linearizer gain state 0 to 1 */
	uint16	     pa_rise_0_1_nbiot;
  
	/* rise value for PA switch threshold from Linearizer gain state 1 to 2 */
	uint16	     pa_rise_1_2_nbiot;
  
	/* rise value for PA switch threshold from Linearizer gain state 2 to 3 */
	uint16	     pa_rise_2_3_nbiot;
  
	/* rise value for PA switch threshold from Linearizer gain state 3 to 4 */
	uint16	    pa_rise_3_4_nbiot;
  
	/* rise value for PA switch threshold from Linearizer gain state 4 to 5 */
	uint16	     pa_rise_4_5_nbiot;
  
	/* rise value for PA switch threshold from Linearizer gain state 5 to 6 */
	uint16	 pa_rise_5_6_nbiot;
  
	/* rise value for PA switch threshold from Linearizer gain state 6 to 7 */
	uint16	 pa_rise_6_7_nbiot;

	
	/* fall value for PA switch threshold from Linearizer gain state 1 to 0 */
	  uint16       pa_fall_1_0_nbiot;
	
	  /* fall value for PA switch threshold from Linearizer gain state 2 to 1 */
	  uint16       pa_fall_2_1_nbiot;
	
	  /* fall value for PA switch threshold from Linearizer gain state 3 to 2 */
	  uint16       pa_fall_3_2_nbiot;
	
	  /* fall value for PA switch threshold from Linearizer gain state 4 to 3 */
	  uint16      pa_fall_4_3_nbiot;
	
	  /* fall value for PA switch threshold from Linearizer gain state 5 to 4 */
	  uint16       pa_fall_5_4_nbiot;
	
	  /* fall value for PA switch threshold from Linearizer gain state 6 to 5 */
	  uint16      pa_fall_6_5_nbiot;
	
	  /* fall value for PA switch threshold from Linearizer gain state 7 to 6 */
	  uint16      pa_fall_7_6_nbiot;

  /* Common Pin/Pout lin vs temp vs freq, and legacy temp comp struct */
  rfcom_txlin_temp_comp_nv_data_type lin_temp_comp_data;


}rfnv_lte_pa_state_nv_type_s;

typedef PACK (struct)
{
  rfnv_max_tx_pwr_vs_earfcn_static_data_type max_pwr_vs_earfcn_static_data;
  uint8 enable;
}rfnv_max_tx_pwr_vs_earfcn_static_data_combo_type;

typedef PACK (struct)
{
  uint8 nv_container_index;
  uint8 ns_number;
  rfnv_lte_ampr_table_type *ampr_table_data[RFNV_TX_STATIC_AMPR_TABLE_SIZE];
  uint8 num_of_ampr_elements;
} rfnv_ampr_backoff_static_data_type;

typedef PACK (struct)
{
  uint8 catm_nb1_mode;
  uint8 active_cluster_mask;
  rfnv_lte_mpr_table_t_v3 *mpr_table_data[RFNV_TX_STATIC_MPR_TABLE_SIZE];
  uint8 num_of_mpr_elements;
} rfnv_mpr_backoff_static_data_type_v3;

typedef PACK (struct)
{
  rfnv_ampr_backoff_static_data_type *ampr_static_data[RFLTE_MAX_NS_SIZE];
  uint8 num_of_ns_elements;
} rfnv_ampr_backoff_static_data_combo_type;

typedef PACK(struct)
{
  uint8 nv_container_index;
  rfnv_max_tx_pwr_limit_adj_static_data_t max_tx_pwr_limit_adj_static_data;
  uint8 nv_cont_enabled;
}rfnv_max_tx_pwr_limit_adj_static_data_combo_type;

typedef PACK(struct)
{
  uint8 catm1_nb1_mode;
  rfnv_max_tx_pwr_limit_adj_static_data_t_v3 max_tx_pwr_limit_adj_static_data;
  uint8 nv_cont_enabled;
}rfnv_max_tx_pwr_limit_adj_static_data_combo_type_v3;


typedef PACK (struct)
{
  uint8 enable;
  rfnv_tx_min_power_limit_control_static_data_t tx_min_power_limit_control_static_data;
  
}rfnv_tx_min_pwr_limit_control_static_data_combo_type;

typedef PACK(struct)
{
  /* ue power class for CATM1 */
  int8 ue_power_class_CATM1;
  /* ue power class for NBIoT */
  int8 ue_power_class_NBIoT;
}rfnv_ue_power_class_band_type;


/*Tx static NV data type*/
typedef struct 
{
  rfnv_data_pa_bias_combo_offset_type *pa_bias_offset_data;
  /* NV Related to Max Tx PWR based on EARFCN static Data */
  rfnv_max_tx_pwr_vs_earfcn_static_data_combo_type   *max_pwr_vs_earfcn_static_combo_data;
  /* NV Related to AMPR backoff */
  rfnv_ampr_backoff_static_data_combo_type *ampr_backoff_static_combo_data;

  rfnv_tx_min_pwr_limit_control_static_data_combo_type *tx_min_pwr_limit_ctrl_static_combo_data;
  /* NV Related to UE power class */
  rfnv_ue_power_class_band_type *ue_power_class_band_data;
  
  rfnv_max_tx_pwr_limit_adj_static_data_combo_type *max_tx_pwr_limit_adj_static_combo_data;

}rfnv_lte_tx_container_type_s;
#endif

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This structure holds LTE CA Band Combinations.
  This structure will cumulatively store UL CA (to be added later) and DL CA combos.

  @elements:
  uint8 num_of_dlca_combos  
  Indicates Number of supported DL CA Band Combiations.
  
  uint8 (*dlca_band_combo_ptr) [LTE_DL_CA_MAX_NUM_CARRIERS];
  Indicates Bands in DL CA 
  uint8 pcc;
  uint8 scc0;
  uint8 scc1;
  uint8 scc2;
  uint8 scc3;

*/
typedef PACK (struct)
{
  uint8 num_of_dlca_combos;
  uint8 (*dlca_band_combo_ptr) [LTE_DL_CA_MAX_NUM_CARRIERS];
} rfnv_lte_ca_type_s;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This structure holds the SAR Tx power limits for ASDiv switch position 1
  as entered in the NV : RFNV_LTE_SAR_BACK_OFF_SWAP_POS_LIMIT_I 
*/
#ifndef FEATURE_RF_LTE_PAGE_ONLY
typedef PACK (struct)
{
  uint8 band;
  uint16 sar_back_off_swap_pos_limit_db10_values[RFLTE_SAR_BACK_OFF_SIZE];
}rfnv_lte_sar_back_off_swap_pos_limit_uncompressed_type;



/*----------------------------------------------------------------------------*/
/*!
  @brief
  This structure holds the band specific ASDiv position dependent SAR Tx power limit values
  The enable_flag is TRUE if the values have been specified in the NV for the particular band.
*/

typedef struct
{
  boolean enable_flag;
  uint16 sar_back_off_swap_pos_limit_db10_values[RFLTE_SAR_BACK_OFF_SIZE];
}rflte_sar_back_off_swap_pos_limit_type;
#endif
/*----------------------------------------------------------------------------*/
typedef struct
{
    uint8 type; /* 0: static (dBm) switch point, 1: dynamic (dBFS) switch point */
    int16 rise_or_backoff; /* if type 1, backoff must be a negative number in dB10 that is the 
    distance from saturation that the LNA will rise to the next state */       
    int16 fall_or_hyst; /* if type 1, hyst must be a negative number in dB10 that is the amount below 
    the rise that the LNA will fall back into this state */
}rfnv_rx_switch_points_data_type;   

/*----------------------------------------------------------------------------*/
typedef struct
{
    /* lna rise threshold for fast agc mode */
    int16 lna_range_rise_fast_agc;
    /* lna fall threshold for fast agc mode */
    int16 lna_range_fall_fast_agc;
}rfnv_rx_swp_init_search_data_type;
/*----------------------------------------------------------------------------*/

/*! @} */

typedef struct
{
  uint8 nv_container_idx;
  rfnv_rx_switch_points_data_type *(switch_points_data_p[RFNV_LTE_RX_MAX_SWITCH_POINT_TYPES]);
  /* Fast AGC mode */
  rfnv_rx_swp_init_search_data_type *swp_fast_agc_p;
}rfnv_rx_static_switch_points_per_nv_container_data_type;


typedef struct
{      
   uint16 data_type;
   uint32  num_of_rx_switch_point_elements;
   uint32  num_of_unique_nv_container_indices;
   rfnv_rx_static_switch_points_per_nv_container_data_type *data_ptr;
}rfnv_rx_static_internal_data_type;

typedef PACK(struct)
{
   /*static data for LTE Rx paths*/
   rfnv_rx_static_data_type         *rx_path_static_data;
   /* Rx alternate path based MTPL NV */
   rfnv_rx_alt_path_mtpl_data_type  *rx_alt_path_mtpl_data;
   /*Intraband static data for LTE Rx paths*/
   rfnv_rx_static_data_type         *rx_path_intra_static_data;
   uint32                           num_of_rx_path_elements;
   uint32                           num_of_rx_path_intra_elements;
   uint32                           num_of_rx_alt_path_mtpl_elements;

   /*---------------------------------------ASDIV------------------------------------------------------*/
   /*pointer to a Table of pointers each pointing to a row containing ASDIV data per switch position*/
   rfnv_rx_asdiv_data_type          **rx_path_asdiv_static_data_ptr_tbl; //[RFLTE_MAX_RX_PATH_NV_CONTAINERS];

   /*Number of ASDIV gain offset elements*/
   uint32                           num_of_rx_path_asdiv_gain_offset_elements;
   /*Number of ASDIV switchpoint elements*/
   uint32                           num_of_rx_path_asdiv_swpt_elements;
   /*Max Number of ASDIV switch positions detected*/
   uint32                           max_num_asdiv_switch_pos;
   /*--------------------------------------------------------------------------------------------------*/
   
   /*pointer to a Table of pointers each pointing to a structure containing spur data per nv container*/
   rfnv_rx_spur_list_type           **rx_spur_static_data;
   uint32                           num_of_rx_spur_elements;
}rflte_nv_rx_static_unified_data_type;


/*----------------------------------------------------------------------------*/
/*!
  @brief 
  structure to define all common fields of LTE band regardless of band

*/
typedef struct
{
    /* lna non-bypass hold timer from low to high gain*/
    uint16  nonbypass_timer;
    /* lna bypass hold timer from high to low gain*/
    uint16  bypass_timer;
    /* RX path delay */
    int16   rx_delay;
    /* rx calibration channels list */
    rfcom_lte_earfcn_type  rx_cal_chan[LTE_RX_CAL_CHAN_SIZE];
    /* lna threshold from g0 to g1 gain states */
    int16   lna_range_rise;
    /* lna threshold from g1 to g2 gain states */
    int16   lna_range_rise_2;
    /* lna threshold from g2 to g3 gain states */
    int16   lna_range_rise_3;
    /* lna threshold from g3 to g4 gain states */
    int16   lna_range_rise_4;
    /* lna threshold from g4 to g5 gain states */
    int16   lna_range_rise_5;
    /* lna threshold from g5 to g6 gain states */
    int16   lna_range_rise_6;
    /* lna threshold from g6 to g7 gain states */ 
    int16   lna_range_rise_7;
    /* lna threshold from g7 to g8 gain states */
    int16   lna_range_rise_8;
    /* lna rise threshold for fast agc mode */
    int16   lna_range_rise_fast_agc;
    /* lna threshold from g1 to g0 gain states */
    int16   lna_range_fall;
    /* lna threshold from g2 to g1 gain states */
    int16   lna_range_fall_2;
    /* lna threshold from g3 to g2 gain states */
    int16   lna_range_fall_3;
    /* lna threshold from g4 to g3 gain states */
    int16   lna_range_fall_4;
    /* lna threshold from g5 to g4 gain states */
    int16   lna_range_fall_5;
    /* lna threshold from g6 to g5 gain states */
    int16   lna_range_fall_6;
    /* lna threshold from g7 to g6 gain states */
     int16   lna_range_fall_7;
    /* lna threshold from g8 to g7 gain states */
    int16   lna_range_fall_8;
    /* lna fall threshold for fast agc mode */
    int16   lna_range_fall_fast_agc;
    /* im threshold from g0 to g1 */
    int16   im_level;
    /* im threshold from g1 to g2 */
    int16   im_level_2;
    /* im threshold from g2 to g3 */
    int16   im_level_3;
    /* im threshold from g3 to g4 */
    int16   im_level_4;
    /* im threshold from g4 to g5 */
    int16   im_level_5;
    /* im threshold from g5 to g6 */
    int16   im_level_6;
    /* absolute rx gain vs temperature */
    int8    rx_gain_vs_temp[RFLTE_MAX_LNA_GAIN_STATES][RX_TEMP_BIN_SIZ];  
    /* absolute rx gain */
    int16   rx_gain[RFLTE_MAX_LNA_GAIN_STATES];
    /* absolute rx gain vs frequency */
    int8    rx_gain_vs_freq[RFLTE_MAX_LNA_GAIN_STATES][NV_FREQ_TABLE_SIZ];
    /* primary path LNA phase control in LTE band */
    uint16  lte_lna_phase_ctrl[RFLTE_MAX_LNA_GAIN_STATES];
    /* im2 i value */
    uint8   im2_i_value;
    /* im2 q value */
    uint8   im2_q_value;
    /* im2 transconductor value */
    uint8   im2_transconductor_value; 
    /* In-Phase 16-bit signed A coefficient for   div RXFE mismatch comp */
    int16   rfx_mis_comp_a_coeff;
    /* In-Phase 16-bit signed B coefficient for   div RXFE mismatch comp */
    int16   rfx_mis_comp_b_coeff;
    /* RF (RX+TX) path delay from the antenna to the IQ digital MSM pins */
    int32   enc_btf;
    /* primary anttena select mapping */
    uint8   antsel[4];    
    /* RX_ON rise delay*/
    uint16 rx_on_rise_delay;
    /* RX_ON fall delay*/
    uint16 rx_on_fall_delay;  
    /* HW path split config */ 
    rfnv_lte_hw_path_config_type rxtx_cfg;	    
    #ifndef FEATURE_RF_LTE_PAGE_ONLY
    /* UE maximum TX power */
    int8    max_tx_power;
    /* tx delays and update agc time */
    uint16  agc_pa_on_rise_delay;
    uint16  agc_pa_on_fall_delay;
    uint16  agc_tx_on_rise_delay;
    uint16  agc_tx_on_fall_delay;
    /* pa range update time */
    uint16  pa_gain_up_time;
    uint16  pa_gain_down_time;
    /* Tx linearizer versus temperature */
    int8    tx_lin_vs_temp[RF_LTE_LEGACY_MAX_NUM_PA_STATES][LTE_TX_TEMP_COMP_SIZ];
    /* tx calibration channels list */
    rfcom_lte_earfcn_type  tx_cal_chan[LTE_TX_CAL_CHAN_SIZ];
    /* absolute Tx power limit versus temperature versus frequency */
    rfnv_lte_tx_limit_type static_tx_limit;
    /* Currently also keeping the limit_vs_temp as an array for backward compatibility.*/
    /* absolute Tx power limit versus temperature */
    int8    tx_limit_vs_temp[LTE_TX_TEMP_COMP_SIZ];
    /* one for each channel */
    uint16  pa_switchpoints_shift[LTE_MPR_SIZ];
    /* maximum power backoff, one for each channel */
    uint16 lte_tx_mpr_backoff[LTE_MPR_SIZ];  
    /*  digital gain compensation */
    int16  lte_tx_digital_gain_comp[LTE_MPR_SIZ];
    /* tx agc offset */
    int16  lte_tx_agc_offset[LTE_MPR_SIZ];
    /* tx power freq compensation per pa range */
    int8    tx_comp_vs_freq[RF_LTE_LEGACY_MAX_NUM_PA_STATES][LTE_TX_FREQ_COMP_SIZ];
    /* Expected HDET versus AGC */
    uint16  exp_hdet_vs_agc[LTE_HDET_VS_AGC_SIZ];
    /* limit tx power versus freq */
    int8   tx_limit_vs_freq[LTE_TX_FREQ_COMP_SIZ];
    /* ADC HDET reading offset */
    uint16  hdet_off;
    /* ADC HDET span */
    uint16  hdet_span;
    /*tx mismatch compensation for IQ*/
    nv_lte_tx_mismatch_comp_type  tx_iq_mismatch_com;
    /*tx carrier feed through compensation for IQ*/
    nv_lte_tx_carrier_feedthrough_comp_type  tx_iq_carrier_feedthru_comp;
    /*LTE Band Edge max power backoff values */
    rfnv_lte_bandedge_maxpower_backoff_type lte_bandedge_maxpower_backoff;
    /*LTE Band Edge max power PA switch point backoff values */
    rfnv_lte_bandedge_pa_switchpt_backoff_type lte_bandedge_maxpower_pa_switchpt_backoff;
    /*HDET Threshold*/
    rfnv_lte_hdet_hpm_threshold_type lte_hdet_hpm_threshold;
    /*switch point for LPM*/
    uint16  lte_hdet_hpm_lpm_switch_point;
    /* Expected HDET versus AGC in LPM*/
    uint16  lte_exp_lpm_hdet_vs_agc[LTE_HDET_VS_AGC_SIZ];
    /* limit tx power versus freq in LPM*/
    int8    lte_tx_limit_vs_freq_lpm[LTE_TX_FREQ_COMP_SIZ];
    PACK (union)
    {
    uint8   lte_ampr[RFLTE_AMPR_SIZ];
      uint8   lte_enh_ampr[RF_LTE_MAX_AMPR_NVS][RFLTE_AMPR_SIZ];
    }lte_ns_ampr;
    uint32  lte_spi_value[RFLTE_MAX_SPI_VALUES];
    uint8   lte_ampr_offset[RFLTE_AMPR_OFFSET_SIZE];
    uint8   lte_ampr_pa_backoff_value[RFLTE_AMPR_PA_BACKOFF_VALUE_SIZE];
    /* SMPS Overdrive Value */
    uint16  smps_odrive;
    /* APT voltage Value */
    uint16  smps_bias;
    /*Max TX Power DB10*/
    rfnv_lte_max_tx_power_nv_db10_type max_tx_power_nv_db10;
    /*Min TX Power DB10*/
    rfnv_lte_min_tx_power_nv_db10_type min_tx_power_nv_db10;
    /*SAR Back-off NV*/
    int16 lte_db10_sar_back_off_limit[RFLTE_SAR_BACK_OFF_SIZE];
    /* Ant Tuner NV */
#ifdef FEATURE_RF_HAS_QTUNER    
    rfcommon_nv_ant_tuner_type ant_tuner;
#endif
    /* Tx Linearizer table */
    tx_band_cal_type *tx_band_cal_data;
    /* Tx Linearizer table for catM1 */
    tx_band_cal_type *tx_band_catm1_cal_data;
  /* Tx Linearizer table for NB1*/
    tx_band_cal_type *tx_band_nb1_cal_data;
    /* TXP2 Threshold data */
    rfnv_lte_tx_gtr_thresh_type tx_gtr_thresh_data;
    /* XPT path delay */
    rfnv_lte_xpt_path_delay_type xpt_path_delay;
    /* Additional XPT path delay to be added for each BW */
    rfnv_lte_xpt_path_bw_offset_delay_type bw_offset_xpt_path_delay;
    /* Additional XPT path delay to be added for each BW for Chain 1
    -- Add for UL CA */
    /* rfnv_lte_xpt_path_bw_offset_delay_type c1_bw_offset_xpt_path_delay; */
    /* XPT path delay offset vs temp vs freq for Tx Carrier-0 */
    rfnv_tx_delay_offsets_type c0_tx_static;
    /* xPT DPD static params for expansion and processing */
    uint32 tx_xpt_dpd_config_params[MAX_XPT_NUM_CONFIG_PARAMS];
#ifdef FEATURE_RF_HAS_QTUNER    
    /* CL antenna tuner control info*/
    rfcommon_nv_ant_tuner_cl_ctrl_type ant_tuner_cl_ctrl_info;
#endif
    /*tdd config Back-off NV*/
    rfnv_lte_tdd_ul_dl_cfg_tx_backoff_db10_type lte_tdd_ul_dl_cfg_backoff;
    /*s_ampr backoff*/
    uint8 ns_05_restricted_region_backoff;
    /* Tx filter path loss offset */
    int8 tx_path_loss_offset[LTE_TX_FREQ_COMP_SIZ];
    /* NV Related to number of PA States for Chain 0 */
    rfnv_lte_pa_state_nv_type_s pa_state_nv_c0; // 6912 Bytes ~7K
#endif
    /* NV Related to RX_GAIN_VS_FREQ  */
    rfnv_rx_cal_data_v3_type rx_band_cal_data[RFLTE_MAX_TECHS_CALDATA];
	    /*RX Unified Static NV*/
    rflte_nv_rx_static_unified_data_type   *rx_unified_static_data;
		
    rfnv_rx_static_internal_data_type  *rx_static_internal_data;
#ifndef FEATURE_RF_LTE_PAGE_ONLY
    /* NV Related to PA switchpoint adjustment */
    rfnv_pa_swp_adj_val_type c0_pa_swp_data;  
    /* SAR Back-off NV */
    rflte_sar_back_off_swap_pos_limit_type lte_db10_sar_back_off_swap_pos_limit;
        /*Tx PATH specific NV's which are unique for a given NV container in RFC*/
     rfnv_lte_tx_container_type_s tx_path_nv_data[LTE_TX_MAX_NUM_CONTAINERS];
     rfnv_mpr_backoff_static_data_type_v3 *mpr_backoff_static_comb_data;
#endif
} rflte_nv_tbl_type;


/*----------------------------------------------------------------------------*/
/*!
  @brief 
  structure to hold LTE Common NV items.
*/
typedef struct
{
   /* LTE CA Band Support Combination. */
   rfnv_lte_ca_type_s ca_combo;

} rflte_common_nv_tbl_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief 
  structures definitions for support of EFS based NV and they are common for all bands

*/

typedef struct{ 
  
  uint16                                           nonbypass_timer;
  
  uint16                                           bypass_timer;
} rfnv_lte_nonbypass_bypass_timer_type;

typedef struct { 
  
  int16                                            rise_threshold;
  
  int16                                            fall_threshold;
} rfnv_lte_lna_range_rise_fall_type;

typedef struct { 
  
  uint8                                            i_level;
  
  uint8                                            q_level;
  
  uint8                                            transconductance;
} rfnv_lte_im2_values_type;

typedef int8 rfnv_lte_rx_gain_vs_temp_type[RFLTE_MAX_LNA_GAIN_STATES][RX_TEMP_BIN_SIZ];

typedef struct{ 
  
  int16                                            A_COEFF;
  
  int16                                            B_COEFF;
} rfnv_lte_rxf_mismatch_comp_type;

#ifndef FEATURE_RF_LTE_PAGE_ONLY

typedef struct { 
  
  uint16                                           rise_delay;
  
  uint16                                           fall_delay;
} rfnv_lte_agc_pa_on_rise_fall_delay_type;

typedef struct { 
  
  uint16                                           rise_delay;
  
  uint16                                           fall_delay;
} rfnv_lte_agc_tx_on_rise_fall_delay_type;

typedef struct { 
  
  uint16                                           up_time;
  
  uint16                                           down_time;
} rfnv_lte_pa_gain_up_down_time_type;

typedef struct { 
  
  int16                                            up_comp;
  
  int16                                            down_comp;
} rfnv_lte_pa_compensate_up_down_type;

typedef struct { 
  
  uint16                                           rise_threshold;
  
  uint16                                           fall_threshold;
} rfnv_lte_pa_rise_fall_threshold_type;

typedef struct { 
  
  uint16                                           offset;
  
  uint16                                           span;
} rfnv_lte_hdet_off_span_type;
#endif
typedef struct { 
  
  uint16                                           nonbypass_timer;
  
  uint16                                           bypass_timer;
} rfnv_lte_c1_nonbypass_bypass_timer_type;

typedef struct { 
  
  int16                                            rise_threshold;
  
  int16                                            fall_threshold;
} rfnv_lte_c1_lna_range_rise_fall_type;

typedef struct { 
  
  uint8                                            i_level;
  
  uint8                                            q_level;
  
  uint8                                            transconductance;
} rfnv_lte_c1_im2_values_type;
#ifndef FEATURE_RF_LTE_PAGE_ONLY
typedef struct { 
  
  int16                                            IQ_SCALE_ALPHA;
  
  int16                                            IQ_SCALE_BETA;
} rfnv_lte_tx_mismatch_comp_type;

typedef struct { 
  
  int16                                            I_OFFSET;
  
  int16                                            Q_OFFSET;
} rfnv_lte_tx_carrier_feedthrough_comp_type;

typedef struct {
  boolean                                          valid;
  uint8*                                           ampr_tbl_ptr;
} rfnv_lte_ampr_type;

/*! 
  @brief  
  Pin Cal versus Freq NV structure
*/
typedef PACK (struct)
{
  /* Pin Cal versus Freq NV information */
  int16 pin_cal_vs_freq[RFCOM_TXLIN_NUM_PA_STATES]
                       [RFCOM_TXLIN_NUM_CHANNELS];
}rfnv_lte_pin_cal_vs_freq_type;
#endif
typedef struct {
   uint16 type_id;
   uint32 num_rx_static_elements;
   uint32 num_rx_static_fast_agc_elements;
   uint32 num_rx_per_unique_path_elements; 
   uint32 num_rx_char_cal_data118_elements; 
   uint32 num_rx_char_cal_data124_elements;
   uint32 num_unique_nv_container_ids;
   uint32 num_asdiv_gain_offset_elements;
   uint32 num_asdiv_swtpt_elements;
   uint32 num_asdiv_unique_nv_container_ids;
   uint32 num_of_rx_spur_elements;
   uint32 num_spur_unique_nv_container_ids;
} rfnv_lte_rx_static_element_counts_s;


/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/
rflte_nv_tbl_type *rflte_nv_get_tbl_ptr
(
rflte_nv_supported_band_type band            //!< band to get table ptr for
);

rfcommon_nv_status_type rflte_nv_retrieve_items
(
  rex_tcb_type *task_ptr,                     //!< pointer to tcb of the calling task
  rex_sigs_type task_wait_sig,                //!< signal that calling task waiting for
  void (*task_wait_handler)( rex_sigs_type )  //!< function ptr to callback function
);

void rflte_nv_set_card_defaults
(
  nv_lte_bc_config_type *bc_config            //!< pointer to lte bc config item
);

void rflte_nv_convert_legacy_rx_gain_vs_freq(void);

boolean rflte_nv_im2_ready
(
  rflte_nv_supported_band_type band           //! band to check for IM2 NV
);

boolean rflte_nv_get_data_valid_flag(rflte_nv_supported_band_type band);

#ifndef FEATURE_RF_LTE_PAGE_ONLY
boolean rflte_populate_band_tx_linearizer(rfnv_item_id_enum_type item_code, tx_band_cal_type *band_tbl);

boolean rflte_nv_get_txlin_cal_data(uint8 band,
                                    uint8 device, 
                                    rfnv_item_id_enum_type* item_code,
                                    tx_band_cal_type **tx_band_cal_data);

rfcommon_core_convert_chan_handler_type rflte_nv_get_tx_freq_to_chan_handler(uint8 band);

rfcommon_core_convert_freq_handler_type rflte_nv_get_chan_to_tx_freq_handler(uint8 band);

boolean rflte_nv_get_hdet_vs_agc_nvid(uint8 band,
                                      uint8 device,
                                      rfnv_item_id_enum_type* item_code);
rfnv_lte_ampr_type rflte_nv_get_ampr_tbl(rfcom_lte_earfcn_type tx_chan,
                                         rfcom_lte_bw_type bw,
                                         rflte_core_txpl_ns_type ns_x);

#ifdef RFLTE_BOLT_MULTICLUSTER_NV_SUPPORTED
rfnv_lte_ampr_type rflte_nv_get_multicluster_ampr_tbl(rfcom_lte_band_type tx_band,
                                                      rflte_core_txpl_cont_ca_agg_bw_type agg_bw,
                                                      rflte_core_txpl_ca_ns_type ca_ns_x,
                                                      rfcom_device_enum_type tx_device);

rfnv_lte_ampr_type rflte_nv_get_singlecluster_ampr_tbl(rfcom_lte_band_type tx_band,
                                                       rflte_core_txpl_cont_ca_agg_bw_type agg_bw,
                                                       rflte_core_txpl_ca_ns_type ca_ns_x,
                                                       rfcom_device_enum_type tx_device);
#endif

void 
rflte_nv_load_legacy_lin_vs_temp_offset
(
  rflte_nv_tbl_type *nv_tbl_ptr,
                                             rfnv_item_type    *rfnv_item_ptr,
  rfm_device_enum_type rfm_device
);

void 
rflte_nv_load_lin_vs_temp_tx_char_chan
(
  rflte_nv_tbl_type      *nv_tbl_ptr,
  rfnv_item_type         *rfnv_item_ptr,
  rfcom_lte_band_type    band,
  rfm_device_enum_type   rfm_device,
  const boolean          parse_as_offset_earfcn  
);

void
rflte_nv_load_equidistant_therm_bins_data
(
  rflte_nv_tbl_type *nv_tbl_ptr, rfm_device_enum_type rfm_device
);

void 
rflte_nv_load_lin_vs_temp_vs_freq_four_pa_static_nv
(
  rflte_nv_tbl_type         *nv_tbl_ptr,
  rfnv_item_type            *rfnv_item_ptr,
  rfnv_lte_static_comp_type  comp_type,
  rfm_device_enum_type       rfm_device,
  uint16                     max_pa_state
  
);

void
rflte_nv_set_pa_state_nv_type
(
  rflte_nv_tbl_type* nv_tbl_type_ptr,
  rfnv_lte_pa_state_nv_type_e nv_pa_states,
  rfm_device_enum_type rfm_device
);

rfnv_lte_pa_state_nv_type_e
rflte_nv_get_pa_state_nv_type
(
  rflte_nv_tbl_type* nv_tbl_type_ptr,
  rfm_device_enum_type rfm_device
);

void
rflte_nv_populate_pa_range_map_nv
(
  rflte_nv_tbl_type            *nv_tbl_type_ptr,
  void                         *rfnv_item_ptr,
  rfnv_lte_pa_state_nv_type_e   nv_pa_states_req,
  rfm_device_enum_type          rfm_device
);

void
rflte_nv_populate_timer_hysterisis_nv
(
  rflte_nv_tbl_type            *nv_tbl_type_ptr,
  void                         *rfnv_item_ptr,
  rfnv_lte_pa_state_nv_type_e   nv_pa_states_req,
  rfm_device_enum_type          rfm_device
);

void
rflte_nv_populate_timer_hysterisis_nbiot_nv
(
	  rflte_nv_tbl_type *nv_tbl_type_ptr,
	  void   *rfnv_item_ptr,
	  rfnv_lte_pa_state_nv_type_e nv_pa_states_req,
	  rfm_device_enum_type rfm_device
);


void
rflte_nv_populate_pa_rise_fall_threshold_nv
(
  rflte_nv_tbl_type            *nv_tbl_type_ptr,
  void                         *rfnv_item_ptr,
  rfnv_lte_pa_state_nv_type_e   nv_pa_states_req,
  rfm_device_enum_type          rfm_device
);

void
rflte_nv_populate_pa_rise_fall_threshold_nbiot_nv	
(
  rflte_nv_tbl_type            *nv_tbl_type_ptr,
  void                         *rfnv_item_ptr,
  rfnv_lte_pa_state_nv_type_e   nv_pa_states_req,
  rfm_device_enum_type          rfm_device
);


void
rflte_nv_populate_tx_rot_angle_pa_state_nv
(
  rflte_nv_tbl_type            *nv_tbl_type_ptr,
  void                         *rfnv_item_ptr,
  rfnv_lte_pa_state_nv_type_e   nv_pa_states_req,
  rfm_device_enum_type          rfm_device
);

void
rflte_nv_populate_lin_vs_temp_vs_freq_nv
(
  rflte_nv_tbl_type            *nv_tbl_type_ptr,
  void                         *rfnv_item_ptr,
  rfnv_lte_pa_state_nv_type_e   nv_pa_states_req,
  rfm_device_enum_type          rfm_device,
  rfnv_lte_static_comp_type     comp_type
);

void
rflte_nv_populate_pa_comp_up_down_pa_switch_threshold_nv
(
  rflte_nv_tbl_type            *nv_tbl_type_ptr,
  void                         *rfnv_item_ptr,
  rfnv_lte_pa_state_nv_type_e   nv_pa_states_req,
  rfm_device_enum_type          rfm_device
);

void
rflte_nv_populate_eight_pa_static_nv
(
  rflte_nv_tbl_type            *nv_tbl_type_ptr,
  void                         *rfnv_item_ptr,
  rfm_device_enum_type          rfm_device,
  rfnv_item_id_enum_type        nv_item_code
);

rfcommon_nv_status_type 
rflte_nv_retrieve_eight_pa_static_items
(
  rfcommon_rfnv_info_type *nv_info_table, 
  int16 nv_info_table_size, 
  rex_tcb_type *task_ptr, 
  rex_sigs_type task_wait_sig,
  void (*task_wait_handler)( rex_sigs_type )
);

boolean rflte_nv_get_tx_band_support(rfcom_lte_band_type band);

void
rflte_nv_populate_tx_xpt_path_bw_offset_delay_nv
(
  rflte_nv_tbl_type            *nv_tbl_type_ptr,
  void                         *rfnv_item_ptr,
  rfm_device_enum_type          rfm_device
);

int16
rflte_nv_get_num_active_pa_states
(
  rflte_nv_tbl_type* nv_tbl_type_ptr,
  rfm_device_enum_type rfm_device
);
#endif
rfcommon_nv_status_type rflte_nv_update_data_valid_flag ( rfcommon_rfnv_info_type  *rfnv_info_table, 
                                                          int16                     rfnv_info_table_size );
#ifndef FEATURE_RF_LTE_PAGE_ONLY
void rflte_nv_populate_pin_cal_vs_freq
(
  rflte_nv_tbl_type *nv_tbl_type_ptr,
  void *item_ptr,
  rfm_device_enum_type rfm_device
);

boolean rflte_nv_populate_tx_static_data
(
  rfnv_item_id_enum_type item_code,
  rflte_nv_tbl_type *nv_tbl_type_ptr,
  void *item_ptr
);

boolean 
rflte_nv_populate_pa_bias_offset_data
( 
  void *temp_data_ptr,
  int32 temp_num_bytes_left,
  uint32 nv_element_size,
  rflte_nv_tbl_type *nv_tbl_type_ptr
);
#endif

void rflte_nv_lte_common_nv_init( void );

boolean rflte_nv_lte_common_nv_copy_rfnv_item( rfnv_item_id_enum_type  item_code,   
                                               rfnv_item_type          *rfnv_item_ptr,
                                               rfnv_item_size_type      nv_item_size );

rfcommon_nv_status_type rflte_nv_lte_common_nv_retrieve_items( rex_tcb_type *task_ptr, 
                                                               rex_sigs_type task_wait_sig,
                                                               void (*task_wait_handler)( rex_sigs_type ) );

uint16 rflte_common_nv_list_size_get( void );

void *rflte_nv_lte_common_nv_table_get( void );

boolean rflte_nv_process_rx_static_data
(
    rfnv_item_id_enum_type item_code,
    rflte_nv_tbl_type *nv_tbl_type_ptr,
    void *item_ptr
);
#ifndef FEATURE_RF_LTE_PAGE_ONLY
rfcommon_nv_status_type rflte_nv_lte_sar_swap_pos_retrieve_nv_items( rex_tcb_type *task_ptr, 
                                                          	     rex_sigs_type task_wait_sig,
                                                          	     void (*task_wait_handler)( rex_sigs_type ) );

boolean rflte_nv_lte_sar_swap_pos_copy_rfnv_item( rfnv_item_id_enum_type  item_code,   
                              			  rfnv_item_type          *rfnv_item_ptr,
                               			  rfnv_item_size_type      nv_item_size );


boolean rflte_nv_tx_max_pwr_vs_earfcn_static_extract(void *temp_data_ptr,
                                            int32 temp_num_bytes_left,
                                            uint32 nv_element_size,
                                            rflte_nv_tbl_type *nv_tbl_type_ptr);

boolean rflte_nv_ampr_backoff_static_extract(void *temp_data_ptr,
                                                                 int32 temp_num_bytes_left,
                                                                 uint32 nv_element_size,
                                             rflte_nv_tbl_type *nv_tbl_type_ptr);

boolean rflte_nv_mpr_v3_static_extract(void *temp_data_ptr,
                                                                 int32 temp_num_bytes_left,
                                                                 uint32 nv_element_size,
                                             rflte_nv_tbl_type *nv_tbl_type_ptr);


#endif
boolean
rflte_nv_populate_rx_gain_vs_freq_data
(
  rfnv_rx_cal_data_v3_type      *nv_tbl_type_ptr,
  void                         *rfnv_item_ptr,
  uint8                        num_gain_states
);

boolean
rflte_nv_populate_rx_gain_vs_freq_v3_nv
(
  rfnv_rx_cal_data_v3_type        *rx_cal_data_ptr,
  void                         *rfnv_item_ptr,
  uint8                        num_gain_states
);

rfcommon_nv_status_type rflte_nv_retrieve_items_all_bands
(
  rex_tcb_type *task_ptr,                     //!< pointer to tcb of the calling task
  rex_sigs_type task_wait_sig,                //!< signal that calling task waiting for
  void (*task_wait_handler)( rex_sigs_type )  //!< function ptr to callback function
);


#ifndef FEATURE_RF_LTE_PAGE_ONLY
boolean rflte_nv_tx_min_power_limit_control_static_data_copy(void *temp_data_ptr,
                                       int32 temp_num_bytes_left,
                                       uint32 nv_element_size,
                                       rflte_nv_tbl_type *nv_tbl_type_ptr);

                                    

boolean 
rflte_nv_ue_power_class_get_band_data
(
void *temp_data_ptr,
int32 temp_num_bytes_left,
uint32 nv_element_size,
rflte_nv_tbl_type *nv_tbl_type_ptr
);

uint8 rflte_nv_get_ue_power_class_per_band_catm1(rfcom_lte_band_type band);

uint8 rflte_nv_get_ue_power_class_per_band_nb1(rfcom_lte_band_type band);

boolean rflte_nv_max_tx_pwr_limit_adj_static_data_copy(void *temp_data_ptr,
                                                       uint32 nv_element_size,
                                                       rflte_nv_tbl_type *nv_tbl_type_ptr);                                     

boolean rflte_nv_max_tx_pwr_limit_adj_v3_static_data_copy(void *temp_data_ptr,
                                                       uint32 nv_element_size,
                                                       rflte_nv_tbl_type *nv_tbl_type_ptr);


#endif
/**
API to initialize nv table data for all lte bands
*/
void 
rflte_bands_nv_init
(
void
);													   
#endif /* RFLTE_NV_H */
