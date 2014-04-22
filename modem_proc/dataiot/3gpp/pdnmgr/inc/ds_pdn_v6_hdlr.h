#ifndef DS_PDN_V6_H
#define DS_PDN_V6_H

/*===========================================================================
                      DS_PDN_V6_HDLR.H
DESCRIPTION
This file exposes PDN v6 handler through APIs and data structures.

Copyright (c) 2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/3gpp/pdnmgr/inc/ds_pdn_v6_hdlr.h#1 $
  $DateTime: 2020/03/17 08:03:16 $$Author: pwbldsvc $

when           who    what, where, why
--------    ---    ----------------------------------------------------------
08/09/17    sd      First checkin
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "ds_pdn_defs.h"
#include "ps_ip6_sm.h"


/*===========================================================================

                        PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
 /*---------------------------------------------------------------------------
 TYPEDEF DS_PDN_V6_SM_EVENT_TYPE
 
 DESCRIPTION
   Type and static variable defining events received by the DS_PDN_V6 sm.
 ---------------------------------------------------------------------------*/
 typedef enum
 {
   DS_PDN_V6_LINK_UP_EV       = 2,
   DS_PDN_V6_LINK_DOWN_EV     = 3,
   DS_PDN_V6_LINK_RESYNC_EV   = 4,
   DS_PDN_V6_NET_UP_EV        = 5,
   DS_PDN_V6_NET_DOWN_EV      = 6,
   DS_PDN_V6_PPP_CLOSE_CMD_EV = 7,
   DS_PDN_V6_FLOW_DELETED_EV  = 8,
   DS_PDN_V6_PHY_LINK_DOWN_EV = 9,
   DS_PDN_V6_NET_ABORTED_EV   = 10, 
   DS_PDN_V6_NET_SOL_WAIT_EV   = 11,
   DS_PDN_V6_MAX_EV        
 } ds_pdn_v6_sm_event_type;
 
 /*---------------------------------------------------------------------------
 TYPEDEF ds_pdn_v6_state_type
 
 DESCRIPTION
   Type and static variable defining the state of the network.
 ---------------------------------------------------------------------------*/
 typedef enum
 {
   DS_PDN_V6_CLOSED_STATE                = 0,
   DS_PDN_V6_WAITING_FOR_NET_UP_STATE    = 1,
   DS_PDN_V6_NET_UP_STATE                = 2,
   DS_PDN_V6_WAITING_FOR_NET_DOWN_STATE  = 3,
   DS_PDN_V6_MAX_STATE
 } ds_pdn_v6_state_type;

/*---------------------------------------------------------------------------
STRUCT __DSSNET6_SM_CB_S

DESCRIPTION
  DSSNET6 SM control block.
---------------------------------------------------------------------------*/
struct __ds_pdn_v6_sm_cb_s
{
  ip6_sm_type          *ip6_instance;           /* IPv6 SM instance         */
  ds_pdn_v6_state_type  state;                  /* Current state            */
  ds_pdn_mgr_s         *pdn_mgr_p;           /* associated PDN     */

  struct ps_in6_addr    nv_primary_dns;
  struct ps_in6_addr    nv_secondary_dns;

  boolean pdsn_as_proxy_ipv6_dns_server;       /* TRUE if pdsn_as_proxy_ipv6
                                                  dns_server NV flag is ON */

  void                         *user_data_p;
  ds_net_down_reason_type       call_end_reason;
};


/*===========================================================================
FUNCTION DS_PDN_V6_SM_CREATE()

DESCRIPTION
  This function is is used to initialize the v6 state machine control 
  block. Called by the mode specific handler. The mode specific handler
  allocates memory for the control block as well as the ps iface.

DEPENDENCIES
  None

RETURN VALUE
 -1 on failure
  0 on success

SIDE EFFECTS
  None
===========================================================================*/
int ds_pdn_v6_sm_create
(
   ds_pdn_v6_sm_cb_type  *instance_ptr,       /* Ptr to DSSNET6 SM instance */
   ds_pdn_mgr_s          *pdn_mgr_p,
   ip6_sm_type           *ip6_inst_ptr        /* Ptr to IP6 SM instance     */
);

/*===========================================================================
FUNCTION DS_PDN_V6_PROCESS_LINK_UP_EV()

DESCRIPTION
  This function processes the DSSNET6_LINK_UP_EV. Changes state to 
  DSSNET6I_WAITING_FOR_NET_UP_STATE.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_pdn_v6_process_link_up_ev
(
  ds_pdn_v6_sm_cb_type *instance_ptr            /* dssnet6 instance ptr      */
);

/*===========================================================================
FUNCTION DSSNET6I_PROCESS_LINK_DOWN_EV()

DESCRIPTION
  This function processes the DSSNET6_LINK_DOWN_EV. Changes state to 
  DSSNET6I_CLOSED_STATE.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_pdn_v6_process_link_down_ev
(
  ds_pdn_v6_sm_cb_type    *instance_ptr,            /* dssnet6 instance ptr      */
  ds_net_down_reason_type  down_reason
);

/*===========================================================================
FUNCTION DS_PDN_V6_PROCESS_IP6_STOP_EV()

DESCRIPTION
  This function processes the DS_PDN_V6_NET_ABORTED_EV. Changes state to 
  DS_PDN_V6_CLOSED_STATE.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_pdn_v6_process_ip6_stop_ev
(
  ds_pdn_v6_sm_cb_type      *instance_ptr      /* dssnet6 instance ptr      */
);

#endif /* DS_PDN_V6_H */
