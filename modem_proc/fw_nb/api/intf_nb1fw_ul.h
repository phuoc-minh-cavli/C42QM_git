/*!
  @file
  intf_nb1fw_ul.h

  @brief
  FW-SW interface UL message definitions for NB1 FW

  @detail
  FW-SW interface UL message definitions for NB1 FW

*/

/*===========================================================================

  Copyright (c) 2016 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //source/qcom/qct/modem/fw/components/fw_nb.mpss/rel/15.2/nb1/api/intf_nb1fw_ul.h#2 $
$DateTime: 2023/04/27 07:48:28 $
$Author: aakaniti $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/23/17   HS      Add PUSCH abort msg request 
12/19/16   HS      API definition for NPRACH avoidance gap 
12/15/16   HS      HDET support to RFSW 
08/22/16   HS      Update enum definition for TA operation; 
                   remove duplicate tone_idx API from PUSCH data Tx request struct.
08/17/16   HS      Fix PRACH confirmation message header definition. 
08/16/16   AM      Added SRS subframe info. 
 
===========================================================================*/

#ifndef INTF_NB1FW_UL_H
#define INTF_NB1FW_UL_H

#include "intf_nb1fw_common.h"

/*===========================================================================

      Constants

===========================================================================*/


/*===========================================================================

      Macros

===========================================================================*/


/* Number of maximum RACH hop offsets for RACH occurences */
#define NB1FW_UL_RACH_NUM_HOP_OFFSETS 128

/* HDET timer period in 1.92 MHz units */
#define NB1FW_UL_HDET_TIMER_PERIOD_1920_KHZ  (1920*20) //20 ms

/* NPRACH avoidance gap bit map sizes possible */
#define NB1FW_UL_PRACH_COLLISION_INFO_BMAP_SIZE_1   (2560)
#define NB1FW_UL_PRACH_COLLISION_INFO_BMAP_SIZE_2   (7680)

/*===========================================================================

      Typedefs

===========================================================================*/

/* Timing advance operation type */
typedef enum
{
  NB1FW_UL_TA_SLAM,        /* Indicate absolute value of TA to slam */
  NB1FW_UL_TA_DELTA_UPDATE /* Delta update to existing TA slammed value */
} nb1fw_ul_ta_op_type_e;

/* DMRS info types */
typedef enum
{
  NB1FW_UL_DMRS_3_TONE,  /* 3 tone DMRS info */
  NB1FW_UL_DMRS_6_TONE,  /* 6 tone DMRS info */
  NB1FW_UL_DMRS_12_TONE, /* 12 tone DMRS info */
  NB1FW_UL_NUM_DMRS_INFO
} nb1fw_ul_dmrs_info_type_e;

/* Tone bandwidth type of UL transmission */
typedef enum
{
  NB1FW_UL_SINGLE_TONE_3_75_KHZ,
  NB1FW_UL_SINGLE_TONE_15_KHZ,
  NB1FW_UL_MULTI_TONE_3,
  NB1FW_UL_MULTI_TONE_6,
  NB1FW_UL_MULTI_TONE_12
} nb1fw_ul_tone_bw_e;

/* ACK or NACK enumartion */
typedef enum
{
  NB1FW_UL_NACK,
  NB1FW_UL_ACK
} nb1fw_ul_ack_nack_e;

/* RACH CP length types */
typedef enum
{
  NB1FW_UL_RACH_CP_SHORT, /* CP length 66.67 micro sec */
  NB1FW_UL_RACH_CP_LONG   /* CP length 266.7 micro sec */
} nb1fw_ul_rach_cp_e;

/* HARQ ID types */
typedef enum
{
  NB1FW_UL_HARQ_ID_0, /* harq id 0 */
  NB1FW_UL_HARQ_ID_1,   /* harq id 1 */
  NB1FW_UL_HARQ_ID_BOTH  
} nb1fw_ul_harq_id_e;

/* Timing advance info struct */
typedef struct
{
  int16 val;                /* TA upate value to be applied as per op */
  nb1fw_ul_ta_op_type_e op; /* OTA operation type, absolute slam or delta adjust */
} nb1fw_ul_ta_s;

/* SRS collision avoidance info struct */
typedef struct
{
  /* Mask of SRS subframes. LSB (bit 0) = sfn 0, MSB (bit 9) = sfn 9. If mask is 0, then there is no puncturing/SRS. */
  uint16 mask     : 10;
  uint16 reserved :  6;
} nb1fw_ul_srs_subframe_config_s;

/* UL DMRS generation info structure for multitone*/
typedef struct
{
  uint8 base_seq_idx;  /* Base sequence index (u) */
  uint8 cyclc_shift;   /* Cyclic shift. Only for 3, 6 tones */
} nb1fw_ul_dmrs_multitone_info_s;

/* UL DMRS generation info structure */
typedef struct
{
  uint8   delta_ss;    /* groupAssignment parameter */
  nb1fw_ul_dmrs_multitone_info_s multitone[NB1FW_UL_NUM_DMRS_INFO]; /* multitone info */
} nb1fw_ul_dmrs_info_s;

/* PUSCH semi static configuration request */
typedef struct
{
  nb1fw_req_hdr_s req_hdr; /* message header */

  boolean srs_valid;                        /* indicate SRS info valid */
  nb1fw_ul_srs_subframe_config_s srs_info;  /* SRS collision avoidance info */

  boolean tone_spacing_info_valid;          /* tone spacing info valid */
  boolean is_3p75k;                         /* flag to indicate 3.75k hz tone spacing or not */  

  nb1fw_ul_dmrs_info_s dmrs_info;           /* DMRS generation info */
} nb1fw_ul_pusch_config_req_s;

NB1FW_INTF_MSG_S(ul_pusch_config_req);

/* PUSCH data encode request */
typedef struct
{
  nb1fw_req_hdr_s    req_hdr;   /* message header */

  uint8*             payload;   /* UL data payload pointer. Must be contigous data payload */
  uint16             tb_size;   /* TB size in bits */
  nb1fw_ul_tone_bw_e tone_bw;   /* Tone BW type.  */
  uint8              num_ru;    /* Number of RU */
  uint8              itbs;      /* Block size and modulation indicator */
  nb1fw_ul_harq_id_e harq_id;   /* UL harq id*/
} nb1fw_ul_pusch_encode_req_s;

NB1FW_INTF_MSG_S(ul_pusch_encode_req);

/* PRACH avoidance gap info */
typedef struct
{
  uint32* bmsk;             /* Pointer to 2560 or 7680 (special) masked bits of collision subframes with time moving 
                               in direction starting from first word LSB (SFN=0, subFn=0).   
                               If mask bit is 0, then there is no collision. */
  boolean if_7680_ms_case;  /* special case for NPRACH 240 ms periodicity config */
  boolean valid;            /* Global flag if any collision exists for 2560/7680 ms duration */

} nb1fw_ul_prach_collision_info_s;

/* PUSCH data Tx request */
typedef struct
{
  nb1fw_req_hdr_s req_hdr; /* message header */

  uint32 earfcn;                                         /* UL EARFCN */
  uint32 rflm_handle;                                    /* RFLM handle for UL RF ops */
  uint32 scramb_seed_mask;                               /* scrambling code seed mask with RNTI and cell Id */
  nb1fw_ul_prach_collision_info_s prach_collision_info;  /* PRACH avoidance gap info */
  uint8 repetition;                                      /* Repetition units */
  nb1fw_ul_tone_bw_e tone_bw;                            /* Tone BW type.  */
  uint8 rv_idx;                                          /* Starting RV index for Tx */
  uint8 start_tone_idx;                                  /* Starting tone for RU */
  uint8 num_ru;                                          /* Number of RU */
  uint8 itbs;                                            /* Block size and modulation indicator */
  int8 tx_pow_dbm;                                       /* Tx power in dBm */
  boolean hopping_en;                                    /* dmrs group hopping enabled flag */
  nb1fw_ul_harq_id_e  harq_id;                           /* UL harq id*/
} nb1fw_ul_pusch_schedule_data_req_s;

NB1FW_INTF_MSG_S(ul_pusch_schedule_data_req);

/* PUSCH ACK/NACK Tx request */
typedef struct
{
  nb1fw_req_hdr_s req_hdr; /* message header */

  uint32 earfcn;                                         /* UL EARFCN */
  uint32 rflm_handle;                                    /* RFLM handle for UL RF ops */
  uint32 scramb_seed_mask;                               /* scrambling code seed mask with RNTI and cell Id */
  nb1fw_ul_prach_collision_info_s prach_collision_info;  /* PRACH avoidance gap info */
  uint8 repetition;                                      /* Repetition units */
  nb1fw_ul_tone_bw_e tone_bw;                            /* Tone BW type. Only single tone are allowed in ACK/NACK */
  uint8 tone_idx;                                        /* 0..47 for 3.75 KHz tone, 0..11 for 15 KHz tone */
  nb1fw_ul_ack_nack_e ack_nack;                          /* ACK or NACK */
  int8 tx_pow_dbm;                                       /* Tx power in dBm */
  nb1fw_ul_harq_id_e  harq_id;                           /* DL harq id for ACK NAK */
} nb1fw_ul_pusch_schedule_ack_nack_req_s;

NB1FW_INTF_MSG_S(ul_pusch_schedule_ack_nack_req);

/* PRACH Tx request */
typedef struct
{
  nb1fw_req_hdr_s req_hdr; /* message header */

  uint32 earfcn;                 /* UL EARFCN */
  uint32 rflm_handle;            /* RFLM handle for UL RF ops */
  uint8 repetition;              /* Repetition units */
  nb1fw_ul_rach_cp_e cp_type;    /* RACH CP type */
  uint8 start_tone_idx;          /* Starting tone for RU */
  uint8 hop_offset[NB1FW_UL_RACH_NUM_HOP_OFFSETS]; /* RACH occurence hop offsets relative to start tone */
  int8 tx_pow_dbm;              /* Tx power in dBm */
} nb1fw_ul_prach_schedule_req_s;

NB1FW_INTF_MSG_S(ul_prach_schedule_req);

/* PUSCH TA update request */
typedef struct
{
  nb1fw_req_hdr_s req_hdr; /* message header */

  nb1fw_ul_ta_s ta_info; /* Timing advance info */
} nb1fw_ul_pusch_ta_update_req_s;

NB1FW_INTF_MSG_S(ul_pusch_ta_update_req);

/* PUSCH deconfig request */
typedef struct
{
  nb1fw_req_hdr_s req_hdr; /* message header */
} nb1fw_ul_pusch_deconfig_req_s;

NB1FW_INTF_MSG_S(ul_pusch_deconfig_req);

/* PUSCH abort request */
typedef struct
{
  nb1fw_req_hdr_s     req_hdr; /* message header */
  nb1fw_ul_harq_id_e  harq_id; /* UL harq id*/
} nb1fw_ul_pusch_abort_req_s;

NB1FW_INTF_MSG_S(ul_pusch_abort_req);

/* PRACH abort request */
typedef struct
{
  nb1fw_req_hdr_s req_hdr; /* message header */
} nb1fw_ul_prach_abort_req_s;

NB1FW_INTF_MSG_S(ul_prach_abort_req);

/* PUSCH semi static configuration confirm */
typedef struct
{
  nb1fw_cnf_hdr_s cnf_hdr; /* message header */
} nb1fw_ul_pusch_config_cnf_s;

NB1FW_INTF_MSG_S(ul_pusch_config_cnf);

/* PUSCH data encode confirm */
typedef struct
{
  nb1fw_cnf_hdr_s cnf_hdr; /* message header */
} nb1fw_ul_pusch_encode_cnf_s;

NB1FW_INTF_MSG_S(ul_pusch_encode_cnf);

/* PUSCH data Tx confirm */
typedef struct
{
  nb1fw_cnf_hdr_s cnf_hdr; /* message header */
} nb1fw_ul_pusch_schedule_data_cnf_s;

NB1FW_INTF_MSG_S(ul_pusch_schedule_data_cnf);

/* PUSCH ACK/NACK Tx confirm */
typedef struct
{
  nb1fw_cnf_hdr_s cnf_hdr; /* message header */
} nb1fw_ul_pusch_schedule_ack_nack_cnf_s;

NB1FW_INTF_MSG_S(ul_pusch_schedule_ack_nack_cnf);

/* PRACH Tx done info */
typedef struct
{
  nb1fw_cnf_hdr_s cnf_hdr; /* message header */

  boolean aborted;   /* indicated if RACH was aborted before completition */
  uint8 repetition;  /* Repetition units executed for RACH.
                        Can be less than schduled if aborted */
} nb1fw_ul_prach_done_s;

/* PRACH Tx confirm */
typedef nb1fw_ul_prach_done_s nb1fw_ul_prach_schedule_cnf_s;
NB1FW_INTF_MSG_S(ul_prach_schedule_cnf);

/* PUSCH TA update confirm */
typedef struct
{
  nb1fw_cnf_hdr_s cnf_hdr; /* message header */

  int16 updated_ta_val; /* indicat the new updated TA value */
} nb1fw_ul_pusch_ta_update_cnf_s;

NB1FW_INTF_MSG_S(ul_pusch_ta_update_cnf);

/* PUSCH deconfig confirm */
typedef struct
{
  nb1fw_cnf_hdr_s cnf_hdr; /* message header */
} nb1fw_ul_pusch_deconfig_cnf_s;

NB1FW_INTF_MSG_S(ul_pusch_deconfig_cnf);

/* PUSCH abort confirm */
typedef struct
{
  nb1fw_cnf_hdr_s cnf_hdr; /* message header */
  nb1fw_ul_harq_id_e harq_id; /* HARQ ID*/
} nb1fw_ul_pusch_abort_cnf_s;

NB1FW_INTF_MSG_S(ul_pusch_abort_cnf);

/* PRACH abort confirm */
typedef nb1fw_ul_prach_done_s nb1fw_ul_prach_abort_cnf_s;
NB1FW_INTF_MSG_S(ul_prach_abort_cnf);

/* UL frequency error update info */
typedef struct
{
  nb1fw_ind_hdr_s ind_hdr; /* message header */

  int32 ferr_ppm_q10;  /* DL frequency error in ppm in Q10 */
} nb1fw_ul_freq_error_s;

/* UL frequency error update indication */
typedef nb1fw_ul_freq_error_s nb1fw_ul_freq_error_update_ind_s;
NB1FW_INTF_MSG_S(ul_freq_error_update_ind);

/* channel end indication. For PRACH and PUSCH only
   End time is in the header time */
typedef enum
{
  NB1FW_UL_CHAN_END_IND_PRACH,
  NB1FW_UL_CHAN_END_IND_PUSCH
} nb1fw_ul_chan_end_ind_e;

typedef struct
{
  nb1fw_ind_hdr_s ind_hdr;      /* message header */
  nb1fw_ul_chan_end_ind_e chan; /* type of channel */
  nb1fw_ul_harq_id_e harq_id;   /* HARQ ID */  
} nb1fw_ul_chan_end_ind_s;

NB1FW_INTF_MSG_S(ul_chan_end_ind);

typedef struct
{
  nb1fw_ind_hdr_s ind_hdr; /* message header */

  uint32 xoActTime;        /* XO action time to next subframe boundary */
} nb1fw_ul_hdet_s;

/* HDET update indication to RFSW after HDET timer expiry */
/* only needed for PUSCH; for PRACH, subsequent RACH attempts would automatically provide updated power */
typedef nb1fw_ul_hdet_s nb1fw_ul_hdet_update_ind_s;
NB1FW_INTF_MSG_S(ul_hdet_update_ind);

/* Union of all UL req messages */
typedef union
{
  msgr_hdr_struct_type hdr;
  NB1FW_INTF_MSG(ul_pusch_config_req);
  NB1FW_INTF_MSG(ul_pusch_encode_req);
  NB1FW_INTF_MSG(ul_pusch_schedule_data_req);
  NB1FW_INTF_MSG(ul_pusch_schedule_ack_nack_req);
  NB1FW_INTF_MSG(ul_prach_schedule_req);
  NB1FW_INTF_MSG(ul_pusch_ta_update_req);
  NB1FW_INTF_MSG(ul_pusch_deconfig_req);
  NB1FW_INTF_MSG(ul_pusch_abort_req);
  NB1FW_INTF_MSG(ul_prach_abort_req);
} nb1fw_ul_msg_req_u;

/* Union of all UL cnf messages */
typedef union
{
  msgr_hdr_struct_type hdr;
  NB1FW_INTF_MSG(ul_pusch_config_cnf);
  NB1FW_INTF_MSG(ul_pusch_encode_cnf);
  NB1FW_INTF_MSG(ul_pusch_schedule_data_cnf);
  NB1FW_INTF_MSG(ul_pusch_schedule_ack_nack_cnf);
  NB1FW_INTF_MSG(ul_prach_schedule_cnf);
  NB1FW_INTF_MSG(ul_pusch_ta_update_cnf);
  NB1FW_INTF_MSG(ul_pusch_deconfig_cnf);
  NB1FW_INTF_MSG(ul_prach_abort_cnf);
} nb1fw_ul_msg_cnf_u;

/* Union of all UL ind messages */
typedef union
{
  msgr_hdr_struct_type hdr;
  NB1FW_INTF_MSG(ul_freq_error_update_ind);
  NB1FW_INTF_MSG(ul_chan_end_ind);
} nb1fw_ul_msg_ind_u;
#endif /* INTF_NB1FW_UL_H */
