#ifndef DS_CAAS_MGR_API_H
#define DS_CAAS_MGR_API_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        DS_CAAS_MGR_API.H

DESCRIPTION
  Header containing  External API's for modem caas manager module

Copyright (c) 2019 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/dev/dataiot.mpss/1.0/sraghuve.dataiot.mpss.1.0.sraghuve_caas_porting/api/public/ds_caas_mgr_api.h#1 

when        who        what, where, why
--------    ---    ---------------------------------------------------------- 
03/05/19    sraghuve   Initial Version
===========================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "sys.h"
#include "comdef.h"

/*===========================================================================
                      MACROS DECLARATIONS
===========================================================================*/
#define DS_CAAS_MAX_NUM_FEATURE_IDS 100

typedef enum
{
  DS_CAAS_MGR_FEATURE_STATUS_ALLOWED          = 0,
  DS_CAAS_MGR_FEATURE_STATUS_NOT_ALLOWED      = 1,
  DS_CAAS_MGR_FEATURE_STATUS_UNKNOWN          = 2
}ds_caas_mgr_feature_status_type_e;

typedef enum
{
  DS_CAAS_MGR_FEATURE_REG_SUCCESS             = 0,
  DS_CAAS_MGR_FEATURE_REG_UNSUPPORTED_ID      = 1,
  DS_CAAS_MGR_FEATURE_REG_INVALID_PARAM       = 2,
  DS_CAAS_MGR_FEATURE_REG_FAILURE             = 3
}ds_caas_mgr_feature_reg_status_e;

typedef enum
{
  DS_CAAS_MGR_ACT_STATUS_ALLOWED              = 0,
  DS_CAAS_MGR_ACT_STATUS_NOT_ALLOWED          = 1,
  DS_CAAS_MGR_ACT_STATUS_UNKNOWN              = 2,
  DS_CAAS_MGR_ACT_STATUS_UNSUPPORTED_ID       = 3
}ds_caas_mgr_feature_activation_status_e;


/*===========================================================================
                      PUBLIC FUNCTION DECLARATIONS
===========================================================================*/
/**
  @brief Callback function to notify clients about current feature activation 
         status.
   
  @param[in] status - To notify the client current feature status.
   
  @see ds_caas_mgr_feature_reg

  Please refer to ds_caas_mgr_feature_status_type_e for the values returned 
  in feature_status
*/

typedef void (*ds_caas_mgr_act_stat_cb_t)
(
  ds_caas_mgr_feature_status_type_e feature_status
);

/*===========================================================================
                   EXTERNAL FUNCTIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS_CAAS_MGR_FEATURE_REG

DESCRIPTION  API called by clients to register to get current feature activation
             status notification.

DEPENDENCIES  None
 
PARAMETERS    feature_id  - the client’s feature ID.
              act_stat_cb - callback function to notify the client the feature
                            activation status.

RETURN VALUE  : Please refer ds_caas_mgr_feature_reg_status_e

SIDE EFFECTS  None                                                             
===========================================================================*/
ds_caas_mgr_feature_reg_status_e ds_caas_mgr_feature_reg 
(
  uint32                     feature_id,
  ds_caas_mgr_act_stat_cb_t  act_stat_cb
);

/*===========================================================================
FUNCTION      DS_CAAS_MGR_GET_ACT_STAT

DESCRIPTION   API for clients to query DS regarding the feature activation 
              status.

DEPENDENCIES  None
 
PARAMETERS    feature_id - the client’s feature ID

RETURN VALUE  Please refer to ds_caas_mgr_feature_activation_status_e

SIDE EFFECTS  None                                                         
===========================================================================*/
ds_caas_mgr_feature_activation_status_e ds_caas_mgr_get_act_stat 
(
  uint32 feature_id 
);

#endif /* DS_CAAS_MGR_API_H */


