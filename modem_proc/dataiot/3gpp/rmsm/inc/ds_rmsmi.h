#ifndef DS_RMSMI_H
#define DS_RMSMI_H
/*===========================================================================

                          D S U M T S _ R M S M I . H

DESCRIPTION
  This file contains the private definitions for RM state machine.
  It contains the definitions and structures used for pdp-ip
  RM state machines
  
  NOTE:
  This definitions are private to DS_RMSM and should not be used by 
  the clients of DS_RMSM directly.

  
EXTERNALIZED FUNCTIONS

Copyright (c) 2002-2018 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/


/*===========================================================================

                            EDIT HISTORY FOR FILE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/dataiot.mpss/2.0/3gpp/rmsm/inc/ds_rmsmi.h#2 $ 
  $DateTime: 2020/04/03 00:33:45 $ $Author: pwbldsvc $

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "ds_headers.h"
#include "sys.h"
#include "dstask.h"
#include "dsm.h"
#include "ps_svc.h"
#include "dsat_v.h"
#include "ds_rmsm_ipi.h"
#include "ds3gsiolib.h"
#include "ps_ppp.h"
#include "modem_mem.h"
#include "ds_3gpp_rmsm_ip_common.h"
#include "ds_sys.h"
#include "ds_fwk.h"
#include "ps_in.h"
#include "dstaski.h"
#include "ds_Utils_DebugMsg.h"




/*---------------------------------------------------------------------------
  Critical Section macros
---------------------------------------------------------------------------*/
#define DS_RMSM_INIT_CRIT_SECTION( rex_crit_section )                        \
    { rex_init_crit_sect( rex_crit_section ); }

#define DS_RMSM_ENTER_CRIT_SECTION( rex_crit_section )                       \
    { rex_enter_crit_sect( rex_crit_section ); }

#define DS_RMSM_BRANCH_LEAVE_CRIT_SECTION( rex_crit_section )                \
    { rex_leave_crit_sect( rex_crit_section ); }

#define DS_RMSM_LEAVE_CRIT_SECTION( rex_crit_section )                       \
    { rex_leave_crit_sect( rex_crit_section ); }

#define DS_RMSM_ENTER_CRIT_SECTION_NO_TASKLOCK( rex_crit_section )           \
    { rex_enter_crit_sect( rex_crit_section ); }

#define DS_RMSM_LEAVE_CRIT_SECTION_NO_TASKLOCK( rex_crit_section )           \
    { rex_leave_crit_sect( rex_crit_section ); }

#define DS_RMSM_DELETE_CRIT_SECTION( rex_crit_section )                        \
    { rex_del_crit_sect( rex_crit_section ); }
	
/*---------------------------------------------------------------------------
  Utility macros
---------------------------------------------------------------------------*/

#define DS_CONDITIONAL_BLOCK_START( cond, msg_low )                     \
        if ( cond ) {                                                       \
          MSG_LOW(msg_low, 0, 0, 0);

#define DS_CONDITIONAL_BLOCK_ELSE( cond, msg_low )                      \
        } else if ( cond ) {                                                \
          MSG_LOW(msg_low, 0, 0, 0);

#define DS_CONDITIONAL_BLOCK_END( )                                     \
        } /* end of conditional block */

/*===========================================================================

                         REGIONAL DATA DECLARATIONS

===========================================================================*/
   
#define  DS_RMSMI_GET_PDP_TYPE(rmsmi_info_ptr)  \
    rmsmi_info_ptr->profile_info.context.pdp_type

/*---------------------------------------------------------------------------
  The state machine type. Either PDP-IP or PDP-PPP
---------------------------------------------------------------------------*/              
typedef enum
{
  DS_RMSM_INVALID_SM_TYPE =  -1, 
  DS_RMSM_MIN_SM_TYPE     =  DS_RMSM_INVALID_SM_TYPE,
  DS_RMSM_SM_TYPE_PDP_IP  =  0,                /* PDP-IP State machine    */
  DS_RMSM_SM_TYPE_PDP_PPP =  1,                /* PDP-PPP state machine   */
  DS_RMSM_MAX_SM_TYPE    
} ds_rmsmi_sm_type;

typedef enum
{
 RMSM_SIO_INST_INVALID = -1,
 RMSM_SIO_INST_MIN  = RMSM_SIO_INST_INVALID,
 RMSM_SIO_INST_DOWN,
 RMSM_SIO_INST_UP,
 RMSM_SIO_INST_COMING_UP 
} ds_rmsm_sio_instance_state_e;

/*---------------------------------------------------------------------------
  SM Instance identifies the instance of sm_info_ptr in ds_rmsmi_info.
  Need to revisit during Dual-IP implementation.
---------------------------------------------------------------------------*/             
typedef enum
{
  DS_RMSM_INVALID_SM_INSTANCE = -1,
  DS_RMSM_ANY_SM_INSTANCE = -1,
  DS_RMSM_V4_SM_INSTANCE = 0,          
  DS_RMSM_V6_SM_INSTANCE,
  DS_RMSM_MAX_SM_INSTANCES
} ds_rmsm_sm_instance_type;

/*---------------------------------------------------------------------------
  Rm Dev instance identifies the instance of ds_rmsmi_info. 
---------------------------------------------------------------------------*/
typedef enum
{
  DS_RMSM_INVALID_RM_DEV_INSTANCE = -1,
  DS_RMSM_RM_DEV_INSTANCE_0 = 0,
  DS_RMSM_RM_DEV_INSTANCE_1 = 1,
  DS_RMSM_MAX_RM_DEV_INSTANCES
} ds_rmsm_rm_dev_instance_type;

/*---------------------------------------------------------------------------
  Maximum possible number of state machine instances  
---------------------------------------------------------------------------*/
#define DS_RMSM_MAX_INSTANCES (DS_RMSM_MAX_SM_INSTANCES *    \
                                   DS_RMSM_MAX_RM_DEV_INSTANCES)

#define DS_INVALID_HANDLE  -1
#define DS_MAX_HANDLE       7

#define DS_RMSM_SET_IS_BCAST_INST(sio_inst, is_bcast_instance) \
	sio_inst->is_bcast_inst = is_bcast_instance                \


/*---------------------------------------------------------------------------
  Declaration of the structure containing Rm Dev instance and Sm Instance
  information. 
---------------------------------------------------------------------------*/
typedef struct
{
  ds_rmsm_rm_dev_instance_type rm_dev_instance;
  ds_rmsm_sm_instance_type     sm_instance;
  void *data_ptr;
} ds_rmsm_instance_type; 

/*-------------------------------------------------------------------------
    PDP-ip or PDP-ppp State machine specific info 
-------------------------------------------------------------------------*/
typedef struct
{
  ds_rmsm_ipi_specific_info  pdp_ip;
} ds_rmsmi_pdp_specific_info_type;

typedef struct
{
  uint32             tx_flow_mask;
  ip_addr_enum_type  addr_family;
  ds_rmsm_sio_instance_state_e             state;
  uint8              in_use;
  ds_v6_addr_type    ipv6_addrs;
}ds_rmsm_sio_instance;

/*---------------------------------------------------------------------------
  State-Machine Specific control block. Each State Machine will have a
  unique instance of this control block.
  When we support dual-IP, there will be 2 instances of this block -
  one for V4 and one for V6.
---------------------------------------------------------------------------*/
typedef struct ds_rmsmi_sm_info_s
{  
  ds_rmsm_sm_instance_type            instance;/*v4 or v6 sm instance type */ 
  boolean                             inited; /* State machine been initialized? */
  ds_fwk_s_type                       *fw_inst;/*fwk inst ptr for this sm instance */      									  
  ds_rmsm_sio_instance                *sio_inst;                                      
  ds_rmsmi_pdp_specific_info_type     pdp_specific_info;									 
  void*                               vobj_data_ptr; /* Corresponding Rmsm info ptr*/                                      
  byte                                flow_enabled;
  boolean                             recd_um_down;/* flag indicating if iface down recd for this instance*/   
  uint8                               instance_hdl;/* FW instance Handle; client handle*/
}ds_rmsmi_sm_info_type;

/*---------------------------------------------------------------------------
  The Generic RM control block. Each Rm Device will have a
  unique instance of this control block.
---------------------------------------------------------------------------*/
typedef struct ds_rmsmi_info_s
{
  rex_crit_sect_type        rx_data_crit_sec;
  boolean                   rx_data_can_be_pulled;
  dsm_watermark_type        sio_tx_wmk;     /* watermark for SIO tx operation */                           
  q_type                    sio_tx_wmk_q;   /* queue for SIO tx watermark */                            
  dsm_watermark_type        sio_rx_wmk;     /* watermark for SIO rx operation */                           
  q_type                    sio_rx_wmk_q;   /* queue for SIO rx watermark */                          
  ps_sig_enum_type          ps_ppp_rm_rx_signal; /*PS signal indicating presence of data in RM Rx WM*/

  uint32                    profile_id;     /* Context/profile ID */                             
  ds_profile_type_s         profile_info;   /* profile: context def */
  void*                     ppp_handle;     /* Handle for registering for PPP events */                            
  ds_rmsmi_sm_type          sm_type;        /* state machine type:pdp-ip or ppp */                            
  ppp_dev_opts_type         *rm_ppp_config_ptr;  /* Dev options for starting PPP */ 								   
  ds_rmsm_rm_dev_instance_type  instance;  /* state machine instance */
  ds_3gpp_rmsm_ip_common_info_type *ds_3gpp_rmsm_ip_common_info_ptr; /* Pointer to RmSM IP Common*/								   
  ds_rmsmi_sm_info_type     *sm_info_ptr [DS_RMSM_MAX_SM_INSTANCES]; /*Pointers to v4/v6 State machine specific info.*/
  ds3g_siolib_port_e_type   port_id; /* Port Id on which the DUN call is being attempted */
  ps_timer_handle_type      ppp_up_delay_timer;
  ps_timer_handle_type      config_wait_timer;
  boolean                   singleip_dual_pf_call;
} ds_rmsmi_info_type;

void ds_rmsmi_set_rx_data_can_be_pulled_flag 
(
  ds_rmsmi_info_type* rmsmi_info_ptr
);

#define SET_RX_DATA_CAN_BE_PULLED(rmsmi_info_ptr) \
  ds_rmsmi_set_rx_data_can_be_pulled_flag(rmsmi_info_ptr)

void ds_rmsmi_reset_rx_data_can_be_pulled_flag 
(
  ds_rmsmi_info_type* rmsmi_info_ptr
);
#define RESET_RX_DATA_CAN_BE_PULLED(rmsmi_info_ptr) \
  ds_rmsmi_reset_rx_data_can_be_pulled_flag(rmsmi_info_ptr)

void ds_rmsmi_reset_rx_data_can_be_pulled_flag 
(
  ds_rmsmi_info_type* rmsmi_info_ptr
);
#define RX_DATA_CAN_BE_PULLED(rmsmi_info_ptr) \
  ds_rmsmi_get_rx_data_can_be_pulled_flag(rmsmi_info_ptr)

/*---------------------------------------------------------------------------
  max_bit_rate values for data rates included in extended result code for 
  CONNECT for PDP type IP call.
---------------------------------------------------------------------------*/

#define MAX_BIT_RATE_8KBPS            8000
#define MAX_BIT_RATE_16KBPS           16000
#define MAX_BIT_RATE_32KBPS           32000
#define MAX_BIT_RATE_64KBPS           64000
#define MAX_BIT_RATE_128KBPS          128000
#define MAX_BIT_RATE_256KBPS          256000
#define MAX_BIT_RATE_512KBPS          512000
#define MAX_BIT_RATE_1024KBPS         1024000
#define MAX_BIT_RATE_2048KBPS         2048000
#define MAX_BIT_RATE_7200KBPS         7200000
#define MAX_BIT_RATE_14000KBPS        14000000
#define MAX_BIT_RATE_21000KBPS        21000000
#define MAX_BIT_RATE_42000KBPS        42000000
#define MAX_BIT_RATE_63000KBPS        63000000

#ifdef FEATURE_DATA_LTE
  #define MAX_BIT_RATE_150000KBPS      (150000*1000)
  #define MAX_BIT_RATE_50000KBPS       (50000*1000)
#endif /* FEATURE_DATA_LTE */

/* Default rate based off HSDPA device capability */
#ifdef FEATURE_WCDMA_3C_HSDPA
  #define MAX_BIT_RATE_DEFAULT        MAX_BIT_RATE_63000KBPS
#elif defined (FEATURE_WCDMA_REL8)
  #define MAX_BIT_RATE_DEFAULT        MAX_BIT_RATE_42000KBPS
#elif defined(FEATURE_HSPA_PLUS_CAT14)
  #define MAX_BIT_RATE_DEFAULT        MAX_BIT_RATE_21000KBPS
#elif defined(FEATURE_HSPA_PLUS_CAT10)
  #define MAX_BIT_RATE_DEFAULT        MAX_BIT_RATE_14000KBPS
#else
  #define MAX_BIT_RATE_DEFAULT        MAX_BIT_RATE_7200KBPS
#endif /* FEATURE_WCDMA_3C_HSDPA */

/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

                        PUBLIC FUNCTION DECLARATIONS
  These are to be used by DS_RMSM internally and should not be used by 
  the clients of DS_RMSM

= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

/*===========================================================================
  FUNCTION DS_RMSMI_INIT_ALL_INSTANCES()

  DESCRIPTION
    This function initializes all the instances of UMTS RM state machines. 
    There is one state machine per Rm interface that is enabled for Data.
    
    As part of initialization it creates SIO iface, init 
    queues/wms, allocates event buffers and resets state variables.
   
   IT also calls the pdp-ip and pdp-ppp specific sub state machine initializations
    
  PARAMETERS
    None
    
  RETURN VALUE
    TRUE on success
    FALSE on failure

  DEPENDENCIES
    Should only be called once at powerup

  SIDE EFFECTS
    None
===========================================================================*/
boolean ds_rmsmi_init_all_instances( void );

/*===========================================================================
  FUNCTION DS_RMSMI_INIT_RMDEV_INSTANCE()

  DESCRIPTION
    This function initializes the passed instance of UMTS RM state machine. 
    There is one state machine per Rm interface that is enabled for Data.
    As part of initialization it creates SIO iface, init 
    queues/wms, allocates event buffers and resets state variables.
   IT also calls the pdp-ip specific sub state machine initializations
    
  PARAMETERS
    
  RETURN VALUE

  DEPENDENCIES

  SIDE EFFECTS
    None
===========================================================================*/

ds_rmsmi_info_type* ds_rmsmi_init_rmdev_instance (
    ds_rmsm_rm_dev_instance_type rm_dev_instance
);

/*====================================================================
  FUNCTION ds_rmsmi_sio_cleanup ()

  DESCRIPTION
    This is used to perform SIO port related cleanup before call tear down

  PARAMETERS

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
====================================================================*/
void ds_rmsmi_sio_cleanup ( ds_rmsm_rm_dev_instance_type rm_dev );

/*===========================================================================
  FUNCTION DS_RMSMI_HANDLE_ATD_ON_RM

  DESCRIPTION
    This function handles the dial strings for pkt data calls on a 
    specific RM instance and initiates the data call over the appropriate 
    pkt iface.
    
    It queries the pdp profile registry to get the profile associated with
    the context number specified in the dial strig. Based on that it sets
    the "sub-state" to either "pdp-ip" or "pdp-ppp" and kick starts the 
    state machine

  PARAMETERS
    dial_string: The dial string to be processed
    digit_mode: Flag to tell whether dial string contains digits only

  RETURN VALUE
    Function pointer to abort handler

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
dsat_call_abort_cb_type ds_rmsmi_handle_atd_on_rm
(
  ds_rmsm_rm_dev_instance_type rm_dev_instance,  /* Rm inst. on which atd */
  const byte *dial_string,                        /* dial string           */
  boolean    digit_mode,                          /* non-digit or digits   */
  const dsat_dial_modifier_info_type * modifiers /* Dial sting modifiers  */
);


/*===========================================================================
  FUNCTION DS_RMSMI_CALL_ABORT_CB()

  DESCRIPTION
    This function is registered as abort handler with ATCOP. If ATCOP
    receives an AT cmd while processing an abortable cmd, this function gets
    called.  It is registered along with the dial string handler. It posts
    appropriate event to DS_RMSM which initiates the call termination.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void ds_rmsmi_call_abort_cb
(
  void
);

/*===========================================================================
  FUNCTION DS_RMSMI_PROCESS_DTR_SIG_CB()

  DESCRIPTION
    This function processes the change in dtr state.  If DTR is deasserted
    appropriate event is posted to gen_rmsm which initiates the call 
    termination. DTR assert is ignored for now.

  PARAMETERS
    is_asserted: flag which indicates current DTR state -
                 TRUE: DTR asserted, FALSE: DTR deasserted
    cb_data:     Callback Data

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void ds_rmsmi_process_dtr_sig_cb
(
  boolean is_asserted,
  void* cb_data
);

/*===========================================================================
  FUNCTION DS_RMSMI_SETUP_SIO_WATERMARKS()

  DESCRIPTION
    This function configures the SIO packet mode watermark items
    to operate for the laptop pkt data calls handled by DS_RMSM.

  PARAMETERS
    rm_dev_instance   - RM Device Instance 
    *tx_wm_ptr - ptr to sio transmit (to laptop) watermark
    *rx_wm_ptr - ptr to sio receive (from laptop) watermark

  RETURN VALUE
    None

  DEPENDENCIES
    Hi/Lo Watermark Settings: The Lo and Hi watermark values and the don't
    exceed values for each watermark is inter-dependent with the DSM small
    and large item counts (DSM_LARGE_ITEM_CNT and DSM_SMALL_ITEM_CNT) in the
    DSM module. Any change to lo/hi watermark or don't exceed values can
    cause ERR_FATALs due to insufficient memory items available.

  SIDE EFFECTS
    None
===========================================================================*/
void ds_rmsmi_setup_sio_watermarks
(
  ds_rmsm_rm_dev_instance_type rm_dev_instance,
  dsm_watermark_type *tx_wm_ptr,
  dsm_watermark_type *rx_wm_ptr,
  q_type             *tx_wmk_q_ptr,
  q_type             *rx_wmk_q_ptr 
);

/*===========================================================================
FUNCTION DS_RMSMI_SIO_TX_PPP_DATA

DESCRIPTION
  This function is called to transmit a packet coming on Um PPP
  interface onto Rm interface.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_rmsmi_sio_tx_ppp_data
(
  dsm_item_type **item_head_ptr,
  void           *meta_info_ptr,
  void           *user_data
);

/*===========================================================================
FUNCTION DS_RMSMI_SIO_RX_PPP_DATA

DESCRIPTION
  This function is called to receive the next PPP packet, coming on the Rm
  interface, for rx processing.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
dsm_item_type* ds_rmsmi_sio_rx_ppp_data( void *user_data );

/*===========================================================================
  FUNCTION DS_RMSMI_PROCESS_ESC_SEQ_CB

  DESCRIPTION
    This function is called when DTR goes low and AT&D = 1.  This should do
    the same action as the dtr_sig_cb.
  PARAMETERS
    Callback Data
  RETURN VALUE
    Always return DSAT_ONLINE_DATA.
  DEPENDENCIES
    None
  SIDE EFFECTS
    None
 ===========================================================================*/
dsat_mode_enum_type ds_rmsmi_process_esc_seq_cb
(
  void* cb_data
);

/*===========================================================================
FUNCTION   DS_RMSMI_GET_INSTANCE_NUM_FROM_SIO_INST()

DESCRIPTION
  Returns the instance of the UMTS_RMSM corresponding to the supplied 
  sio_inst_ptr.

NOTE: THIS IS A RESTRICTED FUNCTION AND IS INTENDED TO BE USED BY 
DS_RMSM MODULES ONLY

PARAMETERS
  sio_inst_ptr: The sio_inst_ptr which the caller wants to map to RMSM
  instance

RETURN VALUE
  -1: In case of error
  instance: The RMSM instance corr. to the supplied sio_inst_ptr, otherwise


DEPENDENCIES
  ds_rmsm_init() should have been called first.

SIDE EFFECTS
  None
===========================================================================*/
ds_rmsm_sm_instance_type  ds_rmsmi_get_instance_num_from_sio_inst
(
  ds_rmsm_sio_instance* sio_inst
);

/*===========================================================================
FUNCTION   DS_RMSMI_GET_RMSMI_INFO_PTR

DESCRIPTION
  Returns pointer to the Rm Control Block.
  
NOTE: THIS IS A RESTRICTED FUNCTION AND IS INTENDED TO BE USED BY 
DS_RMSM MODULES ONLY

PARAMETERS
  rm_dev_instance - Rm Device Instance

RETURN VALUE
  Pointer to the RMSMI info structure.

DEPENDENCIES
  ds_rmsm_init() should have been called first.

SIDE EFFECTS
  None
===========================================================================*/
ds_rmsmi_info_type*  ds_rmsmi_get_rmsmi_info_ptr
(
  ds_rmsm_rm_dev_instance_type rm_dev_instance
);

/*===========================================================================
FUNCTION   DS_RMSMI_GET_SM_INFO_PTR

DESCRIPTION
  Returns pointer to the SM specific Control Block.
  
NOTE: THIS IS A RESTRICTED FUNCTION AND IS INTENDED TO BE USED BY 
DS_RMSM MODULES ONLY

PARAMETERS 
  inst_ptr - Instance of the Rm Device and the State Machine Instance

RETURN VALUE
  Pointer to the RMSMI info structure.

DEPENDENCIES
  ds_rmsm_init() should have been called first.

SIDE EFFECTS
  None
===========================================================================*/
ds_rmsmi_sm_info_type*  ds_rmsmi_get_sm_info_ptr
(
  ds_rmsm_instance_type* inst_ptr
);

/*===========================================================================
FUNCTION   DS_RMSMI_SIOLIB_DEREGISTER_CALLBACKS()

DESCRIPTION
  De reregisters the callbacks registered with SIO lib
  
NOTE: THIS IS A RESTRICTED FUNCTION AND IS INTENDED TO BE USED BY 
DS_RMSM MODULES ONLY

PARAMETERS
  Port Id for which the callbacks need to be deregistered

RETURN VALUE
  none
  
DEPENDENCIES
  ds_rmsm_init() should have been called first.

SIDE EFFECTS
  None
===========================================================================*/
void  ds_rmsmi_siolib_deregister_callbacks
(
  ds3g_siolib_port_e_type port_id
);

/*===========================================================================
FUNCTION DS_RMSMI_SET_SIO_PORT_ID

DESCRIPTION
  This function is used to set the port id corresponding to an Rm Device.

DEPENDENCIES
   None
  
PARAMETERS
  Rm Dev instance
  Port Id
  
RETURN VALUE
   None
    
SIDE EFFECTS
   None

===========================================================================*/
void 
ds_rmsmi_set_sio_port_id
(
  ds_rmsm_rm_dev_instance_type rm_dev,
  ds3g_siolib_port_e_type          port_id
);

/*===========================================================================
FUNCTION DS_RMSMI_GET_SIO_PORT_ID

DESCRIPTION
  This function is used to get the port id corresponding to an Rm Device.

DEPENDENCIES
   None
  
PARAMETERS
  Rm Dev Instance

RETURN VALUE
   Port Id
    
SIDE EFFECTS
   None

===========================================================================*/
ds3g_siolib_port_e_type ds_rmsmi_get_sio_port_id
(
  ds_rmsm_rm_dev_instance_type rm_dev
);

/*===========================================================================
FUNCTION DS_RMSMI_GET_PPP_DEV

DESCRIPTION
  This function is used to map Rm Dev to the PPP Device

DEPENDENCIES
   None
  
PARAMETERS
  Rm Dev Instance

RETURN VALUE
   PPP Device
    
SIDE EFFECTS
   None

===========================================================================*/
ppp_dev_enum_type ds_rmsmi_get_ppp_dev
(
  ds_rmsm_rm_dev_instance_type rm_dev
);

/*===========================================================================
  FUNCTION DS_RMSMI_GET_RM_DEV_FROM_PPP_DEV

  DESCRIPTION
    This function returns the Rm Device instance corresponding to a
    PPP device.
    
  PARAMETERS
    PPP Device

  RETURN VALUE
    Rm Device Instance    

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
ds_rmsm_rm_dev_instance_type ds_rmsmi_get_rm_dev_from_ppp_dev
(
  ppp_dev_enum_type ppp_dev
);


/*===========================================================================
FUNCTION DS_RMSMI_VALIDATE_RM_DEV_INSTANCE

DESCRIPTION
  This function validates the given Rm Dev instance

DEPENDENCIES
   None
  
PARAMETERS
  Rm Device instance

RETURN VALUE
  TRUE or FALSE depending on whether Rm Device instance is valid
    
SIDE EFFECTS
   None

===========================================================================*/
boolean ds_rmsmi_validate_rm_dev_instance
(
  ds_rmsm_rm_dev_instance_type rm_dev
);

/*===========================================================================
FUNCTION DS_RMSMI_VALIDATE_PPP_DEV

DESCRIPTION
  This function validates the given Rm Dev instance

DEPENDENCIES
   None
  
PARAMETERS
  Rm Device instance

RETURN VALUE
  TRUE or FALSE depending on whether PPP Device is valid
    
SIDE EFFECTS
   None

===========================================================================*/
boolean ds_rmsmi_validate_ppp_dev
(
  ppp_dev_enum_type ppp_dev
);

/*============================================================================
  FUNCTION DS_RMSMI_GET_NEXT_RM_DEV_INSTANCE

  DESCRIPTION
    This function fetches the next Rm Dev Instance to be use for DUN calls.
    
  PARAMETERS
    void

  RETURN VALUE
    Next Rm Dev Instance

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
ds_rmsm_rm_dev_instance_type ds_rmsmi_get_next_rm_dev_instance
(
  void
);

/*============================================================================
  FUNCTION DS_RMSMI_GET_COMPANION_SM_PTR
  DESCRIPTION
    This function fetches the next Rm Dev Instance to be use for DUN calls.
    
  PARAMETERS
    void

  RETURN VALUE
    Next Rm Dev Instance

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
ds_rmsmi_sm_info_type* ds_rmsmi_get_companion_sm_ptr
(
  ds_rmsm_instance_type* inst_ptr
);

/*============================================================================
  FUNCTION DS_GET_ADDR_FAMILY_FROM_SM_INST

  DESCRIPTION
    This function fetches address family from sm instance type
    
  PARAMETERS
    void

  RETURN VALUE
    Next Rm Dev Instance

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
ip_addr_enum_type ds_get_addr_family_from_sm_inst
(
  ds_rmsm_sm_instance_type sm_instance
);

/*===========================================================================

FUNCTION DS_RMSM_PDN_EVENT_CBACK

DESCRIPTION   
         
DEPENDENCIES
   None
  
RETURN VALUE
   None
    
SIDE EFFECTS
   None

===========================================================================*/
void ds_rmsm_pdn_event_cback
(
  ds_fwk_clnt_handle_type    client_handle,
  ds_pdn_event_enum_type     event,
  ds_pdn_event_info_u_type   event_info,
  void                      *user_data_ptr
);


/*===========================================================================

FUNCTION DS_RMSM_GET_INST_FROM_CLIENT_HANDLE

DESCRIPTION
         
DEPENDENCIES
   None
  
RETURN VALUE
   None
    
SIDE EFFECTS
   None

===========================================================================*/
void ds_rmsm_get_inst_from_client_handle
(
  int client_hdl,
  ds_rmsm_instance_type* inst_ptr
);

/*===========================================================================

FUNCTION DS_RMSM_PROCESS_TECH_SUSPENSION

DESCRIPTION   
         
DEPENDENCIES
   None
  
RETURN VALUE
   None
    
SIDE EFFECTS
   None

===========================================================================*/
void ds_rmsm_process_tech_suspension
(
  ds_rmsm_instance_type* inst_ptr,
  boolean tech_suspension_status
);

/*===========================================================================

FUNCTION DS_RMSM_RM_ENABLE_FLOW

DESCRIPTION   
         
DEPENDENCIES
   None
  
RETURN VALUE
   None
    
SIDE EFFECTS
   None

===========================================================================*/
void ds_rmsm_rm_enable_flow(ds_rmsmi_sm_info_type *sm_info_ptr);

/*===========================================================================

FUNCTION DS_RMSM_RM_DISABLE_FLOW

DESCRIPTION   
         
DEPENDENCIES
   None
  
RETURN VALUE
   None
    
SIDE EFFECTS
   None

===========================================================================*/
void ds_rmsm_rm_disable_flow(ds_rmsmi_sm_info_type *sm_info_ptr);

/*===========================================================================

FUNCTION DS_RMSM_ALLOCATE_CLIENT_HANDLE
DESCRIPTION   
         
DEPENDENCIES
   None
  
RETURN VALUE
   None
    
SIDE EFFECTS
   None

===========================================================================*/

int8 ds_rmsm_alloc_client_handle
(
  ds_rmsm_instance_type* inst_ptr
);

/*===========================================================================

FUNCTION DS_RMSM_BRINGUP_PDN

DESCRIPTION
         
DEPENDENCIES
   None
  
RETURN VALUE
   None
    
SIDE EFFECTS
   None

===========================================================================*/
int ds_rmsm_bringup_pdn
(
  ds_rmsmi_sm_info_type *sm_ptr
);

/*===========================================================================

FUNCTION DS_BRINGDOWN_PDN

DESCRIPTION   
         
DEPENDENCIES
   None
  
RETURN VALUE
   None
    
SIDE EFFECTS
   None

===========================================================================*/
int ds_rmsm_bringdown_pdn
(
  ds_rmsmi_sm_info_type *sm_ptr
);

/*===========================================================================

FUNCTION DS_RMSMI_FREE_RMSM_INSTANCE

DESCRIPTION   
         
DEPENDENCIES
   None
  
RETURN VALUE
   None
    
SIDE EFFECTS
   None

===========================================================================*/

void ds_rmsmi_free_rmsm_instance
(
  ds_rmsm_rm_dev_instance_type rm_dev
);

/*===========================================================================
  FUNCTION DS_RMSM_INIT()

  DESCRIPTION
    This function initializes the UMTS RM state machine. Once initialized, the 
    state machine manages SIO iface for packet data calls on an appropriate 
    Um iface. As part of initialization it creates SIO iface, init 
    queues/wms, allocates event buffers and resets state variables.
   
    It also calls the pdp-ip and pdp-ppp specific sub state machine
    initializations.
    
  PARAMETERS
    None
    
  RETURN VALUE
    TRUE on success
    FALSE on failure

  DEPENDENCIES
    Should only be called once at powerup

  SIDE EFFECTS
    None
===========================================================================*/
boolean ds_rmsm_init( void );

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_AT_PROCESS_CMD()

  DESCRIPTION
    This function receives a cmd posted to DS_RMSM AT interface in DS task 
    and passes it to the appropriate handling.

  RETURN VALUE
    ds_cmd_ptr: Cmd buffer passed with the cmd

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

void ds_3gpp_rmsm_at_process_cmd
(
  ds_command_type *ds_cmd_ptr
);

/*===========================================================================
  FUNCTION DS_RMSM_DIAL_STR_CB()

  DESCRIPTION
    This function handles the dial stings for pkt data calls and initiates 
    the data call over the appropriate pkt iface.  This function is 
    registered with ATCOP ATD handler table.
    
    It queries the pdp profile registry to get the profile associated with
    the context number specified in the dial strig. Based on that it sets
    the "sub-state" to either "pdp-ip" or "pdp-ppp" and kick starts the 
    state machine

  PARAMETERS
    dial_string: The dial string to be processed
    digit_mode: Flag to tell whether dial string contains digits only

  RETURN VALUE
    Function pointer to abort handler

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
dsat_call_abort_cb_type ds_rmsm_dial_str_cb
(
  const byte *dial_string,                        /* dial string           */
  boolean    digit_mode,                          /* non-digit or digits   */
  dsat_dial_modifier_info_type  *modifiers /* Dial sting modifiers  */
);

/*===========================================================================
  FUNCTION DS_RMSMI_GET_RM_DEV_FROM_FW_INST

  DESCRIPTION
    This function returns the Rm Device instance corresponding to a
    passed FW instance
    
  PARAMETERS
    FW instance

  RETURN VALUE
    Rm Device Instance    

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
ds_rmsm_rm_dev_instance_type ds_rmsmi_get_rm_dev_from_fw_inst
(
  ds_fwk_s_type *fw_inst
);

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */

#endif /* DS_RMSMI_H */
