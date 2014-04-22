/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                     G S T K   W L A N


GENERAL DESCRIPTION
  This source file contains functions for WLAN registration and callback


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

$Header: //components/rel/uim.mpss/3.3.1/gstk/src/gstk_wlan.c#1 $ $DateTime: 2020/07/22 10:53:27 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/03/16   shr     GSTK/ESTK globals memory optimization
05/25/16   shr     F3 reduction: Malloc Failure, Memcpy, Null Ptr checks
03/21/16   gm      Initial Version

===========================================================================*/
/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/* Memory Header Files */
#include "memheap.h"

/* GSTK Header Files   */
#include "gstk.h"
#include "gstkutil.h"
#include "gstk_wlan.h"

/* Output & String Header Files */
#include "err.h"
#include "string.h"
#include "uim_msg.h"


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
gstk_status_enum_type gstk_ds_event_reg_wlan_ssid_change(void)
{
  int16  dss_ret_val = 0;
  int16  dss_errno   = 0;

  UIM_MSG_HIGH_0("gstk_ds_event_reg_wlan_ssid_change");

  dss_ret_val =  ds_sys_event_reg_ex(DS_SYS_TECH_WLAN,
                                     DS_SYS_EVENT_WLAN_SSID_CHANGE,
                                     DS_SYS_PRIMARY_SUBS,
                                     gstk_ds_sys_evt_cb,
                                     NULL,
                                     &dss_errno);
  if(0 > dss_ret_val)
  {
    UIM_MSG_ERR_2("DS WLAN SSID change event reg for Primary Subscription failed ret_val=0x%x, err 0x%x",
                  dss_ret_val, dss_errno);
  }

  if(gstk_shared_data.gstk_num_of_instances > 1)
  {
    dss_ret_val = ds_sys_event_reg_ex(DS_SYS_TECH_WLAN,
                                      DS_SYS_EVENT_WLAN_SSID_CHANGE,
                                      DS_SYS_SECONDARY_SUBS,
                                      gstk_ds_sys_evt_cb,
                                      NULL,
                                      &dss_errno);

    if(0 > dss_ret_val)
    {
      UIM_MSG_ERR_2("DS WLAN SSID change event reg for Secondary Subscription failed, ret_val=0x%x, err 0x%x",
                    dss_ret_val, dss_errno);
    }
  }

  if(gstk_shared_data.gstk_num_of_instances > 2)
  {
   dss_ret_val = ds_sys_event_reg_ex(DS_SYS_TECH_WLAN,
                                      DS_SYS_EVENT_WLAN_SSID_CHANGE,
                                      DS_SYS_TERTIARY_SUBS,
                                      gstk_ds_sys_evt_cb,
                                      NULL,
                                      &dss_errno);

    if(0 > dss_ret_val)
    {
      UIM_MSG_ERR_2("DS SSID change event reg for Tertiary Subscription failed, ret_val=0x%x, err 0x%x",
                    dss_ret_val, dss_errno);
    }
  }

  return GSTK_SUCCESS;
} /* gstk_ds_event_reg_wlan_ssid_change */

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
gstk_status_enum_type gstk_ds_event_reg_wlan_mac_addr_change(void)
{
  int16  dss_ret_val = 0;
  int16  dss_errno   = 0;

  UIM_MSG_HIGH_0("gstk_ds_event_reg_wlan_mac_addr_change");

  dss_ret_val =  ds_sys_event_reg_ex(DS_SYS_TECH_WLAN,
                                     DS_SYS_EVENT_WLAN_MAC_ADDR_CHANGE,
                                     DS_SYS_PRIMARY_SUBS,
                                     gstk_ds_sys_evt_cb,
                                     NULL,
                                     &dss_errno);
  if(0 > dss_ret_val)
  {
    UIM_MSG_ERR_2("DS WLAN MAC address change event reg for Primary Subscription failed ret_val=0x%x, err 0x%x",
                  dss_ret_val, dss_errno);
  }

  if(gstk_shared_data.gstk_num_of_instances > 1)
  {
    dss_ret_val = ds_sys_event_reg_ex(DS_SYS_TECH_WLAN,
                                      DS_SYS_EVENT_WLAN_MAC_ADDR_CHANGE,
                                      DS_SYS_SECONDARY_SUBS,
                                      gstk_ds_sys_evt_cb,
                                      NULL,
                                      &dss_errno);

    if(0 > dss_ret_val)
    {
      UIM_MSG_ERR_2("DS WLAN MAC address change event reg for Secondary Subscription failed, ret_val=0x%x, err 0x%x",
                    dss_ret_val, dss_errno);
    }
  }

 if(gstk_shared_data.gstk_num_of_instances>2)
 {
    dss_ret_val = ds_sys_event_reg_ex(DS_SYS_TECH_WLAN,
                                      DS_SYS_EVENT_WLAN_MAC_ADDR_CHANGE,
                                      DS_SYS_TERTIARY_SUBS,
                                      gstk_ds_sys_evt_cb,
                                      NULL,
                                      &dss_errno);

    if(0 > dss_ret_val)
    {
      UIM_MSG_ERR_2("DS MAC Address change event reg for Tertiary Subscription failed, ret_val=0x%x, err 0x%x",
                    dss_ret_val, dss_errno);
    }
 }
  return GSTK_SUCCESS;
} /* gstk_ds_event_reg_wlan_mac_addr_change */

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
)
{
  if(cmd_ptr == NULL)
  {
    UIM_MSG_ERR_0("cmd_ptr NULL");
    return;
  }

  gstk_util_dump_byte_array(
    "WLAN MAC Address",
    cmd_ptr->ds_wlan_mac_addr_change_ind.ds_wlan_mac_addr_info.ap_mac_addr,
    sizeof(cmd_ptr->ds_wlan_mac_addr_change_ind.ds_wlan_mac_addr_info.ap_mac_addr));

  /* Check if MAC address has changed - the event can also come in if active
     DS SUB has changed but MAC address is the same */
  if (memcmp(gstk_shared_data.wlan_info.wlan_mac_addr.ap_mac_addr,
             cmd_ptr->ds_wlan_mac_addr_change_ind.ds_wlan_mac_addr_info.ap_mac_addr,
             sizeof(gstk_shared_data.wlan_info.wlan_mac_addr.ap_mac_addr)) != 0)
  {
    UIM_MSG_HIGH_0("WLAN MAC address has changed, reset WLAN unusable status if required");
    /* If WLAN MAC address has changed, its likely a new BIP Session over WLAN
       may go through. Hence, reset the WLAN unusable status if its currently
       marked as unusable */
    estk_reset_wlan_unusable_status();
  }

  /* Cache the current WLAN MAC Address */
  gstk_shared_data.wlan_info.wlan_mac_addr =
    cmd_ptr->ds_wlan_mac_addr_change_ind.ds_wlan_mac_addr_info;

} /* gstk_process_ds_wlan_mac_addr_change_ind */

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
)
{
  if(cmd_ptr == NULL)
  {
    UIM_MSG_ERR_0("cmd_ptr NULL");
    return;
  }

  gstk_util_dump_byte_array(
    "WLAN SSID",
    (byte *)cmd_ptr->ds_wlan_ssid_change_ind.ds_wlan_ssid_info.ssid,
    sizeof(cmd_ptr->ds_wlan_ssid_change_ind.ds_wlan_ssid_info.ssid));

  /* Check if SSID has changed - the event can also come in if active
     DS SUB has changed but MAC address is the same */
  if (memcmp(gstk_shared_data.wlan_info.wlan_ssid.ssid,
             (uint8*)cmd_ptr->ds_wlan_ssid_change_ind.ds_wlan_ssid_info.ssid,
             sizeof(cmd_ptr->ds_wlan_ssid_change_ind.ds_wlan_ssid_info.ssid)) != 0)
  {
    UIM_MSG_HIGH_0("WLAN SSID has changed, reset WLAN unusable status if required");
    /* If WLAN SSID has changed, its likely a new BIP Session over WLAN
       may go through. Hence, reset the WLAN unusable status if its currently
       marked as unusable */
    estk_reset_wlan_unusable_status();
  }

  /* Cache the current WLAN SSID */
  gstk_shared_data.wlan_info.wlan_ssid =
    cmd_ptr->ds_wlan_ssid_change_ind.ds_wlan_ssid_info;
} /* gstk_process_ds_wlan_ssid_change_ind */

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
)
{
  uint32         index             = 0;
#if defined(FEATURE_ESTK)
  estk_cmd_type *estk_task_cmd_ptr = NULL;
  boolean        wlan_avail_temp   = FALSE;
#endif /* FEATURE_ESTK */

  UIM_MSG_HIGH_0("In gstk_process_ds_system_status_ex_ind_wlan_status");

  if(cmd_ptr == NULL)
  {
    UIM_MSG_ERR_0("cmd_ptr NULL");
    return;
  }

  /* Check for WLAN status */
#if defined(FEATURE_ESTK)
  wlan_avail_temp = gstk_shared_data.wlan_info.is_wlan_available;
#endif /* FEATURE_ESTK */

  /* Mark WLAN as unavailable  */
  gstk_shared_data.wlan_info.is_wlan_available = FALSE;

  /* Check if WLAN (local breakout, i.e. not I-WLAN) is available */
  for(index = 0;
      index < cmd_ptr->ds_sys_status_ex_ind.ds_sys_status_info.num_avail_sys;
      index++)
  {
    if((cmd_ptr->ds_sys_status_ex_ind.ds_sys_status_info.avail_sys[index].technology ==
        GSTK_DS_SYS_NETWORK_WLAN) &&
       (cmd_ptr->ds_sys_status_ex_ind.ds_sys_status_info.avail_sys[index].rat_value ==
        GSTK_DS_SYS_RAT_EX_WLAN))
    {
      gstk_shared_data.wlan_info.is_wlan_available = TRUE;
      break;
    }
  }

  UIM_MSG_HIGH_1("GSTK_DS_SYSTEM_STATUS_EX_IND, WLAN Status 0x%x",
                 gstk_shared_data.wlan_info.is_wlan_available);

#if defined(FEATURE_ESTK)
  /* If WLAN availability has changed, notify ESTK */
  if(wlan_avail_temp != gstk_shared_data.wlan_info.is_wlan_available)
  {
    estk_task_cmd_ptr = estk_get_cmd_buf();
    if (estk_task_cmd_ptr != NULL)
    {
      estk_task_cmd_ptr->hdr.cmd            = ESTK_BIP_WLAN_STATUS_CHANGE_EVT_CMD;
      estk_task_cmd_ptr->hdr.cmd_ref_id     = 0;           /*doesn't matter*/
      estk_task_cmd_ptr->hdr.command_number = 0;           /*doesn't matter*/
      estk_task_cmd_ptr->hdr.sim_slot_id    = GSTK_SLOT_1; /* doesn't matter */
      estk_task_cmd_ptr->cmd_data.wlan_status_evt.wlan_available =
        gstk_shared_data.wlan_info.is_wlan_available;

      estk_put_cmd(estk_task_cmd_ptr);
    }
  }
#endif /* FEATURE_ESTK */
} /* gstk_process_ds_system_status_ex_ind_wlan_status */

