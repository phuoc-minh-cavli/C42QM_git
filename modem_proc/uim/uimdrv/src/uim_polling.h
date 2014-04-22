#ifndef UIM_POLLING_H
#define UIM_POLLING_H
/*==============================================================================
  FILE:         uim_polling.h

  OVERVIEW:     FIle conatins the inclusions and definitions needed for
                polling functions.

  DEPENDENCIES: N/A

                Copyright (c) 2014-2015, 2020 QUALCOMM Technologies, Inc.
                All Rights Reserved.
                QUALCOMM Technologies Confidential and Proprietary
==============================================================================*/

/*=============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

$Header: //components/rel/uim.mpss/3.3.1/uimdrv/src/uim_polling.h#1 $
$DateTime: 2020/07/22 10:53:27 $
$Author: pwbldsvc $

when        who        what, where, why
------      ----       ---------------------------------------------------------
01/24/20   ykr         Data exchange check in uim_power_control_ds
11/05/15   ks          Rel12 polling enhancement
==============================================================================*/

#include "uimglobals.h"
#include "gstk_exp.h"
#include "estk.h"
#include "ds_sys_event.h"
#include "ds_sys_ioctl.h"


/* Type for log mask used for apdu logging*/
typedef enum
{
  UIM_POLLING_NONE,
  UIM_ICC_CARD_POLLING, /* For ICC cards, STATUS command expects 22 bytes of response data */
  UIM_UICC_PROACTIVE_POLLING,/* For UICC cards, proactive polling expects no return data */
  UIM_UICC_PRESENCE_DETECTION_POLLING /* For UICC cards, detection polling expects rsp data based on directory */
}uim_polling_type;


/*===========================================================================

FUNCTION UIM_PROCESS_POLLING

DESCRIPTION
  This procedure decides on the return type of the POLL command
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void uim_process_polling
(
  rex_sigs_type            *mask,
  uim_instance_global_type *uim_ptr
);

/*===========================================================================

FUNCTION UIM_IS_PS_CALL_DATA_EXCHANGE_HAPPENING

DESCRIPTION
  This procedure calls the DS IOCTL and indicates if there is a data exchange happening

DEPENDENCIES
  None

RETURN VALUE
  Boolean - TRUE/FALSE based on whether data exchange is happening or not

SIDE EFFECTS
  None

===========================================================================*/
boolean uim_is_ps_call_data_exchange_happening
(
  uim_instance_global_type *uim_ptr
);
 /* UIM_IS_PS_CALL_DATA_EXCHANGE_HAPPENING */
 
#endif /* UIM_POLLING_H */
