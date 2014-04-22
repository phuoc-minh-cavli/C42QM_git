/*!
  @file lte_rrc_task_test.cpp

  @brief
  Unit test code for LTE RRC task

*/

/*===========================================================================

  Copyright (c) 2008 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/task/lte_rrc_task_test.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/31/08   ask     Ported RRC to POSIX APIs
09/10/08   ask     Added code to create/delete timer thread on test startup/teardown
08/22/08   ahe     Changed name of includes to compile for gssaa.
07/03/08   ask     Initial Revision
           
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

extern "C"
{
#include <stdio.h>
#include <stdlib.h>

#include <msgr.h>
#include <lte_app.h>
#include <appmgr.h>

//#include "lte_rrc_task.h"
//#include "lte_rrc_taski.h"
#include "lte_static_rrc_task.h"
//#include "lte_static_rrc_taski.h"
}

#include "TestFramework.h"


/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

typedef struct
{
  int placeholder;

} lte_rrc_task_test_s;


typedef struct
{
  msgr_hdr_s    hdr;
  int           cookie;

} lte_rrc_task_test_loopback_msg;

/*===========================================================================

                         INTERNAL VARIABLES

===========================================================================*/

/*extern "C"
{
extern lte_static_rrc_task_s lte_rrc_task;
}*/

/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/



/*===========================================================================

                                FUNCTIONS

===========================================================================*/

/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcTaskTest1);

void LteRrcTaskTest1::Setup()
{
  // Create a msgr client and test message queue
  InitTestResources();

  //lte_rrc_task.task_is_initialized = FALSE;

  // Create and init the RRC thread
  lte_app_test_spawn_thread("RRC", MSGR_LTE_RRC, lte_static_rrc_task_init,
                            1);

  // Start the RRC thread
  lte_app_test_thread_run();
  
}

void LteRrcTaskTest1::Test()
{
  while (lte_static_rrc_task_is_initialized() == FALSE);
  //while (lte_rrc_task.task_is_initialized == FALSE);
}

void LteRrcTaskTest1::Teardown()
{
  // Stop the RRC thread
  lte_app_test_teardown();

  // Remove the test resources (including stopping all threads)
  TeardownTestResources();  
}
