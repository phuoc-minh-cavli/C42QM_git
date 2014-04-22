/**
@file ps_sys.h
@brief
This file defines function, variables and data structures common to all PS 
System API module 
*/

#ifndef PS_SYS_H
#define PS_SYS_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        P S _ S Y S . H


GENERAL DESCRIPTION
  This file defines function, variables and data structures common to all
  PS System API module

Copyright (c) 2011-2016 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/dataiot.mpss/2.0/interface/utils/inc/ps_sys.h#1 $ $DateTime: 2020/03/17 08:03:16 $ $Author: pwbldsvc $
 
when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
03/28/16    skc    Changed number of max pdn from 8 to 4 for 9X07 build
03/29/13    svj    New APIs to modify and query attach pdn list 
09/09/11    bvd    Created module 
 
===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"
#include "queue.h"
#include "ds_sys.h"
#include "ps_in.h"

 /**
   Defines the MAX length for PDN/APN name string
*/
#define PS_SYS_MAX_APN_LEN        DS_SYS_MAX_APN_LEN
#define PS_SYS_MAX_APNS           DS_SYS_MAX_APNS /**< Max number of APNs */
#define PS_SYS_MAX_AVAIL_SYS      DS_SYS_MAX_AVAIL_SYS
#define PS_SYS_MAX_REMOTE_SOCKETS         255




/* For low tier architecture like triton, only 6 pdns are supported */
#ifdef FEATURE_DATA_LOW_MEM
  #define PS_SYS_MAX_AVAIL_PDNS 4
#elif defined (FEATURE_DATA_MPSS_ULT)
  #define PS_SYS_MAX_AVAIL_PDNS 6
#else 
  #define PS_SYS_MAX_AVAIL_PDNS 8
#endif /* FEATURE_DATA_LOW_MEM */

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                         PUBLIC DATA DECLARATIONS

===========================================================================*/

/**
  @brief Defines different Technology type values
*/
typedef enum
{
  PS_SYS_TECH_MIN     = DS_SYS_TECH_MIN,
  PS_SYS_TECH_3GPP    = DS_SYS_TECH_3GPP,  /**< Tech type 3gpp */
  PS_SYS_TECH_ALL     = DS_SYS_TECH_ALL,   /**< All the tech types */
  PS_SYS_TECH_INVALID = DS_SYS_TECH_INVALID
}ps_sys_tech_enum_type;


/**
  @brief This is the type that is used to define different network types
*/
typedef enum
{
  PS_SYS_NETWORK_3GPP,  /**< Network type 3gpp */
  PS_SYS_NETWORK_MAX,
} ps_sys_network_enum_type;


/**
  @brief This is the type that is used to define core network details 
  @see   ds_sys_network_info_type 
*/
typedef struct
{
  uint32   rat_mask;   /**< RAT Mask */
  uint32   so_mask;    /**< SO Mask */
} ps_sys_network_info_type;

/**
  @brief This is the type that is used to define system status. It is 
         common to both configuration PS_SYS_CONF_SYSTEM_STATUS and event
         PS_SYS_EVENT_SYSTEM_STATUS_CHANGE
   
  @see PS_SYS_CONF_SYSTEM_STATUS
  @see PS_SYS_EVENT_SYSTEM_STATUS_CHANGE 
  @see ds_sys_system_status_type 
*/
typedef struct
{
  ps_sys_network_enum_type        preferred_network; /**< pref network value*/
  ps_sys_network_info_type        network_info[PS_SYS_NETWORK_MAX];
  /**< Array of rat and SO mask for all networks */
} ps_sys_system_status_type;


/**
  @brief Defines structure for PS_SYS_IOCTL_PDN_THROTTLE_INFO ioctl command and
         PS_SYS_EVENT_PDN_THROTTLE_INFO event
  
  @see PS_SYS_EVENT_PDN_THROTTLE_INFO 
  @see PS_SYS_IOCTL_PDN_THROTTLE_INFO 
*/
typedef struct
{
  boolean    is_ipv4_throttled;             /**< ipv4 is throttled */
  boolean    is_ipv6_throttled;             /**< ipv6 is throttled */
  boolean    is_non_ip_throttled;           /**< non_ip is throttled */
  uint32     remaining_ipv4_throttled_time; /**< ipv4 remaining time in 
                                                 miliseconds */ 
  uint32     remaining_ipv6_throttled_time; /**< ipv6 remaining time in 
                                                 miliseconds */
  uint32     remaining_non_ip_throttled_time; /**< non ip remaining time in 
                                                  miliseconds */
  char       apn_string[PS_SYS_MAX_APN_LEN]; /**< PDN name */
  uint8      apn_string_len;                 /**< PDN name length */
} ps_sys_pdn_ctxt_throttle_info_type;

typedef struct
{
  ps_sys_pdn_ctxt_throttle_info_type pdn_throttle_info[PS_SYS_MAX_APNS];
  /**< For each internet, we have the values ie max apns */

  uint8 num_apn; /**< Refers to the no. of valid array elements in 
                      pdn_throttle_info[] */
  boolean  is_epc_ho_throttle; /**< This flag indicates whether the 
                                    throttle cmd is invoked by EPC  
                                    or mode handler */
} ps_sys_pdn_throttle_info_type;

/**
  @brief Defines the pdn status
*/
typedef enum
{
  PS_SYS_PDN_DOWN,
  PS_SYS_PDN_UP, 
} ps_sys_pdn_status_enum_type;

/**
  @brief Defines structure for PS_SYS_EVENT_PDN_STATUS_INFO event
  pdn_status     : pdn is up or down
  ip_type        : IP address type
  apn_string     : apn name
  apn_string_len : length of the apn name
  @see PS_SYS_EVENT_PDN_STATUS_INFO 
*/
typedef struct
{
  ps_sys_pdn_status_enum_type  pdn_status; 
  ip_addr_enum_type            ip_type;                     
  uint16                       apn_string_len;                 
  char                         apn_string[PS_SYS_MAX_APN_LEN]; 
} ps_sys_pdn_status_info_type;

 /**
  @brief Gives whether the UE is roaming or in the home network.
  roaming_status = TRUE  : UE is roaming
  roaming_status = FALSE : UE is in the home network   
  @see PS_SYS_EVENT_3GPP_ROAMING_STATUS
*/
typedef struct
{
  boolean   roaming_status;
} ps_sys_3gpp_roaming_status_type;

/**
  @brief This is the type that is used to  define different RATs types
*/
typedef enum
{
  PS_SYS_RAT_EX_NULL_BEARER           = DS_SYS_RAT_EX_NULL_BEARER,

  /* 3GPP RAT Values */

  PS_SYS_RAT_EX_3GPP_GERAN             = DS_SYS_RAT_EX_3GPP_GERAN,
  PS_SYS_RAT_EX_3GPP_LTE               = DS_SYS_RAT_EX_3GPP_LTE,
  PS_SYS_RAT_MAX,
} ps_sys_rat_ex_enum_type;

/* SO Mask not available or not specified */
#define PS_SYS_SO_EX_UNSPECIFIED            DS_SYS_SO_EX_UNSPECIFIED

#define PS_SYS_SO_EX_3GPP_GPRS              DS_SYS_SO_EX_3GPP_GPRS
#define PS_SYS_SO_EX_3GPP_EDGE              DS_SYS_SO_EX_3GPP_EDGE

/* GSM so mask is depreciated. Can only be GPRS or EDGE */
#define PS_SYS_SO_EX_3GPP_GSM               DS_SYS_SO_EX_3GPP_GSM

 
#define PS_SYS_SO_EX_3GPP_LTE_LIMITED_SRVC  DS_SYS_SO_EX_3GPP_LTE_LIMITED_SRVC
#define PS_SYS_SO_EX_3GPP_LTE_FDD           DS_SYS_SO_EX_3GPP_LTE_FDD
#define PS_SYS_SO_EX_3GPP_LTE_TDD           DS_SYS_SO_EX_3GPP_LTE_TDD

 /**
  @brief This is the type that is used to  define different subscription types
         Unknown subs id is used when the client does not care/provide the subs id
*/
typedef enum 
{
  PS_SYS_DEFAULT_SUBS      = DS_SYS_DEFAULT_SUBS, /**< Default data subscription */
  PS_SYS_PRIMARY_SUBS      = DS_SYS_PRIMARY_SUBS, /**< Primary */
  PS_SYS_SECONDARY_SUBS    = DS_SYS_SECONDARY_SUBS, /**< Secondary */
  PS_SYS_TERTIARY_SUBS     = DS_SYS_TERTIARY_SUBS,  /**< Tertiary */  
  PS_SYS_SUBS_MAX          = DS_SYS_SUBS_MAX
} ps_sys_subscription_enum_type;

/**
  @brief This is the tupple that defines network, RAT, SO and service status
*/
typedef struct 
{
  ps_sys_network_enum_type                 technology; /**< technology type*/
  ps_sys_rat_ex_enum_type                  rat_value;  /**< rat type*/
  uint64                                   so_mask;   /**< so type*/
} ps_sys_system_status_info_type;

/**
  @brief  This is the type that defines apn to preferred network relationship. 
          For every APN, there would be tuple that describes its preferred 
          system.  
*/
typedef struct 
{
   char                           apn_name[PS_SYS_MAX_APN_LEN]; /**< PDN name*/
   uint32                         num_avail_sys; 
   /**< Number of valid entries to available system status array */
   ps_sys_system_status_info_type avail_sys[PS_SYS_MAX_AVAIL_SYS];  
  /**<  Array of available systems for the APN. The topmost element of the 
        array will convey the current preferred system for the APN.
        @see ps_sys_system_status_ex_type 
        @see ps_sys_system_status_info_type */
   uint8                          recommended_pref_system_index;
  /**< Index in avail_sys array, which contains the 
       recommended preferred system for the APN */
} ps_sys_apn_pref_sys_info_type;

/**
  @brief This is the type that is used to define system status. It is
         common to both configuration PS_SYS_CONF_SYSTEM_STATUS_EX and event
         PS_SYS_EVENT_SYSTEM_STATUS_EX
*/
typedef struct 
{
  uint32   num_avail_sys; 
  /**< Number of valid entries to available system status array */

  ps_sys_system_status_info_type  avail_sys[PS_SYS_MAX_AVAIL_SYS]; 
  /**< Array of current system status,  the topmost element in the array will 
       contain the current preferred system*/

  uint32   num_avail_apns;
  /**< Number of apn entries to pdn_info */

  ps_sys_apn_pref_sys_info_type apn_pref_sys_info[PS_SYS_MAX_APNS];    
  /**< Array of all apn available */

  uint8                          recommended_pref_system_index;
  /**< Index in avail_sys array for the APN, which contains the 
       recommended preferred system for the APN */
} ps_sys_system_status_ex_type;


/**
  @brief Data type to hold the apn name
*/
typedef struct
{
  unsigned char     apn_name[PS_SYS_MAX_APN_LEN];
} ps_sys_apn_name_type;

/**   
  @see ds_sys_roaming_disallowed_info_type
*/
typedef struct
{
  boolean                 is_roaming_disallowed;
  uint8                   num_apn_names;
  ps_sys_apn_name_type    apn_list[PS_SYS_MAX_APNS];
} ps_sys_roaming_disallowed_info_type;
 
/**
  @brief This is the type that is used for 
         PS_SYS_CONF_3GPP_LTE_ATTACH_TYPE configuration and
         PS_SYS_EVENT_3GPP_LTE_ATTACH_TYPE_CHANGED
   
  @see PS_SYS_CONF_3GPP_LTE_ATTACH_TYPE
  @see PS_SYS_EVENT_3GPP_LTE_ATTACH_TYPE_CHANGED 
*/
typedef enum
{
  PS_SYS_LTE_ATTACH_TYPE_INITIAL  = 0x00,
  PS_SYS_LTE_ATTACH_TYPE_HANDOFF  = 0x01
} ps_sys_lte_attach_enum_type;

/**
  @brief system status event payload for DIAG events
         EVENT_PS_SYSTEM_STATUS
         EVENT_PS_SYSTEM_STATUS_EX
         EVENT_QMI_SYSTEM_STATUS
         EVENT_QMI_SYSTEM_STATUS_EX
  */
typedef struct 
{
  ps_sys_network_enum_type       pref_network; 
  uint32                         rat_value;  
  uint64                         so_mask;
  ps_sys_subscription_enum_type  subs_id;   
} ps_sys_system_status_diag_event_logging_type;

/**
  @brief Traffic state enum type
  Note : Donot change the existing values of below enum type. 
         START : 1 (TRUE) and STOP : 0 (FALSE)
  */
typedef enum
{
  PS_SYS_LOW_LATENCY_TRAFFIC_STATE_STOP,
  PS_SYS_LOW_LATENCY_TRAFFIC_STATE_START,
  PS_SYS_LOW_LATENCY_TRAFFIC_STATE_CONTINUE,
  PS_SYS_LOW_LATENCY_TRAFFIC_STATE_MAX
} ps_sys_low_latency_traffic_state_enum_type;

/**
  @brief Low latency traffic status information. 
         Example : 
         1. If packet arrives at time T, then sleep for
         pkt_interval_time_in_ms - pkt_arrival_delta_in_ms. 
         2. Wait for next packet to arrive with in pkt_max_wait_time_in_ms.
         3. Once packet arrive. Goto step 1     
         4. If packet doesnt arrive then low latency traffic end indication 
            should have been sent         

  @see PS_SYS_EVENT_LOW_LATENCY_TRAFFIC_STATUS
*/
typedef struct
{
  /*! Traffic State */  
  ps_sys_low_latency_traffic_state_enum_type   traffic_state;
  
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
} ps_sys_low_latency_traffic_info_type;

 
/**
  @brief Data type to be used with PS_SYS_CONF_NULL_RESOLVED_APN
         and PS_SYS_EVENT_NULL_RESOLVED_APN
  apn_length       : Length of the apn name
  apn_name         : Apn name 
  */
typedef struct 
{
  uint8         apn_length;  
  char          apn_name[PS_SYS_MAX_APN_LEN];
} ps_sys_null_resolved_apn_info_type;



 
//Enumeration for tx mode levels
typedef enum {
  PS_SYS_MODEM_POWER_LEVEL_ONE,
  PS_SYS_MODEM_POWER_LEVEL_TWO,
  PS_SYS_MODEM_POWER_LEVEL_THREE,
  PS_SYS_MODEM_POWER_LEVEL_FOUR,
  PS_SYS_MODEM_POWER_LEVEL_FIVE,
  PS_SYS_MODEM_POWER_LEVEL_MAX
} ps_sys_modem_power_level_enum_type;

typedef struct {
  ps_sys_modem_power_level_enum_type  tx_mode_level;
  /**< Power levels in Tx(Transmitter) mode */
   
  uint32  tx_mode_duration;
  /**< Duration that modem was active on 
       Tx mode(Transmitter mode) in milliseconds 
  */
} ps_sys_tx_mode_type;

/**
  @brief Modem power stats config type

  @see PS_SYS_CONF_MODEM_POWER_STATISTICS 
  @see PS_SYS_EVENT_MODEM_POWER_STATISTICS 
*/
typedef struct
{
  uint32 idle_mode_duration;
  /**< The duration that modem was awake but RRC connection 
       was released in milliseconds
  */

  uint32 sleep_mode_duration;
  /**< Duration that modem was in power collapse in milliseconds
  */

  uint32 rx_mode_duration;
  /**< Duration that modem was active on Rx mode( Receiver mode) 
       in milliseconds
  */

  ps_sys_tx_mode_type tx_mode_duration[PS_SYS_MODEM_POWER_LEVEL_MAX];
  /**< Duration that modem was active on Tx mode(Transmitter mode)
       in milliseconds 
  */
} ps_sys_modem_power_stats_info_type;

 
/**
  @brief Data type that provides whether remote socket handling 
  is available on the TE or not. 
  remote_socket_hdlr_avail = TRUE : TE has registered for remote 
  socket socket handling indications 
  remote_socket_hdlr_avail = FALSE : TE has not registered for 
  remote socket socket handling indications 
  @see PS_SYS_CONF_REMOTE_SOCKET_HDLR_AVAIL 
  @see PS_SYS_EVENT_REMOTE_SOCKET_HDLR_CHANGE
*/
typedef struct
{
  boolean   remote_socket_hdlr_avail;
} ps_sys_remote_socket_hdlr_avail_type;

typedef struct
{
  ip_version_enum_type      ip_family;   //v4 or v6
  ps_ip_protocol_enum_type  xport_proto; //will only contain TCP or UDP
  uint16                    port_no;     //0 or specific port number
}ps_sys_remote_socket_type;

/**
  @brief Data type that contains parameters for remote socket 
         allocation request
  @see     PS_SYS_EVENT_REMOTE_SOCKET_REQUEST 
*/
typedef struct
{
  uint8                      num_sockets; //num of sockets in list
  ps_sys_remote_socket_type  socket_list[PS_SYS_MAX_REMOTE_SOCKETS]; 
}ps_sys_remote_socket_request_type;

typedef enum
{
  PS_SYS_REMOTE_SOCKET_ALLOC_SUCCESS,
  PS_SYS_REMOTE_SOCKET_ALLOC_IN_USE_FAILURE,
  PS_SYS_REMOTE_SOCKET_ALLOC_GENERAL_FAILURE
} ps_sys_remote_socket_alloc_status;

typedef struct
{
  ps_sys_remote_socket_alloc_status  status;   //SUCCESS or FAILURE
  uint32                             remote_socket_handle; //handle to socket on the TE
  boolean                            is_ephemeral_port; //whether original request was for ephemeral port
  ps_sys_remote_socket_type          socket_info; //Info related to socket
}ps_sys_remote_socket_allocated_info_type;

/**
  @brief Data type that contains parameters for remote socket 
         allocation information from the TE
  @see     PS_SYS_IOCTL_REMOTE_SOCKET_ALLOCATED 
*/
typedef struct
{
  uint8                                     num_sockets; //num of sockets in list
  ps_sys_remote_socket_allocated_info_type  socket_list[PS_SYS_MAX_REMOTE_SOCKETS]; 
}ps_sys_remote_socket_allocated_type;

/**
  @brief Data type that contains parameters for remote socket 
         release
  @see     PS_SYS_EVENT_REMOTE_SOCKET_RELEASE 
*/
typedef struct
{
  uint8   num_sockets; //num of sockets in list
  uint32  socket_list[PS_SYS_MAX_REMOTE_SOCKETS]; //List of handles to be released
}ps_sys_remote_socket_release_type;

/**
  @brief Data type that contains parameters for setting options 
         on a remote socket
  @see     PS_SYS_EVENT_REMOTE_SOCKET_SET_OPTION 
*/
typedef struct
{
  uint32  socket_handle; //Handle to socket on the TE
  boolean is_udp_encaps; // Is UDP encapsulated or not
}ps_sys_remote_socket_set_option_type;




/*===========================================================================

                      PUBLIC FUNCTION DEFINITIONS

===========================================================================*/

/** 
  @brief This function initializes data structures for ps_sys 
         module. It initializes event queues, ps sys critical section,
         and sets the command handler for event callback in PS task. It also
         allocates memory for holding temporary storage for config data
*/
void ps_sys_init
(
  void
);

/** 
  @brief This function returns technology type given rat value
*/
ps_sys_tech_enum_type ps_sys_get_tech_from_rat
(
  ps_sys_rat_ex_enum_type  rat
);

/** 
  @brief This function sets the DDS(Default data subscription) 
         in the system. Only Ds3g module can set this value
*/
void ps_sys_set_default_data_subscription
(
  ps_sys_subscription_enum_type  def_data_subs
);

/** 
  @brief This function gets the DDS(Default data subscription) 
         in the system
*/
ps_sys_subscription_enum_type ps_sys_get_default_data_subscription
(
  void
);


#ifdef __cplusplus
}
#endif

#endif /* PS_SYS_H */
