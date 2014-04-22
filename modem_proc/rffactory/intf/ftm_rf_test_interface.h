#ifndef FTM_RF_TEST_INTERFACE_H
#define FTM_RF_TEST_INTERFACE_H
/*!
  @file
  ftm_rf_test_interface.h

  @brief
  Common framework to perform radio test in FTM mode
*/

/*======================================================================================================================

  Copyright (c) 2015 - 2022 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //components/rel/rffactory.mpss/1.0/intf/ftm_rf_test_interface.h#172 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------------------------------------------------
12/21/22   waq     Add new properties in RRA and Radio Config for NV/Bandwidth Availability check-up
04/15/22   yanwa   [FR77441] Add new FTM XPT internal test framework.
03/30/22   pwj     Add new FTM command and properties for Verify Debug Config
03/30/22   yanwa   [FR77647] RFLTE time profiling for FTM automation
03/23/22   wyc     Add capture type for vswr in enum ftm_rf_test_iq_capture_type
03/14/22   yanwa   Add new Radio Config properties for UL Switch
02/14/22   yka     Add new property for UL Tx Switch Type in RRA
12/09/21   adawn   Sleep Control Response Property for command exec time
12/06/21   pwj     Add new properties in RadioConfig and RRA for supporting 8Rx
08/12/21   adawn   Sleep Control Command Support
04/16/21   dj      Add Input and output Properties to support SRS CS in RRA
02/24/21   dj      Add new FTM RRA property FTM_TRM_RRA_PROP_V2_CARRIER_CONTROL
07/02/20   fxd     add new property for GSM selftest PVT absolute value
05/15/20   pwj     Add a new property type for configuring droop compensation dynamically
05/12/20   hd      Fetching alternate Tx antenna paths during RRA query
05/07/20   hep     Added placeholder for Tx Gain Adjustment (Radio Configure) for GNB
04/08/20   hep     Added placeholder for Waveform Immediate Trigger for GNB
04/07/20   dpk     Add new RRA and Radio Config Properties for NB_ID and LANE_ID
02/27/20   dj      Add new FTM RRA property FTM_TRM_RRA_PROP_V2_MSIM_TUNE_AWAY
01/30/20   hep     Add AGC_DBFS property placeholder for GNB
12/26/19   dpk     FR60431:Enable UE power class setting for NR
12/14/19   JC      CR2585642: QDTA support via QRCT IRAT panel
12/04/19   hep     Merged gNodeB specific RF Test Rx Measure property enums
11/09/19   lzh     FR57112: Multi-tones Estimated Sensitivity
09/06/19   dpk     Sending NS value in Radio Config for Sub6
07/10/19   gz      Adding the Technology property for Radio Config. command
07/01/19   JC      mmW IRAT support
06/06/19   sr      Adding Command Capability Command
06/04/19   ud      Added NDR State for Radio Config
04/18/19   hp      Added Sub Carrier Spacing for Radio Configure
03/20/19   hp      Added measurement type property for Rx Measure
01/14/19   sb      Add power block on symbol basis for tx control command
11/01/18   sb      Add MPE control DFT Distance property types
10/17/18   sb      Added RX Measure prop types for RSB freq and power dbc
09/10/18   sb      Added TECH_MODE property type in Radio Config
09/06/18   sb      Adding BWP_START_LOC property type for RRA
09/03/18   dpk     Adding FTM Sawless Support to RRA and Radio Config for LTE
08/07/18   xj      Change debug error code data type from int64 to uint64
07/25/18   sb      Add PCFL24 enum value for iq capture format
07/12/18   sb      Add Radio Config property Bandwidth part location
07/09/18   pvsr    Add Supoort for Sub Tech V2X
07/02/18   xj      add a new property to hold Tx Measure detailed error code
06/14/18   ld      [FR46503]Add data context information in IQ capture response
05/07/18   sb      Add MPE control sub-command property types
05/02/18   vs      Add Tx control prop for FR50490
04/03/18   cth     Add I/Q Capture Type to Rx Measure
03/29/18   jps     Add SEM and OBW measurement properties to TX Measure
03/13/18   jps     New RX_MEASURE properties for averaging
02/27/18   cth     Included Sub-Tech prop/value in FTM RF Test Radio Config Command.
01/12/18   kc      [Selftest]Changes for ACLR Noise floor reporting
01/09/18   cth     Included IF Frequency for return prop/value in FTM RF Test Radio Config Command.
11/12/17   xsh     Add selftest vswr lse property
10/13/17   zhw     Added time profiling response support for FTM RF Test command
10/11/17   dbz     Add TX measure prop for FR 44645
10/05/17   pkg     Added UE Power Class support for Tx Control and UTI query in FTM.
07/30/17   fz      Add property to support RFVFS LTE TDD bursted mode
05/10/17   zhw     Add VCO ID support to FTM RF TEST framework
05/04/17   cth     Updated to include support for nr5g rf test
04/20/17   ld      Add extended ORFS modulation freq offset
04/05/17   zhw     Added support for returning antenna number as part of RRA
01/11/17   zhw     Updated response length field in subsys128 diag header from 2 bytes to 4 bytes
08/29/16   dyx     Add expected_agc/override lna state for iq capture
02/08/17   stl     Add support for RX Meas No-tone Est Sensitivity spur dynamic update
01/11/17   zhw     Updated response length field in subsys128 diag header from 2 bytes to 4 bytes
01/05/17   stl     Add support for RX Meas No-tone Est Sensitivity update
11/23/16   dbz     Add support for RX Meas No-tone Est Sensitivity
10/18/16   jps     Add support for TX Meas FBRX switchpoint char
09/13/16   ld      Add GSM responses: PVT num passed/requested
09/02/16   ld      Add GSM responses: PVT pass/fail flag, PVT pass ratio, ORFS in dBm, phase error peak
                   Add GSM OOS action, response
08/24/16   jfc     Change Burst Pattern to Num Rx Bursts
08/05/16   jfc     Add support for Rx Measure - Rx gain control type property
08/04/16   jfc     Add override LNA gain state property to Rx Meas
08/01/16   ld      Correct the GSM ORFS modulation and switching definition
07/21/16   ld      Add GSM PVT test support
07/21/16   jfc     Define FTM_RF_TEST_RADIO_CFG_PROP_RFM_DEVICE_PRI for legacy PLs
07/20/16   jfc     Merge PRx and DRx device properties into one (radio cfg)
06/24/16   jfc     Rename rx_mode continuous_mode and add burst_pattern property
06/21/16   jfc     Add wait_trigger subcommand to FTM RF Test
06/07/16   jps     Add new properties to TX measure (GSM measurements)
06/06/16   jfc     Rename ftm_rf_test functions with ftm_properties_framework prefix
06/02/16   jps     Use DEBUG_DATA property for blob data
05/25/16   jps     TX Measure - Remove Sig Path and Antenna Path properties
05/16/16   chm     Added new property type for rx measure cmd
05/11/16   jps     Add new properties for sig path & ant switch path in TX Measure
05/06/16   jfc     Added support for freq adj in radio config
04/28/16   chm     Return the IQ samples used to compute the IQ reports
04/20/16   jfc     Added support for diag subsys128 and multi response packets
04/10/16   zhw     Added support for FTM IRAT CONFIG
04/04/16   zhw     Addeds support for New FTM irat Framework
03/31/16   zhw     Moved RF Test Name Info Display to rffactory component
03/25/16   jps     Added support for TX MEASURE command
03/22/16   zhw     Added support for MSIM CONFIG in radio config
03/21/16   Saul    [FTM_TRM] Antenna Path support.
03/18/16   zhw     Added support for Ant path in radio config
03/08/16   zhw     Added support for Sensitivty, CtoN, Peak Freq measurement in Rx Measure
02/24/16   vv      [FTM_RF_TEST] - Adding enum ftm_trm_rra_property_type
02/24/16   jfc     Add IQ capture subcommand code and enums
02/19/16   br      Added support for Mod_type change in FTM mode
02/18/16   jfc     [FTM RF TEST] Move subscriber and tech out of RF test header
02/16/16   zc      Adding Number of Rx Bursts for GSM
01/18/16   Saul    [FTM_RF_TEST] Recommended Radio Allocations API
01/04/16   zhw     Remove LTE PATH IDX property
12/14/15   zhw     Added Tx slot & Tx waveform support for GSM
11/20/15   zhw     Added support for LTE Path Index in Tx control
11/15/15   zhw     Added support for LTE Path Index in Rx Measure
10/27/15   zhw     Added support for LTE Path Index & Tx Control Properties
10/22/15   zhw     Added support for sig path property in rx measure and Tx control
09/16/15   zhw     Added support for sig path property in radio config
08/11/15   zhw     Added support for Tx Control Command
08/07/15   zhw     Update Rx Measure Property List. Add Rx Mode support in Radio Config
08/04/15   zhw     Added Tech Override support for Rx Measure Command
07/29/15   zhw     Moved REQ & RSP payload type to interface
                   Added support for Rx Measure command
04/15/15   aro     Added subcriber info in radio config unpack data structure
04/15/15   aro     Doxygen fix
04/15/15   aro     Added error codes to detect uncompression failures
04/15/15   aro     Added support for compressed packet
04/15/15   aro     PACK fix for OFT
04/14/15   aro     Extern C
04/14/15   aro     Added Radio Congig test case
04/14/15   aro     Added generic dispatch mechanism
04/13/15   aro     [1] Added support for radio config handler function
                   [2] Added error reporting
04/13/15   aro     Added common response infrastructure
04/13/15   aro     Restructuring/Renaming
04/13/15   aro     Renamed filenames
04/13/15   aro     Added radio config unpack function
04/08/15   aro     [1] Added error code
                   [2] Rename type/fuunc/variable name
                   [3] Initial support for radio config handler
04/08/15   aro     Fixed subscriber ID enum name
04/08/15   aro     Added Subscriber ID
04/08/15   aro     Added command ID
04/08/15   aro     Added initial type definition
04/08/15   aro     Initial Release

======================================================================================================================*/

#include "comdef.h"

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef T_WINNT
#error code not present
#endif

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Type definition to define the size of the reserved field in FTM test command packet.  */
typedef uint16 ftm_rf_test_comamnd_type;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Type definition to define the size of the reserved field in FTM test command packet.  */
typedef uint32 ftm_rf_test_field_reserved_type;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Type definition to define the size of the version field in FTM test command packet.  */
typedef uint32 ftm_rf_test_field_version_type;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Type definition to define the size of the subscription field in FTM test command packet.  */
typedef uint32 ftm_rf_test_field_subscriber_type;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Type definition to define the size of the technology field in FTM test command packet.  */
typedef uint32 ftm_rf_test_field_technology_type;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Type definition to define the error code mask for response packet. Each bit indicates a type of error. */
typedef uint32 ftm_rf_test_field_error_code_mask_type;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Type definition to define the debug error code mask for response packet. Each bit indicates a type of error. */
typedef uint64 ftm_rf_test_field_debug_error_code_mask_type;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Type definition to define the additional set 1 error code mask for response packet. Each bit indicates a type of error. */
typedef uint64 ftm_rf_test_field_additional_error_code_set_1_mask_type;

/*! Type definition to define bwp start loc */
typedef uint32 ftm_rf_test_bwp_start_loc_t;

/*! Type definition for sub carrier spacing  */
typedef uint32 ftm_rf_test_scs_type_t;

/*====================================================================================================================*/
/*!
  @brief
  structure of  type info for FTM Rx Measure Properties
*/
typedef struct
{
  uint32  attr_bmsk;
  boolean is_blob_type;
} ftm_rf_test_rx_meas_property_info_t;


/*!
  @brief
  structure of  type info for FTM MPE Control Properties
*/
typedef struct
{
  boolean is_blob_type;
} ftm_rftest_mpe_control_prop_info_t;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Defintion of command header for FTM command request packet */
typedef PACK( struct )
{
  uint8 diag_cmd;
  /*!< Command Identification */

  uint8 diag_subsys_id;
  /*!< Sub-system Identification. For instance - 11 for FTM */

  uint16 rf_subsys_id;
  /*!< RF Mode Identification. 101 for FTM_RF_TEST_C */

  ftm_rf_test_comamnd_type test_command;
  /*!< FTM test command defined by #ftm_rf_test_command_enum_type */

  uint16 req_len;
  /*!< Defines the request length of the FTM diag packet, <b>when the packet is uncompressed</b>. This includes
  the summation of the size of ftm_rf_test_diag_header_type and size of remaning paylaod (when it is uncompressed) */

  uint16 rsp_len;
  /*!< Defines the response length of the FTM diag packet, <b>when the packet is uncompressed</b>. This includes
  the summation of the size of ftm_rf_test_diag_header_type and size of remaning paylaod (when it is uncompressed) */

}
ftm_rf_test_diag_header_type;


/*--------------------------------------------------------------------------------------------------------------------*/
/*! Deprecated. Use v2
  WARNING: This Data type is deprecated and merely kept there due to for compilation compatibility with legacy PLs.
  For all dev purposes please use ftm_rf_test_diag_header_subsys128_request_type*/
typedef PACK( struct )
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

  ftm_rf_test_comamnd_type test_command;
  /*!< FTM test command defined by #ftm_rf_test_command_enum_type */

  uint16 req_len;
  /*!< Defines the request length of the FTM diag packet, <b>when the packet is uncompressed</b>. This includes
  the summation of the size of ftm_rf_test_diag_header_type and size of remaning paylaod (when it is uncompressed) */

  uint16 rsp_len;
  /*!< Defines the response length of the FTM diag packet, <b>when the packet is uncompressed</b>. This includes
  the summation of the size of ftm_rf_test_diag_header_type and size of remaning paylaod (when it is uncompressed) */

}
ftm_rf_test_diag_header_subsys128_type;
/*! WARNING: This Data type is deprecated and merely kept there due to for compilation compatibility with legacy PLs.
  For all dev purposes please use ftm_rf_test_diag_header_subsys128_request_type */

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Defintion of command header for FTM command request packet */
typedef PACK( struct )
{
  uint8 diag_cmd;
  /*!< Command Identification */

  uint8 diag_subsys_id;
  /*!< Sub-system Identification. For instance - 11 for FTM */

  uint16 rf_subsys_id;
  /*!< RF Mode Identification. 101 for FTM_RF_TEST_C */

  ftm_rf_test_comamnd_type test_command;
  /*!< FTM test command defined by #ftm_rf_test_command_enum_type */

  uint32 packet_length;
  /*!< Defines the length of the uncompressed FTM diag packet, <b>when the packet is uncompressed</b>. This includes
  the summation of the size of ftm_rf_test_diag_header_type and size of remaining paylaod (when it is uncompressed).
  When the diag header is used as part of the response packet, this is the length of response packet. */

}
ftm_rf_test_diag_header_subsys128_request_type;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Defintion of command header for FTM command request packet. This replaces ftm_rf_test_diag_header_subsys128_type */
typedef PACK( struct )
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

  ftm_rf_test_comamnd_type test_command;
  /*!< FTM test command defined by #ftm_rf_test_command_enum_type */

  uint32 packet_length;
  /*!< Defines the length of the uncompressed FTM diag packet, <b>when the packet is uncompressed</b>. This includes
  the summation of the size of ftm_rf_test_diag_header_type and size of remaining paylaod (when it is uncompressed).
  When the diag header is used as part of the response packet, this is the length of response packet. */

}
ftm_rf_test_diag_header_subsys128_response_type;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Defintion of command header for FTM RF Test command request packet */
typedef PACK( struct )
{
  ftm_rf_test_field_reserved_type reserved;
  /*!< Reserved Field */

  ftm_rf_test_field_version_type version;
  /*!< Version of packet */

  ftm_rf_test_field_subscriber_type subscriber;
  /*!< Indicates the subscription ID defined by #ftm_rf_test_command_enum_type */

  ftm_rf_test_field_technology_type technology;
  /*!< Indicates the technology (defined by ftm_rf_technology_type) for which the command is sent */

}
ftm_rf_test_command_header_type;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Defintion of command header for FTM RF Test command request packet
  V2 moves subscriber and technology fields out of test command header and into payload */
typedef PACK( struct )
{
  ftm_rf_test_field_reserved_type reserved;
  /*!< Reserved Field */

  ftm_rf_test_field_version_type version;
  /*!< Version of packet */

}
ftm_rf_test_command_header_type_v2;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Defintion of command header for FTM RF Test command request packet */
typedef PACK( struct )
{
  ftm_rf_test_diag_header_type diag_header;
  /*!<  Structure defining the header for FTM request packet */

  ftm_rf_test_command_header_type command_header;
  /*!<  Structure defining the command header for RF Test command packet */

}
ftm_rf_test_req_header_type;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Defintion of command header for FTM RF Test command request packet
  V2 moves subscriber and technology fields out of test command header and into payload */
typedef PACK( struct )
{
  ftm_rf_test_diag_header_type diag_header;
  /*!<  Structure defining the header for FTM request packet */

  ftm_rf_test_command_header_type_v2 command_header;
  /*!<  Structure defining the command header for RF Test command packet */

}
ftm_rf_test_req_header_type_v2;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Defintion of command header for FTM RF Test command response packet */
typedef PACK( struct )
{
  ftm_rf_test_field_reserved_type reserved;
  /*!< Reserved Field */

  ftm_rf_test_field_version_type version;
  /*!< Version of packet */

  ftm_rf_test_field_error_code_mask_type common_error_code;
  /*!< Error code from the common framework */

}
ftm_rf_test_command_rsp_header_type;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Defintion of command header for FTM RF Test command response packet */
typedef PACK( struct )
{
  ftm_rf_test_diag_header_type diag_header;
  /*!<  Structure defining the header for FTM response packet */

  ftm_rf_test_command_rsp_header_type command_header;
  /*!<  Structure defining the command header for RF Test response packet */

}
ftm_rf_test_header_rsp_type;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Deprecated. Use V2. */
typedef PACK( struct )
{
  ftm_rf_test_diag_header_subsys128_type diag_header;
  /*!<  Structure defining the header for FTM response packet using subsys128 */

  ftm_rf_test_command_rsp_header_type command_header;
  /*!<  Structure defining the command header for RF Test response packet */

}
ftm_rf_test_header_rsp_subsys128_type;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Defintion of command header for FTM RF Test command response packet, Diag subsys 128 */
typedef PACK( struct )
{
  ftm_rf_test_diag_header_subsys128_response_type diag_header;
  /*!<  Structure defining the header for FTM response packet using subsys128 */

  ftm_rf_test_command_rsp_header_type command_header;
  /*!<  Structure defining the command header for RF Test response packet */

}
ftm_rf_test_header_rsp_subsys128_v2_type;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Defintion of data information for FTM RF Tx/Rx capture data response packet, Diag subsys 128 */
typedef PACK( struct )
{
  /* Dword0 */
  union
  {
    uint64 Dword0;
    struct
    {
      uint8 tx_format          : 4;
      uint8 rx_format          : 4;
      uint8 capture_type       : 4;
      uint32 reserved0         : 12;
      uint8 tx_sample_capture_config    : 4;
      uint8 rx_sample_capture_config    : 4;
      uint32 num_tx_samples     : 16;
      uint32 num_rx_samples     : 16;
    };
  };

  /* Dword1 */
  union
  {
    uint64 Dword1;
    struct
    {
      uint32 offset_tx_samples;
      uint32 offset_rx_samples;
    };
  };

  /* Dword2 */
  union
  {
    uint64 Dword2;
    struct
    {
      uint32 tx_sample_rate;
      uint32 rx_sample_rate;
    };
  };

}
iq_sample_debug_info_t;

/*====================================================================================================================*/
/*!
  @addtogroup FTM_TEST_TOOLS_CID
  @{
*/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Enumeration to indicate the type of commands available for FTM test command */
typedef enum
{
  FTM_RF_TEST_CMD_UNASSIGNED = 0, /*!< 0 : Unassigned command */

  FTM_RF_TEST_CMD_RADIO_CONFIGURE = 1, /*!< 1 : Radio Configure command */

  FTM_RF_TEST_CMD_RX_MEASURE = 2, /*!< 2 : Rx Measure command */

  FTM_RF_TEST_CMD_TX_CONTROL = 3, /*!< 3 : Tx Control command */

  FTM_RF_TEST_CMD_QUERY_RECOMMENDED_RADIO_ALLOCATION = 4, /*!< 4 : RF Radio Allocation command */

  FTM_RF_TEST_CMD_RESERVED_5 = 5, /*!< 5 : Reserved  */

  FTM_RF_TEST_CMD_IQ_CAPTURE = 6, /*!< 6 : IQ Capture command */

  FTM_RF_TEST_CMD_TX_MEASURE = 7, /*!< 8 : TX Measure command */

  FTM_RF_TEST_CMD_MSIM_CONFIG = 8, /*!< 8 : MSIM Config command */

  FTM_RF_TEST_CMD_IRAT_CONFIG = 9, /*!< 9 : IRAT CONFIG command */

  FTM_RF_TEST_CMD_WAIT_TRIGGER = 10, /*!< 10 :  Wait Trigger command */

  FTM_RF_TEST_CMD_MPE_CONTROL = 11, /*!< 11 :  MPE Control command */

  FTM_RF_TEST_CMD_COMMAND_CAPABILITY_QUERY = 12, /*!< 12 :  Command Capability command */

  FTM_RF_TEST_CMD_SLEEP_CONTROL = 14, /*!< 14 :  Sleep control command */
  
  FTM_RF_TEST_CMD_XPT_CONFIG = 15, /*!< 15 :  XPT Config command */

  FTM_RF_TEST_CMD_RF_VERIFY_DEBUG_CONFIG = 16, /*!< 16 :  RF Verify Debug Configuration */
  
  FTM_RF_TEST_CMD_MCS_CONFIG = 17 , /*!< 17 :  MCS CONFIG */

  FTM_RF_TEST_CMD_GROUP_DELAY = 18 , /*!< 18 :  GROUP delay measurement */

  FTM_RF_TEST_CMD_NUM  /*!< Max : Defines maximum number of command IDs */

} ftm_rf_test_command_enum_type;

/*! @} */ /* FTM_TEST_TOOLS_CID */

/*====================================================================================================================*/
/*!
  @addtogroup FTM_TEST_TOOLS_SUBS
  @{
*/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Enumeration to indicate the type of subscribers */
typedef enum
{
  FTM_RF_TEST_SUBSCRIBER_0 = 0, /*!< 0 : First subscriber */

  FTM_RF_TEST_SUBSCRIBER_1 = 1, /*!< 1 : Second subscriber */

  FTM_RF_TEST_SUBSCRIBER_2 = 2, /*!< 3 : Second subscriber */

  FTM_RF_TEST_SUBSCRIBER_NUM,  /*!< Max : Defines maximum number of subscriber IDs */

  FTM_RF_TEST_SUBSCRIBER_NA = 0xFF  /*!< Not Applicable */

} ftm_rf_test_subscriber_enum_type;

/*! @} */ /* FTM_TEST_TOOLS_SUBS */

/*====================================================================================================================*/
/*!
  @addtogroup FTM_TEST_TOOLS_ERROR_CODE
  @{
*/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Enumeration to indicate the type of subscribers */
typedef enum
{
  FTM_RF_TEST_EC_SUB_ALLOC = 0, /*!< 0 : Indicates subscription allocation failure */

  FTM_RF_TEST_EC_SUB_RELEASE = 1, /*!< 1 : Indicates subscription release failure */

  FTM_RF_TEST_CMD_HANDLER_FAIL = 2, /*!< 2 : Indicates command handlder failure */

  FTM_RF_TEST_EC_NULL_PARAM = 3, /*!< 3 : Indicates NULL parameter */

  FTM_RF_TEST_EC_RSP_CREATION = 4, /*!< 4 : Failure during response creation */

  FTM_RF_TEST_EC_BAD_CMD = 5, /*!< 5 : Bad Command */

  FTM_RF_TEST_EC_BAD_CMD_HANDLER = 6, /*!< 6 : Indicates bad command hander */

  FTM_RF_TEST_EC_UMCOMPRESS_FAIL = 7, /*!< 7 : Uncompress failure */

  FTM_RF_TEST_EC_MALLOC_FAILRE = 8, /*!< 8 : Malloc failure */

  FTM_RF_TEST_EC_NUM  /*!< Max : Defines maximum number of error codes */

} ftm_rf_test_error_code_enum_type;

/*! @} */ /* FTM_TEST_TOOLS_ERROR_CODE */

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
  FTM_RF_TEST_RADIO_CFG_PROP_UNASSIGNED = 0, /*!< 0 : Unassigned property */

  FTM_RF_TEST_RADIO_CFG_PROP_RX_CARRIER = 1, /*!< 1 : Receiver Carrier Number */

  FTM_RF_TEST_RADIO_CFG_PROP_TX_CARRIER = 2, /*!< 2 : Transmit Carrier Number */

  FTM_RF_TEST_RADIO_CFG_PROP_RFM_DEVICE = 3, /*!< 3 : RFM device */

  FTM_RF_TEST_RADIO_CFG_PROP_RESERVED1 = 4,     /*!< 4 : Reserved*/

  FTM_RF_TEST_RADIO_CFG_PROP_BAND = 5, /*!< 5 : Operating Tech Band */

  FTM_RF_TEST_RADIO_CFG_PROP_CHANNEL = 6, /*!< 6 : Operating Tech channel */

  FTM_RF_TEST_RADIO_CFG_PROP_BANDWIDTH = 7,  /*!< 7 : Radio Bandwidth */

  FTM_RF_TEST_RADIO_CFG_PROP_RX_MODE = 8,          /*!< 8 : For backwards compatibility--Rx Mode: Burst or Continous*/
  FTM_RF_TEST_RADIO_CFG_PROP_CONTINUOUS_MODE = 8,  /*!< 8 : Continuous Mode: Burst or Continous*/

  FTM_RF_TEST_RADIO_CFG_PROP_SIG_PATH = 9,  /*!< 9 : Signal Path for AT forward */

  FTM_RF_TEST_RADIO_CFG_PROP_ANT_PATH = 10,  /*!< 10 : Antenna Path */

  FTM_RF_TEST_RADIO_CFG_PROP_FREQ_ADJ_USER_ADJUST = 11,    /*!< 11 : Frequency adjust: User adjustment in ppb */

  FTM_RF_TEST_RADIO_CFG_PROP_FREQ_ADJ_CENTER_FREQ = 12,    /*!< 12 : Frequency adjust: Center frequency in Hz */

  FTM_RF_TEST_RADIO_CFG_PROP_FREQ_ADJ_ENABLE_XO = 13,    /*!< 13 : Frequency adjust: Enable XO */

  FTM_RF_TEST_RADIO_CFG_PROP_FREQ_ADJ_TOTAL_ADJUST = 14,    /*!< 14 : Frequency adjust: Total adjustment made in Hz */

  FTM_RF_TEST_RADIO_CFG_PROP_NUM_RX_BURSTS = 15,    /*!< 15 : Number of Rx slots (for GSM and TDS) */

  FTM_RF_TEST_RADIO_CFG_PROP_BEAM_ID = 16,    /*!< 16 : Beam ID (MMW) */

  FTM_RF_TEST_RADIO_CFG_PROP_SUB_FRAME_CONFIG = 17,    /*!< 17 : Sub-Frame Symbol Configuration (5G) */

  FTM_RF_TEST_RADIO_CFG_PROP_PLL_ID = 18, /*!< 18 : PLL ID */

  FTM_RF_TEST_RADIO_CFG_PROP_EXEC_TIME_US = 19, /*!< 19 : Time in us for the command to execute */

  FTM_RF_TEST_RADIO_CFG_PROP_INTER_FREQ = 20, /*!< 20 : Intermediate Frequency (IF) (MMW) */

  FTM_RF_TEST_RADIO_CFG_PROP_CENTER_FREQ = 21, /*!< 21 : Center Frequency (RF) (MMW) */

  FTM_RF_TEST_RADIO_CFG_PROP_SUB_TECH = 22, /*!< 22 : Sub-Technology (5G) */

  FTM_RF_TEST_RADIO_CFG_PROP_BWP_START_LOCATION = 23, /*!< 23 : Bandwidth part location in units of PRBs */

  FTM_RF_TEST_RADIO_CFG_PROP_PATH_FILTER_TYPE = 24, /*!< 24 : Path Filter Type for supporting Sawless paths*/

  FTM_RF_TEST_RADIO_CFG_PROP_TECH_MODE = 25, /*!< 25 : Start/Stop Radio Config */

  FTM_RF_TEST_RADIO_CFG_PROP_SCS       = 26, /*!< 26 : Sub Carrier Spacing in Hz */

  FTM_RF_TEST_RADIO_CFG_LOAD_CODEBOOK  = 27, /*!< 27 : Load single element codebook */

  FTM_RF_TEST_RADIO_CFG_PROP_NDR_STATE = 28, /*!< 28 : NDR State */

  FTM_RF_TEST_RADIO_CFG_PROP_TECHNOLOGY = 29, /*!< 29 : Technology */

  FTM_RF_TEST_RADIO_CFG_PROP_NETWORK_SIGNAL_VAL = 30,  /*!< 30 : NR5G Network Signaling Value */

  FTM_RF_TEST_RADIO_CFG_PROP_RESERVED2 = 31, /*! < 31 : Reserved2 */

  FTM_RF_TEST_RADIO_CFG_PROP_SELFTEST_TYPE = 32,  /*!< 32: SELFTEST Type */

  FTM_RF_TEST_RADIO_CFG_PROP_UE_POWER_CLASS = 33,  /*!< NR5G UE Power Class Value */

  FTM_RF_TEST_RADIO_CFG_PROP_RESERVED3 = 34, /*!< 34 : RESERVED3 */

  FTM_RF_TEST_RADIO_CFG_PROP_NB_ID = 35, /*!< 35 : NB ID */

  FTM_RF_TEST_RADIO_CFG_PROP_LANE_ID = 36, /*!< 36 : Lane ID */

  FTM_RF_TEST_RADIO_CFG_PROP_TX_GAIN_ADJUSTMENT = 37, /*!< 37 : Tx Gain Adjustment */

  FTM_RF_TEST_RADIO_CFG_PROP_SRS_CS_TYPE = 38, /*!< 38 : SRS CS Type */

  FTM_RF_TEST_RADIO_CFG_PROP_SRS_SOURCE_CARRIER = 39, /*!< 39 : SRS Source Carrier */

  FTM_RF_TEST_RADIO_CFG_PROP_IF_PLL_UNLOCKED = 40, /*!< 40 : IF PLL Unlock status */

  FTM_RF_TEST_RADIO_CFG_PROP_RF_PLL_UNLOCKED_MODULE_BITMASK = 41, /*!< 41 : RF PLL Unlock status for each mmw module */

  FTM_RF_TEST_RADIO_CFG_PROP_SUBCRIPTION_INDEX = 42, /*<42 : Subscription index, used in DSDA Scenario */

  FTM_RF_TEST_RADIO_CFG_PROP_TX_SHARING = 43, /*<43 : indector to show if it is Tx sharing, used in DSDA scenario */

  FTM_RF_TEST_RADIO_CFG_PROP_TX_PRIORITY = 44, /*<44 : Tx priority index, used for Tx sharing in DSDA scenario */
  
  FTM_RF_TEST_RADIO_CFG_BWP_ID = 45, /*!< 45 : BWP Config ID */

  FTM_RF_TEST_RADIO_CFG_BWP_BW = 46, /*!< 46 : BWP Bandwidth */

  FTM_RF_TEST_RADIO_CFG_TARGET_BWP_ID = 47, /*!< 47 : Target tune BWP ID */

  FTM_RF_TEST_RADIO_CFG_BWP_CENTER_KFREQ = 48, /*!< 48 : BWP Center Frequency */

  FTM_RF_TEST_RADIO_CFG_BWP_PRIORITY = 49, /*!< 49 : BWP Priority */

  FTM_RF_TEST_RADIO_CFG_BWP_CUSTOM = 50, /*!< 50 : BWP Custom */

  FTM_RF_TEST_RADIO_CFG_SUB_CFG_ID = 51, /*!< 51 : Subcfg ID 0 and 1BWP Custom */ 

  FTM_RF_TEST_RADIO_CFG_TARGET_SUB_CFG = 52, /*!< 52 : Target Subcfg ID and CC active */

  FTM_RF_TEST_RADIO_CFG_ANT_NUM = 53, /*!< 53 : Rx Antenna Number */

  FTM_RF_TEST_RADIO_CFG_PROP_UE_COMBO_POWER_CLASS = 54, /*!< 54 : NR5G UE Combo Power Class Value */
  
  FTM_RF_TEST_RADIO_CFG_PROP_UL_TX_SWITCH_TYPE = 55, /*!< 55 : UL Tx Switch Type */

  FTM_RF_TEST_RADIO_CFG_PROP_TX_SWITCH_SOURCE_CARRIER = 56, /*!< 56 : Tx Switch Source Carrier */

  FTM_RF_TEST_RADIO_CFG_TUNE_BUILD_SCRIPT_TIME = 57, /*!< 57 : Tune BuildScript Time */  

  FTM_RF_TEST_RADIO_CFG_PROP_TX_CELL_ID = 58, /*!< 58 : Tx cell id */ 

  FTM_RF_TEST_RADIO_CFG_PROP_NETWORK_SIGNAL_VAL_TYPE = 59,  /*!< 59 : NR5G Network Signaling Value Type */

  FTM_RF_TEST_RADIO_CFG_PROP_NV_BW_AVAILABILITY = 60,  /*!< 60 : NV/Bandwidth Availability */

  FTM_RF_TEST_RADIO_CFG_PROP_ANT_ISOLATION_TUNE_TYPE = 61,		/*!< 61 : Antenna isolation Char tune */
  
  FTM_RF_TEST_RADIO_CFG_PROP_ANT_ISOLATION_AGGRESSOR_BAND = 62,		/*!< 62 : Antenna isolation Aggressor Band */

  FTM_RF_TEST_RADIO_CFG_PROP_ANT_ISOLATION_AGGRESSOR_TECH = 63,		/*!< 63 : Antenna isolation Aggressor Tech */

  FTM_RF_TEST_RADIO_CFG_PROP_ANT_ISOLATION_AGGRESSOR_FREQ = 64,		/*!< 64 : Antenna isolation Aggressor Frequency */

  FTM_RF_TEST_RADIO_CFG_PROP_NUM  /*!< Max : Defines maximum number of properties */

} ftm_rf_test_radio_config_property_type;

/* These macro defs are for legacy targets only. Cannot be removed unless we propagate
   change in other components to older PLs */
#define FTM_RF_TEST_RADIO_CFG_PROP_RFM_DEVICE_PRI 3
#define FTM_RF_TEST_RADIO_CFG_PROP_RFM_DEVICE_DIV 4

extern const char *radio_config_property_names[];

/*! @} */

/*====================================================================================================================*/
/*!
  @name Rx measure Property ID

  @brief
  Rx measure Property ID list
*/
/*! @{ */

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Enumeration to indicate the type of properties for FTM Rx Measure Command.
  These enumeration are used to define the content of #ftm_properties_framework_field_property_type in FTM test command packet.
rx_measure_property_names Must be updated when this list is updated.*/
typedef enum
{
  FTM_RF_MEASURE_PROP_UNASSIGNED = 0, /*!< 0 : Unassigned property */

  FTM_RX_MEASURE_PROP_RX_CARRIER = 1, /*!< 1 : Receiver Carrier Number */

  FTM_RX_MEASURE_PROP_RFM_DEVICE = 2, /*!< 2 : RFM device for this measurement*/

  FTM_RX_MEASURE_PROP_EXPECTED_AGC = 3, /*!< 3 : Expected AGC Value */

  FTM_RX_MEASURE_PROP_AGC = 4, /*!< 4 : RxAGC value */

  FTM_RX_MEASURE_PROP_LNA_GAIN_STATE = 5, /*!< 5 : LNA Gain State */

  FTM_RX_MEASURE_PROP_SIG_PATH = 6,  /*!< 6 : Signal Path for AT forward */

  FTM_RX_MEASURE_PROP_RX_MODE = 7,  /*!< 7 : GSM Rx Mode: Burst or Continous*/

  FTM_RX_MEASURE_PROP_RX_SLOT = 8,  /*!< 8 : GSM Rx Slot Number */

  FTM_RX_MEASURE_PROP_NUM_OF_BURST = 9,  /*!< 9 : GSM Number of Burst */

  FTM_RX_MEASURE_PROP_SENSITIVITY = 10,  /*!< 10 : Sensitivity*/

  FTM_RX_MEASURE_PROP_CTON = 11,  /*!< 11 : CtoN Measurement */

  FTM_RX_MEASURE_PROP_PEAK_FREQ = 12,  /*!< 12 : Peak Frequency Measurement*/

  FTM_RX_MEASURE_PROP_FETCH_RAW_IQ_SAMPLES = 13, /*!< 13 : Raw IQ Samples Capture for Rx Measurement*/

  FTM_RX_MEASURE_PROP_NUM_OF_SAMPLES = 14,  /*!< 14 : Number of IQ samples for Rx Measurement*/

  FTM_RX_MEASURE_PROP_IQ_DATA_FORMAT = 15,  /*!< 15 : Format of IQ samples for Rx Measurement*/

  FTM_RX_MEASURE_PROP_SAMP_FREQ = 16,  /*!< 16 : Sampling Frequency in unit of hz for Rx Measurement*/

  FTM_RX_MEASURE_PROP_OVERRIDE_LNA_GAIN_STATE = 17,  /*!< 17 : Override LNA gain state */

  FTM_RX_MEASURE_PROP_RX_GAIN_CONTRL_TYPE = 18, /*!< 18 : Rx gain control type (open or closed loop) */

  FTM_RX_MEASURE_PROP_NO_TONE_SENSITIVITY = 19,  /*!< 19 : No-Tone Sensitivity*/

  FTM_RX_MEASURE_PROP_NO_TONE_SENSITIVITY_SPUR_NUM = 20,  /*!< 20 : number of spur of No-Tone Sensitivity*/

  FTM_RX_MEASURE_PROP_NO_TONE_SENSITIVITY_SPUR_FREQ_OFFSET = 21,  /*!< 21 : spur 1 freq offset for No-Tone Sensitivity*/

  FTM_RX_MEASURE_PROP_NO_TONE_SENSITIVITY_SPUR_LEVEL = 22,  /*!< 22 : spur 1 power level for No-Tone Sensitivity*/

  FTM_RX_MEASURE_PROP_BEAM_ID = 23,  /*!< 23 : Beam ID (NR 5G) */

  FTM_RX_MEASURE_PROP_EXEC_TIME_US = 24, /*!< 24 : Time in us for the command to execute */

  FTM_RX_MEASURE_PROP_SUB_FRAME_CONFIG = 25,    /*!< 25 : Sub-Frame Symbol Configuration (5G) */

  FTM_RX_MEASURE_PROP_NUM_AVERAGES = 26, /*!< 26 : Number of averages -- Request property */

  FTM_RX_MEASURE_PROP_AGC_MIN = 27, /*!< 27 : Min RxAGC value */

  FTM_RX_MEASURE_PROP_AGC_MAX = 28, /*!< 28 : Max RxAGC value */

  FTM_RX_MEASURE_PROP_AGC_STDDEV = 29, /*!< 29 : Standard Deviation of RxAGC value */

  FTM_RX_MEASURE_PROP_CTON_MIN = 30, /*!< 30 : Min CtoN */

  FTM_RX_MEASURE_PROP_CTON_MAX = 31, /*!< 31 : Max CtoN */

  FTM_RX_MEASURE_PROP_CTON_STDDEV = 32, /*!< 32 : Standard Deviation of CtoN */

  FTM_RX_MEASURE_PROP_SENSITIVITY_MIN = 33, /*!< 33 : Min Sensitivity*/

  FTM_RX_MEASURE_PROP_SENSITIVITY_MAX = 34, /*!< 34 : Max Sensitivity*/

  FTM_RX_MEASURE_PROP_SENSITIVITY_STDDEV = 35, /*!< 35 : Standard Deviation of Sensitivity*/

  FTM_RX_MEASURE_PROP_NO_TONE_SENSITIVITY_MIN = 36, /*!< 36 : Min No-Tone Sensitivity*/

  FTM_RX_MEASURE_PROP_NO_TONE_SENSITIVITY_MAX = 37, /*!< 37 : Max No-Tone Sensitivity*/

  FTM_RX_MEASURE_PROP_NO_TONE_SENSITIVITY_STDDEV = 38, /*!< 38 : Standard Deviation of No-Tone Sensitivity*/

  FTM_RX_MEASURE_PROP_IQ_CAPTURE_TYPE = 39, /*!< 39 : I/Q Capture Type*/

  FTM_RX_MEASURE_PROP_MEAS_FREQUENCY_ERROR = 40, /*!< 40: Estimated Frequency error*/

  FTM_RX_MEASURE_PROP_INPUT_CW_TONE_FREQUENCY = 41, /*!< 41: input CW freq for estimated freq error measurement */

  FTM_RX_MEASURE_PROP_RX_FREQUENCY_ERROR = 42, /*!< 42: Report the estimated RX PLL frequency error, in Hz */

  FTM_RX_MEASURE_PROP_APPLY_FREQUENCY_ERROR_TX = 43, /*!<43 Apply the estimated RX frequency error to the TX PLL */

  FTM_RX_MEASURE_PROP_NUM_OF_CAPTURES = 44, /*!< 44: Number of IQ captures */

  FTM_RX_MEASURE_PROP_IQ_SAMPLE_DEBUG_INFO_0 = 45, /*!< 45: I/Q sample Info */

  FTM_RX_MEASURE_PROP_IQ_SAMPLE_DEBUG_INFO_1 = 46, /*!< 46: I/Q sample Info */

  FTM_RX_MEASURE_PROP_IQ_SAMPLE_DEBUG_INFO_2 = 47, /*!< 47: I/Q sample Info */

  FTM_RX_MEASURE_PROP_SUB_TECH = 48, /*!< 48: SUB TECH */

  FTM_RX_MEASURE_PROP_RSB_FREQ = 49, /*!< 49: RSB Frequency */

  FTM_RX_MEASURE_PROP_RSB_POWER_DBC = 50, /*!< 50: RSB Power dbc */

  FTM_RX_MEASURE_PROP_RSB_MEAS_INTEGRITY = 51, /*!< 51: Reliability of the RSB measurements */

  FTM_RX_MEASURE_PROP_MEAS_TYPE = 52, /*!< 52: Measurement type (e.g. WBEE , FFTEE) */

  FTM_RX_MEASURE_PROP_NUM_TONES = 53, /*!< 53: Num of tones */

  FTM_RX_MEASURE_PROP_MULTI_TONES_SENSITIVITY = 54, /* 54: Multiple Tones Sensitivity */

  FTM_RX_MEASURE_PROP_MULTI_TONES_PEAK_FREQ = 55, /* 55: Multiple Tones Peak Freq */

  FTM_RX_MEASURE_PROP_MULTI_TONES_CTON = 56, /* 56: Multiple Tones CtoN */

  FTM_RX_MEASURE_PROP_WAVE_TYPE = 57, /* 57: Rx Waveform Type */

  FTM_RX_MEASURE_PROP_RB_OFFSET = 58, /* 58: Resource Block Offset */

  FTM_RX_MEASURE_PROP_MEAS_BER = 59, /* 59: Measure block error rate action  */

  FTM_RX_MEASURE_PROP_XPORT_BLKS_ERR = 60, /* 60: Number of transport blocks in error */

  FTM_RX_MEASURE_PROP_XPORT_BLKS_TOT = 61, /* 61: Total number of transport blocks */

  FTM_RX_MEASURE_PROP_WAVE_ACTION = 62, /* 62: Rx waveform action */

  FTM_RX_MEASURE_PROP_AGC_DBFS = 63, /*!< 63 : RxAGC value in dB Full Scale */

  FTM_RX_MEASURE_PROP_WAVEFORM_IMMED_TRIGGER = 64, /*!< 64 : Waveform Immediate Trigger indicator*/

  FTM_RX_MEASURE_PROP_DROOP_COMPENSATION = 65, /* 65: Enable droop compensation */

  FTM_RX_MEASURE_PROP_IF_BACKOFF = 66, /* 66: Enable If backoff */

  FTM_RX_MEASURE_PROP_AGC_OUTER_LOOP = 67, /* 67: RxAGC with Outer loop action */

  FTM_RX_MEASURE_PROP_DL_WAVEFORM_TYPE = 68, /* 68:  The input waveform type for LTE*/

  FTM_RX_MEASURE_PROP_WBEE_DBFS10 = 69, /*!< 69 : raw WBEE in dBFS10 */

  FTM_RX_MEASURE_PROP_FFTEE_DBFS10 = 70, /*!< 70 : raw FFTEE in dBFS10 */

  FTM_RX_MEASURE_POWER_MODE = 71,   /*!< Power mode of LPM, DPM, HPM */

  FTM_RX_MEASURE_PROP_PATH_SWITCH = 72, /*!< 72 : SAW/SAWLESS path switch */

  FTM_RX_MEASURE_PROP_PATH_STATE = 73, /*!< 73 : SAW/SAWLESS Path state */

  FTM_RX_MEASURE_PROP_GAIN_DELTA = 74, /*!< 74 : Sawless gain compensation delta */
  
  FTM_RX_MEASURE_PROP_AGC_NV_READ = 75, /*LNA offset picked from QCN */
  
  FTM_RX_MEASURE_PROP_ANT_ISO_MEAS_VALID = 76, /*Indicates Iso measure value is valid or not.*/

  FTM_RX_MEASURE_PROP_NUM  /*!< Max : Defines maximum number of properties */

} ftm_rf_test_rx_measure_property_type;

/*! Enumeration to indicate the type of the IQ capture data format */
typedef enum
{
  FTM_RF_TEST_IQ_FORMAT_8SC    = 0, /*!< 0 : 8SC IQ format  */
  FTM_RF_TEST_IQ_FORMAT_16SC   = 1, /*!< 1 : 16SC IQ format  */
  FTM_RF_TEST_IQ_FORMAT_SLOPE_PH  = 2, /*!< 2 : Tx: Slope, phase: phase first  */
  FTM_RF_TEST_IQ_FORMAT_8SC_ENV_PH  = 2, /*!< 2 : Rx: 8SC, envelop, phase: phase first  */
  FTM_RF_TEST_IQ_FORMAT_PCFL20 = 3, /*!< 3 : 16SC IQ format  */
  FTM_RF_TEST_IQ_FORMAT_NO_TX_SAMPLE = 4, /*!< 4 : No Tx sample. Rx measurement capture  */
  FTM_RF_TEST_IQ_FORMAT_NO_RX_SAMPLE = 4, /*!< 4 : No Rx/FBRx sample. Reflog only  */
  FTM_RF_TEST_IQ_FORMAT_PCFL24 = 5, /*!< 5 : 24 bits, 4 bits for mantissa and 10 bits for I and for Q each */
  FTM_RF_TEST_IQ_FORMAT_MAX,        /*!< Max */
  FTM_RF_TEST_IQ_FORMAT_INVALID = FTM_RF_TEST_IQ_FORMAT_MAX
} ftm_rf_test_iq_capture_format;

/*! Enumeration to indicate the type of the IQ capture data format */
typedef enum
{
  FTM_RF_TEST_IQ_CAPTURE_TYPE_ACLR_LOWER    = 0,  /*!< 0 : ACLR lower  */
  FTM_RF_TEST_IQ_CAPTURE_TYPE_ACLR_CEN_SF   = 1,  /*!< 1 : ACLR center/SF */
  FTM_RF_TEST_IQ_CAPTURE_TYPE_ACLR_UPPER    = 2,  /*!< 2 : ACLR upper  */
  FTM_RF_TEST_IQ_CAPTURE_TYPE_ACLR_CEN_EVM  = 3,  /*!< 3 : ACLR center/EVM  */
  FTM_RF_TEST_IQ_CAPTURE_TYPE_SF            = 4,  /*!< 4 : SF  */
  FTM_RF_TEST_IQ_CAPTURE_TYPE_EVM_SF        = 5,  /*!< 5 : EVM/SF  */
  FTM_RF_TEST_IQ_CAPTURE_TYPE_EVM           = 6,  /*!< 6 : EVM  */
  FTM_RF_TEST_IQ_CAPTURE_TYPE_EVM_ACLR      = 7,  /*!< 7 : EVM/ACLR  */
  FTM_RF_TEST_IQ_CAPTURE_TYPE_GSM           = 8,  /*!< 8 : GSM  */
  FTM_RF_TEST_IQ_CAPTURE_TYPE_RX_SENS       = 9,  /*!< 9 : Rx sensitivity  */
  FTM_RF_TEST_IQ_CAPTURE_TYPE_RX_FREQ_ERR   = 10, /*!< 10 : Rx freq error  */
  FTM_RF_TEST_IQ_CAPTURE_TYPE_LOFT          = 11, /*!< 11 : LOFT  */
  FTM_RF_TEST_IQ_CAPTURE_TYPE_VSWR          = 12, /*!< 11 : VSWR*/
  FTM_RF_TEST_IQ_CAPTURE_TYPE_MAX                 /*!< Max */

} ftm_rf_test_iq_capture_type;

/*! Enumeration to indicate the TX capture config */
typedef enum
{
  FTM_RF_TEST_IQ_CAPTURE_TX_CONFIG_REFLOG_1 = 0,
  FTM_RF_TEST_IQ_CAPTURE_TX_CONFIG_REFLOG_2 = 1,
  FTM_RF_TEST_IQ_CAPTURE_TX_CONFIG_TD_CAPTURE = 2,
  FTM_RF_TEST_IQ_CAPTURE_TX_CONFIG_FD_CAPTURE = 3
} ftm_rf_test_iq_capture_tx_config_type;

/*! Enumeration to indicate the RX capture config */
typedef enum
{
  FTM_RF_TEST_IQ_CAPTURE_RX_CONFIG_TD_CAPTURE = 0,
  FTM_RF_TEST_IQ_CAPTURE_RX_CONFIG_FD_CAPTURE = 1
} ftm_rf_test_iq_capture_rx_config_type;


extern const char *rx_measure_property_names[];
extern const ftm_rf_test_rx_meas_property_info_t ftm_rftest_rx_meas_prop_info_list[];


/*! @} */

/*====================================================================================================================*/
/*!
  @name Tx Control Property ID

  @brief
  Tx Control Property ID list
*/
/*! @{ */

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Enumeration to indicate the type of properties for FTM TX CONTROL Command.
  These enumeration are used to define the content of #ftm_properties_framework_field_property_type in FTM test command packet
tx_control_property_names Must be updated when this list is updated.*/
typedef enum
{
  FTM_TX_CONTROL_PROP_UNASSIGNED = 0, /*!< 0 : Unassigned property */

  FTM_TX_CONTROL_PROP_TX_CARRIER = 1, /*!< 1 : Receiver Carrier Number */

  FTM_TX_CONTROL_PROP_RFM_DEVICE = 2, /*!< 2 : RFM device for this measurement*/

  FTM_TX_CONTROL_PROP_TX_ACTION = 3, /*!< 3 : TX ACTION ENABLE/DISABLE/POWER SWEEP */

  FTM_TX_CONTROL_PROP_TX_POWER = 4, /*!< 4 : Transmit Power */

  FTM_TX_CONTROL_PROP_RB_CONFIG = 5, /*!< 5 : LTE Start RB */

  FTM_TX_CONTROL_PROP_SIG_PATH = 6,  /*!< 6 : Signal Path for AT forward */

  FTM_TX_CONTROL_PROP_NUM_OF_RB = 7,  /*!< 7 : LTE Number of RBs */

  FTM_TX_CONTROL_PROP_TX_WAVEFORM = 8,  /*!< 8 : LTE & GSM Tx waveform */

  FTM_TX_CONTROL_PROP_NETWORK_SIGNAL = 9,  /*!< 9 : LTE Network signalling value */

  FTM_TX_CONTROL_PROP_TX_SLOT = 10,  /*!< 10 : GSM TX SLOT Number */

  FTM_TX_CONTROL_PROP_MODULATION_TYPE = 11,  /*!< 11 : LTE Modulation Type */

  FTM_TX_CONTROL_PROP_TX_ON_DURATION = 12, /*!< 12: Tx ON Duration */

  FTM_TX_CONTROL_PROP_UL_BURST_PATTERN = 13, /*< 13: UL Burst Pattern */

  FTM_TX_CONTROL_PROP_BEAM_ID = 14,    /*!< 14 : Beam ID (NR 5G) */

  FTM_TX_CONTROL_PROP_UE_POWER_CLASS = 15, /*!< 15 : UE Power Class */

  FTM_TX_CONTROL_PROP_TIME_US = 16,    /*!< 16 : Time in us for the command to execute */

  FTM_TX_CONTROL_PROP_SUB_FRAME_CONFIG = 17,    /*!< 17 : Sub-Frame Symbol Configuration (5G) */

  FTM_TX_CONTROL_PROP_TX_POWER_SWEEP_STOP = 18, /*!< 18 : Transmit Stop Power for POWER SWEEP Tx action*/

  FTM_TX_CONTROL_PROP_TX_POWER_SWEEP_STEP = 19, /*!< 19 : Transmit Tx Power step size for POWER SWEEP Tx action */

  FTM_TX_CONTROL_PROP_SUB_TECH = 20,  /*!< 20 : Sub tech information */

  FTM_TX_CONTROL_PROP_NUM_POWER_BLOCK_TRANSITIONS = 21,  /*!< 21 : Number of power block transitions */

  FTM_TX_CONTROL_PROP_TX_POWER_PER_POWER_BLOCK = 22, /*!< 22 : Transmit Power per power block*/

  FTM_TX_CONTROL_PROP_START_SYMBOL = 23,  /*!< 23 : Start symbol of the transition */

  FTM_TX_CONTROL_PROP_STOP_SYMBOL = 24,  /*!< 24 : Stop symbol of the transition */

  FTM_TX_CONTROL_PROP_TIMING_ADVANCE_NANOSECONDS = 25,  /*!< 25 : Timing advance in units of nano seconds */

  FTM_TX_CONTROL_PROP_RESERVED1 = 28,  /*< 28 : RESERVED1 */

  FTM_TX_CONTROL_PROP_WAVEFORM_IMMED_TRIGGER = 29,  /*!< 29 : Waveform immediate trigger indicator */

  FTM_TX_CONTROL_PROP_MIMO_ENABLED = 30,

  FTM_TX_CONTROL_PROP_IS_RA_TYPE_0 = 31,

  FTM_TX_CONTROL_PROP_IF_BACK_OFF = 32,

  FTM_TX_CONTROL_PROP_C1_RATYPE0_START_RB = 33,

  FTM_TX_CONTROL_PROP_C1_RATYPE0_NRB_ALLOC = 34,

  FTM_TX_CONTROL_PROP_C2_RATYPE0_START_RB = 35,

  FTM_TX_CONTROL_PROP_C2_RATYPE0_NRB_ALLOC = 36,

  FTM_TX_CONTROL_PROP_DPD_ITERATIONS = 37,

  FTM_TX_CONTROL_PROP_MTPL = 38,

  FTM_TX_CONTROL_PROP_NUM  /*!< Max : Defines maximum number of properties */

} ftm_rf_test_tx_control_property_type;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Enumeration to indicate the Tx On Duration Duty Cycle */
typedef enum
{
  FTM_RF_TEST_TX_ON_DURATION_50P = 0,  /*!< 0 : 50% Duty Cycle */
  FTM_RF_TEST_TX_ON_DURATION_20P = 1,  /*!< 1 : 20% Duty Cycle */
  FTM_RF_TEST_TX_ON_DURATION_25P = 2,  /*!< 2 : 25% Duty Cycle */
  FTM_RF_TEST_TX_ON_DURATION_40P = 3,  /*!< 3 : 40% Duty Cycle */
  FTM_RF_TEST_TX_ON_DURATION_MAX       /*!< MAX */
} ftm_rf_test_tx_on_duration_type;

extern const char *tx_control_property_names[];

/*! @} */

/*====================================================================================================================*/
/*!
  @name MPE Control Property ID

  @brief
  MPE Control Property ID list
*/
/*! @{ */

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Enumeration to indicate the type of properties for FTM MPE CONTROL Command.
  These enumeration are used to define the content of #ftm_properties_framework_field_property_type in FTM test command packet
mpe_control_property_names Must be updated when this list is updated.*/
typedef enum
{
  FTM_MPE_CONTROL_PROP_UNASSIGNED = 0,                       /*!< 0 : Unassigned property */

  FTM_MPE_CONTROL_PROP_MPE_STATE = 1,                        /*!< 1 : MPE State */

  FTM_MPE_CONTROL_PROP_BEAM_ID_A = 2,                        /*!< 2 : Beam ID for Pair A */

  FTM_MPE_CONTROL_PROP_BEAM_ID_B = 3,                        /*!< 3 : Beam ID for Pair B */

  FTM_MPE_CONTROL_PROP_TYPE1_I_H_OS = 4,                     /*!< 4 : Type1 char data I-H */

  FTM_MPE_CONTROL_PROP_TYPE1_I_V_OS = 5,                     /*!< 5 : Type1 char data I-V */

  FTM_MPE_CONTROL_PROP_TYPE1_Q_H_OS = 6,                     /*!< 6 : Type1 char data Q-H */

  FTM_MPE_CONTROL_PROP_TYPE1_Q_V_OS = 7,                     /*!< 7 : Type1 char data Q-V */

  FTM_MPE_CONTROL_PROP_TYPE1_DETECTION = 8,                  /*!< 8 : Type1 detection */

  FTM_MPE_CONTROL_PROP_TYPE2_MUSIC_DISTANCE = 9,             /*!< 9 : Type2 music distance */

  FTM_MPE_CONTROL_PROP_ACTION_GET_TYPE2_IQ_DATA = 10,        /*!< 10 : Action - Measure type 2 data */

  FTM_MPE_CONTROL_PROP_TYPE2_DFT_DISTANCE = 11,              /*!< 11 : Type2 dft distance */

  FTM_MPE_CONTROL_PROP_TYPE1_I_K_OS = 12,                    /*!< 12 : Type1 char data I-K */

  FTM_MPE_CONTROL_PROP_TYPE1_Q_K_OS = 13,                    /*!< 13 : Type1 char data Q-K */

  FTM_MPE_CONTROL_PROP_TYPE1_SCENARIO = 14,                  /*!< 14 : Type1 Scenario */

  FTM_MPE_CONTROL_PROP_TYPE2_MUSIC_SPECTRUM = 15,            /*!< 15 : Type2 Music Spectrum data in dB100 */

  FTM_MPE_CONTROL_PROP_TYPE2_DFT_SPECTRUM = 16,              /*!< 16 : Type2 DFT Spectrum data in dB100*/

  FTM_MPE_CONTROL_PROP_TYPE2_FAR_FIELD_CANC_DEPTH = 17,      /*!< 17 : Type2 Far Field Cancellation depth in db100 */

  FTM_MPE_CONTROL_PROP_TYPE2_DFT_SNR = 18,                   /*!< 18 : Type2 DFT SNR in dB100 */

  FTM_MPE_CONTROL_PROP_TYPE1_ACTIVE_COVER = 19,              /*!< 19 : Type1 Active Cover */

  FTM_MPE_CONTROL_PROP_TYPE1_COVER_DET_MODE = 20,            /*!< 20 : Type1 Cover Detection mode*/

  FTM_MPE_CONTROL_PROP_BEAM_GROUP_ID = 21,                   /*!< 21 : Beam Group ID */

  FTM_MPE_CONTROL_PROP_TYPE2_WIN_DFT_SPECTRUM = 22,          /*!< 22 : Type2 Windowed DFT Spectrum data in dB100*/

  FTM_MPE_CONTROL_PROP_TYPE2_MC_CANC_WEIGHTS = 23,          /*!< 23 : Type2 MC Cancellation Weight I */

  FTM_MPE_CONTROL_PROP_TYPE2_TRANSIENT_FLAG = 24,            /*!< 24 : Type2 Transient flag */

  FTM_MPE_CONTROL_PROP_TYPE2_MC_CANC_WEIGHT_Q = 25,          /*!< 25 : Type2 MC Cancellation Weight Q */

  FTM_MPE_CONTROL_PROP_MPE_GROUP_BEAM_SET_INDEX = 26,        /*!< 26 : Beam Set index within Beam Group */

  FTM_MPE_CONTROL_PROP_PLATFORM_TYPE = 27,                   /*!< 27 : Platform Type */

  FTM_MPE_CONTROL_PROP_IQ_SAMPLES = 28,                      /*!< 28 : IQ Samples */

  FTM_MPE_CONTROL_PROP_WIN_DFT_PEAK = 29,                    /*!< 29 : Win DFT Peak */

  FTM_MPE_CONTROL_PROP_WIN_DFT_SNR = 30,                     /*!< 30 : Win DFT SNR */

  FTM_MPE_CONTROL_PROP_DFT_THRESHOLD = 31,                   /*!< 31 : Win DFT Threshold */

  FTM_MPE_CONTROL_PROP_NOISE_EST = 32,                       /*!< 32 : Noise Estimation */

  FTM_MPE_CONTROL_PROP_PHASE_BAD_CAPTURE = 33,               /*!< 33 : Phase Bad Capture */

  FTM_MPE_CONTROL_PROP_DFT_FREQ_DEPENDENT_THRESHOLD = 34,    /*!< 34 : DFT Frequency Dependent Threshold */

  FTM_MPE_CONTROL_PROP_ACCUM_DFT_LINEAR_H = 35,              /*!< 35 : DFT Accumalative Linear Layer -- H*/

  FTM_MPE_CONTROL_PROP_ACCUM_DFT_LINEAR_V = 36,              /*!< 36 : DFT Accumalative Linear Layer -- V*/

  FTM_MPE_CONTROL_PROP_WIN_DFT_DISTANCE = 37,                /*!< 37 : Windows DFT Distance */

  FTM_MPE_CONTROL_PROP_DFT_DUAL_THRESHOLD_OFFSET = 38,       /*!< 38 : Offset on top of DFT Threshold */

  FTM_MPE_CONTROL_PROP_NOISE_EST_VEC_OUT = 39,               /*!< 39 : Noise Estimation Vector Out */

  FTM_MPE_CONTROL_PROP_NOISE_BINS_AVG_SIZE = 40,             /*!< 40 : Noise Bins Average Size */

  FTM_MPE_CONTROL_PROP_NOISE_EST_MODE = 41,                  /*!< 41 : Noise Estimation Mode */

  FTM_MPE_CONTROL_PROP_DUAL_LAYER_MODE = 42,                 /*!< 42 : Dual Layer Mode */

  FTM_MPE_CONTROL_PROP_LOG_LIKELYHOOD_RATIO = 43,            /*!< 43 : Likelyhood dB100 */

  FTM_MPE_CONTROL_PROP_JAMMER_DETECTION = 44,                /*!< 44 : Jammer Detection */

  FTM_MPE_CONTROL_PROP_NF_DETECTION = 45,                    /*!< 45 : Near Field Detection */

  FTM_MPE_CONTROL_PROP_NF_FD_CANC_DEPTH = 46,                /*!< 46 : Frequency Dependent Cancellation Depth */

  FTM_MPE_CONTROL_PROP_NF_RAW_ROT = 47,                      /*!< 47 : Raw RoT Value */

  FTM_MPE_CONTROL_PROP_NF_INST_ROT = 48,                     /*!< 48 : Instantaneous RoT Value */

  FTM_MPE_CONTROL_PROP_NF_FILTER_ROT = 49,                   /*!< 49 : Near Field Filter RoT Value */

  FTM_MPE_CONTROL_PROP_NF_ROT_ADAPT_IIR_ALPHA = 50,          /*!< 50 : IIR alpha from sigmoidal function */

  FTM_MPE_CONTROL_PROP_NF_ROT_BIAS_EST = 51,                 /*!< 51 : RoT Bias Estimation */

  FTM_MPE_CONTROL_PROP_NF_FILTER_ROT_VAR = 52,               /*!< 52 : Near Field RoT Variance */

  FTM_MPE_CONTROL_PROP_NF_XLAYER_NORM = 53,                  /*!< 53 : Normalization scale factor in xlayer cancellation */

  FTM_MPE_CONTROL_PROP_FW_RESET = 54,                     /*!< 54 : Input -- FW Reset Flag */

  FTM_MPE_CONTROL_PROP_INTERNAL_RESP = 55,                   /*!< 54 : Input -- Internal Response Flag; 0 -- Internal Only, 1 -- Everything except IQ, more TBD*/

  FTM_MPE_CONTROL_PROP_NUM                                   /*!< Max : Defines maximum number of properties */

} ftm_rf_test_mpe_control_property_type;

extern const char *mpe_control_property_names[];
extern const ftm_rftest_mpe_control_prop_info_t ftm_rftest_mpe_control_prop_info_list[];
/*! @} */

/*====================================================================================================================*/
/*!
  @name Recommended Radio Allocations Property ID

  @brief
  Recommended Radio Allocations Property ID list
*/
/*! @{ */

/*--------------------------------------------------------------------------------------------------------------------*/
/*! DEPRECATION! THE USAGE OF THIS ENUM FAILED TO CONSIDER THE NON-BRANCHING
  RULE OF RFFACTORY, MAKING IT IMPOSSIBLE TO EXTEND THIS LIST. PLEASE USE V2
  FOR ALL FUTUER PURPOSES */
/*! Enumeration to indicate the type of properties for Recommended Radio Allocations Command.
  These enumeration are used to define the content of test command packet.
  ftm_trm_rra_property_names[] Must be updated when this list is updated.*/
typedef enum
{
  FTM_TRM_RRA_PROP_UNASSIGNED          = 0,  /*!< Deprecated Unassigned property */
  FTM_TRM_RRA_PROP_SUB_IDX             = 1,  /*!< Deprecated Subscription Index from ftm_rf_test_subscriber_enum_type */
  FTM_TRM_RRA_PROP_TECH                = 2,  /*!< Deprecated Tech from ftm_rf_technology_type values */
  FTM_TRM_RRA_PROP_BAND                = 3,  /*!< Deprecated Band from sys_band_class_e_type values */
  FTM_TRM_RRA_PROP_CHAN                = 4,  /*!< Deprecated Chan */
  FTM_TRM_RRA_PROP_BANDWIDTH           = 5,  /*!< Deprecated Bandwidth in Hz */
  FTM_TRM_RRA_PROP_RXTX                = 6,  /*!< Deprecated Indicates if request is for Rx or Tx path from ftm_rf_test_device_radio_trx_t*/
  FTM_TRM_RRA_PROP_CHAIN               = 7,  /*!< Deprecated Chain 0/1/2/3/n */
  FTM_TRM_RRA_PROP_CARRIER_IDX         = 8,  /*!< Deprecated Carrier Index 0/1/2/n */
  FTM_TRM_RRA_PROP_RESOURCE            = 9,  /*!< Deprecated Resource from ftm_trm_resource_type values */
  FTM_TRM_RRA_PROP_REASON              = 10, /*!< Deprecated Resource Reason from ftm_trm_reason_type values */
  FTM_TRM_RRA_PROP_PATH_FUNCTIONALITY  = 11, /*!< Deprecated Path Functionality from ftm_trm_path_functionality_type values */
  FTM_TRM_RRA_PROP_DEVICE              = 12, /*!< Deprecated Device from rfm_device_enum_type values */
  FTM_TRM_RRA_PROP_SIGNAL_PATH         = 13, /*!< Deprecated Signal */
  FTM_TRM_RRA_PROP_ANTENNA_PATH        = 14, /*!< Deprecated Antenna Path */

  /* ADD MORE ITEMS ABOVE THIS LINE */
  FTM_TRM_RRA_PROP_NUM  /*!< Deprecated Max : Defines maximum number of properties */

} ftm_trm_rra_property_type;

/*! DEPRECATION! THE USAGE OF THIS ENUM FAILED TO CONSIDER THE NON-BRANCHING
  RULE OF RFFACTORY, MAKING IT IMPOSSIBLE TO EXTEND THIS LIST. PLEASE USE V2
  FOR ALL FUTUER PURPOSES */
extern const char *ftm_trm_rra_property_names[];

typedef enum
{
  FTM_TRM_RRA_PROP_V2_UNASSIGNED          = 0,  /*!< Unassigned property */

  FTM_TRM_RRA_PROP_V2_SUB_IDX             = 1,  /*!< Subscription Index from ftm_rf_test_subscriber_enum_type */

  FTM_TRM_RRA_PROP_V2_TECH                = 2,  /*!< Tech from ftm_rf_technology_type values */

  FTM_TRM_RRA_PROP_V2_BAND                = 3,  /*!< Band from sys_band_class_e_type values */

  FTM_TRM_RRA_PROP_V2_CHAN                = 4,  /*!< Chan */

  FTM_TRM_RRA_PROP_V2_BANDWIDTH           = 5,  /*!< Bandwidth in Hz */

  FTM_TRM_RRA_PROP_V2_RXTX                = 6,  /*!< Indicates if request is for Rx or Tx path from ftm_rf_test_device_radio_trx_t*/

  FTM_TRM_RRA_PROP_V2_CHAIN               = 7,  /*!< Chain 0/1/2/3/n */

  FTM_TRM_RRA_PROP_V2_CARRIER_IDX         = 8,  /*!< Carrier Index 0/1/2/n */

  FTM_TRM_RRA_PROP_V2_RESOURCE            = 9,  /*!< Resource from ftm_trm_resource_type values */

  FTM_TRM_RRA_PROP_V2_REASON              = 10, /*!< Resource Reason from ftm_trm_reason_type values */

  FTM_TRM_RRA_PROP_V2_PATH_FUNCTIONALITY  = 11, /*!< Path Functionality from ftm_trm_path_functionality_type values */

  FTM_TRM_RRA_PROP_V2_DEVICE              = 12, /*!< Device from rfm_device_enum_type values */

  FTM_TRM_RRA_PROP_V2_SIGNAL_PATH         = 13, /*!< Signal */

  FTM_TRM_RRA_PROP_V2_ANTENNA_PATH        = 14, /*!< Antenna Path */

  FTM_TRM_RRA_PROP_V2_ANTENNA_NUMBER      = 15, /*!< Antenna Number */

  FTM_TRM_RRA_PROP_V2_PLL_ID              = 16, /*!< PLL ID */

  FTM_TRM_RRA_PROP_V2_TIME_US             = 17, /*!< Time in us for the command to execute */

  FTM_TRM_RRA_PROP_V2_SUB_TECH            = 18, /*!< Sub Tech*/

  FTM_TRM_RRA_PROP_V2_RFM_CONTEXT         = 19, /*!< Enables different RFM Context in the response*/

  FTM_TRM_RRA_PROP_V2_PATH_FILTER_TYPE    = 20, /*!< Path Filter Type*/

  FTM_TRM_RRA_PROP_V2_BWP_START_LOC       = 21, /*!< Bandwidth part start location */

  FTM_TRM_RRA_PROP_V2_SCS                 = 22, /*!< SCS in Hz */

  FTM_TRM_RRA_PROP_V2_MSIM_TUNE_AWAY      = 23, /*!< Is Tuned Away */

  FTM_TRM_RRA_PROP_V2_NB_ID               = 24, /*!< NB ID */

  FTM_TRM_RRA_PROP_V2_LANE_ID             = 25, /*!< LANE ID */

  FTM_TRM_RRA_PROP_V2_RESERVED1           = 26, /*!< SRS Antenna path read flag  */

  FTM_TRM_RRA_PROP_V2_RESERVED2           = 27, /*!< SRS Antenna path read  */

  FTM_TRM_RRA_PROP_V2_CARRIER_CONTROL     = 28, /*!< Carrier Control flag: BB restriction*/

  FTM_TRM_RRA_PROP_V2_SRS_SOURCE_CARRIER  = 29, /*!< SRS CS Source Carrier*/

  FTM_TRM_RRA_PROP_V2_SRS_CS_STYLE        = 30, /*!< SRS CS Style*/

  FTM_TRM_RRA_PROP_V2_SRS_CS_IMPACTED     = 31, /*!< SRS CS Impacted Carrier*/

  FTM_TRM_RRA_PROP_V2_SRS_ANTENNA_NUMBER  = 32, /*!< SRS Antenna number */

  FTM_TRM_RRA_PROP_V2_ASDIV_CONFIG_MASK   = 33, /*!< Supported ASDIV Configs Mask */

  FTM_TRM_RRA_PROP_V2_FETCH_REGROUP_TABLE = 34, /*!< Action Fetch Regroup Table */

  FTM_TRM_RRA_PROP_V2_REGROUP_COMBO_NUMBER= 35, /*!< Number of Regroup Combo in Regroup Table */

  FTM_TRM_RRA_PROP_V2_ANT_NUMBER_PER_REGROUP_COMBO = 36, /*!< Number of antenna per regroup combo */

  FTM_TRM_RRA_PROP_V2_TX_SWITCH_TYPE = 37, /*!< TX switch type */

  FTM_TRM_RRA_PROP_V2_NV_BW_AVAILABILITY = 38, /*!< NV/Bandwidth Availability */

  /* ADD MORE ITEMS ABOVE THIS LINE */
  FTM_TRM_RRA_PROP_V2_NUM  /*!< Max : Defines maximum number of properties */
} ftm_trm_rra_property_v2_type;

extern const char *ftm_trm_rra_property_v2_names[];
extern const uint8 ftm_trm_rra_prop_v2_attrib_list[];
extern const uint8 ftm_trm_rra_repack_order_properties[];

#define FTM_TRM_RRA_V2_ATTRIB_CRTCL_BMSK  0x01     /* 1 on bit 0 = critical */
#define FTM_TRM_RRA_V2_ATTRIB_NON_CRTCL_BMSK 0x00  /* 0 on bit 0 = Non critical */

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Enumeration to indicate the type of properties for FTM MSIN CONFIG Command.
  These enumeration are used to define the content of #ftm_properties_framework_field_property_type in FTM test command packet.
msim_config_property_names Must be updated when this list is updated.*/
typedef enum
{
  FTM_MSIM_CONFIG_PROP_UNASSIGNED = 0, /*!< 0 : Unassigned property */

  FTM_MSIM_CONFIG_PROP_SUBSCRIBER = 1, /*!< 1 : FTM Subscriber */

  FTM_MSIM_CONFIG_PROP_TECH = 2,       /*!< 2 : SUB FTM RF TECHNOLOGY */

  FTM_MSIM_CONFIG_PROP_SCENARIO = 3,   /*!< 3 : SUB SCENARIO */

  FTM_MSIM_CONFIG_PROP_NUM             /*!< Max : Defines maximum number of properties */

} ftm_rf_test_msim_config_property_type;

extern const char *msim_config_property_names[];

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
  These enumeration are used to define the content of #ftm_properties_framework_field_property_type in FTM test command packet
tx_control_property_names Must be updated when this list is updated.*/
typedef enum
{
  FTM_IQ_CAPTURE_PROP_UNASSIGNED = 0, /*!< 0 : Unassigned property */

  FTM_IQ_CAPTURE_PROP_SUBSCRIBER = 1, /*!< 1 : Subscriber Number */

  FTM_IQ_CAPTURE_PROP_TECHNOLOGY = 2, /*!< 2 : Technology */

  FTM_IQ_CAPTURE_PROP_TX_CARRIER = 3, /*!< 3 : Transmit Carrier Number */

  FTM_IQ_CAPTURE_PROP_RX_CARRIER = 4, /*!< 4 : Receiver Carrier Number */

  FTM_IQ_CAPTURE_PROP_RFM_DEVICE = 5, /*!< 5 : RFM device for this measurement*/

  FTM_IQ_CAPTURE_PROP_SIG_PATH = 6,  /*!< 6 : Signal Path */

  FTM_IQ_CAPTURE_PROP_ACTION_GET_CFG = 7,   /*!< 7 : IQ capture action, get config */

  FTM_IQ_CAPTURE_PROP_ACTION_ACQUIRE = 8,   /*!< 8 : IQ capture action, acquire samples */

  FTM_IQ_CAPTURE_PROP_ACTION_FETCH = 9,     /*!< 9 : IQ capture action, retrieve samples */

  FTM_IQ_CAPTURE_PROP_ACTION_EST_SENS = 10, /*!< 10 : IQ capture action, retrieve estimated sensitivity */

  FTM_IQ_CAPTURE_PROP_ACTION_CTON = 11,     /*!< 11 : IQ capture action, retrieve CtoN */

  FTM_IQ_CAPTURE_PROP_ACTION_PEAK_FREQ = 12, /*!< 12 : IQ capture action, retrieve CtoN */

  FTM_IQ_CAPTURE_PROP_IQ_CAPTURE_SOURCE = 13, /*!< 13 : IQ capture source */

  FTM_IQ_CAPTURE_PROP_SAMPLE_SIZE = 14, /*!< 14 : Sample size */

  FTM_IQ_CAPTURE_PROP_IQ_DATA_FORMAT = 15, /*!< 15 : IQ data format */

  FTM_IQ_CAPTURE_PROP_SAMP_FREQ = 16, /*!< 16 : Sampling frequency */

  FTM_IQ_CAPTURE_PROP_MAX_DIAG_SIZE = 17, /*!< 17 : Maximum support diag size  */

  FTM_IQ_CAPTURE_PROP_SAMPLE_OFFSET = 18, /*!< 18 : Offset of bytes returned */

  FTM_IQ_CAPTURE_PROP_NUM_SAMPLE_BYTES = 19, /*!< 19 : Number of sample bytes returned */

  FTM_IQ_CAPTURE_PROP_EST_SENSITIVITY = 20, /*!< 20 : Estimated sensitivity */

  FTM_IQ_CAPTURE_PROP_CTON = 21, /*!< 21 : Calculated C/N */

  FTM_IQ_CAPTURE_PROP_CW_PEAK_FREQ = 22, /*!< 22 : CW peak frequency */

  FTM_IQ_CAPTURE_PROP_EXPECTED_AGC = 23, /*!< 23 : expected_agc in dbm10 */

  FTM_IQ_CAPTURE_PROP_ACTION_REPORT_LNA_GAIN_STATE = 24,  /*!< 5 :report LNA Gain State */

  FTM_IQ_CAPTURE_PROP_OVERRIDE_LNA_GAIN_STATE = 25, /*!< 25 : override LNA state before IQ capture */

  FTM_IQ_CAPTURE_PROP_RX_GAIN_CONTRL_TYPE = 26, /*!< 26 : return receiver gain control type  */

  FTM_IQ_CAPTURE_PROP_SUB_TECH = 27,

  FTM_IQ_CAPTURE_PROP_NUM,  /*!< Max : Defines maximum number of properties */

} ftm_rf_test_iq_capture_property_type;

extern const char *iq_capture_property_names[];


/*! @} */



/*! @} */ /* FTM_TEST_TOOLS_CMD_IQ_CAPTURE */

/*====================================================================================================================*/
/*!
  @name TX Measure Property ID

  @brief
  TX Measure Property ID list
*/
/*! @{ */

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Enumeration to indicate the type of properties for FTM TX MEASURE Command.
  These enumeration are used to define the content of #ftm_properties_framework_field_property_type in FTM test command packet
tx_measure_property_names Must be updated when this list is updated.*/
typedef enum
{
  FTM_TX_MEASURE_PROP_UNASSIGNED = 0, /*!< 0 : Unassigned property */

  FTM_TX_MEASURE_PROP_NUM_AVERAGES = 1, /*!< 1 : Number of Averages */

  FTM_TX_MEASURE_PROP_TX_CARRIER = 2, /*!< 2 : TX carrier for this measurement*/

  FTM_TX_MEASURE_PROP_TX_DEVICE = 3, /*!< 3 : TX device for this measurement */

  FTM_TX_MEASURE_ACTION_TX_POWER = 4, /*!< 4 : Action: Measure Transimit Power */

  FTM_TX_MEASURE_ACTION_EVM = 5, /*!< 5 : Action - Measure EVM */

  FTM_TX_MEASURE_ACTION_ACLR = 6,  /*!< 6 : Action - Measure ACLR */

  FTM_TX_MEASURE_ACTION_VSWR = 7,  /*!< 7 : Action - Measure VSWR */

  FTM_TX_MEASURE_ACTION_DEBUG_DATA = 8,  /*!< 8 : Action - Generate debug data in blob property format */

  FTM_TX_MEASURE_ACTION_PHASE_ERROR = 9,  /*!< 9 : Action - Measure Phase Error */

  FTM_TX_MEASURE_ACTION_SPECTRUM_FLATNESS = 10,  /*!< 10 : Action - Measure Spectrum Flatness */

  FTM_TX_MEASURE_TX_POWER = 11,  /*!< 11 : Response - TX power measurement */

  FTM_TX_MEASURE_EVM = 12,  /*!< 12 : Response - EVM measurement */

  FTM_TX_MEASURE_ACLR_UTRA1_POS = 13, /*!< 13 : Response - ACLR UTRA1 POS Report */

  FTM_TX_MEASURE_ACLR_UTRA1_NEG = 14, /*!< 14 : Response - ACLR UTRA1 NEG Report */

  FTM_TX_MEASURE_ACLR_UTRA2_POS = 15, /*!< 15 : Response - ACLR UTRA2 POS Report */

  FTM_TX_MEASURE_ACLR_UTRA2_NEG = 16, /*!< 16 : Response - ACLR UTRA2 NEG Report */

  FTM_TX_MEASURE_ACLR_EUTRA_POS = 17, /*!< 17 : Response - ACLR EUTRA POS Report */

  FTM_TX_MEASURE_ACLR_EUTRA_NEG = 18, /*!< 18 : Response - ACLR EUTRA POS Report */

  FTM_TX_MEASURE_VSWR_RATIO = 19, /*!< 19 : Response - VSWR ratio Report */

  FTM_TX_MEASURE_VSWR_RETURN_LOSS = 20, /*!< 20 : Response - VSWR return loss Report */

  FTM_TX_MEASURE_VSWR_S11_MAG = 21, /*!< 21 : Response - VSWR S11 magnitude Report */

  FTM_TX_MEASURE_VSWR_S11_PHASE = 22, /*!< 22 : Response - VSWR S11 phase Report */

  FTM_TX_MEASURE_VSWR_FWD_POWER = 23, /*!< 23 : Response - VSWR forward power Report */

  FTM_TX_MEASURE_VSWR_REV_POWER = 24, /*!< 24 : Response - VSWR reverse power Report */

  FTM_TX_MEASURE_SPECTRUM_FLATNESS_R1_RIPPLE = 25, /*!< 25 : Response - Spectrum Flatness R1 Ripple Report */

  FTM_TX_MEASURE_SPECTRUM_FLATNESS_R2_RIPPLE = 26, /*!< 26 : Response - Spectrum Flatness R2 Ripple Report */

  FTM_TX_MEASURE_SPECTRUM_FLATNESS_R12_RIPPLE = 27, /*!< 27 : Response - Spectrum Flatness R12 Ripple Report */

  FTM_TX_MEASURE_SPECTRUM_FLATNESS_R21_RIPPLE = 28, /*!< 28 : Response - Spectrum Flatness R21 Ripple Report */

  FTM_TX_MEASURE_SPECTRUM_FLATNESS_R1_RIPPLE_EXTREME = 29, /*!< 29 : Response - Spectrum Flatness R1 Ripple Extreme Report */

  FTM_TX_MEASURE_SPECTRUM_FLATNESS_R2_RIPPLE_EXTREME = 30, /*!< 30 : Response - Spectrum Flatness R2 Ripple Extreme Report */

  FTM_TX_MEASURE_SPECTRUM_FLATNESS_R12_RIPPLE_EXTREME = 31, /*!< 31 : Response - Spectrum Flatness R12 Ripple Extreme Report */

  FTM_TX_MEASURE_SPECTRUM_FLATNESS_R21_RIPPLE_EXTREME = 32, /*!< 32 : Response - Spectrum Flatness R21 Ripple Extreme Report */

  FTM_TX_MEASURE_PHASE_ERROR = 33, /*!< 33 : Response - Phase error (GSM) */

  FTM_TX_MEASURE_ACTION_TX_TOTAL = 34, /*!< 34 : Action - Measure total TX power */

  FTM_TX_MEASURE_TOTAL_TX_POWER = 35, /*!< 35 : Response - Total TX power Report */

  FTM_TX_MEASURE_DEBUG_DATA = 36, /*!< 36 : Response - Debug data in blob property format */

  FTM_TX_MEASURE_ACTION_ORFS_MODULATION = 37, /*!< 37 : Action - ORFS Modulation (GSM) */

  FTM_TX_MEASURE_ACTION_ORFS_SWITCHING = 38, /*!< 38 : Action - ORFS Switching (GSM) */

  FTM_TX_MEASURE_ACTION_PVT = 39, /*!< 39 : Action - PVT (GSM) */

  FTM_TX_MEASURE_RESERVED1 = 40, /*!< 40 : Reserved */

  FTM_TX_MEASURE_RESERVED2 = 41, /*!< 41 : Reserved */

  FTM_TX_MEASURE_PROP_TECH = 42, /*!< 42 : Current Tech */

  FTM_TX_MEASURE_PROP_SUBSCRIBER = 43, /*!< 43 : Subscriber */

  FTM_TX_MEASURE_ORFS_SWITCHING_400KHZ_POS = 44, /*!< 44 : Response - ORFS Switching (GSM) for +400KHz */

  FTM_TX_MEASURE_ORFS_SWITCHING_400KHZ_NEG = 45, /*!< 45 : Response - ORFS Switching (GSM) for -400KHz */

  FTM_TX_MEASURE_ORFS_SWITCHING_600KHZ_POS = 46, /*!< 46 : Response - ORFS Switching (GSM) for +600KHz */

  FTM_TX_MEASURE_ORFS_SWITCHING_600KHZ_NEG = 47, /*!< 47 : Response - ORFS Switching (GSM) for -600KHz */

  FTM_TX_MEASURE_ORFS_SWITCHING_1200KHZ_POS = 48, /*!< 48 : Response - ORFS Switching (GSM) for +1200KHz */

  FTM_TX_MEASURE_ORFS_SWITCHING_1200KHZ_NEG = 49, /*!< 49 : Response - ORFS Switching (GSM) for -1200KHz */

  FTM_TX_MEASURE_ORFS_SWITCHING_1800KHZ_POS = 50, /*!< 50 : Response - ORFS Switching (GSM) for +1800KHz */

  FTM_TX_MEASURE_ORFS_SWITCHING_1800KHZ_NEG = 51, /*!< 51 : Response - ORFS Switching (GSM) for -1800KHz */

  FTM_TX_MEASURE_ORFS_MODULATION_100KHZ_POS = 52, /*!< 52 : Response - ORFS Modulation (GSM) for +100KHz */

  FTM_TX_MEASURE_ORFS_MODULATION_100KHZ_NEG = 53, /*!< 53 : Response - ORFS Modulation (GSM) for -100KHz */

  FTM_TX_MEASURE_ORFS_MODULATION_200KHZ_POS = 54, /*!< 54 : Response - ORFS Modulation (GSM) for +200KHz */

  FTM_TX_MEASURE_ORFS_MODULATION_200KHZ_NEG = 55, /*!< 55 : Response - ORFS Modulation (GSM) for -200KHz */

  FTM_TX_MEASURE_ORFS_MODULATION_250KHZ_POS = 56, /*!< 56 : Response - ORFS Modulation (GSM) for +250KHz */

  FTM_TX_MEASURE_ORFS_MODULATION_250KHZ_NEG = 57, /*!< 57 : Response - ORFS Modulation (GSM) for -250KHz */

  FTM_TX_MEASURE_ORFS_MODULATION_400KHZ_POS = 58, /*!< 58 : Response - ORFS Modulation (GSM) for +400KHz */

  FTM_TX_MEASURE_ORFS_MODULATION_400KHZ_NEG = 59, /*!< 59 : Response - ORFS Modulation (GSM) for -400KHz */

  FTM_TX_MEASURE_ORFS_MODULATION_600KHZ_POS = 60, /*!< 60 : Response - ORFS Modulation (GSM) for +600KHz */

  FTM_TX_MEASURE_ORFS_MODULATION_600KHZ_NEG = 61, /*!< 61 : Response - ORFS Modulation (GSM) for -600KHz */

  FTM_TX_MEASURE_ORFS_MODULATION_1800KHZ_POS = 62, /*!< 62 : Response - ORFS Modulation (GSM) for +1800KHz */

  FTM_TX_MEASURE_ORFS_MODULATION_1800KHZ_NEG = 63, /*!< 63 : Response - ORFS Modulation (GSM) for -1800KHz */

  FTM_TX_MEASURE_PVT_UPPER_MARGIN_0 = 64,  /*!< 64 : Response - PVT (GSM) upper margin for segment 0 */

  FTM_TX_MEASURE_PVT_UPPER_MARGIN_1 = 65,  /*!< 65 : Response - PVT (GSM) upper margin for segment 1 */

  FTM_TX_MEASURE_PVT_UPPER_MARGIN_2 = 66,  /*!< 66 : Response - PVT (GSM) upper margin for segment 2 */

  FTM_TX_MEASURE_PVT_UPPER_MARGIN_3 = 67,  /*!< 67 : Response - PVT (GSM) upper margin for segment 3 */

  FTM_TX_MEASURE_PVT_UPPER_MARGIN_4 = 68,  /*!< 68 : Response - PVT (GSM) upper margin for segment 4 */

  FTM_TX_MEASURE_PVT_UPPER_MARGIN_5 = 69,  /*!< 69 : Response - PVT (GSM) upper margin for segment 5 */

  FTM_TX_MEASURE_PVT_UPPER_MARGIN_6 = 70,  /*!< 70 : Response - PVT (GSM) upper margin for segment 6 */

  FTM_TX_MEASURE_PVT_UPPER_MARGIN_7 = 71,  /*!< 71 : Response - PVT (GSM) upper margin for segment 7 */

  FTM_TX_MEASURE_PVT_UPPER_MARGIN_8 = 72,  /*!< 72 : Response - PVT (GSM) upper margin for segment 8 */

  FTM_TX_MEASURE_PVT_UPPER_MARGIN_9 = 73,  /*!< 73 : Response - PVT (GSM) upper margin for segment 9 */

  FTM_TX_MEASURE_PVT_UPPER_MARGIN_10 = 74,  /*!< 74 : Response - PVT (GSM) upper margin for segment 10 */

  FTM_TX_MEASURE_PVT_LOWER_MARGIN_0 = 75,  /*!< 75 : Response - PVT (GSM) lower margin for segment 0 */

  FTM_TX_MEASURE_PVT_LOWER_MARGIN_1 = 76,  /*!< 76 : Response - PVT (GSM) lower margin for segment 1 */

  FTM_TX_MEASURE_PVT_LOWER_MARGIN_2 = 77,  /*!< 77 : Response - PVT (GSM) lower margin for segment 2 */

  FTM_TX_MEASURE_PVT_LOWER_MARGIN_3 = 78,  /*!< 78 : Response - PVT (GSM) lower margin for segment 3 */

  FTM_TX_MEASURE_PVT_LOWER_MARGIN_4 = 79,  /*!< 79 : Response - PVT (GSM) lower margin for segment 4 */

  FTM_TX_MEASURE_ORFS_SWITCHING_400KHZ_POS_DBM = 80, /*!< 80 : Response - ORFS Switching dBm (GSM) for +400KHz */

  FTM_TX_MEASURE_ORFS_SWITCHING_400KHZ_NEG_DBM = 81, /*!< 81 : Response - ORFS Switching dBm (GSM) for -400KHz */

  FTM_TX_MEASURE_ORFS_SWITCHING_600KHZ_POS_DBM = 82, /*!< 82 : Response - ORFS Switching dBm (GSM) for +600KHz */

  FTM_TX_MEASURE_ORFS_SWITCHING_600KHZ_NEG_DBM = 83, /*!< 83 : Response - ORFS Switching dBm (GSM) for -600KHz */

  FTM_TX_MEASURE_ORFS_SWITCHING_1200KHZ_POS_DBM = 84, /*!< 84 : Response - ORFS Switching dBm (GSM) for +1200KHz */

  FTM_TX_MEASURE_ORFS_SWITCHING_1200KHZ_NEG_DBM = 85, /*!< 85 : Response - ORFS Switching dBm (GSM) for -1200KHz */

  FTM_TX_MEASURE_ORFS_SWITCHING_1800KHZ_POS_DBM = 86, /*!< 86 : Response - ORFS Switching dBm (GSM) for +1800KHz */

  FTM_TX_MEASURE_ORFS_SWITCHING_1800KHZ_NEG_DBM = 87, /*!< 87 : Response - ORFS Switching dBm (GSM) for -1800KHz */

  FTM_TX_MEASURE_ORFS_MODULATION_100KHZ_POS_DBM = 88, /*!< 88 : Response - ORFS Modulation dBm (GSM) for +100KHz */

  FTM_TX_MEASURE_ORFS_MODULATION_100KHZ_NEG_DBM = 89, /*!< 89 : Response - ORFS Modulation dBm (GSM) for -100KHz */

  FTM_TX_MEASURE_ORFS_MODULATION_200KHZ_POS_DBM = 90, /*!< 90 : Response - ORFS Modulation dBm (GSM) for +200KHz */

  FTM_TX_MEASURE_ORFS_MODULATION_200KHZ_NEG_DBM = 91, /*!< 91 : Response - ORFS Modulation dBm (GSM) for -200KHz */

  FTM_TX_MEASURE_ORFS_MODULATION_250KHZ_POS_DBM = 92, /*!< 92 : Response - ORFS Modulation dBm (GSM) for +250KHz */

  FTM_TX_MEASURE_ORFS_MODULATION_250KHZ_NEG_DBM = 93, /*!< 93 : Response - ORFS Modulation dBm (GSM) for -250KHz */

  FTM_TX_MEASURE_ORFS_MODULATION_400KHZ_POS_DBM = 94, /*!< 94 : Response - ORFS Modulation dBm (GSM) for +400KHz */

  FTM_TX_MEASURE_ORFS_MODULATION_400KHZ_NEG_DBM = 95, /*!< 95 : Response - ORFS Modulation dBm (GSM) for -400KHz */

  FTM_TX_MEASURE_ORFS_MODULATION_600KHZ_POS_DBM = 96, /*!< 96 : Response - ORFS Modulation dBm (GSM) for +600KHz */

  FTM_TX_MEASURE_ORFS_MODULATION_600KHZ_NEG_DBM = 97, /*!< 97 : Response - ORFS Modulation dBm (GSM) for -600KHz */

  FTM_TX_MEASURE_ORFS_MODULATION_1800KHZ_POS_DBM = 98, /*!< 98 : Response - ORFS Modulation dBm (GSM) for +1800KHz */

  FTM_TX_MEASURE_ORFS_MODULATION_1800KHZ_NEG_DBM = 99, /*!< 99 : Response - ORFS Modulation dBm (GSM) for -1800KHz */

  FTM_TX_MEASURE_PVT_PASS_FAIL_FLAG = 100, /*!< 100 : Response - PVT pass or fail flag (GSM) */

  FTM_TX_MEASURE_PVT_PASS_RATIO = 101, /*!< 101 : Response - PVT pass ratio flag (GSM) */

  FTM_TX_MEASURE_PVT_PASS_NUM = 102, /*!< 102 : Response - number of slots PVT passed (GSM) */

  FTM_TX_MEASURE_PVT_REQUEST_NUM = 103, /*!< 103 : Response - number of PVT requested (GSM) */

  FTM_TX_MEASURE_PHASE_ERROR_PEAK = 104, /*!< 104 : Response - Phase error peak (GSM) */

  FTM_TX_MEASURE_ACTION_OOS = 105, /*!< 105 : Action - OOS Measurement (GSM) */

  FTM_TX_MEASURE_OOS = 106, /*!< 106 : Response - OOS (GSM) */

  FTM_TX_MEASURE_ACTION_SWPT_CHAR = 107, /*!<107 : Action - Used for FBRX switchpoint characterization */

  FTM_TX_MEASURE_ORFS_MODULATION_800KHZ_POS = 108, /*!< 108: Response - ORFS Modulation (GSM) for +800KHz */

  FTM_TX_MEASURE_ORFS_MODULATION_800KHZ_NEG = 109, /*!< 109: Response - ORFS Modulation (GSM) for -800KHz */

  FTM_TX_MEASURE_ORFS_MODULATION_1000KHZ_POS = 110, /*!< 110: Response - ORFS Modulation (GSM) for +1000KHz */

  FTM_TX_MEASURE_ORFS_MODULATION_1000KHZ_NEG = 111, /*!< 111: Response - ORFS Modulation (GSM) for -1000KHz */

  FTM_TX_MEASURE_ORFS_MODULATION_1200KHZ_POS = 112, /*!< 112: Response - ORFS Modulation (GSM) for +1200KHz */

  FTM_TX_MEASURE_ORFS_MODULATION_1200KHZ_NEG = 113, /*!< 113: Response - ORFS Modulation (GSM) for -1200KHz */

  FTM_TX_MEASURE_ORFS_MODULATION_1400KHZ_POS = 114, /*!< 114: Response - ORFS Modulation (GSM) for +1400KHz */

  FTM_TX_MEASURE_ORFS_MODULATION_1400KHZ_NEG = 115, /*!< 115: Response - ORFS Modulation (GSM) for -1400KHz */

  FTM_TX_MEASURE_ORFS_MODULATION_1600KHZ_POS = 116, /*!< 116: Response - ORFS Modulation (GSM) for +1600KHz */

  FTM_TX_MEASURE_ORFS_MODULATION_1600KHZ_NEG = 117, /*!< 117: Response - ORFS Modulation (GSM) for -1600KHz */

  FTM_TX_MEASURE_ORFS_MODULATION_800KHZ_POS_DBM = 118, /*!< 118: Response - ORFS Modulation dBm (GSM) for +800KHz */

  FTM_TX_MEASURE_ORFS_MODULATION_800KHZ_NEG_DBM = 119, /*!< 119: Response - ORFS Modulation dBm (GSM) for -800KHz */

  FTM_TX_MEASURE_ORFS_MODULATION_1000KHZ_POS_DBM = 120, /*!< 120: Response - ORFS Modulation dBm (GSM) for +1000KHz */

  FTM_TX_MEASURE_ORFS_MODULATION_1000KHZ_NEG_DBM = 121, /*!< 121: Response - ORFS Modulation dBm (GSM) for -1000KHz */

  FTM_TX_MEASURE_ORFS_MODULATION_1200KHZ_POS_DBM = 122, /*!< 122: Response - ORFS Modulation dBm (GSM) for +1200KHz */

  FTM_TX_MEASURE_ORFS_MODULATION_1200KHZ_NEG_DBM = 123, /*!< 123: Response - ORFS Modulation dBm (GSM) for -1200KHz */

  FTM_TX_MEASURE_ORFS_MODULATION_1400KHZ_POS_DBM = 124, /*!< 124: Response - ORFS Modulation dBm (GSM) for +1400KHz */

  FTM_TX_MEASURE_ORFS_MODULATION_1400KHZ_NEG_DBM = 125, /*!< 125: Response - ORFS Modulation dBm (GSM) for -1400KHz */

  FTM_TX_MEASURE_ORFS_MODULATION_1600KHZ_POS_DBM = 126, /*!< 126: Response - ORFS Modulation dBm (GSM) for +1600KHz */

  FTM_TX_MEASURE_ORFS_MODULATION_1600KHZ_NEG_DBM = 127, /*!< 127: Response - ORFS Modulation dBm (GSM) for -1600KHz */

  FTM_TX_MEASURE_ACTION_EXECUTION_DURATION = 128, /*!< 128: Action - Request execution duration */

  FTM_TX_MEASURE_EXECUTION_DURATION = 129, /*!< 129: Response - Report execution duration, in ms */

  FTM_TX_MEASURE_VSWR_FWD_LSE = 130, /*!< 130: Response - VSWR LSE */

  FTM_TX_MEASURE_REPORT_NOISE_FLOOR = 131, /*!< 131: Action - Report Noise Floor */

  FTM_TX_MEASURE_NOISE_FLOOR_ACLR_UTRA1_POS = 132, /*!< 132 : Response - Noise Floor ACLR UTRA1 POS Report */

  FTM_TX_MEASURE_NOISE_FLOOR_ACLR_UTRA1_NEG = 133, /*!< 133 : Response - Noise Floor ACLR UTRA1 NEG Report */

  FTM_TX_MEASURE_NOISE_FLOOR_ACLR_UTRA2_POS = 134, /*!< 134 : Response - Noise Floor ACLR UTRA2 POS Report */

  FTM_TX_MEASURE_NOISE_FLOOR_ACLR_UTRA2_NEG = 135, /*!< 135 : Response - Noise Floor ACLR UTRA2 NEG Report */

  FTM_TX_MEASURE_NOISE_FLOOR_ACLR_EUTRA_POS = 136, /*!< 136 : Response - Noise Floor ACLR EUTRA POS Report */

  FTM_TX_MEASURE_NOISE_FLOOR_ACLR_EUTRA_NEG = 137, /*!< 137 : Response - Noise Floor ACLR EUTRA NEG Report */

  FTM_TX_MEASURE_NOISE_FLOOR_ORFS_SWITCHING_400KHZ_POS = 138, /*!< 138 : Response - Noise Floor ORFS Switching (GSM) for +400KHz */

  FTM_TX_MEASURE_NOISE_FLOOR_ORFS_SWITCHING_400KHZ_NEG = 139, /*!< 139 : Response - Noise Floor ORFS Switching (GSM) for -400KHz */

  FTM_TX_MEASURE_NOISE_FLOOR_ORFS_SWITCHING_600KHZ_POS = 140, /*!< 140 : Response - Noise Floor ORFS Switching (GSM) for +600KHz */

  FTM_TX_MEASURE_NOISE_FLOOR_ORFS_SWITCHING_600KHZ_NEG = 141, /*!< 141 : Response - Noise Floor ORFS Switching (GSM) for -600KHz */

  FTM_TX_MEASURE_NOISE_FLOOR_ORFS_SWITCHING_1200KHZ_POS = 142, /*!< 142 : Response - Noise Floor ORFS Switching (GSM) for +1200KHz */

  FTM_TX_MEASURE_NOISE_FLOOR_ORFS_SWITCHING_1200KHZ_NEG = 143, /*!< 143 : Response - Noise Floor ORFS Switching (GSM) for -1200KHz */

  FTM_TX_MEASURE_NOISE_FLOOR_ORFS_SWITCHING_1800KHZ_POS = 144, /*!< 144 : Response - Noise Floor ORFS Switching (GSM) for +1800KHz */

  FTM_TX_MEASURE_NOISE_FLOOR_ORFS_SWITCHING_1800KHZ_NEG = 145, /*!< 145 : Response - Noise Floor ORFS Switching (GSM) for -1800KHz */

  FTM_TX_MEASURE_NOISE_FLOOR_ORFS_MODULATION_100KHZ_POS = 146, /*!< 146 : Response - Noise Floor ORFS Modulation (GSM) for +100KHz */

  FTM_TX_MEASURE_NOISE_FLOOR_ORFS_MODULATION_100KHZ_NEG = 147, /*!< 147 : Response - Noise Floor ORFS Modulation (GSM) for -100KHz */

  FTM_TX_MEASURE_NOISE_FLOOR_ORFS_MODULATION_200KHZ_POS = 148, /*!< 148 : Response - Noise Floor ORFS Modulation (GSM) for +200KHz */

  FTM_TX_MEASURE_NOISE_FLOOR_ORFS_MODULATION_200KHZ_NEG = 149, /*!< 149 : Response - Noise Floor ORFS Modulation (GSM) for -200KHz */

  FTM_TX_MEASURE_NOISE_FLOOR_ORFS_MODULATION_250KHZ_POS = 150, /*!< 150 : Response - Noise Floor ORFS Modulation (GSM) for +250KHz */

  FTM_TX_MEASURE_NOISE_FLOOR_ORFS_MODULATION_250KHZ_NEG = 151, /*!< 151 : Response - Noise Floor ORFS Modulation (GSM) for -250KHz */

  FTM_TX_MEASURE_NOISE_FLOOR_ORFS_MODULATION_400KHZ_POS = 152, /*!< 152 : Response - Noise Floor ORFS Modulation (GSM) for +400KHz */

  FTM_TX_MEASURE_NOISE_FLOOR_ORFS_MODULATION_400KHZ_NEG = 153, /*!< 153 : Response - Noise Floor ORFS Modulation (GSM) for -400KHz */

  FTM_TX_MEASURE_NOISE_FLOOR_ORFS_MODULATION_600KHZ_POS = 154, /*!< 154 : Response - Noise Floor ORFS Modulation (GSM) for +600KHz */

  FTM_TX_MEASURE_NOISE_FLOOR_ORFS_MODULATION_600KHZ_NEG = 155, /*!< 155 : Response - Noise Floor ORFS Modulation (GSM) for -600KHz */

  FTM_TX_MEASURE_NOISE_FLOOR_ORFS_MODULATION_800KHZ_POS = 156, /*!< 156: Response - Noise Floor ORFS Modulation (GSM) for +800KHz */

  FTM_TX_MEASURE_NOISE_FLOOR_ORFS_MODULATION_800KHZ_NEG = 157, /*!< 157: Response - Noise Floor ORFS Modulation (GSM) for -800KHz */

  FTM_TX_MEASURE_NOISE_FLOOR_ORFS_MODULATION_1000KHZ_POS = 158, /*!< 158: Response - Noise Floor ORFS Modulation (GSM) for +1000KHz */

  FTM_TX_MEASURE_NOISE_FLOOR_ORFS_MODULATION_1000KHZ_NEG = 159, /*!< 159: Response - Noise Floor ORFS Modulation (GSM) for -1000KHz */

  FTM_TX_MEASURE_NOISE_FLOOR_ORFS_MODULATION_1200KHZ_POS = 160, /*!< 160: Response - Noise Floor ORFS Modulation (GSM) for +1200KHz */

  FTM_TX_MEASURE_NOISE_FLOOR_ORFS_MODULATION_1200KHZ_NEG = 161, /*!< 161: Response - Noise Floor ORFS Modulation (GSM) for -1200KHz */

  FTM_TX_MEASURE_NOISE_FLOOR_ORFS_MODULATION_1400KHZ_POS = 162, /*!< 162: Response - Noise Floor ORFS Modulation (GSM) for +1400KHz */

  FTM_TX_MEASURE_NOISE_FLOOR_ORFS_MODULATION_1400KHZ_NEG = 163, /*!< 163: Response - Noise Floor ORFS Modulation (GSM) for -1400KHz */

  FTM_TX_MEASURE_NOISE_FLOOR_ORFS_MODULATION_1600KHZ_POS = 164, /*!< 164: Response - Noise Floor ORFS Modulation (GSM) for +1600KHz */

  FTM_TX_MEASURE_NOISE_FLOOR_ORFS_MODULATION_1600KHZ_NEG = 165, /*!< 165: Response - Noise Floor ORFS Modulation (GSM) for -1600KHz */

  FTM_TX_MEASURE_NOISE_FLOOR_ORFS_MODULATION_1800KHZ_POS = 166, /*!< 166 : Response - Noise Floor ORFS Modulation (GSM) for +1800KHz */

  FTM_TX_MEASURE_NOISE_FLOOR_ORFS_MODULATION_1800KHZ_NEG = 167, /*!< 167 : Response - Noise Floor ORFS Modulation (GSM) for -1800KHz */

  FTM_TX_MEASURE_DELTA_LIMIT_NOISE_FLOOR_ACLR_UTRA1_POS = 168, /*!< 168 : Response - Delta Upper Limit Noise Floor ACLR UTRA1 Report */

  FTM_TX_MEASURE_DELTA_LIMIT_NOISE_FLOOR_ACLR_UTRA1_NEG = 169, /*!< 169 : Response - Delta Lower Limit Noise Floor ACLR UTRA1 Report */

  FTM_TX_MEASURE_DELTA_LIMIT_NOISE_FLOOR_ACLR_UTRA2_POS = 170, /*!< 170 : Response - Delta Upper Limit Noise Floor ACLR UTRA2 Report */

  FTM_TX_MEASURE_DELTA_LIMIT_NOISE_FLOOR_ACLR_UTRA2_NEG = 171, /*!< 171 : Response - Delta Lower Limit Noise Floor ACLR UTRA2 Report */

  FTM_TX_MEASURE_DELTA_LIMIT_NOISE_FLOOR_ACLR_EUTRA_POS = 172, /*!< 172 : Response - Delta Upper Limit Noise Floor ACLR EUTRA Report */

  FTM_TX_MEASURE_DELTA_LIMIT_NOISE_FLOOR_ACLR_EUTRA_NEG = 173, /*!< 173 : Response - Delta Lower Limit Noise Floor ACLR EUTRA Report */

  FTM_TX_MEASURE_DELTA_LIMIT_NOISE_FLOOR_ORFS_MODULATION_POS = 174, /*!< 174 : Response - Delta Upper Limit Noise Floor ORFS Modulation (GSM)*/

  FTM_TX_MEASURE_DELTA_LIMIT_NOISE_FLOOR_ORFS_MODULATION_NEG = 175, /*!< 175 : Response - Delta Lower Limit Noise Floor ORFS Modulation (GSM) */

  FTM_TX_MEASURE_DELTA_LIMIT_NOISE_FLOOR_ORFS_SWITCHING_POS = 176, /*!< 176 : Response - Delta Upper Limit Noise Floor ORFS Switching (GSM)*/

  FTM_TX_MEASURE_DELTA_LIMIT_NOISE_FLOOR_ORFS_SWITCHING_NEG = 177, /*!< 177 : Response - Delta Lower Limit Noise Floor ORFS Switching (GSM) */

  FTM_TX_MEASURE_ACTION_SEM = 178,

  FTM_TX_MEASURE_SEM_PASS_FAIL_FLAG = 179,

  FTM_TX_MEASURE_PROP_SEM_OFFSET_NUM = 180,  /*!< 180 : number of SEM offsets*/

  FTM_TX_MEASURE_PROP_SEM_FREQ_OFFSET = 181,  /*!< 181 : SEM frequency offset*/

  FTM_TX_MEASURE_PROP_SEM_WORST_CASE_POWER = 182,  /*!< 182 : Worst case power*/

  FTM_TX_MEASURE_ACTION_OBW = 183, /*!< 183 : Action: Measure OBW */

  FTM_TX_MEASURE_OBW = 184, /*!< 184 : Measured OBW */

  FTM_TX_MEASURE_NUM_OF_CAPTURES = 185, /*!< 185 : Response - number of IQ captuers */

  FTM_TX_MEASURE_IQ_SAMPLE_DEBUG_INFO_0 = 186, /*!< 186 : Response - I/Q sample Info */

  FTM_TX_MEASURE_IQ_SAMPLE_DEBUG_INFO_1 = 187, /*!< 187 : Response - I/Q sample Info */

  FTM_TX_MEASURE_IQ_SAMPLE_DEBUG_INFO_2 = 188, /*!< 188 : Response - I/Q sample Info */

  FTM_TX_MEASURE_DEBUG_ERROR_CODE = 189, /*!<189: Response - Detailed Debug Error code */

  FTM_TX_MEASURE_PROP_SUB_TECH = 190,

  FTM_TX_MEASURE_PROP_MC_BID = 191, /*!<191: Response - MC bid */

  FTM_TX_MEASURE_PROP_MC_GS = 192, /*!<192: Response - MC lna gs */

  FTM_TX_MEASURE_PVT_UPPER_ABS_0 = 193,  /*!< 193 : Response - PVT (GSM) upper absolute for segment 0 */

  FTM_TX_MEASURE_PVT_UPPER_ABS_1 = 194,  /*!< 194 : Response - PVT (GSM) upper absolute for segment 1 */

  FTM_TX_MEASURE_PVT_UPPER_ABS_2 = 195,  /*!< 195 : Response - PVT (GSM) upper absolute for segment 2 */

  FTM_TX_MEASURE_PVT_UPPER_ABS_3 = 196,  /*!< 196 : Response - PVT (GSM) upper absolute for segment 3 */

  FTM_TX_MEASURE_PVT_UPPER_ABS_4 = 197,  /*!< 197 : Response - PVT (GSM) upper absolute for segment 4 */

  FTM_TX_MEASURE_PVT_UPPER_ABS_5 = 198,  /*!< 198 : Response - PVT (GSM) upper absolute for segment 5 */

  FTM_TX_MEASURE_PVT_UPPER_ABS_6 = 199,  /*!< 199 : Response - PVT (GSM) upper absolute for segment 6 */

  FTM_TX_MEASURE_PVT_UPPER_ABS_7 = 200,  /*!< 200 : Response - PVT (GSM) upper absolute for segment 7 */

  FTM_TX_MEASURE_PVT_UPPER_ABS_8 = 201,  /*!< 201 : Response - PVT (GSM) upper absolute for segment 8 */

  FTM_TX_MEASURE_PVT_UPPER_ABS_9 = 202,  /*!< 202 : Response - PVT (GSM) upper absolute for segment 9 */

  FTM_TX_MEASURE_PVT_UPPER_ABS_10 = 203,  /*!< 203 : Response - PVT (GSM) upper absolute for segment 10 */

  FTM_TX_MEASURE_PVT_LOWER_ABS_0 = 204,  /*!< 204 : Response - PVT (GSM) lower absolute for segment 0 */

  FTM_TX_MEASURE_PVT_LOWER_ABS_1 = 205,  /*!< 205 : Response - PVT (GSM) lower absolute for segment 1 */

  FTM_TX_MEASURE_PVT_LOWER_ABS_2 = 206,  /*!< 206 : Response - PVT (GSM) lower absolute for segment 2 */

  FTM_TX_MEASURE_PVT_LOWER_ABS_3 = 207,  /*!< 207 : Response - PVT (GSM) lower absolute for segment 3 */

  FTM_TX_MEASURE_PVT_LOWER_ABS_4 = 208,  /*!< 208 : Response - PVT (GSM) lower absolute for segment 4 */

  FTM_TX_MEASURE_ACTION_CARRIER_LEAKAGE = 209, /*!< 209 : Action - Measure Carrier Leakage */

  FTM_TX_MEASURE_CARRIER_LEAKAGE = 210,  /*!< 210 : Response - Carrier Leakage measurement */

  FTM_TX_MEASURE_RX_GAIN_STATE_OVERRIDE = 211,  /*!< 211 : Action - Override Rx GS during selftest */

  FTM_TX_MEASURE_ADDITIONAL_ERROR_CODE_SET_1 = 212,  /*!< 212 : Response - Additional Error code set 1 */

  FTM_TX_MEASURE_GNB_SUB6_ACLR_LEFT = 213,  /*!< 213 : Response - ACLR Left for GNB SUB6 */

  FTM_TX_MEASURE_GNB_SUB6_ACLR_RIGHT = 214,  /*!< 214 : Response - ACLR Right for GNB SUB6 */

  FTM_TX_MEASURE_GNB_SUB6_ACLR_MET = 215,  /*!< 215 : Response - ACLR met for GNB SUB6 */

  FTM_TX_MEASURE_CARRIER_LEAKAGE_TONE_PWR = 216,  /*!< 216 : Response - Tone power during Carrier Leakage measurement */

  FTM_TX_MEASURE_ACTION_AUTO_PIN_STATUS = 217,  /*!< 217 : Action - Auto pin status */

  FTM_TX_MEASURE_AUTO_PIN_STATUS = 218,  /*!< 218 : Response - Auto pin status */
  
  FTM_TX_MEASURE_ACTION_AUTO_DELAY_STATUS = 219,  /*!< 219 : Action - Auto delay status */

  FTM_TX_MEASURE_AUTO_DELAY_STATUS = 220,  /*!< 220 : Response - Auto delay status */

  FTM_TX_MEASURE_FBRX_GAIN_STATE = 221,  /*!< 221 : Response - FBRx Gain State */

  FTM_TX_MEASURE_PROP_NUM  /*!< Max : Defines maximum number of properties */

} ftm_rf_test_tx_measure_property_type;

extern const char *tx_measure_property_names[];

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Enumeration to indicate the type of properties for FTM IRAT CONFIG Command.
  These enumeration are used to define the content of #ftm_properties_framework_field_property_type in FTM test command packet.
irat_config_property_names Must be updated when this list is updated.*/
typedef enum
{
  FTM_IRAT_CONFIG_PROP_UNASSIGNED = 0,    /*!< 0 : Unassigned property */

  FTM_IRAT_CONFIG_PROP_TGT_ACTION = 1,    /*!< 1 : iRAT Action - Setup/Tune Away/Tune Back */

  FTM_IRAT_CONFIG_PROP_TGT_SUB = 2,       /*!< 2 : Target Subscription */

  FTM_IRAT_CONFIG_PROP_TGT_TECH = 3,     /*!< 3 : Target Tech */

  FTM_IRAT_CONFIG_PROP_TGT_CARRIER = 4,   /*!< 4 : Target Carrier */

  FTM_IRAT_CONFIG_PROP_TGT_DEVICE = 5,    /*!< 5 : Target Device */

  FTM_IRAT_CONFIG_PROP_TGT_SIG_PATH = 6,  /*!< 6 : Target Sig Path */

  FTM_IRAT_CONFIG_PROP_TGT_ANT_PATH = 7,  /*!< 7 : Target Ant Path */

  FTM_IRAT_CONFIG_PROP_TGT_BAND = 8,      /*!< 8 : Target Band */

  FTM_IRAT_CONFIG_PROP_TGT_CHANNEL = 9,   /*!< 9 : Target Channel */

  FTM_IRAT_CONFIG_PROP_TGT_BANDWIDTH = 10, /*!< 10 : Target Band Width */

  FTM_IRAT_CONFIG_PROP_TGT_PLL_ID = 11,   /*!< 11 : Target PLL ID */

  FTM_IRAT_CONFIG_PROP_TGT_BEAM_CNT = 12,  /*!< 12 : Target Beam Count */

  FTM_IRAT_CONFIG_PROP_TGT_BEAM_ID = 13,   /*!< 13 : Target Beam ID Array */

  FTM_IRAT_CONFIG_PROP_TGT_BEAM_CYCLE = 14,/*!< 14 : Target Beam Cycle Mode */

  /* HI2.0 QDTA UI */
  FTM_IRAT_CONFIG_PROP_SRC_SUB = 15,       /*!< 15 : Source Subscription */

  FTM_IRAT_CONFIG_PROP_SRC_TECH = 16,      /*!< 16 : Source Technology */

  FTM_IRAT_CONFIG_PROP_SRC_CARRIER = 17,   /*!< 17 : Source Carrier (Trigger Sequence) */

  FTM_IRAT_CONFIG_PROP_SRC_DEVICE = 18,    /*!< 18 : Source RfPath.Device  */

  FTM_IRAT_CONFIG_PROP_SRC_SIG_PATH = 19,  /*!< 19 : Source RfPath.SigPath */

  FTM_IRAT_CONFIG_PROP_SRC_ANT_PATH = 20,  /*!< 20 : Source RfPath.AntPath */

  FTM_IRAT_CONFIG_PROP_SRC_PLL_ID = 21,    /*!< 21 : Source RfPath.Pll     */

  FTM_IRAT_CONFIG_PROP_TGT_NB_ID = 22,    /*!< 22 : Target RfPath.NB     */

  FTM_IRAT_CONFIG_PROP_TGT_LANE_ID = 23,    /*!< 23 : Target RfPath.LANE     */

  FTM_IRAT_CONFIG_PROP_SRC_NB_ID = 24,    /*!< 24 : Source RfPath.NB     */

  FTM_IRAT_CONFIG_PROP_SRC_LANE_ID = 25,    /*!< 25 : Source RfPath.LANE     */

  FTM_IRAT_CONFIG_PROP_EXPECTED_RXAGC_POWER = 26,    /*!< 26 : Expected Rx AGC Power     */

  FTM_IRAT_CONFIG_PROP_TGT_PRX_ENERGY_ESTIMATE = 27,    /*!< 27 : Target Prx Energy Estimate     */

  FTM_IRAT_CONFIG_PROP_TGT_DRX_ENERGY_ESTIMATE = 28,    /*!< 28 : Target Drx Energy Estimate     */  
   
  FTM_IRAT_CONFIG_PROP_BUILD_SCRIPT_TIME = 29,          /*!< 29 : BuildScript Time     */

  FTM_IRAT_CONFIG_PROP_NUM

} ftm_rf_test_irat_config_property_type;

extern const char *irat_config_property_names[];

/*====================================================================================================================*/
/*!
  @name Wait Trigger Property IDs

  @brief
  Wait Trigger Property ID list
*/
/*! @{ */

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Enumeration to indicate the type of properties for Wait Trigger Subcommand.
  These enumeration are used to define the content of #ftm_properties_framework_field_property_type in FTM test command packet
  wait_trigger_property_names Must be updated when this list is updated.*/
typedef enum
{
  FTM_WAIT_TRIGGER_PROP_UNASSIGNED = 0, /*!< 0 : Unassigned property */

  FTM_WAIT_TRIGGER_PROP_SUBSCRIBER = 1, /*!< 1 : Subscriber Number */

  FTM_WAIT_TRIGGER_PROP_TECHNOLOGY = 2, /*!< 2 : Technology */

  FTM_WAIT_TRIGGER_PROP_TX_CARRIER = 3, /*!< 3 : Transmit Carrier Number */

  FTM_WAIT_TRIGGER_PROP_RX_CARRIER = 4, /*!< 4 : Receiver Carrier Number */

  FTM_WAIT_TRIGGER_PROP_RFM_DEVICE = 5, /*!< 5 : RFM device for this measurement*/

  FTM_WAIT_TRIGGER_PROP_RXWAITDBM_THRESHOLD  = 6, /*!< 6 : Rx Wait dBm - Threshold (dBm10) */

  FTM_WAIT_TRIGGER_PROP_RXWAITDBM_DIRECTION  = 7, /*!< 7 : Rx Wait dBm - Direction (positive/negative) */

  FTM_WAIT_TRIGGER_PROP_RXWAITDBM_TIMEOUT    = 8, /*!< 8 : Rx Wait dBm - Timeout (us) */

  FTM_WAIT_TRIGGER_PROP_RXWAITDBM_RXAGC      = 9, /*!< 8 : Rx Wait dBm - Measured RxAGC (dBm10) */

  FTM_WAIT_TRIGGER_PROP_NUM,  /*!< Max : Defines maximum number of properties */

} ftm_rf_test_wait_trigger_property_type;

extern const char *wait_trigger_property_names[];

typedef enum
{
  FTM_RF_TEST_CMD_CAP_PROP_UNASSIGNED = 0, /*!< 0 : unassigned command */

  FTM_RF_TEST_CMD_CAP_PROP_QUERY_CMD = 1, /*!< 1 : Query for command */

  FTM_RF_TEST_CMD_CAP_PROP_QUERY_PROP = 2, /*!< 2 : Query for a property */

  FTM_RF_TEST_CMD_CAP_PROP_RSP_CMD_QUERY = 3, /*!< 3 : Response property for cmd query */

  FTM_RF_TEST_CMD_CAP_PROP_RSP_PROP_QUERY_0_63 = 4, /*!< 4 : Response bit mask[0-63] for property query */

  FTM_RF_TEST_CMD_CAP_PROP_RSP_PROP_QUERY_64_127 = 5, /*!< 5 : Response bit mask[64-127] for property query */

  FTM_RF_TEST_CMD_CAP_PROP_RSP_PROP_QUERY_128_191 = 6, /*!< 6 : Response bit mask[128-191] for property query */

  FTM_RF_TEST_CMD_CAP_PROP_RSP_PROP_QUERY_192_255 = 7, /*!< 7 : Response bit mask[192-255] for property query */

  FTM_RF_TEST_CMD_CAP_PROP_NUM  /*!< Max : Defines maximum number of properties */

} ftm_rf_test_command_capability_property_type;


/*! @} */

extern const char *command_capability_property_names[];


/*! @} */

/*====================================================================================================================*/
/*!
  @name Sleep Control Property ID

  @brief
  Sleep Control Property ID list
*/
/*! @{ */

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Enumeration to indicate the type of properties for FTM Sleep Control command. These enumeration are used to define
the content of #ftm_properties_framework_field_property_type in FTM test command packet.
sleep_control_property_names Must be updated when this list is updated.*/
typedef enum
{
  FTM_RF_TEST_SLEEP_CONTROL_PROP_UNASSIGNED = 0, /*!< 0 : Unassigned property */

  FTM_RF_TEST_SLEEP_CONTROL_PROP_SUBSCRIBER = 1, /*!< 1 : Subscriber */

  FTM_RF_TEST_SLEEP_CONTROL_PROP_TECHNOLOGY = 2, /*!< 2 : Technology */

  FTM_RF_TEST_SLEEP_CONTROL_PROP_SLEEP_WAKEUP_INDICATOR = 3, /*!< 3 : Sleep/Wakeup Indicator */

  FTM_RF_TEST_SLEEP_CONTROL_PROP_SLEEP_WAKEUP_MODE = 4,  /*!< 4 : Sleep/Wakeup Mode */

  FTM_RF_TEST_SLEEP_CONTROL_PROP_CARRIER_INDEX = 5, /*!< 5 : Carrier Index */

  FTM_RF_TEST_SLEEP_CONTROL_PROP_CHAIN = 6, /*!< 6 : Rx/Tx Chain */

  FTM_RF_TEST_SLEEP_CONTROL_PROP_RX_TX_INDICATOR = 7, /*!< 7 : Rx/Tx Indicator */

  FTM_RF_TEST_SLEEP_CONTROL_PROP_TIME_US = 8,    /*!< 8 : Time in us for the command to execute */
  
  FTM_RF_TEST_SLEEP_CONTROL_PROP_TECH_SLEEP_WAKEUP_TOTAL_TIME = 9,    /*!< 9 : Tech sleep/wakeup total time */

  FTM_RF_TEST_SLEEP_CONTROL_PROP_NUM  /*!< Max : Defines maximum number of properties */

} ftm_rf_test_sleep_control_property_type;

extern const char *sleep_control_property_names[];


/*--------------------------------------------------------------------------------------------------------------------*/
/*! Enumeration to indicate the type of properties for RF Verify Debug command. These enumeration are used to define
the content of #ftm_properties_framework_field_property_type in FTM test command packet.
rf_verify_debug_config_property_names Must be updated when this list is updated.*/
typedef enum
{
  FTM_RF_TEST_VERIFY_DEBUG_CONFIG_PROP_UNASSIGNED = 0, /*!< 0 : Unassigned property */

  FTM_RF_TEST_VERIFY_DEBUG_CONFIG_PROP_DEBUG_ACTION_TYPE = 1, /*!< 1 : Debug Action Type */

  FTM_RF_TEST_VERIFY_DEBUG_CONFIG_PROP_CRASH_AFTER_CAPTURE_DONE = 2,  /*!< 2 : Trigger a crash whenever an IQ capture is done successfully */

  FTM_RF_TEST_VERIFY_DEBUG_CONFIG_PROP_CRASH_IF_CAPTURE_TIMEOUT = 3,  /*!< 3 : Trigger a crash if an IQ capture cannot be performed and time-out*/

  FTM_RF_TEST_VERIFY_DEBUG_CONFIG_PROP_LSE_MAX_LIMIT   = 4, /*!< 4 : Max Limit of LSE */

  FTM_RF_TEST_VERIFY_DEBUG_CONFIG_PROP_EVM_MAX_LIMIT    = 5, /*!< 5 : Max Limit of EVM */

  FTM_RF_TEST_VERIFY_DEBUG_CONFIG_PROP_SF_R1_MAX_LIMIT = 6, /*!< 6 : Max Limit of Spectrum Flatness R1 Max Limit.  */

  FTM_RF_TEST_VERIFY_DEBUG_CONFIG_PROP_SF_R2_MAX_LIMIT = 7,    /*!< 7 : Max Limit of Spectrum Flatness R2 Max Limit. */

  FTM_RF_TEST_VERIFY_DEBUG_CONFIG_PROP_SF_R12_MAX_LIMIT = 8,    /*!< 8 :Max Limit of Spectrum Flatness R12 Max Limit. */

  FTM_RF_TEST_VERIFY_DEBUG_CONFIG_PROP_SF_R21_MAX_LIMIT = 9,    /*!< 9 : Max Limit of Spectrum Flatness R21 Max Limit. */

  FTM_RF_TEST_VERIFY_DEBUG_CONFIG_PROP_ACLR_UTRA1_MIN_LIMIT = 10,    /*!< 10 : Max Limit of ACLR Utra 1 Margin */

  FTM_RF_TEST_VERIFY_DEBUG_CONFIG_PROP_ACLR_UTRA2_MIN_LIMIT = 11,    /*!< 11 : Max Limit of ACLR Utra 2 Margin. */

  FTM_RF_TEST_VERIFY_DEBUG_CONFIG_PROP_ACLR_EUTRA_MIN_LIMIT = 12,    /*!< 12 : Max Limit of ACLR E-Utra Margin */

  FTM_RF_TEST_VERIFY_DEBUG_CONFIG_PROP_SEM_OFFSET1_MAX_LIMIT = 13,    /*!< 13 : Max Limit of SEM Offset 1 */

  FTM_RF_TEST_VERIFY_DEBUG_CONFIG_PROP_SEM_OFFSET2_MAX_LIMIT = 14,    /*!< 14 : Max Limit of SEM Offset 2 */

  FTM_RF_TEST_VERIFY_DEBUG_CONFIG_PROP_SEM_OFFSET3_MAX_LIMIT = 15,    /*!< 15 : Max Limit of SEM Offset 3 */

  FTM_RF_TEST_VERIFY_DEBUG_CONFIG_PROP_SEM_OFFSET4_MAX_LIMIT = 16,    /*!< 16 : Max Limit of SEM Offset 4 */

  FTM_RF_TEST_VERIFY_DEBUG_CONFIG_PROP_TX_POWER_LIMIT_DELTA = 17,    /*!< 17 : Symmetrical Delta for Target Tx Power */

  FTM_RF_TEST_VERIFY_DEBUG_CONFIG_PROP_CARRIER_LEAKAGE_MAX_LIMIT = 18,    /*!< 18 : Max Limit of Carrier Leakage (LOFT).  */

  FTM_RF_TEST_VERIFY_DEBUG_CONFIG_PROP_GSM_PVT = 19,    /*!< 19 : Pass or Fail Flag for GSM PVT test  */

  FTM_RF_TEST_VERIFY_DEBUG_CONFIG_PROP_GSM_PEAK_PHASE_ERROR_MAX_LIMIT = 20,    /*!< 20 : Max Limit of GSM Peak Phase Error Value */

  FTM_RF_TEST_VERIFY_DEBUG_CONFIG_PROP_GSM_RMS_PHASE_ERROR_MAX_LIMIT = 21,    /*!< 21 : Max Limit of GSM RMS Phase Error Value */

  FTM_RF_TEST_VERIFY_DEBUG_CONFIG_PROP_GSM_ORFS_MOD_LIMIT_DELTA = 22,    /*!< 22 : Symmetrical Delta for GSM ORFS Modulation */

  FTM_RF_TEST_VERIFY_DEBUG_CONFIG_PROP_GSM_ORFS_SW_LIMIT_DELTA = 23,    /*!< 23 : Symmetrical Delta for GSM ORFS Switch */

  FTM_RF_TEST_VERIFY_DEBUG_CONFIG_PROP_ONE_TONE_EST_SEN_MAX_LIMIT = 24,    /*!< 24 : Max Limit of 1 Tone Estimated Sensitivity */

  FTM_RF_TEST_VERIFY_DEBUG_CONFIG_PROP_NO_TONE_EST_SEN_MAX_LIMIT = 25,    /*!< 25 : Max Limit of No Tone Estimated  Sensitivity  */

  FTM_RF_TEST_VERIFY_DEBUG_CONFIG_PROP_DLAFC_LOW_LIMIT = 26,    /*!< 26 : Low Limit of DL AFC Frequency Error */

  FTM_RF_TEST_VERIFY_DEBUG_CONFIG_PROP_DLAFC_UPPER_LIMIT = 27,    /*!< 27 : Upper Limit of DL AFC Frequency Error */

  FTM_RF_TEST_VERIFY_DEBUG_CONFIG_PROP_RXAGC_LOW_DELTA = 28,    /*!< 28 : Low Delta of Rx AGC delta between expected Rx AGC and measured Rx AGC */

  FTM_RF_TEST_VERIFY_DEBUG_CONFIG_PROP_RXAGC_UPPER_DELTA = 29,    /*!< 29 : Upper Delta of Rx AGC delta between expected Rx AGC and measured Rx AGC */

  FTM_RF_TEST_VERIFY_DEBUG_CONFIG_PROP_VSWR_LOW_LIMIT = 30,    /*!< 30 : Low limit of VSWR value. */

  FTM_RF_TEST_VERIFY_DEBUG_CONFIG_PROP_VSWR_UPPER_LIMIT = 31,    /*!< 31 : Upper limit of VSWR value. */

  FTM_RF_TEST_VERIFY_DEBUG_CONFIG_PROP_VSWR_LSE_MAX_LIMIT = 32,    /*!< 32 : Max Limit of VSWR LSE */

  FTM_RF_TEST_VERIFY_DEBUG_CONFIG_PROP_FBRX_PLL_MODE = 33,    /*!< 33 : FBRx PLL Mode */

  FTM_RF_TEST_VERIFY_DEBUG_CONFIG_PROP_SAVE_IQ_DATA = 34,    /*!< 34 : Save IQ Data  */

  FTM_RF_TEST_VERIFY_DEBUG_CONFIG_PROP_TIME_US = 35,    /*!< 35 : Time in us for the command to execute */

  FTM_RF_TEST_VERIFY_DEBUG_CONFIG_PROP_NUM  /*!< Max : Defines maximum number of properties */

} ftm_rf_test_verify_debug_config_property_type;

extern const char *rf_verify_debug_config_property_names[];

/*====================================================================================================================*/
/*!
  @name MCS Config Property ID

  @brief
  MCS Config Property ID list
*/
/*! @{ */

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Enumeration to indicate the type of properties for FTM MCS Config command. These enumeration are used to define
the content of #ftm_properties_framework_field_property_type in FTM test command packet.
mcs_config_property_names Must be updated when this list is updated.*/
typedef enum
{
  FTM_RF_TEST_MCS_CONFIG_PROP_UNASSIGNED = 0, /*!< 0 : Unassigned property */

  FTM_RF_TEST_MCS_CONFIG_PROP_COEX_ACTION = 1, /*!< 1 : Coex Action */

  FTM_RF_TEST_MCS_CONFIG_PROP_STX_ACTION = 2, /*!< 2 : sTx Action */

  FTM_RF_TEST_MCS_CONFIG_PROP_COEX_MODE = 3, /*!< 3 : Coex Mode */

  FTM_RF_TEST_MCS_CONFIG_PROP_DESENSE_PRESENT = 4, /*!< 4 : Desense Present */

  FTM_RF_TEST_MCS_CONFIG_PROP_SUBSCRIBER = 5,  /*!< 5 : Subscriber Index */

  FTM_RF_TEST_MCS_CONFIG_PROP_TECHNOLOGY = 6,  /*!< 6 : Technology */

  FTM_RF_TEST_MCS_CONFIG_PROP_CARRIER = 7,  /*!< 7 : Carrier Index */

  FTM_RF_TEST_MCS_CONFIG_PROP_DESENSE_REASON = 8,  /*!< 8 : Desense Reason */

  FTM_RF_TEST_MCS_CONFIG_PROP_DESENSE_ACTION = 9,  /*!< 9 : Desense Action */

  FTM_RF_TEST_MCS_CONFIG_PROP_COEX_MTPL = 10,  /*!< 10 : Coex MTPL */

  FTM_RF_TEST_MCS_CONFIG_PROP_MCC_INFO = 11,  /*!< 11 : MCC Info */

  FTM_RF_TEST_MCS_CONFIG_PROP_BYPASS_DISALLOW_TABLE = 12,  /*!< 12 : Bypass disallow table */

  FTM_RF_TEST_MCS_CONFIG_PROP_NUM  /*!< Max : Defines maximum number of properties */
  
} ftm_rf_test_mcs_config_property_type;

extern const char *mcs_config_property_names[];


/*====================================================================================================================*/
/*!
  @name XPT Config Property ID

  @brief
  XPT Config Property ID list
*/
/*! @{ */

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Enumeration to indicate the type of properties for FTM XPT Config command. These enumeration are used to define
the content of #ftm_properties_framework_field_property_type in FTM test command packet.
xpt_config_property_names Must be updated when this list is updated.*/
typedef enum
{
  FTM_RF_TEST_XPT_CONFIG_PROP_UNASSIGNED = 0, /*!< 0 : Unassigned property */

  FTM_RF_TEST_XPT_CONFIG_PROP_XPT_MODE = 1, /*!< 1 : XPT Mode */

  FTM_RF_TEST_XPT_CONFIG_PROP_AUTO_PIN_OFFSET = 2, /*!< 2 : AutoPin Offset */

  FTM_RF_TEST_XPT_CONFIG_PROP_AUTO_DELAY_OFFSET = 3, /*!< 3 : AutoDelay Offset */

  FTM_RF_TEST_XPT_CONFIG_PROP_ONLINE_DPD_OFFSET = 4,  /*!< 4 : OnlineDPD Offset */
  
  FTM_RF_TEST_XPT_CONFIG_PROP_NUM  /*!< Max : Defines maximum number of properties */
  
} ftm_rf_test_xpt_config_property_type;

extern const char *xpt_config_property_names[];


/*! @} */

#ifdef T_WINNT
#error code not present
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* FTM_RF_TEST_INTERFACE_H */

