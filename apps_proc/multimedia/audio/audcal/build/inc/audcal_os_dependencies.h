#ifndef _AUDCAL_OS_DEPENDENCIES_H_
#define _AUDCAL_OS_DEPENDENCIES_H_
/*===========================================================================
                          AUDCAL   W r a p p e r 
               f o r   W32/ Simulator OS dependencies

  @file CadOsDependencies.h
  This file contains OS dependencies for CAD. Each platform (OS) will 
  have its own copy of this file with their platform (OS) specific 
  dependencies/ code

Copyright (c) 2008 - 2010 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
========================================================================*/

/*===========================================================================
                             Edit History

$Header:

when       who         what, where, why
--------   ---         -------------------------------------------------------
06/30/10   ayin        update with new build
05/26/10   ayin        initial draft

============================================================================*/
//#include "wce_pmem.h"
//#include <crtdbg.h>
//#include "DALRemote.h" // ard.c
//#include "mmdefs.h"

#define PMEM_TYPE_ID  0x00000001

extern void* pmem_get_phys_addr(void* addr);

extern void* pmem_malloc(size_t sz, unsigned int sid);

extern void pmem_free(void *ptr);

#define AUDCAL_PRINTF_1(format) \
    RETAILMSG(1, (TEXT(format)))

#endif /* _AUDCAL_OS_DEPENDENCIES_H_ */
