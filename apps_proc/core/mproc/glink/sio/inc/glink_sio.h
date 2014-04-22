#ifndef GLINK_SIO_H
#define GLINK_SIO_H
/*===========================================================================

                    GLINK SIO Interface Header File

 Copyright (c) 2005-2014,2018 by Qualcomm Technologies, Inc.
 All Rights
 Reserved.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE



when       who     what, where, why
--------   ---     ----------------------------------------------------------
14/08/29   db     initial version 

===========================================================================*/

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "sio.h"

/* this disables sio_wrapper from registering glink_sio */
#define FEATURE_GLINK_WRAPPER_FREE


/*===========================================================================
                      PUBLIC FUNCTION DECLARATIONS
===========================================================================*/

/*===========================================================================
FUNCTION      GLINK_SIO_DRIVER_INIT

DESCRIPTION   Initializes the GLINK SIO device.
  
DEPENDENCIES  Should be called just once.

ARGUMENTS     sio_register_device_driver_cb_type register_driver_fn 
               The registration function passed by SIO

RETURN VALUE  Always 1
   
SIDE EFFECTS  None
===========================================================================*/
uint32 glink_sio_driver_init
(
  sio_register_device_driver_cb_type register_driver_fn
);

#endif /* GLINK_SIO_H */
