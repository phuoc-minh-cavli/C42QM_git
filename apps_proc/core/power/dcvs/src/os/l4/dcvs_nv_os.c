/*============================================================================
  FILE:         dcvs_nv_os.c
 
  OVERVIEW:     This file provides the os specific layer necessary for NV
                reading and writing.
 
  DEPENDENCIES: This code requires only the basic target NV support.
 
                Copyright (c) 2010-2011 Qualcomm Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/*============================================================================
  EDIT HISTORY FOR MODULE
  
  $Header: //components/rel/core.tx/6.0/power/dcvs/src/os/l4/dcvs_nv_os.c#1 $
  $DateTime: 2020/01/30 22:49:35 $

  when       who     what, where, why
  --------   ----    ---------------------------------------------------
  10/25/11    sg     Added support to use efs_get/put on targets where full EFS
                     support is not available. Also limits length of
                     strings saved to EFS to DCVS_NV_STRING_LEN_LIMIT when full
                     EFS is not supported.
  10/04/11    sg     Switch to std compiler libraries instead of using AEEstd.h
                     since that file is deprecated.
  06/11/08   bjs     Initial version
========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "BuildFeatures.h"
#include "DALStdDef.h"
#include "dcvs_nv.h"
#include "fs_lib.h"
#include "fs_public.h"
#include "stdarg.h"
#include "string.h"
#include "stringl/stringl.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

#define MAX_NV_PATH_LENGTH 256

/* On targets where multiple processors share the same file system,
   DCVS_NV_CORE_CPU_ROOT_PREFIX is used to set a different root folder
   like this: /nv/items_files/DCVS_NV_CORE_CPU_ROOT_PREFIX */
#ifndef DCVS_NV_CORE_CPU_ROOT_PREFIX
  #define DCVS_NV_CORE_CPU_ROOT_PREFIX ""
#endif

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

#ifndef FEATURE_DCVS_LIMITED_EFS
/**
 @brief dcvs_create_path - Create the path in efs to the file in
        question.

 Create all necessary paths to the file in question.
 
 @param path: The full filename the paths will be created for.
 @param mode: The mode to create directories with.
 
 @return None
 
 */
static void dcvs_create_path(char * path, int mode)
{
  uint32 index;
  // Step through all directories, and make sure they 
  // exist.  If not, create them.
  // 
  if (strlen(path) > 0)
  {
    index = 1;
    while (path[index] != 0)
    {
      if (path[index] == '/')
      {
        path[index] = 0;
        efs_mkdir (path, mode);
        path[index] = '/';
      }
      index++;
    }
  }
}

/**
 @brief dcvs_file_open - Open a file for creation.

 Open a file.  If the file doesn't open, assume the path
 doesn't exist, and create the path.  This function should only
 be used to create files.  It doesn't make sense for just
 reading files.
 
 @param path: The full filename the paths will be created for.
 @param oflag: The flags to use when opening the file.
 @param mode: The mode to create directories with.
 
 @return int: efs file handle
 
 */
static int dcvs_file_open(char * path, int oflag, int mode)
{
  int handle;
  handle = efs_open (path, oflag, mode);
  if (handle >= 0)
  {
    return(handle);
  }
  // The file failed to open.  Create the path.
  dcvs_create_path(path, mode);
  return(efs_open (path, oflag, mode));
}
#endif /* !FEATURE_DCVS_LIMITED_EFS */

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/

void DcvsNvJoin(char * path, uint32 pathLength, ...)
{
  va_list ap;
  char * nextStr;

  va_start(ap, pathLength);
  strlcpy(path, "", pathLength);
  nextStr = va_arg(ap, char *);

  while (nextStr != NULL)
  {
    if (strlen(nextStr))
    {
      strlcat(path, nextStr, pathLength - strlen(path));

      nextStr = va_arg(ap, char *);
      if (nextStr !=NULL)
      {
        strlcat(path, "/", pathLength - strlen(path));
      }
    }
    else
    {
      nextStr = va_arg(ap, char *);
    }
  }
  va_end(ap);
}

static void CreateFullPathname(char * filename, 
                               uint32 filenameLength, 
                               const char * path, 
                               const char * paramName,
                               const char * ext)
{
  // Create the full pathname plus filename
  DcvsNvJoin(filename, filenameLength, "/nv", "item_files",
             DCVS_NV_CORE_CPU_ROOT_PREFIX, path, paramName, NULL);

  // Add the file extension to the full filename.
  strlcat(filename, ext, filenameLength - strlen(filename));
}

#ifdef FEATURE_DCVS_LIMITED_EFS
/*                                                              
 * efs_get/put impose the following limitations:
 * Exact length of stored string must be known when efs_get() is called.                                                                
 * To make things simple, we're fixing the length of all stored strings to
 * DCVS_NV_STRING_LEN_LIMIT when this feature is used.
 */
DALBOOL DcvsNvStringRead(const char * path, 
                         const char * paramName, 
                         char * paramString,
                         uint32 paramStringLength)
{
  char fullFilename[MAX_NV_PATH_LENGTH];
  DALBOOL result = FALSE;

  CreateFullPathname(fullFilename, MAX_NV_PATH_LENGTH, path, paramName, ".txt"); 

  if (paramStringLength ==
      efs_get(fullFilename, (void *)paramString, paramStringLength))
  {
    result = TRUE;
  }

  return(result);
}

DALBOOL DcvsNvStringWrite(const char * path, 
                          const char * paramName, 
                          const char * paramString)
{
  char fullFilename[MAX_NV_PATH_LENGTH];
  char writeBuf[DCVS_NV_STRING_LEN_LIMIT];
  size_t paramStringLen = strlen(paramString);
  DALBOOL result = FALSE;

  CreateFullPathname(fullFilename, MAX_NV_PATH_LENGTH, path, paramName, ".txt"); 

  /* Pre-fill with NULL so that when we have a string that's smaller than
   * DCVS_NV_STRING_LEN_LIMIT-1, we don't write garbage.
   */
  memset(writeBuf, '\0', DCVS_NV_STRING_LEN_LIMIT);
  strlcpy(writeBuf, paramString, DCVS_NV_STRING_LEN_LIMIT);

  if (0 == efs_put(fullFilename,
                  (void *)writeBuf,
                  DCVS_NV_STRING_LEN_LIMIT,
                  O_CREAT | O_AUTODIR,
                  ALLPERMS))
  {
    result = TRUE;
  }

  return(result);
}

#else /* FEATURE_DCVS_LIMITED_EFS */

DALBOOL DcvsNvStringRead(const char * path, 
                         const char * paramName, 
                         char * paramString,
                         uint32 paramStringLength)
{
  char fullFilename[MAX_NV_PATH_LENGTH];
  int length, efsHandle;
  DALBOOL result = TRUE;

  CreateFullPathname(fullFilename, MAX_NV_PATH_LENGTH, path, paramName, ".txt"); 

  efsHandle = efs_open (fullFilename, O_RDONLY);
  if (efsHandle >= 0)
  {
    // Read the string.
    length = efs_read (efsHandle, (void *)paramString, (fs_size_t)paramStringLength);

    if ((length >= paramStringLength) || 
        (length < 0))
    {
      result = FALSE;
    }
    else
    {
      // Null terminate the string.
      paramString[length] = 0;
    }

    efs_close (efsHandle);
  }
  else
  {
    result = FALSE;
  }

  return(result);
}

DALBOOL DcvsNvStringWrite(const char * path, 
                          const char * paramName, 
                          const char * paramString)
{
  char fullFilename[MAX_NV_PATH_LENGTH];
  int length, strLength, efsHandle;
  DALBOOL result = TRUE;

  CreateFullPathname(fullFilename, MAX_NV_PATH_LENGTH, path, paramName, ".txt"); 

  efsHandle = dcvs_file_open (fullFilename, O_WRONLY | O_CREAT, ALLPERMS);
  if (efsHandle >= 0)
  {
    strLength = strlen(paramString);
    length = efs_write (efsHandle, (void *)paramString, (fs_size_t)strLength);
    if (length != strLength)
    {
      result = FALSE;
    }

    efs_close (efsHandle);
  }
  else
  {
    result = FALSE;
  }

  return(result);
}

#endif /* FEATURE_DCVS_LIMITED_EFS */

DALBOOL DcvsNvUint32Read(const char * path, 
                         const char * paramName, 
                         uint32 * paramValue)
{
  char fullFilename[MAX_NV_PATH_LENGTH];
  DALBOOL result = FALSE;

  CreateFullPathname(fullFilename, MAX_NV_PATH_LENGTH,
                     path, paramName, ".uint"); 

  if ( sizeof(uint32) == efs_get(fullFilename,
                                 (void *)paramValue,
                                 sizeof(uint32)) )
  {
    result = TRUE;
  }

  return(result);
}

DALBOOL DcvsNvUint32Write(const char * path, 
                          const char * paramName, 
                          uint32 paramValue)
{
  char fullFilename[MAX_NV_PATH_LENGTH];
  DALBOOL result = FALSE;

  CreateFullPathname(fullFilename, MAX_NV_PATH_LENGTH,
                     path, paramName, ".uint"); 

  if ( 0 == efs_put(fullFilename,
                    (void *)&paramValue,
                    sizeof(uint32),
                    O_CREAT | O_AUTODIR,
                    ALLPERMS) )
  {
    result = TRUE;
  }

  return(result);
}

