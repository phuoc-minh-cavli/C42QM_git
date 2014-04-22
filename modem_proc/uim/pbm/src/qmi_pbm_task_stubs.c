/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     Q M I _ PBM _ T A S K _ S T U B S. C

DESCRIPTION
  This is the source file for the QMI PBM Task. Contained herein are the
  stub functions needed to initialize all the modules that execute in QMI task
  context and the main task processing loop.

EXTERNALIZED FUNCTIONS
  qmi_pbm_task()
    QMI Task entry point and main processing loop.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None of the QMI services can be used unless MC has created the QMI task
  and qmi_pbm_task() has been invoked.

Copyright (c) 2016 QUALCOMM Technologies, Inc(QTI).
All Rights Reserved.
QUALCOMM Technologies Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/uim.mpss/3.3.1/pbm/src/qmi_pbm_task_stubs.c#1 $ $DateTime: 2020/07/22 10:53:27 $ $Author: pwbldsvc $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/05/16    nr     Intial version.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */
#include "task.h"


/*===========================================================================

  FUNCTION qmi_pbm_init_before_task_start

  DESCRIPTION
    This is stub function for qmi_pbm_init_before_task_start ().

  DEPENDENCIES
    none

  RETURN VALUE
    none

  SIDE EFFECTS
    none

 ===========================================================================*/
void qmi_pbm_init_before_task_start(
  void
)
{
  return;
} /* qmi_pbm_init_before_task_start */


/*===========================================================================
FUNCTION QMI_PBM_TASK()

DESCRIPTION
  This is stub function for qmi_pbm_task ().

DEPENDENCIES
  Does not Return.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void qmi_pbm_task
(
  uint32 ignored    /* Parameter received from Main Control task - ignored */
)
{
  (void)ignored;

  rcinit_handshake_startup();
} /* qmi_pbm_task() */

