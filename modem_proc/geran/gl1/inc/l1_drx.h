#ifndef L1_DRX_H
#define L1_DRX_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

      L A Y E R  1  D R X

GENERAL DESCRIPTION
   This module contains procedures to support DRX in Layer 1.
   Together these functions make up the DRX manager.

Copyright (c) 2001-2014 Qualcomm Technologies, Inc.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/geran.mpss/5.2.0/gl1/inc/l1_drx.h#5 $
$DateTime: 2019/05/01 01:40:11 $ $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ----------------------------------------------------------
29/06/16   hd       CR1035604 Declare gl1_clear_drx_tx_status
01/10/18   ng       CR2325381 Move all GPS code under feature flag 'FEATURE_CGPS_TIME_TAGGING'
14/05/18   km       CR748521 Remove 5 frame delay to sleep when entering DRX mode
26/12/16   ksb      CR1087656 Enabling sleep in Transfer mode for PEO feature
12/12/16   hd       CR1087185 - FR 36130: Rel.13 idle mode eDRX
28/10/14   cgc      CR745548 GL1 make gl1_drx_require_and_wait_for_next_tick() conditional on awake
06/08/14   df       CR536059 Allow Asynchronous sleep for all idle activities
10/03/14   am       CR624678 TX-ON should happen in full frame
25/02/14   am       CR615566 Activities should not clear their DRX entries based on reference
11/12/13   ap       CR586661 GPRS suspension during SI read after W2G reselection
05/12/13   cs       CR584551 Use correct gas_id for GPS Timetagging in SS mode
05/11/13   cja      CR571501 In DRX on do not turn Tx off if leaving idle in the same frame
19/07/13   gk       CR515617:Don't delay the interrupt if W has the lock and its a rude wakeup
23/10/12   npt      CR413635 - Port sleep code for DIME modem
17/08/12   ip       CR384421 Disable GSM sleep during CCO activity
22/06/12   ip       CR362050 - Set tx state to DRX_TX_ON_ALWAYS when comming from NULL
28/03/12   cja      CR316437 Inhibit sleep on RAT change until it is complete
08/03/12   ip       CR326699 - Code changed for wake up recovery mechanism
31/08/11   cja      CR304401. Wait for frame tick so FW not deactivated when suspended.
20/06/11   cs       DSDS CR287999 Allow sleep to start upon completion of reserve_at
14/06/11   npt      CR287491 Move post tick processing for asynch into DPC to
                    allow for mdsp sleep cmd isr to be processed
28/04/11   pg       Fix compiler warnings
04/04/11   nf       CR280679 added gl1_drx_get_next_tick_req
14/02/11   cs       Slight redesign of asynch int to allow for a silent frame
                    interrupt for the frame after a failed asynch attempt
09/12/10   cs       CR264682 Add support for asynch pch
23/11/09   ip       Call DAL_Device_attach from task context
02/03/07   cs       Add support for new CGPS timetag
03/29/06   gfr      Added gl1_drx_first_awake_frame
04/12/05   gfr      Added sys_stru.h to get definition of IMH_T
04/11/05   gfr      Added prototype for gl1_drx_process_timetag_request
02/25/03   gw       Added prototype for function gl1_drx_require_TX().
02/12/03   gw       Changed prototype for gl1drx_report_te to support new
                    FEE timing error functionality.
01/02/03   gw       Added argument to gl1_drx_post_tick().
10/28/02   gw       Added prototype for gl1_drx_post_tick().
06/10/02   gw       Removed parameter from gl1_drx_tick().  Added prototype
                    for gl1_drx_report_te.
10/10/01   gw       Initial version.
===========================================================================*/

#include "geran_variation.h"
#include "comdef.h"    /* for types */
#include "sys_stru.h"
#include "geran_multi_sim.h"


typedef enum {
   DRX_NULL=0,
   DRX_AWAKE,
   DRX_GOING_TO_SLEEP,
   DRX_ASLEEP,
   DRX_WARMING_UP,
   DRX_RUDE_WAKEUP,
   DRX_WAKEUP_WHILE_GOING_TO_SLEEP,
   #ifdef FEATURE_GERAN_POWER_EFFICIENT_OPERATION
   DRX_TRANSFER, 
#endif 
} l1_drx_state_id;


void gl1_drx_set_drx_state(l1_drx_state_id state);

#ifdef FEATURE_GERAN_POWER_EFFICIENT_OPERATION  
l1_drx_state_id gl1_drx_get_drx_state (void);
boolean gl1_drx_go_to_sleep_in_ptm(uint32 wakeup_fn1);
#endif 

void gl1_drx_reset(void );

void gl1_drx_on( boolean keep_tx_on);

void gl1_drx_off(boolean  turn_tx_on );

void gl1_set_tx_on_always (void);

boolean gl1_drx_tick(void);

void gl1_drx_post_tick( boolean in_task_context );

void gl1_drx_task_active(void);

void gl1_drx_task_inactive(void);

void gl1_drx_require_next_tick(void);

void gl1_drx_require_and_wait_for_next_tick(void);

void gl1_drx_require_and_wait_for_next_tick_conditional(void);

uint8 gl1_drx_require_tick( uint32 frame_number );

void gl1_drx_release_tick( uint8 reference);

void gl1_drx_release_fn_tick( uint32 frame_number );

void gl1_drx_report_te( int16 te[], uint16 snr[], uint16 num );

void gl1_drx_require_TX( uint32 num_frames );

boolean gl1_drx_first_awake_frame(void);

void gl1_drx_set_frame_number_adjustment ( int32 num );

boolean gl1_is_drx_on(void);

uint32 gl1_drx_get_max_sleep_duration(void);

#if defined ( FEATURE_DUAL_SIM ) || defined ( FEATURE_TRIPLE_SIM )
void gl1_drx_rf_reinitialised(void);

boolean gl1_drx_cm_starting(void);
void    gl1_drx_cm_shutdown(void);
boolean l1_is_delaying_interrupt_needed(void);
#endif /* FEATURE_DUAL_SIM | FEATURE_TRIPLE_SIM */

#ifdef FEATURE_CGPS_TIME_TAGGING
void gl1_drx_process_timetag_request (IMH_T *msg_header);
#endif
void gl1_clear_drx_tx_status(void);
boolean gl1_sleep_active (void);
extern boolean gl1_drx_is_gsm_asleep(void);

extern void gl1_drx_set_pch_offset( int16 asynch_pch_offset );
extern void gl1_drx_asynch_post_tick( void );
extern void gl1_drx_task_sleep( boolean called_from_asynch_dpc );
extern void gl1_drx_update_counters( int32 bump, boolean do_gstmr );
extern int16 gl1_drx_get_pch_offset( void );

extern boolean gl1_drx_asynch_int_sleep_possible_precheck(void);

extern void gl1_drx_set_asynch_int_ignore_gstmr( boolean asynch_int_ignore_next_gstmr );
extern boolean gl1_drx_get_asynch_int_ignore_gstmr(void);

extern boolean gl1_drx_get_next_tick_req (void);

extern void gl1_drx_set_sleep_inhibit (boolean sleep_inhibit);
extern boolean gl1_drx_get_sleep_inhibit(void);

extern void gl1_drx_set_sleep_inhibit_for_cco_req (boolean sleep_cco_inhibit);

void gl1_drx_set_sleep_inhibit_for_trm(boolean sleep_inhibit);
boolean gl1_drx_get_sleep_inhibit_for_trm(void);

extern void    gl1_drx_post_tick_dpc(void);
#endif /* L1_DRX_H */
