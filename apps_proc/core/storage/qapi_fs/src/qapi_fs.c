/***********************************************************************
 * qapi_fs.c
 *
 * Copyright (C) 2017-2020, 2022-2023 QUALCOMM Technologies, Inc.
 * All Rights Reserved.
 *
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2023-06-27   sk    Allow usage of valid relative pathnames as arguments
2023-05-11   sk    Allow only absolute pathnames for files and directories
2023-02-03   sk    DAM module containment support for qapi_FS_Iter APIs
2022-09-28   sk    Update qapi_FS_Seek handler to support larger offsets
2020-09-09   vm    Fixed path address verification for module space.
2020-09-09   vm    Fixed NULL Pointer Dereference issue.
2020-07-29   cs    DAM module containment support for unprivileged module.
2020-05-05   bd    Add support for file containment based on module uid.
2019-07-16   as    Remove qapi FS initialize implementation.
2019-04-23   as    Deprecate support for few QAPIs.
2018-04-09   as    Mapping macro changed in qapi_FS_Seek API.
2017-11-02   as    added kernel and user space address check.
2017-04-13   as    Defining statvfs structure in QAPI same as in EFS.
2017-04-13   as    Fixed Klockwork error in qapi_FS_Iter_Next.
2017-02-22   as    Create.

===========================================================================*/

/* This should be on top. Do not move */

#include "qapi_fs.h"

#ifdef QAPI_TXM_SOURCE_CODE
#include "fs_public.h"
#include "stringl/stringl.h"
#include "fs_errno.h"
#include "tx_api.h"
#include "qurt_txm_qapi_fwk.h"
#include "qurt_error.h"
#include "qurt_memory.h"
#ifdef QAPI_FS_DAM_FILE_SEPERATION
#include "qurt_txm_uid.h"
#endif

/*==============================================================================
                                   MACROS
=============================================================================*/

/* Mode bits to open a file */

#define QAPI_FS_MAP_FLAG_TO_EFS(x,y)   y=0;\
                                (QAPI_FS_O_RDONLY_E & x)?y|=O_RDONLY:0; \
                                (QAPI_FS_O_WRONLY_E & x)?y|=O_WRONLY:0; \
                                (QAPI_FS_O_RDWR_E & x)?y|=O_RDWR  :0; \
                                (QAPI_FS_O_CREAT_E & x)?y|=O_CREAT :0; \
                                (QAPI_FS_O_EXCL_E & x)?y|=O_EXCL  :0; \
                                (QAPI_FS_O_TRUNC_E & x)?y|=O_TRUNC :0; \
                                (QAPI_FS_O_APPEND_E & x)?y|=O_APPEND:0; 

#define QAPI_FS_MAP_MODE_TO_EFS(x,y)   y=0;\
                                (QAPI_FS_S_IRUSR_E & x)?y|=S_IRUSR:0; \
                                (QAPI_FS_S_IWUSR_E & x)?y|=S_IWUSR:0; \
                                (QAPI_FS_S_IXUSR_E & x)?y|=S_IXUSR:0; \
                                (QAPI_FS_S_IRGRP_E & x)?y|=S_IRGRP:0; \
                                (QAPI_FS_S_IWGRP_E & x)?y|=S_IWGRP:0; \
                                (QAPI_FS_S_IXGRP_E & x)?y|=S_IXGRP:0; \
                                (QAPI_FS_S_IROTH_E & x)?y|=S_IROTH:0; \
                                (QAPI_FS_S_IWOTH_E & x)?y|=S_IWOTH:0; \
                                (QAPI_FS_S_IXOTH_E & x)?y|=S_IXOTH:0; \
                                (QAPI_FS_S_ISUID_E & x)?y|=S_ISUID:0; \
                                (QAPI_FS_S_ISGID_E & x)?y|=S_ISGID:0; \
                                (QAPI_FS_S_ISVTX_E & x)?y|=S_ISVTX:0;

#define QAPI_FS_MAP_SEEK_TO_EFS(x,y)  y=0;\
                                (QAPI_FS_SEEK_SET_E & x)?y|=SEEK_SET:0; \
                                (QAPI_FS_SEEK_CUR_E & x)?y|=SEEK_CUR:0; \
                                (QAPI_FS_SEEK_END_E & x)?y|=SEEK_END:0; 
                                
/* When file is opened using qapi_FS_Open API,it gets opened in this mode
Access rights of a file for a User(U),Group(G),Other(O)--->(UGO :110 110 110 )
110 ----> RWE (Read, Write, Execute)
That means that all 3 have Read and write permission and no execute permission*/

#define QAPI_DEFAULT_FILE_OPENING_MODE   0666

#ifdef QAPI_FS_DAM_FILE_SEPERATION
#define INVALID_MODULE_UID              0xFFFFFFFF
/* Each byte contains 2 digits, hence multiply by two ; plus buffer bytes to take care of forward slash and null-char 
   Ex : If module_uid is  FFFFFFFF  module_uid_string will be "/FFFFFFFF\0"
   */
#define MODULE_UID_STR_LEN              ((sizeof(UINT)*2)+4)
#define PARENT_PATH_STR_LEN             4 /* length of '/../' */
#define CURRENT_PATH_STR_LEN            3 /* length of '/./' */
#define PARENT_PATH_STR_LEN_SHORT       3 /* length of '/..' */
#define CURRENT_PATH_STR_LEN_SHORT      2 /* length of '/.' */

static UINT fd_ownership_table[EFS_MAX_FILE_DESCRIPTORS]; 
#endif

/*======================================================================
                          LOCAL FUNCTIONS
=======================================================================*/

/* API to map efs errors to QAPI errors. */
static qapi_FS_Status_t map_error_efs_to_qapi(void)
{
 qapi_FS_Status_t qapi_status = QAPI_OK;
 
 switch(efs_errno)
  {
    case ENOERR:
      qapi_status = QAPI_OK;
      break;
    case EPERM:
      qapi_status = QAPI_ERR_EPERM;
      break;
    case ENOENT:
      qapi_status = QAPI_ERR_NO_ENTRY;
      break;
    case EEXIST:
      qapi_status = QAPI_ERR_EXISTS;
      break;
    case EBADF:
      qapi_status = QAPI_ERR_EBADF;
      break;
    case ENOMEM:
      qapi_status = QAPI_ERR_NO_MEMORY;
      break;
    case EACCES:
      qapi_status = QAPI_ERR_EACCES;
      break;
    case EBUSY:
      qapi_status = QAPI_ERR_BUSY;
      break;
    case EXDEV:
      qapi_status = QAPI_ERR_EXDEV;
      break;
    case ENODEV:  
      qapi_status = QAPI_ERR_ENODEV;
      break;
    case ENOTDIR:
      qapi_status = QAPI_ERR_ENOTDIR;
      break;
    case EISDIR:
      qapi_status = QAPI_ERR_EISDIR;
      break;
    case EINVAL:
      qapi_status = QAPI_ERR_INVALID_PARAM;
      break;
    case EMFILE:
      qapi_status = QAPI_ERR_EMFILE;
      break;
    case ETXTBSY:
      qapi_status = QAPI_ERR_ETXTBSY;
      break;
    case ENOSPC:
      qapi_status = QAPI_ERR_ENOSPC;
      break;
    case ESPIPE:
      qapi_status = QAPI_ERR_ESPIPE;
      break;
    case ENAMETOOLONG:
      qapi_status = QAPI_ERR_ENAMETOOLONG;
      break;
    case ENOTEMPTY:
      qapi_status = QAPI_ERR_ENOTEMPTY;
      break;
    case ELOOP:
      qapi_status = QAPI_ERR_ELOOP;
      break;
    case EILSEQ:
      qapi_status = QAPI_ERR_EILSEQ;
      break;
    case ETIMEDOUT:
      qapi_status = QAPI_ERR_TIMEOUT;
      break;
    case ESTALE:
      qapi_status = QAPI_ERR_ESTALE;
      break;
    case EDQUOT:
      qapi_status = QAPI_ERR_EDQUOT;
      break;
    case EEOF:
      qapi_status = QAPI_ERR_EEOF;
      break;
    default:
      qapi_status = QAPI_ERR_UNKNOWN;
    }
    return qapi_status;
}

/* Internal function.
   Gets corresponding QAPI status codes given the fs error codes */
static qapi_FS_Status_t get_qapi_status(int Fs_result)
{
  if (Fs_result >= 0)
  {
    return QAPI_OK;
  }
  return map_error_efs_to_qapi();
}

#ifdef QAPI_FS_DAM_FILE_SEPERATION
static qapi_FS_Status_t is_path_owned_by_dam(char *file_path, char *module_uid)
{
  char *str1, *str2,*tmp;

  str1 = strtok_r((char *)file_path, "//", &tmp);
  str2 = strtok_r((char *)module_uid, "//", &tmp);
  
  if((str1 != NULL) && (str2 != NULL))
  {
    if(strcmp(str1,str2) == 0)
    {
      return (UINT)QAPI_OK;
    }
  }

  return QAPI_ERR_EACCES;
}


/* Parses './','/.' in the relative path */
static void parse_current_path(char *relative_path)
{
  char *substr = NULL;
  char *pos = relative_path;
  int index = -1;
  UINT path_len = 0;
  const char current_path[] = "/./";

  /* Path would be split into first path and last path */
  char first_path[FS_NAME_MAX] = {0}, last_path[FS_NAME_MAX] = {0};

  /* If it is absolute path, return */ 
  substr = strstr(relative_path, current_path);
  /* substr is NULL if substring is not present */
  if (!substr)
  {
    return;
  }
  /* Calculate the position of '/' in '/./' */
  index = substr - pos;
  if (index <= 0)
    return;

  snprintf(first_path, index + 1, "%s", relative_path);
  /* Extra 1 is for Null char */
  path_len = strlen(relative_path) - index - CURRENT_PATH_STR_LEN + 1;

  /* Get path after "/./" if it exists */
  if (relative_path[index + CURRENT_PATH_STR_LEN])
    snprintf(last_path, path_len, "%s",
             relative_path + index + CURRENT_PATH_STR_LEN);

  /* Add '/' between first and last part of path if it does not exist */
  if (last_path[0] && last_path[0] != '/')
    snprintf(relative_path, strlen(first_path) + strlen(last_path) + 2,
             "%s/%s", first_path, last_path);
  else
    snprintf(relative_path, strlen(first_path) + strlen(last_path) + 1,
             "%s%s", first_path, last_path);

  /* Recursively call to parse './','/.' */
  parse_current_path(relative_path);
}

/* Parses '../','/..' in the relative path */
static void parse_parent_path(char *relative_path)
{
  char *substr = NULL;
  char *pos = relative_path;
  int index = -1, i = 0;
  UINT path_len = 0;
  const char parent_path[] = "/../";

  /* Path would be split into first path and last path */
  char first_path[FS_NAME_MAX] = {0}, last_path[FS_NAME_MAX] = {0};

  /* If it is absolute path, return */ 
  substr = strstr(relative_path, parent_path);
  /* substr is NULL if substring is not present */
  if (!substr)
  {
    return;
  }
  /* Calculate the position of '/' in '/../' */
  index = substr - pos;
  if (index <= 0)
    return;

  snprintf(first_path, index + 1, "%s", relative_path);
  i = index;

  /* Remove the path before '/..' */
  while (i >= 0)
  {
    if (first_path[i] == '/')
    {
      first_path[i] = '\0';
      break;
    }
    first_path[i] = '\0';
    i--;
  }

  /* Extra 1 is for Null char */
  path_len = strlen(relative_path) - index - PARENT_PATH_STR_LEN + 1;

  /* Get path after "/../" if it exists */
  if (relative_path[index + PARENT_PATH_STR_LEN])
    snprintf(last_path, path_len, "%s",
             relative_path + index + PARENT_PATH_STR_LEN);

  /* Add '/' between first and last part of path if it does not exist */
  if (last_path[0] && last_path[0] != '/')
    snprintf(relative_path, strlen(first_path) + strlen(last_path) + 2,
             "%s/%s", first_path, last_path);
  else
    snprintf(relative_path, strlen(first_path) + strlen(last_path) + 1,
             "%s%s", first_path, last_path);

  /* Recursively call to parse '../','/..' */
  parse_parent_path(relative_path);
}

/* Convert multiple forward slashes into single slash */
static void parse_slashes_in_path(char *file_path)
{
  char *path_substr = NULL;
  const char invalid_substr[] = "//";
  /* fp_index would store index for file_path,
   * tp_index would store index for temp_path */
  int fp_index = 0, tp_index = 0;
  char temp_path[FS_NAME_MAX] = {0};

  path_substr = strstr(file_path, invalid_substr);

  /* If no '//' in path, then path is already proper */
  if (!path_substr)
    return;

  /* If we reach here, then it means multiple forward slashes exist.
   * Replace multiple '/' with single '/' */
  while (file_path[fp_index+1])
  {
    if (file_path[fp_index+1] == '/')
    {
      if (file_path[fp_index] == '/')
      {  
        fp_index++;
        continue;
      }
      else
      {
        temp_path[tp_index] = file_path[fp_index];
        fp_index++;
        tp_index++;
        continue;
      }
    }
    else
    {
      temp_path[tp_index] = file_path[fp_index];
      temp_path[tp_index+1] = file_path[fp_index+1];
      fp_index += 2;
      tp_index += 2;
    }
  }
  if (file_path[fp_index])
    temp_path[tp_index] = file_path[fp_index];

  snprintf(file_path, strlen(temp_path) + 1, "%s", temp_path);
  return;
}

static qapi_FS_Status_t is_path_inside_dam_module(const char *file_path,
                                                  const char *module_uid_str,
                                                  const UINT module_uid_len)
{
  char path[FS_NAME_MAX] = {0};
  const UINT file_path_size = sizeof(path);
  UINT path_len = 0;
  if (file_path == NULL)
    return QAPI_ERR_INVALID_PARAM;

  snprintf(path, file_path_size, "%s", file_path);

  parse_slashes_in_path(path);

  if (path[file_path_size - 1])
    return QAPI_ERR_INVALID_PARAM;
  path_len = strlen(path);

  if (path_len < CURRENT_PATH_STR_LEN_SHORT)
    return QAPI_ERR_INVALID_PARAM;

  if (!strncmp(&path[path_len - CURRENT_PATH_STR_LEN_SHORT], "/.",
      CURRENT_PATH_STR_LEN_SHORT))
  {
    /* Have extra space for adding "/" and null at end when required */
    if (path_len + 2 > file_path_size)
      return QAPI_ERR_INVALID_PARAM;
    snprintf(path + path_len, 2, "/");
  }

  parse_current_path(path);

  if (path[file_path_size - 1])
    return QAPI_ERR_INVALID_PARAM;
  path_len = strlen(path);

  if (path_len < PARENT_PATH_STR_LEN_SHORT)
    return QAPI_ERR_INVALID_PARAM;

  /* Making sure the path always has /../ even if user passes ".." or "/.."
   * at the end of path */
  if (!strncmp(&path[path_len - PARENT_PATH_STR_LEN_SHORT], "/..",
      PARENT_PATH_STR_LEN_SHORT))
  {
    /* Have extra space for adding "/" and null at end when required */
    if (path_len + 2 > file_path_size)
      return QAPI_ERR_INVALID_PARAM;
    snprintf(path + path_len, 2, "/");
  }

  parse_parent_path(path);

  if (path[file_path_size - 1])
    return QAPI_ERR_INVALID_PARAM;

  /* Check if module_uid is still in start of file path or not */
  if (strncmp(path, module_uid_str, module_uid_len))
    return QAPI_ERR_INVALID_PARAM;

  return QAPI_OK;
}
#endif

/*======================================================================
                          FUNCTIONS
=======================================================================*/
UINT qapi_FS_handler(UINT id, UINT a, UINT b, UINT c, UINT d, UINT e, 
                         UINT f, UINT g, UINT h, UINT i, UINT j, UINT k, UINT l)
{
  qapi_FS_Status_t retval = 0;
  qapi_FS_Offset_t seekoffset = 0;
#ifdef QAPI_FS_DAM_FILE_SEPERATION  
  char modified_path[FS_NAME_MAX] = {0};
  char module_uid_str[MODULE_UID_STR_LEN] = {0};
  UINT module_uid = INVALID_MODULE_UID;
  UINT module_uid_len = 0;
  TXM_MODULE_INSTANCE *module_instance = NULL;


  if(qurt_txm_module_instance_get(&module_instance) == (int)QAPI_OK)
  {
    // module_instance is assigned with a valid pointer in above API
    qurt_txm_module_uid_get(module_instance, &module_uid);
  }
  
  if(module_uid == INVALID_MODULE_UID)
  {
    return (UINT)QAPI_ERROR; // return error in case we failed to get module_uid
  }

  snprintf (module_uid_str, sizeof (module_uid_str), "/%x", module_uid);
  module_uid_len = strlen(module_uid_str);

  if((id == TXM_QAPI_FS_OPEN_WITH_MODE) || (id == TXM_QAPI_FS_OPEN) ||
     (id == TXM_QAPI_FS_RENAME) || (id == TXM_QAPI_FS_TRUNCATE) || 
     (id == TXM_QAPI_FS_UNLINK) || (id == TXM_QAPI_FS_STAT) || 
     (id == TXM_QAPI_FS_STATVFS) || (id == TXM_QAPI_FS_ITER_OPEN))
    {
      //validating if the address of const char* path passed by user falls in the current DAM region or not.
      if (qurt_check_if_module_address((qurt_addr_t)a, 1) == FALSE)
      {
        return (UINT)QAPI_ERR_INVALID_PARAM;
      }
      if (*(char *)a!='/')
        snprintf (modified_path, sizeof (modified_path), "/%x/%s", module_uid,
                 (char *)a);
      else
        snprintf (modified_path, sizeof (modified_path), "/%x%s", module_uid,
                 (char *)a);

      if (is_path_inside_dam_module(modified_path, module_uid_str, 
          module_uid_len) != QAPI_OK)
        return (UINT)QAPI_ERR_INVALID_PARAM;
    }
  if((id == TXM_QAPI_FS_MK_DIR) || (id == TXM_QAPI_FS_RM_DIR) )
  {
    //validating if the address of const char* path passed by user falls in the current DAM region or not.
    if (qurt_check_if_module_address((qurt_addr_t)a, 1) == FALSE)
    {
      return (UINT)QAPI_ERR_INVALID_PARAM;
    }
    if(strcmp((char *)a, module_uid_str) == 0)
      snprintf (modified_path, sizeof (modified_path), "%s", (char *)(a));
    else
    {
      if (*(char *)a!='/')
         snprintf (modified_path, sizeof (modified_path), "/%x/%s", module_uid, (char *)a);
      else
        snprintf (modified_path, sizeof (modified_path), "/%x%s", module_uid, (char *)a);
    }
    if (is_path_inside_dam_module(modified_path, module_uid_str, 
        module_uid_len) != QAPI_OK)
      return (UINT)QAPI_ERR_INVALID_PARAM;
  }
#endif

  switch(id)
  {
    case TXM_QAPI_FS_OPEN_WITH_MODE:
    {
      if(qurt_check_if_kernel_address((qurt_addr_t*)d) == TRUE)
      {
        return (UINT)QAPI_ERR_INVALID_PARAM;
      }
      else
      {
        if (qurt_check_if_module_address ((qurt_addr_t)d, sizeof(int))== FALSE)
        {
          return (UINT)QAPI_ERR_INVALID_PARAM;
        }
#ifdef QAPI_FS_DAM_FILE_SEPERATION
        retval = qapi_FS_Open_With_Mode(modified_path, (int)b,
                                        (qapi_FS_Mode_t)c, (int*)d);
        if(retval == QAPI_OK)
        {
          fd_ownership_table[*((int*)d)] = module_uid; // note down the module_uid, which owns this file-descriptor
        }
#else
        retval = qapi_FS_Open_With_Mode((const char*)a, (int)b,
                                        (qapi_FS_Mode_t)c, (int*)d);
#endif
      }
    }
    break;
    case TXM_QAPI_FS_OPEN:
    {
      if(qurt_check_if_kernel_address((qurt_addr_t*)c) == TRUE)
      {
        return (UINT)QAPI_ERR_INVALID_PARAM;
      }
      else
      {
        if (qurt_check_if_module_address ((qurt_addr_t)c, sizeof(int))== FALSE)
        {
          return (UINT)QAPI_ERR_INVALID_PARAM;
        }
#ifdef QAPI_FS_DAM_FILE_SEPERATION
        retval = qapi_FS_Open((const char *)modified_path, (int)b, (int*)c); 
        if(retval == QAPI_OK)
        {
         fd_ownership_table[*((int*)c)] = module_uid; // note down the module_uid, which owns this file-descriptor
        }
#else
        retval = qapi_FS_Open((const char*)a, (int)b, (int*)c);
#endif
      }
    }
    break;
    case TXM_QAPI_FS_READ:
    {
      if ((qurt_check_if_kernel_address((qurt_addr_t*)b) == TRUE) || 
          (qurt_check_if_kernel_address((qurt_addr_t*)d) == TRUE))
      {
        return (UINT)QAPI_ERR_INVALID_PARAM;
      }
      else
      {
#ifdef QAPI_FS_DAM_FILE_SEPERATION
        if(fd_ownership_table[(int)a] != module_uid)
        {
          return (UINT)QAPI_ERR_EACCES;
        }
#endif
        //validating if the address (buf start addess plus count) falls in the current DAM region or not.
        if (qurt_check_if_module_address((qurt_addr_t)b, (uint32)c) == FALSE)
        {
          return (UINT)QAPI_ERR_INVALID_PARAM;
        }
        if (qurt_check_if_module_address ((qurt_addr_t)d, sizeof(uint32))== FALSE)
        {
          return (UINT)QAPI_ERR_INVALID_PARAM;
        }
        retval = qapi_FS_Read((int)a, (uint8*)b, (uint32)c, (uint32*)d);
      }
    }
    break;
    case TXM_QAPI_FS_WRITE:
    {
      if ((qurt_check_if_kernel_address((qurt_addr_t*)b) == TRUE) || 
          (qurt_check_if_kernel_address((qurt_addr_t*)d) == TRUE))
      {
        return (UINT)QAPI_ERR_INVALID_PARAM;
      }
      else
      {
#ifdef QAPI_FS_DAM_FILE_SEPERATION
        if(fd_ownership_table[(int)a] != module_uid)
        {
          return (UINT)QAPI_ERR_EACCES;
        }
#endif
        //validating if the address (buf start addess plus count) falls in the current DAM region or not.
        if (qurt_check_if_module_address((qurt_addr_t)b, (uint32)c) == FALSE)
        {
          return (UINT)QAPI_ERR_INVALID_PARAM;
        }
        if (qurt_check_if_module_address ((qurt_addr_t)d, sizeof(uint32))== FALSE)
        {
          return (UINT)QAPI_ERR_INVALID_PARAM;
        }
        retval = qapi_FS_Write((int)a, (const uint8*)b, (uint32)c, (uint32*)d);
      }
    }
    break;
    case TXM_QAPI_FS_CLOSE:
    {
#ifdef QAPI_FS_DAM_FILE_SEPERATION
      if(fd_ownership_table[(int)a] != module_uid)
      {
        return (UINT)QAPI_ERR_EACCES;
      }
      retval = qapi_FS_Close((int)a);
      if(retval == QAPI_OK)
      {
        fd_ownership_table[a] = INVALID_MODULE_UID; 
      }
#else
      retval = qapi_FS_Close((int)a);
#endif
    }
      break;
    case TXM_QAPI_FS_RENAME:
    {
#ifdef QAPI_FS_DAM_FILE_SEPERATION
      char modified_path_rename[FS_NAME_MAX]={0};
      //validating if the address of const char* path passed by user falls in the current DAM region or not.
      if (qurt_check_if_module_address((qurt_addr_t)b, 1) == FALSE)
      {
        return (UINT)QAPI_ERR_INVALID_PARAM;
      }
      if (*(char *)b!='/')
        snprintf (modified_path_rename, sizeof (modified_path_rename), "/%x/%s", module_uid, (char *)b);
      else
        snprintf (modified_path_rename, sizeof (modified_path_rename), "/%x%s", module_uid, (char *)b);

      if (is_path_inside_dam_module(modified_path_rename, module_uid_str, 
          module_uid_len) != QAPI_OK)
        return (UINT)QAPI_ERR_INVALID_PARAM;
      retval = qapi_FS_Rename((const char*)modified_path, (const char*)modified_path_rename);
#else
      retval = qapi_FS_Rename((const char*)a, (const char*)b);
#endif
    }
      break;
    case TXM_QAPI_FS_TRUNCATE:
    {
#ifdef QAPI_FS_DAM_FILE_SEPERATION
      retval = qapi_FS_Truncate((const char*)modified_path, (qapi_FS_Offset_t)b);
#else
      retval = qapi_FS_Truncate((const char*)a, (qapi_FS_Offset_t)b);
#endif
    }
      break;
    case TXM_QAPI_FS_SEEK:
    {
      if ((qurt_check_if_kernel_address((qurt_addr_t*)d) == TRUE))
      {
        return (UINT)QAPI_ERR_INVALID_PARAM;
      }
      else
      {
        if (qurt_check_if_module_address((qurt_addr_t)d, sizeof(qapi_FS_Offset_t)) == FALSE)
        {
          return (UINT)QAPI_ERR_INVALID_PARAM;
        }
#ifdef QAPI_FS_DAM_FILE_SEPERATION
        if(fd_ownership_table[(int)a] != module_uid)
        {
          return (UINT)QAPI_ERR_EACCES;
        }
#endif
        /* Merging lower and upper 32-bit to derive original offset. */
        seekoffset = (qapi_FS_Offset_t)b | ((qapi_FS_Offset_t)e<<32);
        retval = qapi_FS_Seek((int)a, (qapi_FS_Offset_t)seekoffset,
                              (int)c, (qapi_FS_Offset_t*)d);
      }
    }
    break;
    case TXM_QAPI_FS_MK_DIR:
#ifdef QAPI_FS_DAM_FILE_SEPERATION
      retval = qapi_FS_Mk_Dir(modified_path, (qapi_FS_Mode_t)b); 
#else
      retval = qapi_FS_Mk_Dir((const char*)a, (qapi_FS_Mode_t)b);
#endif
      break;
    case TXM_QAPI_FS_RM_DIR:
    {
#ifdef QAPI_FS_DAM_FILE_SEPERATION
      retval = qapi_FS_Rm_Dir((const char*)modified_path);
#else
      retval = qapi_FS_Rm_Dir((const char*)a);
#endif
    }
      break;
    case TXM_QAPI_FS_UNLINK:
    {
#ifdef QAPI_FS_DAM_FILE_SEPERATION
      /* Module is not aware of the module ID prefix, so it only sends path without the module-id prefix.
         Unlink(file delete will be successful only if there is a file under this module-id directory */
      retval = qapi_FS_Unlink((const char*)modified_path);
#else
      retval = qapi_FS_Unlink((const char*)a);
#endif
    }
      break;
    case TXM_QAPI_FS_STAT:
    {
      if ((qurt_check_if_kernel_address((qurt_addr_t*)b) == TRUE))
      {
        return (UINT)QAPI_ERR_INVALID_PARAM;
      }
      else
      {
        if (qurt_check_if_module_address((qurt_addr_t)b, sizeof(struct qapi_FS_Stat_Type_s)) == FALSE)
        {
            return (UINT)QAPI_ERR_INVALID_PARAM;
        }
#ifdef QAPI_FS_DAM_FILE_SEPERATION
        retval = qapi_FS_Stat((const char*)modified_path, (struct qapi_FS_Stat_Type_s*)b);
#else
        retval = qapi_FS_Stat((const char*)a, (struct qapi_FS_Stat_Type_s*)b);
#endif
      }
    }
    break;
    case TXM_QAPI_FS_STAT_WITH_HANDLE:
    {
      if ((qurt_check_if_kernel_address((qurt_addr_t*)b) == TRUE))
      {
        return (UINT)QAPI_ERR_INVALID_PARAM;
      }
      else
      {
#ifdef QAPI_FS_DAM_FILE_SEPERATION
        if(fd_ownership_table[(int)a] != module_uid)
        {
          return (UINT)QAPI_ERR_EACCES;
        }
#endif
        if (qurt_check_if_module_address((qurt_addr_t)b, sizeof(struct qapi_FS_Stat_Type_s)) == FALSE)
        {
            return (UINT)QAPI_ERR_INVALID_PARAM;
        }
        retval = qapi_FS_Stat_With_Handle((int)a, (struct qapi_FS_Stat_Type_s*)b);
      }
    }
    break;
    case TXM_QAPI_FS_STATVFS:
    {
      if ((qurt_check_if_kernel_address((qurt_addr_t*)b) == TRUE))
      {
        return (UINT)QAPI_ERR_INVALID_PARAM;
      }
      else
      {
        if (qurt_check_if_module_address((qurt_addr_t)b, sizeof(struct qapi_FS_Statvfs_Type_s)) == FALSE)
        {
            return (UINT)QAPI_ERR_INVALID_PARAM;
        }
#ifdef QAPI_FS_DAM_FILE_SEPERATION
        retval = qapi_FS_Statvfs((const char*)modified_path, 
                               (struct qapi_FS_Statvfs_Type_s*)b);
#else
        retval = qapi_FS_Statvfs((const char*)a, 
                               (struct qapi_FS_Statvfs_Type_s*)b);
#endif
      }
    }
    break;
    case TXM_QAPI_FS_ITER_OPEN:
    {
      if ((qurt_check_if_kernel_address((qurt_addr_t*)a) == TRUE)
      || (qurt_check_if_kernel_address((qurt_addr_t*)b) == TRUE))
      {
        return (UINT)QAPI_ERR_INVALID_PARAM;
      }
      else
      {
        if (qurt_check_if_module_address ((qurt_addr_t)a, sizeof(char))== FALSE)
        {
          return (UINT)QAPI_ERR_INVALID_PARAM;
        }
        if (qurt_check_if_module_address ((qurt_addr_t)b, sizeof(qapi_FS_Iter_Handle_t))== FALSE)
        {
          return (UINT)QAPI_ERR_INVALID_PARAM;
        }
#ifdef QAPI_FS_DAM_FILE_SEPERATION
        retval = qapi_FS_Iter_Open(modified_path, (qapi_FS_Iter_Handle_t*)b);
#else
        retval = QAPI_ERR_NOT_SUPPORTED;
#endif
      }
    }
    break;
    case TXM_QAPI_FS_ITER_NEXT:
    {
      if ((qurt_check_if_kernel_address((qurt_addr_t*)b) == TRUE))
      {
        return (UINT)QAPI_ERR_INVALID_PARAM;
      }
      else
      {
         if (qurt_check_if_module_address ((qurt_addr_t)b, sizeof(struct qapi_FS_Iter_Entry_s))== FALSE)
         {
           return (UINT)QAPI_ERR_INVALID_PARAM;
         }
#ifdef QAPI_FS_DAM_FILE_SEPERATION
         retval = qapi_FS_Iter_Next((qapi_FS_Iter_Handle_t)a, 
                             (struct qapi_FS_Iter_Entry_s*)b);
#else
         retval = QAPI_ERR_NOT_SUPPORTED;
#endif
      }
    }
    break;
    case TXM_QAPI_FS_ITER_CLOSE:
    {
#ifdef QAPI_FS_DAM_FILE_SEPERATION
      retval = qapi_FS_Iter_Close((qapi_FS_Iter_Handle_t)a);
#else
      retval = QAPI_ERR_INVALID_PARAM;
#endif
    }
    break;
    case TXM_QAPI_FS_LAST_ERROR:
      retval = qapi_FS_Last_Error();
      break;
    default:
      retval = QAPI_ERR_INVLD_ID;
  }
  return (UINT)retval;
}

/* API for file opening */
qapi_FS_Status_t qapi_FS_Open(const char* path, 
                              int oflag,
                              int* fd_ptr)
{
  int efs_flag=0;
  qapi_FS_Mode_t efs_mode= QAPI_DEFAULT_FILE_OPENING_MODE;
  
  if(fd_ptr==NULL)
  {
      return QAPI_ERR_INVALID_PARAM;
  }
  QAPI_FS_MAP_FLAG_TO_EFS(oflag, efs_flag);
  *fd_ptr = efs_open(path, efs_flag, efs_mode);
  
  return get_qapi_status(*fd_ptr);                    
}

/* API for file opening with mode as input */
qapi_FS_Status_t qapi_FS_Open_With_Mode(const char* path, 
                              int oflag, 
                              qapi_FS_Mode_t mode, 
                              int* fd_ptr)                        
{
  int efs_flag=0;
  qapi_FS_Mode_t efs_mode=0;
  
  if(fd_ptr==NULL)
  {
      return QAPI_ERR_INVALID_PARAM;
  }
  QAPI_FS_MAP_FLAG_TO_EFS(oflag, efs_flag);
  QAPI_FS_MAP_MODE_TO_EFS(mode, efs_mode);
  *fd_ptr = efs_open(path, efs_flag, efs_mode);
 
  return get_qapi_status(*fd_ptr);
}

/* API for file read */
qapi_FS_Status_t qapi_FS_Read(int fd, 
                              uint8* buf, 
                              uint32 count,
                              uint32* bytes_read_ptr)
{
  if(bytes_read_ptr==NULL)
  {
    return QAPI_ERR_INVALID_PARAM;
  }
  *bytes_read_ptr = efs_read(fd, buf, count);
  
  return get_qapi_status(*bytes_read_ptr);
}

/* API for file write */
qapi_FS_Status_t qapi_FS_Write(int fd, 
                               const uint8* buf, 
                               uint32 count,
                               uint32* bytes_written_ptr)
{
  if(bytes_written_ptr==NULL)
  {
    return QAPI_ERR_INVALID_PARAM;
  }
  *bytes_written_ptr = efs_write(fd, buf, count);
  
  return get_qapi_status(*bytes_written_ptr);
}

/* API for file close */
qapi_FS_Status_t qapi_FS_Close(int fd)
{
  int Fs_result = efs_close(fd);
  return get_qapi_status(Fs_result);
}

/* API for file rename */
qapi_FS_Status_t qapi_FS_Rename(const char* old_path, const char* new_path)                    
{
  int Fs_result=0;
  
  Fs_result = efs_rename(old_path, new_path);
  
  return get_qapi_status(Fs_result);
}

/* API for file truncate upto (4GB-1) bytes */
qapi_FS_Status_t qapi_FS_Truncate(const char* path, qapi_FS_Offset_t length)                 
{
  int Fs_result = efs_truncate64(path, length);
  return get_qapi_status(Fs_result);
}

/* API for file seek upto (4GB-1) bytes */
qapi_FS_Status_t qapi_FS_Seek(int fd, 
                              qapi_FS_Offset_t offset, 
                              int whence,
                              qapi_FS_Offset_t *actual_offset_ptr)                   
{
  int efs_whence = 0;
  
  if(actual_offset_ptr==NULL)
  {
    return QAPI_ERR_INVALID_PARAM;
  }
  QAPI_FS_MAP_SEEK_TO_EFS(whence,efs_whence);
  *actual_offset_ptr = efs_lseek64(fd, offset, efs_whence);
  
  return get_qapi_status(*actual_offset_ptr);
}

/* API for making directory */
qapi_FS_Status_t qapi_FS_Mk_Dir(const char* path, qapi_FS_Mode_t mode)
{
  int efs_mode = 0, Fs_result=0;
  
  QAPI_FS_MAP_MODE_TO_EFS(mode, efs_mode);
  Fs_result = efs_mkdir(path, efs_mode);
  
  return get_qapi_status(Fs_result);
}
 
/* API for removing directory */
qapi_FS_Status_t qapi_FS_Rm_Dir(const char* path)                 
{
  int Fs_result = efs_rmdir(path);
  return get_qapi_status(Fs_result);
}

/* API to delete a file*/
qapi_FS_Status_t qapi_FS_Unlink(const char* path)                   
{
  int Fs_result = efs_unlink(path);
  return get_qapi_status(Fs_result);
}

/*API to get file statistical information */
qapi_FS_Status_t qapi_FS_Stat(const char* path,
                              struct qapi_FS_Stat_Type_s* sbuf)
{
  int Fs_result = efs_stat(path, (struct fs_stat* )sbuf);  
  return get_qapi_status(Fs_result);
}

/*API to get file statistical information using file handle */
qapi_FS_Status_t qapi_FS_Stat_With_Handle(int fd, 
                                          struct qapi_FS_Stat_Type_s* sbufh)
{
  int Fs_result = efs_fstat(fd, (struct fs_stat* )sbufh);
  return get_qapi_status(Fs_result);
}

/*API to get filesystem statistical information */
qapi_FS_Status_t qapi_FS_Statvfs(const char* path,
                                 struct qapi_FS_Statvfs_Type_s* sbuf_vfs)
{
  int Fs_result = efs_statvfs(path, (struct fs_statvfs* )sbuf_vfs);
  return get_qapi_status(Fs_result);
}

/*API to get a directory handle */
qapi_FS_Status_t qapi_FS_Iter_Open(const char* path, 
                                   qapi_FS_Iter_Handle_t* handle)
{
  if(handle == NULL)
  {
    return QAPI_ERR_INVALID_PARAM;
  }
  
  *handle = efs_opendir(path);
  
  if(*handle == NULL)
  {
    return map_error_efs_to_qapi();
  }
    
  return QAPI_OK;
}
 
/*API to iterate through a directory */
qapi_FS_Status_t qapi_FS_Iter_Next(qapi_FS_Iter_Handle_t Iter_Hdl,
                                   struct qapi_FS_Iter_Entry_s* Iter_Entry)
{
  struct qapi_FS_Iter_Entry_s* Temp_Entry;
  int temp_errno = efs_errno;
  
  if(Iter_Entry == NULL)
  {
    return QAPI_ERR_INVALID_PARAM;
  }
  
 /* We can set efs_errno to 0 here and check its value after calling 
    efs_readdir API if it returned NULL.Now if API returns NULL,there are 2 cases:
    1.If efs_errno is still 0, that implies end of directory is reached
    and we will restore errno from backup variable temp_errno.
    2.Otherwise we will return error code.*/

  efs_errno = 0;
  Temp_Entry = (struct qapi_FS_Iter_Entry_s*)efs_readdir((EFSDIR*)Iter_Hdl);
  
  if(Temp_Entry == NULL)
  {
    memset(Iter_Entry, 0, sizeof(*Iter_Entry));
    if(efs_errno == 0)
    {
      /*Since errno did not change, so end of directory must be reached */
      efs_errno = temp_errno;
      return QAPI_OK;
    }
    else
    {
      /*Since errno changed, so an error must have occured.Returning the error*/
      return map_error_efs_to_qapi();
    }
  }
  
  efs_errno = temp_errno;
  memscpy(Iter_Entry, sizeof(*Iter_Entry), Temp_Entry, sizeof(*Temp_Entry));
    
  return QAPI_OK;
}

/*API to close a directory handle */
qapi_FS_Status_t qapi_FS_Iter_Close(qapi_FS_Iter_Handle_t Iter_Hdl)
{
  int Fs_result = 0;
  
  if(Iter_Hdl == NULL)
  {
      return  QAPI_ERR_INVALID_PARAM;
  }

  Fs_result = efs_closedir((EFSDIR*)Iter_Hdl);
  
  return get_qapi_status(Fs_result);
}

/* API to get last error in current task's context*/
qapi_FS_Status_t qapi_FS_Last_Error(void)
{
  return map_error_efs_to_qapi();
}

#endif 


