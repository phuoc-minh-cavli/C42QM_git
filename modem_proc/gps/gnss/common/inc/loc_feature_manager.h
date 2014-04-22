/******************************************************************************
  @file:  loc_feature_manager.h
  @brief: Location Feature Manager
  

  DESCRIPTION
  This is the centralized module to query feature status from either Modem Caas Manager
  or NV item.

  INITIALIZATION AND SEQUENCING REQUIREMENTS
   N/A

  -----------------------------------------------------------------------------
  Copyright (c) 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  -----------------------------------------------------------------------------
******************************************************************************/

/*=====================================================================
                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who      what, where, why
--------   ---      -------------------------------------------------------
06/06/18   yh       Initial version

$Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/common/inc/loc_feature_manager.h#1 $
$DateTime: 2020/07/21 21:23:44 $
$Author: pwbldsvc $
======================================================================*/


#ifndef LOC_FM_H
#define LOC_FM_H

#include "comdef.h"
#include "aries_os_api.h"

/*---------------------------------------------------------------------------
@brief Feature ID are predefined integers to distinguish one feature form the
       other. A QC license certificate can have a list of integers.
       The enforcement of the feature ID, the checking of the integer ranges
       is up to the code on the device implementing the feature.
       Below are the feature IDs have been created for the Location team features.
---------------------------------------------------------------------------*/
#define CAAS_GTP_WWAN_SERVICE_ID        (803)
#define CAAS_GNSS_SERVICE_ID            (804)


#define LOCFM_SUCCESS                   (0)
#define LOCFM_UNSUPPORTED_FEATURE_ID    (1)
#define LOCFM_INVALID_PARAMETER         (2)
#define LOCFM_NO_MEMORY                 (3)
#define LOCFM_INTERNAL_ERROR            (4)
#define LOCFM_TRY_AGAIN                 (5)

typedef struct {
    uint32  q_FeatureID;
    boolean v_Status;
} LocFM_FeatureStatusStructT;

/*---------------------------------------------------------------------------
@brief Callback function to notify clients about the feature activation 
       status.

@param[in/out] pz_FeatureStatus - To notify the client the feature status.

@retval TRUE callback is handled by the client successfully, otherwsie FALSE

@see LocFM_RegisterFeature
---------------------------------------------------------------------------*/
typedef boolean(*LocFM_FeatureStatusCb)
(              
  const LocFM_FeatureStatusStructT *pz_FeatureStatus
);

/*---------------------------------------------------------------------------
@brief API called by clients to register to get the feature activation
       status notification.

@param[in] q_FeatureID - The client’s feature ID.
@param[in] p_FeatureStatusCb - callback function to notify the client the 
                               feature activation status.

@retval LOCFM_SUCCESS
        LOCFM_UNSUPPORTED_FEATURE_ID
        LOCFM_INVALID_PARAMETER
        LOCFM_NO_MEMORY
        LOCFM_INTERNAL_ERROR
        LOCFM_TRY_AGAIN
---------------------------------------------------------------------------*/
uint32 LocFM_RegisterFeature
(
  uint32                 q_FeatureID,
  LocFM_FeatureStatusCb  p_FeatureStatusCb
);

/*---------------------------------------------------------------------------
@brief The function to handle feature register callback IPC message

@param[in] p_IpcMsg - IPC message pointer

@retval    None
---------------------------------------------------------------------------*/
void LocFM_HandleRegisterCb(const os_IpcMsgType* p_IpcMsg);

/*---------------------------------------------------------------------------
@brief The intial task shall call this function to start location feature manager
@param     None
@retval    None
---------------------------------------------------------------------------*/
void locFM_Init(void);

#endif // LOC_FM_H
