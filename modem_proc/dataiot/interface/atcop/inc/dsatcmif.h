#ifndef DSATCMIF_H
#define DSATCMIF_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                D A T A   S E R V I C E S

                A T   C O M M A N D   
                
                ( C A L L  M A N A G E R  I N T E R F A C E )
                
                P R O C E S S I N G

                E X T E R N A L   H E A D E R   F I L E


GENERAL DESCRIPTION
  This file contains the definitions of data structures, defined and
  enumerated constants and function prototypes required for the
  call manager interface.

INITIALIZATION AND SEQUENCING REQUIREMENTS

   Copyright (c) 2002 - 2022 by Qualcomm Technologies Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/dsatcmif.h_v   1.0   29 Oct 2002 13:41:22   sbandaru  $ $DateTime: 2022/10/10 02:02:46 $ $Author: pwbldsvc $
  $Header: //components/rel/dataiot.mpss/2.0/interface/atcop/inc/dsatcmif.h#4 $ $DateTime: 2022/10/10 02:02:46 $ $Author: pwbldsvc $ $DateTime: 2022/10/10 02:02:46 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/10/22   ks      Made changes to look for CIOT_SYS_MODE_MASK which is
                   needed for updating cell-id properly.
01/25/21   ks      Added support for FSK related AT commands.
07/07/20   ks      Fixed incorrect cell-id on attaching to LTE network.
06/13/17   rk      Added support for +CEDRXS & +CEDRXRDP command.
10/05/16   skc     Fixed KW issues.
06/27/14   tk/sc   Added support for Dynamic ATCoP.
02/15/14   pg      Added support for SGLTE+G
11/13/13   sc      Added support for $QCCSGCOPS command.
07/16/13   pg      Fixed ^SYSINFO o/p for "SIM is not inserted"
07/02/13   pg      Migrated to MMGSDI API to find PLMN info in SE13 table
06/05/13   tk      ATCoP changes for SGLTE support on Dime Plus.
03/15/13   tk      Fixed +CFUN issue in CDMA in single SIM device mode.
02/22/13   tk      ATCoP - CM interface changes for Triton DSDA.
02/07/13   tk      ATCoP changes for DSDx Co-existence with TDS.
01/04/13   tk      ATCoP changes for DSDX C/G+G with True World Mode Support.
11/30/12   tk      ATCoP changes for Triton DSDA.
09/24/12   tk      Migrated to CM APIs for MRU table access.
09/05/12   tk      Changed invalid available networks length to 0.
07/26/12   sb      Fixed KW Errors.
02/17/12   sk      Migrating Data related AT commands interface to unified MH.
02/06/12   ad      Added CSTF/CEN/CGPIAF/CVMOD command support.
12/26/11   ad      Added Support the New Call Control Interface in Call Manager.
01/19/12   sk      Feature cleanup.
07/05/11   nc      Added support for ^MODE,^CARDMODE,^SYSCONFIG,^SYSINFO,^DSCI.
09/20/10   bs      Fixed +CGATT issue.
05/10/10   kk      Added ^SYSINFO, ^HDRCSQ, ^HRSSILVL commands. 
12/15/09   nc      Featurisation changes for LTE.
09/09/09   ua      Added support for EONS (spec 22.101).
07/15/09   sa      Added support for $CSQ command.
06/11/09   nc      Modified the prototype of dsatcmif_answer_voice_call 
                   to include answer or reject.
03/04/09   sa      AU level CMI modifications.
01/30/09   ua      Added support of CGEV for DUN calls.
12/29/08   nc      Added support for +VTS Command
02/20/08   sa      Added support for $QCSQ command.
01/25/08   bs      Fixing COPS test command behaviour on SMS arrival scenario.
08/29/05   snb     Add SIM card power control to +CFUN.
07/29/05   ar      Migrate CM SS events to watermark queue. 
04/12/05   ar      Use phone preference struct in dsat_net_reg_state_s_type.
03/21/05   tkk     Moved prototypes for +CGEREP command to ETSI specific files.
01/27/05   pdv     Merge from Sirius-ATCoP development.
01/27/05   pdv     Rebase from Mainline.
01/17/05   tkk     Added support for +CGEREP command.
12/07/04   ar      Add support for +COPS command.
09/03/04   snb     Add +CHLD support.
10/22/03   ar      Move Call Manager PH & SS API calls from ETSI to COMMON.
08/04/03   ar      Removed obsolete dsatcmif_get_cm_call_info()
04/18/02   ar      Created module.

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"


#include "dsat_v.h"
#include "dsati.h"
#include "cm.h"

/*===========================================================================

                        PUBLIC DATA DECLARATIONS

===========================================================================*/

/* CM client (ATCOP) identifier */
extern cm_client_id_type dsatcm_client_id;

#define INVALID_AVAIL_NET_LENGTH    (0)

/* This macro is specific to SYSINFO command */
#define DSAT_SYS_HYBRID 8
#define DSAT_SYSINFO_ROMSIM 240
#define DSAT_SYSINFO_UIM    1
#define DSAT_SYSINFO_NOUIM  255

/* Bitmasks indicating which fields of a stack are changed in CM SS event info. */

#define DSAT_SS_BM( val )                         ( (uint32)1 << (int)(val) )

#define DSAT_SS_SRV_STATUS_MASK                   ( DSAT_SS_BM(0) )
  /**< Serving status: No, limited, full. */

#define DSAT_SS_SRV_DOMAIN_MASK                   ( DSAT_SS_BM(1) )
  /**< Service domain. */

#define DSAT_SS_SYS_MODE_MASK                     ( DSAT_SS_BM(2) )
  /**< System mode. */

#define DSAT_SS_SYS_ID_MASK                       ( DSAT_SS_BM(3) )
  /**< System ID. */

#define DSAT_SS_SRV_IND_MASK                      ( DSAT_SS_BM(4) )
  /**< Service indicators. This mask indicates that a change of service
       occurred. It is set even if the service is the same as the previous
       service. */

#define DSAT_SS_MOBILITY_MGMT_MASK                ( DSAT_SS_BM(5) )
  /**< Mobility management. */

#define DSAT_SS_SIM_STATE_MASK                    ( DSAT_SS_BM(6) )
  /**< SIM card status. */

#define DSAT_SS_PS_DATA_SUSPEND_MASK              ( DSAT_SS_BM(7) )
  /**< PS data suspend mask. */

#define DSAT_SS_HDR_SRV_STATUS_MASK               ( DSAT_SS_BM(8) )
  /**< HDR Serving Status: No, limited, full. */

#define DSAT_SS_CELL_INFO_MASK                    ( DSAT_SS_BM(9) )
  /**< CELL_INFO: change in cell_id and neighbor_cell_id. */

#define DSAT_SS_LAC_TAC_RAC_CHGD_MASK             ( DSAT_SS_BM(10) )
  /**< Location Area Code (LAC), Tracking Area Code (TAC), or Routing Area
       Code (RAC) changed. */

#define DSAT_SS_CSFB_CALL_ACTIVE_MASK             ( DSAT_SS_BM(11) )
  /**< CSFB call status is changed */

#define DSAT_SS_CSG_INFO_CHANGED_MASK             ( DSAT_SS_BM(12) )
  /**< CSG Information has changed */

#define DSAT_SS_ROAM_STATUS_MASK                  ( DSAT_SS_BM(13) ) 
  /**< Roam call status is changed */ 

#define DSAT_SS_SRV_CAPABILITY_MASK               ( DSAT_SS_BM(14) ) 
  /**< CELL service status is changed */ 

#define DSAT_SS_SYS_FORBIDDEN_MASK                ( DSAT_SS_BM(15) ) 
  /**< Forbidden status is changed */ 

#define DSAT_SS_CELL_SRV_IND_MASK                 ( DSAT_SS_BM(16) ) 
  /**< Capability  status is changed */ 

#define DSAT_SS_CQI_INFO_MASK                     ( DSAT_SS_BM(17) ) 
  /**< CQI  status is changed */

#define DSAT_SS_CIOT_LTE_SYS_MODE_MASK            ( DSAT_SS_BM(18) ) 
  /**< CIOT capabilities mask. */

#define DSAT_SS_SRV_CHG_MASK ( DSAT_SS_SRV_STATUS_MASK       | \
                               DSAT_SS_SRV_DOMAIN_MASK       | \
                               DSAT_SS_SYS_MODE_MASK         | \
                               DSAT_SS_SYS_ID_MASK           | \
                               DSAT_SS_SRV_IND_MASK          | \
                               DSAT_SS_SIM_STATE_MASK        | \
                               DSAT_SS_PS_DATA_SUSPEND_MASK  | \
                               DSAT_SS_LAC_TAC_RAC_CHGD_MASK | \
                               DSAT_SS_CSG_INFO_CHANGED_MASK | \
                               DSAT_SS_CIOT_LTE_SYS_MODE_MASK )


/*---------------------------------------------------------------------------
    Typedefs for Network Registration support
---------------------------------------------------------------------------*/
typedef enum net_reg_domain
{
  DSAT_NET_REG_DOMAIN_CS,            /* Circuit switched domain */
  DSAT_NET_REG_DOMAIN_PS,            /* Packet switched domain */
  DSAT_NET_REG_DOMAIN_MAX            /* Internal use only! */
} net_reg_domain_e_type;

typedef enum net_reg_status
{
  DSAT_NET_REG_NONE        = 0,      /* Not registered, not searching */
  DSAT_NET_REG_HOME        = 1,      /* Registered on home network */
  DSAT_NET_REG_SEARCHING   = 2,      /* Not registered, searching */
  DSAT_NET_REG_DENIED      = 3,      /* Registration denied */
  DSAT_NET_REG_UNKNOWN     = 4,      /* Unknown registration state */
  DSAT_NET_REG_ROAMING     = 5,      /* Registered on roaming network */
  DSAT_REG_REGISTERED_MAX            /* Internal use only! */
} net_reg_status_e_type;

typedef enum cgatt_write
{
  DSAT_CGATT_ACTIVATE,            /* Activate PDP context */
  DSAT_CGATT_DEACTIVATE,          /* Deactivate PDP context */
  DSAT_CGATT_MAX
} cgatt_write_e_type;
typedef enum
{
  REPORT_NONE        = 0x00,
  REPORT_CREG        = 0x01,
  REPORT_CGREG       = 0x02,
  REPORT_CEREG       = 0x04,
  REPORT_DOLLAR_CREG = 0x08,
  REPORT_CREG_LAC         = 0x10,
  REPORT_CREG_CELL_ID     = 0x20,
  REPORT_CREG_LTE_TAC     = 0x40,
  REPORT_CREG_LAC_RAC_MME = 0x80,
  REPORT_CREG_CELL_PSC    = 0X100
}dsat_creg_report_e_type;

typedef struct network_id_info
{
  boolean                        present;
  sys_plmn_id_s_type             plmn;
  sys_plmn_lac_id_s_type         plmn_lac;
} net_id_info_s_type;

typedef struct dsat_creg_result_s
{
  net_reg_status_e_type    net_domain[DSAT_NET_REG_DOMAIN_MAX];
  sys_lac_type             plmn_lac;
  boolean                  pending_cell_info_update;
  sys_cell_id_type         cell_id;
  sys_lac_type             lte_tac;
  dsat_cops_act_e_type     act;
  byte                     lte_rac_or_mme_code;
  uint16                   psc;
  byte                     reg_rej_state;
  dsat_creg_report_e_type  cops_pending_report_creg;
  
}dsat_creg_result_s_type;

typedef struct dsat_jd_state_s
{
  boolean                   jamming_status;
  boolean                   soft_jamming_status;
}dsat_jd_state_s_type;

typedef struct net_reg_state
{
  /* Network registration state tracking variables:   */
  /*   +CREG applies to circuit switched domain only  */
  /*   +CGREG applies to packet switched domain only  */

  dsat_creg_result_s_type dsat_creg_reported;
  /* CM phone preferences */
  ds_at_cm_ph_pref_type  cmph_pref;
  
  /* PLMN ID and network names */
  net_id_info_s_type net_id_info;

  /* CM deep sleep state tracking */
  boolean  cm_ph_deep_sleep;

  /* System mode */
  sys_sys_mode_e_type sys_mode;
  sys_sys_mode_e_type ciot_lte_mode;

  dsat_cops_act_e_type act;  
  sys_sys_mode_e_type   prev_sys_mode;
} dsat_net_reg_state_s_type;

typedef struct
{
  uint64 cm_ss_mask;
  uint32 dsat_ss_mask;
} dsatcmif_ss_mask_map_s_type;

/* Call Manager message watermark queue structures */
typedef struct
{
  cm_ss_event_e_type          event;
  ds_at_ss_info_s_type        event_info;
} ds_at_cm_ss_event_type;

typedef struct 
{
  ds_command_enum_type  cmd;
  union
  {
    ds_at_cm_ss_event_type    ss_event;
    /************************************************* 
    Following are not used in initial implementation
    ds_at_cm_ph_event_type    ph_event;
    ds_at_cm_sups_event_type  sups_event;
    ds_at_cm_call_event_type  call_event;
    *************************************************/
  } msg;
} dsat_cm_msg_s_type;

typedef struct cen_emerg_num_list_s_type
{
  sys_plmn_id_s_type             plmn;  /* PLMN where system registered */
  byte                           plmn_num_flag; /* PLMN and Number list present */
} dsat_cen_emerg_num_list_s_type;

#define DSAT_CEN_PLMN_NUMBER_INVALID  0x00
#define DSAT_CEN_NUMBER_PRESENT       0x01
#define DSAT_CEN_PLMN_PRESENT         0x02

#define HDR_RSSI_NO_SIGNAL 125 

extern cgatt_write_e_type dsatcmif_attach_write;
typedef struct net_pref_s
{
  dsat_cops_act_e_type               act;    /* Access technology selection */
  dsat_cops_mode_e_type              mode;   /* Network operator select mode */
  sys_plmn_id_s_type                 plmn;   /* Network operator */
  boolean                            subscription_invoked; /* to ensure its usage at 
                                                subscription available event */
  boolean                            last_dereg_success; /* to ensure the last deregistration
                                                is successfull */
  unsigned int                       no_of_args; /* number of arg's user entered*/
  boolean                            is_arg0_valid; /*is arg 0 is valid*/
  boolean                            is_arg2_valid; /*is arg 2 is valid*/
} dsatetsicall_network_pref_s_type;

typedef enum dsat_cops_cmd_idx
{
  CMD_IDX_NONE = 0,
  CMD_IDX_COPS = 1,
  CMD_IDX_MAX
}dsat_cops_cmd_idx_e_type;

typedef struct network_list_s
{
  dsat_cops_async_state_e_type       cmd_state;
  dsat_cops_cmd_idx_e_type           cmd_idx;
  byte                               cmd_name[DSAT_COPS_CMD_NAME_SIZE];
  dsatetsicall_network_pref_s_type   requested_pref;
  dsatetsicall_network_pref_s_type   previous_pref;
} dsatetsicall_network_list_s_type;

typedef struct dsat_mode_urc_info_s
{
  dsat_num_item_type mode_val;
  boolean            report_flag;
} dsat_mode_urc_info_s_type;


typedef struct dsat_servs_state_info
{
  boolean                           cm_ph_deep_sleep; 
  sys_oprt_mode_e_type              dsat_oprt_mode;
#ifdef FEATURE_DSAT_EXTENDED_CMD
  sys_sys_mode_e_type               dsat_sys_mode;
#endif /*FEATURE_DSAT_EXTENDED_CMD */
  mmgsdi_se13_network_info_type         dsatetsicall_copn_plmn_info;    
/* COPS variable to store network preferences */
  dsatetsicall_network_pref_s_type      net_pref;
/* List of available and preferred networks returned from CM query */
  dsatetsicall_network_list_s_type      dsatetsicall_network_list;
/* CM network registration information */
  dsat_net_reg_state_s_type             dsat_net_reg_state;
  dsat_cen_emerg_num_list_s_type        dsat_cen_num_list;
#ifdef FEATURE_DSAT_ETSI_DATA
  cm_srv_domain_pref_e_type             dsat_ms_service_preference;
  cm_srv_domain_pref_e_type             dsat_prev_ms_service_preference;
#endif /* FEATURE_DSAT_ETSI_DATA */
  dsatcmif_sigrep_s_type                dsatcmif_signal_reporting;
  dsat_num_item_type                    dsat_ws46_curr_val;
  dsatact_ws46_res_e_type               dsat_ws46_res_type;
  int                                   last_reported_tz ; 
  dsat_mode_urc_info_s_type             dsat_mode_urc_info;
  dsat_edrx_ph_pref_s_type              dsat_edrx_info;  
#ifdef FEATURE_DSAT_ATCOP_VEND
  dsat_jd_state_s_type                  jdstate[DSAT_JD_ACT_MAX];
  dsat_ceinfo_s_type	                ceinfo;
#ifdef FEATURE_FSK_TX
  boolean                               is_gw_subscription_available;
#endif  /* FEATURE_FSK_TX */
#endif
}dsatcmif_servs_state_info;

/*-------------------------------------------------------------------------
            PUBLIC FUNCTION DECLARATIONS
-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION DSATCMIF_END_CALLS

DESCRIPTION
  This function issues a request to CM to end call(s).

DEPENDENCIES

RETURN VALUE
  TRUE:     The end request is sent, callback function
              will be called.
  FALSE:    The end request failed.

SIDE EFFECTS
  None
  
===========================================================================*/
boolean dsatcmif_end_calls( 
  const uint8 num_call_ids, 
  const cm_call_id_type * call_id_ptr
);


/*===========================================================================

FUNCTION DSATCMIF_ANSWER_VOICE_CALL

DESCRIPTION
  This command answer a voice call by calling cm_call_cmd_answer

DEPENDENCIES

RETURN VALUE
  TRUE : If the command succeeds.
  FALSE: If the command fails.

SIDE EFFECTS
  None
  
===========================================================================*/
boolean dsatcmif_answer_voice_call
(
  uint8                    call_id_ptr,
  boolean                  answer_call,
  cm_call_mode_info_e_type call_mode
);

/*===========================================================================

FUNCTION  DSAT_UPDATE_CALL_INFO_GLOBAL

DESCRIPTION
  This function update call related global variables for the CM call-related 
events.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void dsat_update_call_info_global
(
  ds_at_cm_call_event_type *call_event /* Call event pointer */
);

/*===========================================================================

FUNCTION  DSATCMIF_CM_CALL_EVENT_HANDLER_EX

DESCRIPTION
  This function is the handler function for the CM call-related events

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
dsat_result_enum_type dsatcmif_cm_call_event_handler_ex
(
  ds_command_type         * cmd_ptr              /* DS Command pointer         */
);

/*===========================================================================

FUNCTION  DSATCMIF_CM_CALL_EVENT_HANDLER

DESCRIPTION
  This function is the handler function for the CM call-related events

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
dsat_result_enum_type dsatcmif_cm_call_event_handler
(
  ds_at_cm_call_event_type *call_event, /* Call event pointer */
  dsat_result_enum_type     result
);

/*===========================================================================

FUNCTION  DSATCMIF_CM_CALL_CMD_HANDLER_EX

DESCRIPTION
  This function is the handler function for the CM call-related commands

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
dsat_result_enum_type dsatcmif_cm_call_cmd_handler_ex
(
  ds_command_type         * cmd_ptr              /* DS Command pointer         */
);


/*===========================================================================

FUNCTION  DSATCMIF_CM_CALL_CMD_HANDLER

DESCRIPTION
  This function is the handler function for the CM call-related commands

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
dsat_result_enum_type dsatcmif_cm_call_cmd_handler
(
  ds_at_cm_call_cmd_type *call_cmd, /* Call command pointer */
  dsat_result_enum_type   result
);

/*===========================================================================

FUNCTION  DSATCMIF_CM_PH_CMD_HANDLER_EX

DESCRIPTION
  This function is the handler function for the CM phone-related commands

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :    if there was any problem in executing the command
    DSAT_ASYNC_CMD :  if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatcmif_cm_ph_cmd_handler_ex
(
  ds_command_type         * cmd_ptr              /* DS Command pointer         */
);

/*===========================================================================

FUNCTION  DSATCMIF_CM_PH_CMD_HANDLER

DESCRIPTION
  This function is the handler function for the CM phone-related commands

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :    if there was any problem in executing the command
    DSAT_ASYNC_CMD :  if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatcmif_cm_ph_cmd_handler
(
  ds_at_cm_ph_cmd_type *ph_cmd, /* PH command pointer */
  dsat_result_enum_type result
);

/*===========================================================================
FUNCTION  DSATCMIF_CM_PH_EVENT_HANDLER_EX

DESCRIPTION
  This function is the event handler invoked by CM in response to Phone
  group commands:
    - PS domain attach or detach triggered by +CGATT command
    - functionality level/operating mode (power setting)
    - Available/preferred network reporting
    - Phone preference changes

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :    if there was any problem in executing the command
    DSAT_OK :       if it is a success.
    DSAT_ASYNC_EVENT : if success and no response required.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatcmif_cm_ph_event_handler_ex
(
  ds_command_type         * cmd_ptr              /* DS Command pointer         */
);

/*===========================================================================
FUNCTION  DSATCMIF_CM_PH_EVENT_HANDLER

DESCRIPTION
  This function is the event handler invoked by CM in response to PS domain 
  attach or detach triggered by +CGATT command.

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
dsat_result_enum_type dsatcmif_cm_ph_event_handler
(
  ds_at_cm_ph_event_type *ph_event, /* PH event pointer */
  dsat_result_enum_type   result
);

/*===========================================================================

FUNCTION  DSATCMIF_CM_SS_EVENT_HANDLER_EX

DESCRIPTION
  This function is the handler function for the CM serving system
  related events

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
dsat_result_enum_type dsatcmif_cm_ss_event_handler_ex
(
  dsat_cm_msg_s_type *msg_ptr
);

/*===========================================================================

FUNCTION  DSATCMIF_CM_SS_EVENT_HANDLER

DESCRIPTION
  This function is the handler function for the CM serving system
  related events

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
dsat_result_enum_type dsatcmif_cm_ss_event_handler
(
  dsat_cm_msg_s_type    *msg_ptr,
  dsat_result_enum_type  result
);


/*===========================================================================
FUNCTION  DSATCMIF_PH_CMD_CB_FUNC

DESCRIPTION
  CM phone command callback function

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  Adds command in DS command buffer

===========================================================================*/
void dsatcmif_ph_cmd_cb_func 
(
  void                         *data_ptr,         /* Data block pointer    */
  cm_ph_cmd_e_type              cmd,              /* Command ID            */
  cm_ph_cmd_err_e_type          cmd_err           /* Command error code    */
);

/*===========================================================================

FUNCTION  DSATCMIF_CHANGE_OPERATING_MODE

DESCRIPTION
  This function submits request to Call MAnager to change the phone
  operating mode.  The results of the call are handled as asynch events.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :    if there was any problem in executing the command
    DSAT_ASYNC_CMD :  if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatcmif_change_operating_mode
(
  sys_oprt_mode_e_type  new_mode         /* Operating mode */
);

#ifdef FEATURE_DSAT_LTE
#ifdef FEATURE_DSAT_ATCOP_VEND

/*===========================================================================

FUNCTION  DSATCMIF_GET_CM_SS_LTE_CPHY_TX_INFO  

DESCRIPTION
  This function issue query to Call Manager to get lte tx info

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
dsat_result_enum_type dsatcmif_get_cm_ss_lte_cphy_tx_info ( void );

/*===========================================================================

FUNCTION  DSATCMIF_SET_CM_SS_GNSS_TX_ACTIVITY  


DESCRIPTION
  This function issue query to Call Manager to set gnss tx activity

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
dsat_result_enum_type dsatcmif_set_cm_ss_gnss_tx_activity ( boolean gnss_stat );
#endif
#endif /* FEATURE_DSAT_LTE */


/*===========================================================================

FUNCTION  DSATCMIF_CM_SS_CMD_HANDLER

DESCRIPTION
  This function is the handler function for the CM service system-related
  commands

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :    if there was any problem in executing the command
    DSAT_ASYNC_CMD :  if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatcmif_cm_ss_cmd_handler
(
  ds_command_type         * cmd_ptr              /* DS Command pointer         */
);

/*===========================================================================

FUNCTION  DSATCMIF_INIT_CMIF_EX

DESCRIPTION
  CM Interface initialization function. This function does the following:
  - Initializes ATCOP as one of the clients to Call Manager.
  - Setup the Call Manager message watermark.
  - Registers the client call back functions.
  - Requests initial phone info (for preferences).
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatcmif_init_cmif_ex ( void );

/*===========================================================================

FUNCTION  DSATCMIF_INIT_CMIF

DESCRIPTION
  CM Interface initialization function. This function does the following:
  - Registers the client call back functions.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatcmif_init_cmif ( void );

/*===========================================================================

FUNCTION  DSATCMIF_DEINIT_CMIF

DESCRIPTION
  CM Interface deinitialization function. This function does the following:
  - Deregisters the client call back functions.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatcmif_deinit_cmif ( void );

/*===========================================================================

FUNCTION DSATCMIF_SIGNAL_HANDLER

DESCRIPTION
  This function processes the asynchronous signals assosiacted with
  messages from Call Manager.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_ERROR: if we find an error in procssing the event
  DSAT_OK: if processing of the event is complete and right
  DSAT_ASYNC_CMD : if we still have more events to process
  DSAT_ASYNC_EVENT : if the event is ignored

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatcmif_signal_handler
(
 dsat_mode_enum_type at_mode
);

/*===========================================================================
FUNCTION  DSATCMIF_CALL_CMD_CB_FUNC

DESCRIPTION
  CM Call status command callback function

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  Adds command in DS command buffer

===========================================================================*/
void dsatcmif_call_cmd_cb_func 
(
  void                         *data_ptr,         /* Data block pointer    */
  cm_call_cmd_e_type            cmd,              /* Command ID            */
  cm_call_cmd_err_e_type        cmd_err           /* Command error code    */
);

/*===========================================================================
FUNCTION  DSATCMIF_CALL_CMD_CB_EXTEN_FUNC

DESCRIPTION
  CM Call status command callback function

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  Adds command in DS command buffer

===========================================================================*/
void dsatcmif_call_cmd_cb_exten_func 
(
  void                            *data_ptr,          /* Data block pointer    */
  cm_call_cmd_e_type               cmd,               /* Command ID            */
  cm_call_cmd_err_e_type           cmd_err,           /* Command error code    */
  cm_call_cmd_err_cause_e_type     call_cmd_err_cause,/* Cause of the error*/
  cm_alpha_s_type                  alpha              /* Alpha value*/
);

/*===========================================================================

FUNCTION  DSATCMIF_IS_VOICE_CALL_TYPE

DESCRIPTION
  This function determines if the given call is voice call or not

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None

===========================================================================*/
boolean dsatcmif_is_voice_call_type( cm_call_type_e_type type);

sys_oprt_mode_e_type dsatcmif_get_oprt_mode(void);

#ifdef FEATURE_DSAT_ETSI_MODE
/*===========================================================================

FUNCTION  dsatcmif_cm_inband_cb_func

DESCRIPTION
  This function is the callback function for the CM inband events 

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatcmif_cm_inband_cmd_cb_func
(
  void*                     data_ptr,  /*  Not used.  */
  cm_inband_cmd_e_type      cmd,
  cm_inband_cmd_err_e_type  cmd_err
);

/*===========================================================================

FUNCTION  DSATCMIF_CM_INBAND_EVENT_HANDLER

DESCRIPTION
  This function is the handler function for the CM inband events related
  commands

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :    if there was any problem in executing the command
    DSAT_ASYNC_EVENT :  if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatcmif_cm_inband_cmd_handler
(
  ds_command_type         * cmd_ptr              /* DS Command pointer         */
);

/*===========================================================================
FUNCTION  DSATCMIF_INBAND_EVENT_CB_FUNC

DESCRIPTION
  CM Inband event command callback function

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  Adds command in DS command buffer

===========================================================================*/
void dsatcmif_cm_inband_event_cb_func
(
 cm_inband_event_e_type        inband_event,
 const cm_inband_info_s_type*  inband_info_ptr
);

/*===========================================================================

FUNCTION  DSATCMIF_CM_INBAND_EVENT_HANDLER

DESCRIPTION
  This function is the handler function for the CM info inband events related
  commands

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :    if there was any problem in executing the command
    DSAT_ASYNC_EVENT :  if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatcmif_cm_inband_event_handler
(
  ds_command_type         * cmd_ptr              /* DS Command pointer         */
);
#endif /* FEATURE_DSAT_ETSI_MODE */

/*===========================================================================

FUNCTION DSATCMIF_INITIALIZE_TO_DEFAULT

DESCRIPTION
  .

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatcmif_initialize_to_default(dsat_mem_type ,void *);

#endif /* DSATCMIF_H */
