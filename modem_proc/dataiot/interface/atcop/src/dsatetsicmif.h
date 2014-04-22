#ifndef DSATETSICMIF_H
#define DSATETSICMIF_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                D A T A   S E R V I C E S

                A T   C O M M A N D   
                
                ( E T S I  C A L L  M A N A G E R  I N T E R F A C E )
                
                P R O C E S S I N G

                E X T E R N A L   H E A D E R   F I L E


GENERAL DESCRIPTION
  This file contains the definitions of data structures, defined and
  enumerated constants and function prototypes required for the
  call manager interface.

INITIALIZATION AND SEQUENCING REQUIREMENTS

   Copyright (c) 2002 - 2017 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  L:/src/asw/MSM5200/data/vcs/dsatetsicmif.h_v   1.1   12 Jul 2002 10:09:44   randrew  $
  $Header: //components/rel/dataiot.mpss/2.0/interface/atcop/src/dsatetsicmif.h#3 $ $DateTime: 2020/10/19 05:11:36 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ---------------------------------------------------------
1/19/17    skc     Added enhancement on CHLD and CLCC for VoLTE MPTY Call.
08/12/14   pg      Modified DS Profile APIs to per subs.
08/01/14   sc      Added support for linked MO and MT USSD sessions.
06/27/14   sc      Added support for Dynamic ATCoP.
02/15/14   pg      Added support for SGLTE+G.
06/05/13   tk      ATCoP changes for SGLTE support on Dime Plus.
05/16/13   sk      $QCCOPS command for Extension of network search.
01/04/13   tk      Fixed +CPAS and +CEER issues.
11/30/12   tk      ATCoP changes for Triton DSDA.
06/15/12   sk      Fixed CEER issue.
03/07/12   sk      Made CLCC common for all modes.
02/17/12   sk      Migrating Data related AT commands interface to unified MH.
12/26/11   ad      Added Support the New Call Control Interface in Call Manager.
01/19/12   sk      Feature cleanup.
06/04/10   bs      MMGSDI SESSION Migration.
01/15/10   ua      Moving out send_response_to_te to common file. 
12/15/09   nc      Featurisation changes for LTE.
04/20/09   ua      Fixed issues related with call commands followed by +CIND?
03/04/09   sa      AU level CMI modifications.
11/04/07   pp      Lint low - fixes.
02/21/06   sa      Merged changes from Sirius Branch
                   -Removed prototypes for activate & deactivate SS service.
01/16/06   ua      Added support for +CDIP command.
07/29/05   ar      Migrate CM SS events to watermark queue. 
07/25/05   snb     Make function dsatetsicmif_find_context_state_from_call_id()
                   external.
04/29/05   ar      Add support for capturing call end status.
04/05/05   iv      Modified +CLCC to support CS data calls.
04/04/05   ar      Added support for +CIND command.
03/29/05   ar      Adjusted change_network_registration() description.
02/03/05   tkk     Added prototypes for SS activate and deactivate.
03/19/04   snb     Add external function to display +CCWA unsolicited result 
                   code.
01/09/04   ar      Add dsatetsicmif_deactivate_all_contexts() prototype
01/05/05   iv      Added supoport for +CLCC command
11/11/04   tkk     Added prototypes for dsatetsicmif_report_cxxp_result 
12/07/04   ar      Add support for +COPS command.
10/22/03   ar      Move Call Manager PH & SS API calls from ETSI to COMMON.
10/15/04   ar      Add Advice of Charge support.
09/23/03   snb     Add support for PS attach timeout failure
07/31/03   ar      Add address type constants.
03/07/03   ar      Remove FEATURE_DATA_ETSI_SUPSERV wrappers
01/16/03   ar      Added dsatetsicmif_send_respose_to_te() prototype
01/10/03   sb      Added support fot +CGATT command
11/21/01   ar      Added support for +CLCK command
10/31/02   ar      Added support for +CCFC command
10/28/02   sb      Opened Voice call related handlers (moved to a common file)
04/18/02   ar      Created module.

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"


/* this file should be included only in GSM or WCDMA mode */
#ifdef FEATURE_DSAT_ETSI_MODE

#include "dsat_v.h"
#include "dsati.h"
#include "dsatcmif.h"
#include "dsatetsicall.h"
#ifdef FEATURE_DSAT_ETSI_DATA
#include "dsatetsipkt.h"
#endif /* FEATURE_DSAT_ETSI_DATA */

#include "mcs_sups.h"

/*===========================================================================

                        PUBLIC DATA DECLARATIONS

===========================================================================*/

#define DSAT_TA_UNKNOWN       129 /* 0x80|CM_TON_UNKNOWN      |CM_NPI_ISDN */
#define DSAT_TA_INTERNATIONAL 145 /* 0x80|CM_TON_INTERNATIONAL|CM_NPI_ISDN */
#define DSAT_TA_INTER_PREFIX  '+' /* ETSI international call dial prefix */
#define E_SUCCESS              0  /*0 is E_SUCCESS for MCS  */

/* Be align with variable 'dial_prefix' in function dsatact_check_dial_prefix()
   If 'dial_prefix' is updated with new entry, new enum should be added below and
   index must match */
typedef enum dsat_dial_prefix
{
  DSAT_DIAL_PREFIX_NONE = -1,
  DSAT_DIAL_PREFIX_CLIR_SUPPRESSION,      /* Supplementary dial prefix *31# */
  DSAT_DIAL_PREFIX_CLIR_INVOCATION,       /* Supplementary dial prefix #31# */
  DSAT_DIAL_PREFIX_MAX
}dsat_dial_prefix_e_type;

typedef enum clcc_state
{
  DSAT_CLCC_STATE_ACTIVE   = 0,
  DSAT_CLCC_STATE_HELD     = 1,
  DSAT_CLCC_STATE_DIALING  = 2,
  DSAT_CLCC_STATE_ALERTING = 3,
  DSAT_CLCC_STATE_INCOMING = 4,
  DSAT_CLCC_STATE_WAITING  = 5,
  DSAT_CLCC_STATE_MAX
}dsat_clcc_state_e_type;

typedef enum clcc_mode
{
  DSAT_CLCC_MODE_VOICE = 0,
  DSAT_CLCC_MODE_DATA  = 1,
  DSAT_CLCC_MODE_FAX   = 2
}dsat_clcc_mode_e_type;

typedef enum clcc_dir
{
  DSAT_CLCC_DIRECTION_MO = 0,
  DSAT_CLCC_DIRECTION_MT  = 1
}dsat_clcc_dir_e_type;

#define DSAT_CLCC_MPTY_CALL        1
/* Sups management data */
typedef struct supserv_cm_op_s
{
  ss_user_data_s_type       user_data;   /* User entered parameters         */
  supserv_cm_parm_u_type    cm_parms;    /* CM API parameters               */
  uint16                    curr_class;  /* Track current class             */ 
  uint16                    done_class;  /* Track classes already processed */
  uint8                     sc_code;     /* Service code for current class  */
  boolean                   pending_msg; /* Flag pending msg                */    
boolean                   pending_rsp; /* Flag pending response           */    
  boolean                   abort_cmd;   /* Flag command abort              */    
  cusd_state_e_type         ussd_state;  /* USSD state tracking             */
  cusd_session_info_s_type  cusd_session_info[DSAT_CUSD_SESSION_ID_MAX];
                            /* CUSD Session Info Array */
  boolean                   pending_clck_pwd_ind; /* Flag for pending
                                              password indication for +CLCK */
} supserv_cm_op_s_type;

typedef struct
{
  supserv_cm_op_s_type  dsat_ss_cm_data;
  ss_operation_code_T         dsat_interr_ss_service;
  boolean                     dsat_ss_abort_status;
}dsatetsicmif_sups_state_info;

/*-------------------------------------------------------------------------
            PUBLIC FUNCTION DECLARATIONS
-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION  DSATETSICMIF_INITIATE_VOICE_CALL

DESCRIPTION
  This function requests CM to start a voice call.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    FALSE :    if there was any problem in executing the command
    TRUE  :    if it succeeds.

SIDE EFFECTS
  None

===========================================================================*/
boolean dsatetsicmif_initiate_voice_call
(
  /* contains the dialing number and its attributes */
  const dsati_dial_str_type     * dial_str,

  /* returns the call id which is returned by cm */
  uint8 * call_id_ptr
);


/*===========================================================================
FUNCTION  ETSICMIF_SEND_SUPS_ERROR

DESCRIPTION
  Function to send sups error.
  
DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL dsat_result_enum_type etsicmif_send_sups_error
(
  dsat_cme_error_e_type    error_code
);
/*===========================================================================

FUNCTION DSATETSICMIF_SUPS_CLASS_ACTION

DESCRIPTION
  This function intiates the action in Call Manager to perform supplemental
  service commands for classes.  The user parameters stored in regional variable
  dsat_ss_cm_data are expected to be validated. This routine populates
  the Call Manager API structures.
  
DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the execution.
  Possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_ASYNC_CMD : if it is a success.

SIDE EFFECTS
  Command is sent to Call Manager
  
===========================================================================*/
dsat_result_enum_type dsatetsicmif_sups_class_action
(
  boolean                    first_call       /* Flag for recursive loop   */
);


/*===========================================================================

FUNCTION DSATETSICMIF_SUPS_CHANGE_PASSWORD

DESCRIPTION
  This function intiates the change of supplementary services facility
  password. The user parameters stored in regional variable
  dsat_ss_cm_data are expected to be validated. This routine populates
  the Call Manager API structures.
  
DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the execution.
  Possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_ASYNC_CMD : if it is a success.

SIDE EFFECTS
  Command is sent to Call Manager
  
===========================================================================*/
dsat_result_enum_type dsatetsicmif_sups_change_password (void);

/*===========================================================================

FUNCTION DSATETSICMIF_SET_CUSD_STATE

DESCRIPTION
  This function is used to set CUSD state.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
void dsatetsicmif_set_cusd_state
(
  supserv_cm_op_s_type          *ss_cm_data_ptr,
  cusd_state_e_type              ussd_state,
  uint8                          invoke_id
);

/*===========================================================================

FUNCTION ETSICMIF_GET_CUSD_INFO

DESCRIPTION
  This function is used to get CUSD info.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
void etsicmif_get_cusd_info
(
  supserv_cm_op_s_type          *ss_cm_data_ptr,
  uint8                         *invoke_id_ptr,
  uint8                         *oper_type_ptr
);

/*===========================================================================

FUNCTION DSATETSICMIF_SUPS_PROCESS_USSD

DESCRIPTION
  This function intiates the action in Call Manager to perform supplemental
  service commands for Unstructured Supplementary Service Data (USSD).
  The user parameters stored in regional variable dsat_ss_cm_data are expected
  to be validated. This routine populates the Call Manager API structures.
  
DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the execution.
  Possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_ASYNC_CMD : if it is a success.

SIDE EFFECTS
  Command is sent to Call Manager
  
===========================================================================*/
dsat_result_enum_type dsatetsicmif_sups_process_ussd (void);


/*===========================================================================
FUNCTION  ETSICMIF_REPORT_USSD_RESULT

DESCRIPTION
  This function generates the unsolicited result code for USSD commands.
  The +CUSD command <n> paramater controls whether results are presented
  or suppressed.

  If TE indications are blocked, the network is sent a busy response. 
  If the data coding scheme is not supported, the network is sent an 
  unknown alphabet response.
  
DEPENDENCIES
  None

RETURNS
  Returns an enum that describes the result of the execution.
  Possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_ASYNC_EVENT : if it is a success.

SIDE EFFECTS
  Send command to Call Manager

===========================================================================*/
dsat_result_enum_type etsicmif_report_ussd_result
(
  const ds_at_cm_sups_event_type *sups_event_ptr, /* SUPS event pointer */
  const cusd_result_e_type        result_code,     /* Result code */
  boolean                        *send_ack
);

/*===========================================================================
FUNCTION  ETSICMIF_SUPS_SEND_USSD_NOTIFY_RESPONSE

DESCRIPTION
  This function requests Call Manager to send the mobile originated
  USSD notify response to the network.  This response message is sent
  by the MS after an USSD notify indication message has been received.
  If the error present flag is FALSE, no error code is specified in
  the message payload.
  
DEPENDENCIES
  None

RETURNS
  Returns an enum that describes the result of the execution.
  Possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_ASYNC_CMD : if it is a success.

SIDE EFFECTS
  Send command to Call Manager

===========================================================================*/
dsat_result_enum_type etsicmif_sups_send_ussd_notify_response
(
  boolean                error_present,         /* Error present */
  byte                   error_code             /* Error code */
);

/*===========================================================================
FUNCTION  ETSICMIF_SUPS_CMD_CB_FUNC

DESCRIPTION
  CM supplementary services command callback function

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  Adds command in DS command buffer

===========================================================================*/
void etsicmif_sups_cmd_cb_func 
(
  void                         *data_ptr,         /* Data block pointer    */
  cm_sups_cmd_e_type            cmd,              /* Command ID            */
  cm_sups_cmd_err_e_type        cmd_err           /* Command error code    */
);

/*===========================================================================

FUNCTION  DSATETSICMIF_CM_SUPS_CMD_HANDLER_EX

DESCRIPTION
  This function is the handler function for the CM supplementary service
  commands.

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR :      if there was any problem in execution.
    DSAT_ASYNC_CMD :  if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsicmif_cm_sups_cmd_handler_ex
(
  ds_command_type         * cmd_ptr              /* DS Command pointer         */
);

/*===========================================================================

FUNCTION  DSATETSICMIF_CM_SUPS_CMD_HANDLER

DESCRIPTION
  This function is the handler function for the CM supplementary service
  commands.

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR :      if there was any problem in execution.
    DSAT_ASYNC_CMD :  if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsicmif_cm_sups_cmd_handler
(
  ds_at_cm_sups_cmd_type *sups_cmd, /* SUPS command pointer */
  dsat_result_enum_type   result
);

/*===========================================================================
FUNCTION  DSATETSICMIF_SUPS_EVENT_CB_FUNC

DESCRIPTION
  CM supplementary service event callback function

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  Adds command in DS command buffer

===========================================================================*/
void dsatetsicmif_sups_event_cb_func 
(
  cm_sups_event_e_type             event,          /* Event ID              */
  const cm_sups_info_s_type       *event_ptr      /* Pointer to Event info */
);

/*===========================================================================

FUNCTION  DSATETSICMIF_CM_SUPS_EVENT_HANDLER_EX

DESCRIPTION
  This function is the handler function for the CM supplementary service
  related events

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the execution.
  Possible values:
    DSAT_ERROR :       if there was any problem in execution
    DSAT_ASYNC_EVENT : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsicmif_cm_sups_event_handler_ex
(
  ds_command_type         * cmd_ptr              /* DS Command pointer         */
);

/*===========================================================================

FUNCTION  DSATETSICMIF_CM_SUPS_EVENT_HANDLER

DESCRIPTION
  This function is the handler function for the CM supplementary service
  related events

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the execution.
  Possible values:
    DSAT_ERROR :       if there was any problem in execution
    DSAT_ASYNC_EVENT : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsicmif_cm_sups_event_handler
(
  ds_at_cm_sups_event_type *sups_event, /* SUPS event pointer */
  dsat_result_enum_type     result
);

/*===========================================================================

FUNCTION DSATETSICMIF_SUPS_ABORT_HANDLER

DESCRIPTION
  This function does nothing but return a result to indicate the
  supplementary service command should be aborted.  It should be
  installed in the command table as the abort handler function for
  abortable supplementary service commands.

DEPENDENCIES
  None

RETURN VALUE
  Value indicating whether or not call should be aborted:
      TRUE if call should be aborted, FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
boolean dsatetsicmif_sups_abort_handler
(
  const dsati_cmd_type *cmd_table_entry_ptr  /* Ptr to cmd entry in table. */
);


/*===========================================================================
FUNCTION  DSATETSICMIF_GEN_CMEE_ERROR

DESCRIPTION
  Generate the passed +CME error response.  One DSM item is allocated for
  response buffer.

DEPENDENCIES
  None

RETURNS
  Returns an enum that describes the result of the execution.
    DSAT_CMD_ERR_RSP :   if +CME error respose was generated

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsicmif_gen_cmee_error 
(
  dsat_cme_error_e_type   error_code                      /* Error ID      */
);


/*===========================================================================

FUNCTION  DSATETSICMIF_QUERY_NETWORKS

DESCRIPTION
  This function invokes the CM API to get either the list of available
  networks from the access stratum or the preferred network list from
  the SIM/USIM.  Handling of the results is done asynchronously in the
  Phone command event handler for the CM_PH_EVENT_AVAILABLE_NETWORKS_CONF event.

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the execution.
  Possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_ASYNC_CMD : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsicmif_query_networks
(
  cm_network_list_type_e_type net_type
);


/*===========================================================================

FUNCTION   DSATETSICMIF_CHANGE_NETWORK_REGISTRATION

DESCRIPTION
  This function invokes the CM API to change the network registration
  preference.  The passed in mode, PLMN, and access technology
  parameters are used to configure the call to the CM API.  For manual
  or automatic network registration, a single API call is required.
  For automatic if manual fails, a second call to the CM API is
  required once the NO_SRV indication is reported.  The aborting flag
  indicates whether the state machine should be updated.

  If the requested mode is AUTO and matches the current preference, no
  action is taken as the lower layers may do unnecessary detach &
  attach cycle.

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the execution.
  Possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if no action was taken.
    DSAT_ASYNC_CMD : if command was sent to CM API successfully.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsicmif_change_network_registration
(
  dsat_cops_mode_e_type       net_mode,
  sys_plmn_id_s_type *        plmn_ptr,
  dsat_cops_act_e_type        net_act
);

/*===========================================================================

FUNCTION   DSATETSICMIF_LOOKUP_CM_RAT_PREF

DESCRIPTION
  This function performs lookup to/from the Call Manager radio access
  technology preference based on the input ATCOP AcT parameter value.

DEPENDENCIES
  None

RETURN VALUE
  TRUE on sucessful lookup; FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean dsatetsicmif_lookup_cm_rat_pref
(
  boolean                to_cm_pref,
  dsat_cops_act_e_type  *net_act_ptr,
  cm_mode_pref_e_type   *cm_rat_ptr
);

#ifdef FEATURE_DSAT_ETSI_DATA

#ifdef FEATURE_ETSI_ATTACH
/*===========================================================================

FUNCTION  DSATETSICMIF_ATTACH_PS

DESCRIPTION
  Wrapper function to inform CM to attach to a PS domain. Called from CGATT
  command

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
dsat_result_enum_type dsatetsicmif_attach_ps ( void );

/*===========================================================================

FUNCTION  DSATETSICMIF_DETACH_PS

DESCRIPTION
  Wrapper function to inform CM to detach from the PS domain. Called from 
  CGATT command

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
dsat_result_enum_type dsatetsicmif_detach_ps ( void );
#endif /* FEATURE_ETSI_ATTACH */

/*===========================================================================

FUNCTION DSATETSICMIF_INIT_PDP_CONNECT_STATE

DESCRIPTION
  This function intializes the PDP context profile connection state
  information.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void dsatetsicmif_init_pdp_connect_state ( void );

/*===========================================================================

FUNCTION  DSATETSICMIF_DEACTIVATE_ALL_CONTEXTS

DESCRIPTION
  This function initiates PDP context deactivation for all contexts.
  This should be called when network domain changes to PS detach, which
  must result in all active contexts becoming deactivated according
  to 3GPP TS 27.007 section 10.1.9

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the execution.
  Possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_ASYNC_CMD : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsicmif_deactivate_all_contexts 
(
  void
);

/*===========================================================================

FUNCTION  DSATETSICMIF_CONTEXT_ACTIVATION_ATZ_CB

DESCRIPTION
  This function intiates PDP context deactivation in response to ATZ command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatetsicmif_context_activation_atz_cb ( void );
          
#endif /* FEATURE_DSAT_ETSI_DATA */

/*===========================================================================

FUNCTION  DSATETSICMIF_PROCESS_CM_MM_INFO_DATA_EX

DESCRIPTION
  This function processes the Call Manager MM information data to display 
  the timezone status to TE

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
dsat_result_enum_type dsatetsicmif_process_cm_mm_info_data_ex
(
  const dsat_sys_mm_information_s_type *mm_info_ptr,        /* MM Info pointer */
  ds_at_ss_info_s_type                 *event_info
);

/*===========================================================================

FUNCTION  DSATETSICMIF_PROCESS_CM_MM_INFO_DATA

DESCRIPTION
  This function processes the Call Manager MM information data to display 
  the timezone status to TE

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
dsat_result_enum_type dsatetsicmif_process_cm_mm_info_data
(
  ds_at_ss_info_s_type   *event_info
);

/*===========================================================================

FUNCTION  DSATETSICMIF_PROCESS_CM_MM_INFO_NET_REG

DESCRIPTION
  This function processes the Call Manager MM information to set
  network registration information.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatetsicmif_process_cm_mm_info_net_reg
(
  const dsat_sys_mm_information_s_type *mm_info_ptr        /* MM Info pointer */
);

/*===========================================================================
FUNCTION DSATETSICMIF_CLCC_ABORT_EVENT_HANDLER

DESCRIPTION
  This function handles the +CLCC abort event handling. Most of the
  processing is done in the abort command handler.  This routine just
  ensures the AT command processor returns to normal processing.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_ERROR : As command is aborted

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsicmif_clcc_abort_event_handler
(
  ds_at_cmd_status_type *cmd_info_ptr
);


/*==========================================================================
FUNCTION  DSATETSICMIF_PROCESS_RSSI_EVENT

DESCRIPTION
  This function processes the CM_SS_EVENT_RSSI from Call Manager.  It
  updates the +CIND indicator value.

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the execution.
  Possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsicmif_process_rssi_event
(
  const ds_at_cm_ss_event_type * event_ptr      /* Event structure         */
);


/*==========================================================================
FUNCTION  DSATETSICMIF_CAPTURE_CALL_END_STATUS

DESCRIPTION
  This function captures the call end status information from CM
  events.  The events contianing the informaiton are CM_CALL_EVENT_END
  and CM_CALL_EVENT_CALL_CONF.  Both CS and PS domain calls are
  monitored.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatetsicmif_capture_call_end_status
(
  cm_call_event_e_type    event,            /* Event ID              */
  ds_at_call_info_s_type *event_ptr         /* Pointer to Event info */
);

/*===========================================================================

FUNCTION  DSATETSICMIF_REPORT_CLCK

DESCRIPTION
  This function reports the facility lock supplementary service status
  received from Call Manager or GSDI.

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
dsat_result_enum_type dsatetsicmif_report_clck
(
  const ds_at_cm_sups_event_type *sups_event, /* Sups event pointer */
  const ds_at_cmd_status_type    *status_ptr  /* DS Status pointer          */
);

#endif /* FEATURE_DSAT_ETSI_MODE */
#endif /* DSATETSICMIF_H */
