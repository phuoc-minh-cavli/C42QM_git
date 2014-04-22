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

/*  $NetBSD: load.c,v 1.29 2004/10/22 05:39:56 skrll Exp $   */

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
#include <sys/types.h>
#include <dirent.h>

#include "debug.h"
#include "rtld.h"
#include "dlfs.h"

Objlist _rtld_list_main =   /* Objects loaded at program startup */
   SIMPLEQ_HEAD_INITIALIZER(_rtld_list_main);
Objlist _rtld_list_global = /* Objects dlopened with RTLD_GLOBAL */
   SIMPLEQ_HEAD_INITIALIZER(_rtld_list_global);

static char *strdup(const char *s){
	char *d = malloc (strlen (s) + 1);   
    if (d == NULL) return NULL;

    if(strlcpy(d, s, sizeof(d)) >= sizeof(d)) return d;          
    else free(d);
    
    return NULL;

}

void
_rtld_objlist_remove(Objlist* list, Obj_Entry* obj)
{
  Objlist_Entry* elm;

  if ((elm = _rtld_objlist_find(list, obj)) != NULL) {
    SIMPLEQ_REMOVE(list, elm, Struct_Objlist_Entry, link);
    free(elm);
  }
}

int
_rtld_objlist_add(Objlist* list, Obj_Entry* obj)
{
  Objlist_Entry* elm;

  if (0 == (elm = malloc(sizeof(Objlist_Entry)))) {
    DLERROR("failed to malloc (0x%X B)", sizeof(Objlist_Entry));
    return -1;
  }

  elm->obj = obj;
  SIMPLEQ_INSERT_TAIL(list, elm, link);
  return 0;
}

Objlist_Entry*
_rtld_objlist_find(Objlist* list, const Obj_Entry* obj)
{
  Objlist_Entry* elm;

  SIMPLEQ_FOREACH(elm, list, link) {
    if (elm->obj == obj) {
      return elm;
    }
  }
  return NULL;
}

/*
 * Load a shared object into memory, if it is not already loaded.  The
 * argument must be a string allocated on the heap.  This function assumes
 * responsibility for freeing it when necessary.
 *
 * Returns a pointer to the Obj_Entry for the object.  Returns NULL
 * on failure.
 */
Obj_Entry*
_rtld_load_object(const char* filepath, int mode)
{
  Obj_Entry* obj;
  struct dlfs_stat sb;
  int fd = -1;
  size_t pathlen = strlen(filepath);
  bool new = false;
  bool added_main = false;
  bool added_global = false;
  int nErr = 0;

  for (obj = _rtld_objlist->next; obj != NULL; obj = obj->next) {
    if (pathlen == obj->pathlen && !strcmp(obj->path, filepath)) {
      break;
    }
  }

  /*
   * If we didn't find a match by pathname, open the file and check
   * again by device and inode.  This avoids false mismatches caused
   * by multiple links or ".." in pathnames.
   *
   * To avoid a race, we open the file and use fstat() rather than
   * using stat().
   */
  if (obj == NULL) {

    fd = dlfs_open(filepath, O_RDONLY);
    if (-1 == fd) {
      FARF(ALWAYS, "cannot open %s", filepath);
      THROW(nErr, 1);
    }

    VERIFYM(nErr, 0 == dlfs_fstat(fd, &sb), "fstat failed %s", filepath);

    for (obj = _rtld_objlist->next; obj != NULL; obj = obj->next) {

      /* XXX_SM:
       * The hexagon simulator will run on windows, calls to fstat will eventually
       * map to a windows call to fstat.  On windows st_ino and st_dev will always
       * be 0 unless the fd is an actual device.  Because of this we create
       * artificial dev/ino values using stats creation time and size if and only
       * if both the original dev/ino values are 0.
       * See _rtld_map_object in map_object.c
       *
       * Non-Unix target OS's (QuRT/Blast) may have similar limitations
      */
      if ((obj->ino == sb.st_ino && obj->dev == sb.st_dev) || /* or windows */
          (obj->ino == sb.st_size && obj->dev == sb.st_ctime)) {
        break;
      }
    }
  }

  if (obj == NULL) {    /* First use of this object, so we must map it in */
    VERIFY(nErr, 0 != (obj = _rtld_map_object(filepath, fd, &sb)));
    new = true;
    VERIFY(nErr, 0 == _rtld_digest_dynamic(obj));
  }

  if (mode & RTLD_MAIN && !obj->mainref) {
    FARF(HIGH, "adding obj %p (%s) to main list", obj, obj->path);
    VERIFY(nErr, 0 == _rtld_objlist_add(&_rtld_list_main, obj));
    obj->mainref = 1;
    added_main = true;
  }

  if (mode & RTLD_GLOBAL && !obj->globalref) {
    FARF(HIGH, "adding obj %p (%s) to global list", obj, obj->path);
    VERIFY(nErr, 0 == _rtld_objlist_add(&_rtld_list_global, obj));
    obj->globalref = 1;
    added_global = true;
  }

  ++obj->refcount;

  if (new) {
    *_rtld_objtail = obj;
    _rtld_objtail = &obj->next;
    _rtld_linkmap_add(obj); /* for GDB */
  }

  CATCH(nErr) {
    if (obj) {
      if (added_global) {
        _rtld_objlist_remove(&_rtld_list_global, obj);
      }
      if (added_main) {
        _rtld_objlist_remove(&_rtld_list_main, obj);
      }
      if (new) {
        _rtld_obj_free(obj);
      }
    }
    obj = 0;
  }
  if (-1 != fd) {
    dlfs_close(fd);
  }
  return obj;
}

/*
 * function: _rtld_basename
 * description:
 *	- Utility function, return the basename of the given string.
 */
static char* _rtld_basename(const char* name)
{
  char* end = (char*)name + (strlen(name) - 1);

  /* back up until we hit the first slash, or the beginning of the string */
  while ((end != name) && (*end != '/')) end--;

  if (*end == '/') end++;

  return end;
}

static char** _hex_rtld_builtin;
static int _hex_rtld_builtin_count;

void _rtld_deinit_builtin(void)
{
  int i;
    
  if (_hex_rtld_builtin) {
    for (i = 0; i < _hex_rtld_builtin_count; i++) {
      free(_hex_rtld_builtin[i]);
    }
    free(_hex_rtld_builtin);
  }
}

int _rtld_init_builtin(int libc, char* libv[])
{
  int i;

  /* Allocate the list of pointers */
  _hex_rtld_builtin = malloc(libc + 1 * sizeof(uintptr_t));
  if (0 == _hex_rtld_builtin) {
    return 0;
  }

  /* Fill in the list */
  for (i = 0; i < libc; i++) {
    _hex_rtld_builtin[i] = strdup(_rtld_basename(libv[i]));
  }
  /* Terminate the list with a NULL */
  _hex_rtld_builtin[i] = NULL;

  /* Number of builtin libraries */
  _hex_rtld_builtin_count = libc;

  return 1;
}


/*
 * function: _rtld_is_builtin
 * description:
 *	- return 1 if the given name is in the list
 *	  of builtin libraries.
 */
static int _rtld_is_builtin(const char* name)
{
  int i = 0;
  for (i = 0; i < _hex_rtld_builtin_count; i++) {
    if (!strcmp(_hex_rtld_builtin[i], _rtld_basename(name))) return 1;
  }
  return 0;
}

/*
 * Given a shared object, traverse its list of needed objects, and load
 * each of them.  Returns 0 on success.  Generates an error message and
 * returns -1 on failure.
 */
int
_rtld_load_needed_objects(Obj_Entry* first, int mode)
{
  Obj_Entry* obj;
  int status = 0;

  for (obj = first; obj != NULL; obj = obj->next) {
    Needed_Entry* needed;

    for (needed = obj->needed; needed != NULL; needed = needed->next) {
      const char* name = obj->strtab + needed->name;

      if (_rtld_is_builtin(name)) {
        FARF(HIGH, "%s builtin", name);
        continue;
      }

      if (!(needed->obj == _rtld_load_library(name, obj, mode))) {
        status = -1;    /* FIXME - cleanup */
      }

      if (status == -1) {
        return status;
      }
    }
  }

  return status;
}

