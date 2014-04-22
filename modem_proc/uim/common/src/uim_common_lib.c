/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


             U I M   C O M M O N   L I B R A R Y


GENERAL DESCRIPTION

  This file contains the common Library functions that all
  UIM modules can call.

EXTERNALIZED FUNCTIONS

uim_common_get_modem_rat_capabilities
  Retrieves the modem tech capability (i.e. RATs supported)


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2016 - 2017 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //components/rel/uim.mpss/3.3.1/common/src/uim_common_lib.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/07/17   me      Added support for the CIOT LTE EFS with CAT1 and NB1 values 
07/27/16   shr     Initial revision

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_common_lib.h"
#include "uim_msg.h"
#include "policyman.h"
#include "hwio_cap.h"
#include "uim_common_efs.h"
#include "cm.h"

/*===========================================================================
FUNCTION:      UIM_COMMON_GET_MODEM_RAT_CAPABILITIES

DESCRIPTION:
  Retrieves the RATs supported by the modem

PARAMETERS:
  None

DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN VALUE:
 uint32: Mask of RATs supported
         GSM Mask     -> 0x01
         1X Mask      -> 0x02
         WCDMA Mask   -> 0x04
         HSPA Mask    -> 0x08
         TDSCDMA Mask -> 0x10
         LTE Mask     -> 0x20
         LTE M1 Mask  -> 0x40
         LTE NB1 Mask -> 0x80

SIDE EFFECTS:
  None
=============================================================================*/
uint32 uim_common_get_modem_rat_capabilities
(
  void
)
{
  static boolean                   is_modem_cap_init;
  static uint32                    modem_cap;
  uint32                           rat_mask              = 0;

  PACKED struct PACKED_POST {
    uint8     version;
    uint8     lte_op_mode;
    uint64    lte_m1_band_pref;
    uint64    lte_nb1_band_pref;
    uint8     rfu[50];
  } nv_data_ciot;

  if(is_modem_cap_init)
  {
    return modem_cap;
  }

  /*-----------------------------------------------------------------------------
    Get RATs that are enabled 
    NOTES:
    1. mcs_modem_has_capability() need not be called seperately for any RATs - 
       policyman_get_hardware_rats_bands() already takes that into account.
    2. policyman_get_hardware_rats_bands() takes band availability into account
       when reporting RATs (removing RATs that have no available bands), so NULL
       can be passed for all the band pointers and only look only at the RATs
       that are returned.
  -----------------------------------------------------------------------------*/
  policyman_get_hardware_rats_bands(&rat_mask, NULL, NULL, NULL);

#ifdef FEATURE_GSM
  if(rat_mask & SYS_SYS_MODE_MASK_GSM)
  {
    modem_cap = modem_cap | UIM_MODEM_CAP_FEATURE_GSM_MASK;
  }
#endif /* FEATURE_GSM */

#ifdef FEATURE_CDMA
  if(rat_mask & SYS_SYS_MODE_MASK_CDMA)
  {
    modem_cap = modem_cap | UIM_MODEM_CAP_FEATURE_1X_MASK;
  }
#endif /* FEATURE_CDMA */

#ifdef FEATURE_WCDMA
  if(rat_mask & SYS_SYS_MODE_MASK_WCDMA)
  {
    modem_cap = modem_cap | UIM_MODEM_CAP_FEATURE_WCDMA_MASK;
  }
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_HSDPA
  if((mcs_modem_has_capability(MCS_MODEM_CAPABILITY_FEATURE_HSPA) ==
      MCS_MODEM_CAP_AVAILABLE) &&
     (rat_mask & SYS_SYS_MODE_MASK_WCDMA))
  {
    modem_cap = modem_cap | UIM_MODEM_CAP_FEATURE_HSPA_MASK;
  }
#endif /* FEATURE_HSDPA */

#ifdef FEATURE_TDSCDMA
  if(rat_mask & SYS_SYS_MODE_MASK_TDS)
  {
    modem_cap = modem_cap | UIM_MODEM_CAP_FEATURE_TDSCDMA_MASK;
  }
#endif /* FEATURE_TDSCDMA */

#ifdef FEATURE_LTE
  if(rat_mask & SYS_SYS_MODE_MASK_LTE)
  {
    modem_cap = modem_cap | UIM_MODEM_CAP_FEATURE_LTE_MASK;
  }

  /* Check if the CIOT EFS (NV 73912 )is set to CAT M1 and NB1, 
     if yes then set the bits in the modem_cap mask */
  if (uim_common_efs_read(UIM_COMMON_EFS_NON_UIM_CIOT_LTE_PREFERENCES,
                           UIM_COMMON_EFS_ITEM_FILE_TYPE,
                           UIM_COMMON_EFS_DEVICE,
                           (uint8 *)&(nv_data_ciot),
                           sizeof(nv_data_ciot)) == UIM_COMMON_EFS_SUCCESS)
  {
    if (nv_data_ciot.lte_op_mode == CM_LTE_IOT_OP_MODE_PREF_M1 ||
        nv_data_ciot.lte_op_mode == CM_LTE_IOT_OP_MODE_PREF_WB_M1)
    {
      modem_cap = modem_cap | UIM_MODEM_CAP_FEATURE_LTE_M1_MASK;
    }

    else if (nv_data_ciot.lte_op_mode == CM_LTE_IOT_OP_MODE_PREF_NB1 ||
             nv_data_ciot.lte_op_mode == CM_LTE_IOT_OP_MODE_PREF_WB_NB1)
    {
      modem_cap = modem_cap | UIM_MODEM_CAP_FEATURE_LTE_NB1_MASK;
    }
    
    else if (nv_data_ciot.lte_op_mode == CM_LTE_IOT_OP_MODE_PREF_M1_NB1 || 
             nv_data_ciot.lte_op_mode == CM_LTE_IOT_OP_MODE_PREF_WB_M1_NB1)
    {
      modem_cap = modem_cap | UIM_MODEM_CAP_FEATURE_LTE_M1_MASK |
                              UIM_MODEM_CAP_FEATURE_LTE_NB1_MASK;
    }
  }
#endif /* FEATURE_LTE */

  is_modem_cap_init = TRUE;

  UIM_MSG_LOW_1("Modem Capabilities: 0x%x", modem_cap);

  return modem_cap;
} /* uim_common_get_modem_rat_capabilities */
