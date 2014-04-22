
#ifndef PMN_IO_H
#define PMN_IO_H

/* =========================================================================

DESCRIPTION
  Routines related to management and manipulation of the ARM11 performance
  counters.

  pmn_init        - Initializes the performance counters
  pmn_reset       - Resets the performance counters to a default state
  pmn_start       - Starts the performance counters
  pmn_stop        - Stops the performance counters
  pmn_read_cnt    - Reads the performance counters

INITIALIZATION AND SEQUENCING REQUIREMENTS
  The init_pmn() needs to be called before any other calls are
  made to this module.
  Before calling read_pmn_cnt() the pmn needs to be stoped by calling stop_pmn()
  or else the reading will not be coherent.

Copyright (c) 2008 by Qualcomm Technologies Incorporated.  All Rights Reserved.
============================================================================ */

/* =========================================================================

                      EDIT HISTORY FOR FILE

 This section contains comments describing changes made to this file.
 Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/api/kernel/perfmon_counters/cortexa7/pmn_io.h#1 $
$DateTime: 2020/01/30 22:49:35 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------
03/24/08   lk      Initial file.

============================================================================ */

/*============================================================================
                          INCLUDE FILES
============================================================================*/

#include "comdef.h"


typedef uint32 pmn_reg_val_type;

typedef enum
{
    // Performance Monitor Control Register
    PMN_REGID_PMCR = 0x0,

    // Count Enable Set Register
    PMN_REGID_PMCNTENSET,

    // Count Enable Clear Register
    PMN_REGID_PMCNTENCLR,

    // Overflow Flag Status Register
    PMN_REGID_PMOVSR,

    // Software Increment Register
    PMN_REGID_PMSWINC,

    // Event Counter Selection Register
    PMN_REGID_PMSELR,

    // Common Event Identification Register
    PMN_REGID_PMCEID0,

    // Common Event Identification Register
    PMN_REGID_PMCEID1,

    // Cycle Count Register
    PMN_REGID_PMCCNTR,

    // Event Type Select Register
    PMN_REGID_PMXEVTYPER,

    // Cycle Count Filter Control Register
    PMN_REGID_PMCCFILTR,

    // Event Count Registers
    PMN_REGID_PMXEVCNTR,

    // User Enable Register
    PMN_REGID_PMUSERENR,

    // Interrupt Enable Set REgister
    PMN_REGID_PMINTENSET,

    // Interrupt Enable Clear Register
    PMN_REGID_PMINTENCLR
}pmn_regid_type;

/*===========================================================================

FUNCTION pmn_read_reg

DESCRIPTION
  Reads the pmn register specified by the parameter and returns it.

DEPENDENCIES
  None

RETURN VALUE
  Returns the value of the read register.

SIDE EFFECTS
  None

ARGUMENTS
  reg_id enumerated type specifying the register to be read.  
===========================================================================*/
pmn_reg_val_type
pmn_read_reg
(
  const pmn_regid_type reg_id
);

/*===========================================================================

FUNCTION pmn_write_reg

DESCRIPTION
  Writes to the pmn register specified by the reg_id parameter the value
  specified in the reg_val parameter.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

ARGUMENTS
  reg_id enumerated type specifying the register to be written.
  reg_val value to be written into the specified register.  
===========================================================================*/
void pmn_write_reg
(
  const pmn_regid_type   reg_id,
  const pmn_reg_val_type reg_val
);


#endif /* PMN_IO_H */
