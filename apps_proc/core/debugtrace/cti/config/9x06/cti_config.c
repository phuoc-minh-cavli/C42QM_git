/****************************************************************************
 * Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
 *
 * FILE:            core/debugtrace/cti/config/9x25/cti_config.c
 *
 * DESCRIPTION:     Device specific configuration for CTI.
 *
 *                  Specifically contains CTI block -> physical address
 *                  mappings.
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

#include "cti_config.h"

const struct CTIPhysAddrMap cti_phys_addr_array[] = {
    {"cti0",        0x06010000},
    {"cti1",        0x06011000},
    {"cti2",        0x06012000},
    {"cti3",        0x06013000},
    {"cti4",        0x06014000},
    {"cti5",        0x06015000},
    {"cti6",        0x06016000},
    {"cti7",        0x06017000},
    {"cti8",        0x06018000},
    {"cpu0",        0x06043000},
    {"modem-cpu0",  0x06038000},
    {"rpm-cpu0",    0x0603C000}
};

const struct CTIPhysAddrMapLen cti_phys_addr_array_len[] = {
    { sizeof(cti_phys_addr_array) / sizeof(struct CTIPhysAddrMap) }
};
 
