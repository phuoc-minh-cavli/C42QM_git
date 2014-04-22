#ifndef _DS_RMNET_SMI_H
#define _DS_RMNET_SMI_H
/*===========================================================================

                        D S _ R M N E T _ S M I . H

DESCRIPTION

  Rm Network device - State machine definitions internal header file.

EXTERNALIZED FUNCTIONS

Copyright (c) 2018-2020 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/interface/rmnet/inc/ds_rmnet_smi.h#6 $ $DateTime: 2020/12/08 02:29:19 $ $Author: pwbldsvc $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
10/08/18     mks   Added api to get the smi_info ptr to check the RM state
===========================================================================*/

/*===========================================================================

                                INCLUDE FILES

===========================================================================*/
#include "ds_headers.h"
#include "ds_rmnet_meta_sm.h"
#include "ds_rmnet_xport.h"
#include "ps_rm_taski.h"
#include "ds_pdn_manager.h"

/*===========================================================================

                                 DEFINITIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  RMNET SIO Watermark level definitions
---------------------------------------------------------------------------*/
#define RMNET_RX_WM_LOW   15000
#define RMNET_RX_WM_HIGH  45000
#define RMNET_RX_WM_DNE   250000
#define RMNET_RX_WM_Q_CNT_DNE 1500


/* RMNET_TX_WM_DNE = DSM_DS_SMALL_ITEM_CNT * DSM_DS_SMALL_ITEM_SIZ
Static counts of DSM have changed */
#define RMNET_TX_WM_DNE       456576
#define RMNET_TX_WM_Q_CNT_DNE 2000

#define RMNET_META_SM_INFO_TO_INSTANCE(info) \
                           ((info)->constants.rmnet_inst)

#define RMNET_EP_ID(info)  RMNET_XPORT_EP_ID((info)->xport_info)
#define RMNET_MUX_ID(info) RMNET_XPORT_MUX_ID((info)->xport_info)

/*---------------------------------------------------------------------------
  Data format macros
---------------------------------------------------------------------------*/
#define RMNET_LINK_PROT(info)   RMNET_XPORT_LINK_PROT((info)->xport_info)
#define RMNET_QOS_ENABLED(info) RMNET_XPORT_QOS_ENABLED((info)->xport_info)
#define RMNET_TE_FC_ENABLED(info) RMNET_XPORT_TE_FC_ENABLED((info)->xport_info)
#define RMNET_UL_DATA_AGG(info) RMNET_XPORT_UL_DATA_AGG((info)->xport_info)
#define RMNET_DL_DATA_AGG(info) RMNET_XPORT_DL_DATA_AGG((info)->xport_info)
#define RMNET_NDP_SIG(info)     RMNET_XPORT_NDP_SIG((info)->xport_info)
#define RMNET_IN_BAND_FC(info)  RMNET_XPORT_IN_BAND_FC((info)->xport_info)
#define RMNET_QOS_HEADER_TYPE(info)  RMNET_XPORT_QOS_HEADER_TYPE((info)->xport_info)
#define RMNET_DATA_AGG_IS_QMAP(data_agg) \
       ((data_agg) == RMNET_ENABLE_DATA_AGG_QMAP || \
        (data_agg) == RMNET_ENABLE_DATA_AGG_QMAP_V2 || \
        (data_agg) == RMNET_ENABLE_DATA_AGG_QMAP_V3)

#define RMNET_MAX_ICMP_OUTSTANDING_REQ 30

/*---------------------------------------------------------------------------
  Declaration of the states of the Meta RmNet state machine.
---------------------------------------------------------------------------*/
typedef enum
{
  RMNET_META_SM_MIN_STATE                 = -1,
  RMNET_META_SM_NULL_STATE                = 0,   /* Initial state           */
  RMNET_META_SM_UM_CONFIGURING_STATE      = 1,   /* Um coming up            */
  RMNET_META_SM_RM_CONFIGURING_STATE      = 2,   /* Rm and Um up            */
  RMNET_META_SM_NET_IF_UP_STATE           = 3,
  RMNET_META_SM_RM_RECONFIGURING_STATE    = 4,   /* Um is temp unavailable  */
  RMNET_META_SM_WAITING_FOR_UM_DOWN_STATE = 5,   /* Um going down, Rm down  */
  RMNET_META_SM_EP_CONFIGURING_STATE      = 6,   /* EP configuring state    */  
  RMNET_META_SM_MAX_STATE
} rmnet_meta_smi_state_type;

/*---------------------------------------------------------------------------
	Enum type indicating the RmNet state machine.
 ---------------------------------------------------------------------------*/
  typedef enum
  {
	RMNET_INVALID_SM = 0,
	RMNET_META_SM	 = 1,
	RMNET_MAX_SM
  } rmnet_smi_e_type;

//For future use as flow events are not done 
typedef enum
{
  RMNET_SM_CMD_TX_FLOW_ENABLE  = 1,    /* RmNet iface flow enable        */
  RMNET_SM_CMD_TX_FLOW_DISABLE = 2,    /* RmNet iface flow disable       */
#ifdef FEATURE_DATA_QCRMCALL
  RMNET_SM_CMD_QCRMCALL        = 3,
#endif /* FEATURE_DATA_QCRMCALL */
  RMNET_SM_CMD_MAX
} rmnet_sm_cmd_e_type;

/*--------------------------------------------------------------------------
                            Flow Control Mask
--------------------------------------------------------------------------*/
#define RMNET_FC_MASK_SIO         0x01
#define RMNET_FC_MASK_IFACE       0x02
#define RMNET_FC_MASK_FLOW        0x04
#define RMNET_FC_MASK_PHYS_LINK   0x08

#define RMNET_MAX_PDN_SESSIONS  8 //(Max PDN supported in chitepin is 4 so total 8 for v4 and v6)
#define RMNET_UL_MAX_SIGNALS    RMNET_MAX_PDN_SESSIONS

/** 
 Size of the hardware address in bytes 
*/
#define PS_MAX_HW_ADDR_LEN 6 

/*---------------------------------------------------------------------------
  The Rm state machine's information block
---------------------------------------------------------------------------*/
typedef struct
{
  /*-------------------------------------------------------------------------
    Common to all RmNet SMs.
  -------------------------------------------------------------------------*/

  struct constants_s
  {
    uint8                 ip_inst;
    rmnet_instance_e_type  rmnet_inst;
  } constants;

  rmnet_xport_type      * xport_info;           /* Logical transport info */
  boolean                 has_flow;
  /*-------------------------------------------------------------------------
    For flow control
  -------------------------------------------------------------------------*/
  uint32                  rx_fc_mask;

  /*-------------------------------------------------------------------------
    Meta SM specific information
  -------------------------------------------------------------------------*/
  struct meta_sm_s
  {
    rmnet_meta_smi_state_type  state;              /* State machine state */
    boolean                dtr_is_high;
    ds_net_down_reason_type call_end_reason; /* Um iface down reason */
    ds_stats_type              call_stats;
    struct settings_s
    {
      ip_addr_type pri_dns_addr;
      ip_addr_type sec_dns_addr;

      ip_addr_type ipv6_pri_dns_addr;
      ip_addr_type ipv6_sec_dns_addr;
    } settings;

    rmnet_meta_sm_network_info_type  net_params;
    rmnet_meta_sm_proc_id_e_type    proc_id; /* proc id for the instance */
  } meta_sm;
  ds_pdn_event_enum_type     last_event;  
  ds_pdn_event_info_u_type   event_info;  
  /*ICMP data*/
  struct icmp_data_s
  {
    int16 identifier;
    int16 sequence_num;
  } icmp_data[RMNET_MAX_ICMP_OUTSTANDING_REQ];
  uint8                      hw_addr[PS_MAX_HW_ADDR_LEN];  
} rmnet_smi_info_type;

typedef struct
{
  rmnet_xport_type      xport_info;
  rmnet_smi_info_type  *info_v4;

  rmnet_smi_info_type  *info_v6;
  rmnet_smi_info_type  *info_nipd;

  uint8                 rx_flow_count;
  ps_rm_sig_enum_type   rmnet_ul_sig;  
  int32                 call_count;
  dsm_item_type        *rx_pkt;
  boolean               ep_cfg_done; 
  boolean               is_mo_ex_pkt_queued;
  /*-----------------------------------------------------------------------
    Stores the Outstanding Pkt in data path when flow is controlled.
    Packet will be routed to the right PDN by storing it here.
  -----------------------------------------------------------------------*/
} rmnet_smi_ip_info_type;

typedef PACKED struct PACKED_POST
{
  uint8           version;
  uint8           flags;
//  ps_flow_type *  flow_ptr;
} rmnet_meta_smi_qos_hdr_type;

typedef PACKED struct PACKED_POST
{
  uint8           version;
  uint8           flags;
  uint32          qos_handle;
} ps_rmnet_qos_hdr_type;

typedef PACKED struct PACKED_POST
{
  ps_rmnet_qos_hdr_type hdr;
  uint8               reserved[2];
} ps_rmnet_qos2_hdr_type;

/*---------------------------------------------------------------------------
  Struct used to pass RMNET event and info pointer
---------------------------------------------------------------------------*/
typedef struct
{
 uint32 * rmnet_sm_info_ptr;
 int      sm_id;
  uint32   event;
} rmnet_smi_ev_cmd_type;

typedef struct
{
  void *  info_ptr;
  int     cmd_type;
  int     sm_id;
  union
  {	
#ifdef FEATURE_DATA_QCRMCALL
   struct
   {
      rmnet_qcrmcall_action_e_type action;
      rmnet_qcrmcall_request_type  request;
      rmnet_qcrmcall_cb_type       callback;
    } qcrmcall;
#endif /* FEATURE_DATA_QCRMCALL */

  } data;
} rmnet_smi_cmd_type;



/*---------------------------------------------------------------------------
  NV struct for storing DNS addresses for each instance
---------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint32                                        ep_id;
  uint8                                         mux_id;
  uint32                                        dns_addr;
} qmi_nv_dns_addr_type;

/*---------------------------------------------------------------------------
  NV struct for storing IPv6 DNS addresses for each instance
---------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint32                                        ep_id;
  uint8                                         mux_id;
  uint64                                        dns_addr[2];
} qmi_nv_ipv6_dns_addr_type;

extern rmnet_smi_info_type * rmnet_smi_info[RMNET_INSTANCE_MAX];
typedef struct
{
  boolean  in_use; 
  boolean  null_apn;
  char     apn[255];
}rmnet_mux_call_info;


/*===========================================================================

                        PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
  FUNCTION RMNET_META_SM_PROCESS_EV_CMD()

  DESCRIPTION
    This function processes a RmNet event cmd.

    It is called by the serving task main loop and will dispatch the
    associated event handler function.

  PARAMETERS
    cmd          : RmNet cmd to be processed
    user_data_ptr: private data buffer containing the RmNet sm
                   event information

  RETURN VALUE
    None

  DEPENDENCIES
    RM State Machine should have already been initialized

  SIDE EFFECTS
    None
===========================================================================*/
void rmnet_meta_sm_process_ev_cmd
(
  ds_command_enum_type cmd,
  void * user_data_ptr
);


/*===========================================================================
  FUNCTION RMNET_META_SM_PROCESS_CMD()

  DESCRIPTION
    This function processes a RmNet Meta SM cmd.

  PARAMETERS
    cmd          : RmNet cmd to be processed
    user_data_ptr: private data buffer containing the RmNet sm
                   event information

  RETURN VALUE
    None

  DEPENDENCIES
    RM State Machine should have already been initialized

  SIDE EFFECTS
    None
===========================================================================*/
void rmnet_meta_sm_process_cmd
(
  ds_command_enum_type    cmd,
  void                *user_data_ptr
);

/*===========================================================================
  FUNCTION RMNET_META_SMI_PROCESS_QOS_HDR()

  DESCRIPTION


  PARAMETERS


  RETURN VALUE


  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

void rmnet_meta_smi_process_qos_hdr
(
  void *                user_data,
  dsm_item_type *       pkt_ptr,
  ps_meta_info_type_ex  ** meta_info_ex_ptr_ptr,
  rmnet_meta_smi_qos_hdr_type * qos_hdr
);

/*===========================================================================
FUNCTION RMNET_GET_ADDR_FAMILY

DESCRIPTION
  This is   function to get the addr family

PARAMETERS
  info:     Rmnet state info pointer

DEPENDENCIES
    None

RETURN VALUE
  ip_version
  
SIDE EFFECTS
  None.
===========================================================================*/

ip_addr_enum_type rmnet_get_addr_family
(
  rmnet_smi_info_type *info
);

/*===========================================================================
  FUNCTION RMNET_GET_SMI_INFO_PTR

  DESCRIPTION
    This function gets the rmnet info on which the call is brought

  PARAMETERS
    cl_inst: FWK Instance on which rmnet instance is bind and call is brought up

  RETURN VALUE
    rmnet smi_info ptr

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

rmnet_smi_info_type * rmnet_get_smi_info_ptr
(
  uint8 rm_inst
);

/*===========================================================================
  FUNCTION RMNET_GET_SMI_PTR

  DESCRIPTION
    This function gets the rmnet info on which the call is brought

  PARAMETERS
    cl_inst: FWK Instance on which rmnet instance is bind and call is brought up

  RETURN VALUE
    rmnet smi_info ptr

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

rmnet_smi_info_type * rmnet_get_smi_ptr
(
  uint8 rm_inst
);

/*===========================================================================
FUNCTION RMNET_GET_IP_INFO_FROM_INST

DESCRIPTION
  This function retrieves rmnet ip info from instance

PARAMETERS
  ip_inst:    ip_info instnace

DEPENDENCIES
  None

RETURN VALUE
  rmnet_smi_ip_info_type pointer

SIDE EFFECTS

  None.
===========================================================================*/
rmnet_smi_ip_info_type *rmnet_get_ip_info_from_inst
(
  uint8 ip_inst
);




/*===========================================================================
  FUNCTION RMNET_SM_PROCESS_EV_CMD()

  DESCRIPTION
    This function processes a RmNet RmSM event cmd.

    It is called by the serving task main loop and will dispatch the 
    associated event handler function.

  PARAMETERS
    cmd          :  RmNet command to be processed
    user_data_ptr:  private data buffer containing the Rmnet command
                    information.

  RETURN VALUE
    None

  DEPENDENCIES
    RM State Machine should have already been initialized

  SIDE EFFECTS
    None
===========================================================================*/
void rmnet_sm_process_ev_cmd
(
  ds_command_enum_type cmd,
  void              *user_data_ptr
);


/*===========================================================================
  FUNCTION RMNET_SM_PROCESS_CMD()

  DESCRIPTION
    This function processes a RmNet RmSM cmd.

  PARAMETERS
    cmd          :  RmNet command to be processed
    user_data_ptr:  private data buffer containing the Rmnet command
                    information.

  RETURN VALUE
    None

  DEPENDENCIES
    RM State Machine should have already been initialized

  SIDE EFFECTS
    None
===========================================================================*/
void rmnet_sm_process_cmd
(
  ds_command_enum_type cmd,
  void              *user_data_ptr
);

#endif /* _DS_RMNET_SMI_H */
