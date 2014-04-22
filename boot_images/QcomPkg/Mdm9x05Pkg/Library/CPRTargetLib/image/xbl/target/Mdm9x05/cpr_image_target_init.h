/**
 * @file:  cpr_image_target_init.h
 * @brief:
 *
 * Copyright (c) 2015 by Qualcomm Technologies Incorporated. All Rights Reserved.
 *
 * $DateTime: 2018/08/08 05:31:55 $
 * $Header: //components/rel/boot.xf/0.2/QcomPkg/Mdm9x05Pkg/Library/CPRTargetLib/image/xbl/target/Mdm9x05/cpr_image_target_init.h#1 $
 * $Change: 16834360 $
 */
#ifndef CPR_IMAGE_TARGET_INIT_H
#define CPR_IMAGE_TARGET_INIT_H

//******************************************************************************
// Default Enablement Structures
//******************************************************************************
struct cpr_cfg_funcs;

typedef enum
{
    CPR_INIT_NONE, /* used to disable CPR */
    CPR_INIT_OPEN_LOOP,
    CPR_INIT_SW_CLOSED_LOOP,

    CPR_INIT_FUNCS_COUNT
} cpr_init_funcs_id;

struct cpr_cfg_funcs* cpr_image_target_get_init_func(cpr_init_funcs_id func_id);

#endif

