#ifndef __DSR_H__
#define __DSR_H__
/*============================================================================
  @file dsr.h

  Function and data structure declarations for DSRs (Deferred Service
  Routines).


                Copyright (c) 2016 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
/* $Header: //components/rel/core.tx/6.0/api/products/dsr/dsr.h#1 $ */

/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/
#include "com_dtypes.h"

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ----------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
 * Type Declarations
 * ----------------------------------------------------------------------*/
/* Priority definitions for DSR objects */
typedef enum
{
  DSR_PRI_LOW,
  DSR_PRI_MEDIUM,
  DSR_PRI_HIGH,
  DSR_NUM_PRIO
} dsr_priority_t;

/* Differentiates whether the DSR is for a ISR or Workloop */
typedef enum
{
	DSR_CONTEXT_ISR,
	DSR_CONTEXT_WORKLOOP,
	DSR_CONTEXT_INVALID
}dsr_context_t;

/* DSR Object type */
typedef struct dsr_obj_s dsr_obj_t;

/* DSR callback handler type */
typedef void (*dsr_hdlr_t)(dsr_obj_t *dsr_obj, void *ctxt, void *payload);

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ----------------------------------------------------------------------*/

/**
 * @brief Creates a DSR object
 *
 * @details This function creates a DSR object that can be enqueued later to
 *          defer work to thread context.
 *
 * @param[out] dsr_obj    DSR object
 * @param[in]  cb         Callback function invoked in DSR thread context
 * @param[in]  cb_ctxt    Optional callback context; can be NULL
 * @param[in]  priority   DSR priority
 * @param[in]  context    Context depicting DSR for ISR or Workloop 
 *
 * @return TRUE if successfully created; otherwise, FALSE
 */
boolean dsr_create(dsr_obj_t **dsr_obj, dsr_hdlr_t cb, void *cb_ctxt, dsr_priority_t priority, dsr_context_t context);

/**
 * @brief Destroys a DSR object
 *
 * @details This function destroys a DSR object. If the DSR is currently
 *          queued then it will be removed from the queue (callback function
 *          will not get invoked).
 *
 * @param[in] dsr_obj    DSR object
 *
 * @return TRUE if DSR object is successfully freed
 */
boolean dsr_destroy(dsr_obj_t *dsr_obj);

/**
 * @brief Enqueues a DSR object
 *
 * @details This function enqueues a DSR object into the DSR pending list.
 *          Worker threads process the pending list based on DSR priority.
 *
 *          Once a DSR is enqueued, it cannot be enqueued again (an object
 *          can only be inserted into a list once). In cases where drivers
 *          may attempt to enqueue a DSR already in the pending list, e.g.
 *          handling interrupts, DSR context should be maintained between
 *          the ISR and DSR.
 *
 *          Do not make blocking calls / wait on events in DSR context.
 *
 *          If this function returns FALSE then it was not enqueued. If
 *          the DSR is already in the pending list, this function will
 *          return FALSE and the payload will not be updated.
 *
 * @param[in] dsr_obj    DSR object
 * @param[in] payload    Optional payload context; can be NULL
 *
 * @return TRUE if successfully enqueued; otherwise, FALSE
 */
boolean dsr_enqueue(dsr_obj_t *dsr_obj, void *payload);

/**
 * @brief Dequeues a DSR object
 *
 * @details This function dequeues a DSR object from the DSR pending list.
 *          If this function returns FALSE then it was not dequeued. 
 *
 * @param[in] dsr_obj    DSR object
 *
 * @return TRUE if successfully dequeued; otherwise, FALSE
 */
boolean dsr_dequeue(dsr_obj_t *dsr_obj);

#endif /* #ifndef __DSR_H__ */

