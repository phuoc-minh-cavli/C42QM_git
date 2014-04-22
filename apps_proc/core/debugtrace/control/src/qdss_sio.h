#ifndef QDSS_SIO_H
#define QDSS_SIO_H

/*=============================================================================

FILE:         qdss_sio.h

DESCRIPTION:  

================================================================================
              Copyright (c) 2013 Qualcomm Technologies Incorporated.
                         All Rights Reserved.
                QUALCOMM Proprietary and Confidential
==============================================================================*/
#include "DALStdDef.h"
#include "sio.h"

int qdss_sio_open(sio_port_id_type port_id, uint8 mode);
int qdss_sio_close(void) ;
int qdss_sio_send_data(uint32 data_size); 



#endif //QDSS_SIO_H
