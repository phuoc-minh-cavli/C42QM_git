/**
 * sns_pwr_scpm_mgr.c
 *
 * The Sensors Power Manager interface to the SCPM
 *
 * Copyright (c) 2017 - 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 *
 **/

#include <stdint.h>
#include <string.h>
#include "sns_assert.h"
#ifdef SNS_SDC_CLOCK_VOTING_ENABLE
#include "sns_clock_def.h"
#endif
#include "sns_isafe_list.h"
#include "sns_island.h"
#include "sns_memmgr.h"
#include "sns_osa_lock.h"
#include "sns_printf_int.h"
#include "sns_profiler.h"
#include "sns_pwr_scpm_mgr.h"
#include "sns_sensor.h"
#include "sns_time.h"
#include "sns_types.h"

#include "DDIChipInfo.h"
#include "mmpm.h"


/**
 * Resolution is in ticks, equivalent to 100msec.
 * Threshold to reset the  max thread utilization updated by
 * a thread, if not updated further within this limit.
 *
 * This avoids unnecessary clock bump-up.
 */
#define SNS_PWR_SCPM_THREAD_UTILIZATION_UPDATE_THRESHOLD ( 1920000 )

/**
 * SDC clock configuration
 */
typedef struct sns_pwr_scpm_sdc_clock_cfg
{
  uint32_t scc_sdc_proc_fclk_clk;
  uint32_t scc_smem_clk;
  uint32_t scc_ccd_clk;

}sns_pwr_scpm_sdc_cfg;

/**
 * Interface structure for MMPM SDC clock vote
 */
#ifdef SNS_SDC_CLOCK_VOTING_ENABLE 
typedef struct sns_mmpm_sdc_vote_cfg
{
  MmpmClkIdSlpiType clock_id;
  uint32 clock_freq_hz;
  uint32_t client_id;

}sns_mmpm_sdc_vote_cfg;
#endif
/**
 * Private thread utilization list item.
 */
typedef struct sns_pwr_scpm_mgr_thread_item
{
  /* Thread pointer */
  sns_osa_thread  *thread;
  /* This thread's utilization percentage */
  uint32_t  thread_utilization;
  /* Time at which thread utilization was updated. */
  sns_time last_update_ts;
  /* Thread list entry */
  sns_isafe_list_item  list_entry;
}sns_pwr_scpm_thread_item;

/**
 * Private state definition.
 */
struct sns_fw_pwr_scpm_mgr
{
  /* SCPM handle for the SSC Q6 */
  uint32_t ssc_q6_client_id;
  /* SCPM handle for the SDC */
  uint32_t sdc_client_id;
  /* SCPM handle for the SRAM */
  uint32_t sram_client_id;
  /* SCPM handle for the CCD */
  uint32_t ccd_client_id;
  /* Current MCPS config */
  sns_scpm_mgr_mcps_config mcps_config;
  /* Current MCPS vote */
  //Todo: Refactoring: combine the run lengths/timestamps into an array
  int32_t low_run_len;
  /* Run length of high thread utilization */
  int32_t high_run_len;
  /* High run length vote timestamp */
  sns_time high_vote_ts;
  /* Low run length vote timestamp */
  sns_time low_vote_ts;
  /* Current MCPS vote */
  uint32_t curr_mcps;
  /* List of threads and their utilization percentages */
  struct sns_isafe_list thread_list;
  /* Lock to protect power SCPM manager data structure fields */
  sns_osa_lock*   scpm_mgr_lock;
  /* List of scpm clients */
  sns_isafe_list client_list;
  /* Thread utilization mcps */
  uint32_t thread_utilization_mcps;
  /* Chip version */
  DalChipInfoVersionType version;
  /* Chip ID */
  DalChipInfoIdType chip;
  /* SDC clock configuration  */
  sns_pwr_scpm_sdc_cfg sdc_cfg;

} ;

/**
 * Private state of the Power SCPM Manager Utility.
 */
static struct sns_fw_pwr_scpm_mgr sns_pwr_scpm_mgr SNS_SECTION(".data.sns");

/*=============================================================================
  Forward Declarations
  ===========================================================================*/

extern bool sns_pwr_scpm_mgr_check_chip_and_version(DalChipInfoIdType chip, DalChipInfoVersionType version);

/**
 * Evaluate all clients and update mcps vote
 */
SNS_SECTION(".text.sns") static bool
sns_pwr_scpm_mgr_check_clients_for_speedup()
{
  sns_isafe_list_iter iter;
  for (sns_isafe_list_iter_init(&iter, &sns_pwr_scpm_mgr.client_list, true);
    NULL != sns_isafe_list_iter_curr(&iter); sns_isafe_list_iter_advance(&iter))
  {
    sns_scpm_mgr_client *client =
      (sns_scpm_mgr_client *)sns_isafe_list_iter_get_curr_data(&iter);
    if (client->require_speedup)
    {
      return true;
    }
  }
  return false;
}
/**
 * Updates the Bandwidth vote depending on whether or not sensor client is present
 * When there is no sensor client it turns the vote to 0
 * When client is presnt it sets the bandwidth vote
 * @param  ssc_bw_vote  bandwidth vote to be set
 */

SNS_SECTION(".text.sns")
void sns_pwr_scpm_mgr_bw_vote(uint64_t ssc_bw_vote)
{ 
#ifdef SNS_BANDWIDTH_VOTING_ENABLED 
  MMPM_STATUS req_result = MMPM_STATUS_SUCCESS;
  MmpmRscExtParamType req_rsc_param;
  MMPM_STATUS result_status[1];
  MmpmRscParamType req_param[1];
  req_rsc_param.apiType = MMPM_API_TYPE_ASYNC;
  req_rsc_param.numOfReq = 1;
  req_rsc_param.reqTag = 0;
  req_rsc_param.pStsArray = result_status;
  req_rsc_param.pReqArray = req_param;
  MmpmGenBwReqType mmpmBwReqParam;
  MmpmGenBwValType bandWidth;

#ifndef SSC_TARGET_HEXAGON_NO_ISLAND
  MMPM_STATUS sts = MMPM_STATUS_FAILED;
  MmpmParameterConfigType paramConfig;
  MmpmIslandParticipationType islandOption;
  islandOption.islandOpt = MMPM_ISLAND_PARTICIPATION_LPI;
  paramConfig.paramId = MMPM_PARAM_ID_ISLAND_PARTICIPATION;
  paramConfig.pParamConfig = &islandOption;
  sts = MMPM_SetParameter(sns_pwr_scpm_mgr.ssc_q6_client_id, &paramConfig);
#endif
  bandWidth.busRoute.masterPort =MMPM_BW_PORT_ID_ADSP_MASTER;
  bandWidth.busRoute.slavePort = MMPM_BW_PORT_ID_DDR_SLAVE;
  bandWidth.bwValue.busBwValue.usagePercentage =50;
  bandWidth.bwValue.busBwValue.usageType=  MMPM_BW_USAGE_LPASS_DSP;
  mmpmBwReqParam.numOfBw = 1;
  mmpmBwReqParam.pBandWidthArray = &bandWidth;
  req_param[0].rscId = MMPM_RSC_ID_GENERIC_BW;
  req_param[0].rscParam.pGenBwReq = &mmpmBwReqParam;
  bandWidth.bwValue.busBwValue.bwBytePerSec =ssc_bw_vote;
  req_result = MMPM_Request_Ext(sns_pwr_scpm_mgr.ssc_q6_client_id, &req_rsc_param);
  if(req_result == MMPM_STATUS_SUCCESS)
  { 
    SNS_PRINTF(HIGH, sns_fw_printf, "Set the SSC Bandwidth Requirement with bandwidth vote %d", ssc_bw_vote);
  }
  else
  {
	SNS_PRINTF(ERROR, sns_fw_printf, "MMPM Request for bandwidth failed with error");
  }
#else
  UNUSED_VAR(ssc_bw_vote);
#endif
}

#ifdef SNS_SDC_CLOCK_VOTING_ENABLE
SNS_SECTION(".text.sns") static bool
sns_pwr_scpm_mgr_send_mmpm_sdc_vote(sns_mmpm_sdc_vote_cfg *sdc_vote_cfg)
{
  MMPM_STATUS sts = MMPM_STATUS_FAILED;
  MMPM_STATUS detailSts = MMPM_STATUS_FAILED;
  MmpmRscParamType mmpmRscParam;
  MmpmRscExtParamType mmpmRscExtParam;
  MmpmClkValType mmpmClkValType[1];
  MmpmClkReqType mmpmClkReqType;

  bool result = TRUE;

  mmpmClkValType[0].clkId.clkIdSlpi = sdc_vote_cfg->clock_id;
  mmpmClkValType[0].clkFreqHz = sdc_vote_cfg->clock_freq_hz;
  mmpmClkValType[0].freqMatch = MMPM_FREQ_AT_LEAST;

  mmpmClkReqType.numOfClk = 1;
  mmpmClkReqType.pClkArray = mmpmClkValType;

  mmpmRscParam.rscId = MMPM_RSC_ID_CORE_CLK;
  mmpmRscParam.rscParam.pCoreClk = &mmpmClkReqType;

  mmpmRscExtParam.apiType = MMPM_API_TYPE_ASYNC;
  mmpmRscExtParam.numOfReq = 1;
  mmpmRscExtParam.pReqArray = &mmpmRscParam;
  mmpmRscExtParam.pStsArray = &detailSts;

  sts = MMPM_Request_Ext(sdc_vote_cfg->client_id, &mmpmRscExtParam);
  if(MMPM_STATUS_SUCCESS != sts)
  {
    SNS_PRINTF(ERROR, sns_fw_printf, "sdc_vote: clientID %u, returned %u",
        sdc_vote_cfg->client_id, detailSts);
    result = false;
  }
  return result;
}
#endif

SNS_SECTION(".text.sns") static void
sns_pwr_scpm_mgr_sdc_vote()
{
#ifdef SNS_SDC_CLOCK_VOTING_ENABLE
  sns_mmpm_sdc_vote_cfg sdc_vote_cfg;
  uint32_t scc_sdc_proc_fclk_clk;
  uint32_t scc_smem_clk;
  uint32_t scc_ccd_clk;

  if(SNS_Q6_MCPS_DEFAULT == sns_pwr_scpm_mgr.curr_mcps)
  {
    scc_sdc_proc_fclk_clk = SDC_PROC_FCLK_HZ_DEFAULT;
    scc_smem_clk = SMEM_CLK_HZ_DEFAULT;
    scc_ccd_clk = CCD_CLK_HZ_DEFAULT;
  }
  else if((SNS_Q6_MCPS_LOW_SVS_DEFAULT == sns_pwr_scpm_mgr.curr_mcps) ||
          (sns_pwr_scpm_mgr.mcps_config.low_svs == sns_pwr_scpm_mgr.curr_mcps))
  {
    scc_sdc_proc_fclk_clk = SDC_PROC_FCLK_HZ_LOW_SVS;
    scc_smem_clk = SMEM_CLK_HZ_LOW_SVS;
    scc_ccd_clk = CCD_CLK_HZ_LOW_SVS;
  }
  else if((SNS_Q6_MCPS_SVS_DEFAULT == sns_pwr_scpm_mgr.curr_mcps) ||
          (sns_pwr_scpm_mgr.mcps_config.svs == sns_pwr_scpm_mgr.curr_mcps))
  {
    scc_sdc_proc_fclk_clk = SDC_PROC_FCLK_HZ_SVS;
    scc_smem_clk = SMEM_CLK_HZ_SVS;
    scc_ccd_clk = CCD_CLK_HZ_SVS;
  }
  else if((SNS_Q6_MCPS_NOMINAL_DEFAULT == sns_pwr_scpm_mgr.curr_mcps) ||
          (sns_pwr_scpm_mgr.mcps_config.nominal == sns_pwr_scpm_mgr.curr_mcps))
  {
    scc_sdc_proc_fclk_clk = SDC_PROC_FCLK_HZ_NOMINAL;
    scc_smem_clk = SMEM_CLK_HZ_NOMINAL;
    scc_ccd_clk = CCD_CLK_HZ_NOMINAL;
  }
  else if((SNS_Q6_MCPS_TURBO_DEFAULT == sns_pwr_scpm_mgr.curr_mcps) ||
          (sns_pwr_scpm_mgr.mcps_config.turbo == sns_pwr_scpm_mgr.curr_mcps))
  {
    scc_sdc_proc_fclk_clk = SDC_PROC_FCLK_HZ_TURBO;
    scc_smem_clk = SMEM_CLK_HZ_TURBO;
    scc_ccd_clk = CCD_CLK_HZ_TURBO;
  }
  else
  {
    SNS_PRINTF(ERROR, sns_fw_printf, "sdc_vote: received unknown MCPS vote %d",
        sns_pwr_scpm_mgr.curr_mcps);
    return;
  }

  if(scc_sdc_proc_fclk_clk != sns_pwr_scpm_mgr.sdc_cfg.scc_sdc_proc_fclk_clk)
  {
    sns_pwr_scpm_mgr.sdc_cfg.scc_sdc_proc_fclk_clk = scc_sdc_proc_fclk_clk;

    sdc_vote_cfg.clock_id = MMPM_CLK_ID_SLPI_SDC_CORE;
    sdc_vote_cfg.clock_freq_hz = sns_pwr_scpm_mgr.sdc_cfg.scc_sdc_proc_fclk_clk;
    sdc_vote_cfg.client_id = sns_pwr_scpm_mgr.sdc_client_id;
    if (TRUE == sns_pwr_scpm_mgr_send_mmpm_sdc_vote(&sdc_vote_cfg))
    {
      SNS_PRINTF(HIGH, sns_fw_printf, "sdc_vote: sdc_clock=%d MHz",
        sns_pwr_scpm_mgr.sdc_cfg.scc_sdc_proc_fclk_clk/1000000);
    }
  }

  if(scc_smem_clk != sns_pwr_scpm_mgr.sdc_cfg.scc_smem_clk)
  {
    sns_pwr_scpm_mgr.sdc_cfg.scc_smem_clk = scc_smem_clk;

    sdc_vote_cfg.clock_id = MMPM_CLK_ID_SLPI_SRAM_CORE;
    sdc_vote_cfg.clock_freq_hz = sns_pwr_scpm_mgr.sdc_cfg.scc_smem_clk;
    sdc_vote_cfg.client_id = sns_pwr_scpm_mgr.sram_client_id;
    if (TRUE == sns_pwr_scpm_mgr_send_mmpm_sdc_vote(&sdc_vote_cfg))
    {
      SNS_PRINTF(HIGH, sns_fw_printf, "sdc_vote: SRAM_clock=%d MHz",
        sns_pwr_scpm_mgr.sdc_cfg.scc_smem_clk/1000000);
    }
  }

  if(scc_ccd_clk != sns_pwr_scpm_mgr.sdc_cfg.scc_ccd_clk)
  {
    sns_pwr_scpm_mgr.sdc_cfg.scc_ccd_clk = scc_ccd_clk;

    sdc_vote_cfg.clock_id = MMPM_CLK_ID_SLPI_CCD_CORE;
    sdc_vote_cfg.clock_freq_hz = sns_pwr_scpm_mgr.sdc_cfg.scc_ccd_clk;
    sdc_vote_cfg.client_id = sns_pwr_scpm_mgr.ccd_client_id;
    if (TRUE == sns_pwr_scpm_mgr_send_mmpm_sdc_vote(&sdc_vote_cfg))
    {
      SNS_PRINTF(HIGH, sns_fw_printf, "sdc_vote: ccd_clock=%d MHz",
        sns_pwr_scpm_mgr.sdc_cfg.scc_ccd_clk/1000000);
    }
  }
#endif
}
/**
 * If the current SLPI SSC Q6 MCPS does not match new mcps vote, send a
 * request to the SCPM to update the vote
 *
 * @param[i]  mcps   New MCPS vote
 */
SNS_SECTION(".text.sns") static bool
sns_pwr_scpm_mgr_update_mcps_vote()
{
  MMPM_STATUS req_result = MMPM_STATUS_SUCCESS;
  MmpmRscExtParamType req_rsc_param;
  MMPM_STATUS result_status[1];
  MmpmRscParamType req_param[1];
  MmpmMipsReqType mcps_param;
  bool ret_val = false;
  uint32_t mcps = sns_pwr_scpm_mgr_check_clients_for_speedup()?sns_pwr_scpm_mgr.mcps_config.turbo:
    sns_pwr_scpm_mgr.thread_utilization_mcps;

  /* Only allow Nominal/Turbo modes on SDM845 V1 due to HW bug */
  if(sns_pwr_scpm_mgr_check_chip_and_version(sns_pwr_scpm_mgr.chip, sns_pwr_scpm_mgr.version))
  {
    if(mcps < sns_pwr_scpm_mgr.mcps_config.nominal)
    {
      mcps = sns_pwr_scpm_mgr.mcps_config.nominal;
    }
  }

  if(mcps != sns_pwr_scpm_mgr.curr_mcps)
  {
    SNS_ISLAND_EXIT();

    sns_pwr_scpm_mgr.curr_mcps = mcps;

    mcps_param.mipsTotal = mcps;
    /* unit is in Million cycles per sec */
    mcps_param.mipsPerThread = mcps / NUM_HW_THREAD;
    mcps_param.codeLocation = MMPM_BW_PORT_ID_ADSP_MASTER;
    mcps_param.reqOperation = MMPM_MIPS_REQUEST_CPU_CLOCK_ONLY;

    req_param[0].rscId = MMPM_RSC_ID_MIPS_EXT;
    req_param[0].rscParam.pMipsExt = &mcps_param;

    req_rsc_param.apiType = MMPM_API_TYPE_ASYNC;
    req_rsc_param.numOfReq = 1;
    req_rsc_param.reqTag = 0;
    req_rsc_param.pStsArray = result_status;
    req_rsc_param.pReqArray = req_param;

    req_result = MMPM_Request_Ext(sns_pwr_scpm_mgr.ssc_q6_client_id, &req_rsc_param);
    ret_val = (MMPM_STATUS_SUCCESS == req_result);
    SNS_PRINTF(HIGH, sns_fw_printf, "Set the SSC MIPS Requirement: %d", sns_pwr_scpm_mgr.curr_mcps);
    SNS_PROFILE(SNS_PWR_SCPM_MGR_SET_Q6_MCPS, 1, mcps);

    sns_pwr_scpm_mgr_sdc_vote();
  }
  return ret_val;
}

/**
 * Check the thread utilizations for every thread and update the mcps vote
 * if required
 *
 * @param[i] max_thread_utilization   Max thread utilization from all worker threads
 */
SNS_SECTION(".text.sns") void
sns_pwr_scpm_mgr_compare_utilization_threshold(uint32_t max_thread_utilization)
{
  /* When thread utilization increases beyond the threshold bump up the MCPS
     request to not drop events. As thread utilization reduces, decrease
     the MCPS request down per step */
  sns_time current_time = sns_get_system_time();
  uint32_t prev_mcps = sns_pwr_scpm_mgr.thread_utilization_mcps;

  if(max_thread_utilization > SNS_HIGH_THREAD_UTILIZATION_THRESHOLD)
  {
    sns_pwr_scpm_mgr.low_run_len = 0;
    
    if((current_time - sns_pwr_scpm_mgr.high_vote_ts)
          > sns_convert_ns_to_ticks(SNS_MCPS_VOTE_TIME_WINDOW))
    {
      /* Reset the high vote run length if the time delta
         between votes is > time window */
      sns_pwr_scpm_mgr.high_run_len = 1;
    }
    else
    {
      sns_pwr_scpm_mgr.high_run_len += 1;

      if((sns_pwr_scpm_mgr.thread_utilization_mcps <= sns_pwr_scpm_mgr.mcps_config.low_svs) &&
          (sns_pwr_scpm_mgr.high_run_len > SNS_HIGH_THREAD_UTILIZATION_RUN_LEN_THRESH_SVS)) 
      {
        sns_pwr_scpm_mgr.thread_utilization_mcps = sns_pwr_scpm_mgr.mcps_config.svs;
      }
      else if((sns_pwr_scpm_mgr.thread_utilization_mcps <= sns_pwr_scpm_mgr.mcps_config.svs) &&
             (sns_pwr_scpm_mgr.high_run_len > SNS_HIGH_THREAD_UTILIZATION_RUN_LEN_THRESH_NOM)) 
        {
          sns_pwr_scpm_mgr.thread_utilization_mcps = sns_pwr_scpm_mgr.mcps_config.nominal;
        }
      else if((sns_pwr_scpm_mgr.thread_utilization_mcps <= sns_pwr_scpm_mgr.mcps_config.nominal) &&
             (sns_pwr_scpm_mgr.high_run_len > SNS_HIGH_THREAD_UTILIZATION_RUN_LEN_THRESH_TURBO)) 
        {
          sns_pwr_scpm_mgr.thread_utilization_mcps = sns_pwr_scpm_mgr.mcps_config.turbo;
        }
      /* Update mcps vote if it has changed */
      if (prev_mcps != sns_pwr_scpm_mgr.thread_utilization_mcps)
        {
        sns_pwr_scpm_mgr_update_mcps_vote();
          sns_pwr_scpm_mgr.high_run_len = 0;
        }
      }
    sns_pwr_scpm_mgr.high_vote_ts = current_time;
  }
  else if(max_thread_utilization < SNS_LOW_THREAD_UTILIZATION_THRESHOLD)
  {
    sns_pwr_scpm_mgr.high_run_len = 0;

    if((current_time - sns_pwr_scpm_mgr.low_vote_ts)
          > sns_convert_ns_to_ticks(SNS_MCPS_VOTE_TIME_WINDOW))
    {
      sns_pwr_scpm_mgr.low_run_len = 1;
    }
    else
    {
      sns_pwr_scpm_mgr.low_run_len += 1;
      if (sns_pwr_scpm_mgr.low_run_len > SNS_LOW_THREAD_UTILIZATION_RUN_LEN_THRESH)
      {
        if(sns_pwr_scpm_mgr.thread_utilization_mcps == sns_pwr_scpm_mgr.mcps_config.turbo)
        {
          sns_pwr_scpm_mgr.thread_utilization_mcps = sns_pwr_scpm_mgr.mcps_config.nominal;
        }
        else if(sns_pwr_scpm_mgr.thread_utilization_mcps == sns_pwr_scpm_mgr.mcps_config.nominal)
        {
          sns_pwr_scpm_mgr.thread_utilization_mcps = sns_pwr_scpm_mgr.mcps_config.svs;
        }
        else if(sns_pwr_scpm_mgr.thread_utilization_mcps == sns_pwr_scpm_mgr.mcps_config.svs)
        {
          sns_pwr_scpm_mgr.thread_utilization_mcps = sns_pwr_scpm_mgr.mcps_config.low_svs;
        }
        if(prev_mcps != sns_pwr_scpm_mgr.thread_utilization_mcps)
        {
          sns_pwr_scpm_mgr_update_mcps_vote();
          sns_pwr_scpm_mgr.low_run_len = 0;
        }
      }
    }
    sns_pwr_scpm_mgr.low_vote_ts = current_time;
  }
}

/**
 * Update the thread utilization for the provided thread
 */
SNS_SECTION(".text.sns") void
sns_pwr_scpm_mgr_update_thread_utilization(
 sns_osa_thread *thread,
 uint32_t thread_utilization
)
{
  sns_isafe_list_iter  iter;
  sns_pwr_scpm_thread_item *thread_item = NULL;
  uint32_t max_thread_utilization = 0;
  bool is_thread_present = false;

  sns_osa_lock_acquire(sns_pwr_scpm_mgr.scpm_mgr_lock);

  sns_isafe_list_iter_init(&iter, &sns_pwr_scpm_mgr.thread_list, true);

  /* Check if this thread is already in the list */
  for(;
      NULL != sns_isafe_list_iter_curr(&iter);
      sns_isafe_list_iter_advance(&iter))
  {
    thread_item = (sns_pwr_scpm_thread_item *)sns_isafe_list_iter_get_curr_data(&iter);

    if(thread == thread_item->thread)
    {
      is_thread_present = true;
      thread_item->thread_utilization = thread_utilization;
      thread_item->last_update_ts = sns_get_system_time();
    }

    if(thread_item->thread_utilization > max_thread_utilization)
    {
      sns_time now = sns_get_system_time();

      //If thread utilization not updates since the threshold reset it
      if ( now - thread_item->last_update_ts > SNS_PWR_SCPM_THREAD_UTILIZATION_UPDATE_THRESHOLD )
      {
        thread_item->thread_utilization = 0;
        thread_item->last_update_ts = sns_get_system_time();
      }
      else
      {
        max_thread_utilization = thread_item->thread_utilization;
      }
    }
  }

  /* Create a new thread item if its not already present on the list */
  if(false == is_thread_present)
  {
    thread_item = sns_malloc(SNS_HEAP_ISLAND, sizeof(*thread_item));
    if(NULL == thread_item)
    {
      SNS_ISLAND_EXIT();
      thread_item = sns_malloc(SNS_HEAP_MAIN, sizeof(*thread_item));
    }
    SNS_ASSERT(thread_item != NULL);

    thread_item->thread = thread;
    thread_item->thread_utilization = thread_utilization;
    thread_item->last_update_ts = sns_get_system_time();

    if(thread_utilization > max_thread_utilization)
    {
      max_thread_utilization = thread_utilization;
    }
    sns_isafe_list_item_init(&thread_item->list_entry, thread_item);
    sns_isafe_list_iter_insert(&iter, &thread_item->list_entry, true);
  }
  
  /* For debug */
  //SNS_PRINTF(HIGH, sns_fw_printf, "Thread: %x, Utilization:%d, Max Thread Utilization:%d", thread, thread_utilization, max_thread_utilization);

  sns_pwr_scpm_mgr_compare_utilization_threshold(max_thread_utilization);

  sns_osa_lock_release(sns_pwr_scpm_mgr.scpm_mgr_lock);
}

/**
 * Register with MMPM to send requests to SCPM
 *
 * @param[i] core_id_type     Core type
 * @param[i] core_name        Core name
 *
 * @return
 * Client id to the specified core in SCPM
 *
 */
static uint32_t
sns_pwr_scpm_mgr_register_core(MmpmCoreIdType core_id_type, char *core_name)
{
   MmpmRegParamType mmpmRegParam;

   mmpmRegParam.rev = MMPM_REVISION;
   mmpmRegParam.coreId = core_id_type;
   mmpmRegParam.instanceId = MMPM_CORE_INSTANCE_0;
   mmpmRegParam.MMPM_Callback = NULL;
   mmpmRegParam.pClientName = core_name;
   mmpmRegParam.pwrCtrlFlag = PWR_CTRL_PERIODIC_CLIENT;
   mmpmRegParam.callBackFlag = CALLBACK_NONE;
   mmpmRegParam.cbFcnStackSize = 128;

   return MMPM_Register_Ext(&mmpmRegParam);
}

/**
 * Initialize the sensors power scpm manager.
 */
sns_rc
sns_pwr_scpm_mgr_init(void)
{
  sns_osa_lock_attr   lock_attr;
  sns_rc rc = SNS_RC_SUCCESS;

  sns_pwr_scpm_mgr.ssc_q6_client_id = sns_pwr_scpm_mgr_register_core(MMPM_CORE_ID_LPASS_ADSP, "SSC_Q6");
  SNS_ASSERT(0 != sns_pwr_scpm_mgr.ssc_q6_client_id);

#ifdef SNS_SDC_CLOCK_VOTING_ENABLE
  sns_pwr_scpm_mgr.sdc_client_id = sns_pwr_scpm_mgr_register_core(MMPM_CORE_ID_SLPI_SDC, "SSC_Q6");
  SNS_ASSERT(0 != sns_pwr_scpm_mgr.sdc_client_id);

  sns_pwr_scpm_mgr.sram_client_id = sns_pwr_scpm_mgr_register_core(MMPM_CORE_ID_SLPI_SRAM, "SSC_Q6");
  SNS_ASSERT(0 != sns_pwr_scpm_mgr.sram_client_id);

  sns_pwr_scpm_mgr.ccd_client_id = sns_pwr_scpm_mgr_register_core(MMPM_CORE_ID_SLPI_CCD, "SSC_Q6");
  SNS_ASSERT(0 != sns_pwr_scpm_mgr.ccd_client_id);
#endif
  /* Initialize list of threads */
  sns_isafe_list_init(&sns_pwr_scpm_mgr.thread_list);

  /* Initialize power scpm manager lock */
  sns_osa_lock_attr_init(&lock_attr);
  sns_osa_lock_attr_set_memory_partition(&lock_attr, SNS_OSA_MEM_TYPE_ISLAND);
  rc = sns_osa_lock_create(&lock_attr, &sns_pwr_scpm_mgr.scpm_mgr_lock);
  SNS_ASSERT(SNS_RC_SUCCESS == rc);

  sns_pwr_scpm_mgr.high_run_len = 0;
  sns_pwr_scpm_mgr.low_run_len = 0;
  sns_pwr_scpm_mgr.high_vote_ts = 0;
  sns_pwr_scpm_mgr.low_vote_ts = 0;

  /* Set the default MCPS config */
  sns_pwr_scpm_mgr.mcps_config.low_svs = SNS_Q6_MCPS_LOW_SVS_DEFAULT;
  sns_pwr_scpm_mgr.mcps_config.svs = SNS_Q6_MCPS_SVS_DEFAULT;
  sns_pwr_scpm_mgr.mcps_config.nominal = SNS_Q6_MCPS_NOMINAL_DEFAULT;
  sns_pwr_scpm_mgr.mcps_config.turbo = SNS_Q6_MCPS_TURBO_DEFAULT;

  // Initialize at Nominal
  sns_pwr_scpm_mgr.thread_utilization_mcps = SNS_Q6_MCPS_NOMINAL_DEFAULT;

  /* Read the chip version and chip id */
  sns_pwr_scpm_mgr.version = DalChipInfo_ChipVersion();
  sns_pwr_scpm_mgr.chip = DalChipInfo_ChipId();

  /* Initialize SSC Q6 MCPS vote */
  sns_pwr_scpm_mgr_update_mcps_vote();

  return rc;
}

sns_scpm_mgr_client * sns_pwr_scpm_mgr_create_client()
{
  sns_scpm_mgr_client *client = sns_malloc(SNS_HEAP_ISLAND, sizeof(sns_scpm_mgr_client));
  if (NULL != client)
  {
    sns_isafe_list_iter iter;
    if (sns_pwr_scpm_mgr.client_list.item.list != &sns_pwr_scpm_mgr.client_list)
    {
      sns_isafe_list_init(&sns_pwr_scpm_mgr.client_list);
    }
    sns_isafe_list_item_init(&client->list_entry, client);
    sns_isafe_list_iter_init(&iter, &sns_pwr_scpm_mgr.client_list, true);
    sns_osa_lock_acquire(sns_pwr_scpm_mgr.scpm_mgr_lock);
    sns_isafe_list_iter_insert(&iter, &client->list_entry, false);
    client->require_speedup = false;
    sns_osa_lock_release(sns_pwr_scpm_mgr.scpm_mgr_lock);
  }
  return client;
}

SNS_SECTION(".text.sns") sns_rc
sns_pwr_scpm_mgr_mcps_bump_up(sns_scpm_mgr_client *client)
{
  sns_rc ret_val = SNS_RC_INVALID_VALUE;
  if (NULL != client)
  {
    if (!sns_island_is_island_ptr((intptr_t)client))
    {
      SNS_ISLAND_EXIT();
    }
    sns_osa_lock_acquire(sns_pwr_scpm_mgr.scpm_mgr_lock);
    client->require_speedup = true;
    sns_pwr_scpm_mgr_update_mcps_vote();
    sns_osa_lock_release(sns_pwr_scpm_mgr.scpm_mgr_lock);
  }
  return ret_val;
}

SNS_SECTION(".text.sns") sns_rc
sns_pwr_scpm_mgr_mcps_bump_down(sns_scpm_mgr_client *client)
{
  sns_rc ret_val = SNS_RC_INVALID_VALUE;
  if (NULL != client)
  {
    if (!sns_island_is_island_ptr((intptr_t)client))
    {
      SNS_ISLAND_EXIT();
    }
    sns_osa_lock_acquire(sns_pwr_scpm_mgr.scpm_mgr_lock);
    client->require_speedup = false;
    sns_pwr_scpm_mgr_update_mcps_vote();
    sns_osa_lock_release(sns_pwr_scpm_mgr.scpm_mgr_lock);
  }
  return ret_val;
}

SNS_SECTION(".text.sns") sns_rc
sns_pwr_scpm_mgr_set_mcps_vote(
  sns_scpm_mgr_client *client, sns_q6_mcps_level mcps_level)
{
  sns_rc ret_val = SNS_RC_SUCCESS;
  uint32_t mcps = 0;
  if (NULL != client)
  {
    if (!sns_island_is_island_ptr((intptr_t)client))
    {
      SNS_ISLAND_EXIT();
    }
    sns_osa_lock_acquire(sns_pwr_scpm_mgr.scpm_mgr_lock);
    client->require_speedup = false;
    sns_pwr_scpm_mgr.high_run_len = 0;
    sns_pwr_scpm_mgr.low_run_len = 0;

    switch (mcps_level)
    {
      case SNS_Q6_MCPS_DEFAULT_LEVEL:
        mcps = SNS_Q6_MCPS_DEFAULT;
        break;
      case SNS_Q6_MCPS_LOW_SVS_LEVEL:
        mcps = sns_pwr_scpm_mgr.mcps_config.low_svs;
        break;
      case SNS_Q6_MCPS_SVS_LEVEL:
        mcps = sns_pwr_scpm_mgr.mcps_config.svs;
        break;
      case SNS_Q6_MCPS_NOMINAL_LEVEL:
        mcps = sns_pwr_scpm_mgr.mcps_config.nominal;
        break;
      case SNS_Q6_MCPS_TURBO_LEVEL:
        mcps = sns_pwr_scpm_mgr.mcps_config.turbo;
        break;
      default:
        SNS_PRINTF(ERROR, sns_fw_printf,
            "Received unsupported MIPS level: %d", mcps_level);
        ret_val = SNS_RC_INVALID_VALUE;
        break;
    }
    if (SNS_RC_SUCCESS == ret_val)
    {
      sns_pwr_scpm_mgr.thread_utilization_mcps = mcps;
      sns_pwr_scpm_mgr_update_mcps_vote();
    }
    sns_osa_lock_release(sns_pwr_scpm_mgr.scpm_mgr_lock);
  }
  return ret_val;
}

SNS_SECTION(".text.sns") uint32_t
sns_pwr_scpm_mgr_get_current_mcps_vote()
{
  return sns_pwr_scpm_mgr.curr_mcps;
}

void 
sns_pwr_scpm_mgr_store_mcps_config(const sns_scpm_mgr_mcps_config *mcps_config)
{
  sns_osa_lock_acquire(sns_pwr_scpm_mgr.scpm_mgr_lock);
  
  sns_pwr_scpm_mgr.mcps_config.low_svs = mcps_config->low_svs;
  sns_pwr_scpm_mgr.mcps_config.svs = mcps_config->svs;
  sns_pwr_scpm_mgr.mcps_config.nominal = mcps_config->nominal;
  sns_pwr_scpm_mgr.mcps_config.turbo = mcps_config->turbo;

  sns_osa_lock_release(sns_pwr_scpm_mgr.scpm_mgr_lock);
}
