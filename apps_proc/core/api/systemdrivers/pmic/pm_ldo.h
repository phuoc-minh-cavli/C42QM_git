#ifndef PM_LDO__H
#define PM_LDO__H

/** @file pm_ldo.h 
 *  
 *  This header file contains enums and API definitions for LDO
 *  power rail driver.
*/
/*
 *  Copyright (c) 2016 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.tx/6.0/api/systemdrivers/pmic/pm_ldo.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/22/16   vk      Created
=============================================================================*/

/*===========================================================================

                 HEADER FILE INCLUDE

===========================================================================*/
#include "pm_err_flags.h"
#include "pm_resources_and_types.h"
#include "com_dtypes.h"

/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/

/** @addtogroup pm_ldo
@{ */

/**
 * LDO peripheral index. This enum type contains all LDO regulators that are
 * required. 
 */
enum
{
  PM_LDO_1,       /**< LDO 1. */
  PM_LDO_2,       /**< LDO 2. */
  PM_LDO_3,       /**< LDO 3. */
  PM_LDO_4,       /**< LDO 4. */
  PM_LDO_5,       /**< LDO 5. */
  PM_LDO_6,       /**< LDO 6. */
  PM_LDO_7,       /**< LDO 7. */
  PM_LDO_8,       /**< LDO 8. */
  PM_LDO_9,       /**< LDO 9. */
  PM_LDO_10,      /**< LDO 10. */
  PM_LDO_11,      /**< LDO 11. */
  PM_LDO_12,      /**< LDO 12. */
  PM_LDO_13,      /**< LDO 13. */
  PM_LDO_14,      /**< LDO 14. */
  PM_LDO_15,      /**< LDO 15. */
  PM_LDO_16,      /**< LDO 16. */
  PM_LDO_17,      /**< LDO 17. */
  PM_LDO_18,      /**< LDO 18. */
  PM_LDO_19,      /**< LDO 19. */
  PM_LDO_20,      /**< LDO 20. */
  PM_LDO_21,      /**< LDO 21. */
  PM_LDO_22,      /**< LDO 22. */
  PM_LDO_23,      /**< LDO 23. */
  PM_LDO_24,      /**< LDO 24. */
  PM_LDO_25,      /**< LDO 25. */
  PM_LDO_26,      /**< LDO 26. */
  PM_LDO_27,      /**< LDO 27. */
  PM_LDO_28,      /**< LDO 28. */
  PM_LDO_29,      /**< LDO 29. */
  PM_LDO_30,      /**< LDO 30. */
  PM_LDO_31,      /**< LDO 31. */
  PM_LDO_32,      /**< LDO 32. */
  PM_LDO_33,      /**< LDO 33. */
  PM_LDO_INVALID  /**< Invalid LDO. */
};

/*===========================================================================

                        API PROTOTYPE

===========================================================================*/

/**
 * @name 
 *     pm_ldo_ecm_local_enable_status
 *
 * @description
 *     Allows you to enable disable ecm local .
 * @param[in] pmic_chip            Primary PMIC: 0, Secondary PMIC: 1.
 * @param[in] perph_index          LDO peripheral index. Starts from 0
 *                                   (for the first LDO peripheral).
 * @param 
 *     on_off -  on and off status
 * 
 * @return 
 *     error flag type - pm_err_flag_type
 */

pm_err_flag_type pm_ldo_ecm_local_enable_status(uint8 pmic_chip, uint8 perph_index, pm_on_off_type *on_off);


/** @} */ /* end_addtogroup pm_ldo */

#endif /* PM_LDO__H */
