/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


             M M G S D I   S E 1 3  A D D L  L A N G  T A B L E


GENERAL DESCRIPTION

  This source file contains a hardcoded array of PLMN names in additional
  languages such as Chinese. The names are stored in UNICODE format.


                        COPYRIGHT INFORMATION

Copyright (c) 2013, 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //components/rel/uim.mpss/3.3.1/mmgsdi/src/mmgsdi_se13_addl_lang_table.h#1 $$ $DateTime: 2020/07/22 10:53:27 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
06/22/16   tkl     Stub out additional APIs
05/04/16   vdc     Added feature FEATURE_MMGSDI_SE13_FROM_HARDCODED_ONLY
04/07/16   vdc     Restructure accessing SE13 additional table

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "uim_variation.h"
#include "customer.h"
#include "mmgsdi_eons.h"
#include "mmgsdi_se13.h"

/*=============================================================================

                       DATA DECLARATIONS

=============================================================================*/

/* ------------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SE13_ADDL_LANG_NETWORK_TABLE_DATA_TYPE

   DESCRIPTION:
     SE13 additional language network table which contains the networks,
     country code, network code and name in 16 bit unicode format.
-------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  char*   lang_id; /* Languages ID string */
  uint16  mcc; /* Mobile Network Code */
  uint16  mnc; /* Mobile Country Code */
  uint8   network_type_mask; /* Type of the network */
  uint8   short_name_ucs2_len;
  uint8   short_name_ucs2[MMGSDI_EONS_MAX_UCS2_CODES];
  /* network's short name in 16-bit unicode format */
  uint8   full_name_ucs2_len;
  uint8   full_name_ucs2[MMGSDI_EONS_MAX_UCS2_CODES];
  /* network's full name in 16-bit unicode format */
} mmgsdi_se13_addl_lang_network_table_data_type;

/*=============================================================================

                       FUNCTION PROTOTYPES

=============================================================================*/

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
);
