#ifndef MMGSDI_EUICC_H
#define MMGSDI_EUICC_H
/*===========================================================================


           M M G S D I   E U I C C   F U N C T I O N   H E A D E R S


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2019 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/uim.mpss/3.3.1/mmgsdi/src/mmgsdi_euicc.h#1 $$ $DateTime: 2020/07/22 10:53:27 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
08/26/19   sch     Don't do protocol switch if card returns 69 99 for ISDR select
08/22/19   sch     Initial version

=============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/

#include "mmgsdi.h"

/*=============================================================================

                       DATA DECLARATIONS

=============================================================================*/

/*=============================================================================

                       STRUCTURE DECLARATIONS

=============================================================================*/

/*=============================================================================

                       FUNCTION PROTOTYPES

=============================================================================*/

/* ============================================================================
FUNCTION MMGSDI_EUICC_IS_ISDR_FOUND

DESCRIPTION
  This function attemmpts to select ISDR (which is present on an eUICC, but not
  on a non-eUICC).

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
============================================================================*/
mmgsdi_return_enum_type mmgsdi_euicc_is_isdr_found(
  mmgsdi_slot_id_enum_type           slot_id,
  boolean                            *is_isdr_found_ptr
);

/*===========================================================================
FUNCTION MMMGSDI_EUICC_IS_ISDR_AID

DESCRIPTION
  This function is used to find out if the input AID is the ISDR AID or not.

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None
===========================================================================*/
boolean mmgsdi_euicc_is_isdr_aid(
  const mmgsdi_static_data_type *aid_ptr
);

#endif /* MMGSDI_EUICC_H */
