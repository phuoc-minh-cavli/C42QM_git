#ifndef _DS_QMI_VOICE_H
#define _DS_QMI_VOICE_H
/*===========================================================================

                         D S _ Q M I _ V O I C E . H

DESCRIPTION



EXTERNALIZED FUNCTIONS



Copyright (c) 2009 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/mmcp.mpss/6.1.10/mmode/qmi/src/qmi_voice.h#1 $ $DateTime: 2023/04/25 14:39:18 $ $Author: pwbldsvc $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/25/09    vs     Initial revision.
===========================================================================*/

#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_MMODE_DISABLE_INBAND_SUPP
#define REMOVE_QMI_VOICE_START_CONT_DTMF_V02
#define REMOVE_QMI_VOICE_STOP_CONT_DTMF_V02
#endif

#ifdef FEATURE_MMODE_CALL_SUPS
#define REMOVE_QMI_VOICE_MANAGE_CALLS_V02
#endif
#ifdef FEATURE_MMODE_SS_SUPS
#define REMOVE_QMI_VOICE_GET_CLIP_V02
#define REMOVE_QMI_VOICE_GET_CALL_WAITING_V02
#define REMOVE_QMI_VOICE_GET_CALL_BARRING_V02
#define REMOVE_QMI_VOICE_SET_CALL_BARRING_PASSWORD_V02
#define REMOVE_QMI_VOICE_SET_SUPS_SERVICE_V02
#define REMOVE_QMI_VOICE_GET_CLIR_V02
#define REMOVE_QMI_VOICE_GET_CALL_FORWARDING_V02
#endif
#ifdef FEATURE_MMODE_CALL_SUPS
#define REMOVE_QMI_VOICE_MANAGE_IP_CALLS_V02
#endif
#ifdef FEATURE_MMODE_SS_SUPS
#define REMOVE_QMI_VOICE_GET_COLP_V02
#define REMOVE_QMI_VOICE_GET_COLR_V02
#define REMOVE_QMI_VOICE_GET_CNAP_V02
#endif

#ifndef FEATURE_MMODE_USS_SUPS
#define REMOVE_QMI_VOICE_ORIG_USSD_V02
#define REMOVE_QMI_VOICE_ANSWER_USSD_V02
#define REMOVE_QMI_VOICE_CANCEL_USSD_V02
#define REMOVE_QMI_VOICE_ORIG_USSD_NO_WAIT_V02


#endif

#if !((defined FEATURE_ALS) && (defined FEATURE_GSM))
#define REMOVE_QMI_VOICE_ALS_SET_LINE_SWITCHING_V02
#define REMOVE_QMI_VOICE_ALS_SELECT_LINE_V02
#define REMOVE_QMI_VOICE_ALS_GET_LINE_SWITCHING_STATUS_V02
#define REMOVE_QMI_VOICE_ALS_GET_SELECTED_LINE_V02
#endif

#ifdef FEATURE_MMODE_DISABLE_MISC_FEATURES
#define REMOVE_QMI_VOICE_SRVCC_CALL_CONFIG_V02
#endif

/*===========================================================================
  FUNCTION QMI_VOICE_INIT()

  DESCRIPTION
    Register the VOICE service with QMUX for all applicable QMI links

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void  qmi_voice_init
(
  void
);

/*===========================================================================
  FUNCTION QMI_VOICE_HAS_VALID_CLIENT()

  DESCRIPTION
    Check if QMI-Voice has a valid client

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_voice_has_valid_client
(
  void
);


#endif /* _DS_QMI_VOICE_H */


