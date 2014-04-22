#ifndef REG_NV_H
#define REG_NV_H
/*==============================================================================


                     R E G  -  N V   H E A D E R   F I L E


                                  DESCRIPTION

  This header file contains functions for reading and writing data from NVRAM.

                             COPYRIGHT INFORMATION

Copyright (c) 2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/reg/vcs/reg_nv.h_v   1.0   06 Jun 2002 10:42:44   kabraham  $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
05/31/02   kwa     Initial version.
05/22/03   kwa     Modified reg_nv_write_equivalent_plmn_list to pass the
                   eplmn_list_p parameter as a const pointer to eliminate
                   a lint warning.
10/15/03   kwa     Added functions for reading and writing RPLMNACT.
12/03/03   kwa     Added functions for reading PSLOCI.
03/07/04   kwa     Added function reg_nv_write_cache.
03/17/04   kwa     Added function reg_nv_read_imsi.
07/07/05   up      Added reg_nv_read_gprs_gcf_flag to support GCF 12.2.2.1d.
07/18/05   up      Added reg_nv_read_ens_enabled_flag to support ENS NV flag.
07/27/05   up      Added reg_nv_equivalent_plmn_list_get to current ePLMN list.
08/09/11   abhi   Added reading of lpm_power_off EFS NV item
11/15/11  abhi   For EFRPLMNSI Selecting RLPMN if HPLMN is not found
09/12/11   HC     Updated the function declaration with void argument 
                  for reg_nv_is_lpm_power_off_enabled()
==============================================================================*/

/*==============================================================================

                            INCLUDE FILES FOR MODULE

==============================================================================*/

#include "mmcp_variation.h"
#include "sys.h"
#include "sys_v.h"
#include "nv_items.h"
#include "reg_sim_v.h"
#include "reg_cm.h"                                                   
#include "mcfg_fs.h"                                                  
/*==============================================================================

                                TYPE DEFINITIONS

==============================================================================*/

/**********************************
** Enhanced HPLMN Search
**********************************/
/* MAX size of HPPLMN config table.*/
#define REG_NV_CONFIG_TBL_MAX_SIZE 100
#define REG_NV_IMSI_MAX_LENGTH 8

#ifdef FEATURE_LTE
#define REG_EPSLOCI_LENGTH 18
#endif

/*
** Maximum length of EHPLMN list supported from NV
*/
#ifdef FEATURE_EQUIVALENT_HPLMN
#define REG_EFS_EHPLMN_MAX_LENGTH 20
#endif

/* Definition of bits to be used in the NV_RPLMNAcT to indicate
** the RATs that are not defined in 23.122
*/
#define TDSCDMA_ACT_BIT   0x01
#define LTE_M1_ACT_BIT    0x01
#define LTE_NB1_ACT_BIT   0x02

#define REG_NV_DEFAULT_MAX_VALIDATE_SIM_COUNTER (uint8)~0


typedef struct reg_nv_imsi_s
{
  byte digits[REG_NV_IMSI_MAX_LENGTH];
} reg_nv_imsi_s_type;

#ifdef FEATURE_DUAL_SIM
extern boolean                                reg_nv_is_hplmn_to_be_selected_sim[MAX_AS_IDS];
#define reg_nv_is_hplmn_to_be_selected reg_nv_is_hplmn_to_be_selected_sim[reg_sub_id]

extern boolean                                reg_nv_is_roaming_on_in_eplmn_sim[MAX_AS_IDS];
#define reg_nv_is_roaming_on_in_eplmn  reg_nv_is_roaming_on_in_eplmn_sim[reg_sub_id]

extern boolean                                reg_nv_is_sim_invalid_recovery_enabled_sim[MAX_AS_IDS];
#define reg_nv_is_sim_invalid_recovery_enabled reg_nv_is_sim_invalid_recovery_enabled_sim[reg_sub_id]

extern boolean                                reg_nv_is_pseudo_reselection_allowed_sim[MAX_AS_IDS];
#define reg_nv_is_pseudo_reselection_allowed reg_nv_is_pseudo_reselection_allowed_sim[reg_sub_id]

extern uint64	                              reg_nv_timestamp_at_power_off_sim[MAX_AS_IDS];
#define reg_nv_timestamp_at_power_off reg_nv_timestamp_at_power_off_sim[reg_sub_id]

#ifdef FEATURE_CIOT
extern uint64	reg_nv_ciot_irat_timer_at_power_off_sim[MAX_AS_IDS];
#define reg_nv_ciot_irat_timer_at_power_off reg_nv_ciot_irat_timer_at_power_off_sim[reg_sub_id]
#endif

extern rex_timer_cnt_type  reg_nv_t3245_timer_stored_value_sim[MAX_AS_IDS];
#define reg_nv_t3245_timer_stored_value  reg_nv_t3245_timer_stored_value_sim[reg_sub_id] 

#else 
extern boolean                                reg_nv_is_hplmn_to_be_selected;
extern boolean                                reg_nv_is_roaming_on_in_eplmn;
extern boolean                                reg_nv_is_sim_invalid_recovery_enabled;
extern boolean                                reg_nv_is_pseudo_reselection_allowed;
extern uint64								  reg_nv_timestamp_at_power_off;
extern uint64                                 reg_nv_ciot_irat_timer_at_power_off;
extern rex_timer_cnt_type                     reg_nv_t3245_timer_stored_value;
#endif

extern boolean reg_nv_is_camp_on_2g_if_reg_failure;

/*New REG structure to take care of future NV item, add all the NVs here*/
typedef struct reg_nv_items
{
  byte version;
  byte is_hplmn_has_to_be_selected;/* version 0*/
  byte is_roaming_on_in_eplmn; /*version 1*/
  byte is_sim_invalid_recovery_enabled;    /*version 2*/
  byte is_camp_on_2g_if_reg_failure;   /*version 3*/
  byte is_pseudo_reselection_allowed;      /*version 4*/
  byte reserved[21];
}reg_nv_items_T;
#define REG_NV_MAX_VPLMN_LENGTH    5        
/* Number of RATs that user may enable per MCC*/
#define REG_NV_MAX_RATS_LENGTH    2
typedef struct reg_nv_vplmn_list_s
{
  uint8 vplmn_length;
  /* structure for VPLMN list */
  struct 
  {
     sys_plmn_id_s_type                        plmn;
    /**< PLMN ID. */
     sys_radio_access_tech_e_type              rat;
  }vplmn_info[REG_NV_MAX_VPLMN_LENGTH];
}reg_nv_vplmn_list_s_type;
extern boolean reg_is_qrd_device ;

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_SGLTE)                    
extern boolean reg_nv_gcf_flag_sim[MAX_NAS_STACKS];
#define reg_nv_gcf_flag reg_nv_gcf_flag_sim[reg_as_id]
#else
extern boolean reg_nv_gcf_flag;
#endif

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_SGLTE)
extern reg_nv_vplmn_list_s_type *reg_nv_vplmn_list_sim[MAX_NAS_STACKS];      
#define reg_nv_vplmn_list (reg_nv_vplmn_list_sim[reg_as_id])
#else
extern reg_nv_vplmn_list_s_type *reg_nv_vplmn_list;
#endif
#ifdef FEATURE_CIOT
typedef struct
{
   /* Timeout for better rat search, specified in minutes */
   dword irat_timer;

   /* This variable specifies an interval before eDRX page
      when a scan should begin, expressed in minutes */
   dword search_alignment;
} ciot_params_s_type;

typedef struct
{
   /* Timeout for better rat search, specified in minutes */
   dword nb1_tier3_acq_timer;

   /* This variable specifies an interval before eDRX page
      when a scan should begin, expressed in minutes */
   dword nb1_tier3_fullband_timer;
} ciot_nb1_tier3_timers_s_type;


/* Max number of MCCs and their enabled bands that can be configured
   Used during PLMN selection
*/
#define NUM_MCC_AND_BANDS 5
/* Max number of MCCs and their enabled RATs that can be configured
   Used during IRATs
*/
#define NUM_MCC_AND_RATS 5




/* This is the reg_state_mcc_band_s_type that is placed in the hash table.
   Each MCC is allocated bands. */

typedef PACKED struct PACKED_POST
{
   uint32 mcc;
  uint64 bands[4];
} reg_nv_mcc_band_s_type;

typedef PACKED struct PACKED_POST
{
  uint32  version;
  reg_nv_mcc_band_s_type reg_nv_mcc_band[NUM_MCC_AND_BANDS];
} reg_nv_efs_mcc_band_s_type;



typedef struct reg_nv_mcc_bands_db_s
{
    sys_sys_mode_e_type rat;
    reg_nv_mcc_band_s_type reg_nv_mcc_band_per_rat[NUM_MCC_AND_BANDS];
}reg_nv_mcc_bands_db_s_type;

/* This is one EFS element */
typedef PACKED struct PACKED_POST
{
   uint32 mcc;
   sys_sys_mode_e_type rat[REG_NV_MAX_RATS_LENGTH];
} reg_nv_mcc_rats_s_type;

#endif
typedef struct reg_nv_psm_cache_s
{
  uint32                                 psm_nv_list_bitmask;
  sys_radio_access_tech_e_type           rat;
  sys_plmn_id_s_type                     plmn;
  sys_sim_state_e_type                   sim_state;
  uint64                                 timers_timestamp;
  boolean                                hplmn_search_period_timer_saved;
  dword                                  hplmn_search_period_timer_rem_duration;
  dword                                  hplmn_timer;
  boolean                                ciot_irat_timer_saved;
  dword                                  ciot_irat_timer_rem_duration;
#ifdef FEATURE_LTE
  reg_sim_backoff_fplmn_list_info_s_type backoff_fplmn_list;
  dword                                  backoff_fplmn_rem_duration;
#endif
  reg_sim_fplmn_counter_list_s_type        gprs_fplmn_list;
  reg_sim_dos_mitigation_plmn_list_s_type  dos_mitigation_fplmn_list;
  boolean                                ciot_nb1_acq_tier3_saved;
  dword                                  ciot_nb1_acq_tier3_rem_duration;
  boolean                                ciot_full_fullband_tier3_saved;
  dword                                  ciot_nb1_fullband_tier3_rem_duration;
  uint32                                  ciot_cm_serv_counter;
} reg_nv_psm_cache_s_type;

typedef enum
{
  REG_PSM_NV_RPLMNACT                           = 0,
  REG_PSM_NV_EQUIVALENT_PLMN_LIST               = 1,
  REG_PSM_NV_EXTENDED_EQUIVALENT_PLMN_LIST      = 2,
  REG_PSM_EFS_IRAT_SEARCH_TIMER                 = 3,
  REG_PSM_EFS_RPLMN                             = 4,
  REG_PSM_EFS_DATA_ROAM_STATUS                  = 5,
  REG_PSM_EFS_LTE_AVAILABLE_TG_CELL_LIST        = 6,
  REG_PSM_EFS_LTE_NON_AVAILABLE_TG_CELL_LIST    = 7,
  REG_PSM_EFS_T3245_TIMER_STORED                = 8,    
  REG_PSM_NV_EFS_MAX                            = 9
} reg_nv_list_psm_e_type;

#ifdef FEATURE_DUAL_SIM
extern boolean                           reg_nv_psm_r12_enabled_sim[MAX_NAS_STACKS];
#define reg_nv_psm_r12_enabled           reg_nv_psm_r12_enabled_sim[reg_as_id]
extern boolean                           reg_nv_psm_restore_pending_sim[MAX_NAS_STACKS];
#define reg_nv_psm_restore_pending       reg_nv_psm_restore_pending_sim[reg_as_id]
extern reg_nv_psm_cache_s_type           *reg_nv_psm_cache_ptr_sim[MAX_NAS_STACKS];
#define reg_nv_psm_cache_ptr             reg_nv_psm_cache_ptr_sim[reg_as_id]
extern uint32                            reg_nv_psm_nv_list_bitmask_sim[MAX_NAS_STACKS];
#define reg_nv_psm_nv_list_bitmask       reg_nv_psm_nv_list_bitmask_sim[reg_as_id]
#else
extern boolean                           reg_nv_psm_r12_enabled;
extern boolean                           reg_nv_psm_restore_pending;
extern reg_nv_psm_cache_s_type           *reg_nv_psm_cache_ptr;
extern uint32                            reg_nv_psm_nv_list_bitmask;
#endif


/*==============================================================================

                               DATA DECLARATIONS

==============================================================================*/

extern boolean  reg_nv_lpm_power_off;
extern boolean  reg_task_power_up_init;


/*==============================================================================

                             FUNCTION DECLARATIONS

==============================================================================*/

/*==============================================================================

                           FUNCTION DECLARATIONS

==============================================================================*/

#ifdef FEATURE_ENHANCED_NW_SELECTION
/*==============================================================================

FUNCTION NAME

  reg_nv_read_ens_enabled_flag

DESCRIPTION

  Function that reads the NV_ENS_ENABLED flag stored in NVRAM.
  This is a blocking call that returns only after reading the data from NVRAM.

RETURN VALUE

  ens_enabled_flag value, TRUE -ENS feature is enabled in NV.

==============================================================================*/

extern boolean reg_nv_read_ens_enabled_flag
(
  void
);
#endif

/*==============================================================================

FUNCTION NAME

  reg_nv_equivalent_plmn_list_get

DESCRIPTION

  Function that returns the current list of Equvalent PLMNs.

RETURN VALUE

  nv_equivalent_plmn_list_type - List of ePLMN list.

==============================================================================*/
extern nv_equivalent_plmn_list_type reg_nv_equivalent_plmn_list_get
(
  void
);
/*==============================================================================

FUNCTION NAME

  reg_nv_get_vplmn_list_length

DESCRIPTION

  Function that returns the length of VPLMNN

RETURN VALUE

  length of VPLMN list 

==============================================================================*/
extern int16 reg_nv_get_vplmn_list_length
(
  void
);

/*============================================================================== 

FUNCTION NAME

  reg_nv_write_vplmn_list

==============================================================================*/
extern mcfg_fs_status_e_type reg_nv_write_vplmn_list
(
  void
);
/*==============================================================================

FUNCTION NAME

  reg_nv_read_vplmn_list

==============================================================================*/
extern mcfg_fs_status_e_type reg_nv_read_vplmn_list 
(
  void
);

extern boolean reg_nv_pseudo_reselection_allowed(void);


/*==============================================================================

FUNCTION NAME

  reg_nv_output_vplmn_list

==============================================================================*/
extern void reg_nv_output_vplmn_list
(
  void
);

/*==============================================================================

FUNCTION NAME

  reg_nv_read_equivalent_plmn_list

DESCRIPTION

  Function that reads the equivalent PLMN list stored in reg_nv_equivalent_plmn_list

RETURN VALUE

  None

==============================================================================*/

extern void reg_nv_read_equivalent_plmn_list
(
  sys_plmn_id_s_type*   rplmn_p,
  sys_plmn_list_s_type* eplmn_list_p
);


/*==============================================================================

FUNCTION NAME

  reg_nv_write_equivalent_plmn_list

DESCRIPTION

  Function that writes the equivalent PLMN list to NVRAM.

RETURN VALUE

  None

==============================================================================*/

extern void reg_nv_write_equivalent_plmn_list
(
        sys_plmn_id_s_type    rplmn,
  const sys_plmn_list_s_type* eplmn_list_p
);


/*==============================================================================

FUNCTION NAME

  reg_nv_read_rplmnact

DESCRIPTION

  Function that reads the last used access technology value stored in reg_nv_rplmnact.

RETURN VALUE

  None

==============================================================================*/

extern void reg_nv_read_rplmnact
(
  nv_rplmnact_type* rplmnact_p
);


/*==============================================================================

FUNCTION NAME

  reg_nv_write_rplmnact

DESCRIPTION

  Function that writes the last used access technology value stored in NVRAM.

RETURN VALUE

  None

==============================================================================*/

extern void reg_nv_write_rplmnact
(
  nv_rplmnact_type rplmnact
);


/*==============================================================================

FUNCTION NAME

  reg_nv_read_psloci

DESCRIPTION

  Function that reads the PLMN from PSLOCI stored in NVRAM.  This is a
  blocking call that returns only after reading the data from NVRAM.

RETURN VALUE

  boolean  Flag indicating if PSLOCI was read successfully (TRUE) or
           not (FALSE).

==============================================================================*/

extern boolean reg_nv_read_psloci
(
  byte* psloci_p
);


/*==============================================================================

FUNCTION NAME

  reg_nv_write_cache

DESCRIPTION

  Function that writes all of the NV items stored in the reg_nv cache.
  This is a blocking call that returns only after writing the data to NVRAM.

RETURN VALUE

  TRUE if successful else FALSE

==============================================================================*/

extern boolean reg_nv_write_cache
(
  void
);


/*==============================================================================

FUNCTION NAME

  reg_nv_read_imsi

DESCRIPTION

  Function that reads the IMSI stored in NVRAM.  This is a blocking call
  that returns only after reading the data from NVRAM.

RETURN VALUE

  boolean  Flag indicating if IMSI was read successfully (TRUE) or
           not (FALSE).

==============================================================================*/

extern boolean reg_nv_read_imsi
(
  reg_nv_imsi_s_type* imsi_p
);
/*==============================================================================

FUNCTION NAME

  reg_nv_sys_equivalent_plmn_list_get

DESCRIPTION

  Function that provides the current list of Equivalent PLMNs in NV in 
  sys_plmn_list_s_type format.

RETURN VALUE

  Void

==============================================================================*/
extern void reg_nv_sys_equivalent_plmn_list_get
(
  sys_plmn_list_s_type* eplmn_list_p
);

/*==============================================================================

FUNCTION NAME

  reg_nv_initialize_cache

DESCRIPTION

  This function reads RPLMNACT and Equivalent PLMNs list from NV and stores them in reg nv cache. his is a blocking call
  that returns only after reading the data from NVRAM.

RETURN VALUE

  Void

==============================================================================*/

extern void reg_nv_initialize_cache
(
  void
);


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
extern void reg_nv_read_enhanced_hplmn_srch_tbl
(
  byte* nv_config_tbl
);

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
extern boolean reg_nv_read_epsloci(byte *epsloci);

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
extern boolean reg_nv_read_ignore_uplmn(void);
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
extern boolean  reg_nv_read_efs_ehplmn_file
(
  reg_sim_plmn_list_s_type   *ehplmn_list
);
/*===========================================================================

FUNCTION reg_nv_read_tdscdma_plmn_file

DESCRIPTION
  Read the PLMN list from EFS

  This function is used fo carrier specific requests.


DEPENDENCIES
  none

RETURN VALUE
  TRUE/FALSE depending on whether valid PLMN list is successfully read
  from NV or not.

SIDE EFFECTS
  none

===========================================================================*/
extern void  reg_nv_read_tdscdma_plmn_file (void);  
/*==============================================================================

FUNCTION NAME

  reg_nv_tdscdma_plmn_list_get
  DESCRIPTION
  This function returns list of TDSCDMA PLMNs stored internally
==============================================================================*/
extern reg_sim_plmn_list_s_type* reg_nv_tdscdma_plmn_list_get(void);
/*==============================================================================

FUNCTION NAME

  reg_nv_reg_special_tdscdma_test_setting_get
  DESCRIPTION
  This function returns the setting of the special TDSCDMA test setting
==============================================================================*/
extern uint8 reg_nv_reg_special_tdscdma_test_setting_get(void);

#endif


/*==============================================================================

FUNCTION NAME

  reg_nv_max_validate_sim_counter_get
  DESCRIPTION
  This function returns the setting of max validate sim counter value read from NV.
==============================================================================*/
extern byte reg_nv_max_validate_sim_counter_get(void);

extern void reg_nv_imp_variable_prints(void);

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
boolean reg_nv_is_imsi_switch_enabled(void);
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
boolean reg_nv_is_wcdma_freq_lock_enabled(void);

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
extern boolean reg_nv_is_lpm_power_off_enabled(void);

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
extern boolean reg_nv_efrplmnsi_select_rplmn(void);

#if defined(FEATURE_FEMTO_CSG) || defined (FEATURE_NAS_ECALL)
/*==============================================================================

FUNCTION NAME

  reg_nv_read_rplmn

==============================================================================*/

extern sys_plmn_id_s_type reg_nv_read_rplmn
(
  void
);

/*==============================================================================

FUNCTION NAME

  reg_nv_write_rplmn

==============================================================================*/

extern void reg_nv_write_rplmn
(
  sys_plmn_id_s_type  rplmn
);

/*==============================================================================

FUNCTION NAME

  reg_nv_write_rplmn_dup

==============================================================================*/

extern void reg_nv_write_rplmn_dup
(
  boolean  rplmn_dup
);

/*==============================================================================

FUNCTION NAME

  reg_nv_read_rplmn_dup

==============================================================================*/

boolean reg_nv_read_rplmn_dup
(
   void
);
#ifdef FEATURE_FEMTO_CSG
/*==============================================================================

FUNCTION NAME

  reg_nv_read_csg_support

==============================================================================*/

extern sys_csg_support_e_type reg_nv_read_csg_support
(
  void
);
#endif

#endif

/*===========================================================================

FUNCTION reg_nv_gcf_flag_get

===========================================================================*/

extern rex_timer_cnt_type reg_nv_gcf_flag_get
(
  void
);

#ifdef FEATURE_TDSCDMA
/*==============================================================================

FUNCTION NAME

  reg_nv_ehplmn_list_set
 
  DESCRIPTION
  This function stores internaly the passed in list of PLMNs

==============================================================================*/
extern void reg_nv_ehplmn_list_set(reg_sim_plmn_list_s_type ehplmn_list_efs);

/*==============================================================================

FUNCTION NAME

  reg_nv_ehplmn_list_get
 
  DESCRIPTION
  This function returns list of EHPLMNs stored internally

==============================================================================*/
extern void reg_nv_ehplmn_list_get(reg_sim_plmn_list_s_type* ehplmn_list_efs);

#endif
#if defined (FEATURE_TDSCDMA) || defined (FEATURE_SGLTE)
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
extern boolean reg_nv_is_forced_irat_enabled(void);
       
/*==============================================================================

FUNCTION NAME

  reg_nv_hplmn_irat_search_timer_set

==============================================================================*/
extern uint32 reg_nv_hplmn_irat_search_timer_get(void);

/*==============================================================================

FUNCTION NAME

  reg_nv_hplmn_irat_search_timer_set

==============================================================================*/
extern void reg_nv_hplmn_irat_search_timer_set(uint32 timer);

extern boolean reg_nv_write_lte_lists_to_efs(void);   

extern void reg_nv_read_lte_cell_lists(void);

void reg_nv_clear_non_available_efs_list(void);
void reg_nv_clear_available_efs_list(void);          

typedef struct lte_search_timer_config
{
   byte    short_lte_scan;
   byte    long_lte_scan;
   byte    max_lte_failures;
   byte    max_lte_failures_on_neutral_cell;
   byte    reserved[4];
} lte_search_timer_feature_config_T;

#ifdef FEATURE_DUAL_SIM
extern lte_search_timer_feature_config_T reg_nv_lte_search_config_sim[MAX_AS_IDS];
#define reg_nv_lte_search_config reg_nv_lte_search_config_sim[reg_sub_id]
#else
extern lte_search_timer_feature_config_T reg_nv_lte_search_config;
#endif
#endif

/*===========================================================================

FUNCTION reg_nv_t3245_timer_stored_value_get

DESCRIPTION
  Returns T3245 that is read from the NV.

DEPENDENCIES
  none

===========================================================================*/

extern rex_timer_cnt_type reg_nv_t3245_timer_stored_value_get
(
  void
);


/*===========================================================================

FUNCTION reg_nv_read_efnas_config

DESCRIPTION
  Reads EFNASCONFIG from NV

DEPENDENCIES
  none

===========================================================================*/
extern void reg_nv_read_efnas_config
(
  void
);



/*===========================================================================

FUNCTION reg_nv_validate_sim_timer_value_get

DESCRIPTION
  Returns T3245 that is read from the NV.

DEPENDENCIES
  none

===========================================================================*/

extern rex_timer_cnt_type reg_nv_validate_sim_timer_value_get
(
  void
);

/*===========================================================================

FUNCTION reg_nv_t3245_test_timer_value_get

DESCRIPTION
  Returns T3245 that is read from the NV.
  This value should only be used for test purposes.

DEPENDENCIES
  none

===========================================================================*/

extern rex_timer_cnt_type reg_nv_t3245_test_timer_value_get
(
  void
);

/*===========================================================================

FUNCTION reg_nv_timestamp_at_power_off_get

DESCRIPTION
  Returns timestamp at power off that is read from the NV.
  This value should only be used for test purposes.

DEPENDENCIES
  none

===========================================================================*/

extern uint64 reg_nv_timestamp_at_power_off_get
(
  void
);

#ifdef FEATURE_CIOT
/*===========================================================================

FUNCTION reg_nv_get_ciot_irat_timer_at_power_off

DESCRIPTION
  Returns timestamp at power off that is read from the NV.
  This value should only be used for test purposes.

DEPENDENCIES
  none

===========================================================================*/

extern uint64 reg_nv_get_ciot_irat_timer_at_power_off
(
  void
);
#endif
/*==============================================================================

FUNCTION NAME

  reg_nv_write_t3245_to_efs

DESCRIPTION

   Function write T3245 value to efs.

RETURN VALUE

  none

==============================================================================*/

extern boolean reg_nv_write_t3245_to_efs(rex_timer_cnt_type t3245_value);

#ifdef FEATURE_CIOT
/*==============================================================================
FUNCTION NAME

  reg_nv_write_ciot_irat_timer_to_efs

DESCRIPTION

   Function write ciot irat timer value to efs.

RETURN VALUE

  none

==============================================================================*/

extern boolean reg_nv_write_ciot_irat_timer_to_efs(rex_timer_cnt_type ciot_irat_value);
#endif

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
void reg_nv_read_carrier_specific_efs(void);

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
void reg_nv_process_mcfg_refresh(void);
#endif
/*==============================================================================

FUNCTION NAME

  reg_nv_read_override_roam_status

==============================================================================*/
extern boolean reg_nv_read_override_roam_status
(
#ifdef FEATURE_DUAL_SIM
  sys_modem_as_id_e_type as_id
#else
  void
#endif
);
/*==============================================================================

FUNCTION NAME

  reg_nv_write_override_roam_status

==============================================================================*/
extern void reg_nv_write_override_roam_status
(
  boolean override_roam_status_flag
#ifdef FEATURE_DUAL_SIM
  ,sys_modem_as_id_e_type reg_as_id_temp
#endif
);

#ifdef DISABLE_9x05_OPTIMIZATIONS
/*==============================================================================

FUNCTION NAME

  reg_nv_read_data_roaming

==============================================================================*/
extern sys_data_roaming_e_type  reg_nv_read_data_roaming
(
  void
);

/*==============================================================================

FUNCTION NAME

  reg_nv_write_data_roaming

==============================================================================*/
extern boolean  reg_nv_write_data_roaming
(
  sys_data_roaming_e_type data_roaming
);
#endif

/*===========================================================================
FUNCTION  reg_nv_get_preferred_plmn_list

DESCRIPTION
  This function returns the preferred PLMN list read from NV

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern reg_sim_plmn_w_act_s_type  * reg_nv_get_preferred_plmn_list(int32 *preferred_plmn_length);

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
extern void reg_nv_read_psm_config(void);

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
extern void reg_nv_psm_update_cache_bitmask
(
  uint32 bitmask,
  reg_nv_list_psm_e_type NV_EFS
);

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
extern boolean reg_nv_save_psm_context(sys_plmn_service_state_s_type *service_status);

/*===========================================================================
FUNCTION  reg_nv_psm_update_restore_status

DESCRIPTION
  This function updates the PSM restore status based on the NV/EFS read status

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void reg_nv_psm_update_restore_status(void);

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
extern void reg_nv_psm_write_cache(reg_nv_psm_cache_s_type *psm_cache_ptr);

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
extern void reg_nv_psm_free_cache(void);
#ifdef FEATURE_CIOT
/*===========================================================================
FUNCTION  reg_nv_read_ciot_params

DESCRIPTION
  This function reads the CIOT specific parameters from EFS

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void reg_nv_read_ciot_params(void);
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
ciot_params_s_type reg_nv_get_ciot_params(void);
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
void  reg_nv_read_ciot_mcc_bands_params (sys_sys_mode_e_type rat);
/*===========================================================================

FUNCTION reg_nv_get_ciot_mcc_bands_params

DESCRIPTION
  Returns the mcc and bands database


DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void  reg_nv_read_ciot_mcc_bands_params (sys_sys_mode_e_type rat);
/*==============================================================================
FUNCTION NAME:  reg_nv_search_mcc_bands

DESCRIPTION
  This function finds an element in the hash table indicated by the key
  parameter passed in

RETURN VALUE
  Element of the hash table, which is MCC allocated Bands
==============================================================================*/
reg_nv_mcc_band_s_type reg_nv_search_mcc_bands(sys_sys_mode_e_type rat, uint32 mcc);
/*==============================================================================
FUNCTION NAME:  reg_nv_search_mcc_rats

DESCRIPTION
  This function finds the mcc in the hash table and 
  returns the RATs that are associated with that MCC

RETURN VALUE
  Element of the hash table, which is rats selected for this MCC
==============================================================================*/
boolean reg_nv_search_mcc_rats(uint32 mcc, sys_sys_mode_e_type *rat, uint32 length) ;
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
void  reg_nv_read_ciot_mcc_rats_params (void);
/*===========================================================================
FUNCTION reg_nv_ciot_irat_rat_enabled

DESCRIPTION
  This function decides if the CIOT timer should be started or not. It takes
  into consideration the MCC of the PLMN that will be searched and if the
  user did/did not enabled the rat for this MCC


DEPENDENCIES
  none

RETURN VALUE
  TRUE if CIOt can be started, FALSE otherwise

SIDE EFFECTS
  none
===========================================================================*/
boolean reg_nv_ciot_irat_rat_enabled (void);

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
void reg_nv_read_ciot_nb1_interlace_params(void);
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
    sys_rat_pri_list_info_s_type *rat_pri_list_info);
#endif

/*===========================================================================

FUNCTION reg_nv_interlace_scan_timer_value_get

DESCRIPTION
  Returns interlace scan timer value that is read from the NV.

DEPENDENCIES
  none

===========================================================================*/

extern rex_timer_cnt_type reg_nv_interlace_scan_timer_value_get
(
  void
);

extern dword reg_nv_nb1_acq_tier3_scan_timer_value_get
(
  void
);
 //FRACQ


extern dword reg_nv_nb1_fullband_tier3_scan_timer_value_get
(
  void
);




#endif
