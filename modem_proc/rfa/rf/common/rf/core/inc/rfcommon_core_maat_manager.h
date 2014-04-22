#if defined(FEATURE_RF_HAS_QTUNER) && defined (FEATURE_RF_LITE_MAAT)
#ifndef RFCOMMON_CORE_MAAT_MANAGER_H
#define RFCOMMON_CORE_MAAT_MANAGER_H
/*!
   @file
   rfcommon_core_maat_manager.h

   @brief
   This file contains enum, typedefs, API function prototypes for
   RF Common Core MAAT Manager
 
*/


/*==============================================================================

  Copyright (c) 2012 - 2019 Qualcomm Technologies Incorporated. All Rights Reserved

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
                              EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/rfa.mpss/3.10/rf/common/rf/core/inc/rfcommon_core_maat_manager.h#1 $

when       who     what, where, why
-------------------------------------------------------------------------------
06/17/19   dh      Fixed compilation warnings
06/13/19   dh      Added support for MAAT LITE feature
04/19/19   dh      Initial version.

============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/

#include "rfm_device_types.h"
#include "rfm_umid_list.h"
#include "rfm_path_types.h"
#include "rfcommon_locks.h"
#include "rf_apps_task.h"
#include "rfcommon_atuner_intf.h"

/* Tech specific header files */
#ifdef FEATURE_LTE
#include "rflte_util.h"
#include "rflte_msg.h"
#endif
#ifdef FEATURE_WCDMA
#include "rfm_wcdma_param_types.h"
#include "rfm_wcdma.h"
#endif

#ifdef FEATURE_CDMA1X
#include "rfm_cdma.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif


/*===========================================================================
                           LIST OF MACROS
===========================================================================*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Macro defining the max number of antennas
*/

#define RFCMN_CORE_MAAT_MAX_NUM_ANTENNAS 1

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Macro defining the max number of carriers
*/

#define RFCMN_CORE_MAAT_MAX_NUM_CARRIERS 1

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Macro defining the max number of rxtx channels
*/

#define RFCMN_CORE_MAAT_MAX_RXTX_CHANNELS 2

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Macro defining the max number of channels
*/

#define RFCMN_CORE_MAAT_MAX_CHANNELS RFCMN_CORE_MAAT_MAX_NUM_CARRIERS*RFCMN_CORE_MAAT_MAX_RXTX_CHANNELS

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Macro defining the max number of measurements

  TODO: Need to check whether it is really needed or not
  
*/

#define RFCMN_CORE_MAAT_MAX_MEASUREMENTS 20

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Macro defining minimum number of measurements needed for evualating the 
  algorithm
*/

#define RFCMN_CORE_MAAT_MIN_NUM_MEASUREMENTS 4

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Macro defining minimum number of critical measurements needed for evualating the 
  algorithm
*/

#define RFCMN_CORE_MAAT_MIN_CRITICAL_NUM_MEASUREMENTS 1

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Macro defining the max metric value
*/

#define RFCMN_CORE_MAAT_MAX_SUMMED_METRIC_VALUE 4


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Macro defining the max metric value
*/

#define RFCMN_CORE_MAAT_TX_MAAT_TIMER_INTERVAL 10

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Macro defining the max metric value
*/

#define RFCMN_CORE_MAAT_TX_MAAT_TIMER_START_INTERVAL 10

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Macro defining max number of maat update log buffers
  Numer of carriers*(Number of RX chains + Numer of Tx chains) * 20
  (5*(4+2)*20) = 
*/

#define RFCMN_CORE_MAAT_UPDATE_MAX_NUM_LOG_BUF 40

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Macro defining minimum TX POWER in db10 required for MAAT algorithm.
  
*/
#define RFCMN_CORE_MAAT_ALGO_MIN_TX_POWER_DB10 -500


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Macro defining invalid EcIo value that will be provided to MAAT when FLDRx is enabled.
  
*/
#define RFCMN_CORE_MAAT_INVALID_C2K_EcIo 0x7FFF



/*===========================================================================
                           LIST OF ENUMS
===========================================================================*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enum for MAAT update ID
*/

typedef enum
{
  /*! For updating  Tx metrics */
  RFCMN_CORE_MAAT_UPDATE_TX = 0,

  /*! For updating  Rx metrics */
  RFCMN_CORE_MAAT_UPDATE_RX = 1,

} rfcmn_core_maat_update_id_type;


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enum for MAAT different Tx/Rx type
*/

typedef enum
{
  /*! Tx0 */
  RFCMN_CORE_MAAT_TX,
  
  /*! Rx0 */
  RFCMN_CORE_MAAT_RX,

  RFCMN_CORE_MAAT_MAX_RXTX = RFCMN_CORE_MAAT_MAX_RXTX_CHANNELS,
  
} rfcmn_core_maat_rx_tx_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enum for MAAT Metric status
  The enum values are used to caluclate the summed metric value 
*/

typedef enum
{
  /*! Metric inconclusive */
  RFCMN_CORE_MAAT_INCONCLUSIVE = 0,

  /*! Metric noncritical */
  RFCMN_CORE_MAAT_NONCRITICAL = RFCMN_CORE_MAAT_INCONCLUSIVE,

  /*! Metric critical */
  RFCMN_CORE_MAAT_CRITICAL = 1

} rfcmn_core_maat_metric_status_type;

/*----------------------------------------------------------------------------*/
/*
  @brief
  This enum defines all cmds that need rf_apps_task to dispatch. To add a new
  cmd to rf_apps_task, one need to add a new cmd in this enum, and then add
  the corresponding cmd handler in rf_apps_cmd_dispatch().
*/
typedef enum
{
  /*Tx MAAT timer expiry  cmd*/
  RFCMN_CORE_MAAT_TX_MAAT_TIMERY_EXPIRY_CMD,

  /* L1 Update*/
  RFCMN_CORE_MAAT_L1_UPDATE_CMD,  

  /* End of adding new cmd*/
  RFCMN_CORE_MAAT_APPS_TASK_MAX_CMD,

}rfcmn_core_maat_apps_task_cmd_enum_type;



/*===========================================================================
                           LIST OF DATA STRUCTURES 
===========================================================================*/
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Data Structure for MAAT Tech Tx data type
*/
typedef union
{
  #ifdef FEATURE_LTE
  rfa_rf_lte_maat_tx_info_type_s lte_maat_data;
  #endif

}rf_maat_tx_info_type_u;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Data Structure for MAAT Tech Rx data type
*/
typedef union
{
  #ifdef FEATURE_LTE
  rfa_rf_lte_maat_info_type_s lte_maat_data;
  #endif

}rf_maat_rx_info_type_u;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Data Structure for MAAT logical channel type
*/
typedef struct 
{
  /* Carrier ID of the channel */ 
  uint8 carrier_id;
 
  /* rfm_mode */
  rfm_mode_enum_type rfm_mode;

  /* MAAT rx_tx type */
  rfcmn_core_maat_rx_tx_type rx_tx_id;
  
}rfcmn_core_maat_channel_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Data Structure for MAAT logical channel data
*/
typedef struct 
{
  /* Flag to tell whether the channel is valid or not */
  boolean channel_valid;
  
  /* Channel type */
  rfcmn_core_maat_channel_type channel_type;

  /* earfcn */
  uint32 earfcn;

  /* DB10 data
  SNR_db10 for MAAT channel type (x,y,RFCMN_CORE_MAAT_CHAN_RX)
  txagc_db10 for MAAT channel type (x,y,RFCMN_CORE_MAAT_CHAN_TX) 
  where x- carrier_id, y- rfm_mode */
  int32 data_db10[RFCMN_CORE_MAAT_MAX_MEASUREMENTS];

  /* data buff idx */
  uint8 data_buff_idx;

  /* avearage_data_db10 */
  int32 summed_data_db10;
  
  /* Measurement Counter */ 
  uint8 meas_counter;

  /* Critical Counter */
  uint8 critical_counter;

  /* Timestamp when the last critical measurement is recorded */
  rf_time_tick_type critical_time_stamp;
  
  /* headroom_db10 */
  int16 headroom_db10;

  /* critical_threshold_db10 */
  int16 critical_th_db10;

  /* threshold_hysteresis_db10 */
  int16 th_hys_db10;

  /* tx_integration_time_ms */
  uint16 integ_time_ms;

  /* Metric status */
  rfcmn_core_maat_metric_status_type metric_status;

  /* Boolean to signify to apply hysteresis to cricital threshold or not */
  boolean apply_hys;

}rfcmn_core_maat_channel_data_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Data Structure Structure for MAAT static data 
*/
typedef struct 
{
  /* Starting optimization algorithm for MAAT */
  uint8 start_maat_priority;

  /* tx_headroom_db10 */
  int16 tx_headroom_db10;

  /* rx_headroom_db10 */
  int16 rx_headroom_db10;

  /* ca_headroom_db10 */
  int16 ca_headroom_db10;

  /* tx_critical_threshold_db10 */
  int16 tx_critical_th_db10;

  /* rx_critical_threshold_db10 */
  int16 rx_critical_th_db10;

  /* ca_rx_critical_threshold_db10 */
  int16 ca_rx_critical_th_db10;

  /* tx_threshold_hysteresis_db10 */
  int16 tx_th_hys_db10;

  /* rx_threshold_hysteresis_db10 */
  int16 rx_th_hys_db10;

  /* tx_integration_time_ms */
  uint16 tx_integ_time_ms;

  /* rx_integration_time_ms */
  uint16 rx_integ_time_ms;

}rfcmn_core_maat_static_data_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Data Structure Structure for MAAT antenna data 
*/
typedef struct 
{
  /* Num of active channels */ 
  uint8 num_active_channels;

  /* Per channel data */
  rfcmn_core_maat_channel_data_type channels_data[RFCMN_CORE_MAAT_MAX_CHANNELS];

  /* MAAT static data per antenna */ 
  rfnv_atuner_maat_static_settings_type static_data;

  /* Current MAAT ALGO */
  rfcommon_atuner_maat_algo_type maat_algo_type;

  /* Flag to tell whether we can start the MAAT measurements */ 
  boolean meas_start;
  
}rfcmn_core_maat_antenna_data_type;


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure for logging the MAAT update
*/
typedef struct 
{
  /*! Time stamp when measurements are recorded */
  rf_time_tick_type time_stamp;

  /*! rfm path */
  rfm_path_type rfm_path;
  
  /*! Ant_num */
  uint32 ant_num;

  /*! Carrier ID of the channel */ 
  uint8 carrier_id;

  /*! rfm_mode */
  rfm_mode_enum_type rfm_mode;

  /*! MAAT rx_tx type */
  rfcmn_core_maat_rx_tx_type rx_tx_id;

  /*! erfcn */  
  uint32 earfcn;

  /*! data_db10 data */
  int32 data_db10;

  /*! meas_start */
  boolean meas_start;

}rfcmn_core_maat_update_log_buffs_type;


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure for MAAT MGR log buffer
*/
typedef struct 
{
  /*!flag to tell whether data can flush into logpacket */
  boolean flush_data;

  /*! Ant_num */
  uint32 ant_num;
  
  /*! Antenna data */
  rfcmn_core_maat_antenna_data_type antenna_dat;

  /*! current_maat_update_buf_idx */
  uint16 current_maat_update_buf_idx;

  /*! num_maat_updates */
  uint16 num_maat_updates;

  /*! MAAT update buffers */
  rfcmn_core_maat_update_log_buffs_type maat_update_data_buf[RFCMN_CORE_MAAT_UPDATE_MAX_NUM_LOG_BUF];
  
}rfcmn_core_maat_mgr_log_buffs_type;


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Global Structure for managing all the antennas assoicated with the manager
*/
typedef struct 
{
  /*! Antenna */
  rfcmn_core_maat_antenna_data_type antenna_data[RFCMN_CORE_MAAT_MAX_NUM_ANTENNAS];
  
  /*! Critical section lock for MAAT Manager */
  rf_lock_data_type lock;

  /*! Boolean to track if Tx timer is running or not */
  boolean tx_timer_enabled;

  /*! Timer for getting the Tx MAAT info */
  timer_type maat_tx_timer;

  /*! Buffer for logging */
  rfcmn_core_maat_mgr_log_buffs_type maat_log_buff[RFCMN_CORE_MAAT_MAX_NUM_ANTENNAS];

}rfcmn_core_maat_mgr_type;


/*===========================================================================
                           LIST OF INTERNAL INTERFACES
===========================================================================*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This is the Internal API used to parse the MAAT metrics data and store 
  per antenna
  
  @param tech
  rfm_mode

  @param update_ID
  update_ID Rx or Tx

  @param payload_ptr
  ptr to the payload
  
  @return
  NONE 
*/
void rfcmn_core_maat_parse_metrics_data
(
  rfcom_mode_enum_type tech,
  uint8 update_ID,
  void *payload_ptr
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This is the Internal API used to store the MAAT metrics data per antenna

  @param rfm_path
  rfm_path

  @param ant_num
  ant_number

  @param curr_channel_type
  current MAAT logical channel type

  @param earfcn
  earfcn channel number

  @param db10_data
  db10 data SNR or TXAGC
  
  @return

*/
void rfcmn_core_maat_store_metrics_data
(
  rfm_path_type rfm_path,
  uint32 ant_num,
  rfcmn_core_maat_channel_type curr_chan_type,
  uint32  earfcn,
  int32 db10_data
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This is the internal API used to update the static parameters from antenna data
  into channel data based on the channel type
  
  @param ant_num
  ant_num

  @param chan_idx
  channel_idx that needs update
  
  @return

*/
void rfcmn_core_maat_update_static_parameters
(
  uint8 ant_num,
  uint8 chan_idx
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This is the internal API used get the maat algorithm weightage factor for the 
  given channel type
  
  @param channel_type
  MAAT  logical channel type

  
  @return

*/
uint8 rfcmn_core_maat_get_channel_weightage_factor
(
  rfcmn_core_maat_channel_type channel_type
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This is the internal API used get the maat algo idx for the given 
  summed metric
  
  @param summed_metric
  ant_num
  
  @return
  rfcommon_atuner_maat_algo_type

*/
rfcommon_atuner_maat_algo_type rfcmn_core_maat_get_algo_idx_for_given_metric
(
  uint8 summed_metric
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This is the internal API used to average the measurements for the given channel
  
  @param channel_data
  rfcmn_core_maat_channel_data_type
  
  @return

*/
void rfcmn_core_maat_run_measurement_filter
(
  rfcmn_core_maat_channel_data_type *channel_data
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This is the internal API used to flush the maat log buf into the MAAT log
  packet
    
  @return
  void

*/
void rfcmn_core_maat_flush_maat_log_buff
(
  uint32 ant_num
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This is the internal API used to log the maat update in maat log buffer
  
  @param rfm_path
  rfm_path

  @param ant_num
  ant_number

  @param curr_channel_type
  current MAAT logical channel type

  @param earfcn
  earfcn channel number

  @param db10_data
  db10 data SNR or TXAGC
  
  @return
  void

*/
void rfcmn_core_maat_log_maat_update
(
  rfm_path_type rfm_path,
  uint32 ant_num,
  rfcmn_core_maat_channel_type curr_channel_type,
  uint32  earfcn,
  int32 db10_data
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This is the internal API use to log the MAAT manager for the given antenna into the maat log buffer
  
  @param channel_type
  MAAT  logical channel type
  
  @return
  void

*/
void rfcmn_core_maat_log_maat_mgr
(
  uint32 ant_num
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  API to check if data for corresponding channel is critical or not
  
  @param channel_data
  Channel Data
  
  @return
  boolean : TRUE if SNR is critical
            FALSE if SNR is not critical

*/
boolean rfcmn_core_maat_is_maat_critical
(
  rfcmn_core_maat_channel_data_type *channel_data
);


/*===========================================================================
                           LIST OF EXTERNAL INTERFACES
===========================================================================*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This is the interface API to Initialize the RF Common Core MAAT Manager from
  RF Common Core Init.

  @details
  

  @return
  TRUE Or FALSE
*/
boolean rfcmn_core_maat_mgr_init();

/*----------------------------------------------------------------------------*/
/*!
  @brief
  call back function registered with the MAAT Tx timer
  This function will be called whenever the timer expired
  
  @param t_unused
  t_unused is unused - hence the name 
 
  @param data
  data passed from the timer. Tuner port instance will be passed
  
  @return
  void
  
*/
void rfcmn_core_tx_maat_timer_expiry_handler
(
 timer_cb_data_type data
);


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This is the interface API used to start the MAAT data processing for the 
  given antenna

  @param ant_num
  Antenna number
 
  @param static_data
  MAAT static data of the antenna
 
  @param current_maat_algo
  current MAAT Algorithm
 
  @return
  TRUE or FALSE
  
*/
boolean rfcmn_core_maat_start
(
  uint32 ant_num,
  rfnv_atuner_maat_static_settings_type *static_data,
  rfcommon_atuner_maat_algo_type current_maat_algo
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This is the interface API used to get the new MAAT algo idx and summed_metric
  
  @param ant_num
  Antenna number
  
  @param summed_metric_ptr
  pointer to the summed_metric
  
  @return
  TRUE or FALSE
*/
rfcommon_atuner_maat_algo_type rfcmn_core_maat_get_algo_idx
(
  uint32 ant_num,
  uint8* summed_metric_ptr
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This is the interface API used to re-set the MAAT data processing for the 
  given antenna

  TODO: Discuss with Yogesh and re-write the API
  
  @param ant_num
  Antenna number
 
  @param static_data
  MAAT static data of the antenna
 
  @param current_maat_algo
  current MAAT Algorithm
 
  @return
  TRUE or FALSE
*/

boolean rfcmn_core_maat_reset
(
  uint32 ant_num
);

/* API to enable Tx information timer for MAAT*/
void rfcmn_core_maat_enable_tx_timer();

/* API to disable Tx information timer for MAAT*/
void rfcmn_core_maat_disable_tx_timer();

uint8 rfcmn_core_maat_get_maat_scenario_from_given_algo_type
(
  rfcommon_atuner_maat_algo_type maat_algo_type
);

#ifdef __cplusplus
}
#endif
#endif
#endif

