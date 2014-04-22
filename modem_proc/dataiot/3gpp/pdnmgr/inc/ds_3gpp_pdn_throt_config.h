#ifndef DS_PDN_THROT_CONFIG_H
#define DS_PDN_THROT_CONFIG_H

/*===========================================================================
                      DS_PDN_THROT_CONFIG.H
DESCRIPTION
This file exposes PDN throttling config APIs and data structures.

Copyright (c) 2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/3gpp/pdnmgr/inc/ds_3gpp_pdn_throt_config.h#2 $
  $DateTime: 2022/08/16 22:48:44 $$Author: pwbldsvc $

when           who    what, where, why
--------    ---    ----------------------------------------------------------
08/09/17    sd      First checkin
===========================================================================*/


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/


#include "ds_pdn_defs.h"
#include "ds_pdn_manager.h"

#define DS_3GPP_PDN_THROTTLE_MAX_FAILURE_TIMERS 10  /* max number of pdn failure timers */
#define DS_3GPP_PDN_THROTTLE_MIN_FAILURE_TIMERS  1 /* min number of failure timers */
#define DS_3GPP_PDN_THROTTLE_REF_FAILURE_TIMERS  6 /* ref cnt to fill from 6-10 failure timers */

/*===========================================================================
FUNCTION      DS_3GPP_PDN_THROT_GET_FAILURE_TIMER

DESCRIPTION   This function provides the pdn throttling failure timer value

PARAMETERS    counter - Throttling counter used as an index 
             
RETURN VALUE  Throttle Failure Timer value in msec

SIDE EFFECTS  None
===========================================================================*/
uint32 ds_3gpp_pdn_throt_get_failure_timer 
(
   uint8                  counter
);

/*===========================================================================
FUNCTION      DS_3GPP_PDN_THROT_READ_CONFIG_FROM_EFS

DESCRIPTION   This function will read from the efs file all the necessary 
              data & fill the PDN throttle config structure.
              Only valid data is populated.

              EFS File Format - Param_Name:Param_Val;
              For example     - FAILURE_TIMER_1:0;
 
PARAMATERS   None

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_3gpp_pdn_throt_read_config_from_efs
(
  void
);

/*===========================================================================
FUNCTION      DS_3GPP_PDN_THROT_GET_REMAINING_TIMER_FOR_ATTACH_PDN

DESCRIPTION   This function will return the remaining throttling timer for
              attach PDN

PARAMATERS    None

DEPENDENCIES  None

RETURN VALUE  timer value

SIDE EFFECTS  None
===========================================================================*/
uint32 ds_3gpp_pdn_throt_get_remaining_timer_for_attach_pdn();

#endif /* DS_PDN_THROT_CONFIG_H */