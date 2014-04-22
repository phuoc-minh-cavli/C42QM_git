#ifndef GTP_QSP_PROTOCOL_H
#define GTP_QSP_PROTOCOL_H

/*===========================================================================

  GTP QSP PROTOCOLS Header File

  DESCRIPTION
  This header file contains definitions of data structure and interfaces
  for GTP QSP implementation.

  Copyright (c) 2023 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

===========================================================================*/
#ifdef FEATURE_GTP_QSP
//#include <loc_util_log.h>
#include "comdef.h"    /* Definition for basic types and macros */
#include "GtpProtocol.h"
//#include "GtpWwanClient.h"
//#include "gps_extended_c.h"
#include <vector>
#include <string>

#include <fs_lib.h>
#include "loc_publicencrypt.h"




#define QSP_MAX_KEY_LEN				32 /* 256 bits */
#define QSP_KEYCRYPT_RSA_OEAP		1
#define QSP_PLCRYPT_AES256_GCM		1



using namespace std;
using namespace izat_gtpwwan;
class GtpWwanClient;

namespace izat_gtpwwan{

/* Data Structures */

typedef struct {
	uint8	payloadEncryptMethod; /* encyption method of payload encryption*/
	uint8_t	keyEncryptMethod; /* encryption used to encrypt session key */
} gtpQspSessInfo;



class GtpQSPProtocol {


	/*constructor and destructor */
	public:
	    GtpQSPProtocol(GtpProtocol* gtpProtocol, GtpWwanClient* client);
	    virtual ~GtpQSPProtocol();
	private:
		uint8 *mPublicKey;
		size_t mKeyLength;
		uint8  mSessionAesKey[QSP_MAX_KEY_LEN]; /* Session Key */
		uint8  mMessageID; /* message ID of the session */
		uint8  mNextMessageID;
		GtpProtocol* mGtpProtocol;
		loc_SharkSslAesGcmCtx *mAesGcmCtx;
		loc_SharkSslAesParams mAesParam;
	public:
		/*
		 * Read a file from EFS
		 *
		 * @param  name with absolute path of cert file
		 * @param cert address of array of binary data
		 * @param size size of array
		 * @return QAPI_OK on success
		 */
		int readCertFromEFS(const char * name);
		int cryptoServicesInitialize();
		int QSPSessionInit(gtpQspSessInfo *sessInfo);
		int QSPSessionEncode();
		int QSPSessionDecode();
	    int RSAOEAPEncryptData(	uint8_t * in_buf,	size_t	in_buf_size,	uint8_t ** out_buf,	size_t	*  out_buf_size);
		 int AESGCMDecryptData(			uint8_t * in_buf, size_t in_buf_size, 
		 	uint8_t *out_buf);
		 int AESGCMEncryptData(uint8_t * in_buf, size_t in_buf_size, 
		 	uint8_t *out_buf);

	
};
}//end of namespace
#endif // FEATURE_GTP_QSP
#endif /* GTP_QSP_PROTOCOL_H */

