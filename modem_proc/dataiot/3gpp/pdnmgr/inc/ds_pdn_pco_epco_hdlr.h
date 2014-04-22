#ifndef DS_PDN_PCO_EPCO_HDLR_H
#define DS_PDN_PCO_EPCO_HDLR_H

/*===========================================================================
                      DS_PDN_PCO_EPCO_HDLR.H
DESCRIPTION
This file exposes PDN PCO/ePCO handler through APIs and data structures.

Copyright (c) 2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/3gpp/pdnmgr/inc/ds_pdn_pco_epco_hdlr.h#1 $
  $DateTime: 2020/03/17 08:03:16 $$Author: pwbldsvc $

when           who    what, where, why
--------    ---    ----------------------------------------------------------
08/09/17    sd      First checkin
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "ds_pdn_defs.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

 /* Max number of protocol ID's */ 
 /* Increament this num everytime a new protocol is added to EPCO */
 
#define MAX_NUM_EPCO_PROTOCOL_ID 20
 
 /* Structure for individual protocol information */
 
 /** @brief Individual protocol information.
 */
 typedef struct
 {
   byte protocol_id[2];
   /**< Protocol IDs. */
   byte protocol_len;
   /**< Protocol ID content length. */
   byte protocol_contents[MAX_PROTOCOL_ID_CONTENT];
   /**< Protocol content. */
 } ds_pdn_protocol_id_s;
  
 /*~ FIELD protocol_id_s.protocol_contents VARRAY MAX_PROTOCOL_ID_CONTENT
     LENGTH protocol_id_s.protocol_len */
  
 /** @brief Protocol configuration options.
 */
 typedef struct
 {
   boolean                 valid;
   /**< Whether protocol configuration options are present. */
   byte                    config_protocol;
   /**< Configuration protocol. */
   word                    num_protocol_ids;
   /**< Number of protocol IDs. */
   
   ds_pdn_protocol_id_s   *protocol_info_p[MAX_NUM_EPCO_PROTOCOL_ID];
 
   /**< Protocol information. */
 } ds_pdn_protocol_cfg_options_s;

 /*-------------------------------------------------------------------- 
   PCO information
 --------------------------------------------------------------------*/
 typedef struct
 {
   uint16  mcc;
   uint16  mnc;
   boolean mnc_includes_pcs_digit;
   uint8   app_specific_info_len;
   uint8   app_specific_info[MAX_OP_RES_PCO_APP_SPECIFIC_LEN];
   uint16  container_id;
 }ds_pdn_mgr_op_reserved_pco_type;
 
 /*---------------------------------------------------------------------------
   Modem reserved pco information type.
   num_pcos     : no of valid pco entries 
   pco_info_ptr : ptr to the pco information
 ---------------------------------------------------------------------------*/
 typedef struct
 {
   uint32                            num_pcos;
   ds_pdn_mgr_op_reserved_pco_type   pco_info;
 } ds_pdn_mgr_modem_reserved_pco_type;



/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================
  FUNCTION ds_pdn_pco_epco_hdlr_convert_nas_epco_to_ds
  
  DESCRIPTION
    This function converts the EPCO from NAS to DS format so that it can be
    decoded by the current PCO decode functions.
  
  PARAMETERS
    [in]   ext_protocol_config_options_p
    [out]  proto_config_option_p
  
  DEPENDENCIES
    None.
  
  RETURN VALUE
    boolean
  
  SIDE EFFECTS
    None.
  
  ===========================================================================*/
  
  boolean ds_pdn_pco_epco_hdlr_convert_nas_epco_to_ds
  (
    ext_protocol_cfg_options_T      *ext_protocol_config_options_p,
    ds_pdn_protocol_cfg_options_s  *proto_config_option_p
  );
  
  /*===========================================================================
  FUNCTION ds_pdn_pco_epco_hdlr_convert_ds_epco_to_nas
  
  DESCRIPTION
    This function converts the EPCO from DS to NAS format before it is sent
    out to NAS
  
  PARAMETERS
    [in]   proto_config_option_p
    [out]  ext_protocol_config_options_p
  
  DEPENDENCIES
    None.
  
  RETURN VALUE
    boolean
  
  SIDE EFFECTS
    None.
  
  ===========================================================================*/
  
  boolean ds_pdn_pco_epco_hdlr_convert_ds_epco_to_nas
  (
    ds_pdn_protocol_cfg_options_s  *proto_config_option_p,
    ext_protocol_cfg_options_T      *ext_protocol_config_options_p
  );
  
  /*===========================================================================
  FUNCTION  ds_pdn_pco_epco_hdlr_convert_nas_pco_to_ds
  
  DESCRIPTION
    This function converts the PCO from NAS format to DS format before it is decoded.
  
  PARAMETERS
    [in]   nas_proto_config_option_p
    [out]  ds_proto_config_option_p
  
  DEPENDENCIES
    None.
  
  RETURN VALUE
    boolean
  
  SIDE EFFECTS
    None.
  
  ===========================================================================*/
  
  boolean ds_pdn_pco_epco_hdlr_convert_nas_pco_to_ds
  (
    protocol_cfg_options_T          *nas_proto_config_option_p,
    ds_pdn_protocol_cfg_options_s  *ds_proto_config_option_p
  );
  
  /*===========================================================================
  FUNCTION  ds_pdn_pco_epco_hdlr_convert_ds_pco_to_nas
  
  DESCRIPTION
    This function converts the PCO from NAS format to DS format before it is decoded.
  
  PARAMETERS
    [in]   ds_proto_config_option_p
    [out]  nas_proto_config_option_p
  
  DEPENDENCIES
    None.
  
  RETURN VALUE
    boolean
  
  SIDE EFFECTS
    None.
  
  ===========================================================================*/
  
  boolean ds_pdn_pco_epco_hdlr_convert_ds_pco_to_nas
  (
    ds_pdn_protocol_cfg_options_s  *ds_proto_config_option_p,
    protocol_cfg_options_T          *nas_proto_config_option_p
  );

/*===========================================================================
FUNCTION  ds_pdn_pco_epco_hdlr_is_epco_enabled

DESCRIPTION
  This function checks if the EPCO is enabled or not

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  boolean

SIDE EFFECTS
  None.

===========================================================================*/

boolean ds_pdn_pco_epco_hdlr_is_epco_enabled
(
  void
);

/*===========================================================================
FUNCTION  ds_pdn_pco_epco_hdlr_get_max_number_of_protocol_ids

DESCRIPTION
  This function returns the max number of protocols ID's used for PCO/EPCO

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  max number of protocol ID's

SIDE EFFECTS
  None.

===========================================================================*/

uint8 ds_pdn_pco_epco_hdlr_get_max_number_of_protocol_ids
(
  void
);

#endif /* DS_PDN_PCO_EPCO_HDLR_H */

