/*===========================================================================

                   P S _ L O G G I N G _ H E L P E R . C

GENERAL DESCRIPTION
  This file defines helper routines to log data

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2004-2010,2015 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

 $Header: //components/rel/dataiot.mpss/2.0/interface/utils/src/ps_logging_helper.c#1 $
 $Author: pwbldsvc $ $DateTime: 2020/03/17 08:03:16 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
07/09/10    sy     Fixed type-punned compiler warnings.
10/24/08    pp     Fixed compile warning observed in OFFTARGET build.
08/18/06    mjp    Fixed IPHC and ROHC Logging
08/31/06    mjp    Added IPHC and ROHC Logging
07/17/06    mp     Fixed logging of zero length DPL packets
06/05/06    mp     Added DPL PPP Partial Logging
05/12/05    ks     Fixed Lint errors.
01/12/05    msr    Added code review comments.
01/10/05    msr    Added support for flow and link logging.
11/15/04   ks/msr  Sending values to DPLI_BUILD_LOG_PACKET. Fixed a bug
                   where DSM is checked to see if it is corrupted.
10/31/04   ks/msr  Created module.
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "comdef.h"
#include "customer.h"
#include "dsm.h"
#include "ps_svc.h"
#include "ps_logging.h"
#include "custdatamodem.h"
#include "amssassert.h"
#include "data_msg.h"
#include "ps_logging_diag.h"

#ifdef PS_LOGGING_IOE_DPL
/*===========================================================================
                            LOCAL DEFINITIONS
===========================================================================*/
/* Used to set final bit in logging packet */
#define DPLI_SEGNUM_FINAL_BIT  0x8000

/* 0xFFFFFFFF is used to indicate that a data item failed to allocate*/
#define DPL_LOG_ALLOC_FAILED   (0xFFFFFFFF)

/* Used to represent short negative one so overflow sets num to zero when
          incremented */
#define ONE_LESS_THAN_ZERO     0xFFFF



/*===========================================================================
MACRO    DPLI_BUILD_LOG_PACKET_HDR

DESCRIPTION
  Constructs the log packet header.

PARAMETERS
  log_ptr       : Pointer to the log packet
  iid           : IID associated with this log packet
  frame_seq_num : Sequence number for this log packet
  seg_num       : Segment number for this log packet

SIDE EFFECTS
  log_ptr is advanced past the constructed dpl log header

RETURN VALUE
  NONE
===========================================================================*/
#define DPLI_BUILD_LOG_PACKET_HDR(log_ptr, iid, frame_seq_num, seg_num)    \
  /*casting to void* to take care of type-punned compiler warning    */\
  log_ptr = put32(log_ptr, *(uint32 *)((void *) &(iid)));              \
  log_ptr = hput16(log_ptr, frame_seq_num);                            \
  log_ptr = hput16(log_ptr, seg_num);



/*===========================================================================

                          INTERNAL FUNCTION DEFINITONS

===========================================================================*/
/*===========================================================================
FUNCTION    DPLI_LOG_FULL_PACKET

DESCRIPTION
  Logs a packet by potentially splitting it to multiple segments

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
)
{
  uint8   * log_ptr;           /* pointer to log buffer                    */
  uint8   * return_ptr;
  uint16    offset;            /* byte offset into DSM chain               */
  uint16    seg_num;           /* segment number sent in each logged frame */
  uint16    payload_len;       /* # of bytes sent in log packet            */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  offset      = 0;
  seg_num     = 0;

  /*-------------------------------------------------------------------------
    Although dsm_seek_extract() modifies this value when we call
    DPLI_BUILD_LOG_PACKET(), value is reset to MAX_PAYLOAD when
    dsm_seek_extract() succeeds. So there is no need to reset the value
    before each call to DLI_BUILD_LOG_PACKET().
  -------------------------------------------------------------------------*/
  payload_len = MAX_PAYLOAD;

  /*-------------------------------------------------------------------------
    Log full-length segments while outstanding payload is > maximum log
    frame length.

    NOTE: Loop is unrolled in to two parts to optimize this function. So if
          body of this loop changes, change other part also.
  -------------------------------------------------------------------------*/
  while (len > MAX_PAYLOAD)
  {
    log_ptr = (uint8 *) log_alloc(LOG_DATA_PROTOCOL_LOGGING_C,
                                  MAX_PAYLOAD + DPL_TOTAL_HDR_SIZE);

    if (NULL == log_ptr)
    {
      DS_MSG0(MSG_LEGACY_MED,"dpli_log_full_packet(): "
                      "log_alloc failed");
      return;
    }

    /*-----------------------------------------------------------------------
      Have to reset max_payload every time as DPLI_BUILDLOG_PACKET changes
      its value
    -----------------------------------------------------------------------*/
    return_ptr =  log_ptr;
    log_ptr    += LOG_HDR_SIZE;

    DPLI_BUILD_LOG_PACKET(&item_ptr,
                          log_ptr,
                          &offset,
                          iid,
                          &payload_len,
                          frame_seq_num,
                          seg_num++);

    if (MAX_PAYLOAD != payload_len)
    {
      DS_MSG1(MSG_LEGACY_ERROR,"dpli_log_full_packet(): "
                      "DSM chain is corrupted, 0x%p", item_ptr);
      return;
    }

    log_commit (return_ptr);

    /*-----------------------------------------------------------------------
      Check for segment number wrap around
    -----------------------------------------------------------------------*/
    seg_num &= 0x7FFF;
    len     -= MAX_PAYLOAD;
  }

  /*-------------------------------------------------------------------------
    This part builds the log packet when len becomes < = MAX_PAYLOAD.
    len can never be zero as the condition used in above loop is
    "len > MAX_PAYLOAD". So here "0 < len <= MAXPAYLOAD".
  -------------------------------------------------------------------------*/
  log_ptr = (uint8 *) log_alloc(LOG_DATA_PROTOCOL_LOGGING_C,
                                len + DPL_TOTAL_HDR_SIZE);

  if (log_ptr == NULL)
  {
    DS_MSG0(MSG_LEGACY_MED,"dpli_log_full_packet(): "
                    "log_alloc failed");                     
    return;
  }

  /*-------------------------------------------------------------------------
    As this is the last segment, set the F bit (most significant bit) of
    the segment number to 1
  -------------------------------------------------------------------------*/
  seg_num     |= DPLI_SEGNUM_FINAL_BIT;
  return_ptr  =  log_ptr;
  log_ptr     += LOG_HDR_SIZE;
  /**/
  payload_len = (uint16)len;
  /**/

  DPLI_BUILD_LOG_PACKET(&item_ptr,
                        log_ptr,
                        &offset,
                        iid,
                        &payload_len,
                        frame_seq_num,
                        seg_num);

  if (len != payload_len)
  {
    DS_MSG1(MSG_LEGACY_ERROR,"dpli_log_full_packet(): "
                    "DSM chain is corrupted, 0x%p", item_ptr);
    log_free(return_ptr);
    return;
  }

  log_commit (return_ptr);

  return;

} /* dpli_log_full_packet() */

void dpl_log_packet
(
  dpl_net_cb                       *if_dpl_cb,
  boolean                           direction,
  dsm_item_type                    *packet,
  dpl_iid_netprot_enum_type         protocol
)
{
  uint16                            *seq_num;
  dpl_iid_type                      net_dpl_id ;
  dpl_net_dir_cb_type              *global_cfg =NULL;
  uint32                            pkt_len;
/*--------------------------------------------------------*/
  net_dpl_id.dpl_flags =0;
  if(direction == TRUE)// Means Rx side
  {
    seq_num = &(if_dpl_cb->rx_seq_num[protocol]);
    net_dpl_id.dpl_flags &= (~(DPL_IID_DIRECTION_MASK));
  }  
  else
  {
    seq_num = &(if_dpl_cb->tx_seq_num[protocol]);
    net_dpl_id.dpl_flags |= DPL_IID_DIRECTION_MASK;
  }

  global_cfg = dpl_get_net_dir_cb();
  if(global_cfg != NULL)
  {
    if (TRUE  == global_cfg->is_logging_enabled &&
         0 != (global_cfg->mask & (0x1 << (protocol - 1))))
      {         
       /*-------------------------------------------------------------------
        Only allocate IID variable if logging is turned on, to impact      
        less on data path in the common case (logging disabled).           
      -------------------------------------------------------------------*/
                                                                             
        if (NULL == packet)                                                  
        {                                                                    
          DS_MSG0(MSG_LEGACY_ERROR,"NULL item ptr");                                  
        }                                                                    
        else                                                                 
        {                                                                    
          /*-----------------------------------------------------------------
            IID is passed at beginning of each DPL log packet. Only protocol 
            field needs to be filled here. Rest of the IID is populated when 
            ps_iface is created in ps_iface_create()                         
          -----------------------------------------------------------------*/
          net_dpl_id.ifname          = if_dpl_cb->dpl_id.ifname;                                      
          net_dpl_id.protocol = protocol;                                       
                                                                             
          /*-----------------------------------------------------------------
            If snaplen is SNAPLEN_ALL log whole packet else log snaplen      \
            number of bytes. By doing this partial logging is supported      \
          -----------------------------------------------------------------*/
          if (SNAPLEN_ALL == global_cfg->snaplen[protocol])                 
          {     
          /*---------------------------------------------------------------
            If dpl state is DPL_ENABLED_IP_HEADER, then get the length of  
            the IP header form the packet                                  
        ---------------------------------------------------------------*/
            if (DPL_ENABLED_IP_HEADER == dpl_get_dpl_state())                          
            {                                                                
                pkt_len = dpli_get_ip_header_length(packet);                   
            }                                                                
            else                                                             
            {                                                                
               pkt_len = dsm_length_packet(packet);                           
            } 
            dpli_log_full_packet(packet,                                     
                                 pkt_len,                  
                                 net_dpl_id,                                 
                                 (*seq_num)++);          
          }                                                                  
          else if (0 == global_cfg->snaplen[protocol])                      
          {                                                                  
            dpli_log_full_packet(packet,                                     
                                 dsm_length_packet(packet),                  
                                 net_dpl_id,                                 
                                (*seq_num)++);          
          }                                                                  
          else                                                               
          {                                                                  
            pkt_len = dsm_length_packet(packet);                     
            pkt_len = MIN(global_cfg->snaplen[protocol], pkt_len);          
            dpli_log_full_packet(packet,                                     
                                 pkt_len,                                    
                                 net_dpl_id,                                 
                                (*seq_num)++);          
          }                                                                  
         
        }                                                                    
      }
  }
  else
  {
    DS_MSG0(MSG_LEGACY_ERROR,"global_cfg 0");
  }

}
#endif
