/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  F3 Trace NV Configuration Initialization
                
GENERAL DESCRIPTION
  Contains handlers to configure F3 trace based on NV settings.
    
INITIALIZATION AND SEQUENCING REQUIREMENTS
  Should be called after NV is initialized.

Copyright (c) 2012-2013 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*==========================================================================

                        EDIT HISTORY FOR MODULEW
$Header: //components/rel/core.tx/6.0/services/diag/f3_trace/src/diag_f3_trace_nvcfg.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
08/17/16   nk      Uncommented EFS calls and added F3 trace variables initialization  
11/19/13   rh      No longer ERR_FATAL if error returned in NV read
08/24/12   mcg     Initial file creation

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "diag_f3_tracei.h"
#include "f3_trace_nv_efs.h"
#include "err.h"

 
/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/


/*===========================================================================

FUNCTION diag_f3_trace_nvcfg_init

DESCRIPTION
  Initializes f3 trace configuration based on nv settings
  -note that diag_f3_trace can be configured through other mechanisms (eg devcfg)
   which may have higher precedence. 

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void diag_f3_trace_nvcfg_init(void)
{
  uint8 nv_control, nv_detail;

  nv_control = diag_f3_trace_control_mask;
  nv_detail = diag_f3_trace_detail_mask;

  // initialize f3_trace_nv_efs structures
  f3_trace_nv_efs_init();

  // get value for trace control
  if (F3_TRACE_NV_EFS_REG_RW_STAT_OK != f3_trace_nv_efs_reg_item_read(F3_TRACE_NV_EFS_F3_TRACE_CONTROL, sizeof(uint8), &nv_control))
  {
    nv_control = diag_f3_trace_control_mask;
  }

  // get value for trace detail
  if (F3_TRACE_NV_EFS_REG_RW_STAT_OK != f3_trace_nv_efs_reg_item_read(F3_TRACE_NV_EFS_F3_TRACE_DETAIL, sizeof(uint8), &nv_detail))
  {
    nv_detail = diag_f3_trace_detail_mask;
  }

  if (diag_f3_trace_control_mask != nv_control || diag_f3_trace_detail_mask != nv_detail)
  {
    //good to initialize main f3 trace module
    diag_f3_trace_config_source = DIAG_F3_TRACE_CONFIG_NV;
    diag_f3_trace_init_configs (nv_control, nv_detail);
  }
}



