#ifndef MCFG_MULIT_MBN_PROC_H
#define MCFG_MULIT_MBN_PROC_H
/*==============================================================================

                M O D E M   C O N F I G   M U L T I   M B N

GENERAL DESCRIPTION
  Module for embedded multi MBN processing.

Copyright (c) 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/mcfg_fwk.mpss/7.10.1.1/mcfg_proc/inc/mcfg_multi_mbn.h#1 $
$DateTime: 2020/02/19 23:07:19 $
$Author: pwbldsvc $
$Change: 22644646 $

when     who  what, where, why
-------- ---  ----------------------------------------------------------------
09/11/15 cys   Modify module
09/04/15 ap    Initial creation
==============================================================================*/

#include "mcfg_common.h"

/*==============================================================================

                PUBLIC DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/
mcfg_error_e_type mcfg_multi_mbn_proc(boolean);

mcfg_error_e_type mcfg_multi_mbn_add_config
(
  mcfg_config_type_e_type  type,
  mcfg_config_s_type      *config,
  mcfg_storage_mode_e_type storage 
);

#endif /* MCFG_MULIT_MBN_PROC_H */
