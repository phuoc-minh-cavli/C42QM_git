#ifndef HALQDSS_REPLICATOR_H
#define HALQDSS_REPLICATOR_H
/*=============================================================================

FILE:         halqdss_replicator.h

DESCRIPTION:  

================================================================================
                Copyright 2011 Qualcomm Technologies Incorporated.
                         All Rights Reserved.
                QUALCOMM Proprietary and Confidential
================================================================================
  $Header: //components/rel/core.tx/6.0/debugtrace/replicator/hal/inc/halqdss_replicator.h#1 $
==============================================================================*/
#include "HALcomdef.h"

void HAL_qdss_replicator_HalConfigInit(uint32 nBaseAddr);
void HAL_qdss_replicator_DisableTPIU(void);
void HAL_qdss_replicator_EnableTPIU(void);
void  HAL_qdss_replicator_AccessUnLock(void);
void  HAL_qdss_replicator_AccessLock(void);


#endif //HALQDSS_REPLICATOR_H
