#ifndef NASLOG_H
#define NASLOG_H

/*===============================================================================================
                             NASLOG.H

DESCRIPTION
This file contains or includes files that contain NAS log packet structure definitions, 
prototypes for NAS logging functions, any constant definitions that are needed, and any external
variable declarations needed for NAS logging.

Copyright (c) 2001 by Qualcomm Technologies, Inc.  All Rights Reserved.
================================================================================================*/


/*================================================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/nas/vcs/naslog.h_v   1.15   22 Mar 2002 15:44:10   vtawker  $
$Header: //components/rel/mmcp.mpss/6.1.10/api/naslog.h#1 $ $DateTime: 2023/04/25 14:39:18 $ $Author: pwbldsvc $

when        who     what, where, why
--------    ---     ------------------------------------------------------------------------------
09/27/12    am      Missing handling of undefined values for MBR and GBR by the MS
10/05/09    hnam    Added support for forbidden tracking are list & GUTI in EMM STATE log packet
08/20/09    hnam    Removed REG_EVENTS, as this is now supported in log_lte_nas_emm_incoming_event_type &
                    log_lte_nas_emm_incoming_event_type events.
08/14/09    hnam    Added support NAS events
07/30/09    hnam    Added log_version,std/major/minor version to LTE NAS OTA log packets
07/23/09    hnam    Changed SDF_ID type from "byte" to "word"
07/7/09     hnam    Made EMM Native security context structure to be compliant with Dec Spec and 
                    added a WIN32 macro (to take care of byte alignment)
07/1/09     hnam    Swapped EPS context tag & eps context len fields in the structure 
                    (as it was causing problems to Diag, while reading the diag buffer)
06/23/09    hnam    Added NAS related log packets and events
08/18/06   rk       Increased MAX_ACTIVE_PS_CONNECTIONS from 16 to 256 because of Extended TI Feature
4/12/01     ts      Initial Revision
06/11/01    TS      Updated log packets based on peer review.
08/09/01    JC      Added LOG_WCDMA_BASE_C for log codes.
08/14/01    JC      Adjusted log code values.                
09/07/01    CD      Modifed CS call info packet to contain single call information
                    Added log packets for CS call release and CS call change
09/18/01    sbk     The log offsets are changed to reflect the new LOG BASE value
09/24/01    jca     Removed redundant #defines to quiet Lint.
11/21/01    TS      Removed uneeded #include.
11/28/01    sbk     IMEISV size is corrected
1/28/02     CD      Replaced LOG_WCDMA_BASE_C by LOG_UMTS_BASE_C
2/15/02     CD      Removed all log packet definitions and included log_codes_umts.h
                    Changed log codes prefix from WCDMA_ to LOG_UMTS_
                    
                    Fixed typo

22-Mar-2002 VT      In the definition of calling_party_bcd_type, replaced 
                      boolean calling_party_bcd_present;  with
                      uint8 calling_party_bcd_length;
                    In the definition of called_party_bcd_type, replaced 
                      boolean called_party_bcd_present;  with
                      uint8 called_party_bcd_length;

05/06/02    AB      Added CC calls statistics indicator and NAS OTA message event logging.
04/30/04    KWA     Added LOG_UMTS_NAS_EPLMN_LIST_LOG_PACKET_C.
06/28/04    AB      Increase the BCD NO length to 20 as defined in 24.008.
12/07/04    kvk     Increased Max Active PS connections to 16
12/20/04    up      Added LOG_UMTS_NAS_FPLMN_LIST_UPDATE_LOG_PACKET_C,
                    LOG_UMTS_NAS_FPLMN_LIST_LOG_PACKET_C,
                    LOG_UMTS_NAS_HPLMN_SEARCH_TIMER_START_LOG_PACKET_C,
                    LOG_UMTS_NAS_HPLMN_SEARCH_START_LOG_PACKET_C and
                    LOG_UMTS_NAS_HPLMN_SEARCH_END_LOG_PACKET_C to define the
                    log packet for FPLMN list update, FPLMN list, HPLMN search
                    timer start and HPLMN search start/end respectively.
                    MAX_NUMBER_FORBIDDEN_PLMNS is defined as 10.
                    MAX_NUMBER_PRIORITY_PLMNS is defined as 20.
                    Structure log_plmn_rat_type(plmn, rat) 
                    and log_detailed_plmn_info_type are added.
06/15/05    HS      Added support for FEATURE_CCBS CC messages
                     NAS_OTA_CC_ESTABLISHMENT
                     NAS_OTA_CC_ESTABLISHMENT_CONFIRMED
                     NAS_OTA_RECALL
                     NAS_OTA_START_CC
06/20/05   hj      Added support for new events EVENT_SMGMM_REQUEST_SENT, 
                   EVENT_SMGMM_REJECT_RECEIVED.
09/14/05   HS      LOG_CALLED_PARTY_BCD_SIZE revised from 20 to 41
06/08/06   rk       Added Release 5 specific logs for QoS
11/21/11   gps     Removed refrence for header file "gsdi_exp.h"
================================================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "log.h"
#include "log_codes_umts.h"
#include "lte_log_codes.h"
#include "ms.h"                /* for MSG_SETs */
#include "gs.h"                /* for QUEUE_IDs */

#include "event.h"             /* for EVENT logging     */
#include "com_iei.h"           /* for PD     values     */




/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

#define MAX_ACTIVE_CS_CONNECTIONS     7

#define MAX_NUMBER_AVAILABLE_PLMNS    10
#define MAX_NUMBER_EQUIVALENT_PLMNS   5
#define MAX_NUMBER_FORBIDDEN_PLMNS    10
#define MAX_NUMBER_PRIORITY_PLMNS     20
#define PLMN_ID_SIZE                  3
#define LOCATION_AREA_ID_SIZE         2
#define LOG_MAX_IMSI_SIZE             8
#define LOG_TMSI_SIZE                 4
#define LOG_IMEI_SIZE                 9
#define LOG_IMEI_SV_SIZE              10
#define LOG_MAX_NAS_OTA_MESSAGE_SIZE  251
#define LOG_MAX_NAS_CFA_MESSAGE_SIZE  251
#define LOG_CALLING_PARTY_BCD_SIZE    20
#define LOG_CALLED_PARTY_BCD_SIZE     41

#define CN_TO_UE_DIRECTION            0
#define UE_TO_CN_DIRECTION            1

#define LOG_TMSI_TYPE  0
#define LOG_PTMSI_TYPE 1


#define   LTE_NAS_OTA_MSG_MAX_SIZE 1000
#define LOG_MAX_NAS_CB_OTA_MESSAGE_SIZE 1567/* it is the max scheduling length */
#define LOG_MAX_NUMBER_PREFERRED_PLMNS    1000
#define LOG_MAX_PPLMN_RATS                3
//#pragma pack(1)


/* Data Structures */
/*----------------------------------------------------------------------------------------------
 Data structure for Calling Party BCD
 ---------------------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST{ 

   /* Length of the Calling party bcd number. A zero indicates that 
        it is not available */
   uint8 calling_party_bcd_length;
   
   uint8 calling_party_bcd[LOG_CALLING_PARTY_BCD_SIZE];

} calling_party_bcd_type;

/*----------------------------------------------------------------------------------------------
 Data structure for Called Party BCD
 ---------------------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST{ 

   /* Length of the Called party bcd number. A zero indicates that 
        it is not available */
   uint8 called_party_bcd_length;
   
   uint8 called_party_bcd[LOG_CALLED_PARTY_BCD_SIZE];

} called_party_bcd_type;


/*----------------------------------------------------------------------------------------------
 Data structure for CS call information
 ---------------------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST{

   /* The connection id assigned to this CS call   */
   uint8   connection_id;

   /* The stream id assigned to this CS call       */
   uint8  stream_id;

   /* The current CC call state of this CS call    */
   uint8   cc_state;  

   /* The current multiparty state of this CS call */
   uint8   mpty_state;

   /* The current hold state of this CS call       */
   uint8   hold_state;

   /* Indicates if data or voice call              */
   uint8   call_type;

   /* Indicates whether the network or the mobile  */
   /* originated the call                          */
   uint8   origination_direction;

   /* Calling party bcd for mobile terminated      */
   /* calls.                                       */
   calling_party_bcd_type calling_party_bcd;

   /* Called party bcd for mobile originated calls */
   called_party_bcd_type  called_party_bcd;

} cs_connection_info_type;

/*----------------------------------------------------------------------------------------------
 Data structure for PS call information
 ---------------------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST{

   /* The connection id assigned to this PS call   */
   uint8   connection_id;

   /* The stream id assigned to this CS call       */
   uint32  nsapi;

} ps_connection_info_type;

typedef PACKED struct PACKED_POST{ 

   uint8 plmn_id[PLMN_ID_SIZE];

} log_plmn_id_type;


typedef PACKED struct PACKED_POST{

   /* PLMN ID */
   log_plmn_id_type plmn;

   /* Radio access Tech associated with this PLMN */
   uint8            rat;
   
} log_plmn_rat_type;

typedef PACKED struct PACKED_POST{   uint8 lai[LOCATION_AREA_ID_SIZE];} log_location_area_id_type;

typedef PACKED struct PACKED_POST{

   /* PLMN id */
   log_plmn_id_type plmn;

   /* PLMN Details
   ** [ Signal Quality | Service Cap | RAT | PLMN type ]
   ** b8    - Signal Quality, 0 - Signal HIGH, 1 - Signal LOW.
   ** b7-b6 - PLMN service Capability.
   **         0 0 = 0 - Not Available
   **         0 1 = 1 - CS Only
   **         1 0 = 2 - PS Only
   **         1 1 = 3 - CS and PS
   ** b5-b4 - RAT
   **         0 0 = 0 - GSM
   **         0 1 = 1 - UMTS
   **         a b = 2-3 - Not Used.
   ** b3-b1 - PLMN Catgory.
   **         0 0 0 = 0 - HPLMN.
   **         0 0 1 = 1 - PREFERRED PLMN.
   **         0 1 0 = 2 - USER Preferred PLMN.
   **         0 1 1 = 3 - OPERATOR Preferred PLMN.
   **         1 0 0 = 4 - OTHER PLMN.
   **         a b c = 5-7 - Not Used.
   */
   uint8            plmn_details;

   /* Signal Strength */
   int32            signal_strength;

} log_detailed_plmn_info_type;

#define MAX_IMSI_SIZE              8

#define LOG_MAX_CSG_LIST     50

#define NAS_LOG_OCSGL_TYPE 0
#define NAS_LOG_ACSGL_TYPE 1

#define LOG_MAX_NB1_PCI_SCAN_REQ 8

#define LOG_MAX_EARFCN_SCAN_REQ 32

#define LOG_MAX_PCI_SCAN_REQ 8

#define LOG_MAX_RAT_SCAN_RESULT 3

#define LOG_MAX_RESULT_PER_RAT 11

#define LOG_MAX_PCI_RESULT  8

#define LOG_PCI_SCAN_MAX_NUM_PLMN 6   /* no of shared LTE PLMNs per cell*/


typedef enum
{ /* will add all enum later currently not sure about it */
  LOG_OFFSET_MINUS_TEN,
  LOG_OFFSET_MINUS_NINE,
  LOG_OFFSET_MINUS_EIGHT,
  LOG_OFFSET_MINUS_SEVEN,
  LOG_OFFSET_MINUS_SIX,
  LOG_OFFSET_MINUS_FIVE,
  LOG_OFFSET_MINUS_FOUR,
  LOG_OFFSET_MINUS_THREE,
  LOG_OFFSET_MINUS_TWO,
  LOG_OFFSET_MINUS_ONE,
  LOG_OFFSET_ZERO,
  LOG_OFFSET_PLUS_ONE,
  LOG_OFFSET_PLUS_TWO,
  LOG_OFFSET_PLUS_THREE,
  LOG_OFFSET_PLUS_FOUR,
  LOG_OFFSET_PLUS_FIVE,
  LOG_OFFSET_PLUS_SIX,
  LOG_OFFSET_PLUS_SEVEN,
  LOG_OFFSET_PLUS_EIGHT,
  
  LOG_OFFSET_PLUS_NINE

}log_nb1_earfcn_offset_e_type;



typedef enum
{
  
  LOG_NB1_SET_RASTER_OFFSET_MIN_7_DOT_5_KHZ, /* -7.5 kHz */
  LOG_NB1_SET_RASTER_OFFSET_MIN_2_DOT_5_KHZ, /* -2.5 kHz */
  LOG_NB1_SET_RASTER_OFFSET_2_DOT_5_KHZ,  /*  2.5 kHz */
  LOG_NB1_SET_RASTER_OFFSET_7_DOT_5_KHZ   /*  7.5 kHz */
  
}log_nb1_raster_offset_e_type;


typedef enum
{
  /** @cond
  */
  LOG_RAT_NONE = -1,  /* FOR INTERNAL USE ONLY! */
  /** @endcond
  */

  LOG_RAT_GSM_RADIO_ACCESS,
    /**< GSM radio access technology. */

  LOG_RAT_UMTS_RADIO_ACCESS,
    /**< UMTS radio access technology. */

  LOG_RAT_LTE_RADIO_ACCESS,
    /**< LTE radio access technology. */

  LOG_RAT_TDS_RADIO_ACCESS,
    /**< TD-SCDMA radio access technology */


  LOG_RAT_LTE_M1_RADIO_ACCESS,
  /*< LTE M1 radio access technology */

  LOG_RAT_LTE_NB1_RADIO_ACCESS,
    /*< LTE NB1 radio access technology */

  /** @cond
  */
  LOG_RAT_MAX   /* FOR INTERNAL USE ONLY! */
  /** @endcond
  */

} log_radio_access_tech_e_type;

typedef enum
{
  /** @cond
  */
  LOG_PLMN_LIST_NONE = -1,       /* FOR INTERNAL USE ONLY */
  /** @endcond
  */

  LOG_PLMN_LIST_SUCCESS,    /**< &nbsp; */
  LOG_PLMN_LIST_AS_ABORT,   /**< &nbsp; */
  LOG_PLMN_LIST_REJ_IN_RLF,
  LOG_PLMN_LIST_PARTIAL,
  LOG_PLMN_LIST_FAIL_IN_CONNECTED_MODE,

  /** @cond
  */
  LOG_PLMN_LIST_MAX              /* FOR INTERNAL USE ONLY */
  /** @endcond
  */

} log_plmn_list_status_e_type;

typedef enum
{
  LOG_SCAN_RESULT_NONE,
  LOG_SCAN_RESULT_INVALID,
  LOG_SCAN_COMPLETE_SUCCESS,
  LOG_SCAN_COMPLETE_NO_CELL_FOUND,
  LOG_SCAN_STOPPED_BY_DURATION_TIMER,
  LOG_SCAN_NOT_STARTED,
  LOG_SCAN_OVERLAPPED_CHANNEL,
  LOG_SCAN_CHANNEL_NOT_SUPPORTEDSS

} log_channel_scan_result_e_type;

typedef enum
{
  LOG_POWER_MEAS_TYPE_RSRP,
  LOG_POWER_MEAS_TYPE_RXLEVEL
}log_wwan_rx_power_meas_e_type;


typedef PACKED struct PACKED_POST{

  /*PLMN ID */
  log_plmn_id_type    plmn_id;

  /*CSG ID*/
  uint32          csg_id;
}log_csg_info_type;


typedef PACKED struct PACKED_POST{

   /* PLMN ID */
   log_plmn_id_type plmn;

    /*CSG ID */
    uint32       csg_id;

   /* Radio access Tech associated with this PLMN */
   uint8            rat; 
} log_plmn_csg_rat_type;


typedef PACKED struct PACKED_POST{

   /* PLMN ID */
   log_plmn_id_type plmn;

   /* Preferred PLMN category */
   uint8            category;

   /* Number of RATs */
   uint8            num_rats;

   /* Radio access Technologies (LTE, UMTS or GSM) associated with this PLMN */
   uint8            rat[LOG_MAX_PPLMN_RATS];
  
} log_pplmn_type;

typedef PACKED struct PACKED_POST{

  uint8  max_cells_per_channel;
  /*max can be 8 , recommend to support upto 5  */ 

}log_loc_wwan_scan_control_s_type;

typedef PACKED struct PACKED_POST{

  boolean is_system_frame_offset_present;
  boolean is_lte_cell_id;
/* Physical cell id (0 to C_MAX_PHYSICAL_CELL_ID) */
  uint16  phy_id; 
  /* Cell timing system frame offset */
  uint32 system_frame_offset;

}log_loc_wwan_scan_ad_per_pci_m1_s_type;

typedef PACKED struct PACKED_POST{

  /* Channel number / downlink center frequency (0 to C_MAX_EXTENDED_EARFCN) */

  uint32 earfcn;
  
  boolean decode_sib1;
/* decode Global Cell Id */

  boolean  measure_power_and_time;
   /* measure RSRP or RxLevel also get cell timing with power  */

  uint8 num_cells_to_return;
/* number of cells to return on this channel overriding default limit */


  /* Per channel assistance data and controls */
  uint8                     num_of_m1_pci_ad_element;

  log_loc_wwan_scan_ad_per_pci_m1_s_type 
             wwan_scan_ad_per_pci_m1[LOG_MAX_PCI_SCAN_REQ];

}log_loc_wwan_scan_ad_element_m1_s_type;

typedef PACKED struct PACKED_POST{

  /* Channel number / downlink center frequency (0 to C_MAX_EXTENDED_EARFCN) */
  boolean is_m1_blind_scan_req;

  uint16 max_scan_duration_m1_sec;

  uint8  num_m1_ad_element;  

  log_loc_wwan_scan_ad_element_m1_s_type   m1_ad_elems[LOG_MAX_EARFCN_SCAN_REQ];


}log_loc_wwan_scan_rat_m1_param_s_type;


typedef PACKED struct PACKED_POST{

  boolean is_system_frame_offset_present;

  boolean is_earfcn_offset_present;

  boolean is_raster_offset_present;

/* Physical cell id (0 to C_MAX_PHYSICAL_CELL_ID) */
  uint16  phy_id;
  /* NB1 offset from downlink center frequency (TBD whether this is useful) */

  log_nb1_raster_offset_e_type  nb1_raster_offset;

  log_nb1_earfcn_offset_e_type  nb1_earfcn_offset;

  /* Cell timing system frame offset */
  uint32  system_frame_offset;

}log_loc_wwan_scan_ad_per_pci_nb1_s_type;

typedef PACKED struct PACKED_POST{

  /* Channel number / downlink center frequency (0 to C_MAX_EXTENDED_EARFCN) */

  uint32 earfcn;
  
  boolean decode_sib1;
/* decode Global Cell Id */

  boolean  measure_power_and_time;
   /* measure RSRP or RxLevel also get cell timing with power  */

  uint8 num_cells_to_return;
/* number of cells to return on this channel overriding default limit */


  /* Per channel assistance data and controls */
  uint8                     num_of_nb1_pci_ad_element;
  log_loc_wwan_scan_ad_per_pci_nb1_s_type wwan_scan_ad_per_pci_nb1[LOG_MAX_NB1_PCI_SCAN_REQ];


}log_loc_wwan_scan_ad_element_nb1_s_type;

typedef PACKED struct PACKED_POST{

  /* Channel number / downlink center frequency (0 to C_MAX_EXTENDED_EARFCN) */
  boolean is_nb1_blind_scan_req;

  uint16 max_scan_duration_nb1_sec;

  uint8  num_nb1_ad_element;  

  log_loc_wwan_scan_ad_element_nb1_s_type   nb1_ad_elems[LOG_MAX_EARFCN_SCAN_REQ];


}log_loc_wwan_scan_rat_nb1_param_s_type;


typedef PACKED struct PACKED_POST{

  /* Absolute radio frequency channel number */

  ARFCN_T  arfcn;

/* Base station identity code */
  uint8   bsic;

    /* Control measurements for this channel */
  boolean                        measurement_power;   /* measure RSRP or RxLevel */
  boolean                       measure_cell_timing;  /* measure cell timing */
  boolean                        decode_gcid;         /* decode Global Cell Id */

}log_loc_wwan_scan_ad_element_gsm_s_type;

typedef PACKED struct PACKED_POST{
  /* GSM WWAN scan assistance data per earfcn*/
  boolean is_gsm_blind_scan_req;
  uint16 max_scan_duration_gsm_sec;
  uint8  num_gsm_ad_element;

  log_loc_wwan_scan_ad_element_gsm_s_type   gsm_ad_elems[LOG_MAX_EARFCN_SCAN_REQ];	

}log_loc_wwan_scan_rat_gsm_param_s_type;


typedef PACKED union PACKED_POST{
  log_loc_wwan_scan_rat_m1_param_s_type m1_scan_params;
  log_loc_wwan_scan_rat_nb1_param_s_type nb1_scan_params;
  log_loc_wwan_scan_rat_gsm_param_s_type gsm_params;
}log_loc_wwan_scan_rats_params_u_type;


typedef PACKED struct PACKED_POST{

  log_radio_access_tech_e_type rat;

  log_loc_wwan_scan_rats_params_u_type  wwan_rat_scan_param;

} log_loc_wwan_scan_assistance_data_s_type;


typedef PACKED struct PACKED_POST{

  log_loc_wwan_scan_control_s_type control_param;
  log_loc_wwan_scan_assistance_data_s_type assist_param;
}log_wwan_adavnce_scan_pram_s_type;

typedef PACKED struct PACKED_POST{

  byte                                identity[3];
    /**< Public Land Mobile Network ID. */

} log_plmn_id_s_type;

typedef word                          log_lac_type;

/** @brief PLMN plus LAC.
*/
typedef struct log_plmn_lac_id_s
{
  log_plmn_id_s_type   plmn;  /**< Public Land Mobile Network. */
  log_lac_type         lac;   /**< Location Area Code. */
}log_plmn_lac_id_s_type;


typedef struct {

  uint16                           tac; /* invalid value will be 0XFFFF  */
  uint32                           global_cell_id;
  uint8                            num_plmn;  
  log_plmn_id_s_type               plmn[LOG_PCI_SCAN_MAX_NUM_PLMN];
} log_loc_m1_cell_id_s_type;


typedef struct {

  uint16                                   tac;/* invalid value will be 0XFFFF  */ 
  uint32                                   global_cell_id;
  uint8                            num_plmn;  
  log_plmn_id_s_type               plmn[LOG_PCI_SCAN_MAX_NUM_PLMN];
} log_loc_nb1_cell_id_s_type;

typedef struct {

  log_plmn_id_s_type      plmn;
  log_lac_type            lac;
  uint16                  cell_identity;
}log_loc_gsm_cell_id_s_type;


typedef  struct {

  log_radio_access_tech_e_type     rat;
  union
  {
   // sys_loc_lte_cell_id_s_type lte_cell;
    log_loc_m1_cell_id_s_type m1_Cell;
    log_loc_nb1_cell_id_s_type nb1_cell;
    log_loc_gsm_cell_id_s_type gsm_cell;
  }rat_cell;

}log_loc_wwan_cell_id_s_type;


typedef  struct {

  log_wwan_rx_power_meas_e_type     rx_power_meas_sel;

  union
  {
    uint16 rsrp; /* -140 to -44 dBm */
    uint16 rxlevel; /* -110 to -48 dBm */
  }  power_measurement;

}log_wwan_rx_power_measurement_s_type;


typedef PACKED struct PACKED_POST{

  boolean is_serving_cell;
  boolean is_cell_info_present;
  boolean is_lte_cell;
  boolean is_rx_power_meas_present;
  boolean is_delta_time_offset_present;
  boolean is_system_frame_offset_present;

/* Physical cell id (0 to C_MAX_PHYSICAL_CELL_ID) */
  uint16  phy_id; 
  log_loc_wwan_cell_id_s_type cell_info;
  log_wwan_rx_power_measurement_s_type rx_power_meas;

/* Cell observation delta time offset from start of scan */
  uint32 delta_time_offset;

  /* Cell timing system frame offset */
  uint32  system_frame_offset;

}log_loc_wwan_scan_result_per_pci_m1_s_type;

typedef PACKED struct PACKED_POST{
/* Channel number / downlink center frequency (0 to C_MAX_EXTENDED_EARFCN) */
  uint32  earfcn;

  log_channel_scan_result_e_type  channel_scan_result_status;

  uint8  num_result_per_earfcn_m1;

  log_loc_wwan_scan_result_per_pci_m1_s_type wwan_result_per_earfcn_m1[LOG_MAX_PCI_RESULT];

}log_loc_wwan_scan_result_m1_s_type;

typedef PACKED struct PACKED_POST{

  boolean is_serving_cell;
  boolean is_cell_info_present;
  boolean is_earfcn_offset_present;
  boolean is_raster_offset_present;
  boolean is_rx_power_meas_present;
  boolean is_delta_time_offset_present;
  boolean is_system_frame_offset_present;

/* Physical cell id (0 to C_MAX_PHYSICAL_CELL_ID) */
  uint16  phy_id; 

  log_nb1_raster_offset_e_type	nb1_raster_offset;


  /* NB1 offset from downlink center frequency */
  log_nb1_earfcn_offset_e_type  nb1_earfcn_offset;


  /* Global cell id */

  log_loc_wwan_cell_id_s_type   cell_info;

  /* Received power */

  log_wwan_rx_power_measurement_s_type rx_power_meas;

/* Cell observation delta time offset from start of scan */
  uint32 delta_time_offset;

  /* Cell timing system frame offset */
  uint32  system_frame_offset;

}log_loc_wwan_scan_result_per_pci_nb1_s_type;


typedef PACKED struct PACKED_POST{


/* Channel number / downlink center frequency (0 to C_MAX_EXTENDED_EARFCN) */
  uint32  earfcn;

  log_channel_scan_result_e_type  channel_scan_result_status;

  uint8  num_result_per_earfcn_nb1;

  log_loc_wwan_scan_result_per_pci_nb1_s_type wwan_result_per_earfcn_nb1[LOG_MAX_PCI_RESULT];
}log_loc_wwan_scan_result_nb1_s_type;

typedef PACKED struct PACKED_POST{


  boolean is_bsic_valid;
  boolean is_cell_timing_present;
  boolean is_global_cell_id_present;
  boolean is_rx_power_meas_present;
/* Absolute radio frequency channel number */
  ARFCN_T  arfcn;
/* Base station identity code */
  uint8  bsic;

  log_channel_scan_result_e_type  channel_scan_result_status;

  /* Cell timing (PMIC time) */
  uint32 cell_timing;
  /* Global cell id */

  log_loc_wwan_cell_id_s_type   cell_info;

  /* Received power */

  log_wwan_rx_power_measurement_s_type rx_power_meas;


}log_loc_wwan_scan_result_gsm_s_type;

typedef union
{

  //sys_loc_wwan_scan_result_lte_s_type wwan_scan_result_lte;

  log_loc_wwan_scan_result_m1_s_type wwan_scan_result_m1;

  log_loc_wwan_scan_result_nb1_s_type wwan_scan_result_nb1;

  log_loc_wwan_scan_result_gsm_s_type wwan_scan_result_gsm;

}log_loc_wwan_rat_scan_result_u_type;


typedef PACKED struct PACKED_POST{

  /* Air interface for which scan result is provided */

  log_radio_access_tech_e_type  rat;

  log_plmn_list_status_e_type scan_status_per_rat;
  
/* Scan reference time */
  uint64 scan_ref_time;     /* PMIC timestamp when scan starts on this air interface */

/* Scan result based on air interface bit */
  uint16            num_scan_result_per_rat;

  log_loc_wwan_rat_scan_result_u_type  wwan_scan_result_per_rat[LOG_MAX_RESULT_PER_RAT];

}log_loc_wwan_scan_result_element_s_type;
/*----------------------------------------------------------------------------------------------
                   LOG PACKET: WCDMA_NAS_GMM_STATE_LOG_PACKET
 ---------------------------------------------------------------------------------------------*/

LOG_RECORD_DEFINE(LOG_UMTS_NAS_GMM_STATE_LOG_PACKET_C)
    
   /* GMM layer state         */
   uint8   gmm_state_log;
   
   /* GMM layer substate      */
   uint8   gmm_substate_log; 

   /* GMM layer update status */
   uint8   gmm_update_status_log;

LOG_RECORD_END

/*----------------------------------------------------------------------------------------------
                   LOG PACKET: WCDMA_NAS_MM_STATE_LOG_PACKET
 ---------------------------------------------------------------------------------------------*/

LOG_RECORD_DEFINE(LOG_UMTS_NAS_MM_STATE_LOG_PACKET_C)
    
   /* MM layer state         */
   uint8   mm_state_log;
   
   /* MM layer idle substate */
   uint8   mm_idle_substate_log; 

   /* MM layer update statu  */
   uint8   mm_update_status;

LOG_RECORD_END


/*----------------------------------------------------------------------------------------------
                   LOG PACKET: WCDMA_NAS_REG_LOG_PACKET
 ---------------------------------------------------------------------------------------------*/

LOG_RECORD_DEFINE(LOG_UMTS_NAS_REG_LOG_PACKET_C)
    
   /* REG layer state */
   uint8   reg_state;

   /* PLMN Selection Mode: Automatic or Manual */
   uint8   plmn_selection_mode;

   /* UE Operation mode: Class A, Class B, or Class c */
   uint8   ue_operation_mode;

LOG_RECORD_END



/*----------------------------------------------------------------------------------------------
                   LOG PACKET: WCDMA_CS_CALL_SETUP_INFO_LOG_PACKET
 ---------------------------------------------------------------------------------------------*/

LOG_RECORD_DEFINE(LOG_UMTS_CS_CALL_SETUP_INFO_LOG_PACKET_C)
    
   cs_connection_info_type   cs_connection_info;  

LOG_RECORD_END




/*----------------------------------------------------------------------------------------------
                   LOG PACKET: WCDMA_MM_INFO_LOG_PACKET
 ---------------------------------------------------------------------------------------------*/

LOG_RECORD_DEFINE(LOG_UMTS_MM_INFO_LOG_PACKET_C)

   /* Network Operation mode: Class I, Class II, or Class III */
   uint8   network_operation_mode;
 
   /* Available Service: Limited, CS, PS, CS and PS, No Service */
   uint8   service_type;

   /* Serving Cell PLMN */
   log_plmn_id_type   selected_plmn_id;

   /* Serving Cell LAI */
   log_location_area_id_type location_area_id;

   /* Serving Cell RAC */
   uint8   routing_area_code;

   /* Number of available plmns */
   uint8   num_available_plmns;

   /* PLMN ids for available plmns */
   log_plmn_id_type   available_plmns[MAX_NUMBER_AVAILABLE_PLMNS];

  /*CSG ID for Serving Cell */
   uint32     csg_id;

LOG_RECORD_END


/*----------------------------------------------------------------------------------------------
                   LOG PACKET: WCDMA_NAS_PS_CONNECTION_QOS_LOG_PACKET
 ---------------------------------------------------------------------------------------------*/

LOG_RECORD_DEFINE(LOG_UMTS_NAS_PS_CONNECTION_QOS_LOG_PACKET_C)

   /* The associated connection id       */
   uint8   connection_id;

   /* QOS: Delay class                   */
   uint8   delay_class;
  
   /* QOS: Reliability class             */
   uint8   reliability_class;
  
   /* QOS: Peak throughput               */
   uint8   peak_throughput;
  
   /* QOS: Precedence class              */
   uint8   precedence_class;
  
   /* QOS: Mean throughput               */
   uint8   mean_throughput;
  
   /* QOS: Traffic class                 */
   uint8   traffic_class;
  
   /* QOS: Delivery order                */
   uint8   deliver_order;
  
   /* QOS: Delivery of erroneous SDUs    */
   uint8   deliver_err_sdu;
  
   /* QOS: Maximum SDU size              */
   uint8   max_sdu_size;
  
   /* QOS: Max uplink bit rate           */
   uint8   max_bit_rate_uplink;
  
   /* QOS: Max downlink bit rate         */
   uint8   max_bit_rate_downlink;
  
   /* QOS: Residual Bit Error Rate       */
   uint8   residual_ber;
  
   /* QOS: SDU error ratio               */
   uint8   sdu_err_ratio;
  
   /* QOS: Transfer delay                */
   uint8   tranfer_delay;
  
   /* QOS: Traffic Handling Priority     */
   uint8   traffic_priority;
  
   /* Guaranteed uplink Bit Error Rate   */
   uint8   guarantee_ber_uplink;
  
   /* Guaranteed downlink Bit Error Rate */
   uint8   guarantee_ber_dwnlink;


   /* QOS: Signaling Indicator                  */
   uint8   sig_ind;

   /* QOS: Source Statistics descriptor                  */
   uint8   src_stat_desc;

   /* QOS: Extended Max downlink bit rate                   */
   uint8   ext_max_bit_rate_downlink;

   /* QOS: Extended Guranteed downlink bit rate                  */
   uint8   ext_gtd_bit_rate_downlink;

    /* QOS: Extended Max uplink bit rate                  */
   uint8 ext_max_bit_rate_uplink;

    /* QOS: Extended Guranteed uplink bit rate                  */
   uint8 ext_gtd_bit_rate_uplink;


LOG_RECORD_END


/*----------------------------------------------------------------------------------------------
                   LOG PACKET: WCDMA_NAS_CS_CONNECTION_BC_LOG_PACKET
 ---------------------------------------------------------------------------------------------*/

LOG_RECORD_DEFINE(LOG_UMTS_NAS_CS_CONNECTION_BC_LOG_PACKET_C)

   /* The associated connection id       */
   uint8   connection_id;

   uint8   radio_channel_requirement;
   uint8   information_transfer_capability;
   uint8   structure;
   uint8   duplex_mode;
   uint8   rate_adaption;
   uint8   signalling_access_protocol;
   uint8   sync_async;
   uint8   number_of_data_bits;
   uint8   number_of_stop_bits;
   uint8   user_rate;
   uint8   parity;
   uint8   modem_type;
   uint8   connection_element;
   uint8   user_information_layer_2_protocol;
   uint8   coding_standard;
   uint8   transfer_mode;
   uint8   configuration;
   uint8   NIRR;
   uint8   establishment;
   uint8   user_information_layer_1_protocol;
   uint8   negotiation;
   uint8   intermediate_rate;
   uint8   NIC_on_Tx;
   uint8   NIC_on_Rx;
   uint8   other_modem_type;
   uint8   fixed_network_user_rate;
   uint8   acceptable_channel_codings;
   uint8   maximum_number_of_trafic_channels;
   uint8   user_initiated_modification_indication;
   uint8   wanted_air_interface_user_rate;
   uint8   mt_fallback_rate;
//   uint8   speech_version_indication;

LOG_RECORD_END

/*----------------------------------------------------------------------------------------------
                   LOG PACKET: WCDMA_NAS_UE_DYNAMIC_ID_LOG_PACKET
 ---------------------------------------------------------------------------------------------*/

LOG_RECORD_DEFINE(LOG_UMTS_NAS_UE_DYNAMIC_ID_LOG_PACKET_C)

   /* Indicates if packet has TMSI or PTMSI */
   uint8 identity_type;
    
   /* TMSI or PTMSI */
   uint8 identity[LOG_TMSI_SIZE];

LOG_RECORD_END

/*----------------------------------------------------------------------------------------------
                   LOG PACKET: WCDMA_NAS_UE_STATIC_ID_LOG_PACKET
 ---------------------------------------------------------------------------------------------*/

LOG_RECORD_DEFINE(LOG_UMTS_NAS_UE_STATIC_ID_LOG_PACKET_C)

   /* Length of the IMSI */
   uint8 imsi_length;

   /* IMSI    */
   uint8 imsi[LOG_MAX_IMSI_SIZE];
 
   /* IMEI    */
   uint8 imei[LOG_IMEI_SIZE];
   
   /* IMEI_SV */
   uint8 imei_sv[LOG_IMEI_SV_SIZE];

LOG_RECORD_END


/*----------------------------------------------------------------------------------------------
                   LOG PACKET: WCDMA_NAS_OTA_MESSAGE_LOG_PACKET
 ---------------------------------------------------------------------------------------------*/

LOG_RECORD_DEFINE(LOG_UMTS_NAS_OTA_MESSAGE_LOG_PACKET_C)
 
   /* Indicates if this was sent to or from the UE */
   uint8 nas_message_direction;

   /* Length of the NAS ota message */
   uint32 nas_message_length;

   /* NAS ota Message */
   uint8 nas_message[LOG_MAX_NAS_OTA_MESSAGE_SIZE];

LOG_RECORD_END


/*----------------------------------------------------------------------------------------------
                   LOG PACKET: WCDMA_NAS_CFA_MESSAGE_LOG_PACKET
 ---------------------------------------------------------------------------------------------*/

LOG_RECORD_DEFINE(LOG_UMTS_NAS_CFA_MESSAGE_LOG_PACKET_C)

   /* NAS cfa log message */
   uint8 cfa_message[LOG_MAX_NAS_CFA_MESSAGE_SIZE];

LOG_RECORD_END


/*----------------------------------------------------------------------------------------------
                   LOG PACKET: WCDMA_NAS_ERROR_MESSAGE_LOG_PACKET
 ---------------------------------------------------------------------------------------------*/

LOG_RECORD_DEFINE(LOG_UMTS_NAS_ERROR_MESSAGE_LOG_PACKET_C)

   /* SM, MM, GMM, REG, CC, SMS, SS, RABM or MN */
   uint8 nas_layer;

   /* Error code */
   uint16 error_code;

LOG_RECORD_END

/*----------------------------------------------------------------------------------------------
                   LOG PACKET: WCDMA_CS_CALL_RELEASE_INFO_LOG_PACKET
 ---------------------------------------------------------------------------------------------*/

LOG_RECORD_DEFINE(LOG_UMTS_CS_CALL_RELEASE_INFO_LOG_PACKET_C)
    
   uint8    connection_id;
   uint8    release_cause;

LOG_RECORD_END

/*----------------------------------------------------------------------------------------------
                   LOG PACKET: WCDMA_CS_CALL_CHANGE_INFO_LOG_PACKET
 ---------------------------------------------------------------------------------------------*/

LOG_RECORD_DEFINE(LOG_UMTS_CS_CALL_CHANGE_INFO_LOG_PACKET_C)
    
   uint8    connection_id;  
   uint8    mpty_state;
   uint8    hold_state;

LOG_RECORD_END

/*----------------------------------------------------------------------------------------------
                   LOG PACKET: UMTS_NAS_EPLMN_LIST_LOG_PACKET
 ---------------------------------------------------------------------------------------------*/

LOG_RECORD_DEFINE(LOG_UMTS_NAS_EPLMN_LIST_LOG_PACKET_C)

   /* Registered PLMN ID */
   log_plmn_id_type rplmn;

   /* Number of equivalent PLMNs */
   uint8            num_equivalent_plmns;

   /* Equivalent PLMN IDs */
   log_plmn_id_type equivalent_plmn[MAX_NUMBER_EQUIVALENT_PLMNS];

LOG_RECORD_END


/*----------------------------------------------------------------------------------------------
                   LOG PACKET: UMTS_NAS_FPLMN_LIST_UPDATE_LOG_PACKET
 ---------------------------------------------------------------------------------------------*/

LOG_RECORD_DEFINE(LOG_UMTS_NAS_FPLMN_LIST_UPDATE_LOG_PACKET_C)

   /* TRUE if added in FPLMN List, FALSE if removed from List */
   uint8            is_added;

   /* Forbidden PLMN ID */
   log_plmn_id_type fplmn;

LOG_RECORD_END

/*----------------------------------------------------------------------------------------------
                   LOG PACKET: UMTS_NAS_FPLMN_LIST_LOG_PACKET
 ---------------------------------------------------------------------------------------------*/

 
LOG_RECORD_DEFINE(LOG_UMTS_NAS_FPLMN_LIST_LOG_PACKET_C)

   /* Number of PLMNs in FPLMN List */
   uint8            num_fplmns;
   
   /* List/Array of Forbidden PLMN IDs */
   log_plmn_id_type fplmns[MAX_NUMBER_FORBIDDEN_PLMNS];

LOG_RECORD_END

/*----------------------------------------------------------------------------------------------
                   LOG PACKET: UMTS_NAS_HPLMN_SEARCH_TIMER_START_LOG_PACKET_C
 ---------------------------------------------------------------------------------------------*/

LOG_RECORD_DEFINE(LOG_UMTS_NAS_HPLMN_SEARCH_TIMER_START_LOG_PACKET_C)

   /* Time Duration, in minutes, of HPLMN Search Timer */
   uint16            hplmn_search_timer;

LOG_RECORD_END

/*----------------------------------------------------------------------------------------------
                   LOG PACKET: UMTS_NAS_HPLMN_SEARCH_TIMER_EXPIRE_LOG_PACKET_C
 ---------------------------------------------------------------------------------------------*/

LOG_RECORD_DEFINE(LOG_UMTS_NAS_HPLMN_SEARCH_TIMER_EXPIRE_LOG_PACKET_C)

   /* Not used. As the log packet need to be allocated kept one byte dummy */
   uint8            dummy;

LOG_RECORD_END

/*----------------------------------------------------------------------------------------------
                   LOG PACKET: LOG_UMTS_NAS_HPLMN_SEARCH_START_LOG_PACKET_C
 ---------------------------------------------------------------------------------------------*/

LOG_RECORD_DEFINE(LOG_UMTS_NAS_HPLMN_SEARCH_START_LOG_PACKET_C)

   /* Number of PLMNs in Higher PLMN List */
   uint8             num_pplmns;

   /* List/Array of higher priority PLMN IDs */
   log_plmn_rat_type pplmns[MAX_NUMBER_PRIORITY_PLMNS];

LOG_RECORD_END

/*----------------------------------------------------------------------------------------------
                   LOG PACKET: LOG_UMTS_NAS_HPLMN_SEARCH_END_LOG_PACKET_C
 ---------------------------------------------------------------------------------------------*/

LOG_RECORD_DEFINE(LOG_UMTS_NAS_HPLMN_SEARCH_END_LOG_PACKET_C)

   /* Number of PLMNs in Higher PLMN List */
   uint8                       num_available_plmns;

   /* List/Array of higher priority PLMN IDs */
   log_detailed_plmn_info_type info[MAX_NUMBER_AVAILABLE_PLMNS];

LOG_RECORD_END


/*----------------------------------------------------------------------------------------------
                   LOG PACKET: UMTS_NAS_CSG_LIST_LOG_PACKET
 ---------------------------------------------------------------------------------------------*/

LOG_RECORD_DEFINE(LOG_UMTS_NAS_CSG_WHITE_LIST_LOG_PACKET_C)

  /* Log version*/
  byte                              log_version;

  /* Type of list, OCSGL or ACSGL*/
  uint8                             type;

  /*Number of CSG in white list*/
  uint8                             num_csg;

  /*List/Array of CSG white list information*/
  log_csg_info_type         csg_info[LOG_MAX_CSG_LIST];

LOG_RECORD_END


/*----------------------------------------------------------------------------------------------
                   LOG PACKET: UMTS_NAS_CSG_LIST_LOG_PACKET
 ---------------------------------------------------------------------------------------------*/

LOG_RECORD_DEFINE(LOG_UMTS_NAS_CSG_AVAILABLE_LIST_LOG_PACKET_C)

  /* Log version*/
  byte                              log_version;

  /*Number of CSG in available list*/
  uint8                             num_csg;

 /*List/Array of available CSG*/
  log_plmn_csg_rat_type         csg_info[LOG_MAX_CSG_LIST];

LOG_RECORD_END

/*----------------------------------------------------------------------------------------------
                   DUAL SIM LOG PACKETS
 ---------------------------------------------------------------------------------------------*/
/* CB log packet */
LOG_RECORD_DEFINE(LOG_UMTS_NAS_CB_LOG_PACKET_C)
uint8 version;
uint8 as_id;
uint8 rat_type;
uint8 msg_type;
uint16 cb_msg_length;
uint8 cb_ota_log_packet[LOG_MAX_NAS_CB_OTA_MESSAGE_SIZE];
LOG_RECORD_END


/*----------------------------------------------------------------------------------------------
                   LOG PACKET: UMTS_NAS_PPLMN_LIST_LOG_PACKET
 ---------------------------------------------------------------------------------------------*/

 
LOG_RECORD_DEFINE(LOG_UMTS_NAS_PPLMN_LIST_LOG_PACKET_C)

  /* Log version */
   uint8             version;

  /* Subscription ID */
   uint8             as_id;

   /* Number of PLMNs in PPLMN List */
   uint16            num_pplmns;
   
   /* List/Array of Preferred PLMNs */
   log_pplmn_type    pplmn_list[LOG_MAX_NUMBER_PREFERRED_PLMNS];

LOG_RECORD_END

/*----------------------------------------------------------------------------------------------
                   LOG PACKET: UMTS NAS ADVANCE SCAN PARAM LOG PACKET
 ---------------------------------------------------------------------------------------------*/

 
LOG_RECORD_DEFINE(LOG_WWAN_ADVANCE_SCAN_PARAM_LOG_C)

  /* Log version */
   uint8             version;

  /* Subscription ID */
   uint8             as_id;

   log_wwan_adavnce_scan_pram_s_type wwan_adavnce_scan_pram;


LOG_RECORD_END


/*----------------------------------------------------------------------------------------------
                   LOG PACKET: UMTS NAS WWAN SCAN RESULT GSM LOG PACKET
 ---------------------------------------------------------------------------------------------*/


LOG_RECORD_DEFINE(LOG_WWAN_SCAN_RESULT_GSM_LOG_C)

  /* Log version */
   uint8             version;

  /* Subscription ID */
   uint8             as_id;

   log_loc_wwan_scan_result_element_s_type wwan_scan_result_gsm;


LOG_RECORD_END


/*----------------------------------------------------------------------------------------------
                   LOG PACKET: UMTS NAS WWAN SCAN RESULT M1 LOG PACKET
 ---------------------------------------------------------------------------------------------*/


LOG_RECORD_DEFINE(LOG_WWAN_SCAN_RESULT_M1_LOG_C)

  /* Log version */
   uint8             version;

  /* Subscription ID */
   uint8             as_id;

   log_loc_wwan_scan_result_element_s_type wwan_scan_result_m1;


LOG_RECORD_END


/*----------------------------------------------------------------------------------------------
                   LOG PACKET: UMTS NAS WWAN SCAN RESULT NB1 LOG PACKET
 ---------------------------------------------------------------------------------------------*/


LOG_RECORD_DEFINE(LOG_WWAN_SCAN_RESULT_NB1_LOG_C)

  /* Log version */
   uint8             version;

  /* Subscription ID */
   uint8             as_id;

   log_loc_wwan_scan_result_element_s_type wwan_scan_result_nb1;


LOG_RECORD_END


#endif
