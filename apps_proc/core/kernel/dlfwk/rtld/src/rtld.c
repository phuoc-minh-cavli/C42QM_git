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

/*  $NetBSD: rtld.c,v 1.107.2.1 2006/02/01 20:38:06 tron Exp $   */

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


#include "sys_mman.h"
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <ctype.h>
#include <dlfcn.h>

#include "debug.h"
#include "rtld.h"

#include <xmtx.h>

#define _RTLD_MIN(a,b)   ((a)<(b)?(a):(b))

/*
 * Data declarations.
 */
static char* error_message;  /* Message for dlopen(), or NULL */


struct r_debug _rtld_debug;    /* for GDB; */
bool _rtld_trust;    /* False for setuid and setgid programs */
Obj_Entry* _rtld_objlist;  /* Head of linked list of shared objects */
Obj_Entry** _rtld_objtail;  /* Link field of last object in list */
Obj_Entry* _rtld_objmain;  /* The main program shared object */
Obj_Entry _rtld_objself;  /* The dynamic linker shared object */
Elf_Sym _rtld_sym_zero; /* For resolving undefined weak refs. */
int _rtld_pagesz;   /* Page size, as provided by kernel */
DL_vtbl _rtld_vtbl;

int dlmutex = 0;

Search_Path* _rtld_default_paths;
Search_Path* _rtld_paths;

extern Elf_Addr _GLOBAL_OFFSET_TABLE_[];
extern Elf_Dyn  _DYNAMIC;

static
void _rtld_static_init(char* progname)
{
  /* Conjure up an Obj_Entry structure for the dynamic linker. */
  _rtld_objself.path = "_rtld_anonymous";
  _rtld_objself.pathlen = strlen(_rtld_objself.path);
  _rtld_objself.rtld = true;
  _rtld_objself.mapbase = 0;
  _rtld_objself.relocbase = 0;
  _rtld_objself.dynamic = (Elf_Dyn*)&_DYNAMIC;

  (void)_rtld_digest_dynamic(&_rtld_objself);
  _rtld_relocate_plt_objects(&_rtld_objself);
  _rtld_add_paths(&_rtld_default_paths, RTLD_DEFAULT_LIBRARY_PATH);

  _rtld_debug.r_brk = _rtld_debug_state;
  _rtld_debug.r_state = RT_CONSISTENT;
  _rtld_pagesz = 0x1000;

  _rtld_objmain = &_rtld_objself;
  _rtld_objmain->mainprog = true;

  _rtld_objtail = &_rtld_objlist;
  *_rtld_objtail = _rtld_objmain;
  _rtld_objtail = &_rtld_objmain->next;

  _rtld_linkmap_add(_rtld_objmain);

  ++_rtld_objmain->refcount;
  _rtld_objmain->mainref = 1;

  _rtld_objlist_add(&_rtld_list_main, _rtld_objmain);

  /* Initialize a fake symbol for resolving undefined weak references. */
  _rtld_sym_zero.st_info = ELF_ST_INFO(STB_GLOBAL, STT_NOTYPE);
  _rtld_sym_zero.st_shndx = SHN_ABS;

  _rtld_objlist = _rtld_objmain;

  _rtld_debug_state();    /* say hello to gdb! */
}

static Obj_Entry*
_rtld_dlcheck(void* handle)
{
  Obj_Entry* obj;

  for (obj = _rtld_objlist; obj != NULL; obj = obj->next) {
    if (obj == (Obj_Entry*)handle) {
      break;
    }
  }

  if (obj == NULL || obj->dl_refcount == 0) {
    DLERROR("invalid shared object handle %p", handle);
    return 0;
  }

  return obj;
}

static void
_rtld_init_dag1(Obj_Entry* root, Obj_Entry* obj)
{
  const Needed_Entry* needed;

  if (!obj->mainref) {
    if (_rtld_objlist_find(&obj->dldags, root)) {
      return;
    }
    // TODO these could fail, if they do a symbol lookup will prob fail
    // but, we should prob fail directly here and report malloc failure
    // instead of symbol lookup failure
    _rtld_objlist_add(&obj->dldags, root);
    _rtld_objlist_add(&root->dagmembers, obj);
  }

  for (needed = obj->needed; needed != NULL; needed = needed->next) {
    if (needed->obj != NULL) {
      _rtld_init_dag1(root, needed->obj);
    }
  }
}

static void
_rtld_init_dag(Obj_Entry* root)
{
  _rtld_init_dag1(root, root);
}

static void
_rtld_unref_dag(Obj_Entry* root)
{
  if (root->refcount != 0) {
    --root->refcount;
    if (root->refcount == 0) {
      const Needed_Entry* needed;

      for (needed = root->needed; needed != NULL;
           needed = needed->next) {
        if (needed->obj != 0) {
          _rtld_unref_dag(needed->obj);
        }
      }
    }
  }
}

static void
_rtld_call_init_functions(Obj_Entry* first)
{
  if (first != NULL) {
    _rtld_call_init_functions(first->next);

    if (first->init != NULL) {
      (*first->init)();
    }
  }
}

/*
 * Note, this is called only for objects loaded by dlopen().
 */
static void
_rtld_unload_object(Obj_Entry* root, bool do_fini_funcs)
{
  if (!(root)) {
    DLERROR("invalid root passed to unload object");
    return;
  }

  _rtld_unref_dag(root);

  if (root->refcount == 0) {    /* We are finished with some objects. */
    Obj_Entry* obj;
    Obj_Entry** linkp;
    Objlist_Entry* elm;

    /* finalize objects that are about to be unmapped */
    if (do_fini_funcs) {
      for (obj = _rtld_objlist->next;  obj != NULL;  obj = obj->next) {
        if (obj->refcount == 0 && obj->fini != NULL) {
          (*obj->fini)();
        }
      }
    }

    /* remove the DAG from all objects' DAG lists */
    SIMPLEQ_FOREACH(elm, &root->dagmembers, link)
    _rtld_objlist_remove(&elm->obj->dldags, root);

    /* remove the DAG from the RTLD_GLOBAL list */
    if (root->globalref) {
      root->globalref = 0;
      _rtld_objlist_remove(&_rtld_list_global, root);
    }

    /* unmap all objects that are no longer referenced */
    linkp = &_rtld_objlist->next;
    while ((obj = *linkp) != NULL) {
      if (obj->refcount == 0) {

        FARF(HIGH, "unloading %s", obj->path);

        sys_munmap(obj->mapbase, obj->mapsize);
        _rtld_objlist_remove(&_rtld_list_global, obj);
        _rtld_linkmap_delete(obj);
        *linkp = obj->next;
        _rtld_obj_free(obj);
      } else {
        linkp = &obj->next;
      }
    }

    _rtld_objtail = linkp;
  }
}

__strong_alias(__dlclose, dlclose)
int
dlclose(void *handle)
{
  Obj_Entry* root;

  sys_Mtxlock((void*)&dlmutex);

  root = _rtld_dlcheck(handle);

  if (root == NULL) {
    DLERROR("corrupted root");
    return -1;
  }

  _rtld_debug.r_state = RT_DELETE;
  _rtld_debug_state();

  --root->dl_refcount;
  _rtld_unload_object(root, true);

  _rtld_debug.r_state = RT_CONSISTENT;
  _rtld_debug_state();

  sys_Mtxunlock((void*)&dlmutex);
  return 0;
}

__strong_alias(__dlerror, dlerror)
char *
dlerror(void)
{
  char* msg = error_message;

  error_message = NULL;
  return msg;
}

__strong_alias(__dldeinit, dldeinit)
void
dldeinit(void)
{

  _rtld_deinit_builtin();
  // TODO
  // _rtld_static_deinit();

  sys_Mtxdst((void*)&dlmutex);
}

__strong_alias(__dlinitex, dlinitex)
int
dlinitex(int libc, char* libv[], DL_vtbl* vtbl)
{
  static boolean _rtld_init_done = false;
  if (!_rtld_init_done) {
    _rtld_init_done = true;

    sys_Mtxinit((void*)&dlmutex);

    _rtld_static_init("_rtld_anonymous");
    if (1 != _rtld_init_builtin(libc, libv)) {
      return 0;
    }
  }

  if (vtbl) {
    memmove(&_rtld_vtbl, vtbl, _RTLD_MIN(vtbl->size, sizeof(_rtld_vtbl)));
  }
  return 1;
}

__strong_alias(__dlinit, dlinit)
int
dlinit(int libc, char *libv[])
{
  return dlinitex(libc, libv, 0);
}

__strong_alias(__dlopen, dlopen)
void*
dlopen(const char *name, int mode)
{
  int nErr = 0;
  Obj_Entry** old_obj_tail;
  Obj_Entry* obj;
  bool reloc_mode;

  sys_Mtxlock((void*)&dlmutex);

  old_obj_tail = _rtld_objtail;
  obj = 0;

  _rtld_debug.r_state = RT_ADD;
  _rtld_debug_state();

  VERIFYM(nErr, name != 0, "dlopen(0, ...) not supported");

  VERIFY(nErr, 0 != (obj = _rtld_load_library(name, _rtld_objmain, mode)));

  if (*old_obj_tail != 0) {
    /* we loaded something new */

    VERIFYM(nErr, *old_obj_tail == obj, "Obj_Entry corruption!");

    VERIFY(nErr, -1 != _rtld_load_needed_objects(obj, mode));
    _rtld_init_dag(obj);

    if(((mode & RTLD_NOW) == 2)) reloc_mode = true;
    else reloc_mode = false;

    VERIFY(nErr, -1 != _rtld_relocate_objects(obj, reloc_mode));
    _rtld_call_init_functions(obj);
  }

  _rtld_debug.r_state = RT_CONSISTENT;
  _rtld_debug_state();

  ++obj->dl_refcount;

  CATCH(nErr) {
    if (obj) {
      _rtld_unload_object(obj, false);
      obj = 0;
    }
  }

  sys_Mtxunlock((void*)&dlmutex);
  return obj;
}

static Obj_Entry*
_rtld_obj_from_addr(const void* addr)
{
  Obj_Entry* obj;

  for (obj = _rtld_objlist;  obj != NULL;  obj = obj->next) {
    if (addr < (void*)obj->mapbase) {
      continue;
    }

    if (addr < (void*)(obj->mapbase + obj->mapsize)) {
      return obj;
    }
  }

  return NULL;
}

__strong_alias(__dlsym, dlsym)
void *
dlsym(void *handle, const char *name)
{
  const Obj_Entry* obj;
  unsigned long hash;
  const Elf_Sym* def;
  const Obj_Entry* defobj;
  void* retaddr;

  hash = _rtld_elf_hash(name);
  def = NULL;
  defobj = NULL;

  switch((intptr_t)handle) {
    case(intptr_t)NULL:
    case(intptr_t)RTLD_NEXT:
    case(intptr_t)RTLD_DEFAULT:
    case(intptr_t)RTLD_SELF:
      retaddr = __builtin_return_address(0); /* __GNUC__ only */

      if ((obj = _rtld_obj_from_addr(retaddr)) == NULL) {
        DLERROR("cannot determine caller's shared object");
        return NULL;
      }

      switch((intptr_t)handle) {
        case(intptr_t)NULL:      /* just the caller's shared object */
          def = _rtld_symlook_obj(name, hash, obj, false);
          defobj = obj;
          break;

        case(intptr_t)RTLD_NEXT:    /* Objects after callers */
          obj = obj->next;
          /*FALLTHROUGH*/

        case(intptr_t)RTLD_SELF:    /* Caller included */
          for (; obj; obj = obj->next) {
            if ((def = _rtld_symlook_obj(name, hash, obj, false)) != NULL) {
              defobj = obj;
              break;
            }
          }
          break;

        case(intptr_t)RTLD_DEFAULT:
          def = _rtld_symlook_default(name, hash, obj, &defobj, false);
          break;

        default:
          abort();
      }
      break;

    default:
      if ((obj = _rtld_dlcheck(handle)) == NULL) {
        return NULL;
      }

      if (obj->mainprog) {
        /* Search main program and all libraries loaded by it */
        def = _rtld_symlook_list(name, hash, &_rtld_list_main, &defobj, false);
      } else {
        /*
         * XXX - This isn't correct.  The search should include
         * the whole DAG rooted at the given object.
         */
        def = _rtld_symlook_obj(name, hash, obj, false);
        defobj = obj;
      }
      break;
  }

  if (def != NULL) {
    return defobj->relocbase + def->st_value;
  }

  DLERROR("Undefined symbol \"%s\"", name);
  return NULL;
}

__strong_alias(__dladdr, dladdr)
int
dladdr(const void *addr, Dl_info *info)
{
  const Obj_Entry* obj;
  const Elf_Sym* def;
  void* symbol_addr;
  unsigned long symoffset;

  if ((addr == NULL) || (info == NULL)) {
    DLERROR("NULL shared object address or info");
    return 0;
  }

  obj = _rtld_obj_from_addr(addr);
  if (obj == NULL) {
    DLERROR("no shared object contains address");
    return 0;
  }

  info->dli_fname = obj->path;
  info->dli_fbase = obj->mapbase;
  info->dli_saddr = (void*)0;
  info->dli_sname = NULL;

  /*
   * Walk the symbol list looking for the symbol whose address is
   * closest to the address sent in.
   */

  for (symoffset = 0; symoffset < obj->nchains; symoffset++) {
    def = obj->symtab + symoffset;

    /*
     * For skip the symbol if st_shndx is either SHN_UNDEF or
     * SHN_COMMON.
     */
    if (def->st_shndx == SHN_UNDEF || def->st_shndx == SHN_COMMON) {
      continue;
    }

    /*
     * If the symbol is greater than the specified address, or if it
     * is further away from addr than the current nearest symbol,
     * then reject it.
     */
    symbol_addr = obj->relocbase + def->st_value;
    if (symbol_addr > addr || symbol_addr < info->dli_saddr) {
      continue;
    }

    /* Update our idea of the nearest symbol. */
    info->dli_sname = obj->strtab + def->st_name;
    info->dli_saddr = symbol_addr;

    /* Exact match? */
    if ((info->dli_saddr == addr) &&
        (ELF_ST_BIND(def->st_info) != STB_LOCAL)) {
      break;
    }
  }

  return 1;
}

void
_rtld_msg(const char *msg, int level, const char *filename, int line)
{
  if (_rtld_vtbl.msg) {
    _rtld_vtbl.msg(msg, level, filename, line);
  }
}

/*
 * Error reporting function.  Use it like printf.  If formats the message
 * into a buffer, and sets things up so that the next call to dlerror()
 * will return the message.
 */
void
_rtld_error(bool print, const char* file, int line, const char* fmt, ...)
{
  static char buf[256];
  va_list ap;

  va_start(ap, fmt);
  vsnprintf(buf, sizeof(buf), fmt, ap);
  if (print && _rtld_vtbl.msg) {
    _rtld_vtbl.msg(buf, FARF_ERROR_LEVEL, file, line);
  }
  error_message = buf;
  va_end(ap);
}

void _rtld_debug_state(void)
{
  // TODO remove this

  /* do nothing */
#if 0
  asm(""); /* Keep calls from being optimized away, see gcc docs
                                                                                        on function attributes */
#endif
}

void
_rtld_linkmap_add(Obj_Entry* obj)
{
  struct link_map* l = &obj->linkmap;
  struct link_map* prev;

  obj->linkmap.l_name = obj->path;
  obj->linkmap.l_addr = obj->relocbase;
  obj->linkmap.l_ld = obj->dynamic;
  if ((strcmp(obj->path, "_rtld_anonymous"))) {
    _rtld_debug_load(1, (unsigned int)obj->path, (unsigned int)obj->relocbase);
  }

  if (_rtld_debug.r_map == NULL) {
    _rtld_debug.r_map = l;
    return;
  }

  /*
   * Scan to the end of the list, but not past the entry for the
   * dynamic linker, which we want to keep at the very end.
   */
  for (prev = _rtld_debug.r_map;
       prev->l_next != NULL && prev->l_next != &_rtld_objself.linkmap;
       prev = prev->l_next) ;

  l->l_prev = prev;
  l->l_next = prev->l_next;

  if (l->l_next != NULL) {
    l->l_next->l_prev = l;
  }

  prev->l_next = l;
}

void
_rtld_linkmap_delete(Obj_Entry* obj)
{
  struct link_map* l = &obj->linkmap;

  _rtld_debug_load(0, (unsigned int)obj->path, (unsigned int)obj->relocbase);
  if (l->l_prev == NULL) {
    if ((_rtld_debug.r_map = l->l_next) != NULL) {
      l->l_next->l_prev = NULL;
    }

    return;
  }

  if ((l->l_prev->l_next = l->l_next) != NULL) {
    l->l_next->l_prev = l->l_prev;
  }
}

