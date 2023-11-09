#ifndef PM_APP_CHG_H
#define PM_APP_CHG_H

/**
 * Copyright (c) 2018, 2023 Qualcomm Technologies, Inc. All rights reserved
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *
 * Export of this technology or software is regulated by the U.S. Government.
 * Diversion contrary to U.S. law prohibited.
 *
 * All ideas, data and information contained in or disclosed by
 * this document are confidential and proprietary information of
 * Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
 * By accepting this material the recipient agrees that this material
 * and the information contained therein are held in confidence and in
 * trust and will not be used, copied, reproduced in whole or in part,
 * nor its contents revealed in any manner to others without the express
 * written permission of Qualcomm Technologies Incorporated.
 *
 */

/**
 * @file pm_app_chg.h PMIC related declaration.
 *
 * @brief This file contains functions and variable declarations to provide 
 *        the generic PMIC api support for usb team.
 */

/*===========================================================================

                  P M    H E A D E R    F I L E

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.xf/0.2/QcomPkg/Include/api/systemdrivers/pmic/pm_app_chg.h#2 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/07/08   ks     Created to provide generic pmic api to usb team
===========================================================================*/

/*===========================================================================

                        PMIC INCLUDE FILES

===========================================================================*/
#include "com_dtypes.h"
#include "pm_err_flags.h"
#include "pm_resources_and_types.h"

/*=========================================================================== 
 
                     GLOBAL DEFINES
 
===========================================================================*/
typedef enum
{
  PM_APP_CHG_SDP_CHARGER,
  PM_APP_CHG_OCP_CHARGER,
  PM_APP_CHG_CDP_CHARGER,
  PM_APP_CHG_DCP_CHARGER,
  PM_APP_CHG_FLOAT_CHARGER,
  PM_APP_CHG_QC_2P0,
  PM_APP_CHG_QC_3P0,
  PM_APP_CHG_ICL_OVERRIDE_LATCH,
  PM_APP_CHG_ACA_DOCK,
  PM_APP_CHG_ACA_C_CHARGER,
  PM_APP_CHG_ACA_B_CHARGER,
  PM_APP_CHG_ACA_A_CHARGER,
  PM_APP_CHG_INVALID,
}pmapp_chg_usb_apsd_result_status_type;

typedef enum
{
  PM_APP_CHG_USB_CC_STATUS_UNAVAILABLE,
  PM_APP_CHG_USB_CC_STATUS_FLIP,
  PM_APP_CHG_USB_CC_STATUS_UNFLIP,
  PM_APP_CHG_USB_CC_STATUS_INVALID,
}pmapp_chg_usb_cc_status_type;

typedef enum
{
    PM_APP_CHG_USB_CONNECTOR_TYPEC,
    PM_APP_CHG_USB_CONNECTOR_MICRO_USB,
    PM_APP_CHG_USB_CONNECTOR_INVALID
} pmapp_chg_usb_connector_type;

/*===========================================================================

                     PROTOTYPES 

===========================================================================*/


 /**
 * @brief This wrapper function detects the type of charger 
 * 
 * @details
 *  This API detects the type of charging port device is connected to
 * @param[out] src_detected       Refer pmapp_CHG_usb_apsd_result_status_type for more info
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pmapp_chg_usb_get_charger_type(pmapp_chg_usb_apsd_result_status_type *src_detected);


/**
 * @brief This wrapper function gives status of USB presence 
 * @param[out] status: 
 *                returns the Vbus status.
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pmapp_chg_usb_get_vbus_presence(boolean *status);

/**
 * @brief This wrapper function gives cc status  
 * @param[out] cc_status: Refer pmapp_chg_usb_cc_status_type for more info
 *                 USB super speed lane selection is based on the cc status.
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pmapp_chg_usb_get_cc_status(pmapp_chg_usb_cc_status_type *cc_status);

/**
 * @brief This wrapper function gives port type    
 * @param[out] : Refer pmapp_chg_usb_connector_type for more info
 *                 
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pmapp_chg_usb_get_usb_connector_type(pmapp_chg_usb_connector_type *usb_type);

/**
 * @brief This wrapper function to disable inok disable    
 *                 
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = SMB is not present.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pmapp_chg_usb_inok_disable(void);

#endif    /* PM_APP_CHG_H */

