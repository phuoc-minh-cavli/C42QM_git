/*===========================================================================

                     R M N E T _D A T A _ U L _O P T _ A 2 . C

DESCRIPTION

This is the implementation of Uplink Data Optimized handler.

Copyright (c) 2017 - 2021 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*====================== =====================================================

                            EDIT HISTORY FOR FILE
  $Header: //components/rel/dataiot.mpss/2.0/interface/rmnet/src/rmnet_data_ul_opt_a2.c#11 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
12/29/17    mkumarsa    First version of the data path module
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"

#include "amssassert.h"
#include "msg.h"
#include "err.h"
#include "dsm.h"
#include "dsm_lock.h"
#include "ds_crit_sect.h"
#include "rmnet_opt_defs.h"
#include "ps_metai_info.h"
#include "ps_tx_meta_info.h"
#include "ds_system_heap.h"
#include "ds_qmi_svc.h"
#include "ps_tcp_config.h"
#include "ps_icmp.h"
#include "ps_ip4_hdr.h"

#include "ps_logging.h"
#include "ps_ip4_frag.h"
#include "ps_icmp6_msg.h"
#include "ps_icmp6.h"
#include "ps_pkt_info_utils.h"
#include "ps_rm_taski.h"
#include "ds_fwk.h"
#include "ds_fwki.h"
#include "ds_ipfltr.h"
#include "ds_flow.h"
#include "ds_rmnet_smi.h"
#include "rmnet_data_ul_legacy.h"
#include "ps_in.h"
#include "ps_ip_fraghdl.h"

#ifdef FEATURE_DATA_PS_464XLAT
#include "ps_clat.h"
#include "ps_clat_sm.h"
#endif /* FEATURE_DATA_PS_464XLAT */
#include "ds_ipfltr_defs.h"
#include "ds_pdn_manager.h"

#define RMNET_UL_OPT_MAX_FRAG_PACKETS 5

#define WWAN_TECH_SUSP_MAX_DBG_RATE_LIMIT 10
uint8 wwan_tech_suspend_cntr = 0;

ps_tx_meta_info_type  * tx_meta_info_ptr;

extern ds_dl_opt_filter_spec_type *ds_global_ul_opt_fltr_arr;
/*===========================================================================

                             INTERNAL FUNCTIONS

===========================================================================*/

static dsm_item_type * pkts_array[RMNET_UL_OPT_MAX_FRAG_PACKETS];

static int rmnet_ul_opt_fragment_packet
(
  dsm_item_type                   * rx_pkt,
  ds_fwk_s_type                   * fwk_inst,
  uint16                            mtu,
  dsm_item_type                  ** pkts_array,
  uint16                            version,
  uint16                            pkt_len,
  int16                           * ds_errno
)
{
  int                               ret_val = 0;
  struct ip                         ipv4_hdr;
  ps_tx_meta_info_type            * frag_tx_meta_info_ptr = NULL;
  errno_enum_type                   ps_errno;
  ps_icmp6_msg_type                 icmp6_msg;
  ip_pkt_info_type                * pkt_info_ptr;
  uint16                            offset;  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  do
  {
    /*-----------------------------------------------------------------------

    1.	Find size of the pkt and compare against mtu. 
        If pkt is smaller than mtu then skip all below steps
    2.	If its ipv6 pkt then generate the error and drop the pkt 
    3.	Parse the pkt using ps_ip4_hdr_parse 
    4.	If df bit is not set then call into ip4_fragment_ex, 
        - UL opt code would pass array of ptrs as out parameter.
        - Protocols would fill up the array of ptrs with dsm items of generated 
          fragments
        - If in case of error, array of ptrs would all be NULL and ul opt code 
          would skip further processing 
        - We would go through all non-null entries of array and call tx cmd on
          each of them.
    5.	If df bit is set then we would call icmpv4_frag_needed_error
    -----------------------------------------------------------------------*/
    if (RMNET_OPT_V4_VERSION == version)
    {
      offset = 0;
      /*-------------------------------------------------------------------
        Parse Ipv4 header
      -------------------------------------------------------------------*/        
      if (FALSE == ps_ip4_hdr_parse_optimized(rx_pkt, &offset, &ipv4_hdr))
      {
        DS_MSG0(MSG_LEGACY_ERROR,"rmnet_ul_opt_fragment_packet(): Dropping pkt"
                                 "Cannot parse ip v4 header ");        
        ret_val = -1;
        break;
      } 
      
      /*-------------------------------------------------------------------
        1. If DF bit is not set then fragment the big packet else call
           icmpv4_frag_needed_error to generate ping response back.
        2. Call ps_iface_tx_cmd for each fragment 
      -------------------------------------------------------------------*/         
      if( ipv4_hdr.df == 0 )
      {
        pkts_array[0] = NULL;
        if (0 != ip4_fragment_optimized(&rx_pkt, 
                                        &ipv4_hdr, 
                                         mtu,
                                         pkts_array,
                                         RMNET_UL_OPT_MAX_FRAG_PACKETS))
        {
          /*-------------------------------------------------------------------
            Indication not to free the rx pkt
          -------------------------------------------------------------------*/         
          *ds_errno = DS_EINVAL;

          ret_val = -1;
          break;                              
        }                    
      }
      else
      {          
        if(-1 != ps_tx_pkt_info_generate(&rx_pkt, &frag_tx_meta_info_ptr, &ps_errno))
        {
          pkt_info_ptr = &(PS_TX_META_GET_PKT_INFO(frag_tx_meta_info_ptr));
          pkt_info_ptr->if_ptr = fwk_inst;
          icmpv4_frag_needed_error(&rx_pkt, mtu, pkt_info_ptr);
          PS_TX_META_INFO_FREE(&frag_tx_meta_info_ptr);

          /*-------------------------------------------------------------------
            Indication not to free the rx pkt
          -------------------------------------------------------------------*/         
          *ds_errno = DS_EINVAL;
        }
        
        else
        {
          DS_MSG2(MSG_LEGACY_ERROR,"ps_dpm_ul_opt_fragment_packet(): Dropping pkt"
                          "Size of pkt %d greater than mtu size %d for v4 iface"
                          " and DF bit is set ", 
                          pkt_len, mtu);      
        }

        ret_val = -1;
        break;                                  
      }        
    }
    else
    {
      if(-1 != ps_tx_pkt_info_generate(&rx_pkt, &frag_tx_meta_info_ptr, &ps_errno))
      {
        memset(&icmp6_msg, 0, sizeof(ps_icmp6_msg_type));
        icmp6_msg.cmn_hdr.type =  ICMP6_PKT_TOO_BIG;  //pkt too big
        icmp6_msg.cmn_hdr.code =  0;
        icmp6_msg.msg_type.mtu = mtu;
        pkt_info_ptr = &(PS_TX_META_GET_PKT_INFO(frag_tx_meta_info_ptr));
        pkt_info_ptr->if_ptr = fwk_inst;
        icmpv6_error_send( &rx_pkt,
                           &icmp6_msg,
                           pkt_info_ptr);
                           
        PS_TX_META_INFO_FREE(&frag_tx_meta_info_ptr);

        /*-------------------------------------------------------------------
          Indication not to free the rx pkt
        -------------------------------------------------------------------*/         
        *ds_errno = DS_EINVAL;
      }
      else
      {
        DS_MSG2(MSG_LEGACY_ERROR,"ps_dpm_ul_opt_fragment_packet(): Dropping pkt"
                        "Size of pkt %d greater than mtu size %d for v6 iface", 
                        pkt_len, mtu);
      }

      ret_val = -1;
      break;        
    }      
  } while(0);

  return ret_val;
} /* ps_dpm_ul_opt_fragment_packet() */
/*===========================================================================
FUNCTION ps_dpm_ul_opt_tcp_ack_pri_check

DESCRIPTION
  This macro returns whether the packet is a TCP ACK and if it needs to be
  prioritized for IPv4 and IPv6.

PARAMETERS
  pkt_buf     : pointer to the pkt buffer
  ip_hdr_len  : IP header length
  total_ip_len: Total IP length

RETURN VALUE
  TRUE  : if packet is a TCP ACK
  FALSE : otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
INLINE boolean rmnet_ul_opt_tcp_ack_pri_check
(
  uint8        * pkt_buf,
  uint8          ip_hdr_len,
  uint16         total_ip_len,
  uint8          prot_offset
)
{
  uint8  tcp_header_length;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    1. Check if it is a TCP packet
    2. Check if TCP ACK prioritization is enabled
    3. Check if it is a TCP ACK packet. If there is no payload for TCP, then
       it is most likely a TCP ACK
  -------------------------------------------------------------------------*/
  if (*(pkt_buf + prot_offset) == PS_IPPROTO_TCP &&
      ps_pkt_info_get_tcp_ack_prio_enabled())
  {
    tcp_header_length =
      (*(pkt_buf + ip_hdr_len + RMNET_OPT_TCP_HDR_LEN_OFFSET) &
         RMNET_OPT_TCP_HDR_LEN_MASK) >> RMNET_UL_OPT_PATH_WORD_SHIFT;

    if (total_ip_len == (ip_hdr_len + tcp_header_length))
    {
      return TRUE;
    }
  }

  return FALSE;

} /* ps_dpm_ul_opt_tcp_ack_pri_check() */

/*===========================================================================
FUNCTION ps_dpm_ul_opt_tcp_dst_port21_pri_check

DESCRIPTION
  This macro returns TRUE if the packet is a TCP packet and the destination port 
  is 21 and if it needs to be prioritized for IPv4 and IPv6.

PARAMETERS
  pkt_buf     : pointer to the pkt buffer
  ip_hdr_len  : IP header length
  prot_offset : Offset for the ipv4 or ipv6

RETURN VALUE
  TRUE  : if packet is a TCP and dst_port is 21
  FALSE : otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
INLINE boolean rmnet_ul_opt_ftp_ctrl_port_pri_check
(
  uint8        * pkt_buf,
  uint8          ip_hdr_len,
  uint8          prot_offset
)
{
  uint16                       dst_port =0;
  uint16                       port =0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    1. Check if it is a TCP packet
    2. Check if dst_port is 21 
  -------------------------------------------------------------------------*/
  
  if (*(pkt_buf + prot_offset) == PS_IPPROTO_TCP)
  {
     dst_port = (uint16)*((uint16*)(pkt_buf + ip_hdr_len +2));
     port     =  ps_ntohs(dst_port);
		
    if(port == RMNET_OPT_FTP_CONTROL_PORT)
    {
      return TRUE;
    }
  }

  return FALSE;

} /* ps_dpm_ul_opt_ftp_ctrl_port_pri_check() */


/*===========================================================================
FUNCTION PS_OPT_PATH_HGET32_UNALIGNED

DESCRIPTION
  This function extracts 32 bits in host order.

PARAMETERS
  cp - Byte value.

RETURN VALUE
  Returns 32 bit algined value.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
INLINE dword rmnet_ul_opt_hget32_unaligned
(
  byte * cp
)
{

  dword rval;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  cp += 4;

  rval = *(--cp);
  rval <<= 8;
  rval |= *(--cp);
  rval <<= 8;
  rval |= *(--cp);
  rval <<= 8;
  rval |= *(--cp);

  return( rval);

} /* ps_dpm_ul_opt_hget32_unaligned() */

/*===========================================================================
FUNCTION ps_dpm_ul_opt_ipv6_prefix_cmp

DESCRIPTION


PARAMETERS
  pkt_buf    : pointer to the pkt buffer
  info       : Rm state machine's information block

RETURN VALUE
  TRUE  : UM and packet prefix match
  FALSE : Prefix does not match

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
INLINE boolean rmnet_ul_opt_ipv6_prefix_cmp
(
  uint8               	    * pkt_buf,
  ds_fwk_s_type             * ds_fwk_ptr
)
{
  uint32   src_v6_prefix_addr32[2] ALIGN(4); //__attribute__ ((aligned(4)));
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  src_v6_prefix_addr32[0] = ((uint32) *(uint32 *)
                             (pkt_buf + RMNET_OPT_V6_SRC_ADDR_OFFSET));
  src_v6_prefix_addr32[1] = ((uint32) *(uint32 *)
                             (pkt_buf + RMNET_OPT_V6_SRC_ADDR_OFFSET+4));

  if ( (NULL != ds_fwk_ptr) && 
  	    (ds_fwk_ptr->v6_pdn_ptr != NULL)&&
      ((*(uint64*)(&src_v6_prefix_addr32[0])) == 
      ds_fwk_ptr->v6_net_info.ipv6_addrs[DS_DEFAULT_V6_INDEX]->prefix)
        //ipv6_addrs[DEFAULT_V6_INDEX]->prefix//ipv6_addrs[DEFAULT_V6_INDEX]->prefix

  )
  {
    return TRUE;
  }

  DS_MSG1(MSG_LEGACY_ERROR,"rmnet_ul_opt_ipv6_prefix_cmp() "
                  "Src V6 prefix 0x%llx doesnt match Um prefix, drop pkt!",
                  ps_ntohll(*(uint64*)((&src_v6_prefix_addr32[0]))));
  return FALSE;
} /* ps_dpm_ul_opt_ipv6_prefix_cmp */

INLINE boolean rmnet_ul_opt_process_filters
(
  ds_dl_opt_filter_spec_type * filter_ptr,
  uint16                       src_port,
  uint16                       dst_port,
  uint16                       protocol,
  uint16                       max_filter_entries
)
{
  uint16                   fltr_dst_port_host;
  uint16                   fltr_src_port_host;
  uint16                   pkt_dst_port_host;
  uint16                   pkt_src_port_host;
  uint8                    filter_cnt;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( filter_ptr != NULL )
  {
    pkt_dst_port_host = ps_ntohs(dst_port);
    pkt_src_port_host = ps_ntohs(src_port);
    for (filter_cnt = 0;
         filter_cnt < max_filter_entries &&
           filter_ptr->fi_handle != 0;
         filter_cnt++, filter_ptr++)
    {
      fltr_dst_port_host = ps_ntohs(filter_ptr->dst_port);
      fltr_src_port_host = ps_ntohs(filter_ptr->src_port);
      if ( ((protocol == filter_ptr->protocol) || 
            (filter_ptr->protocol == PS_IPPROTO_TCP_UDP && 
             (protocol == PS_IPPROTO_TCP || protocol == PS_IPPROTO_UDP)))&&
           ((pkt_dst_port_host >= fltr_dst_port_host &&
             pkt_dst_port_host <= fltr_dst_port_host + filter_ptr->dst_port_range) ||
            (pkt_src_port_host >= fltr_src_port_host &&
             pkt_src_port_host <= fltr_src_port_host + filter_ptr->src_port_range)))
      {
        DS_MSG3(MSG_LEGACY_HIGH,"rmnet_ul_opt_process_filters: "
                                 "Rx filter pass protocol (%d) dst port"
                                 " (%d) src port (%d)",
                                 protocol, pkt_dst_port_host, 
                                 pkt_src_port_host);
        return TRUE;
      }
    }
  }
  return FALSE;
}

/*===========================================================================
FUNCTION PS_UL_OPTIMIZED_IPV4_IS_LEGACY

DESCRIPTION
  This function returns whether the packet needs to take the legacy path or should
  be forwarded to the Um iface for IPv4.
  Packets take the legacy path in following cases
  1. If packet is destined to a broadcast address
  2. if packet is destined to a limited broadcast address
  3. If packet is destined to a multicast address
  4. If packet is fragmented
  5. if packet is destined to Rm iface
  6. if IP header has options

PARAMETERS
  pkt_buf    : pointer to the pkt buffer
  temp_info  : Rm state machine's information block
  ip4_hdr_len: IPv4 Header length

RETURN VALUE
  TRUE  : if packet needs to take legacy path
  FALSE : if packet should be forwarded to the Um iface

DEPENDENCIES
  None

SIDE EFFECTS
  None

===========================================================================*/
INLINE boolean rmnet_ul_opt_ipv4_is_legacy
(
  uint8               * const pkt_buf,
  ds_fwk_s_type       * ds_fwk_ptr,
  uint8                 ip4_hdr_len
)
{
  ds_dl_opt_filter_spec_type * filter_ptr = NULL;
  uint16                       dst_port = 0;
  uint16                       src_port = 0;
  uint8                        protocol = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*---------------------------------------------------------------------
   Extract the src port, dst port and protocol from the pkt_buf.
  ---------------------------------------------------------------------*/
   protocol = *(pkt_buf + RMNET_OPT_V4_PROT_FIELD_OFFSET);
   /*--------------------------------------------------------------------------
    This is to allow ICMP/IGMP packets to match optimized filters installed
    by clients for the same 
    -------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_PS_464XLAT
   if( (protocol == PS_IPPROTO_TCP) || (protocol == PS_IPPROTO_UDP) )
#endif
   {
     src_port = (uint16)*((uint16*)(pkt_buf + RMNET_OPT_V4_HDR_LEN));
     dst_port = (uint16)*((uint16*)(pkt_buf + RMNET_OPT_V4_HDR_LEN + 2));
   }

   /*---------------------------------------------------------------------
     Check to see if RM filters are present instead of checking IP addr.
     Only allow packets which match a filters protocol and port range to
     be pass.
   ---------------------------------------------------------------------*/
   //rm_base_iface_ptr = PS_IFACE_GET_BASE_IFACE(ds_fwk_ptr);
   //filter_ptr = PS_IFACEI_GET_OPT_FILTER_BASE(rm_base_iface_ptr);
   filter_ptr = ds_fwk_ptr->dl_opt_fltr_arr;
 
   if (rmnet_ul_opt_process_filters(filter_ptr, 
                                     src_port, 
                                     dst_port, 
                                     protocol,
                                     DS_MAX_OPT_FILTER_ENTRIES)) 
   {
     return TRUE;
   }


   /* If filters installed on iface fails, then run filters on global filter array for Rm group */
   if (rmnet_ul_opt_process_filters(ds_global_ul_opt_fltr_arr, 
                                     src_port, 
                                     dst_port, 
                                     protocol,
                                     MAX_GLOBAL_OPT_FILTER_ENTRIES)) 
   {
     return TRUE;
   }
   
  if ((((uint16) *((uint16 *) (pkt_buf +
        RMNET_OPT_V4_FRAG_OFFSET)) &
        ps_htons(RMNET_OPT_V4_FRAG_MASK)) != 0) )
   {
     return TRUE;
   }   

   return FALSE;
} /* ps_dpm_ul_opt_ipv4_is_legacy() */

/*===========================================================================
FUNCTION ps_dpm_ul_opt_ipv6_is_legacy

DESCRIPTION
  This macro returns whether the packet needs to take the legacy path or should
  be forwarded to the Um iface for IPv6. It uses optimized function to get 32
  bit values.
  Packets take the legacy path in following cases
  1. If packet is destined to a linklocal address
  2. If packet is destined to a multicast address
  3. If packet has the frag hdr
  4. If packet is ICMPv6

PARAMETERS
  pkt_buf    : pointer to the pkt buffer

RETURN VALUE
  TRUE  : if packet needs to take legacy path
  FALSE : if packet should be forwarded to the Um iface

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
INLINE boolean rmnet_ul_opt_ipv6_is_legacy
(
  uint8  * const pkt_buf
)
{
  if ( 
       ((uint8) (* (pkt_buf + RMNET_OPT_V6_NXT_HDR_OFFSET)) ==
       (uint8)PS_IPPROTO_ICMP6)           ||
  
       ((uint8) (* (pkt_buf + RMNET_OPT_V6_NXT_HDR_OFFSET)) ==
       (uint8)PS_IPPROTO_FRAG_HDR)       ||
       
       PS_IN6_IS_PREFIX_LINKLOCAL
       (rmnet_ul_opt_hget32_unaligned(pkt_buf +
         RMNET_OPT_V6_DEST_ADDR_OFFSET)) ||

       ((uint8) (* (pkt_buf + RMNET_OPT_V6_DEST_ADDR_OFFSET)) ==
       RMNET_OPT_V6_MCAST_MASK)
     )
  {
    return TRUE;
  }

  return FALSE;
} /* ps_dpm_ul_opt_ipv6_is_legacy() */

/*===========================================================================

                             EXTERNAL FUNCTIONS

===========================================================================*/
void rmnet_ul_opt_init
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Pre-allocate meta info at powerup so that it is not allocated every time
    in data path. This meta info is DUPed and sent along with each IP pkt in
    UL optimized path
  -------------------------------------------------------------------------*/
  PS_TX_META_INFO_GET_ALL(tx_meta_info_ptr);
  if (NULL == tx_meta_info_ptr)
  {
    DS_ERR_FATAL("ps_dpm_ul_opt_init(): Couldn't alloc meta info");
    return;
  }

  return;
} /* ps_dpm_ul_opt_init() */

/*===========================================================================
FUNCTION PS_DPM_UL_OPT_IP_NO_QOS_RX_SIG_HDLR

DESCRIPTION
  This function is the optimized Signal handler for IP NO QOS mode. It decides
  if the packet needs to take the legacy path or should be forwarded to Um
  iface.

PARAMETERS
 rx_sig    :  RmNet signal type
 user_data :  pointer to user data.

RETURN VALUE
 TRUE :
 FALSE:

DEPENDENCIES
  None

SIDE EFFECTS
  None.

===========================================================================*/
boolean rmnet_ul_opt_ip_no_qos_rx_sig_hdlr
(
  ps_rm_sig_enum_type    rx_sig,
  void                 * user_data
)
{
  ps_tx_meta_info_type             *dup_tx_meta_info_ptr = NULL;
  ps_tx_meta_info_type             *mo_tx_meta_info_ptr = NULL;
  dsm_item_type                    *rx_pkt;
  rmnet_ul_opt_path_type            path_type = RMNET_UL_OPT_OPT_PATH;
  boolean                           ret_val   = FALSE;
  uint8                             pkt_buf[PS_OPT_PATH_IP_NO_QOS_HEADER_LEN + 1] ALIGN(4); //__attribute__ ((aligned(4)));
  uint8                             ip_hdr_len  = 0;
  uint8                             prot_offset = 0;
  uint16                            total_len   = 0;
  uint16                            version = 0;
  uint8                             ip_hdr_first_byte = 0;  
  uint16                            buf_len = 0;
  uint8                             num_pkts = 0;
  int                               frag_result = -1;
  int16                             ds_errno = DS_ENOERR;
  ip_pkt_info_type                 *pkt_info_ptr = NULL;
  errno_enum_type                   err_no;
  ds_fwk_clnt_info_s_type           clnt_info_ptr ={0};
  ds_fwk_s_type                    *ds_fwk_inst;
  rmnet_smi_ip_info_type           *info;
  rmnet_smi_info_type              *temp_info;
  uint32                            mtu; 
  ds_flow_type                     *ds_flow_ptr =NULL;
  ds_pdn_mgr_instance_type_e        pdn_instance_type = DS_PDN_INSTANCE_IPV4;
  dpl_net_cb                       *rm_dpl = NULL;
  ds_flow_type*                     flow_ptr = NULL;
  ip_version_enum_type              ip_version;  
  ds_stats_type                    *rm_stats = NULL;
  ds_stats_type                    *um_stats = NULL;
  void                             *fraghdl = NULL;  
  boolean                           pdn_sharing = FALSE;
  ps_rx_meta_info_type             *mi_ref_ptr = NULL;
  uint8                             protocol = 0;  
  boolean                           is_mox_data_pkt = FALSE;
  errno_enum_type                   ps_errno = DS_ENOERR;
  uint8                             icmp_msg_type = 0;
  dsm_item_type                     *peek_rx_pkt;
  dsm_item_header_priority_type     dsm_priority;
  boolean                           is_flow_enabled = FALSE;
  dsm_watermark_type                *rx_wm = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_ASSERT((int)user_data);
  info = (rmnet_smi_ip_info_type *)user_data;

  rx_wm = &(info->xport_info.rx_wmk);

  if (rx_wm == NULL)
  {
    DS_MSG0(MSG_LEGACY_ERROR,"rmnet_ul_opt_ip_no_qos_rx_sig_hdlr, RM WM is NULL");
    return TRUE;
  }

  /*Peak into the first packet of the RM WM*/
  DSM_LOCK(&((rx_wm)->lock));
  peek_rx_pkt = (dsm_item_type*)q_check(rx_wm->q_ptr);
  DSM_UNLOCK(&((rx_wm)->lock));

  if (peek_rx_pkt != NULL)
  {
    dsm_priority = peek_rx_pkt->priority;
    if (dsm_priority == DSM_QUEUE_PRIORITY_HIGH)
    {
      is_mox_data_pkt = TRUE;
    }
    memcpy(&ip_hdr_first_byte, peek_rx_pkt->data_ptr,RMNET_UL_OPT_FIRST_BYTE);
    DS_MSG2(MSG_LEGACY_ERROR,"rmnet_ul_opt_ip_no_qos_rx_sig_hdlr,"
            " APP priority %d, is_mox_data_pkt %d", peek_rx_pkt->app_field,
            is_mox_data_pkt);
  }
  else
  {
	DS_MSG0(MSG_LEGACY_ERROR,"rmnet_ul_opt_ip_no_qos_rx_sig_hdlr,"
            " WM first item is NULL. No more pkts");
    return TRUE;
  }

  DS_ENTER_CRIT_SECTION(&global_ps_crit_section);
  do
  {
    /*-----------------------------------------------------------------------
      Peek in to the first byte to determine version of the IP packet and
      process the packet appropriately
    -----------------------------------------------------------------------*/
    version = ip_hdr_first_byte & RMNET_OPT_IP_VERSION_MASK;	
    /*-----------------------------------------------------------------------
      Do appropriate processing for IPv4 and IPv6 packets
    -----------------------------------------------------------------------*/
    if (version == RMNET_OPT_V4_VERSION) 
    {
      temp_info = info->info_v4;
    /*---------------------------------------------------------------------------
       Get the framework instance from the rmnet instance 
    ------------------------------------------------------------------------*/
      ds_fwk_get_clnt_info_from_handle(DS_FWK_CLIENT_RMNET,
                                       temp_info->constants.rmnet_inst,
                                       &clnt_info_ptr);
	  if(clnt_info_ptr.fw_inst == NULL)
	  {	  
        DS_MSG0(MSG_LEGACY_ERROR,"No framework instance could be found for this call");
        path_type = RMNET_UL_OPT_NO_PATH;
        break;
	  }
	  ds_fwk_inst = clnt_info_ptr.fw_inst;
      if (RMNET_META_SM_NULL_STATE == temp_info->meta_sm.state)
      {
        DS_MSG1(MSG_LEGACY_ERROR,"rmnet_ul_opt_ip_no_qos_rx_sig_hdlr(): "
                        "v4 call not UP for sig %d", rx_sig);
        path_type = RMNET_UL_OPT_NO_PATH;                        
        break;
      }
     /* To check more if call is UP or not*/
     pdn_instance_type = DS_PDN_INSTANCE_IPV4;
      ip_version = IP_V4;
    
     rm_dpl = ds_fwk_get_dpl_ptr(ds_fwk_inst,IP_V4,DS_DPL_CLIENT_RM);
     rm_stats = DS_FWK_GET_STATS_PTR(ds_fwk_inst, ip_version,DS_STATS_CLIENT_RM);

     um_stats = DS_FWK_GET_STATS_PTR(ds_fwk_inst, ip_version,DS_STATS_CLIENT_UM);
    }
    else if (version == RMNET_OPT_V6_VERSION)              /* IPv6 packet */
    {    
  	 temp_info = info->info_v6;
     /*---------------------------------------------------------------------------
         Get the framework instance from the rmnet instance 
     ------------------------------------------------------------------------*/
     ds_fwk_get_clnt_info_from_handle(DS_FWK_CLIENT_RMNET,
                                      temp_info->constants.rmnet_inst,
                                      &clnt_info_ptr);

     if(clnt_info_ptr.fw_inst == NULL)
      {
        DS_MSG0(MSG_LEGACY_ERROR,"No framework instance could be found for this call");
        path_type = RMNET_UL_OPT_NO_PATH;
        break;
     }      
     if (RMNET_META_SM_NULL_STATE == temp_info->meta_sm.state)
     {
        DS_MSG1(MSG_LEGACY_ERROR,"rmnet_ul_opt_ip_no_qos_rx_sig_hdlr(): "
               "v6 call not UP for sig %d", rx_sig);
        path_type = RMNET_UL_OPT_NO_PATH;                        
        break;
     }	 
    ds_fwk_inst = clnt_info_ptr.fw_inst;
     //To check more if call is UP or not
      pdn_instance_type = DS_PDN_INSTANCE_IPV6;          
      ip_version = IP_V6;      
      rm_dpl = ds_fwk_get_dpl_ptr(ds_fwk_inst,IP_V6,DS_DPL_CLIENT_RM);
        rm_stats = DS_FWK_GET_STATS_PTR(ds_fwk_inst, ip_version,DS_STATS_CLIENT_RM);
        um_stats = DS_FWK_GET_STATS_PTR(ds_fwk_inst, ip_version,DS_STATS_CLIENT_UM);
    }
    else
    {
      DS_MSG1(MSG_LEGACY_ERROR,"rmnet_ul_opt_ip_no_qos_rx_sig_hdlr(): "
              "Unknown IP type for sig %d", rx_sig);
      path_type = RMNET_UL_OPT_NO_PATH;
      break;
    }

    if (ds_fwk_get_default_bearer_flow_ex(ds_fwk_inst,(ip_addr_enum_type)ip_version,
                                         &flow_ptr, is_mox_data_pkt))
    {
      is_flow_enabled = TRUE;
      rx_pkt = dsm_dequeue(&(info->xport_info.rx_wmk));

      if (NULL == rx_pkt)
      {
        return TRUE;
      }
      if(ds_pdn_mgr_is_wwan_tech_suspended() == TRUE)
      {
        wwan_tech_suspend_cntr++;
        if ((1 == wwan_tech_suspend_cntr)|| !(wwan_tech_suspend_cntr % WWAN_TECH_SUSP_MAX_DBG_RATE_LIMIT))
        {
          DS_MSG0(MSG_LEGACY_ERROR,"rmnet_ul_opt_ip_no_qos_rx_sig_hdlr(): tech suspended freeing the pkt");
        }
        dsm_free_packet(&rx_pkt);
        DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
        return FALSE;
      }
      else
      {
        if (0 != wwan_tech_suspend_cntr)
        {
          /* Reset the cntr, when tech is not suspended */
          wwan_tech_suspend_cntr = 0;
        }
      }
      pkts_array[num_pkts] = rx_pkt;
    }
    else
    {
      if ((ds_fwk_inst->tx_flow_mask == DS_BEARER_TECH_SUSPENSION_MASK ||
           ds_fwk_inst->tx_flow_mask ==DS_BEARER_APN_RATE_CTRL_MOX_ONLY_MASK)&&
          (info->is_mo_ex_pkt_queued))
	  {
	    PS_RM_SET_SIGNAL(info->rmnet_ul_sig);
		info->is_mo_ex_pkt_queued = FALSE;
	  }
      DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      return TRUE;
    }

	
    if( is_mox_data_pkt && is_flow_enabled)
    {
      if( flow_ptr != NULL )
      {
        /**MO Exception packets may need to be routed even when flow is disabled*/
        if(DS_FWK_NUM_FILTERS(ds_fwk_inst, FLTR_CLIENT_MO_EXCP) > 0)
        {      
          DS_MSG0(MSG_LEGACY_HIGH,"rmnet_ul_opt_ip_no_qos_rx_sig_hdlr:"
                                  "MO exceptional filters registered");
          PS_TX_META_INFO_AND_RT_META_INFO_GET(mo_tx_meta_info_ptr);
          if(( mo_tx_meta_info_ptr != NULL )&&
             (mo_tx_meta_info_ptr->rt_meta_info_ptr != NULL))
          {
            (&(PS_TX_META_GET_PKT_INFO( mo_tx_meta_info_ptr)))->\
                  is_fraghdl_allocated = 2;//INVALID;
            if(ps_pkt_info_filter_tx_pkt(ds_fwk_inst,
                                     FLTR_CLIENT_MO_EXCP,
                                     &rx_pkt,
                                     &mo_tx_meta_info_ptr,
                                     &err_no) < 0)
            {
              DS_MSG1(MSG_LEGACY_HIGH,"rmnet_ul_opt_ip_no_qos_rx_sig_hdlr: "
                                  "ps_pkt_info_filter_tx_pkt returned ps_errono = %d",
                                   err_no);
              path_type = RMNET_UL_OPT_NO_PATH;
            }
            else
            {
              is_mox_data_pkt = (mo_tx_meta_info_ptr != NULL) &&
              (PS_TX_META_GET_FILTER_RESULT( mo_tx_meta_info_ptr, FLTR_CLIENT_MO_EXCP));
              if(!ds_fwk_is_flow_enabled_ex(flow_ptr,is_mox_data_pkt))
              {
                ret_val       = TRUE;
                path_type     = RMNET_UL_OPT_FLOW_CONTROL_PATH;
                info->rx_pkt  = rx_pkt;
                DS_MSG2(MSG_LEGACY_ERROR,"RMNET_UL_OPT_FLOW_CONTROL_PATH ip %d "
                      "fwk_inst %p ",clnt_info_ptr.ip_type,ds_fwk_inst);
              }
              else
              {
                path_type = RMNET_UL_OPT_OPT_PATH;
              }
            }
            if ((mo_tx_meta_info_ptr != NULL)
	           &&(PS_TX_META_GET_RT_META_INFO_PTR(mo_tx_meta_info_ptr) != NULL)
				 &&(NULL != (fraghdl = 
                 (&(PS_TX_META_GET_PKT_INFO( mo_tx_meta_info_ptr)))->fraghdl)) &&
                 ((&(PS_TX_META_GET_PKT_INFO( mo_tx_meta_info_ptr)))->\
                     is_fraghdl_allocated == 1 ))//allocated
            {
              (&(PS_TX_META_GET_PKT_INFO( mo_tx_meta_info_ptr)))->\
                  is_fraghdl_allocated = 0;
              ip_fraghdl_free(fraghdl);
              (&(PS_TX_META_GET_PKT_INFO( mo_tx_meta_info_ptr)))->fraghdl = NULL;
			  
            }
          }
          PS_TX_META_INFO_FREE(&mo_tx_meta_info_ptr);
          if( path_type != RMNET_UL_OPT_OPT_PATH )
              break;
        }
        else
        {
          ret_val       = TRUE;
          path_type     = RMNET_UL_OPT_FLOW_CONTROL_PATH;
          info->rx_pkt  = rx_pkt;
          DS_MSG2(MSG_LEGACY_ERROR,"RMNET_UL_OPT_FLOW_CONTROL_PATH ip %d "
                "fwk_inst %p ",clnt_info_ptr.ip_type,ds_fwk_inst);
          break;
       }
      }
      else
      {
        path_type = RMNET_UL_OPT_NO_PATH;
        DS_FWK_INC_INSTANCE_STATS(rm_stats, pkts_dropped_rx, 1);
        DS_MSG2(MSG_LEGACY_ERROR,"No default bearer could be found ip %d "
              "fwk_inst %p ",clnt_info_ptr.ip_type,ds_fwk_inst);
        break;
      }
    }   

    if (version == RMNET_OPT_V4_VERSION)
    {
      mtu = ds_fwk_get_mtu(ds_fwk_inst,IPV4_ADDR);
      /*---------------------------------------------------------------------
        Fetch IHL and use it to extract IP header and port information from
        the IP packet
      ---------------------------------------------------------------------*/
      ip_hdr_len = (ip_hdr_first_byte & RMNET_OPT_IP_HDR_LEN_MASK) << 2;

      if( (ip_hdr_len + RMNET_OPT_TCP_HDR_LEN_EXTRACT) > 
          (PS_OPT_PATH_IP_NO_QOS_HEADER_LEN + 1) )
      {
        path_type = RMNET_UL_OPT_LEGACY_PATH;
        break;
      }

      buf_len = dsm_extract(rx_pkt,
                            0,
                            pkt_buf,
                            ip_hdr_len + RMNET_OPT_TCP_HDR_LEN_EXTRACT);
      if ((ip_hdr_len + RMNET_OPT_TCP_HDR_LEN_EXTRACT) > buf_len)
      {
        path_type = RMNET_UL_OPT_LEGACY_PATH;
        break;
      }
      
      /*---------------------------------------------------------------------
        Get the offset of protocol field and also the packet's total length
      ---------------------------------------------------------------------*/
      prot_offset = RMNET_OPT_V4_PROT_FIELD_OFFSET;
      total_len   =
        ps_htons(*((uint16 *)(pkt_buf + RMNET_OPT_V4_TOTAL_LEN_OFFSET)));

        DS_MSG3(MSG_LEGACY_ERROR,"v4  packet ip_hdr_len %d total len %d rm_dpl id %d",        
                ip_hdr_len,total_len,rm_dpl->dpl_id.ifname);

      /*-----------------------------------------------------------------------
        Check if packet needs to be sent up the legacy path
      -----------------------------------------------------------------------*/
      if (TRUE ==
            rmnet_ul_opt_ipv4_is_legacy(pkt_buf, ds_fwk_inst, ip_hdr_len))
      {
        path_type = RMNET_UL_OPT_LEGACY_PATH;  
        break;
      }
      //Check if PDN sharing is enabled 
      pdn_sharing = ds_fwk_is_pdn_shared(ds_fwk_inst ,RMNET_OPT_V4_VERSION);
      if(pdn_sharing == TRUE)
      {     
        //Check for ICMP for PDN sharing 
        protocol = *(pkt_buf + RMNET_OPT_V4_PROT_FIELD_OFFSET);
        if(protocol == PS_IPPROTO_ICMP)
        {
          if( ps_rx_pkt_info_generate(&rx_pkt, &mi_ref_ptr, &ps_errno, FALSE) == -1)
          {
            path_type = RMNET_UL_OPT_LEGACY_PATH;
            break;
          }
          pkt_info_ptr = &(mi_ref_ptr->pkt_info);
          if((pkt_info_ptr->ip_vsn == IP_V4 )&&
             (pkt_info_ptr->ip_hdr.v4.protocol == PS_IPPROTO_ICMP ) &&
             (pkt_info_ptr->ptcl_info.icmp.type == (uint8)ICMP_ECHO_REQ))
          {
            //store the ID and seqence number
            ds_fwk_set_rmnet_embedded_icmp_packet_info(pkt_info_ptr,
                                                  pkt_info_ptr->ip_vsn,
                                                  rx_pkt,
                                                  temp_info->constants.rmnet_inst);
          }
        }
     }
   }
   else
   {
     mtu = ds_fwk_get_mtu(ds_fwk_inst,IPV6_ADDR);
      /*---------------------------------------------------------------------
        Extract IP header and port information from the IP packet
      ---------------------------------------------------------------------*/
      buf_len = dsm_extract(rx_pkt,
                            0,
                            pkt_buf,
                            RMNET_OPT_V6_HDR_LEN + RMNET_OPT_TCP_HDR_LEN_EXTRACT);
                           
      if ((RMNET_OPT_V6_HDR_LEN + RMNET_OPT_TCP_HDR_LEN_EXTRACT) > buf_len)
      {
        path_type = RMNET_UL_OPT_LEGACY_PATH;
        break;
      }
    /*---------------------------------------------------------------------
      Get the offset of protocol field, IP header length, and also the
      packet's total length
    ---------------------------------------------------------------------*/
     total_len =
        ps_htons(*((uint16 *)(pkt_buf + RMNET_OPT_V6_TOTAL_LEN_OFFSET)));
     total_len  += RMNET_OPT_V6_HDR_LEN;
     ip_hdr_len  = RMNET_OPT_V6_HDR_LEN;
     prot_offset = RMNET_OPT_V6_NXT_HDR_OFFSET;

     DS_MSG3(MSG_LEGACY_ERROR,"v6  packet ip_hdr_len %d total len %d rm_dpl id %d",
             ip_hdr_len,total_len,rm_dpl->dpl_id.ifname);

   /*---------------------------------------------------------------------
      Drop the packet if the packet's source prefix doesn't match Um's prefix
    ---------------------------------------------------------------------*/
    if (FALSE == rmnet_ul_opt_ipv6_prefix_cmp(pkt_buf, ds_fwk_inst))
    {
       DS_MSG0(MSG_LEGACY_ERROR,"rmnet_ul_opt_ip_no_qos_rx_sig_hdlr(): "
                                "IPV6 prefix cmp failed");      
       DS_FWK_INC_INSTANCE_STATS(rm_stats, pkts_dropped_rx, 1);
       path_type = RMNET_UL_OPT_NO_PATH;
       break;
    }   
    //Check if PDN sharing is enabled 
    pdn_sharing = ds_fwk_is_pdn_shared(ds_fwk_inst ,RMNET_OPT_V6_VERSION);
    if(pdn_sharing == TRUE)
    {        
      //Check for ICMP protocol for PDN sharing 
      protocol = (uint8) (* (pkt_buf + RMNET_OPT_V6_NXT_HDR_OFFSET));	
      if(protocol == PS_IPPROTO_ICMP6)
      {
        if( ps_rx_pkt_info_generate(&rx_pkt, &mi_ref_ptr, &ps_errno, FALSE) == -1)
        {
          path_type = RMNET_UL_OPT_LEGACY_PATH;  
          break;           
        }
        pkt_info_ptr = &(mi_ref_ptr->pkt_info);
        if((pkt_info_ptr->ip_vsn == IP_V6 )&&
           (pkt_info_ptr->ip_hdr.v6.hdr_type == PS_IPPROTO_ICMP6 ) &&
           (pkt_info_ptr->ptcl_info.icmp.type == ICMP6_ECHO_REQ))
        {
          //store the ID and seqence number
          ds_fwk_set_rmnet_embedded_icmp_packet_info(pkt_info_ptr,
                                                     pkt_info_ptr->ip_vsn,
                                                     rx_pkt,
                                                     temp_info->constants.rmnet_inst);
        }
      }
    }
  /*-----------------------------------------------------------------------
      Check if packet needs to be sent up the legacy path
    -----------------------------------------------------------------------*/
    if (TRUE == rmnet_ul_opt_ipv6_is_legacy(pkt_buf))
    {
      path_type = RMNET_UL_OPT_LEGACY_PATH;
      if(protocol == PS_IPPROTO_ICMP6)
      {
        icmp_msg_type = (uint8) (* (pkt_buf + RMNET_OPT_V6_HDR_LEN));
        if((icmp_msg_type >= ICMP6_RTR_SOL) && (icmp_msg_type <= ICMP6_NGBR_ADV))
        {
          path_type = RMNET_UL_OPT_LEGACY_PATH;
        }
        else
        {
          path_type = RMNET_UL_OPT_OPT_PATH;
        }
      }
      break;
    }
  }
   
    /*-----------------------------------------------------------------------
      Check if PDN is flow controlled and if so, do not process the packet
      further. There are two scenarios to consider.
        1. Single IP call - Um is flow controlled if corresponding Um iface
           is flow controlled
        2. Dual-IP over single RmNet call (Single APN/Dual APN) or 
           Dual-IP over different RmNet call- Um is
           flow controlled only if both the corresponding v4 and v6 Um ifaces
           are flow controlled

      Detailed algorithm is
       1. To check if the PDN is flow controlled or not run the QOS filters
          and if get the ds_flow_ptr fom filters api.
       2. After getting the flow_ptr check if the flow is enabled or not by 
           calling the FWL api.
       3. If the flow is enabled then process the packet else cache the packet
       4.Do the rest of the processing if flow enabled.
    -----------------------------------------------------------------------*/

    /*-----------------------------------------------------------------------
      Step 1
    -----------------------------------------------------------------------*/
    if(-1 != ps_tx_pkt_info_generate(&rx_pkt, &dup_tx_meta_info_ptr, &err_no))
    {
      pkt_info_ptr = &(PS_TX_META_GET_PKT_INFO(dup_tx_meta_info_ptr));
      pkt_info_ptr->if_ptr =  ds_fwk_inst;          
      if( !ds_fwk_get_data_flow_ex(ds_fwk_inst,
                               (ip_addr_enum_type)ip_version,
                                pkt_info_ptr,
                               &ds_flow_ptr,
                                is_mox_data_pkt))
      {
        DS_MSG1(MSG_LEGACY_HIGH,"flow is disabled %d "
        "fwk_inst %p ",ds_fwk_inst);
        PS_TX_META_INFO_FREE(&dup_tx_meta_info_ptr);
        pkt_info_ptr = NULL;
        if( ds_flow_ptr != NULL )
        {
          ret_val                 = TRUE;
          path_type               = RMNET_UL_OPT_FLOW_CONTROL_PATH;
          info->rx_pkt            = rx_pkt;
          break;
        }
        else
        {
          path_type = RMNET_UL_OPT_NO_PATH; 
          DS_FWK_INC_INSTANCE_STATS(rm_stats, pkts_dropped_rx, 1);
          DS_MSG2(MSG_LEGACY_ERROR,"No default bearer could be found ip %d "
            "fwk_inst %p ",clnt_info_ptr.ip_type,ds_fwk_inst);
           break;
        }
      }
      DS_MSG1(MSG_LEGACY_HIGH,"flow is enabled "
        "ds_flow_ptr %p ",ds_flow_ptr);
      PS_TX_META_INFO_FREE(&dup_tx_meta_info_ptr);
      pkt_info_ptr = NULL;
    }
    else
    {
      PS_TX_META_INFO_FREE(&dup_tx_meta_info_ptr);
      pkt_info_ptr = NULL;
      path_type = RMNET_UL_OPT_NO_PATH;
      DS_MSG2(MSG_LEGACY_ERROR,"No default bearer could be found ip %d "
        "fwk_inst %p ",clnt_info_ptr.ip_type,ds_fwk_inst);
      DS_FWK_INC_INSTANCE_STATS(rm_stats, pkts_dropped_rx, 1);
      break;
    }
    DS_MSG2(MSG_LEGACY_ERROR,"mtu %d total_len %d ",
                      mtu, total_len);

    if (total_len > mtu)
    {    
      frag_result = rmnet_ul_opt_fragment_packet (
                                           rx_pkt,
                                           ds_fwk_inst,
                                           mtu,
                                           pkts_array,
                                           version,
                                           total_len,
                                           &ds_errno);
      if (-1 == frag_result)
      {
        DS_MSG1(MSG_LEGACY_ERROR,"rmnet_ul_opt_fragment_packet failed %d",mtu);
        DS_FWK_INC_INSTANCE_STATS(rm_stats, pkts_dropped_rx, 1);
        path_type = RMNET_UL_OPT_NO_PATH;
      }   
    }      
  } while(0);
 
  DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);  
 /*-------------------------------------------------------------------
    Loop through all the pkts in array and send them one by one
  -------------------------------------------------------------------*/    
  while (num_pkts < RMNET_UL_OPT_MAX_FRAG_PACKETS &&
         NULL != pkts_array[num_pkts])
  {
    DS_ENTER_CRIT_SECTION(&global_ps_crit_section);
   
    if (path_type != RMNET_UL_OPT_OPT_PATH)
    {
      goto send_data;
    }
    
    rx_pkt = pkts_array[num_pkts]; 

    /*-----------------------------------------------------------------------
      Increment packet stats on Rm side
    -----------------------------------------------------------------------*/
    DS_FWK_INC_INSTANCE_STATS(rm_stats, pkts_rx, 1);
    DS_FWK_INC_INSTANCE_STATS(rm_stats, bytes_rx, total_len);
    /*-----------------------------------------------------------------------

     Increment packet stats on Um side
    -----------------------------------------------------------------------*/
    DS_FWK_INC_INSTANCE_STATS(um_stats, pkts_tx, 1);
    DS_FWK_INC_INSTANCE_STATS(um_stats, bytes_tx, total_len);
    
    /*-----------------------------------------------------------------------
      NOTE: AFTER THIS POINT, THERE MUST NOT BE ANY ERROR AND PACKET MUST BE
      SENT TO Um IFACE VIA OPTIMIZED PATH, ELSE UNDUP THE META INFO
    -----------------------------------------------------------------------*/

    /*---------------------------------------------------------------------
        TCP ACK prioritization
    ---------------------------------------------------------------------*/
    if (rmnet_ul_opt_tcp_ack_pri_check
         (
            pkt_buf,
            ip_hdr_len,
            total_len,
            prot_offset
          ) == TRUE
       )
    {
      DS_MSG0(MSG_LEGACY_LOW,"IP_NO_QOS: TCP_ACK_PRIORITY: Tagging DSM_HIGHEST");
      (rx_pkt)->priority = DSM_HIGHEST;
    }     

   /*---------------------------------------------------------------------
	  TCP packet to dst_port 21 prioritization
   ---------------------------------------------------------------------*/	   
	if (rmnet_ul_opt_ftp_ctrl_port_pri_check
	    (
          pkt_buf,
          ip_hdr_len,
          prot_offset
        ) == TRUE
       )
	 {
       (rx_pkt)->priority = DSM_HIGHEST;
     }

    PS_TX_META_INFO_DUP(tx_meta_info_ptr, &dup_tx_meta_info_ptr);

    /*-----------------------------------------------------------------------
      Update meta info with routing cache and QoS filter result
    -----------------------------------------------------------------------*/
    PS_TX_META_SET_ROUTING_CACHE(dup_tx_meta_info_ptr, ds_fwk_inst);
    PS_TX_META_SET_FILTER_RESULT(dup_tx_meta_info_ptr,
                                 FLTR_CLIENT_QOS_OUTPUT,
                                 (uint32)ds_flow_ptr);

    dup_tx_meta_info_ptr->rt_meta_info_ptr->pkt_info.ip_vsn = ip_version;

    PS_TX_META_RESET_FILTER_RESULT(dup_tx_meta_info_ptr, FLTR_CLIENT_MO_EXCP );

	/*-------------------------------------------------------------------------
      Execute MO exceptional filters and update filter result in Tx meta info when packet matches
      with MO Exceptional filters. QoS and Legacy path filter would run in ps_iface_tx_cmd
    -------------------------------------------------------------------------*/
    
    if(DS_FWK_NUM_FILTERS(ds_fwk_inst, FLTR_CLIENT_MO_EXCP) > 0)
    {      
      /*Invalid Packet info so that Protocol generate Packet info */	
      PS_RT_META_INVALIDATE_PKT_INFO(PS_TX_META_GET_RT_META_INFO_PTR(dup_tx_meta_info_ptr)) ;
  
      DS_MSG0(MSG_LEGACY_HIGH,"rmnet_ul_opt_ip_no_qos_rx_sig_hdlr: Iface has "
                               "MO exceptional filters registered");
      if(ps_pkt_info_filter_tx_pkt(ds_fwk_inst,
                                   FLTR_CLIENT_MO_EXCP,
                                   &rx_pkt,
                                   &dup_tx_meta_info_ptr,
                                   &err_no) < 0)
      {
        DS_MSG1(MSG_LEGACY_HIGH,"rmnet_ul_opt_ip_no_qos_rx_sig_hdlr: "
                                "ps_pkt_info_filter_tx_pkt returned ps_errono = %d",
                                 err_no);
      }
    }
send_data :

   /*-------------------------------------------------------------------------
     Leave CS before tx_cmd to avoid possible deadlock issues
   -------------------------------------------------------------------------*/
    DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

    /*-------------------------------------------------------------------------
      Process the packet based on the 'path_type'
    -------------------------------------------------------------------------*/
    switch (path_type)
    {
      case RMNET_UL_OPT_OPT_PATH:
      {
        /*-----------------------------------------------------------------------
          Log the packet on the Rm iface
        -----------------------------------------------------------------------*/
         DS_MSG1(MSG_LEGACY_HIGH,"rmnet_ul_opt_ip_no_qos_rx_sig_hdlr: "
                               "optimised path logging packet on the RX on Rm dplid%d ",
                                    rm_dpl->dpl_id.ifname);
#ifdef PS_LOGGING_IOE_DPL    
         DPL_LOG_NETWORK_RX_PACKET(rm_dpl, rx_pkt, DPL_IID_NETPROT_IP);
#endif //PS_LOGGING_IOE_DPL
		 DS_FWK_GLOBAL_STATS_INC;
         ds_fwk_default_tx_cmd(ds_fwk_inst,
                                &rx_pkt,
                                dup_tx_meta_info_ptr
                                );
        break;
      }
      case RMNET_UL_OPT_LEGACY_PATH:
      {   
        if(rx_pkt !=NULL)
        {
      /*-----------------------------------------------------------------------
        Log the packet on the Rm iface
     -----------------------------------------------------------------------*/
         DS_MSG1(MSG_LEGACY_HIGH,"rmnet_ul_opt_ip_no_qos_rx_sig_hdlr: "
                                 "legacy path logging packet on the RX on Rm dplid%d ",
                                      rm_dpl->dpl_id.ifname);
#ifdef PS_LOGGING_IOE_DPL    
           DPL_LOG_NETWORK_RX_PACKET(rm_dpl, rx_pkt, DPL_IID_NETPROT_IP);
#endif //PS_LOGGING_IOE_DPL
          ret_val = rmnet_data_ul_legacy_input(temp_info,rx_pkt, ds_fwk_inst);
        }
        break;
      }
      case RMNET_UL_OPT_FLOW_CONTROL_PATH:
      {
        DS_MSG1(MSG_LEGACY_HIGH,"rmnet_ul_opt_ip_no_qos_rx_sig_hdlr(): "
                               "PDN is flow controlled, sig %d", rx_sig);
        break;
      }
      case RMNET_UL_OPT_NO_PATH:
        DS_MSG0(MSG_LEGACY_ERROR,"rmnet_ul_opt_ip_no_qos_rx_sig_hdlr(): "
                               "No path is set so dropping packet ");
      default:
      {
       /*---------------------------------------------------------------------
         Check if rx packet needs to be free'd or not
       ---------------------------------------------------------------------*/
       if (DS_EINVAL != ds_errno)
       {
          DS_MSG0(MSG_LEGACY_ERROR,"rmnet_ul_opt_ip_no_qos_rx_sig_hdlr(): "
                               "No path is set so dropping packet ds_errno is!= DS_EINVAL");
          dsm_free_packet(&rx_pkt);
       }
       break;
      }
    }
    pkts_array[num_pkts] = NULL;
    num_pkts++;
  }
  return ret_val;
} /* rmnet_ul_opt_ip_no_qos_rx_sig_hdlr() */

#ifdef FEATURE_DATA_QMI_QOS
/*===========================================================================
FUNCTION PS_DPM_UL_OPT_IP_QOS_RX_SIG_HDLR

DESCRIPTION
  This function is the optimized Signal handler for IP NO QOS mode. It decides
  if the packet needs to take the legacy path or should be forwarded to Um
  iface.

PARAMETERS
 rx_sig    :  RmNet signal type
 user_data :  pointer to user data.

RETURN VALUE
 TRUE :
 FALSE:

DEPENDENCIES
  None

SIDE EFFECTS
  None.

===========================================================================*/

boolean rmnet_ul_opt_ip_qos_rx_sig_hdlr
(
  ps_rm_sig_enum_type    rx_sig,
  void                 * user_data
)
{
  ps_tx_meta_info_type            * dup_tx_meta_info_ptr = NULL;
  dsm_item_type                   * rx_pkt;
  ps_rmnet_qos_hdr_type             qos_hdr;
  ps_rmnet_qos2_hdr_type            qos2_hdr;
  rmnet_ul_opt_path_type            path_type   = RMNET_UL_OPT_OPT_PATH;
  ip_addr_enum_type                ip_version;
  boolean                           ret_val     = FALSE;
  uint8                             pkt_buf[PS_OPT_PATH_IP_QOS2_HEADER_LEN + 1] ALIGN(4); //__attribute__ ((aligned(4)));
  uint8                             ip_hdr_len  = 0;
  uint8                             prot_offset = 0;
  uint16                            total_len   = 0;
  uint16                            version = 0;
  uint8                             ip_hdr_first_byte;  
  uint16                            buf_len = 0;
  uint8                             num_pkts = 0;
  int                               frag_result = -1;
  int16                             ds_errno = DS_ENOERR;
  uint8                             qos_hdr_format;
  uint8                             qos_hdr_offset = 0;
  errno_enum_type                   err_no;
  rmnet_smi_ip_info_type           *info =NULL;
  ds_fwk_clnt_info_s_type           clnt_info_ptr ={0};
  ds_fwk_s_type                     *ds_fwk_inst;
  rmnet_smi_info_type              *temp_info;
  uint32                            mtu; 
  ds_flow_type                     *ds_flow_ptr =NULL;
  ds_bearer_mgr_s                  *ds_bearer_ptr =NULL;
  ds_pdn_mgr_instance_type_e       pdn_instance_type = DS_PDN_INSTANCE_IPV4;  
  ip_pkt_info_type                 *pkt_info_ptr = NULL;  
  uint32                            fi_result = DS_IPFLTR_NOMATCH;	
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_ASSERT((int)user_data);
  info = (rmnet_smi_ip_info_type *)user_data;
  qos_hdr_format = info->xport_info.data_format_cache.qos_format;

  if (RMNET_QOS_FORMAT_8_BYTE == qos_hdr_format)
  {
    qos_hdr_offset = RMNET_OPT_QOS2_HEADER_OFFSET;
  }
  else
  {
    qos_hdr_offset = RMNET_OPT_QOS_HEADER_OFFSET;
  }
  /*-------------------------------------------------------------------------
    If rx_pkt is outstanding from previous time, use it. Otherwise, dequeue
    a new packet
  -------------------------------------------------------------------------*/
  if (NULL != info->rx_pkt)
  {
    rx_pkt                  = info->rx_pkt;
    info->rx_pkt = NULL; 

    DS_MSG1(MSG_LEGACY_HIGH,"rmnet_ul_opt_ip_qos_rx_sig_hdlr(): "
                             "Using outstanding pkt, sig %d", rx_sig);
  }
  else
  {
    rx_pkt = dsm_dequeue(&(info->xport_info.rx_wmk));
    if (NULL == rx_pkt)
    {
      return TRUE;
    }
  }

  pkts_array[num_pkts] = rx_pkt;

  DS_ENTER_CRIT_SECTION(&global_ps_crit_section);
  do
  {
  
    /*-----------------------------------------------------------------------
      Peek in to the first byte to determine version of the IP packet and
      process the packet appropriately
    -----------------------------------------------------------------------*/
    (void) dsm_extract(rx_pkt, qos_hdr_offset, &ip_hdr_first_byte, 1);

    version = ip_hdr_first_byte & RMNET_OPT_IP_VERSION_MASK;
    
    /*-----------------------------------------------------------------------
      Do appropriate processing for IPv4 and IPv6 packets
    -----------------------------------------------------------------------*/
    if (version == RMNET_OPT_V4_VERSION) 
    {
      temp_info = info->info_v4;
      ds_fwk_get_clnt_info_from_handle(DS_FWK_CLIENT_RMNET,
                                       temp_info->constants.rmnet_inst,
                                       &clnt_info_ptr);
	  if(clnt_info_ptr.fw_inst == NULL)
	  {	  
        DS_MSG0(MSG_LEGACY_ERROR,"No framework instance could be found for this call");
        path_type = RMNET_UL_OPT_NO_PATH;
        break;
	  }
	  ds_fwk_inst = clnt_info_ptr.fw_inst;
      if (RMNET_META_SM_NULL_STATE == temp_info->meta_sm.state)
      {
        DS_MSG1(MSG_LEGACY_ERROR,"rmnet_ul_opt_ip_qos_rx_sig_hdlr(): "
                        "v4 call not UP for sig %d", rx_sig);
        path_type = RMNET_UL_OPT_NO_PATH;                        
        break;
      }
     /* To check more if call is UP or not*/
     pdn_instance_type = DS_PDN_INSTANCE_IPV4;
    }
    else if (version == RMNET_OPT_V6_VERSION)              /* IPv6 packet */
    {
      temp_info = info->info_v6;
      ds_fwk_get_clnt_info_from_handle(DS_FWK_CLIENT_RMNET,
                                       temp_info->constants.rmnet_inst,
                                       &clnt_info_ptr);

      if(clnt_info_ptr.fw_inst == NULL)
	  {	  
       DS_MSG0(MSG_LEGACY_ERROR,"No framework instance could be found for this call");
        path_type = RMNET_UL_OPT_NO_PATH;
        break;
      }      
	 if (RMNET_META_SM_NULL_STATE == temp_info->meta_sm.state)
    {
        DS_MSG1(MSG_LEGACY_ERROR,"rmnet_ul_opt_ip_qos_rx_sig_hdlr(): "
                        "v6 call not UP for sig %d", rx_sig);
        path_type = RMNET_UL_OPT_NO_PATH;                        
        break;
      }      
     ds_fwk_inst = clnt_info_ptr.fw_inst;
     //To check more if call is UP or not
      pdn_instance_type = DS_PDN_INSTANCE_IPV6;
    }
    else
    {
      DS_MSG1(MSG_LEGACY_ERROR,"rmnet_ul_opt_ip_qos_rx_sig_hdlr(): "
                      "Unknown IP type for sig %d", rx_sig);
      path_type   = RMNET_UL_OPT_NO_PATH;
      break;
    }

    /*-----------------------------------------------------------------------
     Check if PDN is flow controlled and if so, do not process the packet
      further. There are two scenarios to consider.
        1. Single IP call - Um is flow controlled if corresponding Um iface
           is flow controlled
       2. Dual-IP over single RmNet call (Single APN/Dual APN) or 
          Dual-IP over different RmNet call- Um is
           flow controlled only if both the corresponding v4 and v6 Um ifaces
           are flow controlled

      Detailed algorithm is
      1. To check if the PDN is flow controlled or not run the QOS filters
         and if get the ds_flow_ptr fom filters api.
      2. After getting the flow_ptr check if the flow is enabled or not by 
          calling the FWL api.
      3. If the flow is enabled then process the packet else cache the packet
    -----------------------------------------------------------------------*/

    /*-----------------------------------------------------------------------
      Step 1
    -----------------------------------------------------------------------*/
    if (DS_FWK_NUM_FILTERS(ds_fwk_inst,FLTR_CLIENT_QOS_OUTPUT) > 0)
	{
     if(-1 != ps_tx_pkt_info_generate(&rx_pkt, &dup_tx_meta_info_ptr, &err_no))
    {
        pkt_info_ptr = &(PS_TX_META_GET_PKT_INFO(dup_tx_meta_info_ptr));
        pkt_info_ptr->if_ptr =  ds_fwk_inst;          
      }
      fi_result = ds_ipfltr_execute(ds_fwk_inst->ds_fwk_index,FLTR_CLIENT_QOS_OUTPUT,
                                    DS_IPFLTR_SUBSET_ID_DEFAULT,pkt_info_ptr);
      PS_TX_META_INFO_FREE(&dup_tx_meta_info_ptr);
      pkt_info_ptr = NULL;
      
       ds_flow_ptr = (ds_flow_type *)fi_result;
       DS_MSG1(MSG_LEGACY_HIGH ,"Found flow ptr %p ",ds_flow_ptr);
    }
	if((fi_result == DS_IPFLTR_NOMATCH)||(DS_FWK_NUM_FILTERS(ds_fwk_inst,FLTR_CLIENT_QOS_OUTPUT) <= 0))
    {
        DS_MSG1(MSG_LEGACY_HIGH,"rmnet_ul_opt_ip_qos_rx_sig_hdlr(): "
                "ds_flow is not found by running QOS filters check for default flow ",
                 ds_flow_ptr);
        ds_bearer_ptr = ds_fwk_get_default_bearer(ds_fwk_inst ,clnt_info_ptr.ip_type);
        if(ds_bearer_ptr == NULL)
        {
           path_type = RMNET_UL_OPT_NO_PATH;
           DS_MSG2(MSG_LEGACY_ERROR,"No default bearer could be found ip %d "
           "fwk_inst %p ",clnt_info_ptr.ip_type,ds_fwk_inst);
          break;
        }
      else
      {
         //find the default flow 
         ds_flow_ptr = ds_flow_mgr_get_default_flow_from_bearer(ds_bearer_ptr,pdn_instance_type);
         if(ds_flow_ptr == NULL)
         {
            path_type = RMNET_UL_OPT_NO_PATH;
            DS_MSG2(MSG_LEGACY_ERROR,"No default flow could be found ip %d "
             "ds_bearer_ptr %p ",clnt_info_ptr.ip_type,ds_bearer_ptr);
           break;
         }
      }
    }
     //Call the framework api to check the flow and also about the 
     //companion flow  

    if(ds_fwk_is_flow_enabled(ds_flow_ptr) == FALSE)
    {
       ret_val                 = TRUE;
       path_type               = RMNET_UL_OPT_FLOW_CONTROL_PATH;
       info->rx_pkt            = rx_pkt;
       break;
    }   

    if (version == RMNET_OPT_V4_VERSION)
    {
      /*---------------------------------------------------------------------
        Fetch IHL and use it to extract IP header and port information from
        the IP packet
      ---------------------------------------------------------------------*/
      ip_hdr_len = (ip_hdr_first_byte & RMNET_OPT_IP_HDR_LEN_MASK) << 2;

      if( (ip_hdr_len + RMNET_OPT_TCP_HDR_LEN_EXTRACT) > 
                      (PS_OPT_PATH_IP_QOS2_HEADER_LEN + 1) )
      {
        path_type = RMNET_UL_OPT_LEGACY_PATH;
        break;
      }

      buf_len = dsm_extract(rx_pkt,
                            qos_hdr_offset,
                            pkt_buf,
                            ip_hdr_len + RMNET_OPT_TCP_HDR_LEN_EXTRACT);

      if ((ip_hdr_len + RMNET_OPT_TCP_HDR_LEN_EXTRACT) > buf_len)
      {
        path_type = RMNET_UL_OPT_LEGACY_PATH;
        break;
      }
      
      /*---------------------------------------------------------------------
        Get the offset of protocol field and also the packet's total length
      ---------------------------------------------------------------------*/
      prot_offset = RMNET_OPT_V4_PROT_FIELD_OFFSET;
      total_len   =
        ps_htons(*((uint16 *)(pkt_buf + RMNET_OPT_V4_TOTAL_LEN_OFFSET))); 
        
      ip_version = IPV4_ADDR;

      /*-----------------------------------------------------------------------
        Check if packet needs to be sent up the legacy path
      -----------------------------------------------------------------------*/
      if (TRUE 
          == rmnet_ul_opt_ipv4_is_legacy(pkt_buf, ds_fwk_inst, ip_hdr_len))
      {
        path_type = RMNET_UL_OPT_LEGACY_PATH;
        break;
      }	  
     mtu = ds_fwk_get_mtu(ds_fwk_inst,IPV4_ADDR);
    }
    else
    {
      /*---------------------------------------------------------------------
        Extract IP header and port information from the IP packet
      ---------------------------------------------------------------------*/
      buf_len = dsm_extract(rx_pkt,
                            qos_hdr_offset,
                            pkt_buf,
                            RMNET_OPT_V6_HDR_LEN + RMNET_OPT_TCP_HDR_LEN_EXTRACT);

      if ((RMNET_OPT_V6_HDR_LEN + RMNET_OPT_TCP_HDR_LEN_EXTRACT) > buf_len)
      {
        path_type = RMNET_UL_OPT_LEGACY_PATH;
        break;
      }
      
      /*---------------------------------------------------------------------
        Get the offset of protocol field, IP header length, and also the
        packet's total length
      ---------------------------------------------------------------------*/
      total_len =
        ps_htons(*((uint16 *)(pkt_buf + RMNET_OPT_V6_TOTAL_LEN_OFFSET)));
      total_len  += RMNET_OPT_V6_HDR_LEN;
      ip_hdr_len  = RMNET_OPT_V6_HDR_LEN;
      prot_offset = RMNET_OPT_V6_NXT_HDR_OFFSET;

      /*-----------------------------------------------------------------------
        Check if packet needs to be sent up the legacy path
      -----------------------------------------------------------------------*/
      if (TRUE == rmnet_ul_opt_ipv6_is_legacy(pkt_buf))
      {
        path_type = RMNET_UL_OPT_LEGACY_PATH;
        break;
      }

      /*---------------------------------------------------------------------
        Drop the packet if the packet's source prefix doesn't match Rm's prefix
      ---------------------------------------------------------------------*/
      if (FALSE == rmnet_ul_opt_ipv6_prefix_cmp(pkt_buf, ds_fwk_inst))
      {
        DS_MSG0(MSG_LEGACY_ERROR,"rmnet_ul_opt_ip_qos_rx_sig_hdlr(): "
                        "IPV6 prefix cmp failed");      
        path_type = RMNET_UL_OPT_NO_PATH;
        break;
      }
      
      mtu = ds_fwk_get_mtu(ds_fwk_inst,IPV6_ADDR);
      ip_version = IPV6_ADDR;
    }

    /*-----------------------------------------------------------------------
      Pull out QoS hdr & assign the rx_pkt back since it might get modified
      by dsm_pullup
    -----------------------------------------------------------------------*/
    if (RMNET_QOS_FORMAT_8_BYTE == qos_hdr_format)
    {    
      (void) dsm_pullup(&rx_pkt, &qos2_hdr, sizeof(qos2_hdr));
    }
    else
    {
      (void) dsm_pullup(&rx_pkt, &qos_hdr, sizeof(qos_hdr));
    }

    pkts_array[num_pkts] = rx_pkt;
    
    if (total_len > mtu )
    {     
      frag_result = rmnet_ul_opt_fragment_packet (
                                           rx_pkt,
                                           ds_fwk_inst,
                                           mtu,
                                           pkts_array,
                                           version,
                                           total_len,
                                           &ds_errno);
      if (-1 == frag_result)
      {
        DS_MSG1(MSG_LEGACY_ERROR,"rmnet_ul_opt_fragment_packet failed %d",mtu);
        path_type = RMNET_UL_OPT_NO_PATH;
      }    
    }

  } while(0);
  
  DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  
  /*-------------------------------------------------------------------
    Loop through all the pkts in array and send them one by one
  -------------------------------------------------------------------*/    
  while (num_pkts < RMNET_UL_OPT_MAX_FRAG_PACKETS && 
          NULL != pkts_array[num_pkts]) 
  {
    DS_ENTER_CRIT_SECTION(&global_ps_crit_section);
   
    if (path_type != RMNET_UL_OPT_OPT_PATH)
    {
      goto send_data;
    }
    
    rx_pkt = pkts_array[num_pkts]; 
#if 0
    /*-----------------------------------------------------------------------
      Increment packet stats on Rm iface
    -----------------------------------------------------------------------*/
    rm_iface_ptr->iface_i_stats.pkts_rx++;
    rm_iface_ptr->iface_i_stats.bytes_rx += total_len;

    /*-----------------------------------------------------------------------
      Log the packet on the Rm iface
    -----------------------------------------------------------------------*/
#ifdef PS_LOGGING_IOE_DPL
    DPL_LOG_NETWORK_RX_PACKET(rm_iface_ptr, rx_pkt, DPL_IID_NETPROT_IP);
#endif //PS_LOGGING_IOE_DPL
#endif

    PS_TX_META_INFO_DUP(tx_meta_info_ptr, &dup_tx_meta_info_ptr);
    if (RMNET_QOS_FORMAT_8_BYTE == qos_hdr_format)
    {
      rmnet_process_qos_hdr(rx_pkt,
                             ds_fwk_inst,
                             ip_version,
                             TRUE,
                             dup_tx_meta_info_ptr,
                             (void *)&qos2_hdr,
                             qos_hdr_format,
                             &ds_flow_ptr);
    }
    else
    {
      rmnet_process_qos_hdr(rx_pkt,
                             ds_fwk_inst,
                             ip_version,
                             TRUE,
                             dup_tx_meta_info_ptr,
                             (void*)&qos_hdr,
                             qos_hdr_format,
                             &ds_flow_ptr);
    }
#if 0
    /*-----------------------------------------------------------------------
      Increment packet stats on Um iface
    -----------------------------------------------------------------------*/
    um_base_iface_ptr->iface_i_stats.pkts_tx++;
    um_base_iface_ptr->iface_i_stats.bytes_tx += total_len;

    /*-----------------------------------------------------------------------
      Log packet on the Um iface
    -----------------------------------------------------------------------*/
#ifdef PS_LOGGING_IOE_DPL
    DPL_LOG_NETWORK_TX_PACKET(um_base_iface_ptr, rx_pkt, DPL_IID_NETPROT_IP);
#endif //PS_LOGGING_IOE_DPL
#endif
    /*-----------------------------------------------------------------------
      If flow capability is set to PS_FLOW_CAPABILITY_DATA_DISALLOWED,
      drop the packet
    -----------------------------------------------------------------------*/
    if (ds_flow_get_capability(ds_flow_ptr, DS_FLOW_CAPABILITY_DATA_DISALLOWED))
    {
      DS_MSG1(MSG_LEGACY_HIGH,"rmnet_ul_opt_ip_qos_rx_sig_hdlr(): "
                      "ds_flow 0x%p is DS_FLOW_CAPABILITY_DATA_DISALLOWED",
                      ds_flow_ptr);
      path_type = RMNET_UL_OPT_NO_PATH;
      goto send_data;
    }

    /*---------------------------------------------------------------------
        TCP ACK prioritization
    ---------------------------------------------------------------------*/
    if (rmnet_ul_opt_tcp_ack_pri_check
          (
            pkt_buf,
            ip_hdr_len,
            total_len,
            prot_offset
          ) == TRUE
       )
    {
      DS_MSG0(MSG_LEGACY_LOW,"IP_NO_QOS: TCP_ACK_PRIORITY: Tagging DSM_HIGHEST");
      (rx_pkt)->priority = DSM_HIGHEST;
    }     

#if 0
    /*-----------------------------------------------------------------------
      Log packet on ps_flow and increment phys link stats
    -----------------------------------------------------------------------*/
#ifdef PS_LOGGING_IOE_DPL
    DPL_LOG_FLOW_TX_PACKET(ps_flow_ptr, rx_pkt, DPL_IID_NETPROT_IP);
#endif //PS_LOGGING_IOE_DPL

    if (NULL != ps_phys_link_ptr)
    {
      ps_phys_link_ptr->phys_link_i_stats.pkts_tx++;
    }
#endif
    /*-----------------------------------------------------------------------
      NOTE: COULD HAVE THIS CODE OUTSIDE THE DO-WHILE LOOP, BUT DOING IT
      THIS WAY SAVES ONE IF CHECK
    -----------------------------------------------------------------------*/
    PS_TX_META_RESET_FILTER_RESULT(dup_tx_meta_info_ptr, FLTR_CLIENT_MO_EXCP );
    /*-------------------------------------------------------------------------
      Execute MO exceptional filters and update filter result in Tx meta info when packet matches
      with MO Exceptional filters
    -------------------------------------------------------------------------*/
    if( DS_FWK_NUM_FILTERS(ds_fwk_inst, FLTR_CLIENT_MO_EXCP) > 0)
    {
      DS_MSG0(MSG_LEGACY_HIGH,"rmnet_ul_opt_ip_qos_rx_sig_hdlr: Iface has "
                               "MO exceptional filters registered");
      if(ps_pkt_info_filter_tx_pkt(ds_fwk_inst,
                                   FLTR_CLIENT_MO_EXCP,
                                   &rx_pkt,
                                   &dup_tx_meta_info_ptr,
                                   &err_no) < 0)
      {
        DS_MSG1(MSG_LEGACY_HIGH,"rmnet_ul_opt_ip_qos_rx_sig_hdlr: "
                                 "ps_pkt_info_filter_tx_pkt returned ps_errono = %d",
                                 err_no);
      }
    }
	
  send_data:

   /*-------------------------------------------------------------------------
      Leave CS before tx_cmd to avoid possible deadlock issues
    -------------------------------------------------------------------------*/
    DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

    /*-------------------------------------------------------------------------
      Process the packet based on the 'path_type'
    -------------------------------------------------------------------------*/
    switch (path_type)
    {
      case RMNET_UL_OPT_OPT_PATH:
      {
         ds_fwk_default_tx_cmd(ds_fwk_inst,
                               &rx_pkt,
                               dup_tx_meta_info_ptr
          );
        break;
      }
      case RMNET_UL_OPT_LEGACY_PATH:
      {
        if(rx_pkt !=NULL)
          ret_val = rmnet_data_ul_legacy_input(temp_info,rx_pkt, ds_fwk_inst);
        break;
      }
      case RMNET_UL_OPT_FLOW_CONTROL_PATH:
      {
        DS_MSG1(MSG_LEGACY_HIGH,"rmnet_ul_opt_ip_qos_rx_sig_hdlr(): "
                               "PDN is flow controlled, sig %d", rx_sig);
        break;
      }
      case RMNET_UL_OPT_NO_PATH:
        DS_MSG0(MSG_LEGACY_ERROR,"rmnet_ul_opt_ip_qos_rx_sig_hdlr(): "
                               "No path is set so dropping packet ");
      default:
      {
        /*---------------------------------------------------------------------
          Check if rx packet needs to be free'd or not
        ---------------------------------------------------------------------*/
        if (DS_EINVAL != ds_errno)
        {
          DS_MSG0(MSG_LEGACY_ERROR,"rmnet_ul_opt_ip_qos_rx_sig_hdlr(): "
                 "No path is set so dropping packet ds_errno is!= DS_EINVAL");
          dsm_free_packet(&rx_pkt);
        }
        break;
      }
    }
    pkts_array[num_pkts] = NULL;
    num_pkts++;
  }
  return ret_val;
} /* ps_dpm_ul_opt_ip_qos_rx_sig_hdlr() */
#endif /* FEATURE_DATA_QMI_QOS */


/*===========================================================================
FUNCTION PS_DPM_UL_OPT_NON_IP_RX_SIG_HDLR

DESCRIPTION
  This function drops any data for non IP call becuase for Non IP call
  there is no data expected on uplink path via A2. Uplink packets will
  arrive via QMI control message

PARAMETERS
  rx_sig    : RmNet signal type.
  user_data : pointer to user data.

RETURN VALUE
 TRUE:
 FALSE:

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/

#if 0
boolean rmnet_ul_opt_non_ip_rx_sig_hdlr
(
  ps_rm_sig_enum_type    rx_sig,
  void                 * user_data
)
{
  dsm_item_type       * rx_pkt;
  rmnet_smi_ip_info_type  *info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_ASSERT((int)user_data);
  info = (rmnet_smi_ip_info_type *)user_data;


  if (NULL != info)
  {
    rx_pkt = dsm_dequeue(&(info->xport_info.rx_wmk));
    if (NULL == rx_pkt)
    {
      return TRUE;
    }
    DS_MSG0(MSG_LEGACY_HIGH,"rmnet_ul_opt_non_ip_rx_sig_hdlr : Dropping packet for "
             "non ip call as not data is expected on uplink");
    dsm_free_packet(&rx_pkt);
  }
    
  return FALSE;
} /* ps_dpm_ul_opt_non_ip_rx_sig_hdlr */
#endif

#if 0 
//XLAT FEATURE is still not decided so keeping it commented
/*===========================================================================
FUNCTION ps_dpm_ul_opt_clat_ip_no_qos_rx_sig_hdlr

DESCRIPTION
  This function is the optimized Signal handler for IP NO QOS mode. It decides
  if the packet needs to take the legacy path or should be forwarded to Um
  iface.

PARAMETERS
 rx_sig    :  RmNet signal type
 user_data :  pointer to user data.

RETURN VALUE
 TRUE :
 FALSE:

DEPENDENCIES
  None

SIDE EFFECTS
  None.

===========================================================================*/
boolean rmnet_ul_opt_clat_ip_no_qos_rx_sig_hdlr
(
  ps_rm_sig_enum_type    rx_sig,
  void                 * user_data
)
{
  ps_dpm_rm_info_type             * dpm_rm_info_ptr      = NULL;
  ps_dpm_um_info_type             * v4_dpm_um_info_ptr   = NULL;
  ps_dpm_um_info_type             * v6_dpm_um_info_ptr   = NULL;
  ps_tx_meta_info_type            * dup_tx_meta_info_ptr = NULL;
  ps_phys_link_type               * ps_phys_link_ptr;
  ps_phys_link_type               * companion_ps_phys_link_ptr;
  ps_flow_type                    * ps_flow_ptr;
  ps_flow_type                    * companion_ps_flow_ptr;
  ps_iface_type                   * rm_iface_ptr = NULL;
  ps_iface_type                   * um_iface_ptr = NULL;
  ps_iface_type                   * um_base_iface_ptr = NULL;
  ps_iface_type                   * companion_um_iface_ptr = NULL;
  ps_iface_type                   * logical_iface_ptr = NULL;
  dsm_item_type                   * rx_pkt;
  ps_dpm_ul_opt_path_type           path_type = PS_DPM_UL_OPT_OPT_PATH;
  boolean                           ret_val   = FALSE;
#ifdef FEATURE_DATA_QMI_QOS
  ps_tx_meta_info_type            * qos_tx_meta_info_ptr = NULL;
#endif
  uint8                             pkt_buf[PS_OPT_PATH_IP_NO_QOS_HEADER_LEN + 1] ALIGN(4); //__attribute__ ((aligned(4)));
  uint8                             ip_hdr_len  = 0;
  uint8                             prot_offset = 0;
  uint16                            total_len   = 0;
  uint16                            version = 0;
  uint8                             ip_hdr_first_byte;  
  uint16                            buf_len = 0;
  boolean                           enable      = FALSE;
  sint15                            ps_errno;
  uint8                             num_pkts = 0;
  int                               frag_result = -1;
  int16                             ds_errno = DS_ENOERR;
  boolean                           is_pkt_trnsltd = FALSE;
  errno_enum_type                   err_no;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  dpm_rm_info_ptr = (ps_dpm_rm_info_type *) user_data;

  v4_dpm_um_info_ptr = dpm_rm_info_ptr->v4_um_info_ptr;
  v6_dpm_um_info_ptr = dpm_rm_info_ptr->v6_um_info_ptr;

  /*-------------------------------------------------------------------------
    If rx_pkt is outstanding from previous time, use it. Otherwise, dequeue
    a new packet
  -------------------------------------------------------------------------*/
  if (NULL != dpm_rm_info_ptr->rx_pkt)
  {
    rx_pkt                  = dpm_rm_info_ptr->rx_pkt;
    dpm_rm_info_ptr->rx_pkt = NULL;

    LOG_MSG_DATA_PATH_INFO_1("ps_dpm_ul_opt_clat_ip_no_qos_rx_sig_hdlr(): "
                             "Using outstanding pkt, sig %d", rx_sig );
  }
  else
  {
    rx_pkt = dsm_dequeue(dpm_rm_info_ptr->sio_rx_wm_ptr);
    if (NULL == rx_pkt)
    {
      return TRUE;
    }
  }

  pkts_array[num_pkts] = rx_pkt;

  DS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  do
  {        
    /*-----------------------------------------------------------------------
      Peek in to the first byte to determine version of the IP packet and
      process the packet appropriately
    -----------------------------------------------------------------------*/
    (void) dsm_extract(rx_pkt, 0, &ip_hdr_first_byte, 1);

    version = ip_hdr_first_byte & PS_DPM_OPT_IP_VERSION_MASK;

    /*-------------------------------------------------------------------------
      1. Setup iface info for v4 packets & for v6 packets
      2. Handle flow control
      3. For v4 packets first fragment if required & then translate them to v6.
         This step is not required for v6 packets
      4. Send out the translated v6 packets
    -------------------------------------------------------------------------*/
    if (version == PS_DPM_OPT_V4_VERSION) 
    {
      /*-----------------------------------------------------------------------
        Do appropriate processing for IPv4 packets
      -----------------------------------------------------------------------*/
      if (NULL == v4_dpm_um_info_ptr)
      {
        LOG_MSG_ERROR_1("ps_dpm_ul_opt_clat_ip_no_qos_rx_sig_hdlr(): "
                        "v4 call not UP for sig %d", rx_sig);
        path_type = PS_DPM_UL_OPT_NO_PATH;                        
        break;
      }

      rm_iface_ptr = dpm_rm_info_ptr->v4_iface_ptr; 

      um_iface_ptr = dpm_rm_info_ptr->v4_logical_iface_ptr;     

      /*-----------------------------------------------------------------------
        For companion_um_iface_ptr we first get the CLAT iface & then get the 
        associated logical iface
      -----------------------------------------------------------------------*/
      companion_um_iface_ptr = (NULL == v6_dpm_um_info_ptr)
                                 ? NULL
                                 : dpm_rm_info_ptr->v6_logical_iface_ptr;
      if (PS_IFACE_IS_VALID(companion_um_iface_ptr))
      {
        companion_um_iface_ptr = 
                            PS_IFACEI_GET_ASSOC_IFACE(companion_um_iface_ptr);
      }
    }

    else if(version == PS_DPM_OPT_V6_VERSION)
    {
      /*-----------------------------------------------------------------------
        Do appropriate processing for IPv6 packets
      -----------------------------------------------------------------------*/
      if (NULL == v6_dpm_um_info_ptr)
      {
        LOG_MSG_ERROR_1("ps_dpm_ul_opt_clat_ip_no_qos_rx_sig_hdlr(): "
                        "v6 call not UP for sig %d", rx_sig);
        path_type = PS_DPM_UL_OPT_NO_PATH;                        
        break;
      }

      rm_iface_ptr = dpm_rm_info_ptr->v6_iface_ptr; 

      um_iface_ptr = dpm_rm_info_ptr->v6_logical_iface_ptr; 

      /*-----------------------------------------------------------------------
        For companion_um_iface_ptr we first get the CLAT iface & then get the 
        associated logical iface
      -----------------------------------------------------------------------*/
      companion_um_iface_ptr = (NULL == v4_dpm_um_info_ptr)
                                 ? NULL
                                 : dpm_rm_info_ptr->v4_logical_iface_ptr;
      if (PS_IFACE_IS_VALID(companion_um_iface_ptr))
      {
        companion_um_iface_ptr = 
                            PS_IFACEI_GET_ASSOC_IFACE(companion_um_iface_ptr);
      }
    }

    else 
    {
      LOG_MSG_ERROR_1("ps_dpm_ul_opt_clat_ip_no_qos_rx_sig_hdlr(): "
                      "Unknown IP type for sig %d", rx_sig);
      path_type   = PS_DPM_UL_OPT_NO_PATH;
      break;
    }

    /*-------------------------------------------------------------------
      To handle packet being sent when call is being brought down
    -------------------------------------------------------------------*/
    if (NULL == um_iface_ptr)
    {
      LOG_MSG_ERROR_2("ps_dpm_ul_opt_clat_ip_no_qos_rx_sig_hdlr(): "
                      "Call in down or going down state um iface 0x%p "
                      "rm iface 0x%p ", um_iface_ptr, rm_iface_ptr);
      path_type = PS_DPM_UL_OPT_NO_PATH;
      break;
    }
    
    um_base_iface_ptr = PS_DPM_GET_BASE_IFACE(um_iface_ptr);

    /*-----------------------------------------------------------------------
      For checking flow control, logical iface need to be used. Hence get
      logical iface info from CLAT um iface

      Check if Um is flow controlled and if so, do not process the packet
      further. There are two scenarios to consider.
        1. Single IP call - Um is flow controlled if corresponding Um iface
           is flow controlled
        2. Dual-IP over single RmNet call (Single APN/Dual APN) or 
           Dual-IP over different RmNet call- Um is
           flow controlled only if both the corresponding v4 and v6 Um ifaces
           are flow controlled

      Detailed algorithm is
        1. Check if Um is flow controlled. If it is not, then there is
           no flow control. If it is, go to step 2
        2. Check if companion Um iface is registered with DPM. If it is not,
           then Um is flow controlled. If it is, go to step 3 else goto step
           4.
        3. Check if companion Um is flow controlled. If it is not, then there is
           no flow control. If it is, Um is flow controlled, goto step4
        4 If Um is flow controlled, cache the packet as outstanding packet
           in DPM and process it later
    -----------------------------------------------------------------------*/

    /*-----------------------------------------------------------------------
      Step 1
    -----------------------------------------------------------------------*/
    logical_iface_ptr = PS_IFACEI_GET_ASSOC_IFACE(um_iface_ptr);
	if(!PS_IFACE_IS_VALID(logical_iface_ptr))
	{
	  LOG_MSG_ERROR_1("ps_dpm_ul_opt_clat_ip_qos_rx_sig_hdlr():"
	  	              "failed to find associated um_iface_ptr 0x%p",
	  	              um_iface_ptr);
      path_type = PS_DPM_UL_OPT_NO_PATH;
	  break;
	}
    ps_flow_ptr      = PS_IFACEI_GET_DEFAULT_FLOW(logical_iface_ptr);
    ps_phys_link_ptr = PS_IFACEI_GET_PHYS_LINK(logical_iface_ptr);

    if (!(dpm_rm_info_ptr->te_flow_control || 
          PS_FLOW_IS_TX_ENABLED(ps_flow_ptr)) ||
        !(dpm_rm_info_ptr->te_flow_control || 
          PS_PHYS_LINK_FLOW_ENABLED(ps_phys_link_ptr)) ||
        !(PS_IFACEI_FLOW_ENABLED(logical_iface_ptr)))
    {
      /*---------------------------------------------------------------------
        Step 2
      ---------------------------------------------------------------------*/
      if (NULL != companion_um_iface_ptr)
      {
        /*-------------------------------------------------------------------
          Step 3
        -------------------------------------------------------------------*/
        companion_ps_flow_ptr =
          PS_IFACEI_GET_DEFAULT_FLOW(companion_um_iface_ptr);
        companion_ps_phys_link_ptr =
          PS_IFACEI_GET_PHYS_LINK(companion_um_iface_ptr);

        if (!(dpm_rm_info_ptr->te_flow_control || 
              PS_FLOW_IS_TX_ENABLED(companion_ps_flow_ptr)) ||
            !(dpm_rm_info_ptr->te_flow_control || 
              PS_PHYS_LINK_FLOW_ENABLED(companion_ps_phys_link_ptr)) ||
            !(PS_IFACEI_FLOW_ENABLED(companion_um_iface_ptr)))
        {
          /*-----------------------------------------------------------------
            Step 4
          -----------------------------------------------------------------*/
          ret_val                 = TRUE;
          path_type               = PS_DPM_UL_OPT_FLOW_CONTROL_PATH;
          dpm_rm_info_ptr->rx_pkt = rx_pkt;
          break;
        }
      }
      else
      {
        /*-------------------------------------------------------------------
          Step 4
        -------------------------------------------------------------------*/
        ret_val                 = TRUE;
        path_type               = PS_DPM_UL_OPT_FLOW_CONTROL_PATH;
        dpm_rm_info_ptr->rx_pkt = rx_pkt;
        break;
      }
    }

    if (version == PS_DPM_OPT_V4_VERSION) 
    {    
      if(um_base_iface_ptr->clat_pdn_cntxt == NULL)
      {
        LOG_MSG_ERROR_2("ps_dpm_ul_opt_clat_ip_no_qos_rx_sig_hdlr(): "
                        "clat pdn cntx NULL for iface 0x%x:%d", 
                         um_base_iface_ptr->name, um_base_iface_ptr->instance);
        path_type = PS_DPM_UL_OPT_NO_PATH;
        break;
      }
      
      /*---------------------------------------------------------------------
        Fetch IHL and use it to extract IP header and port information from
        the IP packet
      ---------------------------------------------------------------------*/
      ip_hdr_len = (ip_hdr_first_byte & PS_DPM_OPT_IP_HDR_LEN_MASK) << 2;

      if( (ip_hdr_len + PS_DPM_OPT_TCP_HDR_LEN_EXTRACT) > 
                (PS_OPT_PATH_IP_NO_QOS_HEADER_LEN + 1) )
      {
        path_type = PS_DPM_UL_OPT_LEGACY_PATH;
        break;
      }

      buf_len = dsm_extract(rx_pkt,
                            0,
                            pkt_buf,
                            ip_hdr_len + PS_DPM_OPT_TCP_HDR_LEN_EXTRACT);
      if ((ip_hdr_len + PS_DPM_OPT_TCP_HDR_LEN_EXTRACT) > buf_len)
      {
        path_type = PS_DPM_UL_OPT_LEGACY_PATH;
        break;
      }    
            
      if (TRUE ==
            ps_dpm_ul_opt_ipv4_is_legacy(pkt_buf, 
                                         rm_iface_ptr, 
                                         ip_hdr_len))
      {
        path_type = PS_DPM_UL_OPT_LEGACY_PATH;
        break;
      }

      /*---------------------------------------------------------------------
              LOW LATENCY TRAFFIC handling
             ---------------------------------------------------------------------*/
       if( ds3g_get_config_latency_info())
       {
         ps_dpm_ul_v4_low_latency_traffic_handler ( pkt_buf,
                                                    ip_hdr_len,
                                                    prot_offset );
       }

      /*---------------------------------------------------------------------
        Get the packet's total length
      ---------------------------------------------------------------------*/
      total_len   =
           ps_htons(*((uint16 *)(pkt_buf + PS_DPM_OPT_V4_TOTAL_LEN_OFFSET)));
      
     /*-----------------------------------------------------------------------
        Increment packet stats on Rm iface
      -----------------------------------------------------------------------*/
      rm_iface_ptr->iface_i_stats.pkts_rx++;
      rm_iface_ptr->iface_i_stats.bytes_rx += total_len;

      /*-----------------------------------------------------------------------
        Log the packet on the Rm iface before it is fragmented/translated
      -----------------------------------------------------------------------*/
#ifdef PS_LOGGING_IOE_DPL
      DPL_LOG_NETWORK_RX_PACKET(rm_iface_ptr, rx_pkt, DPL_IID_NETPROT_IP);
#endif //PS_LOGGING_IOE_DPL
      if (total_len > PS_IFACE_GET_MTU(um_iface_ptr))
      {    
        frag_result = ps_dpm_ul_opt_fragment_packet (
                                           rx_pkt,
                                           rm_iface_ptr,
                                           PS_IFACE_GET_MTU(um_iface_ptr),
                                           pkts_array,
                                           version,
                                           total_len,
                                           &ds_errno);
        if (-1 == frag_result)
        {
          path_type = PS_DPM_UL_OPT_NO_PATH;
          break;
        }   
      }
                
      while ((num_pkts < PS_DPM_UL_OPT_MAX_FRAG_PACKETS) && 
             (NULL != pkts_array[num_pkts]))
      {
        /* Translate v4 packet to v6 */
        if (-1 == ps_clat_ip_to_ip6(&(pkts_array[num_pkts]),
                                    rm_iface_ptr,
                                    ((ps_dpm_clat_pdn_cntxt_info_type *)
                                     (um_base_iface_ptr->clat_pdn_cntxt))->client_pdn_handle))
        {
          path_type = PS_DPM_UL_OPT_NO_PATH;

          /*-------------------------------------------------------------------
            To make sure we dont drop the pkt since protcols would have already
            dropped
          -------------------------------------------------------------------*/
          ds_errno = DS_EINVAL;
          break;
        }

        num_pkts++;
      }

      /*---------------------------------------------------------------------
        Reset num_pkts & rx_pkt which will be used when packets are sent out
      ---------------------------------------------------------------------*/	      
      num_pkts = 0;
      rx_pkt = pkts_array[num_pkts];

      is_pkt_trnsltd = TRUE;
    }
  } while(0);
  
  DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  
  
  /*-------------------------------------------------------------------
    Loop through all the pkts in array and send them one by one
  -------------------------------------------------------------------*/    
  while (num_pkts < PS_DPM_UL_OPT_MAX_FRAG_PACKETS && 
           NULL != pkts_array[num_pkts])
  {
    DS_ENTER_CRIT_SECTION(&global_ps_crit_section);
    
    /*-------------------------------------------------------------------------
      We need to point rx_pkt to point to right fragment when a pkt is 
      fragmented and then they need to be queued to legacy watermark 
    -------------------------------------------------------------------------*/
    rx_pkt = pkts_array[num_pkts]; 
   
    if (path_type != PS_DPM_UL_OPT_OPT_PATH)
    {
      goto send_data;
    }
    
    /*---------------------------------------------------------------------
      Extract IP header and port information from the IP packet
    ---------------------------------------------------------------------*/
    buf_len = dsm_extract(rx_pkt,
                          0,
                          pkt_buf,
                          PS_DPM_OPT_V6_HDR_LEN + PS_DPM_OPT_TCP_HDR_LEN_EXTRACT);

    if ((PS_DPM_OPT_V6_HDR_LEN + PS_DPM_OPT_TCP_HDR_LEN_EXTRACT) > buf_len)
    {
      path_type = PS_DPM_UL_OPT_LEGACY_PATH;
      goto send_data;
    }

    /*---------------------------------------------------------------------
      Get the offset of protocol field, IP header length, and also the
      packet's total length
    ---------------------------------------------------------------------*/
    total_len =
      ps_htons(*((uint16 *)(pkt_buf + PS_DPM_OPT_V6_TOTAL_LEN_OFFSET)));
    total_len  += PS_DPM_OPT_V6_HDR_LEN;
    ip_hdr_len  = PS_DPM_OPT_V6_HDR_LEN;
    prot_offset = PS_DPM_OPT_V6_NXT_HDR_OFFSET;

    /*-----------------------------------------------------------------------
      Check if packet needs to be sent up the legacy path
    -----------------------------------------------------------------------*/
    if (TRUE == ps_dpm_ul_opt_ipv6_is_legacy(pkt_buf))
    {
      path_type = PS_DPM_UL_OPT_LEGACY_PATH;
      goto send_data;
    } 

    /*-----------------------------------------------------------------------
      Drop the packet if the packet's source prefix doesn't match Rm's prefix.
      Applicable only to regular v6 packets
    -----------------------------------------------------------------------*/
    if ((FALSE  == is_pkt_trnsltd) && 
         (FALSE == ps_dpm_ul_opt_ipv6_prefix_cmp(pkt_buf, rm_iface_ptr)))
    {
      LOG_MSG_ERROR_0("ps_dpm_ul_opt_clat_ip_no_qos_rx_sig_hdlr(): "
                      "IPV6 prefix cmp failed");      
      path_type = PS_DPM_UL_OPT_NO_PATH;
      goto send_data;
    }

    if (FALSE == is_pkt_trnsltd)
    {
      /*-----------------------------------------------------------------------
        If the pkt got translated then it is already logged. Hence this is NA.
        Increment packet stats on Rm iface
      -----------------------------------------------------------------------*/
      rm_iface_ptr->iface_i_stats.pkts_rx++;
      rm_iface_ptr->iface_i_stats.bytes_rx += total_len;

      /*-----------------------------------------------------------------------
        Log the packet on the Rm iface
      -----------------------------------------------------------------------*/
#ifdef PS_LOGGING_IOE_DPL
      DPL_LOG_NETWORK_RX_PACKET(rm_iface_ptr, rx_pkt, DPL_IID_NETPROT_IP);
#endif //PS_LOGGING_IOE_DPL
    }

#ifdef FEATURE_DATA_QMI_QOS
    if (PS_IFACEI_NUM_FILTERS(um_base_iface_ptr, IP_FLTR_CLIENT_QOS_OUTPUT) > 0)
    {
      path_type = PS_DPM_UL_OPT_QOS_PATH;
      goto send_data;
    }
#endif /* FEATURE_DATA_QMI_QOS */
    /*-----------------------------------------------------------------------
      Increment packet stats on Um iface
    -----------------------------------------------------------------------*/
    um_base_iface_ptr->iface_i_stats.pkts_tx++;
    um_base_iface_ptr->iface_i_stats.bytes_tx += total_len;

    /*-----------------------------------------------------------------------
      Log packet on the Um iface
    -----------------------------------------------------------------------*/
    PS_DPM_GLOBAL_STATS_INC(um_base_iface_ptr);
#ifdef PS_LOGGING_IOE_DPL
    DPL_LOG_NETWORK_TX_PACKET(um_base_iface_ptr, rx_pkt, DPL_IID_NETPROT_IP);
#endif //PS_LOGGING_IOE_DPL

    /*-----------------------------------------------------------------------
      If flow capability is set to PS_FLOW_CAPABILITY_DATA_DISALLOWED,
      drop the packet
    -----------------------------------------------------------------------*/
    if (PS_FLOW_GET_CAPABILITY(ps_flow_ptr, PS_FLOW_CAPABILITY_DATA_DISALLOWED))
    {
      LOG_MSG_INFO2_1("ps_dpm_ul_opt_clat_ip_no_qos_rx_sig_hdlr(): "
                      "ps_flow 0x%p is PS_FLOW_CAPABILITY_DATA_DISALLOWED",
                      ps_flow_ptr);
      path_type = PS_DPM_UL_OPT_NO_PATH;
      goto send_data;
    }

    /*-----------------------------------------------------------------------
      NOTE: AFTER THIS POINT, THERE MUST NOT BE ANY ERROR AND PACKET MUST BE
      SENT TO Um IFACE VIA OPTIMIZED PATH, ELSE UNDUP THE META INFO
    -----------------------------------------------------------------------*/

    path_type = PS_DPM_UL_OPT_OPT_PATH;

    /*---------------------------------------------------------------------
        TCP ACK prioritization
    ---------------------------------------------------------------------*/
    if (ps_dpm_ul_opt_tcp_ack_pri_check
          (
            pkt_buf,
            ip_hdr_len,
            total_len,
            prot_offset
          ) == TRUE
       )
    {
      LOG_MSG_INFO3_0("IP_NO_QOS: TCP_ACK_PRIORITY: Tagging DSM_HIGHEST");
      (rx_pkt)->priority = DSM_HIGHEST;
    }     

    PS_TX_META_INFO_DUP(tx_meta_info_ptr, &dup_tx_meta_info_ptr);

    /*-----------------------------------------------------------------------
      Update meta info with routing cache and QoS filter result
    -----------------------------------------------------------------------*/
    PS_TX_META_SET_ROUTING_CACHE(dup_tx_meta_info_ptr, um_base_iface_ptr);
    PS_TX_META_SET_FILTER_RESULT(dup_tx_meta_info_ptr,
                                 IP_FLTR_CLIENT_QOS_OUTPUT,
                                 (uint32) ps_flow_ptr);
                                 
    /*-----------------------------------------------------------------------
      Log packet on ps_flow and increment phys link stats
    -----------------------------------------------------------------------*/
#ifdef PS_LOGGING_IOE_DPL
    DPL_LOG_FLOW_TX_PACKET(ps_flow_ptr, rx_pkt, DPL_IID_NETPROT_IP);
#endif //PS_LOGGING_IOE_DPL

    if (NULL != ps_phys_link_ptr)
    {
      ps_phys_link_ptr->phys_link_i_stats.pkts_tx++;
    }

    /*-----------------------------------------------------------------------
      Make sure that the power save mode is disabled after recieving the first 
      uplink packet on this path
    -----------------------------------------------------------------------*/
    if( TRUE == PS_IFACE_GET_POWERSAVE_FILTERING_MODE( rm_iface_ptr ) )
    {
      if( PS_IFACE_GET_POWERSAVE_FILTERING_AUTO_EXIT_MODE( rm_iface_ptr ) )
      {
        ps_iface_set_powersave_filtering_mode( rm_iface_ptr, enable, &ps_errno );
      }
    }
    PS_TX_META_RESET_FILTER_RESULT(dup_tx_meta_info_ptr, IP_FLTR_CLIENT_MO_EXCP );
    /*-------------------------------------------------------------------------
      Execute MO exceptional filters and update filter result in Tx meta info when packet matches
      with MO Exceptional filters
    -------------------------------------------------------------------------*/
    if( PS_IFACEI_NUM_FILTERS(um_base_iface_ptr, IP_FLTR_CLIENT_MO_EXCP) > 0)
    {
      LOG_MSG_DATA_PATH_INFO_0("ps_dpm_ul_opt_ip_no_qos_rx_sig_hdlr: Iface has "
                               "MO exceptional filters registered");
      if(ps_pkt_info_filter_tx_pkt(um_base_iface_ptr,
                                   IP_FLTR_CLIENT_MO_EXCP,
                                   &rx_pkt,
                                   &dup_tx_meta_info_ptr,
                                   &err_no) < 0)
      {
        LOG_MSG_DATA_PATH_INFO_1("ps_dpm_ul_opt_ip_no_qos_rx_sig_hdlr: "
                                 "ps_pkt_info_filter_tx_pkt returned ps_errono = %d",
                                 err_no);
      }
    }

send_data :

    /*-------------------------------------------------------------------------
      Leave CS before tx_cmd to avoid possible deadlock issues
    -------------------------------------------------------------------------*/
    DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

    /*-------------------------------------------------------------------------
      Process the packet based on the 'path_type'
    -------------------------------------------------------------------------*/
    switch (path_type)
    {
      case PS_DPM_UL_OPT_OPT_PATH:
      {
          um_base_iface_ptr->iface_private.tx_cmd
          (
            um_base_iface_ptr,
            &rx_pkt,
            dup_tx_meta_info_ptr,
            um_base_iface_ptr->iface_private.tx_cmd_info
          );
        break;
      }

#ifdef FEATURE_DATA_QMI_QOS
      case PS_DPM_UL_OPT_QOS_PATH:
      {
        PS_TX_META_INFO_AND_RT_META_INFO_GET(qos_tx_meta_info_ptr);
        if (qos_tx_meta_info_ptr == NULL ||
            PS_TX_META_GET_RT_META_INFO_PTR(qos_tx_meta_info_ptr) == NULL)
        {
          LOG_MSG_ERROR_1("ps_dpm_ul_opt_clat_ip_no_qos_rx_sig_hdlr() Cannot allocate "
                          "meta info tx 0x%p", qos_tx_meta_info_ptr);
          dsm_free_packet(&rx_pkt);
          rx_pkt = NULL;
          break;
        }

          /*-----------------------------------------------------------------------
            Make sure that the power save mode is disabled after recieving the first 
            uplink packet on this path
            -----------------------------------------------------------------------*/
          if( TRUE == PS_IFACE_GET_POWERSAVE_FILTERING_MODE( rm_iface_ptr ) )
          {
            if( PS_IFACE_GET_POWERSAVE_FILTERING_AUTO_EXIT_MODE( rm_iface_ptr ) )
            {
              ps_iface_set_powersave_filtering_mode( rm_iface_ptr, enable, &ps_errno );
            }
          }
        
        PS_TX_META_SET_ROUTING_CACHE(qos_tx_meta_info_ptr, um_base_iface_ptr);  
        ps_iface_tx_cmd(um_base_iface_ptr, &rx_pkt, qos_tx_meta_info_ptr);
        break;
      }
#endif /* FEATURE_DATA_QMI_QOS */

      case PS_DPM_UL_OPT_LEGACY_PATH:
      {
        ret_val = ps_dpm_ul_legacy_input(rm_iface_ptr, rx_pkt, dpm_rm_info_ptr);
        break;
      }

      case PS_DPM_UL_OPT_FLOW_CONTROL_PATH:
      {
        LOG_MSG_INFO1_1("ps_dpm_ul_opt_clat_ip_no_qos_rx_sig_hdlr(): "
                        "Um is flow controlled, sig %d", rx_sig);
        break;
      }

      case PS_DPM_UL_OPT_NO_PATH:
      default:
      {
        /*---------------------------------------------------------------------
          Check if rx packet needs to be free'd or not
        ---------------------------------------------------------------------*/
        if (DS_EINVAL != ds_errno)
        {
          dsm_free_packet(&rx_pkt);
        }
        break;
      }
    }

    pkts_array[num_pkts] = NULL;
    num_pkts++;
  }

  return ret_val;
} /* ps_dpm_ul_opt_clat_ip_no_qos_rx_sig_hdlr() */

#ifdef FEATURE_DATA_QMI_QOS
/*===========================================================================
FUNCTION PS_DPM_UL_OPT_CLAT_IP_QOS_RX_SIG_HDLR

DESCRIPTION
  This function is the optimized Signal handler for IP NO QOS mode. It decides
  if the packet needs to take the legacy path or should be forwarded to Um
  iface.

PARAMETERS
 rx_sig    :  RmNet signal type
 user_data :  pointer to user data.

RETURN VALUE
 TRUE :
 FALSE:

DEPENDENCIES
  None

SIDE EFFECTS
  None.

===========================================================================*/
boolean rmnetul_opt_clat_ip_qos_rx_sig_hdlr
(
  ps_rm_sig_enum_type    rx_sig,
  void                 * user_data
)
{
  ps_dpm_rm_info_type             * dpm_rm_info_ptr      = NULL;
  ps_dpm_um_info_type             * v4_dpm_um_info_ptr   = NULL;
  ps_dpm_um_info_type             * v6_dpm_um_info_ptr   = NULL;
  ps_tx_meta_info_type            * dup_tx_meta_info_ptr = NULL;
  ps_phys_link_type               * ps_phys_link_ptr;
  ps_phys_link_type               * companion_ps_phys_link_ptr;
  ps_flow_type                    * ps_flow_ptr;
  ps_flow_type                    * companion_ps_flow_ptr;
  ps_iface_type                   * rm_iface_ptr = NULL;
  ps_iface_type                   * um_iface_ptr = NULL;
  ps_iface_type                   * um_base_iface_ptr = NULL;
  ps_iface_type                   * companion_um_iface_ptr = NULL;
  ps_iface_type                   * logical_iface_ptr = NULL;
  dsm_item_type                   * rx_pkt;
  ps_dpm_qos_hdr_type               qos_hdr;
  ps_dpm_qos2_hdr_type              qos2_hdr;
  ps_dpm_ul_opt_path_type           path_type   = PS_DPM_UL_OPT_OPT_PATH;
  boolean                           ret_val     = FALSE;
  uint8                             pkt_buf[PS_OPT_PATH_IP_QOS2_HEADER_LEN + 1] ALIGN(4); //__attribute__ ((aligned(4)));
  uint8                             ip_hdr_len  = 0;
  uint8                             prot_offset = 0;
  uint16                            total_len   = 0;
  uint16                            version = 0;
  uint8                             ip_hdr_first_byte;  
  uint16                            buf_len = 0;
  boolean                           enable      = FALSE;
  sint15                            ps_errno;
  uint8                             num_pkts = 0;
  int                               frag_result = -1;
  int16                             ds_errno = DS_ENOERR;
  uint8                             qos_hdr_format;
  uint8                             qos_hdr_offset = 0;
  boolean                           is_pkt_trnsltd = FALSE;
  errno_enum_type                   err_no;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  dpm_rm_info_ptr = (ps_dpm_rm_info_type *) user_data;

  v4_dpm_um_info_ptr = dpm_rm_info_ptr->v4_um_info_ptr;
  v6_dpm_um_info_ptr = dpm_rm_info_ptr->v6_um_info_ptr;
  qos_hdr_format     = dpm_rm_info_ptr->qos_header_format;
  if (PS_DPM_QOS_FORMAT_8_BYTE == qos_hdr_format)
  {
    qos_hdr_offset = PS_DPM_OPT_QOS2_HEADER_OFFSET;
  }
  else
  {
    qos_hdr_offset = PS_DPM_OPT_QOS_HEADER_OFFSET;
  }
  /*-------------------------------------------------------------------------
    If rx_pkt is outstanding from previous time, use it. Otherwise, dequeue
    a new packet
  -------------------------------------------------------------------------*/
  if (NULL != dpm_rm_info_ptr->rx_pkt)
  {
    rx_pkt                  = dpm_rm_info_ptr->rx_pkt;
    dpm_rm_info_ptr->rx_pkt = NULL;

    LOG_MSG_DATA_PATH_INFO_1("ps_dpm_ul_opt_clat_ip_qos_rx_sig_hdlr(): "
                             "Using outstanding pkt, sig %d", rx_sig );
  }
  else
  {
    rx_pkt = dsm_dequeue(dpm_rm_info_ptr->sio_rx_wm_ptr);
    if (NULL == rx_pkt)
    {
      return TRUE;
    }
  }

  pkts_array[num_pkts] = rx_pkt;

  DS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  do
  {
  
    /*-----------------------------------------------------------------------
      Peek in to the first byte to determine version of the IP packet and
      process the packet appropriately
    -----------------------------------------------------------------------*/
    (void) dsm_extract(rx_pkt, qos_hdr_offset, &ip_hdr_first_byte, 1);

    version = ip_hdr_first_byte & PS_DPM_OPT_IP_VERSION_MASK;
    
    /*-----------------------------------------------------------------------
      Peek in to the first byte to determine version of the IP packet and
      process the packet appropriately
    -----------------------------------------------------------------------*/
    (void) dsm_extract(rx_pkt, 0, &ip_hdr_first_byte, 1);

    version = ip_hdr_first_byte & PS_DPM_OPT_IP_VERSION_MASK;

    /*-----------------------------------------------------------------------
      Do appropriate processing for IPv4 packets
    -----------------------------------------------------------------------*/
    if (version == PS_DPM_OPT_V4_VERSION) 
    {    
      /*-----------------------------------------------------------------------
        Do appropriate processing for IPv4 packets
      -----------------------------------------------------------------------*/
      if (NULL == v4_dpm_um_info_ptr)
      {
        LOG_MSG_ERROR_1("ps_dpm_ul_opt_clat_ip_qos_rx_sig_hdlr(): "
                        "v4 call not UP for sig %d", rx_sig);
        path_type = PS_DPM_UL_OPT_NO_PATH;                        
        break;
      }

      rm_iface_ptr = dpm_rm_info_ptr->v4_iface_ptr;  

      um_iface_ptr = dpm_rm_info_ptr->v4_logical_iface_ptr;     

      /*-----------------------------------------------------------------------
        For companion_um_iface_ptr we first get the CLAT iface & then get the 
        associated logical iface
      -----------------------------------------------------------------------*/
      companion_um_iface_ptr = (NULL == v6_dpm_um_info_ptr)
                                 ? NULL
                                 : dpm_rm_info_ptr->v6_logical_iface_ptr;
      if (PS_IFACE_IS_VALID(companion_um_iface_ptr))
      {
        companion_um_iface_ptr = 
                            PS_IFACEI_GET_ASSOC_IFACE(companion_um_iface_ptr);
      }
    }

    else if(version == PS_DPM_OPT_V6_VERSION)
    {
      /*-----------------------------------------------------------------------
        Do appropriate processing for IPv6 packets
      -----------------------------------------------------------------------*/
      if (NULL == v6_dpm_um_info_ptr)
      {
        LOG_MSG_ERROR_1("ps_dpm_ul_opt_clat_ip_qos_rx_sig_hdlr(): "
                        "v6 call not UP for sig %d", rx_sig);
        path_type = PS_DPM_UL_OPT_NO_PATH;                        
        break;
      }

      rm_iface_ptr = dpm_rm_info_ptr->v6_iface_ptr; 

      um_iface_ptr = dpm_rm_info_ptr->v6_logical_iface_ptr; 

      /*-----------------------------------------------------------------------
        For companion_um_iface_ptr we first get the CLAT iface & then get the 
        associated logical iface
      -----------------------------------------------------------------------*/
      companion_um_iface_ptr = (NULL == v4_dpm_um_info_ptr)
                                 ? NULL
                                 : dpm_rm_info_ptr->v4_logical_iface_ptr;
      if (PS_IFACE_IS_VALID(companion_um_iface_ptr))
      {
        companion_um_iface_ptr = 
                            PS_IFACEI_GET_ASSOC_IFACE(companion_um_iface_ptr);
      }
    }

    else 
    {
      LOG_MSG_ERROR_1("ps_dpm_ul_opt_clat_ip_qos_rx_sig_hdlr(): "
                      "Unknown IP type for sig %d", rx_sig);
      path_type = PS_DPM_UL_OPT_NO_PATH;
      break;
    }

    /*-------------------------------------------------------------------
      To handle packet being sent when call is being brought down
    -------------------------------------------------------------------*/
    if (NULL == um_iface_ptr)
    {
      LOG_MSG_ERROR_2("ps_dpm_ul_opt_clat_ip_qos_rx_sig_hdlr(): "
                      "Call in down or going down state um iface 0x%p "
                      "rm iface 0x%p ", um_iface_ptr, rm_iface_ptr);
      path_type = PS_DPM_UL_OPT_NO_PATH;
      break;
    }

    um_base_iface_ptr = PS_DPM_GET_BASE_IFACE(um_iface_ptr);

    /*-----------------------------------------------------------------------
      For checking flow control, logical iface need to be used. Hence get
      logical iface info from CLAT um iface

      Check if Um is flow controlled and if so, do not process the packet
      further. There are two scenarios to consider.
        1. Single IP call - Um is flow controlled if corresponding Um iface
           is flow controlled
        2. Dual-IP over single RmNet call (Single APN/Dual APN) - Um is
           flow controlled only if both the corresponding v4 and v6 Um ifaces
           are flow controlled

      Detailed algorithm is
        1. Check if Um is flow controlled. If it is not, then there is
           no flow control. If it is, go to step 2
        2. Check if companion Um iface is registered with DPM. If it is not,
           then Um is flow controlled. If it is not, go to step 3
        3. Check if companion Um is flow controlled. If it is not, then there is
           no flow control. If it is, Um is flow controlled
        4. If Um is flow controlled, cache the packet as outstanding packet
           in DPM and process it later
    -----------------------------------------------------------------------*/

    /*-----------------------------------------------------------------------
      Step 1
    -----------------------------------------------------------------------*/
    logical_iface_ptr = PS_IFACEI_GET_ASSOC_IFACE(um_iface_ptr);
	if(!PS_IFACE_IS_VALID(logical_iface_ptr))
	{
	  LOG_MSG_ERROR_1("ps_dpm_ul_opt_clat_ip_qos_rx_sig_hdlr():"
	  	              "failed to find associated um_iface_ptr 0x%p",
	  	              um_iface_ptr);
      path_type = PS_DPM_UL_OPT_NO_PATH;
	  break;
	}
    ps_flow_ptr      = PS_IFACEI_GET_DEFAULT_FLOW(logical_iface_ptr);
    ps_phys_link_ptr = PS_IFACEI_GET_PHYS_LINK(logical_iface_ptr);

    if (!(PS_IFACEI_FLOW_ENABLED(logical_iface_ptr)))
    {
      /*---------------------------------------------------------------------
        Step 2
      ---------------------------------------------------------------------*/
      if (NULL != companion_um_iface_ptr)
      {
        /*-------------------------------------------------------------------
          Step 3
        -------------------------------------------------------------------*/
        companion_ps_flow_ptr =
          PS_IFACEI_GET_DEFAULT_FLOW(companion_um_iface_ptr);
        companion_ps_phys_link_ptr =
          PS_IFACEI_GET_PHYS_LINK(companion_um_iface_ptr);

        if (!(PS_IFACEI_FLOW_ENABLED(companion_um_iface_ptr)))
        {
          /*-----------------------------------------------------------------
            Step 4
          -----------------------------------------------------------------*/
          ret_val                 = TRUE;
          path_type               = PS_DPM_UL_OPT_FLOW_CONTROL_PATH;
          dpm_rm_info_ptr->rx_pkt = rx_pkt;

          break;
        }
      }
      else
      {
        /*-------------------------------------------------------------------
          Step 4
        -------------------------------------------------------------------*/
        ret_val                 = TRUE;
        path_type               = PS_DPM_UL_OPT_FLOW_CONTROL_PATH;
        dpm_rm_info_ptr->rx_pkt = rx_pkt;

        break;
      }
    }

    /*-----------------------------------------------------------------------
      Pull out QoS hdr & assign the rx_pkt back since it might get modified
      by dsm_pullup

    -----------------------------------------------------------------------*/
    if (PS_DPM_QOS_FORMAT_8_BYTE == qos_hdr_format)
    {
      (void) dsm_pullup(&rx_pkt, &qos2_hdr, sizeof(qos2_hdr));
    }

    else
    {
      (void) dsm_pullup(&rx_pkt, &qos_hdr, sizeof(qos_hdr));
    }
    pkts_array[num_pkts] = rx_pkt;

    if (version == PS_DPM_OPT_V4_VERSION) 
    {
      if(um_base_iface_ptr->clat_pdn_cntxt == NULL)
      {
        LOG_MSG_ERROR_2("ps_dpm_ul_opt_clat_ip_no_qos_rx_sig_hdlr(): "
                        "clat pdn cntx NULL for iface 0x%x:%d", 
                         um_base_iface_ptr->name, um_base_iface_ptr->instance);
        path_type = PS_DPM_UL_OPT_NO_PATH;
        break;
      }
      
      /*---------------------------------------------------------------------
        Fetch IHL and use it to extract IP header and port information from
        the IP packet
      ---------------------------------------------------------------------*/
      ip_hdr_len = (ip_hdr_first_byte & PS_DPM_OPT_IP_HDR_LEN_MASK) << 2;

      if( (ip_hdr_len + PS_DPM_OPT_TCP_HDR_LEN_EXTRACT) > 
                            (PS_OPT_PATH_IP_QOS2_HEADER_LEN + 1) )
      {
        path_type = PS_DPM_UL_OPT_LEGACY_PATH;
        break;
      }

      buf_len = dsm_extract(rx_pkt,
                            PS_DPM_OPT_QOS_HEADER_OFFSET,
                            pkt_buf,
                            ip_hdr_len + PS_DPM_OPT_TCP_HDR_LEN_EXTRACT);

      if ((ip_hdr_len + PS_DPM_OPT_TCP_HDR_LEN_EXTRACT) > buf_len)
      {
        path_type = PS_DPM_UL_OPT_LEGACY_PATH;
        break;
      }    
            
      if (TRUE ==
            ps_dpm_ul_opt_ipv4_is_legacy(pkt_buf, 
                                         rm_iface_ptr, 
                                         ip_hdr_len))
      {
        path_type = PS_DPM_UL_OPT_LEGACY_PATH;
        break;
      }

      /*---------------------------------------------------------------------
              LOW LATENCY TRAFFIC handling
             ---------------------------------------------------------------------*/
      if( ds3g_get_config_latency_info())
      {
        ps_dpm_ul_v4_low_latency_traffic_handler ( pkt_buf,
                                                   ip_hdr_len,
                                                   prot_offset );
      }

      /*---------------------------------------------------------------------
        Get the packet's total length
      ---------------------------------------------------------------------*/
      total_len   =
           ps_htons(*((uint16 *)(pkt_buf + PS_DPM_OPT_V4_TOTAL_LEN_OFFSET)));
      
      /*-----------------------------------------------------------------------
        Increment packet stats on Rm iface
      -----------------------------------------------------------------------*/
      rm_iface_ptr->iface_i_stats.pkts_rx++;
      rm_iface_ptr->iface_i_stats.bytes_rx += total_len;

      /*-----------------------------------------------------------------------
        Log the packet on the Rm iface before it is fragmented/translated
      -----------------------------------------------------------------------*/
#ifdef PS_LOGGING_IOE_DPL
      DPL_LOG_NETWORK_RX_PACKET(rm_iface_ptr, rx_pkt, DPL_IID_NETPROT_IP);
#endif //PS_LOGGING_IOE_DPL
      
      if (total_len > PS_IFACE_GET_MTU(um_iface_ptr))
      {
        frag_result = ps_dpm_ul_opt_fragment_packet (
                                                 rx_pkt,
                                                 rm_iface_ptr,
                                                 PS_IFACE_GET_MTU(um_iface_ptr),
                                                 pkts_array,
                                                 version,
                                                 total_len,
                                                 &ds_errno);
        if (-1 == frag_result)
        {
          path_type = PS_DPM_UL_OPT_NO_PATH;
          break;
        }    
      }

      while ((num_pkts < PS_DPM_UL_OPT_MAX_FRAG_PACKETS) && 
              (NULL != pkts_array[num_pkts]))
      {
        /* Translate the v4 packet to v6 */
        if (-1 == ps_clat_ip_to_ip6(
                      &(pkts_array[num_pkts]),
                      rm_iface_ptr,
                      ((ps_dpm_clat_pdn_cntxt_info_type *)
                      (um_base_iface_ptr->clat_pdn_cntxt))->client_pdn_handle))
        {
          path_type = PS_DPM_UL_OPT_NO_PATH;

          /*-------------------------------------------------------------------
            To make sure we dont drop the pkt since protcols would have already
            dropped
          -------------------------------------------------------------------*/
          ds_errno = DS_EINVAL;
          break;
        }

        num_pkts++;
      }

      /*---------------------------------------------------------------------
        Reset num_pkts & rx_pkt which will be used when packets are sent out
      ---------------------------------------------------------------------*/	      
      num_pkts = 0;
      rx_pkt = pkts_array[num_pkts];

      is_pkt_trnsltd = TRUE;
    }
  } while(0);
  
  DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  
  
  /*-------------------------------------------------------------------
    Loop through all the pkts in array and send them one by one
  -------------------------------------------------------------------*/    
  while (num_pkts < PS_DPM_UL_OPT_MAX_FRAG_PACKETS &&
           NULL != pkts_array[num_pkts])
  {
    DS_ENTER_CRIT_SECTION(&global_ps_crit_section);
   
    /*-------------------------------------------------------------------------
      We need to point rx_pkt to point to right fragment when a pkt is 
      fragmented and then they need to be queued to legacy watermark 
    -------------------------------------------------------------------------*/
    rx_pkt = pkts_array[num_pkts]; 
       
    if (path_type != PS_DPM_UL_OPT_OPT_PATH)
    {
      goto send_data;
    }
    
    /*---------------------------------------------------------------------
      Extract IP header and port information from the IP packet
    ---------------------------------------------------------------------*/
    buf_len = dsm_extract(rx_pkt,
                          PS_DPM_OPT_QOS_HEADER_OFFSET,
                          pkt_buf,
                          PS_DPM_OPT_V6_HDR_LEN + PS_DPM_OPT_TCP_HDR_LEN_EXTRACT);

    if ((PS_DPM_OPT_V6_HDR_LEN + PS_DPM_OPT_TCP_HDR_LEN_EXTRACT) > buf_len)
    {
      path_type = PS_DPM_UL_OPT_LEGACY_PATH;
      goto send_data;
    }

    /*---------------------------------------------------------------------
      Get the offset of protocol field, IP header length, and also the
      packet's total length
    ---------------------------------------------------------------------*/
    total_len =
      ps_htons(*((uint16 *)(pkt_buf + PS_DPM_OPT_V6_TOTAL_LEN_OFFSET)));
    total_len  += PS_DPM_OPT_V6_HDR_LEN;
    ip_hdr_len  = PS_DPM_OPT_V6_HDR_LEN;
    prot_offset = PS_DPM_OPT_V6_NXT_HDR_OFFSET;

    /*-----------------------------------------------------------------------
      Check if packet needs to be sent up the legacy path
    -----------------------------------------------------------------------*/
    if (TRUE == ps_dpm_ul_opt_ipv6_is_legacy(pkt_buf))
    {
      path_type = PS_DPM_UL_OPT_LEGACY_PATH;
      goto send_data;
    }

    /*-----------------------------------------------------------------------
      Drop the packet if the packet's source prefix doesn't match Rm's prefix.
      Applicable only to regular v6 packets
    -----------------------------------------------------------------------*/
    if ((FALSE  == is_pkt_trnsltd) && 
         (FALSE == ps_dpm_ul_opt_ipv6_prefix_cmp(pkt_buf, rm_iface_ptr)))
    {
      LOG_MSG_ERROR_0("ps_dpm_ul_opt_ip_no_qos_rx_sig_hdlr(): "
                      "IPV6 prefix cmp failed");      
      path_type = PS_DPM_UL_OPT_NO_PATH;
      goto send_data;
    }

    if (FALSE == is_pkt_trnsltd)
    {
      /*-----------------------------------------------------------------------
        If the pkt got translated then it is already logged. Hence this is NA.
        Increment packet stats on Rm iface
      -----------------------------------------------------------------------*/
      rm_iface_ptr->iface_i_stats.pkts_rx++;
      rm_iface_ptr->iface_i_stats.bytes_rx += total_len;

      /*-----------------------------------------------------------------------
        Log the packet on the Rm iface
      -----------------------------------------------------------------------*/
#ifdef PS_LOGGING_IOE_DPL
      DPL_LOG_NETWORK_RX_PACKET(rm_iface_ptr, rx_pkt, DPL_IID_NETPROT_IP);
#endif //PS_LOGGING_IOE_DPL
    }

    PS_TX_META_INFO_DUP(tx_meta_info_ptr, &dup_tx_meta_info_ptr);

    if (PS_DPM_QOS_FORMAT_8_BYTE == qos_hdr_format)
    {
      ps_dpm_process_qos_hdr(rx_pkt,
                             um_base_iface_ptr,
                             IP_V6,
                             TRUE,
                             dup_tx_meta_info_ptr,
                             (void *)&qos2_hdr,
                             qos_hdr_format,
                             &ps_flow_ptr);
    }
    else
    {
      ps_dpm_process_qos_hdr(rx_pkt,
                             um_base_iface_ptr,
                             IP_V6,
                             TRUE,
                             dup_tx_meta_info_ptr,
                             (void*)&qos_hdr,
                             qos_hdr_format,
                             &ps_flow_ptr);
    }

    /*-----------------------------------------------------------------------
      Increment packet stats on Um iface
    -----------------------------------------------------------------------*/
    um_base_iface_ptr->iface_i_stats.pkts_tx++;
    um_base_iface_ptr->iface_i_stats.bytes_tx += total_len;

    /*-----------------------------------------------------------------------
      Log packet on the Um iface
    -----------------------------------------------------------------------*/
    PS_DPM_GLOBAL_STATS_INC(um_base_iface_ptr);
#ifdef PS_LOGGING_IOE_DPL
    DPL_LOG_NETWORK_TX_PACKET(um_base_iface_ptr, rx_pkt, DPL_IID_NETPROT_IP);
#endif //PS_LOGGING_IOE_DPL

    /*-----------------------------------------------------------------------
      If flow capability is set to PS_FLOW_CAPABILITY_DATA_DISALLOWED,
      drop the packet
    -----------------------------------------------------------------------*/
    if (PS_FLOW_GET_CAPABILITY(ps_flow_ptr, PS_FLOW_CAPABILITY_DATA_DISALLOWED))
    {
      LOG_MSG_INFO2_1("ps_dpm_ul_opt_clat_ip_qos_rx_sig_hdlr(): "
                      "ps_flow 0x%p is PS_FLOW_CAPABILITY_DATA_DISALLOWED",
                      ps_flow_ptr);
      path_type = PS_DPM_UL_OPT_NO_PATH;
      goto send_data;
    }

    /*---------------------------------------------------------------------
        TCP ACK prioritization
    ---------------------------------------------------------------------*/
    if (ps_dpm_ul_opt_tcp_ack_pri_check
          (
            pkt_buf,
            ip_hdr_len,
            total_len,
            prot_offset
          ) == TRUE
       )
    {
      LOG_MSG_INFO3_0("IP_NO_QOS: TCP_ACK_PRIORITY: Tagging DSM_HIGHEST");
      (rx_pkt)->priority = DSM_HIGHEST;
    }     


    /*-----------------------------------------------------------------------
      Log packet on ps_flow and increment phys link stats
    -----------------------------------------------------------------------*/
#ifdef PS_LOGGING_IOE_DPL
    DPL_LOG_FLOW_TX_PACKET(ps_flow_ptr, rx_pkt, DPL_IID_NETPROT_IP);
#endif //PS_LOGGING_IOE_DPL

    if (NULL != ps_phys_link_ptr)
    {
      ps_phys_link_ptr->phys_link_i_stats.pkts_tx++;
    }

    /*-----------------------------------------------------------------------
      NOTE: COULD HAVE THIS CODE OUTSIDE THE DO-WHILE LOOP, BUT DOING IT
      THIS WAY SAVES ONE IF CHECK
    -----------------------------------------------------------------------*/

    /*-----------------------------------------------------------------------
      Make sure that the power save mode is disabled after recieving the first 
      uplink packet on this path
    -----------------------------------------------------------------------*/
    if( TRUE == PS_IFACE_GET_POWERSAVE_FILTERING_MODE( rm_iface_ptr ) )
    {
      if( PS_IFACE_GET_POWERSAVE_FILTERING_AUTO_EXIT_MODE( rm_iface_ptr ) )
      {
        ps_iface_set_powersave_filtering_mode( rm_iface_ptr, enable, &ps_errno );
      }
    }
    PS_TX_META_RESET_FILTER_RESULT(dup_tx_meta_info_ptr, IP_FLTR_CLIENT_MO_EXCP );
    /*-------------------------------------------------------------------------
      Execute MO exceptional filters and update filter result in Tx meta info when packet matches
      with MO Exceptional filters
    -------------------------------------------------------------------------*/
    if( PS_IFACEI_NUM_FILTERS(um_base_iface_ptr, IP_FLTR_CLIENT_MO_EXCP) > 0)
    {
      LOG_MSG_DATA_PATH_INFO_0("ps_dpm_ul_opt_ip_no_qos_rx_sig_hdlr: Iface has "
                               "MO exceptional filters registered");
      if(ps_pkt_info_filter_tx_pkt(um_base_iface_ptr,
                                   IP_FLTR_CLIENT_MO_EXCP,
                                   &rx_pkt,
                                   &dup_tx_meta_info_ptr,
                                   &err_no) < 0)
      {
        LOG_MSG_DATA_PATH_INFO_1("ps_dpm_ul_opt_ip_no_qos_rx_sig_hdlr: "
                                 "ps_pkt_info_filter_tx_pkt returned ps_errono = %d",
                                 err_no);
      }
    }
send_data :

    /*-------------------------------------------------------------------------
      Leave CS before tx_cmd to avoid possible deadlock issues
    -------------------------------------------------------------------------*/
    DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

    /*-------------------------------------------------------------------------
      Process the packet based on the 'path_type'
    -------------------------------------------------------------------------*/
    switch (path_type)
    {
      case PS_DPM_UL_OPT_OPT_PATH:
      {
          um_base_iface_ptr->iface_private.tx_cmd
          (
            um_base_iface_ptr,
            &rx_pkt,
            dup_tx_meta_info_ptr,
            um_base_iface_ptr->iface_private.tx_cmd_info
          );
        break;
      }

      case PS_DPM_UL_OPT_LEGACY_PATH:
      {
        ret_val = ps_dpm_ul_legacy_input(rm_iface_ptr, rx_pkt, dpm_rm_info_ptr);
        break;
      }

      case PS_DPM_UL_OPT_FLOW_CONTROL_PATH:
      {
        LOG_MSG_INFO1_1("ps_dpm_ul_opt_clat_ip_qos_rx_sig_hdlr(): "
                        "Um is flow controlled, sig %d", rx_sig);
        break;
      }

      case PS_DPM_UL_OPT_NO_PATH:
      default:
      {
        /*---------------------------------------------------------------------
          Check if rx packet needs to be free'd or not
        ---------------------------------------------------------------------*/
        if (DS_EINVAL != ds_errno)
        {
          dsm_free_packet(&rx_pkt);
        }
        break;
      }
    }


  pkts_array[num_pkts] = NULL;
	num_pkts++;
  }

  return ret_val;
} /* ps_dpm_ul_opt_clat_ip_qos_rx_sig_hdlr() */

#endif /* FEATURE_DATA_QMI_QOS */
#endif

#ifdef FEATURE_DATA_PS_464XLAT
/*===========================================================================
FUNCTION rmnet_ul_opt_clat_ip_no_qos_rx_sig_hdlr

DESCRIPTION
  This function is the optimized Signal handler for IP NO QOS mode. It decides
  if the packet needs to take the legacy path or should be forwarded to Um
  iface.

PARAMETERS
 rx_sig    :  RmNet signal type
 user_data :  pointer to user data.

RETURN VALUE
 TRUE :
 FALSE:

DEPENDENCIES
  None

SIDE EFFECTS
  None.

===========================================================================*/
boolean rmnet_ul_opt_clat_ip_no_qos_rx_sig_hdlr
(
  ps_rm_sig_enum_type    rx_sig,
  void                 * user_data
)
{
  ps_tx_meta_info_type                   *dup_tx_meta_info_ptr = NULL;
  ps_tx_meta_info_type                   *mo_tx_meta_info_ptr = NULL;
  dsm_item_type                          *rx_pkt;
  rmnet_ul_opt_path_type                  path_type   = RMNET_UL_OPT_OPT_PATH;
  boolean                                 ret_val     = FALSE;
  uint8                                   pkt_buf[PS_OPT_PATH_IP_NO_QOS_HEADER_LEN + 1] ALIGN(4); //__attribute__ ((aligned(4)));
  uint8                                   ip_hdr_len  = 0;
  uint8                                   prot_offset = 0;
  uint16                                  total_len   = 0;
  uint16                                  version = 0;
  uint8                                   ip_hdr_first_byte;  
  uint16                                  buf_len = 0;
  uint8                                   num_pkts = 0;
  int                                     frag_result = -1;
  int16                                   ds_errno = DS_ENOERR;
  ip_pkt_info_type                       *pkt_info_ptr = NULL;
  errno_enum_type                         err_no;
  ds_fwk_clnt_info_s_type                 clnt_info_ptr ={0};
  ds_fwk_s_type                          *ds_fwk_inst;
  rmnet_smi_ip_info_type                 *info;
  rmnet_smi_info_type                    *temp_info;
  uint32                                  mtu; 
  ds_flow_type                           *ds_flow_ptr = NULL;
  ds_pdn_mgr_instance_type_e              pdn_instance_type = DS_PDN_INSTANCE_IPV4;
  dpl_net_cb                             *rm_dpl = NULL;
  ds_flow_type*                           flow_ptr = NULL;
  ip_version_enum_type                    ip_version;  
  ds_stats_type                          *rm_stats = NULL;
  uint8                                   protocol = 0;
  ds_stats_type                          *um_stats = NULL;
  void                                   *fraghdl = NULL;  
  boolean                                 pdn_sharing = FALSE;
  boolean                                 is_pkt_trnsltd = FALSE;  
  ps_rx_meta_info_type                   *mi_ref_ptr = NULL;  
  uint16                                  src_port = 0;
  uint16                                  dst_port = 0;
  errno_enum_type                         ps_errno = DS_ENOERR;
  uint8                                   buf[4] = {0};
  uint16                                  dsm_offset = 0;  
  dsm_item_header_priority_type           dsm_priority;
  dsm_watermark_type                     *rx_wm = NULL;
  ps_clat_sm_type                        *clat_sm_ptr = NULL;
  dsm_item_type                          *peek_rx_pkt;  
  boolean                                 is_mox_data_pkt = FALSE;  
  boolean                                 is_flow_enabled = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_ASSERT((int)user_data);

  info = (rmnet_smi_ip_info_type *)user_data;  
  rx_wm = &(info->xport_info.rx_wmk);

  if (rx_wm == NULL)
  {
    DS_MSG0(MSG_LEGACY_ERROR,"rmnet_ul_opt_ip_no_qos_rx_sig_hdlr, RM WM is NULL");
    return TRUE;
  }
  /*Peak into the first packet of the RM WM*/
  DSM_LOCK(&((rx_wm)->lock));
  peek_rx_pkt = (dsm_item_type*)q_check(rx_wm->q_ptr);
  DSM_UNLOCK(&((rx_wm)->lock));

  if (peek_rx_pkt != NULL)
  {
    dsm_priority = peek_rx_pkt->priority;
    if (dsm_priority == DSM_QUEUE_PRIORITY_HIGH)
    {
      is_mox_data_pkt = TRUE;
    }
    memcpy(&ip_hdr_first_byte, peek_rx_pkt->data_ptr,RMNET_UL_OPT_FIRST_BYTE);
    DS_MSG2(MSG_LEGACY_ERROR,"rmnet_ul_opt_ip_no_qos_rx_sig_hdlr,"
            " APP priority %d, is_mox_data_pkt %d", peek_rx_pkt->app_field,
            is_mox_data_pkt);
  }
  else
  {
    DS_MSG0(MSG_LEGACY_ERROR,"rmnet_ul_opt_ip_no_qos_rx_sig_hdlr,"
            " WM first item is NULL. No more pkts");
      return TRUE;
  }
 
  do
  {
    DS_ENTER_CRIT_SECTION(&global_ps_crit_section);
    /*-----------------------------------------------------------------------
      Peek in to the first byte to determine version of the IP packet and
      process the packet appropriately
    -----------------------------------------------------------------------*/
    version = ip_hdr_first_byte & RMNET_OPT_IP_VERSION_MASK;	
    /*-----------------------------------------------------------------------
      Do appropriate processing for IPv4 and IPv6 packets
    -----------------------------------------------------------------------*/
    if (version == RMNET_OPT_V4_VERSION) 
    {
      temp_info = info->info_v4;
  /*---------------------------------------------------------------------------
       Get the framework instance from the rmnet instance 
  ------------------------------------------------------------------------*/
      ds_fwk_get_clnt_info_from_handle(DS_FWK_CLIENT_RMNET,
                                       temp_info->constants.rmnet_inst,
                                       &clnt_info_ptr);
      if(clnt_info_ptr.fw_inst == NULL)
      {
        DS_MSG0(MSG_LEGACY_ERROR,"No framework instance could be found for this call");
        DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
        return ret_val;
      }
      ds_fwk_inst = clnt_info_ptr.fw_inst;
      if (RMNET_META_SM_NULL_STATE == temp_info->meta_sm.state)
      {
        DS_MSG1(MSG_LEGACY_ERROR,"rmnet_ul_opt_ip_no_qos_rx_sig_hdlr(): "
                "v4 call not UP for sig %d", rx_sig);        
        DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
        return ret_val;
      }
     /* To check more if call is UP or not*/
      pdn_instance_type = DS_PDN_INSTANCE_IPV4;
      ip_version = IP_V4;

      rm_dpl = ds_fwk_get_dpl_ptr(ds_fwk_inst,IP_V4,DS_DPL_CLIENT_RM);
      rm_stats = DS_FWK_GET_STATS_PTR(ds_fwk_inst, ip_version,DS_STATS_CLIENT_RM);
      
      um_stats = DS_FWK_GET_STATS_PTR(ds_fwk_inst, ip_version,DS_STATS_CLIENT_UM);
      DS_MSG1(MSG_LEGACY_ERROR,"v4  packet%d ip_hdr_len %d total len %d rm_dpl id %d",
              rm_dpl->dpl_id.ifname);
    }
    else if (version == RMNET_OPT_V6_VERSION)              /* IPv6 packet */
    {
      temp_info = info->info_v6;
       /*---------------------------------------------------------------------------
           Get the framework instance from the rmnet instance 
     ------------------------------------------------------------------------*/
      ds_fwk_get_clnt_info_from_handle(DS_FWK_CLIENT_RMNET,
                                       temp_info->constants.rmnet_inst,
                                       &clnt_info_ptr);
      if(clnt_info_ptr.fw_inst == NULL)
      {
        DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
        DS_MSG0(MSG_LEGACY_ERROR,"No framework instance could be found for this call");
        return ret_val;
      }
      if (RMNET_META_SM_NULL_STATE == temp_info->meta_sm.state)
      {      
        DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);       
        DS_MSG1(MSG_LEGACY_ERROR,"rmnet_ul_opt_ip_no_qos_rx_sig_hdlr(): "
                "v6 call not UP for sig %d", rx_sig);

        return ret_val;
      }      
      ds_fwk_inst = clnt_info_ptr.fw_inst;
      //To check more if call is UP or not
      pdn_instance_type = DS_PDN_INSTANCE_IPV6;          
      ip_version = IP_V6;      
      rm_dpl = ds_fwk_get_dpl_ptr(ds_fwk_inst,IP_V6,DS_DPL_CLIENT_RM);
      rm_stats = DS_FWK_GET_STATS_PTR(ds_fwk_inst, ip_version,DS_STATS_CLIENT_RM);
      um_stats = DS_FWK_GET_STATS_PTR(ds_fwk_inst, ip_version,DS_STATS_CLIENT_UM);
      DS_MSG1(MSG_LEGACY_ERROR,"v6  packet%d ip_hdr_len %d total len %d rm_dpl id %d",
              rm_dpl->dpl_id.ifname);
    }  
    else
    {
      DS_MSG1(MSG_LEGACY_ERROR,"rmnet_ul_opt_ip_no_qos_rx_sig_hdlr(): "
              "Unknown IP type for sig %d", rx_sig);
      return ret_val;
    }
    if (ds_fwk_inst_is_valid(ds_fwk_inst, IPV6_ADDR) == FALSE)
    {    
      DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      DS_MSG2(MSG_LEGACY_ERROR,"rmnet_ul_opt_clat_ip_no_qos_rx_sig_hdlr(): "
              "Call in down or going down state um iface 0x%p "
              "rm iface 0x%p ", ds_fwk_inst, ds_fwk_inst);
      return ret_val;
    }
    if (ds_fwk_get_default_bearer_flow_ex(ds_fwk_inst,IPV6_ADDR,
                                          &flow_ptr, is_mox_data_pkt))
    {
      is_flow_enabled = TRUE;
      rx_pkt = dsm_dequeue(&(info->xport_info.rx_wmk));
      if (NULL == rx_pkt)
      {
        return TRUE;
      }
      if(ds_pdn_mgr_is_wwan_tech_suspended() == TRUE)
      {
        wwan_tech_suspend_cntr++;
        if ((1 == wwan_tech_suspend_cntr)|| !(wwan_tech_suspend_cntr % WWAN_TECH_SUSP_MAX_DBG_RATE_LIMIT))
        {
          DS_MSG0(MSG_LEGACY_ERROR,"rmnet_ul_opt_clat_ip_no_qos_rx_sig_hdlr(): tech suspended freeing the pkt");
        }
        dsm_free_packet(&rx_pkt);
        DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
        return FALSE;
      }
      else
      {
        /* reset the cntr, when tech is not suspended */
        if (0 != wwan_tech_suspend_cntr)
        {
          /* Reset the cntr, when tech is not suspended */
          wwan_tech_suspend_cntr = 0;
        }
      }
      pkts_array[num_pkts] = rx_pkt;
    }
    else
    {
       DS_MSG3(MSG_LEGACY_ERROR,"rmnet_ul_opt_ip_no_qos_rx_sig_hdlr(): "
               "is_mox_data_pkt %d Flow is disabled  sig %d,fwk_inst 0x%x",
                is_mox_data_pkt, rx_sig, ds_fwk_inst);
       DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
       return TRUE;
    }
    /*-----------------------------------------------------------------------
      For checking flow control,framework instance need to be used.      
      First fetch default flow by passing MO flag as FALSE
      If flow is not enabled and flow is no NULL 
      Run MO filters to check if it is a mo packet ,
      if yes check the if flow is enabled on default flow for mo packets
      If flow is enabled go down and run the qos filters and get the dedicated flow 
      For CLAT pdn the v6 pdn will only be there		
      Again run the MO Checks
    -----------------------------------------------------------------------*/
    if( is_mox_data_pkt && is_flow_enabled)
    {
      if( flow_ptr != NULL )
      {
        /** MO Exception packets may need to be routed even when flow is disabled*/
        if(DS_FWK_NUM_FILTERS(ds_fwk_inst, FLTR_CLIENT_MO_EXCP) > 0)
        {
          DS_MSG0(MSG_LEGACY_HIGH,"rmnet_ul_opt_ip_no_qos_rx_sig_hdlr:"
                  "MO exceptional filters registered");
          PS_TX_META_INFO_AND_RT_META_INFO_GET(mo_tx_meta_info_ptr);
          if((mo_tx_meta_info_ptr != NULL )&&
             (mo_tx_meta_info_ptr->rt_meta_info_ptr != NULL))
          {
             (&(PS_TX_META_GET_PKT_INFO( mo_tx_meta_info_ptr)))->\
                    is_fraghdl_allocated = 2;//INVALID;
             if(ps_pkt_info_filter_tx_pkt(ds_fwk_inst,
                                   FLTR_CLIENT_MO_EXCP,
                                   &rx_pkt,
                                   &mo_tx_meta_info_ptr,
                                   &err_no) < 0)
             {
               DS_MSG1(MSG_LEGACY_HIGH,"rmnet_ul_opt_ip_no_qos_rx_sig_hdlr: "
                                 "ps_pkt_info_filter_tx_pkt returned ps_errono = %d",
                                 err_no);
               path_type = RMNET_UL_OPT_NO_PATH;
             }
             else
             {
               is_mox_data_pkt = (mo_tx_meta_info_ptr != NULL) &&
               (PS_TX_META_GET_FILTER_RESULT( mo_tx_meta_info_ptr, FLTR_CLIENT_MO_EXCP));
               if(!ds_fwk_is_flow_enabled_ex(flow_ptr,is_mox_data_pkt))
               {
                 ret_val    = TRUE;
                 path_type  = RMNET_UL_OPT_FLOW_CONTROL_PATH;
                 info->rx_pkt  = rx_pkt;
                 DS_MSG2(MSG_LEGACY_ERROR,"RMNET_UL_OPT_FLOW_CONTROL_PATH ip %d "
                         "fwk_inst %p ",clnt_info_ptr.ip_type,ds_fwk_inst);
               }
               else
               {
                 path_type = RMNET_UL_OPT_OPT_PATH;
                }
          }
          if((mo_tx_meta_info_ptr != NULL)
              &&(PS_TX_META_GET_RT_META_INFO_PTR(mo_tx_meta_info_ptr) != NULL)
              &&(NULL != (fraghdl = 
                (&(PS_TX_META_GET_PKT_INFO( mo_tx_meta_info_ptr)))->fraghdl)) &&
                ((&(PS_TX_META_GET_PKT_INFO( mo_tx_meta_info_ptr)))->\
                  is_fraghdl_allocated == 1 ))//allocated 
          {
            (&(PS_TX_META_GET_PKT_INFO( mo_tx_meta_info_ptr)))->\
               is_fraghdl_allocated = 0;
             ip_fraghdl_free(fraghdl);
             (&(PS_TX_META_GET_PKT_INFO( mo_tx_meta_info_ptr)))->fraghdl = NULL;
         }
       }
       PS_TX_META_INFO_FREE(&mo_tx_meta_info_ptr);
       if( path_type != RMNET_UL_OPT_OPT_PATH ) 
       {
          break;
       }
     }
     else
     {
       ret_val                 = TRUE;
       path_type = RMNET_UL_OPT_FLOW_CONTROL_PATH;
       info->rx_pkt	= rx_pkt;
       DS_MSG2(MSG_LEGACY_ERROR,"RMNET_UL_OPT_FLOW_CONTROL_PATH ip %d "
               "fwk_inst %p ",clnt_info_ptr.ip_type,ds_fwk_inst);
       break;
     }
   }
   else
   {
     path_type = RMNET_UL_OPT_NO_PATH;
     DS_FWK_INC_INSTANCE_STATS(rm_stats, pkts_dropped_rx, 1);
     DS_MSG2(MSG_LEGACY_ERROR,"No default bearer could be found ip %d "
             "fwk_inst %p ",clnt_info_ptr.ip_type,ds_fwk_inst);
     break;
   }
  }
  if(version == RMNET_OPT_V4_VERSION) 
  {
     if(ds_fwk_inst->clat_pdn_cntxt == NULL)
     {
        DS_MSG1(MSG_LEGACY_ERROR,"rmnet_ul_opt_clat_ip_no_qos_rx_sig_hdlr(): "
                "clat pdn cntx NULL for ds_fwk_inst 0x%x", 
                ds_fwk_inst);
        path_type = RMNET_UL_OPT_NO_PATH;
        break;
    }
    /* Peek into the IPv4 packet and fetch the Dst port and if it matches the 
       DNS server port then set the boolean accordingly. */
       dsm_offset = IPV4_HEADER_LENGTH_OFFSET;
       (void)dsm_extract(rx_pkt, dsm_offset, buf, 2*sizeof(uint16));
       src_port = *((uint16*)&buf[0]);
       dst_port = *((uint16*)&buf[2]);
       DS_MSG2(MSG_LEGACY_ERROR,"rmnet_ul_opt_clat_ip_no_qos_rx_sig_hdlr():"
               "src port = 0x%x, dst port = 0x%x,",src_port, dst_port);
       if(dst_port == PS_DNS_PROXY_SERVER_PORT)
       {
          clat_sm_ptr  = (ps_clat_sm_type*)((ds_fwk_clat_pdn_cntxt_info_type *)
                                   (ds_fwk_inst->clat_pdn_cntxt))->client_pdn_handle;
          DS_MSG0(MSG_LEGACY_HIGH,"UL DNS query with XLAT: set the flag ");
          clat_sm_ptr->is_dns_query = TRUE;                  
       }
      /*---------------------------------------------------------------------
        Fetch IHL and use it to extract IP header and port information from
        the IP packet
      ---------------------------------------------------------------------*/
      ip_hdr_len = (ip_hdr_first_byte & RMNET_OPT_IP_HDR_LEN_MASK) << 2;
      if( (ip_hdr_len + RMNET_OPT_TCP_HDR_LEN_EXTRACT) > 
                (PS_OPT_PATH_IP_NO_QOS_HEADER_LEN + 1) )
      {
        path_type = RMNET_UL_OPT_LEGACY_PATH;
        break;
      }
      buf_len = dsm_extract(rx_pkt,
                            0,
                            pkt_buf,
                            ip_hdr_len + RMNET_OPT_TCP_HDR_LEN_EXTRACT);
      if ((ip_hdr_len + RMNET_OPT_TCP_HDR_LEN_EXTRACT) > buf_len)
      {
        path_type = RMNET_UL_OPT_LEGACY_PATH;
        break;
      }
      if (TRUE ==
            rmnet_ul_opt_ipv4_is_legacy(pkt_buf, 
                                         ds_fwk_inst, 
                                         ip_hdr_len))
      {
        path_type = RMNET_UL_OPT_LEGACY_PATH;
        break;
      }
      //Check if PDN sharing is enabled 
      pdn_sharing = ds_fwk_is_pdn_shared(ds_fwk_inst ,RMNET_OPT_V4_VERSION);
      if(pdn_sharing == TRUE)
      {     
        //Check for ICMP for PDN sharing 
        protocol = *(pkt_buf + RMNET_OPT_V4_PROT_FIELD_OFFSET);
        if(protocol == PS_IPPROTO_ICMP)
        {
          if( ps_rx_pkt_info_generate(&rx_pkt, &mi_ref_ptr, &ps_errno, FALSE) == -1)
          {
            path_type = RMNET_UL_OPT_LEGACY_PATH;
            break;
          }
          pkt_info_ptr = &(mi_ref_ptr->pkt_info);
          if((pkt_info_ptr->ip_vsn == IP_V4 )&&
             (pkt_info_ptr->ip_hdr.v4.protocol == PS_IPPROTO_ICMP ) &&
             (pkt_info_ptr->ptcl_info.icmp.type == (uint8)ICMP_ECHO_REQ))
          {
            //store the ID and seqence number
            ds_fwk_set_rmnet_embedded_icmp_packet_info(pkt_info_ptr,
                                                  pkt_info_ptr->ip_vsn,
                                                  rx_pkt,
                                                  temp_info->constants.rmnet_inst);
          }
         }
       }
      /*---------------------------------------------------------------------
        Get the packet's total length
      ---------------------------------------------------------------------*/
      total_len = ps_htons(*((uint16 *)(pkt_buf + RMNET_OPT_V4_TOTAL_LEN_OFFSET)));
     /*-----------------------------------------------------------------------
      Increment packet stats on Rm side
      -----------------------------------------------------------------------*/
      DS_FWK_INC_INSTANCE_STATS(rm_stats, pkts_rx, 1);
      DS_FWK_INC_INSTANCE_STATS(rm_stats, bytes_rx, total_len);
      /*-----------------------------------------------------------------------
        Log the packet on the Rm iface before it is fragmented/translated
      -----------------------------------------------------------------------*/
#ifdef PS_LOGGING_IOE_DPL
      DPL_LOG_NETWORK_RX_PACKET(rm_dpl, rx_pkt, DPL_IID_NETPROT_IP);
#endif //PS_LOGGING_IOE_DPL
      mtu = ds_fwk_get_mtu(ds_fwk_inst,IPV4_ADDR);
      if (total_len > mtu)
      {
        frag_result = rmnet_ul_opt_fragment_packet (
                                           rx_pkt,
                                           ds_fwk_inst,
                                           mtu,
                                           pkts_array,
                                           version,
                                           total_len,
                                           &ds_errno);
        if (-1 == frag_result)
        {
          path_type = RMNET_UL_OPT_NO_PATH;
          break;
        }   
      }
      while ((num_pkts < RMNET_UL_OPT_MAX_FRAG_PACKETS) && 
             (NULL != pkts_array[num_pkts]))
      {
        /* Translate v4 packet to v6 */
        if (-1 == ps_clat_ip_to_ip6(&(pkts_array[num_pkts]),
                                    ds_fwk_inst,
                                    ((ds_fwk_clat_pdn_cntxt_info_type *)
                                    (ds_fwk_inst->clat_pdn_cntxt))->client_pdn_handle))
        {
          path_type = RMNET_UL_OPT_NO_PATH;
          /*-------------------------------------------------------------------
            To make sure we dont drop the pkt since protcols would have already
            dropped
          -------------------------------------------------------------------*/
          ds_errno = DS_EINVAL;
          break;
        }
        num_pkts++;
      }
      /*---------------------------------------------------------------------
        Reset num_pkts & rx_pkt which will be used when packets are sent out
      ---------------------------------------------------------------------*/	      
      num_pkts = 0;
      rx_pkt = pkts_array[num_pkts];
      is_pkt_trnsltd = TRUE;
    }
  } while(0);
  DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  /*-------------------------------------------------------------------
    Loop through all the pkts in array and send them one by one
  -------------------------------------------------------------------*/    
  while (num_pkts < RMNET_UL_OPT_MAX_FRAG_PACKETS && 
           NULL != pkts_array[num_pkts])
  {
    DS_ENTER_CRIT_SECTION(&global_ps_crit_section);
    mtu = ds_fwk_get_mtu(ds_fwk_inst,IPV6_ADDR);
    /*-------------------------------------------------------------------------
      We need to point rx_pkt to point to right fragment when a pkt is 
      fragmented and then they need to be queued to legacy watermark 
    -------------------------------------------------------------------------*/
    rx_pkt = pkts_array[num_pkts]; 
    if (path_type != RMNET_UL_OPT_OPT_PATH)
    {
      goto send_data;
    }
    
    /*---------------------------------------------------------------------
      Extract IP header and port information from the IP packet
    ---------------------------------------------------------------------*/
    buf_len = dsm_extract(rx_pkt,
                          0,
                          pkt_buf,
                          RMNET_OPT_V6_HDR_LEN + RMNET_OPT_TCP_HDR_LEN_EXTRACT);
    if ((RMNET_OPT_V6_HDR_LEN + RMNET_OPT_TCP_HDR_LEN_EXTRACT) > buf_len)
    {
      path_type = RMNET_UL_OPT_LEGACY_PATH;
      goto send_data;
    }
    /*---------------------------------------------------------------------
      Get the offset of protocol field, IP header length, and also the
      packet's total length
    ---------------------------------------------------------------------*/
    total_len =
      ps_htons(*((uint16 *)(pkt_buf + RMNET_OPT_V6_TOTAL_LEN_OFFSET)));
    total_len  += RMNET_OPT_V6_HDR_LEN;
    ip_hdr_len  = RMNET_OPT_V6_HDR_LEN;
    prot_offset = RMNET_OPT_V6_NXT_HDR_OFFSET;
    /*-----------------------------------------------------------------------
      Check if packet needs to be sent up the legacy path
    -----------------------------------------------------------------------*/
    if (TRUE == rmnet_ul_opt_ipv6_is_legacy(pkt_buf))
    {
      path_type = RMNET_UL_OPT_LEGACY_PATH;
      goto send_data;
    } 
    DS_MSG2(MSG_LEGACY_ERROR,"mtu %d total_len %d ",
            mtu, total_len);
    if (total_len > mtu)
    {    
      frag_result = rmnet_ul_opt_fragment_packet (
                                           rx_pkt,
                                           ds_fwk_inst,
                                           mtu,
                                           pkts_array,
                                           version,
                                           total_len,
                                           &ds_errno);
      if (-1 == frag_result)
      {
        DS_MSG1(MSG_LEGACY_ERROR,"rmnet_ul_opt_fragment_packet failed %d",mtu);
        DS_FWK_INC_INSTANCE_STATS(rm_stats, pkts_dropped_rx, 1);
        path_type = RMNET_UL_OPT_NO_PATH;
      }   
    }      
    /*-----------------------------------------------------------------------
      Drop the packet if the packet's source prefix doesn't match Rm's prefix.
      Applicable only to regular v6 packets
    -----------------------------------------------------------------------*/
    if ((FALSE  == is_pkt_trnsltd) && 
         (FALSE == rmnet_ul_opt_ipv6_prefix_cmp(pkt_buf, ds_fwk_inst)))
    {
      DS_MSG0(MSG_LEGACY_ERROR,"rmnet_ul_opt_clat_ip_no_qos_rx_sig_hdlr(): "
                      "IPV6 prefix cmp failed");      
      path_type = RMNET_UL_OPT_NO_PATH;
      goto send_data;
    } 
    if (FALSE == is_pkt_trnsltd)
    {
     //Check if PDN sharing is enabled 
     pdn_sharing = ds_fwk_is_pdn_shared(ds_fwk_inst ,RMNET_OPT_V6_VERSION);
     if(pdn_sharing == TRUE)
     {        
       //Check for ICMP protocol for PDN sharing 
       protocol = (uint8) (* (pkt_buf + RMNET_OPT_V6_NXT_HDR_OFFSET));	
       if(protocol == PS_IPPROTO_ICMP6)
       {
         if( ps_rx_pkt_info_generate(&rx_pkt, &mi_ref_ptr, &ps_errno, FALSE) == -1)
         {
           path_type = RMNET_UL_OPT_LEGACY_PATH;  
           break;
         }
         pkt_info_ptr = &(mi_ref_ptr->pkt_info);
         if((pkt_info_ptr->ip_vsn == IP_V6 )&&
            (pkt_info_ptr->ip_hdr.v6.hdr_type == PS_IPPROTO_ICMP6 ) &&
            (pkt_info_ptr->ptcl_info.icmp.type == ICMP6_ECHO_REQ))
         {
           //store the ID and seqence number
           ds_fwk_set_rmnet_embedded_icmp_packet_info(pkt_info_ptr,
                                                      pkt_info_ptr->ip_vsn,
                                                      rx_pkt,
                                                      temp_info->constants.rmnet_inst);
         }
       }
     }
    /*-----------------------------------------------------------------------
        Increment packet stats on Rm side
    -----------------------------------------------------------------------*/
     DS_FWK_INC_INSTANCE_STATS(rm_stats, pkts_rx, 1);
     DS_FWK_INC_INSTANCE_STATS(rm_stats, bytes_rx, total_len);
    /*-----------------------------------------------------------------------
        Log the packet on the Rm iface
    -----------------------------------------------------------------------*/
#ifdef PS_LOGGING_IOE_DPL
      DPL_LOG_NETWORK_RX_PACKET(rm_dpl, rx_pkt, DPL_IID_NETPROT_IP);
#endif //PS_LOGGING_IOE_DPL
    }
    /*-----------------------------------------------------------------------
      Increment packet stats on Um iface
    -----------------------------------------------------------------------*/	
    DS_FWK_INC_INSTANCE_STATS(um_stats, pkts_tx, 1);
    DS_FWK_INC_INSTANCE_STATS(um_stats, bytes_tx, total_len);
   /*-----------------------------------------------------------------------
    Log packet on the Um iface
    -----------------------------------------------------------------------*/
    DS_FWK_GLOBAL_STATS_INC;
    if(-1 != ps_tx_pkt_info_generate(&rx_pkt, &dup_tx_meta_info_ptr, &err_no))
    {
      pkt_info_ptr = &(PS_TX_META_GET_PKT_INFO(dup_tx_meta_info_ptr));
      pkt_info_ptr->if_ptr =  ds_fwk_inst;          
      if( !ds_fwk_get_data_flow_ex(ds_fwk_inst,
                                   IPV6_ADDR,
                                   pkt_info_ptr,
                                   &ds_flow_ptr,
                                   is_mox_data_pkt))
      {
        DS_MSG1(MSG_LEGACY_HIGH,"flow is disabled %d "
                "fwk_inst %p ",ds_fwk_inst);
        PS_TX_META_INFO_FREE(&dup_tx_meta_info_ptr);
        pkt_info_ptr = NULL;
        if( ds_flow_ptr != NULL )
        {
          ret_val = TRUE;
          path_type               = RMNET_UL_OPT_FLOW_CONTROL_PATH;
          info->rx_pkt            = rx_pkt;
          break;
        }
        else
        {
          path_type = RMNET_UL_OPT_NO_PATH; 
          DS_FWK_INC_INSTANCE_STATS(rm_stats, pkts_dropped_rx, 1);
          DS_MSG2(MSG_LEGACY_ERROR,"No default bearer could be found ip %d "
                  "fwk_inst %p ",clnt_info_ptr.ip_type,ds_fwk_inst);
          break;
        }
      }
      DS_MSG1(MSG_LEGACY_HIGH,"flow is enabled "
              "ds_flow_ptr %p ",ds_flow_ptr);
      PS_TX_META_INFO_FREE(&dup_tx_meta_info_ptr);
      pkt_info_ptr = NULL;
    }
    else
    {
      PS_TX_META_INFO_FREE(&dup_tx_meta_info_ptr);
      pkt_info_ptr = NULL;
      path_type = RMNET_UL_OPT_NO_PATH;
      DS_MSG2(MSG_LEGACY_ERROR,"No default bearer could be found ip %d "
              "fwk_inst %p ",clnt_info_ptr.ip_type,ds_fwk_inst);
      DS_FWK_INC_INSTANCE_STATS(rm_stats, pkts_dropped_rx, 1);
      break;
    }
    /*-----------------------------------------------------------------------
      NOTE: AFTER THIS POINT, THERE MUST NOT BE ANY ERROR AND PACKET MUST BE
      SENT TO Um IFACE VIA OPTIMIZED PATH, ELSE UNDUP THE META INFO
    -----------------------------------------------------------------------*/
    path_type = RMNET_UL_OPT_OPT_PATH;
    /*---------------------------------------------------------------------
      TCP ACK prioritization
    ---------------------------------------------------------------------*/
    if (rmnet_ul_opt_tcp_ack_pri_check
          (
             pkt_buf,
             ip_hdr_len,
             total_len,
             prot_offset
          ) == TRUE
       )
    {
      DS_MSG0(MSG_LEGACY_MED,"IP_NO_QOS: TCP_ACK_PRIORITY: Tagging DSM_HIGHEST");
      (rx_pkt)->priority = DSM_HIGHEST;
    }
  /*---------------------------------------------------------------------
    TCP packet to dst_port 21 prioritization
  ---------------------------------------------------------------------*/
  if (rmnet_ul_opt_ftp_ctrl_port_pri_check
       (
         pkt_buf,
         ip_hdr_len,
         prot_offset
       ) == TRUE
     )
  {
    (rx_pkt)->priority = DSM_HIGHEST;
  }
  PS_TX_META_INFO_DUP(tx_meta_info_ptr, &dup_tx_meta_info_ptr);
  /*-----------------------------------------------------------------------
     Update meta info with routing cache and QoS filter result
  -----------------------------------------------------------------------*/
  PS_TX_META_SET_ROUTING_CACHE(dup_tx_meta_info_ptr, ds_fwk_inst);
  PS_TX_META_SET_FILTER_RESULT(dup_tx_meta_info_ptr,
                               FLTR_CLIENT_QOS_OUTPUT,
                               (uint32)ds_flow_ptr);
  dup_tx_meta_info_ptr->rt_meta_info_ptr->pkt_info.ip_vsn = ip_version;   
  PS_TX_META_RESET_FILTER_RESULT(dup_tx_meta_info_ptr, FLTR_CLIENT_MO_EXCP );
  /*-------------------------------------------------------------------------
    Execute MO exceptional filters and update filter result in Tx meta info when packet matches
    with MO Exceptional filters. QoS and Legacy path filter would run in ps_iface_tx_cmd
    -------------------------------------------------------------------------*/

  if(DS_FWK_NUM_FILTERS(ds_fwk_inst, FLTR_CLIENT_MO_EXCP) > 0)
  {
    /*Invalid Packet info so that Protocol generate Packet info */	  
    PS_RT_META_INVALIDATE_PKT_INFO(PS_TX_META_GET_RT_META_INFO_PTR(dup_tx_meta_info_ptr)) ;
    DS_MSG0(MSG_LEGACY_HIGH,"rmnet_ul_opt_ip_no_qos_rx_sig_hdlr: Iface has "
            "MO exceptional filters registered");
    if(ps_pkt_info_filter_tx_pkt(ds_fwk_inst,
                                 FLTR_CLIENT_MO_EXCP,
                                 &rx_pkt,
                                 &dup_tx_meta_info_ptr,
                                 &err_no) < 0)
    {
       DS_MSG1(MSG_LEGACY_HIGH,"rmnet_ul_opt_ip_no_qos_rx_sig_hdlr: "
               "ps_pkt_info_filter_tx_pkt returned ps_errono = %d",
                err_no);
    }
  } 
  send_data :
    /*-------------------------------------------------------------------------
      Leave CS before tx_cmd to avoid possible deadlock issues
    -------------------------------------------------------------------------*/
  DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  /*-------------------------------------------------------------------------
    Process the packet based on the 'path_type'
  -------------------------------------------------------------------------*/
  switch (path_type)
  {
    case RMNET_UL_OPT_OPT_PATH:
    {
       /*-----------------------------------------------------------------------
        Log the packet on the Rm iface
      -----------------------------------------------------------------------*/
       DS_MSG1(MSG_LEGACY_HIGH,"rmnet_ul_opt_clat_ip_no_qos_rx_sig_hdlr: "
               "optimised path logging packet on the RX on Rm dplid%d ",
                rm_dpl->dpl_id.ifname);
#ifdef PS_LOGGING_IOE_DPL    
       DPL_LOG_NETWORK_RX_PACKET(rm_dpl, rx_pkt, DPL_IID_NETPROT_IP);
#endif //PS_LOGGING_IOE_DPL
       DS_FWK_GLOBAL_STATS_INC;
       ds_fwk_default_tx_cmd(ds_fwk_inst,
                             &rx_pkt,
                              dup_tx_meta_info_ptr
                            );
       break;
    }
    case RMNET_UL_OPT_LEGACY_PATH:
    {
      if(rx_pkt !=NULL)
      {
      /*-----------------------------------------------------------------------
        Log the packet on the Rm iface
     -----------------------------------------------------------------------*/
       DS_MSG1(MSG_LEGACY_HIGH,"rmnet_ul_opt_clat_ip_no_qos_rx_sig_hdlr: "
               "legacy path logging packet on the RX on Rm dplid %d ",
                rm_dpl->dpl_id.ifname);
#ifdef PS_LOGGING_IOE_DPL    
           DPL_LOG_NETWORK_RX_PACKET(rm_dpl, rx_pkt, DPL_IID_NETPROT_IP);
#endif //PS_LOGGING_IOE_DPL
       ret_val = rmnet_data_ul_legacy_input(temp_info, rx_pkt, ds_fwk_inst);
      }
      break;
    }
    case RMNET_UL_OPT_FLOW_CONTROL_PATH:
    {
      DS_MSG1(MSG_LEGACY_ERROR,"rmnet_ul_opt_clat_ip_no_qos_rx_sig_hdlr(): "
              "Um is flow controlled, sig %d", rx_sig);
      break;
    }
    case RMNET_UL_OPT_NO_PATH:
      DS_MSG1(MSG_LEGACY_ERROR,"rmnet_ul_opt_clat_ip_no_qos_rx_sig_hdlr(): "
             "No path is set so dropping packet %d",ds_errno);
    default:
    {
      /*---------------------------------------------------------------------
        Check if rx packet needs to be free'd or not
      ---------------------------------------------------------------------*/
      if (DS_EINVAL != ds_errno)
      {
        DS_MSG0(MSG_LEGACY_ERROR,"rmnet_ul_opt_clat_ip_no_qos_rx_sig_hdlr(): "
                "No path is set so dropping packet ds_errno is!= DS_EINVAL");
        dsm_free_packet(&rx_pkt);
      }
      break;
    }
  }
  pkts_array[num_pkts] = NULL;
  num_pkts++;
  }
  return ret_val;
} /* ps_dpm_ul_opt_clat_ip_no_qos_rx_sig_hdlr() */

#endif /* FEATURE_DATA_PS_464XLAT */
