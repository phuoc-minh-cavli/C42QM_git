#ifndef NB1_CPHY_FTM_MSG_H
#define NB1_CPHY_FTM_MSG_H
/*!
  @file nb1_cphy_ftm_msg.h

  @brief
    The test interface to FTM task
*/

/*===========================================================================

  Copyright (c) 2016 QUALCOMM Technologies Incorporated. All Rights Reserved

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

$Header: //components/rel/nb1.mpss/2.2/api/nb1_cphy_ftm_msg.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "nb1_cphy_ftm_msg_ids.h"
#include "nb1_cphy_msg.h"
#include <msgr_nb1.h>
#include <IxErrno.h>

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief Structure for passing UL frequency related parameters
*/
typedef struct
{
  /** Message router header */
  msgr_hdr_struct_type                msg_hdr;

  /** Uplink bandwidth */
  nb1_bandwidth_e 			          ul_bandwidth;

  /** Uplink EARFCN */
  nb1_earfcn_t                       ul_frequency;
} nb1_cphy_test_tx_enable_req_s;


/*! @brief Structure for passing tx enable confirmation
*/
typedef struct
{
  /** Message router header */
  msgr_hdr_struct_type                msg_hdr;

  /** Uplink bandwidth */
  errno_enum_type                     status;

} nb1_cphy_test_tx_enable_cnf_s;

/*! @brief Structure for sending skip rach request to GM.
*/
typedef struct
{
  /** Message router header */
  msgr_hdr_struct_type                msg_hdr;

  /*! C-RNTI val */
  nb1_rnti_val_t                      c_rnti_val;
} nb1_cphy_test_gm_skip_rach_req_s ;

/*! @brief Skips DBCH decode and PRE-IDLE state in L1M after successful
           acquisition. It involves transition from ACQ_SUCCESS to IDLE_DRX state
           in L1 Manager.
*/
typedef struct
{
  /** Message router header */
  msgr_hdr_struct_type                msgr_hdr;
} nb1_cphy_test_skip_dbch_pre_idle_req_s;

/*! @brief Structure for sending skip rach request to ML1.
*/
typedef struct
{
  /*! Message router header */
  msgr_hdr_struct_type     msgr_hdr;
  /*! The timing adjust is in TA units (16 30.72 samples) */
  uint16                    ul_timing_advance;
  /*! C-RNTI val */
  nb1_rnti_val_t           c_rnti_val;
} nb1_cphy_test_ml1_skip_rach_req_s ;

/*! @brief L1 Mode type
*/
typedef enum
{
  /*! L1 ONLINE (default) mode: Process All messages */
  NB1_L1_ONLINE_MODE = 0,
  /*! L1 FTM Calibration mode: Drop All messages */
  NB1_L1_FTM_CAL_MODE,
  /*! L1 FTM Non-signalling mode:
      Process SKIP RACH/DBCH test msgs in addition to regular msgs
  */
  NB1_L1_FTM_NS_MODE,
  /*! Invalid MODE */
  NB1_L1_INVALID_MODE
} nb1_l1_proc_mode_e;

/*! @brief
    Request used by FTM task to configure L1 mode
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*! Activate following L1 mode */
  nb1_l1_proc_mode_e   mode;

} nb1_cphy_test_l1_mode_change_req_s;

/*! @brief
    Confirmation for mode change request to FTM task
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*! Status of mode change req */
  errno_enum_type      status;

} nb1_cphy_test_l1_mode_change_cnf_s;


/*! @brief ftm mode UL grant prams
*/
typedef struct
{
  /*! enable = TRUE = Use below UL grant params from FTM
      enable = FASLE = Use below UL grant params from PDCCH
  */
  boolean                         enable;
  /* MCS Index */
  uint8 mcs_idx;
  /* Number of RU */
  uint8 num_ru; 
  /* Tone BW type.  */
  uint8 tone_bw; 
  /* Starting tone for RU */
  uint8 start_tone_idx; 
  /* Tx power in dBm */
  int8 tx_pow_dbm;
} nb1_cphy_test_ftm_ul_grant_params_s;

/*! @brief
    Override UL grant request from FTM task
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type                msgr_hdr;

  /*! UL grant params */
  nb1_cphy_test_ftm_ul_grant_params_s ftm_ul_grant_params;

} nb1_cphy_test_override_ul_grant_req_s;

/*! @brief
    Reset PDSCH stats
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type                msgr_hdr;

} nb1_cphy_test_reset_pdsch_stats_req_s;

/*! @brief Structure for PDSCH stats req (for pcell)
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type                msgr_hdr;
} nb1_cphy_test_get_pdsch_stats_req_s;

/*! @brief Structure for PDSCH stats cnf
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type                msgr_hdr;

  /*! Total number of PDSCH transport blocks received */
  uint32       total_trblk_rcvd;

  /*! Total number of PDSCH transport blocks CRC pass stats */
  uint32      total_trblk_crc_pass;

  /*! Avg. real through put since pdsch monitor start req from FTM
      Avg. integer and floored value in Kbps =
      (tb size bits passed CRC *1000)/(pdsch collection period in ms * 1000)

      Calculation
      tb size bits passed CRC = (total tb CRC pass * Avg. TB size bytes * 8)
      total pdsch collection period in ms from pdsch stat log packet
  */
  uint32       dl_throughput;
} nb1_cphy_test_get_pdsch_stats_cnf_s;

/*! @brief Structure for all carriers PDSCH stats req
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type                msgr_hdr;
} nb1_cphy_test_get_all_carr_pdsch_stats_req_s;

/*! @brief Structure used by all carriers PDSCH stats cnf to report stats per carrier
  */
typedef struct
{
  uint8 carrier_id;

  /*! Scell id*/
  uint8 scell_id;

  /*! Total number of PDSCH transport blocks received */
  uint32 total_trblk_rcvd;

  /*! Total number of PDSCH transport blocks CRC pass stats */
  uint32 total_trblk_crc_pass;

  /*! Avg. real through put since pdsch monitor start req from FTM
   Avg. integer and floored value in Kbps =
   (tb size bits passed CRC *1000)/(pdsch collection period in ms * 1000)

   Calculation
   tb size bits passed CRC = (total tb CRC pass * Avg. TB size bytes * 8)
   total pdsch collection period in ms from pdsch stat log packet
   */
  uint32 dl_throughput;

} nb1_cphy_pdsch_stats_per_carrier_s;

/*! @brief Structure for all carriers PDSCH stats cnf
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*! Number of valid elements in pdsch_stat_per_carrier[] */
  uint8 num_carriers;

  nb1_cphy_pdsch_stats_per_carrier_s pdsch_stat_per_carrier[NB1_CPHY_MAX_SERVING_CELL];
} nb1_cphy_test_get_all_carr_pdsch_stats_cnf_s;

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/*! @brief
  Enum for the external ftm "test" msgs used to interface with L1
*/
enum
{
  /*=========================================================================*/
  /*                        Request/Confirm                                  */
  /*=========================================================================*/
  MSGR_DEFINE_UMID(
          NB1, CPHY_TEST, REQ, GM_SKIP_RACH, NB1_CPHY_TEST_GM_SKIP_RACH_ID,
          nb1_cphy_test_gm_skip_rach_req_s
          ),
  MSGR_DEFINE_UMID(
        NB1, CPHY_TEST, REQ, SKIP_DBCH_PRE_IDLE, NB1_CPHY_TEST_SKIP_DBCH_PRE_IDLE_ID,
        nb1_cphy_test_skip_dbch_pre_idle_req_s
        ),
  MSGR_DEFINE_UMID(
        NB1, CPHY_TEST, REQ, ML1_SKIP_RACH, NB1_CPHY_TEST_ML1_SKIP_RACH_ID,
        nb1_cphy_test_ml1_skip_rach_req_s
        ),
  MSGR_DEFINE_UMID(
          NB1, CPHY_TEST, REQ, TX_ENABLE, NB1_CPHY_TEST_TX_ENABLE_ID,
          nb1_cphy_test_tx_enable_req_s
          ),
  MSGR_DEFINE_UMID(
          NB1, CPHY_TEST, CNF, TX_ENABLE, NB1_CPHY_TEST_TX_ENABLE_ID,
          nb1_cphy_test_tx_enable_cnf_s
          ),
 MSGR_DEFINE_UMID(
          NB1, CPHY_TEST, REQ, L1_MODE_CHANGE, NB1_CPHY_TEST_L1_MODE_CHANGE_ID,
          nb1_cphy_test_l1_mode_change_req_s
          ),

  MSGR_DEFINE_UMID(
        NB1, CPHY_TEST, CNF, L1_MODE_CHANGE, NB1_CPHY_TEST_L1_MODE_CHANGE_ID,
        nb1_cphy_test_l1_mode_change_cnf_s
        ),

  MSGR_DEFINE_UMID(
        NB1, CPHY_TEST, REQ, OVERRIDE_UL_GRANT, NB1_CPHY_TEST_OVERRIDE_UL_GRANT_ID,
        nb1_cphy_test_override_ul_grant_req_s
        ),

  MSGR_DEFINE_UMID(
      NB1, CPHY_TEST, REQ, RESET_PDSCH_STATS, NB1_CPHY_TEST_RESET_PDSCH_STATS_ID,
      nb1_cphy_test_reset_pdsch_stats_req_s
      ),

  MSGR_DEFINE_UMID(
      NB1, CPHY_TEST, REQ, GET_PDSCH_STATS, NB1_CPHY_TEST_GET_PDSCH_STATS_ID,
      nb1_cphy_test_get_pdsch_stats_req_s
      ),

  MSGR_DEFINE_UMID(
    NB1, CPHY_TEST, CNF, GET_PDSCH_STATS, NB1_CPHY_TEST_GET_PDSCH_STATS_ID,
    nb1_cphy_test_get_pdsch_stats_cnf_s
      ),

  MSGR_DEFINE_UMID(
        NB1, CPHY_TEST, REQ, GET_ALL_CARR_PDSCH_STATS, NB1_CPHY_TEST_GET_ALL_CARR_PDSCH_STATS_ID,
        nb1_cphy_test_get_all_carr_pdsch_stats_req_s
        ),

  MSGR_DEFINE_UMID(
        NB1, CPHY_TEST, CNF, GET_ALL_CARR_PDSCH_STATS, NB1_CPHY_TEST_GET_ALL_CARR_PDSCH_STATS_ID,
        nb1_cphy_test_get_all_carr_pdsch_stats_cnf_s
    )
};


#endif /* NB1_CPHY_FTM_MSG_H */

