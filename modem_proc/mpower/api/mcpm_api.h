#ifndef __MCPM_API_H__
#define __MCPM_API_H__

/*=========================================================================

           M O D E M   C L O C K   A N D   P O W E R   M A N A G E R

                E X T E R N A L   H E A D E R   F I L E


  GENERAL DESCRIPTION
    This file contains the external interface functions and definitions for
    Modem Clock and Power Manager (MCPM).

  EXTERNALIZED FUNCTIONS
    MCPM_Config_Modem

  INITIALIZATION AND SEQUENCING REQUIREMENTS
    Invoke the MCPM_Init function to initialize the MCPM.

         Copyright (c) 2019 by QUALCOMM Technologies Inc.  All Rights Reserved.


==========================================================================*/

/*==========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/mpower.mpss/11.0/api/mcpm_api.h#21 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------
05/20/17   mh      Add NUM RAT TECH enum
02/20/17   mh      NB IOT parameters are added
10/16/15   mh      Addition of MSS split rail related changes 
08/20/15   ne      add support for SVS_PLUS and NOM_PLUS corners
08/10/15   mh      Added MCPM_Config_Modem_Try() public API. 
10/17/14   mh      Remove WCDMA EDRX request enums
10/16/14   cl      Adding support for Q6 345.6MHz and 370MHz
10/14/14   mh      Fixed merge issues
09/04/14   sc      Added support for CMCC power optimization feature.
08/11/14   ss      Map LTE data to LTE FDD data
07/17/14   ss      Add MCVS parameters for Scalar and Vector Q6 processors
06/19/14   cl      Add API to support W-L1 team query for Q6 speed
05/04/14   bd      GPIO Protection API Support
05/05/14   cl      First Jolokia official release branch off Bolt tip
04/25/14   ls      FW_VPE MCVS client to support LTE NLIC.
03/21/14   sr      G + G DSDS multi-sim support.
02/13/14   ls      LTE VoLTE and CDRX support.
01/17/14   ls      Support for LTE's MCPM request for TDD and FDD DATA on Bolt.
09/27/13   sr      Corrected formatting of some comments.
09/03/13   ls      Added a new clk_mdm freq enum.
07/31/13   sr      Removed GSM apis
07/26/13   sr      Added enum MCPM_GSM_DL_NONEV.
07/08/13   ls      Added more APIs (request type, DL/UL/BW data rate).
02/11/13   vs      Ported from Dime release branch.


==========================================================================*/


/*==========================================================================

                     INCLUDE FILES FOR MODULE

==========================================================================*/


#include "customer.h"
#include "comdef.h"
#include "msg.h"
#include "npa.h"
#include "icbarb.h"
#include "rfm_mode_types.h"

/*=========================================================================
      Macros for the MCPM clk/pll dump
==========================================================================*/


/*
 * Bit 0: "RECEIVER_CONFIG" field is updated
 * Bit 1: "NEIGHBOR_MEAS" field is updated
 * Bit 2: "DL_DATA_RATE" field is updated
 * Bit 3: "UL_DATA_RATE" field is updated
 * Bit 4: "RF_BANDWIDTH" field is updated
 */
#define MCPM_NO_UPDATE                 0x000
#define MCPM_RECEIVER_CONFIG_UPDATE    0x001
#define MCPM_NEIGHBOR_MEAS_UPDATE      0x002
#define MCPM_DL_DATA_RATE_UPDATE       0x004
#define MCPM_UL_DATA_RATE_UPDATE       0x008
#define MCPM_RF_BANDWIDTH_UPDATE       0x010
#define MCPM_WKTIME_RF_UPDATE          0x020
#define MCPM_WKTIME_RSRC_UPDATE        0x040
#define MCPM_FCW_DCO_RESET_UPDATE      0x080
#define MCPM_MODEM_PERF_MODE_UPDATE    0x100
#define MCPM_MCVS_SCALE_UPDATE         0x200
#define MCPM_SLEEP_CLK_SOURCE_UPDATE   0x400
#define MCPM_TCM_STATE_UPDATE          0x800


/*
 * MCVS Param update info
 */
#define MCPM_MCVS_SILVER_CLK_UPDATE  0x001
#define MCPM_MCVS_CLKBUS_UPDATE        0x002   // Clk_Bus is running at one fixed level. So, not entertaining this req
#define MCPM_MCVS_L1S_MEM_CLK_UPDATE 0x004
#define MCPM_MCVS_Q6_CLK_UPDATE      0x008

#define MCPM_MCVS_SCALAR_PROC_CLK_UPDATE MCPM_MCVS_Q6_CLK_UPDATE
#define MCPM_MCVS_VECTOR_PROC_CLK_UPDATE MCPM_MCVS_SCALAR_PROC_CLK_UPDATE
#define MCPM_MCVS_VPE_UPDATE         MCPM_MCVS_SCALAR_PROC_CLK_UPDATE
#define MCPM_MCVS_FW_VPE_UPDATE          0x010
#define MCPM_MCVS_TDEC_UPDATE         0x0020


/*
 * RECEIVER_CONFIG
 * Bit 0: RXD (ON/OFF)
 * Bit 1: EQ (ON/OFF)
 * Bit 2: QICE (ON/OFF)
 * Bit 3: W-MIMO (ON/OFF)
 */

#define MCPM_RECEIVER_CONFIG_RXD       0x01  /* 1 rxd ON, 0 rxd OFF */
#define MCPM_RECEIVER_CONFIG_EQ        0x02  /* 1 EQ ON, 0 EQ OFF */
#define MCPM_RECEIVER_CONFIG_QICE      0x04  /* 1 QICE ON, 0 QICE OFF */
#define MCPM_RECEIVER_CONFIG_W_MIMO    0x08  /* 1 W MIMO CALL enabled */


#define MCPM_RECEIVER_CONFIG_RXD_SHIFT    0
#define MCPM_RECEIVER_CONFIG_EQ_SHIFT     1
#define MCPM_RECEIVER_CONFIG_QICE_SHIFT   2
#define MCPM_RECEIVER_CONFIG_W_MIMO_SHIFT 3

#define MCPM_MODEM_WCDMA_PERF_NONE      0x0
#define MCPM_MODEM_WCDMA_HIGH_PERF_MODE 0x1 /* OFFLINE clock to 144 for Nikel */


/*=========================================================================
      Typedefs
==========================================================================*/

/*
 * mcpm_request_type
 *
 * Requests to MCPM
 *
 * START_REQ should be sent when a technology is activated.
 *
 * STOP_REQ should be sent when a technology is deactivated.
 *
 * WAKE_UP_REQ should be sent when coming out sleep to decode a page indicator
 *
 * GO_TO_SLEEP_REQ should be sent when going into sleep.
 *
 * GO_TO_PSEUDO_SLEEP should be sent by 1X tech if RTC needs to be kept ON during sleep
 *
 * IDLE_REQ should be sent when the UE moves from page indicator decode
 * to setup paging channel. 1X technology also calls this to move from
 * 1X VOICE and 1X DATA calls.
 *
 * VOICE_START_REQ should be sent on start of a voice call.
 *
 * VOICE_STOP_REQ should be sent on end of a voice call.
 *
 * DATA_START_REQ should be sent on start of a data call
 *
 * DATA_STOP_REQ should be sent on end of a data call
 *
 */

typedef enum
{



  /* A2 Requests */
  MCPM_A2_START_REQ                                   = 0,
  MCPM_A2_STOP_REQ                                    = 1,

  /* LTE Requests */
  MCPM_LTE_START_REQ                                  = 2,
  MCPM_LTE_STOP_REQ                                   = 3,
  MCPM_LTE_INIT_REQ                                   = 4,
  MCPM_LTE_ACQ_REQ                                    = 5,
  MCPM_LTE_WAKE_UP_REQ                                = 6,
  MCPM_LTE_GO_TO_SLEEP_REQ                            = 7,
  MCPM_LTE_IDLE_REQ                                   = 8,
  

  MCPM_LTE_DATA_START_REQ                             = 9,
  MCPM_LTE_TDD_DATA_START_REQ                         = MCPM_LTE_DATA_START_REQ,
  MCPM_LTE_FDD_DATA_START_REQ                         = MCPM_LTE_DATA_START_REQ,
  MCPM_LTE_TDD_VOLTE_DATA_START_REQ                   = 10,
  MCPM_LTE_FDD_VOLTE_DATA_START_REQ                   = MCPM_LTE_TDD_VOLTE_DATA_START_REQ,
  /* To maintain backwards compatibility map data start to fdd data start - going forward LTE Data
   * has either FDD or TDD only state so the Data_Start is redundanct
   */
  MCPM_LTE_GO_TO_LIGHT_SLEEP_REQ                      = 11,
  MCPM_LTE_GO_TO_LIGHT_SLEEP_NO_MODEM_FREEZE_REQ      = MCPM_LTE_GO_TO_LIGHT_SLEEP_REQ,
  MCPM_LTE_GO_TO_LONG_LIGHT_SLEEP_REQ                 = 12,
  MCPM_LTE_DATA_STOP_REQ                              = 13,
  MCPM_LTE_PARMS_UPDATE_REQ                           = 14,
  
#ifndef FEATURE_BUILD_PAGEONLY_IMAGE   
  /* GERAN Requests */
  MCPM_GERAN_START_REQ           = 15,
  MCPM_GERAN_STOP_REQ            = 16,
  MCPM_GERAN_WAKE_UP_REQ         = 17,
  MCPM_GERAN_GO_TO_SLEEP_REQ     = 18,
  MCPM_GERAN_IDLE_REQ            = 19,
  MCPM_GERAN_VOICE_START_REQ     = 20,
  MCPM_GERAN_VOICE_STOP_REQ      = 21,
  MCPM_GERAN_DATA_START_REQ      = 22,
  MCPM_GERAN_DATA_STOP_REQ       = 23,
  MCPM_GERAN_PARMS_UPDATE_REQ    = 24,




  /* GPS Requests */
  MCPM_GPS_STOP_REQ                                   = 25,
  MCPM_GPS_ACQ_REQ                                    = 26,
  MCPM_GPS_NON_DPO_REQ                                = 27,
  MCPM_GPS_DPO_ON_REQ                                 = 28,
  MCPM_GPS_PARMS_UPDATE_REQ                           = 29,


  /* GSM CIPHERING Requests */
  MCPM_GSM_CIPHERING_START_REQ                        = 30,
  MCPM_GSM_CIPHERING_STOP_REQ                         = 31,
  
      /* THERM Requests */
  MCPM_THERM_START_REQ                                   = 32,
  MCPM_THERM_STOP_REQ                                    = 33,
#endif

      /* RF Requests */
  MCPM_RF_START_REQ                                   = 34,
  MCPM_RF_STOP_REQ                                    = 35, 
  
  MCPM_TECH_MAX_REQ
} mcpm_request_type;






/*----------------------------------------------------------------------------
  L1 techs register sleep timeline callback function prototype
  Default 1X and DO will extend timeline
  TRUE in param implies Optimized timeline
  FALSE in param implies Extended timeline
----------------------------------------------------------------------------*/

typedef void (*mcpm_npa_slp_tmln_callback_type) (boolean);




/*----------------------------------------------------------------------------
  A2 notify callback function prototype
----------------------------------------------------------------------------*/

typedef void (*mcpm_a2_notify_callback_type) (void);


/*
 * mcpm_nbr_meas_type
 *
 * The neighbor measurement type
 */

typedef enum
{


  /* LTE Neighbors */
  MCPM_L2G_MEAS_START,
  MCPM_L2G_MEAS_STOP,
  MCPM_L2W_MEAS_START,
  MCPM_L2W_MEAS_STOP,
  MCPM_L21XDO_MEAS_START,
  MCPM_L21XDO_MEAS_STOP,
  MCPM_L2TDSCDMA_MEAS_START,
  MCPM_L2TDSCDMA_MEAS_STOP,

} mcpm_nbr_meas_type;
/*
 * mcpm_rf_bandwidth_type
 *
 * The configured RF bandwidth
 */

typedef enum
{

  /* LTE */
  MCPM_LTE_180KHZ,
  MCPM_LTE_1p4MHZ,
  MCPM_LTE_3MHZ,
  MCPM_LTE_5MHZ,
  MCPM_LTE_10MHZ,
  MCPM_LTE_15MHZ,
  MCPM_LTE_20MHZ,
  MCPM_LTE_5MHZ_5MHZ,
  MCPM_LTE_10MHZ_10MHZ,
  MCPM_LTE_20MHZ_20MHZ,
  MCPM_BW_NA,
  MCPM_NUM_BW

} mcpm_rf_bandwidth_type;


/*
 * MCPM_dl_datarate_type
 *
 * Downlink data rate.
 */

typedef enum
{



  /* GERAN and GERAN1*/
  MCPM_GSM_DL_EV,
  MCPM_GSM_DL_NONEV,

  /* GPS */

  /* LTE */
  MCPM_LTE_DL_RATE_L1,
  MCPM_LTE_DL_RATE_L10,
  MCPM_LTE_DL_RATE_L36,
  MCPM_LTE_DL_RATE_L50,
  MCPM_LTE_DL_RATE_L73,
  MCPM_LTE_DL_RATE_L100,
  MCPM_LTE_DL_RATE_L110,
  MCPM_LTE_DL_RATE_L146,
  MCPM_LTE_DL_RATE_L150,
  MCPM_LTE_DL_RATE_L300,

  MCPM_DL_RATE_NA,
  MCPM_NUM_DL_RATES

} MCPM_dl_datarate_type;


/*
 * mcpm_ul_datarate_type
 *
 * uplink datarate
 */

typedef enum
{

  /* GERAN and GERAN1*/
  MCPM_GSM_UL_EV,

  /* GPS */

  /* LTE */
  MCPM_LTE_UL_RATE_L1,
  MCPM_LTE_UL_RATE_L5,
  MCPM_LTE_UL_RATE_L11,
  MCPM_LTE_UL_RATE_L13,
  MCPM_LTE_UL_RATE_L23,
  MCPM_LTE_UL_RATE_L25,
  MCPM_LTE_UL_RATE_L40,
  MCPM_LTE_UL_RATE_L50,
  MCPM_LTE_UL_RATE_L100,
  MCPM_LTE_UL_RATE_L150,

  /* TDSCDMA */

  MCPM_UL_RATE_NA,
  MCPM_NUM_UL_RATES

} mcpm_ul_datarate_type;

/* 
 * MCPM Sleep Clock Source Type.
 */
typedef enum
{
  MCPM_SLEEP_SOURCE_CALRC, /* =  PM_NPA_GENERIC_DISABLE */
  MCPM_SLEEP_SOURCE_XO,    /* =  PM_NPA_GENERIC_ENABLE */
  MCPM_SLEEP_SOURCE_MAX,   /* = PM_NPA_GENERIC_INVALID */
} mcpm_sleep_clk_src_type;


/* 
 * TCM retention state Type.
 */
typedef enum
{
  MCPM_TCM_STATE_PC, 
  MCPM_TCM_STATE_RETAIN, 
  MCPM_TCM_STATE_MAX,
} mcpm_tcm_state_type;


/*
 * mcvs_request_type
 *
 * mcvs request type
 */

typedef enum
{
  MCVS_REQUEST_NONE = 0,
  MCVS_PRE_SCALE_REQUEST,  /* to prescale Cx/Mx based on clock requests to come in future */
  MCVS_FULL_REQUEST,       /* to update clocks - clk bus/VPE/Q6 - as requested possibly even causing RPM interaction */
  MCVS_FAST_CAP_REQUEST,   /* to update clocks as far as possible but return quickly - No RPM interaction*/
  MCVS_RELEASE_REQUEST     /* To release any request made in past */
} mcvs_request_type;


/*
 * mcpm_mcvsrequest_parms_type
 *
 *  Structure containing the MCPM MCVS Request and Request Attributes.
 *  mcvs_req_type                 - Indicates if request is pre-scale (0) - slow (1) or fast(2), other fileds reserved for future
 *  mcvs_update_info              - Mask that indicates which of the attrbutes are updated.
 *
 *  modem_vpe_KHz                 - Provides the MP update Info
 *  modem_clk_bus_KHz             - Indicates the Clk bus update
 *  q6_clock_KHz                  - Indicates Q6 clock update
 */

typedef struct
{
  mcvs_request_type      mcvs_req_type;
  uint32                 mcvs_update_info;
  uint32                 modem_vpe_KHz;
  uint32                 modem_clk_bus_KHz;
  uint32                 q6_clock_KHz;
  uint32                 tdec_clk_KHz;
} mcpm_mcvsrequest_parms_type;


/*
 * mcpm_request_parms_type
 *
 * Structure containing the MCPM Request and Request Attributes.
 *
 *  update_info        - Indicates which of the attrbutes are updated.
 *  receiver_config    - Provides the receiver config information.
 *  neighbor_meas      - Indiactes the type of neighbor measurement
 *  dl_datarate        - Provides the downlink data rate.
 *  ul_datarate        - Provides the uplink data rate.
 *  rf_bandwidth       - Provides the RF bandwidth.
 *  wkup_time_for_rf   - Time when RF resource needed (in usec)
 *  wkup_time          - Time when bus or any other resource request needed.(in usec)
 *  modem_perf_mode    - Modem performance mode
 *  mcvs_request - L1s can send an MCVS request along with other parameter updates
 *                 MCVS request could be to pre-scale voltage rails based on clock frequency, or make immediate clk bus/Q6/VPE updates
 */
typedef struct
{

  uint32                        update_info;
  uint32                        receiver_config;
  mcpm_nbr_meas_type            neighbor_meas;
  MCPM_dl_datarate_type         dl_datarate;
  mcpm_ul_datarate_type         ul_datarate;
  mcpm_rf_bandwidth_type        rf_bandwidth;
  mcpm_sleep_clk_src_type       mcpm_sleep_clk_src;
  mcpm_tcm_state_type           mcpm_tcm_state;
  uint32                        modem_perf_mode;
  uint64                        wkup_time_for_rf;
  uint64                        wkup_time;
  mcpm_mcvsrequest_parms_type   mcvs_request;
} mcpm_request_parms_type;


/*
 * mcpm_tech_type
 *
 * MCPM supported technologies.
 */

typedef enum
{

  /* MCPM GERAN technolgy definition */
  MCPM_GERAN_TECH,

  /* MCPM LTE technolgy definition */
  MCPM_LTE_TECH,

  /* Number of RAT technologies */
  MCPM_NUM_RAT_TECH,

  /* MCPM GPS technology definition */
  MCPM_GPS_TECH = MCPM_NUM_RAT_TECH,

  /* MCPM RF technology definition */
  MCPM_RF_TECH,

  /* MCPM GSM CIPHERING definition */
  MCPM_GSM_CIPHERING_TECH,

  /* MCPM A2 technolgy definition */
  MCPM_A2_TECH,

    /* MCPM THERM technolgy definition */
  MCPM_THERM_TECH,

  /* number fo techs used for boundary checks  */
  MCPM_NUM_TECH
} mcpm_tech_type;


/*
 * MCPM_RCM_TRIGGER
 *
 * Supported RCM Triggers
 */

typedef enum
{
  /* Primary RCM Debug Toggle */
  RCM_TRIGGER_1,
  /* Max number of RCM triggers */
  RCM_TRIGGER_MAX
} MCPM_RCM_TRIGGER;


/*----------------------------------------------------------------------------
  A2 resources need be setup as fast as possible. Pulling out of A2 from the
  normal path of muxing.
----------------------------------------------------------------------------*/
#define MCPMDRV_NUM_MUX_TECH  (MCPM_A2_TECH)


/* Return values to check for by RF/other SW modules using MCPM_NPA */
typedef enum
{
  MCPMNPA_STAT_NULL,
  MCPMNPA_STAT_OPT_SUCCESS = 1,
  MCPMNPA_STAT_OPT_FAILURE = 2,
  MCPMNPA_STAT_OPT_IMM_REQ_SENT = 3,
  MCPMNPA_STAT_OPT_ENABLE = 4
} MCPMNPA_StatusType;


/*
 * MCPM_ClkSpeedType
 *
 * MCPM clock speed type which controls the Q6 Clk speed for Jolokia.
 *
 */

typedef enum
{
  MCPM_CLK_NULL,
  MCPM_CLK_19_2M,
  MCPM_CLK_Q6_326M,
  MCPM_CLK_Q6_480M,
  MCPM_CLK_Q6_576M,
  MCPM_NUM_CLKSPEED
} MCPM_ClkSpeedType;



#define MCPM_BIMC_BOOST_MASK              0x001
#define MCPM_CPU_BOOST_MASK               0x002

typedef void (*mcpm_deboost_callback)(void);
typedef void (*mcpm_cpuboost_callback)(void);

/*==========================================================================
               FUNCTION DECLARATIONS FOR MODULE
==========================================================================*/


/*==========================================================================

  FUNCTION      MCPM_CONFIG_MODEM

  DESCRIPTION   This function is the main interface function.
                Modem SW should call this function when the modem mode is
                changed. MCPM expects the call made for the following mode
                changes.
                - Activate and deactivate the Portocal stack
                - Start and stop of voice call
                - Start and stop of data call
                - Change in RF bandwidth
                - Start and Stop of nbr measurement
                - Rxd/qice/EQ start and stop.

  PARAMETERS    req      - Modem SW mode change request.
                parms    - Attributes associated with req.

  DEPENDENCIES  MCPM must be initialized.

  RETURN VALUE  None.

  SIDE EFFECTS  Clocks will be enabled and disabled during the test.

==========================================================================*/

void MCPM_Config_Modem(mcpm_request_type req, mcpm_request_parms_type *parms);

/*==========================================================================

  FUNCTION      MCPM_Config_Modem_Try

  DESCRIPTION   This function will first try to get MCPM top lock.  If the
                lock is already acquired then this function will return FALSE
                immediately without processing the request, otherwise it will
                proceed to get the lock, process the request by calling
                MCPM_CONFIG_MODEM and return TRUE when finished.

  PARAMETERS    req      - Modem SW mode change request.
                parms    - Attributes associated with this request.

  DEPENDENCIES  MCPM must be initialized.

  RETURN VALUE  TRUE     - Successfully acquired the lock and finished the request.
                FALSE    - MCPM top lock has been acquired at the time of requst, the
                           request is NOT processed.

  SIDE EFFECTS  If returned TRUE, system configuration is setup as expected.
                If returned FALSE, system configuration is not setup as expected.

==========================================================================*/

boolean MCPM_Config_Modem_Try(mcpm_request_type req, mcpm_request_parms_type *parms);


#ifdef FEATURE_BUILD_PAGEONLY_IMAGE   


/*==========================================================================

  FUNCTION      MCPM_Send_RF_Req

  DESCRIPTION   This function can be called by RF drivers to send the RF requests
                to MCPM.

  PARAMETERS    mode                - Tech/RF mode that is making this request.
                npa_id              - PAM Id that MCPM will use in NPA request
                npa_handle          - NPA handle that MCPM should use to
                pass this request to PMIC
                time_rsrc_needed    - Time when this resource is needed (not used for now)
                sleep_req           - Bool to indicate if this is a sleep or awake request

  DEPENDENCIES  MCPM must be initialized.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/
MCPMNPA_StatusType MCPM_Send_RF_Req
(

  /* NPA Id sent by RF driver */
  uint8 state
);

/*
 * Macro for toggling a specific RCM debug strobe. Stuubed out for POI 
 */
#define MCPM_GPIO_TOGGLE(RCM_STROBE) 

#else /* FEATURE_BUILD_PAGEONLY_IMAGE */ 
/*==========================================================================

  FUNCTION      MCPM_Send_RF_Req

  DESCRIPTION   This function can be called by RF drivers to send the RF requests
                to MCPM.

  PARAMETERS    mode                - Tech/RF mode that is making this request.
                npa_id              - PAM Id that MCPM will use in NPA request
                npa_handle          - NPA handle that MCPM should use to
                pass this request to PMIC
                time_rsrc_needed    - Time when this resource is needed (not used for now)
                sleep_req           - Bool to indicate if this is a sleep or awake request

  DEPENDENCIES  MCPM must be initialized.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/
MCPMNPA_StatusType MCPM_Send_RF_Req
(
  /* Tech that is making this request. */
  rfm_mode_enum_type mode,
  /* NPA Id sent by RF driver */
  uint8 npa_id,
  /* NPA handle that is sent by RF driver */
  npa_client_handle npa_handle,
  /* Time when this resource is needed. */
  uint64 time_rsrc_needed,
  /* Sleep or Awake request. */
  boolean sleep_req
);

/* =========================================================================
  Function : MCPM_Resrc_RF_NPA_Send_Req

  Commits the RF state passed to this resource. Only called for scheduling or
  for requests to turn off RF rails, as immediate requests are called through
  the external API.

  @param eTech         [in] Current technology.
  @param tWakeupTime   [in] Wake-up time.
  @param bSleepReq     [in] Indicates resources should be turned off if TRUE.
                            If false, will send an imm request for the last
                            scheduled value (intended to sync the scheduled
                            request on rude wakeups).

  @return
  None.

  @dependencies
  TBD.

==========================================================================*/

void MCPM_Resrc_RF_NPA_Send_Req
(
  mcpm_tech_type  eTech,
  uint64          tWakeupTime,
  boolean         bSleepReq
);

/* =========================================================================
 **  Function : MCPM_Service_Boost_Req
 ** =========================================================================*/
/**
  This function is the main API called by L1 techs to request for boost/deboost..


  @param client         [in] Current technology mcpm_tech.
  @param boostRequest         [in] uses 2 LSB bits as flags.
                                  MCPM_BIMC_BOOST_MASK(0x1) and MCPM_CPU_BOOST_MASK (0x2)
  @param callbackFunctionPtr  [in] function pointer to call, when deboost is complete.

  @return
  None.

  @dependencies
  TBD.

 */
void  MCPM_Service_Boost_Req
(
   mcpm_tech_type client,
   uint32  boostRequest,
   mcpm_deboost_callback callbackFunctionPtr
);

uint32  MCPM_Service_Boost_Request
(
   mcpm_tech_type client,
   uint32  boostRequest,
   mcpm_deboost_callback callbackFunctionPtr,
   mcpm_cpuboost_callback CpuBoostCbfcnPtr,
   uint32 l1boostTime
);

/*==========================================================================

  FUNCTION      MCPM_MCVSConfig_Modem

  DESCRIPTION   This function is the interface function for MCVS requests.
                Modem SW should call this function when the modem mode mcvs update is required.
                MCPM expects the call made for the following mode
                changes.
                - VPE Clock update
                - CLK BUS Clock update
                - Q6 Clock update

  PARAMETERS    tech      - Modem SW tech type.
                parms    - Attributes associated with req.

  DEPENDENCIES  MCPM must be initialized.

  RETURN VALUE  None.

  SIDE EFFECTS  Clocks will be Updated during this without change in Config. If the request attribute type in parms is
                set to type immediate - then no RPM requests will be sent out to scale the voltages. If voltages have
                not been pre-scaled, then frequencies will be capped.If request type is type slow - this call will
                block if (mx,cx) voltage scaling is required and may take longer to finish. This call needs to be
                accompanied by a call to Release the MCVS Request

==========================================================================*/

void MCPM_MCVSConfig_Modem(mcpm_tech_type tech, mcpm_mcvsrequest_parms_type *parms);


/*==========================================================================

  FUNCTION      MCPM_MCVSConfig_Release
  DESCRIPTION   This function is the interface function for releasing the MCVS requests .
                Modem SW should call this function when the modem mode mcvs update request has been made.
                MCPM expects the call made for the following mode
                changes.
                - VPE Clock update
                - CLK BUS Clock update
                - Q6 Clock update

  PARAMETERS    tech      - Modem SW tech type.


  DEPENDENCIES  MCPM must be initialized. A prior call to MCPM_MCVSConfig_Modem from same tech type should have been made.


  RETURN VALUE  NONE.

  SIDE EFFECTS : This will bring down the clock configs to pre-mcvs level.

==========================================================================*/

void MCPM_MCVSConfig_Release(mcpm_tech_type tech);

/*============================================================================

FUNCTION MCPM_NPA_SET_SLEEP_TIMELINE_CALLBACK

DESCRIPTION
  This function sets the callback function to call when sleep timeline has to
  changed from optimized to extended and vice versa

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

============================================================================*/

void MCPM_NPA_Set_Sleep_Timeline_Callback
(
  /* The tech that is registering the block */
  mcpm_tech_type tech,

  /* register sleep timeline callback */
  mcpm_npa_slp_tmln_callback_type slp_tmln_cb
);





/*==========================================================================

  FUNCTION      mcpm_gpio_profile_pwr_strobe_toggle

  DESCRIPTION   Used to toggle power strobe at profile points. Should not be used
                directly. Instead should be used through macro MCPM_GPIO_TOGGLE.

  PARAMETERS    strobe      - The RCM power strobe to toggle

  DEPENDENCIES  MCPM must be initialized, MCPM GPIO feature must be enabled

  RETURN VALUE  None.

  SIDE EFFECTS  Toggles a specific dedicated debug strobe on RCM HW

==========================================================================*/

extern void MCPM_Gpio_Profile_Pwr_Strobe_Toggle(MCPM_RCM_TRIGGER strobe);

/*
 * Macro for toggling a specific RCM debug strobe
 */

#define MCPM_GPIO_TOGGLE(RCM_STROBE) MCPM_Gpio_Profile_Pwr_Strobe_Toggle(RCM_STROBE);

#endif /* FEATURE_BUILD_PAGEONLY_IMAGE */ 
/*============================================================================

FUNCTION MCPM_Set_A2_Tech_Callback

DESCRIPTION
  This function sets the callback function to notify A2 when only A2 is active

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

============================================================================*/

void MCPM_Set_A2_Tech_Callback
(
  /* register clock change callback */
  mcpm_a2_notify_callback_type a2_notify_cb
);

uint32  MCPM_CLK_RFFE_Resrc_Update
(
  uint32          State
);

#endif /* __MCPM_API_H__ */

