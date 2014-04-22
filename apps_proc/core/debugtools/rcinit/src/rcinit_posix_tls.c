/** vi: tw=128 ts=3 sw=3 et :
@file rcinit_posix_tls.c
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
$Header: //components/rel/core.tx/6.0/debugtools/rcinit/src/rcinit_posix_tls.c#1 $
$DateTime: 2020/01/30 22:49:35 $
$Change: 22400605 $
$Author: pwbldsvc $
===============================================================================*/

#include "rcinit_posix.h"
#include "tms_rcinit.h"

int rcinit_internal_tls_create_key(int* x, void (*y)(void*))
{
   return pthread_key_create((pthread_key_t*)x, y);
}

int rcinit_internal_tls_set_specific(int x, void* y)
{
   return pthread_setspecific((pthread_key_t)x, y);
}

void* rcinit_internal_tls_get_specific(int x)
{
   return pthread_getspecific((pthread_key_t)x);
}
