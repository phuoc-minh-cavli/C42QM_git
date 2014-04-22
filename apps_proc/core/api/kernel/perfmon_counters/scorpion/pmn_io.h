
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

$Header: //components/rel/core.tx/6.0/api/kernel/perfmon_counters/scorpion/pmn_io.h#1 $
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
    /* =0 PMSELR -- Performance Monitor Select Register */
    PMN_REGID_PMSELR = 0,

    /* =1 PMXEVTYPER -- Performance Monitor Event Type Register */
    PMN_REGID_PMXEVTYPER, 
	
    /* =2 PMCR -- Performance Monitor Control Register */
    PMN_REGID_PMCR,

    /* =3 PMACTLR -- Performance Monitor Auxiliary Control Register */
    PMN_REGID_PMACTLR,  

    /* =4 PMCNTENSET -- Performance Monitor Count Enable Set -- Start the monitors */
    PMN_REGID_PMENTSET,

    /* =5 PMCNTENCLR -- Performance Monitor Count Enable Clear -- Stop the monitors */
    PMN_REGID_PMENTCLR,

    /* =6 PMUSERENR -- Performance Monitor User Enable Register */
    PMN_REGID_PMUSERENR,

    /* =7 PMOVSR -- Performance Monitor Overflow Status Register */
    PMN_REGID_PMOVSR,

    /* =8 PMXEVCNTCR -- Performance Monitor Event Count Control */
    PMN_REGID_PMXEVCNTCR,

    /* =9 PMXEVCNTR -- Performance Monitor Event Count Register */
    PMN_REGID_PMXEVCNTR,

    /* =10 PMCCNTR -- Performance Monitor Cycle Count Register */
    PMN_REGID_PMCCNTR,

    /* =11 PMINTENSET -- Performance Monitor Interrupt Enable Set */
    PMN_REGID_PMINTSET,

    /* =12 PMINTENCLR -- Performance Monitor Interrupt Enable Clear */
    PMN_REGID_PMINTCLR,

    /* =13 LPM0EVTYPER -- Local Performance Monitor 0 Event Type */
    PMN_REGID_LPM0EVTYPER,

    /* =14 LPM1EVTYPER -- Local Performance Monitor 1 Event Type */
    PMN_REGID_LPM1EVTYPER,

    /* =15 LPM2EVTYPER -- Local Performance Monitor 2 Event Type */
    PMN_REGID_LPM2EVTYPER,

    /* =16 L2LPMEVTYPER -- L2 Local Performance Monitor Event Type */
    PMN_REGID_L2LPMEVTYPER,

    /* =17 VLPMEVTYPER -- Vector Local Performance Monitor Event Type */
    PMN_REGID_VLPMEVTYPER
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
