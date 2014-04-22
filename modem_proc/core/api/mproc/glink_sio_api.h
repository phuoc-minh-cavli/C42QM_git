#ifndef _GLINK_SIO_H_
#define _GLINK_SIO_H_
/*===========================================================================

                GLink SIO Interface Public Header File

     Copyright (c) 2014-2016,2018 QUALCOMM Technologies Incorporated.
     All rights reserved.
     Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.mpss/3.10/api/mproc/glink_sio_api.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/08/16   ap      Expose API to get SIO port ID given GLink port name.
===========================================================================*/

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "sio.h"

/*===========================================================================
FUNCTION      glink_sio_get_sio_id_from_name

DESCRIPTION   Get SIO port ID for a given GLink port name
  
DEPENDENCIES  None

ARGUMENTS     const char * glink_port_name

RETURN VALUE  SIO port ID for the given GLink port name
              SIO_PORT_NULL : Error
   
SIDE EFFECTS  None
===========================================================================*/
sio_port_id_type glink_sio_get_sio_id_from_name
(
  const char * glink_port_name
);

/*===========================================================================
FUNCTION      glink_sio_get_data_port_name

DESCRIPTION   Get the GLink data port name given the SIO port ID
  
DEPENDENCIES  None

ARGUMENTS     SIO port ID

RETURN VALUE  
              const char * glink_port_name
   
SIDE EFFECTS  None
===========================================================================*/
const char *glink_sio_get_data_port_name
(
  sio_port_id_type pid
);

/*===========================================================================
FUNCTION      glink_sio_get_cntl_port_name

DESCRIPTION   Get the GLink control port name given the SIO port ID
  
DEPENDENCIES  None

ARGUMENTS     SIO port ID

RETURN VALUE  
              const char * glink_port_name
   
SIDE EFFECTS  None
===========================================================================*/
const char *glink_sio_get_cntl_port_name
(
  sio_port_id_type pid
);

#endif
