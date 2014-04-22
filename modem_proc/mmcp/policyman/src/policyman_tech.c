/**
  @file policyman_tech.c

  @brief  Module to handle segment-loading of technologies.
*/

/*
    Copyright (c) 2013-2015 QUALCOMM Technologies Incorporated.
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

  $Header: //components/rel/mmcp.mpss/6.1.10/policyman/src/policyman_tech.c#1 $
  $DateTime: 2023/04/25 14:39:18 $
  $Author: pwbldsvc $
*/

#include "policyman_tech.h"
#include "policyman_phone_events.h"
#include "policyman_rat_capability.h"
#include "policyman_rules.h"
#include "policyman_task.h"
#include "policyman_util.h"
#include "policyman_xml.h"


#if !defined(FEATURE_SEGMENT_LOADING) && !defined(TEST_FRAMEWORK)

#define  MCFG_SEGMENT_LOAD_ALL        0x00000000
#define  MCFG_SEGMENT_EXCLUDE_WCDMA   0x00000001
#define  MCFG_SEGMENT_EXCLUDE_TDSCDMA 0x00000002

IxErrnoType
create_instance(
  clsid_t     clsid,
  interface_t **ppIface
  )
{
  return E_FAILURE;
}

int
mcfg_segment_exclude_technologies(
  uint32  excluded_techs
  )
{
  return 0;
}

#endif


#ifndef FEATURE_DISABLE_DYNAMIC_POLICIES
/*-------- tech_info_t --------*/
/**
@brief  Structure that contains information about technologies that
        can be loaded/unloaded by segment loading.
*/
typedef struct
{
  char const      *pName;         /* name of the technology */
  clsid_t const   clsid;          /* Class ID of the technology */
  uint32 const    mask;           /* Tech mask of the technology */
  interface_t     *pInterface;    /* Pointer to interface for the technology */
} tech_info_t;

#endif
/*-------- tech_info --------*/
/**
@brief  Technologies currently handled by segment loading
*/
static tech_info_t  tech_info[] =
{
  {"WCDMA",     CLSID_WCDMA,    MCFG_SEGMENT_EXCLUDE_WCDMA,   NULL},
  {"TDSCDMA",   CLSID_TDSCDMA,  MCFG_SEGMENT_EXCLUDE_TDSCDMA, NULL},
};



/*=============================================================================
  Utility functions
=============================================================================*/


/*-------- policyman_tech_find_info_by_clsid --------*/
/**
@brief  Return a pointer to the information for a given technology

@param[in]  clsid   Class ID of the technology

@return
  Pointer to the technology information, or NULL if not found.
*/
static tech_info_t *
policyman_tech_find_info_by_clsid(
  clsid_t   clsid
  )
{
  size_t  i;

  for (i = 0 ; i < ARR_SIZE(tech_info) ; ++i)
  {
    if (tech_info[i].clsid == clsid)
    {
      return &tech_info[i];
    }
  }

  return NULL;
}


/*-------- policyman_tech_find_info_by_name --------*/
/**
@brief  Return a pointer to the information for a given technology

@param[in]  pName   Name of the technology

@return
  Pointer to the technology information, or NULL if not found.
*/
tech_info_t *
policyman_tech_find_info_by_name(
  char const  *pName
  )
{
  size_t  i;

  for (i = 0 ; i < ARR_SIZE(tech_info) ; ++i)
  {
    if (strcmp(pName, tech_info[i].pName) == 0)
    {
      return &tech_info[i];
    }
  }

  return NULL;
}


/*-------- policyman_tech_is_loaded --------*/
boolean
policyman_tech_is_loaded(
  clsid_t   clsid
  )
{
  tech_info_t *pInfo = policyman_tech_find_info_by_clsid(clsid);

#if defined(FEATURE_SEGMENT_LOADING)
  return pInfo != NULL && pInfo->pInterface != NULL;
#else
  return pInfo == NULL || pInfo != NULL;  /* TRUE, but uses pInfo so no compile error */
#endif
}



/*=============================================================================
  Initialization/Deinitilization of the module.
=============================================================================*/


/*-------- policyman_tech_init --------*/
void
policyman_tech_init(
  void
  )
{
  size_t          i;
  IxErrnoType     result;

  for (i = 0 ; i < ARR_SIZE(tech_info) ; ++i)
  {
    result = create_instance(tech_info[i].clsid, &tech_info[i].pInterface);
    if (result != E_SUCCESS)
    {
      tech_info[i].pInterface = NULL;
    }
  }
}

#if defined(TEST_FRAMEWORK)
#error code not present
#endif /* +TEST_FRAMEWORK. */

