#ifndef PS_IP4_FRAG_H
#define PS_IP4_FRAG_H
/*===========================================================================

                       P S _ I P 4 _ F R A G . H

DESCRIPTION
  Routines for IPv4 fragmentation and reassembly

Copyright (c) 2004-2020 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/protocols/inet/inc/ps_ip4_frag.h#3 $
  $DateTime: 2021/06/22 06:10:32 $
  $Author: pwbldsvc $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
06/22/21    ds     Code changes to fix double free of meta info ptr in case of
                   UL packet is not sent due to flow disabled.
04/14/20    ds     Code changes for FRAG support in DUN+AP call concurrency.
09/28/18    ds     Code changes for IOT.
06/10/14    cx     Implement ip4_fragment_optimized().
01/07/11    cp     ip4_fragment() API is changed to accept 
                   PATH MTU as input param.
03/13/08    pp     Metainfo optimizations.
10/01/04    ifk    Created module
===========================================================================*/
/*===========================================================================

                                INCLUDE FILES

===========================================================================*/
#include "customer.h"       /* Customer Specific Features */
#include "comdef.h"

#ifdef __cplusplus
extern "C"
{
#endif

#include "dsm.h"
#include "ps_tx_meta_info.h"
#include "ps_ip4_hdr.h"
#include "ps_pkt_info.h"
#include "ds_fwk.h"


/*===========================================================================

                        PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION IP4_FRAGMENT

DESCRIPTION
  This function is called with an IPv4 datagram, meta info associated with
  the datagram and the PS iface to send the datagram over.  It fragments
  and transmits the datagram.

DEPENDENCIES
    None

RETURN VALUE
  -1 in case of error
  0 in case of success

SIDE EFFECTS
  The datagram is transmitted as fragments in case of success.  In case
  of failure the caller is expected to free the datagram and the meta info
===========================================================================*/
int ip4_fragment
(
  dsm_item_type       **ip_pkt_ptr,
  struct ip            *pkt_hdr,
  ds_fwk_s_type        *rt_if_ptr,
  ps_tx_meta_info_type *meta_info_ptr,
  uint32                path_mtu,
  boolean              *is_meta_info_valid
);

/*===========================================================================
FUNCTION IP4_FRAGMENT_OPTIMIZED

DESCRIPTION
  This function is called to fragment one packet into several fragments based 
  on passed in MTU value.All the fragments will be stored in the passed in 
  buffer.The fragments are stored in the queue in case of success.  The passed 
  in hdr will be changed.In case of failure, IP packet should be freed by caller 
  and frag queue will be returned as is.

DEPENDENCIES
  PS_IP4_HDR_PARSE_OPTIMIZED() should be called to parse the IPv4 header 
  before this function is called

RETURN VALUE
  -1 in case of error
  0 in case of success
 
SIDE EFFECTS 
 
  
===========================================================================*/
int ip4_fragment_optimized
(
  dsm_item_type       **ip_pkt_ptr,
  struct ip            *pkt_hdr,
  uint32                path_mtu,
  dsm_item_type       **frag_buf,
  uint16                frag_buf_size
);

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
  struct dsm_item_s *pkt_ptr, /* The fragment itself including IP hdr  	   */
  uint16            *offset,  /* IP payload offset                         */
  boolean           is_concurrency_on /* Boolean for concurrency check     */
);

#ifdef __cplusplus
}
#endif

#endif  /* PS_IP4_FRAG_H */
