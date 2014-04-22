/*===========================================================================

                 Shared Memory 9x25 Target Specific File


 Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/mproc/smd/src/smd_target.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/06/12   bt      Created 9x25 target file, from 8974.  Remove RIVA intrs.
===========================================================================*/

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "smd_internal.h"
#include "smd_proc.h"

/*===========================================================================
                           MACRO DEFINITIONS
===========================================================================*/

/** Table defining interrupts for each channel in the target */
smd_interrupt_info_type smd_interrupt_table[SMEM_NUM_HOSTS];

/*===========================================================================
FUNCTION      smd_target_init_interrupts

DESCRIPTION   This function initializes the interprocessor interrupts for this
              target.

ARGUMENTS     this_host - source of the interrupts

RETURN VALUE  void
===========================================================================*/
void smd_target_init_interrupts( smem_host_type this_host )
{
  uint32 i;
  
  /* Clear out the interrupt table */
  for( i = 0; i < SMEM_NUM_HOSTS; i++ )
  {
    smd_interrupt_table[i].to = SMEM_INVALID_HOST;
    smd_interrupt_table[i].processor = 0;
    smd_interrupt_table[i].irq_out = SMD_NULL_OUT_INTERRUPT;
    smd_interrupt_table[i].irq_out_mask = 0;
    smd_interrupt_table[i].irq_in = SMD_NULL_IN_INTERRUPT;
    smd_interrupt_table[i].disabled = FALSE;
  }
  
  /* From/to APPS Processor */
  smd_interrupt_table[SMEM_APPS].to            = SMEM_APPS;
  smd_interrupt_table[SMEM_APPS].processor     = SMD_APPS_IPCPROC;
  smd_interrupt_table[SMEM_APPS].irq_out       = SMD_APPS_OUT;
  smd_interrupt_table[SMEM_APPS].irq_out_mask  = 0; /* Not used. */
  smd_interrupt_table[SMEM_APPS].irq_in        = SMD_APPS_IN;
  
  /* From/to Modem processor */
  smd_interrupt_table[SMEM_MODEM].to           = SMEM_MODEM;
  smd_interrupt_table[SMEM_MODEM].processor    = SMD_MPSS_IPCPROC;
  smd_interrupt_table[SMEM_MODEM].irq_out      = SMD_MPSS_OUT;
  smd_interrupt_table[SMEM_MODEM].irq_out_mask = SMD_MPSS_OUT_M;
  smd_interrupt_table[SMEM_MODEM].irq_in       = SMD_MPSS_IN;

  /* From/to ADSP Q6 (LPASS) processor */
  smd_interrupt_table[SMEM_Q6].to              = SMEM_Q6;
  smd_interrupt_table[SMEM_Q6].processor       = SMD_ADSP_IPCPROC;
  smd_interrupt_table[SMEM_Q6].irq_out         = SMD_ADSP_OUT;
  smd_interrupt_table[SMEM_Q6].irq_out_mask    = SMD_ADSP_OUT_M;
  smd_interrupt_table[SMEM_Q6].irq_in          = SMD_ADSP_IN;

  /* From/to RPM processor */
  smd_interrupt_table[SMEM_RPM].to             = SMEM_RPM;
  smd_interrupt_table[SMEM_RPM].processor      = SMD_RPM_IPCPROC;
  smd_interrupt_table[SMEM_RPM].irq_out        = SMD_RPM_OUT;
  smd_interrupt_table[SMEM_RPM].irq_out_mask   = 0;   /* Not used */
  smd_interrupt_table[SMEM_RPM].irq_in         = SMD_RPM_IN;

}
