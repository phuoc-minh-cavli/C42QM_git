#ifndef _CSD_DEV_MBHC_H_
#define _CSD_DEV_MBHC_H_
/**
    @file       csd_dev_mbhc.h
    @brief      MBHC common header
*/
/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The CSD_mainpage.dox file contains all file/group descriptions that are 
      in the output PDF generated using Doxygen and LaTeX. To edit or update 
      any of the file/group text in the PDF, edit the CSD_mainpage.dox file or 
      contact Tech Pubs.
===========================================================================*/
/*-----------------------------------------------------------------------------
     Copyright (C) 2013 Qualcomm Technologies Incorporated.
     All rights reserved.
     Qualcomm Confidential and Proprietary.
-----------------------------------------------------------------------------*/
/*=============================================================================
                      EDIT HISTORY FOR MODULE

    This section contains comments describing changes made to this module.
    Notice that changes are listed in reverse chronological order.

    $Header: //components/rel/audio.tx/2.0/audio/csd/dev/target/inc/csd_dev_mbhc.h#1 $
    $DateTime: 2018/12/25 22:39:07 $
    $Author: pwbldsvc $
    $Change: 17929171 $
    $Revision: #1 $

=============================================================================*/

#include "csd_dev_ioctl.h"

uint32_t csd_dev_control_device_mbhc(struct csd_dev_mbhc_config* mbhc_ctrl);

int32_t csd_dev_mbhc_init(void);

int32_t csd_dev_mbhc_deinit(void);

#endif /*_CSD_DEV_MBHC_H_*/

