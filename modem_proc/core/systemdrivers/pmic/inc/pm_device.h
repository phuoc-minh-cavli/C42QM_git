#ifndef __PM_DEVICE__H__
#define __PM_DEVICE__H__

/*! \file pm_device.h
 *  \n
 *  \brief  PMIC DEVICE INIT functions
 *  \details  This file contains functions and variable declarations to support PMIC PON APP Level APIs
 *
 *
 *  \n &copy; Copyright (c) 2019  by QUALCOMM Technologies Incorporated, All Rights Reserved
 */

/*===========================================================================
                                Edit History
This document is created by a code generator, therefore this section will
not contain comments describing changes made to the module.

 
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/25/19   abh     Added file for Tiny Modem image in Twizy
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_err_flags.h"

/*=========================================================================== 
 
                     GLOBAL DEFINES
 
===========================================================================*/

/**
 * @brief Configures PMIC dependencies realted modules. This
 *        is an internal API which handles all the required
 *        SPMI, version detection and other device related
 *        initializations.
 *  
 * @details Initialize the pmic device attributes. 
 *  
 * @param None 
 *                   
 * @return pm_err_flag_type
 *          PM_ERR_FLAG_INVALID_PARAMETER
 *          PM_ERR_FLAG_SUCCESS
 */
pm_err_flag_type
pm_device_setup(void);

#endif  //__PM_DEVICE__H__
