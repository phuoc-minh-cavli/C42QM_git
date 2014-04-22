/***********************************************************************
 * FILE: qapi_utils.c
 *
 * SERVICES: QAPI utility service for user space DAM application
 *
 * DESCRIPTION: QAPI functionality for utility related services from DAM application.
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
  2022-03-14   sna    Created the file.

  ===========================================================================*/

/* This should be on top. Do not move */
#include "fs_flash_usage_stats.h"
#include "qapi_utils.h"
#include "stringl/stringl.h"
#include "fs_errno.h"
#include "qurt_txm_qapi_fwk.h"
#include "qurt_error.h"
#include "qurt_memory.h"
#include <string.h>
#include "qmi_utils_client.h"
#include "qapi_diag.h"


#define ERROR -1
#define FS_FLASH_ERR -2
#define FS_BLOCK_ERR  -3

#define FS_FOP_LOG 2

typedef enum {
	QAPI_UTILS_SUBSYS_APPS,
	QAPI_UTILS_SUBSYS_MPSS
} qapi_efs_ss_type_e;


struct fs_flash_usage_stats_info mon_fs_flash_stats_info_write_counter;  
struct fs_block_info_t mon_fs_block_info;
int qmi_client_init_count;

int Read_Flash_Write_Counter_MPSS_Efs(uint32 *writes,uint32 *block);
int Read_Flash_Write_Counter_APPS_Efs(uint32 *writes,uint32 *block);


/*======================================================================
  LOCAL FUNCTION Definition
  =======================================================================*/
/*
 *
 * @brief :Read_Flash_Write_Counter_APPS_Efs
 *
 *       This function will help in getting the counters
 *       from APPS EFS region side.
 *
 * @param[out]    writes    This will fetch number of writes
 *
 *
 * @param[out]    block     Will used to block count from modem
 * 
 * @retval        NO_EROR       Success
 * @retval        Error code on ERROR
 *
 */


int Read_Flash_Write_Counter_APPS_Efs(uint32 *writes,uint32 *block)
{
	int rc;	
	rc=fs_get_flash_usage_stats_info(&mon_fs_flash_stats_info_write_counter);
	if(rc != 0)
	{
		QAPI_MSG(MSG_SSID_DIAG,MSG_LEGACY_HIGH,
				"fs_get_flash_usage_stats_info failed with  ret %x",rc);
		return FS_FLASH_ERR;
	}        
	*writes = mon_fs_flash_stats_info_write_counter.write_region_stats[FS_FOP_LOG];

	rc=fs_get_block_info(&mon_fs_block_info);
	if(rc != 0)
	{
		QAPI_MSG(MSG_SSID_DIAG,MSG_LEGACY_HIGH,
				"fs_get_block_info failed with  ret %x",rc);
		return FS_BLOCK_ERR;
	}
	*block=mon_fs_block_info.log_block_count;

	return 0;
}

/*
 *
 * @brief :Read_Flash_Write_Counter_MPSS_Efs
 *
 *       This function will help in getting the counters
 *	 from modem side.
 *
 * @param[out]    writes    This will fetch number of writes
 *                            
 *
 * @param[out]    block     Will used to block count from modem
 *
 * @retval        NO_EROR       Success
 * @retval        Error code     ERROR
 *
 */
int Read_Flash_Write_Counter_MPSS_Efs(uint32 *writes,uint32 *block)
{
	struct mpss_efs_counter efs_counter_value;
	uint32 rc = 0;
	uint32 data_len_send = 0;
	uint32 block_count = 0;
	uint32 write_counter = 0;

	if(qmi_client_init_count==0)
	{
		/* QMI client API to initialize the QMI client related settings */
		rc=qmi_util_client_start();
		if (rc != 0)
		{
			QAPI_MSG(MSG_SSID_DIAG,MSG_LEGACY_HIGH,
					"qmi_util_client_start failed with  ret %x",rc);
			return rc;
		}
		qmi_client_init_count++;

	}
	rc=utils_connect_server(&efs_counter_value); 	
	if (rc != 0)
	{
		QAPI_MSG(MSG_SSID_DIAG,MSG_LEGACY_HIGH,
				"utils_connect_server failed with ret %x",rc);
		return rc;
	}

	*writes=efs_counter_value.fs_counter;
	*block=efs_counter_value.fs_block_count;
	QAPI_MSG(MSG_SSID_DIAG,MSG_LEGACY_HIGH,
				"MPSS flash values Flash  block_count %d pagecnt %d",
				efs_counter_value.fs_block_count,efs_counter_value.fs_page_count);	
	return 0;

}
/*
 * Function to get the required counters from the both APPS and MPSS
 *
 * @brief : qapi_utils_efs_log_writecount
 *
 *       This function will help in getting the counters for Modem and APPS.
 *
 * @param[in]     subsys    Get the counters from respective subsystem
 *			 	0- APPS Threadx
 *				1- Modem 
 *
 * @param[out]    writes    This will fetch number of writes
 *
 *
 * @param[out]    block     Will used to block count from modem
 *
 * @retval        NO_EROR       Success
 * @retval        Error code     ERROR
 */
int qapi_utils_efs_log_writecount(int subsys,uint32 *write_count,uint32 *num_block)
{
	int ret=0;
	uint32 wr_cnt = 0;
	uint32 num_blk = 0;
	uint32 data_len = 0;

	switch(subsys)
	{
		case QAPI_UTILS_SUBSYS_APPS:
			{
				ret=Read_Flash_Write_Counter_APPS_Efs(&wr_cnt,&num_blk);
				if(ret!=0)
				{
					QAPI_MSG(MSG_SSID_DIAG,MSG_LEGACY_HIGH,
							"Read_Flash_Write_Counter_APPS_Efs failed with  ret %x",ret);
					return ret;	
				}
				data_len=sizeof(wr_cnt);
				QAPI_MSG(MSG_SSID_DIAG,MSG_LEGACY_HIGH,
						 "Read_Flash_Write_Counter_APSS_Efs write_count %d block count %d",wr_cnt,num_blk);
				qurt_copy_to_user((qurt_addr_t *)write_count,(qurt_size_t)data_len, &wr_cnt,(qurt_size_t)data_len);
				qurt_copy_to_user((qurt_addr_t *)num_block,(qurt_size_t)data_len, &num_blk,(qurt_size_t)data_len);


			}
			break;
		case QAPI_UTILS_SUBSYS_MPSS:
			{
				ret=Read_Flash_Write_Counter_MPSS_Efs(&wr_cnt,&num_blk);
				if(ret!=0)
				{
					QAPI_MSG(MSG_SSID_DIAG,MSG_LEGACY_HIGH,
							"Read_Flash_Write_Counter_MPPS_Efs failed with  ret %x",ret);
					return ret;
				}
				data_len=sizeof(wr_cnt);
				QAPI_MSG(MSG_SSID_DIAG,MSG_LEGACY_HIGH,
							"Read_Flash_Write_Counter_MPSS_Efs write_count %d block_count %d",wr_cnt,num_blk);
				qurt_copy_to_user((qurt_addr_t *)write_count,(qurt_size_t)data_len, &wr_cnt,(qurt_size_t)data_len);
				qurt_copy_to_user((qurt_addr_t *)num_block,(qurt_size_t)data_len, &num_blk,(qurt_size_t)data_len);
			}
			break;
	}


	return ret;
}

/*======================================================================
  FUNCTIONS
  =======================================================================*/
int qapi_utils_handler(UINT id, UINT a, UINT b, UINT c, UINT d, UINT e, 
		UINT f, UINT g, UINT h, UINT i, UINT j, UINT k, UINT l)
{
	int ret=0; 

	switch(id)
	{
		case TXM_QAPI_FLASH_COUNTER:
			{
				ret = qapi_utils_efs_log_writecount((int)a, ((uint32 *)b),((uint32 *)c));
				if (ret > 0)
					return ret;
			}
			break;
		default:
			ret = QAPI_ERR_INVLD_ID;
	}
	return ret;
}



