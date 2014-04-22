/**
 * sns_pwr_sleep_mgr.c
 *
 * The Sensors Power Sleep Manager implementation
 *
 * Copyright (c) 2017-2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * $Id: //components/rel/ssc.slpi/5.1/utils/pwr_sleep_mgr/hexagon/sns_pwr_sleep_mgr.c#2 $
 * $DateTime: 2020/01/09 06:46:23 $
 * $Change: 22125271 $
 *
 **/

#include <math.h>
#include <stdint.h>
#include <string.h>
#include "sns_assert.h"
#include "sns_fw_sensor_instance.h"
#include "sns_isafe_list.h"
#include "sns_island.h"
#include "sns_memmgr.h"
#include "sns_osa_lock.h"
#include "sns_pb_util.h"
#include "sns_printf_int.h"
#include "sns_profiler.h"
#include "sns_pwr_sleep_mgr.h"
#include "sns_rc.h"
#include "sns_sensor.h"
#include "sns_sensor_instance.h"
#include "sns_std_sensor.pb.h"
#include "sns_types.h"

#include "npa.h"
#include "npa_resource.h"

/**
 * Macros defined for setting and removing TCM retention.
 */
#define SLEEP_TCM_LPR_NONRET_ONLY       0
#define SLEEP_TCM_LPR_RETENTION_ONLY    1

#define MAX_PHYSICAL_SENSOR_COUNT       25

/*
 * If Defined : sleep wakeup rate validation is called after every physical
 * config event received and removed.
 */
//#define ENABLE_WAKEUP_RATE_VALIDATION

/*
 * If Defined - Code ASSERTS if sleep wakeup validation logic fails
 * If not Defined - Code Corrects sleep wakeup rate if sleep wakeup validation fails
 */
//#define SNS_PWR_SLEEP_DEBUG

/*=============================================================================
  Type Definitions
  ===========================================================================*/

/**
 * Private instance wakeup rate list item.
 */
typedef struct sns_pwr_sleep_instance_item
{
  /* Sensor UID */
  sns_sensor_uid suid;
  /* Sensors instance pointer */
  sns_sensor_instance *instance;
  /* This sensor instance's Q6 wakeup rate in Hz */
  float               q6_wakeup_rate;
  /* true for an SDC sensor routing data through the DAE
   * false otherwise
   */
  bool                dae_sensor;
  /* True if instance and its data streams are in island */
  bool                island_instance;
  /* Instance list entry */
  sns_isafe_list_item       list_entry;
}sns_pwr_sleep_instance_item;

/**
 * Private latency list item.
 */
typedef struct sns_pwr_sleep_latency_item
{
  /* Client pointer to identify the sensor/instance */
  void                *client;
  /* This clients latency request in uS*/
  uint32_t            latency_us;
  /* Instance list entry */
  sns_isafe_list_item       list_entry;
}sns_pwr_sleep_latency_item;

/**
 * Private state definition.
 */
struct sns_fw_pwr_sleep_mgr
{
  /* NPA client handle to the Sleep driver max duration node */
  npa_client_handle max_duration_handle;
  /* NPA client handle to the Sleep driver normal latency node */
  npa_client_handle normal_latency_handle;
  /* NPA client handle to the Sleep driver TCM collapse node */
  npa_client_handle     tcm_npa_handle;
  /* UNPA client handle to control island sleep modes */
  int32_t           unpa_client;
  /* Current effective normal mode Q6 wakeup rate in Hz */
  float             normal_mode_wakeup_rate;
  /* Current effective island mode Q6 wakeup rate in Hz */
  float             island_mode_wakeup_rate;
  /* Current wakeup rate of sensors routing data through the DAE sensor */
  float             curr_dae_wakeup_rate;
  /* Current island status of sensor instance which is routing data through
   * the DAE sensor. Needed for old vote removal. */
  bool              is_curr_dae_instance_island;
  /* Current latency vote */
  uint32_t          latency_us;
  /* List of sensor instances aggregating to the effective Q6 wakeup rate */
  struct sns_isafe_list  instance_list;
  /* List of sensor/instances aggregating to the effective latency */
  struct sns_isafe_list  latency_list;
  /* True if island mode is enabled, False otherwise */
  bool              island_enabled;
  /* Sensor power sleep manager mutex */
  sns_osa_lock*     sleep_mgr_lock;
} ;

/**
 * Private state of the Power Sleep Manager Service.
 */
static struct sns_fw_pwr_sleep_mgr sns_pwr_sleep_mgr SNS_SECTION(".data.sns");


#ifdef SNS_PWR_SLEEP_DEBUG
typedef struct sns_pwr_sleep_debug_item
{
  sns_time timestamp; 
  bool  island_instance;
  float in_wakeup_rate;
  float out_wakeup_rate;
  float total_normal_wakeup_rate;
  float total_island_wakeup_rate;
  sns_sensor_uid suid;
} sns_pwr_sleep_debug_item;

#define DEBUG_ARRAY_LEN 10
sns_pwr_sleep_debug_item sns_pwr_sleep_debug_array[DEBUG_ARRAY_LEN] SNS_SECTION(".data.sns");
uint32_t sns_pwr_sleep_debug_index SNS_SECTION(".data.sns");
#endif

#ifdef ENABLE_WAKEUP_RATE_VALIDATION
//Local variable for validating effective wake-up rate
float total_normal_wakeup_rate SNS_SECTION(".data.sns");
float total_island_wakeup_rate SNS_SECTION(".data.sns");
float total_dae_wakeup_rate SNS_SECTION(".data.sns");
bool  is_dae_island SNS_SECTION(".data.sns");
#endif
/*=============================================================================
  Forward Declarations
  ===========================================================================*/
extern void sns_pwr_sleep_mgr_disable_lpm_plus(int32_t unpa_client);

extern void sns_pwr_sleep_mgr_enable_lpm_plus(int32_t unpa_client);

extern int32_t sns_pwr_sleep_mgr_create_unpa_handle(void);

/**
 * Enable/disable island mode sleep modes based on Q6 wakeup rates
 * in island mode. Send the request to the sleep driver
 */
SNS_SECTION(".text.sns") static void
sns_pwr_sleep_mgr_set_island_mode_sleep_mode(float wakeup_rate)
{
  SNS_PRINTF(MED, sns_fw_printf,
      "Set island mode sleep mode for wakeup rate: %d/100",
      (int)(wakeup_rate*100));
  if(wakeup_rate > SNS_PWR_SLEEP_MGR_ISLAND_SLEEP_MODE_THRESHOLD_HZ)
  {
    sns_pwr_sleep_mgr_disable_lpm_plus(sns_pwr_sleep_mgr.unpa_client);
  }
  else
  {
    sns_pwr_sleep_mgr_enable_lpm_plus(sns_pwr_sleep_mgr.unpa_client);
  }
}

/**
 * Set tcm node to retention
 */
void
sns_pwr_sleep_mgr_unblock_tcm_pwr_collapse()
{
  sns_osa_lock_acquire(sns_pwr_sleep_mgr.sleep_mgr_lock);
  if(NULL != sns_pwr_sleep_mgr.tcm_npa_handle)
  {
    npa_issue_required_request(sns_pwr_sleep_mgr.tcm_npa_handle,
        SLEEP_TCM_LPR_NONRET_ONLY);
    SNS_PRINTF(HIGH, sns_fw_printf, "unblock tcm_pwr_collapse");
  }
  sns_osa_lock_release(sns_pwr_sleep_mgr.sleep_mgr_lock);
}

/**
 * Remove tcm retention. This leads to TCM collapse.
 */
void
sns_pwr_sleep_mgr_block_tcm_pwr_collapse()
{
  sns_osa_lock_acquire(sns_pwr_sleep_mgr.sleep_mgr_lock);
  if(NULL != sns_pwr_sleep_mgr.tcm_npa_handle)
  {
    npa_issue_required_request(sns_pwr_sleep_mgr.tcm_npa_handle,
        SLEEP_TCM_LPR_RETENTION_ONLY);
    SNS_PRINTF(HIGH, sns_fw_printf, "block tcm_pwr_collapse");
  }
  sns_osa_lock_release(sns_pwr_sleep_mgr.sleep_mgr_lock);
}

/**
 * Set the Q6 wakeup interval based on the Q6 wakeup rate and
 * send the request to the sleep driver NPA nodes
 */
SNS_SECTION(".text.sns") static void
sns_pwr_sleep_mgr_set_normal_mode_wakeup_interval(float wakeup_rate)
{
  uint32_t q6_wakeup_intvl_usec = 0;
  sns_isafe_list_iter  iter;

  SNS_ISLAND_EXIT();

  sns_isafe_list_iter_init(&iter, &sns_pwr_sleep_mgr.instance_list, true);

  /* Calcualte the wakeup interval in usecs from the effective Q6 wakeup rate */
  if(wakeup_rate > 1)
  {
    q6_wakeup_intvl_usec = (uint32_t)((USEC_PER_SEC) / wakeup_rate);
  }
  else
  {
    /* Wakeup interval of 0 with sensor instances in the list is a special case
     * that applies to motion gated or CCD gated sensor instances.
     * Set the wakeup interval to a max value.
     */
    if(sns_isafe_list_iter_len(&iter) > 1)
    {
      q6_wakeup_intvl_usec = SNS_PWR_SLEEP_MGR_MAX_WAKEUP_INTVL_USEC;
    }
    else
    {
      q6_wakeup_intvl_usec = 0;
    }
  }

  if(0 == q6_wakeup_intvl_usec)
  {
    /* If the Q6 wakeup interval is 0, no clients are present, hence
     * release the wakeup rate requirement to the sleep driver allowing for deep sleep
     */
    SNS_PRINTF(MED, sns_fw_printf,
        "Release the Max Duration request for wakeup rate : %d/100",
        (int)(wakeup_rate*100));
    npa_complete_request(sns_pwr_sleep_mgr.max_duration_handle);
  }
  else
  {
    /* Update the sleep driver with the new Q6 wakeup interval */
    SNS_PRINTF(MED, sns_fw_printf, "Set new Q6 wakeup rate : %d/100 q6_wakeup_intvl_usec: %d",
        (int)(wakeup_rate*100), q6_wakeup_intvl_usec);
    npa_issue_required_request(sns_pwr_sleep_mgr.max_duration_handle,
        q6_wakeup_intvl_usec);
  }
  SNS_PROFILE(SNS_PWR_SLEEP_MGR_SET_Q6_WAKEUP_INTERVAL_USEC,
      1, q6_wakeup_intvl_usec);
}


SNS_SECTION(".text.sns") void
sns_pwr_sleep_mgr_validate_wakeup_rate( void )
{
#ifdef  ENABLE_WAKEUP_RATE_VALIDATION
  total_normal_wakeup_rate = 0.0;
  total_island_wakeup_rate = 0.0;
  total_dae_wakeup_rate = 0.0;
  is_dae_island = false;

  sns_sensor_instance *inst_item_array[MAX_PHYSICAL_SENSOR_COUNT];
  float wakeup_rate_array[MAX_PHYSICAL_SENSOR_COUNT];

  sns_isafe_list_iter iter;
  sns_pwr_sleep_instance_item *instance_item = NULL;
  uint8_t index;

  for(index =0; index<MAX_PHYSICAL_SENSOR_COUNT; index++)
  {
    inst_item_array[index] = NULL;
    wakeup_rate_array[index] = 0;
  }

  //Loop through instance list for all instances.
  for(sns_isafe_list_iter_init(&iter, &sns_pwr_sleep_mgr.instance_list, true);
      NULL != sns_isafe_list_iter_curr(&iter);
      sns_isafe_list_iter_advance(&iter))
  {
    instance_item = (sns_pwr_sleep_instance_item*)sns_isafe_list_iter_get_curr_data(&iter);

    for(index =0; index<MAX_PHYSICAL_SENSOR_COUNT; index++)
    {
      if (NULL == inst_item_array[index] )
      {
        inst_item_array[index]  = instance_item->instance;
      }

      if(instance_item->instance == inst_item_array[index] )
      {
        if(true == instance_item->dae_sensor)
        {
          if(instance_item->q6_wakeup_rate > total_dae_wakeup_rate)
          {
            total_dae_wakeup_rate = instance_item->q6_wakeup_rate;
            is_dae_island = instance_item->island_instance;
          }
        }
        else if (instance_item->q6_wakeup_rate > wakeup_rate_array[index])
        {
          if(instance_item->island_instance)
          {
            total_island_wakeup_rate -= wakeup_rate_array[index];
            wakeup_rate_array[index] = instance_item->q6_wakeup_rate;
            total_island_wakeup_rate += wakeup_rate_array[index];
          }
          else
          {
            total_normal_wakeup_rate -= wakeup_rate_array[index];
            wakeup_rate_array[index] = instance_item->q6_wakeup_rate;
            total_normal_wakeup_rate += wakeup_rate_array[index];
          }
        }

        break;
      }
    }

    //check and assert if index breched array bound.
  }

  //Update DAE accordingly
  if(is_dae_island) 
  {
     total_island_wakeup_rate += total_dae_wakeup_rate;
  }
  else 
  {
    total_normal_wakeup_rate += total_dae_wakeup_rate;
  }

  if (fabsf(total_island_wakeup_rate - sns_pwr_sleep_mgr.island_mode_wakeup_rate) > 1)
  {
     SNS_PRINTF(ERROR, sns_fw_printf, "Correct island_sleep_vote: old_vote(%d/1000), new_vote(%d/1000)",
        (uint32_t)(sns_pwr_sleep_mgr.island_mode_wakeup_rate * 1000), (uint32_t)(total_island_wakeup_rate * 1000));

#ifdef SNS_PWR_SLEEP_DEBUG
     SNS_ASSERT(0);
#else

     sns_pwr_sleep_mgr.island_mode_wakeup_rate = total_island_wakeup_rate;

     sns_pwr_sleep_mgr_set_island_mode_sleep_mode(
         sns_pwr_sleep_mgr.island_mode_wakeup_rate);
#endif
  }

  if (fabsf(total_normal_wakeup_rate - sns_pwr_sleep_mgr.normal_mode_wakeup_rate) > 1)
  {
     SNS_PRINTF(ERROR, sns_fw_printf, "Correct max_latency_vote: old_vote(%d/1000), new_vote(%d/1000)",
        (uint32_t)(sns_pwr_sleep_mgr.normal_mode_wakeup_rate * 1000), (uint32_t)(total_normal_wakeup_rate * 1000));

#ifdef SNS_PWR_SLEEP_DEBUG
     SNS_ASSERT(0);
#else
     sns_pwr_sleep_mgr.normal_mode_wakeup_rate = total_normal_wakeup_rate;

     if(sns_pwr_sleep_mgr.island_enabled)
     {
       sns_pwr_sleep_mgr_set_normal_mode_wakeup_interval(
         sns_pwr_sleep_mgr.normal_mode_wakeup_rate);
     }
     else
     {
       // island mode is blocked
       sns_pwr_sleep_mgr_set_normal_mode_wakeup_interval(
           sns_pwr_sleep_mgr.normal_mode_wakeup_rate +
           sns_pwr_sleep_mgr.island_mode_wakeup_rate);
     }
#endif
  }
#endif
  return;
}

/**
 * Calculate the new effective Q6 wakeup rate based on the incoming
 * and outgoing wakeup rates
 *
 * @param[i] in_freq_hz      Q6 wakeup rate to be added to the aggregate
 * @param[i] out_freq_hz     Q6 wakeup rate to tbe removed from the aggregate
 * @param[i] island_instance boolean flag indicating if it is an island instance
 */
SNS_SECTION(".text.sns") static void
sns_pwr_sleep_mgr_calc_eff_wakeup_rate(
  float in_freq_Hz,
  float out_freq_Hz,
  bool island_instance)
{
  float new_q6_wakeup_rate;

  if(island_instance)
  {
    new_q6_wakeup_rate = sns_pwr_sleep_mgr.island_mode_wakeup_rate;
  }
  else
  {
    new_q6_wakeup_rate = sns_pwr_sleep_mgr.normal_mode_wakeup_rate;
  }

  new_q6_wakeup_rate += (in_freq_Hz - out_freq_Hz);
  /* Correct for negative wakeup rate */
  if(new_q6_wakeup_rate < 0)
  {
    new_q6_wakeup_rate = 0;
  }

  SNS_ASSERT(new_q6_wakeup_rate < INT32_MAX);

  if(island_instance)
  {
    sns_pwr_sleep_mgr.island_mode_wakeup_rate = new_q6_wakeup_rate;
  }
  else
  {
    sns_pwr_sleep_mgr.normal_mode_wakeup_rate = new_q6_wakeup_rate;
  }

#ifdef SNS_PWR_SLEEP_DEBUG
  sns_pwr_sleep_debug_array[sns_pwr_sleep_debug_index].timestamp = sns_get_system_time();
  sns_pwr_sleep_debug_array[sns_pwr_sleep_debug_index].island_instance = island_instance;
  sns_pwr_sleep_debug_array[sns_pwr_sleep_debug_index].in_wakeup_rate = in_freq_Hz;
  sns_pwr_sleep_debug_array[sns_pwr_sleep_debug_index].out_wakeup_rate = out_freq_Hz;
  sns_pwr_sleep_debug_array[sns_pwr_sleep_debug_index].total_island_wakeup_rate = sns_pwr_sleep_mgr.island_mode_wakeup_rate;
  sns_pwr_sleep_debug_array[sns_pwr_sleep_debug_index].total_normal_wakeup_rate= sns_pwr_sleep_mgr.normal_mode_wakeup_rate;
  sns_pwr_sleep_debug_index++;
  sns_pwr_sleep_debug_index = sns_pwr_sleep_debug_index%DEBUG_ARRAY_LEN;
#endif

  if (sns_pwr_sleep_mgr.island_enabled)
  {
    // island mode is unblocked
    if(island_instance)
    {
      sns_pwr_sleep_mgr_set_island_mode_sleep_mode(
          sns_pwr_sleep_mgr.island_mode_wakeup_rate);
    }
    else
    {
      sns_pwr_sleep_mgr_set_normal_mode_wakeup_interval(
        sns_pwr_sleep_mgr.normal_mode_wakeup_rate);
    }
  }
  else
  {
    // island mode is blocked
    sns_pwr_sleep_mgr_set_normal_mode_wakeup_interval(
        sns_pwr_sleep_mgr.normal_mode_wakeup_rate +
        sns_pwr_sleep_mgr.island_mode_wakeup_rate);
  }
}

/**
 * Calculate the new wakeup rate based on the incoming physical config
 *
 * @param[i] phy_sensor_config   Incoming Physical config
 * @param[i] new_wakeup_rate Handle to return new rate
 *
 * @return   the computed wake up rate
 */
SNS_SECTION(".text.sns") static sns_rc
sns_pwr_sleep_mgr_calc_new_wakeup_rate(
    sns_std_sensor_physical_config_event const *phy_sensor_config,
    float *new_wakeup_rate)
{
  sns_rc rc = SNS_RC_SUCCESS;

  SNS_ASSERT(phy_sensor_config != NULL);

  if(phy_sensor_config->has_sample_rate &&
     phy_sensor_config->sample_rate != 0)
  {
    /* Use DAE/FIFO watermark to calculate the sensor instance Q6 wakeup rate
     * depending on what the sensor instance supports
     */
    if(phy_sensor_config->has_DAE_watermark == true
        && phy_sensor_config->DAE_watermark != 0)
    {
      *new_wakeup_rate = (phy_sensor_config->sample_rate
          / phy_sensor_config->DAE_watermark);
    }
    else if(phy_sensor_config->has_water_mark == true
        && phy_sensor_config->water_mark != 0)
    {
      *new_wakeup_rate = (phy_sensor_config->sample_rate
          / phy_sensor_config->water_mark);
    }
    else
    {
      *new_wakeup_rate = phy_sensor_config->sample_rate;
    }
  }

  if ((*new_wakeup_rate < 0) || (*new_wakeup_rate > INT16_MAX))
  {
    SNS_PRINTF( ERROR, sns_fw_printf,
        "Received unsupported wakeup_rate=%d/1000", (int32_t)((*new_wakeup_rate)*1000));
    rc = SNS_RC_INVALID_VALUE;
    *new_wakeup_rate = 0;
  }
  return rc;
}

/**
 * Check to see if a sensor instance and all its data streams are in
 * island operation state
 *
 * @param[i] instance    Sensor instance pointer
 *
 * @return
 * true        If island and all its data streams are in island operation
 * false       Otherwise
 */
SNS_SECTION(".text.sns") static bool
sns_pwr_sleep_mgr_check_island_instance(sns_sensor_instance *instance)
{
  bool sns_rc = false;

  if(SNS_ISLAND_STATE_IN_ISLAND ==
      ((sns_fw_sensor_instance *)instance)->island_operation)
  {
    sns_rc = sns_instance_streams_in_island(instance);
  }

  return sns_rc;
}

SNS_SECTION(".text.sns") void
sns_pwr_sleep_mgr_update_dae_wakeup_rate( void )
{
  sns_isafe_list_iter iter;
  sns_pwr_sleep_instance_item *instance_item = NULL;
  sns_pwr_sleep_instance_item *max_dae_wakeup_rate_instance_item = NULL;
  float max_dae_wakeup_rate = 0;

  //Loop through instance list for all DAE instances.
  for(sns_isafe_list_iter_init(&iter, &sns_pwr_sleep_mgr.instance_list, true);
      NULL != sns_isafe_list_iter_curr(&iter);
      sns_isafe_list_iter_advance(&iter))
  {
    instance_item =
        (sns_pwr_sleep_instance_item*)sns_isafe_list_iter_get_curr_data(&iter);
    if(instance_item->dae_sensor &&
       ( instance_item->q6_wakeup_rate > max_dae_wakeup_rate))
    {
      max_dae_wakeup_rate = instance_item->q6_wakeup_rate;
      max_dae_wakeup_rate_instance_item = instance_item;
    }
  }

  if(NULL != max_dae_wakeup_rate_instance_item)
  {
#ifdef SNS_PWR_SLEEP_DEBUG
      sns_pwr_sleep_debug_array[sns_pwr_sleep_debug_index].suid = max_dae_wakeup_rate_instance_item->suid;
#endif
    //vote for max wakeup rate among existing DAE instances.
    sns_pwr_sleep_mgr_calc_eff_wakeup_rate(
        max_dae_wakeup_rate_instance_item->q6_wakeup_rate, 0,
        max_dae_wakeup_rate_instance_item->island_instance);
    //Remove curr vote.
    if(0 != sns_pwr_sleep_mgr.curr_dae_wakeup_rate)
    {
#ifdef SNS_PWR_SLEEP_DEBUG
      sns_pwr_sleep_debug_array[sns_pwr_sleep_debug_index].suid.sensor_uid[0] = 0xAB;
#endif
      sns_pwr_sleep_mgr_calc_eff_wakeup_rate(0,
          sns_pwr_sleep_mgr.curr_dae_wakeup_rate,
          sns_pwr_sleep_mgr.is_curr_dae_instance_island);
    }

    //Update curr vote.
    sns_pwr_sleep_mgr.curr_dae_wakeup_rate =
        max_dae_wakeup_rate_instance_item->q6_wakeup_rate;
    sns_pwr_sleep_mgr.is_curr_dae_instance_island =
        max_dae_wakeup_rate_instance_item->island_instance;
  }
  else
  {
    if(0 != sns_pwr_sleep_mgr.curr_dae_wakeup_rate)
    {
#ifdef SNS_PWR_SLEEP_DEBUG
      sns_pwr_sleep_debug_array[sns_pwr_sleep_debug_index].suid.sensor_uid[0] = 0xAB;
#endif
      //Remove curr vote if already done earlier
      sns_pwr_sleep_mgr_calc_eff_wakeup_rate(0,
          sns_pwr_sleep_mgr.curr_dae_wakeup_rate,
          sns_pwr_sleep_mgr.is_curr_dae_instance_island);
    }

    //reset curr vote.
    sns_pwr_sleep_mgr.curr_dae_wakeup_rate = 0;
    sns_pwr_sleep_mgr.is_curr_dae_instance_island = false;
  }

  return;
}

SNS_SECTION(".text.sns") void
sns_pwr_sleep_mgr_update_wakeup_rate(
  sns_pwr_sleep_instance_item *pwr_sleep_instance_item,
  bool is_inst_dae_switched,
  bool is_inst_island_switched,
  float new_wakeup_rate,
  float old_wakeup_rate)
{
  SNS_ASSERT(pwr_sleep_instance_item != NULL);

  if((true == pwr_sleep_instance_item->dae_sensor) ||
     (true == is_inst_dae_switched))
  {
    /* DAE update function will go over all the DAE sensors and select new MAX */
    sns_pwr_sleep_mgr_update_dae_wakeup_rate();
  }

  /* For non DAE sensors calculate the new effective Q6 wakeup rate */
  /* If the instance has switched from island to non-island or vice versa update
     the wakeup rate for both sleep driver nodes. If not only update the wakeup rate
     for the respective node */
  if(true == is_inst_island_switched)
  {
#ifdef SNS_PWR_SLEEP_DEBUG
    sns_pwr_sleep_debug_array[sns_pwr_sleep_debug_index].suid = pwr_sleep_instance_item->suid;
#endif
    sns_pwr_sleep_mgr_calc_eff_wakeup_rate(0, old_wakeup_rate,
        !pwr_sleep_instance_item->island_instance);

#ifdef SNS_PWR_SLEEP_DEBUG
    sns_pwr_sleep_debug_array[sns_pwr_sleep_debug_index].suid = pwr_sleep_instance_item->suid;
#endif
    sns_pwr_sleep_mgr_calc_eff_wakeup_rate(new_wakeup_rate, 0,
        pwr_sleep_instance_item->island_instance);
  }
  else if(old_wakeup_rate != new_wakeup_rate)
  {
#ifdef SNS_PWR_SLEEP_DEBUG
    sns_pwr_sleep_debug_array[sns_pwr_sleep_debug_index].suid = pwr_sleep_instance_item->suid;
#endif
    sns_pwr_sleep_mgr_calc_eff_wakeup_rate(new_wakeup_rate, old_wakeup_rate,
      pwr_sleep_instance_item->island_instance);
  }

  sns_pwr_sleep_mgr_validate_wakeup_rate();//debug

  return;
}

/**
 * Remove sleep configuration from the sleep manager instance list.
 */
SNS_SECTION(".text.sns") sns_rc
sns_pwr_sleep_mgr_remove_config(
  sns_sensor_uid const *sensor_uid,
  sns_sensor_instance *instance)
{
  sns_rc rc = SNS_RC_FAILED;
  float new_grp_max_wakeup_rate = 0;
  float old_grp_max_wakeup_rate = 0;
  bool is_inst_island_switched = false;
  bool island_instance = false;
  sns_isafe_list_iter iter, iter_item_marked_for_removal = {.list = NULL};

  SNS_PROFILE(SNS_PWR_SLEEP_MGR_REMOVE_INSTANCE_START, 0);

  sns_osa_lock_acquire(sns_pwr_sleep_mgr.sleep_mgr_lock);

  island_instance = sns_pwr_sleep_mgr_check_island_instance(instance);

  /* Find the sensor instance in the list */
  for(sns_isafe_list_iter_init(&iter, &sns_pwr_sleep_mgr.instance_list, true);
      NULL != sns_isafe_list_iter_curr(&iter);
      sns_isafe_list_iter_advance(&iter))
  {
    sns_pwr_sleep_instance_item *instance_item =
      (sns_pwr_sleep_instance_item *)sns_isafe_list_iter_get_curr_data(&iter);

    if(instance == instance_item->instance)
    {
      if(instance_item->island_instance != island_instance )
      {
        is_inst_island_switched = true;
      }

      if((instance_item->q6_wakeup_rate > old_grp_max_wakeup_rate)&&
         (false == instance_item->dae_sensor))
      {
        old_grp_max_wakeup_rate = instance_item->q6_wakeup_rate;
      }

      if(sns_sensor_uid_compare(&instance_item->suid, sensor_uid))
      {
        iter_item_marked_for_removal = iter;
      }
      else if((instance_item->q6_wakeup_rate > new_grp_max_wakeup_rate)&&
              (false == instance_item->dae_sensor))
      {
        /* Determine the MAX wakeup_rate for remaining items of the instance group */
        new_grp_max_wakeup_rate = instance_item->q6_wakeup_rate;
      }

      instance_item->island_instance = island_instance;
    }
  }

  if(NULL != iter_item_marked_for_removal.list)
  {
    SNS_PRINTF( MED, sns_fw_printf,
                "Removing sleep config for SUID %"PRIsuid, SNS_PRI_SUID(sensor_uid));
    sns_pwr_sleep_instance_item *instance_item =
        (sns_pwr_sleep_instance_item *) sns_isafe_list_iter_get_curr_data(
            &iter_item_marked_for_removal);
    sns_isafe_list_iter_remove(&iter_item_marked_for_removal);

    sns_pwr_sleep_mgr_update_wakeup_rate( instance_item, instance_item->dae_sensor,
         is_inst_island_switched, new_grp_max_wakeup_rate, old_grp_max_wakeup_rate);

    sns_free(instance_item);
    rc = SNS_RC_SUCCESS;
  }

  sns_osa_lock_release(sns_pwr_sleep_mgr.sleep_mgr_lock);
  SNS_PROFILE(SNS_PWR_SLEEP_MGR_REMOVE_INSTANCE_END, 0);

  return rc;
}

/**
 * Adds/updates a sensor power config to the power manager sensor instance list.
 * Calls the internal helper functions to aggregate the new Q6 wakeup rate and
 * communicate it to the sleep driver.
 *
 * @param[i] sensor_uid         sensor uid
 * @param[i] instance           sensor instance to be added/updated
 * @param[i] phy_sensor_config  physical sensor config information
 */
SNS_SECTION(".text.sns") void
sns_pwr_sleep_mgr_add_config(
    sns_sensor_uid const *sensor_uid,
    sns_sensor_instance *instance,
    sns_std_sensor_physical_config_event const *phy_sensor_config
)
{
  sns_isafe_list_iter  iter;
  sns_pwr_sleep_instance_item *pwr_sleep_instance_item = NULL;
  float new_grp_max_wakeup_rate = 0;
  float old_grp_max_wakeup_rate = 0;
  float current_wakeup_rate = 0;
  bool island_instance = false;
  bool dae_instance = false;
  bool is_inst_island_switched = false;
  bool is_inst_dae_switched = false;
  sns_rc rc = SNS_RC_SUCCESS;

  SNS_PROFILE(SNS_PWR_SLEEP_MGR_ADD_INSTANCE_START, 0);

  sns_osa_lock_acquire(sns_pwr_sleep_mgr.sleep_mgr_lock);

  island_instance = sns_pwr_sleep_mgr_check_island_instance(instance);

  rc = sns_pwr_sleep_mgr_calc_new_wakeup_rate(phy_sensor_config,
      &current_wakeup_rate);

  if (SNS_RC_SUCCESS != rc)
  {
    sns_osa_lock_release(sns_pwr_sleep_mgr.sleep_mgr_lock);
    SNS_PROFILE(SNS_PWR_SLEEP_MGR_ADD_INSTANCE_END, 0);
    return;
  }

  if((phy_sensor_config->has_DAE_watermark) && (phy_sensor_config->DAE_watermark != 0) )
  {
    dae_instance =  true;
  }

  /* Check if this sensor config is already in the list */
  for(sns_isafe_list_iter_init(&iter, &sns_pwr_sleep_mgr.instance_list, true);
      NULL != sns_isafe_list_iter_curr(&iter);
      sns_isafe_list_iter_advance(&iter))
  {
    sns_pwr_sleep_instance_item *instance_item =
      (sns_pwr_sleep_instance_item *)sns_isafe_list_iter_get_curr_data(&iter);

    if(instance == instance_item->instance)
    {
      if(instance_item->island_instance != island_instance )
      {
        is_inst_island_switched = true;
      }

      if( (instance_item->q6_wakeup_rate > old_grp_max_wakeup_rate) &&
          (false == instance_item->dae_sensor))
      {
        old_grp_max_wakeup_rate = instance_item->q6_wakeup_rate;
      }

      if(sns_sensor_uid_compare(&instance_item->suid, sensor_uid))
      {
          SNS_PRINTF( MED, sns_fw_printf,
                      "Sleep config is found for SUID %"PRIsuid, SNS_PRI_SUID(sensor_uid));
        pwr_sleep_instance_item = instance_item;
        instance_item->q6_wakeup_rate = current_wakeup_rate;
        if( instance_item->dae_sensor != dae_instance )
        {
          is_inst_dae_switched = true;
        }
        pwr_sleep_instance_item->dae_sensor =  dae_instance;
      }

      if( (instance_item->q6_wakeup_rate > new_grp_max_wakeup_rate)&&
          (false == instance_item->dae_sensor))
      {
        new_grp_max_wakeup_rate = instance_item->q6_wakeup_rate;
      }

      instance_item->island_instance = island_instance;
    }
  }

  /* Create a new instance item if its not already present on the list */
  if(NULL == pwr_sleep_instance_item)
  {
    SNS_PRINTF(MED, sns_fw_printf,
               "Adding sleep config for SUID %"PRIsuid, SNS_PRI_SUID(sensor_uid));
    pwr_sleep_instance_item =
        sns_malloc(SNS_HEAP_ISLAND, sizeof(*pwr_sleep_instance_item));
    if(NULL == pwr_sleep_instance_item)
    {
      SNS_ISLAND_EXIT();
      pwr_sleep_instance_item =
          sns_malloc(SNS_HEAP_MAIN, sizeof(*pwr_sleep_instance_item));
    }
    SNS_ASSERT(pwr_sleep_instance_item != NULL);

    pwr_sleep_instance_item->suid = *sensor_uid;
    pwr_sleep_instance_item->instance = instance;
    pwr_sleep_instance_item->q6_wakeup_rate = current_wakeup_rate;
    pwr_sleep_instance_item->island_instance = island_instance;
    pwr_sleep_instance_item->dae_sensor =  dae_instance;
    is_inst_dae_switched = dae_instance;

    sns_isafe_list_item_init(&pwr_sleep_instance_item->list_entry, pwr_sleep_instance_item);
    sns_isafe_list_iter_insert(&iter, &pwr_sleep_instance_item->list_entry, true);

    if((pwr_sleep_instance_item->q6_wakeup_rate > new_grp_max_wakeup_rate)&&
       (false == pwr_sleep_instance_item->dae_sensor))
    {
      new_grp_max_wakeup_rate = pwr_sleep_instance_item->q6_wakeup_rate;
    }
  }

  sns_pwr_sleep_mgr_update_wakeup_rate( pwr_sleep_instance_item, is_inst_dae_switched,
     is_inst_island_switched, new_grp_max_wakeup_rate, old_grp_max_wakeup_rate);

  sns_osa_lock_release(sns_pwr_sleep_mgr.sleep_mgr_lock);
  SNS_PROFILE(SNS_PWR_SLEEP_MGR_ADD_INSTANCE_END, 0);
}

/**
 * Check for physical sensor config events.
 */
SNS_SECTION(".text.sns") sns_rc
sns_pwr_sleep_mgr_check_config_event(sns_sensor_uid const *sensor_uid,
    sns_sensor_instance *instance, sns_sensor_event const *event,
    bool is_physical_sensor)
{
  sns_std_sensor_physical_config_event phy_sensor_config =
      sns_std_sensor_physical_config_event_init_default;
  pb_istream_t stream;
  sns_rc rc = SNS_RC_SUCCESS;

  /* Only decode physical sensor config events */
  if((SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_PHYSICAL_CONFIG_EVENT
      != event->message_id) || !is_physical_sensor)
  {
    return rc;
  }

  stream = pb_istream_from_buffer((pb_byte_t*)event->event, event->event_len);

  if(!pb_decode(&stream,
      sns_std_sensor_physical_config_event_fields, &phy_sensor_config))
  {
    /* Decode failed */
    rc = SNS_RC_FAILED;
  }
  else
  {
    sns_pwr_sleep_mgr_add_config(sensor_uid, instance, &phy_sensor_config);
  }
  return rc;
}

/**
 * Initialize the sensors power sleep manager.
 */
sns_rc
sns_pwr_sleep_mgr_init(void)
{
  sns_osa_lock_attr   lock_attr;
  sns_rc rc = SNS_RC_SUCCESS;

  /* Initialize handles to the sleep driver NPA nodes */
  sns_pwr_sleep_mgr.max_duration_handle = npa_create_sync_client(
                            "/sleep/max_duration/usec",
                            "SNS_PM_MaxDuration",
                            NPA_CLIENT_REQUIRED);
  SNS_ASSERT(NULL != sns_pwr_sleep_mgr.max_duration_handle);

  sns_pwr_sleep_mgr.normal_latency_handle = npa_create_sync_client(
                          "/core/cpu/latency/usec",
                          "SNS_PM_NormalLatency",
                          NPA_CLIENT_REQUIRED);
  SNS_ASSERT(NULL != sns_pwr_sleep_mgr.normal_latency_handle);

  sns_pwr_sleep_mgr.tcm_npa_handle = npa_create_sync_client(
                        "/core/cpu/tcm" ,
                        "SNS_TCM",
                        NPA_CLIENT_REQUIRED);

  sns_pwr_sleep_mgr.unpa_client = sns_pwr_sleep_mgr_create_unpa_handle();

  /* Initialize list of sensor instances aggregated towards Q6 wakeup rate */
  sns_isafe_list_init(&sns_pwr_sleep_mgr.instance_list);

  /* Initialize list of clients aggregated towards the latency vote */
  sns_isafe_list_init(&sns_pwr_sleep_mgr.latency_list);

  /* Initialize current DAE wakeup rate to 0 */
  sns_pwr_sleep_mgr.curr_dae_wakeup_rate = 0;
  sns_pwr_sleep_mgr.is_curr_dae_instance_island = false;

  sns_pwr_sleep_mgr.island_mode_wakeup_rate = 0;
  sns_pwr_sleep_mgr.normal_mode_wakeup_rate = 0;

  /* Initialize current latency setting to 0 */
  sns_pwr_sleep_mgr.latency_us = 0;

  sns_pwr_sleep_mgr.island_enabled = true;

  /* Initialize power sleep manager lock */
  sns_osa_lock_attr_init(&lock_attr);
  sns_osa_lock_attr_set_memory_partition(&lock_attr, SNS_OSA_MEM_TYPE_ISLAND);
  rc = sns_osa_lock_create(&lock_attr, &sns_pwr_sleep_mgr.sleep_mgr_lock);
  SNS_ASSERT(SNS_RC_SUCCESS == rc);

  return rc;
}

SNS_SECTION(".text.sns") void
sns_pwr_sleep_mgr_set_latency(void *client, uint32_t latency_us)
{
  sns_isafe_list_iter  iter;
  sns_pwr_sleep_latency_item *pwr_sleep_latency_item = NULL;

  if(latency_us == 0)
  {
    SNS_PRINTF(ERROR, sns_fw_printf, "Invalid latency request");
    return;
  }

  sns_osa_lock_acquire(sns_pwr_sleep_mgr.sleep_mgr_lock);

  /* Check if this client is already in the list */
  for(sns_isafe_list_iter_init(&iter, &sns_pwr_sleep_mgr.latency_list, true);
      NULL != sns_isafe_list_iter_curr(&iter);
      sns_isafe_list_iter_advance(&iter))
  {
    sns_pwr_sleep_latency_item *latency_item =
      (sns_pwr_sleep_latency_item *)sns_isafe_list_iter_get_curr_data(&iter);

    if(client == latency_item->client)
    {
      pwr_sleep_latency_item = latency_item;
      pwr_sleep_latency_item->latency_us = latency_us;
      break;
    }
  }

  /* Create a new instance item if its not already present on the list */
  if(NULL == pwr_sleep_latency_item)
  {
    pwr_sleep_latency_item =
        sns_malloc(SNS_HEAP_ISLAND, sizeof(*pwr_sleep_latency_item));
    if(NULL == pwr_sleep_latency_item)
    {
      SNS_ISLAND_EXIT();
      pwr_sleep_latency_item =
          sns_malloc(SNS_HEAP_MAIN, sizeof(*pwr_sleep_latency_item));
    }
    SNS_ASSERT(pwr_sleep_latency_item != NULL);

    pwr_sleep_latency_item->client = client;
    pwr_sleep_latency_item->latency_us = latency_us;

    sns_isafe_list_item_init(&pwr_sleep_latency_item->list_entry,
        pwr_sleep_latency_item);
    sns_isafe_list_iter_insert(&iter,
        &pwr_sleep_latency_item->list_entry, true);
  }

  if((sns_pwr_sleep_mgr.latency_us == 0) ||
     (latency_us < sns_pwr_sleep_mgr.latency_us))
  {
    SNS_ISLAND_EXIT();
    npa_issue_required_request(
        sns_pwr_sleep_mgr.normal_latency_handle, latency_us);
    sns_pwr_sleep_mgr.latency_us = latency_us;
    sns_osa_lock_release(sns_pwr_sleep_mgr.sleep_mgr_lock);
    SNS_PRINTF(HIGH, sns_fw_printf, "Set latency to %u us", latency_us);
  }
  else
  {
    sns_osa_lock_release(sns_pwr_sleep_mgr.sleep_mgr_lock);
  }
}

SNS_SECTION(".text.sns") void
sns_pwr_sleep_mgr_release_latency(void *client)
{
  sns_isafe_list_iter  iter;
  uint32_t removed_latency_us = 0;
  uint32_t lowest_latency_us = 0;

  sns_osa_lock_acquire(sns_pwr_sleep_mgr.sleep_mgr_lock);

  /* Check if this client is already in the list */
  for(sns_isafe_list_iter_init(&iter, &sns_pwr_sleep_mgr.latency_list, true);
      NULL != sns_isafe_list_iter_curr(&iter);
      sns_isafe_list_iter_advance(&iter))
  {
    sns_pwr_sleep_latency_item *latency_item =
      (sns_pwr_sleep_latency_item *)sns_isafe_list_iter_get_curr_data(&iter);

    if(client == latency_item->client)
    {
      removed_latency_us = latency_item->latency_us;
      sns_isafe_list_iter_remove(&iter);
      sns_free(latency_item);
      break;
    }
  }

  if((sns_isafe_list_iter_len(&iter) == 0) &&
     (sns_pwr_sleep_mgr.latency_us != 0))
  {
    /* If no clients are present remove the latency requirement */
    SNS_ISLAND_EXIT();
    npa_complete_request(sns_pwr_sleep_mgr.normal_latency_handle);
    sns_pwr_sleep_mgr.latency_us = 0;
    sns_osa_lock_release(sns_pwr_sleep_mgr.sleep_mgr_lock);
    SNS_PRINTF(HIGH, sns_fw_printf, "Release latency request");
  }
  else if((removed_latency_us == sns_pwr_sleep_mgr.latency_us) &&
          (removed_latency_us != 0))
  {
    /* Get and program the new lowest latency value */
    for(sns_isafe_list_iter_init(&iter, &sns_pwr_sleep_mgr.latency_list, true);
        NULL != sns_isafe_list_iter_curr(&iter);
        sns_isafe_list_iter_advance(&iter))
    {
      sns_pwr_sleep_latency_item *latency_item =
        (sns_pwr_sleep_latency_item *)sns_isafe_list_iter_get_curr_data(&iter);

      if(lowest_latency_us == 0)
      {
        lowest_latency_us = latency_item->latency_us;
      }
      else if(latency_item->latency_us < lowest_latency_us)
      {
        lowest_latency_us = latency_item->latency_us;
      }
    }
    SNS_ISLAND_EXIT();
    npa_issue_required_request(
        sns_pwr_sleep_mgr.normal_latency_handle, lowest_latency_us);
    sns_pwr_sleep_mgr.latency_us = lowest_latency_us;
    sns_osa_lock_release(sns_pwr_sleep_mgr.sleep_mgr_lock);
    SNS_PRINTF(HIGH, sns_fw_printf, "Set latency to %u us", lowest_latency_us);
  }
  else
  {
    sns_osa_lock_release(sns_pwr_sleep_mgr.sleep_mgr_lock);
  }
}

/**
 * Set/reset island mode enabled flag in power sleep manager
 */
SNS_SECTION(".text.sns") void
sns_pwr_sleep_mgr_set_island_mode_enabled(bool island_enabled)
{
  sns_osa_lock_acquire(sns_pwr_sleep_mgr.sleep_mgr_lock);

  if(sns_pwr_sleep_mgr.island_enabled != island_enabled)
  {
    sns_pwr_sleep_mgr.island_enabled = island_enabled;

    if(island_enabled)
    {
      sns_pwr_sleep_mgr_set_island_mode_sleep_mode(
          sns_pwr_sleep_mgr.island_mode_wakeup_rate);
      sns_pwr_sleep_mgr_set_normal_mode_wakeup_interval(
          sns_pwr_sleep_mgr.normal_mode_wakeup_rate);
    }
    else
    {
      sns_pwr_sleep_mgr_set_normal_mode_wakeup_interval(
          sns_pwr_sleep_mgr.normal_mode_wakeup_rate +
          sns_pwr_sleep_mgr.island_mode_wakeup_rate);
    }
  }

  sns_osa_lock_release(sns_pwr_sleep_mgr.sleep_mgr_lock);
}
