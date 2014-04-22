/*===========================================================================
    FILE:           acdb_init_utility.c

    OVERVIEW:       This file contains the acdb init utility functions
                    implemented specifically for the MDM environment.

    DEPENDENCIES:   None

                    Copyright (c) 2012 QUALCOMM Technologies Incorporated.
                    All Rights Reserved.
                    Qualcomm Proprietary and Confidential.
========================================================================== */

/*===========================================================================
    EDIT HISTORY FOR MODULE

    This section contains comments describing changes made to the module.
    Notice that changes are listed in reverse chronological order. Please
    use ISO format for dates.

    $Header: //components/rel/audio.tx/2.0/audio/audcal/acdb_hlos/acdb_init_utility.c#1 $

    when        who     what, where, why
    ----------  ---     -----------------------------------------------------
    2017-03-13  vk      Fix QACT crash issue
	2010-07-08  vmn     Initial revision.
   
========================================================================== */

/* ---------------------------------------------------------------------------
 * Include Files
 *--------------------------------------------------------------------------- */

#include "customer.h"
#include "acdb.h"
#include "acdb_init_utility.h"
#include "acdb_command.h"
#include <stdio.h>
#include "csd_os_dependencies.h"

//#define IMAGE_KEY_ACDB_IMG_DEST_ADDR     0x46B00000

#define FILENAME_MAX_LEN           128
#define ACDB_IMG_TAG               0xacdbacdb

extern int32_t g_persistenceStatus;

extern qurt_mem_region_attr_t  acdb_mem_region_attr;

struct file_entry {
   uint32_t filenamelen;
   char filename[FILENAME_MAX_LEN];
   uint32_t offset;
   uint32_t size;
   uint32_t group_id;
   uint32_t flavor_id;
};

struct img_file_format {
   uint32_t acdb_tag;
   uint16_t major_version;
   uint16_t minor_version;
   uint32_t num_files;
//   struct file_entry file_list[0];      // Commented out due to compilation error on target
};

#ifdef _WIN32
#define ACDB_IMG_FILE         "mdm_acdb.img"
#define ACDB_BUFFER_SIZE      0x200000

extern char gAcdbBuffer[];
extern char gIsAcdbFileLoaded;
#endif

int32_t AcdbGetFileData (const char* pFilename,uint8_t **pBfr,uint32_t *pFileSize)
{
#ifdef _WIN32
   FILE *fp = NULL;
   int bytes_read = 0;
   char *pBuffer = gAcdbBuffer;
   int size_left = ACDB_BUFFER_SIZE;
   int size = 0;

   struct img_file_format *pheader = NULL;
   int i = 0;
   char bFileFound = 0;
   struct file_entry *file_list = NULL;

   if (0 == gIsAcdbFileLoaded)
   {
      fp = fopen(ACDB_IMG_FILE, "rb");
      if (NULL == fp)
      {
         return ACDB_ERROR;
      }

      while (size_left > 0 && 0 != (bytes_read = fread(pBuffer, 1, size_left, fp)))
      {
         size += bytes_read;
         size_left -= bytes_read;
         pBuffer += bytes_read;
      }

      fclose(fp);

      if (size_left < 0)
      {
         return ACDB_INSUFFICIENTMEMORY;
      }

      gIsAcdbFileLoaded = 1;
   }

   pheader = (struct img_file_format *)gAcdbBuffer;

   if (NULL == pheader)
      return ACDB_NULLPOINTER;

   file_list = (struct file_entry *)(gAcdbBuffer + sizeof(struct img_file_format));

   // 2. Look-up for the filename in the file list
   for (i = 0; i < pheader->num_files; i++)
   {
      if (0 == memcmp(pFilename, file_list[i].filename, file_list[i].filenamelen))
      {
         // 3. Obtain the offset and size and return it in the function.
         *pBfr = gAcdbBuffer + file_list[i].offset;
         *pFileSize = file_list[i].size;
         bFileFound = 1;
         break;
      }
   }

   if (0 == bFileFound)
      return ACDB_ERROR;

   return ACDB_SUCCESS;

#elif defined IMAGE_KEY_ACDB_IMG_DEST_ADDR
  struct img_file_format *pheader = NULL;
  int i = 0;
  char bFileFound = 0;
  struct file_entry *file_list = NULL;
  int32_t rc;
  uint32_t acdb_cfg;

  if (NULL == pFilename || NULL == pBfr || NULL == pFileSize)
    return ACDB_BADPARM;

  // 1. Read the Image header into structure
  pheader = (struct img_file_format *)acdb_mem_region_attr.virtaddr;
  if (NULL == pheader)
    return ACDB_NULLPOINTER;

  /* Verifying the image loaded is valid */
  if (ACDB_IMG_TAG != pheader->acdb_tag)
    return ACDB_ERROR;

  rc = acdb_get_platform_config( &acdb_cfg );
  
  if( rc != ACDB_SUCCESS )
  {
    return ACDB_ERROR;
  }
      
  file_list = (struct file_entry *)(acdb_mem_region_attr.virtaddr + sizeof(struct img_file_format));

  // 2. Look-up for the filename in the file list
  for (i = 0; i < pheader->num_files; i++)
  {
    if( file_list[i].flavor_id == acdb_cfg ||
        file_list[i].flavor_id == ACDB_FLAVOR_UNKNOWN )
    {
      if (0 == memcmp(pFilename, file_list[i].filename, file_list[i].filenamelen))
      {
        // 3. Obtain the offset and size and return it in the function.
        *pBfr = (uint8_t *)(acdb_mem_region_attr.virtaddr + file_list[i].offset);
        *pFileSize = file_list[i].size;
        bFileFound = 1;
        break;
      }
    }
  }

  if( 0 == bFileFound )
    return ACDB_ERROR;

   return ACDB_SUCCESS;
#else
   return ACDB_NOTSUPPORTED;
#endif
}

void  AcdbFreeFileData (void *pBfr)
{
  return;
}

int32_t AcdbIsPersistenceSupported(void)
{
   return g_persistenceStatus;
}

int32_t AcdbIsDeltaFileAvailable (const char* pFilename, uint32_t fileNameLen, uint32_t* pDeltaFileDataLen)
{
  return ACDB_UTILITY_INIT_FAILURE;
}

char *GetDeltaFileName(const char* pFilename, uint32_t fileNameLen)
{
  return NULL;
}

int32_t AcdbGetDeltaFileData (const char* pFilename, uint32_t fileNameLen, uint8_t *pFileBfr, uint32_t nFileBfrSize)
{
  return ACDB_UTILITY_INIT_FAILURE;
}

int32_t AcdbWriteDeltaFileData (const char* pFilename, uint32_t fileNameLen, uint8_t *pFileBfr, uint32_t nFileBfrSize)
{
  return ACDB_UTILITY_INIT_FAILURE;
}

int32_t AcdbDeleteDeltaFileData (const char* pFilename, uint32_t fileNameLen)
{
  return ACDB_UTILITY_INIT_FAILURE;
}




