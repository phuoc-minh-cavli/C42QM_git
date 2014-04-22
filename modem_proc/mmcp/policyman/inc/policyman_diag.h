#ifndef _POLICYMAN_DIAG_H_
#define _POLICYMAN_DIAG_H_

/**
  @file policyman_diag.h

  @brief Policy Manager diag message definitions.
*/

/*
    Copyright (c) 2014-2017 QUALCOMM Technologies Incorporated.
    All Rights Reserved.
    Qualcomm Technologies Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by this
  document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

  $Header: //components/rel/mmcp.mpss/6.1.10/policyman/inc/policyman_diag.h#1 $
  $DateTime: 2023/04/25 14:39:18 $
  $Author: pwbldsvc $
*/

#include <sys/types.h>
#include "comdef.h"    /* Definition for basic types and macros */
#include "log.h"
#include "event_defs.h"
#include "log_codes.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

#if defined(T_WINNT) || defined (FEATURE_WINCE)
#error code not present
#endif

/*---------------------------------------------------------------------------
  EVENT_PM_RAT_CHANGE structure
---------------------------------------------------------------------------*/

// Event definition EVENT_PM_RAT_CHANGE  0xA72==2674
// Comes from: core/api/services/event_defs.h
//#define EVENT_PM_RAT_CHANGE  0xA72

typedef PACK(struct) {
  uint32   ratMask;
  int8     subs;     // maps to sys_modem_as_id_e_type
} policyman_diag_event_rat_cap_t;

/*---------------------------------------------------------------------------
  LOG_PM_CONFIG_INFO structure
---------------------------------------------------------------------------*/
#define LOG_PM_CONFIG_INFO_LOG_PACKET_VERSION   2

// Log definition LOG_PM_CONFIG_INFO_C
// Comes from: core/api/services/log_codes.h
//#define LOG_PM_CONFIG_INFO_C   0x18A9

typedef PACK(struct) {
  uint8  subs_feature;         // sys_subs_feature_t
  uint8  reserved_1;
  uint8  reserved_2;
  uint8  reserved_3;
  uint32 rat_info;
} policyman_diag_subs_cfg_info_t;

// Define log record: LOG_PM_CONFIG_INFO_type
typedef PACK(struct) {
  log_hdr_type                    hdr;             // Required first, do not access directly (192 bits)
  /* 64-bit align */
  uint32                          version;         // Structure version, required first by diag after header
  uint32                          changed_mask;
  /* 64-bit align */
  uint8                           num_sim_cfg;     // MSB: cfgs:4
                                                   // LSB: sims:4
  uint8                           max_active;      // MSB: data:4
                                                   // LSB: voice:4
  uint8                           modified;        // MSB: reserved:4
                                                   //  |   max_data:1
                                                   //  |   feature:1
                                                   //  |   subs:1
                                                   // LSB: sims:1
  uint8                           specialization;  // sys_specialization_t
  uint32                          lte_band_count;  // number of lte band groups filled in below (1 or 4)
  /* 64-bit align */
  uint64                          hw_gw_bands;     // sys_band_mask_type
  uint64                          hw_lte_bands_0;  // sys_band_mask_type
  uint64                          hw_lte_bands_1;  // sys_band_mask_type
  uint64                          hw_lte_bands_2;  // sys_band_mask_type
  uint64                          hw_lte_bands_3;  // sys_band_mask_type
  uint64                          hw_tds_bands;    // sys_band_mask_type
  uint32                          hw_rat_mask;
  policyman_diag_subs_cfg_info_t  subs_cfg_info[];
} LOG_PM_CONFIG_INFO_type;


/*---------------------------------------------------------------------------
  LOG_PM_SUBS_INFO structure
---------------------------------------------------------------------------*/
#define LOG_PM_SUBS_INFO_LOG_PACKET_VERSION   2

// Log definition LOG_PM_SUBS_INFO_C
// Comes from: core/api/services/log_codes.h
//#define LOG_PM_SUBS_INFO     0x18AA

// Mask for LOG_PM_SUBS_INFO:changed_mask
#define PM_SUBS_INFO_CHANGED_ITEM_1_MASK      BM(POLICYMAN_ITEM_DEVICE_CONFIGURATION)
#define PM_SUBS_INFO_CHANGED_ITEM_2_MASK      BM(POLICYMAN_ITEM_RAT_CAPABILITY)
#define PM_SUBS_INFO_CHANGED_ITEM_3_MASK      BM(POLICYMAN_ITEM_RF_BANDS)
#define PM_SUBS_INFO_CHANGED_ITEM_4_MASK      BM(POLICYMAN_ITEM_SVC_MODE)

// Mask for LOG_PM_SUBS_INFO:flags
#define PM_SUBS_INFO_FLAGS_DOMAIN_PREF_MANDATORY_MASK   0x01
#define PM_SUBS_INFO_FLAGS_RF_FORCE_REGISTRATION_MASK   0x02

// Define log record: LOG_PM_SUBS_INFO_type
typedef PACK(struct) {
  log_hdr_type                    hdr;             // Required first, do not access directly (192 bits)
  /* 64-bit align */
  uint32                          version;         // Structure version, required first by diag
  uint32                          changed_mask;    // Bitmask of changed items: BM(item_id)
  /* 64-bit align */
  uint8                           subs;            // sys_modem_as_id_e_type
  int8                            call_mode;       // sys_call_mode_t
  uint8                           svc_mode;        // policyman_svc_mode_t
  int8                            ue_mode;         // sys_ue_mode_e_type
  uint32                          lte_band_count;  // number of lte band groups filled in below (1 or 4)
  /* 64-bit align */
  uint64                          gw_bands;
  uint64                          lte_bands_0;
  uint64                          lte_bands_1;
  uint64                          lte_bands_2;
  uint64                          lte_bands_3;
  uint64                          tds_bands;
  uint32                          rat_mask;
  uint8                           flags;           // MSB: reserved:6
                                                   //  |   rf_force_registration:1
                                                   // LSB: domain_pref_mandatory:1
  int8                            voice_dom_pref;  // sys_voice_domain_pref_e_type
  int8                            feature0;
  int8                            rat_order_table_len;   // max number of entries in rat_order_entries[] below
  uint16                          num_rat_order_entries;
  uint8                           rat_order_entries[10];
} LOG_PM_SUBS_INFO_type;

#if defined(T_WINNT) || defined (FEATURE_WINCE)
#error code not present
#endif

#endif /* _POLICYMAN_DIAG_H_ */
