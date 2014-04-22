/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


             M M G S D I   S E 1 3  A D D L  L A N G  T A B L E S T U B S


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

$Header: //components/rel/uim.mpss/3.3.1/mmgsdi/src/mmgsdi_se13_addl_lang_table_stubs.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/22/16   tkl     Stub out additional APIs
05/04/16   vdc     Restructure accessing SE13 additional table

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "customer.h"
#include "mmgsdi_se13_addl_lang_table.h"
#include "mmgsdi_eons.h"

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/*==========================================================================
FUNCTION MMGSDI_EONS_GET_ADDL_LANG_SE13_TABLE_ONS

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
  (void)plmn_info_ptr;
  (void)plmn_id;
  (void)sys_mode;

  return FALSE;
}/* mmgsdi_eons_get_se13_addl_lang_table_ons */
