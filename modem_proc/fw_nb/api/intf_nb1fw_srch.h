/*!
  @file
  intf_nb1fw_srch.h

  @brief
  FW-SW interface search message definitions for NB1 FW

  @detail
  FW-SW interface search message definitions for NB1 FW

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

$Header: //source/qcom/qct/modem/fw/components/fw_nb.mpss/rel/15.2/nb1/api/intf_nb1fw_srch.h#2 $
$DateTime: 2023/04/27 07:48:28 $
$Author: aakaniti $

when       who     what, where, why
--------   ---     ----------------------------------------------------------

===========================================================================*/

#ifndef INTF_NB1FW_SRCH_H
#define INTF_NB1FW_SRCH_H

#include "intf_nb1fw_common.h"

/*===========================================================================

      Constants

===========================================================================*/


/*===========================================================================

      Macros

===========================================================================*/

/* number of EARFCN for list frequency scan */
#define NB1FW_SRCH_LIST_FSCAN_NUM_CHAN 32

/* number of EARFCN for FULL frequency scan */
#define NB1FW_SRCH_FULL_FSCAN_NUM_CHAN 100

/* number of block cell Ids for search */
#define NB1FW_SRCH_NUM_BLOCKED_CELL_IDS 32

/* number of block cell Ids for search */
#define NB1FW_SRCH_NUM_BLOCKED_EARFCN 20

/* Maximum peaks returned for full init ACQ search */
#define NB1FW_SRCH_NUM_PEAKS_INIT_ACQ 32

/* Maximum peaks returned for cell reconfirmation search */
#define NB1FW_SRCH_NUM_PEAKS_CELL_SRCH 8

/* frequency error threshold (in ppm) between autocorrelation and cross correlation searcher algorithm */
#define NB1FW_SRCH_FERR_UNCERT_THRESHOLD_FOR_XCORR (1<<10)

/*===========================================================================

      Typedefs

===========================================================================*/

/* Enumerate type of search operations */
typedef enum
{
  NB1FW_SRCH_NONE,
  NB1FW_SRCH_FULL_FSCAN,
  NB1FW_SRCH_LIST_FSCAN,
  NB1FW_SRCH_INIT_ACQ_SRCH,
  NB1FW_SRCH_CELL_SRCH,
  NB1FW_SRCH_ABORT,
  NB1FW_SRCH_BLOCKED_CELL_LIST,
  NB1FW_SRCH_MAX_OPS
} nb1fw_srch_op_type_e;

typedef enum
{
  NB1FW_SRCH_HCORR_SCAN_LEVEL_DEEP ,      /* 0*/
  NB1FW_SRCH_HCORR_SCAN_LEVEL_SHALLOW,       /* 1*/
  NB1FW_SRCH_MAX_FSCAN_LEVEL
} nb1fw_srch_fscan_level_e;
#define NB1FW_SRCH_DEFAULT_FSCAN_LEVEL NB1FW_SRCH_HCORR_SCAN_LEVEL_DEEP
/* Structure defintion to allocate */
typedef struct
{
  uint32 earfcn; /* EARFCN scanned */
  int32 rssi;   /* channel RSSI */
} nb1fw_srch_earfcn_pow_s;

/* freq value info for EARFCN */
typedef struct
{
  uint32  dl_carrier_freq; /* DL carrier freq in MHz 32Q8. Used for freq error PPM to Hz conversion */
  uint32  inv_dl_freq;     /* 2^33/DL_F in Q19 units. Used by XO utils to calculate XO related RXLM params */
} nb1fw_srch_earfcn_freq_info_s;

/* Full frequency scan request */
typedef struct
{
  nb1fw_req_hdr_s req_hdr; /* message header */

  /* full frequency scan parameters */
  uint32  start_earfcn; /* starting EARFCN for full frequency scan */
  nb1fw_srch_earfcn_freq_info_s start_earfcn_freq; /* freq info of start EARFCN */
  nb1fw_srch_fscan_level_e fscan_level; /* frequency scan level */

  uint16  num_chan;     /* number of consecutive EARFCNs to scan */
  uint32  rflm_handle;  /* RFLM handle to use to tuning to channels */
  nb1fw_srch_earfcn_pow_s *fscan_result_buf; /* buffer allocated to return result */

  /* optional search parameters */
  boolean srch_en;            /* enable search on channel that has RxAGC above threshold */
  boolean end_of_band;        /* indicate last raster is the end of frequency end */
  int16  ferr_ppm_q10;        /* frequency error */
  int16  ferr_uncert_ppm_q10; /* frequency error uncertaintity */
} nb1fw_srch_full_freq_scan_req_s;

NB1FW_INTF_MSG_S(srch_full_freq_scan_req);

/* List frequency scan request */
typedef struct
{
  nb1fw_req_hdr_s req_hdr; /* message header */

  uint16  num_chan;      /* number of consecutive EARFCNs to scan */
  uint32  earfcn[NB1FW_SRCH_LIST_FSCAN_NUM_CHAN]; /* EARFCN list for frequency scan */
  nb1fw_srch_earfcn_freq_info_s earfcn_freq[NB1FW_SRCH_LIST_FSCAN_NUM_CHAN]; /* freq info of EARFCNs */
  nb1fw_srch_fscan_level_e fscan_level; /* frequency scan level */

  uint32  rflm_handle;   /* RFLM handle to use to tuning to channels */
  nb1fw_srch_earfcn_pow_s *fscan_result_buf; /* buffer allocated to return result */

  boolean srch_en;            /* enable search on channel that has RxAGC above threshold */
  int16  ferr_ppm_q10;        /* frequency error */
  int16  ferr_uncert_ppm_q10; /* frequency error uncertaintity */
} nb1fw_srch_list_freq_scan_req_s;

NB1FW_INTF_MSG_S(srch_list_freq_scan_req);

/* Blocked cell Id set request */
typedef struct
{
  uint32  earfcn;                              /* EARFCN */
  uint16  num_cells;                           /* number of cells to block for this earfcn */
  uint16  id[NB1FW_SRCH_NUM_BLOCKED_CELL_IDS]; /* cell Ids of blocked cells */
} nb1fw_srch_blocked_earfcn_list_s;
typedef struct
{
  nb1fw_req_hdr_s req_hdr; /* message header */
  uint16  num_earfcn;                       /* number of earfcn's to be blocked */
  nb1fw_srch_blocked_earfcn_list_s earfcnList[NB1FW_SRCH_NUM_BLOCKED_EARFCN];
} nb1fw_srch_blocked_cell_list_req_s;

NB1FW_INTF_MSG_S(srch_blocked_cell_list_req);

/* Init ACQ full PSS/SSS search */
typedef struct
{
  nb1fw_req_hdr_s req_hdr; /* message header */

  uint32  earfcn;             /* EARFCN to search */
  nb1fw_srch_earfcn_freq_info_s earfcn_freq; /* freq info of EARFCN */

  nb1fw_srch_fscan_level_e fscan_level; /* frequency scan level */

  uint32  rflm_handle;        /* RFLM handle to use to tuning to channel */
  int16  ferr_ppm_q10;        /* frequency error */
  int16  ferr_uncert_ppm_q10; /* frequency error uncertaintity */
  uint8  max_cells;           /* maximum number of cells to be detected */ 
  uint16 locked_pcid;      /* set to 0xffff if no locked pcid; set to valid pcid for pcid_locked case, max_cells will be 1 */
  int32 raster_offset_hz;      /*raster offset in Hz, need to be in ini_acq and intra_freq search; set to 0xFFFFFFFF for inter freq search */
} nb1fw_srch_init_acq_req_s;

NB1FW_INTF_MSG_S(srch_init_acq_req);

typedef struct
{
  uint16  cell_pos;     /* cell position to center window around (0..19200-1) */
  uint16  window_size;  /* window size around cell position to search in */
  uint16  cell_id;      /* For selecting SSS sequence */
  uint16  sfn_offset;   /* For selecting DDD sequence. (0..1023). Only mod 8 used */
  int32  raster_offset_hz; /* The raster offset in Hz*/
  boolean slam_ferr;    /* Slam frequency error before searching */
  int16   ferr_ppm_q10; /* Frequency error to slam is slam_ferr is TRUE */
} nb1fw_srch_cell_srch_cell_info_s;

/* Cell reconfirmation search using SSS sequence */
typedef struct
{
  nb1fw_req_hdr_s req_hdr; /* message header */

  uint32  earfcn;       /* EARFCN to search */
  nb1fw_srch_earfcn_freq_info_s earfcn_freq; /* freq info of EARFCN */
  uint32  rflm_handle;  /* RFLM handle to use to tuning to channel */
  uint8   num_cells;   /* number of cells */
  int16   ferr_uncert_ppm_q10; /* frequency error uncertaintity */
  boolean long_reconf_search_flag;  /*if flag =1, means it is long reconf search; flag =0, short reconf search */
  nb1fw_srch_cell_srch_cell_info_s cell_info[NB1FW_SRCH_NUM_PEAKS_CELL_SRCH];
} nb1fw_srch_cell_srch_req_s;

NB1FW_INTF_MSG_S(srch_cell_srch_req);

/* Fscan or search abort request */
typedef struct
{
  nb1fw_req_hdr_s req_hdr; /* message header */
} nb1fw_srch_abort_req_s;

NB1FW_INTF_MSG_S(srch_abort_req);

typedef struct
{
  uint16 pos;          /* Sub radio frame position (0..19200-1) */
  uint16 sfn_offset;   /* SFN (systems frame number) offset from ref time (0..7) */
  uint16 pcell_id;     /* Pcell Id from SSS */
  int16  ferr_ppm_q10; /* frequency error measured by FW */
  int32  pss_psr;
  int32  sss_psr;
} nb1fw_srch_init_acq_peak_s;


typedef nb1fw_srch_init_acq_peak_s nb1fw_srch_cell_srch_peak_s;

/* Full frequency scan result confirmation */
typedef struct
{
  nb1fw_cnf_hdr_s cnf_hdr; /* message header */

  nb1fw_srch_earfcn_pow_s *fscan_res_buf; /* return fscan result buffer pointer */

  boolean srch_res_valid; /* TRUE if search enabled and succeeded */
  uint32  srch_earfcn;    /* EARFCN above threshold with successful search */
  uint8 num_peaks;        /* number of successful peaks */
  nb1fw_srch_init_acq_peak_s peak[NB1FW_SRCH_NUM_PEAKS_INIT_ACQ]; /* search peaks */
} nb1fw_srch_freq_scan_cnf_s;

typedef nb1fw_srch_freq_scan_cnf_s nb1fw_srch_full_freq_scan_cnf_s;
NB1FW_INTF_MSG_S(srch_full_freq_scan_cnf);

typedef nb1fw_srch_freq_scan_cnf_s nb1fw_srch_list_freq_scan_cnf_s;
NB1FW_INTF_MSG_S(srch_list_freq_scan_cnf);

/* Blocked cell Id set confirm */
typedef struct
{
  nb1fw_cnf_hdr_s cnf_hdr; /* message header */
} nb1fw_srch_blocked_cell_list_cnf_s;

NB1FW_INTF_MSG_S(srch_blocked_cell_list_cnf);

/* Init ACQ full PSS/SSS search */
typedef struct
{
  nb1fw_cnf_hdr_s cnf_hdr; /* message header */

  nb1fw_srch_earfcn_pow_s earfcn_info; /* EARFCN info of search */
  uint8 num_peaks;                     /* number of successful peaks */
  nb1fw_srch_init_acq_peak_s peak[NB1FW_SRCH_NUM_PEAKS_INIT_ACQ]; /* search peaks */
} nb1fw_srch_init_acq_cnf_s;

NB1FW_INTF_MSG_S(srch_init_acq_cnf);

/* Cell reconfirmation search using SSS sequence */
typedef struct
{
  nb1fw_cnf_hdr_s cnf_hdr; /* message header */

  nb1fw_srch_earfcn_pow_s earfcn_info; /* EARFCN info of search */
  uint8 num_peaks;                     /* number of successful peaks */
  nb1fw_srch_cell_srch_peak_s peak[NB1FW_SRCH_NUM_PEAKS_CELL_SRCH]; /* search peaks */
  uint8 num_peaks_ext;                 /* number of extra successful peaks */
  nb1fw_srch_cell_srch_peak_s peak_ext[NB1FW_SRCH_NUM_PEAKS_CELL_SRCH]; /* search extra peaks */
} nb1fw_srch_cell_srch_cnf_s;

NB1FW_INTF_MSG_S(srch_cell_srch_cnf);

/* Fscan or search abort request */
typedef struct
{
  nb1fw_cnf_hdr_s cnf_hdr; /* message header */

  nb1fw_srch_op_type_e op_type; /* Search operation type aborted */
} nb1fw_srch_abort_cnf_s;

NB1FW_INTF_MSG_S(srch_abort_cnf);

/* Union of all srch req messages */
typedef union
{
  msgr_hdr_struct_type hdr;

  NB1FW_INTF_MSG(srch_full_freq_scan_req);
  NB1FW_INTF_MSG(srch_list_freq_scan_req);
  NB1FW_INTF_MSG(srch_blocked_cell_list_req);
  NB1FW_INTF_MSG(srch_init_acq_req);
  NB1FW_INTF_MSG(srch_cell_srch_req);
  NB1FW_INTF_MSG(srch_abort_req);
} nb1fw_srch_msg_req_u;

/* Union of all search cnf messages */
typedef union
{
  msgr_hdr_struct_type hdr;

  NB1FW_INTF_MSG(srch_full_freq_scan_cnf);
  NB1FW_INTF_MSG(srch_list_freq_scan_cnf);
  NB1FW_INTF_MSG(srch_blocked_cell_list_cnf);
  NB1FW_INTF_MSG(srch_init_acq_cnf);
  NB1FW_INTF_MSG(srch_cell_srch_cnf);
  NB1FW_INTF_MSG(srch_abort_cnf);
} nb1fw_srch_msg_cnf_u;

typedef struct
{
  int32   timeHypo             : 16;   /* timing offset information of pss candidate */
  uint32   useHighThreshold    : 1;
  uint32   reserved            : 15;
  uint32  freqHypo;   /* frequency offset information of pss candidate */
} nb1fw_srch_pss_candidate_s;

typedef struct
{
  int32   pssPeakVal;   /* pss peak value */
  int32   pssPeakCFO;   /* associated frequency offset value */
  int32   pssPeakPos;   /* associated timing offset value */
} nb1fw_srch_pss_peak_s;

#endif /* INTF_NB1FW_SRCH_H */

