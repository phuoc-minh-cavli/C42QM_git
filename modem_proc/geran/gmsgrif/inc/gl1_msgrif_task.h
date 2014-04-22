#ifndef GL1_MSGRIF_TASK_H
#define GL1_MSGRIF_TASK_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  GL1 MSGR Interface Header File

GENERAL DESCRIPTION
   This header file contains declarations relevant to L1 task processing.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2001 - 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/geran.mpss/5.2.0/gmsgrif/inc/gl1_msgrif_task.h#5 $ $DateTime: 2020/07/15 10:06:15 $ $Author: pwbldsvc $

when       who      what, where, why
--------   -------- ---------------------------------------------
14/07/20   rks       CR2731714 compilation warning reduction for GL1
27/03/20   ktg     CR2575302 FR 58047: FSK based alarm reporting on jamming detection for IIOT
09/01/18   hd      CR2167422 FR 46686: Support GERAN as dynamic module using DMM/External paging framework 
07/04/14   cja     CR644445 Mainline feature FEATURE_GSM_ASYNC_FW_INTF
14/10/13   pa      CR536820: Added GL1 Msgr If Task. Initial version.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/* Alphabetical Order */
#include "geran_variation.h"
#include "comdef.h"
#include "gl1_msgrif_init.h"
#include "queue.h"
#include "geran_multi_sim.h"
#include "geran_tasks.h"

/*===========================================================================

                         DATA DECLARATIONS

===========================================================================*/
extern const geran_task_id_t gl1_msgrif_task_id;

extern q_type  gl1_msgrif_rf_task_msgr_q;
extern q_type  gl1_msgrif_rf_task_msgr_free_q;
extern gl1_msgrif_rf_task_msgr_t  gl1_msgrif_rf_task_msgr[GL1_MSGRIF_RF_TASK_MSGR_QUEUE_SIZE];

extern q_type gl1_msgrif_gfw_async_message_router_q;
extern q_type gl1_msgrif_gfw_async_message_router_free_q;
extern gl1_msgrif_gfw_async_msgr_t  gl1_msgrif_gfw_async_msgr[GL1_MSGRIF_GFW_ASYNC_MSGR_QUEUE_SIZE];

#ifdef FEATURE_FSK_SUPPORT
extern q_type  gl1_msgrif_qmi_msgr_q;
extern q_type  gl1_msgrif_qmi_msgr_free_q;
extern gl1_msgrif_qmi_msgr_t  gl1_msgrif_qmi_msgr[GL1_MSGRIF_QMI_MSGR_QUEUE_SIZE];
#endif

/*===========================================================================

                         FUNCTION DECLARATIONS

===========================================================================*/

void gsm_gl1_msgrif_task (dword dummy);
void gl1_msgrif_main (dword unused);
rex_tcb_type* gl1_msgrif_task_tcb_read(void);

boolean gl1_msgrif_main_loop(void);
#ifdef FEATURE_FSK_SUPPORT
void gl1_msgrif_handle_qmi_msgr_msgs(void);
void gl1_msgrif_handle_qmi_msgr_msgs_static(void);
#endif

#endif /* GL1_MSGRIF_TASK_H */
