#ifndef SECIPSSP_H
#define SECIPSSP_H

/*===========================================================================

               A S W   S E C U R I T Y   S E R V I C E S

                        I P S E C   L A Y E R

         E X T E R N A L   A P I   A N D   D E F I N I T I O N S

    Copyright (c) 2013 - 2020 Qualcomm Technologies, Inc.  All Rights Reserved.
    Qualcomm Technologies Proprietary and Confidential.
===========================================================================*/

/**
  @file secipssp.h
  
  Provides an interface to the internal applications for IKEv2 security association. 
  These functions are used to establish an IPSEC tunnel which can further support the communication between the hosts 
  using their security policies.
  
 */

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/COMMON/vcs/secipssdb.h_v   1.5   30 Aug 2003 20:04:06   omichael  $
  $Header: //components/rel/dataiot.mpss/2.0/api/public/secipssp.h#1 $ $DateTime: 2020/03/17 08:03:16 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/27/20   ds      Enhanced defense mechanism to validate IPSEC SA ptr.
09/16/19   ds      Added entry in IPSEC SA structure to store SP Index/SPD  
                   pointer.
10/07/15   ks      Dynamic_src port for IKE negotiation support
09/29/15   nm      SHA256 support
04/28/14   nm      EAP_AKA FRA and pseudonym support
04/15/14   nm      NULL APN Support
10/31/13   nm      DHgroup 5 and 14 support
07/28/10   dm      Re-architectured IPsec API files
07/07/10   sw     (Tech Pubs) Edited/added Doxygen comments and markup.
06/11/10   ml      Removed seccrypt.h from header definitions.
09/22/09   nk      IPsec CMI Decoupling.
07/16/08   sp      Support for using configured DPD timer value.
07/09/08   sp      Added support for IKEv2 Multiple Authentication.
04/25/07   jjg     Interface changes for IWLAN and IKEv2
04/23/07   ssm/sl  IKEv2 support
02/26/07   ssm     Changes to support multiple selectors per SP
11/23/06   ssm/sv  Manual Keying/Transport Mode support
09/29/06   ssm     Fixes to make lint happy
05/17/05   sv      Added ipfliter handle to policy type.
04/20/05   ssm     Renamed secipssp_init to secipssp_new
01/25/05   sv/ssm  Re-architectured IPSEC to support multiple SA's and added
                   support for AH.
10/08/04   sv      Added support for creating multiple entries in SPD.
10/23/00   et      Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "secerrno.h"
#include "IPSec.h"
#include "secapi.h"
#include "ps_pkt_info.h"
#include "dsm.h"
#include "ds_ipfltr_defs.h"
#include "pstimer.h"

/* ---------- Not needed in the PDF - Start ---------- */
/** @cond */

#define SECIPSTYPES_ESP_AUTH_DATA_SIZE    12
#define SECIPSTYPES_ESP_SEQ_NUM_SIZE      4
#define SECIPSTYPES_IPSEC_SPI_SIZE      4
#define SECIPSTYPES_ESP_IV_SIZE           16
#define SECIPSTYPES_ESP_HDR_SIZE \
           (SECIPSTYPES_IPSEC_SPI_SIZE + SECIPSTYPES_ESP_SEQ_NUM_SIZE)
#define SECIPSTYPES_ESP_ALIGN_VALUE      16 
#define SECIPSTYPES_MAX_PAD_LEN          (SECIPSTYPES_ESP_ALIGN_VALUE - 1)
#define SECIPSTYPES_ESP_TRAILER_SIZE     2
#define SECIPSTYPES_MAX_ESP_HDR_SIZE \
           (SECIPSTYPES_ESP_HDR_SIZE + \
            SECIPSTYPES_ESP_IV_SIZE  + \
            SECIPSTYPES_MAX_PAD_LEN + \
            SECIPSTYPES_ESP_TRAILER_SIZE + \
            SECIPSTYPES_ESP_AUTH_DATA_SIZE)
#define SECIPSTYPES_GATEWAY_NATT_PORT    4500
#define SECIPSTYPES_AH_AUTH_DATA_SIZE    12
#define SECIPSTYPES_AH_HDR_SIZE  (SECIPSTYPES_AH_AUTH_DATA_SIZE + 12)
#define SECIPSTYPES_CRYPTO_ALGO_KEY_MAX_SIZE 32
#define SECIPSTYPES_HASH_MAX_SIZE \
         (MAX(SECAPI_HSH_MD5_DIGEST_SIZE,SECAPI_HSH_SHA256_DIGEST_SIZE))
#define SECIPSPKTDEFS_MAX_ATTRIBUTES 10
#define SECIPSPKTDEFS_MAX_TRANSFORMS 16
#define SECIPSSP_PSK_KEY_MAX_SIZE    32
#define SECIPSSP_MD5_PASSWORD_MAX_SIZE    255
#define SECIPSTYPES_PRF_AES128_XCBC_DIGEST_SIZE  16


/* ---------- Not needed in the PDF - End ---------- */
/** @endcond */

 /**
   @addtogroup ipsec_internal_ikev2_ds
 
   @{ */
 
 /* =================================================================
	  I P S E C   K e y   M a n a g e m e n t	O p t i o n s
 ================================================================= */
 /**
  * Key Managment Type
  */ 
 typedef enum
 {
   SECIPSSP_KEY_MGMT_UNDEFINED	 = IPSEC_KEY_MGMT_UNDEFINED,	/**< -- Initialization value */
   SECIPSSP_KEY_MGMT_IKEV1		 = IPSEC_KEY_MGMT_IKEV1,		/** &nbsp; */
   SECIPSSP_KEY_MGMT_IKEV2		 = IPSEC_KEY_MGMT_IKEV2,		/** &nbsp; */
   SECIPSSP_KEY_MGMT_MANUAL 	 = IPSEC_KEY_MGMT_MANUAL,		/**< -- Manual keying */
   SECIPSSP_KEY_MGMT_COUNT		/**< -- Maximum count */
 } secipssp_key_mgmt_enum_type;
 
 /**
  * Encapsulation method
  */
 typedef enum
 {
   SECIPSSP_ENCAPSULATE_UNDEFINED,	/** &nbsp; */
   SECIPSSP_ENCAPSULATE_TUNNEL,		/** &nbsp; */
   SECIPSSP_ENCAPSULATE_TRANSPORT	/** &nbsp; */
 } secipssp_encapsulate_enum_type;	
 
 /**
  * Authentication Type
  */ 
 typedef enum
 {
   SECIPSSP_AUTH_TYPE_INVALID_PARAM = 0,	/** &nbsp; */
   SECIPSSP_AUTH_TYPE_PSK			= 1,	/** &nbsp; */
   SECIPSSP_AUTH_TYPE_HYBRID		= 2,	/** &nbsp; */
   SECIPSSP_AUTH_TYPE_EAP_SIM_3GPP	= 3,	/** &nbsp; */
   SECIPSSP_AUTH_TYPE_EAP_AKA		= 4,	/** &nbsp; */
   SECIPSSP_AUTH_TYPE_EAP_TLS		= 5,	/** &nbsp; */
   SECIPSSP_AUTH_TYPE_CERTIFICATE	= 6,	/** &nbsp; */
   SECIPSSP_AUTH_TYPE_EAP_MD5		= 7,	/** &nbsp; */
   SECIPSSP_AUTH_TYPE_EAP_AKA_CAVE	= 8,		/** &nbsp; */
   SECIPSSP_AUTH_TYPE_EAP_AKA_SHA1  = 9,   /** &nbsp; */
   SECIPSSP_AUTH_TYPE_EAP_AKA_MILENAGE  = 10   /** &nbsp; */
 } secipssp_auth_type_param_data_type;
 
  /**
   @}
*/ 
/* end_group ipsec_internal_ikev2_ds */

/* ---------- Not needed in the PDF - Start ---------- */
/** @cond */
  
/* =================================================================
	   I P S E C S P  D i r e c t i o n
  ================================================================= */
  typedef enum
  {
	SECIPSSP_DIRECTION_RESERVED   = 0,
	SECIPSSP_DIRECTION_IN		  = 1,
	SECIPSSP_DIRECTION_OUT		  = 2,
	SECIPSSP_DIRECTION_INOUT	  = 3,
	SECIPSSP_DIRECTION_COUNT
  } secipssp_direction_enum_type;
    
  typedef enum
  {
   SECIPSSA_SA_STATE_UNDEFINED,
   SECIPSSA_SA_STATE_DOWN,
   SECIPSSA_SA_STATE_LARVAL,
   SECIPSSA_SA_STATE_PENDING_REKEY,
   SECIPSSA_SA_STATE_PENDING_DELETE,
   SECIPSSA_SA_STATE_COMPLETED
 } secipssa_sa_state_enum_type;
  
 typedef enum
 {
   SECIPSSA_SA_PROTOCOL_UNDEFINED,
   SECIPSSA_SA_PROTOCOL_ISAKMP   = 1,
   SECIPSSA_SA_PROTOCOL_AH       = 2,
   SECIPSSA_SA_PROTOCOL_ESP      = 3
 } secipssa_sa_protocol_enum_type;
 
 typedef enum
 {
   SECIPSSA_SA_ENCAPSULATE_UNDEFINED = SECIPSSP_ENCAPSULATE_UNDEFINED,
   SECIPSSA_SA_ENCAPSULATE_TUNNEL	 = SECIPSSP_ENCAPSULATE_TUNNEL,
   SECIPSSA_SA_ENCAPSULATE_TRANSPORT = SECIPSSP_ENCAPSULATE_TRANSPORT
 } secipssa_sa_encapsulate_enum_type;

  /* ---------- Not needed in the PDF - End ---------- */
 /** @endcond */

 /**
   @addtogroup ipsec_internal_ikev2_ds
 
   @{ */
 
  /**
   * IPsec/IKE SA lifetimes in seconds
   */
  typedef struct
  {
	uint32	  soft_life_sec; /**< -- Rekeying would start after this */
	uint32	  hard_life_sec; /**< -- Rekeying should end before this */
  } secipssa_life_sec_type;
  
  /**
  * Key Information for PSK
  */
 typedef struct
 {
   uint8  key_buffer[SECIPSSP_PSK_KEY_MAX_SIZE]; /**< -- Key */
   uint16 key_len; /**< -- Key Length */
 } secipssp_psk_param_data_type; 
 
 
 /**
  * Key Information for EAP-AKA
  */ 
 typedef struct
 {
   uint8  key_buffer[SECIPSSP_PSK_KEY_MAX_SIZE]; /**< -- Key */
   uint16 key_len; /**< -- Key Length */
 } secipssp_eap_aka_param_data_type;
 
 /**
  * Key Information for EAP-MD5
  */ 
 typedef struct
 {
   uint8  key_buffer[SECIPSSP_MD5_PASSWORD_MAX_SIZE]; /**< -- Key */
   uint16 key_len; /**< -- Key Length */
 } secipssp_eap_md5_param_data_type;

 /**
 Encapsulated Security Payload proposal information.
*/
typedef struct
{
    void*				 sa_index;
    /**< Security Association index into the SADB -- manual keying only. */
	IKETransf_ENCR_EType  sa_encr_algo[IPSEC_PROP_MAX_TRANSF_CNT];
	/**< Encryption algorithm. */
	IKETransf_AUTH_EType  sa_auth_algo[IPSEC_PROP_MAX_TRANSF_CNT];
	/**< Authorization algorithm. */
        IKETransf_DH_EType    dh_group[IPSEC_PROP_MAX_TRANSF_CNT]; 
        /**< -- DH Group */
	IKE_AES_Keysize              aes_key_size[IPSEC_PROP_MAX_TRANSF_CNT]; 
	/**< -- AES key size */
} secipssp_esp_protection_type;

/**
Authentication Header proposal information.
*/
typedef struct
{
    void*				 sa_index;
	/**< Security Association index into the SADB -- manual keying only. */
	IKETransf_AUTH_EType  sa_auth_algo[IPSEC_PROP_MAX_TRANSF_CNT];
	/**< Authentication algorithm. */
} secipssp_ah_protection_type;
	 
/**
Stores a security policy's information.
*/
typedef struct
{
   secipssp_key_mgmt_enum_type			  key_mgmt;
   /**< Key management type. */
   secipssp_encapsulate_enum_type		  encaps_mode;
   /**< Encapsulation mode. */
   secipssp_esp_protection_type 		  esp_protection;
   /**< Encapsulated Security Payload proposal. */
   secipssp_ah_protection_type			  ah_protection;
   /**< Authentication Header proposal. */
   secipssa_life_sec_type				  ipsec_life_sec;
   /**< IPsec SA re-key time value, in seconds. */
   ip_version_enum_type                           sa_type; 
   /**< IKE/IPsec SA type */
 } secipssp_protection_type;

/**
   @}
*/ 
/* end_group ipsec_internal_ikev2_ds */
  
/* ---------- Not needed in the PDF - Start ---------- */
/** @cond */

/* Protection Selector*/
typedef ip_filter_spec_type	secipssp_protection_selector_type;

 typedef struct
 {
   uint16 type;
   uint32 coding;		 /* TV or TLV */
   uint16 length;		 /* Only useful if TLV otherwise always = 2 */
   uint32 value;
 } secipspktdefs_attribute_payload_type;
  
 /* A transform is made of a set of attributes */
 typedef struct
 {
   uint8								transform_number; /* IKEv1 only */
   uint8								transform_type;   /* IKEv2 only */
   uint8								transform_id;
   uint8								attribute_count;
   secipspktdefs_attribute_payload_type attribute[SECIPSPKTDEFS_MAX_ATTRIBUTES];
 } secipspktdefs_transform_payload_type;
  
 typedef struct
 {
   secipssp_auth_type_param_data_type auth_type;
   union
   {
	  secipssp_psk_param_data_type psk;
	  secipssp_eap_aka_param_data_type eap_aka;
	  secipssp_eap_md5_param_data_type eap_md5;
 /*  secipssp_hybrid_param_data_type hybrid;
	 secipssp_auth_type_eap_sim 	 eap_sim;
	 secipssp_auth_type_eap_tls 	 eap_tls;
	 secipssp_auth_type_certificate  cert; */
   }auth_data;
 } secipssp_auth_param_data_type;
 
 /* 
  *Define the SADB SA
  */
 typedef struct
 {
   uint32		  length;
   uint8*		  data_ptr;
 } secipssa_variable_data;
 
 typedef struct
 {
   uint32 type;
   union 
   {
	 uint32 				ipv4_addr;
	 struct  ps_in6_addr	ipv6_addr;
	 secipssa_variable_data key_id;
	 secipssa_variable_data rfc822_addr_id;
   } value;
 } secipssa_sa_identification;
 
 
 /*
  * ID Information - Self ID, Peer ID, Auth Data
  */
 typedef struct
 {
   secipssa_sa_identification	  self_id;
   secipssa_sa_identification	  peer_id;
   secipssa_sa_identification	  peer_cert_id;
   secipssa_sa_identification	  eap_id;
   secipssp_auth_param_data_type  auth_param;
   secipssp_auth_param_data_type  peer_auth_param;
 } secipssp_id_info;
  
 typedef struct
 {
	uint32 Low;
	uint32 High;
 } secipssa_replay_window_type;
  
  typedef struct
  {
	uint32						spi;
	uint32						sequence_number;
	uint32						used_bytes;
	uint32						time_last_used;
	secipssa_replay_window_type replay_window;
  } secipssa_sa_type;
    
 /***
	  * Config attribute values
  ***/
 typedef struct
 {
   struct ps_in_addr addr;
   struct ps_in_addr subnet_mask;
 } secips_cfg_attr_ip4_subnet_type;
 
 /**
  * Config Attribute for INTERNAL_IP6_ADDRESS
  */
 typedef struct
 {
   struct ps_in6_addr addr; /**< -- IPv6 Address */
   uint8  prefix_len; /**< -- IPv6 Address Prefix Length */
 } secips_cfg_attr_ip6_subnet_type;

 typedef struct 
 {
   IPSecConfigAttrEType 			  type;
   uint32							  value_len;
   uint32							  prefix_len;
   uint8                                          v6_pcscf_with_pf; // if TRUE IPV6 Pcscf with Prefix length 17 else 16
   union {
	 uint8* 						  data_ptr;
	 struct ps_in_addr* 			  ip4_address_ptr;
	 struct ps_in6_addr*			  ip6_address_ptr;
	 secips_cfg_attr_ip4_subnet_type* ip4_subnet_ptr;
	 secips_cfg_attr_ip6_subnet_type* ip6_subnet_ptr;
   } value;
 } secips_cfg_attr_type;
  
 /*---------------------------------------------------------------------------
  * 				I S A K M P   P R O P O S A L	T Y P E 
  --------------------------------------------------------------------------*/
 typedef struct
 {
   uint16  protocol;
   //secipssdb_protection_type_enum_type transform[12];
   uint32 transform[12];
   uint16 transform_count;
   uint32 lifetime_seconds;
   uint32 lifetime_bytes;
 
   uint8  encapsulation_mode;	 /* For IPSEC SA only */
   uint32 authentication_method; /* For ISAKMP only */
 } secipstypes_sa_proposal_type;
 
typedef enum
{
  AES_MODE_MIN_VALUE = 0,
  AES_CBC,
  AES_CTR,
  AES_MODE_MAX_VALUE
}secipssa_aes_mode_type;

 /*---------------------------------------------------------------------------
 *                         I P S E C     S A
 --------------------------------------------------------------------------*/
/* IPSEC Security Association type.
*/
typedef struct secipssa_ipsec_sa_type_s secipssa_ipsec_sa_type;

struct secipssa_ipsec_sa_type_s
{
  q_link_type                       link;
  secipssa_sa_type                  in;
  secipssa_sa_type                  out;

  ps_ip_addr_type                   dest_addr;
  ps_ip_addr_type                   local_addr;
  secipssa_sa_state_enum_type       state;
  secipssa_sa_protocol_enum_type    type;
  uint32                            hash_algo;
  uint32                            crypto_algo;
  secipssa_sa_encapsulate_enum_type encapsulation_mode;
  secipstypes_sa_proposal_type      proposal;
  boolean                           rekey_pending;  
  /* Is a rekey pending for this SA */

  void*                             policy_ptr;
  secipssp_esp_protection_type      esp_protection;
  secipssp_ah_protection_type       ah_protection;
  
  secipssa_sa_identification        idii;
  secipssa_sa_identification        idir;
  uint32                            life_max_kilobytes;
  uint32                            life_max_seconds;
  uint32                            life_timeLeft_seconds;
  /* Crypto material */
  uint16                            iv_size;
  uint16                            hash_size;
  uint16                            crypto_key_size;
  uint16                            crypto_key_stream_size;
  uint16                            prf_key_size;
  secapi_handle_type                hash_handle;
  secapi_handle_type                crypto_handle;
  secipssa_ipsec_sa_type            *this_ipsec_sa_data_ptr;
  uint8                             iv[SECAPI_ENC_AES_IV_BLOCK_BYTE_LEN];
  uint8                             iv_ctr[SECAPI_ENC_AES_IV_BLOCK_BYTE_LEN];
  uint8                             hash_key_in[SECIPSTYPES_HASH_MAX_SIZE];
  uint8                             hash_key_out[SECIPSTYPES_HASH_MAX_SIZE];
  uint8                             crypto_key_in[SECIPSTYPES_CRYPTO_ALGO_KEY_MAX_SIZE];
  uint8                             crypto_key_out[SECIPSTYPES_CRYPTO_ALGO_KEY_MAX_SIZE];
  uint32                            blockcounter;

  /* NAT-T-specific data. */
  boolean                           is_udp_encaps;
  ps_ip_addr_type                   natoa_i;
  ps_ip_addr_type                   natoa_r;

  /* Private internal data */
  void*                             ike_sa_ptr;
  void*                             ipsec_entry_ptr;
  void*                             exchange_ptr;
  void*                             collision_exchange_ptr;
  void*                             flow_client_data_ptr; /* Stores the flow
                                                               callback pointer. */

  ps_timer_handle_type              rekey_timer;
    /* IPSEC Security Association re-ekey timer. */
  ps_timer_handle_type              delete_timer;
    /* IPSEC Security Association delete timer. */

  uint8                             transform_count;
  secipspktdefs_transform_payload_type* chosen_transforms[SECIPSPKTDEFS_MAX_TRANSFORMS];
  secipssa_aes_mode_type aes_mode;
  void*                             SPIndex;
};

#define DS_IPSEC_SA_PTR_IS_VALID(ipsec_sa_data_ptr)      \
        ((ipsec_sa_data_ptr != NULL) &&                        \
        (ipsec_sa_data_ptr->this_ipsec_sa_data_ptr == ipsec_sa_data_ptr))
 
 /* ---------- Not needed in the PDF - End ---------- */
 /** @endcond */

#endif  /*  SECIPSSP_H */
