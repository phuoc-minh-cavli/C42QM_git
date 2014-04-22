/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                ERR Serfices NV Items using EFS API Interface Header File

GENERAL DESCRIPTION
  This file contains the structure definitions and function prototypes for 
  the ERR Non-Volatile Memory interface.  

DEPENDENCIES
  dnt_nv_efs

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  

Copyright (c) 2010-2017 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

Version Control

  $Header: //components/rel/core.tx/6.0/debugtools/err/src/err_nv_efs.h#1 $
  $DateTime: 2020/01/30 22:49:35 $
  $Author: pwbldsvc $ 

=============================================================================

                           EDIT HISTORY FOR FILE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/17/11   mcg     plagiarizing sm_nv_efs
05/14/10   gk      Initial Release.

=============================================================================

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#ifndef ERR_NV_EFS_H
#define ERR_NV_EFS_H

/*
 * Include files
*/

#include "comdef.h"

#include "erri.h"
#include "dnt_nv_efs.h"

/* All the NV EFS values are initialzed here */
#define DNT_NV_EFS_ERR_FATAL_ACTION_DEFAULT             (ERR_DEFAULT_ACTION)
#define DNT_NV_EFS_ERR_MPROC_TIMEOUT_VALUE_DEFAULT      (0)

#if defined (FEATURE_DLOAD_HW_RESET_DETECT)
  #define DNT_NV_EFS_ERR_HW_RESET_DETECT_ENABLE_DEFAULT   (TRUE)
#else
  #define DNT_NV_EFS_ERR_HW_RESET_DETECT_ENABLE_DEFAULT   (FALSE)
#endif

/*
 * Structure definitions
*/


/* these are the SM related NV items. Add other NV items below this. All SM items are in
   SM/ directory, this is a subset of cgps_nv_efs_reg_item_type pertaining to SM */

/* This should be max number of SM NV items in the table below */
extern dnt_nv_efs_reg_item_struct dnt_nv_efs_err_reg_items[];


/*===========================================================================

FUNCTION err_nv_efs_init

DESCRIPTION
  This function is used to initialize internal ERR-NV data efs structure.
  
RETURN VALUE
  None
  
DEPENDENCIES
 
===========================================================================*/
void err_nv_efs_init (void);

/*===========================================================================

FUNCTION err_nv_efs_reg_item_read

DESCRIPTION
  This function is used to read NV item. If the read fails, it sets it to
  default value.
  
RETURN VALUE
  None
  
DEPENDENCIES
 
===========================================================================*/
boolean err_nv_efs_reg_item_read(dnt_nv_efs_reg_item_type const item, 
                                uint32 data_size,
                                void *data_ptr
                                );

#if 0 //not yet needed or supported
/*===========================================================================

FUNCTION err_nv_efs_reg_item_write

DESCRIPTION
  This function is used to write an NV item. 
 
  
RETURN VALUE
  None
  
DEPENDENCIES
 
===========================================================================*/
boolean err_nv_efs_reg_item_write(dnt_nv_efs_reg_item_type const item, 
                                uint8 data_size,
                                void *data_ptr
                                );
#endif 


#endif /* ERR_NV_EFS_H */
