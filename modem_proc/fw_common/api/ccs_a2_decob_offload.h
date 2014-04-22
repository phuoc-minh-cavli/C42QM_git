/*!
  @file
  ccs_a2_decob_offload.h

  @brief
  This file contains the CCS-A2 DECOB offloading interface
 
*/

/*===========================================================================

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

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/fw/components/fw_common.mpss/rel/15.2/api/ccs_a2_decob_offload.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------

===========================================================================*/
#ifndef CCS_A2_DECOB_OFFLOAD_H
#define CCS_A2_DECOB_OFFLOAD_H

/*===========================================================================

      Constants

===========================================================================*/

/*! @brief Constant definition for max number of TBs in the DDR DECOB buffer
 *
 *  @description
 *  3 subframes x 3 carriers x 2 PDSCH TBs
 *  This is to match (worst case) DDR DECOB size so that DDR DECOB overrun 
 *  can be warned through this FIFO overflow. 
 *  Note: SIB/PAGE TBs are small, so they ignored here
 */
#define CCS_A2_DDR_DECOB_NUM_ELEM       (3 * 3 * 2)

/*! @brief Constant definition for number of bytes per TB in the DDR DECOB buffer
 *
 *  @description
 *  12288 bytes per TB (16 code blocks x 768 bytes/CB)
 */
#define CCS_A2_DDR_DECOB_ELEM_SIZE      12288

/*! @brief Constant definition for total number of bytes in the DDR DECOB buffer
 *
 *  @description
 */
#define CCS_A2_DDR_DECOB_NUM_BYTES      (CCS_A2_DDR_DECOB_ELEM_SIZE *\
                                         CCS_A2_DDR_DECOB_NUM_ELEM)

/*! @brief Constant definition for number of elements in the CCS-A2 FIFO
 *
 *  @description
 *  The FIFO has one more element than the DDR DECOB buffer, in order to 
 *  facilitate easy overflow detection. If the next write index matches the
 *  current read index, then the FIFO is full with CCS_A2_DDR_DECOB_NUM_ELEM 
 *  elements. This mirrors the number of elements in the DDR DECOB, such that
 *  FIFO full condition implies the DDR DECOB buffer is full.
 */
#define CCS_A2_DECOB_OFFLOAD_BUF_INST   (CCS_A2_DDR_DECOB_NUM_ELEM + 1)

/*===========================================================================

      Typedefs

===========================================================================*/

/*! 
 * @brief 
 * Information for the transport blocks to be copied from the DDR DECOB buffer
 */
typedef union {

  struct {
    uint32 ddr_decob_start_addr;  /*!< Starting physical address of the TB, within the DDR DECOB buffer */
                                     
    uint16 dma_byte_len;          /*!< Length of the TB, in bytes */

    uint16 tb_crc_pass    : 1;    /*!< Indicates if this TB passed CRC
                                       0: Fail
                                       1: Pass */
    uint16 dbe_idx        : 2;    /*!< Demback ID
                                       0: DBE 0
                                       1: DBE 1
                                       2: Reserved
                                       3: Reserved */
    uint16 reserved_0     : 13;   /*!< Reserved for future use */

    uint16 cell_id        : 9;    /*!< Cell ID */
    uint16 scell_id       : 3;    /*!< Serving Cell ID */
    uint16 pmch_id        : 4;    /*!< PMCH ID */ 

    uint16 subfn          : 4;    /*!< Subframe number */
    uint16 sfn            : 10;   /*!< System frame number */
    uint16 discard_tb     : 1;    /*!< Indicates if this TB needs to be discarded */ 
    uint16 mbsfn_ind      : 1;    /*!< Indicates if this is an MBSFN indication 
                                       0: No MBSFN indication 
                                       1: MBSFN indication */ 

    uint32 harq_mbsfn_area_id : 8;    /*!< HARQ ID (if not MBSFN)
                                           MBSFN area ID (if MBSFN) */
    uint32 rnti_type          : 4;    /*!< RNTI type
                                       0x0: C_RNTI
                                       0x1: SPS_C_RNTI
                                       0x2: P_RNTI
                                       0x3: RA_RNTI
                                       0x4: T_C_RNTI
                                       0x5: SI_RNTI
                                       0x6: TPC_PUSCH_RNTI
                                       0x7: TPC_PUCCH_RNTI
                                       0x8: MBMS_RNTI
                                           0x9--0xF: Reserved */
    uint32 earfcn             : 18;   /*!< EARFCN (max value is 262143) */
    uint32 reserved_1         : 2;    /*!< Reserved for future use */
  };

  uint32 word_arr[4];

} ccs_a2_decob_offload_info;

/*! 
 * @brief 
 * CCS-A2 DECOB offload FIFO
 */
typedef struct {

  uint32 rd_idx;                  /*!< Read index. This will be updated by A2 SW, as it consumes offloaded TBs. */
  uint32 wr_idx;                  /*!< Write index. This will be updated by CCS, as it offloads TBs. */

  ccs_a2_decob_offload_info tb_info[CCS_A2_DECOB_OFFLOAD_BUF_INST]; /*!< TB information */

} ccs_a2_decob_offload_buf;

#endif /* CCS_A2_DECOB_OFFLOAD_H */

