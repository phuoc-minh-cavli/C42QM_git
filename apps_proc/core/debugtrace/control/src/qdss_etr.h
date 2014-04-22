#ifndef QDSS_ETR_H
#define QDSS_ETR_H

/*=============================================================================

FILE:         qdss_etr.h

DESCRIPTION:  

================================================================================
              Copyright (c) 2013 Qualcomm Technologies Incorporated.
                         All Rights Reserved.
                QUALCOMM Proprietary and Confidential
==============================================================================*/
#include "DALStdDef.h"
int qdss_etr_configure_buffer(uint32 buf_base_phys_addr, uint32 buf_size_in_bytes); 
int qdss_etr_start_timer(void);
int qdss_etr_stop_timer(void);

int qdss_etr_init_workloop(void);
int qdss_etr_register_isr(void);

#endif //QDSS_ETR_H
