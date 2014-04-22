#ifndef DS_CAAS_MGR_H
#define DS_CAAS_MGR_H
/*===========================================================================
                      DS_CAAS_MGR.H

DESCRIPTION
 Header file of DS CAAS MGR module.

EXTERNALIZED FUNCTIONS

 Copyright (c) 2019 by Qualcomm Technologies, Incorporated.
 All Rights Reserved.
===========================================================================*/
/*===========================================================================

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  when        who                            what, where, why
--------    -------                ----------------------------------------
18/03/18    sraghuve                          First version of file
===========================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "ds_sys_eventi.h"

#ifdef QWES_FEATURE_ENABLE


#define DSUTIL_ATOI(x, y, z)                  \
        z = 0;                                \
        while( x < y )                        \
        {                                     \
          if((*x) >= '0' && (*x) <= '9')      \
          {                                   \
            z = z *10 + (*x- '0');            \
            x++;                              \
          }                                   \
          else                                \
          {                                   \
            break;                            \
          }                                   \
        }


/*==============================================================================
                 EXTERNAL VARIABLE DEFINITIONS AND TYPES
==============================================================================*/


/*===========================================================================
                         GLOBAL VARIABLES
===========================================================================*/

/*===========================================================================
                    EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS_CAAS_MGR_INIT

DESCRIPTION   This function initialize DS CAAS MGR module.

PARAMETERS    None
 
DEPENDENCIES  None 
 
RETURN VALUE  None 

SIDE EFFECTS  None
===========================================================================*/
void ds_caas_mgr_init(void);

/*===========================================================================
FUNCTION      DS_CAAS_MGR_CONFIG_PERIODIC_TIMER

DESCRIPTION   API to give testapp flexibility to configure periodic check 
              feature status request.

DEPENDENCIES  None
 
PARAMETERS    feature_id   - the client’s feature ID
    		  feature_stat - FALSE: feature failed during self-test; 
                             TRUE:  feature passes during self-test.

RETURN VALUE  None

SIDE EFFECTS  None                                                                  
===========================================================================*/
void ds_caas_mgr_config_periodic_timer(uint32 timer_val);

/*===========================================================================
FUNCTION      DS_CAAS_MGR_RESET_DEFUALT_FEATURE_ENABLE

DESCRIPTION   API to give testapp flexibility to reset the default feature 
              enablement

DEPENDENCIES  None
 
PARAMETERS    None

RETURN VALUE  Please refer to ds_caas_mgr_feature_activation_status_e

SIDE EFFECTS  None                                                                  
===========================================================================*/
ds_caas_mgr_feature_activation_status_e ds_caas_mgr_default_feature_enable
(
  uint32 feature_id
);


/*===========================================================================
FUNCTION      DS_CAAS_MGR_SET_FEATURE_LIST

DESCRIPTION   Function stores a copy of the feature List

DEPENDENCIES  None
 
PARAMETERS    req_ptr - Feature List pointer

RETURN VALUE  None

SIDE EFFECTS  None
==============================================================================*/
void ds_caas_mgr_set_feature_list
(
  ds_sys_caas_feature_list_type*          req_ptr
);

/*===========================================================================
FUNCTION      DS_CAAS_MGR_GET_FEATURE_LIST

DESCRIPTION   Function Fetches the feature List

DEPENDENCIES  None
 
PARAMETERS    req_ptr - Feature List pointer

RETURN VALUE  None

SIDE EFFECTS  None
==============================================================================*/
void ds_caas_mgr_get_feature_list
(
  ds_sys_caas_feature_list_type*          req_ptr
);

/*===========================================================================
FUNCTION      DS_CAAS_MGR_RESET_FEATURE_LIST

DESCRIPTION   Function Resets the feature List

DEPENDENCIES  None
 
PARAMETERS    None

RETURN VALUE  None

SIDE EFFECTS  None
==============================================================================*/
void ds_caas_mgr_reset_feature_list();


#endif /* QWES_FEATURE_ENABLE */

#endif /* DS_CAAS_MGR_H */
