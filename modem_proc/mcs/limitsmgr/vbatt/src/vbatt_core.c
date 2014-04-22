/*!
  @file
  vbatt_core.c

  @brief
  This file implements VBATT-based limits
*/

/*=============================================================================

  Copyright (c) 2019 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by this document
  are confidential and proprietary information of Qualcomm Technologies
  Incorporated and all rights therein are expressly reserved.  By accepting
  this material the recipient agrees that this material and the information
  contained therein are held in confidence and in trust and will not be used,
  copied, reproduced in whole or in part, nor its contents revealed in any
  manner to others without the express written permission of Qualcomm
  Technologies Incorporated.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/mcs.mpss/5.1/limitsmgr/vbatt/src/vbatt_core.c#5 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/31/16   btl     Use BCL comparators, remove TA featurization
02/26/16   btl     Added support for NPA flow control based on stage
08/13/15   tl      Release ADC handle when shutting down
06/01/15   rj      Added TA feature flag
05/29/15   tl      Initial revision

=============================================================================*/

/*=============================================================================
                                INCLUDE FILES
=============================================================================*/

#include "lmtsmgr.h"
#include "vbatt_i.h"
#include "vbatt_efs.h"
#include "lmtsmgr_diag.h"
#include "lmtsmgr_task.h"
#include "lmtsmgr_translation.h"
#include "mcsprofile.h"

#include "diagcmd.h"
#include "diagpkt.h"
#include "pmapp_bcl.h"

/*=============================================================================
                              MACRO DEFINITIONS
=============================================================================*/

#ifdef TEST_FRAMEWORK
#error code not present
#endif // TEST_FRAMEWORK

/* need a valid value, so pick a safe one. Doesn't matter what, because
 * comparator is not enabled. */
#define VBATT_BCL_COMP_CLEAR          3600

/* Minimum battery volate in mV for device to be oprational */
#define VBATT_MIN_OPERATIONAL_VOLTAGE 2500

/* USB correction parameters */
#define VBATT_USB_CORR_SLOPE          50
#define VBATT_USB_CORR_OFFSET         265

/*=============================================================================
                         INTERNAL VARIABLES
=============================================================================*/
static const char* vbatt_npa_resource = "/therm/mitigate/vbatt_low";

/*=============================================================================
                         INTERNAL FUNCTION PROTOTYPES
=============================================================================*/


/*=============================================================================

  FUNCTION:  vbatt_adc_set_threshold

=============================================================================*/
/*!
    @brief
    Calls the PMIC API to set the specified voltage threshold.

    @param[in]  type    The threshold type to set, PMAPP_BCL_LOW1_THR or
                        PMAPP_BCL_HI1_THR
    @param[in]  limit   The threshold to set, in mV

    @return
    None
*/
/*===========================================================================*/
static void vbatt_adc_set_threshold(pmapp_bcl_thr_type type, uint32 limit);

/*=============================================================================

  FUNCTION:  vbatt_adc_clear_threshold

=============================================================================*/
/*!
    @brief
    Calls the PMIC API to clear the specified voltage threshold.

    @param[in]  type    The threshold type to clear, PMAPP_BCL_LOW1_THR or
                        PMAPP_BCL_HI1_THR

    @return
    None
*/
/*===========================================================================*/
static void vbatt_adc_clear_threshold(pmapp_bcl_thr_type type);

/*=============================================================================

  FUNCTION:  vbatt_adc_threshold_callback

=============================================================================*/
/*!
    @brief
    Callback called by PMAPP BCL when a BCL comparator threshold is crossed.

    Updates the vbatt value and signals the Limits Manager task to check the
    current vbatt level against the current limits.

    @param[in]  thresh_triggered   Callback is shared by HIGH and LOW BCL
                                   comparators. Explains which triggerd the
                                   callback.
    @param[in]  p2                 Not used
    @param[in]  p3                 Not used
    @param[in]  p4                 Not used

    @return
    None
*/
/*===========================================================================*/
static void vbatt_adc_threshold_callback
(
  pmapp_bcl_thr_type thresh_triggered,
  void * p2,
  void * p3,
  void * p4
);

/*=============================================================================

  FUNCTION:  vbatt_adc_read_callback

=============================================================================*/
/*!
    @brief
    Callback called by ADC when DalAdc_RequestConversion() completes.

    Updates the vbatt value and signals the Limits Manager task to check the
    current vbatt level against the current limits.

    @param[in]  object          Not used
    @param[in]  param           Not used
    @param[in]  vpayload        Pointer to the payload object, which is
                                expected to be a structure of type
                                AdcResultType
    @param[in]  payload_size    Size of the payload object, which is expected
                                to be sizeof(AdcResultType)

    @return
    NULL
*/
/*===========================================================================*/
static void * vbatt_adc_read_callback
(
  void * object,
  uint32 param,
  void * vpayload,
  uint32 payload_size
);

/*=============================================================================

  FUNCTION:  vbatt_set_test_override

=============================================================================*/
/*!
    @brief
    Diag command handler to set the vbatt override value, to test the vbatt
    core algorithm in cases where the actual vbatt level can't be easily
    manipulated

    @param[in]  req_pkt The input packet, representing the command request
                        from QXDM, which is expected to be of type
                        DIAG_SUBSYS_LIMITSMGR_VBATT_SET_req_type
    @param[in]  pkt_len The length of the input packet, which is expected to
                        be sizeof(DIAG_SUBSYS_LIMITSMGR_VBATT_SET_req_type)

    @return
    Diag command response packet
*/
/*===========================================================================*/
static PACK(void *) vbatt_set_test_override
(
  PACK(void*) req_pkt,
  uint16 pkt_len
);

/*=============================================================================

  FUNCTION:  vbatt_set_test_active_tx

=============================================================================*/
/*!
    @brief
    Diag command handler to set the the active tx for the desired tech,
    to test the vbatt core algorithm in cases where the tx can't be easily
    manipulated

    @param[in]  req_pkt The input packet, representing the command request
                        from QXDM, which is expected to be of type
                        DIAG_SUBSYS_LIMITSMGR_VBATT_SET_ACTIVE_TX_req_type
    @param[in]  pkt_len The length of the input packet, which is expected to
                        be sizeof(DIAG_SUBSYS_LIMITSMGR_VBATT_SET_ACTIVE_TX_req_type)

    @return
    Diag command response packet
*/
/*===========================================================================*/
static PACK(void *) vbatt_set_test_active_tx
(
  PACK(void*) req_pkt,
  uint16 pkt_len
);

/*=============================================================================

  FUNCTION:  vbatt_npa_resource_avail_cb

=============================================================================*/
/*!
    @brief
    Callback indicating NPA resource for flow control is available.
*/
/*===========================================================================*/
static void vbatt_npa_resource_avail_cb
(
  void         *context,
  unsigned int  event_type,
  void         *data,
  unsigned int  data_size
);

/*=============================================================================

  FUNCTION:  vbatt_issue_flow_control_req

=============================================================================*/
/*!
    @brief
    Initiate/cancel flow control request via NPA, based on the current VBATT
    stage.

    @param[in]  vbatt_stage: The vbatt stage to base the request off of
*/
/*===========================================================================*/
static void vbatt_issue_flow_control_req
(
  int32 vbatt_stage
);

/*=============================================================================
                              INTERNAL VARIABLES
=============================================================================*/

vbatt_type vbatt;

/* Table containing diag commands and their callback function */
static const diagpkt_user_table_entry_type vbatt_diag_tbl[] =
{
  /* start id, end id, callback function */
  { (word)VBATT_SET, (word)VBATT_SET, vbatt_set_test_override },
  { (word)VBATT_SET_ACTIVE_TX, (word)VBATT_SET_ACTIVE_TX, vbatt_set_test_active_tx },
};

/*=============================================================================
                               PUBLIC FUNCTIONS
=============================================================================*/

/*=============================================================================

  FUNCTION:  vbatt_init

=============================================================================*/
/*!
    @brief
    Initializes vbatt limits and other resources

    @return
    None
*/
/*===========================================================================*/
void vbatt_init(void)
{
  DALResult result;

  memset(&vbatt, 0, sizeof(vbatt));

  rex_init_crit_sect(&vbatt.vbatt_crit_sect);

  vbatt.active_band = SYS_BAND_CLASS_NONE;
  vbatt.active_tech = CXM_TECH_DFLT_INVLD;
  vbatt.active_sub_tech = CXM_LTE_SUB_INVALID;
  vbatt.record = NULL;
  vbatt.current_stage = VBATT_STAGE_0;
  vbatt.current_vbatt = INVALID_VBATT_VALUE;
  vbatt.vbatt_test_override = INVALID_VBATT_VALUE;
  vbatt.hysteresis_timer_expires = 0;
  vbatt.hysteresis_stage = VBATT_STAGE_INVALID;
  vbatt.vbatt_threshold_low = -1;
  vbatt.vbatt_threshold_high = -1;
  vbatt.usb_is_conn = FALSE;

  /* Create a timer used to honor the time hysteresis. When the timer expires
   * it will deliver a signal to the limits manager task, which will call
   * lmtsmgr_process_vbatt_event(). */
  timer_def(&vbatt.hysteresis_timer, NULL, rex_self(), LMTSMGR_SAR_VBATT_SIG,
      NULL, 0);

  /* Connect to ADC */
  DALSYS_InitMod(NULL);

  result = DAL_AdcDeviceAttach(DALDEVICEID_ADC, &vbatt.adc_handle);
  if(result != DAL_SUCCESS)
  {
    VBATT_MSG_1(ERROR,
        "ADC device attach failed with result %d",
        result);
    return;
  }

  /* Get input properties for the VBATT channel */
  result = DalAdc_GetAdcInputProperties(vbatt.adc_handle, ADC_INPUT_VPH_PWR,
      strlen(ADC_INPUT_VPH_PWR), &vbatt.adc_properties);
  if(result != DAL_SUCCESS)
  {
    VBATT_MSG_1(ERROR,
        "ADC get input properties for VBATT failed with result %d",
        result);
  }

  result = DALSYS_EventCreate(
              DALSYS_EVENT_ATTR_CLIENT_DEFAULT |
              DALSYS_EVENT_ATTR_CALLBACK_EVENT,
              &vbatt.adc_read_event,
              NULL);
  if(result != DAL_SUCCESS)
  {
    VBATT_MSG_1(ERROR,
        "DAL create read event failed with result %d",
        result);
  }

  result = DALSYS_SetupCallbackEvent(vbatt.adc_read_event,
      vbatt_adc_read_callback, NULL);
  if(result != DAL_SUCCESS)
  {
    VBATT_MSG_1(ERROR,
        "DAL create read event failed with result %d",
        result);
  }

  /* register the vbatt diag command table to the diag command service */
  DIAGPKT_DISPATCH_TABLE_REGISTER(DIAG_SUBSYS_LIMITSMGR, vbatt_diag_tbl);

  /* create NPA sync client for communicating flow control requests to CFCM --
   * wait until resource available */
  npa_resource_available_cb(vbatt_npa_resource, vbatt_npa_resource_avail_cb, NULL);
}

/*=============================================================================

  FUNCTION:  vbatt_deinit

=============================================================================*/
/*!
    @brief
    Frees all memory and other resources allocated by vbatt_init().

    @return
    None
*/
/*===========================================================================*/
void vbatt_deinit(void)
{
  DALResult result;

  timer_clr(&vbatt.hysteresis_timer, T_NONE);
  timer_undef(&vbatt.hysteresis_timer);

  if(vbatt.adc_handle)
  {
    result = DAL_DeviceDetach(vbatt.adc_handle);

    if(result != DAL_SUCCESS)
    {
      VBATT_MSG_1(ERROR,
          "ADC DAL_DeviceDetach() failed: %d",
          result);
    }
    vbatt.adc_handle = NULL;
  }

  npa_destroy_client(vbatt.flow_control_handle);

}

/*=============================================================================

  FUNCTION:  lmtsmgr_vbatt_set_usb_conn_state

=============================================================================*/
/*!
    @brief
    Updates the usb connection state of the modem.

    @param[in]  usb_is_conn   Flag to indicate usb connection state

    @return
    None
*/
/*===========================================================================*/
void lmtsmgr_vbatt_set_usb_conn_state(boolean usb_is_conn)
{
  /* Set the usb state */
  vbatt.usb_is_conn = usb_is_conn;

  /* Set the signal to correct the ADC thresholds */
  lmtsmgr_set_sigs(LMTSMGR_SAR_VBATT_USB_SIG);
}

/*=============================================================================

  FUNCTION:  lmtsmgr_vbatt_get_usb_conn_state

=============================================================================*/
/*!
    @brief
    Return the usb connection state.

    @param[in]  None

    @return
    TRUE -- USB is connected
    FALSE -- USB is disconnected
*/
/*===========================================================================*/
boolean lmtsmgr_vbatt_get_usb_conn_state(void)
{
  return (vbatt.usb_is_conn);
}

/*=============================================================================

  FUNCTION:  lmtsmgr_process_vbatt_freq_input

=============================================================================*/
/*!
    @brief
    Updates the current active tech used for vbatt limiting. Calls
    lmtsmgr_process_vbatt_event() to run the complete vbatt limiting
    algorithm.

    @param[in]  new_tech        The tech that provided the frequency update

    @return
    None
*/
/*===========================================================================*/
void lmtsmgr_process_vbatt_freq_input(cxm_tech_type new_tech,
                                      cxm_lte_sub_tech_type sub_tech)
{
  int i;
  sys_band_class_e_type new_band = SYS_BAND_CLASS_NONE;
  const lmtsmgr_tech_list_type *currList;

  LMTSMGR_ASSERT((new_tech > CXM_TECH_DFLT_INVLD) && (new_tech < CXM_TECH_MAX));

  if (!(((new_tech == CXM_TECH_LTE) && (sub_tech & (CXM_LTE_SUB_CATM1 | CXM_LTE_SUB_NB1))) ||
       ((new_tech != CXM_TECH_LTE) && (sub_tech == CXM_LTE_SUB_INVALID))))
  {
    VBATT_MSG_2(ERROR,
        "vbatt_freq_input received invalid tech %d and sub_tech %d combination",
        new_tech, sub_tech);
    return;
  }

  MCSMARKER(VBATT_UPDATE_TECHS_I);

  /* Look through the uplink frequency information to determine what the
   * active transmit band is. If the band has changed, update everything. */
  currList = &lmtsmgr.tech_state[new_tech].currList;
  for(i = 0; i < currList->num_ul_entries; i++)
  {
    int j = currList->ul_freqList[i];

    if(new_band == SYS_BAND_CLASS_NONE)
    {
      new_band = currList->links[j].freqInfo.link_info.band;
    }
    else if(new_band != currList->links[j].freqInfo.link_info.band)
    {
      VBATT_MSG_2(HIGH,
          "Multiple bands found for tech %d; using only band %d",
          new_tech, new_band);
    }
  }

  if(new_band == SYS_BAND_CLASS_NONE && new_tech != vbatt.active_tech)
  {
    /* This tech is rx-only, and another tech has active rx and tx. This looks
     * like a DR-DSDS scenario. Ignore this tech. */
    VBATT_MSG_2(MED,
        "Skipping rx-only tech %d in favor of active tech %d",
        new_tech, vbatt.active_tech);
  }
  else if((new_band != vbatt.active_band) ||
          (sub_tech != vbatt.active_sub_tech))
  {
    vbatt.active_band = new_band;
    vbatt.active_tech = new_tech;
    vbatt.active_sub_tech = sub_tech;
    vbatt.record = vbatt_lookup(new_tech, sub_tech, new_band);
    vbatt.current_stage = VBATT_STAGE_INVALID;
    vbatt.hysteresis_timer_expires = 0;
    vbatt.hysteresis_stage = VBATT_STAGE_INVALID;

    timer_clr(&vbatt.hysteresis_timer, T_NONE);

    if(vbatt.record == NULL)
    {
      VBATT_MSG_2(MED, "Record not found for new band %d for tech %d",
          new_band, new_tech);
    }
    else
    {
      VBATT_MSG_2(MED, "Found record for new band %d for tech %d",
          new_band, new_tech);
    }

    lmtsmgr_process_vbatt_event();
  }

  MCSMARKER(VBATT_UPDATE_TECHS_O);
}

/*=============================================================================

  FUNCTION:  lmtsmgr_process_vbatt_event

=============================================================================*/
/*!
    @brief
    Runs the vbatt limiting algorithm on the current active tech. Sets ADC
    thresholds and sends power limits to tech RF.

    @return
    None
*/
/*===========================================================================*/
void lmtsmgr_process_vbatt_event(void)
{
  int i;
  vbatt_stage_type new_stage;
  /* New tx power limit to be applied */
  int16 vbatt_plimit;

  if(vbatt.adc_handle == NULL)
  {
    VBATT_MSG_0(ERROR, "ADC device not connected");
    return;
  }

  MCSMARKER(VBATT_UPDATE_I);

  rex_enter_crit_sect(&vbatt.vbatt_crit_sect);

  if(vbatt.hysteresis_timer_expires > 0)
  {
    uint32 now = timetick_get_ms();
    if(now < vbatt.hysteresis_timer_expires)
    {
      VBATT_MSG_2(MED,
          "Hysteresis timer not yet expired "
          "(expires at %u, current time is %u)",
          vbatt.hysteresis_timer_expires, now);
    }
    else
    {
      /* Hysteresis timer expired. */
      VBATT_MSG_0(MED, "Hysteresis timer expired");
      vbatt.hysteresis_timer_expires = 0;
      vbatt.hysteresis_stage = VBATT_STAGE_INVALID;
      vbatt.current_vbatt = INVALID_VBATT_VALUE;
    }
  }

  /* If the current band does not have a vbatt record, there is no work to do.
   * Clear the state and return immediately. */
  if(vbatt.record == NULL)
  {
    /* No record for this band. Set stage to -1, indicating no limit. */
    vbatt.current_stage = VBATT_STAGE_0;

    /* Clear the ADC limits */
    vbatt_adc_clear_threshold(PMAPP_BCL_LOW1_THR);
    vbatt_adc_clear_threshold(PMAPP_BCL_HI1_THR);

    /* We are no longer tracking vbatt */
    vbatt.current_vbatt = INVALID_VBATT_VALUE;

    rex_leave_crit_sect(&vbatt.vbatt_crit_sect);

    if(vbatt.active_tech != CXM_TECH_DFLT_INVLD)
    {
      lmtsmgr.tech_state[vbatt.active_tech].pwr_info.new_limits.vbatt_plimit
        = DEFAULT_MAX_PLIMIT;
      lmtsmgr_notify_tech_plimit(vbatt.active_tech);
    }

    MCSMARKER(VBATT_UPDATE_O);
    return;
  }

  if(vbatt.vbatt_test_override != INVALID_VBATT_VALUE)
  {
    /* If an override value has been specified using the Diag command from
     * QXDM, use the override value here instead of the actual physical value
     * from ADC. */
    vbatt.current_vbatt = vbatt.vbatt_test_override;
  }

  /* Check whether the vbatt value is valid. If stale, request a new vbatt
   * read. */
  if(vbatt.current_vbatt == INVALID_VBATT_VALUE)
  {
    DALResult result;
    AdcRequestParametersType adc_params;

    adc_params.hEvent = vbatt.adc_read_event;
    adc_params.nDeviceIdx = vbatt.adc_properties.nDeviceIdx;
    adc_params.nChannelIdx = vbatt.adc_properties.nChannelIdx;

    MCSMARKER(VBATT_ADC_READ_REQ);

    result = DalAdc_RequestConversion(vbatt.adc_handle, &adc_params, NULL);
    if(result == DAL_SUCCESS)
    {
      VBATT_MSG_0(LOW, "Requested ADC conversion for VBATT");
    }
    else
    {
      VBATT_MSG_1(ERROR, "ADC read request failed: %d", result);
    }

    /* Wait for the ADC read to complete. When complete, the callback will
     * signal the Limits Manager task to continue the algorithm. */

    rex_leave_crit_sect(&vbatt.vbatt_crit_sect);
    MCSMARKER(VBATT_UPDATE_O);
    return;
  }

  /*
   * Determine any updates to the current stage
   */

  if((vbatt.current_stage <= VBATT_STAGE_INVALID) ||
      (vbatt.current_stage >= VBATT_STAGES_MAX))
  {
    vbatt.current_stage = VBATT_STAGE_INVALID;
    new_stage = VBATT_STAGE_0;
  }
  else
  {
    new_stage = vbatt.current_stage;
  }

  for(i = 0; i <= vbatt.current_stage; i++)
  {
    /* Compare the current vbatt against each lower stage's voltage_up
     * threshold. If the threshold is met, transition to the prior stage. */
    if(vbatt.current_vbatt >= vbatt.record->stages[i].voltage_up)
    {
      if(i - 1 != vbatt.hysteresis_stage)
      {
        new_stage = i - 1;
        VBATT_MSG_4(HIGH,
            "Transition from stage %d to stage %d: vbatt %d > voltage_up %d",
            vbatt.current_stage,
            new_stage,
            vbatt.current_vbatt,
            vbatt.record->stages[i].voltage_up);
        break;
      }
      else
      {
        VBATT_MSG_1(MED,
            "Hysteresis timer not yet expired; cannot go back to stage %d",
            vbatt.hysteresis_stage);
      }
    }
  }

  for(i = VBATT_STAGES_MAX - 1; i > vbatt.current_stage && i >= 0; i--)
  {
    /* Compare the current vbatt against each higher stage's voltage_down
     * threshold. If the threshold is met, transition to that stage. */
    if(vbatt.current_vbatt <= vbatt.record->stages[i].voltage_down)
    {
      if(i != vbatt.hysteresis_stage)
      {
        new_stage = i;
        VBATT_MSG_4(HIGH,
            "Transition from stage %d to stage %d: "
            "vbatt %d < voltage_down %d",
            vbatt.current_stage,
            new_stage,
            vbatt.current_vbatt,
            vbatt.record->stages[i].voltage_down);
        break;
      }
      else
      {
        VBATT_MSG_1(MED,
            "Hysteresis timer not yet expired; cannot go back to stage %d",
            vbatt.hysteresis_stage);
      }
    }
  }

  if(new_stage != vbatt.current_stage)
  {
    if(vbatt.record->time_hysteresis > 0)
    {
      /* If this is a new stage, and the hysteresis timer is non-zero, set a
       * timer to wait before applying the new limits */
      vbatt.hysteresis_timer_expires =
        timetick_get_ms() + vbatt.record->time_hysteresis;

      vbatt.hysteresis_stage = vbatt.current_stage;

      VBATT_MSG_4(HIGH,
          "Band %d stage %d Setting hysteresis timer for %u expiring at %u",
          vbatt.active_band, new_stage,
          vbatt.record->time_hysteresis,
          vbatt.hysteresis_timer_expires);

      timer_set(&vbatt.hysteresis_timer, vbatt.record->time_hysteresis,
          0, T_MSEC);
    }

    vbatt.current_stage = new_stage;

    /* Send flow control request:
     *   send request to thermal (to forward to CFCM) with the new stage */
    vbatt_issue_flow_control_req(new_stage);

    /*
     * Determine power limits for the current stage
     */
    if(vbatt.current_stage == VBATT_STAGE_0)
    {
      vbatt_plimit = DEFAULT_MAX_PLIMIT;
    }
    else if((vbatt.current_stage >= VBATT_STAGE_1) &&
        (vbatt.current_stage < VBATT_STAGES_MAX))
    {
      vbatt_plimit = vbatt.record->stages[vbatt.current_stage].tx_power_limit;
    }
    else
    {
      ERR_FATAL("Invalid current stage %d", vbatt.current_stage, 0, 0);
    }

    VBATT_MSG_3(HIGH,
        "Band %d stage %d vbatt limit %d",
        vbatt.active_band, vbatt.current_stage, vbatt_plimit);

    LMTSMGR_ASSERT((vbatt.active_tech > CXM_TECH_DFLT_INVLD) &&
        (vbatt.active_tech < CXM_TECH_MAX));
    lmtsmgr.tech_state[vbatt.active_tech].pwr_info.new_limits.vbatt_plimit =
      vbatt_plimit;
  } /* new stage != current stage */

  /*
   * Set ADC limits for the limits where the hysteresis timer does not apply
   */
  if((vbatt.current_stage < VBATT_STAGES_MAX - 1) &&
      (vbatt.current_stage + 1 != vbatt.hysteresis_stage) &&
      (vbatt.vbatt_test_override == INVALID_VBATT_VALUE))
  {
    vbatt_adc_set_threshold(PMAPP_BCL_LOW1_THR,
        vbatt.record->stages[vbatt.current_stage + 1].voltage_down);
  }
  else
  {
    vbatt_adc_clear_threshold(PMAPP_BCL_LOW1_THR);
  }

  if((vbatt.current_stage >= 0) &&
      (vbatt.current_stage - 1 != vbatt.hysteresis_stage) &&
      (vbatt.vbatt_test_override == INVALID_VBATT_VALUE))
  {
    vbatt_adc_set_threshold(PMAPP_BCL_HI1_THR,
        vbatt.record->stages[vbatt.current_stage].voltage_up);
  }
  else
  {
    vbatt_adc_clear_threshold(PMAPP_BCL_HI1_THR);
  }

  rex_leave_crit_sect(&vbatt.vbatt_crit_sect);

  /* Notify the active tech if the vbatt limit has changed */
  lmtsmgr_notify_tech_plimit(vbatt.active_tech);

  MCSMARKER(VBATT_UPDATE_O);
}

/*=============================================================================

  FUNCTION:  lmtsmgr_process_vbatt_usb_sig

=============================================================================*/
/*!
    @brief
    This API corrects and set the valid HI/LO thresholds, given usb state
    is changed.

    @return
    None
*/
/*===========================================================================*/
void lmtsmgr_process_vbatt_usb_sig(void)
{
  /* Check for valid lo and hi thresholds and change the thresholds
     to include correction */
  if (vbatt.vbatt_threshold_low != -1)
  {
    vbatt_adc_set_threshold(PMAPP_BCL_LOW1_THR,
                            vbatt.vbatt_threshold_low);
  }
  if (vbatt.vbatt_threshold_high != -1)
  {
    vbatt_adc_set_threshold(PMAPP_BCL_HI1_THR,
                            vbatt.vbatt_threshold_high);
  }
}

/*=============================================================================
                              INTERNAL FUNCTIONS
=============================================================================*/

/*=============================================================================

  FUNCTION:  vbatt_get_corrected_limit

=============================================================================*/
/*!
    @brief
    Corrects the limits based on the usb connection info.
    NOTE: On CT given the ADC channel to directly read VBATT 
          is not present, there is a mismatch in the vbatt readings read.
          ADC VBATT read values are greater than actual for 
          the thresholds set. So, for the ADC read the correction
          is -ve and for setting the thresholds the correction is +ve.

    @param[in]  limit        Vbatt limit to be corrected
                corr_is_pos  Determines +ve/-ve correction

    @return
    Corrected limit
*/
/*===========================================================================*/
static uint32 vbatt_get_corrected_limit(uint32 limit, boolean corr_is_pos)
{
  uint32 correction, corrected_limit = limit;

  /* Check if usb is connected */
  if (vbatt.usb_is_conn)
  {
    /* Calc the correction to be applied */
    correction = ((limit - VBATT_MIN_OPERATIONAL_VOLTAGE)/VBATT_USB_CORR_SLOPE 
                   + VBATT_USB_CORR_OFFSET);

    if (corr_is_pos)
    {
      /* Correct the vbatt limit */
      corrected_limit += correction;
    }
    else
    {
      corrected_limit -= correction;
    }

    VBATT_MSG_2(MED, "Corrected VBATT limits to %d from %d", corrected_limit, 
                     limit);
  }

  return corrected_limit;
}


/*=============================================================================

  FUNCTION:  vbatt_adc_set_threshold

=============================================================================*/
/*!
    @brief
    Calls the ADC API to set the specified threshold.

    @param[in]  type    The threshold type to set, PMAPP_BCL_LOW1_THR or
                        PMAPP_BCL_HI1_THR
    @param[in]  limit   The threshold to set, in mV

    @return
    None
*/
/*===========================================================================*/
static void vbatt_adc_set_threshold(pmapp_bcl_thr_type type, uint32 limit)
{
  pm_err_flag_type result;
  uint32 corrected_vbatt_limit = vbatt_get_corrected_limit(limit, TRUE);
  /*-----------------------------------------------------------------------*/

  /* Set the corrected vbatt limit with bcl driver.
     NOTE: vbatt threshold HI/LO are saved without correction. */
  result = pmapp_bcl_set_thr_voltage(corrected_vbatt_limit, type,
                                     vbatt_adc_threshold_callback, TRUE);

  if(result == PM_ERR_FLAG__SUCCESS)
  {
    VBATT_MSG_2(MED, "Set threshold %d to %d", type, corrected_vbatt_limit);
  }
  else
  {
    VBATT_MSG_3(ERROR, "Unable to set threshold %d to %d: %d",
        type, corrected_vbatt_limit, result);
  }

  /* Save the limit for logging */
  switch(type)
  {
    case PMAPP_BCL_LOW1_THR:
      vbatt.vbatt_threshold_low = limit;
      break;

    case PMAPP_BCL_HI1_THR:
      vbatt.vbatt_threshold_high = limit;
      break;

    default:
      break;
  }
}

/*=============================================================================

  FUNCTION:  vbatt_adc_clear_threshold

=============================================================================*/
/*!
    @brief
    Calls the ADC API to clear the specified threshold.

    @param[in]  type    The threshold type to clear, PMAPP_BCL_LOW1_THR or
                        PMAPP_BCL_HI1_THR

    @return
    None
*/
/*===========================================================================*/
static void vbatt_adc_clear_threshold(pmapp_bcl_thr_type type)
{
  pm_err_flag_type result;
  /*-----------------------------------------------------------------------*/
  result = pmapp_bcl_set_thr_voltage(VBATT_BCL_COMP_CLEAR, type,
      vbatt_adc_threshold_callback, FALSE);

  if(result == DAL_SUCCESS)
  {
    VBATT_MSG_1(MED, "Cleared threshold %d", type);
  }
  else
  {
    VBATT_MSG_2(ERROR, "Unable to clear threshold %d: %d", type, result);
  }

  /* Save the limit for logging */
  switch(type)
  {
    case PMAPP_BCL_LOW1_THR:
      vbatt.vbatt_threshold_low = -1;
      break;

    case PMAPP_BCL_HI1_THR:
      vbatt.vbatt_threshold_high = -1;
      break;

    default:
      break;
  }
}

/*=============================================================================

  FUNCTION:  vbatt_adc_threshold_callback

=============================================================================*/
/*!
    @brief
    Callback called by ADC when a threshold set in the VADC BCL comparator is
    hit.

    Updates the vbatt value and signals the Limits Manager task to check the
    current vbatt level against the current limits.

    @param[in]  thresh_triggered  threshold type: LOW1 or HI1
    @param[in]  p2                Not used
    @param[in]  p3                Not used
    @param[in]  p4                Not used

    @return
    NULL
*/
/*===========================================================================*/
static void vbatt_adc_threshold_callback
(
  pmapp_bcl_thr_type thresh_triggered,
  void * p2,
  void * p3,
  void * p4
)
{
  MCSMARKER(VBATT_ADC_THRESHOLD);

  VBATT_MSG_1(MED, "Threshold %d triggered, reading ADC to confirm",
      thresh_triggered);

  /* Update the new vbatt level -- use special 'INVALID' value to force
   * an ADC read, to confirm the levels are actually in the new vbatt stage */
  rex_enter_crit_sect(&vbatt.vbatt_crit_sect);
  vbatt.current_vbatt = INVALID_VBATT_VALUE;
  rex_leave_crit_sect(&vbatt.vbatt_crit_sect);

  /* Signal the Limits Manager task to handle the updated level */
  lmtsmgr_set_sigs(LMTSMGR_SAR_VBATT_SIG);
}

/*=============================================================================

  FUNCTION:  vbatt_adc_read_callback

=============================================================================*/
/*!
    @brief
    Callback called by ADC when DalAdc_RequestConversion() completes.

    Updates the vbatt value and signals the Limits Manager task to check the
    current vbatt level against the current limits.

    @param[in]  object          Not used
    @param[in]  param           Not used
    @param[in]  vpayload        Pointer to the payload object, which is
                                expected to be a structure of type
                                AdcResultType
    @param[in]  payload_size    Size of the payload object, which is expected
                                to be sizeof(AdcResultType)

    @return
    NULL
*/
/*===========================================================================*/
static void * vbatt_adc_read_callback
(
  void * object,
  uint32 param,
  void * vpayload,
  uint32 payload_size
)
{
  const AdcResultType *payload =
    (const AdcResultType *)vpayload;

  MCSMARKER(VBATT_ADC_READ_RESP);

  VBATT_MSG_3(MED,
      "ADC read completed on channel %d: result %d new level is %d",
      payload->nChannelIdx,
      payload->eStatus,
      payload->nPhysical);

  /* Update the new vbatt level */
  rex_enter_crit_sect(&vbatt.vbatt_crit_sect);
  if(payload->eStatus == ADC_RESULT_VALID)
  {
    vbatt.current_vbatt = vbatt_get_corrected_limit(payload->nPhysical, FALSE);
  }
  else
  {
    /* Result was not successful. Set the current vbatt value to invalid and
     * signal the task. lmtsmgr_process_vbatt_event() will reissue the read
     * request. */
    vbatt.current_vbatt = INVALID_VBATT_VALUE;
  }
  rex_leave_crit_sect(&vbatt.vbatt_crit_sect);

  /* Signal the Limits Manager task to handle the updated level */
  lmtsmgr_set_sigs(LMTSMGR_SAR_VBATT_SIG);

  return NULL;
}

/*=============================================================================

  FUNCTION:  vbatt_set_test_override

=============================================================================*/
/*!
    @brief
    Diag command handler to set the vbatt override value, to test the vbatt
    core algorithm in cases where the actual vbatt level can't be easily
    manipulated

    @param[in]  req_pkt The input packet, representing the command request
                        from QXDM, which is expected to be of type
                        DIAG_SUBSYS_LIMITSMGR_VBATT_SET_req_type
    @param[in]  pkt_len The length of the input packet, which is expected to
                        be sizeof(DIAG_SUBSYS_LIMITSMGR_VBATT_SET_req_type)
    @example:           send_data 75 96 2 0 0x80 0x0c
                        Please refer lmtsmgr_diag.h for more info.

    @return
    Diag command response packet
*/
/*===========================================================================*/
static PACK(void *) vbatt_set_test_override
(
  PACK(void*) req_pkt,
  uint16 pkt_len
)
{
  DIAG_SUBSYS_LIMITSMGR_VBATT_SET_req_type       *req_ptr;
  DIAG_SUBSYS_LIMITSMGR_VBATT_SET_rsp_type       *rsp_ptr;

  /* Check if the request is valid */
  if(req_pkt == NULL)
  {
    VBATT_MSG_0(ERROR, "Invalid diag request to set DSI value");
    return NULL;
  }
  if(pkt_len != sizeof(DIAG_SUBSYS_LIMITSMGR_VBATT_SET_req_type))
  {
    VBATT_MSG_2(ERROR,
        "Invalid diag request to set DSI value (size %d, expected %d)",
        pkt_len, sizeof(DIAG_SUBSYS_LIMITSMGR_VBATT_SET_req_type));
    return NULL;
  }

  req_ptr = (DIAG_SUBSYS_LIMITSMGR_VBATT_SET_req_type *)req_pkt;

  rex_enter_crit_sect(&vbatt.vbatt_crit_sect);
  if(req_ptr->vbatt_value == 0x7fff)
  {
    VBATT_MSG_0(MED, "Clearing VBATT level override");
    vbatt.vbatt_test_override = INVALID_VBATT_VALUE;
    vbatt.current_vbatt = INVALID_VBATT_VALUE;
  }
  else
  {
    VBATT_MSG_1(MED, "Setting VBATT level override to %d",
        req_ptr->vbatt_value);
    vbatt.vbatt_test_override = req_ptr->vbatt_value;
  }
  rex_leave_crit_sect(&vbatt.vbatt_crit_sect);

  /* Signal the Limits Manager task to handle the updated level */
  lmtsmgr_set_sigs(LMTSMGR_SAR_VBATT_SIG);

  /* Allocate the memory for the response */
  rsp_ptr = (DIAG_SUBSYS_LIMITSMGR_VBATT_SET_rsp_type *)
            diagpkt_subsys_alloc((diagpkt_subsys_id_type)DIAG_SUBSYS_LIMITSMGR,
            (diagpkt_subsys_cmd_code_type)VBATT_SET,
            sizeof(DIAG_SUBSYS_LIMITSMGR_VBATT_SET_rsp_type));

  return rsp_ptr;
}

/*=============================================================================

  FUNCTION:  vbatt_set_test_active_tx

=============================================================================*/
/*!
  @brief
    Diag command handler to set the the active tx for the desired tech,
    to test the vbatt core algorithm in cases where the tx can't be easily
    manipulated

    @param[in]  req_pkt The input packet, representing the command request
                        from QXDM, which is expected to be of type
                        DIAG_SUBSYS_LIMITSMGR_VBATT_SET_ACTIVE_TX_req_type
    @param[in]  pkt_len The length of the input packet, which is expected to
                        be sizeof(DIAG_SUBSYS_LIMITSMGR_VBATT_SET_ACTIVE_TX_req_type)

    @example:           send_data 75 96 3 0 0 0x01 0x03 0x00 0x00 0xD2 0x09 0x20 0x03 0x00 0xD2 0x09 0x20 0x4e
                        for setting Nb1 tech:
                        send_data 75 96 3 0 0 0x02 0x03 0x00 0xD2 0x09 0x20 0x4e 0x03 0x00 0xD2 0x09 0x20 0x4e
                        Please refer the lmtsmgr_diag.h for more info.

  @return
    Diag command response packet
*/
/*=========================================================================*/
static PACK(void *) vbatt_set_test_active_tx (
  PACK(void*) req_pkt, /*!< request packet */
  uint16 pkt_len       /*!< request packet length */
)
{
  DIAG_SUBSYS_LIMITSMGR_VBATT_SET_ACTIVE_TX_rsp_type *rsp_ptr = NULL;
  DIAG_SUBSYS_LIMITSMGR_VBATT_SET_ACTIVE_TX_req_type *req_ptr = NULL;
  errno_enum_type estatus = E_SUCCESS;
  uint8 tech_id = 0;
  uint8 cntr = 0;

  LMTSMGR_ASSERT(req_pkt != NULL);
  /*-----------------------------------------------------------------------*/
  if( pkt_len != sizeof(DIAG_SUBSYS_LIMITSMGR_VBATT_SET_ACTIVE_TX_req_type) )
  {
    estatus = E_BAD_DATA;
  }
  else
  {
    /* De-mystify req_pkt */
    req_ptr = (DIAG_SUBSYS_LIMITSMGR_VBATT_SET_ACTIVE_TX_req_type*) req_pkt;
    LMTSMGR_ASSERT(req_ptr != NULL);

    tech_id = req_ptr->tech_id;
    lmtsmgr.tech_state[tech_id].currList.num_ul_entries = 1;
    lmtsmgr.tech_state[tech_id].currList.num_dl_entries = 1;
    lmtsmgr.tech_state[tech_id].currList.ul_freqList[0] = 0;
    lmtsmgr.tech_state[tech_id].currList.dl_freqList[0] = 1;
    if( 0 != req_ptr->ul_freq && 0 != req_ptr->ul_bw )
    {
      lmtsmgr.tech_state[tech_id].currList.links[0].freqInfo.link_info.frequency = (uint32)(req_ptr->ul_freq * 1000);
      lmtsmgr.tech_state[tech_id].currList.links[0].freqInfo.link_info.bandwidth = (uint32)(req_ptr->ul_bw * 1000);
      lmtsmgr.tech_state[tech_id].currList.links[0].freqInfo.link_info.band = (uint32)(req_ptr->ul_band);;
      lmtsmgr.tech_state[tech_id].currList.links[0].freqInfo.link_info.channel = 0;
      lmtsmgr.tech_state[tech_id].currList.links[0].freqInfo.link_info.direction = CXM_LNK_DRCTN_UL;
    }

    if( 0 != req_ptr->dl_freq && 0 != req_ptr->dl_bw )
    {
      lmtsmgr.tech_state[tech_id].currList.links[1].freqInfo.link_info.frequency = (uint32)(req_ptr->dl_freq * 1000);
      lmtsmgr.tech_state[tech_id].currList.links[1].freqInfo.link_info.bandwidth = (uint32)(req_ptr->dl_bw * 1000);
      lmtsmgr.tech_state[tech_id].currList.links[1].freqInfo.link_info.band = (uint32)(req_ptr->dl_band);
      lmtsmgr.tech_state[tech_id].currList.links[1].freqInfo.link_info.channel = 0;
      lmtsmgr.tech_state[tech_id].currList.links[1].freqInfo.link_info.direction = CXM_LNK_DRCTN_DL;
    }
  }

  VBATT_MSG_1(HIGH, "vbatt_set_test_active_tx called with estatus:%d", estatus);

  /* Allocate the memory for the response */
  rsp_ptr = (DIAG_SUBSYS_LIMITSMGR_VBATT_SET_ACTIVE_TX_rsp_type *)
            diagpkt_subsys_alloc((diagpkt_subsys_id_type)DIAG_SUBSYS_LIMITSMGR,
            (diagpkt_subsys_cmd_code_type)VBATT_SET_ACTIVE_TX,
            sizeof(DIAG_SUBSYS_LIMITSMGR_VBATT_SET_ACTIVE_TX_rsp_type));

  lmtsmgr_process_vbatt_freq_input(tech_id, req_ptr->sub_tech);

  if (rsp_ptr != NULL)
  {
    rsp_ptr->error = estatus;
  }

  return(rsp_ptr);

}

/*=============================================================================

  FUNCTION:  vbatt_npa_resource_avail_cb

=============================================================================*/
/*!
    @brief
    Callback indicating NPA resource for flow control is available.
*/
/*===========================================================================*/
static void vbatt_npa_resource_avail_cb
(
  void         *context,
  unsigned int  event_type,
  void         *data,
  unsigned int  data_size
)
{
  /* create NPA sync client for communicating flow control requests to CFCM */
  vbatt.flow_control_handle = npa_create_sync_client(vbatt_npa_resource,
                                  "MCS_VBATT_MON",
                                  NPA_CLIENT_REQUIRED);
  LMTSMGR_ASSERT(vbatt.flow_control_handle != NULL);
  VBATT_MSG_0(LOW, "Created NPA client handle for flow control");

  /* to start, issue request based on current stage */
  vbatt_issue_flow_control_req(vbatt.current_stage);
}

/*=============================================================================

  FUNCTION:  vbatt_issue_flow_control_req

=============================================================================*/
/*!
    @brief
    Initiate/cancel flow control request via NPA, based on the current VBATT
    stage.

    @param[in]  vbatt_stage: The vbatt stage to base the request off of
*/
/*===========================================================================*/
static void vbatt_issue_flow_control_req(int32 stage)
{
  uint32 level;
  /*-----------------------------------------------------------------------*/
  if(vbatt.flow_control_handle != NULL)
  {
    /* translate VBATT stage into thermal NPA req level. Valid levels are
     * [0 to 3]. 0 is the same as cancelling the request. */
    level = (stage > VBATT_STAGE_0 && stage < VBATT_STAGES_MAX) ?
      stage + 1 : 0;
    /* Send flow control request:
     *   send request to thermal (to forward to CFCM) with the new stage */
    npa_issue_required_request(vbatt.flow_control_handle, level);
    VBATT_MSG_2(HIGH,
        "Issued flow control request for level %u (vbatt stage is %d)",
        level, stage);
  }
  else
  {
    VBATT_MSG_0(ERROR, "Could not issue flow control req, NPA node missing");
  }
}

