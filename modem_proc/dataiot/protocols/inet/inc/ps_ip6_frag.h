#ifndef PS_IP6_FRAG_H
#define PS_IP6_FRAG_H
/*===========================================================================

                           P S _ I P 6 _ F R A G . H

DESCRIPTION
  Functions to fragment an IPv6 datagram

  Copyright (c) 2018 - 2020 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/protocols/inet/inc/ps_ip6_frag.h#2 $
  $DateTime: 2020/04/15 22:43:35 $
  $Author: pwbldsvc $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
04/14/20    ds     Code changes for FRAG support in DUN+AP call concurrency.
09/28/18    ds     Code changes for IOT.
09/05/04    ifk    Created module
===========================================================================*/
/*===========================================================================

                                INCLUDE FILES

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */
#ifdef FEATURE_DATA_PS_IPV6
#include "dsm.h"
#include "queue.h"
#include "ps_ip6_hdr.h"
#include "ps_pkt_info.h"


/*===========================================================================

                        PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION IP6_FRAGMENT

DESCRIPTION
  Function to fragment an IPv6 datagram.  It is passed the maximum
  permissible payload of the fragment header for the associated interface,
  the dsm item containing the datagram, the next header, and a queue in
  which to return the fragments.

DEPENDENCIES
  The passed dsm item contains the part of the datagram which is fragmentable
  The passed queue must have been initialized.

RETURN VALUE
  0 in case of success with the datagrams returned in the passed queue
  -1 in case of error

SIDE EFFECTS
  None
===========================================================================*/
int ip6_fragment
(
  uint32          max_payload,/* Maximum permissible payload of datagram   */
  uint8           next_hdr,   /* Next header after the fragment header     */
  dsm_item_type **pkt_ptr,    /* DSM item containing the datagram payload  */
  q_type         *frag_q      /* Queue to return the fragments in          */
);


/*===========================================================================
FUNCTION IP6_REASSEMBLE()

DESCRIPTION
  Reassemble fragments into a complete datagram.  If all fragments have been
  received to complete the datagram return pointer to DSM item containing the
  datagram with the header returned in the pkt_info argument with the offset
  after the fragment header returned in offset argument.  Otherwise the
  fragment is queued and a NULL returned.

DEPENDENCIES
  None

RETURN VALUE
  If datagram is complete, returns DSM chain containing datagram else
  returns NULL.

SIDE EFFECTS
  Passed fragment may be queued and NULL returned in which case no
  further action should be taken on this fragment by the caller
===========================================================================*/
struct dsm_item_s *ip6_reassemble
(
  dsm_item_type    *pkt_ptr,  /* DSM item containing the fragment          */
  uint16           *offset,   /* IP fragment header offset                 */
  ip_pkt_info_type *pkt_info, /* IPv6 packet information                   */
  boolean           is_concurrency_on /* Boolean for concurrency check     */
);
#endif /* FEATURE_DATA_PS_IPV6 */
#endif  /* PS_IP6_FRAG_H */
