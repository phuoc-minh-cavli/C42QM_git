/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                          PHONEBOOK MANAGER UIM ENTRIES

GENERAL DESCRIPTION
  This file contains the PBM wrapper functions for GSDI

  Copyright (c) 2002 - 2016 by QUALCOMM Technologies, Inc(QTI).
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
                        EDIT HISTORY FOR MODULE
$Header: //components/rel/uim.mpss/3.3.1/pbm/src/pbmgsdi.c#1 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/29/18    tq     Deprecated MMGSDI_TERMINAL_PROFILE_DL_EVT and MMGSDI_CARD_REMOVED_EVT 
09/13/16    sn     Fix compilation error
06/23/16    nr     ECC optimizations for IoE devices
05/26/16    sp     Compilation Fix
05/26/16    sp     Logging improvements
03/10/16    nr     Refresh FCN should not be processed for a session if it is
                   already present in pbm initialization queue
02/06/16    nr     ECC only support in PBM
10/26/15    nr     Disable MMGSDI PB cache for PBM module
04/03/15   stv     Migrate some efs access to uim_common_efs API
08/05/15    nr     Operator specifc NV ECC numbers configuration
07/20/15    nr     Enter & leave critical section inside pbm_ecc_set_hardcoded_eccs()
07/02/15    nr     Validate BCD number and EXT data length
06/09/15   stv     Fix to address multiple initialization in parallel on card session
03/10/15    nr     Clearing slot id in session de-activation handling
11/19/14   stv     Fix to build ECC hardcode cache between card power down and power ups
11/14/14   stv     Need not send refresh start and refresh end indication to client during session init
10/13/14    NR     Added NULL check while accessing EXT cache and fixed the memory leak.
10/09/14    NR     Fix to eliminate redundant ECC indications for session
                   change deactivation followed by CARD_ERROR event
09/22/14    NR     GRP-ID should be validated before reading and writing to SIM
08/25/14   stv     Feature to maintain sim absent hardcoded ecc list till pin is verified
09/10/14    NR     Fix to query for WMS ready in case of REFRESH while provisioning 
                   session is ongoing
07/29/14    am     Correctly direct/redirect atomic_ops APIs across MOBs 
07/25/14    am     Added stubs for RFM APIs to fix linker errors
07/07/14    NR     Use v2.0 diag macros
05/30/14    NR     Resetting the globals to avoid sending SIM_INIT_DONE more 
                   than once per slot in REFRESH Scenarios.
05/16/14    NR     Enabling or Disabling ADN based on NV configurations
05/01/14    NR     FR 19033: Reduce Code Duplication pbm
03/06/14    stv    CM SS event handling changes for SGLTE 
02/16/14    NR     PBM/QMI_PBM KW Critical warnings
02/08/14    NR     1X session INIT stopped in ICC mode
1/23/14    stv     FDN number match successful for any dialed number if FDN list has a record with BCD length 
                   as non-0xFF and actual BCD number is all set to 0xFF 
01/21/14   NR      PBM/QMI_PBM valid_pb_mask is not resetting after REFRESH EVENT
01/09/14   NR      potential memory leak issues in PBM funcions
01/03/14   NR      PBM doesn't need to wait for WMS ready in case of 1x_secondary 
                   and 1x_teritary sessions to proceed with the intilzation.
12/19/13   NR      PBM: LLVM compiler warnings
10/28/13   stv     Fix for PBM init halted for invalid group /index record length 
10/10/13   stv     Fix for NULL check klockwork errors
10/04/13   stv     F3 message reduction 
10/03/13  nalavala memcpy banned api replacement with safer version memscpy
09/20/13  nalavala The Txid should be cleard for slot on which we got card error event
09/16/10  krishnac Refresh by path changes
04/30/10  krishnac Read from root adn when app_type is 2G(241978)
04/30/10  krishnac Do not close the sessions during card errors
                     and reuse them for future valid sessions
04/30/10  krishnac initialize the caches with proper pb_id
03/18/10   krishnac Fixed crash CR 231076 by populating the proper slot
02/02/10   krishnac CR 225159 Call to number in EF-ECC is initiated as Normal Voice call
11/05/09   krishnac CR 211973  MO live Call fails if FDN enabled RUIM is used
                     (Fixed issue populating file access method)
10/14/09  krishnac Support for feature DSDS
07/10/09  ashwanik Support for CSIM and Dual Sim Features
05/04/09   krishna changes for CR 167785
04/01/09   kcp     Fixed CR 169091 - Terminal does not read PB files during power up.
16/03/09   kcp     resolved Lint errors
02/02/09   kcp     165174 - After deleting a contact,
                   2nd byte of EF-ANR is 00 instead of FF
09/04/08   sg      Fix the SEEK/SEARCH pattern for GRP files in pbm_get_seek_pattern()
09/03/08   sg      Add proper featurization and multiple phonebook support in
                   pbm_get_seek_pattern()
09/03/08   sg      Return a SEARCH/SEEK pattern of 0x00 for UID and PBC
08/28/08   sg      Featurize MMGSDI seek/search calls based on FEATURE_MMGSDI_ENHANCED_SEARCH
                   and read records sequentially if the feature is not defined
07/07/08   sg      Use mmgsdi_seek() to skip over unused records to improve
                   SIM initialization time
06/13/08   cvs     New GCF field trial docs move ECC number changes to SIM inserted
04/07/08   clm     Handle MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_FCN stage while
                   in MMGSDI_REFRESH_NAA_INIT_FCN mode for MMGSDI refresh events.
03/13/08   clm     Add notification of PBM_EVENT_PB_REFRESH_START/DONE for
                   both local and global USIM files.
03/13/08   clm     Add restart of SIM read init for enable/disable FDN
                   Free pbm_buffer from read cb func if restart is needed.
01/17/08   clm     Add FEATURE_PBM_USIM_SUPPORT to featurize USIM features.
11/02/07   clm     Add refresh okay and init complete support to pbm for mmgsdi.
08/31/07   cvs     Fix issue with fcn during init
05/18/07   cvs     Make pbm_handle_sim_init_completed() public
03/12/07   cvs     Featurize 4 PB set support
02/23/07   cvs     Handle PIN enable/disable only as needed.
02/22/07   cvs     Keep track of filetype for read + write ops
02/09/07   cvs     Support for 4 phonebook sets
12/16/06   cvs     Only respond to PIN events for the provisioning app
12/15/06   cvs     Register for refresh on all files, in one call to fix refresh
11/01/06   cvs     multi pb set fixes
10/24/06   cvs     Register for refresh on all files, to fix FDN refresh issue
10/10/06   cvs     Move messages to PBM's diag msg ID
08/17/06   cvs     Fix race condition with GSDI refresh before done initializing.
07/25/06   slw     Added error reporting for pbm_create_sim_record
07/24/06   cvs     Fixes for unequal size multi pb set support
06/14/06   aps     support for ANRC and ANRC1 type records
05/03/06   cvs     convert to MMGSDI APIs.  Lint cleanup.
03/24/06   cvs     %x -> 0x%x in log messages
02/23/06   cvs     rework memory allocation macros
02/09/06   cvs     When can't encode all characters on SIM, return errors
02/07/06   cvs     better log messages
01/27/06   cvs     don't add UIM_NO_SUCH_ITEMs to refresh list,
                   make refresh list decisions based on USIM local PB
01/09/06   cvs     Lint fixes
12/13/05   cvs     Implement partial PB refresh
12/06/05   cvs     Wait to init sim phonebooks until after WMS
11/16/05   cvs     Lint Fixes
09/30/06   cvs     Fix error message
09/14/05   cvs     Properly handle empty email entries.
08/22/05   AT      Moved contents inside of a FEATURE_PBM_TASK.
08/12/05   AT      Added UCS2 support for strings.
08/12/05   cvs     When extension records go unused, write out blank record
07/19/05   AT      Fix issue where PIN becomes blocked and unblocked
                   after SIM_INIT_COMPLETED.  A bit of code cleanup.
07/11/05   cvs     Better error messages
06/24/05   cvs     Fix dual IMSI FDN support
06/14/05   cvs     Fix GCF failures related to emergency numbers
03/28/05   AT      Updated ECC requirements.
03/08/05   PA      Support for PBM notifications.
02/17/05   PA      Made alphabet translation table constant.
02/14/05   PA      Do not stop initialization if extension record type is
                   not recognized.
01/18/05   PA      Convert 7-bit SMS default alphabet to PBM 8-bit ascii and
                   fix compilation errors for non-MMGSDI builds
01/14/05   PA      Make PBM GSTK unaware
01/14/05   PA      Support for non-MMGSDI builds
01/12/05   PA      PBM USIM Support
11/19/04   PA      Support non-GSDI builds, code review comments
09/29/04   PA      Initial Revision
===========================================================================*/
#include "uim_variation.h"
#include "customer.h"
#include "pbm_common_types.h"
#include "pbm.h"
#include "time_jul.h"
#include "string.h"
#include "pbmgsdi.h"
#include "pbmutils.h"
#include "pbmtask_i.h"
#include "pbm_conversion.h"
#include "pbmuim.h"
#include "atomic_ops.h"
#include "pbmefs.h"
#include "stringl/stringl.h"
#include "wms.h"
#include "pbmlib.h"
#include "cmutil.h"
#include "uim_msg.h"
#include "uim_pbm.h"
#ifdef FEATURE_MODEM_CONFIG_REFRESH
#include "pbm_nv_refresh.h"
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

/*===========================================================================
                         DEFINITIONS
===========================================================================*/
/* Global array to store the card status 
 * pbm_card_status set --- In card inserted event 
 * pbm_card_status Reset -- In card error event
 */
boolean pbm_card_status[PBM_MAX_SLOTS];
extern boolean pbm_efs_disable;

uint32         pbm_hidden_key_verified = 0;

atomic_word_t pbm_pin1_verified; /* to know if PIN1 is verified for particular session */
atomic_word_t pbm_pin2_verified; /* to know if PIN2 is verified for particular session */

/*===========================================================================*
 *              PHONE BOOK MANAGER TO GSDI INTERFACE FUNCTIONS.              *
 *              These function are only used internally by PBM               *
 *              and provide the interface to GSDI.                           *
 *===========================================================================*/

/*===========================================================================
FUNCTION PBM_MMGSDI_FDN_EXT_READ_CB

DESCRIPTION
  This function is registered with GSDI. A PBM command is added to the
  gsdi_resp queue when we receive this event.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
static void pbm_mmgsdi_fdn_ext_read_cb(
  mmgsdi_return_enum_type status,
                                  mmgsdi_cnf_enum_type    data_type,
  const mmgsdi_cnf_type  *data_ptr
)
{
  pbm_mmgsdi_resp_s_type *mmgsdi_resp_ptr   = NULL;
  mmgsdi_len_type         read_cnf_data_len = 0;

   PBM_CHECK_PTR_RET(data_ptr, VOID);

   mmgsdi_resp_ptr = (pbm_mmgsdi_resp_s_type *)
                     pbm_cmd_alloc(sizeof(pbm_mmgsdi_resp_s_type));
  if (!mmgsdi_resp_ptr)
{
    UIM_MSG_ERR_0("Unable to allocate PBM command");
    (void) rex_set_sigs(PBM_TCB, PBM_CMD_COMPLETE_SIG);
   return;
}
  mmgsdi_resp_ptr->cmd_type = PBM_MMGSDI_FDN_EXT_READ_CB;
  mmgsdi_resp_ptr->status = status;
  mmgsdi_resp_ptr->data_type = data_type;
  memscpy(&mmgsdi_resp_ptr->data, sizeof(mmgsdi_cnf_type), data_ptr, sizeof(mmgsdi_cnf_type));
  if (data_ptr->read_cnf.read_data.data_len > 0)
{
    read_cnf_data_len = data_ptr->read_cnf.read_data.data_len;
    PBM_MEM_ALLOC(mmgsdi_resp_ptr->data.read_cnf.read_data.data_ptr,
                  read_cnf_data_len);
    PBM_CHECK_PTR_RET(mmgsdi_resp_ptr->data.read_cnf.read_data.data_ptr,VOID);
    memscpy((void *)mmgsdi_resp_ptr->data.read_cnf.read_data.data_ptr, read_cnf_data_len,
            (void *)data_ptr->read_cnf.read_data.data_ptr,
            read_cnf_data_len); //deep copy
   }
   else
   {
    mmgsdi_resp_ptr->data.read_cnf.read_data.data_ptr = NULL;
}
  pbm_mmgsdi_resp_q_put(PBM_CMD_PTR(mmgsdi_resp_ptr));
} /* pbm_mmgsdi_fdn_ext_read_cb */


/*===========================================================================
FUNCTION PBM_GSDI_ASYNC_CB

DESCRIPTION
  This function is registered with GSDI. A PBM command is added to the
  gsdi_resp queue when we receive this event.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_mmgsdi_async_cb(mmgsdi_return_enum_type status,
                         mmgsdi_cnf_enum_type    data_type,
                         const mmgsdi_cnf_type  *data_ptr)
{
   pbm_mmgsdi_resp_s_type *mmgsdi_resp_ptr;
   uint32 client_ref;
   uint16 read_cnf_data_len = 0;

   PBM_CHECK_PTR_RET(data_ptr, VOID);

   mmgsdi_resp_ptr = (pbm_mmgsdi_resp_s_type *)
                     pbm_cmd_alloc(sizeof(pbm_mmgsdi_resp_s_type));
   if (!mmgsdi_resp_ptr)
   {
      UIM_MSG_ERR_0("Unable to allocate PBM command");
      (void) rex_set_sigs(PBM_TCB, PBM_CMD_COMPLETE_SIG);
      return;
   }

   client_ref = data_ptr->response_header.client_data;
   if ((client_ref >= PBM_ASYNC_BUFFERS) || (!pbm_buffers[client_ref].in_use))
   {
      UIM_MSG_ERR_1("Invalid client_ref found: %d\n", client_ref);
      pbm_cmd_free(mmgsdi_resp_ptr);
      (void) rex_set_sigs(PBM_TCB, PBM_CMD_COMPLETE_SIG);
      return;
   }

   mmgsdi_resp_ptr->cmd_type = pbm_buffers[client_ref].gsdi_resp_type;

   mmgsdi_resp_ptr->status = status;
   mmgsdi_resp_ptr->data_type = data_type;
   memscpy(&mmgsdi_resp_ptr->data, sizeof(mmgsdi_cnf_type), data_ptr, sizeof(mmgsdi_cnf_type));




   if (PBM_MMGSDI_READ_CB == mmgsdi_resp_ptr->cmd_type
       || PBM_DEL_LINKS_READ_CB  == mmgsdi_resp_ptr->cmd_type)
   {
      read_cnf_data_len = data_ptr->read_cnf.read_data.data_len;
      if (data_ptr->read_cnf.read_data.data_len > 0)
      {
        PBM_MEM_ALLOC(mmgsdi_resp_ptr->data.read_cnf.read_data.data_ptr,
                                                       read_cnf_data_len);
        PBM_CHECK_PTR_RET(mmgsdi_resp_ptr->data.read_cnf.read_data.data_ptr,VOID);

         memscpy((void *)mmgsdi_resp_ptr->data.read_cnf.read_data.data_ptr, read_cnf_data_len,
                (void *)data_ptr->read_cnf.read_data.data_ptr,
                read_cnf_data_len); //deep copy
      } else
      {
         mmgsdi_resp_ptr->data.read_cnf.read_data.data_ptr = NULL;
      }
   } else if (PBM_MMGSDI_WRITE_CB != mmgsdi_resp_ptr->cmd_type)
   {
      //print error
      UIM_MSG_ERR_1("Unexpected cmd_type %d", mmgsdi_resp_ptr->cmd_type);
   }

   pbm_mmgsdi_resp_q_put(PBM_CMD_PTR(mmgsdi_resp_ptr));

} /*pbm_mmgsdi_async_cb...*/


/*===========================================================================
FUNCTION pbm_session_deactivate_handling

DESCRIPTION
This function handles session deactivation event from MMGSDI.

RETURN VALUE
  void.

DEPENDENCIES

===========================================================================*/
void pbm_session_deactivate_handling(
  pbm_session_enum_type deact_session_type,
  uint16                slot_id
)
{
  pbm_session_enum_type  deact_session_complement = PBM_SESSION_DEFAULT;
  pbm_session_enum_type  card_session             = PBM_SESSION_DEFAULT;
  int                    q_id                     = 0;

  q_id = slot_id - 1;
  if(q_id < 0 || q_id >= PBM_MAX_SLOTS)
  {
    UIM_MSG_ERR_1("session_deactivate_handling invalid q_id %d", q_id);
    return;
  }
  
  pbm_clear_and_set_session_caches(deact_session_type,PBM_STATUS_NOT_INIT,FALSE);
  pbm_stop_fileinfo_internal(deact_session_type);

  /* get session_index for GPB for same (slot) first */
  card_session = pbm_pb_type_and_slot_to_session_type(
                                          pbm_session_info[deact_session_type].slot_id,
                                          PBM_GPB);

  UIM_MSG_HIGH_3("session_deactivate_handling curr_q[q_id] %d card_session %d q_id %d",
                curr_q[q_id],card_session,q_id);
  
  if(curr_q[q_id] == deact_session_type) 
  {
    curr_q[q_id] = PBM_SESSION_MAX;
  }

  if(pbm_if_queue_has_sess(deact_session_type,q_id))
  {
    UIM_MSG_HIGH_1("session_deactivate_handling removing session from queue deact_session_type %d",
                    deact_session_type);

    /* Remove deactivate_session from queue */
    pbm_remove_deactivate_sess_from_queue(deact_session_type,q_id);
  }

  deact_session_complement = pbm_find_provioning_sesssion_in_queue(deact_session_type);

  UIM_MSG_HIGH_3("session_deactivate_handling deact_session_complement 0x%x init status 0x%x card_session 0x%x",
                 deact_session_complement,
                 (deact_session_complement == PBM_SESSION_MAX)?0:pbm_init_info.pbm_session_init_done[deact_session_complement],
                 card_session);

  /* Initlize ECC NV per card session only if no provisiong is activated in a slot */
  /* Skip ECC ready indication if an card error received before session de-activation */
  if(deact_session_complement == PBM_SESSION_MAX &&
     pbm_card_status[slot_id - 1])
  {
    pbm_build_slot_specific_ecc_cache(card_session, EMERGENCY_GW_1X, 
                                      PBM_NV_EMERGENCY_NUMBERS_PER_SUB);
  }
  
  /* Clearing slot info as well */
  pbm_session_info[deact_session_type].slot_id  = 0 ;
  if(deact_session_complement == PBM_SESSION_MAX ||
     !(pbm_if_queue_has_sess(deact_session_complement,q_id) || 
       curr_q[q_id] == deact_session_complement || 
      (pbm_init_info.pbm_session_init_done[deact_session_complement] == TRUE)))
  {
    pbm_clear_and_set_session_caches(card_session,PBM_STATUS_NOT_INIT,FALSE);
    pbm_stop_fileinfo_internal(card_session);

    /* Remove card_session from queue */
    pbm_remove_deactivate_sess_from_queue(card_session,q_id);
    if(curr_q[q_id] == card_session)
    {
      curr_q[q_id] = PBM_SESSION_MAX;
    }
  }
  else
  {
    if(pbm_is_queue_empty(q_id))
    {
      pbm_session_enum_type temp_session = curr_q[q_id];

      UIM_MSG_HIGH_1("PBM session_deactivate_handling queue empty :temp_session_type 0x%x", temp_session);
      pbm_session_q_init(slot_id);
      curr_q[q_id] = temp_session;
      return;
    }
    else
    {
      if(curr_q[q_id] == deact_session_complement)
      {
        UIM_MSG_HIGH_2("PBM curr_session %d deact_session_complement %d",
                      curr_q[q_id],deact_session_complement);
      }
      else
      {
        curr_q[q_id] = pbm_q_dequeue(q_id);

        UIM_MSG_HIGH_2("PBM session deactivate handling q_id 0x%x curr_q[q_id] 0x%x",
                        q_id, curr_q[q_id]);
        if( curr_q[q_id] != PBM_SESSION_MAX )
        {
          if(pbm_session_type_to_pb_category(curr_q[q_id]) == PBM_GPB && 
             !pbm_session_info[curr_q[q_id]].isActive)
          {
            /* Setting back the isActive flag to TRUE If the dequeued session is a card session and
             * if it is not set already in case of multiapp card in single slot 
             */
            pbm_session_info[curr_q[q_id]].isActive = TRUE;
          }
          pbm_session_phonebook_init( curr_q[q_id] );
        }
      }
    }
  }
}/* pbm_session_deactivate_handling */


/*===========================================================================
FUNCTION PBM_CARD_STATUS_CB

DESCRIPTION
  Callback function to return SIM card status.
  Note:  MMGSDI returns the most recent event at registration.  So, our first
  event may be MMGSDI_CARD_INSERTED_EVT, or it may be MMGSDI_CARD_INIT_COMPLETED_EVT

RETURN VALUE
  void.

DEPENDENCIES
  This routine is not reentrant.  Shouldn't be a problem, as it doesn't exit
  till we're done, and it's only called from the PBM task.
===========================================================================*/
void pbm_card_status_cb(
  mmgsdi_event_data_type   mmgsdi_event
)
{
  pbm_slot_type                   slot         = 0;
  mmgsdi_return_enum_type         ret          = MMGSDI_SUCCESS;
  int                             index        = 0;
  pbm_session_enum_type           session_type = PBM_SESSION_DEFAULT;
  boolean                         new_init     = FALSE;

  session_type = pbm_mmgsdi_session_to_pbm_session(mmgsdi_event.session_id);
      
  if(MMGSDI_CARD_INSERTED_EVT == mmgsdi_event.evt ||
     MMGSDI_SESSION_CHANGED_EVT == mmgsdi_event.evt ||
     MMGSDI_SUBSCRIPTION_READY_EVT == mmgsdi_event.evt)
  {
    new_init = TRUE;
  }

  //except for MMGSDI_REFRESH_NAA_FCN
  //all refresh modes will have MMGSDI_SUBSCRIPTION_READY_EVT followed   
  if(MMGSDI_REFRESH_EVT == mmgsdi_event.evt &&
     (mmgsdi_event.data.refresh.mode != MMGSDI_REFRESH_NAA_FCN && 
      mmgsdi_event.data.refresh.stage == MMGSDI_REFRESH_STAGE_START))
  {
    new_init = TRUE;
  }

  if(new_init == TRUE)
  {
    PBM_SESSION_RESET(pbm_session_activated, session_type);
    PBM_SESSION_RESET(pbm_wms_init_finished,session_type);
  }

  switch (mmgsdi_event.evt)
  {
    case MMGSDI_CARD_INSERTED_EVT:
    {
      pbm_session_enum_type card_sess = PBM_SESSION_DEFAULT ;

      slot = PBM_MMGSDI_SLOT_TO_PBM_SLOT(mmgsdi_event.data.card_inserted.slot);
      UIM_MSG_HIGH_1("PBM received MMGSDI_CARD_INSERTED_EVT for slot 0x%x", slot);
      if(slot <= 0 || slot > PBM_MAX_SLOTS)
      {
        break;
      }

      pbm_card_status[slot - 1] = TRUE;

      /* Initilizing the queue globals per slot */
      pbm_session_q_init(slot);
      card_sess = pbm_pb_type_and_slot_to_session_type(slot,PBM_GPB);

      rex_enter_crit_sect(&pbm_crit_sect);

      pbm_session_info[card_sess].slot_id = (uint16)slot ; 
      for (index = 0; index < (int) MAX_UIM_APPS; index++ )
      {
        /* clear caches in all sessions present on this slot */
        if ((slot == (pbm_slot_type)pbm_session_info[index].slot_id ) &&
              !(pbm_session_lpm_status & PBM_SESSIONID_TO_SESSION_MASK(index)))
        {
          pbm_ecc_clear_all_id((pbm_session_enum_type)index,
                                 PBM_FIELD_SIM_ECC);
          pbm_ecc_clear_all_id((pbm_session_enum_type)index,
                                  PBM_FIELD_NETWORK_ECC);
        }
      }

      rex_leave_crit_sect(&pbm_crit_sect);

      for (index = 0; index < (int) MAX_UIM_APPS; index++ )
      {
        if((slot == (pbm_slot_type)pbm_session_info[index].slot_id ) &&
            !(pbm_session_lpm_status & PBM_SESSIONID_TO_SESSION_MASK(index)))
        {
          /* Initlize ECC NV per SUB */
          pbm_build_slot_specific_ecc_cache(index, EMERGENCY_GW_1X, 
                                            PBM_NV_EMERGENCY_NUMBERS_PER_SUB);

          /* need to set hardcoded eccs as if card is present, and LPM mode is not ON */
          pbm_ecc_set_hardcoded_eccs(TRUE);
          break;
        }
      }

      pbm_set_session_caches_status(pbm_pb_type_and_slot_to_session_type(slot,PBM_GPB),
                                    PBM_STATUS_NOT_READY );
      for (index = 0; index < (int) MAX_UIM_APPS; index++ )
      {
        if (pbm_session_type_to_pb_category( (pbm_session_enum_type)index ) == PBM_LPB &&
             !(pbm_session_activated & PBM_SESSIONID_TO_SESSION_MASK(index)))
        {
          pbm_set_session_caches_status(index,PBM_STATUS_NOT_READY);
        }
      }

      for (index = 0; index < (int) MAX_UIM_APPS; index++ )
      {
        if((slot == (pbm_slot_type)pbm_session_info[index].slot_id ) &&
            !(pbm_session_lpm_status & PBM_SESSIONID_TO_SESSION_MASK(index)))
        {
          PBM_SET_PB_CACHE_READY(&ecc_other_cache); /* cache ready for hardcoded #s */
          break;
        }
      }
    }
    break;  //MMGSDI_CARD_INSERTED_EVT
    case MMGSDI_ECC_EVT:
    {
      pbm_session_enum_type          pbm_card_session = PBM_SESSION_GPB_1;

      UIM_MSG_HIGH_3("PBM received MMGSDI_ECC_EVT slot %d for session %d, num ecc %d.",
                      mmgsdi_event.data.ecc.slot,
                      session_type,
                      mmgsdi_event.data.ecc.ecc_count);

      rex_enter_crit_sect(&pbm_crit_sect);

      /* Get session_index for GPB for same (slot) first */
      pbm_card_session = pbm_pb_type_and_slot_to_session_type(
                                   pbm_session_info[session_type].slot_id,
                                   PBM_GPB);

      /* clear NV ecc per sub no's if any */
      pbm_ecc_clear_all_id(pbm_card_session, PBM_FIELD_SIM_ECC);
      rex_leave_crit_sect(&pbm_crit_sect);

      /* Make sure we free the memory which was allocated in deep copy in the below function.*/
      pbm_update_sim_ecc_cache(session_type,&mmgsdi_event.data.ecc);
    }
    break;
    case MMGSDI_PIN1_EVT:
    {
            boolean found_another = FALSE;
            uint8 l_i;
            pbm_session_enum_type card_session = PBM_SESSION_GPB_1;
            atomic_word_t   pin1_prev_status; 

            pin1_prev_status.value  = pbm_pin1_verified.value ; 

            UIM_MSG_HIGH_3("PBM received MMGSDI_PIN1_EVT with status  %d for session %d, for pin %d.",
                         mmgsdi_event.data.pin.pin_info.status,
                         session_type,
                         mmgsdi_event.data.pin.pin_info.pin_id);
            if( pbm_session_type_to_pb_category(session_type) != PBM_GPB  )
            {
              UIM_MSG_HIGH_2("PIN app_type %d for slot %d",
                           mmgsdi_event.data.pin.aid_type[0].app_type,
                           mmgsdi_event.data.pin.slot);
              pbm_session_info[session_type].app_type = mmgsdi_event.data.pin.aid_type[0].app_type;
	            pbm_session_info[session_type].slot_id = mmgsdi_event.data.pin.slot;
            }

      PBM_VALIDATE_SESSION_RET(session_type, VOID);

            //since PIN events would come only for PROV sessions
            //we need to take the appropriate action on
            //the card session that belonged to this prov session
            switch( mmgsdi_event.data.pin.slot )
            {
               case MMGSDI_SLOT_1 :
                 card_session = PBM_SESSION_GPB_1;
               break;

               case MMGSDI_SLOT_2 :
                 card_session = PBM_SESSION_GPB_2;
               break;

#ifdef FEATURE_TRIPLE_SIM
               case MMGSDI_SLOT_3 :
                 card_session = PBM_SESSION_GPB_3;
               break;
#endif /* FEATURE_TRIPLE_SIM */
               default :
               break;
            }

            for(l_i = (uint8)PBM_SESSION_GPB_1;l_i < (uint8)PBM_SESSION_MAX; l_i++)
            {
              //check if there exists 2nd provisioning session on the same slot
              //if not we should clear the appropriate card session as well
              if(pbm_session_type_to_pb_category((pbm_session_enum_type)l_i) == PBM_LPB && l_i != (uint8)session_type)
              {
                if(pbm_session_info[l_i].isActive == TRUE &&
                   pbm_session_info[l_i].slot_id == (uint16) PBM_MMGSDI_SLOT_TO_PBM_SLOT(mmgsdi_event.data.pin.slot))
                  found_another = TRUE;
              }
            }

            switch ( mmgsdi_event.data.pin.pin_info.status )
            {
               case  MMGSDI_PIN_PERM_BLOCKED :
               {
                  if(TRUE == pbm_session_info[session_type].isActive)
                  {
                    pbm_stop_fileinfo_internal(session_type);
                  }
                  //need to clear caches except ECC
                  pbm_clear_and_set_session_caches(session_type, PBM_STATUS_SIM_FAIL, FALSE);
                  PBM_SESSION_ATOMIC_RESET(pbm_pin1_verified, session_type);
                  if(!found_another)
                  {
                    pbm_stop_fileinfo_internal(card_session);
                    pbm_clear_and_set_session_caches(card_session, PBM_STATUS_SIM_FAIL, FALSE);
                    PBM_SESSION_ATOMIC_RESET(pbm_pin1_verified, card_session);
                  }
               }
               break;

               case MMGSDI_PIN_ENABLED_NOT_VERIFIED :
               {
                   //no need to restart if only ECC being read
                   if(TRUE == pbm_session_info[session_type].isActive)
                   {
                      pbm_stop_fileinfo_internal(session_type);
                   }
                   //need to clear caches except ECC
                   pbm_clear_and_set_session_caches(session_type, PBM_STATUS_PIN_REQ, FALSE);
                   PBM_SESSION_ATOMIC_RESET(pbm_pin1_verified, session_type);
                   if(!found_another)
                   {
                     pbm_stop_fileinfo_internal(card_session);
                     pbm_clear_and_set_session_caches(card_session, PBM_STATUS_PIN_REQ, FALSE);
                     PBM_SESSION_ATOMIC_RESET(pbm_pin1_verified, card_session);
                   }
               }
               break;

               /* there can be 2 possibilities for event MMGSDI_PIN_ENABLED_VERIFIED
                * 1 - Earlier PIN is enabled and not verified , now it is verified
                * 2 - SIM doesnt need PIN verification and now PIN is
                * enabled to prompt for verification in the next powerup
                * for case 2 we dont need to take any action since not applicable for PBM
                */
               case MMGSDI_PIN_ENABLED_VERIFIED :
               {
                  //here we should just mark the caches as not ready.
                  //However subscription ready follows this
                  //and PBM init would be triggred from that
                  if(pbm_session_info[session_type].isActive == FALSE)
                  {
                     pbm_clear_and_set_session_caches(session_type, PBM_STATUS_NOT_READY, FALSE);
                     if(!found_another)
                     {
                        pbm_clear_and_set_session_caches(card_session, PBM_STATUS_NOT_READY, FALSE);
                     }
                  }
                  PBM_SESSION_ATOMIC_SET(pbm_pin1_verified, session_type);
                  /* in case of enabled verified no need to check other  app's status for pin1_verified on same slot */
                  PBM_SESSION_ATOMIC_SET(pbm_pin1_verified, card_session);
               }
               break;

               case MMGSDI_PIN_BLOCKED :
               {
                  //isActive flag tells us whether we have recieved this event during powerup (OR)
                  //this PIN is blocked after MMGSDI_SUBSCRIPTION_READY_EVT
                  if((TRUE == pbm_init_info.pbm_session_init_done[session_type]) &&
                                         (TRUE == pbm_session_info[session_type].isActive))
                  {
                     //in this case the session is already initialized ,
                     //dont flush the caches , instead just mark the status as PBM_STATUS_READY_PUC_REQ
                     //when we recieve MMGSDI_PIN_UNBLOCKED event we can just mark them back to PBM_STATUS_READY
                     pbm_set_session_caches_status(session_type, PBM_STATUS_READY_PUC_REQ);
                  }
                  else
                  {
                     //session is in initialization process
                     //since anyway it is not completed we will flush all the caches
                     //and restart the initialization after we get MMGSDI_PIN_UNBLOCKED event
                     if(TRUE == pbm_session_info[session_type].isActive)
                     {
                        pbm_stop_fileinfo_internal(session_type);
                     }
                     pbm_clear_and_set_session_caches(session_type, PBM_STATUS_PUC_REQ, FALSE);
                  }
                  PBM_SESSION_ATOMIC_RESET(pbm_pin1_verified, session_type);
                  if(!found_another)
                  {
                     if((TRUE == pbm_init_info.pbm_session_init_done[card_session]) &&
                                        (TRUE == pbm_session_info[card_session].isActive))
                     {
                        pbm_set_session_caches_status(card_session, PBM_STATUS_READY_PUC_REQ);
                     }
                     else
                     {
                        if(TRUE == pbm_session_info[card_session].isActive)
                        {
                           pbm_stop_fileinfo_internal(card_session);
                        }
                        pbm_clear_and_set_session_caches(card_session, PBM_STATUS_PUC_REQ, FALSE);
                     }
                     PBM_SESSION_ATOMIC_RESET(pbm_pin1_verified, card_session);
                  }
               }
               break;

               case MMGSDI_PIN_UNBLOCKED :
               {
                  if((TRUE == pbm_init_info.pbm_session_init_done[session_type]) &&
                                      (TRUE == pbm_session_info[session_type].isActive))
                  {
                      //in this case the session is already initialized ,
                      //we can just mark them back to STATUS_READY
                      pbm_set_session_caches_status(session_type, PBM_STATUS_READY);
                  }
                  else if(TRUE == pbm_session_info[session_type].isActive)
                  {
                     //session is in initialization process
                     //restart the initialization
                     pbm_session_info[session_type].isActive = FALSE;
                     pbm_handle_app_init_completed(pbm_session_info[session_type].session_id,
                                                   mmgsdi_event.data.pin.slot,
                                                   pbm_session_info[session_type].app_type);
                  }
                  if(!found_another)
                  {
                     if((TRUE == pbm_init_info.pbm_session_init_done[card_session]) &&
                                        (TRUE == pbm_session_info[card_session].isActive))
                    {
                       pbm_set_session_caches_status(card_session, PBM_STATUS_READY);
                    }
                    else if(TRUE == pbm_session_info[card_session].isActive)
                    {
                      if ( pbm_session_initializing & PBM_SESSIONID_TO_SESSION_MASK(card_session))
                      {
                        UIM_MSG_HIGH_1("Already session init %d",card_session);
                        PBM_SESSION_SET(pbm_session_restart, card_session);
                      }
                      else
                      {
                         PBM_SESSION_SET(pbm_session_initializing, card_session);
                         pbm_session_phonebook_init(card_session);
                      }
                    }
                  }
                  PBM_SESSION_ATOMIC_SET(pbm_pin1_verified, session_type);
                  /* in case of pin unblocked no need to check other  app's status for pin1_verified on same slot */
                  PBM_SESSION_ATOMIC_SET(pbm_pin1_verified, card_session);
              }
              break;

              case MMGSDI_PIN_DISABLED :
              {
                PBM_SESSION_ATOMIC_SET(pbm_pin1_verified, session_type);
                PBM_SESSION_ATOMIC_SET(pbm_pin1_verified, card_session);
              }
              break;

              case MMGSDI_PIN_CHANGED :
        {
          /* changed is equivalent to verified case */
                 PBM_SESSION_ATOMIC_SET(pbm_pin1_verified, session_type);
                 PBM_SESSION_ATOMIC_SET(pbm_pin1_verified, card_session);
              }
              break;

              default :
                 break;
           }

           if( pbm_populate_ecc_other_if_pin1_status_change(pin1_prev_status) ) 
           {
             /* if hardcoded list is updated send ready */
             PBM_SET_PB_CACHE_READY(&ecc_other_cache);
           }
         }
         break;

      case MMGSDI_PIN2_EVT:
         {
            boolean found_another = FALSE;
            uint8 l_i;
            pbm_session_enum_type card_session = PBM_SESSION_GPB_1;

            UIM_MSG_HIGH_3("PBM received MMGSDI_PIN2_EVT with status  %d for session %d, for pin %d.",
                         mmgsdi_event.data.pin.pin_info.status,
                         session_type,
                         mmgsdi_event.data.pin.pin_info.pin_id);
            PBM_VALIDATE_SESSION_RET(session_type, VOID);
            //since PIN events would come only for PROV sessions
            //we need to take the appropriate action on
            //the card session that belonged to this prov session
            if(mmgsdi_event.data.pin.slot == MMGSDI_SLOT_1)
            {
              card_session = PBM_SESSION_GPB_1;
            }
            else if(mmgsdi_event.data.pin.slot == MMGSDI_SLOT_2)
            {
              card_session = PBM_SESSION_GPB_2;
            }
#ifdef FEATURE_TRIPLE_SIM
            else if(mmgsdi_event.data.pin.slot == MMGSDI_SLOT_3)
            {
              card_session = PBM_SESSION_GPB_3;
            }
#endif /* FEATURE_TRIPLE_SIM */
            for(l_i = (uint8)PBM_SESSION_GPB_1;l_i < (uint8)PBM_SESSION_MAX; l_i++)
            {
              //check if there exists 2nd provisioning session on the same slot
              if(pbm_session_type_to_pb_category((pbm_session_enum_type)l_i) == PBM_LPB && l_i != (uint8)session_type)
              {
                if(pbm_session_info[l_i].isActive == TRUE &&
                   pbm_session_info[l_i].slot_id == (uint16) PBM_MMGSDI_SLOT_TO_PBM_SLOT(mmgsdi_event.data.pin.slot))
                  found_another = TRUE;
                break; /* break to have the valid value for l_i */
              }
            }
            switch(mmgsdi_event.data.pin.pin_info.status)
            {
            case MMGSDI_PIN_PERM_BLOCKED:
            case MMGSDI_PIN_ENABLED_NOT_VERIFIED:
            case MMGSDI_PIN_BLOCKED:
              {
                PBM_SESSION_ATOMIC_RESET(pbm_pin2_verified, session_type);
                if(!found_another)
                {
                  PBM_SESSION_ATOMIC_RESET(pbm_pin2_verified, card_session);
                }
              }
              break;
            case MMGSDI_PIN_ENABLED_VERIFIED:
            case MMGSDI_PIN_UNBLOCKED:
            case MMGSDI_PIN_DISABLED:
              {
                PBM_SESSION_ATOMIC_SET(pbm_pin2_verified, session_type);
                /* in case of enabled verified no need to check other  app's status for pin2_verified on same slot */
                PBM_SESSION_ATOMIC_SET(pbm_pin2_verified, card_session);
              }
              break;
            default:
              break;
            }
         }
         break;

      case MMGSDI_CARD_ERROR_EVT:
         {
            uint8                           ecc_sim_airplane         = 0;
            boolean                         card_present_on_any_slot = FALSE;
            uim_common_efs_status_enum_type efs_status               = UIM_COMMON_EFS_ERROR;
            atomic_word_t              pin1_prev_status;
            pin1_prev_status.value   = pbm_pin1_verified.value ; 

            efs_status = uim_common_efs_read(UIM_COMMON_EFS_PBM_SIM_ECC_AIRPLANE,
                                             UIM_COMMON_EFS_ITEM_FILE_TYPE,
                                             UIM_COMMON_EFS_DEVICE,
                                             &ecc_sim_airplane,
                                            (uint32)sizeof(ecc_sim_airplane));
            
            if(efs_status != UIM_COMMON_EFS_SUCCESS)
            {
              UIM_MSG_MED_0("Failed reading pbm_sim_ecc_airplane - pick default");
            }

            slot = PBM_MMGSDI_SLOT_TO_PBM_SLOT(mmgsdi_event.data.card_error.slot);
            PBM_VALIDATE_SLOT_RET(slot, VOID);

            pbm_card_status[slot - 1] = FALSE;
            UIM_MSG_HIGH_2("PBM received MMGSDI_CARD_ERROR_EVT for slot %d, pbm_card_status %d",
                            slot, pbm_card_status[slot - 1]);
            UIM_MSG_HIGH_2("PBM ecc_sim_airplane nv %d and event %d",ecc_sim_airplane,mmgsdi_event.evt);

           /* Initilizing the queue globals per slot */
            pbm_session_q_init(slot);

            for (index = 0; index < (int) MAX_UIM_APPS; index++ )
            {
               /* clear caches in all sessions present on this slot */
               if ( slot == (pbm_slot_type)pbm_session_info[index].slot_id )
               {
                  pbm_stop_fileinfo_internal((pbm_session_enum_type)index);
                  pbm_clear_and_set_session_caches((pbm_session_enum_type)index, PBM_STATUS_NO_SIM, FALSE);
                  rex_enter_crit_sect(&pbm_crit_sect);
                  pbm_session_info[index].isActive = FALSE;
                  if ( ecc_sim_airplane && (mmgsdi_event.evt == MMGSDI_CARD_ERROR_EVT) && 
                      (mmgsdi_event.data.card_error.info == MMGSDI_CARD_ERR_PWR_DN_CMD_NOTIFY ))
                  {
                      PBM_SESSION_SET(pbm_session_lpm_status, index);
                  }
                  else
                  {
                      PBM_SESSION_RESET(pbm_session_lpm_status, index);
                      pbm_ecc_clear_all_id((pbm_session_enum_type)index, PBM_FIELD_SIM_ECC);
                      pbm_ecc_clear_all_id((pbm_session_enum_type)index, PBM_FIELD_NETWORK_ECC);
                  }

                  pbm_session_info[index].slot_id  = 0 ;
                  rex_leave_crit_sect(&pbm_crit_sect);
               }
            }

            if(pbm_card_status[PBM_SLOT_1 - 1] == TRUE || 
               pbm_card_status[PBM_SLOT_2 - 1] == TRUE || pbm_card_status[PBM_SLOT_3 - 1] == TRUE)
            {
              card_present_on_any_slot = TRUE;
            }

            UIM_MSG_HIGH_2("PBM card_present_on_any_slot 0x%x pbm_session_lpm_status 0x%x",
                            card_present_on_any_slot, pbm_session_lpm_status);
            if ( !card_present_on_any_slot && 
		         (!ecc_sim_airplane || (mmgsdi_event.evt != MMGSDI_CARD_ERROR_EVT) || 
                 (mmgsdi_event.data.card_error.info != MMGSDI_CARD_ERR_PWR_DN_CMD_NOTIFY) ))
            {
              pbm_ecc_set_hardcoded_eccs(FALSE);
              PBM_SET_PB_CACHE_READY(&ecc_other_cache);
            }
            else if ( card_present_on_any_slot )
            {
              if ( pbm_populate_ecc_other_if_pin1_status_change(pin1_prev_status) ) 
              {
                 PBM_SET_PB_CACHE_READY(&ecc_other_cache);
              }
            }
         }
         break;


      case MMGSDI_FDN_EVT:
         {
           pbm_session_enum_type pbm_card_session;
           /* Resetting the session_init_done for the provisioning and card session to
              avoid sending PBM_EVENT_SESSION_INIT_DONE twice */
           pbm_init_info.pbm_session_init_done[session_type] = FALSE;
           /* get session_index for GPB for same (slot) first */
           pbm_card_session = pbm_pb_type_and_slot_to_session_type(pbm_session_info[session_type].slot_id, PBM_GPB);
           pbm_init_info.pbm_session_init_done[pbm_card_session] = FALSE;
           UIM_MSG_HIGH_3("PBM received MMGSDI_FDN_EVT status %d  for session %d, rec num %d",
                         mmgsdi_event.data.fdn.enabled, session_type, mmgsdi_event.data.fdn.rec_num);
           PBM_VALIDATE_SESSION_RET(session_type, VOID);

            if(mmgsdi_event.data.fdn.rec_num != 0)
            {
              int buffer_index;
              PB_CACHE *cache = NULL;
              int extension_rec_num, iterator =0;
			  boolean pbm_waiting_write_cnf = FALSE;
			  pbm_phonebook_type pb_id = {PBM_GPB};
			  pbm_record_id_type rec_id;
              //if the card type is ICC though PBM gets FDN rec changed event on prov session
              //PBM need to update the corresponding card session
              if(USE_GSM_SUPPORT(session_type) || USE_CDMA_SUPPORT(session_type))
              {
                session_type = pbm_card_session;
              }

			  pb_id = pbm_file_id_to_pb_id(session_type, PBM_FILE_FDN);
			  rec_id = pbm_session_location_to_record_id(pb_id,mmgsdi_event.data.fdn.rec_num);
              /* if PBM recieves this event before the write cnf
			  has come from gsdi for FDN write originated via PBM.
			  In this case PBM dont need to do anything for this event.
			  PBM will update its commited list and caches when write
			  cnf event would come from GSDI */
              for(iterator = 0; iterator < PBM_ASYNC_BUFFERS; iterator++)
			  {
				  /* check all pbm buffers to see if the for rec_id write rsp cnf is awaited */
				  if( (pbm_buffers[iterator].in_use) && (pbm_buffers[iterator].state == PBM_WRITE_DONE)
				  	&& (pbm_compare_rec_ids(pbm_buffers[iterator].write->rec_id,rec_id) ))
				  {
                                    pbm_waiting_write_cnf = TRUE;
                                    UIM_MSG_HIGH_3("PBM waiting for write cnf for record %d  session %d, buffer index %d",
                                                    rec_id, session_type, iterator);
				  }
			  }
			  if(!pbm_waiting_write_cnf) /* PBM not waiting for write confirmation */
			  {
              cache = (PB_CACHE *) pbm_file_id_to_cache (session_type, PBM_FILE_FDN);
              //if the particular FDN cache is not yet ready,
              //we cannot update the cache,otherwise we need to restart the initialization of this session
              if (cache->status != PBM_STATUS_READY)
              {
                if (pbm_session_initializing & PBM_SESSIONID_TO_SESSION_MASK(session_type))
                {
                  UIM_MSG_HIGH_1("PBM Already init session %d.  Continue later.\n", session_type);
                  PBM_SESSION_SET(pbm_session_restart, session_type);
                  break;
                }
              }
              else
              {
                buffer_index = pbm_alloc_record();
                if(buffer_index < 0)
                {
                  UIM_MSG_ERR_0("Unable to alloc PBM buffer");
                  break;
                }
                pbm_buffers[buffer_index].record.index = mmgsdi_event.data.fdn.rec_num;
                if ( pbm_mmgsdi_process_sim_data(session_type,
                                                 PBM_FILE_FDN,
                                                 mmgsdi_event.data.fdn.rec_num,
                                                 mmgsdi_event.data.fdn.rec_data.data_ptr,
                                                 &pbm_buffers[buffer_index].record.data.num_text,
                                                 &extension_rec_num))
                {
                  pbm_addr_cache_s_type *prev_rec=NULL,*curr_rec=NULL;
                  boolean record_added = FALSE;
                  pbm_notify_event_e_type event = PBM_EVENT_REC_ADD;
                  pbm_session_event_data_u_type evt_data = {0};
                  /* Processing was complete. No ext record */
                  pbm_phonebook_type pb_id = {PBM_GPB};
                  pb_id = pbm_file_id_to_pb_id(session_type, PBM_FILE_FDN);
                  rex_enter_crit_sect(&pbm_crit_sect);
                  /* for fdn cache since fdn cache could be ready for this back door event, and some thread might be updating the fdn cache via record write */
                  prev_rec = cache->pb_cache_array[mmgsdi_event.data.fdn.rec_num];
                  if (cache->pb_cache_array[mmgsdi_event.data.fdn.rec_num] != NULL)
                  {
                     if ( pbm_cache_delete(cache, cache->pb_cache_array[mmgsdi_event.data.fdn.rec_num]->record_id) != PBM_SUCCESS )
                     {
                        rex_leave_crit_sect(&pbm_crit_sect);
                        break;
                     }
                  }
                  record_added = pbm_cache_record_add(pb_id,
                       &pbm_buffers[buffer_index].record.data.num_text,
                    mmgsdi_event.data.fdn.rec_num);
                  if (record_added)
                  {
                    curr_rec = cache->pb_cache_array[mmgsdi_event.data.fdn.rec_num];
                  }
                  rex_leave_crit_sect(&pbm_crit_sect);
                  if(record_added)
                  {
                    evt_data.rec_id = pbm_session_location_to_record_id(pb_id,mmgsdi_event.data.fdn.rec_num);
                    if(!prev_rec && curr_rec)
                    {
                      event = PBM_EVENT_REC_ADD;
                    }
                    else if(prev_rec && curr_rec)
                    {
                      event = PBM_EVENT_REC_UPDATE;
                    }
                    else if(prev_rec && !curr_rec)
                    {
                      event = PBM_EVENT_REC_DELETE;
                    }
                    pbm_add_notify(event, evt_data);
                  }
                  else
                  {
                    UIM_MSG_ERR_2("Unable to add FDN rec to cache, index %d session %d",
                                   mmgsdi_event.data.fdn.rec_num, session_type);
                  }
                  pbm_free_record((int) buffer_index);
                  break;
                }
                else
                {
                   if (extension_rec_num && cache->extension_cache &&
                       (cache->extension_cache->file_id != PBM_FILE_NONE))
                   {
                      if (pbm_send_read_request(session_type,
                                                cache->extension_cache->file_id,
                                                extension_rec_num,
                                                (int) buffer_index,
                                                pbm_mmgsdi_fdn_ext_read_cb))
                      {
                         break;
                      }
                      UIM_MSG_ERR_1("Could not issue read request %d", extension_rec_num);
                   } else
                   {
                      UIM_MSG_ERR_1("PBM could not find a valid extension record from cache 0x%x",
                                  cache);
                   }
                }
              }
            }
            //Clean up deep copy of record data
            PBM_MEM_FREEIF(mmgsdi_event.data.fdn.rec_data.data_ptr);
			}
            else if (mmgsdi_event.data.fdn.enabled)
            {
               pbm_fdn_enable(session_type);
            } else
            {
               pbm_fdn_disable(session_type);
            }
         }
         break;

      case MMGSDI_REFRESH_EVT:
         {
            UIM_MSG_HIGH_3("PBM received MMGSDI_REFRESH_EVT for session %d, mode %d, stage %d",
                         session_type,
                         mmgsdi_event.data.refresh.mode,
                         mmgsdi_event.data.refresh.stage);

            if (mmgsdi_event.data.refresh.mode == MMGSDI_REFRESH_NAA_FCN &&
                mmgsdi_event.data.refresh.stage == MMGSDI_REFRESH_STAGE_START)
            {
              /* Send refresh complete immediately if the session is already in PBM intilization
                 queue for which REFRESH FCN is received here */
              if(pbm_if_queue_has_sess(session_type, pbm_session_info[session_type].slot_id - 1))
              {
                /* This session Initialization is due to refresh - Send refresh complete to MMGSDI */
                (void) mmgsdi_session_refresh_complete(pbm_session_info[session_type].session_id,
                                                       TRUE,
                                                       pbm_mmgsdi_refresh_complete_cb,
                                                       0);
              }
              else
              {
                //Kick off pbm app fcn refresh
                pbm_refresh_file_change_notification_cb(&(mmgsdi_event.data.refresh.refresh_files),
                                                        mmgsdi_event.session_id);
              }
            } else if (((mmgsdi_event.data.refresh.mode == MMGSDI_REFRESH_NAA_INIT_FCN) ||
		        (mmgsdi_event.data.refresh.mode == MMGSDI_REFRESH_3G_SESSION_RESET) ||
                        (mmgsdi_event.data.refresh.mode == MMGSDI_REFRESH_NAA_FCN) )&&
                       (mmgsdi_event.data.refresh.stage == MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_FCN))
            {
               //Notify MMGSDI it's okay to proceed with refresh for the above modes of refresh
               ret = mmgsdi_session_ok_to_refresh(mmgsdi_event.session_id,
                                            TRUE,
                                            pbm_mmgsdi_ok_to_refresh_cb,
                                            0);
            } else if ( ( (mmgsdi_event.data.refresh.mode == MMGSDI_REFRESH_NAA_APP_RESET)
              || (mmgsdi_event.data.refresh.mode == MMGSDI_REFRESH_RESET) )
              && (mmgsdi_event.data.refresh.stage == MMGSDI_REFRESH_STAGE_START))
            {
              /* reset the LPM status variable, since in case of reset refresh, we need to read ecc again */
              PBM_SESSION_RESET(pbm_session_lpm_status, session_type);
            }

            if ( ret != MMGSDI_SUCCESS )
            {
               UIM_MSG_HIGH_1(" PBM: mmgsdi_session_ok_to_refresh returned error  %d",ret);
            }

            //Clean up deep copy of file list
            PBM_MEM_FREEIF(mmgsdi_event.data.refresh.refresh_files.file_list_ptr);
         }
         break;

      case MMGSDI_SESSION_CHANGED_EVT:
      {
        mmgsdi_app_info_type     app_info          = mmgsdi_event.data.session_changed.app_info;
        PB_CACHE                *ecc_cache         = NULL;
        atomic_word_t            pin1_prev_status;

        pin1_prev_status.value = pbm_pin1_verified.value ; 

        UIM_MSG_HIGH_3("PBM received MMGSDI_SESSION_CHANGED_EVT event on session %d app_type %d activated Status %d",
                        session_type, app_info.app_data.app_type,mmgsdi_event.data.session_changed.activated);
        if( pbm_session_type_to_pb_category(session_type) != PBM_LPB )
        {
          break;
        }

        if(mmgsdi_event.data.session_changed.activated == TRUE)
        {
          if( pbm_session_type_to_pb_category(session_type) != PBM_GPB )
          {
            pbm_session_info[session_type].app_type = app_info.app_data.app_type;
          }

          pbm_session_info[session_type].slot_id = (uint16) mmgsdi_event.data.session_changed.app_info.slot;
        }
        else
        {
          rex_enter_crit_sect(&pbm_crit_sect);

          /* clearing the card ECC and OTA numbers */
          pbm_ecc_clear_all_id(session_type, PBM_FIELD_SIM_ECC);
          pbm_ecc_clear_all_id(session_type, PBM_FIELD_NETWORK_ECC);

          rex_leave_crit_sect(&pbm_crit_sect);

          ecc_cache = pbm_file_id_to_cache(session_type, PBM_FILE_ECC);
          UIM_MSG_HIGH_1("PBM received MMGSDI_SESSION_CHANGED_EVT event on slot %d",
                          (uint16) mmgsdi_event.data.session_changed.app_info.slot);

          /* PBM session deactivate handling */
          pbm_session_deactivate_handling(session_type,pbm_session_info[session_type].slot_id);

          /* pin1 cleared in the above de-activate handling and hence cache building is to be after that */
         (void)pbm_populate_ecc_other_if_pin1_status_change(pin1_prev_status);

          /* Skip ECC ready indication if an card error received before session de-activation */
          if(pbm_card_status[PBM_SLOT_1 - 1] == TRUE || 
             pbm_card_status[PBM_SLOT_2 - 1] == TRUE || pbm_card_status[PBM_SLOT_3 - 1] == TRUE)
          {
            /* sending cache ready to clients - not checking for pin1_status_change has updated hardcoded list because this ready has to go 
                      irrespective of its update.Because network and card ecc are cleared out*/
            PBM_SET_PB_CACHE_READY(ecc_cache);
          }
        }
        break;
      }

      case MMGSDI_SUBSCRIPTION_READY_EVT:
         {
            PB_CACHE                cache;
            pbm_session_enum_type   pbm_card_session;
            pbm_slot_type           slot_id;
            mmgsdi_session_id_type  mmgsdi_session_id = mmgsdi_event.session_id;
            mmgsdi_app_info_type    app_info          = mmgsdi_event.data.subscription_ready.app_info;

      memset(&cache,0,sizeof(PB_CACHE));
      if( pbm_session_type_to_pb_category(session_type) != PBM_LPB )
      {
        break;
      }

            pbm_session_info[session_type].in_use_adn_pb = PBM_PB_NOT_KNOWN;
            /* Resetting the session_init_done for the provisioning and card session to
               avoid sending PBM_EVENT_SESSION_INIT_DONE twice */
            pbm_init_info.pbm_session_init_done[session_type] = FALSE;
            PBM_SESSION_SET(pbm_session_subs_ready,session_type);
            slot_id     = pbm_session_info[session_type].slot_id -1;
            UIM_MSG_HIGH_5("PBM received SUBSCRIPTION READY event on session 0x%x slot_id 0x%x ADN gpb_status 0x%x lpb_1x_status 0x%x lpb_gw_status 0x%x",
	  	            session_type,
                            slot_id,
                            pbm_features_status_list.pbm_adn_phonebook_caching_status[slot_id].gpb_caching_status,
                            pbm_features_status_list.pbm_adn_phonebook_caching_status[slot_id].lpb_1x_caching_status,
                            pbm_features_status_list.pbm_adn_phonebook_caching_status[slot_id].lpb_gw_caching_status);

            rex_enter_crit_sect(&pbm_crit_sect); /* for slot id, app type and in_use_adn_pb */
            if(pbm_session_type_to_pb_category(session_type) != PBM_GPB )
            {
                pbm_session_info[session_type].app_type = app_info.app_data.app_type;
            }
            pbm_session_info[session_type].slot_id = (uint16) mmgsdi_event.data.subscription_ready.app_info.slot;
            /* Go for app init completed */
            //reset the Active flag since it should be re-read
            pbm_session_info[session_type].isActive = FALSE;
            //reset the corresponding GPB's Active flag since it also should be re-read
            if(mmgsdi_event.data.subscription_ready.app_info.slot == MMGSDI_SLOT_1)
            {
                 pbm_session_info[PBM_SESSION_GPB_1].slot_id = (uint16) PBM_SLOT_1;
                 /* assigning the value directly as constant boolean warnign comes for this  */
                 /*PBM_MMGSDI_SLOT_TO_PBM_SLOT(MMGSDI_SLOT_1);*/
                 pbm_session_info[PBM_SESSION_GPB_1].isActive = FALSE;
                 pbm_init_info.pbm_session_init_done[PBM_SESSION_GPB_1] = FALSE;
            }
            else if(mmgsdi_event.data.subscription_ready.app_info.slot == MMGSDI_SLOT_2)
            {
                 pbm_session_info[PBM_SESSION_GPB_2].slot_id = (uint16) PBM_SLOT_2;
                 /* assigning the value directly as constant boolean warnign comes for this  */
                 /*PBM_MMGSDI_SLOT_TO_PBM_SLOT(MMGSDI_SLOT_2);*/
                 pbm_session_info[PBM_SESSION_GPB_2].isActive = FALSE;
                 pbm_init_info.pbm_session_init_done[PBM_SESSION_GPB_2] = FALSE;
            }
#ifdef FEATURE_TRIPLE_SIM
            else if(mmgsdi_event.data.subscription_ready.app_info.slot == MMGSDI_SLOT_3)
            {
                 pbm_session_info[PBM_SESSION_GPB_3].slot_id = (uint16) PBM_SLOT_3;
                 /* assigning the value directly as constant boolean warnign comes for this  */
                 /*PBM_MMGSDI_SLOT_TO_PBM_SLOT(MMGSDI_SLOT_3);*/
                 pbm_session_info[PBM_SESSION_GPB_3].isActive = FALSE;
                 pbm_init_info.pbm_session_init_done[PBM_SESSION_GPB_3] = FALSE;
            }
#endif /* FEATURE_TRIPLE_SIM */
            else
            {
                 UIM_MSG_ERR_1("invalid slot from MMGSDI %d", mmgsdi_event.data.subscription_ready.app_info.slot);
            }
  	    rex_leave_crit_sect(&pbm_crit_sect);

            /* This event is to update the QMI PBM cache variables in case of 
             * non-FCN refresh/Hot-swap cases followed by subscription ready event from MMGSDI
             */
            cache.pb_id.slot_id     = pbm_session_info[session_type].slot_id;
            cache.pb_id.prov_type   = pbm_session_type_to_provision_type(session_type);
            cache.pb_id.pb_category = pbm_session_type_to_pb_category(session_type);
            cache.pb_id.device_type = PBM_DEVICE_TYPE_ALL;
            /* Notifying cache status to QMI PBM to clear the global data */
            pbm_notify_pb_cache_status_update(&cache);

            slot_id = PBM_MMGSDI_SLOT_TO_PBM_SLOT(app_info.slot);
            /* get session_index for GPB for same (slot) first */
            pbm_card_session = pbm_pb_type_and_slot_to_session_type(slot_id, PBM_GPB);

            cache.pb_id.slot_id     = pbm_session_info[pbm_card_session].slot_id;
            cache.pb_id.prov_type   = pbm_session_type_to_provision_type(pbm_card_session);
            cache.pb_id.pb_category = pbm_session_type_to_pb_category(pbm_card_session);
            cache.pb_id.device_type = PBM_DEVICE_TYPE_ALL;
            /* Notifying cache status to QMI PBM to clear the global data */
            pbm_notify_pb_cache_status_update(&cache);

            pbm_handle_app_init_completed(mmgsdi_session_id, app_info.slot,app_info.app_data.app_type);
         }
         break;

        case MMGSDI_HIDDENKEY_EVT:
        {
            PB_CACHE *cache = NULL;
            /* check if the event has come after the ADN cache was ready we need to start the ADN read again */
            /*check to see if the cache is ready*/
            UIM_MSG_HIGH_3("PBM received MMGSDI_HIDDENKEY_EVT event on session %d pin status %d with dir_index %d",
                          session_type, mmgsdi_event.data.pin.pin_info.status, mmgsdi_event.data.pin.dir_index);
            rex_enter_crit_sect(&pbm_crit_sect);
            switch(mmgsdi_event.data.pin.pin_info.status)
            {
                case MMGSDI_PIN_ENABLED_VERIFIED:
                case MMGSDI_PIN_DISABLED:
                    //since disabled is also equalent to enabled-verified
                    pbm_session_info[session_type].dir_index = mmgsdi_event.data.pin.dir_index;
                    PBM_SESSION_SET(pbm_hidden_key_verified, session_type);
                    break;
                case MMGSDI_PIN_ENABLED_NOT_VERIFIED:
                    pbm_session_info[session_type].dir_index = mmgsdi_event.data.pin.dir_index;
                    PBM_SESSION_RESET(pbm_hidden_key_verified, session_type);
                    break;
                case MMGSDI_PIN_NOT_FOUND:
                default:
                    pbm_session_info[session_type].dir_index = 0;
                    PBM_SESSION_SET(pbm_hidden_key_verified, session_type);
                    break;
            }
            cache = pbm_file_id_to_cache(session_type, PBM_FILE_ADN);
            //no need to do anything when hiddenkey is MMGSDI_PIN_NOT_FOUND
            if (cache->status == PBM_STATUS_READY && pbm_session_info[session_type].dir_index != 0)
            {
                /*pbm_init_fileinfo_requests(session_type, PBM_FILE_ADN);*/ /* merge_hidden_contacts functionality this funcion will also be called from the init flow if verify event comes in between the init flow*/
                if(pbm_hidden_key_verified & PBM_SESSIONID_TO_SESSION_MASK(session_type))
                {
                  if(pbm_merge_hidden_contacts(session_type) == PBM_SUCCESS) /* this api NEEDS TO KNOW THE INDICES where the hidden cache has to be added to adn cache */
                  {
                     /* to send event to clients that the cache has been updated */
                     pbm_hidden_key_notify(session_type,TRUE);
                  }
                  else
                  {
                      UIM_MSG_ERR_0("PBM Hidden contacts merging encountered error");
                  }
                }
                else
                {
                  if(pbm_delete_hidden_contacts_from_adn_cache(session_type) == PBM_SUCCESS) /* this api NEEDS TO KNOW THE INDICES where the hidden cache has to be added to adn cache */
                  {
                    /* to send event to clients that the cache has been updated */
                    pbm_hidden_key_notify(session_type,FALSE);
                  }
                  else
                  {
                      UIM_MSG_ERR_0("PBM Hidden contacts un-merging encountered error");
                  }
                }
            }
			rex_leave_crit_sect(&pbm_crit_sect);
        }
        break;
      case MMGSDI_UNIVERSAL_PIN_EVT:
      default:
         break;
   }
}


/*===========================================================================
FUNCTION PBM_CLIENT_CMD_HANDLE

DESCRIPTION
  Read the client commands from the queue and handle it

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void pbm_client_cmd_handle(
  void
)
{
   pbm_cmd_s_type *cmd_ptr;
   pbm_write_record_s_type *write_cmd;
   pbm_write_lock_s_type *write_lock;
   pbm_return_type ret;
   rex_sigs_type sigs;
   boolean bGarbageCollect = FALSE;
   pbm_session_event_data_u_type evt_data = {0};

   while ((cmd_ptr = pbm_cmd_q_get(&pbm_client_cmd_q)) != NULL)
   {

      switch (cmd_ptr->cmd_type)
      {
         
         case PBM_CMD_WRITE:
            /* PBM Write */
            PBM_CMD_CHECK_SIZE(cmd_ptr, pbm_write_record_s_type, break);
            write_cmd = PBM_WRITE_CMD_PTR(cmd_ptr);
            ret = pbm_int_record_write(write_cmd);
            PBM_CHECK_ERR(ret, "PBM internal record write failed");
            bGarbageCollect = TRUE;
            break;

         case PBM_CMD_WRITE_LOCK:
            /* PBM Write lock */
            PBM_CMD_CHECK_SIZE(cmd_ptr, pbm_write_lock_s_type, break);
            write_lock = PBM_WRITE_LOCK_CMD_PTR(cmd_ptr);

            /* Call callback sent with cmd */
            if (write_lock->proc_func)
            {
               pbm_writecb_data_s_type cb_data;

               cb_data.ret = PBM_SUCCESS;               
               //since this is the old rec_id we would need just 16 LSBs
               cb_data.rec_id = (uint16)PBM_GET_FIRST_AVAILABLE;
               //copy the new 32 bit rec_id
               cb_data.sess_rec_id = PBM_GET_FIRST_AVAILABLE;
               cb_data.user_data = write_lock->user_data;

               (*write_lock->proc_func)(&cb_data);
            }


            /* Send notification to registered clients */
            evt_data.lock_type = write_lock->lock_type;
            pbm_add_notify(PBM_EVENT_LOCKED, evt_data);


            pbm_cmd_free(cmd_ptr);
            (void) rex_set_sigs(PBM_TCB, PBM_CMD_COMPLETE_SIG);
            break;

         case PBM_CMD_WRITE_UNLOCK:
            /* PBM Write unlock */
            PBM_CMD_CHECK_SIZE(cmd_ptr, pbm_write_lock_s_type, break);
            write_lock = PBM_WRITE_LOCK_CMD_PTR(cmd_ptr);

            /* Nothing to do except send notification to registered clients */            
            evt_data.lock_type = write_lock->lock_type;
            pbm_add_notify(PBM_EVENT_UNLOCKED, evt_data);

            pbm_cmd_free(cmd_ptr);
            (void) rex_set_sigs(PBM_TCB, PBM_CMD_COMPLETE_SIG);
            break;

         default:
            UIM_MSG_ERR_1("Unknown command %d", cmd_ptr->cmd_type);
            continue;
      }

      /* Handle client commands one at a time.
       * While waiting for command complete signal we have to handle GSDI
       * responses. */
      sigs = 0x0;
      while (!(sigs & PBM_CMD_COMPLETE_SIG))
      {
         sigs = pbm_wait(PBM_CMD_COMPLETE_SIG | PBM_GSDI_RESPONSE_SIG);
         if (sigs & PBM_GSDI_RESPONSE_SIG)
         {
            (void) rex_clr_sigs(PBM_TCB, PBM_GSDI_RESPONSE_SIG);
            pbm_mmgsdi_resp_handle();
         }
      }
      (void) rex_clr_sigs(PBM_TCB, PBM_CMD_COMPLETE_SIG);
   }

   if (bGarbageCollect && !pbm_efs_disable)
   {
      pbm_efs_garbage_recollect();
   }

}/* pbm_client_cmd_handle */


/*===========================================================================
FUNCTION PBM_MMGSDI_RESP_HANDLE

DESCRIPTION
  Read the MMGSDI responses from the queue and handle it.
  Typically we do not have to keep the mmgsdi response data around, so it
  is freed immediately after processing.

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void pbm_mmgsdi_resp_handle(
  void
)
{
  pbm_cmd_s_type                       *cmd_ptr       = NULL;
  pbm_mmgsdi_resp_s_type               *mmgsdi_resp   = NULL;
  pbm_mmgsdi_card_status_s_type        *card_status   = NULL;
   void *data_ptr = NULL;

   while ((cmd_ptr = pbm_cmd_q_get(&pbm_gsdi_resp_q)) != NULL)
   {
      switch (cmd_ptr->cmd_type)
      {
         case PBM_MMGSDI_WRITE_CB:
            /* GSDI write callback */
            PBM_CMD_CHECK_SIZE(cmd_ptr, pbm_mmgsdi_resp_s_type, break);
            mmgsdi_resp = PBM_MMGSDI_RESP_PTR(cmd_ptr);
            pbm_mmgsdi_uim_write_cb(mmgsdi_resp->status,
                                    mmgsdi_resp->data_type,
                                    &mmgsdi_resp->data);
            break;

         case PBM_CM_SS_CB:
          {
            pbm_cm_ss_evt_type *cm_ss_evt_ptr;
            PBM_CMD_CHECK_SIZE(cmd_ptr, pbm_cm_ss_evt_type, break);
            cm_ss_evt_ptr = (pbm_cm_ss_evt_type*)cmd_ptr;
            pbm_handle_cm_ss_evt(cm_ss_evt_ptr);
            PBM_MEM_FREEIF(cm_ss_evt_ptr->ii_info_ptr);
          }
          break;

#ifdef FEATURE_MODEM_CONFIG_REFRESH
         case PBM_MCFG_REFRESH_CB:
          {
            pbm_handle_nv_refresh_req_type *pbm_nv_refresh_req_ptr = NULL;

            PBM_CMD_CHECK_SIZE(cmd_ptr, pbm_handle_nv_refresh_req_type, break);
            pbm_nv_refresh_req_ptr = (pbm_handle_nv_refresh_req_type *)cmd_ptr;
            pbm_process_nv_refresh_req(pbm_nv_refresh_req_ptr);
          }
          break;
#endif /*FEATURE_MODEM_CONFIG_REFRESH*/

       case PBM_WMS_STATUS_CB:
          {
            pbm_wms_resp_s_type *wms_resp_ptr;
            PBM_CMD_CHECK_SIZE(cmd_ptr, pbm_wms_resp_s_type, break);
            wms_resp_ptr = (pbm_wms_resp_s_type*)cmd_ptr;
            pbm_wms_cfg_cb(wms_resp_ptr);
          }
          break;
      
         case PBM_MMGSDI_FDN_EXT_READ_CB:
           {
             PBM_CMD_CHECK_SIZE(cmd_ptr, pbm_mmgsdi_resp_s_type, break);
             mmgsdi_resp = PBM_MMGSDI_RESP_PTR(cmd_ptr);
             pbm_fdn_ext_read_cb(mmgsdi_resp->status,
                                 mmgsdi_resp->data_type,
                                 &mmgsdi_resp->data);             

             data_ptr = (void *)mmgsdi_resp->data.read_cnf.read_data.data_ptr;
             if (mmgsdi_resp->data.read_cnf.read_data.data_len > 0)
             {
                //free the memory allocated for deep copy
                PBM_MEM_FREEIF(data_ptr);
             }
             
           }
           
           break;
		
         case PBM_MMGSDI_READ_CB:
            /* GSDI read callback */
            PBM_CMD_CHECK_SIZE(cmd_ptr, pbm_mmgsdi_resp_s_type, break);
            mmgsdi_resp = PBM_MMGSDI_RESP_PTR(cmd_ptr);
            pbm_mmgsdi_read_cb(mmgsdi_resp->status,
                               mmgsdi_resp->data_type,
                               &mmgsdi_resp->data);

            data_ptr = (void *)mmgsdi_resp->data.read_cnf.read_data.data_ptr;
            if (mmgsdi_resp->data.read_cnf.read_data.data_len > 0)
            {
               /* free the memory allocated for deep copy */
               PBM_MEM_FREEIF(data_ptr);
            }
            break;
        case PBM_DEL_LINKS_READ_CB:
           /* GSDI read callback */
           PBM_CMD_CHECK_SIZE(cmd_ptr, pbm_mmgsdi_resp_s_type, break);
           mmgsdi_resp = PBM_MMGSDI_RESP_PTR(cmd_ptr);
           pbm_del_links_read_cb(mmgsdi_resp->status,
                              mmgsdi_resp->data_type,
                              &mmgsdi_resp->data);

           data_ptr = (void *)mmgsdi_resp->data.read_cnf.read_data.data_ptr;
           if (mmgsdi_resp->data.read_cnf.read_data.data_len > 0)
           {
              //free the memory allocated for deep copy
              PBM_MEM_FREEIF(data_ptr);
           }
           break;

         case PBM_DEL_LINKS_WRITE_CB:
            /* GSDI write callback */
            PBM_CMD_CHECK_SIZE(cmd_ptr, pbm_mmgsdi_resp_s_type, break);
            mmgsdi_resp = PBM_MMGSDI_RESP_PTR(cmd_ptr);
            pbm_del_links_write_cb(mmgsdi_resp->status,
                                    mmgsdi_resp->data_type,
                                    &mmgsdi_resp->data);
            break;

         case PBM_MMGSDI_SERVICE_INFO_CB:
            // GSDI file info callback
            PBM_CMD_CHECK_SIZE(cmd_ptr, pbm_mmgsdi_resp_s_type, break);
            mmgsdi_resp = PBM_MMGSDI_RESP_PTR(cmd_ptr);
            pbm_service_info_cb(mmgsdi_resp->status,
                                mmgsdi_resp->data_type,
                                &mmgsdi_resp->data);
            break;

        case PBM_MMGSDI_LOCAL_PB_SRV_CB:
            //MMGSDI file info callback
            PBM_CMD_CHECK_SIZE(cmd_ptr, pbm_mmgsdi_resp_s_type, break);
            mmgsdi_resp = PBM_MMGSDI_RESP_PTR(cmd_ptr);
            pbm_local_phonebook_check_cb(mmgsdi_resp->status,
                                            mmgsdi_resp->data_type,
                                            &mmgsdi_resp->data);
            break;
            

         case PBM_MMGSDI_FILE_INFO_CB:
            // GSDI file info callback
            PBM_CMD_CHECK_SIZE(cmd_ptr, pbm_mmgsdi_resp_s_type, break);
            mmgsdi_resp = PBM_MMGSDI_RESP_PTR(cmd_ptr);
            pbm_fileinfo_cb(mmgsdi_resp->status,
                            mmgsdi_resp->data_type,
                            &mmgsdi_resp->data);
            switch ( mmgsdi_resp->data.get_file_attr_cnf.file_attrib.file_type )
            {
                 case MMGSDI_LINEAR_FIXED_FILE :
			data_ptr = (void *)mmgsdi_resp->data.get_file_attr_cnf.file_attrib.file_info.linear_fixed_file.file_security.write.protection_pin_ptr;
            if (mmgsdi_resp->data.get_file_attr_cnf.file_attrib.file_info.linear_fixed_file.file_security.write.num_protection_pin > 0)
            {
               //free the memory allocated for deep copy
               PBM_MEM_FREEIF(data_ptr);
            }
            break;
            
                case MMGSDI_CYCLIC_FILE :
                   data_ptr = (void *)mmgsdi_resp->data.get_file_attr_cnf.file_attrib.file_info.cyclic_file.file_security.write.protection_pin_ptr;
                   if (mmgsdi_resp->data.get_file_attr_cnf.file_attrib.file_info.cyclic_file.file_security.write.num_protection_pin > 0)
                   {
                      PBM_MEM_FREEIF(data_ptr);
                   }
                break;
                  
                 case MMGSDI_TRANSPARENT_FILE :
                   data_ptr = (void *)mmgsdi_resp->data.get_file_attr_cnf.file_attrib.file_info.transparent_file.file_security.write.protection_pin_ptr;
                   if (mmgsdi_resp->data.get_file_attr_cnf.file_attrib.file_info.transparent_file.file_security.write.num_protection_pin > 0)
                   {
                      PBM_MEM_FREEIF(data_ptr);
                   }
                 break;
                  
                 default : 
                    break;
            }
            break;
            

         case PBM_MMGSDI_REG_STATUS_CB:
            // GSDI file info callback
            PBM_CMD_CHECK_SIZE(cmd_ptr, pbm_mmgsdi_resp_s_type, break);
            mmgsdi_resp = PBM_MMGSDI_RESP_PTR(cmd_ptr);
            if (mmgsdi_resp->status == MMGSDI_SUCCESS)
            {
               if (mmgsdi_resp->data_type== MMGSDI_CLIENT_ID_AND_EVT_REG_CNF)
               {            
                  pbm_mmgsdi_client_id =
                  mmgsdi_resp->data.client_id_and_evt_reg_cnf.response_header.client_id;
                  //open sessions irrespective of knowing the SIM inserted or not
                  pbm_open_sessions();
               }
            } else
            {
               switch (mmgsdi_resp->data_type)
               {
                  case MMGSDI_CLIENT_ID_AND_EVT_REG_CNF:
                     UIM_MSG_ERR_2("MMGSDI failed registering PBM client ID & EVT registration.  Status %d, Confirmation %d",
                                 mmgsdi_resp->status, mmgsdi_resp->data_type);
                     break;
                  default:
                     UIM_MSG_ERR_2("MMGSDI returned Failure async. Status %d, Confirmation %d",
                                 mmgsdi_resp->status, mmgsdi_resp->data_type);
                     break;
               }
            }
            break;

         case PBM_MMGSDI_CARD_STATUS_CB:
            /* MMGSDI Card status callback */
            PBM_CMD_CHECK_SIZE(cmd_ptr, pbm_mmgsdi_card_status_s_type, break);
            card_status = PBM_MMGSDI_CARDSTATUS_PTR(cmd_ptr);
            pbm_card_status_cb(card_status->mmgsdi_event);
            break;

          case PBM_MMGSDI_READ_PBR_ATTR_CB:
            // GSDI PBR read callback
            PBM_CMD_CHECK_SIZE(cmd_ptr, pbm_mmgsdi_resp_s_type, break);
            mmgsdi_resp = PBM_MMGSDI_RESP_PTR(cmd_ptr);
            pbm_usim_read_pbr_attr_cb(mmgsdi_resp->status,
                            mmgsdi_resp->data_type,
                            &mmgsdi_resp->data);
            break;

          case PBM_MMGSDI_READ_PBR_REC_CB:
            // GSDI read callback
            PBM_CMD_CHECK_SIZE(cmd_ptr, pbm_mmgsdi_resp_s_type, break);
            mmgsdi_resp = PBM_MMGSDI_RESP_PTR(cmd_ptr);
            pbm_usim_read_pbr_rec_cb(mmgsdi_resp->status,
                               mmgsdi_resp->data_type,
                               &mmgsdi_resp->data);

            data_ptr = (void *)mmgsdi_resp->data.read_cnf.read_data.data_ptr;
            if (mmgsdi_resp->data.read_cnf.read_data.data_len > 0)
            {
               //free the memory allocated for deep copy
               PBM_MEM_FREEIF(data_ptr);
            }
            break;      			
            

         case PBM_MMGSDI_SEEK_CB:
            // GSDI seek callback
            PBM_CMD_CHECK_SIZE(cmd_ptr, pbm_mmgsdi_resp_s_type, break);
            mmgsdi_resp = PBM_MMGSDI_RESP_PTR(cmd_ptr);
            pbm_seek_cb(mmgsdi_resp->status,
                        mmgsdi_resp->data_type,
                        &mmgsdi_resp->data);
            //no need to check whether data_len > 0 ,since we are assigning it to NULL in pbm_mmgsdi_seek_cb
            //if it  is data_len <= 0
            data_ptr = (void *)mmgsdi_resp->data.search_cnf.searched_record_nums.data_ptr;
            //free the memory allocated for deep copy
            PBM_MEM_FREEIF(data_ptr);
            break;
       case PBM_MMGSDI_OPEN_SESSION_CB:
	    PBM_CMD_CHECK_SIZE(cmd_ptr, pbm_mmgsdi_resp_s_type, break);
	    mmgsdi_resp = PBM_MMGSDI_RESP_PTR(cmd_ptr);
            pbm_open_session_cb(mmgsdi_resp->status,
                                mmgsdi_resp->data_type,
                                &mmgsdi_resp->data);
	    break;
         default:
            UIM_MSG_ERR_1("Unknown command %d", cmd_ptr->cmd_type);
            break;
      }

      pbm_cmd_free(cmd_ptr);
   }
} /* pbm_mmgsdi_resp_handle */


/*===========================================================================
FUNCTION PBM_ALLOC_RECORD

DESCRIPTION
  During async calls, we need to store the memory location in which the
  final answer will be stored.  This call will allocate a buffer and
  return the index.  This index is passed to and from GSDI to know where
  to place the buffers.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
int pbm_alloc_record(
  void
)
{
  int   i;
  int   retval = -1;

  rex_enter_crit_sect(&pbm_crit_sect);
  for (i = 0; i < PBM_ASYNC_BUFFERS; i++)
  {
    if (!pbm_buffers[i].in_use)
    {
      memset(&pbm_buffers[i], 0, sizeof(pbm_buffers[i]));
      pbm_buffers[i].in_use = TRUE;
      retval = i;
      break;
    }
  }
  rex_leave_crit_sect(&pbm_crit_sect);

  return retval;
} /* pbm_alloc_record */


/*===========================================================================
FUNCTION PBM_FREE_RECORD

DESCRIPTION
  Mark the pbm_buffer as available for later use.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_free_record(
  int      record
)
{
  pbm_buffers[record].in_use = FALSE;
} /* pbm_free_record */

