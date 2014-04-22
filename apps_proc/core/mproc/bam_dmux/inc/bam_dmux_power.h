/*=============================================================================

                            BAM DMUX POWER

 BAM Data Multiplexer power.

 Copyright  2016-2017 Qualcomm Technologies, Inc. All Rights Reserved.
 Qualcomm Technologies, Inc. Confidential and Proprietary.

=============================================================================*/

/*=============================================================================

                         EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/mproc/bam_dmux/inc/bam_dmux_power.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/24/16   rv      Initial version.
===========================================================================*/

#ifndef BAM_DMUX_POWER_H
#define BAM_DMUX_POWER_H

#ifdef __cplusplus
extern "C" {
#endif

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "comdef.h"

/*=============================================================================

                         MACRO DECLARATIONS

=============================================================================*/


/*=============================================================================

                         TYPE DEFINITIONS

=============================================================================*/
typedef enum {
  BAM_DMUX_POWER_EVENT_OFF     = 0,
  BAM_DMUX_POWER_EVENT_ON      = 1,
  BAM_DMUX_POWER_EVENT_VOTED   = 2,
  BAM_DMUX_POWER_EVENT_UNVOTED = 3,
  BAM_DMUX_POWER_EVENT_FATAL = 0xDEAD
} bam_dmux_power_event_type;

/*=============================================================================

                     PUBLIC VARIABLE DECLARATIONS

=============================================================================*/


/*=============================================================================

                    PUBLIC FUNCTION DECLARATIONS

=============================================================================*/

/*=============================================================================
  FUNCTION  bam_dmux_power_init
=============================================================================*/
/**
 * Initialization function. Registers callback function with SMSM to receive 
 * the A2 power control updates.
 *
 */
/*===========================================================================*/
void bam_dmux_power_init(void);

/*=============================================================================
  FUNCTION  bam_dmux_power_reset
=============================================================================*/
/**
 * TODO
 *
 */
/*===========================================================================*/
void bam_dmux_power_reset(void);

/*=============================================================================
  FUNCTION  bam_dmux_power_vote
=============================================================================*/
/**
 * Votes for A2 Power. Core layer calls this function.
 * Sets SMSM Power Control bit (if not previously set).
 *
 */
/*===========================================================================*/
boolean bam_dmux_power_vote(void);

/*=============================================================================
  FUNCTION  bam_dmux_power_unvote
=============================================================================*/
/**
 * Unvotes for A2 Power. Core layer calls this function. 
 * Clears SMSM Power Control bit.
 *
 */
/*===========================================================================*/
boolean bam_dmux_power_unvote(void);

#ifdef __cplusplus
}
#endif

#endif /* BAM_DMUX_POWER_H */
