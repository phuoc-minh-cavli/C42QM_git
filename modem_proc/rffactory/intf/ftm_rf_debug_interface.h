#ifndef FTM_RF_DEBUG_INTERFACE_H
#define FTM_RF_DEBUG_INTERFACE_H
/*!
  @file
  ftm_rf_debug_interface.h

  @brief
  Common framework to perform RF Calibration commands.
*/

/*======================================================================================================================

  Copyright (c) 2014 - 2021 Qualcomm Technologies Incorporated. All Rights Reserved

  Confidential and Proprietary - Qualcomm Technologies, Inc.

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

  $Header: //components/rel/rffactory.mpss/1.0/intf/ftm_rf_debug_interface.h#91 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------------------------------------------------
02/09/21   lzh     Added new properties(2 thru 5) for device cal
01/15/21   hep     Added placeholder VDPD Cal properties 15 thru 20 for GNB
04/08/20   hep     Added placeholder for Waveform Immediate Trigger for GNB
01/30/20   hep     Added placeholder for Tx Override IQ Compensation properties (36,37) for GNB
01/09/20   hep     Added placeholder for Tx Override Tx DCOC properties (34,35) for GNB
09/12/18   mpt     Updated Tx Override with SPR Action property
09/07/18   mpt     Add tech enter/exit functionality
08/02/18   mz      support VSWR Measure in CAL mode
04/04/18   jps     Add self test EVM measurement support in calibration mode
10/25/17   abr     Define codebook load property type in radio config command 
10/02/17   abr     1. Add Action type and Antenna Element ID properties for codebook override command
                   2. Define enum for Action type property
07/20/17   mpt     Add PLL_ID property for radio config
06/05/17   dj      Add codebook index as a parameter for codebook override
05/22/17   dj      Add property types for beam lut override
05/17/17   br      Add command for DPD Debug mode override
05/15/17   dj      Initial support for codebook override
05/03/17   dj      1. Fixes to compressed packet handling in RF Debug
                   2. Support for split request packets (v1 request) in RF Debug
04/14/17   br      Add property types for VDPD Conversion.
04/11/17   rsr     Add property types for 5G TX overide.
12/01/17   br      Add property enums  for Load Unity DPD and VDPD cal
08/22/16   br      Radio config- Updated property enums
07/26/16   br      Radio config- Updated property enums
07/25/16   mpt     Added tx override properties
07/14/16   br      Initial Release

======================================================================================================================*/

#include "comdef.h"

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef T_WINNT
#error code not present
#endif

#define FTM_RF_DEBUG_ATTRIB_CRTCL_BMSK  0x01    /* 1 on bit 0 = critical */
#define FTM_RF_DEBUG_ATTRIB_NON_CRTCL_BMSK 0x00  /* 0 on bit 0 = Non critical */

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Type definition to define the size of the reserved field in FTM test command packet.  */
typedef uint16 ftm_rf_debug_command_type;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Type definition to define the size of the reserved field in FTM test command packet.  */
typedef uint32 ftm_rf_debug_field_reserved_type;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Type definition to define the size of the version field in FTM test command packet.  */
typedef uint32 ftm_rf_debug_field_version_type;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Type definition to define the size of the technology field in FTM test command packet.  */
typedef uint32 ftm_rf_debug_field_technology_type;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Type definition to define the error code mask for response packet. Each bit indicates a type of error. */
typedef uint32 ftm_rf_debug_field_error_code_mask_type;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Defintion of command header for FTM command request packet */
typedef PACK(struct)
{
  uint8 diag_cmd;
  /*!< Command Identification */

  uint8 diag_subsys_id;
  /*!< Sub-system Identification. For instance - 11 for FTM */

  uint16 rf_subsys_id;
  /*!< RF Mode Identification for FTM_RF_DEBUG_C */

  ftm_rf_debug_command_type debug_command;
  /*!< FTM test command defined by #ftm_rf_test_command_enum_type */

  uint16 req_len;
  /*!< Defines the request length of the FTM diag packet, <b>when the packet is uncompressed</b>. This includes 
  the summation of the size of ftm_rf_test_diag_header_type and size of remaning paylaod (when it is uncompressed) */

  uint16 rsp_len;
  /*!< Defines the response length of the FTM diag packet, <b>when the packet is uncompressed</b>. This includes 
  the summation of the size of ftm_rf_test_diag_header_type and size of remaning paylaod (when it is uncompressed) */

} ftm_rf_debug_diag_header_type;


/*--------------------------------------------------------------------------------------------------------------------*/
/*! Defintion of command header for FTM command request packet */
typedef PACK(struct)
{
  uint8 diag_cmd;
  /*!< Command Identification */

  uint8 diag_subsys_id;
  /*!< Sub-system Identification. For instance - 11 for FTM */

  uint16 rf_subsys_id;
  /*!< RF Mode Identification. 101 for FTM_RF_TEST_C */

  ftm_rf_debug_command_type debug_command;
  /*!< FTM test command defined by #ftm_rf_test_command_enum_type */

  uint32 packet_length;
  /*!< Defines the length of the uncompressed FTM diag packet, <b>when the packet is uncompressed</b>. This includes 
  the summation of the size of ftm_rf_test_diag_header_type and size of remaining paylaod (when it is uncompressed). 
  When the diag header is used as part of the response packet, this is the length of response packet. */ 

} ftm_rf_debug_diag_header_subsys128_request_type;


/*--------------------------------------------------------------------------------------------------------------------*/
/*! Defintion of command header for FTM command request packet */
typedef PACK(struct)
{
  uint8 diag_cmd;
  /*!< Command Identification */

  uint8 diag_subsys_id;
  /*!< Sub-system Identification. For instance - 11 for FTM */

  uint16 rf_subsys_id;
  /*!< RF Mode Identification. 101 for FTM_RF_DEBUG_C */

  uint32 status;
  /*!< Status field. Not currently used */

  uint16 delayed_rsp_id;
  /*!< Delayed response ID. 0 if only 1 response packet. >0 for more than 1 responses. This is a common
  ID for all packets belonging to the same request  */

  uint16 rsp_cnt;
  /*!< Response count. 0 for the first response packet, 1 for the second response packet, etc. If more than
  two responses, the MSB (0x8000) will be set for all responses except for the last response. (e.g. 0x8000,
  0x8001, 0x8002, 0x0003). The MSB can be thought of as the "more to come" bit. The astute will wonder why 
  the "two response" case doesn't set the MSB of the first response...good question, that's just the way 
  the diag team defined this field. */

  ftm_rf_debug_command_type debug_command;
  /*!< FTM test command defined by #ftm_rf_debug_command_enum_type */

  uint16 req_len;
  /*!< Defines the request length of the FTM diag packet, <b>when the packet is uncompressed</b>. This includes 
  the summation of the size of ftm_rf_debug_diag_header_type and size of remaning paylaod (when it is uncompressed) */

  uint16 rsp_len;
  /*!< Defines the response length of the FTM diag packet, <b>when the packet is uncompressed</b>. This includes 
  the summation of the size of ftm_rf_debug_diag_header_type and size of remaning paylaod (when it is uncompressed) */

} ftm_rf_debug_diag_header_subsys128_type;


/*--------------------------------------------------------------------------------------------------------------------*/
/*! Defintion of command header for FTM command request packet. This replaces ftm_rf_test_diag_header_subsys128_type */
typedef PACK(struct)
{
  uint8 diag_cmd;
  /*!< Command Identification */

  uint8 diag_subsys_id;
  /*!< Sub-system Identification. For instance - 11 for FTM */

  uint16 rf_subsys_id;
  /*!< RF Mode Identification. 101 for FTM_RF_TEST_C */

  uint32 status;
  /*!< Status field. Not currently used */

  uint16 delayed_rsp_id;
  /*!< Delayed response ID. 0 if only 1 response packet. >0 for more than 1 responses. This is a common
  ID for all packets belonging to the same request  */

  uint16 rsp_cnt;
  /*!< Response count. 0 for the first response packet, 1 for the second response packet, etc. If more than
  two responses, the MSB (0x8000) will be set for all responses except for the last response. (e.g. 0x8000,
  0x8001, 0x8002, 0x0003). The MSB can be thought of as the "more to come" bit. The astute will wonder why
  the "two response" case doesn't set the MSB of the first response...good question, that's just the way
  the diag team defined this field. */

  ftm_rf_debug_command_type debug_command;
  /*!< FTM test command defined by #ftm_rf_test_command_enum_type */

  uint32 packet_length;
  /*!< Defines the length of the uncompressed FTM diag packet, <b>when the packet is uncompressed</b>. This includes
  the summation of the size of ftm_rf_test_diag_header_type and size of remaining paylaod (when it is uncompressed).
  When the diag header is used as part of the response packet, this is the length of response packet. */

}
ftm_rf_debug_diag_header_subsys128_response_type;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Defintion of command header for FTM RF Debug command request packet */
typedef PACK(struct)
{
  ftm_rf_debug_field_reserved_type reserved;
  /*!< Reserved Field */

  ftm_rf_debug_field_version_type version;
  /*!< Version of packet */

  ftm_rf_debug_field_technology_type technology;
  /*!< Indicates the technology (defined by ftm_rf_technology_type) for which the command is sent */

} ftm_rf_debug_command_header_type;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Defintion of command header for FTM RF Debug command request packet 
  V2 moves  technology field out of test command header and into payload */
typedef PACK(struct)
{
  ftm_rf_debug_field_reserved_type reserved;
  /*!< Reserved Field */

  ftm_rf_debug_field_version_type version;
  /*!< Version of packet */

} ftm_rf_debug_command_header_type_v2;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Defintion of command header for FTM RF Debug command request packet */
typedef PACK(struct)
{
  ftm_rf_debug_diag_header_type diag_header;
  /*!<  Structure defining the header for FTM request packet */

  ftm_rf_debug_command_header_type command_header;
  /*!<  Structure defining the command header for RF Debug command packet */

} ftm_rf_debug_req_header_type;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Defintion of command header for FTM RF Debug command request packet 
  V2 moves technology fields out of test command header and into payload */
typedef PACK(struct)
{
  ftm_rf_debug_diag_header_type diag_header;
  /*!<  Structure defining the header for FTM request packet */

  ftm_rf_debug_command_header_type_v2 command_header;
  /*!<  Structure defining the command header for RF Debug command packet */

} ftm_rf_debug_req_header_type_v2;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Defintion of split packet header for FTM RF Debug command request packet V2. 
  Note: 1. Supported only in command version 1
        2. If the packet is compressed, this will be part of the compressed payload */
typedef PACK(struct)
{
  ftm_rf_debug_field_reserved_type reserved;
  /*!<  Reserved Field */

  uint32 total_payload_size;
  /*!<  combined payload size not including the headers for split packets 
        Note: This should be the uncompressed size of the payload */

} ftm_rf_debug_split_packet_header_type;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Defintion of command header for FTM RF Debug command response packet */
typedef PACK(struct)
{
  ftm_rf_debug_field_reserved_type reserved;
  /*!< Reserved Field */

  ftm_rf_debug_field_version_type version;
  /*!< Version of packet */

  ftm_rf_debug_field_error_code_mask_type common_error_code;
  /*!< Error code from the common framework */

} ftm_rf_debug_command_rsp_header_type;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Defintion of command header for FTM RF Debug command response packet */
typedef PACK(struct)
{
  ftm_rf_debug_diag_header_type diag_header;
  /*!<  Structure defining the header for FTM response packet */

  ftm_rf_debug_command_rsp_header_type command_header;
  /*!<  Structure defining the command header for RF Debug response packet */

} ftm_rf_debug_header_rsp_type;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Defintion of command header for FTM RF Debug command response packet, Diag subsys 128 */
typedef PACK(struct)
{
  ftm_rf_debug_diag_header_subsys128_type diag_header;
  /*!<  Structure defining the header for FTM response packet using subsys128 */

  ftm_rf_debug_command_rsp_header_type command_header;
  /*!<  Structure defining the command header for RF Debug response packet */

} ftm_rf_debug_header_rsp_subsys128_type;


/*--------------------------------------------------------------------------------------------------------------------*/
/*! Defintion of command header for FTM RF Debug command response packet, Diag subsys 128 */
typedef PACK(struct)
{
  ftm_rf_debug_diag_header_subsys128_response_type diag_header;
  /*!<  Structure defining the header for FTM response packet using subsys128 */

  ftm_rf_debug_command_rsp_header_type command_header;
  /*!<  Structure defining the command header for RF Debug response packet */

} ftm_rf_debug_header_rsp_subsys128_v2_type;

/*====================================================================================================================*/
/*!
  @addtogroup FTM_TEST_TOOLS_CID
  @{
*/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Enumeration to indicate the type of commands available for FTM test command */
typedef enum
{
  FTM_RF_DEBUG_CMD_UNASSIGNED = 0, /*!< 0 : Unassigned command */

  FTM_RF_DEBUG_CMD_RADIO_CONFIGURE = 1, /*!< 1 : Radio Configure command */

  FTM_RF_DEBUG_CMD_TX_OVERRIDE = 2, /*!< 2 : Tx override command */

  FTM_RF_DEBUG_CMD_RX_OVERRIDE = 3, /*!< 3 : Rx override command */

  FTM_RF_DEBUG_CMD_DISCOVERY = 4, /*!< 4 : Command discovery command */

  FTM_RF_DEBUG_CMD_PROPERTY_DISCOVERY = 5, /*!< 5 : Command property discovery command */

  FTM_RF_DEBUG_CMD_TX_MEASURE = 6, /*!< 6 : Tx Measure command */

  FTM_RF_DEBUG_CMD_LOAD_UNITY_DPD = 7, /*!< 7 : Load Unity DPD command */

  FTM_RF_DEBUG_CMD_VDPD_CAL = 8, /*!< 8 : Performing XPT capture and loading DPD table command */

  FTM_RF_DEBUG_CMD_VDPD_CONVERSION = 9, /*!< 9 : Converting the Volterra kernel weights to AMAM and AMPM values */

  FTM_RF_DEBUG_CMD_LOAD_DPD = 10, /*!< 10 : Load DPD command */

  FTM_RF_DEBUG_CMD_RX_MEASURE = 11, /*!< 11 : Rx Measure command */

  FTM_RF_DEBUG_CMD_CODEBOOK_OVERRIDE = 12, /*!< 12 : Codebook Override command */

  FTM_RF_DEBUG_CMD_TX_MEASURE_ADV = 13, /*!< 13 : Tx Measure via advanced 5G handler */

  FTM_RF_DEBUG_CMD_SET_DPD_DEBUG_MODE = 14, /*!< 14 : Override debug mode to predpd or post dpd before DPD Cal */

  FTM_RF_DEBUG_CMD_IDC_CAL = 15, /*!< 15 : Perform WTR IDC Cal on single path and bandwidth */

  FTM_RF_DEBUG_CMD_MPE_TEST = 16, /*!< 16 : MPE X-Pol Cal/FMCW Char */

  FTM_RF_DEBUG_CMD_TECH_ENTER_EXIT = 17, /*!< 17 : Perform tech enter/exit functionality */
  
  FTM_RF_DEBUG_CMD_THERM_READ = 18, /*!< 18 : Thermal Read command */

  FTM_RF_DEBUG_CMD_DEVICE_CAL = 19, /*!< 19 : Perform Device cal */

  FTM_RF_DEBUG_CMD_HWID = 20, /*!< 20 : Query HWID*/

  FTM_RF_DEBUG_CMD_DCOC_OVERRIDE = 21, /*!< 21 : DCOC Override */
  
  FTM_RF_DEBUG_CMD_NUM  /*!< Max : Defines maximum number of command IDs */

} ftm_rf_debug_command_enum_type;

/*! @} */ /* FTM_RF_DEBUG_TOOLS_CID */

/*====================================================================================================================*/
/*!
  @addtogroup FTM_RF_DEBUG_ERROR_CODE
  @{
*/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Enumeration to indicate the type of error */
typedef enum
{
  FTM_RF_DEBUG_CMD_HANDLER_FAIL = 1, /*!< 1 : Indicates command handlder failure */

  FTM_RF_DEBUG_EC_NULL_PARAM = 2, /*!< 2 : Indicates NULL parameter */

  FTM_RF_DEBUG_EC_RSP_CREATION = 3, /*!< 3 : Failure during response creation */

  FTM_RF_DEBUG_EC_BAD_CMD = 4, /*!< 4 : Bad Command */

  FTM_RF_DEBUG_EC_BAD_CMD_HANDLER = 5, /*!< 5 : Indicates bad command hander */

  FTM_RF_DEBUG_EC_UMCOMPRESS_FAIL = 6, /*!< 6: Uncompress failure */

  FTM_RF_DEBUG_EC_MALLOC_FAILRE = 7, /*!< 7 : Malloc failure */

  FTM_RF_DEBUG_EC_NUM  /*!< Max : Defines maximum number of error codes */

} ftm_rf_debug_error_code_enum_type;

/*! @} */ /* FTM_RF_DEBUG_ERROR_CODE */

/*====================================================================================================================*/
/*!
  @addtogroup FTM_TEST_TOOLS_COMMAND Property IDs
  @{
*/

/*====================================================================================================================*/
/*!
  @name Radio Config Property ID

  @brief
  Radio Config Property ID list
*/
/*! @{ */

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Enumeration to indicate the type of properties for FTM Radio Config command. These enumeration are used to define 
the content of #ftm_properties_framework_field_property_type in FTM test command packet. 
radio_config_property_names Must be updated when this list is updated.*/
typedef enum
{
  FTM_RF_DEBUG_RADIO_CFG_PROP_UNASSIGNED = 0,       /*!< 0 : Unassigned property */

  FTM_RF_DEBUG_RADIO_CFG_PROP_IS_TEARDOWN = 1,      /*!< 1 : If teardown needs to be done */

  FTM_RF_DEBUG_RADIO_CFG_PROP_RADIO_SETUP_TYPE = 2, /*!< 2 : Type of Radio setup needed */

  FTM_RF_DEBUG_RADIO_CFG_PROP_RFM_DEVICE = 3,       /*!< 3 : RFM Device*/

  FTM_RF_DEBUG_RADIO_CFG_PROP_SIG_PATH = 4,        /*!< 4 : Signal Path for AT forward */

  FTM_RF_DEBUG_RADIO_CFG_PROP_ANT_PATH = 5,        /*!< 5 : Antenna Path */

  FTM_RF_DEBUG_RADIO_CFG_PROP_RFM_PATH_TYPE = 6,   /*!< 6 : RFM Path type - Tx/PRx/DRx */

  FTM_RF_DEBUG_RADIO_CFG_PROP_BAND = 7,            /*!< 7 : Operating Tech Band */

  FTM_RF_DEBUG_RADIO_CFG_PROP_SUBBAND = 8,         /*!< 8: Radio Bandwidth */

  FTM_RF_DEBUG_RADIO_CFG_PROP_RESERVED = 9,       /*!< 9: Reserved for future use */

  FTM_RF_DEBUG_RADIO_CFG_PROP_CHANNEL = 10,        /*!< 10 : Operating Tech channel */

  FTM_RF_DEBUG_RADIO_CFG_PROP_WAVEFORM = 11,       /*!< 11 : Tx waveform type */

  FTM_RF_DEBUG_RADIO_CFG_PROP_BANDWIDTH = 12,      /*!< 12 : Radio Bandwidth */

  FTM_RF_DEBUG_RADIO_CFG_PROP_NUM_RB = 13,         /*!< 13 : Number of RBs */

  FTM_RF_DEBUG_RADIO_CFG_PROP_START_RB = 14,       /*!< 14 : Start RB Index */

  FTM_RF_DEBUG_RADIO_CFG_PROP_CW_OFFSET = 15,      /*!< 15 : Offset frequency wrt channel to be tuned to*/

  FTM_RF_DEBUG_RADIO_CFG_PROP_IS_DC =  16,         /*!< 16 : Flag for Dual Carrier */

  FTM_RF_DEBUG_RADIO_CFG_PROP_MOD_TYPE  = 17,      /*!< 17 : Modulation type */

  FTM_RF_DEBUG_RADIO_CFG_PROP_LOOPBACK_TYPE = 18,  /*!< 18 : 5GNR Only: Loopback type, as defined by RFC */

  FTM_RF_DEBUG_RADIO_CFG_PROP_BEAM_ID = 19,        /*!< 19 : 5GNR Only: Beam ID, as defined by RFC */

  FTM_RF_DEBUG_RADIO_CFG_PROP_CC_INDEX = 20,       /*!< 20 : 5GNR Only: Component Carrier Index */

  FTM_RF_DEBUG_RADIO_CFG_PROP_CC_START_RB = 21,    /*!< 21 : 5GNR Only: Start RB Index for the Component Carrier*/

  FTM_RF_DEBUG_RADIO_CFG_PROP_CC_NUM_RB = 22,      /*!< 22 : 5GNR Only: Number of RBs for the Component Carrier*/

  FTM_RF_DEBUG_RADIO_CFG_PROP_CC_BANDWIDTH = 23,   /*!< 23 : 5GNR Only: CC Bandwidth*/

  FTM_RF_DEBUG_RADIO_CFG_PROP_LOOPBACK_RFM_DEVICE = 24, /*!< 24 : 5GNR Only: Output param. Loopback RFM Device*/

  FTM_RF_DEBUG_RADIO_CFG_PROP_PLL_ID = 25,         /*!< 25 : PLL id is part of the RFM Path definition*/

  FTM_RF_DEBUG_RADIO_CFG_PROP_TUNE_TX_TO_RX_FREQ = 26, /*!< 26 : Flag indicating tune to Rx frequence*/

  FTM_RF_DEBUG_RADIO_CFG_PROP_LOAD_CODEBOOK = 27, /*!< 27 : 5GNR Only: Load Codebook Flag*/

  FTM_RF_DEBUG_RADIO_CFG_PROP_FREQUENCY = 28, /*!< 28 : Operating Tech frequency*/

  FTM_RF_DEBUG_RADIO_CFG_PROP_VERSION = 29, /*!< 29 : Radio setup  version type*/

  FTM_RF_DEBUG_RADIO_CFG_PROP_SWITCH_TDSCDMA_WAVEFORM = 30, /*!< 30 : flag to indicate only switch TDSCDMA waveform during radio setup call- TDSCDMA specic*/

  FTM_RF_DEBUG_RADIO_CFG_PROP_SET_MOD  = 31,      /*!< 31 : Flag to indicate onnly Change Modulation during radio setup call*/
  
  FTM_RF_DEBUG_RADIO_CFG_PROP_CC_SCS  = 32,      /*!< 32 : 5GNR Only: Sub-carrier spacing for the Component Carrier*/

  FTM_RF_DEBUG_RADIO_CFG_PROP_WAVEFORM_ORIGIN  = 33, /*!< 33 : GNB Only: Waveform source CPM or Modem */
  
  FTM_RF_DEBUG_RADIO_CFG_PROP_NDR_STATE = 34, 	 /*!< 34 : NDR state*/

  FTM_RF_DEBUG_RADIO_CFG_PROP_NR5G_MOD_TYPE  = 35,      /*!< 35 : Modulation type for NR5G */

  FTM_RF_DEBUG_RADIO_CFG_PROP_WAVEFORM_IMMEDIATE_TRIGGER  = 36,      /*!< 36 : GNB Waveform Immediate Trigger Flag */

  FTM_RF_DEBUG_RADIO_CFG_PROP_VPA_MAX_MV  = 37,      /*!< 37 : Vpa Mav mV */ 
  
  FTM_RF_DEBUG_RADIO_CFG_PROP_ANT_ISO_TUNE_TYPE = 38,      /*< 38 FTM_RF_DEBUG_RADIO_CFG_PROP_ANT_ISO_TUNE_TYPE*/

  FTM_RF_DEBUG_RADIO_CFG_PROP_NUM                  /*!< Max : Defines maximum number of properties */

} ftm_rf_debug_radio_config_property_type;

extern const char* rf_debug_radio_config_property_names[];

extern const uint8 ftm_rf_debug_radio_config_prop_attrib_list[];

/*! @} */

/*====================================================================================================================*/
/*!
  @name Tx Control Property ID

  @brief
  Tx Control Property ID list
*/
/*! @{ */

/*====================================================================================================================*/
/*!
  @name Rx Override Property ID

  @brief
  Rx OverrideProperty ID list
*/
/*! @{ */

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Enumeration to indicate the type of properties for FTM Rx Override Command. 
  These enumeration are used to define the content of #ftm_properties_framework_field_property_type in FTM RF Debug command packet. 
rx_override_property_names Must be updated when this list is updated.*/
typedef enum
{
  FTM_RF_DEBUG_RX_OVERRIDE_PROP_UNASSIGNED = 0, /*!< 0 : Unassigned property */

  FTM_RF_DEBUG_RX_OVERRIDE_PROP_PATH_INDEX = 1,  /*!< 1 : RF path index */

  FTM_RF_DEBUG_RX_OVERRIDE_PROP_SIG_PATH = 2,  /*!< 2 : Signal Path for AT forward */

  FTM_RF_DEBUG_RX_OVERRIDE_PROP_RFM_DEVICE = 3, /*!< 3 : RFM device for this measurement*/

  FTM_RF_DEBUG_RX_OVERRIDE_PROP_ANTENNA_PORT = 4, /*!< 4 : RFM device for this measurement*/

  FTM_RF_DEBUG_RX_OVERRIDE_PROP_EXPECTED_AGC = 5, /*!< 5 : Expected AGC Value */

  FTM_RF_DEBUG_RX_OVERRIDE_PROP_LNA_GAIN_STATE = 6, /*!< 6 : LNA Gain State */

  FTM_RF_DEBUG_RX_OVERRIDE_PROP_AGC = 7, /*!< 7 : RxAGC value */

  FTM_RF_DEBUG_RX_OVERRIDE_PROP_NBEE = 8, /*!< 8 : NBEE value */

  FTM_RF_DEBUG_RX_OVERRIDE_PROP_SIGNAL_TYPE = 9,

  FTM_RF_DEBUG_RX_OVERRIDE_PROP_DATA_TYPE = 10, /*!< 10 : GSM only - Burst or Continuous Mode */

  FTM_RF_DEBUG_RX_OVERRIDE_PROP_MEAS_RSSI = 11, /*!< 10 : GSM only - Measured RSSI */

  FTM_RF_DEBUG_RX_OVERRIDE_PROP_RX_ENABLE = 12, /*!< 10 : GSM only - Enable/disable Rx */

  //FTM_RF_DEBUG_RX_OVERRIDE_PROP_BEAM_ID = 13, /*!< 13: 5GNR only - Beam ID */

  //FTM_RF_DEBUG_RX_OVERRIDE_PROP_SET_GAIN_STATE_ACTION = 14, /*!< 14: 5GNR only - (Action) Set Rx Gain State */

  //FTM_RF_DEBUG_RX_OVERRIDE_PROP_SET_DVGA_OFFSET_ACTION = 15, /*!< 15: 5GNR only - (Action) Set DVGA Offset */

  //FTM_RF_DEBUG_RX_OVERRIDE_PROP_DVGA_CC_INDEX = 16, /*!< 16: 5GNR only - which CC Index to set DVGA Offset Value on */

  //FTM_RF_DEBUG_RX_OVERRIDE_PROP_DVGA_OFFSET_VALUE = 17, /*!< 17: 5GNR only - DVGA Offset Value */

  FTM_RF_DEBUG_RX_OVERRIDE_PROP_PLL_ID = 13, /*!< 13 : RFM path PLL ID */

  FTM_RF_DEBUG_RX_OVERRIDE_PROP_RESERVED_1 = 14, /*!< 14 : RESERVED_1 */

  FTM_RF_DEBUG_RX_OVERRIDE_PROP_RESERVED_2 = 15, /*!< 15 : RESERVED_2 */  

  FTM_RF_DEBUG_RX_OVERRIDE_PROP_RESERVED_3 = 16, /*!< 16 : RESERVED_3*/

  FTM_RF_DEBUG_RX_OVERRIDE_PROP_RESERVED_4 = 17, /*!< 17 : RESERVED_4*/

  FTM_RF_DEBUG_RX_OVERRIDE_PROP_NUM = 18, /*!< Max : Defines maximum number of properties */
  
  FTM_RF_DEBUG_RX_OVERRIDE_PROP_RESERVED_5 = 19,                   /*!< 19 : RESERVED_5*/
  
  FTM_RF_DEBUG_RX_OVERRIDE_PROP_RESERVED_6 = 20,                   /*!< 20 : RESERVED_6*/

  FTM_RF_DEBUG_RX_OVERRIDE_PROP_RESERVED_7 = 21,                   /*!< 21 : RESERVED_7*/

  FTM_RF_DEBUG_RX_OVERRIDE_PROP_RESERVED_8 = 22,                   /*!< 22 : RESERVED_8*/
  
  FTM_RF_DEBUG_RX_OVERRIDE_PROP_NUM_UPDATED  /*!< Updated Max : Defines maximum number of properties */

} ftm_rf_debug_rx_override_property_type;

/*! Enumeration to indicate the type of the IQ capture data format */

extern const char* rf_debug_rx_override_property_names[];

extern const uint8 ftm_rf_debug_rx_override_prop_attrib_list[];

/*! @} */
/*====================================================================================================================*/
/*!
  @name Tx Measure Property ID

  @brief
  Tx Measure Property ID list
*/
/*! @{ */

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Enumeration to indicate the type of properties for FTM Tx Measure Command. 
  These enumeration are used to define the content of #ftm_properties_framework_field_property_type in FTM RF Debug command packet. 
  tx_measure_property_names Must be updated when this list is updated.
*/
typedef enum
{
  FTM_RF_DEBUG_TX_MEASURE_PROP_UNASSIGNED = 0, /*!< 0 : Unassigned property */

  FTM_RF_DEBUG_TX_MEASURE_PROP_PATH_INDEX = 1,  /*!< 1 : RF path index */

  FTM_RF_DEBUG_TX_MEASURE_PROP_SIG_PATH = 2,  /*!< 2 : Signal Path for AT forward */

  FTM_RF_DEBUG_TX_MEASURE_PROP_RFM_DEVICE = 3, /*!< 3 : RFM device for this measurement*/

  FTM_RF_DEBUG_TX_MEASURE_PROP_ANTENNA_PORT = 4, /*!< 4 : RFM device for this measurement*/

  FTM_RF_DEBUG_TX_MEASURE_PROP_EXPECTED_PWR = 5, /*!< 5 : Expected TX power */

  FTM_RF_DEBUG_TX_MEASURE_PROP_FBRX_GAIN_STATE = 6, /*!< 6 : FBRX Gain State */

  FTM_RF_DEBUG_TX_MEASURE_PROP_REF_TXPWR = 7, /*!< 7 : Reference TX Power */

  FTM_RF_DEBUG_TX_MEASURE_PROP_MEASURED_PWR = 8, /*!< 8 : TX power measured */

  FTM_RF_DEBUG_TX_MEASURE_PROP_EXPECTED_HW_GAIN = 9, /*!< 9 : Expected HW Gain !!! */

  FTM_RF_DEBUG_TX_MEASURE_PROP_HW_GAIN = 10, /*!< 10 : HW Gain */

  FTM_RF_DEBUG_TX_MEASURE_PROP_HW_GAIN_DELTA = 11, /*!< 11 : HW Gain Delta */

  FTM_RF_DEBUG_TX_MEASURE_PROP_TRUNC_BIT = 12, /*!< 12 : Trunc Bit */

  FTM_RF_DEBUG_TX_MEASURE_PROP_LS_ERROR = 13, /*!< 13 : FW least square error */

  FTM_RF_DEBUG_TX_MEASURE_PROP_PREDET_INDEX = 14, /*!< 14 : Correlation index */

  FTM_RF_DEBUG_TX_MEASURE_PROP_FW_GAIN = 15, /*!< 15 : FW gain */

  FTM_RF_DEBUG_TX_MEASURE_PROP_SS_REF_TXPWR = 16, /*!< 16: SS Reference TX Power */

  FTM_RF_DEBUG_TX_MEASURE_PROP_FBRX_MODE = 17, /*!< 17: Type of FBRx mode- 0 - Override mode, 1- FBRx cal mode , 2- APT cal mode  */

  FTM_RF_DEBUG_TX_MEASURE_PROP_MEASUREMENT_TYPE = 18, /*!< 18: Perform ACLR or FBRx measurement */

  FTM_RF_DEBUG_TX_MEASURE_PROP_ACLR_UTRA1_POS = 19, /*!< 19: ACLR UTRA1 Positive in db100  */

  FTM_RF_DEBUG_TX_MEASURE_PROP_ACLR_UTRA1_NEG = 20, /*!< 20: ACLR UTRA1 Negative in db100  */

  FTM_RF_DEBUG_TX_MEASURE_PROP_ACLR_UTRA2_POS = 21, /*!< 21: ACLR UTRA2 Positive in db100 */

  FTM_RF_DEBUG_TX_MEASURE_PROP_ACLR_UTRA2_NEG = 22, /*!< 22: ACLR UTRA2 Negative in db100 */
  
  FTM_RF_DEBUG_TX_MEASURE_PROP_ACLR_EUTRA_POS = 23, /*!< 23: ACLR EUTRA Positive in db100*/

  FTM_RF_DEBUG_TX_MEASURE_PROP_ACLR_EUTRA_NEG = 24, /*!< 24: ACLR EUTRA Negative in db100*/

  FTM_RF_DEBUG_TX_MEASURE_PROP_IQ_GAIN_ACTION = 25,  /*!< 25 : IQ Gain Action Type */

  FTM_RF_DEBUG_TX_MEASURE_PROP_IQ_GAIN = 26,  /*!< 26 : IQ Gain */

  FTM_RF_DEBUG_TX_MEASURE_PROP_TXFE_GAIN_ACTION = 27,  /*!< 27 : TxFE Gain Action Type */

  FTM_RF_DEBUG_TX_MEASURE_PROP_TXFE_GAIN = 28,  /*!< 28 : TxFE Gain */

  FTM_RF_DEBUG_TX_MEASURE_PROP_ENV_SCALE_ACTION = 29,  /*!< 29 : Envelope Scale Action Type */

  FTM_RF_DEBUG_TX_MEASURE_PROP_ENV_SCALE = 30,  /*!< 30 : Envelope Scale */

  FTM_RF_DEBUG_TX_MEASURE_PROP_RGI = 31,  /*!< 31 : RGI */

  FTM_RF_DEBUG_TX_MEASURE_PROP_PA_BIAS = 32,  /*!< 32 : PA Bias */

  FTM_RF_DEBUG_TX_MEASURE_PROP_PA_STATE = 33,  /*!< 33 : PA State*/

  FTM_RF_DEBUG_TX_MEASURE_PROP_PA_CURRENT = 34,  /*!< 34 : PA Current */

  FTM_RF_DEBUG_TX_MEASURE_PROP_DELAY_ACTION = 35, /*!< 35 : Delay Action Type*/

  FTM_RF_DEBUG_TX_MEASURE_PROP_DELAY = 36,/*!< 36 : Delay */

  FTM_RF_DEBUG_TX_MEASURE_PROP_PLL_ID =37,  /*!< 37 : PLL ID of RF path*/

  FTM_RF_DEBUG_TX_MEASURE_PROP_EVM = 38, /*!< 38 : Measured EVM */

  FTM_RF_DEBUG_TX_MEASURE_PROP_XPT_MODE = 39, /*!< 39 : Input XPT mode for Tx Power measurement  */
  
  FTM_RF_DEBUG_TX_MEASURE_PROP_VSWR_S11_MAG = 40,     /*!< 40 : VSWR S11 Magnitude */

  FTM_RF_DEBUG_TX_MEASURE_PROP_CONVERGED_IQ_GAIN = 41, /*!< 41 : Measured final IQ gain */

  FTM_RF_DEBUG_TX_MEASURE_PROP_ET_VMIN = 42, /*!< 41 : Set ET Vmin Value */

  FTM_RF_DEBUG_TX_MEASURE_PROP_NUM  /*!< Max : Defines maximum number of properties */

} ftm_rf_debug_tx_measure_property_type;

extern const char* rf_debug_tx_measure_property_names[];

extern const uint8 ftm_rf_debug_tx_measure_prop_attrib_list[];

/*! @} */

/*====================================================================================================================*/
/*!
  @name Tx override Property ID

  @brief
  Tx Override Property ID list
*/
/*! @{ */

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Enumeration to indicate the type of properties for FTM TX Override Command. 
  These enumeration are used to define the content of #ftm_properties_framework_field_property_type in FTM RF debug command packet
tx_override_property_names Must be updated when this list is updated.*/
typedef enum
{
  FTM_RF_DEBUG_TX_OVERRIDE_PROP_UNASSIGNED = 0, /*!< 0 : Unassigned property */

  FTM_RF_DEBUG_TX_OVERRIDE_PROP_TX_ENABLE = 1, /*!< 1 : Tx Enable */

  FTM_RF_DEBUG_TX_OVERRIDE_PROP_PATH_INDEX = 2, /*!< 2 : Path Index */

  FTM_RF_DEBUG_TX_OVERRIDE_PROP_SIG_PATH = 3, /*!< 3 : Signal Path*/

  FTM_RF_DEBUG_TX_OVERRIDE_PROP_RFM_DEVICE = 4, /*!< 4 : RFM Device */

  FTM_RF_DEBUG_TX_OVERRIDE_PROP_ANT_PATH = 5, /*!< 5 : Antenna Path */

  FTM_RF_DEBUG_TX_OVERRIDE_PROP_XPT_MODE = 6, /*!< 6 : XPT Mode */

  FTM_RF_DEBUG_TX_OVERRIDE_PROP_IQ_GAIN_ACTION = 7,  /*!< 7 : IQ Gain Action Type */

  FTM_RF_DEBUG_TX_OVERRIDE_PROP_IQ_GAIN = 8,  /*!< 8 : IQ Gain */

  FTM_RF_DEBUG_TX_OVERRIDE_PROP_TXFE_GAIN_ACTION = 9,  /*!< 9 : TxFE Gain Action Type */

  FTM_RF_DEBUG_TX_OVERRIDE_PROP_TXFE_GAIN = 10,  /*!< 10 : TxFE Gain */

  FTM_RF_DEBUG_TX_OVERRIDE_PROP_ENV_SCALE_ACTION = 11,  /*!< 11 : Envelope Scale Action Type */

  FTM_RF_DEBUG_TX_OVERRIDE_PROP_ENV_SCALE = 12,  /*!< 12 : Envelope Scale */

  FTM_RF_DEBUG_TX_OVERRIDE_PROP_RGI = 13,  /*!< 13 : RGI */

  FTM_RF_DEBUG_TX_OVERRIDE_PROP_PA_BIAS = 14,  /*!< 14 : PA Bias */

  FTM_RF_DEBUG_TX_OVERRIDE_PROP_PA_STATE = 15,  /*!< 15 : PA State*/

  FTM_RF_DEBUG_TX_OVERRIDE_PROP_PA_CURRENT = 16,  /*!< 16 : PA Current */

  FTM_RF_DEBUG_TX_OVERRIDE_PROP_DELAY_ACTION = 17,  /*!< 17 : Delay Action Type */

  FTM_RF_DEBUG_TX_OVERRIDE_PROP_DELAY = 18,  /*!< 18 : Delay */

  FTM_RF_DEBUG_TX_OVERRIDE_PROP_DATA_TYPE = 19,  /*!< 19: GSM - Burst or continuous mode*/

  FTM_RF_DEBUG_TX_OVERRIDE_PROP_SLOT_NUMBER = 20, /*!< 20: GSM - Slot number*/

  FTM_RF_DEBUG_TX_OVERRIDE_PROP_DATA_SOURCE = 21, /*!< 21: GSM - Random data or Tone*/

  FTM_RF_DEBUG_TX_OVERRIDE_PROP_MOD_TYPE = 22, /*!< 22: GSM - Modulation type GSM or EDGE*/

  FTM_RF_DEBUG_TX_OVERRIDE_PROP_ENV_GAIN = 23, /*!< 23: GSM - Enveloper gain*/

  FTM_RF_DEBUG_TX_OVERRIDE_PROP_BEAM_ID = 24, /*!< 24: 5G - Beam ID*/

  FTM_RF_DEBUG_TX_OVERRIDE_PROP_PRE_COMB_IQ_GAIN_ACTION = 25, /*!< 25: 5G - Pre-combiner IQ gain action.*/

  FTM_RF_DEBUG_TX_OVERRIDE_PROP_CARR_IDX = 26, /*!< 26: 5G - Carrier index.*/

  FTM_RF_DEBUG_TX_OVERRIDE_PROP_PRE_COMB_IQ_GAIN = 27, /*!< 27: 5G - Pre-combiner (per carrier) IQ gain.*/

  FTM_RF_DEBUG_TX_OVERRIDE_PROP_RF_TRX_IDX = 28, /*!< 28: 5G - RF TRX id .*/

  FTM_RF_DEBUG_TX_OVERRIDE_PROP_PLL_ID = 29, /*!< 29: PLL ID .*/

  FTM_RF_DEBUG_TX_OVERRIDE_PROP_SPR_ACTION = 30, /*!< 30: SPR Action .*/

  FTM_RF_DEBUG_TX_OVERRIDE_PROP_TX_MOD_TYPE = 31, /*!< 31: Tx Modulation Type .*/

  FTM_RF_DEBUG_TX_OVERRIDE_PROP_MIXER_GAIN = 32, /*!< 32: Tx Mixer gain Type .*/

  FTM_RF_DEBUG_TX_OVERRIDE_PROP_PHASE_SHIFTER = 33, /*!< 33: Tx Phase shifter Type .*/

  FTM_RF_DEBUG_TX_OVERRIDE_PROP_DCOC_I_COMP = 34, /*!< 34: Tx DC Offset compensation (I) .*/

  FTM_RF_DEBUG_TX_OVERRIDE_PROP_DCOC_Q_COMP = 35, /*!< 35: Tx DC Offset compensation (Q) .*/

  FTM_RF_DEBUG_TX_OVERRIDE_PROP_IQ_COMP_GAIN = 36, /*!< 36: IQ compensation gain .*/

  FTM_RF_DEBUG_TX_OVERRIDE_PROP_IQ_COMP_CROSS = 37, /*!< 37: IQ compensation cross .*/

  FTM_RF_DEBUG_TX_OVERRIDE_PROP_RESERVED_38 = 38, /*!< 38: Reserved. */

  FTM_RF_DEBUG_TX_OVERRIDE_PROP_ET_VMIN   = 39,

  FTM_RF_DEBUG_TX_OVERRIDE_PROP_NUM  /*!< Max : Defines maximum number of properties */

} ftm_rf_debug_tx_override_property_type;

extern const char* rf_debug_tx_override_property_names[];

extern const uint8 ftm_rf_debug_tx_override_prop_attrib_list[];

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Enumeration to indicate the type of properties for Command Discovery Command. 
  These enumeration are used to define the content of #ftm_properties_framework_field_property_type in FTM RF debug command packet
command_discovery_property_names Must be updated when this list is updated.*/
typedef enum
{
  FTM_RF_DEBUG_COMMAND_DISCOVERY_PROP_UNASSIGNED = 0, /*!< 0 : Unassigned property */

  FTM_RF_DEBUG_COMMAND_DISCOVERY_PROP_COMMAND_BITMASK_0 = 1, /*!< 1 : Command bitmask for command ids 0-63*/

  FTM_RF_DEBUG_COMMAND_DISCOVERY_PROP_COMMAND_BITMASK_1 = 2, /*!< 1 : Command bitmask for command ids 64-127*/

  FTM_RF_DEBUG_COMMAND_DISCOVERY_PROP_NUM  /*!< Max : Defines maximum number of properties */

} ftm_rf_debug_command_discovery_property_type;

extern const char* rf_debug_command_discovery_property_names[];

extern const uint8 ftm_rf_debug_command_discovery_prop_attrib_list[];
/*--------------------------------------------------------------------------------------------------------------------*/
/*! Enumeration to indicate the type of properties for Command Property Discovery Command. 
  These enumeration are used to define the content of #ftm_properties_framework_field_property_type in FTM RF debug command packet
command_property_discovery_property_names Must be updated when this list is updated.*/
typedef enum
{
  FTM_RF_DEBUG_COMMAND_PROPERTY_DISCOVERY_PROP_UNASSIGNED = 0, /*!< 0 : Unassigned property */

  FTM_RF_DEBUG_COMMAND_PROPERTY_DISCOVERY_PROP_SUBCOMMAND = 1, /*!< 1 : Type of Command */

  FTM_RF_DEBUG_COMMAND_PROPERTY_DISCOVERY_PROP_INPUT_BITMASK = 2, /*!< 2 : Input Property bitmask*/

  FTM_RF_DEBUG_COMMAND_PROPERTY_DISCOVERY_PROP_OUTPUT_BITMASK = 3, /*!< 3 : Output Property bitmask*/

  FTM_RF_DEBUG_COMMAND_PROPERTY_DISCOVERY_PROP_NUM  /*!< Max : Defines maximum number of properties */

} ftm_rf_debug_command_property_discovery_property_type;

extern const char* rf_debug_command_property_discovery_property_names[];

extern const uint8 ftm_rf_debug_command_property_discovery_prop_attrib_list[];

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Enumeration to indicate the type of properties for Load Unity DPD Command. 
  These enumeration are used to define the content of #ftm_properties_framework_field_property_type in FTM RF debug command packet
load_unity_dpd_property_names Must be updated when this list is updated.*/
typedef enum
{
  FTM_RF_DEBUG_LOAD_UNITY_DPD_PROP_UNASSIGNED = 0, /*!< 0 : Unassigned property */

  FTM_RF_DEBUG_LOAD_UNITY_DPD_PROP_PATH_INDEX = 1, /*!< 1 : Path Index */

  FTM_RF_DEBUG_LOAD_UNITY_DPD_PROP_SIG_PATH = 2, /*!< 2 : Signal Path*/

  FTM_RF_DEBUG_LOAD_UNITY_DPD_PROP_RFM_DEVICE = 3, /*!< 3 : RFM Device */

  FTM_RF_DEBUG_LOAD_UNITY_DPD_PROP_XPT_MODE = 4, /*!< 4 : ET or EPT*/

  FTM_RF_DEBUG_LOAD_UNITY_DPD_PROP_ETVD_MAX = 5,   /*!< 5 : Bias */

  FTM_RF_DEBUG_LOAD_UNITY_DPD_PROP_ETVD_MIN = 6,  /*!< 6: ETVD Min */

  FTM_RF_DEBUG_LOAD_UNITY_DPD_PROP_DET = 7, /*!< 7 : Detrough*/

  FTM_RF_DEBUG_LOAD_UNITY_DPD_PROP_QPOET_VMIN = 8, /*!< 8 : QPOET Min Voltage in mv*/

  FTM_RF_DEBUG_LOAD_UNITY_DPD_PROP_QPOET_VMAX = 9, /*!< 9 : QPOET Max Voltage in mv*/

  FTM_RF_DEBUG_LOAD_UNITY_DPD_PROP_PLL_ID = 10, /*!< 10 : PLL ID */
  
  FTM_RF_DEBUG_LOAD_UNITY_DPD_PROP_SCALE_DET = 11, /*!< Scale Detrough*/

  FTM_RF_DEBUG_LOAD_UNITY_DPD_PROP_NUM  /*!< Max : Defines maximum number of properties */

} ftm_rf_debug_load_unity_dpd_property_type;

extern const char* rf_debug_load_unity_dpd_property_names[];

extern const uint8 ftm_rf_debug_load_unity_dpd_prop_attrib_list[];

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Enumeration to indicate the type of properties for VDPD Cal Command. 
  These enumeration are used to define the content of #ftm_properties_framework_field_property_type in FTM RF debug command packet
load_dpd_property_names Must be updated when this list is updated.*/
typedef enum
{
  FTM_RF_DEBUG_VDPD_CAL_PROP_UNASSIGNED = 0, /*!< 0 : Unassigned property */

  FTM_RF_DEBUG_VDPD_CAL_PROP_PATH_INDEX = 1, /*!< 1 : Path Index */

  FTM_RF_DEBUG_VDPD_CAL_PROP_SIG_PATH = 2, /*!< 2 : Signal Path*/

  FTM_RF_DEBUG_VDPD_CAL_PROP_RFM_DEVICE = 3, /*!< 3 : RFM Device */

  FTM_RF_DEBUG_VDPD_CAL_PROP_XPT_MODE = 4, /*!< 4 : XPT Mode */

  FTM_RF_DEBUG_VDPD_CAL_PROP_SNR = 5, /*!< 5 : SNR */

  FTM_RF_DEBUG_VDPD_CAL_PROP_EPT_COMPRESSION = 6, /*!< 6 EPT Compression*/

  FTM_RF_DEBUG_VDPD_CAL_PROP_KERNEL_MASK = 7, /*!< 7 Kernel Mask*/

  FTM_RF_DEBUG_VDPD_CAL_PROP_QFACTOR = 8, /*!< 8 Kernel weight Qfactor*/

  FTM_RF_DEBUG_VDPD_CAL_PROP_NUM_KERNEL_WEIGHTS = 9, /*!< 9 Number of Kernel Weights */

  FTM_RF_DEBUG_VDPD_CAL_PROP_KERNEL_WEIGHT_I = 10, /*!< 10 Kernel Weight I coefficient*/

  FTM_RF_DEBUG_VDPD_CAL_PROP_KERNEL_WEIGHT_Q = 11, /*!< 11 Kernel Weight Q coefficient*/

  FTM_RF_DEBUG_VDPD_CAL_PROP_RESERVED = 12, /*!< 12 : Used to indicate saving captures */

  FTM_RF_DEBUG_VDPD_CAL_PROP_PROC_ONLY = 13, /*!< 13 : Used to indicate that DPD should not be loaded after capture */

  FTM_RF_DEBUG_VDPD_CAL_PROP_PLL_ID = 14, /*!< 14 : PLL ID */

  FTM_RF_DEBUG_VDPD_CAL_PROP_DBL_PRECISION = 15, /*!< 15: Double Precision Indicator (GNB) */

  FTM_RF_DEBUG_VDPD_CAL_PROP_ACLR_RIGHT = 16, /*!< 16: Right side ACLR component (GNB) */

  FTM_RF_DEBUG_VDPD_CAL_PROP_ACLR_LEFT = 17, /*!< 16: Left side ACLR component (GNB) */

  FTM_RF_DEBUG_VDPD_CAL_PROP_ACLR_MET = 18, /*!< 16: ACLR criteria met (GNB) */

  FTM_RF_DEBUG_VDPD_CAL_PROP_SNR_MET = 19, /*!< 16: SNR criteria met (GNB) */

  FTM_RF_DEBUG_VDPD_CAL_PROP_FBRX_PWR_DBM10 = 20, /*!< 16: FBRx Power (INPUT) (GNB) */    

  FTM_RF_DEBUG_VDPD_CAL_PROP_NUM  /*!< Max : Defines maximum number of properties */

} ftm_rf_debug_vdpd_cal_property_type;

extern const char* rf_debug_vdpd_cal_property_names[];

extern const uint8 ftm_rf_debug_vdpd_cal_prop_attrib_list[];

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Enumeration to indicate the type of properties for VDPD Conversion Command . 
  These enumeration are used to define the content of #ftm_properties_framework_field_property_type in FTM RF debug command packet
rf_debug_vdpd_conversion_property_names Must be updated when this list is updated.*/
typedef enum
{
  FTM_RF_DEBUG_VDPD_CONVERSION_PROP_UNASSIGNED = 0, /*!< 0 : Unassigned property */

  FTM_RF_DEBUG_VDPD_CONVERSION_PROP_KERNEL_MASK = 1, /*!< 1 Kernel Mask*/

  FTM_RF_DEBUG_VDPD_CONVERSION_PROP_QFACTOR = 2, /*!< 2 Kernel weight Qfactor*/

  FTM_RF_DEBUG_VDPD_CONVERSION_PROP_KERNEL_WEIGHT_I = 3, /*!< 3 Kernel Weight I coefficient*/

  FTM_RF_DEBUG_VDPD_CONVERSION_PROP_KERNEL_WEIGHT_Q = 4, /*!< 4 Kernel Weight Q coefficient*/

  FTM_RF_DEBUG_VDPD_CONVERSION_PROP_NUM_LUT_VALUES = 5, /*!< 5 Number of Kernel Weights */

  FTM_RF_DEBUG_VDPD_CONVERSION_PROP_AMAM = 6, /*!< 6 AMAM LUT values*/

  FTM_RF_DEBUG_VDPD_CONVERSION_PROP_AMPM = 7, /*!< 7 AMPM LUT values*/

  FTM_RF_DEBUG_VDPD_CONVERSION_PROP_GAIN = 8, /*!< 8 Gain values*/

  FTM_RF_DEBUG_VDPD_CONVERSION_PROP_NUM  /*!< Max : Defines maximum number of properties */

} ftm_rf_debug_vdpd_conversion_property_type;

extern const char* rf_debug_vdpd_conversion_property_names[];

extern const uint8 ftm_rf_debug_vdpd_conversion_prop_attrib_list[];
/*--------------------------------------------------------------------------------------------------------------------*/
/*! Enumeration to indicate the type of properties for Load DPD Command . 
  These enumeration are used to define the content of #ftm_properties_framework_field_property_type in FTM RF debug command packet
rf_debug_load_dpd_property_names Must be updated when this list is updated.*/
typedef enum
{
  FTM_RF_DEBUG_LOAD_DPD_PROP_UNASSIGNED = 0, /*!< 0 : Unassigned property */

  FTM_RF_DEBUG_LOAD_DPD_PROP_RFM_DEVICE = 1, /*!< 1 : RFM device*/

  FTM_RF_DEBUG_LOAD_DPD_PROP_SIG_PATH = 2, /*!< 2 : Signal Path*/

  FTM_RF_DEBUG_LOAD_DPD_PROP_XPT_MODE = 3, /*!< 3 XPT Mode*/

  FTM_RF_DEBUG_LOAD_DPD_PROP_KERNEL_MASK = 4, /*!< 4 Kernel Mask*/

  FTM_RF_DEBUG_LOAD_DPD_PROP_QFACTOR = 5, /*!< 5 Kernel weight Qfactor*/

  FTM_RF_DEBUG_LOAD_DPD_PROP_KERNEL_WEIGHT_I = 6, /*!< 6 Kernel Weight I coefficient*/

  FTM_RF_DEBUG_LOAD_DPD_PROP_KERNEL_WEIGHT_Q = 7, /*!< 7 Kernel Weight Q coefficient*/

  FTM_RF_DEBUG_LOAD_DPD_PROP_PLL_ID = 8, /*!< 8 : PLL ID*/

  FTM_RF_DEBUG_LOAD_DPD_PROP_ETVD_MAX = 9,   /*!< 9 : Bias */

  FTM_RF_DEBUG_LOAD_DPD_PROP_ETVD_MIN = 10,  /*!< 10: ETVD Min */

  FTM_RF_DEBUG_LOAD_DPD_PROP_DET = 11, /*!< 11 : Detrough*/

  FTM_RF_DEBUG_LOAD_DPD_PROP_SCALE_DET = 12, /*!< 12 : Scale Detrough*/

  FTM_RF_DEBUG_LOAD_DPD_PROP_NUM  /*!< Max : Defines maximum number of properties */

} ftm_rf_debug_load_dpd_property_type;

extern const char* rf_debug_load_dpd_property_names[];

extern const uint8 ftm_rf_debug_load_dpd_prop_attrib_list[];
/*--------------------------------------------------------------------------------------------------------------------*/
/*! Enumeration to indicate the type of properties for Set DPD debug Mode Command . 
  These enumeration are used to define the content of #ftm_properties_framework_field_property_type in FTM RF debug command packet
rf_debug_set_dpd_debug_mode_property_names Must be updated when this list is updated.*/
typedef enum
{
  FTM_RF_DEBUG_SET_DPD_DEBUG_MODE_PROP_UNASSIGNED = 0, /*!< 0 : Unassigned property */

  FTM_RF_DEBUG_SET_DPD_DEBUG_MODE_PROP_SIG_PATH = 1, /*!< 1 : Signal Path*/

  FTM_RF_DEBUG_SET_DPD_DEBUG_MODE_PROP_RFM_DEVICE = 2, /*!< 2 : RFM device*/

  FTM_RF_DEBUG_SET_DPD_DEBUG_MODE_PROP_DEBUG_MODE = 3, /*!< 3 DPD Debug Mode*/

  FTM_RF_DEBUG_SET_DPD_DEBUG_MODE_PROP_PLL_ID = 4, /*!< 4 PLL ID*/

  FTM_RF_DEBUG_SET_DPD_DEBUG_MODE_PROP_NUM  /*!< Max : Defines maximum number of properties */

} ftm_rf_debug_set_dpd_debug_mode_property_type;

extern const char* rf_debug_set_dpd_debug_mode_property_names[];

extern const uint8 ftm_rf_debug_set_dpd_debug_mode_prop_attrib_list[];

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Enumeration to indicate the type of properties for IDC Cal Command. 
  These enumeration are used to define the content of #ftm_properties_framework_field_property_type in FTM RF debug command packet
  Must be updated when this list is updated.*/
typedef enum
{
  FTM_RF_DEBUG_IDC_CAL_PROP_UNASSIGNED = 0, /*!< 0 : Unassigned property */

  FTM_RF_DEBUG_IDC_CAL_PROP_CAL_TYPE = 1, /*!< 7 : IDC Cal type -- IIP2/JRF/TxRSB/RxRSB/OZIFRSB */

  FTM_RF_DEBUG_IDC_CAL_PROP_IQ_CAPT = 2, /*!< 9 : IQ Capture */

  FTM_RF_DEBUG_IDC_CAL_PROP_DIS_TX_LOOPBACK = 3, /*!< Disable Tx Loopback*/

  //IIP2 Result Result
  FTM_RF_DEBUG_IDC_IIP2_CAL_PROP_TX_OFFSET = 4, /*!< 11 : IIP2 Cal -- Tx offset kHz*/

  FTM_RF_DEBUG_IDC_IIP2_CAL_PROP_I_CODE = 5, /*!< 12 : IIP2 Cal -- I CODE*/

  FTM_RF_DEBUG_IDC_IIP2_CAL_PROP_Q_CODE = 6, /*!< 12 : IIP2 Cal -- Q CODE*/

  FTM_RF_DEBUG_IDC_IIP2_CAL_PROP_IM2_POWER_REF_CODE = 7, /*!< 14 : IM2 power corresponding to the ref code*/

  FTM_RF_DEBUG_IDC_IIP2_CAL_PROP_IM2_BEST_POWER = 8, /*!< 15 : Result of the IIP2 post processing algorithm : IM2 power corresponding to the 
   best IQ mixer code */

  FTM_RF_DEBUG_IDC_IIP2_CAL_PROP_POWER_BEST = 9, /*!< 16 : IIP2 power corresponding to best code in db100 units*/

  FTM_RF_DEBUG_IDC_IIP2_CAL_PROP_POWER_DELTA = 10, /*!< 17 : Delta dBc computed from the IIP2 power of best code (Result of the IIP2 post processing algorithm) and
          optimized value provided by ATE team in dbc100 units*/
  
  FTM_RF_DEBUG_IDC_IIP2_CAL_THERM_ADC = 11, /*!< 18 : IIP2 Therm adc reading*/


  FTM_RF_DEBUG_IDC_CAL_PROP_NUM  /*!< Max : Defines maximum number of properties */

} ftm_rf_debug_idc_cal_property_type;

extern const char* rf_debug_idc_cal_property_names[];

extern const uint8 ftm_rf_debug_idc_cal_prop_attrib_list[];

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Enumeration to indicate the type of properties for Tech Enter/Exit Command. 
    These enumeration are used to define the content of #ftm_properties_framework_field_property_type in FTM RF debug command packet
    command_discovery_property_names Must be updated when this list is updated.*/
typedef enum
{
  FTM_RF_DEBUG_TECH_ENTER_EXIT_PROP_UNASSIGNED = 0, /*!< 0 : Unassigned property */

  FTM_RF_DEBUG_TECH_ENTER_EXIT_PROP_MODE = 1, /*!< 1 : Specifies whether to execute tech enter or exit*/

  FTM_RF_DEBUG_TECH_ENTER_EXIT_ENABLE_COOKBOOK_LOAD = 2, /*!< 2 : Specifies whether to enable cookbook load*/

  FTM_RF_DEBUG_TECH_ENTER_EXIT_PROP_NUM  /*!< Max : Defines maximum number of properties */

} ftm_rf_debug_tech_enter_exit_property_type;

extern const char* rf_debug_tech_enter_exit_property_names[];

extern const uint8 ftm_rf_debug_tech_enter_exit_prop_attrib_list[];

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Enumeration to indicate the type of properties for Tech Enter/Exit Command. 
    These enumeration are used to define the content of #ftm_properties_framework_field_property_type in FTM RF debug command packet
    command_discovery_property_names Must be updated when this list is updated.*/
typedef enum
{
  FTM_RF_DEBUG_DEVICE_CAL_PROP_UNASSIGNED      = 0,  /*!< 0 : Unassigned property */

  FTM_RF_DEBUG_DEVICE_CAL_PROP_CAL_TYPE        = 1,  /*!< 1 : Specifies the type of cal*/

  FTM_RF_DEBUG_DEVICE_CAL_PROP_IDC1_STATUS     = 2,  /*!< 2 : IDC1 status */
  
  FTM_RF_DEBUG_DEVICE_CAL_PROP_IDC2_STATUS     = 3,  /*!< 3 : IDC2 status */
  
  FTM_RF_DEBUG_DEVICE_CAL_PROP_IDC1_DATA       = 4,  /*!< 4 : IDC1 data */
  
  FTM_RF_DEBUG_DEVICE_CAL_PROP_IDC2_DATA	   = 5,  /*!< 5 : IDC2 data */

  FTM_RF_DEBUG_DEVICE_CAL_PROP_NUM  /*!< Max : Defines maximum number of properties */

} ftm_rf_debug_device_cal_property_type;

typedef struct 
{ 
  uint8  attr_bmsk;
  boolean is_blob_type;
} ftm_rf_debug_device_cal_property_info_t;

extern const char* rf_debug_device_cal_property_names[];

extern const uint8 ftm_rf_debug_device_cal_prop_attrib_list[]; //Keep this list for compatibility

extern const ftm_rf_debug_device_cal_property_info_t ftm_rf_debug_device_cal_prop_info_list[];


/*--------------------------------------------------------------------------------------------------------------------*/
/*! Enumeration to indicate the type of properties for dcoc override Command . 
  These enumeration are used to define the content of #ftm_properties_framework_field_property_type in FTM RF debug command packet
rf_debug_dcoc_override_property_names Must be updated when this list is updated.*/
typedef enum
{
  FTM_RF_DEBUG_DCOC_OVERRIDE_PROP_UNASSIGNED = 0, /*!< 0 : Unassigned property */

  FTM_RF_DEBUG_DCOC_OVERRIDE_PROP_RFM_DEVICE = 1, /*!< 1 : RFM device*/

  FTM_RF_DEBUG_DCOC_OVERRIDE_PROP_CC_INDEDX = 2, /*!< 2 : CC Index*/

  FTM_RF_DEBUG_DCOC_OVERRIDE_PROP_DCOC_I = 3, /*!< 3 DCOC_I */

  FTM_RF_DEBUG_DCOC_OVERRIDE_PROP_DCOC_Q = 4, /*!< 4 DCOC_Q */

  FTM_RF_DEBUG_DCOC_OVERRIDE_PROP_NUM  /*!< Max : Defines maximum number of properties */

} ftm_rf_debug_dcoc_override_property_type;

extern const char* rf_debug_dcoc_override_property_names[];

extern const uint8 ftm_rf_debug_dcoc_override_prop_attrib_list[];


/*--------------------------------------------------------------------------------------------------------------------*/

/*! Enumeration to indicate the type of xpt mode */
typedef enum
{
  FTM_RF_DEBUG_XPT_MODE_APT    = 0, /*!< 0 : APT Mode  */
  FTM_RF_DEBUG_XPT_MODE_EPT    = 1, /*!< 1 : EPT Mode  */
  FTM_RF_DEBUG_XPT_MODE_ET     = 2, /*!< 1 : ET Mode  */
  FTM_RF_DEBUG_XPT_MODE_MAX         /*!< Max */

} ftm_rf_debug_xpt_mode_type;

/*! Enumeration to indicate the type of IQ Gain Action */
typedef enum
{
  FTM_RF_DEBUG_IQ_GAIN_ACTION_SKIP        = 0, /*!< 0 : Skip IQ override  */
  FTM_RF_DEBUG_IQ_GAIN_ACTION_OVERRIDE    = 1, /*!< 1 : override with param value  */
  FTM_RF_DEBUG_IQ_GAIN_ACTION_SW_DEFAULT  = 2, /*!< 1 : override with sw default  */
  FTM_RF_DEBUG_IQ_GAIN_ACTION_NV          = 3, /*!< 1 : override with NV value  */
  FTM_RF_DEBUG_IQ_GAIN_ACTION_MAX              /*!< Max */

} ftm_rf_debug_iq_gain_action_type;


/*! Enumeration to indicate the type of TxFe Gain Action */
typedef enum
{
  FTM_RF_DEBUG_TXFE_GAIN_ACTION_SKIP        = 0, /*!< 0 : Skip TxFE override  */
  FTM_RF_DEBUG_TXFE_GAIN_ACTION_OVERRIDE    = 1, /*!< 1 : override with param value  */
  FTM_RF_DEBUG_TXFE_GAIN_ACTION_SW_DEFAULT  = 2, /*!< 1 : override with sw default  */
  FTM_RF_DEBUG_TXFE_GAIN_ACTION_NV          = 3, /*!< 1 : override with NV value  */
  FTM_RF_DEBUG_TXFE_GAIN_ACTION_MAX              /*!< Max */

} ftm_rf_debug_txfe_gain_action_type;

/*! Enumeration to indicate the type of Env Scale Action */
typedef enum
{
  FTM_RF_DEBUG_ENV_SCALE_ACTION_SKIP        = 0, /*!< 0 : Skip env scale override  */
  FTM_RF_DEBUG_ENV_SCALE_ACTION_OVERRIDE    = 1, /*!< 1 : override with param value  */
  FTM_RF_DEBUG_ENV_SCALE_ACTION_SW_DEFAULT  = 2, /*!< 1 : override with sw default  */
  FTM_RF_DEBUG_ENV_SCALE_ACTION_NV          = 3, /*!< 1 : override with NV value  */
  FTM_RF_DEBUG_ENV_SCALE_ACTION_MAX              /*!< Max */

} ftm_rf_debug_env_scale_action_type;

/*! Enumeration to indicate the type of Delay Action */
typedef enum
{
  FTM_RF_DEBUG_DELAY_ACTION_SKIP        = 0, /*!< 0 : Skip delay override  */
  FTM_RF_DEBUG_DELAY_ACTION_OVERRIDE    = 1, /*!< 1 : override with param value  */
  FTM_RF_DEBUG_DELAY_ACTION_SW_DEFAULT  = 2, /*!< 1 : override with sw default  */
  FTM_RF_DEBUG_DELAY_ACTION_NV          = 3, /*!< 1 : override with NV value  */
  FTM_RF_DEBUG_DELAY_ACTION_MAX              /*!< Max */

} ftm_rf_debug_delay_action_type;

/*! Enumeration to indicate the type of tx functionality to perform */
typedef enum
{
  FTM_RF_DEBUG_TX_FUNCTION_SET_TX_STATE     = 0, /*!< 0 : perform set tx state  */
  FTM_RF_DEBUG_TX_FUNCTION_SET_TX_OVERRIDE  = 1, /*!< 1 : perform tx override */
  FTM_RF_DEBUG_TX_FUNCTION_MAX              /*!< Max */

} ftm_rf_debug_tx_function_type;

/*! Enumeration to indicate the type of debug modes for DPD capture */
typedef enum
{
  FTM_RF_DEBUG_POST_DPD     = 0, /*!< 0 : Post DPD  */
  FTM_RF_DEBUG_PRE_DPD  = 1, /*!< 1 : Pre DPD */
  FTM_RF_DEBUG_DEBUG_MODE_MAX             /*!< Max */

} ftm_rf_debug_dpd_debug_mode_type;


/*! Enumeration to indicate the type of tech enter/exit functionality */
typedef enum
{
  FTM_RF_DEBUG_TECH_ENTER             = 0, /*!< 0 : Tech Enter mode */
  FTM_RF_DEBUG_TECH_EXIT              = 1, /*!< 1 : Tech Exit mode */
  FTM_RF_DEBUG_TECH_ENTER_EXIT_MAX         /*!< Max */
} ftm_rf_debug_tech_enter_exit_mode_type;

/*! Enumeration to indicate the type of device cal */
typedef enum
{
  FTM_RF_DEBUG_DAC_CAL              = 1, /*!< 1 : DAC Cal */
  FTM_RF_DEBUG_DAC_AND_QETDEVICE_CAL        = 2,   /*!< 1 : QET Device Cal */
  FTM_RF_DEBUG_ADC_CAL             = 3,     /*!< 1 : ADC Cal */
  FTM_RF_DEBUG_DAC_V2_CAL  =  4,            /*!< 4 : DAC V2 Cal */
  FTM_RF_DEBUG_ADC_AND_DAC_V2_CAL = 5,      /*!< 5 : ADC and DAC V2 Cal */
  FTM_RF_DEBUG_ADC_AND_DAC_AND_QET_CAL = 6,      /*!< 8 : ADC and DAC and QET Cal */
  FTM_RF_DEBUG_DAC_AND_ADC_VCM_CAL             = 7,      /*FBRx ADC VCM Cal*/
  FTM_RF_DEBUG_ADC_AND_DAC_AND_QET_CAL_V2 = 8,      /*!< 8 : ADC and DAC and QET Cal V2: Disable ADC blob data*/
  FTM_RF_DEBUG_DEVICE_CAL_MAX         /*!< Max */
} ftm_rf_debug_device_cal_type;

/*! @} */

/*====================================================================================================================*/
/*!
  @name IQ Capture Property ID

  @brief
  IQ Capture Property ID list
*/
/*! @{ */

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Enumeration to indicate the type of properties for FTM IQ Capture Command. 
  These enumeration are used to define the content of #ftm_properties_framework_field_property_type in FTM RF Debug command packet
iq_capture_property_names Must be updated when this list is updated.*/
typedef enum
{
  FTM_RF_DEBUG_IQ_CAPTURE_PROP_UNASSIGNED = 0, /*!< 0 : Unassigned property */

  FTM_RF_DEBUG_IQ_CAPTURE_PROP_NUM,  /*!< Max : Defines maximum number of properties */

} ftm_rf_debug_iq_capture_property_type;

extern const char* rf_debug_iq_capture_property_names[];

/*--------------------------------------------------------------------------------------------------------------------*/

#ifdef T_WINNT
#error code not present
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ftm_rf_debug_INTERFACE_H */

