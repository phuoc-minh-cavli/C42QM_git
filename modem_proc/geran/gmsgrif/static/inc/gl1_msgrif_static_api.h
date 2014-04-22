#ifndef GL1_MSGRIF_STATC_API_H
#define GL1_MSGRIF_STATC_API_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                          GL1 MSGRIF STATIC API 

GENERAL DESCRIPTION
   This module contains the entry point to MSGRIF task static context

EXTERNALIZED FUNCTIONS
  gsm_l1_main
    .

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2001 - 2015 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/geran.mpss/5.2.0/gmsgrif/static/inc/gl1_msgrif_static_api.h#2 $
$DateTime: 2018/10/10 02:31:27 $ $Author: pwbldsvc $


when       who       what, where, why
--------   ---       ---------------------------------------------------------
09/01/18   hd      CR2167422 FR 46686: Support GERAN as dynamic module using DMM/External paging framework 
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "geran_dual_sim.h"

typedef void (*gl1_msgrif_main_loop_t)(void);

typedef struct
{
  gl1_msgrif_main_loop_t handle_rf_task_msgr_msgs;
  gl1_msgrif_main_loop_t handle_fw_async_msgr_msgs;
}geran_static_gl1_msgrif_ext_api_s;

extern geran_static_gl1_msgrif_ext_api_s gmsgrif_vtbl;


void gl1_msgrif_handle_rf_task_msgr_msgs_static(void);

void gl1_msgrif_handle_fw_async_msgr_msgs_static(void);

#endif  /* GL1_MSGRIF_STATC_API_H */
