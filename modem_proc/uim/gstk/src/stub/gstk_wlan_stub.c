/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                     G S T K   W L A N   S T U B


GENERAL DESCRIPTION
  This source file contains dummy functions for WLAN registration and callback


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

$Header: //components/rel/uim.mpss/3.3.1/gstk/src/stub/gstk_wlan_stub.c#1 $ $DateTime: 2020/07/22 10:53:27 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/21/16   gm      Initial Version

===========================================================================*/
/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/* GSTK Header Files   */
#include "gstk_wlan.h"


/*===========================================================================
FUNCTION gstk_ds_event_reg_wlan_ssid_change

DESCRIPTION
  Dummy function for having GSTK register with DS for WLAN SSID changes.

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
gstk_status_enum_type gstk_ds_event_reg_wlan_ssid_change(void)
{
  return GSTK_UNSUPPORTED_COMMAND;
}/* gstk_ds_event_reg_wlan_ssid_change */

/*===========================================================================
FUNCTION gstk_ds_event_reg_wlan_mac_addr_change

DESCRIPTION
  Dummy function for having GSTK register with DS for WLAN MAC address changes.

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
gstk_status_enum_type gstk_ds_event_reg_wlan_mac_addr_change(void)
{
  return GSTK_UNSUPPORTED_COMMAND;
}/* gstk_ds_event_reg_wlan_mac_addr_change */

/*===========================================================================
FUNCTION gstk_process_ds_wlan_mac_addr_change_ind

DESCRIPTION
  Dummy function for processes WLAN MAC Address change indication from DS

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
)
{
  (void) cmd_ptr;
}/* gstk_process_ds_wlan_mac_addr_change_ind */

/*===========================================================================
FUNCTION gstk_process_ds_wlan_ssid_change_ind

DESCRIPTION
  Dummy function for processes WLAN SSID change indication from DS

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
)
{
  (void) cmd_ptr;
} /* gstk_process_ds_wlan_ssid_change_ind */

/*===========================================================================
FUNCTION gstk_process_ds_system_status_ex_ind_wlan_status

DESCRIPTION
  Dummy function for processes System Status Indication from DS w.r.t. change to WLAN status.
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
)
{
  (void) cmd_ptr;
} /* gstk_process_ds_system_status_ex_ind_wlan_status */

