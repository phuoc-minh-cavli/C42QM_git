#ifndef PM_QC_PMIC_H
#define PM_QC_PMIC_H

/** @file pm_qc_pmic.h
*
*  This file contains prototype definitions to support interaction
    with the QUALCOMM Power Management ICs.
*   
*/
/*
*   Copyright (c) 2003, 2011 - 2013 Qualcomm Technologies, Inc.
*   All Rights Reserved.
*   Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
/*===========================================================================


                  P M    H E A D E R    F I L E


===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/api/systemdrivers/pmic/pm_qc_pmic.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/16/12   hs      Removed obsolete files.
02/10/11   hw      Uncomment pmapp_pmic_events.h header
02/08/11   hw      Merging changes from the PMIC Distributed Driver Arch branch
06/28/03   wra     Created as final PMIC library main include file. Named
                   so that it will not have naming collision with other
                   HLOS header files. Most of the contents in this file
                   were formerly in pm.h
===========================================================================*/

#include "comdef.h"

/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/


/*===========================================================================

                        ERROR FLAGS DEFINITIONS

===========================================================================*/



/*===========================================================================

                        PMIC INCLUDE FILES

===========================================================================*/
#include "pm_lib_api.h"         /* interface files for all library APIs */
#include "pmapp_npa.h"          /* Contains the power node architecture support */
#include "pm_pwroff.h"          /* shutdown API*/

/*===========================================================================

                        GENERIC FUNCTION PROTOTYPES

===========================================================================*/

/** @addtogroup pm_qc_pmic
@{ */

/*===========================================================================

FUNCTION pm_init                                EXTERNAL FUNCTION

=============================================================================*/  


/**
   Initializes the PMIC driver for operation. It performs the following tasks: 
    - Detects the PMIC revision.
    - Intitializes PMIC specific resource settings from device configuration.
    - Initializes Comms.
    - Initializes driver for MPSS owned resources.
    - Initializes PAM table.

@return
  None.

@dependencies
   dalsys,smem_log,npa,rpm drives must be initialized before calling pm_init. 

*/
extern void pm_init(void);

/** @} */ /* end_addtogroup pm_qc_pmic */

#endif /* PM_QC_PMIC_H */
