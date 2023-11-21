#ifndef _DS_QMI_VOICE_MSGR_IF_H
#define _DS_QMI_VOICE_MSGR_IF_H
/*===========================================================================

                         D S _ Q M I _ V O I C E _ MSGR _ I F . H

DESCRIPTION

 The Data Services Qualcomm Voice service Messenger interface header file.


EXTERNALIZED FUNCTIONS



Copyright (c) 2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/mmcp.mpss/6.1.10/mmode/qmi/src/qmi_voice_msgr_if.h#1 $ $DateTime: 2023/04/25 14:39:18 $ $Author: pwbldsvc $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
05/23/13    vs     Initial revision.
===========================================================================*/

#include "cm.h"
#include  "log.h"
#include "cm_msgr_msg.h"
#include "qmi_mmode_task_cmd.h"

#ifndef FEATURE_MMODE_DISABLE_MISC_FEATURES
/*===========================================================================

                    RESPONSES

===========================================================================*/




/*===========================================================================

                    INDICATIONS

===========================================================================*/



/*===========================================================================

                   CALLBACKS

===========================================================================*/


/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                        EXTERNAL FUNCTION DEFINTIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

IxErrnoType qmi_voice_msgr_send_srvcc_config_req
(
  qmi_voice_cm_if_set_srvcc_call_context_s *const in_data_ptr,
  qmi_voice_cm_if_as_id_e_type as_id
);

void qmi_voice_msgr_vice_dialog_info_ind
(
  mmode_qmi_voice_viceDialog_info_ind_msg_type *vice_dialog_info
);
#endif

void qmi_voice_msgr_handler( void *param );

void qmi_voice_msgr_register( msgr_client_t *msgr_client );

#endif /* _DS_QMI_VOICE_MSGR_IF_H */


