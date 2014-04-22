#ifndef GSTK_WLAN_H
#define GSTK_WLAN_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                     G S T K   W L A N   H E A D E R


GENERAL DESCRIPTION
  This header file contains functions declaration for WLAN registration and callback


FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //components/rel/uim.mpss/3.3.1/gstk/src/gstk_wlan.h#1 $ $DateTime: 2020/07/22 10:53:27 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/21/16   gm      Initial Version

===========================================================================*/
/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "gstk_exp.h"
#include "gstk_exp_v.h"

/*===========================================================================

                        FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION gstk_ds_event_reg_wlan_ssid_change

DESCRIPTION
  Function for having GSTK register with DS for WLAN SSID changes.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_ds_event_reg_wlan_ssid_change(void);

/*===========================================================================
FUNCTION gstk_ds_event_reg_wlan_mac_addr_change

DESCRIPTION
  Function for having GSTK register with DS for WLAN MAC address changes.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_ds_event_reg_wlan_mac_addr_change(void);

/*===========================================================================
FUNCTION gstk_process_ds_wlan_mac_addr_change_ind

DESCRIPTION
  Processes WLAN MAC Address change indication from DS

PARAMETERS
  cmd_ptr: [Input] DS indication ptr

DEPENDENCIES
  None

RETURN VALUE
  void

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_process_ds_wlan_mac_addr_change_ind(
  gstk_cmd_type *cmd_ptr
);

/*===========================================================================
FUNCTION gstk_process_ds_wlan_ssid_change_ind

DESCRIPTION
  Processes WLAN SSID change indication from DS

PARAMETERS
  cmd_ptr: [Input] DS indication ptr

DEPENDENCIES
  None

RETURN VALUE
  void

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_process_ds_wlan_ssid_change_ind(
  gstk_cmd_type *cmd_ptr
);

/*===========================================================================
FUNCTION gstk_process_ds_system_status_ex_ind_wlan_status

DESCRIPTION
  Processes System Status Indication from DS w.r.t. change to WLAN status.
  Posts message to ESTK if WLAN status has changed.

PARAMETERS
  cmd_ptr: [Input] DS indication ptr

DEPENDENCIES
  None

RETURN VALUE
  void

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_process_ds_system_status_ex_ind_wlan_status(
  gstk_cmd_type *cmd_ptr
);

#endif /* GSTK_WLAN_H */
