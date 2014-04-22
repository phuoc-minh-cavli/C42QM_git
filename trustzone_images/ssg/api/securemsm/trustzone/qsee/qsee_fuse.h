#ifndef QSEE_FUSE_H
#define QSEE_FUSE_H

/**
@file qsee_fuse.h
@brief Provide HW & SW Fuse functionality
*/

/*===========================================================================
   Copyright (c) 2011, 2018 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/ssg.tz/1.1.3/api/securemsm/trustzone/qsee/qsee_fuse.h#1 $
  $DateTime: 2019/05/23 05:32:51 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
06/05/18    pranavj TPM app separation from winsecapp
10/03/11    rv      Initial version.

===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>
/** @cond */

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

 /**
 QFPROM address space. The address space can contain either raw
 (uncorrected) or corrected (FEC-corrected) data.
 */
typedef enum
{
  QFPROM_ADDR_SPACE_RAW = 0,            /**< Raw address space. */
  QFPROM_ADDR_SPACE_CORR,               /**< Corrected address space. */
  QFPROM_ADDR_SPACE_MAX = 0x7FFFFFFF    /**< Last entry in the QFPROM_ADDR_SPACE enumerator.*/
}QFPROM_ADDR_SPACE;

/**
 Identifies the error type returned by the QFPROM API.
 */
typedef enum
{
  QFPROM_NO_ERR = 0,                            /**< Operation was successful. */
  QFPROM_DATA_PTR_NULL_ERR,                     /**< Null pointer error. */
  QFPROM_ADDRESS_INVALID_ERR,                   /**< Incorrect fuse row address. */
  QFPROM_CLOCK_SETTINGS_INVALID_ERR,            /**< Invalid settings for the
                                                     QFPROM clock. */
  QFPROM_NO_SPECIAL_ITEM_FOUND_ERR,             /**< Deprecated. */
  QFPROM_OPERATION_NOT_ALLOWED_ERR,             /**< Operation is not allowed;
                                                     the hardware is busy. */
  QFPROM_FAILED_TO_CHANGE_VOLTAGE_ERR,          /**< Error when changing the
                                                     PMIC/voltage configuration. */
  QFPROM_WRITE_ERR,                             /**< Write operation failed. */
  QFPROM_REGION_NOT_SUPPORTED_ERR,              /**< Unsupported region for
                                                     reading/blowing fuses. */
  QFPROM_REGION_NOT_READABLE_ERR,               /**< No permission to read from fuses
                                                     in the requested region. */
  QFPROM_REGION_NOT_WRITEABLE_ERR,              /**< No permission to write to fuses
                                                     in the requested region. */
  QFPROM_REGION_FEC_ENABLED_NOT_WRITEABLE_ERR,  /**< Write failed due to an
                                                     FEC-enabled fuse. */
  QFPROM_REGION_RAW_ADDR_READ_BACK_VERIFY_ERR,  /**< Verification error when writing
                                                     to a raw address space. */
  QFPROM_REGION_CORR_ADDR_READ_BACK_VERIFY_ERR, /**< Verification error when writing
                                                     to a corrected address space. */
  QFPROM_ROW_BOUNDARY_CONDITION_VERIFY_ERR,     /**< Row address is not in the
                                                     region's LSB boundary. */
  QFPROM_FEC_ERR,                               /**< FEC read error. */

  /* Please add new enum value here and keep the value incremental to the predecessor */

  QFPROM_ERR_UNKNOWN = 0x7FFFFFFF              /**<  Last entry in the QFPROM_ERR_CODE enumerator. */
}QFPROM_ERR_CODE;

/**
 * Enumeration for the SW fuses TZ supports
 */
typedef enum
{
  QSEE_HLOS_IMG_TAMPER_FUSE    = 0,   /**< Used during boot to determine
                                           if the HLOS image has been authenticated
                                           successfully. */
  QSEE_WINSECAPP_LOADED_FUSE   = 1,   /**< Used by WinSec App to prevent
                                           reloading. */
  QSEE_UEFISECAPP_LOADED_FUSE  = 2,   /**< Used by UefiSecApp to prevent
                                           reloading. */
  QSEE_MDTP_RECOVERY_MS_FUSE   = 3,   /**< Used by MDTP to prevent
                                           access to Bootloader APIs. */
  QSEE_MDTP_HLOS_MS_FUSE       = 4,   /**< Used by MDTP to prevent
                                           access to Bootloader & Recovery APIs. */
  QSEE_MSSECAPP_LOADED_FUSE    = 5,   /**< Used by MSSecApp App to prevent
                                           reloading. */
  QSEE_OEM_FUSE_1              = 6,   /**< Reserved fuse bit for OEMs. */

  QSEE_OEM_FUSE_2              = 7,   /**< Reserved fuse bit for OEMs. */

  QSEE_OEM_FUSE_3              = 8,   /**< Reserved fuse bit for OEMs. */

  QSEE_OEM_FUSE_4              = 9,   /**< Reserved fuse bit for OEMs. */

  QSEE_OEM_FUSE_5              = 10,  /**< Reserved fuse bit for OEMs. */

  QSEE_OEM_FUSE_6              = 11,  /**< Reserved fuse bit for OEMs. */

  QSEE_OEM_FUSE_7              = 12,  /**< Reserved fuse bit for OEMs. */

  QSEE_QCOM_RESERVED_FUSE_1    = 13,  /**< Reserved fuse bit for QCOM. */

  QSEE_QCOM_RESERVED_FUSE_2    = 14,  /**< Reserved fuse bit for QCOM. */

  QSEE_QCOM_RESERVED_FUSE_3    = 15,  /**< Reserved fuse bit for QCOM. */

  QSEE_QCOM_RESERVED_FUSE_4    = 16,  /**< Reserved fuse bit for QCOM. */

  QSEE_QCOM_RESERVED_FUSE_5    = 17,  /**< Reserved fuse bit for QCOM. */

  QSEE_QCOM_RESERVED_FUSE_6    = 18,  /**< Reserved fuse bit for QCOM. */

  QSEE_QCOM_RESERVED_FUSE_7    = 19,  /**< Reserved fuse bit for QCOM. */

  QSEE_QCOM_RESERVED_FUSE_8    = 20,  /**< Reserved fuse bit for QCOM. */

  QSEE_TPM_LOADED_FUSE         = 21,  /**< Used by TPM app to prevent reloading.*/

  QSEE_HLOS_BL_MILESTONE_FUSE  = 22, /**< Used to notify the bootloader milestone call. */

  QSEE_HLOS_TAMPER_NOTIFY_FUSE = 23, /**< Used to notify TZ that HLOS has been tampered. */

  QSEE_NUM_SW_FUSES            = 24  /**< Number of supported SW fuses. */

} qsee_sw_fuse_t;
/** @endcond */

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
  @addtogroup qtee_fuse
  @{
*/

/**
 * Queries whether the given software fuse is blown.
 *
 * @param[in]  fuse_num    Software fuse to query.
 * @param[out] is_blown    Pointer to a boolean indicating whether the given software fuse is blown.
 * @param[in]  is_blown_sz Return pointer size.
 *
 * @return     SUCCESS -- 0 \n
*              FAILURE -- Negative
 * @warning    This function is not thread-safe.
 */
int qsee_is_sw_fuse_blown
(
  qsee_sw_fuse_t    fuse_num,
  bool*             is_blown,
  uint32_t          is_blown_sz
);

/**
 * Changes the state of the software fuse.
 *
 * @param[in] fuse_num Software fuse to blow.
 *
 * @return    SUCCESS -- 0 \n
*             FAILURE -- Negative
 */
int qsee_blow_sw_fuse(qsee_sw_fuse_t fuse_num);

/**
 * Writes the row data to the specified QFPROM raw row address.
 *
 * @param[in] raw_row_address     Row address in the QFPROM region to which the
 *                                row data is to be written.
 *
 * @param[in] row_data            Pointer to the data to write into QFPROM
 *                                region.
 *
 * @param[in] bus_clk_khz         Bus clock frequency (in kHz) connected to
 *                                QFPROM. The value is ignored.
 *
 * @param[out] qfprom_api_status  Pointer to return value from the QFPROM API.
 *
 * @return  SUCCESS -- 0 \n
*           FAILURE -- Negative
 */
int qsee_fuse_write
(
  uint32_t    raw_row_address,
  uint32_t*   row_data,
  uint32_t    bus_clk_khz,
  uint32_t*   qfprom_api_status
);

/**
 * Reads the row data from the specified QFPROM row address.
 *
 * @param[in] row_address         Row address in the QFPROM region from which the row data is read.
 *
 * @param[in] addr_type           Raw (uncorrected) or FEC-corrected data.
 *
 * @param[out] row_data           Pointer to the data to be read.
 *
 * @param[out] qfprom_api_status  Pointer to return value from the QFPROM API.
 *
 * @return SUCCESS -- 0 \n
*          FAILURE -- Negative
 */
int qsee_fuse_read
(
  uint32_t   row_address,
  int32_t    addr_type,
  uint32_t*  row_data,
  uint32_t*  qfprom_api_status
);

/** @} */

#endif /* QSEE_FUSE_H */
