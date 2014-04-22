#ifndef QDSS_USB_BAM_H
#define QDSS_USB_BAM_H

/*=============================================================================

FILE:         qdss_usb_bam.h

DESCRIPTION:  

================================================================================
            Copyright (c) 2013 Qualcomm Technologies, Inc.
                         All Rights Reserved.
          Qualcomm Technologies Proprietary and Confidential
==============================================================================*/
#include "bam.h"

/*-------------------------------------------------------------------------*/

/**
  @brief Provide information for configuring QDSS end of QDSS-USB bam pipe.
  Also retrieve information about QDSS BAM pipe for configuring the USB end.
  
  This function is called by USB driver during sio_open of the QDSS device.
  It configures the QDSS end of the BAM pipe and returns the required 
  information for configuring USB end.

  
  @param  qdss_pipe_config_ptr [in  ] :  This structure provides USB bam 
                                         parameters for QDSS use
  @param qdss_pipe_handle_ptr  [out ] :  On success the qdss bam pipe 
                                         is returned here
  @param    qdss_pipe_num_ptr  [out ] :  On success the QDSS bam pipe number 
                                         to use is returned here.

  @return 0 if successful, error code otherwise
 */
int qdss_per_request_pipe(bam_pipe_config_type *qdss_pipe_config_ptr,
                          bam_handle *qdss_pipe_handle_ptr,
                          uint32 *qdss_pipe_num_ptr);

/*-------------------------------------------------------------------------*/

/**
  @brief Disable QDSS bam pipe

  @return 0 if successful, error code otherwise
 */
int qdss_bam_pipe_disable(void); 

/*-------------------------------------------------------------------------*/

/**
  @brief Enable QDSS bam pipe

  @return 0 if successful, error code otherwise
 */
int qdss_bam_pipe_enable(void);

#endif //QDSS_USB_BAM_H
