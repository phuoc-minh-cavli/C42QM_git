#pragma once
/*
 * Copyright (c) 2019 - 2020, ams AG
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef AMS_TMD2725_VERSION_H
#define AMS_TMD2725_VERSION_H

/*******************************************************************************
 * Version History
 * tmd2725 for OpenSSC
 *
 *   Date
 * YYYYMMDD  Who  Version  Changes
 * --------  ---  -------  -----------------------------------------------------
 * 20170808  span 1.0.0    Initial SEE release
 * 20170821  span 1.2.0    Transformed amsfmt to qfmt
 *                         Added modified json files
 *                         Added modified registry support
 *                         DRI after PROX calibration working
 * 20170822  span 1.3.0    Removed the non required RGB and XYZ stuffs
 * 20170823  span 1.3.1    Removed duplicate definitions
 * 20170823  span 1.3.2    Publish calibration data events.
 * 20170823  span 1.4.0    Island mode refactoring optimization.
 * 20170823  span 1.4.1    Island mode refactoring according the latest guidelines.
 * 20170829  span 1.7.1    As per OpenSSC 5.0.5
 *                         Handled ** for sns_sync_com_port_handle
 *                         Modified .scons
 * 20170912  span 1.7.2    Updated JSON file
 * 20171017  span 1.7.3    Changed PLDRIVE = 0x1E to
 *                         PLDRIVE = 0x1F
 * 20171017  span 1.7.4    Some i2c bus usage optimizations.
 *                         Rectified ALS saturation issue.
 *                         Modified the tmd2725_deviceEventHandler()
 *                         Rectified no ALS DRI after ALS calibration.
 *                         Solution: Enable interrupt after receiving
 *                         SNS_INTERRUPT_MSGID_SNS_INTERRUPT_REG_EVENT has been
 *                         received in tmd2725_inst_notify_event().
 *                         Rectified ALS saturation issue when TMD2725 exposed
 *                         to strong light. ASAT is cleared after an interrupt
 *                         event.
 * 20171019  span 1.7.5    Fix interrupt registering/enabling race. Fix an i2c optimization.
 * 20171024  span 1.7.6    Updating rawClear data in data[1].
 *                         Updating rawRed data in data[2].
 *                         Changed  the macro AMS_PORT_LOG_CRGB_W() to  AMS_PORT_LOG_CR()
 *                         and  the macro AMS_ALS_GET_CRGB_W() to AMS_ALS_GET_CR(),
 *                         as green (G), blue (B) and wideband (W) are not present.
 *                         Removed Green, Blue and Wideband stuffs from the code.
 *                         Some more code cleanup.
 *                         Made changes in the function tmd2725_inst_notify_event().
 *                         Incremented the macro ALS_PROTO_STD_ARRAY_LEN by 1 i.e.
 *                         Now, ALS_PROTO_STD_ARRAY_LEN = 3
 *                         ALS in DRI mode mode updates data whenever there is
 *                         a change in data.
 *                         ALS, after strong external light condition, data update
 *                         won't stop
 * 20171024  span 1.7.7    Resolved the issue, while ALS is working, first event is not
 *                         happen after proximity mode enable ,->  disable,->  enable.
 *                         Conditional enabling of ASIEN through compile switch
 *                         in the scons file.
 * 20171030  span 1.7.8    Conditional enabling of ASIEN through compile switch
 *                         in the scons file. Added compile flag CONFIG_AMS_USE_ASIEN
 *                         in the scons file, for,selecting whether to use ASIEN
 *                         or not. By default, it is disabled.
 * 20171031  span 1.7.9    Removed malloc as it doesn't adheres to the QCOM SEE
 *                         guidelines.
 * 20171101  span 1.7.10   TMD2725 QCOM driver in island mode
 * 20171129  span          Removed comments generated from the script to
 *                         convert ams format to QCOM format.
 * 20171213  span 1.7.11   Proximity modification to cope with glass cover.
 *                         AFE Saturation handle by reducing ALS gain.
 *                         ALS Calibration after AFE saturation by reducing the
 *                         ALS gain.
 * 20180308  span 1.8      Updating rawIR in the proto buffer.
 *                         Removed rawRed instances as there is no red channel.
 *                         SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_PHYSICAL_CONFIG_EVENT
 *                         is only sent when the ALS Gain changes.
 *                         Using correct register name for ALS IR channel.
 *                         Removed AMS_ALS_GAIN_V2.
 * 20180319  span 1.9      Reading config param of ALS and PROX from .json file.
 *                         A number of changes in various functions.
 * 20180320  span 1.10     Added POFFSET to the proximity results.
 * 20180320  span 1.11     Added the Prox attribute SNS_STD_SENSOR_ATTRID_SELECTED_RANGE
 *                         and ALS attribute SNS_STD_SENSOR_ATTRID_SELECTED_RANGE.
 * 20180410  span 1.12     Modified ALS AGC algorithm.
 * 20180412  span 2.0      This modification enables the driver to be used along
 *                         with other ams ALS QCOM drivers.
 *           ds            QCOM requested changes (case 03443860)
 * 20180427  span 2.1      QCOM requested changes (case nos: 03452140, 03443860,
 *                         03449635, 03368894, 03449634)
 *                         Changed the macro UMSG to MSG.
 *                         Fixed with a compile flag in the scons, CONFIG_ALS_INCLUDE_IR_RESULTS.
 *                         These issues are fixed in the driver code. The QCOM driver sends
 *                         physical config events to the framework. The issue is fixed in the
 *                         function tmd2725_als_send_config_update(). The fix is, if current mode
 *                         is AMS_STRM_IDX, then DRI_ENABLED FLAG is set to false.
 *                         Rectified the ALS range. Earlier it was {0,1} as min and max ALS
 *                         range. Fixed with max calculated lux.
 * 20180510  span 2.2      Fixed case: 03452144 i.e. Prox in streaming not
 *                         generating sns_diag_raw_sensor_event.
 *                         Removed MSG macro as it will be deprecated.
 *                         Added AMS_INST_PRINTF and AMS_PRINTF macros for debug
 *                         messages.
 * 20180608 span  2.3      Segregated proximity HW  and 5cm card away calibration.
 *                         Added registry support for proximity HW calibration.
 *                         Added functionality to RW POSSET_L and POFFSET_H
 *                         values from msm8996_tmd2725.json file and registry.
 *                         Reporting change range and resolution of ALS lux after
 *                         ALS gain change.
 *                         Some minor fixes for attribute reporting e.g. Vendor
 *                         and sensor name change.
 * 20180706 span  2.4      First release with multisensor support
 * 20180717 pbnt  2.5      Added Unique SUIDS for all sensors
 * 20180719 pbnt  2.6      Added use of get_registration_index()
 * 20180731 pbnt  2.7      Merged multisensor to master
 *                         Added use of UNUSED_VAR() per QC reqeust
 *                         Fixed Klocwork issues
 *                         Prox near_threshold now 8-bit value in registry
 * 20180911 pbnt  2.8      Fixed prox issue where counts we max but we reported FAR
 *                         with high accuracy.
 *                         Changed logging priorities from ERROR to HIGH
 *                         Added publish of SNS_STD_SENSOR_ATTRID_SELECTED_RESOLUTION for als sensor
 *                         Swapped order of sns_cal_event and sns_physical_config_event
 *                         Added function to calculate proximity active current
 *                         Removed publish SNS_STD_SENSOR_ATTRID_RESOLUTIONS from the prox sensor
 * 20180925 pbnt  2.9      Fixed format string in suid log
 * 20181002 pbnt  2.10     Separated developer logging from functional logging
 * 20181109 pbnt  2.11     - Fixed issue where factory test stops prox from sending
 *                           data to concurrent clients (QTI case 03655638)
 *                         - Fixed issue where ALS/Prox is publishing duplicate events
 *                           to end client (QTI case 03722597)
 *                         - Fixed issue where als gives very high value when
 *                           light is blocked and unblocked (QTI case 03693991)
 *                         - Removed redundant amsLibrary_open_tmd2725 call.
 * 20181205 span  2.12     - Proximity sensor resolution attribute(QTI case 03643055)
 *                         - TMD2725 SEE: ALS sensor missing selected resolution
 *                           attribute (QTI case 03634719)
 *                         - ALS/Proximity shoudn't send out sns_cal_event in
 *                           connectivity test (QTI case 03695071)
 *                         - Resolved TMD2725 SEE: Proximity sensor TMD2725
 *                           streaming at non ODR rate 15 (QTI case 03728916, 03718454)
 * 20181220 pbnt  2.12     - Resolved issue where the driver was reporting available
 *                           even if a part was not found on the I2C bus.
 *                         - Resolved race condition leading to reporting garbage data
 *                         - Merge from QC_v2.5.3: Handle sns_registry_write_event.
 *                           Registry sensor was updated:
 *                           an sns_registry_write_event is sent for every
 *                           sns_registry_write_req.
 * 20190405 pbnt/qc 2.13.0 - Added calibration reset logic: SNS_CAL_MSGID_SNS_CAL_RESET
 *                         - Fixed reset cal registry write problem.
 *                         - Changed default ptime to 20Hz
 *                         - Added target lux and target lux tolerance to JSON files
 *                           for use when calibrating ALS.
 *                         - Fixed issue where
 *                             1. Enable ALS
 *                             2. Enable PROX
 *                             3. Disable PROX
 *                             4. Enable PROX <= was FAIL, now PASS
 *                             and
 *                             1. Enable PROX
 *                             2. Enable ALS
 *                             3. Disable ALS
 *                             4. Enable ALS  <= was FAIL, now PASS
 *                         - ENABLE_DEBUG can be commented out in the scons file
 *                           and there are no problems.
 *                         - Added SNS_PRINTF and SNS_INST_PRINTF calls to trace
 *                           SEE protocol. These logs are always on and approved by QC.
 *                         - (qc) Added handling for REGISTRY_WRITE_EVENT
 *                         - (qc) Fixed driver version to comply with
 *                                major[31:16].minor[15:8].revision[7:0]
 *                         - (qc) Fixed array indexing in tmd2725_calculate_prox_active_current
 *                                which was causing a target crash and i2c errors
 *                         - (qc) Fixed pragma once, and ensured to exit island
 *                                only when #ifdef ENABLE_DEBUG
 * 20190502 qc 2.13.1      - Changes fix the problem where Proximity factory calibration
 *                           was not producing the right threshold value: Reverted latest
 *                           change to retrieving proxHardThreshold and proxMinThreshold
 *                           from registry. Removed flag-control for function
 *                           tmd2725_write_to_prox_fac_cal, to match vendor's working
 *                           implementation. Updated max value for near_threshold, to prevent
 *                           that Prox reaches saturation due to proximity calibration.
 * 20190502 qc 2.13.2      - Fix to Turn off the power rails when sensor is OFF
 * 20190328 pbnt  2.14     - Fixed issue of high lux transient with sudden changes
 *                           in ambient light.
 *                         - Fixed corner case of saturation in during prox calibration
 * 20190408 jzhu  2.15     - Fixed issue of Qualcomm All-Ways Aware hub Validation Tool
 *                           test cases 233.and 733, which are test cases for concurrency of
 *                         - ambient light and factory calibration
 * 20190409 jzhu  2.16     - Fixed issue of Qualcomm All-Ways Aware hub Validation Tool
 *                         - test cases ALS230/ALS730, PRX230/PRX730, test cases of
 *                         - factory calibration when no sensor connected
 * 20190416 jzhu  2.17     - Added Prox default configuration for WTIME register
 *                         - WTIME is configurable through Json file for PROX sensor
 *                         - Both ALS sensor and PROX sensor have their own WTIME setting
 *                         - If WTIME are different for 2 sensors in Json file, the setting of
 *                         - sensor that is activated later will overwrite the previous one
 * 20190425 jzhu  2.18     - send default calibration data in sns_cal_event after sns_cal_reset request
 *                         - merged QC_v2.13 changes (pbnt)
 * 20190429 jzhu  2.19     - Modified to meet timing requriment while execute fac_cal and measurement
 *                         - at the same time( PRX233/733 and ALS233/733 )
 *                         - send 8 bits prx threshold in cal_event
 * 20190514 qc    2.19.1   - Increased AMS_LUX_AVERAGE_COUNT to 100 to make impact of first two
 *                           outliers negligible. Enabled raw log packets for only ambient light.
 *                           Raw log packets are not required for proximity.
 * 20190604 qc    2.19.2   - Enabled proximity raw log packets. Proximity events generated by
 *                           Proximity streaming sensor now adhere to new message id:
 *                           SNS_PROXIMITY_MSGID_SNS_PROXIMITY_EVENT_RECURRENT  = 1031.
 * 20190523 jzhu  2.20     - Fixed issue of physical_config_event for the following cases (Issue #28):
 *                           - physical_config_event needs to be sent before cal_event for cal_reset_request
 *                           - physical_config_event needs to be sent before cal_event for factory tests
 * 20190530 jzhu  2.21       Fixed sns_cal_event missing issues. (Issue #31,33)
 *                           Each newly on sensor should send cal_event after physical_config_event
 *          pbnt           - Fixed issue in ALS calibration where autogain was causing outliers in the
 *                           calibration data set. (Issue #30)
 * 20190607 jzhu             Decouple the sns_cal_reset_request from other standard request. Don't call
 *                           tmd2725_reval_instance_config function for sns_cal_reset_request.
 *                           Process sns_cal_reset_request independently
 *                           Fixed issues in Proximity measurement test case PRX_733.
 *                           Use Hysteresis right after calibration. Clamp threshold with 0xfe00 and minThreshold
 * 20190611 jzhu             Timestamp for physical_config_event and cal_event should remain the same if there is no
 *                           change in the physical configuration and calibration.
 *                           Cal_event should be sent after the physical_config_event if a new client is registered
 * 20190625 qc 2.21.1      - Fix to Turn off the power rails when streaming sensor is OFF
 * 20190722 qc 2.21.2      - Fixed out of order events for on-change sensors (ALS/PROX).
 *                         - Included Julie's fix for gain value via als_update_statics
 * 20190813 qc 2.21.3      - Upating MAX_ODR published for ALS (same as PROX) from 10Hz to 20Hz.
 *
 * 20190724 jzhu  2.22       Prox sensor active current attribute is upadted after registry value was received
 * 20190730 jzhu  2.22.1     Applied QComm bug fix ( details refer to Artifact 45783 )
 *                           Call als_update_statics() right after als gain changed, so that maxLux and
 *                           luxResolution are updated accordingly.
 * 20190802 jzhu  2.22.2   - Fixed Prox on change reporting issue when run on-change/stream prox at the same time.
 *                           Prox on-change reporting are missing if both on-change and streaming are enabled at the same time
 *                           The reason is on-change and streaming are sharing the same state machine. If streaming entered the state machine
 *                           first and changed the state, later, when on-change enter the state machine, it won't be aware of the change
 *                           and won't update the updateavailable flag.
 * 20190823 jzhu  2.22.3   - Fixed ALS factory test's request and response mismatch. First attempt to fix resolution attribute.
 * 20190911 qc    2.22.4   - Updated driver to not publish UNRELIABLE event for PROX on change.
 * 20190913 qc    2.22.5   - Reverted change 2.21.3 because it was the incorrect fix to make, for this tmd2725 part
 *                         - Added validation for request, applicable to new and replace requests
 * 20190930 qc    2.23.1   - Fixed return value, for rejected request having odr (sample rate) outside of valid range
 * 20191001 qc    2.23.2   - Added logic to remove newly created instance in case adding request fails
 * 20191022 qc    2.23.3   - Fix for KW errors due to usage of unassigned stack variable.
 * 20191031 pbnt  2.23.4   - Fix for case 04214084 ALS and Prox calibration failure in concurrency with Prox on change client
 * 20191210 pbnt  2.23.5   - Fix for OC prox continuous interrupts
 * 20200127       2.23.6   - Updated hardcoded registry and corresponding code changes
 * ******************************************************************************/

/* major[31:16].minor[15:8].revision[7:0] */
#define AMS_DRIVER_VERSION   0x00021706

#endif /* AMS_TMD2725_VERSION_H */
