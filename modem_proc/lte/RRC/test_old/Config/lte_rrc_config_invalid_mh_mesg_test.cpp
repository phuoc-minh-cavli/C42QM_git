/*!
  @file lte_rrc_config_invalid_mh_mesg_test.cpp

  @brief
  Unit test code for the Config procedure that tests the processing of 
  an invalid message from the Message Handler.

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

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/24/08   mm     Initial Revision
           
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
extern "C"
{
#include "lte_rrc_ext_msg.h"
#include "lte_rrc_int_msg.h"
#include "lte_rrc_utils.h"
#include "__lte_rrc_config.h"
#include "lte_rrc_configi.h"
#include "lte_static_rrc_mem_util.h"
#include "lte_rrc_dispatcher.h"
#include "__lte_rrc_config_int.h"
#include "lte_rrc_controller.h"
}

#include "TestFramework.h"
#include "lte_rrc_utf.h"
#include "lte_rrc_config_test_common.h"
#include <tf_stub.h> 

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*===========================================================================

                         INTERNAL VARIABLES

===========================================================================*/

extern "C"
{
  TF_PROTO_STUB( stm_state_t, lte_rrc_controller_get_state, void);
}

/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/



/*===========================================================================

                                FUNCTIONS

===========================================================================*/



/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcConfigInvalidMHMesgTest);

void LteRrcConfigInvalidMHMesgTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_CONFIG_SM, NULL, 0);
  TF_STUB(lte_rrc_controller_get_state) = lte_rrc_config_test_get_controller_state;
}

void LteRrcConfigInvalidMHMesgTest::Test()
{
  lte_rrc_mh_dlm_s dl_msg;
  void *payload_ptr = NULL;
  /* ---------------------------------------------------------------------- */

  TF_MSG("\nReconfig: Invalid MH Message Test Begins \n");
  /* Setting the header for internal RRC messages */
  lte_rrc_init_default_hdr(&dl_msg.msg_hdr,LTE_RRC_RRC_CONNECTION_RECONFIGURATION_DLM);
  dl_msg.dl_msg_ptr = NULL;
  /* Send the decoded MH msg containing the Reconfig message to Config module 
    This is accomplished by calling the config mesg handler function in Config
    module. */
  /* Since Security configuration is present without the mobility info being
     present, this message should be flagged as invalid and would cause Config 
     module to ASSERT */
  payload_ptr = lte_rrc_utf_get_payload_ptr((byte *)&dl_msg,sizeof(dl_msg));
  TF_CATCH_FATAL(lte_rrc_reconfig_dlm_handler(LTE_RRC_CONFIG_SM,payload_ptr));
  TF_MSG("\nReconfig: Invalid MH Message Test Ends \n");
  lte_rrc_utf_reset(); 

  /* ---------------------------------------------------------------------- */

}

void LteRrcConfigInvalidMHMesgTest::Teardown()
{
  lte_rrc_utf_teardown();
}









