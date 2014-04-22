#ifndef SECDIAG_H
#define SECDIAG_H
/*==========================================================================

                   Diagnostic Packet Definitions for DRM

  Description: Definitions of DRM Diag packets. These packets constitute the
  diagnostic interface between DRM and the external device.

Copyright (c) 2006 - 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*==========================================================================

  $Header: //components/rel/dataiot.mpss/2.0/protocols/sec/ssl/testapp/src/secdiagsvc.h#1 $
  $DateTime: 2020/03/17 08:03:16 $
  $Author: pwbldsvc $

when         who     what, where, why
--------     ---     -------------------------------------------------------
04/29/14     nk      ECC time test
08/01/13     nk      AES256 support in secapi interface
03/27/13     rk      Added fuzzing test cases
05/13/12     ak      Smetest case enhancements
04/30/12     nk      ZUC algorithm ported. 
02/08/10     wa      Added support for IxContentHandler tests
11/20/09     tk      Added labeling test.
09/15/09     ejt     Added DIAG_SECTESTAPP_ROM_LITE_RANDOM_NUM_START
 
===========================================================================*/

#include "comdef.h"
#include "diagcmd.h"
#include "diagpkt.h"
#include "sectestapp.h"
/*
 * Types used by the SEC Diag code. Note that the values of existing elements
 * in the enumerated types defined below should not be changed. Adding new
 * elements at the end is acceptable.
 */

typedef enum
{
  SEC_IMAGE_MODEM_PROC_ID = 1,
  SEC_IMAGE_APP_PROC_ID = 2 
}SEC_IMAGE_PROC_ID;

/*
 * Types of command data for SEC subsystem.
 */
typedef enum
{
  SUBSYS_CMD_MODEM_SECTEST = 1,
  SUBSYS_CMD_APP_SECTEST = 2,
} SUBSYS_CMD_SEC_E;

/*
 * Error codes. We define our own rather than reusing the error codes in
 * IxErrno.h because Diag error code values cannot change once they have
 * been published. 
 */
typedef enum
{
  SEC_DIAG_E_SUCCESS       =  0,    /* Operation succeeded                 */
  SEC_DIAG_E_FAILURE       =  1,    /* Operation failed due to unknown err */
  SEC_DIAG_E_NOT_ALLOWED   =  2,    /* Operation currently not allowed     */
  SEC_DIAG_E_NOT_SUPPORTED =  4,    /* Operation not yet implemented       */
  SEC_DIAG_E_NO_MEMORY     = 15,    /* Memory unavailable                  */
  SEC_DIAG_E_INVALID_ARG   = 16,    /* Argument is not recognized          */
  SEC_DIAG_E_NO_ENTRY      = 41     /* No entry exists */
} SECDIAG_ERR_CODE_E;

/*
 * Command codes. These must start at 0 and be consecutive.
 */
typedef enum
{
  DIAG_SECTESTAPP_SSL_HTTP_START                = 1,        /* Start an SSL HTTP get/response  */ 
  DIAG_SECTESTAPP_SSL_FUNC_START                = 2,        /* Start an SSL functional test.   */
  DIAG_SECTESTAPP_SSL_MULTIPLE_START            = 3,        /* Start an SSL multi-session test.*/
  DIAG_SECTESTAPP_SSL_LONG_START                = 4,        /* Start an SSL large data test.   */
  DIAG_SECTESTAPP_SSL_CONCURRENT_START          = 5,        /* Start concurrent session test   */
  
  DIAG_SECTESTAPP_API_PKX_START                 = 32,       /* Start an API PKX test series    */
  DIAG_SECTESTAPP_API_ENC_START                 = 33,       /* Start an API cipher test series */
  DIAG_SECTESTAPP_API_HSH_START                 = 34,       /* Start an API hash test series   */
  DIAG_SECTESTAPP_CRYPTO_START                  = 35,       /* Start crypto-specific tests     */
  DIAG_SECTESTAPP_CE_START                      = 36,       /* Start a ce realted test cases   */
  
  DIAG_SECTESTAPP_REMOTE_FS_START               = 37,       /* Start a Remote FS test          */
  DIAG_SECTESTAPP_SFS_START                     = 38,       /* Start a SFS test                */
  DIAG_SECTESTAPP_SECURE_TAR_START              = 39,       /* Start a secure tar test         */
  DIAG_SECTESTAPP_SECURE_CHANNEL_START          = 40,       /* Start a secure channel test     */
  DIAG_SECTESTAPP_HW_PRNG_START                 = 41,       /* Start a hardware test           */
  DIAG_SECTESTAPP_HW_KEY_START                  = 42,       /* Start a hardware NULL Key test  */
  
  DIAG_SECTESTAPP_AES128_SW_TIMETEST_START      = 64,       /* Run AES 128 Time tests          */
  DIAG_SECTESTAPP_HSH_HMAC_SW_TIMETEST_START    = 65,       /* Run HSH/HMAC Time tests         */
  DIAG_SECTESTAPP_PKX_TIMETEST_START            = 66,       /* Run PKX Time tests              */
  
  DIAG_SECTESTAPP_AES128_CE_TIMETEST_START      = 67,       /* Run AES 128 Time tests          */
  DIAG_SECTESTAPP_HSH_HMAC_CE_TIMETEST_START    = 68,       /* Run HSH/HMAC Time tests         */
  DIAG_SECTESTAPP_ZUC_TIMETEST_START            = 69,       /* Run ZUC Time tests              */
  DIAG_SECTESTAPP_SNOW3G_TIMETEST_START         = 70,       /* Run SNOW3G Time tests           */
  DIAG_SECTESTAPP_AES256_CE_TIMETEST_START      = 71,       /* Run AES 256 Time tests          */  
  DIAG_SECTESTAPP_AES256_SW_TIMETEST_START      = 72,       /* Run AES 256 Time tests          */ 
  DIAG_SECTESTAPP_KASUMI_TIMETEST_START         = 73,       /* Run KASUMI Time tests          */
  DIAG_SECTESTAPP_ECC_TIMETEST_START            = 74,       /* Run ECC Time tests             */

#if SECTESTAPP_CE_FUZZ
  DIAG_SECTESTAPP_FUZZ_TEST_START               = 90,       /* Run FUZZ Tests */
#endif /*SECTESTAPP_CE_FUZZ*/
#ifdef FEATURE_SEC_DRM_RDB_UNIT_TEST
  DIAG_SECTESTAPP_SECDRM_ADD_RIGHT              = 128,      /* Add a DRM right                 */
  DIAG_SECTESTAPP_SECDRM_LOOKUP_RIGHTS          = 129,      /* Lookup associated DRM rights    */
  DIAG_SECTESTAPP_SECDRM_CONSUME_RIGHT          = 130,      /* Consume DRM rights              */
  DIAG_SECTESTAPP_SECDRM_DEL_ASSO_RIGHTS        = 131,      /* Delete associated DRM rights    */
  DIAG_SECTESTAPP_SECDRM_DELETE_RIGHT           = 132,      /* Delete DRM right                */
#endif
 
#ifdef FEATURE_SME_DRMOMA
  DIAG_SECTESTAPP_IXCONTENTHANDLER_TEST_START   = 150,     /* Run QXAPI_test_IxContentHandler */
#endif
 
  DIAG_SECTESTAPP_SFS_TEST1_START               = 192,     /* Run SFS Test                    */

  DIAG_SECTESTAPP_ROM_LITE_CHIPSET_API_START    = 200,     /* Run ROM Lite Security Chipset API Test */
  DIAG_SECTESTAPP_ROM_LITE_ISI_MESSAGE_START    = 201,     /* Run ROM Lite Security ISI Message Test  */
  DIAG_SECTESTAPP_ROM_LITE_ISA_SEC_SERVER_START = 202,     /* Run ROM Lite ISA Security       */
  DIAG_SECTESTAPP_ROM_LITE_RANDOM_NUM_START     = 203,     /* Run Crypto random number test   */
  DIAG_SECTESTAPP_ROM_LITE_LABEL                = 204,     /* Run LABEL test                  */
  DIAG_SECTESTAPP_SFP_RUN_TEST                  = 205,      /* RUN SFP test                    */
  DIAG_SECTESTAPP_XPU_CFG_START                 = 206       /* RUN xpu configuration validation test */
} DIAG_SECTESTAPP_E;

/*
 * Query packet: cmd_code = SEC_DIAG_SECTEST
 *
 * Request
 * =======
 * hdr:             Required subsystem header.
 * protocolVersion: Protocol version of incoming packet.
 * op:              Specific operation requested. Supported operations are
 *                  listed in the DRMDiagQueryE enum.
 * opDataLen:       Length of data to follow.
 * opData:          Data needed for the operation. The number of bytes
 *                  of data is specified in the preceding dataLen field.
 * Response
 * ========
 * status:          Overall status.
 * resp_data_len:   Response Data Length (in bytes).
 * resp_data:       Response Data.
 * 
 */
typedef PACKED struct {
  diagpkt_subsys_header_type hdr;
  DIAG_SECTESTAPP_E          cmd;
  uint8                      data[1];    //Placeholder for data 
} SECDiagTestReqT;

typedef PACKED struct {
  diagpkt_subsys_header_type hdr;
  int32                      status;
  uint32                     resp_data_len;
  uint8                      resp_data[1];  //Placeholder for data 
} SECDiagTestRspT;

/*===========================================================================

FUNCTION SECDiagInit
DESCRIPTION
  Initializes the SEC Diag layer. This function must be called first before
  any SEC Diag packet can be handled. Returns 0 on success and a non-zero
  number on failure.
============================================================================*/
int SECDiagInit (SEC_IMAGE_PROC_ID proc_id);
#endif /* DRMDIAG_H */

