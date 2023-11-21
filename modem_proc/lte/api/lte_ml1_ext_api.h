/*!
  @file
  lte_ml1_ext_api.h

  @brief
  This file describes the API interfaces to LTE ML1
*/


/*===========================================================================

  Copyright (c) 2010-2012 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //components/rel/lte.mpss/4.7.2/api/lte_ml1_ext_api.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
05/09/12   ns      Add new API for generic ML1 and GM CMAPI
05/25/11   tjc     Added PRS interface
11/29/10   fhu     Added clock config function here for RF FTM to use
07/09/10   tjc     Add GPS MRL interface
03/26/10   tjc     Initial version
===========================================================================*/

#ifndef LTE_ML1_EXT_API_H
#define LTE_ML1_EXT_API_H
#include "lte_cphy_msg.h"
#include "lte_cphy_rssi_msg.h" 
#include "sys.h"
#include "lte_as.h"
#include "lte_rrc_ext_msg.h"

/*===========================================================================

                           EXTERNAL TYPES FILES

===========================================================================*/

/*! Invalid timing advance */
#define LTE_ML1_GPS_MRL_INVALID_TIMING_ADVANCE 0xFFFF  


/*! Maximum number of 8 neighbor cells + one serving cell in the MRL */
#define LTE_ML1_GPS_MRL_CELLS_MAX 9

/*! The invalid scheduler subframe time. The macro indicates that the
  current time is invalid; it can be returned from the systime query interface.
  It is also used internally within the scheduler for the initialization. */
#define LTE_ML1_SCHDLR_SYS_TIME_INVALID  0xFFFF

#define LTE_ML1_MAX_PSM_ACQ_DB_BANDS     8

typedef struct
{
  /*! Number of bands included in NB1/CATM1 PSM neighbor db */
  uint8                                        num_bands; 
  /*! The bands information in the NB1/CATM1 PSM db */
  sys_sband_lte_e_type                         bands[LTE_ML1_MAX_PSM_ACQ_DB_BANDS];   
}lte_ml1_static_psm_db_band_info_type_s;


/*! SUPL's MeasurementResultsEUTRAType */
typedef struct 
{
  /*! Reference signal - received power [0...97] in dBm */
  uint8      rsrp_result;
  /*! Reference signal - received quality [0...34] in dBm */
  uint8      rsrq_result;

} lte_ml1_gps_mrl_rsrp_rsrq_t;

/*! SUPL's MeasuredResultsEUTRAType */
typedef struct
{
  /*! Physical cell id [0..503] */
  uint16                       phys_cell_id;
  /*! E-ARFCN [0..262143] */
  lte_earfcn_t                 earfcn;
  /*! Timing advance [0...1282], 0xFFFF - Invalid */
  uint16                       timing_advance;
  /*! Measurement results */
  lte_ml1_gps_mrl_rsrp_rsrq_t  z_rsrp_rsrq;

} lte_ml1_gps_mrl_cell_meas_t;  

/*! SUPL's MeasuredResultsListEUTRAType */
typedef struct
{
  /*! Number of cells in the MRL */
  uint8                        num_cells;
  /*! Whether the MRL TA is valid */
  boolean                      ta_valid;
  /*! MRL cell info */
  lte_ml1_gps_mrl_cell_meas_t  cell_mrl_info[LTE_ML1_GPS_MRL_CELLS_MAX];

} lte_ml1_gps_mrl_cell_meas_update_t;

/*! LTE scan data per earfcn */
typedef struct
{
  lte_earfcn_t  earfcn;
  uint8         bandwidth; /* 0..5 for (1.4)..20 MHz*/
  int8          rssi;
} lte_ml1_scan_data_per_earfcn_s;


/*! @brief
    CM start scan capture indication structure
*/
typedef struct
{
  /*! Message router header */
  msgr_hdr_struct_type             msgr_hdr;

  /*! Pointer to memory for LTE scan data */
  void                            *scan_data;

  /*! Memory size (in bytes) for LTE scan data */
  uint16                           scan_data_mem_size;

}lte_ml1_common_cm_scan_capture_start_ind_s;


/*! @brief
    CM stop scan capture indication structure
*/
typedef struct
{
  /*! Message router header */
  msgr_hdr_struct_type             msgr_hdr;
}lte_ml1_common_cm_scan_capture_stop_ind_s;


/*! @brief
    CM stop scan capture cnf structure
*/
typedef struct
{
  /*! Message router header */
  msgr_hdr_struct_type             msgr_hdr;
}lte_ml1_common_cm_scan_capture_stop_cnf_s;


/*! LTE Cost of modem power */
typedef enum
{
  LTE_ML1_TX_PWR_COST_LOW,
  LTE_ML1_TX_PWR_COST_MEDIUM,
  LTE_ML1_TX_PWR_COST_HIGH
} lte_ml1_tx_pwr_cost_e;


/*! Structure for uplink stats */
typedef struct 
{
  /* Cost of tx power */
  lte_ml1_tx_pwr_cost_e  tx_pwr_cost;

  /* Plus flag */
  boolean                plus_flag;
} lte_ml1_ul_stats_s;

/*! LTE PMCH stats, for a single PMCH */
typedef struct lte_pmch_info_s
{
  /*! PMCH ID */
  uint32   pmch_id;
  /*! Number of CRC passes */
  uint32   num_crc_pass;
  /*! Number of CRC failures */
  uint32   num_crc_fail;
} lte_pmch_info_s_type;

/*! LTE PMCH stats for an area */
typedef struct lte_pmch_area_stats_s
{
  /*! Area ID */
  uint16                     area_id;
  /*! Number of PMCHs in this area */
  uint16                     num_pmch;
  /*! Stats for each PMCH */
  lte_pmch_info_s_type     pmch_info[LTE_L1_MAX_PMCH_PER_MBSFN_AREA+1];
} lte_pmch_area_stats_s_type;

/*! LTE PMCH stats */
typedef struct lte_pmch_stats_s
{
  /*! Indicates if there is any activated TMGI */
  boolean                      is_valid;
  /*! Number of areas */
  uint32                       num_area;
  /*! Stats for each area */
  lte_pmch_area_stats_s_type pmch_area[LTE_EMBMS_MAX_MBSFN_AREA];
} lte_pmch_stats_s_type;

/*!
  @brief
  This message indicates PMCH stats to upper layers
*/
typedef struct
{
  msgr_hdr_struct_type     msgr_hdr;
  
  /*! Structure to indicate PMCH stats */
  lte_pmch_stats_s_type *pmch_stats;
} lte_ml1_common_cm_pmch_stats_req_s;

/*! Structure to return allowed bandwidths in a band */
typedef struct {
  /*! Band */
  sys_sband_lte_e_type  band;
  /*! num allowed bandwidths */
  uint32                num_allowed_bandwidths;
  /*! allowed bandwidths */
  lte_bandwidth_idx_e   allowed_bw_idx[LTE_MAX_NUM_BANDWIDTHS];
} lte_ml1_common_band_bw_info_s;

typedef struct
{
  /*! RSRP of serving cell in Q0 format */
    int32                             rsrp;
  /*! SINR of serving cell in  dB format */
    int16                             sinr; 

    lte_cphy_conn_meas_rsrp_t         rsrp_measurement;

   /*! Time of last meas */
   lte_ml1_time_t                   time_of_last_meas;
    
}lte_ml1_serv_cell_meas_rsrp_sinr_s;
typedef void (*lte_ds_jammer_detect_cback_type)(sys_radio_access_tech_e_type rat, uint32 jamming_status, boolean soft_jamming_status);


/*===========================================================================

                        EXTERNAL FUNCTIONS

===========================================================================*/



/*===========================================================================

  FUNCTION:  lte_ml1_sm_get_srv_rsrp_snr_report

===========================================================================*/
/*!
    @brief   

    This function is the external function to get serving cell measurement info     

    @return
    void

*/
/*=========================================================================*/

void lte_ml1_sm_get_srv_rsrp_snr_report
(
  lte_rrc_rx_power_measurement_s_type *meas_cnf
);

/*===========================================================================

                        EXTERNAL FUNCTIONS

===========================================================================*/



/*===========================================================================

  FUNCTION:  lte_ml1_sm_get_srv_rsrp_snr

===========================================================================*/
/*!
    @brief   

     This function is the external RRC interface for requesting a serving cell RSRP in Q0 in and SINR in dB.

    @return
    boolean: TRUE : RSRP and SINR values are valid
    FALSE: RSRP and SINR values are invalid.

*/
/*=========================================================================*/

boolean lte_ml1_sm_get_srv_rsrp_snr
(
  lte_ml1_serv_cell_meas_rsrp_sinr_s *meas_cnf
);

/*===========================================================================

  FUNCTION:  lte_ml1_request_ul_stats

===========================================================================*/
/*!
    @brief
    This function is the external function to get UL statistics

    @return
    TRUE if LTE is in connected/FALSE otherwise
    (UL stats are valid only when TRUE is returned)

*/
/*=========================================================================*/
boolean lte_ml1_request_ul_stats
(
  /*! LTE UL stats */
  lte_ml1_ul_stats_s *ul_stats
);

/*===========================================================================

  FUNCTION:  lte_ml1_request_gps_timetag_int

===========================================================================*/
/*!
    @brief
    This function is the external GPS interface for requesting a timetag.

    @return
    None

*/
/*=========================================================================*/
void lte_ml1_request_gps_timetag_int
(
  /*! Whether sleep estimation is allowed for timetag */
  boolean allow_sleep_timetag
);

/*===========================================================================

  FUNCTION:  lte_ml1_request_gps_timetag

===========================================================================*/
/*!
    @brief
    This function is the external GPS interface for requesting a timetag.

    @return
    None

*/
/*=========================================================================*/
void lte_ml1_request_gps_timetag
(
  /*! Whether sleep estimation is allowed for timetag */
  boolean allow_sleep_timetag
);


/*===========================================================================

  FUNCTION:  lte_ml1_request_gps_timetag

===========================================================================*/
/*!
    @brief
    This function is the external GPS interface for requesting an MRL.

    @return
    None

*/
/*=========================================================================*/
void lte_ml1_request_gps_mrl( void );

/*===========================================================================

  FUNCTION:  lte_ml1_register_gps_event_cb

===========================================================================*/
/*!
    @brief
    This function is the external GPS interface for registering and event
    callback.

    @return
    None

*/
/*=========================================================================*/
void lte_ml1_register_gps_event_cb( lte_ml1_gps_event_cb_t event_cb );
void __lte_ml1_register_gps_event_cb( lte_ml1_gps_event_cb_t event_cb );


/*===========================================================================

  FUNCTION:  lte_ml1_request_gps_timexfer

===========================================================================*/
/*!
    @brief
    This function is the external GPS interface for requesting a time transfer.

    @return
    None

*/
/*=========================================================================*/
void lte_ml1_request_gps_timexfer
(
  uint32 seq_num,                        /*!< Sequence number              */
  lte_cphy_gps_timexfer_prio_e priority, /*!< Priority of the time transfer */
  boolean sib8_required,                 /*!< Whether SIB8 is required     */
  uint8 number_of_tt_samples             /*!< Number of samples requested */
);

/*===========================================================================

  FUNCTION:  lte_ml1_request_gps_gts

===========================================================================*/
/*!
    @brief
    This function is the external GPS interface for requesting a gts.

    @return
    None

*/
/*=========================================================================*/
void lte_ml1_request_gps_gts(void);

/*===========================================================================

  FUNCTION:  lte_ml1_send_gts_req

===========================================================================*/
/*!
    @brief
    This function is to send a gts request to GTS module.

    @return
    None

*/
/*=========================================================================*/
void lte_ml1_send_gts_req(void);

/*===========================================================================

  FUNCTION:  lte_ml1_abort_gps_timexfer

===========================================================================*/
/*!
    @brief
    This function is the external GPS interface for aborting a time transfer.

    @return
    None

*/
/*=========================================================================*/
void lte_ml1_abort_gps_timexfer
(
  uint32 seq_num                         /*!< Sequence number              */
);

/*===========================================================================

  FUNCTION:  lte_ml1_pos_ustmr_to_ostmr

===========================================================================*/
/*!
    @brief
    This function is the external GPS interface for converting ustmr to ostmr.

    @return
    None

*/
/*=========================================================================*/
void lte_ml1_pos_ustmr_to_ostmr
(
  uint32 ustmr,                        /*!< USTMR              */
  uint32* ostmr,                       /*!< OSTMR              */
  uint32* ostmr_ph                     /*!< OSTMR fractional   */
);

/*===========================================================================

  FUNCTION:  lte_ml1_request_gps_rxtx

===========================================================================*/
/*!
    @brief
    This function is the external GPS interface for requesting a RXTX value.

    @return
    None

*/
/*=========================================================================*/
void lte_ml1_request_gps_rxtx
(
  void* p_Meas   /*!< Static GPS struct to be populated    */
);

/*===========================================================================

  FUNCTION:  lte_ml1_gps_crs_start

===========================================================================*/
/*!
    @brief
    This function is the external GPS interface to start CRS reporting.

    @return
    None

*/
/*=========================================================================*/
void lte_ml1_gps_opcrs_start(void);

/*===========================================================================

  FUNCTION:  lte_ml1_gps_crs_stop

===========================================================================*/
/*!
    @brief
    This function is the external GPS interface to stop CRS reporting.

    @return
    None

*/
/*=========================================================================*/
void lte_ml1_gps_opcrs_stop(void);

/*===========================================================================

  FUNCTION:  lte_ml1_common_cm_nbr_rssi_ind_req_int

===========================================================================*/
/*!
    @brief
    Sends RSSI indication to upper layers.

    @return
    boolean.

*/
/*=========================================================================*/

void lte_ml1_common_cm_nbr_rssi_ind_req_int(cmapi_lte_ngbr_ind_s *lte_ngbr_ind);



/*===========================================================================

  FUNCTION:  lte_ml1_common_cm_rssi_ind_req_int

===========================================================================*/
/*!
    @brief
    Sends RSSI data to upper layers.

    @return
    Structure for RSSI Ind.

*/
/*=========================================================================*/
extern lte_api_signal_info_s_type lte_ml1_common_cm_rssi_ind_req_int(void);


/*===========================================================================

  FUNCTION:  lte_ml1_common_cmapi_ml1_mgr_req

===========================================================================*/
/*!
    @brief
    Sends ML1 MGR information to upper layers.

    @return
    Pointer to the ML1 MGR info.

*/
/*=========================================================================*/
lte_ml1_mgr_cm_ml1_mgr_info_s* lte_ml1_common_cmapi_ml1_mgr_req(void);

/*===========================================================================

FUNCTION:  lte_ml1_common_cm_pmch_stats_req

===========================================================================*/
/*!
  @brief
  Sends PMCH stats to upper layers.

  @return
  void

 */
/*=========================================================================*/

void lte_ml1_common_cm_pmch_stats_req(lte_pmch_stats_s_type *pmch_stats);


/*===========================================================================

  FUNCTION:  lte_ml1_common_cmapi_ml1_gm_req

===========================================================================*/
/*!
    @brief
    Sends ML1 GM information to upper layers.

    @return
    Pointer to the ML1 GM info.

*/
/*=========================================================================*/
lte_ml1_gm_cm_ml1_gm_info_s* lte_ml1_common_cmapi_ml1_gm_req(void);


/*===========================================================================

  FUNCTION:  lte_ml1_start_capture_scan_data

===========================================================================*/
/*!
    @brief
    Trigger for LTE ML1 to start capturing scan data
    First 32 bits of scan data memory will contain below information
    - First 16 bits: cmapi_sys_mode_e RAT type typecast to uint16
    - Second 16 bits: number of following 32 bit words filled by the RAT
    Subsequent 32 bit words will be filled with LTE scan data as below
    {
        lte_earfcn_t    earfcn;
        uint8           bandwidth;      //0..5 for (1.4)..20 MHz
        int8            rssi;
    }

    @return
    boolean: TRUE if capture is successfully started, FALSE otherwise (should
    never be the case, can use it later for any error cases)

*/
/*=========================================================================*/
extern boolean lte_ml1_start_capture_scan_data
(
  void   *lte_scan_data, /* Pointer to memory that will store LTE scan data */
  uint16  lte_scan_data_mem_size /* Size allocated for LTE scan data (including 32 bits for RAT and number of results) */
);


/*===========================================================================

  FUNCTION:  lte_ml1_stop_capture_scan_data

===========================================================================*/
/*!
    @brief
    Trigger for LTE ML1 to stop capturing scan data

    @return
    boolean: TRUE if capture is successfully stopped, FALSE otherwise (should
    never be the case, can use it later for any error cases)

*/
/*=========================================================================*/
extern boolean lte_ml1_stop_capture_scan_data
(
  void
);


/*===========================================================================

                        lte_common_change_rx_diversity

===========================================================================*/
/*!
    @brief
    This function changes LTE RX diversity

    @detail
    This function changes LTE RX diversity.
 
    Note:
    1). this action code is different from the code provided by LTE RF.
        LTE ML1 will perform a translation of this
    2).LTE RX diversity can only changed when LTE RX is tuned, and RF S/W
       is not performing any other RF activities.

    @return
    TRUE:   if it is successfully changed
    FALSE:  if not.
     the failure cases include:
       -- RF S/W is busy and cannot take care of RX diversity change
           request
       -- RF S/W returns error when it performs the diversity changing
       -- LTE is busy and cannot take the request 

*/
/*=========================================================================*/
extern boolean lte_common_change_rx_diversity
(
  boolean enable_rx0,   /* TRUE to enable RX0, FALSE to disable RX0 */
  boolean enable_rx1   /* TRUE to enable RX1, FALSE to disable RX1 */
);

/*===========================================================================

                        lte_common_change_scc_rx_diversity

===========================================================================*/
/*!
    @brief
    This function changes LTE RX diversity on a SCC

    @detail
    This function changes LTE RX diversity on a SCC.
 
    Note:
    1). this action code is different from the code provided by LTE RF.
        LTE ML1 will perform a translation of this
    2).LTE RX diversity can only changed when LTE RX is tuned, and RF S/W
       is not performing any other RF activities.
       The following is the status code 
          0: LTE_CPHY_RFCHAIN_REQ successfully processed
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
       -- LTE is busy and cannot take the request 

*/
/*=========================================================================*/
extern boolean lte_common_change_scc_rx_diversity
(
  uint8   scc_id,      /* SCC index, starting from 0 */
  boolean enable_rx0,  /* TRUE to enable RX0, FALSE to disable RX0 */
  boolean enable_rx1   /* TRUE to enable RX1, FALSE to disable RX1 */
);

/*===========================================================================

  FUNCTION:  lte_ml1_common_clock_config

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
extern void lte_ml1_common_clock_config ( void );

/*===========================================================================

  FUNCTION:  lte_ml1_pos_prs_start

===========================================================================*/
/*!
    @brief
    This function sends a start PRS request to ML1.

    @return
    None

*/
/*=========================================================================*/
void lte_ml1_prs_start
(
  /*! PRS start request information */
  lte_cphy_prs_start_req_s *prs_start_req
);

/*===========================================================================

  FUNCTION:  lte_ml1_prs_update_cells

===========================================================================*/
/*!
    @brief
    This function sends a PRS update cells request to ML1.

    @return
    None

*/
/*=========================================================================*/
void lte_ml1_prs_update_cells
(
  /*! PRS update cells request information */
  lte_cphy_prs_update_cells_req_s *prs_update_cells_req
);

/*===========================================================================

  FUNCTION:  lte_ml1_prs_stop

===========================================================================*/
/*!
    @brief
    This function sends a PRS stop request to ML1.

    @return
    None

*/
/*=========================================================================*/
void lte_ml1_prs_stop
(
  /*! PRS stop request information */
  lte_cphy_prs_stop_req_s *prs_stop_req
);


/*===========================================================================

  FUNCTION:  lte_ml1_pos_prs_stop_pbch_decode

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

  FUNCTION:  lte_ml1_pos_prs_inter_frequency_capable

===========================================================================*/
/*!
    @brief
    This function returns whether the PRS module is capable of processing
    inter-frequency cells

    @return
    Whether PRS module is capable of processing inter-frequency cells.

*/
/*=========================================================================*/
boolean lte_ml1_pos_prs_inter_frequency_capable( void );

/*===========================================================================

  FUNCTION:  lte_ml1_schdlr_get_sys_sfn

===========================================================================*/
/*!
    @brief
    The funciton returns the current systime frame number

    @detail
    This will be used by an external API, dont use Mutex
    The SFN value will be valid only when LTE ML1 manager has acquired on
    a cell. 

    @return
    systime frame number
 
*/
/*=========================================================================*/
uint32 lte_ml1_schdlr_get_sys_sfn(void);

/*==============================================================================

  FUNCTION:  lte_ml1_convert_lte_band_to_sys_band

==============================================================================*/
/*!
  @brief
  Converts lte band to Sys band enum

  @detail
  
  @return
  Sys Band enum 
*/
/*============================================================================*/
sys_band_class_e_type lte_ml1_convert_lte_band_to_sys_band(sys_sband_lte_e_type lte_band);

/*==============================================================================

  FUNCTION:  lte_ml1_common_band_earfcn_bw_is valid

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
boolean lte_ml1_common_band_earfcn_bw_is_valid
(
  lte_earfcn_t   dl_earfcn,
  lte_bandwidth_e dl_nrbs
);

/*==============================================================================

  FUNCTION:  lte_ml1_common_band_get_allowed_bw_in_band

==============================================================================*/
/*!
  @brief
  Checks if passed in band is valid and returns allowed BW

  @detail
  Checks is passed in band index is a valid LTE band. If it is, function returns
  the allowed BW in the band

  @return
  True if passed in band is valid, FALSE otherwise
*/
/*============================================================================*/
boolean
lte_ml1_common_band_get_allowed_bw_in_band(
  /*! Band */
  sys_sband_lte_e_type   band_id,
  /*! Ptr to allowed bands data structure */
  lte_ml1_common_band_bw_info_s  * band_bw_info_ptr
);

/*==============================================================================

  FUNCTION:  lte_ml1_common_band_get_band_from_dl_earfcn

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
lte_ml1_common_band_get_band_from_dl_earfcn(
  /*! DL earfcn */
  lte_earfcn_t  dl_earfcn,
  /*! Ptr to band number */
  sys_sband_lte_e_type  * band_ptr
);

/*===========================================================================

  FUNCTION:  lte_ml1_common_cmapi_get_doppler_measurement

===========================================================================*/
/*!
    @brief
    Get doppler measurement of primary serving cell passed by FW.

    @return
    An uint16 value between 0 to 400. Return LTE_ML_MDB_INVALID_DOPPLER_VALUE
    (0xffff) if no measuring is done yet.

*/
/*=========================================================================*/
uint16 lte_ml1_common_cmapi_get_PCC_doppler_measurement(void);
#ifdef FEATURE_LTE_TDD
/*===========================================================================

  FUNCTION:  lte_ml1_validate_tdd_cfg_mbsfn_sf_alloc

===========================================================================*/
/*!
    @brief
    Validate tdd subframe allocation with mbsfn allocation. 

    @return
    Validation status "boolean"
        true if parameters are valid
        false otherwise.
*/
/*=========================================================================*/
boolean lte_ml1_validate_tdd_cfg_mbsfn_sf_alloc(
                              lte_cphy_tdd_cfg_param_s             *tdd_cfg_to_validate,
                              lte_cphy_common_cfg_req_param_s       *common_cfg_params);

#endif
/*===========================================================================

  FUNCTION:  lte_ml1_get_test_bsr_val

===========================================================================*/
/*!
    @brief
    This function reads a test BSR value to be sent (if configured)
    from EFS

    @return
    BSR value (0xFF if invalid)

*/
/*=========================================================================*/
uint8 lte_ml1_get_test_bsr_val
(
  void
);

/*===========================================================================

  FUNCTION:  lte_ml1_mclt_override_scenario

===========================================================================*/
/*!
    @brief
    Function to override the scenario value.

    @details
    The current scenario value is overridden with the provided value. This is
    expected to be used only for debug purposes

    @return
    -1 for failure, 0 for success
*/
/*=========================================================================*/
int lte_ml1_mclt_override_scenario
(
  uint8 antenna,
  uint8 scenario_val
);

/*===========================================================================

  FUNCTION:  lte_ml1_schdlr_current_time_external

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
void lte_ml1_schdlr_current_time_external
(
  lte_l1_sfn_t *             current_sfn,
  lte_l1_hyper_sfn_t *       current_hyper_sfn
);
/*===========================================================================

  FUNCTION:  lte_ml1_sleepmgr_get_remaining_sleep_dur

===========================================================================*/
/*!
    @brief
    This function returns remaining sleep duration.

    @return
    remaining sleep duration in seconds 

*/
/*=========================================================================*/
uint32 lte_ml1_sleepmgr_get_remaining_sleep_dur(void);

/*===========================================================================

  FUNCTION:  lte_ml1_sleepmgr_get_remaining_sleep_dur_in_sec_ms

===========================================================================*/
/*!
    @brief
    This function returns remaining sleep duration.

    @return
    remaining sleep duration in seconds and milli seconds
*/
/*=========================================================================*/
void lte_ml1_sleepmgr_get_remaining_sleep_dur_in_sec_ms
(
 uint32* remaining_dur_in_sec,
 uint64* remaining_dur_in_ms
);

/*===========================================================================

  FUNCTION:  lte_ml1_common_cm_rssi_ind_req

===========================================================================*/
/*!
    @brief
    Sends RSSI data to upper layers.

    @return
    Structure for RSSI Ind.

*/
/*=========================================================================*/
extern lte_api_signal_info_s_type lte_ml1_common_cm_rssi_ind_req(void);


/*===========================================================================

  FUNCTION:  lte_ml1_common_serv_meas_req

===========================================================================*/
/*!
    @brief
    provide serving cell measurement info to upper layers.

    @return
    void.

*/
/*=========================================================================*/

void lte_ml1_common_serv_meas_req(lte_rrc_rx_power_measurement_s_type *lte_serv_meas);


/*===========================================================================

  FUNCTION:  lte_ml1_common_cm_nbr_rssi_ind_req

===========================================================================*/
/*!
    @brief
    Sends RSSI indication to upper layers.

    @return
    boolean.

*/
/*=========================================================================*/

extern void lte_ml1_common_cm_nbr_rssi_ind_req(cmapi_lte_ngbr_ind_s *lte_ngbr_ind);

/*===========================================================================

  FUNCTION:  lte_ml1_mgr_handle_init_req

===========================================================================*/
/*!
  @brief
  Initializes MGR task modules for LTE

  @return
*/
/*=========================================================================*/
void lte_ml1_mgr_handle_init_req
(
  void
);

/*===========================================================================

  FUNCTION:  lte_ml1_mgr_handle_deinit_req

===========================================================================*/
/*!
  @brief
  Deinitializes MGR task modules for LTE

  @return
*/
/*=========================================================================*/
void lte_ml1_mgr_handle_deinit_req
(
  void
);

/*===========================================================================

  FUNCTION:  lte_ml1_gm_handle_init_req

===========================================================================*/
/*!
  @brief
  Main look for GM module

  @return
  None (should not return)
*/
/*=========================================================================*/

void lte_ml1_gm_handle_init_req
(
  void
);

/*===========================================================================

  FUNCTION:  lte_ml1_gm_task_deinit

===========================================================================*/
/*!
  @brief
  ML1 Grant Manager Task de-initialize 

  @return
  TRUE if tear down is successful
*/
/*=========================================================================*/
void lte_ml1_gm_handle_deinit_req
( 
  void 
);

/*===========================================================================

  FUNCTION:  lte_ml1_common_rssi_cmapi_update_tx_global

===========================================================================*/
/*!
    @brief
    Pushes Tx power info to CMAPI supplied global variable

    @return
    None

*/
/*===========================================================================*/
void lte_ml1_common_rssi_cmapi_update_tx_global(void);

/*===========================================================================

  FUNCTION:  lte_ml1_offload_handle_init_req

===========================================================================*/
/*!
  @brief
  Main loop for OFFLOAD module

  @return
  NULL pointer
*/
/*=========================================================================*/
void lte_ml1_offload_handle_init_req
(
  void
);         

/*===========================================================================

  FUNCTION:  lte_ml1_offload_handle_deinit_req

===========================================================================*/
/*!
  @brief
  ML1 offload Task Tear down

  @return
  TRUE if tear down is successful
*/
/*=========================================================================*/
void lte_ml1_offload_handle_deinit_req
(
  void
);

/*===========================================================================

  FUNCTION:  ds_lte_jammer_detect_cb_register

===========================================================================*/
/*!
    @brief
    This function registers the callback function for jammer detection functionality

    @dependencies
    None.

    @sideeffects
    None.

    @return
    TRUE if registration is success, FALSE otherwise
*/
/*=========================================================================*/
extern void ds_lte_jammer_detect_cb_register(lte_ds_jammer_detect_cback_type cb);

#ifdef FEATURE_BUILD_PAGEONLY_IMAGE
/*===========================================================================

  FUNCTION:  lte_ml1_poi_get_rf_band_info

===========================================================================*/
/*!

  @brief
  This function provide RF PSM db band information

  @detail
  This function provide RF PSM db band information. Loop over
  all layers present in MDB compute band and fill RF PSM db excluding
  serving band and any duplicate entry

  @return
  boolean
  TRUE : if there are layers present in MDB
  FALSE: If no layers presnet in MDB, may be PSM restore failed

*/
/*=========================================================================*/
extern boolean lte_ml1_poi_get_rf_band_info
(
  lte_ml1_static_psm_db_band_info_type_s *lte_band_info
);
#endif

/*===========================================================================

        lte_ml1_get_custom_duplex_spacing_applied_for_earfcn

===========================================================================*/
/*!
    @brief
    This function is to return whether RX is tuned to the freq passed in API
    call, and if tuned to the same freq for non_3gpp_duplex_spacing.
   
    @detail
    This function is to return whether RX is tuned to the freq passed in API
    call, and if tuned to the same freq for non_3gpp_duplex_spacing.

    @return
    TRUE if RX is tuned to the same freq for non_3gpp_duplex_spacing;
    FALSE otherwise

*/
/*=========================================================================*/
boolean lte_ml1_get_custom_duplex_spacing_applied_for_earfcn
(
  uint8        instance,

  lte_earfcn_t freq
);
#endif /* LTE_ML1_EXT_API_H */

