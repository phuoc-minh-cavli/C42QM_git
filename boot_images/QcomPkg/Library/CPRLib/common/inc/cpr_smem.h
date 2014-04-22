/**
 * @file:  cpr_smem.h
 * @brief:
 *
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * QUALCOMM Proprietary and Confidential.
 *
 * $DateTime: 2018/08/01 23:51:05 $
 * $Header: //components/rel/boot.xf/0.2/QcomPkg/Library/CPRLib/common/inc/cpr_smem.h#1 $
 * $Change: 16788488 $
 */
#ifndef CPR_SMEM_H
#define CPR_SMEM_H

#include "cpr_cfg.h"
#include "cpr_data.h"

void cpr_smem_deserialize_config(cpr_rail* rail, cpr_rail_state* state);
void cpr_smem_serialize_config(cpr_rail_state* state, boolean append);

#endif
