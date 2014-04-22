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
    {"cti0",        0xFC310000},
    {"cti1",        0xFC311000},
    {"cti2",        0xFC312000},
    {"cti3",        0xFC313000},
    {"cti4",        0xFC314000},
    {"cti5",        0xFC315000},
    {"cti6",        0xFC316000},
    {"cti7",        0xFC317000},
    {"cti8",        0xFC318000},
    {"cpu0",        0xFC343000},
    {"modem-cpu0",  0xFC338000},
    {"audio-cpu0",  0xFC33C000},
    {"rpm-cpu0",    0xFC360000}
};

const struct CTIPhysAddrMapLen cti_phys_addr_array_len[] = {
    { sizeof(cti_phys_addr_array) / sizeof(struct CTIPhysAddrMap) }
};
 
