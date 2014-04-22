/*===========================================================================
                      DS_PDN_PROFILE_VALIDATOR.C
DESCRIPTION
This file defines PDN profile validator APIs.

Copyright (c) 2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/3gpp/pdnmgr/src/ds_pdn_profile_validator.c#1 $
  $DateTime: 2020/03/17 08:03:16 $$Author: pwbldsvc $

when           who    what, where, why
--------    ---    ----------------------------------------------------------
08/09/17    sd      First checkin
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "ds_pdn_defs.h"
#include "ds_pdn_profile_validator.h"
#include "ds_pdn_manager.h"
#include "ds_eps_pdn_manager.h"
#include "ds_pdn_nv_manager.h"
#include "ds_pdn_hdlr.h"


#define LABEL_DELIMITOR 0x2e

/*===========================================================================

                         LOCAL VARIABLES

===========================================================================*/
static q_type ds_profile_validator_forbidden_apn_q = {{NULL}};

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
  uint8 - length of the network identifier
 
SIDE EFFECTS
  None
===========================================================================*/
uint8 ds_pdn_get_network_identifer_from_apn_name
(
  char  *input_apn,
  byte   input_apn_length,
  char  *network_identifier
)
{
  uint32  index = 0;
  boolean has_operator_identifier_part = FALSE;
  boolean has_fqdn_operator_identifier_part = FALSE;
  byte apn_length = input_apn_length;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (input_apn == NULL) 
  {
     return 0;
  }

  apn_length = strnlen(input_apn, input_apn_length) ; 
  if (apn_length < input_apn_length) 
  {
     input_apn_length = apn_length;
  }

  if (input_apn_length == 0 || input_apn[0] == 0) 
  {
     return 0;
  }

  if(network_identifier == NULL)
  {
    return 0;
  }

  /*------------------------------------------------------------------------
    Determine if encoded_apn has operator identifier part. A valid operating
    would have the 3 labels in the following format 
    "mnc<MNC>.mcc<MCC>.gprs" 

   For example resolved apn name is (in byte format)
   {7,'a','p','n','n','a','m','e',6,'m','n','c','0','0','1',6,'m','c','c','0','1','1',4,'g','p','r','s'}
   "mnc001.mcc011.gprs" would be operator identifier (total 19 bytes)
   "apnname" would be network identifier
   
   The APN can also be in FQDN (Fully Qualified Domain Name) format. The
   network operator would be
   "apn.epc.mnc<MNC>.mcc<MCC>.3gppnetwork.org"
   
   For example resolved apn name is (in byte format)
   {3,'i','m','s',3,'a','p','n',3,'e','p','c',6,'m','n','c','0','0','1',6,'m','c','c','0','0','1',11,'3','g','p','p','n','e','t','w','o','r','k',3,'o','r','g'}
   "ims.apn.epc.mnc001.mcc001.3gppnetwork.org" would be the operator identifier (total 42 bytes)
   "ims" would be the network identifier
  
    For references check:

    Spec: 123 003 V 12.4.1 (2014-10)
    9 Definition of Access Point Name
    19.4.2.2 Access Point Name FQDN (APN-FQDN)

  ------------------------------------------------------------------------*/

  /*------------------------------------------------------------------------
    Traverse the encoded APN to determine the number of labels in the string
  ------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
    check for mnc<MNC>.mcc<MCC>.gprs format
   --------------------------------------------------------------------------*/
  if(input_apn_length > DS_PROFILE_OPERATOR_IDENTIFIER_TOTAL_LENGTH ) 
  {
    do
    {
      has_operator_identifier_part = TRUE;
      index = input_apn_length - DS_PROFILE_OPERATOR_IDENTIFIER_TOTAL_LENGTH;

      if(strncasecmp(input_apn+index,".mnc",DS_PROFILE_MCC_MNC_STR_LENGTH_WITH_DELIM) != 0)
      {
        has_operator_identifier_part = FALSE;
        break;
      }

      index += DS_PROFILE_MCC_MNC_LENGTH_WITH_DELIM;

      if(strncasecmp(input_apn+index,".mcc",DS_PROFILE_MCC_MNC_STR_LENGTH_WITH_DELIM) != 0)
      {
        has_operator_identifier_part = FALSE;
        break;
      }

      index += DS_PROFILE_MCC_MNC_LENGTH_WITH_DELIM;

      if(strncasecmp(input_apn+index,".gprs",DS_PROFILE_GPRS_STR_LENGTH_WITH_DELIM) != 0)
      {
        has_operator_identifier_part = FALSE;
        break;
      }
    }while (0); 
  }

  /*-------------------------------------------------------------------------
    check for FQDN format apn.epc.mnc<MNC>.mcc<MCC>.3gppnetwork.org.
    Skip check if format is already valid from non-FQDN check
   --------------------------------------------------------------------------*/
  if ((input_apn_length > DS_PROFILE_FQDN_OPERATOR_IDENTIFIER_TOTAL_LENGTH) && 
           (has_operator_identifier_part == FALSE))
  {
    do
    {
      has_fqdn_operator_identifier_part = TRUE;
      index = input_apn_length - DS_PROFILE_FQDN_OPERATOR_IDENTIFIER_TOTAL_LENGTH;

      if(strncasecmp(input_apn+index,".mnc",DS_PROFILE_MCC_MNC_STR_LENGTH_WITH_DELIM) != 0)
      {
        has_fqdn_operator_identifier_part = FALSE;
        break;
      }

      index += DS_PROFILE_MCC_MNC_LENGTH_WITH_DELIM;

      if(strncasecmp(input_apn+index,".mcc",DS_PROFILE_MCC_MNC_STR_LENGTH_WITH_DELIM) != 0)
      {
        has_fqdn_operator_identifier_part = FALSE;
        break;
      }

      index += DS_PROFILE_MCC_MNC_LENGTH_WITH_DELIM;

      if(strncasecmp(input_apn+index,".3gppnetwork",DS_PROFILE_3GPPNETWORK_STR_LENGTH_WITH_DELIM) != 0)
      {
        has_fqdn_operator_identifier_part = FALSE;
        break;
      }

      index += DS_PROFILE_3GPPNETWORK_STR_LENGTH_WITH_DELIM;

      if(strncasecmp(input_apn+index,".org",DS_PROFILE_ORG_STR_LENGTH_WITH_DELIM) != 0)
      {
        has_fqdn_operator_identifier_part = FALSE;
        break;
      }
    }while (0);
  }

  if (has_operator_identifier_part) 
  {
    input_apn_length -= DS_PROFILE_OPERATOR_IDENTIFIER_TOTAL_LENGTH;
  }
  else if (has_fqdn_operator_identifier_part)
  {
    input_apn_length -= DS_PROFILE_FQDN_OPERATOR_IDENTIFIER_TOTAL_LENGTH;
    if((input_apn_length > DS_PROFILE_APN_EPC_STR_LENGTH_WITH_DELIM) &&
        (strncasecmp((const char*)(input_apn+(input_apn_length-DS_PROFILE_APN_EPC_STR_LENGTH_WITH_DELIM)),
              ".apn.epc",DS_PROFILE_APN_EPC_STR_LENGTH_WITH_DELIM) == 0))
    {
      input_apn_length -= DS_PROFILE_APN_EPC_STR_LENGTH_WITH_DELIM;
    }
  }

  if (input_apn_length <= DS_PROFILE_NETWORK_IDENTIFIER_MAX_LENGTH) 
  {
    memscpy(network_identifier, input_apn_length,
                 input_apn,
                 input_apn_length);
    network_identifier[input_apn_length] = 0x0;
  }
  
  return input_apn_length;
} /* ds_pdn_get_network_identifer_from_apn_name */


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
  byte   *apn_ni,
  uint8   apn_ni_len
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
   if (apn_ni_len == 0)
   {
      return TRUE;
   }

   if (apn_ni_len > DS_PROFILE_NETWORK_IDENTIFIER_MAX_LENGTH) 
   {
     return FALSE;
   }

   if (apn_ni == NULL)
   {
      return FALSE;
   }

   if(apn_ni_len >= DS_PROFILE_GPRS_STR_LENGTH_WITH_DELIM && 
       strncasecmp((char*)apn_ni + apn_ni_len-DS_PROFILE_GPRS_STR_LENGTH_WITH_DELIM ,
                   ".gprs",DS_PROFILE_GPRS_STR_LENGTH_WITH_DELIM) == 0)
    {
      return FALSE;
    }
    else if(apn_ni_len >= DS_PROFILE_RAC_LAC_RNC_STR_LENGTH && 
            (strncasecmp((char*)apn_ni,"rac",DS_PROFILE_RAC_LAC_RNC_STR_LENGTH) == 0))
    {
      return FALSE;
    }
    else if(apn_ni_len >= DS_PROFILE_RAC_LAC_RNC_STR_LENGTH &&
            (strncasecmp((char*)apn_ni,"lac",DS_PROFILE_RAC_LAC_RNC_STR_LENGTH) == 0))
    {
      return FALSE;
    }
    else if(apn_ni_len >= DS_PROFILE_RAC_LAC_RNC_STR_LENGTH &&
            (strncasecmp((char*)apn_ni,"rnc",DS_PROFILE_RAC_LAC_RNC_STR_LENGTH) == 0))
    {
      return FALSE;
    }
    else if(apn_ni_len >= DS_PROFILE_SGSN_STR_LENGTH &&
            (strncasecmp((char*)apn_ni,"sgsn",DS_PROFILE_SGSN_STR_LENGTH) == 0))
    {
      return FALSE;
    }

    return TRUE;
} /*ds_pdn_profile_apn_is_network_identifier_valid*/

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
  char   *apn_name,
  uint8   apn_name_len
)
{
  uint8 network_ident_length = 0;
  byte apn_network_identifier[DS_PROFILE_NETWORK_IDENTIFIER_MAX_LENGTH+1];
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(apn_name == NULL)
  {
    return TRUE;
  }

  //Validate operator identifier and extract the network identifier here
  network_ident_length = ds_pdn_get_network_identifer_from_apn_name(
     apn_name, apn_name_len, (char*)apn_network_identifier);

  if (!ds_pdn_profile_apn_is_network_identifier_valid(apn_network_identifier,
                                                      network_ident_length )) 
  {
     return FALSE;
  }

  return TRUE;
} /*ds_pdn_profile_validate_apn_name*/

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
  byte    *encoded_apn,
  uint32   encoded_apn_length,
  byte    *decoded_apn
)
{
  uint32  label_count = 0;
  uint32  label_length = 0;
  uint32  index = 0;
  boolean ret_val = TRUE;
  uint32  apn_string_offset = 0;
  boolean has_operator_identifier_part = FALSE;
  boolean has_fqdn_operator_identifier_part = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*------------------------------------------------------------------------
    Determine if encoded_apn has operator identifier part. A valid operating
    would have the 3 labels in the following format 
    "mnc<MNC>.mcc<MCC>.gprs" 

   For example resolved apn name is (in byte format)
   {7,'a','p','n','n','a','m','e',6,'m','n','c','0','0','1',6,'m','c','c','0','1','1',4,'g','p','r','s'}
   "mnc001.mcc011.gprs" would be operator identifier (total 19 bytes)
   "apnname" would be network identifier
   
   The APN can also be in FQDN (Fully Qualified Domain Name) format. The
   network operator would be
   "apn.epc.mnc<MNC>.mcc<MCC>.3gppnetwork.org"
   
   For example resolved apn name is (in byte format)
   {3,'i','m','s',3,'a','p','n',3,'e','p','c',6,'m','n','c','0','0','1',6,'m','c','c','0','0','1',11,'3','g','p','p','n','e','t','w','o','r','k',3,'o','r','g'}
   "apn.epc.mnc001.mcc001.3gppnetwork.org" would be the operator identifier (total 38 bytes)
   "ims" would be the network identifier
  
    For references check:

    Spec: 123 003 V 12.4.1 (2014-10)
    9 Definition of Access Point Name
    19.4.2.2 Access Point Name FQDN (APN-FQDN)

  ------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
    check for mnc<MNC>.mcc<MCC>.gprs format
   --------------------------------------------------------------------------*/
  if(encoded_apn_length > DS_PROFILE_OPERATOR_IDENTIFIER_TOTAL_LENGTH) //Greater than 19 bytes
  {
    do
    {
      has_operator_identifier_part = TRUE;
      index = encoded_apn_length - DS_PROFILE_OPERATOR_IDENTIFIER_TOTAL_LENGTH;
      label_length = (uint32)(*(encoded_apn+index));
      /*-------------------------------------------------------------------------
        check length and make sure it has "mnc"
       --------------------------------------------------------------------------*/
      if((label_length != DS_PROFILE_MCC_MNC_LABEL_LENGTH) || 
         (strncasecmp((char*)encoded_apn+index+1,"mnc",DS_PROFILE_MCC_MNC_STR_LENGTH) != 0))
      {
        has_operator_identifier_part = FALSE;
        break;
      }

      index += label_length + 1;
      label_length = (uint32)(*(encoded_apn+index));
      /*-------------------------------------------------------------------------
       check length and make sure it has "mcc"
       --------------------------------------------------------------------------*/
      if((label_length != DS_PROFILE_MCC_MNC_LABEL_LENGTH) || 
        (strncasecmp((char*)encoded_apn+index+1,"mcc",DS_PROFILE_MCC_MNC_STR_LENGTH) != 0))
      {
        has_operator_identifier_part = FALSE;
        break;
      }

      index += label_length + 1;
      label_length = (uint32)(*(encoded_apn+index));
      /*-------------------------------------------------------------------------
       check length and make sure it has "gprs"
      --------------------------------------------------------------------------*/
      if((label_length != DS_PROFILE_GPRS_LABEL_LENGTH) || 
        (strncasecmp((char*)encoded_apn+index+1,"gprs",DS_PROFILE_GPRS_STR_LENGTH) != 0))
      {
        has_operator_identifier_part = FALSE;
        break;
      }
    }while (0); 
  }

  /*-------------------------------------------------------------------------
    check for FQDN format apn.epc.mnc<MNC>.mcc<MCC>.3gppnetwork.org.
    Skip check if format is already valid from non-FQDN check
   --------------------------------------------------------------------------*/
  if ((encoded_apn_length > DS_PROFILE_FQDN_OPERATOR_IDENTIFIER_TOTAL_LENGTH) && 
      (has_operator_identifier_part == FALSE))
  {
    do
    {
      has_fqdn_operator_identifier_part = TRUE;
      index = encoded_apn_length - DS_PROFILE_FQDN_OPERATOR_IDENTIFIER_TOTAL_LENGTH;
      label_length = (uint32)(*(encoded_apn+index));


      /*-------------------------------------------------------------------------
        check length and make sure it has "mnc"
       --------------------------------------------------------------------------*/
      if((label_length != DS_PROFILE_MCC_MNC_LABEL_LENGTH) || 
         (strncasecmp((char*)encoded_apn+index+1,"mnc",DS_PROFILE_MCC_MNC_STR_LENGTH) != 0))
      {
        has_fqdn_operator_identifier_part = FALSE;
        break;
      }

      index += label_length + 1;
      label_length = (uint32)(*(encoded_apn+index));

      /*-------------------------------------------------------------------------
       check length and make sure it has "mcc"
       --------------------------------------------------------------------------*/
      if((label_length != DS_PROFILE_MCC_MNC_LABEL_LENGTH) || 
        (strncasecmp((char*)encoded_apn+index+1,"mcc",DS_PROFILE_MCC_MNC_STR_LENGTH) != 0))
      {
        has_fqdn_operator_identifier_part = FALSE;
        break;
      }

      index += label_length + 1;
      label_length = (uint32)(*(encoded_apn+index));

      /*-------------------------------------------------------------------------
        check length and make sure it has "3gppnetwork"
       --------------------------------------------------------------------------*/
      if((label_length != DS_PROFILE_3GPPNETWORK_STR_LENGTH) || 
         (strncasecmp((char*)encoded_apn+index+1,"3gppnetwork",DS_PROFILE_3GPPNETWORK_STR_LENGTH) != 0))
      {
        has_fqdn_operator_identifier_part = FALSE;
        break;
      }

      index += label_length + 1;
      label_length = (uint32)(*(encoded_apn+index));

      /*-------------------------------------------------------------------------
        check length and make sure it has "org"
       --------------------------------------------------------------------------*/
      if((label_length != DS_PROFILE_ORG_STR_LENGTH) || 
         (strncasecmp((char*)encoded_apn+index+1,"org",DS_PROFILE_ORG_STR_LENGTH) != 0))
      {
        has_fqdn_operator_identifier_part = FALSE;
        break;
      }
    }while (0); 
  }

  if(has_operator_identifier_part == TRUE) 
  {
    encoded_apn_length -= DS_PROFILE_OPERATOR_IDENTIFIER_TOTAL_LENGTH;
  }
  else if (has_fqdn_operator_identifier_part == TRUE)
  {
    encoded_apn_length -= DS_PROFILE_FQDN_OPERATOR_IDENTIFIER_TOTAL_LENGTH;
  }
  
  label_length = 0;
  index = 0;

  while(index < encoded_apn_length)
  {
    /*----------------------------------------------------------------------
      Get the length of the label
      Increment the label count
      Increase the offset into the string to point to the next label length
      indicator
    ----------------------------------------------------------------------*/
    label_length = (uint32)(*(encoded_apn+index));

    if(label_length > 0)
    {
      label_count++;
    }
    /*--------------------------------------------------------------------------
      Always increment by label_length+1. This is to ensure that we get to the
      next label_length indicator
    --------------------------------------------------------------------------*/
    index += (label_length+1);

  }// while loop to traverse the encoded APN string

  DS_MSG1(MSG_LEGACY_HIGH,"Number of labels in APN Network identifier: %d",label_count);

  if(label_count < 1)
  {
    ret_val = FALSE;
  }
  else
  {
    /*--------------------------------------------------------------------------
      Reset the index to 1.
      Also reset the label_length to 0
    --------------------------------------------------------------------------*/
    index = 1;
    label_length = 0;

    /*------------------------------------------------------------------------
      Remember the following while copying from src to dest:

      Apn_string_offset is used to index into both buffers, but the offset
      in the src string is always one greater than dest
    ------------------------------------------------------------------------*/
    
    while(index <= label_count)
    {
      label_length = (uint32)(*(encoded_apn+apn_string_offset));

      if((apn_string_offset + label_length) < encoded_apn_length)
      {
        memscpy(decoded_apn+apn_string_offset, 
                label_length,
                encoded_apn+apn_string_offset+1,
                label_length);

        /*----------------------------------------------------------------------
          Increment the apn_string_offset by label_length
        ----------------------------------------------------------------------*/
        apn_string_offset += label_length;

        if(index == label_count)
        {
          /*--------------------------------------------------------------------
            Terminate the string with NULL
          --------------------------------------------------------------------*/
          *(decoded_apn+apn_string_offset) = 0x0;
        }
        else
        {
          /*--------------------------------------------------------------------
            Write the label delimitor in the decoded string
          --------------------------------------------------------------------*/
          *(decoded_apn+apn_string_offset) = LABEL_DELIMITOR;

          /*--------------------------------------------------------------------
            Increment the apn_string_offset by one.
          --------------------------------------------------------------------*/
          apn_string_offset++;
        }
        index++;
      }
      else
      {
        ret_val = FALSE;
        break;
      }

    } //while loop to extract labels corresponding to N/w Identifier
  } // label count greater than 3

  /*-----------------------------------------------------------------------------------------
   The APN Network Identifier shall contain at least one label and shall have, after encoding as 
   defined in subclause 9.1 above, a maximum length of 63 octets. An APN Network Identifier shall
   not start with any of the strings "rac", "lac", "sgsn" or "rnc", and it shall not end in ".gprs".
   ------------------------------------------------------------------------------------------*/
  
  if((has_fqdn_operator_identifier_part == TRUE)&&
     (apn_string_offset > DS_PROFILE_APN_EPC_STR_LENGTH_WITH_DELIM) &&
     (strncasecmp((const char *)(decoded_apn+(apn_string_offset-DS_PROFILE_APN_EPC_STR_LENGTH_WITH_DELIM)),
              ".apn.epc",DS_PROFILE_APN_EPC_STR_LENGTH_WITH_DELIM) == 0))
  {
    apn_string_offset -= DS_PROFILE_APN_EPC_STR_LENGTH_WITH_DELIM;
    decoded_apn[apn_string_offset] = 0;
  }

  if (ret_val && ds_pdn_profile_apn_is_network_identifier_valid(decoded_apn, apn_string_offset)) 
  {
    ret_val = TRUE; 
  }
  else
  {
    ret_val = FALSE;
  }
  
  return ret_val;
} /* ds_pdn_decode_apn_name */

/*===========================================================================
FUNCTION ds_pdn_decode_apn_name_non_strict

DESCRIPTION
  This function is used to decode the APN name returned by the network. This
  function leaves out the operator identifier. It picks up only the labels
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
  byte    *encoded_apn,
  uint32   encoded_apn_length,
  byte    *decoded_apn
)
{
  uint32  label_count = 0;
  uint32  label_length = 0;
  uint32  index = 0;
  boolean ret_val = TRUE;
  uint32  apn_string_offset = 0;
  uint32  temp_encoded_apn_length = encoded_apn_length;
  boolean op_id_found = FALSE;
  uint16  label_length_arr[20] = {0};
  uint16  temp_label_count = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*----------------------------------------------------------------------- 
    Store all the label lengths in an array, This is helpful to scan the
    string from the reverse and to find keywords like mcc.
    Also get the number of labels that exist
    -----------------------------------------------------------------------*/
  while(temp_encoded_apn_length > 0)
  {
    label_length = (uint16)(*(encoded_apn+index));

    if((label_count < 20) && (label_length > 0))
    {
      label_length_arr[label_count] = label_length;
      label_count++;
    }

    index += label_length + 1;

    if (temp_encoded_apn_length < (label_length + 1))
    {
      return FALSE;
    }
    else
    {
      temp_encoded_apn_length -= (label_length + 1);
    }
  }

  index = 0;
  temp_encoded_apn_length = encoded_apn_length;

  /*--------------------------------------------------------------------- 
    Now go to the end of the string and try to find if key word
    mnc  exists, If MNC exists then look for the match epc.apn.mnc.
   
    MNC should not be at the first level. If mNc is at the first level
    treat the whole string as network identifier
   
    If epc.apn.mnc is found all content before epc.apn.mnc is the
    Network ID
   
    If mcc is found all content before MNC is the Network ID
    ---------------------------------------------------------------------*/

  /*--------------------------------------------------------------------- 
    Reach the last label
  ---------------------------------------------------------------------*/
  do
  {
    label_length = *(encoded_apn+index);
    temp_label_count++;

    if(temp_label_count == label_count)
    {
      break;
    }

    index += label_length+1;

  }while(1);


  /*---------------------------------------------------------------------- 
    Now start traversing from the back of the label
    and examine the contents to see if MNC is found
    If MNC is found, exit out of the while loop
    ---------------------------------------------------------------------*/
  while(temp_label_count > 0)
  {
    label_length = *(encoded_apn+index);

    if((strncasecmp((char*)encoded_apn+index+1,
                    "mnc",DS_PROFILE_MCC_MNC_STR_LENGTH) == 0) &&
        temp_label_count != 1)
    {
      op_id_found =  TRUE;
      break;
    }
 
    if((temp_label_count-2) >= 0)
    {
      index = index - (label_length_arr[temp_label_count-2]) - 1;
    }
    temp_label_count--;
  }

  /*----------------------------------------------------------------------- 
    If no match occurs at this point then entire length is the
    Network identifier length.
   
    If a match occurs then check for epc.apn(reverse)
    -----------------------------------------------------------------------*/
  do
  {
    if(op_id_found)
    {
      temp_encoded_apn_length = index;
      /*-------------------------------------------------------------------- 
        Get the previous label and see if it is epc
      -------------------------------------------------------------------*/
      temp_label_count--;

      if((temp_label_count > 0) && ((temp_label_count-2) >= 0))
      {
        index = index - (label_length_arr[temp_label_count-2]) - 1;
      }
      else
      {
        break; /* Means not enough labels to scan*/
      }

      label_length = *(encoded_apn+index);
      /*-------------------------------------------------------------------------
         check for EPC
      --------------------------------------------------------------------------*/
      if((label_length != DS_PROFILE_EPC_STR_LENGTH) || 
         (strncasecmp((char*)encoded_apn+index+1,"epc",
                      DS_PROFILE_EPC_STR_LENGTH) != 0))
      {
        break;
      }

      temp_label_count--;
      /*-------------------------------------------------------------------- 
        Get the previous label and see if it is apn
      -------------------------------------------------------------------*/
      if((temp_label_count > 0) && (temp_label_count-2) >= 0)
      {
        index = index - (label_length_arr[temp_label_count-2]) - 1;
      }
      else
      {
        break; /* Means not enough labels to scan*/
      }

      label_length = *(encoded_apn+index);
    /*-------------------------------------------------------------------------
       check for apn
     --------------------------------------------------------------------------*/
      if((label_length != DS_PROFILE_APN_STR_LENGTH) || 
         (strncasecmp((char*)encoded_apn+index+1,"apn",
                      DS_PROFILE_APN_STR_LENGTH) != 0))
      {
        break;
      }


     /*-------------------------------------------------------------------- 
       If we reach this point epc.apn match is successful
       Now set the temp_encoded_apn_length to the index value
      --------------------------------------------------------------------*/
      temp_encoded_apn_length = index;
    }
  }while(0);
  

  label_length = 0;
  index = 0;
  label_count = 0;

  while(index < temp_encoded_apn_length)
  {
    /*----------------------------------------------------------------------
      Get the length of the label
      Increment the label count
      Increase the offset into the string to point to the next label length
      indicator
    ----------------------------------------------------------------------*/
    label_length = (uint32)(*(encoded_apn+index));

    if(label_length > 0)
    {
      label_count++;
    }
    /*--------------------------------------------------------------------------
      Always increment by label_length+1. This is to ensure that we get to the
      next label_length indicator
    --------------------------------------------------------------------------*/
    index += (label_length+1);

  }// while loop to traverse the encoded APN string

  DS_MSG1(MSG_LEGACY_HIGH,"Number of labels in APN Network identifier: %d",
  	       label_count);

  if(label_count < 1)
  {
    ret_val = FALSE;
  }
  else
  {

    /*--------------------------------------------------------------------------
      Reset the index to 1.
      Also reset the label_length to 0
    --------------------------------------------------------------------------*/
    index = 1;
    label_length = 0;

    /*------------------------------------------------------------------------
      Remember the following while copying from src to dest:

      Apn_string_offset is used to index into both buffers, but the offset
      in the src string is always one greater than dest
    ------------------------------------------------------------------------*/
    
    while(index <= label_count)
    {
      label_length = (uint32)(*(encoded_apn+apn_string_offset));

      if((apn_string_offset + label_length) < temp_encoded_apn_length)
      {
        memscpy(decoded_apn+apn_string_offset, 
                label_length,
                encoded_apn+apn_string_offset+1,
                label_length);

        /*----------------------------------------------------------------------
          Increment the apn_string_offset by label_length
        ----------------------------------------------------------------------*/
        apn_string_offset += label_length;

        if(index == label_count)
        {
          /*--------------------------------------------------------------------
            Terminate the string with NULL
          --------------------------------------------------------------------*/
          *(decoded_apn+apn_string_offset) = 0x0;
        }
        else
        {
          /*--------------------------------------------------------------------
            Write the label delimitor in the decoded string
          --------------------------------------------------------------------*/
          *(decoded_apn+apn_string_offset) = LABEL_DELIMITOR;

          /*--------------------------------------------------------------------
            Increment the apn_string_offset by one.
          --------------------------------------------------------------------*/
          apn_string_offset++;
        }
        index++;
      }
      else
      {
        ret_val = FALSE;
        break;
      }
    } //while loop to extract labels corresponding to N/w Identifier
  } // label count greater than 3

  DS_MSG_SPRINTF_1(MSG_LEGACY_HIGH,"Decoded APN name: %s",decoded_apn);
  return ret_val;
} /* ds_pdn_decode_apn_name */

/*===========================================================================
FUNCTION ds_pdn_profile_validator_compare_forbidden_apn

DESCRIPTION
  This function compares the APN name passed with the APN name present in
  a forbidden APN info block

PARAMETERS 
  item_ptr: Element from forbidden APN list queue 
  target_val(apn_name): APN to be checked if its forbidden or not 

DEPENDENCIES
  None.

RETURN VALUE
  TRUE: If the forbidden APN name matches the APN name present in the queue block
  FALSE: Otherwise

SIDE EFFECTS
  None.
===========================================================================*/
int ds_pdn_profile_validator_compare_forbidden_apn
(
  void  *item_ptr,
  void  *target_val
)
{
  char                                            *apn_name = NULL;
  int                                              ret_val = FALSE;
  ds_profile_validator_forbidden_apn_info_type  *forbidden_apn_info_blk_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( NULL == item_ptr || NULL == target_val )
  {
    return ret_val;
  }

  forbidden_apn_info_blk_ptr = 
                   (ds_profile_validator_forbidden_apn_info_type *)item_ptr;
  apn_name = (char *)target_val;

  if(strlen(apn_name) == 
               strlen((const char*)forbidden_apn_info_blk_ptr->forbidden_apn))
  {
    if(strncasecmp((const char *)apn_name, 
                   (const char *)forbidden_apn_info_blk_ptr->forbidden_apn,
                   DS_PROFILE_APN_STRING_LEN) == 0)
    {
      ret_val = TRUE;
    }
  }

  return ret_val;
} /* ds_pdn_profile_validator_compare_forbidden_apn */

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
)
{
  boolean              ret_val = FALSE;
  sys_sys_mode_e_type  network_mode = SYS_SYS_MODE_NONE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  do
  {
    network_mode = ds_pdn_hdlr_get_current_network_mode();
    if ( (SYS_SYS_MODE_LTE == network_mode) && 
         (DS_EPS_ATTACHED != ds_eps_pdn_attach_sm_get_state()) )
    {
      break;
    }

    /*------------------------------------------------------------------------ 
      If we have reached this stage, It is an additional PDN
      Check if the apn in the PDN context is NULL, If NULL apn return TRUE
     ------------------------------------------------------------------------*/
    if(NULL != q_linear_search(&ds_profile_validator_forbidden_apn_q,
                               ds_pdn_profile_validator_compare_forbidden_apn,
                               (void *)pdn_mgr_p->pdp_profile->context.apn))
    {
      ret_val = TRUE;
    }
  }while(0);

  return ret_val;
} /* ds_pdn_profile_validator_is_apn_forbidden */

/*===========================================================================
FUNCTION ds_pdn_profile_validator_free_forbidden_apn_info_q

DESCRIPTION
  This function frees the existing APN info queue before saving new contents

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Empties Forbidden APN Info Queue
===========================================================================*/
void ds_pdn_profile_validator_free_forbidden_apn_info_q
(
  void
)
{
  ds_profile_validator_forbidden_apn_info_type   *extracted_q_block_ptr = NULL;
  ds_profile_validator_forbidden_apn_info_type   *prev_q_block_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  extracted_q_block_ptr = (ds_profile_validator_forbidden_apn_info_type *)
          q_check(&ds_profile_validator_forbidden_apn_q);

  while(NULL != extracted_q_block_ptr)
  {
    prev_q_block_ptr = extracted_q_block_ptr;
    extracted_q_block_ptr = q_next(
                             &ds_profile_validator_forbidden_apn_q,
                             &(extracted_q_block_ptr->link));

    q_delete(&ds_profile_validator_forbidden_apn_q,
             &(prev_q_block_ptr->link));
    DS_SYSTEM_HEAP_MEM_FREE(prev_q_block_ptr);
    prev_q_block_ptr = NULL;
  }

  return;
} /* ds_pdn_profile_validator_free_forbidden_apn_info_q */

/*===========================================================================
FUNCTION ds_pdn_profile_validator_copy_forbidden_apn_info

DESCRIPTION
  This function copies the APN info extracted from EFS file into a local
  queue

PARAMETERS
  efs_extracted_info_q_ptr: Queue containing the extracted EFS info

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_pdn_profile_validator_copy_forbidden_apn_info
(
  q_type                  *efs_extracted_info_q_ptr
)
{
  ds_profile_validator_extracted_info_type   *efs_extracted_info_block_ptr = NULL;
  ds_profile_validator_extracted_info_type   *prev_info_block_ptr = NULL;
  ds_profile_validator_forbidden_apn_info_type  
                                       *forbidden_apn_info_block_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_pdn_profile_validator_free_forbidden_apn_info_q();

  efs_extracted_info_block_ptr = (ds_profile_validator_extracted_info_type *)
                                              q_check(efs_extracted_info_q_ptr);

  while(NULL != efs_extracted_info_block_ptr)
  {
    DS_SYSTEM_HEAP_MEM_ALLOC(forbidden_apn_info_block_ptr,
		                     sizeof(ds_profile_validator_forbidden_apn_info_type),
		                     ds_profile_validator_forbidden_apn_info_type *);

    if(NULL != forbidden_apn_info_block_ptr)
    {
      memset(forbidden_apn_info_block_ptr, 
              0,
              sizeof(ds_profile_validator_forbidden_apn_info_type));

      strlcpy(forbidden_apn_info_block_ptr->forbidden_apn, 
              efs_extracted_info_block_ptr->extracted_data,
              efs_extracted_info_block_ptr->extracted_data_length+1);

      q_link(forbidden_apn_info_block_ptr , 
             &(forbidden_apn_info_block_ptr->link));
      q_put(&ds_profile_validator_forbidden_apn_q, 
            &(forbidden_apn_info_block_ptr->link));
    }

    prev_info_block_ptr = efs_extracted_info_block_ptr;

    efs_extracted_info_block_ptr = (ds_profile_validator_extracted_info_type *)
      q_next(efs_extracted_info_q_ptr, &(efs_extracted_info_block_ptr->link));

    q_delete(efs_extracted_info_q_ptr, &(prev_info_block_ptr->link));

    DS_SYSTEM_HEAP_MEM_FREE(prev_info_block_ptr->extracted_data);
    DS_SYSTEM_HEAP_MEM_FREE(prev_info_block_ptr);
    prev_info_block_ptr = NULL;
  }

  return;
} /* ds_pdn_profile_validator_copy_forbidden_apn_info */

/*===========================================================================
FUNCTION      ds_pdn_profile_validator_fill_extracted_info

DESCRIPTION   This function fills the EFS extracted info into the queue passed 
              by the client calling function 

PARAMETERS    char *from - start of the token (param name)
              char *to   - end of the token (param name)
              extracted_info_q_ptr - Queue that will contain extracted EFS info
              subs_id - Current active PS subscription

DEPENDENCIES  None

RETURN VALUE  0  - success
              -1 - failure

SIDE EFFECTS  None
===========================================================================*/
int16 ds_pdn_profile_validator_fill_extracted_info
(
  char         *from,
  char         *to,
  q_type       *extracted_info_q_ptr
)
{
  int16 ret_val = -1;
  uint8 length = 0;
  ds_profile_validator_extracted_info_type      *extracted_info_block_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(NULL == from ||
     NULL == to ||
     NULL == extracted_info_q_ptr ||
     from > to)
  {
    return ret_val;
  }

  length = (uint8)(to - from);

  /*----------------------------------------------------------------------- 
    This memory will be freed by the calling client function who calls the util
    to extract EFS info
    -----------------------------------------------------------------------*/
  DS_SYSTEM_HEAP_MEM_ALLOC(extracted_info_block_ptr ,
                           sizeof(ds_profile_validator_extracted_info_type),
                           ds_profile_validator_extracted_info_type *);

  if(NULL != extracted_info_block_ptr)
  {
    memset(extracted_info_block_ptr, 0, sizeof(ds_profile_validator_extracted_info_type));
    DS_SYSTEM_HEAP_MEM_ALLOC(extracted_info_block_ptr->extracted_data,
		                      sizeof(char)*(length+1), char *);
    if(NULL != extracted_info_block_ptr->extracted_data)
    {
      memset(extracted_info_block_ptr->extracted_data, 0, length+1);
      strlcpy((char *)extracted_info_block_ptr->extracted_data, from, length+1);

      extracted_info_block_ptr->extracted_data_length = 
                            strlen(extracted_info_block_ptr->extracted_data);

      q_link(extracted_info_block_ptr, &(extracted_info_block_ptr->link));
      q_put(extracted_info_q_ptr, &(extracted_info_block_ptr->link));
      ret_val = 0;
    }
    else
    {
      DS_SYSTEM_HEAP_MEM_FREE(extracted_info_block_ptr);
      extracted_info_block_ptr = NULL;
    }
  }
  
  return ret_val;
}

/*===========================================================================
FUNCTION      ds_pdn_profile_validator_verify_apn_token_id

DESCRIPTION   This function verifies the token in the EFS file with the passed 
              comparison string 

PARAMETERS    char *from - start of the token (param name)
              char *to   - end of the token (param name)
              char *comp_string - string against which token id needs to be
                                  compared

DEPENDENCIES  None

RETURN VALUE  0  - success
              -1 - failure

SIDE EFFECTS  None
===========================================================================*/
int16 ds_pdn_profile_validator_verify_apn_token_id
(
  char *from,
  char *to,
  char *comp_string
)
{
  int16 ret_val = -1; /* return value */
  uint8 length = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(NULL == from ||
     NULL == to ||
     NULL == comp_string ||
     from > to)
  {
    return ret_val;
  }

  /* Assumption that the token length will not be greater than 255 */
  length =(uint8)(to-from);

  if(strncasecmp(from,comp_string,length) != 0)
  {
    return ret_val;
  }
  
  ret_val = 0;
  return ret_val;

} /* ds_pdn_profile_validator_verify_apn_token_id() */

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
)
{

  ds_pdn_nv_efs_token_type  efs_db; /* structure used for tokenizing the file*/
  char *from, *to; /* ptrs to start and end of the token */
  ds_pdn_nv_efs_token_parse_status_type  ret_val 
                                              = DS_PDN_NV_EFS_TOKEN_PARSE_SUCCESS;
  boolean param_name = TRUE; /* flag to keep track of the parser state 
                                TRUE - parser expects Token Name in the EFS
                                FALSE - parser expects Token Val in the EFS*/
  boolean return_val = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(NULL == comparison_tag ||
     NULL == efs_file_name)
  {
    return ret_val;
  }

  if(ds_pdn_nv_manager_efs_file_init(efs_file_name, &efs_db) == -1)
  {
    return return_val;
  }

  /* Set the seperator as : */
  efs_db.seperator = ':';
/*lint -save -esym(644,token_id) param_name boolean flag takes care of this */
  while (DS_PDN_NV_EFS_TOKEN_PARSE_EOF 
          != (ret_val = ds_pdn_nv_manager_efs_tokenizer(&efs_db, &from, &to)))
  {
    if (DS_PDN_NV_EFS_TOKEN_PARSE_FAILURE == ret_val)
    {
      DS_ERROR_LOG_0("Token Parse Failure");
      break;
    }
    /*------------------------------------------------------------------------
      Token being read. 'from' points to the beginning of the token and 
      'to' point to the end of the token.

      The tokenizer automatically skips blank lines and comments (lines 
      beginning with #, so no need to check for them here).
    ------------------------------------------------------------------------*/
    if((from == to) || (DS_PDN_NV_EFS_TOKEN_PARSE_EOL == ret_val))
    {
      /*----------------------------------------------------------------------
        Skip empty tokens.
      ----------------------------------------------------------------------*/
      continue;
    }     
    else if(DS_PDN_NV_EFS_TOKEN_PARSE_SUCCESS == ret_val)
    {
      /*--------------------------------------------------------------------- 
      check if we are looking for param name or param value 
      ---------------------------------------------------------------------*/
      if (param_name == TRUE)
      {
        /*------------------------------------------------------------------
        get the token identifier for this param name 
        ------------------------------------------------------------------*/
        if (ds_pdn_profile_validator_verify_apn_token_id(from, to, 
                                               comparison_tag) < 0)
        {
          return return_val;
        }
        else
        {
          /*-----------------------------------------------------------------
          set param_name as FALSE This means the next token is a 
          param value
          -----------------------------------------------------------------*/
          param_name = FALSE;
          /* set the seperator as ; */
          efs_db.seperator = ';';
        }
        
      }
      /*---------------------------------------------------------------------
      This means that the token is a param value
      ---------------------------------------------------------------------*/
      else 
      {
        /*-------------------------------------------------------------------
         pass in the identifier and param value to fill the APN reject SM
         config structure
         ------------------------------------------------------------------*/
        if(ds_pdn_profile_validator_fill_extracted_info(from, to, efs_extracted_info_q) < 0)
        {
          return return_val;
        }
        /*-------------------------------------------------------------------
         set param_name as TRUE This means that next token is a param name
        -------------------------------------------------------------------*/
        param_name = TRUE;
        /* Set the seperator as : */
        efs_db.seperator = ':';
      }
    }
  } /* end of while loop */

  return_val = TRUE;
/*lint -restore (644,token_id)*/
  ds_pdn_nv_manager_efs_file_close(&efs_db);
  return return_val;

} /* ds_pdn_profile_validator_read_apn_info_from_efs() */

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTXT_READ_FORBIDDEN_APN_LIST_FROM_EFS

DESCRIPTION
  This function invokes the utility to read forbidden APN list from EFS
  and then copies the info into its local structure

PARAMETERS
  None.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_profile_validator_read_forbidden_apn_list_from_efs
(
  void
)
{
   q_type  efs_extracted_info_q;
   char    efs_file_name[] = "/data/forbidden_apn_list.txt";
   char    compare_tag[] = "forbidden_apn";
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   memset(&efs_extracted_info_q, 0, sizeof(q_type));
   q_init(&efs_extracted_info_q);

   if(FALSE == ds_pdn_profile_validator_read_apn_info_from_efs((char *)compare_tag,
                                                     (char *)efs_file_name,
                                                     &efs_extracted_info_q))
   {
     q_destroy(&efs_extracted_info_q);
     return;
   }

   ds_pdn_profile_validator_copy_forbidden_apn_info(&efs_extracted_info_q);
   q_destroy(&efs_extracted_info_q);

   return;
} /* ds_pdn_profile_validator_read_forbidden_apn_list_from_efs */

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
void ds_pdn_profile_validator_powerup_init( boolean is_init)
{
 
  /* initialize the forbidden apn queue at powerup itself as NV refresh may happen 
        before queue gets initialized later */

  if(is_init)
  {
    memset(&ds_profile_validator_forbidden_apn_q, 0, sizeof(q_type));
    q_init(&ds_profile_validator_forbidden_apn_q);
  }
  else
  {
    ds_pdn_profile_validator_free_forbidden_apn_info_q();
  }
  /*-----------------------------------------------------------------------
    Read forbidden APN List info from EFS: /data/forbidden_apn_list.txt      
  -----------------------------------------------------------------------*/
  ds_pdn_profile_validator_read_forbidden_apn_list_from_efs();

  return;
}


