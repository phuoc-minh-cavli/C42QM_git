#ifndef SECTEST_H
#define SECTEST_H

#if (defined(FEATURE_SEC_CRYPT_TESTAPP) || defined(FEATURE_SEC_SSL_TESTAPP))

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                             S E C T E S T    

GENERAL DESCRIPTION
  The Security Services Unit Test functions test the crypto layer functions 
  for public key exchange, message digest, and bulk encryption  
  in the crypto layer using the internal (seccrypt) programming interface.
  These functions should be invoked by a task other than the 
  SEC task, usually the SECTESTAPP task.  

  The external interface to the API-level tests is defined in secapitest.h.
  The internal interfaces between the test files plus the declarations of
  the tests of the internal interface are defined in this file.

  The tests of the internal cipher  interface are defined in sectestcipher.c.
  The tests of the internal hash    interface are defined in sectesthash.c.
  The tests of the internal modmath interface are defined in sectestmodmath.c.
  Several very large data sets (no functions) are defined in seccipherdata.c.
  
EXTERNAL FUNCTIONS

  sectest_all()
  sectest_modmath()
  sectest_hash()
  sectest_cipher()
  sectest_custom()

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2001-2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/COMMON/vcs/sectest.h_v   1.13   22 Oct 2003 11:58:52   omichael  $
  $Header: //components/rel/dataiot.mpss/2.0/protocols/sec/ssl/testapp/src/sectest.h#1 $ 
  $DateTime: 2020/03/17 08:03:16 $ 
  $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/30/12   nk      ZUC algorithm ported. 
09/07/10   ejt     Removed DEBUG_SEC_DSP_DME_TEST 
02/23/10   ejt     Removed all arc4 and des longtests. 
08/31/09   sm      Added test to verify real hardware key is being read
06/10/09   ejt     Removed seccrypt.h and updated code to use secapi APIs
05/05/04   rwh     Commented out #define FEATURE_SEC_DRM_RDB_UNIT_TEST.
04/20/04   rwh     Mods for RDB unit test.
04/05/04   rwh     Added comments.
04/01/04   rwh     Increased the dh key sizes to SECAPI_PKX_MAX_KEY_SIZE
03/30/04   rwh     Added cipher and modmath adversarial tests.
03/09/04   rwh     Added HMAC test functions and data for API tests.
02/25/04   rwh     Added cipher and hash algorithms lists.
02/23/04   rwh     Added sectest_new_dsm_chain_with_nasty_alignments().
02/20/04   rwh     Added sectest_new_dsm_chain_from_buffer().
02/05/04   rwh     Added support for sectest.c split into smaller files.
12/16/03   yhw     Added AES128 support.
11/25/03   rwh     Added storage for PKX handle in the PKX test set.
11/13/03   rwh     Created sectest_pkx_testset_type.
11/11/03   rwh     Removed extern for cb_errno.
11/05/03   rwh     Added sectest_pkx_set_dh_private_key() to test modmath.
                   Added includes of secapi.h and seccrypt.h.
                   Added FEATURE_SECTEST_AVOID_ECB_MODE
10/21/03   jay     Added externs for MD5 test
09/19/03   rwh     Added prototypes and externs.
08/08/03   rwh     Added prototype for secapitest_compare_results().
08/06/03   rwh     Added externs for VERY_LARGE_DATA_SETS.
07/22/03   rwh     Moved rex timer to sectestapp.c, added prototype for
                   sectestapp_delay().  Added storage for appended MAC to 
                   secssltest_ buf's.  Added prototype for UI-based tests.
04/21/03   rwh     Added extra bytes to the key buffers.
01/16/03   rwh     Added declaration of crypto unit test master driver.
12/03/02   rwh     Added shared declarations to support crypto API tests.
05/03/02   dld     Added Debug for DSP DME testing
01/12/01   yll     Created file

===========================================================================*/

/*===========================================================================

                             INCLUDE FILES

===========================================================================*/

#include "rex.h"

#include "nv.h"

#include "secapi.h"

/*===========================================================================

                             MACRO DEFINITIONS

===========================================================================*/

#define FEATURE_SEC_DME_PARAM_XFER /* data values for Phase 3 DSP i/f.*/

//#define FEATURE_SEC_DRM_RDB_UNIT_TEST /* enable DRM RDB unit test */


#ifndef SSL_V3_MAX_RECORD_LENGTH
  #define SSL_V3_MAX_RECORD_LENGTH     16384
#endif  

#undef FEATURE_SECTEST_AVOID_ECB_MODE

#define SECTEST_SHORT_DELAY_PERIOD_MS   5   /* msec */
#define SECTEST_LONG_DELAY_PERIOD_MS  100   /* msec */

#define SECTEST_MAX_CONCURRENT_INSTANCES (11)

#define MAX_BYTES_AT_TIME 2000
#define TOTAL_MSG_BYTE_SIZE 1000000 

/*===========================================================================

            REGIONAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/* The list of platforms to test when looping through
   each of the platforms.
*/
#define SECTEST_PLATFORM_LIST_SIZE (3)

typedef struct
{
  uint16           size;
  secapi_platform_enum_type platform[ SECTEST_PLATFORM_LIST_SIZE];
} sectest_platform_list_type;

extern sectest_platform_list_type sectest_platform_list;

/* Use the following #define's until the API's enum and set_param()
   work correctly to set the platform. */

/* The list of platforms to test when looping through
   each of the platforms. */
#define SECAPITEST_PLATFORM_LIST_SIZE (3)

typedef struct
{
  uint16           size;
  secapi_exec_platform_param_data_type platform[ SECAPITEST_PLATFORM_LIST_SIZE];
} secapitest_platform_list_type;

extern secapitest_platform_list_type secapitest_platform_list;

/* The list of ciphers to test when looping through
   each of the ciphers. */

#define SECAPITEST_ALGO_LIST_SIZE   (SECAPI_ALGO_MAX)

typedef struct
{
  uint16                size;
  secapi_algo_enum_type algo[ SECAPITEST_ALGO_LIST_SIZE];
} secapitest_algo_list_type;

#define SECAPITEST_CIPHER_LIST_SIZE (4)
extern secapitest_algo_list_type secapitest_cipher_list;

/* The list of hashs to test when looping through
   each of the hashs. */
#define SECAPITEST_HASH_LIST_SIZE   (2)
extern secapitest_algo_list_type secapitest_hash_list;

#define SECAPITEST_MAX_CONCURRENT_INSTANCES (11)


/* Supply inputs and reference output to a PKX test. */
typedef struct sectest_pkx_testset_struct
{
  /* Setup info. */
  secapi_handle_type           handle;
  secapi_algo_enum_type        algo;
  secapi_exec_mode_enum_type   exec_mode;
  secapi_exec_platform_param_data_type exec_platform;
  
  /* Running info. */
  secerrno_enum_type           sec_errno;
  
  /* Reference input and reference output. */
  uint8                        *base_ptr;
  uint16                       base_len;
  uint8                        *mod_ptr;
  uint16                       mod_len;
  uint8                        *exp_ptr;
  uint16                       exp_len;
  
  uint8                        *ref_out_ptr;
  uint16                       ref_out_len;
} sectest_pkx_testset_type;

/* Supply inputs and reference output to a cipher test. */
typedef struct sectest_cph_testset_struct
{
  /* Setup info. */
  secapi_handle_type              handle;
  secapi_algo_enum_type           cipher;
  secapi_enc_input_mode_enum_type input_mode;
  secapi_enc_op_mode_enum_type    op_mode;
  secapi_enc_padding_enum_type    padding;
  secapi_exec_mode_enum_type      exec_mode;
  secapi_exec_platform_param_data_type exec_platform;

  /* MAC-related stuff. */
  secapi_hsh_hmac_enum_type    hmac;
  secapi_handle_type           mac_handle;

  /* Running info. */
  secerrno_enum_type           sec_errno;
  uint32                       mismatch_offset;
  uint32                       iteration;
  uint32                       byte_cnt;

  /* Reference input and reference output. */
  uint8                        *enc_key_ptr;
  uint8                        *enc_iv_ptr;
  uint8                        *dec_key_ptr;
  uint8                        *dec_iv_ptr;
  
  uint8                        *ref_in_ptr;
  uint32                       ref_in_len;
  uint8                        *ref_out_ptr;
  uint32                       ref_out_len;
} sectest_cph_testset_type;

/* Supply inputs and reference output to a hash test. */
typedef struct sectest_hsh_testset_struct
{
  /* Setup info. */
  secapi_handle_type           handle;
  secapi_algo_enum_type        hash;
  secapi_hsh_mode_enum_type    input_mode;
  secapi_exec_mode_enum_type   exec_mode;
  secapi_exec_platform_param_data_type exec_platform;
  
  /* Running info. */
  secerrno_enum_type           sec_errno;
  uint32                       mismatch_offset;
  uint32                       iteration;
  uint32                       byte_cnt;
  
  /* Reference input and reference output. */
  uint8                        *ref_in_ptr;
  uint32                       ref_in_len;
  uint8                        *ref_out_ptr;
  uint16                       ref_out_len;

  uint8                        *enc_mac_secret_ptr;
  uint8                        *dec_mac_secret_ptr;
  
} sectest_hsh_testset_type;

extern boolean   sectest_first_run;

/* Test data for rx / tx tests.  
The additional bytes make room for MAC's and run-time alignment tweaks.
*/


extern uint8*   secssltest_ref_pattern;
extern uint8*   secssltest_pattern;
extern uint8*   secssltest_mid_pattern;
                                                      

extern char  sectest_msg[128];

#define MAX_ITERATION_SETS 10

typedef struct
{
  int start;
  int end;
  int increment;
} iteration_set_type;

// #2 (1024 bit precision) 


/* Modulus value (in network order) taken from RFC 2412. */


/* Use the following as the MAC secret key. 
Yes, it is larger than necessary...
*/

/* Initialization vector values */
extern uint8* const_ivec64_enc_buf;

/* These values can be overwritten. */
extern uint8* ivec64_enc_buf;

extern uint8* const_ivec128_enc_buf;
extern uint8* ivec128_enc_buf;

/* These values are required for one of the tests. */
extern uint8* const_ivec64_dec_buf;

/* These values can be overwritten. */
extern uint8* ivec64_dec_buf;

extern uint8* const_ivec128_dec_buf;
extern uint8* ivec128_dec_buf;



/* Data for the DH key exchange. */
extern uint8* secapitest_public_keyA;  
                                   /* add 4 bytes to try other alignments */
extern uint8* secapitest_secret_keyA;  
                                   /* add 4 bytes to try other alignments */
extern uint8* secapitest_public_keyB;  
                                   /* add 4 bytes to try other alignments */
extern uint8* secapitest_secret_keyB;  
                                   /* add 4 bytes to try other alignments */

extern uint8* md5_test1_output_buf;
extern uint8* sha_test2_output_buf;
extern uint8* sha_test4_expected_output;

/* The following 3 HMAC data sets are from RFC 2104. */
extern uint8* secapitest_hmac_key1;
extern uint8 secapitest_hmac_data1 [ 8]; 
extern uint8* secapitest_hmac_digest_ref1; 

extern uint8 secapitest_hmac_key2  [ 4]; 
extern uint8 secapitest_hmac_data2 [ 28]; 
extern uint8* secapitest_hmac_digest_ref2; 

extern uint8* secapitest_hmac_key3;
extern uint8* secapitest_hmac_data3;
extern uint8* secapitest_hmac_digest_ref3; 

extern uint8* secapitest_hmac_key4; 
extern uint8* secapitest_hmac_digest_ref4; 

extern uint8* secapitest_keygen_data_sha_result;
extern uint8* secapitest_keygen_data_md5_result;
extern uint8 secapitest_abc_data[ 3];
extern uint8* secapitest_abc_data_md5_result;

//#define FEATURE_SEC_TESTAPP_VERY_LARGE_DATA_SETS

#ifdef FEATURE_SEC_TESTAPP_VERY_LARGE_DATA_SETS

#define ARC4_NUM_BYTES_LONGTEST  55299
extern uint8* arc4_key_longtest;
extern uint8* arc4_ct_longtest;

#define DES_NUM_BYTES_LONGTEST  55296
extern uint8* des_key_longtest;
extern uint8* des_iv_longtest;
extern uint8* des_ct_longtest;

#define AES_NUM_BYTES_LONGTEST 55296
extern uint8* aes_key_longtest;
extern uint8* aes_iv_longtest;
extern uint8* aes_ct_longtest;


/* MD5 hash of all 55299 bytes of arc4_pt_longtest buffer.
*/ 
extern uint8* arc4_pt_longtest_md5_result;

/* SHA hash of all 55299 bytes of arc4_pt_longtest buffer.
*/
extern uint8* arc4_pt_longtest_sha_result;

#endif /* FEATURE_SEC_TESTAPP_VERY_LARGE_DATA_SETS */

/* Set the payload length tuples (start, end, increment) for the tests. */
extern iteration_set_type secapitest_iteration_sets[ ];
extern iteration_set_type secapitest_iteration_setarc4[ ];
extern iteration_set_type secapitest_iteration_setdes[ ];
extern iteration_set_type secapitest_iteration_setdes2[ ];
extern iteration_set_type secapitest_hash1M_iteration_sets1[ ];
extern iteration_set_type secapitest_hash1M_iteration_sets2[ ];
extern iteration_set_type secapitest_hash1M_iteration_sets5[ ];
extern iteration_set_type secapitest_hash1M_iteration_sets6[ ];
extern iteration_set_type secapitest_hash1M_iteration_sets3[ ];
extern iteration_set_type secapitest_hash1M_iteration_sets4[ ];

extern int sectest_len_incr1[8];
extern int sectest_len_incr2[8];
extern int sectest_len_incr3[8];
extern int sectest_len_incr4[8];

void sectest_aes_3( void );
void sectest_aes_2( void );
void sectest_aes_1( void );
uint16 sectest_elementary_tests( void );
void print_error (uint16, uint16, secerrno_enum_type);
int sectest_random_num( void); 
/*=================================================================
Deallocate a crypto instance using the internal interfaces.
=================================================================*/
extern secerrno_enum_type sectest_delete_instance
(
  secapi_handle_type          *handle_ptr
);

/*=================================================================
Free the dsm items that may be chained to our private dsm item.
=================================================================*/
void sectest_free_chained_items
(
  dsm_item_type            **item_ptr_ptr
);

/*=================================================================
Allocate a new hash instance and set the hash input mode.
=================================================================*/
secerrno_enum_type sectest_new_hash
(
  secapi_handle_type         *hash_handle_ptr,
  secapi_algo_enum_type       algo,
  secapi_hsh_mode_enum_type   mode,
  secapi_platform_enum_type   exec_platform
);

dsm_item_type* secapii_dsm_new_buffer
(
  uint8*  data_ptr,
  uint16  data_len
);

void secapii_dsm_free_buffer
(
  dsm_item_type**  item_ptr
);


/*===========================================================================

                     EXTERNAL FUNCTIONS

===========================================================================*/

/*===========================================================================

FUNCTION SECTESTAPP_GET_NV_ITEM

DESCRIPTION
  This function retrieves the specified item from NV.

DEPENDENCIES
  The NV task has been started and is running.
  This functions is Non-reentrant.

RETURN VALUE
  Status returned from the NV read request.  An ERR is logged if status is
  other than:
    NV_DONE_S       - request done
    NV_NOTACTIVE_S  - item was not active

SIDE EFFECTS
  While this function is running all other SEC TEST task activities are
  suspended except for watchdog kicking, and until the NV item is
  read in.

===========================================================================*/
extern nv_stat_enum_type sectestapp_get_nv_item
(
  nv_items_enum_type  item_code,       /* Item to get                      */
  nv_item_type        *item_ptr        /* Pointer where to put the item    */
);



/*===========================================================================

FUNCTION SECTESTAPP_DELAY

DESCRIPTION
  Delay the sectestapp task for "delay" milliseconds.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Lets lower-priority tasks run.
===========================================================================*/
extern void sectestapp_delay( 
  rex_timer_cnt_type   delay      /* value in milliseconds   */
);

/*==========================================================================
 FUNCTION SECTEST_XXX
  
 DESCRIPTION 
   These functions start the crypto layer testing of the key exchange, 
   message digest, or encrypt / decrypt using the specified algorithms.
  
 DEPENDENCIES 
   None

 RETURN VALUE 
   None
  
 SIDE EFFECTS
   None                          
===========================================================================*/

extern uint16 sectest_dh_create_pubkey_tests( void);
extern uint16 sectest_modmath(void);
extern uint16 sectest_custom( void);
extern uint16 sectest_hash( void);

/*==================================================================
Simple hash tests.
==================================================================*/
extern uint16 sectest_hash_simple_tests( void);

/*=================================================================
Supply hash payload at a constant level.
=================================================================*/
extern uint16 sectest_hash_steady_load_tests( void);

/*=================================================================
Supply hash payload at varying levels.
=================================================================*/
extern uint16 sectest_hash_variable_load_tests( void);

extern uint16 sectest_cipher( void);

extern void sectest_arc4(void);
extern void sectest_des(void);
extern void sectest_sha(void);
extern void sectest_md5(void);
extern void sectest_mac(void);
extern int  sectest_snow3g(void);
extern int  sectest_milenage(void);
extern int  sectest_zuc(void);
extern int  sectest_kasumi(void);

/*==========================================================================
 FUNCTION SECTEST_ALL
  
 DESCRIPTION 
   Invoke a sequence of functional tests which exercise the crypto layer.
   These functions start the crypto layer testing of the key exchange, 
   message digest, or encrypt / decrypt using the specified algorithms.
  
 DEPENDENCIES 
   None

 RETURN VALUE 
   None
  
 SIDE EFFECTS
   Lots during execution, but everything should be returned to normal 
   at the end.                          
===========================================================================*/

extern int sectest_all( void);


/*==================================================================
Cipher adversarial tests.
==================================================================*/
extern uint16 secapitest_cipher_adversarial_tests( void);


/*=================================================================
Deallocate a crypto instance.
=================================================================*/
extern secerrno_enum_type secapitest_delete_instance(
  secapi_handle_type          *handle_ptr
);


/*=================================================================
Compare result to reference data, output values on QXDM if no match,
return offset of mismatch, or zero if results match.
=================================================================*/
extern uint16 secapitest_compare_results(
  uint8                       *data_ptr,
  uint8                       *data_ref_ptr,
  uint16                      data_len
);

/*=================================================================
Given a large payload, choose smaller lengths to achieve 
greater code coverage, e.g. lengths of 1,2,block_size,..
=================================================================*/
extern uint32 secapitest_choose_interesting_length( 
  uint32                      byte_cnt,
  uint16                      block_size
);

/*===========================================================================
Set the private key value for a Public-Key_exchange (PKX) Diffie-Hellman
instance rather than using the random value.
===========================================================================*/
extern secerrno_enum_type sectest_pkx_set_dh_private_key(
  secapi_handle_type          handle,
  secapi_algo_enum_type       algo,
  uint8                       *private_key_ptr
);

/*===========================================================================
Modmath tests with adversarial data.
===========================================================================*/
extern uint16 sectest_modmath_adversarial_tests(void);


/*=================================================================
 Given a contiguous buffer, create the requested number of
 dsm headers and chain them together.
 The calling function is responsible for setting up the 
 payload-related fields: data_ptr, size, used.
                                                                         
PARAMETERS                                                              
  buf_ptr - Buffer to use for storage
  buf_len - Length of the buffer in bytes                    
  count   - Number of each headers needed
                                                                       
DEPENDENCIES                                                            
  None.          
                                                                        
RETURN VALUE                                                            
  A pointer to the DSM header chain.
=================================================================*/
dsm_item_type* sectest_new_dsm_chain_from_buffer
(
  uint32* buf_ptr, 
  uint32  byte_len,
  uint16  count
);

/*=================================================================
Given a contiguous buffer of input data (and its length), and
a vector of length increments to test (and its length)
create the dsm chain containing a copy of the input data.
Use normal-length dsm items to hold the remaining data after
exhausting the length increment vector.
The order of the length increments is designed to hit all the
combinations of odd/even length, odd/even starting byte address,
odd/even starting word address.
                                                                         
PARAMETERS                                                              
  buf_ptr   - Buffer to use for storage
  buf_len   - Length of the buffer in bytes                    
  incr_ptr  - Array of increment values
  incr_size - Number of elements in increment array
                                                                       
DEPENDENCIES                                                            
  None.          
                                                                        
RETURN VALUE                                                            
  A pointer to the DSM header chain.
=================================================================*/
dsm_item_type* sectest_new_dsm_chain_with_nasty_alignments
(
  uint8*          buf_ptr, 
  uint32          buf_len,
  const int       incr_ptr[],                                
  uint16          incr_size
);


extern uint16 secapitest_hash_simple_tests( void);
extern uint16 secapitest_hash_steady_load_tests( void);
extern uint16 secapitest_hash_variable_load_tests( void);

extern  int   secapitest_cipher_loopback_tests( void);

#ifdef FEATURE_SEC_DRM

extern void secdrmtest_add_right( void);
extern void secdrmtest_lookup_rights( void);
extern void secdrmtest_consume_right( void);
extern void secdrmtest_del_associated_rights( void);
extern void secdrmtest_delete_right( void);

#endif /* FEATURE_SEC_DRM */

#endif /* FEATURE_SEC_CRYPT_TESTAPP || FEATURE_SEC_SSL_TESTAPP */

#endif /* SECTEST_H */
