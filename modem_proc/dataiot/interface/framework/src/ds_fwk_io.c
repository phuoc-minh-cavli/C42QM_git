/*=========================================================================*/
/*!
  @file
  ds_fwk_io.c

  @brief
  This file abstracts the data path functionality of the DS FWK

  @see
  ds_fwk.c

  Copyright (c) 2018 - 2021  Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*/
/*=========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //components/rel/dataiot.mpss/2.0/interface/framework/src/ds_fwk_io.c#19 $
  $DateTime: 2023/06/05 01:26:50 $$Author: pwbldsvc $

  when            who what, where, why
  ---------- --- ------------------------------------------------------------
07/02/2019   ds     Added Fwk instance validation check before calling API to 
                    match IID of received packet against the modem IID.
05/02/2019   ds     Added logic to identify whether packet is destined to
                    modem or APPs while genenating pkt info.
04/25/2019   svaka  Fixed issue V4 DL ping request
01/16/2019   ds     Added support for RFC2460 Regulator Mandatory Requirement
05/10/2018   svaka  Created module.

============================================================*/

/*============================================================

                     INCLUDE FILES FOR MODULE

============================================================*/
#include "ds_defs.h"
#include "ds_crit_sect.h"
#include "rdevmap.h"
#include "ps_pkt_info_utils.h"
#include "ps_ip4.h"
#include "ps_metai_info.h"
#include "ps_utils.h"
#include "ps_ip6.h"
#include "ps_logging.h"
#include "data_msg.h"
#include "ds_fwk_opt_defs.h"
#ifdef FEATURE_DATA_PS_464XLAT
#include "ps_clat.h"
#include "ps_clat_sm.h"
#endif
#include "ds_ipfltr.h"
#include "ds_fwk.h"
#include "ds_pdn_manager.h"
#include "ds_pdn_defs.h"
#include "ds_flow.h"
#include "ds_Utils_DebugMsg.h"
#include "ps_ip_fraghdl.h"
#include "ds_rmnet_meta_sm.h"
#include "ds_fwki.h"
#include "ps_icmp6.h"
#include "ps_logging_diag.h"
#include "ds_Utils_DebugMsg.h"


static int ds_fwk_frag_tx_cmd 
(
  void                 * device_ptr,
  dsm_item_type       ** dsm_item_ptr,
  ps_tx_meta_info_type * meta_info_ptr
);

/*============================================================

                             INTERNAL FUNCTIONS

============================================================*/

/*============================================================
FUNCTION DS_FWKI_INPUT_COMMON_PROC()

DESCRIPTION
   Interface client gives the protocol stack a packet to transmit.

PARAMETERS
  ds_fwk_inst_ptr:  framework instance.
  pkt_ref_ptr:       ref to dsm item ptr received
  rx_meta_info_ptr:  ptr to RX meta meta information
  tx_meta_info_ptr:  ptr to TX meta meta information

RETURN VALUE
  0: data was transfered
 -1: data was not transferred, but memory was freed

DEPENDENCIES
  None

SIDE EFFECTS
  None
============================================================*/
int ds_fwki_input_common_proc
(
  ds_fwk_s_type*                   ds_fwk_inst_ptr,
  dsm_item_type                  ** pkt_ref_ptr,
  ps_rx_meta_info_type            * rx_meta_info_ptr,
  ps_tx_meta_info_type            * tx_meta_info_ptr,
  ds_fwk_clnt_handle_info_s_type  * clnt_handle_info,
  boolean                           direction
)
{
  ps_tx_meta_info_type        * tmp_meta_info_ptr = NULL;
  ps_rx_meta_info_type        * tmp_rx_meta_info_ptr = NULL;
  void                        * fraghdl = NULL;
  dsm_item_type               * frag_ptr;
  int                           retval = 0;
  errno_enum_type               ps_errno;
  ip_pkt_info_type            * meta_pkt_info_ptr = NULL;
  boolean                       pkt_info_gen = FALSE;
  uint64                        packet_len_in_bytes = 0;
  q_type                        temp_queue;
  ps_icmp6_msg_type             icmp6;
  ds_stats_type                *rx_stats = NULL;
  uint8                         version;
  uint8                         ip_hdr_first_byte;
  ip_pkt_info_type             *pkt_info_ptr = NULL;
  boolean                       pdn_sharing = FALSE;
  dsm_item_type                *pkt_ptr = NULL;
  uint16                        offset = 0;
  ip_addr_enum_type             ip_type = IPV4_ADDR;  
  ip_pkt_info_type             *pkt_info = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( pkt_ref_ptr == NULL || *pkt_ref_ptr == NULL )
  {
    //IFACE_INC_INSTANCE_STATS(this_iface_ptr, pkts_dropped_tx, 1);
    DS_MSG0(MSG_LEGACY_ERROR,"ds_fwki_input_common_proc(): "
                    "Packet ptr is NULL!");
    PS_RX_META_INFO_FREE(&rx_meta_info_ptr);
    PS_TX_META_INFO_FREE(&tx_meta_info_ptr);
    DS_ASSERT(0);
    return -1;
  }
  (void) dsm_extract(*pkt_ref_ptr, 0, &ip_hdr_first_byte, 1);
  version = ip_hdr_first_byte & DS_FWK_OPT_IP_VERSION_MASK;

  if((direction == TRUE)&&(tx_meta_info_ptr)&&
      (tx_meta_info_ptr->rt_meta_info_ptr)&&
      (tx_meta_info_ptr->rt_meta_info_ptr->is_rmnet_packet == TRUE))
  {
    //Check if the packet came from rmnet client 
    if(version == DS_FWK_OPT_V6_VERSION)
      rx_stats = DS_FWK_GET_STATS_PTR(ds_fwk_inst_ptr,IP_V6,DS_STATS_CLIENT_RM);
    else if(version == DS_FWK_OPT_V4_VERSION)
      rx_stats = DS_FWK_GET_STATS_PTR(ds_fwk_inst_ptr,IP_V4,DS_STATS_CLIENT_RM);
  }
  else if(direction == FALSE)
  {
   //Packet      is a downlink for Modem stack
    if(version == DS_FWK_OPT_V6_VERSION)
      rx_stats = DS_FWK_GET_STATS_PTR(ds_fwk_inst_ptr,IP_V6,DS_STATS_CLIENT_UM);
    else if(version == DS_FWK_OPT_V4_VERSION)
      rx_stats = DS_FWK_GET_STATS_PTR(ds_fwk_inst_ptr,IP_V4,DS_STATS_CLIENT_UM);
  }
  /*-------------------------------------------------------------------------
    Increment the receive packet count
  -------------------------------------------------------------------------*/
  DS_FWK_INC_INSTANCE_STATS(rx_stats, pkts_rx, 1);
  packet_len_in_bytes = dsm_length_packet(*pkt_ref_ptr);
  DS_FWK_INC_INSTANCE_STATS(rx_stats, bytes_rx, packet_len_in_bytes);
  DS_FWK_GLOBAL_STATS_INC;

  //Check if PDN sharing is enabled 
   pdn_sharing = ds_fwk_is_pdn_shared(ds_fwk_inst_ptr, version);
   DS_MSG5(MSG_LEGACY_HIGH,"ds_fwki_input_common_proc(): "
         "pdn_sharing %d direction %d pkt_ref_ptr %x, packet_len_in_bytes %d ds_fwk_inst_ptr %x"
           ,pdn_sharing ,direction,*pkt_ref_ptr,packet_len_in_bytes, ds_fwk_inst_ptr);
   if((pdn_sharing == TRUE)&&(direction == FALSE))
   {
      return ds_fwki_input_common_proc_ex(ds_fwk_inst_ptr ,
                                          pkt_ref_ptr ,
                                          rx_meta_info_ptr,
                                          tx_meta_info_ptr);
   }
  /*-------------------------------------------------------------------------
    Check if the packet gets picked up by a sockets filter, pass it up the
    stack then.
  -------------------------------------------------------------------------*/
  DS_MSG3(MSG_LEGACY_HIGH,"Packet direction %d sock filters %d"
                  "rm client handle %d",direction,
                   DS_FWK_NUM_FILTERS(ds_fwk_inst_ptr, FLTR_CLIENT_SOCKETS),
                   clnt_handle_info->clnt_handle);
  if( (DS_FWK_NUM_FILTERS(ds_fwk_inst_ptr,
                              FLTR_CLIENT_SOCKETS) > 0 ))
  {
    if( ps_pkt_info_filter_rx_pkt(ds_fwk_inst_ptr,
                                 FLTR_CLIENT_SOCKETS,
                                 pkt_ref_ptr,
                                 &rx_meta_info_ptr,
                                 &ps_errno,
                                 FALSE) < 0 )
    {
      meta_pkt_info_ptr = &(PS_RX_META_GET_PKT_INFO(rx_meta_info_ptr));
      switch(ps_errno)
      {
        /*-------------------------------------------------------------------
          Insufficient information to filter.
        -------------------------------------------------------------------*/
        case E_WOULD_BLOCK:
          /*--------------------------------------------------------------------
            IP fragment scenario! Set the iface in the corresponding Frag handle
            and free the Meta Info.
          --------------------------------------------------------------------*/
          //TODO: Frag scenario
          ps_ip_fraghdl_set_bridge_device(meta_pkt_info_ptr->fraghdl,
                                          (void *)ds_fwk_inst_ptr,
                                          IP_FRAGHDL_BRIDGE_TYPE_IFACE_BRIDGE,
                                          ds_fwk_frag_tx_cmd);
          pkt_info = &(PS_RX_META_GET_PKT_INFO(rx_meta_info_ptr));
          if(pkt_info != NULL)
          {
            pkt_info->fraghdl = NULL;
          }
          PS_RX_META_INFO_FREE(&rx_meta_info_ptr);
          if (NULL != tx_meta_info_ptr)
          {
            pkt_info = &(PS_TX_META_GET_PKT_INFO(tx_meta_info_ptr));
            if(pkt_info != NULL)
            {
              pkt_info->fraghdl = NULL;
            }
          }
          PS_TX_META_INFO_FREE(&tx_meta_info_ptr);
          return 0;

        case E_BAD_DATA:
        {
 #ifdef FEATURE_DATA_PS_IPV6
          DS_MSG4( MSG_LEGACY_HIGH, 
                   "ds_fwki_input_common_proc: E_BAD_DATA, ip %d, "
                   "type %d, code %d, offset 0x%X",
                    meta_pkt_info_ptr->ip_vsn,
                    meta_pkt_info_ptr->ptcl_info.icmp.type,
                    meta_pkt_info_ptr->ptcl_info.icmp.code,
                    meta_pkt_info_ptr->ptcl_info.icmp.prob_offset);
          if(meta_pkt_info_ptr->ip_vsn == IP_V6)
          {
            if(0 != meta_pkt_info_ptr->ptcl_info.icmp.type)
            {
              /* Check wehther the packet is destined for AP or Modem 
                 Compare the IID's and send the packet to AP if none of IID's
                 of modem are not matching. */
              if( ds_fwk_inst_is_valid_ex(ds_fwk_inst_ptr, IPV6_ADDR) && 
                  !ps_pkt_info_v6_iid_match(ds_fwk_inst_ptr, pkt_ref_ptr) )
              {
                DS_MSG0(MSG_LEGACY_HIGH,"ds_fwki_input_common_proc(): "
                                         "Packet not matched to local IID");
                break;
              }
              memset(&icmp6, 0, sizeof(icmp6));
              meta_pkt_info_ptr->if_ptr = ds_fwk_inst_ptr;
              icmp6.cmn_hdr.type = (uint8)meta_pkt_info_ptr->ptcl_info.icmp.type;
              icmp6.cmn_hdr.code = (uint8)meta_pkt_info_ptr->ptcl_info.icmp.code;
              icmp6.msg_type.param_prob_offset = meta_pkt_info_ptr->ptcl_info.icmp.prob_offset;
              icmpv6_error_send(pkt_ref_ptr, &icmp6, meta_pkt_info_ptr);
              PS_TX_META_INFO_FREE(&tx_meta_info_ptr);
              PS_RX_META_INFO_FREE(&rx_meta_info_ptr);
              return -1;
            }
          }
#endif
        }

        case E_INVALID_ARG:
        case E_VERSION_MISMATCH:
        case E_NO_MEMORY:
        default:
        /*-------------------------------------------------------------------
            Insufficient information to filter, packet consumed, return.
            - Currently do not allow such packets, Need to DO research more, what
              kind we SHOULD forward!!
        -------------------------------------------------------------------*/
          DS_MSG1(MSG_LEGACY_ERROR,"ds_fwki_input_common_proc(): "
                          "Dropping packet, reason: %d ", ps_errno );
          DS_FWK_INC_INSTANCE_STATS(rx_stats, pkts_dropped_rx, 1);
          PS_RX_META_INFO_FREE(&rx_meta_info_ptr);
          PS_TX_META_INFO_FREE(&tx_meta_info_ptr);
          dsm_free_packet( pkt_ref_ptr );
          return -1;
      }
    }
  }

  /*Special Condition to take care of MT V4 pings incase of ATCoP client.
    Modem Stack should consume the ping request and generate the response,
    all the other packets other than ICMP requests are routed to respective 
    clients.*/
  DS_MSG2(MSG_LEGACY_HIGH," clnt_handle_info->clnt_type %d handle %d",
                            clnt_handle_info->clnt_type,
                            clnt_handle_info->clnt_handle);
  if( ( direction != TRUE) && 
      ( clnt_handle_info->clnt_handle != DS_FWK_INVALID_CLNT_HANDLE )&&
      ( clnt_handle_info->clnt_type == DS_FWK_CLIENT_ATCOP ) )
  {
    /*Check whether the packet is V4 ping request or not*/
    if( rx_meta_info_ptr != NULL )
    {
      pkt_info_ptr = &(rx_meta_info_ptr->pkt_info);
      if(( pkt_info_ptr->ip_vsn == IP_V4 )&&
         ( pkt_info_ptr->ip_hdr.v4.protocol == PS_IPPROTO_ICMP ) &&
         ( pkt_info_ptr->ptcl_info.icmp.type == (uint8)ICMP_ECHO_REQ))
      {
        clnt_handle_info->clnt_handle = DS_FWK_INVALID_CLNT_HANDLE;
      }

      if(version == DS_FWK_OPT_V4_VERSION)
      {
        ip_type = IPV4_ADDR;
      }
      else if (version == DS_FWK_OPT_V6_VERSION)
      {
        ip_type = IPV6_ADDR;
      }
      if ((ds_fwk_is_modem_embedded_call_active(ds_fwk_inst_ptr, ip_type)) &&
           (ds_fwk_get_tx_cb(DS_FWK_CLIENT_ATCOP)== NULL))
      {
        clnt_handle_info->clnt_handle = DS_FWK_INVALID_CLNT_HANDLE;

        DS_MSG2(MSG_LEGACY_HIGH,"clnt_handle_info->clnt_type %d handle %d",
                            clnt_handle_info->clnt_type,
                            clnt_handle_info->clnt_handle);
      }
    }
  }

  /*-------------------------------------------------------------------------
    Pass the packet up to the routing layer for full blown routing 
  -------------------------------------------------------------------------*/
  if( (direction != TRUE) && ( clnt_handle_info->clnt_handle 
                                == DS_FWK_INVALID_CLNT_HANDLE) )
  {
    /*-----------------------------------------------------------------------
      Clear any TX metainfo, if allocated
    -----------------------------------------------------------------------*/
    if (NULL != tx_meta_info_ptr)
    {
      pkt_info = &(PS_TX_META_GET_PKT_INFO(tx_meta_info_ptr));
      if(pkt_info != NULL)
      {
        pkt_info->fraghdl = NULL;
      }
    }
    PS_TX_META_INFO_FREE(&tx_meta_info_ptr);
    /*-----------------------------------------------------------------------
      Send pending fragments too, if there are any. Empty frag queue into 
      local temp queue first to avoid looping if fragments get queued again 
      for local reassembly (ip6_reassemble.)
    -----------------------------------------------------------------------*/
    if (rx_meta_info_ptr != NULL &&
         NULL != (fraghdl = 
                 (&(PS_RX_META_GET_PKT_INFO( rx_meta_info_ptr)))->fraghdl))
    {
      memset( &temp_queue, 0, sizeof(q_type));
      (void) q_init( &temp_queue );
      while( NULL != (frag_ptr = ip_fraghdl_get_fragment(fraghdl )) )
      {
        q_put( &temp_queue, &frag_ptr->link ); 
      }

      while( NULL != (frag_ptr = q_get( &temp_queue )))
      {
        PS_RX_META_INFO_DUP(rx_meta_info_ptr, &tmp_rx_meta_info_ptr);
        if( 0 != ip_any_input(ds_fwk_inst_ptr,
                              frag_ptr,
                              NULL,
                              FALSE,
                              tmp_rx_meta_info_ptr,
                              FALSE,
                              &pkt_ptr,
                              &offset))
        {
          DS_MSG1(MSG_LEGACY_ERROR,
                  "Failed to transmit data on instance %d:%d",
                   ds_fwk_inst_ptr->ds_fwk_index);
        }
      }
      q_destroy( &temp_queue );
    }

    /*-----------------------------------------------------------------------
      Send this pkt and return
    -----------------------------------------------------------------------*/
    DS_MSG1(MSG_LEGACY_HIGH,"sending packet to Proto stack 0x%p",ds_fwk_inst_ptr);
    return ip_any_input(ds_fwk_inst_ptr,
                        *pkt_ref_ptr,
                        NULL,
                        FALSE,
                        rx_meta_info_ptr,
                        FALSE,
                        &pkt_ptr,
                        &offset);
  }

  if( rx_meta_info_ptr != NULL )
  {
    if( PS_RX_META_GET_FILTER_RESULT(rx_meta_info_ptr, FLTR_CLIENT_SOCKETS) !=
          DS_IPFLTR_NOMATCH )
    {
      DS_MSG2(MSG_LEGACY_HIGH,"sending packet to Proto stack 0x%p direction %d",
                              ds_fwk_inst_ptr,direction);
      /*-----------------------------------------------------------------------
          Pkt belongs to local stack. (Clear any TX metainfo, if allocated) 
           -----------------------------------------------------------------------*/
      if( direction == TRUE)
      {
        rx_meta_info_ptr->pkt_info.pkt_group = DS_RM_GROUP;
      }
      PS_TX_META_INFO_FREE(&tx_meta_info_ptr);
      /*-----------------------------------------------------------------------
         Send pending fragments too, if there are any. Empty frag queue into 
         local temp queue first to avoid looping if fragments get queued again 
         for local reassembly (ip6_reassemble.)
         -----------------------------------------------------------------------*/
     if (rx_meta_info_ptr != NULL &&
          NULL != (fraghdl = 
                  (&(PS_RX_META_GET_PKT_INFO( rx_meta_info_ptr)))->fraghdl))
     {
       memset( &temp_queue, 0, sizeof(q_type));
       (void) q_init( &temp_queue );
       while( NULL != (frag_ptr = ip_fraghdl_get_fragment(fraghdl )) )
       {
         q_put( &temp_queue, &frag_ptr->link ); 
       }

       while( NULL != (frag_ptr = q_get( &temp_queue )))
       {
         PS_RX_META_INFO_DUP(rx_meta_info_ptr, &tmp_rx_meta_info_ptr);
         if( 0 != ip_any_input(ds_fwk_inst_ptr,
                               frag_ptr,
                               NULL,
                               FALSE,
                               tmp_rx_meta_info_ptr,
                               FALSE,
                               &pkt_ptr,
                               &offset))
         {
           DS_MSG1(MSG_LEGACY_ERROR,"Failed to transmit data on inst 0x%x:%d",
                           ds_fwk_inst_ptr->ds_fwk_index);
         }
       }
       q_destroy( &temp_queue );
     }
      /*-----------------------------------------------------------------------
        Send this pkt and return
      -----------------------------------------------------------------------*/
     return ip_any_input( ds_fwk_inst_ptr,
                          *pkt_ref_ptr,
                           NULL,
                           FALSE,
                           rx_meta_info_ptr,
                           FALSE,
                           &pkt_ptr,
                           &offset);
    }
  }

  /*-------------------------------------------------------------------------
    Copy fields from incoming RX meta_info_ptr into TX meta info and
    clear RX meta info, the created TX meta info used in further bridging
    scenarios!
  -------------------------------------------------------------------------*/
  LOG_MSG_DATA_PATH_INFO_2("rx_meta_info_ptr 0x%p tx_meta_info_ptr 0x%p",
        rx_meta_info_ptr,tx_meta_info_ptr );
  if( rx_meta_info_ptr != NULL )
  {
    /* Generate TX meta info (if not created already). */
    if( tx_meta_info_ptr == NULL )
    {
      PS_TX_META_INFO_AND_RT_META_INFO_GET(tx_meta_info_ptr);
      if( (NULL == tx_meta_info_ptr) ||
          (NULL == PS_TX_META_GET_RT_META_INFO_PTR(tx_meta_info_ptr)) )
      {
        DS_FWK_INC_INSTANCE_STATS(rx_stats, pkts_dropped_rx, 1);
        DS_MSG0(MSG_LEGACY_ERROR,"ds_fwki_input_common_proc(): "
                        "Out of metainfo items");
        PS_RX_META_INFO_FREE(&rx_meta_info_ptr);
        PS_TX_META_INFO_FREE(&tx_meta_info_ptr);
        dsm_free_packet(pkt_ref_ptr);
        return -1;
      }
    }
    if(rx_meta_info_ptr != NULL)
    {
      PS_TX_META_INFO_COPY_FROM_RX_META_INFO(rx_meta_info_ptr,
                                           tx_meta_info_ptr);
    }
    PS_RX_META_INFO_FREE(&rx_meta_info_ptr);
     /*----------------------------------------------------------------------
       We have copied packet info to TX path. We don't need to generate again
       for broadcast cases
     ----------------------------------------------------------------------*/
    pkt_info_gen = TRUE;
  } 
    /*-------------------------------------------------------------------------
    If we make it here, the pkt needs to be forwarded to the bridged iface.
    If fraghdl is non-NULL, get fragments.
  -------------------------------------------------------------------------*/
  if (tx_meta_info_ptr != NULL &&
      (PS_TX_META_GET_RT_META_INFO_PTR(tx_meta_info_ptr) != NULL) &&
      NULL != (fraghdl = (&(PS_TX_META_GET_PKT_INFO( tx_meta_info_ptr)))->fraghdl) &&
      ( TRUE != ps_ip_fraghdl_is_local(fraghdl) ))
  {
    while( NULL != (frag_ptr = ip_fraghdl_get_fragment(fraghdl )) && 
           TRUE != ps_ip_fraghdl_is_local(fraghdl))
    {
      PS_TX_META_INFO_DUP(tx_meta_info_ptr, &tmp_meta_info_ptr);
      DS_MSG0(MSG_LEGACY_HIGH,"ds_fwki_input_common_proc,sending frags to tx ");
      tx_meta_info_ptr->rt_meta_info_ptr->pkt_info.direction = direction;
      if( -1 == ds_fwk_tx_cmd(  ds_fwk_inst_ptr,
                                &frag_ptr,
                                tmp_meta_info_ptr))
      {
        retval = -1;
        /* TX function frees the Metainfo */
      }
    }
  }
  /*-------------------------------------------------------------------------
    If we make it here, the pkt needs to be forwarded to the bridged iface.
  -------------------------------------------------------------------------*/
  /*Set the tx_meta info is_local flag to FALSE , so that it will transmitted to rmnet/rmsm clients*/
  /*Call the sio_transmit directly if it is RMNET client, need to decide on rmsm.*/
  LOG_MSG_DATA_PATH_INFO_2("sending packet to fw_inst 0x%p,direction %d",
                             ds_fwk_inst_ptr,direction);
  if((tx_meta_info_ptr !=NULL) &&(tx_meta_info_ptr->rt_meta_info_ptr != NULL))
  {
    tx_meta_info_ptr->rt_meta_info_ptr->pkt_info.direction = direction;
  }
  
  if( -1 == ds_fwk_tx_cmd(  ds_fwk_inst_ptr,
                            pkt_ref_ptr,
                            tx_meta_info_ptr) )
  {
    retval = -1;
  }

  return retval;

} /* ds_fwki_input_common_proc */

/*============================================================
FUNCTION DS_FWK_DEFAULT_TX_CMD()

DESCRIPTION
  This function is the default TX function.  
  It frees the data passed in.

PARAMETERS
  fwk_inst_ptr:      instance on which it is being called
  pkt_ref_ptr:       ref to dsm item ptr for Tx - freed
  meta_info_ptr:     pointer to meta info type - also freed.

RETURN VALUE
  -1: indicating error

DEPENDENCIES
  Should not be called for logical ifaces

SIDE EFFECTS
  None
============================================================*/
int ds_fwk_default_tx_cmd
(
  ds_fwk_s_type          * ds_fwk_inst_ptr,
  dsm_item_type         ** pkt_ref_ptr,
  ps_tx_meta_info_type   * meta_info_ptr
)
{
  ds_flow_type         *ds_flow_ptr = NULL;  
  dpl_net_cb           *um_dpl      = NULL;
  ds_bearer_mgr_s      *bearer_ptr  = NULL; 
  ds_stats_type        *um_stats    = NULL;
  ip_version_enum_type  ip_vsn;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( meta_info_ptr != NULL )
  {
    ds_flow_ptr   = (ds_flow_type *)PS_TX_META_GET_FILTER_RESULT(meta_info_ptr,
                                                 FLTR_CLIENT_QOS_OUTPUT);
  }
  else
  {
    DS_MSG0(MSG_LEGACY_ERROR," dropping packet invalid bearer");
    dsm_free_packet(pkt_ref_ptr);
    return 0;
  } 
  ip_vsn  = meta_info_ptr->rt_meta_info_ptr->pkt_info.ip_vsn ;
  if( ds_flow_ptr == NULL )
  {
    bearer_ptr = ds_fwk_get_default_bearer(ds_fwk_inst_ptr,
       (ip_addr_enum_type)meta_info_ptr->rt_meta_info_ptr->pkt_info.ip_vsn);
  }
  else 
  {
    bearer_ptr = ds_flow_ptr->bearer_ptr;
  }
  
  DS_MSG2(MSG_LEGACY_HIGH," Sending packet to bearer 0x%x meta_info_ptr 0x%x", 
                    bearer_ptr,meta_info_ptr);

  um_stats = DS_FWK_GET_STATS_PTR(ds_fwk_inst_ptr,ip_vsn,DS_STATS_CLIENT_UM);

  if( bearer_ptr != NULL )
  {
    /*-----------------------------------------------------------------------
      Log packet on the Um iface
    -----------------------------------------------------------------------*/

    if(ip_vsn != NON_IP)
    {
     #ifdef PS_LOGGING_IOE_DPL
       um_dpl = ds_fwk_get_dpl_ptr(ds_fwk_inst_ptr,ip_vsn,DS_DPL_CLIENT_UM);
      if(um_dpl !=NULL)
      {
        DPL_LOG_NETWORK_TX_PACKET(um_dpl, *pkt_ref_ptr, DPL_IID_NETPROT_IP);
      }
      #endif //PS_LOGGING_IOE_DPL
    }
    bearer_ptr->tx_ptr(bearer_ptr,pkt_ref_ptr,meta_info_ptr);
    PS_TX_META_INFO_FREE(&meta_info_ptr);
    return 0;
  }
  else
  {
    DS_MSG0(MSG_LEGACY_ERROR," dropping packet invalid bearer");
    DS_FWK_INC_INSTANCE_STATS(um_stats, pkts_dropped_tx, 1);
    dsm_free_packet(pkt_ref_ptr);
    PS_TX_META_INFO_FREE(&meta_info_ptr);
    return -1;
  }
} /* ds_fwk_default_tx_cmd() */

/*============================================================
FUNCTION DS_FWK_TX_CMD()

DESCRIPTION
  Sends the packet to respective bearer by invoking bearer TX command.
  bearer id would be extracted from meta_info_ptr.
  
PARAMETERS
  dsm_item_ptr:     ref to dsm item ptr received
  meta_info_ptr:     ptr to meta info (TX meta information)

RETURN VALUE
   0: data was transfered
 -1: data was not transferred, but memory was freed

DEPENDENCIES
  None

SIDE EFFECTS
  None
============================================================*/
int ds_fwk_tx_cmd
(
  ds_fwk_s_type         * ds_fwk_inst_ptr,
  dsm_item_type        ** pkt_ref_ptr,
  ps_tx_meta_info_type  * meta_info_ptr
)
{
  boolean                        is_transmit = FALSE;
  errno_enum_type                ps_errno;
  int                            retval = 0;
  ds_flow_type                 * ds_flow_ptr = NULL;
  ip_pkt_info_type             * meta_pkt_info_ptr = NULL;
  uint64                         packet_len_in_bytes = 0;
  boolean                        direction = FALSE;
  ds_fwk_client_type             clnt_type = DS_FWK_CLIENT_INVALID;
  ds_fwk_clnt_handle_type        clnt_handle = DS_FWK_INVALID_CLNT_HANDLE;
  uint8                          version;
  uint8                          ip_hdr_first_byte;  
  dpl_net_cb                    *rm_dpl =NULL;
  ds_stats_type                 *rm_stats =NULL;
  ds_stats_type                 *um_stats = NULL;
  ip_addr_enum_type              ip_type = IPV4_ADDR;  
  boolean                        pdn_sharing = FALSE;  
  ps_rx_meta_info_type          *mi_ref_ptr = NULL;  
  uint32                         filter_result = DS_IPFLTR_NOMATCH;       
  ds_fwk_clnt_handle_type        rmnet_client_handle = DS_FWK_INVALID_CLNT_HANDLE;    
  ds_fwk_clnt_handle_type        rmsm_client_handle = DS_FWK_INVALID_CLNT_HANDLE;  
  void                          *fraghdl = NULL;
  boolean                        skip_filter = FALSE;  
  ip_addr_enum_type              ip_ver = IPV4_ADDR;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( ds_fwk_inst_ptr == NULL )
  {
    DS_ASSERT(0);
    dsm_free_packet(pkt_ref_ptr);
    PS_TX_META_INFO_FREE(&meta_info_ptr);
    return -1;
  }

  packet_len_in_bytes = dsm_length_packet(*pkt_ref_ptr);
  (void) dsm_extract(*pkt_ref_ptr, 0, &ip_hdr_first_byte, 1);
  version = ip_hdr_first_byte & DS_FWK_OPT_IP_VERSION_MASK;
  
  if(version == DS_FWK_OPT_V6_VERSION)
  {
    rm_stats = DS_FWK_GET_STATS_PTR(ds_fwk_inst_ptr,IP_V6,DS_STATS_CLIENT_RM);
    um_stats = DS_FWK_GET_STATS_PTR(ds_fwk_inst_ptr,IP_V6,DS_STATS_CLIENT_UM);
    ip_ver = IPV6_ADDR;
  }
  else if(version == DS_FWK_OPT_V4_VERSION)
  {
    rm_stats = DS_FWK_GET_STATS_PTR(ds_fwk_inst_ptr,IP_V4,DS_STATS_CLIENT_RM);
    um_stats = DS_FWK_GET_STATS_PTR(ds_fwk_inst_ptr,IP_V4,DS_STATS_CLIENT_UM);
    ip_ver = IPV4_ADDR;
  }
  if(ds_fwk_inst_is_valid_ex(ds_fwk_inst_ptr, ip_ver) == FALSE)
  {
    dsm_free_packet( pkt_ref_ptr );
    PS_TX_META_INFO_FREE( &meta_info_ptr );
    return -1;
  }
  
  if( meta_info_ptr != NULL && meta_info_ptr->rt_meta_info_ptr != NULL)
  {
    direction = meta_info_ptr->rt_meta_info_ptr->pkt_info.direction;
  }
  else
  {
    DS_MSG2(MSG_LEGACY_ERROR,"ds_fwk_tx_cmd: Invalid meta info:"
                    " Dropping packet 0x%p,0x%p",
    meta_info_ptr,(meta_info_ptr ==NULL)?NULL:meta_info_ptr->rt_meta_info_ptr);
    dsm_free_packet( pkt_ref_ptr );
    PS_TX_META_INFO_FREE( &meta_info_ptr );
    return -1;
  }
  
  DS_MSG1(MSG_LEGACY_HIGH,"ds_fwk_tx_cmd direction %d",direction);
  if( direction == TRUE )
  {
    ds_flow_ptr   = (ds_flow_type *)PS_TX_META_GET_FILTER_RESULT(meta_info_ptr,
                                                FLTR_CLIENT_QOS_OUTPUT);
    DS_MSG1(MSG_LEGACY_HIGH,"flow ptr recieved in meta info %p",ds_flow_ptr);
    if( ds_flow_ptr == NULL )
    {
      if( -1 == ps_tx_pkt_info_generate(pkt_ref_ptr, &meta_info_ptr, &ps_errno) )
      {
        PS_TX_META_INFO_FREE( &meta_info_ptr );
        DS_FWK_INC_INSTANCE_STATS(um_stats, pkts_dropped_tx, 1);
        dsm_free_packet( pkt_ref_ptr );
        return -1;
      }
      meta_pkt_info_ptr = &(PS_TX_META_GET_PKT_INFO(meta_info_ptr));
      meta_pkt_info_ptr->if_ptr =  ds_fwk_inst_ptr;			
      if( !ds_fwk_get_data_flow( ds_fwk_inst_ptr,
                                 (ip_addr_enum_type)meta_pkt_info_ptr->ip_vsn,
                                  meta_pkt_info_ptr,
                                  &ds_flow_ptr))
      {
        is_transmit = FALSE;
      }
      else
      {
        is_transmit = TRUE;
        PS_TX_META_SET_FILTER_RESULT(meta_info_ptr,
                                 FLTR_CLIENT_QOS_OUTPUT,
                                 (uint32)ds_flow_ptr);
      }
      DS_MSG2(MSG_LEGACY_HIGH,"is_transmit = %d ds_flow_ptr %p ",
                               is_transmit,ds_flow_ptr);
    }
    else
    {
      is_transmit = TRUE;
    }
   /*-------------------------------------------------------------------------
    Execute MO exceptional filters and update filter result in Tx meta info when packet matches
    with MO Exceptional filters
  -------------------------------------------------------------------------*/
    if( DS_FWK_NUM_FILTERS(ds_fwk_inst_ptr, FLTR_CLIENT_MO_EXCP) > 0 )
    {
      if(ps_pkt_info_filter_tx_pkt(ds_fwk_inst_ptr,
                                 FLTR_CLIENT_MO_EXCP,
                                 pkt_ref_ptr,
                                 &meta_info_ptr,
                                 &ps_errno) < 0)
      {
        DS_MSG1(MSG_LEGACY_ERROR,"ds_fwk_tx_cmd: "
                "ps_pkt_info_filter_tx_pkt returned ps_errono = %d",ps_errno);
      }
    }
  }
  /*-------------------------------------------------------------------------
    Transmit the packet if transmit flag is TRUE
  -------------------------------------------------------------------------*/
  if ( TRUE == is_transmit || direction != TRUE )
  {
    /* Check for the TCP_ACK_PRIORITY NV item */
    if( ps_pkt_info_get_tcp_ack_prio_enabled() )
    {
#define PKT_SIZE 80
      if (packet_len_in_bytes <= PKT_SIZE)
      {
        if (TCP_ACK_REG == ps_pkt_info_check_tcp_ack(*pkt_ref_ptr,
                                                     &ps_errno))
        {
          (*pkt_ref_ptr)->priority = DSM_HIGHEST;
        }
      }
    }
    if( direction == TRUE)
    {
    
#ifdef FEATURE_DATA_PS_464XLAT
       if((ds_fwk_inst_ptr->is_clat_capable == TRUE )&&(ds_fwk_inst_ptr->clat_pdn_cntxt != NULL))
       {
         if(version == DS_FWK_OPT_V4_VERSION)
         {
           if (-1 == ps_clat_ip_to_ip6(pkt_ref_ptr,
                                        ds_fwk_inst_ptr,
                                        ((ds_fwk_clat_pdn_cntxt_info_type *)
                                        (ds_fwk_inst_ptr->clat_pdn_cntxt))->client_pdn_handle))
           {
             dsm_free_packet( pkt_ref_ptr );
             PS_TX_META_INFO_FREE( &meta_info_ptr );
             return -1;
           }
         }
        }	   
        packet_len_in_bytes = dsm_length_packet(*pkt_ref_ptr);
#endif
        DS_FWK_INC_INSTANCE_STATS(um_stats, pkts_tx, 1);
        DS_FWK_INC_INSTANCE_STATS(um_stats, bytes_tx, packet_len_in_bytes);
        if( -1 == ds_fwk_default_tx_cmd(ds_fwk_inst_ptr,
                       pkt_ref_ptr,
                       meta_info_ptr) )
        {
          retval = -1;
        }
    }
    else
    {
      if(version == DS_FWK_OPT_V4_VERSION)
      {
        ip_type = IPV4_ADDR;
      }
      else if (version == DS_FWK_OPT_V6_VERSION)
      {
        ip_type = IPV6_ADDR;
      }
      if (version == DS_FWK_OPT_V4_VERSION)  /* IPv4 packet */
      {
        /*get Client V4 info*/
        ds_fwk_get_client_info(ds_fwk_inst_ptr,IPV4_ADDR,
                               &clnt_type,&clnt_handle);
        pdn_sharing = ds_fwk_is_pdn_shared(ds_fwk_inst_ptr ,DS_FWK_OPT_V4_VERSION);
        if(pdn_sharing == TRUE)
        {
           rmsm_client_handle = ds_fwk_inst_ptr->client_info[DS_FWK_CLIENT_RMSM].v4_client_handle;
           rmnet_client_handle = ds_fwk_inst_ptr->client_info[DS_FWK_CLIENT_RMNET].v4_client_handle;
        }
      }
      else if(version == DS_FWK_OPT_V6_VERSION)
      {
        ds_fwk_get_client_info(ds_fwk_inst_ptr,IPV6_ADDR,
                               &clnt_type,&clnt_handle);
        pdn_sharing = ds_fwk_is_pdn_shared(ds_fwk_inst_ptr ,DS_FWK_OPT_V6_VERSION);	  
        if(pdn_sharing == TRUE)
        {
          rmsm_client_handle = ds_fwk_inst_ptr->client_info[DS_FWK_CLIENT_RMSM].v6_client_handle;
          rmnet_client_handle = ds_fwk_inst_ptr->client_info[DS_FWK_CLIENT_RMNET].v6_client_handle;
        }
      }
      /*If PDN sharing is TRUE then run the filters
        to check which is the end client */
      if(pdn_sharing == TRUE)            
      {
          /*
          Fetch the pkt info from the below api 
          This is needed for executing the filters
        */
        if(meta_info_ptr != NULL)
        {
          //Check if pkt info is valid and is a fragmented packet.
          if (meta_info_ptr != NULL &&
               NULL != (fraghdl = 
              (&(PS_TX_META_GET_PKT_INFO( meta_info_ptr)))->fraghdl))
          {
            //Check if the pkt info is already allocated if yes 
            //then dont run filters again            
            if(meta_info_ptr->rt_meta_info_ptr->pkt_info.is_pkt_info_valid == TRUE)
            {
                skip_filter = TRUE;
            }
          }
        }        
        if(skip_filter == FALSE)
        {
          if(DS_FWK_NUM_FILTERS(ds_fwk_inst_ptr,
              FLTR_CLIENT_PDN_SHARING) > 0 )
          {
            if((ps_rx_pkt_info_generate(pkt_ref_ptr, &mi_ref_ptr, &ps_errno, FALSE)) ==0)
            {         
              filter_result = ds_fwk_execute_pdn_sharing_filters
                             (ds_fwk_inst_ptr, &(PS_RX_META_GET_PKT_INFO(mi_ref_ptr)));		  
              DS_MSG2(MSG_LEGACY_HIGH,"fltr result %d fwk_inst %p",filter_result,ds_fwk_inst_ptr);
              if(filter_result == DS_IPFLTR_NOMATCH)
              {          
                clnt_type  = DS_FWK_CLIENT_RMSM;
                clnt_handle = rmsm_client_handle;
              }
              else 
              {
                clnt_type  = DS_FWK_CLIENT_RMNET;
                clnt_handle = rmnet_client_handle;
              }
            }
            else
            {
              clnt_type = DS_FWK_CLIENT_RMSM;
              clnt_handle = rmsm_client_handle;
            }
            PS_RX_META_INFO_FREE(&mi_ref_ptr);
          }
          else
          {
            clnt_type = DS_FWK_CLIENT_RMSM;
            clnt_handle = rmsm_client_handle;
          } 
       }
       else
       {
         clnt_type = DS_FWK_CLIENT_RMNET;
         clnt_handle = rmnet_client_handle;
       }
     }   
     if(clnt_handle != DS_FWK_INVALID_CLNT_HANDLE && 
        clnt_type == DS_FWK_CLIENT_RMNET)
     {
        rm_dpl = ds_fwk_get_dpl_ptr(ds_fwk_inst_ptr,
                         (ip_version_enum_type)ip_type,DS_DPL_CLIENT_RM);
        DS_FWK_INC_INSTANCE_STATS(rm_stats,pkts_tx,1);
        DS_FWK_INC_INSTANCE_STATS(rm_stats,bytes_tx,packet_len_in_bytes);
#ifdef PS_LOGGING_IOE_DPL
        if(rm_dpl != NULL)
        {
          DPL_LOG_NETWORK_TX_PACKET(rm_dpl, *pkt_ref_ptr, DPL_IID_NETPROT_IP);
        }
#endif //PS_LOGGING_IOE_DPL
      }
      ds_fwk_invoke_tx_cb(clnt_type, clnt_handle, *pkt_ref_ptr);
      PS_TX_META_INFO_FREE(&meta_info_ptr);
     }
    }
  /*-------------------------------------------------------------------------
    Otherwise Free the packet.
    If the instance state is invalid, the packet shouldn't have reached here.
  -------------------------------------------------------------------------*/
  else
  {
    DS_FWK_INC_INSTANCE_STATS(um_stats, pkts_dropped_tx, 1);   
    PS_META_INFO_FREE( &meta_info_ptr );
    dsm_free_packet( pkt_ref_ptr );
    retval = -1;	
    DS_MSG2(MSG_LEGACY_ERROR,"packet drop tx:%d dir:%d",is_transmit,direction);
  } 
  return retval;

} /* ds_fwk_tx_cmd() */

/*===========================================================
FUNCTION DS_FWK_INPUT()

DESCRIPTION
   client gives packet to transmit over the network.

PARAMETERS
  pkt_ref_ptr:       ref to dsm item ptr received
  meta_info_ptr:     ptr to meta info (TX/RX meta information)

RETURN VALUE
  0: data was transfered
 -1: data was not transferred, but memory was freed

DEPENDENCIES
  None

SIDE EFFECTS
  None
============================================================*/
int ds_fwk_input
(
  ds_fwk_s_type*          ds_fwk_inst_ptr,
  dsm_item_type        ** pkt_ref_ptr,
  ps_meta_info_type_ex  * meta_info_ex_ptr
)
{
  ps_tx_meta_info_type           * tx_meta_info_ptr = NULL;
  ps_rx_meta_info_type           * rx_meta_info_ptr = NULL;
  ds_fwk_clnt_handle_info_s_type   clnt_handle_info;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy TX / RX meta information from incoming meta_info_ex_ptr,  */
  if(meta_info_ex_ptr != NULL)
  {
    tx_meta_info_ptr = meta_info_ex_ptr->tx_meta_info_ptr;
    rx_meta_info_ptr = meta_info_ex_ptr->rx_meta_info_ptr;
    /* Clear meta_info_ex_ptr */
    PS_META_INFO_FREE_EX(&meta_info_ex_ptr);
  }
  
  DS_MSG1(MSG_LEGACY_HIGH,"Sending Packet fw_inst 0x%p",ds_fwk_inst_ptr);
  clnt_handle_info.clnt_handle = DS_FWK_INVALID_CLNT_HANDLE;
  return ds_fwki_input_common_proc(ds_fwk_inst_ptr,
                                   pkt_ref_ptr,
                                   rx_meta_info_ptr,
                                   tx_meta_info_ptr,
                                  &clnt_handle_info,
                                   TRUE);
}

/*============================================================
FUNCTION DS_FWK_INPUT_LEGACY()

DESCRIPTION
   Interface client gives the protocol stack a packet to transmit.

PARAMETERS
  ds_fwk_inst_ptr:    ptr to fwk inst on which to operate on.
  pkt_ref_ptr:       ref to dsm item ptr received
  meta_info_ptr:     ptr to meta info (TX/RX meta information)

RETURN VALUE
  0: data was transfered
 -1: data was not transferred, but memory was freed

DEPENDENCIES
  None

SIDE EFFECTS
  None
============================================================*/
int ds_fwk_input_legacy
(
  ds_fwk_s_type*                      ds_fwk_inst_ptr,
  dsm_item_type                     **pkt_ref_ptr,
  ds_fwk_clnt_handle_info_s_type     *clnt_handle_info, 
  ps_meta_info_type_ex      *meta_info_ex_ptr
)
{
  ps_tx_meta_info_type  * tx_meta_info_ptr = NULL;
  ps_rx_meta_info_type  * rx_meta_info_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy TX / RX meta information from incoming meta_info_ex_ptr,  */
  if( meta_info_ex_ptr != NULL )
  {
    tx_meta_info_ptr = meta_info_ex_ptr->tx_meta_info_ptr;
    rx_meta_info_ptr = meta_info_ex_ptr->rx_meta_info_ptr;
    /* Clear meta_info_ex_ptr */
    PS_META_INFO_FREE_EX(&meta_info_ex_ptr);
  }
  
   return ds_fwki_input_common_proc( ds_fwk_inst_ptr,
                                     pkt_ref_ptr,
                                     rx_meta_info_ptr, 
                                     tx_meta_info_ptr,
                                     clnt_handle_info,
                                     FALSE);
} /* ds_fwk_input_legacy() */


/*============================================================
FUNCTION DS_FWK_FRAG_TX_CMD()

DESCRIPTION
  This function is called when Frag handle time out happens which flushes
  all the packets inside FragQ using this TX function.

  This function transmits out the passed IP fragment, by calling
  tx_cmd of specified iface.

PARAMETERS
  device_ptr:    Iface
  dsm_item_ptr:  Data Packet
  meta_info_ptr: Meta Info associated with the Packet

RETURN VALUE
 0 for success, -1 in case of an error.

DEPENDENCIES
  None

SIDE EFFECTS
  None
============================================================*/
int ds_fwk_frag_tx_cmd
(
  void                 * device_ptr,
  dsm_item_type       ** dsm_item_ptr,
  ps_tx_meta_info_type * meta_info_ptr
)
{
  ds_fwk_s_type           *fwk_inst_ptr = (ds_fwk_s_type *)device_ptr;

  /*-------------------------------------------------------------------------
  -------------------------------------------------------------------------*/
  DS_MSG1(MSG_LEGACY_HIGH,"ds_fwk_frag_tx_cmd fw inst 0x%p",fwk_inst_ptr);
  if ( fwk_inst_ptr == NULL || (dsm_item_ptr == NULL) )
  {
    return -1;
  }
  if( -1 == ds_fwk_tx_cmd(fwk_inst_ptr,
                   dsm_item_ptr,
                   meta_info_ptr) )
  {
    DS_MSG1(MSG_LEGACY_HIGH,"Failed to transmit data fw inst 0x%p",fwk_inst_ptr);
    return -1;
  }
  return 0;
} /* ds_fwk_frag_tx_cmd() */

/*============================================================
FUNCTION DS_FWKI_INPUT_COMMON_PROC_EX()

DESCRIPTION
   Interface client gives the protocol stack a packet to transmit.

PARAMETERS
  ds_fwk_inst_ptr    :  framework instance.
  pkt_ref_ptr        :  ref to dsm item ptr received
  rx_meta_info_ptr   :  ptr to RX meta meta information
  tx_meta_info_ptr   :  ptr to TX meta meta information

RETURN VALUE
  0: data was transfered
 -1: data was not transferred, but memory was freed

DEPENDENCIES
  None

SIDE EFFECTS
  None
============================================================*/
int ds_fwki_input_common_proc_ex
(
  ds_fwk_s_type                    *ds_fwk_inst_ptr,
  dsm_item_type                   **pkt_ref_ptr,
  ps_rx_meta_info_type             *rx_meta_info_ptr,
  ps_tx_meta_info_type             *tx_meta_info_ptr
)
{
  ps_rx_meta_info_type         *tmp_rx_meta_info_ptr = NULL;
  void                         *fraghdl = NULL;
  dsm_item_type                *frag_ptr;  
  ip_pkt_info_type             *meta_pkt_info_ptr = NULL;
  int                           retval = 0;
  errno_enum_type               ps_errno;
  q_type                        temp_queue;
  ps_icmp6_msg_type             icmp6;
  ds_stats_type                *rx_stats = NULL;
  uint8                         version;  
  uint32                        filter_result = DS_IPFLTR_NOMATCH;       
  uint8                         ip_hdr_first_byte;    
  ds_fwk_clnt_handle_type       client_handle = DS_FWK_INVALID_CLNT_HANDLE;
  ds_fwk_client_type            clnt_type = DS_FWK_CLIENT_INVALID;
  dsm_item_type                *pkt_ptr = NULL;  
  ps_rx_meta_info_type         *mi_ref_ptr = NULL;  
  ds_fwk_clnt_handle_type       rmnet_client_handle = DS_FWK_INVALID_CLNT_HANDLE;	
  ds_fwk_clnt_handle_type       rmsm_client_handle = DS_FWK_INVALID_CLNT_HANDLE;    
  dpl_net_cb                   *rm_dpl = NULL;
  uint8                         protocol = 0;
  uint64                        packet_len_in_bytes = 0;    
  uint16                        dst_port = 0;
  uint8                         buf[4];
  uint16                        offset = 0;  
  uint16                        src_port = 0;  
  ip_pkt_info_type	        pi_ptr;  
  ip_pkt_info_type             *pkt_info = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( pkt_ref_ptr == NULL || *pkt_ref_ptr == NULL )
  {
    //IFACE_INC_INSTANCE_STATS(this_iface_ptr, pkts_dropped_tx, 1);
    DS_MSG0(MSG_LEGACY_ERROR,"ds_fwki_input_common_proc_ex(): "
            "Packet ptr is NULL!");
    PS_RX_META_INFO_FREE(&rx_meta_info_ptr);
    PS_TX_META_INFO_FREE(&tx_meta_info_ptr);
    DS_ASSERT(0);
    return -1;
  }
  (void) dsm_extract(*pkt_ref_ptr, 0, &ip_hdr_first_byte, 1);
  version = ip_hdr_first_byte & DS_FWK_OPT_IP_VERSION_MASK;
  if (version == DS_FWK_OPT_V4_VERSION)  /* IPv4 packet */ 
  {
     /*get Client V4 info*/
     ds_fwk_get_client_info(ds_fwk_inst_ptr, IPV4_ADDR,
                            &clnt_type, &client_handle);

     rmsm_client_handle = ds_fwk_inst_ptr->client_info[DS_FWK_CLIENT_RMSM].v4_client_handle;
     rmnet_client_handle = ds_fwk_inst_ptr->client_info[DS_FWK_CLIENT_RMNET].v4_client_handle;
     rm_dpl = ds_fwk_get_dpl_ptr(ds_fwk_inst_ptr,IP_V4,DS_DPL_CLIENT_RM);
  }
  else if(version == DS_FWK_OPT_V6_VERSION)  /* IPv6 packet */ 
  {
     /*get Client V6 info*/
     ds_fwk_get_client_info(ds_fwk_inst_ptr, IPV6_ADDR,
                            &clnt_type, &client_handle);
     rmsm_client_handle = ds_fwk_inst_ptr->client_info[DS_FWK_CLIENT_RMSM].v6_client_handle;
     rmnet_client_handle = ds_fwk_inst_ptr->client_info[DS_FWK_CLIENT_RMNET].v6_client_handle;
     rm_dpl = ds_fwk_get_dpl_ptr(ds_fwk_inst_ptr,IP_V6,DS_DPL_CLIENT_RM);
  }
  /*-------------------------------------------------------------------------
    Check if the packet gets picked up by a sockets filter, pass it up the
    stack then.
  -------------------------------------------------------------------------*/
  DS_MSG2(MSG_LEGACY_HIGH," ds_fwki_input_common_proc_ex sock filters %d version %d",
          DS_FWK_NUM_FILTERS(ds_fwk_inst_ptr, FLTR_CLIENT_SOCKETS),version);
   
  if( (DS_FWK_NUM_FILTERS(ds_fwk_inst_ptr,
                           FLTR_CLIENT_SOCKETS) > 0 ))
  {
   if( ps_pkt_info_filter_rx_pkt(ds_fwk_inst_ptr, 
                                 FLTR_CLIENT_SOCKETS,
                                 pkt_ref_ptr,
                                 &rx_meta_info_ptr,
                                 &ps_errno, TRUE) < 0 )
    {
      meta_pkt_info_ptr = &(PS_RX_META_GET_PKT_INFO(rx_meta_info_ptr));
      switch(ps_errno)
      {
        /*-------------------------------------------------------------------
          Insufficient information to filter.
        -------------------------------------------------------------------*/
        case E_WOULD_BLOCK:
          /*--------------------------------------------------------------------
            IP fragment scenario! Set the iface in the corresponding Frag handle
            and free the Meta Info.
          --------------------------------------------------------------------*/
          //TODO: Frag scenario
          ps_ip_fraghdl_set_bridge_device(meta_pkt_info_ptr->fraghdl,
                                          (void *)ds_fwk_inst_ptr,
                                          IP_FRAGHDL_BRIDGE_TYPE_IFACE_BRIDGE,
                                          ds_fwk_frag_tx_cmd);
          PS_RX_META_INFO_FREE(&rx_meta_info_ptr);
          PS_TX_META_INFO_FREE(&tx_meta_info_ptr);
          return 0;

        case E_BAD_DATA:
        {
 #ifdef FEATURE_DATA_PS_IPV6
          DS_MSG4( MSG_LEGACY_HIGH, 
                   "ds_fwki_input_common_proc_ex: E_BAD_DATA, ip %d, "
                   "type %d, code %d, offset 0x%X",
                    meta_pkt_info_ptr->ip_vsn,
                    meta_pkt_info_ptr->ptcl_info.icmp.type,
                    meta_pkt_info_ptr->ptcl_info.icmp.code,
                    meta_pkt_info_ptr->ptcl_info.icmp.prob_offset);
          if(meta_pkt_info_ptr->ip_vsn == IP_V6)
          {
            if(0 != meta_pkt_info_ptr->ptcl_info.icmp.type)
            {
              /* Check wehther the packet is destined for AP or Modem 
                 Compare the IID's and send the packet to AP if none of IID's
                 of modem are not matching. */
              if( ds_fwk_inst_is_valid_ex(ds_fwk_inst_ptr, IPV6_ADDR) && 
                  !ps_pkt_info_v6_iid_match(ds_fwk_inst_ptr, pkt_ref_ptr) )
              {
                DS_MSG0(MSG_LEGACY_HIGH,"ds_fwki_input_common_proc_ex(): "
                                         "Packet not matched to local IID");
                break;
              }
              memset(&icmp6, 0, sizeof(icmp6));
              meta_pkt_info_ptr->if_ptr = ds_fwk_inst_ptr;
              icmp6.cmn_hdr.type = (uint8)meta_pkt_info_ptr->ptcl_info.icmp.type;
              icmp6.cmn_hdr.code = (uint8)meta_pkt_info_ptr->ptcl_info.icmp.code;
              icmp6.msg_type.param_prob_offset = meta_pkt_info_ptr->ptcl_info.icmp.prob_offset;
              icmpv6_error_send(pkt_ref_ptr, &icmp6, meta_pkt_info_ptr);
              PS_TX_META_INFO_FREE(&tx_meta_info_ptr);
              PS_RX_META_INFO_FREE(&rx_meta_info_ptr);
              return -1;
            }
          }
#endif
        }

        case E_INVALID_ARG:
        case E_VERSION_MISMATCH:
        case E_NO_MEMORY:
        default:
        /*-------------------------------------------------------------------
            Insufficient information to filter, packet consumed, return.
            - Currently do not allow such packets, Need to DO research more, what
              kind we SHOULD forward!!
        -------------------------------------------------------------------*/
          DS_MSG1(MSG_LEGACY_ERROR,"ds_fwki_input_common_proc_ex(): "
                          "Dropping packet, reason: %d ", ps_errno );
          DS_FWK_INC_INSTANCE_STATS(rx_stats, pkts_dropped_rx, 1);
          PS_RX_META_INFO_FREE(&rx_meta_info_ptr);
          PS_TX_META_INFO_FREE(&tx_meta_info_ptr);
          dsm_free_packet( pkt_ref_ptr );
          return -1;
      }
    }
  }

  /*-----------------------------------------------------------------------
    Send pending fragments too, if there are any. Empty frag queue into 
    local temp queue first to avoid looping if fragments get queued again 
    for local reassembly (ip6_reassemble.)
  -----------------------------------------------------------------------*/
  if (rx_meta_info_ptr != NULL &&
       NULL != (fraghdl = 
               (&(PS_RX_META_GET_PKT_INFO( rx_meta_info_ptr)))->fraghdl))
  {  
    DS_MSG1(MSG_LEGACY_HIGH,"ds_fwki_input_common_proc_ex,sending frags to tx fraghdl %x",fraghdl);
    memset( &temp_queue, 0, sizeof(q_type));
    (void) q_init( &temp_queue );
    while( NULL != (frag_ptr = ip_fraghdl_get_fragment(fraghdl )) )
    {
      q_put( &temp_queue, &frag_ptr->link ); 
    }

    while( NULL != (frag_ptr = q_get( &temp_queue )))
    {
      PS_RX_META_INFO_DUP(rx_meta_info_ptr, &tmp_rx_meta_info_ptr);
      retval = ip_any_input(ds_fwk_inst_ptr,
                            frag_ptr,
                            NULL,
                            FALSE,
                            tmp_rx_meta_info_ptr,
                            TRUE,
                            &pkt_ptr,
                            &offset);
      if(retval != 0)
      {
        DS_MSG3(MSG_LEGACY_ERROR,
                "ds_fwki_input_common_proc_ex Failed to transmit data on instance %p:%d frag_ptr %x",
                 ds_fwk_inst_ptr,ds_fwk_inst_ptr->ds_fwk_index,frag_ptr);
        pkt_info = &(PS_RX_META_GET_PKT_INFO(rx_meta_info_ptr));
        if(pkt_info != NULL)
        {
          pkt_info->fraghdl = NULL;
        }
        PS_RX_META_INFO_FREE(&rx_meta_info_ptr);
        pkt_info = (tx_meta_info_ptr)?(&(PS_TX_META_GET_PKT_INFO(tx_meta_info_ptr))):NULL;
        if(pkt_info != NULL)
        {
          pkt_info->fraghdl = NULL;
        }
        PS_TX_META_INFO_FREE(&tx_meta_info_ptr);
        ip_fraghdl_free(fraghdl);
        q_destroy( &temp_queue );
        return -1;
      }
      else if (pkt_ptr == NULL)
      {
        DS_MSG2(MSG_LEGACY_HIGH,
                "ds_fwki_input_common_proc_ex Fragment queued in Frag queue !! frag_ptr %x "
              "  *pkt_ref_ptr %x",frag_ptr,*pkt_ref_ptr);

      }
      else if (pkt_ptr != NULL)
      {
        DS_MSG0(MSG_LEGACY_HIGH,
                "ds_fwki_input_common_proc_ex All fragments processed, process it further ");
        
      }
    }
    q_destroy( &temp_queue );
  }

  /*-----------------------------------------------------------------------
    Send this pkt and return
  -----------------------------------------------------------------------*/
  if(rx_meta_info_ptr != NULL)
  {
    meta_pkt_info_ptr = &(PS_RX_META_GET_PKT_INFO(rx_meta_info_ptr));	
    if(meta_pkt_info_ptr != NULL)
    {
      if(meta_pkt_info_ptr->ip_vsn == IP_V4)
      {
        protocol = meta_pkt_info_ptr->ip_hdr.v4.protocol;
      }
      else if(meta_pkt_info_ptr->ip_vsn == IP_V6)
      {
        protocol = (uint8)meta_pkt_info_ptr->ip_hdr.v6.hdr_type;
      }
      if(NULL != (fraghdl = 
                (&(PS_RX_META_GET_PKT_INFO( rx_meta_info_ptr)))->fraghdl))
      {
        if((protocol != PS_IPPROTO_ICMP)&&(protocol != PS_IPPROTO_ICMP6))
        {         
          retval = ip_any_input(ds_fwk_inst_ptr,
                               *pkt_ref_ptr,
                                NULL,
                                FALSE,
                                rx_meta_info_ptr,
                                TRUE,
                               &pkt_ptr,
                               &offset);
          DS_MSG2(MSG_LEGACY_ERROR,"ds_fwki_input_common_proc_ex retval %d:pkt_ptr%p",		
                  retval,pkt_ptr);

          if(retval != 0)
          {
            PS_RX_META_INFO_FREE(&rx_meta_info_ptr);
            PS_TX_META_INFO_FREE(&tx_meta_info_ptr);
            return -1;
          }
          else if (pkt_ptr == NULL)
          {
            PS_TX_META_INFO_FREE(&tx_meta_info_ptr);
            return 0;
          }
          else if (pkt_ptr != NULL)
          {
            packet_len_in_bytes = dsm_length_packet(pkt_ptr);
            DS_MSG1(MSG_LEGACY_HIGH,
                    "ds_fwki_input_common_proc_ex All fragments"
                    "processed, process it further %d",packet_len_in_bytes);
          }
         }
       }
       if((protocol == PS_IPPROTO_ICMP ) ||(protocol == PS_IPPROTO_ICMP6))
       {
          if((meta_pkt_info_ptr->ptcl_info.icmp.type == (uint8)ICMP_DEST_UNREACH)||
             (meta_pkt_info_ptr->ptcl_info.icmp.type == ICMP_TIME_EXCEED)||
             (meta_pkt_info_ptr->ptcl_info.icmp.type == ICMP6_DEST_UNRCH)||
             (meta_pkt_info_ptr->ptcl_info.icmp.type == ICMP6_PKT_TOO_BIG)||
             (meta_pkt_info_ptr->ptcl_info.icmp.type == ICMP6_TIME_EXCEEDED)||
             (meta_pkt_info_ptr->ptcl_info.icmp.type == ICMP6_PAR_PROBLEM)||
             (meta_pkt_info_ptr->ptcl_info.icmp.type == ICMP_TIME_EXCEED))
          {
            DS_MSG3(MSG_LEGACY_ERROR,"ds_fwki_input_common_proc_ex Invalid packet type  %d: pkt_ref_ptr %p pkt_ptr %p",		
                  meta_pkt_info_ptr->ptcl_info.icmp.type,*pkt_ref_ptr,pkt_ptr);
            PS_RX_META_INFO_FREE(&rx_meta_info_ptr);
            PS_TX_META_INFO_FREE(&tx_meta_info_ptr);
            dsm_free_packet( pkt_ref_ptr );
            if(pkt_ptr != NULL)
            {
              dsm_free_packet( &pkt_ptr );
            }
            return 0;
          }
          else
          {
		    if((meta_pkt_info_ptr->ptcl_info.icmp.type != (uint8)ICMP_ECHO_REQ)&&
               (meta_pkt_info_ptr->ptcl_info.icmp.type != ICMP_ECHO_REPLY)&&
               (meta_pkt_info_ptr->ptcl_info.icmp.type != ICMP6_ECHO_REQ)&&
               (meta_pkt_info_ptr->ptcl_info.icmp.type != ICMP6_ECHO_RSP))
            {         
              PS_TX_META_INFO_FREE(&tx_meta_info_ptr);
              return ip_any_input(ds_fwk_inst_ptr,
                                  *pkt_ref_ptr,
                                   NULL,
                                   FALSE,
                                   rx_meta_info_ptr,
                                   FALSE,
                                  &pkt_ptr,
                                  &offset);
            }
         }
       }
     }
   }
   if(rx_meta_info_ptr != NULL)
   {
     meta_pkt_info_ptr = &(PS_RX_META_GET_PKT_INFO(rx_meta_info_ptr));
     if(meta_pkt_info_ptr != NULL)
     {
       /*Fetch the protocol from the pkt info
         Filter execution will be taken care only for the TCP and UDP 
         Since for ICMP no filter is been added need to
         match the id and sequence number from RM 
         and DUN
       */
       if(meta_pkt_info_ptr->ip_vsn == IP_V4)
       {
         protocol = meta_pkt_info_ptr->ip_hdr.v4.protocol;
       }
       else if(meta_pkt_info_ptr->ip_vsn == IP_V6)
       {
         protocol = (uint8)meta_pkt_info_ptr->ip_hdr.v6.hdr_type;
         if(offset != 0)
         {
          /*Frag case get the src port and dest port
            extract src and dst port */
          (void)dsm_extract(pkt_ptr, offset, buf, 2*sizeof(uint16));
           src_port = *((uint16*)&buf[0]);
           dst_port = *((uint16*)&buf[2]);
         }
       }
       else
       {
         PS_RX_META_INFO_FREE(&rx_meta_info_ptr);
         PS_TX_META_INFO_FREE(&tx_meta_info_ptr);
         dsm_free_packet( pkt_ref_ptr );		
         if(pkt_ptr != NULL)
           dsm_free_packet( &pkt_ptr );
         return -1;
       }
        
       DS_MSG2(MSG_LEGACY_HIGH,"ds_fwki_input_common_proc_ex protocol %d %p " , protocol,pkt_ptr);
 
       //It means that packet is reassembled from protocols 
      if((protocol == PS_IPPROTO_TCP)||(protocol == PS_IPPROTO_UDP))
      {
        /*Run the filter on the assembled packet 
          If matches send all the fragments to AP else to DUN client */
        if(pkt_ptr == NULL)
           pkt_ptr = *pkt_ref_ptr;
        if( DS_FWK_NUM_FILTERS(ds_fwk_inst_ptr, FLTR_CLIENT_PDN_SHARING) > 0 )
        {
          if((ps_rx_pkt_info_generate(&pkt_ptr, &mi_ref_ptr, &ps_errno, FALSE)) ==0)
          {           
             filter_result = ds_fwk_execute_pdn_sharing_filters
                            (ds_fwk_inst_ptr, &(PS_RX_META_GET_PKT_INFO(mi_ref_ptr)));
             if(filter_result == DS_IPFLTR_NOMATCH)
             {          
                clnt_type  = DS_FWK_CLIENT_RMSM;
                client_handle = rmsm_client_handle;
             }
             else
             {         
                clnt_type  = DS_FWK_CLIENT_RMNET;
                client_handle = rmnet_client_handle;
             }
           }		
           PS_RX_META_INFO_FREE(&mi_ref_ptr);			

           //Check if we have valid src and dst port 
           if((src_port != 0)&&(dst_port !=0))
           {
             if(meta_pkt_info_ptr->ip_vsn == IP_V6)
             {
                pi_ptr.ip_vsn = IP_V6;   
                pi_ptr.ip_hdr.v6.hdr_type = protocol;
                if(protocol == PS_IPPROTO_TCP)
                {
                   pi_ptr.ptcl_info.tcp.src_port = src_port;
                   pi_ptr.ptcl_info.tcp.dst_port = dst_port;
                 }
                 if(protocol == PS_IPPROTO_UDP)
                 {
                    pi_ptr.ptcl_info.udp.src_port = src_port;
                    pi_ptr.ptcl_info.udp.dst_port = dst_port;
                 }
                 filter_result = ds_fwk_execute_pdn_sharing_filters
                                 (ds_fwk_inst_ptr, &pi_ptr);		  
                 if(filter_result == DS_IPFLTR_NOMATCH)
                 {          
                    clnt_type  = DS_FWK_CLIENT_RMSM;
                    client_handle = rmsm_client_handle;
                 }
                 else
                 {
                   clnt_type = DS_FWK_CLIENT_RMNET;
                   client_handle = rmnet_client_handle;
                 }
               }
             }
         }  
         else
         { 
           clnt_type = DS_FWK_CLIENT_RMSM;
           client_handle = rmsm_client_handle;
         }  
      }
      if((protocol == PS_IPPROTO_ICMP) ||(protocol == PS_IPPROTO_ICMP6))
      {
         pkt_ptr = *pkt_ref_ptr;
         //ds_fwk_run_icmp_rule 
         if(TRUE == ds_fwk_is_rmnet_embedded_icmp_packet(meta_pkt_info_ptr,
                                                         meta_pkt_info_ptr->ip_vsn,
                                                         pkt_ptr,
                                                         rmnet_client_handle
                                                         ))
          {
            clnt_type  = DS_FWK_CLIENT_RMNET;
            client_handle = rmnet_client_handle;
          }
          else
          {
            clnt_type  = DS_FWK_CLIENT_RMSM;
            client_handle = rmsm_client_handle;
          }
      }  
    }
  }  
  /*-------------------------------------------------------------------------
   If we make it here, the pkt needs to be forwarded to the bridged iface.
   If fraghdl is non-NULL, get fragments.
  ------------------------------------------------------------------------*/
  if ((rx_meta_info_ptr != NULL) &&
       (NULL != (fraghdl = 
                 (&(PS_RX_META_GET_PKT_INFO( rx_meta_info_ptr)))->fraghdl))&&
       ( TRUE == ps_ip_fraghdl_is_local(fraghdl)))
  {    
    while( NULL != (frag_ptr = ip_fraghdl_get_fragment_ex(fraghdl )) && 
           TRUE == ps_ip_fraghdl_is_local(fraghdl))
    {
      DS_MSG2(MSG_LEGACY_HIGH,"ds_fwki_input_common_proc_ex,sending frags"
              "to client %d ,%p", ps_ip_fraghdl_is_local(fraghdl), frag_ptr);             
      if(client_handle != DS_FWK_INVALID_CLNT_HANDLE && 
         clnt_type == DS_FWK_CLIENT_RMNET )
      {
         /*------------------------------------------------- ------
          Log packet on the Rm iface
         ----------------------------------------------------*/
#ifdef PS_LOGGING_IOE_DPL
        if(rm_dpl != NULL)
        {
          DPL_LOG_NETWORK_TX_PACKET(rm_dpl, frag_ptr, DPL_IID_NETPROT_IP);		  
        }
#endif //PS_LOGGING_IOE_DPL
      }
      ds_fwk_invoke_tx_cb(clnt_type, client_handle, frag_ptr);
    }
    if(pkt_ptr != NULL)
    {
      dsm_free_packet(&pkt_ptr);
    }
    ip_fraghdl_free(fraghdl);
    pkt_info = &(PS_RX_META_GET_PKT_INFO(rx_meta_info_ptr));
    if(pkt_info != NULL)
    {
      pkt_info->fraghdl = NULL;
    }
    pkt_info = (tx_meta_info_ptr)?(&(PS_TX_META_GET_PKT_INFO(tx_meta_info_ptr))):NULL;
    if(pkt_info != NULL)
    {
      pkt_info->fraghdl = NULL;
    }


  }
  else
  {
     if(client_handle != DS_FWK_INVALID_CLNT_HANDLE && 
        clnt_type == DS_FWK_CLIENT_RMNET )
     {
        /*------------------------------------------------- ------
          Log packet on the Rm iface
       ----------------------------------------------------*/
#ifdef PS_LOGGING_IOE_DPL
       if(rm_dpl != NULL)
       {
         DPL_LOG_NETWORK_TX_PACKET(rm_dpl, pkt_ptr, DPL_IID_NETPROT_IP);		 
       }
#endif //PS_LOGGING_IOE_DPL
    }			
    ds_fwk_invoke_tx_cb(clnt_type, client_handle, pkt_ptr);
  }	
  PS_RX_META_INFO_FREE(&rx_meta_info_ptr);
  PS_TX_META_INFO_FREE(&tx_meta_info_ptr);
  return retval;
} /* ds_fwki_input_common_proc_ex */


