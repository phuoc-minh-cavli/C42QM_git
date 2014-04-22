/*======================================================================
                     DSR_Framework.c

GENERAL DESCRIPTION
  DSR (Deferred Service Routine)

 EXTERNALIZED FUNCTIONS


 INITIALIZATION AND SEQUENCING REQUIREMENTS
  Clients that require deferred service routines must register using DSR APIs

 Copyright (c) 2015-2016  by Qualcomm Technologies, Inc.  All Rights Reserved.
 ======================================================================*/
/*======================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *
 *
 *
 ======================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "dsr.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define DSR_ISR_NUM			5
#define DSR_WORKLOOP_NUM	3
#define DSR_PRI_MEDIUM_PLUS	5

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
typedef struct
{
   dsr_obj_t *dsr;
   uint32 count;
   dsr_priority_t priority_from_cb;
} dsr_test_ctxt_t;

/*----------------------------------------------------------------------------
 * Static Function Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/
dsr_test_ctxt_t dsr_isr_test_ctxt[DSR_ISR_NUM] = {0};
dsr_test_ctxt_t dsr_wklp_test_ctxt[DSR_WORKLOOP_NUM] = {0};

dsr_priority_t dsr_isr_test_priorities[DSR_ISR_NUM] =
{
  DSR_PRI_LOW,
  DSR_PRI_HIGH,
  DSR_PRI_MEDIUM,
  DSR_NUM_PRIO,
  DSR_PRI_HIGH,
};

dsr_priority_t dsr_wklp_test_priorities[DSR_WORKLOOP_NUM] =
{
  DSR_PRI_LOW,
  DSR_PRI_HIGH,
  DSR_PRI_MEDIUM,
};

uint32 dsr_test_create_error_count = 0;
uint32 dsr_test_enqueue_error_count = 0;

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/
static void dsr_test_handler(dsr_obj_t *dsr_obj, void *ctxt, void *payload)
{
  dsr_test_ctxt_t *test_ctxt = (dsr_test_ctxt_t *)ctxt;
  dsr_priority_t *priority = (dsr_priority_t *)payload;

  test_ctxt->count++;
  test_ctxt->priority_from_cb = *priority;
}

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/
void dsr_unit_test(void)
{
  dsr_test_ctxt_t *ctxt;
  boolean succeeded;
  uint32 i;

  for (i = 0; i < DSR_ISR_NUM ; i++)
  {
	ctxt = &dsr_isr_test_ctxt[i];
	
	if(i == DSR_ISR_NUM - 1)
	{
		succeeded = dsr_create(&ctxt->dsr,
                           dsr_test_handler,
                           (void *)ctxt,
                           dsr_isr_test_priorities[i],
						   DSR_CONTEXT_INVALID);
	}
    else
	{
		succeeded = dsr_create(&ctxt->dsr,
                           dsr_test_handler,
                           (void *)ctxt,
                           dsr_isr_test_priorities[i],
						   DSR_CONTEXT_ISR);
	}
    if (!succeeded)
    {
      dsr_test_create_error_count++;
	  ctxt->dsr = NULL;
    }
  }
  
  for (i = 0; i < DSR_WORKLOOP_NUM; i++)
  {
    ctxt = &dsr_wklp_test_ctxt[i];
    succeeded = dsr_create(&ctxt->dsr,
                           dsr_test_handler,
                           (void *)ctxt,
                           dsr_wklp_test_priorities[i],
						   DSR_CONTEXT_WORKLOOP);
    if (!succeeded)
    {
      dsr_test_create_error_count++;
    }
  }

  for (i = 0; i < DSR_ISR_NUM; i++)
  {
    ctxt = &dsr_isr_test_ctxt[i];
    succeeded = dsr_enqueue(ctxt->dsr,
                            (void *)&dsr_isr_test_priorities[i]);
    if (!succeeded)
    {
      dsr_test_enqueue_error_count++;
    }
  }
  
  for (i = 0; i < DSR_WORKLOOP_NUM; i++)
  {
    ctxt = &dsr_wklp_test_ctxt[i];
    succeeded = dsr_enqueue(ctxt->dsr,
                            (void *)&dsr_wklp_test_priorities[i]);
    if (!succeeded)
    {
      dsr_test_enqueue_error_count++;
    }
  }
}

