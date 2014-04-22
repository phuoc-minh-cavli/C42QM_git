#ifndef RR_REDUCED_MOBILITY_H
#define RR_REDUCED_MOBILITY_H

/*============================================================================
  @file rr_resel.h

  Signalling interface for the RR reselection module.

                Copyright (c) 2002, 2008-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //components/rel/geran.mpss/5.2.0/grr/src/rr_reduced_mobility.h#2 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "geran_dual_sim.h"
#include "geran_multi_sim.h"

#if defined (FEATURE_GERAN_EXTENDED_DRX) || defined(FEATURE_GERAN_POWER_EFFICIENT_OPERATION) || defined(FEATURE_GERAN_POWER_SAVE_MODE) 
#include "rr_events.h"
#include "rr_task.h"
#include "rr_message_header.h"
#include "rr_defs.h"
#include "gprsdef_g.h"
#include "rr_reduced_mobility_if.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
 /*----------------------------------------------------------------------------
 * Local Defines
 *--------------------------------------------------------------------------*/
#ifdef FEATURE_GERAN_POWER_EFFICIENT_OPERATION  
#define RR_NCELL_MEAS_REFERSH_TIMER           60000  /* 60 min  */
#define RR_NCELL_MEAS_REFERSH_COUNTER         1440  /* 24*60  */
#define INVALID_PEO_BCCH_CHANGE_MARK          0xFF
#define INVALID_RCC                           0xFF
#define DEFAULT_C1_DELTA                      3 /*3dB*/
#define DEFAULT_C1_DELTA_MIN                  3 /*3dB*/
#define DEFAULT_C1_DELTA_MAX                  9 /*3dB*/
#endif /*FEATURE_GERAN_POWER_EFFICIENT_OPERATION*/

#define RR_MIN_BS_ePA_MFRMS                   8
#define RR_MAX_BS_ePA_MFRMS                   13312

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
  @brief Initialise the RR reselection module.
*/
#if defined(FEATURE_GERAN_EXTENDED_DRX) || defined(FEATURE_GERAN_POWER_SAVE_MODE)

extern void rr_init_psm_edrx_params( void );
extern void rr_set_psm_edrx_support(rr_mtc_nw_config_param_update_ind_T *rr_mtc_nw_config_param_update_ind, boolean *update_l1 );
#endif /* FEATURE_GERAN_EXTENDED_DRX  || FEATURE_GERAN_POWER_SAVE_MODE*/

#if defined (FEATURE_GERAN_EXTENDED_DRX) || defined(FEATURE_GERAN_POWER_EFFICIENT_OPERATION)

extern void rr_edrx_update_l1_params(rr_l1_update_reason_e reason );
extern void rr_get_l1_edrx_peo_params(layer1_edrx_peo_params_T  *layer1_edrx_peo_params );

#endif /* FEATURE_GERAN_EXTENDED_DRX || FEATURE_GERAN_POWER_EFFICIENT_OPERATION */


#ifdef FEATURE_GERAN_EXTENDED_DRX 
extern boolean rr_get_edrx_indicator( void );
extern word rr_get_edrx_BS_ePA_MFRMS( void );
#endif /* FEATURE_GERAN_EXTENDED_DRX */



#ifdef FEATURE_GERAN_POWER_EFFICIENT_OPERATION  

extern void rr_resel_rmc_init( void );

extern void rr_resel_c1_delta_init( void );
extern boolean rr_ue_in_peo_mode( void );

extern boolean rr_cell_supports_peo( void );

extern uint8 rr_get_c1_delta_min(void );

extern uint8 rr_get_c1_delta_max(void );

extern void rr_update_c1_delta(rr_c1_delta_update_reason_e update_reason);

extern int rr_get_peo_dsc( void );

extern void rr_rmc_update_ncell_refresh_time_duration( void );

extern rr_event_T rr_reduced_mobility_control(
  rr_event_T        event,
  rr_cmd_bdy_type * message
);

extern boolean rr_process_rcc_or_bcch_change(uint8 new_rcc, uint8 peo_bcch_change_mark   );

extern boolean rr_expire_refresh_timer(void );

extern boolean rr_rmc_ps_eval_in_prgrs(void );
#endif /*FEATURE_GERAN_POWER_EFFICIENT_OPERATION*/

#endif /* FEATURE_GERAN_EXTENDED_DRX || FEATURE_GERAN_POWER_EFFICIENT_OPERATION || FEATURE_GERAN_POWER_SAVE_MODE*/
#ifdef PERLUTF
#error code not present
#endif /*PERLUTF*/

#endif /* #ifndef RR_REDUCED_MOBILITY_H */

/* EOF */

