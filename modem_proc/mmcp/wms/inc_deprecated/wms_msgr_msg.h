#ifndef WMS_MSGR_MSG_H
#define WMS_MSGR_MSG_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
           W I R E L E S S    M E S S A G I N G   S E R V I C E S
           wms_msgr_msg.h -- MSGR definitions

  This file contains definitions for the MSGR services used by WMS.


  -------------

    Copyright (c) 2010-2013 QUALCOMM Technologies Incorporated.
    All Rights Reserved.
    Qualcomm Confidential and Proprietary

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

  $Header: //components/rel/mmcp.mpss/6.1.10/wms/inc_deprecated/wms_msgr_msg.h#1 $
  $DateTime: 2023/04/25 14:39:18 $
  $Author: pwbldsvc $
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include "comdef.h"
#include <msgr.h>
#include <msgr_umid.h>


/*==============================================================================
                           Module definitions
==============================================================================*/

#define MSGR_MODULE_CM_WMS  0x01

#define MSGR_WMS_CM            MSGR_TECH_MODULE( MSGR_TECH_WMS, MSGR_MODULE_CM_WMS )

#define MSGR_MODULE_POLICYMAN_WMS  0x02
#define MSGR_WMS_POLICYMAN     MSGR_TECH_MODULE( MSGR_TECH_WMS, MSGR_MODULE_POLICYMAN_WMS )

#define MSGR_MODULE_LTE_WMS        0x03
#define MSGR_WMS_LTE           MSGR_TECH_MODULE( MSGR_TECH_WMS, MSGR_MODULE_LTE_WMS )

#define MSGR_MODULE_WMS_DS_3GPP    0x04
#define MSGR_WMS_DS_3GPP       MSGR_TECH_MODULE(MSGR_TECH_WMS,MSGR_MODULE_WMS_DS_3GPP)


/*==============================================================================
                           Msg UMID definitions
==============================================================================*/

#endif /* WMS_MSGR_MSG_H */
