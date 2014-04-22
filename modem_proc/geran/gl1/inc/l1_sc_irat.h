#ifndef L1_SC_IRAT_H
#define L1_SC_IRAT_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        INTERRAT (WCDMA) SURROUND CELL

GENERAL DESCRIPTION
   This module contains routines specific to performing WCDMA interRAT
   measurements while in GSM.

EXTERNALIZED FUNCTIONS
   l1_sc_wcdma_update_list
   l1_sc_ded_wcdma_update_list
   l1_sc_irat_check_active
   l1_sc_wcdma_get_best_six
   l1_sc_wcdma_set_paging_multiframes
   l1_sc_wcdma_init
   l1_sc_wcdma_enter_idle
   l1_sc_wcdma_leave_idle
   l1_sc_wcdma_enter_xfer
   l1_sc_wcdma_enter_dedi
   l1_sc_check_priority_threshold
   l1_sc_wcdma_ded_pending
   l1_sc_wcdma_ded_ready
   l1_sc_wcdma_receive
   l1_sc_wcdma_srch_active
   l1_sci_tick_wcdma
   l1_sci_wcdma_abort_reconfirm
   l1_sci_wcdma_paging_block_tick
   l1_sc_lte_process_timed_search_results

   l1_sc_populate_cm_g2l
   l1_sc_cm_g2l_state
   l1_sc_wcdma_ded_tick

INITIALIZATION AND SEQUENCING REQUIREMENTS


*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/geran.mpss/5.2.0/gl1/inc/l1_sc_irat.h#2 $
$DateTime: 2018/10/10 02:31:27 $ $Author: pwbldsvc $

when       who       what, where, why
--------   --------  ---------------------------------------------------------
21/01/16   mk        CR957771 Avoid error recovery when G2W scripts are built during wakeup
22/12/15   mko       CR954226: Update the l1 parameters in ilde waiting state. Correcting the CCCH gap logging issue
04/02/15   fm        CR791018 GL1 should check SIM mode for NCell SCH Rx before starting it
28/01/15   fm        CR784997 GL1 checks if the current TRM reservation is for NCell SCH Rx before starting it
10/12/14   sjv       CR769423 Ensure l1_idle_mode_isr returns TRUE only when it is safe to release TRM 
12/12/14   sk        CR763710 Ensure NBCCH is read when consistently colliding with serving PCH or other sub's PCH
12/02/14   am        CR754046 WLAN-COEX: End high priority indication if activity is finished
19/11/14   sk        CR743686: Ensure Frames for the Cb activity are cleared when new scheduling/skip req received
04/11/14   df        CR734490 Rework the wake up recovery mechanism
01/11/14   df        CR748602 Rework and enable adaptive timeline
17/10/14   sk        CR743536 Ensure gap to CCCH is updated on rude wakeup before ISR runs
12/09/14   fan       CR636420: Run Sleep commit after GSTMR ISR at wakeup
21/02/14   sp       CR635810: Don't release TRM lock for access when the lock is granted after request_and_notify(), improve CR/620698
08/04/14   cs        CR638933 Allow equal priority event to be booked when suspend is set
25/02/14   sk        CR614910 Update G BPLMN handling inline with DSDA-OMRD implementation
21/02/14   sk/smd    CR620698:Don't release TRM lock for access when the lock is granted after request_and_notify()
09/01/14   zf        CR598637: DRX only updates TRM booking if DRX activity is matching with the TRM booking
15/11/13   js        CR572966:Reselections to the Ncell are still failing as SUB2 is not able to decode all the SIs leading to SYS_INFO_TIMER expiry
11/20/13   nk        CR 549654 : Preventing scheduling of bcch state machine when frame manager has aborted that activity
15/10/13   js        CR508118: Send missed block ind if wakeup for CCCH cancelled.
01/10/13   sk        CR552259 CR sync from 1.0/1.1
29/08/13   us      CR519643 Avoid ncell acquisition or delay ncell scheduling when bcch schedule is within 48msec
15/08/13   sk        CR524039 GBTA bringup changes
18/09/13   us      CR519643 Avoid ncell acquisition or delay ncell scheduling when bcch schedule is within 48msec
29/07/13   sk        CR519663 Partial QBTA bringup changes
10/06/13   cs        TSTS Updates
29/10/12   ss        CR396846 : Process Read sys info request only if next BCCH read does not coincides with active G2X measurement
                     scheduled in frame manager
10/02/12   ws        CR 336194 Enable TD-SCDMA IRAT for GERAN
20/06/11   cs        Add in CBCH for DSDS using individual priority
29/11/10   og        Adding G2L support.
19/11/10   nt        DSDS Enhancement to SCE scheduling during Tuneaway
10/18/05   sv        Add function prototype to return gap to next ccch\sdcch block.
08/04/05   sv        Add new logging event.
05/23/05   sv        Remove function prototype l1_fm_update_ccch_gap.
04/11/05   sv        Added new function prototype that updates the CCCH gap
                     in Frame Manager.
02/05/05   bm        Increased FM_PRI_BACKGROUND_PLMN priority next to CCCH
01/26/05   sv        Add support for Frame Manager.

===========================================================================*/

#include "geran_variation.h"
#ifdef CUST_H
#include "customer.h"
#endif

#if defined ( FEATURE_WCDMA ) || defined ( FEATURE_GSM_TDS )|| defined ( FEATURE_GSM_TO_LTE )
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "err.h"

#ifdef  FEATURE_GSM_TO_LTE

#include "lte_as.h"
#include "lte_l1_types.h"
#include "lte_cphy_irat_meas_msg.h"
#include "lte_ml1_irat_ext_api.h"


#include "msgr.h"
#include "msgr_geran.h"

#include "rr_l1.h"

#endif

#include "l1_sc.h"
#include "l1i.h"
#include "l1_utils.h"
#include "gl1_sys_params.h"
#include "msg.h"
#include "gl1_msg_wcdma_meas.h"
#include "l1_drx.h"
#include "l1_fm.h"

#include "log.h"            /* For log_commit() and log_alloc() and log codes */
#include "log_codes_gsm.h"  /* For WCDMA log codes */
#include "log_codes_gsm_v.h"
#include "l1_task.h"

#include "l1_isr.h"
#include "gpl1_gprs_isr.h"  /* For GPRS_NORMAL_PAGING enum */
#include "gpl1_gprs_log.h"
#include "gpl1_gprs_serving_cell.h"

#include "l1_log.h"
#include "g2wsrchdrv.h"
#include "g2wsrchwfwintf.h"

#include "gl1_hw_clk_ctl_g.h"
#include "mcpm_api.h"
#include "gl1_hw_sleep_ctl.h"
#include "gl1_hwi.h"

#ifdef FEATURE_GSM_TDS
#include "tdsiratif.h"
#endif

#ifdef FEATURE_GL1_GPLT
#include "gplt_result.h"
#endif /* FEATURE_GL1_GPLT*/
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains definitions for constants, macros, types, variables
  and other items needed by this module.

===========================================================================*/
/*extern uint8 l1_get_idle_monitors_in_progress(void);
extern void l1_increment_idle_monitors_in_progress(void);
extern void l1_decrement_idle_monitors_in_progress(void);*/
extern uint16 mdsp_ftsm (void);

/*boolean l1_sc_frames_available( l1_fm_priority_T pri, uint32 frames_required,
                                       void ( *abort_callback )( void* ), 
                                       void*   abort_data );

void  l1_sci_wcdma_aborted( void* dummy );*/
/************************** defines ***************************************/

#define MIN_LVL_dBm_x16              (-1024*16)  /* -1024 dBm (minimum sint15) */

#define MIN_EcNo_TO_BE_VALID_CELL    (-31*2)

#define MAX_EXPD_WCDMA_CELLS_SEARCH   2
#define MAX_PG_MFRMS                  9
#define MAX_ID_SEARCH_FRAMES         30

#define MAX_LTE_SEARCH_FRAMES         15

#define MAX_SEARCH_CYCLE_WCDMA_FREQS  (3)
#ifdef FEATURE_GSM_TDS

/*This value needs to be refined*/
#define MAX_TDS_SEARCH_FRAMES         15

/*This value is the number of FREQs that we will cap to
  in order to get required time between G2T ID searchs -
  in this case the fastest, which is ~30s*/
#define MAX_FREQ_FASTEST_ACQ_TIME     1

#endif

/* 
* idle frame gap to next idle frame for GSM activity 
*/
#define NUM_IDLE_FRAMES_PER_DED_SRCH      4   
#define NUM_IDLE_FRAMES_PER_DED_SRCH_G2L  (NUM_IDLE_FRAMES_PER_DED_SRCH-1)   

#define MAX_SAVED_MEAS               (MAX_WCDMA_CELLS_PER_UARFCN/2)
#define MIN_PWR_TO_BE_VALID_CELL     (-31*2)

/* This is the number of times the WCDMA ID search can be aborted before
 * getting abandoned.
 */
#define WCDMA_MAX_TRIES              3

#define RAT_IS_WCDMA     (1)
#define RAT_IS_LTE_FDD   (2)
#define RAT_IS_LTE_TDD   (3)

/* 100 is choosen so that this will never get executed, and can be enabled later on */
#define MAX_RSCP_FLUCTUATION 100

#define LOG_MAX_KNOWN_CELL_LIST_SIZE  MAX_WCDMA_CELLS_PER_FREQ

#define L1_ISR_FEMTO_TIMEOUT  300000  /* ms = 5 mins */
#define L1_ISR_FEMTO_RUNNING  20000   /* ms = 20 seconds */

#define GSM_L1_NUM_PRIORITY_SEARCH_THRESHOLDS  16


#ifdef FEATURE_GSM_TDS

/* return the slot before the lowest downlink slot in xfer*/
#define LOWEST_DL_SLOT(s) ((s > 1) ? (s - 1):(s))

#endif

/*
** currently 16 to include both L+W entries , may increase for TDS
*/
#define MAX_L1_PRIORITY_ENTRIES        MAX_RR_L1_PRIORITY_ENTRIES
#define G2L_5_GAPS_USED                (5)

/************************** defines ***************************************/



/************************** enums / structures ***************************************/

#ifdef FEATURE_GSM_TO_LTE
typedef lte_cphy_irat_meas_meas_results_s  gl1_lte_cell_meas_type;
#endif



/* -----------KNOWN CELL LIST---------------- */
typedef PACKED struct PACKED_POST
{
    uint16 UARFCN;
    uint16 scr_code;   /* Scrambling code (0..511) */
    uint16  pn_pos;    /* PN position */
    boolean diversity; /* cell diversity */
    boolean ignore;    /* on ignore list */
    boolean thresholds_valid; /* thresholding used */

    int8 sQual;
    int8 sRxLev;
    int8 RSCP;  /* Received signal strength (in dBm) */
    int8 EcNo;  /* Energy value returned from firmware */

} l1_log_wcdma_known_list_info_T;

LOG_RECORD_DEFINE(LOG_GSM_WCDMA_FILL_SAMPLE_RAM_C)   /*lint -esym(754, LOG_GSM_WCDMA_FILL_SAMPLE_RAM_C_tag::hdr) suppress 'LOG_GSM_WCDMA_FILL_SAMPLE_RAM_C_tag::hdr' not accessed*/
  uint32  frame_number;
  uint8   granted;
  uint16  UARFCN;
  uint8   num_cells;
  uint16  scr_code[MAX_WCDMA_CELLS_PER_FREQ];
LOG_RECORD_END

#define LOG_GSM_WCDMA_FILL_SAMPLE_RAM_C_LEN(n_cell)  (\
    FPOS(LOG_GSM_WCDMA_FILL_SAMPLE_RAM_C_type, scr_code) + \
      (n_cell*sizeof(uint16))   \
    )

LOG_RECORD_DEFINE(LOG_GSM_WCDMA_DED_CELL_LIST_C)   /*lint -esym(754, LOG_GSM_WCDMA_DED_CELL_LIST_C_tag::hdr) suppress 'LOG_GSM_WCDMA_DED_CELL_LIST_C_tag::hdr' not accessed*/
  uint8   qSearch_C;
  uint8   search_Prio;
  uint8   num_freqs;
  l1_log_wcdma_freq_info_T freq_info[MAX_WCDMA_FREQS];
LOG_RECORD_END

#define LOG_GSM_WCDMA_DED_CELL_LIST_C_LEN(n_freq, n_cell)  (\
    FPOS(LOG_GSM_WCDMA_DED_CELL_LIST_C_type, freq_info) + \
      (n_freq* (sizeof(uint16) + sizeof(uint8))) + \
      (n_cell* (sizeof(uint16) + sizeof(uint8)))   \
    )
LOG_RECORD_DEFINE(LOG_GSM_WCDMA_KNOWN_LIST_C) /*lint -esym(754, LOG_GSM_WCDMA_KNOWN_LIST_C_tag::hdr) suppress 'LOG_GSM_WCDMA_KNOWN_LIST_C_tag::hdr' not accessed*/
  uint8          Cell_Count;
  l1_log_wcdma_known_list_info_T Cell[LOG_MAX_KNOWN_CELL_LIST_SIZE];
LOG_RECORD_END


typedef struct
{
  uint32 frame_number;
  uint16 qs_offset;
} l1_gl1_time_s;


typedef struct
{
  l1_gl1_time_s start;
  l1_gl1_time_s end;
  l1_gl1_time_s clock;

} l1_rat_time_s;


#ifdef  FEATURE_GSM_TO_LTE

#define INVALID_FN               (0xFFFF)
#define G2L_GAP_START_OFFSET       (3750)
#define G2L_GAP_END_OFFSET          (325)    
#define G2L_GAP_CLOCK_OFFSET          (0)   /* not used by LTE  */

#define G2L_MDSP_START_OFFSET      (3125)   /* offset wrt FNmod26=24 idle-1 */
#define G2L_MDSP_END_OFFSET        (4500)   /* offset wrt FNmod26=25 idle   */

typedef enum {
   G2L_START_CMD_IDX,
   G2L_CLEANUP_CMD_IDX,
   G2L_END_CMD_IDX
} g2l_mdsp_cmd_idx;

void   l1_sc_send_lte_cell_search_results_to_rr( uint16 freq_index);

#endif

/************************** local function definitions ***************************************/
uint16   l1_sc_wcdma_get_ded_next_srch_freq( boolean inhibit_update );
void     update_stored_measurements_for_top_8( uint16 freq_idx);
boolean  l1_sc_frames_available( l1_fm_priority_T pri, uint32 frames_required,
                                       void ( *abort_callback ),
                                       void* abort_data );
void l1_sci_wcdma_aborted( void* dummy );                                     

boolean l1_sc_irat_state_machine_active(void);

/************************** external function definitions ***************************************/


/************************** global data ***************************************/
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)

extern t_stored_meas     stored_meas[MAX_WCDMA_FREQS];


/************************** function prototypes ***************************************/

void l1_sc_g2x_filter_treatment(t_gsm_l1_lte_wcdma_cell_energy_details**  cell_energy,
                                     int16 power, uint16 freq_idx, uint16 meas_idx, rr_l1_irat_rat_e rat_type);
#endif
extern void l1_sc_init_femto_timer (void);
extern void l1_sc_init_femto_timer_delete (void);

boolean l1_sc_is_reconf_next_pch_possible(void);
#endif
#endif /*L1_SC_IRAT_H*/
