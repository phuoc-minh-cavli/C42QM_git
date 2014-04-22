#ifndef L1_FM_H
#define L1_FM_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

      L A Y E R  1  F R A M E  M A N A G E R

GENERAL DESCRIPTION
   This module contains procedures to support the Neighbor cell activity
   scheduling in Layer 1. Together these functions make up the Frame manager.

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2001-2014 Qualcomm Technologies, Inc.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/geran.mpss/5.2.0/gl1/inc/l1_fm.h#4 $
$DateTime: 2019/01/01 22:24:35 $ $Author: pwbldsvc $

when       who       what, where, why
--------   --------  ---------------------------------------------------------
20/07/18   shm      CR2165044 FR 47234: AFC done at RFPLL if temperature above a certain threshold for spur channels
03/01/17   hd        CR2011646 Clear CCCH activity from fm table, if ext BCCH was scheduled and SI mask is cleared
17/02/17   nm        CR2005498 EDRX/PEO Gl1 incorrectly updating timing error to GSTMR even though SCH decode fails
11/01/17   nm        CR1087215 FR 36703: Rel.13 GERAN Power Efficient Operation (PEO)
08/06/16   akm       CR1025723 NBCCH is not prioritized over PCH for consecutive 6 attempts
08/03/16   km       CR986802 Resolving 64MB LWG IOE G compilation errors on MPSS.JO.2.0.1
21/01/16   km        CR957771 Avoid error recovery when G2W scripts are built during wakeup
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

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "geran_variation.h"
#ifndef CUSTOMER_H
  #include "customer.h"
#endif
#ifndef COMDEF_H
  #include "comdef.h"
#endif
#include "geran_multi_sim.h"


/*****************************************************

 LOCAL DEFINES

******************************************************/
#define QTA_OVERHEAD  11  /*Rounding  QTA overhead 48 ms to 11 FRAMES */
#define NOT_A_FRAME_NUMBER  0xFFFFFF
#define MAX_GAP             ((2048*51*26)-1)

/* Stores the number of frames required for the abort activity are completion of required frames by frame manager */
#define FRAMES_REQUIRED_FM_ABORT 2
#ifdef FEATURE_QTA
#define FM_MAX_EXTENDED_QTA_GAP 37 /* ~170ms to cover the longest PCH + BCCH + Ext BCCH + Ext PCH scenario */
#endif /* FEATURE_QTA */

typedef enum {
   FM_PRI_RESERVED = 0,
   FM_PRI_CCCH,

#ifdef FEATURE_GERAN_POWER_EFFICIENT_OPERATION
   FM_PRI_PEO_ACT,
#endif /*FEATURE_GERAN_POWER_EFFICIENT_OPERATION*/

   FM_PRI_BACKGROUND_PLMN,
   FM_PRI_NCELL_BCCH,
   FM_PRI_CBCH,
   FM_PRI_NCELL_ACQ,
   FM_PRI_NCELL_SCH,
   FM_PRI_WCDMA_IDSEARCH,

#ifdef  FEATURE_GSM_TO_LTE
   FM_PRI_LTE_IDSEARCH,
#endif

#ifdef FEATURE_GSM_TDS
   FM_PRI_TDS_ACQ, /*Low Chip Rate TD-SCDMA*/
#endif

#if defined ( FEATURE_DUAL_SIM ) || defined ( FEATURE_TRIPLE_SIM )
   FM_PRI_MSIM_CBCH,
#endif /* FEATURE_DUAL_SIM | FEATURE_TRIPLE_SIM */

   FM_PRI_MAX
} l1_fm_priority_T;

typedef enum {
   FM_STATE_NULL = 0,
   FM_STATE_USING_FRAMES,
   FM_STATE_CB_PENDING
} l1_fm_state_T;

typedef enum {
   EVE_FM_TICK,
   EVE_START_TASK,
   EVE_GET_AVAIL_FRAMES,
   EVE_ABORT_CURR_TASK,
   EVE_ABORT_LP_TASK,
   EVE_CLEAR_FRAMES,
   EVE_SET_REQD_FRAMES
} fm_event_T;

#if defined ( FEATURE_DUAL_SIM ) || defined ( FEATURE_TRIPLE_SIM )
typedef enum {
   FM_ACTION_NONE,
   FM_ACTION_CLEAR,
   FM_ACTION_CLEAR_ACTIVE_TASK,
   FM_ACTION_SET,
   FM_ACTION_DELAYED_RELEASE_FOR_TUNEAWAY,
   FM_ACTION_CLEAR_AND_SET,
   FM_ACTION_DELAYED_RELEASE_FOR_BPLMN
} action_event_T;
#endif /* FEATURE_DUAL_SIM | FEATURE_TRIPLE_SIM */

typedef struct {
   l1_fm_state_T state;
   uint32    required_frames;
   uint32    max_required_frames;
   uint32    reserved_frame_number;
   void (*abort_callback)(void*);
   void*     abort_data;
#if defined ( FEATURE_DUAL_SIM ) || defined ( FEATURE_TRIPLE_SIM )
   action_event_T action;
   uint16    fw_load_frames;
#endif /* FEATURE_DUAL_SIM | FEATURE_TRIPLE_SIM */
} l1_fm_table_element_T;

extern l1_fm_priority_T l1_fm_current_activity;
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
/*Add FN offset while calclating the gap to next activity after wakeup cancel*/
#define L1_FM_WMGR_CANCEL_CALC_GAP_TO_NEXT_ACTIVITY_FN_OFFSET   2
#endif
/*****************************************************

 LOCAL FUNCTION PROTOTYPES

******************************************************/
boolean l1_fm_any_activity_running(void);
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
extern uint32 l1_fm_wmgr_rebook_activity(void);
extern l1_fm_priority_T l1_get_fm_next_priority(uint32 cancelled_activity_fn );
extern uint32 l1_get_fm_next_priority_duration(l1_fm_priority_T pri);
#endif
#if defined ( FEATURE_DUAL_SIM ) || defined ( FEATURE_TRIPLE_SIM )
/*===========================================================================

FUNCTION l1_fm_find_next_task_to_reserve

DESCRIPTION
  This function looks through the frame manger table to find the next activity
  that will run of type
  FM_PRI_CCCH
  FM_PRI_NCELL_BCCH
  FM_PRI_CBCH
  FM_PRI_NCELL_SCH
  Then this will be booked into TRM

INPUTS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
boolean gl1_check_l1_state( byte state);
void l1_fm_find_next_task_to_reserve_or_release(void);
boolean l1_multi_sim_trm_manager(void);

extern void l1_fm_set_suspend_trm_release( uint32 frames,
                                           l1_fm_priority_T suspend_release_event_pri );
uint32 l1_fm_get_suspend_trm_release(void);
void l1_fm_clear_required_frames_after_pch_decode(l1_fm_priority_T pri);
boolean l1_fm_check_reserved_trm_activity(   uint32 frames_required );

void gl1_ms_fm_update_booking_frames(int32 frame_adjust);
l1_fm_priority_T l1_get_fm_trm_priority(void);

#ifdef FEATURE_GPRS_GBTA
boolean l1_fm_check_sch_activity_next(void);
#endif  /* FEATURE_GPRS_GBTA */


#endif /* FEATURE_DUAL_SIM | FEATURE_TRIPLE_SIM */

extern boolean l1_drx_wakeup_cancel_for_CCCH(uint32 reserved_frame_num);
extern void l1_drx_set_gap_to_CCCH_on_rude_wakeup(void);

void l1_fm_init(void);
void l1_fm_reset(void);
void l1_fm_tick(uint32 gap_to_next_ccch_activity);
void l1_fm_set_leaving_idle_mode( boolean leaving_idle_mode);

boolean l1_fm_prioritise_nbcch_over_ccch(void);

void l1_fm_set_required_frame_number(l1_fm_priority_T pri, uint32 frame_no, uint32 frames_reqd );
void l1_fm_clear_required_frames(l1_fm_priority_T pri);
void l1_fm_extend_required_frames(l1_fm_priority_T pri, uint32 frames_required );

boolean l1_fm_are_frames_available( l1_fm_priority_T pri,
                                    uint32 frames_required,
                                    void ( *abort_callback )(void*),
                                    void* abort_data);

uint32 l1_fm_get_available_frames(l1_fm_priority_T pri, uint32 current_frame_number );
uint32 l1_fm_get_ccch_gap(void);
void l1_fm_set_ccch_gap( uint32 gap_to_ccch_activity );


void l1_fm_log_event(l1_fm_priority_T pri , fm_event_T event);
void l1_clear_cbch_activity(void);
l1_fm_priority_T l1_fm_get_running_activity(void);

l1_fm_priority_T l1_fm_get_active_CBCH_pri(void);
extern void l1_clear_inactive_cbch_entires(void);

uint32 l1_fm_get_required_frames_to_complete(l1_fm_priority_T pri);
#ifdef FEATURE_GSM_COEX_SW_CXM
l1_fm_priority_T l1_fm_get_next_activity(uint32* next_activity_fn );
#endif
l1_fm_priority_T l1_fm_get_running_activity(void);

l1_fm_state_T l1_fm_get_fm_table_state(    l1_fm_priority_T pri);
boolean l1_fm_is_fn_ccch(uint32 frame_fn );

boolean l1_fm_is_ccch_activity_running(void);

#endif /* L1_FM_H */

