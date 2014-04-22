#ifndef DS_EPS_ATTACH_HDLR_H
#define DS_EPS_ATTACH_HDLR_H

/*===========================================================================
                      DS_EPS_ATTACH_HDLR.H
DESCRIPTION
This file exposes attach w/o PDN handler through API'S and data structures.

Copyright (c) 2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/3gpp/pdnmgr/inc/ds_eps_attach_hdlr.h#1 $
  $DateTime: 2020/03/17 08:03:16 $$Author: pwbldsvc $

when           who    what, where, why
--------    ---    ----------------------------------------------------------
08/09/17    sd      First checkin
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "ds_pdn_defs.h"
#include "ds_eps_pdn_manager.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/


/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/


/*===========================================================================

FUNCTION DS_EPS_ATTACH_HDLR_BRING_UP_ATTACH_WO_PDN

DESCRIPTION
  This function handles the attach w/o PDN bring-up.

PARAMETERS
  subs_id       - Subs id on which the connection is sent out
DEPENDENCIES
  

RETURN VALUE  
  DS_EPS_BRINGUP_ATTACH_PDN_FAIL - if bringup fails
  DS_EPS_BRINGUP_ATTACH_PDN_SUCCESS - if all operations went fine

SIDE EFFECTS
  None
===========================================================================*/

ds_eps_bringup_attach_pdn_result_e ds_eps_attach_hdlr_bring_up_attach_wo_pdn
(
  void
);

/*===========================================================================

FUNCTION DS_EPS_ATTACH_HDLR_INITIATE_ATTACH_WO_PDN

DESCRIPTION
  This function initiates attach without PDN connection. This is applicable only to the
  LTE mode of operation.

PARAMETERS
  seq_num       - Seq_num to be sent out
  subs_id       - Subs id on which the connection is sent out
DEPENDENCIES
  

RETURN VALUE  
  DS3G_SUCCESS - call was originated with CM.  
  DS3G_FAILURE - call could not be originated.

SIDE EFFECTS
  None
===========================================================================*/
boolean ds_eps_attach_hdlr_initiate_attach_wo_pdn
(
  uint8                   seq_num,
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION DS_EPS_ATTACH_HDLR_ATTACH_WO_PDN_REJ_HDLR

DESCRIPTION
  This function is used to handle the PDN Conn Reject indication in case 
  of attach withour PDN.

  This function is applicable only to EPS mode of operation.

PARAMETERS
  subs_id     - Subscription id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_eps_attach_hdlr_attach_wo_pdn_rej_hdlr
(
  void
);

/*===========================================================================
FUNCTION DS_EPS_ATTACH_HDLR_ATTACH_WO_PDN_FAIL_HDLR

DESCRIPTION
  This function is used to handle the PDN Conn Fail indication in case 
  of attach withour PDN.

  This function is applicable only to EPS mode of operation.

PARAMETERS
  subs_id     - Subscription id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_eps_attach_hdlr_attach_wo_pdn_fail_hdlr
(
  void
);

/*===========================================================================
FUNCTION DS_EPS_ATTACH_HDLR_ATTACH_WO_PDN_ERR_HDLR

DESCRIPTION
  This function is used to handle the PDN Conn request error in case 
  of attach withour PDN.

  This function is applicable only to EPS mode of operation.

PARAMETERS
  subs_id     - Subscription id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_eps_attach_hdlr_attach_wo_pdn_err_hdlr
(
  void
);

/*===========================================================================
FUNCTION DS_EPS_ATTACH_HDLR_DETACH_IND_HDLR

DESCRIPTION
  This function is used to handle the Detach indication from NAS.
  WE will recieve this detach ind only when we are attached without PDN
  Note: This message has no payload.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_eps_attach_hdlr_detach_ind_hdlr
(
  void
);

/*===========================================================================
FUNCTION DS_EPS_ATTACH_HDLR_SET_NW_ATTACH_WO_PDN_SUPPORT

DESCRIPTION
  This function store the attach w/o PDN capability of the NW

PARAMETERS
  nw_atttach_wo_pdn_support - attach w/o PDN capability of the NW
  subs_id                   - Subscription id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_eps_attach_hdlr_set_nw_attach_wo_pdn_support
(
  boolean                 nw_attach_wo_pdn_support
);

/*===========================================================================
FUNCTION DS_EPS_ATTACH_HDLR_GET_NW_ATTACH_WO_PDN_SUPPORT

DESCRIPTION
  This function returns the attach w/o PDN capability of the NW

PARAMETERS
  subs_id                   - Subscription id

DEPENDENCIES
  None.

RETURN VALUE
  nw_atttach_wo_pdn_support - attach w/o PDN capability of the NW

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_eps_attach_hdlr_get_nw_attach_wo_pdn_support
(
  void
);

#endif /* DS_EPS_ATTACH_HDLR_H */

