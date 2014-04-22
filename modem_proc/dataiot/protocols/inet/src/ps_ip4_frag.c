/*===========================================================================

                        P S _ I P 4 _ F R A G . C

DESCRIPTION
  Contains routines to perform IPv4 fragmentation and reassembly

Copyright (c) 2005-2021 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/protocols/inet/src/ps_ip4_frag.c#7 $
  $DateTime: 2021/06/22 06:10:32 $
  $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/22/21    ds     Code changes to fix double free of meta info ptr in case of
                   UL packet is not sent due to flow disabled.
04/07/21   ds      Code changes to use correct structure data type to fetch
                   CLAT SM pointer from CLAT PDN context.
12/30/20   ds      Added XLAT feature.
04/17/20   ds      Code changes to use MUTILS to efficiently utilize DSM items.
04/14/20   ds      Code changes for FRAG support in DUN+AP call concurrency.
07/02/19   ds      Code changes to add fwk instance validation before sending 
                   v4 fragment.
10/12/18   ds      Fixed the KW errors.
08/13/18   ds      Code changes as per new FWK design for IOT.
12/23/14   ss      Add limit on maximum entries in frag queue
08/08/14   cx      Free the oringinal packet if fragmentation fails in 
                   ip4_fragment_optimized()
04/28/14   cx      Implement ip4_fragment_optimized().
07/08/13   cx      Replace memcpy and memmove with memscpy and memsmove
12/12/12   ds      Replacing LOG_MSG_FATAL_ERROR with LOG_MSG_ERROR. 
02/24/11   cp      Fixed compiler warnings.
01/07/11   cp      ip4_fragment() API is changed to accept 
                   PATH MTU as input param.
11/14/10   am      Update ip hdr lenght in ip_pkt_info in ip4_fragment().
11/06/09   ba      set the fraghdl in the pkt_info to NULL after reassembling
09/24/09   ss      Fixed KW warnings.
09/30/09   ss      IP hdr init before parsing in ip4_reassemble().
08/21/09   sp      Fix for recalculating IP Header checksum after reassembling
                   the IP fragments.
03/13/08   pp      Metainfo optimizations.
10/24/05   ifk     Removed check for NULL meta info in ip4_fragment()
08/12/05   kr      Added code for updating statistics
05/11/05   sv      Lint changes.
05/04/05   ifk     Fixed bug in reassembling duplicate fragments.
04/20/05   sv      Modified ip4_reassmebly to get fraghdl from pkt info.
01/06/05   msr     Freeing meta info and DSM item in ip4_fragment() in all
                   error cases also.
08/28/04   ifk     Created module.
===========================================================================*/
/*===========================================================================

                                INCLUDE FILES

===========================================================================*/
#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */
#include "msg.h"
#include "amssassert.h"

#include "dsm.h"
#include "ps_ip4_hdr.h"
#include "ps_ip4_frag.h"

#include "err.h"
#include "ps_ip.h"
#include "ps_tcp.h"
#include "ps_iputil.h"
#include "ps_ip_fraghdl.h"
#include "ds_Utils_DebugMsg.h"
#include "mutils_dsm.h"
#include "mutils_common.h"
#include "ds_system_heap.h"
#ifdef FEATURE_DATA_PS_464XLAT
#include "ds_fwki.h"
#include "ps_clat_sm.h"
#include "ps_clat.h"
#endif /*#ifdef FEATURE_DATA_PS_464XLAT*/

/*---------------------------------------------------------------------------
  Macro for calcuating the header checksum based on RF1642:
  HC' = HC - ~m - m 
  HC : old checksum in header
  HC': new checksum in header
  m  : old value of a 16-bit field
  m' : new value of a 16-bit field    
----------------------------------------------------------------------------*/
#define GET_CARRY_FLAG(value_1, value_2) ((value_1)>=(uint16)(value_2)?0:1)
#define SUB_WITH_CARRY(value_1, value_2) \
        ((value_1) - (value_2) - GET_CARRY_FLAG(value_1,value_2))
#define UPDATE_CHECKSUM(cksum,old_value, new_value)\
  ((cksum) = SUB_WITH_CARRY(SUB_WITH_CARRY((cksum),(~(old_value))),(new_value)))

/*===========================================================================

                    PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION IP4_FRAGMENT

DESCRIPTION
  This function is called with an IPv4 datagram, meta info associated with
  the datagram and the PS iface to send the datagram over.  It fragments
  and transmits the datagram.

  NOTE: Meta Info and DSM items are freed in error cases also.

DEPENDENCIES
    None

RETURN VALUE
  -1 in case of error
  0 in case of success
 
SIDE EFFECTS
  The datagram is transmitted as fragments in case of success.
===========================================================================*/
int ip4_fragment
(
  dsm_item_type       **ip_pkt_ptr,
  struct ip            *pkt_hdr,
  ds_fwk_s_type        *rt_if_ptr,
  ps_tx_meta_info_type *meta_info_ptr, 
  uint32                path_mtu,
  boolean               *is_meta_info_valid
)
{
  int                retval = 0;
  struct ip          hdr;
  uint16             i = 0;
  uint16             fragment_offset;
  uint16             fragment_payload;
  dsm_item_type     *fragment_ptr = NULL;
  int                opt_size;
  uint16             copy_bytes;
  ps_tx_meta_info_type *local_meta_info_ptr;
  int                mf_bit;
  uint16             hlen = 0;
  uint16             payload_length;
  ip_pkt_info_type   *pkt_info_ptr=NULL;
  uint8              *temp_buf = NULL;
#ifdef FEATURE_DATA_PS_464XLAT
  void               *instance = NULL;
  ds_fwk_clat_pdn_cntxt_info_type *clat_pdn_context = NULL;
  uint8           buf[4] = {0};
  uint16          dsm_offset = 0;
  uint16          src_port = 0;
  uint16          dst_port = 0;
  ps_clat_sm_type *clat_sm_ptr = NULL;
#endif
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Verify arguments
  -------------------------------------------------------------------------*/
  if( NULL == ip_pkt_ptr || NULL == *ip_pkt_ptr )
  {
    PS_TX_META_INFO_FREE(&meta_info_ptr);
  }

  if( NULL == rt_if_ptr || NULL == pkt_hdr )
  {
    dsm_free_packet(ip_pkt_ptr);
    PS_TX_META_INFO_FREE(&meta_info_ptr);
  }
  
  if( NULL == ip_pkt_ptr || NULL == *ip_pkt_ptr || NULL == rt_if_ptr || NULL == pkt_hdr )
  {
    LOG_MSG_ERROR_2("NULL DSM item passed or NULL arg: rt_if_ptr %p, pkt_hdr %p",
                    rt_if_ptr, pkt_hdr);
    return -1;
  }

  LOG_MSG_INFO1_2("ip4_fragment called to fragment packet of length %d since MTU is %d",
                   pkt_hdr->length, path_mtu);
  /*-------------------------------------------------------------------------
    Copy the IP header
  -------------------------------------------------------------------------*/
  memscpy( &hdr, sizeof(hdr), pkt_hdr, sizeof(struct ip) );
  hlen = (uint16)(hdr.ihl * 4);

  /*-------------------------------------------------------------------------
    Pull off header
  -------------------------------------------------------------------------*/
  if( hlen != dsm_pullup( ip_pkt_ptr, NULL, hlen) )
  {
    dsm_free_packet(ip_pkt_ptr);
    PS_TX_META_INFO_FREE(&meta_info_ptr);
    LOG_MSG_ERROR_0( "error from dsm_pullup while getting IP header" );
    return -1;
  }

  /*-------------------------------------------------------------------------
    Calculate payload length (including offset of original datagram), mf bit
    value set in original datagram, fragment offset and fragment payload.
    Set mf bit in header to 1.
  -------------------------------------------------------------------------*/
  mf_bit           = hdr.mf;
  fragment_offset  = (uint16)hdr.off * 8;
  payload_length   = ((uint16)hdr.length) - hlen + fragment_offset;
  hdr.mf           = 1;
  fragment_payload = ((uint16)path_mtu - hlen) & ~0x7;
  hdr.optlen       = hlen - IPLEN;

  /*-------------------------------------------------------------------------
    Save local copy of meta info
  -------------------------------------------------------------------------*/
  local_meta_info_ptr = meta_info_ptr;
  if (meta_info_ptr != NULL && NULL != PS_TX_META_GET_RT_META_INFO_PTR(meta_info_ptr))
  {
    pkt_info_ptr = &PS_TX_META_GET_PKT_INFO(meta_info_ptr);
  }
  else
  {
    LOG_MSG_ERROR_1("Found NULL RT for tx meta info 0x%p", meta_info_ptr);
    dsm_free_packet(ip_pkt_ptr);
    PS_TX_META_INFO_FREE(&meta_info_ptr);
    return -1;
  }

  /*-------------------------------------------------------------------------
    Form and TX fragments
  -------------------------------------------------------------------------*/
  while( fragment_offset < payload_length )
  {
    /*-----------------------------------------------------------------------
      Calculate payload of this fragment.  Dup the passed DSM item to form
      this fragment
    -----------------------------------------------------------------------*/
    copy_bytes = MIN(fragment_payload, (payload_length - fragment_offset));
    /*Allocate a DSM item to store the compressed header based on its length */
    fragment_ptr = mutils_dsm_anysize_new_buffer(MUTILS_DSM_CLIENT_PS,
                                                 DSM_DS_LARGE_ITEM_POOL,
                                                 copy_bytes);

    /*-----------------------------------------------------------------------
      Allocate Memory for temporary buffer to hold fragment payload. 
      Make sure to free this memory once it is copied to fragment DSM item.
    -----------------------------------------------------------------------*/
    DS_SYSTEM_HEAP_MEM_ALLOC(temp_buf, copy_bytes, uint8*);
    if(temp_buf == NULL)
    {
      LOG_MSG_ERROR_0("Malloc failed");
      dsm_free_packet( &fragment_ptr );
      retval = -1;  
      break;
    }

    /*-----------------------------------------------------------------------
      Fetch the fragment payload from the original packet. 
    -----------------------------------------------------------------------*/
    if( copy_bytes != dsm_pullup( ip_pkt_ptr, temp_buf, copy_bytes ) )
    {
      retval = -1;
      dsm_free_packet( &fragment_ptr );
      break;
    }

    /*-----------------------------------------------------------------------
    Copy the frag payload. 
    -----------------------------------------------------------------------*/
    ASSERT((fragment_ptr != NULL) &&
          (fragment_ptr->used + copy_bytes <= fragment_ptr->size));
    memscpy((fragment_ptr->data_ptr + fragment_ptr->used),
            (fragment_ptr->size - fragment_ptr->used),
            temp_buf, copy_bytes);
   
    /* increase the used fields */
    fragment_ptr->used += copy_bytes;
   
    /* set the cache flush flag to ture */
    dsm_item_set_cache_operation(fragment_ptr,DSM_PKT_CACHE_FLUSH_ON);

    /*-----------------------------------------------------------------------
      Form the IP header for the fragment by updating the total_length,
      flags and fragment offset fields.
      In setting the MF bit, if original datagram was a fragment then don't
      clear the MF bit even for the last fragment
    -----------------------------------------------------------------------*/
    hdr.length = copy_bytes + hlen;
    if( (fragment_offset + copy_bytes) >= payload_length && mf_bit != 0x1 )
    {
      hdr.mf = 0;
    }
    hdr.offset = fragment_offset;
    hdr.off = (fragment_offset >> 3);

    if( FALSE == ps_ip4_hdr_create_ex( &fragment_ptr, &hdr, FALSE ) )
    {
      retval = -1;
      dsm_free_packet( &fragment_ptr );
      break;
    }

    /* Modify the ip pkt info's ip4 hdr params, 
     * aware we are modifying the dup mem here
     */
    pkt_info_ptr->ip_hdr.v4.length = copy_bytes + hlen;
    pkt_info_ptr->ip_hdr.v4.offset = fragment_offset;
    pkt_info_ptr->ip_hdr.v4.off =  (fragment_offset >> 3);

    LOG_MSG_INFO1_3("Generated Fragment header length = %d with header of %d and payload of %d",
                    pkt_info_ptr->ip_hdr.v4.length, hlen, copy_bytes);
    /*-----------------------------------------------------------------------
      Allocate a meta_info_ptr for the fragment and copy the original
      datagram's meta_info into it.
    -----------------------------------------------------------------------*/
    PS_TX_META_INFO_DUP( local_meta_info_ptr, &meta_info_ptr );
    if( NULL != local_meta_info_ptr && NULL == meta_info_ptr )
    {
      retval = -1;
      dsm_free_packet( &fragment_ptr );
      break;
    }

    /*-----------------------------------------------------------------------
      TX the fragment.  In case of error, tx_cmd is responsible for freeing
      the fragment and meta info
    -----------------------------------------------------------------------*/
    if( meta_info_ptr != NULL && meta_info_ptr->rt_meta_info_ptr != NULL)
    {
      meta_info_ptr->rt_meta_info_ptr->pkt_info.direction = TRUE;
      DS_MSG0(MSG_LEGACY_HIGH,"Frag debug! sending uplink Frag packet");
    }
	 
#ifdef FEATURE_DATA_PS_464XLAT	 
    /* Check if this is CLAT Call then call API to translate the packets
       before giving it to FWK for Tx only if IPv4 protocol is not ESP. */
    if(rt_if_ptr->clat_pdn_cntxt != NULL && 
       hdr.protocol != PS_IPPROTO_ESP)
    {
      clat_pdn_context = (ds_fwk_clat_pdn_cntxt_info_type*)rt_if_ptr->clat_pdn_cntxt;
      instance = (void*)clat_pdn_context->client_pdn_handle;
      /* Peek into the IPv4 packet and fetch the Dst port and if it matches the 
         DNS server port then set the boolean accordingly. */
      dsm_offset = IPV4_HEADER_LENGTH_OFFSET;

      (void)dsm_extract(*ip_pkt_ptr, dsm_offset, buf, 2*sizeof(uint16));
      src_port = *((uint16*)&buf[0]);
      dst_port = *((uint16*)&buf[2]);
      LOG_MSG_INFO1_2("ip4_route(): src port = 0x%x, dst port = 0x%x,",src_port, dst_port);
      if(dst_port == PS_DNS_PROXY_SERVER_PORT)
      {
         clat_sm_ptr  = (ps_clat_sm_type*)instance;
         LOG_MSG_INFO1_0( "UL DNS query with XLAT: set the flag ");
         clat_sm_ptr->is_dns_query = TRUE; 
      }
      LOG_MSG_INFO1_1("Translate the packet: CLAT inst 0x%x",instance);
      if(-1 == ps_clat_ip_to_ip6(&fragment_ptr, rt_if_ptr, instance ))
      {
        retval = -1;
        dsm_free_packet( &fragment_ptr );
        break;
      }
    }
#endif     
     
    /*---------------------------------------------------------------------
      Validate the Fwk instance before calling Fwk API to send the packet.
    ----------------------------------------------------------------------*/
    if(!ds_fwk_inst_is_valid_ex (rt_if_ptr, IPV4_ADDR) || (rt_if_ptr->is_clat_capable && 
         ds_fwk_inst_is_valid_ex(rt_if_ptr,IPV6_ADDR)))
    {
      dsm_free_packet( &fragment_ptr );
      retval = -1;
      break;
    }
	
    retval = ds_fwk_tx_cmd(rt_if_ptr, &fragment_ptr, meta_info_ptr );
    if( -1 == retval )
    {
      *is_meta_info_valid = FALSE;
      break;
    }

    /*-----------------------------------------------------------------------
      Handle options for subsequent fragments by copying from first fragment
    -----------------------------------------------------------------------*/
    if( 0 == fragment_offset )
    {
      /*---------------------------------------------------------------------
        Copy only those options which go into a fragment.
      ---------------------------------------------------------------------*/
      while( i < (hlen - IPLEN) )
      {
        /*-------------------------------------------------------------------
          Handle the one byte options
        -------------------------------------------------------------------*/
        if( hdr.options[i] == IP_EOL )
        {
          if( hlen - IPLEN != i )
          {
            hdr.optlen = i;
            hlen       = i + IPLEN;
            hdr.ihl    = (hlen>>2);
          }
          break;
        }
        if( hdr.options[i] == IP_NOOP )
        {
          i++;
          continue;
        }
        /*-------------------------------------------------------------------
          Handle the type-length-value options.  Copy only if the OPT_COPIED
          flag is set.  Otherwise set the option to NO-OP
        -------------------------------------------------------------------*/
        if( (i+1) >= IP_MAXOPT )
        {
          LOG_MSG_ERROR_0("Header options index exceeds limit.");
          retval = -1;
          break;
        }
        if( OPT_COPIED & hdr.options[i] )
        {
          i += hdr.options[i+1];
        }
        else
        {
          opt_size = hdr.options[i+1];
          for(; i<(i+opt_size) && i<IP_MAXOPT; i++ )
          {
            hdr.options[i] = IP_NOOP;
          }
        }
      } /* while */
    }

    /*-----------------------------------------------------------------------
      Update the fragment offset and the offset into the original DSM item
      by the payload bytes in the fragment just sent
    -----------------------------------------------------------------------*/
    fragment_offset += copy_bytes;

    /*---------------------------------------------------------------------
     Free the temporary buffer.
    ----------------------------------------------------------------------*/
    DS_SYSTEM_HEAP_MEM_FREE(temp_buf);
	
  } /* while */

  if (NULL != *ip_pkt_ptr)
  {
    dsm_free_packet(ip_pkt_ptr);
  }
  PS_TX_META_INFO_FREE( &local_meta_info_ptr );
  
  if (retval == -1)
  {
    /* Free the temp_buf heap memory. */
    DS_SYSTEM_HEAP_MEM_FREE(temp_buf);
  }

  return retval;
} /* ip4_fragment() */

/*===========================================================================
FUNCTION IP4_FRAGMENT_OPTIMIZED

DESCRIPTION
  This function is called to fragment one packet into several fragments based 
  on passed in MTU value.All the fragments will be stored in the passed in 
  buffer

DEPENDENCIES
  PS_IP4_HDR_PARSE_OPTIMIZED() should be called to parse the IPv4 header 
  before this function is called

RETURN VALUE
  -1 in case of error
  0 in case of success
 
SIDE EFFECTS
  In good case, the fragments are stored in the queue, the oringinal packet 
  will be freed.
  In error case, all the fragments are freed, oringinal packet will NOT be
  freed.
===========================================================================*/
int ip4_fragment_optimized
(
  dsm_item_type       **ip_pkt_ptr,
  struct ip            *hdr,
  uint32                path_mtu,
  dsm_item_type       **frag_buf, 
  uint16                frag_buf_size
)
{
  int                retval = 0;  
  uint16             frag_num = 0;
  uint16             original_packet_len;
  uint16             original_offset;
  int                original_df;
  int                original_congest;
  int                original_mf;
  uint16             fragment_max_len;
  uint16             offset = 0;
  uint16             fragment_len=0;
  dsm_item_type     *fragment_dsm_ptr = NULL;
  uint16             old_flag_offset;
  uint16             new_flag_offset;
  uint8              *temp_buf = NULL;
  uint16             bytes_to_copy = 0;
  /*------------------------------------------------------------------------
    Parameters Check 
  -------------------------------------------------------------------------*/
  if(NULL == ip_pkt_ptr || 
     NULL == frag_buf || 
     0 == path_mtu)
  {
    LOG_MSG_ERROR_0("ip4_fragment_ex(): Invalid input arguments!");
    return -1;
  }
  
  /*------------------------------------------------------------------------
    Check buffer size is big enough to hold all the fragments
  -------------------------------------------------------------------------*/
  original_packet_len = ps_ntohs(hdr->length);
  original_offset     = (uint16)hdr->off * 8;
  original_df         = hdr->df;
  original_congest    = hdr->congest;
  original_mf         = hdr->mf;
  old_flag_offset     = hdr->flags_offset;
  hdr->mf             = 1;
  fragment_max_len    = (((uint16)path_mtu - IPLEN) & ~0x7) + IPLEN;

  /*------------------------------------------------------------------------
    Check buffer size is big enough to hold all the fragments
  -------------------------------------------------------------------------*/
  if((original_packet_len - IPLEN) > frag_buf_size*((uint16)path_mtu-IPLEN) )
  {
    LOG_MSG_ERROR_1( "ip4_fragment_ex():"
                     "fragment number exceed the buffer size! "
                     "packet ptr: 0x%x",
                      ip_pkt_ptr);
    dsm_free_packet(ip_pkt_ptr);
    return -1;
  }

  /*-------------------------------------------------------------------------
    Form fragments and push it into buffer
  -------------------------------------------------------------------------*/
  while( offset < (original_packet_len - IPLEN))
  {  
    /*-----------------------------------------------------------------------
      Calculate payload of this fragment.  Dup the passed DSM item to form
      this fragment
    -----------------------------------------------------------------------*/
    fragment_len = MIN(fragment_max_len, (original_packet_len - offset));

    /*Allocate a DSM item to store the compressed header based on its length */
    fragment_dsm_ptr = mutils_dsm_anysize_new_buffer(MUTILS_DSM_CLIENT_PS,
                                                     DSM_DS_LARGE_ITEM_POOL,
                                                     (fragment_len - IPLEN) );

    /*-----------------------------------------------------------------------
      Allocate Memory for temporary buffer to hold fragment payload. 
      Make sure to free this memory once it is copied to fragment DSM item.
    -----------------------------------------------------------------------*/
    bytes_to_copy = (fragment_len - IPLEN);
    DS_SYSTEM_HEAP_MEM_ALLOC(temp_buf, bytes_to_copy, uint8*);
    if(temp_buf == NULL)
    {
      LOG_MSG_ERROR_0("Malloc failed");
      dsm_free_packet( &fragment_dsm_ptr );
      retval = -1;  
      break;
    }

    /*-----------------------------------------------------------------------
      Fetch the fragment payload from the original packet. 
    -----------------------------------------------------------------------*/
    if( bytes_to_copy != dsm_extract( *ip_pkt_ptr, (offset + IPLEN), 
                                       temp_buf, bytes_to_copy ) )
    {
      retval = -1;
      dsm_free_packet( &fragment_dsm_ptr );
      break;
    }

    /*-----------------------------------------------------------------------
    Copy the frag payload. 
    -----------------------------------------------------------------------*/
    ASSERT((fragment_dsm_ptr != NULL) &&
          (fragment_dsm_ptr->used + bytes_to_copy <= fragment_dsm_ptr->size));

    memscpy((fragment_dsm_ptr->data_ptr + fragment_dsm_ptr->used),
            (fragment_dsm_ptr->size - fragment_dsm_ptr->used),
            temp_buf, bytes_to_copy);
   
    /* increase the used fields */
    fragment_dsm_ptr->used += bytes_to_copy;

    /*-----------------------------------------------------------------------
      Flags_offset always changes. So update the flags,offset and checksum
      for each fragments
    -----------------------------------------------------------------------*/
    if(fragment_len + offset >= original_packet_len &&  
       original_mf != 1)
    {
      hdr->mf = 0;
    }   
    hdr->offset     = original_offset + offset;
    hdr->congest    = original_congest;
    hdr->df         = original_df;

    new_flag_offset = (((uint16)hdr->congest) << 15) |
                           (((uint16)hdr->df) << 14) |
                           (((uint16)hdr->mf) << 13) |
                        ((uint16)(hdr->offset >> 3));     
    
    ((uint16*)hdr)[3] = ps_htons(new_flag_offset);
    
    hdr->checksum = ps_htons( hdr->checksum);

    UPDATE_CHECKSUM( hdr->checksum,
                     old_flag_offset,
                     new_flag_offset);

    /*-----------------------------------------------------------------------
      Cache the old flag_offset for next fragment usage
    -----------------------------------------------------------------------*/
    old_flag_offset = new_flag_offset;
     
    /*-----------------------------------------------------------------------
      If packet length changed, update the checksum and length in header
    -----------------------------------------------------------------------*/
    if(hdr->length != ps_htons(fragment_len))
    {  
      UPDATE_CHECKSUM( hdr->checksum,
                       ps_ntohs(hdr->length),
                       fragment_len);
      hdr->length = ps_htons(fragment_len);
    }

    hdr->checksum = ps_htons(hdr->checksum);
  
    /*-----------------------------------------------------------------------
       Push the header into fragment packet
    -----------------------------------------------------------------------*/
    if ((dsm_pushdown_packed(&fragment_dsm_ptr, 
                            ((uint8*)hdr), IPLEN,
                            DSM_DS_SMALL_ITEM_POOL))< IPLEN)
    {
      LOG_MSG_ERROR_0("Error in dsm_pushdown_packed()"
                      " in ip4_frag_optimized()");
      retval = -1;
      break;
    }
    
    /*-----------------------------------------------------------------------
      Store the fragment pointer in buffer
    -----------------------------------------------------------------------*/
    frag_buf[frag_num] = fragment_dsm_ptr;  
    
    /*-----------------------------------------------------------------------
      Increase the number of fragments
    -----------------------------------------------------------------------*/
    frag_num++;
    
    /*-----------------------------------------------------------------------
      Update the offset
    -----------------------------------------------------------------------*/
    offset += (fragment_len-IPLEN);

    /*---------------------------------------------------------------------
     Free the temporary buffer.
    ----------------------------------------------------------------------*/
    DS_SYSTEM_HEAP_MEM_FREE(temp_buf);

  } /* offset < (original_packet_len -20) */

  /*-------------------------------------------------------------------------
    Free all the fragments in error case;
    Free the oringinal packet in good case;
  -------------------------------------------------------------------------*/
  if( -1 == retval )
  {
    while( NULL != frag_buf[frag_num] &&
           0 != frag_num)
    {
      frag_num--;
      dsm_free_packet( &frag_buf[frag_num] );
      frag_buf[frag_num] = NULL;
    }  

    /* Free the temp_buf heap memory. */
    DS_SYSTEM_HEAP_MEM_FREE(temp_buf);
  }
  
  /*-------------------------------------------------------------------------
    Free the oringinal packet
  -------------------------------------------------------------------------*/
  dsm_free_packet(ip_pkt_ptr);
  
  return retval;
}


/*===========================================================================
FUNCTION IP4_REASSEMBLE()

DESCRIPTION
  Function is passed a fragment and associated packet info for the fragment
  along with offset to payload of fragment.  If enough fragments have been
  collected to complete the datagram return value is pointer to DSM item
  containing the datagram with the header returned in the pkt_info argument
  and offset to datagram payload in offset argument.  Otherwise the fragment
  is queued and a NULL returned.

DEPENDENCIES
  None

RETURN VALUE
  If datagram is complete, returns DSM chain containing datagram else
  returns NULL.

SIDE EFFECTS
  Passed fragment may be queued and NULL returned in which case no
  further action should be taken on this fragment by the caller
===========================================================================*/
struct dsm_item_s *ip4_reassemble
(
  ip_pkt_info_type  *pkt_info,/* IP packet information                     */
  struct dsm_item_s *pkt_ptr, /* The fragment itself including IP hdr      */
  uint16            *offset,  /* IP pkt offset                             */
  boolean           is_concurrency_on  /* Boolean for concurrency check    */
)
{
  void          *handle;
  uint16         datagram_len;
  uint16         frag_offset = 0;
  dsm_item_type *reas_ptr = NULL;
  dsm_item_type *curr_ptr;
  struct ip      currhdr;
  uint16         payload_offset = 0;
  uint16         local_offset = 0;
  uint16         curr_offset;
  uint16         curr_len;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
   LOG_MSG_ERROR_0("ip4_reassemble called");
  /*-------------------------------------------------------------------------
    Verify arguments
  -------------------------------------------------------------------------*/
  if( NULL == pkt_ptr || NULL == pkt_info || NULL == offset )
  {
    LOG_MSG_ERROR_3("NULL arg: pkt_ptr 0x%x, pkt_info 0x%x, offset 0x%x",
        (uint32)pkt_ptr, (uint32)pkt_info, (uint32)offset);
    dsm_free_packet(&pkt_ptr);
    DS_ASSERT( 0 );
    return NULL;
  }

  /*-------------------------------------------------------------------------
    Verify fraghdl
  -------------------------------------------------------------------------*/
  if (NULL == (handle = pkt_info->fraghdl))
  {
    LOG_MSG_ERROR_0("NULL fraghdl passed");
    dsm_free_packet(&pkt_ptr);
    DS_ASSERT( 0 );
    return NULL;
  }
  ps_ip_fraghdl_set_local(handle);

  /*-------------------------------------------------------------------------
    Initialization.
  -------------------------------------------------------------------------*/
  memset((void *)&currhdr, 0, sizeof(currhdr));

  /*-------------------------------------------------------------------------
    Parse the current fragment.
  -------------------------------------------------------------------------*/
  if(ps_ip4_hdr_parse(pkt_ptr, offset, &(pkt_info->ip_hdr.v4)) == FALSE)
  {
    LOG_MSG_ERROR_0("Failed to parse IP packet");
    return NULL;
  }

  /*-------------------------------------------------------------------------
    Set local offset to offset to v4 header
  -------------------------------------------------------------------------*/
  local_offset = *offset - ((uint16)(pkt_info->ip_hdr.v4.ihl *4));

  if(ip_fraghdl_put( handle, pkt_ptr, pkt_info ) != 0)
  {
    LOG_MSG_ERROR_0("Failed to put frag to queue, freeing packet");
  
    /* Free the packet */
    dsm_free_packet( &pkt_ptr );
    return NULL;
  }

  /*-------------------------------------------------------------------------
    Check if all fragments of fragmented datagram are present.  If so
    reassemble
  -------------------------------------------------------------------------*/
  if( ip_fraghdl_get_initial_cont_bytes_count( handle ) ==
      (datagram_len = ip_fraghdl_get_datagram_len( handle )) &&
      0 != datagram_len )
  {
    /*-----------------------------------------------------------------------
      Go through all the fragments until have a complete datagram
    -----------------------------------------------------------------------*/
    while( frag_offset < datagram_len )
    {
      /*---------------------------------------------------------------------
        Get next fragment.  In case the fragment pointer is NULL free the
        datagram under reassembly as we don't have the complete datagram yet
      ---------------------------------------------------------------------*/
      curr_ptr = ip_fraghdl_get_fragment( handle );
      if( NULL == curr_ptr )
      {
        LOG_MSG_ERROR_0("Got NULL fragment unexpectedly while reassembling");
        dsm_free_packet( &reas_ptr );
        break;
      }

      /*---------------------------------------------------------------------
        Set payload_offset to offset to v4 header for each fragment
      ---------------------------------------------------------------------*/
      payload_offset = local_offset;

      /*---------------------------------------------------------------------
        Parse the fragment header for the current fragment
      ---------------------------------------------------------------------*/
      if( FALSE == ps_ip4_hdr_parse( curr_ptr, &payload_offset, &currhdr ) )
      {
        LOG_MSG_ERROR_0("ps_ip4_hdr_parse() returned FALSE, dropping pkt");
        dsm_free_packet( &curr_ptr );
        continue;
      }

      /*---------------------------------------------------------------------
        If the fragment offset doesn't match the counter of fragment offset
        free the datagram being reassembled and return
      ---------------------------------------------------------------------*/
      if( (curr_offset = ((uint16)currhdr.off*8)) != frag_offset )
      {
        LOG_MSG_INFO2_2("expecting fragment offset %d, got %d.",
                        frag_offset, (currhdr.off*8));
        curr_len = ((uint16)currhdr.length) - ((uint16)currhdr.ihl*4);
        if ((curr_offset + curr_len) <= frag_offset)
        {
          dsm_free_packet( &curr_ptr );
          continue;
        }
        else if (curr_offset < frag_offset)
        {
          /* Partial overlap with reassembled datagram so far */
          payload_offset += frag_offset - curr_offset;
        }
        else
        {
          /* This point should not have been reached */
          LOG_MSG_ERROR_0( "In reassembly with hole in data");
          //IPV4_INC_STATS(reasm_fails,1) ; /* IOE_LONLY */
          dsm_free_packet( &reas_ptr );
          dsm_free_packet( &curr_ptr );
          break;
        }
      }

      /*---------------------------------------------------------------------
        Remove header from fragment
      ---------------------------------------------------------------------*/
      if(payload_offset != dsm_pullup(&curr_ptr, NULL, payload_offset))
      {
        LOG_MSG_ERROR_1("unable to pull %d bytes from fragment.  Dropping fragment",
                        *offset);
        dsm_free_packet(&curr_ptr);
        continue;
      }

      /*---------------------------------------------------------------------
        Set reassembly pointer to current fragment for the first fragment
        otherwise append the dsm item to the reassembly pointer.  Update
        fragment offset counter by the length of payload in fragment.
        Reconstitute header for datagram from first fragment
      ---------------------------------------------------------------------*/
      if(0 == frag_offset)
      {
        reas_ptr = curr_ptr;
        frag_offset = (uint16)dsm_length_packet( reas_ptr );
        currhdr.mf = 0;
        currhdr.length = datagram_len + (currhdr.ihl*4);
        if(FALSE == ps_ip4_hdr_create( &reas_ptr, &currhdr, FALSE ))
        {
          LOG_MSG_ERROR_0("ps_ip4_hdr_create() returned FALSE during reassembly");
          dsm_free_packet( &reas_ptr );
          break;
        }
        pkt_info->ip_hdr.v4 = currhdr;
        *offset = (uint16)(currhdr.ihl*4);
        LOG_MSG_ERROR_0("ps_ip4_hdr_create() reached here");
      }
      else
      {
       frag_offset += (uint16)dsm_length_packet( curr_ptr);
        dsm_append(&reas_ptr, &curr_ptr);
      }
    } /* while */

    /*-----------------------------------------------------------------------
      Free the fraghdl structure only if PDN is not shared.
    -----------------------------------------------------------------------*/    
    if(!is_concurrency_on)
    {
      ip_fraghdl_free( handle );
      pkt_info->fraghdl = NULL;
    }
  }

  /*-------------------------------------------------------------------------
    Return NULL or reconstituted datagram
  -------------------------------------------------------------------------*/
  return reas_ptr;
} /* ip4_reassemble() */

