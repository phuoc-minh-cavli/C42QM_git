/*============================================================================
  FILE:         rr_resel.c

  OVERVIEW:     RR main reselection state machine.

  DEPENDENCIES: None

                Copyright (c) 2009-2014 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //components/rel/geran.mpss/5.2.0/grr/src/rr_reduced_mobility.c#4 $ */


/*----------------------------------------------------------------------------
 * Include Files
 *--------------------------------------------------------------------------*/
#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "rr_multi_sim.h"
#if defined (FEATURE_GERAN_EXTENDED_DRX) || defined(FEATURE_GERAN_POWER_EFFICIENT_OPERATION) || defined(FEATURE_GERAN_POWER_SAVE_MODE)
#include "rr_seg_load.h"
#include "rr_reduced_mobility.h"
#include "rr_reduced_mobility_if.h"
#include "rr_resel.h"
#include "rr_resel_utils.h"
#include "rr_task.h"
#include "rr_gprs_dsc.h"
#include "rr_gprs_defs.h"
#include "rr_general.h"
#include "rr_resel_calcs.h"
#include "rr_defs.h"
#include "rr_log.h"
#include "rr_resel_if.h"
#include "rr_cell_acq_if.h"
#include "rr_gprs_defs.h"
#include "rr_l1_send.h"
#include "rr_conn_establish.h"
#include "rr_gprs_conn_establish.h"
#include "rr_mac_send.h"
#include "rr_psho.h"
#include "rr_mm_send.h"
#include "rr_pscan.h"
#include "rr_sys_info_i.h"
#include "rr_grr.h"
#include "rr_cell_acq.h"
#include "rr_sys_info_ncell.h"
#include "rr_sys_info_pcell.h"
#include "rr_sys_info_si.h"
#include "gprs_mem.h"
#include "rr_l1_idle_mode.h"
#include "rr_ps_access.h"
#include "rr_ps_domain.h"
#include "rr_main.h"
#include "rr_sim.h"
#include "rr_nv.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#ifdef FEATURE_GERAN_POWER_EFFICIENT_OPERATION 
#define NUM_OF_PED_DSC_ENTRIES       4 
#define NUM_OF_C1_DELTA_MIN_ENTRIES       4
#define NUM_OF_C1_DELTA_MAX_ENTRIES       8


/*--------------------------------------------------------------------------*/
typedef enum
{
  RR_RMC_PAGE,
  RR_RMC_CS_REQ,
  RR_RMC_PS_REQ,      
  RR_RMC_INVALID
} rr_scell_eval_reason_e;


/*--------------------------------------------------------------------------*/
typedef enum
{
  RR_RMC_INACTVE,
  RR_RMC_EVALUATE_SCELL,      /*RMS= Reduced mobility state idle and on receiving MEAS chekcing for better cell*/
  RR_RMC_PEO_BCCH_CHANGE,     /*RMS =Waiting for Scell BCCH read completion which could have been due to PEOBCCH detected */
  RR_RMC_NONE
} rr_rmc_state_e;

typedef struct
{
  rr_rmc_state_e  state;
  rr_rmc_state_e  old_state;
  int             C1a;
  int             C1b;
  uint8           C1_DELTA;
  uint8           C1_DELTA_MIN;
  uint8           C1_DELTA_MAX;
  uint16          ncell_refresh_time;
  uint16          ncell_refresh_count;
  uint8           peo_bcch_change_mark;
  rr_scell_eval_reason_e  eval_reason;
} rr_reduced_mobility_data_t;

#endif /*FEATURE_GERAN_POWER_EFFICIENT_OPERATION*/

#if defined(FEATURE_GERAN_EXTENDED_DRX) || defined (FEATURE_GERAN_POWER_SAVE_MODE)

typedef struct
{
  boolean psm_indicator;
  boolean edrx_indicator;
  word    BS_ePA_MFRMS;
  boolean attach_in_prgrs;
} rr_psm_edrx_data_t;

/*----------------------------------------------------------------------------
 * Local Variables
 *--------------------------------------------------------------------------*/
static rr_psm_edrx_data_t         rr_psm_edrx_data_spaces;

#define NUM_OF_ENTRIES_FOR_EDRX       12
static const int BS_ePA_MFRMS[NUM_OF_ENTRIES_FOR_EDRX] = {8, 16, 32, 52, 104, 208, 416, 832, 1664, 3328, 6656, 13312};

#endif /* FEATURE_GERAN_EXTENDED_DRX  || FEATURE_GERAN_POWER_SAVE_MODE*/


#ifdef FEATURE_GERAN_POWER_EFFICIENT_OPERATION 

static rr_reduced_mobility_data_t rr_reduced_mobility_data_spaces;

static const int PEO_DSC_VALUE[NUM_OF_PED_DSC_ENTRIES] = { 4, 6, 8, 10 };

static const int C1_DELTA_MIN_IN_dBM[NUM_OF_C1_DELTA_MIN_ENTRIES] = { 3, 6, 9, 12 };

static const int C1_DELTA_MAX_IN_dBM[NUM_OF_C1_DELTA_MAX_ENTRIES] = { 3, 6, 9, 12, 15, 18, 21 };

#endif /*FEATURE_GERAN_POWER_EFFICIENT_OPERATION*/

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 *--------------------------------------------------------------------------*/

#if defined(FEATURE_GERAN_EXTENDED_DRX ) || defined(FEATURE_GERAN_POWER_SAVE_MODE)

/*===========================================================================

FUNCTION      rr_psm_edrx_get_data_ptr

DESCRIPTION   Gets a pointer to PSM/EDRX data

DEPENDENCIES  None

RETURN VALUE  VOID

SIDE EFFECTS  None

===========================================================================*/

static rr_psm_edrx_data_t *rr_psm_edrx_get_data_ptr( void )
{
  return &rr_psm_edrx_data_spaces;
}

/*===========================================================================

FUNCTION      rr_init_psm_edrx_params

DESCRIPTION   Inits the PSM eDRX params

DEPENDENCIES  None

RETURN VALUE  VOID

SIDE EFFECTS  None

===========================================================================*/

void rr_init_psm_edrx_params( void )
{
  rr_psm_edrx_data_t *rr_psm_edrx_ptr = rr_psm_edrx_get_data_ptr();
  //RR_NULL_CHECK_FATAL(rr_psm_edrx_ptr);
  memset(rr_psm_edrx_ptr, 0, sizeof(rr_psm_edrx_data_t)); 
  MSG_GERAN_HIGH_0_G("Initialise PSM eDRX params");
}

/*===========================================================================

FUNCTION      rr_set_psm_edrx_support

DESCRIPTION   Sets the PSM/EDRX support

DEPENDENCIES  None

RETURN VALUE  VOID

SIDE EFFECTS  None

===========================================================================*/

void rr_set_psm_edrx_support(rr_mtc_nw_config_param_update_ind_T *rr_mtc_nw_config_param_update_ind, boolean *update_l1   )
{
  rr_psm_edrx_data_t *rr_psm_edrx_ptr = rr_psm_edrx_get_data_ptr();
  //RR_NULL_CHECK_FATAL(rr_psm_edrx_ptr);
  
  MSG_GERAN_HIGH_4_G("EDrx = %d: PSM Enabled = %d, edrx_cyle_value= %d, attach_in_prgrs = %d",
                      rr_mtc_nw_config_param_update_ind->edrx_params_valid, 
                      rr_mtc_nw_config_param_update_ind->psm_enabled, 
                      rr_mtc_nw_config_param_update_ind->edrx_params, 
                      rr_mtc_nw_config_param_update_ind->attach_in_prgrs);
  
  /*Update L1 only if there is change in params */
  if ((rr_psm_edrx_ptr->psm_indicator != rr_mtc_nw_config_param_update_ind->psm_enabled) ||
      (rr_psm_edrx_ptr->edrx_indicator != rr_mtc_nw_config_param_update_ind->edrx_params_valid) ||
      (rr_psm_edrx_ptr->attach_in_prgrs != rr_mtc_nw_config_param_update_ind->attach_in_prgrs) ||
      (rr_psm_edrx_ptr->BS_ePA_MFRMS   != BS_ePA_MFRMS[rr_mtc_nw_config_param_update_ind->edrx_params])
     )
  {
    *update_l1 = TRUE;
    MSG_GERAN_HIGH_0_G("MTC Params changed, update l1");
  }
  else
  {
    MSG_GERAN_HIGH_0_G("MTC Params un-chaged, do not update l1");
    *update_l1 = FALSE;
  }

  rr_psm_edrx_ptr->psm_indicator = rr_mtc_nw_config_param_update_ind->psm_enabled;
  rr_psm_edrx_ptr->edrx_indicator = rr_mtc_nw_config_param_update_ind->edrx_params_valid;
  rr_psm_edrx_ptr->attach_in_prgrs = rr_mtc_nw_config_param_update_ind->attach_in_prgrs;
  
  if (rr_mtc_nw_config_param_update_ind->edrx_params > 11)
  {
    MSG_GERAN_HIGH_1_G("Invalid edrx cycle value = %d", rr_mtc_nw_config_param_update_ind->edrx_params);
    rr_psm_edrx_ptr->edrx_indicator = FALSE;    
  }
  else
  {
    rr_psm_edrx_ptr->BS_ePA_MFRMS   = BS_ePA_MFRMS[rr_mtc_nw_config_param_update_ind->edrx_params];  
  }

  
}

#ifdef FEATURE_GERAN_EXTENDED_DRX 

/*===========================================================================

FUNCTION      rr_get_edrx_indicator

DESCRIPTION   Gets the EDRX support

DEPENDENCIES  None

RETURN VALUE  Edrx support

SIDE EFFECTS  None

===========================================================================*/

boolean rr_get_edrx_indicator(void )
{
  rr_psm_edrx_data_t *rr_psm_edrx_ptr = rr_psm_edrx_get_data_ptr();
  //RR_NULL_CHECK_FATAL(rr_psm_edrx_ptr);
  MSG_GERAN_HIGH_1_G("Edrx indicator set to  = %d", rr_psm_edrx_ptr->edrx_indicator);
  return rr_psm_edrx_ptr->edrx_indicator;
}

/*===========================================================================

FUNCTION      rr_get_edrx_bs_epa_mfrms

DESCRIPTION   Gets the BS_ePA_MFRMS

DEPENDENCIES  None

RETURN VALUE  BS_ePA_MFRMS value

SIDE EFFECTS  None

===========================================================================*/

word rr_get_edrx_BS_ePA_MFRMS( void )
{
  rr_psm_edrx_data_t *rr_psm_edrx_ptr = rr_psm_edrx_get_data_ptr();
  //RR_NULL_CHECK_FATAL(rr_psm_edrx_ptr);
  return rr_psm_edrx_ptr->BS_ePA_MFRMS;
}
#endif /* FEATURE_GERAN_EXTENDED_DRX */

#endif /* FEATURE_GERAN_EXTENDED_DRX  || FEATURE_GERAN_POWER_SAVE_MODE*/

#ifdef FEATURE_GERAN_POWER_EFFICIENT_OPERATION
/*===========================================================================

FUNCTION      rr_reduced_mobility_get_data_ptr

DESCRIPTION   Gets a pointer to reduced mobility data

DEPENDENCIES  None

RETURN VALUE  VOID

SIDE EFFECTS  None

===========================================================================*/

static rr_reduced_mobility_data_t *rr_reduced_mobility_get_data_ptr( void )
{
  return &rr_reduced_mobility_data_spaces;
}

/*===========================================================================

FUNCTION rr_resel_rmc_init

DESCRIPTION
  Resets the RMC state machine and variables etc

PARAMS
  NONE

DEPENDENCIES

RETURN VALUE

===========================================================================*/
void rr_resel_rmc_init( void )
{
  rr_reduced_mobility_data_t *rr_reduced_mobility_data_ptr = rr_reduced_mobility_get_data_ptr();

  rr_reduced_mobility_data_ptr->state = RR_RMC_INACTVE;
  rr_reduced_mobility_data_ptr->old_state = RR_RMC_INACTVE;
  rr_reduced_mobility_data_ptr->C1a = 0;
  rr_reduced_mobility_data_ptr->C1b =  0;
  rr_reduced_mobility_data_ptr->ncell_refresh_count = 0;
  rr_reduced_mobility_data_ptr->ncell_refresh_time = 0;
  rr_reduced_mobility_data_ptr->peo_bcch_change_mark = INVALID_PEO_BCCH_CHANGE_MARK;
  rr_reduced_mobility_data_ptr->eval_reason = RR_RMC_PAGE;
  rr_timer_cancel(RR_RMC_CCCH_READ_TIMER);
  MSG_GERAN_HIGH_0_G("Initialise RESEL RMC params");
}

/*===========================================================================

FUNCTION rr_resel_rmc_c1_delta_init

DESCRIPTION
  Resets the RMC state machine and variables etc

PARAMS
  NONE

DEPENDENCIES

RETURN VALUE

===========================================================================*/
void rr_resel_c1_delta_init(void )
{
  rr_reduced_mobility_data_t *rr_reduced_mobility_data_ptr = rr_reduced_mobility_get_data_ptr();

  rr_reduced_mobility_data_ptr->C1_DELTA = DEFAULT_C1_DELTA;
  rr_reduced_mobility_data_ptr->C1_DELTA_MIN = DEFAULT_C1_DELTA_MIN;
  rr_reduced_mobility_data_ptr->C1_DELTA_MAX = DEFAULT_C1_DELTA_MAX;

  MSG_GERAN_HIGH_0_G("Initialise C1_DELTA params");
}

/*===========================================================================

FUNCTION      rr_ue_in_peo_mode

DESCRIPTION   Returns if PEO is supported  or not

DEPENDENCIES  None

RETURN VALUE  TRUE/FALSE

SIDE EFFECTS  None

===========================================================================*/

boolean rr_ue_in_peo_mode( void)
{
  /*Cell should support PEO*/
  MSG_GERAN_HIGH_2_G("Cell supports PEO= %d, enabled in NV = %d",rr_cell_supports_peo(), rr_nv_get_peo_enabled());
  
#ifdef FEATURE_GERAN_POWER_EFFICIENT_OPERATION   
  if ((rr_cell_supports_peo()) && (rr_nv_get_peo_enabled()))
  {

#if defined(FEATURE_GERAN_EXTENDED_DRX) || defined(FEATURE_GERAN_POWER_SAVE_MODE)
    rr_psm_edrx_data_t *rr_psm_edrx_ptr = rr_psm_edrx_get_data_ptr();
    //RR_NULL_CHECK_FATAL(rr_psm_edrx_ptr);
    /*Either PSM or EDRX needs to be enabled*/
    if (rr_psm_edrx_ptr->psm_indicator)
    {
      MSG_GERAN_HIGH_0_G("PEO is enabled , PSM enabled");
      return TRUE;
    } 
    else if ((rr_psm_edrx_ptr->edrx_indicator) && (FALSE == rr_psm_edrx_ptr->attach_in_prgrs))
    {
      MSG_GERAN_HIGH_0_G("PEO is enabled , EDRX  enabled");
      return TRUE;
    }
    else
    {
      MSG_GERAN_HIGH_1_G("EDRX/PSM not enabled, or Attach in prgrss = %d disable PEO", rr_psm_edrx_ptr->attach_in_prgrs);
      return FALSE;
    }
#endif /* FEATURE_GERAN_EXTENDED_DRX || FEATURE_GERAN_POWER_SAVE_MODE */
  }
  else
#endif /*FEATURE_GERAN_POWER_EFFICIENT_OPERATION*/  
  {
#ifdef FEATURE_GERAN_POWER_EFFICIENT_OPERATION    
    MSG_GERAN_HIGH_0_G("Cell or UE dosent support PEO"); 
#else
    MSG_GERAN_HIGH_0_G("UE dosent support PEO"); 
#endif /*FEATURE_GERAN_POWER_EFFICIENT_OPERATION*/

  }
  return FALSE;
}

/*===========================================================================

FUNCTION      rr_cell_supports_peo

DESCRIPTION   Returns if PEO is supported  by cell or not

DEPENDENCIES  None

RETURN VALUE  TRUE/FALSE

SIDE EFFECTS  None

===========================================================================*/

boolean rr_cell_supports_peo(void )
{
  boolean cell_supports_peo = FALSE;

  if (rr_is_camped())
  {
    gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info();
    cell_supports_peo = scell_info_ptr->bcch_sys_info.peo_params.peo_supported_flag; 
  }
  else
  {
    gprs_scell_info_t *pcell_info_ptr = rr_gprs_get_pcell_info();
    cell_supports_peo = pcell_info_ptr->bcch_sys_info.peo_params.peo_supported_flag; 
  }

#ifdef FEATURE_GERAN_R13_TEST_FRAMEWORK
  if (rr_nv_is_peo_testing_params_present())
  {
    MSG_GERAN_HIGH_0_G("PEO support is through EFS");
    cell_supports_peo = TRUE;
  }
#endif /*FEATURE_GERAN_R13_TEST_FRAMEWORK*/

  return cell_supports_peo;
}

/*===========================================================================

FUNCTION      rr_gprs_get_scell_rcc

DESCRIPTION   Returns RCC, if provided

DEPENDENCIES  None

RETURN VALUE  RCC 

SIDE EFFECTS  None

===========================================================================*/

static uint8 rr_gprs_get_scell_rcc( void)
{
  gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info();
  //RR_NULL_CHECK_FATAL(scell_info_ptr);
   
  if (scell_info_ptr->bcch_sys_info.peo_params.peo_supported_flag)
  {
     MSG_GERAN_HIGH_1_G("PEO RCC in n/w is %d",scell_info_ptr->bcch_sys_info.peo_params.rcc);
    return scell_info_ptr->bcch_sys_info.peo_params.rcc;
  }
#ifdef FEATURE_GERAN_R13_TEST_FRAMEWORK
  if (rr_nv_is_peo_testing_params_present())
  {
    scell_info_ptr->bcch_sys_info.peo_params.rcc = rr_nv_get_peo_rcc_from_efs();
    return scell_info_ptr->bcch_sys_info.peo_params.rcc;
  }
#endif /*FEATURE_GERAN_R13_TEST_FRAMEWORK*/     
  return INVALID_RCC;
}

/*===========================================================================

FUNCTION      rr_store_c1_delta_min_max_from_scell

DESCRIPTION   Returns if C1_DELTA_MIN value

DEPENDENCIES  None

RETURN VALUE  TRUE/FALSE

SIDE EFFECTS  None

===========================================================================*/
static void rr_store_c1_delta_min_max_from_scell( void)
{
  uint8 c1_delta_min = 0;
  uint8 c1_delta_max = 2;
  rr_reduced_mobility_data_t *rr_reduced_mobility_data_ptr = rr_reduced_mobility_get_data_ptr();
  if (rr_is_camped())
  {
    gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info();
    if (scell_info_ptr->bcch_sys_info.peo_params.c1_delta_present)
    {
      c1_delta_min = scell_info_ptr->bcch_sys_info.peo_params.c1_delta_min; 
      c1_delta_max = scell_info_ptr->bcch_sys_info.peo_params.c1_delta_max;
    }
    else
    {
      MSG_GERAN_HIGH_0_G("C1_DELTA MIN not present use default for Scell");
    }
  }
  else
  {
    MSG_GERAN_ERROR_0_G("GRR should be in camped state while storing this value");
  }

#ifdef FEATURE_GERAN_R13_TEST_FRAMEWORK
    if (rr_nv_is_peo_testing_params_present())
    {
      c1_delta_min = rr_nv_get_peo_c1_delta_min_from_efs();
      c1_delta_max = rr_nv_get_peo_c1_delta_max_from_efs();
      MSG_GERAN_HIGH_2_G("C1_DELTA MIN and C1_DELTA MAX in EFS is %d, %d", c1_delta_min, c1_delta_max );
    }
#endif /*FEATURE_GERAN_R13_TEST_FRAMEWORK*/

  rr_reduced_mobility_data_ptr->C1_DELTA_MIN = C1_DELTA_MIN_IN_dBM[c1_delta_min];
  rr_reduced_mobility_data_ptr->C1_DELTA_MAX = C1_DELTA_MAX_IN_dBM[c1_delta_max];

}

/*===========================================================================

FUNCTION      rr_get_c1_delta

DESCRIPTION   Returns if C1_DELTA_MIN value

DEPENDENCIES  None

RETURN VALUE  TRUE/FALSE

SIDE EFFECTS  None

===========================================================================*/

uint8 rr_get_c1_delta_min(void )
{
  rr_reduced_mobility_data_t *rr_reduced_mobility_data_ptr = rr_reduced_mobility_get_data_ptr();

  return rr_reduced_mobility_data_ptr->C1_DELTA_MIN;
}

/*===========================================================================

FUNCTION      rr_get_c1_delta_max

DESCRIPTION   Returns corresponding DB value

DEPENDENCIES  None

RETURN VALUE  TRUE/FALSE

SIDE EFFECTS  None

===========================================================================*/

uint8 rr_get_c1_delta_max(void )
{
  rr_reduced_mobility_data_t *rr_reduced_mobility_data_ptr = rr_reduced_mobility_get_data_ptr();
  
  return rr_reduced_mobility_data_ptr->C1_DELTA_MAX;
}

/*===========================================================================

FUNCTION      rr_update_c1_delta

DESCRIPTION   Returns if C1_DELTA_MAX value

DEPENDENCIES  None

RETURN VALUE  TRUE/FALSE

SIDE EFFECTS  None

===========================================================================*/

void rr_update_c1_delta(rr_c1_delta_update_reason_e update_reason  )
{
  rr_reduced_mobility_data_t *rr_reduced_mobility_data_ptr = rr_reduced_mobility_get_data_ptr();
  
  if ((RR_C1_DELTA_AFTER_PLMN_SELECT == update_reason) || (RR_C1_DELTA_AFTER_CELL_CHANGE == update_reason))
  {
    /*After cell selection, C1_DELTA shall be set to to the value of the C1_DELTA_MIN parameter sent in SI13 for PEO   */
    rr_store_c1_delta_min_max_from_scell();
    rr_reduced_mobility_data_ptr->C1_DELTA = rr_get_c1_delta_min();
    MSG_GERAN_HIGH_1_G("Update CI1_DELTA  to  MIN = %ddB",rr_reduced_mobility_data_ptr->C1_DELTA); 

  }
  else if (RR_C1_DELTA_AFTER_SCELL_NCELL_MEAS == update_reason)
  {  
    uint32                   i;
    int                      best_c1_of_ncell = 0;
    uint8                    num_of_ncell = 0;
    boolean                  scell_present = FALSE;
    int                      recent_c1_of_scell = 0;
    ARFCN_T                  scell_arfcn  = rr_get_previous_camped_BCCH_ARFCN();
    gprs_better_cell_list_T *better_cell_list_ptr = rr_get_better_cell_list_ptr();

    for (i = 0;(i < better_cell_list_ptr->no_of_entries) && (i < MAX_NUMBER_OF_BEST_CELLS); i++)
    {
      if (ARFCNS_EQUAL(better_cell_list_ptr->candidate_cell[i].reselection_data.BCCH_ARFCN, scell_arfcn))
      {
        scell_present = TRUE;
        recent_c1_of_scell = better_cell_list_ptr->candidate_cell[i].reselection_data.C1;
   
      }
      else if (best_c1_of_ncell <= better_cell_list_ptr->candidate_cell[i].reselection_data.C1)
      {
        best_c1_of_ncell = better_cell_list_ptr->candidate_cell[i].reselection_data.C1;
        num_of_ncell++;
      }
      
    }

    /*No neighbour then set to C1_DELTA_MAX*/
    if (scell_present && (num_of_ncell == 0))
    {
      rr_reduced_mobility_data_ptr->C1_DELTA = rr_get_c1_delta_max(); 
      MSG_GERAN_HIGH_1_G("Update C1_DELTA  to  MAX  = %ddB for Zero Neighbor",rr_reduced_mobility_data_ptr->C1_DELTA ); 
    }
    else
    {
      rr_reduced_mobility_data_ptr->C1_DELTA = (uint8)MIN (MAX((recent_c1_of_scell - best_c1_of_ncell),rr_get_c1_delta_min()),rr_get_c1_delta_max());
      MSG_GERAN_HIGH_5_G("C1->Scell= %d, C1 of Strongest N = %d, C1_DELTA_MIN = %ddB, C1_DELTA_MAX =%ddB, C1_DELTA in dB= %ddB",
                          recent_c1_of_scell,
                          best_c1_of_ncell,
                          rr_get_c1_delta_min(),
                          rr_get_c1_delta_max(),
                          rr_reduced_mobility_data_ptr->C1_DELTA 
                         ); 


    }    
   
  }

}

/*===========================================================================

FUNCTION      rr_get_peo_dsc

DESCRIPTION   Returns if PEO DSC value

DEPENDENCIES  None

RETURN VALUE  PEO_DSC value

SIDE EFFECTS  None

===========================================================================*/

int rr_get_peo_dsc( void)
{
  int peo_dsc_max = 4;

  if (rr_is_camped())
  {
    gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info();
    peo_dsc_max = PEO_DSC_VALUE[scell_info_ptr->bcch_sys_info.peo_params.peo_dsc]; 
  }
  else
  {
    gprs_scell_info_t *pcell_info_ptr = rr_gprs_get_pcell_info();
    peo_dsc_max = PEO_DSC_VALUE[pcell_info_ptr->bcch_sys_info.peo_params.peo_dsc]; 
  }

#ifdef FEATURE_GERAN_R13_TEST_FRAMEWORK
  if (rr_nv_is_peo_testing_params_present())
  {
    peo_dsc_max = PEO_DSC_VALUE[rr_nv_get_peo_dsc_from_efs()];
    MSG_GERAN_HIGH_1_G("PEO DSC is  %d",peo_dsc_max);
  }
#endif /*FEATURE_GERAN_R13_TEST_FRAMEWORK*/

  return peo_dsc_max;
}
#endif /*FEATURE_GERAN_POWER_EFFICIENT_OPERATION*/


#if defined (FEATURE_GERAN_EXTENDED_DRX) || defined(FEATURE_GERAN_POWER_EFFICIENT_OPERATION)

/*===========================================================================

FUNCTION      rr_get_l1_edrx_peo_params

DESCRIPTION   Fills L1 edrx peo params

DEPENDENCIES  None

RETURN VALUE  TRUE/FALSE

SIDE EFFECTS  None

===========================================================================*/

void rr_get_l1_edrx_peo_params(layer1_edrx_peo_params_T  *layer1_edrx_peo_params  )
{
  if (layer1_edrx_peo_params != NULL)
  {
    uint32 imsi_mod_10000000 = 0x00000000;
    memset(layer1_edrx_peo_params, 0, sizeof(layer1_edrx_peo_params_T));

#ifdef FEATURE_GERAN_EXTENDED_DRX 

    /*Fill the Edrx  */
    if (rr_get_edrx_indicator())
    {
      layer1_edrx_peo_params->edrx_enabled = TRUE;
      layer1_edrx_peo_params->BS_ePA_MFRMS = rr_get_edrx_BS_ePA_MFRMS();
    }
#endif /* FEATURE_GERAN_EXTENDED_DRX */


#ifdef FEATURE_GERAN_POWER_EFFICIENT_OPERATION 
    /*Reduced N cell monitor needs to be done only if PEO is supported */
    if (rr_ue_in_peo_mode())
    {
      layer1_edrx_peo_params->reduced_ncell_monitor = TRUE;
    }

    layer1_edrx_peo_params->rcc = rr_gprs_get_scell_rcc();
    
#endif /*FEATURE_GERAN_POWER_EFFICIENT_OPERATION*/

    /*Imsi valid  sent independently of PEO suppport */
    layer1_edrx_peo_params->imsi_valid = rr_get_imsi_mod_10000000(&imsi_mod_10000000);
    if (layer1_edrx_peo_params->imsi_valid)
    {
      layer1_edrx_peo_params->imsi_mod_10000000.msb = (word)(imsi_mod_10000000 >> 16);
      layer1_edrx_peo_params->imsi_mod_10000000.lsb = (word)(imsi_mod_10000000 & 0xffff);
    }

  }
}

/*===========================================================================

FUNCTION      rr_edrx_update_l1_params

DESCRIPTION   Update  the PSM/EDRX support

DEPENDENCIES  None

RETURN VALUE  VOID

SIDE EFFECTS  None

===========================================================================*/

void rr_edrx_update_l1_params(rr_l1_update_reason_e reason  )
{
  if (rr_is_camped())
  {
    gprs_scell_info_t  * scell_info = rr_gprs_get_scell_info();
    grr_l1_idle_mode_t  l1_idle_mode = scell_info->l1_idle_mode;

    if (l1_idle_mode == GRR_L1_IDLE_MODE_CCCH )
    {
      ccch_pccch_params_t  ccch_pccch_params;
      rr_gprs_get_l1_params(&ccch_pccch_params);

      if (reason == RR_DSC_DECREMENT )
      {
        ccch_pccch_params.ccch_idle_mode_params.layer1_edrx_peo_params.BS_ePA_MFRMS_DSC = RR_MIN_BS_ePA_MFRMS;
      }
      else
      {
        ccch_pccch_params.ccch_idle_mode_params.layer1_edrx_peo_params.BS_ePA_MFRMS_DSC = 0; //Give the invalid values
      }

      if (reason == RR_MM_READY_TIMER_START)
      {
        MSG_GERAN_HIGH_0_G("Ready timer started, move to lowest eDRX");
        ccch_pccch_params.ccch_idle_mode_params.layer1_edrx_peo_params.BS_ePA_MFRMS = RR_MIN_BS_ePA_MFRMS; //Give the lowest  values
      }

      /* Send updated parameters to L1 */
      rr_send_mph_gprs_l1param_update_req(&ccch_pccch_params);   
    }
    else
    {
      MSG_GERAN_HIGH_0_G("Can't update params to L1");

    }
  }
  else
  {
     MSG_GERAN_HIGH_0_G("UE not camped Can't update params to L1"); 
  }
  
}
#endif /* FEATURE_GERAN_EXTENDED_DRX || FEATURE_GERAN_POWER_EFFICIENT_OPERATION*/


#ifdef FEATURE_GERAN_POWER_EFFICIENT_OPERATION 

/*===========================================================================

FUNCTION rr_resel_state_name

DESCRIPTION
  Returns constant string of the rr resele state name.

PARAMS
  state

DEPENDENCIES
  None

RETURN VALUE
  char * - constant string

===========================================================================*/
static char *rr_reduced_mobility_state_name(
  rr_rmc_state_e state
)
{
  switch (state)
  {
    case RR_RMC_INACTVE:
      return "RMC INACTIVE";

    case RR_RMC_EVALUATE_SCELL:
      return "RMC EVALUATE SCELL";

    case RR_RMC_PEO_BCCH_CHANGE:
      return "RR_RMC_PEO_BCCH_CHANGE";

    default:
      MSG_GERAN_LOW_1("Unknown reselection state: %d", state);
      return "?";
  }
} /* rr_resel_state_name() */

/****************************************************************************
 *   Function name:  rr_rmc_reselection_meas_required
 *   --------------------------
 *
 *   Description: This function is used to store and calculate C1, C2 or C31, C32
 *   ------------ values for the serving cell. These are based upon measurement
 *                reports from L1.
 *
 *   -----------
 *
 *   Returns:  EV_START_RESELECTION or EV_NO_EVENT 
 *   --------
 *
 ****************************************************************************/
static rr_event_T rr_rmc_reselection_meas_required( )
{

  rr_event_T  output_event = EV_NO_EVENT;

  rr_reduced_mobility_data_t *rr_reduced_mobility_data_ptr = rr_reduced_mobility_get_data_ptr();
  gprs_reselection_structure_T *local_gprs_reselection_list_ptr = rr_get_gprs_reselection_list_ptr();
  //RR_NULL_CHECK_FATAL(local_gprs_reselection_list_ptr);

  /*Update C1a and C1b accordingly */
  if ((local_gprs_reselection_list_ptr->serving_values_valid == TRUE) && (local_gprs_reselection_list_ptr->serving_values.C1 > 0))
  {
    rr_reduced_mobility_data_ptr->C1b = local_gprs_reselection_list_ptr->serving_values.C1;
    if (rr_reduced_mobility_data_ptr->C1b >= rr_reduced_mobility_data_ptr->C1a)
    {
      rr_reduced_mobility_data_ptr->C1a = rr_reduced_mobility_data_ptr->C1b;
    }
    MSG_GERAN_HIGH_3_G("RMS: C1a= %d, C1b= %d, C1_DELTA in dB= %d", 
                        rr_reduced_mobility_data_ptr->C1a,
                        rr_reduced_mobility_data_ptr->C1b,
                        rr_reduced_mobility_data_ptr->C1_DELTA);

  }
  
  /****************************/
  /*Step 1. C1 of Serving cell <=0
    /***************************/
  if ((local_gprs_reselection_list_ptr->serving_values_valid == TRUE) && (local_gprs_reselection_list_ptr->serving_values.C1<= 0))
  {
   MSG_GERAN_HIGH_1_G("RMS: Cell Reselection due to C1 less than 0= %d", 
                        local_gprs_reselection_list_ptr->serving_values.C1);
    return EV_START_RESELECTION;
  }

  /****************************/
  /*Step 2. C1a -C1b > C1_DELTA
    /***************************/
  if ((rr_reduced_mobility_data_ptr->C1a - rr_reduced_mobility_data_ptr->C1b) > rr_reduced_mobility_data_ptr->C1_DELTA)
  {
     MSG_GERAN_HIGH_0_G("RMS: Cell Reselection due to C1a- C1b ");
    return EV_START_RESELECTION;
  }

  /************************************/
  /*Step 3. 24 hours since last measuremnet 
    /***********************************/
  if ((rr_reduced_mobility_data_ptr->ncell_refresh_time) >= (RR_NCELL_MEAS_REFERSH_TIMER))
  {
    rr_reduced_mobility_data_ptr->ncell_refresh_count++;
    if ((rr_reduced_mobility_data_ptr->ncell_refresh_count) >= (RR_NCELL_MEAS_REFERSH_COUNTER))
    {
      MSG_GERAN_HIGH_0_G("RMS: Cell Reselection due to 24 hours since last RPS ");
      return EV_START_RESELECTION;
    }    
  }
  return output_event;
}

/****************************************************************************
 *   Function name:  rr_rmc_reselection_meas_required
 *   --------------------------
 *
 *   Description: This function is used to store and calculate C1, C2 or C31, C32
 *   ------------ values for the serving cell. These are based upon measurement
 *                reports from L1.
 *
 *   Parameters: No input parameters
 *   -----------
 *
 *   Returns:  none
 *   --------
 *
 ****************************************************************************/
static rr_event_T rr_rmc_path_loss_for_GSM(void )
{

  rr_event_T  output_event = EV_NO_EVENT;

  gprs_reselection_structure_T *local_gprs_reselection_list_ptr = rr_get_gprs_reselection_list_ptr();
  //RR_NULL_CHECK_FATAL(local_gprs_reselection_list_ptr);

 
  /****************************/
  /*Step 1. C1 of Serving cell <=0
    /***************************/
  if ((local_gprs_reselection_list_ptr->serving_values_valid == TRUE) && (local_gprs_reselection_list_ptr->serving_values.C1<= 0))
  {
    MSG_GERAN_HIGH_1_G("RMS: Cell Reselection due to C1 less than 0= %d", 
                        local_gprs_reselection_list_ptr->serving_values.C1);
    output_event = EV_START_RESELECTION;
  }

  return output_event;
}

/****************************************************************************
 *   Function name:  rr_rmc_update_ncell_refresh_time
 *   --------------------------
 *
 *   Description: This function is used to update the N cell refersh time count
 *
 *   Parameters: No input parameters
 *   -----------
 *
 *   Returns:  none
 *   --------
 *
 ****************************************************************************/
void rr_rmc_update_ncell_refresh_time_duration(void )
{
  rr_reduced_mobility_data_t *rr_reduced_mobility_data_ptr = rr_reduced_mobility_get_data_ptr();
  int                BS_PA_MFRMS = 0;
#ifdef FEATURE_GERAN_EXTENDED_DRX   
  if (rr_get_edrx_indicator())
  {
    BS_PA_MFRMS =  rr_get_edrx_BS_ePA_MFRMS();
  }
  else
#endif /* FEATURE_GERAN_EXTENDED_DRX */
  {
    gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info();
    int                BS_PA_MFRMS = 0;
    if (scell_info_ptr && scell_info_ptr->gsm.valid_data_flag)
    {
      BS_PA_MFRMS = (int)(scell_info_ptr->gsm.control_channel_description.BS_PA_MFRMS);
    }

    if (BS_PA_MFRMS == 0)
    {
      /* default to smallest value (corresponding to fastest paging cycle) */
      BS_PA_MFRMS = BS_PA_MFRMS_MIN;
    }
  }
   rr_reduced_mobility_data_ptr->ncell_refresh_time += (RR_DSC_DRX_DURATION_PER_MULTIFRAME * BS_PA_MFRMS);    
}

/*===========================================================================

FUNCTION rr_reduced_mobility_control

DESCRIPTION
  Main rr cell reselection state machine.

PARAMS
  event   - Event to be acted on.
  message - Message content

DEPENDENCIES
  None

RETURN VALUE
  rr_event_T - return control event

===========================================================================*/
rr_event_T rr_reduced_mobility_control(
  rr_event_T event,
  rr_cmd_bdy_type *message
   
)
{
  byte                                message_id = 0;
  byte                                message_set = 0;
  rr_event_T                          new_event = EV_NO_EVENT;
  rr_event_T                          output_event = EV_NO_EVENT;

  // Obtain a pointer to the module data
  rr_reduced_mobility_data_t *rr_reduced_mobility_data_ptr = rr_reduced_mobility_get_data_ptr();
  //RR_NULL_CHECK_FATAL(rr_reduced_mobility_data_ptr);
  while (event != EV_NO_EVENT)
  {
    event = rr_extract_message_header_details(
      event,
      message,
      &message_set,
      &message_id
    );

    /* State independent event/signal handler */
    switch (event)
    {
      case EV_NO_EVENT:
        continue;

      case EV_RESET_SOFTWARE:
      {
        rr_resel_rmc_init();
        rr_reduced_mobility_data_ptr->state = RR_RMC_INACTVE;
        rr_reduced_mobility_data_ptr->old_state = RR_RMC_INACTVE;
        event = EV_NO_EVENT;
        return EV_NO_EVENT;
      }
      default:
        /*no action*/
       break;
    }

    /* State dependent event/signal handler */
    switch (rr_reduced_mobility_data_ptr->state)
    {
      case RR_RMC_INACTVE:
      {
        switch (event)
        {
          case EV_INPUT_MESSAGE:
          {
            switch (message_set)
            {
              case MS_RR_RR:
              {
                switch (message->rr.header.rr_message_set)
                {
                  case RR_RESEL_RMC_IMSG:
                  {
                    switch (message->message_header.message_id)
                    {
                      case RR_RESEL_RMC_IMSG_START_IND:
                      {
                        rr_reduced_mobility_start_reason_e reason = message->rr.resel_rmc.imsg.rmc_start_ind.reason;
                        if (reason == RR_REDUCED_MOBILITY_CELL_SELECTION)
                        {
                          new_event = EV_PATH_LOSS_TIMEOUT;
                        }
                        rr_reduced_mobility_data_ptr->state = RR_RMC_EVALUATE_SCELL;
                      }
                    }
                    
                    break;
                  }

                  default:
                    LOG_UNEXPECTED_MESSAGE(message, rr_reduced_mobility_state_name(rr_reduced_mobility_data_ptr->state));
                  break;
                }
                break;
              } //MS_RR_L1

              default:
               LOG_UNEXPECTED_MESSAGE(message, rr_reduced_mobility_state_name(rr_reduced_mobility_data_ptr->state));
              break;
            }            
          }
          break;          

          case EV_START_RESELECTION:
          {
            /*During RACH procedure if so happens that reslection gets triggered tdue to RCC change then 
                        reduced mobility control gets reset  so if RR_RESEL module if still in reduced mobility state sends the 
                        message then RMC ocntrol should blindly send the confirmation back*/
            rr_resel_rmc_send_omsg_find_cell_ind();
          }
          break;  

          default:
            LOG_UNEXPECTED_MESSAGE(message, rr_reduced_mobility_state_name(rr_reduced_mobility_data_ptr->state));
          break;

        }
        break; 
      }
      break;
      /**
          * RR_RMS_EVALUATE_SCELL
          * Idle state, no reselection is taking place.
          *
          * In this state:
          *   Measurements are used to detect path loss.
          *
          * */    
      case RR_RMC_EVALUATE_SCELL:
      {
        switch (event)
        {
          case EV_INPUT_MESSAGE:
          {
            switch (message_set)
            {
              case MS_RR_L1:
              {
                switch ((rr_l1_message_id_enum_T) message_id)
                {
                  case MPH_SERVING_MEAS_IND:
                  {
                    rr_gprs_store_serving_meas(&message->mph_serving_meas_ind);
                    new_event = EV_PATH_LOSS_TIMEOUT;
                    break;
                  }/*MPH_SERVING_MEAS_IND*/

                  case MPH_SERVING_IDLE_MEAS_IND:
                  {
                    rr_gprs_store_serving_idle_meas(&message->mph_serving_idle_meas_ind);
                    new_event = EV_PATH_LOSS_TIMEOUT;
                    break;
                  }

                  case MPH_SERVING_AUX_MEAS_IND:
                  {
                    rr_gprs_process_serving_aux_meas_ind(&message->mph_serving_aux_meas_ind);                                        
                    new_event = EV_PATH_LOSS_TIMEOUT;
                    break;
                  }

                  default:
                    LOG_UNEXPECTED_MESSAGE(message, rr_reduced_mobility_state_name(rr_reduced_mobility_data_ptr->state));
                  break;
                }
                break;
              } //MS_RR_L1

              case MS_RR_RR:
              {
                switch (message->rr.header.rr_message_set)
                {
                  case RR_RESEL_RMC_IMSG:
                  {
                    switch (message->message_header.message_id)
                    {
                      case RR_RESEL_RMC_IMSG_PS_DATA_REQ:
                      {
                        rr_reduced_mobility_data_ptr->eval_reason = RR_RMC_PS_REQ;
                        /*As per rel13 sepc 44.018 A mobile station that has enabled PEO shall, when attempting to establish a PS connection */
                        /*and while ignoring MS identities included within PAGING REQUEST messages, be able to */
                        /* successfully decode an RR message within 500ms of starting to listen to the downlink CCCH. */
                        /* Otherwise, the packet access procedure is aborted, a random access failure is indicated to upper layers and autonomous */
                        /* cell re-selection is performed according to 3GPP TS 43.022.*/
                        /* Here Gl1 will continue to read CCCH till either it succesffuly decodes or timer RR_RMC_CCCH_READ_TIMER expires*/
                        MSG_GERAN_HIGH_0_G("Start 500ms wait timer for CCH read");                                          
                        rr_timer_start(RR_RMC_CCCH_READ_TIMER,RR_RMC_CCCH_READ_TIMER_DURATION);
                        rr_send_mph_serving_meas_req();
                        break;
                      }                      

                      case RR_RESEL_RMC_IMSG_CS_DATA_REQ:
                      {
                        if (MAC_TRANSFER == rr_macinfo_get_mac_status())
                        {
                          MSG_GERAN_HIGH_0_G("Pkt txfer in prgss, send conf back immediately.");                                          
                          rr_resel_rmc_send_omsg_cs_data_cnf();
                        }
                        else
                        {                          
                          rr_reduced_mobility_data_ptr->eval_reason = RR_RMC_CS_REQ;
                          MSG_GERAN_HIGH_0_G("Start 500ms wait timer for CCH read for CS case");                                          
                          rr_timer_start(RR_RMC_CCCH_READ_TIMER,RR_RMC_CCCH_READ_TIMER_DURATION);
                          rr_send_mph_serving_meas_req();                       
                        }
                        break;
                      }

                      case RR_RESEL_RMC_IMSG_CCCH_MSG_RCVD_IND:
                      {
                        MSG_GERAN_HIGH_0_G("Stop CCCH read timer");
                        /*Cancel any READ CCCH timer if running*/
                        rr_timer_cancel(RR_RMC_CCCH_READ_TIMER);
                        if (message->rr.resel_rmc.imsg.ccch_msg_ind.rcc_changed)
                        {
                          new_event = EV_START_RESELECTION;
                        }
                        else if (message->rr.resel_rmc.imsg.ccch_msg_ind.peo_bcch_changed)
                        {
                          if (rr_reduced_mobility_data_ptr->eval_reason != RR_RMC_CS_REQ)
                          {
                            output_event = EV_BCCH_CHANGE_DETECTED;                          
                            rr_reduced_mobility_data_ptr->state = RR_RMC_PEO_BCCH_CHANGE;
                          }
                        }
                        break;
                      }

                      case RR_RESEL_RMC_IMSG_START_IND:
                      {
                        /*This message is ignored*/
                        break;
                      }

                      default:
                      LOG_UNEXPECTED_MESSAGE(message, rr_reduced_mobility_state_name(rr_reduced_mobility_data_ptr->state));
                    }
                    break;
                  }                      

                  default:
                    LOG_UNEXPECTED_MESSAGE(message, rr_reduced_mobility_state_name(rr_reduced_mobility_data_ptr->state));

                }
                break;
              }//MS_RR_RR       

              case MS_TIMER:
              {
                rr_timer_id_T timer_id = (rr_timer_id_T)message->mid_timer_expiry.timer_id;

                switch (timer_id)
                {
                  case RR_RMC_CCCH_READ_TIMER:
                  {
                    MSG_GERAN_HIGH_0_G("Procedure failed - timeout, trigger reselection");
                    new_event = EV_START_RESELECTION;
                    break;
                  }

                  default:
                  {
                    MSG_GERAN_ERROR_1_G("Unhandled timer (%d)",(int)timer_id);
                  }
                }
                break;
              } //MS_TIMER
              
              default:
               LOG_UNEXPECTED_MESSAGE(message, rr_reduced_mobility_state_name(rr_reduced_mobility_data_ptr->state));
              break;
            }            
          }
          break;

          /****************************************************************/
          /**********Reselection Decision**** */
          /**********Evalaute S celll************ */
          /****************************************************************/

          case EV_PATH_LOSS_TIMEOUT:
          {
            switch (rr_reduced_mobility_data_ptr->eval_reason)
            {
              case RR_RMC_PS_REQ:
              {
                /*Cancel any READ CCCH timer if running*/
                rr_timer_cancel(RR_RMC_CCCH_READ_TIMER);
                new_event = rr_rmc_path_loss_for_GSM();

                if (new_event == EV_NO_EVENT)
                {
                  rr_reduced_mobility_data_ptr->eval_reason = RR_RMC_PAGE; 
                  rr_resel_rmc_send_omsg_ps_data_cnf();
                }
                break;
              }              

              case RR_RMC_CS_REQ:
              {
                /*Cancel any READ CCCH timer if running*/
                rr_timer_cancel(RR_RMC_CCCH_READ_TIMER);
                new_event = rr_rmc_path_loss_for_GSM();

                if (new_event == EV_NO_EVENT)
                {
                  rr_reduced_mobility_data_ptr->eval_reason = RR_RMC_PAGE; 
                  rr_resel_rmc_send_omsg_cs_data_cnf();
                }
                break;
              }
              default:
              {
                new_event = rr_rmc_reselection_meas_required();
              }
            }
          }
          break;          

          /****************************************************************/
          /*                     Reselection Triggered                      */
          /*                     --------------------                     */
          /****************************************************************/

          case EV_START_RESELECTION:
          {
            switch (rr_reduced_mobility_data_ptr->eval_reason)
            {
              case RR_RMC_PS_REQ:
              {
                rr_delete_saved_mac_ul_tbf_req();
                /*The below message will be ignored but just send it*/
                rr_resel_rmc_send_omsg_ps_data_cnf();
                break;
              }              

              case RR_RMC_CS_REQ:
              {
#ifdef FEATURE_GERAN_CS_SUPPORT
                if (grr_verify_saved_mm_message(RR_EST_REQ))
                {
                  rr_send_abort_ind(OTHER_REASON, MM_RR_OTHER_FAILURE);
                  grr_throwaway_saved_message();
                }
#endif /* FEATURE_GERAN_CS_SUPPORT */

                /*The below message will be ignored but just send it*/
                rr_resel_rmc_send_omsg_cs_data_cnf();
                break;
              }
              default:
              break;
     
            }
            rr_resel_rmc_send_omsg_find_cell_ind();          
            rr_resel_rmc_init();
          }
          break;              

          default:
           LOG_UNEXPECTED_MESSAGE(message, rr_reduced_mobility_state_name(rr_reduced_mobility_data_ptr->state));

        }
        break; 
      }  /*RR_RMS_EVALUATE_SCELL*/
      break;
      /**
          * RR_RMC_PEO_BCCH_CHANGE
          * PEO_BCCH_DETECTED, wait for SI readcompletion.
          *
          * In this state:
          *   Measurements are used to detect path loss.
          *
          * */    
      case RR_RMC_PEO_BCCH_CHANGE:
      {
        switch (event)
        {
          case EV_INPUT_MESSAGE:
          {
            switch (message_set)
            {
              case MS_RR_L1:
              {
                switch ((rr_l1_message_id_enum_T) message_id)
                {
                  case MPH_SERVING_MEAS_IND:
                  {
                    rr_gprs_store_serving_meas(&message->mph_serving_meas_ind);
                    new_event = EV_PATH_LOSS_TIMEOUT;
                    break;
                  }/*MPH_SERVING_MEAS_IND*/

                  case MPH_SERVING_IDLE_MEAS_IND:
                  {
                    rr_gprs_store_serving_idle_meas(&message->mph_serving_idle_meas_ind);
                    new_event = EV_PATH_LOSS_TIMEOUT;
                    break;
                  }

                  case MPH_SERVING_AUX_MEAS_IND:
                  {
                    rr_gprs_process_serving_aux_meas_ind(&message->mph_serving_aux_meas_ind);                                        
                    new_event = EV_PATH_LOSS_TIMEOUT;
                    break;
                  }

                  default:
                    LOG_UNEXPECTED_MESSAGE(message, rr_reduced_mobility_state_name(rr_reduced_mobility_data_ptr->state));
                  break;
                }
                break;
              } //MS_RR_L1

              case MS_RR_RR:
              {
                switch (message->rr.header.rr_message_set)
                {
                  case RR_RESEL_RMC_IMSG:
                  {
                    switch (message->message_header.message_id)
                    {                      

                      case RR_RESEL_RMC_IMSG_PS_DATA_REQ:
                      {
                        rr_reduced_mobility_data_ptr->eval_reason = RR_RMC_PS_REQ;
                        break;
                      }
                    
                      case RR_RESEL_RMC_IMSG_CCCH_MSG_RCVD_IND:
                      {
                        if (message->rr.resel_rmc.imsg.ccch_msg_ind.rcc_changed)
                        {
                          new_event = EV_START_RESELECTION;
                        }                        
                        break;
                      }
 
                      case RR_RESEL_RMC_IMSG_BSI_ACQ_COMPLETE_IND:
                      {            
                        switch (rr_reduced_mobility_data_ptr->eval_reason)
                        {
                          case RR_RMC_PS_REQ:
                          {
                            rr_resel_rmc_send_omsg_ps_data_cnf();
                            break;
                          }
                          default:;
                        }

                        MSG_GERAN_HIGH_0_G("RMC: BSI Complete recvd");
                        rr_reduced_mobility_data_ptr->state = RR_RMC_EVALUATE_SCELL;
                        break;
                      }

                      default:
                      LOG_UNEXPECTED_MESSAGE(message, rr_reduced_mobility_state_name(rr_reduced_mobility_data_ptr->state));
                    }
                    break;
                  }                      

                  default:
                      LOG_UNEXPECTED_MESSAGE(message, rr_reduced_mobility_state_name(rr_reduced_mobility_data_ptr->state));

                }
                break;
              }//MS_RR_RR
              
              default:
               LOG_UNEXPECTED_MESSAGE(message, rr_reduced_mobility_state_name(rr_reduced_mobility_data_ptr->state));
              break;
            }            
          }
          break;

          /****************************************************************/
          /**********Reselection Decision**** */
          /**********Evalaute S celll************ */
          /****************************************************************/

          case EV_PATH_LOSS_TIMEOUT:
          {
            switch (rr_reduced_mobility_data_ptr->eval_reason)
            {
              case RR_RMC_PS_REQ:
              {
                new_event = rr_rmc_path_loss_for_GSM();
                break;
              }
              default:
              {
                new_event = rr_rmc_reselection_meas_required();
              }
            }
          }
          break;          

          /****************************************************************/
          /*                     Reselection Triggered                      */
          /*                     --------------------                     */
          /****************************************************************/

          case EV_START_RESELECTION:
          {
            switch (rr_reduced_mobility_data_ptr->eval_reason)
            {
              case RR_RMC_PS_REQ:
              {
                rr_delete_saved_mac_ul_tbf_req();
                /*The below message will be ignored but just send it*/
                rr_resel_rmc_send_omsg_ps_data_cnf();
                break;
              }
              default:
              break;
     
            }
            rr_resel_rmc_send_omsg_find_cell_ind();          
            rr_resel_rmc_init();
          }
          break;              

          default:
            LOG_UNEXPECTED_MESSAGE(message, rr_reduced_mobility_state_name(rr_reduced_mobility_data_ptr->state));
          
        }
        break; 
      } /*RR_RMC_PEO_BCCH_CHANGE*/                    
      break;

      default:
        LOG_UNEXPECTED_MESSAGE(message, rr_reduced_mobility_state_name(rr_reduced_mobility_data_ptr->state));       
     
    }
    /* Check for a change in state and log appropriately. */
    if (rr_reduced_mobility_data_ptr->state != rr_reduced_mobility_data_ptr->old_state)
    {      
      MSG_SPRINTF_2(MSG_SSID_DFLT,
                    MSG_LEGACY_HIGH,
                    "rr_reduced_mobility_control(%s->%s)",
                    rr_reduced_mobility_state_name(rr_reduced_mobility_data_ptr->old_state),
                    rr_reduced_mobility_state_name(rr_reduced_mobility_data_ptr->state));
    
    }    

    if (new_event != EV_NO_EVENT)
    {
      event = new_event;
      new_event = EV_NO_EVENT;
    }
    else
    {
      event = EV_NO_EVENT;
    }

    rr_reduced_mobility_data_ptr->old_state = rr_reduced_mobility_data_ptr->state;
  }/* while (event != EV_NO_EVENT) */

  return output_event;
}

/*===========================================================================

FUNCTION      rr_process_rcc_or_bcch_change

DESCRIPTION   Checks for change in RCC or PEO_BCCH_CHANGE and acts accordingly 

DEPENDENCIES  None

RETURN VALUE  TRUE/FALSE

SIDE EFFECTS  None

===========================================================================*/

boolean rr_process_rcc_or_bcch_change(uint8 new_rcc, uint8 peo_bcch_change_mark   )
{
  if ((rr_ue_in_peo_mode()) && (rr_is_camped()))
  {
  
    gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info();
    rr_reduced_mobility_data_t *rr_reduced_mobility_data_ptr = rr_reduced_mobility_get_data_ptr();

    //RR_NULL_CHECK_FATAL(rr_reduced_mobility_data_ptr);

    /*Check for RCC change and if  TRUE trigger reselection*/
    if ((new_rcc != INVALID_RCC) && (new_rcc != scell_info_ptr->bcch_sys_info.peo_params.rcc))
    {
      MSG_GERAN_HIGH_2_G("RCC changed New RCC = %d, OLD RCC= %d", new_rcc, scell_info_ptr->bcch_sys_info.peo_params.rcc );
      rr_resel_rmc_send_imsg_ccch_msg_rcvd_ind(TRUE, FALSE);
      return TRUE;

    }
  
    /*Update PEO_BCCH_CHANGE_MARK if not done*/
    if (rr_reduced_mobility_data_ptr->peo_bcch_change_mark == INVALID_PEO_BCCH_CHANGE_MARK)
    {
      rr_reduced_mobility_data_ptr->peo_bcch_change_mark = peo_bcch_change_mark;
    }

    /*Check for change in PEO_BCCH_CHANGE MARK if valid*/
    else if ((INVALID_PEO_BCCH_CHANGE_MARK != peo_bcch_change_mark) &&
             (rr_reduced_mobility_data_ptr->peo_bcch_change_mark != peo_bcch_change_mark)
            ) 
    {
      /* timer expiry and timer expiry callback */
      if (rr_timer_is_running(GRR_SYS_INFO_REFRESH_TIMER))
      {
        MSG_GERAN_HIGH_1_G("BCCH_CHANGE MARK detected new BCCH is %d, re-read SI13", peo_bcch_change_mark );
        rr_reduced_mobility_data_ptr->peo_bcch_change_mark = INVALID_PEO_BCCH_CHANGE_MARK;
        rr_resel_rmc_send_imsg_ccch_msg_rcvd_ind(FALSE, TRUE);
        return TRUE;
      }
    }// PEO BCCH change mark detected  

    /*Send RR_RESEL_RMC_IMSG_CCCH_MSG_RCVD_IND to RMC module to stop  RR_RMC_READ_CCH_TIMER if running*/
    if (rr_timer_is_running(RR_RMC_CCCH_READ_TIMER))
    {
      rr_resel_rmc_send_imsg_ccch_msg_rcvd_ind(FALSE, FALSE);
      MSG_GERAN_HIGH_2_G("RCC = %d , PEO_BCCH_CHANGE MARK =%d",
                          scell_info_ptr->bcch_sys_info.peo_params.rcc, 
                          rr_reduced_mobility_data_ptr->peo_bcch_change_mark);  
    }
  }//PEO is disabled 
  else
  {
    MSG_GERAN_HIGH_0_G("PEO is disabled");  
  }

  return FALSE;
}

boolean rr_expire_refresh_timer( )
{      
  if (rr_timer_is_running(GRR_SYS_INFO_REFRESH_TIMER))
  {
    /* timer expiry and timer expiry callback */
    if (rr_timer_force_timer_expiry(GRR_SYS_INFO_REFRESH_TIMER))
    {
      return TRUE;
    }
  }

  return FALSE;
}

/****************************************************************************
 *   Function name:  rr_rmc_ps_eval_in_prgrs
 *   --------------------------
 *
 *   Description: This function is used to inform if PS rach was requested 
 *
 *   Parameters: No input parameters
 *   -----------
 *
 *   Returns:  TRUE if yes FALSE otherwise
 *   --------
 *
 ****************************************************************************/
boolean rr_rmc_ps_eval_in_prgrs(void )
{
  rr_reduced_mobility_data_t *rr_reduced_mobility_data_ptr = rr_reduced_mobility_get_data_ptr();
  
  if (rr_reduced_mobility_data_ptr->eval_reason == RR_RMC_PS_REQ)
  {
    return TRUE;
  }

  return FALSE;
}

#endif /*FEATURE_GERAN_POWER_EFFICIENT_OPERATION*/

#endif /* FEATURE_GERAN_EXTENDED_DRX || FEATURE_GERAN_POWER_EFFICIENT_OPERATION || FEATURE_GERAN_POWER_SAVE_MODE*/

#ifdef PERLUTF
#error code not present
#endif /*PERLUTF*/

/* EOF */

