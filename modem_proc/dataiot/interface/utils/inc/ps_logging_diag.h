#ifndef PS_LOGGING_DIAG_H
#define PS_LOGGING_DIAG_H
/*===========================================================================

                        P S _ L O G G I N G  _ D I A G . H

GENERAL DESCRIPTION
  The file defines diag sub system commands and regesters the request handler
  callback functions at powerup. This contains diag specific data. 

EXTERNALIZED FUNCTIONS
 dpl_init()
   Registers the functions(dpl request handlers) with DIAG.
   
 Copyright (c) 2004 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

 $Header: //components/rel/dataiot.mpss/2.0/interface/utils/inc/ps_logging_diag.h#1 $
 $Author: pwbldsvc $ $DateTime: 2020/03/17 08:03:16 $
  
when        who     what, where, why
--------    ---     --------------------------------------------------------- 
07/29/18    vd      Created the file. 
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "customer.h"
#include "comdef.h"
#include "customer.h"
#include "ps_logging_defs.h"
#include "ds_fwk.h"
#include "ps_byte.h"
#include "ps_logging.h"


/*===========================================================================
 
                      DIAG REQUEST / RESPONSE STRUCTURES 

===========================================================================*/
/*---------------------------------------------------------------------------
  NOTE : DO NOT compile in these structures. These are for informational
  purposes only.
---------------------------------------------------------------------------*/ 
/*---------------------------------------------------------------------------
struct dpl_reset_logging_req
{
  uint8   command_code;
  uint8   subsys_id;
  uint16  subsys_command_code;
}

struct dpl_reset_logging_resp
{
  uint8   command_code;
  uint8   subsys_id;
  uint16  subsys_command_code;
}

struct dpl_get_supported_ifaces_req
{
  uint8   command_code;
  uint8   subsys_id;
  uint16  subsys_command_code;
}

struct dpl_get_supported_ifaces_resp
{
  uint8   command_code;
  uint8   subsys_id;
  uint16  subsys_command_code;
  uint8   num_ifaces;
  uint8   iface_id[num_ifaces];
}

struct dpl_get_iface_desc_req
{
  uint8   command_code;
  uint8   subsys_id;
  uint16  subsys_command_code;
  uint8   iface_id;
}

struct dpl_get_iface_desc_resp
{
  uint8     command_code;
  uint8     subsys_id;
  uint16    subsys_command_code;
  uint8     iface_id;
  char    * iface_desc; // Can at most be 32 bytes and is null termnated
  uint8     num_links;
  char    * link_desc[num_links]; // Can at most be 32 bytes and is null
                                  // termnated
  uint8     num_flows;
  char    * flow_desc[num_flows]; // Can at most be 32 bytes and is null
                                  // termnated
}

struct dpl_set_iface_logging_req
{
  uint8   command_code;
  uint8   subsys_id;
  uint16  subsys_command_code;
  uint8   iface_id;
  uint8   num_log_flags;

  struct
  {
    uint32  iid;
    uint32  snaplen;
  } log_flag[num_log_flags];
}

struct dpl_set_iface_logging_resp
{
  uint8   command_code;
  uint8   subsys_id;
  uint16  subsys_command_code;
  uint8   iface_id;
  uint8   num_log_flags;

  struct
  {
    uint32  iid;
    uint32  link_type;
  } log_flag[num_log_flags];
}

struct dpl_get_supported_protocols_req
{
  uint8   command_code;
  uint8   subsys_id;
  uint16  subsys_command_code;
}

struct dpl_get_supported_protocols_resp
{
  uint8   command_code;
  uint8   subsys_id;
  uint16  subsys_command_code;
  uintt8  num_nw_protocols; // Number of supported network level protocols
  uint8   protocol[num_nw_protocols];                
}
---------------------------------------------------------------------------*/


/*===========================================================================
 
                                  EXTERNAL FUNCTIONS

===========================================================================*/

/*===========================================================================
FUNCTION      DPL_INIT

DESCRIPTION
  Registers functions with DIAG.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void dpl_init
(
  void
);

/*===========================================================================
FUNCTION      DPLI_LOG_IID_DESC_AND_STATUS

DESCRIPTION
  Logs a packet with iface status(UP/DOWN) and iid description

PARAMETERS

  this_iface_ptr      : logged this iface description
  bridged_iface_ptr : bridged iface ptr(will be used in case of RM ifaces) 

RETURN VALUE
  None

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void dpli_log_iid_desc_and_status
(
  ds_fwk_s_type                *fw_inst,
  ip_addr_enum_type             family,  
  ds_dpl_client_type            client_id,
  char                         *apn
);

/*===========================================================================
FUNCTION DPL_GET_DPL_STATE
 
DESCRIPTION
  This method will return current DPL state
   
RETURN VALUE
     None
 
DEPENDENCIES
  	 None 
  
SIDE EFFECTS
    None
===========================================================================*/

dpl_state_enum_type dpl_get_dpl_state
(
  void
);
 
/*===========================================================================
FUNCTION dpli_update_logging_config

DESCRIPTION
  This method enables the logging in iface, sets protocol and packet
  log length
 
PARAMETERS
  dpl_state[in]              dpl_state 
   
RETURN VALUE
    None
 
DEPENDENCIES
    None 
  
SIDE EFFECTS
    None
===========================================================================*/
 void dpli_update_logging_config
(
  dpl_state_enum_type  dpl_state
);

/*===========================================================================
FUNCTION DPL_GET_NET_DIR_CB
 
DESCRIPTION
  This method will return current DPL state
   
RETURN VALUE
     None
 
DEPENDENCIES
  	 None 
  
SIDE EFFECTS
    None
===========================================================================*/

dpl_net_dir_cb_type *dpl_get_net_dir_cb
(
  void
);

#ifdef __cplusplus
}
#endif

#endif /* PS_LOGGING_DIAG_H */
