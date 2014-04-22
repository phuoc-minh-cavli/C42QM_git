#ifndef HEAP_CONFIG_H
#define HEAP_CONFIG_H

/**
  @file assert.h
  @brief Definitions for ASSERT macro.

*/
/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The Utility_Services_API_mainpage.dox file contains all file/group 
      descriptions that are in the output PDF generated using Doxygen and 
      Latex. To edit or update any of the file/group text in the PDF, edit 
      the Utility_Services_API_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the "utils_assert" group 
      description in the Utility_Services_API_mainpage.dox file. 
===========================================================================*/

/*==========================================================================

            A S S E R T   S E R V I C E S   H E A D E R   F I L E

DESCRIPTION
  Definitions for ASSERT macro.
  
Copyright (c) 1998-2011 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$PVCSPath: L:/src/asw/COMMON/vcs/assert.h_v   1.0   Aug 21 2000 11:51:02   lpetrucc  $
$Header: //components/rel/core.tx/6.0/api/services/heap_config.h#1 $ $DateTime: 2020/01/30 22:49:35 $ $Author: pwbldsvc $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/22/11   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
11/12/10   EBR     Doxygenated File.
04/27/04    gr     Added comment to suppress LINT error.
12/15/98    ms     Removed spurious tabs.
02/19/98   jct     Created

===========================================================================*/


/*===========================================================================
 
                            INCLUDE FILES

===========================================================================*/


/*===========================================================================
 
                            CONSTANT DEFINITIONS

===========================================================================*/
typedef enum
{
  HEAP_TYPE_RTOS,
  HEAP_TYPE_MEMHEAP,
}heap_type_enum;

typedef struct
{
 heap_type_enum heap_type;
 void* heap_start_address;
 uint32 heap_size;
}heap_config_type;

#endif /* HEAP_CONFIG_H */

