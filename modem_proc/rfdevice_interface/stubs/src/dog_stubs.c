/*!
  @file
  dog_stubs.c

  @brief
  REQUIRED brief one-sentence description of this C module.

  @detail
  OPTIONAL detailed description of this C module.
  - DELETE this section if unused.

*/

/*===========================================================================

  Copyright (c) 2008-2015 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //components/rel/rfdevice_interface.mpss/1.40/stubs/src/dog_stubs.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
#07/16/15   avi    Initial Version
===========================================================================*/

/*===========================================================================
File:C:/Q6/SCMSync/MOBs/paragk_PARAGK_modem.1h09.tip/core//dog/inc/dog.h
===========================================================================*/
#include "dog.h"

dog_report_type dog_ftm_rpt;              

const rex_timer_cnt_type   dog_ftm_rpt_time = 1;                  /* time_decl */ 

/* Makes the watchdog timer wait */
void HAL_dogKick( void ){ /* Ouch! */ };  



