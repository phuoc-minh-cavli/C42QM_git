#pragma once
// AUTOGENERATED FILE: DO NOT EDIT

#include <stdint.h>
#include "object.h"

// Copyright (c) 2018 Qualcomm Technologies, Inc.  
// All Rights Reserved.
// Qualcomm Technologies Proprietary and Confidential.

#define ISignatureOperation_OP_update 0
#define ISignatureOperation_OP_sign 1
#define ISignatureOperation_OP_verify 2

static inline int32_t
ISignatureOperation_release(Object self)
{
  return Object_invoke(self, Object_OP_release, 0, 0);
}

static inline int32_t
ISignatureOperation_retain(Object self)
{
  return Object_invoke(self, Object_OP_retain, 0, 0);
}

static inline int32_t
ISignatureOperation_update(Object self, const uint8_t *input_ptr, size_t input_len, const uint8_t *inParamsCBOR_ptr, size_t inParamsCBOR_len, uint32_t *inputConsumed_ptr)
{
  ObjectArg a[3];
  a[0].bi = (ObjectBufIn) { input_ptr, input_len * sizeof(uint8_t) };
  a[1].bi = (ObjectBufIn) { inParamsCBOR_ptr, inParamsCBOR_len * sizeof(uint8_t) };
  a[2].b = (ObjectBuf) { inputConsumed_ptr, sizeof(uint32_t) };

  return Object_invoke(self, ISignatureOperation_OP_update, a, ObjectCounts_pack(2, 1, 0, 0));
}

static inline int32_t
ISignatureOperation_sign(Object self, const uint8_t *input_ptr, size_t input_len, const uint8_t *inParamsCBOR_ptr, size_t inParamsCBOR_len, uint8_t *signature_ptr, size_t signature_len, size_t *signature_lenout)
{
  ObjectArg a[3];
  a[0].bi = (ObjectBufIn) { input_ptr, input_len * sizeof(uint8_t) };
  a[1].bi = (ObjectBufIn) { inParamsCBOR_ptr, inParamsCBOR_len * sizeof(uint8_t) };
  a[2].b = (ObjectBuf) { signature_ptr, signature_len * sizeof(uint8_t) };

  int32_t result = Object_invoke(self, ISignatureOperation_OP_sign, a, ObjectCounts_pack(2, 1, 0, 0));

  *signature_lenout = a[2].b.size / sizeof(uint8_t);

  return result;
}

static inline int32_t
ISignatureOperation_verify(Object self, const uint8_t *input_ptr, size_t input_len, const uint8_t *inParamsCBOR_ptr, size_t inParamsCBOR_len, const uint8_t *signature_ptr, size_t signature_len, int32_t *valid_ptr)
{
  ObjectArg a[4];
  a[0].bi = (ObjectBufIn) { input_ptr, input_len * sizeof(uint8_t) };
  a[1].bi = (ObjectBufIn) { inParamsCBOR_ptr, inParamsCBOR_len * sizeof(uint8_t) };
  a[2].bi = (ObjectBufIn) { signature_ptr, signature_len * sizeof(uint8_t) };
  a[3].b = (ObjectBuf) { valid_ptr, sizeof(int32_t) };

  return Object_invoke(self, ISignatureOperation_OP_verify, a, ObjectCounts_pack(3, 1, 0, 0));
}


#define ICipherOperation_OP_update 0
#define ICipherOperation_OP_finish 1

static inline int32_t
ICipherOperation_release(Object self)
{
  return Object_invoke(self, Object_OP_release, 0, 0);
}

static inline int32_t
ICipherOperation_retain(Object self)
{
  return Object_invoke(self, Object_OP_retain, 0, 0);
}

static inline int32_t
ICipherOperation_update(Object self, const uint8_t *input_ptr, size_t input_len, const uint8_t *inParamsCBOR_ptr, size_t inParamsCBOR_len, uint8_t *output_ptr, size_t output_len, size_t *output_lenout, uint32_t *inputConsumed_ptr)
{
  ObjectArg a[4];
  a[0].bi = (ObjectBufIn) { input_ptr, input_len * sizeof(uint8_t) };
  a[1].bi = (ObjectBufIn) { inParamsCBOR_ptr, inParamsCBOR_len * sizeof(uint8_t) };
  a[2].b = (ObjectBuf) { output_ptr, output_len * sizeof(uint8_t) };
  a[3].b = (ObjectBuf) { inputConsumed_ptr, sizeof(uint32_t) };

  int32_t result = Object_invoke(self, ICipherOperation_OP_update, a, ObjectCounts_pack(2, 2, 0, 0));

  *output_lenout = a[2].b.size / sizeof(uint8_t);

  return result;
}

static inline int32_t
ICipherOperation_finish(Object self, const uint8_t *input_ptr, size_t input_len, const uint8_t *inParamsCBOR_ptr, size_t inParamsCBOR_len, uint8_t *output_ptr, size_t output_len, size_t *output_lenout)
{
  ObjectArg a[3];
  a[0].bi = (ObjectBufIn) { input_ptr, input_len * sizeof(uint8_t) };
  a[1].bi = (ObjectBufIn) { inParamsCBOR_ptr, inParamsCBOR_len * sizeof(uint8_t) };
  a[2].b = (ObjectBuf) { output_ptr, output_len * sizeof(uint8_t) };

  int32_t result = Object_invoke(self, ICipherOperation_OP_finish, a, ObjectCounts_pack(2, 1, 0, 0));

  *output_lenout = a[2].b.size / sizeof(uint8_t);

  return result;
}


#define IKeymasterApp_ALGORITHM_RSA UINT32_C(1)
#define IKeymasterApp_ALGORITHM_ECC UINT32_C(3)
#define IKeymasterApp_ALGORITHM_AES UINT32_C(32)
#define IKeymasterApp_ALGORITHM_HMAC UINT32_C(128)
#define IKeymasterApp_KEY_PURPOSE_SIGN UINT32_C(2)
#define IKeymasterApp_KEY_PURPOSE_VERIFY UINT32_C(3)
#define IKeymasterApp_KEY_PURPOSE_ENCRYPT UINT32_C(0)
#define IKeymasterApp_KEY_PURPOSE_DECRYPT UINT32_C(1)
#define IKeymasterApp_DIGEST_NONE UINT32_C(0)
#define IKeymasterApp_DIGEST_MD5 UINT32_C(1)
#define IKeymasterApp_DIGEST_SHA1 UINT32_C(2)
#define IKeymasterApp_DIGEST_SHA_2_224 UINT32_C(3)
#define IKeymasterApp_DIGEST_SHA_2_256 UINT32_C(4)
#define IKeymasterApp_DIGEST_SHA_2_384 UINT32_C(5)
#define IKeymasterApp_DIGEST_SHA_2_512 UINT32_C(6)
#define IKeymasterApp_PAD_NONE UINT32_C(1)
#define IKeymasterApp_PAD_RSA_OAEP UINT32_C(2)
#define IKeymasterApp_PAD_RSA_PSS UINT32_C(3)
#define IKeymasterApp_PAD_RSA_PKCS1_1_5_ENCRYPT UINT32_C(4)
#define IKeymasterApp_PAD_RSA_PKCS1_1_5_SIGN UINT32_C(5)
#define IKeymasterApp_PAD_PKCS7 UINT32_C(64)
#define IKeymasterApp_BLOCK_MODE_ECB UINT32_C(1)
#define IKeymasterApp_BLOCK_MODE_CBC UINT32_C(2)
#define IKeymasterApp_BLOCK_MODE_CTR UINT32_C(3)
#define IKeymasterApp_BLOCK_MODE_GCM UINT32_C(32)
#define IKeymasterApp_EC_CURVE_P_224 UINT32_C(0)
#define IKeymasterApp_EC_CURVE_P_256 UINT32_C(1)
#define IKeymasterApp_EC_CURVE_P_384 UINT32_C(2)
#define IKeymasterApp_EC_CURVE_P_521 UINT32_C(3)
#define IKeymasterApp_KEY_FORMAT_X509 UINT32_C(0)
#define IKeymasterApp_KEY_FORMAT_PKCS8 UINT32_C(1)
#define IKeymasterApp_KEY_FORMAT_RAW UINT32_C(3)
#define IKeymasterApp_AES_BLOCK_SIZE UINT32_C(16)
#define IKeymasterApp_AES128_KEY_SIZE UINT32_C(128)
#define IKeymasterApp_AES192_KEY_SIZE UINT32_C(192)
#define IKeymasterApp_AES256_KEY_SIZE UINT32_C(256)
#define IKeymasterApp_TAG_ALGORITHM UINT32_C(51)
#define IKeymasterApp_TAG_KEY_SIZE UINT32_C(52)
#define IKeymasterApp_TAG_KEY_PURPOSE UINT32_C(53)
#define IKeymasterApp_TAG_DIGEST UINT32_C(54)
#define IKeymasterApp_TAG_PADDING UINT32_C(55)
#define IKeymasterApp_TAG_BLOCK_MODE UINT32_C(56)
#define IKeymasterApp_TAG_EC_CURVE UINT32_C(57)
#define IKeymasterApp_TAG_NONCE UINT32_C(58)
#define IKeymasterApp_TAG_RSA_PUBLIC_EXPONENT UINT32_C(59)
#define IKeymasterApp_TAG_MIN_MAC_LENGTH UINT32_C(60)
#define IKeymasterApp_TAG_APP_DATA UINT32_C(61)
#define IKeymasterApp_TAG_CALLER_NONCE UINT32_C(62)

#define IKeymasterApp_E_PARSE_CBOR INT32_C(10)
#define IKeymasterApp_E_GEN_CBOR INT32_C(11)
#define IKeymasterApp_E_UNEXPECTED_CBOR_ITEM INT32_C(12)

#define IKeymasterApp_OP_generateKey 0
#define IKeymasterApp_OP_importKey 1
#define IKeymasterApp_OP_exportKey 2
#define IKeymasterApp_OP_getKeyCharacteristics 3
#define IKeymasterApp_OP_beginSignature 4
#define IKeymasterApp_OP_beginCipher 5

static inline int32_t
IKeymasterApp_release(Object self)
{
  return Object_invoke(self, Object_OP_release, 0, 0);
}

static inline int32_t
IKeymasterApp_retain(Object self)
{
  return Object_invoke(self, Object_OP_retain, 0, 0);
}

static inline int32_t
IKeymasterApp_generateKey(Object self, const uint8_t *keyParamsCBOR_ptr, size_t keyParamsCBOR_len, uint8_t *keyBlob_ptr, size_t keyBlob_len, size_t *keyBlob_lenout)
{
  ObjectArg a[2];
  a[0].bi = (ObjectBufIn) { keyParamsCBOR_ptr, keyParamsCBOR_len * sizeof(uint8_t) };
  a[1].b = (ObjectBuf) { keyBlob_ptr, keyBlob_len * sizeof(uint8_t) };

  int32_t result = Object_invoke(self, IKeymasterApp_OP_generateKey, a, ObjectCounts_pack(1, 1, 0, 0));

  *keyBlob_lenout = a[1].b.size / sizeof(uint8_t);

  return result;
}

static inline int32_t
IKeymasterApp_importKey(Object self, const uint8_t *keyParamsCBOR_ptr, size_t keyParamsCBOR_len, int32_t keyFormat_val, const uint8_t *keyData_ptr, size_t keyData_len, uint8_t *keyBlob_ptr, size_t keyBlob_len, size_t *keyBlob_lenout)
{
  ObjectArg a[4];
  a[0].bi = (ObjectBufIn) { keyParamsCBOR_ptr, keyParamsCBOR_len * sizeof(uint8_t) };
  a[1].b = (ObjectBuf) { &keyFormat_val, sizeof(int32_t) };
  a[2].bi = (ObjectBufIn) { keyData_ptr, keyData_len * sizeof(uint8_t) };
  a[3].b = (ObjectBuf) { keyBlob_ptr, keyBlob_len * sizeof(uint8_t) };

  int32_t result = Object_invoke(self, IKeymasterApp_OP_importKey, a, ObjectCounts_pack(3, 1, 0, 0));

  *keyBlob_lenout = a[3].b.size / sizeof(uint8_t);

  return result;
}

static inline int32_t
IKeymasterApp_exportKey(Object self, const uint8_t *keyBlob_ptr, size_t keyBlob_len, int32_t keyFormat_val, uint8_t *keyData_ptr, size_t keyData_len, size_t *keyData_lenout)
{
  ObjectArg a[3];
  a[0].bi = (ObjectBufIn) { keyBlob_ptr, keyBlob_len * sizeof(uint8_t) };
  a[1].b = (ObjectBuf) { &keyFormat_val, sizeof(int32_t) };
  a[2].b = (ObjectBuf) { keyData_ptr, keyData_len * sizeof(uint8_t) };

  int32_t result = Object_invoke(self, IKeymasterApp_OP_exportKey, a, ObjectCounts_pack(2, 1, 0, 0));

  *keyData_lenout = a[2].b.size / sizeof(uint8_t);

  return result;
}

static inline int32_t
IKeymasterApp_getKeyCharacteristics(Object self, const uint8_t *keyBlob_ptr, size_t keyBlob_len, uint8_t *keyCharacteristicsCBOR_ptr, size_t keyCharacteristicsCBOR_len, size_t *keyCharacteristicsCBOR_lenout)
{
  ObjectArg a[2];
  a[0].bi = (ObjectBufIn) { keyBlob_ptr, keyBlob_len * sizeof(uint8_t) };
  a[1].b = (ObjectBuf) { keyCharacteristicsCBOR_ptr, keyCharacteristicsCBOR_len * sizeof(uint8_t) };

  int32_t result = Object_invoke(self, IKeymasterApp_OP_getKeyCharacteristics, a, ObjectCounts_pack(1, 1, 0, 0));

  *keyCharacteristicsCBOR_lenout = a[1].b.size / sizeof(uint8_t);

  return result;
}

static inline int32_t
IKeymasterApp_beginSignature(Object self, int32_t purpose_val, const uint8_t *keyBlob_ptr, size_t keyBlob_len, const uint8_t *inParamsCBOR_ptr, size_t inParamsCBOR_len, Object *signOperation_ptr)
{
  ObjectArg a[4];
  a[0].b = (ObjectBuf) { &purpose_val, sizeof(int32_t) };
  a[1].bi = (ObjectBufIn) { keyBlob_ptr, keyBlob_len * sizeof(uint8_t) };
  a[2].bi = (ObjectBufIn) { inParamsCBOR_ptr, inParamsCBOR_len * sizeof(uint8_t) };

  int32_t result = Object_invoke(self, IKeymasterApp_OP_beginSignature, a, ObjectCounts_pack(3, 0, 0, 1));

  *signOperation_ptr = a[3].o;

  return result;
}

static inline int32_t
IKeymasterApp_beginCipher(Object self, uint32_t purpose_val, const uint8_t *keyBlob_ptr, size_t keyBlob_len, const uint8_t *inParamsCBOR_ptr, size_t inParamsCBOR_len, uint8_t *outParamsCBOR_ptr, size_t outParamsCBOR_len, size_t *outParamsCBOR_lenout, Object *cipherOperation_ptr)
{
  ObjectArg a[5];
  a[0].b = (ObjectBuf) { &purpose_val, sizeof(uint32_t) };
  a[1].bi = (ObjectBufIn) { keyBlob_ptr, keyBlob_len * sizeof(uint8_t) };
  a[2].bi = (ObjectBufIn) { inParamsCBOR_ptr, inParamsCBOR_len * sizeof(uint8_t) };
  a[3].b = (ObjectBuf) { outParamsCBOR_ptr, outParamsCBOR_len * sizeof(uint8_t) };

  int32_t result = Object_invoke(self, IKeymasterApp_OP_beginCipher, a, ObjectCounts_pack(3, 1, 0, 1));

  *outParamsCBOR_lenout = a[3].b.size / sizeof(uint8_t);
  *cipherOperation_ptr = a[4].o;

  return result;
}



