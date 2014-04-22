/*!
  @file
  nb1_startup.h

  @brief
  This file contains all External interfaces exported by Test Loopback Module.


*/

/*===========================================================================

  Copyright (c) 2016 QUALCOMM Technologies Incorporated. All Rights Reserved

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

$Header: //components/rel/nb1.mpss/2.2/api/nb1_startup.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/22/10   AE      Initial version
===========================================================================*/

#ifndef NB1_STARTUP_H
#define NB1_STARTUP_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include <msgr.h>
#include <msgr_umid.h>
#include <appmgr.h>
#include <customer.h>
#include <lte_app.h>

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/


/*===========================================================================

  FUNCTION:  nb1_tlb_ctrl_task_init

===========================================================================*/
/*!
    @brief
    This function is a initialization function for TLB Control Task.

    @detail
    Initialization consists of:
    - Initialization TLB control attributes
    - Initialization stack size and priority
    - Starting the TLB Control Task

    @return
    Thread ID

    @note

    @see related_function()

*/
/*=========================================================================*/
EXTERN void nb1_startup_get_thread_table
(
  appmgr_thread_init_entry_s **p_table,
  uint32 *num_entries
);

/*===========================================================================

  FUNCTION:  nb1_get_plt_mode_enabled

===========================================================================*/
/*!
    @brief
    This function checks if NB1 PLT mode is enabled.

    @detail
    getter for a static variable defined in nb1_startup.c

    @return
    value of nb1_plt_enabled flag

*/
/*=========================================================================*/
extern boolean nb1_get_plt_mode_enabled (void);

/*===========================================================================

  FUNCTION:  nb1_get_plt_snoop_mode

===========================================================================*/
/*!
    @brief Returns if MSGR snooping is enabled.  

    @detail

    @return None

*/
/*=========================================================================*/
boolean nb1_get_plt_snoop_mode (void);

/*===========================================================================

  FUNCTION:  nb1_get_plt_as_mode_enabled

===========================================================================*/
/*!
    @brief Whether PLT AS mode is enabled or not.  

    @detail

    @return True if enabled, else False

*/
/*=========================================================================*/

extern boolean nb1_get_plt_as_mode_enabled (void);

#endif /* NB1_STARTUP_H */
