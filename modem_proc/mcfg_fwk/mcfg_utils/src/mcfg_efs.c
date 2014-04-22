/*==============================================================================

                M O D E M   C O N F I G   E F S
 
GENERAL DESCRIPTION
  EFS library for MCFG storage

Copyright (c) 2012-2013 by QUALCOMM Technologies Incorporated.
==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/mcfg_fwk.mpss/7.10.1.1/mcfg_utils/src/mcfg_efs.c#1 $ 
$DateTime: 2020/02/19 23:07:19 $ 
$Author: pwbldsvc $ 
$Change: 22644646 $ 

when     who  what, where, why
-------- ---  ----------------------------------------------------------------
05/08/12 cy   Initial creation

==============================================================================*/

#include "comdef.h"

#include "fs_errno.h"
#include "fs_public.h"
#include "md5.h"

#include "mcfg_osal.h"

#include "mcfg_common.h"
#include "mcfg_utils.h"
#include "mcfg_utils_i.h"
#include "mcfg_auth_i.h"
#include "mcfg_setting.h"

#include "mcfg_efs.h"
#include "mcfg_efs_i.h"
#include "mcfg_map_i.h"
#include "mcfg_remote_i.h"
#include "mcfg_trl.h"
#include "mcfg_proc.h"

/*==============================================================================

                  DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/
/*==============================================================================
  Constants and Macros
==============================================================================*/

/*==============================================================================
  Typedefs
==============================================================================*/

/*==============================================================================
  Variables
==============================================================================*/
  
/*==============================================================================

                     INTERNAL FUNCTION DECLARATIONS

==============================================================================*/

/*==============================================================================
                    PUBLIC FUNCTION DECLARATIONS FOR MODULE
==============================================================================*/
static boolean mcfg_efs_read_fd(int fd, uint32 offset, uint32 size, uint8* data)
{
  int efs_ret;
  fs_size_t total_bytes_read = 0;

  if (data != NULL)
  {
    efs_ret = efs_lseek(fd, offset, SEEK_SET);
    if (efs_ret == offset)
    {
      do
      {
        efs_ret = efs_read(fd, (data + total_bytes_read), (size - total_bytes_read));
      } while (efs_ret > 0 && (total_bytes_read += efs_ret) < size);
    }
  }
  return (total_bytes_read == size);
} /* mcfg_efs_read_all() */

/*===========================================================================

  FUNCTION mcfg_efs_get_config_filename

  DESCRIPTION
    This function translates the config ID into the efs file name which
    stores the config file

  DEPENDENCIES
    None

  PARAMETERS
    type      [in]  type of ID 
    id        [in]  unique ID struct
    filename  [out] filename
    size      [in]  max size of filename buf
 
  RETURN VALUE
    TRUE if success, FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_efs_get_config_filename
(
  mcfg_config_type_e_type type,
  mcfg_config_id_s_type  *id,
  char                   *filename,
  size_t                  size
)
{
  MD5_CTX hash_context;
  uint8 md5sum[16];
  int i, offset = 0;

  MCFG_CHECK_NULL_PTR_RET_FALSE(id);
  MCFG_CHECK_NULL_PTR_RET_FALSE(filename);
  MCFG_ASSERT_COND_RET_FALSE(type < MCFG_NUM_OF_TYPES);

  if(type == MCFG_TYPE_HW &&
     memcmp(id->id, MCFG_EFS_DEFAULT_ID_HW, id->id_len) == 0)
  {
     if(strlcpy(filename, MCFG_EFS_DEFAULT_FILE_HW, size) ==
         strlen(MCFG_EFS_DEFAULT_FILE_HW))
    {
      return TRUE;
    }
    else
    {
      MCFG_MSG_ERROR("mcfg_efs_get_config_filename failed");
      return FALSE;
    }
  }
  else if (type == MCFG_TYPE_SW &&
     memcmp(id->id, MCFG_EFS_DEFAULT_ID_SW, id->id_len) == 0)
  {
    if(strlcpy(filename, MCFG_EFS_DEFAULT_FILE_SW, size) ==
         strlen(MCFG_EFS_DEFAULT_FILE_SW))
    {
      return TRUE;
    }
    else
    {
      MCFG_MSG_ERROR("mcfg_efs_get_config_filename failed");
      return FALSE;
    }
  }

  memset(filename, 0, MCFG_EFS_RECORD_FILENAME_MAX);

  /* Initialize md5 hash calculation. */
  MD5Init(&hash_context);

  /* Perform hash calculation for unique config id. */
  MD5Update(&hash_context, id->id, id->id_len);

  /* Copy the md5sum value to filename  */
  MD5Final((unsigned char *)md5sum, &hash_context);

  switch(type)
  {
    case MCFG_TYPE_HW:
      offset = snprintf(filename, MCFG_EFS_RECORD_FILENAME_MAX, "%s", MCFG_EFS_FILE_HW_PREFIX);
      break;

    case MCFG_TYPE_SW:
      offset = snprintf(filename,MCFG_EFS_RECORD_FILENAME_MAX, "%s", MCFG_EFS_FILE_SW_PREFIX);
      break;

    default:
      MCFG_MSG_ERROR_1("mcfg_efs_get_config_filename: type %d not supported", type);
      return FALSE;
  }

  for (i=0; i<16; i++)
  {
    if (snprintf(filename+offset,MCFG_EFS_RECORD_FILENAME_MAX, "%02X", 
                 md5sum[i]) != 2)
    {
      MCFG_MSG_ERROR_1("mcfg_efs_get_config_filename: snprintf failed %d", i);
      return FALSE;
    }

    offset += 2;
  }

  //MCFG_MSG_SPRINTF_1("mcfg_efs_get_config_filename:  %s", filename);
  
  return TRUE;
} /* mcfg_efs_get_config_filename() */

/*===========================================================================

  FUNCTION mcfg_efs_get_config_path

  DESCRIPTION
    This function translates the config type and ID into the efs path which
    stores the config file

  DEPENDENCIES
    None

  PARAMETERS
    type      [in]  type of ID to query
    id        [in]  unique ID struct
    path      [out] file path
 
  RETURN VALUE
    TRUE if success, FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_efs_get_config_path
(
  mcfg_config_type_e_type type,
  mcfg_config_id_s_type  *id,
  char                   *path
)
{
  int offset = 0;
  MCFG_CHECK_NULL_PTR_RET_FALSE(id);
  MCFG_CHECK_NULL_PTR_RET_FALSE(path);
  MCFG_ASSERT_COND_RET_FALSE(type < MCFG_NUM_OF_TYPES);

  offset = snprintf(path, MCFG_EFS_RECORD_FILEPATH_MAX, "%s/cfg_%02d/", MCFG_EFS_PATH_META, type);
  //MCFG_MSG_LOW_1("mcfg_efs_get_config_path offset=%d", offset);
  if (offset <= 0 || !mcfg_efs_get_config_filename(
        type, id, path+offset, MCFG_EFS_RECORD_FILEPATH_MAX-offset))
  {
    return FALSE;
  }

  //MCFG_MSG_SPRINTF_1("mcfg_efs_get_config_path: %s", path);

  return TRUE;
} /* mcfg_efs_get_config_path() */

/*===========================================================================

  FUNCTION mcfg_efs_get_config_path

  DESCRIPTION
    This function translates the config type and ID into the efs path which
    stores the config file

  DEPENDENCIES
    None

  PARAMETERS
    type      [in]  type of ID to query
    id        [in]  unique ID struct
    path      [out] file path
 
  RETURN VALUE
    TRUE if success, FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_efs_get_config_path_ext
(
  mcfg_config_type_e_type type,
  mcfg_config_id_s_type  *id,
  char                   *ext,
  char                   *path
)
{
  if (!mcfg_efs_get_config_path(type, id, path))
  {
    MCFG_MSG_ERROR("get_config_path_ext: Failed to get Path");
    return FALSE;
  }

  if (strlcpy(path+strlen(path), ext, MCFG_EFS_RECORD_FILEPATH_MAX+5-strlen(path)) != strlen(ext))
  {
    MCFG_MSG_ERROR("get_config_path_ext: Failed to get Path");
    return FALSE;
  }

  //MCFG_MSG_SPRINTF_1("mcfg_efs_get_config_path_ext: %s", path);

  return TRUE;
} /* mcfg_efs_get_config_path_ext() */


#ifdef FEATURE_MCFG_REMOTE_STORAGE
/*===========================================================================

  FUNCTION mcfg_efs_get_config_remote_path

  DESCRIPTION
    This function translates the config type and ID into the efs path which
    stores the config file

  DEPENDENCIES
    None

  PARAMETERS
    type      [in]  type of ID to query
    id        [in]  unique ID struct
    path      [out] file path
 
  RETURN VALUE
    TRUE if success, FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_efs_get_config_remote_path
(
  mcfg_config_type_e_type type,
  mcfg_config_id_s_type  *id,
  char                   *remote_path
)
{
  char path[MCFG_EFS_RECORD_FILEPATH_MAX+5];
  int fd = -1;

  memset(path, 0, sizeof(path));
  if (!mcfg_efs_get_config_path_ext(type, id, "/path", path))
  {
    MCFG_MSG_ERROR("get_config_path_ext: Failed to get Path");
    return FALSE;
  }

  fd = efs_open(path, O_RDONLY);
  if (fd < 0)
  {
    MCFG_MSG_LOW("mcfg_efs_get_config_remote_path: remote path not found, try default");
    
    if (!mcfg_remote_get_config_path(type, id, remote_path))
    {
      MCFG_MSG_LOW("mcfg_efs_get_config_remote_path: default remote path also not found");
      return FALSE;
    }
  }
  else
  {
    mcfg_efs_read_fd(fd, 0, MCFG_EFS_RECORD_FILEPATH_MAX+5, (uint8 *)remote_path);
    if (strlen(remote_path) > (MCFG_EFS_RECORD_FILEPATH_MAX+5) ||
        strlen(remote_path) == 0)
    {
      MCFG_MSG_ERROR("mcfg_efs_get_config_remote_path: fail to get remote path");
      efs_close(fd);
      return FALSE;
    }
  }

  efs_close(fd);
  MCFG_MSG_SPRINTF_1("mcfg_efs_get_config_remote_path: %s", remote_path);

  return TRUE;
} /* mcfg_efs_get_config_remote_path() */
#endif /* FEATURE_MCFG_REMOTE_STORAGE */

/*===========================================================================

  FUNCTION mcfg_efs_get_config_size

  DESCRIPTION
    This function is used to read config size for a given
    type and ID of config file.

  DEPENDENCIES
    None

  PARAMETERS
    type [in]  type of config memory to query
    id   [in]  ID of the config to query
    size [out] size of config

  RETURN VALUE
    TRUE if it exists, FALSE if it doesn't

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_efs_get_config_size
(
  mcfg_config_type_e_type type,
  mcfg_config_id_s_type  *id,
  int                    *size
)
{
  char    path[MCFG_EFS_RECORD_FILEPATH_MAX+3];
  int     fd = -1;
  boolean status = FALSE;

  MCFG_CHECK_NULL_PTR_RET_FALSE(id);
  MCFG_CHECK_NULL_PTR_RET_FALSE(size);
  MCFG_ASSERT_COND_RET_FALSE(type < MCFG_NUM_OF_TYPES);

  if (!mcfg_efs_get_config_path_ext(type, id, "/size", path))
  {
    MCFG_MSG_ERROR("get_config_size: Failed to get Path");
    return FALSE;
  }

  /* Open the file in read only mode */
  fd = efs_open(path, O_RDONLY); 
  if (fd < 0)
  {
    MCFG_MSG_SPRINTF_2("get_config_size: error %d opening '%s'", efs_errno, path);
    return FALSE;
  }

  if (!mcfg_efs_read_fd(fd, 0, sizeof(*size), (uint8 *)size))
  {
    MCFG_MSG_ERROR_2("get_config_size: Error reading %d bytes of data (%d)",
                  sizeof(*size), efs_errno);
    *size = 0;
  }
  else
  {
    status = TRUE;
  }

  if (efs_close(fd) != 0)
  {
    MCFG_MSG_ERROR_2("get_config_size: Error closing file"
                     " (fd %d / errno %d)",	fd, efs_errno);
  }

  return TRUE;
} /* mcfg_efs_get_config_size() */

/*===========================================================================

  FUNCTION mcfg_efs_get_config

  DESCRIPTION
    This function is used to load a new config file on the device into
    the config buffer in memory.

  DEPENDENCIES
    None

  PARAMETERS
    type   [in]  type of the new config
    id     [in]  ID of the new config
    config [in/out] config structure including config size and buffer pointer

  RETURN VALUE
    TRUE if success, FALSE otherwise

  SIDE EFFECTS
    Config file is loaded into memory

===========================================================================*/
boolean mcfg_efs_get_config
(
  mcfg_config_type_e_type type,
  mcfg_config_id_s_type  *id,
  mcfg_config_s_type     *config
)
{
  char    path[MCFG_EFS_RECORD_FILEPATH_MAX];
  int     fd;
  boolean status = FALSE;
  int     tmp_config_len;
  struct  fs_stat stat;
  /*size after uncompression*/
  uint32 uncompressed_len = 0;

  MCFG_CHECK_NULL_PTR_RET_FALSE(id);
  MCFG_CHECK_NULL_PTR_RET_FALSE(config);
  MCFG_CHECK_NULL_PTR_RET_FALSE(config->config_addr);
  MCFG_ASSERT_COND_RET_FALSE(type < MCFG_NUM_OF_TYPES);

  if (!mcfg_efs_get_config_path_ext(type, id, "/mbn", path))
  {
    MCFG_MSG_ERROR("mcfg_efs_get_config: Failed to get Path");
    return FALSE;
  }

  /* Open the file in read only mode */
  fd = efs_open(path, O_RDONLY); 
  if (fd < 0)
  {
    MCFG_MSG_MED_1("cannot find local MBN in EFS errno%d, try remote", efs_errno);
#ifdef FEATURE_MCFG_REMOTE_STORAGE
    return mcfg_remote_get_config(type, id, config);
#else
    return FALSE;
#endif /* FEATURE_MCFG_REMOTE_STORAGE */
  }

  if (efs_stat(path, &stat) < 0)
  {
    MCFG_MSG_ERROR("mcfg_efs_get_config: config not exist ");
    return FALSE;
  }

  tmp_config_len = stat.st_size;

  if (!mcfg_efs_read_fd(fd, 0, tmp_config_len, config->config_addr))
  {
    MCFG_MSG_ERROR_2("mcfg_efs_get_config: Error reading %d bytes of data (%d)",
                  config->config_len, efs_errno);
  }
  else
  {
    status = TRUE;
  }

  if(status && tmp_config_len < config->config_len)
  {
    MCFG_MSG_MED_1("mcfg_efs_get_config: compressed config len %d", tmp_config_len);
    uncompressed_len = config->config_len;
    config->config_len = tmp_config_len;

    status = mcfg_utils_check_and_update_config(config, uncompressed_len);
  }

  if (efs_close(fd) != 0)
  {
    MCFG_MSG_ERROR_2("Error closing MCFG image file (fd %d / errno %d)",
                     fd, efs_errno);
  }

#ifdef FEATURE_MCFG_REMOTE_STORAGE
  if (!status)
  {
    MCFG_MSG_MED("Can't find config in EFS, try to get from remote");
    status = mcfg_remote_get_config(type, id, config);
  }
#endif /* FEATURE_MCFG_REMOTE_STORAGE */

  return status;
} /* mcfg_efs_get_config() */

/*===========================================================================

  FUNCTION mcfg_efs_get_config_info

  DESCRIPTION
    This function is used to read extended information for a given config file
    on the device.

  DEPENDENCIES
    None

  PARAMETERS
    type      [in]  type of ID to query
    id        [in]  ID of the config to query
    info      [out] info for the specified config

  RETURN VALUE
    TRUE if it exists, FALSE if it doesn't

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_efs_get_config_info
(
  mcfg_config_type_e_type  type,
  mcfg_config_id_s_type   *id,
  mcfg_config_info_s_type *info
)
{
  char    path[MCFG_EFS_RECORD_FILEPATH_MAX];
  int     fd;
  int     config_len;
  boolean status = FALSE;
  uint32  trailer_offset;
  uint8   prefix_type;
  uint16  trailer_type, trailer_len;

  MCFG_CHECK_NULL_PTR_RET_FALSE(id);
  MCFG_CHECK_NULL_PTR_RET_FALSE(info);
  MCFG_ASSERT_COND_RET_FALSE(type < MCFG_NUM_OF_TYPES);

  if (!mcfg_efs_get_config_path(type, id, path))
  {
    MCFG_MSG_ERROR("mcfg_efs_get_config_info: Failed to get Path");
    return FALSE;
  }

  /* Open the file in read only mode */
  fd = efs_open(path, O_RDONLY); 
  if (fd < 0)
  {
    MCFG_MSG_ERROR_1("mcfg_efs_get_config_info: Unable to open MCFG Image "
                     "file (%d)", efs_errno);
    return FALSE;
  }

  do
  {                
    config_len = efs_lseek(fd, 0, SEEK_END);
    if (config_len == -1)
    {
      MCFG_MSG_SPRINTF_1("mcfg_efs_get_config_info: seek failure for %s", path);
      break;
    }
    info->config_len = config_len;
 
    if (!mcfg_efs_read_fd(fd, config_len - sizeof(trailer_offset),
                          sizeof(trailer_offset), (uint8 *) &trailer_offset))
    {
      MCFG_MSG_ERROR_1("mcfg_efs_get_config_info: Error reading trailer offset"
                       " (%d)", efs_errno);
      break;
    }
    trailer_offset = config_len - trailer_offset;

    if (!mcfg_efs_read_fd(fd, trailer_offset + MCFG_GCI_PREFIX_TYPE_OFFSET,
                          sizeof(prefix_type), (uint8 *) &prefix_type))
    {
      MCFG_MSG_ERROR_1("mcfg_efs_get_config_info: Error reading trailer prefix"
                       " (%d)", efs_errno);
      break;
    }

    if (prefix_type != MCFG_GCI_MAGIC_PREFIX_TYPE)
    {
      MCFG_MSG_ERROR_1("mcfg_efs_get_config_info: invalid trailer prefix type"
                       " (%d)", prefix_type);
      break;
    }

    if (!mcfg_efs_read_fd(fd, trailer_offset + MCFG_GCI_TRAILER_TYPE_OFFSET,
                          sizeof(trailer_type), (uint8 *) &trailer_type))
    {
      MCFG_MSG_ERROR_1("mcfg_efs_get_config_info: Error reading trailer type"
                       " (%d)", efs_errno);
      break;
    }

    if (trailer_type != MCFG_GCI_MAGIC_TRAILER_TYPE)
    {
      MCFG_MSG_ERROR_1("mcfg_efs_get_config_info: invalid trailer type"
                       " (%d)", trailer_type);
      break;
    }

    if (!mcfg_efs_read_fd(fd, trailer_offset + MCFG_GCI_TRAILER_LENGTH_OFFSET,
                          sizeof(trailer_len), (uint8 *) &trailer_len))
    {
      MCFG_MSG_ERROR_1("mcfg_efs_get_config_info: Error reading trailer length"
                       " (%d)", efs_errno);
      break;
    }

    if (trailer_len > MCFG_CONFIG_DESCRIPTION_SIZE_MAX)
    {
      MCFG_MSG_ERROR_1("mcfg_efs_get_config_info: trailer too long"
                       " (%d)", trailer_len);
      break;
    }

    if (!mcfg_efs_read_fd(fd, trailer_offset + MCFG_GCI_TRAILER_DESC_OFFSET,
                          trailer_len, info->description))
    {
      MCFG_MSG_ERROR_1("mcfg_efs_get_config_info: Error reading config info"
                       "i (%d)", efs_errno);
      break;
    }

    if (strncmp((char *)(info->description), "MCFG_TRL", 8))
    {
      MCFG_MSG_ERROR("mcfg_efs_get_config_info: invalid MCFG trailer magic");
      break;
    }

    info->description_len = (uint32) trailer_len;

    status = TRUE;
  } while (0);

  if (efs_close(fd) != 0)
  {
    MCFG_MSG_ERROR_2("mcfg_efs_get_config_info: Error closing MCFG image file"
                     " (fd %d / errno %d)",	fd, efs_errno);
  }

  return status;
} /* mcfg_efs_get_config_info() */

/*===========================================================================

  FUNCTION mcfg_efs_get_saved_config_info

  DESCRIPTION
    This function is used to read extended information for a given config file
    on the device.

  DEPENDENCIES
    None

  PARAMETERS
    type      [in]  type of ID to query
    id        [in]  ID of the config to query
    info      [out] info for the specified config

  RETURN VALUE
    TRUE if it exists, FALSE if it doesn't

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_efs_get_saved_config_info
(
  mcfg_config_type_e_type  type,
  mcfg_config_id_s_type   *id,
  mcfg_config_info_s_type *info
)
{
  struct  fs_stat stat;
  char    path[MCFG_EFS_RECORD_FILEPATH_MAX+5];
  int     fd = -1;
  boolean status = FALSE;

  MCFG_CHECK_NULL_PTR_RET_FALSE(id);
  MCFG_CHECK_NULL_PTR_RET_FALSE(info);
  MCFG_ASSERT_COND_RET_FALSE(type < MCFG_NUM_OF_TYPES);

  if (!mcfg_efs_get_config_path_ext(type, id, "/info", path))
  {
    MCFG_MSG_ERROR("get_saved_config_info: Failed to get path");
    return FALSE;
  }

  if (efs_stat(path, &stat) < 0)
  {
    MCFG_MSG_ERROR("get_saved_config_info: info not exist ");
    return FALSE;
  }

  MCFG_ASSERT_COND_RET_FALSE(stat.st_size <= MCFG_CONFIG_DESCRIPTION_SIZE_MAX);

  info->description_len = stat.st_size;
  /* Open the file in read only mode */
  fd = efs_open(path, O_RDONLY); 
  if (fd < 0)
  {
    MCFG_MSG_ERROR_1("mcfg_efs_get_saved_config_info: Unable to open info "
                     "file (%d)", efs_errno);
    return FALSE;
  }

  if (!mcfg_efs_read_fd(fd, 0, info->description_len, (uint8 *)info->description))
  {
    MCFG_MSG_ERROR_2("mcfg_efs_get_saved_config_info: Error reading %d bytes of data (%d)",
                  info->description_len, efs_errno);
  }
  else
  {
    status = mcfg_efs_get_config_size(type, id, &info->config_len);
  }

  if (efs_close(fd) != 0)
  {
    MCFG_MSG_ERROR_2("mcfg_efs_get_saved_config_info: Error closing MCFG image file"
                     " (fd %d / errno %d)",	fd, efs_errno);
  }

  return status;
} /* mcfg_efs_get_saved_config_info() */

/*===========================================================================

  FUNCTION mcfg_efs_store_buf_in_path

  DESCRIPTION
    This function stores data buf into EFS with specified path

  DEPENDENCIES
    None

  PARAMETERS
    buf       [in]  data buffer
    len       [in]  length of data buffer
    path      [in]  config path
 
  RETURN VALUE
    TRUE if success, FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_efs_store_buf_in_path
(
  void *buf,
  int   len,
  char *path
)
{
  int fd;
  int efs_ret;
  int bytes_written;

  MCFG_CHECK_NULL_PTR_RET_FALSE(buf);
  MCFG_CHECK_NULL_PTR_RET_FALSE(path);
  MCFG_ASSERT_COND_RET_FALSE(len <= MCFG_CONFIG_SIZE_MAX);

  efs_unlink(path);
  
  fd = efs_open(path, O_CREAT | O_WRONLY); 
  if (fd < 0)
  {
    MCFG_MSG_ERROR_1("Unable to create/open MCFG Image file (%d)",efs_errno);
    return FALSE;
  }
  
  bytes_written = 0;
  do
  {
    efs_ret = efs_write(fd, (uint8 *)buf + bytes_written, 
                        len - bytes_written);
  } while (efs_ret > 0 && (bytes_written += efs_ret) < len);
  
  efs_close(fd); 

  if (bytes_written < len)
  {
    MCFG_MSG_ERROR_2("only wrote %d bytes for config with len %d",
                     bytes_written, len);
    return FALSE;
  }

  return TRUE;
} /* mcfg_efs_store_buf_in_path() */

/*===========================================================================

  FUNCTION mcfg_efs_store_buf_in_path

  DESCRIPTION
    This function stores data buf into EFS with specified path

  DEPENDENCIES
    None

  PARAMETERS
    buf       [in]  data buffer
    len       [in]  length of data buffer
    path      [in]  config path
 
  RETURN VALUE
    TRUE if success, FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_efs_store_buf_ext
(
  void                    *buf,
  int                      len,
  mcfg_config_type_e_type  type,
  mcfg_config_id_s_type   *id,
  char                    *ext
)
{
  char    path[MCFG_EFS_RECORD_FILEPATH_MAX+5];

  MCFG_CHECK_NULL_PTR_RET_FALSE(buf);
  MCFG_ASSERT_COND_RET_FALSE(type < MCFG_NUM_OF_TYPES);
  MCFG_ASSERT_COND_RET_FALSE(len>0);
  MCFG_CHECK_NULL_PTR_RET_FALSE(id);
  MCFG_CHECK_NULL_PTR_RET_FALSE(ext);

  if (!mcfg_efs_get_config_path(type, id, path))
  {
    MCFG_MSG_ERROR("mcfg_efs_store_buf_ext: Failed to get Path");
    return FALSE;
  }

  efs_mkdir(path, 0755);
  
  if ((strlen(path)+strlen(ext)) >= (MCFG_EFS_RECORD_FILEPATH_MAX+5))
  {
    MCFG_MSG_ERROR("mcfg_efs_store_buf_ext: path is too long");
    return FALSE;
  }

  if (strlcpy(path+strlen(path), ext, MCFG_EFS_RECORD_FILEPATH_MAX+5-strlen(path)) != strlen(ext))
  {
    MCFG_MSG_ERROR("get_config_path_ext: Failed to get Path");
    return FALSE;
  }

  //MCFG_MSG_SPRINTF_1("mcfg_efs_store_buf_ext: %s", path);
  if (!mcfg_efs_store_buf_in_path(buf, len, path))
  {
    MCFG_MSG_ERROR_1("mcfg_efs_store_buf_ext: Failed to save buf len %d into local EFS", len);
    return FALSE;
  }

  return TRUE;
} /* mcfg_efs_store_buf_ext() */

#ifdef FEATURE_MCFG_REMOTE_STORAGE
/*===========================================================================

  FUNCTION mcfg_efs_store_config_path

  DESCRIPTION
    This function is used to store remote path into local efs info file

  DEPENDENCIES
    None

  PARAMETERS
    type      [in]  type of ID to query
    id        [in]  ID of the config to query
    path      [in]  remote path

  RETURN VALUE
    TRUE if success

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_efs_store_config_path
(
  mcfg_config_type_e_type  type,
  mcfg_config_id_s_type   *id,
  char                    *remote_path
)
{
  MCFG_ASSERT_COND_RET_FALSE(type < MCFG_NUM_OF_TYPES);
  MCFG_CHECK_NULL_PTR_RET_FALSE(id);
  MCFG_CHECK_NULL_PTR_RET_FALSE(remote_path);

  if (!mcfg_efs_store_buf_ext(remote_path, strlen(remote_path), type, id, "/path"))
  {
    MCFG_MSG_ERROR("mcfg_efs_store_config_path: Failed to save remove path into local EFS");
    return FALSE;
  }

  return TRUE;
} /* mcfg_efs_store_config_path() */
#endif /* FEATURE_MCFG_REMOTE_STORAGE */

/*===========================================================================

  FUNCTION mcfg_efs_store_config_size

  DESCRIPTION
    This function is used to store config size into local efs info file

  DEPENDENCIES
    None

  PARAMETERS
    type      [in]  type of ID to query
    id        [in]  ID of the config to query
    path      [in]  remote path

  RETURN VALUE
    TRUE if success

  SIDE EFFECTS
    None

===========================================================================*/
mcfg_error_e_type mcfg_efs_store_config_size
(
  mcfg_config_type_e_type  type,
  mcfg_config_id_s_type   *id,
  uint32                   size
)
{
  MCFG_ASSERT_COND_RET_FALSE(type < MCFG_NUM_OF_TYPES);
  MCFG_CHECK_NULL_PTR_RET_FALSE(id);

  if (!mcfg_efs_store_buf_ext(&size, sizeof(size), type, id, "/size"))
  {
    MCFG_MSG_ERROR("mcfg_efs_store_config_path: Failed to save file size into local EFS");
    return MCFG_ERR_OUT_OF_SPACE;
  }

  return MCFG_ERR_NONE;
} /* mcfg_efs_store_config_size() */

/*===========================================================================

  FUNCTION mcfg_efs_store_config_info

  DESCRIPTION
    This function is used to store config info into local efs info file

  DEPENDENCIES
    None

  PARAMETERS
    type      [in]  type of ID to query
    id        [in]  ID of the config to query
    path      [in]  config

  RETURN VALUE
    mcfg_error_e_type

  SIDE EFFECTS
    None

===========================================================================*/
mcfg_error_e_type  mcfg_efs_store_config_info
(
  mcfg_config_type_e_type  type,
  mcfg_config_id_s_type   *id,
  mcfg_config_s_type      *config
)
{
  mcfg_config_info_s_type info;
  mcfg_trl_config_version_s_type version;

  MCFG_ASSERT_COND_RET_FALSE(type < MCFG_NUM_OF_TYPES);
  MCFG_CHECK_NULL_PTR_RET_FALSE(id);
  MCFG_CHECK_NULL_PTR_RET_FALSE(config);
  MCFG_CHECK_NULL_PTR_RET_ERROR(config->config_addr, MCFG_ERR_NULL_POINTER);
  MCFG_ASSERT_COND_RET_ERROR(config->config_len > 0, MCFG_ERR_INVALID_PARAMETER);
  MCFG_ASSERT_COND_RET_ERROR(config->config_len <= MCFG_CONFIG_SIZE_MAX, MCFG_ERR_INVALID_PARAMETER);

  memset(&info, 0, sizeof(info));
  memset(&version, 0, sizeof(version));

  if (!mcfg_utils_parse_config_info(config, &info))
  {
    MCFG_MSG_ERROR("store_config_info: Failed to parse config info");
    return MCFG_ERR_INCOMPATIBLE;
  }

  if(!mcfg_trl_decode_tlv(&info, MCFG_TRL_TLV_ID_CONFIG_VERSION,
                       &version, sizeof(version)))
  {
    MCFG_MSG_ERROR("trl_decode_tlv:config version not found");
    return MCFG_ERR_INCOMPATIBLE;
  }

  if(mcfg_compat_check(type, (uint32 *)&version) == MCFG_COMPAT_FAIL)
  {
    MCFG_MSG_ERROR("config version not compatible");
    return MCFG_ERR_INCOMPATIBLE;
  }

  if (!mcfg_efs_store_buf_ext(&info.description, info.description_len, type, id, "/info"))
  {
    MCFG_MSG_ERROR("mcfg_efs_store_config_path: Failed to save config info into local EFS");
    return MCFG_ERR_INTERNAL;
  }

  return MCFG_ERR_NONE;
} /* mcfg_efs_store_config_info() */

/*===========================================================================

  FUNCTION mcfg_efs_store_config_info

  DESCRIPTION
    This function is used to store config info into local efs info file

  DEPENDENCIES
    None

  PARAMETERS
    type      [in]  type of ID to query
    id        [in]  ID of the config to query
    path      [in]  remote path

  RETURN VALUE
    mcfg_error_e_type

  SIDE EFFECTS
    None

===========================================================================*/
mcfg_error_e_type mcfg_efs_store_config
(
  mcfg_config_type_e_type  type,
  mcfg_config_id_s_type   *id,
  mcfg_config_s_type      *config
)
{
  MCFG_ASSERT_COND_RET_FALSE(type < MCFG_NUM_OF_TYPES);
  MCFG_CHECK_NULL_PTR_RET_FALSE(id);
  MCFG_CHECK_NULL_PTR_RET_FALSE(config);
  MCFG_CHECK_NULL_PTR_RET_ERROR(config->config_addr, MCFG_ERR_NULL_POINTER);
  MCFG_ASSERT_COND_RET_ERROR(config->config_len > 0, MCFG_ERR_INVALID_PARAMETER);
  MCFG_ASSERT_COND_RET_ERROR(config->config_len <= MCFG_CONFIG_SIZE_MAX, MCFG_ERR_INVALID_PARAMETER);

  MCFG_MSG_MED_2("mcfg_efs_store_config: config_len %d, comp_config_len = %d", config->config_len, config->comp_config_len);
  MCFG_ASSERT_COND_RET_ERROR(config->comp_config_len > 0, MCFG_ERR_INVALID_PARAMETER);
  MCFG_CHECK_NULL_PTR_RET_ERROR(config->comp_config_addr, MCFG_ERR_NULL_POINTER);

  if (!mcfg_efs_store_buf_ext(config->comp_config_addr, config->comp_config_len, type, id, "/mbn"))
  {
    MCFG_MSG_ERROR("mcfg_efs_store_config: Failed to save config into local EFS");
    return MCFG_ERR_INTERNAL;
  }

  return MCFG_ERR_NONE;
} /* mcfg_efs_store_config() */



/*===========================================================================

  FUNCTION mcfg_efs_delete_config_in_path

  DESCRIPTION
    This function delete config on EFS with specified path

  DEPENDENCIES
    None

  PARAMETERS
    path      [in]  config path
 
  RETURN VALUE
    TRUE if success, FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_efs_delete_ext
(
  char  *path,
  char  *ext
)
{
  char    ext_path[MCFG_EFS_RECORD_FILEPATH_MAX+5];
  MCFG_CHECK_NULL_PTR_RET_FALSE(path);
  
  memset(ext_path, 0, MCFG_EFS_RECORD_FILEPATH_MAX+5);
  if (strlcpy(ext_path, path, MCFG_EFS_RECORD_FILEPATH_MAX+5) != strlen(path))
  {
    MCFG_MSG_ERROR("mcfg_efs_delete_ext: Failed to get Path");
    return FALSE;
  }

  if (strlcpy(ext_path+strlen(path), ext, MCFG_EFS_RECORD_FILEPATH_MAX+5-strlen(path)) != strlen(ext))
  {
    MCFG_MSG_ERROR("mcfg_efs_delete_ext: Failed to get Path");
    return FALSE;
  }

  if (efs_unlink(ext_path) !=0 )
  {
    MCFG_MSG_SPRINTF_1("mcfg_efs_delete_ext: %s not exist", ext_path);
    return FALSE;
  }

  //MCFG_MSG_SPRINTF_1("mcfg_efs_delete_ext: delete %s", ext_path);
  return TRUE;
} /* mcfg_efs_delete_config_in_path() */

/*===========================================================================

  FUNCTION mcfg_efs_config_present_local

  DESCRIPTION
    This function checks if the config is present in local efs

  DEPENDENCIES
    None

  PARAMETERS
    type      [in]  type of the config to delete
    id        [in]  unique ID struct to fill with selected config (if NULL then
                    delete all config files of the given type)

  RETURN VALUE
    TRUE if success, FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_efs_config_present_local
(
  mcfg_config_type_e_type type,
  mcfg_config_id_s_type  *id
)
{
  char path[MCFG_EFS_RECORD_FILEPATH_MAX];
  struct fs_stat stat;

  MCFG_ASSERT_COND_RET_FALSE(type < MCFG_NUM_OF_TYPES);
  MCFG_ASSERT_COND_RET_FALSE(id);

  if (!mcfg_efs_get_config_path_ext(type, id, "/mbn", path))
  {
    MCFG_MSG_ERROR("mcfg_efs_config_present_local: Failed to get path");
    return FALSE;
  }

  if (efs_stat(path, &stat) == -1)
  {
    MCFG_MSG_SPRINTF_1("mcfg_efs_config_present_local: config not present in local efs, %s", path);
    return FALSE;
  }

  MCFG_MSG_SPRINTF_1("mcfg_efs_config_present_local: config present in local efs %s", path);
  return TRUE;
} /* mcfg_efs_config_present_local() */

/*===========================================================================

  FUNCTION mcfg_efs_delete_config_local

  DESCRIPTION
    This function deletes the config file with the specified unique ID from the
    device.

  DEPENDENCIES
    None

  PARAMETERS
    type      [in]  type of the config to delete
    id        [in]  unique ID struct to fill with selected config (if NULL then
                    delete all config files of the given type)

  RETURN VALUE
    TRUE if success, FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_efs_delete_config_local
(
  mcfg_config_type_e_type type,
  mcfg_config_id_s_type  *id,
  int                     size
)
{
  char path[MCFG_EFS_RECORD_FILEPATH_MAX];
  char ipath[MCFG_EFS_RECORD_FILEPATH_MAX+3];
  mcfg_config_s_type     config;
#ifdef FEATURE_MCFG_REMOTE_STORAGE
  boolean remote_saved = FALSE;
  int remote_size;
#endif

  MCFG_ASSERT_COND_RET_FALSE(type < MCFG_NUM_OF_TYPES);
  MCFG_ASSERT_COND_RET_FALSE(id);
  memset(&config, 0, sizeof(config));

  do
  {
#ifdef FEATURE_MCFG_REMOTE_STORAGE
    if (!mcfg_remote_accessible())
    {
      MCFG_MSG_ERROR("delete_config_local: remote not accessible");
      break;
    }
  
    if (mcfg_remote_config_stat(type, id, &remote_size))
    {
      remote_saved = TRUE;
      break;
    }
#endif

    config.config_len = size;
    if(!mcfg_utils_alloc_config_buffer(type, &config))
    {
      MCFG_MSG_ERROR_1("delete_config_local: failed to allocate buffer size %d", size);
      break;
    }
   
    if (!mcfg_utils_get_config(type, id, &config))
    {
      MCFG_MSG_ERROR("delete_config_local: failed to get config");
      break;
    }

#ifdef FEATURE_MCFG_REMOTE_STORAGE
    remote_saved = mcfg_remote_put_config(type, id, &config);
#endif
  }  while (0);

  if (config.config_addr)
  {
    mcfg_utils_free_config_buffer(&config);
  }

#ifdef FEATURE_MCFG_REMOTE_STORAGE
  if(!remote_saved)
  {
    MCFG_MSG_ERROR("delete_config_local: remote mbn not found, skip deletion");
    return FALSE;
  }
#endif

  if (!mcfg_efs_get_config_path_ext(type, id, "/mbn", path))
  {
    MCFG_MSG_ERROR("delete_config_local: Failed to get path");
    return FALSE;
  }

  if (efs_unlink(path) != 0)
  {
    MCFG_MSG_SPRINTF_1("delete_config_local: Failed to delete config, %s", path);
    return FALSE;
  }

  return TRUE;
} /* mcfg_efs_delete_config_local() */

/*===========================================================================

  FUNCTION mcfg_efs_delete_config

  DESCRIPTION
    This function deletes the config file with the specified unique ID from the
    device.

  DEPENDENCIES
    None

  PARAMETERS
    type      [in]  type of the config to delete
    id        [in]  unique ID struct to fill with selected config (if NULL then
                    delete all config files of the given type)

  RETURN VALUE
    TRUE if success, FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_efs_delete_config
(
  mcfg_config_type_e_type type,
  mcfg_config_id_s_type  *id,
  mcfg_storage_mode_e_type storage
)
{
  char path[MCFG_EFS_RECORD_FILEPATH_MAX];

  MCFG_ASSERT_COND_RET_FALSE(type < MCFG_NUM_OF_TYPES);
  MCFG_ASSERT_COND_RET_FALSE(id);

  if (!mcfg_efs_get_config_path(type, id, path))
  {
    MCFG_MSG_ERROR("mcfg_efs_delete_config: Failed to get path");
    return FALSE;
  }

#ifdef FEATURE_MCFG_REMOTE_STORAGE
  if (storage == MCFG_STORAGE_LOCAL)
  {
    //MCFG_MSG_SPRINTF_1("mcfg_efs_delete_config: Failed to delete %s\/path", path);
    if (mcfg_remote_accessible())
    {
      mcfg_remote_delete_config(type, id);
    }
  }
#endif /* FEATURE_MCFG_REMOTE_STORAGE */

  if (0 != efs_deltree(path))
  {
    MCFG_MSG_SPRINTF_1("mcfg_efs_delete_config: Failed to rmdir %s", path);
  }

  if(!mcfg_map_store_config_map(type))
  {
    MCFG_MSG_ERROR("mcfg_efs_delete_config: Failed to store map");
    return FALSE;
  }

  return TRUE;
} /* mcfg_efs_delete_config() */

/*===========================================================================

  FUNCTION mcfg_efs_add_config_local

  DESCRIPTION
    This function is used to store a new config file on the device.  

  DEPENDENCIES
    None

  PARAMETERS
    type   [in]  type of the new config
    id     [in]  ID of the new config
    config [in/out] config structure including config size and buffer pointer

  RETURN VALUE
    mcfg_error

  SIDE EFFECTS
    Config file is stored into device

===========================================================================*/
mcfg_error_e_type mcfg_efs_add_config_local
(
  mcfg_config_type_e_type type,
  mcfg_config_id_s_type  *id,
  mcfg_config_s_type     *config,
  boolean                 remote
)
{
  char path[MCFG_EFS_RECORD_FILEPATH_MAX];
  mcfg_error_e_type error = MCFG_ERR_NONE;
  mcfg_map_record_s_type record;
  boolean space_available = FALSE;

  MCFG_ASSERT_COND_RET_ERROR(type < MCFG_NUM_OF_TYPES, MCFG_ERR_NOT_SUPPORTED);
  MCFG_CHECK_NULL_PTR_RET_ERROR(id, MCFG_ERR_NULL_POINTER);
  MCFG_CHECK_NULL_PTR_RET_ERROR(config, MCFG_ERR_NULL_POINTER);
  MCFG_CHECK_NULL_PTR_RET_ERROR(config->config_addr, MCFG_ERR_NULL_POINTER);
  MCFG_ASSERT_COND_RET_ERROR(config->config_len > 0, MCFG_ERR_INVALID_PARAMETER);
  MCFG_ASSERT_COND_RET_ERROR(config->config_len <= MCFG_CONFIG_SIZE_MAX, MCFG_ERR_INVALID_PARAMETER);

  if (MCFG_ERR_NONE != mcfg_utils_space_available(type, config, FALSE))
  {
#ifdef FEATURE_MCFG_REMOTE_STORAGE
    MCFG_MSG_HIGH("add_config_local: no more local space, try to make room");
#else
    MCFG_MSG_ERROR("add_config_local: no more space");
    return MCFG_ERR_OUT_OF_SPACE;
#endif
  }
  else
  {
     space_available = TRUE;
  }

  if(!mcfg_efs_get_config_path(type, id, path))
  {
    MCFG_MSG_ERROR("add_config_local: Failed to get path for folder");
    return MCFG_ERR_ACCESS_DENIED;
  }

  error = mcfg_efs_store_config_size(type, id, config->config_len);
  if (error != MCFG_ERR_NONE)
  {
    MCFG_MSG_ERROR_1("add_config_local: Failed to store config size %d", config->config_len);
    return error;
  }

  error = mcfg_efs_store_config_info(type, id, config);
  if (error != MCFG_ERR_NONE)
  {
    MCFG_MSG_ERROR("add_config_local: Failed to store info");
    return error;
  }

  if(!mcfg_map_init_record_by_id(&record, id))
  {
    MCFG_MSG_ERROR("add_config_local: Failed to init record");
    return MCFG_ERR_INTERNAL;
  }

  record.storage = (remote) ? MCFG_STORAGE_REMOTE : MCFG_STORAGE_LOCAL;
  record.efs_present = FALSE;

  record.size = config->config_len;
#ifdef FEATURE_MCFG_REMOTE_STORAGE
  if (space_available || mcfg_map_make_room(type, record.size))
#endif
  {
    error = mcfg_efs_store_config(type, id, config);
    if (error != MCFG_ERR_NONE)
    {
#ifdef FEATURE_MCFG_REMOTE_STORAGE
      if (!remote)
      {
        MCFG_MSG_ERROR("add_config_local: Failed to store config in local and remote");
        return error;
      }
      else
      {
        MCFG_MSG_ERROR("add_config_local: config can't be saved to local, only saved to remote");
        error = MCFG_ERR_NONE;
      }
#else
      MCFG_MSG_ERROR("add_config_local: config can't be saved");
#endif
    }
    else
    {
      MCFG_MSG_LOW("add_config_local: config stored locally");
      record.efs_present = TRUE;
    }
  }
#ifdef FEATURE_MCFG_REMOTE_STORAGE
  else
  {
    MCFG_MSG_LOW("add_config_local: config only saved to remote");
  }
#endif

  if (!mcfg_map_add_record(type, &record, 0))
  {
    MCFG_MSG_ERROR("add_config_local: Failed to add record to map");
    return MCFG_ERR_INTERNAL;
  }

  if(!mcfg_map_store_config_map(type))
  {
    MCFG_MSG_ERROR("mcfg_efs_add_config: Failed to store map");
    mcfg_map_delete_record(type, &record);
    return MCFG_ERR_OUT_OF_SPACE;
  }

  return error;
} /* mcfg_efs_add_config_local() */


/*===========================================================================

  FUNCTION mcfg_efs_add_config

  DESCRIPTION
    This function is used to store a new config file on the device.  

  DEPENDENCIES
    None

  PARAMETERS
    type   [in]  type of the new config
    id     [in]  ID of the new config
    config [in/out] config structure including config size and buffer pointer

  RETURN VALUE
    mcfg_error

  SIDE EFFECTS
    Config file is stored into device

===========================================================================*/
mcfg_error_e_type mcfg_efs_add_config
(
  mcfg_config_type_e_type type,
  mcfg_config_id_s_type  *id,
  mcfg_config_s_type     *config
)
{
  char path[MCFG_EFS_RECORD_FILEPATH_MAX];
  mcfg_error_e_type error = MCFG_ERR_NONE;

  MCFG_CHECK_NULL_PTR_RET_ERROR(id, MCFG_ERR_NULL_POINTER);
  MCFG_CHECK_NULL_PTR_RET_ERROR(config, MCFG_ERR_NULL_POINTER);
  MCFG_ASSERT_COND_RET_ERROR(type < MCFG_NUM_OF_TYPES, MCFG_ERR_NOT_SUPPORTED);

  error = mcfg_auth_check_config(config);
  if (error != MCFG_ERR_NONE)
  {
    return error;
  }

#ifdef FEATURE_MCFG_REMOTE_STORAGE
  if (mcfg_remote_accessible())
  {
    /* store config in remote, no need to save default path info */
    mcfg_remote_put_config(type, id, config);
  }
#endif /* FEATURE_MCFG_REMOTE_STORAGE */

  return mcfg_efs_add_config_local(type, id, config, FALSE);
} /* mcfg_efs_add_config() */

#ifdef FEATURE_MCFG_REMOTE_STORAGE
mcfg_error_e_type mcfg_efs_copy_config_from_remote
(
  mcfg_config_type_e_type type,
  mcfg_config_id_s_type  *id,
  int                     size
)
{
  mcfg_config_s_type     config;
  mcfg_error_e_type      error;

  MCFG_CHECK_NULL_PTR_RET_ERROR(id, MCFG_ERR_NULL_POINTER);
  MCFG_ASSERT_COND_RET_ERROR(size < MCFG_CONFIG_SIZE_MAX, MCFG_ERR_INVALID_PARAMETER);

  if (!mcfg_remote_accessible())
  {
    return MCFG_ERR_INVALID_STATE;
  }

  memset(&config, 0, sizeof(config));
  config.config_len = size;

  if(!mcfg_utils_alloc_config_buffer(type, &config))
  {
    return MCFG_ERR_INTERNAL;
  }
 
  if (!mcfg_remote_get_config(type, id, &config))
  {
    error = MCFG_ERR_INTERNAL;
  }
  else
  {
    error = mcfg_efs_store_config(type, id, &config);
  }

  mcfg_utils_free_config_buffer(&config);

  return error;
} /* mcfg_efs_copy_config_from_remote() */
#endif /* FEATURE_MCFG_REMOTE_STORAGE */

boolean mcfg_efs_statvfs
(
  void *stat_vfs
)
{
  /*Check the current available free space on EFS */
  if (efs_statvfs(MCFG_EFS_PATH_META, stat_vfs) < 0)
  {
    MCFG_MSG_ERROR_1("efs_fstat on MCFG failed with efs_errno %d",efs_errno);
    return FALSE;
  }
  
  return TRUE;
}
