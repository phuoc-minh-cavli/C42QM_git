#ifndef WCD_USCASE_MANAGER_INTF_
#define WCD_USCASE_MANAGER_INTF_

/**
@file usecase_manager.h
@brief 
External Utility functions for WCD usecase manager.
*/
/*-------------------------------------------------------------------------------
     Copyright (c) 2011 Qualcomm Technologies Incorporated.
	 All rights reserved.
	 Qualcomm Confidential and Proprietary
-----------------------------------------------------------------------------*/
/*=============================================================================
                             Edit History
$Id: //components/rel/audio.tx/2.0/audio/adie/driver/src/usecase_manager.h#2 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
12/15/10	  SK		  Initial revision
===============================================================================*/

#include "wcd_intf.h"

/******************************************************************************
 **************                 USECASE PROFILES                ***************
 *****************************************************************************/

/**
 * \brief The mask in parameter_one that has the command type
 */
#define WCD_COMMAND_TYPE_MASK             0xF0000000

/**
 * \brief The command types.
 */
#define WCD_COMMAND_TYPE_TRIGGER          0x10000000
#define WCD_COMMAND_TYPE_END_OF_ENABLE    0x20000000
#define WCD_COMMAND_TYPE_END_OF_DISABLE   0x30000000
#define WCD_COMMAND_TYPE_DELAY            0x40000000
#define WCD_COMMAND_TYPE_BEGIN_CHAN_CONFIG  0x50000000
#define WCD_COMMAND_TYPE_END_CHAN_CONFIG  0x60000000

/*
 *\brief The mask of the parameter (time in MS) for delay.
 */
#define WCD_DELAY_COMMAND_PARAMETER_MASK  0xFFFFFFFF

/**
 *\brief The mask of the channel to configure.
 */
#define WCD_BEGIN_CHAN_CONFIG_PARAMETER_MASK  0x0000000F



/** 
 * \brief Initialize the global usecase list.
 *
 */
wcd_result wcd_initialize_global_usecase_list(void);

/** 
 * \brief Deinitialize the global usecase list.
 *
 */
wcd_result wcd_deinitialize_global_usecase_list(void);

/** 
 * \brief Finds a usecase inside the global usecase list.
 *
 */
wcd_result wcd_find_usecase(wcd_usecase* p_handle);

/**
 * \brief Enables the usecase by executing all the commands in the profile on all
 * connections (differential included).
 */
wcd_result wcd_enable_usecase(wcd_usecase* p_handle);

/**
 * \brief Disables the usecase by executing all the remaining commands in the profile on all
 * connections (differential included).
 */
wcd_result wcd_disable_usecase(wcd_usecase* p_handle);

/**
 * \brief Verifies the ACDB profile. Checks for END OF ENABLE and END OF DISABLE.
 * Also verifies the version of the profile is compatible with the driver.
 */
wcd_result wcd_verify_acdb_profile(uint32 command_count, wcd_acdb_command* commands, uint32 version);

/**
 * \brief Enables the ANC input devices using the reserved elements + ANC command list
 * inside of the usecase
 */
wcd_result wcd_enable_anc_profile(wcd_usecase* usecase);

/**
 * \brief Disables the ANC input devices using the reserved elements + ANC command list
 * inside of the usecase
 */
wcd_result wcd_disable_anc_profile(wcd_usecase* usecase);

#endif
