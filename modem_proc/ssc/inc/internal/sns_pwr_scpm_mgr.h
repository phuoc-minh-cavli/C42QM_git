#pragma once
/** ======================================================================================
  @file sns_pwr_scpm_mgr.h

  @brief Sensors Power SCPM Manager

  Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

 ===================================================================================== **/


/**
*****************************************************************************************
                               Includes
*****************************************************************************************
*/
#include <stdbool.h>
#include <stdint.h>
#include "sns_isafe_list.h"
#include "sns_osa_thread.h"
#include "sns_rc.h"
#include "sns_time.h"

/**
*****************************************************************************************
                                  Constants & Macros
*****************************************************************************************
*/
/* Sampling period of 10 ms */
#define SNS_THREAD_UTIL_SAMPLING_PERIOD_NS 10000000ULL

/* High thread utilization threshold for bumping up MCPS request */
#define SNS_HIGH_THREAD_UTILIZATION_THRESHOLD 60

/* Low thread utilization threshold for bumping down MCPS request */
#define SNS_LOW_THREAD_UTILIZATION_THRESHOLD 10

/* Run length threshold for low thread utilization */
#define SNS_LOW_THREAD_UTILIZATION_RUN_LEN_THRESH 40

/* Run length threshold for high thread utilization */
#define SNS_HIGH_THREAD_UTILIZATION_RUN_LEN_THRESH_SVS 10

/* Run length threshold for high thread utilization */
#define SNS_HIGH_THREAD_UTILIZATION_RUN_LEN_THRESH_NOM 30

/* Run length threshold for high thread utilization */
#define SNS_HIGH_THREAD_UTILIZATION_RUN_LEN_THRESH_TURBO 40

/* MCPS vote time window of 1s */
#define SNS_MCPS_VOTE_TIME_WINDOW 1000000000ULL

/* Number of SSC Q6 HW threads */
#define NUM_HW_THREAD 2

/* Default MCPS level configuration. These values apply only if registry isn't read */
#define SNS_Q6_MCPS_DEFAULT           0
#define SNS_Q6_MCPS_LOW_SVS_DEFAULT   125
#define SNS_Q6_MCPS_SVS_DEFAULT       250
#define SNS_Q6_MCPS_NOMINAL_DEFAULT   550
#define SNS_Q6_MCPS_TURBO_DEFAULT     700

/* Bandwidth vote */
#define SSC_BW_ZERO 0

#ifndef SNS_SSC_BW_DEF_VOTE
#define SNS_SSC_BW_DEF_VOTE 10000000
#endif

typedef enum sns_q6_mcps_level
{
  SNS_Q6_MCPS_DEFAULT_LEVEL = 0,
  SNS_Q6_MCPS_LOW_SVS_LEVEL,
  SNS_Q6_MCPS_SVS_LEVEL,
  SNS_Q6_MCPS_NOMINAL_LEVEL,
  SNS_Q6_MCPS_TURBO_LEVEL,
} sns_q6_mcps_level;

/**
*****************************************************************************************
                                  Structures
*****************************************************************************************
*/
typedef struct
{
  sns_isafe_list_item list_entry;
  bool          require_speedup;
} sns_scpm_mgr_client;

/* Target SSC Q6 operating levels in MHz as read from the registry */
typedef struct
{
  uint32_t low_svs;
  uint32_t svs;
  uint32_t nominal;
  uint32_t turbo;
}sns_scpm_mgr_mcps_config;

/**
*****************************************************************************************
                                  Public Functions
*****************************************************************************************
*/

/**
 * Update the thread utilization for the provided thread
 * Check and update the SLPI MIPS vote based on the thread utilization update
 *
 * @param[i]    Thread pointer
 * @param[i]    Thread utilization in percentage
 *
 */
void
sns_pwr_scpm_mgr_update_thread_utilization(sns_osa_thread *thread, uint32_t thread_utilization);

/**
 * Initialize the sensors power scpm manager.
 * Create handles to the SCPM required to set SSC MIPS and BW requirements
 *
 * @return
 * SNS_RC_SUCCESS
 */
sns_rc
sns_pwr_scpm_mgr_init(void);

/**
 * Register scpm client
 */
sns_scpm_mgr_client * sns_pwr_scpm_mgr_create_client(void);

/**
 * Request an increase in MCPS
 */
sns_rc
sns_pwr_scpm_mgr_mcps_bump_up(sns_scpm_mgr_client *client);

/**
 * Request a decrease in MCPS
 */
sns_rc
sns_pwr_scpm_mgr_mcps_bump_down(sns_scpm_mgr_client *client);
/**
 * Request SCPM manager for absolute MCPS vote
 */
sns_rc
sns_pwr_scpm_mgr_set_mcps_vote(
  sns_scpm_mgr_client *client, sns_q6_mcps_level mcps_level);
/**
 * Request SCPM manager for Bandwidth vote
 */
void
sns_pwr_scpm_mgr_bw_vote(uint64_t ssc_bw_vote);
/**
 * Request to get Current active MCPS vote
 */
uint32_t
sns_pwr_scpm_mgr_get_current_mcps_vote();
/**
 * Store current MCPS config read from the registry into the MCPS state data structure.
 * The MCPS operating levels will subsequently used to scale up/down the Q6 frequency 
 */
void 
sns_pwr_scpm_mgr_store_mcps_config(const sns_scpm_mgr_mcps_config *mcps_config);
