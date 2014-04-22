/*==================================================================
 *
 * FILE:        ddi_mdm9x05.c
 *
 * DESCRIPTION:
 *
 *
 *        Copyright © 2017 Qualcomm Technologies Incorporated.
 *               All Rights Reserved.
 *               QUALCOMM Proprietary
 *==================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *
 * YYYY-MM-DD   who     what, where, why
 * ----------   ---     ----------------------------------------------
 *2019-02-25   ds      DDI framwork re-architecture for Twizy
 *2016-04-09   zhz     Add DQ Vref CDC shmoo test
 *2016-12-15   zhz     Initial file creation
 */
#include "ddi_mdm9x05.h"
#include "ddi_initialize.h"
#include "ddi_tool.h"
#include "ddi_target.h"
#include "DDIChipInfo.h"
#include "DalDevice.h"
#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include "ddr_params.h"
#include "boot_cache_mmu.h"

#define NAME_STR_RESULT    "Result"
#define NAME_STR_CLKLVL    "ClockLevel"
#define NAME_STR_VREF      "Vref"
#define NAME_STR_ROUT      "Rout"
#define NAME_STR_BYTE      "Byte"
#define NAME_STR_RWTYPE    "RWType"
#define NAME_STR_STRTADDR  "Start_Address"
#define NAME_STR_TST_SIZE  "Test_Size"
#define NAME_STR_ITEM_A    "TestingDDRSignalLines"
#define NAME_STR_ITEM_B    "DDRBasicllyTesting"
#define NAME_STR_ITEM_C    "DDRDefectTest"
#define NAME_STR_ITEM_D    "DDRIntervalStressTest"
#define NAME_STR_ITEM_E    "DQLineShmoo"
#define NAME_STR_ITEM_F    "CALineShmoo"

uint32 ddr_type;

ddi_firehose_rspons item_result_rsp[] =
{
    {NAME_STR_RESULT,    DDI_FIREHOSE_BOOLEAN},
    {"",                 DDI_FIREHOSE_NULL}
};

ddi_firehose_params item_dqlineshmoo_arg[] = {
  {NAME_STR_CLKLVL,   DDI_FIREHOSE_UINT32,          0,       0,              0,             ""}, // max, default value will be updated in pre-work
  {NAME_STR_VREF,     DDI_FIREHOSE_UINT32,          0,       1200,           600,           ""},
  {NAME_STR_ROUT,     DDI_FIREHOSE_UINT32,          0,       7,              5,             ""},
  {NAME_STR_BYTE,     DDI_FIREHOSE_UINT32,          0,       3,              0,             ""},
  {NAME_STR_RWTYPE,   DDI_FIREHOSE_UINT32,          0,       2,              0,             ""},
  {NAME_STR_STRTADDR, DDI_FIREHOSE_UINT32,          0,       0xffffffff,     0x80000000,    ""},
  {NAME_STR_TST_SIZE, DDI_FIREHOSE_UINT32,          0,       0xffffffff,     1024 * 1024,   ""},
  {"",                DDI_FIREHOSE_NULL,            0 ,      0,              0,             ""}
};

ddi_firehose_params item_calineshmoo_arg[] = {
  {NAME_STR_CLKLVL,   DDI_FIREHOSE_UINT32,        0,        0,          0,        ""}, // max, default value will be updated in pre-work
  {NAME_STR_VREF,     DDI_FIREHOSE_UINT32,        0,        1200,       600,      ""},
  {"",              DDI_FIREHOSE_NULL,            0,        0,             0,     ""}
};

ddi_firehose_item ddi_firehose_pack_items[] =
{
  	{NAME_STR_ITEM_A, 1, DDR_TYPE_MASK(DDR_TYPE_LPDDR2)|DDR_TYPE_MASK(DDR_TYPE_LPDDR3) , 		NULL, 			item_result_rsp, 		NULL, 		NULL, &signalines_dotest,  NULL},
    {NAME_STR_ITEM_B, 1, DDR_TYPE_MASK(DDR_TYPE_LPDDR2)|DDR_TYPE_MASK(DDR_TYPE_LPDDR3) , 		NULL, 			item_result_rsp, 		NULL,		NULL, &basic_dotest,       NULL},
	{NAME_STR_ITEM_C, 1, DDR_TYPE_MASK(DDR_TYPE_LPDDR2)|DDR_TYPE_MASK(DDR_TYPE_LPDDR3) ,       NULL, 			item_result_rsp, 		NULL,		NULL, &ddr_defect_dotest,       NULL},
	{NAME_STR_ITEM_D, 1, DDR_TYPE_MASK(DDR_TYPE_LPDDR2)|DDR_TYPE_MASK(DDR_TYPE_LPDDR3) ,       NULL, 			item_result_rsp, 		NULL,		NULL, &ddr_intervalstress_dotest,       NULL},
    {NAME_STR_ITEM_E, 1, DDR_TYPE_MASK(DDR_TYPE_LPDDR2)|DDR_TYPE_MASK(DDR_TYPE_LPDDR3) , item_dqlineshmoo_arg, item_result_rsp, &dqlineshmoo_init, NULL, &dqlineshmoo_dotest, NULL},
    {NAME_STR_ITEM_F, 1, DDR_TYPE_MASK(DDR_TYPE_LPDDR2)|DDR_TYPE_MASK(DDR_TYPE_LPDDR3) , item_calineshmoo_arg, item_result_rsp, &calineshmoo_init, NULL, &calineshmoo_dotest, NULL},
    {"",              0,     											0,                                                                							NULL,                 NULL,              NULL,      NULL,         NULL,    	   NULL}
};

ddi_firehose_info ddi_firehose_pack_info[DDI_FIREHOSE_SIZE_INFO_MAX] =
{
    {"", ""}
};

ddi_firehose_item_set ddi_firehose_pack =
{
    DDI_PLATFORM,               //platform
    DDI_VERSION,                //version
    ddi_firehose_pack_info,     //info
    ddi_firehose_pack_items,    //items
    &ddi_firehose_pre_work,     //preworker
    &ddi_firehose_post_work     //postworker
};

boolean ddr_defect_dotest(void* this_p)
{
  ddi_firehose_item_p item = NULL;
  boolean result = TRUE;
  if(NULL == this_p) return FALSE;
   item = (ddi_firehose_item_p)this_p;

   result = ddr_defect_interval_test(0,SDRAM_INTERFACE_0,SDRAM_CS0);
		
   ddi_firehose_print_response(item->item_name,
                              NAME_STR_RESULT,
                              ddi_firehose_bool_name[result]);  
  return TRUE;
}

boolean ddr_intervalstress_dotest(void* this_p)
{
	ddi_firehose_item_p item = NULL;
	boolean result = TRUE;
	if(NULL == this_p) return FALSE;
	item = (ddi_firehose_item_p)this_p;
	
	result = ddr_defect_interval_test(1,SDRAM_INTERFACE_0,SDRAM_CS0);
		
	ddi_firehose_print_response(item->item_name,
							NAME_STR_RESULT,
							ddi_firehose_bool_name[result]);
	
	return TRUE;	
}

boolean signalines_dotest(void* this_p)
{
	boolean result = TRUE ;
	ddi_firehose_item_p item = NULL;
	
	if(NULL == this_p) return FALSE;
	item = (ddi_firehose_item_p)this_p;
	
	result = ddr_basically_test(DDR_TEST_SUITE_ACCESS_RAM_TEST,0,SDRAM_BOTH);
	ddi_firehose_print_response(item->item_name,
								NAME_STR_RESULT,
								ddi_firehose_bool_name[result]);
	
	return TRUE;
}

boolean basic_dotest(void* this_p)
{
	boolean result = TRUE;
	ddi_firehose_item_p item = NULL;
	
	if(NULL == this_p) return FALSE;
	item = (ddi_firehose_item_p)this_p;
	
	result = ddr_basically_test(DDR_TEST_SUITE_ACCESS_RAM_TEST,1,SDRAM_BOTH);
	
	ddi_firehose_print_response(item->item_name,
								NAME_STR_RESULT,
								ddi_firehose_bool_name[result]);
	return TRUE;
}

boolean dqlineshmoo_init(void* this_p)
{
	ddi_firehose_item_p item_this;
	clock_info_p tmpClockInfo;
	ddi_firehose_params_p tmpPar;
	
	if(NULL == this_p) return FALSE;
	item_this = (ddi_firehose_item_p)this_p;
	
	tmpClockInfo = ddr_debug_get_clock_info();
	if(NULL == tmpClockInfo) return FALSE;
	
	// update NAME_STR_CLKLVL
	if(NULL != item_this->item_params_arr){
	tmpPar = item_this->item_params_arr;
	while(NULL != tmpPar->param_name){
		if(0 == strncmp(tmpPar->param_name, NAME_STR_CLKLVL, DDI_FIREHOSE_SIZE_NAME)){
		tmpPar->param_max = tmpClockInfo->ClockLevels - 1;
		tmpPar->param_default = tmpClockInfo->ClockLevels - 1;
		break;
		}
		tmpPar++;
	}
	}
	
	return TRUE;
}

boolean dqlineshmoo_dotest(void* this_p)
{
  ddi_firehose_item_p item = NULL;
  arg_dqshmoo dqArgs;
  boolean result = TRUE;
  if(NULL == this_p) return FALSE;
   item = (ddi_firehose_item_p)this_p;
   
  // clock level
  if(FALSE == ddi_firehose_fetch_parameters( NAME_STR_CLKLVL,item, &dqArgs.cur_clocklevel))
  {
	  return FALSE;
  }
  
  if(FALSE == ddi_firehose_fetch_parameters( NAME_STR_VREF,item, &dqArgs.cur_vref))
  {
	  return FALSE;
  }
   if(FALSE == ddi_firehose_fetch_parameters( NAME_STR_ROUT,item, &dqArgs.cur_rout))
  {
	  return FALSE;
  }
  
  if(FALSE == ddi_firehose_fetch_parameters( NAME_STR_BYTE,item, &dqArgs.cur_byte))
  {
	  return FALSE;
  }
  
  if(FALSE == ddi_firehose_fetch_parameters( NAME_STR_RWTYPE,item, &dqArgs.cur_rwtype))
  {
	  return FALSE;
  }
  if(FALSE == ddi_firehose_fetch_parameters( NAME_STR_STRTADDR,item, &dqArgs.cur_startaddr))
  {
	  return FALSE;
  }
   if(FALSE == ddi_firehose_fetch_parameters( NAME_STR_TST_SIZE,item, &dqArgs.cur_testsize))
  {
	  return FALSE;
  }
 
  result = ddr_debug_dq_cdc_sweep(&dqArgs);
  ddi_firehose_print_response(item->item_name,
                              NAME_STR_RESULT,
                              ddi_firehose_bool_name[result]);
  
  return TRUE;
}

boolean calineshmoo_init(void* this_p)
{
  ddi_firehose_item_p item_this;
  clock_info_p tmpClockInfo;
  ddi_firehose_params_p tmpPar;

  if(NULL == this_p) return FALSE;
  item_this = (ddi_firehose_item_p)this_p;

  tmpClockInfo = ddr_debug_get_clock_info();
  if(NULL == tmpClockInfo) return FALSE;

  // update NAME_STR_CLKLVL
  if(NULL != item_this->item_params_arr){
    tmpPar = item_this->item_params_arr;
    while(NULL != tmpPar->param_name){
      if(0 == strncmp(tmpPar->param_name, NAME_STR_CLKLVL, DDI_FIREHOSE_SIZE_NAME)){
        tmpPar->param_max = tmpClockInfo->ClockLevels - 1;
        tmpPar->param_default = tmpClockInfo->ClockLevels - 1;
        break;
      }
      tmpPar++;
    }
  }

  return TRUE;
}

boolean calineshmoo_dotest(void *this_p)
{
  ddi_firehose_item_p item = NULL;
  arg_cashmoo caArgs;
  boolean result = TRUE ;
  
  if(NULL == this_p) return FALSE;
  item = (ddi_firehose_item_p)this_p;
  
  
  if(FALSE == ddi_firehose_fetch_parameters( NAME_STR_CLKLVL,item, &caArgs.cur_clocklevel))
  {
	  return FALSE;
  }
  
  if(FALSE == ddi_firehose_fetch_parameters( NAME_STR_VREF,item, &caArgs.cur_vref))
  {
	  return FALSE;
  }
  
result = ddr_debug_ca_cdc_sweep(&caArgs);

ddi_firehose_print_response(item->item_name,
                              NAME_STR_RESULT,
                              ddi_firehose_bool_name[result]);

  return TRUE;
}

boolean ddi_firehose_pre_work(void* this_p)
{
  uint32 ii = 0;
  clock_info_p tmpClockInfo;
  char tmpKey[DDI_FIREHOSE_SIZE_NAME];
  uint32 cdc_val = 0;
  DalDeviceHandle *hChipinfo;
  char platform_id[10]={0};
  int16 ret;

  ret = DAL_DeviceAttach(DALDEVICEID_CHIPINFO, &hChipinfo);
  if((ret == DAL_SUCCESS) && (hChipinfo != NULL)) {
    DalChipInfo_GetChipIdString(hChipinfo, platform_id,10);

    snprintf(ddi_firehose_pack.platform,
             strlen(platform_id) + 1,
             "%s",
             platform_id);
  }

  tmpClockInfo = ddr_debug_get_clock_info();
  if(NULL == tmpClockInfo) return FALSE;

  ddi_firehose_add_ddi_infomation(ddi_firehose_pack.info_arr, "rwtype_0", 's', "read_write");
  ddi_firehose_add_ddi_infomation(ddi_firehose_pack.info_arr, "rwtype_1", 's', "read_only");
  ddi_firehose_add_ddi_infomation(ddi_firehose_pack.info_arr, "rwtype_2", 's', "write_only");

#ifdef CDC_DEFAULT_EACH
  // DQ fixed cdc info
  cdc_val = CDC_DEFAULT_RD_DQ0;
  ddi_firehose_add_ddi_infomation(ddi_firehose_pack.info_arr, "DefaultCDC_rd_dq0", 'u', &cdc_val);

  cdc_val = CDC_DEFAULT_RD_DQ1;
  ddi_firehose_add_ddi_infomation(ddi_firehose_pack.info_arr, "DefaultCDC_rd_dq1", 'u', &cdc_val);

  cdc_val = CDC_DEFAULT_RD_DQ2;
  ddi_firehose_add_ddi_infomation(ddi_firehose_pack.info_arr, "DefaultCDC_rd_dq2", 'u', &cdc_val);

  cdc_val = CDC_DEFAULT_RD_DQ3;
  ddi_firehose_add_ddi_infomation(ddi_firehose_pack.info_arr, "DefaultCDC_rd_dq3", 'u', &cdc_val);
  
  // CA fixed cdc info
  cdc_val = CDC_DEFAULT_WR_DQ0;
  ddi_firehose_add_ddi_infomation(ddi_firehose_pack.info_arr, "DefaultCDC_wr_dq0", 'u', &cdc_val);
  
  cdc_val = CDC_DEFAULT_WR_DQ1;
  ddi_firehose_add_ddi_infomation(ddi_firehose_pack.info_arr, "DefaultCDC_wr_dq1", 'u', &cdc_val);

  cdc_val = CDC_DEFAULT_WR_DQ2;
  ddi_firehose_add_ddi_infomation(ddi_firehose_pack.info_arr, "DefaultCDC_wr_dq2", 'u', &cdc_val);

  cdc_val = CDC_DEFAULT_WR_DQ3;
  ddi_firehose_add_ddi_infomation(ddi_firehose_pack.info_arr, "DefaultCDC_wr_dq3", 'u', &cdc_val);
#else
  // DQ fixed cdc info
  cdc_val = CDC_DEFAULT_RD_DQ;
  ddi_firehose_add_ddi_infomation(ddi_firehose_pack.info_arr, "DefaultCDC_rd_dq", 'u', &cdc_val);
  
  // CA fixed cdc info
  cdc_val = CDC_DEFAULT_WR_DQ;
  ddi_firehose_add_ddi_infomation(ddi_firehose_pack.info_arr, "DefaultCDC_wr_dq", 'u', &cdc_val);
#endif

  cdc_val = CDC_DEFAULT_CA;
  ddi_firehose_add_ddi_infomation(ddi_firehose_pack.info_arr, "DefaultCDC_ca", 'u', &cdc_val);

  for(ii = 0; ii < tmpClockInfo->ClockLevels; ii++){
    snprintf(tmpKey,
             DDI_FIREHOSE_SIZE_NAME,
             "clock_level_%u",
             ii);
    ddi_firehose_add_ddi_infomation(ddi_firehose_pack.info_arr, tmpKey, 'u', &tmpClockInfo->ClockFreqKHz[ii]);
  }

  return TRUE;
} 


boolean ddi_firehose_post_work(void* this_p)
{

    return TRUE;
}

//The entry point from SBL.
void ddi_entry(boot_pbl_shared_data_type *pbl_shared)
{
	char temp[40];
	ddr_type = ddr_get_params(SDRAM_INTERFACE_0)->common.device_type;
	snprintf(temp,40,"DDI Entry Point");
	boot_log_message(temp);
    extern pm_sbl_pdm_config_type pm_psi_header;
    pm_psi_header.pm_psi_info->major_ver = 0x01;
    ddi_firehose_update_item_set(&ddi_firehose_pack);
    ddi_init_hw();
    main_firehose();
}

