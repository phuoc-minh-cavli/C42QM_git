/*!
  @file
  intf_nb1fw_dl.h

  @brief
  FW-SW interface DL message definitions for NB1 FW

  @detail
  FW-SW interface DL message definitions for NB1 FW

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

$Header: //source/qcom/qct/modem/fw/components/fw_nb.mpss/rel/15.2/nb1/api/intf_nb1fw_dl.h#3 $
$DateTime: 2023/04/27 08:05:05 $
$Author: quickvu $

when       who     what, where, why
--------   ---     ----------------------------------------------------------

===========================================================================*/

#ifndef INTF_NB1FW_DL_H
#define INTF_NB1FW_DL_H

#include "intf_nb1fw_common.h"
#include "intf_nb1fw_sys.h"

/*===========================================================================

      Constants

===========================================================================*/


/*===========================================================================

      Macros

===========================================================================*/

/* Number of SIBs in NB1. TBD - Need confirmation and update */
#define NB1FW_DL_NUM_SI 8

/* Number of meas cells for RSRP reports. TBD - Need confirmation and update */
#define NB1FW_DL_NUM_MEAS_CELLS 8

/* Define the IM Doppler CNT for the Doppler control logic*/
#define NB1FW_DL_L1_DOPPLER_IM_CNF 0xFFFF

/*===========================================================================

      Typedefs

===========================================================================*/

/* DL PBCH decode request */
typedef struct
{
  nb1fw_req_hdr_s req_hdr; /* message header */

  boolean acq_mode; /* PBCH ACQ mode to do timing and CRC mask hypothesis */
} nb1fw_dl_pbch_decode_req_s;

NB1FW_INTF_MSG_S(dl_pbch_decode_req);

/* DL PBCH decode stop request */
typedef struct
{
  nb1fw_req_hdr_s req_hdr; /* message header */
} nb1fw_dl_pbch_decode_stop_req_s;

NB1FW_INTF_MSG_S(dl_pbch_decode_stop_req);

/* DL SIB info */
/* More information abour SIBs
   - Num RUs per TB is always 8
   - SIB one has determinstic scheduling 
     * period = 256, win_size = 256
     * rep_interval depends on number of repetitions 
     * start_offset depends on pCellId and repeitions
     * valid_sf_mask corresponds to SF4 only */
typedef struct
{
  uint16 tb_size;      /* TB size of SIB block in num_ru */
  uint16 period;       /* In radio frame units. 256 .. 16384 */
  uint16 win_size;     /* In radio frame units. 16 .. 160 */
  uint8  num_ru;       /* Number of RUs in one TB of SIB. 2 or 8 */
  uint8  rep_interval; /* SIB repeats with in window every n radio frame. 2, 4, 8, 16 */
  uint8  soft_buf_idx; /* Soft buffer to be used by this SI info to soft combine across rep and period 0..2 and also 0xFF */
  uint8  num_prd_comb; /* number of period to combine 0..255. 0 imples no combining across periods */
  uint16 start_offset; /* Start offset from period in radio frame units. 0 .. 16384 - 1 */
  uint16 valid_sf_mask[4]; /* Valid sub frame mask as per downlinkBitmap (excluding PSS, SSS, PBCH and SIB1) */
  uint32 sib_id_mask;  /* SIB ID mask, used to distinguish SIB1 vs others to apply LTE control region. Bit 0 is SIB 1 */
} nb1fw_dl_si_info_s;

/* DL SIB info update req */
typedef struct
{
  nb1fw_req_hdr_s req_hdr; /* message header */

  uint16 update_mask; /* SIB info update mask */
  nb1fw_dl_si_info_s si_info[NB1FW_DL_NUM_SI]; /* SIB info payload */
} nb1fw_dl_si_update_info_req_s;

NB1FW_INTF_MSG_S(dl_si_update_info_req);

/* DL SIB start collection req. Must have update info request before */
typedef struct
{
  nb1fw_req_hdr_s req_hdr; /* message header */

  /* Important: Mask and array index 0 point to SIB 1 */
  uint16 mask; /* SIB info mask */
  uint16 clear_soft_buf_mask; /* clear the soft buffer */
} nb1fw_dl_si_start_collect_req_s;

NB1FW_INTF_MSG_S(dl_si_start_collect_req);

/* DL SIB stop collection req */
typedef struct
{
  nb1fw_req_hdr_s req_hdr; /* message header */

  /* Important: Mask and array index 0 point to SIB 1 */
  uint16 mask; /* SIB info mask */
} nb1fw_dl_si_stop_collect_req_s;

NB1FW_INTF_MSG_S(dl_si_stop_collect_req);


#define NB1FW_DL_RNTI_TYPE_C_RNTI  (1)
#define NB1FW_DL_RNTI_TYPE_RA_RNTI (3)
#define NB1FW_DL_RNTI_TYPE_TC_RNTI (4)
/* DL PDCCH CRC mask based RNTI change req */
typedef struct
{
  nb1fw_req_hdr_s req_hdr; /* message header */
  boolean rnti_change_flag;       /* indicate if rnti is changed or not. 1 changed, 0 not changed*/
                                  /* will ignore rntis in this cmd if this flag is set to 0 */
  uint16 crc_rnti_mask;           /* CRC mask based on RNTI */
  uint16 rnti_type;               /* RNTI type - to be looped back to SW in data ind */

  boolean rnti2_valid;            /* indicate if RNTI 2 info is valid */
  uint16 crc_rnti2_mask;          /* CRC mask based on second RNTI */
  uint16 rnti2_type;              /* Second RNTI type - to be looped back to SW in data ind */
  boolean iRandomize_change_flag; /* indicate if interference randomiztion is changed or not in this cmd*/
                                  /* 1 changed, 0 not changed; will ignore iRandomize_enable in this cmd if set to 0*/
  boolean iRandomize_enable;      /* interference randomization for NPDCCH/NPDSCH is enabled or not */
} nb1fw_dl_pdcch_rnti_change_req_s;

NB1FW_INTF_MSG_S(dl_pdcch_rnti_change_req);

/* DL PDCCH deconfig or disable req */
typedef struct
{
  nb1fw_req_hdr_s req_hdr; /* message header */
  boolean disable_pdsch; /* FW will disable PDSCH ASAP if it is active */
  boolean clear_hd_ul;   /* FW will clear HL UL back to DL if there is no UL channel active */
} nb1fw_dl_pdcch_deconfig_req_s;

NB1FW_INTF_MSG_S(dl_pdcch_deconfig_req);

typedef struct
{
  uint16     pcell_id;   /* Pcell Id */
  uint16     sfn_offset; /* SFN (systems frame number) offset from ref time (0..1023) or (0..7) in case not known */
  uint16     pos_offset; /* Sub radio frame position (0..19200-1) */
  int16      init_ferr_ppm_q10;  /* initial frequency error in Q10 ppm units */
  nb1fw_cell_op_mode_e   op_mode;  /* cell operating mode */
  nb1fw_cell_crs_info_s  crs_info; /* LTE CRS info for NB cell in IN-BAND op mode */
  uint32     snrForMeas;      /* SSS snr or CE SNR to decide on NRSRP measurement params */
  boolean    snrValid;   /* snr value shared is valid for deciding meas param */
  uint32     mib33To6;   /* mib of this cell */
  uint8      hsfn;       /*HSFN */
  uint8      num_tx_ant;   /* Rsport number for this cell. for pbch based meas use only */
} nb1fw_dl_meas_cell_s;

typedef enum
{
  NB1FW_DL_MEAS_NRS_BASED,      /*run nrs based measurement */
  NB1FW_DL_MEAS_PBCH_BASED,     /*run pbch based measurement  */
  NB1FW_DL_MEAS_INVALID_METHOD 
} nb1fw_dl_meas_method_e; 

/* DL cell meas req */
typedef struct
{
  /* Common params */
  nb1fw_req_hdr_s req_hdr; /* message header */
  uint32          rflm_handle;        /* RFLM handle to use to tuning to channel */
  uint32          dl_carrier_freq;    /* DL carrier freq in MHz 32Q8. Used for freq error PPM to Hz conversion */
  uint32          inv_dl_freq;        /* 2^33/DL_F in Q19 units. Used by XO utils to calculate XO related RXLM params */
  int32           nrsrpThresdBQ7;     /* NRSRP Threshold to stop further meas - In dB,Q7 */
  uint32          dl_earfcn;          /* DL EARFCN value */
  nb1fw_cell_tl_mode_e ttl_mode;      /* TTL mode */
  nb1fw_cell_tl_mode_e ftl_mode;      /* FTL mode */
  uint16     nbrs_valid_mask[4];      /* NB RS (reference signal) valid mask for 4 frame at (SFN mod 4) */
  uint8 num_cells;                    /* number of meas cell */
  nb1fw_dl_meas_method_e    measMethod; /* NB1FW_DL_MEAS_NRS_BASED or NB1FW_DL_MEAS_PBCH_BASED*/
  nb1fw_dl_meas_cell_s meas_cells[NB1FW_DL_NUM_MEAS_CELLS]; /* cells to meas */
} nb1fw_dl_meas_cell_req_s;

NB1FW_INTF_MSG_S(dl_meas_cell_req);

/* DL cell meas abort req */
typedef struct
{
  nb1fw_req_hdr_s req_hdr; /* message header */
} nb1fw_dl_meas_cell_abort_req_s;

NB1FW_INTF_MSG_S(dl_meas_cell_abort_req);


/* DL DRX status update req */
typedef struct
{
  nb1fw_req_hdr_s req_hdr; /* message header */
  boolean dopplerReportEnable;  /* this indicate the Doppler reprot ind is need */
  uint32  dopplerMeasurementPeriod; /* this indicate the measurement period for the doppler estimation */
  boolean enableRxSignalOff;  /* FW will update the signalPath shutDown Per command */
  boolean enableRfPathFullOff; /* FW will update the full Rf path shutdown Per command */
} nb1fw_dl_drx_status_update_req_s;

NB1FW_INTF_MSG_S(dl_drx_status_update_req);

/* DL PBCH decode confirm */
typedef struct
{
  nb1fw_cnf_hdr_s cnf_hdr; /* message header */
} nb1fw_dl_pbch_decode_cnf_s;

NB1FW_INTF_MSG_S(dl_pbch_decode_cnf);

/* DL PBCH decode stop confirm */
typedef struct
{
  nb1fw_cnf_hdr_s cnf_hdr; /* message header */
} nb1fw_dl_pbch_decode_stop_cnf_s;

NB1FW_INTF_MSG_S(dl_pbch_decode_stop_cnf);

/* DL SIB info update confirm */
typedef struct
{
  nb1fw_cnf_hdr_s cnf_hdr; /* message header */
} nb1fw_dl_si_update_info_cnf_s;

NB1FW_INTF_MSG_S(dl_si_update_info_cnf);

/* DL SIB start collection confirm */
typedef struct
{
  nb1fw_cnf_hdr_s cnf_hdr; /* message header */

  uint16 mask; /* current active SI collect mask */
} nb1fw_dl_si_start_collect_cnf_s;

NB1FW_INTF_MSG_S(dl_si_start_collect_cnf);

/* DL SIB stop collection confirm */
typedef struct
{
  nb1fw_cnf_hdr_s cnf_hdr; /* message header */

  uint16 mask; /* current active SI collect mask */
} nb1fw_dl_si_stop_collect_cnf_s;

NB1FW_INTF_MSG_S(dl_si_stop_collect_cnf);

/* DL PDCCH config confirm */
typedef struct
{
  nb1fw_cnf_hdr_s cnf_hdr; /* message header */
} nb1fw_dl_pdcch_config_cnf_s;

NB1FW_INTF_MSG_S(dl_pdcch_config_cnf);

/* DL PDCCH CRC mask based RNTI change confirm */
typedef struct
{
  nb1fw_cnf_hdr_s cnf_hdr; /* message header */
} nb1fw_dl_pdcch_rnti_change_cnf_s;

NB1FW_INTF_MSG_S(dl_pdcch_rnti_change_cnf);

/* DL PDCCH deconfig or disable confirm */
typedef struct
{
  nb1fw_cnf_hdr_s cnf_hdr; /* message header */
} nb1fw_dl_pdcch_deconfig_cnf_s;

NB1FW_INTF_MSG_S(dl_pdcch_deconfig_cnf);
typedef enum
{
  NB1FW_DL_MEAS_PBCH_INVALID,      /*mib detection failed */
  NB1FW_DL_MEAS_PBCH_VALID,        /*mib detection successed  */
  NB1FW_DL_MEAS_DO_NOT_KNOW,       /*do not finish mib detection */
} nb1fw_dl_meas_pbch_valid_e; 

typedef struct
{
  uint16 pcell_id;           /* Pcell Id */
  uint16 sfn_offset;         /* SFN (systems frame number) offset from ref time (0..1023) */
  uint16 pos_offset;         /* Sub radio frame position (0..19200-1) */
  int32  rssi;              /* cell RSSI. dB, Q0 */
  int32  rsrp;              /* cell RSRP. dB, Q7 */
  int32  rsrq;              /* cell RSRQ. dB, Q7 */
  int32  total_ferr_ppm_q10; /* total frequency error as seen by cell */
  int32  snr_linear;          /* cell SNR   Q7 linear */
  nb1fw_dl_meas_pbch_valid_e mibValidFlag;      /* if mib has changed set this flag to TRUE */
} nb1fw_dl_meas_cell_info_s;

typedef struct
{
  uint8 num_cells; /* number of cells measured */
  nb1fw_dl_meas_cell_info_s cell_info[NB1FW_DL_NUM_MEAS_CELLS]; /* meas cells info */
} nb1fw_dl_meas_cell_result_s;

/* DL cell meas confirm */
typedef struct
{
  nb1fw_cnf_hdr_s cnf_hdr; /* message header */
  nb1fw_dl_meas_cell_result_s meas_results;
} nb1fw_dl_meas_cell_cnf_s;

NB1FW_INTF_MSG_S(dl_meas_cell_cnf);

/* DL cell meas abort confirm */
typedef struct
{
  nb1fw_cnf_hdr_s cnf_hdr; /* message header */
  nb1fw_dl_meas_cell_result_s meas_results;
} nb1fw_dl_meas_cell_abort_cnf_s;

NB1FW_INTF_MSG_S(dl_meas_cell_abort_cnf);


/* DL DRX status update confirm */
typedef struct
{
  nb1fw_cnf_hdr_s cnf_hdr; /* message header */
} nb1fw_dl_drx_status_update_cnf_s;

NB1FW_INTF_MSG_S(dl_drx_status_update_cnf);

/* Type of decode payloads */
typedef enum {
  NB1FW_DL_RLM_IND,     /* RLM info update */
  NB1FW_DL_DOPPLER_INFO /* Doppler info update */
} nb1fw_dl_rlm_ind_type_e;

typedef struct
{
  nb1fw_dl_rlm_ind_type_e rlm_ind_type; /* pindicate for the RLM type */
  boolean nrsDetected;     /* boolean for the NRS detection status report */
  uint16 numSlotsAccumed;  /* Period in slots waited before sampling for SNR
                              ..for debug only*/
  uint16 numSlotsInPeriod; /* Number of slots in a period .. for debug only
                            */
  int16 alphaNc;           /* Non-coherent R1 iir Alpha ..For debug only */
  uint16 dopplerHz16Q4;     /* doppler value in HZ in Q4 format */
  int32 SNREstimate; /* SNR estimate after completion of numSlotsInPeriod */
} nb1fw_dl_rlm_stat_s;


/* Type of decode payloads */
typedef enum
{
  NB1FW_DL_DECODE_PBCH_MIB, /* MIB contsnts from PBCH */
  NB1FW_DL_DECODE_SIB,      /* SIB from PDSCH */
  NB1FW_DL_DECODE_DCI,      /* DCI from PDCCH */
  NB1FW_DL_DECODE_PAGE,     /* Page decode from PDSCH */
  NB1FW_DL_DECODE_NON_PAGE, /* Non-Page data from PDSCH using CSS */
  NB1FW_DL_DECODE_DATA,     /* Data from PDSCH using USS */
  NB1FW_DL_DECODE_PAGE_MISSED,     /* Page decode missed on PDCCH search window */
  NB1FW_DL_DECODE_PAGE_FAIL_PDCCH, /* Page decode failed on PDCCH max repetition */
  NB1FW_DL_DECODE_PAGE_FAIL_PDSCH,  /* Page decode failed on PDSCH max repetition */
  NB1FW_DL_DECODE_DROP
} nb1fw_dl_decode_type_e;

typedef struct
{
  uint32 earfcn;    /* DL EARFCN for cell */
  uint16 cell_id;   /* NB1 cell Id */
  uint16 rnti_type; /* RNTI type - received in PDCCH cfg */
} nb1fw_dl_payload_metadata_s;

/* Payload indication indication */
typedef struct
{
  nb1fw_ind_hdr_s ind_hdr; /* message header */
  nb1fw_dl_payload_metadata_s metadata; /* DL payload matadata */
  nb1fw_dl_decode_type_e type; /* type of payload */
  boolean autoAck;             /* Applicable for PDSCH data only, indicate decode skipped and auto-acked previous one */
  uint8 harqId;                /* harqId of the decoded PDSCH data */
  uint16 sfCntr;               /* number of the sf counter */
  uint8* payload;              /* payload pointer */
  uint16 numBytes;             /* Size of payload TBsize quantized up to byte */
} nb1fw_dl_payload_ind_s;

/* PBCH MIB payload indication */
typedef struct
{
  nb1fw_ind_hdr_s ind_hdr; /* message header */
  nb1fw_dl_decode_type_e type; /* type of payload */
  uint8 numAnt;                /* indicate number of antennas */
  uint16 sfn;                  /* cell SFN corresponding to current SFN in header timestamp based onpasing hypothesis */
  uint8* payload;              /* payload pointer */
  int32  snr;                /* cell SNR . dB10 Q7 */
  int32  snr_linear;          /* cell SNR   Q7 linear */
} nb1fw_dl_pbch_mib_ind_s;

NB1FW_INTF_MSG_S(dl_pbch_mib_ind);

/* SIB payload indication */
typedef nb1fw_dl_payload_ind_s nb1fw_dl_sib_ind_s;
NB1FW_INTF_MSG_S(dl_sib_ind);

/* PAGE failed of missed indication */
typedef nb1fw_dl_payload_ind_s nb1fw_dl_page_failed_ind_s;
NB1FW_INTF_MSG_S(dl_page_failed_ind);

/* PDCCH DCI payload indication */
typedef struct
{
  nb1fw_ind_hdr_s ind_hdr;     /* message header */
  nb1fw_dl_decode_type_e type; /* type of payload */
  uint16 rnti_type;            /* RNTI type - received in PDCCH cfg */
  uint32 rep;                  /* PRNTI decode rep number */
  uint8* payload;              /* payload pointer */
} nb1fw_dl_pdcch_dci_ind_s;
NB1FW_INTF_MSG_S(dl_pdcch_dci_ind);

/* PDSCH (CSS page/non-page, USS data) payload indication */
typedef nb1fw_dl_payload_ind_s nb1fw_dl_data_ind_s;
NB1FW_INTF_MSG_S(dl_data_ind);

/* decoder stats */
typedef struct
{
  boolean crc_pass;  /* CRC pass fail indication */
  uint16 egy_metric; /* decode energy */
  uint16 sym_err;    /* decode symbol errors */
} nb1fw_dl_decode_stat_info_s;

/* Decoder stats indication */
typedef struct
{
  nb1fw_ind_hdr_s ind_hdr; /* message header */

  nb1fw_dl_decode_type_e type; /* type of payload */
  uint16 frame_num;    /* Radio frame where decode succeeded or repetition finished (0..1023) */
  uint16 subframe_num; /* Sub frame number where decode succeeded or repetition finished (0..9) */
  uint16 rep_accum;    /* Number of repetition accumulated */

  uint16 block_size;  /* size of block in bits */
  nb1fw_dl_decode_stat_info_s stat; /* decoder stats */
} nb1fw_dl_decode_stat_ind_s;

NB1FW_INTF_MSG_S(dl_decode_stat_ind);

/* chanel end indication. For PDSCH and PUSCH only
   End time is in the header time */
typedef enum
{
  NB1FW_DL_CHAN_END_IND_PDCCH,
  NB1FW_DL_CHAN_END_IND_PDSCH
} nb1fw_dl_chan_end_ind_e;

#define NB1FW_DL_CHAN_END_MAX_REP 0x1
#define NB1FW_DL_CHAN_END_DCI_REP 0x2

typedef struct
{
  nb1fw_ind_hdr_s ind_hdr;      /* message header */
  nb1fw_dl_chan_end_ind_e chan; /* type of channel */
  uint8 reason_mask;            /* reason for channel end */
} nb1fw_dl_chan_end_ind_s;

NB1FW_INTF_MSG_S(dl_chan_end_ind);

typedef struct
{
  nb1fw_ind_hdr_s ind_hdr;      /* message header */
  nb1fw_dl_rlm_stat_s rlmStat;  /* rlm statistics common for both Teval RLM stat request
                                                            triggered by SW and RLM IND triggered at end of 
                                                            Tavg period or PDCCh deconfig autonomously from FW
                                                            side */
}nb1fw_dl_rlm_ind_s;


NB1FW_INTF_MSG_S(dl_rlm_ind);

/* Union of all DL req messages */
typedef union
{
  msgr_hdr_struct_type hdr;
  NB1FW_INTF_MSG(dl_pbch_decode_req);
  NB1FW_INTF_MSG(dl_pbch_decode_stop_req);
  NB1FW_INTF_MSG(dl_si_update_info_req);
  NB1FW_INTF_MSG(dl_si_start_collect_req);
  NB1FW_INTF_MSG(dl_si_stop_collect_req);
  NB1FW_INTF_MSG(dl_pdcch_config_req);
  NB1FW_INTF_MSG(dl_pdcch_rnti_change_req);
  NB1FW_INTF_MSG(dl_pdcch_deconfig_req);
  NB1FW_INTF_MSG(dl_meas_cell_req);
  NB1FW_INTF_MSG(dl_meas_cell_abort_req);
  NB1FW_INTF_MSG(dl_drx_status_update_req);
} nb1fw_dl_msg_req_u;

/* Union of all DL cnf messages */
typedef union
{
  msgr_hdr_struct_type hdr;
  NB1FW_INTF_MSG(dl_pbch_decode_cnf);
  NB1FW_INTF_MSG(dl_pbch_decode_stop_cnf);
  NB1FW_INTF_MSG(dl_si_update_info_cnf);
  NB1FW_INTF_MSG(dl_si_start_collect_cnf);
  NB1FW_INTF_MSG(dl_si_stop_collect_cnf);
  NB1FW_INTF_MSG(dl_pdcch_config_cnf);
  NB1FW_INTF_MSG(dl_pdcch_rnti_change_cnf);
  NB1FW_INTF_MSG(dl_pdcch_deconfig_cnf);
  NB1FW_INTF_MSG(dl_meas_cell_cnf);
  NB1FW_INTF_MSG(dl_meas_cell_abort_cnf);
  NB1FW_INTF_MSG(dl_drx_status_update_cnf);
} nb1fw_dl_msg_cnf_u;

/* Union of all DL ind messages */
typedef union
{
  msgr_hdr_struct_type hdr;
  NB1FW_INTF_MSG(dl_pbch_mib_ind);
  NB1FW_INTF_MSG(dl_sib_ind);
  NB1FW_INTF_MSG(dl_page_failed_ind);
  NB1FW_INTF_MSG(dl_pdcch_dci_ind);
  NB1FW_INTF_MSG(dl_data_ind);
  NB1FW_INTF_MSG(dl_decode_stat_ind);
  NB1FW_INTF_MSG(dl_chan_end_ind);
  NB1FW_INTF_MSG(dl_rlm_ind);
} nb1fw_dl_msg_ind_u;

#endif /* INTF_NB1FW_DL_H */

