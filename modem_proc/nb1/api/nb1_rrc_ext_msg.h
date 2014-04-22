/*!
  @file
  nb1_rrc_ext_msg.h

  @brief
  This file describes the interfaces to NB1 Radio resource Controller (RRC)

   <!-- Doxygen chokes on the @mainpage vs. @page inside a conditional, but the
       following seems to work.  Basically, make this the mainpage if it
       is the only component being built, otherwise, just make it a page and
       let the upper level component/s @ref it. -->
  @if rrc
  @mainpage NB1 Radio Resource Controller (RRC)
  @endif
  @ifnot rrc
  @page NB1 Radio Resource Controller (RRC)
  @endif

  RRC module is the heart of the configuration and control for the NB1 access
  stratum. This layer intercepts all the over the air messages and configures
  all the lower layer modules. This module is also in charge if
  tracking system information change, connection set up , security and keeping
  the connection up through mobility control.  This module also acts as the
  access stratum's interface to the NAS layer.

  RRC is a single task architecture and consists of a set of co-operating state
  machines which communicate with each other, and with external modules, via
  messages with unique message ids (UMIDs). All the messages posted by RRC to
  modules outside of RRC is always through the message router. Internally
  almost all of the major modules and procedures of RRC are modeled as state
  machines. State machines inside of RRC can communicate either synchronously
  ( i.e without invoking a message router or a CS call) or asynchronously using
  the message router /CS call. Either way all the received external and internal
   UMIDs of RRC go through a common RRC dispatcher which has the knowledge of
   the recipient state machines for the various UMIds.

  <h2>High level architecture</h2>
  <img src='../pics/rrc_arch.jpg'>

  <h2>API</h2>
 <h3><i>Message Interfaces</i></h3>
   -- External API to RRC is defined in @ref nb1_rrc_ext_msg.h

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

$Header: //components/rel/nb1.mpss/2.2/api/nb1_rrc_ext_msg.h#1 $

when       who       what, where, why
--------   ---       ----------------------------------------------------------

===========================================================================*/

#ifndef NB1_RRC_EXT_MSG_H
#define NB1_RRC_EXT_MSG_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include <lte_rrc_ext_msg.h>
#include <msgr_nb1.h>
#include "nb1_as.h"
/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

typedef lte_rrc_log_ota_msg_s nb1_rrc_log_ota_msg_s;
#endif /* NB1_RRC_EXT_MSG_H */

