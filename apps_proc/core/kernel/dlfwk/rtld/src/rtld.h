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

/*  $NetBSD: rtld.h,v 1.71 2004/07/05 11:50:07 cube Exp $    */

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


#ifndef RTLD_H
#define RTLD_H

typedef char* caddr_t;
typedef long _Int32_t;
typedef unsigned int u_int32_t;
typedef unsigned int u_int;
typedef unsigned long long u_quad_t;

#if 0
#define __strong_alias(x,y) __asm__(".global "  #x  ";\n .set "  #x  ", "  #y ";");
#define __weak_alias(x,y) __asm__(".weak "  #x  ";\n .set "  #x  ", "  #y ";");
#else
#define __strong_alias(x,y)
#define __weak_alias(x,y)
#endif

#include "dlfcn.h"

#include <yvals.h>
#include <dlfcn.h>
#include <stddef.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/queue.h>
#include "exec_elf.h"
#include "link_elf.h"
#include "sys_mman.h"
#include <stdio.h>
#include "dlfs.h"

//#ifndef FARF_HIGH
//#define FARF_HIGH 1
//#endif
#ifndef FARF_ERROR
#define FARF_ERROR 1
#endif
#undef HAP_debug
#define HAP_debug _rtld_msg
#include "HAP_farf.h"

#define DLERROR(fmt, ...) \
  _rtld_error(true, __FILENAME__, __LINE__, fmt, ##__VA_ARGS__)

#define TRY(exception, func) \
   if (TEST_SUCCESS != (exception = func)) {\
      goto exception##bail; \
   }

#define THROW(exception, errno) \
   exception = errno; \
   goto exception##bail;

#define CATCH(exception) exception##bail: if (exception != 0)

#define VERIFY(exception, val) \
  do { \
    if(0 == (val)) { \
      exception = exception == 0 ? -1 : exception; \
      goto exception##bail; \
    } \
  } while (0)

#define VERIFYM(exception, val, fmt, ...) \
  do { \
    if(0 == (val)) { \
      _rtld_error(true, __FILENAME__, __LINE__, fmt, ##__VA_ARGS__); \
      exception = exception == 0 ? -1 : exception; \
      goto exception##bail; \
    } \
  } while (0)

#define __predict_true(exp) __builtin_expect((exp) != 0, 1)
#define __predict_false(exp)    __builtin_expect((exp) != 0, 0)

#ifndef RTLD_DEFAULT_LIBRARY_PATH
#define RTLD_DEFAULT_LIBRARY_PATH   "/usr/lib"
#endif

extern int _rtld_pagesz;

#define round_down(x)   ((x) & ~(_rtld_pagesz - 1))
#define round_up(x) round_down((x) + _rtld_pagesz - 1)

/*
 * C++ has mandated the use of the following keywords for its new boolean
 * type.  We might as well follow their lead.
 */
typedef enum {
  false = 0,
  true = 1
}
bool;

typedef int ino_t;
struct Struct_Obj_Entry;

typedef struct Struct_Objlist_Entry
{
  SIMPLEQ_ENTRY(Struct_Objlist_Entry) link;
  struct Struct_Obj_Entry* obj;
} Objlist_Entry;

typedef SIMPLEQ_HEAD(Struct_Objlist, Struct_Objlist_Entry) Objlist;

typedef struct Struct_Needed_Entry
{
  struct Struct_Needed_Entry* next;
  struct Struct_Obj_Entry* obj;
  unsigned long name;   /* Offset of name in string table */
} Needed_Entry;

typedef struct _rtld_search_path_t
{
  struct _rtld_search_path_t* sp_next;
  const char* sp_path;
  size_t sp_pathlen;
} Search_Path;

/*
 * Shared object descriptor.
 *
 * Items marked with "(%)" are dynamically allocated, and must be freed
 * when the structure is destroyed.
 */

#define RTLD_MAGIC  0xd550b87a
#define RTLD_VERSION    1
#define RTLD_MAIN   0x800

typedef struct Struct_Obj_Entry
{
  Elf32_Word magic; /* Magic number (sanity check) */
  Elf32_Word version; /* Version number of struct format */

  struct Struct_Obj_Entry* next;
  char* path; /* Pathname of underlying file (%) */
  int refcount;
  int dl_refcount; /* Number of times loaded by dlopen */

  /* These items are computed by map_object() or by digest_phdr(). */
  caddr_t mapbase; /* Base address of mapped region */
  size_t mapsize; /* Size of mapped region in bytes */
  caddr_t textbase; /* Base of executable region */
  size_t textsize; /* Size of text segment in bytes */
  Elf_Addr vaddrbase; /* Base address in shared object file */
  caddr_t relocbase; /* Reloc const = mapbase - *vaddrbase */
  Elf_Dyn* dynamic; /* Dynamic section */
  caddr_t entry; /* Entry point */
  const Elf_Phdr* __junk001;
  size_t pathlen; /* Pathname length */

  /* Items from the dynamic section. */
  Elf_Addr* pltgot; /* PLTGOT table */
  const Elf_Rel* rel; /* Relocation entries */
  const Elf_Rel* rellim; /* Limit of Relocation entries */
  const Elf_Rela* rela; /* Relocation entries */
  const Elf_Rela* relalim; /* Limit of Relocation entries */
  const Elf_Rel* pltrel; /* PLT relocation entries */
  const Elf_Rel* pltrellim; /* Limit of PLT relocation entries */
  const Elf_Rela* pltrela; /* PLT relocation entries */
  const Elf_Rela* pltrelalim; /* Limit of PLT relocation entries */
  const Elf_Sym* symtab; /* Symbol table */
  const char* strtab; /* String table */
  unsigned long   strsize; /* Size in bytes of string table */

  const Elf_Word* buckets; /* Hash table buckets array */
  unsigned long   nbuckets; /* Number of buckets */
  const Elf_Word* chains; /* Hash table chain array */
  unsigned long   nchains; /* Number of chains */

  Search_Path* rpaths; /* Search path specified in object */
  Needed_Entry* needed; /* Shared objects needed by this (%) */

  void (*init)(void); /* Initialization function to call */
  void (*fini)(void); /* Termination function to call */

  /* Entry points for dlopen() and friends. */
  void* (*dlopen)(const char*, int);
  void* (*dlsym)(void*, const char*);
  char* (*dlerror)(void);
  int (*dlclose)(void*);
  int (*dladdr)(const void*, Dl_info*);

  u_int32_t mainprog: 1, /* True if this is the main program */
            rtld: 1, /* True if this is the dynamic linker */
            textrel: 1, /* True if there are relocations to text seg */
            symbolic: 1, /* True if generated with "-Bsymbolic" */
            printed: 1, /* True if ldd has printed it */
            isdynamic: 1, /* True if this is a pure PIC object */
            mainref: 1, /* True if on _rtld_list_main */
            globalref: 1; /* True if on _rtld_list_global */

  struct link_map linkmap; /* for GDB */

  /* These items are computed by map_object() or by digest_phdr(). */
  const char* interp; /* Pathname of the interpreter, if any */
  Objlist dldags; /* Object belongs to these dlopened DAGs (%) */
  Objlist dagmembers; /* DAG has these members (%) */
  dev_t dev; /* Object's filesystem's device */
  ino_t ino; /* Object's inode number */
} Obj_Entry;

extern struct r_debug _rtld_debug;
extern Search_Path* _rtld_default_paths;
extern Obj_Entry* _rtld_objlist;
extern Obj_Entry** _rtld_objtail;
extern Obj_Entry* _rtld_objmain;
extern Obj_Entry _rtld_objself;
extern Search_Path* _rtld_paths;
extern bool _rtld_trust;
extern Objlist _rtld_list_global;
extern Objlist _rtld_list_main;
extern Elf_Sym _rtld_sym_zero;
extern DL_vtbl _rtld_vtbl;

/* rtld.c */

void _rtld_msg(const char *msg, int level, const char *filename, int line);
void _rtld_error(bool print, const char* file, int line, const char*, ...);
extern void _rtld_debug_state(void) __attribute__((used, noinline));

void _rtld_linkmap_add(Obj_Entry *);
void _rtld_linkmap_delete(Obj_Entry *);
void _rtld_objlist_remove(Objlist*, Obj_Entry* obj);
int _rtld_objlist_add(Objlist *, Obj_Entry *);
Objlist_Entry *_rtld_objlist_find(Objlist *, const Obj_Entry *);

/* headers.c */
int _rtld_digest_dynamic(Obj_Entry *);

/* load.c */
Obj_Entry *_rtld_load_object(const char *, int);
int _rtld_load_needed_objects(Obj_Entry *, int);
int _rtld_preload(const char *);
int _rtld_init_builtin(int libc, char *libv[]);
void _rtld_deinit_builtin(void);

/* path.c */
void _rtld_add_paths(Search_Path **, const char *);

/* reloc.c */
int _rtld_relocate_objects(Obj_Entry *, bool);
int _rtld_relocate_nonplt_objects(const Obj_Entry *);
int _rtld_relocate_plt_lazy(const Obj_Entry *);
int _rtld_relocate_plt_objects(const Obj_Entry *);
void _rtld_setup_pltgot(const Obj_Entry *);

/* search.c */
Obj_Entry *_rtld_load_library(const char *, const Obj_Entry *, int);

/* symbol.c */
unsigned long _rtld_elf_hash(const char *);
const Elf_Sym *_rtld_symlook_obj(const char *, unsigned long,
                                 const Obj_Entry *, bool);
const Elf_Sym *_rtld_find_symdef(unsigned long, const Obj_Entry *,
                                 const Obj_Entry **, bool);
const Elf_Sym *_rtld_symlook_list(const char *, unsigned long,
                                  const Objlist *, const Obj_Entry **, bool);
const Elf_Sym *_rtld_symlook_default(const char *, unsigned long,
                                     const Obj_Entry *, const Obj_Entry **, bool);

/* map_object.c */
Obj_Entry *_rtld_map_object(const char *, int, const struct dlfs_stat*);
void _rtld_obj_free(Obj_Entry*);
Obj_Entry* _rtld_obj_new(void);

#endif /* RTLD_H */
