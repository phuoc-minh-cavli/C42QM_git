/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  GNSS Message Router Header

GENERAL DESCRIPTION
  
This module contains GNSS Message Router related definitions
  

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
 

  Copyright (c) 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved.

  Copyright (c) 2016 Qualcomm Technologies, Inc.
  Qualcomm Technologies Confidential and Proprietary. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/sm/common/inc/gnss_msgr.h#1 $
  $DateTime: 2020/07/21 21:23:44 $
  $Author: pwbldsvc $
  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/19/16   kpn  Remove gnss_msgr_send_msg() as it is not being used anywhere.
03/29/16   kpn  Modify interface functions for subscription ID 
04/06/12   rh   Initial creation of file.
======================================================================*/

#ifndef _GNSS_MSGR_H_
#define _GNSS_MSGR_H_

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "msgr.h"
#include "msgr_umid.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/


/*===========================================================================

                EXTERNAL FUNCTIONS (for other GNSS modules)

===========================================================================*/

/*=========================================================================
FUNCTION       gnss_msgr_register_nas_msgs
 
DESCRIPTION    This function registers a message that GNSS is interested in 
               collecting from the message router
 
DEPENDENCIES   GNSS MSGR client setup with the MSGR must have been completed.
 
RETURN VALUE   Boolean: TRUE for success, FALSE for failure
 
SIDE EFFECTS   None 
 
===========================================================================*/
boolean gnss_msgr_register_nas_msgs(void);

/*===========================================================================
FUNCTION      gnss_msgr_deregister_nas_msgs

DESCRIPTION   Deregister NAS's MSGR messages
 
RETURN VALUE  Boolean: TRUE for success, FALSE for failure

SIDE EFFECTS  None 
 
=============================================================================*/
boolean gnss_msgr_deregister_nas_msgs(void);

/*===========================================================================
FUNCTION       GNSS_MSGR_SEND_NAS_MSG

DESCRIPTION    Send MSGR message to NAS
 
RETURN VALUE   Boolean: TRUE for success, FALSE for failure

SIDE EFFECTS   None
=============================================================================*/
boolean gnss_msgr_send_nas_msg(
  uint8 *pkt_ptr, 
  uint32 pkt_size, 
  uint32 nas_id,
  uint16 trans_id,
  uint8 inst_id
);

/*===========================================================================
FUNCTION       GNSS_MSGR_RECEIVE_NAS_MSG

DESCRIPTION    Receive NAS MSGR message from Queue
 
RETURN VALUE   Boolean: TRUE for success, FALSE for failure

SIDE EFFECTS   None
=============================================================================*/
boolean gnss_msgr_receive_nas_msg(
  uint8 *pkt_buf, 
  uint32 buf_size, 
  uint32 *pkt_size,
  uint32 *nas_id, 
  uint8 *inst_id
);


#endif /* _GNSS_MSGR_H_ */

