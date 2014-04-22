/*  $NetBSD: mdreloc.c,v 1.25 2004/12/17 10:53:27 mycroft Exp $ */

/*==============================================================================
  Copyright (c) 2013 Qualcomm Technologies, Inc.
  All rights reserved. Qualcomm Proprietary and Confidential.
==============================================================================*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <string.h>

#include "debug.h"
#include "rtld.h"

void _rtld_bind_start(void);
void _rtld_relocate_nonplt_self(Elf_Dyn*, Elf_Addr);
caddr_t _rtld_bind(const Obj_Entry*, Elf_Word);

void
_rtld_setup_pltgot(const Obj_Entry* obj)
{
  obj->pltgot[2] = (Elf_Addr)obj;
  obj->pltgot[1] = (Elf_Addr) & _rtld_bind_start;
}

void
_rtld_relocate_nonplt_self(Elf_Dyn* dynp, Elf_Addr relocbase)
{
  const Elf_Rela* rela = 0, *relalim;
  Elf_Addr relasz = 0;
  Elf_Addr* where;

  for (; dynp->d_tag != DT_NULL; dynp++) {
    switch(dynp->d_tag) {
      case DT_RELA:
        rela = (const Elf_Rela*)(relocbase + dynp->d_un.d_ptr);
        break;
      case DT_RELASZ:
        relasz = dynp->d_un.d_val;
        break;
      default:
        break;
    }
  }
  if (0 == rela) {
    // TODO
    return;
  }

  relalim = (const Elf_Rela*)((caddr_t)rela + relasz);

  for (; rela < relalim; rela++) {
    where = (Elf_Addr*)(relocbase + rela->r_offset);
    *where += (Elf_Addr)relocbase;
  }
}

int
_rtld_relocate_nonplt_objects(const Obj_Entry* obj)
{
  const Elf_Rela* rela;

  for (rela = obj->rela; rela < obj->relalim; rela++) {
    Elf_Addr* where;
    const Elf_Sym* def;
    const Obj_Entry* defobj;
    unsigned long    symnum;

    where = (Elf_Addr*)(obj->relocbase + rela->r_offset);
    symnum = ELF_R_SYM(rela->r_info);

    switch(ELF_R_TYPE(rela->r_info)) {
      case R_TYPE(NONE):
        break;

      case R_TYPE(32):    /* word32 S + A */
        def = _rtld_find_symdef(symnum, obj, &defobj, false);
        if (def == NULL) {
          return -1;
        }

        *where = (Elf_Addr)defobj->relocbase + def->st_value + rela->r_addend;
        break;

      case R_TYPE(GLOB_DAT):      /* word32 S + A in GOT */
        def = _rtld_find_symdef(symnum, obj, &defobj, false);
        if (def == NULL) {
          return -1;
        }

        *where = (Elf_Addr)defobj->relocbase + def->st_value + rela->r_addend;
        break;

      case R_TYPE(JMP_SLOT):  /* word32 S + A */
        def = _rtld_find_symdef(symnum, obj, &defobj, false);
        if (def == NULL) {
          return -1;
        }
        *where = (Elf_Addr)defobj->relocbase + def->st_value + rela->r_addend;

        break;

      case R_TYPE(RELATIVE):  /* word32 B + A */
        *where = (Elf_Addr)obj->relocbase + rela->r_addend;
        break;

      case R_TYPE(COPY):
        /*
         * These are deferred until all other relocations have
         * been done.  All we do here is make sure that the
         * COPY relocation is not in a shared library.  They
         * are allowed only in executable files.
         */
        if (obj->isdynamic) {
          DLERROR("Unexpected R_COPY relocation in %s", obj->path);
          return -1;
        }
        break;

      default:
        DLERROR("unsupported reloc %ld in non-PLT reloc %s",
                (unsigned long)ELF_R_TYPE(rela->r_info), obj->path);
        return -1;
    }
  }

  return 0;
}

int
_rtld_relocate_plt_lazy(const Obj_Entry* obj)
{
  const Elf_Rela* rela = (const Elf_Rela*)((caddr_t)obj->pltrela);
  Elf_Word* where;

  if (rela == NULL) {
    return 0;
  }

  where = (Elf_Word*)(obj->relocbase + rela->r_offset);

  if (!(ELF_R_TYPE(rela->r_info) == R_TYPE(JMP_SLOT))) {
    DLERROR("Bad JMP_SLOT in r_info");
    return 0;
  }
  if (!(ELF_R_SYM(rela->r_info) != 0)) {
    DLERROR("NULL r_info info");
    return 0;
  }

  for (; rela < obj->pltrelalim; rela++) {
    where = (Elf_Word*)(obj->relocbase + rela->r_offset + rela->r_addend);
    *where += (Elf_Addr)obj->relocbase;
  }

  return 0;
}

caddr_t
_rtld_bind(const Obj_Entry* obj, Elf_Word reloff)
{
  const Elf_Rela* rela = (const Elf_Rela*)(obj->pltrela + reloff);
  Elf_Addr* where = (Elf_Addr*)(obj->relocbase + rela->r_offset);
  Elf_Addr new_value;
  const Elf_Sym* def;

  const Obj_Entry* defobj;

  if (!(ELF_R_TYPE(rela->r_info) == R_TYPE(JMP_SLOT))) {
    // this will result in a jump to 0x0
    DLERROR("Bad JMP_SLOT in r_info");
    return 0;
  }

  def = _rtld_find_symdef(ELF_R_SYM(rela->r_info), obj, &defobj, true);
  if (def == NULL) {
    return 0;
  }

  new_value = (Elf_Addr)(defobj->relocbase + def->st_value + rela->r_addend);

  *where = new_value;

  return (caddr_t)new_value;
}

int
_rtld_relocate_plt_objects(const Obj_Entry* obj)
{
  const Elf_Rela* rela;
  int retval = 0;

  for (rela = obj->pltrela; rela < obj->pltrelalim; rela++) {
    Elf_Addr* where = (Elf_Addr*)(obj->relocbase + rela->r_offset);
    Elf_Addr target;
    const Elf_Sym* def;
    const Obj_Entry* defobj;

    if (!(ELF_R_TYPE(rela->r_info) == R_TYPE(JMP_SLOT))) {
      // this will result in a jump to 0x0
      DLERROR("Bad JMP_SLOT in r_info");
      return 0;
    }

    def = _rtld_find_symdef(ELF_R_SYM(rela->r_info), obj, &defobj, true);
    if (def == NULL) {
      retval = -1;
    } else {
      target = (Elf_Addr)(defobj->relocbase + def->st_value + rela->r_addend);
      *where = target;
    }
  }

  return retval;
}
