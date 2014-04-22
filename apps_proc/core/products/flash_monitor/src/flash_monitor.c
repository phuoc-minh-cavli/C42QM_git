/***********************************************************************
 * FILE: flash_monitor.c
 *
 * SERVICES: Flash Monitor service is for the alarm the false writes
 *
 * DESCRIPTION: Flash Monitor thread will be used to track the false writes 
 *		happened to Nand storage.
 *
 * PUBLIC CLASSES AND STATIC FUNCTIONS:
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:
 *
 * Copyright (c) 2022 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *
 ==================================================================*/

/*===========================================================================

  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  when         who   what, where, why
  ----------   ---   ---------------------------------------------------------
  2022-06-03   sna    Created the file.

  ===========================================================================*/


/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stringl/stringl.h>
#include "fs_flash_usage_stats.h"
#include "fs_counters.h"
#include "rcinit.h"
#include "qurt.h" 
#include "timer.h"
#include "qurt_timetick.h"
#include "DDITimetick.h"
#include "qurt_error.h"
#include "err.h"
#include "fs_public.h"
#include "time_svc.h"
#include "fs_err.h"
/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ---------------------------------------------------------------------------*/
#define MIN_PSM_DUR_IN_MINS 			5
#define MON_TIMER_INTERVAL_IN_MINS 			5
#define FLASH_MONITOR_TIMER_SIGNAL			0x00000002   /* Move it to headerfile  -- Name as appropriate */
#define FLASH_MONITOR_DEFAULT_POLL_PERIOD	(MON_TIMER_INTERVAL_IN_MINS*60000)      /* 5 mins = 5*60*1000ms */ 

#define NUM_ENTRIES_IN_PROFILING_ARRAY		12
#define USER_CONFIG_THRESHOLD_VAL 			100
#define FLASH_MON_WAIT_SIG 					0x00010000
#define FLASH_MON_STACK_SIZE  				(2048)

#define MAX_STRNG_SIZE 						512
#define EFS_FLSH_MON_WRT_CNT_PATH           "/fsmon_max_cnt.txt"
#define EFS_FLSH_MON_THRSHLD_CNT_PATH           "/fsmon_threshold_cnt.txt"

qurt_signal_t      flash_mon_sig;
qurt_thread_t      flash_mon_tcb;
qurt_thread_attr_t flash_mon_attr;


/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

struct mon_fs_flash_log_write_counters_info {
	unsigned long long qtime;
	int flash_counter;
};

struct flash_count_timetick_info_type
{
	DalDeviceHandle *handle;
	uint8 init_successful;
};
static struct flash_count_timetick_info_type fs_os_timetick_info; 
struct mon_fs_flash_log_write_counters_info threshold_write_counters[NUM_ENTRIES_IN_PROFILING_ARRAY];
struct fs_flash_usage_stats_info mon_fs_flash_stats_info;  


unsigned int init_wrt_cnt;
unsigned int currnt_wrt_cnt;
unsigned int prv_wrt_cnt;
unsigned int wrt_cnt_per_min_max;
unsigned int threshold_wrt_cntr;

unsigned int wrt_cnt_per_min;
uint32 threshold_array_index ;
int fd,threshold_fd;
	

void flash_monitor_shutdown_event_cb(void);



typedef uint64 fs_os_timetick;
static qurt_signal_t flash_monitor_signal_obj;

/*======================================================================
  LOCAL FUNCTION Definition
  =======================================================================*/
/*
 *
 * @brief : flash_monitor_thread
 *
 *       flash_monitor_thread is a continous free running thread.
 *       This thread  will start running after 5min of booting and will help in getting the counters
 *       from APPS EFS region side and stores them in EFS location if the count value exceeds limits
 *       which also help in alarm the user if false writes are happened.
 *
 */
 

void flash_monitor_thread(void *unused)
{
	uint32 timer_delta=0,sigs=0,rc=0;
	unsigned long long crnt_qtime=0,prv_qtime=0;
	unsigned int wrt_cnt_per_5_min=0;
	char  str[MAX_STRNG_SIZE];
	time_julian_type time;	

	timer_type flash_monitor_timer = {0};
	uint32 flash_monitor_poll_period=0;
	timer_group_type flash_monitor_timer_group = {0};
	DALResult result=0;
	fs_os_timetick timetick = 0;

	flash_monitor_poll_period = FLASH_MONITOR_DEFAULT_POLL_PERIOD;

	memset (&fs_os_timetick_info, 0x0, sizeof (fs_os_timetick_info));

	result = DalTimetick_Attach ("SystemTimer", &fs_os_timetick_info.handle);
	if ((result != DAL_SUCCESS) || (fs_os_timetick_info.handle == NULL))
	{
		FS_MSG_ERROR_1("%d DalTimetick_Attach failed", result);
		fs_os_timetick_info.handle = NULL;
		fs_os_timetick_info.init_successful = 0;
		return;
	}

	result = DalTimetick_InitTimetick64 (fs_os_timetick_info.handle);
	if ((result != DAL_SUCCESS))
	{
		FS_MSG_ERROR_1("%d DalTimetick_InitTimetick64 failed", result);
		fs_os_timetick_info.handle = NULL;
		fs_os_timetick_info.init_successful = 0;
		return ;
	}	

	result = DalTimetick_GetTimetick64 (fs_os_timetick_info.handle, &timetick);
	if (result != DAL_SUCCESS)
	{
		FS_MSG_ERROR_1("%d DalTimetick_GetTimetick64 fail", result);
		return ;
	}


	//block for start signal
	rcinit_handshake_startup();

	qurt_signal_init(&flash_monitor_signal_obj);


	memset(&flash_monitor_timer, 0, sizeof(timer_type));

	/* Create a deferable timer */
	timer_group_set_deferrable(&flash_monitor_timer_group, TRUE);

	timer_def_osal(&flash_monitor_timer,
			&flash_monitor_timer_group,
			TIMER_NATIVE_OS_SIGNAL_TYPE,
			&flash_monitor_signal_obj,
			FLASH_MONITOR_TIMER_SIGNAL);

	/*Start the periodic timer for 5min*/
	timer_set(&flash_monitor_timer, flash_monitor_poll_period, 0, T_MSEC);

	/*Read the flash counters after the device comes up*/
	//task forever loop
	while(1)
	{
		sigs = qurt_signal_wait(&flash_monitor_signal_obj, FLASH_MONITOR_TIMER_SIGNAL, QURT_SIGNAL_ATTR_WAIT_ANY);

		if (sigs & FLASH_MONITOR_TIMER_SIGNAL)
		{
			qurt_signal_clear( &flash_monitor_signal_obj, FLASH_MONITOR_TIMER_SIGNAL );
			timer_set(&flash_monitor_timer, flash_monitor_poll_period, 0, T_MSEC); 

			/*Read the Flash Counter for every flash_monitor_poll_period*/

			rc=fs_get_flash_usage_stats_info(&mon_fs_flash_stats_info);
			if(rc != 0)
			{
				FS_MSG_ERROR_1("fs_get_flash_usage_stats_info failed with ret %d",rc);
				return;
			}
			/* update the prv_qtime variable with crnt_qtime to read the next iteration of qtime */ 
			prv_qtime  = crnt_qtime;
			
			/* Get the crnt_qtime when read the flash counter value */
			result = DalTimetick_GetTimetick64 (fs_os_timetick_info.handle, &crnt_qtime);
			if (result != DAL_SUCCESS)
			{
				FS_MSG_ERROR_1("%d DalTimetick_GetTimetick64 fail", result);
				return ;
			}
			timer_delta = (crnt_qtime/19200000)-(prv_qtime/19200000);


			/* update the counter values read from the flash API's */
			currnt_wrt_cnt = mon_fs_flash_stats_info.write_region_stats[FS_FOP_LOG];
			wrt_cnt_per_5_min = currnt_wrt_cnt - prv_wrt_cnt;
			wrt_cnt_per_min = (wrt_cnt_per_5_min/timer_delta);

#ifdef FS_DEBUG
  			
			FS_MSG_ERROR_1("current_qtime %x",crnt_qtime);
			FS_MSG_ERROR_3("Current_wrt_cnt %d prv_wrt_cnt %d wrt_cnt_per_min %d",currnt_wrt_cnt,prv_wrt_cnt,wrt_cnt_per_min);
			FS_MSG_ERROR_3("wrt_cnt_per_5_min %d timer_delta %x prv_qtime %x",wrt_cnt_per_5_min,timer_delta,prv_qtime);
#endif
			if(wrt_cnt_per_min > wrt_cnt_per_min_max)
			{
				wrt_cnt_per_min_max = wrt_cnt_per_min;

				time_get_julian(&time);
				FS_MSG_ERROR_3("RTC Time %d-%d-%d",time.year,time.month,time.day);
				FS_MSG_ERROR_3("%d-%d Write_count=%d",time.hour,time.minute,wrt_cnt_per_min);
				FS_MSG_ERROR_3("Current_wrt_cnt %d prv_wrt_cnt %d wrt_cnt_per_min_max %d",currnt_wrt_cnt,prv_wrt_cnt,wrt_cnt_per_min_max);

				memset(&str, 0x00, sizeof(str));		
				efs_lseek (fd, 0, SEEK_SET);
				snprintf(str, MAX_STRNG_SIZE,"%d, %d-%d-%d:%d-%d",wrt_cnt_per_min_max,time.year,time.month,time.day,time.hour,time.minute);

				if (-1 == efs_write(fd, str, strlen(str)))
				{
					FS_MSG_ERROR_0("APPS EFS file write failed\n");
					efs_close(fd);
					return ;
				}
				 
			}
			if(wrt_cnt_per_min > USER_CONFIG_THRESHOLD_VAL)
			{
				DalTimetick_GetTimetick64(fs_os_timetick_info.handle,&crnt_qtime);
				FS_MSG_ERROR_2("Threshold Reached %u wrt_cnt_per_min %d"
						,crnt_qtime
						,wrt_cnt_per_min);
				FS_MSG_ERROR_1("Threshold from boot -- %d -- ",threshold_wrt_cntr);

                                efs_lseek (threshold_fd, 0, SEEK_SET);
				threshold_wrt_cntr = threshold_wrt_cntr +1;

				FS_MSG_ERROR_1("Threshold count updated -- %d -- ",threshold_wrt_cntr);

                                memset(&str,0x00,sizeof(str));
				snprintf(str,MAX_STRNG_SIZE,"%d",threshold_wrt_cntr);

				if (-1 == efs_write(threshold_fd, str, strlen(str)))
				{
					FS_MSG_ERROR_0("EFS file write failed\n");
					efs_close(fd);
					return ;
				}
#ifdef DEBUG_FLASH_MON_PROFILE_DATA
				/* store the counts which are more then user threshould with their time stamps for the debug purpose */
				threshold_write_counters[threshold_array_index].flash_counter = wrt_cnt_per_min;
				threshold_write_counters[threshold_array_index].qtime = crnt_qtime;
				threshold_array_index++;
				if(threshold_array_index == NUM_ENTRIES_IN_PROFILING_ARRAY-1)
				{
					/* re initialize the array index to 1 */
					threshold_array_index=0;
				}
#endif
#ifdef FLASH_MON_CRASH_ON_THRESHOLD
				ERR_FATAL("Flash monitor shutdown callback \n",0,0,0);
#endif

			}
			prv_wrt_cnt = currnt_wrt_cnt;
		}	
	}

}

/*======================================================================
  LOCAL FUNCTION Definition
  =======================================================================*/
/*
 *
 * @brief : init_flash_monitor_start
 *
 *       This init_flash_monitor_start function is called from the RCINIT script
 *       function will read the flash counter after boot up and initlizes
 *	 flash monitor thread to run.
 *
 *
 * @retval        NO_EROR       Success
 * @retval        Error code     ERROR
 *
 */


int init_flash_monitor_start()
{
	int rc=0,i,res=0;
	struct fs_stat buf;
	char str[MAX_STRNG_SIZE];
	qurt_signal_init(&flash_mon_sig);
	qurt_thread_attr_init(&flash_mon_attr);
	qurt_thread_attr_set_name(&flash_mon_attr, "FLASH_MONITOR_THRD");
	qurt_thread_attr_set_priority(&flash_mon_attr, 10);
	qurt_thread_attr_set_stack_size(&flash_mon_attr, FLASH_MON_STACK_SIZE);
	qurt_thread_create(&flash_mon_tcb, &flash_mon_attr,
			flash_monitor_thread, NULL);

	rc=fs_get_flash_usage_stats_info(&mon_fs_flash_stats_info);
	if(rc != 0)
	{
		FS_MSG_ERROR_1("fs_get_flash_usage_stats_info failed with ret %d",rc);
		return -1;
	}
	init_wrt_cnt = mon_fs_flash_stats_info.write_region_stats[FS_FOP_LOG];

	fd=efs_open(EFS_FLSH_MON_WRT_CNT_PATH,O_CREAT | O_RDWR, 0666);
	if (-1 == fd)
	{
		FS_MSG_ERROR_1("Failed to open /fsmon_max_cnt.txt return %d\n",fd);
		return -1;
	}
	threshold_fd=efs_open(EFS_FLSH_MON_THRSHLD_CNT_PATH,O_CREAT | O_RDWR, 0744);
	if (-1 == threshold_fd)
	{
		FS_MSG_ERROR_1("Failed to open /fsmon_threshold_cnt.txt return %d\n",threshold_fd);
		return -1;
	}
	/* Check to ensure that the file is created and size should be zero in first boot */
	if (efs_stat (EFS_FLSH_MON_WRT_CNT_PATH, &buf) != 0)
	{
		return -1;
	}
	if ( buf.st_size != 0)
	{
		memset(&str,0x00,sizeof(str));
		efs_lseek (fd, 0, SEEK_SET);
		rc=efs_read(fd,str,MAX_STRNG_SIZE);
		res=0;
		for (i = 0; str[i] != ','; ++i)
		{
			res = res*10 + str[i] - '0';
		}
		wrt_cnt_per_min_max = res;
	}
	else
	{
		/* if file is empty update with initial data */
		memset(&str, 0x00, sizeof(str));
		snprintf(str,MAX_STRNG_SIZE,"0,0");
		efs_write(fd,str,strlen(str));
		wrt_cnt_per_min_max = 0;
	}
	/* Check to ensure that the file is created and size should be zero in first boot */
	if (efs_stat (EFS_FLSH_MON_THRSHLD_CNT_PATH, &buf) != 0)
	{
	     return -1;
	}
	if ( buf.st_size != 0)
	{
		memset(&str,0x00,sizeof(str));
		efs_lseek (threshold_fd, 0, SEEK_SET);
		rc=efs_read(threshold_fd,str,MAX_STRNG_SIZE);
		res=0;
		for (i = 0; str[i] != '\0'; ++i)
		{
			res = res*10 + str[i] - '0';
		}
		threshold_wrt_cntr = res;
	}
	else
	{
		/* if file is empty update with initial data */
		memset(&str, 0x00, sizeof(str));
		snprintf(str,MAX_STRNG_SIZE,"0");
		efs_write(threshold_fd,str,strlen(str));
		threshold_wrt_cntr = 0;
	}
		/* Register for the shutdown event callback notification */
        rcinit_register_termfn_group(RCINIT_GROUP_7, flash_monitor_shutdown_event_cb);
	prv_wrt_cnt = init_wrt_cnt;

	return 0;
}
/*
 *
 * @brief : flash_monitor_shutdown_event_cb 
 *
 *       This flash_monitor_shutdown_event_cb function will be called as
 *	 a part of PSM sequence.
 *       function will read the flash counter in PSM callbacks and calculates
 *       the per_min flash write counters and perform critical logging and stored in EFS.
 *
 *
 * @retval        NO_EROR       Success
 * @retval        Error code     ERROR
 *
 */

void flash_monitor_shutdown_event_cb(void)
{
	int rc=0;
	unsigned long long psm_qtime;
	char  str[MAX_STRNG_SIZE];
	unsigned int slp_curr_wr_cnt=0,slp_wr_cnt_delta=0;
	time_julian_type time;

	rc=fs_get_flash_usage_stats_info(&mon_fs_flash_stats_info);
	if(rc != 0)
	{
		FS_MSG_ERROR_1("fs_get_flash_usage_stats_info failed with ret %d",rc);
	}

	slp_curr_wr_cnt = mon_fs_flash_stats_info.write_region_stats[FS_FOP_LOG];
	/* minimum PSM duration will be 5min, even if the call back called within 5 min it is ok to divide by MIN_PSM_DUR_IN_MINS*/
	slp_wr_cnt_delta = ((slp_curr_wr_cnt - prv_wrt_cnt)/MIN_PSM_DUR_IN_MINS);
	wrt_cnt_per_min = slp_wr_cnt_delta;
	DalTimetick_GetTimetick64(fs_os_timetick_info.handle,&psm_qtime);
	if(wrt_cnt_per_min > wrt_cnt_per_min_max)
	{
		wrt_cnt_per_min_max = wrt_cnt_per_min;

		time_get_julian(&time);
		FS_MSG_ERROR_3("RTC Time %d-%d-%d",time.year,time.month,time.day);
		FS_MSG_ERROR_3("%d-%d Write_count=%d",time.hour,time.minute,wrt_cnt_per_min);
		FS_MSG_ERROR_3("Current_wrt_cnt %d prv_wrt_cnt %d wrt_cnt_per_min_max %d",currnt_wrt_cnt,prv_wrt_cnt,wrt_cnt_per_min_max);

		memset(&str, 0x00, sizeof(str));
		efs_lseek (fd, 0, SEEK_SET);
		snprintf(str, MAX_STRNG_SIZE,"%d, %d-%d-%d:%d-%d",wrt_cnt_per_min_max,time.year,time.month,time.day,time.hour,time.minute);

		if (-1 == efs_write(fd, str, strlen(str)))
		{
			FS_MSG_ERROR_0("APPS EFS file write failed\n");
			efs_close(fd);
			return ;
		}

	}
	if(wrt_cnt_per_min > USER_CONFIG_THRESHOLD_VAL)
	{
		DalTimetick_GetTimetick64(fs_os_timetick_info.handle,&psm_qtime);
		FS_MSG_ERROR_2("Threshold Reached %u wrt_cnt_per_min %d"
				,psm_qtime
				,wrt_cnt_per_min);
		FS_MSG_ERROR_1("Threshold counter from file -- %d -- ",threshold_wrt_cntr);

		efs_lseek (threshold_fd, 0, SEEK_SET);
		threshold_wrt_cntr = threshold_wrt_cntr +1;

		FS_MSG_ERROR_1("Threshold count updated to file -- %d -- ",threshold_wrt_cntr);

		memset(&str,0x00,sizeof(str));
		snprintf(str,MAX_STRNG_SIZE,"%d",threshold_wrt_cntr);

		if (-1 == efs_write(threshold_fd, str, strlen(str)))
		{
			FS_MSG_ERROR_0("EFS file write failed\n");
			efs_close(fd);
			return ;
		}

#ifdef DEBUG_FLASH_MON_PROFILE
		/* store the counts which are more then user threshould with their time stamps for the debug purpose */
		threshold_write_counters[threshold_array_index].flash_counter = wrt_cnt_per_min;
		threshold_write_counters[threshold_array_index].qtime = psm_qtime;
		threshold_array_index++;
		if(threshold_array_index == NUM_ENTRIES_IN_PROFILING_ARRAY-1)
		{
			threshold_array_index = 0;
		}
#endif	

#ifdef FLASH_MON_CRASH_ON_THRESHOLD
		ERR_FATAL("Flash monitor shutdown callback \n",0,0,0);
#endif

	}
	efs_close(fd);
	FS_MSG_ERROR_3("flash_monitor sleep enter curr_wr_cnt %d prv_wrt_cnt %d wr_cnt_delta %d ",slp_curr_wr_cnt,prv_wrt_cnt,slp_wr_cnt_delta);
}
