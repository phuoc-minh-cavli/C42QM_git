/*================================================================================

                         Q M I _ U I M _ S U B _ M G R _ P A R S I N G. C


DESCRIPTION
  This file contains functions for QMI UIM service's Subscription Manager TLV parsing functionality.


Copyright (c) 2016-2018 QUALCOMM Technologies, Inc(QTI).
All Rights Reserved.
QUALCOMM Technologies Confidential and Proprietary

$Header: //components/rel/uim.mpss/3.3.1/uimqmi/src/qmi_uim_sub_mgr_parsing.c#1 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
10/15/18    tq     Migration of QMI UIM service to QCCI-QCSI
06/16/16    vdc    Initial Revision
=============================================================================*/

#include "uim_variation.h"
#include "qmi_framework.h"
#include "qmi_uim_parsing.h"
#include "qmi_uim_cat_common.h"
#include "qmi_uim_sub_mgr_parsing.h"
#include "qmi_svc_utils.h"
#include "user_identity_module_v01.h"
#include "amssassert.h"
#include <stringl/stringl.h>


/*===========================================================================
  FUNCTION QMI_UIM_SUB_MGR_RESPONSE_PHYSICAL_SLOTS_STATUS()

  DESCRIPTION
    Populate for slots status indication

  PARAMETERS
    phy_slot_status_ptr             : Output status of physical slots 
    phy_slot_status_len_ptr         : Output number of slots
    is_valid                        : Output status of physical slots is valid
    slot_status_ptr                 : Status of slots
    num_slots                       : Number of physical slots

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_sub_mgr_response_physical_slots_status
(
  uim_physical_slot_status_type_v01 *phy_slot_status_ptr,
  uint32                            *phy_slot_status_len_ptr,
  uint8                             *is_valid,
  const uim_phy_slot_status_type   * slot_status_ptr,
  uint8                              num_slots
)
{
  uint8   i                           = 0;

  ASSERT(phy_slot_status_ptr && phy_slot_status_len_ptr && is_valid &&slot_status_ptr);

  *phy_slot_status_len_ptr = num_slots;

  /* Copy physical slots status in the TLV */
  for (i = 0; i < num_slots && i < UIM_MAX_NUMBER_INSTANCES; i++)
  {
    switch (slot_status_ptr[i].card_presence_status)
    {
      case UIM_CARD_REMOVED:
        phy_slot_status_ptr[i].physical_card_status = UIM_PHYSICAL_CARD_STATE_ABSENT_V01;
        break;
      case UIM_CARD_INSERTED:
        phy_slot_status_ptr[i].physical_card_status = UIM_PHYSICAL_CARD_STATE_PRESENT_V01;
        break;
      case UIM_CARD_UNCHANGED:
      case UIM_CARD_STATUS_UNKNOWN:
      default:
        phy_slot_status_ptr[i].physical_card_status = UIM_PHYSICAL_CARD_STATE_UNKNOWN_V01;
      break;
    }

    switch (slot_status_ptr[i].activity_status)
    {
      case UIM_SLOT_STATE_INACTIVE:
        phy_slot_status_ptr[i].physical_slot_state = UIM_PHYSICAL_SLOT_STATE_INACTIVE_V01;
        break;
      case UIM_SLOT_STATE_ACTIVE:
        phy_slot_status_ptr[i].physical_slot_state = UIM_PHYSICAL_SLOT_STATE_ACTIVE_V01;
        break;
      default:
        phy_slot_status_ptr[i].physical_slot_state = UIM_PHYSICAL_SLOT_STATE_ACTIVE_V01;
        break;
    }

    switch (slot_status_ptr[i].logical_slot)
    {
      case UIM_SLOT_1:
        phy_slot_status_ptr[i].logical_slot = UIM_SLOT_1_V01;
        break;
      case UIM_SLOT_2:
        phy_slot_status_ptr[i].logical_slot = UIM_SLOT_2_V01;
        break;
      case UIM_SLOT_3:
        phy_slot_status_ptr[i].logical_slot = UIM_SLOT_3_V01;
        break;
      case UIM_SLOT_NONE:
      default:
        return FALSE;
    }

    if(slot_status_ptr[i].iccid_len > QMI_UIM_ICCID_LEN_MAX_V01)
    {
      return FALSE;
    }

    phy_slot_status_ptr[i].iccid_len = slot_status_ptr[i].iccid_len;

    if(phy_slot_status_ptr[i].iccid_len > 0)
    {
      (void)memscpy(phy_slot_status_ptr[i].iccid,
                    sizeof(phy_slot_status_ptr[i].iccid),
                    (void*)slot_status_ptr[i].iccid,
                    slot_status_ptr[i].iccid_len);
    }
  }

  return TRUE;
} /* qmi_uim_sub_mgr_response_physical_slots_status */

