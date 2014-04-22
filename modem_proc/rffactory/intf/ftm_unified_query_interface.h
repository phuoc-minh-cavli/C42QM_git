#ifndef FTM_UNIFIED_QUERY_INTERFACE_H
#define FTM_UNIFIED_QUERY_INTERFACE_H
/*!
  @file
  ftm_unified_query_interface.h

  @brief
  Common framework to perform FTM Unified Query
*/

/*======================================================================================================================

  Copyright (c) 2016 - 2022 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by this document are confidential
  and proprietary information of Qualcomm Technologies Incorporated and all rights therein are
  expressly reserved. By accepting this material the recipient agrees that this material and the
  information contained therein are held in confidence and in trust and will not be used, copied,
  reproduced in whole or in part, nor its contents revealed in any manner to others without the
  express written permission of Qualcomm Technologies Incorporated.

======================================================================================================================*/

/*======================================================================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/rffactory.mpss/1.0/intf/ftm_unified_query_interface.h#63 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------------------------------------------------
09/30/22   adawn   Add mmW HPM beamer mask to the uti query
06/01/22   abr     Add support for mmW cal path info
03/21/22   yanwa   Support LTE sub-band frequency range query.
04/30/21   qq      Added FTM UNIFIED RF PATH Database information command
11/05/20   adawn   Num DMRS Symbols param added to FTM UNIFIED TX INFORMATION command
01/10/19   abr     Add antenna module type information to HW connection table query's property list   
11/08/18   ude     Adding LINK_ID and UL_TYPE properties to CA_CAPABILITY_QUERY command
10/28/18   sb      Adding paired beam ID prop for Subarray query
10/23/18   dpk     Adding Band, Channel and BW info for ACA response
10/18/18   abr     Define property type for sub-band frequency information in HW connection table query property list 
09/07/18   sn      Adding new type to return frequency range for unified query command.
06/01/18   WXL     FR 48638 - Request to return BCS and associated BW_MASK in CA Combo Query command response 
04/24/18   abr     Added properties to support v1 response of HW connection query
04/10/18   abr     Added Antenna Relative Positions properties to Sub Array query property enum
03/16/18   kk      Added IDC path properties to the list of properties in Tx Rx RF Path info Command. 
03/07/18   abr     Remove properties representing number of L1/L2/L3 beams from Sub Array query property enum
01/31/18   dn      CR2181538: RFFE V3 addition
01/18/18   abr     Replace antenna feed mask with antenna feed pin in Sub-Array information query property list
01/03/18   abr     Add Beam List properties to Sub-Array information query property list
10/05/17   pkg     Added UE Power Class support for UTI query in FTM.
10/02/17   sn      Added new command type enum for Tx Rx Cal path info command.
08/22/17   zhw     Added support for PLL ID in ACA command
08/17/17   dyx     logging txagc when received FTM UTI command with property 23
08/14/17   abr     Update property type enum definitions for Sub-Array information
07/13/17   abr     Add enum and property ID list for Sub-Array information
07/10/17   zbz     Added modulation and tx slots number to the input of UTI query for GSM
02/14/17   dyx     Support UTI query with property 20
12/02/16   kpa     PORT B Tx power can be negative
11/09/16   zhw     Added CA combo query support
11/08/16   kpa     FTM UNIFIED TX INFORMATION query with all the PCLs
07/11/16   dyx     Add Action - Query NTPL into UTI API
08/18/16   dyx     Add Unified Query Tx Info subcommand related API
08/17/16   chm     Add Active Client Allocations command
06/07/15   jfc     Initial Release

======================================================================================================================*/

#include "comdef.h"

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef T_WINNT
#error code not present
#endif

#define RFNV_NUMBER_OF_PCLS 16

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Type definition to define RFM device mask */
typedef uint16 ftm_unified_query_rfm_device_mask_type;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Type definition to define chain */
typedef uint32 ftm_unified_query_chain_type;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Type definition to define carrier */
typedef uint32 ftm_unified_query_carrier_type;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Type definition to define carrier mask */
typedef uint8 ftm_unified_query_carrier_mask_type;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Type definition to define band */
typedef uint32 ftm_unified_query_band_type;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Type definition to define channel */
typedef uint32 ftm_unified_query_channel_type;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Type definition to define bandwidth */
typedef uint32 ftm_unified_query_bandwidth_type;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Type definition for Unified Query's  rfm_device.  */
typedef uint32 ftm_unified_query_rfm_device_type;

/*! Type definition for unified_query's sig_path.  */
typedef uint32 ftm_unified_query_sig_path_type;

/*! Type definition for unified_query's antenna_path.  */
typedef uint16 ftm_unified_query_antenna_path_type;

/*! Type definition for unified_query's antenna_path.  */
typedef int16 ftm_unified_gsm_pcl_to_pwr_tbl_type[RFNV_NUMBER_OF_PCLS];

/*! Type definition for unified_query's pfir nv.  */
typedef uint8 ftm_unified_query_pfir_nv;

/*! Type definition for GSM specific modulation type.  */
typedef enum
{
  FTM_UNIFIED_QUERY_MOD_GMSK = 0,
  FTM_UNIFIED_QUERY_MOD_8PSK = 1,
  FTM_UNIFIED_QUERY_MOD_NUM
}ftm_unified_query_modulation_type;

/*! Type definition for GSM specific number of contiguous tx slots.  */
typedef uint16 ftm_unified_query_num_contiguous_tx_slots_type;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Type definition to define the size of the version field in unified_query command packet.  */
typedef uint32 ftm_unified_query_version_type;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Type definition to define the error code mask for response packet. Each bit indicates a type of error. */
typedef uint32 ftm_unified_query_error_code_mask_type;

/*====================================================================================================================*/
/*!
  @addtogroup FTM_TEST_TOOLS_CID
  @{
*/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Enumeration to indicate the type of commands available for FTM Unified Query command */
typedef enum
{
  FTM_UNIFIED_QUERY_CMD_UNASSIGNED = 0, /*!< 0 : Unassigned command */

  FTM_UNIFIED_QUERY_CMD_QUERY_RECOMMENDED_RADIO_ALLOCATION = 1, /*!< 1 : RF Radio Allocation command */

  FTM_UNIFIED_QUERY_CMD_QUERY_ACTIVE_CLIENT_ALLOCATION = 2, /*!< 2 : RF Active Client Allocation command */

  FTM_UNIFIED_QUERY_CMD_QUERY_UNIFIED_TX_INFO = 3,  /*!< 3 : RF Unified Tx Info Query subcommand */

  FTM_UNIFIED_QUERY_CMD_QUERY_CA_CAPABILITY = 4,  /*!< 4 : RF CA capability query subcommand */

  FTM_UNIFIED_QUERY_CMD_QUERY_HW_CONNECTION = 5,  /*!< 5 : Hardware connection query subcommand */

  FTM_UNIFIED_QUERY_CMD_QUERY_SUB_ARRAY_INFO = 6, /*!< 6 : Sub-Array table query subcommand */
  
  FTM_UNIFIED_QUERY_CMD_TX_RX_CAL_PATH_INFO = 7, /*!< 7 : Tx Rx Cal Path Info query subcommand */

  FTM_UNIFIED_QUERY_CMD_RFFE_DEV_INFO = 8, /*!< 8 : RFFE device Info query subcommand */

  FTM_UNIFIED_QUERY_CMD_QUERY_MPE_BEAM_INFO = 9, /*!< 9 : MPE Beam Info query subcommand */

  FTM_UNIFIED_QUERY_CMD_CAL_INPUT_PARAMS_INFO = 10, /*!< 10: Cal Input Params Info Query subcommand */

  FTM_UNIFIED_QUERY_CMD_QUERY_DEV_ATTRIBUTES = 11, /*!< 11 : BOARD ID query subcommand */

  FTM_UNIFIED_QUERY_CMD_QUERY_TX_OVERRIDE_PARAMS = 12, /*!< 12: TX override param query subcommand */
  
  FTM_UNIFIED_QUERY_CMD_QUERY_RFFE_DEV_BW_INFO = 14,  /*!< 14 : RFFE device BW information Query subcommand */

  FTM_UNIFIED_QUERY_CMD_QUERY_CAL_DB_INFO = 15,  /*!< 15 : Cal DB information Query subcommand */

  FTM_UNIFIED_QUERY_CMD_QUERY_CMN_RFFE_DEV_INFO = 16,  /*!< 16: RFFE device common information Query subcommand */

  FTM_UNIFIED_QUERY_CMD_RX_SIG_PATH_FREQ_INFO = 17, /*!< 17 : Retrieve frequency ranges for individual signal paths (Small Cell FSM) */

  FTM_UNIFIED_QUERY_CMD_QUERY_MMW_CAL_PATH_INFO = 18, /*!< 17: MMW Calibration Path Information */

  FTM_UNIFIED_QUERY_CMD_QUERY_MMW_RX_INFO = 19, /*!< 19: MMW Rx Offset information */

  FTM_UNIFIED_QUERY_CMD_NUM  /*!< Max : Defines maximum number of command IDs */

} ftm_unified_query_command_enum_type;

/*! @} */

/*====================================================================================================================*/
/*!
  @name Unified TX Info Property ID

  @brief
  Unified TX Info Property Property ID list
*/
/*! @{ */

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Enumeration to indicate the type of properties for Recommended Radio Allocations Command.
  These enumeration are used to define the content of test command packet.
  ftm_trm_rra_property_names[] Must be updated when this list is updated.*/
typedef enum
{
  FTM_UNIFIED_TX_INFO_PROP_UNASSIGNED          = 0,  /*!< Unassigned property */
  FTM_UNIFIED_TX_INFO_PROP_SUB_IDX             = 1,  /*!< Subscription Index from ftm_rf_test_subscriber_enum_type */
  FTM_UNIFIED_TX_INFO_PROP_TECH                = 2,  /*!< Tech from ftm_rf_technology_type values */
  FTM_UNIFIED_TX_INFO_PROP_BAND                = 3,  /*!< Band from sys_band_class_e_type values */
  FTM_UNIFIED_TX_INFO_PROP_CHAN                = 4,  /*!< Chan */
  FTM_UNIFIED_TX_INFO_PROP_BANDWIDTH           = 5,  /*!< Bandwidth in Hz */
  FTM_UNIFIED_TX_INFO_PROP_CARRIER_IDX         = 8,  /*!< Carrier Index 0/1/2/n */
  FTM_UNIFIED_TX_INFO_PROP_DEVICE              = 12, /*!< Device from rfm_device_enum_type values */
  FTM_UNIFIED_TX_INFO_PROP_SIGNAL_PATH         = 13, /*!< Signal */
  FTM_UNIFIED_TX_INFO_PROP_ANTENNA_PATH        = 14, /*!< Antenna Path */
  FTM_UNIFIED_TX_INFO_PROP_ACTION_MAX_TX_POWER        = 15, /*!< Target Max Tx Power*/
  FTM_UNIFIED_TX_INFO_PROP_ACTION_PCL_TO_PWR_MAPPING  = 16, /*!< Action: if property present, return the PCL to PWR mapping table, GSM specific*/  
  FTM_UNIFIED_TX_INFO_PROP_PCL_INDEX	              = 17, /*!< Response only, PCL index, GSM specific*/	
  FTM_UNIFIED_TX_INFO_PROP_PCL_SPECIFIC_PWR           = 18, /*!< Response only, TX PWR corresponding PCL index, GSM specific */  
                                                    
  FTM_UNIFIED_TX_INFO_PROP_ACTION_NTPL                = 19,     /*!< Max Tx Power from NV*/
  FTM_UNIFIED_TX_INFO_PROP_ACTION_ULCA_COMBO_MTPL     = 20,     /*!< Combined MTPL under ULCA scenario*/
  FTM_UNIFIED_TX_INFO_PROP_MODULATION_TYPE            = 21,     /*!< Modulation type GSM specific*/
  FTM_UNIFIED_TX_INFO_PROP_NUM_CONTIGUOUS_TX_SLOTS    = 22,     /*!< Number of contiguous Tx slots GSM specific*/
  FTM_UNIFIED_TX_INFO_PROP_ACTION_TXAGC_INFO_LOGGING  = 23,     /*!< Logging TXAGC related info while receiving this action*/
  FTM_UNIFIED_TX_INFO_PROP_ACTION_GET_UE_POWER_CLASS  = 24,     /*!< Get Supported UE power classes as a bitmask */
  FTM_UNIFIED_TX_INFO_PROP_ACTION_GET_PFIR_NV  = 25,     /*!< Get value of Pfir_nv for FBRx Droop Char */
  FTM_UNIFIED_TX_INFO_PROP_ACTION_GET_NUM_DMRS_SYMBOLS = 26,     /*!< Get number of DMRS symbols */
  FTM_UNIFIED_TX_INFO_PROP_MAX_NUM_ELEMENTS = 27, /*! MMW specific, max number of elements*/
  FTM_UNIFIED_TX_INFO_PROP_ACTIVE_NUM_ELEMENTS = 28, /*! MMW specific, active number of elements for current beam id*/
  FTM_UNIFIED_TX_INFO_PROP_ACTIVE_BMR_MASK = 29, /*! MMW specific, active bmr mask for current beam id*/
  FTM_UNIFIED_TX_INFO_PROP_MTPL_DEL_DB10 = 30,   /*! MMW specific, for Power class5*/
  FTM_UNIFIED_TX_INFO_PROP_COMBO_POWER_CLASS = 31, /*! Power class for combo */
  FTM_UNIFIED_TX_INFO_PROP_HPM_PSR_MASK = 32, /*! MMW specific, HPM beamer mask */
  FTM_UNIFIED_TX_INFO_PROP_TXD = 33, /*Sub6 specific, txDiversity_r16 indication*/
  /* ADD MORE ITEMS ABOVE THIS LINE */
  FTM_UNIFIED_TX_INFO_PROP_NUM                      /*!< Max : Defines maximum number of properties */

} ftm_unified_tx_info_property_type;

extern const char *ftm_unified_tx_info_property_names[];

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Enumeration to indicate the type of subscribers */
typedef enum
{
  FTM_UNIFIED_QUERY_SUBSCRIBER_0 = 0, /*!< 0 : First subscriber */

  FTM_UNIFIED_QUERY_SUBSCRIBER_1 = 1, /*!< 1 : Second subscriber */

  FTM_UNIFIED_QUERY_SUBSCRIBER_NUM,  /*!< Max : Defines maximum number of subscriber IDs */

  FTM_UNIFIED_QUERY_SUBSCRIBER_NA = 0xFF  /*!< Not Applicable */

} ftm_unified_query_subscriber_enum_type;

/*====================================================================================================================*/
/*!

  @name Active Client Allocations Property ID

  @brief
  Active Client Allocations Property ID list
*/
/*! @{ */
      
/*--------------------------------------------------------------------------------------------------------------------*/
/*! Enumeration to indicate the type of properties for Active Client Allocations Command. 
  These enumeration are used to define the content of test command packet. 
  ftm_trm_aca_property_names[] Must be updated when this list is updated.*/
typedef enum
{
  FTM_TRM_ACA_PROP_UNASSIGNED          = 0,  /*!< Unassigned property */
  FTM_TRM_ACA_PROP_SUB_IDX             = 1,  /*!< Subscription Index from ftm_rf_test_subscriber_enum_type */
  FTM_TRM_ACA_PROP_TECH                = 2,  /*!< Tech from ftm_rf_technology_type values */
  FTM_TRM_ACA_PROP_RXTX                = 3,  /*!< Indicates if request is for Rx or Tx path from ftm_rf_test_device_radio_trx_t*/
  FTM_TRM_ACA_PROP_CHAIN               = 4,  /*!< Chain 0/1/2/3/n. 0:PRx/Tx 1: DRx >= 2: HO_RxD*/
  FTM_TRM_ACA_PROP_CARRIER_IDX         = 5,  /*!< Carrier Index 0/1/2/n */
  FTM_TRM_ACA_PROP_DEVICE              = 6,  /*!< Device from rfm_device_enum_type values */
  FTM_TRM_ACA_PROP_SIGNAL_PATH         = 7,  /*!< Signal */
  FTM_TRM_ACA_PROP_ANTENNA_PATH        = 8,  /*!< Antenna Path */
  FTM_TRM_ACA_PROP_ANTENNA_NUM         = 9,  /*!< Number of Antennas */
  FTM_TRM_ACA_PROP_CURR_ASDIV_CFG      = 10,  /*!< Current Antenna config for ASDIV, i.e. Port state from trm_ant_switch_state_e_type */
  FTM_TRM_ACA_PROP_CURR_CAL_STATE      = 11,  /*!< Current Antenna Cal state for ASDIV,  */
  FTM_TRM_ACA_PROP_PENDING_ASDIV_CFG   = 12,  /*!< Pending Antenna config for ASDIV */
  FTM_TRM_ACA_PROP_PENDING_CAL_STATE   = 13,  /*!< Pending Antenna Cal state for ASDIV */
  FTM_TRM_ACA_PROP_PLL_ID              = 14,  /*!< PLL ID */
  FTM_TRM_ACA_PROP_BAND                = 15,  /*!< BAND */
  FTM_TRM_ACA_PROP_CHANNEL             = 16,  /*!< CHANNEL */
  FTM_TRM_ACA_PROP_BW                  = 17,  /*!< BW */

  /* ADD MORE ITEMS ABOVE THIS LINE */
  FTM_TRM_ACA_PROP_NUM  /*!< Max : Defines maximum number of properties */

} ftm_trm_aca_property_type;

extern const char *ftm_trm_aca_property_names[];

/*====================================================================================================================*/
/*!
  @name Unified TX Info Property ID

  @brief
  Unified TX Info Property Property ID list
*/
/*! @{ */

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Enumeration to indicate the type of properties for Recommended Radio Allocations Command.
  These enumeration are used to define the content of test command packet.
  ftm_trm_rra_property_names[] Must be updated when this list is updated.*/
typedef enum
{
  FTM_CA_COMBO_QUERY_PROP_UNASSIGNED           = 0, /*!< Unassigned property */
  FTM_CA_COMBO_QUERY_PROP_TECH                 = 1, /*!< FTM Technology */
  FTM_CA_COMBO_QUERY_PROP_TOTAL_COMBO_COUNT    = 2, /*!< Total count of CA combos available */
  FTM_CA_COMBO_QUERY_PROP_COMBO_INDEX          = 3, /*!< Index of the current CA combo.  */
  FTM_CA_COMBO_QUERY_PROP_ANTENNA_LIMIT        = 4, /*!< Max Number of concurrently active DL Antennas supported */
  FTM_CA_COMBO_QUERY_PROP_BAND                 = 5, /*!< RF band */
  FTM_CA_COMBO_QUERY_PROP_DL_BW_CLASS          = 6, /*!< DL Bandwidth class supported by the current band */
  FTM_CA_COMBO_QUERY_PROP_DL_ANTENNAS          = 7, /*!< Maximum number of DL antennas supported by the current band */
  FTM_CA_COMBO_QUERY_PROP_UL_BW_CLASS          = 8, /*!< UL Bandwidth class supported by the current band */
  FTM_CA_COMBO_QUERY_PROP_UL_ANTENNAS          = 9, /*!< Maximum number of UL antennas supported by the current band */
  FTM_CA_COMBO_QUERY_PROP_BW_PER_CARRIER       = 10,/*!< Max Bandwidth supported per cc per BCS */
  FTM_CA_COMBO_QUERY_PROP_LINK_ID              = 11,/*!< An index relating multiple band entries in the group as being intra-band contiguous by sharing the same ID*/
  FTM_CA_COMBO_QUERY_PROP_UL_TYPE              = 12,/*!< Indicates whether Tx is of intra-band contiguous configuration if Rx is*/
  /* ADD MORE ITEMS ABOVE THIS LINE */
  FTM_CA_COMBO_QUERY_PROP_NUM                      /*!< Max : Defines maximum number of properties */

} ftm_ca_combo_query_property_type;

extern const char *ftm_ca_combo_query_property_names[];

/*====================================================================================================================*/
/*!
  @name Unified HW Connection Property ID

  @brief
  Unified HW Connection Property ID list
*/
/*! @{ */

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Enumeration to indicate the type of properties for HW Connection Command.
  These enumeration are used to define the content of test command packet.
  ftm_hw_conn_property_names[] Must be updated when this list is updated.*/
typedef enum
{
  FTM_HW_CONN_QUERY_PROP_UNASSIGNED             = 0,  /*!< Unassigned property */
  FTM_HW_CONN_QUERY_PROP_BAND                   = 1,  /*!< Band number */
  FTM_HW_CONN_QUERY_PROP_SUBBAND                = 2,  /*!< Sub-band number */
  FTM_HW_CONN_QUERY_PROP_RAW_RESULT             = 3,  /*!< Raw response data */
  FTM_HW_CONN_QUERY_PROP_IF_TRX_INFO            = 4,  /*!< IF TRx information */
  FTM_HW_CONN_QUERY_PROP_RFTRX_ANTMOD_INTF_INFO = 5,  /*!< RF TRx - Antenna Module interface information */
  FTM_HW_CONN_QUERY_PROP_ANT_MODULE_INFO        = 6,  /*!< Antenna Module information */
  FTM_HW_CONN_QUERY_PROP_SUBBAND_FREQ_INFO      = 7,  /*!< Sub-band frequency information */
  FTM_HW_CONN_QUERY_PROP_ANT_MODULE_TYPE        = 8,  /*!< Antenna Module type */
  FTM_HW_CONN_QUERY_PROP_TXRX_CAL_FEED_INFO     = 9,  /*!< Tx-Rx Cal Feed information */
  FTM_HW_CONN_QUERY_PROP_PLATFORM_TYPE          = 10, /*!< Platform type */
  FTM_HW_CONN_QUERY_PROP_NUM                         /*!< Max : Defines maximum number of properties */
} ftm_hw_conn_query_property_type;

extern const char *ftm_hw_conn_query_property_names[];

/*====================================================================================================================*/
/*!
  @name Unified Sub Array Property ID

  @brief
  Unified Sub Array Property ID list
*/
/*! @{ */

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Enumeration to indicate the type of properties for Sub Array Command.
  These enumeration are used to define the content of test command packet.
  ftm_sub_array_info_property_names[] Must be updated when this list is updated.*/
typedef enum
{
  FTM_SUB_ARRAY_INFO_QUERY_PROP_UNASSIGNED            = 0, /*!< Unassigned property */
  FTM_SUB_ARRAY_INFO_QUERY_PROP_SUB_ARRAY_ID          = 1, /*!< Sub-Array ID */
  FTM_SUB_ARRAY_INFO_QUERY_PROP_RF_TRX_ID             = 2, /*!< RF TRX ID */
  FTM_SUB_ARRAY_INFO_QUERY_PROP_ANT_GRP_ID            = 3, /*!< Antenna Group ID */
  FTM_SUB_ARRAY_INFO_QUERY_PROP_ANT_FEED_PIN          = 4, /*!< Antenna Feed Pin */
  FTM_SUB_ARRAY_INFO_QUERY_PROP_POL_TYPE              = 5, /*!< Polarization Type */
  FTM_SUB_ARRAY_INFO_QUERY_PROP_BAND_MASK             = 6, /*!< Band Mask */
  FTM_SUB_ARRAY_INFO_QUERY_PROP_AZIMUTH_START         = 7, /*!< Azimuth start angle (deg) */
  FTM_SUB_ARRAY_INFO_QUERY_PROP_AZIMUTH_STOP          = 8, /*!< Azimuth stop angle (deg) */
  FTM_SUB_ARRAY_INFO_QUERY_PROP_ELEVATION_START       = 9, /*!< Elevation start angle (deg) */
  FTM_SUB_ARRAY_INFO_QUERY_PROP_ELEVATION_STOP        = 10, /*!< Elevation stop angle (deg) */
  FTM_SUB_ARRAY_INFO_QUERY_PROP_BEAM_ID               = 11, /*!< Beam ID*/
  FTM_SUB_ARRAY_INFO_QUERY_PROP_BEAM_SUB_BAND         = 12, /*!< Beam Sub Band*/
  FTM_SUB_ARRAY_INFO_QUERY_PROP_BEAM_LIST_QUERY_FLAG  = 13, /*!< Beam List Query Flag */
  FTM_SUB_ARRAY_INFO_QUERY_PROP_ANT_FEED_X_COORDINATE = 14, /*!< Antenna Feed Pin X Coordinate*/
  FTM_SUB_ARRAY_INFO_QUERY_PROP_ANT_FEED_Y_COORDINATE = 15, /*!< Antenna Feed Pin Y Coordinate*/
  FTM_SUB_ARRAY_INFO_QUERY_PROP_ANT_FEED_Z_COORDINATE = 16, /*!< Antenna Feed Pin Z Coordinate*/
  FTM_SUB_ARRAY_INFO_QUERY_PROP_PAIRED_BEAM_ID        = 17, /*!< Paired beam ID*/
  FTM_SUB_ARRAY_INFO_QUERY_PROP_ZEB_ID                = 18, /*!< Zero Element Beam ID*/
  FTM_SUB_ARRAY_INFO_QUERY_PROP_NUM_ACTIVE_ELEMENTS_PER_BEAM  = 19, /*!< Number of Active Elements per Beam*/
  FTM_SUB_ARRAY_INFO_QUERY_PROP_REF_BEAM_LIST_QUERY_FLAG = 20,
  FTM_SUB_ARRAY_INFO_QUERY_PROP_REF_BEAM_ID = 21,
  FTM_SUB_ARRAY_INFO_QUERY_PROP_REF_BEAM_RF_TRX_MASK = 22,
  FTM_SUB_ARRAY_INFO_QUERY_PROP_NUM                         /*!< Max : Defines maximum number of properties */
} ftm_sub_array_info_query_property_type;

extern const char *ftm_sub_array_info_query_property_names[];

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Enumeration to indicate the type of properties for Tx Rx RF Path info Command.*/
typedef enum
{
    FTM_RF_PATH_INFO_TX_RX_TECH_PROP                   = 0,  /*!< Technology*/
    FTM_RF_PATH_INFO_TX_RX_VERSION_PROP                = 1,  /*!< Version of the response*/
    FTM_RF_PATH_INFO_TX_RX_BAND_PROP                   = 2,  /*!< Band Property */
    FTM_RF_PATH_INFO_TX_RX_SUB_BAND_PROP               = 3,  /*!< Sub Band Property */
    FTM_RF_PATH_INFO_TX_RX_PATH_ID_PROP                = 4,  /*!< Path Property */
    FTM_RF_PATH_INFO_TX_RX_RFM_PATH_PROP               = 5,  /*!< RFM Path Blob Property */
    FTM_RF_PATH_INFO_TX_RX_COMMON_PATH_PROP            = 6,  /*!< Common Path Blob Property */
    FTM_RF_PATH_INFO_TX_RX_ANT_PROP                    = 7,  /*!< Antenna Blob Property */
    FTM_RF_PATH_INFO_TX_RX_CAL_ACTION_PROP             = 8,  /*!< TX/RX Cal action property */
    FTM_RF_PATH_INFO_TX_RX_ASSOCIATED_RX_PROP          = 9,  /*!< Associated Rx Blob Property */
    FTM_RF_PATH_INFO_TX_RX_ASSOCIATED_TX_PROP          = 10, /*!< Associated Tx Blob Property */
    FTM_RF_PATH_INFO_TX_RX_COMMON_BAND_PROP            = 11, /*!< Comon Bands Blob Property */
    FTM_RF_PATH_INFO_TX_RX_CAL_MAX_PROP                = 12, /*!< deprecate */
    FTM_RF_PATH_INFO_TX_RX_IDC_CAL_TYPE_BITMASK_PROP   = 13, /*!< IDC cal Bitmask property */
    FTM_RF_PATH_INFO_TX_RX_IDC_CAL_TYPE_PROP           = 14, /*!< IDC Cal Type property */
    FTM_RF_PATH_INFO_TX_RX_IDC_CAL_BW_PROP             = 15, /*!< IDC Cal BW property for all paths in a band*/
    FTM_RF_PATH_INFO_TX_RX_START_STOP_FREQ_TYPE        = 16, /*!< Frequency range for a band */
    FTM_RF_PATH_INFO_TX_RX_NDR_CAL_INFO                = 17, /*!< NDR Cal info */
    FTM_RF_PATH_INFO_TX_RX_ENABLE_COMMON_CAL_PROP      = 18, /*!< Common Cal prop */
    FTM_RF_PATH_INFO_TX_RX_SDR_INDEX_PROP              = 19, /*!< SDR Index Property */ 
    FTM_RF_PATH_INFO_FBRX_CAL_ACTION_PROP              = 20, /*!< FBRx Cal Action Property */
    FTM_RF_PATH_INFO_TX_RX_ENABLE_FBRX_COMMON_CAL_PROP = 21, /*!< FBRx Common cal Prop */
    FTM_RF_PATH_INFO_FBRX_PATH_PROP                    = 22, /*!< FBRx Path Property */
    FTM_RF_PATH_INFO_TX_RX_PHY_ANT_ARAY_PROP           = 23, /*!< Phy Ant Swt Property*/
    FTM_RF_PATH_INFO_LNA_INFO_BITMASK_PROP             = 24, /*!< LNA info Bitmask property */
    FTM_RF_PATH_INFO_LNA_INFO_TYPE_PROP                = 25, /*!< LNA info Type property */
    FTM_RF_PATH_INFO_LNA_INFO_DATA_PROP                = 26, /*!< LNA info Data property */
    FTM_RF_PATH_INFO_TX_RX_BANDWIDTH_PROP              = 27, /*!< Bandwidth property */
    FTM_RF_PATH_INFO_HL_MODE_FLAG_PROP                 = 28, /*!< mLNA indication */
    FTM_RF_PATH_INFO_TX_RX_REF_IDX_PATH_PROP           = 29, /*!< Reference signal path index */
    FTM_RF_PATH_INFO_TX_RX_REF_SIGNAL_PATH_CHECK_PROP  = 30, /*!< Reference signal path Common Cal check */
    FTM_RF_PATH_INFO_TX_RX_CAL_INVALID_PROP                  /*!< Invalid Property type */

}ftm_rf_path_info_tx_rx_property_type;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Enumeration to indicate the type of properties for mmW RF Calibration Path info Command.*/
typedef enum
{
	FTM_MMW_CAL_PATH_INFO_QUERY_PROP_UNASSIGNED       = 0, /*!< Unassigned property */
    FTM_MMW_CAL_PATH_INFO_QUERY_PROP_BAND             = 1, /*!< Band number */
	FTM_MMW_CAL_PATH_INFO_QUERY_PROP_PATH_ID          = 2, /*!< Path ID */
	FTM_MMW_CAL_PATH_INFO_QUERY_PROP_ANT_GRP_ID       = 3, /*!< Antenna Group ID */
	FTM_MMW_CAL_PATH_INFO_QUERY_PROP_RFM_DEVICE       = 4, /*!< RFM Device */
	FTM_MMW_CAL_PATH_INFO_QUERY_PROP_PATH_TYPE        = 5, /*!< Logical Path Type */
	FTM_MMW_CAL_PATH_INFO_QUERY_PROP_ASSOC_TX_PATH_ID = 6, /*!< Associated Tx Path ID */
	FTM_MMW_CAL_PATH_INFO_QUERY_PROP_ASSOC_RX_PATH_ID = 7, /*!< Associated Rx Path ID */
	FTM_MMW_CAL_PATH_INFO_QUERY_PROP_NUM                   /*!< Max : Defines maximum number of properties */

} ftm_mmw_cal_path_info_property_type;

extern const char *ftm_mmw_cal_path_info_query_property_names[];

/*====================================================================================================================*/
/*!
  @name Unified mmw rx info Property ID

  @brief
  Unified mmw rx info Property ID list
*/
/*! @{ */

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Enumeration to indicate the type of properties for mmw rx info Command.
  These enumeration are used to define the content of test command packet.
  ftm_mmw_rx_info_property_names[] Must be updated when this list is updated.*/
typedef enum
{
  FTM_MMW_RX_INFO_QUERY_PROP_UNASSIGNED            = 0, /*!< Unassigned property */
  FTM_MMW_RX_INFO_QUERY_PROP_RX_OFFSET_QUERY_FLAG  = 1, /*!< Rx offset list Query Flag */
  FTM_MMW_RX_INFO_QUERY_PROP_NF_QUERY_FLAG         = 2, /*!< NF table Query Flag */
  FTM_MMW_RX_INFO_QUERY_PROP_BAND                  = 3, /*!< Band number */
  FTM_MMW_RX_INFO_QUERY_PROP_BEAM_ID               = 4, /*!< Beam ID*/
  FTM_MMW_RX_INFO_QUERY_PROP_RX_OFFSET             = 5, /*!< Rx Offset in dB10*/
  FTM_MMW_RX_INFO_QUERY_PROP_NUM_SUB_BANDS         = 6, /*!< Num of sub bands for band x*/
  FTM_MMW_RX_INFO_QUERY_PROP_SUB_BAND_INDEX        = 7, /*!< sub band index*/
  FTM_MMW_RX_INFO_QUERY_PROP_SUB_BAND_MIN_FREQ     = 8, /*!< sub band min freq*/
  FTM_MMW_RX_INFO_QUERY_PROP_SUB_BAND_MAX_FREQ     = 9, /*!< sub band max freq*/
  FTM_MMW_RX_INFO_QUERY_PROP_GS                    = 10,/*!< GS 0/1/2/3 */ 
  FTM_MMW_RX_INFO_QUERY_PROP_NF                    = 11,/*!< NF in dB100*/ 
  FTM_MMW_RX_INFO_QUERY_PROP_NUM                        /*!< Max : Defines maximum number of properties */
} ftm_mmw_rx_info_query_property_type;

extern const char *ftm_mmw_rx_info_query_property_names[];

#ifdef T_WINNT
#error code not present
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* FTM_UNIFIED_QUERY_INTERFACE_H */

