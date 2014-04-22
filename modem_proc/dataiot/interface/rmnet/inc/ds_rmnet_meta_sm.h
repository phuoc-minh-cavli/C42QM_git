#ifndef _DS_RMNET_META_SM_H
#define _DS_RMNET_META_SM_H
/*===========================================================================

                     D S _ R M N E T _ M E T A _ S M . H
DESCRIPTION

  The Data Services WWAN Rm State Machine header file.

  This state machine manages the Rm network interface and coordinates
  wireless WAN connectivity between the Rm and Um interfaces over all
  supported radio technologies.

Copyright (c) 2020 Qualcomm Technologies Incorporated.
All Rights Reserved.

Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE
  $Header: //components/rel/dataiot.mpss/2.0/interface/rmnet/inc/ds_rmnet_meta_sm.h#2 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------

===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "ds_headers.h"
#include "ds_rmnet_defs.h"
#include "ds_fwk.h"
#include "sio.h"

/*===========================================================================
                         REGIONAL DATA DECLARATIONS

===========================================================================*/

#define RMNET_IP_MAX 5 
/*---------------------------------------------------------------------------
  Net-RMSM instance enumeration.
----------------------------------------------------------------------------*/  
typedef enum
{
  RMNET_INSTANCE_MIN      =  0,
  RMNET_INSTANCE_MAX      =  (RMNET_IP_MAX * 3)
}rmnet_instance_e_type;


/*---------------------------------------------------------------------------
  Declaration of the events of the state machine.
---------------------------------------------------------------------------*/
typedef enum
{
  RMNET_META_SM_RM_DEV_START_PKT_EV  = 0,       /* Rm wants packet call         */
  RMNET_META_SM_RM_LINK_DOWN_EV      = 1,       /* Rm interface is down         */
  RMNET_META_SM_PDN_DOWN_EV          = 2,       /* Rm interface is down         */
  RMNET_META_SM_PDN_UP_EV            = 3,       /* Rm interface is up           */
  RMNET_META_SM_ADDR_CFG_CHANGED_EV  = 4,       /* Rm address changed           */
  RMNET_META_SM_EP_CFG_DONE          = 5,
  RMNET_META_SM_MAX_EV
} rmnet_meta_sm_event_type;

/*---------------------------------------------------------------------------
  Enum type indicating the underlying link protocol used by RmNet during the
  call.
---------------------------------------------------------------------------*/
typedef enum
{
  RMNET_INVALID_MODE  = 0x0,
  RMNET_ETHERNET_MODE = 0x1,
  RMNET_IP_MODE       = 0x2,
  RMNET_ALL_MODE      = 0x3
} rmnet_sm_link_prot_e_type;

/*---------------------------------------------------------------------------
  QoS header format
---------------------------------------------------------------------------*/
typedef enum
{
  RMNET_QOS_FORMAT_NONE   = 0,
  RMNET_QOS_FORMAT_6_BYTE = 1,
  RMNET_QOS_FORMAT_8_BYTE = 2
} rmnet_qos_format_e_type;

typedef enum
{
  PS_RMNET_QOS_DISABLED = 0,
  PS_RMNET_QOS_ENABLED  = 1
} ps_rmnet_qos_data_format;


typedef enum
{
  //0x0 is reserved for local (modem)
  //0x1 is reserved for RmSm
  RMNET_PROC_ID_APPS1   = 0x02,      /* AProc call    */
  RMNET_PROC_ID_LAPTOP1 = 0x03,      /* TE-RmNet call */
  RMNET_PROC_ID_MAX     = 0x7FFFFFFF
} rmnet_meta_sm_proc_id_e_type;

/*---------------------------------------------------------------------------
  enum for RmNet data aggregation
---------------------------------------------------------------------------*/
typedef enum
{
  RMNET_DISABLE_DATA_AGG           = 0,
  RMNET_ENABLE_DATA_AGG_TLP        = 1,
  RMNET_ENABLE_DATA_AGG_QC_NCM     = 2,
  RMNET_ENABLE_DATA_AGG_MBIM       = 3,
  RMNET_ENABLE_DATA_AGG_RNDIS      = 4,
  RMNET_ENABLE_DATA_AGG_QMAP       = 5,
  RMNET_ENABLE_DATA_AGG_QMAP_V2    = 6,
  RMNET_ENABLE_DATA_AGG_QMAP_V3    = 7,
  RMNET_DATA_AGG_MAX               = 0x7FFFFFFF
} rmnet_data_agg_enum_type;   //Check the enums can be removed

/*---------------------------------------------------------------------------
  Data formats
---------------------------------------------------------------------------*/
typedef struct
{
  rmnet_set_data_fmt_e_type   data_format_client;
  rmnet_sm_link_prot_e_type   link_prot;
  uint8                       qos;
  rmnet_qos_format_e_type     qos_format;
  boolean                     mbim_sw_deagg;  // MBIM SW UL de-aggregation  //Remove
  rmnet_data_agg_enum_type    ul_data_agg_protocol;
  rmnet_data_agg_enum_type    dl_data_agg_protocol;
  uint32                      ndp_signature;   //Remove 
  uint32                      ncm_signature;  //remove
  uint32                      dl_data_agg_max_num;
  uint32                      dl_data_agg_max_size;
  uint32                      ul_data_agg_max_num;
  uint32                      ul_data_agg_max_size;
  uint32                      dl_min_padding;
  boolean                     te_flow_control;
} rmnet_data_format_type;

typedef struct
{
  boolean in_band_fc;
} rmnet_qmap_settings_type;

typedef enum
{
  RMNET_CALL_TYPE_LAPTOP   = 0x00,
  RMNET_CALL_TYPE_EMBEDDED = 0x01,
  RMNET_CALL_TYPE_MAX
} rmnet_meta_sm_call_e_type;

/*---------------------------------------------------------------------------
  Struct defintion to pass SNI info to rmnet_meta_sm
  consists of override parameters + tech preference, profile numbers
  rmnet_meta_sm stores this info and uses it to pass relevant info in
  route_get() and bring_up().
---------------------------------------------------------------------------*/
typedef struct
{
  uint8                      ip_family_pref;
  rmnet_meta_sm_call_e_type  call_type;
  uint8                      apn_type;
  boolean                    mo_excp_data_cap;
  boolean                    auth_param_valid;
  ds_profile_auth_type       auth_params;
  struct
  {
    uint8 length;
    char name[255];
  } apn;
} rmnet_meta_sm_network_info_type;


/*===========================================================================
  DESCRIPTION
    function pointer to register for pdn events

  PARAMETERS
    client_handle : handle associated with the pdn.
    event: pdn event.
    event-info: event payload
    user_data_ptr: user data passed as part of registration

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

typedef void (*rmnet_wds_event_cback_type)
(
  ds_fwk_clnt_handle_type    client_handle,
  ds_pdn_event_enum_type     event,
  ds_pdn_event_info_u_type   event_info,
  void                      *user_data_ptr
);

/*===========================================================================
  Event buffer to store clients callback and user data that needs to be passed 
  as part of events notification
===========================================================================*/

typedef struct
{
  rmnet_wds_event_cback_type    event_cback_f_ptr;
  void                              *user_data_ptr;
} rmnet_wds_event_buf_type;


/*--------------------------------------------------------------------------
  EFS Global Config parameters.
---------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint32   ul_agg_prot_mask;   /* UL global config  */
  uint32   dl_agg_prot_mask;   /* DL global config */
  boolean  eth_mode_enabled;   /* Ethernet mode */     //Remove
  uint8    reserved[31];
} rmnet_efs_global_config_s_type;       //Not using remove it

#define RMNET_GLOBAL_EFS_DATA_FORMAT_DISABLE_MASK    (0x00000000)
#define RMNET_GLOBAL_EFS_DATA_FORMAT_TLP_MASK        (0x00000001)
#define RMNET_GLOBAL_EFS_DATA_FORMAT_QC_NCM_MASK     (0x00000002)
#define RMNET_GLOBAL_EFS_DATA_FORMAT_MBIM_MASK       (0x00000004)
#define RMNET_GLOBAL_EFS_DATA_FORMAT_RNDIS_MASK      (0x00000008)
#define RMNET_GLOBAL_EFS_DATA_FORMAT_QMAP_MASK       (0x00000010)
#define RMNET_GLOBAL_EFS_DATA_FORMAT_QMAP_V2_MASK    (0x00000020)
#define RMNET_GLOBAL_EFS_DATA_FORMAT_QMAP_V3_MASK    (0x00000040)

#define MAX_MUX_ID_POOL_SIZE         5
#define TETHERED_CALL_MUXID          129

#ifdef FEATURE_DATA_QCRMCALL
/*---------------------------------------------------------------------------
  Definitions for $QCRMCALL
---------------------------------------------------------------------------*/
#define RMNET_QCRMCALL_MAX  (8)

typedef enum
{
  QCRMCALL_ACTION_NONE   = 0,
  QCRMCALL_ACTION_START  = 1,
  QCRMCALL_ACTION_STOP   = 2,

  QCRMCALL_ACTION_QUERY  = 3,
} rmnet_qcrmcall_action_e_type;

/* qcrmcall_inst: starts from 0. IP Family: 4 or 6. Status = TRUE: success */
typedef union
{
  // START
  struct
  {
    uint8                             qcrmcall_inst;
    rmnet_meta_sm_network_info_type * sni_params;  
  } start;

  // STOP
  struct
  {
    uint8  qcrmcall_inst;
    uint8  ip_family;
  } stop;

} rmnet_qcrmcall_request_type;

typedef union
{
  // START and STOP
  struct
  {
    uint8      qcrmcall_inst;
    uint8      ip_family;
    boolean    status;
  } sni;

  // QUERY
  struct
  {
    uint8  num_qcrmcall;
    struct
    {
      uint8  qcrmcall_inst;
      uint8  ip_family;
    } qcrmcall[RMNET_QCRMCALL_MAX];
  } list;

} rmnet_qcrmcall_result_type;

typedef void (* rmnet_qcrmcall_cb_type)
(

 rmnet_qcrmcall_action_e_type  action,
 rmnet_qcrmcall_result_type  * result_code

);

#endif /* FEATURE_DATA_QCRMCALL */
/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

                        PUBLIC FUNCTION DECLARATIONS

= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/


/*===========================================================================
  FUNCTION RMNET_META_SM_POST_EVENT()

  DESCRIPTION
    This function posts an event to RMNetSM and enqueues the
    corresponding command to be processed in the DCC task.

  PARAMETERS
    instance:  Instance of RmNetSm to post event to
    event: The event that is being posted to the state machine.

  RETURN VALUE
    Boolean

  DEPENDENCIES
    RM State Machine should have already been initialized.

  SIDE EFFECTS
    None
===========================================================================*/
extern boolean rmnet_meta_sm_post_event
(
  rmnet_instance_e_type       instance,
  rmnet_meta_sm_event_type    event
);

/*===========================================================================
  FUNCTION RMNET_META_SM_IN_CALL()
  DESCRIPTION

    Returns true if we're in a packet call

  PARAMETERS
    instance - the WWAN Rm interface to query

  RETURN VALUE
    TRUE  - Rm interface 'instance' is starting/in/stopping a packet call
    FALSE - Rm interface 'instance' is idle

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern boolean rmnet_meta_sm_in_call
(
  rmnet_instance_e_type  instance
);


/*===========================================================================
FUNCTION RMNET_META_SM_SET_NETWORK_CFG_PARAMS

DESCRIPTION
  This function is called to set the parameters received in Start Network
  Interface for network configuration in the rmnet info structure.

PARAMETERS
  instance       : RmNet instance
  net_cfg_params : Network Configuration parameters

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void rmnet_meta_sm_set_network_cfg_params
(
  rmnet_instance_e_type              instance,
  rmnet_meta_sm_network_info_type *  net_cfg_params
);

/*===========================================================================
  FUNCTION RMNET_META_SM_GET_IPV4_MANUAL_DNS_ADDRS()

  DESCRIPTION
    Gets the manual DNS settings for this instance.  Each address that has not
    been set, the type will be returned as IP_ADDR_INVALID.

  PARAMETERS
    instance    - the RmNet instance
    pri_dns_ptr - primary DNS address pointer
    sec_dns_ptr - secondary DNS address pointer

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern void rmnet_meta_sm_get_ipv4_manual_dns_addrs
(
  rmnet_instance_e_type  instance,
  ip_addr_type           *pri_dns_ptr,
  ip_addr_type           *sec_dns_ptr
);

/*===========================================================================
  FUNCTION RMNET_META_SM_GET_IPV6_MANUAL_DNS_ADDRS()

  DESCRIPTION
    Gets the manual DNS settings for this instance.  Each address that has not
    been set, the type will be returned as IP_ADDR_INVALID.

  PARAMETERS
    instance    - the RmNet instance
    pri_dns_ptr - primary DNS address pointer
    sec_dns_ptr - secondary DNS address pointer

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern void rmnet_meta_sm_get_ipv6_manual_dns_addrs
(
  rmnet_instance_e_type  instance,
  ip_addr_type           *pri_dns_ptr,
  ip_addr_type           *sec_dns_ptr
);

/*===========================================================================
FUNCTION RMNET_GET_NUM_INSTANCES

DESCRIPTION
  This function gets the number of enumerated RmNet instances

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  uint8 - number of RmNet instances

SIDE EFFECTS
  None.
===========================================================================*/
extern uint8 rmnet_get_num_instances
(
  void
);

/*===========================================================================
  FUNCTION RMNET_WDS_REGISTER_EVENTS()

  DESCRIPTION
   regsiters client for all the PDN events. this callback is invoked for all the PDN events if the client is active.
      
  PARAMETERS
    client_id: client type
    callback_type: user callback 
    user-data_ptr: data that needs to be passed to client as part of PDN event invocation.

  RETURN VALUE
      0  -- if reg is success
     -1 -- if failed

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

int rmnet_wds_reg_events
(
  rmnet_wds_event_cback_type  callback_type,
  void                    *user_data_ptr
);

/*===========================================================================
FUNCTION RMNET_META_SM_BIND

DESCRIPTION
  This function binds a logical rmnet instance to a physical port

PARAMETERS
  ep_id:   physical port end point id
  mux_id:  logical rmnet mux id

DEPENDENCIES
  None

RETURN VALUE
  TRUE : success
  FALSE: failure

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean rmnet_meta_sm_bind
(
  uint32     ep_id,
  uint8      mux_id
);

/*===========================================================================
FUNCTION RMNET_META_SM_UNBIND

DESCRIPTION
  This function unbinds a logical rmnet instance from a physical port

PARAMETERS
  ep_id:   physical port end point id
  mux_id:  logical rmnet mux id

DEPENDENCIES
  None

RETURN VALUE
  TRUE : success
  FALSE: failure

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean rmnet_meta_sm_unbind
(
  uint32     ep_id,
  uint8      mux_id
);

/*===========================================================================
FUNCTION RMNET_INST_FROM_EP_AND_MUX_ID

DESCRIPTION
  This function retrieves rmnet instance from ep_id and mux_id

PARAMETERS
  ep_id:       physical port end point id
  mux_id:      logical rmnet mux id
  is_default:  is default instance
   ip_types:   ip
DEPENDENCIES
  None

RETURN VALUE
  rmnet instance number
  RMNET_INSTANCE_MAX if failure

SIDE EFFECTS
  None.
===========================================================================*/
extern rmnet_instance_e_type rmnet_inst_from_ep_and_mux_id
(
  uint32   ep_id,
  uint8    mux_id,
  boolean  is_default,
  uint8    ip_types
);

#ifdef FEATURE_DATA_QCRMCALL
/*===========================================================================
  FUNCTION RMNET_QCRMCALL_INST_ADD()

  DESCRIPTION
    This function adds the rmnet instance to the qcrmcall table

  PARAMETERS
    ep_id:     End point ID
    mux_id:    Mux ID

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void rmnet_qcrmcall_inst_add
(
  uint32   ep_id,
  uint8    mux_id
);

/*===========================================================================
  FUNCTION RMNET_QCRMCALL_INST_REMOVE()

  DESCRIPTION
    This function removes qcrmcall instance

  PARAMETERS
    ep_id:     End point ID

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void rmnet_qcrmcall_inst_remove
(
  uint32   ep_id
);


/*===========================================================================
  FUNCTION RMNET_SEND_QCRMCALL_REQUEST

  DESCRIPTION
    This function sends a qcrmcall request to RMNET

  PARAMETERS
    action:        qcrmcall reqeust type
    request:       request info
    qcrmcall_cb:   Callback to be invoked for getting result 

  RETURN VALUE
    TRUE: success
    FALSE: failed

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean rmnet_send_qcrmcall_request
(
  rmnet_qcrmcall_action_e_type    action,
  rmnet_qcrmcall_request_type   * request,
  rmnet_qcrmcall_cb_type          callback
);

#endif /* FEATURE_DATA_QCRMCALL */

/*===========================================================================
  FUNCTION RMNET_META_SM_GET_FREE_MUX_ID

  DESCRIPTION
    This function gets the mux on which the call is brought

  PARAMETERS
    call type         type of call
    apn               Name of apn on which call is been made

  RETURN VALUE
    TRUE  - Success
    FALSE - Failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
uint8 rmnet_meta_sm_get_free_mux_id
(
  rmnet_meta_sm_call_e_type  call_type,
  char                      *apn
);

/*===========================================================================
  FUNCTION GET_MUX_ID_FROM_RMNET_INST

  DESCRIPTION
    This function gets the mux on which the call is brought

  PARAMETERS
    rmnet_inst: Instance from which mux id is to find
    
  RETURN VALUE
    TRUE  - Success
    FALSE - Failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

extern uint8 rmnet_get_mux_id_from_inst
(
  ds_fwk_clnt_handle_type  client_inst
);

/*===========================================================================
  FUNCTION BIND_MUX_ID_WITH_RMNET_INST

  DESCRIPTION
    This function gets the mux on which the call is brought

  PARAMETERS
    mux_id :    MUX id
    rmnet_inst: Instance on which mux id is to bind

  RETURN VALUE
    TRUE  - Success
    FALSE - Failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

extern void rmnet_bind_mux_id_with_rm_inst
(
  uint8 mux_id,
  rmnet_instance_e_type rmnet_inst
);

/*===========================================================================
  FUNCTION RMNET_GET_STREAM_ID_FROM_CLIENT

  DESCRIPTION
    This function gets the stream id on which the call is brought

  PARAMETERS
    cl_inst: FWK Instance on which rmnet instance is bind and call is brought up

  RETURN VALUE
    stream id

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

sio_stream_id_type rmnet_get_stream_id_from_client
(
 ds_fwk_clnt_handle_type cl_inst
);

/*===========================================================================
FUNCTION RMNET_GET_EP_ID_FROM_RMNET_INST

DESCRIPTION
  This is   function to get the EP ID from Rmnet instance

PARAMETERS
  cl_inst:      Rm inst

DEPENDENCIES
  None

RETURN VALUE
  EP_ID
  
SIDE EFFECTS
  None.
===========================================================================*/
uint32 rmnet_get_ep_id_from_rmnet_inst
(
  ds_fwk_clnt_handle_type  client_inst
);

/*===========================================================================
FUNCTION RMNET_GET_RMNET_INST_FROM_STREAM_ID

DESCRIPTION
  This is function to get the Rmnet instance from stream ID

PARAMETERS
  stream_id -  stream id of RMNET call
  ip_type -    IP Type of the call

DEPENDENCIES
  None

RETURN VALUE
  EP_ID
  
SIDE EFFECTS
  None.
===========================================================================*/
rmnet_instance_e_type rmnet_get_rmnet_inst_from_stream_id
(
  sio_stream_id_type  stream_id,
  ip_addr_enum_type   ip_type,
  void                **rmnet_smi_info 
);

/*===========================================================================
  FUNCTION DS_RMNET_PORT_OPEN_CNF_CB

  DESCRIPTION
	This function gets called by A2 once logical port is opened
	on which call is brought up

  PARAMETERS
    port_id: port on which the call is brought up

  RETURN VALUE
    None 

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void ds_rmnet_port_open_cnf_cb
(
  sio_port_id_type	port_id
);

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */

#endif /* _DS_RMNET_META_SM_H */

