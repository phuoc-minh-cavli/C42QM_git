/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                           P S _ P R O T O _ I N I T . C

GENERAL DESCRIPTION
  Protocols Generic Initialization Functions.

Copyright (c) 2018 - 2022 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/dss_ps.c_v   1.13   24 Feb 2003 18:32:12   akuzhiyi  $
  $Header: //components/rel/dataiot.mpss/2.0/protocols/inet/src/ps_proto_init.c#2 $ $DateTime: 2022/05/24 00:01:24 $ $Author: pwbldsvc $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
05/19/22    rj     FR 78240: Memory optimizations- Disable DHCPv6, PPP, HDLC and SSL
04/02/19    ds     Fixed Compiler warnings.
09/28/18    ds     Code changes for IOT.
07/07/16    ds     IOE phase-I changes to disable compression libs.
06/27/14    vl     Created the initial version
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "amssassert.h"
#include "msg.h"
#include "nv_items.h"
#include "ps_utils.h"
#include "ps_proto_utils.h"
#include "ps_icmp.h"
#include "ps_proto_init.h"
#include "ps_svc.h"
#include "ps_utils.h"
#include "ps_tcp_config.h"
#include "dss_ps.h"
#include "ps_pkt_info_utils.h"
#include "dhcp6_server_mgri.h"
#ifdef FEATURE_MMGSDI
#include "mmgsdilib.h"
#endif /* FEATURE_MMGSDI */

#include "ds_Utils_DebugMsg.h"
#include "ds_system_heap.h"

#ifdef FEATURE_DATA_PS_IPV6
#include "ps_ip6i_sm.h"
#include "ps_icmp6.h"
#endif

#ifdef FEATURE_DATA_PS_PPP
#include "ps_ppp_task.h"
#endif

#include "dssi.h"
#include "mcfg_fs.h"
#include "ds_crit_sect.h"

/*===========================================================================

            REGIONAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/
/*-------------------------------------------------------------------------
  Macros for TCP ACK PRIO NV item read
-------------------------------------------------------------------------*/
#define  TCP_ACK_PRIO_NV_PATH "/nv/item_files/modem/data/protocols/tcp_ack_prio"

/*= = = = = = = =  = = = = = = = = = =  = = =  = = = = = = = = = = = = = = =
                    INTERNAL FUNCTION DEFINTIONS
= = = = = = = = = = = = = = =  = = = = =  = = = = = = = = = = = = = = = = =*/
#ifdef FEATURE_SEC_IPSEC
ds_crit_sect_type sa_mutex_var;
#endif 


#ifdef FEATURE_MODEM_CONFIG_REFRESH
/*===========================================================================
FUNCTION PS_PROTO_REFRESH_NV_EFS()

DESCRIPTION
  This function invokes all NV items of different protocol modules that was 
  changed due to SIM swap. 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ps_proto_refresh_nv_efs
(
  void
)
{
/*-------------------------------------------------------------------------*/
  /*------------------------------------------------------------------------
    Read the NV item for ICMP reply rate for both v4 and v6 before 
    initialising the token bucket.
  ------------------------------------------------------------------------*/
  icmp_read_tokens_nv();

#ifdef FEATURE_DATA_PS_IPV6
  /*-------------------------------------------------------------------------
    Do NV related initialization of IPv6 state machine.
  -------------------------------------------------------------------------*/
  ip6_sm_read_nv_efs();
#endif

#ifdef FEATURE_DATA_PS_PPP
  /*-------------------------------------------------------------------------
    Initialize PPP devices
  -------------------------------------------------------------------------*/
  ppp_task_nv_init();
#endif
}/* ps_proto_refresh_nv_efs() */

/*===========================================================================
FUNCTION PS_PROTO_REFRRESH_CB()

DESCRIPTION
  Callback function to be invoked when NV refresh event is received from 
  MCFG framework.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ps_proto_refresh_cb
(
  ps_utils_refresh_notify_info  ps_refresh_info
)
{
/*-------------------------------------------------------------------------*/
  LOG_MSG_INFO1_1("ps_proto_refresh_cb(): refresh_type : %d",
                   ps_refresh_info.refresh_type);

  /*------------------------------------------------------------------------
    Re-read the required NV items as NV refersh event is received. 
    For all the below mentioned 3 types, actions are the same for now.
  ------------------------------------------------------------------------*/  
  if( (PS_UTILS_REFRESH_TYPE_SLOT == ps_refresh_info.refresh_type) ||
      (PS_UTILS_REFRESH_TYPE_SUBS == ps_refresh_info.refresh_type) || 
      (PS_UTILS_REFRESH_TYPE_SLOT_N_SUBS == ps_refresh_info.refresh_type) )
  {
    ps_proto_refresh_nv_efs();
  }
  else
  {
    LOG_MSG_ERROR_1("utils_reg_call_back_fn() : Invalid refresh type : %d",
                    ps_refresh_info.refresh_type);
  }
} /* ps_proto_refresh_cb() */
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

/*===========================================================================
FUNCTION PS_PROTO_INIT()

DESCRIPTION
  This function invokes all protocol initialization functions from all its 
  different modules. It also registers to PS framework for NV refresh 
  notification. In addition this registers with PS for SIM ready event. This 
  function is called from psi_init() in PS task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ps_proto_init
(
  void
)
{
#ifdef FEATURE_MODEM_CONFIG_REFRESH  
  ps_utils_refresh_reg_info  proto_reg_info;
  ps_utils_status_e_type     reg_status;
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

  /*-------------------------------------------------------------------------*/ 
  LOG_MSG_INFO1_0("ps_proto_init(): "
                  "Protocol Service Initialization");
  
  /*-------------------------------------------------------------------------
    Initialize inet module
  -------------------------------------------------------------------------*/
  dssps_powerup_init();

#ifdef FEATURE_DATA_PS_IPV6
  /*-------------------------------------------------------------------------
    Do NV related initialization of IPv6 state machine.
  -------------------------------------------------------------------------*/
  ip6_sm_init();
#endif /* FEATURE_DATA_PS_IPV6 */

#ifdef FEATURE_DATA_PS_PPP
  /*-------------------------------------------------------------------------
    Initialize PPP devices
  -------------------------------------------------------------------------*/
  ppp_task_init();
#endif /* FEATURE_DATA_PS_PPP */

#ifdef FEATURE_DATA_PS_COMP_LOGGING
  /*-------------------------------------------------------------------------
    Initialize PS Compression Logging Module
  -------------------------------------------------------------------------*/
  ps_comp_logging_init();
#endif /* FEATURE_DATA_PS_COMP_LOGGING */

#ifdef FEATURE_MODEM_CONFIG_REFRESH
  proto_reg_info.func_ptr = 
                 (ps_utils_refresh_notify_func_type) ps_proto_refresh_cb;
  proto_reg_info.user_data_ptr = NULL;

  reg_status = ps_utils_refresh_reg(proto_reg_info);
  if(PS_UTILS_REG_SUCCESS != reg_status)
  {
    LOG_MSG_ERROR_1("Registration failure status : %d", reg_status);
  }
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

#ifdef FEATURE_SEC_IPSEC
  DS_INIT_CRIT_SECTION(&sa_mutex_var);
#endif

#ifdef DHCP_SUPPORT_V6
  dhcp6_server_ipfltr_init();
#endif

} /* ps_proto_init() */


void ps_proto_nv_read
(
  void
)
{
  nv_stat_enum_type nv_status;
  nv_item_type     *ps_nv_item_ptr;
  uint8            tcp_ack_prio_eb = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  /*-------------------------------------------------------------------------
    Allocate temporary memory for the NV item
  -------------------------------------------------------------------------*/
  ps_nv_item_ptr = (nv_item_type *)
    ds_system_heap_mem_alloc(sizeof(nv_item_type),__FILENAME__ , __LINE__);
  if( ps_nv_item_ptr == NULL )
  {
    DATA_ERR_FATAL("Mem alloc from system heap failed.");
  }

  /*-------------------------------------------------------------------------
    Retrieve the value of tcp_graceful_dormant_close flag from the NV
  -------------------------------------------------------------------------*/
  nv_status =
    ds_get_nv_item(NV_TCP_GRACEFUL_DORMANT_CLOSE_I, ps_nv_item_ptr);
  if( nv_status != NV_DONE_S )
  {
    LOG_MSG_INFO1_1("TCP graceful dormant close flag unavailable, nv_status %d,"
                    " using default", nv_status);
    /*-----------------------------------------------------------------------
      Set the global variable with default value and then set the NV item to
      default value
   ------------------------------------------------------------------------*/
    sock_config_cb.tcp_graceful_dormant_close = FALSE;
    ps_nv_item_ptr->tcp_graceful_dormant_close = FALSE;
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
    LOG_MSG_INFO1_1("TCP keepalive idle time unavailable, nv_status %d,"
                    " using default", nv_status);
    /*-----------------------------------------------------------------------
      set the global variable with default value and then set the NV item
      to default value
     ----------------------------------------------------------------------*/
    sock_config_cb.tcp_keepalive_idle_time = TCP_KEEPALIVE_IDLE_TIME;
    ps_nv_item_ptr->tcp_keepalive_idle_time = TCP_KEEPALIVE_IDLE_TIME/(60 * 1000);
  }
  else
  {
    /*-----------------------------------------------------------------------
     The NV keepalive time is supplied in minutes. Convert it to milliseconds
    -----------------------------------------------------------------------*/
     sock_config_cb.tcp_keepalive_idle_time =
       ((ps_nv_item_ptr->tcp_keepalive_idle_time) * 60 * 1000);
  }
 
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*------------------------------------------------------------------------
  Set the global var tcp_ack_prio_nv_val to NV item val tcp_ack_priority
  -------------------------------------------------------------------------*/
  if(mcfg_fs_read(TCP_ACK_PRIO_NV_PATH, 
                   &tcp_ack_prio_eb, 
                   sizeof(uint8), 
                   MCFG_FS_TYPE_EFS,
                   MCFG_FS_SUBID_NONE) 
                  != MCFG_FS_STATUS_OK)
  {
    /* By Default tcp_ack_prio_enabled variable is set to one (Enabled) */
    LOG_MSG_INFO1_1(
    "Bydefault tcp_ack_prio_enabled is set to 1, tcp_ack_prio_enabled=%d",
    ps_pkt_info_get_tcp_ack_prio_enabled());
  }
  else
  {
    ps_pkt_info_set_tcp_ack_prio_enabled(tcp_ack_prio_eb);
  }
  
  dssps_init();
  
  /*------------------------------------------------------------------------    
    Read the NV item for ICMP reply rate for both v4 and v6 before 
    initialising the token bucket.
  ------------------------------------------------------------------------*/
  icmp_read_tokens_nv();

  /*-------------------------------------------------------------------------
    Initialize token bucket to rate limit icmp messages.
  -------------------------------------------------------------------------*/
  icmp_init_token_bucket();
#ifdef FEATURE_DATA_PS_IPV6
  /*-------------------------------------------------------------------------
    Initialize token bucket to rate limit icmpv6 messages.
  -------------------------------------------------------------------------*/
  icmp6_init_token_bucket();
#endif /* FEATURE_DATA_PS_IPV6 */

  ip6_sm_read_nv_efs();

  DS_SYSTEM_HEAP_MEM_FREE(ps_nv_item_ptr);
}

