/*!
  @file
  lte_dsm_util_ext_api.h

  @brief
  REQUIRED brief one-sentence description of this C header file.

  @detail
  OPTIONAL detailed description of this C header file.
  - DELETE this section if unused.

*/

/*==============================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/lte.mpss/4.7.2/api/lte_dsm_util_ext_api.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
10/20/14   sb      CR506328: Remove unused code and file
01/04/11   ax      Separate out what is needed by FTM to API dir
==============================================================================*/

#ifndef LTE_DSM_UTIL_EXT_API_H
#define LTE_DSM_UTIL_EXT_API_H

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include <comdef.h>
#include <dsm_item.h>
#include <dsm_queue.h>

/*=============================================================================

                     EXTERNAL TYPE DEFINATION

=============================================================================*/
/*!
    @brief
    the type for recycle consumer pool
*/
typedef void * lte_dsm_util_recycle_consumer_pool_s;

/*!
    @brief
    the type for recycle producer pool
*/
typedef void * lte_dsm_util_recycle_producer_pool_s;

/*=============================================================================

                     EXTERNAL GLOBAL VARIABLE

=============================================================================*/
/*!
    @brief
    the RLCDL producer pool(used in IPA contet)
    it produces both dup and sl dsm item
*/
extern lte_dsm_util_recycle_producer_pool_s                                    \
  lte_dsm_util_recycle_lte_rlcdl_prodcuer_pool;

/*!
    @brief
    the MACDL CONSUMER pool(used in MACL context)
    it consumes dup item
*/ 
extern lte_dsm_util_recycle_consumer_pool_s                                    \
  lte_dsm_util_recycle_dup_lte_macdl_consumer_pool;

/*!
    @brief
    the A2 CONSUMER pool(used in A2 context)
    it consumes sl item
*/ 
extern lte_dsm_util_recycle_consumer_pool_s                                    \
  lte_dsm_util_recycle_sl_lte_a2_consumer_pool;

/*=============================================================================

                     EXTERNAL MACRO

=============================================================================*/
/*!
    @brief
    The DSM item that recycled between A2 and RLCDL
*/
#define LTE_DSM_UTIL_RECYCLE_LTE_A2_ITEM DSM_DS_SL_ITEM_POOL

/*!
    @brief
    The DSM item that recycled between MACDL and RLCDL
*/
#define LTE_DSM_UTIL_RECYCLE_LTE_MACDL_ITEM DSM_DUP_ITEM_POOL

/*!
    @brief
    API called macdl to init the macdl consumer pool
*/
#define lte_dsm_util_recycle_dup_macdl_init()                                  \
  lte_dsm_util_recyclei_consumer_init(                                         \
    lte_dsm_util_recycle_lte_rlcdl_prodcuer_pool,                              \
      lte_dsm_util_recycle_dup_lte_macdl_consumer_pool,                        \
        LTE_DSM_UTIL_RECYCLE_LTE_MACDL_ITEM,                                   \
          dsm_file,__LINE__)

/*!
    @brief
    API called macdl to deinit the macdl consumer pool
*/
#define lte_dsm_util_recycle_dup_macdl_deinit()                                \
  lte_dsm_util_recyclei_consumer_flush_pool(                                   \
    lte_dsm_util_recycle_dup_lte_macdl_consumer_pool,                          \
      LTE_DSM_UTIL_RECYCLE_LTE_MACDL_ITEM)

/*!
    @brief
    API called macdl to flush the macdl consumer pool back to dsm lib
*/
#define lte_dsm_uitl_recycle_dup_macdl_flush_pool()                            \
  lte_dsm_util_recyclei_consumer_flush_pool(                                   \
    lte_dsm_util_recycle_dup_lte_macdl_consumer_pool,                          \
      LTE_DSM_UTIL_RECYCLE_LTE_MACDL_ITEM)

/*!
    @brief
    API called A2 to init the A2 consumer pool
*/
#define lte_dsm_util_recycle_sl_a2_init()                                      \
  lte_dsm_util_recyclei_consumer_init(                                         \
    lte_dsm_util_recycle_lte_rlcdl_prodcuer_pool,                              \
      lte_dsm_util_recycle_sl_lte_a2_consumer_pool,                            \
         LTE_DSM_UTIL_RECYCLE_LTE_A2_ITEM,                                     \
           dsm_file,__LINE__)

/*!
    @brief
    API called A2 to deinit the A2 consumer pool
*/
#define lte_dsm_util_recycle_sl_a2_deinit()                                    \
  lte_dsm_util_recyclei_consumer_flush_pool(                                   \
    lte_dsm_util_recycle_sl_lte_a2_consumer_pool,                              \
      LTE_DSM_UTIL_RECYCLE_LTE_A2_ITEM)

/*!
    @brief
    API called A2 to flush the A2 consumer pool back to dsm lib
*/
#define lte_dsm_uitl_recycle_sl_a2_flush_pool()                                \
  lte_dsm_util_recyclei_consumer_flush_pool(                                   \
    lte_dsm_util_recycle_sl_lte_a2_consumer_pool,                              \
      LTE_DSM_UTIL_RECYCLE_LTE_A2_ITEM)

/*!
    @brief
    API called RLCDL to init the RLCDL producer pool
*/
#define lte_dsm_util_recycle_dup_sl_rlcdl_init()                               \
  lte_dsm_util_recyclei_producer_init(                                         \
    lte_dsm_util_recycle_lte_rlcdl_prodcuer_pool)

/*!
    @brief
    API called RLCDL to deinit the RLCDL producer pool
*/
#define lte_dsm_util_recycle_dup_sl_rlcdl_deinit()                             \
  lte_dsm_util_recyclei_producer_flush_pool(                                   \
    lte_dsm_util_recycle_lte_rlcdl_prodcuer_pool)

/*!
    @brief
    API called RLCDL to flush the RLCDL producer pool back to dsm lib
*/
#define lte_dsm_util_recycle_dup_sl_rlcdl_flush_pool()                         \
  lte_dsm_util_recyclei_producer_flush_pool(                                   \
    lte_dsm_util_recycle_lte_rlcdl_prodcuer_pool)

/*!
    @brief
    API to disable the DSM recycling
*/
#define lte_dsm_util_recycle_dup_sl_rlcdl_disable()                            \
  lte_dsm_util_recyclei_disable(                                               \
    lte_dsm_util_recycle_lte_rlcdl_prodcuer_pool)

/*!
    @brief
    API to enable the DSM recycling
*/
#define lte_dsm_util_recycle_dup_sl_rlcdl_enable()                             \
  lte_dsm_util_recyclei_enable(                                                \
    lte_dsm_util_recycle_lte_rlcdl_prodcuer_pool)

/*!
    @brief
    API to free a DSM item to recycle pool

    @parameter
    dsm_ptr_ptr: the pointer pointing to the pointer of the dsm need be freed
*/
#define lte_dsm_util_recycle_free_packet(dsm_ptr_ptr)                          \
  lte_dsm_util_recyclei_free_packet(dsm_ptr_ptr,                               \
    lte_dsm_util_recycle_lte_rlcdl_prodcuer_pool,                              \
      dsm_file, __LINE__)

/*!
    @brief
    API to dup a dsm by using a dup item from recycle pool

    @parameter
    dsm_ptr_ptr: the pointer pointing to the pointer of the target dsm
    src_ptr: the piinter pointing to the dsm need to be dupped
    offset: the offset of the first byte in src dsm ptr where the dup begins
    cnt: number of bytes need to be dupped

    @return:
    number of bytes dupped
*/
#define lte_dsm_util_recycle_dup_packet(dsm_ptr_ptr, src_ptr, offset, cnt)     \
  lte_dsm_util_recyclei_dup_packet(dsm_ptr_ptr,                                \
    src_ptr,offset,cnt,                                                        \
      lte_dsm_util_recycle_dup_lte_macdl_consumer_pool)

/*!
    @brief
    API to alloc number of SL DSM items from recycle pool

    @parameter
    dsm_array: the array stores the allocated DSM item
    wr_idx_ptr: the write idx in the array where the new dsms are written 
                into
    rd_idx: the read idx in the array where the user of the array will fetch 
            the new dsm item

    @return:
    true: success
    false: failed
*/
#define lte_dsm_util_recycle_sl_new_buffers(dsm_array, wr_idx_ptr, rd_idx)     \
  lte_dsm_util_recyclei_sl_new_buffers(dsm_array,                              \
    wr_idx_ptr,rd_idx ,                                                        \
        lte_dsm_util_recycle_sl_lte_a2_consumer_pool)

/*=============================================================================

                     EXTERNAL FUNCTIONS

=============================================================================*/
/*!
    @brief
    API to init a producer pool. The function will be wrapped by a MACRO.
    Only MACRO is used by clients
*/
extern void lte_dsm_util_recyclei_producer_init
(
  lte_dsm_util_recycle_producer_pool_s producer_pool
);

/*!
    @brief
    API to init a consumer pool. The function will be wrapped by a MACRO.
    Only MACRO is used by clients
*/
extern void lte_dsm_util_recyclei_consumer_init
(
  lte_dsm_util_recycle_producer_pool_s  producer_pool,
  lte_dsm_util_recycle_consumer_pool_s  consumer_pool,
  dsm_mempool_id_type                   recycle_pood_id,
  const char *                          file,
  uint32                                line
);

/*!
    @brief
    API to flush all dsm items from a producer pool to DSM lib. 
    The function will be wrapped by a MACRO.
    Only MACRO is used by clients
*/
extern void lte_dsm_util_recyclei_producer_flush_pool
(
  lte_dsm_util_recycle_producer_pool_s  producer_pool
);

/*!
    @brief
    API to flush all dsm items from a consumer pool to DSM lib. 
    The function will be wrapped by a MACRO.
    Only MACRO is used by clients
*/
extern void lte_dsm_util_recyclei_consumer_flush_pool
(
  lte_dsm_util_recycle_consumer_pool_s  consumer_pool,
  dsm_mempool_id_type                   recycle_pood_id
);

/*!
    @brief
    API to dup a DSM item. The function will be wrapped by a MACRO.
    Only MACRO is used by clients
*/
extern uint32 lte_dsm_util_recyclei_dup_packet
(
  dsm_item_type **                      dup_ptr_ptr,
  dsm_item_type *                       src_ptr,
  uint32                                offset,
  uint32                                cnt,
  lte_dsm_util_recycle_consumer_pool_s  consumer_pool
);

/*!
    @brief
    API to init a producer pool. The function will be wrapped by a MACRO.
    Only MACRO is used by clients
*/
extern boolean lte_dsm_util_recyclei_sl_new_buffers
(
  dsm_item_type **                      dsm_array,
  uint8 *                               wr_idx_ptr,
  uint8                                 rd_idx,
  lte_dsm_util_recycle_consumer_pool_s  consumer_pool
);

/*!
    @brief
    API to trigger qsh analysis. The function will be wrapped by a MACRO.
    Only MACRO is used by clients
*/
extern boolean lte_dsm_util_recycle_qsh_handler
(
  uint32 cmd
);

/*!
    @brief
    API to enable the dsm recycle. The function will be wrapped by a MACRO.
    Only MACRO is used by clients
*/
extern void lte_dsm_util_recyclei_enable
(
  lte_dsm_util_recycle_producer_pool_s producer_pool
);

/*!
    @brief
    API to disable the dsm recycle. The function will be wrapped by a MACRO.
    Only MACRO is used by clients
*/
extern void lte_dsm_util_recyclei_disable
(
  lte_dsm_util_recycle_producer_pool_s producer_pool
);

/*!
    @brief
    API to free dsm to recyclepool. The function will be wrapped by a MACRO.
    Only MACRO is used by clients
*/
extern void lte_dsm_util_recyclei_free_packet
(
  dsm_item_type **                     dsm_ptr_ptr,
  lte_dsm_util_recycle_producer_pool_s producer_pool,
  const char *                         file,
  uint32                               line
);

#endif /* LTE_DSM_UTIL_EXT_API_H */
