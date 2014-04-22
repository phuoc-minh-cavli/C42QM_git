#ifndef DS_PDN_APN_RATE_CTRL_H
#define DS_PDN_APN_RATE_CTRL_H

/*===========================================================================
                      DS_PDN_APN_RATE_CTRL.H
DESCRIPTION
This file exposes APN rate control through APIs and data structures.

Copyright (c) 2018 - 2019 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/3gpp/pdnmgr/inc/ds_pdn_apn_rate_ctrl.h#2 $
  $DateTime: 2020/03/19 02:52:16 $$Author: pwbldsvc $

when           who    what, where, why
--------    ---    ----------------------------------------------------------
08/09/17    sd      First checkin
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "ds_pdn_defs.h"


#include "ds_pdn_pco_epco_hdlr.h"
#include "ds_pdn_manager.h"
#include "ds_pdn_rate_ctrl_common.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*----------------------------------------------------------------------- 
  APN Rate Control Configuration Information received from PCO
  - Applies to MOX - Represents whether or not APN Rate Control applies
                     to MO Exception.
  - Uplink Time Unit - Represents the time unit that we are restricting
                 the uplink data.
  - Max uplink rate - represents the maximum number of messages that can
                 be transmitted uplink within the Uplink Time Unit.
------------------------------------------------------------------------*/
typedef struct
{
  boolean      applies_to_mo_ex;
  uint8        uplink_time_unit;
  uint32       max_uplink_rate;
}ds_apn_rate_ctrl_config_s;

/*----------------------------------------------------------------------- 
  Additional APN Rate Control Configuration Information received from PCO
    Enabled - Represents whether or not additional rate control is enabled
  - Uplink Time Unit - Represents the time unit that we are restricting
                 the uplink MOX data.
  - Max uplink rate - represents the maximum number of MOX messages that can
                 be transmitted uplink within the Uplink Time Unit.
------------------------------------------------------------------------*/
typedef struct
{
  boolean      enabled;
  uint8        uplink_time_unit;
  uint32       max_uplink_rate;
}ds_additional_apn_rate_ctrl_config_s;


/*----------------------------------------------------------------------- 
  APN Rate Control Information
  - config      - PCO received information related to APN Rate control
  Runtime information
  - apn         - apn name for this entry
  - num_bearers - tracks the number of bearers that are using this
                 same APN name.  Note that this is across all PDNs so
                 you could have an additional PDN with same APN name
                 that would count towards this counter.
  pkt_counter   -- used strictly only for IOCTL 
                   DS_PDN_IOCTL_GET_APN_RATE_CONTROL_INFO
------------------------------------------------------------------------*/
typedef struct
{
  ds_apn_rate_ctrl_config_s               config;
  ds_additional_apn_rate_ctrl_config_s    additional_rate_control_config;
  uint8                                   index;
  byte                                    apn[DS_PROFILE_APN_STRING_LEN+1];
  uint8                                   num_bearers;
  ds_pdn_rate_ctrl_info_s                 rate_ctrl_info;
  ds_pdn_rate_ctrl_info_s                 additional_rate_ctrl_info;
  uint32                                  pkt_counter;
  uint32                                  additional_pkt_counter;
} ds_apn_rate_ctrl_context_s;

/*===========================================================================

                           FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================
FUNCTION ds_pdn_apn_rate_ctrl_get_instance_from_apn

DESCRIPTION
  This function finds the APN Rate Control info context entry that matches a
  particular APN name.
  
PARAMETERS  
  APN name
 
DEPENDENCIES 
  APN passed in must be the resolved APN

RETURN VALUE 
  None
 
SIDE EFFECTS 
  None.
  
===========================================================================*/
ds_apn_rate_ctrl_context_s* ds_pdn_apn_rate_ctrl_get_instance_from_apn
(
  byte                    *apn
);

/*===========================================================================
FUNCTION ds_pdn_apn_rate_ctrl_get_instance_from_apn_index

DESCRIPTION
  This function finds the APN Rate Control info context entry that matches a
  particular APN index
  
PARAMETERS  
  APN Index
 
DEPENDENCIES 
  None.

RETURN VALUE 
  None
 
SIDE EFFECTS 
  None.
  
===========================================================================*/
ds_apn_rate_ctrl_context_s* ds_pdn_apn_rate_ctrl_get_instance_from_apn_index
(
 int       apn_index
);

/*=========================================================================
  FUNCTION ds_pdn_apn_rate_ctrl_process_timer_expiry

  DESCRIPTION
    This function is used to handle any additional processing needed for
    rate control.
  
  PARAMETERS
    APN Rate Control info pointer
                     
  DEPENDENCIES
    None.
  
  RETURN VALUE 
    None.
  
  SIDE EFFECTS 
    None.
  
===========================================================================*/
void ds_pdn_apn_rate_ctrl_process_timer_expiry
(
  ds_pdn_rate_ctrl_info_s    *rate_ctrl_info_ptr
);

/*=========================================================================
  FUNCTION ds_pdn_additional_apn_rate_ctrl_process_timer_expiry

  DESCRIPTION
    This function processes the Additional APN Rate Control Timer Expiry
    and restarts the timer
  
  PARAMETERS
    APN Rate Control info pointer
                     
  DEPENDENCIES
    None.
  
  RETURN VALUE 
    None.
  
  SIDE EFFECTS 
    None.
  
===========================================================================*/
void ds_pdn_additional_apn_rate_ctrl_process_timer_expiry
(
  ds_pdn_rate_ctrl_info_s     *rate_ctrl_info_ptr
);


/*===========================================================================
FUNCTION      DS_PDN_APN_RATE_CTRL_DECODE_PCO_INFO

DESCRIPTION
  This function decodes the APN Rate Control Info.

PARAMETERS
  protocol_pkt_ptr   -  pointer to the PCO contents field
  protocol_pkt_len   -  container length
  pdn_context_p      -  Pointer to current PDN context
  decode_mask        -  Mask pointer containing status of decode operation

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
void ds_pdn_apn_rate_ctrl_decode_pco_info
(
  const byte           *protocol_pkt_ptr,
  byte                  protocol_pkt_len,
  ds_pdn_mgr_s         *pdn_mgr_p,
  uint32               *decode_mask_ptr
);

/*===========================================================================
FUNCTION      DS_PDN_ADDITIONAL_APN_RATE_CTRL_DECODE_PCO_INFO

DESCRIPTION
  This function decodes the Additional APN Rate Control Info.

PARAMETERS
  protocol_pkt_ptr   -  pointer to the PCO contents field
  protocol_pkt_len   -  container length
  pdn_context_p      -  Pointer to current PDN context
  decode_mask        -  Mask pointer containing status of decode operation

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None.

===========================================================================*/

void ds_pdn_additional_apn_rate_ctrl_decode_pco_info
(
  const byte           *protocol_pkt_ptr,
  byte                  protocol_pkt_len,
  ds_pdn_mgr_s         *pdn_mgr_p,
  uint32               *decode_mask_ptr
);

/*===========================================================================
FUNCTION ds_pdn_apn_rate_ctrl_set_mo_ex_in_use

DESCRIPTION
  This function sets the mo exception filters installed/in use flag for
  that APN entry and evaluates whether flow needs to be enabled/disabled.
  
PARAMETERS 
  PDN Context
  MO Exception Usage flag (In use or not in use)
    
DEPENDENCIES 
  PS will only send the first MOX filter installation indication.  If
  PDN installs additional MO Exception filters, we will not get the
  indication. They will only tell us that MO Exception filter was
  removed when the last MO Exception filter is removed for a particular PDN.
 
  In the case of IPV4 and IPV6 PDN has separately installed their MO
  exception filters, we will track in this function the number of filters
  installed using a counter below.  

RETURN VALUE 
  None
 
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_pdn_apn_rate_ctrl_set_mo_ex_in_use
(
  ds_pdn_mgr_s                         *pdn_mgr_p,
  ds_pdn_ioctl_mo_excp_usage_enum_type  mo_excp_usage
);

/*===========================================================================
FUNCTION ds_pdn_apn_rate_ctrl_process_call_end

DESCRIPTION
  This function processes the CALL END command for the give apn index

PARAMETERS
  APN Index

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_apn_rate_ctrl_process_call_end
(
  int                     apn_index
);


/*===========================================================================
FUNCTION ds_pdn_apn_rate_ctrl_enable_flow_for_all_bearers

DESCRIPTION
  This function cycles through the bearer context table, finds all
  bearers that are associated with the same APN, and enables flow. This is
  relevant to several cases:
  1.  APN Rate Control timer has expired, try to enable flow.
  2.  MO Exception filters are installed and APN Rate Control does not apply
      to MO Exception data.
  3.  New APN Rate Control information has been updated through additional
      PDNs that share the same APN name or bearer modification.

PARAMETERS
  APN Rate Control Info Context
  Force Enable (overrides the checks for enabling flow)
 
DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_apn_rate_ctrl_enable_flow_for_all_bearers
(
  ds_apn_rate_ctrl_context_s  *apn_rate_ctrl_info_p,
  uint32                       mask
);

/*===========================================================================
FUNCTION ds_pdn_apn_rate_ctrl_evaluate_flow_for_all_bearers

DESCRIPTION
  This function cycles through the bearer context table, finds all
  bearers that are associated with the same APN on the given subscription,
  and evaluates the flow as applicable.:

 DS_BEARER_APN_RATE_CTRL_MASK : This mask is set when either APN rate control threshold is reached
 with AER bit as 0 OR when both APN rate control and Additional APN rate control thresholds are
 reached

 DS_BEARER_APN_RATE_CTRL_MOX_ONLY_MASK : This mask is set when APN rate control threshold is reached
 with AER bit set to 1 but either Additional APN rate control is not enabled or the threshold is



 not yet reached.

 Note: No action is needed when APN rate control threshold is not reached. 

PARAMETERS
  APN rate control context pointer

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void ds_pdn_apn_rate_ctrl_evaluate_flow_for_all_bearers
(
  ds_apn_rate_ctrl_context_s  *apn_rate_ctrl_info_p
);

/*===========================================================================
FUNCTION ds_pdn_apn_rate_ctrl_assign_apn_index_to_bearer

DESCRIPTION
  This function assigns the matching apn index to the APN to the bearer
  context pointer and increments the number of bearers associated with
  the given APN name

PARAMETERS 
  APN

DEPENDENCIES
  APN passed in must be the resolved APN

RETURN VALUE  
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_apn_rate_ctrl_assign_apn_index_to_bearer
(
  byte         *apn
);

/*===========================================================================
FUNCTION ds_pdn_apn_rate_ctrl_drop_data_packet

DESCRIPTION
  This function decides whether or not DS should drop the data packet or not
  when enqueueing into the bearer's watermark.

PARAMETERS 
  Meta info for the packet 
  Bearer Context Pointer 
  APN context pointer
  Current packet is MO Exception data packet flag
  
  Drop packet only if: 
  1.  APN Rate Control info is valid 
  2.  APN Rate Control threshold is exceeded 
  3.  APN Rate Control does NOT apply to MO exception data 
  4.  Data packet is not mo exception and
      a.  Case IP: MO Exception filters are installed
      b.  Case NONIP

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_apn_rate_ctrl_drop_data_packet
(
  ps_meta_info_type           *meta_info_ptr,
  ds_bearer_mgr_s             *bearer_mgr_p,
  ds_apn_rate_ctrl_context_s  *apn_rate_ctrl_info_ptr,
  boolean                      is_mox_data_pkt
);

/*===========================================================================
FUNCTION ds_pdn_apn_rate_ctrl_allocate_new_apn_context

DESCRIPTION
  This function dynamically allocates a new APN rate control info context
  that stores all of the APN rate control related parameters for a unique APN
  
PARAMETERS  
  APN Rate Control Configuration (as received from network)
  APN name
 
DEPENDENCIES 
  APN passed in must be the resolved APN

RETURN VALUE 
  None
 
SIDE EFFECTS 
  None.
  
===========================================================================*/
ds_apn_rate_ctrl_context_s* ds_pdn_apn_rate_ctrl_allocate_new_apn_context
(
  ds_apn_rate_ctrl_config_s   apn_rate_ctrl_config,
  byte                       *apn
);

/*=========================================================================
  FUNCTION DS_PDN_RATE_CTRL_PROCESS_INTERVAL_TIMER_EXPIRY

  DESCRIPTION
    This function processes the Rate Control Timer Expiry
    and restarts the timer for a given rate control algorithm.
  
  PARAMETERS
                     
  DEPENDENCIES
    None.
  
  RETURN VALUE 
    None.
  
  SIDE EFFECTS 
    None.
  
===========================================================================*/
void ds_pdn_rate_ctrl_process_interval_timer_expiry
(
  void    *payload
);
#endif /* DS_PDN_APN_RATE_CTRL_H */

