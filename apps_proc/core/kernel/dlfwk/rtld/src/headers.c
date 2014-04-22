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

/*  $NetBSD: headers.c,v 1.20 2005/01/09 12:43:23 simonb Exp $   */

/*
 * Copyright 1996 John D. Polstra.
 * Copyright 1996 Matt Thomas <matt@3am-software.com>
 * Copyright 2002 Charles M. Hannum <root@ihack.net>
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
#include <dirent.h>

#include "debug.h"
#include "rtld.h"

/*
 * Process a shared object's DYNAMIC section, and save the important
 * information in its Obj_Entry structure.
 */
int
_rtld_digest_dynamic(Obj_Entry* obj)
{
  Elf_Dyn* dynp;
  Needed_Entry** needed_tail = &obj->needed;
  const Elf_Dyn* dyn_rpath = NULL;
  Elf_Sword   plttype = DT_NULL;
  Elf_Addr        relsz = 0, relasz = 0;
  Elf_Addr    pltrel = 0, pltrelsz = 0;
  Elf_Addr    init = 0, fini = 0;

  for (dynp = obj->dynamic; dynp->d_tag != DT_NULL; ++dynp) {
    switch(dynp->d_tag) {

      case DT_REL:
        obj->rel = (const Elf_Rel*)
           (obj->relocbase + dynp->d_un.d_ptr);
        break;

      case DT_RELSZ:
        relsz = dynp->d_un.d_val;
        break;

      case DT_RELENT:
        if (!(dynp->d_un.d_val == sizeof(Elf_Rel))) DLERROR("Bad DT_RELENT size");
        break;

      case DT_JMPREL:
        pltrel = dynp->d_un.d_ptr;
        break;

      case DT_PLTRELSZ:
        pltrelsz = dynp->d_un.d_val;
        break;

      case DT_RELA:
        obj->rela = (const Elf_Rela*)
           (obj->relocbase + dynp->d_un.d_ptr);
        break;

      case DT_RELASZ:
        relasz = dynp->d_un.d_val;
        break;

      case DT_RELAENT:
        if (!(dynp->d_un.d_val == sizeof(Elf_Rela))) DLERROR("Bad DT_RELAENT size");
        break;

      case DT_PLTREL:
        plttype = dynp->d_un.d_val;
        if (!(plttype == DT_REL || plttype == DT_RELA)) DLERROR("Bad DT_PLTREL value");
        break;

      case DT_SYMTAB:
        obj->symtab = (const Elf_Sym*)
           (obj->relocbase + dynp->d_un.d_ptr);
        break;

      case DT_SYMENT:
        if (!(dynp->d_un.d_val == sizeof(Elf_Sym))) DLERROR("Bad DT_SYMENT value");
        break;

      case DT_STRTAB:
        obj->strtab = (const char*)
           (obj->relocbase + dynp->d_un.d_ptr);
        break;

      case DT_STRSZ:
        obj->strsize = dynp->d_un.d_val;
        break;

      case DT_HASH:
      {
        const Elf_Word* hashtab = (const Elf_Word*)
           (obj->relocbase + dynp->d_un.d_ptr);

        obj->nbuckets = hashtab[0];
        obj->nchains = hashtab[1];
        obj->buckets = hashtab + 2;
        obj->chains = obj->buckets + obj->nbuckets;
        break;
      }
        
      case DT_NEEDED:
      {
        Needed_Entry* nep = malloc(sizeof(Needed_Entry));
        if (0 == nep) {
          DLERROR("failed to malloc (0x%X B)", sizeof(Needed_Entry));
          return -1;
        }
        nep->name = dynp->d_un.d_val;
        nep->obj = NULL;
        nep->next = NULL;

        *needed_tail = nep;
        needed_tail = &nep->next;
        break;
      }

      case DT_PLTGOT:
        obj->pltgot = (Elf_Addr*)
           (obj->relocbase + dynp->d_un.d_ptr);
        break;

      case DT_TEXTREL:
        obj->textrel = true;
        break;

      case DT_SYMBOLIC:
        obj->symbolic = true;
        break;

      case DT_RPATH:
        /*
         * We have to wait until later to process this, because
         * we might not have gotten the address of the string
         * table yet.
        */
        dyn_rpath = dynp;
        break;

      case DT_SONAME:
        /* Not used by the dynamic linker. */
        break;

      case DT_INIT:
        init = dynp->d_un.d_ptr;
        break;

      case DT_FINI:
        fini = dynp->d_un.d_ptr;
        break;

      case DT_DEBUG:
        dynp->d_un.d_ptr = (Elf_Addr) & _rtld_debug;
        break;
    }
  }

  obj->rellim = (const Elf_Rel*)((caddr_t)obj->rel + relsz);
  obj->relalim = (const Elf_Rela*)((caddr_t)obj->rela + relasz);

  if (plttype == DT_REL) {
    obj->pltrel = (const Elf_Rel*)(obj->relocbase + pltrel);
    obj->pltrellim = (const Elf_Rel*)(obj->relocbase + pltrel + pltrelsz);
    obj->pltrelalim = 0;

    /* On PPC and SPARC, at least, REL(A)SZ may include JMPREL.
       Trim rel(a)lim to save time later. */
    if (obj->rellim && obj->pltrel &&
        obj->rellim > obj->pltrel &&
        obj->rellim <= obj->pltrellim) {
      obj->rellim = obj->pltrel;
    }
  } else if (plttype == DT_RELA) {
    obj->pltrela = (const Elf_Rela*)(obj->relocbase + pltrel);
    obj->pltrellim = 0;
    obj->pltrelalim = (const Elf_Rela*)(obj->relocbase + pltrel + pltrelsz);

    /* On PPC and SPARC, at least, REL(A)SZ may include JMPREL.
       Trim rel(a)lim to save time later. */
    if (obj->relalim && obj->pltrela &&
        obj->relalim > obj->pltrela &&
        obj->relalim <= obj->pltrelalim) {
      obj->relalim = obj->pltrela;
    }
  }

  if (init != 0) {
    obj->init = (void (*)(void))(obj->relocbase + init);
  }

  if (fini != 0) {
    obj->fini = (void (*)(void))(obj->relocbase + fini);
  }

  if (dyn_rpath != NULL) {
    _rtld_add_paths(&obj->rpaths, obj->strtab +
                    dyn_rpath->d_un.d_val);
  }

  return 0;
}

