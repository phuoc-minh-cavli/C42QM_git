#ifndef DS_PDN_PROFILE_VALIDATOR_H
#define DS_PDN_PROFILE_VALIDATOR_H

/*===========================================================================
                      DS_PDN_PROFILE_VALIDATOR.H
DESCRIPTION
This file exposes PDN profile validator through APIs and data structures.

Copyright (c) 2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/3gpp/pdnmgr/inc/ds_pdn_profile_validator.h#1 $
  $DateTime: 2020/03/17 08:03:16 $$Author: pwbldsvc $

when           who    what, where, why
--------    ---    ----------------------------------------------------------
08/09/17    sd      First checkin
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "ds_pdn_defs.h"



/*===========================================================================

                     MACRO

===========================================================================*/
/*---------------------------------------------------------------------------
  length of string "mncXXX.mccYYY.gprs" or 
  "6mncXXX6mccYYY113gppnetwork4org" is 19 bytes
---------------------------------------------------------------------------*/
#define DS_PROFILE_OPERATOR_IDENTIFIER_TOTAL_LENGTH 19
#define DS_PROFILE_NETWORK_IDENTIFIER_MAX_LENGTH 62
#define DS_PROFILE_MCC_MNC_STR_LENGTH_WITH_DELIM 4
#define DS_PROFILE_GPRS_STR_LENGTH_WITH_DELIM 5
#define DS_PROFILE_MCC_MNC_STR_LENGTH 3
#define DS_PROFILE_GPRS_STR_LENGTH 4
#define DS_PROFILE_MCC_MNC_LABEL_LENGTH 6
#define DS_PROFILE_GPRS_LABEL_LENGTH 4
#define DS_PROFILE_RAC_LAC_RNC_STR_LENGTH 3
#define DS_PROFILE_SGSN_STR_LENGTH 4
#define DS_PROFILE_MCC_MNC_LENGTH_WITH_DELIM 7

/*---------------------------------------------------------------------------
  length of string "mncXXX.mccYYY.3gppnetwork.org" or 
  "6mncXXX6mccYYY113gppnetwork4org" is 30 bytes
---------------------------------------------------------------------------*/
#define DS_PROFILE_FQDN_OPERATOR_IDENTIFIER_TOTAL_LENGTH 30

#define DS_PROFILE_APN_STR_LENGTH 3
#define DS_PROFILE_APN_STR_LENGTH_WITH_DELIM 4
#define DS_PROFILE_EPC_STR_LENGTH 3
#define DS_PROFILE_EPC_STR_LENGTH_WITH_DELIM 4

/*---------------------------------------------------------------------------
  length of string ".apn.epc" 8 bytes
---------------------------------------------------------------------------*/
#define DS_PROFILE_APN_EPC_STR_LENGTH_WITH_DELIM 8

#define DS_PROFILE_3GPPNETWORK_STR_LENGTH 11
#define DS_PROFILE_3GPPNETWORK_STR_LENGTH_WITH_DELIM 12
#define DS_PROFILE_ORG_STR_LENGTH 3
#define DS_PROFILE_ORG_STR_LENGTH_WITH_DELIM 4

/*===========================================================================

                         INTERNAL DEFINATION & TYPES

===========================================================================*/
/*---------------------------------------------------------------------------
  Structure block that contains forbidden APN name info as read from EFS

  /data/forbidden_apn_list.txt
---------------------------------------------------------------------------*/
typedef struct
{
  q_link_type        link;
  char               forbidden_apn[DS_SYS_MAX_APN_LEN];
}ds_profile_validator_forbidden_apn_info_type;

/*------------------------------------------------------------------------------
  Structure that contains the information extracted any EFS file
------------------------------------------------------------------------------*/
typedef struct
{
  q_link_type          link;
  char                *extracted_data;
  uint16               extracted_data_length;
}ds_profile_validator_extracted_info_type;

/*===========================================================================

                     FUNCTION DELCARATION

===========================================================================*/


/*===========================================================================
FUNCTION ds_pdn_get_network_identifer_from_apn_name

DESCRIPTION
  This function is used to get network identifier part from apn name stored in the profile. This
  function leaves out the operator identifier. It picks up only the labels
  that reflect the network identifier.

PARAMETERS
  IN
  input_apn - Pointer to the APN in the profile. 
  input_apn_length - length of the apn string

  OUT
  network_identifier - Pointer to the byte array in which the network identifier of APN is returned

DEPENDENCIES
  None

RETURN VALUE
  boolean - TRUE if extraction is successful
            FALSE if there is any error
   
SIDE EFFECTS
  None
===========================================================================*/
uint8 ds_pdn_get_network_identifer_from_apn_name
(
  char*  input_apn,
  byte   input_apn_length,
  char*  network_identifier
);


/*===========================================================================
FUNCTION ds_pdn_profile_apn_is_network_identifier_valid

DESCRIPTION
  This functions checks if network identifier is fully compliant with 3gpp spec.

PARAMETERS
  IN
  apn_ni - Pointer to the network identifier in the profile. 
  apn_ni_len - length of the network identifier string


DEPENDENCIES
  None

RETURN VALUE
  boolean - TRUE if network indentifier is valid
            FALSE  otherwise
   
SIDE EFFECTS
  None
===========================================================================*/

boolean ds_pdn_profile_apn_is_network_identifier_valid
(
   byte*  apn_ni,
   uint8 apn_ni_len
);

/*===========================================================================
FUNCTION ds_pdn_profile_validate_apn_name

DESCRIPTION
  The function validates apn name according to 3gpp spec
    
PARAMETERS: 
  *apn_name  --  apn name char pointer
  *info     --  apn name lenght
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  TRUE - if apn name is valid
  FALSE  - otherwise
  
SIDE EFFECTS 
  None 
  
===========================================================================*/

boolean ds_pdn_profile_validate_apn_name
(
  char *apn_name,
  uint8 apn_name_len
);


/*===========================================================================
FUNCTION ds_pdn_decode_apn_name

DESCRIPTION
  This function is used to decode the APN name returned by the network. This
  function leaves out the operator identified. It picks up only the labels
  that reflect the network identifier.

PARAMETERS
  IN
  encoded_apn - Pointer to the APN returned by the network. This is encoded
  encoded_apn_length - length of the encoded string

  OUT
  decoded_apn - Pointer to the byte array in which the decoded APN is returned

DEPENDENCIES
  None

RETURN VALUE
  boolean - TRUE if decoding is successful
            FALSE if there is any error
   
SIDE EFFECTS
  None
===========================================================================*/
boolean ds_pdn_decode_apn_name
(
  byte*  encoded_apn,
  uint32 encoded_apn_length,
  byte*  decoded_apn
);

/*===========================================================================
FUNCTION ds_pdn_decode_apn_name_non_strict

DESCRIPTION
  This function is used to decode the APN name returned by the network. This
  function leaves out the operator identified. It picks up only the labels
  that reflect the network identifier.

PARAMETERS
  IN
  encoded_apn - Pointer to the APN returned by the network. This is encoded
  encoded_apn_length - length of the encoded string

  OUT
  decoded_apn - Pointer to the byte array in which the decoded APN is returned

DEPENDENCIES
  None

RETURN VALUE
  boolean - TRUE if decoding is successful
            FALSE if there is any error
   
SIDE EFFECTS
  None
===========================================================================*/
boolean ds_pdn_decode_apn_name_non_strict
(
  byte*  encoded_apn,
  uint32 encoded_apn_length,
  byte*  decoded_apn
);

/*===========================================================================
FUNCTION ds_pdn_profile_validator_is_apn_forbidden

DESCRIPTION
  This function invokes the utility to check if the APN is a forbidden APN or
  not

PARAMETERS 
  pdn_context_ptr: PDN who APN name needs to be validated 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_profile_validator_is_apn_forbidden
(
 ds_pdn_mgr_s       *pdn_mgr_p
);

/*===========================================================================
FUNCTION  ds_pdn_profile_validator_powerup_init

DESCRIPTION
  This function initializes  forbidden APN queue at powerup

PARAMETERS
  is_init          : Is it part of power up init

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_profile_validator_powerup_init(boolean is_init);

/*===========================================================================
FUNCTION      ds_pdn_profile_validator_read_apn_info_from_efs

DESCRIPTION   This function will read from the efs file to see if the 
              reject/forbidden apn has been set 

              EFS File Format - Param_Name:Param_Val;
              For example     - reject_apn:test.apn;             

DEPENDENCIES  None

RETURN VALUE  TRUE - If parsing is successful 
              FALSE - Otherwise 

SIDE EFFECTS  None
===========================================================================*/
boolean ds_pdn_profile_validator_read_apn_info_from_efs 
(  
  char                        *comparison_tag,
  char                        *efs_file_name,
  q_type                      *efs_extracted_info_q
);

#endif /* DS_PDN_PROFILE_VALIDATOR_H */
