/*!
  @file
  prof_hist.h

  @brief
  Histogram profiling.

*/

/*===========================================================================

  Copyright (c) 2018 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

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

#ifndef PROF_HIST_H
#define PROF_HIST_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "fws_types.h"
#include "mcs_comdef.h"
#include "msgr_types.h"


/*===========================================================================

                         MACRO DEFINITIONS

===========================================================================*/

/*! @brief Datatypes for each bin size. */
#define _PROF_HIST_DATATYPE_1BYTE         uint8
#define _PROF_HIST_DATATYPE_2BYTE         uint16
#define _PROF_HIST_DATATYPE_4BYTE         uint32
#define _PROF_HIST_DATATYPE_1BYTE_WRAP    _PROF_HIST_DATATYPE_1BYTE
#define _PROF_HIST_DATATYPE_2BYTE_WRAP    _PROF_HIST_DATATYPE_2BYTE
#define _PROF_HIST_DATATYPE_4BYTE_WRAP    _PROF_HIST_DATATYPE_4BYTE
#define _PROF_HIST_DATATYPE_1BYTE_SAT     _PROF_HIST_DATATYPE_1BYTE
#define _PROF_HIST_DATATYPE_2BYTE_SAT     _PROF_HIST_DATATYPE_2BYTE
#define _PROF_HIST_DATATYPE_4BYTE_SAT     _PROF_HIST_DATATYPE_4BYTE

/*! @brief Histogram bin types, also specify size.
    WRAP: Counters wraparound when they reach their maximum. Need to be logged
          periodically before they wrap twice for reliable logging.
    SAT:  Counter is saturated to its maximum value.
 */
#define PROF_HIST_BIN_TYPE_1BYTE_WRAP     (0x91)
#define PROF_HIST_BIN_TYPE_2BYTE_WRAP     (0x92)
#define PROF_HIST_BIN_TYPE_4BYTE_WRAP     (0x94)
#define PROF_HIST_BIN_TYPE_1BYTE_SAT      (0xA1)
#define PROF_HIST_BIN_TYPE_2BYTE_SAT      (0xA2)
#define PROF_HIST_BIN_TYPE_4BYTE_SAT      (0xA4)
#define PROF_HIST_BIN_TYPE_1BYTE          (PROF_HIST_BIN_TYPE_1BYTE_WRAP)
#define PROF_HIST_BIN_TYPE_2BYTE          (PROF_HIST_BIN_TYPE_2BYTE_WRAP)
#define PROF_HIST_BIN_TYPE_4BYTE          (PROF_HIST_BIN_TYPE_4BYTE_WRAP)
#define PROF_HIST_BIN_TYPE_SIZE(t)        ((t) & 0x7)
#define PROF_HIST_BIN_TYPE_ISWRAP(t)      (((t) >> 4) == 0x9)
#define PROF_HIST_BIN_TYPE_ISSAT(t)       (((t) >> 4) == 0xA)

/*! @brief Current version of histogram log packets. */
#define PROF_HIST_LOG_VERSION             (1)

/*! @brief Maximum size of histogram logging packet. */
#define PROF_HIST_MAX_PACKET_SIZE         (4096)

/*! @brief Default histogram min logging interval, 100 msec in XO ticks. */
#define PROF_HIST_MIN_LOG_INTERVAL_DEFAULT  (XO_NOM_RATE_HZ/10)


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief Histogram log header. */
typedef struct ALIGN(8)
{
  uint8   version;        /*!< Version of this histogram log */
  uint8   num_histograms; /*!< Number of histograms in this log */
  uint8   pad[6];
  uint64  timestamp;      /*!< XO timestamp */
} prof_hist_log_header_t;

/*! @brief Histogram log packet header, includes MSGR and FWS diag wrappers */
typedef struct ALIGN(8)
{
#ifndef MODEM_FW_COPROC_IMAGE
  msgr_hdr_struct_type      msgr;     /*!< MSGR subheader */
#endif
  fw_diag_log_sub_header_t  fws_diag; /*!< FWS diag subheader */
  prof_hist_log_header_t    hist_log; /*!< Histogram log subheader */
} prof_hist_log_packet_header_t;
/* Zero or more prof_hist_header_t+data follow this. */

/*! @brief Histogram log packet handle. */
typedef struct
{
  prof_hist_log_packet_header_t * header;
  uint32                          length; /*!< Total packet length, includes header. */
} prof_hist_log_packet_handle_t;

/*! @brief Histogram header. */
typedef struct ALIGN(4)
{
  uint16 event_id;          /*!< Event ID for this histogram */
  uint8  num_bins_minus_1;  /*!< Number of bins minus 1 (0=1 bin, 255=256 bins) */
  uint8  bin_type;          /*!< Type/size of each bin (see PROF_HIST_BIN_TYPE_x). */
} prof_hist_header_t;
/* num_bins*(bin size in bytes) data follows this, then padding to the next
   32-bit boundary. */

/*! @brief Histogram parameters, not part of log packet. */
typedef struct
{
  int32   min_value;    /*!< Minimum value (left edge of first bin) */
  uint32  span;         /*!< Range of values covered by bins. */
  uint32  scale;        /*!< (num_bins/span) in Q31 */
  uint8   max_bin_idx;  /*!< Maximum bin index (num_bins-1) */
  uint8   bin_type;     /*!< Type/size of each bin (see PROF_HIST_BIN_TYPE_x) */
  prof_hist_header_t * header;  /*!< Header+data of histogram */
} prof_hist_param_t;

/*! @brief Declare a histogram section of a log packet. */
#define _PROF_HIST_DECLARE(_pkt,_args) __PROF_HIST_DECLARE _args
#define __PROF_HIST_DECLARE(_name,_event_id,_num_bins,_min_value,_span,_bin_type) \
  struct { \
    prof_hist_header_t header; \
    _PROF_HIST_DATATYPE_##_bin_type data[_num_bins]; \
  } _name;

/*! @brief Initialize a histogram section of a log packet. */
#define _PROF_HIST_INIT(_pkt,_args) __PROF_HIST_INIT _args
#define __PROF_HIST_INIT(_name,_event_id,_num_bins,_min_value,_span,_bin_type) \
  ._name = { \
    .header = { \
      .event_id = _event_id, \
      .num_bins_minus_1 = (_num_bins)-1, \
      .bin_type = PROF_HIST_BIN_TYPE_##_bin_type \
    } \
  },

/*! @brief Initialize a histogram param structure.
    These parameters are the same as the tuples in PROF_HIST_PACKET_DEFINE.
    _name: Name of the histogram param variable.
    _num_bins: Number of bins in the histogram (0 < _num_bins <= 256)
    _min_value: The minimum value in the histogram (left edge of first bin,inclusive).
    _span: The range of values covered by the histogram.
    _bin_type: Data type/size of each bin. Can be 1BYTE, 2BYTE, or 4BYTE.
    _header_ptr: A pointer to the prof_hist_header_t element in the log packet
                 corresponding to this histogram.

    Example:
    _num_bins=16, _min_value=0, _span=16
      - Values from 0..15 are accepted, and go into bins 0..15.
        Values < 0 are assigned to bin 0, and values >= 16 are assigned to bin 15.
    _num_bins=16, _min_value=-16, _span = 256
      - A span of 256 over 16 bins, so 16 values in each bin, with -16 as the start
        of the first bin.
        -16..-1 go into bin[0], 0..15 into bin[1], 224..239 into bin[15], with
        larger or smaller values grouped into the min/max bin.
 */
#define _PROF_HIST_PARAM_INIT(_name,_num_bins,_min_value,_span,_bin_type,_header_ptr) \
{ \
  .min_value = _min_value, \
  .span = _span, \
  .scale = (uint32) (((_num_bins)*0x80000000uLL+(_span)/2)/(_span)), \
  .max_bin_idx = (_num_bins) - 1, \
  .bin_type = PROF_HIST_BIN_TYPE_##_bin_type, \
  .header = _header_ptr \
}

/*! @brief Declare a param structure for a log packet. */
#define _PROF_HIST_PARAM_DECLARE(_pkt,_args) __PROF_HIST_PARAM_DECLARE(_pkt,MACRO_REMOVE_PAREN _args)
#define __PROF_HIST_PARAM_DECLARE(...) ___PROF_HIST_PARAM_DECLARE(__VA_ARGS__)
#define ___PROF_HIST_PARAM_DECLARE(_pkt,_name,_event_id,_num_bins,_min_value,_span,_bin_type) \
  prof_hist_param_t _name = _PROF_HIST_PARAM_INIT(_name,_num_bins,_min_value,_span,_bin_type,&_pkt._name.header);

/*! @brief Define and declare a histogram log packet.
    This will define a structure for the packet format, hist_param_t
    variables for each individual histogram, and a handle to pass when
    submitting the log packet.

    Arguments:
    _name: Log packet variable name
    _section: Section to place the log structure (leave blank for default section).
    _log_id: MPSS log packet ID.
    _log_period: Logging period. Defaults to 100 msec.
    remainder: A list of parentheses-enclosed tuples for each histogram in the log packet.
               The format of each tuple matches the PROF_HIST_PARAM_INIT() macro.

    Example:
    PROF_HIST_PACKET_DEFINE(lte_LL1_dl_hist, , 0xB1B9,
                            (lte_LL1_ttl_hist,109,10,0,1000,1BYTE_SAT),
                            (lte_LL1_ftl_hist,130,20,0,1000,2BYTE_WRAP),
                            (lte_LL1_dl_thread_hist,98,32,0,19200,4BYTE))

    The above defines the following:
    struct lte_LL1_dl_hist_s {
       prof_hist_log_packet_header_t header;
       struct {
         prof_hist_header_t header;
         uint8 data[10];
       } lte_LL1_ttl_hist;
       struct {
         prof_hist_header_t header;
         uint16 data[20];
       } lte_LL1_ftl_hist;
       struct {
         prof_hist_header_t header;
         uint32 data[32];
       } lte_LL1_dl_thread_hist;
    } lte_LL1_dl_hist_packet;

    prof_hist_packet_handle_t lte_LL1_dl_hist_handle;
    prof_hist_param_t lte_LL1_ttl_hist;
    prof_hist_param_t lte_LL1_ftl_hist;
    prof_hist_param_t lte_LL1_dl_thread_hist;

    and all of the above structures are statically initialized.

    The packet structure (lte_LL1_dl_hist_packet above) never needs to be
    accessed directly. Histograms are updated via their param structures
    (lte_LL1_ttl_hist, etc.), and the log is submitted with the handle
    (lte_LL1_dl_hist_packet_handle).
    All the required packet parameter fields are initialized.

    Externs to the above variables, to access from other files, must be
    manually defined. This also allows placing param structures or handle
    in other memory sections, by providing the section in the prototype.
*/
#define PROF_HIST_PACKET_DEFINE(_name, _section, _log_id, ...) \
  struct _name##_s { \
    prof_hist_log_packet_header_t header; \
    MACRO_APPLY(_PROF_HIST_DECLARE,,__VA_ARGS__) \
  } _name _section = { \
    .header = { \
      .fws_diag = { \
        .code = _log_id, \
        .length = sizeof(struct _name##_s) - OFFSETOF(prof_hist_log_packet_header_t,hist_log) \
      }, \
      .hist_log = { \
        .version = PROF_HIST_LOG_VERSION, \
        .num_histograms = MACRO_COUNT_ARGS(__VA_ARGS__) \
      } \
    }, \
    MACRO_APPLY(_PROF_HIST_INIT,,__VA_ARGS__) \
  }; \
  STATIC_ASSERT(sizeof(_name) <= PROF_HIST_MAX_PACKET_SIZE); \
  prof_hist_log_packet_handle_t _name##_handle = { \
    .header = &_name.header, \
    .length = sizeof(_name) \
  }; \
  MACRO_APPLY(_PROF_HIST_PARAM_DECLARE,_name,__VA_ARGS__)


/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

void prof_hist_update
(
  prof_hist_param_t * hist,
  int32 value
);

void prof_hist_clear
(
  prof_hist_param_t * hist
);

boolean prof_hist_log_periodic
(
  prof_hist_log_packet_handle_t * handle
);

boolean prof_hist_log_now
(
  prof_hist_log_packet_handle_t * handle
);

void prof_hist_set_min_log_interval
(
  uint16 interval
);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif /* PROF_HIST_H */
