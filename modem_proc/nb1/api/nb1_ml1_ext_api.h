/*!
  @file
  nb1_ml1_ext_api.h

  @brief
  This file describes the API interfaces to NB1 ML1
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

$Header: //components/rel/nb1.mpss/2.2/api/nb1_ml1_ext_api.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/

#ifndef NB1_ML1_EXT_API_H
#define NB1_ML1_EXT_API_H
#include "nb1_cphy_msg.h"
#include "nb1_cphy_rssi_msg.h"
#include "sys.h"
#include "nb1_as.h"
#include <lte_static_ml1_ext_api.h>
#include "lte_static_ml1_common.h"
#include "lte_rrc_ext_api.h"

/*===========================================================================

                           EXTERNAL TYPES FILES

===========================================================================*/

/*! Invalid timing advance */
#define NB1_ML1_GPS_MRL_INVALID_TIMING_ADVANCE 0xFFFF


/*! Maximum number of 8 neighbor cells + one serving cell in the MRL */
#define NB1_ML1_GPS_MRL_CELLS_MAX 9

/*! The invalid scheduler subframe time. The macro indicates that the
  current time is invalid; it can be returned from the systime query interface.
  It is also used internally within the scheduler for the initialization. */
#define NB1_ML1_SCHDLR_SYS_TIME_INVALID  0xFFFF

/*! SUPL's MeasurementResultsEUTRAType */
typedef struct
{
  /*! Reference signal - received power [0...97] in dBm */
  uint8      rsrp_result;
  /*! Reference signal - received quality [0...34] in dBm */
  uint8      rsrq_result;

} nb1_ml1_gps_mrl_rsrp_rsrq_t;

/*! SUPL's MeasuredResultsEUTRAType */
typedef struct
{
  /*! Physical cell id [0..503] */
  uint16                       phys_cell_id;
  /*! E-ARFCN [0..262143] */
  nb1_earfcn_t                 earfcn;
  /*! Timing advance [0...1282], 0xFFFF - Invalid */
  uint16                       timing_advance;
  /*! Measurement results */
  nb1_ml1_gps_mrl_rsrp_rsrq_t  z_rsrp_rsrq;

} nb1_ml1_gps_mrl_cell_meas_t;

/*! SUPL's MeasuredResultsListEUTRAType */
typedef struct
{
  /*! Number of cells in the MRL */
  uint8                        num_cells;
  /*! Whether the MRL TA is valid */
  boolean                      ta_valid;
  /*! MRL cell info */
  nb1_ml1_gps_mrl_cell_meas_t  cell_mrl_info[NB1_ML1_GPS_MRL_CELLS_MAX];

} nb1_ml1_gps_mrl_cell_meas_update_t;

/*! NB1->GPS event types */
typedef enum
{
  NB1_ML1_GPS_EVENT_CELL_MEAS_UPDATE,  /*!< MRL event type */
  NB1_ML1_GPS_EVENT_MAX,               /*!< Max event for range checking */

} nb1_ml1_gps_event_e;

/*! NB1->GPS message data type */
typedef struct
{
  uint8  *p_data;
  uint32  length;
} nb1_ml1_gps_msg_data_s;

/*! NB1->GPS message type */
typedef struct
{
  nb1_ml1_gps_event_e     e_event_type;
  nb1_ml1_gps_msg_data_s  z_l1_cgps_msg_data;
} nb1_ml1_gps_event_data_s;

/*! NB1->GPS event callback */
typedef boolean (*nb1_ml1_gps_event_cb_t)(nb1_ml1_gps_event_data_s *event_data_ptr);


/*! NB1 scan data per earfcn */
typedef struct
{
  nb1_earfcn_t  earfcn;
  uint8         bandwidth; /* 0..5 for (1.4)..20 MHz*/
  int8          rssi;
} nb1_ml1_scan_data_per_earfcn_s;


/*! @brief
    CM start scan capture indication structure
*/
typedef struct
{
  /*! Message router header */
  msgr_hdr_struct_type             msgr_hdr;

  /*! Pointer to memory for NB1 scan data */
  void                            *scan_data;

  /*! Memory size (in bytes) for NB1 scan data */
  uint16                           scan_data_mem_size;

}nb1_ml1_common_cm_scan_capture_start_ind_s;


/*! @brief
    CM stop scan capture indication structure
*/
typedef struct
{
  /*! Message router header */
  msgr_hdr_struct_type             msgr_hdr;
}nb1_ml1_common_cm_scan_capture_stop_ind_s;


/*! @brief
    CM stop scan capture cnf structure
*/
typedef struct
{
  /*! Message router header */
  msgr_hdr_struct_type             msgr_hdr;
}nb1_ml1_common_cm_scan_capture_stop_cnf_s;

/*! NB1 PMCH stats, for a single PMCH */
typedef struct nb1_pmch_info_s
{
  /*! PMCH ID */
  uint32   pmch_id;
  /*! Number of CRC passes */
  uint32   num_crc_pass;
  /*! Number of CRC failures */
  uint32   num_crc_fail;
} nb1_pmch_info_s_type;

/*! NB1 PMCH stats for an area */
typedef struct nb1_pmch_area_stats_s
{
  /*! Area ID */
  uint16                     area_id;
  /*! Number of PMCHs in this area */
  uint16                     num_pmch;
  /*! Stats for each PMCH */
  nb1_pmch_info_s_type     pmch_info[NB1_L1_MAX_PMCH_PER_MBSFN_AREA+1];
} nb1_pmch_area_stats_s_type;

/*! NB1 PMCH stats */
typedef struct nb1_pmch_stats_s
{
  /*! Indicates if there is any activated TMGI */
  boolean                      is_valid;
  /*! Number of areas */
  uint32                       num_area;
  /*! Stats for each area */
  nb1_pmch_area_stats_s_type pmch_area[NB1_EMBMS_MAX_MBSFN_AREA];
} nb1_pmch_stats_s_type;

/*!
  @brief
  This message indicates PMCH stats to upper layers
*/
typedef struct
{
  msgr_hdr_struct_type     msgr_hdr;

  /*! Structure to indicate PMCH stats */
  nb1_pmch_stats_s_type *pmch_stats;
} nb1_ml1_common_cm_pmch_stats_req_s;

/*! Structure to return allowed bandwidths in a band */
typedef struct {
  /*! Band */
  sys_sband_lte_e_type  band;
  /*! num allowed bandwidths */
  uint32                num_allowed_bandwidths;
  /*! allowed bandwidths */
  nb1_bandwidth_idx_e   allowed_bw_idx[NB1_MAX_NUM_BANDWIDTHS];
} nb1_ml1_common_band_bw_info_s;


typedef struct
{
  /*! RSRP of serving cell in Q0 dBm format */
    int32                             rsrp;
  /*! SINR of serving cell in  Q0 dB format */
    int16                             sinr; 
  /* time of last measurement */
    nb1_ml1_time_t                   time_of_last_meas;

}nb1_ml1_serv_cell_meas_rsrp_sinr_s;


/*===========================================================================

                        EXTERNAL FUNCTIONS

===========================================================================*/

/*===========================================================================

  FUNCTION:  nb1_ml1_sm_get_srv_rsrp_snr_report

===========================================================================*/
/*!
    @brief   

    This function is the external function to get serving cell measurement info     

    @return
    void

*/
/*=========================================================================*/


void nb1_ml1_sm_get_srv_rsrp_snr_report
(
  lte_rrc_rx_power_measurement_s_type* meas_cnf
);

/*===========================================================================

                        EXTERNAL FUNCTIONS

===========================================================================*/

/*===========================================================================

  FUNCTION:  nb1_ml1_sm_get_srv_rsrp_snr

===========================================================================*/
/*!
    @brief
    This function is the external RRC interface for requesting a serving cell RSRP and SINR.

    @return
    boolean: TRUE : RSRP and SINR values are valid
    FALSE: RSRP and SINR values are invalid.

*/
/*=========================================================================*/

boolean nb1_ml1_sm_get_srv_rsrp_snr
(
  nb1_ml1_serv_cell_meas_rsrp_sinr_s* meas_cnf
);

/*===========================================================================

  FUNCTION:  nb1_ml1_request_gps_timetag_int

===========================================================================*/
/*!
    @brief
    This function is the external GPS interface for requesting a timetag.

    @return
    None

*/
/*=========================================================================*/
void nb1_ml1_request_gps_timetag_int
(
  /*! Whether sleep estimation is allowed for timetag */
  boolean allow_sleep_timetag
);


/*===========================================================================

  FUNCTION:  nb1_ml1_request_gps_timetag

===========================================================================*/
/*!
    @brief
    This function is the external GPS interface for requesting an MRL.

    @return
    None

*/
/*=========================================================================*/
void nb1_ml1_request_gps_mrl( void );

/*===========================================================================

  FUNCTION:  nb1_ml1_request_gps_timexfer

===========================================================================*/
/*!
    @brief
    This function is the external GPS interface for requesting a time transfer.

    @return
    None

*/
/*=========================================================================*/
void nb1_ml1_request_gps_timexfer
(
  uint32 seq_num,                        /*!< Sequence number              */
  nb1_cphy_gps_timexfer_prio_e priority, /*!< Priority of the time transfer */
  boolean sib8_required,                 /*!< Whether SIB8 is required     */
  uint8 number_of_tt_samples             /*!< Number of samples requested */
);

/*===========================================================================

  FUNCTION:  nb1_ml1_request_gps_gts

===========================================================================*/
/*!
    @brief
    This function is the external GPS interface for requesting a gts.

    @return
    None

*/
/*=========================================================================*/
void nb1_ml1_request_gps_gts(void);

/*===========================================================================

  FUNCTION:  nb1_ml1_abort_gps_timexfer

===========================================================================*/
/*!
    @brief
    This function is the external GPS interface for aborting a time transfer.

    @return
    None

*/
/*=========================================================================*/
void nb1_ml1_abort_gps_timexfer
(
  uint32 seq_num                         /*!< Sequence number              */
);

/*===========================================================================

  FUNCTION:  nb1_ml1_request_gps_rxtx

===========================================================================*/
/*!
    @brief
    This function is the external GPS interface for requesting a RXTX value.

    @return
    None

*/
/*=========================================================================*/
void nb1_ml1_request_gps_rxtx
(
  void* p_Meas   /*!< Static GPS struct to be populated    */
);

/*===========================================================================

  FUNCTION:  nb1_ml1_gps_crs_start

===========================================================================*/
/*!
    @brief
    This function is the external GPS interface to start CRS reporting.

    @return
    None

*/
/*=========================================================================*/
void nb1_ml1_gps_opcrs_start(void);

/*===========================================================================

  FUNCTION:  nb1_ml1_gps_crs_stop

===========================================================================*/
/*!
    @brief
    This function is the external GPS interface to stop CRS reporting.

    @return
    None

*/
/*=========================================================================*/
void nb1_ml1_gps_opcrs_stop(void);

/*===========================================================================

  FUNCTION:  nb1_ml1_common_cm_nbr_rssi_ind_req_int

===========================================================================*/
/*!
    @brief
    Sends RSSI indication to upper layers.

    @return
    boolean.

*/
/*=========================================================================*/

void nb1_ml1_common_cm_nbr_rssi_ind_req_int(cmapi_lte_ngbr_ind_s *lte_ngbr_ind);

/*===========================================================================

  FUNCTION:  nb1_ml1_common_cm_rssi_ind_req_int

===========================================================================*/
/*!
    @brief
    Sends RSSI data to upper layers.

    @return
    Structure for RSSI Ind.

*/
/*=========================================================================*/
extern lte_api_signal_info_s_type nb1_ml1_common_cm_rssi_ind_req_int(void);


/*===========================================================================

  FUNCTION:  nb1_ml1_common_cmapi_ml1_mgr_req

===========================================================================*/
/*!
    @brief
    Sends ML1 MGR information to upper layers.

    @return
    Pointer to the ML1 MGR info.

*/
/*=========================================================================*/
nb1_ml1_mgr_cm_ml1_mgr_info_s* nb1_ml1_common_cmapi_ml1_mgr_req(void);

/*===========================================================================

FUNCTION:  nb1_ml1_common_cm_pmch_stats_req

===========================================================================*/
/*!
  @brief
  Sends PMCH stats to upper layers.

  @return
  void

 */
/*=========================================================================*/

void nb1_ml1_common_cm_pmch_stats_req(nb1_pmch_stats_s_type *pmch_stats);


/*===========================================================================

  FUNCTION:  nb1_ml1_common_cmapi_ml1_gm_req

===========================================================================*/
/*!
    @brief
    Sends ML1 GM information to upper layers.

    @return
    Pointer to the ML1 GM info.

*/
/*=========================================================================*/
nb1_ml1_gm_cm_ml1_gm_info_s* nb1_ml1_common_cmapi_ml1_gm_req(void);


/*===========================================================================

  FUNCTION:  nb1_ml1_start_capture_scan_data

===========================================================================*/
/*!
    @brief
    Trigger for NB1 ML1 to start capturing scan data
    First 32 bits of scan data memory will contain below information
    - First 16 bits: cmapi_sys_mode_e RAT type typecast to uint16
    - Second 16 bits: number of following 32 bit words filled by the RAT
    Subsequent 32 bit words will be filled with NB1 scan data as below
    {
        nb1_earfcn_t    earfcn;
        uint8           bandwidth;      //0..5 for (1.4)..20 MHz
        int8            rssi;
    }

    @return
    boolean: TRUE if capture is successfully started, FALSE otherwise (should
    never be the case, can use it later for any error cases)

*/
/*=========================================================================*/
extern boolean nb1_ml1_start_capture_scan_data
(
  void   *nb1_scan_data, /* Pointer to memory that will store NB1 scan data */
  uint16  nb1_scan_data_mem_size /* Size allocated for NB1 scan data (including 32 bits for RAT and number of results) */
);


/*===========================================================================

  FUNCTION:  nb1_ml1_stop_capture_scan_data

===========================================================================*/
/*!
    @brief
    Trigger for NB1 ML1 to stop capturing scan data

    @return
    boolean: TRUE if capture is successfully stopped, FALSE otherwise (should
    never be the case, can use it later for any error cases)

*/
/*=========================================================================*/
extern boolean nb1_ml1_stop_capture_scan_data
(
  void
);


/*===========================================================================

                        nb1_common_change_rx_diversity

===========================================================================*/
/*!
    @brief
    This function changes NB1 RX diversity

    @detail
    This function changes NB1 RX diversity.

    Note:
    1). this action code is different from the code provided by NB1 RF.
        NB1 ML1 will perform a translation of this
    2).NB1 RX diversity can only changed when NB1 RX is tuned, and RF S/W
       is not performing any other RF activities.

    @return
    TRUE:   if it is successfully changed
    FALSE:  if not.
     the failure cases include:
       -- RF S/W is busy and cannot take care of RX diversity change
           request
       -- RF S/W returns error when it performs the diversity changing
       -- NB1 is busy and cannot take the request

*/
/*=========================================================================*/
extern boolean nb1_common_change_rx_diversity
(
  boolean enable_rx0,   /* TRUE to enable RX0, FALSE to disable RX0 */
  boolean enable_rx1   /* TRUE to enable RX1, FALSE to disable RX1 */
);

/*===========================================================================

                        nb1_common_change_scc_rx_diversity

===========================================================================*/
/*!
    @brief
    This function changes NB1 RX diversity on a SCC

    @detail
    This function changes NB1 RX diversity on a SCC.

    Note:
    1). this action code is different from the code provided by NB1 RF.
        NB1 ML1 will perform a translation of this
    2).NB1 RX diversity can only changed when NB1 RX is tuned, and RF S/W
       is not performing any other RF activities.
       The following is the status code
          0: NB1_CPHY_RFCHAIN_REQ successfully processed
          1: ML1 is busy and can not process the command
          2: RF cannot process the command
          3: RF returns error
          4: general error


    @return
    TRUE:   if it is successfully changed
    FALSE:  if not.
     the failure cases include:
       -- RF S/W is busy and cannot take care of RX diversity change
           request
       -- RF S/W returns error when it performs the diversity changing
       -- NB1 is busy and cannot take the request

*/
/*=========================================================================*/
extern boolean nb1_common_change_scc_rx_diversity
(
  uint8   scc_id,      /* SCC index, starting from 0 */
  boolean enable_rx0,  /* TRUE to enable RX0, FALSE to disable RX0 */
  boolean enable_rx1   /* TRUE to enable RX1, FALSE to disable RX1 */
);

/*===========================================================================

  FUNCTION:  nb1_ml1_common_clock_config

===========================================================================*/
/*!
    @brief
    Configures the hw, including clock setting

    @details
    Configures the hw, including clock setting

    @return
    None

*/
/*=========================================================================*/
extern void nb1_ml1_common_clock_config ( void );

/*===========================================================================

  FUNCTION:  nb1_ml1_pos_prs_start

===========================================================================*/
/*!
    @brief
    This function sends a start PRS request to ML1.

    @return
    None

*/
/*=========================================================================*/
void nb1_ml1_prs_start
(
  /*! PRS start request information */
  nb1_cphy_prs_start_req_s *prs_start_req
);

/*===========================================================================

  FUNCTION:  nb1_ml1_prs_update_cells

===========================================================================*/
/*!
    @brief
    This function sends a PRS update cells request to ML1.

    @return
    None

*/
/*=========================================================================*/
void nb1_ml1_prs_update_cells
(
  /*! PRS update cells request information */
  nb1_cphy_prs_update_cells_req_s *prs_update_cells_req
);

/*===========================================================================

  FUNCTION:  nb1_ml1_prs_stop

===========================================================================*/
/*!
    @brief
    This function sends a PRS stop request to ML1.

    @return
    None

*/
/*=========================================================================*/
void nb1_ml1_prs_stop
(
  /*! PRS stop request information */
  nb1_cphy_prs_stop_req_s *prs_stop_req
);


/*===========================================================================

  FUNCTION:  nb1_ml1_pos_prs_stop_pbch_decode

===========================================================================*/
/*!
    @brief
    This function sends a PBCH decode stop request to the SM.

    @return
    None

*/
/*=========================================================================*/
void prs_ml1_pos_prs_stop_pbch_decode( void );

/*===========================================================================

  FUNCTION:  nb1_ml1_pos_prs_inter_frequency_capable

===========================================================================*/
/*!
    @brief
    This function returns whether the PRS module is capable of processing
    inter-frequency cells

    @return
    Whether PRS module is capable of processing inter-frequency cells.

*/
/*=========================================================================*/
boolean nb1_ml1_pos_prs_inter_frequency_capable( void );

/*===========================================================================

  FUNCTION:  nb1_ml1_schdlr_get_sys_sfn

===========================================================================*/
/*!
    @brief
    The funciton returns the current systime frame number

    @detail
    This will be used by an external API, dont use Mutex
    The SFN value will be valid only when NB1 ML1 manager has acquired on
    a cell.

    @return
    systime frame number

*/
/*=========================================================================*/
uint32 nb1_ml1_schdlr_get_sys_sfn(void);

/*==============================================================================

  FUNCTION:  nb1_ml1_convert_nb1_band_to_sys_band

==============================================================================*/
/*!
  @brief
  Converts nb1 band to Sys band enum

  @detail

  @return
  Sys Band enum
*/
/*============================================================================*/
sys_band_class_e_type nb1_ml1_convert_nb1_band_to_sys_band(sys_sband_lte_e_type nb1_band);

/*==============================================================================

  FUNCTION:  nb1_ml1_common_band_earfcn_bw_is valid

==============================================================================*/
/*!
  @brief
  Validate that for a given earfcn, the UE does not access channels outside
  the band range.

  @detail
  Validate that for a given earfcn, the UE does not access channels outside
  the band range. This is to prevent illegal band access when a UE is given
  a bad earfcn on handover. Validates the dl earfcn since the ul earfcn should
  mirror the bandwidth usage.

  @return
  TRUE if the UE is restricted to the band it is in.
*/
/*============================================================================*/
boolean nb1_ml1_common_band_earfcn_bw_is_valid
(
  nb1_earfcn_t   dl_earfcn,
  nb1_bandwidth_e dl_nrbs
);

/*==============================================================================

  FUNCTION:  nb1_ml1_common_band_get_allowed_bw_in_band

==============================================================================*/
/*!
  @brief
  Checks if passed in band is valid and returns allowed BW

  @detail
  Checks is passed in band index is a valid NB1 band. If it is, function returns
  the allowed BW in the band

  @return
  True if passed in band is valid, FALSE otherwise
*/
/*============================================================================*/
boolean
nb1_ml1_common_band_get_allowed_bw_in_band(
  /*! Band */
  sys_sband_lte_e_type   band_id,
  /*! Ptr to allowed bands data structure */
  nb1_ml1_common_band_bw_info_s  * band_bw_info_ptr
);

/*==============================================================================

  FUNCTION:  nb1_ml1_common_band_get_band_from_dl_earfcn

==============================================================================*/
/*!
  @brief
  Checks if passed in DL earfcn is valid and returns the band number

  @detail
  Checks is passed in DL earfcn is a valid value. If it is, function returns
  the band number

  @return
  True if passed in DL earfcn is valid, FALSE otherwise
*/
/*============================================================================*/
boolean
nb1_ml1_common_band_get_band_from_dl_earfcn(
  /*! DL earfcn */
  nb1_earfcn_t  dl_earfcn,
  /*! Ptr to band number */
  sys_sband_lte_e_type  * band_ptr
);

/*===========================================================================

  FUNCTION:  nb1_ml1_common_cmapi_get_doppler_measurement

===========================================================================*/
/*!
    @brief
    Get doppler measurement of primary serving cell passed by FW.

    @return
    An uint16 value between 0 to 400. Return NB1_ML_MDB_INVALID_DOPPLER_VALUE
    (0xffff) if no measuring is done yet.

*/
/*=========================================================================*/
uint16 nb1_ml1_common_cmapi_get_PCC_doppler_measurement(void);

/*===========================================================================

  FUNCTION:  nb1_ml1_get_test_bsr_val

===========================================================================*/
/*!
    @brief
    Log messages to create a trace for NB1 stack call flows

    @return
    None.

*/
/*=========================================================================*/
void nb1_ml1_log_msgr_msg
(
  uint8                 inst,
  msgr_umid_type        id,
  msgr_tech_module_type from,
  msgr_tech_module_type to
);

/*===========================================================================

  FUNCTION:  nb1_ml1_schdlr_current_time_external

===========================================================================*/
/*!
    @brief
    This function returns the current absolute sf for external clients

    @return
    current
 
    @note
    This function is only ever called externally so no semaphore
    reader is used.
*/
/*=========================================================================*/
void nb1_ml1_schdlr_current_time_external
(
  nb1_cphy_cell_info_s *     cell_info,
  nb1_l1_sfn_t *             current_sfn,
  nb1_l1_hyper_sfn_t *       current_hyper_sfn
);
/*===========================================================================

  FUNCTION:  nb1_ml1_sleepmgr_get_remaining_sleep_dur

===========================================================================*/
/*!
    @brief
    This function returns remaining sleep duration.

    @return
    remaining sleep duration in seconds 

*/
/*=========================================================================*/
uint32 nb1_ml1_sleepmgr_get_remaining_sleep_dur(void);

/*===========================================================================

  FUNCTION:  nb1_ml1_sleepmgr_get_remaining_sleep_dur_in_sec_ms

===========================================================================*/
/*!
    @brief
    This function returns remaining sleep duration.

    @return
    remaining sleep duration in seconds and milli seconds
*/
/*=========================================================================*/
void nb1_ml1_sleepmgr_get_remaining_sleep_dur_in_sec_ms
(
 uint32* remaining_dur_in_sec,
 uint64* remaining_dur_in_ms
);

/*===========================================================================

  FUNCTION:  nb1_ml1_get_nb1_vtbl

===========================================================================*/
/*!
  @brief
  Returns a pointer to the NB1 vtbl of externally exported APIs

  @return
  void 
*/
/*=========================================================================*/
lte_static_ml1_ext_api_s *nb1_ml1_get_nb1_vtbl
(
  void
);
/*===========================================================================

  FUNCTION:  nb1_ml1_sleepmgr_is_edrx_sleep

===========================================================================*/
/*!
    @brief
    This function returns whether current sleep is edrx or not

    @return
    Boolean - Whether the sleep is edrx
*/
/*=========================================================================*/
boolean nb1_ml1_sleepmgr_is_edrx_sleep(void);



/*===========================================================================

  FUNCTION:  check_intra_panic

===========================================================================*/
/*!
    @brief
  Checks if intra panic mode is running

    @detail


    @return
    boolean
*/
/*=========================================================================*/
boolean
nb1_ml1_mgmt_db_check_intra_panic
(
);

/*===========================================================================
  FUNCTION:  nb1_ml1_sm_idle_mgr_stm_can_rach_be_suspended_for_srch

===========================================================================*/
/*!
  @brief
 checks if RACH can be suspended to prioritize search

  @return
  none
*/
/*=========================================================================*/
boolean
nb1_ml1_sm_idle_mgr_stm_can_rach_be_suspended_for_srch();

/*===========================================================================

  FUNCTION:  nb1_ml1_mgr_handle_init_req

===========================================================================*/
/*!
  @brief
  Initializes MGR task modules for NB1

  @return
*/
/*=========================================================================*/
void nb1_ml1_mgr_handle_init_req
(
  void
);

/*===========================================================================

  FUNCTION:  nb1_ml1_mgr_handle_deinit_req

===========================================================================*/
/*!
  @brief
  Deinitializes MGR task modules for NB1

  @return
*/
/*=========================================================================*/
void nb1_ml1_mgr_handle_deinit_req
(
  void
);

/*===========================================================================

  FUNCTION:  nb1_ml1_gm_handle_init_req

===========================================================================*/
/*!
  @brief
  Main look for GM module

  @return
  None (should not return)
*/
/*=========================================================================*/

void nb1_ml1_gm_handle_init_req
(
  void
);

/*===========================================================================

  FUNCTION:  nb1_ml1_gm_handle_deinit_req

===========================================================================*/
/*!
  @brief
  ML1 Grant Manager Task Tear down

  @return
  TRUE if tear down is successful
*/
/*=========================================================================*/
void nb1_ml1_gm_handle_deinit_req
( 
  void
);

/*===========================================================================

   FUNCTION: nb1_ns_check_ns_mode

===========================================================================*/
/*!
   @brief
   Checks if we are in NS mode

   @detail

   @return
   Result -- True or false.
*/
/*=========================================================================*/
boolean nb1_ns_check_ns_mode( );

/*===========================================================================

  FUNCTION:  nb1_ml1_common_rssi_cmapi_update_tx_global

===========================================================================*/
/*!
    @brief
    Pushes Tx power info to CMAPI supplied global variable

    @return
    None

*/
/*===========================================================================*/
void nb1_ml1_common_rssi_cmapi_update_tx_global(void);

/*===========================================================================

  FUNCTION:  nb1_ml1_offload_handle_init_req

===========================================================================*/
/*!
  @brief
  Main loop for OFFLOAD module

  @return
  NULL pointer
*/
/*=========================================================================*/
void nb1_ml1_offload_handle_init_req
(
  void
);

/*===========================================================================

  FUNCTION:  nb1_ml1_offload_handle_deinit_req

===========================================================================*/
/*!
  @brief
  ML1 offload Task Tear down

  @return
  TRUE if tear down is successful
*/
/*=========================================================================*/
void nb1_ml1_offload_handle_deinit_req(void);

extern boolean nb1_poi_get_psm_db_band_info
(
   lte_rrc_static_psm_db_band_info_type_s  *lte_band_info
);

/*===========================================================================

  FUNCTION:  nb1_poi_get_psm_nghbr_band_info

===========================================================================*/
/*!
    @brief
    This function provide RF PSM db band information

    @return
    boolean

*/
/*=========================================================================*/

extern boolean nb1_poi_get_psm_nghbr_band_info
(
   lte_ml1_static_psm_db_band_info_type_s  *lte_band_info
);


/*===========================================================================

  FUNCTION:  nb1_ml1_mgmt_db_get_scell_measure_result()

===========================================================================*/
/*!
  @brief
  RSRP and RSRQ of serving cell are required to report to network in RRC MSG5 for network maintenance and optimization.
  So RRC calls this interface to get the mapped RSRP and RSRQ values.

  The accuracy of NRSRP and NRSRQ measurements in MSG5 is as specified in TS36.133 
      Table 9.1.22.9-1: NRSRP measurement report mapping
      Table 9.1.24-1: NRSRQ measurement report mapping

  @return
  NULL pointer
*/
/*=========================================================================*/
void nb1_ml1_mgmt_db_get_scell_measure_result
(
uint8 *p_reportedRSRP,
int8 *p_reportedRSRQ
);

/*===========================================================================

  FUNCTION:  nb1_ml1_ulm_validate_prach_cfg_params_local

===========================================================================*/
/*!
    @brief
    This function validates PRACH configuration parameters.

    @dependencies
    None.

    @sideeffects
    None.

    @return
    TRUE if validation passes, FALSE otherwise
*/
/*=========================================================================*/
boolean nb1_ml1_ulm_validate_prach_cfg_params_local
(nb1_cphy_prach_setup_param_s const *cfg_ptr,
 nb1_mem_instance_type instance);

/*===========================================================================

  FUNCTION:  nb1_ml1_get_custom_duplex_spacing_enabled_for_earfcn

===========================================================================*/
/*!
    @brief
    Returns whether custom duplex spacing is enabled for this particular earfcn

    @return
    boolean
*/
/*=========================================================================*/
boolean nb1_ml1_get_custom_duplex_spacing_applied_for_earfcn
(
  nb1_mem_instance_type instance,
  nb1_earfcn_t          earfcn
);

#endif /* NB1_ML1_EXT_API_H */

