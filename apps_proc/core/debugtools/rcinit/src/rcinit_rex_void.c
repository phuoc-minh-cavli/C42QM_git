/** vi: tw=128 ts=3 sw=3 et :
@file rcinit_rex_void.c
@brief This file contains the API for the Run Control Framework, API 2.1
*/
/*=============================================================================
NOTE: The @brief description above does not appear in the PDF.
The tms_mainpage.dox file contains the group/module descriptions that
are displayed in the output PDF generated using Doxygen and LaTeX. To
edit or update any of the group/module text in the PDF, edit the
tms_mainpage.dox file or contact Tech Pubs.
===============================================================================*/
/*=============================================================================
Copyright (c) 2015 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
===============================================================================*/
/*=============================================================================
Edit History
$Header: //components/rel/core.tx/6.0/debugtools/rcinit/src/rcinit_rex_void.c#1 $
$DateTime: 2020/01/30 22:49:35 $
$Change: 22400605 $
$Author: pwbldsvc $
===============================================================================*/

// TODO: Remove after rex.h is also removed
//typedef unsigned int rex_tcb_type;

#include "tms_rcinit.h"

rex_tcb_type* rcinit_lookup_rextask_info(RCINIT_INFO info)
{
   ERR_FATAL("no implementation", 0, 0, 0);
   return RCINIT_NULL;
}

rex_tcb_type* rcinit_lookup_rextask(RCINIT_NAME name)
{
   ERR_FATAL("no implementation", 0, 0, 0);
   return RCINIT_NULL;
}

RCINIT_GROUP rcinit_lookup_group_rextask(rex_tcb_type* tid)
{
   ERR_FATAL("no implementation", 0, 0, 0);
   return RCINIT_GROUP_NONE;
}

RCINIT_INFO rcinit_lookup_info_rextask(rex_tcb_type* tid)
{
   ERR_FATAL("no implementation", 0, 0, 0);
   return RCINIT_NULL;
}

void rcinit_internal_start_rextask(const rcinit_info_t* rcinit_p, void* entry)
{
   ERR_FATAL("no implementation", 0, 0, 0);
}
