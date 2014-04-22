#ifndef RFLM_CCS_TASK_DEFS_H
#define RFLM_CCS_TASK_DEFS_H

/*!
  @file 
  rflm_ccs_task_defs.h

  @brief
  RF CCS task definitions
 
*/

/*============================================================================== 
   
  Copyright (c) 2019  Qualcomm Technologies Incorporated. All Rights Reserved 
   
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
 
$Header: //components/rel/rfmodem_jolokia.mpss/1.10/api/ccs/rflm_ccs_task_defs.h#1 $ 
$DateTime: 2020/04/16 12:27:55 $ $Author: pwbldsvc $ 
   
when       who     what, where, why 
--------   ---    -------------------------------------------------------------- 
01/16/19   sbo    Added RFFE CLK enums 
07/12/18   mns    RFFE GRP changes.
05/25/18   mns    MMU Changes.
06/20/14   jc     Increased TQ pair and SW port size 
03/14/14   oei    Remove contents & replace by including FW's "ccs_rf_intf.h"
03/05/14   oei    Initial version rfmodem_dimepm, 16 tasks, use D3925 constants
12/16/13   Saul   CCS. Increase number of tasks per TQ to 32.
11/25/13   Saul   CCS. New CSS Data Buffer Allocation For All Techs.
10/18/13   Saul   CCS. RFFE EXT writes using DMA.
10/08/13   cd     Define task types for D3925
09/03/13   ra     Rebaseline: compatible with older modem and new CCS processor
07/31/13   hdz    Added more backward compatability support
07/23/13   ra     Initial version. Created to support backward compatability
==============================================================================*/ 
#include "comdef.h"
/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! 
   @brief In RFMODEM_DIMEPM, FW owns the CCS. As such, we need to rely on
   their definitions and structures for our RF CCS driver
 */
#include "ccs_rf_intf.h"


/*! 
  @brief Number of 32-bit words in the CCS data buffer for any TQ pair
  Each TQ pair is allocated 3 K bytes in the CCS data buffer.
  This define is created for general use whenever it is necessary to get
  the number of words used by any TQ pair.
  Total size in bytes / size of one word (4 bytes or uint32) = num words
*/
// Temporarily apply all increased memory from FW towards TQ pair
// Expecting 11K from FW, which would result in 4K per TQ pair
#if (CCS_RF_TASKQ_DATA_BUF_WORDS>((9*1024)/4))
#define CCS_RF_TASKQ_PAIRS_DATA_BUF_NUM_WORDS     ( (CCS_RF_TASKQ_DATA_BUF_WORDS-(3*1024)/4)/2 )
#else
#define CCS_RF_TASKQ_PAIRS_DATA_BUF_NUM_WORDS     ( (3*1024) / 4 )
#endif

/*! @brief TQ pair 0 address offset in the CCS data buffer
*/
#define CCS_RF_TASKQ_DATA_BUF_PAIR0_ADDR_OFFSET_WORDS     0

/*! @brief TQ pair 0 number of 32-bit words in the CCS data buffer
*/
#define CCS_RF_TASKQ_DATA_BUF_PAIR0_NUM_WORDS     CCS_RF_TASKQ_PAIRS_DATA_BUF_NUM_WORDS

/*! @brief TQ pair 1 address offset in the CCS data buffer
*/
#define CCS_RF_TASKQ_DATA_BUF_PAIR1_ADDR_OFFSET_WORDS    ( CCS_RF_TASKQ_DATA_BUF_PAIR0_ADDR_OFFSET_WORDS + CCS_RF_TASKQ_DATA_BUF_PAIR0_NUM_WORDS )

/*! @brief TQ pair 1 number of 32-bit words in the CCS data buffer
*/
#define CCS_RF_TASKQ_DATA_BUF_PAIR1_NUM_WORDS     CCS_RF_TASKQ_PAIRS_DATA_BUF_NUM_WORDS
 

/*! @brief TQ DEDICATED address offset in the CCS data buffer
*/
#define CCS_RF_TASKQ_DATA_BUF_DEDICATED_ADDR_OFFSET_WORDS    ( CCS_RF_TASKQ_DATA_BUF_IRAT_ADDR_OFFSET_WORDS + CCS_RF_TASKQ_DATA_BUF_IRAT_NUM_WORDS )

#define CCS_RF_TASKQ_TOTAL_DATA_BUF_WORDS CCS_RF_TASKQ_DATA_BUF_WORDS


/*! @brief  Enum for rffe clk*/
typedef enum
{
  RF_HAL_RFFE_9_p_6       = 0,  /*! 9.6 MHz */
  RF_HAL_RFFE_19_p_2      = 1,  /*! 19.2 MHz */
  RF_HAL_RFFE_38_p_4      = 2,  /*! 38.4 MHz*/
  RF_HAL_RFFE_MAX_NUM_CLK = 3, /*! Max number of rffe clks */
  RF_HAL_RFFE_INVALID_CLK = 0xFF
} rf_ccs_rffe_clk_enum;

/*! @brief  Struct for dynamically setting clk rate for one RFFE channel */
typedef struct
{
  uint8 rffe_channel;
  rf_ccs_rffe_clk_enum rffe_clk;
}rffe_clk_info_type;


/*! @brief Format of RFFE extended transactions (read or write).
 * */
typedef union
{
  struct {
    uint32 addr        : 16;  /*!< RFFE address */
    uint32 mode        : 2;   /*!< Transaction mode (see below) */
#define CCS_RF_TRANSACTION_ACCESS_EXT_MODE_WRITE     2
#define CCS_RF_TRANSACTION_ACCESS_EXT_MODE_READ      3
    uint32 num_data    : 4;   /*!< Number of data bytes (0=1 byte, 3=4 bytes) */
    uint32 slave_id    : 4;   /*!< RFFE slave ID */
    uint32 half_rate   : 1;   /*!< Half-rate read */
    uint32 rd_delay    : 1;   /*!< Insert extra cycle of delay before read-back */
    uint32 type        : 2;   /*!< Transaction type (see below) */
#define CCS_RF_TRANSACTION_RFFE_EXT_TYPE_REG0      0
#define CCS_RF_TRANSACTION_RFFE_EXT_TYPE_REG       1
#define CCS_RF_TRANSACTION_RFFE_EXT_TYPE_EXT       2
#define CCS_RF_TRANSACTION_RFFE_EXT_TYPE_EXT_LONG  3
uint32             : 2;   /*!< ** MUST be set to 0 */

  };

  uint32 transaction_word[1];

} ccs_rf_transaction_rffe_ext_group_rw_t;

#endif /* RFLM_CCS_TASK_DEFS_H */