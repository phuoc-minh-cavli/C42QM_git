#ifndef UIM_COMMON_LIB_H
#define UIM_COMMON_LIB_H
/*===========================================================================


           U I M   C O M M O N   L I B R A R Y   H E A D E R S


GENERAL DESCRIPTION

This is the header file that provides definition for routines that
can be called by all UIM modules.



EXTERNALIZED FUNCTIONS

uim_common_get_modem_rat_capabilities
  Retrieves the modem tech capability (i.e. RATs supported)


INITIALIZATION AND SEQUENCING REQUIREMENTS

===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2016 - 2017 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/uim.mpss/3.3.1/common/inc/uim_common_lib.h#1 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
06/07/17   me      Added support for the CIOT LTE EFS with CAT1 and NB1 values 
07/27/16   shr     Initial version

=============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/

#include "comdef.h"

/*=============================================================================

                       DATA DECLARATIONS

=============================================================================*/

/* Masks to use to interpret uim_common_get_modem_rat_capabilities()
   return value */
#define UIM_MODEM_CAP_FEATURE_GSM_MASK              0x00000001
#define UIM_MODEM_CAP_FEATURE_1X_MASK               0x00000002
#define UIM_MODEM_CAP_FEATURE_WCDMA_MASK            0x00000004
#define UIM_MODEM_CAP_FEATURE_HSPA_MASK             0x00000008
#define UIM_MODEM_CAP_FEATURE_TDSCDMA_MASK          0x00000010
#define UIM_MODEM_CAP_FEATURE_LTE_MASK              0x00000020
#define UIM_MODEM_CAP_FEATURE_LTE_M1_MASK           0x00000040
#define UIM_MODEM_CAP_FEATURE_LTE_NB1_MASK          0x00000080


/*=============================================================================

                           FUNCTION DECLARATIONS

=============================================================================*/
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
);

#endif /* UIM_COMMON_LIB_H */
