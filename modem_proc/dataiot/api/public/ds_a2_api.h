#ifndef DS_A2_API_H
#define DS_A2_API_H

/*===========================================================================

                          DS_A2_API . H

DESCRIPTION
  Header file for declaration of functions exposed to A2

EXTERNALIZED FUNCTIONS
  DS_IS_PKT_MO_EXCEPTION()
    Determines the PKT is MO Exception or not

Copyright (c) 2020 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
#include "sio.h"
#include "dsm.h"


/*===========================================================================
FUNCTION DS_IS_PKT_MO_EXCEPTION()

DESCRIPTION
  This function determines whether the packet MO-Exception or not

DEPENDENCIES
  None

RETURN VALUE
  TRUE for a successful match
  FALSE for no match

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_is_pkt_mo_exception
(
  sio_stream_id_type      stream_id,
  dsm_item_type           *rx_pkt
);

#endif
