/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  F3 Trace NV Configuration Initialization
                
GENERAL DESCRIPTION
  Contains handlers to configure F3 trace based on NV settings.
    
INITIALIZATION AND SEQUENCING REQUIREMENTS
  Should be called after NV is initialized.

Copyright (c) 2012-2017, 2020 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*==========================================================================

                        EDIT HISTORY FOR MODULEW
$Header: //components/rel/core.mpss/3.10/services/diag/f3_trace/src/diag_f3_trace_nvcfg.c#2 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
11/19/13   rh      Added tracking for current source of F3 Trace settings 
03/04/13   rh      NV detail really read into detail and NV control to control 
02/13/13   rh      Fixed compiler errors 
12/19/12   rh      Transitioned REX signal calls to Services OSAL 
08/24/12   mcg     Initial file creation

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "diag_f3_tracei.h"
#include "nv.h"
#include "osal.h"
#include "diagnv.h"

 
/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/
#define  DIAG_F3_TRACE_NV_CMD_SIG        0x0008

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
  nv_item_type nv_control, nv_detail;
  nv_stat_enum_type ret_val;

  /* Read the diag_debug_detail NV Item */
  ret_val = diag_nv_read(NV_DIAG_DEBUG_DETAIL_I, &nv_detail);
  
  if (ret_val != NV_DONE_S)
  {
    /* NV Item has yet to be written, or was not read correctly. */
    return;
  }

  /* Read the diag_debug_control NV Item */ 
  ret_val = diag_nv_read(NV_DIAG_DEBUG_CONTROL_I, &nv_control); 
  if (ret_val != NV_DONE_S)
  {
    /* NV Item has yet to be written, or was not read correctly. */
    return;
  }

  //good to initialize main f3 trace module
  diag_f3_trace_config_source = DIAG_F3_TRACE_CONFIG_NV;
  diag_f3_trace_init_configs (nv_control.diag_debug_control, nv_detail.diag_debug_detail);
}


