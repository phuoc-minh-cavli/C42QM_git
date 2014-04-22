#ifndef RFCOMMON_DATA_H
#define RFCOMMON_DATA_H

/*! 
  @file
  rfcommon_data.h
 
  @brief
  This file contains prototypes and definitions to be used by centralized 
  RF COMMON Global Data.
*/

/*==============================================================================

  Copyright (c) 2012 - 2018 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/rfa.mpss/3.10/rf/common/rf/core/inc/rfcommon_data.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
10/25/18   sk      TRACK B Bringup changes
09/24/18   mns     Device snapshot feature
10/05/18   mns     Device snapshot feature
08/06/14   aro     Added FBRx data structure and access interface 
                   in common data
11/11/13   kma     Add pin IQ capture type to support pin cal
01/30/14   jmf     Added XPT IQ capture type to internal state variable  
11/18/13   cd      Added debug flag for RFLM based Tx AGC override
07/17/13   jmf     [xPT] New Delay Table separate from DPD data module
04/15/13   Saul    Common. Consolidated XPT data into one structure.
04/15/13   Saul    Common. DPD APIs use critical section.
04/12/13   Saul    [Common] Use RF dispatch for fw responses.
03/19/13   aro     Added XPT CDMA cal deconfig
03/19/13   aro     Added call to XPT cal config from protocal IQ capture
03/19/20   aro     Added Data module hookup
03/05/13   aro     Enhanced XPT override command and migrated to FTM common
                   dispatch
02/04/13   aro     Migrated the variable to enable/disable slave resource
                   to RF common data
01/10/13   cri     Track allocated DPD tables 
11/19/12   aki     Added rfcommon_data_deinit() function
11/05/12   Saul    Common XPT Sample Capture. Support for capture and proc rsp.
10/19/12   Saul    COMMON XPT. Partitioned dpd data per tx path.
08/08/12   Saul    CDMA. Added code to profile writing dpd tables.
07/23/12   Saul    RF COMMON. MDSP/MSGR framework. Initial Revision.

==============================================================================*/

#include "comdef.h"
#include "rfcommon_mdsp_data.h"
#include "rfcommon_semaphore.h"
#include "rfcommon_fw_response.h"
#include "rfcommon_mdsp_types.h"
#include "rfcommon_core_xpt.h"
#include "rfcommon_core_xpt_buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Number of bytes that fit */
#define MAX_DPD_BANK_SIZE 1024
/* Number of tables that fit in a bank */
#define MAX_DPD_TBLS_PER_BANK MAX_DPD_BANK_SIZE/(64*2)
/* Number of banks allocated to a tx path */
#define MAX_DPD_BANKS 6 /* WARNING: If this val changes, search for refs in SW for 
                           calculations that depend on this val */

/*----------------------------------------------------------------------------*/
/*! Structure to keepm track of XPT override data */
typedef struct
{
  boolean override_en;
  /*!< Flag indicating if the override is enabled or not */

  xpt_cfg_type xpt_mode;
  /*!< XPT mode to be overridden to */
} rfcommon_xpt_override_data;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure Containing Common XPT Data used by RF COMMON Driver. 
*/ 
typedef struct
{
  /*! XPT Global Mutex */
  rf_lock_data_type xpt_crit_section;

  /*! Structure to store the XPT override data */
  rfcommon_xpt_override_data xpt_override;

} rfcommon_xpt_data_type;

#define RFA_SNAPSHOT_DEFAULT_RFFE_ADDR_RANGE                                0xFF
#define RFA_SNAPSHOT_MAX_DEFAULT_DEVICE_NUM                                 25
#define RFA_SNAPSHOT_MAX_CUSTOM_DEVICE_NUM                                 (2*RFA_SNAPSHOT_MAX_DEFAULT_DEVICE_NUM)

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure Containing RFFE Snapshot Data using device specific reg list. 
*/ 
typedef struct
{
  /*! RFFE BUS */
  uint8 bus;  

  /*! SLAVE ID */
  uint32 slave_id;

  /*! Size of data dump, dynamic */
  uint16 data_dump_size;

  /*! addr list */
  uint16* addr_list;

  /*! data dump, same size as addr list*/
  uint32* data_dump;
      
} rfcommon_rffe_snapshot_addr_list_data_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure Containing Device Snapshot Data used by RF COMMON Driver. 
*/ 


typedef struct
{
  /*! Initialized flag */
  boolean initialized;

  /*! Successful Dump Flag */
  boolean dump_successful;

  /*! RFC HW ID */
  rf_hw_type hw_id;
      
  /*! Number of RFFE devices using custom addr list */
  uint16 reg_list_phy_rffe_device_count;

  /*! RFFE dumps using custom addr list */
  rfcommon_rffe_snapshot_addr_list_data_type* reg_list_rffe_dump[RFA_SNAPSHOT_MAX_CUSTOM_DEVICE_NUM];

}rfcommon_device_snapshot_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure containing debug flags for RF COMMON driver. 
*/ 
typedef struct
{
  boolean use_new_fwrsp;
  boolean use_rflm_txagc_override;
} rfcommon_debug_flags_t;

/*----------------------------------------------------------------------------*/
extern rfcommon_debug_flags_t rfcommon_debug_flags;

/*----------------------------------------------------------------------------*/
rfcommon_mdsp_data_type *
rfcommon_get_mdsp_data
(
  void
);

/*----------------------------------------------------------------------------*/
rfcommon_semaphore_data_type *
rfcommon_get_semaphore_data
(
  void
);

/*----------------------------------------------------------------------------*/
rfcommon_fw_response_data_type *
rfcommon_get_fw_response_data
(
  void
);

/*----------------------------------------------------------------------------*/
rf_lock_data_type *
rfcommon_get_xpt_lock_data
(
  void
);

/*----------------------------------------------------------------------------*/
boolean
rfcommon_configure_xpt_override
(
  rfm_device_enum_type device,
  boolean override_en,
  xpt_cfg_type xpt_mode
);

/*----------------------------------------------------------------------------*/
boolean
rfcommon_is_xpt_override
(
  rfm_device_enum_type device,
  xpt_cfg_type *xpt_mode
);

/*------------------------------------------------------------------------------------------------*/
rfcommon_device_snapshot_type* 
rfcommon_get_snapshot_data
(
   void
);
/*------------------------------------------------------------------------------------------------*/


typedef struct
{
  /*! Data structure to hold all RF COMMON MDSP data */
  rfcommon_mdsp_data_type mdsp;

  /*! Data structure for FW response */
  rfcommon_fw_response_data_type fw_response;
      
  /*! Data structure to hold RF COMMON semaphore information */
  rfcommon_semaphore_data_type semaphore;

  /*! Common XPT data structure */
  rfcommon_xpt_data_type xpt_data;

  /*! Device Snapshot data structure */
  rfcommon_device_snapshot_type snapshot_data;

} rfcommon_data_type;

#ifdef __cplusplus
}
#endif

#endif /* RFCOMMON_DATA_H */