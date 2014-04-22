/*!
  @file
  mcs_wrappers.h

  @brief
  MCS wrappers header file.

*/

/*===========================================================================

  Copyright (c) 2015 Qualcomm Technologies Incorporated.
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

===========================================================================*/

/*===========================================================================

$Header: //components/rel/mcs.mpss/5.1/api/mcs_wrappers.h#1 $

===========================================================================*/

#ifndef MCS_WRAPPERS_H
#define MCS_WRAPPERS_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <comdef.h>
#include <rcevt.h>
#include <amssassert.h>

/*===========================================================================

                         MACRO DEFINITIONS

===========================================================================*/


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/


/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

void mcs_rcevt_signal_name
(
  RCEVT_NAME const name  
);

void mcs_rcevt_wait_name
(
  RCEVT_NAME const name  
);

void mcs_rcevt_register_sigex_name
(
  RCEVT_NAME const name,        
  RCEVT_SIGEX_TYPE const type,  
  RCEVT_SIGEX const sigex       
);

void mcs_rcevt_unregister_sigex_name
(
  RCEVT_NAME const name,        
  RCEVT_SIGEX_TYPE const type,  
  RCEVT_SIGEX const sigex       
);

#endif /* MCS_WRAPPERS_H */
