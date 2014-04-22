/****************************************************************************
 * Copyright (c) 2013 Qualcomm Technologies, Inc. All Rights Reserved.
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
 * 2013-10-29   rpoddar     Initial revision.
 *
 *****************************************************************************/

#include "cti_config.h"

const struct CTIPhysAddrMap cti_phys_addr_array[] = {
    {"cti0",        0xFC308000},
    {"cti1",        0xFC309000},
    {"cti2",        0xFC30A000},
    {"cti3",        0xFC30B000},
    {"cti4",        0xFC30C000},
    {"cti5",        0xFC30D000},
    {"cti6",        0xFC30E000},
    {"cti7",        0xFC30F000},
    {"cti8",        0xFC310000},
    {"cpu0",        0xFC333000},
    {"modem-cpu0",  0xFC350000},
    {"audio-cpu0",  0xFC354000},
    {"rpm-cpu0",    0xFC358000}
};

const struct CTIPhysAddrMapLen cti_phys_addr_array_len[] = {
    { sizeof(cti_phys_addr_array) / sizeof(struct CTIPhysAddrMap) }
};
 
