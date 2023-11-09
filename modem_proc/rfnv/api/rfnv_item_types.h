#ifndef RFNV_ITEM_TYPES_H
#define RFNV_ITEM_TYPES_H
/*===========================================================================

                             R F   N V  Data  Types

DESCRIPTION
  This header file contains the Data types and IDs  used for variant NVs

*/

#if defined(RFA_INTERNAL_NOTES)
#error code not present
#endif

/*

Copyright (c) 2012 - 2023 by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //components/rel/rfnv.mpss/1.0/api/rfnv_item_types.h#277 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/10/20   aki     SUB6 sawless type
06/10/20   wz      FR60197 QTuner ACL support for 256 impedance scenarios
05/27/20   aki     New tuner types
05/06/20   ljz     New V2 of AMPR SUPP type
04/27/20   tik     SAR datatypes for techs to support 40 indices.
04/23/20   aki     reserve 528, will be updated later
01/14/20   aki     RFNV_DATA_TYPE_KEY_VALUE
01/06/20   chx     TX_SAR_POWER_LIMIT_CONTROL_V5
12/20/19   ljz     LTE AMPR V3
12/10/19   ljz     20 DSI ca sar nv type 518
11/04/19   aki     Add sawless enable/disable type
10/29/19   aki     XPT DPD V3 add min/max bw fields
09/06/19   aki     XPT online duplexer eq - 514
08/22/19   tik     LPM LNA switch-point support for LTE (type 438)
06/11/19   aki     Add new espr type
05/16/19   aki     Add new spur type
05/13/19   aki     Update txfe data type memebr types and names
05/08/19   aki     Renamed spr/apt type to clipper settings
05/06/19   aki     Added spr/apt type
05/06/19   aki     Added txfe data type
05/03/19   aki     Added new DPD and XPT mode types
03/20/19   sm      Added  RFNV_DATA_ATUNER_MIMO_TC_TBL type
03/13/19   png     ENDC SAR type ID
03/06/19   png     New qet et cal type
02/14/19   xin     Updated MPR and AMPR types
07/01/19   tik     New GSM Tx cal datatypes to support 64 RGI cal [CT]
11/07/18   aki     New version of 302 type with updated RFM path members
12/20/18   xng     Add RFNV_DATA_TYPE_ENDC_PA_SWITCH_POINT
12/04/18   ljz     ADD CA_MAX_POWER_ADJ type 488
11/07/18   wxl     Add NV support Fast AGC mode
11/07/18   aki     New IDC types with 32-bit bandwidth field
10/15/18   ljz     RFNV_DATA_TYPE_ENDC_AMPR
09/13/18   ljz     Add RFNV_DATA_SWITCH_POINT_BACKOFF_MAX_MPR and RFNV_DATA_TYPE_TXAGC_CONTROL_V2
08/30/18   pkg     Added support for Max power based on DLCA victim and DLCA victim list variant NV's.
08/23/18   aki     Add char offset type
08/22/18   aki     Update RFNV_DATA_TYPE_STAND_ALONE_RX_CAL_OFFSET_V5
08/21/18   aki     Add type ca_rx_rsx
08/20/18   aki     Add RFNV_DATA_TYPE_STAND_ALONE_RX_CAL_OFFSET_V5
07/17/18   ank     New datatype for tx max power for CT
07/12/18   sm      Added RFNV_DATA_ATUNER_CL_DETUNE_TC_TBL
07/10/18   sg      FR 49966: SAR Backoff NV for Inter-band ULCA
07/04/18   sas     [FR49199]Adding NV structure for temp vs Max power
07/03/18   aki     Add XPT_MLINE_RES_TYPE_V2
06/07/18   aki     Add VOLTERRA_TX_REF_RMS_V2
05/31/18   ay      Added RFNV_DATA_ATUNER_SLEEP_TC_TBL,RFNV_DATA_ATUNER_LPM_TC_TBL
05/23/18   dx      Added RFNV_DATA_TYPE_LTE_SNR_SAWLESS_PARA
05/09/18   jj      Added FBRX_DROOP_FIR_FILTER_COEFF_V3
05/07/18   ndb     Added RFNV_DATA_TYPE_ATUNER_SUB_ANTENNA_SWITCH_SETTINGS_TYPE_V1
04/12/18   aki     Flip fail and bypass values in cal state enum for IDC
03/28/18   ljz     LTE chan2freq nv change
03/16/18   rmb     Add support for WCDMA DBDC and MIMO NV support.
03/15/18   ndb     Added datatypes needed for RFNV_TREE_ATUNER_SETTINGS
02/27/18   jr      FR44237: GNSS Blanking NV changes
02/27/18   pkg     Use 2 antenna's for legacy SAR NV instead of 4.
02/22/18   aki     Add XPT autopin type
02/22/18   aki     Overhaul of IDC types and added FBRX RSB type
02/21/18   aki     Add frequency field to IDC cal types
02/09/18   pkg     LTE 4 way AsDIV/PortABCD, FR42857: Enhanced TxAGC support for XSW.
02/05/18   aki     Rename LTE switch point enum
01/30/18   ndb     Added data types for Sub-antenna switching
01/23/18   esw     Add GSM freq based types
01/23/18   aki     Update LTE switch point type with 11 elements
01/19/18   aki     Add LTE switch packed type
01/17/18   aki     Add LTE switch point type with 11 elements
01/10/18   aki     XPT NV chan to freq updates - new variants
12/14/17   ndb     Added sub antenna switching NV data types
12/07/17   aki     Add IP2 cal type
12/04/17   aki     Rename WCDMA band combo to include VARIANT
12/04/17   aki     Add WCDMA band combo
10/28/17   and     Add 4x4 MIMO HORxD NV type for tuner
10/06/17   aki     Update freq based cal types
10/03/17   and     Add qps sharing type for tuner
09/21/17    ck     Updated Pcmax_adjust NV to Pemax_adjust
09/18/17   aki     Add rfdevice config data type
08/25/17   pkg     Add support to prioritize APT over ET in Intra ULCA
                   with < 10RB on each CC.
08/15/17   ck      Added new data type for LIM_ADJ_VS_FREQ_VS_TEMP
07/25/17   ck      Added new data type for PCMAX ADJUST
07/24/17   stw     New QPOET RSSI Thresold type
07/21/17   ck      Updated reserved field to ue_power_class_mask in types 216 and 409
07/21/17   mcao    New volterra V2 type
07/12/17   yzw     Add atuner tc table data type for reading ACL NV
07/14/17   aen     Update freq bases cal types
07/11/17   sam     Add new WCDMA variant Type for PAPM_RSSI switchpoint
07/07/17   ars     Added new type ID for NLIC IMD NV
06/28/17   ndb     Define a new data type for closed loop tune codes
06/21/17   aki     Move 5G types out of this file
06/21/17   aen     Add Frequency based Cal NV type
06/21/17    kr     Add RFNV_DATA_TYPE_TX_SAR_POWER_LIMIT_CONTROL_V2 support.
06/20/17   aki     Remove reserved field from types 335 and 336
06/19/17   aki     New 5 field RFM path types
05/16/17   aki     Update 5G type numbers
05/16/17   tt      AMPR NV based on UE Power Class.
05/09/17   aki     Add flat tree NV node types
05/08/17   aki     Add tree nv reference type
05/05/17   ndb     Define the IDs for UIACL V1 NVs
05/05/17   sk      Change reserve field to rgi_indices in RFNV_DATA_TYPE_LTE_TX_STATIC
04/27/17   aki     Add placeholders for 5G
04/24/17   sn      XPT ANALOG DELAY type updates
04/19/17   sn      Added XPT ANALOG DELAY typrs
04/14/17   ndb     Added atuner datatypes needed for new tree model NV
04/07/17   aen     Add V2 volterra type
03/17/17   dpk     Add new xpt txagc v2 type
03/22/17   sm      Added new data type for CA SCC activation band
                   Added new ACL params datatype
03/14/17   jj      Add new FBRX droop type
03/01/17   aki     Add atuner acl params and tree node variant ids
02/24/17   aki     Remove rfcom.h dependency
01/30/17   pv      FR 35442: New CA NV Data Type Variant ID:302 to support
                   max pwr limit in diff UL/DL combos
12/26/16   pk      Added SAR DSI support for CDMA
12/14/16   ase     Add GSM Narrow Band Jammer NV data type
12/14/16   cd      XPT modde control V2
12/09/16   vb      Framework for WTR6K NLIC harmonic cases
11/16/16   aki     Add support for IP2 cal data
11/07/16   ska/aki Add CDMA SAR NV variant
10/31/16   mcao    Add XPT_VOLTERRA_TX_REF_RMS
10/26/16   dh      Added new RFNV_DATA_TYPES for Active Detune, Detune Registration and Wifi sharing
10/21/16   rcui    Add FBRx DC data type
10/03/16   shk     Adding item types for GSM Tx internal cal
09/30/16   aki     Add SAWless JDET threshold variant
09/30/16   ani     Add Bandedge backoff RFNV data type
09/15/16   rmb     Replace LNA fall switchpoint with Hysteresis backoff which is again per switchpoint
                   since LNA Fall SP will be computed using LNA Rise, Hyst and LNA Offsets from Calibration.
07/26/16   ndb     Added CL CA band mask & CL tx offset datatypes
07/20/16   ska     Adding support for new 1x/HDR Tx Max Power NV
07/18/16   rb      GSM PA ICQ BIAS Datatype
07/18/16   asn     Update wcdma sar NV
07/08/16   ska     Update Rx Switchpoints variant structure
07/07/16   aki     Rename RFNV_RX_RSB_MAX_NUM_OF_BANDWITHS to _TAPS
07/07/16   pv/bmg  New LTE max power adjust data type for intra-CA and new
                   power classes.
07/07/16   ndb     Added a new datatype RFNV_DATA_TYPE_ATUNER_DETUNE_SETTINGS_TYPE
07/06/16   cd      XPT EPT Prime static NV variant support
06/10/16   ndb     Added datatypes needed for NV RFNV_ATUNER_SETTINGS
05/25/16   ljz     Added rx static field for DBFS switchpoint\Jammper detection\Notch filter
05/16/16   aro     Typo correction
05/11/16   hm      Added Data Types for Tuner NVs
05/12/16   rb      New GSM RX/TX STATIC Datatypes to support Port AB NV structure/Cal
05/12/16   aro     Added new WCDMA variant type for W static NV items
05/11/16   rb      New GSM RX/TX CAL Datatypes to support Port AB NV structure/Cal
05/10/16   aro     Repurpose reserved field in rfnv_txagc_control_static_data_t
                   as carrier_config
05/09/16   rsr     MLine type
05/03/16    pv     Added #of Active PA states to 217.
05/02/16   aki     Use old name for 226 not to break old builds
05/02/16   ska     Remove duplicate fields from 226 as they overlap with 217
04/26/16   sg/aki  Change the MPR/AMPR NV parameter to agg_bw_mask and support
                   for active cluster mask, add WTR ref path
04/21/16   rsr     Added TX reference path type and rename fields in sub header
04/19/16   aki     Added CDMA types
04/15/16    rb     Added new GSM Datatypes for ASDIV NV changes
04/14/16   asn     Added new structure for NV with LNA switch points
04/12/16   aki     Added type for RX offset cal data and reference path
04/04/16    pv     Added new Variant Datatypes for Tx Static Unification.
03/15/16   ndb     Added conflict detune priority logic data payload type
03/25/16   aki     Add RX cal_type enum
03/23/16   cd      Add XPT types
03/08/16   aki     Rename asdiv_pos to ant_swt
03/07/16   aki     Add RSB types and PACK macro for RX type
03/02/16   aki     Remove 'reserved' field, as it's for same purpose as asdiv_pos
02/22/16   dw      Added RFNV_DATA_DELTA_GAIN_PIN_POUT_TYPE
02/08/16   rcui    Add new FBRX temp comp type
12/22/15   aki     RX to 32-bit channel, added cal-type
12/14/15   gvn     Static NV re-structuring
12/17/15   mg      Add support to retrieve gapless 2DLCA band combos from NV and
                   advertise the same to RRC.
12/17/15   stw     Added datatypes for BW class and Number Antennas
12/15/15   kga     Added datatype to support spur data in RX static nv
12/04/15   mg      Added dataype to support 2DLCA gapless band combos
12/01/15   rb      Variant NV Re-architecture datatypes added
11/20/15   rcui    Add ilpc switch point NV
11/17/15    kr     Added datatype to support ca_ns mpr backoff override
11/17/15   ars     Added datatype to support NLIC IMD and Harmonic NVs.
11/16/15   ndb     Added Detune NV datatype & detune priority NV datatype
11/13/15    pv     Added datatype to support ca_ns ampr backoff override
11/06/15   aki     Channel size to uint16 to match NvDefintion.xml
10/26/16   ck      Add GSM FBRx Gain NV
10/23/15   mcao    Add Tx Multilin uinfied NV containers, volterra support.
10/15/15   kr      Added Multi cluster BW class support
10/14/15   aki     RX update and add RFNV_DATA_RF_CAL_RFM_DEVICE_NV_CONTAINER
09/30/15   aki     And going to uint8 to save some space
09/28/15   aki     Rename RX cal NV defs
09/24/15   aki     Added updated RX cal NV defs
09/18/15   kr      Added datatype to support ampr backoff
08/25/15   sbm     FBRx NV writer for AT.
09/02/15   kr/bmg  Added dataype to support pa bais offset based on earfcn
09/01/15   kr/bmg  Added datatype to support max tx pwr based on earfcn
09/01/15   bmg     Added Rx and Tx cal offset data types for ASDIV
07/12/15   stw     Added Limits on MAX UNIQUE COMBOS in NV 28874
06/23/15   yb      Added ACL parameter payload type
07/02/15   bmg     Added support for Tx STATIC Data NVs for LTE bands for
                   Inta-band ULCA.
07/01/15   sw/zlg  AsDiv with max power back off for SAR consideration
06/04/15   ndb     Added Sparameter payload type
05/18/15   zhw     Added CDMA Rx static NV data types
04/20/15    sk     Re-structure Tx path specific NV's per Tx NV container
02/25/15   bg      Added Common Atuner binning scheme data type
02/20/15   stw     Added LTE DL path support macros
02/12/15   stw     Added Unified static type and Intraband static type
01/26/15   aa      Added data type for fbrx therm adc
01/21/15   kg      Added support for Rx RSB calibration
12/09/14   yzw     Added Rx static and Rx alt path MTPL data type
11/17/14   bmg     Added Rx RSB calibration data type
11/06/14   vb/ka   Added LTE FBRx droop cal
10/13/14   kab     Added support for 3DLCA BC Config
10/07/14   cri     Added support for LTE interband CA BW
09/26/16   kai     Added support for new WCDMA RX static and cal NVs
09/10/14   pl      Added support for C2K cal PM cal_type
05/09/14   pl      Added support for common RX CAL data
02/24/14   aa      Updated gain cal for predet
01/30/14   aa      Updated fbrx gain cal params
11/12/13   aa      Added data type for RFNV_DATA_TYPE_FBRX_GAIN_V2
08/15/13   dw      Update Rx static and Cal type
08/12/13   vs      Added data for Rx static and cal type
07/16/13   dbz     Added data type for FBRX_GAIN type
03/05/13   aka     Added data type ID for RFNV_DATA_TYPE_TX_PWR_ERR_LIST
10/25/12   aka     New file for Variant Nv strcutures and IDs
===========================================================================*/

/*===========================================================================

                                 INCLUDE FILES

===========================================================================*/
#include "comdef.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER) || defined(__MINGW32__)
/*
  START of section to make visual studio happy in order for "PACK"
  to work properly for off-target testing.
  {
*/
#pragma pack( push, 1 )
#endif

#define RFNV_LINEAR_MAX_RGI_INDEX_PER_GAIN_STATE   32

#define RFNV_LINEAR_MAX_RGI_INDEX_PER_GAIN_STATE_V2   64

#define RFNV_LINEAR_MAX_SWPT_INDEX   5


#define GSM_AMAM_TBL_SIZE      128

#define GSM_AMPM_TBL_SIZE      128

#define RFNV_DATA_TYPE_ID_SIZE 2

#define RFNV_NUMBER_OF_PCLS    16

#define RFNV_MAX_LNA_GAIN_STATES     8
#define RFNV_MAX_STATIC_LNA_GAIN_STATES 12
#define RFNV_MAX_CAL_LNA_GAIN_STATES RFNV_MAX_STATIC_LNA_GAIN_STATES

#define WCDMA_NUM_LNA_STATES 5

/* MAX DL CA Combos need to increase with "RFNV_LTE_CA_BC_CONFIG_I" NV Item Capacity. */
#define LTE_MAX_DL_CA_COMBOS   128

/* MAX # of DL CA carriers Supported is 5 DLCA. */
#define LTE_DL_CA_MAX_NUM_CARRIERS    5

/* MAX # of Containers for independent Tx path is 2 */
#define LTE_TX_MAX_NUM_CONTAINERS    2

/* MAX # of PATHS supported per band : 10 for now taking into account 5 DL CA */
#define LTE_DL_MAX_NUM_PATHS_BAND    10

/* MAX # of PATHS supported per TRANSCEIVER : 10 taking into account 5DL CA present on one transceiver*/
#define LTE_DL_MAX_NUM_PATHS_TRX    10

/* MAX # of PATHS supported per RF Card : 10 taking into account 5DL CA*/
#define LTE_DL_MAX_NUM_PATHS_RFC    10

/* Max Number of Unique Bands in a bandwidth class combo */
#define NV_MAX_UNIQUE_BAND_COMBOS 6

/* Max Number of Unique Bands in a Gapless CA band combo */
#define NV_MAX_GAPLESS_BAND_COMBOS 2

#define RFNV_RX_PATH_LNA_SWTPS   2*RFNV_MAX_LNA_GAIN_STATES

#define RFNV_TX_STATIC_APT_CHAR_ENTRIES   32

#define RFNV_TX_MAX_PWR_ENTRIES  16

#define RFNV_TX_STATIC_AMPR_TABLE_SIZE 128

#define RFNV_TX_STATIC_MPR_TABLE_SIZE 128

#define RFNV_RX_STATIC_SPUR_TABLE_SIZE 8

#define RFNV_RX_STATIC_SUB6_SAWLESS_TABLE_SIZE 4

#define RFNV_TX_STATIC_MPR_CONT_RB_GRANT_TBL_SIZE        20

#define RFNV_TX_STATIC_AMPR_CONT_RB_GRANT_TBL_SIZE       20

#define RFNV_TX_STATIC_MPR_NC_RB_GRANT_TBL_SIZE       20

#define RFNV_TX_STATIC_AMPR_NC_RB_GRANT_TBL_SIZE       20

/* semi size based on ns number*/
#define RFNV_TX_STATIC_MPR_CONT_RB_SEMI_TBL_SIZE        6

#define RFNV_TX_STATIC_AMPR_CONT_RB_SEMI_TBL_SIZE       6

#define RFNV_TX_STATIC_MPR_NC_RB_SEMI_TBL_SIZE       6

#define RFNV_TX_STATIC_AMPR_NC_RB_SEMI_TBL_SIZE       6

#define RFNV_TX_STATIC_MPR_SUPPLEMENTARY_GRANT_TBL_SIZE        20

#define RFNV_TX_STATIC_AMPR_SUPPLEMENTARY_GRANT_TBL_SIZE       20

#define RFNV_TX_STATIC_MPR_SUPPLEMENTARY_SEMI_TBL_SIZE        6

#define RFNV_TX_STATIC_AMPR_SUPPLEMENTARY_SEMI_TBL_SIZE       6

#define RFNV_TX_DSDA_DIGITAL_BACKOFF_GRANT_TBL_SIZE       4

#define RFNV_TX_DSDA_DIGITAL_BACKOFF_GRANT_POWER_TBL_SIZE       10

#define RFNV_MAX_NUM_SPLIT_LNA_CASES 3

#define RFNV_ATUNER_MAX_TUNE_CODE_SIZE 8

#define RFNV_RX_RSB_MAX_NUM_OF_TAPS   8

#define RFNV_OZIF_RX_RSB_MAX_NUM_OF_FREQ_OFFSETS   9
#define RFNV_CA_RX_RSB_MAX_NUM_OF_FREQ_OFFSETS    39

#define RFNV_RX_RSB_MAX_NUM_OF_TONES              8
#define RFNV_FBRX_RSB_MAX_NUM_OF_TONES           22
#define RFNV_TX_RSB_MAX_NUM_OF_TONES             22

#define RFNV_LTE_TX_TEMP_BIN_SIZE 8

#define RFNV_LTE_TX_CHAN_SIZE 16

#define RFNV_LTE_TX_PA_STATE 8

#define RFNV_MAX_PA_STATE 8

#define RFNV_MAX_SAR_STATE 8

#define RFNV_MAX_SAR_STATE_V2 20

#define RFNV_MAX_SAR_STATE_V3 40

#define RFNV_HDET_OFFSET_SPAN_IND 2

#define RFNV_TEMP_TABLE_SIZ 8

#define RFNV_WCDMA_MPR_BIN_NUM 8

#define RFNV_WCDMA_SAR_BIN_NUM 8

#define RFNV_ATUNER_UIACL_MAX_FREQ_NUMBER 50

#define RFNV_WCDMA_MAX_NUM_NOTCH 20

#define RFNV_MAX_SAWLESS_JDET_THRESHOLD_OFFSETS 4

#define RFNV_CDMA_SAR_BIN_NUM 8

#define RFNV_MAX_NB_JDET_THRESHOLDS 4

#define RFNV_MAX_LTE_UL_BANDS_MAX_PWR_LIMIT_TBL_SIZE 5

#define RFNV_MAX_LTE_ALL_BANDS_MAX_PWR_LIMIT_TBL_SIZE 10

/*  Macro to define the maximum number of scenarios for ACL operations */
#define RFNV_ATUNER_MAX_CL_SCENARIOS  144
#define RFNV_ATUNER_MAX_CL_SCENARIOS_V2  255

#define RFNV_MAX_TX_AGGRESSOR_FREQ_BINS 4

/*! Maximum number of IM3 frequency range for EN-DC AMPR lookup */
#define RFNV_AMPR_IM3_FREQ_RANGE_SIZE 16

/*! Maximum number of RB alloc range for EN-DC AMPR lookup */
#define RFNV_AMPR_RB_ALLOC_RANGE_SIZE 4

/*! Maximum number of IM3 frequency range for EN-DC AMPR lookup */
#define RFNV_AMPR_IM3_FREQ_RANGE_SIZE_V2 16

/*! Maximum number of RB alloc range for EN-DC AMPR lookup */
#define RFNV_AMPR_RB_ALLOC_RANGE_SIZE_V2 8

/*! Maximum number of EN-DC PA switch point lookup */
#define RFNV_ENDC_PA_SWITCH_POINT_MAX_SIZE  20

/*! Maximum number of PA switch point sets */
#define RFNV_TX_MAX_PA_SWP_SETS 10

/*! Maximum number of QETs on an RF card supported by the et cal feature */
#define RFNV_QET_ET_CAL_MAX_ENTRIES 10

/*! Maximum number of BW number of ESPR */
#define RFNV_ESPR_BW_RANGE_SIZE 16

/*!
  @brief
  RFNV Number of Tx PA states supported in variant types
*/
#define RFNV_TX_NUM_PA_STATE                                                  8

/*!
  @brief
  RFNV channel list size used by Chan info type ID
*/
#define RFNV_CHAN_INFO_CHANNEL_LIST_SIZE                                     16
#define RFNV_CHAN_INFO_FREQ_LIST_SIZE                                        16

/*!
  @brief
  RFNV channel list size used by Chan info type ID for GSM tech
*/
#define RFNV_GSM_CHAN_INFO_CHANNEL_LIST_SIZE                                  3

/*!
  @brief
  RFNV temperature bin size
*/
#define RFNV_TEMP_BIN_SIZE                                                    8

#define RFNV_ATUNER_MAX_NUM_ANTENNAS 6

#define RFNV_ATUNER_AOL_TO_UI_MAPPING_MAX_UIS 9

#define RFNV_ATUNER_MAX_NUM_AOL 9

#define RFNV_ATUNER_MAX_NUM_ICL_SCENARIOS 3

#define RFNV_ATUNER_MAX_NUM_BANDS 8

#define RFNV_KEY_VALUE_MAX_KEY_SIZE     32
#define RFNV_KEY_VALUE_MAX_VALUE_SIZE  128

#define RFNV_ANTENNA_DETUNE_LIST_SIZE  12

#define RFNV_GSM_GNSS_BLANKING_MAX_SIZE 4

#define RFNV_RFM_ANTENNA_SIZE 16 // num of ant id, same to RFM_ANTENNA_MAX
#define RFNV_WCN_COEX_DESENSE_LIST_SIZE 4

#define RFNV_ASDIV_IMBALANCE_OFFSET_SIZE  16

#define RFNV_TXAGC_CTRL_STRUCT_SIZE_V4 4
#define RFNV_AUDIO_MAX_PA_STATE 6

enum{
  /* 0-99 is reserved for legacy data types, will be added later */
  RFNV_DATA_TYPE_ID_CAL_CHANS                        = 100,
  RFNV_DATA_TYPE_ID_FREQ_OFFSETS                     = 101,
  RFNV_DATA_TYPE_ID_LIN_TABLE_V3                     = 102,
  //RFNV_DATA_TYPE_ID_EPT_DPD_V1                     = 103, /* <-- Not used. Here only to acknowledge Nikel's EPT */
  RFNV_DATA_TYPE_ID_EPT_DPD_V2                       = 104,
  RFNV_DATA_TYPE_ID_ET_DPD                           = 105,
  RFNV_DATA_TYPE_ID_RGI_LIST                         = 106,   /* rfnv_data_rgi_list_type */
  RFNV_DATA_TYPE_ID_PMEAS_LIST                       = 107,   /* rfnv_data_pmeas_list_type */
  RFNV_DATA_TYPE_ID_AMAM_LIST                        = 108,   /* rfnv_data_amam_list_type */
  RFNV_DATA_TYPE_ID_AMPM_LIST                        = 109,   /* rfnv_data_ampm_list_type */
  RFNV_DATA_TYPE_TX_PWR_ERR_LIST                     = 110,   /* rfnv_data_pwr_err_list_type */
  RFNV_DATA_TYPE_REF_LIN_TEMP_ADC                    = 111,   /* rfnv_data_ref_lin_temp_adc_type */
  RFNV_DATA_TYPE_FBRX_GAIN                           = 112,   /* rfnv_data_fbrx_gain_type */
  RFNV_DATA_TYPE_ID_XPT_DPD_SCALING_V1               = 113,   /* rfnv_data_xpt_dpd_scaling_v1 */
  RFNV_DATA_TYPE_LNA_SWPTS_V1                        = 114,   /* rfnv_data_lna_swpts_type */
  RFNV_DATA_TYPE_RX_CAL_OFFSET_V1                    = 115,   /* rfnv_rx_cal_offset_type */
  RFNV_DATA_TYPE_RX_OFFSET_VS_FREQ_V1                = 116,   /* rfnv_rx_offset_vs_freq_type */
  RFNV_DATA_TYPE_FBRX_GAIN_V2                        = 117,   /* rfnv_data_fbrx_gain_type_v2 */
  RFNV_DATA_TYPE_RX_CAL_OFFSET_V2                    = 118,   /* rfnv_rx_cal_data_typeid_118_type in rflte_nv.h */
  RFNV_DATA_TYPE_ID_LIN_TABLE_V4                     = 119,   /* Multi Lin NV with Tx RSB and LOFT correction  */
  RFNV_DATA_TYPE_LNA_SWPTS_V2                        = 120,   /* rfnv_data_lna_swpts_type_v2 */
  RFNV_DATA_TYPE_LNA_SWPTS_V3                        = 122,   /* rfnv_data_lna_swpts_type_v3 */
  RFNV_DATA_TYPE_ID_LTE_FBRX_DROOP_FIR_FILTER_COEFF  = 123,   /* FBRx internal data structure: rfcommon_fbrx_nv_lte_bw_params_droop_type */
  RFNV_DATA_TYPE_RXRSB_V1                            = 124,   /* rfnv_data_type_rxrsb_v1 */
  RFNV_DATA_TYPE_LTE_RX_STATIC                       = 125,   /* rfnv_rx_static_data_type*/
  RFNV_DATA_TYPE_LTE_RX_ALT_PATH_MTPL                = 126,   /* rfnv_rx_alt_path_mtpl_data_type*/
  RFNV_DATA_TYPE_FBRX_THERM_ADC_V1                   = 127,   /* rfcommon_fbrx_therm_adc_val_type */
  RFNV_DATA_TYPE_LTE_RX_STATIC_INTRABAND             = 128,   /* rfnv_rx_static_data_type*/
  RFNV_DATA_TYPE_ATUNER_BINNING_SCHEME_TYPE          = 129,   /* Common Atuner binning scheme data type */
  RFNV_DATA_TYPE_1X_LNA_SWITCHPOINTS                 = 130,   /* rfnv_rx_swpt_type */
  RFNV_DATA_TYPE_DO_REVA_LNA_SWITCHPOINTS            = 131,   /* rfnv_rx_swpt_type */
  RFNV_DATA_TYPE_DO_REVB_SC_LNA_SWITCHPOINTS         = 132,   /* rfnv_rx_swpt_type */
  RFNV_DATA_TYPE_DO_REVB_MC_LNA_SWITCHPOINTS         = 133,   /* rfnv_rx_swpt_type */
  RFNV_DATA_TYPE_1X_SPUR_TABLE                       = 134,   /* rfcommon_spur_table_type */
  RFNV_DATA_TYPE_DO_SPUR_TABLE                       = 135,   /* rfcommon_spur_table_type */
  RFNV_DATA_TYPE_ACL_PARAMS_BIN_TYPE                 = 136,   /* ACL Parameters Bin type */
  RFNV_DATA_TYPE_DL_CA_BW_COMBO                      = 137,
  RFNV_DATA_TYPE_UL_CA_BW_COMBO                      = 138,
  RFNV_DATA_TYPE_ID_SAR_BACKOFF                      = 139,   /* rfnv_data_sar_backoff_type */
  RFNV_DATA_TYPE_LTE_TX_STATIC                       = 140,   /* rfnv_tx_static_data_type */
  RFNV_DATA_TYPE_ASDIV_RX_GAIN_OFFSET                = 141,   /* rfnv_asdiv_rx_gain_offset_type */
  RFNV_DATA_TYPE_ASDIV_RX_SWITCH_POINTS              = 142,   /* rfnv_asdiv_rx_switch_points_type */
  RFNV_DATA_TYPE_ASDIV_TX_GAIN_OFFSET                = 143,   /* rfnv_asdiv_tx_gain_offset_type */
  RFNV_DATA_TYPE_LTE_TX_MAX_POWER_BASED_ON_EARFCN    = 144,   /* rfnv_max_tx_pwr_vs_earfcn_static_data_type */
  RFNV_DATA_TYPE_PA_BIAS_OFFSET_CHANNELS             = 145,   /*rfnv_data_pa_bias_combo_offset_type*/
  RFNV_DATA_TYPE_FBRX_GAIN_V3                        = 146,   /* rfcommon_fbrx_ref_chan_gain_nv_type */
  RFNV_DATA_FBRX_CHAN_LIST                           = 147,   /* rfcommon_fbrx_channel_nv_type */
  RFNV_DATA_TYPE_FBRX_THERM_ADC_V2                   = 148,   /* rfcommon_fbrx_therm_adc_nv_type */
  RFNV_DATA_FBRX_FREQ_COMP_GAIN_TYPE                 = 149,   /* rfcommon_fbrx_freq_comp_gain_nv_type */
  RFNV_DATA_FBRX_DC_CAL_TYPE                         = 150,   /* rfcommon_fbrx_dc_cal_nv_type */
  RFNV_DATA_TYPE_LTE_AMPR                            = 151,   /* rfnv_ampr_static_data_type*/
  RFNV_DATA_TYPE_STAND_ALONE_RX_CAL_OFFSET_V3        = 152,   /* rfnv_rx_stand_alone_lna_data_v3 */
  RFNV_DATA_TYPE_SPLIT_RX_CAL_OFFSET_V3              = 153,   /* rfnv_rx_split_lna_data_v3 */
  RFNV_DATA_RF_CAL_RFM_DEVICE_NV_CONTAINER           = 154,   /* rfnv_variant_element_header */
  RFNV_DATA_TYPE_MULTI_CLUSTER_BW_CLASS_I            = 155,   /* rfnv_multi_cluster_bw_class_data_type*/
  RFNV_DATA_TYPE_ID_VOLTERRA_EPT                     = 156,   /* tx_ept_volterra_nv_type */
  RFNV_DATA_TYPE_ID_VOLTERRA_ET                      = 157,   /* tx_et_volterra_nv_type */
  RFNV_DATA_GSM_FB_GAIN_TYPE                         = 158,   /* rfnv_data_gsm_fb_gain_type */
  RFNV_DATA_TYPE_LTE_CA_NS_AMPR                      = 159,   /* rfnv_ca_ns_ampr_static_data_type*/
  RFNV_DATA_TYPE_ATUNER_DETUNE_DATA_TYPE             = 160,   /* detune datatype*/
  RFNV_DATA_TYPE_ATUNER_DETUNE_PRIORITY_TABLE_TYPE   = 161,   /* detune priority table type*/
  RFNV_DATA_TYPE_NLIC_HARMONIC_PARAMS                = 162,
  RFNV_DATA_TYPE_NLIC_IMD_PARAMS                     = 163,
  RFNV_DATA_TYPE_LTE_CA_NS_MPR                       = 164,   /* rfnv_ca_ns_mpr_static_data_type*/
  RFNV_DATA_TYPE_FBRX_ILPC_SWITCHPOINTS              = 165,   /* rfcommon_fbrx_ilpc_switch_pt_nv_type */
  RFNV_DATA_TYPE_FBRX_XPT_SWITCHPOINTS               = 166,
  RFNV_DATA_TYPE_FBRX_SELFTEST_SWITCHPOINTS          = 167,
  RFNV_DATA_TYPE_GSM_TX_TIMING_I_LIST                = 168,   /*Variant NV dataypes*/
  RFNV_DATA_TYPE_GSM_EXTENDED_PA_SWPT_I_LIST         = 169,
  RFNV_DATA_TYPE_GSM_TX_GTR_THRESH_I_LIST            = 170,
  RFNV_DATA_TYPE_VBATT_I_LIST                        = 171,
  RFNV_DATA_TYPE_ENV_GAIN_I_LIST                     = 172,
  RFNV_DATA_TYPE_AMAM_TEMP_COMP_I_LIST               = 173,
  RFNV_DATA_TYPE_TEMP_COMP_I_LIST                    = 174,
  RFNV_DATA_TYPE_ENH_TEMP_COMP_I_LIST                = 175,
  RFNV_DATA_TYPE_LINEAR_TX_GAIN_PARAM_I_LIST         = 176,
  RFNV_DATA_TYPE_MULTISLOT_MAX_TX_PWR_I_LIST         = 177,
  RFNV_DATA_TYPE_POWER_LEVELS_I_LIST                 = 178,
  RFNV_DATA_TYPE_EXTENDED_SMPS_PDM_TBL_I_LIST        = 179,
  RFNV_DATA_TYPE_EXTENDED_SAR_BACKOFF_I_LIST         = 180,
  RFNV_DATA_TYPE_EXTENDED_POLAR_RAMP_PROFILE_I_LIST  = 181,
  RFNV_DATA_TYPE_EXTENDED_PA_RANGE_MAP_I_LIST        = 182,
  RFNV_DATA_TYPE_ENH_APT_I_LIST                      = 183,
  RFNV_DATA_TYPE_PA_ICQ_BIAS_LIST                    = 184,
  RFNV_DATA_TYPE_CAL_TEMP_I_LIST                     = 185,
  RFNV_DATA_TYPE_ANT_TUNER_CL_I_LIST                 = 186,
  RFNV_DATA_TYPE_COEX_TX_OFFSET_INFO_TBL_LIST        = 187,
  RFNV_DATA_TYPE_RX_CAL_DATA_I_LIST                  = 188,
  RFNV_DATA_TYPE_TX_CAL_CHAN_I_LIST                  = 189,
  RFNV_DATA_TYPE_TX_CAL_AMAM_RGI_I_LIST              = 190,
  RFNV_DATA_TX_CAL_RGI_LIST_TYPE                     = 191,
  RFNV_DATA_TX_CAL_PMEAS_LIST_TYPE                   = 192,
  RFNV_DATA_TX_CAL_AMAM_LIST_TYPE                    = 193,
  RFNV_DATA_TX_CAL_PWR_ERR_LIST_TYPE                 = 194,
  RFNV_DATA_TX_CAL_AMPM_LIST_TYPE                    = 195,
  RFNV_DATA_TYPE_GSM_LNA_SWPT_LIST                   = 196,  /*RX Items*/
  RFNV_DATA_TYPE_GSM_TX_RX_ANT_TUNER_LIST            = 197,
  RFNV_DATA_TYPE_GSM_COEX_RX_OFFSET_INFO_TBL_LIST    = 198,
  RFNV_DATA_TYPE_2DLCA_GAPLESS_BAND_COMBO            = 199,
  RFNV_DATA_TYPE_STATIC_SPUR_DATA_LIST               = 200,
  RFNV_DATA_TYPE_DL_CA_BW_ANTENNA_COMBO              = 201,
  RFNV_DATA_TYPE_UL_CA_BW_ANTENNA_COMBO              = 202,
  RFNV_DATA_TYPE_LTE_RX_SWITCH_POINT_CONTAINER       = 203,
  RFNV_DATA_DELTA_GAIN_PIN_POUT_TYPE                 = 204,
  RFNV_DATA_FB_GAIN_VS_TEMP_VS_FREQ_TYPE             = 205,
  RFNV_DATA_VARIANT_SUB_HEADER_V2                    = 206, /* rfnv_variant_element_header_v2 */
  RFNV_DATA_RSB_CAL_BB_VEC_TYPE                      = 207, /* rfnv_data_rsb_cal_bb_vec_type */
  RFNV_DATA_TYPE_TX_LIN_VS_TEMP_FREQ                 = 208, /* rfnv_tx_lin_vs_freq_vs_temp_data_type */
  RFNV_DATA_TYPE_CHAN_INFO                           = 209, /* rfnv_data_chan_info_type */
  RFNV_DATA_TYPE_ID_XPT_MODE_CONTROL                 = 210, /* rfnv_xpt_static_mode_ctrl_type */
  RFNV_DATA_TYPE_ID_XPT_PIN_POUT                     = 211, /* rfnv_xpt_static_pin_pout_data_type */
  RFNV_DATA_TYPE_ID_XPT_ANALOG_DELAY                 = 212, /* rfnv_xpt_static_analog_delay_data_type */
  RFNV_DATA_TYPE_ID_XPT_ANALOG_DELAY_OFFSETS         = 213, /* rfnv_xpt_static_analog_delay_offsets_data_type*/
  RFNV_DATA_TYPE_ID_XPT_TXAGC                        = 214, /* rfnv_xpt_static_txagc_data_type */
  RFNV_DATA_TYPE_LTE_MPR                             = 215, /* rfnv_mpr_static_data_t */
  RFNV_DATA_TYPE_TX_SAR_POWER_LIMIT_CONTROL          = 216, /* rfnv_tx_sar_power_limit_control_static_data_t */
  RFNV_DATA_TYPE_TXAGC_CONTROL                       = 217, /* rfnv_txagc_control_static_data_t */
  RFNV_DATA_TYPE_TX_MIN_POWER_LIMIT_CONTROL          = 218, /* rfnv_tx_min_power_limit_control_static_data_t */
  RFNV_DATA_ATUNER_CONFLICT_DETUNE_PRIORITY_LOGIC_TYPE   = 219, /*conflict detune priority logic payload type*/
  RFNV_DATA_TYPE_STAND_ALONE_RX_CAL_CHAR_OFFSET_V3   = 220,  /* rfnv_rx_stand_alone_offset_lna_data_v3 */
  RFNV_DATA_VARIANT_REFERENCE_SUB_HEADER_V2          = 221,  /* rfnv_variant_element_ref_header */
  RFNV_GSM_EXTENDED_PA_SWPT_DATA_TYPE                = 222,  /* NEW GSM TYPES */
  RFNV_GSM_MULTISLOT_MAX_TX_PWR_DATA_TYPE            = 223,
  RFNV_GSM_LNA_SWPT_DATA_TYPE                        = 224,  /* NEW GSM TYPES */
  RFNV_DATA_TYPE_TDS_TX_MAX_POWER_LIMIT_CONTROL      = 225,  /* rfnv_tds_tx_max_power_limit_control_static_data_t*/
  RFNV_DATA_TYPE_CDMA_PA_SWITCHPOINTS_TYPE           = 226,  /* rfnv_data_type_cdma_pa_switchpoints */
  RFNV_DATA_TYPE_CDMA_PA_STATIC                      = 226,  /* rfnv_data_type_cdma_pa_static */ // INTENTIONAL DUPLICATE
  RFNV_DATA_TYPE_CDMA_RX_SWITCHPOINTS_TYPE           = 227,  /* rfnv_data_type_cdma_rx_switchpoints*/
  RFNV_DATA_TYPE_CDMA_ENC_BTF_TYPE                   = 228,  /* rfnv_data_type_cdma_enc_btf*/
  RFNV_DATA_TYPE_LTE_CA_MPR                          = 229,  /* rfnv_ca_mpr_static_data_type */
  RFNV_DATA_OFFSET_TX_FREQ_WRT_REFPATH_TYPE          = 230,
  RFNV_DATA_TYPE_PA_COMP_CONTROL                     = 231,  /* rfnv_data_type_pa_comp_control*/
  RFNV_DATA_TYPE_XPT_MLINE_RES_TYPE                  = 232,  /* rfnv_data_type_mline_results */
  RFNV_GSM_RX_CAL_DATA_TYPE                          = 233, /*New GSM CAL Rx Type*/
  RFNV_GSM_TX_CAL_CHAN_DATA_TYPE                     = 234, /*New GSM CAL Tx Type begin*/
  RFNV_GSM_TX_CAL_AMAM_RGI_DATA_TYPE                 = 235,
  RFNV_GSM_TX_CAL_RGI_DATA_TYPE                      = 236,
  RFNV_GSM_TX_CAL_PMEAS_DATA_TYPE                    = 237,
  RFNV_GSM_TX_CAL_AMAM_DATA_TYPE                     = 238,
  RFNV_GSM_TX_CAL_PWR_ERR_DATA_TYPE                  = 239,
  RFNV_GSM_TX_CAL_AMPM_DATA_TYPE                     = 240,
  RFNV_WCDMA_VARIANT_TX_POWER                        = 241, /* rfnv_wcdma_variant_tx_power_t */
  RFNV_WCDMA_VARIANT_TX_TIMING                       = 242, /* rfnv_wcdma_variant_tx_timing_t */
  RFNV_WCDMA_VARIANT_TX_MPR                          = 243, /* rfnv_wcdma_variant_tx_mpr_t */
  RFNV_WCDMA_VARIANT_SAR                             = 244, /* rfnv_wcdma_variant_tx_sar_t */
  RFNV_WCDMA_VARIANT_RX_NOISE_FLOOR                  = 245, /* rfnv_wcdma_variant_rx_noise_floor_t */
  RFNV_WCDMA_VARIANT_BTF                             = 246, /* rfnv_wcdma_variant_btf_t */
  RFNV_GSM_TX_TIMING_DATA_TYPE                       = 247,
  RFNV_GSM_TX_GTR_THRESH_DATA_TYPE                   = 248,
  RFNV_GSM_VBATT_DATA_TYPE                           = 249,
  RFNV_GSM_ENV_GAIN_DATA_TYPE                        = 250,
  RFNV_GSM_AMAM_TEMP_COMP_DATA_TYPE                  = 251,
  RFNV_GSM_TEMP_COMP_DATA_TYPE                       = 252,
  RFNV_GSM_ENH_TEMP_COMP_DATA_TYPE                   = 253,
  RFNV_GSM_LINEAR_TX_GAIN_PARAM_DATA_TYPE            = 254,
  RFNV_GSM_POWER_LEVELS_DATA_TYPE                    = 255,
  RFNV_GSM_EXTENDED_SMPS_PDM_TBL_DATA_TYPE           = 256,
  RFNV_GSM_EXTENDED_SAR_BACKOFF_DATA_TYPE            = 257,
  RFNV_GSM_EXTENDED_POLAR_RAMP_PROFILE_DATA_TYPE     = 258,
  RFNV_GSM_EXTENDED_PA_RANGE_MAP_DATA_TYPE           = 259,
  RFNV_GSM_ENH_APT_DATA_TYPE                         = 260,
  RFNV_GSM_PA_ICQ_BIAS_DATA_TYPE                     = 261,
  RFNV_GSM_CAL_TEMP_DATA_TYPE                        = 262,
  RFNV_GSM_ANT_TUNER_CL_DATA_TYPE                    = 263,
  RFNV_GSM_COEX_TX_OFFSET_INFO_TBL_DATA_TYPE         = 264,
  RFNV_DATA_TYPE_ATUNER_UIACL_PARAMS_ANTENNA_GROUP_DELIMITER    = 265,  /* rfnv_atuner_uiacl_params_antenna_group_delimiter */
  RFNV_DATA_TYPE_ATUNER_UIACL_SCENARIO_TYPE                     = 266,  /* rfnv_atuner_uiacl_scenario_type */
  RFNV_DATA_TYPE_TDS_TX_CHAN_LIST                               = 267,
  RFNV_DATA_TYPE_TDS_MPR_VAL_V2                                 = 268,
  RFNV_DATA_TYPE_TDS_LNA_RANGE_RISE_FALL                        = 269,
  RFNV_WCDMA_VARIANT_RX_LNA_SP_TYPE                             = 270,  /* rfnv_wcdma_variant_rx_lna_sp_t */
  RFNV_WCDMA_VARIANT_RX_JAMMER_THRES_TYPE                       = 271,  /* rfnv_wcdma_variant_rx_jammer_thres_t */
  RFNV_WCDMA_VARIANT_RX_NOTCH_TYPE                              = 272,  /* rfnv_wcdma_variant_rx_notch_t */
  RFNV_DATA_TYPE_ATUNER_SETTINGS_ANTENNA_GROUP_DELIMITER        = 273,
  RFNV_DATA_TYPE_ATUNER_AOL_TO_UI_MAPPING_TYPE                  = 274,
  RFNV_DATA_TYPE_ATUNER_AOL_TO_ICL_MAPPING_TYPE                 = 275,
  RFNV_DATA_TYPE_ATUNER_MAAT_SETTINGS_TYPE                      = 276,
  RFNV_DATA_TYPE_ATUNER_SETTINGS_TYPE                           = 277,
  RFNV_DATA_TYPE_ID_XPT_EPT_PRIME                               = 278, /* rfnv_xpt_static_ept_prime_type */
  RFNV_DATA_TYPE_ATUNER_DETUNE_SETTINGS_TYPE                    = 279,
  RFNV_DATA_TYPE_TX_MAX_POWER_LIMIT_ADJUST                      = 280, /* rfnv_max_tx_pwr_limit_adj_static_data_t*/
  RFNV_DATA_TYPE_1X_TX_LIM_VS_TEMP_TYPE                         = 281, /*rfnv_data_type_1x_tx_lim_vs_temp*/
  RFNV_DATA_TYPE_DO_TX_LIM_VS_TEMP_TYPE                         = 282, /*rfnv_data_type_hdr_tx_lim_vs_temp*/
  RFNV_GSM_PA_ICQ_BIAS_ENABLE_DATA_TYPE                         = 283,
  RFNV_DATA_TYPE_ATUNER_CL_TX_OFFSETS_SETTINGS_TYPE             = 284,
  RFNV_DATA_TYPE_ATUNER_CL_CA_PCC_EXCLUDED_BANDS_TYPE           = 285,
  RFNV_DATA_TYPE_ATUNER_CL_CA_SCC_EXCLUDED_BANDS_TYPE           = 286,
  RFNV_GSM_MOD_EXTENDED_SMPS_PDM_TBL_DATA_TYPE                  = 287, /* Add SMPS bias per modulation */
  RFNV_DATA_TYPE_LTE_TX_BANDEDGE_BACKOFF                        = 288, /* rfnv_lte_bandedge_backoff_type */
  RFNV_DATA_TYPE_SAWLESS_JDET_THRESHOLD_OFFSET                  = 289, /* rfnv_sawless_jdet_threshold_offset */
  RFNV_DATA_TYPE_FBRX_DC                                        = 290, /* rfcommon_fbrx_dc_nv_type */
  RFNV_DATA_TYPE_ATUNER_USE_ANT_FOR_DETUNE_TYPE                 = 291, /*rfnv_atuner_detune_registration*/
  RFNV_DATA_TYPE_ATUNER_WIFI_SHARING_TYPE                       = 292, /* RFNV_DATA_TYPE_ATUNER_MULTI_RADIO_COEXISTENCE_TYPE in NvDefiniton File*/
  RFNV_DATA_TYPE_ATUNER_CA_PCC_ACTIVE_DETUNE_BANDS_TYPE         = 293,
  RFNV_DATA_TYPE_ATUNER_CA_SCC_ACTIVE_DETUNE_BANDS_TYPE         = 294,
  RFNV_DATA_TYPE_ID_XPT_VOLTERRA_TX_REF_RMS                     = 295, /* tx_volterra_dpd_rms_nv_type */
  RFNV_DATA_TYPE_CDMA_VARIANT_SAR                               = 296, /* rfnv_data_type_cdma_variant_tx_sar_t */
  RFNV_DATA_TYPE_IP2_CAL                                        = 297, /* rfnv_data_type_ip2_cal_t */
  RFNV_DATA_TYPE_NLIC_HARMONIC_PARAMS_ENHANCED                  = 298,
  RFNV_DATA_TYPE_NLIC_IMD_PARAMS_ENHANCED                       = 299,
  RFNV_DATA_TYPE_ID_XPT_MODE_CONTROL_V2                         = 300, /* rfnv_xpt_static_mode_ctrl_v2_type */
  RFNV_DATA_TYPE_NBJDET_THRESHOLDS_TYPE                         = 301, /* rfnv_nbjdet_thresholds_type */
  RFNV_DATA_TYPE_LTE_CA_TX_MAX_POWER_LIMIT                      = 302, /* rfnv_ca_max_tx_pwr_limit_static_data_t  */
  RFNV_DATA_TYPE_TDS_SPUR_LIST_TYPE                             = 303, /* tds spur freq list type*/
  RFNV_DATA_TYPE_ATUNER_ACL_PARAMS_PATH_GROUP_DELIMITER         = 304, /*rfnv_atuner_acl_params_ant_group_delimiter_data_type */
  RFNV_DATA_TYPE_NV_TREE_NODE_TYPE                              = 305,
  RFNV_DATA_TYPE_ID_LTE_FBRX_DROOP_FIR_FILTER_COEFF_V2          = 306, /* FBRx internal data structure: rfcommon_fbrx_nv_lte_bw_params_droop_v2_type */
  RFNV_DATA_TYPE_ATUNER_CA_SCC_ACTIVATION_BANDS_TYPE            = 307, /* rfnv_atuner_ca_scc_activation_bands_data_type */
  RFNV_DATA_TYPE_ATUNER_ACL_PARAMS_ANTENNA_GROUP_DELIMITER      = 308, /* rfnv_atuner_acl_params_ant_group_delimiter_data_type */
  RFNV_DATA_TYPE_ID_XPT_TXAGC_V2                                = 309, /* rfnv_xpt_static_txagc_data_v2_type */
  RFNV_DATA_TYPE_ID_VOLTERRA_ET_V2                              = 310, /* tx_et_volterra_v2_nv_type  */
  RFNV_DATA_ATUNER_ACTIVE_TC_TBL_IDX                            = 311, /* Active Tune code table idx */
  RFNV_DATA_ATUNER_DETUNE_TC_TBL_IDX                            = 312, /* Detune Tune code table idx */
  RFNV_DATA_ATUNER_AAT_TBL_IDX                                  = 313, /* AAT table idx */
  RFNV_DATA_ATUNER_TC_TBL_DATA                                  = 314, /* Tune code table data */
  RFNV_DATA_TYPE_ID_XPT_ANALOG_DELAY_V2                         = 315, /* rfnv_xpt_static_analog_delay_v2_data_type */
  RFNV_DATA_TYPE_ID_XPT_ANALOG_DELAY_OFFSETS_V2                 = 316, /* rfnv_xpt_static_analog_delay_offsets_v2_data_type*/
  RFNV_DATA_TYPE_ATUNER_UIACL_IMPEDANCE_TYPE                    = 317,
  RFNV_DATA_TYPE_ATUNER_UIACL_AAT_TBL_TYPE                      = 318,
  RFNV_DATA_TYPE_ATUNER_UIACL_PARAMS_ANT_GR_DELIMITER_V1        = 319,
  RFNV_DATA_TYPE_NV_TREE_NODE_DATA_REF_TYPE                     = 320,
  RFNV_DATA_TYPE_NV_TREE_NODE_HEADER_TYPE                       = 321,
  RFNV_DATA_TYPE_NV_TREE_NODE_ITEM_HEADER_TYPE                  = 322,
  RFNV_DATA_TYPE_NV_TREE_HEADER_TYPE                            = 323,
  RFNV_DATA_TYPE_RFDEVICE_CONFIG                                = 324, /* rfnv_rfdevice_config_t */
  RFNV_DATA_TYPE_DL_CA_BW_ANTENNA_VECTOR_COMBO                  = 333,
  RFNV_DATA_TYPE_UL_CA_BW_ANTENNA_VECTOR_COMBO                  = 334,
  RFNV_DATA_VARIANT_SUB_HEADER_V3                               = 335,  /* rfnv_variant_element_header_v3 */
  RFNV_DATA_VARIANT_REFERENCE_SUB_HEADER_V3                     = 336,  /* rfnv_variant_element_ref_header_v3 */
  RFNV_DATA_TX_LIN_V5_TYPE_RSB                                  = 337,
  RFNV_DATA_FREQ_LIST_TYPE                                      = 338, /* rfnv_data_freq_info_type Use for Tx, Rx and FBRx frequency list */
  RFNV_DATA_TYPE_STAND_ALONE_RX_CAL_OFFSET_V4                   = 339, /* rfnv_rx_stand_alone_lna_data_v4 */
  RFNV_DATA_TYPE_STAND_ALONE_RX_CAL_CHAR_OFFSET_V4              = 340,
  RFNV_DATA_ATUNER_CL_TC_TBL_DATA                               = 342,
  RFNV_DATA_TYPE_QPOET_RSSI_THRESH                              = 343,
  RFNV_DATA_TYPE_FBRX_GAIN_V4                                   = 344, /* rfcommon_fbrx_ref_freq_gain_nv_type */
  RFNV_DATA_TYPE_TX_SAR_POWER_LIMIT_CONTROL_V2                  = 409, /* rfnv_tx_sar_power_limit_control_v2_static_data_t */
  RFNV_DATA_TYPE_NLIC_IMD_PARAMS_ENHANCED_V2                    = 410,
  RFNV_WCDMA_VARIANT_PAPM_RSSI_SWITCHPOINT                      = 411, /* rfnv_wcdma_variant_papm_rssi_switchpoint_t */
  RFNV_DATA_TYPE_ID_VOLTERRA_EPT_V2                             = 412, /* tx_ept_volterra_v2_nv_type  */
  RFNV_DATA_TYPE_PEMAX_ADJUST                                   = 413, /* rfnv_pemax_adj_static_data_t */
  RFNV_DATA_TYPE_CDMA_PAPM_RSSI_SWITCHPOINT                     = 414, /* rfnv_data_type_cdma_papm_rssi_switchpoint_t */
  RFNV_DATA_TYPE_TX_MAX_PWR_LIM_ADJ_VS_FREQ_VS_TEMP             = 415, /* rfnv_max_tx_pwr_lim_adj_vs_freq_vs_temp_static_data_t */
  RFNV_DATA_TYPE_LTE_TX_INTRA_ULCA_APT_ET_CONTROL               = 416, /* rfnv_tx_intra_ulca_apt_et_control_t */
  RFNV_DATA_TYPE_NLIC_HARMONIC_PARAMS_ENHANCED_V3               = 417,
  RFNV_DATA_TYPE_NLIC_IMD_PARAMS_ENHANCED_V3                    = 418,
  RFNV_DATA_TYPE_ATUNER_GPS_SHARING_TYPE                        = 419, /*rfnv for antenna shared with GPS*/

  RFNV_DATA_FBRX_RSB_CAL_BB_VEC_TYPE                            = 423, /* rfnv_data_fbrx_rsb_cal_bb_vec_type */
  RFNV_DATA_JRF_CAL_RESULT_TYPE                                 = 424, /* rfnv_data_type_jrf_cal */
  RFNV_DATA_RX_RSB_CAL_BB_VEC_TYPE                              = 425, /* rfnv_data_rx_rsb_cal_bb_vec_type */
  RFNV_DATA_OZIF_RX_RSB_CAL_BB_VEC_TYPE                         = 426, /* rfnv_data_ozif_rsb_cal_bb_vec_type */
  RFNV_DATA_TX_RSB_CAL_BB_VEC_TYPE                              = 427,

  RFNV_DATA_ATUNER_4X_TC_TBL_IDX                                = 429, /* 4x4 MIMO and HORxD code table idx */
  RFNV_WCDMA_VARIANT_DBDC_BAND_COMBO                            = 430, /* WCDMA DBDC Band combinations */
  RFNV_DATA_TYPE_IIP2_CAL_VS_TEMP_VS_FREQ                       = 431, /* rfnv_data_type_iip2_cal_vs_temp_vs_freq */
  RFNV_DATA_TYPE_ATUNER_SUB_ANTENNA_SWITCH_SETTINGS_TYPE        = 432,
  RFNV_DATA_ATUNER_CL_DETUNE_TC_TBL_IDX                         = 433,
  RFNV_DATA_ATUNER_CL_DETUNE_4X_TC_TBL_IDX                      = 434,
  RFNV_DATA_TYPE_ID_XPT_PIN_POUT_V2                             = 435, /* rfnv_xpt_static_pin_pout_v2_data_type */
  RFNV_DATA_TYPE_ID_XPT_ANALOG_DELAY_V3                         = 436, /*rfnv_xpt_static_analog_delay_v3_data_type */
  RFNV_DATA_TYPE_ID_XPT_ANALOG_DELAY_OFFSETS_V3                 = 437, /* rfnv_xpt_static_analog_delay_offsets_v3_data_type*/
  RFNV_DATA_TYPE_LTE_RX_SWITCH_POINT_CONTAINER_V2               = 438, /*rfnv_lte_rx_switch_point_container_v2_data_type*/
  RFNV_DATA_TYPE_GSM_FREQ_BASED_RX_CAL_DATA                     = 439,
  RFNV_DATA_TYPE_GSM_TX_CAL_FREQ_DATA                           = 440,

  /* Added datatypes for Atuner Tree NV ->
  RFNV_TREE_ATUNER_TUNECODES */
  RFNV_DATA_ATUNER_ACTIVE_TC_TBL                                = 441,
  RFNV_DATA_ATUNER_DETUNE_TC_TBL                                = 442,
  RFNV_DATA_ATUNER_ACTIVE_DETUNE_TC_TBL                         = 443,
  RFNV_DATA_TYPE_LTE_TX_MAX_POWER_BASED_ON_FREQ                 = 444,
  RFNV_DATA_TYPE_LTE_AMPR_V2                                    = 445,   /* rfnv_ampr_static_data_type*/
  RFNV_DATA_TYPE_LTE_CA_MPR_V2                                  = 446,
  RFNV_DATA_TYPE_TX_MAX_POWER_LIMIT_ADJUST_V2                   = 447,
  RFNV_DATA_TYPE_LTE_CA_NS_AMPR_V2                              = 448,
  RFNV_XPT_AUTOPIN_DATA_TYPE                                    = 449,
  RFNV_DATA_TYPE_LTE_GNSS_BLANKING                              = 450,

  RFNV_DATA_TYPE_ATUNER_GENERAL_SETTINGS                        = 451,
  RFNV_DATA_TYPE_ATUNER_ANTENNA_SPECIFIC_SETTINGS               = 452,
  RFNV_DATA_TYPE_ATUNER_ACM_SETTINGS                            = 453,
  RFNV_DATA_TYPE_ATUNER_ACL_MAAT_SETTINGS                       = 454,
  RFNV_DATA_TYPE_ATUNER_ACM_ADVANCED_SETTINGS                   = 455,
  RFNV_DATA_TYPE_ATUNER_ACL_MAAT_ADVANCED_SETTINGS              = 456,

  RFNV_WCDMA_VARIANT_MIMO_RADIO_INFO                            = 457, /* WCDMA MIMO Radio Band Info */
  RFNV_DATA_TYPE_LTE_MPR_V2                                     = 458,   /* rfnv_mpr_static_data_t_v2 */
  RFNV_DATA_TYPE_DC_CAL_OFFSET                                  = 459, /* rfnv_rx_cal_data_dc_offset_type */
  RFNV_DATA_TYPE_ATUNER_SUB_ANTENNA_SWITCH_SETTINGS_TYPE_V1     = 460,
  RFNV_DATA_TYPE_FBRX_DROOP_FIR_FILTER_COEFF_V3                 = 461,
  RFNV_DATA_TYPE_LTE_RX_SWITCH_POINT_INIT_SEARCH                = 462,
  RFNV_DATA_TYPE_UE_POWER_CLASS_BAND                            = 463,
  RFNV_DATA_TYPE_LTE_SNR_SAWLESS_PARA                           = 464,
  RFNV_DATA_ATUNER_SLEEP_TC_TBL                                 = 465,
  RFNV_DATA_ATUNER_LPM_TC_TBL                                   = 466,
  RFNV_DATA_TYPE_GSM_TEMP_VS_MAX_PWR_TYPE                       = 467,
  RFNV_DATA_TYPE_ID_XPT_VOLTERRA_TX_REF_RMS_V2                  = 468, /* tx_volterra_dpd_rms_nv_type_v2 */
  RFNV_DATA_TYPE_XPT_MLINE_RES_TYPE_V2                          = 469, /* rfnv_data_type_mline_results_v2 */
  RFNV_DATA_TYPE_LTE_CA_TX_SAR_POWER_LIMIT_CONTROL              = 470, /* rfnv_ca_tx_sar_pwr_limit_static_data_t  */
  RFNV_DATA_ATUNER_CL_DETUNE_TC_TBL                             = 471,
  RFNV_DATA_TYPE_TX_MAX_POWER_LIMIT_ADJUST_V3                   = 472,
  RFNV_DATA_TYPE_LTE_MPR_V3                                     = 473,
  RFNV_DATA_TYPE_STAND_ALONE_RX_CAL_OFFSET_V5                   = 474, /* rfnv_rx_stand_alone_lna_data_v5 */
  RFNV_DATA_CA_RX_RSB_CAL_BB_VEC_TYPE                           = 475, /* rfnv_data_ca_rx_rsb_cal_bb_vec_type */
  RFNV_DATA_ASDIV_CHAR_OFFSET_V1                                = 476, /* rfnv_data_asdiv_char_offset_v1_type */
  RFNV_DATA_TYPE_LTE_MTPL_VS_DLCA_VICTIM                        = 477, /* rfnv_lte_mtpl_vs_dlca_victim_t */
  RFNV_DATA_TYPE_TXAGC_CONTROL_V2                               = 478, /* rfnv_txagc_control_static_data_t_v2 */
  RFNV_DATA_SWITCH_POINT_BACKOFF_MAX_MPR                        = 479, /* rfnv_pa_swp_max_mpr_control_t */
  RFNV_DATA_TYPE_ENDC_AMPR                                      = 480, /* rfnv_endc_ampr_static_data_t */
  RFNV_GSM_EXTENDED_SAR_BACKOFF_DATA_TYPE_V2                    = 481,
  RFNV_DATA_TYPE_TX_SAR_POWER_LIMIT_CONTROL_V3                  = 482, /* rfnv_tx_sar_power_limit_control_v3_static_data_t */
  RFNV_DATA_RX_RSB_CAL_BB_VEC_TYPE_V2                           = 483, /* rfnv_data_rx_rsb_cal_bb_vec_type_v2 */
  RFNV_DATA_TX_RSB_CAL_BB_VEC_TYPE_V2                           = 484, /* rfnv_data_tx_rsb_cal_bb_vec_type_v2 */
  RFNV_DATA_FBRX_RSB_CAL_BB_VEC_TYPE_V2                         = 485, /* rfnv_data_fbrx_rsb_cal_bb_vec_type_v2 */
  RFNV_DATA_JRF_CAL_RESULT_TYPE_V2                              = 486, /* rfnv_data_type_jrf_cal_v2 */
  RFNV_DATA_TYPE_IIP2_CAL_VS_TEMP_VS_FREQ_V2                    = 487, /* rfnv_data_type_iip2_cal_vs_temp_vs_freq_v2 */
  RFNV_DATA_TYPE_TX_CA_MAX_POWER_ADJ                            = 488, /* rfnv_tx_ca_max_power_adjust_data_t */
  RFNV_DATA_TYPE_ENDC_AMPR_V2                                   = 489, /* rfnv_endc_ampr_static_data_t_v2 */
  RFNV_DATA_TYPE_ENDC_PA_SWITCH_POINT                           = 490, /* rfnv_endc_pa_switch_static_data_t */
  RFNV_DATA_TYPE_LTE_CA_TX_MAX_POWER_LIMIT_V2                   = 491, /* rfnv_ca_max_tx_pwr_limit_static_data_v2_t  */
  RFNV_DATA_RGI_LIST_TYPE_V2                                    = 492, /* rfnv_data_rgi_list_type_v2 */
  RFNV_DATA_PMEAS_LIST_TYPE_V2                                  = 493, /* rfnv_data_pmeas_list_type_v2 */
  RFNV_DATA_TYPE_TX_MAX_POWER_LIMIT_ADJUST_V4                   = 494, /* rfnv_max_tx_pwr_limit_adj_static_data_t_v4 */
  RFNV_DATA_QET_ET_CAL_TYPE                                     = 495, /* rfnv_data_qet_et_cal_type */
  RFNV_DATA_TYPE_ENDC_TX_SAR_POWER_LIMIT_CONTROL                = 496, /* rfnv_tx_sar_power_limit_control_v3_static_data_t */
  RFNV_DATA_ATUNER_MIMO_TC_TBL                                  = 497,
  RFNV_DATA_TYPE_ID_XPT_MODE_CONTROL_V3                         = 498, /* rfnv_xpt_static_mode_ctrl_v3_type */
  RFNV_DATA_TXFE_GAIN_OFFSET_STATIC_DATA                        = 499, /* rfnv_txfe_gain_offset_static_data_t */
  RFNV_DATA_TYPE_LTE_MPR_CONT_RB                                = 500, /* rfnv_mpr_cont_rb_static_data_t */
  RFNV_DATA_TYPE_LTE_AMPR_CONT_RB                               = 501, /* rfnv_ampr_cont_rb_static_data_t */
  RFNV_DATA_TYPE_LTE_MPR_NC_RB                                  = 502, /* rfnv_mpr_nc_rb_static_data_t */
  RFNV_DATA_TYPE_LTE_AMPR_NC_RB                                 = 503, /* rfnv_ampr_nc_rb_static_data_t */
  RFNV_DATA_TYPE_LTE_MPR_SUPPLEMENTARY                          = 504, /* rfnv_mpr_supplementary_static_data_t */
  RFNV_DATA_TYPE_LTE_AMPR_SUPPLEMENTARY                         = 505, /* rfnv_ampr_supplementary_static_data_t */
  RFNV_DATA_TYPE_ID_EPT_DPD_V3                                  = 506, /* tx_ept_dpd_v3_nv_type */
  RFNV_DATA_TYPE_ID_ET_DPD_V3                                   = 507, /* tx_ep_dpd_v3_nv_type */
  RFNV_DATA_CLIPPER_SETTINGS_TYPE_STATIC_DATA                   = 508, /* rfnv_clipper_settings_static_data_t */
  RFNV_DATA_TYPE_LTE_CA_NS_AMPR_V3                              = 509, /* rfnv_ca_ns_ampr_static_data_type_v3*/
  RFNV_DATA_TYPE_STATIC_MULTI_SPUR_DATA_LIST                    = 510, /* rfnv_rx_multi_spur_data_type */
  RFNV_DATA_TYPE_ESPR                                           = 511, /* rfnv_espr_static_data_t */
  RFNV_DATA_TYPE_XPT_ONLINE_DPD_MODE_V3                         = 512, /* rfnv_xpt_online_dpd_mode_v3_type */
  RFNV_DATA_TYPE_TX_SAR_POWER_LIMIT_CONTROL_V4                  = 513, /* rfnv_tx_sar_power_limit_control_v4_static_data_t */
  RFNV_DATA_TYPE_XPT_ONLINE_DUPLEXER_EQ                         = 514, /* rfnv_xpt_online_duplexer_eq */
  RFNV_DATA_TYPE_ENDC_MTPL_ADJ                                  = 515, /* rfnv_tx_endc_mtpl_adj_static_data_t */
  RFNV_DATA_TYPE_SAWLESS_STATE                                  = 516, /* rfnv_sawless_state */
  RFNV_DATA_TYPE_ENDC_PA_SWITCH_POINT_AMPR                      = 517, /* rfnv_endc_pa_switch_ampr_static_data_t */
  RFNV_DATA_TYPE_LTE_CA_TX_SAR_POWER_LIMIT_CONTROL_V2           = 518, /* rfnv_ca_tx_sar_pwr_limit_static_data_t_v2  */
  RFNV_DATA_TYPE_LTE_AMPR_V3                                    = 519, /* rfnv_ampr_static_data_type V3*/
  RFNV_DATA_TYPE_TX_SAR_POWER_LIMIT_CONTROL_V5                  = 520, /* rfnv_tx_sar_power_limit_control_v5_static_data_t */
  RFNV_DATA_TYPE_KEY_VALUE                                      = 521, /* rfnv_key_value_t */
  RFNV_DATA_ENDC_COEX_CHAR_IMD_PARAM_TYPE                       = 522,
  RFNV_DATA_TYPE_Tx_AsDIV_SWITCH_OFFSET                         = 523,
  RFNV_DATA_TYPE_LTE_CA_TX_SAR_POWER_LIMIT_CONTROL_V3           = 524, /* rfnv_ca_tx_sar_pwr_limit_static_data_t_v3  */
  RFNV_DATA_TYPE_TX_SAR_POWER_LIMIT_CONTROL_V6                  = 525, /* rfnv_tx_sar_power_limit_control_v6_static_data_t */
  RFNV_GSM_EXTENDED_SAR_BACKOFF_DATA_TYPE_V3                    = 526,
  RFNV_DATA_TYPE_TX_SAR_POWER_LIMIT_CONTROL_V7                  = 527, /* rfnv_tx_sar_power_limit_control_v7_static_data_t */
  RFNV_DATA_TYPE_SMART_ET_CTRL                                  = 528, /* rfnv_smart_et_ctrl_t */
  RFNV_DATA_TYPE_LTE_AMPR_SUPPLEMENTARY_V2                      = 529, /* rfnv_ampr_supplementary_static_data_t_v2 */
  RFNV_DATA_TYPE_ATUNER_UIACL_ML_PARAMS_FREQUENCY_AT_TYPE       = 530, /* rfnv_data_type_atuner_uiacl_ml_params_frequency_at_type */
  RFNV_DATA_TYPE_ATUNER_UIACL_ML_PARAMS_ANTENNA_GROUP_DELIMITER = 531, /* rfnv_data_type_atuner_uiacl_ml_params_antenna_group_delimiter */
  RFNV_DATA_TYPE_ACL_PARAMS_BIN_TYPE_V2                         = 532,	/* ACL Parameters Bin type V2 */
  RFNV_DATA_TYPE_ATUNER_ACL_MAAT_SETTINGS_V2                    = 533,
  RFNV_DATA_ATUNER_CL_TC_TBL_DATA_V2                            = 534,
  RFNV_DATA_TYPE_ID_SAR_BACKOFF_V2                              = 535, /*SAR 20 DSI states*/
  RFNV_DATA_TYPE_TX_SAR_ASDIV_POWER_LIMIT_CONTROL_V3            = 536, /* rfnv_tx_sar_asdiv_power_limit_control_v3_static_data_t */
  RFNV_DATA_TYPE_TXAGC_CONSTANT_PAPR_CONTROL                    = 537, /* rfnv_txagc_constant_papr_ctrl_t NV to disable constant PAPR*/
  RFNV_DATA_TYPE_NLIC_HARMONIC_PARAMS_ENHANCED_V4               = 538, /* DE NLIC types*/
  RFNV_DATA_TYPE_NLIC_IMD_PARAMS_ENHANCED_V4                    = 539, /* DE NLIC types*/
  RFNV_DATA_TYPE_STAND_ALONE_RX_CAL_OFFSET_V6                   = 540, /* rfnv_rx_stand_alone_lna_data_v6 */
  RFNV_DATA_TYPE_PA_SWITCH_POINT_DYNAMIC_POWER_TRACKING         = 541, /* rfnv_dynamic_power_tracking_static_data_t */
  RFNV_DATA_TYPE_SUB6_RX_SAWLESS_PARAMS                         = 542, /* rfnv_rx_sawless_static_data_t */
  RFNV_DATA_TYPE_ANTENNA_DETUNE_LIST                            = 543, /* rfnv_antenna_detune_list */
  RFNV_DATA_TYPE_ID_XPT_ANALOG_DELAY_V4                         = 544, /* rfnv_xpt_static_analog_delay_v4_data_type */
  RFNV_DATA_TYPE_ENDC_LTE_MTPL_LIMIT_ADJUST                     = 545, /* rfnv_lte_mtpl_endc_limit_adjust_static_data_t */
  RFNV_DATA_TYPE_ENDC_MTPL_ADJ_V2                               = 546, /* rfnv_tx_endc_mtpl_adj_static_data_v2_t */
  RFNV_DATA_TYPE_TX_SAR_POWER_LIMIT_CONTROL_V8                  = 547, /* rfnv_tx_sar_power_limit_control_v8_static_data_t */
  RFNV_DATA_TYPE_FBRX_GAIN_ANT_OFFSET                           = 548, /* rfnv_fbrx_gain_ant_offset_static_data_t */
  RFNV_DATA_TYPE_TX_DCOC_CAL                                    = 549, /* rfnv_tx_dcoc_cal */
  RFNV_DATA_TYPE_GSM_GNSS_BLANKING                              = 550, /* rfnv_gsm_gnss_blanking_sets_t */
  RFNV_DATA_TYPE_NLIC_HARMONIC_PARAM_ENHANCED_V5                = 551, /* ENDC NLIC Char datatype*/
  RFNV_GSM_TX_VCC_CAP                                           = 552, /* rfnv_gsm_tx_vcc_cap */
  RFNV_XPT_ENHANCED_AUTOPIN_DATA_TYPE                           = 553,
  RFNV_DATA_TYPE_TXAGC_CONTROL_V3                               = 554, /* rfnv_txagc_control_static_data_t_v3 */
  RFNV_DATA_TYPE_TX_SAR_POWER_LIMIT_CONTROL_V9                  = 555, /* rfnv_tx_sar_power_limit_control_v9_static_data_t */
  RFNV_DATA_TYPE_DSDA_COEX_DIGITAL_BACKOFF                      = 556, /*rfnv_dsda_digital_backoff_semi_static_data_t*/

  /* V10_x are new types with sweep and bandwidth members */
  RFNV_TX_LIN_V10_0                                             = 557, /* tx_linearizer_table_v10_0_nv_type */
  RFNV_XPT_AUTOPIN_V10_0                                        = 558, /* tx_xpt_autopin_v10_0_nv_type */
  RFNV_EPT_DPD_V10_0                                            = 559, /* tx_ept_dpd_v10_0_nv_type */
  RFNV_ET_DPD_V10_0                                             = 560, /* tx_et_dpd_v10_0_nv_type */
  RFNV_XPT_MLINE_RES_V10_0                                      = 561, /* rfnv_data_type_mline_results_v10_0 */

  RFNV_DATA_TYPE_ID_EPT_DPD_V4                                  = 562, /* tx_ept_dpd_v4_nv_type */
  RFNV_DATA_TYPE_ID_ET_DPD_V4                                   = 563, /* tx_ep_dpd_v4_nv_type */
  RFNV_DATA_TYPE_WCN_COEX_DESENSE_POWER_LIMIT_CONTROL           = 564, /* rfnv_wcn_coex_desense_power_limit_control_list_t */
  RFNV_DATA_TYPE_XPT_ONLINE_DPD_MODE_V4                         = 565, /* rfnv_xpt_online_dpd_mode_v4_type */
  RFNV_DATA_TYPE_XPT_ONLINE_DUPLEXER_EQ_V2                      = 566, /* rfnv_xpt_online_duplexer_eq_v2 */
  RFNV_DATA_TYPE_ID_XPT_MODE_CONTROL_V4                         = 567, /* rfnv_xpt_static_mode_ctrl_v4_type */
  RFNV_DATA_TYPE_STAND_ALONE_RX_CAL_OFFSET_HL_MODE              = 568, /* rfnv_rx_stand_alone_lna_data_hl_mode */
  RFNV_DATA_TYPE_COMBO_MTPL_ADJ                                 = 569, /* rfnv_tx_combo_mtpl_adj_static_data_t */
  RFNV_DATA_TYPE_STAND_ALONE_RX_CAL_OFFSET_V7                   = 570, /* rfnv_rx_stand_alone_lna_data_v6 (v6 and v7 have same NV structure) */
  RFNV_DATA_TYPE_TX_SAR_POWER_LIMIT_CONTROL_V10                 = 571, /* rfnv_tx_sar_power_limit_control_v10_static_data_t */
  RFNV_DATA_TYPE_TX_SAR_POWER_LIMIT_CONTROL_V11                 = 572, /*rfnv_tx_sar_power_limit_control_v11_static_data_t*/
  RFNV_DATA_TYPE_LTE_GNSS_BLANKING_V2                           = 573, /* rfnv_lte_gnss_blanking_sets_t_V2*/
  RFNV_DATA_RX_RSB_CAL_BB_VEC_TYPE_V3                           = 574, /* rfnv_data_rx_rsb_cal_bb_vec_type_v3 */
  RFNV_DATA_CA_RX_RSB_CAL_BB_VEC_TYPE_V3                        = 575, /* rfnv_data_rx_rsb_cal_bb_vec_type_v3 */
  RFNV_DATA_TYPE_ATUNER_ANTENNA_SPECIFIC_SETTINGS_V2            = 576,
  RFNV_DATA_TYPE_STATIC_MULTI_SPUR_DATA_LIST_V2                 = 577, /* rfnv_rx_multi_spur_v2_data_type */
  RFNV_DATA_TYPE_COMBO_MTPL_ADJ_V2                              = 578, /* rfnv_tx_combo_mtpl_adj_static_data_t_v2 */
  RFNV_XPT_ENHANCED_AUTOPIN_V10_0                               = 579, /* tx_xpt_enhanced_autopin_v10_0_nv_type */
  RFNV_DATA_TYPE_FDD_DYNAMIC_PA_STATE_MAPPING                   = 580, /* rfnv_fdd_dynamic_pa_state_mapping_t */
  RFNV_DATA_TYPE_FBRX_FREQ_COMP_GAIN_V2                         = 581, /* rfcommon_fbrx_freq_comp_gain_nv_v2_type */
  RFNV_DATA_TYPE_FBRX_GAIN_V5                                   = 582, /* rfcommon_fbrx_ref_freq_gain_nv_v2_type */
  RFNV_DATA_TYPE_FBRX_VCM_CAL                                   = 583, /* rfnv_data_fbrx_vcm_cal_item_type */
  RFNV_DATA_TYPE_TX_SAPT_S_FACTOR                               = 584, /* rfnv_tx_sapt_info_t */
  RFNV_DATA_TYPE_XPT_CONFIG                                     = 585, /* rfnv_xpt_config_type */
  RFNV_DATA_TYPE_DIVERSITY_PATH_ENABLE                          = 586, /* rfnv_data_diversity_path_enable */
  RFNV_DATA_TYPE_ATUNER_UIACL_ML_PARAMS_FREQUENCY_AT_TYPE_V2    = 587, /* rfnv_data_type_atuner_uiacl_ml_params_frequency_at_type_v2 */
  RFNV_DATA_TYPE_TX_RGI_LIMIT                                   = 588, /* rfnv_data_type_tx_rgi_limit_t */
  RFNV_DATA_TYPE_COEX_EXTRACTOR_PATHLOSS                        = 589, /* rfnv_tx_coex_extractor_pathloss_static_data_t */
  RFNV_DATA_QET_ET_CAL_TYPE_V2                                  = 590, /* rfnv_data_qet_et_cal_v2_type */
  RFNV_DATA_TYPE_ID_ET_DPD_V5                                   = 591, /* tx_ep_dpd_v5_nv_type */
  RFNV_DATA_TYPE_ASDIV_IMBALANCE_ADJ                            = 592, /* rfnv_asdiv_imbalance_data_t */
  RFNV_DATA_TYPE_TXAGC_CONTROL_V4                               = 593, /* rfnv_txagc_control_static_data_t_v4 */
  RFNV_DATA_TYPE_TX_DAPT_ENABLE                                 = 594, /* rfnv_tx_dapt_enable_t */
  /* KA NV Items*/
  RFNV_DATA_TYPE_TX_CAL_LINEARIZER                              = 595,/* rfnv_tx_lin_data_t */
  RFNV_DATA_TYPE_CAL_THERM_ADC                                  = 596,/* rfnv_cal_therm_adc_data_t */
  RFNV_DATA_TYPE_CLIPPER_TYPE                                   = 597,/* rfnv_tx_clipper_type_data_t */
  RFNV_DATA_TYPE_TXAGC_MIN_MAX_POWER_LIMIT_CONTROL              = 598,/* rfnv_tx_min_max_pwr_lmt_ctrl_data_t */
  RFNV_DATA_TYPE_TXAGC_CONTROL_PA_SWP                           = 599,/* rfnv_tx_agc_ctrl_pa_swp_data_t */
  RFNV_DATA_TYPE_POUT_FREQ_TEMP_COMP                            = 600,/* rfnv_pout_freq_temp_comp_data_t */
  RFNV_DATA_TYPE_VBATT_POUT_COMP                                = 601,/* rfnv_vbatt_pout_comp_data_t */
  RFNV_DATA_TYPE_RX_GAIN_CAL_OFFSET                             = 602,/* rfnv_rx_gain_cal_offset_data_t */
  RFNV_DATA_TYPE_RX_LNA_SWITCH_POINT                            = 603,/* rfnv_rx_lna_swp_data_t */
  RFNV_DATA_TYPE_RX_SPUR                                        = 604,/* rfnv_rx_spur_data_t */
  RFNV_DATA_TYPE_GSM_NBJDET_THRESOLD                            = 605,/* rfnv_gsm_nbjdet_threshold_data_t */
  RFNV_DATA_TYPE_GSM_MULTISLOT_MAX_TX_PWR                       = 606,/* rfnv_gsm_multislot_max_tx_pwr_data_t */
  RFNV_DATA_TYPE_GSM_TX_TIMING_OFFSET                           = 607,/* rfnv_gsm_tx_timing_data_t */
  RFNV_DATA_TYPE_GSM_TX_GTR_THRESHOLDS                          = 608,/* rfnv_gsm_tx_gtr_thresold_data_t */
  RFNV_DATA_TYPE_GSM_POLAR_RAMP_PROFILE                         = 609,/* rfnv_gsm_polar_ramp_profile_data_t */
  RFNV_DATA_TYPE_GSM_POWER_LEVELS                               = 610,/* rfnv_gsm_power_levels_data_t */
  RFNV_DATA_TYPE_GSM_ENH_APT_CONTROL                            = 611,/* rfnv_gsm_enh_apt_ctrl_data_t */
  RFNV_DATA_TYPE_GSM_TEMP_VS_MAX_PWR_CTRL                       = 612,/* rfnv_gsm_temp_vs_maxpwr_data_t */
  RFNV_DATA_TYPE_GSM_LINEAR_TX_GAIN_PARAM                       = 613,/* rfnv_gsm_linear_tx_gain_param_data_t */
  RFNV_DATA_TYPE_GSM_VBATT_POUT_TEMP_COMP                       = 614,/* rfnv_gsm_vbatt_pout_temp_comp_data_t */
  RFNV_DATA_TYPE_GSM_PA_ICQ_ENABLE                              = 615,/* rfnv_gsm_pa_icq_data_t */
  RFNV_DATA_TYPE_GSM_POUT_TEMP_COMP                             = 616,/* rfnv_gsm_pout_temp_comp_data_t */
  RFNV_DATA_TYPE_LTE_GNSS_BLANKING_TYPE                         = 617,/* rfnv_lte_gnss_blanking_data_t */
  RFNV_DATA_TYPE_THERM_ADC                                      = 618,/* rfnv_therm_adc_bin_data_t */
  RFNV_DATA_TYPE_NLIC_HARMONIC_PARAM_ENHANCED_V6                = 619,/* ENDC/NRCA NLIC Char datatype*/
  RFNV_DATA_TYPE_ATUNER_EXTENDED_ACTIVE_DETUNE_SETTINGS         = 620,
  RFNV_DATA_TYPE_LEGACY_MAX                                     = 3999
  /* This enum can go up to 4000 at which point it starts to
     overlap with type ids from other rfnv_item_types files */

};

enum
{
  /* WCDMA RX NV_TYPE use 1-5000*/
  RFNV_RX_CAL_DATA_NV_TYPE_WCDMA_SC = 1,
  RFNV_RX_CAL_DATA_NV_TYPE_WCDMA_DC = 2,
  RFNV_RX_CAL_DATA_NV_TYPE_WCDMA_3C = 3,

  /* LTE RX NV_TYPE use 5001-10000*/
  RFNV_RX_CAL_DATA_NV_TYPE_LTE_BW_1p4 = 5001,
  RFNV_RX_CAL_DATA_NV_TYPE_LTE_BW_3p5 = 5002,
  RFNV_RX_CAL_DATA_NV_TYPE_LTE_BW_5p0 = 5003,
  RFNV_RX_CAL_DATA_NV_TYPE_LTE_BW_10p0 = 5004,
  RFNV_RX_CAL_DATA_NV_TYPE_LTE_BW_15p0 = 5005,
  RFNV_RX_CAL_DATA_NV_TYPE_LTE_BW_20p0 = 5006,
  RFNV_RX_CAL_DATA_NV_TYPE_LTE_BW_40p0 = 5007,
  RFNV_RX_CAL_DATA_NV_TYPE_LTE_INTRA_BW_1p4 = 5008,
  RFNV_RX_CAL_DATA_NV_TYPE_LTE_INTRA_BW_3p5 = 5009,
  RFNV_RX_CAL_DATA_NV_TYPE_LTE_INTRA_BW_5p0 = 5010,
  RFNV_RX_CAL_DATA_NV_TYPE_LTE_INTRA_BW_10p0 = 5011,
  RFNV_RX_CAL_DATA_NV_TYPE_LTE_INTRA_BW_15p0 = 5012,
  RFNV_RX_CAL_DATA_NV_TYPE_LTE_INTRA_BW_20p0 = 5013,
  RFNV_RX_CAL_DATA_NV_TYPE_LTE_INTRA_BW_40p0 = 5014,
  RFNV_RX_CAL_DATA_NV_TYPE_LTE_NB_IoT = 5015,

  /* C2K RX NV_TYPE use 10001-15000*/
  RFNV_RX_CAL_NV_TYPE_CDMA_PM0         = 10001 ,
  RFNV_RX_CAL_NV_TYPE_CDMA_PM1         = 10002 ,
  RFNV_RX_CAL_NV_TYPE_CDMA_PM2         = 10003 ,

  /* TDS RX NV_TYPE use 15001-20000*/
  RFNV_RX_CAL_DATA_NV_TYPE_TDSCDMA_LL = 15001,
  RFNV_RX_CAL_DATA_NV_TYPE_TDSCDMA_HL = 15002,
};

// Note, only add new items to the end of the list
// also always fix the enum number as it cannot change once released
typedef enum {
  RFNV_TX_LIN_SWEEP_RGI_DELTA          = 0, // Also called FULL BIAS
  RFNV_TX_LIN_SWEEP_APT_XPT_LIN        = 1, //
} rfnv_tx_lin_sweep_type;

/*
  Expected values for BW mask:

  RFNV_INVALID_BW               = 0,
  RFNV_BW_1P4                   = 1,
  RFNV_BW_3                     = 2,
  RFNV_BW_5                     = 4,
  RFNV_BW_10                    = 8,
  RFNV_BW_15                    = 16,
  RFNV_BW_20                    = 32,
  RFNV_BW_40                    = 64,
  RFNV_BW_INTRA_ULCA_CONTIGUOUS = 256,

If its applicable to all BW type set all bits to 1.

*/

/*
  Expected values for Modulation mask:

  RFNV_INVALID_MOD       = 0,
  RFNV_MOD_BPSK          = 1,
  RFNV_MOD_QPSK          = 2,
  RFNV_MOD_16QAM         = 4,
  RFNV_MOD_64QAM         = 16,
  RFNV_MOD_256QAM        = 32,

If MPR is applicable to all modulation type set all bits to 1.
*/

/*!
  @brief
  Defines the variant marker type used in variable-size NVs
*/
typedef PACK(struct)
{
  /*! NV item version */
  uint16  version;

  /*! The number of elements contained in the NV item (not counting variant_marker element) */
  uint16  num_elements;
} rfnv_data_variant_marker_type;

/*!
  @brief
  Defines the variant type and data array.

  @details
  When reading/writing variant NVs this is what the buffer will have
*/
typedef PACK(struct)
{
  /*! Data type */
  uint16 data_type;

  /*! data of variant is here */
  uint8 data[];
} rfnv_data_variant_type;

/*!
  @brief
  Defines common channel table data type
*/
typedef PACK(struct)
{
  /*! Number of valid channels in the channel list */
  uint8 number_of_valid_channels;

  /*! Channel list*/
  uint32  channel_list[RFNV_CHAN_INFO_CHANNEL_LIST_SIZE];
} rfnv_data_chan_info_type;

/*!
  @brief
  Defines common freq table data type
*/
typedef PACK(struct)
{
  /*! Number of valid frequencies in the freq list */
  uint8 number_of_valid_frequencies;

  /*! Freq list*/
  uint32  freq_list[RFNV_CHAN_INFO_FREQ_LIST_SIZE];
} rfnv_data_freq_info_type;

/*!
  @brief
  Defines common channel table data type
*/
typedef PACK(struct)
{
  /*! Number of valid channels in the channel list */
  uint8 number_of_valid_channels;

  /*! Channel list*/
  uint16  channel_list[RFNV_GSM_CHAN_INFO_CHANNEL_LIST_SIZE];
} rfnv_data_gsm_chan_info_type;

/*!
  @brief
  Defines common frequency table data type
*/
typedef PACK(struct)
{
  /*! Number of valid channels in the channel list */
  uint8 number_of_valid_frequencies;

  /*! Frequency list in kHZ*/
  uint32  freq_list[RFNV_GSM_CHAN_INFO_CHANNEL_LIST_SIZE];
} rfnv_data_gsm_freq_info_type;

typedef PACK (struct)
{
  uint8 rffe_bus;
  uint8 rffe_usid;
  uint8 vpa_read_v1;
  uint8 vpa_read_v2;
}rfnv_data_qet_et_cal_item_type;

typedef PACK (struct)
{
  uint8 num_valid_qets;
  rfnv_data_qet_et_cal_item_type qet_item[RFNV_QET_ET_CAL_MAX_ENTRIES];
}rfnv_data_qet_et_cal_type;

typedef PACK (struct)
{
  uint8 rffe_bus;
  uint8 rffe_usid;
  uint16 rt_code_cal_pt1;
  uint16 rt_code_cal_pt2;
}rfnv_data_qet_et_cal_v2_item_type;

typedef PACK (struct)
{
  uint8 num_valid_qets;
  rfnv_data_qet_et_cal_v2_item_type qet_item[RFNV_QET_ET_CAL_MAX_ENTRIES];
}rfnv_data_qet_et_cal_v2_type;


typedef PACK (struct)
{
  uint8 mod;     /*0 = GMSK, 1 = 8PSK*/
  uint8 pa_state;
  uint8 valid_rgi_num;
  uint8 rgi_list[RFNV_LINEAR_MAX_RGI_INDEX_PER_GAIN_STATE];
}rfnv_data_rgi_list_type;

typedef PACK (struct)
{
  uint8 mod;     /*0 = GMSK, 1 = 8PSK*/
  uint8 pa_state;
  uint8 valid_rgi_num;
  uint8 rgi_list[RFNV_LINEAR_MAX_RGI_INDEX_PER_GAIN_STATE_V2];
}rfnv_data_rgi_list_type_v2;

typedef PACK (struct)
{
  uint8 channel_index;
  uint8 gain_state;
  uint8 modulation;
  int16 fbrx_rms;
  int16 box_rms;
}rfnv_data_fbrx_gain_cal_node_type;


typedef PACK (struct)
{
  uint8 mod;     /*0 = GMSK, 1 = 8PSK*/
  uint8 channel_index;
  uint8 pa_state;
  int16 pmeas_list[RFNV_LINEAR_MAX_RGI_INDEX_PER_GAIN_STATE];
}rfnv_data_pmeas_list_type;

typedef PACK (struct)
{
  uint8 mod;     /*0 = GMSK, 1 = 8PSK*/
  uint8 channel_index;
  uint8 pa_state;
  int16 pmeas_list[RFNV_LINEAR_MAX_RGI_INDEX_PER_GAIN_STATE_V2];
}rfnv_data_pmeas_list_type_v2;


typedef PACK (struct)
{
  int16 amam_max_dbm;
  uint8 rgi_for_pred;
}rfnv_data_amam_rgi_data_type;

typedef PACK (struct)
{
  uint8 amam_ampm_identifier;  /* 0 = AMAM, 1= AMPM */
  uint8 channel_index;
  uint16 data_list[GSM_AMAM_TBL_SIZE];
}rfnv_data_amam_list_type;

typedef PACK (struct)
{
  uint8 amam_ampm_identifier;  /* 0 = AMAM, 1= AMPM */
  uint8 channel_index;
  int16 data_list[GSM_AMPM_TBL_SIZE];
}rfnv_data_ampm_list_type;

typedef PACK (struct)
{
  uint16 therm_read;
} rfnv_data_gsm_cal_temp_type;

typedef PACK (struct)
{
  uint8 mod;     /*0 = GMSK, 1 = 8PSK*/
  uint8 channel_index;
  int16 pwr_err_list[RFNV_NUMBER_OF_PCLS];
}rfnv_data_pwr_err_list_type;

typedef PACK (struct)
{
  uint8 channel_index;
  uint8 gain_state_index;
  int16 fbrx_gain;
}rfnv_data_fbrx_gain_type;

typedef PACK (struct)
{
  uint8 channel_index;
  uint8 fbrx_mode_index;
  uint8 gain_state_index;
  int32 fbrx_gain;
  int16 tx_power;
  uint8 ubias;
  uint16 rx_scale;
  uint16 predet_index;
}rfnv_data_fbrx_gain_type_v2;

typedef PACK(struct)
{
  uint8 chain_id;
  uint8 carr_id;
  int16 lna_rise[WCDMA_NUM_LNA_STATES];
  int16 lna_fall[WCDMA_NUM_LNA_STATES];
  int16 lna_hs_rise[WCDMA_NUM_LNA_STATES];
  int16 lna_hs_fall[WCDMA_NUM_LNA_STATES];
}rfnv_data_lna_swpts_type;

typedef PACK(struct)
{
  uint8 chain_id;
  uint8 carr_id;
  int16 lna_offsets[WCDMA_NUM_LNA_STATES];
  int16 vga_offset;
}rfnv_rx_cal_offset_type;

typedef PACK(struct)
{
  uint8 chain_id;
  uint16 channel;
  uint8 chan_index;
  int8 vga_gain_offset_vs_freq;
  int8 lna_offset_vs_freq;
  int8 lna_offset_vs_freq_2;
  int8 lna_offset_vs_freq_3;
  int8 lna_offset_vs_freq_4;
}rfnv_rx_offset_vs_freq_type;


typedef PACK ( struct )
{
  uint16   path_idx;
  uint16   cal_type;
  uint16  channel_list[RFNV_NUMBER_OF_PCLS];
  int16   freq_comp_offset[RFNV_MAX_LNA_GAIN_STATES][RFNV_NUMBER_OF_PCLS];
} rfnv_rx_cal_data_typeid_118_type;

typedef PACK ( struct )
{
  uint16   path_idx;
  uint16   cal_type;
  uint32  channel_list[RFNV_NUMBER_OF_PCLS];
  int16   freq_comp_offset[RFNV_MAX_LNA_GAIN_STATES][RFNV_NUMBER_OF_PCLS];
} rfnv_rx_cal_data_typeid_118_v2_type;

typedef PACK( struct )
{
  uint8 carr_id;
  uint8 hs_config;
  int16 lna_rise[WCDMA_NUM_LNA_STATES];
  int16 lna_fall[WCDMA_NUM_LNA_STATES];
}rfnv_data_lna_swpts_type_v3;

typedef PACK(struct)
{
  uint8  nv_container;
  uint8  rfm_dev;
} rfnv_variant_element_header;

typedef PACK(struct)
{
  uint8  sig_path;
  uint8  rfm_dev;
  uint8  ant_swt;
} rfnv_variant_element_header_v2;

typedef PACK(struct)
{
   PACK(struct){
     uint8  sig_path;
     uint8  rfm_dev;
     uint8  ant_swt;
   } offset;
   PACK(struct) {
     uint8  sig_path;
     uint8  rfm_dev;
     uint8  ant_swt;
   } abs;
} rfnv_variant_element_ref_header;

typedef PACK(struct)
{
   uint16 sig_path;
   uint16 rfm_device;
   uint16 ant_switch_path;
   uint16 pll_id;
} rfnv_variant_element_header_v3;

typedef PACK(struct)
{
   PACK(struct){
      uint16 sig_path;
      uint16 rfm_device;
      uint16 ant_switch_path;
      uint16 pll_id;
   } offset;
   PACK(struct) {
      uint16 sig_path;
      uint16 rfm_device;
      uint16 ant_switch_path;
      uint16 pll_id;
   } abs;
} rfnv_variant_element_ref_header_v3;

typedef enum {
  FULL_CAL = 0,
  OFFSET_CAL_ONE_GAIN_STATE = 1,
  OFFSET_CAL_ALL_GAIN_STATES = 2
} rfnv_rx_cal_type;

typedef PACK ( struct )
{
  uint16 cal_type;
  uint8 number_of_valid_channels;
  uint32 channel_list[RFNV_NUMBER_OF_PCLS];
  int16  freq_comp_offset[RFNV_MAX_LNA_GAIN_STATES][RFNV_CHAN_INFO_CHANNEL_LIST_SIZE];
} rfnv_rx_stand_alone_lna_data_v3;

typedef PACK ( struct )
{
  uint16 cal_type;
  uint8 number_of_valid_frequencies;
  uint32 freq_list[RFNV_NUMBER_OF_PCLS];
  int16  freq_comp_offset[RFNV_MAX_LNA_GAIN_STATES][RFNV_CHAN_INFO_FREQ_LIST_SIZE];
} rfnv_rx_stand_alone_lna_data_v4;

typedef PACK ( struct )
{
  uint16 cal_type;
  uint8 number_of_valid_frequencies;
  uint32 freq_list[RFNV_CHAN_INFO_FREQ_LIST_SIZE];
  int16  freq_comp_offset[RFNV_MAX_CAL_LNA_GAIN_STATES][RFNV_CHAN_INFO_FREQ_LIST_SIZE];
} rfnv_rx_stand_alone_lna_data_v5;

typedef PACK ( struct )
{
  uint16 cal_type;
  uint32 bw;
  uint8 number_of_valid_frequencies;
  uint32 freq_list[RFNV_CHAN_INFO_FREQ_LIST_SIZE];
  int16  freq_comp_offset[RFNV_MAX_CAL_LNA_GAIN_STATES][RFNV_CHAN_INFO_FREQ_LIST_SIZE];
} rfnv_rx_stand_alone_lna_data_v6;

typedef enum {
  RX_STANDLONE_OFFSET_DATA_ONE_GAIN_STATE = 0,
  RX_STANDLONE_OFFSET_DATA_ALL_GAIN_STATES = 1
} rfnv_rx_offset_data_type;

typedef PACK ( struct )
{
  uint16 type_of_offset_data; //rfnv_rx_offset_data_type
  rfnv_data_chan_info_type channel_table;
  uint16 gain_state_mask;
  int16  freq_comp_offset[RFNV_MAX_LNA_GAIN_STATES][RFNV_CHAN_INFO_CHANNEL_LIST_SIZE];
} rfnv_rx_stand_alone_offset_lna_data_v3;

typedef PACK ( struct )
{
  uint16 type_of_offset_data; //rfnv_rx_offset_data_type
  rfnv_data_freq_info_type freq_table;
  uint16 gain_state_mask;
  int16  freq_comp_offset[RFNV_MAX_LNA_GAIN_STATES][RFNV_CHAN_INFO_CHANNEL_LIST_SIZE];
} rfnv_rx_stand_alone_offset_lna_data_v4;

typedef PACK ( struct )
{
  uint8 number_of_valid_channels;
  uint32 channel_list[RFNV_NUMBER_OF_PCLS];
  uint8 number_of_valid_splits;
  PACK (struct) {
    uint16 concurrent_dev_mask;
    int16  freq_comp_offset[RFNV_MAX_LNA_GAIN_STATES][RFNV_NUMBER_OF_PCLS];
  } split_freq_comp_offset[RFNV_MAX_NUM_SPLIT_LNA_CASES];
} rfnv_rx_split_lna_data_v3;

typedef PACK ( struct )
{
  uint8 number_of_valid_frequencies;
  uint32 freq_list[RFNV_NUMBER_OF_PCLS];
  uint8 number_of_valid_splits;
  PACK (struct) {
    uint16 concurrent_dev_mask;
    int16  freq_comp_offset[RFNV_MAX_LNA_GAIN_STATES][RFNV_NUMBER_OF_PCLS];
  } split_freq_comp_offset[RFNV_MAX_NUM_SPLIT_LNA_CASES];
} rfnv_rx_split_lna_data_v4;

typedef PACK(struct)
{
   uint16 bw;
   uint8 num_rsb_cal_taps;
   PACK(struct) {
      int32 r;
      int32 i;
   } rsb_bb_vec[RFNV_RX_RSB_MAX_NUM_OF_TAPS];
} rfnv_data_rsb_cal_bb_vec_type;

typedef PACK(struct)
{
   uint8 cal_state;   /* values are from cal_state_per_bw */
   uint32 frequency;
   uint8 bw;
   uint8 lna_gain_state;
   uint8 num_rsb_cal_tones;
   PACK(struct) {
      int32 r;
      int32 i;
   } rsb_bb_vec[RFNV_RX_RSB_MAX_NUM_OF_TONES];
} rfnv_data_rx_rsb_cal_bb_vec_type;

typedef PACK(struct)
{
   uint8 cal_state;   /* values are from cal_state_per_bw */
   uint32 frequency;
   uint32 bw;
   uint8 lna_gain_state;
   uint8 num_rsb_cal_tones;
   PACK(struct) {
      int32 r;
      int32 i;
   } rsb_bb_vec[RFNV_RX_RSB_MAX_NUM_OF_TONES];
} rfnv_data_rx_rsb_cal_bb_vec_type_v2;

typedef PACK(struct)
{
   uint8 cal_state; /* values are from cal_state_per_bw */
   uint32 bw;
   uint32 frequency;
   uint8 lna_gain_state;
   uint8 tone_set_id;
   uint8 num_rsb_cal_tones;
   PACK(struct) {
      int32 r;
      int32 i;
   } rsb_bb_vec[RFNV_RX_RSB_MAX_NUM_OF_TONES];

} rfnv_data_rx_rsb_cal_bb_vec_type_v3;

typedef PACK(struct)
{
   uint8 cal_state;   /* values are from cal_state_per_bw */
   uint32 frequency;
   uint8 bw;
   uint8 lna_gain_state;
   uint8 num_rsb_cal_tones;
   PACK(struct) {
      int32 r;
      int32 i;
   } rsb_bb_vec[RFNV_FBRX_RSB_MAX_NUM_OF_TONES];
} rfnv_data_fbrx_rsb_cal_bb_vec_type;

typedef PACK(struct)
{
   uint8 cal_state;   /* values are from cal_state_per_bw */
   uint32 frequency;
   uint32 bw;
   uint8 lna_gain_state;
   uint8 num_rsb_cal_tones;
   PACK(struct) {
      int32 r;
      int32 i;
   } rsb_bb_vec[RFNV_FBRX_RSB_MAX_NUM_OF_TONES];
} rfnv_data_fbrx_rsb_cal_bb_vec_type_v2;

typedef PACK(struct)
{
   uint8 cal_state;   /* values are from cal_state_per_bw */
   uint32 frequency;
   uint8 bw;
   uint8 therm_bin;
   uint8 rgi;
   uint16 therm_adc_val;
   uint8 num_rsb_cal_tones;
   PACK(struct) {
      int32 r;
      int32 i;
   } rsb_bb_vec[RFNV_TX_RSB_MAX_NUM_OF_TONES];
} rfnv_data_tx_rsb_cal_bb_vec_type;

typedef PACK(struct)
{
   uint8 cal_state;   /* values are from cal_state_per_bw */
   uint32 frequency;
   uint32 bw;
   uint8 therm_bin;
   uint8 rgi;
   uint16 therm_adc_val;
   uint8 num_rsb_cal_tones;
   PACK(struct) {
      int32 r;
      int32 i;
   } rsb_bb_vec[RFNV_TX_RSB_MAX_NUM_OF_TONES];
} rfnv_data_tx_rsb_cal_bb_vec_type_v2;

// Deprecated do not use this type
typedef PACK(struct)
{
   uint8 cal_state;   /* values are from cal_state_per_bw */
   uint32 frequency;
   uint8 lna_gain_state;
   uint8 num_of_freq_offset;
   PACK(struct) {
      uint16 freq_offset;
      uint8 num_rsb_cal_tones;
    PACK(struct) {
      int32 r;
      int32 i;
    } rsb_bb_vec[RFNV_RX_RSB_MAX_NUM_OF_TONES];
   }ozif_rsb_per_freq_offset[RFNV_OZIF_RX_RSB_MAX_NUM_OF_FREQ_OFFSETS];
} rfnv_data_ozif_rx_rsb_cal_bb_vec_type;

typedef PACK(struct)
{
  uint8 cal_state; /* values are from cal_state_per_bw */
  uint16 freq_offset;
  uint8 num_rsb_cal_tones;
  PACK(struct)
  {
    int32 r;
    int32 i;
  } rsb_bb_vec[RFNV_RX_RSB_MAX_NUM_OF_TONES];
} rfnv_data_ca_rx_rsb_cal_freq_offset_type;

typedef PACK(struct)
{
   uint32 frequency;
   uint8 lna_gain_state;
   uint8 num_of_freq_offset;
   rfnv_data_ca_rx_rsb_cal_freq_offset_type ca_rx_rsb_per_freq_offset[RFNV_CA_RX_RSB_MAX_NUM_OF_FREQ_OFFSETS];
} rfnv_data_ca_rx_rsb_cal_bb_vec_type;

typedef PACK(struct)
{
   uint32 frequency;
   uint8 bw;
   uint8 lna_gain_state;
   int8 xmfr_code;
   uint8 f1db_code;
} rfnv_data_type_jrf_cal;

typedef PACK(struct)
{
   uint32 frequency;
   uint32 bw;
   uint8 lna_gain_state;
   int8 xmfr_code;
   uint8 f1db_code;
} rfnv_data_type_jrf_cal_v2;

typedef enum {
  CAL_PASS          = 0,
  CAL_BYPASS        = 1,
  CAL_FAIL          = 2,
  INVALID_CAL_STATE = 0xFF,
} cal_state_per_bw;

typedef PACK(struct)
{
  uint8    nv_active;
  int16    sin_theta;
  int16    cos_theta;
  int16    gain_inv;
} rsb_data_list_per_gain;

typedef PACK(struct)
{
  uint16                     path_idx;
  uint8                      rsb_type;
  uint16                     dl_channel;
  int32                      freq_offset_khz;
  uint8                      bw;
  rsb_data_list_per_gain     rsb_data[RFNV_MAX_LNA_GAIN_STATES];
} rfnv_rx_cal_data_typeid_124_type;

typedef PACK(struct)
{
  int32    dc_offset_i;
  int32    dc_offset_q;
} dc_offset_data_per_gain;

typedef PACK(struct)
{
  uint16                     catm_nb_mode;
  dc_offset_data_per_gain    dc_offset[RFNV_MAX_LNA_GAIN_STATES];
} rfnv_rx_cal_data_dc_offset_type;

typedef PACK(struct)
{
  rfnv_data_variant_marker_type     variant_marker;
  rfnv_rx_cal_data_typeid_118_type  *cal_118_data;
  uint32                            num_of_cal_118_elements;
  uint32                            num_of_cal_124_elements;
  rfnv_rx_cal_data_typeid_124_type  *cal_124_data;
} rfnv_rx_cal_data_type;

typedef PACK(struct)
{
  rfnv_data_variant_marker_type       variant_marker;
  rfnv_rx_cal_data_typeid_118_v2_type  *cal_118_data;
  uint32                              num_of_cal_118_elements;
  uint32                              num_of_cal_124_elements;
  rfnv_rx_cal_data_typeid_124_type    *cal_124_data;
} rfnv_rx_cal_data_v2_type;

typedef PACK(struct)
{
  rfnv_data_variant_marker_type       variant_marker;
  rfnv_rx_cal_data_typeid_118_v2_type  *cal_118_data;
  uint32                              num_of_cal_118_elements;
  uint32                              num_of_cal_124_elements;
  rfnv_rx_cal_data_typeid_124_type    *cal_124_data;
  uint32                              num_of_dc_offset_elements;
  rfnv_rx_cal_data_dc_offset_type     *dc_offset_data;
} rfnv_rx_cal_data_v3_type;

typedef PACK(struct)
{
  uint8   rx_num_gain_states;
  int16   rx_gain_offsets[RFNV_MAX_LNA_GAIN_STATES];
  int16   rx_path_lna_swtps[RFNV_RX_PATH_LNA_SWTPS];
}rfnv_rx_static_data_type;

typedef PACK(struct)
{
  int16   lna_range_rise_fast_agc;
  int16   lna_range_fall_fast_agc;
}rfnv_rx_static_fast_agc_data_type;

typedef PACK(struct)
{
  uint8 enable;
  int16 mtpl_value;
}rfnv_rx_alt_path_mtpl_data_type;

typedef PACK(struct)
{
   /* 0: static (dBm) switch point
      1: dynamic (dBFS) switch point */
   uint8 type;
   /* if type 1, backoff must be a negative number in dB10 that is the
      distance from saturation that the LNA will rise to the next state */
   int16 rise_or_backoff;
   /* if type 1, hyst must be a negative number in dB10 that is the amount
      below the rise that the LNA will fall back into this state */
   int16 fall_or_hyst;
} switch_point_s;

/* QMSL_LTE_RX_SWITCH_POINT_CONTAINER_DATA_TYPE (type ID 203) */
typedef PACK(struct)
{
  uint8 nv_container;
  uint8 reserved;
  /* switch_point_type:
     0: single-carrier switch points
     1: multi-carrier shared-LNA switch points
     2: LTE-D mode switch points */
  uint8 switch_point_type;
  switch_point_s switch_points[RFNV_MAX_LNA_GAIN_STATES - 1];
} rfnv_lte_rx_switch_point_container_data_type;

/* RFNV_DATA_TYPE_LTE_RX_SWITCH_POINT_CONTAINER_V2 (type ID 438) */
typedef PACK(struct)
{
	/* switch_point_mode:
     0: default mission-mode
     1: LPM_LNA mode*/
  uint8 switch_point_mode;
  /* switch_point_type:
     0: single-carrier switch points
     1: multi-carrier shared-LNA switch points
     2: LTE-D mode switch points */
  uint8 switch_point_type;
  switch_point_s switch_points[RFNV_MAX_STATIC_LNA_GAIN_STATES - 1];
} rfnv_lte_rx_switch_point_container_v2_data_type;

/* RFNV_DATA_TYPE_QPOET_RSSI_THRESH (type ID 343) */
typedef PACK(struct)
{
  int32 rssi_threshold_dbm10;
} rfnv_lte_rx_qpoet_rssi_threshold_data_type;

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
}rfnv_rx_static_unified_data_type;

typedef PACK(struct)
{
  uint8    nv_container_idx; /* NV Container Index for each Tx path for a given LTE band */
  uint32   tx_earfcn; /* Channel based APT Char data for given Tx Path */
  uint8    pa_state_idx; /* For a given PA state */
  uint8    tx_bandwidth; /* For a given Tx Bandwidth */
  uint32   rgi_indices; /* RGI range for which CHAR data is valid. MSB(2 bytes)=Max RGI, LSB(2 bytes)=Min RGI */
  int16    pwr_db10 [RFNV_TX_STATIC_APT_CHAR_ENTRIES]; /* Characterization data: Power measurement */
  uint16   vcc_mv[RFNV_TX_STATIC_APT_CHAR_ENTRIES]; /* Characterization data: Applied VCC measurement */
  uint32   icq [RFNV_TX_STATIC_APT_CHAR_ENTRIES]; /* Characterization data: Applied ICQ measurement */
}rfnv_tx_static_data_type;

/* RFNV_ASDIV_RX_GAIN_OFFSET
   RFNV variant typeid 141 */
typedef PACK(struct)
{
  /* NV "path." */
  uint8 nv_container_index;
  /* Reserved for now, not used. */
  uint8 reserved;
  /* The ASDIV switch position, must match the definition from the RF card. */
  uint8 switch_position;
  /* Offsets to be applied to the Rx calibration data per gain state for
     this switch position.  Units are tech specific. */
  int16 gain_offset[8];
} rfnv_asdiv_rx_gain_offset_type;

/* RFNV_SWITCH_POINT_FALL_RISE_OFFSET
   Helper type to define a switch point rise/fall offset pair. */
typedef PACK(struct)
{
  /* Switch point fall offset, to be used in ASDIV Rx switch points NV. */
  int16 fall_offset;
  /* Switch point rise offset, to be used in ASDIV Rx switch points NV. */
  int16 rise_offset;
} rfnv_switch_point_fall_rise_offset_type;

/* RFNV_ASDIV_RX_SWITCH_POINTS
   RFNV variant typeid 142 */
typedef PACK(struct)
{
  /* NV "path." */
  uint8 nv_container_index;
  /* Reserved for now, not used. */
  uint8 reserved;
  /* The ASDIV switch position, must match the definition from the RF card. */
  uint8 switch_position;
  /* Offsets to be applied to the Rx switch points for this switch
     position.  Units are tech specific. */
  rfnv_switch_point_fall_rise_offset_type switch_point_offset[7];
} rfnv_asdiv_rx_switch_points_type;

/* RFNV_ASDIV_TX_GAIN_OFFSET
   RFNV variant typeid 143 */
typedef PACK(struct)
{
  /* NV "path." */
  uint8 nv_container_index;
  /* Reserved for now, not used. */
  uint8 reserved;
  /* The ASDIV switch position, must match the definition from the RF card. */
  uint8 switch_position;
  /* Offsets to be applied to the Tx calibration data (for all gain states)
     for this switch position.  Units are tech specific. */
  int16 switch_point_offset;
} rfnv_asdiv_tx_gain_offset_type;

/* RFNV_DATA_TYPE_LTE_TX_MAX_POWER_BASED_ON_EARFCN
   RFNV variant typeid 144 */
typedef PACK(struct)
{
  /* NV Container Index for each Tx path for a given LTE band */
  uint8 nv_container_index;
  /* Reserved for now, not used. */
  uint8 reserved;
  /* Channel boundaries for power limit regions */
  uint32 tx_upper_bound_earfcn[RFNV_TX_MAX_PWR_ENTRIES];
  /* Maximum power limit for channels up to the EARFCN in tx_upper_bound_earfcn */
  int16 pwr_dbm10[RFNV_TX_MAX_PWR_ENTRIES];
} rfnv_max_tx_pwr_vs_earfcn_static_data_type;
/* RFNV_DATA_TYPE_LTE_TX_MAX_POWER_BASED_ON_FREQ
   RFNV variant typeid 444 */
typedef PACK(struct)
{
  /* NV Container Index for each Tx path for a given LTE band */
  uint8 nv_container_index;
  /* Reserved for now, not used. */
  uint8 reserved;
  /* Channel boundaries for power limit regions */
  uint32 tx_upper_bound_freq[RFNV_TX_MAX_PWR_ENTRIES];
  /* Maximum power limit for channels up to the EARFCN in tx_upper_bound_earfcn */
  int16 pwr_dbm10[RFNV_TX_MAX_PWR_ENTRIES];
} rfnv_max_tx_pwr_vs_freq_static_data_type;

typedef PACK(struct)
{
  /* Channel based PA Bias offset for given Tx Path */
  uint32 tx_earfcn;
  /* PA Bias offset to be applied to PA Bias for given channel*/
  int16 pa_bias_offset;
} rfnv_data_pa_bias_offset_type;

/* RFNV_DATA_TYPE_PA_BIAS_OFFSET_CHANNELS
   RFNV variant typeid 145 */
typedef PACK(struct)
{
  /* NV Container Index for each Tx path for a given LTE band */
  uint8 nv_container_index;
  /* Reserved for now, not used. */
  uint8 reserved;
  /* structure to hold tx_earfcn and pa bias offset*/
  rfnv_data_pa_bias_offset_type offset[RFNV_LTE_TX_CHAN_SIZE];
} rfnv_data_pa_bias_combo_offset_type;

typedef PACK(struct)
{
  /* channel boundaries for ampr backoff region */
  int32 tx_earfcn_min;
  /* channel boundaries for ampr backoff region */
  int32 tx_earfcn_max;
  /* modulation type mask for ampr backoff region */
  uint32 modulation_type_mask;
  /* channel bandwidth mask for ampr backoff region */
  uint32 bandwidth_mask;
  /* MPR boundaries for ampr backoff region */
  int16 mpr_min;
  /* MPR boundaries for ampr backoff region */
  int16 mpr_max;
  /* RB start boundaries for ampr backoff region */
  int16 rb_start_min;
  /* RB start boundaries for ampr backoff region */
  int16 rb_start_max;
  /* RB block boundaries for ampr backoff region */
  int16 rb_block_min;
  /* RB block boundaries for ampr backoff region */
  int16 rb_block_max;
  /* RB end boundaries for ampr backoff region */
  int16 rb_end_min;
  /* RB end boundaries for ampr backoff region */
  int16 rb_end_max;
  /* ampr backoff in db10 */
  int16 ampr_db10;
} rfnv_lte_ampr_table_type;

typedef PACK(struct)
{
  /* channel boundaries for ampr backoff region */
  int32 tx_freq_min;
  /* channel boundaries for ampr backoff region */
  int32 tx_freq_max;
  /* modulation type mask for ampr backoff region */
  uint32 modulation_type_mask;
  /* channel bandwidth mask for ampr backoff region */
  uint32 bandwidth_mask;
  /* MPR boundaries for ampr backoff region */
  int16 mpr_min;
  /* MPR boundaries for ampr backoff region */
  int16 mpr_max;
  /* RB start boundaries for ampr backoff region */
  int16 rb_start_min;
  /* RB start boundaries for ampr backoff region */
  int16 rb_start_max;
  /* RB block boundaries for ampr backoff region */
  int16 rb_block_min;
  /* RB block boundaries for ampr backoff region */
  int16 rb_block_max;
  /* RB end boundaries for ampr backoff region */
  int16 rb_end_min;
  /* RB end boundaries for ampr backoff region */
  int16 rb_end_max;
  /* ampr backoff in db10 */
  int16 ampr_db10;
} rfnv_lte_ampr_table_type_v2;

typedef PACK(struct)
{
  /* channel boundaries for ampr backoff region */
  int32 tx_freq_min;
  /* channel boundaries for ampr backoff region */
  int32 tx_freq_max;
  /* modulation type mask for ampr backoff region */
  uint32 modulation_type_mask;
  /* channel bandwidth mask for ampr backoff region */
  uint32 bandwidth_mask;
  /* MPR boundaries for ampr backoff region */
  int16 mpr_min;
  /* MPR boundaries for ampr backoff region */
  int16 mpr_max;
  /* RB start boundaries for ampr backoff region */
  int16 rb_start_min;
  /* RB start boundaries for ampr backoff region */
  int16 rb_start_max;
  /* RB block boundaries for ampr backoff region */
  int16 rb_block_min;
  /* RB block boundaries for ampr backoff region */
  int16 rb_block_max;
  /* RB end boundaries for ampr backoff region */
  int16 rb_end_min;
  /* RB end boundaries for ampr backoff region */
  int16 rb_end_max;
  /* ampr backoff in db10 */
  int16 ampr_db10;
  /* NS step size for ampr calculation with gpost gain */
  int16 ampr_step;

} rfnv_lte_ampr_table_type_v3;


/* RFNV_DATA_TYPE_LTE_AMPR RFNV variant typeid 151 to be used with Tx STatic NV*/
typedef PACK(struct)
{
  /* NV Container Index for each Tx path for a given LTE band */
  uint8 nv_container_index;
  /* Power Class defined in 36101*/
  uint8 power_class;
  /* Network Signalling Value */
  uint32 ns_number;
  /* AMPR table structure */
  rfnv_lte_ampr_table_type ampr_table[RFNV_TX_STATIC_AMPR_TABLE_SIZE];
} rfnv_ampr_static_data_type;
/* RFNV_DATA_TYPE_LTE_AMPR RFNV variant typeid 445 to be used with Tx STatic NV*/
typedef PACK(struct)
{
  /* NV Container Index for each Tx path for a given LTE band */
  uint8 nv_container_index;
  /* Power Class defined in 36101*/
  uint8 power_class;
  /* Network Signalling Value */
  uint32 ns_number;
  /* AMPR table structure */
  rfnv_lte_ampr_table_type_v2 ampr_table[RFNV_TX_STATIC_AMPR_TABLE_SIZE];
} rfnv_ampr_static_data_type_v2;

typedef PACK(struct)
{
  /* NV Container Index for each Tx path for a given LTE band */
  uint8 nv_container_index;
  /* Power Class defined in 36101*/
  uint8 power_class;
  /* Network Signalling Value */
  uint32 ns_number;
  /* AMPR table structure */
  uint16 gpost_connect;  /*Used to specify external antenna gain */
  uint8 nv_valid_flag;   /*To specify NV based or spec based 1- NV based and 0- spec based */
  rfnv_lte_ampr_table_type_v3 ampr_table[RFNV_TX_STATIC_AMPR_TABLE_SIZE];
} rfnv_ampr_static_data_type_v3;



/* RFNV_DATA_TYPE_MULTI_CLUSTER_BW_CLASS_I RFNV variant typeid 155 to be used with Tx Static NV */
typedef PACK(struct)
{
  /* NV Container Index for each Tx path for a given LTE band */
  uint8 nv_container_index;
  /* Reserved for now, not used. */
  uint8 reserved;
  /* bit 0 - class A,bit 1 - class B, bit 2 - class C, bit 3 - class D, and so on */
  uint32 multi_cluster_bw_class;
} rfnv_multi_cluster_bw_class_data_type;

typedef PACK(struct)
{
  /* channel boundaries for ampr backoff region */
  int32 tx_earfcn_min;
  /* channel boundaries for ampr backoff region */
  int32 tx_earfcn_max;

  /* Aggregated modulation type mask for ampr backoff region */
  uint32 agg_modulation_type_mask;

  /*Aggregated channel bandwidth mask for ampr backoff region */
  /* bit 0 - class A (NA), bit 1 - class B, bit 2 - class C, and so on */
  uint32 bandwidth_class;

  /* Aggregate RB start boundaries for ampr backoff region */
  int16 agg_rb_start_min;
  /* Aggregate RB start boundaries for ampr backoff region */
  int16 agg_rb_start_max;
  /* Aggregate RB block boundaries for ampr backoff region */
  int16 agg_rb_block_min;
  /* Aggregate RB block boundaries for ampr backoff region */
  int16 agg_rb_block_max;

  /* Aggregate RB end boundaries for ampr backoff region */
  int16 agg_rb_end_min;
  /* Aggregate RB end boundaries for ampr backoff region */
  int16 agg_rb_end_max;

  /* CA NS ampr backoff in db10 */
  int16 ca_ns_ampr_db10;

} rfnv_lte_ca_ns_ampr_table_type;
typedef PACK(struct)
{
  /* channel boundaries for ampr backoff region */
  int32 tx_freq_min;
  /* channel boundaries for ampr backoff region */
  int32 tx_freq_max;

  /* Aggregated modulation type mask for ampr backoff region */
  uint32 agg_modulation_type_mask;

  /*Aggregated channel bandwidth mask for ampr backoff region */
  /* bit 0 - class A (NA), bit 1 - class B, bit 2 - class C, and so on */
  uint32 bandwidth_class;

  /* Aggregate RB start boundaries for ampr backoff region */
  int16 agg_rb_start_min;
  /* Aggregate RB start boundaries for ampr backoff region */
  int16 agg_rb_start_max;
  /* Aggregate RB block boundaries for ampr backoff region */
  int16 agg_rb_block_min;
  /* Aggregate RB block boundaries for ampr backoff region */
  int16 agg_rb_block_max;

  /* Aggregate RB end boundaries for ampr backoff region */
  int16 agg_rb_end_min;
  /* Aggregate RB end boundaries for ampr backoff region */
  int16 agg_rb_end_max;

  /* CA NS ampr backoff in db10 */
  int16 ca_ns_ampr_db10;

} rfnv_lte_ca_ns_ampr_table_type_v2;

typedef PACK(struct)
{
  /* channel boundaries for ampr backoff region */
  int32 tx_earfcn_min;
  /* channel boundaries for ampr backoff region */
  int32 tx_earfcn_max;

  /* Aggregated modulation type mask for ampr backoff region */
  uint32 agg_modulation_type_mask;

  /*Aggregated channel bandwidth mask for ampr backoff region */
  /* bit 0 - class A (NA), bit 1 - class B, bit 2 - class C, and so on */
  uint32 bandwidth_class;

  /* Aggregate RB start boundaries for ampr backoff region */
  int16 agg_rb_start_min;
  /* Aggregate RB start boundaries for ampr backoff region */
  int16 agg_rb_start_max;
  /* Aggregate RB block boundaries for ampr backoff region */
  int16 agg_rb_block_min;
  /* Aggregate RB block boundaries for ampr backoff region */
  int16 agg_rb_block_max;

  /* Aggregate RB end boundaries for ampr backoff region */
  int16 agg_rb_end_min;
  /* Aggregate RB end boundaries for ampr backoff region */
  int16 agg_rb_end_max;

  /* CA NS ampr backoff in db10 */
  int16 ca_ns_ampr_db10;

  /* CA NS step size for ampr calculation with gpost gain */
  int16 ca_ns_ampr_step;

} rfnv_lte_ca_ns_ampr_table_type_v3;

/* RFNV_DATA_TYPE_LTE_CA_NS_AMPR RFNV variant typeid 159 to be used with Tx Static NV */
typedef PACK(struct)
{
  /* NV Container Index for each Tx path for a given LTE band */
  uint8 nv_container_index;

  /* Reserved for now, not used. */
  uint8 reserved;

  /* CA-Network Signalling Value */
  uint32 ca_ns_number;

  /* AMPR table structure */
  rfnv_lte_ca_ns_ampr_table_type ca_ns_ampr_table[RFNV_TX_STATIC_AMPR_TABLE_SIZE];

} rfnv_ca_ns_ampr_static_data_type;
/* RFNV_DATA_TYPE_LTE_CA_NS_AMPR RFNV_V2 variant typeid 448 to be used with Tx Static NV */
typedef PACK(struct)
{
  /* NV Container Index for each Tx path for a given LTE band */
  uint8 nv_container_index;

  /* Reserved for now, not used. */
  uint8 reserved;

  /* CA-Network Signalling Value */
  uint32 ca_ns_number;

  /* AMPR table structure */
  rfnv_lte_ca_ns_ampr_table_type_v2 ca_ns_ampr_table[RFNV_TX_STATIC_AMPR_TABLE_SIZE];

} rfnv_ca_ns_ampr_static_data_type_v2;

/* RFNV_DATA_TYPE_LTE_CA_NS_AMPR RFNV_v3 variant typeid 509 to be used with Tx Static NV */
typedef PACK(struct)
{
  /* NV Container Index for each Tx path for a given LTE band */
  uint8 nv_container_index;

  /* NV valid flag - hard-coded vs NV based AMPR. */
  uint8 nv_valid_flag;

  /* CA-Network Signalling Value */
  uint32 ca_ns_number;

  /* Post Antenna connector gain */
  uint16 gpost_connect;

  /* AMPR table structure */
  rfnv_lte_ca_ns_ampr_table_type_v3 ca_ns_ampr_table[RFNV_TX_STATIC_AMPR_TABLE_SIZE];

} rfnv_ca_ns_ampr_static_data_type_v3;

typedef PACK(struct)
{
  uint16 tech;
  uint64 band_map;
  uint8 detune_code_size_in_bytes;
  uint8 detune_code[ RFNV_ATUNER_MAX_TUNE_CODE_SIZE];
}rfnv_atuner_detune_data_type;


/*RFNV_DATA_TYPE_ATUNER_SETTINGS_ANTENNA_GROUP_DELIMITER*/

typedef PACK(struct)
{
  /* Version */
  uint8 version;

  /* Use antenna for detune */
  uint8 use_ant_for_detune[RFNV_ATUNER_MAX_NUM_ANTENNAS];

  /* reserved */
  uint8 reserved[16];

}rfnv_atuner_use_ant_for_detune_data_type;

/*RFNV_DATA_TYPE_ATUNER_USE_ANT_FOR_DETUNE_TYPE*/

typedef PACK(struct)
{
  /* Version */
  uint8 version;

  /* Wifi sharing mode (mode in NvDefinition file) */
  uint8 wifi_sharing_mode;

  /* Wifi sharing tunecode (tunecode in NvDefinition file) */
  uint8 wifi_sharing_tunecode[RFNV_ATUNER_MAX_TUNE_CODE_SIZE];

  /* Wifi band (band_num in NvDefinition file)*/
  uint8 wifi_band;

  /* size of wifi tunecode */
  uint8 size_of_tune_code;

  /* reserved*/
  uint8 reserved[14];

}rfnv_atuner_wifi_sharing_data_type;

/*RFNV_DATA_TYPE_ATUNER_WIFI_SHARING_TYPE*/

typedef PACK(struct)
{
  /* Version */
  uint8 version;

  /* Ant_Num */
  uint8 ant_num;

  /* reserved_1 */
  uint8 reserved_1;

  /* reserved_2 */
  uint8 reserved_2;

}rfnv_atuner_settings_ant_group_delimiter_data_type;

/*RFNV_DATA_TYPE_ATUNER_AOL_TO_UI_MAPPING_TYPE*/

typedef PACK(struct)
{
  /* Version */
  uint8 version;

  /* AOL mapping for different UIs */
  uint8 mapping_data[RFNV_ATUNER_AOL_TO_UI_MAPPING_MAX_UIS];

}rfnv_atuner_aol_to_ui_mapping_data_type;

/*RFNV_DATA_TYPE_ATUNER_AOL_TO_ICL_MAPPING_TYPE*/

typedef PACK(struct)
{
  /* AOL index */
  uint8 aol_index;

  /* ICL mapping that needs to be used when AOL index is found */
  uint8 icl_mapping[RFNV_ATUNER_MAX_NUM_ICL_SCENARIOS];

}rfnv_atuner_aol_to_icl_mapping_payload_type;

typedef PACK(struct)
{
  /* Version */
  uint8 version;

  /* icl mapping for different AOL */
  rfnv_atuner_aol_to_icl_mapping_payload_type mapping_data[RFNV_ATUNER_MAX_NUM_AOL];

}rfnv_atuner_aol_to_icl_mapping_data_type;

/*RFNV_DATA_TYPE_ATUNER_MAAT_SETTINGS_TYPE*/

typedef PACK(struct)
{
  /* Starting optimization algorithm for MAAT */
  uint8 start_maat_priority;

  /* tx_headroom_db10 */
  int16 tx_headroom_db10;

  /* rx_headroom_db10 */
  int16 rx_headroom_db10;

  /* ca_headroom_db10 */
  int16 ca_headroom_db10;

  /* tx_critical_threshold_db10 */
  int16 tx_critical_th_db10;

  /* rx_critical_threshold_db10 */
  int16 rx_critical_th_db10;

  /* ca_rx_critical_threshold_db10 */
  int16 ca_rx_critical_th_db10;

  /* tx_threshold_hysteresis_db10 */
  int16 tx_th_hys_db10;

  /* rx_threshold_hysteresis_db10 */
  int16 rx_th_hys_db10;

  /* tx_integration_time_ms */
  uint16 tx_integ_time_ms;

  /* rx_integration_time_ms */
  uint16 rx_integ_time_ms;

  /* reserved_1 */
  int16 reserved_1;

  /* reserved_2 */
  int16 reserved_2;

  /* reserved_3 */
  int16 reserved_3;

  /* reserved_4 */
  int16 reserved_4;

}rfnv_atuner_maat_static_settings_type;

typedef PACK(struct)
{
  /* Version */
  uint8 version;

  /* tech_id */
  uint8 tech_id;

  /*Array of band numbers */
  uint16 band_num[RFNV_ATUNER_MAX_NUM_BANDS];

  /* icl mapping for different AOL */
  rfnv_atuner_maat_static_settings_type maat_static_settings;

 }rfnv_atuner_maat_settings_data_type;

/*RFNV_DATA_TYPE_ATUNER_SETTINGS_TYPE*/

typedef PACK(struct)
{

   /* vswr_trigger_time_ms */
  uint16 vswr_trigger_time_ms;

  /* adaptation_time_ms */
  int16 adaptation_time_ms;

  /* closed_loop_tx_pwr_switch_threshold_dbm10 */
  int16 cl_tx_pwr_switch_th_dbm10;

  /* acl_lock_threshold */
  int16 acl_lock_th;

  /* acl_unlock_threshold */
  int16 acl_unlock_th;

  /* Tx_impedance_algo_type */
  uint8 tx_impedance_algo_type;

  /* Tx_aperture_algo_type_when_cl_is_enabled */
  uint8 tx_aper_algo_type_when_cl_enabled;

  /* Rx_algo_type */
  uint8 rx_algo_type;

  /* acl_valid_impedance_scenario_bitmask */
  uint8 acl_valid_impedance_scenario_bitmask[18];

  /* acl_ol_starting_impedance_scenario */
  uint8 acl_ol_starting_impedance_scenario;

  /* acl_ol_starting_aperture_scenario */
  uint8 acl_ol_starting_aperture_scenario;

  /* acl_ui _offset_tolerance*/
  uint8 acl_ui_offset_tolerance;

}rfnv_atuner_static_settings_type;

typedef PACK(struct)
{
  /* Version */
  uint8 version;

  /* tech_id */
  uint8 tech_id;

  /*Array of band numbers */
  uint16 band_num[RFNV_ATUNER_MAX_NUM_BANDS];

  rfnv_atuner_static_settings_type atuner_static_settings;

}rfnv_atuner_settings_data_type;



/*RFNV_DATA_TYPE_ATUNER_DETUNE_SETTINGS_TYPE*/

typedef PACK(struct)
{
  /* enable detune*/
  uint8 enable_detune;

  /*detune tune code size in bytes*/
  uint8 detune_code_size_in_bytes;

  /*detune tune code*/
  uint8 detune_code[ RFNV_ATUNER_MAX_TUNE_CODE_SIZE];

}rfnv_atuner_detune_static_settings_type;

typedef PACK(struct)
{
  /* Version */
  uint8 version;

  /* tech_id */
  uint8 tech_id;

  /*Array of band numbers */
  uint16 band_num[RFNV_ATUNER_MAX_NUM_BANDS];

  rfnv_atuner_detune_static_settings_type  detune_static_settings;

}rfnv_atuner_detune_settings_data_type;


/*RFNV_DATA_TYPE_ATUNER_CL_TX_OFFSETS_SETTINGS_TYPE*/

typedef PACK(struct)
{
  /* Real part */
  int8 real;

  /* Imag part */
  int8 imag;

}rfnv_atuner_vect_16_type;

typedef PACK(struct)
{
  /* Real part */
  rfnv_atuner_vect_16_type offset_data[3];

}rfnv_atuner_cl_tx_offset_data_type;


/*RFNV_DATA_TYPE_ATUNER_CL_TX_OFFSETS_SETTINGS_TYPE*/

typedef PACK(struct)
{
  /* Version */
  uint8 version;

  /* tech_id */
  uint8 tech_id;

  /*Array of band numbers */
  uint16 band_num[RFNV_ATUNER_MAX_NUM_BANDS];

  rfnv_atuner_cl_tx_offset_data_type tx_offset_static_data;

}rfnv_atuner_cl_tx_offset_settings_data_type;

/*RFNV_DATA_TYPE_ATUNER_CL_CA_PCC_EXCLUDED_BANDS_TYPE*/
/*RFNV_DATA_TYPE_ATUNER_CL_CA_SCC_EXCLUDED_BANDS_TYPE */
typedef PACK(struct)
{
  /* Version */
  uint8 version;

  /* tech_id */
  uint8 tech_id;

  /*Array of band numbers */
  uint16 band_num[RFNV_ATUNER_MAX_NUM_BANDS];

}rfnv_atuner_cl_ca_excluded_bands_data_type;

/*RFNV_DATA_TYPE_ATUNER_CA_SCC_ACTIVATION_BANDS_TYPE*/
typedef PACK(struct)
{
  /* tech_id */
  uint8 tech_id;

  /*Array of band numbers */
  uint16 band_num[RFNV_ATUNER_MAX_NUM_BANDS];

}rfnv_atuner_ca_scc_activation_bands_data_type;

/* RFNV_DATA_TYPE_ATUNER_ACL_PARAMS_ANTENNA_GROUP_DELIMITER */
typedef PACK(struct)
{
  /* antenna_num */
  uint8 antenna_num;

}rfnv_atuner_acl_params_ant_group_delimiter_data_type;

/*RFNV_DATA_TYPE_ATUNER_ACL_PARAMS_PATH_GROUP_DELIMITER*/
typedef PACK(struct)
{
  /*path_id*/
  uint8 path_id;

  /*Number of frequency bins*/
  uint8 num_of_freqz_bins;

  /*Number of impedance scenarios*/
  uint8 num_of_impedance_scenarios;

}rfnv_atuner_acl_params_path_group_delimiter_data_type;

/*RFNV_DATA_ATUNER_CL_TC_TBL_DATA*/
typedef PACK(struct)
{
  /*table type*/
  uint8 table_type;

  /*tune code size*/
  uint8 tune_code_size;

  /*Number of  scenarios*/
  uint8 num_of_scenarios;

  /*scenarios*/
  uint8  scenario_tune_code[RFNV_ATUNER_MAX_CL_SCENARIOS][RFNV_ATUNER_MAX_TUNE_CODE_SIZE];
}rfnv_atuner_cl_tc_tbl_data_type;

typedef PACK(struct)
{
  /*table type*/
  uint8 table_type;

  /*tune code size*/
  uint8 tune_code_size;

  /*Number of  scenarios*/
  uint8 num_of_scenarios;

  /*scenarios*/
  uint8  scenario_tune_code[RFNV_ATUNER_MAX_CL_SCENARIOS_V2][RFNV_ATUNER_MAX_TUNE_CODE_SIZE];
}rfnv_atuner_cl_tc_tbl_data_type_v2;

/*RFNV_DATA_TYPE_ATUNER_CA_PCC_ACTIVE_DETUNE_BANDS_TYPE*/
/*RFNV_DATA_TYPE_ATUNER_CA_SCC_ACTIVE_DETUNE_BANDS_TYPE*/
typedef PACK(struct)
{
  /* tech_id */
  uint8 tech_id;

  /*Array of band numbers */
  uint16 band_num[RFNV_ATUNER_MAX_NUM_BANDS];

}rfnv_atuner_ca_pcc_active_detune_bands_data_type;

typedef PACK(struct)
{
  uint8 enable_bit;
  uint8 detune_priority_for_ant[RFNV_ATUNER_MAX_NUM_ANTENNAS-1];
}rfnv_atuner_detune_priority_logic_conflict_info_for_antenna_data_type;


typedef PACK(struct)
{
  uint8 num_antennas;
  rfnv_atuner_detune_priority_logic_conflict_info_for_antenna_data_type conflict_data_for_ant[RFNV_ATUNER_MAX_NUM_ANTENNAS];
}rfnv_atuner_detune_priority_logic_conflict_data_type;


typedef PACK(struct)
{

  /* osl_char_path_id */
  uint8 osl_char_path_id;

  /* aperture_tune_code_mask */
  uint8 aperture_tune_code_mask[ RFNV_ATUNER_MAX_TUNE_CODE_SIZE];

  /* detune_tune_code_mask */
  uint8 detune_tune_code_mask[ RFNV_ATUNER_MAX_TUNE_CODE_SIZE];

  /* reserved_tune_code_mask */
  uint8 reserved_tune_code_mask[ RFNV_ATUNER_MAX_TUNE_CODE_SIZE];

}rfnv_atuner_cl_sub_antenna_switch_path_static_settings_type;

typedef PACK(struct)
{

   /* enable_intermediate_transition_state */
  uint8 enable_intermediate_transition_state;

  /* reserved1 */
  uint8 reserved1;

  /* reserved2 */
  uint8 reserved2;

  rfnv_atuner_cl_sub_antenna_switch_path_static_settings_type path_static_data[2];

}rfnv_atuner_cl_sub_antenna_switch_static_settings_type;


typedef PACK(struct)
{
  /* Version */
  uint8 version;

  /* tech_id */
  uint8 tech_id;

  /*Array of band numbers */
  uint16 band_num[RFNV_ATUNER_MAX_NUM_BANDS];

  rfnv_atuner_cl_sub_antenna_switch_static_settings_type cl_sub_ant_switch_static_data;

}rfnv_atuner_cl_sub_antenna_switch_settings_data_type;


typedef PACK(struct)
{
  /* channel boundaries for mpr backoff region */
  int32 tx_earfcn_min;
  /* channel boundaries for mpr backoff region */
  int32 tx_earfcn_max;

  /* Aggregated modulation type mask for mpr backoff region */
  uint32 agg_modulation_type_mask;

  /*Aggregated channel bandwidth mask for mpr backoff region */
  /* bit 0 - 25RB_100RB
     bit 1 - 50RB_100RB
     bit 2 - 75RB_75RB
     bit 3 - 75RB_100RB
     bit 4 - 100RB_100RB */
  uint32 agg_bw_mask;

  /* Aggregate RB start boundaries for mpr backoff region */
  int16 agg_rb_start_min;
  /* Aggregate RB start boundaries for mpr backoff region */
  int16 agg_rb_start_max;
  /* Aggregate RB block boundaries for mpr backoff region */
  int16 agg_rb_block_min;
  /* Aggregate RB block boundaries for mpr backoff region */
  int16 agg_rb_block_max;

  /* CA mpr backoff in db10 */
  int16 ca_mpr_db10;
} rfnv_lte_ca_mpr_table_type;

typedef PACK(struct)
{
  /* channel boundaries for mpr backoff region */
  int32 tx_freq_min;
  /* channel boundaries for mpr backoff region */
  int32 tx_freq_max;

  /* Aggregated modulation type mask for mpr backoff region */
  uint32 agg_modulation_type_mask;

  /*Aggregated channel bandwidth mask for mpr backoff region */
  /* bit 0 - 25RB_100RB
     bit 1 - 50RB_100RB
     bit 2 - 75RB_75RB
     bit 3 - 75RB_100RB
     bit 4 - 100RB_100RB */
  uint32 agg_bw_mask;

  /* Aggregate RB start boundaries for mpr backoff region */
  int16 agg_rb_start_min;
  /* Aggregate RB start boundaries for mpr backoff region */
  int16 agg_rb_start_max;
  /* Aggregate RB block boundaries for mpr backoff region */
  int16 agg_rb_block_min;
  /* Aggregate RB block boundaries for mpr backoff region */
  int16 agg_rb_block_max;

  /* CA mpr backoff in db10 */
  int16 ca_mpr_db10;
} rfnv_lte_ca_mpr_table_type_v2;
/* RFNV_DATA_TYPE_LTE_CA_MPR RFNV variant typeid 229 to be used with Tx Static NV */
typedef PACK(struct)
{
  /* NV Container Index for each Tx path for a given LTE band */
  uint8 nv_container_index;
  /* Reserved for now, not used. */
  uint8 reserved;
  /* Cluster Mask:
     00000001 - single cluster
     00000010 - Multicluster */
  uint8 active_cluster_mask;
  /* BW class mask*/
  uint32 bw_class_mask;
 /* MPR table structure */
  rfnv_lte_ca_mpr_table_type ca_mpr_table[RFNV_TX_STATIC_MPR_TABLE_SIZE];
} rfnv_ca_mpr_static_data_type;

/* RFNV_DATA_TYPE_LTE_CA_MPR RFNV variant typeid 446 to be used with Tx Static NV */
typedef PACK(struct)
{
  /* NV Container Index for each Tx path for a given LTE band */
  uint8 nv_container_index;
  /* Reserved for now, not used. */
  uint8 reserved;
  /* Cluster Mask:
     00000001 - single cluster
     00000010 - Multicluster */
  uint8 active_cluster_mask;
  /* BW class mask*/
  uint32 bw_class_mask;
 /* MPR table structure */
  rfnv_lte_ca_mpr_table_type_v2 ca_mpr_table[RFNV_TX_STATIC_MPR_TABLE_SIZE];
} rfnv_ca_mpr_static_data_type_v2;

typedef PACK(struct)
{
  /* channel boundaries for mpr backoff region */
  int32 tx_earfcn_min;
  /* channel boundaries for mpr backoff region */
  int32 tx_earfcn_max;

  /* Aggregated modulation type mask for mpr backoff region */
  uint32 agg_modulation_type_mask;

  /*Aggregated channel bandwidth mask for mpr backoff region */
  /* bit 0 - class A (NA), bit 1 - class B, bit 2 - class C, and so on */
  uint32 bandwidth_class;

  /* Aggregate RB start boundaries for mpr backoff region */
  int16 agg_rb_start_min;
  /* Aggregate RB start boundaries for mpr backoff region */
  int16 agg_rb_start_max;
  /* Aggregate RB block boundaries for mpr backoff region */
  int16 agg_rb_block_min;
  /* Aggregate RB block boundaries for mpr backoff region */
  int16 agg_rb_block_max;

  /* CA NS mpr backoff in db10 */
  int16 ca_ns_mpr_db10;
} rfnv_lte_ca_ns_mpr_table_type;


/* RFNV_DATA_TYPE_LTE_CA_NS_MPR RFNV variant typeid 159 to be used with Tx Static NV */
typedef PACK(struct)
{
  /* NV Container Index for each Tx path for a given LTE band */
  uint8 nv_container_index;
  /* Reserved for now, not used. */
  uint8 reserved;
  /* CA-Network Signalling Value */
  uint32 ca_ns_number;
 /* MPR table structure */
  rfnv_lte_ca_ns_mpr_table_type ca_mpr_table[RFNV_TX_STATIC_MPR_TABLE_SIZE];

} rfnv_ca_ns_mpr_static_data_type;

/* QMSL_RX_SPUR_DATA_TYPE (type ID 200) */
typedef PACK(struct)
{
  /*! Path index or NV container, used to connect this item to the RFC
      data structures. */
  uint8 nv_container;
  /*! Absolute frequency(Hz) at which spur is present. */
  uint64 spur_freq_hz;
  /*! Notch depth (hardware dependent) to be used supress the spur. */
  uint8 notch_depth;
  /*! Rx power threshold in dBm10 below which a notch will be applied to the
      spur. */
  int32 spur_thresh;
  /*! Hysteresis in dB10, the level above the Rx threshold above which the
      notch will be removed. */
  uint16 hysteresis;
} rfnv_rx_spur_data_type;


/* RFNV_DATA_TYPE_STATIC_MULTI_SPUR_DATA_LIST (type ID 510) */
typedef PACK(struct)
{
  /*! BW */
  uint32 bw_khz;
  /*! Absolute frequency(Hz) at which spur is present. */
  uint64 spur_freq_hz[RFNV_RX_STATIC_SPUR_TABLE_SIZE];
  /*! Notch depth (hardware dependent) to be used supress the spur. */
  uint8 notch_depth[RFNV_RX_STATIC_SPUR_TABLE_SIZE];
  /*! Rx power threshold in dBm10 below which a notch will be applied to the
      spur. */
  int32 spur_thresh[RFNV_RX_STATIC_SPUR_TABLE_SIZE];
  /*! Hysteresis in dB10, the level above the Rx threshold above which the
      notch will be removed. */
  uint16 hysteresis[RFNV_RX_STATIC_SPUR_TABLE_SIZE];
  /*  reserved for debug option */
  uint32 option;
} rfnv_rx_multi_spur_data_type;

/* RFNV_DATA_TYPE_STATIC_MULTI_SPUR_DATA_LIST_V2 (type ID 577) */
typedef PACK(struct)
{
  /*! BW */
  uint32 bw_khz;
  /*! Absolute frequency(Hz) at which spur is present. */
  uint64 spur_freq_hz[RFNV_RX_STATIC_SPUR_TABLE_SIZE];
  /*! Notch depth (hardware dependent) to be used supress the spur. */
  uint8 notch_depth[RFNV_RX_STATIC_SPUR_TABLE_SIZE];
  /*! Spur strength per SDR GS per spur. */
  int8 spur_strength[RFNV_RX_STATIC_SPUR_TABLE_SIZE][RFNV_MAX_STATIC_LNA_GAIN_STATES];
} rfnv_rx_multi_spur_v2_data_type;

/* RFNV_DATA_TYPE_SUB6_RX_SAWLESS_PARAMS (type ID 542) */
typedef PACK(struct)
{
  /*! noise threashold x1 in state machine */
  int8 noise_thresh_x1;
  /*! noise threashold x1 in state machine */
  int8 noise_thresh_x2;
  /*! the reciprocal for the min filter coeff */
  int8 noise_filt_coeff_recip;
  /*! the threshold for CRC BLER % */
  int8 crc_bler_thresh;
  /*! timer for CRC BLER count */
  uint16 timer_for_crc_bler_cnt_ms;
  /*! timer for noise accumulation */
  uint16 timer_for_noise_accum_ms;
  /*! the high threshold of frequency, which is used to determine the thermal saw floor.
  If freq is less than this thresh, then use the corresponding thermal saw floor*/
  uint32 freq_high_thresh_khz[RFNV_RX_STATIC_SUB6_SAWLESS_TABLE_SIZE];
  /*! thermal saw floor in dBm/Hz */
  int16 thermal_saw_floor_db100[RFNV_RX_STATIC_SUB6_SAWLESS_TABLE_SIZE];
  /*! the high threshold of frequency, which used to determin gain char between saw and sawless path */
  uint32 gain_freq_high_thresh_khz[RFNV_RX_STATIC_SUB6_SAWLESS_TABLE_SIZE];
  /*! gain delta between saw and sawless path, positive */
  uint8 gain_delta_db10[RFNV_RX_STATIC_SUB6_SAWLESS_TABLE_SIZE];
} rfnv_rx_sawless_static_data_t;


/* RFNV_DATA_TYPE_TX_LIN_VS_TEMP_FREQ RFNV variant typeid 208 to be used with
   Tx Static NV */
typedef PACK(struct)
{
  /*! NV Container Index for each Tx path for a given LTE band */
  uint8 nv_container_index;

  /*! Reserved for now, not used. */
  uint8 reserved;

  /*! Radio bandwidth bitmask that defines which modes this configuration
      data applies to.
      Bit 0: 1.4MHz
      Bit 1:   3MHz
      Bit 2:   5MHz
      Bit 3:  10MHz
      Bit 4:  15MHz
      Bit 5:  20MHz
      Bit 6:  40MHz
      Bit 7:  20MHz + 20MHz intra-contiguous ULCA
  */
  uint32 bandwidth_mask;

  /*! channel list*/
  int32 channel_list[RFNV_LTE_TX_CHAN_SIZE];

  /*! PA input power compensation vs temperature and frequency. */
  int16 tx_lin_vs_temp_vs_freq_p_in[RFNV_LTE_TX_TEMP_BIN_SIZE][RFNV_LTE_TX_CHAN_SIZE][RFNV_LTE_TX_PA_STATE];

  /*! Tx output power compensation vs temperature and frequency. */
  int16 tx_lin_vs_temp_vs_freq_p_out[RFNV_LTE_TX_TEMP_BIN_SIZE][RFNV_LTE_TX_CHAN_SIZE][RFNV_LTE_TX_PA_STATE];

} rfnv_tx_lin_vs_freq_vs_temp_data_type;

/* Variant type 210 */
typedef PACK (struct)
{
  /* Volterra Mode, 0 = Disable, 1 = Enable, 2 = Enable, No Update */
  uint8 volterra_mode;
  /* Auto Pin Mode, 0 = Disable, 1 = Enable, 2 = Enable, No Update */
  uint8 autopin_mode;
  /* Auto Delay Mode, 0 = Disable, 1 = Enable, 2 = Enable, No Update */
  uint8 autodelay_mode;
  /* DPD Mode,  0 = Memory Mask DPD, 1 = MemoryLess DPD */
  uint8 dpd_mode;
} rfnv_xpt_static_mode_ctrl_type;

/* Variant type 300 */
typedef PACK (struct)
{
  /* Volterra Mode, 0 = Disable, 1 = Enable, 2 = Enable, No Update,
  3 = Single carrier only Enable, 4 = Multi carrier only Enable */
  uint8 volterra_mode;
  /* Auto Pin Mode, 0 = Disable, 1 = Enable, 2 = Enable, No Update */
  uint8 autopin_mode;
  /* Auto Delay Mode, 0 = Disable, 1 = Enable, 2 = Enable, No Update */
  uint8 autodelay_mode;
  /* Online Volterra Memory 0 = Disable, 1 = Enable */
  uint8 volterra_mem_enable;
  /* Memory kernel mask override */
  uint32 mem_kernel_mask_override;
} rfnv_xpt_static_mode_ctrl_v2_type;

/* Variant type 498 */
typedef PACK (struct)
{
  /* Online DPD Mode, 0 = Disable, 1 = Enable, 2 = Enable, No Update,
  3 = Single carrier only Enable, 4 = Multi carrier only Enable */
  uint8 online_dpd_mode;
  /* Auto Pin Mode, 0 = Disable, 1 = Enable, 2 = Enable, No Update */
  uint8 autopin_mode;
  /* Auto Delay Mode, 0 = Disable, 1 = Enable, 2 = Enable, No Update */
  uint8 autodelay_mode;
} rfnv_xpt_static_mode_ctrl_v3_type;

/* RFNV_DATA_TYPE_ID_XPT_MODE_CONTROL_V4 : 567 */
typedef PACK (struct)
{
  /* PA state info */
  uint8 pa_state;
  /* Auto Pin Mode, 0 = Disable, 1 = Enable, 2 = Enable, No Update */
  uint8 autopin_mode;
  /* Auto Delay Mode, 0 = Disable, 1 = Enable, 2 = Enable, No Update */
  uint8 autodelay_mode;
  /* Field for modulation info*/
  uint8 modulation_mask;
  /* Optional field - for eg. to feed temperature or debug info */
  uint8 temperature;
} rfnv_xpt_static_mode_ctrl_v4_type;

/* Bandwidth header info used for XPT variant types */
typedef PACK (struct)
{
  int32 min_bw_kHz;
  int32 max_bw_kHz;
} rfnv_xpt_bw_header;

/* Variant type 211 */
typedef PACK(struct)
{
  /* TX BW info */
  rfnv_xpt_bw_header bw_info;
  /* Tx channel info */
  rfnv_data_chan_info_type chan_info;
  /* Pout offsets for PA state Vs Temp Vs Chan */
  int16 pout_vs_temp_vs_freq[RFNV_TEMP_BIN_SIZE]
                            [RFNV_CHAN_INFO_CHANNEL_LIST_SIZE]
                            [RFNV_TX_NUM_PA_STATE];
  /* Pin offsets for PA state Vs Temp Vs Chan */
  int16 pin_vs_temp_vs_freq[RFNV_TEMP_BIN_SIZE]
                           [RFNV_CHAN_INFO_CHANNEL_LIST_SIZE]
                           [RFNV_TX_NUM_PA_STATE];
  /* reference Pin for each PA state */
  int16 ref_pin_offset[RFNV_TX_NUM_PA_STATE];
} rfnv_xpt_static_pin_pout_data_type;

/* Variant type 435 */
typedef PACK(struct)
{
  /* TX BW info */
  rfnv_xpt_bw_header bw_info;
  /* Tx freq info */
  rfnv_data_freq_info_type freq_info;
  /* Pout offsets for PA state Vs Temp Vs Chan */
  int16 pout_vs_temp_vs_freq[RFNV_TEMP_BIN_SIZE]
                            [RFNV_CHAN_INFO_CHANNEL_LIST_SIZE]
                            [RFNV_TX_NUM_PA_STATE];
  /* Pin offsets for PA state Vs Temp Vs Chan */
  int16 pin_vs_temp_vs_freq[RFNV_TEMP_BIN_SIZE]
                           [RFNV_CHAN_INFO_CHANNEL_LIST_SIZE]
                           [RFNV_TX_NUM_PA_STATE];
  /* reference Pin for each PA state */
  int16 ref_pin_offset[RFNV_TX_NUM_PA_STATE];
} rfnv_xpt_static_pin_pout_v2_data_type;

/* Variant type 212 */
typedef PACK (struct)
{
  /* TX BW info */
  rfnv_xpt_bw_header bw_info;
  /* Tx channel info */
  rfnv_data_chan_info_type chan_info;
  /*ADC temperature reading */
  uint32 therm_adc;
  /* ET delay offsets based on temp vs freq */
  int32 delay_vs_freq[RFNV_CHAN_INFO_CHANNEL_LIST_SIZE];
} rfnv_xpt_static_analog_delay_data_type;

/* Variant type 315 */
typedef PACK (struct)
{
  /* pa state */
  uint8 pa_state;
  /* V1 data type */
  rfnv_xpt_static_analog_delay_data_type analog_delay_data_v1;
} rfnv_xpt_static_analog_delay_v2_data_type;

/* Variant type 436 */
typedef PACK (struct)
{
  /* pa state */
  uint8 pa_state;
  /* TX BW info */
    rfnv_xpt_bw_header bw_info;
  /* Tx freq info */
  rfnv_data_freq_info_type freq_info;
  /*ADC temperature reading */
  uint32 therm_adc;
  /* ET delay offsets based on temp vs freq */
  int32 delay_vs_freq[RFNV_CHAN_INFO_CHANNEL_LIST_SIZE];
} rfnv_xpt_static_analog_delay_v3_data_type;

/* Variant type 544 */
typedef PACK (struct)
{
  /* pa state */
  uint8 pa_state;
  /* TX BW info */
    rfnv_xpt_bw_header bw_info;
  /* Tx freq info */
  rfnv_data_freq_info_type freq_info;
  /*ADC temperature reading */
  uint32 therm_adc;
  /* ET delay offsets vs freq */
  int32 delay_vs_freq[RFNV_CHAN_INFO_CHANNEL_LIST_SIZE];
/* ET partial RB delay offsets vs freq */
  int32 partial_rb_delay_vs_freq[RFNV_CHAN_INFO_CHANNEL_LIST_SIZE];
} rfnv_xpt_static_analog_delay_v4_data_type;

/* Variant type 213 */
typedef PACK (struct)
{
  /* TX BW info */
  rfnv_xpt_bw_header bw_info;
  /* Tx channel info */
  rfnv_data_chan_info_type chan_info;
  /* ET analog delay based on freq */
  int32 delay_vs_temp_vs_freq_offset[RFNV_TEMP_BIN_SIZE]
                                    [RFNV_CHAN_INFO_CHANNEL_LIST_SIZE];
} rfnv_xpt_static_analog_delay_offsets_data_type;

/* Variant type 316 */
typedef PACK (struct)
{
  /* pa state */
  uint8 pa_state;
  /* V1 data type */
  rfnv_xpt_static_analog_delay_offsets_data_type analog_delay_offsets_data_v1;
} rfnv_xpt_static_analog_delay_offsets_v2_data_type;

/* Variant type 437 */
typedef PACK (struct)
{
  /* pa state */
  uint8 pa_state;
  /* TX BW info */
  rfnv_xpt_bw_header bw_info;
  /* Tx freq info */
  rfnv_data_freq_info_type freq_info;
  /* ET analog delay based on freq */
  int32 delay_vs_temp_vs_freq_offset[RFNV_TEMP_BIN_SIZE]
                                    [RFNV_CHAN_INFO_CHANNEL_LIST_SIZE];
} rfnv_xpt_static_analog_delay_offsets_v3_data_type;

/* Variant type 214 */
typedef PACK (struct)
{
  /* EPT mode env scale offset */
  int16 ept_env_scale_offset;
  /* ET mode env scale offset */
  int16 et_env_scale_offset;
  /* EPT mode env scale offset for LTE ULCA*/
  int16 ept_env_scale_offset_ulca;
  /* ET mode env scale offset for LTE ULCA*/
  int16 et_env_scale_offset_ulca;
  /* Max power linearizer, 0 = Disable, 1 = Env Scale Boost, 2 = Env Scale Cap */
  uint8 max_power_lin_algo;
  /* env scale boost value */
  int16 lin_algo_env_scale_boost_val;
  /* env scale cap value */
  int16 lin_algo_env_scale_cap_val;
} rfnv_xpt_static_txagc_data_type;

/* Variant type 309 */
typedef PACK (struct)
{
  /* EPT mode env scale offset */
  int16 ept_env_scale_offset;
  /* ET mode env scale offset */
  int16 et_env_scale_offset;
  /* EPT mode env scale offset for LTE ULCA*/
  int16 ept_env_scale_offset_ulca;
  /* ET mode env scale offset for LTE ULCA*/
  int16 et_env_scale_offset_ulca;
  /* Max power linearizer, 0 = Disable, 1 = Env Scale Boost, 2 = Env Scale Cap */
  uint8 max_power_lin_algo;
  /* env scale boost value */
  int16 lin_algo_env_scale_boost_val;
  /* env scale cap value */
  int16 lin_algo_env_scale_cap_val;
  /* txfe clipper offset*/
  int16 txfe_clipper_offset;
  /* txfe clipper offset ULCA*/
  int16 txfe_clipper_offset_ulca;
} rfnv_xpt_static_txagc_data_v2_type;

/* Variant type 278 */
typedef PACK (struct)
{
  /* EPT Prime mode, 0 = Disable, 1 = Enable */
  uint8 mode;
  /* EPT Prime linearizer expansion resolution */
  uint8 lin_expand_res;
  /* EPT Prime linearizer extrapolation */
  uint8 lin_extrap;
} rfnv_xpt_static_ept_prime_type;

typedef PACK(struct)
{
  /*! channel boundaries for mpr backoff region */
  int32 tx_earfcn_min;
  /*! channel boundaries for mpr backoff region */
  int32 tx_earfcn_max;
  /*! Modulation type mask for mpr backoff region
      rfnv_lte_mod_type_s */
  uint32 modulation_type_mask;
  /*! Identifies all the Bandwidths supported for this MPR selection
      rfnv_lte_bw_type_s */
  uint32 bandwidth_type_mask;
  /*! Aggregate RB start boundaries for mpr backoff region */
  int16 agg_rb_start_min;
  /*! Aggregate RB start boundaries for mpr backoff region */
  int16 agg_rb_start_max;
  /*! Aggregate RB block boundaries for mpr backoff region */
  int16 agg_rb_block_min;
  /*! Aggregate RB block boundaries for mpr backoff region */
  int16 agg_rb_block_max;
  /*! mpr backoff in db10 */
  int16 mpr_db10;
} rfnv_lte_mpr_table_t;

typedef PACK(struct)
{
  /*! channel boundaries for mpr backoff region */
  int32 tx_freq_min;
  /*! channel boundaries for mpr backoff region */
  int32 tx_freq_max;
  /*! Modulation type mask for mpr backoff region
      rfnv_lte_mod_type_s */
  uint32 modulation_type_mask;
  /*! Identifies all the Bandwidths supported for this MPR selection
      rfnv_lte_bw_type_s */
  uint32 bandwidth_type_mask;
  /*! Aggregate RB start boundaries for mpr backoff region */
  int16 agg_rb_start_min;
  /*! Aggregate RB start boundaries for mpr backoff region */
  int16 agg_rb_start_max;
  /*! Aggregate RB block boundaries for mpr backoff region */
  int16 agg_rb_block_min;
  /*! Aggregate RB block boundaries for mpr backoff region */
  int16 agg_rb_block_max;
  /*! mpr backoff in db10 */
  int16 mpr_db10;
} rfnv_lte_mpr_table_t_v2;

typedef PACK(struct)
{
  /*! channel boundaries for mpr backoff region */
  int32 tx_earfcn_min;
  /*! channel boundaries for mpr backoff region */
  int32 tx_earfcn_max;
  /*! Modulation type mask for mpr backoff region
      rfnv_lte_mod_type_s */
  uint32 modulation_type_mask;
  /*! Identifies all the Bandwidths supported for this MPR selection
      rfnv_lte_bw_type_s */
  uint32 bandwidth_type_mask;
  /*! Aggregate RB start boundaries for mpr backoff region */
  int16 agg_rb_start_min;
  /*! Aggregate RB start boundaries for mpr backoff region */
  int16 agg_rb_start_max;
  /*! Aggregate RB block boundaries for mpr backoff region */
  int16 agg_rb_block_min;
  /*! Aggregate RB block boundaries for mpr backoff region */
  int16 agg_rb_block_max;
  /*! mpr backoff in db10 */
  int16 mpr_db10;
} rfnv_lte_mpr_table_t_v3;

/* RFNV_DATA_TYPE_LTE_MPR RFNV variant typeid 215 to be used with Tx Static NV */
typedef PACK(struct)
{
  /*! NV Container Index for each Tx path for a given LTE band */
  uint8 nv_container_index;
  /*! Reserved for now, not used. */
  uint8 reserved;
  /* Cluster Mask:
     00000001 - single cluster
     00000010 - Multicluster */
  uint8 active_cluster_mask;
  /*! MPR table structure */
  rfnv_lte_mpr_table_t mpr_table[RFNV_TX_STATIC_MPR_TABLE_SIZE];
} rfnv_mpr_static_data_t;
/* RFNV_DATA_TYPE_LTE_MPR RFNV variant typeid 449 to be used with Tx Static NV */
typedef PACK(struct)
{
  /*! NV Container Index for each Tx path for a given LTE band */
  uint8 nv_container_index;
  /*! Reserved for now, not used. */
  uint8 reserved;
  /* Cluster Mask:
     00000001 - single cluster
     00000010 - Multicluster */
  uint8 active_cluster_mask;
  /*! MPR table structure */
  rfnv_lte_mpr_table_t_v2 mpr_table[RFNV_TX_STATIC_MPR_TABLE_SIZE];
} rfnv_mpr_static_data_t_v2;

typedef PACK(struct)
{
   uint8 catm_nb1_mode;
  /* Cluster Mask:
     00000001 - single cluster
     00000010 - Multicluster */
  uint8 active_cluster_mask;
  /*! MPR table structure */
  rfnv_lte_mpr_table_t_v3 mpr_table[RFNV_TX_STATIC_MPR_TABLE_SIZE];
} rfnv_mpr_static_data_t_v3;

typedef PACK(struct)
{
  /*! Modulation type mask for mpr backoff region
      rfnv_lte_mod_type_s */
  uint32 modulation_type_mask;
  /*! Aggregate RB start boundaries for mpr backoff region */
  int16 agg_rb_start_min;
  /*! Aggregate RB start boundaries for mpr backoff region */
  int16 agg_rb_start_max;
  /*! Aggregate RB block boundaries for mpr backoff region */
  int16 agg_rb_block_min;
  /*! Aggregate RB block boundaries for mpr backoff region */
  int16 agg_rb_block_max;
  /*! Aggregate RB end boundaries for mpr backoff region */
  int16 agg_rb_end_min;
  /*! Aggregate RB end boundaries for mpr backoff region */
  int16 agg_rb_end_max;
  /*! mpr backoff in db10 */
  int16 mpr_db10;
} rfnv_lte_mpr_cont_rb_grant_tbl_t;

typedef PACK(struct)
{
  uint32 modulation_type_mask;
  int16 agg_rb_start_min;
  int16 agg_rb_start_max;
  int16 agg_rb_block_min;
  int16 agg_rb_block_max;
  int16 agg_rb_end_min;
  int16 agg_rb_end_max;
  int16 ampr_db10;
} rfnv_lte_ampr_cont_rb_grant_tbl_t;

typedef PACK(struct)
{
  /*! Modulation type mask for mpr backoff region
      rfnv_lte_mod_type_s */
  uint32 modulation_type_mask;
  /*! A0 Coefficient for MA*/
  int16 ma_a0_coeff_100;
  /*! A1 Coefficient for MA*/
  int16 ma_a1_coeff_1000;
  /*! Total # RB Max*/
  int16 agg_rb_block_max;
  /*! A1 for IM5 region max, A1*BW_agg, Aggregated channel bandwidth, expressed in MHz*/
  int16 im5_agg_bw_coeff_100;
  /*! A0 for IM5 region max, freq out of band*/
  int16 im5_f_oob_100;
  /*! A0 Coefficient for IM5, Mim5*/
  int16 im5_val_100;
  int16 mpr_db10;
} rfnv_lte_mpr_nc_rb_grant_tbl_t;

typedef PACK(struct)
{
  /*! Modulation type mask for mpr backoff region
      rfnv_lte_mod_type_s */
  uint32 modulation_type_mask;
  /*! A0 Coefficient for MA*/
  int16 ma_a0_coeff_100;
  /*! A1 Coefficient for MA*/
  int16 ma_a1_coeff_1000;
  /*! Total # RB Max*/
  int16 agg_rb_block_max;
  int16 ampr_db10;
} rfnv_lte_ampr_nc_rb_grant_tbl_t;


/*! Power class that this data item applies to.
    Implemented as a bitmask.
      Bit0 - Power Class 1
      Bit1 - Power Class 2
      Bit2 - Power Class 3
      ...
    If all Bits are enabled this backoff will be applied to all supported
    UE Power classes. */

/* RFNV_DATA_TYPE_LTE_MPR_SC RFNV variant typeid 500 to be used with Tx Static NV */
typedef PACK (struct)
{
  uint8 num_of_grant_elements;
  uint8 power_class;
  rfnv_lte_mpr_cont_rb_grant_tbl_t mpr_cont_rb_grant_tbl_data[RFNV_TX_STATIC_MPR_CONT_RB_GRANT_TBL_SIZE];
} rfnv_lte_mpr_cont_rb_semi_tbl_t;

typedef PACK (struct)
{
  uint8 num_of_semi_elements;
  uint64 bw_mask; /* A mask captures up to 4 intra-carriers BW, single cluster size 1 */
  int32 tx_earfcn_min;
  int32 tx_earfcn_max;
  rfnv_lte_mpr_cont_rb_semi_tbl_t mpr_cont_rb_semi_tbl_data[RFNV_TX_STATIC_MPR_CONT_RB_SEMI_TBL_SIZE];
} rfnv_mpr_cont_rb_static_data_t;

/* RFNV_DATA_TYPE_LTE_AMPR_SC RFNV variant typeid 501 to be used with Tx Static NV */
typedef PACK (struct)
{
  uint8 num_of_grant_elements;
  uint8 power_class;
  uint64 ns_mask;
  rfnv_lte_ampr_cont_rb_grant_tbl_t ampr_cont_rb_grant_tbl_data[RFNV_TX_STATIC_AMPR_CONT_RB_GRANT_TBL_SIZE];
} rfnv_lte_ampr_cont_rb_semi_tbl_t;

typedef PACK (struct)
{
  uint8 num_of_semi_elements;
  uint64 bw_mask; /* A mask captures up to 4 intra-carriers BW */
  int32 tx_earfcn_min;
  int32 tx_earfcn_max;
  rfnv_lte_ampr_cont_rb_semi_tbl_t ampr_cont_rb_semi_tbl_data[RFNV_TX_STATIC_AMPR_CONT_RB_SEMI_TBL_SIZE];
} rfnv_ampr_cont_rb_static_data_t;

/* RFNV_DATA_TYPE_LTE_MPR_NC RFNV variant typeid 502 to be used with Tx Static NV */
typedef PACK (struct)
{
  uint8 num_of_grant_elements;
  uint8 power_class;
  rfnv_lte_mpr_nc_rb_grant_tbl_t mpr_nc_rb_grant_tbl_data[RFNV_TX_STATIC_MPR_NC_RB_GRANT_TBL_SIZE];
} rfnv_lte_mpr_nc_rb_semi_tbl_t;

typedef PACK (struct)
{
  uint8 num_of_semi_elements;
  uint64 bw_mask; /* Single BW for SC or Aggregated BW for CA */
  int32 tx_earfcn_min;
  int32 tx_earfcn_max;
  rfnv_lte_mpr_nc_rb_semi_tbl_t mpr_nc_rb_semi_tbl_data[RFNV_TX_STATIC_MPR_NC_RB_SEMI_TBL_SIZE];
} rfnv_mpr_nc_rb_static_data_t;

/* RFNV_DATA_TYPE_LTE_AMPR_NC RFNV variant typeid 503 to be used with Tx Static NV */
typedef PACK (struct)
{
  uint8 num_of_grant_elements;
  uint8 power_class;
  uint64 ns_mask;
  rfnv_lte_ampr_nc_rb_grant_tbl_t ampr_nc_rb_grant_tbl_data[RFNV_TX_STATIC_AMPR_NC_RB_GRANT_TBL_SIZE];
} rfnv_lte_ampr_nc_rb_semi_tbl_t;

typedef PACK (struct)
{
  uint8 num_of_semi_elements;
  uint64 bw_mask; /* Single BW for SC or Aggregated BW for CA */
  int32 tx_earfcn_min;
  int32 tx_earfcn_max;
  rfnv_lte_ampr_nc_rb_semi_tbl_t ampr_nc_rb_semi_tbl_data[RFNV_TX_STATIC_AMPR_NC_RB_SEMI_TBL_SIZE];
} rfnv_ampr_nc_rb_static_data_t;

typedef PACK(struct)
{
  /*! Modulation type mask for mpr backoff region
      rfnv_lte_mod_type_s */
  uint32 modulation_type_mask;
  /*! Aggregate RB start boundaries for mpr backoff region */
  int16 agg_rb_start_min;
  /*! Aggregate RB start boundaries for mpr backoff region */
  int16 agg_rb_start_max;
  /*! Aggregate RB block boundaries for mpr backoff region */
  int16 agg_rb_block_min;
  /*! Aggregate RB block boundaries for mpr backoff region */
  int16 agg_rb_block_max;
  /*! Aggregate RB end boundaries for mpr backoff region */
  int16 agg_rb_end_min;
  /*! Aggregate RB end boundaries for mpr backoff region */
  int16 agg_rb_end_max;
  /*! mpr backoff in db10 */
  int16 mpr_db10;
} rfnv_lte_mpr_supplementary_grant_tbl_t;

typedef PACK(struct)
{
  uint32 modulation_type_mask;
  int16 agg_rb_start_min;
  int16 agg_rb_start_max;
  int16 agg_rb_block_min;
  int16 agg_rb_block_max;
  int16 agg_rb_end_min;
  int16 agg_rb_end_max;
  int16 ampr_db10;
  int16 total_max_backoff;
} rfnv_lte_ampr_supplementary_grant_tbl_t;

typedef PACK(struct)
{
  uint32 modulation_type_mask;
  int16 agg_rb_start_min;
  int16 agg_rb_start_max;
  int16 agg_rb_block_min;
  int16 agg_rb_block_max;
  int16 agg_rb_end_min;
  int16 agg_rb_end_max;
  int16 ampr_db10;
  int16 total_max_backoff;
  int16 ampr_step;  /*100 times the value defined in the protocol.*/
  uint16 gpost_connector;
} rfnv_lte_ampr_supplementary_grant_tbl_t_v2;

/*! Power class that this data item applies to.
    Implemented as a bitmask.
      Bit0 - Power Class 1
      Bit1 - Power Class 2
      Bit2 - Power Class 3
      ...
    If all Bits are enabled this backoff will be applied to all supported
    UE Power classes. */

/* RFNV_DATA_TYPE_LTE_MPR_SC_SUPPLEMENTARY RFNV variant typeid 504 to be used with Tx Static NV */
typedef PACK (struct)
{
  uint8 num_of_grant_elements;
  uint8 power_class;
  rfnv_lte_mpr_supplementary_grant_tbl_t mpr_supplementary_grant_tbl_data[RFNV_TX_STATIC_MPR_SUPPLEMENTARY_GRANT_TBL_SIZE];
} rfnv_lte_mpr_supplementary_semi_tbl_t;

typedef PACK (struct)
{
  uint8 num_of_semi_elements;
  uint64 bw_mask; /* Single BW for SC or Aggregated BW for CA */
  int32 tx_earfcn_min;
  int32 tx_earfcn_max;
  rfnv_lte_mpr_supplementary_semi_tbl_t mpr_supplementary_semi_tbl_data[RFNV_TX_STATIC_MPR_SUPPLEMENTARY_SEMI_TBL_SIZE];
} rfnv_mpr_supplementary_static_data_t;

/* RFNV_DATA_TYPE_LTE_AMPR_SC_SUPPLEMENTARY RFNV variant typeid 505 to be used with Tx Static NV */
typedef PACK (struct)
{
  uint8 num_of_grant_elements;
  uint8 power_class;
  uint64 ns_mask;
  rfnv_lte_ampr_supplementary_grant_tbl_t ampr_supplementary_grant_tbl_data[RFNV_TX_STATIC_AMPR_SUPPLEMENTARY_GRANT_TBL_SIZE];
} rfnv_lte_ampr_supplementary_semi_tbl_t;

typedef PACK (struct)
{
  uint8 num_of_grant_elements;
  uint8 power_class;
  uint64 ns_mask;
  rfnv_lte_ampr_supplementary_grant_tbl_t_v2 ampr_supplementary_grant_tbl_data[RFNV_TX_STATIC_AMPR_SUPPLEMENTARY_GRANT_TBL_SIZE];
} rfnv_lte_ampr_supplementary_semi_tbl_t_v2;

typedef PACK (struct)
{
  uint8 num_of_semi_elements;
  uint64 bw_mask; /* Single BW for SC or Aggregated BW for CA */
  int32 tx_earfcn_min;
  int32 tx_earfcn_max;
  rfnv_lte_ampr_supplementary_semi_tbl_t ampr_supplementary_semi_tbl_data[RFNV_TX_STATIC_AMPR_SUPPLEMENTARY_SEMI_TBL_SIZE];
} rfnv_ampr_supplementary_static_data_t;

typedef PACK (struct)
{
  uint8 num_of_semi_elements;
  uint64 bw_mask; /* Single BW for SC or Aggregated BW for CA */
  int32 tx_earfcn_min;
  int32 tx_earfcn_max;
  rfnv_lte_ampr_supplementary_semi_tbl_t_v2 ampr_supplementary_semi_tbl_data[RFNV_TX_STATIC_AMPR_SUPPLEMENTARY_SEMI_TBL_SIZE];
} rfnv_ampr_supplementary_static_data_t_v2;

typedef PACK(struct)
{
  /* SAR BACKOFF LIMIT: RFNV_LTE_Bxx_DB10_SAR_BACK_OFF_LIMIT_I
     0th index = Default Antenna POS
     1st index = Swap Antenna POS for 8 SAR STATES */
  int16 sar_tx_power_limit_dB10[2][RFNV_MAX_SAR_STATE];

}rfnv_tx_sar_power_limit_control_t;

typedef PACK (struct)
{
  uint32 modulation_type_mask;
  uint8 num_of_power_range;
  uint16 power_range[RFNV_TX_DSDA_DIGITAL_BACKOFF_GRANT_POWER_TBL_SIZE];
  uint8  power_digital_backoff[RFNV_TX_DSDA_DIGITAL_BACKOFF_GRANT_POWER_TBL_SIZE];
} rfnv_dsda_digital_backoff_grant_tbl_t;

typedef PACK (struct)
{
  uint8 num_of_grant_elements;
  uint64 ns_mask;
  uint8  ffh_mask;
  rfnv_dsda_digital_backoff_grant_tbl_t dsda_digital_backoff_grant_tbl_data[RFNV_TX_DSDA_DIGITAL_BACKOFF_GRANT_TBL_SIZE];
} rfnv_dsda_digital_backoff_semi_static_data_t;

/* RFNV_DATA_TYPE_TX_SAR_POWER_LIMIT_CONTROL RFNV variant typeid 216 to be used with Tx Static NV */
typedef PACK(struct)
{
  /*! NV Container Index for each Tx path for a given LTE band */
  uint8 nv_container_index;

  /*! Power class that this data item applies to.
      Implemented as a bitmask.
        Bit0 - Power Class 1
        Bit1 - Power Class 2
        Bit2 - Power Class 3
        ...
      If all Bits are enabled this backoff will be applied to all supported
      UE Power classes. */
  uint8 ue_power_class_mask;

  /*! TxPL Control structure */
  rfnv_tx_sar_power_limit_control_t tx_max_pl_ctrl;

} rfnv_tx_sar_power_limit_control_static_data_t;

/* RFNV_DATA_TYPE_TX_SAR_POWER_LIMIT_CONTROL_V2 RFNV variant typeid 409 to be used with Tx Static NV */
typedef PACK(struct)
{
  /*! NV Container Index for each Tx path for a given LTE band */
  uint8 nv_container_index;

  /*! Power class that this data item applies to.
      Implemented as a bitmask.
        Bit0 - Power Class 1
        Bit1 - Power Class 2
        Bit2 - Power Class 3
        ...
      If all Bits are enabled this backoff will be applied to all supported
      UE Power classes. */
  uint8 ue_power_class_mask;

  int16 sar_tx_power_limit_dB10 [RFNV_MAX_SAR_STATE];

} rfnv_tx_sar_power_limit_control_v2_static_data_t;

/* RFNV_DATA_TYPE_TX_SAR_POWER_LIMIT_CONTROL_V3 RFNV variant typeid 482 to be used with Tx Static NV */
typedef PACK(struct)
{
  /*! Power class that this data item applies to.
      Implemented as a bitmask.
        Bit0 - Power Class 1
        Bit1 - Power Class 2
        Bit2 - Power Class 3
        ...
      If all Bits are enabled this backoff will be applied to all supported
      UE Power classes. */
  uint16 ue_power_class_mask;

  int16 sar_tx_power_limit_dB10 [RFNV_MAX_SAR_STATE_V2];

} rfnv_tx_sar_power_limit_control_v3_static_data_t;

/* RFNV_DATA_TYPE_TX_SAR_ASDIV_POWER_LIMIT_CONTROL_V3 RFNV variant typeid 536 to be used with Tx Static NV */
typedef PACK(struct)
{
  /*! Power class that this data item applies to.
      Implemented as a bitmask.
        Bit0 - Power Class 1
        Bit1 - Power Class 2
        Bit2 - Power Class 3
        ...
      If all Bits are enabled this backoff will be applied to all supported
      UE Power classes. */
  uint16 ue_power_class_mask;

  int16 sar_tx_power_limit_dB10 [2][RFNV_MAX_SAR_STATE_V2];

} rfnv_tx_sar_asdiv_power_limit_control_v3_static_data_t;


/* RFNV_DATA_TYPE_TX_SAR_POWER_LIMIT_CONTROL_V4 RFNV variant to be used with Tx Static NV */
typedef PACK(struct)
{
  /* 0 means only used in non-ENDC mode ,
     1 means only used in ENDC mode,
     2 means used in both mode */
  uint8 endc_active;

  /* band number for the other tech in ENDC, 0xFFFF means any band is suitable
     Ignore it if current mode is non-ENDC */
  uint32 endc_band_number;

  /* sub band number for the other tech in ENDC, 0xFFFF means any subband band is suitable
     Ignore it if current mode is non-ENDC */
  uint8 endc_sub_band_number;

  /* mask of mimo chain(logical path), bit0 for PATH_TYPE_TX0, bit1 for PATH_TYPE_TX1 ...
     0x0 for non-MIMO case,
             i.e. single chain, PATH_TYPE_TX0 only or PATH_TYPE_TX1 only,
             in this case, mimo_chain will be ignored.
     0x1 for PATH_TYPE_TX0 only,
     0x2 for PATH_TYPE_TX1 only,
     0x3 for PATH_TYPE_TX0 + PATH_TYPE_TX1
     ...
     0xff means any chain mask is suitable */
  uint8 mimo_chain_mask;

  /* current mimo chain(logical path), 0 for PATH_TYPE_TX0, 1 for PATH_TYPE_TX1 ...
     0xFF means any chain is suitable.
     Ignore it if mimo chain mask is 0 */
  uint8 mimo_chain;

  /*! Power class that this data item applies to.
      Implemented as a bitmask.
        Bit0 - Power Class 1
        Bit1 - Power Class 2
        Bit2 - Power Class 3
        ...
      If all Bits are enabled this backoff will be applied to all supported
      UE Power classes. */
  uint16 ue_power_class_mask;

  int16 sar_tx_power_limit_dB10 [RFNV_MAX_SAR_STATE_V2];

} rfnv_tx_sar_power_limit_control_v4_static_data_t;

/* RFNV_DATA_TYPE_TX_SAR_POWER_LIMIT_CONTROL_V5 RFNV variant to be used with Tx Static NV */
typedef PACK(struct)
{
  /* 0 means only used in non-ENDC mode ,
     1 means only used in ENDC mode,
     2 means used in both mode */
  uint8 endc_active;

  /* band number for the other tech in ENDC, 0xFFFF means any band is suitable
     Ignore it if current mode is non-ENDC */
  uint32 endc_band_number;

  /* sub band number for the other tech in ENDC, 0xFFFF means any subband band is suitable
     Ignore it if current mode is non-ENDC */
  uint8 endc_sub_band_number;

  /* ant switch path for the other tech in ENDC, 0xFFFF means any ant switch path is suitable
     Ignore it if current mode is non-ENDC */
  uint16 endc_ant_switch_path;

  /* mask of mimo chain(logical path), bit0 for PATH_TYPE_TX0, bit1 for PATH_TYPE_TX1 ...
     0x0 for non-MIMO case, single PATH_TYPE_TX0 chain only. ignore mimo_chain in this case.
     0x1 for non-MIMO case, single PATH_TYPE_TX0 chain only. ignore mimo_chain in this case.
     0x2 not supported, dont use.
     0x3 for MIMO case, PATH_TYPE_TX0 + PATH_TYPE_TX1
     ...
     0xff means any chain mask is suitable */
  uint8 mimo_chain_mask;

  /* current mimo chain(logical path), 0 for PATH_TYPE_TX0, 1 for PATH_TYPE_TX1 ...
     0xFF means any chain is suitable.
     Ignore it if mimo chain mask is 0 */
  uint8 mimo_chain;

  /*! Power class that this data item applies to.
      Implemented as a bitmask.
        Bit0 - Power Class 1
        Bit1 - Power Class 2
        Bit2 - Power Class 3
        ...
      If all Bits are enabled this backoff will be applied to all supported
      UE Power classes. */
  uint16 ue_power_class_mask;

  int16 sar_tx_power_limit_dB10 [RFNV_MAX_SAR_STATE_V2];

} rfnv_tx_sar_power_limit_control_v5_static_data_t;

/* RFNV_DATA_TYPE_TX_SAR_POWER_LIMIT_CONTROL_V6 RFNV variant to be used with Tx Static NV */
typedef PACK(struct)
{
  /* 0 means only used in non-ENDC mode ,
     1 means only used in ENDC mode,
     2 means used in both mode */
  uint8 endc_active;

  /* band number for the other tech in ENDC, 0xFFFF means any band is suitable
     Ignore it if current mode is non-ENDC */
  uint32 endc_band_number;

  /* sub band number for the other tech in ENDC, 0xFFFF means any subband band is suitable
     Ignore it if current mode is non-ENDC */
  uint8 endc_sub_band_number;

  /* ant switch path for the other tech in ENDC, 0xFFFF means any ant switch path is suitable
     Ignore it if current mode is non-ENDC */
  uint16 endc_ant_switch_path;

  /* mask of mimo chain(logical path), bit0 for PATH_TYPE_TX0, bit1 for PATH_TYPE_TX1 ...
     0x0 for non-MIMO case,
             i.e. single chain, PATH_TYPE_TX0 only or PATH_TYPE_TX1 only,
             in this case, mimo_chain will be ignored.
     0x1 for PATH_TYPE_TX0 only,
     0x2 for PATH_TYPE_TX1 only,
     0x3 for PATH_TYPE_TX0 + PATH_TYPE_TX1
     ...
     0xff means any chain mask is suitable */
  uint8 mimo_chain_mask;

  /* current mimo chain(logical path), 0 for PATH_TYPE_TX0, 1 for PATH_TYPE_TX1 ...
     0xFF means any chain is suitable.
     Ignore it if mimo chain mask is 0 */
  uint8 mimo_chain;

  /*! Power class that this data item applies to.
      Implemented as a bitmask.
        Bit0 - Power Class 1
        Bit1 - Power Class 2
        Bit2 - Power Class 3
        ...
      If all Bits are enabled this backoff will be applied to all supported
      UE Power classes. */
  uint16 ue_power_class_mask;

  int16 sar_tx_power_limit_dB10 [RFNV_MAX_SAR_STATE_V3];

} rfnv_tx_sar_power_limit_control_v6_static_data_t;

/* RFNV_DATA_TYPE_TX_SAR_POWER_LIMIT_CONTROL_V7 RFNV variant typeid 525 to be used with Tx Static NV */
typedef PACK(struct)
{
  /*! Power class that this data item applies to.
      Implemented as a bitmask.
        Bit0 - Power Class 1
        Bit1 - Power Class 2
        Bit2 - Power Class 3
        ...
      If all Bits are enabled this backoff will be applied to all supported
      UE Power classes. */
  uint16 ue_power_class_mask;

  int16 sar_tx_power_limit_dB10 [RFNV_MAX_SAR_STATE_V3];

} rfnv_tx_sar_power_limit_control_v7_static_data_t;

/* RFNV_DATA_TYPE_TX_SAR_POWER_LIMIT_CONTROL_V8 RFNV variant to be used with Tx Static NV */
typedef PACK(struct)
{
  /* -------------------------------------------DSDA------------------------------------------- */
  /* 1 means sub1,
     2 means sub2,
     0xFF means each sub is suitable*/
  uint8 sub_id;

  /* -------------------------------------------CA------------------------------------------- */
  /* cc_id of current band, not the other band in CA
     0 means current band is cc0 or PCC,
     1 means current band is cc1 or SCC,
     0xFF means suitable for all CCs*/
  uint8 cc_id;

  /* 0 means only used in non-CA mode,
     1 means only used in CA mode,
     2 means used in both mode */
  uint8 ca_active;

  /* band number for the other cc in CA, 0xFFFF means any band is suitable
     Ignore it if current mode is non-CA */
  uint32 ca_band_number;

  /* sub band number for the other cc in CA, 0xFF means any subband band is suitable
     Ignore it if current mode is non-CA */
  uint8 ca_sub_band_number;

  /* signal path for the other cc in CA, 0xFFFF means any signal path is suitable
     Ignore it if current mode is non-CA */
  uint16 ca_sig_path;

  /* ant switch path for the other cc in CA, 0xFFFF means any ant switch path is suitable
     Ignore it if current mode is non-CA */
  uint16 ca_ant_switch_path;

  /* -------------------------------------------ENDC------------------------------------------- */
  /* 0 means only used in non-ENDC mode ,
     1 means only used in ENDC mode,
     2 means used in both mode */
  uint8 endc_active;

  /* band number for the other tech in ENDC, 0xFFFF means any band is suitable
     Ignore it if current mode is non-ENDC */
  uint32 endc_band_number;

  /* sub band number for the other tech in ENDC, 0xFFFF means any subband band is suitable
     Ignore it if current mode is non-ENDC */
  uint8 endc_sub_band_number;

  /* ant switch path for the other tech in ENDC, 0xFFFF means any ant switch path is suitable
     Ignore it if current mode is non-ENDC */
  uint16 endc_ant_switch_path;

  /* -------------------------------------------MIMO------------------------------------------- */
  /* mask of mimo chain(logical path), bit0 for PATH_TYPE_TX0, bit1 for PATH_TYPE_TX1 ...
     0x0 for non-MIMO case, single PATH_TYPE_TX0 chain only. ignore mimo_chain in this case.
     0x1 for non-MIMO case, single PATH_TYPE_TX0 chain only. ignore mimo_chain in this case.
     0x2 not supported, dont use.
     0x3 for MIMO case, PATH_TYPE_TX0 + PATH_TYPE_TX1
     ...
     0xff means any chain mask is suitable */
  uint8 mimo_chain_mask;

  /* current mimo chain(logical path), 0 for PATH_TYPE_TX0, 1 for PATH_TYPE_TX1 ...
     0xFF means any chain is suitable.
     Ignore it if mimo chain mask is 0 */
  uint8 mimo_chain;

  /* -------------------------------------------Power Class------------------------------------------- */
  /*! Power class that this data item applies to.
      Implemented as a bitmask.
        Bit0 - Power Class 1
        Bit1 - Power Class 2
        Bit2 - Power Class 3
        ...
      If all Bits are enabled this backoff will be applied to all supported
      UE Power classes. */
  uint16 ue_power_class_mask;

  /* -------------------------------------------Freq Range------------------------------------------- */
  /*! min tx freq in KHz. set 0 if bound not available */
  uint32 low_freq_bound_khz;
  /*! max tx freq in KHz. set 0 if bound not available */
  uint32 high_freq_bound_khz;

  /* -------------------------------------------SAR Table------------------------------------------- */
  int16 sar_tx_power_limit_dB10 [RFNV_MAX_SAR_STATE_V2];

}rfnv_tx_sar_power_limit_control_v8_static_data_t;

/* RFNV_DATA_TYPE_TX_SAR_POWER_LIMIT_CONTROL_V10 RFNV variant to be used with Tx Static NV */
typedef PACK(struct)
{
  /* -------------------------------------------DSDA------------------------------------------- */
  /* 1 means sub1,
     2 means sub2,
     0xFF means each sub is suitable*/
  uint8 sub_id;

  /* -------------------------------------------CA------------------------------------------- */
  /* cc_id of current band, not the other band in CA
     0 means current band is cc0 or PCC,
     1 means current band is cc1 or SCC,
     0xFF means suitable for all CCs*/
  uint8 cc_id;

  /* 0 means only used in non-CA mode,
     1 means only used in CA mode,
     2 means used in both mode */
  uint8 ca_active;

  /* band number for the other cc in CA, 0xFFFF means any band is suitable
     Ignore it if current mode is non-CA */
  uint32 ca_band_number;

  /* sub band number for the other cc in CA, 0xFF means any subband band is suitable
     Ignore it if current mode is non-CA */
  uint8 ca_sub_band_number;

  /* signal path for the other cc in CA, 0xFFFF means any signal path is suitable
     Ignore it if current mode is non-CA */
  uint16 ca_sig_path;

  /* ant switch path for the other cc in CA, 0xFFFF means any ant switch path is suitable
     Ignore it if current mode is non-CA */
  uint16 ca_ant_switch_path;

  /* -------------------------------------------ENDC------------------------------------------- */
  /* 0 means only used in non-ENDC mode ,
     1 means only used in ENDC mode,
     2 means used in both mode */
  uint8 endc_active;

  /* band number for the other tech in ENDC, 0xFFFF means any band is suitable
     Ignore it if current mode is non-ENDC */
  uint32 endc_band_number;

  /* sub band number for the other tech in ENDC, 0xFFFF means any subband band is suitable
     Ignore it if current mode is non-ENDC */
  uint8 endc_sub_band_number;

  /* ant switch path for the other tech in ENDC, 0xFFFF means any ant switch path is suitable
     Ignore it if current mode is non-ENDC */
  uint16 endc_ant_switch_path;

  /* -------------------------------------------MIMO------------------------------------------- */
  /* mask of mimo chain(logical path), bit0 for PATH_TYPE_TX0, bit1 for PATH_TYPE_TX1 ...
     0x0 for non-MIMO case, single PATH_TYPE_TX0 chain only. ignore mimo_chain in this case.
     0x1 for non-MIMO case, single PATH_TYPE_TX0 chain only. ignore mimo_chain in this case.
     0x2 not supported, dont use.
     0x3 for MIMO case, PATH_TYPE_TX0 + PATH_TYPE_TX1
     ...
     0xff means any chain mask is suitable */
  uint8 mimo_chain_mask;

  /* current mimo chain(logical path), 0 for PATH_TYPE_TX0, 1 for PATH_TYPE_TX1 ...
     0xFF means any chain is suitable.
     Ignore it if mimo chain mask is 0 */
  uint8 mimo_chain;

  /* -------------------------------------------Power Class------------------------------------------- */
  /*! Power class that this data item applies to.
      Implemented as a bitmask.
        Bit0 - Power Class 1
        Bit1 - Power Class 2
        Bit2 - Power Class 3
        ...
      If all Bits are enabled this backoff will be applied to all supported
      UE Power classes. */
  uint16 ue_power_class_mask;

  /* -------------------------------------------Freq Range------------------------------------------- */
  /*! min tx freq in KHz. set 0 if bound not available */
  uint32 low_freq_bound_khz;
  /*! max tx freq in KHz. set 0 if bound not available */
  uint32 high_freq_bound_khz;

  /* -------------------------------------------SAR Table------------------------------------------- */
  int16 sar_tx_power_limit_dB10 [RFNV_MAX_SAR_STATE_V3];

}rfnv_tx_sar_power_limit_control_v10_static_data_t;


typedef PACK(struct)
{
    /* ADD sub id incaseof L++L DSDA
       1 means sub1,
       2 means sub2,
     0xFF means each sub is suitable*/
  //uint8 sub_id;
  /* 0 means only used in standalone mode ,
     1 means only used in ENDC mode,
     2 means only used in DSDA mode
     all bit means used in both mode */
  uint8 active_mask;

  /* band number for the other tech in ENDC/DSDA, 0xFFFF means any band is suitable
     Ignore it if current mode is non-ENDC/DSDA */
  uint32 band_number;

  /* sub band number for the other tech in ENDC/DSDA, 0xFFFF means any subband band is suitable
     Ignore it if current mode is non-ENDC/DSDA */
  uint8 sub_band_number;

  /* ant switch path for the other tech in ENDC/DSDA, 0xFFFF means any ant switch path is suitable
     Ignore it if current mode is non-ENDC/DSDA */
  uint16 ant_switch_path;

  /* mask of mimo chain(logical path), bit0 for PATH_TYPE_TX0, bit1 for PATH_TYPE_TX1 ...
     0x0 for non-MIMO case, single PATH_TYPE_TX0 chain only. ignore mimo_chain in this case.
     0x1 for non-MIMO case, single PATH_TYPE_TX0 chain only. ignore mimo_chain in this case.
     0x2 not supported, dont use.
     0x3 for MIMO case, PATH_TYPE_TX0 + PATH_TYPE_TX1
     ...
     0xff means any chain mask is suitable */
  uint8 mimo_chain_mask;

  /* current mimo chain(logical path), 0 for PATH_TYPE_TX0, 1 for PATH_TYPE_TX1 ...
     0xFF means any chain is suitable.
     Ignore it if mimo chain mask is 0 */
  uint8 mimo_chain;

  /*! Power class that this data item applies to.
      Implemented as a bitmask.
        Bit0 - Power Class 1
        Bit1 - Power Class 2
        Bit2 - Power Class 3
        ...
      If all Bits are enabled this backoff will be applied to all supported
      UE Power classes. */
  uint16 ue_power_class_mask;

  int16 sar_tx_power_limit_dB10 [RFNV_MAX_SAR_STATE_V2];

} rfnv_tx_sar_power_limit_control_v9_static_data_t;

typedef PACK(struct)
{
  /* 0 means only used in standalone mode ,
     1 means only used in ENDC mode,
     2 means only used in DSDA mode
     all bit means used in both mode */
  uint8 active_mask;

  /* band number for the other tech in ENDC/DSDA, 0xFFFF means any band is suitable
     Ignore it if current mode is non-ENDC/DSDA */
  uint32 band_number;

  /* sub band number for the other tech in ENDC/DSDA, 0xFFFF means any subband band is suitable
     Ignore it if current mode is non-ENDC/DSDA */
  uint8 sub_band_number;

  /* ant switch path for the other tech in ENDC/DSDA, 0xFFFF means any ant switch path is suitable
     Ignore it if current mode is non-ENDC/DSDA */
  uint16 ant_switch_path;

  /* mask of mimo chain(logical path), bit0 for PATH_TYPE_TX0, bit1 for PATH_TYPE_TX1 ...
     0x0 for non-MIMO case, single PATH_TYPE_TX0 chain only. ignore mimo_chain in this case.
     0x1 for non-MIMO case, single PATH_TYPE_TX0 chain only. ignore mimo_chain in this case.
     0x2 not supported, dont use.
     0x3 for MIMO case, PATH_TYPE_TX0 + PATH_TYPE_TX1
     ...
     0xff means any chain mask is suitable */
  uint8 mimo_chain_mask;

  /* current mimo chain(logical path), 0 for PATH_TYPE_TX0, 1 for PATH_TYPE_TX1 ...
     0xFF means any chain is suitable.
     Ignore it if mimo chain mask is 0 */
  uint8 mimo_chain;

  /*! Power class that this data item applies to.
      Implemented as a bitmask.
        Bit0 - Power Class 1
        Bit1 - Power Class 2
        Bit2 - Power Class 3
        ...
      If all Bits are enabled this backoff will be applied to all supported
      UE Power classes. */
  uint16 ue_power_class_mask;

  int16 sar_tx_power_limit_dB10 [RFNV_MAX_SAR_STATE_V3];

} rfnv_tx_sar_power_limit_control_v11_static_data_t;

/* RFNV_DATA_TYPE_ENDC_MTPL_ADJ RFNV variant to be used with Tx Static NV */
typedef PACK(struct)
{
  /* 0 means only used in non-ENDC mode ,
     1 means only used in ENDC mode,
     2 means used in both mode */
  uint8 endc_active;

  /* band number for the other tech in ENDC, 0xFFFF means any band is suitable
     Ignore it if current mode is non-ENDC */
  uint32 endc_band_number;

  /* sub band number for the other tech in ENDC, 0xFF means any subband band is suitable
     Ignore it if current mode is non-ENDC */
  uint8 endc_sub_band_number;

  /*! Power class that this data item applies to.
      Implemented as a bitmask.
        Bit0 - Power Class 1
        Bit1 - Power Class 2
        Bit2 - Power Class 3
        ...
      If all Bits are enabled this backoff will be applied to all supported
      UE Power classes. */
  uint16 ue_power_class_mask;

  /* NR MTPL adjust value: in dB10 */
  int16 nr_mtpl_adj_db10;

  /* ENDC MTPL adjust value: in dB10 */
  int16 endc_mtpl_adj_db10;

} rfnv_tx_endc_mtpl_adj_static_data_t;

/* RFNV_DATA_TYPE_ENDC_MTPL_ADJ_V2 RFNV variant to be used with Tx Static NV */
typedef PACK(struct)
{
  /* 0 means only used in non-ENDC mode ,
     1 means only used in ENDC mode,
     2 means used in both mode */
  uint8 endc_active;

  /* band number for the other tech in ENDC, 0xFFFF means any band is suitable
     Ignore it if current mode is non-ENDC */
  uint32 endc_band_number;

  /* sub band number for the other tech in ENDC, 0xFF means any subband band is suitable
     Ignore it if current mode is non-ENDC */
  uint8 endc_sub_band_number;

  /*! Power class that this data item applies to.
      Implemented as a bitmask.
        Bit0 - Power Class 1
        Bit1 - Power Class 2
        Bit2 - Power Class 3
        ...
      If all Bits are enabled this backoff will be applied to all supported
      UE Power classes. */
  uint16 ue_power_class_mask;

  /*! Channel type that this data item applies to.
        Implemented as a bitmask.
        Before DE.5.0
            Bit0 - channel SRS
            Bit1 - channel others
            If all Bits are set this backoff will be applied to all channel
            also bitmask = 0 means all channel are active.

        For DE.5.0 and post DE.5.0:
              Bit0 - channel SRS
              Bit1 - channel others
              Bit8 - flag indicates SRS MTPL adj only for 2Tx SRS
                     (bit8 and bit0 needs to be set both if true, xxxxxxx1 xxxxxxx1)
              Set all bits means given NV entry applicable to all channels (for SRS, only applied in 2T SRS);
              Set chan_mask = 0 means given NV entry applicable to all channels (for SRS, both 1T and 2T SRS);
  */
  uint16 chan_mask;

  /*! Ant switch path when channel type is SRS
        0xFFFF means any ant switch path is suitable. */
  uint16 srs_ant_switch_path;

  /* NR MTPL adjust value: in dB10 */
  int16 nr_mtpl_adj_db10;

  /* ENDC MTPL adjust value: in dB10 */
  int16 endc_mtpl_adj_db10;

  /* Additional NR MTPL adjust value in ULMIMO SMDL mode: in dB10
      mtpl adj should be the sum of nr_mtpl_adj and nr_smdl_additional_mtpl_adj */
  int16 nr_smdl_additional_mtpl_adj_db10;

}rfnv_tx_endc_mtpl_adj_static_data_v2_t;

/* RFNV_DATA_TYPE_COMBO_MTPL_ADJ 569*/
typedef PACK(struct)
{
  /* -------------------------------------------sub id and carrier id------------------------------------------- */
  /* 1 means sub1,
     2 means sub2,
     0xFF means each sub is suitable*/
  uint8 sub_id;

  /* cc_id of current band, not the other band in CA
     0 means current band is cc0 or PCC,
     1 means current band is cc1 or SCC,
     0xFF means suitable for all CCs*/
  uint8 cc_id;

  /* -------------------------------------------intra tech ca/dsda ------------------------------------------- */
  /* Bit mask of intra-tech combo type
    INTRA_TECH_CC_SKIP_CHECKING     = 0x0, // skip checking
    INTRA_TECH_CC_MASK_NO_COMBO     = 0x1, // bit 0 no combo
    INTRA_TECH_CC_MASK_ULCA         = 0x2, // bit 1 ulca mode
    INTRA_TECH_CC_MASK_NR_NR_DSDA   = 0x4  // bit 2 nr-nr dsda mode or lte-lte dsda mode */
  uint8 ca_active;

  /* band number for the other cc in CA, 0xFFFF means any band is suitable
     Ignore it if current mode is non-CA */
  uint32 ca_band_number;

  /* sub band number for the other cc in CA, 0xFF means any subband band is suitable
     Ignore it if current mode is non-CA */
  uint8 ca_sub_band_number;

  /* signal path for the other cc in CA, 0xFFFF means any signal path is suitable
     Ignore it if current mode is non-CA */
  uint16 ca_sig_path;

  /* ant switch path for the other cc in CA, 0xFFFF means any ant switch path is suitable
     Ignore it if current mode is non-CA */
  uint16 ca_ant_switch_path;

  /* -------------------------------------------inter-tech ca/dsda ------------------------------------------- */

  /* Bit mask of inter-tech combo type
     INTER_TECH_SKIP_CHECKING     = 0x0, // skip checking
     INTER _TECH_MASK_NO_COMBO     = 0x1, // bit 0 no combo
     INTER _TECH_MASK_ENDC         = 0x2, // bit 1 endc mode
     INTER _TECH_MASK_NR_LTE_DSDA  = 0x4  // bit 2 nr-lte dsda mode */
  uint8 endc_active;

  /* band number for the other tech in ENDC, 0xFFFF means any band is suitable
     Ignore it if current mode is non-ENDC */
  uint32 endc_band_number;

  /* sub band number for the other tech in ENDC, 0xFFFF means any subband band is suitable
     Ignore it if current mode is non-ENDC */
  uint8 endc_sub_band_number;

  /* ant switch path for the other tech in ENDC, 0xFFFF means any ant switch path is suitable
     Ignore it if current mode is non-ENDC */
  uint16 endc_ant_switch_path;

  /* -------------------------------------------power class and chan mask  ------------------------------------------- */
  /*! Power class that this data item applies to.
      Implemented as a bitmask.
        Bit0 - Power Class 1
        Bit1 - Power Class 2
        Bit2 - Power Class 3
        ...
      If all Bits are enabled this backoff will be applied to all supported
      UE Power classes. */
  uint16 ue_power_class_mask;

  /*! Channel type that this data item applies to.
        Implemented as a bitmask.
        Bit0 - channel SRS
        Bit1 - channel others
        If all Bits are set this backoff will be applied to all channel
        also bitmask = 0 means all channel are active. */
  uint16 chan_mask;

  /*! Ant switch path when channel type is SRS
        0xFFFF means any ant switch path is suitable. */
  uint16 srs_ant_switch_path;

  /* -------------------------------------------combo mtpl adjust  ------------------------------------------- */
  /*! combo mtpl adjust in db10 */
  int16 combo_mtpl_adj_db10;

} rfnv_tx_combo_mtpl_adj_static_data_t;

/* RFNV_DATA_TYPE_ASDIV_IMBALANCE_ADJ 592 */
typedef PACK(struct)
{

  /*  Store antenna imbalance value per band/asp */
  int16 imbalance_offset[RFNV_ASDIV_IMBALANCE_OFFSET_SIZE];

  /*  Physical antenna bit mask */
  uint16 physic_ant_bitmask;

}rfnv_asdiv_imbalance_data_t;

/* RFNV_DATA_TYPE_COMBO_MTPL_ADJ_V2 578*/
typedef PACK(struct)
{
  /* -------------------------------------------sub id and carrier id------------------------------------------- */
  /* 1 means sub1,
     2 means sub2,
     0xFF means each sub is suitable*/
  uint8 sub_id;

  /* cc_id of current band, not the other band in CA
     0 means current band is cc0 or PCC,
     1 means current band is cc1 or SCC,
     0xFF means suitable for all CCs*/
  uint8 cc_id;

  /* -------------------------------------------intra tech ca/dsda ------------------------------------------- */
  /* Bit mask of intra-tech combo type
    INTRA_TECH_CC_SKIP_CHECKING     = 0x0, // skip checking
    INTRA_TECH_CC_MASK_NO_COMBO     = 0x1, // bit 0 no combo
    INTRA_TECH_CC_MASK_ULCA         = 0x2, // bit 1 ulca mode
    INTRA_TECH_CC_MASK_NR_NR_DSDA   = 0x4  // bit 2 nr-nr dsda mode or lte-lte dsda mode */
  uint8 ca_active;

  /* band number for the other cc in CA, 0xFFFF means any band is suitable
     Ignore it if current mode is non-CA */
  uint32 ca_band_number;

  /* sub band number for the other cc in CA, 0xFF means any subband band is suitable
     Ignore it if current mode is non-CA */
  uint8 ca_sub_band_number;

  /* signal path for the other cc in CA, 0xFFFF means any signal path is suitable
     Ignore it if current mode is non-CA */
  uint16 ca_sig_path;

  /* ant switch path for the other cc in CA, 0xFFFF means any ant switch path is suitable
     Ignore it if current mode is non-CA */
  uint16 ca_ant_switch_path;

  /* -------------------------------------------inter-tech ca/dsda ------------------------------------------- */

  /* Bit mask of inter-tech combo type
     INTER_TECH_SKIP_CHECKING     = 0x0, // skip checking
     INTER_TECH_MASK_NO_COMBO     = 0x1, // bit 0 no combo
     INTER_TECH_MASK_ENDC         = 0x2, // bit 1 endc mode
     INTER_TECH_MASK_NR_LTE_DSDA  = 0x4  // bit 2 nr-lte dsda mode */
  uint8 endc_active;

  /* band number for the other tech in ENDC, 0xFFFF means any band is suitable
     Ignore it if current mode is non-ENDC */
  uint32 endc_band_number;

  /* sub band number for the other tech in ENDC, 0xFFFF means any subband band is suitable
     Ignore it if current mode is non-ENDC */
  uint8 endc_sub_band_number;

  /* ant switch path for the other tech in ENDC, 0xFFFF means any ant switch path is suitable
     Ignore it if current mode is non-ENDC */
  uint16 endc_ant_switch_path;

  /* -------------------------------------------power class and chan mask  ------------------------------------------- */
  /*! Power class that this data item applies to.
      Implemented as a bitmask.
        Bit0 - Power Class 1
        Bit1 - Power Class 2
        Bit2 - Power Class 3
        ...
      If all Bits are enabled this backoff will be applied to all supported
      UE Power classes. */
  uint16 ue_power_class_mask;

  /*! Channel type that this data item applies to.
        Implemented as a bitmask.
        Bit0 - channel SRS
        Bit1 - channel others
        If all Bits are set this backoff will be applied to all channel
        also bitmask = 0 means all channel are active. */
  uint16 chan_mask;

  /*! Ant switch path when channel type is SRS
        0xFFFF means any ant switch path is suitable. */
  uint16 srs_ant_switch_path;

  /* -------------------------------------------combo mtpl adjust  ------------------------------------------- */
  /*! combo mtpl adjust in db10 */
  int16 combo_mtpl_adj_db10;

  /* -------------------------------------------pemax adjust  ------------------------------------------- */
  /*! 1. set ca_active = 0x1, endc_active = 0x0, cc_id = 0,
            pemax_cc_adj_db10 for give cc will be applied in single cc scenario
      2. set ca_active = 0x2, endc_active = 0x0, cc_id = 0,
            pemax_cc_adj_db10 for cc0 and pemax_ca_adj_db10 for ULCA combo will be applied in ULCA scenario
         set ca_active = 0x2, endc_active = 0x0, cc_id = 1,
            pemax_cc_adj_db10 for cc1 will be applied in ULCA scenario
      3. set ca_active = 0x0, endc_active = 0x0,
            pemax_cc_adj_db10 and potential pemax_ca_adj_db10 will be applied according to real scenario */
  /*! pemax adjust for single cc in db10 */
  int16 pemax_cc_adj_db10;
  /*! pemax adjust for ca combo in db10 */
  int16 pemax_ca_adj_db10;

} rfnv_tx_combo_mtpl_adj_static_data_t_v2;

/* RFNV_DATA_TYPE_COEX_EXTRACTOR_PATHLOSS */
typedef PACK(struct)
{
  /*! min tx freq in KHz. set -1 as wildcard */
  int32 min_freq_khz;

  /*! max tx freq in KHz. set -1 as wildcard */
  int32 max_freq_khz;

  /*! pathloss in dB10 */
  int16 pathloss_db10;

}rfnv_tx_coex_extractor_pathloss_static_data_t;

typedef PACK(struct)
{
  /* HDET HPM LPM SWITCHPOINT : RFNV_LTE_Bxx_HDET_HPM_LPM_SWITCH_POINT_I */
  int16 hpm_lpm_swpt_dB10;

  /* HDET THRESHOLD: RFNV_LTE_Bxx_HDET_HPM_THRESHOLD_I: in dB10 */
  int16 threshold_dB10;

  /* HDET OFFSET and SPAN: RFNV_LTE_Bxx_HDET_OFF_SPAN_I */
  uint16 offset_span[RFNV_HDET_OFFSET_SPAN_IND];

  /* HDET 2D compensation: RFNV_LTE_Bxx_TX_LIM_VS_TEMP_VS_FREQ_I */
  int8 lim_vs_temp_vs_freq[RFNV_LTE_TX_CHAN_SIZE][RFNV_TEMP_TABLE_SIZ];

}rfnv_tx_hdet_control_t;

/* RFNV_DATA_TYPE_TX_HDET_CONTROL RFNV variant typeid 209 to be used with Tx Static NV */
typedef PACK(struct)
{
  /*! NV Container Index for each Tx path for a given LTE band */
  uint8 nv_container_index;

  /*! Reserved for now, not used. */
  uint8 reserved;

  /*! Tx HDET Control structure */
  rfnv_tx_hdet_control_t tx_hdet_ctrl;

} rfnv_tx_hdet_control_static_data_t;

/* RFNV_LTE_Cx_Bxx_PA_STATIC_Z_I is getting broken into more sub-sections for more flexibility */

typedef PACK(struct)
{
  /* Number of active PA states */
  uint8 number_of_active_pa_states;

  /* PA Range Map NV */
  int16 pa_range_map[RFNV_MAX_PA_STATE];

  /* PA Hysterisis in us  */
  int16 pa_hysterisis_us;

  /* PA Fall switchpoint: 0-> Reserved | 1-> R1 fall | 2 -> R2 fall etc */
  int16 pa_fall_sw[RFNV_MAX_PA_STATE];

  /* PA Rise switchpoint: 0-> reserved | 1-> R1 rise | 2 -> R2 rise etc */
  int16 pa_rise_sw[RFNV_MAX_PA_STATE];

   /* MPR based PA Fall switchpoint Backoff to the actual Switchpoints:
   0->    Reserved | 1-> R1-O1 fall | 2 -> R2-O2 fall etc */
  int16 mpr_based_pa_fall_sw_backoff[RFNV_MAX_PA_STATE];

  /* MPR based PA Rise switchpoint Backoff to the actual Swithpoints:
  0-> reserved | 1-> R1-O1 rise | 2 -> R2-O2 rise etc */
  int16 mpr_based_pa_rise_sw_backoff[RFNV_MAX_PA_STATE];

  /* Tx Rotator Angle - per PA State */
  int16 tx_rot_angle[RFNV_MAX_PA_STATE];

}rfnv_txagc_control_t;

typedef PACK(struct)
{
  /* Number of active PA states */
  uint8 number_of_active_pa_states;

  /* PA Range Map NV */
  int16 pa_range_map[RFNV_MAX_PA_STATE];

  /* PA Hysterisis in us  */
  int16 pa_hysterisis_us;

  /* PA Fall switchpoint: 0-> Reserved | 1-> R1 fall | 2 -> R2 fall etc */
  int16 pa_fall_sw[RFNV_MAX_PA_STATE];

  /* PA Rise switchpoint: 0-> reserved | 1-> R1 rise | 2 -> R2 rise etc */
  int16 pa_rise_sw[RFNV_MAX_PA_STATE];

   /* MPR based PA Fall switchpoint Backoff to the actual Switchpoints:
   0->    Reserved | 1-> R1-O1 fall | 2 -> R2-O2 fall etc */
  int16 mpr_based_pa_fall_sw_backoff[RFNV_MAX_PA_STATE];

  /* MPR based PA Rise switchpoint Backoff to the actual Swithpoints:
  0-> reserved | 1-> R1-O1 rise | 2 -> R2-O2 rise etc */
  int16 mpr_based_pa_rise_sw_backoff[RFNV_MAX_PA_STATE];

  /* Tx Rotator Angle - per PA State */
  int16 tx_rot_angle[RFNV_MAX_PA_STATE];

    /* PA Fall switchpoint: 0-> Reserved | 1-> R1 fall | 2 -> R2 fall etc */
  int16 pa_fall_sw_ns[RFNV_MAX_PA_STATE];

  /* PA Rise switchpoint: 0-> reserved | 1-> R1 rise | 2 -> R2 rise etc */
  int16 pa_rise_sw_ns[RFNV_MAX_PA_STATE];

     /* MPR based PA Fall switchpoint Backoff to the actual Switchpoints:
   0->    Reserved | 1-> R1-O1 fall | 2 -> R2-O2 fall etc */
  int16 mpr_based_pa_fall_sw_backoff_ns[RFNV_MAX_PA_STATE];

  /* MPR based PA Rise switchpoint Backoff to the actual Swithpoints:
  0-> reserved | 1-> R1-O1 rise | 2 -> R2-O2 rise etc */
  int16 mpr_based_pa_rise_sw_backoff_ns[RFNV_MAX_PA_STATE];

}rfnv_txagc_control_t_v2;

typedef PACK(struct)
{
    /* Number of active PA states */
  uint8 number_of_active_pa_states;

  /* PA Range Map NV */
  int16 pa_range_map[RFNV_MAX_PA_STATE];

  /* PA Hysterisis in us  */
  int16 pa_hysterisis_us;

  /* PA Fall switchpoint: 0-> Reserved | 1-> R1 fall | 2 -> R2 fall etc */
  int16 pa_fall_sw[RFNV_MAX_PA_STATE];

  /* PA Rise switchpoint: 0-> reserved | 1-> R1 rise | 2 -> R2 rise etc */
  int16 pa_rise_sw[RFNV_MAX_PA_STATE];

   /* MPR based PA Fall switchpoint Backoff to the actual Switchpoints:
   0->    Reserved | 1-> R1-O1 fall | 2 -> R2-O2 fall etc */
  int16 mpr_based_pa_fall_sw_backoff[RFNV_MAX_PA_STATE];

  /* MPR based PA Rise switchpoint Backoff to the actual Swithpoints:
  0-> reserved | 1-> R1-O1 rise | 2 -> R2-O2 rise etc */
  int16 mpr_based_pa_rise_sw_backoff[RFNV_MAX_PA_STATE];

  /* Tx Rotator Angle - per PA State */
  int16 tx_rot_angle[RFNV_MAX_PA_STATE];

  /* PA Fall switchpoint: 0-> Reserved | 1-> R1 fall | 2 -> R2 fall etc */
  int16 pa_fall_sw_ns[RFNV_MAX_PA_STATE];

  /* PA Rise switchpoint: 0-> reserved | 1-> R1 rise | 2 -> R2 rise etc */
  int16 pa_rise_sw_ns[RFNV_MAX_PA_STATE];

     /* MPR based PA Fall switchpoint Backoff to the actual Switchpoints:
   0->    Reserved | 1-> R1-O1 fall | 2 -> R2-O2 fall etc */
  int16 mpr_based_pa_fall_sw_backoff_ns[RFNV_MAX_PA_STATE];

  /* MPR based PA Rise switchpoint Backoff to the actual Swithpoints:
  0-> reserved | 1-> R1-O1 rise | 2 -> R2-O2 rise etc */
  int16 mpr_based_pa_rise_sw_backoff_ns[RFNV_MAX_PA_STATE];

  /* PA Fall switchpoint: 0-> Reserved | 1-> R1 fall | 2 -> R2 fall etc
     To be used only for SRS waveform*/
  int16 srs_specific_pa_fall_sw[RFNV_MAX_PA_STATE];

  /* PA Rise switchpoint: 0-> reserved | 1-> R1 rise | 2 -> R2 rise etc
     To be used only for SRS waveform*/
  int16 srs_specific_pa_rise_sw[RFNV_MAX_PA_STATE];
}rfnv_txagc_control_t_v3;

/* Switchpoint_Backoff_Max_MPR_NV RFNV variant typeid 477 */
typedef PACK(struct)
{
  /*Aggregated channel bandwidth mask for mpr backoff region */
  /* bit 0 - class A (NA), bit 1 - class B, bit 2 - class C, and so on */
  uint8 bandwidth_class;

  /*LTE Modulation type  1->QPSK/2->16QAM/4->256QAM etc*/
  uint8 modulation_idx;

  /* switchpoint_adjust_idx  0 -> switchpoint_backoff_mpr = 0:
                             1 -> switchpoint_backoff_mpr = switchpoint_backoff_max_mpr*/
  uint8 switchpoint_adjust_idx;

  /* Max mpr switchpoint Backoff to the actual Switchpoints */
  int16 switchpoint_backoff_max_mpr;

}rfnv_pa_swp_max_mpr_control_t;

/* RFNV_DATA_TYPE_TXAGC_CONTROL RFNV variant typeid 217 to be used with Tx Static NV */
typedef PACK(struct)
{

  /*! NV Container Index for each Tx path for a given LTE band */
  uint8 nv_container_index;

  /*!Carrier config : 0 - Single Carrier, 1 - Dual Carrier */
  uint8 carrier_config;

  /*Aggregated channel bandwidth mask for mpr backoff region */
  /* bit 0 - class A (NA), bit 1 - class B, bit 2 - class C, and so on */
  uint32 bandwidth_class;

  /*! Identifies all the Bandwidths supported for this MPR selection */
  uint32 bandwidth_type_mask;

  /* CA-Network Signalling Value */
  uint32 ca_ns_number;

  /*! TxAGC Control structure */
  rfnv_txagc_control_t txagc_ctrl;

} rfnv_txagc_control_static_data_t;

/* RFNV_DATA_TYPE_TXAGC_CONTROL_V2 RFNV variant typeid 478 to be used with Tx Static NV */
typedef PACK(struct)
{
  /*LTE Modulation type  1->QPSK/2->16QAM/4->256QAM etc*/
  uint8 modulation_idx;

  /*! NV Container Index for each Tx path for a given LTE band */
  uint8 nv_container_index;

  /*!Carrier config : 0 - Single Carrier, 1 - Dual Carrier */
  uint8 carrier_config;

  /*Aggregated channel bandwidth mask for mpr backoff region */
  /* bit 0 - class A (NA), bit 1 - class B, bit 2 - class C, and so on */
  uint32 bandwidth_class;

  /*! Identifies all the Bandwidths supported for this MPR selection */
  uint32 bandwidth_type_mask;

  /* CA-Network Signalling Value */
  uint32 ca_ns_number;

  /*! TxAGC Control structure */
  rfnv_txagc_control_t_v2 txagc_ctrl;

} rfnv_txagc_control_static_data_t_v2;

/* RFNV_DATA_TYPE_TXAGC_CONTROL_V3 RFNV variant typeid 552 to be used with Tx Static NV */
typedef PACK(struct)
{
  /*LTE Modulation type  1->QPSK/2->16QAM/4->256QAM etc*/
  uint8 modulation_idx;

  /*! NV Container Index for each Tx path for a given LTE band */
  uint8 nv_container_index;

  /*!Carrier config : 0 - Single Carrier, 1 - Dual Carrier */
  uint8 carrier_config;

  /*! Aggregated channel bandwidth mask for mpr backoff region */
  /*! bit 0 - class A (NA), bit 1 - class B, bit 2 - class C, and so on */
  uint32 bandwidth_class;

  /*! Identifies all the Bandwidths supported for this MPR selection */
  uint32 bandwidth_type_mask;

  /*! CA-Network Signalling Value */
  uint32 ca_ns_number;

  /*! TxAGC Control structure */
  rfnv_txagc_control_t_v3 txagc_ctrl;

} rfnv_txagc_control_static_data_t_v3;

/* RFNV_DATA_TYPE_TXAGC_CONTROL_V4 RFNV variant typeid 593 to be used with Tx Static NV */
typedef PACK(struct)
{
  /*LTE Modulation type  1->QPSK/2->16QAM/4->256QAM etc*/
  uint8 modulation_idx;

  uint8 nv_container_index;

  uint8 carrier_config;

  /*! Aggregated channel bandwidth mask for mpr backoff region */
  /*! bit 0 - class A (NA), bit 1 - class B, bit 2 - class C, and so on */
  uint32 bandwidth_class;

  /*! Identifies all the Bandwidths supported for this MPR selection */
  uint32 bandwidth_type_mask;

  /*! CA-Network Signalling Value */
  uint32 ca_ns_number;
  
  /*! Set bit 0 for data and bit 1 for voice*/
  uint8 active_txagc_control_bitmask;

  /*! TxAGC Control structure */
  /*! 0 - data (default), 1 - voice, 2&3 reserved */
  rfnv_txagc_control_t_v3 txagc_ctrl[RFNV_TXAGC_CTRL_STRUCT_SIZE_V4];

} rfnv_txagc_control_static_data_t_v4;

typedef PACK(struct)
{
  /* MIN POWER: RFNV_LTE_Bxx_MIN_TX_POWER_DB10_I */
  int16 min_tx_power_dB10;

}rfnv_tx_min_power_limit_control_t;

/* RFNV_DATA_TYPE_TX_MIN_POWER_LIMIT_CONTROL RFNV variant typeid 218 to be used with Tx Static NV */
typedef PACK(struct)
{
  /*! NV Container Index for each Tx path for a given LTE band */
  uint8 nv_container_index;

  /*! Reserved for now, not used. */
  uint8 reserved;

  /*! TxPL Control structure */
  rfnv_tx_min_power_limit_control_t tx_min_pl_ctrl;

} rfnv_tx_min_power_limit_control_static_data_t;


typedef PACK(struct)
{
  /* MAX POWER: RFNV_TDS_TX_POWER_DBM10_I */
  int16 tds_tx_power_dBm10;

}rfnv_tds_tx_power_dbm10_t;

/*RFNV_DATA_TYPE_TDS_TX_MAX_POWER_LIMIT_CONTROL RFNV variant typeid 225*/
typedef PACK(struct)
{
  /*! NV Container Index for each Tx path */
  uint8 reserved;

  /*! Reserved */
  uint8 reserved2;

  /*! MTPL Control*/
  rfnv_tds_tx_power_dbm10_t tx_max_power_dbm10;

}rfnv_tds_tx_max_power_limit_control_static_data_t;

typedef int16 rfnv_data_type_cdma_pa[RFNV_MAX_PA_STATE];
typedef rfnv_data_type_cdma_pa rfnv_data_type_cdma_freq[RFNV_CHAN_INFO_CHANNEL_LIST_SIZE];
typedef rfnv_data_type_cdma_freq rfnv_data_type_cdma_pin_pout[RFNV_MAX_PA_STATE];

/* Variant type 226 */
typedef PACK(struct){
   rfnv_data_type_cdma_pa Access_PA_FALL_SW;
   rfnv_data_type_cdma_pa Access_PA_RISE_SW;
   rfnv_data_type_cdma_pa DATA_rate1_PA_FALL_SW;
   rfnv_data_type_cdma_pa DATA_rate1_PA_RISE_SW;
   rfnv_data_type_cdma_pa DATA_rate2_PA_FALL_SW;
   rfnv_data_type_cdma_pa DATA_rate2_PA_RISE_SW;
   rfnv_data_type_cdma_pa DATA_rate3_PA_FALL_SW;
   rfnv_data_type_cdma_pa DATA_rate3_PA_RISE_SW;
   rfnv_data_type_cdma_pa DATA_rate4_PA_FALL_SW;
   rfnv_data_type_cdma_pa DATA_rate4_PA_RISE_SW;
   rfnv_data_type_cdma_pa DATA_rate5_PA_FALL_SW;
   rfnv_data_type_cdma_pa DATA_rate5_PA_RISE_SW;
   rfnv_data_type_cdma_pa DATA_rate6_PA_FALL_SW;
   rfnv_data_type_cdma_pa DATA_rate6_PA_RISE_SW;
   rfnv_data_type_cdma_pa HDR_Access_PA_FALL_SW;
   rfnv_data_type_cdma_pa HDR_Access_PA_RISE_SW;
   rfnv_data_type_cdma_pa HDR_DOR0_RRI0_PA_FALL_SW;
   rfnv_data_type_cdma_pa HDR_DOR0_RRI0_PA_RISE_SW;
   rfnv_data_type_cdma_pa HDR_DOR0_RRI1_PA_FALL_SW;
   rfnv_data_type_cdma_pa HDR_DOR0_RRI1_PA_RISE_SW;
   rfnv_data_type_cdma_pa HDR_DOR0_RRI2_PA_FALL_SW;
   rfnv_data_type_cdma_pa HDR_DOR0_RRI2_PA_RISE_SW;
   rfnv_data_type_cdma_pa HDR_DOR0_RRI3_PA_FALL_SW;
   rfnv_data_type_cdma_pa HDR_DOR0_RRI3_PA_RISE_SW;
   rfnv_data_type_cdma_pa HDR_DOR0_RRI4_PA_FALL_SW;
   rfnv_data_type_cdma_pa HDR_DOR0_RRI4_PA_RISE_SW;
   rfnv_data_type_cdma_pa HDR_DOR0_RRI5_PA_FALL_SW;
   rfnv_data_type_cdma_pa HDR_DOR0_RRI5_PA_RISE_SW;
   rfnv_data_type_cdma_pa HDR_DORA_PA_FALL_SW;
   rfnv_data_type_cdma_pa HDR_DORA_PA_RISE_SW;
   rfnv_data_type_cdma_pa cdma_reserved[RFNV_DATA_TYPE_ID_SIZE];
} rfnv_data_type_cdma_pa_switchpoints_type;

/* Variant type 226 */
typedef PACK(struct){
   rfnv_data_type_cdma_pa Access_PA_FALL_SW;
   rfnv_data_type_cdma_pa Access_PA_RISE_SW;
   rfnv_data_type_cdma_pa DATA_rate1_PA_FALL_SW;
   rfnv_data_type_cdma_pa DATA_rate1_PA_RISE_SW;
   rfnv_data_type_cdma_pa DATA_rate2_PA_FALL_SW;
   rfnv_data_type_cdma_pa DATA_rate2_PA_RISE_SW;
   rfnv_data_type_cdma_pa DATA_rate3_PA_FALL_SW;
   rfnv_data_type_cdma_pa DATA_rate3_PA_RISE_SW;
   rfnv_data_type_cdma_pa DATA_rate4_PA_FALL_SW;
   rfnv_data_type_cdma_pa DATA_rate4_PA_RISE_SW;
   rfnv_data_type_cdma_pa DATA_rate5_PA_FALL_SW;
   rfnv_data_type_cdma_pa DATA_rate5_PA_RISE_SW;
   rfnv_data_type_cdma_pa DATA_rate6_PA_FALL_SW;
   rfnv_data_type_cdma_pa DATA_rate6_PA_RISE_SW;
   rfnv_data_type_cdma_pa HDR_Access_PA_FALL_SW;
   rfnv_data_type_cdma_pa HDR_Access_PA_RISE_SW;
   rfnv_data_type_cdma_pa HDR_DOR0_RRI0_PA_FALL_SW;
   rfnv_data_type_cdma_pa HDR_DOR0_RRI0_PA_RISE_SW;
   rfnv_data_type_cdma_pa HDR_DOR0_RRI1_PA_FALL_SW;
   rfnv_data_type_cdma_pa HDR_DOR0_RRI1_PA_RISE_SW;
   rfnv_data_type_cdma_pa HDR_DOR0_RRI2_PA_FALL_SW;
   rfnv_data_type_cdma_pa HDR_DOR0_RRI2_PA_RISE_SW;
   rfnv_data_type_cdma_pa HDR_DOR0_RRI3_PA_FALL_SW;
   rfnv_data_type_cdma_pa HDR_DOR0_RRI3_PA_RISE_SW;
   rfnv_data_type_cdma_pa HDR_DOR0_RRI4_PA_FALL_SW;
   rfnv_data_type_cdma_pa HDR_DOR0_RRI4_PA_RISE_SW;
   rfnv_data_type_cdma_pa HDR_DOR0_RRI5_PA_FALL_SW;
   rfnv_data_type_cdma_pa HDR_DOR0_RRI5_PA_RISE_SW;
   rfnv_data_type_cdma_pa HDR_DORA_PA_FALL_SW;
   rfnv_data_type_cdma_pa HDR_DORA_PA_RISE_SW;
   rfnv_data_type_cdma_pa cdma_reserved[RFNV_DATA_TYPE_ID_SIZE];
} rfnv_data_type_cdma_pa_static;

typedef PACK(struct){
  uint8 swpt_type;
  int16 fall_or_hyst;
  int16 rise_or_backoff;
  int16 fall_hold;
  int16 rise_hold;
} ic_swt_type;

typedef PACK(struct){
  uint8 active;
  ic_swt_type SWPT[6];
} icx_type;

typedef PACK(struct){
  icx_type pm[3];
} icx_pmx_type;

/* Variant type 227 */
typedef PACK(struct){
  icx_pmx_type onex;
  icx_pmx_type hdr;
} rfnv_data_type_cdma_rx_switchpoints;

 /* Variant type 228 */
typedef PACK(struct){
  uint32 onex_enc_btf;
  uint32 hdr_enc_btf;
} rfnv_data_type_cdma_enc_btf;

/* Variant type 296 */
typedef PACK(struct)
{
  uint8 reserved;
  /*!< Reserved for future use */
  uint16 sar_tx_power_limit_dB10[RFNV_CDMA_SAR_BIN_NUM];
  /*!< Power limit in dBm10 for DSI states.
       The value 0xFFFE causes the software to use the power limit
       of the other antenna position NV item, instead of this one.
   */
} rfnv_data_type_cdma_variant_tx_sar_t;

/* Variant type 297 */
typedef PACK(struct)
{
  uint8 bw;
  int16 i_code;
  int16 q_code;
} rfnv_data_type_ip2_cal_t;

/* Variant type 431 */
typedef PACK(struct)
{
  uint32 frequency;
  uint8 bw;
  uint8 therm_bin;
  uint16 therm_adc_val;
  int16 tx_offset_kHz;
  uint8 i_code_main;
  uint8 q_code_main;
  uint8 i_code_jrf;
  uint8 q_code_jrf;
} rfnv_data_type_iip2_cal_vs_temp_vs_freq;

/* Variant type 487 */
typedef PACK(struct)
{
  uint32 frequency;
  uint32 bw;
  uint8 therm_bin;
  uint16 therm_adc_val;
  int16 tx_offset_kHz;
  uint8 i_code_main;
  uint8 q_code_main;
  uint8 i_code_jrf;
  uint8 q_code_jrf;
} rfnv_data_type_iip2_cal_vs_temp_vs_freq_v2;

/* Variant type 281 */
typedef PACK(struct){
  uint8 max_pwr_lim_vs_temp[RFNV_TEMP_BIN_SIZE];
}rfnv_data_type_1x_tx_lim_vs_temp;

/* Variant type 282 */
typedef PACK(struct){
  int16 max_pwr_lim_vs_temp[RFNV_TEMP_BIN_SIZE];
}rfnv_data_type_hdr_tx_lim_vs_temp;

/* RFNV_DATA_TYPE_PA_COMP_CONTROL RFNV variant typeid 231*/
typedef PACK(struct)
{
  /*! reserved*/
  uint8 reserved;
  /*! Reserved */
  uint8 reserved2;
  /*! MTPL Control*/
  int16 pa_comp_up[8];
  int16 pa_comp_down[8];
  int16 pa_comp_up_vs_freq[16][8];
  int16 pa_comp_down_vs_freq[16][8];
}rfnv_pa_comp_control_static_data_t;

 /* Variant type 232 */
typedef PACK(struct){
  uint8 pa_state;
  uint8 wvfm_type;
  uint8 lin_chan_idx;
  int32 alpha_vin;
  int32 beta_vin;
  int32 alpha_vcc;
  int32 beta_vcc;
}rfnv_data_type_mline_results;

 /* Variant type 469 */
typedef PACK(struct){
  uint8 pa_state;
  uint8 waveform_type;
  uint32 frequency;
  int32 alpha_vin;
  int32 beta_vin;
  int32 alpha_vcc;
  int32 beta_vcc;
}rfnv_data_type_mline_results_v2;

/* Variant type 414 */
typedef PACK(struct){
  int16 onex_papm_rssi_switchpoint_dbm10;
  int16 hdr_papm_rssi_switchpoint_dbm10;
}rfnv_data_type_cdma_papm_rssi_switchpoint_t;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Type definition for Variant RFNV_WCDMA_VARIANT_TX_POWER */
typedef PACK(struct)
{
  uint8 reserved;
  /*!< Reserved for future use */

  int16 max_power_dbm10;
  /*!< Maximum Tx power in dBm10 units */

  int16 lim_vs_temp[RFNV_TEMP_TABLE_SIZ];
  /*!< Tx lim vs Freq */

} rfnv_wcdma_variant_tx_power_t;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Type definition for Variant RFNV_WCDMA_VARIANT_TX_TIMING */
typedef PACK(struct)
{
  uint8 reserved;
  /*!< Reserved for future use */

  uint16 pa_on_rise;
  /*!< PA On rise */

  uint16 pa_on_fall;
  /*!< PA On fall */

  uint16 tx_on_rise;
  /*!< Tx On rise */

  uint16 tx_on_fall;
  /*!< Tx On fall */

} rfnv_wcdma_variant_tx_timing_t;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Type definition for Variant RFNV_WCDMA_VARIANT_TX_MPR */
typedef PACK(struct)
{
  uint8 reserved;
  /*!< Reserved for future use */

  uint8 carrier_mode;
  /*!< Carrier Mode: 0 - SC, 1 - DC */

  uint16 beta_scaling[RFNV_WCDMA_MPR_BIN_NUM];
  /*!< Beta scaling */

  uint16 agc_offset[RFNV_WCDMA_MPR_BIN_NUM];
  /*!< AGC Offset */

  uint16 tx_mpr_backoff[RFNV_WCDMA_MPR_BIN_NUM];
  /*!< Tx MPR backoff */

  uint16 pa_mpr_backoff[RFNV_WCDMA_MPR_BIN_NUM-1];
  /*!< PA MPR backoff */

} rfnv_wcdma_variant_tx_mpr_t;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Type definition for Variant RFNV_WCDMA_VARIANT_SAR */
typedef PACK(struct)
{
  uint8 reserved;
  /*!< Reserved for future use */

  uint8 antenna_config;
  /*!< Antenna config */

  int16 sar_mtpl_dbm10[RFNV_WCDMA_SAR_BIN_NUM];
  /*!< Beta scaling */

} rfnv_wcdma_variant_tx_sar_t;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Type definition for Variant RFNV_WCDMA_VARIANT_RX_NOISE_FLOOR */
typedef PACK(struct)
{
  uint8 reserved;
  /*!< Reserved for future use */

  uint8 carrier_mode;
  /*!< Carrier Mode: 0 - SC, 1 - DC */

  int16 noise_floor;
  /*!< Beta scaling */

} rfnv_wcdma_variant_rx_noise_floor_t;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Type definition for Variant RFNV_WCDMA_VARIANT_BTF */
typedef PACK(struct)
{
  uint8 reserved;
  /*!< Reserved for future use */

  uint32 btf;
  /*!< BTF */

} rfnv_wcdma_variant_btf_t;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Type definition for Variant RFNV_WCDMA_VARIANT_PAPM_RSSI_SWITCHPOINT */
typedef PACK(struct)
{
  uint8 reserved;
  /*!< Reserved for future use */

  uint8 enable;

  int16 rssi_switchpoint_db10;

} rfnv_wcdma_variant_papm_rssi_switchpoint_t;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Type definition for Variant RFNV_WCDMA_VARIANT_RX_LNA_SP_T */
typedef PACK( struct )
{
  uint8 reserved;
  uint8 carr_id;
  uint8 hs_config;
  uint8 jammer;
  int16 lna_rise[WCDMA_NUM_LNA_STATES];
  int16 hysteresis[WCDMA_NUM_LNA_STATES];
} rfnv_wcdma_variant_rx_lna_sp_t;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Type definition for Variant RFNV_WCDMA_VARIANT_RX_JAMMER_THRES_TYPE */
typedef PACK( struct )
{
  uint8 reserved;
  int32 wbee_thresh;
  int32 ee_aci_thresh;
} rfnv_wcdma_variant_rx_jammer_thres_t;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Type definition for Variant RFNV_WCDMA_VARIANT_RX_NOTCH_PARAM_TYPE */
typedef PACK( struct )
{
  uint8 reserved;
  int32 freq_hz;
  uint8 notch_depth;
  int16 activation_threshold_db;
  int16 deactivation_threshold_db;
} rfnv_wcdma_rx_notch_param_t;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Type definition for Variant RFNV_WCDMA_VARIANT_RX_NOTCH_TYPE */
typedef PACK( struct )
{
  uint8 reserved;
  uint8 no_of_notch;
  rfnv_wcdma_rx_notch_param_t notch_param[RFNV_WCDMA_MAX_NUM_NOTCH];
} rfnv_wcdma_variant_rx_notch_t;

/*
  RFNV_DATA_TYPE_TDS_TX_CHAN_LIST                    = 267,
  RFNV_DATA_TYPE_TDS_MPR_VAL_V2                      = 268,
  RFNV_DATA_TYPE_TDS_LNA_RANGE_RISE_FALL             = 269,
*/


/*--------------------------------------------------------------------------------------------------------------------*/

#ifndef RFNV_TDS_LNA_GS_PAIR_SIZE
#define RFNV_TDS_LNA_GS_PAIR_SIZE 12
#endif

typedef PACK(struct)
{
  uint16 reserved;
  /*!< Tx channel list */
  uint16 tx_chan_list[16];
}rfnv_tds_tx_chan_list_t;


typedef PACK(struct)
{
  uint16 reserved;
  /*!< MPR power backoff V2 */
  uint16 mpr_val_v2[17];
}rfnv_tds_mpr_val_v2_t;


typedef PACK(struct)
{
  uint16 reserved;
  /*!<  lna threshold in narrow band in acquisation mode*/
  uint8   lna_range_rise_fall_nb_mode_acq[RFNV_TDS_LNA_GS_PAIR_SIZE];
  /*!<  lna threshold in wide band in acquisation mode*/
  uint8   lna_range_rise_fall_wb_mode_acq[RFNV_TDS_LNA_GS_PAIR_SIZE];
  /*!<  lna threshold in narrow band in acquisation mode*/
  uint8   lna_range_rise_fall_nb_mode_acq_2[RFNV_TDS_LNA_GS_PAIR_SIZE];
  /*!<  lna threshold in wide band in acquisation mode*/
  uint8   lna_range_rise_fall_wb_mode_acq_2[RFNV_TDS_LNA_GS_PAIR_SIZE];
  /*!<  lna threshold in narrow band in idle mode*/
  uint8   lna_range_rise_fall_nb_mode_idle[RFNV_TDS_LNA_GS_PAIR_SIZE];
  /*!<  lna threshold in wide band in idle mode*/
  uint8   lna_range_rise_fall_wb_mode_idle[RFNV_TDS_LNA_GS_PAIR_SIZE];
  /*!<  lna threshold in narrow band in inter freq mode*/
  uint8   lna_range_rise_fall_nb_mode_ifreq[RFNV_TDS_LNA_GS_PAIR_SIZE];
  /*!<  lna threshold in wide band in inter freq mode*/
  uint8   lna_range_rise_fall_wb_mode_ifreq[RFNV_TDS_LNA_GS_PAIR_SIZE];
  /*!<  lna threshold in narrow band in power scan mode*/
  uint8   lna_range_rise_fall_nb_mode_power_scan[RFNV_TDS_LNA_GS_PAIR_SIZE];
  /*!<  lna threshold in wide band in power scan mode*/
  uint8   lna_range_rise_fall_wb_mode_power_scan[RFNV_TDS_LNA_GS_PAIR_SIZE];
  /*!<  lna threshold in narrow band in tracking mode*/
  uint8   lna_range_rise_fall_nb_mode_tracking[RFNV_TDS_LNA_GS_PAIR_SIZE];
  /*!<  lna threshold in wide band in tracking mode*/
  uint8   lna_range_rise_fall_wb_mode_tracking[RFNV_TDS_LNA_GS_PAIR_SIZE];
  /*!<  lna threshold in narrow band in tracking mode*/
  uint8   lna_hs_range_rise_fall_nb_mode_tracking[RFNV_TDS_LNA_GS_PAIR_SIZE];
  /*!<  lna threshold in wide band in tracking mode*/
  uint8   lna_hs_range_rise_fall_wb_mode_tracking[RFNV_TDS_LNA_GS_PAIR_SIZE];
}rfnv_tds_lna_range_rise_fall_t;


/*RFCOMMON_MAX_SPUR_ENTRIES = 30*/
/*RFNV_DATA_TYPE_TDS_SPUR_LIST_TYPE                             = 303*/

#ifndef RFNV_TDS_SPUR_FREQ_LIST_SIZE
#define RFNV_TDS_SPUR_FREQ_LIST_SIZE 12
#endif

typedef PACK(struct)
{
  uint8 reserved;
  /*!< Num of spur freq */
  uint8 num_spur_freq;
  /*!< Spur channel list */
  rfnv_rx_spur_data_type spur_freq_list[RFNV_TDS_SPUR_FREQ_LIST_SIZE];
}rfnv_tds_spur_freq_list_t;

typedef PACK(struct)
{
  /*! Channel boundaries for power limit regions.  This must be ordered
      in increasing channel order, and the table is terminated by the
      first occurence of 0xFFFFFFFF if less than 16 entries are used. */
  uint32 tx_upper_bound_earfcn[RFNV_TX_MAX_PWR_ENTRIES];

  /*! Maximum power limit adjustment to be applied for each channel region.
      Adjustment can be positive or negative to boost or reduce the power
      defined in the max power limit NV.  Units are dB10. */
  int16 pwr_adjust_db10[RFNV_TX_MAX_PWR_ENTRIES];
} rfnv_tx_max_pwr_limit_adj_vs_earfcn_table_t;

typedef PACK(struct)
{
  /*! Channel boundaries for power limit regions.  This must be ordered
      in increasing freq order, and the table is terminated by the
      first occurence of 0xFFFFFFFF if less than 16 entries are used. */
  uint32 tx_upper_bound_freq[RFNV_TX_MAX_PWR_ENTRIES];

  /*! Maximum power limit adjustment to be applied for each channel region.
      Adjustment can be positive or negative to boost or reduce the power
      defined in the max power limit NV.  Units are dB10. */
  int16 pwr_adjust_db10[RFNV_TX_MAX_PWR_ENTRIES];
} rfnv_tx_max_pwr_limit_adj_vs_freq_table_t;

typedef PACK(struct)
{
  /*! Channel boundaries for power limit regions.  This must be ordered
      in increasing freq order, and the table is terminated by the
      first occurence of 0xFFFFFFFF if less than 16 entries are used. */
  uint32 tx_upper_bound_freq_khz[RFNV_TX_MAX_PWR_ENTRIES];

  /*! Maximum power limit adjustment to be applied for each channel region.
      Units are dBm10. */
  int16 max_pwr_limit_dbm10[RFNV_TX_MAX_PWR_ENTRIES];
} rfnv_tx_max_pwr_limit_adj_vs_freq_table_t_V2;

/* RFNV_DATA_TYPE_TX_MAX_POWER_LIMIT_ADJUST_V4 RFNV variant typeid 494 */
typedef PACK(struct)
{
  /*! NV container that this data item applies to. */
  uint8 nv_container_index;

  /*! Channel and max power adjust table */
  rfnv_tx_max_pwr_limit_adj_vs_freq_table_t_V2 tx_pwr_limit_adjust;
} rfnv_max_tx_pwr_limit_adj_static_data_t_v4;

/* RFNV_DATA_TYPE_TX_MAX_POWER_LIMIT_ADJUST_V2 RFNV variant typeid 447 */
typedef PACK(struct)
{
  /*! NV container that this data item applies to. */
  uint8 nv_container_index;

  /*! Power class that this data item applies to.
      Implemented as a bitmask.
        Bit0 - Power Class 1
        Bit1 - Power Class 2
        Bit2 - Power Class 3
        ...
      If all Bits are enabled this backoff will be applied to all supported
      UE Power classes. */
  uint32 ue_power_class_mask;

  /*! Minimum contiguous CA bandwidth in KHz that this data item
      applies to.  -1 indicates there is no lower bound for this data item. */
  int32 min_bw_khz;

  /*! Maximum contiguous CA bandwidth in KHz that this data item
      applies to.  -1 indicates there is no upper bound for this data item. */
  int32 max_bw_khz;

  /*! Channel and max power adjust table */
  rfnv_tx_max_pwr_limit_adj_vs_freq_table_t tx_pwr_limit_adjust;
} rfnv_max_tx_pwr_limit_adj_static_data_t_v2;

/* RFNV_DATA_TYPE_TX_MAX_POWER_LIMIT_ADJUST_V3 RFNV variant typeid 473 */
typedef PACK(struct)
{
  /*! NV container that this data item applies to. */
  uint8 catm1_nb1_mode;

  /*! Power class that this data item applies to.
      Implemented as a bitmask.
        Bit0 - Power Class 1
        Bit1 - Power Class 2
        Bit2 - Power Class 3
        ...
      If all Bits are enabled this backoff will be applied to all supported
      UE Power classes. */
  uint32 ue_power_class_mask;

  /*! Minimum contiguous CA bandwidth in KHz that this data item
      applies to.  -1 indicates there is no lower bound for this data item. */
  int32 min_bw_khz;

  /*! Maximum contiguous CA bandwidth in KHz that this data item
      applies to.  -1 indicates there is no upper bound for this data item. */
  int32 max_bw_khz;

  /*! Channel and max power adjust table */
  rfnv_tx_max_pwr_limit_adj_vs_freq_table_t tx_pwr_limit_adjust;
} rfnv_max_tx_pwr_limit_adj_static_data_t_v3;


/* RFNV_DATA_TYPE_TX_MAX_POWER_LIMIT_ADJUST RFNV variant typeid 280 */
typedef PACK(struct)
{
  /*! NV container that this data item applies to. */
  uint8 nv_container_index;

  /*! Power class that this data item applies to.
      Implemented as a bitmask.
        Bit0 - Power Class 1
        Bit1 - Power Class 2
        Bit2 - Power Class 3
        ...
      If all Bits are enabled this backoff will be applied to all supported
      UE Power classes. */
  uint32 ue_power_class_mask;

  /*! Minimum contiguous CA bandwidth in KHz that this data item
      applies to.  -1 indicates there is no lower bound for this data item. */
  int32 min_bw_khz;

  /*! Maximum contiguous CA bandwidth in KHz that this data item
      applies to.  -1 indicates there is no upper bound for this data item. */
  int32 max_bw_khz;

  /*! Channel and max power adjust table */
  rfnv_tx_max_pwr_limit_adj_vs_earfcn_table_t tx_pwr_limit_adjust;
} rfnv_max_tx_pwr_limit_adj_static_data_t;

/* RFNV_DATA_TYPE_ENDC_LTE_MTPL_LIMIT_ADJUST RFNV variant typeid 545 */
typedef PACK(struct)
{
  /*! Power class that this data item applies to.
     Implemented as a bitmask.
       Bit0 - Power Class 1
       Bit1 - Power Class 2
       Bit2 - Power Class 3
       ...
     If all Bits are enabled this backoff will be applied to all supported
     UE Power classes. */
  uint32 ue_power_class_mask;

  /*! Minimum contiguous CA bandwidth in KHz that this data item
     applies to.  -1 indicates there is no lower bound for this data item. */
  int32 min_bw_khz;

  /*! Maximum contiguous CA bandwidth in KHz that this data item
     applies to.  -1 indicates there is no upper bound for this data item. */
  int32 max_bw_khz;

  /* band number for the other tech in ENDC, 0xFFFF means any band is suitable
     Ignore it if current mode is non-ENDC */
  uint32 endc_band_number;

  /* ul channel for the other tech in ENDC,
     Ignore it if current mode is non-ENDC */
  uint32 endc_ul_upper_bound_arfcn;
  uint32 endc_ul_lower_bound_arfcn;

  /*! Channel and max power adjust table */
  rfnv_tx_max_pwr_limit_adj_vs_earfcn_table_t endc_mtpl_limit_adjust;
}rfnv_lte_mtpl_endc_limit_adjust_static_data_t;

typedef PACK(struct)
{
  /*! Channel boundaries for power limit regions.  This must be ordered
      in increasing channel order, and the table is terminated by the
      first occurence of 0xFFFFFFFF if less than 16 entries are used. */
  uint32 tx_upper_bound_earfcn[RFNV_LTE_TX_CHAN_SIZE];

  /*! Maximum power limit adjustment to be applied for each channel region and a given temperature range.
      Adjustment can be positive or negative to boost or reduce the power defined in the max power limit NV.
      Units are dB10. */
  int16 db10_pwr_adj[RFNV_LTE_TX_CHAN_SIZE][RFNV_LTE_TX_TEMP_BIN_SIZE];
} rfnv_tx_max_pwr_lim_adj_vs_earfcn_vs_temp_table_t;

/* RFNV_DATA_TYPE_TX_MAX_PWR_LIM_ADJ_FREQ_VS_TEMP RFNV variant typeid 415 */
typedef PACK(struct)
{
  /*! NV container that this data item applies to. */
  uint8 nv_container_index;

  /*! Power class that this data item applies to.
      Implemented as a bitmask.
        Bit0 - Power Class 1
        Bit1 - Power Class 2
        Bit2 - Power Class 3
        ...
      If all Bits are enabled this backoff will be applied to all supported
      UE Power classes. */
  uint8 ue_power_class_mask;

  /*! Reserved for future use */
  int16 reserved;

  /*! Minimum contiguous CA bandwidth in KHz that this data item
      applies to.  -1 indicates there is no lower bound for this data item. */
  int32 min_bw_khz;

  /*! Maximum contiguous CA bandwidth in KHz that this data item
      applies to.  -1 indicates there is no upper bound for this data item. */
  int32 max_bw_khz;

  /*! Channel, temp and max power adjust table */
  rfnv_tx_max_pwr_lim_adj_vs_earfcn_vs_temp_table_t tx_pwr_limit_adjust;
} rfnv_max_tx_pwr_lim_adj_vs_freq_vs_temp_static_data_t;


/* RFNV_DATA_TYPE_PEMAX_ADJUST RFNV variant typeid 413 */
typedef PACK(struct)
{
  /*! NV container that this data item applies to. */
  uint8 nv_container_index;

  /*! Power class that this data item applies to.
      Implemented as a bitmask.
        Bit0 - Power Class 1
        Bit1 - Power Class 2
        Bit2 - Power Class 3
        ...
      If all Bits are enabled this offset will be applied to all supported
      UE Power classes. */
  uint8 ue_power_class_mask;

  /*! Reserved for future use */
  int16 reserved;

  /*! Minimum contiguous CA bandwidth in KHz that this data item
      applies to.  -1 indicates there is no lower bound for this data item. */
  int32 min_bw_khz;

  /*! Maximum contiguous CA bandwidth in KHz that this data item
      applies to.  -1 indicates there is no upper bound for this data item. */
  int32 max_bw_khz;

  /*! Pemax adjust value in dB10 */
  int16 pemax_adj_db10;
} rfnv_pemax_adj_static_data_t;

/*RFNV_DATA_TYPE_LTE_TX_BANDEDGE_BACKOFF RFNV variant typeid 288*/
typedef PACK(struct)
{
  /* backoff value for lower edge */
  uint8 lo_bandedge_backoff_db10;

  /* backoff values for higher edge */
  uint8 hi_bandedge_backoff_db10;

  /*! Lower edge BW in KHz
    -1 indicates there is no lower bound for this data item. */
  int32 lo_bandedge_bw_khz;

    /*! Upper edge bandwidth in KHz
        -1 indicates there is no upper bound for this data item. */
  int32 hi_bandedge_bw_khz;

  /*Pa switchpoint backoff for edges*/
  uint8 lo_bandedge_pa_switchpt_backoff;
  uint8 hi_bandedge_pa_switchpt_backoff;

} rfnv_lte_bandedge_backoff_type;

/* RFNV_DATA_TYPE_SNR_SAWLESS_PARA varaint typeid 464 */
typedef PACK (struct)
{
  /*! the period of snr learning initialized as 1000*/
  uint16  th_learning_counter_saw;
  uint16  th_learning_counter_sawless;
  uint16  counter_penalty_saw_learning;
  uint16  counter_penalty_sawless_learning;

  /*! floor of estimated Noise reading from NV*/
  int16  th_noise_low;
  /*! ceiling of estimated Noise */
  int16  th_noise_high;
  int16  th_snr_stop_sawless_learning;
} rfnv_lte_snr_sawless_para_data_type;

/* RFNV_DATA_TYPE_SAWLESS_JDET_THRESHOLD_OFFSET varaint typeid 289 */
typedef PACK(struct)
{
  uint8 jam_low_th;
  uint8 jam_high_th;
} sawless_jdet_threshold_offset;

typedef PACK(struct)
{
  uint8 number_of_valid_entries;
  sawless_jdet_threshold_offset thresholds[RFNV_MAX_SAWLESS_JDET_THRESHOLD_OFFSETS];
} rfnv_sawless_jdet_threshold_offsets;

typedef PACK(struct)
{
  uint32 nbjdet_th_g0;
  uint32 nbjdet_th_g1;
} nb_jdet_threshold_type;

typedef PACK(struct)
{
  uint8 number_of_valid_entries;
  nb_jdet_threshold_type thresholds[RFNV_MAX_NB_JDET_THRESHOLDS];
} rfnv_nb_jdet_thresholds;


/* This structure will have  UL and DL bands configuration */
typedef PACK(struct)
{
  /* LTE 3GPP SPEC BAND NUMBER */
  uint32 band_num;

  /* Aggregated channel bandwidth mask for ampr backoff region */
  /* bit 0 - class A, bit 1 - class B, bit 2 - class C, and so on */
  uint32 bandwidth_class;

  /* Is UL enabled on this band
     1 - Enabled
     0 - Disabled */
  uint8 ul_enabled;

 /* Index to the UL band Table for Max power intries corresponding
  to this band to be filled part of the same NV.
  This field is NA, if this band tbl entry has ul_enabled as FALSE */
  uint8 ul_band_tbl_index;

} rfnv_lte_static_band_info_t;

/* This structure will solely define UL bands configuration */
typedef PACK(struct)
{
  /* NV to be considered only if RFM PATH header being used by this band,
     Wild-card is allowed for all supported paths */
  rfnv_variant_element_header_v2 path_hdr;

  /* Is Tx Diversity or MIMO enabled
     1 - Enabled
     0 - Disabled */
  uint8 tx_diversity_enabled;

  /* Total combined Tx power limit without Tx Diversity Enabled */
  int16 total_combined_power_limit_without_tx_div_dB10;

  /* Total combined Tx power limit with Tx Diversity Enabled.
     This field is irrelevant if tx_diversity_enabled flag is FALSE */
  int16 total_combined_power_limit_with_tx_div_dB10;

  /* Channel boundaries for power limit regions */
  uint32 tx_upper_bound_earfcn[RFNV_TX_MAX_PWR_ENTRIES];

  /* Maximum power limit for channels up to the EARFCN in tx_upper_bound_earfcn */
  int16 pwr_dbm10[RFNV_TX_MAX_PWR_ENTRIES];

} rfnv_ca_max_tx_pwr_limit_static_band_tbl_t;

/* RFNV_DATA_TYPE_LTE_CA_TX_MAX_POWER_LIMIT varaint typeid 302 */
typedef PACK(struct)
{
  /* Number of band Table active in this NV.
     This will help in looking up the band_tbl item below */
  uint8 num_active_band_tbl_entries;

  /* Entry for BW combos including all DL + UL bands  */
  rfnv_lte_static_band_info_t  band_tbl[RFNV_MAX_LTE_ALL_BANDS_MAX_PWR_LIMIT_TBL_SIZE];

/* Entry for Max power for a given UL band */
  rfnv_ca_max_tx_pwr_limit_static_band_tbl_t ul_band_tbl[RFNV_MAX_LTE_UL_BANDS_MAX_PWR_LIMIT_TBL_SIZE];

} rfnv_ca_max_tx_pwr_limit_static_data_t;

/* This structure will solely define UL bands configuration */
typedef PACK(struct)
{
  /* NV to be considered only if RFM PATH header being used by this band,
     Wild-card is allowed for all supported paths */
  rfnv_variant_element_header_v3 path_hdr;

  /* Is Tx Diversity or MIMO enabled
     1 - Enabled
     0 - Disabled */
  uint8 tx_diversity_enabled;

  /* Total combined Tx power limit without Tx Diversity Enabled */
  int16 total_combined_power_limit_without_tx_div_dB10;

  /* Total combined Tx power limit with Tx Diversity Enabled.
     This field is irrelevant if tx_diversity_enabled flag is FALSE */
  int16 total_combined_power_limit_with_tx_div_dB10;

  /* Channel boundaries for power limit regions */
  uint32 tx_upper_bound_earfcn[RFNV_TX_MAX_PWR_ENTRIES];

  /* Maximum power limit for channels up to the EARFCN in tx_upper_bound_earfcn */
  int16 pwr_dbm10[RFNV_TX_MAX_PWR_ENTRIES];

} rfnv_ca_max_tx_pwr_limit_static_band_tbl_v2_t;

/* RFNV_DATA_TYPE_LTE_CA_TX_MAX_POWER_LIMIT_V2 varaint typeid 491 */
typedef PACK(struct)
{
  /* Number of band Table active in this NV.
     This will help in looking up the band_tbl item below */
  uint8 num_active_band_tbl_entries;

  /* Entry for BW combos including all DL + UL bands  */
  rfnv_lte_static_band_info_t  band_tbl[RFNV_MAX_LTE_ALL_BANDS_MAX_PWR_LIMIT_TBL_SIZE];

/* Entry for Max power for a given UL band */
  rfnv_ca_max_tx_pwr_limit_static_band_tbl_v2_t ul_band_tbl[RFNV_MAX_LTE_UL_BANDS_MAX_PWR_LIMIT_TBL_SIZE];

} rfnv_ca_max_tx_pwr_limit_static_data_v2_t;

typedef PACK(struct)
{
   /* LTE 3GPP SPEC BAND NUMBER for all TX carriers */
   uint16 band[RFNV_MAX_LTE_UL_BANDS_MAX_PWR_LIMIT_TBL_SIZE];

   /*L1 L5 blanking types for different gnss base bands */
   uint8 gnss_l1_l5_blanking_type;

} rfnv_lte_gnss_blanking_sets_t;

/* RFNV_DATA_TYPE_LTE_GNSS_BLANKING varaint typeid 573 */
typedef PACK(struct)
{
   /* LTE 3GPP SPEC BAND NUMBER for all TX carriers */
   uint16 band[RFNV_MAX_LTE_UL_BANDS_MAX_PWR_LIMIT_TBL_SIZE];

   /*L1 L5 blanking types for different gnss base bands */
   uint8 gnss_l1_l5_blanking_type;

   uint16 antenna_switch_paths[RFNV_MAX_LTE_UL_BANDS_MAX_PWR_LIMIT_TBL_SIZE];

} rfnv_lte_gnss_blanking_sets_t_V2;

/* RFNV_DATA_TYPE_LTE_GNSS_BLANKING varaint typeid 450 */
typedef PACK(struct)
{
  /* Number of lte gnss blanking sets active in this NV.
     This will help in looking up the gnss_blanking_sets item below */
  uint8 num_gnss_blanking_sets;

  /* Entry for gnss_blanking_sets including all band_combos and blanking types  */
  rfnv_lte_gnss_blanking_sets_t  *gnss_blanking_sets;

} rfnv_lte_gnss_blanking_data_combos_s;

typedef PACK(struct)
{
  /* Number of lte gnss blanking sets active in this NV.
     This will help in looking up the gnss_blanking_sets item below */
  uint8 num_gnss_blanking_sets;

  /* Entry for gnss_blanking_sets including all band_combos and blanking types  */
  rfnv_lte_gnss_blanking_sets_t_V2  *gnss_blanking_sets;

} rfnv_lte_gnss_blanking_data_combos_s_v2;

/* RFNV_DATA_TYPE_GSM_GNSS_BLANKING variant typeid 550 */
typedef PACK(struct)
{
   /*L1 L5 blanking types for different gnss base bands */
   uint8 gsm_gnss_l1_l5_blanking_type[RFNV_GSM_GNSS_BLANKING_MAX_SIZE];

} rfnv_gsm_gnss_blanking_sets_t;

/* RFNV_DATA_TYPE_LTE_TX_INTRA_ULCA_APT_ET_CONTROL RFNV variant typeid 416 */
typedef PACK(struct)
{
  /*! NV container that this data item applies to. */
  uint8 nv_container_index;

  /*! Minimum contiguous CA bandwidth in KHz that this data item
      applies to.  -1 indicates there is no lower bound for this data item. */
  int32 min_bw_khz;

  /*! Maximum contiguous CA bandwidth in KHz that this data item
      applies to.  -1 indicates there is no upper bound for this data item. */
  int32 max_bw_khz;

  /*! CA-Network Signalling Value */
  uint32 ca_ns_number;

  /*! Entry to prioritize APT over ET in Intra ULCA scenario
  *** 1 - APT is prioritized
  *** 0 - ET  is prioritized                                 */
  uint8 intra_ulca_prioritize_apt;

} rfnv_tx_intra_ulca_apt_et_control_data_t;


/* RFNV_DATA_TYPE_RFDEVICE_CONFIG RFNV variant typeid 324 */
typedef PACK(struct)
{
  uint32 version;
  uint8 power_mode_detect;
  uint8 reserved[100];
} rfnv_rfdevice_config_t;

/*RFNV_DATA_TYPE_ATUNER_GPS_SHARING_TYPE ID: 419*/
typedef PACK(struct)
{
  uint8 gps_sharing_enable;
  uint8 reserved[3];
}rfnv_atuner_gps_sharing_data_type;

/*RFNV_DATA_TYPE_VS_MAX_PWR_TYPE variant type 467*/
typedef PACK(struct)
{
  int16 temp_bin_gmsk[16];
  int16 temp_bin_8psk[16];
}max_pwr_temp_bins_type;

typedef PACK(struct)
{
  uint8 number_of_valid_entries;
  max_pwr_temp_bins_type max_tx_pwr_temp_bin;
} rfnv_temp_vs_max_pwr;


/*! Defines the SAR limit for one or more hardware paths in an ULCA
    configuration defined in the RFNV_DATA_TYPE_LTE_CA_TX_SAR_POWER_LIMIT_CONTROL
    item. */
typedef PACK(struct)
{
  /*! The signal paths to which this SAR limit can apply.

      Wildcards may be used to indicate the SAR limits applies to multiple
      hardware paths.*/
  rfnv_variant_element_header_v3 path_hdr;

  /*! Reserved for future use.  Set to 0. */
  uint8 reserved1;

  /*! Reserved for future use.  Set to 0. */
  uint8 reserved2;

  /*! SAR Tx power limit for this specific path, for 8 DSI states. */
  int16 pwr_dbm10[RFNV_MAX_SAR_STATE];
} rfnv_ca_tx_sar_pwr_limit_static_band_tbl_t;
/*! Defines the SAR limit for one or more hardware paths in an ULCA
    configuration defined in the RFNV_DATA_TYPE_LTE_CA_TX_SAR_POWER_LIMIT_CONTROL
    item. */
typedef PACK(struct)
{
  /*! The signal paths to which this SAR limit can apply.

      Wildcards may be used to indicate the SAR limits applies to multiple
      hardware paths.*/
  rfnv_variant_element_header_v3 path_hdr;

  /*! Reserved for future use.  Set to 0. */
  uint8 reserved1;

  /*! Reserved for future use.  Set to 0. */
  uint8 reserved2;

  /*! SAR Tx power limit for this specific path, for 20 DSI states. */
  int16 pwr_dbm10[RFNV_MAX_SAR_STATE_V2];
} rfnv_ca_tx_sar_pwr_limit_static_band_tbl_t_v2;

/*! Defines the SAR limit for one or more hardware paths in an ULCA
    configuration defined in the RFNV_DATA_TYPE_LTE_CA_TX_SAR_POWER_LIMIT_CONTROL
    item. */
typedef PACK(struct)
{
  /*! The signal paths to which this SAR limit can apply.

      Wildcards may be used to indicate the SAR limits applies to multiple
      hardware paths.*/
  rfnv_variant_element_header_v3 path_hdr;

  /*! Reserved for future use.  Set to 0. */
  uint8 reserved1;

  /*! Reserved for future use.  Set to 0. */
  uint8 reserved2;

  /*! SAR Tx power limit for this specific path, for 40 DSI states. */
  int16 pwr_dbm10[RFNV_MAX_SAR_STATE_V3];
} rfnv_ca_tx_sar_pwr_limit_static_band_tbl_t_v3;

/*! Data type definition for RFNV_DATA_TYPE_LTE_CA_TX_SAR_POWER_LIMIT
    varaint typeid 470 */
typedef PACK(struct)
{
  /*! The number of band tables that are valid in this item.

      @ref band_tbl elements 0..@ref num_active_band_tbl_entries - 1
      contain valid data, the remaining items are unused and have no
      effect.
  */
  uint8 num_active_band_tbl_entries;

  /*! This table defines the CA configuration for which the SAR
      power limit applies.  It is an array of
      @ref num_active_band_tbl_entries entries defining the
      band and bandwidth groups of the CA combination.

      Each entry in this table that is marked as UL enabled has
      an index into the @ref ul_band_tbl which contains the
      SAR power limits for that band.

      Unused entries should be filled with 0's. */
  rfnv_lte_static_band_info_t band_tbl[RFNV_MAX_LTE_ALL_BANDS_MAX_PWR_LIMIT_TBL_SIZE];

  /*! SAR power limits for the uplink enabled bands in @band_tbl.

      An uplink enabled entry in @band_tbl refers to an index in this
      table.  Only the entries with a valid index reference from @band_tbl
      are used in this table.

      Unsed entries should be filled with 0's.

      @note There are fewer entries in this table than in @ref band_tbl
            since there are fewer uplink paths than downlink paths supported
            in CA, in general. */
  rfnv_ca_tx_sar_pwr_limit_static_band_tbl_t ul_band_tbl[RFNV_MAX_LTE_UL_BANDS_MAX_PWR_LIMIT_TBL_SIZE];
} rfnv_ca_tx_sar_pwr_limit_static_data_t;

typedef PACK(struct)
{
  /*! The number of band tables that are valid in this item.

      @ref band_tbl elements 0..@ref num_active_band_tbl_entries - 1
      contain valid data, the remaining items are unused and have no
      effect.
  */
  uint8 num_active_band_tbl_entries;

  /*! This table defines the CA configuration for which the SAR
      power limit applies.  It is an array of
      @ref num_active_band_tbl_entries entries defining the
      band and bandwidth groups of the CA combination.

      Each entry in this table that is marked as UL enabled has
      an index into the @ref ul_band_tbl which contains the
      SAR power limits for that band.

      Unused entries should be filled with 0's. */
  rfnv_lte_static_band_info_t band_tbl[RFNV_MAX_LTE_ALL_BANDS_MAX_PWR_LIMIT_TBL_SIZE];

  /*! SAR power limits for the uplink enabled bands in @band_tbl.

      An uplink enabled entry in @band_tbl refers to an index in this
      table.  Only the entries with a valid index reference from @band_tbl
      are used in this table.

      Unsed entries should be filled with 0's.

      @note There are fewer entries in this table than in @ref band_tbl
            since there are fewer uplink paths than downlink paths supported
            in CA, in general. */
  rfnv_ca_tx_sar_pwr_limit_static_band_tbl_t_v2 ul_band_tbl[RFNV_MAX_LTE_UL_BANDS_MAX_PWR_LIMIT_TBL_SIZE];
} rfnv_ca_tx_sar_pwr_limit_static_data_t_v2;

typedef PACK(struct)
{
  /*! The number of band tables that are valid in this item.

      @ref band_tbl elements 0..@ref num_active_band_tbl_entries - 1
      contain valid data, the remaining items are unused and have no
      effect.
  */
  uint8 num_active_band_tbl_entries;

  /*! This table defines the CA configuration for which the SAR
      power limit applies.  It is an array of
      @ref num_active_band_tbl_entries entries defining the
      band and bandwidth groups of the CA combination.

      Each entry in this table that is marked as UL enabled has
      an index into the @ref ul_band_tbl which contains the
      SAR power limits for that band.

      Unused entries should be filled with 0's. */
  rfnv_lte_static_band_info_t band_tbl[RFNV_MAX_LTE_ALL_BANDS_MAX_PWR_LIMIT_TBL_SIZE];

  /*! SAR power limits for the uplink enabled bands in @band_tbl.

      An uplink enabled entry in @band_tbl refers to an index in this
      table.  Only the entries with a valid index reference from @band_tbl
      are used in this table.

      Unsed entries should be filled with 0's.

      @note There are fewer entries in this table than in @ref band_tbl
            since there are fewer uplink paths than downlink paths supported
            in CA, in general. */
  rfnv_ca_tx_sar_pwr_limit_static_band_tbl_t_v3 ul_band_tbl[RFNV_MAX_LTE_UL_BANDS_MAX_PWR_LIMIT_TBL_SIZE];
} rfnv_ca_tx_sar_pwr_limit_static_data_t_v3;

typedef PACK(struct)
{
   int16 offset;
} rfnv_data_asdiv_char_offset_v1_type;

/* RFNV_DATA_TYPE_LTE_MTPL_VS_DLCA_VICTIM RFNV variant typeid 477 */
typedef PACK(struct)
{
  /*! Tx Aggressor band's minimum BW in KHz. Can be 0 */
  int32 aggressor_min_bw_khz;

  /*! Tx Aggressor band's maximum BW in KHz. */
  int32 aggressor_max_bw_khz;

  /*! Downlink victim Band number */
  uint32 victim_band_num;

  /*! Downlink victim band EARFCN. For single carrier it is the tuned EARFCN value.
  **  For CA cases it is the center value of the aggregated bandwidth */
  int32 victim_band_earfcn;

  /*! Tx Aggressor earfcn bin array */
  int32 aggressor_band_earfcn[RFNV_MAX_TX_AGGRESSOR_FREQ_BINS];

  /*! Tx Aggressor band's Max power per earfcn bin entry */
  int16 aggressor_max_power[RFNV_MAX_TX_AGGRESSOR_FREQ_BINS];
} rfnv_lte_mtpl_vs_dlca_victim_t;


/*   RFNV_DATA_TYPE_ENDC_AMPR                                  = 480,  */
/*--------------------------------------------------------------------------------------------------------------------*/
/*!
  @brief
  EN-DC AMPR LUT structure

*/

typedef PACK(struct)
{
  /*! IM3 frequency range table in KHz */
  uint32 im3_freq_khz_tbl[RFNV_AMPR_IM3_FREQ_RANGE_SIZE];

  /*! Number of allocated RB table  */
  uint8 rb_alloc_thres[RFNV_AMPR_RB_ALLOC_RANGE_SIZE];

  /*! EN-DC LUT  */
  uint8 ampr_tbl[RFNV_AMPR_IM3_FREQ_RANGE_SIZE][RFNV_AMPR_RB_ALLOC_RANGE_SIZE];

}rfnv_endc_ampr_lut_t;

/*--------------------------------------------------------------------------------------------------------------------*/
/*!
  @brief
  EN-DC AMPR static NV data type

*/
typedef PACK(struct)
{
  /*! IM3 SEM -13 AMPR LUT */
  rfnv_endc_ampr_lut_t sem_13_lut;

 /*! IM3 Spurious -25 AMPR LUT */
  rfnv_endc_ampr_lut_t spurious_25_lut;

}rfnv_endc_ampr_static_data_t;

/*--------------------------------------------------------------------------------------------------------------------*/
/*!
  @brief
  CA MAX POWER ADJUST NV data type

*/
typedef PACK(struct)
{
  /*! ca max power adjust enable */
  uint8 power_adjust_enable;

 /*! ca_max_power_adjut in db10 */
  int8 ca_max_power_adj_db10;

} rfnv_tx_ca_max_power_adjust_data_t;

/*   RFNV_DATA_TYPE_ENDC_AMPR_V2                                  = 489,  */
/*--------------------------------------------------------------------------------------------------------------------*/
/*!
  @brief
  EN-DC AMPR LUT structure

*/

typedef PACK(struct)
{
  /*! IM3 frequency range table in KHz */
  uint32 im3_freq_khz_tbl[RFNV_AMPR_IM3_FREQ_RANGE_SIZE_V2];

  /*! Number of allocated RB table  */
  uint8 rb_alloc_thres[RFNV_AMPR_RB_ALLOC_RANGE_SIZE_V2];

  /*! EN-DC LUT  */
  uint8 ampr_tbl[RFNV_AMPR_IM3_FREQ_RANGE_SIZE_V2][RFNV_AMPR_RB_ALLOC_RANGE_SIZE_V2];

}rfnv_endc_ampr_lut_t_v2;

/*--------------------------------------------------------------------------------------------------------------------*/
/*!
  @brief
  EN-DC AMPR static NV data type

*/
typedef PACK(struct)
{
  /*! IM3 SEM -13 AMPR LUT */
  rfnv_endc_ampr_lut_t sem_13_lut;

 /*! IM3 Spurious -25 AMPR LUT */
  rfnv_endc_ampr_lut_t_v2 spurious_25_lut;

}rfnv_endc_ampr_static_data_t_v2;

typedef PACK(struct)
{
  /* Number of active PA states */
  uint8 number_of_active_pa_states;

  /* PA Range Map NV */
  int16 pa_range_map[RFNV_MAX_PA_STATE];

  /* PA Hysterisis in us  */
  int16 pa_hysterisis_us;

  /* PA Fall switchpoint: 0-> Reserved | 1-> R1 fall | 2 -> R2 fall etc */
  int16 pa_fall_sw[RFNV_MAX_PA_STATE];

  /* PA Rise switchpoint: 0-> reserved | 1-> R1 rise | 2 -> R2 rise etc */
  int16 pa_rise_sw[RFNV_MAX_PA_STATE];

}rfnv_endc_pa_switch_t;

/* RFNV_DATA_TYPE_ENDC_PA_SWITCH_POINT RFNV variant typeid 490 to be used with Tx Static NV */
typedef PACK(struct)
{
  /* LTE 3GPP SPEC BAND NUMBER for all TX carriers */
  uint16 band;

  /*sub band number, such as B41A, B41B*/
  uint8 sub_band;

  /*LTE Modulation type  1->QPSK/2->16QAM/4->256QAM etc*/
  uint8 modulation_idx;

  /*!Carrier config : 0 - Single Carrier, 1 - Dual Carrier */
  uint8 carrier_config;

  /*Aggregated channel bandwidth mask for mpr backoff region */
  /* bit 0 - class A (NA), bit 1 - class B, bit 2 - class C, and so on */
  uint32 bandwidth_class;

  /*! Identifies all the Bandwidths supported for this MPR selection */
  uint32 bandwidth_type_mask;

  /* CA-Network Signalling Value */
  uint32 ca_ns_number;

  /*! TxAGC Control structure */
  rfnv_endc_pa_switch_t endc_pa_switch;

} rfnv_endc_pa_switch_static_data_t;


/*--------------------------------------------------------------------------------------------------------------------*/
/*!
  @brief
  LTE ESPR structure
  RFNV_DATA_TYPE_ESPR  511
  if bw is valid, not 0 or 0xFFFF, it is valid number.
*/
typedef PACK(struct)
{
  /*! bw table in 100KHz */
  uint16 bw_100khz[RFNV_ESPR_BW_RANGE_SIZE];
}rfnv_espr_static_data_t;

/* RFNV_DATA_TXFE_GAIN_OFFSET_STATIC_DATA = 499 */
typedef PACK(struct)
{
  /*! Txfe gain offset for max cal point of PA state */
  int8 txfe_gain_offset_max_db10;

  /*! Txfe gain offset for all other cal points */
  int8 txfe_gain_offset_default_db10[RFNV_MAX_PA_STATE];
}rfnv_txfe_gain_offset_static_data_t;

/* RFNV_DATA_CLIPPER_SETTINGS_STATIC_DATA = 508 */
typedef PACK(struct)
{
  /*! SPR APT TYPE - 00 for APTL, 01 for APTH/EPT/ET, 10 for all LTE xPT, 11 unused */
  uint8 clipper_settings[RFNV_MAX_PA_STATE];
}rfnv_clipper_settings_static_data_t;

/* RFNV_DATA_TYPE_XPT_ONLINE_DPD_MODE_V3 512 */
typedef PACK(struct)
{
  uint8 pa_state;
  uint32 min_bw_khz;
  uint32 max_bw_khz;
  uint8 online_dpd_mem_enable;
  uint8 online_dpd_kernel_index;
} rfnv_xpt_online_dpd_mode_v3_type;

/* RFNV_DATA_TYPE_XPT_ONLINE_DPD_MODE_V4 565 */
typedef PACK(struct)
{
/* PA state info */
uint8 pa_state;
/* online DPD mode - 0 = Disable, 1= Memory Enable ILA, 2= Memless Enable ILA, 3= Memory Enable DLA, 4 = Memless Enable DLA */
uint8 odpd_mode;
/* Kernel index information */
uint8 online_dpd_kernel_index;
/* Field to store modulation type */
uint8 modulation_mask;
/* Lower frequency limit */
uint32 low_freq_limit_khz;
/* Higher Frequency limit */
uint32 high_freq_limit_khz;
/* Lower bandwidth limit */
uint32 min_bw_khz;
/* Higher bandwidth limit */
uint32 max_bw_khz;
/* Field to override linear mask, renaming to avoid revealing actual field name externally*/
uint32 kernel_index_ext;
/* Reserved field for inputting debug info - eg. temperature */
uint8 temperature;
}rfnv_xpt_online_dpd_mode_v4_type;

/* RFNV_DATA_TYPE_XPT_ONLINE_DUPLEXER_EQ 514 */
typedef PACK(struct)
{
  uint8 enabled;
  uint8 pa_state;
  uint32 min_bw_khz;
  uint32 max_bw_khz;
  uint32 low_freq_limit_khz;
  uint32 high_freq_limit_khz;
} rfnv_xpt_online_duplexer_eq;

/* RFNV_DATA_TYPE_XPT_ONLINE_DUPLEXER_EQ_V2 566 */
typedef PACK(struct)
{
  /* PA state info */
  uint8 pa_state;
 /* Field for enable/disable; extend this for pre-dpd/post-dpd if needed*/
  uint8 odup_mode;
  /* Field to store modulation type */
  uint8 modulation_mask;
  /* Lower frequency limit */
  uint32 low_freq_limit_khz;
 /* Higher Frequency limit */
  uint32 high_freq_limit_khz;
 /* Higher bandwidth limit */
  uint32 min_bw_khz;
  /* Higher bandwidth limit */
  uint32 max_bw_khz;
  /* Reserved field for inputting debug info - eg. temperature */
  uint8 temperature;
}rfnv_xpt_online_duplexer_eq_v2;

/* RFNV_DATA_TYPE_XPT_CONFIG 585 */
typedef PACK (struct)
{
  uint32 config0;
  uint32 config1;
  uint32 config2;
  uint32 config3;
  uint32 config4;
  uint32 config5;
  uint32 config6;
  uint32 config7;
  uint32 config8;
  uint32 config9;

 }rfnv_xpt_config_type;


/* RFNV_DATA_TYPE_SAWLESS_STATE 516 */
typedef PACK(struct)
{
   uint8 sawless_state;
} rfnv_sawless_state;

/* RFNV_DATA_TYPE_ENDC_PA_SWITCH_POINT_AMPR 517 */
typedef PACK(struct)
{
  /*! Start BW range for PA switch points */
  uint16 start_bw_Mhz;
  /*! Start BW range for PA switch points */
  uint16 end_bw_Mhz;
  /* Number of active PA states */
  uint8 number_of_active_pa_states;
  /* PA Fall switchpoint: 0-> Reserved | 1-> R1 fall | 2 -> R2 fall etc */
  int16 pa_fall_sw[RFNV_MAX_PA_STATE];
  /* PA Rise switchpoint: 0-> reserved | 1-> R1 rise | 2 -> R2 rise etc */
  int16 pa_rise_sw[RFNV_MAX_PA_STATE];
} tx_pa_switchpoints_set_per_bw;

typedef PACK(struct)
{
  /*! Num of PA switchpoint sets based on BW */
  uint8 num_of_pa_swp_sets;
  /*! PA switch points values with band info*/
  tx_pa_switchpoints_set_per_bw pa_swp_set[RFNV_TX_MAX_PA_SWP_SETS];
} tx_pa_switchpoints_set;

typedef PACK(struct)
{
  /*! FALSE- If swp are for all LTE bands TRUE - if swp are band specific */
  uint8 band_specific;
  /*! LTE band num for ENDC mode */
  uint8 lte_band_num;
  /*! PA switch points for the NR-LTE band combos */
  tx_pa_switchpoints_set pa_swp_per_bw;
} tx_endc_pa_switchpoints_set;

typedef PACK(struct)
{
  /*! Num of PA switchpoint sets based on ENDC band combo */
  uint8 num_pa_swp_sets;
  tx_endc_pa_switchpoints_set endc_pa_swp_set[RFNV_TX_MAX_PA_SWP_SETS];
} tx_endc_pa_switchpoint_info;

typedef PACK(struct)
{
  /*! APT PA switch points for ENDC mode */
  tx_endc_pa_switchpoint_info endc_pa_swp_info_apt;
  /*! ET PA switch points for ENDC mode */
  tx_endc_pa_switchpoint_info endc_pa_swp_info_et;
  /*! IM backoff */
  uint16 bo_im_db10;
  /*! Emission bandwidth */
  uint32 e_bw_khz;
  /*! IM slope */
  uint16 im_slope_db10;
  /*! Transition threshold */
  uint16 b_thresh_db10;
  /*! Max AMPR in ENDC APT mode */
  uint16 bo_apt_db10;
  /*! IM backoff for ENDC AMPR region A and C */
  uint16 bo_im_ac_db10;
  /*! IM backoff for ENDC AMPR region B and D */
  uint16 bo_im_bd_db10;
} rfnv_endc_pa_switch_ampr_static_data_t;

/* RFNV_DATA_TYPE_PA_SWITCH_POINT_DYNAMIC_POWER_TRACKING  541 */
typedef PACK(struct)
{
  /*Number of active PA states */
  uint8 number_of_active_pa_states;
  /* PA Fall switchpoint: ET*/
  int16 pa_fall_sw_et[RFNV_MAX_PA_STATE];
  /* PA Rise switchpoint: ET*/
  int16 pa_rise_sw_et[RFNV_MAX_PA_STATE];
  /* PA Fall switchpoint: EPT */
  int16 pa_fall_sw_ept[RFNV_MAX_PA_STATE];
  /* PA Rise switchpoint: EPT */
  int16 pa_rise_sw_ept[RFNV_MAX_PA_STATE];
  /*! RB threshold - Dynamic Power Tracking */
  uint16 rb_threshold;
} rfnv_dynamic_power_tracking_static_data_t;

/* RFNV_DATA_TYPE_FDD_DYNAMIC_PA_STATE_MAPPING 580 */
typedef PACK(struct)
{
  /*Number of active PA states in either mapping table*/
  uint8 fdd_num_active_pa_states;
  /*physical pa state (real in cal NV) for FDD non-SMDL, i.e. SISO and SMSL*/
  uint8 fdd_non_smdl_phy_pa_state[RFNV_MAX_PA_STATE];
  /*physical pa state (real in cal NV) for FDD SMDL, where usually max pa state ---> APT*/
  uint8 fdd_smdl_phy_pa_state[RFNV_MAX_PA_STATE];
} rfnv_fdd_dynamic_pa_state_mapping_t;

/* 521 RFNV_DATA_TYPE_KEY_VALUE */
typedef PACK(struct)
{
  // NULL teminated strings, unused indexes filled with 0's
  uint8 key[RFNV_KEY_VALUE_MAX_KEY_SIZE];
  uint8 value[RFNV_KEY_VALUE_MAX_VALUE_SIZE];
} rfnv_key_value_t;

/* 528 RFNV_DATA_TYPE_SMART_ET_CTRL  */
typedef PACK(struct)
{
  /* */
  uint8 smart_et_opt_mode;

} rfnv_smart_et_ctrl_t;

/* RFNV_DATA_TYPE_TXAGC_CONSTANT_PAPR_CONTROL 537 */
typedef PACK(struct)
{
  uint32 mode;
} rfnv_txagc_constant_papr_ctrl_t;

/* RFNV_DATA_TYPE_DIVERSITY_PATH_ENABLE 586 */
typedef PACK(struct)
{
  uint8 div_enable;
} rfnv_div_enable_info_t;

/* RFNV_DATA_TYPE_TX_SAPT_S_FACTOR 584 */
typedef PACK(struct)
{
  uint16 s_factor;
} rfnv_tx_sapt_info_t;

/* RFNV_DATA_TYPE_TX_DAPT_ENABLE 594 */
typedef PACK(struct)
{
  uint8 enable_dapt;
} rfnv_tx_dapt_enable_t;


/* 543 RFNV_DATA_TYPE_ANTENNA_DETUNE_LIST */
typedef enum {
  DETUNE_OPEN = 0,
  DETUNE_SHORT = 1,
  DETUNE_LOAD = 2,
  DETUNE_MAX = 255
} rfnv_antenna_detune_type;

typedef PACK(struct)
{
  uint16 antenna;
  uint8  detune_type;
} rfnv_antenna_detune_list_entry;

typedef PACK(struct)
{
  uint8 antenna_list_size;
  rfnv_antenna_detune_list_entry antenna_list[RFNV_ANTENNA_DETUNE_LIST_SIZE];
} rfnv_antenna_detune_list;

// RFNV_DATA_TYPE_TX_DCOC_CAL 549
typedef PACK(struct)
{
  uint32 bw;
  uint32 dc_i;
  uint32 dc_q;
} rfnv_tx_dcoc_cal;

// RFNV_GSM_TX_VCC_CAP 552
typedef PACK(struct)
{
  int8 therm_decC;
  uint16 vcc_mv;
} rfnv_gsm_tx_vcc_cap;

// Variant type RFNV_XPT_MLINE_RES_V10_0 */
typedef PACK(struct)
{
  uint8 pa_state;
  uint8 sweep; //rfnv_tx_lin_sweep_type
  uint32 bandwidth;
  uint32 frequency;
  int32 alpha_vin;
  int32 beta_vin;
  int32 alpha_vcc;
  int32 beta_vcc;
} rfnv_data_type_mline_results_v10_0;

/* RFNV_DATA_TYPE_WCN_COEX_DESENSE_POWER_LIMIT_CONTROL RFNV variant to be used with Tx Static NV */
typedef PACK(struct)
{
  /*! min tx freq in KHz. set -1 as wildcard */
  int32 min_freq_khz;

  /*! max tx freq in KHz. set -1 as wildcard */
  int32 max_freq_khz;

  /*! min tx bandwidth in KHz. set -1 as wildcard */
  int32 min_bw_khz;

  /*! max tx bandwidth in KHz. set -1 as wildcard */
  int32 max_bw_khz;

  /* wcn_desense index from victim table, 0 is invalid */
  uint16 wcn_desense_idx;

  /* wcn desense plimit list for all ant ports in dB10
     set 32767 = MAX_INT16 if there is no limitation in the ant port */
  int16 wcn_desense_plimit_db10[RFNV_RFM_ANTENNA_SIZE];
}rfnv_wcn_coex_desense_list_entry_t;

typedef PACK(struct)
{
  /*! wcn coex desense list */
  rfnv_wcn_coex_desense_list_entry_t wcn_coex_desense_list[RFNV_WCN_COEX_DESENSE_LIST_SIZE];
}rfnv_wcn_coex_desense_power_limit_control_list_t;

typedef PACK (struct)
{
	uint8 fbrx_vcm_cal_reg_data;

}rfnv_data_fbrx_vcm_cal_item_type;

/*! NV 588 RFNV_DATA_TYPE_TX_RGI_LIMIT */
typedef PACK (struct)
{
  /* max rgi value per pa state. 
     set value 0xFFFF and 0 meaning no rgi pruning
  */
	uint16 max_rgi_value[RFNV_MAX_PA_STATE];
}rfnv_data_type_tx_rgi_limit_t;

#if defined(_MSC_VER) || defined(__MINGW32__)
/*
  }
  END of section to make visual studio happy in order for "PACK"
  to work properly for off-target testing.
*/
#pragma pack( pop )
#endif


#ifdef __cplusplus
}
#endif

#endif
