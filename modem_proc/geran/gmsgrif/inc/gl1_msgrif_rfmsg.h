#ifndef GL1_MSGRIF_RFMSG_H
#define GL1_MSGRIF_RFMSG_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  GL1 Msgr Interface
                  RF Messages Header File

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

$Header: //components/rel/geran.mpss/5.2.0/gmsgrif/inc/gl1_msgrif_rfmsg.h#2 $ $DateTime: 2018/10/10 02:31:27 $ $Author: pwbldsvc $

when       who      what, where, why
--------   -------- ---------------------------------------------
07/04/14   cja     CR644445 Mainline feature FEATURE_GSM_ASYNC_FW_INTF
14/10/13   pa      CR536820: Added GL1 Msgr If Task. Initial version.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/* Alphabetical Order */
#include "geran_variation.h"
#include "comdef.h"
#include "geran_dual_sim.h"
/*===========================================================================

                         DATA DECLARATIONS

===========================================================================*/


/*===========================================================================

                         FUNCTION DECLARATIONS

===========================================================================*/
void gl1_msgrif_handle_rf_task_msgr_msgs(void);

#endif /* GL1_MSGRIF_RFMSG_H */
