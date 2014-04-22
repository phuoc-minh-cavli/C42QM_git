/*!
  @file
  nb1_pdcp_ext_api.h

  @brief
  REQUIRED brief one-sentence description of this C header file.

  @detail
  OPTIONAL detailed description of this C header file.
  - DELETE this section if unused.

*/

/*==============================================================================

  Copyright (c) 2018 QUALCOMM Technologies Incorporated. All Rights Reserved

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

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/nb1.mpss/2.2/api/nb1_pdcp_ext_api.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
08/09/18   ss      CR 2294217 : NB1 Missing eMBMS and UDC featurization
07/15/18   ss      CR 2278167 : FR 50565: L2 task merge for NB
==============================================================================*/

#ifndef NB1_PDCP_EXT_API_H
#define NB1_PDCP_EXT_API_H

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include <comdef.h>
#include <nb1.h>
#include <queue.h>
#include <dsm_item.h>
#include <dsm_queue.h>
#include <mutils_circ_q.h>
#include <nb1_security.h>
#include <nb1_pdcp_msg.h>

/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/

/*! @brief PDCPDL->PDCP OFFLOAD circular queue size (number of elements)
     10ms*1 entry per TTI per bearer*total number of active bearers
*/
#define	NB1_MAX_ACTIVE_ROHC_RB 32

#define PDCP_DL_SEC_LTE_NUM_KEYS_MAX 16
#define PDCP_DL_SEC_KEY_SIZE 16

#define NB1_PDCP_CFG_DL_PDU_CIRC_Q_SIZE                    (NB1_MAX_ACTIVE_ROHC_RB)
/*macro to help OFFLOAD identify if the pkt is RoHC feedback or Data packet*/
#define NB1_PDCPDL_ROHC_FEEDBACK_PDU                     0xFF

/*! @brief Maximum UDC prefill context IDs */
#define NB1_PDCPUL_UDC_PREFILL_CNTXT_ID_MAX                8

/*! @brief DL data activity statistics
*/
typedef struct
{
  uint32  num_bytes;        /*!< data pdu bytes recieved*/
  uint32  num_pdus;         /*!< number of data PDUs received */
  uint32  num_pdus_dropped; /*!< number of data pdus dropped */
} nb1_pdcpdl_activity_stats_s;

/*! @brief PDCP indication optimization related structure.
    This is to track that PDCP layers are not indicating OFFLOAD task more than
    once if the signal/indication is not yet processed.
*/
typedef struct
{
  volatile boolean pending; /*!< indication is enqueued but not yet processed*/
  uint32  probable_enqueue_count; /*!< probable number of times indication
                                       command could have been enqueued */
  uint32  enqueue_count; /*<! absolute number of times indication command
                              enqueued for the receiver */
  uint32  dequeue_count; /*!< absolute number of times indication command
                              is received and processed by the receiver task */
} nb1_l2_ind_info_s;

/*! @brief PDCP DL-PDCP OFFLOAD circular PDCP PDU buffer element structure
*/
typedef struct
{
  nb1_rb_cfg_idx_t rb_cfg_idx;       /*!< unique RB configuration index */
  nb1_eps_id_t eps_id;
  dsm_item_type*   head_ptr;         /*!< dsm pointer to the 1st PDCP PDU */
  dsm_item_type*   tail_ptr;         /*!< dsm pointer to the Last PDCP PDU */
  uint32               num_pending_pdus;
} nb1_pdcpdl_pdu_buffer_element_s;

typedef struct
{
  uint8 key_index; /*!< valid range 0..15 */
  /*! Use uint8 instead of a2_cipher_algo_e algo to make sure size of this
      this sturcture is always 2 bytes in all compilers */
  uint8 algo;
}nb1_pdcpdl_cipher_init_param_s;

typedef union
{
  nb1_pdcpdl_cipher_init_param_s ciph_init_param;
  uint16 value;
}nb1_pdcpdl_cipher_param_u;

/*! @brief PDCP UL RLC WM state
*/
typedef enum
{
  NB1_PDCPUL_RLC_WM_SUSPEND = 0, /* enable */
  NB1_PDCPUL_RLC_WM_ENABLE = 1  /* suspend */
} nb1_pdcpul_rlc_wm_state_e;

/*! @brief PDCP UL RLC WM
*/
typedef struct
{
  dsm_watermark_type  wm_item; /*!< PDCP RLC tx watermark */
  q_type              wm_q;       /*!< PDCP RLC tx queue */
} nb1_pdcpul_rlc_wm_s;

#ifdef FEATURE_NB1_UDC_ENABLED
/*! @brief UDC Capability details
*/
typedef struct
{
  /*!< UDCMemorySizeMax */
  udc_context_buf_size_e_type  max_cntx_buff_size;

  /*!< UDCPrefillContextId List*/
  uint16  prefill_context_id_list[NB1_PDCPUL_UDC_PREFILL_CNTXT_ID_MAX];

  /*!< Number of UDCPrefillContextId*/
  uint8                          num_prefill_context_id;

  /*!< Algo version supported - bitfield */
  uint8                          algo_version_supported;

  /*!< Max number of DRBs on which compression is supported */
  uint8                          max_drb_supported;

  /*!< Indicates if UE supports buffer transfer for PDCP reestablishment */
  boolean                        buffer_transfer_supported;

} nb1_pdcpul_udc_cap_s;
#endif /*FEATURE_NB1_UDC_ENABLED*/
/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/


/*==============================================================================

  FUNCTION:  nb1_pdcpdl_get_activity_statistics

==============================================================================*/
/*!
    @brief
    access function for DL data statistics that can be called from other module.

    @detail
    Caller needs to be aware of the possibility of statistics overflow, and
    handle it appropriately.

    @return
    TRUE if the statistics returned is valid. FALSE if the statistics for the
    eps_id is not available (bearer removed, etc)
*/
/*============================================================================*/
EXTERN boolean _nb1_pdcpdl_get_activity_statistics
(
  nb1_eps_id_t                  eps_id,   /*!< eps ID */
  nb1_pdcpdl_activity_stats_s*  stats_ptr /*!< pointer to caller provided
                                               staticstics struct */
);
extern boolean nb1_pdcpdl_get_activity_statistics
(
  nb1_eps_id_t                  eps_id,   /*!< eps ID */
  nb1_pdcpdl_activity_stats_s*  stats_ptr /*!< pointer to caller provided
                                               staticstics struct */
);
 
/*==============================================================================

  FUNCTION:  nb1_pdcpul_get_pdu_bytes_tx

==============================================================================*/
/*!
    @brief
    Get the number of PDCP PDU bytes transmitted per epsid

    @detail
    Gives the total number of PDCP PDU bytes transmitted so far per epsid.
    The values are reset during the connection release.

    @return
    Number of PDCP PDU bytes transmitted per epsid if it is valid.
    0 Otherwise

*/
/*============================================================================*/
extern uint64 nb1_pdcpul_get_pdu_bytes_tx( nb1_eps_id_t epsid );
extern uint64 _nb1_pdcpul_get_pdu_bytes_tx
(
  nb1_eps_id_t epsid 
);


/*==============================================================================

  FUNCTION:  nb1_pdcpul_tag_each_pkt

==============================================================================*/
/*!
    @brief
    This function is called to tag eack packet with discard tick and populate
    kind fill as "NEW"

    @detail

    @return

    @note

    @see

*/
/*============================================================================*/

extern void nb1_pdcpul_tag_each_pkt
(
  dsm_item_type* pkt_ptr      /*packet to be enqueued to PS-PDCP WM*/
);
extern void _nb1_pdcpul_tag_each_pkt
(
  dsm_item_type* pkt_ptr      /*packet to be enqueued to PS-PDCP WM*/
);
/*==============================================================================

  FUNCTION:  nb1_pdcpul_tag_and_enqueue_comp_pkt

==============================================================================*/
/*!
    @brief
    This function is called to tag eack packet with discard tick.
    it also enqueues RoHC packet to WM passed during
    function call

    @detail

    @return

    @note

    @see

*/
/*============================================================================*/

extern void nb1_pdcpul_tag_and_enqueue_comp_pkt
(
  dsm_watermark_type* wm_ptr,
  dsm_item_type* pkt_ptr      /*packet to be enqueued to PS-PDCP WM*/
);

/*==============================================================================

  FUNCTION:  nb1_pdcpul_get_udc_capability

==============================================================================*/
/*!
    @brief
    This function returns the UDC capability parameters

    @detail

    @return

    @note

    @see

*/
/*============================================================================*/
#ifdef FEATURE_NB1_UDC_ENABLED
extern void nb1_pdcpul_get_udc_capability
(
  nb1_pdcpul_udc_cap_s* udc_cap_ptr /* UDC capability pointer */
);
#endif /*FEATURE_NB1_UDC_ENABLED*/
#endif /* NB1_PDCP_EXT_API_H */
