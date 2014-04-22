#ifndef WCD_SHARED_CHANNEL_MANAGER_INTF_
#define WCD_SHARED_CHANNEL_MANAGER_INTF_

/**
@file shared_channel_manager.h
@brief 
Exported functions of WCD Shared Channel Manager
*/
/*-------------------------------------------------------------------------------
     Copyright (c) 2011 Qualcomm Technologies Incorporated.
	 All rights reserved.
	 Qualcomm Confidential and Proprietary
-----------------------------------------------------------------------------*/
/*=============================================================================
                             Edit History
$Header: //source/qcom/qct/multimedia2/Audio/drivers/hw/Adie/AdieCodec/
			Common3/main/latest/src/shared_channel_manager.h $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
12/15/10	SK		Initial revision
===============================================================================*/

#include "wcd_result.h"
#include "wcd_types.h"

/** 
 * \brief Configures slimbus shared channels.
 * Steps: For multichannel figures out multi channel mapping
 *        For all channels: Write multichannel mapping and watermark to registers
 *                          Gets a handle for all slave ports
 *                          Allocated shared data channel
 *                          Connects the port to the channel
 *        Applies configuration to all data channels at once
 *        Configures data channels
 *
 * @param[Usecase] : Usecase associated with the client state for book keeping
 *                        when closing data channels
 * @param[slimbus_cmd]  : Slimbus data channel configuration from ACDB
 *
 */
wcd_result wcd_configure_shared_channel(wcd_usecase *usecase, wcd_ioctl_cmd_slimbus_intf_config* slimbus_cmd);

/** 
 * \brief Closes any shared channels
 * Deactivates all the shared channels and deallocates them.
 *
 * @param[Usecase] : Usecase to remove shared channel mappings from
 *
 */
wcd_result wcd_close_shared_channel(wcd_usecase *usecase);

/** 
 * \brief Configures slimbus shared channels in the internal codec speaker protection path 
 *        Similar to wcd_configure_shared_channel
 *
 * @param[v_shared_channel_num] : The shared channel number of the voltage across the speaker
 * @param[i_shared_channel_num] : The shared channel number of the current through the speaker
 *
 */
wcd_result wcd_configure_spkr_prot_shared_channel(uint8 v_shared_channel_num, uint8 i_shared_channel_num);

/** 
 * \brief Closes speaker protection shared channels
 * Deactivates all the shared channels and deallocates them.
 *
 *
 */
wcd_result wcd_close_spkr_prot_shared_channel(void);

#endif
