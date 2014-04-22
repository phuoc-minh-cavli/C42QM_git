/*!
  @file
  lte_rrc_mem_heap_test.cpp

  @brief
  REQUIRED one-sentence brief description of this C module.

  @detail
  OPTIONAL detailed description of this C module.
  - DELETE this section if unused.

*/

/*===========================================================================

  Copyright (c) 2008 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/rrc_mem_heap/lte_rrc_mem_heap_test.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/05/08   da     Initial Revision
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

extern "C"
{
#include "lte_static_rrc_mem_util.h"
#include "lte_rrc_cfg.h"
}

#include "TestFramework.h"
#include "lte_rrc_utf.h"
#include "lte_rrc_utfi.h"


/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief Typedef for variables internal to module lte_rrc_mem_heap_test.cpp
*/
typedef struct
{
  int   internal_var;  /*!< My internal variable */
  void *internal_ptr;  /*!< My internal pointer */
} lte_rrc_mem_heap_test_s;

/*===========================================================================

                         LOCAL VARIABLES

===========================================================================*/

/*! @brief Variables internal to module lte_rrc_mem_heap_test.cpp
*/


/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/



/*===========================================================================

                                FUNCTIONS

===========================================================================*/


/*===========================================================================

                                UNIT TEST

===========================================================================*/

/*===========================================================================
  LteRrcMemHeapTest1
===========================================================================*/
/*!
  @brief
  Tests RRC heap malloc\Free\Calloc etc...

  @detail
  The following is being tested
  - UTF setup and teardown have no memory leaks
*/
/*=========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMemHeapTest1);

void LteRrcMemHeapTest1::Setup()
{

}

void LteRrcMemHeapTest1::Test()
{

  void *buf_ptr = NULL;

  lte_rrc_mem_init_heap();

  // -----------------------------------------------------------------------
  // Allocate memory of size 1024 bytes from RRC Big heap using calloc  
  TF_MSG("\n --- Using lte_rrc_mem_calloc() to allocate 0 initialized chunk of 1024.");
  buf_ptr = lte_rrc_mem_calloc(1024);
  TF_ASSERT(buf_ptr != NULL);


  // -----------------------------------------------------------------------
  // Resize memory to double the size
  TF_MSG("\n --- Using lte_rrc_mem_realloc to resize allocated chunk to 2048.");
  buf_ptr = lte_rrc_mem_realloc(buf_ptr, 2048);
  TF_ASSERT(buf_ptr != NULL);

  // -----------------------------------------------------------------------
  // Free the allocated chunk  
  lte_rrc_mem_free(buf_ptr); 

  // -----------------------------------------------------------------------
  // Allocate more memory than available - 60 K is max RRC heap so allocate
  // 60k+1
  TF_MSG("\n --- Allocate more memory than availabe - Catch EF.");
#ifdef FEATURE_LTE_RRC_USE_RRC_HEAP
  TF_CATCH_FATAL(lte_rrc_mem_calloc((1024*60)+1));
#endif

  // -----------------------------------------------------------------------
  // Allocate memory of size 63 bytes from RRC small heap using calloc
  TF_MSG("\n --- Using lte_rrc_mem_calloc to allocate a chunk of 63 bytes - Should come from small heap.");
  buf_ptr = lte_rrc_mem_calloc(63);
  TF_ASSERT(buf_ptr != NULL);

  // -----------------------------------------------------------------------
  // Free the allocated chunk  
  lte_rrc_mem_free(buf_ptr);


  // -----------------------------------------------------------------------
  // Allocate memory of size 1024 bytes from RRC sib heap
  
  TF_MSG("\n --- Allocate 1024 bytes from RRC SIB heap.");
  buf_ptr = lte_rrc_sib_mem_calloc(1024);
  TF_ASSERT(buf_ptr != NULL);

  // -----------------------------------------------------------------------
  // Free the allocated chunk  
  lte_rrc_mem_free(buf_ptr);

}

void LteRrcMemHeapTest1::Teardown()
{
}


