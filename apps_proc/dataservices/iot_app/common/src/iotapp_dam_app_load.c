/**
 * @file  iotapp_dam_app_load.c
 * @brief File will load the DAM Modules.
 **/
 

/*===========================================================================
  Copyright (c) 2017-2021  by Qualcomm Technologies, Inc.  All Rights Reserved.
 
  Qualcomm Technologies Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.
 ======================================================================*/
/*======================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *  
 *
 *
 * when         who     what, where, why
 * ----------   ---     ------------------------------------------------
 ======================================================================*/


#include "tx_api.h"
#include "txm_module.h"
#include "fs_public.h"

#include "qurt_error.h"
#include "qurt_system.h"
#include "qurt_memory.h"

#include "qurt_txm_integrity_check.h"
#include "qurt_storage_cfg.h"
#include "ds_log.h"


extern qurt_txm_module_manager_setup_module_mmu(TXM_MODULE_INSTANCE *module);

extern int qurt_mapping_create_with_asid(qurt_addr_t vaddr, qurt_addr_t paddr, qurt_size_t size,
                         qurt_mem_cache_mode_t cache_attribs, qurt_perm_t perm, uint32 asid);

#define MAX_DAM_MODULES      10
/* Define this module's instance.  */
TXM_MODULE_INSTANCE* oem_app_dam_module[MAX_DAM_MODULES];
static int module_index = 0; 


#define NAND_STORAGE         0x11A11D
#define EFS_STORAGE          0


#define OEM_APP_PATH_FILE "/datatx/oem_app_path.ini"
#define OEM_APP_PATH_NAND_FILE "/datatx/oem_app_path_nand.ini"

/* FILE_PATH_SZ = 128+strlen("/datatx/"), 
   document should mention to customer that max file name/path is 128*/
#define FILE_PATH_SZ 136 
#define MAX_FILE_PATH_GLOBAL 2048

uint8_t tmp_index = 0;
char*   tmp_module_path = NULL;
void oem_app_load_log_print_err(const char* str, int ret)
{
  if(tmp_module_path != NULL && str != NULL)
  {
    ds_iot_log_strf_err("oem_app_load[%d]: Module[%s] %s ret[%d]", tmp_index, tmp_module_path, str, ret);	
  }
}

void oem_app_load_util(uint32_t storage_type)
{
  int ret = 0;
  int filehandle = 0xFFFFFFFF;
  char path[MAX_FILE_PATH_GLOBAL+1] = {0};
  char mpath[FILE_PATH_SZ*2] = {0};
  qurt_addr_t module_phys_base[MAX_DAM_MODULES] = {0};
  char path_temp[FILE_PATH_SZ] = {0};
  char * tok = NULL;
  char * prevtok = NULL;
  char * tok1 = NULL;
  char * prevtok1 =NULL;


  if(storage_type == EFS_STORAGE)
  {
     ds_iot_log_strf_err("oem_app_load: Module name list EFS %s",path);
     filehandle = efs_open(OEM_APP_PATH_FILE, O_RDONLY);
  }
  else if(storage_type == NAND_STORAGE)
  {
    ds_iot_log_strf_err("oem_app_load: Module name list NAND %s",path);
    filehandle = efs_open(OEM_APP_PATH_NAND_FILE, O_RDONLY);
  }
  else
  {
    ds_iot_ulog_high("oem_app_load: Invalid Storage Type");
  }

  if(filehandle < 0) {
  	ds_iot_ulog_high("oem_app_load: ini file not preset");
    return;
  }
  
  memset(path, 0, MAX_FILE_PATH_GLOBAL);
  ret = efs_read(filehandle, path, MAX_FILE_PATH_GLOBAL);
  efs_close(filehandle);
  if(ret < 0) 
  {
    return;
  }

  path[ret]=';';
  
  prevtok = path;
  ds_iot_log_strf_err("oem_app_load: Module name list %s",path);

  tok = strchr(path,';');

  if(tok == NULL)
  {
    return;
  }

  tmp_module_path = malloc(FILE_PATH_SZ);
  if(tmp_module_path == NULL)
		return;
  
  while((tok != NULL) && (module_index <MAX_DAM_MODULES))
  {
    /*if non-absolute file path provided in ini, fill the path with the default folder that QFLOG use(/datatx/)*/
    TXM_MODULE_INSTANCE* module_tmp = NULL;
	if(storage_type == EFS_STORAGE)
	{
          char *last = NULL;
          memset(path_temp, 0, FILE_PATH_SZ);
          memset(mpath,0, sizeof(mpath));
          *tok = '\0';
          strlcpy(path_temp, prevtok, strlen(prevtok)+1);
          ds_iot_log_strf_err("oem_app_load : Module name %s",path_temp);
          last = strchr(path_temp, '/');
          if(last == NULL)
          {
            #define DEFAULT_PATH "/datatx/"
            strlcpy(mpath, DEFAULT_PATH, FILE_PATH_SZ);
          }
          strlcpy(mpath+strlen(mpath), path_temp, FILE_PATH_SZ - strlen(mpath));
          ds_iot_log_strf_err("oem_app_load : Module path %s",mpath);
	}
	if(storage_type == NAND_STORAGE)
	{
	     char part_name_temp[FILE_PATH_SZ] = {0};
		memset(path_temp, 0, FILE_PATH_SZ);
		memset(part_name_temp,0,sizeof(part_name_temp));
		memset(mpath,0, sizeof(mpath));
		
         *tok = '\0';
		strlcpy(part_name_temp,prevtok,strlen(prevtok)+1);
         prevtok1 = part_name_temp;
		tok1 = strchr(part_name_temp,':');
		if(tok1)
		{
		  *tok1 = '\0';
		}
		else
		return;

		if(prevtok1 )
		{
		  strlcpy(mpath,prevtok1,FILE_PATH_SZ);
		  strlcpy(path_temp,tok1+1,FILE_PATH_SZ);
		}
		else
			return;
	}
    tok++;
    prevtok = tok;
    tok = strchr(tok,';');
    filehandle = 0xfffffff;

    if(storage_type == EFS_STORAGE)
    {
      filehandle = efs_open(mpath, O_RDONLY);
       
      if(filehandle < 0) {
        /* OEM_APP_MODULE_FILE not found or failed to read. */           
	    ds_iot_log_strf_err("oem_app_load: open module[%s] file failed",mpath);
        continue;
      }
       
      /* found OEM_APP_MODULE_FILE, closing the file handle so that Module manager
        can
        use it to load the module */
      efs_close(filehandle);
    }

    module_tmp = oem_app_dam_module[module_index] = malloc(sizeof(TXM_MODULE_INSTANCE));
	if(module_tmp == NULL)
	{
      ds_iot_log_strf_err("QFLOG Module failed to malloc module memory for %s",mpath);
	  break;
	}

    ret = qurt_txm_module_set_storage_type(module_tmp, storage_type);

    strlcpy(tmp_module_path, path_temp, FILE_PATH_SZ);
	tmp_index = module_index;
    if(ret != TX_SUCCESS)
    {
      oem_app_load_log_print_err("set storage type failed", ret);
      continue;
    }
       
    /* load the module from DEMO_MODULE_FILE */
    ret = txm_module_manager_file_load(module_tmp, path_temp,
                                            NULL, mpath);
       
    if(ret != TX_SUCCESS) {
	   oem_app_load_log_print_err("file load failed", ret);
       continue;
    }


    /* check if the image loaded is of MBN format */
    ret = qurt_txm_check_if_MBN_type (module_tmp ,(void *)mpath, TRUE);
  
    /* integrity check needs to be called only if file loaded is of type MBN */
    if(ret == QURT_EOK)
    {
      ret = qurt_txm_integrity_check(module_tmp );
  
      if(ret != QURT_EOK)
      {
        oem_app_load_log_print_err("file Integrity failed", ret);
        ret = txm_module_manager_unload(module_tmp );
        continue;
      }
    }

       
#ifdef TX_DAM_QC_CUSTOMIZATIONS       
    ret = qurt_txm_module_manager_setup_module_mmu(module_tmp);
    if(ret != QURT_EOK)
    {
       oem_app_load_log_print_err("setup mmu failed", ret);
       continue;
    }
    {
      qurt_size_t size = module_tmp ->txm_module_instance_total_ram_usage;
      /* Align to page size of 4KB */
      size = (size + 0xfff) & ~0xfff;
       
      module_phys_base[module_index] = qurt_lookup_physaddr((qurt_addr_t)module_tmp ->txm_module_instance_phys_base);
       
      ret = qurt_mapping_create_with_asid( (qurt_addr_t)module_tmp->txm_module_instance_virt_base,
                                           module_phys_base[module_index],
                                           size, QURT_MEM_CACHE_WRITEBACK, QURT_PERM_FULL,module_tmp ->txm_module_instance_asid);
      if(ret != QURT_EOK)
      {
        oem_app_load_log_print_err("create asid failed", ret);
        continue;
      }
      ret = qurt_mapping_create((qurt_addr_t)module_tmp->txm_module_instance_virt_base,
                                (qurt_addr_t)module_phys_base[module_index], size,
                                QURT_MEM_CACHE_WRITEBACK, QURT_PERM_FULL);
       
      if(ret != QURT_EOK) {
        oem_app_load_log_print_err("mapping create failed", ret);
        continue;
      }
    }
#endif
       
    /* Start the module.  */
    ret = txm_module_manager_start(module_tmp);
    if(ret != TX_SUCCESS) {
       oem_app_load_log_print_err("start failed", ret);
       continue;
    }
    module_index++;
  }

  if(tmp_module_path != NULL) 
  {
  	free(tmp_module_path); 
	tmp_module_path = NULL;
  }
  ds_iot_ulog_high("oem_app_load:Done");
}



void oem_app_load()
{
 ds_iot_ulog_high("OEM APP LOAD CALLED");
 
 oem_app_load_util(EFS_STORAGE);
 oem_app_load_util(NAND_STORAGE);

}

