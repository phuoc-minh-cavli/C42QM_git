#ifndef DSATETSICALL_H
#define DSATETSICALL_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                D A T A   S E R V I C E S

                U M T S

                C A L L   C O N T R O L
                                
                A T   C O M M A N D   P R O C E S S I N G

DESCRIPTION
  This file contains the definitions of data structures, defined and
  enumerated constants and function prototypes required for the
  data services AT command ( UMTS call control commands ) processor.

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2002 - 2023 by Qualcomm Technologies Incorporated.
 All Rights Reserved.
 Qualcomm Confidential and Proprietary.
  
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  L:/src/asw/MSM5200/data/vcs/dsatetsicall.h_v   1.5   12 Jul 2002 10:05:20   randrew  $
  $Header: //components/rel/dataiot.mpss/2.0/interface/atcop/inc/dsatetsicall.h#2 $ $DateTime: 2023/05/16 04:05:59 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/15/23   ks      Enhancing +COPS, +CEREG commands to support NTN RAT.
01/15/18   rk      Fixed issue related to COPS cmd.
01/19/17   skc     Added enhancement on CHLD and CLCC for VoLTE MPTY Call.
12/16/16   skc     Added support for AT+VTS before connect event.
08/19/16   skc     Fixed USSD issue when both MO and MT USSD present.
08/18/16   skc     Fixed issue with at+chld=2 during MPTY calls
12/16/15   skc     Fixed issue where wrong call id is stored during one 
                   active call and one held call and at+chld=2 is issued.
04/15/15   sc      Fixed issue in event handling to use subs_id provided in 
                   event info.
08/01/14   sc      Added support for linked MO and MT USSD sessions.
07/08/14   sc      Fixed SGLTE+G issue on SUB2.
06/27/14   tk/sc   Added support for Dynamic ATCoP.
02/15/14   pg      Added support for SGLTE+G.
04/29/14   sc      Added csg_list_cat support to $QCCSGCOPS command.
10/11/13   sc      Added support for $QCNSP, $QCSIMT, $QCRCIND commands and
                   REMOTE CALL END, REMOTE RING, REMOTE ANSWER
                   unsolicited result codes.
07/02/13   pg      Migrated to MMGSDI API to find PLMN info in SE13 table
06/05/13   tk      ATCoP changes for SGLTE support on Dime Plus.
05/16/13   sk      $QCCOPS command for Extension of network search.
02/22/13   tk      ATCoP - CM interface changes for Triton DSDA.
01/23/13   sk      DS command buffer memory reduction.
11/30/12   tk      ATCoP changes for Triton DSDA.
10/12/12   sk      Support to add "No Change" when  value is not given in +COPS.
07/31/12   sk      C-EONS Feature Support.
03/07/12   sk      Made CLCC common for all modes.
02/17/12   sk      Migrating Data related AT commands interface to unified MH.
02/06/12   ad      Added CSTF/CEN/CGPIAF/CVMOD command support.
12/26/11   ad      Added Support the New Call Control Interface in Call Manager.
01/19/12   sk      Feature cleanup.
10/17/11   ad      Added +CECALL support.
07/05/11   nc      Added support for ^MODE,^CARDMODE,^SYSCONFIG,^SYSINFO,^DSCI.
04/07/11   ad      Fix to ignore unexpected password indication from CM.
10/04/10   ua      Increasing LONG network name to be 24 characters. 
08/30/10   ad      Fixed cops act issue.
06/04/10   bs      MMGSDI SESSION Migration.
05/10/10   kk      Mainlining pending GOBI changes.
03/15/10   bs      Added +CQI support.
01/15/10   sa      Added support for +CPIN,^CPIN and +CLCK for RUIM cards.
12/15/09   nc      Featurisation changes for LTE.
11/30/09   bs      Added +COPS support for LTE.
06/11/09   nc      Modified the waiting call end using +CHUP to 
                   reflect the correct cause code.
12/29/08   nc      Added support for +VTS Command
12/12/08   ua      Fixed Off target lint errors.
12/10/08   ss      Off Target Lint fix.
11/04/07   pp      Lint low - fixes.
09/07/07   ua      Updated supserv_cm_op_s_type to support +CUSD abort
                   for NW initiated USSD operations.
08/24/07   ar      Fix feature wrapping for non-GCSD builds.
07/27/07   ar      Added dsatetsicall_update_crlp_param_set() prototype.
04/05/07   ua      Added support for COPS DEREGISTRATION;
03/21/07   sa      Add support for BS code not being sent in case of class 255 
                   to map to All tele and bearerservices of MMI code.
03/02/07   sa      Added outgoing voice call support when there is an active call  
                   for ATD command.
02/15/07   ua      Added new phone book "FD" support.
02/13/07   pkp     Added comments on the usage of DSAT_INVALID_OP_CODE.
07/11/06   ua      CACM/CAMM modifications according to the new CM API's
02/21/06   sa      Merged Changes from Sirius Branch
                   -Added +CLIR interrogation result enums.
05/12/06   ua      Function dsatetsicall_display_pref_networks is wrapped 
                   under FEATURE_MMGSDI 
01/19/06   ua      Added support for +CTFR command.
08/26/05   tkk     Added a new macro for CLI validity.
11/30/05   snb     Add support for +CGCMOD command.
09/07/05   ar      Added dsatetsicall_init_call().
07/25/05   snb     Add external RAB event handler function to support 
                   WCDMA-mode $QCDGEN.
05/05/05   iv      Added call id to sequence number mapping for +CLCC
04/29/05   ar      Add support for +CEER command.
04/27/05   ar      Add support for +CV120 command.
04/12/05   ar      Add support for +COPS access technology selection.
04/05/05   iv      Modified +CLCC to support CS data calls.
02/28/05   hap     Added support for +CSSN command
02/14/05   dvp     Added support for Personalization facility in +CLCK
02/03/05   iv      Added support for +CPOL command.
01/31/05   iv      Added support for +COPN command.
01/27/05   pdv     Merge from Sirius-ATCoP development.
01/27/05   pdv     Rebase from Mainline.
01/20/05   tkk     Added prototypes for +CLIP abort and event handlers.
01/05/05   iv      Added support for +CLCC command
12/19/04   tkk     Added prototypes for CLIP, COLP functions.
12/15/04   snb     Add ATH to command enum and make some variables external.
12/07/04   ar      Add support for +COPS command.
11/09/04   ar      Add support for +CPUC.
10/15/04   ar      Add +CAOC,+CACM,+CAMM support.
09/03/04   snb     Add +CHLD support.
05/12/04   snb     Fix +CGACT response when deactivating contexts activated via
                   embedded sockets.
04/06/04   ar      Added dsatetsicall_init_call_param_from_nv() prototype
03/19/04   snb     Added support for +CCWA command.
10/07/03   ar      Add +CREG and +CGREG support
09/05/03   ar      Added typedef dsat_sups_cmd_info_u_type.  Adjust
                   function prototypes for new parameter.
07/30/03   ar      Added prototypes for +CRLP command.
06/26/03   sb      Moved dsatetsicall_voice_cmd_orig_cb_handler() to common
                   files. Now it is dsatvoice_cmd_orig_cb_handler().
06/24/03   ar      Remove LOCAL on supserv_cm_op_s and supserv_cm_parm_u.
03/07/03   ar      Remove FEATURE_DATA_ETSI_SUPSERV wrappers
01/08/02   wx      Added support for +ES and +ESA command
12/09/02   ar      Added support for +CUSD command
12/02/02   ar      Added support for +CPWD command
11/15/02   ar      Added support for +CCUG and +CLCK commands
10/31/02   ar      Added support for +CCFC command
10/28/02   sb      Opened Voice call related handlers (moved to a common file)
10/17/01   sb      created file

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

/* This file should be included only in GSM or WCDMA mode */
#ifdef FEATURE_DSAT_ETSI_MODE

#include "mn_cm_exp.h"
#include "dsati.h"
#include "dsatcmif.h"

/*===========================================================================

            PUBLIC DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains public definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/

/*---------------------------------------------------------------------------
    Typedefs for Supplementary Services support
---------------------------------------------------------------------------*/

#define DSAT_MAX_MMI_STRING_LEN  82 /*Maximum length of MMI string (+1 for NULL character)*/

/* Enum to track the current command executing. */
/* Mapping is a follows:                        */
/*   FACILITY --> +CLCK or +CPWD                */
/*   USSD     --> +CUSD                         */
typedef enum ss_operation_e
{
  DSAT_SSOP_FACILITY,
  DSAT_SSOP_USSD,
  DSAT_SSOP_MAX
} ss_operation_e_type;

/* This enum must match order of dsat_clck_fac_valstr[] */
/* and dsat_cpwd_fac_valstr[]                           */
typedef enum ss_fac_e
{
  DSAT_SS_FAC_ALLB,    /* All Barring Services */
  DSAT_SS_FAC_AINB,    /* All Incoming Barring Services */
  DSAT_SS_FAC_AOTB,    /* All Outgoing Barring Services */
  DSAT_SS_FAC_BAIC,    /* Barr All Incoming Calls */
  DSAT_SS_FAC_BAOC,    /* Barr All Outgoing Calls */
  DSAT_SS_FAC_BICR,    /* Barr Incoming Calls when Roaming */
  DSAT_SS_FAC_BOIC,    /* Barr Outgoing International Calls */
  DSAT_SS_FAC_BOICHC,  /* BOIC except to Home Country */
  DSAT_SS_FAC_PIN1,    /* SIM PIN1 Lock */
  DSAT_SS_FAC_PIN2,    /* SIM PIN2 Lock */
  DSAT_SS_FAC_MAX
} ss_fac_e_type;

/* States for USSD */
typedef enum cusd_state_e
{
  DSAT_CUSD_STATE_NULL,            /* No pending USSD transaction */
  DSAT_CUSD_STATE_MS_SEND,         /* MS initiated transaction    */
  DSAT_CUSD_STATE_MS_ABORT,        /* MS abort of transaction     */
  DSAT_CUSD_STATE_SS_REQUEST,      /* SS requested data           */
  DSAT_CUSD_STATE_SS_NOTIFY,       /* SS notification received    */
  DSAT_CUSD_STATE_CLEAR_MT_USSD,   /* Clear MT USSD Session */
  DSAT_CUSD_STATE_MAX
} cusd_state_e_type;

typedef enum cusd_result_e
{
  DSAT_CUSD_RESULT_DONE    = 0,    /* No further action required   */
  DSAT_CUSD_RESULT_MORE    = 1,    /* Further user action required */
  DSAT_CUSD_RESULT_ABORT   = 2,    /* USSD terminated by network   */
  DSAT_CUSD_RESULT_OTHER   = 3,    /* Other local client responded */
  DSAT_CUSD_RESULT_NOSUP   = 4,    /* Operation not supported      */
  DSAT_CUSD_RESULT_TIMEOUT = 5,    /* Network time out             */
  DSAT_CUSD_RESULT_MAX
} cusd_result_e_type;

typedef enum ss_class_e
{
  DSAT_SS_CLASS_MIN        = 0x0,
  DSAT_SS_CLASS_VOICE      = 0x1,
  DSAT_SS_CLASS_DATA       = 0x2,
  DSAT_SS_CLASS_FAX        = 0x4,
  DSAT_SS_CLASS_SMS        = 0x8,
  DSAT_SS_CLASS_DATA_SYNC  = 0x10,
  DSAT_SS_CLASS_DATA_ASYNC = 0x20,
  DSAT_SS_CLASS_DATA_PKT   = 0x40,
  DSAT_SS_CLASS_DATA_PAD   = 0x80,
  DSAT_SS_CLASS_ALL        = 0xFF,
  DSAT_SS_CLASS_MAX        = 0xFFFF
} ss_class_e_type;

/* To support asynchronous processing for +CACM/CAMM read commands */
typedef enum dsat_acm_e
{
  DSAT_ACM_NONE,
  DSAT_ACM_READ,    /* CACM Read */
  DSAT_ACM_MAX_READ /* CAMM Read */
}dsat_acm_e_type;

#define DSAT_CREG_STAT_CNG_UNLCK             0
#define DSAT_CREG_STAT_CNG_LCK               1
#define DSAT_CREG_STAT_CNG_CS_LCK            2
#define DSAT_CREG_STAT_CNG_PS_LCK            4
#define DSAT_CREG_STAT_CNG_CSPS_LCK          8

typedef struct bsg_conversion_s
{
  ss_class_e_type class; 
  bsg_code_type   bsg_type;/* BSG type - bearer services, teleservices */
  byte   net_bsg_code;     /* network BSG value */
} bsg_conversion_s_type;

/* Facility lock user data */
typedef struct ss_fl_user_s
{
  ss_fac_e_type facility;                         /* Facility code    */
  clck_mode_e_type mode;                          /* Mode code        */
/*  Password length should be MAX(SIMLOCK_CK_MAX, GSDI_MAX_PIN_LEN, MAX_PWD_CHAR) + 1 currently,
    SIMLOCK_CK_MAX is 16 so using that */
  byte  passwd[SIMLOCK_CK_MAX + 1]; /* Passwords */
  byte  new_passwd[SIMLOCK_CK_MAX + 1];
} ss_fl_user_s_type;

/* USSD user data */
typedef struct ss_us_user_s
{
  uint8 dcs;                                  /* Data coding scheme   */
  byte  string[MAX_USS_CHAR_ASCII+1];               /* USSD string          */
} ss_us_user_s_type;

/* User data operation union */
typedef union ss_user_data_u
{
  ss_fl_user_s_type fac_lock;                 /* Facility lock data   */
  ss_us_user_s_type ussd;                     /* USSD data            */
} ss_op_data_u_type;

/* User entered data */
typedef struct ss_user_data_s
{
  uint8 class;                                 /* Class user entered  */
  ss_operation_e_type operation;               /* Operation underway  */
  ss_op_data_u_type opdata;                    /* Operation data      */
} ss_user_data_s_type;

/* Call Manager sups types */
typedef union supserv_cm_parm_u
{
  cm_reg_ss_params_s_type      reg;            /* Registration        */
  cm_erase_ss_params_s_type    erase;          /* Erasure             */
  cm_act_ss_params_s_type      act;            /* Activation          */
  cm_deact_ss_params_s_type    deact;          /* Deactivation        */
  cm_interr_ss_params_s_type   interr;         /* Interrogation       */ 
} supserv_cm_parm_u_type;

/* CUSD Session ID */
typedef enum cusd_session_id_e
{
  DSAT_CUSD_SESSION_ID_MO  = 0, /* MO USSD Session */
  DSAT_CUSD_SESSION_ID_MT  = 1, /* MT USSD Session */
  DSAT_CUSD_SESSION_ID_MAX = 2  /* Maximum number of USSD sessions */
} cusd_session_id_e_type;

/* CUSD Session Info */
typedef struct cusd_session_info_s
{
  boolean is_active;      /* Session active flag */
  uint8   invoke_id;      /* Network invocation ID */
  uint8   ussd_oper_type; /* USSD operation type */
} cusd_session_info_s_type;

typedef enum cops_fmt_e
{
  DSAT_COPS_FMT_LONG = 0,  /* Long format alphanumeric */
  DSAT_COPS_FMT_SHRT = 1,  /* Short format alphanumeric */
  DSAT_COPS_FMT_PLMN = 2,  /* Numeric PLMN */
  DSAT_COPS_FMT_MAX      /* Internal use only */
} dsat_cops_fmt_e_type;

 typedef enum
{
  CMD_CHUP,
  CMD_ATH,
  CMD_NONE   = 0x80
} etsicall_cmd_type;

/* Call command types. */
typedef enum
{
  DSAT_CALL_CMD_ORIG,
  DSAT_CALL_CMD_ANSWER,
  DSAT_CALL_CMD_END,
} dsat_call_cmd_e_type;

extern etsicall_cmd_type etsicall_cmd_pending;

extern rex_timer_type dsatetsicall_inband_dtmf_timer;

#define DSAT_CLIP_ENABLED  (dsat_num_item_type)1
#define DSAT_COLP_ENABLED  (dsat_num_item_type)1
#define DSAT_CDIP_ENABLED  (dsat_num_item_type)1

#define DSAT_MAX_LINEID_BUF        100
#define DSAT_MAX_CALLED_LINEID_BUF 100

#define DSAT_NO_NW_SERVICE (uint32)2

#define DSAT_CLIP_NUMBER_TYPE_UNKNOWN 128
#define DSAT_CDIP_NUMBER_TYPE_UNKNOWN 128
/* CLIP, COLP and CLIR lie in the range 11 through 13 */

/* This opcode also mapped to all_ss in ss_operation_code_T enum */
#define DSAT_INVALID_OP_CODE            0

#define DSAT_CLIP_NUMBER_CLI_VALIDITY (byte)2

/* This enum represents the CLIR provisioning status at the network */
typedef enum dsat_clir_status_e {
  DSAT_CLIR_SRV_NOT_PROVISIONED         = 0,
  DSAT_CLIR_SRV_PROVISIONED_PERMANENT   = 1,
  DSAT_CLIR_SRV_NO_NETWORK              = 2,
  DSAT_CLIR_SRV_PRESENTATION_RESTRICTED = 3,
  DSAT_CLIR_SRV_PRESENTATION_ALLOWED    = 4 
} dsat_clir_status_e_type;

/* CLIR enable/disable settings */
typedef enum clir_type_e{
  DSAT_CLIR_PRESENTATION_INDICATOR = 0,
  DSAT_CLIR_INVOCATION_OPTION  = 1,
  DSAT_CLIR_SUPPRESSION_OPTION = 2
}dsat_clir_e_type;

#define DSAT_CLI_ALLOWED        (byte)0
#define DSAT_CLI_RESTRICTED     (byte)1
#define DSAT_CLI_NOT_AVAILABLE  (byte)2
#define DSAT_CLI_INVALID        (byte)2

/* Capture the call end status codes for +CEER command */
typedef enum
{
  DSAT_CES_DOMAIN_NULL,    /* Internal use only */
  DSAT_CES_DOMAIN_CS,
  DSAT_CES_DOMAIN_PS,
  DSAT_CES_DOMAIN_MAX      /* Internal use only */
} dsatetsicmif_ces_domain_e_type;
  
typedef  struct
{
  cm_call_end_e_type         end_status;
  ie_cm_cc_cause_s_type      cc_cause;
  ie_cm_cc_reject_s_type     cc_reject;
} ceer_cs_domain_s_type;



typedef struct
{
  sys_sys_mode_e_type        sys_mode;
  cause_enum_type_T          pdp_cause_type;
  pdp_cause_T                cause;
  /*for lte cause codes*/
  esm_cause_T                esm_cause;
  lte_nas_local_cause_T      esm_local_cause;
} ceer_ps_domain_s_type;

typedef struct dsatetsicmif_ces_info_s
{
  dsatetsicmif_ces_domain_e_type domain;
  union
  {
    ceer_cs_domain_s_type  cs_domain;
    ceer_ps_domain_s_type  ps_domain;
  } error_info;
} dsatetsicmif_ces_info_s_type;


typedef struct
{
  cm_call_id_type call_id;
  cm_call_substate_type sub_state;
}dsat_clcc_call_info;

typedef struct {
  boolean                 is_profile_id_valid;
  uint16                  profile_id;
  dsat_call_abort_cb_type data_s7_abort_handler;
}dsat_s7_info_type;

#define MAX_COPN_LEN                  6
#define DSAT_PUCT_LEN                 5

typedef struct 
{
  cm_call_id_type           call_id;
  cm_call_mode_info_e_type  info_type;  
}dsat_call_info_before_connect;


/*SS*/
typedef struct dsatetsicall_call_state_info
{
  byte                         dsatetsicall_cpuc_mmgsdi_data[DSAT_PUCT_LEN];
/* To store the index value of next PLMN stored on the ME */
  int dsatetsicall_copn_plmn_index;
  char etsicall_copn_display_name[MAX_COPN_LEN];
  dsat_s7_info_type dsatetsicall_s7_info;
/* CLIP and COLP bufs are integrated in one as dsat_lineid_buf */
  byte                     dsat_lineid_buf[DSAT_MAX_LINEID_BUF];
  uint32                   dsat_lineid_buf_length;
  byte                     dsat_called_lineid_buf[DSAT_MAX_CALLED_LINEID_BUF];
  uint32                   dsat_called_lineid_buf_length;
/* Theses variables needed because CM_CALL_EVENT_MNG_CALLS_CONF may not precede
   call END/CONNECT events as described in CM documentation. If a waiting 
   call is connected and made active and then MNG_CALLS_CONF occurs, state
   info has been lost and correct course of action may not be clear.
   These variables keeps track of any waiting call at time command
   was issued. */
  boolean etsicall_was_waiting;
  cm_call_id_type was_waiting_call_id;
  /* This variable is needed to filter CM_CALL_EVENT_MNG_CALLS_CONF in cases 
   where a call is originated by ATD when another call is already up */ 
  boolean etsicall_was_call_active;
/* To keep track of calls in effect */
  uint8 etsicall_num_active_calls;
  cm_call_id_type etsicall_active_call_ids[CM_CALL_ID_MAX];
  uint8 etsicall_num_held_calls;
  cm_call_id_type etsicall_held_call_ids[CM_CALL_ID_MAX];
  boolean etsicall_wait_call_present;
  cm_call_id_type etsicall_wait_call_id;
/* To track call before connect state after Call origination */
  dsat_call_info_before_connect call_info_before_connect;
  /* To keep track of the sequence of setting up or receiving the calls 
    (active, held or waiting) as seen by the served subscriber, described
    in 3GPP TS 22.030 section 6.5.5.1 and required by +CHLD command. */
   dsat_seqnum_s_type dsat_seqnum_callid[CM_CALL_ID_MAX];
  /* Mode Info */
  cm_call_mode_info_e_type mode_info[CM_CALL_ID_MAX];

/* Capture the call end status information for +CEER command */
  dsatetsicmif_ces_info_s_type dsatetsicmif_call_end_status;   
#ifdef FEATURE_DSAT_ETSI_DATA
  cm_call_end_e_type dsat_pdp_deactivation_cause;
#endif /* FEATURE_DSAT_ETSI_DATA */
  uint8   dsatact_num_clcc_calls;
/* Counter which says number of active/in conversation voice calls
   based on which we can find out whether conferance call is there or not */
  uint8   dsatact_clcc_mpty_calls;                       
  boolean dsat_abort_call_info_listing;
 /* To store CM call ids of all CS calls */
  dsat_clcc_call_info dsatcmif_clcc_no_call_ids[CM_CALL_ID_MAX];
}dsatetsicall_call_state_info;

#define DSATETSICALL_GET_CIOT_LTE_COPS_PREF(act_pref, dsat_cops_act) \
  if(act_pref == DSAT_COPS_ACT_EUTRAN) \
  { \
    act_pref = dsatetsicall_get_ciot_lte_cops_pref( \
               ph_val->dsat_net_reg_state.cmph_pref.network_ciot_user_lte_pref.lte_op_mode_pref, \
               dsat_cops_act \
               );\
  }

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================

FUNCTION  DSATETSICALL_INIT_CALL

DESCRIPTION
  This function intializes ETSI call handling.

DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void dsatetsicall_init_call ( void );

/*===========================================================================

FUNCTION  DSATETSICALL_SEND_CR_RESULT

DESCRIPTION
  Sends intermediate service reporting result code to TE or PS protocol
  stack based on response routing provided by response router.

  Should be called from mode specific protocol stack at time during
  connection negotiation that speed and quality of service is determined 
  and before compression and connect result code are sent.
 
DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void dsatetsicall_send_cr_result
(
  dsat_cr_service_e_type service
);

/*===========================================================================

FUNCTION  DSATETSICALL_SEND_CR_RESULT_EX

DESCRIPTION
  Sends intermediate service reporting result code to TE or PS protocol
  stack based on response routing provided by response router.

  Should be called from mode specific protocol stack at time during
  connection negotiation that speed and quality of service is determined 
  and before compression and connect result code are sent.
 
DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void dsatetsicall_send_cr_result_ex
(
  dsat_cr_service_e_type service,
  ds3g_siolib_port_e_type port 
);

/*===========================================================================

FUNCTION  DSATETSICALL_SET_CRC_RESULT

DESCRIPTION
  Produces unformatted incoming call indication or GPRS network request
  for PDP context activation result and returns in DSM item.

  If AT+CRC is 0, only RING is returned.
  If AT+CRC is 1, the extendted format RING <type> is returned.
                  <type> is defined in Section 6.11 of 3GPP TS 27.007

DEPENDENCIES
  None
  
RETURN VALUE
  Pointer to DSM item containing unformatted CRC result.
  
SIDE EFFECTS
  None
===========================================================================*/
dsm_item_type *dsatetsicall_set_crc_result
(
  dsat_result_enum_type crc_code
);

/*===========================================================================

FUNCTION DSATETSICALL_VOICE_DIAL_CMD

DESCRIPTION
  Handle atd voice dial commands.  This function is called by 
  dsatact_exec_atd_cmd when dial string has ;.

  This function initiates a voice call and goes to DIALING state.

DEPENDENCIES
  None
  
RETURN VALUE
  DSAT_ASYNC_CMD : If the command succeeds.  CM will call the cmd cb 
                   function later to report status.
  DSAT_ERROR:      If the command fails.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsicall_voice_dial_cmd
(
  const dsati_dial_str_type * dial_str        /* containing dial string 
                                                 and its attribs*/
);


/*===========================================================================

FUNCTION DSATETSICALL_EXEC_CHUP_CMD

DESCRIPTION
  This is the command handler function for +CHUP.

  This function sends request to CM to end current ETSI voice calls for
  action command.

  Return OK for test command.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_ASYNC_CMD : if the action command succeeds.
  DSAT_OK        : if there is no active voice call or if this
                   is a test command.
  DSAT_ERROR     : if the action command fails.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsicall_exec_chup_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATETSICALL_EXEC_CREG_CMD

DESCRIPTION
  This function takes the result from the command line parser and executes 
  it. It executes +CREG and +CGREG commands, which manage the parameters
  for network registration reporting.

DEPENDENCIES
  None
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsicall_exec_creg_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATETSICALL_REPORT_CREG_RESULT

DESCRIPTION
  This function generates the unsolicited result code for registration
  commands +CREG and +CGREG.  The command parameter <n> controls whether
  results are presented or suppressed.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void dsatetsicall_report_creg_result
(
  dsat_creg_report_e_type report_creg
);

/*===========================================================================

FUNCTION DSATETSICALL_CREG_STATE_MACHINE

DESCRIPTION
  This function generates the unsolicited result code for registration
  commands +CREG and +CGREG.  The command parameter <n> controls whether
  results are presented or suppressed.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void dsatetsicall_creg_state_machine
(
  const ds_at_cm_ss_event_type * event_ptr      /* DS event pointer */
);

/*===========================================================================

FUNCTION ETSICALL_MOVE_CALLS

DESCRIPTION
  This function moves calls from one call list to another, excluding call
  excl_call_id if it is something other than CM_CALL_ID_INVALID. It is used by
  event handlers that maintain call state info.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of validation.
  possible values:
    TRUE:   if call id to be excluded was found
    FALSE : if not found

SIDE EFFECTS
  None
  
===========================================================================*/
LOCAL boolean etsicall_move_calls
(
  uint8 *fm_qty,
  cm_call_id_type *fm_list,
  uint8 *to_qty,
  cm_call_id_type *to_list,
  const cm_call_id_type excl_call_id
);

/*===========================================================================

FUNCTION DSATETSICALL_UPDATE_CREG_LAC_CELL_ID

DESCRIPTION
  This function update CELL ID/LAC ID when COPS command is under process.
  It also set CREG pending flag.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void dsatetsicall_update_creg_lac_cell_id
(
  const ds_at_cm_ss_event_type  *event_ptr,        /* DS event pointer */
  dsat_creg_report_e_type       *report_creg
);

/*===========================================================================

FUNCTION DSATETSICALL_RESET_LAST_REPORTED_CREG

DESCRIPTION
  This function reset last reported creg parameter

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void dsatetsicall_reset_last_reported_creg(dsatcmif_servs_state_info  *ph_val);
/*===========================================================================

FUNCTION DSATETSICALL_SET_NET_REG

DESCRIPTION
  This function set last reported domain.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/

void dsatetsicall_set_net_reg
(
  net_reg_domain_e_type   domain ,
  net_reg_status_e_type   value
);
/*===========================================================================

FUNCTION DSATETSICALL_GET_NET_REG

DESCRIPTION
  This function return last reported domain.

DEPENDENCIES
  None
  
RETURN VALUE
  Domain Net registation status

SIDE EFFECTS
  None
  
===========================================================================*/

net_reg_status_e_type  dsatetsicall_get_net_reg
( 
  net_reg_domain_e_type  domain
);

/*===========================================================================

FUNCTION DSATETSICALL_CREG_GET_NET_REG_STATUS

DESCRIPTION
  This function return CREG network registration status based on COPS 
  and CFUN value.

DEPENDENCIES
  None
  
RETURN VALUE
  NET REG STATUS

SIDE EFFECTS
  None
  
===========================================================================*/
net_reg_status_e_type dsatetsicall_creg_get_net_reg_status
(
  const ds_at_cm_ss_event_type * event_ptr       /* DS event pointer */
);

/*===========================================================================

FUNCTION DSATETSICALL_EXEC_CTFR_CMD

DESCRIPTION
  This function takes the result from the command line parser and executes 
  it. It executes +CTFR command, which controls the call deflection (call transfer)
  supplementary service acording to 3GPP TS 22.072 [30]

DEPENDENCIES
  None
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsicall_exec_ctfr_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATETSICALL_EXEC_COPS_CMD

DESCRIPTION
  This function takes the result from the command line parser and executes 
  it. It executes +COPS command, which list available networks and sets
  automatic/manual registration.

DEPENDENCIES
  None
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_ASYNC_CMD : if aynchronous command continues successfully
    DSAT_OK : if it is a success for local execution

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsicall_exec_cops_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATETSICALL_COPS_ABORT_CMD_HANDLER

DESCRIPTION
  This function handles an abort command to terminate asynchronous
  processing for +COPS registration operation.  The network
  registration preferences before the last +COPS comamnd handler
  execution are restored.

DEPENDENCIES
  None

RETURN VALUE
  Value indicating whether or not command should be aborted:
      TRUE if call should be aborted, FALSE otherwise.

SIDE EFFECTS
  Allocates and submits command buffer to DS task

===========================================================================*/
boolean dsatetsicall_cops_abort_cmd_handler
(
  const dsati_cmd_type *cmd_table_entry_ptr  /* Ptr to cmd entry in table. */
);

/*===========================================================================

FUNCTION DSATETSICALL_COPS_ABORT_EVENT_HANDLER

DESCRIPTION
  This function handles teh +COPS abort event handling. Most of the
  processing is done in the abort command handler.  This routine just
  ensures the AT command processor returns to normal processing.

DEPENDENCIES
  None

RETURN VALUE
  Value indicating whether or not command should be aborted:
      TRUE if call should be aborted, FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsicall_cops_abort_event_handler
(
   ds_at_cmd_status_type *cmd_info_ptr
);

/*===========================================================================

FUNCTION DSATETSICALL_COPS_ASYNCH_HANDLER

DESCRIPTION
  This function manages the asnychronous processing of the +COPS
  command. Numerous acynch queries are required depnding on the +COPS
  syntax.  For test syntax, its is necessary to get available and
  poreferred networks from CM.  The home PLMN is determined from the
  GSDI IMSI query if necessary.  For read syntax, current state of
  phone is requried from CM.

DEPENDENCIES
  None
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_CMD_ERR_RSP: if there was a problem in lower layers
    DSAT_ASYNC_CMD : if aynchronous command continues successfully

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsicall_cops_async_handler
(
  dsat_cops_async_state_e_type  cmd_state,    /* Processing state    */
  ds_at_cm_ph_event_type       *ph_event      /* PH event pointer */
);


/*===========================================================================

FUNCTION DSATETSICALL_CALL_EVENT_INCOM

DESCRIPTION
  This function is called by dsatcmif_cm_call_event_handler when CM reports 
  the CM_CALL_EVENT_INCOM event and adds the waiting call id to our call state
  info.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_CMD_ERR_RSP : if CM reports failure.
  DSAT_ASYNC_EVENT:  otherwise.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsicall_call_event_incom
(
  ds_at_cm_call_event_type*   call_event    /* Call Event pointer         */
);

#ifdef FEATURE_DSAT_ATCOP_VEND
/*===========================================================================

FUNCTION DSATETSICALL_QCCELLSCAN_ASYNCH_HANDLER

DESCRIPTION
  This function manages the asnychronous processing of the $QCCELLSCAN
  command. Numerous acynch queries are required depnding on the $QCCELLSCAN

DEPENDENCIES
  None
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_CMD_ERR_RSP: if there was a problem in lower layers
    DSAT_ASYNC_CMD : if aynchronous command continues successfully

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsicall_qccellscan_async_handler
(
  ds_at_cm_ph_event_type       *ph_event      /* PH event pointer */
);
#endif
/*===========================================================================

FUNCTION DSATETSICALL_REPORT_COPS_RESULT

DESCRIPTION
  This function generates the response for network query command +COPS
  read syntax.  The order of networks shown in the respose is dictated
  in TS 27.007 section 7.3: home, SIM (preferred), other.  Only those
  networks "present" are to be reported.

DEPENDENCIES
  None
  
RETURN VALUE
  DSAT_CMD_ERR_RSP : if CM reports failure
  DSAT_OK : if it is a success for local execution
  DSAT_ERROR : if there was any problem in executing the command
  
SIDE EFFECTS
  May append further DSM items to original result buffer.
  
===========================================================================*/
dsat_result_enum_type dsatetsicall_report_cops_result
(
  ds_at_mmgsdi_oper_name_type *oper_name /* Operator name pointer */
);

/*===========================================================================

FUNCTION DSATETSICALL_EXEC_CLCK_CMD

DESCRIPTION
  This function takes the result from the command line parser and executes 
  it. It executes +CLCK command, which controls the facility lock
  supplementary service acording to 3GPP TS 22.088

DEPENDENCIES
  None
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR : if there was an problem in executing the command
    DSAT_CMD_ERR_RSP : if there was a problem during aynchronous execution
    DSAT_ASYNC_CMD : if aynchronous command continues successfully
    DSAT_OK : if it is a success for local execution

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsicall_exec_clck_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATETSICALL_EXEC_CPWD_CMD

DESCRIPTION
  This function takes the result from the command line parser and executes 
  it. It executes +CPWD command, which sets new passwords for facility lock
  supplementary service.

DEPENDENCIES
  None
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_ASYNC_CMD : if aynchronous command continues successfully
    DSAT_OK : if it is a success for local execution

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsicall_exec_cpwd_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATETSICALL_EXEC_CUSD_CMD

DESCRIPTION
  This function takes the result from the command line parser and executes 
  it. It executes +CUSD command, which allows control of Unstructured
  Supplementary Service Data (USSD).

DEPENDENCIES
  None
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_ASYNC_CMD : if aynchronous command continues successfully
    DSAT_OK : if it is a success for local execution

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsicall_exec_cusd_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);


/*===========================================================================

FUNCTION DSATETSICALL_CUSD_RESPONSE_FMT

DESCRIPTION
  This function generates the response for the +CUSD read & test
  commands.  The &V command respose is handled as well.  Data is
  extracted from data structure and formatted according to parameter flag.

DEPENDENCIES
  None

RETURN VALUE
  Returns number of characters in response buffer.

SIDE EFFECTS
  None

===========================================================================*/
dsat_num_item_type dsatetsicall_cusd_response_fmt
(
  const dsati_cmd_type   *parse_table,    /*  Ptr to cmd in parse table   */
  dsm_item_type * const  *out_buff_ptr,   /*  Place to put response       */
  multi_index_fmt_e_type  format          /*  Flag for format type        */
);

/*===========================================================================

FUNCTION DSATETSICALL_SUPS_CMD_CB_HANDLER

DESCRIPTION
  This function is the handler function for the CM SupS call commands

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR :      if there was any problem in execution.
    DSAT_ASYNC_CMD :  if it is a success and asynch cmd continues.
    DSAT_OK :         if it is a success and asynch cmd done. 

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsicall_sups_cmd_cb_handler
(
  ds_at_cm_call_cmd_type *call_cmd /* Call command pointer */
);

/*===========================================================================

FUNCTION ETSICALL_SEARCH_CALL_LIST

DESCRIPTION
  This function searches for the call id in the call list provided, 
  deleting it if remove is TRUE and returning TRUE if call id was found. 
  It is used by event handlers that maintain call state info.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of validation.
  possible values:
    TRUE :  if call id was found
    FALSE : not found.

SIDE EFFECTS
  Call id is removed from list per boolean remove 
  
===========================================================================*/
boolean etsicall_search_call_list
(
  uint8 *call_qty,
  cm_call_id_type *call_list,
  const cm_call_id_type call_id,
  boolean rmv_call_id
);

/*===========================================================================

FUNCTION DSATETSICALL_CALL_EVENT_CONNECT_EX

DESCRIPTION
  This function is called by dsatcmif_cm_call_event_handler when CM reports 
  the CM_CALL_EVENT_CONNECT event and adds the call id connected to the active
  calls list of our state info.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_CMD_ERR_RSP : if CM reports failure.
  DSAT_ASYNC_EVENT:  otherwise.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsicall_call_event_connect_ex
(
  ds_at_cm_call_event_type*  call_event     /* call event pointer         */
);

/*===========================================================================

FUNCTION DSATETSICALL_CALL_EVENT_CONNECT

DESCRIPTION
  This function is called by dsatcmif_cm_call_event_handler when CM reports 
  the CM_CALL_EVENT_CONNECT event and adds the call id connected to the active
  calls list of our state info.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_CMD_ERR_RSP : if CM reports failure.
  DSAT_ASYNC_EVENT:  otherwise.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsicall_call_event_connect
(
  ds_at_cm_call_event_type*  call_event,     /* call event pointer         */
  dsat_result_enum_type      result
);

/*===========================================================================

FUNCTION DSATETSICALL_CALL_EVENT_END_EX

DESCRIPTION
  This function is called by dsatcmif_cm_call_event_handler when CM reports 
  the CM_CALL_EVENT_MNG_CALLS_CONF event and removes the call id ended from 
  our call state info.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_CMD_ERR_RSP : if CM reports failure.
  DSAT_ASYNC_EVENT:  otherwise.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsicall_call_event_end_ex
(
  ds_at_cm_call_event_type *call_event /* Call event pointer */
);

/*===========================================================================

FUNCTION DSATETSICALL_CALL_EVENT_END

DESCRIPTION
  This function is called by dsatcmif_cm_call_event_handler when CM reports 
  the CM_CALL_EVENT_MNG_CALLS_CONF event and removes the call id ended from 
  our call state info.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_CMD_ERR_RSP : if CM reports failure.
  DSAT_ASYNC_EVENT:  otherwise.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsicall_call_event_end
(
  ds_at_cm_call_event_type *call_event /* Call event pointer */
);


/*===========================================================================

FUNCTION  DSATETSICALL_REPORT_SS_LINEID_STATUS

DESCRIPTION
  This function reports the COLP/CLIP supplementary service status
  received from Call Manager.

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the execution.
  Possible values:
    DSAT_ERROR :    if there was any problem in execution.
    DSAT_OK :       if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsicall_report_ss_lineid_status
(
  ss_operation_code_T       * op_code,    /* Interrogation operation code */
  const ie_ss_status_T      * ss_info_ptr, /* SS info pointer */
  boolean                     in_service
);

/*===========================================================================

FUNCTION  DSATETSICALL_EXEC_SS_ABORT_HANDLER

DESCRIPTION
  This function is the abort handler function for the SS functionality. 
  
DEPENDENCIES
  None

RETURN VALUE
  TRUE - meaning to abort the delay execution.

SIDE EFFECTS
  None

===========================================================================*/
boolean dsatetsicall_exec_ss_abort_handler
(
  const struct dsati_cmd_struct *ptr
);


/*===========================================================================

FUNCTION DSATETSICALL_SS_ABORT_EVENT_HANDLER

DESCRIPTION
  This function handles +CLIP/+COLP/+CLIR abort event handling.We set the SS 
  service to invalid state and return. This routine just ensures the AT 
  command processor returns to normal processing.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_ERROR : as the function is aborted

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsicall_ss_abort_event_handler
(
  ds_at_cmd_status_type *cmd_info_ptr
);

#ifndef FEATURE_DSAT_CUST
/*===========================================================================

FUNCTION DSATETSICALL_EXEC_COPN_CMD

DESCRIPTION
  This function takes the result from the command line parser and
  executes it. It executes +COPN command, which displays list of 
  operator names from the ME. This command is implemented as 
  asynchronous command to make it abortable

DEPENDENCIES
  None
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success for local execution

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsicall_exec_copn_cmd
(
  dsat_mode_enum_type       mode,         /*  AT command mode:            */
  const dsati_cmd_type     *parse_table,  /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type            *res_buff_ptr  /*  Place to put response       */
);
#endif /* FEATURE_DSAT_CUST */

/*===========================================================================

FUNCTION DSATETSICALL_DISPLAY_PLMN_LIST

DESCRIPTION
  This function displays list of operator names from the ME.

DEPENDENCIES
  None
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success for local execution

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsicall_display_plmn_list
(
  void
);

#ifdef FEATURE_MMGSDI
/*===========================================================================

FUNCTION  DSATETSICALL_DISPLAY_PREF_NETWORKS

DESCRIPTION
  This function displays valid preffered PLMNs in the given index range.
  
DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :    if there was any problem in executing the command
    DSAT_OK :       if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsicall_display_pref_networks
(
  ds_at_mmgsdi_oper_name_type *oper_name_ptr /* Operator name pointer */
);
#endif /* FEATURE_MMGSDI */

/*===========================================================================

FUNCTION DSATETSICALL_PLMN_ABORT_CMD_HANDLER

DESCRIPTION
  This function handles an abort command to terminate listing of both 
  preffered PLMN and PLMN list stored on the ME.

DEPENDENCIES
  None

RETURN VALUE
  Value indicating whether or not command should be aborted:
      TRUE if call should be aborted, FALSE otherwise.

===========================================================================*/
boolean dsatetsicall_plmn_abort_cmd_handler
(
  const dsati_cmd_type *cmd_table_entry_ptr  /* Ptr to cmd entry in table. */
);

/*===========================================================================

FUNCTION DSATETSICALL_COPN_ABORT_EVENT_HANDLER

DESCRIPTION
  This function handles teh +COPN abort event handling. Most of the
  processing is done in the abort command handler.  This routine just
  ensures the AT command processor returns to normal processing.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_ERROR : As command is aborted

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsicall_copn_abort_event_handler
(
  ds_at_cmd_status_type *cmd_info_ptr
);


/*===========================================================================

FUNCTION DSATETSICALL_EXEC_CLIR_CMD

DESCRIPTION
  This function takes the result from the command line parser and executes 
  it. It executes +COLP command, which enables/disables the restriction of the 
  display of calling line identification.

DEPENDENCIES
  None
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsicall_exec_clir_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*========================================================================

FUNCTION  DSATETSICALL_REPORT_CLIR_STATUS

DESCRIPTION
  This function reports the CLIR supplementary service interrogation status
  received from Call Manager.

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the execution.
  Possible values:
    DSAT_ERROR :    if there was any problem in execution.
    DSAT_OK :       if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsicall_report_clir_status
(
  ds_at_cm_sups_event_type *sups_event_ptr /* SUPS event pointer */
);
#ifdef FEATURE_DSAT_LTE
/*===========================================================================
FUNCTION: DSATETSICALL_EXEC_CVMOD_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at+cvmod command.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsicall_exec_cvmod_cmd
(
  dsat_mode_enum_type mode,               /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr             /*  Place to put response       */
);
#endif /* FEATURE_DSAT_LTE*/

/*===========================================================================

FUNCTION  DSATETSICALL_START_DTMF

DESCRIPTION
  This function process the 'write' command of +vts.

DEPENDENCIES
  A active call

RETURN VALUE
  Returns an enum that describes the result of the execution.
  Possible values:
    DSAT_ERROR :    if there was any problem in execution.
    DSAT_ASYNC_CMD : Intermediate Response
    DSAT_OK :       if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsicall_start_dtmf 
(
  const tokens_struct_type *tok_ptr 
);

#ifdef FEATURE_MMGSDI

/*===========================================================================
FUNCTION: DSATETSICALL_COPS_MMGSDI_CMD_HANDLER

DESCRIPTION
  This function handles at+cops command.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsicall_cops_mmgsdi_handler
(
   ds_at_mmgsdi_plmn_info_type  *plmn_info_ptr
);

/*===========================================================================
FUNCTION: DSATETSICALL_COPS_CMD_CB_FUNC

DESCRIPTION
  This function handles at+cops command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsicall_cops_cmd_cb_func
(
  uint32                                   client_ref,
  mmgsdi_se13_plmn_static_info_type       *se13_plmn_info_ptr,
  uint16                                   plmn_id
);

#endif /*FEATURE_MMGSDI*/

#ifdef FEATURE_DATA_ETSI_PIN
/*===========================================================================

FUNCTION  DSATETSICALL_EXEC_VTS_CMD

DESCRIPTION
  This function generates a dtmf during an ongoing call.

DEPENDENCIES
  A active call

RETURN VALUE
  Returns an enum that describes the result of the execution.
  Possible values:
    DSAT_ERROR :    if there was any problem in execution.
    DSAT_ASYNC_CMD : Intermediate Response
    DSAT_OK :       if it is a success.

SIDE EFFECTS
  None

===========================================================================*/

dsat_result_enum_type dsatetsicall_exec_vts_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION  dsatetsicall_inband_dtmf_timer_expired_handler

DESCRIPTION
  Handler Function for DTMF Timer.

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the execution.
  Possible values:
    DSAT_ERROR :    if there was any problem in execution.
    DSAT_ASYNC_EVENT : Intermediate Response
    DSAT_OK :       if it is a success.

SIDE EFFECTS
  None

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsicall_inband_dtmf_timer_expired_handler 
(
  ds_command_type   * cmd_ptr         /* DS Command pointer		   */
);

#endif /* FEATURE_DATA_ETSI_PIN */

/*===========================================================================
FUNCTION: DSATETSICALL_PROCESS_PLMN_INFO

DESCRIPTION
  This function handles at+cops command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsicall_process_plmn_info ( void );

/*===========================================================================
FUNCTION: DSATETSICALL_GET_CIOT_LTE_COPS_PREF

DESCRIPTION
  This function maps CM CIOT LTE preference to +COPS ACT value.

DEPENDENCIES
  None

RETURN VALUE
  +COPS ACT value

SIDE EFFECTS
  None

===========================================================================*/
dsat_cops_act_e_type dsatetsicall_get_ciot_lte_cops_pref ( 
  cm_ciot_lte_op_mode_e_type cm_mode,
  dsat_cops_act_e_type       at_mode
);

#endif /* FEATURE_DSAT_ETSI_MODE */
#endif /* DSATETSICALL_H */
