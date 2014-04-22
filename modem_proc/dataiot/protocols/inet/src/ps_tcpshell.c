/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            T C P S H E L L

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 1995-2021 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_tcpshell.c_v   1.1   30 Aug 2002 20:59:26   omichael  $
  $Header: //components/rel/dataiot.mpss/2.0/protocols/inet/src/ps_tcpshell.c#5 $ $DateTime: 2023/02/01 02:49:49 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/06/21   sg      Code changes to enable the force flag for TCP server type 
                   socket only to make sure that data is sent immediately
                   if server initiates the data transfer first.
04/12/20   ds      Code changes to set the TCB force flag to TRUE so that 
                   packet is sent immediately irrespective of TCP connection
                   type (Client or server).
01/09/20   ds      Code changes to save wscale and TCP window size in TCB 
                   needed to update while reopening listening socket. 
10/10/19   ds      Code changes to initialize congestion window size to 3*MSS.
08/13/18   ds      Code changes as per new FWK design for IOT.
09/21/12   ss      Replacing LOG_MSG_FATAL_ERROR with LOG_MSG_ERROR and 
                   evaluating ASSERT requirement.
12/23/10   ss      Cleaning up of Globals for thread safety in Q6 Free 
                   Floating environment.
01/11/10   ss      Lint warning fixes.
09/31/06   msr     Checking if rexmit timer is running before starting persist
                   timer in append_to_sndq()
06/13/06   msr     Making sure that timers are non-NULL before manipulating
                   them. Async TCB is not creating all the timers
11/05/05    rt     Fixed coverity errors.
08/12/05   kr      Removed FEATURE DS_PSTATS
07/20/05   rt      Setting the half_close flag as FALSE if close_tcp() during
                   FINWAIT1 state.
12/27/04   ifk     Replaced setting of tracer field with DSM_TOUCH_ITEM
12/02/04   msr     Including pstimer.h
10/25/04   msr     Starting persist timer only if there is data to be sent
                   and if receiver's window size is 0
10/13/04   vp      Removal of byte order conversion on ports in reset_tcp().
09/10/04   vp      Corrected the assignment of sequence number in RESET
                   segment in reset_tcp().
08/16/04   sv      Removed nxt pointer increments when we are sending FIN as
                   tcp_output already updates that.
06/11/04   vp      Removal of inclusion of psglobal.h,netuser.h and internet.h.
                   Included ps_pkt_info.h and ps_socket_defs.h. Replaced use of
                   TCP_PTCL with PS_IPPROTO_TCP. Replaced the use of byte ordering
                   macros with their PS versions.
05/19/04   vp      Assigned tcb_ptr->cwind according to passed mss in open_tcp
04/14/04   vp      Changed the input signature of reset_tcp to include
                   close_reason as an argument and removed a byte ordering bug
                   in reset_tcp
02/04/04   vp      Fixed function reset_tcp() due to change in signature of
                   send_reset()
02/26/04   vp      Changed the name of functions setstate, close_self,
                   send_reset, send_syn and added tcp_in front of thier names.
02/17/04   mct     Replaced references to tcb window field with rcv.wnd
12/12/03   sv      Modified tcp_close to change tcp state from FIN_WAIT2 to
                   TIME_WAIT upon a close call in FIN_WAIT2 state.
10/17/03   sv      Modified open_tcp to return void.
08/18/03   sv      Modified the code to use IPv6 addresses internally.
08/01/03   ss/sv   Restrucutured the code to support IPv6
08/30/02   om      Updates for using routing cache when sending TCP resets.
07/31/02   usb     Added include ps_iface.h
07/31/02   usb     Removed include ps.h
03/05/02   pjb     Finished Accept Listen Code Review Items
02/22/02   pjb     Added accept and listen
11/12/01   dwp     Add "|| FEATURE_DATA_PS" to whole module. Wrap out
                   ps_initiate_upper_layer() if FEATURE_DATA_PS is defined.
10/30/00   snn     Replaced Tcb_ptr with tcb_ptr to follow naming convention.
10/10/00   na/snn  Removed the sndq_tail_ptr and put it in the TCB. This was
                   done to support multiple TCP sockets.
08/25/00   rc      Changed FEATURE_DS_MEM_CHK to FEATURE_DSM_MEM_CHK
09/14/98   na      Made changes for improved bidirectional performance.
05/05/98   na      Fixed a pointer problem in append_to_sendq.
04/14/98   ldg     Added counter for payload sent.
04/14/98   na      Changed append_to_sendq to save memory items by copying
                   small ones.
01/27/98   ldg     Removed the DS_SLIM switch.
01/19/98   na      Changes in append to sendq to facilitate escaping at task
                   level. Also, removed ISS2 target dependent code that needed
                   smaller memory items.
06/17/97   fkm     FEATURE_xxx Updates (via Automatic Scripts)
06/12/97   ldg     Reset stats in open_tcp for new FEATURE_DS_PSTATS.
01/23/97   na      Optimized append to sndq for better throughput. Uses small
                   items on the TX stream instead of large ones. Older code
                   under #if T_I2 due to ISS2 memory limitations.
01/20/97   na      Fixed bug for proper port assignment when TCP is opened
                   passively in a LtoM call
11/14/96   jjw     Upgraded (sped up) TCP open scheme
10/17/96   jjw     Minor change to initiate TCP Open after PPP (IPCP) Opens
05/24/96   jjw     Gemini & ISS2 Data Services release.
11/13/95   jjw     Changed min/max macros to upper case
11/09/95   jjw     IS-99 Compliant Release

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

/* User calls to TCP
 * Copyright 1991 Phil Karn, KA9Q
 * Additional material Copyright 1992 Glenn Hamilton McGregor
 * Additional material Copyright 1992 William Allen Simpson
 */


#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */
#include "dsm.h"
#include "ps_pkt_info.h"
#include "ps_tcp.h"
#include "ps_pcb_close_reason_codes.h"
#include "ps_ip.h"
#include "memory.h"
#include "msg.h"
#include "err.h"
#include "ps_socket_defs.h"
#include "pstimer.h"
#include "ds_Utils_DebugMsg.h"
#include "ps_in.h"
#include "ps_in_alias.h"
#include "ps_utils.h"
#include "ps_utils_defs.h"

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*-------------------------------------------------------------------------*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

static const uint16 Tcp_mss    = DEF_MSS;
static const uint16 Tcp_window = DEF_WND;
uint32 def_rtt = DEF_RTT;
uint32 min_rto = MIN_RTO; 

#ifdef FEATURE_NBIOT_NTN
typedef struct{  
  uint32_t def_rtt;  // T1 = used during connection establishment .
  uint32_t min_rto; // T2 = used for starting retransmission timeout for UL data packets.
} ntn_tcp_timer_v0x00;  /* Stores the tcp timer for ntn config*/

typedef union
{
   ntn_tcp_timer_v0x00 v0x00;
} ntn_tcp_timer_versions;



#pragma pack(push,1)
typedef struct 
{
  uint8 version;                   /* NV version                       */
  ntn_tcp_timer_versions versions;    /* TCP timer versions config for NTN     */
} ntn_timer_config;               /* Reject timer Config              */
#pragma pack(pop)

#endif
/*===========================================================================

FUNCTION OPEN_TCP

DESCRIPTION
  This function will Open a TCP connection using the specified parameters.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

 void open_tcp
(
  struct socket *lsocket,  /* Local socket */
  struct socket *fsocket,  /* Remote socket */
  int           mode,      /* Active/passive/server */
  uint32        window,    /* Receive window (and send buffer) sizes */
  uint16        mss,       /* TCP mss  */
  int           tos,       /* Type of service   */
  int           user,      /* User linkage area */
  struct tcb    *tcb_ptr
)
{
  uint32 high_rcv_window = 0;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( lsocket == NULL )
  {
    DATA_ERR_FATAL("open_tcp: NULL socket passed");
    return;
  }

  tcb_ptr->conn.local.address = lsocket->address;
  tcb_ptr->conn.local.port    = lsocket->port;
  if(fsocket != NULL)
  {
    tcb_ptr->conn.remote.address = fsocket->address;
    tcb_ptr->conn.remote.port    = fsocket->port;
  }
  else
  {
    tcb_ptr->conn.remote.address = ps_in6addr_any;
    tcb_ptr->conn.remote.port    = 0;
  }

  tcb_ptr->user = user;
  if( window != 0)
  {
    tcb_ptr->rcv.wnd = window;
  }
  else if( Tcp_window >= Tcp_mss )
  {
    tcb_ptr->rcv.wnd = Tcp_window;
  }
  else
  {
    tcb_ptr->rcv.wnd = Tcp_window * Tcp_mss;
  }

  /*-------------------------------------------------------------------------
   Backup original window value.
  -------------------------------------------------------------------------*/
  tcb_ptr->rcv.wnd_bkup = tcb_ptr->rcv.wnd;
  
  /*-------------------------------------------------------------------------
   Enable window scale option by default
  -------------------------------------------------------------------------*/
  tcb_ptr->flags.window_scale_enabled = TRUE;
  high_rcv_window = tcb_ptr->rcv.wnd >> 16;
  if(high_rcv_window > 0)
  {
    do
    {
      tcb_ptr->rcv.scale++;
      high_rcv_window >>= 1;
    } while(high_rcv_window != 0);
  }
  /*-------------------------------------------------------------------------
   Backup original window scale value.
  -------------------------------------------------------------------------*/
  tcb_ptr->rcv.scale_bkup = tcb_ptr->rcv.scale;

  tcb_ptr->snd.wnd = 1;      /* Allow space for sending a SYN */
  DS_ASSERT( tos >= 0 && tos < 256 );
  tcb_ptr->tos                 = (uint8)tos;
  tcb_ptr->mss                 = mss;
  tcb_ptr->timer_overload_flag = FALSE;
  /*-------------------------------------------------------------------------
    Update the congestion window depending upon the passed mss. The reason
    for this is that Async/Fax data calls have different default mss than PS.
  -------------------------------------------------------------------------*/
  tcb_ptr->cwind   = MIN(3 * mss, PS_TCP_DEF_CWIND);

  switch( mode)
  {
    case TCP_SERVER:
      tcb_ptr->flags.clone = 1;
      tcb_ptr->flags.is_server = 1;
      tcp_setstate( tcb_ptr, TCP_LISTEN);
      break;

    case TCP_PASSIVE:
      tcb_ptr->flags.is_server = 1;
      tcp_setstate( tcb_ptr, TCP_LISTEN);
      break;

    case TCP_ACTIVE:
      /* Send SYN, go into TCP_SYN_SENT state */
      tcb_ptr->flags.active = 1;
      tcp_send_syn( tcb_ptr);
      tcp_setstate( tcb_ptr, TCP_SYN_SENT);
      tcp_output( tcb_ptr);
      break;
   default:   /* Error Trap,  Path-whatever-logical */
    LOG_MSG_ERROR_0( "Invalid TCP Open");
  }
  return;
}

/*===========================================================================

FUNCTION APPEND_TO_TCP

DESCRIPTION
  This function will append the passed data item to the TCP send Q.

  To optimize for CPU, the TCP sndq is traversed manually instead of using
  dsm routines like dsm_append() and dsm_length_packet().

DEPENDENCIES
  sndq_tail_ptr is only updated in this function. So, no item from the
  TCP sendq should be removed from end of the sndq.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

LOCAL word append_to_sndq
(
  struct tcb  *tcb,            /* ptr to TCP Control Block        */
  dsm_item_type        *new_item_ptr    /* ptr to data item for TCP output */
)
{
  word          cnt=0;               /* holds count of new data bytes      */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-----------------------------------------------------------------------
    Appending to end of tcb_sndq - using small item pointers as against
     large items previously; improves throughput
   ----------------------------------------------------------------------*/
  if ( tcb->sndq == NULL)
  {
    tcb->sndq               = new_item_ptr;
  }
  else
  {
    (tcb->sndq_tail_ptr)->pkt_ptr  = new_item_ptr;
  }

  while ( new_item_ptr->pkt_ptr != NULL)
  {
    cnt += new_item_ptr->used;
    new_item_ptr = new_item_ptr->pkt_ptr;
  }
  cnt += new_item_ptr->used;
  tcb->sndq_tail_ptr = new_item_ptr;

  /*-------------------------------------------------------------------------
    Now update the TCP send Q count, free the passed in data item, and return
    the number of bytes loaded.
  -------------------------------------------------------------------------*/
  tcb->sndcnt += cnt;
  
  /*------------------------------------------------------------------------
    Set the force flag to 1 if TCP socket is of server/passive type.
  ------------------------------------------------------------------------*/
  if (tcb->flags.is_server)
  {
    tcb->flags.force = 1;
  }
  
  /*-------------------------------------------------------------------------
    Start persist timer only if the peer's window size is 0 and there is
    more data to be sent
  -------------------------------------------------------------------------*/
  if (0 == tcb->snd.wnd && 0 < tcb->sndcnt && tcb->persist_timer != NULL &&
      ps_timer_is_running(tcb->timer) == FALSE &&
      ps_timer_is_running(tcb->persist_timer) == FALSE)
  {
    tcp_start_persist_timer(tcb);
  }

  return( cnt);
} /* append_to_sndq */


/*===========================================================================

FUNCTION SEND_TCP

DESCRIPTION
  This function will queue the passed data for TXing on the passed TCP
  connection.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

sint15 send_tcp
(
  struct tcb  *tcb,			/* ptr to TCP Control Block 	   */
  dsm_item_type 	   *new_item_ptr   /* ptr to data item for TCP output */
)
{
  word			cnt;				 /* holds count of new data bytes	   */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(tcb == NULL || new_item_ptr == NULL)
  {
    dsm_free_packet( &new_item_ptr);
    return -1;
  }

  DSM_TOUCH_ITEM(new_item_ptr);

  cnt = 0;
  switch(tcb->state)
  {
    case TCP_CLOSED:
      dsm_free_packet( &new_item_ptr);
      return -1;

    case TCP_LISTEN:
      /* --------------------------------------------------------------
        If any remote address is 0, then it is a passive connection (LtoM).
        We stay in LISTEN state till we receive TCP packets from the IWF.
        Earlier the remote address was set to 0 in psmgr.c, so we enter
        the if when we are in passive mode and don't have dest. port #
        yet
      ----------------------------------------------------------------*/

      if( PS_IN6_ARE_ADDR_EQUAL(&tcb->conn.remote.address, &ps_in6addr_any) ||
          tcb->conn.remote.port == 0)
      {
        /* Save data for later */
        cnt = append_to_sndq( tcb, new_item_ptr);
        break;
      }
      /* Change state from passive to active */
      tcb->flags.active = 1;
      tcp_send_syn(tcb);
      tcp_setstate(tcb,TCP_SYN_SENT);      /* Note fall-thru */

    case TCP_SYN_SENT:
    case TCP_SYN_RECEIVED:
    case TCP_ESTABLISHED:
    case TCP_CLOSE_WAIT:
      cnt = append_to_sndq( tcb, new_item_ptr);
      tcp_output(tcb);
      break;

    case TCP_FINWAIT1:
    case TCP_FINWAIT2:
    case TCP_CLOSING:
    case TCP_LAST_ACK:
    case TCP_TIME_WAIT:
      dsm_free_packet( &new_item_ptr);
      return -1;
  }
  return (sint15)cnt;
}

/*===========================================================================

FUNCTION RESET_TCP

DESCRIPTION
  This function will

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void reset_tcp
(
  struct tcb *tcb,
  uint8 close_reason
)
{
  struct tcp fakeseg;
  pseudo_header_type ph;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(tcb == NULL)
  {
    return;
  }
  if(tcb->state != TCP_LISTEN)
  {
    /* Compose a fake segment with just enough info to generate the
     * correct RST reply
     */
    memset(&fakeseg,0,sizeof(fakeseg));
    fakeseg.dest      = tcb->conn.local.port;
    fakeseg.source    = tcb->conn.remote.port;
    fakeseg.ack_flag = 1;
    /* Here we try to pick a sequence number with the greatest likelihood
     * of being in his receive window.
     */
    fakeseg.ack = tcb->snd.nxt;

    /* Fill required fields of pseudo_hdr */
    ph.dest = tcb->conn.local.address;
    ph.source = tcb->conn.remote.address;
    ph.length = TCPLEN;
    ph.protocol = (uint8)PS_IPPROTO_TCP;

    /* Interface */

    tcp_send_reset(&fakeseg,&ph,&tcb->ipcb,tcb->family);
  }
  tcp_close_self(tcb,close_reason);
}

/*===========================================================================

FUNCTION CLOSE_TCP

DESCRIPTION
  This function will close the TCP connection for the passed TCB. This really
  means "I have no more data to send". It only closes the  connection in one
  direction, and we can continue to receive data indefinitely.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

int close_tcp
(
  struct tcb *tcb
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

      if(tcb == NULL)
      {
            return -1;
      }
      switch(tcb->state)
      {
      case TCP_CLOSED:
            return 0;      /* Unlikely */
      case TCP_LISTEN:
      case TCP_SYN_SENT:
            tcp_close_self(tcb, PS_PCB_CLOSE_REASON_CODE_NORMAL);
            return 0;
      case TCP_SYN_RECEIVED:
      case TCP_ESTABLISHED:
            tcb->sndcnt++;
            tcp_setstate(tcb,TCP_FINWAIT1);
            tcp_output(tcb);
            return 0;
      case TCP_CLOSE_WAIT:
            tcb->sndcnt++;
            tcp_setstate(tcb,TCP_LAST_ACK);
            tcp_output(tcb);
            return 0;
      case TCP_FINWAIT2:
           tcp_setstate(tcb, TCP_TIME_WAIT);
           return 0;
      case TCP_FINWAIT1:
           tcb->flags.half_close = FALSE;
           /* lint-fall through */
      case TCP_CLOSING:
      case TCP_LAST_ACK:
      case TCP_TIME_WAIT:
            return -1;
      }
      return -1;      /* "Can't happen" */
}

/*===========================================================================

FUNCTION SET_NTN_TIMER()

DESCRIPTION
  This is reads NV value and sets def_rtt and min_rto timer. 
  This API is used for updating timer value in case of NTN ( Non-Terrestrial Network )
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
#ifdef FEATURE_NBIOT_NTN
boolean set_ntn_timer()
{
  LOG_MSG_INFO1_0("set_ntn_timer: ENTER");
  
  ntn_timer_config ntn_nv_timer_values;
  boolean          is_success          = FALSE;
  const char*      ntn_timer_file_path = {PS_NV_CONF_ITEM_INFO(PS_NV_CONF_ntn_tcp_timer)};

  /*reading the nv value */
  is_success = (0 == ps_read_efs_nv(ntn_timer_file_path, 
                                     &(ntn_nv_timer_values), 
                                     sizeof(ntn_nv_timer_values)));
  /* Version overrided to 0 */
  ntn_nv_timer_values.version = 0;
  
  /* Success reading NV values */
  if(is_success)
  {
    LOG_MSG_INFO1_0("set_ntn_timer: Success reading NV value");
	
    def_rtt = ntn_nv_timer_values.versions.v0x00.def_rtt ;//timer value from NV
	min_rto = ntn_nv_timer_values.versions.v0x00.min_rto ; // timer valur from NV

	LOG_MSG_INFO1_2("set_ntn_timer: def_rtt = %u  def_rto = %u ",def_rtt,min_rto);
  }
  else
  { 
    /* NV item is not set so storing default value*/
    LOG_MSG_INFO1_0("set_ntn_timer:NV value not set");
	
    def_rtt = 3000L; //storing default def_rtt for NTN
	min_rto = 6000L; //storing deafult min_rto for NTN

    LOG_MSG_INFO1_2("set_ntn_timer: def_rtt = %u  def_rto = %u ",def_rtt,min_rto);
  }

  LOG_MSG_INFO1_0("set_ntn_timer: EXIT");
  return TRUE;
}

/*===========================================================================

FUNCTION RESET_NTN_TIMER()

DESCRIPTION
  This function will reset the default value of def_rtt and min_rto timer
  This API is used for updating timer value in case of NTN ( Non-Terrestrial Network )
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean reset_ntn_timer()
{
    LOG_MSG_INFO1_0("reset_ntn_timer: ENTER");
	
	def_rtt = DEF_RTT; //restoring default def_rtt of TCP
	min_rto = MIN_RTO; //restoring default min_rto of TCP

	LOG_MSG_INFO1_2("reset_ntn_timer: def_rtt = %u  def_rto = %u ",def_rtt,min_rto);
	LOG_MSG_INFO1_0("reset_ntn_timer: EXIT");

	return TRUE;
}
#endif
