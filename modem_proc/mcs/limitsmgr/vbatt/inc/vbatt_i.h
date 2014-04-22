/*!
  @file vbatt_i.h

  @brief
   Internal header for VBATT limit support

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

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/mcs.mpss/5.1/limitsmgr/vbatt/inc/vbatt_i.h#3 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/31/16   btl     Use BCL comparators instead of VADC polling
02/26/16   btl     Added NPA client for flow control
06/01/15   rj      Added TA feature flag
05/29/15   tl      Initial Revision

=============================================================================*/

#ifndef VBATT_I_H
#define VBATT_I_H

/*=============================================================================
                                INCLUDE FILES
=============================================================================*/

#include "cxm_intf_types.h"
#include "cxm.h"
#include "vbatt_efs.h"
#include "DDIAdc.h"
#include "npa.h"

/*=============================================================================
                              MACRO DEFINITIONS
=============================================================================*/

#define INVALID_VBATT_VALUE (0x7fffffff)

#define VBATT_MSG_0(LVL, TEXT) \
  MSG(MSG_SSID_LMTSMGR, MSG_LEGACY_ ## LVL, "VBATT: " TEXT)
#define VBATT_MSG_1(LVL, TEXT, A) \
  MSG_1(MSG_SSID_LMTSMGR, MSG_LEGACY_ ## LVL, "VBATT: " TEXT, A)
#define VBATT_MSG_2(LVL, TEXT, A, B) \
  MSG_2(MSG_SSID_LMTSMGR, MSG_LEGACY_ ## LVL, "VBATT: " TEXT, A, B)
#define VBATT_MSG_3(LVL, TEXT, A, B, C) \
  MSG_3(MSG_SSID_LMTSMGR, MSG_LEGACY_ ## LVL, "VBATT: " TEXT, A, B, C)
#define VBATT_MSG_4(LVL, TEXT, A, B, C, D) \
  MSG_4(MSG_SSID_LMTSMGR, MSG_LEGACY_ ## LVL, "VBATT: " TEXT, A, B, C, D)

/*=============================================================================
                                   TYPEDEFS
=============================================================================*/

typedef struct
{
  /** Critical section protecting vbatt data
   *
   * This critical section must be held while accessing current_vbatt or
   * vbatt_test_override, which are accessed from multiple threads. The
   * remainder of the data in this structure is accessed only from the Limits
   * Manager thread, so holding the critical section while accessing it is not
   * necessary. */
  rex_crit_sect_type            vbatt_crit_sect;
  /** The current vbatt value, in mV, as returned from ADC; or
   * INVALID_VBATT_VALUE if the vbatt value is not valid. */
  int32                         current_vbatt;
  /** The override value set by the Diag command from QXDM to test the vbatt
   * algorithm, or INVALID_VBATT_VALUE if the override value is not set. */
  int32                         vbatt_test_override;

  /** The current active transmitting tech/band, or SYS_BAND_CLASS_NONE if no
   * technology is active. Only one active transmit is supported; DSDS is
   * supported but DSDA is not. */
  sys_band_class_e_type         active_band;
  /** The current active transmitting tech, or CXM_TECH_DFLT_INVLD if no
   * technology is active. */
  cxm_tech_type                 active_tech;
  /** The current active transmitting sub_tech   */
  cxm_lte_sub_tech_type         active_sub_tech;
  /** Pointer to the record read from EFS for the current active tech/band, or
   * NULL if the record is not specified or no technology is active. */
  const vbatt_record_type      *record;
  /** The current stage. If the value is 0 or 1, this value is used to index
   * into the stage array. */
  vbatt_stage_type              current_stage;
  /** Timer used to support the hysteresis timeout to keep from transitioning
   * rapidly between stages. */
  timer_type                    hysteresis_timer;
  /** The system timestamp, in milliseconds, at which the hysteresis timer
   * will expire; or 0 if the hysteresis timer is not set. */
  uint32                        hysteresis_timer_expires;
  /** The stage from which the hysteresis timer prevents transitioning back to
   * until the timer expires. */
  vbatt_stage_type              hysteresis_stage;

  /** DAL device handle for the ADC device for reading the current vbatt value
   * and setting thresholds. */
  DalDeviceHandle              *adc_handle;
  /** DAL event handle used when ADC completes an asynchronous read. The event
   * handle is configured as a callback handle to call
   * vbatt_adc_read_callback(). */
  DALSYSEventHandle             adc_read_event;
  /** DAL ADC properties type to store the device id, channel id, and other
   * information necessary to read the vbatt channel. */
  AdcInputPropertiesType        adc_properties;

  /** The current low ADC threshold, in mV; or -1 if no threshold is set. Used
   * for logging the current thresholds. */
  int32                         vbatt_threshold_low;
  /** The current high ADC threshold, in mV; or -1 if no threshold is set.
   * Used for logging the current thresholds. */
  int32                         vbatt_threshold_high;
  /** NPA sync client handle for requesting flow control mitigation to CFCM.
   * Will be aggregated with QMI service for TMD. */
  npa_client_handle             flow_control_handle;
  /** Usb conn state variable */
  boolean                       usb_is_conn;
} vbatt_type;

/*=============================================================================
                            VARIABLE DECLARATIONS
=============================================================================*/

extern vbatt_type vbatt;

/*=============================================================================
                            FUNCTION DECLARATIONS
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
void vbatt_init(void);

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
void vbatt_deinit(void);

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
                                      cxm_lte_sub_tech_type lte_sub_tech);

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
void lmtsmgr_process_vbatt_event(void);


/*=============================================================================

  FUNCTION:  lmtsmgr_vbatt_set_usb_conn_state

=============================================================================*/
/*!
    @brief
    Sets the USB connection state of the modem/device.

    @return
    None
*/
/*===========================================================================*/
void lmtsmgr_vbatt_set_usb_conn_state(boolean usb_is_conn);


/*=============================================================================

  FUNCTION:  lmtsmgr_process_vbatt_usb_sig

=============================================================================*/
/*!
    @brief
    This API does the required processing if the USB sig is set.
    Basically, adjust and set the valid HI/LO thresholds.

    @return
    None
*/
/*===========================================================================*/
void lmtsmgr_process_vbatt_usb_sig();

#endif /* VBATT_I_H */
