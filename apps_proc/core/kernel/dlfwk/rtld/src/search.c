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


/*  $NetBSD: search.c,v 1.19 2004/10/22 05:39:57 skrll Exp $     */

/*
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
#include <sys/stat.h>
#include <dirent.h>

#include "debug.h"
#include "rtld.h"

static Obj_Entry*
_rtld_search_library_path(const char* name, size_t namelen,
                          const char* dir, size_t dirlen, int mode)
{
  char* pathname = 0;
  size_t pathnamelen;
  Obj_Entry* obj = 0;

  pathnamelen = dirlen + 1 + namelen;
  pathname = malloc(pathnamelen + 1);
  if (pathname) {
    (void)strlcpy(pathname, dir, dirlen);
    pathname[dirlen] = '/';
    strlcpy(pathname + dirlen + 1, name, sizeof(name));

    FARF(HIGH, "try %s", pathname);
    obj = _rtld_load_object((const char*)pathname, mode);

    free(pathname);
  } else {
    DLERROR("%s: failed to alloc %d bytes", name, dirlen);
  }
  return obj;
}

/*
 * Find the library with the given name, and return its full pathname.
 * The returned string is dynamically allocated.  Generates an error
 * message and returns NULL if the library cannot be found.
 *
 * If the second argument is non-NULL, then it refers to an already-
 * loaded shared object, whose library search path will be searched.
 */
Obj_Entry*
_rtld_load_library(const char* name, const Obj_Entry* refobj, int mode)
{
  char tmperror[256], *tmperrorp;
  Search_Path* sp;
  int namelen;
  Obj_Entry* obj;

  if (strchr(name, '/') != NULL) {
    /* hard coded pathname */
    if (name[0] != '/' && !_rtld_trust) {
      DLERROR("abs pathname required for shared object %s", name);
      return 0;
    }
    goto found;
  }

  FARF(HIGH, "load %s", name);

  tmperrorp = dlerror();
  if (tmperrorp != NULL) {
    strlcpy(tmperror, tmperrorp, sizeof(tmperror));
    tmperrorp = tmperror;
  }

  namelen = strlen(name);

  for (sp = _rtld_paths; sp != NULL; sp = sp->sp_next) {
    if ((obj = _rtld_search_library_path(name, namelen, sp->sp_path, sp->sp_pathlen, mode)) != NULL) {
      goto pathfound;
    }
  }

  if (refobj != NULL) {
    for (sp = refobj->rpaths; sp != NULL; sp = sp->sp_next) {
      if ((obj = _rtld_search_library_path(name, namelen, sp->sp_path, sp->sp_pathlen, mode)) != NULL) {
        goto pathfound;
      }
    }
  }

  for (sp = _rtld_default_paths; sp != NULL; sp = sp->sp_next) {
    if ((obj = _rtld_search_library_path(name, namelen, sp->sp_path, sp->sp_pathlen, mode)) != NULL) {
      goto pathfound;
    }
  }

  return NULL;

pathfound:
  /*
   * Successfully found a library; restore the dlerror state as it was
   * before _rtld_load_library() was called (any failed call to
   * _rtld_search_library_path() will set the dlerror state, but if the
   * library was eventually found, then the error state should not
   * change.
   */
  if (tmperrorp) {
    _rtld_error(false, __FILE__, __LINE__, tmperrorp);
  } else {
    (void)dlerror();
  }
  return obj;

found:
  obj = _rtld_load_object(name, mode);
  return obj;
}
