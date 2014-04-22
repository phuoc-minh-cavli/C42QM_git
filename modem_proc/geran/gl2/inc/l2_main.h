#ifndef L2_MAIN_H
#define L2_MAIN_H

/*!
  @file l2_main.h

  @brief

  @detail

*/

/*===========================================================================

  Copyright (c) 2017 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //components/rel/geran.mpss/5.2.0/gl2/inc/l2_main.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
22/11/17   RRP     Initial Revision

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

#define L2_CMD_Q_SIG      0x0001    /* A command is available in the queue. */
#define L2_RPT_TIMER_SIG  0x0002    /* Used to signal a watchdog report.    */

/*===========================================================================

                         INTERNAL VARIABLES

===========================================================================*/

/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/
extern void gsm_l2_fn_init(void);

#endif /* L2_MAIN_H */
