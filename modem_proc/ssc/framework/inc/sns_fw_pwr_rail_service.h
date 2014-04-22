#pragma once
/**
 * sns_fw_pwr_rail_service.h
 *
 * Framework header for Sensors Power Rail Service.
 *
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * $Id: //components/rel/ssc.slpi/5.1/framework/inc/sns_fw_pwr_rail_service.h#1 $
 * $DateTime: 2019/04/24 05:02:39 $
 * $Change: 18986584 $
 *
 */

/** Forward Declaration. */
typedef struct sns_fw_pwr_rail_service sns_fw_pwr_rail_service;

/**
 * Initialize the power rail service; to be used only by the
 * Service Manager.
 */
sns_fw_pwr_rail_service* sns_pwr_rail_service_init(void);
