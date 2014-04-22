#ifndef _POLICYMAN_TECH_H_
#define _POLICYMAN_TECH_H_

/**
  @file policyman_tech.h

  @brief  Module to handle segment-loading of technologies.
*/

/*
    Copyright (c) 2013,2014 QUALCOMM Technologies Incorporated.
    All Rights Reserved.
    Qualcomm Technologies Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by this
  document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

  $Header: //components/rel/mmcp.mpss/6.1.10/policyman/inc/policyman_tech.h#1 $
  $DateTime: 2023/04/25 14:39:18 $
  $Author: pwbldsvc $
*/

#include "policyman_i.h"
#include "mcfg_seg_load.h"

#if defined FEATURE_DISABLE_DYNAMIC_POLICIES
/*-------- tech_info_t --------*/
/**
@brief  Structure that contains information about technologies that
        can be loaded/unloaded by segment loading.
*/
typedef struct
{
  char const      *pName;         /* name of the technology */
  clsid_t const    clsid;         /* Class ID of the technology */
  uint32 const     mask;          /* Tech mask of the technology */
  interface_t     *pInterface;    /* Pointer to interface for the technology */
} tech_info_t;
#endif

/*-------- policyman_tech_init --------*/
void
policyman_tech_init(
  void
  );

#if defined(TEST_FRAMEWORK)
#error code not present
#endif /* +TEST_FRAMEWORK. */

/*-------- policyman_tech_is_loaded --------*/
boolean
policyman_tech_is_loaded(
  clsid_t   clsid
  );

#if defined FEATURE_DISABLE_DYNAMIC_POLICIES
tech_info_t *
policyman_tech_find_info_by_name(
  char const  *pName
);
#endif
#endif /* _POLICYMAN_TECH_H_ */
