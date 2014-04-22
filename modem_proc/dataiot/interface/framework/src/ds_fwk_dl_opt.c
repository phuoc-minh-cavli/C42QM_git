/*==========================================================================*/
/*!
  @file 
  ds_fwk_dl_opt.c

  @brief
   This file is route the DL data packet
   
  Copyright (c) 2018 - 2021 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*/
/*==========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //components/rel/dataiot.mpss/2.0/interface/framework/src/ds_fwk_dl_opt.c#5 $
  $DateTime: 2021/03/11 06:00:52 $$Author: pwbldsvc $

  when          who     what, where, why
  ---------- --- ------------------------------------------------------------
  2018-03-05 svaka  Created module.

===========================================================================*/

/*---------------------------------------------------------------------------
  Include Files
---------------------------------------------------------------------------*/
#include "ds_headers.h"
#include "rdevmap.h"
#include "ds_fwk.h"
#include "ds_defsi.h"
#include "ds_fwki.h"
#include "ds_fwk_opt_defs.h"
#include "ps_metai_info.h"
#include "ds_rmnet_meta_sm.h"
#include "ps_logging.h"
#include "ps_logging_diag.h"
#include "ds_fwki.h"
#include "ds_ipfltr.h"
#include "ps_pkt_info_utils.h"
#ifdef FEATURE_DATA_PS_464XLAT
#include "rmnet_opt_defs.h"
#include "ps_clat.h"
#endif
extern ds_dl_opt_filter_spec_type *ds_global_dl_opt_fltr_arr;

typedef enum
{
  DS_FWK_DL_OPT_NO_PATH      = 0,
  DS_FWK_DL_OPT_OPT_PATH     = 1,
  DS_FWK_DL_OPT_LEGACY_PATH  = 2
} dl_opt_path_type;

#define DS_OPTIMIZED_IP_DL_V4_FRAG_CHECK(val)                              \
    (( (val) & ps_htons(DS_FWK_OPT_V4_FRAG_MASK) ) != 0)
 
/*===========================================================================

                        INTERNAL FUNCTION DEFINITIONS

===========================================================================*/
  
/*===========================================================================
FUNCTION DS_FWK_DL_OPTI_PROCESS_FILTERS()

DESCRIPTION
   executes DL opt filters

PARAMETERS
  filter-ptr:    list of DL opt filters
  filter sepc

RETURN VALUE
  1: if filter matched
  0: if filter does not matches to any of filters in the list

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
INLINE boolean ds_fwk_dl_opti_process_filters
(
  ds_dl_opt_filter_spec_type * filter_ptr,
  uint16 src_port,
  uint16 dst_port,
  uint16 protocol,
  uint16 max_filter_entries
)
{
  uint16                   fltr_dst_port_host;
  uint16                   fltr_src_port_host;
  uint16                   pkt_dst_port_host;
  uint16                   pkt_src_port_host;
  uint8                    filter_cnt;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL != filter_ptr)
  {
    pkt_dst_port_host = ps_ntohs(dst_port);
    pkt_src_port_host = ps_ntohs(src_port);

    for (filter_cnt = 0;
         filter_cnt < max_filter_entries && filter_ptr->fi_handle != 0;
         filter_cnt++, filter_ptr++)
    {
      fltr_dst_port_host = ps_ntohs(filter_ptr->dst_port);
      fltr_src_port_host = ps_ntohs(filter_ptr->src_port);

      if ((protocol == filter_ptr->protocol ||
           (filter_ptr->protocol == PS_IPPROTO_TCP_UDP &&
            (protocol == PS_IPPROTO_TCP || protocol == PS_IPPROTO_UDP))) &&
          ((pkt_dst_port_host >= fltr_dst_port_host &&
            pkt_dst_port_host <=
              fltr_dst_port_host + filter_ptr->dst_port_range) ||
          (pkt_src_port_host >= fltr_src_port_host &&
            pkt_src_port_host <=
              fltr_src_port_host + filter_ptr->src_port_range)))
      {
        return TRUE;
      }
    }
  }

  return FALSE;
} /* ps_dl_opti_process_filters() */

/*===========================================================================
 All Data Path related functions
===========================================================================*/
  
/*===========================================================================
FUNCTION DS_FWK_BEARER_INPUT()

DESCRIPTION
   Transfers the DL packet from bearer to respective client.

PARAMETERS
  bearer_ptr:    on which the packet recieved.
  pkt_ref_ptr:   ref to dsm item ptr received

RETURN VALUE
  0: data was transfered
 -1: data was not transferred, but memory was freed

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ds_fwk_bearer_input
(
  ds_bearer_mgr_s       *bearer_ptr,
  dsm_item_type        **pkt_ref_ptr
)
{
  ds_fwk_s_type                  * ds_fwk_inst_ptr = NULL;
  uint8                            ip_hdr_first_byte;
  uint8                            version;
  ds_fwk_client_type               clnt_type = DS_FWK_CLIENT_INVALID;
  ds_fwk_clnt_handle_type          clnt_handle = DS_FWK_INVALID_CLNT_HANDLE;  
  ds_fwk_clnt_handle_info_s_type   clnt_handle_info;
  dl_opt_path_type                 path_type = DS_FWK_DL_OPT_NO_PATH;
  uint16                           total_len;
  uint16                           dst_port;
  uint16                           src_port;
  uint16                           buf_len;
  uint16                           ip_hdr_len;
  uint8                            protocol;
  ps_meta_info_type_ex           * mi_ex_ptr;
  ds_dl_opt_filter_spec_type     * filter_ptr;
  dpl_net_cb                      *um_dpl;
  dpl_net_cb                      *rm_dpl;
  uint8  pkt_buf[DS_FWK_OPT_EXP_HEADER_LEN_DL + 1] ALIGN(4); //__attribute__((aligned(4)));
  ds_stats_type                   *rm_stats = NULL;
  ds_stats_type                   *um_stats = NULL;
  boolean                          pdn_sharing = FALSE;  
  ps_rx_meta_info_type            *mi_ref_ptr = NULL;  
  errno_enum_type                  ps_errno;
  uint32                           filter_result = DS_IPFLTR_NOMATCH;       
  ds_fwk_clnt_handle_type          rmnet_client_handle = DS_FWK_INVALID_CLNT_HANDLE;    
  ds_fwk_clnt_handle_type          rmsm_client_handle = DS_FWK_INVALID_CLNT_HANDLE;  
#ifdef FEATURE_DATA_PS_464XLAT
  struct ps_in6_addr               dst_v6_addr ;
  ds_fwk_clat_pdn_cntxt_info_type *clat_pdn_cntxt = NULL;  
  boolean                          is_pkt_trnsltd = FALSE;
  uint32                           i;
  uint32                           mask;              
  boolean                          te_match = TRUE;  
  uint32                           u6_addr32[4];  /**< IPv6 address in uint32 format. */
  boolean                          is_broadcast = FALSE;  
  boolean                          prefix_match = TRUE;  
#endif
  /*-------------------------------------------------------------------------
   Algo:
   1)Get the pdn and framework instance from the bearer pointer.
   2)Get the IP Version from packet.
   3)Check whether the PDN is NON - IP instance and invoke QMI API.
   4)if PDN is not non-ip PDN and packet is either TCP/UDP then route the pakcet
       to optmized path/sio if there is no modem embedded client.
   5)if there is only modem embedded client then route the packet to modem embedded
       protocol stack.
   6)when there is a modem embedded client and the packet is either TCP/UDP then 
       run optimized filters , if any filter is matching then route the packet to protocol stack.
   7)if no opt filters are matching then route the packet to optimized path if packet is 
       either TCP/UDP.
   8)if the packet is not TCP/UDP route the packet to modem embedded path.
      ------------------------------------------------------------------------*/

  do
  {
    ds_fwk_inst_ptr = ds_fwki_get_fwk_instance_from_bearer(bearer_ptr);
    DS_MSG3(MSG_LEGACY_HIGH,"ds_fwk_bearer_input bearer 0x%p fw inst 0x%p *pkt_ref_ptr %x",
                             bearer_ptr,ds_fwk_inst_ptr,*pkt_ref_ptr );
    if(ds_fwk_inst_ptr ==NULL)
    {
       path_type = DS_FWK_DL_OPT_NO_PATH;
       break;
    }
    /*Check whether the packet is on non-ip instance and route it*/
	//TODO: need to write API for non_ip_bearer
    if(ds_fwk_is_non_ip_bearer(bearer_ptr))
    { 
      ds_fwk_get_client_info(ds_fwk_inst_ptr,NON_IP_ADDR,
                             &clnt_type,&clnt_handle);
      if(clnt_handle != DS_FWK_INVALID_CLNT_HANDLE)
      {
        ds_fwk_invoke_tx_nipd_cb(clnt_type ,
                                 clnt_handle ,
                                *pkt_ref_ptr);
      }
      else
      {
        path_type = DS_FWK_DL_OPT_NO_PATH;
        break;
      }
      DS_FWK_GLOBAL_STATS_INC;
      return 0;
    }
  
    /*Get the packet IP version and route it*/
   (void) dsm_extract(*pkt_ref_ptr, 0, &ip_hdr_first_byte, 1);
    version = ip_hdr_first_byte & DS_FWK_OPT_IP_VERSION_MASK;

    DS_MSG1(MSG_LEGACY_HIGH,"ds_fwk_bearer_input version %d",version);
    if (version == DS_FWK_OPT_V4_VERSION)  /* IPv4 packet */
    {
      /*get Client V4 info*/
      ds_fwk_get_client_info(ds_fwk_inst_ptr,IPV4_ADDR,
                             &clnt_type,&clnt_handle);
	  
      pdn_sharing = ds_fwk_is_pdn_shared(ds_fwk_inst_ptr, DS_FWK_OPT_V4_VERSION);
      if(pdn_sharing == TRUE)
      {
        rmsm_client_handle = ds_fwk_inst_ptr->\
			                 client_info[DS_FWK_CLIENT_RMSM].v4_client_handle;
        rmnet_client_handle = ds_fwk_inst_ptr->\
			                  client_info[DS_FWK_CLIENT_RMNET].v4_client_handle;
      }

      rm_dpl = ds_fwk_get_dpl_ptr(ds_fwk_inst_ptr,IP_V4,DS_DPL_CLIENT_RM);
      um_dpl = ds_fwk_get_dpl_ptr(ds_fwk_inst_ptr,IP_V4,DS_DPL_CLIENT_UM) ;
      rm_stats = DS_FWK_GET_STATS_PTR(ds_fwk_inst_ptr, IP_V4,DS_STATS_CLIENT_RM);
      um_stats = DS_FWK_GET_STATS_PTR(ds_fwk_inst_ptr, IP_V4,DS_STATS_CLIENT_UM);
      /*---------------------------------------------------------------------
          Fetch IHL and use it to extract IP header and port information from
          the IP packet
      ---------------------------------------------------------------------*/
      ip_hdr_len = (ip_hdr_first_byte & DS_FWK_OPT_IP_HDR_LEN_MASK) << 2;

      buf_len = dsm_extract(*pkt_ref_ptr,
                            0,
                            pkt_buf,
                            ip_hdr_len + DS_FWK_OPT_PORT_INFO_LEN);
      if ((ip_hdr_len + DS_FWK_OPT_PORT_INFO_LEN) > buf_len)
      {
        path_type = DS_FWK_DL_OPT_LEGACY_PATH;
        break;
      }

      /*---------------------------------------------------------------------
        Take legacy path for all IPv4 fragments
      ---------------------------------------------------------------------*/
      if (DS_OPTIMIZED_IP_DL_V4_FRAG_CHECK
          (
            *((uint16 *)(pkt_buf + DS_FWK_OPT_V4_FRAG_OFFSET)))
          )
      {
        path_type = DS_FWK_DL_OPT_LEGACY_PATH;
        break;
      }

      protocol  = *(pkt_buf + DS_FWK_OPT_V4_PROT_FIELD_OFFSET);
      total_len =
        ps_htons(*((uint16 *)(pkt_buf + DS_FWK_OPT_V4_TOTAL_LEN_OFFSET)));
    }
    else if (version == DS_FWK_OPT_V6_VERSION)              /* IPv6 packet */
    {
      ds_fwk_get_client_info(ds_fwk_inst_ptr,IPV6_ADDR,
                             &clnt_type,&clnt_handle);
	  
      pdn_sharing = ds_fwk_is_pdn_shared(ds_fwk_inst_ptr, DS_FWK_OPT_V6_VERSION);	  
      if(pdn_sharing ==  TRUE)
      {
        rmsm_client_handle = ds_fwk_inst_ptr->\
			                 client_info[DS_FWK_CLIENT_RMSM].v6_client_handle;
        rmnet_client_handle = ds_fwk_inst_ptr->\
			                  client_info[DS_FWK_CLIENT_RMNET].v6_client_handle;
      }
	  
      rm_dpl = ds_fwk_get_dpl_ptr(ds_fwk_inst_ptr,IP_V6,DS_DPL_CLIENT_RM);
      um_dpl = ds_fwk_get_dpl_ptr(ds_fwk_inst_ptr,IP_V6,DS_DPL_CLIENT_UM) ;
      rm_stats = DS_FWK_GET_STATS_PTR(ds_fwk_inst_ptr, IP_V6,DS_STATS_CLIENT_RM);
      um_stats = DS_FWK_GET_STATS_PTR(ds_fwk_inst_ptr, IP_V6,DS_STATS_CLIENT_UM);
      /*---------------------------------------------------------------------
        Extract IP header and port information from the IP packet
      ---------------------------------------------------------------------*/
      buf_len = dsm_extract(*pkt_ref_ptr,
                            0,
                            pkt_buf,
                            DS_FWK_OPT_V6_HDR_LEN + DS_FWK_OPT_PORT_INFO_LEN);

      if ((DS_FWK_OPT_V6_HDR_LEN + DS_FWK_OPT_PORT_INFO_LEN) > buf_len)
      {
        path_type = DS_FWK_DL_OPT_LEGACY_PATH;
        break;
      }

      /*---------------------------------------------------------------------
        Take legacy path for IP packets destined to linklocal address
      ---------------------------------------------------------------------*/
      if (PS_IN6_IS_PREFIX_LINKLOCAL
          (
            *(uint32 *) (pkt_buf + DS_FWK_OPT_V6_DEST_ADDR_OFFSET))
          )
      {
        path_type = DS_FWK_DL_OPT_LEGACY_PATH;
        break;
      }

      /*---------------------------------------------------------------------
        Explicit frag check is not needed for IPv6 as all non TCP/UDP packets
        are sent in legacy path
      ---------------------------------------------------------------------*/

      protocol  = *(pkt_buf + DS_FWK_OPT_V6_NXT_HDR_OFFSET);
      total_len =
        ps_htons(*((uint16 *)(pkt_buf + DS_FWK_OPT_V6_TOTAL_LEN_OFFSET)));
      total_len += DS_FWK_OPT_V6_HDR_LEN;
      ip_hdr_len = DS_FWK_OPT_V6_HDR_LEN;	 
#ifdef FEATURE_DATA_PS_464XLAT
      memset(&dst_v6_addr,0, sizeof(dst_v6_addr));
      dst_v6_addr = *((struct ps_in6_addr *)(pkt_buf + RMNET_OPT_V6_DEST_ADDR_OFFSET));	  
      //u6_addr32   =   *(pkt_buf + RMNET_OPT_V6_DEST_ADDR_OFFSET);
      memscpy(&u6_addr32,16,(pkt_buf + RMNET_OPT_V6_DEST_ADDR_OFFSET),16);
      DS_MSG3(MSG_LEGACY_MED,"ds_fwk_bearer_input pkt_buf[25]	%d %d %d",pkt_buf[24],pkt_buf[25],ds_fwk_inst_ptr->is_clat_capable );
      DS_MSG3(MSG_LEGACY_MED,"ds_fwk_bearer_input ds_fwk_inst_ptr %x %d %x",ds_fwk_inst_ptr->clat_pdn_cntxt,ip_hdr_len,ds_fwk_inst_ptr );
      DS_MSG5(MSG_LEGACY_MED,"ds_fwk_bearer_input ds_fwk_inst_ptr %d %d %d %d %d",u6_addr32[0],u6_addr32[1],u6_addr32[2],u6_addr32[3],u6_addr32[0]);
 
      if((ds_fwk_inst_ptr->clat_pdn_cntxt != NULL)&&
         (ds_fwk_inst_ptr->is_clat_capable))
      {
        if((pkt_buf[RMNET_OPT_V6_DEST_ADDR_OFFSET] == 0xFF) &&(pkt_buf[RMNET_OPT_V6_DEST_ADDR_OFFSET+1] == 2))
        {
          is_broadcast = TRUE;
        }
        DS_MSG1(MSG_LEGACY_HIGH,"ds_fwk_bearer_input is_broadcast %d ",is_broadcast);
        if(is_broadcast == FALSE)
        {
          clat_pdn_cntxt =  
              (ds_fwk_clat_pdn_cntxt_info_type *)ds_fwk_inst_ptr->clat_pdn_cntxt;
          if (NULL == clat_pdn_cntxt) 
          {
            DS_MSG0(MSG_LEGACY_ERROR,"ds_fwk_bearer_input:"
                   "Invalid clat pdn cntxt");
            path_type = DS_FWK_DL_OPT_NO_PATH;
            break;
          }
          for (i=0; i<(clat_pdn_cntxt->v6_own_addr_len/32); i++)
          {
            DS_MSG2(MSG_LEGACY_HIGH,"ds_fwk_bearer_input: clat dst address %d own address %d",dst_v6_addr.ps_s6_addr32[i],
               clat_pdn_cntxt->v6_own_addr.ps_s6_addr32[i]);
            if (dst_v6_addr.ps_s6_addr32[i] != clat_pdn_cntxt->v6_own_addr.ps_s6_addr32[i])
            {
              DS_MSG0(MSG_LEGACY_HIGH,"ds_fwk_bearer_input: clat own address not matching ");
              prefix_match = FALSE;
              break;
            }
          }
          if (clat_pdn_cntxt->v6_own_addr_len%32)
          {
            mask = ~((uint32)((1<<(32-(clat_pdn_cntxt->v6_own_addr_len%32)))-1));
            mask = ps_htonl(mask);
            if ((clat_pdn_cntxt->v6_own_addr.ps_s6_addr32[i] & mask) != (dst_v6_addr.ps_s6_addr32[i] & mask))
            {    
              DS_MSG2(MSG_LEGACY_ERROR,"ds_fwk_bearer_input: clat own addres mask  not matching %d %d",

                       (clat_pdn_cntxt->v6_own_addr.ps_s6_addr32[i] & mask),(dst_v6_addr.ps_s6_addr32[i] & mask));
              prefix_match =  FALSE;
            }
          }
          if(prefix_match == FALSE)
          {
            for (i=0; i<(clat_pdn_cntxt->v6_te_addr_len/32); i++)
            {
              DS_MSG2(MSG_LEGACY_ERROR,"ds_fwk_bearer_input: clat dst address %d te address %d",dst_v6_addr.ps_s6_addr32[i],
              clat_pdn_cntxt->v6_te_addr.ps_s6_addr32[i]);
              if (dst_v6_addr.ps_s6_addr32[i] != clat_pdn_cntxt->v6_te_addr.ps_s6_addr32[i])
              {
                DS_MSG0(MSG_LEGACY_ERROR,"ds_fwk_bearer_input: clat te address not matching");
                te_match = FALSE;
                break;
              }
           }
           if (clat_pdn_cntxt->v6_te_addr_len%32)
           {
             mask = ~((uint32)((1<<(32-(clat_pdn_cntxt->v6_te_addr_len%32)))-1));
             mask = ps_htonl(mask);
             if ((clat_pdn_cntxt->v6_te_addr.ps_s6_addr32[i] & mask) != (dst_v6_addr.ps_s6_addr32[i] & mask))
             {    
                DS_MSG2(MSG_LEGACY_ERROR,"ds_fwk_bearer_input: clat te addres mask  not matching %d %d",
                        (clat_pdn_cntxt->v6_te_addr.ps_s6_addr32[i] & mask),(dst_v6_addr.ps_s6_addr32[i] & mask));
                te_match =  FALSE;
           }
         }
        }
        if((prefix_match == TRUE)||(te_match == TRUE))
        {  
         DS_MSG1(MSG_LEGACY_HIGH,"ds_fwk_bearer_input is_broadcast %d ",is_broadcast);
          /*-----------------------------------------------------------------------
           Increment Rx stats on Um iface
          -----------------------------------------------------------------------*/
          DS_FWK_INC_INSTANCE_STATS(um_stats, pkts_rx, 1);
          DS_FWK_INC_INSTANCE_STATS(um_stats, bytes_rx, total_len);
         /*-------------------------------------------------------------------
           Log the packet on the Um iface before translating
         -------------------------------------------------------------------*/
#ifdef PS_LOGGING_IOE_DPL
          DPL_LOG_NETWORK_RX_PACKET(um_dpl,
                                   *pkt_ref_ptr,
                                    DPL_IID_NETPROT_IP);
#endif //PS_LOGGING_IOE_DPL

          DS_FWK_GLOBAL_STATS_INC;
          if( -1 == ps_clat_ip6_to_ip(pkt_ref_ptr,
                                      ds_fwk_inst_ptr,
                                      clat_pdn_cntxt->client_pdn_handle))
          {
            DS_MSG0(MSG_LEGACY_ERROR,"ps_dpm_dl_opti_clat_rx_pkt_internal : "
                            "IP6 to IP conversion failed. Not dropping pkt");          
            path_type = DS_FWK_DL_OPT_NO_PATH;
            break;
          }
         /*-------------------------------------------------------------------
           Update version & is_pkt_trnsltd to indicate that the pkt got
           translated
         -------------------------------------------------------------------*/           
          version = DS_FWK_OPT_V4_VERSION;
          is_pkt_trnsltd = TRUE;
          ds_fwk_get_client_info(ds_fwk_inst_ptr,IPV4_ADDR,
                                 &clnt_type,&clnt_handle);			        pdn_sharing = ds_fwk_is_pdn_shared(ds_fwk_inst_ptr, DS_FWK_OPT_V4_VERSION);
          if(pdn_sharing == TRUE)
          {
             rmsm_client_handle = ds_fwk_inst_ptr->\
                                  client_info[DS_FWK_CLIENT_RMSM].v4_client_handle;
             rmnet_client_handle = ds_fwk_inst_ptr->\
                                  client_info[DS_FWK_CLIENT_RMNET].v4_client_handle;
          }
        }
       }
        else
        {
          //Pass through mode
          ds_fwk_get_client_info(ds_fwk_inst_ptr,IPV6_ADDR,
                             &clnt_type,&clnt_handle);
        }
      }
      if (version == DS_FWK_OPT_V6_VERSION)  /* IPv6 packet didnt get translated */
      {  
        /*---------------------------------------------------------------------
          Explicit frag check is not needed for IPv6 as all non TCP/UDP packets
          are sent in legacy path
        ---------------------------------------------------------------------*/
        protocol  = *(pkt_buf + RMNET_OPT_V6_NXT_HDR_OFFSET);
        ip_hdr_len = RMNET_OPT_V6_HDR_LEN;
      }
  #endif
    }
    else
    {
      DS_MSG0(MSG_LEGACY_ERROR,"ds_fwk_bearer_input: invalid IP version");
      path_type = DS_FWK_DL_OPT_NO_PATH;
      break;
    }
  #ifdef FEATURE_DATA_PS_464XLAT
    if ((version == DS_FWK_OPT_V4_VERSION)&&
        (is_pkt_trnsltd == TRUE))  /* Translated V6 to V4 packet */
    {
       /*--------------------------------------------------------------------- 
       Fetch IHL and use it to extract IP header and port information from
       the IP packet
       ---------------------------------------------------------------------*/
      buf_len = dsm_extract(*pkt_ref_ptr,
                             0,
                             pkt_buf,
                             RMNET_OPT_MAX_V4_HDR_LEN + RMNET_OPT_PORT_INFO_LEN);
      if ((RMNET_OPT_MAX_V4_HDR_LEN + RMNET_OPT_PORT_INFO_LEN) > buf_len)
      { 
        path_type = DS_FWK_DL_OPT_LEGACY_PATH;
        break;
      }  
 
     /*---------------------------------------------------------------------
       Take legacy path for all IPv4 fragments
     ---------------------------------------------------------------------*/
    if (DS_OPTIMIZED_IP_DL_V4_FRAG_CHECK
         (
           *((uint16 *)(pkt_buf + RMNET_OPT_V4_FRAG_OFFSET)))
         )
      {
         path_type = DS_FWK_DL_OPT_LEGACY_PATH;
         break;
      }

       protocol  = *(pkt_buf + RMNET_OPT_V4_PROT_FIELD_OFFSET);
       total_len =
        ps_htons(*((uint16 *)(pkt_buf + RMNET_OPT_V4_TOTAL_LEN_OFFSET)));
      ip_hdr_len = (*pkt_buf & RMNET_OPT_IP_HDR_LEN_MASK) << 2;
    }

    if(version != DS_FWK_OPT_V4_VERSION && version != DS_FWK_OPT_V6_VERSION)
    {
      DS_MSG1(MSG_LEGACY_ERROR,"ps_dpm_dl_opti_clat_rx_pkt_internal(): Invalid IP version %d",
                       version);
      path_type = DS_FWK_DL_OPT_NO_PATH;
      break;
    }
 #endif   
    if(clnt_handle == DS_FWK_INVALID_CLNT_HANDLE) 
    {
      DS_MSG1(MSG_LEGACY_HIGH,"ds_fwk_bearer_input path_type %d",path_type);
      path_type = DS_FWK_DL_OPT_LEGACY_PATH;
      break;
    }
    /*-----------------------------------------------------------------------
        Ship all packets other than TCP and UDP to legacy path
      -----------------------------------------------------------------------*/
    if (protocol != PS_IPPROTO_TCP && protocol != PS_IPPROTO_UDP)
    {
      path_type = DS_FWK_DL_OPT_LEGACY_PATH;
      DS_MSG2(MSG_LEGACY_HIGH,"ds_fwk_bearer_input path_type %d protocol %d",path_type,protocol);
      break;
    }
	
    /*-----------------------------------------------------------------------
      Extract source and destination ports and run optimized socket filters
      to determine if packet needs to be sent in legacy path
    -----------------------------------------------------------------------*/
    src_port = *((uint16 *)(pkt_buf + ip_hdr_len));
    dst_port = *((uint16 *)(pkt_buf + ip_hdr_len + 2));

    filter_ptr = ds_fwk_inst_ptr->dl_opt_fltr_arr;
    if (ds_fwk_dl_opti_process_filters(filter_ptr,
                                   src_port,
                                   dst_port,
                                   protocol,
                                   DS_MAX_OPT_FILTER_ENTRIES) != FALSE)
    {
      DS_MSG1(MSG_LEGACY_HIGH,"ds_fwk_bearer_input path_type %d",path_type);
      path_type = DS_FWK_DL_OPT_LEGACY_PATH;
      break;
    }

    if (ds_fwk_dl_opti_process_filters(ds_global_dl_opt_fltr_arr,
                                   src_port,
                                   dst_port,
                                   protocol,
                                   MAX_GLOBAL_OPT_FILTER_ENTRIES) != FALSE)
    {
      DS_MSG1(MSG_LEGACY_HIGH,"ds_fwk_bearer_input path_type %d",path_type);
      path_type = DS_FWK_DL_OPT_LEGACY_PATH;
      break;
    }

  //If packet has made out till here ,
  //it means that is is not a fragment or legacy packet    
   if(pdn_sharing == TRUE)
   {
      /*
      Fetch the pkt info from the below api 
      This is needed for executing the filters
      */              
      if( DS_FWK_NUM_FILTERS(ds_fwk_inst_ptr, FLTR_CLIENT_PDN_SHARING) > 0 )
      {
        if((ps_rx_pkt_info_generate(pkt_ref_ptr, &mi_ref_ptr, &ps_errno, FALSE)) == 0)
        {        
          filter_result = ds_fwk_execute_pdn_sharing_filters
                         (ds_fwk_inst_ptr, &(PS_RX_META_GET_PKT_INFO(mi_ref_ptr)));		  
          DS_MSG2(MSG_LEGACY_HIGH,"ds_fwk_bearer_input path_type %d "
		  	      "protocol %d",filter_result,protocol);
          if(filter_result == DS_IPFLTR_NOMATCH)
          {          
             DS_MSG1(MSG_LEGACY_HIGH,"ds_fwk_bearer_input filter_result %d",filter_result);
             clnt_type  = DS_FWK_CLIENT_RMSM;
             clnt_handle = rmsm_client_handle;
          }
          else
          {
             clnt_type  = DS_FWK_CLIENT_RMNET;
             clnt_handle = rmnet_client_handle;
          }
       }		
       PS_RX_META_INFO_FREE(&mi_ref_ptr);			
     }
     else
     {
       clnt_type = DS_FWK_CLIENT_RMSM;
       clnt_handle = rmsm_client_handle;
     }
   }
   
 /*-----------------------------------------------------------------------
         Increment Rx stats on Um iface
  -----------------------------------------------------------------------*/
#ifdef FEATURE_DATA_PS_464XLAT
    if(is_pkt_trnsltd == FALSE)
    {
#endif
     DS_FWK_INC_INSTANCE_STATS(um_stats, pkts_rx, 1);
     DS_FWK_INC_INSTANCE_STATS(um_stats, bytes_rx, total_len);
#ifdef PS_LOGGING_IOE_DPL
  /*-----------------------------------------------------------------------
       Log the packet on the Um iface
   -----------------------------------------------------------------------*/
      DPL_LOG_NETWORK_RX_PACKET(um_dpl,*pkt_ref_ptr,DPL_IID_NETPROT_IP);
#endif //PS_LOGGING_IOE_DPL
     DS_FWK_GLOBAL_STATS_INC;
#ifdef FEATURE_DATA_PS_464XLAT
    }
#endif
    if(clnt_handle != DS_FWK_INVALID_CLNT_HANDLE && 
        clnt_type == DS_FWK_CLIENT_RMNET )
    {
  /*-------------------------------------------------------
        Increment Tx stats on Rm iface
 ---------------------------------------------------------*/    
        DS_FWK_INC_INSTANCE_STATS(rm_stats, pkts_tx, 1);
        DS_FWK_INC_INSTANCE_STATS(rm_stats, bytes_tx, total_len);
  /*------------------------------------------------- ------
        Log packet on the Rm iface
      ----------------------------------------------------*/
#ifdef PS_LOGGING_IOE_DPL
        DPL_LOG_NETWORK_TX_PACKET(rm_dpl, *pkt_ref_ptr,DPL_IID_NETPROT_IP);
#endif //PS_LOGGING_IOE_DPL
    }
    ds_fwk_invoke_tx_cb(clnt_type ,clnt_handle , *pkt_ref_ptr);
    return 0;
  }while(0);

  if(path_type == DS_FWK_DL_OPT_LEGACY_PATH)
  {
    PS_META_INFO_GET_EX(mi_ex_ptr);
    if(mi_ex_ptr !=NULL)
    {
     PS_TX_META_INFO_GET_ALL(mi_ex_ptr->tx_meta_info_ptr);
    }
    else
    {
      DS_ASSERT(0);
    }
    if(mi_ex_ptr->tx_meta_info_ptr ==NULL)
    {
      DS_ASSERT(0);
    }
    else
    {
      clnt_handle_info.clnt_handle = clnt_handle;
      clnt_handle_info.clnt_type   = clnt_type;
    }
#ifdef FEATURE_DATA_PS_464XLAT
    if(is_pkt_trnsltd == FALSE)
    {
#endif
#ifdef PS_LOGGING_IOE_DPL
  /*-----------------------------------------------------------------------
       Log the packet on the Um iface
   -----------------------------------------------------------------------*/
   
    DPL_LOG_NETWORK_RX_PACKET(um_dpl,*pkt_ref_ptr,DPL_IID_NETPROT_IP);
#endif //PS_LOGGING_IOE_DPL
#ifdef FEATURE_DATA_PS_464XLAT
    }
#endif
    ds_fwk_input_legacy(ds_fwk_inst_ptr,pkt_ref_ptr,
                        &clnt_handle_info,mi_ex_ptr);
  }
  else
  {
    DS_MSG1(MSG_LEGACY_HIGH,"ds_fwk_bearer_input fw inst 0x%p drop packet",
                             ds_fwk_inst_ptr);
    dsm_free_packet(pkt_ref_ptr);
  }
  return FALSE;
}


