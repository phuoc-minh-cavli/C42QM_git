#ifndef RFM_GNSS_H
#define RFM_GNSS_H
/*
   @file
   rfm_gnss.h

   @brief
   RF Driver's GNSS external interface file.

   @details

*/

/*===========================================================================

Copyright (c) 1999 - 2002, 2006, 2007 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2008-2020 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/rfa.mpss/3.10/api/gnss/rfm_gnss.h#2 $
$DateTime: 2020/08/12 02:21:06 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/07/20   akm     API for NavRx get device id info
04/12/19   aj      Provide Wrapper for rfc_gnss_is_mb_path_supported API to GNSS
02/06/19   Om      Provide Wrapper for rfc_common_set_grfc API to GNSS
02/05/19   Om      Expose standalone API to GNSS to GRFC signal info as well for GNSS path
02/04/19   Om      Expose API to GNSS for getting RFC HWID
08/27/18   aj      Expose standalone API to GNSS to return rfc signal info for WGR
10/17/14   hs      Added WTR4905 command support.
08/15/14   lm      Added GNSS B20/28 mode in rfm_gnss_device_seq_cmd_type 
06/26/14   hs      Added WTR4905 command support.
10/17/13   jn(rs)  Added WTR3925 command support.
08/28/13   jn      Added API to support navrf band tuning
07/22/13   gy      Added support to configure eLNA state
05/03/13   gy      Added GNSS RF STATUS and HEALTH commands
04/30/13   gy      Added API for GPS to vote rfc common to wakeup or sleep
04/30/13   gy      Added GNSS M5HT and M5ET modes
04/09/13   dbz     Add new bootup FE and gain calibration API
02/11/13   dbz/gy  Added API to disable/enable gnss pga gain backoff
02/11/13   gy      Added gnss rfsw repartition
08/02/11   dbz     Add new API for GNSS PGA tracking loop control
09/03/10   hp      Update to LTE GPS B13 Gain Adjust Algorithm  
06/30/10   hp      Added Support for LTE GPS B13 Gain Adj for SVLTE-Fusion
06/25/10   gy      Added GNSS Demod Quality API 
01/19/10   gy      Removed Gen7 rfm api
01/06/10   dbz     Added rfm_gnss_debug support
10/15/09   hp      Added function prototype needed to support ELNA API
03/10/09   gy      Code clean up
12/20/08   gy      Initial version to port gnss specific external interface
10/14/08   sr      Initial version to separate gps specific external interface.

============================================================================*/


#include "rfcom.h"
#include "rfc_msm_typedef.h"
#include "rfdevice_chip_ids.h"


#ifdef __cplusplus
extern "C"{
#endif

typedef enum
{
  RFGNSS_DEVICE_CMD_M5_LL_INIT = 0,
  /* rfm_gnss_device_seq_execute(RFGNSS_DEVICE_CMD_M5_LL_INIT, NULL) */
  RFGNSS_DEVICE_CMD_M5_ELNA_INIT,
  /* rfm_gnss_device_seq_execute(RFGNSS_DEVICE_CMD_M5_ELNA_INIT, NULL) */
  RFGNSS_DEVICE_CMD_M5_HL_INIT,
  /* rfm_gnss_device_seq_execute(RFGNSS_DEVICE_CMD_M5_HL_INIT, NULL) */
  RFGNSS_DEVICE_CMD_M5HT_INIT,
  /* rfm_gnss_device_seq_execute(RFGNSS_DEVICE_CMD_M5HT_INIT, NULL) */
  RFGNSS_DEVICE_CMD_M5ET_INIT,
  /* rfm_gnss_device_seq_execute(RFGNSS_DEVICE_CMD_M5ET_INIT, NULL) */
  RFGNSS_DEVICE_CMD_M5ET1_INIT,
  /* rfm_gnss_device_seq_execute(RFGNSS_DEVICE_CMD_M5ET1_INIT, NULL) */
  RFGNSS_DEVICE_CMD_MTE_INIT,
  /* rfm_gnss_device_seq_execute(RFGNSS_DEVICE_CMD_MTE_INIT, NULL) */
  RFGNSS_DEVICE_CMD_COMPASS_ELNA_INIT,
  /* rfm_gnss_device_seq_execute(RFGNSS_DEVICE_CMD_COMPASS_ELNA_INIT, NULL) */
  RFGNSS_DEVICE_CMD_SLEEP,
  /* rfm_gnss_device_seq_execute(RFGNSS_DEVICE_CMD_SLEEP, NULL) */
  RFGNSS_DEVICE_CMD_RESUME,
  /* rfm_gnss_device_seq_execute(RFGNSS_DEVICE_CMD_RESUME, NULL) */
  RFGNSS_DEVICE_CMD_PAUSE,
  /* rfm_gnss_device_seq_execute(RFGNSS_DEVICE_CMD_PAUSE, NULL) */
  RFGNSS_DEVICE_CMD_M5_IF,
  RFGNSS_DEVICE_CMD_M5_BP1_DOWNCONV_IF,
  RFGNSS_DEVICE_CMD_M5_BP3_DOWNCONV_IF,
  RFGNSS_DEVICE_CMD_CHIP_OFF,
  RFGNSS_DEVICE_CMD_ENABLE_TG,
  RFGNSS_DEVICE_CMD_DISABLE_TG,
  RFGNSS_DEVICE_CMD_WRITE_BT_CODE,
  RFGNSS_DEVICE_CMD_SET_TXATTN,
  RFGNSS_DEVICE_CMD_WRITE_SEL_CODE,
  RFGNSS_DEVICE_CMD_DCOC_WRITE_FINE_DAC,
  RFGNSS_DEVICE_CMD_DCOC_WRITE_COARSE_DAC,
  RFGNSS_DEVICE_CMD_DCOC_FINE1_PRE_CONFIG,
  RFGNSS_DEVICE_CMD_DCOC_FINE1_POST_CONFIG,
  RFGNSS_DEVICE_CMD_DCOC_COARSE_PRE_CONFIG,
  RFGNSS_DEVICE_CMD_DCOC_FINE2_PRE_CONFIG,
  RFGNSS_DEVICE_CMD_DCOC_FINE2_POST_CONFIG,
  RFGNSS_DEVICE_CMD_DCOC_READ_FINE_DAC,
  RFGNSS_DEVICE_CMD_DCOC_READ_COARSE_DAC,
  RFGNSS_DEVICE_CMD_PGA_WRITE,
  RFGNSS_DEVICE_CMD_WRITE_R_TUNE,
  RFGNSS_DEVICE_CMD_WRITE_RC_TUNE,
  RFGNSS_DEVICE_CMD_SET_MIXER_NOTCH,
  RFGNSS_DEVICE_CMD_GET_RF_STATUS,
  RFGNSS_DEVICE_CMD_GET_RF_HEALTH,
  RFGNSS_DEVICE_CMD_GET_CHIP_REVISION,
  RFGNSS_DEVICE_CMD_GET_CHIP_PROCESS,
  RFGNSS_DEVICE_CMD_IELNA_CONFIG,    /* WFR */
  RFGNSS_DEVICE_CMD_GET_RF_STATUS1,
  RFGNSS_DEVICE_CMD_GET_RF_HEALTH1,
  RFGNSS_DEVICE_CMD_GET_LNA_STATE,
  RFGNSS_DEVICE_CMD_CALCULATE_BT_GNSS_ELNA,
  RFGNSS_DEVICE_CMD_QFUSE_READ_GPS,
  RFGNSS_DEVICE_CMD_ADC_CAL_PREP,
  RFGNSS_DEVICE_CMD_ADC_CAL_AFTER,
  RFGNSS_DEVICE_CMD_SE_DCOC,
  RFGNSS_DEVICE_CMD_ENTER_2H_MODE,
  RFGNSS_DEVICE_CMD_EXIT_2H_MODE,
  RFGNSS_DEVICE_CMD_ENTER_B20_B28_MODE,
  RFGNSS_DEVICE_CMD_EXIT_B20_B28_MODE,   
  RFGNSS_DEVICE_CMD_SET_FINE_GAIN
} rfm_gnss_device_seq_cmd_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enum for of Device Status
*/
typedef enum
{
  /*! var that specifies Device is OFF */
  RFGNSS_DEVICE_OFF = 0,
  /*! var that specifies Device is ON */
  RFGNSS_DEVICE_ON = 1

} rfgnss_device_status_type;

/*!
  @brief
  Enum for of Vote type on a specific Device
*/
typedef enum
{
  /*! var that specifies No vote */
  RFGNSS_DEVICE_NO_VOTE = 0,

  /*! var that specifies Normal vote to be used by all techs except GPS */
  RFGNSS_DEVICE_NORMAL_VOTE,

  /*! var that specifies Critical vote to be used by GPS tech only */
  RFGNSS_DEVICE_CRITICAL_VOTE = RFGNSS_DEVICE_NORMAL_VOTE,

  /*! var that specifies Non-Critical vote to be used by GPS tech only */
  RFGNSS_DEVICE_NON_CRITICAL_VOTE,

} rfgnss_device_vote_type;

/*!
  @brief
  Device register type
*/
typedef struct
{
  uint8 r1;
} rfgnss_device_reg1_type;

/*!
  @brief
  Device register type
*/
typedef struct
{
  uint8 i;
  uint8 q;
} rfgnss_device_regiq_type;

/*!
  @brief
  Device register type
*/
typedef struct
{
  uint8 r1;    /* i or r_tune */
  uint8 r2;    /* q or tia */
} rfgnss_device_reg2_type;

/*!
  @brief
  Device register type
*/
typedef struct
{
  uint8 r1;   /* hpc */
  uint8 r2;   /* bba */
  uint8 r3;   /* abuf */
} rfgnss_device_reg3_type;

/*!
  @brief
  Device register type
*/
typedef struct
{
  uint8 r1;   /* hpc */  
  uint8 r2;   /* tia */
  uint8 r3;   /* bba */
  uint8 r4;   /* abuf */
} rfgnss_device_reg4_type;

/*!
  @brief
  Device dpo register type
*/
typedef struct
{
  uint8 r_tune;
  uint8 mixer_notch_code;
  uint8 code_hpc;
  uint8 code_tia;
  uint8 code_bba;
  uint8 code_abuf;
  uint8 pga;
  uint8 i_dac_fine;
  uint8 q_dac_fine;
  uint8 i_dac_coarse;
  uint8 q_dac_coarse;
  uint8 bt_code;
  uint8 sel_code;
  uint8 dcoc_1;
  uint8 dcoc_2;
} rfgnss_device_dpo_reg_type;

/*!
  @brief
  Device kep register type
*/
typedef struct
{
  uint8 npler23;
  uint8 reg01;
  uint8 reg04;
  uint8 reg25;
  uint8 reg24;
  uint8 reg16;
  uint8 reg17;
  uint8 reg15;
  uint8 reg13;
  uint8 reg14;
  uint8 reg12;
  uint8 reg11;
  uint8 reg23;
  uint8 reg1f;
  uint8 pll_bank;
  uint8 pll_ld;
} rfgnss_device_key_reg_type;

typedef enum
{
  RFM_RFC_LOW,
  RFM_RFC_HIGH,
  RFM_RFC_CONFIG_ONLY,
  RFM_RFC_LOGIC_NUM,
  RFM_RFC_LOGIC_INVALID = 0xFFFFFFFF
} rfm_rfc_logic_type;


typedef struct
{
  rfm_rfc_logic_type logic;
  int32 time_offset;
} rfm_rfc_sig_timing_info_type;

/*!
  @brief
  GNSS rfc signal info type
*/
typedef struct
{
  rfc_gpio_grfc_type sig_type;
  uint8 grfc_id;
  uint8 gpio_id;
  rfm_rfc_sig_timing_info_type start;
  rfm_rfc_sig_timing_info_type stop;
} rfgnss_rfc_signal_info_type;

typedef struct
{
  uint8 num_rfc_signals;
  rfgnss_rfc_signal_info_type *rfc_signals;
} rfgnss_rfc_signal_list_type;

/*----------------------------------------------------------------------------*/
/*!
   @brief
   get rfc gnss signal info

   @param rf_signals

   @retval None
*/
void rfm_get_gnss_rfc_signal_list (rfgnss_rfc_signal_list_type *rf_signal_list);
/*----------------------------------------------------------------------------*/
/*!
   @brief
   get rfc gnss port status

   @param none

   @retval None
*/

boolean rfm_is_mb_path_supported(void);

rf_hw_type rfm_rfcommon_nv_get_rf_hwid(void);

/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*!
   @brief
   Configure GNSS mode operation after entering gnss mode.
*/
void rfm_gnss_configure (rfcom_device_enum_type device, rfgnss_mode_cfg_enum_type rfgnss_mode);

/*----------------------------------------------------------------------------*/
/*!
   @brief
   Exit gnss operation.
*/

void rfm_gnss_exit ( rfcom_device_enum_type device );

/*----------------------------------------------------------------------------*/
/*!
   @brief
   Resume gnss operation after entering gnss pause mode.
*/
void rfm_gnss_resume(rfcom_device_enum_type device);

/*----------------------------------------------------------------------------*/
/*!
   @brief
   Pause gnss operation for fts or dpo.
*/
void rfm_gnss_pause(rfcom_device_enum_type device);

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function is periodically called by gnss software. The RF driver will then
   query the gnss software for dc and amplitude values and update the RF as needed.
*/
void rfm_gnss_trigger_bbc_update(void);

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This API sets up the environment based of presence or absence or an external LNA.
   It is the OEM's responsibility to properly call the API before the start of GNSS session.
   If this API is not called, then the RFSW assumes no external LNA
*/
void rfm_gnss_has_ext_lna (boolean gnss_has_ext_lna);

/*----------------------------------------------------------------------------*/
/*!
   @brief

   @details
  This function is used for debug purposes. The function queries
  the GPS state and returns TRUE if GPS is ON and FALSE if GPS is
  turned OFF.

   @param device : RF Multimode device

          Boolean - TRUE if GPS State is ON and FALSE if turned OFF.
*/
boolean rfm_gnss_debug(boolean verbose, rfcom_device_enum_type device);

/*----------------------------------------------------------------------------*/
/*!
   @brief
   Perform GNSS demod quality control

*/
void rfm_gnss_demod_quality_control(rfgnss_demod_quality_enum_type rfgnss_demod_quality);

/*----------------------------------------------------------------------------*/
/*!
   @brief
   GNSS PGA tracking loop control

*/
void rfm_gnss_pga_tracking_loop_control(boolean enable);

/*----------------------------------------------------------------------------*/
/*!
   @brief
   GNSS vote for dev on/off

*/
boolean rfm_gnss_resource_vote(rfgnss_device_status_type dev_status, rfgnss_device_vote_type dev_vote);

/*----------------------------------------------------------------------------*/
/*!
   @brief
   GNSS device sequence execute command dispatch

*/
boolean rfm_gnss_device_seq_execute(rfm_gnss_device_seq_cmd_type cmd, void *data);

/*----------------------------------------------------------------------------*/
/*!
   @brief
   API to disable/enable GNSS PGA gain backoff

*/
void rfm_gnss_pga_gain_backoff_control(boolean enable);

/*----------------------------------------------------------------------------*/
/*!
   @brief
 
   @details
   This function is used to initiate GNSS boot-up calibration from
   GNSS SW once NPA resources are available and GNSS SW can turn clocks
   ON.

*/
void rfm_gnss_bootup_cal (rfcom_device_enum_type device);

/*----------------------------------------------------------------------------*/
/*!
   @brief
   GNSS vote for ccs enable

*/
boolean rfm_gnss_hal_bus_enable(boolean enable);


/*----------------------------------------------------------------------------*/
/*!
  @brief
  API for NavRx initialize the RF device using default mode

  @return boolean Success or Failure
*/
boolean rfm_gnss_init_dev(void);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  API for NavRx initialize the RF device regarding the choosen GNSS mode

  @param rfgnss_mode_config_type rf_mode
         LNA and saw filter mode config

  @return boolean Success or Failure
*/
boolean rfm_gnss_init_dev_mode(rfgnss_mode_config_type rf_mode);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  API for NavRx to set the PGA gain

  @param gain_hwu
  HW units need to be set to PGA register

  @return 
  boolean Success or Failure
*/
boolean rfm_gnss_set_pga(uint16 gain_hwu);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  API for NavRx to dump gnss register for debug purpose

  @param data 
  pointer to structure stores values

  @return 
  boolean Success or Failure
*/
boolean rfm_gnss_reg_dump(uint32 *data);


/*----------------------------------------------------------------------------*/
/*!
  @brief
  API for NavRx to congfigure LTE B13 2H mode using default mode

  @param boolean cfg
         True - enter mode
         False - exit mode

  @return boolean Success or Failure
*/
boolean rfm_gnss_configure_lte_b13_2h_mode(boolean cfg);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  API for NavRx to congfigure LTE B13 2H mode regarding the choosen GNSS mode

  @param boolean cfg
         True - enter mode
         False - exit mode
  @param rfgnss_mode_config_type rf_mode
         LNA and saw filter mode config

  @return boolean Success or Failure
*/
boolean rfm_gnss_configure_lte_b13_2h_mode_v2(boolean cfg, rfgnss_mode_config_type rf_mode);


/*----------------------------------------------------------------------------*/
/*!
  @brief
   API for NavRF to vote for RFC wakeup or sleep

  @param boolean enable
         True - wakeup
         False - sleep

  @return boolean Success or Failure
*/
boolean rfm_gnss_rfc_common_enable(boolean enable);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get GPS state

  @param 

  @return boolean ON or OFF
*/
boolean rfm_gnss_get_state(void);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  API for NavRx to set the Rx fine gain code

  @param gain_hwu
  HW units need to be set to Rx fine gain register

  @return 
  boolean Success or Failure
*/
boolean rfm_gnss_set_rx_fine_gain(uint16 fine_gain_hwu);

/*----------------------------------------------------------------------------*/
 /*!
  @brief
  Configure ASM device for GNSS Rx path.
  @param enable
  TRUE for connecting the switch or FALSE for disconnecting the switch with SDR.

  @return 
  boolean Success or Failure
 */
boolean rfm_gnss_asm_dev_enable( boolean enable );

/*----------------------------------------------------------------------------*/
/*!
  @brief
  API for NavRx get device id info

  @param
  @return rfdevice_id_enum_type device id
*/
rfdevice_id_enum_type rfm_gnss_get_dev_id(void);


/*----------------------------------------------------------------------------*/
/*!
  @brief
  API for NavRx to turn off the RF chip

  @return boolean Success or Failure
*/
boolean rfm_gnss_chip_off(void);


boolean rfm_rfc_common_set_grfc (uint8 grfc, rfm_rfc_logic_type logic);

#ifdef __cplusplus
}
#endif


#endif /* RFM_GNSS_H */
