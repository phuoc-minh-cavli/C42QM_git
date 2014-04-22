#ifndef QMI_CAT_H
#define QMI_CAT_H
/*===========================================================================

                         Q M I _ C A T . H

DESCRIPTION

 The Data Services QMI Card Application Toolkit Service header file.

                      COPYRIGHT INFORMATION

Copyright (c) 2009 - 2010, 2012 - 2016, 2019 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/uim.mpss/3.3.1/uimqmi/inc/qmi_cat.h#1 $ $DateTime: 2020/07/22 10:53:27 $ $Author: pwbldsvc $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
10/23/19    dt      Migration of QMICAT to QCSI/QCCI
10/10/16    shr    Enable Data Card Config. if QMI CAT is not supported
03/08/16    vr     Memory optimization for IOT devices
11/25/15    sp     Updated copyright
09/07/15    bcho   Migrate to mcfg EFS wrappers
08/27/15    vdc    Do not retry service registration if it fails once
02/13/15    hh     Split qmi_cat_init into pre and post startup
10/30/14    gm     Move enable/disable of raw unsupported features to NV
09/24/14    gm     Remove GOBI mode code
09/09/14    bd     Moved an enum to header file
08/05/14    hh     Added support to retrieve supported proactive commands
01/09/14    kb     Add de-init function when gstk task is stopped
04/11/13    tkl    Add NV suport for SETUP Call display alpha value
04/02/13    av     Move UIM tasks to group4 of RCINIT
02/19/13    av     Merge mmgsdi,gstk,qmiuim/cat conf files to mmgsdi.conf
02/14/13    tl     Move QMI CAT to GSTK context from QMI modem context
02/23/12    nmb    QMI QMUX Transition
08/05/10    tkl    QMI UIM & QMI CAT split from data package
03/25/10    dd     Adding decoded tlvs
11/01/09    mj     Initial check-in of QMI_CAT
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "gstk_exp.h"
#include "mmgsdilib_common.h"
#include "rex.h"
#ifdef FEATURE_MODEM_RCINIT
#include "rcinit.h"
#endif

/*===========================================================================

                                DATA TYPES

===========================================================================*/

/*---------------------------------------------------------------------------
  Set Event Report Raw event bitmask
---------------------------------------------------------------------------*/
#define CATI_REPORT_STATUS_NONE                                (0x0000000)
#define CATI_REPORT_STATUS_EV_DISPLAY_TEXT                     (0x0000001)
#define CATI_REPORT_STATUS_EV_GET_INKEY                        (0x0000002)
#define CATI_REPORT_STATUS_EV_GET_INPUT                        (0x0000004)
#define CATI_REPORT_STATUS_EV_SETUP_MENU                       (0x0000008)
#define CATI_REPORT_STATUS_EV_SELECT_ITEM                      (0x0000010)
#define CATI_REPORT_STATUS_EV_SENDSMS_ALPHA_ID                 (0x0000020)
#define CATI_REPORT_STATUS_EV_SETUPEVT_USER_ACT                (0x0000040)
#define CATI_REPORT_STATUS_EV_SETUPEVT_IDL_SCR_NOTIFY          (0x0000080)
#define CATI_REPORT_STATUS_EV_SETUPEVT_LANGSEL_NOTIFY          (0x0000100)
#define CATI_REPORT_STATUS_EV_SETUP_IDL_MODE_TEXT              (0x0000200)
#define CATI_REPORT_STATUS_EV_LANG_NOTIFICATION                (0x0000400)
#define CATI_REPORT_STATUS_EV_REFRESH                          (0x0000800)
#define CATI_REPORT_STATUS_EV_END_PROACTIVE_SESSION            (0x0001000)
#define CATI_REPORT_STATUS_EV_PLAY_TONE                        (0x0002000)
#define CATI_REPORT_STATUS_EV_SETUP_CALL                       (0x0004000)
#define CATI_REPORT_STATUS_EV_SEND_DTMF                        (0x0008000)
#define CATI_REPORT_STATUS_EV_LAUNCH_BROWSER                   (0x0010000)
#define CATI_REPORT_STATUS_EV_SEND_SS                          (0x0020000)
#define CATI_REPORT_STATUS_EV_SEND_USSD                        (0x0040000)
#define CATI_REPORT_STATUS_EV_PROVIDE_LOCAL_INFO_LANG          (0x0080000)
#define CATI_REPORT_STATUS_EV_DATA_EVT                         (0x0100000)
#define CATI_REPORT_STATUS_EV_SETUPEVT_BROWSER_TERM            (0x0200000)
#define CATI_REPORT_STATUS_EV_PROVIDE_LOCAL_INFO_TIME          (0x0400000)
#define CATI_REPORT_STATUS_EV_SCWS_EVT                         (0x0800000)
#define CATI_REPORT_STATUS_EV_ACTIVATE                         (0x1000000)
#define CATI_REPORT_STATUS_EV_SETUPEVT_HCI_CONN                (0x2000000)
#define CATI_REPORT_STATUS_EV_DATA_EVT_STATUS                  (0x4000000)

#define CATI_RAW_PROACTIVE_CMD_MASK ((CATI_REPORT_STATUS_EV_DISPLAY_TEXT)       | \
                                     (CATI_REPORT_STATUS_EV_GET_INKEY)          | \
                                     (CATI_REPORT_STATUS_EV_GET_INPUT)          | \
                                     (CATI_REPORT_STATUS_EV_SETUP_MENU)         | \
                                     (CATI_REPORT_STATUS_EV_SELECT_ITEM)        | \
                                     (CATI_REPORT_STATUS_EV_SETUP_IDL_MODE_TEXT)| \
                                     (CATI_REPORT_STATUS_EV_LANG_NOTIFICATION)  | \
                                     (CATI_REPORT_STATUS_EV_PLAY_TONE)          | \
                                     (CATI_REPORT_STATUS_EV_SETUP_CALL)         | \
                                     (CATI_REPORT_STATUS_EV_SEND_DTMF)          | \
                                     (CATI_REPORT_STATUS_EV_LAUNCH_BROWSER)     | \
                                     (CATI_REPORT_STATUS_EV_SEND_SS)            | \
                                     (CATI_REPORT_STATUS_EV_SEND_USSD)          | \
                                     (CATI_REPORT_STATUS_EV_SENDSMS_ALPHA_ID)   | \
                                     (CATI_REPORT_STATUS_EV_DATA_EVT)           | \
                                     (CATI_REPORT_STATUS_EV_SCWS_EVT)           | \
                                     (CATI_REPORT_STATUS_EV_ACTIVATE)           | \
                                     (CATI_REPORT_STATUS_EV_REFRESH))

#define CATI_PROACTIVE_SESSION_END_DUE_TO_CARD  0x01
#define CATI_PROACTIVE_SESSION_END_INTERNAL     0x02

#define CATI_SETUP_EVT_LIST_MASK ((CATI_REPORT_STATUS_EV_SETUPEVT_USER_ACT) | \
                            (CATI_REPORT_STATUS_EV_SETUPEVT_IDL_SCR_NOTIFY) | \
                            (CATI_REPORT_STATUS_EV_SETUPEVT_LANGSEL_NOTIFY) | \
                            (CATI_REPORT_STATUS_EV_SETUPEVT_BROWSER_TERM) | \
                            (CATI_REPORT_STATUS_EV_SETUPEVT_HCI_CONN))

#define CATI_PROVIDE_LOCAL_INFO_MASK ((CATI_REPORT_STATUS_EV_PROVIDE_LOCAL_INFO_LANG) | \
                            (CATI_REPORT_STATUS_EV_PROVIDE_LOCAL_INFO_TIME))

#define CATI_CACHED_EVT_MASK ((CATI_REPORT_STATUS_EV_DISPLAY_TEXT)           | \
                              (CATI_REPORT_STATUS_EV_SETUP_MENU)             | \
                              (CATI_REPORT_STATUS_EV_SETUP_IDL_MODE_TEXT)    | \
                              (CATI_REPORT_STATUS_EV_SETUPEVT_USER_ACT)      | \
                              (CATI_REPORT_STATUS_EV_SETUPEVT_IDL_SCR_NOTIFY)| \
                              (CATI_REPORT_STATUS_EV_SETUPEVT_LANGSEL_NOTIFY)| \
                              (CATI_REPORT_STATUS_EV_SETUPEVT_HCI_CONN)      | \
                              (CATI_REPORT_STATUS_EV_SETUPEVT_BROWSER_TERM)  | \
                              (CATI_REPORT_STATUS_EV_PROVIDE_LOCAL_INFO_LANG)| \
                              (CATI_REPORT_STATUS_EV_PROVIDE_LOCAL_INFO_TIME)| \
                              (CATI_REPORT_STATUS_EV_GET_INPUT))
                              
/*---------------------------------------------------------------------------
  Get Cached Proactive Command command_id type
---------------------------------------------------------------------------*/
#define CATI_CACHED_CMD_TYPE_SETUP_MENU                  0x01
#define CATI_CACHED_CMD_TYPE_SETUP_EVT_LIST              0x02
#define CATI_CACHED_CMD_TYPE_SETUP_IDLE_MODE_TEXT        0x03

/*=============================================================================

                   ENUMERATED DATA

=============================================================================*/

/*---------------------------------------------------------------------------
  QMI CAT configuration type
---------------------------------------------------------------------------*/
typedef enum
{
  CATI_CONFIG_MODE_DISABLED                    = 0x00,
  /* GOBI mode no longer supported */
  CATI_CONFIG_MODE_ANDROID                     = 0x02,
  CATI_CONFIG_MODE_DECODED                     = 0x03,
  CATI_CONFIG_MODE_DECODED_PULLONLY            = 0x04,
  CATI_CONFIG_MODE_CUSTOM_RAW                  = 0x05,
  CATI_CONFIG_MODE_CUSTOM_DECODED              = 0x06
} qmi_cati_config_mode_e_type;

/*=============================================================================

                   FUNCTIONS

=============================================================================*/

/*===========================================================================
  FUNCTION QMI_CAT_INIT_PRE_STARTUP()

  DESCRIPTION
    Initializes the QMI CAT global data and message queues

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern void qmi_cat_init_pre_startup
(
  void
);


/*===========================================================================
  FUNCTION QMI_CAT_INIT_POST_STARTUP()

  DESCRIPTION
    Registers the QMI CAT service to the QMI framework to initalize the service
    and registers with MMGSDI, GSTK

  PARAMETERS
    rex_tcb_type  : Rex tcb type
    rex_sigs_type : rex signal type

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern void qmi_cat_init_post_startup
(
  rex_tcb_type * task_tcb,
  rex_sigs_type  sigs
);

/*===========================================================================
QMI_CAT_SIG_HANDLER

DESCRIPTION
  QMI CAT signal handler for GSTK signals

PARAMETERS
  None.

  Returns TRUE if signal should be cleared from the set signal mask,
  FALSE if further signal processing is needed and hence signal
  should not be cleared.
===========================================================================*/
extern boolean qmi_cat_sig_handler
(
  void
);

/*===========================================================================
  FUNCTION QMI_CAT_GET_TERMINAL_PROFILE_VALUE_LIST()

  DESCRIPTION
    Get terminal profile value list according to the QMI_CAT config mode.

  PARAMETERS
    none

  RETURN VALUE
    gstk_profile_dl_support_ext_type* : array of terminal profile

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern gstk_profile_dl_support_ext_type* qmi_cat_get_terminal_profile_value_list
(
  uint8                            * tp_count_ptr
);

/*===========================================================================
  FUNCTION QMI_CAT_RECOVERY_COMPLETE()

  DESCRIPTION
    This function is called by the QMI UIM when a recovery indication
    has completed successfully

  PARAMETERS
    slot: Indicates the slot in which the recovery occured

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_cat_recovery_complete
(
  mmgsdi_slot_id_enum_type  slot
);

/*===========================================================================
FUNCTION: qmi_cat_deinit

DESCRIPTION:
  This function is used to free the qmicat private resources.
  This is called when GSTK is powering down.

PARAMETERS:
  None

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None
===========================================================================*/
extern void qmi_cat_deinit
(
  void
);

/*===========================================================================
FUNCTION: qmi_cat_check_if_supported

DESCRIPTION:
  This function is used to check if QMI CAT is supported

PARAMETERS:
  None

DEPENDENCIES:
  None

RETURN VALUE:
  boolean: Indicates if QMI CAT is supported or not

COMMENTS:
  None

SIDE EFFECTS:
  None
===========================================================================*/
extern boolean qmi_cat_check_if_supported
(
  void
);

/*===========================================================================
FUNCTION: QMI_CAT_SIG_HANDLER

DESCRIPTION
  QMI CAT signal handler for GSTK signals

PARAMETERS
  None.

  Returns TRUE if signal should be cleared from the set signal mask,
  FALSE if further signal processing is needed and hence signal
  should not be cleared.
===========================================================================*/
void qmi_cat_handle_qcsi_evt
(
  rex_tcb_type * task_tcb, 
  rex_sigs_type  sigs
);

#endif /* QMI_CAT_H */
