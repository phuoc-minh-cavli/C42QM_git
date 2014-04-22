#ifndef RR_LAI_H
#define RR_LAI_H
/*============================================================================
 $Header: //components/rel/geran.mpss/5.2.0/api/rr_lai.h#1 $$DateTime: 2018/02/08 01:15:27 $$Author: pwbldsvc $

 @file rr_lai.h

 Header file containing the definition of the LAI_T type

 Copyright (c) 2008-2013 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Qualcomm Confidential and Proprietary
============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 *--------------------------------------------------------------------------*/
#ifndef COMDEF_H
  #include "comdef.h"
#endif

#include "sys_plmn.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 *--------------------------------------------------------------------------*/

// represents an LAI
typedef struct
{
  PLMN_id_T plmn_id;
  word      location_area_code;
} LAI_T;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 *--------------------------------------------------------------------------*/


#endif /* #ifndef RR_LAI_H */

/* EOF */

