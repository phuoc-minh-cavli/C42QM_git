#ifndef __ECM_CLIENT_IF_H__
#define __ECM_CLIENT_IF_H__
/**
  @file ecm_client_if.h
  
  This file contains the client interface for ECM peripherals.
  It provides methods to control each ecm peripheral.


*/
/*
===============================================================================

                             Edit History

 $Header: //components/rel/core.tx/6.0/hwengines/ecm/ecm.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
08/1/15   rl      Created

===============================================================================
                   Copyright (c) 2015 Qualcomm Technologies Incorporated.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/
#define ECM_PERIPHERAL_NAME_MAX 8

#include "DALSys.h"
#include "ecm_diag.h"

typedef enum _ecm_sample_rate
{
    ECM_SAMPLE_RATE_1ms   = 0,
    ECM_SAMPLE_RATE_2ms   = 1,
    ECM_SAMPLE_RATE_4ms   = 2,
    ECM_SAMPLE_RATE_8ms   = 3,
    ECM_SAMPLE_RATE_16ms  = 4,
    ECM_SAMPLE_RATE_32ms  = 5,
    ECM_SAMPLE_RATE_64ms  = 6,
    ECM_SAMPLE_RATE_128ms  = 7,
    ECM_SAMPLE_RATE_256ms  = 8,
    ECM_SAMPLE_RATE_512ms  = 9,
    ECM_SAMPLE_RATE_MAX   = ECM_SAMPLE_RATE_512ms,
    ECM_SAMPLE_RATE_MIN   = ECM_SAMPLE_RATE_1ms
}ecm_sample_rate;

typedef enum _ecm_ret
{
	ECM_OK,
	ECM_ERROR,
	ECM_PENDING_DATA,
	ECM_NO_DATA
}ecm_ret;

enum ecm_logger
{
	LOGGER_SW,           /**<-- ecm data is read through software */
	LOGGER_SPMIF,        /**<-- ecm data is ready by spmi fetcher hw block */
};

/* callback that clients should register to get notifications of read done */
typedef void(*ecm_cb_type)(void* buf, uint32 size);

/**
 * @brief  open a ecm logger, and get a handle. The logger can be SW or 
 *         a special HW block ( SPMI fetcher). Use the returned 'fd' in
 *         subsequent calls
 * @param[in]  logger  which logger to use \see ecm_logger
 * @return     fd      handle
 */
ecm_ret ecm_open(enum ecm_logger logger, int* fd);

/**
 * @brief  read all the available ecm data. 
 *
 * The user has to call this api multiple times, till there is no more 
 * data to read. This call will terminate after a finite number of
 * invocations ( will only fetch the available data, and will not 
 * generate data ). 
 *
 * @param[in]  fd      handle
 * @param[out] buffer  pointer to the buffer containing the data
 * @param[out  size    size of the valid data in the buffer
 * @return     size    size of the data. 0 means end-of-data
 *
 * @note this call is a blocking call when ecm_logger is LOGGER_SW
 */
ecm_ret ecm_read_single(int fd, uint32** buffer, uint32* size);

/**
 * @brief  register a read notifier callback
 *
 * Register a callback that gets invoked when data is ready
 *
 * @param[in]  fd      handle
 * @param[in]  cb      callback
 *
 */
ecm_ret ecm_register_read_notifier(int fd, ecm_cb_type cb);

/**
 * @brief   change the ECM sample rate and select sample A
 *
 * @param[in]  sample_rate     new sample rate
 *
 */
ecm_ret ecm_set_sample_rate(uint8 sample_rate);

/**
 * @brief  read the ecm data continously from pmic
 *
 * The user has to call this api multiple times, till the user decides
 * to stop fetching data. When the user decides to not fetch any more
 * data, he/she has to call ecm_stop, and then continue to do ecm_read
 * till there is no more data to read.
 *
 * @param[in]  fd      handle
 * @param[out] buffer  pointer to the buffer containing the data
 * @param[out  size    size of the valid data in the buffer
 * @return     size    size of the data. 0 means end-of-data
 *
 * @note this call is a blocking call when ecm_logger is LOGGER_SW
 */
ecm_ret ecm_read(int fd, uint32** buffer, uint32* size);

/**
 * @brief  stop generating ecm data. 
 *
 * There can be data already generated in the buffers/fifo. The user
 * should call ecm_read to empty this buffer/fifo.
 *
 * @param[in]  fd      handle
 */
ecm_ret ecm_stop(int fd);

/**
 * @brief  close the logger
 *
 * The handle is not valid after close returns
 *
 * @param[in]  fd      handle
 */
void ecm_close(int fd);

/**
 * @brief  get the ecm sample rate
 */
uint32 ecm_get_sample_rate(void);

/**
 * @brief number of ecm frames per EOT 
 */
uint32 ecm_get_frames_per_eot(void);

/**
 * @brief get the RTC count from PMIC
 */
ecm_ret ecm_get_pmic_rtc_count(uint32* count);

/**
 * @brief get the ECM frame size
 */
uint32 ecm_get_ecm_frame_size(void);
/**
 * @brief get the calibration data ( regulators )
 */
void ecm_fill_calibration_data(ecm_regulator_info_type *map);
/**
 * @brief get the PMIC info
 */
uint32 ecm_fill_pmic_info(ecm_diag_pkt_pmic_dev_info_type* pkt);

void ecm_get_enabled_regulator_cnt(uint8* buck_cnt, uint8* ldo_cnt, uint8* misc_cnt);
void ecm_dev_init(void);
void ecm_program_poll_n_read_gap(uint8 prg_sample_rate);
#endif
