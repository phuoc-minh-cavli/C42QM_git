/*==============================================================================
                             COPYRIGHT INFORMATION

Copyright (c) 2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/reg/vcs/reg_nv.c_v   1.1   06 Jun 2002 17:30:52   kabraham  $
$Header: //components/rel/mmcp.mpss/6.1.10/nas/reg/src/reg_nv.c#1 $ $DateTime: 2023/04/25 14:39:18 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
11/28/12   am      Fixing compilation warning 
05/31/02   kwa     Initial version.
06/06/02   kwa     Eliminated use of memcpy.
12/30/02   kwa     Replaced ERR with MESSAGE_HIGH when attempt to read the
                   equivalent PLMN list from NVRAM fails in
                   reg_nv_read_equivalent_plmn_list.  This message is
                   consistently displayed if the equivalent PLMN list has never
                   been written to NVRAM.
12/31/02   kwa     Added F3 messages to print out the equivalent PLMN list
                   whenever it is saved to NVRAM.
05/22/03   kwa     Modified reg_nv_write_equivalent_plmn_list to pass the
                   eplmn_list_p parameter as a const pointer to eliminate
                   a lint warning.  Replaced reg_nv_equivalent_plmn_list
                   with reg_nv_item to eliminate lint warnings.
07/19/03   kwa     Added functionality to display the equivalent PLMN list
                   after reading it from NVRAM.
10/15/03   kwa     Added functions for reading and writing RPLMNACT.
12/03/03   kwa     Added functions for reading PS LOCI.
03/07/04   kwa     Added function reg_nv_write_cache.
03/17/04   kwa     Added function reg_nv_read_imsi.
04/28/04   kwa     Added call to reg_log_umts_nas_eplmn_list.
07/07/05   up      Added reg_nv_read_gprs_gcf_flag to support GCF 12.2.2.1d.
07/18/05   up      Added reg_nv_ens_enabled_flag to enable the ENS feature.
07/27/05   up      Added reg_nv_equivalent_plmn_list_get to current ePLMN list.
06/09/05   sn      Added flag reg_nv_data_valid flag.
08/09/11   abhi   Added reading of lpm_power_off EFS NV item
11/15/11  abhi   For EFRPLMNSI Selecting RLPMN if HPLMN is not found
09/12/11   HC     Updated the function definition with void argument 
                  for the deprecated function reg_nv_is_lpm_power_off_enabled()
==============================================================================*/

/*==============================================================================

                            INCLUDE FILES FOR MODULE

==============================================================================*/

#include "mmcp_variation.h"
#include "err.h"
#include "nv.h"
#include "reg_log.h"
#include "reg_nv.h"
#include "reg_task.h"
#include "reg_task_int.h"
#include "reg_timers.h"
#include "fs_lib.h"
#include <stringl/stringl.h>
#include "fs_public.h"
#include "reg_timers.h"
#include "reg_mode.h"
#ifdef FEATURE_MODEM_HEAP
#include "modem_mem.h"
#else
#include "gs_v.h"
#endif
#include "ghdi_exp.h"
#include "ghdi_exp_int.h"
#include "ghdi_nvmem.h"
#include "ghdi_nvmem_v.h"
#include "fs_errno.h"
#include "ds3gpp_api.h"
#include "time_svc.h"
#include "ULogFront.h"
#include "cm_v.h"

extern ULogHandle reg_get_ulog_handle();
/*==============================================================================

                                TYPE DEFINITIONS

==============================================================================*/

/*==============================================================================

                               DATA DECLARATIONS

==============================================================================*/
#define REG_NV_EFNAS_CONFIG_LENGTH 120
#define REG_NV_EFS_EHPLMN_LIST 7

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_SGLTE)
static nv_equivalent_plmn_list_type reg_nv_equivalent_plmn_list_sim[MAX_NAS_STACKS];
#define reg_nv_equivalent_plmn_list reg_nv_equivalent_plmn_list_sim[reg_as_id]
static nv_rplmnact_type             reg_nv_rplmnact_sim[MAX_NAS_STACKS];
#define reg_nv_rplmnact          reg_nv_rplmnact_sim[reg_as_id]
static nv_item_type                 reg_nv_item_sim[MAX_NAS_STACKS];
#define reg_nv_item                reg_nv_item_sim[reg_as_id]

boolean                            reg_nv_gcf_flag_sim[MAX_NAS_STACKS] = {FALSE};                             
#define reg_nv_gcf_flag            reg_nv_gcf_flag_sim[reg_as_id]
/*static nv_cmd_type                  reg_nv_cmd;

static nv_cmd_ext_type              reg_nv_ext_cmd;
#define reg_nv_cmd_per_subs(a) reg_nv_ext_cmd.context = reg_nv_context_id_sim[reg_as_id]; \
                      reg_nv_ext_cmd.nvcmd = a; \
                      nv_cmd_ext(&reg_nv_ext_cmd)*/
static nv_extended_equivalent_plmn_list_type  reg_nv_extended_equivalent_plmn_list_sim[MAX_NAS_STACKS];
#define reg_nv_extended_equivalent_plmn_list  reg_nv_extended_equivalent_plmn_list_sim[reg_as_id] 

#define REG_EFS_CONTEXT_ID_SUB1  0
#define REG_EFS_CONTEXT_ID_SUB2  1
#define REG_EFS_CONTEXT_ID_SUB3  2

reg_nv_vplmn_list_s_type *reg_nv_vplmn_list_sim[MAX_NAS_STACKS]={NULL};
#define reg_nv_vplmn_list (reg_nv_vplmn_list_sim[reg_as_id])
static boolean reg_nv_override_roam_status_sim[MAX_NAS_STACKS] = {FALSE, FALSE};
#define reg_nv_override_roam_status reg_nv_override_roam_status_sim[reg_sub_id]
#else
static boolean reg_nv_override_roam_status = FALSE;
static nv_equivalent_plmn_list_type reg_nv_equivalent_plmn_list;

static nv_rplmnact_type             reg_nv_rplmnact;

static nv_item_type                 reg_nv_item;

boolean                                reg_nv_gcf_flag = FALSE;

/*
static nv_cmd_type                  reg_nv_cmd;

#define reg_nv_cmd_per_subs(a)               nv_cmd(&reg_nv_cmd) */

static nv_extended_equivalent_plmn_list_type  reg_nv_extended_equivalent_plmn_list;
reg_nv_vplmn_list_s_type *reg_nv_vplmn_list= NULL;                   
#endif /* FEATURE_DUAL_SIM*/

#ifdef FEATURE_DUAL_SIM
rex_timer_cnt_type  reg_nv_t3245_timer_stored_value_sim[MAX_AS_IDS];
#define reg_nv_t3245_timer_stored_value  reg_nv_t3245_timer_stored_value_sim[reg_sub_id] 

static rex_timer_cnt_type  reg_nv_validate_sim_timer_value_sim[MAX_AS_IDS];
#define reg_nv_validate_sim_timer_value  reg_nv_validate_sim_timer_value_sim[reg_sub_id] 
static uint8  reg_nv_max_validate_sim_counter_sim[MAX_AS_IDS];
#define reg_nv_max_validate_sim_counter  reg_nv_max_validate_sim_counter_sim[reg_sub_id] 

static rex_timer_cnt_type  reg_nv_t3245_test_timer_value_sim[MAX_AS_IDS];
#define reg_nv_t3245_test_timer_value  reg_nv_t3245_test_timer_value_sim[reg_sub_id] 

uint64	reg_nv_timestamp_at_power_off_sim[MAX_AS_IDS];
#define reg_nv_timestamp_at_power_off reg_nv_timestamp_at_power_off_sim[reg_sub_id]

#ifdef FEATURE_CIOT
uint64	reg_nv_ciot_irat_timer_at_power_off_sim[MAX_AS_IDS];
#define reg_nv_ciot_irat_timer_at_power_off reg_nv_ciot_irat_timer_at_power_off_sim[reg_sub_id]
#endif

static sys_data_roaming_e_type reg_nv_data_romaing_sim[MAX_AS_IDS];
#define reg_nv_data_romaing  reg_nv_data_romaing_sim[reg_sub_id]

#else
rex_timer_cnt_type                             reg_nv_t3245_timer_stored_value ; /*Timer value is in milli seconds*/

static rex_timer_cnt_type                      reg_nv_validate_sim_timer_value;
static uint8                                   reg_nv_max_validate_sim_counter;
#ifdef FEATURE_CIOT
uint64								           reg_nv_ciot_irat_timer_at_power_off;
#endif
static rex_timer_cnt_type                      reg_nv_t3245_test_timer_value = REG_CIOT_IRAT_TIMER_DEFAULT_VALUE;
uint64								           reg_nv_timestamp_at_power_off;
#ifdef DISABLE_9x05_OPTIMIZATIONS
static sys_data_roaming_e_type                 reg_nv_data_romaing;
#endif

#endif

boolean reg_is_qrd_device = FALSE;                             


#ifdef FEATURE_DUAL_SIM
#ifdef FEATURE_ENHANCED_NW_SELECTION
#define reg_sim_read_ens_flag() reg_sim_per_nas_stacks_read_ens_flag((sys_modem_as_id_e_type)reg_sub_id)
#endif
#endif

#if defined(FEATURE_TRIPLE_SIM) || defined(FEATURE_SGLTE_DUAL_SIM)
static boolean                      reg_nv_data_valid_sim[MAX_NAS_STACKS] = { FALSE, FALSE,FALSE};
#define reg_nv_data_valid           reg_nv_data_valid_sim[reg_as_id] 

#elif defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE 
static boolean                      reg_nv_data_valid_sim[MAX_NAS_STACKS] = { FALSE, FALSE};
#define reg_nv_data_valid           reg_nv_data_valid_sim[reg_as_id] 
#else
static boolean                      reg_nv_data_valid = FALSE;
#endif

#ifdef FEATURE_ENHANCED_NW_SELECTION
/*
** NV Flag to enable the ENS Feature.
*/
static boolean                      reg_nv_ens_enabled_flag = FALSE;
#endif

#if defined(FEATURE_LTE) && defined(FEATURE_MMSS_3_1)
#ifdef FEATURE_DUAL_SIM
static boolean                      reg_nv_ignore_uplmn_sim[MAX_AS_IDS];
#define reg_nv_ignore_uplmn reg_nv_ignore_uplmn_sim[reg_sub_id]
#else
static boolean                      reg_nv_ignore_uplmn = FALSE;
#endif 
#endif

static boolean                      reg_nv_imsi_switch      = FALSE;

static boolean                      reg_nv_wcdma_freq_lock_enabled = FALSE;
boolean                             reg_nv_lpm_power_off = TRUE;
static boolean                     reg_nv_efrplmnsi_selection = FALSE;
#if defined FEATURE_TDSCDMA || defined FEATURE_SGLTE
#ifdef FEATURE_DUAL_SIM
static boolean                      reg_nv_forced_irat_enabled_sim[MAX_AS_IDS];
#define reg_nv_forced_irat_enabled reg_nv_forced_irat_enabled_sim[reg_sub_id]

static uint32                       reg_nv_hplmn_irat_search_timer_sim[MAX_AS_IDS]; /*Timer value is in milli seconds*/;
#define reg_nv_hplmn_irat_search_timer reg_nv_hplmn_irat_search_timer_sim[reg_sub_id]

lte_search_timer_feature_config_T reg_nv_lte_search_config_sim[MAX_AS_IDS];
#define reg_nv_lte_search_config  reg_nv_lte_search_config_sim[reg_sub_id]
#else
static boolean                      reg_nv_forced_irat_enabled = FALSE;
static uint32                       reg_nv_hplmn_irat_search_timer /*Timer value is in milli seconds*/;
lte_search_timer_feature_config_T   reg_nv_lte_search_config;
#endif 
#endif
#ifdef FEATURE_TDSCDMA 
static reg_sim_plmn_list_s_type    reg_nv_ehplmn_efs_list;
#define    REG_HPLMN_IRAT_SEARCH_TIMER     5*60*1000  /*default to 5 minutes*/
#endif

static reg_sim_plmn_list_s_type     reg_nv_tdscdma_op_plmn_efs_list; 

boolean reg_nv_is_camp_on_2g_if_reg_failure = FALSE;
#if defined(FEATURE_FEMTO_CSG) || defined (FEATURE_NAS_ECALL)
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_SGLTE)
static sys_plmn_id_s_type  reg_nv_rplmn_sim[MAX_NAS_STACKS];
#define  reg_nv_rplmn reg_nv_rplmn_sim[reg_as_id]

static boolean reg_nv_rplmn_dup_sim[MAX_NAS_STACKS];
#define reg_nv_rplmn_dup reg_nv_rplmn_dup_sim[reg_as_id]
#else
 /*
   RPLMN read from NV and also RPLMN used by REG. 
   This is updated with current RPLMN in all net sel modes except MANUAL CSG
*/
static sys_plmn_id_s_type  reg_nv_rplmn; 

/*
  This is set when reg_nv_rplmn is not updated in manaul csg mode.
  If device is powered off and this is set then only RPLMN is written to NV
*/
static boolean reg_nv_rplmn_dup;          
#endif /*FEATURE_DUAL_SIM*/

#ifdef FEATURE_FEMTO_CSG
#ifdef FEATURE_DISABLE_CSG_BYDEFAULT
static sys_csg_support_e_type    reg_nv_csg_support = SYS_CSG_SUPPORT_DISABLED;
#else
static sys_csg_support_e_type    reg_nv_csg_support = SYS_CSG_SUPPORT_WCDMA;
#endif 
#endif 

#endif 

#ifdef FEATURE_DUAL_SIM
boolean                                reg_nv_is_hplmn_to_be_selected_sim[MAX_AS_IDS];
#define reg_nv_is_hplmn_to_be_selected reg_nv_is_hplmn_to_be_selected_sim[reg_sub_id]

boolean                                reg_nv_is_roaming_on_in_eplmn_sim[MAX_AS_IDS];
#define reg_nv_is_roaming_on_in_eplmn  reg_nv_is_roaming_on_in_eplmn_sim[reg_sub_id]

boolean                                reg_nv_is_sim_invalid_recovery_enabled_sim[MAX_AS_IDS];
#define reg_nv_is_sim_invalid_recovery_enabled reg_nv_is_sim_invalid_recovery_enabled_sim[reg_sub_id]

boolean                                reg_nv_is_pseudo_reselection_allowed_sim[MAX_AS_IDS];
#define reg_nv_is_pseudo_reselection_allowed reg_nv_is_pseudo_reselection_allowed_sim[reg_sub_id]

#else
boolean                                reg_nv_is_hplmn_to_be_selected = FALSE;
boolean                                reg_nv_is_roaming_on_in_eplmn = FALSE; 
boolean                                reg_nv_is_sim_invalid_recovery_enabled = FALSE;
boolean                                reg_nv_is_pseudo_reselection_allowed = TRUE;
#endif

reg_sim_plmn_w_act_s_type  *reg_nv_preferred_plmn_list = NULL; 
int32 reg_nv_pref_plmn_list_length = 0;
#define REG_NV_MAX_PREF_PLMN_LIST 50
extern nas_crit_sect_type reg_tdscdma_op_list_crit_sect;

static ciot_params_s_type reg_nv_ciot_params = {60, 20};

#define INDEX_M1  0
#define INDEX_NB1 1
/* This is the database that contains the MCCs and corresponding bands.
   One location for each rat */
static reg_nv_mcc_bands_db_s_type reg_nv_mcc_bands_db[2];
/* This is the database of MCCs and enabled RATs*/
static reg_nv_mcc_rats_s_type reg_nv_mcc_rats_db[NUM_MCC_AND_RATS];

static rex_timer_cnt_type             reg_nv_interlace_scan_timer_value;
static dword                          reg_nv_nb1_acq_tier3_timer_value;
static dword                          reg_nv_nb1_fullband_tier3_timer_value;

#ifdef FEATURE_DUAL_SIM
boolean                                reg_nv_psm_r12_enabled_sim[MAX_NAS_STACKS] = {FALSE, FALSE};
#define reg_nv_psm_r12_enabled         reg_nv_psm_r12_enabled_sim[reg_as_id]
boolean                                reg_nv_psm_restore_pending_sim[MAX_NAS_STACKS] = { FALSE, FALSE };
#define reg_nv_psm_restore_pending     reg_nv_psm_restore_pending_sim[reg_as_id]
reg_nv_psm_cache_s_type                *reg_nv_psm_cache_ptr_sim[MAX_NAS_STACKS] = { NULL, NULL };
#define reg_nv_psm_cache_ptr           reg_nv_psm_cache_ptr_sim[reg_as_id]
uint32                                 reg_nv_psm_nv_list_bitmask_sim[MAX_NAS_STACKS] = { 0, 0 };
#define reg_nv_psm_nv_list_bitmask     reg_nv_psm_nv_list_bitmask_sim[reg_as_id]
#else
boolean                                reg_nv_psm_r12_enabled = FALSE;
boolean                                reg_nv_psm_restore_pending = FALSE;
reg_nv_psm_cache_s_type                *reg_nv_psm_cache_ptr = NULL;
uint32                                 reg_nv_psm_nv_list_bitmask = 0;
#endif


#ifdef FEATURE_DUAL_SIM
#define reg_ghdi_nvmem_read(a,b) ghdi_nvmem_read_per_subs(a,b,reg_nv_context_id_sim[reg_as_id])
#define reg_ghdi_nvmem_write(a,b) ghdi_nvmem_write_per_subs(a,b,reg_nv_context_id_sim[reg_as_id])

#else
#define reg_ghdi_nvmem_read(a,b) ghdi_nvmem_read(a,b)
#define reg_ghdi_nvmem_write(a,b) ghdi_nvmem_write(a,b)
#endif

boolean reg_task_sim_available = FALSE;
boolean reg_task_power_up_init = FALSE;


/*==============================================================================

                          STATIC FUNCTION DEFINITIONS

==============================================================================*/

/*==============================================================================
FUNCTION NAME:  reg_nv_insert_mcc_rats

DESCRIPTION
  This function inserts a new element to the database (hash table)

RETURN VALUE
  None
==============================================================================*/
static void reg_nv_insert_mcc_rats (reg_nv_mcc_rats_s_type mcc_rat)
{
    uint32 index = 0;

    /* Walk through the list until first empty location is found for this rat */
    for (index = 0; index < NUM_MCC_AND_RATS; ++index)
    {       
        if (reg_nv_mcc_rats_db[index].mcc == mcc_rat.mcc)
        {      
            /* There is an entry for this MCC already*/
           return;
        }

        if (reg_nv_mcc_rats_db[index].mcc == 0)
        {      
           break;
        }        
    }
    if (index >= NUM_MCC_AND_RATS)
    {
         /* Database is full */
         return;
    }
    /* Insert a new element into the database */
    reg_nv_mcc_rats_db[index] = mcc_rat;
    for (index = 0; index < REG_NV_MAX_RATS_LENGTH; ++index)
    {
        MSG_HIGH_DS_2(REG_SUB, "=REG= CIOT Inserted RAT %d for MCC %d", 
            mcc_rat.rat[index], mcc_rat.mcc);
    }    
}
/*==============================================================================
FUNCTION NAME:  reg_nv_insert_mcc_bands

DESCRIPTION
  This function inserts a new element to the database (hash table)

RETURN VALUE
  None
==============================================================================*/
static void reg_nv_insert_mcc_bands(reg_nv_mcc_band_s_type ciot_mcc_bands, sys_sys_mode_e_type rat) 
{   
    uint32 index = 0;
    uint32 i = 0;
    reg_nv_mcc_band_s_type item;    

    if ((ciot_mcc_bands.mcc == 0)&&
		((ciot_mcc_bands.bands[0] == 0 )&&
		  (ciot_mcc_bands.bands[1] == 0 ) &&
		  (ciot_mcc_bands.bands[2] == 0 ) &&
		  (ciot_mcc_bands.bands[3] == 0 ))&&
        ((rat != SYS_SYS_MODE_LTE_NB1) && (rat != SYS_SYS_MODE_LTE_M1)))
    {
        return;
    }
    if (rat == SYS_SYS_MODE_LTE_M1)
    {
        i = INDEX_M1;
    }
    else if (rat == SYS_SYS_MODE_LTE_NB1)
    {
        i = INDEX_NB1;
    }
    else
    {
      return;
    }
    memscpy(&item, sizeof(reg_nv_mcc_band_s_type), &ciot_mcc_bands, sizeof(reg_nv_mcc_band_s_type));

    /* Walk through the list until first empty location is found for this rat */
    for (index = 0; index < NUM_MCC_AND_BANDS; ++index)
    {       
        if (reg_nv_mcc_bands_db[i].reg_nv_mcc_band_per_rat[index].mcc == ciot_mcc_bands.mcc)
        {      
            /* Same mcc again */
            return;
        }
        if (reg_nv_mcc_bands_db[i].reg_nv_mcc_band_per_rat[index].mcc == 0)
        {      
           break;
        }        
    }
    if (index >= NUM_MCC_AND_BANDS)
    {
         /* Database is full */
         return;
    }
    /* Insert a new element into the database */
    reg_nv_mcc_bands_db[i].reg_nv_mcc_band_per_rat[index] = item;
    reg_nv_mcc_bands_db[i].rat = rat;
}
/*==============================================================================
FUNCTION NAME:  reg_nv_search_mcc_bands

DESCRIPTION
  This function finds an element in the hash table indicated by the key
  parameter passed in

RETURN VALUE
  Element of the hash table, which is MCC allocated Bands
==============================================================================*/
reg_nv_mcc_band_s_type reg_nv_search_mcc_bands(sys_sys_mode_e_type rat, uint32 mcc) 
{   
   int i,index;
   reg_nv_mcc_band_s_type ret; 
   ret.bands[0] = ret.bands[1] = ret.bands[2] = ret.bands[3] = 0;
   ret.mcc = 0; 
   if ((mcc == 0)||((rat != SYS_SYS_MODE_LTE_NB1) && (rat != SYS_SYS_MODE_LTE_M1)))
   {
        return ret;
   }
   if (rat == SYS_SYS_MODE_LTE_M1)
   {
        i = INDEX_M1;
   }
   else if (rat == SYS_SYS_MODE_LTE_NB1)
   {
        i = INDEX_NB1;
   }
    
   
   for (index = 0; index < NUM_MCC_AND_BANDS; ++index)
   {       
     MSG_HIGH_DS_1(REG_SUB, "=REG= REG DB search for mcc %u", reg_nv_mcc_bands_db[i].reg_nv_mcc_band_per_rat[index].mcc);
     if(reg_nv_mcc_bands_db[i].reg_nv_mcc_band_per_rat[index].mcc == mcc)
     {
       return reg_nv_mcc_bands_db[i].reg_nv_mcc_band_per_rat[index]; 
     }       
   }
   return ret;
}

/*==============================================================================
FUNCTION NAME:  reg_nv_search_mcc_rats

DESCRIPTION
  This function finds the mcc in the hash table and 
  returns the RATs that are associated with that MCC

RETURN VALUE 
  TRUE if there is an entry for the MCC. FALSE otherwise. 
  Also element of the hash table, which is rats selected for this MCC
==============================================================================*/
boolean reg_nv_search_mcc_rats(uint32 mcc, sys_sys_mode_e_type *rat, uint32 length) 
{   
   uint32 index, i;
   boolean ret = FALSE;

   if (mcc == 0)
   {
        return FALSE;
   }
   if (length > REG_NV_MAX_RATS_LENGTH)
   {
       return FALSE;
   }

   for (index = 0; index < NUM_MCC_AND_RATS; ++index)
   {       
       MSG_HIGH_DS_1(REG_SUB, "=REG= REG DB search for mcc 0x%x", reg_nv_mcc_rats_db[index].mcc);

       if(reg_nv_mcc_rats_db[index].mcc == mcc)
       {
           for (i = 0; i < length; ++i)
           {
               rat[i] = reg_nv_mcc_rats_db[index].rat[i];
           }
           MSG_HIGH_DS_1(REG_SUB, "=REG= REG DB found for mcc 0x%x", mcc);
           ret = TRUE;
       }
   }   
   return ret;
}
/*===========================================================================

FUNCTION reg_nv_populate_default_enhanced_hplmn_srch_tbl

DESCRIPTION
  Populate default tbl.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void reg_nv_populate_default_enhanced_hplmn_srch_tbl(byte *nv_config_tbl)
{
#if defined(FEATURE_DISABLE_MCC_FILTERING) && !defined(FEATURE_DISABLE_HPLMN_MCC_FILTERING)
#ifdef FEATURE_ENHANCED_NW_SELECTION
  byte default_tbl[REG_NV_CONFIG_TBL_MAX_SIZE] = {
                                            1, /* Config type is SET type */
                                            1, /* Only One Set present */
                                            10, /* 10 MCC present in the set */
                                                 /* 10 MCC entries */
                                            0x13, 0xF0,
                                            0x13, 0xF1,
                                            0x13, 0xF2,
                                            0x13, 0xF3,
                                            0x13, 0xF4,
                                            0x13, 0xF5,
                                            0x13, 0xF6,
                                            0x03, 0xF2,
                                            0x33, 0xF4,
                                            0x43, 0xF8 
                          };
  
  if(reg_sim_read_ens_flag())
  {
    MSG_HIGH_DS_0(REG_SUB, "=REG= Populating default enhanced hplmn search tbl");
  
    /* Copy to tbl byte stream */
    memscpy((void*)nv_config_tbl,REG_NV_CONFIG_TBL_MAX_SIZE, (void*)default_tbl, sizeof(default_tbl));
  }
#endif
#endif

  return;

}


#if defined (FEATURE_TDSCDMA) || defined (FEATURE_SGLTE)

boolean reg_nv_pseudo_reselection_allowed(void)
{
  return reg_nv_is_pseudo_reselection_allowed;
}
#endif
/*===========================================================================

FUNCTION reg_nv_output_vplmn_list

DESCRIPTION
  Prints the VPLMN

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void reg_nv_output_vplmn_list                
(
  void
)
{
  sys_plmn_id_s_type plmn;

  boolean plmn_id_is_undefined;
  boolean mnc_includes_pcs_digit;
  uint32  mcc;
  uint32  mnc;

  uint8 i;
  if(!reg_nv_vplmn_list)
  {
    MSG_FATAL_DS(REG_SUB, "=REG= reg_nv_vplmn_list is not initialized, and called for display", 0, 0, 0);
  }
  MSG_HIGH_DS_1(REG_SUB, "=REG= VPLMN PLMN list (length = %d)", reg_nv_vplmn_list->vplmn_length);

  if (reg_nv_vplmn_list->vplmn_length> 0)
  {
    for (i = 0; i < reg_nv_vplmn_list->vplmn_length; i++)
    {
        plmn.identity[0] = reg_nv_vplmn_list->vplmn_info[i].plmn.identity[0];
        plmn.identity[1] = reg_nv_vplmn_list->vplmn_info[i].plmn.identity[1];
        plmn.identity[2] = reg_nv_vplmn_list->vplmn_info[i].plmn.identity[2];
 
        sys_plmn_get_mcc_mnc
        (
         plmn,
         &plmn_id_is_undefined,
         &mnc_includes_pcs_digit,
         &mcc,
         &mnc
         );
        if(mnc_includes_pcs_digit)
        {
          MSG_HIGH_DS_3(REG_SUB, "=REG= %2lu %03lu-%03lu", i, mcc, mnc);
          MSG_HIGH_DS_1(REG_SUB, "=REG= RAT= %d", reg_nv_vplmn_list->vplmn_info[i].rat);
        }
        else
        {
          MSG_HIGH_DS_3(REG_SUB, "=REG= %2lu %03lu- %02lu", i, mcc, mnc);
          MSG_HIGH_DS_1(REG_SUB, "=REG= RAT= %d", reg_nv_vplmn_list->vplmn_info[i].rat);
        }
    }
  }

}
void reg_nv_output_eplmn_list
(
  void
)
{
  sys_plmn_id_s_type plmn;

  boolean plmn_id_is_undefined;
  boolean mnc_includes_pcs_digit;
  uint32  mcc;
  uint32  mnc;

  uint8 i;

  MSG_HIGH_DS_1(REG_SUB, "=REG= Equivalent PLMN list (length = %d)", reg_nv_equivalent_plmn_list.length);

  if (reg_nv_equivalent_plmn_list.length > 0)
  {
    plmn.identity[0] = reg_nv_equivalent_plmn_list.rplmn.identity[0];
    plmn.identity[1] = reg_nv_equivalent_plmn_list.rplmn.identity[1];
    plmn.identity[2] = reg_nv_equivalent_plmn_list.rplmn.identity[2];

    sys_plmn_get_mcc_mnc
    (
      plmn,
      &plmn_id_is_undefined,
      &mnc_includes_pcs_digit,
      &mcc,
      &mnc
    );

    if(mnc_includes_pcs_digit)
    {
      MSG_HIGH_DS_2(REG_SUB, "=REG= Equivalent RPLMN(%03lu-%03lu)", mcc, mnc);
    }
    else
    {
      MSG_HIGH_DS_2(REG_SUB, "=REG= Equivalent RPLMN(%03lu- %02lu)", mcc, mnc);
    }

    MSG_HIGH_DS_0(REG_SUB, "=REG=  # MCC-MNC");

    for (i = 0; i < reg_nv_equivalent_plmn_list.length; i++)
    {
      /* get the additional EPLMNs from extended nv list*/
      if(i >= NV_EQUIVALENT_PLMN_LIST_MAX_SIZE)/*after this use new extended list*/
      {
        byte j = i- NV_EQUIVALENT_PLMN_LIST_MAX_SIZE;
        plmn.identity[0] = reg_nv_extended_equivalent_plmn_list.plmn_identity[j][0];
        plmn.identity[1] = reg_nv_extended_equivalent_plmn_list.plmn_identity[j][1];
        plmn.identity[2] = reg_nv_extended_equivalent_plmn_list.plmn_identity[j][2];
      }
      else
      {
        plmn.identity[0] = reg_nv_equivalent_plmn_list.plmn[i].identity[0];
        plmn.identity[1] = reg_nv_equivalent_plmn_list.plmn[i].identity[1];
        plmn.identity[2] = reg_nv_equivalent_plmn_list.plmn[i].identity[2];
      }
      sys_plmn_get_mcc_mnc
      (
        plmn,
        &plmn_id_is_undefined,
        &mnc_includes_pcs_digit,
        &mcc,
        &mnc
      );
      if(mnc_includes_pcs_digit)
      {
        MSG_HIGH_DS_3(REG_SUB, "=REG= %2lu %03lu-%03lu", i, mcc, mnc);
      }
      else
      {
        MSG_HIGH_DS_3(REG_SUB, "=REG= %2lu %03lu- %02lu", i, mcc, mnc);
      }
    }
  }
}
/*==============================================================================

                              FUNCTION DEFINITIONS

==============================================================================*/

#ifdef FEATURE_ENHANCED_NW_SELECTION
/*==============================================================================

FUNCTION NAME

  reg_nv_read_ens_enabled_flag

==============================================================================*/

boolean reg_nv_read_ens_enabled_flag
(
  void
)
{
  /*
  ** Fill NV command to read NV_ENS_ENABLED.
  */
   if (reg_ghdi_nvmem_read(NV_ENS_ENABLED_I,
       (ghdi_nvmem_data_T *)&reg_nv_ens_enabled_flag) != GHDI_SUCCESS)
  {
    MSG_HIGH_DS_0(REG_SUB, "=REG= Reading NV_ENS_ENABLED FLAG failed");

    reg_nv_ens_enabled_flag = FALSE;
  }
  /*
  ** NV_ENS_ENABLED was successfully read from NVRAM.
  */
  else
  {
    MSG_HIGH_DS_1(REG_SUB, "=REG= Read NV_ENS_ENABLED FLAG  - %d", reg_nv_ens_enabled_flag);
  }
  
  return reg_nv_ens_enabled_flag;
}
#endif

/*==============================================================================

FUNCTION NAME

  reg_nv_equivalent_plmn_list_get

==============================================================================*/
nv_equivalent_plmn_list_type reg_nv_equivalent_plmn_list_get
(
  void
)
{
  return reg_nv_equivalent_plmn_list;
}
/*==============================================================================

FUNCTION NAME

  reg_nv_get_vplmn_list_length              

==============================================================================*/
int16 reg_nv_get_vplmn_list_length
(
  void
)
{
  if(reg_nv_vplmn_list)
  {
    return reg_nv_vplmn_list->vplmn_length;
  }
  else
  {
    return -1;
  }
}
/*============================================================================== 

FUNCTION NAME

  reg_nv_write_vplmn_list

==============================================================================*/
mcfg_fs_status_e_type reg_nv_write_vplmn_list
(
  void
)
{
  mcfg_fs_status_e_type status = MCFG_FS_STATUS_EFS_ERR;
  uint32 data_size = (uint32)sizeof(reg_nv_vplmn_list_s_type);
  
  if(!reg_nv_vplmn_list)
  {
    MSG_FATAL_DS(REG_SUB, "=REG= Global Structure is not allocated memory during power up", 0, 0, 0);
  }

  status = mcfg_fs_write("/nv/item_files/modem/nas/vpmln",reg_nv_vplmn_list,data_size,MCFG_FS_O_RDWR | MCFG_FS_O_CREAT | MCFG_FS_O_AUTODIR, MCFG_FS_ALLPERMS,MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)reg_as_id);


  
  MSG_HIGH_DS_1(REG_SUB, "=REG= VPLMN: Write to VPLMN list status = %d", status);

  return status;
}
/*============================================================================== 

FUNCTION NAME

  reg_nv_read_vplmn_list

==============================================================================*/
mcfg_fs_status_e_type reg_nv_read_vplmn_list
(
  void
)
{
  uint32 data_size;
  mcfg_fs_status_e_type status = MCFG_FS_STATUS_EFS_ERR;
  data_size = (uint32)sizeof(reg_nv_vplmn_list_s_type);
  if(!reg_nv_vplmn_list)
  {

    reg_nv_vplmn_list = (reg_nv_vplmn_list_s_type *) modem_mem_calloc(1,data_size, MODEM_MEM_CLIENT_NAS);
    if(!reg_nv_vplmn_list )
    {
       MSG_FATAL_DS(REG_SUB, "=REG= Could not allocate the memory", 0, 0, 0);
    }

    memset((void*)reg_nv_vplmn_list, 0, data_size);

    status = mcfg_fs_read("/nv/item_files/modem/nas/vpmln", reg_nv_vplmn_list, data_size, MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)reg_as_id);

    if(status == MCFG_FS_STATUS_OK)
    {
       MSG_HIGH_DS_0(REG_SUB, "=REG= VPLMN: Reading VPLMN list from NV, updating the Global");
       reg_nv_output_vplmn_list();
    }
    else
    {
       MSG_HIGH_DS_1(REG_SUB, "=REG= VPLMN: Reading VPLMN list from NV failed status = %d", status);
       reg_nv_vplmn_list->vplmn_length = 0;
    }

  }
  return status;
}
/*==============================================================================

FUNCTION NAME

  reg_nv_sys_equivalent_plmn_list_get

==============================================================================*/
void reg_nv_sys_equivalent_plmn_list_get
(
  sys_plmn_list_s_type* eplmn_list_p
)
{
  uint32 i;

  eplmn_list_p->length = MIN(reg_nv_equivalent_plmn_list.length,
                             NV_EQUIVALENT_PLMN_LIST_MAX_SIZE
                           + NV_EXTENDED_EQUIVALENT_PLMN_LIST_MAX_SIZE);

  eplmn_list_p->length = MIN(eplmn_list_p->length,
                               SYS_PLMN_LIST_MAX_LENGTH);

  for (i = 0; i < eplmn_list_p->length; i++)
  {
    /*get additional EPLMNs from the extended list*/
    if(i >= NV_EQUIVALENT_PLMN_LIST_MAX_SIZE)/*after this use new extended list*/
    {
      byte j = (byte)MIN(NV_EXTENDED_EQUIVALENT_PLMN_LIST_MAX_SIZE-1,(i- NV_EQUIVALENT_PLMN_LIST_MAX_SIZE));
      eplmn_list_p->plmn[i].identity[0] = reg_nv_extended_equivalent_plmn_list.plmn_identity[j][0];
      eplmn_list_p->plmn[i].identity[1] = reg_nv_extended_equivalent_plmn_list.plmn_identity[j][1];
      eplmn_list_p->plmn[i].identity[2] = reg_nv_extended_equivalent_plmn_list.plmn_identity[j][2];
    }
    else
    {
      eplmn_list_p->plmn[i].identity[0] = reg_nv_equivalent_plmn_list.plmn[i].identity[0];
      eplmn_list_p->plmn[i].identity[1] = reg_nv_equivalent_plmn_list.plmn[i].identity[1];
      eplmn_list_p->plmn[i].identity[2] = reg_nv_equivalent_plmn_list.plmn[i].identity[2];
    }
  } 

  return;
}

/*==============================================================================

FUNCTION NAME

  reg_nv_read_equivalent_plmn_list

==============================================================================*/

void reg_nv_read_equivalent_plmn_list
(
  sys_plmn_id_s_type*   rplmn_p,
  sys_plmn_list_s_type* eplmn_list_p
)
{
  uint8 i;

  MSG_HIGH_DS_0(REG_SUB, "=REG= Read equivalent PLMN list from cache");
    
  reg_nv_output_eplmn_list();
  
  rplmn_p->identity[0] = reg_nv_equivalent_plmn_list.rplmn.identity[0];
  rplmn_p->identity[1] = reg_nv_equivalent_plmn_list.rplmn.identity[1];
  rplmn_p->identity[2] = reg_nv_equivalent_plmn_list.rplmn.identity[2];
  
    eplmn_list_p->length = MIN(reg_nv_equivalent_plmn_list.length,
                             NV_EQUIVALENT_PLMN_LIST_MAX_SIZE
                             + NV_EXTENDED_EQUIVALENT_PLMN_LIST_MAX_SIZE);
  eplmn_list_p->length = MIN(eplmn_list_p->length,
                             SYS_PLMN_LIST_MAX_LENGTH);
  
  for (i = 0; i < eplmn_list_p->length; i++)
  {
     /*get additional EPLMNs from the extended list*/
      if(i >= NV_EQUIVALENT_PLMN_LIST_MAX_SIZE)/*after this use new extended list*/
      {
        byte j = (byte)MIN(NV_EXTENDED_EQUIVALENT_PLMN_LIST_MAX_SIZE-1, i- NV_EQUIVALENT_PLMN_LIST_MAX_SIZE);
        eplmn_list_p->plmn[i].identity[0] = reg_nv_extended_equivalent_plmn_list.plmn_identity[j][0];
        eplmn_list_p->plmn[i].identity[1] = reg_nv_extended_equivalent_plmn_list.plmn_identity[j][1];
        eplmn_list_p->plmn[i].identity[2] = reg_nv_extended_equivalent_plmn_list.plmn_identity[j][2];
      }
      else 
      {
        eplmn_list_p->plmn[i].identity[0] = reg_nv_equivalent_plmn_list.plmn[i].identity[0];
        eplmn_list_p->plmn[i].identity[1] = reg_nv_equivalent_plmn_list.plmn[i].identity[1];
        eplmn_list_p->plmn[i].identity[2] = reg_nv_equivalent_plmn_list.plmn[i].identity[2];
      }
  }

}


/*==============================================================================

FUNCTION NAME

  reg_nv_write_equivalent_plmn_list

==============================================================================*/

void reg_nv_write_equivalent_plmn_list
(
        sys_plmn_id_s_type    rplmn,
  const sys_plmn_list_s_type* eplmn_list_p
)
{
  uint8 i;

  /*
  ** Copy the RPLMN into the static variable to be written to NVRAM.
  */
  reg_nv_equivalent_plmn_list.rplmn.identity[0] = rplmn.identity[0];
  reg_nv_equivalent_plmn_list.rplmn.identity[1] = rplmn.identity[1];
  reg_nv_equivalent_plmn_list.rplmn.identity[2] = rplmn.identity[2];

  /*
  ** Copy the equivalent PLMN list into the static variable
  ** to be written to NVRAM.
  */
  reg_nv_equivalent_plmn_list.length = NV_EQUIVALENT_PLMN_LIST_MAX_SIZE
                                     + NV_EXTENDED_EQUIVALENT_PLMN_LIST_MAX_SIZE;

  if ((uint8)SYS_PLMN_LIST_MAX_LENGTH < reg_nv_equivalent_plmn_list.length)
  {
    reg_nv_equivalent_plmn_list.length = (uint8)SYS_PLMN_LIST_MAX_LENGTH;
  }

  if (eplmn_list_p->length < reg_nv_equivalent_plmn_list.length)
  {
    reg_nv_equivalent_plmn_list.length = (uint8)eplmn_list_p->length;
  }

  for (i = 0; i < reg_nv_equivalent_plmn_list.length; i++)
  {
    /*get additional EPLMNs from the extended list*/
    if(i >= NV_EQUIVALENT_PLMN_LIST_MAX_SIZE)/*after this use new extended list*/
    {
      uint8 j = i- NV_EQUIVALENT_PLMN_LIST_MAX_SIZE;
      reg_nv_extended_equivalent_plmn_list.plmn_identity[j][0] = eplmn_list_p->plmn[i].identity[0];
      reg_nv_extended_equivalent_plmn_list.plmn_identity[j][1] = eplmn_list_p->plmn[i].identity[1];
      reg_nv_extended_equivalent_plmn_list.plmn_identity[j][2] = eplmn_list_p->plmn[i].identity[2];
    }
    else
    {
      reg_nv_equivalent_plmn_list.plmn[i].identity[0] = eplmn_list_p->plmn[i].identity[0];
      reg_nv_equivalent_plmn_list.plmn[i].identity[1] = eplmn_list_p->plmn[i].identity[1];
      reg_nv_equivalent_plmn_list.plmn[i].identity[2] = eplmn_list_p->plmn[i].identity[2];
    }
  }

  /*
  ** Undefine the unused PLMN ids contained within the equivalent
  ** PLMN list to make it easier to verify that the NV item has
  ** been properly updated.
  */
  for (i = reg_nv_equivalent_plmn_list.length; i < NV_EQUIVALENT_PLMN_LIST_MAX_SIZE + NV_EXTENDED_EQUIVALENT_PLMN_LIST_MAX_SIZE; i++)
  {
    /*get additional EPLMNs from the extended list*/
    if(i >= NV_EQUIVALENT_PLMN_LIST_MAX_SIZE)/*after this use new extended list*/
    {
      uint8 j = i- NV_EQUIVALENT_PLMN_LIST_MAX_SIZE;
      reg_nv_extended_equivalent_plmn_list.plmn_identity[j][0] = 0xFF;
      reg_nv_extended_equivalent_plmn_list.plmn_identity[j][1] = 0xFF;
      reg_nv_extended_equivalent_plmn_list.plmn_identity[j][2] = 0xFF;
    }
    else
    {
      reg_nv_equivalent_plmn_list.plmn[i].identity[0] = 0xFF;
      reg_nv_equivalent_plmn_list.plmn[i].identity[1] = 0xFF;
      reg_nv_equivalent_plmn_list.plmn[i].identity[2] = 0xFF;
    }
  }

  reg_nv_data_valid = TRUE;

  reg_nv_output_eplmn_list();

  reg_log_umts_nas_eplmn_list
  (
    rplmn,
    eplmn_list_p
  );
}


/*==============================================================================

FUNCTION NAME

  reg_nv_read_rplmnact

==============================================================================*/

void reg_nv_read_rplmnact
(
  nv_rplmnact_type* rplmnact_p
)
{
  MSG_HIGH_DS_2(REG_SUB, "=REG= Read RPLMNACT %d %d from cache", reg_nv_rplmnact.act[0], reg_nv_rplmnact.act[1]);
  rplmnact_p->act[0] = reg_nv_rplmnact.act[0];
  rplmnact_p->act[1] = reg_nv_rplmnact.act[1];
}


/*==============================================================================

FUNCTION NAME

  reg_nv_write_rplmnact

==============================================================================*/

void reg_nv_write_rplmnact
(
  nv_rplmnact_type rplmnact
)
{
  reg_nv_data_valid = TRUE;
  reg_nv_rplmnact = rplmnact;
  
  MSG_HIGH_DS_2(REG_SUB, "=REG= Save RPLMNACT %d %d in cache", reg_nv_rplmnact.act[0], reg_nv_rplmnact.act[1]);
}

/*==============================================================================

FUNCTION NAME
  reg_nv_read_override_roam_status

==============================================================================*/
boolean reg_nv_read_override_roam_status
(
#ifdef FEATURE_DUAL_SIM
  sys_modem_as_id_e_type reg_as_id_temp
#else
  void
#endif
)
{
#ifdef FEATURE_DUAL_SIM
  return reg_nv_override_roam_status_sim[reg_as_id_temp];
#else
  return reg_nv_override_roam_status;
#endif
}

/*==============================================================================

FUNCTION NAME

  reg_nv_write_override_roam_status

==============================================================================*/
void reg_nv_write_override_roam_status
(
  boolean override_roam_status_flag
#ifdef FEATURE_DUAL_SIM
  ,sys_modem_as_id_e_type reg_as_id_temp
#endif
)
{
#ifdef FEATURE_DUAL_SIM
  reg_nv_override_roam_status_sim[reg_as_id_temp] = override_roam_status_flag;
#else
  reg_nv_override_roam_status = override_roam_status_flag;
#endif
}
#ifdef DISABLE_9x05_OPTIMIZATIONS

/*==============================================================================

FUNCTION NAME

  reg_nv_read_data_roaming

==============================================================================*/
sys_data_roaming_e_type  reg_nv_read_data_roaming
(
  void
)
{
  return reg_nv_data_romaing;
}

/*==============================================================================

FUNCTION NAME

  reg_nv_write_data_roaming

==============================================================================*/
boolean  reg_nv_write_data_roaming
(
  sys_data_roaming_e_type data_roaming
)
{
  if(reg_nv_data_romaing != data_roaming)
  {
    MSG_HIGH_DS_2(REG_SUB, "=REG= Data roaming indication change from %d to %d",
                reg_nv_data_romaing, data_roaming);
    reg_nv_data_romaing = data_roaming;
    return TRUE;
  }
  return FALSE;
}
#endif

#if defined(FEATURE_FEMTO_CSG) || defined (FEATURE_NAS_ECALL)
/*==============================================================================

FUNCTION NAME

  reg_nv_read_rplmn

==============================================================================*/

sys_plmn_id_s_type reg_nv_read_rplmn
(
  void
)
{
  MSG_HIGH_DS_3(REG_SUB, "=REG= Read RPLMN  %x %x %x from cache",  
                reg_nv_rplmn.identity[0], 
                reg_nv_rplmn.identity[1], 
                reg_nv_rplmn.identity[2]);
  return reg_nv_rplmn;
}


/*==============================================================================

FUNCTION NAME

  reg_nv_write_rplmn

==============================================================================*/

void reg_nv_write_rplmn
(
  sys_plmn_id_s_type  rplmn
)
{
    reg_nv_rplmn = rplmn;
}

/*==============================================================================

FUNCTION NAME

  reg_nv_write_rplmn_dup

==============================================================================*/

void reg_nv_write_rplmn_dup
(
  boolean  rplmn_dup
)
{
  reg_nv_rplmn_dup = rplmn_dup;
}

/*==============================================================================

FUNCTION NAME

  reg_nv_read_rplmn_dup

==============================================================================*/

boolean reg_nv_read_rplmn_dup
(
  void
)
{
  return reg_nv_rplmn_dup;
}

#ifdef FEATURE_FEMTO_CSG
/*==============================================================================

FUNCTION NAME

  reg_nv_read_csg_support

==============================================================================*/

sys_csg_support_e_type reg_nv_read_csg_support
(
  void
)
{
  return reg_nv_csg_support;
}

#endif

#endif 

/*==============================================================================

FUNCTION NAME

  reg_nv_read_psloci

==============================================================================*/

boolean reg_nv_read_psloci
(
  byte* psloci_p
)
{
  boolean status = FALSE;

  /*
  ** PSLOCI could not be read from NVRAM.
  */
   if (reg_ghdi_nvmem_read(NV_GSM_LOCIGPRS_I,
       (ghdi_nvmem_data_T *)psloci_p) != GHDI_SUCCESS)
  {
    MSG_HIGH_DS_0(REG_SUB, "=REG= Reading PS LOCI failed");
  }
  /*
  ** PSLOCI was successfully read from NVRAM.
  */
  else
  {
    MSG_HIGH_DS_0(REG_SUB, "=REG= Read PS LOCI");

   /* psloci_p[0]  = reg_nv_item.gsm_locigprs[0];
    psloci_p[1]  = reg_nv_item.gsm_locigprs[1];
    psloci_p[2]  = reg_nv_item.gsm_locigprs[2];
    psloci_p[3]  = reg_nv_item.gsm_locigprs[3];
    psloci_p[4]  = reg_nv_item.gsm_locigprs[4];
    psloci_p[5]  = reg_nv_item.gsm_locigprs[5];
    psloci_p[6]  = reg_nv_item.gsm_locigprs[6];
    psloci_p[7]  = reg_nv_item.gsm_locigprs[7];
    psloci_p[8]  = reg_nv_item.gsm_locigprs[8];
    psloci_p[9]  = reg_nv_item.gsm_locigprs[9];
    psloci_p[10] = reg_nv_item.gsm_locigprs[10];
    psloci_p[11] = reg_nv_item.gsm_locigprs[11];
    psloci_p[12] = reg_nv_item.gsm_locigprs[12];
    psloci_p[13] = reg_nv_item.gsm_locigprs[13]; */

    status = TRUE;
  }

  return status;
}


/*==============================================================================

FUNCTION NAME

  reg_nv_write_cache

==============================================================================*/

boolean reg_nv_write_cache
(
  void
)
{
  boolean result = TRUE;
  mcfg_fs_status_e_type status = MCFG_FS_STATUS_EFS_ERR;
  if(!reg_nv_data_valid)
  {
    MSG_MED_DS_0(REG_SUB, "=REG= Trying to write NV with uninitialized data - blocked");
    return FALSE;
  }
  /*
  ** Fill NV command to write RPLMNACT.
  */
  
  if (reg_ghdi_nvmem_write(NV_RPLMNACT_I, (ghdi_nvmem_data_T *)&reg_nv_rplmnact) != GHDI_SUCCESS)
  {
    MSG_ERROR_DS_0(REG_SUB, "=REG= Write RPLMNACT failed");
    result = FALSE;
  }
  else
  {
    MSG_HIGH_DS_2(REG_SUB, "=REG= Wrote RPLMNACT %d %d", reg_nv_rplmnact.act[0], reg_nv_rplmnact.act[1]);
    if(reg_nv_psm_cache_ptr != NULL)
    {
      REG_SET_BM_32BIT(reg_nv_psm_cache_ptr->psm_nv_list_bitmask, REG_PSM_NV_RPLMNACT);
    }
  }

  /*
  ** Fill NV command to write the equivalent PLMN list.
  */
  
  if (reg_ghdi_nvmem_write(NV_EQUIVALENT_PLMN_LIST_I, (ghdi_nvmem_data_T *)&reg_nv_equivalent_plmn_list) != GHDI_SUCCESS)
  {
    MSG_ERROR_DS_0(REG_SUB, "=REG= Write equivalent PLMN list failed");
    result = FALSE;
  }
  else
  {
    reg_nv_data_valid = FALSE;

    if(reg_nv_psm_cache_ptr != NULL)
    {
      REG_SET_BM_32BIT(reg_nv_psm_cache_ptr->psm_nv_list_bitmask, REG_PSM_NV_EQUIVALENT_PLMN_LIST);
    }
    /*
    ** Fill NV command to write the extended equivalent PLMN list.
    */
    if (reg_ghdi_nvmem_write(NV_EXTENDED_EQUIVALENT_PLMN_LIST_I, (ghdi_nvmem_data_T *)&reg_nv_extended_equivalent_plmn_list) != GHDI_SUCCESS)
    {
      MSG_ERROR_DS_0(REG_SUB, "=REG= Wrote equivalent PLMN list and extended equivalent PLMN list failed");
      result = FALSE;
    }
    else
    {
      MSG_HIGH_DS_0(REG_SUB, "=REG= Wrote equivalent PLMN list and Wrote extended equivalent PLMN list");
      reg_nv_data_valid = FALSE;
      if(reg_nv_psm_cache_ptr != NULL)
      {
        REG_SET_BM_32BIT(reg_nv_psm_cache_ptr->psm_nv_list_bitmask, REG_PSM_NV_EXTENDED_EQUIVALENT_PLMN_LIST);
      }
    }
  }
  
  #ifdef FEATURE_TDSCDMA
  /* Write HPLMN IRAT Search timer */
  status = mcfg_fs_write("/nv/item_files/modem/nas/irat_search_timer",&reg_nv_hplmn_irat_search_timer,sizeof(uint32), MCFG_FS_O_CREAT | MCFG_FS_O_AUTODIR, MCFG_FS_ALLPERMS,MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)REG_AS_ID_1) ;
  if(status != MCFG_FS_STATUS_OK)
  {
    MSG_ERROR_DS_1(REG_SUB, "=REG= Wrote HPLMN IRAT search timer to NV failed status = %d", status);
    result = FALSE;
  }
  else
  {  
    MSG_HIGH_DS_0(REG_SUB, "=REG=  Wrote HPLMN IRAT search timer to NV");
    if(reg_nv_psm_cache_ptr != NULL)
    {
      REG_SET_BM_32BIT(reg_nv_psm_cache_ptr->psm_nv_list_bitmask, REG_PSM_EFS_IRAT_SEARCH_TIMER);
    }
  }
#endif 

#if defined(FEATURE_FEMTO_CSG) || defined (FEATURE_NAS_ECALL)
  if(reg_nv_rplmn_dup == FALSE)
  {
    sys_plmn_undefine_plmn_id(&reg_nv_rplmn);
  }
  /* Write RPLMN to NV */
  status = mcfg_fs_write("/nv/item_files/modem/nas/rplmn",&reg_nv_rplmn,sizeof(sys_plmn_id_s_type), MCFG_FS_O_CREAT | MCFG_FS_O_AUTODIR, MCFG_FS_ALLPERMS,MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)reg_as_id);
  if( status != MCFG_FS_STATUS_OK)
  {
    MSG_ERROR_DS_1(REG_SUB, "=REG= Wrote RPLMN to NV failed status = %d", status);
    result = FALSE;
  }
  else
  {  
    MSG_HIGH_DS_3(REG_SUB, "=REG=  Wrote RPLMN to NV %d %d %d", 
                  reg_nv_rplmn.identity[0], 
                  reg_nv_rplmn.identity[1], 
                  reg_nv_rplmn.identity[2]);
    if(reg_nv_psm_cache_ptr != NULL)
    {
      REG_SET_BM_32BIT(reg_nv_psm_cache_ptr->psm_nv_list_bitmask, REG_PSM_EFS_RPLMN);
    }
  }
#endif   /*FEATURE_FEMTO_CSG*/
#ifdef DISABLE_9x05_OPTIMIZATIONS
  status = mcfg_fs_write("/nv/item_files/modem/nas/data_roam_status",&reg_nv_data_romaing,sizeof(sys_data_roaming_e_type), MCFG_FS_O_CREAT | MCFG_FS_O_AUTODIR, MCFG_FS_ALLPERMS,MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)reg_sub_id);
  if(status != MCFG_FS_STATUS_OK)
  {
    MSG_ERROR_DS_0(REG_SUB, "=REG= Write reg_nv_data_romaing to NV failed");
    result = FALSE;
  }
  else
  {  
    MSG_HIGH_DS_1(REG_SUB, "=REG=  Wrote reg_nv_data_romaing to NV %d",reg_nv_data_romaing);
    if(reg_nv_psm_cache_ptr != NULL)
    {
      REG_SET_BM_32BIT(reg_nv_psm_cache_ptr->psm_nv_list_bitmask, REG_PSM_EFS_DATA_ROAM_STATUS);
    }
  }
#endif

  return result;
}


/*==============================================================================

FUNCTION NAME

  reg_nv_read_imsi

==============================================================================*/

boolean reg_nv_read_imsi
(
  reg_nv_imsi_s_type* imsi_p
)
{
  boolean status = FALSE;

//  byte i;

   if (reg_ghdi_nvmem_read(NV_GSMUMTS_IMSI_I,
       (ghdi_nvmem_data_T *)&imsi_p->digits) != GHDI_SUCCESS)
  {
    MSG_HIGH_DS_0(REG_SUB, "=REG= Reading IMSI failed");
  }
  /*
  ** IMSI was successfully read from NVRAM.
  */
  else
  {
    /*for (i = 0; i < REG_NV_IMSI_MAX_LENGTH; i++)
    {
      imsi_p->digits[i] = reg_nv_item.gsmumts_imsi[i];
    }*/
    MSG_HIGH_DS_8(REG_SUB, "=REG= IMSI[0] = 0x%02x IMSI[1] = 0x%02x IMSI[2] = 0x%02x  IMSI[3] = 0x%02x \
                  IMSI[4] = 0x%02x IMSI[5] = 0x%02x IMSI[6] = 0x%02x IMSI[7] = 0x%02x ",imsi_p->digits[0],
                  imsi_p->digits[1],imsi_p->digits[2],imsi_p->digits[3],imsi_p->digits[4],imsi_p->digits[5],
                  imsi_p->digits[6],imsi_p->digits[7]);
    status = TRUE;
  }

  return status;
}

/*==============================================================================

FUNCTION NAME

  reg_nv_read_preferred_plmn_list

==============================================================================*/

void reg_nv_read_preferred_plmn_list()
{
  byte *buffer;  
  uint8 length;
  mcfg_fs_status_e_type status = MCFG_FS_STATUS_EFS_ERR;
  reg_nv_preferred_plmn_list = NULL; 
  reg_nv_pref_plmn_list_length = 0;

  buffer = (byte*) modem_mem_alloc(REG_NV_MAX_PREF_PLMN_LIST *5+1, MODEM_MEM_CLIENT_NAS);
  reg_check_for_null_ptr((void*)buffer );  

  reg_timers_pet_watchdog();
  status = mcfg_fs_read("/nv/item_files/modem/nas/preferred_plmn_list", &buffer[0], (REG_NV_MAX_PREF_PLMN_LIST *5)+1, MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)REG_AS_ID_1);

  if(status == MCFG_FS_STATUS_OK )
  {  
    length = buffer[0];
  
    if(length > REG_NV_MAX_PREF_PLMN_LIST)
    {
      length = REG_NV_MAX_PREF_PLMN_LIST;
    }
  
    if(length > 0)
    {
      reg_nv_preferred_plmn_list = (reg_sim_plmn_w_act_s_type*)modem_mem_alloc(length*5, MODEM_MEM_CLIENT_NAS);
      reg_check_for_null_ptr((void*)reg_nv_preferred_plmn_list);  
      memscpy( reg_nv_preferred_plmn_list, length*5, &buffer[1], length*5);
      reg_nv_pref_plmn_list_length = length;
    }
  }
  modem_mem_free(buffer, MODEM_MEM_CLIENT_NAS);
}

/*==============================================================================

FUNCTION NAME

  reg_nv_get_preferred_plmn_list

==============================================================================*/
reg_sim_plmn_w_act_s_type  * reg_nv_get_preferred_plmn_list(int32 *preferred_plmn_length)
{

  boolean plmn_id_is_undefined;
  boolean mnc_includes_pcs_digit;
  uint32  mcc;
  uint32  mnc;
  int i;
 
  if(reg_nv_preferred_plmn_list == NULL)
  {
    *preferred_plmn_length = 0;    
     return NULL;
  }
  if(reg_mode_is_usim_compliant())
  {
    *preferred_plmn_length = reg_nv_pref_plmn_list_length;
     MSG_HIGH_DS_1(REG_SUB, "=REG= NV Preferred PLMN list (length = %d)", reg_nv_pref_plmn_list_length);

     MSG_HIGH_DS_0(REG_SUB, "=REG=  # MCC-MNC  RAT   CAT");
     for (i = 0; i < reg_nv_pref_plmn_list_length; i++)
     {
       //SKIP the undefined plmn
       if(sys_plmn_id_is_undefined(reg_nv_preferred_plmn_list[i].plmn))
       {
         continue;
       }
       sys_plmn_get_mcc_mnc
       (
         reg_nv_preferred_plmn_list[i].plmn,
         &plmn_id_is_undefined,
         &mnc_includes_pcs_digit,
         &mcc,
         &mnc
       ); 

       if((reg_nv_preferred_plmn_list[i].act[0] & 0x80)  &&
           (reg_nv_preferred_plmn_list[i].act[1] == 0x00 ))
       {
         if(mnc_includes_pcs_digit)
         {
           MSG_HIGH_DS_3(REG_SUB, "=REG= %2lu %03lu-%03lu  UMTS PPLMN", i, mcc, mnc);
         }
         else
         {
           MSG_HIGH_DS_3(REG_SUB, "=REG= %2lu %03lu- %02lu  UMTS PPLMN", i, mcc, mnc);
         }
       }
       else if((reg_nv_preferred_plmn_list[i].act[0] == 0x00) &&
           (reg_nv_preferred_plmn_list[i].act[1] & 0x80 ))
       {
         if(mnc_includes_pcs_digit)
         {
           MSG_HIGH_DS_3(REG_SUB, "=REG= %2lu %03lu-%03lu  GSM PPLMN", i, mcc, mnc);
         }
         else
         {
           MSG_HIGH_DS_3(REG_SUB, "=REG= %2lu %03lu- %02lu  GSM PPLMN", i, mcc, mnc);
         }
       }
       else if((reg_nv_preferred_plmn_list[i].act[0] & 0x80) &&
               (reg_nv_preferred_plmn_list[i].act[1] & 0x80 ))
       {
         if(mnc_includes_pcs_digit)
         {
           MSG_HIGH_DS_3(REG_SUB, "=REG= %2lu %03lu-%03lu  UMTS-GSM PPLMN", i, mcc, mnc);
         }
         else
         {
           MSG_HIGH_DS_3(REG_SUB, "=REG= %2lu %03lu- %02lu  UMTS-GSM PPLMN", i, mcc, mnc);
         }
       }
       else if((reg_nv_preferred_plmn_list[i].act[0] == 0x00) &&
               (reg_nv_preferred_plmn_list[i].act[1] == 0x00 ))
       {
         if(mnc_includes_pcs_digit)
         {
#ifdef FEATURE_LTE
           MSG_HIGH_DS_3(REG_SUB, "=REG= %2lu %03lu-%03lu  UMTS-LTE-GSM PPLMN", i, mcc, mnc);
#else
           MSG_HIGH_DS_3(REG_SUB, "=REG= %2lu %03lu-%03lu  UMTS-GSM PPLMN", i, mcc, mnc);
#endif
         }
         else
         {
#ifdef FEATURE_LTE
           MSG_HIGH_DS_3(REG_SUB, "=REG= %2lu %03lu- %02lu  UMTS-LTE-GSM PPLMN", i, mcc, mnc);
#else
           MSG_HIGH_DS_3(REG_SUB, "=REG= %2lu %03lu- %02lu  UMTS-GSM PPLMN", i, mcc, mnc);
#endif

         }
       }
#ifdef FEATURE_LTE
       else if((reg_nv_preferred_plmn_list[i].act[0] & 0x40) &&
           (reg_nv_preferred_plmn_list[i].act[1] == 0x00 ))
       {
         if(mnc_includes_pcs_digit)
         {
           MSG_HIGH_DS_3(REG_SUB, "=REG= %2lu %03lu-%03lu  LTE PPLMN", i, mcc, mnc);
         }
         else
         {
           MSG_HIGH_DS_3(REG_SUB, "=REG= %2lu %03lu- %02lu  LTE PPLMN", i, mcc, mnc);
         }
       }

       else if((reg_nv_preferred_plmn_list[i].act[0] & 0x40) &&
               (reg_nv_preferred_plmn_list[i].act[1] & 0x80 ))
       {
         if(mnc_includes_pcs_digit)
         {
           MSG_HIGH_DS_3(REG_SUB, "=REG= %2lu %03lu-%03lu  LTE-GSM PPLMN", i, mcc, mnc);
         }
         else
         {
           MSG_HIGH_DS_3(REG_SUB, "=REG= %2lu %03lu- %02lu  LTE-GSM PPLMN", i, mcc, mnc);
         }
       }
       else if((reg_nv_preferred_plmn_list[i].act[0] & 0xc0) &&
               (reg_nv_preferred_plmn_list[i].act[1] == 0x00 ))
       {
         if(mnc_includes_pcs_digit)
         {
           MSG_HIGH_DS_3(REG_SUB, "=REG= %2lu %03lu-%03lu  UMTS-LTE PPLMN", i, mcc, mnc);
         }
         else
         {
           MSG_HIGH_DS_3(REG_SUB, "=REG= %2lu %03lu- %02lu  UMTS-LTE PPLMN", i, mcc, mnc);
         }
       }

       else if((reg_nv_preferred_plmn_list[i].act[0] & 0xc0) &&
               (reg_nv_preferred_plmn_list[i].act[1] & 0x80 ))
       {
         if(mnc_includes_pcs_digit)
         {
            MSG_HIGH_DS_3(REG_SUB, "=REG= %2lu %03lu-%03lu  UMTS-LTE-GSM PPLMN", i, mcc, mnc);
         }
         else
         {
            MSG_HIGH_DS_3(REG_SUB, "=REG= %2lu %03lu- %02lu  UMTS-LTE-GSM PPLMN", i, mcc, mnc);
         }
       }
   #endif
     }
     return reg_nv_preferred_plmn_list;
  }
  else
  {
    *preferred_plmn_length = 0;    
     return NULL;
  }  
}


/*==============================================================================

FUNCTION NAME

  reg_nv_initialize_cache

==============================================================================*/

void reg_nv_initialize_cache(void)
{
  mcfg_fs_status_e_type status = MCFG_FS_STATUS_EFS_ERR;
  int status_new = 1;
  uint16 wcdma_freq_lock_item = 0;                 

  reg_task_sim_available = TRUE;
  reg_task_power_up_init = TRUE;

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE 
  for ( reg_as_id=REG_AS_ID_1; reg_as_id < (reg_as_id_e_type)MAX_NAS_STACKS; reg_as_id++ )
#endif
  {
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE 
    reg_sub_id = reg_sub_id_stack[reg_as_id];
#endif

    reg_nv_read_psm_config();
  
    /*
    ** RPLMNACT could not be read from NVRAM.
    */
   if (reg_ghdi_nvmem_read(NV_RPLMNACT_I,
       (ghdi_nvmem_data_T *)&reg_nv_rplmnact) != GHDI_SUCCESS)
    {  
      MSG_HIGH_DS_0(REG_SUB, "=REG= Reading RPLMNACT failed");
  
      reg_nv_rplmnact.act[0] = 0;
      reg_nv_rplmnact.act[1] = 0;
      reg_nv_psm_update_cache_bitmask(reg_nv_psm_nv_list_bitmask,
                                      REG_PSM_NV_RPLMNACT);
    }
    /*
    ** RPLMNACT was successfully read from NVRAM.
    */
    else
    {
      MSG_HIGH_DS_2(REG_SUB, "=REG= Read RPLMNACT %d %d", reg_nv_rplmnact.act[0], reg_nv_rplmnact.act[1]);
      reg_nv_data_valid = TRUE;
    }
  }

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE 
  reg_as_id = REG_AS_ID_1;
#endif

#if defined (FEATURE_TDSCDMA) || defined (FEATURE_SGLTE)
  reg_nv_read_lte_cell_lists();
  reg_state_start_backoff_lte_search_timer_count(lte_non_available_list_ptr);
#endif

  status = mcfg_fs_read("/nv/item_files/wcdma/rrc/wcdma_rrc_freq_lock_item", &wcdma_freq_lock_item, sizeof(uint16), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)REG_AS_ID_1);

  MSG_HIGH_DS_2(REG_SUB, "=REG= NV wcdma_freq_lock_item read from EFS  with status %d and value is %d",
                status,
                wcdma_freq_lock_item);

  if ((status == MCFG_FS_STATUS_OK )&& (wcdma_freq_lock_item !=0))
  {
    reg_nv_wcdma_freq_lock_enabled = TRUE;
  }
  status = mcfg_fs_read("/nv/item_files/modem/nas/lpm_power_off", &reg_nv_lpm_power_off, sizeof(boolean), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)REG_AS_ID_1);

  if (status != MCFG_FS_STATUS_OK)
  {
    reg_nv_lpm_power_off = TRUE;
  }
  MSG_HIGH_DS_2(REG_SUB, "=REG=  NV write EFS/NV at LPM read and value=%d status = %d",reg_nv_lpm_power_off,status);
  reg_timers_pet_watchdog();
  status = mcfg_fs_read("/nv/item_files/modem/nas/efrplmnsi_select_rplmn_after_hplmn", &reg_nv_efrplmnsi_selection, sizeof(boolean), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)REG_AS_ID_1);

  if (status != MCFG_FS_STATUS_OK)
  {
    reg_nv_efrplmnsi_selection = FALSE;
  }
  MSG_HIGH_DS_2(REG_SUB, "=REG=  NV read EFRPLMNSI selection =%d status = %d",reg_nv_efrplmnsi_selection,status);

#if defined(FEATURE_FEMTO_CSG) || defined (FEATURE_NAS_ECALL)
#ifdef FEATURE_DUAL_SIM
  sys_plmn_undefine_plmn_id(&reg_nv_rplmn_sim[REG_AS_ID_1]);
  reg_nv_rplmn_dup_sim[REG_AS_ID_1] = FALSE;
  reg_timers_pet_watchdog();

  status = mcfg_fs_read("/nv/item_files/modem/nas/rplmn", &reg_nv_rplmn_sim[REG_AS_ID_1], sizeof(sys_plmn_id_s_type), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)REG_AS_ID_1);

  if(status ==  MCFG_FS_STATUS_OK)
  {
    MSG_HIGH_DS_3(REG_SUB, "=REG=  READ RPLMN from NV %d %d %d",
                  reg_nv_rplmn_sim[REG_AS_ID_1].identity[0],
                  reg_nv_rplmn_sim[REG_AS_ID_1].identity[1],
                  reg_nv_rplmn_sim[REG_AS_ID_1].identity[2]);
    if(!sys_plmn_id_is_undefined(reg_nv_rplmn_sim[REG_AS_ID_1]))
    {
      reg_nv_rplmn_dup_sim[REG_AS_ID_1] = TRUE;
    }
  }
  else
  {
    reg_as_id = REG_AS_ID_1;
    reg_nv_psm_update_cache_bitmask(reg_nv_psm_nv_list_bitmask,
                                    REG_PSM_EFS_RPLMN);
  }

  sys_plmn_undefine_plmn_id(&reg_nv_rplmn_sim[REG_AS_ID_2]);
  reg_nv_rplmn_dup_sim[REG_AS_ID_2] = FALSE;
  reg_timers_pet_watchdog(); 
  status = mcfg_fs_read("/nv/item_files/modem/nas/rplmn", &reg_nv_rplmn_sim[REG_AS_ID_2], sizeof(sys_plmn_id_s_type), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)REG_AS_ID_2);

  if(status == MCFG_FS_STATUS_OK)
  {
    MSG_HIGH_DS_3(REG_SUB, "=REG=  READ RPLMN from NV %d %d %d", 
                   reg_nv_rplmn_sim[REG_AS_ID_2].identity[0],
                   reg_nv_rplmn_sim[REG_AS_ID_2].identity[1],
                   reg_nv_rplmn_sim[REG_AS_ID_2].identity[2]);
    if(!sys_plmn_id_is_undefined(reg_nv_rplmn_sim[REG_AS_ID_2]))
    {
      reg_nv_rplmn_dup_sim[REG_AS_ID_2] = TRUE;
    }
  }
  else
  {
    reg_as_id = REG_AS_ID_2;
    reg_nv_psm_update_cache_bitmask(reg_nv_psm_nv_list_bitmask,
                                    REG_PSM_EFS_RPLMN);
    reg_as_id = REG_AS_ID_1;
  }
#ifdef FEATURE_SGLTE_DUAL_SIM
  sys_plmn_undefine_plmn_id(&reg_nv_rplmn_sim[REG_AS_ID_3]);
  reg_nv_rplmn_dup_sim[REG_AS_ID_3] = FALSE;
  reg_timers_pet_watchdog(); 
  status = mcfg_fs_read("/nv/item_files/modem/nas/rplmn", &reg_nv_rplmn_sim[REG_AS_ID_3], sizeof(sys_plmn_id_s_type), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)REG_AS_ID_3);

  if(status == MCFG_FS_STATUS_OK)
  {
    MSG_HIGH_DS_3(REG_SUB, "=REG=  READ RPLMN from NV %d %d %d", 
                   reg_nv_rplmn_sim[REG_AS_ID_3].identity[0],
                   reg_nv_rplmn_sim[REG_AS_ID_3].identity[1],
                   reg_nv_rplmn_sim[REG_AS_ID_3].identity[2]);
    if(!sys_plmn_id_is_undefined(reg_nv_rplmn_sim[REG_AS_ID_3]))
    {
      reg_nv_rplmn_dup_sim[REG_AS_ID_3] = TRUE;
    }
  }
  else
  {
    reg_as_id = REG_AS_ID_3;
    reg_nv_psm_update_cache_bitmask(reg_nv_psm_nv_list_bitmask,
                                    REG_PSM_EFS_RPLMN);
    reg_as_id = REG_AS_ID_1;
  }

#endif 
#else
  sys_plmn_undefine_plmn_id(&reg_nv_rplmn);
  reg_nv_rplmn_dup = FALSE;
  reg_timers_pet_watchdog();
  status = mcfg_fs_read("/nv/item_files/modem/nas/rplmn", &reg_nv_rplmn, sizeof(sys_plmn_id_s_type), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)REG_AS_ID_1);

  if(status == MCFG_FS_STATUS_OK)
  {
    MSG_HIGH_DS_3(REG_SUB, "=REG=  READ RPLMN from NV %d %d %d", 
     reg_nv_rplmn.identity[0], reg_nv_rplmn.identity[1], reg_nv_rplmn.identity[2]);
    /* Value read from NV will be defined only if it is different from RPLMN
        So marking duplicate flag TRUE*/
    if(!sys_plmn_id_is_undefined(reg_nv_rplmn))
    {
      reg_nv_rplmn_dup = TRUE;
    }
  }
  else
  {
    reg_nv_psm_update_cache_bitmask(reg_nv_psm_nv_list_bitmask,
                                    REG_PSM_EFS_RPLMN);
  }
#endif /*FEATURE_DUAL_SIM*/
#endif /*FEATURE_FEMTO_CSG*/


#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  for ( reg_as_id=REG_AS_ID_1; reg_as_id < (reg_as_id_e_type)MAX_NAS_STACKS; reg_as_id++ )
#endif
  {
     /* --------------------------------------------------------------------                                
      ** Read the ANITE GCF FLAG value from the NV
      ** ------------------------------------------------------------------*/

    if ((status_new = ghdi_nvmem_read(NV_GPRS_ANITE_GCF_I,
          (ghdi_nvmem_data_T *)&reg_nv_gcf_flag)) != GHDI_SUCCESS)
    {
      reg_nv_gcf_flag = FALSE;  /* Set the default value */
      MSG_ERROR_DS_1(REG_SUB, "=REG= Unable to read NV for GCF Flag: %d", status);
    }
  }
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  reg_as_id = REG_AS_ID_1;
  reg_sub_id = reg_sub_id_stack[reg_as_id];
#endif
#ifdef DISABLE_9x05_OPTIMIZATIONS
#ifdef FEATURE_DUAL_SIM
  reg_timers_pet_watchdog();
  status = mcfg_fs_read("/nv/item_files/modem/nas/data_roam_status", &reg_nv_data_romaing_sim[REG_AS_ID_1], sizeof(sys_data_roaming_e_type), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)REG_AS_ID_1);

  if(status != MCFG_FS_STATUS_OK )
  {
    reg_nv_data_romaing_sim[REG_AS_ID_1] = SYS_DATA_ROAMING_ON;
    reg_nv_psm_update_cache_bitmask(reg_nv_psm_nv_list_bitmask,
                                    REG_PSM_EFS_DATA_ROAM_STATUS);
  }
  MSG_HIGH_DS_2(REG_SUB, "=REG= NV reg_nv_data_romaing from EFS  with status %d and value is %d for SIM1",
                status,
                reg_nv_data_romaing_sim[REG_AS_ID_1]);
  reg_timers_pet_watchdog();
  status = mcfg_fs_read("/nv/item_files/modem/nas/data_roam_status", &reg_nv_data_romaing_sim[REG_AS_ID_2], sizeof(sys_data_roaming_e_type), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)REG_AS_ID_2);
 
  if(status != MCFG_FS_STATUS_OK)
  {
    reg_nv_data_romaing_sim[REG_AS_ID_2] = SYS_DATA_ROAMING_ON;
    reg_sub_id = REG_AS_ID_2;
    reg_nv_psm_update_cache_bitmask(reg_nv_psm_nv_list_bitmask,
                                    REG_PSM_EFS_DATA_ROAM_STATUS);
    reg_sub_id = REG_AS_ID_1;
  }
  MSG_HIGH_DS_2(REG_SUB, "=REG= NV reg_nv_data_romaing from EFS with status %d and value is %d for SIM2",
                status,
                reg_nv_data_romaing_sim[REG_AS_ID_2]);
  
#ifdef FEATURE_TRIPLE_SIM
  reg_timers_pet_watchdog();
  status = mcfg_fs_read("/nv/item_files/modem/nas/data_roam_status", &reg_nv_data_romaing_sim[REG_AS_ID_3], sizeof(sys_data_roaming_e_type), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)REG_AS_ID_3);
 
  if(status != MCFG_FS_STATUS_OK)
  {
   reg_nv_data_romaing_sim[REG_AS_ID_3] = SYS_DATA_ROAMING_ON;
    reg_sub_id = REG_AS_ID_3;
    reg_nv_psm_update_cache_bitmask(reg_nv_psm_nv_list_bitmask,
                                    REG_PSM_EFS_DATA_ROAM_STATUS);
    reg_sub_id = REG_AS_ID_1;
  }
  MSG_HIGH_DS_2(REG_SUB, "=REG= NV reg_nv_data_romaing from EFS with status %d and value is %d for SIM3",
                status,
                reg_nv_data_romaing_sim[REG_AS_ID_3]);

#endif
#else
  reg_timers_pet_watchdog();
  status = mcfg_fs_read("/nv/item_files/modem/nas/data_roam_status", &reg_nv_data_romaing, sizeof(sys_data_roaming_e_type), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)REG_AS_ID_1);
  if(status != MCFG_FS_STATUS_OK )
  {
    reg_nv_data_romaing = SYS_DATA_ROAMING_ON;
    reg_nv_psm_update_cache_bitmask(reg_nv_psm_nv_list_bitmask,
                                    REG_PSM_EFS_DATA_ROAM_STATUS);
  }
  MSG_HIGH_DS_2(REG_SUB,"=REG= NV reg_nv_data_romaing from EFS with status %d and value is %d",
                                                                     status,reg_nv_data_romaing);
#endif
#endif


#if defined FEATURE_DUAL_SIM
  for(reg_sub_id = REG_AS_ID_1; reg_sub_id < MAX_AS_IDS; reg_sub_id++)
#endif
  {
    reg_nv_read_carrier_specific_efs();
  }
#ifdef FEATURE_CIOT
#ifndef FEATURE_CATM1_DISABLE
  reg_nv_read_ciot_mcc_bands_params(SYS_SYS_MODE_LTE_M1);
#endif
  reg_nv_read_ciot_mcc_bands_params(SYS_SYS_MODE_LTE_NB1);
  reg_nv_read_ciot_mcc_rats_params();
  reg_nv_read_ciot_nb1_interlace_params();
#endif
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  reg_as_id = REG_AS_ID_1;
  reg_sub_id = reg_sub_id_stack[reg_as_id];
#endif

}

/*===========================================================================

FUNCTION reg_nv_t3245_timer_stored_value_get

DESCRIPTION
  Returns T3245 that is read from the NV.

DEPENDENCIES
  none

===========================================================================*/

rex_timer_cnt_type reg_nv_t3245_timer_stored_value_get
(
  void
)
{
    MSG_HIGH_DS_1(REG_SUB, "=REG= reg_nv_t3245_timer_stored_value is %d", reg_nv_t3245_timer_stored_value);
  return reg_nv_t3245_timer_stored_value;
}

/*===========================================================================

FUNCTION reg_nv_read_efnas_config

DESCRIPTION
  Reads EFNASCONFIG from NV.

DEPENDENCIES
  none

===========================================================================*/

void reg_nv_read_efnas_config
(
  void
)
{
    byte  *ef_nasconfig_p = NULL;
    mcfg_fs_status_e_type status = MCFG_FS_STATUS_EFS_ERR;
#ifdef FEATURE_MODEM_HEAP
  ef_nasconfig_p = (byte *)modem_mem_alloc((unsigned int)REG_NV_EFNAS_CONFIG_LENGTH, MODEM_MEM_CLIENT_NAS);
#else
  ef_nasconfig_p = (byte *)gs_alloc(REG_NV_EFNAS_CONFIG_LENGTH);
#endif
  if(ef_nasconfig_p == NULL)
  {
    MSG_ERROR_DS_0(REG_SUB, "=REG= Could not allocate the memory");    
    return;
  }
  memset((void*)ef_nasconfig_p, 0, REG_NV_EFNAS_CONFIG_LENGTH);

  status = mcfg_fs_read("/nv/item_files/modem/nas/efnas_config", ef_nasconfig_p,REG_NV_EFNAS_CONFIG_LENGTH, MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)reg_as_id);

    MSG_HIGH_DS_1(REG_SUB, "=REG= NV reg_nv_efnas_config from EFS with status %d",status);

  if(status == MCFG_FS_STATUS_OK)
  {
    reg_mode_parse_nas_config(ef_nasconfig_p, REG_NV_EFNAS_CONFIG_LENGTH);
  }
#ifdef FEATURE_MODEM_HEAP
  modem_mem_free(ef_nasconfig_p, MODEM_MEM_CLIENT_NAS);
#else
  gs_free(ef_nasconfig_p);
#endif
  ef_nasconfig_p = NULL;
}



/*===========================================================================

FUNCTION reg_nv_validate_sim_timer_value_get

DESCRIPTION
  Returns T3245 that is read from the NV.

DEPENDENCIES
  none

===========================================================================*/

rex_timer_cnt_type reg_nv_validate_sim_timer_value_get
(
  void
)
{
  MSG_HIGH_DS_1(REG_SUB, "=REG= reg_nv_validate_sim_timer_value value %d", reg_nv_validate_sim_timer_value);
  return reg_nv_validate_sim_timer_value;
}


/*===========================================================================

FUNCTION reg_nv_t3245_test_timer_value_get

DESCRIPTION
  Returns T3245 that is read from the NV.
  This value should only be used for test purposes.

DEPENDENCIES
  none

===========================================================================*/

rex_timer_cnt_type reg_nv_t3245_test_timer_value_get
(
  void
)
{
  MSG_HIGH_DS_1(REG_SUB, "=REG= reg_nv_t3245_test_timer_value value %d", reg_nv_t3245_test_timer_value);
  return reg_nv_t3245_test_timer_value;
}

/*===========================================================================

FUNCTION reg_nv_timestamp_at_power_off_get

DESCRIPTION
  Returns timestamp at power off that is read from the NV.
  This value should only be used for test purposes.

DEPENDENCIES
  none

===========================================================================*/

uint64 reg_nv_timestamp_at_power_off_get
(
  void
)
{
  MSG_HIGH_DS_1(REG_SUB, "=REG= reg_nv_timestamp_at_power_off value %d", reg_nv_timestamp_at_power_off);
  return reg_nv_timestamp_at_power_off;
}

#ifdef FEATURE_CIOT
/*===========================================================================

FUNCTION reg_nv_get_ciot_irat_timer_at_power_off

DESCRIPTION
  Returns ciot irat timer at power off that is read from the NV.
  This value should only be used for test purposes.

DEPENDENCIES
  none

===========================================================================*/

uint64 reg_nv_get_ciot_irat_timer_at_power_off
(
  void
)
{
  MSG_HIGH_DS_1(REG_SUB, "=REG= reg_nv_ciot_irat_timer_at_power_off value %d", reg_nv_ciot_irat_timer_at_power_off);
  return reg_nv_ciot_irat_timer_at_power_off;
}
#endif

/*==============================================================================

FUNCTION NAME

  reg_nv_write_t3245_to_efs

==============================================================================*/

boolean reg_nv_write_t3245_to_efs(rex_timer_cnt_type t3245_value)
{
  mcfg_fs_status_e_type status = MCFG_FS_STATUS_EFS_ERR;

  reg_nv_t3245_timer_stored_value = t3245_value;
  status = mcfg_fs_write("/nv/item_files/modem/nas/t3245_timer_stored",&t3245_value,sizeof(rex_timer_cnt_type), MCFG_FS_O_CREAT | MCFG_FS_O_AUTODIR, MCFG_FS_ALLPERMS,MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)reg_as_id);
  if( status != MCFG_FS_STATUS_OK)
  {
    MSG_ERROR_DS_1(REG_SUB, "=REG= Writing to NV for T3245 timer failed status = %d", status);
    return FALSE;
  }
  else
  {
    MSG_HIGH_DS_1(REG_SUB, "=REG=  Wrote %d value for timer T3245 to NV", t3245_value);
    if(reg_nv_psm_cache_ptr != NULL)
    {
      REG_SET_BM_32BIT(reg_nv_psm_cache_ptr->psm_nv_list_bitmask, REG_PSM_EFS_T3245_TIMER_STORED);
    }
  }
  return TRUE;
}

#ifdef FEATURE_CIOT
/*==============================================================================

FUNCTION NAME

  reg_nv_write_ciot_irat_timer_to_efs

==============================================================================*/

boolean reg_nv_write_ciot_irat_timer_to_efs(rex_timer_cnt_type ciot_irat_value)
{
  if(ciot_irat_value > 0)
  {
    mcfg_fs_status_e_type status = MCFG_FS_STATUS_EFS_ERR;

    status = mcfg_fs_write("/nv/item_files/modem/nas/ciot_irat_timer_at_power_off",&ciot_irat_value,sizeof(rex_timer_cnt_type), MCFG_FS_O_CREAT | MCFG_FS_O_AUTODIR, MCFG_FS_ALLPERMS,MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)reg_as_id);
    if( status != MCFG_FS_STATUS_OK)
    {
      MSG_ERROR_DS_1(REG_SUB, "=REG= Writing to NV for ciot_irat_value timer failed status = %d", status);
      return FALSE;
    }
    else
    {
      MSG_HIGH_DS_1(REG_SUB, "=REG=  Wrote %d value for timer CIOT IRAT TIMER to NV", ciot_irat_value);
    }
	
	return TRUE;
  }
  else
  {
	return FALSE;
  }
}
#endif

/*===========================================================================

FUNCTION reg_nv_read_enhanced_hplmn_srch_tbl

DESCRIPTION
  Read ENHANCED_HPLMN_SRCH_TBL from NV.
  If read fails, populate default tbl.

DEPENDENCIES
  none

RETURN VALUE
  Whether NV read was successful or not.

SIDE EFFECTS
  none

===========================================================================*/
void reg_nv_read_enhanced_hplmn_srch_tbl
(
  byte* nv_config_tbl
)
{

  /*
  ** Fill NV command to read IMSI.
  */
   if (reg_ghdi_nvmem_read(NV_ENHANCED_HPLMN_SRCH_TBL_I,
            (ghdi_nvmem_data_T *)&nv_config_tbl) != GHDI_SUCCESS)
  {
    MSG_HIGH_DS_0(REG_SUB, "=REG= Reading ENHANCED_HPLMN_SRCH_TBL failed");
    /* Check ENS feature and populate default table if needed */
    reg_nv_populate_default_enhanced_hplmn_srch_tbl(nv_config_tbl);
  }
  /*
  ** TBL was successfully read from NVRAM.
  */
  else
  {
      //memscpy((void*)nv_config_tbl, REG_NV_CONFIG_TBL_MAX_SIZE, (void*)(reg_nv_item.enhanced_hplmn_srch_tbl), REG_NV_CONFIG_TBL_MAX_SIZE);
      MSG_MED_DS_3(REG_SUB, "=REG= REG NV Bytes: %d, %d, %d", reg_nv_item.enhanced_hplmn_srch_tbl[0], reg_nv_item.enhanced_hplmn_srch_tbl[1], reg_nv_item.enhanced_hplmn_srch_tbl[2]);
  }
}

#ifdef FEATURE_LTE
/*===========================================================================

FUNCTION reg_nv_read_epsloci

DESCRIPTION
  Read EPSLOCI from EFS

DEPENDENCIES
  none

RETURN VALUE
  Whether NV read was successful or not.

SIDE EFFECTS
  none

===========================================================================*/

boolean reg_nv_read_epsloci(byte *epsloci)
{
  mcfg_fs_status_e_type status = MCFG_FS_STATUS_EFS_ERR;
 
  status = mcfg_fs_read("/nvm/alpha/modem/nas/lte_nas_eps_loci", epsloci,REG_EPSLOCI_LENGTH, MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)reg_sub_id);
  MSG_HIGH_DS_1(REG_SUB, "=REG=  EPSLOCI read status = %d ", status);

  return (status == MCFG_FS_STATUS_OK);  
}

#ifdef FEATURE_MMSS_3_1
/*===========================================================================

FUNCTION reg_nv_read_ignore_uplmn

DESCRIPTION
 Whether to ignore uplmn or not

DEPENDENCIES
  none

RETURN VALUE

SIDE EFFECTS
  none

===========================================================================*/
boolean reg_nv_read_ignore_uplmn(void)
{
  MSG_HIGH_DS_1(REG_SUB, "=REG= Ignore UPLMN = %d", reg_nv_ignore_uplmn);
  return reg_nv_ignore_uplmn;
}
#endif /*FEATURE_MMSS_3_1*/

#endif /*FEATURE_LTE*/

#ifdef FEATURE_EQUIVALENT_HPLMN
/*===========================================================================

FUNCTION reg_nv_read_efs_ehplmn_file

DESCRIPTION
  Read EHPLMN list from EFS

  Reading from EFS NV failed or NV is set with invalid EHPLMN list length
  (length greater than max supported length 20 or length equal to 0) this function will set ehplmn_list length as 0.
  WHEN NV read is successful and EHPLMN list length is valid this function will copy the EHPLMN list from NV
  into the ehplmn_list.


DEPENDENCIES
  none

RETURN VALUE
  TRUE/FALSE depending on whether valid EHPLMN list is successfully read from NV or not.

SIDE EFFECTS
  none

===========================================================================*/
boolean  reg_nv_read_efs_ehplmn_file
(
  reg_sim_plmn_list_s_type   *ehplmn_list
)
{
  mcfg_fs_status_e_type status = MCFG_FS_STATUS_EFS_ERR;


  /*
  **EFS NV will contain length of the list in the first byte and length of the list *3 entries 
  **containing valid PLMN IDs. Total length of EFS NV item is 1+REG_EFS_EHPLMN_MAX_LENGTH*3
  */
  byte               plmn_list[REG_EFS_EHPLMN_MAX_LENGTH*3+1];
  sys_plmn_id_s_type       hplmn;
  uint32 i,j;
  boolean                  found_plmn = FALSE;

  hplmn = reg_sim_read_hplmn();
  for( j=0; j< REG_NV_EFS_EHPLMN_LIST; j++)
  {
    reg_timers_pet_watchdog();
    switch(j+1)
    {
      case 1:       
       status = mcfg_fs_read("/nv/item_files/modem/nas/ehplmn", &plmn_list[0], (REG_EFS_EHPLMN_MAX_LENGTH*3)+1, MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)REG_AS_ID_1);
        break;
      case 2:
       status = mcfg_fs_read("/nv/item_files/modem/nas/ehplmn", &plmn_list[0], (REG_EFS_EHPLMN_MAX_LENGTH*3)+1, MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)REG_AS_ID_2);
        break;
      case 3:
       status = mcfg_fs_read("/nv/item_files/modem/nas/ehplmn1", &plmn_list[0], (REG_EFS_EHPLMN_MAX_LENGTH*3)+1, MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)REG_AS_ID_1);
        break;
      case 4:
       status = mcfg_fs_read("/nv/item_files/modem/nas/ehplmn2", &plmn_list[0], (REG_EFS_EHPLMN_MAX_LENGTH*3)+1, MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)REG_AS_ID_1);
        break;
      case 5:
       status = mcfg_fs_read("/nv/item_files/modem/nas/ehplmn3", &plmn_list[0], (REG_EFS_EHPLMN_MAX_LENGTH*3)+1, MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)REG_AS_ID_1);
        break;
      case 6:
       status = mcfg_fs_read("/nv/item_files/modem/nas/ehplmn4", &plmn_list[0], (REG_EFS_EHPLMN_MAX_LENGTH*3)+1, MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)REG_AS_ID_1);
        break;
      case 7:
       status = mcfg_fs_read("/nv/item_files/modem/nas/ehplmn5", &plmn_list[0], (REG_EFS_EHPLMN_MAX_LENGTH*3)+1, MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)REG_AS_ID_1);
        break;
      default:
        break;
    }
    ehplmn_list->length=0;
    if (status == MCFG_FS_STATUS_OK)
    {
      MSG_MED_DS_1(REG_SUB,"=REG= Reading from EFS NV succeeded %d",status);
      /*Update EHPLMN list length as first byte read from the NV*/
      ehplmn_list->length = plmn_list[0];
      if(( ehplmn_list->length <= REG_EFS_EHPLMN_MAX_LENGTH) && (ehplmn_list->length != 0))
      {
        MSG_HIGH_DS_1(REG_SUB, "=REG= Received valid EHPLMN list length %d", ehplmn_list->length);
        memscpy( &ehplmn_list->plmn[0], sizeof(sys_plmn_id_s_type)*REG_SIM_PLMN_LIST_LENGTH, &plmn_list[1], ehplmn_list->length*3);
		
      if (reg_mode_is_usim_compliant())
      {
        found_plmn = TRUE;
        MSG_HIGH_DS_0(REG_SUB, "=REG= SIM compliant use the EHPLMNs");
        break;
      }
        for (i = 0; i < ehplmn_list->length; ++i)
        {
          if ( sys_plmn_match( hplmn, ehplmn_list->plmn[i]))   
          {              
            found_plmn = TRUE;
            MSG_HIGH_DS_0(REG_SUB, "=REG= Match between HPLMN IMSI and EHPLMN");
            break;
          }
        }
      }
      else
      {
        MSG_HIGH_DS_1(REG_SUB, "=REG= Invalid EHPLMN list length %d is received ", ehplmn_list->length);
        /*EHPLMN list length is invalid. update length as 0*/
        ehplmn_list->length=0;
      }     
    }
    else
    {
      MSG_HIGH_DS_1(REG_SUB, "=REG= Reading from EFS EHPLMN NV failed %d", status);
    }
    if(found_plmn)          
    {
      break;
    }
    else
    {
      continue;
    }
  }

  if(found_plmn == FALSE)
  {
    ehplmn_list->length=0;
  }
  return (ehplmn_list->length != 0);
}

/*===========================================================================

FUNCTION reg_nv_read_tdscdma_plmn_file

DESCRIPTION
  Read PLMN list from EFS and populates a local database

  Used for TDSCDMA carrier specific requests


DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void  reg_nv_read_tdscdma_plmn_file (void)
{
  mcfg_fs_status_e_type status = MCFG_FS_STATUS_EFS_ERR;

 
  /*
  **EFS NV will contain length of the list in the first byte and length of the list *3 entries 
  **containing valid PLMN IDs. Total length of EFS NV item is 1+REG_EFS_EHPLMN_MAX_LENGTH*3
  */
  byte               plmn_list[REG_EFS_EHPLMN_MAX_LENGTH*3+1];
  
  

  NAS_ENTER_CRIT_SECT(reg_tdscdma_op_list_crit_sect);
  status = mcfg_fs_read("/nv/item_files/modem/nas/tdscdma_op_plmn_list", &plmn_list[0],(REG_EFS_EHPLMN_MAX_LENGTH*3)+1, MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)REG_AS_ID_1);

  if(status != MCFG_FS_STATUS_OK)
  {
    status = mcfg_fs_read("/nv/item_files/modem/nas/tdscdma_op_plmn_list", &plmn_list[0],(REG_EFS_EHPLMN_MAX_LENGTH*3)+1, MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)REG_AS_ID_2);
  }
  
  reg_nv_tdscdma_op_plmn_efs_list.length = 0;
  if (status == MCFG_FS_STATUS_OK)
  {
    MSG_MED_DS_0(REG_SUB, "=REG= Reading from TDSCDMA OP PLMN LIST EFS NV succeeded ");
    /*Update PLMN list length as first byte read from the NV*/
    reg_nv_tdscdma_op_plmn_efs_list.length = plmn_list[0];
    if(( reg_nv_tdscdma_op_plmn_efs_list.length <= REG_EFS_EHPLMN_MAX_LENGTH) && (reg_nv_tdscdma_op_plmn_efs_list.length != 0))
    {
      MSG_HIGH_DS_1(REG_SUB, "=REG= Read a valid TDSCDMA_PLMN list length %d", reg_nv_tdscdma_op_plmn_efs_list.length);
      memscpy( &reg_nv_tdscdma_op_plmn_efs_list.plmn[0],sizeof(sys_plmn_id_s_type)*REG_SIM_PLMN_LIST_LENGTH, &plmn_list[1], reg_nv_tdscdma_op_plmn_efs_list.length*3);      
    }
    else
    {
      MSG_HIGH_DS_1(REG_SUB, "=REG= Invalid TDSCDMA PLMN list length %d is received ", 
                    reg_nv_tdscdma_op_plmn_efs_list.length);
      /*PLMN list length is invalid. update length as 0*/
      reg_nv_tdscdma_op_plmn_efs_list.length=0;
    }
  }
  else
  {
    MSG_HIGH_DS_1(REG_SUB, "=REG= Reading from EFS PLMN NV failed status = %d", status);
  }
  NAS_EXIT_CRIT_SECT(reg_tdscdma_op_list_crit_sect);
}

#endif

#if defined FEATURE_TDSCDMA || defined FEATURE_SGLTE
void reg_nv_read_lte_cell_lists(void)
{
  mcfg_fs_status_e_type status = MCFG_FS_STATUS_EFS_ERR;

  uint32 data_size = 0;
  uint32 total_size=0;
  byte *byte_array = NULL;

  
  struct fs_stat  nas_conf_stat;

//  memset((void *)nas_conf_stat,0,sizeof(nas_conf_stat));

  nas_conf_stat.st_size = 0;
  if ( mcfg_fs_stat ("/nv/item_files/modem/nas/lte_avaialable_cell_list",&nas_conf_stat,MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)REG_AS_ID_1) != MCFG_FS_STATUS_OK)
  {
      MSG_HIGH_DS_0(REG_SUB, "=REG= efs stat failed for older efs file lte_avaialable_cell_list");
  }
  else
  {
      MSG_HIGH_DS_0(REG_SUB,"=REG= Older efs file exists :lte_avaialable_cell_list - Deleting it");
      status = mcfg_fs_delete("/nv/item_files/modem/nas/lte_avaialable_cell_list", MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)REG_AS_ID_1);
  }
  nas_conf_stat.st_size = 0;

  status = mcfg_fs_stat ("/nv/item_files/modem/nas/lte_available_tg_cell_list",&nas_conf_stat,MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)REG_AS_ID_1);

  if (status != MCFG_FS_STATUS_OK)
  {
    MSG_HIGH_DS_1(REG_SUB,"=REG= LTE IRAT BPLMN : LTE Available List mcfg stat failed ,status %d", status);
  }
  else
  {
    MSG_HIGH_DS_1(REG_SUB, "=REG= LTE IRAT BPLMN : LTE Available List lenght of file is %d ",nas_conf_stat.st_size);           
  }

  total_size = nas_conf_stat.st_size;
  
  byte_array = (byte * )modem_mem_alloc(total_size,MODEM_MEM_CLIENT_NAS);
  status = mcfg_fs_read("/nv/item_files/modem/nas/lte_available_tg_cell_list", byte_array, total_size, MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)REG_AS_ID_1);

  if (status == MCFG_FS_STATUS_OK)
  {
    memscpy((void *)&data_size,sizeof(data_size),(void*)&byte_array[0],sizeof(uint32));
    if (data_size == 0 )
    {
      MSG_HIGH_DS_0(REG_SUB, "=REG= LTE IRAT BPLMN : LTE avaialble list total size is zero no reading further");             
    }
    else
    {
      lte_available_list_ptr = NULL;
      lte_available_list_ptr = reg_convert_byte_array_to_lte_cell_list(lte_available_list_ptr,byte_array,TRUE);
    }
  } 
  else
  {
    MSG_HIGH_DS_1(REG_SUB, "=REG= LTE IRAT BPLMN : lte avaialble list efs reading fail, error value %d ", efs_errno);
    reg_nv_psm_update_cache_bitmask(reg_nv_psm_nv_list_bitmask,
                                    REG_PSM_EFS_LTE_AVAILABLE_TG_CELL_LIST);
  }

  if(byte_array != NULL)
  {
    modem_mem_free(byte_array,MODEM_MEM_CLIENT_NAS);
  }
  
  data_size = 0;
  total_size=0; 
  byte_array = NULL;
  status =  MCFG_FS_STATUS_EFS_ERR;


 // memset((void *)nas_conf_stat,0,sizeof(nas_conf_stat));

  nas_conf_stat.st_size = 0;

  if( mcfg_fs_stat ("/nv/item_files/modem/nas/lte_non_avaialable_cell_list",&nas_conf_stat,MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)REG_AS_ID_1) != MCFG_FS_STATUS_OK)
  {
      MSG_HIGH_DS_0(REG_SUB, "=REG= efs stat failed for older mcfg file lte_non_avaialable_cell_list");
  }
  else
  {
      MSG_HIGH_DS_0(REG_SUB,"=REG= Older file exists :lte_non_avaialable_cell_list - Deleting it");
      status = mcfg_fs_delete("/nv/item_files/modem/nas/lte_non_avaialable_cell_list", MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)REG_AS_ID_1);
  }


  nas_conf_stat.st_size = 0;
  status = mcfg_fs_stat ("/nv/item_files/modem/nas/lte_non_available_tg_cell_list",&nas_conf_stat,MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)REG_AS_ID_1);

  if (status != MCFG_FS_STATUS_OK)
  {
    MSG_HIGH_DS_1(REG_SUB,"=REG= LTE IRAT BPLMN : LTE Non Available List mcfg stat failed, status %d ", status);
  }
  else
  {
    MSG_HIGH_DS_1(REG_SUB, "=REG= LTE IRAT BPLMN : LTE Non Available List lenght of file is %d", nas_conf_stat.st_size);
  }

  total_size = nas_conf_stat.st_size;

  byte_array = (byte * )modem_mem_alloc(total_size,MODEM_MEM_CLIENT_NAS);
  status = mcfg_fs_read("/nv/item_files/modem/nas/lte_non_available_tg_cell_list", byte_array, total_size, MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)REG_AS_ID_1);

  if (status == MCFG_FS_STATUS_OK)
  {
    memscpy((void *)&data_size,sizeof(data_size),(void*)&byte_array[0],sizeof(uint32));
    if (total_size == 0 )
    {
      MSG_HIGH_DS_0(REG_SUB,"=REG= LTE IRAT BPLMN : LTE not avaialble list total size is zero no reading further");            
    }
    else
    {
      lte_non_available_list_ptr = NULL;
      lte_non_available_list_ptr = reg_convert_byte_array_to_lte_cell_list(lte_non_available_list_ptr,byte_array,FALSE);
    }
  } 
  else
  {
    MSG_HIGH_DS_1(REG_SUB, "=REG= LTE IRAT BPLMN : LTE not avaialble list list efs reading fail error number %d", efs_errno);
    reg_nv_psm_update_cache_bitmask(reg_nv_psm_nv_list_bitmask,
                                    REG_PSM_EFS_LTE_NON_AVAILABLE_TG_CELL_LIST);
  }
  if(byte_array != NULL)
  {
    modem_mem_free(byte_array,MODEM_MEM_CLIENT_NAS);
  }

}

boolean  reg_nv_write_lte_lists_to_efs(void)
{
  boolean result = TRUE;
  mcfg_fs_status_e_type efs_status = MCFG_FS_STATUS_EFS_ERR;

  uint32 data_size=0;
  byte *byte_array = NULL;
  int i;

  if(lte_available_list_ptr != NULL)
  {
    MSG_HIGH_DS_0(REG_SUB, "=REG= LTE IRAT BPLMN : Trying to Write LTE available list"); 
    byte_array = reg_convert_lte_list_into_array(lte_available_list_ptr,&data_size,TRUE);
    if (byte_array == NULL)
    {
      MSG_HIGH_DS_0(REG_SUB, "=REG= LTE IRAT BPLMN : LTE Available Array memory failed efs writing fail");                                     
      result = FALSE;
    }
    else
    {
      for(i =0; i<byte_array[0]; i++)
      {
        MSG_HIGH_DS_1(REG_SUB, "=REG= LTE IRAT BPLMN : LTE Available byte_array[i] = %d", byte_array[i]);                                                          
      }
      efs_status = mcfg_fs_write("/nv/item_files/modem/nas/lte_available_tg_cell_list",byte_array,data_size, MCFG_FS_O_RDWR|MCFG_FS_O_CREAT | MCFG_FS_O_AUTODIR, MCFG_FS_ALLPERMS,MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)REG_AS_ID_1);
      if( efs_status != MCFG_FS_STATUS_OK)
      {
        MSG_HIGH_DS_1(REG_SUB, "=REG= LTE IRAT BPLMN : LTE Avaialble list mcfg writing failed status = %d", efs_status);
        result = FALSE;
      }
      else
      {
        MSG_HIGH_DS_0(REG_SUB, "=REG= LTE IRAT BPLMN : lte avaialble list efs writing succeed");
        if(reg_nv_psm_cache_ptr != NULL)
        {
          REG_SET_BM_32BIT(reg_nv_psm_cache_ptr->psm_nv_list_bitmask, REG_PSM_EFS_LTE_AVAILABLE_TG_CELL_LIST);
        }
      }    
    }

    if(byte_array != NULL)
    {
      modem_mem_free(byte_array,MODEM_MEM_CLIENT_NAS);
    }
  }
  
  efs_status = 0;
  data_size=0;
  byte_array = NULL;

  if(lte_non_available_list_ptr != NULL)
  {
    MSG_HIGH_DS_0(REG_SUB, "=REG= LTE IRAT BPLMN : Trying to Write LTE non available list to EFS"); 
    byte_array = reg_convert_lte_list_into_array(lte_non_available_list_ptr,&data_size,FALSE);
    if (byte_array == NULL)
    {
      MSG_HIGH_DS_0(REG_SUB, "=REG= LTE IRAT BPLMN : LTE Non Avaialble list Array memmory failed efs writing fail");                                          
      result = FALSE;
    }
    else
    {
      for(i =0; i<byte_array[0]; i++)
      {
        MSG_HIGH_DS_2(REG_SUB, "=REG= LTE IRAT BPLMN : LTE Non Avaialble list byte_array[%d] = %d", i, byte_array[i]);
      }
      efs_status = mcfg_fs_write("/nv/item_files/modem/nas/lte_non_available_tg_cell_list",byte_array,data_size, MCFG_FS_O_RDWR|MCFG_FS_O_CREAT | MCFG_FS_O_AUTODIR, MCFG_FS_ALLPERMS,MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)REG_AS_ID_1);

      if(efs_status != MCFG_FS_STATUS_OK)
      {
         MSG_HIGH_DS_1(REG_SUB, "=REG= LTE IRAT BPLMN : LTE Non Avaialble list mcfg writing failed error no = %d", efs_status);                        
         result = FALSE;
      }
      else
      {
        MSG_HIGH_DS_0(REG_SUB, "=REG= LTE IRAT BPLMN : lte not avaialble list efs writing succeed");
        if(reg_nv_psm_cache_ptr != NULL)
        {
          REG_SET_BM_32BIT(reg_nv_psm_cache_ptr->psm_nv_list_bitmask, REG_PSM_EFS_LTE_NON_AVAILABLE_TG_CELL_LIST);
        }
      }
    }
    if(byte_array != NULL)
    {
      modem_mem_free(byte_array,MODEM_MEM_CLIENT_NAS);
    }
  }
  return result;
//  reg_state_clear_lte_lists();
//  reg_timers_stop_backoff_lte_search_period_timer();
}

void reg_nv_clear_non_available_efs_list(void)
{
  uint32 length = 0;
  uint32 data_size = sizeof(uint32);

  if(mcfg_fs_write("/nv/item_files/modem/nas/lte_non_available_tg_cell_list",&length,data_size, MCFG_FS_O_RDWR|MCFG_FS_O_CREAT | MCFG_FS_O_AUTODIR, MCFG_FS_ALLPERMS,MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)REG_AS_ID_1)!= MCFG_FS_STATUS_OK)
  {
     MSG_HIGH_DS_0(REG_SUB, "=REG= LTE IRAT BPLMN : LTE Non Avaialble list efs writing fail");
  }
  reg_timers_stop_backoff_lte_search_period_timer();
}

void reg_nv_clear_available_efs_list(void)
{
  uint32 length = 0;
  uint32 data_size = sizeof(uint32);
  if(mcfg_fs_write("/nv/item_files/modem/nas/lte_available_tg_cell_list",&length,data_size, MCFG_FS_O_RDWR|MCFG_FS_O_CREAT | MCFG_FS_O_AUTODIR, MCFG_FS_ALLPERMS,MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)REG_AS_ID_1)!= MCFG_FS_STATUS_OK)
  {
    MSG_HIGH_DS_0(REG_SUB, "=REG= LTE IRAT BPLMN : LTE Avaialble list efs writing fail");                         
  }
}

#endif

/*===========================================================================

FUNCTION reg_nv_imp_variable_prints

DESCRIPTION
 print important efs once rather than always in servcie confimation/inidcation path

DEPENDENCIES
  none

RETURN VALUE

SIDE EFFECTS
  none

===========================================================================*/
void reg_nv_imp_variable_prints()
{
    MSG_HIGH_DS_1(REG_SUB, "=REG= NV IMSI Switch = %d", reg_nv_imsi_switch);
}

/*===========================================================================

FUNCTION reg_nv_is_imsi_switch_enabled

DESCRIPTION
 Whether to Switch the IMSI or not after trying registration on all the available PLMNs

DEPENDENCIES
  none

RETURN VALUE

SIDE EFFECTS
  none

===========================================================================*/
boolean reg_nv_is_imsi_switch_enabled()
{
  return reg_nv_imsi_switch;
}

/*===========================================================================

FUNCTION reg_nv_is_wcdma_freq_lock_enabled

DESCRIPTION
 Whether to Enable the UE to lock to ATT enaled WCDMA FREQ or NOT

DEPENDENCIES
  none

RETURN VALUE

SIDE EFFECTS
  none

===========================================================================*/
boolean reg_nv_is_wcdma_freq_lock_enabled()
{
  MSG_HIGH_DS_1(REG_SUB, "=REG= NV WCDMA FREQ LOCK enabled flag = %d", reg_nv_wcdma_freq_lock_enabled);
  return reg_nv_wcdma_freq_lock_enabled;
}

/*===========================================================================

FUNCTION reg_nv_is_lpm_power_off_enabled

DESCRIPTION
 Whether to write NV/EFS item on LPM

DEPENDENCIES
  none

RETURN VALUE

SIDE EFFECTS
  none

===========================================================================*/
boolean reg_nv_is_lpm_power_off_enabled(void)
{
  MSG_HIGH_DS_1(REG_SUB, "=REG= LPM power off behavior = %d", reg_nv_lpm_power_off);
  return reg_nv_lpm_power_off;  
}


/*===========================================================================

FUNCTION reg_nv_efrplmnsi_select_rplmn

DESCRIPTION
 This function return TRUE is UE should select RPLMN is HPMN is not found. 

DEPENDENCIES
  none

RETURN VALUE

SIDE EFFECTS
  none

===========================================================================*/
boolean reg_nv_efrplmnsi_select_rplmn(void)
{ 
  return reg_nv_efrplmnsi_selection;
}
#if defined FEATURE_TDSCDMA || defined FEATURE_SGLTE
/*===========================================================================

FUNCTION reg_nv_is_forced_irat_enabled

DESCRIPTION
 This function return TRUE is forced IRAT is enabled. 

DEPENDENCIES
  none

RETURN VALUE

SIDE EFFECTS
  none

===========================================================================*/
boolean reg_nv_is_forced_irat_enabled(void)
{     
    return reg_nv_forced_irat_enabled;
}

/*==============================================================================

FUNCTION NAME

  reg_nv_hplmn_irat_search_timer_set

==============================================================================*/
void reg_nv_hplmn_irat_search_timer_set(uint32 timer)
{
  reg_nv_hplmn_irat_search_timer = timer;
  MSG_HIGH_DS_1(REG_SUB, "=REG= Set HPLMN IRAT NV Search Timer to %d", timer);
}
/*==============================================================================

FUNCTION NAME

  reg_nv_hplmn_irat_search_timer_set

==============================================================================*/
uint32 reg_nv_hplmn_irat_search_timer_get()
{
  return reg_nv_hplmn_irat_search_timer;
}

#endif

/*===========================================================================

FUNCTION reg_nv_gcf_flag_get

===========================================================================*/

rex_timer_cnt_type reg_nv_gcf_flag_get
(
  void
)
{
  MSG_HIGH_DS_1(REG_SUB, "=REG= reg_nv_gcf_flag value set to %d", reg_nv_gcf_flag);
  return reg_nv_gcf_flag;
}

/*==============================================================================

FUNCTION NAME

  reg_nv_tdscdma_plmn_list_get

==============================================================================*/
reg_sim_plmn_list_s_type* reg_nv_tdscdma_plmn_list_get(void)
{
       return &reg_nv_tdscdma_op_plmn_efs_list; 
}


#ifdef FEATURE_TDSCDMA
/*==============================================================================

FUNCTION NAME

  reg_mode_ehplmn_list_set

==============================================================================*/
void reg_nv_ehplmn_list_set(reg_sim_plmn_list_s_type ehplmn_list_efs)
{
   uint32 i;   
   reg_nv_ehplmn_efs_list.length = ehplmn_list_efs.length;

   for(i = 0; i < ehplmn_list_efs.length; ++i)
   {
     reg_nv_ehplmn_efs_list.plmn[i] = ehplmn_list_efs.plmn[i];
   }
}


/*==============================================================================

FUNCTION NAME

  reg_nv_ehplmn_list_get

==============================================================================*/
void reg_nv_ehplmn_list_get(reg_sim_plmn_list_s_type* ehplmn_list_efs)
{
   uint32 i;
   
   ehplmn_list_efs->length = reg_nv_ehplmn_efs_list.length;

   for(i = 0; i < reg_nv_ehplmn_efs_list.length; ++i)
   {
     ehplmn_list_efs->plmn[i] = reg_nv_ehplmn_efs_list.plmn[i];
   }
}

#endif

/*==============================================================================

FUNCTION NAME

  reg_nv_max_validate_sim_counter_get
  DESCRIPTION
  This function returns the setting of max validate sim counter value read from NV.
==============================================================================*/
uint8 reg_nv_max_validate_sim_counter_get(void)
{
   MSG_HIGH_DS_1(REG_SUB, "=REG= reg_nv_max_validate_sim_counter value set to %d", reg_nv_max_validate_sim_counter);
   return reg_nv_max_validate_sim_counter;
}

/*===========================================================================
FUNCTION  reg_nv_read_carrier_specific_efs

DESCRIPTION
  This function reads carrier specific EFS/NV values

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void reg_nv_read_carrier_specific_efs(void)
{
  mcfg_fs_status_e_type status = MCFG_FS_STATUS_EFS_ERR;

/* NV item of REG*/
  reg_nv_items_T  reg_nv_items;

  /* IMSI SWITCH */
  reg_timers_pet_watchdog();
  status = mcfg_fs_read("/nv/item_files/modem/nas/imsi_switch", &reg_nv_imsi_switch, sizeof(boolean), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)REG_AS_ID_1);


  if(status != MCFG_FS_STATUS_OK)
  {
    reg_nv_imsi_switch = FALSE;
  }

  MSG_HIGH_DS_2(REG_SUB,"=REG=NV imsi_switch read from EFS with status %d and value is %d",
                         status,reg_nv_imsi_switch);

#ifdef FEATURE_FEMTO_CSG
  reg_timers_pet_watchdog();
  status = mcfg_fs_read("/nv/item_files/modem/nas/csg_support_configuration", &reg_nv_csg_support, sizeof(sys_csg_support_e_type), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)REG_AS_ID_1);

  if(status != MCFG_FS_STATUS_OK)
  {
#ifdef FEATURE_DISABLE_CSG_BYDEFAULT
    reg_nv_csg_support = SYS_CSG_SUPPORT_DISABLED;
#else
    reg_nv_csg_support = SYS_CSG_SUPPORT_WCDMA;
#endif
  }

  MSG_HIGH_DS_2(REG_SUB,"=REG=NV csg_support read from EFS with status %d and value is %d",
                         status,reg_nv_csg_support);
#endif

  reg_timers_pet_watchdog();
  status = mcfg_fs_read("/nv/item_files/modem/nas/override_roaming_status",
                        &reg_nv_override_roam_status,
                        sizeof(boolean),
                        MCFG_FS_TYPE_EFS,
                        (mcfg_fs_sub_id_e_type)reg_sub_id);

  if(status != MCFG_FS_STATUS_OK )
  {
    reg_nv_override_roam_status = FALSE;
  }
  MSG_HIGH_DS_2(REG_SUB, "=REG= NV reg_nv_override_roam_status from EFS with status %d and value is %d",
                status,
                reg_nv_override_roam_status);

#ifdef FEATURE_TDSCDMA
  /* FORCED IRAT */
  reg_timers_pet_watchdog();
  status = mcfg_fs_read("/nv/item_files/modem/nas/forced_irat", &reg_nv_forced_irat_enabled, sizeof(boolean), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)reg_sub_id);


  if(status !=  MCFG_FS_STATUS_OK)
  { 
    reg_nv_forced_irat_enabled = FALSE;    
  }
   
  MSG_HIGH_DS_2(REG_SUB,"=REG=NV reg_nv_forced_irat_enabled from EFS  with status %d and value is %d",
                          status,reg_nv_forced_irat_enabled);

  /* IRAT SEARCH TIMER */
  reg_timers_pet_watchdog();
  status = mcfg_fs_read("/nv/item_files/modem/nas/irat_search_timer", &reg_nv_hplmn_irat_search_timer, sizeof(uint32), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)reg_sub_id);


  MSG_HIGH_DS_2(REG_SUB,"=REG=NV reg_nv_hplmn_irat_search_timer from EFS with status %d and value is %d",
                  status,reg_nv_hplmn_irat_search_timer);    
  if(status !=  MCFG_FS_STATUS_OK)
  { 
    reg_nv_hplmn_irat_search_timer = REG_HPLMN_IRAT_SEARCH_TIMER;   /*default value is 5 minutes*/   
    reg_nv_psm_update_cache_bitmask(reg_nv_psm_nv_list_bitmask,
                                    REG_PSM_EFS_IRAT_SEARCH_TIMER);
  }

  reg_nv_read_preferred_plmn_list();
#endif

  memset( &reg_nv_items, 0x00, sizeof(reg_nv_items_T) );
  reg_nv_is_hplmn_to_be_selected = TRUE;
  reg_nv_is_roaming_on_in_eplmn = FALSE; 
  reg_nv_is_sim_invalid_recovery_enabled = FALSE;
  reg_nv_is_pseudo_reselection_allowed = TRUE;
#if defined (FEATURE_TDSCDMA) || defined (FEATURE_SGLTE)
  reg_nv_lte_search_config.short_lte_scan = 45;
  reg_nv_lte_search_config.long_lte_scan  = 60;
  reg_nv_lte_search_config.max_lte_failures = 128;
  reg_nv_lte_search_config.max_lte_failures_on_neutral_cell = 2;
#endif 
  status = mcfg_fs_read("/nv/item_files/modem/nas/reg_nv_items", &reg_nv_items, sizeof(reg_nv_items_T), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)reg_sub_id);
  if(status == MCFG_FS_STATUS_OK)
  {
    reg_nv_is_hplmn_to_be_selected = reg_nv_items.is_hplmn_has_to_be_selected;
    if(reg_nv_items.version >= 1)
    {
      reg_nv_is_roaming_on_in_eplmn = reg_nv_items.is_roaming_on_in_eplmn;
    }
    if(reg_nv_items.version >= 2)
    {
      reg_nv_is_sim_invalid_recovery_enabled = reg_nv_items.is_sim_invalid_recovery_enabled;
    }
    if(reg_nv_items.version >= 3)
    {
      reg_nv_is_camp_on_2g_if_reg_failure = reg_nv_items.is_camp_on_2g_if_reg_failure;
    }
#if defined (FEATURE_TDSCDMA) || defined (FEATURE_SGLTE)
    if(reg_nv_items.version >=4)
    {
      mcfg_fs_status_e_type status_temp = MCFG_FS_STATUS_EFS_ERR;
      reg_nv_is_pseudo_reselection_allowed = (reg_nv_items.is_pseudo_reselection_allowed > 0) ? TRUE : FALSE;
      MSG_HIGH_DS_1(REG_SUB,"=REG= NV reg_nv_is_pseudo_reselection_allowed set to = %d",reg_nv_is_pseudo_reselection_allowed);
      status_temp = mcfg_fs_read("/nv/item_files/modem/nas/lte_available_timer_values", &reg_nv_lte_search_config, sizeof(lte_search_timer_feature_config_T), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)reg_sub_id);
      reg_timers_pet_watchdog();

      if (status_temp !=  MCFG_FS_STATUS_OK)
      {
          /* Failed to read, setting this value to zero so that default can be 40 sec in the overall*/
          reg_nv_lte_search_config.short_lte_scan = (reg_nv_is_pseudo_reselection_allowed == TRUE) ? 45 : 3;
          reg_nv_lte_search_config.max_lte_failures = (reg_nv_is_pseudo_reselection_allowed == TRUE) ? 128 : 3;
      }
      else
      {
         MSG_HIGH_DS_3(REG_SUB,"=REG= LTE IRAT BPLMN : EFS read Success setting the values: short time %d long time %d max failures %d",reg_nv_lte_search_config.short_lte_scan,reg_nv_lte_search_config.long_lte_scan,reg_nv_lte_search_config.max_lte_failures);
      }
       MSG_HIGH_DS_3(REG_SUB,"=REG= LTE IRAT BPLMN : EFS short time %d long time %d max failures %d ",reg_nv_lte_search_config.short_lte_scan,reg_nv_lte_search_config.long_lte_scan,reg_nv_lte_search_config.max_lte_failures);
       MSG_HIGH_DS_1(REG_SUB,"=REG= LTE IRAT BPLMN : EFS neutral cell failure %d",reg_nv_lte_search_config.max_lte_failures_on_neutral_cell);
    }
#endif
  }
  if( reg_is_qrd_device == TRUE)
  {
     reg_nv_is_sim_invalid_recovery_enabled = TRUE;
  }
   MSG_HIGH_DS_3(REG_SUB, "=REG= NV reg_nv_is_hplmn_to_be_selected  = %d, reg_nv_is_roaming_on_in_eplmn = %d , reg_nv_is_sim_invalid_recovery_enabled = %d",
                reg_nv_is_hplmn_to_be_selected,
                reg_nv_is_roaming_on_in_eplmn,
                reg_nv_is_sim_invalid_recovery_enabled);
   MSG_HIGH_DS_1(REG_SUB,"=REG= NV reg_nv_is_camp_on_2g_if_reg_failure = %d",reg_nv_is_camp_on_2g_if_reg_failure);
}


#ifdef FEATURE_MODEM_CONFIG_REFRESH
/*===========================================================================
FUNCTION  reg_nv_process_mcfg_refresh

DESCRIPTION
  This function refreshes REG specific EFS/NV values for the current subscription

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void reg_nv_process_mcfg_refresh(void)
{
  mcfg_fs_status_e_type status1 = MCFG_FS_STATUS_EFS_ERR, status2 = MCFG_FS_STATUS_EFS_ERR;

  reg_nv_read_carrier_specific_efs();

  /* T3245 TIMER */
  reg_timers_pet_watchdog();
  status1 = mcfg_fs_read("/nv/item_files/modem/nas/t3245_timer", &reg_nv_validate_sim_timer_value,sizeof(rex_timer_cnt_type), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)reg_as_id);


  if(status1 != MCFG_FS_STATUS_OK || reg_nv_validate_sim_timer_value == 0)
  {
    reg_nv_validate_sim_timer_value = REG_TIMERS_MAX_TIMER_VALUE;
  }
  else
  {
    /* Timer value in EFS is provided in seconds convert to milliseconds. */
    reg_nv_validate_sim_timer_value = reg_nv_validate_sim_timer_value * 1000;
  }

  /* MAX VALIDATE SIM COUNTER */
  reg_timers_pet_watchdog();
  status2 = mcfg_fs_read("/nv/item_files/modem/nas/max_validate_sim_counter", &reg_nv_max_validate_sim_counter,sizeof(uint8), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)reg_as_id);

  if(status2 != MCFG_FS_STATUS_OK || reg_nv_validate_sim_timer_value == 0)
  {
    reg_nv_max_validate_sim_counter = REG_NV_DEFAULT_MAX_VALIDATE_SIM_COUNTER;
  }
  MSG_HIGH_DS_4(REG_SUB,"=REG=NV reg_nv_validate_sim_timer_value EFS read status %d. Value is %d(ms), reg_nv_max_validate_sim_counter_sim EFS read status %d. Value is %d(ms).Refresh Done",
                                   status1,reg_nv_validate_sim_timer_value,
                                   status2,reg_nv_max_validate_sim_counter);
}
#endif

/*===========================================================================
FUNCTION  reg_nv_read_psm_config

DESCRIPTION
  This function reads the PSM configuration and also checks if the boot up
  is due to PSM exit

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void reg_nv_read_psm_config(void)
{
  mcfg_fs_status_e_type status = MCFG_FS_STATUS_EFS_ERR;
  cm_psm_config_s_type  psm_config;
  boolean psm_btw_edrx_enabled = FALSE;

  memset((void*)&psm_config, 0, sizeof(cm_psm_config_s_type));

  /* NV 73769 */
  reg_timers_pet_watchdog();
  status = mcfg_fs_read("/nv/item_files/modem/mmode/psm_config", 
                          &psm_config,sizeof(cm_psm_config_s_type),
                           MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)reg_as_id);

  if(status != MCFG_FS_STATUS_OK)
  {
    reg_nv_psm_r12_enabled = FALSE; /* Default value disabled */
	// Enable by default as MMODE as enabled by default
	psm_config.is_enabled = TRUE;
    psm_config.psm_version = CM_PSM_VERSION_MASK_ALL;
  }
  if(psm_config.is_enabled == TRUE) /* Check for overall PSM feature config */
  {
    reg_nv_psm_r12_enabled = (((psm_config.psm_version 
                                  & (uint8)CM_PSM_VERSION_MASK_REL12_WO_NW_CONTEXT_RETENTION ) != 0)
                             ||
                             ((psm_config.psm_version 
                                  & (uint8)CM_PSM_VERSION_MASK_WITH_CONTEXT_RETENTION ) != 0))
                            ? TRUE
                            : FALSE;

    psm_btw_edrx_enabled = ((psm_config.psm_version 
                               & (uint8)CM_PSM_VERSION_MASK_PSM_BTW_EDRX_CYCLE ) != 0)
                           ? TRUE
                           : FALSE;

    if (((psm_config.psm_version & (uint8)CM_PSM_VERSION_MASK_WITH_CONTEXT_RETENTION ) != 0)
          ||
        (psm_btw_edrx_enabled == TRUE))
    {
      reg_nv_psm_nv_list_bitmask = 0xFFFFFFFF;
    }
  }
  else
  {
    reg_nv_psm_r12_enabled = FALSE;
  }

  if(reg_get_ulog_handle() != NULL)
  {
    ULOG_RT_PRINTF_6(reg_get_ulog_handle(),
    "=REG= EFS psm_config read: status = %d, overall_psm_enabled = %d, psm_r12_enabled = %d, psm_version = 0x%x, reg_task_power_up_init = %d, psm_btw_edrx_enabled %d",
    status, psm_config.is_enabled, reg_nv_psm_r12_enabled, psm_config.psm_version, reg_task_power_up_init, psm_btw_edrx_enabled);
  }

  MSG_HIGH_DS_6(REG_SUB, "=REG= EFS psm_config read: status = %d, overall_psm_enabled = %d, psm_r12_enabled = %d, psm_version = 0x%x, reg_task_power_up_init = %d, psm_btw_edrx_enabled %d",
                         status, psm_config.is_enabled, reg_nv_psm_r12_enabled, psm_config.psm_version, reg_task_power_up_init, psm_btw_edrx_enabled);

}

/*===========================================================================
FUNCTION  reg_nv_psm_update_cache_bitmask

DESCRIPTION
  This function clears the bit in the bitmask for the specified 
  NV/EFS in PSM cache structure. This function is called when
  NV/EFS read failed during PSM restore that was written while
  UE entered PSM.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void reg_nv_psm_update_cache_bitmask
(
  uint32 bitmask,
  reg_nv_list_psm_e_type NV_EFS
)
{
  REG_CLEAR_BM_32BIT(reg_nv_psm_nv_list_bitmask, NV_EFS);
  
}

/*===========================================================================
FUNCTION  reg_nv_psm_write_cache

DESCRIPTION
  This function saves the PSM cache in EFS

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/
void reg_nv_psm_write_cache(reg_nv_psm_cache_s_type *psm_cache_ptr)
{
  mcfg_fs_status_e_type status = MCFG_FS_STATUS_EFS_ERR;

  if(reg_nv_psm_cache_ptr != NULL)
  {
    memscpy(psm_cache_ptr, sizeof(reg_nv_psm_cache_s_type),
                       reg_nv_psm_cache_ptr, sizeof(reg_nv_psm_cache_s_type));

    modem_mem_free(reg_nv_psm_cache_ptr, MODEM_MEM_CLIENT_NAS);
    reg_nv_psm_cache_ptr = NULL;
  }
  
}

/*===========================================================================
FUNCTION  reg_nv_save_psm_context

DESCRIPTION
  This function saves REG specific EFS/NV values upon entering PSM

DEPENDENCIES
  None

RETURN VALUE
  TRUE: EFS/NV written successfully, FALSE otherwise

SIDE EFFECTS
  None
===========================================================================*/
boolean reg_nv_save_psm_context(sys_plmn_service_state_s_type *service_status)
{
  boolean result = TRUE;

  reg_nv_psm_cache_ptr = (reg_nv_psm_cache_s_type *)modem_mem_alloc(sizeof(reg_nv_psm_cache_s_type), MODEM_MEM_CLIENT_NAS);
  reg_check_for_null_ptr((void*)reg_nv_psm_cache_ptr);
  memset((void*)reg_nv_psm_cache_ptr, 0, sizeof(reg_nv_psm_cache_s_type));

  reg_nv_psm_cache_ptr->timers_timestamp = time_get_ms_native();
  result = (result && reg_nv_write_cache()) ? TRUE : FALSE;
#ifdef FEATURE_LTE
  if( ds3gpp_is_plmn_blocked_force_lte_pref((sys_modem_as_id_e_type)reg_sub_id))
  {
    reg_sim_write_rplmn_rat(SYS_RAT_NONE);
  }
#endif

#if defined (FEATURE_TDSCDMA) || defined (FEATURE_SGLTE)
  result = (result && reg_nv_write_lte_lists_to_efs()) ? TRUE : FALSE;
#endif

#ifdef FEATURE_DUAL_SIM
  if(reg_is_t3245_timer_active((sys_modem_as_id_e_type)reg_as_id))
#else
  if(reg_is_t3245_timer_active())
#endif
  {
    result = (result && reg_nv_write_t3245_to_efs(reg_timers_get_rem_t3245_value()))
             ? TRUE : FALSE;
  }

  reg_nv_psm_cache_ptr->plmn = service_status->plmn;
  reg_nv_psm_cache_ptr->rat = service_status->active_rat;
  reg_nv_psm_cache_ptr->sim_state = service_status->sim_state;
#ifdef FEATURE_LTE
  if(reg_sim_backoff_fplmn_list.length > 0)
  {
    reg_nv_psm_cache_ptr->backoff_fplmn_list = reg_sim_backoff_fplmn_list;
    reg_nv_psm_cache_ptr->backoff_fplmn_rem_duration = 
             reg_timers_get_rem_backoff_fplmn_search_timer_value(reg_as_id);
  }
#endif

  if(reg_sim_gprs_fplmn_list.length > 0)
  {
    reg_nv_psm_cache_ptr->gprs_fplmn_list = reg_sim_gprs_fplmn_list;
  }
  if (reg_sim_dos_mitigation_fplmn_list.length > 0)
  {
    reg_nv_psm_cache_ptr->dos_mitigation_fplmn_list= reg_sim_dos_mitigation_fplmn_list;
  }

  reg_timers_psm_save_hplmn_search_timer();
#ifdef FEATURE_CIOT
  reg_timers_psm_save_ciot_irat_timer();
  reg_timers_psm_save_ciot_nb1_interleave_timers();
#endif

  return result;
}

/*===========================================================================
FUNCTION  reg_nv_psm_update_restore_status

DESCRIPTION
  This function updates the PSM restore status to failed based on the
  NV/EFS read status

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void reg_nv_psm_update_restore_status(void)
{
  reg_nv_list_psm_e_type NV_EFS = 0;
#ifdef FEATURE_DUAL_SIM 
  for(reg_as_id = REG_AS_ID_1; reg_as_id < (reg_as_id_e_type)MAX_NAS_STACKS; reg_as_id++ )
#endif
  {
    if(reg_nv_psm_cache_ptr != NULL)
    {
      
      MSG_HIGH_DS_2(REG_SUB, "=REG= Stored Mask %d  Current Mask = %d", reg_nv_psm_cache_ptr->psm_nv_list_bitmask, 
                                                                         reg_nv_psm_nv_list_bitmask);
	  
      if(reg_get_ulog_handle() != NULL)
      {
         ULOG_RT_PRINTF_2(reg_get_ulog_handle(),"=REG= Stored Mask %d  Current Mask = %d", reg_nv_psm_cache_ptr->psm_nv_list_bitmask, 
                                                                                         reg_nv_psm_nv_list_bitmask);
      }
      for(NV_EFS = 0; NV_EFS < REG_PSM_NV_EFS_MAX; NV_EFS++)
      {
        if((REG_CHECK_BM_32BIT(reg_nv_psm_cache_ptr->psm_nv_list_bitmask, NV_EFS) == TRUE) &&
           (REG_CHECK_BM_32BIT(reg_nv_psm_nv_list_bitmask, NV_EFS) == FALSE))
        {
          reg_nv_psm_restore_pending = FALSE;
          reg_nv_psm_free_cache();
          if(reg_get_ulog_handle() != NULL)
          {
            ULOG_RT_PRINTF_0(reg_get_ulog_handle(), "=REG= Error: PSM restore failed");
          }
          break;
        }
      }
    }
  }
#ifdef FEATURE_DUAL_SIM 
  reg_as_id = REG_AS_ID_1;
#endif
}

/*===========================================================================
FUNCTION  reg_nv_psm_free_cache

DESCRIPTION
  This function frees up the memory allocated for psm cache

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void reg_nv_psm_free_cache(void)
{
  if(reg_nv_psm_cache_ptr != NULL)
  {
    modem_mem_free(reg_nv_psm_cache_ptr, MODEM_MEM_CLIENT_NAS);
    reg_nv_psm_cache_ptr = NULL;
  }
}

/*===========================================================================
FUNCTION  reg_nv_per_subs_psm_restore_pending

DESCRIPTION
  This function checks if the PSM restore is pending

DEPENDENCIES
  None

RETURN VALUE
  TRUE if PSM restore is pending, else FALSE

SIDE EFFECTS
  None
===========================================================================*/
boolean reg_nv_per_subs_psm_restore_pending
(
  sys_modem_as_id_e_type        as_id,
  sys_plmn_id_s_type            plmn,
  sys_radio_access_tech_e_type  rat,
  sys_sim_state_e_type          sim_state
)
{
  boolean status = FALSE;
#ifdef FEATURE_DUAL_SIM
  if((reg_nv_psm_restore_pending_sim[as_id] == TRUE) &&
     (reg_nv_psm_cache_ptr_sim[as_id] != NULL) &&
     sys_plmn_match(reg_nv_psm_cache_ptr_sim[as_id]->plmn, plmn) &&
     (reg_nv_psm_cache_ptr_sim[as_id]->rat == rat) &&
     (reg_nv_psm_cache_ptr_sim[as_id]->sim_state == sim_state))
  {
    status = TRUE;
  }
#else
  (void)as_id;
  if((reg_nv_psm_restore_pending == TRUE) && 
     (reg_nv_psm_cache_ptr != NULL) &&
     sys_plmn_match(reg_nv_psm_cache_ptr->plmn, plmn) &&
     (reg_nv_psm_cache_ptr->rat == rat) &&
     (reg_nv_psm_cache_ptr->sim_state == sim_state))
  {
    status = TRUE;
  }
#endif
  return status;
}


void reg_read_sim_available_nv()
{

  mcfg_fs_status_e_type status = MCFG_FS_STATUS_EFS_ERR;
#if defined(FEATURE_LTE) && defined(FEATURE_MMSS_3_1)
  byte ignore_uplmn;
#endif

  if (reg_task_sim_available == TRUE)
  {
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE 
  for ( reg_as_id=REG_AS_ID_1; reg_as_id < (reg_as_id_e_type)MAX_NAS_STACKS; reg_as_id++ )
#endif
  {
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE 
    reg_sub_id = reg_sub_id_stack[reg_as_id];
#endif

    /*
    ** Fill NV command to read equivalent PLMN list.
    */
    if (reg_ghdi_nvmem_read(NV_EQUIVALENT_PLMN_LIST_I,
       (ghdi_nvmem_data_T *)&reg_nv_equivalent_plmn_list) != GHDI_SUCCESS)
    {  
      reg_nv_equivalent_plmn_list.rplmn.identity[0] = 0xFF;
      reg_nv_equivalent_plmn_list.rplmn.identity[1] = 0xFF;
      reg_nv_equivalent_plmn_list.rplmn.identity[2] = 0xFF;
      
      reg_nv_equivalent_plmn_list.length = 0;
  
      MSG_HIGH_DS_0(REG_SUB, "=REG= Reading equivalent PLMN list failed");
      reg_nv_psm_update_cache_bitmask(reg_nv_psm_nv_list_bitmask,
                                      REG_PSM_NV_EQUIVALENT_PLMN_LIST);
    } 
    else
    {
      MSG_HIGH_DS_0(REG_SUB, "=REG= Read equivalent PLMN list");
  
      /*Read the EPLMN extended list if length of EPLMNs is greater than 5*/
      if(reg_nv_equivalent_plmn_list.length > NV_EQUIVALENT_PLMN_LIST_MAX_SIZE)
      {
         reg_nv_equivalent_plmn_list.length = MIN(reg_nv_equivalent_plmn_list.length, NV_EQUIVALENT_PLMN_LIST_MAX_SIZE + NV_EXTENDED_EQUIVALENT_PLMN_LIST_MAX_SIZE);
         /*
         ** Fill NV command to read extended equivalent PLMN list.
         */
         if (reg_ghdi_nvmem_read(NV_EXTENDED_EQUIVALENT_PLMN_LIST_I,
             (ghdi_nvmem_data_T *)&reg_nv_extended_equivalent_plmn_list) != GHDI_SUCCESS)
         {
            MSG_HIGH_DS_0(REG_SUB, "=REG= Reading extended equivalent PLMN list failed");
            reg_nv_psm_update_cache_bitmask(reg_nv_psm_nv_list_bitmask,
                                            REG_PSM_NV_EXTENDED_EQUIVALENT_PLMN_LIST);
         } 
         else
         {
           MSG_HIGH_DS_0(REG_SUB, "=REG= Read extended equivalent PLMN list");
         }
      }
      else
      {
         MSG_HIGH_DS_1(REG_SUB, "=REG= No need to read extended EPLMN list as length is %d", reg_nv_equivalent_plmn_list.length);
      }
      reg_nv_data_valid = TRUE;    
    }
  }

  /* IGNORE UPLMN */
#if defined(FEATURE_LTE) && defined(FEATURE_MMSS_3_1)
  reg_timers_pet_watchdog();
  reg_nv_ignore_uplmn = FALSE;
  status = mcfg_fs_read("/nv/item_files/modem/nas/ignore_uplmn", &ignore_uplmn, sizeof(boolean), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)reg_sub_id);
  if(status  == MCFG_FS_STATUS_OK && ignore_uplmn >= 1)              
  {
    reg_nv_ignore_uplmn = TRUE;
  }

  MSG_HIGH_DS_2(REG_SUB,"=REG= NV ignore_uplmn read from SIM Ignore UPLMN = %d status = %d",reg_nv_ignore_uplmn, status);
#endif

  status = mcfg_fs_read("/nv/item_files/modem/mmode/qmss_enabled", &reg_is_qrd_device, sizeof(boolean), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)REG_AS_ID_1);
                                                
    if( status == MCFG_FS_STATUS_OK)
    {
      MSG_HIGH_DS_1(REG_SUB,"=REG= setting QRD device setting to %d",reg_is_qrd_device);
    }
    else
    {
      MSG_HIGH_DS_1(REG_SUB,"=REG= setting QRD device to false as read failed status = %d",status);
      reg_is_qrd_device = FALSE;
    }

  if( reg_is_qrd_device == TRUE)
    {
 #ifdef FEATURE_DUAL_SIM
      for ( reg_as_id=REG_AS_ID_1; reg_as_id < (reg_as_id_e_type)MAX_NAS_STACKS; reg_as_id++ )
 #endif
      {
        status = reg_nv_read_vplmn_list();                      
 
        if( status == MCFG_FS_STATUS_OK)
        {
          MSG_HIGH_DS_1(REG_SUB, "=REG= NV read VPLMN success Length of VPLMN = %d", reg_nv_get_vplmn_list_length());
        }
        else
        {
           MSG_HIGH_DS_1(REG_SUB, "=REG= NV read VPLMN Falied status = %d", status);
        }
      }
#ifdef FEATURE_DUAL_SIM
     reg_as_id=REG_AS_ID_1;
     reg_sub_id = reg_sub_id_stack[reg_as_id];
#endif

    }

#ifdef FEATURE_DUAL_SIM

   status = mcfg_fs_read("/nv/item_files/modem/nas/t3245_timer_stored", &reg_nv_t3245_timer_stored_value_sim[REG_AS_ID_1], sizeof(rex_timer_cnt_type), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)REG_AS_ID_1);

   if(status != MCFG_FS_STATUS_OK)
   {
     reg_nv_t3245_timer_stored_value_sim[REG_AS_ID_1] = REG_T3245_DEFUALT_VALUE;
     reg_nv_psm_update_cache_bitmask(reg_nv_psm_nv_list_bitmask,
                                     REG_PSM_EFS_T3245_TIMER_STORED);
   }
   MSG_HIGH_DS_2(REG_SUB, "=REG= NV reg_nv_t3245_timer_stored_value from EFS  with status %d and value is %d for SIM1",
                 status,
                 reg_nv_t3245_timer_stored_value_sim[REG_AS_ID_1]);
   
   status = mcfg_fs_read("/nv/item_files/modem/nas/t3245_timer_stored", &reg_nv_t3245_timer_stored_value_sim[REG_AS_ID_2], sizeof(rex_timer_cnt_type), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)REG_AS_ID_2);

   if(status != MCFG_FS_STATUS_OK)
   {
     reg_nv_t3245_timer_stored_value_sim[REG_AS_ID_2] = REG_T3245_DEFUALT_VALUE;
     reg_sub_id = REG_AS_ID_2;
     reg_nv_psm_update_cache_bitmask(reg_nv_psm_nv_list_bitmask,
                                     REG_PSM_EFS_T3245_TIMER_STORED);
     reg_sub_id = REG_AS_ID_1;
   }
   MSG_HIGH_DS_2(REG_SUB, "=REG= NV reg_nv_t3245_timer_stored_value from EFS with status %d and value is %d for SIM2",
                 status,
                 reg_nv_t3245_timer_stored_value_sim[REG_AS_ID_2]);

#ifdef FEATURE_TRIPLE_SIM
   status = mcfg_fs_read("/nv/item_files/modem/nas/t3245_timer_stored", &reg_nv_t3245_timer_stored_value_sim[REG_AS_ID_3], sizeof(rex_timer_cnt_type), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)REG_AS_ID_3);


   if(status != MCFG_FS_STATUS_OK)
   {
     reg_nv_t3245_timer_stored_value_sim[REG_AS_ID_3] = REG_T3245_DEFUALT_VALUE;
     reg_sub_id = REG_AS_ID_3;
     reg_nv_psm_update_cache_bitmask(reg_nv_psm_nv_list_bitmask,
                                     REG_PSM_EFS_T3245_TIMER_STORED);
     reg_sub_id = REG_AS_ID_1;
   }
   MSG_HIGH_DS_2(REG_SUB, "=REG= NV reg_nv_t3245_timer_stored_value from EFS with status %d and value is %d for SIM3",
                 status,
                 reg_nv_t3245_timer_stored_value_sim[REG_AS_ID_3]);
#endif

	  status = mcfg_fs_read("/nv/item_files/modem/nas/timestamp_at_power_off", &reg_nv_timestamp_at_power_off_sim[REG_AS_ID_1], sizeof(uint64), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)REG_AS_ID_1);
   
	  if(status != MCFG_FS_STATUS_OK)
	  {
		reg_nv_timestamp_at_power_off_sim[REG_AS_ID_1] = 0;
	  }
	  MSG_HIGH_DS_2(REG_SUB, "=REG= NV timestamp_at_power_off from EFS  with status %d and value is %d for SIM1",
					status,
					reg_nv_timestamp_at_power_off_sim[REG_AS_ID_1]);
	  
	  status = mcfg_fs_read("/nv/item_files/modem/nas/timestamp_at_power_off", &reg_nv_timestamp_at_power_off_sim[REG_AS_ID_2], sizeof(uint64), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)REG_AS_ID_2);
   
	  if(status != MCFG_FS_STATUS_OK)
	  {
		reg_nv_timestamp_at_power_off_sim[REG_AS_ID_2] = 0;
	  }
	  MSG_HIGH_DS_2(REG_SUB, "=REG= NV timestamp_at_power_off from EFS with status %d and value is %d for SIM2",
					status,
					reg_nv_timestamp_at_power_off_sim[REG_AS_ID_2]);
   
#ifdef FEATURE_TRIPLE_SIM
	  status = mcfg_fs_read("/nv/item_files/modem/nas/timestamp_at_power_off", &reg_nv_timestamp_at_power_off_sim[REG_AS_ID_3], sizeof(uint64), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)REG_AS_ID_3);
   
   
	  if(status != MCFG_FS_STATUS_OK)
	  {
		reg_nv_timestamp_at_power_off_sim[REG_AS_ID_3] = 0;
	  }
	  MSG_HIGH_DS_2(REG_SUB, "=REG= NV reg_nv_t3245_timer_stored_value from EFS with status %d and value is %d for SIM3",
					status,
					reg_nv_timestamp_at_power_off_sim[REG_AS_ID_3]);
#endif

#ifdef FEATURE_CIOT
		 status = mcfg_fs_read("/nv/item_files/modem/nas/ciot_irat_timer_at_power_off", &reg_nv_ciot_irat_timer_at_power_off_sim[REG_AS_ID_1], sizeof(uint64), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)REG_AS_ID_1);
	  
		 if(status != MCFG_FS_STATUS_OK)
		 {
		   reg_nv_ciot_irat_timer_at_power_off_sim[REG_AS_ID_1] = REG_CIOT_IRAT_TIMER_DEFAULT_VALUE;
		 }
		 MSG_HIGH_DS_2(REG_SUB, "=REG= NV reg_nv_ciot_irat_timer_at_power_off_sim from EFS  with status %d and value is %d for SIM1",
					   status,
					   reg_nv_ciot_irat_timer_at_power_off_sim[REG_AS_ID_1]);
		 
		 status = mcfg_fs_read("/nv/item_files/modem/nas/ciot_irat_timer_at_power_off", &reg_nv_ciot_irat_timer_at_power_off_sim[REG_AS_ID_2], sizeof(uint64), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)REG_AS_ID_2);
	  
		 if(status != MCFG_FS_STATUS_OK)
		 {
		   reg_nv_ciot_irat_timer_at_power_off_sim[REG_AS_ID_2] = REG_CIOT_IRAT_TIMER_DEFAULT_VALUE;
		 }
		 MSG_HIGH_DS_2(REG_SUB, "=REG= NV reg_nv_ciot_irat_timer_at_power_off_sim from EFS with status %d and value is %d for SIM2",
					   status,
					   reg_nv_ciot_irat_timer_at_power_off_sim[REG_AS_ID_2]);
	  
#ifdef FEATURE_TRIPLE_SIM
		 status = mcfg_fs_read("/nv/item_files/modem/nas/ciot_irat_timer_at_power_off", &reg_nv_ciot_irat_timer_at_power_off_sim[REG_AS_ID_3], sizeof(uint64), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)REG_AS_ID_3);
	  
	  
		 if(status != MCFG_FS_STATUS_OK)
		 {
		   reg_nv_ciot_irat_timer_at_power_off_sim[REG_AS_ID_3] = REG_CIOT_IRAT_TIMER_DEFAULT_VALUE;
		 }
		 MSG_HIGH_DS_2(REG_SUB, "=REG= NV reg_nv_ciot_irat_timer_at_power_off_sim from EFS with status %d and value is %d for SIM3",
					   status,
					   reg_nv_ciot_irat_timer_at_power_off_sim[REG_AS_ID_3]);
#endif
#endif

   status = mcfg_fs_read("/nv/item_files/modem/nas/t3245_timer", &reg_nv_validate_sim_timer_value_sim[REG_AS_ID_1], sizeof(rex_timer_cnt_type), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)REG_AS_ID_1);

   if(status != MCFG_FS_STATUS_OK || reg_nv_validate_sim_timer_value_sim[REG_AS_ID_1] == 0)
   {
     reg_nv_validate_sim_timer_value_sim[REG_AS_ID_1] = REG_TIMERS_MAX_TIMER_VALUE;
   }
   else
   {
     //Timer value in EFS is provided in seconds convert to milliseconds.
     reg_nv_validate_sim_timer_value_sim[REG_AS_ID_1] = reg_nv_validate_sim_timer_value_sim[REG_AS_ID_1] * 1000;
   }
   MSG_HIGH_DS_2(REG_SUB, "=REG= NV reg_nv_validate_sim_timer_value from EFS  with status %d and value is %d(ms) for SIM1",
                 status,
                 reg_nv_validate_sim_timer_value_sim[REG_AS_ID_1]);
				  
   status = mcfg_fs_read("/nv/item_files/modem/nas/t3245_timer", &reg_nv_validate_sim_timer_value_sim[REG_AS_ID_2], sizeof(rex_timer_cnt_type), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)REG_AS_ID_2);
   
   if(status != MCFG_FS_STATUS_OK || reg_nv_validate_sim_timer_value_sim[REG_AS_ID_2] == 0)
   {
     reg_nv_validate_sim_timer_value_sim[REG_AS_ID_2] = REG_TIMERS_MAX_TIMER_VALUE;
   }
   else
   {
     //Timer value in EFS is provided in seconds convert to milliseconds.
     reg_nv_validate_sim_timer_value_sim[REG_AS_ID_2] = reg_nv_validate_sim_timer_value_sim[REG_AS_ID_2] * 1000;
   }
   MSG_HIGH_DS_2(REG_SUB, "=REG= NV reg_nv_validate_sim_timer_value from EFS with status %d and value is %d(ms) for SIM2",
                 status,
                 reg_nv_validate_sim_timer_value_sim[REG_AS_ID_2]);
   
#ifdef FEATURE_TRIPLE_SIM
   status = mcfg_fs_read("/nv/item_files/modem/nas/t3245_timer", &reg_nv_validate_sim_timer_value_sim[REG_AS_ID_3], sizeof(rex_timer_cnt_type), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)REG_AS_ID_3);


   if(status != MCFG_FS_STATUS_OK || reg_nv_validate_sim_timer_value_sim[REG_AS_ID_3] == 0)
   {
     reg_nv_validate_sim_timer_value_sim[REG_AS_ID_3] = REG_TIMERS_MAX_TIMER_VALUE;
   }
   else
   {
     //Timer value in EFS is provided in seconds convert to milliseconds.
     reg_nv_validate_sim_timer_value_sim[REG_AS_ID_3] = reg_nv_validate_sim_timer_value_sim[REG_AS_ID_3] * 1000;
   }
   MSG_HIGH_DS_2(REG_SUB, "=REG= NV reg_nv_validate_sim_timer_value from EFS with status %d and value is %d(ms) for SIM3",
                 status, 
                 reg_nv_validate_sim_timer_value_sim[REG_AS_ID_3]);
#endif
   status = mcfg_fs_read("/nv/item_files/modem/nas/max_validate_sim_counter", &reg_nv_max_validate_sim_counter_sim[REG_AS_ID_1], sizeof(uint8), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)REG_AS_ID_1);

   if(status != MCFG_FS_STATUS_OK || reg_nv_max_validate_sim_counter_sim[REG_AS_ID_1] == 0)
   {
     reg_nv_max_validate_sim_counter_sim[REG_AS_ID_1] = REG_NV_DEFAULT_MAX_VALIDATE_SIM_COUNTER;
   }

   MSG_HIGH_DS_2(REG_SUB, "=REG= NV reg_nv_max_validate_sim_counter_sim from EFS  with status %d and value is %d for SIM1",
                 status,
                 reg_nv_max_validate_sim_counter_sim[REG_AS_ID_1]);
   status = mcfg_fs_read("/nv/item_files/modem/nas/max_validate_sim_counter", &reg_nv_max_validate_sim_counter_sim[REG_AS_ID_2], sizeof(uint8), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)REG_AS_ID_2);
   
   if(status != MCFG_FS_STATUS_OK || reg_nv_max_validate_sim_counter_sim[REG_AS_ID_2] == 0)
   {
     reg_nv_max_validate_sim_counter_sim[REG_AS_ID_2] = REG_NV_DEFAULT_MAX_VALIDATE_SIM_COUNTER;
   }
   MSG_HIGH_DS_2(REG_SUB, "=REG= NV reg_nv_max_validate_sim_counter_sim from EFS with status %d and value is %d for SIM2",
                 status,
                 reg_nv_max_validate_sim_counter_sim[REG_AS_ID_2]);
   
#ifdef FEATURE_TRIPLE_SIM
   status = mcfg_fs_read("/nv/item_files/modem/nas/max_validate_sim_counter", &reg_nv_max_validate_sim_counter_sim[REG_AS_ID_3], sizeof(uint8), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)REG_AS_ID_3);

   if(status != MCFG_FS_STATUS_OK || reg_nv_max_validate_sim_counter_sim[REG_AS_ID_3] == 0)
   {
     reg_nv_max_validate_sim_counter_sim[REG_AS_ID_3] = REG_NV_DEFAULT_MAX_VALIDATE_SIM_COUNTER;
   }
   MSG_HIGH_DS_2(REG_SUB, "=REG= NV reg_nv_max_validate_sim_counter_sim from EFS with status %d and value is %d for SIM3",
                 status,
                 reg_nv_max_validate_sim_counter_sim[REG_AS_ID_3]);
#endif
   status = mcfg_fs_read("/nv/item_files/modem/nas/t3245_timer_test", &reg_nv_t3245_test_timer_value_sim[REG_AS_ID_1], sizeof(rex_timer_cnt_type), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)REG_AS_ID_1);

   if(status != MCFG_FS_STATUS_OK || reg_nv_t3245_test_timer_value_sim[REG_AS_ID_1] == 0)
   {
     reg_nv_t3245_test_timer_value_sim[REG_AS_ID_1] = REG_TIMERS_MAX_TIMER_VALUE;
   }
   else
   {
     //Timer value in EFS is provided in seconds convert to milliseconds.
     reg_nv_t3245_test_timer_value_sim[REG_AS_ID_1] = reg_nv_t3245_test_timer_value_sim[REG_AS_ID_1] * 1000;
   }
   MSG_HIGH_DS_2(REG_SUB, "=REG= NV reg_nv_t3245_test_timer_value from EFS  with status %d and value is %d(ms) for SIM1",
                 status,
                 reg_nv_t3245_test_timer_value_sim[REG_AS_ID_1]);
   status = mcfg_fs_read("/nv/item_files/modem/nas/t3245_timer_test", &reg_nv_t3245_test_timer_value_sim[REG_AS_ID_2], sizeof(rex_timer_cnt_type), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)REG_AS_ID_2);
   
   if(status != MCFG_FS_STATUS_OK || reg_nv_t3245_test_timer_value_sim[REG_AS_ID_2] == 0)
   {
     reg_nv_t3245_test_timer_value_sim[REG_AS_ID_2] = REG_TIMERS_MAX_TIMER_VALUE;
   }
   else
   {
     //Timer value in EFS is provided in seconds convert to milliseconds.
     reg_nv_t3245_test_timer_value_sim[REG_AS_ID_2] = reg_nv_t3245_test_timer_value_sim[REG_AS_ID_2] * 1000;
   }
   MSG_HIGH_DS_2(REG_SUB, "=REG= NV reg_nv_t3245_test_timer_value from EFS with status %d and value is %d(ms) for SIM2",
                 status,
                 reg_nv_t3245_test_timer_value_sim[REG_AS_ID_2]);

#ifdef FEATURE_TRIPLE_SIM
   status = mcfg_fs_read("/nv/item_files/modem/nas/t3245_timer_test", &reg_nv_t3245_test_timer_value_sim[REG_AS_ID_3], sizeof(rex_timer_cnt_type), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)REG_AS_ID_3);

   if(status != MCFG_FS_STATUS_OK || reg_nv_t3245_test_timer_value_sim[REG_AS_ID_3] == 0)
   {
     reg_nv_t3245_test_timer_value_sim[REG_AS_ID_3] = REG_TIMERS_MAX_TIMER_VALUE;
   }
   else
   {
     //Timer value in EFS is provided in seconds convert to milliseconds.
     reg_nv_t3245_test_timer_value_sim[REG_AS_ID_3] = reg_nv_t3245_test_timer_value_sim[REG_AS_ID_3] * 1000;
   }
   MSG_HIGH_DS_2(REG_SUB, "=REG= NV reg_nv_t3245_test_timer_value from EFS with status %d and value is %d(ms) for SIM3",
                 status,
                 reg_nv_t3245_test_timer_value_sim[REG_AS_ID_3]);
#endif

#else
   status = mcfg_fs_read("/nv/item_files/modem/nas/t3245_timer_stored", &reg_nv_t3245_timer_stored_value, sizeof(rex_timer_cnt_type), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)REG_AS_ID_1);

   if(status != MCFG_FS_STATUS_OK)
   {
     reg_nv_t3245_timer_stored_value = REG_T3245_DEFUALT_VALUE;
     reg_nv_psm_update_cache_bitmask(reg_nv_psm_nv_list_bitmask,
                                     REG_PSM_EFS_T3245_TIMER_STORED);
   }
   MSG_HIGH_DS_2(REG_SUB, "=REG= NV reg_nv_t3245_timer_stored_value from EFS  with status %d and value is %d",
              status,
              reg_nv_t3245_timer_stored_value);

   status = mcfg_fs_read("/nv/item_files/modem/nas/timestamp_at_power_off", &reg_nv_timestamp_at_power_off, sizeof(uint64), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)REG_AS_ID_1);
   
   if(status != MCFG_FS_STATUS_OK)
   {
     reg_nv_timestamp_at_power_off = 0;
   }
	  MSG_HIGH_DS_2(REG_SUB, "=REG= NV reg_nv_t3245_timer_stored_value from EFS  with status %d and value is %d",
				 status,
				 reg_nv_timestamp_at_power_off);
				 

#ifdef FEATURE_CIOT
   status = mcfg_fs_read("/nv/item_files/modem/nas/ciot_irat_timer_at_power_off", &reg_nv_ciot_irat_timer_at_power_off, sizeof(uint64), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)REG_AS_ID_1);
	
	if(status != MCFG_FS_STATUS_OK)
	{
	  reg_nv_ciot_irat_timer_at_power_off = REG_CIOT_IRAT_TIMER_DEFAULT_VALUE;
	}
	   MSG_HIGH_DS_2(REG_SUB, "=REG= NV reg_nv_ciot_irat_timer_at_power_off from EFS  with status %d and value is %d",
				  status,
				  reg_nv_ciot_irat_timer_at_power_off);
#endif

   status = mcfg_fs_read("/nv/item_files/modem/nas/t3245_timer", &reg_nv_validate_sim_timer_value, sizeof(rex_timer_cnt_type), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)REG_AS_ID_1);

    if(status != MCFG_FS_STATUS_OK || reg_nv_validate_sim_timer_value == 0)
   {
     reg_nv_validate_sim_timer_value = REG_TIMERS_MAX_TIMER_VALUE;
   }
    else
   {
     //Timer value in EFS is provided in seconds convert to milliseconds.
     reg_nv_validate_sim_timer_value = reg_nv_validate_sim_timer_value * 1000;
   }
   MSG_HIGH_DS_2(REG_SUB, "=REG= NV reg_nv_validate_sim_timer_value from EFS  with status %d and value is %d(ms)",
                 status,
                 reg_nv_validate_sim_timer_value);
   status = mcfg_fs_read("/nv/item_files/modem/nas/max_validate_sim_counter", &reg_nv_max_validate_sim_counter, sizeof(uint8), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)REG_AS_ID_1);

   if(status != MCFG_FS_STATUS_OK)
   {
     reg_nv_max_validate_sim_counter = REG_NV_DEFAULT_MAX_VALIDATE_SIM_COUNTER;
   }
   MSG_HIGH_DS_2(REG_SUB, "=REG= NV reg_nv_max_validate_sim_counter from EFS  with status %d and value is %d",
                 status,
                 reg_nv_max_validate_sim_counter);

   status = mcfg_fs_read("/nv/item_files/modem/nas/t3245_timer_test", &reg_nv_t3245_test_timer_value, sizeof(rex_timer_cnt_type), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)REG_AS_ID_1);

   if(status != MCFG_FS_STATUS_OK || reg_nv_t3245_test_timer_value == 0)
   {
     reg_nv_t3245_test_timer_value = REG_TIMERS_MAX_TIMER_VALUE;
   }
    else
   {
     //Timer value in EFS is provided in seconds convert to milliseconds.
     reg_nv_t3245_test_timer_value = reg_nv_t3245_test_timer_value * 1000;
   }
   MSG_HIGH_DS_2(REG_SUB, "=REG= NV reg_nv_t3245_test_timer_value from EFS  with status %d and value is %d",
                 status,
                 reg_nv_t3245_test_timer_value);
#endif

#ifdef FEATURE_CIOT
    reg_nv_read_ciot_params();
#endif


  reg_task_sim_available = FALSE;
  }
}



#ifdef FEATURE_CIOT

/*===========================================================================

FUNCTION reg_nv_interlace_scan_timer_value_get

DESCRIPTION
  Returns interlace scan timer value that is read from the NV.

DEPENDENCIES
  none

===========================================================================*/

rex_timer_cnt_type reg_nv_interlace_scan_timer_value_get
(
  void
)
{
  MSG_HIGH_DS_1(REG_SUB, "=REG= reg_nv_interlace_scan_timer_value value %d", reg_nv_interlace_scan_timer_value);
  return reg_nv_interlace_scan_timer_value;
} //FRACQ

/*===========================================================================

FUNCTION reg_nv_interlace_scan_timer_value_get

DESCRIPTION
  Returns interlace scan timer value that is read from the NV.

DEPENDENCIES
  none

===========================================================================*/

dword reg_nv_nb1_acq_tier3_scan_timer_value_get
(
  void
)
{
  MSG_HIGH_DS_1(REG_SUB, "=REG= reg_nv_nb1_acq_tier3_timer_value value %d", reg_nv_nb1_acq_tier3_timer_value);
  return reg_nv_nb1_acq_tier3_timer_value;
} //FRACQ


/*===========================================================================

FUNCTION reg_nv_interlace_scan_timer_value_get

DESCRIPTION
  Returns interlace scan timer value that is read from the NV.

DEPENDENCIES
  none

===========================================================================*/

dword reg_nv_nb1_fullband_tier3_scan_timer_value_get
(
  void
)
{
  MSG_HIGH_DS_1(REG_SUB, "=REG= reg_nv_nb1_fullband_tier3_timer_value value %d", reg_nv_nb1_fullband_tier3_timer_value);
  return reg_nv_nb1_fullband_tier3_timer_value;
} //FRACQ




/*===========================================================================

FUNCTION reg_nv_read_ciot_params

DESCRIPTION
  Read ciot parameters and populate a local database


DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void  reg_nv_read_ciot_params (void)
{
  mcfg_fs_status_e_type status = MCFG_FS_STATUS_EFS_ERR;

  ciot_params_s_type ciot_params;
    
  status = mcfg_fs_read("/nv/item_files/modem/nas/ciot_params", &ciot_params,
                        sizeof(ciot_params_s_type), 
                        MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)reg_as_id);

  if (status == MCFG_FS_STATUS_OK)
  {
    MSG_MED_DS_0(REG_SUB, "=REG= Reading of CIOT params succeeded ");
    
    if ((ciot_params.irat_timer < 360)&&(ciot_params.irat_timer != 0))
    {
      MSG_HIGH_DS_1(REG_SUB, "=REG= Read a valid ciot irat timer %d [min]", ciot_params.irat_timer); 
      reg_nv_ciot_params.irat_timer = ciot_params.irat_timer;    
    }
    else
    {
      MSG_HIGH_DS_1(REG_SUB, "=REG= Invalid ciot irat timer %d received ",
                    ciot_params.irat_timer);
       
    }
    if(( ciot_params.search_alignment < 20)&&( ciot_params.search_alignment != 0))
    {
      MSG_HIGH_DS_1(REG_SUB, "=REG= Read a valid ciot alignment %d [min]", ciot_params.search_alignment); 
      reg_nv_ciot_params.search_alignment = ciot_params.search_alignment;    
    }
    else
    {
      MSG_HIGH_DS_1(REG_SUB, "=REG= Invalid ciot alignment %d received ",
                    ciot_params.search_alignment);
       
    }
  }
  else
  {
     MSG_HIGH_DS_1(REG_SUB, "=REG= Reading ciot params from EFS PLMN NV failed status = %d", status);
  }
}
/*===========================================================================
FUNCTION  reg_nv_get_ciot_params

DESCRIPTION
  This function returns the CIOT specific parameters

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
ciot_params_s_type reg_nv_get_ciot_params(void)
{
   return reg_nv_ciot_params;
}
/*===========================================================================

FUNCTION reg_nv_read_ciot_mcc_bands_params

DESCRIPTION
  Read mcc and bands from the NV


DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void  reg_nv_read_ciot_mcc_bands_params (sys_sys_mode_e_type rat)
{
  mcfg_fs_status_e_type status = MCFG_FS_STATUS_EFS_ERR;
  reg_nv_efs_mcc_band_s_type ciot_efs_mcc_bands;
  reg_nv_mcc_band_s_type ciot_mcc_and_band;
  uint8 i;
  struct fs_stat  nas_conf_stat;

  uint32 efs_size = sizeof(uint32)+ NUM_MCC_AND_BANDS *(sizeof(uint32)+4*sizeof(uint64));
  nas_conf_stat.st_size = 0;



  if ((rat != SYS_SYS_MODE_LTE_NB1) && (rat != SYS_SYS_MODE_LTE_M1))
  {
      return;
  }

  memset(&ciot_efs_mcc_bands, 0, sizeof(reg_nv_efs_mcc_band_s_type));
  memset(&ciot_mcc_and_band, 0, sizeof(reg_nv_mcc_band_s_type));
  if(reg_get_ulog_handle() != NULL)
  {
	ULOG_RT_PRINTF_2(reg_get_ulog_handle(),  "=REG= 0 size of efs %u efs size %d",sizeof(ciot_efs_mcc_bands),efs_size);
  }  
  if (rat == SYS_SYS_MODE_LTE_NB1)
  {
    status = mcfg_fs_read("/nv/item_files/modem/nas/ciot_mccs_bands_params_nb1", &ciot_efs_mcc_bands,
                        sizeof(ciot_efs_mcc_bands), 
                        MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)reg_as_id);
	if ( mcfg_fs_stat ("/nv/item_files/modem/nas/ciot_mccs_bands_params_nb1",&nas_conf_stat,MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)REG_AS_ID_1) != MCFG_FS_STATUS_OK)
    {
        if(reg_get_ulog_handle() != NULL)
        {
	       ULOG_RT_PRINTF_2(reg_get_ulog_handle(),  "=REG= size of efs %u efs size %d ",sizeof(ciot_efs_mcc_bands),efs_size);
        }
    }
    else
    {
       MSG_HIGH_DS_0(REG_SUB,"=REG=  efs file exists ");
       if(reg_get_ulog_handle() != NULL)
       {
	      ULOG_RT_PRINTF_3(reg_get_ulog_handle(),  "=REG= efs file exists  size of efs %u efs size %d file size = %d",sizeof(ciot_efs_mcc_bands),efs_size,nas_conf_stat.st_size);
       }
    }
    if(reg_get_ulog_handle() != NULL)
    {
	   ULOG_RT_PRINTF_3(reg_get_ulog_handle(),  "=REG=  size of efs %u efs size %d file size = %d",sizeof(ciot_efs_mcc_bands),efs_size,nas_conf_stat.st_size);
    }
    memset(&reg_nv_mcc_bands_db[1], 0, sizeof(reg_nv_mcc_bands_db_s_type));
  }
 #ifndef FEATURE_CATM1_DISABLE 
  else if (rat == SYS_SYS_MODE_LTE_M1)
  {
      status = mcfg_fs_read("/nv/item_files/modem/nas/ciot_mccs_bands_params_m1", &ciot_efs_mcc_bands,
                        sizeof(ciot_efs_mcc_bands),                         
                        MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)reg_as_id);

      if(reg_get_ulog_handle() != NULL)
      {
       ULOG_RT_PRINTF_2(reg_get_ulog_handle(),  "=REG= size of efs %u efs size %d",sizeof(ciot_efs_mcc_bands),efs_size);
      }
	  if ( mcfg_fs_stat ("/nv/item_files/modem/nas/ciot_mccs_bands_params_m1",&nas_conf_stat,MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)REG_AS_ID_1) != MCFG_FS_STATUS_OK)
	  {
		 if(reg_get_ulog_handle() != NULL)
		 {
		   ULOG_RT_PRINTF_2(reg_get_ulog_handle(),	"=REG=  size of efs %u efs size %d file size = %d",sizeof(ciot_efs_mcc_bands),efs_size);
		 }
	  }
	  else
	  {
		MSG_HIGH_DS_0(REG_SUB,"=REG=  efs file exists ");
		if(reg_get_ulog_handle() != NULL)
		{
		   ULOG_RT_PRINTF_3(reg_get_ulog_handle(),	"=REG= efs file exists  size of efs %u efs size %d file size = %d",sizeof(ciot_efs_mcc_bands),efs_size,nas_conf_stat.st_size);
		}
	  }
	  if(reg_get_ulog_handle() != NULL)
	  {
	   ULOG_RT_PRINTF_3(reg_get_ulog_handle(),	"=REG= 6 size of efs %u efs size %d file size = %d",sizeof(ciot_efs_mcc_bands),efs_size,nas_conf_stat.st_size);
      }

      memset(&reg_nv_mcc_bands_db[0], 0, sizeof(reg_nv_mcc_bands_db_s_type));
  }
 #endif 
  MSG_HIGH_DS_2(REG_SUB, "=REG= Reading mcc bands params from EFS PLMN NV  status = %d ciot_efs_mcc_bands version = %d", status, ciot_efs_mcc_bands.version);
  if(reg_get_ulog_handle() != NULL)
  {
    ULOG_RT_PRINTF_2(reg_get_ulog_handle(),
    "=REG= Reading mcc bands params from EFS PLMN NV  status = %d version = %d",
    status, ciot_efs_mcc_bands.version);
  }


  if (status == MCFG_FS_STATUS_OK)
  { 
      /* Sanity check */
      for (i = 0; i < NUM_MCC_AND_BANDS; ++i)
      {
  
          MSG_HIGH_DS_1(REG_SUB, "=REG= Reading mcc bands params from EFS PLMN NV status mcc %u",ciot_efs_mcc_bands.reg_nv_mcc_band[i].mcc);
          if(reg_get_ulog_handle() != NULL)
          {
            ULOG_RT_PRINTF_1(reg_get_ulog_handle(),  "=REG= Reading mcc bands params from EFS PLMN NV  mcc %u",ciot_efs_mcc_bands.reg_nv_mcc_band[i].mcc);
          }
          if (ciot_efs_mcc_bands.reg_nv_mcc_band[i].mcc != 0)
          {
              MSG_HIGH_DS_4(REG_SUB, "=REG= Reading mcc bands params from EFS PLMN NV failed status band[0] %u,band[1] %u, band[2] %u band[3] =%u",ciot_efs_mcc_bands.reg_nv_mcc_band[i].bands[0], ciot_efs_mcc_bands.reg_nv_mcc_band[i].bands[1],ciot_efs_mcc_bands.reg_nv_mcc_band[i].bands[2],ciot_efs_mcc_bands.reg_nv_mcc_band[i].bands[3]);
	          if(reg_get_ulog_handle() != NULL)
              {
               ULOG_RT_PRINTF_4(reg_get_ulog_handle(),  "=REG= Reading mcc bands params from EFS PLMN NV status band[0] %lu,band[1] %lu, band[2] %lu band[3] =%lu",ciot_efs_mcc_bands.reg_nv_mcc_band[i].bands[0], ciot_efs_mcc_bands.reg_nv_mcc_band[i].bands[1],ciot_efs_mcc_bands.reg_nv_mcc_band[i].bands[2],ciot_efs_mcc_bands.reg_nv_mcc_band[i].bands[3]);
              }			  
              memscpy(&ciot_mcc_and_band, sizeof(reg_nv_mcc_band_s_type), &ciot_efs_mcc_bands.reg_nv_mcc_band[i], sizeof(reg_nv_mcc_band_s_type));
              /* MCC entered is valid. Bands will be checked at a later stage */
              reg_nv_insert_mcc_bands(ciot_mcc_and_band, rat);
          }
      }
  }
  else
  {
     MSG_HIGH_DS_1(REG_SUB, "=REG= Reading mcc bands params from EFS PLMN NV failed status = %d", status);
     if(reg_get_ulog_handle() != NULL)
     {
       ULOG_RT_PRINTF_1(reg_get_ulog_handle(),  "=REG= Reading mcc bands params from EFS PLMN NV failed status = %d", status);
     }
  }  
}

/*===========================================================================

FUNCTION reg_nv_read_ciot_mcc_rats_params

DESCRIPTION
  Read mcc and rats from the NV


DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void  reg_nv_read_ciot_mcc_rats_params (void)
{
  mcfg_fs_status_e_type status = MCFG_FS_STATUS_EFS_ERR;
  reg_nv_mcc_rats_s_type ciot_mcc_rats[NUM_MCC_AND_RATS];
  uint32 i;

  memset(ciot_mcc_rats, 0, sizeof(reg_nv_mcc_rats_s_type)*NUM_MCC_AND_RATS);
  memset(&reg_nv_mcc_rats_db, 0, sizeof(reg_nv_mcc_rats_db));
       
  status = mcfg_fs_read("/nv/item_files/modem/nas/ciot_mcc_rats_params", &ciot_mcc_rats,
                        sizeof(ciot_mcc_rats), 
                        MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)reg_as_id);
  
  if (status == MCFG_FS_STATUS_OK)
  { 
      /* Sanity check */
      for (i = 0; i < NUM_MCC_AND_RATS; ++i)
      {          
          if (ciot_mcc_rats[i].mcc != 0)
          {
              /* MCC is valid, insert the element into the database*/
              reg_nv_insert_mcc_rats(ciot_mcc_rats[i]);
          }
      }
  }
  else
  {
     MSG_HIGH_DS_1(REG_SUB, "=REG= Reading mcc rats from EFS PLMN NV failed status = %d", status);
  }  
}

/*===========================================================================

FUNCTION reg_nv_read_ciot_nb1_interlace_params

DESCRIPTION
  Read NB1 Interlace scan related params


DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void reg_nv_read_ciot_nb1_interlace_params(void)
{
  mcfg_fs_status_e_type status = MCFG_FS_STATUS_EFS_ERR;
  ciot_nb1_tier3_timers_s_type nb1_tier3_timers;
  reg_timers_pet_watchdog();
  status = mcfg_fs_read("/nv/item_files/modem/nas/interlace_scan_timer", &reg_nv_interlace_scan_timer_value, sizeof(rex_timer_cnt_type), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)reg_as_id);

  if(status != MCFG_FS_STATUS_OK)
  {
     reg_nv_interlace_scan_timer_value = REG_INTERLACE_SCAN_TIMER_DEFUALT_VALUE * 1000;
  }
  else
  {
     //Timer value in EFS is provided in seconds convert to milliseconds.
     reg_nv_interlace_scan_timer_value = reg_nv_interlace_scan_timer_value * 1000;
  }
  MSG_HIGH_DS_2(REG_SUB, "=REG= NV reg_nv_interlace_scan_timer_value from EFS with status %d and value is %d(ms)",
                 status,
                 reg_nv_interlace_scan_timer_value);

  reg_timers_pet_watchdog();
  status = mcfg_fs_read("/nv/item_files/modem/nas/nb1_tier3_timers", &nb1_tier3_timers, sizeof(nb1_tier3_timers), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)reg_as_id);

  if(status != MCFG_FS_STATUS_OK)
  {
     reg_nv_nb1_acq_tier3_timer_value = REG_INTERLACE_NB1_TIER3_ACQ_DEFUALT_VALUE * 1000;
	 reg_nv_nb1_fullband_tier3_timer_value = REG_INTERLACE_NB1_TIER3_FULLBAND_DEFUALT_VALUE * 1000;
  }
  else
  {
     //Timer value in EFS is provided in seconds convert to milliseconds.
     reg_nv_nb1_acq_tier3_timer_value = nb1_tier3_timers.nb1_tier3_acq_timer* 1000;
	 reg_nv_nb1_fullband_tier3_timer_value = nb1_tier3_timers.nb1_tier3_fullband_timer* 1000;
  }
  MSG_HIGH_DS_2(REG_SUB, "=REG= NV reg_nv_nb1_fullband_tier3_timer_value from EFS with status %d and value is %d(ms)",
                 status,
                 reg_nv_nb1_fullband_tier3_timer_value);
 MSG_HIGH_DS_2(REG_SUB, "=REG= NV reg_nv_nb1_acq_tier3_timer_value from EFS with status %d and value is %d(ms)",
                 status,
                 reg_nv_nb1_acq_tier3_timer_value);

//FRACQ


}

//andre added MCC rats
/*==============================================================================

FUNCTION NAME

  reg_nv_filter_rats

  DESCRIPTION

  Enable the rats specified in the first argument which is a list of RATs
  that were set by the user for a given MCC. The order of those rats will be
  preserved. The third argument represents the UE capabilities and might
  be modified in this function. If no RATs match the UE capabilities
  no modofocation will be made to the UE capabilities.

  RETURN
  FALSE if no match has been found, TRUE otherwise
==============================================================================*/
boolean reg_nv_filter_rats(sys_sys_mode_e_type *rat_list, uint32 rat_length, 
    sys_rat_pri_list_info_s_type *rat_pri_list_info)
{
    uint32 i, j, k;
    sys_rat_pri_list_info_s_type rat_pri_created;
    memset(&rat_pri_created, 0, sizeof(rat_pri_created));
    memscpy(&rat_pri_created, sizeof(sys_rat_pri_list_info_s_type), rat_pri_list_info, 
        sizeof(sys_rat_pri_list_info_s_type));

    /* Clear the RAT field in the target */
    for (i = 0; i < rat_pri_created.num_items; ++i )
    {
        rat_pri_created.priority_list_info[i].acq_sys_mode = SYS_SYS_MODE_NONE;
    }
    rat_pri_created.num_items = 0;
    
    k = 0;
    for (j = 0; j<rat_length; ++j)
    {
        for (i = 0; i < rat_pri_list_info->num_items; ++i )
        {
            if (rat_list[j] == rat_pri_list_info->priority_list_info[i].acq_sys_mode)
            {
                rat_pri_created.priority_list_info[k].acq_sys_mode = rat_list[j];
                ++k;                
                if (k > rat_pri_list_info->num_items)
                {
                    break;
                }
                rat_pri_created.num_items++;
            }
        }
    }
    if (rat_pri_created.num_items != 0)
    {
        /* At least one rat specified by the user matches the rat in the phone capability */
        memscpy(rat_pri_list_info, sizeof(sys_rat_pri_list_info_s_type), &rat_pri_created, 
            sizeof(sys_rat_pri_list_info_s_type));
        return TRUE;
    }
    return FALSE;
}

/*===========================================================================
FUNCTION reg_nv_ciot_irat_rat_enabled

DESCRIPTION
  This function decides if the CIOT timer should be started or not. It takes
  into consideration the MCC of the PLMN that will be searched and if the
  user did/did not enabled the rat for this MCC


DEPENDENCIES
  none

RETURN VALUE
  TRUE if CIoT IRATs can be enabled, FALSE otherwise

SIDE EFFECTS
  none
===========================================================================*/
boolean reg_nv_ciot_irat_rat_enabled (void)
{          
    //andre added MCC rats
    reg_mode_3gpp_capability_s_type  hplmn_rat_search_order;
    sys_radio_access_tech_e_type initial_rat = SYS_RAT_NONE;
    sys_rat_pri_list_info_s_type rat_list_local;
    sys_sys_mode_e_type rat_list[REG_NV_MAX_RATS_LENGTH];    
    sys_sys_mode_e_type active_rat;
    boolean plmn_id_is_undefined;
    boolean mnc_includes_pcs_digit;
    uint32  mcc, mnc, i;    

#ifdef FEATURE_EQUIVALENT_HPLMN
    reg_sim_plmn_list_s_type     ehplmn_list;
    sys_plmn_id_s_type           plmn_initial;
#else
    sys_plmn_id_s_type           hplmn;
#endif

#ifdef FEATURE_EQUIVALENT_HPLMN
    ehplmn_list = reg_sim_ehplmn_list_get();
#else
    hplmn = reg_sim_read_hplmn();
#endif

    active_rat = reg_mode_convert_access_tech_to_acq_mode(reg_state_active_rat_get());
#ifdef FEATURE_EQUIVALENT_HPLMN
    plmn_initial = ehplmn_list.plmn[0];
    /* If RPLMN is one of the EHPLNM then start PLMN selection from RPLMN 
    ** else use EHPLMN[0]
    */    
#endif
    rat_list_local = *reg_mode_rat_pri_list_get();
    
    sys_plmn_get_mcc_mnc
    (
         plmn_initial,
         &plmn_id_is_undefined,
         &mnc_includes_pcs_digit,
         &mcc,
         &mnc
    );
        
    /* Find the rat list for this MCC */
    if (reg_nv_search_mcc_rats(mcc, &rat_list[0], REG_NV_MAX_RATS_LENGTH))
    {
        /* Found MCC in the database */
        /* Check if at least one RAT in that list matches the UE capability 
        * and populate the local list
        */
        if (reg_nv_filter_rats(rat_list, REG_NV_MAX_RATS_LENGTH, &rat_list_local))
        {
            /* For this MCC the user specified RAT(s) that match the UE capability*/
            for (i = 0; i < rat_list_local.num_items; ++i )
            {
                MSG_HIGH_DS_2(REG_SUB, "=REG= CIoT IRAT filtered RAT %d for MCC %d", 
                    rat_list_local.priority_list_info[i].acq_sys_mode, mcc);
            }
        }
        else
        {
            /* For this MCC, the user specified RAT(s) that do not match any of the rats
               in the UE capability */
            return FALSE;
        }
    }
    else
    {
        /* MCC not in the database. No RAT restriction for this MCC*/
        return TRUE;
    }
       
    
    reg_mode_get_3gpp_rat_capabilty(&hplmn_rat_search_order, reg_mode_rat_pri_list_get()
#ifdef FEATURE_SGLTE
        ,TRUE
#endif
        );
        
    /* If the most significant RAT in the UE capability, which is the rat that will be 
    *  searched through IRAT, matches the most significant rat set by the 
    *  user for this MCC, return TRUE
    */
    if (reg_mode_convert_access_tech_to_acq_mode(hplmn_rat_search_order.rat_list[0]) ==
        rat_list_local.priority_list_info[i].acq_sys_mode)
    {
        return TRUE;
    }

    /* Case where the top rat specified by the user does not match the top rat in the
    * UE Capability
    */
    /* Example : User enabled [M1, NB1]. UE capability is [G, M1, NB1] and UE is 
     * camped on M1. No scan of G expected
     */
    if (active_rat == rat_list_local.priority_list_info[0].acq_sys_mode)
    {
        return FALSE;
    }

    /* Example : User enabled [M1, NB1]. UE capability is [G, M1, NB1] and UE is 
     * camped on NB1. UE to scan M1
     */
    return TRUE;
}

#endif

