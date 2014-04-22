#ifndef __UXDEBUGNEW_H__
#define __UXDEBUGNEW_H__
/*===========================================================================

               U X D E B U G N E W   H E A D E R   F I L E

FILE:  UxDebugNew.h

DESCRIPTION
        This header file has new and delete operator overide 
        functions for debugging.

INITIALIZATION AND SEQUENCING REQUIREMENTS
NA

 (c) COPYRIGHT <2005 - 2011> Qualcomm Technologies Incorporated. All Rights Reserved. QUALCOMM Proprietary/GTDR
===========================================================================

EDIT HISTORY FOR FILE
 $Header: //components/rel/core.mpss/3.10/securemsm/smecom/ixutil/shared/inc/UxDebugNew.h#1 $ 
 $DateTime: 2018/06/26 03:18:17 $  
 $Author: pwbldsvc $

=========================================================================*/ 

#ifdef __UXMALLOC_DEBUG__
void * operator new(size_t size, const char *file, unsigned long line);
void * operator new[](size_t size, const char *file, unsigned long line);

#define new DEBUG_NEW
#define DEBUG_NEW new(__FILE__,__LINE__)

void operator delete(void *p);
void operator delete[](void *p);
#endif

#endif // __UXDEBUGNEW_H__
