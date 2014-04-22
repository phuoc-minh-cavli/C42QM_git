/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            M M G S D I   I O T   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the IOT functions 

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2016 - 2017 ,2019 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //components/rel/uim.mpss/3.3.1/mmgsdi/src/mmgsdi_iot.c#1 $$ $DateTime: 2020/07/22 10:53:27 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/14/19   bcho    Use bit_mask_256_t for lte_m1_band_pref/lte_nb1_band_pref
03/12/19   vm      Set byte 10 in EF-TST as per ATT REQ 13340 v18.3  
11/17/17   me      Support for FEATURE_UIM_JO_3_0_3_CAPABILITIES 
08/08/17   nr      Added support for 256 bands (LTE U related)
12/14/16   me      Added check for NAS EFS to get the preferred LTE MTC CAT - M1 or NB1
10/18/16   sn      Support for FEATURE_UIM_JO_3_0_2_CAPABILITIES 
09/23/16   me      UIM compile failure in WCG flavor  
08/16/16   me      initial version 
 
===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "mmgsdi_nv.h"
#include "mmgsdi_iot.h"

#ifdef FEATURE_LTE
#include "cm.h"
#include "sys.h"
#include "policyman.h"
#include "hwio_cap.h"
#endif /* FEATURE_LTE */

#include "uim_msg.h"

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/*---------------------------------------------------------------------------
  To store list of status of IOT features supported on the target
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
  To store list of IOT LTE Bands 
---------------------------------------------------------------------------*/
static PACKED struct PACKED_POST 
{
  sys_sband_lte_e_type               band_position;
  boolean                            M1;
  boolean                            NB1;
} mmgsdi_lte_category_iot_m1_nb1[] =
{
  { SYS_SBAND_LTE_EUTRAN_BAND2,         TRUE,  TRUE},
  { SYS_SBAND_LTE_EUTRAN_BAND3,         TRUE,  TRUE},
  { SYS_SBAND_LTE_EUTRAN_BAND4,         TRUE,  FALSE},
  { SYS_SBAND_LTE_EUTRAN_BAND5,         TRUE,  TRUE},
  { SYS_SBAND_LTE_EUTRAN_BAND7,         TRUE,  FALSE},
  { SYS_SBAND_LTE_EUTRAN_BAND8,         TRUE,  TRUE},
  { SYS_SBAND_LTE_EUTRAN_BAND11,        TRUE,  FALSE},
  { SYS_SBAND_LTE_EUTRAN_BAND12,        TRUE,  TRUE},
  { SYS_SBAND_LTE_EUTRAN_BAND13,        TRUE,  TRUE},
  { SYS_SBAND_LTE_EUTRAN_BAND17,        FALSE, TRUE},
  { SYS_SBAND_LTE_EUTRAN_BAND18,        TRUE,  TRUE},
  { SYS_SBAND_LTE_EUTRAN_BAND19,        TRUE,  TRUE},
  { SYS_SBAND_LTE_EUTRAN_BAND20,        TRUE,  TRUE},
  { SYS_SBAND_LTE_EUTRAN_BAND21,        TRUE,  FALSE},
  { SYS_SBAND_LTE_EUTRAN_BAND26,        TRUE,  TRUE},
  { SYS_SBAND_LTE_EUTRAN_BAND28,        TRUE,  TRUE},
  { SYS_SBAND_LTE_EUTRAN_BAND31,        TRUE,  FALSE},
  { SYS_SBAND_LTE_EUTRAN_BAND39,        TRUE,  FALSE},
  { SYS_SBAND_LTE_EUTRAN_BAND41,        TRUE,  FALSE}
};



/* ============================================================================
FUNCTION MMGSDI_IS_IOT_DEVICE

DESCRIPTION
  This utility function return the TRUE or FALSE,
  if the target is IOE and NV is not set or set to MTC then return true
  if the target is IOE and NV is set to non MTC then return false
  if the target is not IOE and NV is not set then return false
  if the target is not IOE and NV is set to MTC then return true
  if the target is not IOE and NV is set to non MTC then return false 

DEPENDENCIES
  None

LIMITATIONS
  none.

RETURN VALUE
  MTC :   TRUE or FALSE

SIDE EFFECTS
  None

============================================================================*/
boolean mmgsdi_is_iot_device(
  void
)
{
  boolean                 is_iot_device = FALSE;
  uint8                   nv_data       = 0;
  
#ifdef FEATURE_UIM_IOE
  is_iot_device = TRUE;
#else 
  is_iot_device = FALSE;
#endif /* FEATURE_UIM_IOT */

  if (uim_common_efs_read(UIM_COMMON_EFS_MMGSDI_MTC,
                          UIM_COMMON_EFS_ITEM_FILE_TYPE,
                          UIM_COMMON_EFS_DEVICE,
                          &nv_data,
                          sizeof(nv_data)) == UIM_COMMON_EFS_SUCCESS)
  {
    if (nv_data == 1)
    {
      is_iot_device = TRUE;
    }
    else if (nv_data == 2)
    {
      is_iot_device = FALSE;
    }
  }
  return is_iot_device;
}/* mmgsdi_is_iot_device */


#if (defined FEATURE_UIM_JO_3_0_2_CAPABILITIES || defined FEATURE_UIM_JO_3_0_3_CAPABILITIES)
/* ============================================================================
FUNCTION MMGSDI_LTE_IOT_IS_BAND_ENABLED

DESCRIPTION
  This utility function returns IOT LTE Band Enabled or Disabled 

DEPENDENCIES
  None

LIMITATIONS
  none.

RETURN VALUE
  ENABLED:  if the IOT LTE Band is enabled
  DISABLED: if the IOT LTE Band is disabled 

SIDE EFFECTS
  None

============================================================================*/
static boolean mmgsdi_lte_iot_is_band_enabled(
  uint64                       bands,
  uint8                        lte_op_mode,
  mmgsdi_iot_category_enum     m1_nb1_cat
)
{
  uint16               i              = 0;
  
  /* if operation mode is M1 run loop through M1 values and return true,
     if or M1_NB1 and flag is M1 then run loop through M1 values and return true
     and dont go through NB1 values as M1 is enabled and its Higher CAT */
  if (lte_op_mode == CM_LTE_IOT_OP_MODE_PREF_M1 || 
       (lte_op_mode == CM_LTE_IOT_OP_MODE_PREF_M1_NB1 && m1_nb1_cat == MMGSDI_IOT_CAT_M1))
  {
    for(i = 0; i < sizeof(mmgsdi_lte_category_iot_m1_nb1) / sizeof(mmgsdi_lte_category_iot_m1_nb1[0]); i++)
    {
      if (mmgsdi_lte_category_iot_m1_nb1[i].M1) 
      {
        if(((bands >> mmgsdi_lte_category_iot_m1_nb1[i].band_position) & 1) != 0)
        {
          return TRUE;
        }
      }
    }
  }

  /* if operation mode is NB1 or M1_NB1 and M1_NB1 flag is NB1 then run loop through NB1 values */
  if (lte_op_mode == CM_LTE_IOT_OP_MODE_PREF_NB1 || 
       (lte_op_mode == CM_LTE_IOT_OP_MODE_PREF_M1_NB1 && m1_nb1_cat == MMGSDI_IOT_CAT_NB1))
  {
    for (i = 0; i < sizeof(mmgsdi_lte_category_iot_m1_nb1) / sizeof(mmgsdi_lte_category_iot_m1_nb1[0]); i++)
    {
      if (mmgsdi_lte_category_iot_m1_nb1[i].NB1) 
      {
        if(((bands >> mmgsdi_lte_category_iot_m1_nb1[i].band_position) & 1) != 0)
        {
          return TRUE;
        }
      }
    }
  }

  return FALSE;
}/* mmgsdi_lte_iot_is_band_enabled */
#endif /* FEATURE_UIM_JO_3_0_2_CAPABILITIES || FEATURE_UIM_JO_3_0_3_CAPABILITIES */  


/* ============================================================================
FUNCTION MMGSDI_GET_IOT_CATEGORY

DESCRIPTION
 This utility function return the IOT category .
 The IOT category will be returned based on the NV 73912 CIOT EFS,
 if the EFS is present and op modes are M1 or NB1 or M1_NB1,
 then return the Iot category as M1 or NB1 or M1_NB1 based on the bands.
 if the EFS is present and the values are set to WB_M1 or WB_NB1 or WB_M1_NB1.
 or EFS not present then return IOT_UNKNOWN.

 CHECKS
  policyman_get_hardware_rats_bands : to check if the LTE RAT mask is supported
  mcs_modem_has_capability          : to check if the LTE is available and supported by the modem
  mmgsdi_lte_iot_is_band_enabled    : to check if the lte bands preference are enabled in the EFS
  

DEPENDENCIES
  None

LIMITATIONS
  none.

RETURN VALUE
  CATEGORY:   IOT category 

SIDE EFFECTS
  None

============================================================================*/
mmgsdi_iot_category_enum mmgsdi_get_iot_category(
  void
)
{
#ifdef FEATURE_LTE
  uint32                             rat_mask               = 0;
#ifdef LTE_BAND_NUM
  sys_lte_band_mask_e_type           lte_bands              = SYS_LTE_BAND_MASK_CONST_NONE;
#else
  sys_band_mask_type                 lte_bands              = SYS_BAND_MASK_EMPTY;
#endif /* LTE_BAND_NUM */
  mcs_modem_cap_return_enum          lte_available          = MCS_MODEM_CAP_UNKNOWN;

#if (defined FEATURE_UIM_JO_3_0_2_CAPABILITIES || defined FEATURE_UIM_JO_3_0_3_CAPABILITIES)
  boolean                            is_lte_m1_band_pref    = FALSE;
  boolean                            is_lte_nb1_band_pref   = FALSE;

  PACKED struct PACKED_POST {
    uint8          version;
    uint8          lte_op_mode;
    bit_mask_256_t lte_m1_band_pref;
    bit_mask_256_t lte_nb1_band_pref;
    uint8     rfu[2];
  } nv_data_ciot;
#endif /* FEATURE_UIM_JO_3_0_2_CAPABILITIES || FEATURE_UIM_JO_3_0_3_CAPABILITIES */  

  policyman_get_hardware_rats_bands(&rat_mask,
                                    NULL,
                                    &lte_bands,
                                    NULL );

  if (!(rat_mask & SYS_SYS_MODE_MASK_LTE) || 
#ifdef LTE_BAND_NUM
      (SYS_LTE_BAND_MASK_CHK_IF_EMPTY(&lte_bands))
#else
      (lte_bands == 0)
#endif /* LTE_BAND_NUM */
     )
  {
    return MMGSDI_IOT_UNKNOWN;
  }

  lte_available = mcs_modem_has_capability(MCS_MODEM_CAPABILITY_FEATURE_LTE);
  if (!(lte_available == MCS_MODEM_CAP_AVAILABLE))
  {
    return MMGSDI_IOT_UNKNOWN;
  }

#if (defined FEATURE_UIM_JO_3_0_2_CAPABILITIES || defined FEATURE_UIM_JO_3_0_3_CAPABILITIES)
  memset(&nv_data_ciot, 0, sizeof(nv_data_ciot));

  /* Check the MMODE CIOT NV 73822 */
  if (uim_common_efs_read(UIM_COMMON_EFS_NON_UIM_CIOT_LTE_PREFERENCES,
                          UIM_COMMON_EFS_ITEM_FILE_TYPE,
                          UIM_COMMON_EFS_DEVICE,
                          (uint8*)&(nv_data_ciot),
                          sizeof(nv_data_ciot)) == UIM_COMMON_EFS_SUCCESS)
  {
    /* Check if LTE M1 band preference is set in the CIOT EFS when the LTE
       Operation mode is set to PREF_M1 or PREF_M1_NB1 */
    if ((nv_data_ciot.lte_op_mode == CM_LTE_IOT_OP_MODE_PREF_M1 ||
         nv_data_ciot.lte_op_mode == CM_LTE_IOT_OP_MODE_PREF_M1_NB1) &&
         nv_data_ciot.lte_m1_band_pref.bits_1_64 != 0)
    {
      /* check if the required LTE bands are set in lte_m1_band_pref,
         if it set then the selected option in lte_op_mode is enabled */
      if(mmgsdi_lte_iot_is_band_enabled(nv_data_ciot.lte_m1_band_pref.bits_1_64, nv_data_ciot.lte_op_mode, MMGSDI_IOT_CAT_M1))
      {
        is_lte_m1_band_pref = TRUE;
      }
    }

    /* check if LTE NB1 band preference is set in the CIOT EFS when the LTE
       Operation mode is set to PREF_NB1 or PREF_M1_NB1 */
    if ((nv_data_ciot.lte_op_mode == CM_LTE_IOT_OP_MODE_PREF_NB1 ||
  	 nv_data_ciot.lte_op_mode == CM_LTE_IOT_OP_MODE_PREF_M1_NB1) &&
  	 nv_data_ciot.lte_nb1_band_pref.bits_1_64 != 0)
    {
      /* check if the required LTE bands are set in lte_nb1_band_pref,
         if it set then the selected option in lte_op_mode is enabled */
      if (mmgsdi_lte_iot_is_band_enabled(nv_data_ciot.lte_nb1_band_pref.bits_1_64, nv_data_ciot.lte_op_mode, MMGSDI_IOT_CAT_NB1))
      {
        is_lte_nb1_band_pref = TRUE;
      }
    }
  }

  /* check if the IOT LTE operation mode and band prefrences are sets in the EFS,
     - if the IOT LTE operation mode in EFS is CM_LTE_IOT_OP_MODE_PREF_M1 
       and LTE M1 band prefrences is set in the EFS then return IOT CAT M1.
     - if the IOT LTE operation mode in EFS is CM_LTE_IOT_OP_MODE_PREF_NB1
       and LTE NB1 band prefrences is set in the EFS then return IOT CAT NB1.
     - if the IOT LTE operation mode in EFS is CM_LTE_IOT_OP_MODE_PREF_M1_NB1: 
       - if LTE M1 & NB1 bands prefrences is set in the EFS then return
         IOT_CAT_M1_NB1.
       - if LTE M1 bands prefrences is set in the EFS then return IOT CAT M1.
       - if LTE NB1 bands prefrences is set in the EFS then return IOT CAT NB1.
       - if both LTE M1 & NB1 band preferences are not set then return
         IOT_UNKNOWN
     - if the IOT LTE operation mode in EFS is CM_LTE_IOT_OP_MODE_WB_M1 or 
       CM_LTE_IOT_OP_MODE_WB_NB1 or CM_LTE_IOT_OP_MODE_WB_M1_NB1 then return 
       IOT_UNKNOWN */
  if ((nv_data_ciot.lte_op_mode == CM_LTE_IOT_OP_MODE_PREF_M1) && is_lte_m1_band_pref)
  {
    return MMGSDI_IOT_CAT_M1;
  }
  else if((nv_data_ciot.lte_op_mode == CM_LTE_IOT_OP_MODE_PREF_NB1) && is_lte_nb1_band_pref)
  {
    return MMGSDI_IOT_CAT_NB1;
  }
  else if(nv_data_ciot.lte_op_mode == CM_LTE_IOT_OP_MODE_PREF_M1_NB1) 
  {
    if (is_lte_m1_band_pref && is_lte_nb1_band_pref)
    {
      /* return CAT M1 NB1 */
      return MMGSDI_IOT_CAT_M1_NB1;
    }
    else if(is_lte_m1_band_pref)
    {
      return MMGSDI_IOT_CAT_M1;
    }
    else if(is_lte_nb1_band_pref)
    {
      return MMGSDI_IOT_CAT_NB1;
    }
  }
#endif /* FEATURE_UIM_JO_3_0_2_CAPABILITIES || FEATURE_UIM_JO_3_0_3_CAPABILITIES */     
#endif /* FEATURE_LTE */
  /* if the Ciot LTE prefrences EFS is not present or present and having
     a CM_LTE_IOT_OP_MODE_WB_M1 or CM_LTE_IOT_OP_MODE_WB_NB1 or
     CM_LTE_IOT_OP_MODE_WB_M1_NB1 or junk data then return IOT_UNKNOWN */
  return MMGSDI_IOT_UNKNOWN;
}/* mmgsdi_get_iot_category */

