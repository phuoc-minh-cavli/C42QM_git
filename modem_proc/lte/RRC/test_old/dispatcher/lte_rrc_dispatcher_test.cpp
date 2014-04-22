/*!
  @file lte_rrc_dispatcher_test.cpp

  @brief
  Unit test code for the RRC dispatcher

  @detail
  Remaining tests
  - Check to see if umids sent to the dispatcher are received by the dispatcher
    (tests that the registration with msgr part succeeded)
  - 

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

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/dispatcher/lte_rrc_dispatcher_test.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/31/08   ask     Ported RRC to POSIX APIs
09/02/08   ask     Using UTF. Changed test1 to check for duplicate umids.
08/23/08   ahe     Re-arranged header files for GSSAA build.
07/23/08   ask     Initial Revision
           
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
extern "C"
{
#include <msgr.h>

#include "lte_rrc_utils.h"
#include "lte_rrc_dispatcher.h"
#include "lte_static_rrc_pendq.h"
#include "__lte_rrc_paging.h"
#include "lte_rrc_dispatcheri.h"
#include "lte_static_rrc_dispatcher.h"
//#include "lte_static_rrc_dispatcheri.h"


}

#include "TestFramework.h"
#include "lte_rrc_utf.h"

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief Typedef of variables internal to module lte_rrc_dispatcher_test.cpp
*/
typedef struct
{
  uint32 callback_count;
  boolean callback_return;

} lte_rrc_dispatcher_test_s;


typedef struct
{
  msgr_hdr_s        hdr;
  int               dont_care;

} lte_rrc_dispatcher_test_dummy_msg_s;

/*===========================================================================

                         INTERNAL VARIABLES

===========================================================================*/

/*! @brief Variables internal to module lte_rrc_dispatcher_test.cpp
*/
STATIC lte_rrc_dispatcher_test_s lte_rrc_dispatcher_test;


extern "C"
{
  extern lte_rrc_dispatcher_s lte_rrc_dispatcher;
  extern lte_rrc_dispatcher_dispatch_entry_s lte_rrc_dispatcher_dispatch_table[];
  extern int lte_rrc_dispatcher_dispatch_table_cnt;

  extern msgr_umid_type lte_static_rrc_dispatcher_external_umid_list[];
  extern msgr_umid_type lte_static_rrc_dispatcher_internal_umid_list[];
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

// Test that the dispatch table has no duplicates
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcDispatcherNoDuplicates);

void LteRrcDispatcherNoDuplicates::Setup()
{
  lte_rrc_utf_setup_task();
}

void LteRrcDispatcherNoDuplicates::Test()
{
  uint32 cnt = lte_rrc_dispatcher_dispatch_table_cnt;
  uint32 i, j;

  for (i = 0; i < cnt; i++)
  {
    for (j = i+1; j < cnt; j++)
    {
      TF_ASSERT(lte_rrc_dispatcher_dispatch_table[i].msg_id != 
                lte_rrc_dispatcher_dispatch_table[j].msg_id);
    }
  }
}

void LteRrcDispatcherNoDuplicates::Teardown()
{
  lte_rrc_utf_teardown_task();
}

#ifdef RRC_MOB_INCLUDE
// Test that dispatch hash table can be used to lookup every UMID
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcDispatcherHashLookup);

void LteRrcDispatcherHashLookup::Setup()
{
  lte_rrc_utf_setup_task();
}

void LteRrcDispatcherHashLookup::Test()
{
  uint32 cnt;
  uint32 i;
  lte_rrc_dispatcher_dispatch_entry_s *ht_entry;

  // Validate the ht for all external umids
  cnt = lte_rrc_dispatcher.external_mb_umid_cnt;
  for (i = 0; i < cnt; i++)
  {
    ht_entry = NULL;

    TF_MSG("Looking up external msg umid: 0x%x", lte_static_rrc_dispatcher_external_umid_list[i]);

    // When we get to the SPR messages, we can quit, because those are
    // not in the dispatch table.
    if (MSGR_TYPE_VAL(lte_static_rrc_dispatcher_external_umid_list[i]) == MSGR_TYPE_SPR)
    {
      break;
    }

    // Lookup the umid in the hash table
    TF_ASSERT(lte_rrc_dispatcher_ht_lookup(lte_static_rrc_dispatcher_external_umid_list[i],
                                        &ht_entry) == TRUE);

    // Validate that the dispatch entry looks good
    TF_ASSERT(ht_entry != NULL);
    TF_ASSERT(ht_entry->msg_id == lte_static_rrc_dispatcher_external_umid_list[i]);
    TF_ASSERT(ht_entry->num_sms > 0);
    TF_ASSERT(ht_entry->num_sms <= LTE_RRC_DISPATCHER_MAX_NUM_SM_PER_MSGID);
  }

  // Validate the ht for all internal umids
  cnt = lte_rrc_dispatcher.internal_mb_umid_cnt;
  for (i = 0; i < cnt; i++)
  {
    ht_entry = NULL;

    TF_MSG("Looking up internal msg umid: 0x%x", lte_static_rrc_dispatcher_internal_umid_list[i]);

    // Lookup the umid in the hash table
    TF_ASSERT(lte_rrc_dispatcher_ht_lookup(lte_static_rrc_dispatcher_internal_umid_list[i],
                                        &ht_entry) == TRUE);

    // Validate that the dispatch entry looks good
    TF_ASSERT(ht_entry != NULL);
    TF_ASSERT(ht_entry->msg_id == lte_static_rrc_dispatcher_internal_umid_list[i]);
    TF_ASSERT(ht_entry->num_sms > 0);
    TF_ASSERT(ht_entry->num_sms <= LTE_RRC_DISPATCHER_MAX_NUM_SM_PER_MSGID);
  }
}

void LteRrcDispatcherHashLookup::Teardown()
{
  lte_rrc_utf_teardown_task();
}
#endif

// Test the pending queue functionality
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcDispatcherPendingQueue);

boolean LteRrcDispatcherPendingQueue__Callback(byte *msg_ptr, uint32 msg_len)
{
  lte_rrc_dispatcher_test_s *priv = &lte_rrc_dispatcher_test;

  TF_ASSERT(msg_ptr != NULL);

  // Increment the callback count
  priv->callback_count++;

  return priv->callback_return;
}

void LteRrcDispatcherPendingQueue::Setup()
{
  // We use the paging state machine, because we will ultimately send a msg to paging
  lte_rrc_utf_setup(LTE_RRC_PAGING_SM, NULL, 0);
}

void LteRrcDispatcherPendingQueue::Test()
{
  lte_rrc_dispatcher_test_s *priv = &lte_rrc_dispatcher_test;
  lte_rrc_not_camped_indi_s not_camped_indi;
  lte_rrc_dispatcher_test_dummy_msg_s lte_rrc_dispatcher_test_dummy_msg;
  msgr_hdr_s *msg_ptr;
  uint32 msg_len;
  lte_static_rrc_pendq_queue_item_s *item_ptr;
  int i = 0;

  // Things are steady, pending queue should be empty..
  priv->callback_count = 0;
  priv->callback_return = FALSE;

  // Now insert an item into the pending queue
  lte_static_rrc_pendq_put((msgr_hdr_s*) &lte_rrc_dispatcher_test_dummy_msg, 
                    sizeof(lte_rrc_dispatcher_test_dummy_msg),
                    LteRrcDispatcherPendingQueue__Callback);

  TF_SLEEP(10);

  // Without a stimulus, the callback should never be called.
  TF_ASSERT(priv->callback_count == 0);

  // Insert a good message to the dispatcher
  lte_rrc_init_default_hdr(&not_camped_indi.msg_hdr, LTE_RRC_NOT_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&not_camped_indi, sizeof(not_camped_indi));

  lte_rrc_utf_wait_for_done();
  TF_SLEEP(10);
  
  // We expect the callback to have been called once..
  TF_ASSERT(priv->callback_count == 1);

  // Call the pendq get routine directly
  item_ptr = lte_static_rrc_pendq_get(&msg_ptr, &msg_len);
  TF_ASSERT(item_ptr == NULL);

  // We expect the callback to have been called twice..
  TF_ASSERT(priv->callback_count == 2);

  // Change the call return status and see if that gets returned correctly
  priv->callback_return = TRUE;

  // Call the pendq get routine directly
  item_ptr = lte_static_rrc_pendq_get(&msg_ptr, &msg_len);
  TF_ASSERT(item_ptr != NULL);

  // We expect the callback to have been called thrice..
  TF_ASSERT(priv->callback_count == 3);

  // Free the queue item
  lte_static_rrc_pendq_free(item_ptr);

  // Call the pendq get routine directly
  item_ptr = lte_static_rrc_pendq_get(&msg_ptr, &msg_len);

  // Nothing should be returned since the queue should be empty
  TF_ASSERT(item_ptr == NULL);

  // We expect the callback to have not been called
  TF_ASSERT(priv->callback_count == 3);
  
  // To test the reset routine, we add an item on the list..
  // and then call reset, and make sure that the queue is empty..
  
  // Things are steady, pending queue should be empty..
  priv->callback_count = 0;
  priv->callback_return = TRUE;

  // Now insert an item into the pending queue
  lte_static_rrc_pendq_put((msgr_hdr_s*) &lte_rrc_dispatcher_test_dummy_msg, 
                    sizeof(lte_rrc_dispatcher_test_dummy_msg),
                    LteRrcDispatcherPendingQueue__Callback);

  TF_SLEEP(10);

  // Without a stimulus, the callback should never be called.
  TF_ASSERT(priv->callback_count == 0);

  // Reset the pending queue
  lte_static_rrc_pendq_reset();
  
  // Call the pendq get routine directly
  item_ptr = lte_static_rrc_pendq_get(&msg_ptr, &msg_len);

  // Nothing should be returned since the queue should be empty
  TF_ASSERT(item_ptr == NULL);

  // We expect the callback to have not been called
  TF_ASSERT(priv->callback_count == 0);
  
  // Make sure the pending queue is adding/deleting nodes between
  // the Pend and Free queue correctly.
  for ( i= 0; i < 100; i++)
  {
    lte_static_rrc_pendq_put((msgr_hdr_s*) &lte_rrc_dispatcher_test_dummy_msg, 
                      sizeof(lte_rrc_dispatcher_test_dummy_msg),
                      LteRrcDispatcherPendingQueue__Callback);

    item_ptr = lte_static_rrc_pendq_get(&msg_ptr, &msg_len);
    TF_ASSERT(item_ptr != NULL);
  }

  // Now fill up the pending queue
  for ( i= 0; i < 10; i++)
  {
    lte_static_rrc_pendq_put((msgr_hdr_s*) &lte_rrc_dispatcher_test_dummy_msg, 
                      sizeof(lte_rrc_dispatcher_test_dummy_msg),
                      LteRrcDispatcherPendingQueue__Callback);  
  }

  // Reset the pending queue
  lte_static_rrc_pendq_reset();
  
  // Call the pendq get routine directly
  item_ptr = lte_static_rrc_pendq_get(&msg_ptr, &msg_len);

  // Nothing should be returned since the queue should be empty
  TF_ASSERT(item_ptr == NULL);

  // Now fill up the pending queue again
  for ( i= 0; i < 10; i++)
  {
    lte_static_rrc_pendq_put((msgr_hdr_s*) &lte_rrc_dispatcher_test_dummy_msg, 
                      sizeof(lte_rrc_dispatcher_test_dummy_msg),
                      LteRrcDispatcherPendingQueue__Callback);
  }

  // Get all the pended items out
  for ( i= 0; i < 10; i++)
  {
    
    item_ptr = lte_static_rrc_pendq_get(&msg_ptr, &msg_len);
    TF_ASSERT(item_ptr != NULL);
  }
  
  // Call the pendq get routine one more time
  item_ptr = lte_static_rrc_pendq_get(&msg_ptr, &msg_len);

  // Nothing should be returned since the queue should be empty
  TF_ASSERT(item_ptr == NULL);


}

void LteRrcDispatcherPendingQueue::Teardown()
{
  lte_rrc_utf_teardown();
}


