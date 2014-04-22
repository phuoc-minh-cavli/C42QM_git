/*!
  @file
  nb1_pdcp_msg.h

  @brief
  This file contains all External message data structure and IDs interfaces
  exported by PDCP layer.


*/

/*===========================================================================

  Copyright (c) 2021 QUALCOMM Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/nb1.mpss/2.2/api/nb1_pdcp_msg.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/07/21   ve      CR 2909375 : Conn sus/resume fixes for OOS, MARB and PSM
03/18/19   ss      CR 2407883 : DMM suspend with RLF stop all timer and restart
03/07/19   ss      CR 2405310 : PSM Context restore only at UL and free at DL
12/03/18   ss      CR 2359564 : FR 42128: PSM NB1 user plane CIoT EPS optimization
10/31/18   ss      CR 2312885 : Next_PDCP_TX_SN, and TX_HFN to 0 upon suspend/resume
08/09/18   ss      CR 2294217 : NB1 Missing eMBMS and UDC featurization
07/15/18   ss      CR 2278167 : FR 50565: L2 task merge for NB
===========================================================================*/

#ifndef NB1_PDCP_MSG_H
#define NB1_PDCP_MSG_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <comdef.h>
#include <dsm_queue.h>
#include <msgr.h>
#include <msgr_nb1.h>
#include <lte_app.h>
#include "nb1_l2_comdef.h"
#include <nb1_security.h>
#ifdef FEATURE_NB1_UDC_ENABLED
#include <udcif.h>
#endif /*FEATURE_NB1_UDC_ENABLED*/

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/
/*-----------------------------------------------------------------------*/
/*     NB1 PDCP/RRC External message definitions                          */
/*-----------------------------------------------------------------------*/
/*! @brief PDCP seq len in bits
*/
#define NB1_PDCP_SRB_SEQ_LEN 5
#define NB1_PDCP_DRB_SEQ_LEN_7 7
#define NB1_PDCP_DRB_SEQ_LEN_12 12
#define NB1_PDCP_DRB_SEQ_LEN_15 15
#define NB1_PDCP_DRB_AM_SEQ_LEN 12
#define NB1_PDCP_CHK_SUM_BYTES_SIZE 5

/*! @brief PDCP RB Add/Modifed/Remove/Re-establish cfg action
*/
typedef enum
{
  NB1_PDCP_CFG_ACT_INVALID,
  NB1_PDCP_CFG_ACT_ADD,
  NB1_PDCP_CFG_ACT_MODIFIED,
  NB1_PDCP_CFG_ACT_MODIFIED_SUSPEND,    /*!< Action used for modifying then suspending RB */
  NB1_PDCP_CFG_ACT_MODIFIED_RESUME,     /*!< Action used for modifying then resuming RB */
  NB1_PDCP_CFG_ACT_REMOVE,
  NB1_PDCP_CFG_ACT_FULL_CONFIG          /*!< Action used for full configuration - retain
                                            the EPS id and its DS watermark, delete all
                                            other RB config  */
} nb1_pdcp_cfg_act_e;

/*! @brief PDCP Start/Stop type */
typedef enum
{
  NB1_PDCP_DEFAULT_INIT_DEINIT = 0, 
  NB1_PDCP_PSM_INIT_DEINIT = 1,
  NB1_PDCP_SHIM_CXT_INIT_DEINIT = 2,
  NB1_PDCP_INIT_DEINIT_NONE
} nb1_pdcp_init_deinit_type_e;

/*! @brief PDCP DL integrity verification result
*/
typedef enum
{
  NB1_PDCPDL_INTEGRITY_VERIFY_NA,
  NB1_PDCPDL_INTEGRITY_VERIFY_SUCCESS,
  NB1_PDCPDL_INTEGRITY_VERIFY_FAIL
} nb1_pdcpdl_integrity_verify_status_e;

/*! @brief PDCP discard timer not configured
*/
#define NB1_PDCP_DISCARD_TIMER_NOT_CONFIG NB1_OTA_IE_INFINITY_UINT16


/*! @brief DRB count info struct
*/

typedef struct
{
  nb1_rb_cfg_idx_t    rb_cfg_idx;      /*!< RB Cfg ID */
  uint32 count; /*!< Count*/
} nb1_pdcp_count_info_s;

/*!
  @brief
  An enum describing the bit positions in the
  nb1_pdcp_rohc_profile_mask_t
  If support of two ROHC profile identifiers with the same 8 LSB's is signalled,
  only the profile corresponding to the highest value will be applied by RRC.
  */
enum
{
  NB1_PDCP_ROHC_PROFILE_ID_0X0000 = 0x0000,
  NB1_PDCP_ROHC_PROFILE_ID_0X0001 = 0x0001,
  NB1_PDCP_ROHC_PROFILE_ID_0X0002 = 0x0002,
  NB1_PDCP_ROHC_PROFILE_ID_0X0003 = 0x0004,
  NB1_PDCP_ROHC_PROFILE_ID_0X0004 = 0x0008,
  NB1_PDCP_ROHC_PROFILE_ID_0X0006 = 0x0010,
  NB1_PDCP_ROHC_PROFILE_ID_0X0101 = 0x0021,
  NB1_PDCP_ROHC_PROFILE_ID_0X0102 = 0x0042,
  NB1_PDCP_ROHC_PROFILE_ID_0X0103 = 0x0084,
  NB1_PDCP_ROHC_PROFILE_ID_0X0104 = 0x0108,
  /*! UPDATE the value below whenever any of the above bitmasks change */
  NB1_PDCP_ROHC_PROFILE_ID_CURRENT_VALID_MASK_BITS = 0x1FF
} ;

/*! @brief
  ROHC profile mask that indicates what ROHC profiles are supported
*/
typedef uint32 nb1_pdcp_rohc_profile_mask_t;

/*! @brief PDCP bearer type */
typedef enum
{
  NB1_PDCP_BEARER_TYPE_DEFAULT = 0,
  NB1_PDCP_BEARER_TYPE_ROHC = 1,
  NB1_PDCP_BEARER_TYPE_UDC = 2,
  NB1_PDCP_BEARER_TYPE_NONE
} nb1_pdcp_bearer_type_e;

#ifdef FEATURE_NB1_UDC_ENABLED
/*!
  @brief An enum describing the bit positions in the
  nb1_pdcp_udc_cfg_mask_t */
enum
{
  /* RELEASE will be set in the following conditions
           i) If disableUDC (for all DRBs) is set by the eNB
           ii) If UDC Cfg is being released for individual DRBs*/
  NB1_PDCP_UDC_CFG_MASK_RELEASE = 0x01,
  /* RESET_BUFFER will be set in the following conditions
           i) If resetUDC (for all DRBs) is set by the eNB or
           ii) If UDC Cfg is being reset for individual DRBs*/
  NB1_PDCP_UDC_CFG_MASK_RESET_BUFFER = 0x02,
  NB1_PDCP_UDC_CFG_MASK_ADD_MOD = 0x04,
  NB1_PDCP_UDC_CFG_MASK_MAX
};

/*! @brief PDCP UDC Cfg Mask */
typedef uint16 nb1_pdcp_udc_cfg_mask_t;

/*! @brief PDCP UDC release reason type */
typedef enum
{
  NB1_PDCP_UDC_REL_REASON_NONE = 0, /*!< UDC release reason not applicable */
  NB1_PDCP_UDC_REL_REASON_ELS_RECFG = 1,  /*!< UDC cfg released explicitly through ELS Reconfig */
  NB1_PDCP_UDC_REL_REASON_LEGACY = 2, /*!< UDC cfg released implicitly due to legacy messages */
  NB1_PDCP_UDC_REL_REASON_MAX
} nb1_pdcp_udc_rel_reason_type_e;

/*! @brief UDC addmod config structure
*/
typedef struct
{
  /*! Algo version to be used
  Should be <= algoVersionSupported
  Default value is 1*/
  udc_lib_version_e_type          algo_version;

  /*! UDCMemorySize in KBytes
  Should be <= udcContextMemorySizeMax
  Default value is 4K*/
  udc_context_buf_size_e_type     cntx_buff_size;

  /*! UDCPrefillContextId
  Default value is "Use all zero buffer"*/
  uint16                          prefill_context_id;

  /*! UDCHeader Length In terms of 20 bytes Default value is 4*/
  uint8                           hdr_length;

} nb1_pdcp_udc_addmod_cfg_s;

/*! @brief UDC reset config structure
*/
typedef struct
{
  /*! If PDCP reset seq number is included in the UDC reset config */
  boolean                         reset_seq_num_present;

  /*! PDCP seq number that failed checksum and caused reset, range (0..4095)
    Though ASN1 allows PDCP seq num upto 32767, ELS spec states the max allowed
    value is 4095 */
  uint16                          reset_seq_num;

  /*! If PDCP checksum src bytes is included in the UDC reset config */
  boolean                         checksum_src_bytes_present;

  /*! If computed checksum fails, the source bytes from UL Compression memory
    that were used to compute the checksum; Size 5 bytes */
  uint8                          checksum_src_bytes[NB1_PDCP_CHK_SUM_BYTES_SIZE];

} nb1_pdcp_udc_reset_cfg_s;

/*! @brief UDC DRB details
*/
typedef struct
{
  /*!< UDC Cfg Mask*/
  nb1_pdcp_udc_cfg_mask_t         cfg_mask;

  /*!< Addmod config is applicable only if
    UDC cfg mask = NB1_PDCP_UDC_CFG_MASK_ADD_MOD */
  nb1_pdcp_udc_addmod_cfg_s       udc_addmod_cfg;

  /*!< Reset config is applicable only if
    UDC cfg mask = NB1_PDCP_UDC_CFG_MASK_RESET_BUFFER */
  nb1_pdcp_udc_reset_cfg_s        udc_reset_cfg;

  /*!< UDC release reason, when release reason is not applicable
    this field will be set to NB1_PDCP_UDC_REL_REASON_NONE */
  nb1_pdcp_udc_rel_reason_type_e  udc_rel_reason;
} nb1_pdcpul_udc_cfg_s;
#endif /*FEATURE_NB1_UDC_ENABLED*/

/*! @brief PDCP UL RB info definition
*/
typedef struct
{
  nb1_rb_id_t    rb_id;      /*!< RB ID */
  nb1_rb_cfg_idx_t rb_cfg_idx; /*!< RB cfg index */
  nb1_eps_id_t   eps_id;     /*!< EPS ID */

  uint8          seq_len;    /*!< PDCP seq len in bits (5, 7, 12 bits) */
  nb1_rb_type_e  rb_type;    /*!< RB type */
  uint32         discard_timer_new; /*!< PDCP discard timer in msec,
                                     NB1_PDCP_DISCARD_TIMER_NOT_CONFIG is used
                                     when discard timer is not configured or
                                     discard timer is infinity */

  nb1_pdcp_bearer_type_e bearer_type; /*!< Bearer type*/
  uint16          rohc_max_cid; /*!< ROHC max context ID */
  nb1_pdcp_rohc_profile_mask_t  rohc_mask;    /*!< ROHC mask */
} nb1_pdcpul_info_s;

/*! @brief PDCP DL RB info definition
*/
typedef struct
{
  nb1_rb_id_t    rb_id;      /*!< RB ID */
  nb1_rb_cfg_idx_t rb_cfg_idx; /*!< RB cfg index */
  nb1_eps_id_t   eps_id;     /*!< EPS ID */
  nb1_rb_type_e  rb_type;    /*!< RB type */
  uint8          seq_len; /*!< PDCP seq len in bits (5, 7, 12 bits) */
  nb1_pdcp_bearer_type_e bearer_type; /*!< Bearer type*/
  uint16         rohc_max_cid; /*!< ROHC max context ID */
  nb1_pdcp_rohc_profile_mask_t  rohc_mask;    /*!< ROHC mask */

} nb1_pdcpdl_info_s;

/*! @brief PDCP add/modified RB cfg action
*/
typedef struct
{
  nb1_rb_cfg_idx_t                    rb_cfg_idx; /*!< RB cfg index */
  nb1_pdcp_cfg_act_e                  cfg_act; /*!< cfg action (add/modified) */
} nb1_pdcp_cfg_act_s;

/* @brief security configuration
*/
typedef struct
{
  /*!< SRB Cipher Algo (NONE/SNOW3G/AES) */
  nb1_cipher_algo_e srb_cipher_algo;
  uint8        srb_cipher_key_index; /*!< SRB Cipher Key index */
  uint8        *srb_cipher_key_ptr; /*!< SRB Cipher Key Pointer */

  /*!< SRB Integrity Algo (NONE/SNOW3G/AES)*/
  nb1_integrity_algo_e srb_integrity_algo;
  uint8        srb_integrity_key_index; /*!< SRB Integrity Key index */
  uint8        *srb_integrity_key_ptr; /*!< SRB Integrity Key Pointer */

  /*!< DRB Cipher Algo (NONE/SNOW3G/AES) */
  nb1_cipher_algo_e drb_cipher_algo;
  uint8        drb_cipher_key_index; /*!< DRB Cipher Key index */
  uint8        *drb_cipher_key_ptr; /*!< DRB Cipher Key Pointer */
} nb1_pdcp_security_cfg_s;

/*! @brief configuration request from RRC to PDCPUL
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;  /*!< message header */
  /* Message payload */
  nb1_l2_cfg_reason_e                 cfg_reason; /*!< configuration reason mask */
  uint8                               num_released_rb; /*!< number of released RB */
  nb1_rb_cfg_idx_t                    released_rb_cfg_idx[NB1_MAX_ACTIVE_RB]; /*!< released RB cfg index */
  uint8                               num_add_modified_rb; /*!< number of add/modified RB */
  nb1_pdcp_cfg_act_s                  add_modified_rb[NB1_MAX_ACTIVE_RB]; /*!< cfg action for add/modified RB ID */
  nb1_pdcpul_info_s*                  rb_cfg_base_ptr;  /*!< UL RB config data base ptr */

  /*!< Whether security cfg is valid, PDCP will apply the security
       config from RRC only it is set to TRUE */
  boolean                             security_cfg_valid;
  nb1_pdcp_security_cfg_s             security_cfg;

  /*!<  Boolean to indciate RoHC-Continue-R11 field */
  boolean                             rohc_reset;
  /*!<  Boolean to indciate supended resume -R14 field */
  boolean                             suspend_resume;

} nb1_pdcpul_cfg_req_msg_s;

/*! @brief configuration request confirmation from PDCP UL to RRC
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;  /*!< message header */
  /* Message payload */
  nb1_errno_e                         status; /*!< status */
} nb1_pdcpul_cfg_cnf_msg_s;

/*! @brief configuration request from RRC to PDCPDL
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;  /*!< message header */
  /* Message payload */
  nb1_l2_cfg_reason_e                 cfg_reason; /*!< configuration reason mask */
  uint8                               num_released_rb; /*!< number of released RB */
  nb1_rb_cfg_idx_t                    released_rb_cfg_idx[NB1_MAX_ACTIVE_RB]; /*!< released RB cfg index */
  uint8                               num_add_modified_rb; /*!< number of add/modified RB */
  nb1_pdcp_cfg_act_s                  add_modified_rb[NB1_MAX_ACTIVE_RB]; /*!< cfg action for add/modified RB ID */
  nb1_pdcpdl_info_s*                  rb_cfg_base_ptr;  /*!< DL RB config data base ptr */

  /*!< Whether security cfg is valid, PDCP will apply the security
       config from RRC only it is set to TRUE */
  boolean                             security_cfg_valid;
  nb1_pdcp_security_cfg_s             security_cfg;

  /*!<  Boolean to indciate RoHC-Continue-R11 field */
  boolean                             rohc_reset;
  /*!<  Boolean to indciate supended resume -R14 field */
  boolean                             suspend_resume;

} nb1_pdcpdl_cfg_req_msg_s;

/*! @brief configuration request confirmation from PDCP DL to RRC
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;  /*!< message header */
  /* Message payload */
  nb1_errno_e                         status; /*!< status */
} nb1_pdcpdl_cfg_cnf_msg_s;

/*! @brief Counter Request from RRC to PDCPUL
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;  /*!< message header */
} nb1_pdcpul_counter_req_msg_s;

/*! @brief Counter Response from PDCPUL to RRC
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;  /*!< message header */
  uint8                               num_active_drb;
                                      /*!< number of active DRB */
  nb1_pdcp_count_info_s               drb_count_info[NB1_MAX_ACTIVE_RB];
                                      /*!< DRB info*/
} nb1_pdcpul_counter_cnf_msg_s;

/*! @brief Counter Request from RRC to PDCPDL
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;  /*!< message header */
} nb1_pdcpdl_counter_req_msg_s;

/*! @brief Counter Response from PDCPDL to RRC
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;  /*!< message header */
  nb1_pdcp_count_info_s               drb_count_info[NB1_MAX_ACTIVE_RB];
                                      /*!< DRB info*/
  uint8                               num_active_drb;
                                      /*!< number of active DRB */
} nb1_pdcpdl_counter_cnf_msg_s;

/*! @brief Enter Factory Test Mode(FTM) Request from FTM module to PDCPUL
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;  /*!< message header */
} nb1_pdcpul_enter_ftm_req_msg_s;

/*! @brief Enter Factory Test Mode(FTM) confirm from PDCPUL to FTM module
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;  /*!< message header */
} nb1_pdcpul_enter_ftm_cnf_msg_s;

/*! @brief Enter Factory Test Mode(FTM) Request from FTM module to PDCPDL
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;  /*!< message header */
} nb1_pdcpdl_enter_ftm_req_msg_s;

/*! @brief Enter Factory Test Mode(FTM) confirm from PDCPDL to FTM module
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;  /*!< message header */
} nb1_pdcpdl_enter_ftm_cnf_msg_s;

/*! @brief Exit Factory Test Mode(FTM) Request from FTM module to PDCPUL
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;  /*!< message header */
} nb1_pdcpul_exit_ftm_req_msg_s;

/*! @brief Exit Factory Test Mode(FTM) confirm from PDCPUL to FTM module
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;  /*!< message header */
} nb1_pdcpul_exit_ftm_cnf_msg_s;

/*! @brief Exit Factory Test Mode(FTM) Request from FTM module to PDCPDL
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;  /*!< message header */
} nb1_pdcpdl_exit_ftm_req_msg_s;

/*! @brief Exit Factory Test Mode(FTM) confirm from PDCPDL to FTM module
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;  /*!< message header */
} nb1_pdcpdl_exit_ftm_cnf_msg_s;

/*! @brief PDCPDL SDU Type
*/
typedef enum
{
  NB1_PDCP_SDU_SPEC,
  NB1_PDCP_SDU_ELS,
  NB1_PDCP_SDU_MAX
} nb1_pdcp_sdu_type_e;

/*! @brief signal data (SDU) request from RRC to PDCPUL
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;  /*!< message header */
  msgr_attach_s                       dsm_attach;
                                      /*!< dsm attachment
                                      contained signal message */
  /* Message payload */
  nb1_rb_cfg_idx_t                    rb_cfg_idx; /*!< RB cfg index */
  uint8                               mu_id; /*!< message unit identifier */

  /* Security Info. */
  boolean                             integrity_protect;  /*! need integrity protection. */
  boolean                             cipher; /*! need cipher. */

  /*!< Enum to indicate PDCP UL SDU type*/
  nb1_pdcp_sdu_type_e                 sdu_type;

} nb1_pdcpul_sdu_req_msg_s;

/*! @brief request from RRC to reset security state
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;  /*!< message header */
} nb1_pdcpdl_sec_reset_req_msg_s;

/*! @brief Number of different ranges of PDCP UL delays
*/
#define NB1_PDCP_DELAY_STATS_SIZE               21

/*! @brief PDCP UL statistics for UDC
  */
typedef struct
{
  uint32  num_udc_comp_pkts; /*!<*total num of UDC compressed pkts*/
  uint32  udc_comp_bytes; /*!<*total num of UDC compressed  bytes*/
  uint32  num_udc_fc_uncomp_pkts; /*!< total num of UDC uncompressed pkts*/
  uint32  udc_fc_uncomp_bytes; /*!<*total num of UDC non-compressed  bytes*/
  uint32  num_udc_fail;  /*!< Number of UDC compression failed*/
  uint16  num_udc_reset; /*!< Num of time UDC RESET is called */
  uint32  num_ue_udc_ctrl_pdus;      /*!< num of UE triggered UDC Ctrl PDUs */
  uint32  num_enb_udc_ctrl_pdus;      /*!< num of eNB triggered UDC Ctrl PDUs */
  uint32  num_udc_dsm_fc_down;         /*!< num of  CMD_DOWN for DSM monitor*/
  uint32  num_udc_dsm_fc_shutdown;        /*!< num of  CMD_SHUT_DOWN for DSM monitor*/
  uint32  num_udc_cpu_fc_down;         /*!< num of  CMD_DOWN for CPU monitor*/
  uint32  num_udc_cpu_fc_freeze;           /*!< num of  CMD_UP for CPU monitor*/
  uint32  num_enb_trigg_udc_reset;  /*!<num of UDC reset triggered by enB during non-HO/non-RLF*/
}nb1_pdcpul_udc_stats_s;

/*! @brief PDCP UL statistics, part of the control block
  */
typedef struct
{
  uint32  num_flow_ctrl_trig;  /*!< number of timer flow disable */
  uint32  num_data_pdu;        /*!< number of PDCP pdu transmitted */
  uint32  data_pdu_bytes;      /*!< PDCP PDU in bytes transmitted */
  uint32  data_bytes_from_ps;  /*!< Total bytes dequeued from PS Wm*/
  uint32  num_control_pdu;     /*!< number of PDCP pdu transmitted */
  uint32  control_pdu_bytes;   /*!< PDCP PDU in bytes transmitted */
  uint32  num_status_report;  /*!< number of status report transmitted */
  uint32  num_rohc_feedback;  /*!< number of ROHC feedback transmitted */
  uint32  num_rohc_fail;      /*!< nmuber of ROHC compression failed */
  uint32  num_discard_sdu;   /*!< number of time based discard SDUs */
  uint32  discard_sdu_bytes; /*!< time based discard SDUs bytes */
  uint32  num_retransmit_sdu;  /*!< number of retransmit SDUs */
  uint32  retransmit_sdu_bytes; /*!< retransmit SDU in bytes */
  uint32  num_piggyback_rohc_feedback_rxed;  /*!< number of piggyback ROHC
                                                  feedback rxed */
  uint32  num_rerohc_pdu_drop;  /*!< number of RoHC PDUs drop during HO*/
  uint32  rerohc_pdu_drop_bytes; /*!< number of RoHC PDU bytes drop during HO*/
  uint32  pdcp_delay_pkt_cnt[NB1_PDCP_DELAY_STATS_SIZE]; /*number of packets
  for each delay range bin. Last bin conatins packets having delay greater
  than a range*/
  nb1_pdcpul_udc_stats_s udc_stats;  /*!< Stats related to UDC*/
} nb1_pdcpul_stats_set_s;

/*! @brief UL RB statistics
*/
typedef struct
{
  nb1_eps_id_t eps_id;
  nb1_pdcpul_stats_set_s rb_stats;
}nb1_pdcpul_rb_stats_s;

typedef struct
{
  nb1_pdcpul_rb_stats_s rb_metric_stats[NB1_MAX_ACTIVE_DRB];
}nb1_pdcpul_qual_metrics_s;

/*! @brief UL data metric statistics
*/
typedef struct
{
  msgr_hdr_struct_type   hdr;          /*!< common message router header */
  nb1_eps_id_t eps_id;                     /*!< eps bearer for which info is requested*/
  nb1_pdcpul_qual_metrics_s* rb_stats_ptr; /*!< Pointer to the allocated memory where L2 will return the requested DRB stats*/
}nb1_pdcpul_rb_stats_req_msg_s;

#define NB1_PDCP_MAX_MUID_SIZE_IN_MSG 16

/*! @brief SDU request confirmation from PDCP UL to RRC for AM SRB
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;  /*!< message header */
  /* Message payload */
  uint8                               num_ack_muid; /*!< number of ack MU ID */
  uint8                               ack_muid[NB1_PDCP_MAX_MUID_SIZE_IN_MSG]; /*!< ack MU ID array */
  uint8                               num_nack_muid; /*!< number of nack MU ID */
  uint8                               nack_muid[NB1_PDCP_MAX_MUID_SIZE_IN_MSG]; /*!< nack MU ID array */
  uint8                               num_maybe_muid; /*!< number of maybe transmitted MU ID */
  uint8                               maybe_muid[NB1_PDCP_MAX_MUID_SIZE_IN_MSG]; /*!< maybe transmitted MU ID array */
} nb1_pdcpul_sdu_cnf_msg_s;

/*! @brief reconfiguration prep request from RRC to PDCPUL
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;  /*!< message header */
  /* Message payload */
  /*!< configuration reason mask - The applicable values are NB1_L2_CFG_HO,
    NB1_L2_CFG_RELEASE and NB1_L2_CFG_RADIO_FAILURE. The other config reasons
    are not applicable for this request */
  nb1_l2_cfg_reason_e                 cfg_reason; /*!< configuration reason mask */
  uint8                               num_released_rb; /*!< number of released RB */
  nb1_rb_cfg_idx_t                    released_rb_cfg_idx[NB1_MAX_ACTIVE_RB]; /*!< released RB cfg index */
  boolean                             dmm_suspend;  
} nb1_pdcpul_recfg_prep_req_msg_s;

/*! @brief reconfiguration prep request confirmation from PDCP UL to RRC
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;  /*!< message header */
  /* Message payload */
  nb1_errno_e                         status; /*!< status */
} nb1_pdcpul_recfg_prep_cnf_msg_s;

/*! @brief reconfiguration prep request from RRC to PDCPDL
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;  /*!< message header */
  /* Message payload */
  boolean                             hand_over; /*!< handover or not: TRUE for handover */
} nb1_pdcpdl_recfg_prep_req_msg_s;

/*! @brief reconfiguration prep request confirmation from PDCP DL to RRC
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;  /*!< message header */
  /* Message payload */
  nb1_errno_e                         status; /*!< status */
} nb1_pdcpdl_recfg_prep_cnf_msg_s;

/*! @brief signal message (SDU) indication from PDCP DL to RRC
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;  /*!< message header */
  msgr_attach_s                       dsm_attach; /*!< dsm attachment contained signal message */
  /* Message payload */
  nb1_rb_cfg_idx_t                    rb_cfg_idx; /*!< RB Cfg index */

  /* Security related info. */
  uint8                               pdcp_hdr;    /*!<PDCP PDU header*/
  /*The above hdr is being sent separately to RRC because it is part of the
  input to the integrity check function*/
  uint32                              count;      /*!< PDCP count */
  uint8                               mac_i[4];   /*!< MAC_I */

  /*!< Integrity Verfication Result */
  nb1_pdcpdl_integrity_verify_status_e     integrity_status;
  nb1_sfn_s                           sfn;        /*!< The system and subframe number */

  /*!< Enum to indicate PDCP DL SDU type*/
  nb1_pdcp_sdu_type_e                 sdu_type;

} nb1_pdcpdl_sdu_ind_msg_s;

/*-----------------------------------------------------------------------*/
/*     NB1 PDCP/PS External message definitions                          */
/*-----------------------------------------------------------------------*/
/*! @brief NB1_PDCPUL_HFN_UPDATE_REQ request message definition
*/

typedef struct
{
  msgr_hdr_struct_type   hdr;    /*!< common message router header */
  nb1_rb_cfg_idx_t       rb_cfg_idx; /*!< RB Cfg index */
} nb1_pdcpul_conn_reestab_req_msg_s;

typedef struct
{
  msgr_hdr_struct_type   hdr;    /*!< common message router header */
  nb1_rb_cfg_idx_t       rb_cfg_idx; /*!< RB Cfg index */
} nb1_pdcpdl_hfn_update_req_msg_s;

typedef struct
{
  msgr_hdr_struct_type   hdr;    /*!< common message router header */
  nb1_rb_cfg_idx_t       rb_cfg_idx; /*!< RB Cfg index */
} nb1_pdcpdl_hfn_state_reset_req_msg_s;

/*-----------------------------------------------------------------------*/
/*     NB1 PDCP DL/PDCP UL message definitions                          */
/*----------------------------------------------------------------------*/
typedef struct
{
  msgr_hdr_struct_type                hdr;  /*!< message header */
  msgr_attach_s                       dsm_attach; /*!< dsm attachment */
  /* Message payload */
  nb1_rb_cfg_idx_t                    rb_cfg_idx; /*!< RB Cfg index */
  /* dsm attachment contained PDCP peer control message from EnodeB */
} nb1_pdcpdl_peer_ctrl_ind_msg_s;

/*! @brief message indication of piggyback RoHC feedback received from eNB
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;        /*!< message header */
  msgr_attach_s                       dsm_attach; /*!< dsm attachment */
  uint16                              fb_cid;     /*!< feedback context ID */
  nb1_rb_cfg_idx_t                    rb_cfg_idx; /*!< RB Cfg index */
} nb1_pdcpdl_peer_pb_rohc_fb_ind_msg_s;

/*! @brief indication that is triggered when PDCP to A2 low watermark crosssed
*/
typedef struct
{
  msgr_hdr_struct_type                msg_hdr;    /*!< message header */
  nb1_eps_id_t                        eps_id; /*!< EPS ID */
} nb1_pdcpdl_wm_low_ind_msg_s;

/*! @brief NB1_PDCPXX_START_REQ request message definition
*/
typedef struct
{
  msgr_hdr_struct_type   msg_hdr;    /*!< common message router header */
  boolean tune_away_enabled;        /*!< TRUE indicates TA can happen */
  nb1_pdcp_init_deinit_type_e   pdcp_init_deinit_type;/*!< Init type message */
} nb1_pdcp_start_req_msg_s;

/*! @brief NB1_PDCPXX_START_CNF request message definition
*/
typedef struct
{
  msgr_hdr_struct_type   msg_hdr;    /*!< common message router header */
  nb1_errno_e            cnf_status; /*!< confimation status message */
} nb1_pdcp_start_cnf_msg_s;

/*! @brief NB1_PDCPXX_STOP_REQ request message definition
*/
typedef struct
{
  msgr_hdr_struct_type   msg_hdr;  /*!< common message router header */
  nb1_pdcp_init_deinit_type_e   pdcp_init_deinit_type;/*!< Init type message */
} nb1_pdcp_stop_req_msg_s;

/*! @brief NB1_PDCPXX_STOP_CNF request message definition
*/
typedef struct
{
  msgr_hdr_struct_type   msg_hdr;    /*!< common message router header */
  nb1_errno_e            cnf_status; /*!< confimation status message */
} nb1_pdcp_stop_cnf_msg_s;

/*! @brief NB1_PDCPXX_QSH_REQ request message definition
*/
typedef struct
{
  msgr_hdr_struct_type   msg_hdr;    /*!< common message router header */
    /*! whether to run analysis, reset stats ... */
  uint32 action;
  /*! category mask. Use enum qsh_cat_e values to define mask e.g.
      QSH_CAT_CFG|QSH_CAT_PERF (since the values are bit mask simple
      "or" will work) */
  uint32 category_mask;
  /*! whether the registered CB can run in sync/async mode.
      When called with async_allowed = false the clients MUST RUN
      the registered CB in the same task context which invoked their call back
      (e.g. error fatal). When called with async_allowed = true
      the clients have an option to run their registered CB in async mode. */
  boolean async_allowed;
} nb1_pdcp_qsh_analysis_req_msg_s;

/*! @brief UL data Stats CNF message
*/
typedef struct
{
  msgr_hdr_struct_type   msg_hdr;     /*!< common message router header */
  nb1_errno_e            cnf_status;  /*!< confirmation status message  - SUCCESS/FAILURE*/
  uint8                  num_report;  /*!<number of reported structure(s) (one report per DRB).*/
}nb1_pdcpul_rb_stats_cnf_msg_s;

/*! @brief indication that is triggered by PDCP to request RRC to trigger RLF
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;    /*!< message header */
} nb1_pdcpul_rlf_ind_msg_s;

/*! @brief indication that is triggered by PDCP to request RRC to trigger Conn rel
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;    /*!< message header */
} nb1_pdcpul_conn_rel_ind_msg_s;

/*! @brief RAB register IND from COMP to PDCP  for UL Data
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;         /*!< message header */
  /* Message payload */
  nb1_rb_cfg_idx_t                    rb_cfg_idx; /*!< RB cfg index */
  nb1_eps_id_t                        eps_id;     /*!< EPS ID */
  boolean                             send_ctrl_pdu; /*!< Send UDC ctrl PDU*/
} nb1_pdcpul_rab_reg_ind_msg_s;

/*! @brief indication that is triggered by PDCP to store context before enter PSM
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;    /*!< message header */
} nb1_pdcpul_contex_store_req_msg_s;
  
typedef struct
{
  boolean psm_entered;
  // TODO:
  //nb1_rb_rohc_ctx_s       nb1_rb_rohc_ctx; /*!< TODO: ROHC */
} nb1_pdcpul_psm_db_s;

/*! @brief indication that is triggered by PDCP to store context before enter PSM
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;    /*!< message header */
  boolean                             store_request;
  nb1_pdcpul_psm_db_s *               nb1_pdcpul_psm_db_restore;
      // TODO: ROHC Context store
} nb1_pdcpul_contex_store_restore_cnf_msg_s;

/*-----------------------------------------------------------------------*/
/*     NB1 PDCPUL External Request messages                              */
/*-----------------------------------------------------------------------*/
/*! @brief external requests that the NB1 PDCPUL module receives.  */
enum
{
  NB1_PDCPUL_FIRST_REQ = MSGR_UMID_BASE(MSGR_NB1_PDCPUL, MSGR_TYPE_REQ),
  MSGR_DEFINE_UMID( NB1,PDCPUL,REQ,CFG,0x00,
                   nb1_pdcpul_cfg_req_msg_s),
  MSGR_DEFINE_UMID( NB1,PDCPUL,REQ,SDU,0x01,
                   nb1_pdcpul_sdu_req_msg_s),
  MSGR_DEFINE_UMID( NB1,PDCPUL,REQ,RECFG_PREP,0x02,
                   nb1_pdcpul_recfg_prep_req_msg_s),
  MSGR_DEFINE_UMID( NB1,PDCPUL,REQ,COUNTER,0x04,
                   nb1_pdcpul_counter_req_msg_s),
  MSGR_DEFINE_UMID( NB1,PDCPUL,REQ,ENTER_FTM,0x05,
                   nb1_pdcpul_enter_ftm_req_msg_s),
  MSGR_DEFINE_UMID( NB1,PDCPUL,REQ,EXIT_FTM,0x06,
                   nb1_pdcpul_exit_ftm_req_msg_s),
  MSGR_DEFINE_UMID( NB1,PDCPUL,REQ,START,0x07,
                   nb1_pdcp_start_req_msg_s),
  MSGR_DEFINE_UMID( NB1,PDCPUL,REQ,STOP,0x08,
                   nb1_pdcp_stop_req_msg_s),
  MSGR_DEFINE_UMID( NB1,PDCPUL,REQ,RB_STATS,0x0e,
                   nb1_pdcpul_rb_stats_req_msg_s),
  MSGR_DEFINE_UMID( NB1,PDCPUL,REQ,OFFLOAD_RAB_DEREGISTER,0x0f,
                   nb1_pdcp_offload_rab_dereg_ind_msg_s),
  MSGR_DEFINE_UMID( NB1,PDCPUL,REQ,CONN_REESTAB,0x10,
                   nb1_pdcpul_conn_reestab_req_msg_s),
  MSGR_DEFINE_UMID( NB1,PDCPUL,REQ,CXT_STORE,0x11,
                   nb1_pdcpul_contex_store_req_msg_s),
  NB1_PDCPUL_MAX_REQ,
  NB1_PDCPUL_LAST_REQ = NB1_PDCPUL_MAX_REQ - 1
};

/*-----------------------------------------------------------------------*/
/*     NB1 PDCPDL External Request messages                              */
/*-----------------------------------------------------------------------*/
/*! @brief external requests that the NB1 PDCPDL module sends/receives.  */
enum
{
  NB1_PDCPDL_FIRST_REQ = MSGR_UMID_BASE(MSGR_NB1_PDCPDL, MSGR_TYPE_REQ),
  MSGR_DEFINE_UMID( NB1,PDCPDL,REQ,CFG,0x00,
                   nb1_pdcpdl_cfg_req_msg_s),
  MSGR_DEFINE_UMID( NB1,PDCPDL,REQ,RECFG_PREP,0x01,
                   nb1_pdcpdl_recfg_prep_req_msg_s),
  MSGR_DEFINE_UMID( NB1,PDCPDL,REQ,COUNTER,0x03,
                   nb1_pdcpdl_counter_req_msg_s),
  MSGR_DEFINE_UMID( NB1,PDCPDL,REQ,ENTER_FTM,0x04,
                   nb1_pdcpdl_enter_ftm_req_msg_s),
  MSGR_DEFINE_UMID( NB1,PDCPDL,REQ,EXIT_FTM,0x05,
                   nb1_pdcpdl_exit_ftm_req_msg_s),
  MSGR_DEFINE_UMID( NB1,PDCPDL,REQ,START,0x06,
                   nb1_pdcp_start_req_msg_s),
  MSGR_DEFINE_UMID( NB1,PDCPDL,REQ,STOP,0x07,
                   nb1_pdcp_stop_req_msg_s),
  MSGR_DEFINE_UMID( NB1,PDCPDL,REQ,FC,0x08,
                   cfm_fc_cmd_msg_s),
  MSGR_DEFINE_UMID( NB1,PDCPDL,REQ,SEC_RESET,0x09,
                   nb1_pdcpdl_sec_reset_req_msg_s),
  MSGR_DEFINE_UMID( NB1,PDCPDL,REQ,A2_RAB_DEREGISTER,0x0d,
                   nb1_pdcpdl_a2_rab_deregister_req_msg_s),
  MSGR_DEFINE_UMID( NB1,PDCPDL,REQ,ROHC_RESET,0x0f,
                   nb1_pdcpdl_decomp_cfg_req_msg_s),
  MSGR_DEFINE_UMID( NB1,PDCPDL,REQ,QSH_ANALYSIS,0x10,
                   nb1_pdcp_qsh_analysis_req_msg_s),
  MSGR_DEFINE_UMID( NB1,PDCPDL,REQ,CIPH_RESET,0x11,
                   nb1_pdcpdl_ciph_reset_req_msg_s),
  MSGR_DEFINE_UMID( NB1,PDCPDL,REQ,HFN_UPDATE,0x12,
                   nb1_pdcpdl_hfn_update_req_msg_s),
  MSGR_DEFINE_UMID( NB1,PDCPDL,REQ,HFN_STATE_RESET,0x13,
                   nb1_pdcpdl_hfn_state_reset_req_msg_s),
  NB1_PDCPDL_MAX_REQ,
  NB1_PDCPDL_LAST_REQ = NB1_PDCPDL_MAX_REQ - 1
};

/*-----------------------------------------------------------------------*/
/*     NB1 PDCPUL External Confirmation messages                         */
/*-----------------------------------------------------------------------*/
/*! @brief external Confirmations that the NB1 PDCPUL module sends  */
enum
{
  NB1_PDCPUL_FIRST_CNF = MSGR_UMID_BASE(MSGR_NB1_PDCPUL, MSGR_TYPE_CNF),
  MSGR_DEFINE_UMID( NB1,PDCPUL,CNF,CFG,0x00,
                   nb1_pdcpul_cfg_cnf_msg_s),
  MSGR_DEFINE_UMID( NB1,PDCPUL,CNF,RECFG_PREP,0x01,
                   nb1_pdcpul_recfg_prep_cnf_msg_s),
  MSGR_DEFINE_UMID( NB1,PDCPUL,CNF,SDU,0x02,
                   nb1_pdcpul_sdu_cnf_msg_s),
  MSGR_DEFINE_UMID( NB1,PDCPUL,CNF,COUNTER,0x04,
                   nb1_pdcpul_counter_cnf_msg_s),
  MSGR_DEFINE_UMID( NB1,PDCPUL,CNF,ENTER_FTM,0x05,
                   nb1_pdcpul_enter_ftm_cnf_msg_s),
  MSGR_DEFINE_UMID( NB1,PDCPUL,CNF,EXIT_FTM,0x06,
                   nb1_pdcpul_exit_ftm_cnf_msg_s),
  MSGR_DEFINE_UMID( NB1,PDCPUL,CNF,START,0x07,
                   nb1_pdcp_start_cnf_msg_s),
  MSGR_DEFINE_UMID( NB1,PDCPUL,CNF,STOP,0x08,
                   nb1_pdcp_stop_cnf_msg_s),
  MSGR_DEFINE_UMID( NB1,PDCPUL,CNF,RB_STATS,0x0a,
                   nb1_pdcpul_rb_stats_cnf_msg_s),
  MSGR_DEFINE_UMID( NB1,PDCPUL,CNF,CXT_STORE_RESTORE,0x0b,
                   nb1_pdcpul_contex_store_restore_cnf_msg_s),

  NB1_PDCPUL_MAX_CNF,
  NB1_PDCPUL_LAST_CNF = NB1_PDCPUL_MAX_CNF - 1
};

/*-----------------------------------------------------------------------*/
/*     NB1 PDCPDL External Confirmation messages                         */
/*-----------------------------------------------------------------------*/
/*! @brief external Confirmations that the NB1 PDCPDL module sends  */
enum
{
  NB1_PDCPDL_FIRST_CNF = MSGR_UMID_BASE(MSGR_NB1_PDCPDL, MSGR_TYPE_CNF),
  MSGR_DEFINE_UMID( NB1,PDCPDL,CNF,CFG,0x00,
                   nb1_pdcpdl_cfg_cnf_msg_s),
  MSGR_DEFINE_UMID( NB1,PDCPDL,CNF,RECFG_PREP,0x01,
                   nb1_pdcpdl_recfg_prep_cnf_msg_s),
  MSGR_DEFINE_UMID( NB1,PDCPDL,CNF,COUNTER,0x03,
                   nb1_pdcpdl_counter_cnf_msg_s),
  MSGR_DEFINE_UMID( NB1,PDCPDL,CNF,ENTER_FTM,0x04,
                   nb1_pdcpdl_enter_ftm_cnf_msg_s),
  MSGR_DEFINE_UMID( NB1,PDCPDL,CNF,EXIT_FTM,0x05,
                   nb1_pdcpdl_exit_ftm_cnf_msg_s),
  MSGR_DEFINE_UMID( NB1,PDCPDL,CNF,START,0x06,
                   nb1_pdcp_start_cnf_msg_s),
  MSGR_DEFINE_UMID( NB1,PDCPDL,CNF,STOP,0x07,
                   nb1_pdcp_stop_cnf_msg_s),
  MSGR_DEFINE_UMID( NB1,PDCPDL,CNF,HFN_UPDATE,0x09,
                   nb1_pdcpdl_hfn_update_cnf_msg_s),
  NB1_PDCPDL_MAX_CNF,
  NB1_PDCPDL_LAST_CNF = NB1_PDCPDL_MAX_CNF - 1
};

/*-----------------------------------------------------------------------*/
/*     NB1 PDCPDL External Indication messages                           */
/*-----------------------------------------------------------------------*/
/*! @brief external Indications that the NB1 PDCPDL module sends  */
enum
{
  NB1_PDCPDL_FIRST_IND = MSGR_UMID_BASE(MSGR_NB1_PDCPDL, MSGR_TYPE_IND),
  MSGR_DEFINE_UMID( NB1,PDCPDL,IND,SDU,0x00,
                   nb1_pdcpdl_sdu_ind_msg_s),
  MSGR_DEFINE_UMID( NB1,PDCPDL,IND,PEER_CTRL,0x01,
                   nb1_pdcpdl_peer_ctrl_ind_msg_s),
  MSGR_DEFINE_UMID( NB1,PDCPDL,IND,TX_STATUS,0x02,
                   nb1_pdcpdl_peer_ctrl_ind_msg_s),
  MSGR_DEFINE_UMID( NB1,PDCPDL,IND,TX_ROHC_FB,0x03,
                   nb1_pdcpdl_peer_ctrl_ind_msg_s),
  MSGR_DEFINE_UMID( NB1,PDCPDL,IND,WM_LOW,0x04,
                   nb1_pdcpdl_wm_low_ind_msg_s),
  MSGR_DEFINE_UMID( NB1,PDCPDL,IND,PEER_ROHC_FB,0x05,
                   nb1_pdcpdl_peer_ctrl_ind_msg_s),
  NB1_PDCPDL_MAX_IND,
  NB1_PDCPDL_LAST_IND = NB1_PDCPDL_MAX_IND - 1
};

/*-----------------------------------------------------------------------*/
/*     NB1 PDCPUL External Indication messages                           */
/*-----------------------------------------------------------------------*/
/*! @brief external Indications that the NB1 PDCPUL module sends         */
enum
{
  NB1_PDCPUL_FIRST_IND = MSGR_UMID_BASE(MSGR_NB1_PDCPUL, MSGR_TYPE_IND),
  MSGR_DEFINE_UMID( NB1,PDCPUL,IND,RL_FAIL, 0x00,
                   nb1_pdcpul_rlf_ind_msg_s),
  MSGR_DEFINE_UMID( NB1,PDCPUL,IND,CONN_REL, 0x01,
                   nb1_pdcpul_conn_rel_ind_msg_s),
  MSGR_DEFINE_UMID( NB1,PDCPUL,IND,RAB_REG, 0x02,
                   nb1_pdcpul_rab_reg_ind_msg_s),
/* Temporary. Remove after Posix support for signals*/
  NB1_PDCPUL_MAX_IND,
  NB1_PDCPUL_LAST_IND = NB1_PDCPUL_MAX_IND - 1
};

/*-----------------------------------------------------------------------*/
/*     NB1 PDCPUL Internal timer Indication messages                           */
/*-----------------------------------------------------------------------*/
/*! @brief Internal timer Indications that the NB1 PDCPUL module receives  */
enum
{
  NB1_PDCPUL_FIRST_TMRI = MSGR_UMID_BASE(MSGR_NB1_PDCPUL, MSGR_TYPE_TMRI),
  MSGR_DEFINE_UMID( NB1,PDCPUL,TMRI,DISCARD_TIMER_TICK_EXPIRED,0x00,(void*)),
  MSGR_DEFINE_UMID( NB1,PDCPUL,TMRI,LOG_STATS_FLUSH_TIMER_EXPIRED,0x01,(void*)),
  MSGR_DEFINE_UMID( NB1,PDCPUL,TMRI,WDOG_TIMER_EXPIRED,0x02,(void*)),
  #ifdef FEATURE_NB1_UDC_ENABLED
  MSGR_DEFINE_UMID( NB1,PDCPUL,TMRI,UDC_RESET_TIMER_EXPIRED,0x3,(void*)),
  #endif /*FEATURE_NB1_UDC_ENABLED*/
  NB1_PDCPUL_MAX_TMRI,
  NB1_PDCPUL_LAST_TMRI = NB1_PDCPUL_MAX_TMRI - 1
};

/*-----------------------------------------------------------------------*/
/*     NB1 PDCPDL Internal timer Indication messages                           */
/*-----------------------------------------------------------------------*/
/*! @brief Internal timer Indications that the NB1 PDCPDL module receives  */
enum
{
  NB1_PDCPDL_FIRST_TMRI = MSGR_UMID_BASE(MSGR_NB1_PDCPDL, MSGR_TYPE_TMRI),
  MSGR_DEFINE_UMID( NB1,PDCPDL,TMRI,LOG_FLUSH_TIMER_EXPIRED,0x00,(void*)),
  MSGR_DEFINE_UMID( NB1,PDCPDL,TMRI,WDOG_TIMER_EXPIRED,0x01,(void*)),
  NB1_PDCPDL_MAX_TMRI,
  NB1_PDCPDL_LAST_TMRI = NB1_PDCPDL_MAX_TMRI - 1
};

/*! @brief Supervisory messages that PDCP UL receives or throws
*/
enum
{
  MSGR_DEFINE_UMID( NB1, PDCPUL, SPR, LOOPBACK, MSGR_ID_LOOPBACK,
                   msgr_spr_loopback_s),
  MSGR_DEFINE_UMID( NB1, PDCPUL, SPR, LOOPBACK_REPLY, MSGR_ID_LOOPBACK_REPLY,
                   msgr_spr_loopback_reply_s),
  MSGR_DEFINE_UMID( NB1, PDCPUL, SPR, THREAD_READY, LTE_APP_ID_THREAD_READY,
                   none)
};

#endif /* NB1_PDCP_MSG_H */
