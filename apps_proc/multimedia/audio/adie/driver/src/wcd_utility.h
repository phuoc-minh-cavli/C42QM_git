#ifndef WCD_UTILITY_INTF_
#define WCD_UTILITY_INTF_

/**
@file wcd_utility.h
@brief 
External Utility functions for WCD.
*/
/*-------------------------------------------------------------------------------
     Copyright (c) 2011 Qualcomm Technologies Incorporated.
	 All rights reserved.
	 Qualcomm Confidential and Proprietary
-----------------------------------------------------------------------------*/
/*=============================================================================
                             Edit History
$Id: //components/rel/audio.tx/2.0/audio/adie/driver/src/wcd_utility.h#2 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
12/15/10	  SK		Initial revision
===============================================================================*/

#include "wcd_intf.h"

/**
 * \brief False boolean for error checking.
 *
 */
extern wcd_bool WCD_G_FALSE_BOOLEAN;

/*****************************************************************************
 * WCD 93xx linked list
*****************************************************************************/

/**
 * \brief This function creates and initializes a list
 *
 */
wcd_result wcd_create_and_initialize_list (void **p_list_handle);

/**
 * \brief This function adds a new element with the data to the top of the list
 *
 */
wcd_result wcd_push_to_list (wcd_list *p_list_handle, void *data);

/**
 * \brief This function adds a new element with the data to the bottom of the list
 *
 */
wcd_result wcd_add_to_list (wcd_list *p_list_handle, void *data);

/**
 * \brief This function removes the element with the data from the passed in list
 * 
 * @note This does not free the data within the element, it is the callee's
 * responsibility to free the data after calling this function.
 *
 */
wcd_result wcd_remove_from_list (wcd_list *p_list_handle, void *data);

/**
 * \brief This function finds the element containing the passed in data.
 * If the data is not found, or if the data or list is invalid
 * NULL is returned.
 *
 */
wcd_list_element *wcd_find_element_inside_list (wcd_list *p_list_handle,
                                                   void *data);

#endif
