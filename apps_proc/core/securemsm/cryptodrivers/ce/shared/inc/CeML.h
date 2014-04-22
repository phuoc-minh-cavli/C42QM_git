#ifndef _CeML
#define _CeML

/*===========================================================================

                    Crypto Engine Module API

GENERAL DESCRIPTION


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2000-2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                      EDIT HISTORY FOR FILE

 $Header: //components/rel/core.tx/6.0/securemsm/cryptodrivers/ce/shared/inc/CeML.h#1 $
 $DateTime: 2020/01/30 22:49:35 $
 $Author: pwbldsvc $ 

===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "comdef.h"

/*===========================================================================
                 DEFINITIONS AND TYPE DECLARATIONS
===========================================================================*/

#define CEML_HASH_DIGEST_SIZE_SHA1    20
#define CEML_HASH_DIGEST_SIZE_SHA256  32

typedef enum
{
  CEML_ERROR_SUCCESS                = 0x0,
  CEML_ERROR_FAILURE                = 0x1,
  CEML_ERROR_INVALID_PARAM          = 0x2,
  CEML_ERROR_NOT_SUPPORTED          = 0x3
} CeMLErrorType;

typedef enum
{
  CEML_HASH_ALGO_SHA1               = 0x1,
  CEML_HASH_ALGO_SHA256             = 0x2
} CeMLHashAlgoType;

typedef struct 
{
  void                              *pvBase; 
  uint32                            dwLen;  
}CEMLIovecType;

typedef struct 
{
  CEMLIovecType                     *iov;                 
  uint32                            size;                 
}CEMLIovecListType;

typedef struct 
{
  void * pClientCtxt;
} CeMLCntxHandle;

typedef enum {
  CE_HIGH_BW = 0,
  CE_MEDIUM_BW = 1,
  CE_LOW_BW = 2,
  CE_MAX_BW = 3
}CeMLClkBwType;

/**
 * @brief    This function enable the crypto clocks and request for bandwidth.  
 *
 * @param freq [in]One of valid options from CeMLClkBwType
 * @param req  [in]TRUE for enable and FALSE for disable 
 *
 * @return 0 if successful, negative if failure.
 *
 *
 */
int CeMLSetBandwidth(CeMLClkBwType freq, boolean req);

/**
 * @brief This function initializes the CE
 *
 * @param void
 *
 * @return CeMLErrorType
 *
 * @see 
 *
 */

CeMLErrorType 
CeMLInit            (void);

/**
 * @brief This function deinitializes the CE
 *
 * @param void
 *
 * @return CeMLErrorType
 *
 * @return CeMLErrorType
 *
 * @see 
 */

CeMLErrorType 
CeMLDeInit          (void);

/**
 * @brief Intialize a hash context for Hash update and final functions
 *
 * @param _h      [in] Pointer to a pointer to the hash context
 * @param pAlgo   [in] Algorithm type
 *
 * @return CeMLErrorType
 *
 * @see CeMLHashUpdate and CeMLHashfinal
 *
 */

CeMLErrorType 
CeMLHashInit        (CeMLCntxHandle       ** _h, 
                     CeMLHashAlgoType     pAlgo);

/**
 * @brief Deintialize a hash context  
 *
 * @param _h      [in] Pointer to a pointer to the hash context
 *
 * @return CeMLErrorType
 *
 * @see CeMLDeInit
 *
 */

CeMLErrorType 
CeMLHashDeInit      (CeMLCntxHandle       ** _h);

/**
 * @brief This function will hash data into the hash context
 *        structure, which must have been initialized by
 *        CeMLHashInit.
 *
 * @param _h          [in] Pointer to Hash context
 * @param ioVecIn     [in] Pointer to input message to be
 *                     hashed
 * @return CeMLErrorType 
 *
 * @see CeMLHashInit 
 *
 */

CeMLErrorType 
CeMLHashUpdate      (CeMLCntxHandle       * _h, 
                     CEMLIovecListType    ioVecIn); 

/**
 * @brief Compute the final digest hash value.
 *
 * @param _h         [in] Pointer to Hash context
 * @param ioVecOut   [out] Pointer to output digest
 
 * @return CeMLErrorType 
 *
 * @see CeMLHashInit
 *
 */

CeMLErrorType 
CeMLHashFinal       (CeMLCntxHandle       * _h,
                     CEMLIovecListType    * ioVecOut);

/**
 * @brief This function will hash data into the hash context
 *        structure and compute the final digest hash value.
 *
 * @param _h       [in] Pointer to Hash context 
 * @param ioVecIn  [in] Pointer to input message to be hashed 
 * @param ioVecOut [Out] Pointer to output digest
 *
 * @return CeMLErrorType  
 *
 * @see CeMLHashInit
 *
 */

CeMLErrorType 
CeMLHashAtomic      (CeMLCntxHandle       * _h, 
                     CEMLIovecListType    ioVecIn,
                     CEMLIovecListType    * ioVecOut);

#endif //_CeML


