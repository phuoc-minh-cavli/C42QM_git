#ifndef PS_LOGGING_H
#define PS_LOGGING_H
/*===========================================================================

                        P S _ L O G G I N G . H

GENERAL DESCRIPTION
  This file defines external API used by various protocols to log their
  packets

EXTERNAL FUNCTIONS
  DPL_LOG_NETWORK_TX_PACKET()
    Used to log a network packet on Tx side

  DPL_LOG_NETWORK_RX_PACKET()
    Used to log a network packet on Rx side

  DPL_LOG_FLOW_TX_PACKET()
    Used to log a flow packet on Tx side

  DPL_LOG_LINK_FULL_TX_PACKET()
    Used to log a full link level packet on Tx side

  DPL_LOG_LINK_FULL_RX_PACKET()
    Used to log a full link level packet on Rx side

Copyright (c) 2004-2019 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

 $Header: //components/rel/dataiot.mpss/2.0/interface/utils/inc/ps_logging.h#1 $
 $Author: pwbldsvc $ $DateTime: 2020/03/17 08:03:16 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
12/23/09    cp     Lint fixes.
06/05/06    mp     Moved DPL link logging control block from ps_iface to
                   ps_phys_link and added DPL PPP Partial Logging
08/15/05    msr    Removed FEATURE_DATA_PS_DATA_LOGGING_FLOW and using
                   ps_flow instead of ps_phys_link for flow logging
01/12/05    msr    Added code review comments.
01/10/05    msr    Added support for flow and link logging.
10/31/04   msr/ks  Changed the inline functions to macros.
09/24/04   ks/msr  Created module.
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "customer.h"
#include "comdef.h"
#include "customer.h"
#include "dsm.h"
#include "log.h"
#include "err.h"
#include "log_codes.h"
#include "diagdiag.h"
#include "ps_byte.h"
#include "ps_logging_defs.h"

/*===========================================================================

                               CONSTANTS

===========================================================================*/
/*---------------------------------------------------------------------------
  Size of header added by DIAG subsytem to each log packet
---------------------------------------------------------------------------*/
#define LOG_HDR_SIZE  sizeof(log_hdr_type)

/*---------------------------------------------------------------------------
  Size of header added by PS to each log packet
---------------------------------------------------------------------------*/
#define DPL_LOG_HDR_SIZE  8

/*---------------------------------------------------------------------------
  Total size of all headers, visible to PS, that are included in log packet
---------------------------------------------------------------------------*/
#define DPL_TOTAL_HDR_SIZE  (LOG_HDR_SIZE + DPL_LOG_HDR_SIZE)

/*---------------------------------------------------------------------------
Iface description buffer is 64 Bytes long to accomodate 32 byte long iface description and 
32 bytes for APN name(optimistic length)
---------------------------------------------------------------------------*/
#define  MAX_DPL_DESC_LEN  64

/*---------------------------------------------------------------------------
  Maximum data bytes from the packet to be logged that can be carried
  in each log packet
---------------------------------------------------------------------------*/
#define MAX_PAYLOAD  256


/*===========================================================================

                          HELPER FUNCTIONS

===========================================================================*/

/*dpl client */
typedef enum 
{
  DS_DPL_CLIENT_RM,
  DS_DPL_CLIENT_UM
}ds_dpl_client_type;
  

/*stats client */
typedef enum 
{
  DS_STATS_CLIENT_RM,
  DS_STATS_CLIENT_UM
}ds_stats_client_type;
  

/*===========================================================================
FUNCTION    DPLI_LOG_FULL_PACKET

DESCRIPTION
  Logs a full packet, potentially splitting it to multiple segments

PARAMETERS
  item_ptr      : pointer to the dsm chain to be logged
  len           : length of the packet to be logged
  iid           : IID of the data log point
  frame_seq_num : sequence number for the packet being logged

RETURN VALUE
  TRUE  : packet was logged
  FALSE : packet was not logged

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void dpli_log_full_packet
(
  dsm_item_type  * item_ptr,
  uint32           len,
  dpl_iid_type     iid,
  uint16           frame_seq_num
);

  

/*===========================================================================
FUNCTION    DPLI_LOG_PACKET

DESCRIPTION
  Logs a full packet, potentially splitting it to multiple segments

PARAMETERS
  if_dpl_cb      : pointer to the dpl cb
  direction           : direction of the packet to be logged
 item_ptr      : pointer to the dsm chain to be logged
 protocol  : protocol of the packet to logg

RETURN VALUE
  None

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void dpli_log_packet
(
  dpl_net_cb                       *if_dpl_cb,
  boolean                           direction,
  dsm_item_type                    *packet,
  dpl_iid_netprot_enum_type         protocol
);

void dpl_log_packet
(
  dpl_net_cb                       *if_dpl_cb,
  boolean                           direction,
  dsm_item_type                    *packet,
  dpl_iid_netprot_enum_type         protocol
);


/*===========================================================================
FUNCTION    DPLI_GET_IP_HEADER_LENGTH

DESCRIPTION
  Provides the length of the IP header of the input packet

PARAMETERS
  item_ptr          : pointer to the dsm chain

RETURN VALUE
  header_length     : length of the IP header in the packet

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
uint32 dpli_get_ip_header_length
(
  dsm_item_type  * item_ptr
);

/*===========================================================================
MACRO    DPLI_BUILD_LOG_PACKET

DESCRIPTION
  Constructs the log packet by filling in header and data information.

PARAMETERS
  item_ptr_ptr  : pointer to the dsm chain to extract data from
  log_ptr       : Pointer to the log packet
  len_ptr       : Number of bytes to be extracted
  iid           : IID associated with this log packet
  frame_seq_num : Sequence number for this log packet
  seg_num       : Segment number for this log packet
  offset_ptr    : Number of bytes to offset into packet

RETURN VALUE
  NONE
===========================================================================*/
#define DPLI_BUILD_LOG_PACKET(item_ptr_ptr, log_ptr, offset_ptr,         \
                              iid, len_ptr, frame_seq_num, seg_num)      \
  /*lint -save -e740 IID is a 4 byte value but represented in a struct */\
  /*casting to void* to take care of type-punned compiler warning      */\
  log_ptr = put32(log_ptr, *(uint32 *)((void *) &(iid)));                \
  /*lint -restore */                                                     \
  log_ptr = hput16(log_ptr, frame_seq_num);                              \
  log_ptr = hput16(log_ptr, seg_num);                                    \
                                                                         \
  (void) dsm_seek_extract(item_ptr_ptr, offset_ptr, log_ptr, len_ptr);   \
 *offset_ptr += *len_ptr;


/*===========================================================================

                                 EXTERNAL FUNCTIONS

===========================================================================*/
/*===========================================================================
MACRO   DPL_LOG_NETWORK_TX_PACKET

DESCRIPTION
  Log a network protocol packet going on tx side

PARAMETERS
  if_dpl_cb : pointer to dpl cb
  tx_packet    : packet to be logged
  protocol     : network protocol of packet to be logged

RETURN VALUE
   None
===========================================================================*/
#define DPL_LOG_NETWORK_TX_PACKET(if_dpl_cb, tx_packet, protocol)     \
           dpl_log_packet(if_dpl_cb, FALSE,tx_packet,DPL_IID_NETPROT_IP) \

/*===========================================================================
MACRO   DPL_LOG_NETWORK_RX_PACKET

DESCRIPTION
  Log a network protocol packet going on rx side

PARAMETERS
  if_dpl_cb : pointer to dpl cb
  rx_packet    : packet to be logged
  protocol     : network protocol of packet to be logged

RETURN VALUE
  None
===========================================================================*/
#define DPL_LOG_NETWORK_RX_PACKET(if_dpl_cb,rx_packet, protocol)     \
          dpl_log_packet(if_dpl_cb, TRUE,rx_packet,DPL_IID_NETPROT_IP) \

/*===========================================================================
MACRO   DPL_LOG_NETWORK_TX_DROPPED_PACKET

DESCRIPTION
  Log a network protocol dropped packet going on tx side

PARAMETERS
  ps_iface_ptr : pointer to ps_iface
  tx_packet    : packet to be logged
  protocol     : network protocol of packet to be logged

RETURN VALUE
   None
===========================================================================*/
#define DPL_LOG_NETWORK_TX_DROPPED_PACKET(if_dpl_cb, tx_packet, protocol)     \
{                                                                               \
    if_dpl_cb->dpl_id.dpl_flags != DPL_IID_DROPPED_MASK;                        \
       dpl_log_packet(if_dpl_cb, FALSE,tx_packet,DPL_IID_NETPROT_IP)            \
    if_dpl_cb->dpl_id.dpl_flags &= ~(DPL_IID_DROPPED_MASK);                     \
}


/*===========================================================================
MACRO   DPL_LOG_NETWORK_RX_DROPPED_PACKET

DESCRIPTION
  Log a network protocol dropped packet going on rx side

PARAMETERS
  ps_iface_ptr : pointer to ps_iface
  rx_packet    : packet to be logged
  protocol     : network protocol of packet to be logged

RETURN VALUE
  None
===========================================================================*/
#define DPL_LOG_NETWORK_RX_DROPPED_PACKET(if_dpl_cb, rx_packet, protocol)     \
{                                                                               \
    if_dpl_cb->dpl_id.dpl_flags != DPL_IID_DROPPED_MASK;                        \
      dpl_log_packet(if_dpl_cb, TRUE,rx_packet,DPL_IID_NETPROT_IP)              \
    if_dpl_cb->dpl_id.dpl_flags &= ~(DPL_IID_DROPPED_MASK);                     \
}

#endif /* PS_LOGGING_HELPER_H */

