/*******************************************************************************
* Copyright (c) 2018-2019, STMicroelectronics.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *     2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     3. Neither the name of the STMicroelectronics nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 ******************************************************************************/

/*==============================================================================
  Change Log:

  EDIT HISTORY FOR FILE
  Fri  Sept 8 2017 ST - 1.4
  - Updated for OpenSSC-5.0.5
  - Fix Flush_REQ issue
  Thur  Nov 9 2017 ST - 1.5
  - fix pressure and temperature published ATTRID_RATES
  Wed  Nov 15 2017 ST - 1.6
  - move island functions to island files
  - add process_timer_events subfunction and POWER_RAIL_PENDING_OFF handling
  - complete log support
  - Enable periodic in polling timer and add timestamp printing in the log
  - Use nominal timestamp instead of timer timestamp
  - Remove 50Hz support
  Wed  Nov 29 2017 ST - 1.7
  - add self test - COM support
  - log optimization
  - add oem specific feature interface functions
  Tue  Dec 5 2017 ST - 1.8
  - fix the issue that update_siblings get called twice
  - fix com port keeping open issue
  - clean up irq stuff from polling mode
  - remove DISABLE_REGISTRY for cleaning up
  - remvoe redandent dependencies initialization
  Fri Dec 15 2017 ST - 1.9
  - Fix timing issue with power rail off timer
  Tue Feb 20 2018 ST - 1.10
  - Fix Active current attribute ID
  - Add SNS_STD_SENSOR_ATTRID_PHYSICAL_SENSOR attribute
  Wed Mar 7 2018 ST - 2.0
  - Merged Driver for LPS22HB and LPS22HH
  - Log optimizations
  Wed Apr 25 2018 ST - 2.1
  - Add publish event for SELECTED RANGE
  - Fix ADSP crash due to timer stream
  - Adding initial samples discard support
  Tue Oct 16 2018 ST - 2.2
  - Updating samples to discard
  Fri Dec 21 2018 ST - 2.3
  - Added COM self test support
  Fri Jan 30 2019 QC - 2.4
  - Fixed slave addr for target SM_8250
  Fri Jan 30 2019 QC - 2.5
  - Updated Build FLAGs to use 5.0 HW TAGs
  Thur Feb 21 2019 ST - 2.4
  - use timer timestamp instead of calculation for polling mode
  - reconfig only when odr/config change
  - add LPF support
  Wed Feb 27 2019 ST - 2.4.0.1
  - fix temperature COM test failure.
  Mon Mar 11 2019 ST - 2.5
  - Update the message decode sequence for factory tests
  Mon Apr 08 2019 QCOM - 2.6.0
  - Merged ST driver version 2.5 onto QC driver version 2.5
  - Updated driver version to comply with major[31:16].minor[15:8].revision[7:0]
  Wed Jan 23 2020 QCOM - 2.6.1
  - Support for standalone COM test
============================================================================*/

#ifndef _SNS_LPS22HX_VER_H
#define _SNS_LPS22HX_VER_H

#define SNS_VERSION_LPS22HX        0x00020601


#endif //_SNS_LPS22HX_VER_H

