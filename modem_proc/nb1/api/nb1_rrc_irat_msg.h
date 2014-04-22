/*!
  @file
  nb1_irat_types.h

  @brief
  This file describes the interfaces to NB1 Radio resource Controller (RRC_IRAT)

   <!-- Doxygen chokes on the @mainpage vs. @page inside a conditional, but the
       following seems to work.  Basically, make this the mainpage if it
       is the only component being built, otherwise, just make it a page and
       let the upper level component/s @ref it. -->
  @if rrc
  @mainpage NB1 Radio Resource Controller (RRC_IRAT)
  @endif
  @ifnot rrc
  @page NB1 Radio Resource Controller (RRC_IRAT)
  @endif

  RRC_IRAT is a sub-module of RRC. This file contains data strucutres and UMIDs
  for messages that RRC will use to communicate with other RATs over
  message router.

  <h2>High level architecture</h2>
  <img src='../pics/rrc_arch.jpg'>

  <h2>API</h2>
 <h3><i>Message Interfaces</i></h3>
   -- External API to RRC_IRAT is defined in @ref nb1_irat_types.h

 <h3><i>Function Call Interfaces</i></h3>
  This module does not have any function call API.

  <b><i> QXDM Log Parser </i></b>
  - @ref QXDM_Parser_DLL

  @ref umids
  @ref all_umids

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

$Header: //components/rel/nb1.mpss/2.2/api/nb1_rrc_irat_msg.h#1 $

when       who       what, where, why
--------   ---       ----------------------------------------------------------


===========================================================================*/

#ifndef NB1_RRC_IRAT_MSG_H
#define NB1_RRC_IRAT_MSG_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include <lte_rrc_irat_msg.h>

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

#endif /* NB1_RRC_IRAT_MSG_H */

