/*!
  @file
  a2_power.h

  @brief
  The interface to the A2 power collapse feature implementation file

  @author
  araina
*/

/*==============================================================================

  Copyright (c) 2016 QUALCOMM Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/utils.mpss/2.3.4/api/a2/a2_power_poi.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
03/11/19   rs      CR2400964: Page only Image (PSM mode power save) changes
12/20/16   vt      CR1103363 NB1 changes
04/15/16   vt      CR997332 Memory reduction changes 
01/08/14   sn      CR776718: qtf changes
09/17/13   vd      CR531369,add a2_power_task_sleep\a2_power_reset_sleep_counter
09/13/13   vd      CR469984: Support for mount/unmount feature
08/06/13   yjz     Add A2_POWER_REQ_VOLTE_SLEEP for modem freeze
02/06/12   yjz     Fix Mob link error
10/19/12   ar      added A2_POWER_CLIENT_A2_IP_FILTER client
10/12/12   ar      added A2_POWER_REQ_SMSM_NAK smsm request
09/14/12   ar      added a2_power_send_ack_and_pc_notification_to_apps() api 
04/16/12   ar      added a2 pc mutex operation related apis
02/13/12   ar      added support for A2 PC + SPS RESET feature
01/06/12   ar      added a2_power_collapse_disabled() api 
                   split the A2_POWER_CLIENT_INTERNAL client into PHY and PER
11/22/11   ar      added support for UL PER client for a2 pc
11/10/11   st      Added A2 power collapse support for the WCDMA RRC module
05/24/09   ar      initial version
==============================================================================*/

#ifndef A2_POWER_H
#define A2_POWER_H

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include <comdef.h>
#include "a2_common.h"
#include "a2_power.h"


//@TODO: Move it to utils/api/a2 and remove utils/a2_poi

/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/
/*==============================================================================

  FUNCTION:  a2_power_poi_wakeup

==============================================================================*/
/*!
  @brief
  initializes the A2 HW and driver structures for DL PHY.

*/
/*============================================================================*/
boolean a2_power_poi_wakeup
(
  void
);

/*==============================================================================

  FUNCTION:  a2_power_poi_shutdown

==============================================================================*/
/*!
  @brief
  De-initializes the A2 HW and driver structures for DL PHY.

*/
/*============================================================================*/
boolean a2_power_poi_shutdown
(
  void
);

/*==============================================================================

  FUNCTION:  a2_power_poi_psm_init

==============================================================================*/
/*!
  @brief
  Function initializes A2 data structures like modem bootup scenario.

  To be called by ML1 when coming up from PSM

*/
/*============================================================================*/
void a2_power_poi_psm_init();

/*==============================================================================

  FUNCTION:  a2_power_poi_psm_deinit

==============================================================================*/
/*!
  @brief
  De-initialize A2 completely like modem is going to shutdown.

  To be called by ML1 when going to PSM mode.
*/
/*============================================================================*/
void a2_power_poi_psm_deinit();

#endif /* A2_POWER_H */

