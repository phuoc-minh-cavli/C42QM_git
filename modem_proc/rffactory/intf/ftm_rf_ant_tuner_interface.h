#ifndef FTM_RF_ANT_TUNER_INTERFACE_H
#define FTM_RF_ANT_TUNER_INTERFACE_H
/*!
  @file
  ftm_rf_ant_tuner_interface.h

  @brief
  Common framework to perform RF FTM antenna tuner operations
*/

/*======================================================================================================================

  Copyright (c) 2021 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //components/rel/rffactory.mpss/1.0/intf/ftm_rf_ant_tuner_interface.h#9 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------------------------------------------------
03/10/22   pm      Appended 11 in enum ftm_rf_ant_tuner_tune_code_update_enum_type
05/12/21   pm      Updated FTM_RF_MAX_NUM_ANTENNAS to RFM_ANTENNA_MAX
10/31/18   dh      Added new packet version FTM_RF_ANT_TUNER_PKT_VERSION2
08/27/18   sm      Increase FTM_RF_MAX_NUM_ANTENNAS to 12
04/19/18   ay      Added new data packets for FTM request "Update All Antenna Tuner"
01/08/17   dh      Added support for AAT table override
10/05/16   dh     Define a new V0 req pkt for FTM_RF_ANT_TUNER_CMD_SET_TUNER_PROPERITES
09/28/16   dh      Added interfaces for Impedance Scenario, Aperture Scenario and asdiv position
04/25/16   ndb     Initial Release

======================================================================================================================*/

#include "comdef.h"
#include "rfm_antenna_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Macro defining the maximum number of antennas supported*/
#define FTM_RF_MAX_NUM_ANTENNAS RFM_ANTENNA_MAX

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Macro defining the maximum size of tunecodes in bytes*/
#define FTM_RF_MAX_TUNE_CODE_SIZE_IN_BYTES 8

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Type definition to define the size of the sub-command field in FTM Ant Tuner command packet.  */
typedef uint16 ftm_rf_ant_tuner_comamnd_type;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Type definition to define the size of the reserved field in FTM Ant Tuner command packet.  */
typedef uint16 ftm_rf_ant_tuner_field_reserved_type;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Type definition to define the size of the version field in FTM Ant Tuner command packet. */
typedef uint16 ftm_rf_ant_tuner_field_version_type;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Type definition to define the status code mask for response packet. Each bit indicates a type of status. */ 
typedef uint32 ftm_rf_ant_tuner_field_status_code_mask_type;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Structure defining the tuner status associated with antenna. */ 
typedef PACK(struct)
{
  uint8 ant_num;
  /*!< Antenna number */

  uint8 tuner_mgr_type;
  /*!< Tuner Manager Type */

  uint8 impedance_tx_algo_type;
  /*!< Impedance Tuner - Tx Algo Type */

  uint8 tune_code_size;
  /*!< Indicates the size of tune code in bytes */
  
  uint8 tune_code_data[FTM_RF_MAX_TUNE_CODE_SIZE_IN_BYTES];
  /*!< Tune data data*/

  uint8 device_state;
  /*!< Tuner manager device state*/

  uint8 aperature_tx_algo_type;
  /*!< Aperature Tuner - Tx Algo Type */

  uint8 rx_algo_type;
  /*!< Tuner - Rx Algo Type */

  uint8 charge_pump_status;
  /*!< charge pump status of the tuners associated with the antenna */
  
  uint8 tune_code_mapping[FTM_RF_MAX_TUNE_CODE_SIZE_IN_BYTES];
  /*!< Byte level tunecode mapping. 
  Each byte carries the tuner instance ID of the corresponding 
  tunecode byte in TuneCode data*/

}ftm_rf_ant_tuner_tuner_status_data_type;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Structure defining the tuner status associated with antenna : Version 1 */ 
typedef PACK(struct)
{
  uint8 ant_num;
  /*!< Antenna number */

  uint8 tuner_mgr_type;
  /*!< Tuner Manager Type */

  uint8 impedance_tx_algo_type;
  /*!< Impedance Tuner - Tx Algo Type */

  uint8 tune_code_size;
  /*!< Indicates the size of tune code in bytes */
  
  uint8 tune_code_data[FTM_RF_MAX_TUNE_CODE_SIZE_IN_BYTES];
  /*!< Tune data data*/

  uint8 device_state;
  /*!< Tuner manager device state*/

  uint8 aperature_tx_algo_type;
  /*!< Aperature Tuner - Tx Algo Type */

  uint8 rx_algo_type;
  /*!< Tuner - Rx Algo Type */

  uint8 charge_pump_status;
  /*!< charge pump status of the tuners associated with the antenna */
  
  uint8 tune_code_mapping[FTM_RF_MAX_TUNE_CODE_SIZE_IN_BYTES];
  /*!< Byte level tunecode mapping. 
  Each byte carries the tuner instance ID of the corresponding 
  tunecode byte in TuneCode data*/

  uint8 impedance_scenario;
  /*!< Scenario for Impedance Tuner */

  uint8 aperture_scenario;
  /*!< Scenario for Aperture Tuner */

  uint8 asdiv_position;
  /*!< ASDIV position */

  uint8 reserved[8];
  /*!< Reserved bytes for future use*/

}ftm_rf_ant_tuner_tuner_status_v1_data_type;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Defintion of command header for FTM command request packet */
typedef PACK(struct)
{
  uint8 diag_cmd;
  /*!< Command Identification */

  uint8 diag_subsys_id;
  /*!< Sub-system Identification. For instance - 11 for FTM */

  uint16 rf_subsys_id;
  /*!< RF Mode Identification. 121 for FTM_RF_ANT_TUNER_C */

  ftm_rf_ant_tuner_comamnd_type command;
  /*!< FTM ant tuner command defined by #ftm_rf_ant_tuner_command_enum_type */

  uint16 req_len;
  /*!< Defines the request length of the FTM diag packet. This includes 
  the summation of the size of ftm_rf_ant_tuner_diag_header_type and size of remaning paylaod */

  uint16 rsp_len;
  /*!< Defines the response length of the FTM diag packet. This includes 
  the summation of the size of ftm_rf_ant_tuner_diag_header_type and size of remaning paylaod  */

} ftm_rf_ant_tuner_diag_header_type;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Defintion of command header for FTM RF Ant Tuner command request packet */
typedef PACK(struct)
{
  ftm_rf_ant_tuner_field_reserved_type reserved;
  /*!< Reserved Field */

  ftm_rf_ant_tuner_field_version_type version;
  /*!< Version of Req packet common header  */

} ftm_rf_ant_tuner_command_req_header_type;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Defintion of command header for FTM RF Ant Tuner command request packet */
typedef PACK(struct)
{
  ftm_rf_ant_tuner_diag_header_type diag_header;
  /*!<  Structure defining the header for FTM request packet */

  ftm_rf_ant_tuner_command_req_header_type command_header;
  /*!<  Structure defining the command header for RF FTM Ant Tuner command packet */

} ftm_rf_ant_tuner_header_req_type;


/*--------------------------------------------------------------------------------------------------------------------*/
/*! Defintion of command header for FTM RF Ant Tuner command response packet */
typedef PACK(struct)
{

  ftm_rf_ant_tuner_field_version_type version;
  /*!< Version of packet */

  ftm_rf_ant_tuner_field_status_code_mask_type status;
  /*!< Status code for the requested command */
  
} ftm_rf_ant_tuner_command_header_rsp_type;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Defintion of command header for FTM RF Test command response packet */
typedef PACK(struct)
{
  ftm_rf_ant_tuner_diag_header_type diag_header;
  /*!<  Structure defining the header for FTM response packet */

  ftm_rf_ant_tuner_command_header_rsp_type command_header;
  /*!<  Structure defining the command header for RF Test response packet */

} ftm_rf_ant_tuner_header_rsp_type;


/*--------------------------------------------------------------------------------------------------------------------*/
/*! Enumeration to indicate the type of commands available for FTM ANT Tuner  command */
typedef enum
{
  FTM_RF_ANT_TUNER_CMD_UNASSIGNED = 0, /*!< 0 : Unassigned command */

  FTM_RF_ANT_TUNER_CMD_SET_TUNER_PROPERITES = 1, /*!< 1 :  Set Tuner Properites command */

  FTM_RF_ANT_TUNER_CMD_GET_TUNER_STATUS = 2, /*!< 2 : Get Tuner Status command */

  FTM_RF_ANT_TUNER_CMD_SET_ALL_TUNER_PROPERTIES = 3, /*!< 3: Set ALL Tuner Properites command*/

  FTM_RF_ANT_TUNER_CMD_NUM  /*!< Max : Defines maximum number of command IDs */

} ftm_rf_ant_tuner_command_enum_type;



/*--------------------------------------------------------------------------------------------------------------------*/
/*! Enumeration to indicate the status code*/
typedef enum
{
  FTM_RF_ANT_TUNER_SC_SUCCESS = 0, /*!< 0 : Indicates the success status of requested command */

  FTM_RF_ANT_TUNER_SC_BAD_CMD = 1,/*!< 1 : Indicates bad command */

  FTM_RF_ANT_TUNER_SC_CMD_HANDLER_FAIL = 2,/*!< 2 : Indicates the failure from the command handler */

  FTM_RF_ANT_TUNER_SC_NUM  /*!< Max : Defines maximum number of error codes */

} ftm_rf_ant_tuner_status_code_enum_type;


/*--------------------------------------------------------------------------------------------------------------------*/
/*! Enumeration to indicate the type of the tune code updates */
typedef enum
{
  FTM_SET_TUNER_DISABLE_TUNE_CODE_OVERRIDE = 0, /*!< 0 : Disable TuneCode override */

  FTM_SET_TUNER_ENABLE_TUNE_CODE_OVERRIDE = 1, /*!< 1 :  Enable TuneCode override(stick to Tune code) */

  FTM_SET_TUNER_UPDATE_TUNE_CODE_ONCE = 2, /*!< 2 : Update TuneCode Once (Do not stick to TuneCode */

  FTM_SET_TUNER_TRIGGER_SCENARIO_OVERRIDE = 3, /*!< 3 : Scenario override */
  
  FTM_SET_TUNER_ENABLE_ACL = 4, /*!< 4 : Enable ACL*/
  
  FTM_SET_TUNER_DISABLE_ACL = 5, /*!< 5 : Disable ACL*/

  FTM_SET_TUNER_AOL_SCENARIO_OVERRIDE = 6, /*!< 6 : AOL Scenario Override*/

  FTM_SET_TUNER_ENABLE_MAAT_MODE_OVERRIDE = 7, /*!< 7 : MAAT Mode Override*/

  FTM_SET_TUNER_DISABLE_MAAT_MODE_OVERRIDE = 8, /*!< 8 : Disable MAAT Mode Override*/

  FTM_SET_TUNER_ENABLE_AAT_TBL_IDX_OVERRIDE = 9,/*!< 9: Enable the AAT_TBL_IDX Override*/

  FTM_SET_TUNER_DISABLE_AAT_TBL_IDX_OVERRIDE = 10,/*!< 10: Disable the AAT_TBL_IDX Override*/
  
  FTM_SET_TUNER_UPDATE_GENERATE_AND_VSWR_CYCLE = 11,/* override generate and vswr cycle intervals */

  FTM_SET_TUNER_TUNE_CODE_UPDATE_TYPE_NUM  /*!< Max : Defines maximum number of tune code update types */

} ftm_rf_ant_tuner_tune_code_update_enum_type;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Enumeration to indicate the source of tune code */
typedef enum
{
  FTM_GET_TUNER_TUNE_CODE_FROM_HW = 0, /*!< 0 : Tune code from harware  */

  FTM_GET_TUNER_TUNE_CODE_FROM_SW = 1, /*!< 1 : Tune code from software states */

  FTM_GET_TUNER_TUNE_CODE_SOURCE_NUM  /*!< Max : Defines maximum number of tune code sources*/

} ftm_rf_ant_tuner_tune_code_sources_type;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Enumeration for defining the version of the packet */
typedef enum
{
  FTM_RF_ANT_TUNER_PKT_VERSION0 = 0, /*!< VER0 packet */
  
  FTM_RF_ANT_TUNER_PKT_VERSION1 = 1, /*!< VER1 packet */

  FTM_RF_ANT_TUNER_PKT_VERSION2 = 2, /*!< VER2 packet */

  FTM_RF_ANT_TUNER_PKT_MAX_VERSION /*!< <MAX VER packet */
}ftm_rf_ant_tuner_rsp_version;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Enumeration to indicate the type of update to update all active tuners */
typedef enum
{
  FTM_SET_ALL_TUNER_UPDATE_TUNE_CODE = 0, /*!< 0 : Update tune codes for all active tuners */

  FTM_SET_ALL_TUNER_UPDATE_TYPE_NUM  /*!< Max : Defines maximum number of update types */
}ftm_rf_ant_tuner_update_all_tuner_enum_type;

/*====================================================================================================================*/
/*!
     			ANT Tuner Sub Commands Request/Response Packet structure 
*/     
/*====================================================================================================================*/

/*====================================================================================================================*/
/*!
     			FTM_RF_ANT_TUNER_CMD_SET_TUNER_PROPERITES
*/     
/*====================================================================================================================*/

/*! Defintion of structure for FTM RF Ant tuner set tuner properites request 
data*/
typedef PACK(struct)
{
  ftm_rf_ant_tuner_field_version_type version;
  /*!< Version of Req packet */

  uint8 antenna_num;
  /*!< Indicates the Antenna number */

  ftm_rf_ant_tuner_tune_code_update_enum_type tune_code_update_type;
  /*!< Indicates the type of the tune code updates */

  uint8 tune_code_size;
  /*!< Indicates the size of tune code in bytes */

  uint8 tune_code_data[FTM_RF_MAX_TUNE_CODE_SIZE_IN_BYTES];
  /*!< Tune data data*/

  uint8 scenario;
  /*!< Impedance Scenario number that needs to be programmed*/
  
  uint8 reserved[8];
  /*!<Reserved bytes for future use */

}ftm_rf_ant_tuner_set_tuner_properties_req_data_type;

/*! Defintion of structure for FTM RF Ant tuner set tuner properites request 
data*/
typedef PACK(struct)
{
  ftm_rf_ant_tuner_field_version_type version;
  /*!< Version of Req packet */

  uint8 antenna_num;
  /*!< Indicates the Antenna number */

  ftm_rf_ant_tuner_tune_code_update_enum_type tune_code_update_type;
  /*!< Indicates the type of the tune code updates */

  uint8 tune_code_size;
  /*!< Indicates the size of tune code in bytes */

  uint8 tune_code_data[FTM_RF_MAX_TUNE_CODE_SIZE_IN_BYTES];
  /*!< Tune data data*/

  uint8 impedance_scenario;
  /*!< Impedance Scenario number that needs to be programmed*/

  uint8 aperture_scenario;
  /*!<  Aperture Scenario number that needs to be programmed*/

  uint8 maat_mode;
  /*!<  maat mode that needs to be overriden*/
  
  uint8 aat_tbl_idx;
  /*!< path idx that needs to be overriden*/
  
  uint8 reserved[5];
  /*!<Reserved bytes for future use */

}ftm_rf_ant_tuner_set_tuner_properties_v0_req_data_type;

/*! Defintion of structure for FTM RF Ant tuner set tuner properites request packet */
typedef PACK(struct)
{
  ftm_rf_ant_tuner_header_req_type header;
  /*!<  Structure defining the header for FTM RF Ant tuner set tuner properites request packet */

  ftm_rf_ant_tuner_set_tuner_properties_req_data_type req_data;
  /*!<  Structure defining the FTM RF Ant tuner set tuner properites request data */

}ftm_rf_ant_tuner_set_tuner_properties_req_pkt_type;

/*! Defintion of structure for FTM RF Ant tuner set tuner properites request packet: Version 0 */
typedef PACK(struct)
{
  ftm_rf_ant_tuner_header_req_type header;
  /*!<  Structure defining the header for FTM RF Ant tuner set tuner properites request packet */

  ftm_rf_ant_tuner_set_tuner_properties_v0_req_data_type req_data;
  /*!<  Structure defining the FTM RF Ant tuner set tuner properites request data */

}ftm_rf_ant_tuner_set_tuner_properties_v0_req_pkt_type;

/*! Defintion of structure for FTM RF Ant tuner set tuner properites response packet */
typedef PACK(struct)
{
  ftm_rf_ant_tuner_header_rsp_type header;
  /*!<  Structure defining the header for FTM RF Ant tuner set tuner 
  properites response packet */
  
  ftm_rf_ant_tuner_set_tuner_properties_req_data_type req_data;
  /*!<  Echo back the req data */

  ftm_rf_ant_tuner_field_version_type version;
  /*!< Version of Response packet */

}ftm_rf_ant_tuner_set_tuner_properties_rsp_pkt_type;

/*! Defintion of structure for FTM RF Ant tuner set tuner properites response packet version 0*/
typedef PACK(struct)
{
  ftm_rf_ant_tuner_header_rsp_type header;
  /*!<  Structure defining the header for FTM RF Ant tuner set tuner 
  properites response packet */
  
  ftm_rf_ant_tuner_set_tuner_properties_v0_req_data_type req_data;
  /*!<  Echo back the req data */

  ftm_rf_ant_tuner_field_version_type version;
  /*!< Version of Response packet */

}ftm_rf_ant_tuner_set_tuner_properties_v0_rsp_pkt_type;


/*====================================================================================================================*/
/*!
     			FTM_RF_ANT_TUNER_CMD_GET_TUNER_STATUS
*/     
/*====================================================================================================================*/

/* Defintion of structure for FTM RF Ant tuner get tuner status request data*/
typedef PACK(struct)
{
  ftm_rf_ant_tuner_field_version_type version;
  /*!< Version of Req packet */

  ftm_rf_ant_tuner_tune_code_sources_type tune_code_source;
  /*!< Indicates the source of the tune code */

}ftm_rf_ant_tuner_get_tuner_status_req_data_type;

/*! Defintion of structure for FTM RF Ant tuner get tuner status request packet */
typedef PACK(struct)
{
  ftm_rf_ant_tuner_header_req_type header;
  /*!<  Structure defining the header for FTM RF Ant tuner get tuner status request packet */

  ftm_rf_ant_tuner_get_tuner_status_req_data_type req_data;
  /*!<	Structure defining the FTM RF Ant tuner set tuner properites request data */

}ftm_rf_ant_tuner_get_tuner_status_req_pkt_type;


/*! Defintion of structure for FTM RF Ant tuner get tuner status response packet */
typedef PACK(struct)
{
  ftm_rf_ant_tuner_header_rsp_type header;
  /*!<  Structure defining the header for FTM RF Ant tuner get tuner status request packet */

  ftm_rf_ant_tuner_get_tuner_status_req_data_type req_data;
  /*!<  Echo back the req data */

  ftm_rf_ant_tuner_field_version_type version;
  /*!< Version of Response packet */

  uint8 num_antennas;
  /*!< Number of antennas*/

  ftm_rf_ant_tuner_tuner_status_data_type tuner_status_data[FTM_RF_MAX_NUM_ANTENNAS];
  /*!< Tune status data */

}ftm_rf_ant_tuner_get_tuner_status_rsp_pkt_type;

/*! Defintion of structure for FTM RF Ant tuner get tuner status response packet :Version 1 */
typedef PACK(struct)
{
  ftm_rf_ant_tuner_header_rsp_type header;
  /*!<  Structure defining the header for FTM RF Ant tuner get tuner status request packet */

  ftm_rf_ant_tuner_get_tuner_status_req_data_type req_data;
  /*!<  Echo back the req data */

  ftm_rf_ant_tuner_field_version_type version;
  /*!< Version of Response packet */

  uint8 num_antennas;
  /*!< Number of antennas*/

  ftm_rf_ant_tuner_tuner_status_v1_data_type tuner_status_data[FTM_RF_MAX_NUM_ANTENNAS];
  /*!< Tune status data v1 */

}ftm_rf_ant_tuner_get_tuner_status_v1_rsp_pkt_type;

/*====================================================================================================================*/
/*!
          FTM_RF_ANT_TUNER_CMD_SET_ALL_TUNER_PROPERTIES
*/     
/*====================================================================================================================*/

/* Defintion of structure for FTM RF Ant Tuner Update All Tuners Request Data*/
typedef PACK(struct)
{
  ftm_rf_ant_tuner_field_version_type version;
  /*!< Version of Req packet */

  ftm_rf_ant_tuner_update_all_tuner_enum_type update_type;
  /*!< Indicates the type of the updates */

  uint8 reserved[8];

}ftm_rf_ant_tuner_set_all_tuner_properties_req_data_type;


/*! Defintion of structure for FTM RF Ant tuner Update All Tuners Request Packet*/
typedef PACK(struct)
{
  ftm_rf_ant_tuner_header_req_type header;
  /*!<  Structure defining the header for FTM RF Ant tuner set tuner properites request packet */

  ftm_rf_ant_tuner_set_all_tuner_properties_req_data_type req_data;
  /*!<  Structure defining the FTM RF Ant tuner set tuner properites request data */

}ftm_rf_ant_tuner_set_all_tuner_properties_req_pkt_type;

/*! Defintion of structure for FTM RF Ant tuner Update All Tuners Response Packet*/
typedef PACK(struct)
{
  ftm_rf_ant_tuner_header_rsp_type header;
  /*!<  Structure defining the header for FTM RF Ant tuner set tuner properites request packet */

  ftm_rf_ant_tuner_set_all_tuner_properties_req_data_type req_data;
  /*!<  Structure defining the FTM RF Ant tuner set tuner properites request data */

  ftm_rf_ant_tuner_field_version_type version;
  /*!< Version of Req packet */

}ftm_rf_ant_tuner_set_all_tuner_properties_rsp_pkt_type;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* FTM_RF_ANT_TUNER_INTERFACE_H */

