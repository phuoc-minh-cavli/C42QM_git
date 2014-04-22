#ifndef NB1_CPHY_IRAT_MEAS_MSG_H
#define NB1_CPHY_IRAT_MEAS_MSG_H
/*!
  @file nb1_cphy_irat_meas_msg.h

  @brief
    The header file describes the interface to the NB1 L1 Measurement module.


 */

/*==============================================================================

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

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/nb1.mpss/2.2/api/nb1_cphy_irat_meas_msg.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------

==============================================================================*/

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include <nb1_l1_types.h>
#include <msgr_nb1.h>
#include <msgr.h>
#include <sys.h>

/*==============================================================================

                               MACROS

==============================================================================*/
/*! The size of the rf parameter buffer */
#define NB1_ML1_MAX_RF_SRC_PARAM_BUF_SIZE 16

#define NB1_CPHY_IMEAS_NUM_RX_ANTENNAS 2
/*! Maximum number of E-UTRA cells per frequency*/
#define NB1_CPHY_IMEAS_MAX_BLACKLIST_CELLS 16

/*! Maximum number of E-UTRA frequency*/
#define NB1_CPHY_IMEAS_MAX_BLACKLIST_FREQS 8

/*! Maximum number of detected cells per frequncy */
#define NB1_CPHY_IMEAS_MAX_DETECTED_CELLS 15

/*! Maximum number of cells that can be measured in a single meas req */
#define NB1_CPHY_IMEAS_MAX_MEAS_CELLS 15

/*! Maximum number of cells that can be measured in a single meas req
for X2L Connected mode*/
#define NB1_CPHY_IMEAS_MAX_MEAS_CELLS_TIMED 9

/*! The number of antennas for which measurements have returned */
#define NB1_CPHY_IRAT_MEAS_NUM_ANT 2

/*! The size of the cell bar array
  ciel(504/8) = ciel(num_cell_ids/sizeof(uint8)
 */
#define CELL_BLACKLIST_ARRAY_MASK_SIZE 63

/*! Utility macro for setting a barring bit in the cell bar array */
#define SET_BAR_BIT(bar_array, cell_id) \
  ( (bar_array)[(cell_id)/8] |= (1<<((cell_id) % 8)))

/*! Utility macro for checking barred bit in the cell bar array */
#define BAR_BIT_SET(bar_array, cell_id) \
  (( (bar_array)[(cell_id)/8] & (1<<((cell_id) % 8))) ? TRUE : FALSE )

/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/


typedef enum
{
  NB1_CPHY_IMEAS_RAT_WCDMA=0,
  NB1_CPHY_IMEAS_RAT_GSM,
  NB1_CPHY_IMEAS_RAT_DO,
  NB1_CPHY_IMEAS_RAT_CDMA1X,
  NB1_CPHY_IMEAS_RAT_TDS,
  NB1_CPHY_IMEAS_RAT_NONE
} nb1_cphy_irat_meas_rat_type_e;

/*! Fractional integer with 4 fractional bits */
typedef int16 nb1_cphy_irat_meas_x16_t;

#define CPHY_IRAT_MEAS_X16_TO_INT(x) ((x)/16)

#define NB1_CPHY_IMEAS_MAX_SOURCE_ITEMS 2

typedef enum
{
  NB1_CPHY_IMEAS_DEVICE_TRANSCEIVER_0 =0,
  NB1_CPHY_IMEAS_DEVICE_RECEIVER_1,
  NB1_CPHY_IMEAS_DEVICE_RECEIVER_2,
  NB1_CPHY_IMEAS_DEVICE_RECEIVER_3,
  NB1_CPHY_IMEAS_DEVICE_RECEIVER_DIV,
  NB1_CPHY_IMEAS_DEVICE_RFCOM_MAX_DEVICES
}nb1_cphy_irat_meas_device_e;

typedef enum
{
  NB1_CPHY_IMEAS_CARRIER_SINGLE_CARRIER = 0,
  NB1_CPHY_IMEAS_CARRIER_DUAL_CARRIER = 1,
  NB1_CPHY_IMEAS_CARRIER_3_CARRIER = 2,
  NB1_CPHY_IMEAS_CARRIER_MAX_CARRIERS = 3
} nb1_cphy_irat_meas_carrier_hspa_id_e;

typedef enum
{
  NB1_CPHY_IMEAS_AGC_MODE_NORMAL,
  NB1_CPHY_IMEAS_AGC_MODE_FAST,
  NB1_CPHY_IMEAS_AGC_MODE_RESUME_WITH_NEW,
  NB1_CPHY_IMEAS_AGC_MODE_RESUME_WITH_CURR,
  NB1_CPHY_IMEAS_AGC_MODE_ENABLE_DIV,
  NB1_CPHY_IMEAS_AGC_MODE_ENABLE_DIV_NORMAL,

  // don't add modes below this
  NB1_CPHY_IMEAS_AGC_MODE_INVALID
} nb1_cphy_irat_meas_hdr_agc_mode_e;

/*! @brief WCDMA->NB1 carrier specific tuning parameters */
typedef struct
{
  /*!  Device on which WCDMA will be tuned  */
  uint32 device;

  /*! TRUE if RxD is ON, FALSE otherwise */
  boolean div_en;

  /*! Diversity device on which WCDMA will be tuned */
  uint32 div_device;

  /*! Specifes source technology band */
  uint32             source_band;

  /*! Specify SC or DC mode */
  nb1_cphy_irat_meas_carrier_hspa_id_e carrier_type;

  /*! DL channels on which WCDMA will be tuned */
  uint16             channel[NB1_CPHY_IMEAS_CARRIER_MAX_CARRIERS];

} nb1_cphy_irat_meas_wcdma_source_param_s;

/*! @brief This structure will be used for WCDMA->NB1 */
typedef struct
{
  uint8 num_source_item;
  nb1_cphy_irat_meas_wcdma_source_param_s source_param[NB1_CPHY_IMEAS_MAX_SOURCE_ITEMS];

  /*!  Device on which WCDMA will be tuned  */
  nb1_cphy_irat_meas_device_e device;

  /*! True if diversity device required */
  boolean div_en;

  /*! Diversity device on which WCDMA will be tuned  */
  nb1_cphy_irat_meas_device_e div_device;

  /*! Specify source technology band*/
  uint32             source_band;

  /*!  DL Channel on which WCDMA will be tuned  */
  uint16             channel[NB1_CPHY_IMEAS_CARRIER_MAX_CARRIERS];

  /*!  Specify   SC or DC mode  */
  nb1_cphy_irat_meas_carrier_hspa_id_e carrier_type;

} nb1_cphy_irat_meas_wcdma_rf_param_type;

/*! @brief  This structure will be used for TDS->NB1   */
typedef struct
{
  /*! Antenna 0 rxlm buffer for rxlm related targets
   */
  /*! uarfcn of TDSCDMA source cell*/
  uint32                  uarfcn;
  /*! Buffer index for rf scripts.*/
  uint32                  buffer_index;

  /*!  Device on which TDS will be tuned  */
  uint32 device_id;

  /*! Boolean to indicate if TDS diversity is enabled  */
  boolean div_enabled;

  /*! TDS Diversity Device_id  */
  uint32 div_device_id;

} nb1_cphy_irat_meas_tds_rf_param_type;

typedef struct
{

  /*! The source GSM frequency for the tuneback script */
  nb1_l1_gsm_arfcn_t arfcn;

  /*! The source GSM band */
  nb1_l1_gsm_band_e band;

  /*! The rssi on the source frequency, in dBmx16 units */
  nb1_l1_gsm_rssi_t rssi;

  /*! Device_id of GSM  */
  uint32 device_id;
} nb1_cphy_irat_meas_gsm_rf_param_type;


/*! @brief  This structure will be used for DO->NB1   */
typedef struct
{
  /*!  Channel on which HDR will be tuned, */
  int16 band;

  /*!  Band on which HDR will be tuned, */
  uint16 chan;

  /*! Specify HDR RxAGC Mode */
  nb1_cphy_irat_meas_hdr_agc_mode_e   agc_mode;

  /*! Specify HDR RxAGC RTC Offset */
  uint32                agc_rtc_offset;

  /*! Specify HDR RxAGC previous AGC value */
  int16                 prev_agc;

  /*! @brief  Current active HDR device */
  uint32 device;

} nb1_cphy_irat_meas_hdr_rf_param_type;


typedef struct
{
  /*! @brief  Band and channel on which 1x will be tuned  */
  sys_channel_type   band_chan;

  /*! @brief  Current active 1x device */
  uint32 device;

}nb1_cphy_irat_meas_onex_rf_param_type;


/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/



/*! @brief Blacklist structure specifying the blacklisted cells on a particular
    frequency.
    */
typedef struct
{
  /*! The frequency which the black listed cells reside on.
  range 0...65535
  */
  nb1_earfcn_t       freq;

  /*! Large bitmask identifying which cells should be barred by L1.
    The first N entries will be barred via firmware (N is defined by
    what is supported by firmware), whereas the rest will be
    performed by L1 which will prune out any barred cells.

    The first array entry corresponds to the first 8 barred cells and the
    second corresponds to the next 8.  For instance for cell id 0, bit 0 of
    entry 0 will be set to 1.  For cell id 17 bit 1 of entry 2 will be set to 1.

    @see SET_BAR_BIT for details about which bits will be set.  By calling
    SET_BAR_BIT(cell_blacklist_array_mask,n) will bar cell id n.
   */
  uint8 cell_blacklist_array_mask[CELL_BLACKLIST_ARRAY_MASK_SIZE ];


}  nb1_cphy_irat_meas_blacklist_cells_s;


typedef struct
{
  /*! The frame boundary of the NB1 cell based upon the NB1 RTC */
  uint64 timing_offset[NB1_CPHY_IMEAS_NUM_RX_ANTENNAS];

  /*! The timing adjustment in the cir domain */
  uint16 timing_adjustment_cir[NB1_CPHY_IMEAS_NUM_RX_ANTENNAS];

  /*!  initial PSS based frequency offset estimation in Hz*/
  int16 freq_offset_est;

  /*! sss correlation value */
  uint32 sss_corr;

} nb1_cphy_irat_meas_cell_meas_state_s;


/*! @brief IRAT search detected cell structure.

  */
typedef struct
{
  /*! The physical cell id of the detected cell */
  nb1_phy_cell_id_t cell_id;
  /*! The CP type of the detected cell. */
  nb1_l1_cp_e cp_type;

  /*! The current FTL/TTL state for the detected cell */
  nb1_cphy_irat_meas_cell_meas_state_s state;
} nb1_cphy_irat_meas_detected_cell_s;


/*! @brief IRAT search detected cell structure.

  */
typedef struct
{
  /*! The physical cell id of the detected cell */
  nb1_phy_cell_id_t cell_id;

  /*! The CP type of the detected cell. */
  nb1_l1_cp_e cp_type;

  /*! The current FTL/TTL state of the cell (related to cell tracking) */
  nb1_cphy_irat_meas_cell_meas_state_s state;
} nb1_cphy_irat_meas_meas_params_s;


/*! @brief IRAT search detected cell structure.
*/
typedef struct
{
  /*! The physical cell id of the detected cell */
  nb1_phy_cell_id_t cell_id;

  /*! The CP type of the cell. */
  nb1_l1_cp_e cp_type;

  /*! RSRP combined value combined across Tx-Rx pairs. In linear scale.
  Range -144dBm..-44dBm with resolution of 1dBm
  Ref 36.133 Sec 9.1.4

  THIS FIELD WILL BE REMOVED
  */
  int16 rsrp;
  /*! Instantaneous RSRQ value combined across Tx-Rx pairs. In linear scale.
  Range -19.5dB ... -3dB with 0.5 dB resolution
  Ref 36.133 Sec 9.1.7

  THIS FIELD WILL BE REMOVED
  */
  int16 rsrq;

  /*! Instantaneous RSRP per antenna
  Range -144dBm..-44dBm with resolution of 1dBm
  Ref 36.133 Sec 9.1.4

  Values are returned in Q4 or X16 format meaning each unit is
  1/16th of a dbm.  To convert to int use CPHY_IRAT_MEAS_X16_TO_INT().
  */
  int16 rsrp_ant[NB1_CPHY_IMEAS_NUM_RX_ANTENNAS];

  /*! Instantaneous RSRQ per antenna
  Range -19.5dB ... -3dB
  Ref 36.133 Sec 9.1.7

  Values are returned in Q4 or X16 format meaning each unit is
  1/16th of a dbm.  To convert to int use CPHY_IRAT_MEAS_X16_TO_INT().
  */
  int16 rsrq_ant[NB1_CPHY_IMEAS_NUM_RX_ANTENNAS];

  /*! Instantaneous RSSI per antenna
  Range -120 ... 0
  Ref 36.133 Sec 9.1.7

  Values are returned in Q4 or X16 format meaning each unit is
  1/16th of a db.  To convert to int use CPHY_IRAT_MEAS_X16_TO_INT().
  */
  int16 rssi_ant[NB1_CPHY_IMEAS_NUM_RX_ANTENNAS];

  /*! Instantaneous CER SNR per antenna
  Values are returned in Q16
  The field will be populated only for W2L TDD connected mode
  per current system design.
  */
  uint32  cer_snr[NB1_CPHY_IMEAS_NUM_RX_ANTENNAS];

  /*! The current FTL/TTL state of the cell (related to cell tracking) */
  nb1_cphy_irat_meas_cell_meas_state_s state;

  /*!Result is interfered by other tech*/
  boolean  interf_active;

} nb1_cphy_irat_meas_meas_results_s;

/*!
 @defgroup cphy_irat_meas NB1 L1 IRAT Meas Interface

  @brief This module describes the NB1 L1 IRAT Measurement module.

  @details

  The NB1 L1 IRAT meas interface is a message-based interface
  exposed by the NB1 L1 task to external RATs.

  The messages can be sent in the following order:

  - Prior to Measurement
    -# nb1_cphy_irat_meas_init_req_s / nb1_cphy_irat_meas_init_cnf_s
    -# nb1_cphy_irat_meas_blacklist_cmd_s
  - During Measurement ( At least one search or meas must be present )
    (Without timing).  DEPRECATED TO BE REMOVED IN BOLT
    -# nb1_cphy_irat_meas_startup_cmd_s / nb1_cphy_irat_meas_startup_cnf_s
    -# nb1_cphy_irat_meas_search_req_s / nb1_cphy_irat_meas_search_cnf_s (optional)
    -# nb1_cphy_irat_meas_meas_req_s / nb1_cphy_irat_meas_meas_cnf_s (optional)
    -# nb1_cphy_irat_meas_meas_req_s / nb1_cphy_irat_meas_meas_cnf_s

  - During Measurement (Timed measurement flow
    -# nb1_cphy_irat_meas_timed_srch_meas_req_s / nb1_cphy_irat_meas_timed_srch_meas_cnf_s
  - After NB1 measurements no longer need to be made, or a switch back to
    NB1 as the source rat is possible.
    -# nb1_cphy_irat_meas_deinit_req_s

  This interface is generated from
  $Header: //components/rel/nb1.mpss/2.2/api/nb1_cphy_irat_meas_msg.h#1 $

@{*/


/*! @brief Sets up the blacklist for this module.

  The blacklist is an absolute configuration of which cells are blacklisted.
  The blacklist is not expected to be received b/w startup req & cleanup req
  in idle mode.
  In case if it is received in between then this blacklist update will happen
  only at the next startup req. (in connected mode this will most likely be
  the case ).

  This command simply specifies which cells that NB1 L1 will not report in
  the search results.  This will include avoiding cells during the detection
  process.  Both cells that are barred or blacklisted can be specified using
  this command.

*/
typedef struct
{
  msgr_hdr_struct_type hdr;

  /*! Number of freqs for which the blacklist information is sent
  Range 1..NB1_CPHY_IMEAS_MAX_BLACKLIST_FREQS
  */
  uint8    num_freqs;

  /*! The blacklist of cells, which are organized in frequency groups. */
  nb1_cphy_irat_meas_blacklist_cells_s
    blacklist[NB1_CPHY_IMEAS_MAX_BLACKLIST_FREQS];
} nb1_cphy_irat_meas_blacklist_cmd_s;


/*! @brief Configures and initializes the NB1 IRAT meas module.

  This message should be sent prior to any other IRAT meas messages being
  sent.

*/
typedef struct
{
  msgr_hdr_struct_type hdr;

  /*! The home RAT which will be driving the measurements */
  nb1_cphy_irat_meas_rat_type_e rat;

  /*! Flag to make sure the NB1 timing remains on.
    Note that if the OSTMR is shut down NB1 timing will be invalid
    and the timing restore mechanism must be used.

    This flag is recommended to be set to TRUE for W->L cell
    DCH or GSM dedicated mode */
  boolean maintain_timing;

  /*! Flag to indicate the snapshot is present */
  boolean snapshot_present;

  /*! Snapshot of the NB1 and Other RAT timelines */
  nb1_l1_nb1_stmr_snapshot_s snapshot;
} nb1_cphy_irat_meas_init_req_s;

/*! @brief

  This message should be sent once the app enable is successful
  and NB1 driver was initialized properly

*/
typedef struct
{
  msgr_hdr_struct_type hdr;

  /*! Max NB1 Transmit Power for UE in dBm */
  int16                max_tx_pwr;

} nb1_cphy_irat_meas_init_cnf_s;


/*! @brief Deconfigures the IRAT meas module.

  This should be sent when a particular RAT is done measuring NB1 cells.
*/
typedef struct
{
  msgr_hdr_struct_type hdr;

} nb1_cphy_irat_meas_deinit_req_s;

/*! @brief

  This should be sent when a NB1 is done with the resetting of the NB1 fw
*/
typedef struct
{
  msgr_hdr_struct_type hdr;
  /*! Flag to indicate the snapshot is present */
  boolean snapshot_present;
  /*! Snapshot of the NB1 and Other RAT timelines for HO time transfer*/
  nb1_l1_nb1_stmr_snapshot_s snapshot;

} nb1_cphy_irat_meas_deinit_cnf_s;

typedef union nb1_cphy_irat_meas_rf_source_params_union
{
  nb1_cphy_irat_meas_wcdma_rf_param_type wcdma_params;
  nb1_cphy_irat_meas_hdr_rf_param_type   hdr_params;
  nb1_cphy_irat_meas_onex_rf_param_type   onex_params;
  nb1_cphy_irat_meas_gsm_rf_param_type    gsm_params;
  nb1_cphy_irat_meas_tds_rf_param_type    tds_params;

} nb1_cphy_irat_meas_rf_source_params_u;

/*! @brief
  Temporarily define this to fix 3.x.  When WCMDA support removal of this struct, delete it. */
typedef struct nb1_cphy_irat_meas_rf_source_params_struct
{
  uint32 rf_param_buffer[NB1_ML1_MAX_RF_SRC_PARAM_BUF_SIZE];

} nb1_cphy_irat_meas_rf_source_params_s;

/*! @brief Structure containing the RF parameters to populate to build
  the scripts
  */
typedef struct
{

  /*! RF startup buffer Index */
  uint16 startup_rf_idx;

  /*! RF startup buffer Index */
  uint16 cleanup_rf_index;

  /*! Needs to be the appropriate RF type for tune back. */
  nb1_cphy_irat_meas_rf_source_params_u source_parameters;


  /******************* RXLM Parameters ***************************/

  /*! Antenna 0 rxlm buffer for rxlm related targets
   should be allocated with rxlm_allocate_buffer() and cast
   to uint8
   */
  uint32 nb1_rxlm_rx_buffer_ant0;

  /*! Antenna 1 rxlm buffer for rxlm related targets
   should be allocated with rxlm_allocate_buffer() and cast
   to uint8
   */
  uint32 nb1_rxlm_rx_buffer_ant1;

  /*! NB1 Device_id  */
  uint32 nb1_device_id;

  /*! Boolean to indicate if NB1 Diversity is allocated  */
  boolean nb1_div_available;

  /*! NB1 Diversity Device_id  */
  uint32 nb1_div_device_id;

  /*! Source rat rxlm buffer for rxlm related targets
   should be allocated with rxlm_allocate_buffer() and cast
   to uint8
   */
  uint32 source_rat_rxlm_rx_buffer_ant0;

} nb1_cphy_irat_meas_startup_rf_params_s;
/*! @brief Starts up the NB1 measurement module so searches and mesurements
  can be performed.

  This message will be executed immediately.

  @deprecated THIS COMMAND WILL BE REMOVED IN BOLT. Use TIMED_SEARCH_MEAS_REQ

*/
typedef struct
{
  msgr_hdr_struct_type hdr;

  /*! The frequency on which the nb1 search is to be performed on.
  RANGE 0..65535
  */
  nb1_earfcn_t earfcn;

  /*! The downlink bandwith for the cell to be searched/measured */
  nb1_bandwidth_e  dl_bandwidth;

  /*! Flag to indicate to the NB1 measurement driver to use scripts instead
    of function calls to tune.  This will require the source rat to pass
    the rf parameters in the rf_params variable as well as run the tune back
    script after cleanup has been run. */
  boolean use_scripts;

  /*! Structure containing the RF parameters to populate to build
    the scripts
   */
  nb1_cphy_irat_meas_startup_rf_params_s rf_params;

  /*! The accumulated timing drift from the source RAT stmr, between
    the last STARTUP_REQ and this STARTUP_REQ.

   Units are in NB1 RTC/OSTMR ticks, 30720 ticks/ ms which
   is equivalent to WCDMA cx8.

   */
  int32 timing_adjustment;

} nb1_cphy_irat_meas_startup_cmd_s;


/*! @brief Finishes the NB1 measurement modules' searches.

  This message must be sent prior to performing any operations on the home
  RAT and will potentially trigger the source RAT to tune back (when in
  connected mode).

  This message can be sent prior to finishing the last command and the NB1
  mesurements module will execute it after the last command is complete.


  @deprecated THIS COMMAND WILL BE REMOVED IN BOLT. Use TIMED_SEARCH_MEAS_REQ
*/
typedef struct
{
  msgr_hdr_struct_type hdr;

} nb1_cphy_irat_meas_cleanup_req_s;

/*! @brief Notifies the other RAT that all NB1 activities have been cleaned
  up.

  @deprecated THIS COMMAND WILL BE REMOVED IN BOLT. Use TIMED_SEARCH_MEAS_REQ
*/
typedef struct
{
  msgr_hdr_struct_type hdr;

} nb1_cphy_irat_meas_cleanup_cnf_s;

/*! @brief Executes an NB1 search on the frequency tuned via the STARTUP_REQ.

  The search will use the blacklist specified in the config to eliminate
  undesireable cell ids when searching.

  NB1 searches use the PSS/SSS to discover the NB1 cell id, timing as well
  as cyclic prefix.

  @deprecated THIS COMMAND WILL BE REMOVED IN BOLT. Use TIMED_SEARCH_MEAS_REQ
  */
typedef struct
{
  msgr_hdr_struct_type hdr;

  /* Search parameters will be defaunb1d at the moment, TODO explore having source rat pass these params */
} nb1_cphy_irat_meas_search_req_s;

/*! @brief IRAT Search confirmation containing the search results.


  @deprecated THIS COMMAND WILL BE REMOVED IN BOLT. Use TIMED_SEARCH_MEAS_REQ
  */
typedef struct
{

  msgr_hdr_struct_type hdr;

  /*! The frequency which the black listed cells reside on.
  range 0...65535
  */
  nb1_earfcn_t       freq;

  /*! The number of detected cells */
  uint8 num_detected_cells;

  /*! The detected cells from the cell search */
  nb1_cphy_irat_meas_detected_cell_s det_cells[ NB1_CPHY_IMEAS_MAX_DETECTED_CELLS];

  /*!Result is interfered by other tech*/
  boolean  interf_active;

} nb1_cphy_irat_meas_search_cnf_s;

/*! @brief IRAT Measurement request, triggers NB1 measurements on already
  detected NB1 cells.

  Cells must be detected via an nb1_cphy_irat_meas_search_req_s prior to
  measuring them.  Measurements are performed on the RS tones in the NB1
  frequency grid.

  @deprecated THIS COMMAND WILL BE REMOVED IN BOLT. Use TIMED_SEARCH_MEAS_REQ
  */
typedef struct
{

  msgr_hdr_struct_type hdr;

  /*! The number of detected cells */
  uint8 num_cells;

  /*! The cells which to measure on the specified frequency */
  nb1_cphy_irat_meas_meas_params_s cells[ NB1_CPHY_IMEAS_MAX_MEAS_CELLS];

} nb1_cphy_irat_meas_meas_req_s;

/*! Parameters specifying the size and location of a
  measurement gap.
  */
typedef struct
{
  /*! The type of timing to use for timing restore */
  nb1_cphy_irat_meas_rat_type_e time_rat_type;

  /*! Gap will start as soon as possible gap_start field will
    be ignored but gap_end should remain valid */
  boolean asap_gap_start;

  /*! The beginning of the gap time available to NB1, including
    the NB1 tune time.  The source rat must not be active at this
    time.
    */
  nb1_l1_stmr_u gap_start;
  /*! The end of the gap provided to NB1.  This does not include
    the source RAT tune-back time or any Source Rat procedures
    such as AGC. */
  nb1_l1_stmr_u gap_end;

  /*! The acceptable time for NB1 to turn of it's clocks.
    This must be after the RF tune back on the source RAT.
    */
  nb1_l1_stmr_u clock_off_time;

} nb1_cphy_irat_meas_gap_s;

typedef struct
{
  /*! TRUE when search needs to be performed, FALSE otherwise */
  boolean perform_search;

  /*! Flag to indicate whether NB1 L1 should measure newly detected
    cells and provide them in the measurment results

    This can only be set to TRUE if perform_search is TRUE
   */
  boolean measure_newly_detected_cells;

} nb1_cphy_irat_meas_timed_search_param_s ;

typedef struct
{
  /*! The measurement frequency range 0...65535 */
  nb1_earfcn_t       meas_freq;

  /*! The number of cells to be measured */
  uint8 num_cells;

  /*! The cells which to measure on the specified frequency */
  nb1_cphy_irat_meas_meas_params_s
    cells[NB1_CPHY_IMEAS_MAX_MEAS_CELLS];

} nb1_cphy_irat_meas_timed_meas_param_s;


typedef struct
{
  /*! The measurement frequency range 0...65535 */
  nb1_earfcn_t       meas_freq;

  /*! The size of the array 'cells' */
  uint8 num_cells;

  /*! The cells which where measured */
  nb1_cphy_irat_meas_meas_results_s cells
    [ NB1_CPHY_IMEAS_MAX_MEAS_CELLS + NB1_CPHY_IMEAS_MAX_DETECTED_CELLS];

  /*!Result is interfered by other tech*/
  boolean  interf_active;

} nb1_cphy_irat_meas_timed_meas_rsp_param_s;

/*! The searched cells. */
typedef struct
{
  /*! This indicates if a search was perfromed */
  boolean search_performed;

  /*! The number of detected cells */
  uint8 num_detected_cells;

  /*! The detected cells from the cell search */
  nb1_cphy_irat_meas_detected_cell_s det_cells[ NB1_CPHY_IMEAS_MAX_DETECTED_CELLS];

  /*!Result is interfered by other tech*/
  boolean  interf_active;

} nb1_cphy_irat_meas_timed_search_rsp_param_s;

/*!  This structure provides information current state of a frequency
  */
typedef struct
{
  /* TODO fill this in when firmware parameters are available. */
  uint32 dummy;

} nb1_cphy_irat_meas_freq_state_info_s;

/*! @brief Timed IRAT Search or Measurement request

  This command can be used to trigger a timed search and/or meas on
  the NB1 timeline.  The timed search occurs in a window and must be of
  size greater than NB1_ML1_CPHY_IRAT_MEAS_GAP_MIN_SIZE

  NB1 will acquire the OTA resources at the beginning of the gap
  and relinquish them at the end of the gap

  Only one of these commands may be sent at a time, and the command must
  be sent 10ms before the OTA gap.

  Measurement will only be done on previously detected cells at this time.

  Note the source RAT STMR must be enabled prior to sending this command
  */
typedef struct
{

  msgr_hdr_struct_type hdr;

  /*! The frequency on which to tune to.
  In AGC + Meas gap, this freq correspond to the AGC freq
  range 0...65535
  */
  nb1_earfcn_t       freq;

  /*! Structure containing the source rat parameters
    to build the RF tuning scripts.
   */
  nb1_cphy_irat_meas_startup_rf_params_s rf_params;

  /*! Flag to indicate if the dl_bandwidth field is present */
  boolean dl_bandwith_present;

  /*! The downlink bandwith for the cell to be searched/measured */
  nb1_bandwidth_e  dl_bandwidth;

  /*! The accumulated timing drift from the source RAT stmr, between
    the last TIMED_SRCH_MEAS_REQ and this TIMED_SRCH_MEAS_REQ .

   Units are in NB1 RTC/OSTMR ticks, 30720 ticks/ ms which
   is equivalent to WCDMA cx8.

   */
  int32 timing_adjustment;

  /*! The gap of time allowed for NB1 search/Meas.

    The time must exceed NB1_ML1_CPHY_IRAT_MEAS_GAP_MIN_SIZE
   */
  nb1_cphy_irat_meas_gap_s gap;

  /*! Measurement parameters */
  nb1_cphy_irat_meas_timed_meas_param_s meas;

  /*! Search parameters */
  nb1_cphy_irat_meas_timed_search_param_s search;

  /*! TRUE if the freq_state array is populated and to be used */
  boolean freq_state_included;

  /*! The previous state of the frequency

   This should be passed back from the previous time_srch_meas_cnf.
   if none is available then freq_state_included should be set to FALSE
   */
  nb1_cphy_irat_meas_freq_state_info_s freq_state;

  /*! Virtual NB1 cell is the cell with the best SSS_CORR in current frequency.

     TRUE if the vcell info is present and the next gap is used for measurement.*/
  boolean vcell_info_present;

  /*! Virtual NB1 cell info, must be populated when vcell_info_present is TRUE.
    If vcell is present, gain state of current gap will be fixed based on
    vcell timing for collecting TDD measurement samples */
  nb1_cphy_irat_meas_cell_meas_state_s vcell_info;

  /*! Gap abort indicator, Allow NB1 L1 to internally skip the gap
      if there is insufficient time for gap scheduling.
      Source RAT will have to
      call the gap_end scheduling API to support gap skipping and RF script abort.
  */
  boolean support_gap_abort_before_tune;

} nb1_cphy_irat_meas_timed_srch_meas_req_s;

/*! @brief Timed IRAT Search or Measurement confirmation

  This command is in response to a timed search or measurement command.

  This command will be sent at the end of both of the search/meas
  following the gap.
  */
typedef struct
{

  msgr_hdr_struct_type hdr;

  /*! The frequency on which the search/AGC was performed
  range 0...65535
  */
  nb1_earfcn_t       freq;

  /*! Measured cell results

    If a cell is measured twice in the procedure it will only
    appear once with the most recent measurement result.
   */
  nb1_cphy_irat_meas_timed_meas_rsp_param_s meas;

  /*! Search results */
  nb1_cphy_irat_meas_timed_search_rsp_param_s search;

  /*! The state of the frequency */
  nb1_cphy_irat_meas_freq_state_info_s freq_state;

  /*! Boolean to indicate if the gap is aborted  */
  boolean gap_aborted;

} nb1_cphy_irat_meas_timed_srch_meas_cnf_s;

/*! @brief IRAT Measurement confirm, carrying the measurement results on
  the requested NB1 cells.

  */
typedef struct
{

  msgr_hdr_struct_type hdr;

  /*! The frequency which the cells reside on.
  range 0...65535
  */
  nb1_earfcn_t       freq;

  uint8 num_cells;

  /*! The cells which to measure on the specified frequency */
  nb1_cphy_irat_meas_meas_results_s cells[ NB1_CPHY_IMEAS_MAX_MEAS_CELLS];

  /*!Result is interfered by other tech*/
  boolean  interf_active;

} nb1_cphy_irat_meas_meas_cnf_s;

/*! @brief IRAT Measurement done indiciation.

  @details
  This indicates that NB1 L1 is done all online operations and the RF can be
  tuned and used for another RAT.  NB1 will still be active until the
  NB1_CPHY_IRAT_MEAS_CLEANUP_REQ is sent.

  */
typedef struct
{
  msgr_hdr_struct_type hdr;

} nb1_cphy_irat_meas_online_done_ind_s;

/*! @brief Aborts the ongoing search or measurement operation.

  @details
  It is possible that a search/meas confirm could still be received after
  sending this command due to the multithreaded nature of the system.

  */
typedef struct
{
  msgr_hdr_struct_type hdr;

} nb1_cphy_irat_meas_abort_req_s;

/*! @brief Confirms that any active search or measurement is aborted
  and no longer active.

  */
typedef struct
{
  msgr_hdr_struct_type hdr;

} nb1_cphy_irat_meas_abort_cnf_s;

/*@}*//* end cphy_irat_meas doxygen group */

enum
{

  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_F2L, REQ, INIT, 0x0,
                   nb1_cphy_irat_meas_init_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_F2L, CNF, INIT, 0x0,
                   nb1_cphy_irat_meas_init_cnf_s),

  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_F2L, REQ, DEINIT, 0x1,
                   nb1_cphy_irat_meas_deinit_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_F2L, CNF, DEINIT, 0x1,
                   nb1_cphy_irat_meas_deinit_cnf_s),

  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_F2L, CMD, STARTUP, 0x2,
                   nb1_cphy_irat_meas_startup_cmd_s),

  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_F2L, CMD, BLACKLIST, 0x3,
                   nb1_cphy_irat_meas_blacklist_cmd_s),

  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_F2L, REQ, SEARCH, 0x4,
                   nb1_cphy_irat_meas_search_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_F2L, CNF, SEARCH, 0x4,
                   nb1_cphy_irat_meas_search_cnf_s),

  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_F2L, REQ, MEAS, 0x5,
                   nb1_cphy_irat_meas_meas_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_F2L, CNF, MEAS, 0x5,
                   nb1_cphy_irat_meas_meas_cnf_s),

  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_F2L, REQ, CLEANUP, 0x6,
                   nb1_cphy_irat_meas_cleanup_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_F2L, CNF, CLEANUP, 0x6,
                   nb1_cphy_irat_meas_cleanup_cnf_s),

  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_F2L, REQ, ABORT, 0x7,
                   nb1_cphy_irat_meas_abort_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_F2L, CNF, ABORT, 0x7,
                   nb1_cphy_irat_meas_abort_cnf_s),

  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_F2L, REQ, TIMED_SRCH_MEAS, 0x8,
                   nb1_cphy_irat_meas_timed_srch_meas_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_F2L, CNF, TIMED_SRCH_MEAS, 0x8,
                   nb1_cphy_irat_meas_timed_srch_meas_cnf_s),

  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_W2L, REQ, INIT, 0x0,
                   nb1_cphy_irat_meas_init_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_W2L, CNF, INIT, 0x0,
                   nb1_cphy_irat_meas_init_cnf_s),

  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_W2L, REQ, DEINIT, 0x1,
                   nb1_cphy_irat_meas_deinit_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_W2L, CNF, DEINIT, 0x1,
                   nb1_cphy_irat_meas_deinit_cnf_s),

  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_W2L, CMD, STARTUP, 0x2,
                   nb1_cphy_irat_meas_startup_cmd_s),

  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_W2L, CMD, BLACKLIST, 0x3,
                   nb1_cphy_irat_meas_blacklist_cmd_s),

  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_W2L, REQ, SEARCH, 0x4,
                   nb1_cphy_irat_meas_search_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_W2L, CNF, SEARCH, 0x4,
                   nb1_cphy_irat_meas_search_cnf_s),

  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_W2L, REQ, MEAS, 0x5,
                   nb1_cphy_irat_meas_meas_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_W2L, CNF, MEAS, 0x5,
                   nb1_cphy_irat_meas_meas_cnf_s),

  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_W2L, REQ, CLEANUP, 0x6,
                   nb1_cphy_irat_meas_cleanup_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_W2L, CNF, CLEANUP, 0x6,
                   nb1_cphy_irat_meas_cleanup_cnf_s),

  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_W2L, REQ, ABORT, 0x7,
                   nb1_cphy_irat_meas_abort_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_W2L, CNF, ABORT, 0x7,
                   nb1_cphy_irat_meas_abort_cnf_s),

  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_W2L, REQ, TIMED_SRCH_MEAS, 0x8,
                   nb1_cphy_irat_meas_timed_srch_meas_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_W2L, CNF, TIMED_SRCH_MEAS, 0x8,
                   nb1_cphy_irat_meas_timed_srch_meas_cnf_s),

  /*--------------------------------------------------------*/
  /* T2L commands, requests & confirms */

  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_T2L, REQ, INIT, 0x0,
                   nb1_cphy_irat_meas_init_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_T2L, CNF, INIT, 0x0,
                   nb1_cphy_irat_meas_init_cnf_s),

  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_T2L, REQ, DEINIT, 0x1,
                   nb1_cphy_irat_meas_deinit_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_T2L, CNF, DEINIT, 0x1,
                   nb1_cphy_irat_meas_deinit_cnf_s),

  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_T2L, CMD, STARTUP, 0x2,
                   nb1_cphy_irat_meas_startup_cmd_s),

  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_T2L, CMD, BLACKLIST, 0x3,
                   nb1_cphy_irat_meas_blacklist_cmd_s),

  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_T2L, REQ, SEARCH, 0x4,
                   nb1_cphy_irat_meas_search_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_T2L, CNF, SEARCH, 0x4,
                   nb1_cphy_irat_meas_search_cnf_s),

  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_T2L, REQ, MEAS, 0x5,
                   nb1_cphy_irat_meas_meas_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_T2L, CNF, MEAS, 0x5,
                   nb1_cphy_irat_meas_meas_cnf_s),

  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_T2L, REQ, CLEANUP, 0x6,
                   nb1_cphy_irat_meas_cleanup_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_T2L, CNF, CLEANUP, 0x6,
                   nb1_cphy_irat_meas_cleanup_cnf_s),

  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_T2L, REQ, ABORT, 0x7,
                   nb1_cphy_irat_meas_abort_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_T2L, CNF, ABORT, 0x7,
                   nb1_cphy_irat_meas_abort_cnf_s),

  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_T2L, REQ, TIMED_SRCH_MEAS, 0x8,
                   nb1_cphy_irat_meas_timed_srch_meas_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_T2L, CNF, TIMED_SRCH_MEAS, 0x8,
                   nb1_cphy_irat_meas_timed_srch_meas_cnf_s),
  /*--------------------------------------------------------*/
  /* G2L commands, requests & confirms */

  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_G2L, REQ, INIT, 0x0,
                   nb1_cphy_irat_meas_init_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_G2L, CNF, INIT, 0x0,
                   nb1_cphy_irat_meas_init_cnf_s),

  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_G2L, REQ, DEINIT, 0x1,
                   nb1_cphy_irat_meas_deinit_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_G2L, CNF, DEINIT, 0x1,
                   nb1_cphy_irat_meas_deinit_cnf_s),

  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_G2L, CMD, STARTUP, 0x2,
                   nb1_cphy_irat_meas_startup_cmd_s),

  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_G2L, CMD, BLACKLIST, 0x3,
                   nb1_cphy_irat_meas_blacklist_cmd_s),

  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_G2L, REQ, SEARCH, 0x4,
                   nb1_cphy_irat_meas_search_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_G2L, CNF, SEARCH, 0x4,
                   nb1_cphy_irat_meas_search_cnf_s),

  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_G2L, REQ, MEAS, 0x5,
                   nb1_cphy_irat_meas_meas_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_G2L, CNF, MEAS, 0x5,
                   nb1_cphy_irat_meas_meas_cnf_s),

  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_G2L, REQ, CLEANUP, 0x6,
                   nb1_cphy_irat_meas_cleanup_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_G2L, CNF, CLEANUP, 0x6,
                   nb1_cphy_irat_meas_cleanup_cnf_s),

  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_G2L, REQ, ABORT, 0x7,
                   nb1_cphy_irat_meas_abort_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_G2L, CNF, ABORT, 0x7,
                   nb1_cphy_irat_meas_abort_cnf_s),

  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_G2L, REQ, TIMED_SRCH_MEAS, 0x8,
                   nb1_cphy_irat_meas_timed_srch_meas_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_G2L, CNF, TIMED_SRCH_MEAS, 0x8,
                   nb1_cphy_irat_meas_timed_srch_meas_cnf_s),

  /*--------------------------------------------------------*/
  /* D2L commands, requests & confirms */

  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_D2L, REQ, INIT, 0x0,
                   nb1_cphy_irat_meas_init_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_D2L, CNF, INIT, 0x0,
                   nb1_cphy_irat_meas_init_cnf_s),

  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_D2L, REQ, DEINIT, 0x1,
                   nb1_cphy_irat_meas_deinit_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_D2L, CNF, DEINIT, 0x1,
                   nb1_cphy_irat_meas_deinit_cnf_s),

  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_D2L, CMD, STARTUP, 0x2,
                   nb1_cphy_irat_meas_startup_cmd_s),

  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_D2L, CMD, BLACKLIST, 0x3,
                   nb1_cphy_irat_meas_blacklist_cmd_s),

  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_D2L, REQ, SEARCH, 0x4,
                   nb1_cphy_irat_meas_search_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_D2L, CNF, SEARCH, 0x4,
                   nb1_cphy_irat_meas_search_cnf_s),

  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_D2L, REQ, MEAS, 0x5,
                   nb1_cphy_irat_meas_meas_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_D2L, CNF, MEAS, 0x5,
                   nb1_cphy_irat_meas_meas_cnf_s),

  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_D2L, REQ, CLEANUP, 0x6,
                   nb1_cphy_irat_meas_cleanup_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_D2L, CNF, CLEANUP, 0x6,
                   nb1_cphy_irat_meas_cleanup_cnf_s),

  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_D2L, REQ, ABORT, 0x7,
                   nb1_cphy_irat_meas_abort_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_D2L, CNF, ABORT, 0x7,
                   nb1_cphy_irat_meas_abort_cnf_s),

  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_D2L, REQ, TIMED_SRCH_MEAS, 0x8,
                   nb1_cphy_irat_meas_timed_srch_meas_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_D2L, CNF, TIMED_SRCH_MEAS, 0x8,
                   nb1_cphy_irat_meas_timed_srch_meas_cnf_s),

  /*--------------------------------------------------------*/
  /* C2L commands, requests & confirms */

  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_C2L, REQ, INIT, 0x0,
                   nb1_cphy_irat_meas_init_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_C2L, CNF, INIT, 0x0,
                   nb1_cphy_irat_meas_init_cnf_s),

  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_C2L, REQ, DEINIT, 0x1,
                   nb1_cphy_irat_meas_deinit_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_C2L, CNF, DEINIT, 0x1,
                   nb1_cphy_irat_meas_deinit_cnf_s),

  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_C2L, CMD, STARTUP, 0x2,
                   nb1_cphy_irat_meas_startup_cmd_s),

  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_C2L, CMD, BLACKLIST, 0x3,
                   nb1_cphy_irat_meas_blacklist_cmd_s),

  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_C2L, REQ, SEARCH, 0x4,
                   nb1_cphy_irat_meas_search_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_C2L, CNF, SEARCH, 0x4,
                   nb1_cphy_irat_meas_search_cnf_s),

  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_C2L, REQ, MEAS, 0x5,
                   nb1_cphy_irat_meas_meas_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_C2L, CNF, MEAS, 0x5,
                   nb1_cphy_irat_meas_meas_cnf_s),

  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_C2L, REQ, CLEANUP, 0x6,
                   nb1_cphy_irat_meas_cleanup_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_C2L, CNF, CLEANUP, 0x6,
                   nb1_cphy_irat_meas_cleanup_cnf_s),

  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_C2L, REQ, ABORT, 0x7,
                   nb1_cphy_irat_meas_abort_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_C2L, CNF, ABORT, 0x7,
                   nb1_cphy_irat_meas_abort_cnf_s),

  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_C2L, REQ, TIMED_SRCH_MEAS, 0x8,
                   nb1_cphy_irat_meas_timed_srch_meas_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY_IRAT_MEAS_C2L, CNF, TIMED_SRCH_MEAS, 0x8,
                   nb1_cphy_irat_meas_timed_srch_meas_cnf_s),

};

#endif /* NB1_CPHY_IRAT_MEAS_MSG_H */

