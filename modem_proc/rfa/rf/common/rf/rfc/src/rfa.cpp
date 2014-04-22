/*!
   @file
   rfa.cpp

   @brief
   This file contains implementation the RFA class.
*/

/*===========================================================================

Copyright (c) 2011-12 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2020/04/28 01:53:09 $ $Author: mplcsds1 $
$Header: //components/rel/rfa.mpss/3.10/rf/common/rf/rfc/src/rfa.cpp#2 $

when       who     what, where, why
------------------------------------------------------------------------------- 
04/24/20   mp      Clade heap changes
05/09/12   sr      Initial version.

============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "rfa.h"
#include "rfcommon_msg.h"
#include "modem_mem.h"
#ifdef FEATURE_CLADE_HEAP
#include "clade_heap.h"
#endif

// Constructor 
rfa::rfa() 
{

}

// Destructor
rfa::~rfa()
{

}

void* rfa::operator new(size_t size)
{
#ifndef FEATURE_CLADE_HEAP
  return modem_mem_alloc(size,MODEM_MEM_CLIENT_RFA);
#else
  return clade_heap_alloc(size);
#endif

}

void rfa::operator delete(void* ptr)
{
#ifndef FEATURE_CLADE_HEAP
  modem_mem_free(ptr,MODEM_MEM_CLIENT_RFA);
#else
 clade_heap_free(ptr);
#endif
}
