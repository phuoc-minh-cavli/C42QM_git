#ifndef WCD_ROUTE_MANAGER_INTF_
#define WCD_ROUTE_MANAGER_INTF_

/**
@file route_manager.h
@brief 
External route manager functions for WCD.
*/
/*-------------------------------------------------------------------------------
     Copyright (c) 2011 Qualcomm Technologies Incorporated.
    All rights reserved.
    Qualcomm Confidential and Proprietary
-----------------------------------------------------------------------------*/
/*=============================================================================
                        Edit History
$Id: //components/rel/audio.tx/2.0/audio/adie/driver/src/route_manager.h#2 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
12/15/10   SK      Initial revision
===============================================================================*/

#include "wcd_intf.h"

/**
 * \brief Creates and resets the element table.
 */
wcd_result create_element_table(wcd_element_table** p_element_table);

/**
 * \brief Adds the element pointer to the element table array
 */
wcd_result add_element_to_table(wcd_element* element, wcd_element_table* element_table);

/** 
 * \brief This function frees all the internal members inside the wcd_route
 * Also frees the route. 
 */
wcd_result wcd_clear_route(wcd_route* p_handle);

/**
 * \brief This function routes from the source to the sink, using the provided route stack.
 *
 */
wcd_result wcd_determine_route(wcd_route** p_handle, wcd_element* source, wcd_element* sink, uint32* route_stack);

/**
 * \brief This function routes the differential portion of the connection using the supplied
 * route stack.
 *
 */
wcd_result wcd_determine_differential_route(wcd_route* p_route, wcd_element* sink, uint32* route_stack);

#endif


