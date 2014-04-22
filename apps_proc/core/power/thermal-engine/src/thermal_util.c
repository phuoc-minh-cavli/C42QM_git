/*===========================================================================

  Copyright (c) 2013-18 QUALCOMM Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

===========================================================================*/

#include <stdio.h>
#include <stdlib.h>
//#include <unistd.h>
#include <stringl/stringl.h>

#include "thermal.h"
#include "therm_log.h"
#include "DDIChipInfo.h"
#include "log.h"
#include "therm_diag.h"

struct therm_msm_soc_type {
	enum therm_msm_id msm_id;
	DalChipInfoFamilyType soc_id;
};

static struct therm_msm_soc_type msm_soc_table[] = {
	{THERM_MDM_9x25, DALCHIPINFO_FAMILY_MDM9x25},
	{THERM_MDM_9x35, DALCHIPINFO_FAMILY_MDM9x35},
	{THERM_MDM_9x45, DALCHIPINFO_FAMILY_MDM9x45},
	{THERM_MDM_9x55, DALCHIPINFO_FAMILY_MDM9x55},
	{THERM_MDM_9x06, DALCHIPINFO_FAMILY_MDM9x07},
	{THERM_MDM_9205, DALCHIPINFO_FAMILY_MDM9205},
};

enum therm_msm_id therm_get_msm_id(void)
{
	enum therm_msm_id ret_val = THERM_MSM_UNKNOWN;
	int idx;
	DalChipInfoFamilyType soc_id = DalChipInfo_ChipFamily();

	for (idx = 0 ; idx < ARRAY_SIZE(msm_soc_table); idx++) {
		if (soc_id == msm_soc_table[idx].soc_id) {
			ret_val = msm_soc_table[idx].msm_id;
			break;
		}
	}
	return ret_val;
}

static int compare_ascending_int(const void *x, const void *y)
{
	int i = *((int*)x);
	int j = *((int*)y);

	if (i < j)
		return -1;
	else if (i > j)
		return 1;
	return 0;
}

static int compare_descending_int(const void *x, const void *y)
{
	int i = *((int*)x);
	int j = *((int*)y);

	if (j < i)
		return -1;
	else if (j > i)
		return 1;
	return 0;
}

void sort_int_arr(int *arr, uint32_t arr_sz, uint8_t ascending)
{
  if (arr == NULL || arr_sz == 0)
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 1, "%s: Bad arg.", __func__);
    return;
  }

  if (ascending)
    qsort(arr, arr_sz, sizeof(int), compare_ascending_int);
  else
    qsort(arr, arr_sz, sizeof(int), compare_descending_int);
}

void therm_sensor_diag_log_reading(diag_therm_packet *therm_packet,
                                   const char *sensor_name)
{
  PACK(void *)       log_ptr = NULL;

  therm_log_printf(THERM_LOG_LEVEL_DEBUG, 5,
                   "%s read ss_id %d %s, sensor_id %d, TEMP %d C", __func__,
                   therm_packet->samples[0].subsystem_id,
                   sensor_name, therm_packet->samples[0].sensor_id,
                   therm_packet->samples[0].temp);

  therm_log_printf(THERM_LOG_LEVEL_STATS, 2, "Temperature ( %s:%d )",
                   sensor_name, therm_packet->samples[0].temp);

  /* Only send out a log packet if we have samples to report on. */
  log_ptr = log_alloc(LOG_TEMPERATURE_MONITOR_C, sizeof(diag_therm_packet) +
                      sizeof(log_hdr_type));

  if (log_ptr)
  {
    memscpy( (uint8 *)log_ptr + sizeof(log_hdr_type),   /* dest */
             sizeof(diag_therm_packet),                 /* dst_size */
             therm_packet,                              /* src */
             sizeof(diag_therm_packet) );               /* src_size */

    log_commit(log_ptr);
  }
}

void therm_pthread_attr_init(qurt_thread_attr_t  *attr, const char *thread_name)
{

  qurt_thread_t thread_id;
  uint16 priority;
  
  thread_id = qurt_thread_get_id();
  priority =  qurt_thread_get_priority(thread_id);

  qurt_thread_attr_init(attr);
  qurt_thread_attr_set_name(attr, (char*)thread_name);
  qurt_thread_attr_set_priority(attr, priority);
}

#if 0
void therm_thread_attr_setstack(qurt_thread_attr_t  *attr, size_t stack_size)
{

  void * stack_addr = NULL;
  
  stack_addr = (void *)malloc(stack_size);
  CORE_VERIFY_PTR( stack_addr );
  
  if (0 != pthread_attr_setstack(attr, stack_addr, stack_size))
  {
	therm_log_printf(THERM_LOG_LEVEL_ERROR, 0,
                   "pthread_attr_setstack failed");
	CORE_VERIFY(NULL);
  }

}
#endif

