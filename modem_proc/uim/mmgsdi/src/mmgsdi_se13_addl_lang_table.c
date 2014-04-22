/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


             M M G S D I   S E 1 3  A D D L  L A N G  T A B L E


GENERAL DESCRIPTION

  This source file contains a hardcoded array of PLMN names in additional
  languages such as Chinese. The names are stored in UNICODE format.


                        COPYRIGHT INFORMATION

Copyright (c) 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //components/rel/uim.mpss/3.3.1/mmgsdi/src/mmgsdi_se13_addl_lang_table.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/22/16   tkl     Stub out additional APIs
05/04/16   vdc     Added feature FEATURE_MMGSDI_SE13_FROM_HARDCODED_ONLY
05/04/16   vdc     Restructure accessing SE13 additional table

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "customer.h"
#include "mmgsdi_se13_addl_lang_table.h"
#include "mmgsdi_eons.h"
#include "mmgsdiutil.h"
#include "uim_msg.h"

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* Additional languages (such as Chinese) PLMN names table.
   zh_hant == Traditional Chinese
   zh_hans == Simplified Chinese */
static mmgsdi_se13_addl_lang_network_table_data_type mmgsdi_addl_lang_network_table[] = {
  /* Traditional Chinese */
  { "zh_hant", 454, 12, MMGSDI_PLMN_NETWK_TYPE_DCS_1800_MASK,
    12, {0x4E, 0x2D, 0x57, 0x0B, 0x79, 0xFB, 0x52, 0xD5, 0x99, 0x99, 0x6E, 0x2F}, 12, {0x4E, 0x2D, 0x57, 0x0B, 0x79, 0xFB, 0x52, 0xD5, 0x99, 0x99, 0x6E, 0x2F}},
  { "zh_hant", 460, 00, MMGSDI_PLMN_NETWK_TYPE_GSM_900_MASK,
    8, {0x4E, 0x2D, 0x57, 0x0B, 0x79, 0xFB, 0x52, 0xD5}, 8, {0x4E, 0x2D, 0x57, 0x0B, 0x79, 0xFB, 0x52, 0xD5}},
  { "zh_hant", 460, 01, MMGSDI_PLMN_NETWK_TYPE_GSM_900_MASK|MMGSDI_PLMN_NETWK_TYPE_UMTS_MASK,
    8, {0x4E, 0x2D, 0x57, 0x0B, 0x80, 0x6F, 0x90, 0x1A}, 8, {0x4E, 0x2D, 0x57, 0x0B, 0x80, 0x6F, 0x90, 0x1A}},
  { "zh_hant", 460, 02, 0xFF,
    8, {0x4E, 0x2D, 0x57, 0x0B, 0x79, 0xFB, 0x52, 0xD5}, 8, {0x4E, 0x2D, 0x57, 0x0B, 0x79, 0xFB, 0x52, 0xD5}},
  { "zh_hant", 460, 05, 0xFF,
    8, {0x4E, 0x2D, 0x57, 0x0B, 0x96, 0xFB, 0x4F, 0xE1}, 8, {0x4E, 0x2D, 0x57, 0x0B, 0x96, 0xFB, 0x4F, 0xE1}},
  { "zh_hant", 460, 06, 0xFF,
    8, {0x4E, 0x2D, 0x57, 0x0B, 0x80, 0x6F, 0x90, 0x1A}, 8, {0x4E, 0x2D, 0x57, 0x0B, 0x80, 0x6F, 0x90, 0x1A}},
  { "zh_hant", 460, 07, 0xFF,
    8, {0x4E, 0x2D, 0x57, 0x0B, 0x79, 0xFB, 0x52, 0xD5}, 8, {0x4E, 0x2D, 0x57, 0x0B, 0x79, 0xFB, 0x52, 0xD5}},
  { "zh_hant", 460, 20, 0xFF,
    14, {0x4E, 0x2D, 0x57, 0x0B, 0x79, 0xFB, 0x52, 0xD5, 0x00, 0x2D, 0x00, 0x33, 0x00, 0x47}, 14, {0x4E, 0x2D, 0x57, 0x0B, 0x79, 0xFB, 0x52, 0xD5, 0x00, 0x2D, 0x00, 0x33, 0x00, 0x47}},
  { "zh_hant", 466, 01, MMGSDI_PLMN_NETWK_TYPE_GSM_900_MASK|MMGSDI_PLMN_NETWK_TYPE_DCS_1800_MASK,
    8, {0x90, 0x60, 0x50, 0xB3, 0x96, 0xFB, 0x4F, 0xE1}, 8, {0x90, 0x60, 0x50, 0xB3, 0x96, 0xFB, 0x4F, 0xE1}}, /* FET */
  { "zh_hant", 466, 06, 0xFF,
    8, {0x54, 0x8C, 0x4F, 0xE1, 0x96, 0xFB, 0x8A, 0x0A}, 8, {0x54, 0x8C, 0x4F, 0xE1, 0x96, 0xFB, 0x8A, 0x0A}},
  { "zh_hant", 466, 88, MMGSDI_PLMN_NETWK_TYPE_DCS_1800_MASK,
    8, {0x54, 0x8C, 0x4F, 0xE1, 0x96, 0xFB, 0x8A, 0x0A}, 8, {0x54, 0x8C, 0x4F, 0xE1, 0x96, 0xFB, 0x8A, 0x0A}}, /* KGT-Online */
  { "zh_hant", 466, 89, MMGSDI_PLMN_NETWK_TYPE_UMTS_MASK,
    8, {0x5A, 0x01, 0x5B, 0xF6, 0x96, 0xFB, 0x4F, 0xE1}, 8, {0x5A, 0x01, 0x5B, 0xF6, 0x96, 0xFB, 0x4F, 0xE1}}, /* Vibo Telecom */
  { "zh_hant", 466, 92, MMGSDI_PLMN_NETWK_TYPE_UMTS_MASK|MMGSDI_PLMN_NETWK_TYPE_GSM_900_MASK|MMGSDI_PLMN_NETWK_TYPE_DCS_1800_MASK,
    8, {0x4E, 0x2D, 0x83, 0xEF, 0x96, 0xFB, 0x4F, 0xE1}, 8, {0x4E, 0x2D, 0x83, 0xEF, 0x96, 0xFB, 0x4F, 0xE1}}, /* Chungwa Telecom */
  { "zh_hant", 466, 97, MMGSDI_PLMN_NETWK_TYPE_DCS_1800_MASK,
    10, {0x53, 0xF0, 0x70, 0x63, 0x59, 0x27, 0x54, 0xE5, 0x59, 0x27}, 10, {0x53, 0xF0, 0x70, 0x63, 0x59, 0x27, 0x54, 0xE5, 0x59, 0x27}}, /* Taiwan Mobile */
  { "zh_hant", 466, 99, MMGSDI_PLMN_NETWK_TYPE_GSM_900_MASK,
    8, {0x6C, 0xDB, 0x4E, 0x9E, 0x96, 0xFB, 0x4F, 0xE1}, 8, {0x6C, 0xDB, 0x4E, 0x9E, 0x96, 0xFB, 0x4F, 0xE1}},
  { "zh_hant", 466, 93, MMGSDI_PLMN_NETWK_TYPE_GSM_900_MASK,
    8, {0x67, 0x71, 0x4F, 0xE1, 0x96, 0xFB, 0x8A, 0x0A}, 8, {0x67, 0x71, 0x4F, 0xE1, 0x96, 0xFB, 0x8A, 0x0A}}, /* TW MOB */

  /* Simplified Chinese */
  { "zh_hans", 454, 12, MMGSDI_PLMN_NETWK_TYPE_DCS_1800_MASK,
    12, {0x4E, 0x2D, 0x56, 0xFD, 0x79, 0xFB, 0x52, 0xA8, 0x99, 0x99, 0x6E, 0x2F}, 12, {0x4E, 0x2D, 0x56, 0xFD, 0x79, 0xFB, 0x52, 0xA8, 0x99, 0x99, 0x6E, 0x2F}},
  { "zh_hans", 460, 00, MMGSDI_PLMN_NETWK_TYPE_GSM_900_MASK,
    8, {0x4E, 0x2D, 0x56, 0xFD, 0x79, 0xFB, 0x52, 0xA8}, 8, {0x4E, 0x2D, 0x56, 0xFD, 0x79, 0xFB, 0x52, 0xA8}},
  { "zh_hans", 460, 01, MMGSDI_PLMN_NETWK_TYPE_GSM_900_MASK|MMGSDI_PLMN_NETWK_TYPE_UMTS_MASK,
    8, {0x4E, 0x2D, 0x56, 0xFD, 0x80, 0x54, 0x90, 0x1A}, 8, {0x4E, 0x2D, 0x56, 0xFD, 0x80, 0x54, 0x90, 0x1A}},
  { "zh_hans", 460, 02, 0xFF,
    8, {0x4E, 0x2D, 0x56, 0xFD, 0x79, 0xFB, 0x52, 0xA8}, 8, {0x4E, 0x2D, 0x56, 0xFD, 0x79, 0xFB, 0x52, 0xA8}},
  { "zh_hans", 460, 05, 0xFF,
    8, {0x4E, 0x2D, 0x56, 0xFD, 0x75, 0x35, 0x4F, 0xE1}, 8, {0x4E, 0x2D, 0x56, 0xFD, 0x75, 0x35, 0x4F, 0xE1}},
  { "zh_hans", 460, 06, 0xFF,
    8, {0x4E, 0x2D, 0x56, 0xFD, 0x80, 0x54, 0x90, 0x1A}, 8, {0x4E, 0x2D, 0x56, 0xFD, 0x80, 0x54, 0x90, 0x1A}},
  { "zh_hans", 460, 07, 0xFF,
    8, {0x4E, 0x2D, 0x56, 0xFD, 0x79, 0xFB, 0x52, 0xA8}, 8, {0x4E, 0x2D, 0x56, 0xFD, 0x79, 0xFB, 0x52, 0xA8}},
  { "zh_hans", 460, 20, 0xFF,
    14, {0x4E, 0x2D, 0x56, 0xFD, 0x79, 0xFB, 0x52, 0xA8, 0x00, 0x2D, 0x00, 0x33, 0x00, 0x47}, 14, {0x4E, 0x2D, 0x56, 0xFD, 0x79, 0xFB, 0x52, 0xA8, 0x00, 0x2D, 0x00, 0x33, 0x00, 0x47}},
  { "zh_hans", 466, 01, MMGSDI_PLMN_NETWK_TYPE_GSM_900_MASK|MMGSDI_PLMN_NETWK_TYPE_DCS_1800_MASK,
    8, {0x8F, 0xDC, 0x4F, 0x20, 0x75, 0x35, 0x4F, 0xE1}, 8, {0x8F, 0xDC, 0x4F, 0x20, 0x75, 0x35, 0x4F, 0xE1}}, /* FET */
  { "zh_hans", 466, 06, 0xFF,
    8, {0x54, 0x8C, 0x4F, 0xE1, 0x75, 0x35, 0x8B, 0xAF}, 8, {0x54, 0x8C, 0x4F, 0xE1, 0x75, 0x35, 0x8B, 0xAF}},
  { "zh_hans", 466, 88, MMGSDI_PLMN_NETWK_TYPE_DCS_1800_MASK,
    8, {0x54, 0x8C, 0x4F, 0xE1, 0x75, 0x35, 0x8B, 0xAF}, 8, {0x54, 0x8C, 0x4F, 0xE1, 0x75, 0x35, 0x8B, 0xAF}}, /* KGT-Online */
  { "zh_hans", 466, 89, MMGSDI_PLMN_NETWK_TYPE_UMTS_MASK,
    8, {0x5A, 0x01, 0x5B, 0x9D, 0x75, 0x35, 0x4F, 0xE1}, 8, {0x5A, 0x01, 0x5B, 0x9D, 0x75, 0x35, 0x4F, 0xE1}}, /* Vibo Telecom */
  { "zh_hans", 466, 92, MMGSDI_PLMN_NETWK_TYPE_UMTS_MASK|MMGSDI_PLMN_NETWK_TYPE_GSM_900_MASK|MMGSDI_PLMN_NETWK_TYPE_DCS_1800_MASK,
    8, {0x4E, 0x2D, 0x53, 0x4E, 0x75, 0x35, 0x4F, 0xE1}, 8, {0x4E, 0x2D, 0x53, 0x4E, 0x75, 0x35, 0x4F, 0xE1}}, /* Chungwa Telecom */
  { "zh_hans", 466, 97, MMGSDI_PLMN_NETWK_TYPE_DCS_1800_MASK,
    10, {0x53, 0xF0, 0x6E, 0x7E, 0x59, 0x27, 0x54, 0xE5, 0x59, 0x27}, 10, {0x53, 0xF0, 0x6E, 0x7E, 0x59, 0x27, 0x54, 0xE5, 0x59, 0x27}}, /* Taiwan Mobile */
  { "zh_hans", 466, 99, MMGSDI_PLMN_NETWK_TYPE_GSM_900_MASK,
    8, {0x6C, 0xDB, 0x4E, 0x9A, 0x75, 0x35, 0x4F, 0xE1}, 8, {0x6C, 0xDB, 0x4E, 0x9A, 0x75, 0x35, 0x4F, 0xE1}},
  { "zh_hans", 466, 93, MMGSDI_PLMN_NETWK_TYPE_GSM_900_MASK,
    8, {0x4E, 0x1C, 0x4F, 0xE1, 0x75, 0x35, 0x8B, 0xAF}, 8, {0x4E, 0x1C, 0x4F, 0xE1, 0x75, 0x35, 0x8B, 0xAF}} /* TW MOB */
}; /* mmgsdi_addl_lang_network_table */


/*==========================================================================
FUNCTION: MMGSDI_SE13_ADDL_LANG_TABLE_GET_INFO

DESCRIPTION:
  Fetches SE13 additional langauge table pointer and number of entries in it.

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
==========================================================================*/
static mmgsdi_return_enum_type mmgsdi_se13_addl_lang_table_get_info
(
  mmgsdi_se13_addl_lang_network_table_data_type **mmgsdi_se13_addl_lang_network_table_pptr,
  uint16                                         *num_of_entries_ptr
)
{
  MMGSDIUTIL_RETURN_IF_NULL_2(num_of_entries_ptr,
                              mmgsdi_se13_addl_lang_network_table_pptr);

  *mmgsdi_se13_addl_lang_network_table_pptr = mmgsdi_addl_lang_network_table;

  *num_of_entries_ptr = sizeof(mmgsdi_addl_lang_network_table)/
                        sizeof(mmgsdi_se13_addl_lang_network_table_data_type);

  return MMGSDI_SUCCESS;
} /* mmgsdi_se13_addl_lang_table_get_info */


/*==========================================================================
FUNCTION MMGSDI_EONS_GET_SE13_ADD_LANG_TABLE_ONS

DESCRIPTION
  Function to get operator name string from the SE13 additional languages
  table

DEPENDENCIES
  None

RETURN VALUE
  BOOLEAN: TRUE OR FALSE

SIDE EFFECTS
  None
==========================================================================*/
boolean mmgsdi_eons_get_se13_addl_lang_table_ons(
  mmgsdi_plmn_info_type * plmn_info_ptr,
  mmgsdi_plmn_id_type     plmn_id,
  sys_sys_mode_e_type     sys_mode
)
{
  uint8                                          index                                   = 0;
  uint16                                         mcc                                     = 0;
  uint16                                         mnc                                     = 0;
  mmgsdi_return_enum_type                        mmgsdi_status                           = MMGSDI_ERROR;
  uint16                                         num_of_entries                          = 0;
  mmgsdi_se13_addl_lang_network_table_data_type *mmgsdi_se13_addl_lang_network_table_ptr = NULL;

  if(plmn_info_ptr == NULL)
  {
    return FALSE;
  }

  /* First fetch SE13 additional table pointer and number of entries in it */
  mmgsdi_status = mmgsdi_se13_addl_lang_table_get_info(&mmgsdi_se13_addl_lang_network_table_ptr,
                                                       &num_of_entries);
  if(mmgsdi_status != MMGSDI_SUCCESS ||
     num_of_entries == 0 ||
     mmgsdi_se13_addl_lang_network_table_ptr == NULL)
  {
    UIM_MSG_MED_0("Additional language SE13 table is invalid");
    return MMGSDI_ERROR;
  }

  mmgsdi_status = mmgsdi_eons_plmn_get_mcc_mnc(plmn_id, &mcc, &mnc);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* If NW PLMNID's MCC is in range of 310-316 (BCCH MCC) and MNC's third
     digit is 'F', multiply MNC with 10 */
  if(mcc >= 310 && mcc <= 316 && ((plmn_id.plmn_id_val[1] & 0xF0) == 0xF0))
  {
    mnc = mnc * 10;
  }

  plmn_info_ptr->num_plmn_additional_names = 0;

  for(index = 0; index < num_of_entries && plmn_info_ptr->num_plmn_additional_names < MMGSDI_ADDITIONAL_NAMES_MAX; index ++)
  {
    if(mcc == mmgsdi_se13_addl_lang_network_table_ptr[index].mcc &&
       mnc == mmgsdi_se13_addl_lang_network_table_ptr[index].mnc &&
       TRUE == mmgsdi_eons_sys_mode_network_type_match(sys_mode,
                                                       mmgsdi_se13_addl_lang_network_table_ptr[index].network_type_mask))
    {
      /* Copy the contents from hardcoded table */
      plmn_info_ptr->plmn_additional_names[plmn_info_ptr->num_plmn_additional_names].plmn_lang.lang_len =
        strlen(mmgsdi_se13_addl_lang_network_table_ptr[index].lang_id);
      mmgsdi_memscpy(plmn_info_ptr->plmn_additional_names[plmn_info_ptr->num_plmn_additional_names].plmn_lang.lang_id,
                     MMGSDI_LANG_ID_LEN_MAX,
                     mmgsdi_se13_addl_lang_network_table_ptr[index].lang_id,
                     plmn_info_ptr->plmn_additional_names[plmn_info_ptr->num_plmn_additional_names].plmn_lang.lang_len);
      plmn_info_ptr->plmn_additional_names[plmn_info_ptr->num_plmn_additional_names].plmn_short_name.plmn_name.eons_data.data_len =
        mmgsdi_se13_addl_lang_network_table_ptr[index].short_name_ucs2_len;
      plmn_info_ptr->plmn_additional_names[plmn_info_ptr->num_plmn_additional_names].plmn_short_name.plmn_name.eons_data.data_ptr =
        mmgsdi_se13_addl_lang_network_table_ptr[index].short_name_ucs2;
      plmn_info_ptr->plmn_additional_names[plmn_info_ptr->num_plmn_additional_names].plmn_short_name.plmn_name.eons_encoding =
      MMGSDI_EONS_ENC_UCS2;
      plmn_info_ptr->plmn_additional_names[plmn_info_ptr->num_plmn_additional_names].plmn_long_name.plmn_name.eons_data.data_len =
        mmgsdi_se13_addl_lang_network_table_ptr[index].full_name_ucs2_len;
      plmn_info_ptr->plmn_additional_names[plmn_info_ptr->num_plmn_additional_names].plmn_long_name.plmn_name.eons_data.data_ptr =
        mmgsdi_se13_addl_lang_network_table_ptr[index].full_name_ucs2;
      plmn_info_ptr->plmn_additional_names[plmn_info_ptr->num_plmn_additional_names].plmn_long_name.plmn_name.eons_encoding =
      MMGSDI_EONS_ENC_UCS2;

      plmn_info_ptr->num_plmn_additional_names = plmn_info_ptr->num_plmn_additional_names + 1;
    }
  }

  return TRUE;
}/* mmgsdi_eons_get_se13_addl_lang_table_ons */
