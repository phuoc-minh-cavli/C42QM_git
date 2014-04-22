/*============================================================================
  FILE:         uim_stubs_supply_voltage_inidcation.c

  OVERVIEW:
   This is a stubs file for uim_supply_voltage_inidcation.c

  EXTERNALIZED FUNCTIONS:

  DEPENDENCIES: N/A

                Copyright (c) 2016 QUALCOMM Technologies, Inc(QTI).
                All Rights Reserved.
                QUALCOMM Technologies Confidential and Proprietary
============================================================================*/

/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

$Header: //components/rel/uim.mpss/3.3.1/uimdrv/src/uim_stubs_supply_voltage_indication.c#1 $
$DateTime: 2020/07/22 10:53:27 $
$Author: pwbldsvc $

when          who        what, where, why
--------      ----       ---------------------------------------------------------
05/19/16      sam        Typo correction
05/16/16      sam        Initial Revision
============================================================================*/
#include "uim_p.h"
#include "uimglobals.h"


/*===========================================================================
FUNCTION UIM_NOTIFY_VOLTAGE_CHANGE_INDICATION

DESCRIPTION
  stubs function for uim_notify_voltage_change_indication
===========================================================================*/
void uim_notify_voltage_change_indication 
(
  uim_instance_global_type     *uim_ptr, 
  uimdrv_qmi_indications_type  indication
)
{
  (void)uim_ptr;
  (void)indication;
}/*uim_notify_voltage_change_indication*/


