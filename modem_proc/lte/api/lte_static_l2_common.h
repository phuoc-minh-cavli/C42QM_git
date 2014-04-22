/*!
  @file
  lte_static_l2_common.h

  @brief
  This file contains common defines  and functions interfaces for LTE L2 layer.


*/

/*===========================================================================

  Copyright (c) 2015 QUALCOMM Technologies Incorporated. All Rights Reserved

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

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/lte.mpss/4.7.2/api/lte_static_l2_common.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/22/18   ss      Initial version
===========================================================================*/

#ifndef LTE_STATIC_L2_COMMON_H
#define LTE_STATIC_L2_COMMON_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include <comdef.h>
#include <msgr.h>
#include <lte_l2_comdef.h>
#include <lte_l1_types.h>
#include <lte_mac_msg.h>
#include <msgr_nb1.h>
#include <msgr_lte.h>
#include <nb1_assert.h>

/*===========================================================================

                               MACROS

===========================================================================*/

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/


/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

#define MAX_L2_M1_TASK   8

#define MAX_L2_NB_TASK   8

#define LTE_L2_COMMON_TASK_MAX_MSG   1

#define LTE_L2_COMMON_MAX_L2_TASK   (MAX_L2_M1_TASK + MAX_L2_NB_TASK)

/*! @brief Define Mailbox priority for static Task
*/
#define LTE_L2_COMMON_MB_PRIO 1


/*! @brief Global CS structure for L2 Static
*/
typedef struct
{
  boolean        task_is_active;/*!< Whether the "main" function of the thread is running */
  uint16         msgr_tech_module; 
  char           msg_name[30]; 
  msgr_client_t  msgr_client;  /*!< MSGR client */
  msgr_id_t      mb_id;        /*!< Mailbox Id */
  void *(*lte_l2_common_task_cb)(void *);
  uint16          task_mask;
} lte_static_l2_common_cs_s;

/*! @brief defined Bootup type 
*/
typedef enum
{
  LTE_NB1_L2_MAC_DL_TASK_ID  =      0,
  LTE_NB1_L2_MAC_UL_TASK_ID,
  LTE_NB1_L2_MAC_CTRL_TASK_ID, 
  LTE_NB1_L2_RLC_UL_TASK_ID, 
#if defined (FEATURE_LTE_UDC_ENABLED) || defined (FEATURE_NB1_UDC_ENABLED)
  LTE_NB1_L2_PDCP_COMP_TASK_ID,
#endif 
  LTE_NB1_L2_TASK_MAX_ID
 }lte_static_l2_task_id_e;

#endif /* LTE_L2_COMMON_H */

