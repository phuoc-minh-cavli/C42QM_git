#ifndef UIM_RXTX_H
#define UIM_RXTX_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  UIM RX/TX HEADER

GENERAL DESCRIPTION
  This header defines the external interface for the uim_rxtx module.

EXTERNALIZED FUNCTIONS

  uim_tx
    Sends outgoing bytes to the UIM HW interface.

Copyright (c) 2001-2014 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Header: //components/rel/uim.mpss/3.3.1/uimdrv/src/uim_rxtx.h#1 $
$DateTime: 2020/07/22 10:53:27 $
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/20/14   ak      Created

===========================================================================*/

/**
*  This function sends outgoing bytes to the UIM interface.
*
*  
*  @param buffer A pointer to an array of bytes to send.
*  @param size The number of bytes to send
*  @param uim_ptr Pointer to the current instance of the UIM
*                 global structure 
*/
void uim_tx(unsigned char *buffer,
            int size,
            uim_instance_global_type *uim_ptr);

#endif  /* UIM_RXTX_H */
