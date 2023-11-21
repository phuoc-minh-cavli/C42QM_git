#pragma once
/**
 * sns_fw_pwr_sync_com_port_service.h
 *
 * Framework header for Sensors Sync Com Port Service.
 *
 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * $Id: //components/rel/ssc.slpi/5.1/framework/inc/sns_fw_sync_com_port_service.h#1 $
 * $DateTime: 2019/04/24 05:02:39 $
 * $Change: 18986584 $
 *
 */

/** Forward Declaration. */
typedef struct sns_sync_com_port_service sns_sync_com_port_service;

/**
 * Initialize the sync com port service; to be used only by the
 * Service Manager.
 */
sns_sync_com_port_service* sns_sync_com_port_service_init(void);
