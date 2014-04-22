/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                D A T A   S E R V I C E S

                A T   C O M M A N D   
                ( C O M M O N   A C T I O N  
                  C O M M A N D S )

                P R O C E S S I N G

GENERAL DESCRIPTION
  This module executes the AT commands. It mainly executes the common 
  (across all modes) action commands.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
 

Copyright (c) 2001 - 2017 by Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/dataiot.mpss/2.0/interface/atcop/src/dsatact_ex.c#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
1/19/17    skc     Added enhancement on CHLD and CLCC for VoLTE MPTY Call.
06/27/14   tk      Initial revision (created file for Dynamic ATCoP).

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include <stringl/stringl.h>


#include <stdio.h>
#include <string.h>

#include "dsati.h"
#include "dsatact.h"
#include "dsatctab.h"
#include "dsatdl.h"
#include "dsatparm.h"
#include "dsatvoice.h"
#include "dsatcmif.h"
#include "ds_cap.h" /* ITC_SPEECH */
#include "dstask.h"
#include "dstaski.h"
#include "err.h"
#include "mobile.h"
#include "msg.h"
#include "ds3gsiolib_ex.h"

#ifdef FEATURE_DSAT_ETSI_MODE
#include "dsatetsime.h"
#include "dsatetsicall.h"
#include "dsatetsicmif.h"
#include "dsatetsictab.h"

#ifdef FEATURE_DSAT_ETSI_DATA
#include "ds_rmsmi.h"
#include "dsatetsipkt.h"
#endif /* FEATURE_DSAT_ETSI_DATA*/
#endif /* FEATURE_DSAT_ETSI_MODE */

#ifdef FEATURE_DATA_MUX
#include "ds_mux_ext_defs.h"
#endif /* FEATURE_DATA_MUX */

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/
/*-------------------------------------------------------------------------
            Constants:
-------------------------------------------------------------------------*/

/*-------------------------------------
  Local Variables
---------------------------------------*/

/*-------------------------------------
  Regional Variables
---------------------------------------*/
/* Regional variable that is used to retain the */
/* call back function pointers for ATD, ATA     */
/* ATH,+CGANS and abort handlers for ATA,       */
/* ATD and +CGANS                               */
dsat_dial_string_type dial_string_handler = {0};

#ifdef FEATURE_DATA_MUX
dsat_num_item_type dsat_port_open_count = 0;
rex_timer_type dsat_rdm_open_timer;
#endif /* FEATURE_DATA_MUX */
/*-------------------------------------------------------------------------
    Protypes for local functions:
-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
            Function Definitions:
-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION DSAT_REGISTER_HANDLERS

DESCRIPTION
  This function is used to register a call back functions.
  It is used to register call back funtions for ATA and ATH.

  The call back handler can be NULL. If it is not an incoming call, 
  then ATA handler will be NULL.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_OK :  if successfully registered.
  DSAT_ERROR: if dsat_cgauto_val == 2(only in ETSI mode)
  
SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsat_register_handlers
(
  dsat_incom_answer_cb_type  call_answer_cb, /* Answer call back */
  dsat_call_hangup_cb_type   call_hangup_cb  /* Hangup call back */
)
{
  dsat_result_enum_type result = DSAT_OK;
  /* ATA Handler */
    dial_string_handler.answer_cb = call_answer_cb; 

  /* ATH Handler */
  dial_string_handler.hangup_cb = call_hangup_cb;

  return result ;
} /* dsat_register_handlers */

/*===========================================================================

FUNCTION DSAT_DEREGISTER_HANDLERS

DESCRIPTION
  This function is used to deregister the call back functions.
  It resets all the registered handlers to NULL.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsat_deregister_handlers ( void )
{
    /* ATA Handler */

    dial_string_handler.answer_cb = NULL; 
    
  /* ATH Handler */
  dial_string_handler.hangup_cb = NULL;
  return;
} /* dsat_deregister_handlers */


/*===========================================================================

FUNCTION DSAT_REG_ATZ_CB

DESCRIPTION
  This function is used to register a call back function for ATZ.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsat_reg_atz_cb
(
  dsat_atz_cb_type atz_cb  /* atz call back */
)
{
  dial_string_handler.atz_cb = atz_cb;
  return;
} /* dsat_reg_atz_cb */



/*===========================================================================

FUNCTION DSAT_INIT_CB_HANDLERS

DESCRIPTION
  Initialize all the registered call handlers to NULL
  Called from dsat_init() after every reset.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsat_init_cb_handlers(void)
{
  /* Reset all the CB handlers to NULL */
  memset ( &dial_string_handler, 0, sizeof(dsat_dial_string_type) );

  return;
}



/*===========================================================================

FUNCTION DSAT_PROC_DIAL_STR

DESCRIPTION
  Parses a dial string, looking for allowed non-digits, and
  illegal non-digits.  Digits, and allowed non-digits are copied
  to the output.  Illegal characters terminate processing.  All
  other characters are ignore.  Upper and lower case are treated
  as being the same.

  Since the output will be no longer than the input, the output
  string may be the same as the input string.

DEPENDENCIES
  None

RETURN VALUE
  Returns enum value indicating dial string content or error

SIDE EFFECTS
  None

===========================================================================*/
dsat_dial_val_e_type dsat_proc_dial_str
(
  const byte * in_ptr,          /*  Input string, null terminated   */
  byte * out_ptr                /*  Output string, null terminated  */
)
{
  dsat_dial_val_e_type result = DSAT_DIAL_EMPTY;

  if (DSAT_DL_VALIDATE_SYMBOL_ADDR(dsatdl_vtable.dsatact_proc_dial_str_fp))
  {
    result = dsatdl_vtable.dsatact_proc_dial_str_fp(in_ptr, out_ptr);
  }

  return result;
} /* dsat_proc_dial_str () */

/*===========================================================================

FUNCTION DSATACT_TRACK_SEQUENCE_NUMBER

DESCRIPTION
  Track the sequence number (as described in 3GPP TS 22.030 section 6.5.5.1) 
  for this call ID and event: Set in call sequence number indexed array 
  dsat_seqnum_callid[] on call incoming or connected event and clear on call 
  end event.
  3GPP TS 22.030 section 6.5.5.1 reads - "X" is the numbering (starting with 1)
  of the call given by the sequence of setting up or receiving the calls 
  (active, held or waiting) as seen by the served subscriber. Calls hold their
  number until they are released. New calls take the lowest available number.

DEPENDENCIES
  None
  
RETURN VALUE
  Returns tracked sequence number

SIDE EFFECTS
  None
  
===========================================================================*/
int dsatact_track_sequence_number
(
  cm_call_id_type        call_id,
  cm_call_event_e_type   call_event,
  cm_call_type_e_type    call_type
)
{
  dsatetsicall_call_state_info  *call_da_val = NULL;
  /* Sequence numbers internal to this function are 
     really 22.030 sequence numbers - 1 */
  int index, first_avail_seqnum, callid_seqnum;
  int seq_number = CALL_SEQNUM_INVALID;
  
  call_da_val = dsat_get_base_addr(DSAT_CALL_VALS, TRUE);
  if (NULL == call_da_val)
  {
    return seq_number;
  }
  /* Search through sequence number indexed array... */
  for ( index = 0, first_avail_seqnum = callid_seqnum = CALL_SEQNUM_INVALID; 
        index < CM_CALL_ID_MAX; 
        index++ )
  {
    /* When first available sequence number is found... */
    if ( CALL_SEQNUM_INVALID == first_avail_seqnum && 
         CM_CALL_ID_INVALID ==  call_da_val->dsat_seqnum_callid[index].call_id )
    {
      first_avail_seqnum = index;
      seq_number = index + 1;
    }
    
    /* If call ID found... */
    if ( CALL_SEQNUM_INVALID == callid_seqnum &&
         call_id == call_da_val->dsat_seqnum_callid[index].call_id )
    {
      callid_seqnum = index;
      seq_number = index + 1;
    }
  }
  
  DS_AT_MSG5_HIGH("In track_sequence_number call_id %d call_evt %d call_type %d first_avail_seqnum %d callid_seqnum %d",
                   call_id, call_event, call_type, first_avail_seqnum, callid_seqnum);

  switch( call_event )
  {
    case CM_CALL_EVENT_ORIG:
    case CM_CALL_EVENT_SETUP_IND:
    case CM_CALL_EVENT_ACT_BEARER_IND:
      /* Should never be existing sequence number for incoming call ID,
         try to clean up */
      if ( CALL_SEQNUM_INVALID != callid_seqnum )
      {
        DS_AT_MSG1_ERROR("Event incoming & already sequence number for call ID %d",
                   call_id);
        call_da_val->dsat_seqnum_callid[callid_seqnum].call_id = CM_CALL_ID_INVALID;
      }
      if ( CALL_SEQNUM_INVALID != first_avail_seqnum )
      {
        call_da_val->dsat_seqnum_callid[first_avail_seqnum].call_id = call_id;
        call_da_val->dsat_seqnum_callid[first_avail_seqnum].call_type = call_type;
#ifdef FEATURE_DSAT_VOLTE_MPTY
        call_da_val->dsat_seqnum_callid[first_avail_seqnum].is_conf_participant = FALSE;
#endif /* FEATURE_DSAT_VOLTE_MPTY */
      }
      break;
    case CM_CALL_EVENT_CONNECT:
    case CM_CALL_EVENT_INCOM:
    case CM_CALL_EVENT_RAB_REESTAB_IND:
#ifdef FEATURE_DSAT_VOLTE_MPTY		
    case CM_CALL_EVENT_CONFERENCE_INFO:
#endif /* FEATURE_DSAT_VOLTE_MPTY */		
      /* May find existing sequence number for connecting call ID if previous 
         incoming call, else assign first available */
      if (( CALL_SEQNUM_INVALID == callid_seqnum ) && 
         ( CALL_SEQNUM_INVALID != first_avail_seqnum ))
      {
        call_da_val->dsat_seqnum_callid[first_avail_seqnum].call_id = call_id;
        call_da_val->dsat_seqnum_callid[first_avail_seqnum].call_type = call_type;
#ifdef FEATURE_DSAT_VOLTE_MPTY		
        /* Update seqnum and call id mapping for CM_CALL_EVENT_CONFERENCE_INFO */
        if(call_event == CM_CALL_EVENT_CONFERENCE_INFO)
        {
          call_da_val->dsat_seqnum_callid[first_avail_seqnum].is_conf_participant = TRUE;
        }
        else
        {
          call_da_val->dsat_seqnum_callid[first_avail_seqnum].is_conf_participant = FALSE;
        }
#endif /* FEATURE_DSAT_VOLTE_MPTY */
      }
#ifdef FEATURE_DSAT_VOLTE_MPTY	  
      /* If there is any participants info in disconnected status, callid_seqnum will be valid one.
         So clearing sequence info for the participant. */
      if(call_event == CM_CALL_EVENT_CONFERENCE_INFO && CALL_SEQNUM_INVALID != callid_seqnum)
      {
        call_da_val->dsat_seqnum_callid[callid_seqnum].call_id = CM_CALL_ID_INVALID;
        call_da_val->dsat_seqnum_callid[callid_seqnum].call_type = CM_CALL_TYPE_NONE;
        call_da_val->dsat_seqnum_callid[callid_seqnum].is_conf_participant = FALSE;        
      }
#endif /* FEATURE_DSAT_VOLTE_MPTY */	  
      break;
    case CM_CALL_EVENT_END:
    case CM_CALL_EVENT_PDN_CONN_REJ_IND:
    case CM_CALL_EVENT_PDN_CONN_FAIL_IND:
    case CM_CALL_EVENT_RES_ALLOC_REJ_IND:
    case CM_CALL_EVENT_RES_ALLOC_FAIL_IND:
      /* Should find existing sequence number for ending call ID */
      if ( CALL_SEQNUM_INVALID != callid_seqnum )
      {
        call_da_val->dsat_seqnum_callid[callid_seqnum].call_id = CM_CALL_ID_INVALID;
        call_da_val->dsat_seqnum_callid[callid_seqnum].call_type = CM_CALL_TYPE_NONE;

#ifdef FEATURE_DSAT_VOLTE_MPTY		
        call_da_val->dsat_seqnum_callid[callid_seqnum].is_conf_participant = FALSE;

        /* Clearing sequence number for conference participants if any */
        if(call_da_val->conf_linked_call_id == call_id && call_da_val->
etsicall_num_participant_in_conf > 0)
        {
          for(index = 0; index < CM_CALL_ID_MAX; index++)
          {
            if(call_da_val->dsat_seqnum_callid[index].is_conf_participant == TRUE)
            {
              call_da_val->dsat_seqnum_callid[index].call_id = CM_CALL_ID_INVALID;
              call_da_val->dsat_seqnum_callid[index].call_type = CM_CALL_TYPE_NONE;
              call_da_val->dsat_seqnum_callid[index].is_conf_participant = FALSE;
            }
          }
        }
#endif /* FEATURE_DSAT_VOLTE_MPTY */		
      }
      break;
    default:
      DS_AT_MSG1_ERROR("Unanticipated event %d",call_event);
      break;
  }
  return seq_number;
}/* dsatact_track_sequence_number */

