/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                               D S S _ P S . C

GENERAL DESCRIPTION
   This task has the cmd processing for commands sent by the sockets layer
   to PS to be processed.

   These commands include commands to WRITE/CONNECT/CLOSE.

   Copyright (c) 2002-2019 Qualcomm Technologies Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/



/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/dss_ps.c_v   1.13   24 Feb 2003 18:32:12   akuzhiyi  $
  $Header: //components/rel/dataiot.mpss/2.0/protocols/inet/src/dss_ps.c#1 $ $DateTime: 2020/03/17 08:03:16 $ $Author: pwbldsvc $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/28/18    ds     Code changes for IOT.
07/30/14    vl     Moved IPv6 related NV item to ps_ip6_sm.c file.
11/01/13    rk     Removed feature mobileap.
01/30/13    ds     Replacing LOG_MSG_FATAL_ERROR macro with LOG_MSG_ERROR.
12/14/12    fn     Remove default NV item writes when NV read fails 
11/05/12    fn     Optimizing NV read for NV IPV6 ENABLED item  
05/21/12    am     Invoking NAT callbacks for DSM small item events.
12/23/10    ss     Cleaning up of Globals for thread safety in Q6 Free 
                   Floating environment.
10/01/10    sa     Added appropriate log messages before ASSERT(0).
04/06/10    am     Making sure queues are zeroed out before init.
05/22/09    kk     Q6 compiler warning fixes.
03/13/08    pp     Metainfo optimizations.
06/26/07    rs     Added code to register command handler for IGMP/MLD
                   Multicasting.
04/29/07    ss     Added support for ICMPv6 sockets (PS_IPPROTO_ICMP6).
04/11/07    msr    Posting a cmd to PS in dsm_mem_event_ctrl
01/29/07    hm     Moved Q initialations to dssps_powerup_init()
11/21/06    rt     Added IGMP/MLD support for MBMS.
11/06/06    sv     Added PS command to process socket events.
11/02/06    mct    Added event queues for IPv6 privacy extensions.
10/13/06    msr    Handling PS_SOCKET_CMD_FLOW_ENABLED_IND
07/31/06    rt     Route Scope changes.
04/12/06    msr    Removed a F3 msg from data path
03/27/06    rt     Added a new case PS_SOCKET_CMD_WINDOW_REOPEN.
02/22/06    msr    Using single critical section
02/07/06    rt     L4 TASKLOCK changes.
12/19/05    msr    Removed PS_SOCKET_FLOW_ENABLED_CMD
12/03/05    rt     Added sockfd check to insure that the socket is not
                   freed while the command was in the queue.
11/29/05    sv     Added support for RSVP
08/15/05    ks     Changed PS_SOCKET_FLOW_ENABLED_SIG to CMD.
05/26/05    rt     Initialized tcp_count in powerup_init.
06/20/05    mct    Added the Multicast event queue.
04/18/05    vp     Change in signature of bind and listen functions.
04/17/05    msr    Setting command handler for PS_FLOW_EVENT_CMD_TYPE.
09/30/04    ks     Registering low wm callback for PS_MEM_META_INFO_TYPE in
                   dssps_powerup_init().
08/30/04    ks/msr Initialized SCB/ACB/TCB/UCB/ICMB queues in powerup_init
                   and removed dsstcp_init_tcb_array() call from init.
08/23/04    sv     Split the function dssps_init into powerup_init and init.
07/11/04    aku    Added suport for the QOS feature.
08/11/04    mvl    Changed the way dssps_init() initializes the default Any
                   IP family policy.
08/06/04    sv     Updated dssps_init() to read the IPV6 fail over config
                   and set the failover policy in sock cfg control block.
07/30/04    sv     Initialize socket configuration control block.
07/22/04    msr    Renamed the function ps_mem_reg_low_wm_cb to
                   ps_mem_reg_mem_avail_cb.
06/04/04    sv     Updated dssps_init to register for low watermark call back
                   of ps memory items.
04/30/04    sv     Fixed potential memory leak in process_socket_cmd.
03/30/04    vp     Merged changes from June04Dev_05.00.00 PS branch.
12/12/03    sv     Consolidated PS commands for iface events into one PS cmd.
10/24/03    sv     Added command processing functions for new ps commands
                   to support transient state call backs.
10/17/03    sv     Modified protocol specific functions to take protocl
                   control block as an argument.
10/01/03    sv     Added ps cmd code to handle PS bind request.
08/28/03    ss     Removed some no more relevant comments
08/25/03    aku    ps_mem_free() takes pointer-to-pointer as arg.
08/12/03    sv     Fixed the bug in handling socket option arguments.
08/08/03    sv     Restructured the sockets code.
07/18/03    ss     Get the sockfd from the socket command and validate that
                   the scb_ptr points to the correct socket.
02/24/02    aku    Access ps_iface_ptr from scb instead of acb.
02/13/03    aku    Added support to notify sockets when DSM becomes available
                   and also fixed bug wherein sockets could not be closed
                   once dss_pppclose() is called.
02/13/03    ss     Added socket shutdown command processing
01/24/03    usb    Removed unused variable to fix compiler warning
12/24/02    aku    Register for PS_SOCKET_PHYS_LINK_UP_EVENT_CMD and
                   PS_SOCKET_PHYS_LINK_DOWN_EVENT_CMD command handlers with
                   PS.
12/22/02    aku    IFACE_UP and IFACE_DOWN events have independent cmd
                   handlers.
10/08/02    om     Fixed preservation of app-field across send_q.
10/06/02    aku    Added support for processing iface callbacks in PS task
                   context.
10/06/02    aku    Added support for processing iface callbacks in PS task
10/02/02    mvl    fixed meta info memory leak.
09/18/02    aku    Added check for NULL ps_iface_ptr prior to calling the
                   abort function.
09/11/02    aku    Added support for aborting protocol stack if interface is
                   configured to do so.
07/16/02    aku    Include dss_ps.h
07/14/02    na/aku Created.
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "amssassert.h"
#include "msg.h"
#include "ps_svc.h"
#include "queue.h"
#include "dsm.h"
#include "dss_ps.h"
#include "ps_mem.h"
#include "ps_meta_info.h"
#include "ps_tx_meta_info.h"
#include "ps_pkt_meta_info.h"
#include "ps_rt_meta_info.h"
#include "ps_socket_defs.h"
#include "dssi.h"
#include "ps_utils.h"
#include "dssocki.h"
#include "dsstcp.h"
#include "dssudp.h"
#include "dssicmp.h"
#include "ps_tcp.h"
#include "ds_Utils_DebugMsg.h"
#include "ds_system_heap.h"
#include "ps_metai_info.h"
#include "ds3gsiolib.h"

/*===========================================================================

            REGIONAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/
q_type ps_socket_send_q;
ps_meta_info_type_ex    * dss_meta_info_ex_ptr;
ps_tx_meta_info_type    * dss_tx_meta_info_ptr;


/*= = = = = = = =  = = = = = = = = = =  = = =  = = = = = = = = = = = = = = =

                    INTERNAL FUNCTION DEFINTIONS

= = = = = = = = = = = = = = =  = = = = =  = = = = = = = = = = = = = = = = =*/


/*===========================================================================
FUNCTION DSSPS_PROCESS_SOCKET_CMD()

DESCRIPTION
  This function process the protocol packets that need to be sent by a socket
  Each item of the psi_socket_send_q is a protocol PDU and contains the
  pointer to the socket control block. This function removes the pointer to
  the socket control block from the enqueued item and calls the corresponding
  send routine for the socket.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE: When ps_socket_send_q is empty, clear signal from set signal mask
  FALSE: Don't clear signal yet - wait for send_q to be emptied out.

SIDE EFFECTS
  None.
===========================================================================*/
static boolean dssps_process_socket_cmd
(
  ps_sig_enum_type    sig,                 /* Signal to be processed       */
  void             *  user_data_ptr        /* Parameter carrying user data */
)
{
  void                       * optval;              /* value of the option */
  dsm_item_type              * item_ptr = NULL;    /* ptr to item dequeued */
  ps_meta_info_type          * meta_info_ptr = NULL;  /* meta info pointer */
  dssocki_app_to_ps_cmd_type * ps_cmd;                       /* PS command */
  struct dss_fcn_ptr_table   * sp;          /* function pointer table item */
  int                          level;                      /* option level */
  int                          optname;                     /* option name */
  int                          optlen;            /* length of option data */
  int                          cnt;                          /* bytes read */
  int16                        sockfd;
  int16                        ps_errno;             /* errno for ps calls */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Get the item from the ps_socket_send_q.
    If queue is empty then then ask for the signal to be removed.
    Note, we don't empty the queue in a while loop. Reason being that we want
    to process one write cmd for socket and then give a chance for a packet
    to be read thus balancing the flows.
  -------------------------------------------------------------------------*/
  
  ps_cmd = (dssocki_app_to_ps_cmd_type *) q_get( &ps_socket_send_q);
  if( ps_cmd == NULL)
  {
    return( TRUE);
  }
  

  if(ps_cmd->protocol == PS_IPPROTO_TCP)
  {
    if (ps_cmd->pcb.tcb != NULL)
    {
      sockfd = ps_cmd->pcb.tcb->sockfd;
    }
    else
    {
      LOG_MSG_ERROR_1("TCB is NULL for sock %d", ps_cmd->sockfd);
      PS_MEM_FREE(ps_cmd);
      return FALSE;
    }
  }
  else if(ps_cmd->protocol == PS_IPPROTO_UDP)
  {
    sockfd = ps_cmd->pcb.ucb->sockfd;
  }
  else if(ps_cmd->protocol == PS_IPPROTO_ICMP ||
          ps_cmd->protocol == PS_IPPROTO_ICMP6)
  {
    sockfd = ps_cmd->pcb.icb->sockfd;
  }
  else
  {
    LOG_MSG_ERROR_1("Unsupported transport protocol %d", ps_cmd->protocol);
    PS_MEM_FREE(ps_cmd);
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    get protocol specific function pointer table
  -------------------------------------------------------------------------*/
  sp = dssocki_get_fcnptr_table(ps_cmd->protocol);
  ASSERT(sp != NULL);


  /*-------------------------------------------------------------------------
    Call protocol specific functions depending on the command.
  -------------------------------------------------------------------------*/
  LOG_MSG_INFO1_3("Command: %d, Sockfd: %d, Protocol %d",
                ps_cmd->kind, ps_cmd->sockfd, ps_cmd->protocol);
  switch (ps_cmd->kind)
  {
    case APP_TO_PS_SOCKET_PAYLOAD:
      item_ptr = (dsm_item_type *) ps_cmd->user_data;
      meta_info_ptr = (ps_meta_info_type *) item_ptr->app_field;
      if(sp->write != NULL && ps_cmd->sockfd == sockfd)
      {
        (void)sp->write( ps_cmd->pcb, &(ps_cmd->ta), item_ptr, meta_info_ptr);
      }
      else
      {
        LOG_MSG_ERROR_2("Null write function: sockfd %d, protocol %d",
                 sockfd, ps_cmd->protocol);
        dsm_free_packet(&item_ptr);
        PS_META_INFO_FREE(&meta_info_ptr);
      }
      break;

    case APP_TO_PS_SOCKET_CONNECT:
      meta_info_ptr = (ps_meta_info_type *) ps_cmd->user_data;
      if(sp->ds_connect != NULL && ps_cmd->sockfd == sockfd)
      {
        LOG_MSG_INFO2_1("Calling protocol specific connect func for sockfd %d", sockfd);
        (void)sp->ds_connect(ps_cmd->pcb, &ps_cmd->ta, meta_info_ptr);
      }
      else
      {
        LOG_MSG_ERROR_2("Null connect function: sockfd %d, protocol %d",
                         sockfd, ps_cmd->protocol);
        PS_META_INFO_FREE(&meta_info_ptr);
      }
      break;

    case APP_TO_PS_SOCKET_CLOSE:
      if (sp->close != NULL && ps_cmd->sockfd == sockfd)
      {
        LOG_MSG_INFO2_1("Calling protocol specific close func for sockfd %d",
                sockfd);
        (void)sp->close( ps_cmd->pcb,(boolean)((int)ps_cmd->user_data),
                         &ps_errno );
      }
      else
      {
        LOG_MSG_ERROR_2( "Null close function: sockfd %d, protocol %d",
                         sockfd, ps_cmd->protocol);
      }
      break;

    case APP_TO_PS_SOCKET_SHUTDOWN:
      if (sp->shutdown != NULL && ps_cmd->sockfd == sockfd)
      {
        LOG_MSG_INFO2_2("Calling shutdown func for sockfd %d, how=%d",
                        sockfd, (int)(ps_cmd->user_data));
        (void)sp->shutdown( ps_cmd->pcb, (int)(ps_cmd->user_data), &ps_errno );
      }
      else
      {
        LOG_MSG_ERROR_2("Null shutdown function: sockfd %d, protocol %d",
                        sockfd, ps_cmd->protocol);
      }
      break;

    case APP_TO_PS_SOCKET_LISTEN:
      if (sp->listen != NULL && ps_cmd->sockfd == sockfd)
      {
        LOG_MSG_INFO2_1( "Calling listen func for sockfd %d", sockfd);
        (void)sp->listen( ps_cmd->pcb, (int)ps_cmd->user_data );
      }
      else
      {
        LOG_MSG_ERROR_2("Null listen function: sockfd %d, protocol %d",
                         sockfd, ps_cmd->protocol);
      }
      break;

    case APP_TO_PS_SOCKET_SETOPT:
    item_ptr = (dsm_item_type *) ps_cmd->user_data;
    
     /*---------------------------------------------------------------------
       Get the socket option name
     ---------------------------------------------------------------------*/
     cnt = dsm_pullup( &item_ptr, &optname, sizeof(int));
     if ((unsigned int)cnt != sizeof(int))
      {
       LOG_MSG_ERROR_1( "option name pullup ERR on sockfd %d", sockfd);
       break;
      }

     /*---------------------------------------------------------------------
       Get the socket option level
     ---------------------------------------------------------------------*/
     cnt = dsm_pullup( &item_ptr, &level, sizeof(int));
     if ((unsigned int)cnt != sizeof(int))
     {
       LOG_MSG_ERROR_1( "option level pull up error on sockfd %d", sockfd);
      break;
     }

     /*---------------------------------------------------------------------
       Get the socket option length
     ---------------------------------------------------------------------*/
     cnt = dsm_pullup( &item_ptr, &optlen, sizeof(int));
     if ((unsigned int)cnt != sizeof(int))
     {
       LOG_MSG_ERROR_1( "option length pullup error on sockfd %d", sockfd);
       break;
     }

     /*---------------------------------------------------------------------
       Get the socket option value. We make optval point to the data ptr
       of the dsm item and free the DSM item after setsockopt() is done.
     ---------------------------------------------------------------------*/
     optval = (void *)item_ptr->data_ptr;

     if ( sp->setopt != NULL && ps_cmd->sockfd == sockfd)
      {
       LOG_MSG_INFO2_1("Calling setsockopt func for sockfd %d", sockfd);
       sp->setopt( ps_cmd->pcb, level, optname, optval, optlen);
      }
      else
      {
       LOG_MSG_ERROR_2( "Null setopt function: sockfd %d, protocol %d",
                        sockfd, ps_cmd->protocol);
      }

     dsm_free_packet(&item_ptr);
      break;


    case APP_TO_PS_SOCKET_BIND:
      if (sp->bind != NULL && ps_cmd->sockfd == sockfd)
      {
        LOG_MSG_INFO2_1( "Calling BIND func for sockfd %d", sockfd);
        sp->bind(ps_cmd->pcb, &ps_cmd->ta);
      }
      else
      {
        LOG_MSG_ERROR_2( "Null Bind function: sockfd %d, protocol %d",
                         sockfd, ps_cmd->protocol);
      }
      break;

    case APP_TO_PS_SOCKET_WINDOW_REOPEN:
      if (sp->window_reopen != NULL && ps_cmd->sockfd == sockfd)
      {
        (void) sp->window_reopen(ps_cmd->pcb, &ps_errno);
      }
      else
      {
        LOG_MSG_INFO1_2("Null window_reopen function: sockfd %d, protocol %d",
                         sockfd, ps_cmd->protocol);
      }
      break;

    case APP_TO_PS_SOCKET_FLOW_ENABLED_IND:
      if (sp->flow_enabled_ind != NULL)
      {
        sp->flow_enabled_ind( ps_cmd->pcb);
      }
      else
      {
        LOG_MSG_INFO1_2("NULL FLOW_ENABLED_IND func, sockfd %d, protocol %d",
                        ps_cmd->sockfd, ps_cmd->protocol);
      }
      break;

    case APP_TO_PS_SOCKET_SET_ROUTE_META_INFO:
      if (sp->set_route_meta_info != NULL)
      {
        sp->set_route_meta_info( ps_cmd->pcb,
                                 ps_cmd->sockfd,
                                 ps_cmd->rt_meta_info_ptr);
      }
      else
      {
        PS_RT_META_INFO_FREE( &( ps_cmd->rt_meta_info_ptr));
        LOG_MSG_INFO1_2("NULL SET_ROUTE_META_INFO func, sockfd %d, protocol %d",
                        ps_cmd->sockfd, ps_cmd->protocol);
      }

      break;

    case APP_TO_PS_SOCKET_LINGER_RESET:
    {
      if (sp->set_linger_reset != NULL)
      {
        sp->set_linger_reset( ps_cmd->pcb,
                              ps_cmd->sockfd,
                              ps_cmd->is_linger_on,
                              ps_cmd->time_in_sec);
      }
      else
      {
        LOG_MSG_INFO1_2("NULL SET_LINGER_RESET func, sockfd %d, protocol %d",
                        ps_cmd->sockfd, ps_cmd->protocol);
      }

      break;
    }

    default:
      LOG_MSG_ERROR_1("unknown cmd %d", ps_cmd->kind);
      ASSERT( 0);
  }

  PS_MEM_FREE(ps_cmd);
  return FALSE;

} /* dssps_process_socket_cmd() */

/*===========================================================================

                    EXTERNAL FUNCTION DEFINTIONS

===========================================================================*/

/*===========================================================================
FUNCTION DSS_MEM_EVENT_CTRL()

DESCRIPTION
  This function notify sockets apps when DSM memory is available.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void dss_mem_event_ctrl
(
  dsm_mempool_id_enum_type pool_id,                      /* memory pool ID */
  dsm_mem_level_enum_type event,                   /* type of memory event */
  dsm_mem_op_enum_type op
)
{
  static uint32 dsm_mem_mask=0;
  dssocki_dsm_item_cmd_type *cmd = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(pool_id == DSM_DS_SMALL_ITEM_POOL)
  {
    switch (event)
    {
      case DSM_MEM_LEVEL_MANY:
        /*-----------------------------------------------------------------
          Notify sockets applications if the mobile has previously run out
          of both large and small items.
        -----------------------------------------------------------------*/
        LOG_MSG_INFO1_0(" SMALL ITEM MANY ITEMS MARK Reached");
        if(dsm_mem_mask == (DS_FLOW_DSM_SMALL_Q_MASK |
                            DS_FLOW_DSM_LARGE_Q_MASK))
        {
          LOG_MSG_INFO1_1( "Small items available, mask=0x%x",dsm_mem_mask);
          DS_SYSTEM_HEAP_MEM_ALLOC(cmd,
                                   sizeof(dssocki_dsm_item_cmd_type),
                                   dssocki_dsm_item_cmd_type*);

          if (cmd == NULL)
          {
            LOG_MSG_ERROR_0("Cannot alloc memory for posting small"
                                     " pool DSM MANY cmd");
            return;
          }
          cmd->kind = DSSOCKI_DSM_ITEMS_MANY;
          ps_send_cmd(PS_DSM_MEM_EVENT_CTRL_CMD, (void*)cmd);
        }
        dsm_mem_mask &= ~(DS_FLOW_DSM_SMALL_Q_MASK);
        break;

      case DSM_MEM_LEVEL_FEW:
        /*-----------------------------------------------------------------
          Update the static mask.
        -----------------------------------------------------------------*/
        LOG_MSG_INFO1_0( " SMALL ITEM FEW ITEMS MARK Reached");

        DS_SYSTEM_HEAP_MEM_ALLOC(cmd,
                                 sizeof(dssocki_dsm_item_cmd_type),
                                 dssocki_dsm_item_cmd_type*);

        if (cmd == NULL)
        {
          LOG_MSG_ERROR_0("Cannot alloc memory for posting small"
                                   " pool DSM FEW cmd");
          return;
        }

        cmd->kind = DSSOCKI_DSM_ITEMS_FEW;

        ps_send_cmd(PS_DSM_MEM_EVENT_CTRL_CMD, (void*)cmd);
        dsm_mem_mask |= DS_FLOW_DSM_SMALL_Q_MASK;
        break;

      default:
        break;
    }
  }
  else if(pool_id == DSM_DS_LARGE_ITEM_POOL)
  {
    switch (event)
    {
      case DSM_MEM_LEVEL_MANY:
        /*-----------------------------------------------------------------
          Notify sockets applications if the mobile has previously run out
          of both large and small items.
        -----------------------------------------------------------------*/
        LOG_MSG_INFO1_0(" LARGE ITEM MANY ITEMS MARK Reached");
        if(dsm_mem_mask == (DS_FLOW_DSM_SMALL_Q_MASK |
                            DS_FLOW_DSM_LARGE_Q_MASK))
        {
          LOG_MSG_INFO1_1( "Large items available, mask=0x%x",dsm_mem_mask);

          DS_SYSTEM_HEAP_MEM_ALLOC(cmd,
                                   sizeof(dssocki_dsm_item_cmd_type),
                                   dssocki_dsm_item_cmd_type*);

          if (cmd == NULL)
          {
            LOG_MSG_ERROR_0("Cannot alloc memory for posting large"
                                     " pool DSM MANY cmd");
            return;
          }
          cmd->kind = DSSOCKI_DSM_ITEMS_MANY;

          ps_send_cmd(PS_DSM_MEM_EVENT_CTRL_CMD, (void*)cmd);
        }
        dsm_mem_mask &= ~(DS_FLOW_DSM_LARGE_Q_MASK);
        break;

      case DSM_MEM_LEVEL_FEW:
        /*-----------------------------------------------------------------
          Update the static mask.
        -----------------------------------------------------------------*/
        LOG_MSG_INFO1_0( " LARGE ITEM FEW ITEMS MARK Reached");

        DS_SYSTEM_HEAP_MEM_ALLOC(cmd,
                                 sizeof(dssocki_dsm_item_cmd_type),
                                 dssocki_dsm_item_cmd_type*);

        if (cmd == NULL)
        {
          LOG_MSG_ERROR_0("Cannot alloc memory for posting large"
                                   " pool DSM FEW cmd");
          return;
        }

        cmd->kind = DSSOCKI_DSM_ITEMS_FEW;

        ps_send_cmd(PS_DSM_MEM_EVENT_CTRL_CMD, (void*)cmd);
        dsm_mem_mask |= DS_FLOW_DSM_LARGE_Q_MASK;
        break;

      default:
        break;
    }
  }
} /* dss_mem_event_ctrl() */

/*===========================================================================
FUNCTION DSSPS_PROCESS_DSM_MEM_EVENT_CTRL_CMD()

DESCRIPTION
  This function processes the DSM memory related events posted by DSM layer

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dssps_process_dsm_mem_event_ctrl_cmd
(
  ps_cmd_enum_type    cmd_name,
  void              * user_info_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  dssocki_dsm_item_cmd_type *cmd = NULL;

  if (user_info_ptr == NULL)
  {
    DS_MSG0(MSG_LEGACY_ERROR, "User info is NULL");
    return;
  }

  cmd = (dssocki_dsm_item_cmd_type*)user_info_ptr;
  switch (cmd->kind)
  {
    case DSSOCKI_DSM_ITEMS_MANY:
      dssocki_socket_event_occurred(NULL, NULL);
    break;

    default:
      break;
  }

  //Memory allocated in dss_mem_event_ctrl
  DS_SYSTEM_HEAP_MEM_FREE(cmd);
} /* dssps_process_dsm_mem_event_ctrl_cmd() */


/*===========================================================================
FUNCTION DSSPS_POWERUP_INIT()

DESCRIPTION
  Powerup initialization of the sockets stuff in the PS task.

DEPENDENCIES
  Called at powerup from the PS context

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void dssps_powerup_init
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Initialize the Socket command queue (from the app tasks to PS.)
  -------------------------------------------------------------------------*/
  memset(&ps_socket_send_q, 0, sizeof(q_type));
  (void) q_init( &ps_socket_send_q);

  /*-------------------------------------------------------------------------
    Register handlers and enable signals used by Sockets.
  -------------------------------------------------------------------------*/
  (void)
  ps_set_sig_handler( PS_SOCKET_RX_Q_SIGNAL, dssps_process_socket_cmd, NULL);
  ps_enable_sig( PS_SOCKET_RX_Q_SIGNAL);

  /*-------------------------------------------------------------------------
    Pre-allocate meta info at powerup so that it is not allocated every time
    in data path. This meta info is DUPed and sent along with each legacy pkt
  -------------------------------------------------------------------------*/
  PS_META_INFO_GET_EX(dss_meta_info_ex_ptr);
  PS_TX_META_INFO_GET_ALL(dss_tx_meta_info_ptr);
  if (NULL == dss_meta_info_ex_ptr || NULL == dss_tx_meta_info_ptr)
  {
    DS_ERROR_FATAL("ps_dpm_ul_legacy_init(): Couldn't alloc meta info");
    return;
  }
    LOG_MSG_INFO1_1("dss_tx_meta_info_ptr 0x%p",dss_tx_meta_info_ptr);
  dss_meta_info_ex_ptr->tx_meta_info_ptr = dss_tx_meta_info_ptr;

  
  /*-------------------------------------------------------------------------
    Register for DSM memory events (both SMALL and LARGE items)
  -------------------------------------------------------------------------*/
  dsm_reg_mem_event_cb ( DSM_DS_SMALL_ITEM_POOL,
                         DSM_MEM_LEVEL_MANY,
                         DSM_MEM_OP_FREE,
                         dss_mem_event_ctrl );

  dsm_reg_mem_event_cb ( DSM_DS_SMALL_ITEM_POOL,
                         DSM_MEM_LEVEL_FEW,
                         DSM_MEM_OP_NEW,
                         dss_mem_event_ctrl );

  dsm_reg_mem_event_cb ( DSM_DS_LARGE_ITEM_POOL,
                         DSM_MEM_LEVEL_MANY,
                         DSM_MEM_OP_FREE,
                         dss_mem_event_ctrl );

  dsm_reg_mem_event_cb ( DSM_DS_LARGE_ITEM_POOL,
                         DSM_MEM_LEVEL_FEW,
                         DSM_MEM_OP_NEW,
                         dss_mem_event_ctrl );

  /*-------------------------------------------------------------------------
    Register command handler for all DSM memory events
  -------------------------------------------------------------------------*/
  (void) ps_set_cmd_handler(PS_DSM_MEM_EVENT_CTRL_CMD,
                            dssps_process_dsm_mem_event_ctrl_cmd);
  
  return;

} /* dssps_powerup_init() */


/*===========================================================================
FUNCTION DSSPS_INIT()

DESCRIPTION
   Initialization of the sockets stuff in the PS task.

DEPENDENCIES
  Called at initialization from the PS context

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void dssps_init(void)
{
  nv_stat_enum_type nv_status;
  nv_item_type     *ps_nv_item_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Allocate temporary memory for the NV item
  -------------------------------------------------------------------------*/
  ps_nv_item_ptr = (nv_item_type *)
    ds_system_heap_mem_alloc(sizeof(nv_item_type), __FILENAME__ , __LINE__);
  if( ps_nv_item_ptr == NULL )
  {
    DATA_ERR_FATAL("Mem alloc from system heap failed.");
  }

 /*-------------------------------------------------------------------------
   * Retrieve the value of tcp_graceful_dormant_close flag from the NV
  -------------------------------------------------------------------------*/
  nv_status = ds_get_nv_item( NV_TCP_GRACEFUL_DORMANT_CLOSE_I,
                              ps_nv_item_ptr );
  if( nv_status != NV_DONE_S )
  {
    LOG_MSG_INFO1_0("TCP graceful dormant close flag unavailable, using default");
    /*-----------------------------------------------------------------------
     * set the global variable with default value
     ----------------------------------------------------------------------*/
    sock_config_cb.tcp_graceful_dormant_close = FALSE;
  }
  else
  {
     sock_config_cb.tcp_graceful_dormant_close =
       ps_nv_item_ptr->tcp_graceful_dormant_close;
  }

  /*-------------------------------------------------------------------------
    Retrieve the value of tcp_keepalive_idle_time from the NV
  -------------------------------------------------------------------------*/
  nv_status = ds_get_nv_item(NV_TCP_KEEPALIVE_IDLE_TIME_I, ps_nv_item_ptr);
  if( nv_status != NV_DONE_S )
  {
    LOG_MSG_INFO1_0("TCP keepalive idle time unavailable, using default");
    /*-----------------------------------------------------------------------
      set the global variable with default value in milliseconds
     ----------------------------------------------------------------------*/
    sock_config_cb.tcp_keepalive_idle_time = TCP_KEEPALIVE_IDLE_TIME;
  }
  else
  {
    /*-----------------------------------------------------------------------
     The NV keepalive time is supplied in minutes. Convert it to milliseconds
    -----------------------------------------------------------------------*/
     sock_config_cb.tcp_keepalive_idle_time =
       ((ps_nv_item_ptr->tcp_keepalive_idle_time) * 60 * 1000);
  }

  DS_SYSTEM_HEAP_MEM_FREE(ps_nv_item_ptr);
  return;
} /* dssps_init() */

/*===========================================================================
FUNCTION dssps_set_network_policy_setting

DESCRIPTION
  Sets the Network Policy setting in Sockets config.

DEPENDENCIES

RETURN VALUE
  None.
SIDE EFFECTS
  None.
===========================================================================*/
void dssps_set_network_policy_setting
(
  ip_addr_enum_type nw_policy
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  sock_config_cb.any_network_policy = nw_policy;  
} /* dssps_set_network_policy_setting() */
