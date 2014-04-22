/*================================================================================

                         Q M I _ U I M _ S U B _M G R _ P A R S I N G. H


DESCRIPTION
  This file contains functions for QMI UIM service's Subscription Manager TLV parsing functionality.

Copyright (c) 2016, 2018 QUALCOMM Technologies, Inc(QTI).
All Rights Reserved.
QUALCOMM Technologies Confidential and Proprietary

$Header: //components/rel/uim.mpss/3.3.1/uimqmi/src/qmi_uim_sub_mgr_parsing.h#1 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
10/15/18    tq     Migration of QMI UIM service to QCCI-QCSI
06/16/16    vdc    Initial Revision
=============================================================================*/

#include "uim_variation.h"
#include "customer.h"
#include "uim_msg.h"
#include "qmi_uim_parsing.h"

/*===========================================================================
  FUNCTION QMI_UIM_SUB_MGR_RESPONSE_PHYSICAL_SLOTS_STATUS()

  DESCRIPTION
    Composes the TLV content for slots status indication

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
  const uim_phy_slot_status_type    *slot_status_ptr,
  uint8                              num_slots
);
