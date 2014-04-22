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


/*  $NetBSD: paths.c,v 1.32 2004/10/22 05:39:56 skrll Exp $  */

/*
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
 * 3. The name of the author may not be used to endorse or promote products
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


#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "debug.h"
#include "rtld.h"

static Search_Path* _rtld_find_path(Search_Path*, const char*, size_t);
static Search_Path** _rtld_append_path(Search_Path**, Search_Path**,
                                       const char*, const char*);

static int matchstr(const char*, const char*, const char*) __attribute__((unused));

//static const char WS[] = " \t\n";

/*
 * Like xstrdup(), but takes end of string as a argument.
 */
static char *
exstrdup(const char *bp, const char *ep)
{
  char* cp;
  size_t len = ep - bp;

  cp = malloc(len + 1);
  if (cp) {
    memcpy(cp, bp, len);
    cp[len] = '\0';
  }
  return (cp);
}

/*
 * Match `bp' against NUL terminated string pointed by `p'.
 */
static int
matchstr(const char* p, const char* bp, const char* ep)
{
  int c;

  while (bp < ep) if ((c = *p++) == 0 || c != *bp++) {
      return (0);
    }

  return (*p == 0);
}

static Search_Path*
_rtld_find_path(Search_Path* path, const char* pathstr, size_t pathlen)
{

  for (; path != NULL; path = path->sp_next) {
    if (pathlen == path->sp_pathlen &&
        memcmp(path->sp_path, pathstr, pathlen) == 0) {
      return path;
    }
  }

  return NULL;
}

static Search_Path**
_rtld_append_path(Search_Path** head_p, Search_Path** path_p,
                  const char* bp, const char* ep)
{
  Search_Path* path;

  if (_rtld_find_path(*head_p, bp, ep - bp) != NULL) {
    return path_p;
  }

  if (0 == (path = malloc(sizeof(Search_Path)))) {
    // TODO for now don't fail load, if rpath used more
    // by customers unwind the callpath to return an error
    DLERROR("failed to add rpath");
  } else {
    path->sp_pathlen = ep - bp;
    path->sp_path = exstrdup(bp, ep);
    path->sp_next = (*path_p);
    *path_p = path;
    path_p = &path->sp_next;

    FARF(HIGH, "added path \"%s\"", path->sp_path);
  }
  return path_p;
}

void
_rtld_add_paths(Search_Path** path_p, const char* pathstr)
{
  Search_Path** head_p = path_p;

  if (pathstr == NULL) {
    return;
  }

  if (pathstr[0] == ':') {
    /*
     * Leading colon means append to current path
     */
    while ((*path_p) != NULL) {
      path_p = &(*path_p)->sp_next;
    }

    pathstr++;
  }

  for (;;) {
    const char* bp = pathstr;
    const char* ep = strchr(bp, ':');

    if (ep == NULL) {
      ep = &pathstr[strlen(pathstr)];
    }
    
    path_p = _rtld_append_path(head_p, path_p, bp, ep);
    if (ep[0] == '\0') {
      break;
    }

    pathstr = ep + 1;
  }
}

