/* ============================================================================
Copyright (c) 2018-2019 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
 ============================================================================*/

/** @file qapi_qwes.h

 QWES Agent QAPI

 QWES stands for Qualcomm Wireless Edge Service

 Sample TLS Certificate Usage (Some error handling omitted for clarity)

 @code
 qapi_QWES_Cert_Handle_t setup() {
   uint8_t cert_id[QAPI_QWES_CERT_ID_MAX_LEN];
   size_t cert_id_len;
   // Load ID From Persistent Storage(cert_id, &cert_id_len);
   if (SUCCESS) {
     qapi_QWES_Cert_Handle_t rv;
     if (QAPI_OK == qapi_QWES_Agent_Get_Cert(cert_id, cert_id_len, &rv)) {
       return rv;
     }
   }
   // Failed to load cert, or QWES did not recognize ID
   // Caller should call or schedule full_setup.
   return -1;
 }

 void cert_callback(void* context, qapi_Status_t status, qapi_QWES_Cert_Handle_t cert_handle) {
   if (QAPI_OK == status) {
     uint8_t cert_id[QAPI_QWES_CERT_ID_MAX_LEN];
     size_t max_len = QAPI_QWES_CERT_ID_MAX_LEN;
     size_t len = 0;
     qapi_QWES_Cert_Get_ID(cert_handle, max_len, &len);
     // Save ID To Persistent Storage (cert_id, len);
     // Notify main thread of cert_handle
   } else {
     // Schedule retry of full_setup()
   }
 }

 // If this function returns QAPI_OK, the callback will be called
 // at some point in the future.
 qapi_Status_t full_setup() {
   // Here we're using pre-generated, fixed parameters.
   // Alternatively, dynamically generate key_parameters and csr_parameters
   extern const uint8_t* key_parameters;
   extern const size_t key_param_len;
   extern const uint8_t* csr_parameters;
   extern const size_t csr_param_len;

   return qapi_QWES_Agent_Generate_Certificate("registered_service_name",
                                               csr_parameters, csr_param_len,
                                               key_parameters, key_param_len,
                                               cert_callback, NULL);
 }
 @endcode

 Sample Provisioning Usage (Some error handling omitted for clarity)
 @code
 void provision_callback(void* context, qapi_Status_t status, qapi_QWES_Data_Handle_t data_handle, size_t data_len) {
   bool success = false;
   if (QAPI_OK == status) {
     uint8_t *cose_sign = malloc(data_len);
     if (cose_sign) {
       size_t bytes_read = 0;
       qapi_QWES_Data_Get(data_handle, cose_sign, 0, data_len, &bytes_read);
       // Validate signature and store provisioned data
       success = true;
       free(cose_sign);
     }
   }
   if (!success) {
     // Something failed.  Schedule retry of provision()
   }
 }

 // If this function returns QAPI_OK, the callback will be called
 // at some point in the future.
 qapi_Status_t provision() {
   return qapi_QWES_Agent_Get_Provisioned_Data("MegaCorpUniqueID", NULL, provision_callback, NULL);
 }
 @endcode

*/

#ifndef QAPI_QWES_H_
#define QAPI_QWES_H_

#include "qapi_status.h"

#define QAPI_MOD_QWES (22)

#define QAPI_ERR_QWES_ATTESTATION_VERIFY_FAIL ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_QWES, 1)))
#define QAPI_ERR_QWES_DEVICE_NOT_CLAIMED ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_QWES, 2)))
#define QAPI_ERR_QWES_SERVICE_ID_MISMATCH ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_QWES, 3)))
#define QAPI_ERR_QWES_CERT_DENIED_DEVICE_REVOKED ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_QWES, 4)))
#define QAPI_ERR_QWES_CERT_DENIED_DEVICE_SUSPENDED ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_QWES, 5)))
#define QAPI_ERR_QWES_CERT_DENIED_DEVICE_POLICY ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_QWES, 6)))
#define QAPI_ERR_CERT_EXPIRED_OR_NOTYETVALID ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_QWES, 7)))
#define QAPI_ERR_CERT_OEM ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_QWES, 8)))
#define QAPI_ERR_CERT_HWVERSION ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_QWES, 9)))
#define QAPI_ERR_CERT_DEVICEID ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_QWES, 10)))
#define QAPI_ERR_INVALID_PFM_EXTENSION ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_QWES, 11)))
#define QAPI_ERR_PFMFILER_FAILED ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_QWES, 12)))

#ifndef IPFM_LICENSE_HAS_OEMID
#include <stdint.h>
#define IPFM_LICENSE_HAS_OEMID UINT64_C(0x00000001)
#define IPFM_LICENSE_HAS_HWVERSION UINT64_C(0x00000002)
#define IPFM_LICENSE_HAS_DEVICEID UINT64_C(0x00000004)
#define IPFM_LICENSE_HAS_PKHASH UINT64_C(0x00000008)
#define IPFM_LICENSE_HAS_LICENSEEHASH UINT64_C(0x00000010)
#define IPFM_LICENSE_HAS_VALIDTIME UINT64_C(0x00000020)
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** maximum certificate ID length */
#ifndef QAPI_QWES_CERT_ID_MAX_LEN
#define QAPI_QWES_CERT_ID_MAX_LEN 64
#endif

/**
 * Hashing Algorithms
 *
 * When signing or verifying a signature,
 * only one of these values should be passed.
 */
typedef enum {
    QAPI_QWES_HASH_NONE_E,
    QAPI_QWES_HASH_MD5_E,
    QAPI_QWES_HASH_SHA1_E,
    QAPI_QWES_HASH_SHA2_224_E,
    QAPI_QWES_HASH_SHA2_256_E,
    QAPI_QWES_HASH_SHA2_384_E,
    QAPI_QWES_HASH_SHA2_512_E,
} qapi_QWES_Hash_Type_e;

/** @name Certificate management
 *  Methods and types used to generate signed TLS certs.
 */
//!@{
/** Certificate handle */
typedef int32_t qapi_QWES_Cert_Handle_t;

/**
* @versiontable{2.0,2.45,
* Security 1.2.0  &  Introduced. @tblendline
* }
*
* Callback to receive generated certificate */
typedef void (*qapi_QWES_Cert_Callback_CB)(void* context,
                                          qapi_Status_t status,
                                          qapi_QWES_Cert_Handle_t cert_handle);

/**
* @versiontable{2.0,2.45,
* Security 1.2.0  &  Introduced. @tblendline
* }
*
* Callback to receive Sync notification
*
* This function is called after the qwesd service communicates with the cloud.
*
* @param[in] context User-defined data
* @param[in] status QAPI_OK for success,
* QAPI_ERR_NO_RESOURCE if no network connection could be established,
* QAPI_ERROR for any other failure.
*
* @param[in] sync_interval The new sync interval (in seconds) from the cloud.
* When using Force_Sync, this value will normally be zero.
* */
typedef void (*qapi_QWES_Force_Sync_CB)(void* context,
                                        qapi_Status_t status);

/**
* @versiontable{2.0,2.45,
* Security 1.2.0  &  Introduced. @tblendline
* }
*
* Cert format when retrieving or registering a cert */
typedef enum {
    QAPI_QWES_CERT_FORMAT_PEM_E,
    QAPI_QWES_CERT_FORMAT_DER_E,
} qapi_QWES_Cert_Format_Type_e;

/**
* @versiontable{2.0,2.45,
* Security 1.2.0  &  Introduced. @tblendline
* }
*
* Initializes QWES Agent
*/
qapi_Status_t qapi_QWES_Agent_Init(void);

/**
* @versiontable{2.0,2.45,
* Security 1.2.0  &  Introduced. @tblendline
* }
*
* Cleans up resources acquired in qapi_QWES_Agent_Init
*/
void qapi_QWES_Agent_Exit(void);

/**
* @versiontable{2.0,2.45,
* Security 1.2.0  &  Introduced. @tblendline
* }
*
* Generate a new certificate
*
* This method must establish a connection with a cloud-based service.
* This function will return immediately.  If this function returns QAPI_OK,
* The response generated by the service will be processed asynchronously,
* after which the given callback will be called on a different thread.
*
* The Key parameters structure is a CBOR map:
* @code
* keyParamsCBOR = {
*  IKeymaster_TAG_KEY_PURPOSE: [+ &key-purpose],
*  IKeymaster_TAG_KEY_SIZE: uint // IKeymaster_TAG_EC_CURVE: &ec_curve,
*  IKeymaster_TAG_PADDING: [+ &padding],
*  IKeymaster_TAG_DIGEST: [+ &digest],
*  ? IKeymaster_TAG_RSA_PUBLIC_EXPONENT: uint,
*  ? IKeymaster_TAG_APP_DATA: bstr
* }
*
* Valid values for the entries are:
*  key-purpose = (
*      IKeymaster_KEY_PURPOSE_SIGN,
*      IKeymaster_KEY_PURPOSE_VERIFY,
*      IKeymaster_KEY_PURPOSE_ENCRYPT,
*      IKeymaster_KEY_PURPOSE_DECRYPT)
*
*  padding = (
*      IKeymaster_PAD_NONE,
*      IKeymaster_PAD_RSA_OAEP,
*      IKeymaster_PAD_RSA_PSS,
*      IKeymaster_PAD_RSA_PKCS1_1_5_ENCRYPT,
*      IKeymaster_PAD_RSA_PKCS1_1_1_SIGN)
*
*  digest = (
*      IKeymaster_DIGEST_NONE,
*      IKeymaster_DIGEST_MD5,
*      IKeymaster_DIGEST_SHA1,
*      IKeymaster_DIGEST_SHA_2_224,
*      IKeymaster_DIGEST_SHA_2_256,
*      IKeymaster_DIGEST_SHA_2_384,
*      IKeymaster_DIGEST_SHA_2_51)
*
*  ec_curve = (
*      IKeymaster_EC_CURVE_P_224,
*      IKeymaster_EC_CURVE_P_256,
*      IKeymaster_EC_CURVE_P_384,
*      IKeymaster_EC_CURVE_P_521)
* @endcode
*
* The constants are defined in IKeymaster.h
*
* The CSR parameters structure is a CBOIR map:
*
* @code
*   rdn = (
*      ICertManager_TAG_RDN_COMMON_NAME,
*      ICertManager_TAG_RDN_ORG,
*      ICertManager_TAG_RDN_ORG_UNIT,
*      ICertManager_TAG_RDN_LOCALITY,
*      ICertManager_TAG_RDN_STATE,
*      ICertManager_TAG_RDN_COUNTRY,
*      ICertManager_TAG_RDN_SERIAL_NUM)
*
*   digest = (
*      ICertManager_DIGEST_SHA_2_256,
*      ICertManager_DIGEST_SHA_2_384,
*      ICertManager_DIGEST_SHA_2_512)
*
*   csrParamsCBOR = {
*       ? ICertManager_TAG_RDN_LIST
*       : { ? ICertManager_TAG_RDN_COMMON_NAME : text,
*           ? ICertManager_TAG_RDN_ORG : text,
*           ? ICertManager_TAG_RDN_ORG_UNIT : text,
*           ? ICertManager_TAG_RDN_LOCALITY : text,
*           ? ICertManager_TAG_RDN_STATE : text,
*           ? ICertManager_TAG_RDN_COUNTRY : text,
*           ? ICertManager_TAG_RDN_SERIAL_NUM : text},
*       ICertManager_TAG_DIGEST_ALG : &digest
*   }
* @endcode
*
* The constants are defined in ICertManager.h
*
* @param[in] service_id        Identifies the IoT app / service, null-terminated string.
*                              Maximum length is 64 bytes, including terminator.
* @param[in] csr_params        CSR parameters in CBOR format
* @param[in] csr_params_len    Length of csr_params cbor data
* @param[in] key_params        Key parameters in CBOR format
* @param[in] key_params_len    Length of key_params cbor data
* @param[in] callback          Callback provided by the client
* @param[in] context           Data to be passed to the callback
*
* @retval QAPI_OK on success
* @retval QAPI_ERR_INVALID_PARAM if any parameter is null or invalid
* @retval QAPI_ERR_BUSY if the connection to the server failed
* @retval QAPI_ERR_INVALID_STATE if the device security has been compromised
*/
qapi_Status_t qapi_QWES_Agent_Generate_Certificate(const char* service_id,
                                                   const uint8_t* csr_params,
                                                   size_t csr_params_len,
                                                   const uint8_t* key_params,
                                                   size_t key_params_len,
                                                   qapi_QWES_Cert_Callback_CB callback,
                                                   void* context);
/**
* @versiontable{2.0,2.45,
* Security 1.2.0  &  Introduced. @tblendline
* }
*
* Generate a TLS certificate request blob
*
* @ref qapi_QWES_Agent_Generate_Certificate uses a TLS network connection
* to communicate with the server.  If such a network connection is not
* avaiable, the client application can use this method and transport the
* certificate signing request by other means.  After the client application
* receives a response from the server, it must pass that response to
* @ref qapi_QWES_Agent_Save_Cert.
*
* For a description of the key parameters CBOR structure, refer to
* @ref qapi_QWES_Agent_Generate_Certificate
*
* For a description of the CSR parameters CBOR structure, refer to
* @ref qapi_QWES_Agent_Generate_Certificate
*
* @param[in] service_id        Identifies the IoT app / service, null-terminated string.
*                              Maximum length is 64 bytes, including terminator.
* @param[in] csr_params        CSR parameters in CBOR format
* @param[in] csr_params_len    Length of csr_params cbor data
* @param[in] key_params        Key parameters in CBOR format
* @param[in] key_params_len    Length of key_params cbor data
* @param[out] data             Buffer to hold request blob.
* @param[in] data_len          Length of data buffer
* @param[out] data_len_out     Data length
*
* @retval QAPI_ERR_NO_MEMORY if data_len is too small.
*                    No data will be written to the data buffer, but
*                    data_len_out will be set to the required size.
*/
qapi_Status_t qapi_QWES_Agent_Get_CSR(const char* service_id,
                                      const uint8_t* csr_params,
                                      size_t csr_params_len,
                                      const uint8_t* key_params,
                                      size_t key_params_len,
                                      uint8_t* data,
                                      size_t data_len,
                                      size_t* data_len_out);

/**
* @versiontable{2.0,2.45,
* Security 1.2.0  &  Introduced. @tblendline
* }
*
* Register a cert returned from the server
*
* This method is used by the client in conjunction with qapi_QWES_Agent_Get_CSR
* to send a CSR over a non-tls connection.  The cert passed to this method
* must be signed by a QC root CA, or an intermediate CA.
*
* @param[in] cert              X.509 certificate data, in DER or PEM format
* @param[in] cert_len          Length of the cert, in bytes
* @param[out] cert_handle_out  Pointer to hold the resulting cert handle
*/
qapi_Status_t qapi_QWES_Agent_Save_Cert(uint8_t* cert,
                                        size_t cert_len,
                                        qapi_QWES_Cert_Handle_t* cert_handle_out);

/**
* @versiontable{2.0,2.45,
* Security 1.2.0  &  Introduced. @tblendline
* }
*
* Retrieve a certificate by ID
*
* See @ref qapi_QWES_Cert_Get_ID for details about the ID.
* The client must call @ref qapi_QWES_Cert_Release when the cert is no longer needed.
*
* @param[in] cert_id           Certificate ID
* @param[in] cert_id_len       Certificate ID length
* @param[out] cert_handle_out  Certificate handle
*
* @retval QAPI_OK on success
* @retval QAPI_ERR_INVALID_PARAM if cert_handle_out is null
* @retval QAPI_ERR_NO_ENTRY if the certificate ID is unknown
*/
qapi_Status_t qapi_QWES_Agent_Get_Cert_Handle(uint8_t* cert_id,
                                              size_t cert_id_len,
                                              qapi_QWES_Cert_Handle_t* cert_handle_out);
//!@}

/** @name Certificate Usage
 *  Methods and types used with certificate handles
 */
//!@{

/**
* @versiontable{2.0,2.45,
* Security 1.2.0  &  Introduced. @tblendline
* }
*
* Retrieve the certificate identifier
*
* The cert_handle ID uniquely and persistently identifies the cert_handle on
* the local device, and can be stored by the client and used later to
* retrieve the cert_handle from the manager.
*
* @param[in] cert_handle          Certificate handle
* @param[out] cert_id_out         Certificate ID
* @param[in] cert_id_len          Length of cert_id_out buffer.
* @param[out] cert_id_len_out     Certificate ID length
*
* @retval QAPI_OK if the cert_handle was retrieved successfully.
* @retval QAPI_ERR_INVALID_PARAM if any parameter is null
* @retval QAPI_ERR_NO_MEMORY if cert_id_len is too small
*     (cert_id_len_out will be set to the minimum required value)
*/
qapi_Status_t qapi_QWES_Cert_Get_ID(qapi_QWES_Cert_Handle_t cert_handle,
                                    uint8_t* cert_id_out,
                                    size_t cert_id_len,
                                    size_t* cert_id_len_out);

/**
* @versiontable{2.0,2.45,
* Security 1.2.0  &  Introduced. @tblendline
* }
*
* Retrieve the full certificate data
*
* The retrieved certificate can be used with third-party software.
*
* @param[in] cert_handle   Certificate handle
* @param[in] format        Format to use, DER or PEM.
* @param[out] data         Buffer to hold cert_handle.
* @param[in] data_len      Length of data buffer
* @param[out] data_len_out Length of cert_handle in PEM format
*
* @retval QAPI_OK if the cert_handle was retrieved successfully.
* @retval QAPI_ERR_INVALID_PARAM if any pointer is null.
* @retval QAPI_ERR_NO_MEMORY if data_len is too small
*     (data_len_out will be set to the minimum required value)
*/
qapi_Status_t qapi_QWES_Cert_Format(qapi_QWES_Cert_Handle_t cert_handle,
                                    qapi_QWES_Cert_Format_Type_e format,
                                    uint8_t* data,
                                    size_t data_len,
                                    size_t* data_len_out);

/**
* @versiontable{2.0,2.45,
* Security 1.2.0  &  Introduced. @tblendline
* }
*
* Sign the given data with the certificate's private key
*
* @param[in] cert_handle   Certificate handle
* @param[in] data          Data to sign
* @param[in] data_len      Length of data buffer
* @param[in] algorithm     Signature algorithm
* @param[out] sig          Buffer to hold signature.  Raw octets.  Not null-terminated.
* @param[in] sig_len       Length of sig buffer, in bytes.
*                          Typically key size in bits / 8 is large enough.
* @param[out] sig_len_out  Length of generated signature
*
* @retval QAPI_OK if the signature is generated successfully.
* @retval QAPI_ERR_INVALID_PARAM if any pointer is null.
* @retval QAPI_ERR_NO_MEMORY if sig_len is too small for generated signature.
*                    No data will be written to the sig buffer, but
*                    sig_len_out will be set to the required size.
*/
qapi_Status_t qapi_QWES_Cert_Sign(qapi_QWES_Cert_Handle_t cert_handle,
                                  const uint8_t* data,
                                  size_t data_len,
                                  qapi_QWES_Hash_Type_e algorithm,
                                  uint8_t* sig,
                                  size_t sig_len,
                                  size_t* sig_len_out);

/**
* @versiontable{2.0,2.45,
* Security 1.2.0  &  Introduced. @tblendline
* }
*
* Decrypt the given data with the certificate's private key
*
* @param[in] cert_handle       Certificate handle
* @param[in] cipher            Encrypted data
* @param[in] cipher_len        Length of cipher buffer
* @param[in] data              Buffer to hold decrypted data
* @param[in] data_len          Length of data buffer
* @param[out] data_len_out     Length of decrypted data
*
* TODO:  Allow client to specify padding
*
* @retval QAPI_OK if the generated signature matches the signature that was passed in.
* @retval QAPI_ERR_INVALID_PARAM if any pointer is null.
* @retval QAPI_ERR_NO_MEMORY if sig_len is too small for generated signature.
*                    No data will be written to the sig buffer, but
*                    sig_len_out will be set to the required size.
*/
qapi_Status_t qapi_QWES_Cert_Decrypt(qapi_QWES_Cert_Handle_t cert_handle,
                                     const uint8_t* cipher,
                                     size_t cipher_len,
                                     uint8_t* data,
                                     size_t data_len,
                                     size_t* data_len_out);

/**
* @versiontable{2.0,2.45,
* Security 1.2.0  &  Introduced. @tblendline
* }
*
* Release the certificate
*
* This call frees up any underlying resources.  After this call, the
* cert_handle is invalid and may no longer be used.
*/
qapi_Status_t qapi_QWES_Cert_Release(qapi_QWES_Cert_Handle_t cert_handle);

//!@}

/** @name Provisioning
 *  Methods and types used to retrieve provisioned data
 */
//!@{

/** Opaque data buffer handle */
typedef int32_t qapi_QWES_Data_Handle_t;

/**
* @versiontable{2.0,2.45,
* Security 1.2.0  &  Introduced. @tblendline
* }
*
* Callback to receive decrypted provisioned data
*
* The data handle will only be valid for the duration of the callback.
*
* @param[in] context           Application context
* @param[in] status            QAPI_OK if the transaction succeeded.
* @param[in] decrypted_data    Data handle
* @param[in] data_len          Size of data, in bytes.
**/
typedef void (*qapi_QWES_Provision_Callback_t)(void* context,
                                               qapi_Status_t status,
                                               qapi_QWES_Data_Handle_t decrypted_data,
                                               size_t data_len);

/**
* @versiontable{2.0,2.45,
* Security 1.2.0  &  Introduced. @tblendline
* }
*
* Retrive Provisioned data from the cloud
*
* Provisioning retrieves data from a cloud-based service.
* This method will return as soon as the request is sent.
* If the request is successful, the callback will be called asynchronously.
* The callback will receive an opaque handle to the encrypted provisioned data,
* and a handle to a key to decrypt the provisioned data.
*
* Note that the service_id string must be a value that is registered with the QWES
* server, and is used by the QWES server to forward the request to the correct
* service.
*
* Example of use:

TODO Update

*
* @code
* const char* service_id = "12345678-1234-1234-1234-123456789012"
* const char* opts = "[passwords, server_urls]";
*
* qapi_Status_t status = qapi_QWES_Agent_Get_Provisioned_Data(service_id, opts, callback, NULL);
* if (QAPI_OK != status) {
*    // Schedule a retry
* }
* @endcode
*
* @param[in] service_id    String that identifies the IoT app / service
*                          Maximum length is 64 bytes, including terminator.
* @param[in] request_opt   Request options.  App-defined, may pass NULL.
* @param[in] opt_len       Options length, may be zero if request_opt is NULL.
* @param[in] callback      Callback to call with provisioned data
* @param[in] context       Data to pass to callback.
*
* @retval QAPI_OK on success
* @retval QAPI_ERR_INVALID_PARAM if callback is null
* @retval QAPI_ERROR if the server rejected the request
* @retval QAPI_ERR_BUSY if the connection to the server failed.
* @retval QAPI_ERR_INVALID_STATE if the device security has been compromised.
*/
qapi_Status_t qapi_QWES_Agent_Get_Provisioned_Data(
    const char* service_id,
    void* request_opt,
    size_t opt_len,
    qapi_QWES_Provision_Callback_t callback,
    void* context);

/**
* @versiontable{2.0,2.45,
* Security 1.2.0  &  Introduced. @tblendline
* }
*
* Generate a provisioning request blob
*
* @param[in] service_id            String that identifies the IoT app / service
*                                  Maximum length is 64 bytes, including terminator.
* @param[in] request_opt           Request options.  App-defined, may pass NULL.
* @param[in] opt_len               Options length, may be zero if request_opt is NULL.
* @param[out] data                 Buffer to hold request blob.
* @param[in] data_len              Length of data buffer
* @param[out] data_len_out         Data length
*
* @retval QAPI_ERR_NO_MEMORY if data_len is too small.
*                    No data will be written to the data buffer, but
*                    data_len_out will be set to the required size.
*/
qapi_Status_t qapi_QWES_Agent_Get_Provisioning_Blob(const char* service_id,
                                                    void* request_opt,
                                                    size_t opt_len,
                                                    uint8_t* data,
                                                    size_t data_len,
                                                    size_t* data_len_out);

/**
* @versiontable{2.0,2.45,
* Security 1.2.0  &  Introduced. @tblendline
* }
*
* Decrypt a COSE message generated by the server.
*
* During provisioning, the server is expected to generate a COSE_Encrypt
* structure to securely transmit the provisioned data.  The payload
* will be encrypted with a temporary Content Encryption Key (CEK).  The
* CEK itself is also encrypted.
*
* This method will decrypt the CEK and use that CEK to decrypt the ciphertext.
*
* @code
*  COSE_Encrypt = [
*    Headers,
*    ciphertext : bstr,
*    recipients : [+COSE_recipient]
*  ]
* @endcode
*
* See @ref qapi_QWES_Agent_Get_CSR for details.
*
* @param[in] encrypted     Server-generated COSE_Encrypt structure
* @param[in] encrypted_len Length of encrypted, in bytes.
* @param[in] callback      Callback to call with decrypted data
* @param[in] context       Data to pass to callback.
*/
qapi_Status_t qapi_QWES_Agent_Decrypt_Provisioned_Data(
    uint8_t* encrypted,
    size_t encrypted_len,
    qapi_QWES_Provision_Callback_t callback,
    void* context);

/**
* @versiontable{2.0,2.45,
* Security 1.2.0  &  Introduced. @tblendline
* }
*
* Retrieve data from a data handle
*
* Note that the offset parameter refers to the data _source_.  Bytes are
* always written starting at the address passed as the 'data' argument.
* If offset is >= the data source length, bytes_read will be set to zero.
* Otherwise, bytes read will be the lesser of bytes_to_read and
* data source length - offset.
*
* @param[in] decrypted_data_handle Handle to the data
* @param[out] decrypted_data       Buffer to hold data
* @param[in] offset                Offset into the managed data
* @param[in] bytes_to_read         Number of bytes to read
* @param[out] bytes_read           Number of bytes read
*/
qapi_Status_t qapi_QWES_Data_Get(qapi_QWES_Data_Handle_t decrypted_data_handle,
                                 uint8_t* decrypted_data,
                                 size_t offset,
                                 size_t bytes_to_read,
                                 size_t* bytes_read);

//!@}   End of Provisioning group

/** @name Attestation
 *  Methods and types used to generate signed TLS certs.
 */

///@{
/**
* @versiontable{2.0,2.45,
* Security 1.2.0  &  Introduced. @tblendline
* }
*
* Create attestation token with user data and nonce.
*
* Put user data and the nonce into a haven token and return the
* token to the client.  The token will be base-64 encoded.
*
* The client will send the token to OEM server, which
* will forward it to the DTS cloud.  The DTS cloud will unpack the token and
* return the app data to the OEM server.
*
* @param[in] nonce             Transaction ID provided by the server
* @param[in] nonce_len         Number of bytes in nonce
* @param[in] data              Application-defined data, may be NULL
* @param[in] data_len          Length of the data buffer, may be 0 if data is NULL
* @param[out] token            Buffer to hold token
* @param[in] token_len         Length of the token buffer
* @param[out] token_len_out    Length of generated token.
*
* @retval QAPI_OK if the token was created successfully.
* @retval QAPI_ERR_NO_MEMORY if token_len is too small for generated haven token.
*                    No data will be written to the token buffer, but
*                    token_len_out will be set to the required size.
* @retval QAPI_ERR_INVALID_STATE if the device security has been compromised.
* @retval QAPI_ERROR for other errors
*/
qapi_Status_t qapi_QWES_Agent_Attest(uint8_t* nonce,
                                     size_t nonce_len,
                                     const uint8_t* data,
                                     size_t data_len,
                                     uint8_t* token,
                                     size_t token_len,
                                     size_t* token_len_out);
///@}

/** @name Licensing
 *  Methods and types used to check licenses
 */

/**
 * license status codes
 */
typedef enum {
    QAPI_QWES_LICENSE_NONE_E,     //!< No license found
    QAPI_QWES_LICENSE_OK_E,       //!< Valid license found
    QAPI_QWES_LICENSE_EXPIRED_E,  //!< Expired license found
} qapi_QWES_License_Status_e;

///@{
/**
* @versiontable{2.0,2.45,
* Security 1.2.0  &  Introduced. @tblendline
* }
*
* Check the license status of one or more feature,client pairs.
*
* The query is a CBOR array of feature id, ISV Hash pairs
* @code
* query = [ 1* query_entry ]
* query_entry = (
*     feature_id: uint,
*     client_id: bstr,
*     )
* @endcode
*
* The response is an array of license serial number and status pairs
* @code
* response = [ 1* response_entry ]
* response_entry = (
*     feature_id: uint,
*     license_hash: bstr,
*     license_serial_num: bstr,
*     status: uint,
*     )
* @endcode
*
* The status values match @ref qapi_QWES_License_Status_e.  If the status
* is QAPI_QWES_LICENSE_NONE_E, the license_serial_num should be ignored.
*
* @param[in] query             Buffer containing CBOR query structure
* @param[in] query_len         Length of the query buffer
* @param[out] results          Buffer to hold CBOR results structure
* @param[in] results_len       Length of the results buffer
* @param[out] results_len_out  Length of generated results
*
* @retval QAPI_OK if the query was executed successfully.
* @retval QAPI_ERR_INVALID_PARAM if any pointer is null.
* @retval QAPI_ERR_NO_MEMORY if results_len is too small for generated haven results.
*                    No data will be written to the results buffer, but
*                    results_len_out will be set to the required size.
* @retval QAPI_ERR_INVALID_STATE if the device security has been compromised.
* @retval QAPI_ERROR for other errors
*/
qapi_Status_t qapi_QWES_Agent_Check_Licenses(const uint8_t* query,
                                             size_t query_len,
                                             uint8_t* results,
                                             size_t results_len,
                                             size_t* results_len_out);

///@{
/**
* @versiontable{2.0,2.45,
* Security 1.2.0  &  Introduced. @tblendline
* }
*
* Check the license status of one or more feature,client pairs against all installed licenses
*
* The query is a CBOR array of feature id, ISV Hash pairs
* @code
* query = [ 1* query_entry ]
* query_entry = (
*     feature_id: uint,
*     client_id: bstr,
*     )
* @endcode
*
* The response is an array of license serial number and status pairs
* @code
* response = [ 1* response_entry ]
* response_entry = (
*     feature_id: uint,
*     license_hash: bstr,
*     [ 1* result ]
*     )
* result = (
*     license_serial_num: bstr,
*     status: uint
*     )
* @endcode
*
* The status values match @ref qapi_QWES_License_Status_e.  If the status
* is QAPI_QWES_LICENSE_NONE_E, the license_serial_num should be ignored.
*
* @param[in] query             The feature ID to check enablement of.
* @param[in] query_len         Length of the query buffer
* @param[out] results          Buffer to hold CBOR results structure
* @param[in] results_len       Length of the results buffer
* @param[out] results_len_out  Length of generated results
*
* @retval QAPI_OK if the query was executed successfully.
* @retval QAPI_ERR_INVALID_PARAM if any pointer is null.
* @retval QAPI_ERR_NO_MEMORY if results_len is too small for generated haven results.
*                    No data will be written to the results buffer, but
*                    results_len_out will be set to the required size.
* @retval QAPI_ERR_INVALID_STATE if the device security has been compromised.
*/
qapi_Status_t qapi_QWES_Agent_Check_All_Licenses(const uint8_t* query,
                                             size_t query_len,
                                             uint8_t* results,
                                             size_t results_len,
                                             size_t* results_len_out);

/**
* @versiontable{2.0,2.45,
* Security 1.2.0  &  Introduced. @tblendline
* }
*
* Register a license with the manager
*
* @param [in] license_buffer   Feature License Cert, in PEM or DER format.
* @param [in] license_len      Number of bytes in license_buffer.
* @param [out] serial          Buffer to hold license serial number.
* @param [in] serial_len       Length of serial buffer
* @param [out] serial_len_out  Data length
* @param [out] info            Buffer to hold license feature ID information
* @param [in] info_len         Length of info buffer
* @param [out] info_len_out    Data length
* @param [out] restrictions    Bitmask of restrictions satisfied by the license
*
* Data written to the info buffer is a CBOR structure.  The structure is
* @code
* feature_ids = {
*   "FIDs" : [ 1* uint ],                  Array of feature IDs
*   "FIDRanges" : [ 1 * (uint, uint) ]     Array of (first,last) ID ranges
* }
* @endcode
*
* @retval QAPI_OK if the license was installed successfully.
* @retval QAPI_ERR_INVALID_STATE if the license manager is unavailable.
* @retval QAPI_ERR_NO_MEMORY if serial_len or info_len is too small.
*                    No data will be written to the buffer, but
*                    serial_len_out and info_len_out will be set to the required size.
* @retval QAPI_ERR_CERT_EXPIRED_OR_NOTYETVALID
* @retval QAPI_ERR_CERT_OEM
* @retval QAPI_ERR_CERT_HWVERSION
* @retval QAPI_ERR_CERT_DEVICEID
* @retval QAPI_ERR_INVALID_PFM_EXTENSION
* @retval QAPI_ERR_PFMFILER_FAILED
* @retval QAPI_ERR_INVALID_PARAM if the license manager rejects the license
*                                for some reason not listed above.
*/
qapi_Status_t qapi_QWES_Agent_Install_License(const uint8_t* license_buffer,
                                              size_t license_len,
                                              uint8_t* serial,
                                              size_t serial_len,
                                              size_t* serial_len_out,
                                              uint8_t* info,
                                              size_t info_len,
                                              size_t* info_len_out,
                                              uint64_t* restrictions);

qapi_Status_t qapi_QWES_Agent_Get_License_Info(const uint8_t* serial,
                                               size_t serial_len,
                                               uint8_t* info,
                                               size_t info_len,
                                               size_t* info_len_out);

/**
* @versiontable{2.0,2.45,
* Security 1.2.0  &  Introduced. @tblendline
* }
*
* Define network connection parameters (defaults are to be overridden)
*
* @param [in] param_cbor   CBOR map containing parameter identifiers and the
*                          corresponding value to be used.
*
* Possible map key values (typically map contains one of the below):
* Value | Meaning
* ------|-------
*   1   | UMTS profile ID, 0=default
*   2   | APN name
*   6   | CDMA profile ID
*
* (advanced options below, See QTI DSI NetCtrl documentation for details)
*
* Value | Meaning
* ------|-------
*   3   | APN user name
*   4   | APN password
*   5   | Authentication preference
*   7   | IP_ADDR,
*   8   | DEVICE_NAME,
*   9   | Technology preference
*   10  | CALL_TYPE
*   11  | Preferred IP family for the call (IPV4=4, IPV6=6, IPV4_or_V6=10)
*   12  | EXT_TECH_PREF
*   13  | PARTIAL_RETRY
*   14  | APP_TYPE
*
* CDDL
* @code
* Params: map() {
*     ?1        => int;
*     ?2        => tstr;
*     ?3        => tstr;
*     ?4        => tstr;
*     ?5        => int;
*     ?6        => int;
*     ?7        => int;
*     ?8        => tstr;
*     ?9        => int;
*     ?10       => int;
*     ?11       => int;
*     ?12       => int;
*     ?13       => int;
*     ?14       => int;
* }
* @endcode
*
* Typical usage is to either set the profile idx (0 means default) or set a
* specific APN.  Example code snippets below:
*
* (set profile idx to 0, for either CDMA or UMTS connection)
* @code
*    CBOREncodeContext ECtx;
*    uint8_t pEncoded[500];
*    size_t nEncodedLen = sizeof(pEncoded);
*    EncodedCBOR Enc;
*
*    CBOREncode_Init(&ECtx, pEncoded, nEncodedLen);
*    CBOREncode_OpenMap(&ECtx);
*    CBOREncode_AddInt64ToMapN(&ECtx, DSI_CALL_INFO_CDMA_PROFILE_IDX, 0);
*    CBOREncode_AddInt64ToMapN(&ECtx, DSI_CALL_INFO_UMTS_PROFILE_IDX, 0);
*    CBOREncode_CloseMap(&ECtx);
*    if(CBOREncode_Finish2(&ECtx, &Enc)) {
*        return QAPI_ERROR;
*    }
*    return qapi_QWES_Agent_Set_Network_Parameters(Enc.Bytes.ptr, Enc.Bytes.len);
* @endcode
*
* (set APN name to <param_APN>)
* @code
*    CBOREncodeContext ECtx;
*    uint8_t pEncoded[500];
*    size_t nEncodedLen = sizeof(pEncoded);
*    EncodedCBOR Enc;
*
*    CBOREncode_Init(&ECtx, pEncoded, nEncodedLen);
*    CBOREncode_OpenMap(&ECtx);
*    CBOREncode_AddSZStringToMapN(&ECtx, DSI_CALL_INFO_APN_NAME, param_APN);
*    CBOREncode_CloseMap(&ECtx);
*    if(CBOREncode_Finish2(&ECtx, &Enc)) {
*        return QAPI_ERROR;
*    }
*    return qapi_QWES_Agent_Set_Network_Parameters(Enc.Bytes.ptr, Enc.Bytes.len);
* @endcode
*
* @retval QAPI_OK if the request was sent to the qwes service successfully
* @retval QAPI_ERROR otherwise
*/
qapi_Status_t qapi_QWES_Agent_Set_Network_Parameters(uint8_t* param_cbor,
                                                     size_t param_cbor_len);
/**
* @versiontable{2.0,2.45,
* Security 1.2.0  &  Introduced. @tblendline
* }
*
* Register a callback method for sync events.
*
* @param[in] cb        Function to call when a sync event occurs
* @param[in] context   User-defined data, passed to cb when a sync event occurs
* @param[out] handle   Opaque handle used to unregister callback
*/
qapi_Status_t qapi_QWES_Agent_Register_Sync_Callback(qapi_QWES_Force_Sync_CB cb, void* context, void** handle);

/**
* @versiontable{2.0,2.45,
* Security 1.2.0  &  Introduced. @tblendline
* }
*
* Un-Register a callback method for sync events.
*
* After this method is called, the handle is not valid and must not be used.
*
* @param[in] handle    Handle produced by @ref qapi_QWES_Agent_Register_Sync_Callback
*/
qapi_Status_t qapi_QWES_Agent_Unregister_Sync_Callback(void* handle);

/**
* @versiontable{2.0,2.45,
* Security 1.2.0  &  Introduced. @tblendline
* }
*
* Command the qwes service to synchronize licenses with the cloud
*/
qapi_Status_t qapi_QWES_Agent_Force_Sync(void);

///@}
#ifdef __cplusplus
}
#endif

#endif
