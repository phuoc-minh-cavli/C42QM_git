#ifndef MCFG_REMOTE_I_H
#define MCFG_REMOTE_I_H
/*==============================================================================

                M O D E M   C O N F I G   E F S
 
GENERAL DESCRIPTION
  Internal header for MCFG EFS Storage

Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/mcfg_fwk.mpss/7.10.1.1/mcfg_utils/src/mcfg_remote_i.h#1 $ 
$DateTime: 2020/02/19 23:07:19 $ 
$Author: pwbldsvc $ 
$Change: 22644646 $ 

when     who  what, where, why
-------- ---  ----------------------------------------------------------------
05/08/12 cy   Initial creation

==============================================================================*/
#include "list.h"

#include "mcfg_common.h"
#include "mcfg_utils.h"
#include "mcfg_remote.h"

/*==============================================================================

                PRIVATE DEFINITIONS AND DECLARATIONS FOR MODULE

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
 
                    PRIVATE FUNCTION DECLARATIONS FOR MODULE
 
==============================================================================*/

/*==============================================================================

                PUBLIC DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/
/*==============================================================================
  Constants and Macros
==============================================================================*/
#define MCFG_RFS_PATH_META       "/readwrite"
#define MCFG_RFS_PATH_TEST       "/readwrite/mcfg.tmp"
#define MCFG_RFS_DEFAULT_FILE_HW   "mcfg_hw.mbn"
#define MCFG_RFS_DEFAULT_FILE_SW   "mcfg_sw.mbn"
#define MCFG_RFS_DEFAULT_ID_HW     MCFG_RFS_DEFAULT_FILE_HW
#define MCFG_RFS_DEFAULT_ID_SW     MCFG_RFS_DEFAULT_FILE_SW
#define MCFG_RFS_FILE_HW_PREFIX    "mcfg_hw"
#define MCFG_RFS_FILE_SW_PREFIX    "mcfg_sw"
#define MCFG_RFS_MBN_SW_FILENAME   "/mcfg_sw/mbn_sw.txt"
#define MCFG_RFS_MBN_HW_FILENAME   "/mcfg_hw/mbn_hw.txt"
#define MCFG_RFS_CUST_MBN_SW_FILENAME   "/mcfg_sw/oem_sw.txt"
#define MCFG_RFS_CUST_MBN_HW_FILENAME   "/mcfg_hw/oem_hw.txt"
#define MCFG_RFS_MBN_SW_DIGEST_FILENAME   "/mcfg_sw/mbn_sw.dig"
#define MCFG_RFS_MBN_HW_DIGEST_FILENAME   "/mcfg_hw/mbn_hw.dig"

#define MCFG_EFS_MBN_SW_FILENAME   "/mcfg/mbn_sw.txt"
#define MCFG_EFS_MBN_HW_FILENAME   "/mcfg/mbn_hw.txt"

#define MCFG_RFS_RECORD_FILENAME_MAX MCFG_UTILS_RECORD_FILENAME_MAX
#define MCFG_RFS_RECORD_FILEPATH_MAX (MCFG_RFS_RECORD_FILENAME_MAX + 20)


/*==============================================================================
  Typedefs
==============================================================================*/


/*==============================================================================
  Variables
==============================================================================*/


/*============================================================================== 
 
                    PUBLIC FUNCTION DECLARATIONS FOR MODULE
 
==============================================================================*/

#ifdef FEATURE_MCFG_REMOTE_STORAGE
/*===========================================================================

  FUNCTION mcfg_remote_add_config

  DESCRIPTION
    This function is used to store a new config file on the device.  

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
);

/*===========================================================================

  FUNCTION mcfg_remote_get_config

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
boolean mcfg_remote_get_config
(
  mcfg_config_type_e_type type,
  mcfg_config_id_s_type  *id,
  mcfg_config_s_type     *config
);


/*===========================================================================

  FUNCTION mcfg_remote_get_config_info

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
boolean mcfg_remote_get_config_info
(
  mcfg_config_type_e_type  type,
  mcfg_config_id_s_type   *id,
  mcfg_config_info_s_type *info
);

/*===========================================================================

  FUNCTION mcfg_remote_get_config_size

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
boolean mcfg_remote_get_config_size
(
  mcfg_config_type_e_type type,
  mcfg_config_id_s_type  *id,
  int                    *size
);

boolean mcfg_remote_get_config_path
(
  mcfg_config_type_e_type type,
  mcfg_config_id_s_type  *id,
  char                   *path
);

boolean mcfg_remote_delete_config
(
  mcfg_config_type_e_type type,
  mcfg_config_id_s_type  *id
);

boolean mcfg_remote_config_exists
(
  mcfg_config_type_e_type type,
  mcfg_config_id_s_type  *id
);

boolean mcfg_remote_config_stat
(
  mcfg_config_type_e_type type,
  mcfg_config_id_s_type  *id,
  int                    *size
);

boolean mcfg_remote_get_filename
(
  mcfg_config_type_e_type type,
  mcfg_config_id_s_type  *id,
  char                   *filename,
  size_t                  size
);

boolean mcfg_remote_accessible
(
  void
);

boolean mcfg_remote_discover
(
  const char* filepath
);

int mcfg_remote_rfsa_open(const char* filename, unsigned int  fileMode);
boolean mcfg_remote_rfsa_read(int fd, uint32 offset, uint32 size, uint8* data);
#endif

#endif /* MCFG_REMOTE_I_H */
