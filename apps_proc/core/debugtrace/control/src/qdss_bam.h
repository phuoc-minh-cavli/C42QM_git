#ifndef QDSS_BAM_H
#define QDSS_BAM_H

/*=============================================================================

FILE:         qdss_bam.h

DESCRIPTION:  

================================================================================
              Copyright (c) 2011 Qualcomm Technologies Incorporated.
                         All Rights Reserved.
                QUALCOMM Proprietary and Confidential
==============================================================================*/
#include "bam.h"

int qdss_bam_init(void);
int qdss_bam_pipe_setup(bam_pipe_config_type *qdss_pipe_config_ptr,
                        bam_handle *qdss_pipe_handle_ptr,
                        uint32 *qdss_pipe_num_ptr);

#endif //QDSS_BAM_H
