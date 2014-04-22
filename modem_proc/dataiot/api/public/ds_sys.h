/**
@file ds_sys.h
@brief
This file defines function, variables and data structures common to all PS
System API module
*/

#ifndef DS_SYS_H
#define DS_SYS_H

/*===========================================================================

                          D S  _ S Y S . H

DESCRIPTION

  Data Services System module interface file. This contains variables and
  definitions used by the various modules to access interface definitions.

Copyright (c) 2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  Copyright (c) 2018 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary.


  $Header: //components/rel/dataiot.mpss/2.0/api/public/ds_sys.h#5 $ $DateTime: 2023/05/09 08:36:53 $ $Author: pwbldsvc $

when        who    what, where, why
--------    ---    ----------------------------------------------------------


===========================================================================*/

#include "comdef.h"
#include "sys.h"

/*===========================================================================

                         EXTERNAL DATA DECLARATIONS

===========================================================================*/
#define DS_SYS_MAX_APN_LEN        101
#define DS_SYS_MAX_APNS           8 /**< Max number of APNs */

#define DS_SYS_MAX_AVAIL_SYS      15


/**
  @brief Technology type values
*/
typedef enum
{
  DS_SYS_TECH_MIN     = 0x00,
  DS_SYS_TECH_3GPP    = DS_SYS_TECH_MIN, /**< Tech type 3gpp */
  DS_SYS_TECH_ALL     = 0x01,            /**< All the tech types */
  DS_SYS_TECH_INVALID = 0xFF
} ds_sys_tech_enum_type; 


/**
  @brief This is the type that is used to different network types
*/
typedef enum
{
  DS_SYS_NETWORK_3GPP,     /**< Network type 3gpp */
  DS_SYS_NETWORK_MAX,
} ds_sys_network_enum_type;

/**
  @brief This is the type that is used to define core network details
*/
typedef struct
{
  uint32   rat_mask;  /**< RAT Mask */
  uint32   so_mask;   /**< SO Mask */
} ds_sys_network_info_type;

/**
  @brief This is the type that is used to define system status. It is
         common to both configuration DS_SYS_CONF_SYSTEM_STATUS and event
         DS_SYS_EVENT_SYSTEM_STATUS_CHANGE

  @see   DS_SYS_CONF_SYSTEM_STATUS
  @see   DS_SYS_EVENT_SYSTEM_STATUS_CHANGE
*/
typedef struct
{
  ds_sys_network_enum_type         preferred_network; /**< pref network value*/
  ds_sys_network_info_type         network_info[DS_SYS_NETWORK_MAX];
  /**< Array of rat and SO mask for all networks */
} ds_sys_network_status_type;

/**
  @brief This is the type that is used to  define different RATs types
*/
typedef enum
{
  DS_SYS_RAT_EX_NULL_BEARER            = 0,
  DS_SYS_RAT_EX_3GPP_WCDMA             = 1,
  DS_SYS_RAT_EX_3GPP_GERAN             = 2,//In accordance with the qmi_dsd_service
  DS_SYS_RAT_EX_3GPP_LTE               = 3,
  DS_SYS_RAT_3GPP_EDGE                 = 4,
  DS_SYS_RAT_3GPP_GPRS                 = 5,
  DS_SYS_RAT_MAX,
} ds_sys_rat_ex_enum_type;

/**
  @brief This is the type that is used to  define different subscription types
         Unknown subs id is used when the client does not care/provide the subs id
*/
typedef enum 
{
  DS_SYS_DEFAULT_SUBS      = 0x0000, /** < Default Data Subscription */
  DS_SYS_PRIMARY_SUBS      = 0x0001, /**< Primary */
  DS_SYS_SECONDARY_SUBS    = 0x0002, /**< Secondary */
  DS_SYS_TERTIARY_SUBS     = 0x0003,  /**< Tertiary */
  DS_SYS_SUBS_MAX,
} ds_sys_subscription_enum_type;


/* SO Mask has a size of 8 bytes */

/* SO Mask not available or not specified */
#define DS_SYS_SO_EX_UNSPECIFIED                    0x0
//Same as RAT enum
/* 3GPP SO Mask, it occupies the first 3 bytes. ie. bytes 0, 1 and 2 */
#define DS_SYS_SO_EX_3GPP_GPRS                      0x0000000000000080
#define DS_SYS_SO_EX_3GPP_EDGE                      0x0000000000000100
/* GSM so mask is depreciated. Can only be GPRS or EDGE */
#define DS_SYS_SO_EX_3GPP_GSM                       0x0000000000000200

/* LTE SO Mask */
#define DS_SYS_SO_EX_3GPP_LTE_LIMITED_SRVC          0x0000000000000800
#define DS_SYS_SO_EX_3GPP_LTE_FDD                   0x0000000000001000
#define DS_SYS_SO_EX_3GPP_LTE_TDD                   0x0000000000002000

/**
  @brief This is the type that is used to define system status. 
*/
typedef struct 
{
  ds_sys_network_enum_type                 technology; /**< technology type*/
  ds_sys_rat_ex_enum_type                  rat_value;  /**< rat type*/
  uint64                                   so_mask;   /**< so type*/
  sys_radio_access_tech_e_type             active_rat; /**< CATM1/NB1*/    
} ds_sys_status_type;

/**
  @brief  This is the type that defines apn to preferred network relationship. 
          For every APN, there would be tuple that describes its preferred 
          system. 
*/
typedef struct 
{
   char                                apn_name[DS_SYS_MAX_APN_LEN]; /**< PDN name*/
   uint32                              num_avail_sys; 
  /**< Number of valid entries to available system status array */
   ds_sys_status_type      avail_sys[DS_SYS_MAX_AVAIL_SYS];
  /**<  Array of available systems for the APN. The first element of the 
        array will contain the current preferred system for the APN.
        @see ds_sys_system_status_info_type 
        @see ds_sys_system_status_info_type */
   uint8                          recommended_pref_system_index;
  /**< Index in avail_sys array, which contains the 
       recommended preferred system for the APN */
} ds_sys_apn_pref_sys_info_type;
  
/**
  @brief This is the type that is used to define system status. It is
	   common to both configuration DS_SYS_CONF_SYSTEM_STATUS_EX and event
	   DS_SYS_EVENT_SYSTEM_STATUS_EX
*/
typedef struct 
{
	uint32	 num_avail_sys; 
	/**< Number of valid entries to available system status array */
  
	ds_sys_status_type	avail_sys[DS_SYS_MAX_AVAIL_SYS]; 
	/**< Array of current system status, the first element in the array will 
		 contain the current preferred system */
  
	uint32	 num_avail_apns;
	/**< Number of apn entries to pdn_info */
  
	ds_sys_apn_pref_sys_info_type apn_pref_sys_info[DS_SYS_MAX_APNS];	 
	/**< Array of all apns available */
  
	uint8						   recommended_pref_system_index;
	/**< Index in avail_sys array for the APN, which contains the 
		 recommended preferred system for the APN */
} ds_sys_system_status_type;

/**
  @brief Data type to store the apn name 
   
*/
typedef struct
{
  unsigned char     apn_name[DS_SYS_MAX_APN_LEN];
} ds_sys_apn_name_type;

/**
  @brief Data type to be used with roaming disallowed info
  is_roaming_disallowed : whether roaming is allowed or disallowed
                          on the apns
  num_apn_names         : No of apn names listed
  apn_list              : List of apn names 
   
  @see   DS_SYS_CONF_3GPP_ROAMING_DISALLOWED_INFO
  @see   DS_SYS_EVENT_3GPP_ROAMING_DISALLOWED_INFO  
*/
typedef struct
{
  boolean                 is_roaming_disallowed;
  uint8                   num_apn_names;
  ds_sys_apn_name_type    apn_list[DS_SYS_MAX_APNS];
} ds_sys_roaming_disallowed_info_type;

/**
  @brief Gives whether the UE is roaming or in the home network.
  roaming_status = TRUE  : UE is roaming
  roaming_status = FALSE : UE is in the home network   
  @see PS_SYS_EVENT_3GPP_ROAMING_STATUS
*/
typedef struct
{
  boolean   roaming_status;
} ds_sys_3gpp_roaming_status_type; 

/**
  @brief Defines structure for DS_SYS_IOCTL_3GPP_PDN_THROTTLE_INFO ioctl command
         and   DS_SYS_EVENT_3GPP_PDN_THROTTLE_INFO event
  
  @see DS_SYS_EVENT_3GPP_PDN_THROTTLE_INFO 
  @see DS_SYS_IOCTL_3GPP_PDN_THROTTLE_INFO 
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
  char       apn_string[DS_SYS_MAX_APN_LEN]; /**< PDN name */
  uint8      apn_string_len;                 /**< PDN name length */
} ds_sys_pdn_ctxt_throttle_info_type;

typedef struct
{
  ds_sys_pdn_ctxt_throttle_info_type pdn_throttle_info[DS_SYS_MAX_APNS];
  /**< For each apn, we have the values */
  uint8 num_apn; /**< Refers to the no. of valid array elements in 
                      pdn_throttle_info[] */
} ds_sys_pdn_throttle_info_type;

typedef enum
{
  DS_SYS_3GPP_DATA_DEFAULT,
  DS_SYS_3GPP_DATA_ACK,     /**< Network type 3gpp */
  DS_SYS_3GPP_DATA_NACK,
  DS_SYS_3GPP_DATA_MAX
} ds_sys_3gpp_ul_data_delivery_status_enum_type;

/**
  @brief Data type to be used with data delivery info
                         
  delivery_status       : status of data
  fwk_index             : index
  fwk_index             : index
   
  @see   DS_SYS_EVENT_3GPP_UL_DATA_DELIVERY_INFO 
*/
typedef struct
{
  ds_sys_3gpp_ul_data_delivery_status_enum_type delivery_status;
  uint8                                         fwk_index;
} ds_sys_3gpp_ul_data_delivery_ind_info_type;





/****************************************************************
adding structures to avoid compilation erros on gstk_exp_v.h and mmcp/api/public/sys.h
most of the errors are because of this files inclution.
************************************************************/
#define DS_SYS_MAC_ADDR_LEN       6
#define DS_SYS_MAX_SSID_LEN       33




typedef struct
{
  uint8            ap_mac_addr[DS_SYS_MAC_ADDR_LEN]; 
  /**< WLAN Access Point MAC addr*/
} ds_sys_wlan_mac_addr_type;

/**
  @brief This is the type that is used for 
         DS_SYS_CONF_GET_WLAN_SSID configuration and
         DS_SYS_EVENT_WLAN_SSID_CHANGE
   
  @see DS_SYS_CONF_GET_WLAN_SSID
  @see DS_SYS_EVENT_WLAN_SSID_CHANGE 
*/
typedef struct
{
  char            ssid[DS_SYS_MAX_SSID_LEN]; 
  /**< WLAN Service Set Identifier*/
} ds_sys_wlan_ssid_type;

/**
  @brief Data type to be used with msisdn info
  msisdn_length          : length of msisdn information
  msisdn                 : msisdn information.
   
  @see ds_sys_event_msisdn_info_type 
*/

typedef struct
{
  uint8        msisdn_length; /* MSISDN length */
  uint8        msisdn[255];   /* MSISDN */
} ds_sys_msisdn_info_type;

/**
  @brief Data type to be used with msisdn info
  apn_name               : APN name
  resolved_apn_name      : Resolved APN name
  msisdn_info            : msisdn info of type ds_sys_msisdn_info_type.
   
  @see   DS_SYS_EVENT_APN_MSISDN_INFO_CHANGE_EV 
*/

typedef struct
{
  char                          apn_name[DS_SYS_MAX_APN_LEN];
  char                          resolved_apn_name[DS_SYS_MAX_APN_LEN];
  ds_sys_msisdn_info_type       msisdn_info;
}ds_sys_event_msisdn_info_type;

/**
  @brief Define structure for IOCTL DS_SYS_IOCTL_GET_APN_MSISDN_INFO
  apn_name               : APN name
  msisdn_info            : msisdn info of type ds_sys_msisdn_info_type.
   
  @see   DS_SYS_IOCTL_GET_APN_MSISDN_INFO 
*/

typedef struct
{
  char                          apn_name[DS_SYS_MAX_APN_LEN];
  ds_sys_msisdn_info_type       msisdn_info;
}ds_sys_ioctl_msisdn_info_type;

typedef struct 
{
  boolean rai_val;
}ds_sys_ioctl_l2_rai_info;

#endif /* DS_SYS_H */

