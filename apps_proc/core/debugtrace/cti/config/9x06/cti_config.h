#ifndef CTI_CONFIG_H
#define CTI_CONFIG_H

/****************************************************************************
 * Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
 *
 * FILE:            core/debugtrace/cti/config/9x25/cti_config.h
 *
 * DESCRIPTION:     CTI Configuration Structures.
 *
 *
 * EDIT HISTORY FOR MODULE
 * --------------------------------------------------------------------------
 *
 * when         who         what, where, why
 * ----------   -------     -----------------
 * 2014-01-24   rpoddar     Initial revision.
 *
 *****************************************************************************/

#include "com_dtypes.h"
#include "qdss_constants.h"

struct CTIPhysAddrMap
{
    char    cti_name[CTI_BLOCK_SIZE];
    uint32  phys_addr;
};

struct CTIPhysAddrMapLen
{
    uint32 len;
};

#endif /* CTI_CONFIG_H */
