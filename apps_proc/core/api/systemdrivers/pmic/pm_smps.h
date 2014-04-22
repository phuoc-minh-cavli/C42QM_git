#ifndef PM_SMPS__H
#define PM_SMPS__H

/*! \file pm_smps.h
 *  \n
 *  \brief This header file contains enums and API definitions for SMPS power rail driver.
 *  \n
 *  \n &copy; Copyright 2012-2015 Qualcomm Technologies Incorporated, All Rights Reserved
 */
/* ======================================================================= */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.tx/6.0/api/systemdrivers/pmic/pm_smps.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/06/12   hw      Rearchitecturing module driver to peripheral driver
=============================================================================*/
/*===========================================================================

                        HEADER FILES

===========================================================================*/

#include "pm_err_flags.h"
#include "pm_resources_and_types.h"
#include "com_dtypes.h"

/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/
/** @addtogroup pm_smps
@{ */

/** SMPS peripheral index. This enum type contains all required LDO
    regulators. */
enum
{
  PM_SMPS_1,   /**< SMPS 1. */
  PM_SMPS_2,   /**< SMPS 2. */
  PM_SMPS_3,   /**< SMPS 3. */
  PM_SMPS_4,   /**< SMPS 4. */
  PM_SMPS_5,   /**< SMPS 5. */    
  PM_SMPS_6,   /**< SMPS 6. */
  PM_SMPS_7,   /**< SMPS 7. */
  PM_SMPS_8,   /**< SMPS 8. */
  PM_SMPS_9,   /**< SMPS 9. */
  PM_SMPS_10,  /**< SMPS 10. */ 
  PM_SMPS_11,  /**< SMPS 11. */ 
  PM_SMPS_12,  /**< SMPS 12. */ 
  PM_SMPS_INVALID
};

/** SMPS mode type */
typedef enum
{
    PM_ILIM_SMPS_PWM_MODE,  /**< SMPS PWM mode. */ 
    PM_ILIM_SMPS_AUTO_MODE, /**< SMPS auto mode. For SMPS mode transition
                                  between PFM and PWM. */
    PM_ILIM_SMPS_MODE_INVALID
}pm_smps_ilim_mode_type;

typedef enum 
{
   PM_CLK_TCXO,
   PM_CLK_RC,
   PM_CLK_SOURCE_INVALID
}pm_clk_src_type;

/** SMPS switching frequency. This enumeration assumes an input clock
 * frequency of 19.2 MHz and is 5 bits long. \n
 * Clock frequency = (input clock freq) / ((CLK_PREDIV+1)(CLK_DIV + 1)).
 */
typedef enum 
{
    PM_CLK_19p2_MHz     = 0,   /**< Clock frequency = 19.2 MHz. */
    PM_CLK_9p6_MHz      = 1,   /**< Clock frequency = 9.6 MHz. */
    PM_CLK_6p4_MHz      = 2,   /**< Clock frequency = 6.2 MHz. */
    PM_CLK_4p8_MHz      = 3,   /**< Clock frequency = 4.8 MHz. */
    PM_CLK_3p84_MHz     = 4,   /**< Clock frequency = 3.84 MHz. */
    PM_CLK_3p2_MHz      = 5,   /**< Clock frequency = 3.2 MHz. */
    PM_CLK_2p74_MHz     = 6,   /**< Clock frequency = 2.74 MHz. */
    PM_CLK_2p4_MHz      = 7,   /**< Clock frequency = 2.4 MHz. */
    PM_CLK_2p13_MHz     = 8,   /**< Clock frequency = 2.13 MHz. */
    PM_CLK_1p92_MHz     = 9,   /**< Clock frequency = 1.92 MHz. */
    PM_CLK_1p75_MHz     = 10,  /**< Clock frequency = 1.75 MHz. */
    PM_CLK_1p6_MHz      = 11,  /**< Clock frequency = 1.6 MHz. */
    PM_CLK_1p48_MHz     = 12,  /**< Clock frequency = 1.48 MHz. */
    PM_CLK_1p37_MHz     = 13,  /**< Clock frequency = 1.37 MHz. */
    PM_CLK_1p28_MHz     = 14,  /**< Clock frequency = 1.28 MHz. */
    PM_CLK_1p2_MHz      = 15,  /**< Clock frequency = 1.2 MHz. */
    PM_CLK_1p13_MHz     = 16,  /**< Clock frequency = 1.13 MHz. */
    PM_CLK_1p07_MHz     = 17,  /**< Clock frequency = 1.07 MHz. */
    PM_CLK_1p01_MHz     = 18,  /**< Clock frequency = 1.01 MHz. */
    PM_CLK_960_KHz      = 19,  /**< Clock frequency = 960 kHz. */
    PM_CLK_914_KHz      = 20,  /**< Clock frequency = 914 kHz. */
    PM_CLK_873_KHz      = 21,  /**< Clock frequency = 873 kHz. */
    PM_CLK_835_KHz      = 22,  /**< Clock frequency = 835 kHz. */
    PM_CLK_800_KHz      = 23,  /**< Clock frequency = 800 kHz. */
    PM_CLK_768_KHz      = 24,  /**< Clock frequency = 768 kHz. */
    PM_CLK_738_KHz      = 25,  /**< Clock frequency = 738 kHz. */
    PM_CLK_711_KHz      = 26,  /**< Clock frequency = 711 kHz. */
    PM_CLK_686_KHz      = 27,  /**< Clock frequency = 686 kHz. */
    PM_CLK_662_KHz      = 28,  /**< Clock frequency = 662 kHz. */
    PM_CLK_640_KHz      = 29,  /**< Clock frequency = 640 kHz. */
    PM_CLK_619_KHz      = 30,  /**< Clock frequency = 619 kHz. */
    PM_CLK_600_KHz      = 31,  /**< Clock frequency = 600 kHz. */
    PM_SWITCHING_FREQ_INVALID,
	PM_SWITCHING_FREQ_FREQ_NONE
}pm_smps_switching_freq_type;

/** Quiet mode. */
typedef enum
{
    PM_QUIET_MODE__DISABLE,      /**< Quiet mode disabled (default). */
    PM_QUIET_MODE__QUIET,        /**< Quiet mode enabled. */
    PM_QUIET_MODE__SUPER_QUIET,  /**< Super quiet mode enabled. */
    PM_QUIET_MODE__INVALID
}pm_quiet_mode_type;

/*===========================================================================

                        API PROTOTYPE

===========================================================================*/

/**
 * @name 
 *     pm_smps_ecm_local_enable_status
 *
 * @description
 *     Allows you to enable disable ecm local .
 * @param[in] pmic_chip            Primary PMIC: 0, Secondary PMIC: 1.
 * @param[in] perph_index           SMPs peripheral index. Starts from 0
 *                                   (for the first LDO peripheral).
 * @param 
 *     on_off -  on and off status
 * 
 * @return 
 *     error flag type - pm_err_flag_type
 */

pm_err_flag_type pm_smps_ecm_local_enable_status(uint8 pmic_chip, uint8 perph_index, pm_on_off_type *on_off);


/** @} */ /* end_addtogroup pm_smps */

#endif /* PM_SMPS__H */
