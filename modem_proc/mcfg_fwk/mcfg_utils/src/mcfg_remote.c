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

$Header: //components/rel/mcfg_fwk.mpss/7.10.1.1/mcfg_utils/src/mcfg_remote.c#1 $ 
$DateTime: 2020/02/19 23:07:19 $ 
$Author: pwbldsvc $ 
$Change: 22644646 $ 

when     who  what, where, why
-------- ---  ----------------------------------------------------------------
05/08/12 cy   Initial creation

==============================================================================*/


#include "comdef.h"

#include "mcfg_osal.h"


#include "mcfg_common.h"
#include "mcfg_utils.h"
#include "mcfg_utils_i.h"
#include "mcfg_auth_i.h"

#include "mcfg_map_i.h"
#include "mcfg_efs_i.h"
#include "mcfg_remote_i.h"
#include "md5.h"
#include "mcfg_nv_api.h"
#include "mcfg_multi_mbn.h"
#include "mcfg_setting.h"
#include "mcfg_uim.h"

#include "fs_estdio.h"
#ifdef FEATURE_MCFG_REMOTE_STORAGE
#include "rfs_api.h"

/*==============================================================================

                  DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/
/*==============================================================================
  Constants and Macros
==============================================================================*/
//#define DLFS_RFSA_FMT "MPSS:/%s"

/*==============================================================================
  Typedefs
==============================================================================*/

/*==============================================================================
  Variables
==============================================================================*/
static char curr_mcfg_sw_remote_digest[MCFG_DIGEST_SIZE];
static char curr_mcfg_hw_remote_digest[MCFG_DIGEST_SIZE];  
/*==============================================================================

                     INTERNAL FUNCTION DECLARATIONS

==============================================================================*/
int mcfg_remote_rfsa_open(const char* filename, unsigned int  fileMode) 
{
   return rfs_open(filename, fileMode, 0);
}

boolean mcfg_remote_rfsa_read(int fd, uint32 offset, uint32 size, uint8* data)
{
  int rfs_ret;
  fs_size_t total_bytes_read = 0;

  if (data != NULL)
  {
    rfs_ret = rfs_seek(fd, offset, RFS_SEEK_SET);
    MCFG_MSG_LOW_1("rfs_seek ret %d", rfs_ret);
    if (rfs_ret == offset)
    {
      do
      {
        rfs_ret = rfs_read(fd, (char *)(data + total_bytes_read), (size - total_bytes_read));
        MCFG_MSG_LOW_1("rfs_read ret %d", rfs_ret);
      } while (rfs_ret > 0 && (total_bytes_read += rfs_ret) < size);
    }
  }
  
  if(total_bytes_read != size)
  {
    MCFG_MSG_ERROR_2("mcfg_remote_rfsa_read %lu but end up read %lu", size, total_bytes_read);
    return FALSE;
  }

  return TRUE;
} /* mcfg_remote_rfsa_read() */

static boolean mcfg_remote_rfsa_write(int fd, uint32 offset, uint32 size, uint8* data)
{
  int rfs_ret;
  fs_size_t total_bytes_write = 0;

  if (data != NULL)
  {
    rfs_ret = rfs_seek(fd, offset, RFS_SEEK_SET);
    MCFG_MSG_LOW_1("rfs_seek ret %d", rfs_ret);
    if (rfs_ret == offset)
    {
      do
      {
        rfs_ret = rfs_write(fd, (char *)(data + total_bytes_write), (size - total_bytes_write));
        MCFG_MSG_LOW_1("rfs_write ret %d", rfs_ret);
      } while (rfs_ret > 0 && (total_bytes_write += rfs_ret) < size);
    }
  }

  if(total_bytes_write != size)
  {
    MCFG_MSG_ERROR_2("mcfg_remote_rfsa_write %lu but end up write %lu", size, total_bytes_write);
    return FALSE;
  }

  return TRUE;
} /* mcfg_remote_rfsa_write() */

/*==============================================================================
                    PUBLIC FUNCTION DECLARATIONS FOR MODULE
==============================================================================*/

/*===========================================================================

  FUNCTION mcfg_remote_add_config

  DESCRIPTION
    This function is used to add a new remote config file on the device.  

  DEPENDENCIES
    None

  PARAMETERS
    type   [in]  type of the new config
    id     [in]  ID of the new config
    config [in/out] config structure including config size and buffer pointer

  RETURN VALUE
    TRUE if success, FALSE otherwise

  SIDE EFFECTS
    Config file is stored into device

===========================================================================*/
mcfg_error_e_type mcfg_remote_add_config
(
  mcfg_config_type_e_type type,
  mcfg_config_id_s_type  *id,
  mcfg_config_s_type     *config
)
{
  char path[MCFG_UTILS_RECORD_FILEPATH_MAX];
  char path_info[MCFG_UTILS_RECORD_FILEPATH_MAX+4];
  mcfg_error_e_type error = MCFG_ERR_NONE;
  mcfg_map_record_s_type record;
  mcfg_config_s_type remote_config;
  struct rfs_stat_buf buf;
  int fd = -1 ;

  MCFG_CHECK_NULL_PTR_RET_ERROR(id, MCFG_ERR_NULL_POINTER);
  MCFG_CHECK_NULL_PTR_RET_ERROR(config, MCFG_ERR_NULL_POINTER);
  MCFG_ASSERT_COND_RET_ERROR(type < MCFG_NUM_OF_TYPES, MCFG_ERR_NOT_SUPPORTED);

  memset(&remote_config, 0, sizeof(remote_config));

  if(rfs_stat(config->config_addr, &buf) != 0)
  {
    MCFG_MSG_ERROR("mcfg_remote_add_config: Failed to stat");
    return MCFG_ERR_ACCESS_DENIED;
  }

  fd = mcfg_remote_rfsa_open(config->config_addr, RFS_O_RDONLY);
  if (fd < 0)
  {
    MCFG_MSG_ERROR("mcfg_remote_add_config: Failed to open");
    return MCFG_ERR_ACCESS_DENIED;
  }
  
  remote_config.config_len = buf.st_size;
  remote_config.config_addr = NULL;

  do
  {
    if (remote_config.config_len < 0 || 
        remote_config.config_len > MCFG_CONFIG_SIZE_MAX)
    {
      MCFG_MSG_ERROR_1("mcfg_remote_add_config: invalid flen %d", remote_config.config_len);
      error = MCFG_ERR_ACCESS_DENIED;
      break;
    }

    if(!mcfg_utils_alloc_config_buffer(type, &remote_config))
    {
      MCFG_MSG_ERROR_1("mcfg_remote_add_config: failed to malloc size %d", 
                       remote_config.config_len);
      error = MCFG_ERR_INTERNAL;
      break;
    }

    if (!mcfg_remote_rfsa_read(fd, 0, remote_config.config_len, remote_config.config_addr))
    {
      MCFG_MSG_ERROR_1("mcfg_remote_add_config: can't read len %d", remote_config.config_len);
      error = MCFG_ERR_ACCESS_DENIED;
      break;
    }

    if (!mcfg_utils_check_and_update_config(&remote_config, MCFG_CONFIG_SIZE_MAX))
    {
      MCFG_MSG_ERROR_1("mcfg_remote_add_config: failed check_and_update_config len = %d", MCFG_CONFIG_SIZE_MAX);
      error = MCFG_ERR_INTERNAL;
      break;
    }

    /* Authentication */
    error = mcfg_auth_check_config(&remote_config);
    if (error != MCFG_ERR_NONE)
    {
      error = MCFG_ERR_AUTHENTICATION_FAILED;
      break;
    }

    /* save to remote path */
    if(mcfg_efs_store_config_path(type, id, (char *)config->config_addr))
    if (error != MCFG_ERR_NONE)
    {
      error = MCFG_ERR_OUT_OF_SPACE;
      break;
    }

    /* Try to save to local EFS */
    if(mcfg_efs_add_config_local(type, id, &remote_config, TRUE))
    if (error != MCFG_ERR_NONE)
    {
      error = MCFG_ERR_OUT_OF_SPACE;
      break;
    }

    MCFG_MSG_LOW("mcfg_remote_add_config success");

  }while(0);

  if (fd >= 0)
  {
    rfs_close(fd);
  }

  if (remote_config.config_addr)
  {
    mcfg_utils_free_config_buffer(&remote_config);
  }

  return error;
} /* mcfg_remote_add_config() */

/*===========================================================================

  FUNCTION mcfg_remote_get_config

  DESCRIPTION
    This function is used to load a config file from remote into
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
boolean mcfg_remote_get_config
(
  mcfg_config_type_e_type type,
  mcfg_config_id_s_type  *id,
  mcfg_config_s_type     *config
)
{
  int fd = -1;
  char remote_path[MCFG_RFS_RECORD_FILEPATH_MAX];
  struct rfs_stat_buf stat_buf;
  int rfs_stat_result = -1;
  mcfg_config_s_type path_config;
  mcfg_error_e_type error = MCFG_ERR_NONE;
  /*size after uncompression*/
  uint32 uncompressed_len = 0;

  MCFG_CHECK_NULL_PTR_RET_FALSE(id);
  MCFG_CHECK_NULL_PTR_RET_FALSE(config);
  MCFG_CHECK_NULL_PTR_RET_FALSE(config->config_addr);
  MCFG_ASSERT_COND_RET_FALSE(type < MCFG_NUM_OF_TYPES);

  if(!mcfg_remote_accessible())
  {
    return FALSE;
  }

  do
  {
    memset(remote_path, 0, sizeof(remote_path));
    if (!mcfg_efs_get_config_size(type, id, &config->config_len))
    {
      MCFG_MSG_ERROR("mcfg_remote_get_config: Failed to get size");
      error = MCFG_ERR_INTERNAL;
      break;
    }

    if (!mcfg_efs_get_config_remote_path(type, id, remote_path))
    {
      MCFG_MSG_ERROR("mcfg_remote_get_config: Failed to get remote path");
      error = MCFG_ERR_INTERNAL;
      break;
    }

    fd = mcfg_remote_rfsa_open(remote_path, RFS_O_RDONLY);
    if (fd < 0)
    {
      MCFG_MSG_ERROR("mcfg_remote_get_config: Failed to open");
      MCFG_MSG_SPRINTF_1("remote file %s", remote_path);
      error = MCFG_ERR_INTERNAL;
      break;
    }

    rfs_stat_result = rfs_stat(remote_path, &stat_buf);
    if(rfs_stat_result < 0)
    {
       MCFG_MSG_ERROR_1("mcfg_remote_get_config: rfs_stat on MCFG failed with errno %d",rfs_stat_result);
       error = MCFG_ERR_INTERNAL;
       break;
    }

    if(stat_buf.st_size >  config->config_len)
    {
       MCFG_MSG_ERROR_1("mcfg_remote_get_config: unexpected stat_buf.st_size %lu",  stat_buf.st_size);
       error = MCFG_ERR_INTERNAL;
       break;
    }

    if(!mcfg_remote_rfsa_read(fd, 0, stat_buf.st_size, config->config_addr))
    {
      MCFG_MSG_ERROR_1("mcfg_remote_get_config: read failed %lu", stat_buf.st_size);
      error = MCFG_ERR_ACCESS_DENIED;
      break;
    }

    uncompressed_len = config->config_len;

    if(stat_buf.st_size < config->config_len)
    {
      MCFG_MSG_MED_1("mcfg_remote_get_config: compresses config len %lu", stat_buf.st_size);
      config->config_len = stat_buf.st_size;
    }

    if(!mcfg_utils_check_and_update_config(config, uncompressed_len))
    {
      error = MCFG_ERR_INTERNAL;
      break;
    }

    MCFG_MSG_LOW("mcfg_remote_get_config success");
  }while(0);

  if (fd >= 0)
  {
    rfs_close(fd);
  }

  return (error == MCFG_ERR_NONE);
} /* mcfg_remote_get_config() */

/*===========================================================================

  FUNCTION mcfg_remote_store_buf_in_path

  DESCRIPTION
    This function stores data buf into RFS with specified path

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
boolean mcfg_remote_store_buf_in_path
(
  void *buf,
  int   len,
  char *path
)
{
  int fd = -1;
  int rfs_ret;

  MCFG_CHECK_NULL_PTR_RET_FALSE(buf);
  MCFG_CHECK_NULL_PTR_RET_FALSE(path);
  MCFG_ASSERT_COND_RET_FALSE(len <= MCFG_CONFIG_SIZE_MAX);

  fd = mcfg_remote_rfsa_open(path, RFS_O_RDWR|RFS_O_TRUNC|RFS_O_CREAT);
  if (fd < 0)
  {
     MCFG_MSG_ERROR_1("Unable to create/open MCFG Image file (%d)",fd);
     return FALSE;
  }
  if (!mcfg_remote_rfsa_write(fd, 0, len, buf))
  {
     MCFG_MSG_ERROR("mcfg_remote_store_buf_in_path: rfs store failed");
     rfs_close(fd);
     return FALSE;
  }
  rfs_close(fd);

  return TRUE;
} /* mcfg_remote_store_buf_in_path() */

/*===========================================================================

  FUNCTION mcfg_remote_delete_config_in_path

  DESCRIPTION
    This function delete config on RFS with specified path

  DEPENDENCIES
    None

  PARAMETERS
    path      [in]  config path
 
  RETURN VALUE
    TRUE if success, FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_remote_delete_config_in_path
(
  char  *path
)
{
  int err = 0;
  MCFG_CHECK_NULL_PTR_RET_FALSE(path);

  //MCFG_MSG_SPRINTF_1("mcfg_remote_delete_config_in_path: %s", path);
  err = rfs_unlink(path);
  if ( err!=0 )
  {
    MCFG_MSG_ERROR_1("mcfg_remote_delete_config_in_path: failed to delete config err %d", err);
    MCFG_MSG_SPRINTF_1("path: %s", path);
    return FALSE;
  }

  return TRUE;
} /* mcfg_remote_delete_config_in_path() */

/*===========================================================================

  FUNCTION mcfg_remote_get_config_path

  DESCRIPTION
    This function translates the config type and ID into the rfs path which
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
boolean mcfg_remote_get_config_path
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

  //offset = snprintf(path, MCFG_RFS_RECORD_FILEPATH_MAX, "%s/cfg_%02d/", MCFG_RFS_PATH_META, type);
  offset = snprintf(path, MCFG_RFS_RECORD_FILEPATH_MAX, "%s/", MCFG_RFS_PATH_META);
  //MCFG_MSG_LOW_1("mcfg_remote_get_config_path offset=%d", offset);
  if (offset <= 0 || !mcfg_remote_get_filename(
        type, id, path+offset, MCFG_RFS_RECORD_FILEPATH_MAX-offset))
  {
    return FALSE;
  }

  //MCFG_MSG_SPRINTF_1("mcfg_remote_get_config_path %s", path);
  return TRUE;
} /* mcfg_remote_get_config_path() */

/*===========================================================================

  FUNCTION mcfg_remote_put_config

  DESCRIPTION
    This function translates the config type and ID into the rfs path which
    stores the config file

  DEPENDENCIES
    None

  PARAMETERS
    type      [in]  type of ID to query
    id        [in]  unique ID struct
    config    [in/out] config structure including config size and buffer pointer
 
  RETURN VALUE
    TRUE if success, FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_remote_put_config
(
  mcfg_config_type_e_type type,
  mcfg_config_id_s_type  *id,
  mcfg_config_s_type     *config
)
{
  char path[MCFG_RFS_RECORD_FILEPATH_MAX];
  mcfg_error_e_type error = MCFG_ERR_NONE;
  void *buf;
  int   len;

  MCFG_CHECK_NULL_PTR_RET_FALSE(id);
  MCFG_CHECK_NULL_PTR_RET_FALSE(config);
  MCFG_ASSERT_COND_RET_FALSE(type < MCFG_NUM_OF_TYPES);

  if (mcfg_remote_accessible() == FALSE)
  {
     MCFG_MSG_ERROR("mcfg_remote_put_config failed...rfs not accessible!");
     return FALSE;
  }
  //Query: if the config with id is already stored , do We need to still store the config again ...or we can return with TRUE from here.
  //if (mcfg_remote_config_exists(type, id)) 
  //{
  //   MCFG_MSG_LOW("mcfg_remote_put_config: config already exists with id");
  //   return TRUE;
  //}

  //error = mcfg_auth_check_config(config);
  //if (error != MCFG_ERR_NONE)
  //{
  //  return FALSE;
  //}

  if (!mcfg_remote_get_config_path(type, id, path))
  {
    MCFG_MSG_ERROR("mcfg_remote_put_config: Failed to get Path");
    return FALSE;
  }

  MCFG_MSG_SPRINTF_1("mcfg_remote_put_config %s", path);
  if(config->comp_config_len != 0 && config->comp_config_addr !=NULL)
  {
    buf = config->comp_config_addr;
    len = config->comp_config_len;
  }
  else
  {
    buf = config->config_addr;
    len = config->config_len;
    MCFG_MSG_ERROR("mcfg_remote_put_config: check why config uncompressed");
  }
  
  if (!mcfg_remote_store_buf_in_path(buf, len, path))
  {
    MCFG_MSG_ERROR("mcfg_remote_put_config: Failed to store config");
    return FALSE;
  }

  return TRUE;
}/*mcfg_remote_put_config()*/

/*===========================================================================

  FUNCTION mcfg_remote_delete_config

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
boolean mcfg_remote_delete_config
(
  mcfg_config_type_e_type type,
  mcfg_config_id_s_type  *id
)
{
  char path[MCFG_RFS_RECORD_FILEPATH_MAX];

  MCFG_ASSERT_COND_RET_FALSE(type < MCFG_NUM_OF_TYPES);
  MCFG_ASSERT_COND_RET_FALSE(id);

  if (mcfg_remote_accessible() == FALSE)
  {
     MCFG_MSG_ERROR("mcfg_remote_delete_config failed...rfs not accessible!");
     return FALSE;
  }

  if (!mcfg_remote_config_exists(type, id))
  {
    MCFG_MSG_ERROR("mcfg_remote_delete_config: Failed to get Path");
    return FALSE;
  }

  if (!mcfg_remote_get_config_path(type, id, path))
  {
    MCFG_MSG_ERROR("mcfg_remote_put_config: Failed to get Path");
    return FALSE;
  }

  if (!mcfg_remote_delete_config_in_path(path))
  {
    MCFG_MSG_SPRINTF_1("mcfg_remote_delete_config: Failed to delete config, %s", path);
    return FALSE;
  }

  return TRUE;
}
/*===========================================================================

  FUNCTION mcfg_remote_config_exists

  DESCRIPTION
    This function checks if the config exists or not in RFS.

  DEPENDENCIES
    None

  PARAMETERS
    type      [in]  type of the config to delete
    id        [in]  unique ID struct to fill with selected config.
    
 
  RETURN VALUE
    TRUE if success, FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_remote_config_exists
(
  mcfg_config_type_e_type type,
  mcfg_config_id_s_type  *id
)
{
  char path[MCFG_RFS_RECORD_FILEPATH_MAX];

  MCFG_ASSERT_COND_RET_FALSE(type < MCFG_NUM_OF_TYPES);
  MCFG_ASSERT_COND_RET_FALSE(id);

  if (mcfg_remote_accessible() == FALSE)
  {
     MCFG_MSG_ERROR("mcfg_remote_delete_config failed...rfs not accessible!");
     return FALSE;
  }

  if (!mcfg_remote_get_config_path(type, id, path))
  {
    MCFG_MSG_ERROR("mcfg_remote_config_exists: Failed to get Path");
    return FALSE;
  }
  return TRUE;
}
/*===========================================================================

  FUNCTION mcfg_remote_config_stat

  DESCRIPTION
    This function is used for remote config stat for a given
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
boolean mcfg_remote_config_stat
(
  mcfg_config_type_e_type type,
  mcfg_config_id_s_type  *id,
  int                    *size
)
{
  struct rfs_stat_buf stat_buf;
  char path[MCFG_RFS_RECORD_FILEPATH_MAX];
  int rfs_stat_result = -1;

  MCFG_ASSERT_COND_RET_FALSE(type < MCFG_NUM_OF_TYPES);
  MCFG_ASSERT_COND_RET_FALSE(id);
  MCFG_ASSERT_COND_RET_FALSE(size);
  memset(path, 0, sizeof(path));

  if (mcfg_remote_accessible() == FALSE)
  {
     MCFG_MSG_ERROR("mcfg_remote_config_stat: rfs not accessible!");
     return FALSE;
  }

  if (!mcfg_efs_get_config_remote_path(type, id, path))
  {
    MCFG_MSG_ERROR("mcfg_remote_config_stat: Failed to get rfs config Path");
    return FALSE;
  }

  rfs_stat_result = rfs_stat(path, &stat_buf);
  if (rfs_stat_result < 0)
  {
     MCFG_MSG_ERROR_1("rfs_stat on MCFG failed with errno %d",rfs_stat_result);
     return FALSE;
  }

  *size = stat_buf.st_size;

  return TRUE;
}
/*===========================================================================

  FUNCTION mcfg_remote_get_filename

  DESCRIPTION
    This function translates the config ID into the rfs file name which
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
boolean mcfg_remote_get_filename
(
  mcfg_config_type_e_type type,
  mcfg_config_id_s_type  *id,
  char                   *filename,
  size_t                  size
)
{
  MCFG_CHECK_NULL_PTR_RET_FALSE(id);
  MCFG_CHECK_NULL_PTR_RET_FALSE(filename);
  MCFG_ASSERT_COND_RET_FALSE(type < MCFG_NUM_OF_TYPES);

  if (mcfg_remote_accessible() == FALSE)
  {
     MCFG_MSG_ERROR("mcfg_remote_get_filename failed...rfs not accessible!");
     return FALSE;
  }
  MD5_CTX hash_context;
  uint8 md5sum[16];
  int i, offset = 0;

  if(type == MCFG_TYPE_HW &&
     memcmp(id->id, MCFG_RFS_DEFAULT_ID_HW, id->id_len) == 0)
  {
     if(strlcpy(filename, MCFG_EFS_DEFAULT_FILE_HW, size) ==
           strlen(MCFG_RFS_DEFAULT_FILE_HW))
     {
       return TRUE;
     }
     else
     {
       MCFG_MSG_ERROR("mcfg_remote_get_filename failed");
       return FALSE;
     }
  }
  else if (type == MCFG_TYPE_SW &&
     memcmp(id->id, MCFG_RFS_DEFAULT_ID_SW, id->id_len) == 0)
  {
    if(strlcpy(filename, MCFG_RFS_DEFAULT_FILE_SW, size) ==
         strlen(MCFG_RFS_DEFAULT_FILE_SW))
    {
      return TRUE;
    }
    else
    {
      MCFG_MSG_ERROR("mcfg_remote_get_filename failed");
      return FALSE;
    }
  }

  memset(filename, 0, MCFG_RFS_RECORD_FILENAME_MAX);

  /* Initialize md5 hash calculation. */
  MD5Init(&hash_context);

  /* Perform hash calculation for unique config id. */
  MD5Update(&hash_context, id->id, id->id_len);

  /* Copy the md5sum value to filename  */
  MD5Final((unsigned char *)md5sum, &hash_context);

  switch(type)
  {
    case MCFG_TYPE_HW:
      offset = snprintf(filename, MCFG_RFS_RECORD_FILENAME_MAX, "%s", MCFG_RFS_FILE_HW_PREFIX);
      break;

    case MCFG_TYPE_SW:
      offset = snprintf(filename,MCFG_RFS_RECORD_FILENAME_MAX, "%s", MCFG_RFS_FILE_SW_PREFIX);
      break;

    default:
      MCFG_MSG_ERROR_1("mcfg_remote_get_filename: type %d not supported", type);
      return FALSE;
  }

  for (i=0; i<16; i++)
  {
    if (snprintf(filename+offset,MCFG_RFS_RECORD_FILENAME_MAX, "%02X", 
                 md5sum[i]) != 2)
    {
      MCFG_MSG_ERROR_1("mcfg_remote_get_filename: snprintf failed %d", i);
      return FALSE;
    }

    offset += 2;
  }
  
  //MCFG_MSG_SPRINTF_1("mcfg_remote_get_config_filename %s", filename);

  return TRUE;
}
/*===========================================================================

  FUNCTION mcfg_remote_accessible

  DESCRIPTION
    This function checks whether rfs is accessible or not.

  DEPENDENCIES
    None

  PARAMETERS
    None
 
  RETURN VALUE
    TRUE if success, FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_remote_accessible
(
  void
)
{
  static boolean mcfg_remote_accessible = FALSE;
  int ret = 0;
  
  if (!mcfg_task_bootup_completed())
  {
     MCFG_MSG_ERROR("mcfg_remote_accessible: MCFG task is not fully booted up! ");
     return FALSE;
  }
  
  if (mcfg_remote_accessible)
  {
     //MCFG_MSG_LOW("already mcfg_remote_accessible!No need to query again...");
     return TRUE;
  }
 
  //try to create if not present initially.
  ret = rfs_put(MCFG_RFS_PATH_TEST, &mcfg_remote_accessible, sizeof(mcfg_remote_accessible), RFS_O_RDWR|RFS_O_CREAT, 0);
  if (ret != sizeof(mcfg_remote_accessible))
  {
     MCFG_MSG_ERROR("mcfg_remote_accessible: Failed to save test file.");
  }
  else
  {
    rfs_unlink(MCFG_RFS_PATH_TEST);
    mcfg_remote_accessible=TRUE;
  }
  return mcfg_remote_accessible;
}/*mcfg_remote_accessible()*/

/*===========================================================================

  FUNCTION mcfg_remote_customized_list_file

  DESCRIPTION
    This function checks whether oem provided list file is present or not.

  DEPENDENCIES
    None

  PARAMETERS
    None
 
  RETURN VALUE
    TRUE if success, FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_remote_customized_list_file
(
  const char* filepath,
  uint8 current_mcfg_type
)
{
   int fd = -1;
   char cust_path_info[MCFG_RFS_RECORD_FILENAME_MAX];

   MCFG_MSG_LOW("inside mcfg_remote_customized_list_file...");
   if (snprintf(cust_path_info, MCFG_RFS_RECORD_FILENAME_MAX, "%s%s", filepath, 
                  (current_mcfg_type == MCFG_TYPE_HW) ? MCFG_RFS_CUST_MBN_HW_FILENAME: MCFG_RFS_CUST_MBN_SW_FILENAME) < 0)
   {
     MCFG_MSG_LOW_1("mcfg_remote_customized_list_file: Failed to get cust_path_info for cfg type %d ", current_mcfg_type);
     return FALSE;
   }

   fd = mcfg_remote_rfsa_open(cust_path_info, RFS_O_RDONLY);
   if (fd < 0)
   {
      MCFG_MSG_LOW_1("mcfg_remote_customized_list_file: Failed to open file for cust_path_info with err_no: %d", fd);
      return FALSE;
   }
   MCFG_MSG_SPRINTF_1("mcfg_remote_customized_list_file: cust_path_info is %s ", cust_path_info);
   rfs_close(fd);
   return TRUE;

}/*mcfg_remote_customized_list_file()*/

/*===========================================================================

  FUNCTION mcfg_remote_update_required

  DESCRIPTION
    This function compares the rfs digest version.

  DEPENDENCIES
    None

  PARAMETERS
    None
 
  RETURN VALUE
    TRUE if success, FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_remote_update_required
(
  const char* filepath,
  uint8 current_mcfg_type
)
{
   int fd = -1;
   char prev_mcfg_remote_digest[MCFG_DIGEST_SIZE];
   char digest_path_info[MCFG_RFS_RECORD_FILENAME_MAX];
   boolean ret = FALSE;
   
   MCFG_MSG_LOW("inside mcfg_remote_update_required");

   if (MCFG_NV_STATUS_OK != mcfg_nv_read((current_mcfg_type == MCFG_TYPE_HW) ? MCFG_NV_ITEM_MCFG_RFS_HW_DIGEST_VERSION: 
                                         MCFG_NV_ITEM_MCFG_RFS_SW_DIGEST_VERSION,
                                         0, MCFG_SUB_ID_FIRST, prev_mcfg_remote_digest, MCFG_DIGEST_SIZE))
   {
      MCFG_MSG_LOW_1("mcfg_remote_update_required: Failed to read remote digest version for cfg type %d ", current_mcfg_type);
      //return FALSE;
   }
    MCFG_MSG_LOW("inside mcfg_remote_update_required after reading prev rfs digest version");
   if (snprintf(digest_path_info, MCFG_RFS_RECORD_FILENAME_MAX, "%s%s", filepath, 
                   (current_mcfg_type == MCFG_TYPE_HW) ? MCFG_RFS_MBN_HW_DIGEST_FILENAME: MCFG_RFS_MBN_SW_DIGEST_FILENAME) < 0)
   {
      MCFG_MSG_ERROR_1("mcfg_remote_update_required: Failed to get remote path_info for cfg type %d ", current_mcfg_type);
      return FALSE;
   }
   MCFG_MSG_SPRINTF_1("digest_path_info: %s", digest_path_info);
   fd = mcfg_remote_rfsa_open(digest_path_info, RFS_O_RDONLY);
   if (fd < 0)
   {
      MCFG_MSG_ERROR_1("mcfg_remote_update_required: Failed to open file with err_no: %d", fd);
      return FALSE;
   }

   do
   {
     MCFG_MSG_SPRINTF_1("mcfg_remote_update_required: success in opening the file : %s", digest_path_info);
     if (!mcfg_remote_rfsa_read(fd, 0, MCFG_DIGEST_SIZE, 
                                      (current_mcfg_type == MCFG_TYPE_HW) ? (uint8 *)curr_mcfg_hw_remote_digest: (uint8 *)curr_mcfg_sw_remote_digest))
     {
       MCFG_MSG_ERROR_1("mcfg_remote_update_required: can't read len %d", MCFG_DIGEST_SIZE);
       break;
     }
     MCFG_MSG_SPRINTF_1("mcfg_remote_update_required: curr_mcfg_remote_digest after reading file : %s", (current_mcfg_type == MCFG_TYPE_HW) ? 
                                                                            (uint8 *)curr_mcfg_hw_remote_digest: (uint8 *)curr_mcfg_sw_remote_digest);
     if (memcmp(prev_mcfg_remote_digest, (current_mcfg_type == MCFG_TYPE_HW) ? 
                                         (uint8 *)curr_mcfg_hw_remote_digest: (uint8 *)curr_mcfg_sw_remote_digest, MCFG_DIGEST_SIZE) == 0)
     {
        MCFG_MSG_MED_1("prev and curr remote digest version is same for cfg_type : (%d)!", current_mcfg_type);
        break;
     }
     MCFG_MSG_SPRINTF_2("prev_mcfg_remote_digest: %s and curr_mcfg_remote_digest : %s", prev_mcfg_remote_digest, (current_mcfg_type == MCFG_TYPE_HW) ? 
                                                                              (uint8 *)curr_mcfg_hw_remote_digest: (uint8 *)curr_mcfg_sw_remote_digest);
    
     MCFG_MSG_LOW("mcfg_remote_update_required: success");
     ret = TRUE;
   }while(0);

   if (fd >= 0)
   {
     rfs_close(fd);
   }
   return ret;
}/*mcfg_remote_update_required()*/


/*===========================================================================

  FUNCTION mcfg_remote_discover

  DESCRIPTION
    This function checks for rfs discovery.

  DEPENDENCIES
    None

  PARAMETERS
    None
 
  RETURN VALUE
    TRUE if success, FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_remote_discover
(
  const char* filepath
)
{
  int fd = -1;
  struct rfs_stat_buf stat;
  struct rfs_stat_buf stat_mbn;
  char path_info[MCFG_RFS_RECORD_FILENAME_MAX];
  char line[MCFG_RFS_RECORD_FILENAME_MAX];
  char updated_line[MCFG_RFS_RECORD_FILENAME_MAX];
  mcfg_config_s_type config;
  uint16 line_offset = 0;
  uint8 current_mcfg_type;
  mcfg_slot_index_type_e_type sloti;
  mcfg_uim_cmd_payload_s_type uim_cmd_payload;
  boolean status = TRUE;

  MCFG_CHECK_NULL_PTR_RET_FALSE(filepath);

  do
  {
   if (!mcfg_remote_accessible())
   {
     MCFG_MSG_HIGH("mcfg_remote_discover failed...rfs not accessible!");
     status = FALSE;
     break;
   } 

   if (!mcfg_setting_remote_discover())
   {
    MCFG_MSG_HIGH("mcfg_remote_discover feature disabled");
    status = FALSE;
    break;
   } 

   for(current_mcfg_type=MCFG_TYPE_HW; current_mcfg_type <= MCFG_TYPE_SW; current_mcfg_type++ )
   {
      line_offset = 0;
      //comapare digest version...
      if (!mcfg_remote_update_required(filepath, current_mcfg_type))
      {
         MCFG_MSG_LOW_1("mcfg_remote_discover: update not required for cfg type %d ", current_mcfg_type);
         continue;
      }

     //if version is different , then delete all the remote mbn's...else just return
      if (MCFG_ERR_NONE != mcfg_utils_delete_all(current_mcfg_type, MCFG_STORAGE_MASK_REMOTE))
      {
        MCFG_MSG_LOW_1("unable to delete existing inactive configs for cfg type %d ", current_mcfg_type);
        continue;
      }

      if (mcfg_remote_customized_list_file(filepath, current_mcfg_type))
      {
         if (snprintf(path_info, MCFG_RFS_RECORD_FILENAME_MAX, "%s%s", filepath, 
                      (current_mcfg_type == MCFG_TYPE_HW) ? MCFG_RFS_CUST_MBN_HW_FILENAME: MCFG_RFS_CUST_MBN_SW_FILENAME) < 0)
         {
           MCFG_MSG_ERROR_1("mcfg_remote_discover: Failed to get cust_path_info for cfg type %d ", current_mcfg_type);
           status = FALSE;
           break;
         }
      }
      else
      {
         if (snprintf(path_info, MCFG_RFS_RECORD_FILENAME_MAX, "%s%s", filepath, 
                      (current_mcfg_type == MCFG_TYPE_HW) ? MCFG_RFS_MBN_HW_FILENAME: MCFG_RFS_MBN_SW_FILENAME) < 0)
         {
           MCFG_MSG_ERROR_1("mcfg_remote_discover: Failed to get path_info for cfg type %d ", current_mcfg_type);
           status = FALSE;
           break;
         }
      }

     //Need to revisit here again.
     MCFG_MSG_SPRINTF_1("mcfg_remote_discover: path_info is %s ", path_info);
     fd = mcfg_remote_rfsa_open(path_info, RFS_O_RDONLY);
     if (fd < 0)
     {
        MCFG_MSG_ERROR_1("mcfg_remote_discover: Failed to open file for path_info with err_no: %d", fd);
        status = FALSE;
        break;
     }
     else
     {
        if (rfs_stat(path_info, &stat) < 0)
        {
           MCFG_MSG_ERROR("mcfg_remote_discover: Unable to fstat");
           rfs_close(fd); 
           status = FALSE;
           break;
        }
        MCFG_MSG_LOW_1("mcfg_remote_discover: stat.st_size is %d ", stat.st_size);
         char* data_from_path_info;//Need to revisit here...
         data_from_path_info = mcfg_malloc(stat.st_size+1);
         if (!data_from_path_info)
         {
           rfs_close(fd);
           status = FALSE;
           break;
         }
         //Read the remote txt file into data.
         if (!mcfg_remote_rfsa_read(fd, 0, stat.st_size, (uint8 *)data_from_path_info))
         {
           MCFG_MSG_ERROR_1("mcfg_remote_discover: can't read len %lu", stat.st_size);
           mcfg_free(data_from_path_info); 
           status = FALSE;
           break;
         }
         rfs_close(fd);
         data_from_path_info[stat.st_size] = '\0';

        //Read line by line and add the config using mcfg_multi_mbn_add_config routine.
        MCFG_MSG_HIGH_2("mcfg_remote_discover: line_offset before while loop starts is %d  and stat.st_size is %lu ", line_offset, stat.st_size);
        while (line_offset < stat.st_size && mcfg_utils_read_one_line(data_from_path_info, stat.st_size+1, &line_offset, line, MCFG_RFS_RECORD_FILENAME_MAX-1))
        {
           MCFG_MSG_SPRINTF_2("mcfg_remote_discover: line read is : %s and  and line_offset is %d :", line, line_offset);
           if (snprintf(updated_line, MCFG_RFS_RECORD_FILENAME_MAX, "%s%c%s", filepath, '/', line) < 0)
           {
             MCFG_MSG_ERROR_1("mcfg_remote_discover: Failed to get updated_line for cfg type %d ", current_mcfg_type);
             continue;
           }
           MCFG_MSG_SPRINTF_1("mcfg_remote_discover: updated_line is : %s ", updated_line); 
           config.config_len =  strlen(updated_line);   
           config.config_addr = updated_line ;    
           mcfg_multi_mbn_add_config(current_mcfg_type, &config, MCFG_STORAGE_REMOTE);

           mcfg_task_dog_hb_report();
        }
        
         mcfg_free(data_from_path_info); 
     }
     
      //Update the remote digest version with curr_mcfg_remote_digest.
      if (MCFG_NV_STATUS_OK != mcfg_nv_write((current_mcfg_type == MCFG_TYPE_HW) ? MCFG_NV_ITEM_MCFG_RFS_HW_DIGEST_VERSION: 
                                            MCFG_NV_ITEM_MCFG_RFS_SW_DIGEST_VERSION,
                                            0, MCFG_SUB_ID_FIRST, 
                                            (current_mcfg_type == MCFG_TYPE_HW) ? (uint8 *)curr_mcfg_hw_remote_digest: (uint8 *)curr_mcfg_sw_remote_digest, 
                                            MCFG_DIGEST_SIZE))
      {
         MCFG_MSG_ERROR_1("mcfg_remote_update_required: Failed to write remote digest version for cfg type %d ", current_mcfg_type);
      }
      MCFG_MSG_HIGH_1("mcfg_remote_discover: success in update for cfg type %d ", current_mcfg_type);
   }
  } while(0);

  if(status)
  {
    MCFG_MSG_HIGH("mcfg_remote_discover: success ");
  }

  if(status)
  {
    if(mcfg_utils_selected_config_available(MCFG_TYPE_HW, 0))
    {
      mcfg_utils_activate_config(MCFG_TYPE_HW, MCFG_ACTIVATION_BY_PDC);
    }
    else
    {
      for (sloti = MCFG_SLOT_INDEX_FIRST; sloti < MCFG_NUM_OF_SLOT_INDEX; sloti++)
      {
        if (mcfg_uim_nv_autoselect_enabled(sloti))
        {
          uim_cmd_payload.sloti = sloti;
          mcfg_uim_cmd_send(MCFG_UIM_CMD_RESELECT, &uim_cmd_payload);
        }
      }
    }
  }

  return status;
}/*mcfg_remote_discover()*/

#endif /* FEATURE_MCFG_REMOTE_STORAGE */

