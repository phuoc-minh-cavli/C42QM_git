#ifndef __DS3GPP_EXT_MSG_H__
#define __DS3GPP_EXT_MSG_H__

/*===========================================================================
                      DS_3GPP_EXT_MSG.H
DESCRIPTION
This file exposes 3GPP msgr to external clients outside DS.

Copyright (c) 2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE
when           who    what, where, why
--------    ---    ----------------------------------------------------------
08/09/17    sd      First checkin
===========================================================================*/

#include "dsmsgr.h"



typedef enum
{
  DS_LOW_LATENCY_TRAFFIC_STATE_STOP,
  DS_LOW_LATENCY_TRAFFIC_STATE_START,
  DS_LOW_LATENCY_TRAFFIC_STATE_CONTINUE,
  DS_LOW_LATENCY_TRAFFIC_STATE_MAX 
} ds_low_latency_traffic_state_enum_type;

typedef struct
{
   msgr_hdr_struct_type hdr; /* message header */ 
  /*! Traffic State */  
  ds_low_latency_traffic_state_enum_type   traffic_state;
  
  /*! Filter result */  
  uint32    filter_handle;    
  /*! Expected periodicity of the packet arrivals for this low latency 
    traffic (in ms).
    Value 0xFFFFFFFF - means unknown */
  uint32                     pkt_interval_time_in_ms;
  
  /*! Expected delta time of the packet from its interval time to wake up
    Value 0xFFFFFFFF - means unknown */
  uint32                     pkt_arrival_delta_in_ms;
  /*! Expected time to wait for the packets at every period 
    for this low latency traffic (in ms).
    Value 0xFFFFFFFF - means unknown  */
  uint32                     pkt_max_wait_time_in_ms;
} ds_mgr_latency_info_ext_type;

enum
{ 
  MSGR_DEFINE_UMID(DS, MGR, IND, LOW_LATENCY, 0x24, ds_mgr_latency_info_ext_type),
};



#endif
