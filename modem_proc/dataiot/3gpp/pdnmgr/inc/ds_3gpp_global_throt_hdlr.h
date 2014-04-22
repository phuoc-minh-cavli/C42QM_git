#ifndef DS_3GPP_GLOBAL_THROT_H
#define DS_3GPP_GLOBAL_THROT_H

/*===========================================================================
                      DS_PDN_GLOBAL_THROT_HDLR.H
DESCRIPTION
This file declares header for PDN global throttling handler APIs.

Copyright (c) 2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/3gpp/pdnmgr/inc/ds_3gpp_global_throt_hdlr.h#1 $
  $DateTime: 2020/03/17 08:03:16 $$Author: pwbldsvc $

when           who    what, where, why
--------    ---    ----------------------------------------------------------
08/09/17    sd      First checkin
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "ds_pdn_defs.h"
#include "ds_3gpp_pdn_throttle_sm.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*===========================================================================
FUNCTION  DS_3GPP_GLOBAL_THROT_INIT      

DESCRIPTION
  This function performs initialization of the Global Throttling module

PARAMETERS
  None.
 
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_global_throt_init
(
  void
);

/*===========================================================================
FUNCTION  DS_3GPP_GLOBAL_THROT_POWER_UP_INIT      

DESCRIPTION
  This function performs power up initialization of the Global
  Throttling module

PARAMETERS
  None.
 
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_global_throt_power_up_init
(
  void 
);

/*===========================================================================
FUNCTION DS_3GPP_GLOBAL_THROT_T3396_ABSENT_OR_INACTIVE

DESCRIPTION
  This function performs infinite throttling and is called when no T3396
  timer value is obatined in PDN rej/ PDP deactivation
  

PARAMETERS   : 
  
 rat_based_pdn_throt_p        --- Pdn throttle_ptr
 throt_ident_info_p           --- Throt Identification pointer
)
DEPENDENCIES
  None.

RETURN VALUE
  None.
 
SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_global_throt_t3396_absent_or_inactive
(
  ds_3gpp_pdn_throt_rat_based_s      *rat_based_pdn_throt_p,
  ds_3gpp_pdn_throt_ident_info_type  *throt_ident_info_p
);

/*===========================================================================
FUNCTION      DS_3GPP_GLOBAL_THROT_INFINITE_BLOCK_REQUIRED

DESCRIPTION 
  This function checks whether infinite blocking is required or not based on
  the down reason passed to it

PARAMETERS 
  down_reason - PS Net Down Reason

RETURN VALUE 
  TRUE,  if inifinte blocking is required
  FALSE, otherwise

SIDE EFFECTS 
  None
===========================================================================*/
boolean ds_3gpp_global_throt_infinite_block_required
( 
  ds_net_down_reason_type   down_reason
);

#endif /* DS_3GPP_GLOBAL_THROT_H */
