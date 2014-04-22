#ifndef QMALLOC_H
#define QMALLOC_H
/**
  @file qmalloc.h
  @brief Externalized functions for the qmalloc library.

*/
/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The Utility_Services_API_mainpage.dox file contains all file/group 
      descriptions that are in the output PDF generated using Doxygen and 
      Latex. To edit or update any of the file/group text in the PDF, edit 
      the Utility_Services_API_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the "utils_memory" 
	  group description in the Utility_Services_API_mainpage.dox file. 
===========================================================================*/
/*===========================================================================
                              <qmalloc.h>
DESCRIPTION
  Externalised functions for the qmalloc library

Copyright (c) 2005-2011 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary. 
===========================================================================*/

/*===========================================================================
                             Edit History

$Header: //components/rel/core.tx/6.0/api/services/qmalloc.h#1 $ $DateTime: 2020/01/30 22:49:35 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/15/11   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
11/17/10   EBR     Doxygenated File.
09/09/05   TJH	   Created
===========================================================================*/
 

/*===========================================================================
                 Includes and Public Data Declarations
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*===========================================================================
  Constant / Define Declarations
===========================================================================*/

/*===========================================================================
  Type Declarations
===========================================================================*/

/*===========================================================================
  Global Constant Data Declarations 
===========================================================================*/

/*===========================================================================
                            Macro Definitions
===========================================================================*/

/*===========================================================================
                          Function Declarations
===========================================================================*/
/** @addtogroup utils_memory
@{ */

#ifdef __cplusplus /* If this is reused in a C++ environment */
  extern "C" {
#endif


/**
  Allocates memory from the local heap if AMSS is not running, 
  or from the BREW<sup>&reg;</sup> heap once AMSS is started.
 
  @param[in] size Amount of memory to allocate.

  @return
  Returns a pointer to the area of memory allocated, or NULL is no memory
  area is available.
 
  @dependencies
  None.
*/
void *q_malloc (uint32 size);


/**
  Frees memory from the BREW heap. Any memory allocated from q_heap 
  is left allocated.
 
  @param[in] ptr Pointer to the memory to be freed.

  @return
  None.

  @dependencies
  None.
*/
void q_free (void *ptr);
      
#ifdef __cplusplus /* If this is reused in a C++ environment */
  }
#endif

/** @} */ /* end_addtogroup utils_memory */
#endif /* Q_MALLOC_H */
