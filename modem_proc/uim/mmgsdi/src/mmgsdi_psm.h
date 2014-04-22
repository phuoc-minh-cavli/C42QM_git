#ifndef MMGSDI_PSM_H
#define MMGSDI_PSM_H
/*===========================================================================


            M M G S D I  P S M  D E F I N I T I O N S

                      A N D   F U N C T I O N S


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2015 - 2018 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/uim.mpss/3.3.1/mmgsdi/src/mmgsdi_psm.h#1 $$ $DateTime: 2020/07/22 10:53:27 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
01/08/18   ra      Optimized write during PSM context save request processing
03/15/17   nr      Add support to cache PIN1 of the SIM card when PSM mode enabled
03/09/17   nr      Register for PSM_ENTER_REQ voting to save context
08/25/16   tkl     Support PSM card detected mode
01/12/16   kv      UIM PSM handling updates
01/12/16   bcho    Reset early init flag when card error occurs
12/25/15   bcho    initial version

=============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/

#include "uim_variation.h"
#include "mmgsdi.h"

/*=============================================================================

                       DATA TYPE DEFINITIONS

=============================================================================*/

/* PSM mode enum type, enum value matches the value stored in NV item 73804 */
typedef enum {
  MMGSDI_PSM_MODE_DISABLED       = 0,
  MMGSDI_PSM_MODE_EARLY_EVTS     = 1,
  MMGSDI_PSM_MODE_CARD_DETECTED  = 2
} mmgsdi_psm_mode_enum_type;

/*===========================================================================
FUNCTION MMGSDI_PSM_PURGE_DATA

DESCRIPTION
  To purge the data in EFS and to free heap memory occupied by PSM data

PARAMETERS
  is_complete_delete : It indicates whether to delete all PSM files from file system
                       or delete only the uicc profile file from file system

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mmgsdi_psm_purge_data(
  boolean is_complete_delete
);

/*===========================================================================
FUNCTION MMGSDI_PSM_INITIALIZE

DESCRIPTION
  To trigger reading of EFS data, caching it and seding card/session events.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mmgsdi_psm_initialize(
  void
);

/*===========================================================================
FUNCTION MMGSDI_PSM_CM_REGISTER

DESCRIPTION
  This function register with CM as a client to partcipate in PSM_ENTER_REQ
  voting for saving the context gracefully into SFS.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mmgsdi_psm_cm_register(
  void
);

/*===========================================================================
FUNCTION MMGSDI_PSM_STORE_DATA

DESCRIPTION
  To populate and store PSM UICC profile data on reception of task stop signal

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_psm_store_data(
  void
);

/*===========================================================================
FUNCTION MMGSDI_PSM_HANDLE_CARD_INIT_POST_TP_DL

DESCRIPTION
  This function handles select of AID and other init operations and
  make sure the early init is done on the same card that is inserted.
  if the card is found to be different, CARD ERROR is triggered and 
  card is reset to continue as a new card

PARAMETERS:
  None

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS:
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_psm_handle_card_init_post_tp_dl(
  void
);

/*===========================================================================
FUNCTION MMGSDI_PSM_ICCID_MATCH_DURING_LINK_EST

DESCRIPTION
  This function is invoked on link est of real card to match ICCID
  with psm iccid data  to ensure the early CARD and session events
  are valid

PARAMETERS:
  ICCID of the real card
  Protocol of the real card

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS:
  None
===========================================================================*/
boolean mmgsdi_psm_iccid_and_protocol_match_during_link_est(
  mmgsdi_iccid_info_type    iccid,
  mmgsdi_protocol_enum_type protocol
);

/*===========================================================================
FUNCTION MMGSDI_PSM_GET_STATUS_EARLY_INIT

DESCRIPTION
  This function returns the status of PSM early init during the time 
  between early card and session events are sent and the real card is 
  detected. once the real card is detected, this function returns FALSE,
  indicating no impact to command processing and other functionalities

PARAMETERS:
  None

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS:
  None
===========================================================================*/
boolean mmgsdi_psm_get_status_early_init(
  void
);

/*===========================================================================
FUNCTION MMGSDI_PSM_ENABLED_STATUS

DESCRIPTION
   Return the status whether UIM PSM NV, MMODE PSM NV and NV 4398
   auto provisioning are enabled or not.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_psm_mode_enum_type mmgsdi_psm_get_enabled_status(
  void
);

/*===========================================================================
FUNCTION MMGSDI_PSM_RESET_EARLY_INIT

DESCRIPTION
  This function reset the PSM early init flag and set MMGSDI_TASK_CMD_Q_SIG 
  when card error occurs. MMGSDI Slot1 task does not process this signal until
  early init flag is set to FALSE.

PARAMETERS
  is_complete_reset : Indicates whether to delete all files from PSM's EFS and SFS path or
                      delete only uicc profile file from PSM's SFS path so that remaining
                      files are retained.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mmgsdi_psm_reset_early_init(
  boolean is_complete_reset
);

/*===========================================================================
FUNCTION   MMGSDI_PROCESS_PSM_CONTEXT_SAVE_REQ

DESCRIPTION
  This function will process PSM enter request and call
  mmgsdi_psm_store_data() to store context to SFS.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void mmgsdi_process_psm_context_save_req(
  void
);

/*===========================================================================
FUNCTION   MMGSDI_PSM_PIN_EVT

DESCRIPTION
  This function will process PIN1 event and send PSM_READY_IND if it is
  pending due to PIN status.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void mmgsdi_psm_pin_evt(
  const mmgsdi_pin_evt_info_type    *pin_ptr
);

/*===========================================================================
FUNCTION  MMGSDI_PSM_HANDLE_CHANGE_NOTIFICATION_OF_EF

DESCRIPTION
  This function handles the notification of change in any cached EF content

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
void mmgsdi_psm_handle_change_notification_of_ef
(
  mmgsdi_session_type_enum_type session_type,
  mmgsdi_file_enum_type         mmgsdi_file
);

#endif /* MMGSDI_PSM_H */
