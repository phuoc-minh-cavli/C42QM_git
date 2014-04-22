/*==============================================================================

Dynamic Loading Support Libs

Copyright (c) 2014 by QUALCOMM Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR

--------------------------------------------------------------------------------
 EDIT HISTORY FOR FILE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.

 when               who         what, where, why
 --------           ---         -----------------------------------------------
 1/22/15          amitkulk      created 
==============================================================================*/


/*  $NetBSD: reloc.c,v 1.87 2005/01/09 14:58:15 martin Exp $     */

/*
 * Copyright 1996 John D. Polstra.
 * Copyright 1996 Matt Thomas <matt@3am-software.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by John Polstra.
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Dynamic linker for ELF.
 *
 * John Polstra <jdp@polstra.com>.
 */



#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>

#include "debug.h"
#include "rtld.h"
#include "sys_mman.h"

/*
 * Relocate newly-loaded shared objects.  The argument is a pointer to
 * the Obj_Entry for the first such object.  All objects from the first
 * to the end of the list of objects are relocated.  Returns 0 on success,
 * or -1 on failure.
 */
int
_rtld_relocate_objects(Obj_Entry* first, bool bind_now)
{
  Obj_Entry* obj;
  int ok = 1;

  for (obj = first; obj != NULL; obj = obj->next) {
    if (obj->nbuckets == 0 || obj->nchains == 0 ||
        obj->buckets == NULL || obj->symtab == NULL ||
        obj->strtab == NULL) {
      DLERROR("shared object has no run-time symbol table %s", obj->path);
      return -1;
    }

    if (obj->textrel) {
      /* there are relocations to the write-protected text segment */
      FARF(HIGH, "unprotect [%p - %p] rw. (0x%X bytes)",
           obj->textbase, obj->textbase + obj->textsize, obj->textsize);
      if (sys_mprotect(obj->textbase, obj->textsize, PROT_READ | PROT_WRITE) == -1) 
	  {
        DLERROR("Cannot unprotect text segment %s", obj->path);
        return -1;
      }
    }

    if (_rtld_relocate_nonplt_objects(obj) < 0) {
      ok = 0;
    }

    if (obj->textrel) {
      /* re-protect the text segment */
      FARF(HIGH, "protect [%p - %p] r.x (0x%X bytes)",
           obj->textbase, obj->textbase + obj->textsize, obj->textsize);
      if (sys_mprotect(obj->textbase, obj->textsize,
                   PROT_READ | PROT_EXEC) == -1) {
        DLERROR("Cannot protect text segment %s", obj->path);
        return -1;
      }
    }

    if (_rtld_relocate_plt_lazy(obj) < 0) {
      ok = 0;
    }

    if (bind_now) {
      if (_rtld_relocate_plt_objects(obj) < 0) {
        ok = 0;
      }
    }

    if (!ok) {
      return -1;
    }

    /* Set some sanity-checking numbers in the Obj_Entry. */
    obj->magic = RTLD_MAGIC;
    obj->version = RTLD_VERSION;

    /* Fill in the dynamic linker entry points. */
    obj->dlopen = dlopen;
    obj->dlsym = dlsym;
    obj->dlerror = dlerror;
    obj->dlclose = dlclose;
    obj->dladdr = dladdr;

    /* Set the special PLTGOT entries. */
    if (obj->pltgot != NULL) {
      _rtld_setup_pltgot(obj);
    }
  }

  return 0;
}
