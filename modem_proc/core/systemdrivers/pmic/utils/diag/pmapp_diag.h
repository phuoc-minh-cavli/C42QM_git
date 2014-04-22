#ifndef PM_APP_DIAG_H
#define PM_APP_DIAG_H

/** @file pmapp_diag.h
 *
 *  PMIC-DIAG
 *  Packet definitions between the diagnostic subsystem
 *  and the external device.
 *
 *  !!! NOTE: All member structures of DIAG packets must be PACKED.
 * 
 *  !!! WARNING: Each command code number is part of the externalized
 *  diagnostic command interface.  This number *MUST* be assigned
 *  by a member of QCT's tools development team.
 */
/* Copyright (c) 2004-2005, 2009, 2011, 2013 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/*===========================================================================

                             Edit History

  $Header: //components/rel/core.mpss/3.10/systemdrivers/pmic/utils/diag/pmapp_diag.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/13/11   hs      Changed PACKED struct to PACK(struct) to work with Q6 compiler.
02/08/11   hw      Merging changes from the PMIC Distributed Driver Arch branch
04/24/09   wra     Added the pm_app_set_mode diag message definition.
08/25/05   Frank   Added diag lib.
04/05/04   rmd     Featurized PMIC DIAG services under 
                   "FEATURE_PMIC_DIAG_SERVICES".
03/24/04   rmd     Updated file to follow PMIC VU coding standards.
02/29/04   st      Created file.
===========================================================================*/


#include "comdef.h"
#include "diagcmd.h"
#include "diagpkt.h"

#if defined(T_WINNT)
   #error code not present
#endif


/*==========================================================================
  
                  SUBSYSTEM PACKET CODE DEFINITIONS
   
============================================================================*/

/** @addtogroup pmapp_diag
@{ */

#define PM_APP_DIAG_PEEK_POKE   0

/*===========================================================================

FUNCTION PM_APP_DIAG_INIT
===========================================================================*/

/**
  Registers the diagnostic packet function dispatch table.

@return
 None.
*/
void pm_app_diag_init (void);

/** @} */ /* end_addtogroup pmapp_diag */

#endif /* PM_APP_DIAG_H */

