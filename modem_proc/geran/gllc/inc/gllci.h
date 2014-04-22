/*****************************************************************************
***
*** TITLE  gllci.h
***
***  GPRS LLC Layer Internal Definitions
***
***
*** DESCRIPTION
***
***  Provides LLC definitions for the exclusive use of the LLC module files
***  only.
***
***
*** Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
*** All Rights Reserved. QUALCOMM Proprietary.
***
*** Export of this technology or software is regulated by the U.S.
*** Government. Diversion contrary to U.S. law prohibited.
***
*****************************************************************************/


/*****************************************************************************
***
***                        EDIT HISTORY FOR MODULE
***
***  This section contains comments describing changes made to the module.
***  Notice that changes are listed in reverse chronological order.
***
*** $Header: //components/rel/geran.mpss/5.2.0/gllc/inc/gllci.h#8 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** mm/dd/yy   xyz     changes
***
*****************************************************************************/

#ifndef INC_GLLCI_H
#define INC_GLLCI_H


/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#include "customer.h" /* This include must be first before #ifdef FEATURE_  */

#include "comdef.h"
#include "gmutex.h"
#include "dsm.h"
#include "queue.h"
#include "rex.h"
#include "task.h"

#include "gllc.h"
#include "gllc_v.h"
#include "gllc_v_g.h"
#include "mmtask.h"
#include "mmtask_v.h"
#include "grlcllc.h"
#include "grlcllcif.h"
#include "geran_msgs.h"
#include "geran_tasks.h"
#include "gllcswgea34def.h"
//#include "gllc_static_main.h"

/*****************************************************************************
***
***     Public Type Definitions
***
*****************************************************************************/

#define GLLC_SAPI_NOT_SUPPORTED           0

/* ***************************************************************************
    Client Registration Record Internal Definitions
   ************************************************************************ */

typedef struct
{
  gllc_ll_dl_sig_get_mes_buf_t  ll_dl_sig_get_mes_buf_fnc_ptr;
  gllc_ll_dl_sig_put_mes_buf_t  ll_dl_sig_put_mes_buf_fnc_ptr;

  gllc_ll_dl_dat_put_mes_buf_t  ll_dl_dat_put_mes_buf_fnc_ptr;
} gllc_ll_client_record_t;


/* ***************************************************************************
    GPRS currently supported LL Service Access Points
   ************************************************************************ */

/* Changes to enum below were made to fix Klocwork errors. Error was that due
   to the use of GLLC_LL_NUM_SAPIS to define gllc_lle[ ] array size, Klockwork
   thinks that this array could be accessed with indexs GLLC_LL_NUM_SAPIS and
   GLLC_LL_SAPI_RESERVED when actual size of array is GLLC_LL_NUM_SAPIS thus
   over flowing.

   To fix this new enum value GLLC_LL_LAST_SAPI = GLLC_LL_SAPI_11_LL11 is
   introduced and GLLC_LL_NUM_SAPIS is #defined outside the enum. */

typedef enum
{
  GLLC_LL_SAPI_1_GMM  = 0,  /* Must be zero!             */
  GLLC_LL_SAPI_3_LL3,
  GLLC_LL_SAPI_5_LL5,
  GLLC_LL_SAPI_7_SMS,
  GLLC_LL_SAPI_9_LL9,
  GLLC_LL_SAPI_11_LL11,

  GLLC_LL_LAST_SAPI = GLLC_LL_SAPI_11_LL11,
  GLLC_LL_SAPI_RESERVED     /* Must be last!             */

} gllc_ll_sapi_t;
#define GLLC_LL_NUM_SAPIS   ((int32)GLLC_LL_LAST_SAPI+2) /* Defines array sizes */


/* ***************************************************************************
   Up Link Event Masking Scheme Definitions.
   ************************************************************************ */

/* These definitions represent conditions which require specific internal
   events to be masked off to prevent further processing until such time as
   the condition is cleared. */

/* Schemes which are not LLE specific (LLME schemes). */

typedef enum
{
  GLLC_LLME_SCHEME_LLGMM_SUSPEND,
  GLLC_LLME_SCHEME_GRR_SUSPEND,
  GLLC_LLME_SCHEME_DSM_SUSPEND,
  GLLC_LLME_SCHEME_TEST_MODE,

  GLLC_LLME_NUM_SCHEMES            /* Must be last! */

} gllc_llme_ul_evt_mask_scheme_t;

/* Schemes which are LLE specific. */

typedef enum
{
  GLLC_LLE_SCHEME_PEER_RCVR_BUSY,
  GLLC_LLE_SCHEME_RETRX_BUF_FULL,
  GLLC_LLE_SCHEME_WAIT_LAY3_RESP,
  GLLC_LLE_SCHEME_WAIT_PEER_RESP,
  GLLC_LLE_SCHEME_WAIT_ABM,

  GLLC_LLE_NUM_SCHEMES            /* Must be last! */

} gllc_lle_ul_evt_mask_scheme_t;


/* ***************************************************************************
   LLC PDU Frame Internal Representation Definitions
   ************************************************************************ */

/* LLC frame format identifiers. */

typedef enum
{
  GLLC_NO_FORMAT,

  GLLC_I_S_FORMAT,          /* Information plus supervisory format.         */
  GLLC_S_FORMAT,            /* Supervisory format.                          */
  GLLC_UI_FORMAT,           /* Unconfirmed information format.              */
  GLLC_U_FORMAT,            /* Unnumbered format.                           */

  GLLC_REJ_FORMAT           /* Rej fr format (internal representation only) */

} gllc_frame_format_t;

/* ------------------------------------------------------------------------ */

typedef enum
{
  GLLC_COMMAND,
  GLLC_RESPONSE

} gllc_cr_t;

/* ------------------------------------------------------------------------ */

/* Supervisory frame command and response identifiers. */

typedef enum
{
  GLLC_RR           = 0x0,  /* Receive Ready command/response.              */
  GLLC_RNR          = 0x2,  /* Receive Not Ready command/response.          */

  GLLC_ACK          = 0x1,  /* Acknowledge command/response.                */
  GLLC_SACK         = 0x3   /* Selective Acknowledge command/response.      */

} gllc_supervisory_t;

/* ------------------------------------------------------------------------ */

/* Unnumbered frame command and response identifiers. */

typedef enum
{
  GLLC_SABM         = 0x7,  /* Set Asynchronous Balanced Mode (ABM).        */
  GLLC_UA           = 0x6,  /* Unumbered Acknowledgment.                    */

  GLLC_DISC         = 0x4,  /* set DISConnected mode.                       */
  GLLC_DM           = 0x1,  /* Disconnected Mode.                           */

  GLLC_XID          = 0xB,  /* eXchange of IDentification.                  */
  GLLC_FRMR         = 0x8,  /* FRaMe Reject.                                */
  GLLC_NULL         = 0x0

} gllc_unnumbered_t;

/* ------------------------------------------------------------------------ */

/* GLLC_XID, GLLC_SABM, GLLC_UA: Exchange of identification information field
   definitions. */

typedef enum
{
  GLLC_XID_NONE,
  GLLC_XID_REQ,
  GLLC_XID_NEG

} gllc_xid_t;

/* ........................................................................ */

/* XID info field parameter header 'type' element definitions. */

#define GLLC_VERSION_XID_TYPE     0
#define GLLC_IOV_UI_XID_TYPE      1
#define GLLC_IOV_I_XID_TYPE       2
#define GLLC_T200_XID_TYPE        3
#define GLLC_N200_XID_TYPE        4
#define GLLC_N201_U_XID_TYPE      5
#define GLLC_N201_I_XID_TYPE      6
#define GLLC_mD_XID_TYPE          7
#define GLLC_mU_XID_TYPE          8
#define GLLC_kD_XID_TYPE          9
#define GLLC_kU_XID_TYPE          10
#define GLLC_LAY3_XID_TYPE        11
#define GLLC_RESET_XID_TYPE       12

/* ........................................................................ */

/* XID info field content bitmask definitions. */

#define GLLC_VERSION_XID          (1 << GLLC_VERSION_XID_TYPE )
#define GLLC_IOV_UI_XID           (1 << GLLC_IOV_UI_XID_TYPE  )
#define GLLC_IOV_I_XID            (1 << GLLC_IOV_I_XID_TYPE   )
#define GLLC_T200_XID             (1 << GLLC_T200_XID_TYPE    )
#define GLLC_N200_XID             (1 << GLLC_N200_XID_TYPE    )
#define GLLC_N201_U_XID           (1 << GLLC_N201_U_XID_TYPE  )
#define GLLC_N201_I_XID           (1 << GLLC_N201_I_XID_TYPE  )
#define GLLC_mD_XID               (1 << GLLC_mD_XID_TYPE      )
#define GLLC_mU_XID               (1 << GLLC_mU_XID_TYPE      )
#define GLLC_kD_XID               (1 << GLLC_kD_XID_TYPE      )
#define GLLC_kU_XID               (1 << GLLC_kU_XID_TYPE      )
#define GLLC_LAY3_XID             (1 << GLLC_LAY3_XID_TYPE    )
#define GLLC_RESET_XID            (1 << GLLC_RESET_XID_TYPE   )

#define GLLC_ALL_UL_XID                                                     \
        ( GLLC_VERSION_XID | GLLC_T200_XID   | GLLC_N200_XID |              \
          GLLC_N201_U_XID  | GLLC_N201_I_XID | GLLC_mD_XID   |              \
          GLLC_mU_XID      | GLLC_kD_XID     | GLLC_kU_XID     )

/* ........................................................................ */

/* Layer 2 XID parameter sizes in octets. */

#define GLLC_VERSION_XID_SIZE     1
#define GLLC_IOV_UI_XID_SIZE      4
#define GLLC_IOV_I_XID_SIZE       4
#define GLLC_T200_XID_SIZE        2
#define GLLC_N200_XID_SIZE        1
#define GLLC_N201_U_XID_SIZE      2
#define GLLC_N201_I_XID_SIZE      2
#define GLLC_mD_XID_SIZE          2
#define GLLC_mU_XID_SIZE          2
#define GLLC_kD_XID_SIZE          1
#define GLLC_kU_XID_SIZE          1
#define GLLC_RESET_XID_SIZE       0

/* ........................................................................ */

/* XID info field error map bitmask definitions. */

#define GLLC_XID_MULTI_INSTANCE   (1 << 0)
#define GLLC_XID_UNSUPPORTED_TYPE (1 << 1)
#define GLLC_XID_UNSUPPORTED_LEN  (1 << 2)
#define GLLC_XID_RESET_NOT_FIRST  (1 << 3)

/* ........................................................................ */

/* XID RESET State definitions. */

#define GLLC_LLME_XID_RESET_INACTIVE  0
#define GLLC_LLME_XID_RESET_SAPI_3    1
#define GLLC_LLME_XID_RESET_SAPI_5    2
#define GLLC_LLME_XID_RESET_SAPI_9    4
#define GLLC_LLME_XID_RESET_SAPI_11   8

/* ------------------------------------------------------------------------ */

/* Layer 3 XID parameter representation type definition. */

typedef struct
{
  uint8                     length;
  dsm_item_type             *params;

} gllc_l3_xid_info_t;

/* ........................................................................ */

/* XID information field */

typedef struct
{

  uint8                     errors;     /* Bitmsk of xid info errors.  */

  uint8                     version_xid;
  uint8                     n200_xid;

  uint8                     kd_xid;
  uint8                     ku_xid;



  uint16                    t200_xid;
  uint16                    contents;   /* Bitmsk of xid info contents */
  uint16                    n201_u_xid;
  uint16                    n201_i_xid;
  uint16                    md_xid;
  uint16                    mu_xid;
  uint32                    iov_ui_xid;
  uint32                    iov_i_xid;


  gllc_l3_xid_info_t        l3_xid;

} gllc_xid_info_t;

/* ------------------------------------------------------------------------ */

/* GLLC_FRMR: Frame reject information field content definition. */

typedef struct
{
  uint8                     reject_status_W4_W1;
  gllc_cr_t                 rejected_frame_CR;
  uint8                     rej_fr_ctrl_field[ 6 ]; /* Only first 6 octets. */
  uint16                    send_state_var_V_S;
  uint16                    recv_state_var_V_R;
} gllc_frmr_info_t;

/* ........................................................................ */

/* Frame reject status indication bitmap definitions for use in setting the
   reject_status field of gllc_frmr_info_t.

   W4: Indicates that the LLE was in ABM mode whilst reporting the frame
   reject condition. */

#define GLLC_FRMR_W4_IN_ABM               (1 << 3)

/* W3: Indicates that the control field received and returned in octets 1 & 2
   was undefined or not implemented. */

#define GLLC_FRMR_W3_INVALID_CTRL_FIELD   (1 << 2)

/* W2: Indicates that the information field received exceeded the maximum
   established information field length (N201) of the LLE reporting the
   rejection condition. */

#define GLLC_FRMR_W2_I_FIELD_LENGTH_ERR   (1 << 1)

/* W1: Indicates the control field received and returned in octets 1 & 2 was
   considered invalid because the frame contained an information field that
   is not permitted within this frame, or is a supervisory or unnumbered
   frame with incorrect length. W3 shall be set in conjunction with this
   bit. */

#define GLLC_FRMR_W1_INVALID_I_FIELD      (1 << 0)

/* ------------------------------------------------------------------------ */

/* LLC PDU information & supervisory frame format type def - element of
   gllc_frame_t definition. */

#define GLLC_SACK_BMP_LEN   32

typedef struct
{
  gllc_frame_format_t       format;             /* GLLC_I_S_FORMAT          */
  gllc_supervisory_t        cmd_res_S1S2;
  uint8                     sack_bitmap_len_K;
  uint8                     sack_bitmap_R[ GLLC_SACK_BMP_LEN ];

  gllc_cr_t                 cmd_or_res_CR;
  boolean                   ack_req_A;
  uint16                    send_seq_num_N_S;
  uint16                    recv_seq_num_N_R;
  uint32                    cipher_oc;
  dsm_item_type             *l3_pdu;
} gllc_i_s_frame_t;

/* ------------------------------------------------------------------------ */

/* LLC PDU supervisory frame format type def - element of gllc_frame_t
   definition. */

typedef struct
{
  gllc_frame_format_t       format;             /* GLLC_S_FORMAT            */
  gllc_supervisory_t        cmd_res_S1S2;
  uint8                     sack_bitmap_len_K;
  uint8                     sack_bitmap_R[ GLLC_SACK_BMP_LEN ];
  gllc_cr_t                 cmd_or_res_CR;
  boolean                   ack_req_A;
  uint16                    recv_seq_num_N_R;
} gllc_s_frame_t;

/* ------------------------------------------------------------------------ */

/* LLC PDU unconfirmed information frame format type def - element of
   gllc_frame_t definition. */

typedef struct
{
  gllc_frame_format_t       format;             /* GLLC_UI_FORMAT           */
  boolean                   encrypted_E;
  boolean                   protected_mode_PM;

  gllc_cr_t                 cmd_or_res_CR;
  uint16                    send_seq_num_N_U;
  uint32                    cipher_oc;

  dsm_item_type             *l3_pdu;

} gllc_ui_frame_t;

/* ------------------------------------------------------------------------ */

/* LLC PDU unnumbered frame format type def - element of gllc_frame_t
   definition. */

typedef union
{
  gllc_xid_info_t           xid_info;
  gllc_frmr_info_t          frmr_info;

} gllc_u_frame_info_t;

/* ........................................................................ */

#define GLLC_PF_POLL   1   /* Solicit(poll) a peer res when sending cmd. */
#define GLLC_PF_FINAL  1   /* Send a solicited res to a cmd from peer.   */
#define GLLC_PF_UNSOL  0   /* Send an unsolicited res to peer.           */

/* ........................................................................ */

typedef struct
{
  gllc_frame_format_t       format;        /* GLLC_U_FORMAT */

  gllc_cr_t                 cmd_or_res_CR;
  boolean                   poll_final_PF; /* Use GLLC_PF_POLL/GLLC_PF_FINAL */
  gllc_unnumbered_t         cmd_res_M4_M1;

  gllc_xid_t                xid;
  gllc_u_frame_info_t       info_field;

} gllc_u_frame_t;

/* ------------------------------------------------------------------------ */

/* LLC PDU frame type definition -

   For internal use during the collation of all pertinent content which is
   necesary to construct or represent an LLC PDU frame. */

typedef union
{
  gllc_frame_format_t       format;
  gllc_ui_frame_t           ui_frame;
  gllc_i_s_frame_t          i_s_frame;
  gllc_u_frame_t            u_frame;
  gllc_s_frame_t            s_frame;

} gllc_frame_t;


/* ***************************************************************************
   GRR Message Assembly and Disassembly Internal Working Structure Defs
   ************************************************************************ */

/* GRR up link message assembly working structure type definition -

   For internal use during the assembling of a GRR up link message for
   transmission. Message assembly takes place in the gprs_grr_ul_mes_t using
   collated frame information contained in the gllc_frame_t. */

typedef struct
{
  gllc_ll_sapi_t            lle;
  gllc_frame_t              *frame;
  uint16                    fr_hdr_length;
  uint16                    fr_info_length;
  uint32                    fr_fcs;
  uint8                     *sack_bitmap_R_p;
  gprs_grr_ul_mes_t         *grr_ul_mes;

} gllc_frame_assy_t;

/* ------------------------------------------------------------------------ */

/* GRR down link message disassembly working structure type definition -

   For internal use during the disassembling of a received GRR down link
   message. Message disassembly takes place in the gllc_frame_t using the
   source GRR message contents of gprs_grr_dl_mes_t. */

typedef struct
{
  gprs_grr_dl_mes_t         *grr_dl_mes;
  gllc_ll_sapi_t            lle;
  uint32                    fr_fcs;
  uint16                    fr_info_length;
  uint8                     fr_dis_err_map;
  gllc_cr_t                 rejected_frame_CR;
  uint8                     rej_fr_ctrl_field[ 6 ];
  uint8                     fr_info_debug_sample[ 4 ];
  gllc_frame_t              frame;

} gllc_frame_disassy_t;

/* ........................................................................ */

/* Frame disassembly error indication bitmap definitions for use in setting
   the fr_dis_err_map field of gllc_frame_disassy_t. */

/* *NOTE* - The least significant 4 bits of the error bitmap are intended
            to provide frame reject indication. Use GLLC_FRMR_*_W* defs. */

/* LL PDU being disassembled contains insufficient octets. */

#define GLLC_DIS_SHORT_ERR    (1 << 7)

/* LL PDU being disassembled contains an FCS error. */

#define GLLC_DIS_FCS_ERR      (1 << 6)

/* LL PDU contains a protocol discriminator bit which is set to 1, the SAPI
   address is either reserved or bears no current LL SAP client. */

#define GLLC_DIS_ADDR_ERR     (1 << 5)

/* LL PDU is to be rejected on one of the grounds given by the GLLC_FRMR_*_W*
   definitions in the lower nibble. */

#define GLLC_DIS_FRMR_ERR     (1 << 4)

/* ........................................................................ */

/* LL PDU Deciphering -
   Number of re-attempts to schedule in the event of initial FCS error at an
   increasingly advanced value of decipher OC. */

#define GLLC_NUM_DECIPHER_REATTEMPTS 3

/* ***************************************************************************
   Peer LLE Command/Response Message Internal Representation Definition.
   ************************************************************************ */

/* LLC PDU rejected frame information format type def - element of
   gllc_lle_cr_mes_u definition. */

typedef struct
{
  gllc_frame_format_t       format;             /* GLLC_REJ_FORMAT          */

  uint8                     fr_dis_err_map;
  gllc_cr_t                 rejected_frame_CR;
  uint8                     rej_fr_ctrl_field[ 6 ];

} gllc_rej_frame_t;

/* ------------------------------------------------------------------------ */

/* Peer status (command/response) message union internal definition -

   For the conveyance of peer command/response information from the LLC down
   link process to the LLC up link process, via the gllc_cr_ul_q message
   queue. */

typedef union
{
  gllc_frame_format_t       format;
  uint32                    rxd_tlli;

  gllc_u_frame_t            u_frame;
  gllc_rej_frame_t          rej_frame;

} gllc_lle_cr_mes_u;

/* ------------------------------------------------------------------------ */

/* Peer status (command/response) message type internal definition - */

typedef struct
{
  q_link_type               link;
  gllc_lle_cr_mes_u         mes;

} gllc_lle_cr_mes_t;


/* ***************************************************************************
    Logical Link Management Entity (LLME) Internal Structure Definitions
   ************************************************************************ */

/* LLME State Definition. */

typedef enum
{
  GLLC_LLME_TLLI_UNASSIGNED,
  GLLC_LLME_TLLI_ASSIGNED,
  GLLC_LLME_TEST_MODE

} gllc_llme_state_t;

/* ------------------------------------------------------------------------ */

/* LLME GAN State Definition */

typedef enum
{
  GLLC_LLME_GAN_INACTIVE   = 0,
  GLLC_LLME_GAN_ACTIVE     = 1,
  GLLC_LLME_NUM_GAN_STATES = 2

} gllc_llme_gan_state_t;

/* ------------------------------------------------------------------------ */

/* LLME Definition */

#define GLLC_TLLI_UNASSIGNED    0xFFFFFFFFUL

typedef struct
{
  uint8                 llc_version;
  gllc_llme_state_t     state;
  boolean               force_l3_flow_control;

  /* Current and old Temporary Logical Link Identifiers (TLLI) which are
     assigned to this MS. */

  uint32                tlli;
  boolean               tlli_rxd;
  uint32                tlli_old;
  uint32                tlli_last_rxd;

  /* Currently assigned GEA (GPRS Encryption Algorithm) cipher key and
     algorithm identifier. */

  uint32                key_Kc[2];
  uint32                key_Kc128[4];
  gmmllc_gea_e_type     gea;
  uint32                input_offset_IOV_UI;
  vect64                key_stream[1523];
  uint8                 key_stream_index;
  uint8                 key_stream_index_sub;

  /* Statistics relating to downlink frame receipt and decode. */

  uint32                fcs_errors_ctr;
  uint32                short_errors_ctr;
  uint32                addr_errors_ctr;
  uint32                frmr_errors_ctr;
  uint32                tlli_errors_ctr;

  /* Etsi Test Mode A/B parameters */

  uint16                  test_llc_pdu_size;
  uint16                  test_llc_pdu_num;
  uint8                   *test_llc_pdu_p;
  uint8                   test_mode;
  uint8                   loopback_mode;
  uint8                   ts_offset;
  boolean                 active_test_mode_b_or_srb;

  /* old xid indicator received from GMM during PS handover */

#ifdef FEATURE_GPRS_PS_HANDOVER
  boolean               use_old_xid_ind;
#endif /* FEATURE_GPRS_PS_HANDOVER */

  /* Indicates current state of GAN Mode */

  gllc_llme_gan_state_t gan_state;

  /* Bit map indicates which SAPI has xid reset in progress. 
     Bit-0 set for SAPI-3, Bit-1 set for SAPI-5,
	   Bit-2 set for SAPI-9, Bit-3 set for SAPI-11 */
  uint8                   xid_reset_in_progress;

  /* Set to TRUE when LLGMM-RESUME-REQ is received. Set to FALSE when GRR-RESUME-CNF
     or LLGMM-SUSPEND-REQ is received. */
  boolean               is_resume_cnf_pending;

} gllc_llme_t;

/* ***************************************************************************
    Structure Definitions to store info during Power Saving Mode
   ************************************************************************ */
typedef struct
{
  uint8     llc_version;
  uint32    input_offset_IOV_UI;
} gllc_llme_psm_info_t;

typedef struct
{
  uint8         max_reseq_buf_frames_kD;
  uint8         max_retrx_buf_frames_kU;
  uint16        xid_pending;
  uint16        max_retrx_cnt_N200;
  uint16        max_info_octets_N201_U;
  uint16        max_info_octets_N201_I;
  uint16        max_reseq_buf_octets_mD;
  uint16        max_retrx_buf_octets_mU;
  uint32        unack_encipher_oc;
  uint32        ack_encipher_oc;
  uint32        unack_decipher_oc;
  uint32        ack_decipher_oc;
  uint32        input_offset_IOV_I;
  uint32        retrx_interval_T200;
} gllc_lle_l2_info_t;

typedef struct
{
  boolean            l2_info_valid;
  uint8              l3_xid_len;
  uint8             *l3_xid_info;
  gllc_lle_l2_info_t l2_info;
} gllc_lle_psm_info_t;

typedef struct
{
  gllc_llme_psm_info_t   llme_info;
  gllc_lle_psm_info_t    lle_info[GLLC_LL_NUM_SAPIS];
} gllc_psm_info_t;

/* ***************************************************************************
    Logical Link Entity (LLE) Internal Structure Definitions
   ************************************************************************ */

/* LLE State Definition. */

typedef enum
{
  GLLC_LLE_TLLI_UNASSIGNED,
  GLLC_LLE_ADM,
  GLLC_LLE_LOC_ESTABLISH,
  GLLC_LLE_REM_ESTABLISH,
  GLLC_LLE_ABM,
  GLLC_LLE_LOC_RELEASE

} gllc_lle_state_t;

/* ------------------------------------------------------------------------ */

/* LLE Substate Definition. */

typedef enum
{
  GLLC_LLE_NONE,              /* No substate.                            */
  GLLC_LLE_L2_XID_EXCHANGE,   /* Substate of GLLC_LLE_ADM & GLLC_LLE_ABM */
  GLLC_LLE_L3_XID_EXCHANGE,   /* Substate of GLLC_LLE_ADM & GLLC_LLE_ABM */
  GLLC_LLE_L2_ESTABLISH,      /* Substate of GLLC_LLE_LOC_ESTABLISH      */
  GLLC_LLE_L3_ESTABLISH       /* Substate of GLLC_LLE_LOC_ESTABLISH      */

} gllc_lle_substate_t;

/* ------------------------------------------------------------------------ */

/* LLE operating parameters which are pertinent to both unacknowledged and
   acknowledged modes of operation. */

typedef struct
{
  boolean                   pdp_active;
  boolean                   psm_restore;
  /* Set to TRUE when ACK mode LLE is entered. Set to FALSE when an LLE is exiting
     ACK mode. */

  boolean                   is_grr_purge_data_req_enabled;

  uint16                    max_retrx_cnt_N200;
  uint32                    retrx_interval_T200;
} gllc_lle_general_t;

/* ------------------------------------------------------------------------ */

/* LLE operating parameters and state information which are pertinent to both
   the unacknowledged mode (ADM state only) and acknowledged mode (ABM and
   ADM states) of operation. */

typedef struct
{
  grr_data_req_cause_t      cause;
  /* Transaction Identifier in UL NAS signalling PDU */
  uint8                     transaction_id;
  uint8                     reliability_class;
  uint8                     radio_priority;
  uint8                     peak_throughput;

  uint8                     pfi;
  geran_pdu_priority_t      pdu_priority;

  /* Unacknowledged operation general negotiated paramaters. */

  uint16                    max_info_octets_N201_U;

  /* Unacknowledged operation state variables. */

  uint16                    send_state_var_V_U;
  uint16                    recv_state_var_V_UR;
  uint16                    l3_pdu_length;
  ll_sndcp_ref_t            sndcp_ref;

  uint32                    recv_state_bitmap;

  /* Encipher/decipher operating parameters. */

  uint32                    encipher_oc;
  uint32                    decipher_oc;
  /* Collated UI frame contents for frame assembly and transmission. */

  gllc_frame_t              frame;
} gllc_lle_unack_t;

/* ------------------------------------------------------------------------ */

/* Retransmission buffer element definition. */

typedef struct gllc_lle_retrx_buf
{
  struct gllc_lle_retrx_buf *next_p;
  struct gllc_lle_retrx_buf *prev_p;

  gllc_frame_t              frame;
  uint16                    l3_pdu_length;
  uint8                     radio_priority;
  uint8                     peak_throughput;

  uint8                     pfi;

  ll_sndcp_ref_t            sndcp_ref;
  grr_data_req_cause_t      cause;

  boolean                   retrx_flag; /* PDU is marked for retrx.         */
  uint16                    retrx_ctr;  /* Number of retrx attempts to date */

  geran_pdu_priority_t      pdu_priority;

} gllc_lle_retrx_buf_t;

/* ------------------------------------------------------------------------ */

/* Resequencing buffer element definition. */

typedef struct gllc_lle_reseq_buf
{
  struct gllc_lle_reseq_buf *next_p;
  struct gllc_lle_reseq_buf *prev_p;
  
  uint16                    l3_pdu_length;
  uint32                    rxd_tlli;
  gllc_i_s_frame_t          i_s_frame;    /* Received unsequenced I frame. */

} gllc_lle_reseq_buf_t;

/* ------------------------------------------------------------------------ */

/* LLE operating parameters and state information which are pertinent to the
   acknowledged mode (ABM state only) of operation. */

typedef struct
{

  boolean                   peer_rcvr_busy;
  boolean                   own_rcvr_busy;

  /* Elements associated with standalone S frame transmission. */
  uint8                     radio_priority;
  uint8                     peak_throughput;

  uint8                     pfi;
  uint8                     max_retrx_buf_frames_kU;
  uint8                     sack_bitmap_len_K;
  uint8                     max_reseq_buf_frames_kD;
  uint8                     sack_bitmap_R[ GLLC_SACK_BMP_LEN ];
  geran_pdu_priority_t      pdu_priority;

  grr_data_req_cause_t      cause;

  uint16                    retrx_buf_num_frames;
  uint16                    retrx_buf_occupancy_B;
  uint16                    max_info_octets_N201_I;
  uint16                    max_retrx_buf_octets_mU;

  /* Acknowledged operation state variables. */
  uint16                    send_state_var_V_S;
  uint16                    recv_state_var_V_R;
  uint16                    ackn_state_var_V_A;

  /* Highest and previously received I frame N(S) sequence number records. */
  uint16                    highest_rcvd_N_S;
  uint16                    previous_rcvd_N_S;
  uint16                    max_reseq_buf_octets_mD;

  /* Encipher/decipher operating parameters. */
  uint32                    encipher_oc;
  uint32                    decipher_oc;
  uint32                    input_offset_IOV_I;
  uint32                    retrx_interval_T201;
  /* I frame (re)transmission buffer and related operating parameters. */

  gllc_lle_retrx_buf_t      *retrx_first_p;
  gllc_lle_retrx_buf_t      *retrx_last_p;

  /* Receive resequencing buffer and related operating parameters. */

  gllc_lle_reseq_buf_t      *reseq_first_p;
  gllc_lle_reseq_buf_t      *reseq_last_p;

  gllc_lle_retrx_buf_t      *T201_association;
  gllc_lle_retrx_buf_t      retrx_dummy;
  /* Acknowledged operation general negotiated paramaters. */
  gllc_frame_t              frame;
} gllc_lle_ack_t;

/* ------------------------------------------------------------------------ */

/* LLE operating parameters and state information which are pertinent to the
   transmission of unnumbered command frames. */

typedef struct
{
  boolean                   disc_rcvd_in_loc_est;
  uint8                     radio_priority;
  uint8                     peak_throughput;
  uint8                     pfi;
  grr_data_req_cause_t      cause;
  uint16                    xid_pending;  /* Bitmask of xid param idents. */

  /* Elements associated with U frame transmission. */
  uint16                    retrx_ctr;

  /* Elements associated with layer 2 XID negotiation transactions. */

  gllc_xid_info_t           xid_req;
  gllc_xid_info_t           xid_neg;

  /* Element to record the receipt of a DISC command in the Loc Est State. */
  // geran_pdu_priority_t      pdu_priority;
  gllc_frame_t              cmd_frame;
  gllc_frame_t              sol_res_frame;
  gllc_frame_t              unsol_res_frame;
} gllc_lle_unnum_t;

/* ------------------------------------------------------------------------ */

#ifndef FEATURE_GERAN_REDUCED_DEBUG
/* LLE diagnostic logging statistics parameters. */

typedef struct
{
  gllc_diag_pdu_stat_rsp_t  pdu_stats;
  uint32                    reestablishment_cnt;
  uint32                    general_a_cnt;
  uint32                    general_b_cnt;
  uint32                    general_c_cnt;

} gllc_lle_diag_t;
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */


/* ------------------------------------------------------------------------ */

/* LLE Definition. */

typedef struct
{
  gllc_lle_state_t          state;
  gllc_lle_substate_t       substate;

  gllc_lle_general_t        gen;
  gllc_lle_unack_t          unack;
  gllc_lle_ack_t            ack;
  gllc_lle_unnum_t          unnum;

#ifndef FEATURE_GERAN_REDUCED_DEBUG
  gllc_lle_diag_t           diag;
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */

} gllc_lle_t;


/*****************************************************************************
***
***     Public Defines & Constants
***
*****************************************************************************/

/* ===========================================================================
    GPRS LLC Task Signal Definitions

    For use in applying to data of type rex_sigs_type.

    Avoid signal definitions 0x00008000, 0x00004000 and 0x00002000 as
    these are reserved for task mode control signals TASK_START_SIG,
    TASK_STOP_SIG and TASK_OFFLINE_SIG (see task.h).
   ======================================================================== */

#define GLLC_UL_MASTER_SIG                0x01UL
#define GLLC_DL_MASTER_SIG                0x02UL
#define GLLC_GMM_RDY_TMR_SIG              0x04UL
#define GLLC_DOG_RPT_TMR_SIG              0x08UL
#define GLLC_TEST_MODE_SIG                0x10UL
#define GLLC_DYN_RESUME_SIG               0x20UL


/*****************************************************************************
***
***     Public Data References
***
*****************************************************************************/
extern rex_crit_sect_type gllc_dl_msg_crit_sec;
extern rex_crit_sect_type gllc_cipher_crit_sec;

/* ===========================================================================
    Task Associated References
   ======================================================================== */

extern rex_sigs_type     gllc_task_sigs;
extern boolean           gllc_initialised;
extern boolean           llc_log_initialized;
extern rex_timer_type    gllc_gmm_ready_timer;

/* ---------------------------------------------------------------------------
   GMM Ready Timer restart timeout and current state references
   ------------------------------------------------------------------------ */

extern boolean gllc_gmm_ready_timer_active;
extern uint32  gllc_gmm_ready_time;

/* ===========================================================================
    Translation Table Utility References
   ======================================================================== */

/* LLE to SAPI address translator. */

extern const uint8 gllc_sapi_addr_lookup[ GLLC_LL_NUM_SAPIS ];

/* SAPI address to LLE translator. */

extern const gllc_ll_sapi_t gllc_lle_lookup[ 16 ];


/* ===========================================================================
   LL SAP Client References
   ======================================================================== */

/* Client Registration Records. */

extern gllc_ll_client_record_t gllc_ll_client_record[ GLLC_LL_NUM_SAPIS ];


/* ===========================================================================
   XID Parameter References
   ======================================================================== */

extern const uint16 gllc_dflt_N201_I_octets[ GLLC_LL_NUM_SAPIS ];
extern const uint16 gllc_dflt_N201_U_octets[ GLLC_LL_NUM_SAPIS ];
extern const uint16 gllc_max_N201_U_octets[ GLLC_LL_NUM_SAPIS ];

extern const uint32 gllc_prfd_T200_ms[ GLLC_LL_NUM_SAPIS ];
extern const uint8  gllc_prfd_N200[ GLLC_LL_NUM_SAPIS ];
extern const uint16 gllc_prfd_N201_U_octets[ GLLC_LLME_NUM_GAN_STATES ][ GLLC_LL_NUM_SAPIS ];
extern const uint16 gllc_prfd_N201_I_octets[ GLLC_LLME_NUM_GAN_STATES ][ GLLC_LL_NUM_SAPIS ];
extern const uint16 gllc_prfd_mD_octet16[ GLLC_LL_NUM_SAPIS ];
extern const uint16 gllc_prfd_mU_octet16[ GLLC_LL_NUM_SAPIS ];
extern const uint8  gllc_prfd_kD_frames[ GLLC_LL_NUM_SAPIS ];
extern const uint8  gllc_prfd_kU_frames[ GLLC_LL_NUM_SAPIS ];


/* ===========================================================================
   Up Link Event Queue References
   ======================================================================== */

/* LLME GMM SAP Messaging Queue Reference.

   For receipt of LLGMM service primitives from the GMM entity via the
   LLGMM SAP. */

extern q_type               gllc_gmm_ul_q;

/* ------------------------------------------------------------------------ */

/* LLE LL SAP Signalling Messaging Queue Array Reference

   For receipt of LL service primitives (other than LL-UNITDATA-REQ &
   LL-DATA-REQ) from layer 3 network entities via the LL SAP interfaces. */

extern q_type               gllc_ll_ul_sig_q[ GLLC_LL_NUM_SAPIS ];

/* ------------------------------------------------------------------------ */

/* LLE LL SAP Unacknowledged Data Messaging Queue Array Reference

   For receipt of LL-UNITDATA-REQ service primitives from layer 3 network
   entities via the LL SAP interfaces. */

extern dsm_watermark_type   gllc_ll_ul_unack_wm[ GLLC_LL_NUM_SAPIS ];
extern q_type               gllc_ll_ul_unack_q[ GLLC_LL_NUM_SAPIS ];

/* ------------------------------------------------------------------------ */

/* LLE LL SAP Acknowledged Data Messaging Queue Array Reference

   For receipt of LL-DATA-REQ service primitives from layer 3 network
   entities via the LL SAP interfaces. */

extern dsm_watermark_type   gllc_ll_ul_ack_wm[ GLLC_LL_NUM_SAPIS ];
extern q_type               gllc_ll_ul_ack_q[ GLLC_LL_NUM_SAPIS ];

/* ------------------------------------------------------------------------ */

/* LLE C/R Messaging Queue Array Reference

   For receipt of down link originated commands and responses from the peer
   LLE. */

extern q_type               gllc_cr_ul_q[ GLLC_LL_NUM_SAPIS ];


/* ===========================================================================
   Down Link Event Queue References
   ======================================================================== */

/* LLE GRR SAP Messaging Queue Reference

   For receipt of GRR service primitives from the RLC/MAC layer via the GRR
   SAP interface. */
extern q_type               gllc_grr_dl_q;

/* ===========================================================================
   Event Bitmap Definitions and Event/Mask Record References
   ======================================================================== */

/* ------------------------------------------------------------------------ */

/* Up Link MPEX Event Bitmap Definitions and Record Reference(s). */

#define GLLC_UL_NO_EVT             0x00
#define GLLC_UL_GMM_Q_EVT          0x01     /* gllc_gmm_ul_q occupancy >= 1 */

/* Up Link LLE Event Bitmap Definitions and Record Reference(s). */

#define GLLC_LLE_UL_NO_EVT         0x0000

#define GLLC_LLE_UL_LL_SIG_Q_EVT   0x0001   /* gllc_ll_ul_sig_q occu. >= 1  */
#define GLLC_LLE_UL_LL_ACK_Q_EVT   0x0002   /* gllc_ll_ul_ack_q occu. >= 1  */
#define GLLC_LLE_UL_LL_UNACK_Q_EVT 0x0004   /* gllc_ll_ul_unack_q occu.>= 1 */
#define GLLC_LLE_UL_CR_Q_EVT       0x0008   /* gllc_cr_ul_q occupancy >= 1  */

#define GLLC_LLE_UL_TIMER_T200_EVT 0x0010   /* timer t200 has expired       */
#define GLLC_LLE_UL_TIMER_T201_EVT 0x0020   /* timer t201 has expired       */

#define GLLC_LLE_UL_U_FR_CMD_EVT   0x0040   /* U frame cmd trx reqt exists  */
#define GLLC_LLE_UL_U_FR_SRES_EVT  0x0080   /* U fr sol res trx reqt exists */
#define GLLC_LLE_UL_U_FR_URES_EVT  0x0100   /* U fr unsol res trx req exists*/
#define GLLC_LLE_UL_UI_FR_EVT      0x0200   /* UI frame trx reqt exists     */
#define GLLC_LLE_UL_I_FR_EVT       0x0400   /* I frame trx reqt exists      */
#define GLLC_LLE_UL_I_FR_RETRX_EVT 0x0800   /* I frame retrx reqt exists    */
#define GLLC_LLE_UL_S_FR_EVT       0x1000   /* S frame trx reqt exists      */
#define GLLC_LLE_UL_S_FR_ABIT_EVT  0x2000   /* acknowledge trx reqt exists  */


#define GLLC_LLE_UL_TIMER_EVT       (GLLC_LLE_UL_TIMER_T200_EVT |            \
                                     GLLC_LLE_UL_TIMER_T201_EVT)

#define GLLC_LLE_UL_LL_DAT_Q_EVT    (GLLC_LLE_UL_LL_ACK_Q_EVT   |            \
                                     GLLC_LLE_UL_LL_UNACK_Q_EVT)

#define GLLC_LLE_UL_TRX_EVT         (GLLC_LLE_UL_U_FR_CMD_EVT   |            \
                                     GLLC_LLE_UL_U_FR_SRES_EVT  |            \
                                     GLLC_LLE_UL_U_FR_URES_EVT  |            \
                                     GLLC_LLE_UL_UI_FR_EVT      |            \
                                     GLLC_LLE_UL_I_FR_EVT       |            \
                                     GLLC_LLE_UL_I_FR_RETRX_EVT |            \
                                     GLLC_LLE_UL_S_FR_EVT       |            \
                                     GLLC_LLE_UL_S_FR_ABIT_EVT)

#define GLLC_LLE_UL_I_FR_TRX_EVT    (GLLC_LLE_UL_I_FR_EVT       |            \
                                     GLLC_LLE_UL_I_FR_RETRX_EVT)

#define GLLC_LLE_UL_S_FR_TRX_EVT    (GLLC_LLE_UL_S_FR_EVT       |            \
                                     GLLC_LLE_UL_S_FR_ABIT_EVT)

/* ------------------------------------------------------------------------ */

extern uint8                gllc_ul_event_map;
extern uint16               gllc_lle_ul_event_map[ GLLC_LL_NUM_SAPIS ];
extern uint16               gllc_lle_ul_mask_map[ GLLC_LL_NUM_SAPIS ];
extern uint16               gllc_lle_ul_event_flags;

/* References to Up Link Event Masking Scheme Records. */

extern boolean              gllc_llme_ul_evt_mask_scheme_flags[GLLC_LLME_NUM_SCHEMES];
extern boolean              gllc_lle_ul_evt_mask_scheme_flags[GLLC_LLE_NUM_SCHEMES][GLLC_LL_NUM_SAPIS];

/* ===========================================================================
   References to LLC Up Link Timer Storage

   REX extended timers for each LLE.
   ======================================================================== */

extern rex_timer_type       gllc_timer_t200[ GLLC_LL_NUM_SAPIS ];
extern rex_timer_type       gllc_timer_t201[ GLLC_LL_NUM_SAPIS ];


/* ===========================================================================
   Reference to the Logical Link Management Entity (LLME).
   ======================================================================== */

extern gllc_llme_t          gllc_llme;


/* ===========================================================================
   Reference to the Logical Link Entities (LLEs).
   ======================================================================== */

extern gllc_lle_t           gllc_lle[ GLLC_LL_NUM_SAPIS ];

extern gllc_psm_info_t      gllc_psm_info;

/* ===========================================================================
   Reference to the LLC frame assembly working storage.
   ======================================================================== */

extern gllc_frame_assy_t    gllc_assy_info;


/* ===========================================================================
   Reference to the LLC frame disassembly working storage.
   ======================================================================== */

extern gllc_frame_disassy_t gllc_disassy_info;


/*****************************************************************************
***
***     Public Macros
***
*****************************************************************************/

/* ===========================================================================
   LLC Utility Macros
   ======================================================================== */

#define GLLC_LLE_IS_3_5_9_11( lle )                                          \
        (                                                                    \
          ((lle) == GLLC_LL_SAPI_3_LL3  ) ||                                 \
          ((lle) == GLLC_LL_SAPI_5_LL5  ) ||                                 \
          ((lle) == GLLC_LL_SAPI_9_LL9  ) ||                                 \
          ((lle) == GLLC_LL_SAPI_11_LL11)                                    \
        )

/* ===========================================================================
   LLC Event Management Macros
   ======================================================================== */

/* System lock and free macros for use where scheduler could be called
   in between, such as when setting a rex signal. This avoids unwanted
   context switching, but at the same time guards against preemption. */

/* ........................................................................ */

#define GLLC_SET_UL_EVT(evt_map)                                                          \
        {                                                                                 \
          GLLC_LOCK();                                                                    \
          gllc_ul_event_map |= (evt_map);                                                 \
                                                                                          \
          if (!(gllc_task_sigs & GLLC_UL_MASTER_SIG))                                     \
          {                                                                               \
            (void)rex_set_sigs(geran_tcb_read(GERAN_TASK_ID_GLLC_1), GLLC_UL_MASTER_SIG); \
            gllc_task_sigs = rex_get_sigs(geran_tcb_read(GERAN_TASK_ID_GLLC_1));          \
          }                                                                               \
          GLLC_UNLOCK();                                                                  \
        }

/* ........................................................................ */

#define GLLC_CLR_UL_EVT(evt_map)                                                            \
        {                                                                                   \
          GLLC_LOCK();                                                                      \
          gllc_ul_event_map &= ~(evt_map);                                                  \
          if (!gllc_ul_event_map && !gllc_lle_ul_event_flags)                               \
          {                                                                                 \
            if (gllc_task_sigs & GLLC_UL_MASTER_SIG)                                        \
            {                                                                               \
              (void)rex_clr_sigs(geran_tcb_read(GERAN_TASK_ID_GLLC_1), GLLC_UL_MASTER_SIG); \
              gllc_task_sigs = rex_get_sigs(geran_tcb_read(GERAN_TASK_ID_GLLC_1));          \
            }                                                                               \
          }                                                                                 \
          GLLC_UNLOCK();                                                                    \
        }

/* ------------------------------------------------------------------------ */

#define GLLC_SET_LLE_UL_EVT(lle, evt_map)                                                   \
        {                                                                                   \
          GLLC_LOCK();                                                                      \
          gllc_lle_ul_event_map[(uint32)lle] |= (evt_map);                                  \
                                                                                            \
          if (gllc_lle_ul_event_map[(uint32)lle] &                                          \
              gllc_lle_ul_mask_map[(uint32)lle])                                            \
          {                                                                                 \
            gllc_lle_ul_event_flags |= (uint16)(1 << (uint16)lle);                          \
                                                                                            \
            if (!(gllc_task_sigs & GLLC_UL_MASTER_SIG))                                     \
            {                                                                               \
              (void)rex_set_sigs(geran_tcb_read(GERAN_TASK_ID_GLLC_1), GLLC_UL_MASTER_SIG); \
              gllc_task_sigs = rex_get_sigs(geran_tcb_read(GERAN_TASK_ID_GLLC_1));          \
            }                                                                               \
          }                                                                                 \
          GLLC_UNLOCK();                                                                    \
        }

/* ........................................................................ */

#define GLLC_CLR_LLE_UL_EVT(lle, evt_map)                                                   \
        {                                                                                   \
          GLLC_LOCK();                                                                      \
          gllc_lle_ul_event_map[(uint32)lle] &= ~(evt_map);                                 \
          if (!(gllc_lle_ul_event_map[(uint32)lle] &                                        \
                gllc_lle_ul_mask_map[(uint32)lle]))                                         \
          {                                                                                 \
            gllc_lle_ul_event_flags &= ~(uint16)(1 << (uint16)lle);                         \
          }                                                                                 \
                                                                                            \
          if (!gllc_ul_event_map && !gllc_lle_ul_event_flags)                               \
          {                                                                                 \
            if (gllc_task_sigs & GLLC_UL_MASTER_SIG)                                        \
            {                                                                               \
              (void)rex_clr_sigs(geran_tcb_read(GERAN_TASK_ID_GLLC_1), GLLC_UL_MASTER_SIG); \
              gllc_task_sigs = rex_get_sigs(geran_tcb_read(GERAN_TASK_ID_GLLC_1));          \
            }                                                                               \
          }                                                                                 \
          GLLC_UNLOCK();                                                                    \
        }

/* ------------------------------------------------------------------------ */

#define GLLC_SET_DL_EVT()                                                                   \
        {                                                                                   \
          GLLC_LOCK();                                                                      \
          if (!(gllc_task_sigs & GLLC_DL_MASTER_SIG))                                       \
          {                                                                                 \
            (void)rex_set_sigs(geran_tcb_read(GERAN_TASK_ID_GLLC_1), GLLC_DL_MASTER_SIG);   \
            gllc_task_sigs = rex_get_sigs(geran_tcb_read(GERAN_TASK_ID_GLLC_1));            \
          }                                                                                 \
          GLLC_UNLOCK();                                                                    \
        }

/* ........................................................................ */

#define GLLC_CLR_DL_EVT()                                                                   \
        {                                                                                   \
          GLLC_LOCK();                                                                      \
          if (gllc_task_sigs & GLLC_DL_MASTER_SIG)                                          \
          {                                                                                 \
            (void)rex_clr_sigs(geran_tcb_read(GERAN_TASK_ID_GLLC_1), GLLC_DL_MASTER_SIG);   \
            gllc_task_sigs = rex_get_sigs(geran_tcb_read(GERAN_TASK_ID_GLLC_1));            \
          }                                                                                 \
          GLLC_UNLOCK();                                                                    \
        }

/* ===========================================================================
   LLE Retransmission Buffer Management Macros

   NOTE - item_p argument must valid and not NULL!
   ======================================================================== */

#define GLLC_RETRX_BUF_APPEND_ITEM( lle, item_p )                            \
        {                                                                    \
          if ( gllc_lle[lle].ack.retrx_first_p == NULL )                     \
          {                                                                  \
            item_p -> next_p                = NULL;                          \
            item_p -> prev_p                = NULL;                          \
            gllc_lle[lle].ack.retrx_first_p = item_p;                        \
            gllc_lle[lle].ack.retrx_last_p  = item_p;                        \
          }                                                                  \
          else                                                               \
          {                                                                  \
            item_p -> next_p                = NULL;                          \
            item_p -> prev_p                = gllc_lle[lle].ack.retrx_last_p;\
            gllc_lle[lle].ack.retrx_last_p  = item_p;                        \
                                                                             \
            if ( item_p -> prev_p != NULL )                                  \
              item_p -> prev_p -> next_p = item_p;                           \
          }                                                                  \
                                                                             \
          gllc_lle[lle].ack.retrx_buf_occupancy_B += item_p -> l3_pdu_length;\
          gllc_lle[lle].ack.retrx_buf_num_frames++;                          \
        }

/* ........................................................................ */

#define GLLC_RETRX_BUF_REMOVE_ITEM( lle, item_p )                            \
        {                                                                    \
          if ( gllc_lle[lle].ack.retrx_first_p == item_p )                   \
          {                                                                  \
            gllc_lle[lle].ack.retrx_first_p = item_p -> next_p;              \
                                                                             \
            if ( item_p -> next_p == NULL )                                  \
              gllc_lle[lle].ack.retrx_last_p = NULL;                         \
          }                                                                  \
                                                                             \
          else if ( gllc_lle[lle].ack.retrx_last_p == item_p )               \
          {                                                                  \
            item_p -> prev_p -> next_p     = NULL;                           \
            gllc_lle[lle].ack.retrx_last_p = item_p -> prev_p;               \
          }                                                                  \
                                                                             \
          else                                                               \
          {                                                                  \
            item_p -> prev_p -> next_p = item_p -> next_p;                   \
            item_p -> next_p -> prev_p = item_p -> prev_p;                   \
          }                                                                  \
                                                                             \
          if ( gllc_lle[lle].ack.retrx_buf_num_frames > 0 )                  \
            gllc_lle[lle].ack.retrx_buf_num_frames--;                        \
                                                                             \
          if ( item_p -> l3_pdu_length <=                                    \
               gllc_lle[lle].ack.retrx_buf_occupancy_B )                     \
          {                                                                  \
            gllc_lle[lle].ack.retrx_buf_occupancy_B -=                       \
              item_p -> l3_pdu_length;                                       \
          }                                                                  \
          else                                                               \
            gllc_lle[lle].ack.retrx_buf_occupancy_B = 0;                     \
        }


/* ===========================================================================
   LLE Resequencing Buffer Management Macros

   NOTE - item_p and insert_p arguments must valid and not NULL!
   ======================================================================== */

#define GLLC_RESEQ_BUF_APPEND_ITEM( lle, item_p )                            \
        {                                                                    \
          if ( gllc_lle[lle].ack.reseq_first_p == NULL )                     \
          {                                                                  \
            item_p -> next_p                = NULL;                          \
            item_p -> prev_p                = NULL;                          \
            gllc_lle[lle].ack.reseq_first_p = item_p;                        \
            gllc_lle[lle].ack.reseq_last_p  = item_p;                        \
          }                                                                  \
          else                                                               \
          {                                                                  \
            item_p -> next_p                = NULL;                          \
            item_p -> prev_p                = gllc_lle[lle].ack.reseq_last_p;\
            gllc_lle[lle].ack.reseq_last_p  = item_p;                        \
                                                                             \
            if ( item_p -> prev_p != NULL )                                  \
              item_p -> prev_p -> next_p = item_p;                           \
          }                                                                  \
        }

/* ........................................................................ */

#define GLLC_RESEQ_BUF_INSERT_ITEM( lle, item_p, insert_p )                  \
        {                                                                    \
          if ( gllc_lle[lle].ack.reseq_first_p == insert_p )                 \
          {                                                                  \
            item_p -> next_p                = insert_p;                      \
            item_p -> prev_p                = NULL;                          \
            insert_p -> prev_p              = item_p;                        \
            gllc_lle[lle].ack.reseq_first_p = item_p;                        \
          }                                                                  \
          else                                                               \
          {                                                                  \
            item_p -> next_p                = insert_p;                      \
            item_p -> prev_p                = insert_p -> prev_p;            \
            item_p -> prev_p -> next_p      = item_p;                        \
            insert_p -> prev_p              = item_p;                        \
          }                                                                  \
        }

/* ........................................................................ */

#define GLLC_RESEQ_BUF_REMOVE_ITEM( lle, item_p )                            \
        {                                                                    \
          if ( gllc_lle[lle].ack.reseq_first_p == item_p )                   \
          {                                                                  \
            gllc_lle[lle].ack.reseq_first_p = item_p -> next_p;              \
                                                                             \
            if ( item_p -> next_p == NULL )                                  \
              gllc_lle[lle].ack.reseq_last_p = NULL;                         \
          }                                                                  \
                                                                             \
          else if ( gllc_lle[lle].ack.reseq_last_p == item_p )               \
          {                                                                  \
            item_p -> prev_p -> next_p     = NULL;                           \
            gllc_lle[lle].ack.reseq_last_p = item_p -> prev_p;               \
          }                                                                  \
                                                                             \
          else                                                               \
          {                                                                  \
            item_p -> prev_p -> next_p = item_p -> next_p;                   \
            item_p -> next_p -> prev_p = item_p -> prev_p;                   \
          }                                                                  \
        }

#ifndef FEATURE_GERAN_REDUCED_DEBUG
#define gllc_diag_rpt_llme_state()   _gllc_diag_rpt_llme_state()
#define gllc_diag_rpt_lle_state(lle) _gllc_diag_rpt_lle_state(lle)
#define gllc_diag_rpt_lle_xid(lle)   _gllc_diag_rpt_lle_xid(lle)
#else /* !FEATURE_GERAN_REDUCED_DEBUG */
#define gllc_diag_rpt_llme_state()
#define gllc_diag_rpt_lle_state(lle)
#define gllc_diag_rpt_lle_xid(lle)
#endif /* FEATURE_GERAN_REDUCED_DEBUG */

/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      GLLC_LLME_SET_UL_EVENT_MASK_SCHEME()
===
===  Internal prototype.
===  Refer to gllchdlr.c for full description.
===
===========================================================================*/

extern void gllc_llme_set_ul_event_mask_scheme
(
  gllc_llme_ul_evt_mask_scheme_t llme_ul_scheme
);


/*===========================================================================
===
===  FUNCTION      GLLC_LLME_CLR_UL_EVENT_MASK_SCHEME()
===
===  Internal prototype.
===  Refer to gllchdlr.c for full description.
===
===========================================================================*/

extern void gllc_llme_clr_ul_event_mask_scheme
(
  gllc_llme_ul_evt_mask_scheme_t llme_ul_scheme
);


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_SET_UL_EVENT_MASK_SCHEME()
===
===  Internal prototype.
===  Refer to gllchdlr.c for full description.
===
===========================================================================*/

extern void gllc_lle_set_ul_event_mask_scheme
(
  gllc_ll_sapi_t                lle,
  gllc_lle_ul_evt_mask_scheme_t lle_ul_scheme
);


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_CLR_UL_EVENT_MASK_SCHEME()
===
===  Internal prototype.
===  Refer to gllchdlr.c for full description.
===
===========================================================================*/


extern void gllc_lle_clr_ul_event_mask_scheme
(
  gllc_ll_sapi_t                lle,
  gllc_lle_ul_evt_mask_scheme_t lle_ul_scheme
);


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_CHK_UL_EVENT_MASK_SCHEME()
===
===  Internal prototype.
===  Refer to gllchdlr.c for full description.
===
===========================================================================*/

extern boolean gllc_lle_chk_ul_event_mask_scheme
(
  gllc_ll_sapi_t                lle,
  gllc_lle_ul_evt_mask_scheme_t lle_ul_scheme
);


/* *********************************************************************** */

/*===========================================================================
===
===  FUNCTION      GLLC_LL_DL_SIG_GET_MES_BUF()
===
===  Internal prototype.
===  Refer to gllcifce.c for full description.
===
===========================================================================*/

extern gprs_ll_dl_sig_mes_t *gllc_ll_dl_sig_get_mes_buf
(
  gllc_ll_sapi_t ll_sapi
);


/*===========================================================================
===
===  FUNCTION      GLLC_LL_DL_SIG_PUT_MES_BUF()
===
===  Internal prototype.
===  Refer to gllcifce.c for full description.
===
===========================================================================*/

extern void gllc_ll_dl_sig_put_mes_buf
(
  gllc_ll_sapi_t        ll_sapi,
  gprs_ll_dl_sig_mes_t  *mes_ptr
);


/*===========================================================================
===
===  FUNCTION      GLLC_LL_DL_DAT_PUT_MES_BUF()
===
===  Internal prototype.
===  Refer to gllcifce.c for full description.
===
===========================================================================*/

extern void gllc_ll_dl_dat_put_mes_buf
(
  gllc_ll_sapi_t        ll_sapi,
  gprs_ll_dl_dat_mes_t  *mes_ptr
);


#ifndef FEATURE_GERAN_REDUCED_DEBUG
/*===========================================================================
===
===  FUNCTION      GLLC_DIAG_RPT_LLME_STATE()
===
===  Internal prototype.
===  Refer to gllcifce.c for full description.
===
===========================================================================*/

extern void _gllc_diag_rpt_llme_state( void );


/*===========================================================================
===
===  FUNCTION      GLLC_DIAG_RPT_LLE_STATE()
===
===  Internal prototype.
===  Refer to gllcifce.c for full description.
===
===========================================================================*/

extern void _gllc_diag_rpt_lle_state( gllc_ll_sapi_t lle );


/*===========================================================================
===
===  FUNCTION      GLLC_DIAG_RPT_LLE_XID()
===
===  Internal prototype.
===  Refer to gllcifce.c for full description.
===
===========================================================================*/

extern void _gllc_diag_rpt_lle_xid( gllc_ll_sapi_t lle );
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */


/* *********************************************************************** */

/*===========================================================================
===
===  FUNCTION      GLLC_READ_PSM_INFO()
===
===  Internal prototype.
===  Refer to gllcstate.c for full description.
===
===========================================================================*/

extern boolean gllc_read_psm_info( void );

/*===========================================================================
===
===  FUNCTION      GLLC_LLE_FLUSH_LL_UL_SIG_Q()
===
===  Internal prototype.
===  Refer to gllchldr.c for full description.
===
===========================================================================*/

extern void gllc_lle_flush_ll_ul_sig_q( gllc_ll_sapi_t ll_sapi );


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_FLUSH_LL_UL_ACK_Q()
===
===  Internal prototype.
===  Refer to gllchldr.c for full description.
===
===========================================================================*/

extern void gllc_lle_flush_ll_ul_ack_q( gllc_ll_sapi_t ll_sapi );


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_FLUSH_LL_UL_UNACK_Q()
===
===  Internal prototype.
===  Refer to gllchldr.c for full description.
===
===========================================================================*/

extern void gllc_lle_flush_ll_ul_unack_q( gllc_ll_sapi_t ll_sapi );


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_FLUSH_CR_UL_Q()
===
===  Internal prototype.
===  Refer to gllchldr.c for full description.
===
===========================================================================*/

extern void gllc_lle_flush_cr_ul_q( gllc_ll_sapi_t lle );


/*===========================================================================
===
===  FUNCTION      GLLC_GMM_READY_TIMER_HANDLER()
===
===  Internal prototype.
===  Refer to gllchldr.c for full description.
===
===========================================================================*/

extern void gllc_gmm_ready_timer_handler( void );


/*===========================================================================
===
===  FUNCTION      GLLC_TIMER_CALLBACK_1()
===
===  Internal prototype.
===  Refer to gllchldr.c for full description.
===
===========================================================================*/

extern void gllc_timer_callback_1( uint32 lle_and_timer_evt );

/*===========================================================================
===
===  FUNCTION      GLLC_TEST_MODE_HANDLER()
===
===  Internal prototype.
===  Refer to gllchldr.c for full description.
===
===========================================================================*/

extern void gllc_test_mode_handler( void );


/*===========================================================================
===
===  FUNCTION      GLLC_EVENT_HANDLER()
===
===  Internal prototype.
===  Refer to gllchldr.c for full description.
===
===========================================================================*/

extern void gllc_event_handler( void );


/* *********************************************************************** */

/*===========================================================================
===
===  FUNCTION      GLLC_ASS_FRAME_HEADER_FCS
===
===  Internal prototype.
===  Refer to gllccode.c for full description.
===
===========================================================================*/

extern void gllc_ass_frame_header_fcs
(

  uint8    *hdr_p,
  uint16   hdr_len
);


/*===========================================================================
===
===  FUNCTION      GLLC_ASS_FRAME_INFO_FCS
===
===  Internal prototype.
===  Refer to gllccode.c for full description.
===
===========================================================================*/

extern void gllc_ass_frame_info_fcs
(
  dsm_item_type **pdu_p,
  uint16        fcs_len
);


/*===========================================================================
===
===  FUNCTION      GLLC_ASS_FRAME_INFO_FCS_AND_ENCIPHER
===
===  Internal prototype.
===  Refer to gllccode.c for full description.
===
===========================================================================*/

extern void gllc_ass_frame_info_fcs_and_encipher
(
  dsm_item_type *src_pdu_p,
  dsm_item_type **des_pdu_p,
  uint16        frame_info_len,
  uint16        fcs_len,
  uint32        gea_input_seed
);


/*===========================================================================
===
===  FUNCTION      GLLC_DIS_FRAME_HEADER_FCS
===
===  Internal prototype.
===  Refer to gllccode.c for full description.
===
===========================================================================*/

extern void gllc_dis_frame_header_fcs
(
  uint8    *hdr_p,
  uint16   hdr_len
);


/*===========================================================================
===
===  FUNCTION      GLLC_DIS_FRAME_INFO_FCS
===
===  Internal prototype.
===  Refer to gllccode.c for full description.
===
===========================================================================*/

extern boolean gllc_dis_frame_info_fcs
(
  dsm_item_type **pdu_p,
  uint16        fcs_len
);


/*===========================================================================
===
===  FUNCTION      GLLC_DIS_FRAME_INFO_FCS_AND_DECIPHER
===
===  Internal prototype.
===  Refer to gllccode.c for full description.
===
===========================================================================*/

extern boolean gllc_dis_frame_info_fcs_and_decipher
(
  dsm_item_type **pdu_p,
  uint16        frame_info_len,
  uint16        fcs_len,
  uint32        gea_input_seed
);


/* *********************************************************************** */

/*===========================================================================
===
===  FUNCTION      GLLC_ASSEMBLE_FRAME_HDR()
===
===  Internal prototype.
===  Refer to gllcfrme.c for full description.
===
===========================================================================*/

extern void gllc_assemble_frame_hdr( void );


/*===========================================================================
===
===  FUNCTION      GLLC_ASSEMBLE_FRAME_INFO_AND_FCS()
===
===  Internal prototype.
===  Refer to gllcfrme.c for full description.
===
===========================================================================*/

extern void gllc_assemble_frame_info_and_fcs( void );


/*===========================================================================
===
===  FUNCTION      GLLC_DISASSEMBLE_FRAME_HDR()
===
===  Internal prototype.
===  Refer to gllcfrme.c for full description.
===
===========================================================================*/

extern void gllc_disassemble_frame_hdr( void );


/*===========================================================================
===
===  FUNCTION      GLLC_DISASSEMBLE_FRAME_INFO_AND_FCS()
===
===  Internal prototype.
===  Refer to gllcfrme.c for full description.
===
===========================================================================*/

extern void gllc_disassemble_frame_info_and_fcs( void );


/* *********************************************************************** */

/*===========================================================================
===
===  FUNCTION      GLLC_LLME_RESET()
===
===  Internal prototype.
===  Refer to gllcstate.c for full description.
===
===========================================================================*/

extern void gllc_llme_reset( void );


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_RESET()
===
===  Internal prototype.
===  Refer to gllcstate.c for full description.
===
===========================================================================*/

extern void gllc_lle_reset( gllc_ll_sapi_t lle, gllc_lle_state_t reset_state );


/*===========================================================================
===
===  FUNCTION      GLLC_LLME_PROCESS_ASSIGN_REQ_MES()
===
===  Internal prototype.
===  Refer to gllcstate.c for full description.
===
===========================================================================*/

extern void gllc_llme_process_assign_req_mes
(
  
  gmmllc_llgmm_assign_req_type *assign_req_mes
);


/*===========================================================================
===
===  FUNCTION      GLLC_LLME_PROCESS_TRIGGER_REQ_MES()
===
===  Internal prototype.
===  Refer to gllcstate.c for full description.
===
===========================================================================*/

extern void gllc_llme_process_trigger_req_mes
(
  
  gmmllc_llgmm_trigger_req_type *trigger_req_mes
);


/*===========================================================================
===
===  FUNCTION      GLLC_LLME_PROCESS_SUSPEND_REQ_MES()
===
===  Internal prototype.
===  Refer to gllcstate.c for full description.
===
===========================================================================*/

extern void gllc_llme_process_suspend_req_mes
(
 
  gmmllc_llgmm_suspend_req_type *suspend_req_mes
);


/*===========================================================================
===
===  FUNCTION      GLLC_LLME_PROCESS_RESUME_REQ_MES()
===
===  Internal prototype.
===  Refer to gllcstate.c for full description.
===
===========================================================================*/

extern void gllc_llme_process_resume_req_mes
(
 
  gmmllc_llgmm_resume_req_type *resume_req_mes
);


/*===========================================================================
===
===  FUNCTION      GLLC_LLME_PROCESS_STRT_RDY_TMR_REQ_MES()
===
===  Internal prototype.
===  Refer to gllcstate.c for full description.
===
===========================================================================*/

extern void gllc_llme_process_strt_rdy_tmr_req_mes
(
  gmmllc_llgmm_strt_rdy_tmr_req_type *strt_rdy_tmr_req_mes
);


/*===========================================================================
===
===  FUNCTION      GLLC_LLME_PROCESS_STOP_RDY_TMR_REQ_MES()
===
===  Internal prototype.
===  Refer to gllcstate.c for full description.
===
===========================================================================*/

extern void gllc_llme_process_stop_rdy_tmr_req_mes
(
  gmmllc_llgmm_stop_rdy_tmr_req_type *stop_rdy_tmr_req_mes
);


/*===========================================================================
===
===  FUNCTION      GLLC_LLME_PROCESS_TEST_MODE_REQ_MES()
===
===  Internal prototype.
===  Refer to gllcstate.c for full description.
===
===========================================================================*/

extern void gllc_llme_process_test_mode_req_mes
(
  gmmllc_llgmm_test_mode_req_type *test_mode_req_mes
);


/*===========================================================================
===
===  FUNCTION      GLLC_LLME_PROCESS_GRR_STATUS_IND_MES()
===
===  Internal prototype.
===  Refer to gllcstate.c for full description.
===
===========================================================================*/

extern void gllc_llme_process_grr_status_ind_mes
(
  gprs_grr_dl_mes_t *status_ind_mes
);




/*===========================================================================
===
===  FUNCTION      GLLC_LLME_PROCESS_GRR_RESUME_CNF_MES()
===
===  Internal prototype.
===  Refer to gllcstate.c for full description.
===
===========================================================================*/

extern void gllc_llme_process_grr_resume_cnf_mes ( void );


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_PROCESS_EST_REQ_MES()
===
===  Internal prototype.
===  Refer to gllcstate.c for full description.
===
===========================================================================*/

extern void gllc_lle_process_est_req_mes
(
  gllc_ll_sapi_t      lle,
  ll_establish_req_t  *est_req_mes
);


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_PROCESS_EST_RES_MES()
===
===  Internal prototype.
===  Refer to gllcstate.c for full description.
===
===========================================================================*/

extern void gllc_lle_process_est_res_mes
(
  gllc_ll_sapi_t      lle,
  ll_establish_res_t  *est_res_mes
);


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_PROCESS_REL_REQ_MES()
===
===  Internal prototype.
===  Refer to gllcstate.c for full description.
===
===========================================================================*/

extern void gllc_lle_process_rel_req_mes
(
  gllc_ll_sapi_t      lle,
  ll_release_req_t    *rel_req_mes
);


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_PROCESS_ACT_IND_MES()
===
===  Internal prototype.
===  Refer to gllcstate.c for full description.
===
===========================================================================*/

extern void gllc_lle_process_act_ind_mes
(
  gllc_ll_sapi_t      lle,
  ll_activate_ind_t   *act_ind_mes
);


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_PROCESS_DEACT_IND_MES()
===
===  Internal prototype.
===  Refer to gllcstate.c for full description.
===
===========================================================================*/

extern void gllc_lle_process_deact_ind_mes
(
  gllc_ll_sapi_t      lle,
  ll_deactivate_ind_t *deact_ind_mes
);


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_PROCESS_XID_REQ_MES()
===
===  Internal prototype.
===  Refer to gllcstate.c for full description.
===
===========================================================================*/

extern void gllc_lle_process_xid_req_mes
(
  gllc_ll_sapi_t      lle,
  ll_xid_req_t        *xid_req_mes
);


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_PROCESS_XID_RES_MES()
===
===  Internal prototype.
===  Refer to gllcstate.c for full description.
===
===========================================================================*/

extern void gllc_lle_process_xid_res_mes
(
  gllc_ll_sapi_t      lle,
  ll_xid_res_t        *xid_res_mes
);


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_PROCESS_RESET_RES_MES()
===
===  Internal prototype.
===  Refer to gllcstate.c for full description.
===
===========================================================================*/

extern void gllc_lle_process_reset_res_mes
(
  gllc_ll_sapi_t      lle
);


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_PROCESS_ENTER_PSM_IND_MES()
===
===  Internal prototype.
===  Refer to gllcstate.c for full description.
===
===========================================================================*/

extern void gllc_lle_process_enter_psm_ind_mes
(
  ll_enter_psm_ind_t  *enter_psm_ind
);


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_PROCESS_INACT_SAPI_RES_MES()
===
===  Internal prototype.
===  Refer to gllcstate.c for full description.
===
===========================================================================*/

extern void gllc_lle_process_inact_sapi_res_mes
(
  gllc_ll_sapi_t      lle
);


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_PROCESS_DATA_REQ_MES()
===
===  Internal prototype.
===  Refer to gllcstate.c for full description.
===
===========================================================================*/

extern void gllc_lle_process_data_req_mes
(
  gllc_ll_sapi_t      lle,
  ll_data_req_t       *data_req_mes,
  uint32              pending_data_req_cnt
);


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_PROCESS_UNITDATA_REQ_MES()
===
===  Internal prototype.
===  Refer to gllcstate.c for full description.
===
===========================================================================*/

extern void gllc_lle_process_unitdata_req_mes
(
  gllc_ll_sapi_t      lle,
  ll_unitdata_req_t   *unitdata_req_mes
);


#ifdef FEATURE_GPRS_PS_HANDOVER
/*===========================================================================
===
===  FUNCTION      GLLC_LLME_PROCESS_PSHO_REQ_MES()
===
===  Internal prototype.
===  Refer to gllcstate.c for full description.
===
===========================================================================*/

extern void gllc_llme_process_psho_req_mes
(
  gmmllc_llgmm_psho_req_type *psho_req_mes
);

/*===========================================================================
===
===  FUNCTION      GLLC_UL_XID_CMD_PSHO_PUT_MES()
===
===  Internal prototype.
===  Refer to gllchdlr.c for full description.
===
===========================================================================*/

extern void gllc_ul_xid_cmd_psho_put_mes( uint32 iov_ui );

#endif /* FEATURE_GPRS_PS_HANDOVER */

/*===========================================================================
===
===  FUNCTION      GLLC_LLE_PROCESS_I_S_FRAME()
===
===  Internal prototype.
===  Refer to gllcstate.c for full description.
===
===========================================================================*/

extern void gllc_lle_process_i_s_frame
(

  gllc_ll_sapi_t      lle,
  gllc_i_s_frame_t    *i_s_frame,
  uint16              l3_pdu_length,
  uint32              rxd_tlli
);


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_PROCESS_S_FRAME()
===
===  Internal prototype.
===  Refer to gllcstate.c for full description.
===
===========================================================================*/

extern void gllc_lle_process_s_frame
(
  gllc_ll_sapi_t      lle,
  gllc_s_frame_t      *s_frame,
  uint32              rxd_tlli
);


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_PROCESS_U_FRAME()
===
===  Internal prototype.
===  Refer to gllcstate.c for full description.
===
===========================================================================*/

extern void gllc_lle_process_u_frame
(
  gllc_ll_sapi_t      lle,
  gllc_u_frame_t      *u_frame,
  uint32              rxd_tlli
);


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_REPORT_FRMR_CONTENT()
===
===  Internal prototype.
===  Refer to gllcstate.c for full description.
===
===========================================================================*/

extern void gllc_lle_report_frmr_content
(
  gllc_ll_sapi_t   lle,
  gllc_frmr_info_t *frmr_info_p
);


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_PROCESS_REJ_FRAME()
===
===  Internal prototype.
===  Refer to gllcstate.c for full description.
===
===========================================================================*/

extern void gllc_lle_process_rej_frame
(
  gllc_ll_sapi_t      lle,
  gllc_rej_frame_t    *rej_frame,
  uint32              rxd_tlli
);


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_PROCESS_UI_FRAME()
===
===  Internal prototype.
===  Refer to gllcstate.c for full description.
===
===========================================================================*/

extern void gllc_lle_process_ui_frame
(
  gllc_ll_sapi_t      lle,
  gllc_ui_frame_t     *ui_frame,
  uint16              l3_pdu_length,
  uint32              rxd_tlli
);


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_PROCESS_TIMER_T200_EXPIRY()
===
===  Internal prototype.
===  Refer to gllcstate.c for full description.
===
===========================================================================*/

extern void gllc_lle_process_timer_t200_expiry( gllc_ll_sapi_t lle );


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_PROCESS_TIMER_T201_EXPIRY()
===
===  Internal prototype.
===  Refer to gllcstate.c for full description.
===
===========================================================================*/

extern void gllc_lle_process_timer_t201_expiry( gllc_ll_sapi_t lle );


/* *********************************************************************** */

/*===========================================================================
===
===  FUNCTION      GLLC_LLME_RESET_L2_XID_PARAMETERS()
===
===  Internal prototype.
===  Refer to gllcxid.c for full description.
===
===========================================================================*/

extern void gllc_llme_reset_l2_xid_parameters( void );


/*===========================================================================
===
===  FUNCTION      GLLC_LLME_RESET_IOV_XID_PARAMETERS()
===
===  Internal prototype.
===  Refer to gllcxid.c for full description.
===
===========================================================================*/

extern void gllc_llme_reset_IOV_xid_parameters( void );


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_RESET_L2_XID_PARAMETERS()
===
===  Internal prototype.
===  Refer to gllcxid.c for full description.
===
===========================================================================*/

extern void gllc_lle_reset_l2_xid_parameters( gllc_ll_sapi_t lle );


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_VALIDATE_U_FORMAT_CR_XID_REQ()
===
===  Internal prototype.
===  Refer to gllcxid.c for full description.
===
===========================================================================*/

extern boolean gllc_lle_validate_u_format_cr_xid_req
(
  gllc_ll_sapi_t lle,
  gllc_u_frame_t *u_frame
);


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_VALIDATE_U_FORMAT_CR_XID_NEG()
===
===  Internal prototype.
===  Refer to gllcxid.c for full description.
===
===========================================================================*/

extern boolean gllc_lle_validate_u_format_cr_xid_neg
(
  gllc_ll_sapi_t lle,
  gllc_u_frame_t *u_frame
);


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_STORE_U_FORMAT_CR_XID_REQ()
===
===  Internal prototype.
===  Refer to gllcxid.c for full description.
===
===========================================================================*/

extern void gllc_lle_store_u_format_cr_xid_req
(
  gllc_ll_sapi_t lle,
  gllc_u_frame_t *u_frame
);


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_STORE_U_FORMAT_CR_XID_NEG()
===
===  Internal prototype.
===  Refer to gllcxid.c for full description.
===
===========================================================================*/

extern void gllc_lle_store_u_format_cr_xid_neg
(
  gllc_ll_sapi_t lle,
  gllc_u_frame_t *u_frame
);


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_GENERATE_L2_XID_REQ()
===
===  Internal prototype.
===  Refer to gllcxid.c for full description.
===
===========================================================================*/

extern boolean gllc_lle_generate_l2_xid_req( gllc_ll_sapi_t lle );


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_GENERATE_L2_XID_NEG()
===
===  Internal prototype.
===  Refer to gllcxid.c for full description.
===
===========================================================================*/

extern void gllc_lle_generate_l2_xid_neg( gllc_ll_sapi_t lle );


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_SET_L2_XID_PARAMETERS()
===
===  Internal prototype.
===  Refer to gllcxid.c for full description.
===
===========================================================================*/

extern void gllc_lle_set_l2_xid_parameters
(
  gllc_ll_sapi_t lle,
  gllc_u_frame_t *u_frame
);


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_REPORT_L2_XID_CONTENT()
===
===  Internal prototype.
===  Refer to gllcxid.c for full description.
===
===========================================================================*/

extern void gllc_lle_report_l2_xid_content
(
  gllc_ll_sapi_t  lle,
  gllc_xid_info_t *xid_info_p
);


/* *********************************************************************** */

/*===========================================================================
===
===  FUNCTION      GLLC_LLE_SEND_LLGMM_STATUS_IND_MES()
===
===  Internal prototype.
===  Refer to gllcmsg.c for full description.
===
===========================================================================*/

extern void gllc_lle_send_llgmm_status_ind_mes
(
  gllc_ll_sapi_t              lle,
  uint32                      tlli,
  gllcmm_status_cause_e_type  cause
);


/*===========================================================================
===
===  FUNCTION      GLLC_LLME_SEND_LLGMM_STATUS_IND_MES()
===
===  Internal prototype.
===  Refer to gllcmsg.c for full description.
===
===========================================================================*/

extern void gllc_llme_send_llgmm_status_ind_mes
(
  uint8                   protocol_disc,
  uint8                   transaction_id,
  gprs_grr_status_cause_t tx_status
);


/*===========================================================================
===
===  FUNCTION      GLLC_LLME_SEND_LLGMM_TRIGGER_IND_MES()
===
===  Internal prototype.
===  Refer to gllcmsg.c for full description.
===
===========================================================================*/

extern void gllc_llme_send_llgmm_trigger_ind_mes( void );


/*===========================================================================
===
===  FUNCTION      GLLC_LLME_SEND_LLGMM_RDY_TMR_IND_MES()
===
===  Internal prototype.
===  Refer to gllcmsg.c for full description.
===
===========================================================================*/

extern void gllc_llme_send_llgmm_rdy_tmr_ind_mes( void );


/*===========================================================================
===
===  FUNCTION      GLLC_LLME_SEND_LLGMM_SUSPEND_CNF_MES()
===
===  Internal prototype.
===  Refer to gllcmsg.c for full description.
===
===========================================================================*/

extern void gllc_llme_send_llgmm_suspend_cnf_mes( void );


/*===========================================================================
===
===  FUNCTION      GLLC_LLME_SEND_LLGMM_NEW_TLLI_IND_MES()
===
===  Internal prototype.
===  Refer to gllcmsg.c for full description.
===
===========================================================================*/

extern void gllc_llme_send_llgmm_new_tlli_ind_mes( uint32 new_tlli );


#ifdef FEATURE_GPRS_PS_HANDOVER

/*===========================================================================
===
===  FUNCTION      GLLC_LLME_PROCESS_PSHO_CNF_MES()
===
===  Internal prototype.
===  Refer to gllcmsg.c for full description.
===
===========================================================================*/

extern void gllc_llme_send_llgmm_psho_cnf_mes( void );

#endif /* FEATURE_GPRS_PS_HANDOVER */

/* *********************************************************************** */

/*===========================================================================
===
===  FUNCTION      GLLC_LLE_SEND_LL_RST_IND_MES()
===
===  Internal prototype.
===  Refer to gllcmsg.c for full description.
===
===========================================================================*/

extern void gllc_lle_send_ll_rst_ind_mes
( 
  gllc_ll_sapi_t lle, 
  uint32 tlli
);


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_SEND_LL_EST_IND_MES()
===
===  Internal prototype.
===  Refer to gllcmsg.c for full description.
===
===========================================================================*/

extern void gllc_lle_send_ll_est_ind_mes
(
  gllc_ll_sapi_t lle,
  gllc_u_frame_t *u_frame,
  uint32         tlli
);


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_SEND_LL_EST_CNF_MES()
===
===  Internal prototype.
===  Refer to gllcmsg.c for full description.
===
===========================================================================*/

extern void gllc_lle_send_ll_est_cnf_mes
(
  gllc_ll_sapi_t lle,
  gllc_u_frame_t *u_frame,
  uint32         tlli
);


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_SEND_LL_REL_IND_MES()
===
===  Internal prototype.
===  Refer to gllcmsg.c for full description.
===
===========================================================================*/

extern void gllc_lle_send_ll_rel_ind_mes
(
  gllc_ll_sapi_t lle,
  uint32         tlli,
  ll_cause_t     cause
);


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_SEND_LL_REL_CNF_MES()
===
===  Internal prototype.
===  Refer to gllcmsg.c for full description.
===
===========================================================================*/

extern void gllc_lle_send_ll_rel_cnf_mes( gllc_ll_sapi_t lle, uint32 tlli );


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_SEND_LL_XID_IND_MES()
===
===  Internal prototype.
===  Refer to gllcmsg.c for full description.
===
===========================================================================*/

extern void gllc_lle_send_ll_xid_ind_mes
(
  gllc_ll_sapi_t    lle,
  gllc_u_frame_t    *u_frame,
  uint32            tlli
);


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_SEND_LL_XID_CNF_MES()
===
===  Internal prototype.
===  Refer to gllcmsg.c for full description.
===
===========================================================================*/

extern void gllc_lle_send_ll_xid_cnf_mes
(
  gllc_ll_sapi_t    lle,
  gllc_u_frame_t    *u_frame,
  uint32            tlli
);


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_CLR_EST_SUSP()
===
===  DESCRIPTION
===
===   Collates the necessary information to compose the required layer 3
===   message and posts this message to the layer 3 entity associated with
===   the given LLE.
===
===   Message information is collated from state information relating to
===   the LLE, and the given parameters.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

extern void gllc_lle_clr_est_susp ( gllc_ll_sapi_t lle );


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_SEND_LL_DATA_IND_MES()
===
===  Internal prototype.
===  Refer to gllcmsg.c for full description.
===
===========================================================================*/

extern void gllc_lle_send_ll_data_ind_mes
(
  gllc_ll_sapi_t    lle,
  gllc_i_s_frame_t  *i_s_frame,
  uint32            tlli
);


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_SEND_LL_DATA_CNF_MES()
===
===  Internal prototype.
===  Refer to gllcmsg.c for full description.
===
===========================================================================*/

extern void gllc_lle_send_ll_data_cnf_mes
(
  gllc_ll_sapi_t lle,
  ll_sndcp_ref_t *sndcp_ref
);


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_SEND_LL_UNITDATA_IND_MES()
===
===  Internal prototype.
===  Refer to gllcmsg.c for full description.
===
===========================================================================*/

extern void gllc_lle_send_ll_unitdata_ind_mes
(
  gllc_ll_sapi_t  lle,
  gllc_ui_frame_t *ui_frame,
  uint32          tlli
);


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_SEND_LL_UNITDATA_CNF_MES()
===
===  DESCRIPTION
===
===   Collates the necessary information to compose the required layer 3
===   message and posts this message to the layer 3 entity associated with
===   the given LLE.
===
===   Message information is collated from state information relating to
===   the LLE, and the given parameters.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

extern void gllc_lle_send_ll_unitdata_cnf_mes
(
  gllc_ll_sapi_t       lle,
  ll_sndcp_ref_t       *sndcp_ref
);


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_SEND_LL_STATUS_IND_MES()
===
===  Internal prototype.
===  Refer to gllcmsg.c for full description.
===
===========================================================================*/

extern void gllc_lle_send_ll_status_ind_mes
(
  gllc_ll_sapi_t lle,
  uint32         tlli,
  ll_cause_t     cause
);


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_SEND_SABM_CMD_MES()
===
===  Internal prototype.
===  Refer to gllcmsg.c for full description.
===
===========================================================================*/

extern void gllc_lle_send_sabm_cmd_mes
(
  gllc_ll_sapi_t lle,
  dsm_item_type  **l3_xid_req
);


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_RESEND_SABM_CMD_MES()
===
===  Internal prototype.
===  Refer to gllcmsg.c for full description.
===
===========================================================================*/

extern boolean gllc_lle_resend_sabm_cmd_mes( gllc_ll_sapi_t lle );


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_SEND_DISC_CMD_MES()
===
===  Internal prototype.
===  Refer to gllcmsg.c for full description.
===
===========================================================================*/

extern void gllc_lle_send_disc_cmd_mes( gllc_ll_sapi_t lle );


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_RESEND_DISC_CMD_MES()
===
===  Internal prototype.
===  Refer to gllcmsg.c for full description.
===
===========================================================================*/

boolean gllc_lle_resend_disc_cmd_mes( gllc_ll_sapi_t lle );


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_SEND_XID_CMD_MES()
===
===  Internal prototype.
===  Refer to gllcmsg.c for full description.
===
===========================================================================*/

extern void gllc_lle_send_xid_cmd_mes
(
  gllc_ll_sapi_t lle,
  dsm_item_type  **l3_xid_req
);


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_RESEND_XID_CMD_MES()
===
===  Internal prototype.
===  Refer to gllcmsg.c for full description.
===
===========================================================================*/

extern boolean gllc_lle_resend_xid_cmd_mes( gllc_ll_sapi_t lle );


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_SEND_NULL_CMD_MES()
===
===  Internal prototype.
===  Refer to gllcmsg.c for full description.
===
===========================================================================*/

extern void gllc_lle_send_null_cmd_mes( gllc_ll_sapi_t lle );


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_SEND_UA_RES_MES()
===
===  Internal prototype.
===  Refer to gllcmsg.c for full description.
===
===========================================================================*/

extern void gllc_lle_send_ua_res_mes
(
  gllc_ll_sapi_t lle,
  dsm_item_type  **l3_xid_neg
);


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_SEND_DM_RES_MES()
===
===  Internal prototype.
===  Refer to gllcmsg.c for full description.
===
===========================================================================*/

extern void gllc_lle_send_dm_res_mes
(
  gllc_ll_sapi_t  lle,
  boolean         poll_final_PF
);


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_SEND_XID_RES_MES()
===
===  Internal prototype.
===  Refer to gllcmsg.c for full description.
===
===========================================================================*/

extern void gllc_lle_send_xid_res_mes
(
  gllc_ll_sapi_t lle,
  dsm_item_type  **l3_xid_neg
);

/*===========================================================================
===
===  FUNCTION      GLLC_LLE_SEND_INTERNAL_XID_RES_MES()
===
===  Internal prototype.
===  Refer to gllcmsg.c for full description.
===
===========================================================================*/
extern void gllc_lle_internal_u_frame_rsp
(
  gllc_ll_sapi_t    lle,
  gllc_unnumbered_t cmd
);

/*===========================================================================
===
===  FUNCTION      GLLC_LLE_SEND_FRMR_RES_MES()
===
===  Internal prototype.
===  Refer to gllcmsg.c for full description.
===
===========================================================================*/

extern void gllc_lle_send_frmr_res_mes
(
  gllc_ll_sapi_t   lle,
  gllc_rej_frame_t *rej_frame
);


/*===========================================================================
===
===  FUNCTION      GLLC_LLME_SEND_GRR_SUSPEND_REQ_MES()
===
===  Internal prototype.
===  Refer to gllcmsg.c for full description.
===
===========================================================================*/

extern void gllc_llme_send_grr_suspend_req_mes( void );


/*===========================================================================
===
===  FUNCTION      GLLC_LLME_SEND_GRR_RESUME_REQ_MES()
===
===  Internal prototype.
===  Refer to gllcmsg.c for full description.
===
===========================================================================*/

extern void gllc_llme_send_grr_resume_req_mes( void );


/*===========================================================================
===
===  FUNCTION      GLLC_LLME_SEND_GRR_PURGE_SUSPENDED_DATA_REQ_MES()
===
===  Internal prototype.
===  Refer to gllcmsg.c for full description.
===
===========================================================================*/

extern void gllc_llme_send_grr_purge_suspended_data_req_mes( void );


/*===========================================================================
===
===  FUNCTION      GLLC_LLME_SEND_GRR_PURGE_DATA_REQ_MES()
===
===  Internal prototype.
===  Refer to gllcmsg.c for full description.
===
===========================================================================*/

extern void gllc_llme_send_grr_purge_data_req_mes( gllc_ll_sapi_t lle );


/* *********************************************************************** */

/*===========================================================================
===
===  FUNCTION      GLLC_DSM_EVENT_IND_1()
===
===  Internal prototype.
===  Refer to gllcifce.c for full description.
===
===========================================================================*/

extern void gllc_dsm_event_ind_1
(
  dsm_mempool_id_enum_type  mempool_id,
  dsm_mem_level_enum_type   mem_level,
  dsm_mem_op_enum_type      mem_op
);

/* *********************************************************************** */

/* #define to enable Frame and Keystream Content Printing */
#undef DEBUG_GSM_GPRS_GLLC_FRAME

#ifdef DEBUG_GSM_GPRS_GLLC_FRAME
#error code not present
#endif /* DEBUG_GSM_GPRS_GLLC_FRAME */

/*===========================================================================
===
===  FUNCTION      GLLC_DEBUG_PRINT_LLE_CIPHER_PARAMS()
===
===  Internal prototype.
===  Refer to gllchdlr.c for full description.
===
===========================================================================*/

void gllc_debug_print_lle_cipher_params
(
  gllc_ll_sapi_t lle
);


/*===========================================================================
===
===  FUNCTION      GLLC_DEBUG_PRINT_LLME_CIPHER_PARAMS()
===
===  Internal prototype.
===  Refer to gllchdlr.c for full description.
===
===========================================================================*/

void gllc_debug_print_llme_cipher_params( void );

/*===========================================================================
===
===  FUNCTION      GLLC_UL_UNNUM_CR_PUT_U_FRAME()
===
===  Internal prototype.
===  Refer to gllchdlr.c for full description.
===
===========================================================================*/

extern void gllc_ul_unnum_cr_put_u_frame
(
  gllc_ll_sapi_t lle,
  uint32         tlli,
  gllc_u_frame_t frm
);

/* *********************************************************************** */

/* Dynamically loaded APIs */
extern void geran_gllc_init();
extern void geran_gllc_deinit();
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
extern void geran_gllc_resume();
extern void geran_gllc_suspend();
extern void gllc_process_dyn_pendq(q_type *q_ptr);
#endif

#endif /* INC_GLLCI_H  */

/*** EOF: don't remove! ***/
