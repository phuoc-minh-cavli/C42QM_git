#ifndef SLIM_QMILOC_PROVIDER_H
#define SLIM_QMILOC_PROVIDER_H
/**
  @file
  @brief QMI-LOC Provider Factory Interface.

  @ingroup slim_QMILOCProvider
*/
/*============================================================================
               Copyright (c) 2013-2015 QUALCOMM Atheros, Inc.
               All Rights Reserved.
               Qualcomm Atheros Confidential and Proprietary
============================================================================*/
/* $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/slim/provider/qmiloc/inc/slim_qmiloc_provider.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "slim_internal.h"
#include "slim_provider_data.h"

#ifdef __cplusplus
extern "C" {
#endif

//! @addtogroup slim_QMILOCProvider
//! @{

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/
/**
@brief Initializes QMI_LOC provider module.

Function initializes QMI_LOC provider module.

@return Pointer to the QMI_LOC provider implementation.
*/
slim_ServiceProviderInterfaceType *slim_QmiLocInitialize
(
  void
);

//! @}

#ifdef __cplusplus
}
#endif

#endif /* #ifndef SLIM_QMILOC_PROVIDER_H */
