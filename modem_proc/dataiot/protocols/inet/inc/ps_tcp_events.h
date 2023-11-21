#ifndef _PS_TCP_EVENTS_H
#define _PS_TCP_EVENTS_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                         P S _ T C P _ E V E N T . H

GENERAL DESCRIPTION
  This header file contains the definitions of the enums that are used to
  generate TCP events.



    Copyright (c) 2018 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                            EDIT HISTORY FOR FILE
                                      
  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_tcp_events.h_v   1.1   27 Feb 2003 15:41:54   jysong  $
  $Header: //components/rel/dataiot.mpss/2.0/protocols/inet/inc/ps_tcp_events.h#1 $ $DateTime: 2020/03/17 08:03:16 $ $Author: pwbldsvc $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/28/18    ds     Code changes for IOT.
03/07/03    js     Added include files and other minor changes. 
02/27/03    js     initial revision.
===========================================================================*/
#include "comdef.h"

#include "event.h"
#include "event_defs.h"

/*===========================================================================
  NOTE: changes to these enums MUST be coordinated with the diag/tools team.
        as of the creation date mail had to be sent to asw.diag.request
===========================================================================*/
/*---------------------------------------------------------------------------
TYPEDEF TCP_EVENT_STATE_TYPE

DESCRIPTION
  enum that defines the states of the protocol for event repporting.  
---------------------------------------------------------------------------*/
typedef enum
{
     EVENT_TCP_OPENING,
     EVENT_TCP_OPEN,
     EVENT_TCP_CLOSING,
     EVENT_TCP_CLOSED
} tcp_event_state_type;

/*--------------------------------------------------------------------------
TYPEDEF TCP_EVENT_PAYLOAD_TYPE

DESCRIPTION
    TCP Event Payload Type
--------------------------------------------------------------------------*/
typedef struct
{
   int sock_fd;
   tcp_event_state_type tcp_event_state;
}tcp_event_payload_type;

#endif /*  _PS_TCP_EVENTS_H */
