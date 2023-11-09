/*! \file rr_nv.c 
 
  This module contains access functions for NV data.
 
                Copyright (c) 2011-2014 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*/
/* $Header: //components/rel/geran.mpss/5.2.0/grr/src/rr_nv.c#6 $ */
/* $DateTime: 2023/09/29 01:38:57 $$Author: pwbldsvc $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "rr_seg_load.h"
#include "geran_variation.h"
#include "rr_nv.h"
#include "rr_nv_g.h"
#include "rr_channel_mode.h"
#include "rr_gprs_defs.h"
#include "fs_public.h"
#include "rr_msc.h"
#include "geran_multi_sim.h"
#include "sys.h"
#include "geran_nv.h"
#include "gprs_mem.h" 
#include "rr_sim.h"
#include "geran_grr_dyn_api.h"
#include "rr_static_general.h"
#ifdef FEATURE_GSM_TDS
#include "ghdi_exp.h"
#include "ghdi_exp_v.h"
#endif /*FEATURE_GSM_TDS*/
#ifdef FEATURE_MODEM_CONFIG_REFRESH
#include "rr_if.h"
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define RR_SCELL_RESYNC_TIME_DEFAULT (255) /* 255 seconds */
#define RR_PSCAN_STALENESS_DEFAULT   (4)   /*   4 seconds */

#ifdef FEATURE_IDLE_DRX_SCALING
/* Default timeout of 10 seconds will be used to suspend DRX scaling after (re)selection */
#define RR_IDLE_DRX_SCALING_SUSPEND_TIMEOUT_DEFAULT 10
#endif /* FEATURE_IDLE_DRX_SCALING */

#ifdef FEATURE_FAST_G2L_PSEUDO_SI2Q
/*----------------------------------------------------------------------------
 * Default GERAN, LTE and TDS Parameters
 * -------------------------------------------------------------------------*/
 #define RR_DEFAULT_GERAN_PRIO 1
 #define RR_GERAN_DEF_H_PRIO   0  /* This implies 06 DB */
 #define RR_GERAN_DEF_THRESH_GSM_LOW 15 /* This implies GRR will always measure low Priority IRAT neighbors */
 #define RR_GERAN_DEF_THRESH_PRIO_SRCH 15
 #define RR_GERAN_DEF_T_RESEL 0  /* Zero implies 05 seconds */
 
 #define RR_LTE_DEF_PRIO      7
 #define RR_LTE_DEF_MEAS_BW   0
 #define RR_LTE_DEF_QRXLEV_MIN 0
 #define RR_LTE_DEF_THRESH_HIGH  3
 #define RR_LTE_DEF_THRESH_LOW  0

 #define RR_UTRAN_DEF_PRIO   3
 #define RR_UTRAN_DEF_QRXLEV_MIN 0
 #define RR_UTRAN_DEF_THRESH_HIGH 0
 #define RR_UTRAN_DEF_THRESH_LOW  0
#endif

#define RR_MAX_ARFCN_PS_SUPPORT          100
/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

// Maximum number of PLMNs in EFS white list
#define MAX_NUM_OF_PLMNS 20

// Number of bytes storage per PLMN id
#define BYTES_PER_PLMN 3

#ifdef FEATURE_GERAN_R13_TEST_FRAMEWORK

typedef struct
{
  uint8   peo_dsc;
  uint8   rcc;
  uint8   c1_delta_min;
  uint8   c1_delta_max;
  uint8   peo_bcch_change_mark;
} rr_peo_efs_params_t;

#endif /*FEATURE_GERAN_R13_TEST_FRAMEWORK*/

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

/*!
 * \brief Takes the encoded split page cycle code and returns the decoded value
 * 
 * \param split_page_cycle_enc (in)
 * 
 * \return uint16 
 */
static uint16 decode_split_page_cycle(uint8 split_page_cycle_enc)
{
/* Translation of SPLIT PAGE CYCLE (encoded) to SPLIT PAGE CYCLE value for values 65 -> 98 */
  static const uint16 spc_code_to_val[] =
  {
    71, 72, 74, 75, 77, 79, 80, 83, 86, 88, /* indices 0..9   */
    90, 92, 96,101,103,107,112,116,118,128, /* indices 10..19 */
   141,144,150,160,171,176,192,214,224,235, /* indices 20..29 */
   256, 288, 320, 352                       /* indices 30..33 */
  };

  uint16 split_page_cycle;

/*
 SPLIT PAGE CYCLE values - see GSM 04.08 10.5.5.6

 SPLIT PAGE CYCLE CODE   SPLIT PAGE CYCLE value
 0                       704
 1 -> 64                 1 -> 64
 65 -> 98                as constant array above
 all others              1
*/

  if ( split_page_cycle_enc == 0 )
  {
    split_page_cycle = 704;
  }
  else
  if ( ( split_page_cycle_enc >= 1 ) && ( split_page_cycle_enc <= 64 ) )
  {
    split_page_cycle = split_page_cycle_enc;
  }
  else
  if ( ( split_page_cycle_enc >= 65 ) && ( split_page_cycle_enc <= 98 ) )
  {
    split_page_cycle = spc_code_to_val[split_page_cycle_enc - 65];
  }
  else
  {
    split_page_cycle = 1;
  }

  return split_page_cycle;

} /* end decode_split_page_cycle() */

/*!
 * \brief Reads carrier-specific EFS items and populates the local EFS data store.
 */
static void rr_read_carrier_specific_efs_nv_items(void)
{
  // Obtain a pointer to the EFS data
  rr_efs_nv_data_t *efs_data_ptr = rr_nv_get_efs_data_ptr();

#if defined(FEATURE_LTE) && defined(FEATURE_GERAN_CS_SUPPORT)
  // Read /nv/item_files/modem/geran/grr/g2l_blind_redir_control
  {
    int8 efs_val;

    if (geran_efs_read_per_sub(
          GERAN_EFS_G2L_BLIND_REDIR_CONTROL,
          &efs_val,
          sizeof(efs_val)) < 0)
    {
      MSG_GERAN_HIGH_0("g2l_blind_redir_control not present");
      efs_val = -1;
    }

    // Sanity-check the result is 0 or 1
    if ((efs_val != 0) && (efs_val != 1))
    {
      efs_val = 0;  // Default is DISABLED for this EFS-NV item
    }

    // Update the module store
    if (efs_val == 1)
    {
      MSG_GERAN_HIGH_0("g2l_blind_redir_after_voice_ss_or_periodic_lau_enabled is ENABLED");
      efs_data_ptr->g2l_blind_redir_after_voice_ss_or_periodic_lau_enabled = TRUE;
    }
    else
    {
      MSG_GERAN_HIGH_0("g2l_blind_redir_after_voice_ss_or_periodic_lau_enabled is DISABLED");
      efs_data_ptr->g2l_blind_redir_after_voice_ss_or_periodic_lau_enabled = FALSE;
    }
  }

  // Read /nv/item_files/modem/geran/grr/g2l_blind_redir_after_csfb_control
  {
    int8 efs_val;

    if (geran_efs_read_per_sub(
          GERAN_EFS_G2L_BLIND_REDIR_AFTER_CSFB_CONTROL,
          &efs_val,
          sizeof(efs_val)) < 0)
    {
      MSG_GERAN_HIGH_0("g2l_blind_redir_after_csfb_control not present");
      efs_val = -1;
    }

    // Sanity-check the result is 0 or 1
    if ((efs_val != 0) && (efs_val != 1))
    {
      efs_val = 0;  // Default is DISABLED for this EFS-NV item
    }

    // Update the module store
    if (efs_val == 1)
    {
      MSG_GERAN_HIGH_0("g2l_blind_redir_after_csfb_enabled is ENABLED");
      efs_data_ptr->g2l_blind_redir_after_csfb_enabled = TRUE;
    }
    else
    {
      MSG_GERAN_HIGH_0("g2l_blind_redir_after_csfb_enabled is DISABLED");
      efs_data_ptr->g2l_blind_redir_after_csfb_enabled = FALSE;
    }
  }

  // Read /nv/item_files/modem/geran/grr/g2l_blind_redir_after_srvcc_control
  {
    int8 efs_val;

    if (geran_efs_read_per_sub(
          GERAN_EFS_G2L_BLIND_REDIR_AFTER_SRVCC_CONTROL,
          &efs_val,
          sizeof(efs_val)) < 0)
    {
      MSG_GERAN_HIGH_0("g2l_blind_redir_after_srvcc_control not present");
      efs_val = -1;
    }

    // Sanity-check the result is 0 or 1
    if ((efs_val != 0) && (efs_val != 1))
    {
      efs_val = 0;  // Default is DISABLED for this EFS-NV item
    }

    // Update the module store
    if (efs_val == 1)
    {
      MSG_GERAN_HIGH_0("g2l_blind_redir_after_srvcc_enabled is ENABLED");
      efs_data_ptr->g2l_blind_redir_after_srvcc_enabled = TRUE;
    }
    else
    {
      MSG_GERAN_HIGH_0("g2l_blind_redir_after_srvcc_enabled is DISABLED");
      efs_data_ptr->g2l_blind_redir_after_srvcc_enabled = FALSE;
    }
  }

  // Read /nv/item_files/modem/geran/grr/fast_si_acq_during_csfb_control
  {
    int8 efs_val;

    if (geran_efs_read_per_sub(
          GERAN_EFS_FAST_SI_ACQ_DURING_CSFB_CONTROL,
          &efs_val,
          sizeof(efs_val)) < 0)
    {
      MSG_GERAN_HIGH_0("fast_si_acq_during_csfb_control not present");
      efs_val = -1;
    }

    // Sanity-check the result is 0 or 1
    if ((efs_val != 0) && (efs_val != 1))
    {
      efs_val = 1;  // Default is ENABLED for this EFS-NV item
    }

    // Update the module store
    if (efs_val == 1)
    {
      MSG_GERAN_HIGH_0("fast_si_acq_during_csfb_enabled is ENABLED");
      efs_data_ptr->fast_si_acq_during_csfb_enabled = TRUE;
    }
    else
    {
      MSG_GERAN_HIGH_0("fast_si_acq_during_csfb_enabled is DISABLED");
      efs_data_ptr->fast_si_acq_during_csfb_enabled = FALSE;
    }
  }
#endif /* FEATURE_LTE && FEATURE_GERAN_CS_SUPPORT */

  // Read /nv/item_files/modem/geran/grr/fast_si_acq_when_cs_only_enabled
  {
    int8 efs_val;

    if (geran_efs_read_per_sub(
          GERAN_EFS_FAST_SI_ACQ_WHEN_CS_ONLY_ENABLED,
          &efs_val,
          sizeof(efs_val)) < 0)
    {
      MSG_GERAN_HIGH_0("EFS-NV fast_si_acq_when_cs_only_enabled not present");
      efs_val = -1;
    }

    // Sanity-check the result is 0 or 1
    if ((efs_val != 0) && (efs_val != 1))
    {
      efs_val = 0;  // Default is DISABLED for this EFS-NV item
    }

    // Update the module store
    if (efs_val == 1)
    {
      MSG_GERAN_HIGH_0("Fast SI Acq when CS-only is ENABLED");
      efs_data_ptr->fast_si_acq_when_cs_only_enabled = TRUE;
    }
    else
    {
      MSG_GERAN_HIGH_0("Fast SI Acq when CS-only is DISABLED");
      efs_data_ptr->fast_si_acq_when_cs_only_enabled = FALSE;
    }
  }

#ifdef FEATURE_LTE
  // Read /nv/item_files/modem/geran/x2g_fast_si_acq_white_list
  {
    struct fs_stat file_status;

    efs_data_ptr->x2g_fast_si_acq_info.x2g_fast_si_acq_white_list_present = FALSE;

    if (geran_efs_stat_per_sub(
          GERAN_EFS_X2G_FAST_SI_ACQ_WHITE_LIST,
          &file_status))
    {
      // Number of bytes storage per PLMN id
      #define BYTES_PER_PLMN 3

      // EFS file size in bytes
      fs_size_t file_size_bytes = file_status.st_size;

      // Number of PLMNs that can be represented by this file size, capped at MAX_NUM_OF_PLMNS
      int num_of_plmns = MIN((file_size_bytes / BYTES_PER_PLMN), MAX_NUM_OF_PLMNS);

      // Number of bytes to allocate to store the PLMN list
      int alloc_size_bytes = num_of_plmns * BYTES_PER_PLMN;

      efs_data_ptr->x2g_fast_si_acq_info.x2g_fast_si_acq_white_list.plmn_list_ptr = GPRS_MEM_MALLOC(alloc_size_bytes);

      if (efs_data_ptr->x2g_fast_si_acq_info.x2g_fast_si_acq_white_list.plmn_list_ptr != NULL)
      {
        if (geran_efs_read_per_sub(
              GERAN_EFS_X2G_FAST_SI_ACQ_WHITE_LIST,
              efs_data_ptr->x2g_fast_si_acq_info.x2g_fast_si_acq_white_list.plmn_list_ptr,
              alloc_size_bytes
            ) == alloc_size_bytes)
        {
          efs_data_ptr->x2g_fast_si_acq_info.x2g_fast_si_acq_white_list.num_of_plmns = num_of_plmns;
          efs_data_ptr->x2g_fast_si_acq_info.x2g_fast_si_acq_white_list_present = TRUE;
        }
      }
      else
      {
        MSG_GERAN_ERROR_1("Failed to allocate %d bytes for x2g_fast_si_acq_white_list", alloc_size_bytes);
      }
    }
  }
#endif

  // Read /nv/item_files/modem/geran/negative_c1_white_list
  {
    struct fs_stat file_status;

    efs_data_ptr->negative_c1_white_list.num_of_plmns = 0;

    if (geran_efs_stat_per_sub(
          GERAN_EFS_NEGATIVE_C1_WHITE_LIST,
          &file_status))
    {
      // EFS file size in bytes
      fs_size_t file_size_bytes = file_status.st_size;

      // Number of PLMNs that can be represented by this file size, capped at MAX_NUM_OF_PLMNS
      int num_of_plmns = MIN((file_size_bytes / BYTES_PER_PLMN), MAX_NUM_OF_PLMNS);

      // Number of bytes to allocate to store the PLMN list
      int alloc_size_bytes = num_of_plmns * BYTES_PER_PLMN;

      efs_data_ptr->negative_c1_white_list.plmn_list_ptr = GPRS_MEM_MALLOC(alloc_size_bytes);

      if (efs_data_ptr->negative_c1_white_list.plmn_list_ptr != NULL)
      {
        if (geran_efs_read_per_sub(
              GERAN_EFS_NEGATIVE_C1_WHITE_LIST,
              efs_data_ptr->negative_c1_white_list.plmn_list_ptr,
              alloc_size_bytes
            ) == alloc_size_bytes)
        {
          efs_data_ptr->negative_c1_white_list.num_of_plmns = num_of_plmns;
        }
      }
      else
      {
        MSG_GERAN_ERROR_1("Failed to allocate %d bytes for negative C1 whitelist", alloc_size_bytes);
      }
    }
  }

#ifdef FEATURE_GSM_GPRS_READ_CCCH_IN_XFER 
  // Read /nv/item_files/modem/geran/read_pch_during_transfer_whitelist
  {
    struct fs_stat file_status;

    efs_data_ptr->read_pch_during_transfer_whitelist.num_of_plmns = 0;   

    if (geran_efs_stat_per_sub(
          GERAN_EFS_READ_PCH_DURING_TRANSFER_WHITELIST,
          &file_status))
    {
      // EFS file size in bytes
      fs_size_t file_size_bytes = file_status.st_size;

      // Number of PLMNs that can be represented by this file size, capped at MAX_NUM_OF_PLMNS
      int num_of_plmns = MIN((file_size_bytes / BYTES_PER_PLMN), MAX_NUM_OF_PLMNS);

      // Number of bytes to allocate to store the PLMN list
      int alloc_size_bytes = num_of_plmns * BYTES_PER_PLMN;

      efs_data_ptr->read_pch_during_transfer_whitelist.plmn_list_ptr = GPRS_MEM_MALLOC(alloc_size_bytes);

      if (efs_data_ptr->read_pch_during_transfer_whitelist.plmn_list_ptr != NULL)
      {
        if (geran_efs_read_per_sub(
              GERAN_EFS_READ_PCH_DURING_TRANSFER_WHITELIST,
              efs_data_ptr->read_pch_during_transfer_whitelist.plmn_list_ptr,
              alloc_size_bytes
            ) == alloc_size_bytes)
        {
          efs_data_ptr->read_pch_during_transfer_whitelist.num_of_plmns = num_of_plmns;
        }
      }
      else
      {
        MSG_GERAN_ERROR_1("Failed to allocate %d bytes for read_pch_during_transfer_whitelist", alloc_size_bytes);
      }
    }
  }
#endif /*FEATURE_GSM_GPRS_READ_CCCH_IN_XFER */

  // Read /nv/item_files/modem/geran/plmn_search_freq_pruning_enabled
  {
    int8 efs_val;

    if (geran_efs_read_per_sub(
          GERAN_EFS_PLMN_SEARCH_FREQ_PRUNING_ENABLED,
          &efs_val,
          sizeof(efs_val)) < 0)
    {
      MSG_GERAN_HIGH_0("EFS-NV plmn_search_freq_pruning_enabled not present");
      efs_val = -1;
    }

    // Sanity-check the result is 0 or 1
    if ((efs_val != 0) && (efs_val != 1))
    {
      efs_val = 1;  // Default is ENABLED for this EFS-NV item
    }

    // Update the module store
    if (efs_val == 1)
    {
      MSG_GERAN_HIGH_0("PLMN search freq pruning is ENABLED");
      efs_data_ptr->plmn_search_freq_pruning_enabled = TRUE;
    }
    else
    {
      MSG_GERAN_HIGH_0("PLMN search freq pruning is DISABLED");
      efs_data_ptr->plmn_search_freq_pruning_enabled = FALSE;
    }
  }

  // Read /nv/item_files/modem/geran/enable_si3_only
  {
    int8 efs_val;

    if (geran_efs_read_per_sub(
          GERAN_EFS_ENABLE_SI3_ONLY,
          &efs_val,
          sizeof(efs_val)) < 0)
    {
      MSG_GERAN_HIGH_0("EFS-NV enable_si3_only not present");
      efs_val = -1;
    }

    // Sanity-check the result is 0 or 1
    if ((efs_val != 0) && (efs_val != 1))
    {
      efs_val = 0;  // Default is DISABLED for this EFS-NV item
    }

    // Update the module store
    if (efs_val == 1)
    {
      MSG_GERAN_HIGH_0("SI3 only is ENABLED");
      efs_data_ptr->enable_si3_only = TRUE;
    }
    else
    {
      MSG_GERAN_HIGH_0("SI3 only is DISABLED");
      efs_data_ptr->enable_si3_only = FALSE;
    }
  }

#ifdef FEATURE_FAST_G2L_PSEUDO_SI2Q
  // Read /nv/item_files/modem/geran/fast_g2l_using_pseudo_si2q_enabled
  {
    int8 efs_val;

    if (geran_efs_read_per_sub(
          GERAN_EFS_FAST_G2L_USING_PSEUDO_SI2Q_ENABLED,
          &efs_val, 
          sizeof(efs_val)) < 0)
    {
      MSG_GERAN_HIGH_0("fast_g2l_using_pseudo_si2q_enabled not present");
      efs_val = -1;
    }

    // Sanity-check the result is 0 or 1
    if ((efs_val != 0) && (efs_val != 1))
    {
      efs_val = 1;  // Default is ENABLED for this EFS-NV item
    }

    // Update the module store
    if (efs_val == 1)
    {
      MSG_GERAN_HIGH_0("fast_g2l_using_pseudo_si2q_enabled is ENABLED");
      efs_data_ptr->fast_g2l_using_pseudo_si2q_enabled = TRUE;
    }
    else
    {
      MSG_GERAN_HIGH_0("fast_g2l_using_pseudo_si2q_enabled is DISABLED");
      efs_data_ptr->fast_g2l_using_pseudo_si2q_enabled = FALSE;
    }
  }

  if (efs_data_ptr->fast_g2l_using_pseudo_si2q_enabled)
  {
    MSG_GERAN_HIGH_0("FEATURE_FAST_G2L_PSEUDO_SI2Q IS ENABLED, in NV ");

    /* Set the Default Value */
    efs_data_ptr->rr_efs_pseudo_si2q_def_data.pseudo_si2q_def_data_valid = TRUE;
  
    /* Set Default Params Here */
    efs_data_ptr->rr_efs_pseudo_si2q_def_data.rr_geran_def_data.geran_priority = RR_DEFAULT_GERAN_PRIO; 
    efs_data_ptr->rr_efs_pseudo_si2q_def_data.rr_geran_def_data.h_prio = RR_GERAN_DEF_H_PRIO;
    efs_data_ptr->rr_efs_pseudo_si2q_def_data.rr_geran_def_data.thresh_gsm_low = RR_GERAN_DEF_THRESH_GSM_LOW;
    efs_data_ptr->rr_efs_pseudo_si2q_def_data.rr_geran_def_data.thresh_priority_search = RR_GERAN_DEF_THRESH_PRIO_SRCH;
    efs_data_ptr->rr_efs_pseudo_si2q_def_data.rr_geran_def_data.t_reselection = RR_GERAN_DEF_T_RESEL;

    efs_data_ptr->rr_efs_pseudo_si2q_def_data.rr_lte_def_data.priority = RR_LTE_DEF_PRIO;
    efs_data_ptr->rr_efs_pseudo_si2q_def_data.rr_lte_def_data.meas_bandwidth = RR_LTE_DEF_MEAS_BW;
    efs_data_ptr->rr_efs_pseudo_si2q_def_data.rr_lte_def_data.qrxlevmin = RR_LTE_DEF_QRXLEV_MIN;
    efs_data_ptr->rr_efs_pseudo_si2q_def_data.rr_lte_def_data.thresh_high = RR_LTE_DEF_THRESH_HIGH;
    efs_data_ptr->rr_efs_pseudo_si2q_def_data.rr_lte_def_data.thresh_low = RR_LTE_DEF_THRESH_LOW;
  
    efs_data_ptr->rr_efs_pseudo_si2q_def_data.rr_utran_def_data.priority = RR_UTRAN_DEF_PRIO;
    efs_data_ptr->rr_efs_pseudo_si2q_def_data.rr_utran_def_data.qrxlevmin = RR_UTRAN_DEF_QRXLEV_MIN;
    efs_data_ptr->rr_efs_pseudo_si2q_def_data.rr_utran_def_data.thresh_high = RR_UTRAN_DEF_THRESH_HIGH;
    efs_data_ptr->rr_efs_pseudo_si2q_def_data.rr_utran_def_data.thresh_low = RR_UTRAN_DEF_THRESH_LOW;
  }
#endif /* FEATURE_FAST_G2L_PSEUDO_SI2Q */

#ifdef FEATURE_SGLTE_G2X_CONN_MODE_RESEL
  // Read /nv/item_files/modem/geran/sglte_g2x_conn_mode_resel_enabled
  {
    int8 efs_val;

    if (geran_efs_read_per_sub(
          GERAN_EFS_SGLTE_G2X_CONN_MODE_RESEL_ENABLED,
          &efs_val, 
          sizeof(efs_val)) < 0)
    {
      MSG_GERAN_HIGH_0("sglte_g2x_conn_mode_resel_enabled not present");
      efs_val = -1;
    }

    // Sanity-check the result is 0 or 1
    if ((efs_val != 0) && (efs_val != 1))
    {
      efs_val = 1;  // Default is ENABLED for this EFS-NV item
    }

    // Update the module store
    if (efs_val == 1)
    {
      MSG_GERAN_HIGH_0("sglte_g2x_conn_mode_resel_enabled is ENABLED");
      efs_data_ptr->sglte_g2x_conn_mode_resel_enabled = TRUE;
    }
    else
    {
      MSG_GERAN_HIGH_0("sglte_g2x_conn_mode_resel_enabled is DISABLED");
      efs_data_ptr->sglte_g2x_conn_mode_resel_enabled = FALSE;
    }
  }
#endif
}

/*!
 * \brief Reads EFS-NV items and populates the local NV data.
 */
static void rr_read_efs_nv_items(void)
{
  // Obtain a pointer to the EFS data
  rr_efs_nv_data_t *efs_data_ptr = rr_nv_get_efs_data_ptr();

  MSG_GERAN_HIGH_0("Reading EFS data...");

  /* I. Read device-specific EFS items first */

#ifdef FEATURE_VAMOS
  // Read /nv/item_files/modem/geran/vamos_support
  {
    uint8 vamos_support;

    if (geran_efs_read_primary(GERAN_EFS_VAMOS_SUPPORT,
                               &vamos_support,
                               sizeof(vamos_support)) < 0)
    {
      MSG_GERAN_HIGH_0("VAMOS-I enabled (default)");
      vamos_support = 1;
    }

    // vamos_support: 0=OFF 1=VAMOS-I 2=VAMOS-II
    if (vamos_support != 0)
    {
      efs_data_ptr->vamos_enabled = TRUE;
    }
    else
    {
      efs_data_ptr->vamos_enabled = FALSE;
    }
  }
#endif /* FEATURE_VAMOS */

  // Read /nv/item_files/modem/geran/pscan_results_reuse_time_secs

  if (geran_efs_read_primary(GERAN_EFS_PSCAN_RESULTS_REUSE_TIME_SECS,
                             &efs_data_ptr->pscan_results_reuse_time_secs,
                             sizeof(efs_data_ptr->pscan_results_reuse_time_secs)) < 0)
  {
    MSG_GERAN_HIGH_1("pscan_results_reuse_time_secs not present - default=%ds",
                     RR_PSCAN_STALENESS_DEFAULT);
    efs_data_ptr->pscan_results_reuse_time_secs = RR_PSCAN_STALENESS_DEFAULT;
  }
  else
  {
    MSG_GERAN_HIGH_1("pscan_results_reuse_time_secs=%d", efs_data_ptr->pscan_results_reuse_time_secs);
  }

  // Read /nv/item_files/modem/geran/add_crh_db

  if (geran_efs_read_primary(GERAN_EFS_ADD_CRH_DB,
                             &efs_data_ptr->add_crh_db, 
                             sizeof(efs_data_ptr->add_crh_db)) < 0)
  {
    // If the value if not configured (file not present), the default value is 0
    efs_data_ptr->add_crh_db = 0;
  }

  // Read /nv/item_files/modem/geran/add_crh_scell_thresh_db

  if (efs_data_ptr->add_crh_db != 0)
  {
    if (geran_efs_read_primary(GERAN_EFS_ADD_CRH_SCELL_THRESH_DB,
                               &efs_data_ptr->add_crh_scell_thresh_db, 
                               sizeof(efs_data_ptr->add_crh_scell_thresh_db)) < 0)
    {
      // If the value if not configured (file not present), the default value is -70
      efs_data_ptr->add_crh_scell_thresh_db = -70;
    }
  }

  MSG_GERAN_HIGH_2("add_crh_db=%ddB add_crh_scell_thresh_db=%ddB",
                   efs_data_ptr->add_crh_db, efs_data_ptr->add_crh_scell_thresh_db);

  // Read /nv/item_files/modem/geran/grr/sys_info_cache_enabled
  {
    int8 efs_val;

    if (geran_efs_read_primary(GERAN_EFS_SYS_INFO_CACHE_ENABLED,
                               &efs_val,
                               sizeof(efs_val)) < 0)
    {
      MSG_GERAN_HIGH_0("EFS-NV sys_info_cache_enabled not present");
      efs_val = -1;
    }

    // Sanity-check the result is 0 or 1
    if ((efs_val != 0) && (efs_val != 1))
    {
      efs_val = 1;  // Default is ENABLED for this EFS-NV item
    }

    // Update the module store
    if (efs_val == 1)
    {
      MSG_GERAN_HIGH_0("Sys info cache is ENABLED");
      efs_data_ptr->sys_info_cache_enabled = TRUE;
    }
    else
    {
      MSG_GERAN_HIGH_0("Sys info cache is DISABLED");
      efs_data_ptr->sys_info_cache_enabled = FALSE;
    }
  }

#ifdef FEATURE_GSM_BAND_AVOIDANCE
  if (geran_efs_read_primary(GERAN_EFS_BAND_AVOID_ENABLED, 
                             &efs_data_ptr->band_avoid_enabled, 
                             sizeof(efs_data_ptr->band_avoid_enabled)) < 0)
  {
    MSG_GERAN_HIGH_0("Band Avoid enabled (default)");
    efs_data_ptr->band_avoid_enabled = TRUE;
  }
  MSG_GERAN_HIGH_1("Band Avoid enabled: %d", efs_data_ptr->band_avoid_enabled);

  if (geran_efs_read_primary(GERAN_EFS_BAND_AVOID_NCELL_THRESHOLD, 
                             &efs_data_ptr->band_avoid_ncell_threshold, 
                             sizeof(efs_data_ptr->band_avoid_ncell_threshold)) < 0)
  {   
    MSG_GERAN_HIGH_0("Band Avoid Ncell Threshold (default)"); 
    efs_data_ptr->band_avoid_ncell_threshold = 21; /*rxlev = -90dBm*/
  }
  MSG_GERAN_HIGH_1("Band Avoid Ncell Threshold: %d", efs_data_ptr->band_avoid_ncell_threshold);

  if (geran_efs_read_primary(GERAN_EFS_BAND_AVOID_MAX_MEAS_RPTS, 
                             &efs_data_ptr->band_avoid_max_meas_rpts, 
                             sizeof(efs_data_ptr->band_avoid_max_meas_rpts)) < 0)
  {   
    MSG_GERAN_HIGH_0("Band Avoid Max Meas Rpts (default)"); 
    efs_data_ptr->band_avoid_max_meas_rpts = 4;
  }
  MSG_GERAN_HIGH_1("Band Avoid Max Meas Rpts: %d", efs_data_ptr->band_avoid_max_meas_rpts);
#endif /*FEATURE_GSM_BAND_AVOIDANCE*/

  // Read /nv/item_files/modem/geran/grr/iterative_si_acq_enabled
  {
    int8 efs_val;

    if (geran_efs_read_primary(GERAN_EFS_ITERATIVE_SI_ACQ_ENABLED,
                               &efs_val,
                               sizeof(efs_val)) < 0)
    {
      MSG_GERAN_HIGH_0("EFS-NV iterative_si_acq_enabled not present");
      efs_val = -1;
    }

    // Sanity-check the result is 0 or 1
    if ((efs_val != 0) && (efs_val != 1))
    {
      efs_val = 1;  // Default is ENABLED for this EFS-NV item
    }

    // Update the module store
    if (efs_val == 1)
    {
      MSG_GERAN_HIGH_0("Iterative SI acq is ENABLED");
      efs_data_ptr->iterative_si_acq_enabled = TRUE;
    }
    else
    {
      MSG_GERAN_HIGH_0("Iterative SI acq is DISABLED");
      efs_data_ptr->iterative_si_acq_enabled = FALSE;
    }
  }

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  // Read /nv/item_files/modem/geran/scell_resync_time_secs
  if (geran_efs_read_primary(GERAN_EFS_SCELL_RESYNC_TIME_SECS,
                             &efs_data_ptr->scell_resync_time_secs,
                             sizeof(efs_data_ptr->scell_resync_time_secs)) < 0)
  {
    MSG_GERAN_HIGH_1("scell_resync_time_secs not present - default=%ds",
                     RR_SCELL_RESYNC_TIME_DEFAULT);
    efs_data_ptr->scell_resync_time_secs = RR_SCELL_RESYNC_TIME_DEFAULT;
  }
  else
  {
    MSG_GERAN_HIGH_1("scell_resync_time_secs=%d", efs_data_ptr->scell_resync_time_secs);
  }
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

  // Read /nv/item_files/modem/geran/eng_mode_debug_enabled
  {
    int8 efs_val;

    if (geran_efs_read_primary(GERAN_EFS_ENG_MODE_DEBUG_ENABLED,
                               &efs_val,
                               sizeof(efs_val)) < 0)
    {
      MSG_GERAN_HIGH_0("EFS-NV eng_mode_debug_enabled not present");
      efs_val = -1;
    }

    // Sanity-check the result is 0 or 1
    if ((efs_val != 0) && (efs_val != 1))
    {
      efs_val = 0;  // Default is DISABLED for this EFS-NV item
    }

    // Update the module store
    if (efs_val == 1)
    {
      MSG_GERAN_HIGH_0("Eng Mode Debug is ENABLED");
      efs_data_ptr->eng_mode_debug_enabled = TRUE;
    }
    else
    {
      MSG_GERAN_HIGH_0("Eng Mode Debug is DISABLED");
      efs_data_ptr->eng_mode_debug_enabled = FALSE;
    }
  }

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
  if (geran_efs_read_primary(GERAN_EFS_G2X_MEASUREMENT_CONTROL,
                             &efs_data_ptr->g2x_measurement_control,
                             sizeof(efs_data_ptr->g2x_measurement_control)) < 0)
  {
    MSG_GERAN_HIGH_0("g2x_measurement_control not present (default=ENABLED)");

    efs_data_ptr->g2x_measurement_control.g2w_enabled = TRUE;
    efs_data_ptr->g2x_measurement_control.g2t_enabled = TRUE;
    efs_data_ptr->g2x_measurement_control.g2l_enabled = TRUE;
  }
  else
  {
    if ((efs_data_ptr->g2x_measurement_control.g2w_enabled != TRUE &&
         efs_data_ptr->g2x_measurement_control.g2w_enabled != FALSE) ||
        (efs_data_ptr->g2x_measurement_control.g2t_enabled != TRUE &&
         efs_data_ptr->g2x_measurement_control.g2t_enabled != FALSE) ||
        (efs_data_ptr->g2x_measurement_control.g2l_enabled != TRUE &&
         efs_data_ptr->g2x_measurement_control.g2l_enabled != FALSE))
    {
      MSG_GERAN_HIGH_0("g2x_measurement_control inconsistent (default=ENABLED)");

      efs_data_ptr->g2x_measurement_control.g2w_enabled = TRUE;
      efs_data_ptr->g2x_measurement_control.g2t_enabled = TRUE;
      efs_data_ptr->g2x_measurement_control.g2l_enabled = TRUE;
    }
  }

  MSG_GERAN_HIGH_3("g2x_measurement_control(g2w_enabled:%d, g2t_enabled:%d, g2l_enabled:%d)",
                   efs_data_ptr->g2x_measurement_control.g2w_enabled,
                   efs_data_ptr->g2x_measurement_control.g2t_enabled,
                   efs_data_ptr->g2x_measurement_control.g2l_enabled);
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

#ifdef FEATURE_IDLE_DRX_SCALING
  // Read /nv/item_files/modem/geran/idle_drx_scaling_suspend_time_secs
  if (geran_efs_read_primary(GERAN_EFS_IDLE_DRX_SCALING_SUSPEND_TIME_SECS,
                             &efs_data_ptr->idle_drx_scaling_suspend_time_secs,
                             sizeof(efs_data_ptr->idle_drx_scaling_suspend_time_secs)) < 0)
  {
    MSG_GERAN_HIGH_1("idle_idle_drx_scaling_suspend_time_secs not present - default=%ds",
                     RR_IDLE_DRX_SCALING_SUSPEND_TIMEOUT_DEFAULT);
    efs_data_ptr->idle_drx_scaling_suspend_time_secs = RR_IDLE_DRX_SCALING_SUSPEND_TIMEOUT_DEFAULT;
  }
  else
  {
    MSG_GERAN_HIGH_1("idle_drx_scaling_suspend_time_secs=%d", efs_data_ptr->idle_drx_scaling_suspend_time_secs);
  }
#endif /* FEATURE_IDLE_DRX_SCALING */

#ifdef FEATURE_GERAN_POWER_EFFICIENT_OPERATION 
  // Read /nv/item_files/modem/geran/peo_enabled
  if (geran_efs_read_primary(GERAN_EFS_PEO_ENABLED,
                             &efs_data_ptr->peo_enabled,
                             sizeof(efs_data_ptr->peo_enabled)) < 0)
  {
    MSG_GERAN_HIGH_0("peo_enabled not present - default=ENABLED");
    efs_data_ptr->peo_enabled = TRUE;
  }

#endif /*FEATURE_GERAN_POWER_EFFICIENT_OPERATION*/

#ifdef FEATURE_GERAN_R13_TEST_FRAMEWORK  

  // Read /nv/item_files/modem/geran/peo_dsc
  if (geran_efs_read_primary(GERAN_EFS_PEO_DSC,
                             &efs_data_ptr->peo_dsc,
                             sizeof(efs_data_ptr->peo_dsc)) < 0)
  {
    MSG_GERAN_HIGH_0("PEO DSC for testing is not present");
    efs_data_ptr->peo_test_param_present = FALSE;
  }
  else
  {
    MSG_GERAN_HIGH_1("PEO DSC for testing =%d", efs_data_ptr->peo_dsc);
     efs_data_ptr->peo_test_param_present = TRUE;
  }

  // Read /nv/item_files/modem/geran/peo_rcc
  if (geran_efs_read_primary(GERAN_EFS_PEO_RCC,
                             &efs_data_ptr->peo_rcc,
                             sizeof(efs_data_ptr->peo_rcc)) < 0)
  {
    MSG_GERAN_HIGH_0("PEO RCC testing is not present");

  }
  else
  {
    MSG_GERAN_HIGH_1("PEO RCC for testing =%d", efs_data_ptr->peo_rcc);

  }

  // Read /nv/item_files/modem/geran/peo_c1_delta_min
  if (geran_efs_read_primary(GERAN_EFS_PEO_C1_DELTA_MIN,
                             &efs_data_ptr->peo_c1_delta_min,
                             sizeof(efs_data_ptr->peo_c1_delta_min)) < 0)
  {
    MSG_GERAN_HIGH_0("PEO C1 DELTA MIN for testing is not present");
  }
  else
  {
    MSG_GERAN_HIGH_1("PEO C1 DELTA_MIN for testing =%d", efs_data_ptr->peo_c1_delta_min);
  }

  // Read /nv/item_files/modem/geran/peo_c1_delta_max
  if (geran_efs_read_primary(GERAN_EFS_PEO_C1_DELTA_MAX,
                             &efs_data_ptr->peo_c1_delta_max,
                             sizeof(efs_data_ptr->peo_c1_delta_max)) < 0)
  {
    MSG_GERAN_HIGH_0("PEO C1 DELTA MAX for testing is not present");
  }
  else
  {
    MSG_GERAN_HIGH_1("PEO C1 DELTA MAX for testing =%d", efs_data_ptr->peo_c1_delta_max);
  }  

  // Read /nv/item_files/modem/geran/peo_bcch_change_mark
  if (geran_efs_read_primary(GERAN_EFS_PEO_BCCH_CHANGE_MARK,
                             &efs_data_ptr->peo_bcch_change_mark,
                             sizeof(efs_data_ptr->peo_bcch_change_mark)) < 0)
  {
    MSG_GERAN_HIGH_0("PEO pebcch_change_mark for testing is not present");
  }
  else
  {
    MSG_GERAN_HIGH_1("PEO bcch_change_mark for testing =%d", efs_data_ptr->peo_bcch_change_mark);
  } 

  // Read /nv/item_files/modem/geran/implicit_reject
  if (geran_efs_read_primary(GERAN_EFS_IMPLICIT_REJECT,
                             &efs_data_ptr->implicit_reject,
                             sizeof(efs_data_ptr->implicit_reject)) < 0)
  {
    MSG_GERAN_HIGH_0("Implicit Reject for testing is not present");
  }
  else
  {
    MSG_GERAN_HIGH_1("Implicit Reject for testing =%d", efs_data_ptr->implicit_reject);
  }   

  // Read /nv/item_files/modem/geran/allow_peo_rach
  if (geran_efs_read_primary(GERAN_EFS_ALLOW_PEO_RACH,
                             &efs_data_ptr->allow_peo_rach,
                             sizeof(efs_data_ptr->allow_peo_rach)) < 0)
  {
    MSG_GERAN_HIGH_0("Allow PEO RACH for testing is not present by default is ENBALED");
    efs_data_ptr->allow_peo_rach = TRUE;
  }
  else
  {
    MSG_GERAN_HIGH_1("PEO bcch_change_mark for testing =%d", efs_data_ptr->allow_peo_rach);
  } 

  // Read /nv/item_files/modem/geran/ir_eab_enabled
  if (geran_efs_read_primary(GERAN_EFS_IR_EAB_ENABLED,
                              &efs_data_ptr->ir_eab_test_param_present,
                              sizeof(efs_data_ptr->ir_eab_test_param_present)) < 0)
  {
    MSG_GERAN_HIGH_0("IR EAB tset efs not present");
    efs_data_ptr->ir_eab_test_param_present = FALSE;
  }
  else
  {
    MSG_GERAN_HIGH_0("IR EAB test efs  present");
    efs_data_ptr->ir_eab_test_param_present = TRUE;
  }

#endif /*FEATURE_GERAN_R13_TEST_FRAMEWORK*/

#ifdef FEATURE_GERAN_EAB
  // Read /nv/item_files/modem/geran/eab_enabled
  {
    int8 efs_val;

    if (geran_efs_read_primary(GERAN_EFS_EAB_ENABLED, &efs_val, sizeof(efs_val)) < 0)
    {
      MSG_GERAN_HIGH_0("EFS eab_enabled not present");
      efs_val = -1;
    }

    // Sanity-check the result is 0 or 1
    if ((efs_val != 0) && (efs_val != 1))
    {
      efs_val = 0;  // Default is DISABLED for this EFS-NV item
    }

    // Update the module store
    if (efs_val == 1)
    {
      MSG_GERAN_HIGH_0("EAB is ENABLED");
      efs_data_ptr->eab_enabled = TRUE;
    }
    else
    {
      MSG_GERAN_HIGH_0("EAB is DISABLED");
      efs_data_ptr->eab_enabled = FALSE;
    }
  }
#endif /* FEATURE_GERAN_EAB */

#ifdef FEATURE_PS_PREFERRED_CELL
  {
    // Read /nv/item_files/modem/geran/ps_preferred_enabled
    int8 efs_val;
    if (geran_efs_read_primary(GERAN_EFS_PS_PREFERRED_ENABLED,
          &efs_val,
          sizeof(efs_val)) < 0)
    {
      MSG_GERAN_HIGH_0("EFS-NV ps_preferred_enabled not present");
      efs_val = -1;
    }

    // Sanity-check the result is 0 or 1
    if ((efs_val != 0) && (efs_val != 1))
    {
      efs_val = 0;  // Default is Disabled for this EFS-NV item
    }

    // Update the module store
    if (efs_val == 1)
    {
      MSG_GERAN_HIGH_0("PS preferred NV is ENABLED");
      efs_data_ptr->ps_preferred_enabled = TRUE;
    }
    else
    {
      MSG_GERAN_HIGH_0("PS preferred NV is DISABLED");
      efs_data_ptr->ps_preferred_enabled = FALSE;
    }
  }

  {
    // Read /nv/item_files/modem/geran/num_ps_supported_arfcn_thres
    uint8 efs_val;
    efs_data_ptr->num_ps_supported_arfcn_thres = RR_MAX_ARFCN_PS_SUPPORT;
    if (geran_efs_read_primary(GERAN_EFS_NUM_PS_SUPPORTED_ARFCN_THRES,
          &efs_val,
          sizeof(efs_val)) < 0)
    {
      MSG_GERAN_HIGH_1("EFS-NV num_ps_supported_arfcn_thres not present, use default (%d)",
        efs_data_ptr->num_ps_supported_arfcn_thres);
    }
    else
    {
      MSG_GERAN_HIGH_1("EFS Read: num_ps_supported_arfcn_thres=%d", efs_val);
  
      // Sanity-check the result is within range 0 - 100
      if (efs_val <= RR_MAX_ARFCN_PS_SUPPORT)
      {
        efs_data_ptr->num_ps_supported_arfcn_thres = efs_val;
      }
      else
      {
        MSG_GERAN_HIGH_1("Invalid value set, use default (%d) for num_ps_supported_arfcn_thres", 
          efs_data_ptr->num_ps_supported_arfcn_thres);
      }
    }
  }
#endif /* FEATURE_PS_PREFERRED_CELL */

  /* II. Read sub-specific EFS items next */
  rr_read_carrier_specific_efs_nv_items();

  return;
}

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/

/*!
 * \brief Returns the value of SPLIT_PAGE_CYCLE stored in NV.
 * 
 * \param as_id (in) - AS-ID (Note: Not GAS-ID)
 * 
 * \return uint16 
 */
uint16 rr_get_nv_split_page_cycle(void)
{
  return rr_nv_get_data_ptr()->split_page_cycle;

} /* end rr_get_nv_split_page_cycle() */

/*!
 * \brief Returns the value of NON_DRX_TIMER.
 * 
 * 
 * \return uint8 
 */
uint8 rr_get_nv_non_drx_timer(void)
{
  return rr_nv_get_data_ptr()->non_drx_timer;

} /* end rr_get_nv_non_drx_timer() */

/*!
 * \brief Returns the value of MULTISLOT_CLASS stored in NV.
 * 
 * 
 * \return uint8 
 */
uint8 rr_get_nv_multislot_class(void)
{
  return rr_nv_get_data_ptr()->multislot_class;

} /* end rr_get_nv_multislot_class() */

/*!
 * \brief Returns the value of ANITE_GCF_FLAG stored in NV. 
 * 
 * 
 * \return boolean - TRUE if enabled, FALSE otherwise
 */
boolean rr_get_nv_anite_gcf_flag(void)
{
  return rr_nv_get_data_ptr()->anite_gcf_flag;

} /* end rr_get_nv_anite_gcf_flag() */

/*!
 * \brief Returns TRUE if EGPRS is enabled in NV; FALSE otherwise.
 * 
 * 
 * \return boolean - TRUE if enabled; FALSE otherwise
 */
boolean rr_get_nv_egprs_enabled(void)
{
  return rr_nv_get_data_ptr()->egprs_enabled;

} /* end rr_get_nv_egprs_enabled() */

/*!
 * \brief Returns the value of EGPRS_MULTISLOT_CLASS stored in NV.
 * 
 * 
 * \return uint8
 */
uint8 rr_get_nv_egprs_multislot_class(void)
{
  return rr_nv_get_data_ptr()->egprs_multislot_class;

} /* end rr_get_nv_egprs_multislot_class() */

/*!
 * \brief Returns TRUE if the NV parameter EGPRS_8PSK_POWER_CLASS is non-zero; FALSE if zero.
 * 
 * 
 * \return boolean - TRUE if enabled; FALSE otherwise
 */
boolean rr_get_nv_egprs_8psk_ul_enabled(void)
{
  return rr_nv_get_data_ptr()->egprs_8psk_ul_enabled;

} /* end rr_get_nv_egprs_8psk_ul_enabled() */

/*!
 * \brief Returns the egprs 8psk power capability.
 * 
 * 
 * \return uint8 
 */
uint8 rr_get_nv_egprs_8psk_power_capability(void)
{
  return rr_nv_get_data_ptr()->egprs_8psk_power_capability;

} /* end rr_get_nv_egprs_8psk_power_capability() */

/*!
 * \brief Returns TRUE if GERAN FEATURE PACK 1 is enabled in NV; FALSE otherwise.
 * 
 * 
 * \return boolean - TRUE if enabled; FALSE otherwise
 */
boolean rr_get_nv_geran_feature_pack_1_enabled(void)
{
  return rr_nv_get_data_ptr()->geran_feature_pack_1_enabled;

} /* end rr_get_nv_geran_feature_pack_1_enabled() */

#ifdef FEATURE_WCDMA
/*!
 * \brief Returns the current mask from NV that indicates support for inter-RAT NACC.
 * 
 * 
 * \return uint8 - 0x01 indicates W2G, 0x02 indicates G2W, 0x03 indicates both
 */
uint8 rr_get_nv_interrat_nacc_support(void)
{
  return rr_nv_get_data_ptr()->interrat_nacc_support;

} /* end rr_get_nv_interrat_nacc_support() */
#endif

#ifdef FEATURE_GSM_DTM

/*!
 * \brief Indicates if DTM is enabled in NV.
 * 
 * 
 * \return boolean - TRUE if enabled; FALSE otherwise
 */
boolean rr_get_nv_dtm_enabled(void)
{
  return rr_nv_get_data_ptr()->dtm_enabled;

} /* end rr_get_nv_dtm_enabled() */

/*!
 * \brief Returns the value of DTM_EGPRS_MULTISLOT_SUBCLASS stored in NV.
 * 
 * 
 * \return uint8 
 */
uint8 rr_get_nv_dtm_egprs_multislot_subclass(void)
{
  return rr_nv_get_data_ptr()->dtm_egprs_multislot_subclass;

} /* end rr_get_nv_dtm_egprs_multislot_subclass() */

/*!
 * \brief Returns the value of DTM_MULTISLOT_CLASS, derived from DTM_EGPRS_MULTISLOT_SUBCLASS.
 * 
 * 
 * \return uint8 
 */
uint8 rr_get_nv_dtm_multislot_class(void)
{
  uint8 dtm_multislot_subclass = rr_get_nv_dtm_egprs_multislot_subclass();

  return rr_msc_conv_dtm_multislot_subclass_to_class(dtm_multislot_subclass);
}

#ifdef FEATURE_GSM_EDTM
/*!
 * \brief Indicates if EDTM is enabled in NV.
 * 
 * 
 * \return boolean - TRUE if enabled; FALSE otherwise
 */
boolean rr_get_nv_edtm_enabled(void)
{
  return rr_nv_get_data_ptr()->edtm_enabled;
}
#endif /* FEATURE_GSM_EDTM */

#endif /* FEATURE_GSM_DTM */

/*!
 * \brief Returns the value of GEA_SUPPORTED stored in NV.
 * 
 * 
 * \return uint8
 */
uint8 rr_get_nv_gea_supported(void)
{
  return rr_nv_get_data_ptr()->gea_supported;

} /* end rr_get_nv_gea_supported() */

#ifdef FEATURE_GSM_GPRS_MSC33

/*!
 * \brief Returns the value of HMSC_GPRS_CODED_MS_CLASS stored in NV.
 * 
 * \return uint8
 */
uint8 rr_get_nv_hmsc_gprs_coded_ms_class(void)
{
  return rr_nv_get_data_ptr()->hmsc_gprs_coded_ms_class;

} /* end rr_get_nv_hmsc_gprs_coded_ms_class() */

/*!
 * \brief Returns the value of HMSC_EGPRS_CODED_MS_CLASS stored in NV.
 * 
 * 
 * \return uint8
 */
uint8 rr_get_nv_hmsc_egprs_coded_ms_class(void)
{
  return rr_nv_get_data_ptr()->hmsc_egprs_coded_ms_class;

} /* end rr_get_nv_hmsc_egprs_coded_ms_class() */

#endif /* FEATURE_GSM_GPRS_MCS33 */

#ifdef FEATURE_VAMOS
/*!
 * \brief Indicates if VAMOS is enabled in NV.
 * 
 * 
 * \return boolean - TRUE is VAMOS is enabled in NV (VAMOS-I or VAMOS-II); FALSE otherwise
 */
boolean rr_get_nv_vamos_enabled(void)
{
  return rr_nv_get_efs_data_ptr()->vamos_enabled;
}
#endif /* FEATURE_VAMOS */

#ifdef FEATURE_LTE
/*!
 * \brief Indicates if Fast SI Acq during CSFB is enabled in NV.
 * 
 * 
 * \return boolean - TRUE if Fast SI Acq during CSFB is enabled, FALSE otherwise
 */
boolean rr_get_nv_fast_si_acq_during_csfb_enabled(void)
{
  return rr_nv_get_efs_data_ptr()->fast_si_acq_during_csfb_enabled;
}

boolean rr_nv_x2g_fast_si_acq_feature_is_enabled(void)
{
  rr_efs_nv_data_t *efs_data_ptr = rr_nv_get_efs_data_ptr();

  if ((efs_data_ptr->x2g_fast_si_acq_info.x2g_fast_si_acq_white_list_present) &&
      (efs_data_ptr->x2g_fast_si_acq_info.x2g_fast_si_acq_white_list.num_of_plmns > 0))
  {
    MSG_GERAN_HIGH_0("X2G SKIP SI13 enabled");
    return TRUE;
  }
  else
  {
    MSG_GERAN_HIGH_0("X2G SKIP SI13 NOT enabled");
    return FALSE;
  }	
}

boolean rr_nv_x2g_fast_si_acq_is_plmn_match(
  sys_plmn_id_s_type plmn
)
{
  int i;

  rr_efs_nv_data_t *efs_data_ptr = rr_nv_get_efs_data_ptr();

  // Point to the first PLMN in the list
  sys_plmn_id_s_type *plmn_id_ptr = efs_data_ptr->x2g_fast_si_acq_info.x2g_fast_si_acq_white_list.plmn_list_ptr;

  if (plmn_id_ptr != NULL)
  {
    sys_mcc_type mcc;
    sys_mnc_type mnc;
    boolean is_undefined;
    boolean includes_pcsd;

    sys_plmn_get_mcc_mnc(
      plmn,
      &is_undefined,
      &includes_pcsd,
      &mcc,
      &mnc
    );

    MSG_GERAN_HIGH_2(
      "X2G SKIP SI13 compare PLMN=(%d,%d)",
      mcc,
      mnc
    );

    for (i = 0; ((i < efs_data_ptr->x2g_fast_si_acq_info.x2g_fast_si_acq_white_list.num_of_plmns)&&(i < MAX_NUM_OF_PLMNS)); i++)
    {
      sys_plmn_get_mcc_mnc(
        *plmn_id_ptr,
        &is_undefined,
        &includes_pcsd,
        &mcc,
        &mnc
      );

      MSG_GERAN_HIGH_2(
        "x2g_fast_si_acq_white_list PLMN=(%d,%d)",
        mcc,
        mnc
      );

      if (sys_plmn_match(plmn, *plmn_id_ptr))
      {
        return TRUE;
      }

      // Point to the next PLMN in the list
      ++plmn_id_ptr;
    }
  }

  return FALSE;
}
#endif /* FEATURE_LTE */

/*!
 * \brief Indicates if Fast SI Acq when CS-only is enabled in NV.
 *        Note: Feature is disabled for a Type Approval SIM. 
 * 
 * 
 * \return boolean - TRUE if enabled, FALSE otherwise
 */
boolean rr_nv_fast_si_acq_when_cs_only_is_enabled(void)
{
  // Feature is disabled for a Type Approval SIM
  if (rr_get_nv_anite_gcf_flag())
  {
    return FALSE;
  }

  return rr_nv_get_efs_data_ptr()->fast_si_acq_when_cs_only_enabled;
}

/*!
 * \brief Indicates if the supplied PLMN is present in the C1 relaxation white-list.
 * 
 * \return boolean - TRUE if present, FALSE otherwise
 */
static boolean rr_nv_check_plmn_in_whitelist(sys_plmn_id_s_type *plmn_id_ptr, int num_list_entries, sys_plmn_id_s_type plmn)
{
  int i;

  MSG_GERAN_HIGH_1("Num of PLMN %d ", num_list_entries);
  
  if (plmn_id_ptr != NULL)
  {
    sys_mcc_type mcc;
    sys_mnc_type mnc;
    boolean is_undefined;
    boolean includes_pcsd;

    sys_plmn_get_mcc_mnc(
      plmn,
      &is_undefined,
      &includes_pcsd,
      &mcc,
      &mnc
    );
    MSG_GERAN_HIGH_2("Current  PLMN=(%d,%d)",mcc,mnc);

    for (i = 0; ((i < num_list_entries) && (i < MAX_NUM_OF_PLMNS)); i++)
    {
      sys_plmn_get_mcc_mnc(
        *plmn_id_ptr,
        &is_undefined,
        &includes_pcsd,
        &mcc,
        &mnc
      );

      MSG_GERAN_HIGH_2("White list PLMN=(%d,%d)",mcc,mnc);

      if (sys_plmn_match(plmn, *plmn_id_ptr))
      {
        return TRUE;
      }

      // Point to the next PLMN in the list
      ++plmn_id_ptr;
    }
  }

  return FALSE;
}

#ifdef FEATURE_GSM_GPRS_READ_CCCH_IN_XFER 
boolean rr_nv_read_pch_during_transfer_is_plmn_match(sys_plmn_id_s_type plmn)
{
  
  rr_efs_nv_data_t *efs_data_ptr = rr_nv_get_efs_data_ptr();

  // Feature is disabled for a Type Approval SIM
  if (rr_get_nv_anite_gcf_flag())
  {
    MSG_GERAN_HIGH_0_G("Read PCH during xfer disabled (GCF is ON)");  
    return FALSE;
  }
 
  return (rr_nv_check_plmn_in_whitelist(efs_data_ptr->read_pch_during_transfer_whitelist.plmn_list_ptr,
                                        efs_data_ptr->read_pch_during_transfer_whitelist.num_of_plmns,
                                        plmn));
}

#endif /*FEATURE_GSM_GPRS_READ_CCCH_IN_XFER */

boolean rr_nv_c1_relaxation_is_plmn_match(sys_plmn_id_s_type plmn)
{
 
  rr_efs_nv_data_t *efs_data_ptr = rr_nv_get_efs_data_ptr();

  if (rr_get_nv_anite_gcf_flag())
  {
    MSG_GERAN_HIGH_0_G("C1 relaxation disabled (GCF is ON)");  
    return FALSE;
  }

  return (rr_nv_check_plmn_in_whitelist(efs_data_ptr->negative_c1_white_list.plmn_list_ptr,
                                        efs_data_ptr->negative_c1_white_list.num_of_plmns,
                                        plmn));
}

/*!
 * \brief Returns the value of the power-scan re-use timer (0=disabled)
 * 
 * \return rex_timer_cnt_type - timer vaue in milliseconds
 */
rex_timer_cnt_type rr_nv_get_pscan_results_reuse_time_millisecs(void)
{
  rex_timer_cnt_type timeout_secs = rr_nv_get_efs_data_ptr()->pscan_results_reuse_time_secs;

  return (CONVERT_SECONDS_TO_MILLISECONDS(timeout_secs));
}

#ifdef FEATURE_SGLTE
#ifdef FEATURE_SGLTE_G2X_CONN_MODE_RESEL
/*!
 * \brief Indicates if SGLTE Connected Mode Reselection feature is enabled in NV.
 * 
 * \return boolean - TRUE if enabled, FALSE otherwise
 */
boolean rr_get_nv_sglte_g2x_conn_mode_resel_enabled(void)
{
  return rr_nv_get_efs_data_ptr()->sglte_g2x_conn_mode_resel_enabled;
}
#endif /* FEATURE_SGLTE_G2X_CONN_MODE_RESEL */
#endif /* FEATURE_SGLTE */

#ifdef FEATURE_FAST_G2L_PSEUDO_SI2Q

boolean rr_is_efs_nv_fast_g2l_using_pseudo_si2q_enabled(void)
{
  return rr_nv_get_efs_data_ptr()->fast_g2l_using_pseudo_si2q_enabled;
}

boolean rr_is_efs_nv_fast_g2l_using_pseudo_si2q_data_valid(void)
{
  return rr_nv_get_efs_data_ptr()->rr_efs_pseudo_si2q_def_data.pseudo_si2q_def_data_valid;

}

rr_efs_pseudo_si2q_def_data_t* rr_get_efs_nv_fast_g2l_pseudo_si2q_data(void)
{
  return(&rr_nv_get_efs_data_ptr()->rr_efs_pseudo_si2q_def_data);
}

#endif /* FEATURE_FAST_G2L_PSEUDO_SI2Q */

/*!
 * \brief Returns the additional C2 cell reselection hysteresis values.
 * 
 * \param add_crh_db_ptr (out)
 * \param add_crh_scell_thresh_db_ptr (out)
 * 
 * \return boolean - TRUE if configured, FALSE otherwise
 */
boolean rr_nv_get_add_crh_enabled(
  int8 *add_crh_db_ptr,
  int8 *add_crh_scell_thresh_db_ptr
)
{
  rr_efs_nv_data_t *efs_data_ptr = rr_nv_get_efs_data_ptr();

  if (efs_data_ptr->add_crh_db != 0)
  {
    *add_crh_db_ptr = efs_data_ptr->add_crh_db;
    *add_crh_scell_thresh_db_ptr = efs_data_ptr->add_crh_scell_thresh_db;

    return TRUE;
  }

  return FALSE;
}

/*!
 * \brief Returns the value of the plmn_search_freq_pruning_enabled. 
 *        Note: Feature is disabled for a Type Approval SIM. 
 * 
 * 
 * \return TRUE if enabled, FALSE otherwise
 */
boolean rr_nv_get_plmn_search_freq_pruning_enabled(void)
{
  // Feature is disabled for a Type Approval SIM
  if (rr_get_nv_anite_gcf_flag())
  {
    return FALSE;
  }

  return rr_nv_get_efs_data_ptr()->plmn_search_freq_pruning_enabled;
}

/*!
 * \brief Returns the value of the enable_si3_only. 
 *        Note: Feature is disabled for a Type Approval SIM. 
 * 
 * 
 * \return TRUE if enabled, FALSE otherwise
 */
boolean rr_nv_get_enable_si3_only(void)
{
  // Feature is disabled for a Type Approval SIM
  if (rr_get_nv_anite_gcf_flag())
  {
    return FALSE;
  }

  return rr_nv_get_efs_data_ptr()->enable_si3_only;
}

/*!
 * \brief Indicates if the sys info cache functionality is enabled.
 *        Note: Feature is disabled for a Type Approval SIM. 
 * 
 * \return boolean - TRUE if enabled, FALSE otherwise
 */
boolean rr_nv_sys_info_cache_is_enabled( void)
{
  // Feature is disabled for a Type Approval SIM
  if (rr_get_nv_anite_gcf_flag())
  {
    return FALSE;
  }

  return rr_nv_get_efs_data_ptr()->sys_info_cache_enabled;
}

/*!
 * \brief Indicates if the iterative updating during SI acq feature is enabled.
 *        Note: Feature is not disabled for a Type Approval SIM.
 * 
 * \return boolean - TRUE if enabled, FALSE otherwise
 */
boolean rr_nv_iterative_si_acq_is_enabled(void)
{
  // Note: Feature is not disabled for a Type Approval SIM

  return rr_nv_get_efs_data_ptr()->iterative_si_acq_enabled;
}

/*!
 * \brief Indicates if eng mode debug is enabled.
 *        
 * 
 * \return boolean - TRUE if enabled, FALSE otherwise
 */
boolean rr_nv_eng_mode_debug_is_enabled_dyn(void)
{
  return rr_nv_get_efs_data_ptr()->eng_mode_debug_enabled;
}

#ifdef FEATURE_GSM_BAND_AVOIDANCE
/**
  @brief Returns if band avoidance is enabled
*/
boolean rr_is_band_avoidance_enabled(void)
{
  return rr_nv_get_efs_data_ptr()->band_avoid_enabled;
}

/**
  @brief Returns the threshold for band avoidance ncells as 
         rxlev (0.63)
*/
uint8 rr_get_band_avoid_ncell_threshold(void)
{
  return rr_nv_get_efs_data_ptr()->band_avoid_ncell_threshold;
}

/**
  @brief Returns the max number of meas repts that can be sent 
  when doing band avoidance. 
*/
uint8 rr_get_band_avoid_max_meas_rpts(void)
{
  return rr_nv_get_efs_data_ptr()->band_avoid_max_meas_rpts;
}
#endif /*FEATURE_GSM_BAND_AVOIDANCE*/

#if defined(FEATURE_LTE) && defined(FEATURE_GERAN_CS_SUPPORT)
boolean rr_nv_get_g2l_blind_redir_after_voice_ss_or_periodic_lau_enabled(void)
{
  // Feature is disabled for a Type Approval SIM
  if (rr_get_nv_anite_gcf_flag())
  {
    return FALSE;
  }

  return rr_nv_get_efs_data_ptr()->g2l_blind_redir_after_voice_ss_or_periodic_lau_enabled;
}

boolean rr_nv_get_g2l_blind_redir_after_csfb_enabled(void)
{
  // Feature is disabled for a Type Approval SIM
  if (rr_get_nv_anite_gcf_flag())
  {
    return FALSE;
  }

  return rr_nv_get_efs_data_ptr()->g2l_blind_redir_after_csfb_enabled;
}

boolean rr_nv_get_g2l_blind_redir_after_srvcc_enabled(void)
{
  return rr_nv_get_efs_data_ptr()->g2l_blind_redir_after_srvcc_enabled;
}
#endif /* FEATURE_LTE && FEATURE_GERAN_CS_SUPPORT */

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
boolean rr_nv_get_g2w_measurements_enabled(void)
{
  return rr_nv_get_efs_data_ptr()->g2x_measurement_control.g2w_enabled;
}

boolean rr_nv_get_g2t_measurements_enabled(void)
{
  return rr_nv_get_efs_data_ptr()->g2x_measurement_control.g2t_enabled;
}

boolean rr_nv_get_g2l_measurements_enabled(void)
{
  return rr_nv_get_efs_data_ptr()->g2x_measurement_control.g2l_enabled;
}
#endif
#ifdef FEATURE_IDLE_DRX_SCALING
uint32 rr_nv_get_idle_drx_scaling_suspend_time_millisecs(void)
{
  uint32 timeout_secs = rr_nv_get_efs_data_ptr()->idle_drx_scaling_suspend_time_secs;

  return(CONVERT_SECONDS_TO_MILLISECONDS(timeout_secs));
}
#endif /* FEATURE_IDLE_DRX_SCALING */

#ifdef FEATURE_GERAN_POWER_EFFICIENT_OPERATION 

/*!
 * \brief Indicates if PEO EFS is enabled.
 *        
 * 
 * \return boolean - TRUE if enabled, FALSE otherwise
 */
boolean rr_nv_get_peo_enabled(void)
{
  return rr_nv_get_efs_data_ptr()->peo_enabled;
}
#endif /*FEATURE_GERAN_POWER_EFFICIENT_OPERATION*/

#ifdef FEATURE_GERAN_R13_TEST_FRAMEWORK  

/*!
 * \brief Indicates if PEO Test params in EFS  is present or not.
 *        
 * 
 * \return boolean - TRUE if enabled, FALSE otherwise
 */

boolean  rr_nv_is_peo_testing_params_present(void)
{
 MSG_GERAN_HIGH_1_G("PEO testing param is %d",rr_nv_get_efs_data_ptr()->peo_test_param_present );
 return rr_nv_get_efs_data_ptr()->peo_test_param_present;
}

/*!
 * \brief Indicates if PEO Test  DSC params from  EFS .
 *        
 * 
 * \return PEO DSC
 */

uint8 rr_nv_get_peo_dsc_from_efs(void)
{
  return rr_nv_get_efs_data_ptr()->peo_dsc;
}

/*!
 * \brief Indicates if PEO Test  RCC params from  EFS .
 *        
 * 
 * \return uint8 - PEO RCC
 */

uint8 rr_nv_get_peo_rcc_from_efs(void)
{
  MSG_GERAN_HIGH_1_G("PEO RCC testing param from EFS  is %d",rr_nv_get_efs_data_ptr()->peo_rcc); 
  return rr_nv_get_efs_data_ptr()->peo_rcc;
}

/*!
 * \brief Indicates if PEO Test  C1 DELTA MIN params from  EFS .
 *        
 * 
 * \return uint8 - PEO C1 DELTA MIN
 */

uint8 rr_nv_get_peo_c1_delta_min_from_efs(void)
{
  return rr_nv_get_efs_data_ptr()->peo_c1_delta_min;
}

/*!
 * \brief Indicates if PEO Test  C1 DELTA MAX params from  EFS .
 *        
 * 
 * \return uint8 - PEO C1 DELTA MAX
 */

uint8 rr_nv_get_peo_c1_delta_max_from_efs(void)
{
  return rr_nv_get_efs_data_ptr()->peo_c1_delta_max;
}

/*!
 * \brief Indicates if PEO Test  BCCH CHANGE MARK params from  EFS .
 *        
 * 
 * \return uint8 - PEO BCCH CHANGE MARK
 */

uint8 rr_nv_get_peo_bcch_change_mark_from_efs(void)
{
  MSG_GERAN_HIGH_1_G("PEO BCCH CHANGE MARK  testing param from EFS  is %d",rr_nv_get_efs_data_ptr()->peo_bcch_change_mark); 
  return rr_nv_get_efs_data_ptr()->peo_bcch_change_mark;
}

/*!
 * \brief Indicates if IR and EAB  Test params in EFS  is present or not. .
 *        
 * 
 * \return boolean - TRUE or false
 */

boolean rr_nv_is_ir_eab_testing_params_present(void)
{
  return rr_nv_get_efs_data_ptr()->ir_eab_test_param_present;
}

/*!
 * \brief Indicates if Implict  Test  Implcit Reject params from  EFS .
 *        
 * 
 * \return uint8 - Implcit Reject
 */

uint8 rr_nv_get_implicit_reject_from_efs(void)
{
  return rr_nv_get_efs_data_ptr()->implicit_reject;
}


/*!
 * \brief Indicates if Implict  Test  Implcit Reject  during RACH params from  EFS .
 *        
 * 
 * \return uint8 - Implcit Reject during rach
 */

uint8 rr_nv_get_implicit_reject_during_rach_from_efs(void)
{
  return rr_nv_get_efs_data_ptr()->implicit_reject_during_rach;
}

/*!
 * \brief Indicates if PEO rach is allowed from  EFS .
 *        
 * 
 * \return boolean - allow_peo_rach
 */

boolean rr_nv_get_allow_peo_rach_from_efs(void)
{
  return rr_nv_get_efs_data_ptr()->allow_peo_rach;
}

/*!
 * \brief Called when QXDM command is received to update the params.
 *  
 * \param msg_ptr (in) - peo_dsc
 */

void rr_nv_update_peo_params_present_flag(boolean peo_test_param_present)
{
  rr_efs_nv_data_t *efs_data_ptr = rr_nv_get_efs_data_ptr();
  efs_data_ptr->peo_test_param_present = peo_test_param_present;
  MSG_GERAN_HIGH_1_G("PEO Test params updated from QXDM is  =%d", peo_test_param_present);
}

/*!
 * \brief Called when QXDM command is received to update the params.
 *  
 * \param msg_ptr (in) - peo_dsc
 */

void rr_nv_update_peo_dsc_in_efs(uint8 peo_dsc)
{
  rr_efs_nv_data_t *efs_data_ptr = rr_nv_get_efs_data_ptr();
  efs_data_ptr->peo_dsc = peo_dsc;
  MSG_GERAN_HIGH_1_G("PEO DSC updated to EFS is  =%d", peo_dsc);
}

/*!
 * \brief Called when QXDM command is received to update the params.
 *  
 * \param msg_ptr (in) - rcc
 */

void rr_nv_update_peo_rcc_in_efs(uint8 rcc)
{
  rr_efs_nv_data_t *efs_data_ptr = rr_nv_get_efs_data_ptr();
  efs_data_ptr->peo_rcc = rcc;
  MSG_GERAN_HIGH_1_G("PEO RCC updated to EFS is  =%d", rcc);  
}

/*!
 * \brief Called when QXDM command is received to update the params.
 *  
 * \param msg_ptr (in) - c1_delta_min
 */

void rr_nv_update_peo_c1_delta_min_in_efs(uint8 c1_delta_min)
{
  rr_efs_nv_data_t *efs_data_ptr = rr_nv_get_efs_data_ptr();
  efs_data_ptr->peo_c1_delta_min = c1_delta_min;
  MSG_GERAN_HIGH_1_G("PEO C1 DELTA MIN updated to  EFS is  =%d", c1_delta_min);  
}

/*!
 * \brief Called when QXDM command is received to update the params.
 *  
 * \param msg_ptr (in) - c1_delta_max
 */

void rr_nv_update_peo_c1_delta_max_in_efs(uint8 c1_delta_max)
{
  rr_efs_nv_data_t *efs_data_ptr = rr_nv_get_efs_data_ptr();
  efs_data_ptr->peo_c1_delta_max = c1_delta_max;
  MSG_GERAN_HIGH_1_G("PEO C1 DELTA MAX updated to  EFS is  =%d", c1_delta_max);  
}

/*!
 * \brief Called when QXDM command is received to update the params.
 *  
 * \param msg_ptr (in) - bcch_change_mark
 */

void rr_nv_update_peo_bcch_change_mark_in_efs(uint8 bcch_change_mark)
{
  rr_efs_nv_data_t *efs_data_ptr = rr_nv_get_efs_data_ptr();
  efs_data_ptr->peo_bcch_change_mark = bcch_change_mark;
  MSG_GERAN_HIGH_1_G("PEO BCCH CHANGE MARK  updated to  EFS is  =%d", bcch_change_mark); 
}

/*!
 * \brief Called when QXDM command is received to update the params.
 *  
 * \param msg_ptr (in) - implicit_reject
 */

void rr_nv_update_implict_reject_in_efs(uint8 implicit_reject)
{
  rr_efs_nv_data_t *efs_data_ptr = rr_nv_get_efs_data_ptr();
  efs_data_ptr->implicit_reject = implicit_reject;
  MSG_GERAN_HIGH_1_G("Implict Reject updated in  EFS is  =%d", implicit_reject); 
}

/*!
 * \brief Called when QXDM command is received to update the params.
 *  
 * \param msg_ptr (in) - implicit_reject_during_rach
 */

void rr_nv_update_implict_reject_during_rach_in_efs(uint8 implicit_reject_during_rach)
{
  rr_efs_nv_data_t *efs_data_ptr = rr_nv_get_efs_data_ptr();
  efs_data_ptr->implicit_reject_during_rach = implicit_reject_during_rach;
  MSG_GERAN_HIGH_1_G("Implict Reject during RACH updated in  EFS is  =%d", implicit_reject_during_rach); 
}

/*!
 * \brief Called when QXDM command is received to update the params.
 *  
 * \param msg_ptr (in) - allow_peo_rach
 */

void rr_nv_update_allow_peo_rach_in_efs(boolean allow_peo_rach)
{
  rr_efs_nv_data_t *efs_data_ptr = rr_nv_get_efs_data_ptr();
  efs_data_ptr->allow_peo_rach = allow_peo_rach;
  MSG_GERAN_HIGH_1_G("PEO rach  updated in  EFS is  =%d", allow_peo_rach); 
}

boolean rr_nv_get_SI21_indicator(void)
{
  return rr_nv_get_efs_data_ptr()->si21_indicator;
}

void rr_nv_update_SI21_indicator(boolean si21_indicator)
{
  rr_efs_nv_data_t *efs_data_ptr = rr_nv_get_efs_data_ptr();
  efs_data_ptr->si21_indicator = si21_indicator;
  MSG_GERAN_HIGH_1_G("SI21 Indicator updated (%d)", si21_indicator);
}

uint8 rr_nv_get_SI21_position(void)
{
  return rr_nv_get_efs_data_ptr()->si21_position;
}

void rr_nv_update_SI21_position(uint8 si21_position)
{
  rr_efs_nv_data_t *efs_data_ptr = rr_nv_get_efs_data_ptr();
  efs_data_ptr->si21_position = si21_position & 0x01;
  MSG_GERAN_HIGH_1_G("SI21 Position updated (%d)", si21_position);
}

boolean rr_nv_get_SI9_where_indicator(void)
{
  return rr_nv_get_efs_data_ptr()->si9_where_indicator;
}

void rr_nv_update_SI9_where_indicator(boolean si9_where_indicator)
{
  rr_efs_nv_data_t *efs_data_ptr = rr_nv_get_efs_data_ptr();
  efs_data_ptr->si9_where_indicator = si9_where_indicator;
  MSG_GERAN_HIGH_1_G("SI9 WHERE Indicator updated (%d)", si9_where_indicator);
}

uint8 rr_nv_get_SI9_where(void)
{
  return rr_nv_get_efs_data_ptr()->si9_where;
}

void rr_nv_update_SI9_where(uint8 si9_where)
{
  rr_efs_nv_data_t *efs_data_ptr = rr_nv_get_efs_data_ptr();
  efs_data_ptr->si9_where = si9_where & 0x07;
  MSG_GERAN_HIGH_1_G("SI9 WHERE updated (%d)", si9_where);
}

boolean rr_nv_get_EAB_config(void)
{
  return rr_nv_get_efs_data_ptr()->eab_config;
}

void rr_nv_update_EAB_config(boolean eab_config)
{
  rr_efs_nv_data_t *efs_data_ptr = rr_nv_get_efs_data_ptr();
  efs_data_ptr->eab_config = eab_config;
  MSG_GERAN_HIGH_1_G("EAB Config updated (%d)", eab_config);
}

boolean rr_nv_get_special_access_class(void)
{
  return rr_nv_get_efs_data_ptr()->special_access_class;
}

void rr_nv_update_special_access_class(boolean special_access_class)
{
  rr_efs_nv_data_t *efs_data_ptr = rr_nv_get_efs_data_ptr();
  efs_data_ptr->special_access_class = special_access_class;
  MSG_GERAN_HIGH_1_G("Special Access Class updated (%d)", special_access_class);
}
#endif /*FEATURE_GERAN_R13_TEST_FRAMEWORK*/

#ifdef FEATURE_GERAN_EAB
/*!
 * \brief Indicates if EAB EFS is enabled.
 *        
 * 
 * \return boolean - TRUE if enabled, FALSE otherwise
 */
boolean rr_nv_get_eab_enabled()
{
  return rr_nv_get_efs_data_ptr()->eab_enabled;
}
#endif /* FEATURE_GERAN_EAB */

/*!
 * \brief Called when the NV data is received from NAS.
 *  
 * The NV data is copied into local storage. The Public Store is updated later when PS access is enabled. 
 * Note: the NV data is stored according to AS-ID in this message, not GAS-ID
 * 
 * \param msg_ptr (in) - pointer to the RR_GMM_GPRS_NV_PARAMS_IND message
 */
void rr_nv_process_nv_params_ind(rr_gmm_gprs_nv_params_ind_t *msg_ptr)
{
  rr_nv_data_t *rr_nv_data_ptr;

  // Sanity-check the input
  //RR_NULL_CHECK_RETURN_VOID(msg_ptr);

  // Obtain a pointer to the module data
  rr_nv_data_ptr = rr_nv_get_data_ptr();

  rr_nv_data_ptr->multislot_class              = msg_ptr->nv_multislot_class;
  rr_nv_data_ptr->non_drx_timer_enc            = msg_ptr->nv_non_drx_timer;
  rr_nv_data_ptr->split_page_cycle_enc         = msg_ptr->nv_split_paging_cycle;
  rr_nv_data_ptr->anite_gcf_flag               = msg_ptr->nv_anite_gcf_flag;
  rr_nv_data_ptr->egprs_enabled                = msg_ptr->nv_edge_feature_support;
  rr_nv_data_ptr->egprs_multislot_class        = msg_ptr->nv_edge_multislot_class;
  rr_nv_data_ptr->egprs_8psk_ul_enabled        = msg_ptr->nv_edge_8psk_power_class != 0;
  rr_nv_data_ptr->egprs_8psk_power_capability  = msg_ptr->nv_edge_8psk_power_capability;
  rr_nv_data_ptr->geran_feature_pack_1_enabled = msg_ptr->nv_edge_nw_ass_cell_change;
#ifdef FEATURE_WCDMA
  rr_nv_data_ptr->interrat_nacc_support        = msg_ptr->nv_interrat_nacc_support;
#endif
#ifdef FEATURE_GSM_DTM
  rr_nv_data_ptr->dtm_enabled                  = msg_ptr->nv_dtm_feature_support;
  rr_nv_data_ptr->dtm_egprs_multislot_subclass = msg_ptr->nv_dtm_multislot_subclass;
#ifdef FEATURE_GSM_EDTM
  rr_nv_data_ptr->edtm_enabled                 = msg_ptr->nv_edtm_feature_support;
#endif /* FEATURE_GSM_EDTM */
#endif /* FEATURE_GSM_DTM */
  rr_nv_data_ptr->gea_supported                = msg_ptr->nv_gea_supported;
#ifdef FEATURE_GSM_GPRS_MSC33
  rr_nv_data_ptr->hmsc_gprs_coded_ms_class     = msg_ptr->nv_hmsc_gprs_coded_ms_class;
  rr_nv_data_ptr->hmsc_egprs_coded_ms_class    = msg_ptr->nv_hmsc_egprs_coded_ms_class;
#endif /* FEATURE_GSM_GPRS_MSC33 */

#if defined(NV_DISABLE_VOICE_SUPPORT_MASK) || defined(NV_DISABLE_CSDATA_SUPPORT_MASK)
  {
    // Assume all call types are allowed
    boolean voice_calls_supported = TRUE;
    boolean csdata_calls_supported = TRUE;

    /* NAS will provide the mask in the RR_GMM_GPRS_NV_PARAMS_IND primitive */
#ifdef NV_DISABLE_VOICE_SUPPORT_MASK
    if ((msg_ptr->nv_disable_call_type & NV_DISABLE_VOICE_SUPPORT_MASK) != 0)
    {
      MSG_GERAN_HIGH_0("ds%d:Voice calls are disabled by NV");
      voice_calls_supported = FALSE;
    }
#endif /* NV_DISABLE_VOICE_SUPPORT_MASK */

#ifdef NV_DISABLE_CSDATA_SUPPORT_MASK
    if ((msg_ptr->nv_disable_call_type & NV_DISABLE_CSDATA_SUPPORT_MASK) != 0)
    {
      MSG_GERAN_HIGH_0("CS data calls are disabled by NV");
      csdata_calls_supported = FALSE;
    }
#endif /* NV_DISABLE_CSDATA_SUPPORT_MASK */

#ifdef FEATURE_GERAN_CS_SUPPORT
    rr_channel_mode_set_supported(
      voice_calls_supported,    // voice_calls_supported
      csdata_calls_supported   // csdata_calls_supported
    );
#endif /* FEATURE_GERAN_CS_SUPPORT */
  }
#endif /* NV_DISABLE_VOICE_SUPPORT_MASK | NV_DISABLE_CSDATA_SUPPORT_MASK */

  MSG_GERAN_HIGH_0("NV params from MM");
  MSG_GERAN_HIGH_4("multislot:%d non_drx:%d, spc:%d, geran_feature_pack_1:%d",
                   (int)rr_nv_data_ptr->multislot_class,
                   (int)rr_nv_data_ptr->non_drx_timer_enc,
                   (int)rr_nv_data_ptr->split_page_cycle_enc,
				   (int)rr_nv_data_ptr->geran_feature_pack_1_enabled);

  MSG_GERAN_HIGH_3("egprs:%d, egprs_multislot:%d, 8psk_ul:%d",
                   (int)rr_nv_data_ptr->egprs_enabled,
                   (int)rr_nv_data_ptr->egprs_multislot_class,
                   (int)rr_nv_data_ptr->egprs_8psk_ul_enabled );

#ifdef FEATURE_GSM_DTM
  {
    int edtm_support;

#ifdef FEATURE_GSM_EDTM
    edtm_support = (int)rr_nv_data_ptr->edtm_enabled;
#else
    edtm_support = 0xFF;
#endif /*  FEATURE_GSM_EDTM */

    MSG_GERAN_HIGH_3("DTM:%1d MSC:%d (EDTM:%02X)",
                     (int)rr_nv_data_ptr->dtm_enabled,
                     (int)rr_nv_data_ptr->dtm_egprs_multislot_subclass,
                     edtm_support);
  }
#endif /* FEATURE_GSM_DTM */

#ifdef FEATURE_WCDMA
  MSG_GERAN_HIGH_1("interrat_nacc 0x%02X", (int)rr_nv_data_ptr->interrat_nacc_support);
#endif

  /* Calculate value for split_page_cycle based on encoded value */
  rr_nv_data_ptr->split_page_cycle = decode_split_page_cycle(rr_nv_data_ptr->split_page_cycle_enc);

  /* Calculate value for non_drx_timer based on encoded value */
  rr_nv_data_ptr->non_drx_timer = rr_gprs_decode_drx_timer(rr_nv_data_ptr->non_drx_timer_enc);

  geran_read_efs_nv_items();

  /* Only GRR EFS items are carrier-specific and should be read per sub */
  rr_read_efs_nv_items();

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_RR_THERMAL_MITIGATION)
  /* Inform the Multi-Slot Class module that NV data is available */
  rr_msc_nv_ready();
#endif /* FEATURE_DUAL_SIM || FEATURE_RR_THERMAL_MITIGATION */
} /* end rr_nv_process_nv_params_ind() */

#ifdef FEATURE_MODEM_CONFIG_REFRESH
/*!
 * \brief Called from GCOMMON to post GERAN_NV_REFRESH_IND to GRR queue.
 *  
 */
void rr_nv_send_nv_refresh_ind_dyn(void)
{
  MSG_GERAN_HIGH_1_G("Send GERAN_NV_REFRESH_IND to GRR (gas_id=%d)");
  rr_send_imsg_geran_nv_refresh_ind();
}

/*!
 * \brief Called when the NV refresh ind received from MCFG.
 *  
 * Re-reads carrier specific EFS/NV items. 
 * 
 */
void rr_nv_process_nv_refresh_ind(void)
{
  MSG_GERAN_HIGH_0("Refreshing EFS data...");

  /* Re-read carrier specific EFS items */
  rr_read_carrier_specific_efs_nv_items();
}
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

#ifdef FEATURE_PS_PREFERRED_CELL
/*!
 * \brief Function to check if the PS preferred EFS is enabled
 * 
 * \return boolean - TRUE if enabled, FALSE otherwise
 */
boolean rr_nv_ps_preferred_enabled()
{
  return rr_nv_get_efs_data_ptr()->ps_preferred_enabled;
}

/*!
 *\brief Function to return the NV value set for threshold for number of ARFCN to limit for checking PS SUPPORT
 * Returns in uint8 - 0-100
 */
uint8 rr_nv_get_num_ps_supported_arfcn_thres()
{
   return rr_nv_get_efs_data_ptr()->num_ps_supported_arfcn_thres;
}
#endif /* FEATURE_PS_PREFERRED_CELL */

/* EOF */

