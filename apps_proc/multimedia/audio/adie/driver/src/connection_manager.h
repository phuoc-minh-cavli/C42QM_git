#ifndef WCD_CONNECTION_MANAGER_INTF_
#define WCD_CONNECTION_MANAGER_INTF_

/**
@file wcd_connection_manager.h
@brief 
External connection manager functions for WCD.
*/
/*-------------------------------------------------------------------------------
     Copyright (c) 2011 Qualcomm Technologies Incorporated.
    All rights reserved.
    Qualcomm Confidential and Proprietary
-----------------------------------------------------------------------------*/
/*=============================================================================
                        Edit History
$Id: //components/rel/audio.tx/2.0/audio/adie/driver/src/connection_manager.h#2 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
12/15/10   SK      Initial revision
===============================================================================*/

#include "wcd_intf.h"

/** 
 * \brief This function frees all the internal members inside the connection_info
 * Also frees the connection info. 
 * 
 * \Note Do not call this function on a connection that is connected...bad things
 * will happen.
 */
wcd_result wcd_clear_connection(wcd_connection_info* p_handle);

/**
 * \brief This function validates the source/sink/connection type and manages all the
 * routes that are needed for the particular connection.
 *
 */
wcd_result wcd_connect(wcd_usecase* p_usecase, wcd_connection_info* p_connection, wcd_source* source, wcd_sink* sink, wcd_connection_type connection_type);

/**
 * \brief This function disconnects all elements in an existing connection.
 *
 */
wcd_result wcd_disconnect(wcd_usecase* p_usecase, wcd_connection_info* p_connection);

/**
 * \brief Executes a ACDB command on a list of elements. Looking for that element type
 * (from the command) and executing that element's event.
 */
wcd_result
wcd_execute_command_on_elements(wcd_list* element_list, wcd_acdb_command* command, 
                                wcd_element_table* element_table, wcd_event_context_pointer* context_ptr);

#endif


