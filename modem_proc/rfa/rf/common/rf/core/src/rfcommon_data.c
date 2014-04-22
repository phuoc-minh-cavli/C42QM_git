/*! 
  @file
  rfcommon_data.c
 
  @brief
  This file contains Centralized Global Data used by RF Common Driver.
 
  @addtogroup RFCOMMON_DATA
  @{
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

  $Header: //components/rel/rfa.mpss/3.10/rf/common/rf/core/src/rfcommon_data.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
10/25/18   sk      TRACK B Bringup changes
10/05/18   mns     device snapshot feature.
08/06/14   aro     Added FBRx data structure and access interface 
                   in common data
12/12/13   hdz     Switched to issue_seq based txagc override
11/18/13   cd      Added debug flag for RFLM based Tx AGC override
04/15/13   Saul    Common. Consolidated XPT data into one structure.
04/15/13   Saul    Common. DPD APIs use critical section.
04/12/13   Saul    [Common] Use RF dispatch for fw responses.
03/05/13   aro     Enhanced XPT override command and migrated to FTM common
                   dispatch
02/22/13   cri     Load DPD tables based on tx chain 
02/04/13   aro     Migrated the variable to enable/disable slave resource
                   to RF common data
11/19/12   aki     Added rfcommon_data_deinit() function
11/05/12   Saul    Common XPT Sample Capture. Support for capture and proc rsp.
10/19/12   Saul    COMMON XPT. Added Tx common data containing dpd tables.
08/08/12   Saul    CDMA. Added code to profile writing dpd tables.
07/23/12   Saul    RF COMMON. MDSP/MSGR framework. Initial Revision.

==============================================================================*/

#include "comdef.h"
#include "rfcommon_data.h"
#include "rfcommon_msg.h"
#include "modem_mem.h"
#include "rfcommon_core_xpt.h"
#include "rfcommon_core_utils.h"

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Variable to store global Data to be used by RF COMMON Driver
*/
static rfcommon_data_type rfcommon_data;

/*!
  Struct that holds all RF COMMON debug flags
*/ 
rfcommon_debug_flags_t rfcommon_debug_flags = 
{
  TRUE,  /* use_new_fwrsp */
  TRUE, /* use_rflm_txagc_override */
};

/*! @} */

/*============================================================================*/
/*!
  @name MDSP

  @brief
  This section includes interfaces pertaining to RF COMMON MDSP Data.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get Mutable RF COMMON MDSP Data

  @return
  Pointer to RF COMMON MDSP Data Structure used for updates
*/
rfcommon_mdsp_data_type *
rfcommon_get_mdsp_data
(
  void
)
{

  /* Return MDSP Data Pointer */
  return &(rfcommon_data.mdsp);

} /* rfcommon_get_mdsp_data */

/*! @} */

/*============================================================================*/
/*!
  @name Semaphore

  @brief
  This section includes interfaces pertaining to semaphore data.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get Mutable RF COMMON Semaphore Data

  @details
  This function will return the RF semaphore Data. The
  pointer returned from this function can be used to update the
  data as well.

  @return
  Pointer to COMMON semaphore Data Structure used for updates
*/
rfcommon_semaphore_data_type *
rfcommon_get_semaphore_data
(
  void
)
{

  /* Return SEMAPHORE Pointer */
  return &(rfcommon_data.semaphore);

} /* rfcommon_get_semaphore_data */

/*! @} */

/*============================================================================*/
/*!
  @name Task

  @brief
  This section includes interfaces pertaining to intelliceiver and RF Task
  Data.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get Mutable RF COMMON Fw response Data

  @details
  This function will return the RF COMMON FW response Data. The
  pointer returned from this function can be used to update the
  data as well.

  @return
  Pointer to RF COMMON firmware Data Structure used for updates
*/
rfcommon_fw_response_data_type *
rfcommon_get_fw_response_data
(
  void
)
{

  /* Return RF Task Data Pointer */
  return &(rfcommon_data.fw_response);

} /*rfcommon_get_fw_response_data */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get XPT Lock Data

  @return
  Writeable Pointer to XPT Lock Data
*/
rf_lock_data_type *
rfcommon_get_xpt_lock_data
(
  void
)
{
  return &rfcommon_data.xpt_data.xpt_crit_section;
} /* rfcommon_get_xpt_lock_data */

/*! @} */

/*============================================================================*/
/*!
  @name Tx Resource manager

  @brief
  This section includes interfaces pertaining to Tx resource Manager
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enable Slave resource
 
  @details
  This function will be called in debug mode to enable the slave resource. Slave
  resource will be enabled by default; however, if the slave resource is
  disabled for debug purpose, then this function can be called to undo the 
  process. With this, the normal execution of ET/EPT transition will happen
  based on SV or nonSV scenario.
 
  The effect of this function will take place in next tx wakeup sequence.
*/
boolean
rfcommon_configure_xpt_override
(
  rfm_device_enum_type device,
  boolean override_en,
  xpt_cfg_type xpt_mode
)
{
  rfcommon_data.xpt_data.xpt_override.override_en = override_en;
  if ( override_en == TRUE )
  {
    rfcommon_data.xpt_data.xpt_override.xpt_mode = xpt_mode;
  }
  else
  {
    rfcommon_data.xpt_data.xpt_override.xpt_mode = XPT_CFG_TYPE_MAX;
  }

  return TRUE;

} /* rfcommon_configure_xpt_override */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Query if the slave resource is enabled
 
  @details
  This function will be called to query to check if the slave resource is
  enabled or not.
*/
boolean
rfcommon_is_xpt_override
(
  rfm_device_enum_type device,
  xpt_cfg_type *xpt_mode
)
{
  boolean ret_val; /* Return Value */

  /* NULL Pointer check */
  if ( xpt_mode == NULL )
  {
    RF_MSG_1( RF_FATAL, "rfcommon_is_xpt_override: Dev %d, NULL container",
              device );
    return FALSE;
  } /* if ( xpt_mode == NULL ) */

  ret_val = rfcommon_data.xpt_data.xpt_override.override_en;
  *xpt_mode = rfcommon_data.xpt_data.xpt_override.xpt_mode;

  return ret_val;

} /* rfcommon_is_xpt_override */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get Device Snapshot Data
*/
rfcommon_device_snapshot_type* 
rfcommon_get_snapshot_data
(
    void
    )
{
    return &(rfcommon_data.snapshot_data);
}/* rfcommon_get_snapshot_data */

/*! @} */

/*! @} */
