/*===========================================================================
                      DS_PDN_APM_DSC.C
DESCRIPTION
This file defines APM DSC command APIs.

Copyright (c) 2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/3gpp/pdnmgr/src/ds_pdn_apm_dsc.c#1 $
  $DateTime: 2020/03/17 08:03:16 $$Author: pwbldsvc $

when           who    what, where, why
--------    ---    ----------------------------------------------------------
08/09/17    sd      First checkin
===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "ds_pdn_defs.h"
#include "ds_pdn_apm_dsc.h"
#include "ds_pdn_apm.h"
#include "ran.h"

/*===========================================================================

                          GLOBAL VARIABLES

===========================================================================*/
#define DS_PDN_APM_DSC_INVALID_PLMN_BLOCKING_INTERVAL       0

/*===========================================================================

                          INTERNAL FUNCTIONS

===========================================================================*/

/*===========================================================================
FUNCTION      ds_pdn_apm_dsc_command

DESCRIPTION   This is a DSC command interface to DSC module. 
 
DEPENDENCIES  None.

RETURN VALUE  

SIDE EFFECTS  Commands CM module
===========================================================================*/
void ds_pdn_apm_dsc_command
(
  ds_pdn_apm_dsc_command_type          command,
  uint32                           blocking_interval_value
)
{
  cm_client_id_type                client_id = 1; 
  cm_plmn_blocking_info_s_type     plmn_blocking_info;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DS_MSG1(MSG_LEGACY_HIGH,"ds_pdn_apm_dsc_command cmd:%d",command);
  memset (&plmn_blocking_info, 0, sizeof(cm_plmn_blocking_info_s_type));

  if (blocking_interval_value != DS_PDN_APM_DSC_INVALID_PLMN_BLOCKING_INTERVAL)
  {
    blocking_interval_value += (ran_dist(ran_next(), 1, 15) * 1000);
    plmn_blocking_info.blocking_interval = blocking_interval_value;

    DS_MSG1(MSG_LEGACY_HIGH, "PLMN Blocking Interval is %d",
            blocking_interval_value);
  }

  switch(command)
  {
    case DS_DSC_PLMN_BLOCKING:
      plmn_blocking_info.request_pref 
        = CM_PLMN_BLOCKING_PREF_PLMN_BLOCKING_ONLY;
      //client_id = ds3g_get_cm_client_id();
      if ( FALSE == ds_pdn_apm_get_current_plmn_id(&(plmn_blocking_info.plmn)))
      {
        return;
      }

      cm_ph_cmd_detach_and_plmn_blocking(NULL,
                                         NULL,
                                         client_id,
                                         plmn_blocking_info);
      break;

    case DS_DSC_PS_DETACH:
      plmn_blocking_info.request_pref = CM_PLMN_BLOCKING_PREF_LTE_DETACH_ONLY;
      //client_id = ds3g_get_cm_client_id();
      cm_ph_cmd_detach_and_plmn_blocking(NULL,
                                         NULL,
                                         client_id,
                                         plmn_blocking_info);
      break;

    case DS_DSC_PLMN_BLOCKING_AND_PS_DETACH:
      plmn_blocking_info.request_pref 
        = CM_PLMN_BLOCKING_PREF_LTE_DETACH_PLMN_BLOCKING;
      //client_id = ds3g_get_cm_client_id();
      if ( FALSE == ds_pdn_apm_get_current_plmn_id(&(plmn_blocking_info.plmn)) )
      {
        return;
      }

      cm_ph_cmd_detach_and_plmn_blocking(NULL,
                                         NULL,
                                         client_id,
                                         plmn_blocking_info);
      break;

    default:
      break;
  }
  return;
}/* ds_pdn_apm_dsc_command()*/

