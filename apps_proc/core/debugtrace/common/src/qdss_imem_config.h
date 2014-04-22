#ifndef QDSS_IMEM_CONFIG_H
#define QDSS_IMEM_CONFIG_H

/*=============================================================================

FILE:         qdss_imem_config.h

DESCRIPTION:  

================================================================================
              Copyright (c) 2013, 2019 QUALCOMM Technologies Incorporated.
                         All Rights Reserved.
                QUALCOMM Proprietary and Confidential
$ Header: $
  when       who      what, where, why
--------   -------    ----------------------------------------------------------
02/23/19     kdey     Changes for IMEM address and SNOC BIMC clocks
================================================================================*/


#define QDSS_IMEM_BASE_BEAR_DEFAULT    0x08600000
#define QDSS_IMEM_BASE_TWIZY_DEFAULT   0x08628000
#define QDSS_IMEM_BASE_BADGER_DEFAULT  0xFE805000
#define QDSS_IMEM_BASE_BADGER_MSGRAM   0xFC42B000

#define QDSS_IMEM_BASE_9x25_V2         0xFE807800
#define QDSS_IMEM_BASE_9x25_V1         0xFC42A800 


#define QDSS_IMEM_QDSS_OFFSET  0x728  // control location
#define QDSS_IMEM_SIZE          12

#endif //QDSS_IMEM_CONFIG_H
