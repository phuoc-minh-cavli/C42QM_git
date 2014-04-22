#ifndef MMGSDI_EONS_CDMA_H
#define MMGSDI_EONS_CDMA_H
/*===========================================================================


                  M M G S D I   E O N S   C D M A   H E A D E R


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2013, 2015 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.  
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/uim.mpss/3.3.1/mmgsdi/src/mmgsdi_eons_cdma.h#1 $$ $DateTime: 2020/07/22 10:53:27 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
03/09/16   vdc     Featurize eons cdma code with FEATURE_MMGSDI_3GPP2
07/15/13   vs      EONS updates to support single baseband SGLTE

=============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/
#include "uim_variation.h"

#ifdef FEATURE_MMGSDI_3GPP2
#include "comdef.h"
#include "mmgsdi.h"

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/


/*=============================================================================

                       FUNCTION PROTOTYPES

=============================================================================*/

/*===========================================================================
  FUNCTION MMGSDI_EONS_UPDATE_CDMA_SERVING_SYS_INFO

DESCRIPTION
  Function to update serving system related info. 

DEPENDENCIES
  This function can only be called if there is an SS event trigger

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
void mmgsdi_eons_update_cdma_serving_sys_info(
  mmgsdi_eons_info_type   * eons_info_ptr
);

/*===========================================================================
  FUNCTION MMGSDI_EONS_RUN_ALGO_FOR_CDMA

DESCRIPTION
  Function to run algorithm for 1x primary or 1x secondary session ids.

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_eons_run_algo_for_cdma(
  mmgsdi_session_id_type                         session_id,
  mmgsdi_eons_info_type                        * eons_info_ptr,
  mmgsdi_app_enum_type                           app_type,
  mmgsdi_eons_ignore_disp_cond_enum_type         ignore_display_condition,
  mmgsdi_eons_name_type                        * spn_ptr,
  uint8                                        * lang_ind_ptr,
  mmgsdi_eons_display_info_type                * display_info_ptr
);
#endif /* FEATURE_MMGSDI_3GPP2 */
#endif /* MMGSDI_EONS_CDMA_H */
