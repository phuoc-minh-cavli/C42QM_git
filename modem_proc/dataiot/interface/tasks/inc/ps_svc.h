#ifndef PS_SVC_H
#define PS_SVC_H
/*===========================================================================

                        P S _ S V C . H

DESCRIPTION
  This is the header file for the Data protocol Task. Contained herein are
  all the definitions, functions, and structures for other software tasks
  to communicate with the Data Protocol Task.

EXTERNALIZED FUNCTIONS
  ps_enable_sig()
    Allow a signal to be processed

  ps_disable_sig()
    Disallow a signal to be processed

  ps_set_sig_handler()
    Set a handler to process a signal in PS context

  ps_send_cmd()
    Post a command to the Data Protocol Task

  ps_set_cmd_handler()
    Set a handler to process a cmd in PS context

Copyright (c) 1998-2019 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //components/rel/dataiot.mpss/2.0/interface/tasks/inc/ps_svc.h#1 $ $DateTime: 2020/03/17 08:03:16 $ $Author: pwbldsvc $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
04/08/19    ds     Memory optimization related changes.
===========================================================================*/
/*--------------------------------------------------------------------------
NOTE: No Platform indepdent header inclusion...eg: rex.h
--------------------------------------------------------------------------*/
#include "comdef.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*--------------------------------------------------------------------------
  Data Protocol Task Signal Definitions.  The value of existing signals
  should not be changed while adding or deleting a signal.  Some signals
  (marked Reserved) have fixed value that should stay as it is.
--------------------------------------------------------------------------*/
typedef enum
{
  PS_DPL_DIAG_SIGNAL              =  0,   /* Incoming TCP data signal      */
  PS_CMD_Q_SIGNAL                 =  1,   /* PS command Q signal           */
  PS_DOG_HB_REPORT_SIGNAL         =  2,   /* Dog heart beat report signal  */  
  PS_EXT1_RX_SIGNAL               =  3,   /* generic RX signal - interface(s)
                                             with data pending are set in
                                             ps_ext1_sig_mask              */  
  PS_SIG_MASK_CHANGE_SIGNAL	      =  4,   /* Indicates sig mask has changed*/
  PS_RX_RMNET1_SIGNAL			  =  5,   /* RmNet1 RX signal	*/ 

  PS_DPL_SIGNAL                   =  6,   /**< IPA DPL packets available Signal  */
#ifdef FEATURE_MODE_TLB
  PS_3GPP_LO_B_UM_RX_Q_SIGNAL     =  7,   /* 3GPP Loopback mode B Rx Sig   */
#endif /* FEATURE_MODE_TLB */
  PS_SOCKET_RX_Q_SIGNAL			  =  9,
  
/*---------------------------------------------------------------------------
  BEWARE: DO NOT CHANGE ENUMERATION VALUE FOR OFFLINE, STOP AND START SIGS.
---------------------------------------------------------------------------*/
  PS_TASK_OFFLINE_SIGNAL          = 13,   /* Reserved TASK_OFFLINE_SIG     */
  PS_TASK_STOP_SIGNAL             = 14,   /* Reserved TASK_STOP_SIG        */
  PS_TASK_START_SIGNAL            = 15,   /* Reserved TASK_START_SIG       */

  /*DL data singals for each bearer*/
  PS_FWK_DL_SIGNAL_0              = 16,
  PS_FWK_DL_SIGNAL_1              = 17,
  PS_FWK_DL_SIGNAL_2              = 18,
  PS_FWK_DL_SIGNAL_3              = 19,  
  PS_FWK_DL_SIGNAL_4              = 20,

 /*Keeping the 3gpp_UM bearer signals*/
#ifdef FEATURE_MODE_TLB
  PS_3GPP_UM_RX_Q_SIGNAL_0		  = 21,   /* 3GPP Bearer 0 signal		   */
  PS_3GPP_UM_RX_Q_SIGNAL_1		  = 22,   /* 3GPP Bearer 1 signal		   */
  PS_3GPP_UM_RX_Q_SIGNAL_2		  = 23,   /* 3GPP Bearer 2 signal		   */
  PS_3GPP_UM_RX_Q_SIGNAL_3		  = 24,   /* 3GPP Bearer 3 signal		   */
  PS_3GPP_UM_RX_Q_SIGNAL_4		  = 25,   /* 3GPP Bearer 4 signal		   */
#endif /* FEATURE_MODE_TLB */

  /* PPP related signals */
  PS_RM_RX_SIGNAL                 = 26,
  PS_RM_2_RX_SIGNAL               = 27,
  PS_UIM_CMD_SIGNAL               = 28,
  PS_RMSM_AT_UM_RX_Q_SIGNAL       = 29,

  PS_MAX_SIGNALS,                         /* use next value for enum       */
  #ifdef TEST_FRAMEWORK
  #error code not present
#endif
} ps_sig_enum_type;

/*---------------------------------------------------------------------------
  Data Task Command Definitions. These are the commands that can be sent to
  one.
---------------------------------------------------------------------------*/

typedef enum
{ 
  PS_CMD_MIN                        = 0,
  PS_TIMER_CALLBACK_CMD             = 1, /* PS Timer expired call cback
                                             in PS context.*/
  /*-------------------------------------------------------------------------
   Instance statistics logging commands
  ------------------------------------------------------------------------*/
  PS_STAT_INST_GET_DESC_CMD         = 2,  /* Get instance description    */
  PS_STAT_INST_GET_STAT_CMD         = 3,  /* Get instance stats          */
  PS_STAT_INST_RESET_STAT_CMD       = 4,  /* Reset instance statistics   */

  /* Commands for Stats */
   PS_STAT_LOG_ON_DEMAND_CMD		= 5, /* Process log on demand request */
  
  PS_SOCKET_FLOW_EVENT_CMD          = 6,     /* Flow event         */            
  /*-------------------------------------------------------------------------
   SIM event handling commands
  ------------------------------------------------------------------------*/
  PS_SIM_EVENT_HANDLER_CMD          = 7,  /* Handle SIM_INIT_COMPLETED ev*/  
  
  /*-------------------------------------------------------------------------
    MCFG NV refresh command.
  -------------------------------------------------------------------------*/
   PS_UTILS_NV_REFRESH_EVENT_CMD            = 8,  /*MCFG NV refresh */  //Need

  // These are the new commands for PPP handling - these  
  // replace all of the other PPP commands that are in here.  ,
  PS_PPP_START_CMD              = 24 /* start a PPP device                */  ,
  PS_PPP_STOP_CMD               = 25 /* gracefully close a PPP device     */  ,
  PS_PPP_ABORT_CMD              = 26 /* abort a PPP device                */  ,
  PS_PPP_SET_MODE_CMD           = 27 /* change PPP operational mode       */  ,
  PS_PPP_INJECT_PKT_CMD         = 28 /* inject a PPP pkt into the device  */  ,
  PS_PPP_RESYNC_CMD             = 29 /* resync a PPP device               */  ,
  PS_PPP_VENDOR_SEND_CMD        = 30 /* send a vendor-specific packet     */  ,
  PS_PPP_NCP_START_CMD          = 31 /* start an NCP instance             */  ,
  PS_PPP_NCP_STOP_CMD           = 32 /* stop an NCP instance              */  ,
  PS_SNOOP_IP_PPP_DEV_UP_CMD    = 33  /* PPP device is up for IP          */  ,
  PS_SNOOP_IP_PPP_DEV_DOWN_CMD  = 34  /* PPP device is down               */  ,
  PS_SNOOP_IP_FLTR_ENABLED_CMD  = 35  /* IP filtering enabled on iface    */  ,
  PS_SNOOP_IP_FLTR_DISABLED_CMD = 36  /* IP filtering disabled on iface   */  ,
  PS_SNOOP_IP_COMP_ON_CMD       = 37  /* handle VJ compressed pkts        */  ,
  PS_SNOOP_IP_COMP_OFF_CMD      = 38  /* No more VJ compressed pkts       */

  
 /*-------------------------------------------------------------------------
    The IPv6 State Machine commands
  -------------------------------------------------------------------------*/
  ,PS_IP6_SM_BASE_CMD            = 46  /* used to do math                  */  //Move To DS
  ,PS_IP6_SM_START_CMD           = PS_IP6_SM_BASE_CMD
                                       /* the IPv6 SM Start event          */
  ,PS_IP6_SM_RS_TIMER_CMD        = 47  /* the IPv6 SM RS Timer event       */
  ,PS_IP6_SM_RA_TIMER_CMD        = 48  /* the IPv6 SM RA Timer event       */
  ,PS_IP6_SM_VALID_RA_CMD        = 49  /* the IPv6 SM Valid RA event       */
  ,PS_IP6_SM_LINK_CHANGED_CMD    = 50  /* the IPv6 SM Link changed event   */
  ,PS_IP6_SM_STOP_CMD            = 51  /* the IPv6 SM Stop event           */
  ,PS_IP6_SM_ABORT_CMD           = 52  /* the IPv6 SM Abort event */  
  ,PS_IP6_SM_UPDATE_CONFIG_CMD      = 53  /* the IPv6 SM Update the config event */  
  ,PS_IP6_SM_DHCP6_PROVISIONED_CMD  = 54  /* the IPv6 SM Link changed event   */
  ,PS_IP6_SM_DHCP6_FAILED_CMD       = 55  /* the IPv6 SM Link changed event   */
  ,PS_IP6_SM_DHCP6_STOPPED_CMD      = 56  /* the IPv6 SM Stopped event */  
  ,PS_IP6_SM_DHCP6_ABORTED_CMD      = 57  /* the IPv6 SM Aborted event */  
  ,PS_IP6_SM_MAX_CMD             = PS_IP6_SM_DHCP6_ABORTED_CMD
  
  ,PS_PPP_CLNT_REG_CMD            = 58 

  /*-------------------------------------------------------------------------
    DSS PING command.
  -------------------------------------------------------------------------*/
  ,PS_DSS_PING_SEND_PING_CMD         = 99 /* Send ICMP ECHO_REQUEST */
  ,PS_DSS_PING_NET_CB_CMD            = 100 /* Ping network callback */
  ,PS_DSS_PING_SOCK_CB_CMD           = 101 /* Ping socket callback */
  ,PS_DSS_PING_RSP_TIMEOUT_CB_CMD    = 102 /* Ping response timeout cb */
  ,PS_DSM_MEM_EVENT_CTRL_CMD         = 103 /* DSM MEM event cmd*/

  /*-------------------------------------------------------------------------
    Domain Name System resolver command.
  -------------------------------------------------------------------------*/
  ,PS_DNS_RESOLVE_CMD                = 106 /* To start the DNS resolver     */
  ,PS_DNS_IO_MGR_SOCK_EVENT_CMD      = 107 /* IO manager sock event cmd     */
  ,PS_DNS_RESOLVER_TIMEOUT_CMD       = 108 /* Resolver timeout event        */
  ,PS_DNS_DELETE_SESSION_CMD         = 109 /* Delete session command        */

/*-------------------------------------------------------------------------
    Stateless DHCPv6 Server MGR event handling commands
  -------------------------------------------------------------------------*/
  ,PS_DHCP6_SL_SERVER_MGR_START_CMD     = 145 /* Stateless DHCP6 mgr start */
  ,PS_DHCP6_SL_SERVER_MGR_STOP_CMD      = 146 /* Stateless DHCP6 mgr stop */
  ,PS_DHCP6_SL_SERVER_SOCK_EVENT_CMD    = 147 /* Send notification of
                                                 socket event */

    /*-------------------------------------------------------------------------
    More Domain Name System resolver commands.
  -------------------------------------------------------------------------*/
  ,PS_DNS_RESOLVER_IFACE_DOWN_CMD         = 148 /* Iface Down event received */
  ,PS_DNS_RESOLVER_IFACE_ADDR_CHANGED_CMD = 149 /* Iface Address Changed event
                                                   received */
  ,PS_DNS_RESOLVER_DELETE_INSTANCE_CMD    = 150 /* Delete the resolver */

    /*-------------------------------------------------------------------------
    Domain Name System flush cache commands.
  -------------------------------------------------------------------------*/
  ,PS_DNS_FLUSH_CACHE_CMD               = 153 /* Flush cache command       */
  ,PS_DNS_FLUSH_CACHE_ENTRY_CMD         = 154 /* Flush cache-entry command */
  
  /* Always must be last */
  ,PS_MAX_DEFINED_CMD_TYPES

} ps_cmd_enum_type;


/*--------------------------------------------------------------------------
  Type definition for the signal handler.
  Returns TRUE if signal should be cleared from the set signal mask,
  FALSE if further signal processing is needed and hence signal
  should not be cleared
---------------------------------------------------------------------------*/

typedef boolean (*ps_sig_handler_type)
(
  ps_sig_enum_type sig,                    /* Signal to be processed       */
  void *user_data_ptr                      /* Parameter carrying user data */
);

/*--------------------------------------------------------------------------
  Type definition for the command handler
---------------------------------------------------------------------------*/

typedef void (*ps_cmd_handler_type)
(
  ps_cmd_enum_type cmd,         /* Actual command to be processed         */
  void *user_data_ptr           /* Command specific user parameters       */
);

#define PS_SET_SIGNAL ps_set_signal
/*===========================================================================
FUNCTION PS_SET_SIGNAL()

DESCRIPTION
  Sets a SIGNAL onto ps_tcb
  - Recommended API for all signals including ext signals.

PARAMETERS
  sig - ps_sig_enum_type

RETURN VALUE
  None.
===========================================================================*/
void ps_set_signal
(
  ps_sig_enum_type sig
);

/*===========================================================================
FUNCTION PS_CLR_SIGNAL()
- Recommended API for all signals including ext signals.

DESCRIPTION
  Clears a SIGNAL in ps_tcb

PARAMETERS
  sig - ps_sig_enum_type

RETURN VALUE
  None.
===========================================================================*/
void ps_clr_signal
(
  ps_sig_enum_type sig
);//Remove macro and extern C CODE


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================

FUNCTION PS_ENABLE_SIG()

DESCRIPTION
  This function enables the specified signal by adding it to the mask
  which the PS task uses for rex_wait()

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void ps_enable_sig
(
  ps_sig_enum_type sig                        /* Signal to be enabled      */
);

/*===========================================================================

FUNCTION PS_DISABLE_SIG()

DESCRIPTION
  This function disables a signal handling by removing it from the
  currently enabled signal mask.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void ps_disable_sig
(
  ps_sig_enum_type sig                        /* Signal to be disabled     */
);

/*===========================================================================

FUNCTION PS_SET_SIG_HANDLER()

DESCRIPTION
  This function sets a handler for the specified signal

DEPENDENCIES
  None.

RETURN VALUE
  Previously registered signal handler

SIDE EFFECTS
  None.

===========================================================================*/

extern ps_sig_handler_type ps_set_sig_handler
(
  ps_sig_enum_type sig,              /* Signal assocaited with handler     */
  ps_sig_handler_type sig_handler,   /* Signal handler function pointer    */
  void *user_data_ptr                /* Param to be passed to the handler  */
);

/*===========================================================================

FUNCTION PS_RESET_SIG_HANDLER()

DESCRIPTION
  This function resets a handler for the specified signal

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void ps_reset_sig_handler
(
  ps_sig_enum_type sig
);

/*===========================================================================

FUNCTION PS_SEND_CMD()

DESCRIPTION
  This function posts a cmd for processing in PS task context.  The cmd is
  processed by calling the registered cmd handler, if any.

  NOTE: The passed command will be copied to a PS task
  command buffer local to the PS Task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void ps_send_cmd
(
  ps_cmd_enum_type cmd,          /* Actual command to be processed         */
  void *user_data_ptr            /* Command specific user parameters       */
);

/*===========================================================================

FUNCTION PS_SET_CMD_HANDLER()

DESCRIPTION
  This function sets a handler for the specified cmd type.  The cmd handling
  can be disabled by setting a NULL handler

DEPENDENCIES
  None.

RETURN VALUE
  Previously registered cmd handler

SIDE EFFECTS
  None.

===========================================================================*/

extern ps_cmd_handler_type ps_set_cmd_handler
(
  ps_cmd_enum_type cmd,              /* Command name                      */
  ps_cmd_handler_type cmd_handler    /* Cmd handler function pointer      */
);

#endif /* PS_SVC_H */
