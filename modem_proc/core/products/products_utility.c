 /*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                    PRODUCTS UTILITY

GENERAL DESCRIPTION

  It has all utility functions related to products 

FUNCTIONS
	get_modem_boot_mode



Copyright (c) 2002 - 2015,2019 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

when       who     		what, where, why
--------   ---     		----------------------------------------------------------
09/13/23   hsathu       ADDED flash log  support for debug 
08/12/23   hsathu       ADDED Abrupt shutdown API in SOI
05/06/18   ahemanth     ADDED tiny_modem_flow function and ENUM 
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stringl/stringl.h>
#include "products_utility.h"
#include "com_dtypes.h"
#include "pm_pon.h"
#include "HALhwio.h"
#include "msmhwiobase.h"
#include "pmapp_pon.h"
#include "fs_public.h"
#include "time_svc.h"
#include "fs_err.h"
#include "pm_rtc.h"



#define PRODUCT_FLASH_LOG_FILE_SIZE 1024*2
#define MAX_STRNG_SIZE 512
#define EFS_PON_CNT_FILE_1 "/product/pon_cnt_file1.txt"
#define EFS_PON_CNT_FILE_2 "/product/pon_cnt_file2.txt"
#define EFS_PON_RTC_RESET  "/product/pon_rtc_reset.txt"
#define EFS_PON_CNT_FILE_NO "/product/product_flash_file_no"

/*----------------------------------------------------------------------------
 * MODULE: MPM2_PSHOLD
 *--------------------------------------------------------------------------*/

#define MPM2_PSHOLD_REG_BASE                       (MPM2_MPM_BASE      + 0x0000b000)

#define HWIO_MPM2_MPM_PS_HOLD_ADDR                 (MPM2_PSHOLD_REG_BASE      + 0x00000000)
#define HWIO_MPM2_MPM_PS_HOLD_RMSK                        0x1
#define HWIO_MPM2_MPM_PS_HOLD_IN          \
        in_dword_masked(HWIO_MPM2_MPM_PS_HOLD_ADDR, HWIO_MPM2_MPM_PS_HOLD_RMSK)
#define HWIO_MPM2_MPM_PS_HOLD_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_PS_HOLD_ADDR, m)
#define HWIO_MPM2_MPM_PS_HOLD_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_PS_HOLD_ADDR,v)
#define HWIO_MPM2_MPM_PS_HOLD_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_PS_HOLD_ADDR,m,v,HWIO_MPM2_MPM_PS_HOLD_IN)
#define HWIO_MPM2_MPM_PS_HOLD_PSHOLD_BMSK                 0x1
#define HWIO_MPM2_MPM_PS_HOLD_PSHOLD_SHFT                 0x0

/*===========================================================================

FUNCTION get_modem_boot_mode

DESCRIPTION
	This function can be used to check the flow and different

DEPENDENCIES
  static map for shared IMEM region

RETURN VALUE
  ENUM of Type FLowtype

===========================================================================*/

flowType get_modem_boot_mode(void)
{
  uint32 *ocimem_address = (uint32 *)SHARED_IMEM_PMIC_BASE;
  unsigned int bitcheck ;
  bitcheck = *ocimem_address >> MODEM_BOOT_TYPE_BIT;
  if ((bitcheck & 1) == 1)
  {
	bitcheck = *ocimem_address >> MODEM_BOOT_APPS_LOAD_CHECK_BIT;
	if ((bitcheck & 1) == 1)
	{
		return MODEM_BOOT_TINY_WITH_APSS;
	}
	else 
	{
		return MODEM_BOOT_TINY_NO_APSS;
	}			
  }
  else
  {
	  return MODEM_BOOT_FULL;
  } 
}

/**
 *
 * @brief sys_shutdown_abrupt_pshold
 *
 * Power Offs the chip by programming pmic chip directly from Modem.
 * This works in Modem pageOnly mode case and doesnt interact with Apps.
 * This is not graceful shutdown and recommended only if no clients register
 * with RCINIT shutdown callbacks
*/
void sys_shutdown_abrupt_pshold(void)
{
	
#if defined FEATURE_BUILD_PAGEONLY_IMAGE || defined FEATURE_BUILD_SENSORONLY_IMAGE
/*-----------------------------------------------------------------------*/
/* Configure the PMIC for shutdown.                                      */
/*-----------------------------------------------------------------------*/

pm_app_pon_pshold_cfg(PM_APP_PON_CFG_NORMAL_SHUTDOWN);

//pull ps_hold.

HWIO_OUTF(MPM2_MPM_PS_HOLD, PSHOLD, 0);

#endif

}
//flash logging
void product_flash_log(void)
{
	int fd = 0  , fs_rsp = 0;
	uint8 product_flash_file_no=0;

	char  str[MAX_STRNG_SIZE];
	pm_rtc_time_type time;
	pm_pon_debug_type debug_data;
	uint8 boot_misc =0;
	uint64 ts = 0;
	/*Read RTC time stamp*/
	pm_rtc_rw_get_ms_time(0 ,&time); 
	/*Read PMIC MISC bit set by SNSMGR in previos boot*/
	pm_app_misc_get_cfg(PM_APP_MISC_CFG_BOOT_OPTION , &boot_misc);
	/*Fetch PON AON PMIC version data*/
	pmapp_pon_reason_debug(&debug_data);
	/*Fetch time services time stamp*/
	ts = time_get_ms_native();  
	snprintf(str, MAX_STRNG_SIZE,"PON-POFF:0x8c0:%d 0x8c2:%d 0x8c4:%d 0x8c5:%d 0x8c7:%d 0x8c8:%d 0x8C9:%d 0x8CA:%d 0x8CB:%d AON1:%d AON2:%d AON3:%d AON4:%d PMIC_version:%d -%d -%d  BOOT_MISC %d, RTC :%lu-%lu ,TS:%llu \n",debug_data.pon_data[0],debug_data.pon_data[1],debug_data.pon_data[2],debug_data.pon_data[3] ,debug_data.pon_data[4] ,debug_data.pon_data[5] ,debug_data.pon_data[6] ,debug_data.pon_data[7] ,debug_data.pon_data[8] , debug_data.AON_data[0] ,debug_data.AON_data[1] ,debug_data.AON_data[2] ,debug_data.AON_data[3] ,debug_data.pmic_version[0] ,debug_data.pmic_version[1] , debug_data.pmic_version[2] , boot_misc , time.sec,time.msec ,ts);
	
	efs_get(EFS_PON_CNT_FILE_NO , &product_flash_file_no ,8);

if(product_flash_file_no <= 0)
{
	product_flash_file_no =0;
	efs_put(EFS_PON_CNT_FILE_NO,&product_flash_file_no,8, O_CREAT|O_AUTODIR,077);
}
if(time.sec < 20) //LOG RTC RESET registers in seperate file
{
	efs_truncate("/product/pon_rtc_reset.txt" ,0);
	fd=efs_open(EFS_PON_RTC_RESET,O_CREAT | O_WRONLY | O_APPEND, 0666);
	if (-1 == efs_write(fd, str, strlen(str)))
	{
		FS_MSG_ERROR_0("PRODUCT EFS file write failed\n");
		efs_close(fd);
		return ;
	}

	efs_close(fd);

}
if (product_flash_file_no ==0)
{
	fd=efs_open(EFS_PON_CNT_FILE_1,O_CREAT | O_WRONLY | O_APPEND, 0666);	
	fs_rsp = efs_lseek(fd,0,SEEK_END);
	
	if((fs_rsp + sizeof(str)) > PRODUCT_FLASH_LOG_FILE_SIZE)
	{
		
		product_flash_file_no =1;
		efs_close(fd);
		efs_truncate("/product/pon_cnt_file2.txt" ,0); //reset second file and start writing.
		efs_put(EFS_PON_CNT_FILE_NO,&product_flash_file_no,8, O_CREAT|O_AUTODIR,077);
		fd=efs_open(EFS_PON_CNT_FILE_2,O_CREAT | O_WRONLY | O_APPEND, 0666);	
	}
}
else 
{
		fd=efs_open(EFS_PON_CNT_FILE_2,O_CREAT | O_WRONLY | O_APPEND, 0666);	
		fs_rsp = efs_lseek(fd,0,SEEK_END);
		
	if((fs_rsp + sizeof(str)) > PRODUCT_FLASH_LOG_FILE_SIZE)
	{
		
		product_flash_file_no =0; //move logging to first file
		efs_close(fd);
		efs_truncate("/product/pon_cnt_file1.txt" ,0); //reset first file and start writing.
		efs_put(EFS_PON_CNT_FILE_NO,&product_flash_file_no,8, O_CREAT|O_AUTODIR,077);
		fd=efs_open(EFS_PON_CNT_FILE_1,O_CREAT | O_WRONLY | O_APPEND, 0666);	
	}
}

if (-1 == efs_write(fd, str, strlen(str)))
{
	FS_MSG_ERROR_0("PRODUCT EFS file write failed\n");
	efs_close(fd);
	return ;
}

	efs_close(fd);
}



