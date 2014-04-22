#ifndef _CSD_ERROR_MAP_H_
#define _CSD_ERROR_MAP_H_

/*! \file csd_error_map.h
    \brief CSD module error mapping

    Copyright (C) 2012 Qualcomm Technologies Incorporated.
    All Rights Reserved.
    Qualcomm Proprietary and Confidential.
*/

/*===========================================================================
                      EDIT HISTORY FOR MODULE

This section contains comments describing changes made to this module.
Notice that changes are listed in reverse chronological order.

$Header: 
$DateTime:  
$Author: 
$Change:  
$Revision:  

when           who      what, where, why
--------       ----     ----------------------------------------------------------
08/07/13       vk       creation
============================================================================*/

#include "mmdefs.h"
#include "acdb.h"
#include "csd_status.h"
#include "apr_errcodes.h"

/*
 * FUNCTION: csd_map_apr_error_to_csd

 * PARAMETERS: apr status

 * CONTEXT: any context

 * SIDE EFFECTS: none.

 * RETURN: csd status
*/
int32_t csd_map_apr_error_to_csd( uint32_t apr_rc );

/*
 * FUNCTION: csd_map_acdb_error_to_csd

 * PARAMETERS: acdb status

 * CONTEXT: any context

 * SIDE EFFECTS: none.

 * RETURN: csd status
*/
int32_t csd_map_acdb_error_to_csd( int32_t acdb_error );

#endif
