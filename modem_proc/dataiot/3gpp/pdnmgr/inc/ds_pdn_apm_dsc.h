#ifndef DS_APM_DSC_H
#define DS_APM_DSC_H

/*===========================================================================
                      DS_PDN_APM_DSC.H
DESCRIPTION
This file exposes APM DSC commands through APIs and data structures.

Copyright (c) 2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/3gpp/pdnmgr/inc/ds_pdn_apm_dsc.h#1 $
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

                          PUBLIC DATA DECLARATIONS

===========================================================================*/
typedef enum
{
  DS_DSC_COMMAND_INVALID              = 0,
  DS_DSC_PLMN_BLOCKING                = 1,
  DS_DSC_PS_DETACH                    = 2,
  DS_DSC_PLMN_BLOCKING_AND_PS_DETACH  = 3,
  DS_DSC_COMMAND_MAX = DS_DSC_PLMN_BLOCKING_AND_PS_DETACH
}ds_pdn_apm_dsc_command_type;


/*===========================================================================
FUNCTION      ds_pdn_apm_dsc_command

DESCRIPTION   This is a DSC command interface
 
DEPENDENCIES  None.

RETURN VALUE  

SIDE EFFECTS  
===========================================================================*/
void ds_pdn_apm_dsc_command
(
  ds_pdn_apm_dsc_command_type         command,
  uint32                              blocking_interval_value
);

#endif /* DS_APM_DSC_H */
