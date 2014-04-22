/*!
  @file
  vstmr_nb1.h

  @brief
  VSTMR NB1 header file
 
*/

/*===========================================================================

  Copyright (c) 2016 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

$Header: //components/rel/mcs.mpss/5.1/api/vstmr_nb1.h#1 $ 

===========================================================================*/

#ifndef VSTMR_NB1_H
#define VSTMR_NB1_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <comdef.h>
#include "vstmr.h"
 

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief Nominal FCW value in Q30.
    NB1:  1.92 MHz / 19.2e6 = 0.1 * 2^30 = 0x6666666 (rounded)
*/
#define VSTMR_NB1_FCW                   (0x6666666)

/*! @brief Wraparound for NB1 RTC.
    16 hyper frame x 1024 radio frames x 10 ms at 1.92 MHz. 
    (VSTMR_NB1_HFR_CNT_RANGE x VSTMR_NB1_SFR_CNT_RANGE x VSTMR_NB1_SUBFR_CNT_RANGE x VSTMR_NB1_SUBFRAME_RANGE).
*/
#define VSTMR_NB1_RTC_MODULO            (0x12C00000)

/*! @brief Radio frame range for NB1 STMR (=10msec @ 1.92 MHz).
*/
#define VSTMR_NB1_SLOT_RANGE            (960)
#define VSTMR_NB1_SUBFRAME_RANGE        (1920)
#define VSTMR_NB1_RADIOFRAME_RANGE      (19200)

/*! @brief NB1 frame count range 
*/
#define VSTMR_NB1_SLOT_CNT_RANGE        (2)
#define VSTMR_NB1_SUBFR_CNT_RANGE       (10)
#define VSTMR_NB1_SFR_CNT_RANGE         (1024)
#define VSTMR_NB1_HFR_CNT_RANGE         (16)

#define VSTMR_NB1_HSFR_CNT_RANGE        (VSTMR_NB1_HFR_CNT_RANGE  * VSTMR_NB1_SFR_CNT_RANGE)

#define VSTMR_NB1_SSUBFR_CNT_RANGE      (VSTMR_NB1_SFR_CNT_RANGE  * VSTMR_NB1_SUBFR_CNT_RANGE)
#define VSTMR_NB1_HSUBFR_CNT_RANGE      (VSTMR_NB1_HSFR_CNT_RANGE * VSTMR_NB1_SUBFR_CNT_RANGE)

#define VSTMR_NB1_SUBFR_SLOT_CNT_RANGE  (VSTMR_NB1_SUBFR_CNT_RANGE * VSTMR_NB1_SLOT_CNT_RANGE)
#define VSTMR_NB1_SFR_SLOT_CNT_RANGE    (VSTMR_NB1_SFR_CNT_RANGE   * VSTMR_NB1_SUBFR_SLOT_CNT_RANGE)
#define VSTMR_NB1_HFR_SLOT_CNT_RANGE    (VSTMR_NB1_HFR_CNT_RANGE   * VSTMR_NB1_SFR_SLOT_CNT_RANGE)

/*! @brief Get global hyper subframe and global system sub frame numbner from NB1 STMR 
*/
#define VSTMR_NB1_G_H_FN(stmr)          ((stmr).fullFrNum)
#define VSTMR_NB1_G_H_SFN(stmr)         (((stmr).fullFrNum * 10) + (stmr).subFrNum)
#define VSTMR_NB1_G_S_FN(stmr)          ((stmr).sFrNum)
#define VSTMR_NB1_G_S_SFN(stmr)         (((stmr).sFrNum * 10) + (stmr).subFrNum)

/*! @brief Enum to identify NB1 views 
*/
typedef enum  
{
  VSTMR_NB1_REF_COUNT_VIEW, /*!< Unmodified RTC reference count */
  VSTMR_NB1_DL_COUNT_VIEW,  /*!< For DL cell timing */
  VSTMR_NB1_UL_COUNT_VIEW,  /*!< For UL timing */
  VSTMR_NB1_ML1_COUNT_VIEW, /*!< For ML1 timing */
  VSTMR_NB1_NUM_VIEWS
} vstmr_nb1_view_e;


/*! @brief Structure to identify NB1 segmentation format.
*/
typedef union {

  /* Types for internal manipulation */
  uint64 d;
  uint32 w[2];

  uint64 all;     /*!< For clearing entire dword */

  struct {
    uint32 frac;                /*!< Fractional bits */ 

    union {
      struct {
        uint32 subFrCnt    : 11;  /*!< Bits within a radio frame, range [0..VSTMR_NB1_SUBFRAME_RANGE-1] */ 
        uint32 subFrNum    : 4;   /*!< Sub frame with in radio frame, range [0..VSTMR_NB1_SUBFR_CNT_RANGE-1]  */ 
        uint32             : 1;   /*!< Unused */
        uint32 sFrNum      : 10;  /*!< System frame number in radio frame unit, range [0..VSTMR_NB1_SFR_CNT_RANGE-1] */ 
        uint32 hFrNum      : 4;   /*!< Hyper frame numberframe number in radio frame unit, range [0..VSTMR_NB1_HFR_CNT_RANGE-1] */ 
        uint32             : 2;   /*!< Unused */
      };
      struct {
        uint32             : 16;
        uint32 fullFrNum   : 14;  /*!< Hyper frame in radio frame unit, range [0..VSTMR_NB1_HSFR_CNT_RANGE-1] */ 
        uint32             : 2;
      };
    };
  };
} vstmr_nb1_stmr_t;


/*! @brief NB1 view management structure.
*/
typedef struct 
{
  struct vstmr_rtc_s  *rtc;       /*!< Pointer to the RTC struct */ 
  uint32               offset;    /*!< Offset for this view in NB1 RTC units, 
                                       Note: Its a uint32, so no fractional part. */
} vstmr_nb1_view_s;


/*! @brief NB1 event table management structure.
*/
typedef struct vstmr_nb1_event_tbl_s_
{
  vstmr_event_tbl_s   event_tbl_data;  /*!< Internal event table data, stores 
                                            event expiration counts in XO resolution. */

  /* NB1 specific fields */
  vstmr_nb1_view_s*   view;            /*!< Pointer to the associated view. 
                                            View is the source of time for this event table */ 

  uint32              events_seg_cnt[VSTMR_MAX_EVENTS];  
  /*!< Array to store event's expiration time in NB1 resolution at 1.92 MHz. 
       There are in all VSTMR_MAX_EVENTS events for an event table.
       If a value is 0, that just means that event is not in use. */
   
  void (*isr_callback)(struct vstmr_nb1_event_tbl_s_ *event_tbl, void *arg, uint32 pending);
  /*!< NB1 callback routine which NB1 VSTMR layer calls once an event expires */

  void *isr_arg;
  /*!< Argument for the callback routine */

  /*!< When the comparator fires, ISR calls the registered VSTMR callback function. 
       VSTMR function services the event table, and then calls registered NB1 callback.
       Servicing the event table involves parsing the event table to find out what
       all events have expired and then reprogramming the comparator for the
       next event. */

  atomic_word_t         service_sync_flag;     /*!< Refer to file vstmr_wcdma.h to 
                                                    check the usage of this flag */

  atomic_word_t         isr_sync_flag;         /*!< Refer to file vstmr_wcdma.h to 
                                                    check the usage of this flag */

} vstmr_nb1_event_tbl_s;



#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

vstmr_nb1_stmr_t vstmr_nb1_segment_cnt
(
  uint64 rtc              
);

vstmr_nb1_view_s * vstmr_nb1_view_init
( 
  vstmr_rtc_id         rtc_id,
  vstmr_nb1_view_e     view_id,
  vstmr_nb1_stmr_t     offset
);

vstmr_nb1_view_s * vstmr_nb1_get_view_handle
(
  vstmr_rtc_id       rtc_id,
  vstmr_nb1_view_e   view_id
);

vstmr_nb1_stmr_t vstmr_nb1_get_current_stmr
(
  vstmr_nb1_view_s  *view
);

vstmr_nb1_stmr_t vstmr_nb1_xo_to_stmr
(
  vstmr_nb1_view_s  *view,
  uint32             xo_cnt  
);

uint32 vstmr_nb1_stmr_to_xo
(
  vstmr_nb1_view_s  *view,
  vstmr_nb1_stmr_t   stmr
);

void vstmr_nb1_stmr_sync
(
  vstmr_nb1_view_s  *view,        
  vstmr_nb1_stmr_t   stmr,    
  uint32             xo_cnt    
);

void vstmr_nb1_view_set_offset
(
  vstmr_nb1_view_s  *view,
  vstmr_nb1_stmr_t   offset
);

void vstmr_nb1_view_adjust_offset
(
  vstmr_nb1_view_s  *view,
  int32              adjust
);

void vstmr_nb1_update_fcw
(
  vstmr_nb1_view_s  *view,
  uint32             fcw
);

void vstmr_nb1_event_tbl_register
(
  vstmr_nb1_event_tbl_s   *event_tbl,
  vstmr_nb1_view_s        *view,
  vstmr_int_types_e        int_type
);

void vstmr_nb1_event_tbl_deregister
(
  vstmr_nb1_event_tbl_s   *event_tbl
);

void vstmr_nb1_update_event_tbl_view
(
  vstmr_nb1_event_tbl_s   *event_tbl,    
  vstmr_nb1_view_s        *view          
);

extern uint32 vstmr_nb1_event_service
(
  vstmr_nb1_event_tbl_s   *event_tbl
);

void vstmr_nb1_interrupt_register
(
  vstmr_nb1_event_tbl_s   *event_tbl,
  void (*cb)(vstmr_nb1_event_tbl_s *event_tbl, void *arg, uint32 pending),
  void *cb_arg
);

void vstmr_nb1_interrupt_deregister
(
  vstmr_nb1_event_tbl_s   *event_tbl
);

void vstmr_nb1_event_sched_full
(
  vstmr_nb1_event_tbl_s   *id,
  uint32                   event_idx,
  vstmr_nb1_stmr_t         stmr  
);

void vstmr_nb1_event_sched
(
  vstmr_nb1_event_tbl_s  *id,
  uint32                  event_idx,
   uint32                 sub_rf_offset
);

void vstmr_nb1_event_sched_oneshot
(
  vstmr_nb1_event_tbl_s  *id,
  uint32                  event_idx,
  uint32                  sub_rf_offset
);

void vstmr_nb1_event_cancel
(
  vstmr_nb1_event_tbl_s  *id,
  uint32                  event_idx
);

void vstmr_nb1_event_cancel_mask
(
  vstmr_nb1_event_tbl_s  *id,
  uint32                  event_idx_mask   
);


/*===========================================================================

  FUNCTION:  vstmr_nb1_seg_to_full_cnt

===========================================================================*/
/*!
    @brief
    Convert a segmented stmr value to linear full count @ 1.92 MHz.
    The fractional portion is discarded.

    @return
    Linear full count in the range [0..VSTMR_NB1_RTC_MODULO-1].
*/
/*=========================================================================*/
INLINE uint32 vstmr_nb1_seg_to_full_cnt
(
  vstmr_nb1_stmr_t  stmr   /*!< Segmented stmr count to convert */
)
{
  return ((stmr.fullFrNum * VSTMR_NB1_RADIOFRAME_RANGE) + (stmr.subFrNum * VSTMR_NB1_SUBFRAME_RANGE) + stmr.subFrCnt);
  
} /* vstmr_nb1_seg_to_full_cnt() */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /*VSTMR_NB1_H*/
