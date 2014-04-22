#ifndef RFLM_CCS_MMU_INTF_H
#define RFLM_CCS_MMU_INTF_H


/*!
  @file
  rflm_ccs_mmu_intf.h

  @brief
  RFLM CCS MMU interface.

*/

/*===========================================================================

  Copyright (c) 2018 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

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
/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/rfmodem_jolokia.mpss/1.10/api/ccs/rflm_ccs_mmu_intf.h#1 $
$Date: 2020/04/16 $
when         who     what, where, why
--------   ---     -----------------------------------------------------------
05/25/18   mns     CCS MMU Changes.
03/24/16   sbo     Static Scripts changes.
02/04/15   aw      Initial version
==============================================================================*/


 
/*===========================================================================

                           INCLUDE FILES

===========================================================================*/ 
#include "rflm_ccs_task_defs.h"
#include "rflm_api_cmn.h"

/*=========================================================================== 
 
    					   CONSTANTS
 
===========================================================================*/
#define RFLM_CCS_USTMR_COUNT_BITS               22

#define RFLM_CCS_MMU_BITMAP_START_INDEX         0
#define RFLM_CCS_MMU_BITMAP_END_INDEX           (RFLM_CCS_MMU_BITMAP_SIZE -1)
#define RFLM_CCS_MMU_BITMAP_WORD_FETCH_CNT      3
#define RFLM_CCS_MMU_MAX_BLOCK_CNT_PER_ALLOC    64
#define RFLM_CCS_MMU_MAX_ALLOC_SIZE_IN_BYTES    (RFLM_CCS_MMU_MAX_BLOCK_CNT_PER_ALLOC*RFLM_CCS_MMU_BLOCK_SIZE_BYTES)
#define RFLM_CCS_MMU_MAX_BITMAP_SPAN            96

/*! @brief Start addr of Static Scripts*/
#define RFLM_CCS_MMU_STATIC_SCRIPTS_BASE_ADDR rflm_ccs_get_data_buf_item_ptr(RFLM_CCS_RF_MMU_STATIC_REGION)

/*! @brief Static scripts details*/
#define RFLM_CCS_MMU_STATIC_SCRIPTS_1H_SIZE_B     100
#define RFLM_CCS_MMU_STATIC_SCRIPTS_2H_SIZE_B     200
#define RFLM_CCS_MMU_STATIC_SCRIPTS_3H_SIZE_B     300
#define RFLM_CCS_MMU_STATIC_SCRIPTS_7H_50_SIZE_B  750


#define RFLM_CCS_MMU_STATIC_SCRIPTS_1H_NUM     2
#define RFLM_CCS_MMU_STATIC_SCRIPTS_2H_NUM     2
#define RFLM_CCS_MMU_STATIC_SCRIPTS_3H_NUM     2
#define RFLM_CCS_MMU_STATIC_SCRIPTS_7H_50_NUM  1


#define RFLM_CCS_MMU_STATIC_SCRIPTS_NUM  ( RFLM_CCS_MMU_STATIC_SCRIPTS_1H_NUM+ \
                                       RFLM_CCS_MMU_STATIC_SCRIPTS_2H_NUM+ \
                                       RFLM_CCS_MMU_STATIC_SCRIPTS_3H_NUM+ \
                                       RFLM_CCS_MMU_STATIC_SCRIPTS_7H_50_NUM)

/*===========================================================================

                   DEFINITIONS AND TYPES

===========================================================================*/
typedef enum
{
  RFLM_CCS_MMU_ALLOC_NO_ERR = 0x80,
  RFLM_CCS_MMU_ALLOC_ERR_OUT_OF_MEM,
  RFLM_CCS_MMU_EARLY_ENQ,
  RFLM_CCS_MMU_EARLY_ENQ_OUT_BUFFER,
  RFLM_CCS_MMU_INVALID_ENQ_API
}rflm_ccs_mmu_ret_code_t;

typedef struct rflm_ccs_mmu_alloc_s
{
  rflm_tech_id_t alloc_tech;      /* tech_id */ 
  uint8 alloc_block_cnt;          /* block cnt for the allocation */
  uint16 start_index;             /* start_index, starting index for the continuous blocks. */
  uint16 next_index;              /* next_index to point to next task of an event, auto-increased by task enqueue API.*/
  uint16 task_payload_size_bytes; /* size of each individual task in an event */
}rflm_ccs_mmu_alloc_t;

#define  RFLM_CCS_MMU_GET_TASK_BLOCK_CNT(task_size_in_bytes)                  \
         (task_size_in_bytes + RFLM_CCS_MMU_STATUS_WORDS_NUM*sizeof(uint32)  \
          + RFLM_CCS_MMU_BLOCK_SIZE_BYTES - 1)/RFLM_CCS_MMU_BLOCK_SIZE_BYTES

#ifndef TEST_FRAMEWORK
#define RFLM_CCS_MMU_GET_CCS_ADDR(next_index)                        \
        rflm_ccs_mmu_ccs_intf->rflm_ccs_rf_intf.data_buf + (next_index) * (RFLM_CCS_MMU_BLOCK_SIZE_BYTES/sizeof(uint32))

#else
#error code not present
#endif

#ifdef RFLM_CCS_MMU_BACKEND_SCHEDULE_ENABLE
// To use when enq a task
#define RFLM_CCS_MMU_ENQ_TIMING_CHECK(trig_ustmr,enq_ustmr,is_early_enq,is_past_enq,diff_ret)     \
  int32 diff_ustmr = trig_ustmr - enq_ustmr;                                                      \
  diff_ustmr = (diff_ustmr << (32-RFLM_CCS_USTMR_COUNT_BITS)) >> (32-RFLM_CCS_USTMR_COUNT_BITS);  \
  diff_ret = diff_ustmr;                                                                          \
  is_early_enq = (diff_ustmr > RFLM_CCS_MMU_EARLY_ENQ_CHK_THRESHOLD)? TRUE:FALSE;                 \
  is_past_enq  = (diff_ustmr < RFLM_CCS_MMU_EARLY_ENQ_CHK_THRESHOLD_PAST)? TRUE:FALSE

// To use by the backend ustmr thread. Diffferent threshold
#define RFLM_CCS_MMU_PEND_ENQ_CHECK(trig_ustmr,enq_ustmr,is_early_enq,diff_ret)                  \
  int32 diff_ustmr = trig_ustmr - enq_ustmr;                                                     \
  diff_ustmr = (diff_ustmr << (32-RFLM_CCS_USTMR_COUNT_BITS)) >> (32-RFLM_CCS_USTMR_COUNT_BITS); \
  diff_ret = diff_ustmr;                                                                         \
  is_early_enq = (diff_ustmr > RFLM_CCS_MMU_EARLY_ENQ_THRESHOLD)? TRUE:FALSE


#define RFLM_CCS_MMU_EARLY_ENQ_BUF_ALLOC(enq_buf_index,tq_idx,is_enq_buf_overflow)               \
  enq_buf_index  = atomic_inc_return((atomic_word_t*)&rflm_ccs_mmu_early_enq_tq_wr_idx[tq_idx]); \
  enq_buf_index --;                                                                              \
  enq_buf_index &= RFLM_CCS_MMU_EARLY_ENQ_DEPTH_MASK;                                            \
  uint32 buf_mask = BIT(enq_buf_index);                                                          \
  is_enq_buf_overflow = Q6_R_and_RR(rflm_ccs_mmu_early_enq_buffer_bitmap[tq_idx], buf_mask)

#endif

#ifdef __cplusplus
extern "C" {
#endif
/*===========================================================================

                    EXTERNAL INTERFACE FUNCTIONS

===========================================================================*/
extern void rflm_ccs_mmu_init_q6_bitmap(void);

extern rflm_ccs_mmu_ret_code_t rflm_ccs_mmu_allocate_mem
(
  rflm_ccs_mmu_alloc_t * mmu_alloc
);

extern uint32 rflm_ccs_get_task_payload_size
(
   ccs_rf_task_t *task_ptr
);

extern uint32 rflm_ccs_mmu_adv_bitmap_index
(
  uint32 * mmu_q6_bitmap_index,
  uint32 task_blk_cnt
);

extern uint32 rflm_ccs_mmu_early_enq_allocate_buf
(
   uint32 * mmu_enq_buffer_bitmap
);

extern void rflm_ccs_mmu_early_enq_buffer_handler(void);

extern void rflm_ccs_mmu_get_ccs_intf(void);

extern void rflm_ccs_mmu_vstmr_init(void);

extern void rflm_ccs_mmu_vstmr_cleanup(void);

extern void rflm_ccs_mmu_enable_mem_profiling(void);
/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/
void rflm_ccs_mmu_vstmr_register(void);
void rflm_ccs_mmu_sig_handler(void *arg);


uint8 rflm_ccs_mmu_allocate_static_region_block(uint32 mem_size_bytes,rflm_tech_id_t tech_id);

void rflm_ccs_mmu_deallocate_static_region_block(rflm_tech_id_t tech_id);

void rflm_ccs_mmu_deallocate_static_region_block_mask(rflm_tech_id_t tech_id, uint32 curr_allocation_mask);

uint32* rflm_ccs_mmu_get_static_mem_offset(uint8 block_indx);

uint32 rflm_ccs_mmu_get_static_mem_block_size(uint8 block_indx);

void rflm_ccs_mmu_write_memory_static_region(
	uint8 block_indx, 
	const void *src_ptr,           /*!< Source buffer, in DSP memory */
        uint32 num_bytes               /*!< Size of buffer to copy, in bytes */
);


#ifdef __cplusplus
}
#endif


#endif

