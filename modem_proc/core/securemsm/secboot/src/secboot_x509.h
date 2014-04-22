#ifndef SECBOOT_X509_H
#define SECBOOT_X509_H
/*===========================================================================

                       S E C U R I T Y    S E R V I C E S

                  S E C U R E  B O O T   X.5 0 9  P A R S E R

                          H E A D E R   F I L E

FILE:  secboot_x509.h

DESCRIPTION:
    Secure Boot X.509 PARSER header file

EXTERNALIZED FUNCTIONS


  Copyright (c) 2004,2006,2008-2010,2013-2014,2016-2017 Qualcomm Technologies, Inc.
  All rights reserved.
  Qualcomm Technologies, Inc. Confidential and Proprietary.
===========================================================================*/

/*=========================================================================

                            EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.10/securemsm/secboot/src/secboot_x509.h#2 $
$DateTime: 2018/11/08 03:29:06 $
$Author: pwbldsvc $

when       who                  what, where, why
--------   ----                 -------------------------------------------
03/08/17   mm                   Remove OU fields from Certificate
03/21/16   sm                   SHA384 and ECDSA parsing support
07/13/16   hw                   Remove SHA1, PKCS 1.5 support, disable exp3
03/23/16   hw                   Support multiple soc hw version
08/05/14   hw                   Add QPSI code review fix
05/28/14   hw                   Added RoT support
08/28/13   hw                   Added retail unlock feature code
02/27/13   hw                   add support for multiple root selection
04/12/10   sm                   Added support for flexible chain size
09/10/09   sm                   Added pbl_secx509_get_asn1_cert_size to 
                                support regional security
03/01/08   ANB                  Added const qualifiers.
05/02/06   AMW                  Initial SC2X port.
08/25/04   SJD                  Initial revision.  Original SW from the
                                security team.

===========================================================================*/


/*==========================================================================

                          Include Files

==========================================================================*/
#include "comdef.h"
#include "secboot_asn1.h"
#include "secboot_fuse_info.h"
/*==========================================================================

                          Constant Declaration

==========================================================================*/
/* Key Usage Masks */
#define SECX509_KEY_USAGE_DIG_SIG   (0x0100) /* digital signature */
#define SECX509_KEY_USAGE_NON_REP   (0x0080) /* non-repudiation   */
#define SECX509_KEY_USAGE_KEY_ENC   (0x0040) /* key encipherment  */
#define SECX509_KEY_USAGE_DAT_ENC   (0x0020) /* data encipherment */
#define SECX509_KEY_USAGE_KEY_ARG   (0x0010) /* key agreement     */
#define SECX509_KEY_USAGE_KEY_CRT   (0x0008) /* key cert sign     */
#define SECX509_KEY_USAGE_CRL_SIG   (0x0004) /* CRL sign          */
#define SECX509_KEY_USAGE_ENC_OLY   (0x0002) /* encipher only     */
#define SECX509_KEY_USAGE_DEC_OLY   (0x0001) /* decipher only     */
/* Extended Key Usage Masks */
#define SECX509_EX_KEY_USAGE_SAUTH  (0x0001) /* TLS Web Server Authentication*/
#define SECX509_EX_KEY_USAGE_CAUTH  (0x0002) /* TLS Web Client Authentication*/
#define SECX509_EX_KEY_USAGE_CODE   (0x0004) /* Downloadable Code Signing    */
#define SECX509_EX_KEY_USAGE_EMAIL  (0x0008) /* Email Protection             */
#define SECX509_EX_KEY_USAGE_TIME   (0x0010) /* Time Stamping                */
#define SECX509_EX_KEY_USAGE_SGC    (0x0020) /* Secured Gated Crypto         */
/* Constants for parsing x509 OU field */
#define SECX509_OU_FIELD_INDEX_LEN  (0x2) /* the length of chars for the OU field index */
#define SECX509_OU_FIELD_SPACE_LEN  (0x1) /* the length of the space char*/

/* Index of attest/subca/root certificate within cert chain supported by the x509 parser */
#define ATTEST_CERT_INDEX        0
#define CA_CERT_INDEX            1
#define ROOT_CERT_INDEX          2

/**< Size of Hash Digest. */
#define SECX509_DIGEST_SIZE_SHA256 (32)
#define SECX509_DIGEST_SIZE_SHA384 (48)

/* get hash len */
#define SECX509_HASH_LEN(hashtype) \
  ((hashtype) == E_X509_CODE_HASH_SHA256 ? SECX509_DIGEST_SIZE_SHA256 : \
  ((hashtype) == E_X509_CODE_HASH_SHA384 ? SECX509_DIGEST_SIZE_SHA384 : \
  0))

/*===========================================================================
MACRO SECX509_UPDATE_OU_FIELD_VALUE

DESCRIPTION
This macro check the status after the pbl_secx509_parse_ou_field call:
if ou field is found, it updates the ou field variable in the assignment;
if ou field is not found, it updates the status to success, not an error;
if any error, it breaks out the loop with the error code.

DEPENDENCIES
this must be called within a loop or switch statement so it can break
from execution.

PARAMETERS
ou_field_value_to_update - the variable to update
parsed_value - the value to be assigned to the variable

SIDE EFFECTS
NONE
===========================================================================*/
#define SECX509_UPDATE_OU_FIELD_VALUE(status, ou_field_value_to_update, parsed_value) \
if(E_X509_SUCCESS == status) \
    { \
    ou_field_value_to_update = parsed_value; \
    } \
    else if(E_X509_OU_FIELD_NOT_FOUND == status) \
    { \
    status = E_X509_SUCCESS; \
    } \
    else \
    { \
    break; \
    }
/*==========================================================================

                       Type Definitions

==========================================================================*/
/* Public Key algorithms in the certificate */
typedef enum
{
  SECX509_PUBKEY_RSA = 0,
  SECX509_PUBKEY_DSA = 1,
  SECX509_PUBKEY_DH  = 2,
  SECX509_PUBKEY_ECDSA = 3,
  SECX509_PUBKEY_MAX,               /* Last one, for error checking */
  SECX509_RESERVED_1 = 0x7FFFFFFF
} pbl_secx509_pubkey_algo_type;

/* Certificate signature algorithm type */
typedef enum
{
  SECX509_md5WithRSAEncryption    = 0,
  SECX509_md2WithRSAEncryption    = 1,
  SECX509_sha384WithRSAEncryption = 4,
  SECX509_ecdsaWithSHA384         = 5,
  SECX509_RSAPSSEncryption        = 6,
  SECX509_SIG_ALGO_MAX,                 /* Last one, for error checking */
  SECX509_RESERVED_2            = 0x7FFFFFFF
} pbl_secx509_sig_algo_type;

/*RSA PSS signature parameters*/
typedef struct pbl_secx509_pss_param_type
{
  uint32  hash_algo;
  uint32  mgf_algo;
  uint32  salt_len;

} pbl_secx509_pss_param_type;

/* RSA public key parameters */
typedef struct pbl_secx509_rsa_pubkey_type
{
  uint32  mod_len;
  const uint8   *mod_data;
  uint32  exp_e_len;
  const uint8   *exp_e_data;

} pbl_secx509_rsa_pubkey_type;

/* ECC public key parameters (section 2.3.5 in RFC3279) */
typedef struct pbl_secx509_ecdsa_pubkey_type
{
  const uint8   *x_data;
  uint32  x_len;
  const uint8   *y_data;
  uint32  y_len;

} pbl_secx509_ecdsa_pubkey_type;

/* Union of all the public key types */
typedef struct pbl_secx509_pubkey_type
{
  pbl_secx509_pubkey_algo_type  algo;
  union
  {
    pbl_secx509_rsa_pubkey_type  rsa;
    pbl_secx509_ecdsa_pubkey_type  ecdsa;
  }key;

} pbl_secx509_pubkey_type;

/* Signature Padding Scheme Structure */
typedef union pbl_secx509_pkcs_param_type
{
    boolean use_hash_id;
} pbl_secx509_pkcs_param_type;

/* Signature Structure */
typedef struct pbl_secx509_rsa_sig_type
{
  secasn1_data_type  sig_data;
  union 
  {
    pbl_secx509_pss_param_type  pss_param;
    pbl_secx509_pkcs_param_type pkcs_param;
  } padding;

} pbl_secx509_rsa_sig_type;

/* ECC signature parameters (Section 2.2.3 RFC3279) */
typedef struct pbl_secx509_ecdsa_sig_type
{
  secasn1_data_type  r;
  secasn1_data_type  s;

} pbl_secx509_ecdsa_sig_type;

/* Signature Structure */
typedef struct pbl_secx509_signature_type
{
  pbl_secx509_sig_algo_type   algo_id;
  union
  {
    pbl_secx509_rsa_sig_type  rsa;
    pbl_secx509_ecdsa_sig_type  ecdsa;
  }sig;

} pbl_secx509_signature_type;

/* Distinguished name structure */
typedef struct pbl_secx509_dn_type
{
  uint32             num_attrib;
  secasn1_data_type  data;

} pbl_secx509_dn_type;

/* Version structure */
typedef struct pbl_secx509_version_type
{
  uint32             ver;
  secasn1_data_type  val;

} pbl_secx509_version_type;

/* Time structure */
typedef struct pbl_secx509_time_type
{
  uint32             time;
  secasn1_data_type  data;

} pbl_secx509_time_type;

/* Authority Key Identifier structure */
typedef struct pbl_secx509_auth_key_id_type
{
  boolean            set;
  secasn1_data_type  key_id;
  pbl_secx509_dn_type    name;
  secasn1_data_type  serial_number;

} pbl_secx509_auth_key_id_type;

/* Subject Key Identifier structure */
typedef struct pbl_secx509_subject_key_id_type
{
  boolean            set;
  secasn1_data_type  key_id;

} pbl_secx509_subject_key_id_type;

/* Key Usage structure */
typedef struct pbl_secx509_key_usage_type
{
  uint32   val;
  boolean  set;

} pbl_secx509_key_usage_type;

/* CA structure */
typedef struct pbl_secx509_ca_type
{
  boolean  set;
  boolean  val;

} pbl_secx509_ca_type;

/* Extension structure type */
typedef struct pbl_secx509_ext_type
{
  boolean                          set;
  pbl_secx509_auth_key_id_type     auth_key_id;
  pbl_secx509_subject_key_id_type  subject_key_id;
  pbl_secx509_key_usage_type       key_usage;
  pbl_secx509_key_usage_type       ex_key_usage;
  int32                            path_len;
  pbl_secx509_ca_type              ca;

} pbl_secx509_ext_type;

/* Certificate information structure */
typedef struct pbl_secx509_cert_info_type
{
  pbl_secx509_version_type     version;
  secasn1_data_type            serial_number;
  pbl_secx509_signature_type   algorithm;
  pbl_secx509_dn_type          issuer;
  pbl_secx509_time_type        not_before;
  pbl_secx509_time_type        not_after;
  pbl_secx509_dn_type          subject;
  secasn1_bit_string_type      issuer_unique_id;
  secasn1_bit_string_type      subject_unique_id;
  pbl_secx509_ext_type         extension;

} pbl_secx509_cert_info_type;

/* Certificate structure */
typedef struct pbl_secx509_cert_type
{
  /* The cert_info needs to be the first member */
  pbl_secx509_cert_info_type  cert_info;
  const uint8*                      data_ptr;    //where the certificate data starts
  uintnt                      cinf_offset; //where the certificate actually starts -
                                           //after the initial tag/len
  uintnt                      cinf_byte_len; //length of where the certificate actually starts
                                             //upto (but not including) the certificate signature
  uintnt                      asn1_size_in_bytes; //size of the entire certificate (including the initial tag/len)

  /* Signature info on the cert */
  pbl_secx509_pubkey_type     pkey;
  pbl_secx509_signature_type  sig_info;
  
  /*For verification */
  uint8                       cert_hash[SECBOOT_HASH_DIGEST_BYTE_SIZE];
} pbl_secx509_cert_type;



/* Certificate list struct */
typedef struct pbl_secx509_cert_list_struct
{
  pbl_secx509_cert_type  cert[SECBOOT_MAX_CERT_CHAIN_LEVEL];
  uint32               size;

} pbl_secx509_cert_list_type;

/* certificate list context type */
typedef struct
{
  uint32                       purpose;
  uint32                       trust;
  uint32                       depth;
  pbl_secx509_cert_list_type*  ca_list;

} pbl_secx509_cert_ctx_type;


typedef enum
{
  E_X509_SUCCESS = 0,
  E_X509_FAILURE,
  E_X509_NO_DATA,
  E_X509_DATA_INVALID,
  E_X509_BAD_DATA,
  E_X509_DATA_TOO_LARGE,
  E_X509_DATA_EXPIRED,
  E_X509_NO_MEMORY,
  E_X509_INVALID_ARG,
  E_X509_NOT_SUPPORTED,
  E_X509_OU_FIELD_NOT_FOUND,
  E_X509_RESERVED       = 0x7FFFFFFF
} secx509_errno_enum_type;

typedef enum
{
  E_X509_CODE_HASH_NOT_SPECIFIED = 0,
  E_X509_CODE_HASH_SHA1,
  E_X509_CODE_HASH_SHA256,
  E_X509_CODE_HASH_SHA384,
  E_X509_CODE_HASH_RESERVED       = 0x7FFFFFFF
}secx509_code_hash_algo_type;

// secboot control fields to determine how to authenticate image (from certificate OU field)
typedef struct secx509_metadata_info_type
{
  uint64                      debug_enable; /* for ou field, DEBUG */
  uint64                      sw_id; /* for ou field, SW_ID */
  uint64                      hw_id; /* for ou field, HW_ID */
  secx509_code_hash_algo_type code_hash_algo; /* for hash algorithm ou field */
  uint64                      crash_dump_enable; /* for ou field, CRASH_DUMP */
  uint64                      rot_ou_field; /* for ou field, ROT_EN */
  uint64                      in_use_soc_hw_version; /* for ou field, IN_USE_SOC_HW_VERSION */
  uint64                      ou_use_serial_num; /* for ou field, USE_SERIAL_NUMBER_IN_SIGNING */
  uint64                      oem_id_independent; /* for ou field, OEM_ID_INDEPENDENT */
  uint64                      is_sn_match_found;  /* for SN OU field, found the match of serial
                                                     number in SN OU field data. */
  secboot_fuse_info_type      *fuse_info;  /* fuse info type */
  uint64                      is_soc_hw_ver_match_found; /* for ou field, SOC_VERS, found the match of
                                                            device's soc hw version */
  uint64                      is_sn_ou_found; /* for SN OU field, SN OU field exists */
  uint64                      uie_key_switch_enable; /* for UIE_KEY_SWITCH_ENABLE OU field */
  uint16                      root_cert_sel; /* for ou field, ROOT_CERT_SEL */
  uint64                      root_revoke_activate_enable; /* for ou field, ROOT_REVOKE_ACTIVATE_ENABLE */
  uint32                      is_root_cert_sel_provided;  /* do not update root_cert_sel, it is provided */
} secx509_metadata_info_type;

#define PBL_MIN(x,y) (((x) <= (y)) ? (x) : (y))


/*==========================================================================

                      GLOBAL FUNCTION DECLARATIONS

==========================================================================*/

/*===========================================================================
FUNCTION pbl_secx509_parse_ou_field

DESCRIPTION
Parse OU field data and return the value as integer for the ou field.

DEPENDENCIES
None

PARAMETERS
*data_ptr           - Pointer to the searched data
*ou_field_value     - Pointer to the returned ou field value
ou_field_value_len  - Length of the ou field value (in bytes)
*ou_field_string    - Pointer to the queried ou field string
ou_field_string_len - Length of the queried ou field string (in bytes)

RETURN VALUE
E_SUCCESS         - if no error
x509 error number   - x509 error secx509_errno_enum_type

SIDE EFFECTS
None
===========================================================================*/
secx509_errno_enum_type pbl_secx509_parse_ou_field
(
secasn1_data_type           *data_ptr,
uint64                      *ou_field_value,
uint32                      ou_field_value_len,
const uint8                 *ou_field_string,
uint32                      ou_field_string_len
);

/*===========================================================================

FUNCTION SECX509_PARSE_ECDSA_SIGNATURE

DESCRIPTION
Parses the image signature (specifically for ECDSA)

DEPENDENCIES
None

PARAMETERS
data_ptr - current data holder position
sig_info     - signature info to store the signature

RETURN VALUE
E_X509_SUCCESS - if the signature is parsed correctly
E_INVALID_DATA - otherwise

SIDE EFFECTS
None
===========================================================================*/
secx509_errno_enum_type pbl_secx509_parse_ecdsa_signature
(
secasn1_data_type*  data_ptr,
pbl_secx509_signature_type*  sig_info
);

/*===========================================================================

FUNCTION SECX509_PARSE_CERTIFICATE

DESCRIPTION
  Parses an X509 certificate from the data_ptr, if *cert == NULL then the
  certificate is allocated from the memory pool, if the certificate is
  already allocated then the function just uses the pre-allocated memory

DEPENDENCIES
  None

PARAMETERS
  data_ptr - pointer to the raw certificate data
  data_bound - cert must end before here
  cert - pointer to the certificate
  ou_field_info_ptr - pointer to store OU Field values

RETURN VALUE
  E_SUCCESS if the certificate is parsed properly
  E_DATA_INVALID if the certificate cannot be parsed properly
  E_NO_MEMORY if no more memory slots are available for the certs
  E_NOT_SUPPORTED if an algorithm found in the cert is not supported
  E_INVALID_ARG if a pointer argument is NULL
  E_FAILURE if the *cert is pre-allocated but not *cert->cert or if the
            certificate data length does not match cert_len

SIDE EFFECTS
  None
===========================================================================*/
extern secx509_errno_enum_type pbl_secx509_parse_certificate
(
  const uint8                *cert_data,  /* cert data in the DER format */
  const uint8                *data_bound, /* parsing must stop before this address */
  pbl_secx509_cert_type      *cert,       /* pointer to the parsed struct*/
  secx509_metadata_info_type *ou_field_info_ptr
);

/*===========================================================================
FUNCTION pbl_secx509_parse_cert_buffer

DESCRIPTION
  Parses binary x509 certificates from memory into the buffer.
  parsing is done in order (attest cert first, CA cert next and
  root cert last).

DEPENDENCIES
  None

PARAMETERS
  cert_chain_ptr          - points to x509 certificate chain (in Flash)
                            CA cert and Attestation cert are here
  cert_chain_len          - upper bound on length of chain (parse fails
                            if chain appears to cross exceed this length,
                            or if the space between the last cert and
                            this length has any bytes that are not 0xFF).
  pbl_secx509_cert_list   - destination for all the certificates
                            after they have been parsed in order of chaining.
  metadata_info_ptr       - pointer to store meta value (from cert OU fields)
                            this could be NULL if not needed by the caller
  mrc_fuse_info_ptr       - pointer to mrc fuse values

RETURN VALUE
  E_SUCCESS on successful parsing of all certs
  E_FAILURE on unsuccessful parsing of all certs

SIDE EFFECTS
  None
===========================================================================*/
secx509_errno_enum_type pbl_secx509_parse_cert_buffer
(
  const uint8*                 cert_chain_ptr,
  uint32                       cert_chain_len, /* including tail padding */
  pbl_secx509_cert_list_type*  pbl_secx509_cert_list,
  secx509_metadata_info_type*  metadata_info_ptr,
  secboot_mrc_fuse_data_type*  mrc_fuse_info
);

/*===========================================================================
FUNCTION pbl_secx509_check_cert_list

DESCRIPTION
  Simpy checks if the certificate information, over the whole chain
  is valid and that the issuer and subject ID's are chained
  consecutively. Cryptographic signature verification down the chain
  is _NOT_ done here.

DEPENDENCIES
  None

PARAMETERS
  *cert_list_ptr  - Pointer to array of parsed certificates
  *check_ctx              - Sets of the parameters for the checking process

RETURN VALUE
  E_SUCCESS       - if no error
  E_DATA_INVALID  - Invalid Data
  E_NOT_SUPPORTED - Unknown Data

SIDE EFFECTS
  None
===========================================================================*/
secx509_errno_enum_type pbl_secx509_check_cert_list
(
  const pbl_secx509_cert_list_type *cert_list_ptr,
  const pbl_secx509_cert_ctx_type  *check_ctx
);

/*===========================================================================
FUNCTION pbl_secx509_search_ou_soc_vers

DESCRIPTION
  Search OU field SOC_VERs and see if deivce's soc hw version is within the
  SOC_VERS ou field (the authorized list). this searches all soc hw version
  entries within SOC_VERS (including padding 0) and returns SUCCESS if soc 
  hw version match is found.

DEPENDENCIES
  None

PARAMETERS
  *data_ptr           - Pointer to the searched data
  cert_field_info       - Pointer to ou field info structure

RETURN VALUE
  E_X509_SUCCESS      - if soc hw verion match is found
  E_X509_OU_FIELD_NOT_FOUND - if soc hw verion match is not found
  x509 error number   - x509 error secx509_errno_enum_type

SIDE EFFECTS
  None
===========================================================================*/
secx509_errno_enum_type pbl_secx509_search_ou_soc_vers
(
  secasn1_data_type           *data_ptr,
  secx509_metadata_info_type  *cert_field_info
);

/*===========================================================================
FUNCTION pbl_secx509_get_data_pointer

DESCRIPTION
  Search and get the pointer to data session within x509 cert data boundary 
  based on the offset and the length of the data session.

DEPENDENCIES
  None

PARAMETERS
  *data         - Pointer to the x509 cert data
  data_offset   - the offset to the data session
  data_len      - the length of the data session
  **data_ptr    - the pointer to pointer of the data session

RETURN VALUE
  E_X509_SUCCESS        - if no error
  x509 error number     - x509 error secx509_errno_enum_type

SIDE EFFECTS
  None
===========================================================================*/
secx509_errno_enum_type pbl_secx509_get_data_pointer
(
  secasn1_data_type *data,
  uint32 data_offset,
  uint32 data_len,
  const uint8  **data_ptr
);

/*===========================================================================
FUNCTION pbl_secx509_convert_string_to_unsigned_int

DESCRIPTION
  convert string to unsigned integer

DEPENDENCIES
  None

PARAMETERS
  *data_string_ptr   - Pointer to the data string
  data_string_len    - data string length
  data_value         - converted unsigned integer

RETURN VALUE
  E_SUCCESS         - if no error
  x509 error number   - x509 error secx509_errno_enum_type

SIDE EFFECTS
  None
===========================================================================*/
secx509_errno_enum_type pbl_secx509_convert_string_to_unsigned_int
(
  const uint8* data_string_ptr,
  uint32 data_string_len,
  uint64* data_value
);

/* ---------------------------------------------------------------------- */
/*                       MRC X509 Functions                               */
/* ---------------------------------------------------------------------- */
/*===========================================================================

FUNCTION SECX509_PARSE_MRC_DN

DESCRIPTION
Parse a MRC info in the distinguished name(DN) field of an X509 certificate,
including RO0T_CERT_SEL and ROOT_REVOKE_ACTIVATE_ENABLE.

DEPENDENCIES
None

PARAMETERS
ou_field_info_ptr - pointer to store OU Field values
dat_ptr - pointer to the data holder
RETURN VALUE
E_X509_SUCCESS - if the version is parsed correctly
E_INVALID_DATA - otherwise

===========================================================================*/
secx509_errno_enum_type pbl_secx509_parse_mrc_dn
(
secx509_metadata_info_type  *ou_field_info_ptr,
secasn1_data_type *dat_ptr
);

/*===========================================================================
FUNCTION pbl_secx509_cal_mrc_cert_num

DESCRIPTION
Parses binary x509 certificates from memory into the buffer.
parsing is done in order (attest cert first, CA cert next and
root cert last).

DEPENDENCIES
None

PARAMETERS
cert_chain_ptr          - points to x509 certificate chain (in Flash)
CA cert and Attestation cert are here
cert_chain_len          - upper bound on length of chain (parse fails
if chain appears to cross exceed this length,
or if the space between the last cert and
this length has any bytes that are not 0xFF).
pbl_secx509_cert_list   - destination for all the certificates
after they have been parsed in order of chaining.
ou_field_info_ptr       - pointer to store OU Field values

RETURN VALUE
E_X509_SUCCESS on successful parsing of all certs
E_X509_FAILURE on unsuccessful parsing of all certs
E_X509_INVALID_ARG on invalid arguments

SIDE EFFECTS
None
===========================================================================*/
secx509_errno_enum_type pbl_secx509_cal_mrc_cert_num
(
const uint8*                 cert_chain_ptr,
uint32                       cert_chain_len, /* including tail padding */
uint32*                      total_cert_num_ptr,
secboot_mrc_fuse_data_type*  mrc_fuse_info
);

/*===========================================================================

FUNCTION SECX509_SKIP_NUM_CERTS

DESCRIPTION
Skips over number of certificates and sets the chain ptr to point to the
next certificate

DEPENDENCIES
None

PARAMETERS
cert_chain_pptr         - points to ptr of x509 certificate chain
cert_chain_len          - upper bound on length of chain (parse fails
if the length is crossed when skipping
certs)
num_certs_to_skip       - number of certs to skip in the chain

RETURN VALUE
E_X509_SUCCESS on successful skipping of certs
E_X509_FAILURE on unsuccessful skipping of all certs

SIDE EFFECTS
None
===========================================================================*/
secx509_errno_enum_type pbl_secx509_skip_num_certs(
  const uint8**                 cert_chain_ptr,
  const uint8*                  cert_chain_bound,
  uint8                         num_certs_to_skip
  );

/*===========================================================================

FUNCTION SECX509_GET_ASN1_CERT_SIZE

DESCRIPTION
Gets the size (in bytes) of the certificate

DEPENDENCIES
None

PARAMETERS
data_ptr          - pointer to the raw certificate data
data_bound        - address beyond which the certificate does not cross into
asn1_size_in_bytes - pointer to the data to be filled with the asn1 size

RETURN VALUE
E_X509_SUCCESS - if the data was parsed properly to get the certificate size
E_X509_INVALID_ARG - if a pointer argument is NULL
E_X509_DATA_INVALID - if the length is greater than SECASN1_MAX_LEN or is more than
data_bound argument

SIDE EFFECTS
None
===========================================================================*/
secx509_errno_enum_type pbl_secx509_get_asn1_cert_size
(
const uint8*                 data_ptr,
const uint8*                 data_bound,
uint32*                      asn1_size_in_bytes_ptr
);


/*===========================================================================

FUNCTION SECX509_CAL_CERT_NUM

DESCRIPTION
calculate the total number of certificates

DEPENDENCIES
None

PARAMETERS
cert_chain_pptr         - points to ptr of x509 certificate chain
cert_chain_len          - upper bound on length of chain (parse fails
if the length is crossed when skipping
certs)
fuse_info_ptr           - pointer to the fuse info

RETURN VALUE
E_X509_SUCCESS on successful skipping of certs
E_X509_FAILURE on unsuccessful skipping of all certs

SIDE EFFECTS
None
===========================================================================*/
secx509_errno_enum_type pbl_secx509_cal_cert_num
(
const uint8                 *cert_chain_ptr,
uint32                       cert_chain_len,
uint32                      *total_cert_num
);

/*===========================================================================

FUNCTION SECX509_VALIDATE ROOT CERT

DESCRIPTION
validate root certificate

DEPENDENCIES
None

PARAMETERS
root_cert               - pointer to the parsed root certificate

RETURN VALUE
E_X509_SUCCESS on successful skipping of certs
E_X509_FAILURE on unsuccessful skipping of all certs

SIDE EFFECTS
None
===========================================================================*/
secx509_errno_enum_type pbl_secx509_validate_root_cert(
  pbl_secx509_cert_type*       root_cert
);

/*===========================================================================

FUNCTION SECX509_VALIDATE SUBCA CERT

DESCRIPTION
validate subca certificate

DEPENDENCIES
None

PARAMETERS
verifier_cert              - pointer to the parsed certificate to verify cert
verifiee_cert              - pointer to the parsed subca certificate being verified

RETURN VALUE
E_X509_SUCCESS on successful skipping of certs
E_X509_FAILURE on unsuccessful skipping of all certs

SIDE EFFECTS
None
===========================================================================*/
secx509_errno_enum_type pbl_secx509_validate_subca_cert(
  pbl_secx509_cert_type*           verifier_cert,
  pbl_secx509_cert_type*           verifiee_cert
);

/*===========================================================================

FUNCTION SECX509_VALIDATE ATTEST CERT

DESCRIPTION
validate attest certificate

DEPENDENCIES
None

PARAMETERS
subca_cert              - pointer to the parsed subca certificate
attest_cert             - pointer to the parsed attest certificate being verified

RETURN VALUE
E_X509_SUCCESS on successful skipping of certs
E_X509_FAILURE on unsuccessful skipping of all certs

SIDE EFFECTS
None
===========================================================================*/
secx509_errno_enum_type pbl_secx509_validate_attest_cert(
  pbl_secx509_cert_type*           subca_cert,
  pbl_secx509_cert_type*           attest_cert
);
#endif /* SECBOOT_X509_H */
