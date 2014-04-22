/*==============================================================================

Dynamic Loading Support Libs

Copyright (c) 2014 by QUALCOMM Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR

--------------------------------------------------------------------------------
 EDIT HISTORY FOR FILE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.

 when               who         what, where, why
 --------           ---         -----------------------------------------------
 1/22/15          amitkulk      created 
==============================================================================*/
#ifndef __SIGVERIFY_H__
#define __SIGVERIFY_H__

// return codes for SigVerify_AuthenticateELF
#define SV_SUCCESS               0
#define SV_E_FAILURE             1
#define SV_E_INVALID_PARM        2
#define SV_E_INVALID_DATA        3
#define SV_E_UNSUPPORTED         4
#define SV_E_RSA                 5
#define SV_E_HASH                6
#define SV_E_HW                  7
#define SV_E_X509                8
#define SV_E_INVALID_CERT        9
#define SV_E_INVALID_CERT_SIG    10   
#define SV_E_UNTRUSTED_ROOT      11
#define SV_E_INVALID_SIG         12
#define SV_E_INIT                17


typedef struct SigVerify SigVerify;

#ifdef __cplusplus
extern "C" {
#endif

int  SigVerify_Init                 (void);
void SigVerify_Deinit               (void);
int  SigVerify_AuthenticateELF      (const unsigned char *pELF, int cbELF);

#ifdef __cplusplus
}
#endif

#endif   // __SIGVERIFY_H__
