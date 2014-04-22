#ifndef DS_3GPP_DAM_H
#define DS_3GPP_DAM_H

/*===========================================================================
                      DS_3GPP_DATA_RETRY.H
DESCRIPTION
This file declares headers for data retry throttling APIs.

Copyright (c) 2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/3gpp/pdnmgr/inc/ds_3gpp_dam.h#1 $
  $DateTime: 2020/03/17 08:03:16 $$Author: pwbldsvc $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/09/17    sd     First checkin
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "ds_pdn_defs.h"
#include "ds_3gpp_pdn_throttle_sm.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/
/*---------------------------------------------------------------------------- 
  This structure is used to update the apn from EFS and on receiving a
  PDN connectivity Reject, a decision is made to set block_pdn to TRUE based
  on the cause code received.
  ----------------------------------------------------------------------------*/
typedef struct
{
  boolean                block_pdn;
  byte                  *apn;
  ps_timer_handle_type   apn_reject_sm_timer;
  boolean                is_initialized;
}ds_3gpp_dam_info_s;


/*===========================================================================
FUNCTION      DS_3GPP_DAM_IS_APN_REJECT_SM_THROTTLED

DESCRIPTION
  This function checks to see if apn_reject_sm for the particular PDN
  is blocked.
 
PARAMETERS

DEPENDENCIES

RETURN VALUE
  True -  If the apn_reject_sm is blocked
  False - Otherwise
 
SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_3gpp_dam_is_apn_reject_sm_throttled
(
  ds_pdn_mgr_s     *pdn_cntx_p  
);

/*===========================================================================
FUNCTION  DS_3GPP_DAM_SET_PDN_BLOCKED

DESCRIPTION
  This function sets PDN _BLOCKED in the apn_reject sm

PARAMETERS
   void              
 
DEPENDENCIES
  None.

RETURN VALUE
   True - If SET is successful
   False - Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_3gpp_dam_set_pdn_blocked
(      
  boolean                 set_val
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_THROT_APN_REJ_SM_INIT

DESCRIPTION
  This function initialize the apn reject sm

PARAMETERS

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void ds_3gpp_dam_init
(
  void
);

/*===========================================================================
FUNCTION  DS_3GPP_DAM_T3402_TIMER_EXPIRY_CMD_HDLR

DESCRIPTION
  This function resets the sm on timer expiry

PARAMETERS
   void            
 
DEPENDENCIES
  None.

RETURN VALUE 
  void 

SIDE EFFECTS
  None.

===========================================================================*/

void ds_3gpp_dam_t3402_timer_expiry_cmd_hdlr
(
  void
);

/*===========================================================================
FUNCTION  DS_3GPP_DAM_CLR_TIMER

DESCRIPTION
  This function clears the t3402 timer 

PARAMETERS
   void            
 
DEPENDENCIES
  None.

RETURN VALUE 
  void 

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_dam_clr_timer
(
  void
);


/*===========================================================================
FUNCTION  DS_3GPP_DAM_PDN_BLOCKED_STATUS

DESCRIPTION
  This function gets the PDN blocked flag from Reject SM

PARAMETERS
  void           
 
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If the flag is set
  FALSE - Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_3gpp_dam_get_pdn_blocked_status
(
  void
);

/*===========================================================================
FUNCTION  DS_3GPP_DAM_GET_T3402_TIMER_VAL

DESCRIPTION
  This function gets the remaining Timer value of the throttle SM

PARAMETERS
   void            
 
DEPENDENCIES
  None.

RETURN VALUE 
  timer_val: Remaining Timer value 

SIDE EFFECTS
  None.

===========================================================================*/

uint32 ds_3gpp_dam_get_t3402_timer_val
(
  void
);

/*===========================================================================
FUNCTION  DS_3GPP_DAM_GET_APN_NAME

DESCRIPTION
  This function stores the Reject APN in the pointer passed

PARAMETERS
  apn_name: Memory block in which APN name has to be copied            
 
DEPENDENCIES
  None.

RETURN VALUE 
  None 

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_dam_get_apn_name
(
  char                     *apn_name
);

/*===========================================================================
FUNCTION  DS_3GPP_DAM_POWER_UP_INIT      

DESCRIPTION
  This function performs power up initialization of the DAM Throttling module

PARAMETERS
  None.
 
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_dam_power_up_init
(
  void 
);

#endif /* DS_3GPP_DAM_H */
