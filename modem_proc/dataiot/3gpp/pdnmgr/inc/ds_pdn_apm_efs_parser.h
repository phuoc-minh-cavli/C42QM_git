#ifndef DS_PDN_APM_EFS_PARSER_H
#define DS_PDN_APM_EFS_PARSER_H

/*===========================================================================
                      DS_PDN_APM_EFS_PARSER.H
DESCRIPTION
This file exposes APM EFS parser through APIs and data structures.

Copyright (c) 2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/3gpp/pdnmgr/inc/ds_pdn_apm_efs_parser.h#1 $
  $DateTime: 2020/03/17 08:03:16 $$Author: pwbldsvc $

when           who    what, where, why
--------    ---    ----------------------------------------------------------
08/09/17    sd      First checkin
===========================================================================*/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/
#include "ds_pdn_defs.h"
#include "ds_pdn_apm.h"
#include "lte.h"
#include "queue.h"


#define DS_PDN_APM_MAX_PDN_NUM                      2
#define DS_PDN_APM_RULE_MAX_PARAMS_NUM              10
#define DS_PDN_APM_RULE_MAX_PARAM_NAME_SIZE         42
#define DS_PDN_APM_ATTACH_PROF_MAX_PARAM_NAME_SIZE  47
#define DS_PDN_APM_ATTACH_PROFILES_MAX              2

#define DS_PDN_APM_ROOT_DIR                    "/data"
#define DS_PDN_APM_RULES_FILE               "/data/ds_dsd_apm_rules.txt"
#define DS_PDN_APM_ATTACH_PROFILE_FILE      "/data/ds_dsd_attach_profile.txt"
#define DS_PDN_APM_TMP_ATTACH_PROFILE_FILE  "/data/ds_dsd_attach_profile.tmp"

#define DS_PDN_APM_PROFILE_MAX_BUFF_SIZE       1024

/*-----------------------------------------------------------------------------
      Attach Profile Status 
------------------------------------------------------------------------------*/
#define ATTACH_PROF_READ_FAIL              0x0000
#define ATTACH_PROF_READ_SUCCESS           0x0001
#define ATTACH_PROF_CHANGED                0x0002

/*===========================================================================
FUNCTION      ds_pdn_apm_read_pdn_rule_list_from_efs

DESCRIPTION   This function will read PDN rules from EFS

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
boolean ds_pdn_apm_read_pdn_rule_list_from_efs
(
  q_type                    *attach_prof_q,
  q_type                    *q
);


/*===========================================================================
FUNCTION      ds_pdn_apm_read_attach_profile_list_from_efs

DESCRIPTION   This function will read attach profiles from EFS

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
boolean ds_pdn_apm_read_attach_profile_list_from_efs
(
  q_type                    *q
);

/*===========================================================================
FUNCTION      DSD_APM_READ_DEFAULT_ATTACH_PROF_FILE

DESCRIPTION   This function will read attach profiles from the PDP registry

PARAMETERS    
              attach_prof_q:               Queue that contains the attach
                                           profiles

DEPENDENCIES  None

RETURN VALUE  TRUE: If we were able to obtain default attach prof 
              FALSE: Otherwise 

SIDE EFFECTS  Modification of global attach profile queue of APM
===========================================================================*/
boolean ds_pdn_apm_read_default_attach_prof 
(
  q_type                                *attach_prof_q
);

/*===========================================================================
FUNCTION       ds_pdn_apm_gen_apm_rule_string

DESCRIPTION    Creates apm rule string from apm_rule_q

DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
boolean ds_pdn_apm_gen_apm_rule_string
(
  q_type                       *rule_q,
  char                         *str,
  uint16                        buffer_size
);


/*===========================================================================
FUNCTION      ds_pdn_apm_update_apn_rule_efs_file

DESCRIPTION   This function will overwrite the EFS file with string passed

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
boolean ds_pdn_apm_update_apn_rule_efs_file
(
  q_type                    *q, 
  char*                     str
);

/*===========================================================================
FUNCTION      ds_pdn_apm_translate_ipreq_to_char

DESCRIPTION   This is a utility to translate IP Required enum type to char

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
char  ds_pdn_apm_translate_ipreq_to_char
(
  ipRequiredType ip_req
);

/*===========================================================================
FUNCTION      ds_pdn_apm_efs_overwrite_attach_pdn_list

DESCRIPTION   This function overwrite the attach PDN list in EFS with input 
              attach PDN profile ID array 

DEPENDENCIES  None.

RETURN VALUE  TRUE: Operation succeeds 
              FALSE: Operation fails 

===========================================================================*/
boolean ds_pdn_apm_efs_overwrite_attach_pdn_list
(
  uint16                    *profile_id_arr,
  uint8                      num_of_pdn
);

/*===========================================================================
FUNCTION      DSD_ATOI

DESCRIPTION    Converts the character input to integer type

PARAMETERS    x - Begin pointer (char *)
              y - End pointer (char *)

DEPENDENCIES  None

RETURN VALUE  ouput integer

SIDE EFFECTS  None
===========================================================================*/
int ds_pdn_apm_atoi
(
  char* x, char* y
);

#endif /* DS_PDN_APM_EFS_PARSER_H*/
