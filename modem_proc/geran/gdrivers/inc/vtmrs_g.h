#ifndef VTMRS_G_H
#define VTMRS_G_H
/*===========================================================================

                 G S M   T I M E R S  H E A D E R    F I L E

DESCRIPTION
  This module contains declarations and definitions to interface
  with the GSM timers module.

Copyright (c) 2001-2013, 2015 Qualcomm Technologies, Inc.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.


$Header: //components/rel/geran.mpss/5.2.0/gdrivers/inc/vtmrs_g.h#3 $ $DateTime: 2018/10/10 02:31:27 $ $Author: pwbldsvc $

when       who       what, where, why
--------   ---    -------------------------------------------------------- 
07/08/18   shm     CR 2274892 | Instantaneous WWAN timing and frequency information (rude wakeup)
03/01/18   ksb    CR2164555 Geran support to send Global Time Services Information TO GPS module
09/10/15   akh    CR912421 FN/QS not correct for active ttg, sleep ttg resp delayed by 1s
11/05/15   df     CR834120 Delay sleep if VSTMR frame extension in progress
04/04/14   cah    CR643764 VSTMR multi-sim restruct and FEATURE_MSIM_VSTMR removal
24/01/14   cah    CR598552: enable sleep for DSDS GSTMR+VSTMR
15/10/13   cah    created new header file for common vstmr geran api's         

===========================================================================*/
#ifdef FEATURE_GERAN_VSTMR
#include "geran_variation.h"
#ifndef CUSTOMER_H
  #include "customer.h"
#endif
#ifndef COMDEF_H
  #include "comdef.h"
#endif

#include "vstmr_geran.h"

#include "gtmrs.h"
#include "gtmrs_g.h"

#define GERAN_VSTMR_EVENT_INDEX_0 0 //periodic event
#define GERAN_VSTMR_EVENT_INDEX_1 1 //one shot event

#define G_STMR_QSYM_TO_RAW_13MHZ              12
#define G_STMR_RAW_13MHZ_TO_QSYM              5462
#define G_STMR_MAX_QS_FOR_REGISTER            4900


typedef enum
{
  GERAN_VFR_PRIMARY = 0,
  GERAN_VFR_SECONDARY = 1,
  GERAN_VFR_INVALID = 255
}vfr_allocation_t;

extern void vstmr_wake_up (void);
extern void vstmr_sleep (void);

extern void vstmr_schedule_event (   uint16 fn, boolean next );


extern void vstmr_schedule_event_and_set_fn (void);

#define GERAN_VSTMR_MAX_FN       (VSTMR_GERAN_RTC_MODULO/VSTMR_GERAN_FRAME_RANGE)
 
extern uint32 vstmr_rd_modMaxFN_frame_num(void);

extern uint32 get_vstmr_FN_modMaxFN(  );

extern void reset_vstmr_FN_modMaxFN (void);

extern void vstmr_adjust_terminal_count_geran (int16 quarter_symbol_count);

extern void reset_gstmr_FN_modMaxFN(void);

extern void vstmr_register_vstmr_view (void);

extern void    vstmr_enable_interrupt (gtmrs_hw_t gtmrs_hw);

extern int32 vstmr_get_tc_adjustment(void);

extern uint32 vstmr_rd_symbol_count(void);

extern uint32 vstmr_rd_qsymbol_count_geran(void);

extern uint32 vstmr_rd_odd_even(void);

extern uint32 vstmr_rd_modMaxFN_frame_num(void);

extern uint32 vstmr_rd_mod4_frame_num(void);

extern uint32 geran_rd_hw_frame_count_vstmr(void);

extern void gl1_ustmr_vstmr_snapshot_before_sleep(void);

extern void vstmr_deactivate(void);

extern void vstmr_geran_vstmr_deregister (void);

extern void vstmr_geran_vstmr_deregister_from_isr (void);

extern void vstmr_snapshot_before_vstmr_deregister (void);

extern void vstmr_x2g_wakeup_init( sys_modem_as_id_e_type as_id );

boolean vstmr_is_frame_extension_in_progress(void);

extern boolean vstmr_fnqs_rd_disabled_for_slamming(void);


extern int16 vstmr_get_drift( void);

extern void vstmr_reset_drift( void );

#endif /*FEATURE_GERAN_VSTMR*/

#endif /* VTMRS_G_H */

