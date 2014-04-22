#ifndef PS_UTILS_DEFS_H
#define PS_UTILS_DEFS_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                           P S _ U T I L S _ DEFS. H

GENERAL DESCRIPTION
  Collection of utility variables being used by various modules in PS.


Copyright (c) 2014 - 2019 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_utils.h_v   1.0   08 Aug 2002 11:19:58   akhare  $
  $Header: //components/rel/dataiot.mpss/2.0/interface/utils/inc/ps_utils_defs.h#2 $ $DateTime: 2023/01/31 04:42:27 $ $Author: pwbldsvc $

when      who     what, where, why
-------   ---     ---------------------------------------------------------- 
10/10/14  ssh     IP Translation Engine 464XLAT 
09/02/14  cx      Add NV configuration for ICMP6 modem iid filter
04/09/14  ash     Added Prefix Delegation support.
01/19/14  cx      Support for unused external addresses probe optimization
11/22/13  ds      Added the item file path for icmp reply rate parameters.
11/12/12  jjw     Initial version of file


===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
/** -------------------------------------------------------------------------
  EFS config path to be written to EFS.
---------------------------------------------------------------------------*/
static const char *ps_nv_conf_efs_path_array[] = 
{
  "/nv/item_files/datacommon/utils/aka_auth_info",
  "/nv/item_files/datacommon/utils/aka_in_sw",
  "/nv/item_files/datacommon/utils/is_fmc_enabled",
  "/nv/item_files/datacommon/utils/is_at_kdf_in_aka_prime_sync_fail_enabled",
  "/nv/item_files/datamodem/protocols/ip6_default_mtu",
  "/nv/item_files/datamodem/protocols/ip6_include_rdnss_opt",
  "/nv/item_files/data/protocols/icmp_reply_rate",
  "/nv/item_files/datamodem/protocols/ipv6_ext_addr_probe_config_items",
  "/nv/item_files/datamodem/protocols/dhcp6_prefix_delegation_enable",
  "/nv/item_files/datamodem/protocols/dhcp6_client_config",
  "/nv/item_files/datamodem/protocols/icmp6_mdm_iid_fltr_enable",
  "/nv/item_files/datamodem/protocols/clat_config_info",
  "/nv/item_files/datamodem/protocols/secssl_alert_config",
  "/nv/item_files/datamodem/protocols/ntn_tcp_timer"
};

/** -------------------------------------------------------------------------
  NV item enum indexes corresponding to EFS config path.s
---------------------------------------------------------------------------*/
typedef enum 
{
  PS_NV_CONF_AKA_SW_AUTH
  ,PS_NV_CONF_AKA_IN_SW
  ,PS_NV_CONF_IS_FMC_ENABLED
  ,PS_NV_CONF_AT_KDF_IN_SYNC
  ,PS_NV_CONF_IPV6_DEFAULT_MTU
  ,PS_NV_CONF_IPV6_INCLUDE_RDNSS_OPT
  ,PS_NV_CONF_ICMP_REPLY_RATE
  ,PS_NV_CONF_IPV6_EXT_ADDR_PROBE_CONFIG
  ,PS_NV_CONF_DHCP6_PREFIX_DELEGATION_ENABLE
  ,PS_NV_CONF_DHCP6_CLIENT_CONFIG
  ,PS_NV_CONF_ICMP6_MDM_IID_FLTR_ENABLE
  ,PS_NV_CONF_CLAT_CONFIG_INFO
  ,PS_NV_CONF_SECSSL_ALERT_CONFIG
  ,PS_NV_CONF_ntn_tcp_timer
} ps_nv_conf_enum_type;

/** -------------------------------------------------------------------------
  Accessor macros to access config item from config item array
---------------------------------------------------------------------------*/
#define PS_NV_CONF_ITEM_INFO(item) (ps_nv_conf_efs_path_array[(int)item])

#ifdef __cplusplus
}
#endif

#endif /* PS_UTILS_DEFS_H */
