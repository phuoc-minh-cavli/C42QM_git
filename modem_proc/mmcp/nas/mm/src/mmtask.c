/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mm/vcs/mmtask.c_v   1.16   13 Jun 2002 15:25:44   jault  $
$Header: //components/rel/mmcp.mpss/6.1.10/nas/mm/src/mmtask.c#1 $ $DateTime: 2023/04/25 14:39:18 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
11/28/12   am      Fixing compilation warnings 
10/22/12   cogolt  CR407246. Disable LSTI feature by default and allow enabling
                   LSTI feature through EFS only
10/01/12   am      Back off timer code
07/05/12   us      Removing static buffers allocated by EMM for msgr & adding dynamic mechanism of 
                    allocating buffers when required.  
05/29/12   pahire  Added changes for UE Specific DRX Cycle for QChat/LTE
10/28/11   rajesh  Added support for reading "nas_lai_change_force_lau_for_emergency" EFS item.
10/10/11   HC      Removed AEEstd.h inclusion as aeestdlib is obsoleted 
                   Replaced the std_strlen with C equivalent function strlen
                   Replaced the STD_OFFSETOF with local definition NAS_STD_OFFSETOF

09/30/11   rajesh  Added support for "exclude_ptmsi_type_field" EFS item.
08/01/11   rajesh Initialized "t3442_value" of emm_ctrl_data_ptr.
06/16/11   PA      Added changes for Rel 9 Emergency Bearer services 
05/27/11   PA      Added changes for Security Key generation during W2L PS HO
02/01/10   PA      Added changes for Security Key generation during L2W PS HO
10/07/10   MNK     1XCSFB introduction 
08/09/10   MNK     Make code compatible for 1xCSFB & 3GPPCSFB
08/30/10   MNK     CSFB changes
07/11/10   zren    Updated to correctly free ESM and RRC cmd
05/21/10   zren    Updated support of handling REG CMD coming during L2GW IRAT
05/17/10   zren    Updated EMM to only process EMM timers
05/07/10   zren    Added temp FPLMN feature
04/22/10   zren    Updated to use global structure for NAS outgoing message
04/09/10   zren    Added combined procedure feature
02/11/10   MNK     Moved LTE NAS EFS read to happen after SIM read is successful
03/03/10   MNK     TIN support 
02/21/10   MNK     Add support for GW to LTE i-RAT
02/16/10   RI      Updates to EFS read ( temporary)
02/16/10   MNK     Initialize tin and ignore_GW_activation_ind
02/12/10   zren    Updated to always abort the existing connection before 
                   sending RRC DEACT REQ
02/01/10   zren    Added TAU trigger of RRC connection failure and no user data
                   pending. 
                   Fixed service requests collision.
01/22/10   RI      Added detach reason initialization in emm ctrl data.
01/12/10   RI      Updated the EFS read for UE sec capabilities
                   for target.
12/10/09   zren    Updated to make routing information inclusion in 
                   rrc conn req spec compliant
11/30/09   zren    Modified access barring timeout handling
11/04/09   MNK     Initialize deactivate reason
03/13/02   ATM     Added support for new GS_SEND_MESSAGE
02/20/02   ATM     Merge GS_GSM and GS functions together
04/12/01   jca     Began integration with GMM and new MM_COORD.
05/03/01   jca     Added watchdog support.
05/29/01   sbk     Added RRC_ACT_CNF processing to the mm_rrc_get_cmd_buf
06/13/01   jca     Deleted RRC_UNIT_DATA_IND primitive.
09/18/01   sbk     Added dog support by calling the mm_wait function
                   Fixed task startup problems by fixing init code
                   Initialize the GHDI interface for NV by ghdi_nvmem_register_task fn
                   RRC_DEACT_CNF is added to the list of MM commands RRC can request for
06/05/02   ks      Added support for Dual mode operation.
09/10/02   vdr     Added func for allocation of the LLC primitive mm_gllc_get_cmd_buf()
11/12/02   jca     Added RRC_CLOSE_SESSION_CNF.
12/08/02   mks     Added allocation for RRC_ACTIVATION_IND primitive in mm_rrc_get_cmd_buf()
02/12/03   vdr     Added mm_cm_get_cmd_buf() CM->MM interface function to support PS Data
                   Continuity feature
05/13/03   npr     Added command buf support for GLLCMM_LLGMM_NEW_TLLI_IND
06/04/03   ks      Added support for RRC_MODE_CHANGE_CNF for MODE_CHANGE
                   support for UMTS.
06/16/03   jca     Now consistently ERR_FATAL if attempt to allocate memory fails.
11/16/04   sv      Addition of new primitive RRC_SERVICE_IND_FROM_OTA_MSG
07/20/05   abe     Added support for new primitive RRC_WTOG_CCO_COMPLETE_IND
04/06/06   sn      Added support for RRC_HSDPA_HSUPA_CELL_IND.
08/14/06   pk      Updated to log CFA packet for RRC->MM messages.
11/21/11   gps     Removed refrence for header file "gsdi_exp.h"
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include <stringl/stringl.h>
#include "mmcp_variation.h"
#include <stdio.h>
#include <stdlib.h> /* for srand() */
#include <string.h>
#include "customer.h"
#include "rex.h"
#include "dog_hb_rex.h"
#include "queue.h"
#include "err.h"
#include "sys_type.h"
#include "sys_cnst_v.h"
#include "sys_stru_v.h"
#include "ms.h"
#include "timers_v.h"
#include "gs_v.h"
#include "ms_timer_v.h"
#include "mm_rr.h"
#include "mm_mm_v.h"
#include "mm_v.h"
#include "mm_coord.h"
#ifndef FEATURE_MODEM_RCINIT_PHASE2
#include "tmc.h"
#endif /* FEATURE_MODEM_RCINIT_PHASE2 */
#include "mmtask.h"
#include "mmtask_int.h"
#include "mm_umts_v.h"
#include "gmm_int_v.h"
#include "gmm_v.h"
#include "cfa_v.h"
#include "fs_public.h"
#include "mcfg_fs.h"
#include "fs_lib.h"
#include "policyman.h"
#ifdef FEATURE_MODEM_HEAP
#include "modem_mem.h"
#endif
#include "ULogFront.h"
#ifdef FEATURE_LTE
#include "amssassert.h"
#include "emm_rrc_emm_if.h"
#include "emm_timer_sigs.h"
#include "msgr_lte.h"
#include "msgr_nb1.h"
#include "emm_esm_if_msg.h"
#include "emm_reg_handler.h"
#include "esm_v.h"
#include "emm_rrc_handler.h"
#include "emm_update_lib.h"
#include "emm_database.h"
#include "emm_mem.h"
#include "emm_connection_handler.h"
#include "emm_esm_handler.h"
#include "lte_nas_msg_parse.h"
#include "emm_service_request_lib.h"
#include "emm_utility.h"
#include "pthread.h"
#include "emm.h"
#include "rex.h"
#include "mmauth.h"
#include "err.h"
#include "lte_nas_tlb.h"
#include "queue.h"
#include "msgr_nas.h"
#include "msgr_rex.h"
#include "mm_multimode_common.h"
#include "emm_irat_if_msg.h"
#include "emm_csfb_handler.h"
#include "lte_rrc_irat_msg.h"
#if defined (FEATURE_LTE_REL9)
#include "emm_gps_handler.h"
#include "emm_gps_if_msg.h"
#endif /*FEATURE_LTE_REL9*/
#ifdef FEATURE_MODEM_RCINIT
#include "rcinit.h"
#endif
#include "gmm_int_v.h"
#ifdef FEATURE_MODEM_CONFIG_REFRESH
#include "mcfg_refresh.h"
#endif
#include "event.h"
#include "naslog_v.h"
#include "event_defs.h"

#ifdef FEATURE_CIOT
/* needed for NB1_CPHY_EDRX_UIM_PAGE_ID*/
#include "nb1_cphy_msg.h"
#include "nb1_cphy_msg_ids.h"
#endif

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

nas_crit_sect_type mm_rat_prioritylist_crit_sec;

#ifdef FEATURE_CIOT
nas_crit_sect_type nas_ciot_config_crit_sec;
#endif

/* ----------------------------
** Define watchdog report handle
** ---------------------------- */
  #undef  DOG_MM_RPT
  #define DOG_MM_RPT       mm_dog_rpt_var
  dog_report_type mm_dog_rpt_var  = 0;      /* Initial Safety */

/* -----------------------------
** Command queue for the MM task
** ----------------------------- */
q_type                    mm_cmd_q;
extern q_type             sm_cmd_q;
extern q_type             mn_cnm_cmd_q;
extern q_type             esm_cmd_q_msg; 



rex_tcb_type* mm_tcb_ptr = NULL;


#ifdef FEATURE_LTE
pthread_mutex_t  emm_mutex = PTHREAD_MUTEX_INITIALIZER;  
pthread_mutex_t  emm_manual_ftai_list_mutex = PTHREAD_MUTEX_INITIALIZER;  

encode_nas_msg_func_ptr* encode_nas_msg_fp = NULL;
decode_nas_msg_func_ptr* decode_nas_msg_fp = NULL;
nas_retrieve_msg_func_ptr* nas_retrieve_msg_fp = NULL;
  msgr_client_t    esm_client;
  msgr_client_t    rrc_client;
  msgr_client_t    tlb_client;
#ifdef FEATURE_LTE_TO_1X
  msgr_client_t    emm_csfb_client;
#endif

#if defined (FEATURE_LTE_REL9)
 msgr_client_t    emm_gps_client;
#endif
#ifdef FEATURE_CIOT
 msgr_client_t    emm_phy_client;
#endif
q_type esm_esm_cmd_q_msg;
q_type rrc_emm_cmd_q_msg;
q_type tlb_tlb_cmd_q_msg;
#ifdef FEATURE_LTE_TO_1X
q_type emm_csfb_cmd_q_msg;
#endif
#if defined (FEATURE_LTE_REL9)
q_type emm_gps_cmd_q_msg;
#endif /*FEATURE_LTE_REL9 */
#ifdef FEATURE_CIOT
q_type emm_phy_cmd_q_msg;
#endif /*FEATURE_LTE_REL9 */
#if defined (FEATURE_SGLTE)|| defined (FEATURE_DUAL_SIM)
static mm_emm_dispatcher_fp *mm_emm_dispatcher_func_ptr_sim[MAX_NAS_STACKS];
#define mm_emm_dispatcher_func_ptr mm_emm_dispatcher_func_ptr_sim[mm_as_id]
#else
static mm_emm_dispatcher_fp *mm_emm_dispatcher_func_ptr;
#endif
typedef PACKED struct PACKED_POST{ 
  boolean    eps_encrypt_alg_128_eea0_supported; 
  boolean    eps_encrypt_alg_128_eea1_supported; 
  boolean    eps_encrypt_alg_128_eea2_supported;
  boolean    eps_encrypt_alg_eea3_supported;
  boolean    eps_encrypt_alg_eea4_supported;
  boolean    eps_encrypt_alg_eea5_supported;
  boolean    eps_encrypt_alg_eea6_supported;
  boolean    eps_encrypt_alg_eea7_supported;
  boolean    eps_integrity_alg_128_eia1_supported; 
  boolean    eps_integrity_alg_128_eia2_supported; 
  boolean    eps_integrity_alg_128_eia3_supported; 
  boolean    eps_integrity_alg_128_eia4_supported; 
  boolean    eps_integrity_alg_128_eia5_supported; 
  boolean    eps_integrity_alg_128_eia6_supported; 
  boolean    eps_integrity_alg_128_eia7_supported; 
  boolean    umts_encrypt_alg_uea0_supported; 
  boolean    umts_encrypt_alg_uea1_supported;
  boolean    umts_encrypt_alg_uea2_supported;
  boolean    umts_encrypt_alg_uea3_supported;
  boolean    umts_encrypt_alg_uea4_supported;
  boolean    umts_encrypt_alg_uea5_supported;
  boolean    umts_encrypt_alg_uea6_supported;
  boolean    umts_encrypt_alg_uea7_supported;
  boolean    ucs2_supported; 
  boolean    umts_integrity_alg_uia1_supported; 
  boolean    umts_integrity_alg_uia2_supported;
  boolean    umts_integrity_alg_uia3_supported;
  boolean    umts_integrity_alg_uia4_supported;
  boolean    umts_integrity_alg_uia5_supported;
  boolean    umts_integrity_alg_uia6_supported;
  boolean    umts_integrity_alg_uia7_supported;
  boolean    gprs_encrypt_alg_1_supported; 
  boolean    gprs_encrypt_alg_2_supported; 
  boolean    gprs_encrypt_alg_3_supported; 
  boolean    gprs_encrypt_alg_4_supported; 
  boolean    gprs_encrypt_alg_5_supported; 
  boolean    gprs_encrypt_alg_6_supported; 
  boolean    gprs_encrypt_alg_7_supported; 

} ue_security_capability;
#endif

#ifdef FATURE_DUAL_SIM
#define mm_ghdi_nvmem_read(a,b) ghdi_nvmem_read_per_subs(a,b,mm_nv_context_id)
#else
#define mm_ghdi_nvmem_read(a,b) ghdi_nvmem_read(a,b)
#endif

#if defined (FEATURE_SGLTE)|| defined (FEATURE_DUAL_SIM)
extern boolean mm_get_as_id(mm_cmd_type *message);
#endif

#if defined FEATURE_LTE
extern void esm_process_pending_messages(void);
extern void esm_process (mm_cmd_type* emm_cmd_ptr);
#endif


extern void mm_set_mm_controls_resel_dec_mt_cs_call(mm_cmd_type *message);

static mn_cnm_cmd_type *mn_cnm_cmd_q_get(void);
void mn_cnm_process_external_messages(void);
static void mn_cnm_cmd_q_free(mn_cnm_cmd_type *cmd_ptr);
rex_crit_sect_type        cm_conn_ids_crit_sect;  /* to be used for critical sections */
extern void ghdi_mvs_init(void);


#ifdef FEATURE_LTE
extern void emm_get_esm_as_id
(
  void *emm_message_router_cmd
);
extern void emm_get_rrc_as_id
(
  void *rrc_message_router_cmd
);
extern void emm_get_tlb_as_id
(
  void *cmd_ptr
);
extern void emm_get_irat_csfb_as_id
(
  void *cmd_ptr
);
extern void emm_get_gps_as_id
(
  void *cmd_ptr
);
extern void emm_populate_mm_msg_info
(
  void *rrc_message_router_cmd
);

boolean emm_process_unexpected_rrc_primitive
(
  void                 *rrc_message_router_cmd,
  emm_ctrl_data_type   *emm_ctrl_data_ptr,
  mm_emm_dispatcher_fp **mm_emm_dispatcher_fn_ptr
);
#endif

#if defined(FEATURE_CIOT) && !defined(FEATURE_NAS_GW)
extern boolean emm_process_oos_rlf_rrc_primitive_lte
(
  void                 *rrc_message_router_cmd,
  emm_ctrl_data_type   *emm_ctrl_data_ptr,
  mm_emm_dispatcher_fp **mm_emm_dispatcher_fn_ptr
);

extern boolean mmcoord_route_oos_rlf_mm_emm_messages_lte 
(
  void        *command_ptr,
  emm_ctrl_data_type *emm_ctrl_data_ptr_arg,
  mm_emm_dispatcher_fp **mm_emm_dispatcher_fn_ptr
);

void mm_emm_oos_rlf_msg_handler_lte(rex_sigs_type sigs,void **mm_process_msg_fn_ptr)
{
  mm_process_messages_fp **local_fp_ptr = ((mm_process_messages_fp **)mm_process_msg_fn_ptr);

  if (sigs & MM_CMD_Q_SIG)
  {
    *local_fp_ptr = (void *)mmcoord_route_oos_rlf_mm_emm_messages_lte;
  }
  else if(sigs & ESM_EMM_CMD_Q_SIG)
  {
    *local_fp_ptr = (void *)emm_process_oos_rlf_outgoing_esm_message; 
  }
  else if(sigs & RRC_EMM_CMD_Q_SIG)
  {
    *local_fp_ptr =(void *) emm_process_oos_rlf_rrc_primitive_lte;
  }
  else if(sigs & TLB_EMM_CMD_Q_SIG )
  {
    *local_fp_ptr = emm_process_outgoing_tlb_message;
  }

#if defined (FEATURE_LTE_REL9)
#ifdef FEATURE_NAS_GPS_ENABLE
  else if(sigs & GPS_EMM_CMD_Q_SIG )
  {
    *local_fp_ptr = emm_process_ul_generic_nas_transport_msg; // leave as is
  } 
#endif
#endif
#ifdef FEATURE_CIOT
  else if(sigs & PHY_EMM_CMD_Q_SIG )
  {
    *local_fp_ptr = emm_process_phy_cmd_msg; 
  } 
#endif
  else
  {
    *local_fp_ptr = (void *)NULL;
    MSG_ERROR_DS(MM_SUB,"=EMM= received invalid message in EMM queue during OOS/RLF",0,0,0);
  }
}
#endif

#ifdef FEATURE_NAS_GW
extern boolean emm_disable_geran_cap;
#endif

#ifdef FEATURE_LTE
#ifdef FEATURE_DUAL_SIM
extern boolean gmm_set_drx_req_pending_sim[MAX_AS_IDS];
#define gmm_set_drx_req_pending gmm_set_drx_req_pending_sim[mm_sub_id]
#else
#ifdef DISABLE_9x05_OPTIMIZATIONS
extern boolean gmm_set_drx_req_pending;
#endif
#endif
#endif
void emm_init_msglib_encode_func_ptr(encode_nas_msg_func_ptr encode_func_ptr)
{
  encode_nas_msg_fp = encode_func_ptr;
}

void emm_init_msglib_decode_func_ptr(decode_nas_msg_func_ptr decode_func_ptr)
{
  decode_nas_msg_fp = decode_func_ptr;
}

void emm_init_msglib_retrieve_func_ptr(nas_retrieve_msg_func_ptr retrieve_func_ptr)
{
  nas_retrieve_msg_fp = retrieve_func_ptr;
}
#endif /*FEATURE_LTE*/

static ULogHandle mm_ulog_handle = NULL;

/*===========================================================================
FUNCTION mm_get_ulog_handle

DESCRIPTION
  Returns ULOG Handle for MM task
  
DEPENDENCIES
  None

RETURN VALUE
  mm_ulog_handle

SIDE EFFECTS
  None
===========================================================================*/
ULogHandle mm_get_ulog_handle()
{
  return mm_ulog_handle;
}


#ifdef FEATURE_NAS_DIAG_INIT
extern void nas_diag_init (void);
#endif

#ifdef FEATURE_NAS_GW


#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE 
void mm_timer_expiry_sub1(unsigned long param)
{
   mm_cmd_type   *cmd_ptr;

   /* Get command buffer from reg task */
   cmd_ptr = mm_get_cmd_buf();

   if (cmd_ptr == NULL)
   {
      MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
   }
    /* Construct message */
   cmd_ptr->cmd.timer_expiry.IMH.message_set = MS_TIMER;
   cmd_ptr->cmd.timer_expiry.IMH.message_id  = MID_TIMER_EXPIRY;

   PUT_IMH_LEN (sizeof (cmd_ptr->cmd.timer_expiry) - sizeof (cmd_ptr->cmd.timer_expiry.IMH),
                     &(cmd_ptr->cmd.timer_expiry.IMH));

   cmd_ptr->cmd.timer_expiry.data        = SYS_MODEM_AS_ID_2;
   cmd_ptr->cmd.timer_expiry.timer_id    = (byte)(param >> 16);

   /* Put on REG command queue */
   mm_put_cmd(cmd_ptr);
}
#if defined(FEATURE_TRIPLE_SIM) || defined(FEATURE_SGLTE_DUAL_SIM)
void mm_timer_expiry_sub2(unsigned long param)
{
   mm_cmd_type   *cmd_ptr;

   /* Get command buffer from reg task */
   cmd_ptr = mm_get_cmd_buf();

   if (cmd_ptr == NULL)
   {
      MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
   }
    /* Construct message */
   cmd_ptr->cmd.timer_expiry.IMH.message_set = MS_TIMER;
   cmd_ptr->cmd.timer_expiry.IMH.message_id  = MID_TIMER_EXPIRY;

   PUT_IMH_LEN (sizeof (cmd_ptr->cmd.timer_expiry) - sizeof (cmd_ptr->cmd.timer_expiry.IMH),
                     &(cmd_ptr->cmd.timer_expiry.IMH));

   cmd_ptr->cmd.timer_expiry.data        = SYS_MODEM_AS_ID_3;
   cmd_ptr->cmd.timer_expiry.timer_id    = (byte)(param >> 16);

   /* Put on REG command queue */
   mm_put_cmd(cmd_ptr);
}
#endif
#endif
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif

#ifdef FEATURE_WCDMA
/*===========================================================================

FUNCTION MM_RRC_GET_CMD_BUF

DESCRIPTION
  Get a mm_cmd_type buffer for specific use of RRC with the header information
  already filled in.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to mm_cmd_type buffer.

SIDE EFFECTS
  None

===========================================================================*/

mm_cmd_type *mm_rrc_get_cmd_buf(rrc_to_mm_sink_e_type cmd_type)
{
   mm_cmd_type *cmd;

   if(cmd_type >= RRC_MM_SINK_CMD_MAX)
   {
      MSG_ERROR_DS(MM_SUB, "=MM= Invalid mm command ID is supplied", 0,0,0);
      return NULL;
   }
#ifndef FEATURE_MODEM_HEAP
   cmd = (mm_cmd_type*)gs_alloc(sizeof(mm_cmd_type));
#else
   cmd = (mm_cmd_type*)modem_mem_calloc(1,sizeof(mm_cmd_type), MODEM_MEM_CLIENT_NAS);
#endif
   mm_check_for_null_ptr((void*)cmd);

   cmd->cmd.hdr.message_set = MS_MM_RRC;
   cmd->cmd.hdr.message_id  = (byte) cmd_type;
   switch(cmd_type)
   {
      case RRC_EST_CNF:
         PUT_IMH_LEN(0, &cmd->cmd.hdr);
         break;
      case RRC_REL_IND:
         PUT_IMH_LEN(sizeof(rrc_rel_ind_type) - sizeof(IMH_T), &cmd->cmd.hdr);
         break;
      case RRC_SYNC_IND:
         PUT_IMH_LEN(sizeof(rrc_sync_ind_type)- sizeof(IMH_T), &cmd->cmd.hdr);
         break;
      case RRC_SERVICE_CNF:
         PUT_IMH_LEN(sizeof(rrc_service_cnf_type) - sizeof(IMH_T), &cmd->cmd.hdr);
         break;
      case RRC_SERVICE_IND:
         PUT_IMH_LEN(sizeof(rrc_service_ind_type) - sizeof(IMH_T), &cmd->cmd.hdr);
         break;
      case RRC_SERVICE_IND_FROM_OTA_MSG:
         PUT_IMH_LEN(sizeof(rrc_service_ind_type) - sizeof(IMH_T), &cmd->cmd.hdr);
         break;
      case RRC_PLMN_LIST_CNF:
         PUT_IMH_LEN(sizeof(rrc_plmn_list_cnf_type)- sizeof(IMH_T), &cmd->cmd.hdr);
         break;
      case RRC_ABORT_IND:
         PUT_IMH_LEN(sizeof(rrc_abort_ind_type)- sizeof(IMH_T), &cmd->cmd.hdr);
         break;
      case RRC_PAGE_IND:
         PUT_IMH_LEN(sizeof(rrc_page_ind_type)- sizeof(IMH_T), &cmd->cmd.hdr);
         break;
      case RRC_DATA_IND:
         PUT_IMH_LEN(sizeof(rrc_data_ind_type)- sizeof(IMH_T), &cmd->cmd.hdr);
         break;
      case RRC_OPEN_SESSION_CNF:
         PUT_IMH_LEN(sizeof(rrc_open_session_cnf_type)- sizeof(IMH_T), &cmd->cmd.hdr);
         break;
      case RRC_CLOSE_SESSION_IND:
         PUT_IMH_LEN(sizeof(rrc_close_session_ind_type)- sizeof(IMH_T), &cmd->cmd.hdr);
         break;
      case RRC_CLOSE_SESSION_CNF:
         PUT_IMH_LEN(sizeof(rrc_close_session_cnf_type)- sizeof(IMH_T), &cmd->cmd.hdr);
         break;
      case RRC_ACTIVATION_IND:
         PUT_IMH_LEN(sizeof(rrc_activation_ind_type)- sizeof(IMH_T), &cmd->cmd.hdr);
         break;
      case RRC_ABORT_CNF:
         PUT_IMH_LEN(0, &cmd->cmd.hdr);
         break;
      case RRC_DEACT_CNF:
         PUT_IMH_LEN(0, &cmd->cmd.hdr);
         break;
      case RRC_MODE_CHANGE_CNF:
         PUT_IMH_LEN(0, &cmd->cmd.hdr);
         break;
      case RRC_STOP_WCDMA_MODE_CNF:
         PUT_IMH_LEN(0, &cmd->cmd.hdr);
         break;
      case RRC_INTERRAT_CELL_CHANGE_IND:
         PUT_IMH_LEN(sizeof(rrc_interrat_cell_change_ind_type)- sizeof(IMH_T), &cmd->cmd.hdr);
         break;
#ifdef FEATURE_HSDPA
      case RRC_HSDPA_HSUPA_CELL_IND:
         PUT_IMH_LEN(sizeof(rrc_hsdpa_hsupa_cell_ind_type) - sizeof(IMH_T), &cmd->cmd.hdr);
         break;
#ifdef FEATURE_HSPA_CALL_STATUS_IND
      case RRC_HSPA_CALL_STATUS_IND:
         PUT_IMH_LEN(sizeof(rrc_hspa_call_status_ind_type) - sizeof(IMH_T), &cmd->cmd.hdr);
         break;
#endif
#endif
#ifdef FEATURE_SPEC_COMPLIANT_WTOG_CELL_CHANGE_ORDER
      case RRC_WTOG_CCO_COMPLETE_IND:
         PUT_IMH_LEN(0, &cmd->cmd.hdr);
         break;
#endif /* FEATURE_SPEC_COMPLIANT_WTOG_CELL_CHANGE_ORDER */
#ifdef FEATURE_OOSC_SERVICE_STATUS_UI_UPDATE
      case RRC_CONNECTED_MODE_SERVICE_STATUS_IND:
         PUT_IMH_LEN(sizeof(rrc_conn_mode_service_status_ind_type) - sizeof(IMH_T), &cmd->cmd.hdr);
         break;
#endif
#ifdef FEATURE_MBMS //rrc_mbms_conn_req_ind_type
      case RRC_MBMS_CONN_REQ_IND:
         PUT_IMH_LEN(sizeof(rrc_mbms_conn_req_ind_type)- sizeof(IMH_T), &cmd->cmd.hdr);
         break;
#endif
      case RRC_FAST_DORMANCY_CNF:
         PUT_IMH_LEN(sizeof(rrc_fast_dormancy_cnf_type)- sizeof(IMH_T), &cmd->cmd.hdr);
         break;
#if defined(FEATURE_WRLF_SYSTEM_SEL) && defined(FEATURE_NAS_GW) && !defined(FEATURE_NAS_CS_DISABLED)
      case RRC_DISABLE_BPLMN_IND:
         PUT_IMH_LEN(sizeof(rrc_disable_bplmn_ind_type)- sizeof(IMH_T), &cmd->cmd.hdr);
         break;
      case RRC_SUITABLE_SEARCH_END_IND:
         PUT_IMH_LEN(0, &cmd->cmd.hdr);
         break;
#endif
#ifdef FEATURE_LTE
      case RRC_SRVCC_HANDOVER_FAILURE_IND:
         PUT_IMH_LEN(0, &cmd->cmd.hdr);
         break;
#endif
      case RRC_PCH_STATE_IND:
        PUT_IMH_LEN(sizeof(rrc_pch_state_ind_type)- sizeof(IMH_T), &cmd->cmd.hdr);
        break;

      default:
         MSG_FATAL_DS(MM_SUB, "=MM= Unknown RRC command type", 0,0,0);
   }

   return cmd;
}

/*===========================================================================

FUNCTION MM_TDSRRC_GET_CMD_BUF

DESCRIPTION
  Get a mm_cmd_type buffer for specific use of TD-SCDMA RRC with the header information
  already filled in.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to mm_cmd_type buffer.

SIDE EFFECTS
  None

===========================================================================*/

mm_cmd_type *mm_tdsrrc_get_cmd_buf(rrc_to_mm_sink_e_type cmd_type)
{
   mm_cmd_type *cmd;

   cmd = mm_rrc_get_cmd_buf( cmd_type );

   if (cmd != NULL)
   {
     cmd->cmd.hdr.message_set = MS_MM_TDSRRC;
   }

   return cmd;
}
#endif

#if defined(FEATURE_GSM_GPRS) && defined (FEATURE_NAS_GW)
/*===========================================================================

FUNCTION MM_GLLC_GET_CMD_BUF

DESCRIPTION
  Get a mm_cmd_type buffer for specific use of GPRS LLC with the header
  information already filled in.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to mm_cmd_type buffer.

SIDE EFFECTS
  None

===========================================================================*/

#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
mm_cmd_type *mm_gllc_get_cmd_buf( uint8 cmd )
{
#ifndef FEATURE_MODEM_HEAP
  mm_cmd_type *cmd_p = (mm_cmd_type*)gs_alloc(sizeof(mm_cmd_type));
#else
  mm_cmd_type *cmd_p = (mm_cmd_type*)modem_mem_calloc(1,sizeof(mm_cmd_type), MODEM_MEM_CLIENT_NAS);
#endif
  mm_check_for_null_ptr((void*)cmd_p);

  cmd_p -> cmd.hdr.message_set = MS_LLC_GMM;
  cmd_p -> cmd.hdr.message_id  = cmd;

  switch( cmd )
  {
  case GLLCMM_LLGMM_STATUS_IND:
    PUT_IMH_LEN(sizeof(gllcmm_llgmm_status_ind_type) - sizeof(IMH_T), &cmd_p->cmd.hdr);
    break;

  case GLLCMM_LLGMM_TRIGGER_IND:
    PUT_IMH_LEN(sizeof(gllcmm_llgmm_trigger_ind_type) - sizeof(IMH_T), &cmd_p->cmd.hdr);
    break;

  case GLLCMM_LLGMM_RDY_TMR_IND:
    PUT_IMH_LEN(sizeof(gllcmm_llgmm_rdy_tmr_ind_type) - sizeof(IMH_T), &cmd_p->cmd.hdr);
    break;

  case GLLCMM_LLGMM_SUSPEND_CNF:
    PUT_IMH_LEN(sizeof(gllcmm_llgmm_suspend_cnf_type) - sizeof(IMH_T), &cmd_p->cmd.hdr);
    break;

  case GLLCMM_LLGMM_NEW_TLLI_IND:
    PUT_IMH_LEN(sizeof(gllcmm_llgmm_new_tlli_ind_type) - sizeof(IMH_T), &cmd_p->cmd.hdr);
    break;

  case GLLCMM_LL_UNITDATA_IND:
    PUT_IMH_LEN(sizeof(gllcmm_ll_unitdata_ind_type) - sizeof(IMH_T), &cmd_p->cmd.hdr);
    break;

  case GLLCMM_LL_XID_IND:
    PUT_IMH_LEN(sizeof(gllcmm_ll_xid_ind_type) - sizeof(IMH_T), &cmd_p->cmd.hdr);
    break;

  case GLLCMM_LL_XID_CNF:
    PUT_IMH_LEN(sizeof(gllcmm_ll_xid_cnf_type) - sizeof(IMH_T), &cmd_p->cmd.hdr);
    break;

  case GLLCMM_LL_RESET_IND:
    PUT_IMH_LEN(sizeof(gllcmm_ll_reset_ind_type) - sizeof(IMH_T), &cmd_p->cmd.hdr);
    break;

  case GLLCMM_LL_STATUS_IND:
    PUT_IMH_LEN(sizeof(gllcmm_ll_status_ind_type) - sizeof(IMH_T), &cmd_p->cmd.hdr);
    break;

#if defined(FEATURE_GPRS_PS_HANDOVER) && defined(FEATURE_NAS_GW)
  case GLLCMM_LLGMM_PSHO_CNF:
    PUT_IMH_LEN(sizeof(gllcmm_ll_psho_cnf_type) - sizeof(IMH_T), &cmd_p->cmd.hdr);
    break;    
#endif    

  case GLLCSMS_LL_UNITDATA_IND:
    PUT_IMH_LEN(sizeof(gllcsms_ll_unitdata_ind_type) - sizeof(IMH_T), &cmd_p->cmd.hdr);
    break;

  case GLLCSMS_LL_XID_IND:
    PUT_IMH_LEN(sizeof(gllcsms_ll_xid_ind_type) - sizeof(IMH_T), &cmd_p->cmd.hdr);
    break;

  case GLLCSMS_LL_XID_CNF:
    PUT_IMH_LEN(sizeof(gllcsms_ll_xid_cnf_type) - sizeof(IMH_T), &cmd_p->cmd.hdr);
    break;

  case GLLCSMS_LL_RESET_IND:
    PUT_IMH_LEN(sizeof(gllcsms_ll_reset_ind_type) - sizeof(IMH_T), &cmd_p->cmd.hdr);
    break;

  case GLLCSMS_LL_STATUS_IND:
    PUT_IMH_LEN(sizeof(gllcsms_ll_status_ind_type) - sizeof(IMH_T), &cmd_p->cmd.hdr);
    break;

  default:
     MSG_FATAL_DS(MM_SUB, "=MM= Unknown LLC command type", 0,0,0);
  }

  return cmd_p;

} /* end of mm_gllc_get_cmd_buf() */

#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
#endif /* FEATURE_GSM_GPRS */

#ifdef FEATURE_LTE

void mm_emm_oos_rlf_msg_handler(rex_sigs_type sigs,void **mm_process_msg_fn_ptr)
{
  mm_process_messages_fp **local_fp_ptr = ((mm_process_messages_fp **)mm_process_msg_fn_ptr);

  if (sigs & MM_CMD_Q_SIG)
  {
    *local_fp_ptr = (void *)mmcoord_route_oos_rlf_mm_emm_messages;  
  }
  else if(sigs & ESM_EMM_CMD_Q_SIG)
  {
    *local_fp_ptr = (void *)emm_process_oos_rlf_outgoing_esm_message; 
  }
  else if(sigs & RRC_EMM_CMD_Q_SIG)
  {
    *local_fp_ptr =(void *) emm_process_oos_rlf_rrc_primitive; 
  }
  else if(sigs & TLB_EMM_CMD_Q_SIG )
  {
    *local_fp_ptr = emm_process_outgoing_tlb_message; 
  }
#ifdef FEATURE_LTE_TO_1X
  else if(sigs & CSFB_EMM_CMD_Q_SIG  )
  {
    *local_fp_ptr = emm_process_csfb_primitive; 
  }
#endif
#if defined (FEATURE_LTE_REL9)
#ifdef FEATURE_NAS_GPS_ENABLE
  else if(sigs & GPS_EMM_CMD_Q_SIG )
  {
    *local_fp_ptr = emm_process_ul_generic_nas_transport_msg; 
  } 
#endif
#endif
#ifdef FEATURE_CIOT
 else if(sigs & PHY_EMM_CMD_Q_SIG )
 {
   *local_fp_ptr = emm_process_phy_cmd_msg; 
 } 
#endif
  else
  {
    *local_fp_ptr = (void *)NULL;
    MSG_ERROR_DS(MM_SUB,"=EMM= received invalid message in EMM queue during OOS/RLF",0,0,0);
  }
}


#ifdef FEATURE_NAS_GW
#ifndef FEATURE_NAS_IRAT_DISABLED
void emm_process_LTE_to_GW_rat_change(rex_sigs_type sigs,void **mm_process_msg_fn_ptr)
{
  mm_process_messages_fp **local_fp_ptr = ((mm_process_messages_fp **)mm_process_msg_fn_ptr);
  if (sigs & MM_CMD_Q_SIG)
  {
    *local_fp_ptr = (void *)mmmultimode_route_L2GW_irat_mm_emm_messages;  
  }
  else
  if(sigs & ESM_EMM_CMD_Q_SIG)
  {
    *local_fp_ptr = (void *)emm_process_irat_outgoing_esm_message; 
  }
  else if(sigs & RRC_EMM_CMD_Q_SIG)
  {
    *local_fp_ptr = (void *)emm_process_irat_rrc_primitive; 
  }
  else if(sigs & TLB_EMM_CMD_Q_SIG )
  {
    *local_fp_ptr = emm_process_outgoing_tlb_message; 
  }
#ifdef FEATURE_LTE_TO_1X
  else if(sigs & CSFB_EMM_CMD_Q_SIG  )
  {
    *local_fp_ptr = emm_process_csfb_primitive; 
  }
#endif
#if defined (FEATURE_LTE_REL9)
#ifdef FEATURE_NAS_GPS_ENABLE
  else if(sigs & GPS_EMM_CMD_Q_SIG )
  {
    *local_fp_ptr = emm_process_ul_generic_nas_transport_msg; 
  } 
#endif
#endif
  else
  {
    *local_fp_ptr = NULL;
    MSG_ERROR_DS(MM_SUB,"=MM= received invalid message in MM/EMM queue during Inter-RAT",0,0,0);
  }
}

void mm_process_GW_to_LTE_rat_change(rex_sigs_type sigs,void **mm_process_msg_fn_ptr)
{
  mm_process_messages_fp **local_fp_ptr = ((mm_process_messages_fp **)mm_process_msg_fn_ptr);

  if (sigs & MM_CMD_Q_SIG)
  {
    *local_fp_ptr = (void *)mmmultimode_route_GW2L_irat_mm_emm_messages;  
  }
  else if(sigs & ESM_EMM_CMD_Q_SIG)
  {
    *local_fp_ptr = (void *)emm_process_irat_outgoing_esm_message; 
  }
  else if(sigs & RRC_EMM_CMD_Q_SIG)
  {
    *local_fp_ptr = (void *)emm_process_unexpected_rrc_primitive;
  }
  else if(sigs & TLB_EMM_CMD_Q_SIG )
  {
    *local_fp_ptr = emm_process_outgoing_tlb_message; 
  }
#ifdef FEATURE_LTE_TO_1X
  else if(sigs & CSFB_EMM_CMD_Q_SIG  )
  {
    *local_fp_ptr = emm_process_csfb_primitive; 
  }
#endif
#if defined (FEATURE_LTE_REL9)
#ifdef FEATURE_NAS_GPS_ENABLE
  else if(sigs & GPS_EMM_CMD_Q_SIG )
  {
    *local_fp_ptr = emm_process_ul_generic_nas_transport_msg; 
  } 
#endif
#endif
  else
  {
    *local_fp_ptr = NULL;
    MSG_ERROR_DS(MM_SUB,"=MM= received invalid message in MM/EMM queue during Inter-RAT",0,0,0);
  }
}

#endif

#endif
#endif/*FEATURE_LTE*/

#endif
#ifdef FEATURE_MODEM_CONFIG_REFRESH
/*===========================================================================
FUNCTION MM_MCFG_REFRESH_CB

DESCRIPTION
This is a callback function used by MCFG for NV/EFS refresh purpose
This function puts the received refresh info into MM command queue

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean mm_mcfg_refresh_cb(mcfg_refresh_info_s_type *p_info)
{
   mm_cmd_type   *cmd_ptr;


   if(p_info->type != MCFG_REFRESH_TYPE_SUBS)
   {
     MSG_ERROR_DS_1(MM_SUB,"=MM= Unexpected refresh type %d rcvd in mm_mcfg_refresh_cb", p_info->type);
     return FALSE;
   }

   /* Get command buffer from MM task */
   if( (cmd_ptr = mm_get_cmd_buf()) != NULL)
   {
      /* Construct message */
     cmd_ptr->cmd.mcfg_refresh_ind.IMH.message_set = MS_MM_MM;
     cmd_ptr->cmd.mcfg_refresh_ind.IMH.message_id  = MM_MCFG_REFRESH_IND;

     PUT_IMH_LEN (sizeof(cmd_ptr->cmd.mcfg_refresh_ind) - sizeof(IMH_T),
                       &(cmd_ptr->cmd.mcfg_refresh_ind.IMH));

     cmd_ptr->cmd.mcfg_refresh_ind.as_id = (sys_modem_as_id_e_type)p_info->sub_index;
     cmd_ptr->cmd.mcfg_refresh_ind.reference_id = p_info->reference_id;

     /* Put on MM command queue */
     mm_put_cmd(cmd_ptr);
   }
   else
   {
     MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
   }
   return TRUE;
}

/*===========================================================================
FUNCTION MM_MCFG_REFRESH_REGISTER

DESCRIPTION
This function call registers MM Task with MCFG for NV/EFS refresh purpose
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void mm_mcfg_refresh_register(void)
{
  mcfg_refresh_registration_s_type mcfg_reg;

  memset((void *)&mcfg_reg,0x00,sizeof(mcfg_refresh_registration_s_type));

  mcfg_reg.order = MCFG_REFRESH_ORDER_50;  
  mcfg_reg.slot_mask = MCFG_REFRESH_INDEX_MASK_NONE;
  mcfg_reg.sub_mask = MCFG_REFRESH_INDEX_MASK_ALL;
  mcfg_reg.group = 0;
  mcfg_reg.cb = mm_mcfg_refresh_cb;

  if(mcfg_refresh_register(&mcfg_reg) != TRUE)
  {
     MSG_ERROR_DS_0( MM_SUB, "=MM= ERROR: MCFG Registration failed");    
  }
}
#endif

/*===========================================================================

FUNCTION MM_PUT_CMD

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
void mm_put_cmd(mm_cmd_type *cmd_ptr)
{

   /* Check for Null pointer */
   if(cmd_ptr != NULL)
   {
#ifdef FEATURE_NAS_GW
#ifdef FEATURE_SEGMENT_LOADING
      if (((cmd_ptr->cmd.hdr.message_set == MS_MM_RRC)&&(get_wcdma_interface())) ||
          ((cmd_ptr->cmd.hdr.message_set == MS_MM_TDSRRC)&&(get_tdscdma_interface())))
#else
      if ((cmd_ptr->cmd.hdr.message_set == MS_MM_RRC) ||
          (cmd_ptr->cmd.hdr.message_set == MS_MM_TDSRRC))
#endif
      {
        word      message_length;
        /*    Get message length */
        GET_IMH_LEN(message_length, &(cmd_ptr->cmd));
        /*    Add in header size */
        message_length += sizeof(IMH_T);
        /* Log using CFA packet */
        cfa_log_packet( GS_QUEUE_MM, message_length, (byte*)&(cmd_ptr->cmd) );
      }
#endif
      /* First initialize the queue link. This is always done
      before putting an item in a queue. */
      (void) q_link(cmd_ptr, &(cmd_ptr->link));

      /* Put the message on the queue */
      q_put(&mm_cmd_q, &cmd_ptr->link);

      /* Set the command queue signal for */
     /* receive task                     */
     (void) rex_set_sigs(mm_tcb_ptr, MM_CMD_Q_SIG);

   }
   else
   {
      /* Raise error condition */
      MSG_FATAL_DS(MM_SUB, "=MM= Trying to put a null pointer on the mm queue", 0,0,0);
   }
}

#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif

void mn_cnm_put_cmd(mn_cnm_cmd_type *cmd_ptr)
{

   /* Check for Null pointer */
   if(cmd_ptr != NULL)
   {

      /* First initialize the queue link. This is always done
      before putting an item in a queue. */
      (void) q_link(cmd_ptr, &(cmd_ptr->link));

      /* Put the message on the queue */
      q_put(&mn_cnm_cmd_q, &cmd_ptr->link);

      /* Set the command queue signal for */
      /* receive task                     */
      (void)rex_set_sigs(mm_tcb_ptr, MN_CNM_CMD_Q_SIG);

   }
   else
   {
       /* Raise error condition */
       MSG_FATAL_MN_DS(MN_SUB,"=MNCNM= NULL Command pointer passed!!\n",0,0,0);

   }
}



static mn_cnm_cmd_type *mn_cnm_cmd_q_get(void)
{

   /* Read next item from queue */
   return (mn_cnm_cmd_type*)q_get(&mn_cnm_cmd_q);
   
}

static void mn_cnm_cmd_q_free(mn_cnm_cmd_type *cmd_ptr)
{

   /* Free the memory for the command */
#ifndef FEATURE_MODEM_HEAP
   gs_free( cmd_ptr );
#else
   modem_mem_free(cmd_ptr, MODEM_MEM_CLIENT_NAS);
#endif
}
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif


void cm_init_conn_ids_crit_sect(void)
{
   /* Initialize the critical section */
   (void)gs_attach_lock(&cm_conn_ids_crit_sect);
}

void mn_cnm_process_external_messages(void)
{
     gs_status_T           status;     
     IMH_T                 *msg_header = NULL;     
     unsigned int          message_length;
     mn_cnm_cmd_type       *cmd_ptr;
     byte                  *message_p = NULL;

     
     status = GS_SUCCESS;

#ifdef FEATURE_MODEM_HEAP
     message_p = (byte*)modem_mem_calloc(1, sizeof( mn_cnm_cmd_type ), MODEM_MEM_CLIENT_NAS);
#else
     message_p = message;
#endif
     if(message_p == NULL)
     {
         ERR("Memory ALLOC failure", 0, 0, 0); 
         return;
     }
     MSG_HIGH("mn_cnm_process_external_messages, message_p = %d",message_p,0,0);
     /* Create variable which points to message as if its an IMH */
     msg_header = (IMH_T *)message_p; 
     while((cmd_ptr = mn_cnm_cmd_q_get()) != NULL)
     {
         
         GET_IMH_LEN(message_length,((IMH_T *)&cmd_ptr->cmd));
#ifdef FEATURE_DUAL_SIM
         if(cmd_ptr->cmd.hdr.message_set == MS_MN_CC)
         {
           if(MNCNM_IS_SINGLE_STACK)
           {
             MSG_HIGH_DS_3(MN_SUB,"=MNCNM= =MN= MS: %d   MSG_ID: %d AS_ID_%d", cmd_ptr->cmd.hdr.message_set, cmd_ptr->cmd.hdr.message_id, mn_as_id+1);
           }
#ifdef FEATURE_DUAL_ACTIVE
           else
           {
             MSG_HIGH_DS_3(MN_SUB,"=MNCNM= =MN= MS: %d  MSG_ID: %d AS_ID_%d", cmd_ptr->cmd.hdr.message_set, cmd_ptr->cmd.hdr.message_id, mn_dsda_as_id+1);
           }
#endif
         }
         else
         {
           dword trail_bytes = 0;
           byte *trail_bytes_loc_p = (byte *) &(cmd_ptr->cmd);
           trail_bytes_loc_p += (message_length + sizeof(IMH_T)-sizeof(dword));
           memscpy(&trail_bytes, sizeof(dword) ,trail_bytes_loc_p, sizeof(dword));
           MSG_HIGH_DS_3(MN_SUB,"=MNCNM= =MN= MS: %d   MSG_ID: %d ...0x%x", cmd_ptr->cmd.hdr.message_set, cmd_ptr->cmd.hdr.message_id, trail_bytes);
         }
#endif

         memset (message_p, 0x00, sizeof(mn_cnm_cmd_type));      
         /* Copy data from command queue to local buffer */
         memscpy(message_p, sizeof( mn_cnm_cmd_type ) ,&cmd_ptr->cmd, (unsigned int)(message_length + sizeof(IMH_T)));
         
         /* Deallocate Command Buffer */
         mn_cnm_cmd_q_free(cmd_ptr);

         if ( status == GS_SUCCESS  
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
               && mn_cnm_validate_and_set_as_id( (IMH_T *) msg_header)
#endif
             )
         {                    
             switch (msg_header->message_set)
             {
             case MS_CC_MM:
             case MS_CC_MM_PS:
                 CNM_route_CNM_messages( (IMH_T *) msg_header );
                 break;
                 
             case MS_MN_CM:
             case MS_MN_UASMS:
                 MN_handle_MN_messaging( (IMH_T *) msg_header);
                 break;
                 
             case MS_MN_CC:
                 
             /*
             * messages between MN and CC - need to establish
             * the destination based on the message id
                 */
                 
                 if (msg_header->message_id < 0x40 )
                 {
                     
                 /*
                 * message is going to CM
                     */
                     
                     CNM_route_CNM_messages( (IMH_T *) msg_header);
                     
                 }
                 else
                 {
                     MN_handle_MN_messaging( (IMH_T *) msg_header);
                 }
                 break;
                 
             case MS_MN_SS:
                 
             /*
             * need to decide on destination
                 */
                 
                 if (msg_header->message_id < 0x80 )
                 {
                     
                 /*
                 * message is going to CM
                     */
                     
                     CNM_route_CNM_messages( (IMH_T *) msg_header);
                 }
                 else
                 {
                     MN_handle_MN_messaging( (IMH_T *) msg_header);
                 }
                 break;
                 
                 
             case MS_MN_SMS:
                 
             /*
             * need to decide on destination
                 */
                 
                 if (msg_header->message_id < 0x80 )
                 {
                     
                 /*
                 * message is going to CM
                     */
                     
                     CNM_route_CNM_messages( (IMH_T *) msg_header);
                 }
                 else
                 {
                     MN_handle_MN_messaging( (IMH_T *) msg_header);
                 }
                 break;
                 
#if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))                 
             case MS_TIMER:
                 
             /*
             * need to use timer id to decide on message
             * destination
                 */
                 
                 if (msg_header->message_id == MID_TIMER_EXPIRY)
                 {
                     gs_queue_id_T queue_id = GS_TIMER_QUEUE_MN_CNM;
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
                     if(mn_dsda_as_id == (mn_dsda_as_id_e_type)SYS_MODEM_AS_ID_2)
                     {
                       queue_id = GS_TIMER_QUEUE_MN_CNM_SUB2;
                     }
#endif
                     gs_clear_timer(queue_id, ((timer_expiry_T *)msg_header)->timer_id);

                     if ((((timer_expiry_T *)msg_header)->timer_id >= MN_CC_TIMER) &&
                         (((timer_expiry_T *)msg_header)->timer_id < 
                                            (MN_SMS_TIMER+MAXNO_SMS_TRANSACTIONS)))
                     {
                         /* timer message for MN */
                         MN_handle_MN_messaging( (IMH_T *) msg_header);
                     }
                     else
                     {
                         /* * message is going to CM */
                         CNM_route_CNM_messages( (IMH_T *) msg_header);
                     }
                 }
                 else
                 {
                     ERR("Unknown timer expiration, message_id = %d\n",
                         (dword) msg_header->message_id, 0, 0);
                 }
                 break;
#endif /*(defined(FEATURE_GSM) || defined(FEATURE_WCDMA))*/
                 
             case MS_MN_REG:
                 MN_handle_MN_messaging( (IMH_T *) msg_header);
                 break;
                 
             default:
                 ERR("Unexpected message set, message_set = %d\n",
                     (dword) msg_header->message_set, 0, 0);
                 break;
             } /* switch msg_header... */
                    
         }   /* if status... */
         else
         {
             ERR("Bad message read, status = %d\n", (dword) status, 0, 0);
         }   /* if status... */

	 /* After processing any external message, just check if there is any message
	  * in internal high priority queue */
#if 0
         mn_cnm_process_internal_messages();
#endif
     }   /* while cmd_ptr... */
#ifdef FEATURE_MODEM_HEAP
     if(message_p != NULL)
     {
       modem_mem_free(message_p, MODEM_MEM_CLIENT_NAS);
     }
#endif
     message_p = NULL;
       
}


/*===========================================================================

FUNCTION MM_SEND_ROUTINE

DESCRIPTION
  This is the MM-specific routine used when other layers call gs_send_message

DEPENDENCIES
  None

RETURN VALUE
  GS_STATUS - GS_SUCCESS if sent, GS_FAILURE otherwise

SIDE EFFECTS
  None

===========================================================================*/
gs_status_T mm_send_routine( void *msg_p )
{
    word          message_length;
    void         *cmd_ptr;

    /* Get message length */
    GET_IMH_LEN(message_length, msg_p);

    message_length = MIN(sizeof(mm_int_cmd_type_T) - sizeof(IMH_T),message_length);

    /* Add in header size */
    message_length += sizeof(IMH_T);

    /* Get command buffer from destination task */
    if((cmd_ptr = mm_get_cmd_buf()) != NULL)
    {
        /* Copy data to command buffer */
        memscpy(&((mm_cmd_type*)cmd_ptr)->cmd,sizeof(mm_int_cmd_type_T), (byte*)msg_p, message_length);

        PUT_IMH_LEN((message_length-sizeof(IMH_T)), &((mm_cmd_type*)cmd_ptr)->cmd);

        /* Put message in destination task queue */
        mm_put_cmd((mm_cmd_type*) cmd_ptr);
        return GS_SUCCESS;
    }
    return GS_FAILURE;
}


#ifdef FEATURE_LTE


/*=============================================================================

FUNCTION  LTE_NAS_EFS_READ

DESCRIPTION
  This function reads NV items from EFS upon power up

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void lte_nas_efs_read
(
  void
)
{
  emm_ctrl_data_type *emm_s1_ctrl_data_ptr = NULL;
  mcfg_fs_status_e_type efs_status = MCFG_FS_STATUS_EFS_ERR;
  emm_nas_nv_items_T emm_nas_nv;
#ifndef FEATURE_NAS_GW
  ghdi_status_T nv_status;
#endif
#ifdef FEATURE_TDSCDMA
  uint8 nv_special_tdscdma_test_setting_enable = 0;
#endif

#ifdef FEATURE_DUAL_SIM
  for(mm_sub_id = MM_AS_ID_1, mm_as_id = MM_AS_ID_1; mm_sub_id < mm_sim_max_subscription; mm_sub_id++, mm_as_id++)
#endif
  {
    emm_read_carrier_specific_efs();
  }

#ifdef FEATURE_DUAL_SIM
  mm_sub_id = MM_AS_ID_1;
  mm_as_id = MM_AS_ID_1;
#endif

  emm_s1_ctrl_data_ptr = emm_db_get_ctrl_data();

  /* default encryption and integrity algorithms supported */
  memset((void *)&emm_s1_ctrl_data_ptr->ue_security_capability, 0, sizeof(lte_nas_emm_ue_security_type));

  /* 
   * Initialize STRUCT emm_nas_nv to 0.
   * Later in the function, values read from EFS will be stored here.
   */
  memset(&emm_nas_nv, 0x00, sizeof(emm_nas_nv_items_T));
  dog_hb_report(DOG_MM_RPT);
  efs_status = mcfg_fs_read("/nv/item_files/modem/nas/emm_nas_nv_items",
                            &emm_nas_nv,
                            sizeof(emm_nas_nv_items_T),
                            MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)MM_AS_ID_1);

  if(efs_status != MCFG_FS_STATUS_OK)
  {
    MSG_HIGH_DS_1(MM_SUB ,"=MM= Unable to read NV emm_nas_nv_items, status : %d ",efs_status);
  }
  else
  {
    if((emm_nas_nv.version >= 3) &&
       (emm_nas_nv.psm_local_detach_enabled == TRUE))
    {
      emm_s1_ctrl_data_ptr->psm_local_detach_enabled = TRUE;
    }
    else
    {
      emm_s1_ctrl_data_ptr->psm_local_detach_enabled = FALSE;
    }
  }
  MSG_HIGH_DS_2(MM_SUB , "=EMM= version of emm_nas_nv = %d  psm_local_detach_enabled %d",
                emm_nas_nv.version,emm_s1_ctrl_data_ptr->psm_local_detach_enabled);


#ifdef FEATURE_NAS_EAB
  /* Marking this as true for now, may need NV later on to control this feature */
  emm_s1_ctrl_data_ptr->eab_barring_flag = TRUE;  
#endif
#ifndef FEATURE_NAS_GW
  if ((nv_status = mm_ghdi_nvmem_read(NV_GPRS_ANITE_GCF_I,
       (ghdi_nvmem_data_T *)&gmm_anite_gcf_flag)) != GHDI_SUCCESS)
  {
    gmm_anite_gcf_flag = FALSE;  /* Set the default value */
    MSG_ERROR_DS_1(MM_SUB, "=MM= Unable to read NV for Anite GCF Flag: %d", nv_status);
  }
  /* Table 10.5.139 has the the binary coded value Split paging cycle */
  else if ( (gmm_anite_gcf_flag != FALSE) && (gmm_anite_gcf_flag != TRUE))
  {
    MSG_HIGH_DS_1(MM_SUB, "=MM= Wrong value for Anite GCF Flag %d", gmm_anite_gcf_flag);
    gmm_anite_gcf_flag = FALSE;  /* Set the default value */
  }

  if ((nv_status = mm_ghdi_nvmem_read(NV_NON_DRX_TIMER_I,
       (ghdi_nvmem_data_T *)&gmm_non_drx_timer)) != GHDI_SUCCESS)
  {
    gmm_non_drx_timer = GPRS_NON_DRX_TIMER;  /* Set the default value */
    MSG_ERROR_DS_1(MM_SUB, "=MM= Unable to read NV for non_drx_timer: %d", nv_status);
  }
  else if (gmm_non_drx_timer > 7)
  {
    MSG_HIGH_DS_1(MM_SUB, "=MM= Wrong value for non_drx_timer %d", gmm_non_drx_timer);
    gmm_non_drx_timer = GPRS_NON_DRX_TIMER;  /* Set the default value */
  }
  /* --------------------------------------------------------------------
  ** Read the SPLIT PAGING CYCLE value from the NV
  ** ------------------------------------------------------------------*/
  if ((nv_status = mm_ghdi_nvmem_read(NV_SPLIT_PAGING_CYCLE_I,
           (ghdi_nvmem_data_T *)&gmm_split_paging_cycle)) != GHDI_SUCCESS)
  {
    gmm_split_paging_cycle = GPRS_SPLIT_PAGING_CYCLE;  /* Set the default value */
    MSG_ERROR_DS_1(MM_SUB, "=MM= Unable to read NV for split_paging_cycle: %d", nv_status);
  }
  /* Table 10.5.139 has the the binary coded value Split paging cycle */
  else if (gmm_split_paging_cycle > 98)
  {
    MSG_HIGH_DS_1(MM_SUB, "=MM= Wrong value for split_paging_cycle %d", gmm_split_paging_cycle);
    gmm_split_paging_cycle = GPRS_SPLIT_PAGING_CYCLE;  /* Set the default value */
  }
#endif
  if((efs_status = mcfg_fs_read("/nv/item_files/modem/nas/isr", 
                              &emm_s1_ctrl_data_ptr->isr_efs_flag, 
                              sizeof(boolean), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_as_id )) == MCFG_FS_STATUS_OK)
  {
    MSG_HIGH_DS_1(MM_SUB ,"=EMM= 'ISR_EFS_FLAG' is %d",emm_s1_ctrl_data_ptr->isr_efs_flag);
  }
  else
  {
    emm_s1_ctrl_data_ptr->isr_efs_flag = FALSE;
    MSG_HIGH_DS_1(MM_SUB,"=EMM= 'ISR_EFS_FLAG' file could not be read from EFS, status =%d ", efs_status);
  }

  /* Report to the Dog HB */
  dog_hb_report(DOG_MM_RPT);
#if defined(FEATURE_NAS_GW)
  if((efs_status = mcfg_fs_read("/nv/item_files/modem/nas/lte_nas_ignore_mt_csfb_during_volte_call", 
                              &emm_s1_ctrl_data_ptr->ignore_mt_csfb_during_volte_call, 
                              sizeof(boolean), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_as_id )) == MCFG_FS_STATUS_OK)
  {
    MSG_HIGH_DS_1(MM_SUB ,"=EMM= 'lte_nas_ignore_mt_csfb_during_volte_call' is %d",emm_s1_ctrl_data_ptr->ignore_mt_csfb_during_volte_call);
  }
  else
  {
    emm_s1_ctrl_data_ptr->ignore_mt_csfb_during_volte_call = FALSE;
    MSG_HIGH_DS_1(MM_SUB,"=EMM= 'lte_nas_ignore_mt_csfb_during_volte_call' file could not be read from EFS, status = %d", efs_status);
  }
#endif
  /* Report to the Dog HB */
  dog_hb_report(DOG_MM_RPT);
  if((efs_status = mcfg_fs_read("/nv/item_files/modem/nas/emm_combined_proc", 
                              &emm_s1_ctrl_data_ptr->emm_combined_proc_efs_flag, 
                              sizeof(boolean), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_as_id )) == MCFG_FS_STATUS_OK)
  {
    MSG_HIGH_DS_1(MM_SUB ,"=EMM= 'EMM_COMBINED_PROC_EFS_FLAG' is %d",emm_s1_ctrl_data_ptr->emm_combined_proc_efs_flag);
  }
  else
  {
    emm_s1_ctrl_data_ptr->emm_combined_proc_efs_flag = TRUE;
    MSG_HIGH_DS_1(MM_SUB,"=EMM= 'EMM_COMBINED_PROC_EFS_FLAG' file could not be read from EFS, status = %d", efs_status);
  }

  /* Report to the Dog HB */
  dog_hb_report(DOG_MM_RPT);

  if((efs_status = mcfg_fs_read("/nv/item_files/modem/nas/avoid_guti_nas_security_check", 
                              &emm_s1_ctrl_data_ptr->avoid_guti_nas_security_check, 
                              sizeof(boolean), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_as_id )) == MCFG_FS_STATUS_OK)
  {
    MSG_HIGH_DS_1(MM_SUB ,"=EMM= 'AVOID_GUTI_NAS_SECURITY_CHECK' is %d",emm_s1_ctrl_data_ptr->avoid_guti_nas_security_check);
  }
  else
  {
    emm_s1_ctrl_data_ptr->avoid_guti_nas_security_check = FALSE;
    MSG_HIGH_DS_1(MM_SUB,"=EMM= 'AVOID_GUTI_NAS_SECURITY_CHECK' file could not be read from EFS, status = %d", efs_status);
  }

#ifdef TEST_FRAMEWORK
  #error code not present
#endif

  /* GERAN cap */

  /* Report to the Dog HB */
  dog_hb_report(DOG_MM_RPT);
#ifdef FEATURE_NAS_GW
  if((efs_status = mcfg_fs_read("/nv/item_files/modem/nas/geran_cap", 
                              &emm_disable_geran_cap, 
                              sizeof(boolean), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_as_id )) == MCFG_FS_STATUS_OK)
  {
    MSG_HIGH_DS_1(MM_SUB ,"=EMM= GERAN_CAP is %d",emm_disable_geran_cap);
  }
  else
  {
    emm_disable_geran_cap = FALSE;
    MSG_HIGH_DS_1(MM_SUB,"=EMM= GERAN_CAP file could not be read from EFS, status = %d", efs_status);
  }
#ifdef FEATURE_TDSCDMA
  /* Report to the Dog HB */
  dog_hb_report(DOG_MM_RPT);

  /*------------ Check for the TDS NV0 policy flag ------------*/
  if((efs_status = mcfg_fs_read("/nv/item_files/modem/tdscdma/rrc/special_test_setting_enabled", 
                      &nv_special_tdscdma_test_setting_enable, 
                                      sizeof(uint8), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_as_id )) == MCFG_FS_STATUS_OK)
  {
    MSG_HIGH_DS_1(MM_SUB ,"=EMM= TDS RRC NV special_test_setting_enabled val = %d",
                                    nv_special_tdscdma_test_setting_enable);
  }
  else
  {
    MSG_ERROR_DS_1(MM_SUB,"=EMM= Unable to access TDS RRC NV  special_test_setting_enabled, status = %d", efs_status);
    /* making as 2 for the failure case.*/
    nv_special_tdscdma_test_setting_enable = 2;
  }
  
  if (nv_special_tdscdma_test_setting_enable != 2)
  {
    /* Report to the Dog HB */
    dog_hb_report(DOG_MM_RPT);
    /*------------ Check for the TDS ciphering Flag ------------*/
    if((efs_status = mcfg_fs_read("/nv/item_files/modem/tdscdma/rrc/ciphering_enabled", 
                             &emm_s1_ctrl_data_ptr->emm_tds_nv_cipher_enable_flag, 
                             sizeof(boolean), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_as_id )) == MCFG_FS_STATUS_OK)
    {
      MSG_HIGH_DS_1(MM_SUB ,"=EMM= NV TDS_RRC_CIPHERING_ENABLED = %d",
                        emm_s1_ctrl_data_ptr->emm_tds_nv_cipher_enable_flag);
    }
    else
    {
      MSG_ERROR_DS_1(MM_SUB,"=EMM= Unable to access NV to read TDS_RRC_CIPHERING_ENABLED, status = %d", efs_status);
      emm_s1_ctrl_data_ptr->emm_tds_nv_cipher_enable_flag = FALSE;
    }
    
    /* Report to the Dog HB */
    dog_hb_report(DOG_MM_RPT);
    
    /*------------ Check for the TDS INTEGRITY Flag ------------*/
    if((efs_status = mcfg_fs_read("/nv/item_files/modem/tdscdma/rrc/integrity_enabled", 
                              &emm_s1_ctrl_data_ptr->emm_tds_nv_integ_enable_flag, 
                              sizeof(boolean), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_as_id )) == MCFG_FS_STATUS_OK)
    {
      MSG_HIGH_DS_1(MM_SUB ,"=EMM= NV TDS_RRC_INTEGRITY_ENABLED = %d",
                          emm_s1_ctrl_data_ptr->emm_tds_nv_integ_enable_flag);
    }
    else
    {
      MSG_ERROR_DS_1(MM_SUB,"=EMM= Unable to access NV to read TDS_RRC_INTEGRITY_ENABLED, status = %d", efs_status);
      /*By default in the fail case make the  integrity as TRUE as per latest NV0 policy discussion */
      emm_s1_ctrl_data_ptr->emm_tds_nv_integ_enable_flag = TRUE;
    }
    
    /* Report to the Dog HB */
    dog_hb_report(DOG_MM_RPT);
    
    
    /*------------ Check for the TDS Fake Security Flag ------------*/
    if(mcfg_fs_read("/nv/item_files/modem/tdscdma/rrc/fake_sec_enabled", 
                              &emm_s1_ctrl_data_ptr->emm_tds_nv_fake_sec_flag, 
                              sizeof(boolean), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_as_id ) == MCFG_FS_STATUS_OK)
    {
      MSG_HIGH_DS_1(MM_SUB ,"=EMM= NV TDS_RRC_FAKE_SEC_ENABLED = %d",
                             emm_s1_ctrl_data_ptr->emm_tds_nv_fake_sec_flag );
    }
    else
    {
      MSG_ERROR_DS_1(MM_SUB,"=EMM= Unable to access NV to read TDS_RRC_FAKE_SEC_ENABLED, status = %d", efs_status);
      emm_s1_ctrl_data_ptr->emm_tds_nv_fake_sec_flag = FALSE;
    }
    /*If TDS integrity is TRUE irrespective of fake security value make it FALSE  */
    if (emm_s1_ctrl_data_ptr->emm_tds_nv_integ_enable_flag == TRUE )
    {
      MSG_HIGH_DS_0(MM_SUB,"=EMM= TDS RRC integrity Enable Making Fake sec FALSE");
      emm_s1_ctrl_data_ptr->emm_tds_nv_fake_sec_flag = FALSE;
    }
  }
  else 
  {
    /*Its the case when the NV0 policy says 2 value, treat it as fail 
         make cipher = false , fake Security  as FALSE, intergrity as TRUE*/
    emm_s1_ctrl_data_ptr->emm_tds_nv_cipher_enable_flag = FALSE;
    emm_s1_ctrl_data_ptr->emm_tds_nv_integ_enable_flag = TRUE;
    emm_s1_ctrl_data_ptr->emm_tds_nv_fake_sec_flag = FALSE;
    
  }
#endif
#endif

  /* Report to the Dog HB */
  dog_hb_report(DOG_MM_RPT);

  /* set the exclude_identity_type_ie from EFS */
  if((efs_status = mcfg_fs_read("/nv/item_files/modem/nas/exclude_ptmsi_type_field", 
             &emm_s1_ctrl_data_ptr->exclude_identity_type_ie, 
             sizeof(boolean), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_as_id )) == MCFG_FS_STATUS_OK)
  {
    MSG_HIGH_DS_1(MM_SUB ,"=MM= setting exclude_identity_type_ie to %d",emm_s1_ctrl_data_ptr->exclude_identity_type_ie);
  }
  else
  {
    emm_s1_ctrl_data_ptr->exclude_identity_type_ie = FALSE;
    MSG_HIGH_DS_1(MM_SUB,"=MM= exclude_identity_type_ie file could not be read from EFS, status = %d", efs_status);
  }

#ifdef FEATURE_NAS_GW
  dog_hb_report(DOG_MM_RPT);
  /* set the srvcc_capability_indicator from EFS */
  if((efs_status = mcfg_fs_read("/nv/item_files/modem/nas/nas_srvcc_support", 
             &emm_s1_ctrl_data_ptr->srvcc_capability_support, 
             sizeof(boolean), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_as_id )) == MCFG_FS_STATUS_OK)
  {
    MSG_HIGH_DS_1(MM_SUB ,"=MM= Setting srvcc_capability_support to %d",
              emm_s1_ctrl_data_ptr->srvcc_capability_support);
  }
  else
  {
    emm_s1_ctrl_data_ptr->srvcc_capability_support = TRUE;
    MSG_HIGH_DS_2(MM_SUB ,"=MM= srvcc_capability_support file read failed. default %d, status = %d",
                        emm_s1_ctrl_data_ptr->srvcc_capability_support, efs_status);
  }
#endif

#ifdef FEATURE_CIOT
  /* Report to the Dog HB */
  dog_hb_report(DOG_MM_RPT);
  NAS_ENTER_CRIT_SECT(nas_ciot_config_crit_sec);

  if((efs_status = mcfg_fs_read("/nv/item_files/modem/nas/nas_ciot_capability_config", 
                              &emm_s1_ctrl_data_ptr->nas_ciot_capability_config, 
                              sizeof(emm_s1_ctrl_data_ptr->nas_ciot_capability_config), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_as_id )) == MCFG_FS_STATUS_OK)
  {
    MSG_HIGH_DS_1(MM_SUB ,"=EMM= 'CIOT capability' is %d",emm_s1_ctrl_data_ptr->nas_ciot_capability_config);
    if(mm_get_ulog_handle() != NULL)
     {
	  ULOG_RT_PRINTF_2(mm_get_ulog_handle(), "=EMM= Rest CIOT nas_ciot_capability_config = %d, efs_status = %d", 
							 emm_s1_ctrl_data_ptr->nas_ciot_capability_config,
							 efs_status);
     }
  }
  else
  {
    emm_s1_ctrl_data_ptr->nas_ciot_capability_config = NAS_CIOT_DEFAULT_CONFIG;
    MSG_HIGH_DS_1(MM_SUB,"=EMM= 'nas_ciot_capability_config' file could not be read from EFS, status = %x", efs_status);
    if(mm_get_ulog_handle() != NULL)
    {
	  ULOG_RT_PRINTF_2(mm_get_ulog_handle(), "=EMM= 'nas_ciot_capability_config' file could not be read from EFS, nas_ciot_capability_config %d status = %d", 
							 emm_s1_ctrl_data_ptr->nas_ciot_capability_config,
							 efs_status);
    }
  }
  NAS_EXIT_CRIT_SECT(nas_ciot_config_crit_sec);
#endif

#ifdef FEATURE_LTE_TO_1X
  dog_hb_report(DOG_MM_RPT);
  /* set the srvcc_capability_indicator from EFS */
  if((efs_status = mcfg_fs_read("/nv/item_files/modem/nas/l21xsrvcc_support", 
             &emm_s1_ctrl_data_ptr->onexsrvcc_capability_support, 
             sizeof(boolean), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_as_id )) == MCFG_FS_STATUS_OK)
  {
    MSG_HIGH_DS_1(MM_SUB,"=MM= Setting 1x srvcc_capability_support to %d",
              emm_s1_ctrl_data_ptr->onexsrvcc_capability_support);
  }
  else
  {
    emm_s1_ctrl_data_ptr->onexsrvcc_capability_support = FALSE;
    MSG_HIGH_DS_2(MM_SUB,"=MM= 1x srvcc_capability_support file read failed. default %d, status = %d",
                        emm_s1_ctrl_data_ptr->onexsrvcc_capability_support, efs_status);
  }
#endif

#ifdef FEATURE_NAS_GW
  dog_hb_report(DOG_MM_RPT);
  /* set the l2g_srvcc_capability_indicator from EFS */
  if((efs_status = mcfg_fs_read("/nv/item_files/modem/nas/nas_l2g_srvcc_support", 
             &emm_s1_ctrl_data_ptr->l2g_srvcc_capability_support, 
             sizeof(boolean), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_as_id) ) == MCFG_FS_STATUS_OK)
  {
    MSG_HIGH_DS_1(MM_SUB ,"=MM= Setting L2G SRVCC support to %d",
              emm_s1_ctrl_data_ptr->l2g_srvcc_capability_support);
  }
  else
  {
    emm_s1_ctrl_data_ptr->l2g_srvcc_capability_support = TRUE;
    MSG_HIGH_DS_2(MM_SUB ,"=MM= L2G SRVCC support file read failed. default %d, status = %d",
                        emm_s1_ctrl_data_ptr->l2g_srvcc_capability_support, efs_status);
  }

  dog_hb_report(DOG_MM_RPT);
  /* set the srvcc_capability_indicator from EFS */
  if((efs_status = mcfg_fs_read("/nv/item_files/modem/nas/early_call_init", 
              &emm_s1_ctrl_data_ptr->early_call_init, 
              sizeof(boolean), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_as_id) ) == MCFG_FS_STATUS_OK)
  {
    MSG_HIGH_DS_1(MM_SUB ,"=MM= Setting early_call_init to %d",
                      emm_s1_ctrl_data_ptr->early_call_init);
  }
  else
  {
    /* Default TRUE, will take decision later for default value*/
    emm_s1_ctrl_data_ptr->early_call_init = TRUE;
    MSG_HIGH_DS_2(MM_SUB ,"=MM= early_call_init file read failed. default %d, status = %d",
                            emm_s1_ctrl_data_ptr->early_call_init, efs_status);
  } 
  dog_hb_report(DOG_MM_RPT);
#endif
  if((efs_status = mcfg_fs_read("/nvm/alpha/modem/nas/lte_nas_TIN_item", 
                           &emm_s1_ctrl_data_ptr->TIN, 
                           sizeof(nas_mm_tin_type), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_as_id) ) == MCFG_FS_STATUS_OK)
  {
    MSG_HIGH_DS_1(MM_SUB ,"=EMM= Read TIN from EFS = %d", emm_s1_ctrl_data_ptr->TIN);
  }
  else
  {
    /* Default value */
    emm_set_and_write_tin_type(NAS_MM_TIN_NONE);
    MSG_HIGH_DS_2(MM_SUB ,"=EMM= FAILED to read TIN from EFS = %d, status = %d", emm_s1_ctrl_data_ptr->TIN, efs_status);
  }


  if((efs_status = mcfg_fs_read("/nv/item_files/modem/nas/mm_rplmn_type", 
                           &emm_s1_ctrl_data_ptr->rplmn_info.rplmn_type, 
                           sizeof(mm_rplmn_type_enum_T), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_as_id )) == MCFG_FS_STATUS_OK)
  {
    MSG_HIGH_DS_1(MM_SUB ,"=EMM= Read RPLMN TYPE from EFS = %d", emm_s1_ctrl_data_ptr->rplmn_info.rplmn_type);
  }
  else
  {
    /* Default value */
    emm_s1_ctrl_data_ptr->rplmn_info.rplmn_type = MM_RPLMN_TYPE_NONE;
    MSG_HIGH_DS_2(MM_SUB ,"=EMM= FAILED to read RPLMN TYPE from EFS = %d, status = %d", emm_s1_ctrl_data_ptr->rplmn_info.rplmn_type, efs_status);
    mm_psm_update_cache_bitmask(mm_psm_nv_list_bitmask,
                                MM_PSM_EFS_MM_BACKOFF_REMAINING_INFO);
  }
#if defined  FEATURE_LTE && defined FEATURE_3G_USER_NW_REDIR_TO_LTE
  /* IS ACCEPTED ON LTE */
   dog_hb_report(DOG_MM_RPT);
    if((efs_status = mcfg_fs_read("/nv/item_files/modem/nas/is_accepted_on_lte", 
                           &emm_s1_ctrl_data_ptr->is_accepted_on_lte, 
                           sizeof(boolean), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_as_id )) == MCFG_FS_STATUS_OK)
    {
       MSG_HIGH_DS_1(MM_SUB ,"=EMM= Is_accepted_on_lte flag value : %d",
              emm_s1_ctrl_data_ptr->is_accepted_on_lte);
    }
    else
    {
      /* Default value */
      emm_s1_ctrl_data_ptr->is_accepted_on_lte = FALSE;
      MSG_HIGH_DS_2(MM_SUB ,"=EMM= is_accepted_on_lte file could not be read from EFS default %d, status = %d", emm_s1_ctrl_data_ptr->is_accepted_on_lte, efs_status);
    }

#endif

#ifdef FEATURE_DUAL_SIM
  if(mm_sim_max_subscription > 1)
  {
    emm_ctrl_data_type *emm_s2_ctrl_data_ptr = NULL;
    mm_as_id = MM_AS_ID_2;
    mm_sub_id = mm_sub_id_stack[mm_as_id];
    emm_s2_ctrl_data_ptr = emm_db_get_ctrl_data();
    dog_hb_report(DOG_MM_RPT);
    if((efs_status = mcfg_fs_read("/nvm/alpha/modem/nas/lte_nas_TIN_item", 
                           &emm_s2_ctrl_data_ptr->TIN, 
                           sizeof(nas_mm_tin_type), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_as_id )) == MCFG_FS_STATUS_OK)
    {
      MSG_HIGH_DS_1(MM_SUB ,"=EMM= Read TIN from EFS = %d", emm_s2_ctrl_data_ptr->TIN);
    }
    else
    {
      /* Default value */
      emm_set_and_write_tin_type(NAS_MM_TIN_NONE);
      MSG_HIGH_DS_2(MM_SUB ,"=EMM= FAILED to read TIN from EFS = %d, status = %d", emm_s2_ctrl_data_ptr->TIN, efs_status);
    }

    if((efs_status = mcfg_fs_read("/nv/item_files/modem/nas/mm_rplmn_type", 
                           &emm_s2_ctrl_data_ptr->rplmn_info.rplmn_type, 
                           sizeof(mm_rplmn_type_enum_T), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_as_id) ) == MCFG_FS_STATUS_OK)
    {
      MSG_HIGH_DS_1(MM_SUB ,"=EMM= Read RPLMN TYPE from EFS = %d", emm_s2_ctrl_data_ptr->rplmn_info.rplmn_type);
    }
    else
    {
      /* Default value */
      emm_s2_ctrl_data_ptr->rplmn_info.rplmn_type = MM_RPLMN_TYPE_NONE;
      MSG_HIGH_DS_2(MM_SUB ,"=EMM= FAILED to read RPLMN TYPE from EFS = %d, status = %d", emm_s2_ctrl_data_ptr->rplmn_info.rplmn_type, efs_status);
      mm_psm_update_cache_bitmask(mm_psm_nv_list_bitmask,
                                  MM_PSM_EFS_MM_BACKOFF_REMAINING_INFO);
    }
    //Copy the EFS data to other stack which share the same value
#ifdef FEATURE_NAS_GW
    emm_s2_ctrl_data_ptr->srvcc_capability_support = emm_s1_ctrl_data_ptr->srvcc_capability_support;
    emm_s2_ctrl_data_ptr->l2g_srvcc_capability_support = emm_s1_ctrl_data_ptr->l2g_srvcc_capability_support;
#endif
#ifdef FEATURE_CIOT
    NAS_ENTER_CRIT_SECT(nas_ciot_config_crit_sec);

    emm_s2_ctrl_data_ptr->nas_ciot_capability_config = emm_s1_ctrl_data_ptr->nas_ciot_capability_config;
    NAS_EXIT_CRIT_SECT(nas_ciot_config_crit_sec);
#endif
#ifdef FEATURE_LTE_TO_1X
    emm_s2_ctrl_data_ptr->onexsrvcc_capability_support = emm_s1_ctrl_data_ptr->onexsrvcc_capability_support;
#endif
    emm_s2_ctrl_data_ptr->isr_efs_flag = emm_s1_ctrl_data_ptr->isr_efs_flag;
    emm_s2_ctrl_data_ptr->emm_combined_proc_efs_flag = emm_s1_ctrl_data_ptr->emm_combined_proc_efs_flag;
    emm_s2_ctrl_data_ptr->avoid_guti_nas_security_check = emm_s1_ctrl_data_ptr->avoid_guti_nas_security_check;
#ifdef FEATURE_NAS_GW
#ifdef FEATURE_TDSCDMA
    emm_s2_ctrl_data_ptr->emm_tds_nv_cipher_enable_flag = emm_s1_ctrl_data_ptr->emm_tds_nv_cipher_enable_flag;
    emm_s2_ctrl_data_ptr->emm_tds_nv_integ_enable_flag = emm_s1_ctrl_data_ptr->emm_tds_nv_integ_enable_flag;
    emm_s2_ctrl_data_ptr->emm_tds_nv_fake_sec_flag = emm_s1_ctrl_data_ptr->emm_tds_nv_fake_sec_flag;
#endif
#endif
    emm_s2_ctrl_data_ptr->ue_security_capability = emm_s1_ctrl_data_ptr->ue_security_capability;
    emm_s2_ctrl_data_ptr->exclude_identity_type_ie = emm_s1_ctrl_data_ptr->exclude_identity_type_ie;
#ifdef FEATURE_NAS_GW
    emm_s2_ctrl_data_ptr->early_call_init = emm_s1_ctrl_data_ptr->early_call_init;
#endif

  /* 
   * Initialize STRUCT emm_nas_nv to 0.
   * Later in the function, values read from EFS will be stored here.
   */
  memset(&emm_nas_nv, 0x00, sizeof(emm_nas_nv_items_T));
  dog_hb_report(DOG_MM_RPT);
  efs_status = mcfg_fs_read("/nv/item_files/modem/nas/emm_nas_nv_items",
                            &emm_nas_nv,
                            sizeof(emm_nas_nv_items_T),
                            MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)mm_as_id);

  if(efs_status != MCFG_FS_STATUS_OK)
  {
    MSG_HIGH_DS_1(MM_SUB ,"=MM= Unable to read NV emm_nas_nv_items, status : %d ",efs_status);
  }
  else
  {
    if((emm_nas_nv.version >= 3) &&
       (emm_nas_nv.psm_local_detach_enabled == TRUE))
    {
      emm_s2_ctrl_data_ptr->psm_local_detach_enabled = TRUE;
    }
    else
    {
      emm_s2_ctrl_data_ptr->psm_local_detach_enabled = FALSE;
    }
  }
  MSG_HIGH_DS_2(MM_SUB , "=EMM= version of emm_nas_nv = %d  psm_local_detach_enabled %d",
                emm_nas_nv.version,emm_s2_ctrl_data_ptr->psm_local_detach_enabled);

#ifdef FEATURE_NAS_GW
    dog_hb_report(DOG_MM_RPT);
    /* set the srvcc_capability_indicator from EFS */
    if((efs_status = mcfg_fs_read("/nv/item_files/modem/nas/nas_srvcc_support", 
               &emm_s2_ctrl_data_ptr->srvcc_capability_support, 
               sizeof(boolean), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_as_id )) == MCFG_FS_STATUS_OK)
    {
      MSG_HIGH_DS_1(MM_SUB ,"=MM= Setting srvcc_capability_support to %d",
                    emm_s2_ctrl_data_ptr->srvcc_capability_support);
    }
    else
    {
      emm_s2_ctrl_data_ptr->srvcc_capability_support = TRUE;
      MSG_HIGH_DS_2(MM_SUB ,"=MM= srvcc_capability_support file read failed. default %d, status = %d",
                    emm_s2_ctrl_data_ptr->srvcc_capability_support, efs_status);
    }
#endif
    mm_as_id = MM_AS_ID_1;
    mm_sub_id = mm_sub_id_stack[mm_as_id];
  }
#endif
} 
#ifndef FEATURE_NAS_IMS_VOICE_DISABLED
/*===========================================================================

FUNCTION  emm_chk_cs_rat_available

DESCRIPTION: This function checks whether cs rat available

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None
===========================================================================*/
void emm_chk_cs_rat_availability(emm_ctrl_data_type  *emm_ctrl_data_ptr)
{
  uint32              ratMask  = 0;
  sys_band_mask_type cgw_band_capability = 0;
  sys_lte_band_mask_e_type  lte_band_capability;
  sys_band_mask_type tds_band_capability = 0;

  SYS_LTE_BAND_MASK_CLR_ALL_BANDS(&lte_band_capability);
  ASSERT(emm_ctrl_data_ptr != NULL);




  policyman_get_hardware_rats_bands(&ratMask, &cgw_band_capability, &lte_band_capability, &tds_band_capability);

  if(((ratMask & SYS_SYS_MODE_MASK_CDMA) == 0)&&
    ((ratMask & SYS_SYS_MODE_MASK_GSM) == 0)&&
    ((ratMask & SYS_SYS_MODE_MASK_WCDMA) == 0)&&
    ((ratMask & SYS_SYS_MODE_MASK_TDS) == 0))
  {
    MSG_HIGH("CS RAT is not available.",0,0,0);
    emm_ctrl_data_ptr->cs_rat_available = FALSE;
  }
  else
  {
    MSG_HIGH("CS RAT is available. rat mask %d", ratMask,0,0);
    emm_ctrl_data_ptr->cs_rat_available = TRUE;
  }
}
#endif
/*===========================================================================

FUNCTION  EMM_INITIALIZE_CTRL_DATA

DESCRIPTION
  This function initializes emm_ctrl_data structure

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_initialize_ctrl_data
(
  void
)
{
  uint32            count = 0 ;
  emm_ctrl_data_type   *emm_ctrl_data_ptr;
#ifdef FEATURE_DUAL_SIM  
  for(mm_sub_id = MM_AS_ID_1, mm_as_id = MM_AS_ID_1; mm_sub_id < mm_sim_max_subscription; mm_sub_id++, mm_as_id++)
#endif
  {   
    emm_ctrl_data_ptr = emm_db_get_ctrl_data();
    
    memset((void *)emm_ctrl_data_ptr,0x00,sizeof(emm_ctrl_data_type)) ;
#ifdef FEATURE_GSTK
    emm_ctrl_data_ptr->attach_ctrl_timer = 0x00;
#endif
    emm_ctrl_data_ptr->plmn_service_state  = &mm_serving_plmn.info; 
    emm_ctrl_data_ptr->t3346_plmn_ptr	  = &gmm_3gpp_t3346_plmn_id;
    emm_ctrl_data_ptr->emm_mutex_ptr = &emm_mutex;
    emm_ctrl_data_ptr->manual_ftai_list_mutex_ptr = &emm_manual_ftai_list_mutex;
    emm_ctrl_data_ptr->plmn_service_state->sim_state = SYS_SIM_STATE_NOT_AVAILABLE ;
    emm_ctrl_data_ptr->t3402_value = EMM_DEFAULT_T3402; /* 12 minutes default value*/
    emm_ctrl_data_ptr->t3402_attach_rej_param.is_t3402_recv_attach_rej = FALSE;
    memset((void *)&(emm_ctrl_data_ptr->t3402_attach_rej_param.plmn), 
                                 0xFF, sizeof(sys_plmn_id_s_type));
    emm_ctrl_data_ptr->t3412_value = EMM_DEFAULT_T3412;/* 54 minutes default value*/
    emm_ctrl_data_ptr->t3423_value = EMM_DEFAULT_T3412; /* 54 minutes default value. Same as T3412. 
                                                                                      This can be overwriten by attach accept or tau accept*/
#ifdef FEATURE_LTE_REL9
    emm_ctrl_data_ptr->t3442_value = 0;
#endif
    emm_ctrl_data_ptr->eps_bearer_context_status_updated = FALSE;
    emm_ctrl_data_ptr->emm_location_area = &mm_location_information;
#ifdef FEATURE_CIOT
    emm_ctrl_data_ptr->attached_lte_rat = SYS_RAT_NONE;    
    emm_ctrl_data_ptr->loaded_lte_rat = SYS_RAT_NONE;
    emm_ctrl_data_ptr->attach_without_pdn = FALSE;
    emm_ctrl_data_ptr->plmn_service_state->cp_sms_service_status = SYS_CP_SMS_STATUS_NONE;
    emm_ctrl_data_ptr->plmn_service_state->eps_nw_feature_support.er_without_pdn_supported = FALSE; //new
    emm_ctrl_data_ptr->plmn_service_state->eps_nw_feature_support.ce_restriction = FALSE;
	emm_ctrl_data_ptr->plmn_service_state->eps_nw_feature_support.up_ciot_supported = TRUE;
    reg_service_report = (emm_reg_service_report_type)emm_send_reg_ind;
    emm_invalidate_dcn_list(emm_ctrl_data_ptr);
#ifdef FEATURE_LTE_REL14
    emm_ctrl_data_ptr->is_ds_data_pending_while_page = FALSE;
#endif
#endif
    emm_ctrl_data_ptr->force_req_ser_domain = FALSE;

#ifdef FEATURE_NAS_GW
    emm_ctrl_data_ptr->is_srvcc_in_progress = FALSE;
#endif
    emm_ctrl_data_ptr->rrc_deact_req.deact_reason = SYS_STOP_MODE_REASON_NONE;
    emm_ctrl_data_ptr->rrc_deact_req.deact_req_pending = FALSE;
    emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_INVALID; 
    emm_ctrl_data_ptr->last_tau_type = TAU_CAUSE_INVALID;
    emm_ctrl_data_ptr->detach_reason = INVALID_DETACH;
#ifdef FEATURE_NAS_GW
    emm_ctrl_data_ptr->emm_umts_nv_cipher_enable_flag = FALSE;
    emm_ctrl_data_ptr->emm_umts_nv_integ_enable_flag = FALSE;
    emm_ctrl_data_ptr->emm_nv_fake_sec_flag = FALSE;
#ifdef FEATURE_TDSCDMA
    emm_ctrl_data_ptr->emm_tds_nv_cipher_enable_flag = FALSE;
    emm_ctrl_data_ptr->emm_tds_nv_integ_enable_flag	= FALSE;
    emm_ctrl_data_ptr->emm_tds_nv_fake_sec_flag		= FALSE;
#endif
#endif

    emm_ctrl_data_ptr->update_sim_params = TRUE;
    emm_ctrl_data_ptr->avoid_guti_nas_security_check = FALSE;
    emm_ctrl_data_ptr->isr_efs_flag = FALSE;
    emm_ctrl_data_ptr->emm_combined_proc_efs_flag = TRUE;
    emm_ctrl_data_ptr->last_incoming_sqn = 0;
    emm_ctrl_data_ptr->w2l_psho = FALSE;
#ifdef FEATURE_NAS_GW
    emm_ctrl_data_ptr->l2gcco_state = EMM_LTOG_CCO_INACTIVE;
#endif
#ifdef DISABLE_9x05_OPTIMIZATIONS
    emm_ctrl_data_ptr->set_drx_req_pending_ptr = &gmm_set_drx_req_pending;
#endif
#ifdef FEATURE_NAS_GW
    //*emm_ctrl_data_ptr->set_drx_req_pending_ptr = FALSE;
    emm_ctrl_data_ptr->TIN = NAS_MM_TIN_NONE;
#endif


    emm_ctrl_data_ptr->reg_lte_pended_cs_domain_param_ptr = &gmm_reg_pended_lte_cs_param;


    /* pended nw_sel_mode is checked for sglte. initialize it */
    if(emm_ctrl_data_ptr->reg_req_pending_ptr != NULL)
    {
     emm_ctrl_data_ptr->reg_req_pending_ptr->network_selection_mode
                                    = SYS_NETWORK_SELECTION_MODE_NONE;
    }
    
#if defined(FEATURE_LTE_REL9) && !defined(FEATURE_NAS_IMS_VOICE_DISABLED)
    emm_ctrl_data_ptr->emc_srv_status = LTE_NAS_EMC_SRV_NONE;
    emm_ctrl_data_ptr->non_emc_bearer_deact_in_progress = FALSE;
#endif
  
#ifdef FEATURE_LTE_REL9 
#ifndef TEST_FRAMEWORK
    emm_ctrl_data_ptr->emm_3gpp_spec_ver = LTE_3GPP_DEFAULT_RELEASE_VERSION; /*Initialize to some default value*/
#else
    #error code not present
#endif
#endif
  
    emm_ctrl_data_ptr->combined_attach_cs_rej_cause = LTE_NAS_CAUSE_NONE;
    emm_ctrl_data_ptr->emm_sms_active = FALSE;
  
    /* Init FPLMN list for combined registration */
    emm_ctrl_data_ptr->combined_reg_fplmn_list.length_of_plmn_lst_contents = 0;
    memset((void *)emm_ctrl_data_ptr->combined_reg_fplmn_list.plmn, 
                    0xFF, sizeof(sys_plmn_id_s_type) * MAX_NO_OF_PLMNs);
  
    /* Flag to indicate whether USIM supports the sotrage of EPS into or not */
    emm_ctrl_data_ptr->eps_storage_supported = FALSE;
  
    emm_ctrl_data_ptr->temp_fplmn_backoff_time = EMM_TEMP_FPLMN_DISABLED;
  
    emm_ctrl_data_ptr->mt_detach_info.type = INVALID_MT_DETACH_TYPE;
    emm_ctrl_data_ptr->mt_detach_info.emm_cause = LTE_NAS_NO_FAILURE;
  
    emm_ctrl_data_ptr->emm_rat_drx_param_to_be_sent = NAS_SEND_DRX_NONE;
  
    emm_ctrl_data_ptr->last_reg_cause = MMR_CAUSE_NONE;
  
#if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
    emm_ctrl_data_ptr->operator_csg_list = &mm_sim_active_operator_csg_list;
    emm_ctrl_data_ptr->allowed_csg_list = &mm_sim_active_allowed_csg_list;
    emm_ctrl_data_ptr->nv_csg_support = &mm_nv_csg_support;
#endif 
#if defined(TEST_FRAMEWORK)
    #error code not present
#endif // TEST_FRAMEWORK
  
    /* Init cached PDN_CONNECTIVITY_REQ message and corresponding ESM trans ID */
    emm_ctrl_data_ptr->pdn_conn_req_ptr = NULL;

    emm_ctrl_data_ptr->out_msg_ptr = NULL;
  
    emm_ctrl_data_ptr->emm_ota_message_ptr  = NULL;
  
    emm_ctrl_data_ptr->nas_incoming_msg_ptr = NULL;
  
    emm_ctrl_data_ptr->pdn_conn_req_trans_id = EMM_INVALID_PDN_CONN_REQ_TRANS_ID;
  
    emm_ctrl_data_ptr->attach_pdn_seq_num = PDN_CONN_REQ_SEQ_NUM_UNASSIGNED;
  
    emm_ctrl_data_ptr->esm_srv_req_trans_id = EMM_INVALID_ESM_SRV_REQ_TRANS_ID;
  
    memset(&emm_ctrl_data_ptr->last_rrc_deactivate_cnf,0,sizeof(lte_rrc_deactivate_cnf_s));
  
    emm_ctrl_data_ptr->mm_send_mmr_stop_mode_cnf_fp = mm_send_mmr_stop_mode_cnf;
  
#ifdef FEATURE_LTE_REL9 
    emm_ctrl_data_ptr->emm_ota_message_ptr_flipped = FALSE;
#endif
    memset((void *)&emm_ctrl_data_ptr->last_rrc_bearer_ind, 0x0, 
         sizeof(lte_rrc_active_eps_bearer_update_ind_s));
  
    emm_ctrl_data_ptr->sync_drb_status = FALSE;
  
    emm_ctrl_data_ptr->aggression_management = FALSE;
    emm_ctrl_data_ptr->plmn_service_state->extend_srv_info = 
                                                      SYS_EXTEND_SRV_INFO_NONE;
    emm_ctrl_data_ptr->plmn_service_state->emc_attached = FALSE;
#if defined  FEATURE_LTE && defined FEATURE_3G_USER_NW_REDIR_TO_LTE
    emm_ctrl_data_ptr->is_accepted_on_lte = FALSE;
#endif  
#ifdef FEATURE_NAS_GW
    emm_ctrl_data_ptr->csfb_mt_call_info_ptr = &mm_csfb_mt_call_info;
    emm_ctrl_data_ptr->csfb_mt_call_info_ptr->mt_csfb_highPriority = FALSE;
    emm_ctrl_data_ptr->csfb_mt_call_info_ptr->is_mt_call_recv_in_conn_state = FALSE;
#endif
    /* Set t311_rem_time = 0 & no_svc_cause = SYS_NO_SVC_CAUSE_NORMAL
     to prevent Service request to be sent in connected state during
     w2l handover*/
    if(emm_ctrl_data_ptr->last_rrc_service_ind_ptr != NULL)
    {
      emm_ctrl_data_ptr->last_rrc_service_ind_ptr->t311_rem_time = 0;
      emm_ctrl_data_ptr->last_rrc_service_ind_ptr->no_svc_cause = 
                                                  SYS_NO_SVC_CAUSE_NORMAL;
#ifndef FEATURE_NAS_IMS_VOICE_DISABLED
      emm_ctrl_data_ptr->skip_acb_for_volte_call = FALSE;
#endif
    }
    emm_ctrl_data_ptr->reg_complete_ack_pending = EMM_PENDING_REG_ACK_NONE;
    emm_ctrl_data_ptr->is_plmn_block_req_pending = FALSE;
#ifdef FEATURE_DUAL_SIM
    emm_ctrl_data_ptr->lte_trm_priority = LTE_TRM_PRIORITY_LOW;
#endif
#ifdef FEATURE_LTE_REL9
#if !defined FEATURE_NAS_IMS_VOICE_DISABLED || !defined FEATURE_NAS_DISABLE_VOICE
#ifdef FEATURE_NAS_GW
    emm_ctrl_data_ptr->mmit_parameters.utran_ps_voice_parameters_ptr = &mm_utran_ps_voice_parameters;
#endif
    emm_ctrl_data_ptr->mmit_parameters.ims_registration_parameters.is_ims_reg_for_voice_tag = FALSE;
    emm_ctrl_data_ptr->mmit_parameters.eutran_ps_voice_parameters.is_ps_voice_on_eutran_supported = FALSE;
    emm_ctrl_data_ptr->mmit_parameters.eutran_ps_voice_parameters.is_last_eutran_voims_supported = FALSE;
#endif
#endif

    sys_plmn_undefine_plmn_id(&(emm_ctrl_data_ptr->rplmn_info.rplmn));
    emm_ctrl_data_ptr->emm_lte_mode_change_pending = FALSE;
#ifdef FEATURE_NAS_GW
    emm_ctrl_data_ptr->ps_resumption_status = EMM_PS_RESUMPTION_NONE;
    emm_ctrl_data_ptr->rr_blind_redir_pending_call_abort = FALSE;
#endif
  
    for(count = MIN_EMM_TIMER; count < MAX_EMM_TIMER; count++)
    {
      mm_timer_status[count] = TIMER_STOPPED;
    }
#ifndef FEATURE_NAS_IMS_VOICE_DISABLED
    rex_def_timer_ex(&emm_ctrl_data_ptr->emm_access_barring_emergency_timer, (rex_timer_cb_type)emm_barring_call_back, (dword)EMM_EMERGENCY_BARRED) ;
#endif
    rex_def_timer_ex(&emm_ctrl_data_ptr->emm_access_barring_mt_access_timer, (rex_timer_cb_type)emm_barring_call_back, (dword)EMM_MT_ACCESS_BARRED) ;
  
    rex_def_timer_ex(&emm_ctrl_data_ptr->emm_access_barring_mo_signaling_timer, (rex_timer_cb_type)emm_barring_call_back, (dword)EMM_MO_SIGNALING_BARRED) ;
  
    rex_def_timer_ex(&emm_ctrl_data_ptr->emm_access_barring_mo_data_timer, (rex_timer_cb_type)emm_barring_call_back, (dword)EMM_MO_DATA_BARRED) ;

#ifdef FEATURE_LTE_REL10
#ifdef FEATURE_NAS_GW
    rex_def_timer_ex(&emm_ctrl_data_ptr->emm_access_barring_mo_csfb_timer, (rex_timer_cb_type)emm_barring_call_back, (dword)EMM_MO_CSFB_BARRED);
#endif
    rex_def_timer_ex(&emm_ctrl_data_ptr->emm_access_barring_delay_tolerant_timer, (rex_timer_cb_type)emm_barring_call_back, (dword)EMM_DELAY_TOLERANT_BARRED);

#endif
#ifdef FEATURE_CIOT
    rex_def_timer_ex(&emm_ctrl_data_ptr->emm_access_barring_mo_excep_data_timer, (rex_timer_cb_type)emm_barring_call_back, (dword)EMM_MO_EXCEPTION_BARRED);
#endif
#ifdef FEATURE_NAS_GW
    emm_ctrl_data_ptr->ignore_GW_activation_ind = FALSE;
#endif
#if defined(FEATURE_LTE_TO_1X) || defined(FEATURE_NAS_GW)
    emm_ctrl_data_ptr->esr_type_in_progress = NAS_EMM_CSFB_NONE;
    emm_ctrl_data_ptr->pended_esr_msg = NULL;
#endif

    emm_ctrl_data_ptr->plmn_service_state->lte_cs_capability = SYS_LTE_CS_CAPABILITY_NONE;
#ifdef FEATURE_NAS_GW
    emm_ctrl_data_ptr->is_sglte_device  = FALSE;
#endif
#ifdef FEATURE_DUAL_SIM
    emm_ctrl_data_ptr->imsi_not_native = FALSE;
    emm_ctrl_data_ptr->tau_trm_priority = FALSE;
#endif
    emm_ctrl_data_ptr->conn_mode_manual_search_ptr = &mm_conn_mode_manual_search_enabled;
#ifdef FEATURE_LTE_REL12
		emm_ctrl_data_ptr->service_req_back_off_timer_value = mm_emm_service_req_backoff_timer_value;
#endif

    emm_ctrl_data_ptr->access_class = LTE_RRC_INVALID_ACCESS_CLASS;
#ifdef FEATURE_DUAL_SIM
    emm_ctrl_data_ptr->dds_switch_pended_ptr  = &gmm_force_local_detach_sim[mm_sub_id];
#endif
#ifdef FEATURE_LTE_REL10
    emm_ctrl_data_ptr->delay_tolerant_access = FALSE;
    emm_ctrl_data_ptr->responding_to_page = FALSE;
    emm_ctrl_data_ptr->extended_wait_time = 0;
#endif
#ifdef FEATURE_CIOT
	 emm_ctrl_data_ptr->emm_start_mo_excp_signalling = FALSE;
    emm_ctrl_data_ptr->las_nas_ul_count = 0x0000;
#endif
    emm_ctrl_data_ptr->rpm_ftai_list_ptr = NULL;
    emm_ctrl_data_ptr->is_sim_cs_invalid_due_to_dos = &mm_is_sim_cs_invalid_due_to_dos;
    emm_ctrl_data_ptr->is_sim_ps_invalid_due_to_dos = &mm_is_sim_ps_invalid_due_to_dos;
    emm_ctrl_data_ptr->is_sim_to_be_cs_invalid_after_power_off = &mm_is_sim_to_be_cs_invalid_after_power_off;
    emm_ctrl_data_ptr->is_sim_to_be_ps_invalid_after_power_off = &mm_is_sim_to_be_ps_invalid_after_power_off;

#ifdef FEATURE_NAS_EAB
    emm_ctrl_data_ptr->eab_barring = FALSE;
    emm_ctrl_data_ptr->eab_barring_flag = FALSE;
#endif
    emm_ctrl_data_ptr->psm_local_detach_enabled = FALSE;
    emm_ctrl_data_ptr->t3324_value = NAS_INVALID_PSM_TIMER_VALUE;
    emm_ctrl_data_ptr->t3324_value_to_nw = NAS_INVALID_PSM_TIMER_VALUE;
    emm_ctrl_data_ptr->t3324_expired_ptr  = &gmm_psm_timer_ctxt.t3324_expired;
    *emm_ctrl_data_ptr->t3324_expired_ptr = FALSE;
    emm_ctrl_data_ptr->t3412_ext_value_to_nw = NAS_INVALID_PSM_TIMER_VALUE;
    emm_ctrl_data_ptr->psm_set_cfg_status_ptr = &gmm_psm_timer_ctxt.psm_set_cfg_req_status;
    *emm_ctrl_data_ptr->psm_set_cfg_status_ptr = NAS_PSM_CFG_REQ_NONE;
    emm_ctrl_data_ptr->psm_active = FALSE;
    emm_ctrl_data_ptr->psm_restore_done = FALSE;
    emm_ctrl_data_ptr->edrx_params.enabled = FALSE;
    emm_ctrl_data_ptr->edrx_params.ptw = NAS_INVALID_EDRX_VALUE;
    emm_ctrl_data_ptr->edrx_params.cycle_length = NAS_INVALID_EDRX_VALUE;
    emm_ctrl_data_ptr->edrx_params_to_nw.enabled = FALSE;
    emm_ctrl_data_ptr->edrx_params_to_nw.ptw = NAS_INVALID_EDRX_VALUE;
    emm_ctrl_data_ptr->edrx_params_to_nw.cycle_length = NAS_INVALID_EDRX_VALUE;
    emm_ctrl_data_ptr->edrx_set_cfg_req_status = NAS_EDRX_CFG_REQ_NONE;
    emm_ctrl_data_ptr->allow_psm_in_oos_ltd_srv_ptr = &gmm_psm_timer_ctxt.allow_psm_in_oos_ltd_srv;
    *emm_ctrl_data_ptr->allow_psm_in_oos_ltd_srv_ptr = FALSE;
#ifdef FEATURE_CIOT	
	emm_ctrl_data_ptr->is_conn_suspended = FALSE;
	emm_ctrl_data_ptr->send_on_resume = FALSE;
	emm_ctrl_data_ptr->esm_data_pending_after_res = FALSE;
	emm_ctrl_data_ptr->drb_pending_post_resume = FALSE;
    emm_ctrl_data_ptr->include_dcn = FALSE;
    emm_ctrl_data_ptr->ciot_irat_tau_mobility = FALSE;
    emm_ctrl_data_ptr->rach_retry_counter = 0;
    emm_ctrl_data_ptr->mt_page_rach_retry_needed = FALSE;
	emm_ctrl_data_ptr->conn_suspended_rat = SYS_RAT_NONE;
#endif
    emm_ctrl_data_ptr->curr_smc_status = LTE_SMC_STATUS_NONE;
    emm_ctrl_data_ptr->reset_nas_ul_cnt_rel15 = FALSE;
  }
#ifdef FEATURE_DUAL_SIM
  mm_sub_id = MM_AS_ID_1;
  mm_as_id = MM_AS_ID_1;
#endif
 emm_ctrl_data_ptr->emm_ignore_tau_req = FALSE;
 emm_ctrl_data_ptr->is_periodic_tau_required = FALSE;
 #ifdef FEATURE_FMD_SPEED_INFO
 emm_ctrl_data_ptr->fmd_force_local_detach = FALSE;
 emm_ctrl_data_ptr->fmd_psm_fallback_timer = 0;
 emm_ctrl_data_ptr->fmd_psm_fallback_timer_expired = FALSE;
#endif
}



/*===========================================================================

FUNCTION  EMM_INITIALIZE

DESCRIPTION
  This function initializes emm related data

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_initialize
(
  msgr_client_t        *esm_client,
  msgr_client_t        *rrc_client,
  msgr_client_t        *tlb_client
#ifdef FEATURE_LTE_TO_1X
  ,msgr_client_t        *emm_csfb_client
#endif
#if defined (FEATURE_LTE_REL9)
  ,
  msgr_client_t        *emm_gps_client
#endif /* FEATURE_LTE_REL9 */
#ifdef FEATURE_CIOT
  ,msgr_client_t        *emm_phy_client
#endif
)
{
  q_type          *q_ptr;
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;


  emm_initialize_ctrl_data();

  mm_as_id = MM_AS_ID_1;
  mm_sub_id = MM_AS_ID_1;

  emm_ctrl_data_ptr = emm_db_get_ctrl_data();
  emm_ctrl_data_mem_alloc(emm_ctrl_data_ptr);

  lte_nas_efs_read();

  /* ESM input Qs */

  q_ptr = q_init(&esm_esm_cmd_q_msg);

  mm_check_for_null_ptr((void *) q_ptr);



  q_ptr = q_init(&tlb_tlb_cmd_q_msg);

  mm_check_for_null_ptr((void *) q_ptr );



  q_ptr = q_init(&rrc_emm_cmd_q_msg);

  mm_check_for_null_ptr((void *) q_ptr );

#ifdef FEATURE_LTE_TO_1X
  q_init(&emm_csfb_cmd_q_msg);
#endif

#if defined (FEATURE_LTE_REL9)
  /* IRAT input Qs */

  q_init(&emm_gps_cmd_q_msg);

#endif

#ifdef FEATURE_CIOT
  q_init(&emm_phy_cmd_q_msg);
#endif

  emm_init_msglib_decode_func_ptr(decode_nas_msg);
  emm_init_msglib_encode_func_ptr(encode_nas_msg);
  emm_init_msglib_retrieve_func_ptr(nas_retrieve_msg);

  //*mm_emm_dispatcher_fn_ptr = mm_emm_normal_msg_handler;

  /* Register with message router */
   lte_nas_msgr_client_create(esm_client);
   lte_nas_msgr_client_set_rex_q(esm_client,mm_tcb_ptr,
                                   ESM_EMM_CMD_Q_SIG,&esm_esm_cmd_q_msg,
                                   MSGR_NO_QUEUE,NAS_STD_OFFSETOF( esm_cmd_type, cmd ),
                                   MSGR_NO_OFFSET,(uint16)sizeof(emm_cmd_type));

   lte_nas_msgr_client_create(tlb_client);
   lte_nas_msgr_client_set_rex_q(tlb_client,mm_tcb_ptr,
                                   TLB_EMM_CMD_Q_SIG,&tlb_tlb_cmd_q_msg,
                                   MSGR_NO_QUEUE,NAS_STD_OFFSETOF( tlb_cmd_type, cmd ),
                                   MSGR_NO_OFFSET,(uint16)sizeof(tlb_cmd_type));

   lte_nas_msgr_client_create(rrc_client);
   lte_nas_msgr_client_set_rex_q(rrc_client,mm_tcb_ptr,
                                   RRC_EMM_CMD_Q_SIG,&rrc_emm_cmd_q_msg,
                                   MSGR_NO_QUEUE,NAS_STD_OFFSETOF( emm_rrc_cmd_type, rrc_msgr_cmd ),
                                   MSGR_NO_OFFSET,(uint16)sizeof(emm_rrc_cmd_type));
#ifdef FEATURE_LTE_TO_1X
   lte_nas_msgr_client_create(emm_csfb_client);
   lte_nas_msgr_client_set_rex_q(emm_csfb_client,
                                   mm_tcb_ptr,
                                   CSFB_EMM_CMD_Q_SIG,
                                   &emm_csfb_cmd_q_msg,
                                   MSGR_NO_QUEUE,
                                   NAS_STD_OFFSETOF(emm_csfb_cmd_type, cmd),
                                                   MSGR_NO_OFFSET,
                                   (uint16)sizeof(emm_csfb_cmd_type));
#endif

#if defined (FEATURE_LTE_REL9)
#ifdef FEATURE_NAS_GPS_ENABLE
   lte_nas_msgr_client_create(emm_gps_client);
   lte_nas_msgr_client_set_rex_q(emm_gps_client,mm_tcb_ptr,
                                   GPS_EMM_CMD_Q_SIG,&emm_gps_cmd_q_msg,
                                   MSGR_NO_QUEUE,NAS_STD_OFFSETOF( emm_gps_cmd_type, gps_msgr_cmd ),
                                   MSGR_NO_OFFSET,(uint16)sizeof(emm_gps_cmd_type));
#endif
#endif /*FEATURE_LTE_REL9 */
   
#ifdef FEATURE_CIOT
   lte_nas_msgr_client_create(emm_phy_client);
   lte_nas_msgr_client_set_rex_q(emm_phy_client,mm_tcb_ptr,
                                   PHY_EMM_CMD_Q_SIG,&emm_phy_cmd_q_msg,
                                   MSGR_NO_QUEUE,NAS_STD_OFFSETOF(emm_phy_cmd_s, cmd),
                                   MSGR_NO_OFFSET,(uint16)sizeof(emm_phy_cmd_s));   
   lte_nas_msgr_register(MSGR_NAS_EMM,emm_phy_client,MSGR_ID_NONE,NB1_CPHY_EDRX_UIM_PAGE_IND);
   /* Register for M1 messages*/
   lte_nas_msgr_register(MSGR_NAS_EMM,emm_phy_client,MSGR_ID_NONE,LTE_CPHY_EDRX_UIM_PAGE_IND);
#endif
   

   lte_nas_msgr_register(MSGR_NAS_EMM,tlb_client,MSGR_ID_NONE,LTE_TLB_UL_INFO_REQ);
   lte_nas_msgr_register(MSGR_NAS_EMM,esm_client,MSGR_ID_NONE,NAS_EMM_SERVICE_REQ);
   lte_nas_msgr_register(MSGR_NAS_EMM,esm_client,MSGR_ID_NONE,NAS_EMM_DATA_REQ);
   lte_nas_msgr_register(MSGR_NAS_EMM,esm_client,MSGR_ID_NONE,NAS_EMM_DETACH_CMD);
   lte_nas_msgr_register(MSGR_NAS_EMM,esm_client,MSGR_ID_NONE,NAS_EMM_EPS_BEARER_STATUS_CMD) ;
#if defined(FEATURE_LTE_REL9) && !defined(FEATURE_NAS_IMS_VOICE_DISABLED)
   lte_nas_msgr_register(MSGR_NAS_EMM,esm_client,MSGR_ID_NONE,NAS_EMM_EMC_SRV_STATUS_CMD) ;
#endif
   lte_nas_msgr_register(MSGR_NAS_EMM,rrc_client,MSGR_ID_NONE,LTE_RRC_CONN_EST_CNF) ;
   lte_nas_msgr_register(MSGR_NAS_EMM,rrc_client,MSGR_ID_NONE,LTE_RRC_UL_DATA_CNF) ;
   lte_nas_msgr_register(MSGR_NAS_EMM,rrc_client,MSGR_ID_NONE,LTE_RRC_DEACTIVATE_CNF) ;
   lte_nas_msgr_register(MSGR_NAS_EMM,rrc_client,MSGR_ID_NONE,LTE_RRC_ACTIVATION_IND) ;
   lte_nas_msgr_register(MSGR_NAS_EMM,rrc_client,MSGR_ID_NONE,LTE_RRC_SERVICE_IND) ;
   lte_nas_msgr_register(MSGR_NAS_EMM,rrc_client,MSGR_ID_NONE,LTE_RRC_BARRING_UPDATE_IND) ;
   lte_nas_msgr_register(MSGR_NAS_EMM,rrc_client,MSGR_ID_NONE,LTE_RRC_CONN_REL_IND) ;
   lte_nas_msgr_register(MSGR_NAS_EMM,rrc_client,MSGR_ID_NONE,LTE_RRC_DL_DATA_IND) ;
   lte_nas_msgr_register(MSGR_NAS_EMM,rrc_client,MSGR_ID_NONE,LTE_RRC_PAGE_IND) ;
   lte_nas_msgr_register(MSGR_NAS_EMM,rrc_client,MSGR_ID_NONE,LTE_RRC_ACTIVE_EPS_BEARER_UPDATE_IND) ;
   lte_nas_msgr_register(MSGR_NAS_EMM,rrc_client,MSGR_ID_NONE,LTE_RRC_PLMN_SEARCH_CNF);
#ifdef FEATURE_NAS_GW
   lte_nas_msgr_register(MSGR_NAS_EMM,rrc_client,MSGR_ID_NONE,LTE_RRC_NAS_UMTS_KEY_IND);
   lte_nas_msgr_register(MSGR_NAS_EMM,rrc_client,MSGR_ID_NONE,LTE_RRC_NAS_GSM_KEY_IND);
#endif
   lte_nas_msgr_register(MSGR_NAS_EMM,rrc_client,MSGR_ID_NONE,LTE_RRC_NAS_LTE_KEY_IND);
#ifdef FEATURE_LTE_TO_1X
   lte_nas_msgr_register(MSGR_NAS_EMM,rrc_client,MSGR_ID_NONE,LTE_RRC_IRAT_TUNNEL_UL_MSG_CNF);
   lte_nas_msgr_register(MSGR_NAS_EMM,rrc_client,MSGR_ID_NONE,LTE_RRC_IRAT_TUNNEL_DL_MSG_IND);
#endif
#ifdef FEATURE_NAS_GW
   lte_nas_msgr_register(MSGR_NAS_EMM,rrc_client,MSGR_ID_NONE,LTE_RRC_CCO_NACC_COMPLETED_IND);
#endif
   lte_nas_msgr_register(MSGR_NAS_EMM,rrc_client,MSGR_ID_NONE,LTE_RRC_CSG_INFO_UPDATE_IND);
#ifdef FEATURE_LTE_TO_1X
   lte_nas_msgr_register(MSGR_NAS_EMM,emm_csfb_client,MSGR_ID_NONE,NAS_EMM_IRAT_UL_MSG_REQ);
#ifdef FEATURE_LTE_TO_HDR_OH  
  /* UL and DL messages for LTE - DO interface */
   lte_nas_msgr_register(MSGR_NAS_EMM,rrc_client,MSGR_ID_NONE,LTE_RRC_IRAT_HDR_TUNNEL_UL_MSG_CNF);
   lte_nas_msgr_register(MSGR_NAS_EMM,rrc_client,MSGR_ID_NONE,LTE_RRC_IRAT_HDR_TUNNEL_DL_MSG_IND);
   lte_nas_msgr_register(MSGR_NAS_EMM,emm_csfb_client,MSGR_ID_NONE,NAS_EMM_IRAT_HDR_UL_MSG_REQ);
#endif
   lte_nas_msgr_register(MSGR_NAS_EMM,esm_client,MSGR_ID_NONE,NAS_EMM_1XCSFB_ESR_CALL_REQ);
   lte_nas_msgr_register(MSGR_NAS_EMM,esm_client,MSGR_ID_NONE,NAS_EMM_1XCSFB_ESR_CALL_ABORT_REQ);
   lte_nas_msgr_register(MSGR_NAS_EMM,rrc_client,MSGR_ID_NONE,LTE_RRC_1XCSFB_HO_STARTED_IND);
   lte_nas_msgr_register(MSGR_NAS_EMM,rrc_client,MSGR_ID_NONE,LTE_RRC_1XCSFB_HO_FAILED_IND);
   lte_nas_msgr_register(MSGR_NAS_EMM,rrc_client,MSGR_ID_NONE,LTE_RRC_1XSRVCC_HO_STARTED_IND);
   lte_nas_msgr_register(MSGR_NAS_EMM,rrc_client,MSGR_ID_NONE,LTE_RRC_1XSRVCC_HO_FAILED_IND);
#endif
#if defined (FEATURE_LTE_REL9)
#ifdef FEATURE_NAS_GPS_ENABLE
   lte_nas_msgr_register(MSGR_NAS_EMM,emm_gps_client,MSGR_ID_NONE,NAS_EMM_UL_GENERIC_NAS_TRANSPORT_REQ);
#endif
#endif /*FEATURE_LTE_REL9 */
   lte_nas_msgr_register(MSGR_NAS_EMM,rrc_client,MSGR_ID_NONE,LTE_RRC_SIB6_INFO_IND);
   lte_nas_msgr_register(MSGR_NAS_EMM,rrc_client,MSGR_ID_NONE,LTE_RRC_SIB7_INFO_IND);
#ifdef FEATURE_NAS_EAB
   lte_nas_msgr_register(MSGR_NAS_EMM,rrc_client,MSGR_ID_NONE,LTE_RRC_EAB_UPDATE_IND);
#endif
  lte_nas_msgr_register(MSGR_NAS_EMM,esm_client,MSGR_ID_NONE,NAS_EMM_RESTORE_CTXT_PSM_RSP);
  lte_nas_msgr_register(MSGR_NAS_EMM,esm_client,MSGR_ID_NONE,NAS_EMM_SAVE_CTXT_PSM_RSP);

#ifdef FEATURE_CIOT
   lte_nas_msgr_register(MSGR_NAS_EMM,rrc_client,MSGR_ID_NONE,LTE_RRC_AB_UPDATE_IND);
#endif
  /*Notify T3402 clients of the T3402 value*/
  emm_send_t3402_changed_ind(emm_ctrl_data_ptr->t3402_value);
#ifndef FEATURE_NAS_IMS_VOICE_DISABLED
  emm_chk_cs_rat_availability(emm_ctrl_data_ptr);
#endif
} /* end of emm_initialize() */

/*===========================================================================

FUNCTION  EMM_PROCESS_TIMER_EXPIRY

DESCRIPTION
  This function processes timer expiry and calls corresponding handler

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_process_timer_expiry
(
  mm_cmd_type         *cmd_ptr, 
  emm_ctrl_data_type  *emm_ctrl_data_ptr
)
{
  log_lte_nas_emm_timer_event_type event;

  

  gs_clear_timer_ext( 
                      get_mm_timer_queue(),
                      cmd_ptr->cmd.timer_expiry.timer_id
                    );

  if (((mm_timer_status[cmd_ptr->cmd.timer_expiry.timer_id] == TIMER_ACTIVE) &&
       ((uint64) gs_enquire_timer_ext (
         get_mm_timer_queue(),
         cmd_ptr->cmd.timer_expiry.timer_id) == 0))
      ||
      (cmd_ptr->cmd.timer_expiry.timer_id == EMM_ACCESS_BARRING_TIMER))
  {
    if(((cmd_ptr->cmd.timer_expiry.timer_id >= MIN_EMM_TIMER) &&
       (cmd_ptr->cmd.timer_expiry.timer_id <= MAX_EMM_TIMER)) ||
       (cmd_ptr->cmd.timer_expiry.timer_id == TIMER_T3346) ||
       (cmd_ptr->cmd.timer_expiry.timer_id == TIMER_T3324) ||
       (cmd_ptr->cmd.timer_expiry.timer_id == TIMER_T3325))
    {
      mm_timer_status[cmd_ptr->cmd.timer_expiry.timer_id] = TIMER_STOPPED;
    }
    MSG_HIGH_DS_1(MM_SUB,"=EMM= TIMER = %d has expired", cmd_ptr->cmd.timer_expiry.timer_id);
    /* ---------------------------------------------
     ** Send timer message off to appropriate handler
     ** --------------------------------------------- */
    switch (cmd_ptr->cmd.timer_expiry.timer_id)
    {
      case TIMER_T3410:
        emm_timer_t3410_expire(emm_ctrl_data_ptr);
        break;
  
      case TIMER_T3402:
        emm_attach_timer_t3402_t3411_expire(emm_ctrl_data_ptr,
                                            cmd_ptr->cmd.timer_expiry.timer_id);
        break;
  
      case TIMER_T3411:
        emm_attach_timer_t3402_t3411_expire(emm_ctrl_data_ptr,
                                            cmd_ptr->cmd.timer_expiry.timer_id);
        break;
  
      case EMM_POWEROFF_DETACH_TIMER:
        emm_powerdown_timer_expire(emm_ctrl_data_ptr);
        break;
  
      case TIMER_T3421:
        emm_t3421_expire(emm_ctrl_data_ptr);
        break;
  
      case TIMER_T3417:
        emm_timer_t3417_expire(emm_ctrl_data_ptr) ;
        break;
  
      case TIMER_T3418:
      case TIMER_T3420:
        auth_handle_timer_expiry(cmd_ptr);
      break;
  
      case TIMER_T3416: 
        /* cache guard timer */
        auth_reset_cache_memory(TRUE,
                                cmd_ptr->cmd.timer_expiry.timer_id,
                                RRC_PS_DOMAIN_CN_ID);
        break;
        
      case TIMER_T3440:
        emm_timer_t3440_expire(emm_ctrl_data_ptr) ;
        break;
  
      case TIMER_T3412:
        emm_timer_t3412_expire(emm_ctrl_data_ptr) ;
        break;
  
      case TIMER_T3430:
        emm_timer_t3430_expire(emm_ctrl_data_ptr) ;
        break;
  
      case EMM_ACCESS_BARRING_TIMER:
        emm_process_access_barring_timer_expiry(emm_ctrl_data_ptr);
        break;
  
      case EMM_FORBIDDEN_TAI_CLEAR_TIMER:
        emm_forbidden_tai_clear_timer_expiry(emm_ctrl_data_ptr);
        break;
#ifdef FEATURE_NAS_GW
      case TIMER_T3423:
        emm_timer_t3423_expire(emm_ctrl_data_ptr) ;
        break;
#endif
      case TIMER_T3417_EXT:
        emm_timer_t3417_ext_expire(emm_ctrl_data_ptr) ;
        break;
      case TIMER_EMM_PS_DETACH:
        emm_ps_detach_clean_up(emm_ctrl_data_ptr);
        reg_service_report(MMR_CAUSE_NOT_SPECIFIED, FALSE, emm_ctrl_data_ptr);
        break;
  #ifdef FEATURE_LTE_REL9
      case TIMER_T3442:
        MSG_HIGH_DS_0(MM_SUB,"=EMM= T3442 timer expired");
        break;
  #endif
#ifdef FEATURE_LTE_REL10
    case TIMER_T3346:
      emm_timer_t3346_expire(FALSE, emm_ctrl_data_ptr) ;
#ifdef FEATURE_NAS_GW
/*Reset LU initiated flag to flase since 3346 expired.*/
      mm_is_lu_to_be_initiated_t3346_ltogw = FALSE;
#endif
      break;
#endif
#ifdef FEATURE_LTE_REL12
    case TIMER_T3325:
      break;
#endif
#ifdef FEATURE_LTE_REL14
      case TIMER_T3449:
        break;
#endif
#if defined FEATURE_DUAL_SIM || defined FEATURE_1XSRLTE || defined FEATURE_CATM_LTE_EXT_GNSS_COEX || defined FEATURE_WWAN_GNSS_DYNAMIC
    case TIMER_EMM_RADIO_RETRY:
      emm_timer_radio_retry_expire(emm_ctrl_data_ptr) ;
      break;
#endif 

#ifdef FEATURE_1XSRLTE 
      case TIMER_SRLTE_ESR:
        emm_srlte_esr_timer_expire(emm_ctrl_data_ptr);
        break;
#endif
#ifdef FEATURE_SGLTE
      case TIMER_DELAY_TAU:
        emm_timer_delay_tau_expire(emm_ctrl_data_ptr) ;
        break;
#endif
      case TIMER_T3324:
        emm_timer_t3324_expire(emm_ctrl_data_ptr);
        break;
      
       case TIMER_T3247:
        emm_t3247_expire(emm_ctrl_data_ptr);
        break;
#ifdef FEATURE_CIOT
       case TIMER_T3448:
	   	emm_handle_timer_t3448(FALSE,0);
		break;
#endif
#ifdef FEATURE_FMD_SPEED_INFO
    case TIMER_FMD_PSM_FALLBACK:
 	emm_handle_fmd_psm_fallback_timer_expiry(emm_ctrl_data_ptr);
 	break;
#endif
      default:
        MSG_FATAL_DS(MM_SUB, "Unexpected timer ID %d",cmd_ptr->cmd.timer_expiry.timer_id, 0,0 );
        break;
    }
  }
  else
  {
     MSG_ERROR_DS(MM_SUB,"=EMM= Timer %d expired after being stopped or restarted", cmd_ptr->cmd.timer_expiry.timer_id,0,0);  
  }
  
  event.emm_timer_event = (byte)(cmd_ptr->cmd.timer_expiry.timer_id);
  event_report_payload(EVENT_LTE_EMM_TIMER_EXPIRY,sizeof(event),(void*)&event);
}

boolean emm_process_unexpected_rrc_primitive
(
  void                 *rrc_message_router_cmd,
  emm_ctrl_data_type   *emm_ctrl_data_ptr,
  mm_emm_dispatcher_fp **mm_emm_dispatcher_fn_ptr
)
{
  emm_rrc_cmd_type   *rrc_msgr_cmd = NULL;
  

  rrc_msgr_cmd = (emm_rrc_cmd_type*)rrc_message_router_cmd;

  MSG_ERROR_DS(MM_SUB,"Unexpected RRC message %d - discarded",
           rrc_msgr_cmd->rrc_msgr_cmd.msg_hdr.id,0,0);

  switch(rrc_msgr_cmd->rrc_msgr_cmd.msg_hdr.id)
  {
    case LTE_RRC_DL_DATA_IND:
      emm_free_dsm_memory_in_rrc_data_ind((lte_rrc_dl_data_ind_s *)\
                &(rrc_msgr_cmd->rrc_msgr_cmd.rrc_dl_data_ind));
      break;

    default:
      break;
  }

  return TRUE;
}
#endif
/*===========================================================================

FUNCTION FREE_CMD_QUEUE

DESCRIPTION
  This function clears the command queue

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void free_cmd_queue(void *cmd_ptr, mm_process_messages_fp *mm_process_fp)
{
  if((mm_process_fp == mmcoord_route_mm_emm_messages) 
#ifdef FEATURE_NAS_GW
#ifdef FEATURE_LTE 
     ||(mm_process_fp == mmcoord_route_oos_rlf_mm_emm_messages)
#endif
#ifndef FEATURE_NAS_IRAT_DISABLED
     ||(mm_process_fp == mmmultimode_route_L2GW_irat_mm_emm_messages)
     ||(mm_process_fp == mmmultimode_route_GW2L_irat_mm_emm_messages)
#endif
#endif
#if defined(FEATURE_CIOT) && !defined(FEATURE_NAS_GW)
     || (mm_process_fp == mmcoord_route_oos_rlf_mm_emm_messages_lte) 
#endif
     )

  {
    mm_free_cmd_buf( cmd_ptr );
  }

#ifdef FEATURE_LTE
  else if((mm_process_fp == emm_process_outgoing_esm_message)
#ifdef FEATURE_NAS_GW
          ||
          (mm_process_fp == emm_process_oos_rlf_outgoing_esm_message) 
#ifndef FEATURE_NAS_IRAT_DISABLED
          || (mm_process_fp == emm_process_irat_outgoing_esm_message)
#endif
#elif defined(FEATURE_CIOT)
           || (mm_process_fp == emm_process_oos_rlf_outgoing_esm_message) 
#endif
         )
  {
    msgr_rex_free_msg_buf(&(((emm_cmd_type*)cmd_ptr)->link));
  }
  else if((mm_process_fp == emm_process_rrc_primitive)
#ifdef FEATURE_NAS_GW
          ||
          (mm_process_fp == emm_process_oos_rlf_rrc_primitive)
#ifndef FEATURE_NAS_IRAT_DISABLED
          ||(mm_process_fp == emm_process_irat_rrc_primitive)
#endif
#endif          
#if defined(FEATURE_CIOT) && !defined(FEATURE_NAS_GW)
          ||(mm_process_fp == emm_process_oos_rlf_rrc_primitive_lte)
#endif
          ||(mm_process_fp == emm_process_unexpected_rrc_primitive))
  {
    msgr_rex_free_msg_buf(&(((emm_rrc_cmd_type*)cmd_ptr)->link));
  }
  else if(mm_process_fp == emm_process_outgoing_tlb_message)
  {
    msgr_rex_free_msg_buf(&(((tlb_cmd_type*)cmd_ptr)->link));
  }
#ifdef FEATURE_LTE_TO_1X
  else if(mm_process_fp == emm_process_csfb_primitive)
  {
    msgr_rex_free_msg_buf(&(((emm_csfb_cmd_type*)cmd_ptr)->link));
  }
#endif
#if defined (FEATURE_LTE_REL9)
#ifdef FEATURE_NAS_GPS_ENABLE
  else if(mm_process_fp == emm_process_ul_generic_nas_transport_msg )
  {
    msgr_rex_free_msg_buf(&(((emm_gps_cmd_type*)cmd_ptr)->link));
  }
#endif
#endif /*FEATURE_LTE_REL9 */
#endif
#ifdef FEATURE_CIOT
  else if(mm_process_fp == emm_process_phy_cmd_msg )
  {
    msgr_rex_free_msg_buf(&(((emm_phy_cmd_s*)cmd_ptr)->link));
  }
#endif  
  else
  {
    MSG_ERROR_DS(MM_SUB,"=MM= Unable to free queue w/ fp %x", mm_process_fp,0,0);
  }
}

/*===========================================================================

FUNCTION MM_CM_GET_CMD_BUF

DESCRIPTION
  Get a mm_cmd_type buffer for specific use of CM with the header information
  already filled in.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to mm_cmd_type buffer.

SIDE EFFECTS
  None

===========================================================================*/
mm_cmd_type *mm_cm_get_cmd_buf(cm_mm_cmd_e_T cmd_type)
{
   mm_cmd_type *cmd;

   if(cmd_type >= MMCM_CMD_MAX)
   {
      MSG_ERROR_DS(MM_SUB, "=MM= Invalid mm command ID is supplied", 0,0,0);
      return NULL;
   }
#ifndef FEATURE_MODEM_HEAP
   cmd = (mm_cmd_type*)gs_alloc(sizeof(mm_cmd_type));
#else
   cmd = (mm_cmd_type*)modem_mem_calloc(1,sizeof(mm_cmd_type), MODEM_MEM_CLIENT_NAS);
#endif
   mm_check_for_null_ptr((void*)cmd);

   cmd->cmd.hdr.message_set = MS_CM_MM;
   cmd->cmd.hdr.message_id  = (byte) cmd_type;
   switch(cmd_type)
   {
     case MMCM_PS_DATA_AVAILABLE_REQ:
#ifdef FEATURE_LTE
     case MMCM_DUAL_RX_1XCSFB_TAU_REQ:
#endif
       PUT_IMH_LEN(0, &cmd->cmd.hdr) ;
       break;
#ifdef FEATURE_LTE
     case MMCM_RPM_UPDATE_FTAI_LIST:
       PUT_IMH_LEN(sizeof(cm_mm_update_ftai_list_T) - sizeof(IMH_T), &cmd->cmd.hdr) ;
       break;
#endif
#if defined(FEATURE_LTE) && defined(FEATURE_LTE_REL9)
     case MMCM_IMS_REGISTRATION_INFO:
       PUT_IMH_LEN(sizeof(cm_mm_ims_registration_info_T) - sizeof(IMH_T), &cmd->cmd.hdr) ;
       break;
#endif
#ifdef FEATURE_LTE
     case MMCM_UE_CAPABILITIES_INFO:
       PUT_IMH_LEN(sizeof(cm_mm_ue_capabilities_info_T) - sizeof(IMH_T), &cmd->cmd.hdr) ;
       break;
#endif
     case MMCM_MMTEL_CALL_INFO:
       PUT_IMH_LEN(sizeof(cm_mm_mmtel_call_info_T) - sizeof(IMH_T), &cmd->cmd.hdr) ;
       break;

     case MMCM_CALL_CTRL_RSP:
       PUT_IMH_LEN(sizeof(cm_mm_call_ctrl_rsp_s_type) - sizeof(IMH_T), &cmd->cmd.hdr) ;
       break;

     default:
       MSG_FATAL_DS(MM_SUB, "=MM= Unknown command type", 0,0,0);
   }

   return cmd ;
} /* end mm_cm_get_cmd_buf() */


/*===========================================================================

FUNCTION mm_emm_get_as_id

===========================================================================*/
void mm_emm_get_as_id
(
  void *cmd_ptr, 
  rex_sigs_type sigs
)
{
#ifndef FEATURE_LTE
  mm_populate_mm_msg_info((mm_cmd_type *) cmd_ptr);
  mm_add_message_to_debug_buffer(((mm_cmd_type*)cmd_ptr)->cmd.hdr.message_set,
                                   ((mm_cmd_type*)cmd_ptr)->cmd.hdr.message_id, TO_MM
                                   ,(sys_modem_as_id_e_type)mm_as_id
                                   );

#else

  mm_as_id = MM_AS_ID_1;
  mm_sub_id = MM_AS_ID_1;
  if (sigs & MM_CMD_Q_SIG)
  {
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_SGLTE)
    (void)mm_get_as_id((mm_cmd_type*) cmd_ptr);
#endif
    mm_populate_mm_msg_info((mm_cmd_type *) cmd_ptr);
#ifdef FEATURE_NAS_GW
    if((((mm_cmd_type*) cmd_ptr)->cmd.hdr.message_set == MS_CC_MM ||
        ((mm_cmd_type*) cmd_ptr)->cmd.hdr.message_set == MS_CC_MM_PS) &&
        (((mm_cmd_type*) cmd_ptr)->cmd.hdr.message_id == MMCNM_DATA_REQ))
    {
       mm_set_mm_controls_resel_dec_mt_cs_call((mm_cmd_type*) cmd_ptr);
    }
#endif
#ifdef FEATURE_DUAL_SIM
    emm_ctrl_data_ptr = emm_db_get_ctrl_data();
    
#endif
    mm_add_message_to_debug_buffer(((mm_cmd_type*)cmd_ptr)->cmd.hdr.message_set,
                                   ((mm_cmd_type*)cmd_ptr)->cmd.hdr.message_id, TO_MM
                                   ,(sys_modem_as_id_e_type)mm_as_id
                                   );
    if((mm_sim_card_mode == MMGSDI_APP_USIM)
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
       &&
       mm_is_multimode_sub() 
       && (mm_sub_capability == SUBS_CAPABILITY_MULTIMODE 
       || ((mm_cmd_type*)cmd_ptr)->cmd.hdr.message_id != MMR_SIM_NOT_AVAILABLE_REQ)
#endif
      )
    {
      emm_ctrl_data_mem_alloc_check(emm_ctrl_data_ptr);
    }
  }
  else if(sigs & ESM_EMM_CMD_Q_SIG)
  {
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_SGLTE)
    emm_get_esm_as_id(cmd_ptr);
#endif
#ifdef FEATURE_DUAL_SIM
    emm_ctrl_data_ptr = emm_db_get_ctrl_data();
    
#endif
    emm_add_message_to_debug_buffer(((emm_cmd_type*)cmd_ptr)->emm_msgr_cmd.msg_hdr.id, 
                                    TO_EMM,
                                    (sys_modem_as_id_e_type)mm_as_id);
    emm_ctrl_data_mem_alloc_check(emm_ctrl_data_ptr);
  }
  else if(sigs & RRC_EMM_CMD_Q_SIG)
  {
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_SGLTE)
    emm_get_rrc_as_id(cmd_ptr);
#endif
    emm_populate_mm_msg_info(cmd_ptr);
#ifdef FEATURE_DUAL_SIM
    emm_ctrl_data_ptr = emm_db_get_ctrl_data();
    
#endif
      /* Don't store for DL data ind here will add to debug buffer later*/
     if ((((emm_rrc_cmd_type*)cmd_ptr)->rrc_msgr_cmd.msg_hdr.id)!= LTE_RRC_DL_DATA_IND)
     {
       emm_add_message_to_debug_buffer(((emm_rrc_cmd_type*)cmd_ptr)->rrc_msgr_cmd.msg_hdr.id,
                                         TO_EMM,(sys_modem_as_id_e_type)mm_as_id);
	 }
                                     
     emm_ctrl_data_mem_alloc_check(emm_ctrl_data_ptr);
  }
  else if(sigs & TLB_EMM_CMD_Q_SIG)
  {
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_SGLTE)
    emm_get_tlb_as_id(cmd_ptr);
#endif
#ifdef FEATURE_DUAL_SIM
    emm_ctrl_data_ptr = emm_db_get_ctrl_data();
    
#endif
     emm_add_message_to_debug_buffer(((tlb_cmd_type*)cmd_ptr)->cmd.hdr.id,
                                      TO_EMM,
                                      (sys_modem_as_id_e_type)mm_as_id);
    emm_ctrl_data_mem_alloc_check(emm_ctrl_data_ptr);
  }
  else if(sigs & CSFB_EMM_CMD_Q_SIG)
  {
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_SGLTE)
    emm_get_irat_csfb_as_id(cmd_ptr);
#endif
#ifdef FEATURE_DUAL_SIM
    emm_ctrl_data_ptr = emm_db_get_ctrl_data();
    
#endif
    emm_add_message_to_debug_buffer(((emm_csfb_cmd_type*)cmd_ptr)->cmd.msg_hdr.id,
                                     TO_EMM,
                                     (sys_modem_as_id_e_type)mm_as_id);
    emm_ctrl_data_mem_alloc_check(emm_ctrl_data_ptr);
  }
#if defined (FEATURE_LTE_REL9)
  else if(sigs & GPS_EMM_CMD_Q_SIG)
  {
#ifdef FEATURE_NAS_GPS_ENABLE
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_SGLTE)
    emm_get_gps_as_id(cmd_ptr);
#endif
#ifdef FEATURE_DUAL_SIM
    emm_ctrl_data_ptr = emm_db_get_ctrl_data();
    
#endif
    emm_add_message_to_debug_buffer(((emm_gps_cmd_type*)cmd_ptr)->gps_msgr_cmd.msg_hdr.id,
                                     TO_EMM,
                                     (sys_modem_as_id_e_type)mm_as_id);
    emm_ctrl_data_mem_alloc_check(emm_ctrl_data_ptr);
#endif
  }

#endif /*FEATURE_LTE_REL9 */
#endif

}

void mm_emm_normal_msg_handler(rex_sigs_type sigs,void **mm_process_msg_fn_ptr)
{
  mm_process_messages_fp **local_fp_ptr = ((mm_process_messages_fp **)mm_process_msg_fn_ptr);

  if (sigs & MM_CMD_Q_SIG)
  {
    *local_fp_ptr = mmcoord_route_mm_emm_messages;  
  }
#ifdef FEATURE_LTE
  else if(sigs & ESM_EMM_CMD_Q_SIG)
  {
    *local_fp_ptr = emm_process_outgoing_esm_message; 
  }
  else if(sigs & RRC_EMM_CMD_Q_SIG)
  {
    *local_fp_ptr = emm_process_rrc_primitive; 
  }
  else if(sigs & TLB_EMM_CMD_Q_SIG )
  {
    *local_fp_ptr = emm_process_outgoing_tlb_message; 
  }
#ifdef FEATURE_LTE_TO_1X
  else if(sigs & CSFB_EMM_CMD_Q_SIG  )
  {
    *local_fp_ptr = emm_process_csfb_primitive; 
  }
#endif
#if defined (FEATURE_LTE_REL9)
#ifdef FEATURE_NAS_GPS_ENABLE
  else if(sigs & GPS_EMM_CMD_Q_SIG )
  {
    *local_fp_ptr = emm_process_ul_generic_nas_transport_msg; 
  } 
#endif
#endif
#ifdef FEATURE_CIOT
  else if(sigs & PHY_EMM_CMD_Q_SIG )
  {
    *local_fp_ptr = emm_process_phy_cmd_msg; 
  } 
#endif
#endif
  else
  {
    *local_fp_ptr = NULL;
  }
}


/*===========================================================================

FUNCTION MM_EXTERN_INIT

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_extern_init (void)
{
   /* Initialize the Critical Section */
   rex_init_crit_sect(&timer_crit_sect); 
   /* Initialize the Critical Section */
   rex_init_crit_sect(&nas_nv_crit_sect); 
#ifdef FEATURE_NAS_GW
   /* Initialize the Critical Section */
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE 
   /* Initialize the Critical Section */
   rex_init_crit_sect(&mm_sim_crit_sect); 
#endif
#endif
rex_init_crit_sect(&mm_ftd_crit_sect); 
#ifdef FEATURE_RAT_PRIORITY_LIST

   rex_init_crit_sect(&mm_rat_prioritylist_crit_sec);

#endif
#ifdef FEATURE_CIOT
    rex_init_crit_sect(&nas_ciot_config_crit_sec);

#endif
}


/*===========================================================================

FUNCTION MM_FREE_CMD_BUF

DESCRIPTION
  Free a previously allocated mm_cmd_type buffer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_free_cmd_buf( mm_cmd_type *cmd_buf )
{
   if(cmd_buf == NULL)
   {
      MSG_ERROR_DS(MM_SUB, "=MM= Trying to free an empty pointer", 0,0,0);
   }
#ifndef FEATURE_MODEM_HEAP
   gs_free(cmd_buf);
#else
   modem_mem_free(cmd_buf, MODEM_MEM_CLIENT_NAS);
#endif
}


/*===========================================================================

FUNCTION MM_GET_CMD_BUF

DESCRIPTION
  This function gets a mm_cmd_type from MM task with memory already allocated.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to mm_cmd_type buffer.

SIDE EFFECTS
  None

===========================================================================*/

#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
mm_cmd_type *mm_get_cmd_buf( void )
{
   /* Allocate command buffer */
#ifndef FEATURE_MODEM_HEAP
   return (mm_cmd_type*) gs_alloc( sizeof( mm_cmd_type ));
#else
   return (mm_cmd_type*) modem_mem_calloc(1, sizeof( mm_cmd_type ), MODEM_MEM_CLIENT_NAS);
#endif
}
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif

/*===========================================================================

FUNCTION MM_INIT_BEFORE_TASK_START

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_init_before_task_start (void)
{
   /* Initialize command queue */
   (void)q_init(&mm_cmd_q);
    /* Initialize command queue */
   (void)q_init(&mn_cnm_cmd_q);
 
   /* Initialize gs message passing for this queue */
   if (gs_init_for_send_only(GS_QUEUE_MM,
         (gs_put_handler_ptr)mm_send_routine) != GS_SUCCESS)
   {
       MSG_ERROR_DS(MM_SUB, "=MM= MM not initialized!",0,0,0);
   }

#ifndef FEATURE_MODEM_RCINIT
   /* Clear signals */
   (void) rex_clr_sigs(mm_tcb_ptr, (rex_sigs_type)~0);
#endif
#ifndef FEATURE_MODEM_RCINIT
   /* Initialize the Critical Section */
   rex_init_crit_sect(&timer_crit_sect); 
   /* Initialize the Critical Section */
   rex_init_crit_sect(&nas_nv_crit_sect); 
#ifdef FEATURE_NAS_GW
   /* Initialize the Critical Section */
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE 
   /* Initialize the Critical Section */
   rex_init_crit_sect(&mm_sim_crit_sect); 
#endif
#endif
rex_init_crit_sect(&mm_ftd_crit_sect); 
#ifdef FEATURE_RAT_PRIORITY_LIST

   rex_init_crit_sect(&mm_rat_prioritylist_crit_sec);

#endif
#ifdef FEATURE_CIOT
   rex_init_crit_sect(&nas_ciot_config_crit_sec);

#endif
#endif /*FEATURE_MODEM_RCINIT*/
}
static void mm_init_main_routine_handler(void)
{
    /* Set the main routine handler to normal_msg_handler to begin with*/
#if defined (FEATURE_SGLTE)|| defined (FEATURE_DUAL_SIM)
  byte index;
  for(index = 0; index < MAX_NAS_STACKS; index++)
  {
    mm_emm_dispatcher_func_ptr_sim[index] = mm_emm_normal_msg_handler;
  }
#else
  mm_emm_dispatcher_func_ptr = mm_emm_normal_msg_handler;
#endif
}
 /*===========================================================================
 
 FUNCTION MM_ROUTE_NON_MM_MESSAGES
 
 DESCRIPTION
   This function processes the non MM messages 
 
 DEPENDENCIES
   None
 
 RETURN VALUE
   None
 
 SIDE EFFECTS
   None
 
 ===========================================================================*/

 boolean mm_route_non_mm_messages(mm_cmd_type* cmd_ptr)
{

  boolean return_val = TRUE;

  MSG_HIGH_DS_2(MM_SUB, "=MM= Message Set= %d Message Id= %d", cmd_ptr->cmd.hdr.message_set, cmd_ptr->cmd.hdr.message_id);

   if(cmd_ptr->cmd.hdr.message_set == MS_MM_DS )
   {
      switch (cmd_ptr->cmd.hdr.message_id)
     {
      case MM_PDP_ACTIVATE_REQ :
      case MM_PDP_DEACTIVATE_REQ :
      case MM_PDP_ACTIVATE_REJ_RSP :
      case MM_PDP_MODIFY_REQ :
      case MM_PDP_ACTIVATE_SEC_REQ :
      case MM_PDP_ABORT_REQ :
      case MM_PS_SIGNALING_REL_REQ :
      case MM_PDP_GRACEFUL_ABORT_REQ :
      case MM_PDP_NW_MODIFY_REJ :
      case MM_PDP_NW_MODIFY_CNF :
      case MM_RESTORE_PDP_CTXT_REQ :
      case MM_PDP_SEC_ACTIVATE_REQ :
      case MM_PDP_ACT_SEC_CNF :
           mm_add_message_to_debug_buffer(((mm_cmd_type*)cmd_ptr)->cmd.hdr.message_set,
                                             ((mm_cmd_type*)cmd_ptr)->cmd.hdr.message_id, TO_MM
                                             ,(sys_modem_as_id_e_type)mm_as_id
                                           );

           sm_process_ds_cmd(cmd_ptr);
           break;

      case EMM_RAB_REESTAB_REQ :
      case EMM_PDN_CONNECTIVITY_REQ :
      case EMM_ACT_DEFAULT_BEARER_CONTEXT_ACCEPT :
      case EMM_ACT_DEFAULT_BEARER_CONTEXT_REJ :
      case EMM_ACT_DEDICATED_BEARER_CONTEXT_ACCEPT :
      case EMM_ACT_DEDICATED_BEARER_CONTEXT_REJ :
      case EMM_MODIFY_BEARER_CONTEXT_ACCEPT :
      case EMM_MODIFY_BEARER_CONTEXT_REJ :
      case EMM_BEARER_RESRC_MOD_REQ :
      case EMM_PDN_CONNECTIVITY_ABORT_REQ :
      case EMM_PDN_DISCONNECT_REQ :
      case EMM_MOD_BEARER_REQ :
      case EMM_MOD_BEARER_RSP :
      case EMM_BEARER_RESRC_ALLOC :
      case EMM_BEARER_RESRC_ALLOC_ABORT :
      case EMM_RESTORE_BEARER_CTXT_REQ :
      case EMM_ESM_DATA_TRANSPORT_REQ :
           mm_add_message_to_debug_buffer(((mm_cmd_type*)cmd_ptr)->cmd.hdr.message_set,
                                   ((mm_cmd_type*)cmd_ptr)->cmd.hdr.message_id, TO_MM
                                   ,(sys_modem_as_id_e_type)mm_as_id
                                 );
           esm_process(cmd_ptr);
           esm_process_pending_messages();
           break;

#if defined(FEATURE_GSM_GPRS) && defined (FEATURE_NAS_GW)
      case MM_PS_DATA_AVAIL_REQ :
          mm_add_message_to_debug_buffer(((mm_cmd_type*)cmd_ptr)->cmd.hdr.message_set,
                                   ((mm_cmd_type*)cmd_ptr)->cmd.hdr.message_id, TO_MM
                                   ,(sys_modem_as_id_e_type)mm_as_id
                                 );
          gmm_process_ds_req(cmd_ptr) ;
          break;
#endif
#ifdef FEATURE_WWAN_GNSS_DYNAMIC
      case EMM_MO_EXCEPTION_DATA_IND :
          emm_send_rrc_wwan_set_priority(cmd_ptr->cmd.emm_mo_exception_data_ind.mo_exception_data);
          break;
#endif
      case MM_RESTORE_PSM_CTXT_RSP :
          mm_add_message_to_debug_buffer(((mm_cmd_type*)cmd_ptr)->cmd.hdr.message_set,
                                   ((mm_cmd_type*)cmd_ptr)->cmd.hdr.message_id, TO_MM
                                   ,(sys_modem_as_id_e_type)mm_as_id
                                 );
          if (cmd_ptr->cmd.mm_restore_psm_ctxt_rsp.sys_mode == SYS_SYS_MODE_LTE)
          {
            esm_process(cmd_ptr);
            esm_process_pending_messages();
          }
          else
          {
            sm_process_ds_cmd(cmd_ptr);
          }
         break;

       default:
          //MSG_ERROR_DS_1(MM_SUB," =MM= ERROR: UNKNOWN cmd from DS : %d", ((mm_cmd_type*)cmd_ptr)->cmd.hdr.message_id);
          return_val = FALSE;
          break;
      };
     
   }
   else if (cmd_ptr->cmd.hdr.message_set == MS_CM_MM)
   {
      switch (cmd_ptr->cmd.hdr.message_id)
       {
          case MMCM_CALL_CTRL_RSP :
             mm_add_message_to_debug_buffer(((mm_cmd_type*)cmd_ptr)->cmd.hdr.message_set,
                                   ((mm_cmd_type*)cmd_ptr)->cmd.hdr.message_id, TO_MM
                                   ,(sys_modem_as_id_e_type)mm_as_id
                                 );
           if ((cmd_ptr->cmd.cm_mm_call_ctrl_rsp.ps_rat == SYS_RAT_LTE_M1_RADIO_ACCESS) ||
               (cmd_ptr->cmd.cm_mm_call_ctrl_rsp.ps_rat == SYS_RAT_LTE_NB1_RADIO_ACCESS))
           {
              esm_process(cmd_ptr);
              esm_process_pending_messages();
           }
           else
           {
               sm_process_ds_cmd(cmd_ptr);
               
           }
           break;

         default:
          //MSG_ERROR_DS_1(MM_SUB," =MM= ERROR: UNKNOWN cmd from CM : %d", ((mm_cmd_type*)cmd_ptr)->cmd.hdr.message_id);
          return_val = FALSE;
          break;
      };
   }
   else
   {
     //Let the message processed by MM/GMM/EMM
     return_val = FALSE;
   }

  return return_val;
  
}
  

/*===========================================================================

FUNCTION MM_READ_QUEUE

DESCRIPTION
  This function gets the messages from the queue one at a time untile the queue is exhausted.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean mm_read_queue(q_type *queue, rex_sigs_type sigs)
{  
  void *cmd_ptr = NULL;
  boolean return_val = FALSE;
  void *process_msg_func_ptr = NULL;

  mm_check_for_null_ptr((void *)queue);

  while((cmd_ptr = (void*) q_get(queue)) != NULL)
  {

   if(mm_emm_dispatcher_func_ptr == NULL)
   {
      MSG_FATAL_DS( MM_SUB, "=MM= DEBUG Unexpected MM cmd %d", ((mm_cmd_type*)cmd_ptr)->cmd.hdr.message_id, 0,0 );
   }
   
  /* Process non MM messages and continue to get next Message from Q */
   if( mm_route_non_mm_messages((mm_cmd_type*) cmd_ptr) == TRUE)
   {
      mm_free_cmd_buf( cmd_ptr );
   	  continue;
   }

    /* Parse the as_id and add received messages to the debug buffer */
    mm_emm_get_as_id(cmd_ptr,sigs);
    mm_emm_dispatcher_func_ptr(sigs,&process_msg_func_ptr);
    /*Note: Following order should be maintained 
    process the message --> free the queue  -->reset/set the mm_emm_dispatcher_fn_ptr*/
/*lint -save -e611 */
    return_val = ((mm_process_messages_fp*)process_msg_func_ptr)(
                                                                    cmd_ptr,
#ifdef FEATURE_LTE
                                                                    emm_ctrl_data_ptr,
#endif
                                                                    &mm_emm_dispatcher_func_ptr
                                                                );    
 
    if(mm_emm_dispatcher_func_ptr == NULL)
    {
      MSG_FATAL_DS( MM_SUB, "=MM= DEBUG Unexpected MM cmd %d", ((mm_cmd_type*)cmd_ptr)->cmd.hdr.message_id, 0,0 );
    }  
	
    free_cmd_queue(cmd_ptr,(mm_process_messages_fp *)process_msg_func_ptr); 
   
   /* For OOS/RLF/INTER_RAT features, mm_emm_dispatcher_fn_ptr will be changed when service is lost or found, 
   once this pointer changes - we need to set/reset the mm_process_fp correctly before processing any 
   new incoming message, so that all incoming messages are handled appropriately. 
   In case there are two or more messages in the queue (after mm_emm_dispatcher_fn_ptr is changed), 
   then we need to immedicately change mm_process_fp, so call the  mm_emm_dispatcher_fn_ptr() here*/
   mm_emm_dispatcher_func_ptr(sigs,&process_msg_func_ptr);
  }
  /*lint -restore */
  return return_val;
}



boolean send_message_to_emm(msgr_hdr_s*     msg_ptr, uint32  msg_len, rex_sigs_type sigs)
{  
  
  emm_cmd_type  *emm_msgr_cmd;
  void *cmd_ptr = NULL;
  boolean return_val = FALSE;
  void *process_msg_func_ptr = NULL;

  /* Allocate memory and initialize it */
  emm_msgr_cmd = (emm_cmd_type*)modem_mem_calloc(1,sizeof(emm_cmd_type), MODEM_MEM_CLIENT_NAS);
  mm_check_for_null_ptr((void*)emm_msgr_cmd);
  
  memscpy(((uint8* )&(emm_msgr_cmd->emm_msgr_cmd)), msg_len, msg_ptr, msg_len);
  
  cmd_ptr = (emm_cmd_type*)emm_msgr_cmd;

  {

    mm_emm_dispatcher_func_ptr(sigs,&process_msg_func_ptr);

    /*Note: Following order should be maintained 
    process the message --> free the queue  -->reset/set the mm_emm_dispatcher_fn_ptr*/
/*lint -save -e611 */
    return_val = ((mm_process_messages_fp*)process_msg_func_ptr)(
                                                                    cmd_ptr,
#ifdef FEATURE_LTE
                                                                    emm_ctrl_data_ptr,
#endif
                                                                    &mm_emm_dispatcher_func_ptr
                                                                   );    
 
    
   /* For OOS/RLF/INTER_RAT features, mm_emm_dispatcher_fn_ptr will be changed when service is lost or found, 
   once this pointer changes - we need to set/reset the mm_process_fp correctly before processing any 
   new incoming message, so that all incoming messages are handled appropriately. 
   In case there are two or more messages in the queue (after mm_emm_dispatcher_fn_ptr is changed), 
   then we need to immedicately change mm_process_fp, so call the  mm_emm_dispatcher_fn_ptr() here*/
   mm_emm_dispatcher_func_ptr(sigs,&process_msg_func_ptr);
  }

  modem_mem_free(emm_msgr_cmd, MODEM_MEM_CLIENT_NAS);

  /*lint -restore */
  return return_val;
}


boolean send_message_to_mm(IMH_T *  msg_ptr, uint32  msg_len)
{

  mm_cmd_type  *mm_cnm_cmd;

  void *cmd_ptr = NULL;
  boolean return_val = FALSE;
  void *process_msg_func_ptr = NULL;
  rex_sigs_type sigs = MM_CMD_Q_SIG;

    /* Allocate memory and initialize it */
  mm_cnm_cmd = (mm_cmd_type*)modem_mem_calloc(1,sizeof(mm_cmd_type), MODEM_MEM_CLIENT_NAS);
  mm_check_for_null_ptr((void*)mm_cnm_cmd);
  
  memscpy(((uint8* )&(mm_cnm_cmd->cmd)), msg_len, msg_ptr, msg_len);
  
  cmd_ptr = (mm_cmd_type*)mm_cnm_cmd;

  {
    //mm_emm_get_as_id(cmd_ptr,sigs);
    mm_emm_dispatcher_func_ptr(sigs,&process_msg_func_ptr);

    /*Note: Following order should be maintained 
    process the message --> free the queue  -->reset/set the mm_emm_dispatcher_fn_ptr*/
/*lint -save -e611 */
    return_val = ((mm_process_messages_fp*)process_msg_func_ptr)(
                                                                    cmd_ptr,
#ifdef FEATURE_LTE
                                                                    emm_ctrl_data_ptr,
#endif
                                                                    &mm_emm_dispatcher_func_ptr
                                                                   );    
 
    
   /* For OOS/RLF/INTER_RAT features, mm_emm_dispatcher_fn_ptr will be changed when service is lost or found, 
   once this pointer changes - we need to set/reset the mm_process_fp correctly before processing any 
   new incoming message, so that all incoming messages are handled appropriately. 
   In case there are two or more messages in the queue (after mm_emm_dispatcher_fn_ptr is changed), 
   then we need to immedicately change mm_process_fp, so call the  mm_emm_dispatcher_fn_ptr() here*/
   mm_emm_dispatcher_func_ptr(sigs,&process_msg_func_ptr);
  }

  
  modem_mem_free(mm_cnm_cmd, MODEM_MEM_CLIENT_NAS);
  
  /*lint -restore */
  return return_val;
}
/*===========================================================================

FUNCTION MM_TIMER_EXPIRY

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif

void mm_timer_expiry(unsigned long param)
{
   mm_cmd_type   *cmd_ptr;

   /* Get command buffer from reg task */
   cmd_ptr = mm_get_cmd_buf();

   if (cmd_ptr == NULL)
   {
      MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
   }
    /* Construct message */
   cmd_ptr->cmd.timer_expiry.IMH.message_set = MS_TIMER;
   cmd_ptr->cmd.timer_expiry.IMH.message_id  = MID_TIMER_EXPIRY;

   PUT_IMH_LEN (sizeof (cmd_ptr->cmd.timer_expiry) - sizeof (cmd_ptr->cmd.timer_expiry.IMH),
                     &(cmd_ptr->cmd.timer_expiry.IMH));

   cmd_ptr->cmd.timer_expiry.data        = (byte)(param & 0x0000FFFF);
   cmd_ptr->cmd.timer_expiry.timer_id    = (byte)(param >> 16);

   /* Put on REG command queue */
   mm_put_cmd(cmd_ptr);
}

/*===========================================================================

FUNCTION MM_WAIT

DESCRIPTION
  Main Wait routine for MM task. This routine performs the following functions:
    - Checks MM queue to see if anything is available.  The MM queue
      will be checked to see if anything is available before performing
      a rex_wait().  (There is no need to wait if something is on the queue
      already).  If an item is found on the queue the corresponding bit in
      the returned mask is set.
    - Kicks the watchdog timer while waiting for the specified mask.

DEPENDENCIES
  None

RETURN VALUE
  A rex_sigs_type value indicating all signals set (the current state
  of all signals) The calling unit can then check for the reason for
  exiting the wait.

SIDE EFFECTS
  None

===========================================================================*/
rex_sigs_type mm_wait
(
   rex_sigs_type requested_mask    /* Mask of signals to wait for */
)
{
   rex_sigs_type queue_mask;        /* Mask of signals indicating queue status */
   rex_sigs_type rex_signals_mask;  /* Mask of signals returned by REX     */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   /* Initialize temporary mask holder for queue information */
   queue_mask = 0;

   /* --------------------------------------------------------------
   ** Check message queue:  If the request mask contains a check for
   ** MM_CMD_Q_SIG_MASK then we should check the queue, and set in
   ** the queue_mask an indication that something is waiting to be
   ** serviced on that queue.  The rex signal is also cleared
   ** regardless of the queue count.  This is to cover the case
   ** where there is a signal set but nothing on the queue.
   ** ------------------------------------------------------------- */
   if (requested_mask & MM_CMD_Q_SIG)
   {
      (void) rex_clr_sigs( mm_tcb_ptr, MM_CMD_Q_SIG );

      if (q_cnt( &mm_cmd_q ))
      {
         queue_mask |= MM_CMD_Q_SIG;
      }
   }

   /* Get current state of rex signals */
   rex_signals_mask = rex_get_sigs(mm_tcb_ptr);

   /* --------------------------------------------------------------------
   ** Loop until the current signals from REX (including the queue signals
   ** faked due to an item on a queue, if any) match with one or more of
   ** the requested signals.
   ** -------------------------------------------------------------------- */
   do
   {
#ifndef TEST_FRAMEWORK  
        /* Kick the watchdog */
        /* Report to the Dog HB */
        dog_hb_report(DOG_MM_RPT);
#endif

      /* Perform the 'real' wait (if needed) */
      if (((rex_signals_mask | queue_mask) & requested_mask) == 0)
      {
         rex_signals_mask = rex_wait( requested_mask | MM_DOGRPT_TIMER_SIG );
       /* Clear the watchdog signal */
       (void) rex_clr_sigs( mm_tcb_ptr, MM_DOGRPT_TIMER_SIG);

      }

      /* ---------------------------------------------------------------
      ** Make sure that if we got a queue signal, that there is actually
      ** an item on the queue indicated
      ** --------------------------------------------------------------- */
      if (rex_signals_mask & MM_CMD_Q_SIG)
      {
         (void) rex_clr_sigs( mm_tcb_ptr, MM_CMD_Q_SIG );

         if (q_cnt( &mm_cmd_q ) == 0)   /* No elements in queue ? */
         {
            rex_signals_mask &= ~MM_CMD_Q_SIG;  /* Turn off bit */
         }
      }

   /* do while mask requested does not match current mask */
   } while (((rex_signals_mask | queue_mask) & requested_mask) == 0);

   return ((rex_signals_mask | queue_mask));

} /* end mm_wait() */

#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
#ifdef FEATURE_MODEM_HEAP
/*===========================================================================

FUNCTION nas_mem_alloc

DESCRIPTION
  This function is called by the external modules like RRC in order to allocate 
  dynamic memory. This function simply calls mem_malloc and selects suitable
  heap and returs pointer to the allocated memory.

DEPENDENCIES
  The memory manager must have been previously initialized.

RETURN VALUE
  A pointer to the allocated memory if the memory was successfully allocated,
  otherwise NULL.

SIDE EFFECTS
  None.

===========================================================================*/
void * nas_mem_alloc(size_t size)
{
  void * return_ptr =  NULL;
  
  return_ptr = 
       (unsigned char *) modem_mem_calloc(1,size,MODEM_MEM_CLIENT_NAS);

  mm_check_for_null_ptr((void*)return_ptr);

  return(return_ptr);
}
#endif

/*===========================================================================

FUNCTION NAS_SET_EFS_NAS_MM_CONFIG

DESCRIPTION
 This function would check if the directory for "conf" exists, if not
 will create the directory and nas_mm.conf file in the directoru which
 would contain the paths to NV items. nas_mm.conf is used by the qpst tools
 to backup and restore nv items specified in the .conf file.

DEPENDENCIES
  None.

RETURN VALUE
  None. 
 
SIDE EFFECTS
  None.

===========================================================================*/
#define NAS_MM_CONF_FILE "/nv/item_files/conf/nas_mm.conf"
#define NAS_MM_PERMISSIONS 0777

void nas_set_efs_nas_mm_config( void )
{
  struct fs_stat     nas_conf_stat;
  int32              nas_conf_fd;

  char *nas_conf_items = 
    "/nv/item_files/modem/nas/geran_cap\n"\
"/nv/item_files/modem/nas/lte_nas_ue_sec_capability\n"\
"/nv/item_files/modem/nas/lte_nas_temp_fplmn_backoff_time\n"\
"/nv/item_files/modem/nas/lte_nas_temp_fplmn_backoff_time_Subscription01\n"\
"/nv/item_files/modem/nas/lte_nas_srlte_esr_time\n"\
"/nv/item_files/modem/nas/lte_nas_srlte_esr_time_Subscription01\n"\
"/nv/item_files/modem/nas/drx_cn_coeff_s1\n"\
"/nv/item_files/modem/nas/exclude_ptmsi_type_field\n"\
"/nv/item_files/modem/nas/exclude_old_lai_type_field\n"\
"/nv/item_files/modem/nas/nas_lai_change_force_lau_for_emergency\n"\
"/nv/item_files/modem/nas/nas_lai_change_force_lau_for_emergency_Subscription01\n"\
"/nv/item_files/modem/nas/nas_srvcc_support\n"\
"/nv/item_files/modem/nas/l21xsrvcc_support\n"\
"/nv/item_files/modem/nas/mobility_management_for_voims_feature\n"\
"/nv/item_files/modem/nas/mobility_management_for_voims_feature_Subscription01\n"\
"/nv/item_files/modem/nas/nas_config_feature\n"\
"/nv/item_files/modem/nas/aggression_management\n"\
"/nv/item_files/modem/nas/aggression_management_Subscription01\n"\
"/nv/item_files/modem/nas/csg_support_configuration\n"\
"/nv/item_files/modem/nas/nas_l2g_srvcc_support\n"\
"/nv/item_files/modem/nas/tighter_capability\n"\
"/nv/item_files/modem/nas/nas_nv_classmark_ie\n"\
"/nv/item_files/modem/nas/sglte_nas_nv_config\n"\
"/nv/item_files/modem/nas/gmm_drx_cn_coeff_s1\n"\
"/nv/item_files/modem/nas/isr\n"\
"/nv/item_files/modem/nas/emm_combined_proc\n"\
"/nv/item_files/modem/nas/conn_mode_manual_search\n"\
"/nv/item_files/modem/nas/avoid_guti_nas_security_check\n"\
"/nv/item_files/modem/nas/is_accepted_on_lte\n"\
"/nv/item_files/modem/nas/lte_nas_ignore_mt_csfb_during_volte_call\n"\
"/nv/item_files/modem/nas/lte_nas_ignore_mt_csfb_during_volte_call_Subscription01\n"\
"/nv/item_files/modem/nas/mm_manual_search_timer_value\n"\
"/nv/item_files/modem/nas/service_req_back_off_timer_value\n"\
"/nv/item_files/modem/nas/early_call_init\n"\
"/nv/item_files/modem/nas/volte_rej_mt_csfb\n"\
"/nv/item_files/modem/nas/emm_nas_nv_items\n"\
"/nv/item_files/modem/nas/emm_nas_nv_items_Subscription01\n"\
"/nv/item_files/modem/nas/nas_psm_timer_info\n"\
"/nv/item_files/modem/nas/nas_ext_psm_timer_info\n"\
"/nv/item_files/modem/nas/nas_edrx_config\n"\
"/nv/item_files/modem/nas/nas_gsm_eab_support\n"\
"/nv/item_files/modem/nas/edrx_gsm_fake_negotiation\n"\
"/nv/item_files/modem/nas/nas_ciot_capability_config\n"\
"/nv/item_files/modem/nas/dos_mitigation_feature_config\n"\
"/nv/item_files/modem/nas/dos_mitigation_feature_config_Subscription01\n"\
"/nv/item_files/modem/nas/ciot_irat_tau_mobility\n"\
"/nv/item_files/modem/nas/t3247_timer_val_test\n"\
"/nv/item_files/modem/sms/sms_retry_limit\n"\
"/nv/item_files/modem/nas/t3247_timer_val_test_Subscription01\n"\
"/nv/item_files/modem/nas/emm_reset_ul_cnt_cpsr\n"\
"/nv/item_files/modem/nas/map_cause12_to_cause15\n"\
"/nv/item_files/modem/nas/map_cause12_to_cause15_Subscription01\n"\
"/nv/item_files/modem/nas/mt_page_rach_retry_needed\n"\
"/nv/item_files/modem/nas/fmd_force_local_detach\n"\
"/nv/item_files/modem/nas/fmd_psm_fallback_timer\n\
";

  /* create the directory if it doesn't exist */
  if (efs_stat("/nv/item_files/conf", &nas_conf_stat) == -1 )
  {
    if (efs_stat("/nv/item_files", &nas_conf_stat) == -1 )
    {
      if (efs_stat("/nv", &nas_conf_stat) == -1 )
      {
        (void)efs_mkdir("/nv", NAS_MM_PERMISSIONS);
      }
      (void)efs_mkdir("/nv/item_files", NAS_MM_PERMISSIONS);          
    }
    (void)efs_mkdir("/nv/item_files/conf", NAS_MM_PERMISSIONS); 
  }
 
  /* create the nas_mm.conf file if it doesn't exist */
  if (efs_stat(NAS_MM_CONF_FILE, &nas_conf_stat) < 0 )
  {
    nas_conf_fd = efs_creat(NAS_MM_CONF_FILE, NAS_MM_PERMISSIONS);
  
    if( nas_conf_fd >= 0 )
    {
      (void)efs_write(nas_conf_fd, nas_conf_items,strlen(nas_conf_items));
    }
    else
    {
      MSG_ERROR_DS(MM_SUB,"MM EFS CONF file NOT created %d",nas_conf_fd,0,0);
    }
    (void)efs_close(nas_conf_fd);  
    MSG_HIGH_DS_1(MM_SUB ,"=MM= nas_mm.conf file status %d",nas_conf_fd);
  }
} 

/*===========================================================================
FUNCTION mm_ulog_register

DESCRIPTION
  This function call registers MM Task with ULOG
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mm_ulog_register(void)
{
  ULogResult mm_buffer_status;
  mm_buffer_status = ULogFront_RealTimeInit(&mm_ulog_handle,
                               "NAS_MM_ULOG",  
                               512, 
                               ULOG_MEMORY_LOCAL,
                               ULOG_LOCK_OS);
  MSG_HIGH_DS_1(MM_SUB,"=MM= ULOG REGISTER mm_buffer_status: %d", mm_buffer_status);
}

/*===========================================================================

FUNCTION MM_MAIN

DESCRIPTION
  Entry point for the MM task.  This function performs task initialisation,
  then sits in an infinite loop, waiting on an input queue, and responding to
  messages received.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

#ifdef GMM_DEBUG
int main( int argc, char *argv[] )
/*lint -esym( 715, argv, argc ) */
#else
void mm_main( dword dummy )
/*lint -esym(715,dummy) */
#endif
{
  static rex_sigs_type wait_sigs,sigs;

  sm_cmd_type   *sm_cmd_ptr;     /* To hold received SM command */

#if defined(FEATURE_LTE) && defined(TEST_FRAMEWORK)
   #error code not present
#endif /*FEATURE_LTE*/   

   mm_tcb_ptr = rex_self();

#ifndef FEATURE_MODEM_RCINIT
   mm_init_before_task_start ();
#endif

   /* -------------------------
   ** Initialize watchdog timer
   ** ------------------------- */
#ifndef TEST_FRAMEWORK    
   /* Register with DOG HB new API */
   mm_dog_rpt_var = dog_hb_register_rex((rex_sigs_type) MM_DOGRPT_TIMER_SIG);
#endif

#ifdef FEATURE_MODEM_RCINIT
   rcinit_handshake_startup();
#else
#ifndef FEATURE_MODEM_RCINIT_PHASE2
   tmc_task_start();
#endif
#endif

#ifdef FEATURE_MODEM_CONFIG_REFRESH
   mm_mcfg_refresh_register();
#endif

   nas_set_efs_nas_mm_config();
   mm_ulog_register();
   
   sm_ulog_register();
   
   /* Initialize the SM task and RABM subtask */
   sm_init();
   
#ifdef FEATURE_LTE
   esm_init();
#endif /*FEATURE_LTE*/
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_SGLTE)
   mm_read_device_configuration();
#endif 

#ifdef FEATURE_LTE
   emm_db_alloc_memory();
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
   mm_as_id = MM_AS_ID_1;
   mm_sub_id = MM_AS_ID_1;
#endif
   emm_ctrl_data_ptr  = emm_db_get_ctrl_data();
#endif
   mm_initialise();
#ifdef FEATURE_NAS_GW
   gmm_initialize();
#endif

   cm_init_conn_ids_crit_sect();  /* initialized semaphore used to access mn_call_information[] */ 

#ifndef FEATURE_NAS_DISABLE_VOICE
#ifndef FEATURE_VAAA

   ghdi_mvs_init(); /* Initialise gsvoice library */

   /* MN call back function services */
#endif
#endif

   CNM_initialise_CNM_data();
   MN_initialise_MN_data();

   /* Set the main routine handler to normal_msg_handler to begin with*/
   mm_init_main_routine_handler();
#ifdef FEATURE_LTE   
   emm_initialize(&esm_client, 
                  &rrc_client, 
                  &tlb_client 
#ifdef FEATURE_LTE_TO_1X
                  ,&emm_csfb_client
#endif
#if defined (FEATURE_LTE_REL9)
                  ,
                  &emm_gps_client
#endif /* FEATURE_LTE_REL9*/
#ifdef FEATURE_CIOT
                  ,&emm_phy_client
#endif
                  );
#endif

   mm_as_id = MM_AS_ID_1;
   mm_sub_id = MM_AS_ID_1;

  mm_psm_update_restore_status();

#ifdef FEATURE_NAS_DIAG_INIT
  nas_diag_init();
#endif

   wait_sigs = MM_CMD_Q_SIG  
              | MN_CNM_CMD_Q_SIG
              | SM_CMD_Q_SIG
              | TIMER_ESTABLISH_PENDING
#ifdef FEATURE_LTE
		       | ESM_CMD_Q_SIG
               | ESM_EMM_CMD_Q_SIG | 
               RRC_EMM_CMD_Q_SIG   |
               TLB_EMM_CMD_Q_SIG 
#ifdef FEATURE_LTE_TO_1X
               |
               CSFB_EMM_CMD_Q_SIG
#endif
#if defined (FEATURE_LTE_REL9)
               |
               GPS_EMM_CMD_Q_SIG
#endif
#ifdef FEATURE_CIOT
               |
               PHY_EMM_CMD_Q_SIG
#endif
#endif /*FEATURE_LTE*/
#ifdef TEST_FRAMEWORK
               #error code not present
#endif /*TEST_FRAMEWORK*/              
               ;             
   for(;;)
   {            
      /* Wait on signal mask*/
      sigs = mm_wait( wait_sigs );
      /* If this signal is for the command queue */
      if (sigs & MM_CMD_Q_SIG)
      {
         /* Clear signals */
        (void) rex_clr_sigs( mm_tcb_ptr, MM_CMD_Q_SIG );
/*lint -save -e611 */
        (void)mm_read_queue(&mm_cmd_q,MM_CMD_Q_SIG);
 /*lint -restore */
      }
	  
    /* If this signal is for the SM command queue */
      else if (sigs & SM_CMD_Q_SIG)
      {
      /* Received a Command on the SM command Queue, clear it and process */
      /* Clear signals */
      (void) rex_clr_sigs( mm_tcb_ptr, SM_CMD_Q_SIG);

      /* Get the command from the SM_CMD_Q and process it */
      while ((sm_cmd_ptr = (sm_cmd_type*) q_get(&sm_cmd_q)) != NULL)
      {
#ifdef FEATURE_DUAL_SIM
       if(sm_get_as_id(sm_cmd_ptr) == TRUE)
       {
#endif 
         /* Switch on the command id of the received command, and
          call the appropriate function to process each command. */
        switch (sm_cmd_ptr->header.message_set)
        {
#ifdef FEATURE_NAS_GW
#ifdef FEATURE_GSM_GPRS
          case MS_SNDCP_SM:
            sm_process_sndcp_cmd( sm_cmd_ptr );
            break;
#endif
          case MS_TIMER:
            sm_process_timer_expiry_cmd( sm_cmd_ptr );
            break;

#endif /*FEATURE_GW_NAS*/
          case MS_CM_SM:
            //sm_process_cm_cmd( sm_cmd_ptr );
            //sm_process_ds_cmd    //			RAVI_9x05_CHECK
            break;

          default:
            /* Wrong command received on the SM CMD Q */
            MSG_ERROR_DS_1(SM_SUB, "Unknown cmd received by SM %d", 
                 sm_cmd_ptr->header.cmd_id);
            break;
         }/* end switch */
#ifdef FEATURE_DUAL_SIM
       }
#endif 
        /* The command has been processed. Free the memory for this cmd */
        /*lint -e424*/
#ifdef FEATURE_NAS_GW

        if (sm_check_ps_call_in_progress())
        {
          send_gmm_ps_call_status_ind(TRUE);
        }
        else
        {
          send_gmm_ps_call_status_ind(FALSE);
        }
#endif

#ifdef FEATURE_MODEM_HEAP  
        if((sm_cmd_ptr->header.message_set == MS_GMM_SM) && 
           (sm_cmd_ptr->header.cmd_id == GMMSM_UNITDATA_IND) && 
           (sm_cmd_ptr->cmd.gmm_unitdata_ind.data != NULL))
        {
          modem_mem_free(sm_cmd_ptr->cmd.gmm_unitdata_ind.data, MODEM_MEM_CLIENT_NAS);
          sm_cmd_ptr->cmd.gmm_unitdata_ind.data = NULL;
        }
        modem_mem_free(sm_cmd_ptr, MODEM_MEM_CLIENT_NAS);
#else
        if((sm_cmd_ptr->header.message_set == MS_GMM_SM) &&
           (sm_cmd_ptr->header.cmd_id == GMMSM_UNITDATA_IND) && 
           (sm_cmd_ptr->cmd.gmm_unitdata_ind.data != NULL))
        {
          mem_free( &(tmc_heap), sm_cmd_ptr->cmd.gmm_unitdata_ind.data );
          sm_cmd_ptr->cmd.gmm_unitdata_ind.data = NULL;
        }
        mem_free( &(tmc_heap), sm_cmd_ptr );
#endif
 
        /*lint +e424*/
      }/* end while */
    }
#ifdef FEATURE_NAS_GW
    else if(sigs & TIMER_ESTABLISH_PENDING)
    {           
       // Clear the Corresonding timer.
      sm_clear_pdp_timer (TIMER_ESTABLISH_PENDING);
  
       /*clear signal */
       rex_clr_sigs( mm_tcb_ptr, TIMER_ESTABLISH_PENDING );

      // Perform timer expiry action procedures
      sm_proc_timer_expired(timers_pending[TIMER_PENDING], TIMER_ESTABLISH_PENDING);   
    }
#endif
#ifdef FEATURE_LTE      
      /* Process ESM signal */
     else if(sigs & ESM_CMD_Q_SIG)
     {     
        /*clear signal */
        (void) rex_clr_sigs( mm_tcb_ptr, ESM_CMD_Q_SIG);
         esm_task();
      }
      else if (sigs & ESM_EMM_CMD_Q_SIG)
      {
        (void) rex_clr_sigs( mm_tcb_ptr, ESM_EMM_CMD_Q_SIG);

/*lint -save -e611 */
        (void)mm_read_queue(&esm_esm_cmd_q_msg,ESM_EMM_CMD_Q_SIG);
/*lint -restore */
      }
      else if (sigs & RRC_EMM_CMD_Q_SIG)
      {
        (void) rex_clr_sigs( mm_tcb_ptr, RRC_EMM_CMD_Q_SIG);

/*lint -save -e611 */
        (void)mm_read_queue(&rrc_emm_cmd_q_msg,RRC_EMM_CMD_Q_SIG);
/*lint -restore */
      }
      else if(sigs & TLB_EMM_CMD_Q_SIG)
      {
        (void) rex_clr_sigs( mm_tcb_ptr, TLB_EMM_CMD_Q_SIG);

/*lint -save -e611 */
        (void)mm_read_queue(&tlb_tlb_cmd_q_msg,TLB_EMM_CMD_Q_SIG);
/*lint -restore */
      }
#ifdef FEATURE_LTE_TO_1X
      else if(sigs & CSFB_EMM_CMD_Q_SIG)
      {
        (void) rex_clr_sigs( mm_tcb_ptr, CSFB_EMM_CMD_Q_SIG);

/*lint -save -e611 */
        (void)mm_read_queue(&emm_csfb_cmd_q_msg,CSFB_EMM_CMD_Q_SIG);
/*lint -restore */
      }
#endif
#if defined (FEATURE_LTE_REL9)
      else if(sigs & GPS_EMM_CMD_Q_SIG)
      {
        (void) rex_clr_sigs( mm_tcb_ptr, GPS_EMM_CMD_Q_SIG);

/*lint -save -e611 */
        (void)mm_read_queue(&emm_gps_cmd_q_msg,GPS_EMM_CMD_Q_SIG);
/*lint -restore */
      }
#endif /* FEATURE_LTE_REL9 */
#ifdef FEATURE_CIOT
      else if(sigs & PHY_EMM_CMD_Q_SIG)
      {
        (void) rex_clr_sigs( mm_tcb_ptr, PHY_EMM_CMD_Q_SIG);
/*lint -save -e611 */
        (void)mm_read_queue(&emm_phy_cmd_q_msg,PHY_EMM_CMD_Q_SIG);
/*lint -restore */
      }
#endif /* FEATURE_LTE_REL9 */
#endif /*FEATURE_LTE*/
#ifdef TEST_FRAMEWORK
      #error code not present
#endif/*TEST_FRAMEWORK*/   
      else if(sigs & MN_CNM_CMD_Q_SIG)
      {

        /* Clear signals */
        (void) rex_clr_sigs( mm_tcb_ptr,MN_CNM_CMD_Q_SIG);

        /*
        ** READ INPUT MESSAGE QUEUE
        */
        mn_cnm_process_external_messages();


      }  
   }/* FOREVER */
}


/*===========================================================================

FUNCTION SM_PUT_CMD

DESCRIPTION
  his function puts a command buffer into SM command queue.
  Note that the command buffer must have been requested through a 
  call to sm_get_cmd_buf(). Also, the calling function must have 
  filled all the necessary data including the command id before 
  passing the buffer to this function.

  This function also sets the appropriate signal to the SM
  task to indicate that a command has been placed in its queue. 
    
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  A signal is set for the SM task and this causes a context switch.


===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
void sm_put_cmd (sm_cmd_type * cmd_buf)
{
   if(cmd_buf->header.message_set == MS_CM_SM)
   {
     /* Log using CFA packet */
     cfa_log_packet_ex( GS_QUEUE_SM,
                        cmd_buf->header.message_set,
                        (byte)(cmd_buf->header.cmd_id),
                        MIN(250, sizeof(cmd_buf->cmd)),
                        (byte*)&(cmd_buf->cmd) );
   }
   if(cmd_buf->header.message_set == MS_GMM_SM)
   {
#ifdef FEATURE_SGLTE
      if(MM_SUB_IS_SGLTE &&
         (mm_as_id != mm_ps_stack_id))
      {
        MSG_HIGH_DS_2(SM_SUB," =MM=  Ignoring GMM SM message %d on CS stack %d",(dword)(cmd_buf->header.cmd_id),mm_as_id);
#ifdef FEATURE_MODEM_HEAP
        modem_mem_free(cmd_buf,MODEM_MEM_CLIENT_NAS);
#endif
        return;
      }
#endif
      mm_add_message_to_debug_buffer(
              cmd_buf->header.message_set,
              (dword)(cmd_buf->header.cmd_id),
              FROM_MM,
              (sys_modem_as_id_e_type)mm_as_id

         );
   }

  MSG_LOW_DS_0(SM_SUB," Putting CMD in sm_cmd_q ");

  /* Initialize the queue link before putting an item in a queue. */
  (void) q_link(cmd_buf, &(cmd_buf->header.link));

  /* Then put the command buffer in the queue */
  q_put(&sm_cmd_q, &(cmd_buf->header.link));

  /* Then set the signal for this queue */
  (void) rex_set_sigs(mm_tcb_ptr, SM_CMD_Q_SIG);
  
}
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif

/*===========================================================================

FUNCTION ESM_PUT_CMD

DESCRIPTION
  This function puts a command buffer into MM command queue. and sets EMS_CMD_Q_SIG

    
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  A signal is set for the SM task and this causes a context switch.


===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
void esm_put_cmd (void* cmd_buf)
{
   
  MSG_LOW_DS_0(SM_SUB," Putting CMD in mm_cmd_q ");

  /* Initialize the queue link before putting an item in a queue. */
  (void) q_link((mm_cmd_type*)cmd_buf, &(((mm_cmd_type*)cmd_buf)->link));

  /* Then put the command buffer in the queue */
  q_put(&esm_cmd_q_msg, &(((mm_cmd_type*)cmd_buf)->link));

  /* Then set the signal for this queue */
  (void) rex_set_sigs(mm_tcb_ptr, ESM_CMD_Q_SIG);
  
}
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif


