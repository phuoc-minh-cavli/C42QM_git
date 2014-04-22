#ifndef SECIPSSPI_H
#define SECIPSSPI_H

/*===========================================================================
*
*               A S W   S E C U R I T Y   S E R V I C E S
*
*                        I P S E C   L A Y E R
*
* FILE: secipsspi.h
*
* SERVICES: 
*
* DESCRIPTION: internal data types and functions related to security policy
*              entries. These functions should only be called by SECIPS_TASK.
*                      
*  Copyright (c) 2018 - 2019 Qualcomm Technologies, Inc.  All Rights Reserved.
*  Qualcomm Technologies Proprietary and Confidential.
*==========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: $
  $Header: //components/rel/dataiot.mpss/2.0/protocols/sec/ipsec/src/secipsspi_ex.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/11/19    ds     Code Cleanup related changes.
08/13/18    ds     Code Re-architecture for Manual IPSEC.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_SEC_IPSEC
#include "secipssp.h"
#include "IPSec.h"
#include "secips.h"
#include "ds_ipfltr.h"

#define SECIPS_MAX_SELF_ID  2
#define SECIPS_ESP_MAX_HDR_SIZE   64
#define SECIPSSADB_MAX_IPSEC_SA_COUNT     48
#define MAX_REX_TIMER_VALUE 129600
#define SECIPSSA_REPLAY_WINDOW_SIZE 1024


#define secipstypes_htonl(x) \
			((uint32)((((uint32)(x) & 0x000000ffU) << 24) | \
			(((uint32)(x) & 0x0000ff00U) <<  8) | \
			(((uint32)(x) & 0x00ff0000U) >>  8) | \
			(((uint32)(x) & 0xff000000U) >> 24)))
#define secipstypes_ntohl(x) \
			((uint32)((((uint32)(x) & 0x000000ffU) << 24) | \
			(((uint32)(x) & 0x0000ff00U) <<  8) | \
			(((uint32)(x) & 0x00ff0000U) >>  8) | \
			(((uint32)(x) & 0xff000000U) >> 24)))
#define secipstypes_htons(x) \
			((uint16)((((uint16)(x) & 0x00ff) << 8) | \
			(((uint16)(x) & 0xff00) >> 8)))
#define secipstypes_ntohs(x) \
			((uint16)((((uint16)(x) & 0x00ff) << 8) | \
			(((uint16)(x) & 0xff00) >> 8)))

/* Security suite type.*/
typedef struct
{
  uint32                   ipsec_handle;  /* --Pointer to the SPD information. */
  secipssa_ipsec_sa_type*  esp_sa_ptr;
  secipssa_ipsec_sa_type*  ah_sa_ptr;
} secips_sec_suite_type;

#define SECIPS_MAX_IPSEC_SA_CNT         16

extern secipssa_ipsec_sa_type ipsec_sa_list[];

extern q_type *IPSecSA_Q_List[];


/* ISAKMP Security Association direction. */
typedef enum
{
  SECIPSSA_SA_DIRECTION_UNDEFINED,
  SECIPSSA_SA_DIRECTION_INCOMING,
  SECIPSSA_SA_DIRECTION_OUTGOING
} secipssa_sa_direction_enum_type;

/* =================================================================
	 I P S E C	  p r o t e c t i o n	 s u i t e s
  ================================================================= */
  typedef enum
  {
	SECIPSSP_ESP_PROTECTION_TYPE_RESERVED		 =	0,
	SECIPSSP_ESP_PROTECTION_TYPE_DH1024_AES_SHA  =	1,
	SECIPSSP_ESP_PROTECTION_TYPE_DH1024_AES_MD5  =	2,
	SECIPSSP_ESP_PROTECTION_TYPE_DH1024_3DES_SHA =	3,
	SECIPSSP_ESP_PROTECTION_TYPE_DH1024_3DES_MD5 =	4,
		
	SECIPSSP_ESP_PROTECTION_TYPE_DH1024_DES_SHA  =	5,
	SECIPSSP_ESP_PROTECTION_TYPE_DH1024_DES_MD5  =	6,
	SECIPSSP_ESP_PROTECTION_TYPE_AES_SHA		 =	7,
	SECIPSSP_ESP_PROTECTION_TYPE_AES_MD5		 =	8,
	SECIPSSP_ESP_PROTECTION_TYPE_3DES_SHA		 =	9,
		
	SECIPSSP_ESP_PROTECTION_TYPE_3DES_MD5		 =	10,
	SECIPSSP_ESP_PROTECTION_TYPE_DES_SHA		 =	11,
	SECIPSSP_ESP_PROTECTION_TYPE_DES_MD5		 =	12,
	SECIPSSP_ESP_PROTECTION_TYPE_AES_NULL		 =	13,
	SECIPSSP_ESP_PROTECTION_TYPE_3DES_NULL		 =	14,
	SECIPSSP_ESP_PROTECTION_TYPE_DES_NULL		 =	15,
	SECIPSSP_ESP_PROTECTION_TYPE_NULL_SHA		 =	16,
	SECIPSSP_ESP_PROTECTION_TYPE_NULL_MD5		 =	17,
	SECIPSSP_ESP_PROTECTION_TYPE_NO_ESP 		 =	18,

        // DH group 5
        SECIPSSP_ESP_PROTECTION_TYPE_DH1356_AES_SHA  =  19,
        SECIPSSP_ESP_PROTECTION_TYPE_DH1356_AES_MD5  =  20,
        SECIPSSP_ESP_PROTECTION_TYPE_DH1356_3DES_SHA =  21,
        SECIPSSP_ESP_PROTECTION_TYPE_DH1356_3DES_MD5 =  22,
        SECIPSSP_ESP_PROTECTION_TYPE_DH1356_DES_SHA  =  23,
        SECIPSSP_ESP_PROTECTION_TYPE_DH1356_DES_MD5  =  24,

        // DH group 14
        SECIPSSP_ESP_PROTECTION_TYPE_DH2048_AES_SHA  =  25,
        SECIPSSP_ESP_PROTECTION_TYPE_DH2048_AES_MD5  =  26,
        SECIPSSP_ESP_PROTECTION_TYPE_DH2048_3DES_SHA =  27,
        SECIPSSP_ESP_PROTECTION_TYPE_DH2048_3DES_MD5 =  28,
        SECIPSSP_ESP_PROTECTION_TYPE_DH2048_DES_SHA  =  29,
        SECIPSSP_ESP_PROTECTION_TYPE_DH2048_DES_MD5  =  30,
  
        // SAH2 
        SECIPSSP_ESP_PROTECTION_TYPE_DH1024_AES_SHA256  =  31,
        SECIPSSP_ESP_PROTECTION_TYPE_DH1024_3DES_SHA256 =  32,
        SECIPSSP_ESP_PROTECTION_TYPE_DH1024_DES_SHA256  =  33,
        SECIPSSP_ESP_PROTECTION_TYPE_AES_SHA256     =  34,
        SECIPSSP_ESP_PROTECTION_TYPE_3DES_SHA256    =  35,
        SECIPSSP_ESP_PROTECTION_TYPE_DES_SHA256     =  36,
        SECIPSSP_ESP_PROTECTION_TYPE_NULL_SHA256    =  37,
        SECIPSSP_ESP_PROTECTION_TYPE_DH1356_AES_SHA256  =  38,
        SECIPSSP_ESP_PROTECTION_TYPE_DH1356_3DES_SHA256 =  39,
        SECIPSSP_ESP_PROTECTION_TYPE_DH1356_DES_SHA256  =  40,
        SECIPSSP_ESP_PROTECTION_TYPE_DH2048_AES_SHA256  =  41,
        SECIPSSP_ESP_PROTECTION_TYPE_DH2048_3DES_SHA256 =  42,
        SECIPSSP_ESP_PROTECTION_TYPE_DH2048_DES_SHA256  =  43,

	SECIPSSP_ESP_PROTECTION_TYPE_COUNT
  } secipssp_esp_protection_type_enum_type;

  /*
   * Options that can be Set/Retrieved by Brew Interface
   */
  typedef enum
  {
	SECIPSSP_PROTECTION_POLICY					= 0, 
	SECIPSSP_PROTECTION_SELECTOR				= 1, 
	SECIPSSP_PROTECTION_SELECTOR_SRC_ADDR		= 2, 
	SECIPSSP_PROTECTION_SELECTOR_SRC_NETMASK	= 3, 
	SECIPSSP_PROTECTION_SELECTOR_DST_ADDR		= 4, 
	SECIPSSP_PROTECTION_SELECTOR_DST_NETMASK	= 5, 
	SECIPSSP_PROTECTION_SELECTOR_PROTOCOL		= 6, 
	SECIPSSP_PROTECTION_SELECTOR_SRC_PORT		= 7, 
	SECIPSSP_PROTECTION_SELECTOR_DST_PORT		= 8, 
	SECIPSSP_PROTECTION_SUITE					= 9, 
	SECIPSSP_ENDPOINT							= 10,
	SECIPSSP_GW_PARAM							= 11,
	SECIPSSP_ESP_PROTECTION_TYPE				= 12,
	SECIPSSP_AH_PROTECTION_TYPE 				= 13,
	SECIPSSP_ISAKMP_MODE						= 14,
	SECIPSSP_GROUP_NAME 						= 15,
	SECIPSSP_AUTH_PARAM_DATA					= 16,
	SECIPSSP_OPTION_COUNT
  } secipssp_option_type;

    typedef enum
  {
	SECIPSSP_AH_PROTECTION_TYPE_RESERVED   =  0,
	SECIPSSP_AH_PROTECTION_TYPE_DH1024_SHA =  1,
	SECIPSSP_AH_PROTECTION_TYPE_DH1024_MD5 =  2,
  
	SECIPSSP_AH_PROTECTION_TYPE_SHA 	   =  3,
	SECIPSSP_AH_PROTECTION_TYPE_MD5 	   =  4,
	SECIPSSP_AH_PROTECTION_TYPE_NO_AH	   =  5,
	SECIPSSP_AH_PROTECTION_TYPE_COUNT
  } secipssp_ah_protection_type_enum_type;
 
 /*===========================================================================

                     DEFINITIONS AND TYPE DECLARATIONS

===========================================================================*/

typedef IKETransf_ENCR_EType secipssp_encr_algo_enum_type;
typedef IKETransf_AUTH_EType secipssp_auth_algo_enum_type;
typedef IKETransf_DH_EType   secipssp_dh_algo_enum_type;
typedef IKETransf_PRF_EType  secipssp_prf_algo_enum_type;

/*
 * Protection Policy
 *   selector defines what is protected
 *   suite    defines how it is protected
 *   ipfltr_handle stores the tunnel list.
 */
typedef struct
{
  boolean                           policy_valid; /* XXX allocated? */

  ps_ip_addr_type                   endpoint;
  uint8*                            segw_fqdn;

  secipssp_direction_enum_type      direction; /* XXX part of selector? */
  secipssp_protection_selector_type selector;

  secipssp_protection_type          protection;

  uint32                            cfg_attr_cnt;
  secips_cfg_attr_type*             cfg_attr[IPSEC_CFG_MAX_ATTRS];

    /* List of all the ID(s) */
  secipssp_id_info*                 id_list[SECIPS_MAX_SELF_ID];
  /* Indexing into the id_list */
  uint8                             current_id_index;

  void*                       sp_cb_ptr; /* IxCallback function to be invoked in case of SP related events */
 
  /* Internal use */
  ds_ipfltr_handle_type       input_fi_handle;
  ds_ipfltr_handle_type       output_fi_handle;
 
} secipssp_protection_policy_type;

/*
 * Protection Policy with ike_handle_list in place of iface_list
 *   selector defines what is protected
 *   suite    defines how it is protected
 *   ipfltr_handle stores the tunnel list.
 */
typedef struct
{
  boolean                           policy_valid; /* XXX allocated? */

  ps_ip_addr_type                   endpoint;
  uint8*                            segw_fqdn;

  secipssp_direction_enum_type      direction; /* XXX part of selector? */
  secipssp_protection_selector_type selector;

  secipssp_protection_type          protection;

  uint32                            cfg_attr_cnt;
  secips_cfg_attr_type*             cfg_attr[IPSEC_CFG_MAX_ATTRS];

    /* List of all the ID(s) */
  secipssp_id_info*                 id_list[SECIPS_MAX_SELF_ID];
  /* Indexing into the id_list */
  uint8                             current_id_index;

  void*                       sp_cb_ptr; /* IxCallback function to be invoked in case of SP related events */
  
  /* transport selector responder */
  secipssp_protection_selector_type tsr;


} secipssp_protection_ipsec_policy_type;


/**  Encapsulated Security Payload header.
 */ 
typedef struct
{
  uint32 spi;       /**< Security parameter index. */
  uint32 seqno;     /**< Sequence number. */
} secips_esp_hdr;

/* Encapsulated Security Payload header trailer.
 */
typedef struct
{
  uint8 pad_length;     /* Length of the padding. */
  uint8 next_header;    /* Protocol of the next header. */
} secips_esp_trailer;


/*---------------------------------------------------------------------------
               Inline Function definations
----------------------------------------------------------------------------*/
/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================
FUNCTION      secipsspi_spd_new 

DESCRIPTION   
              
  
DEPENDENCIES  None
    
PARAMETERS    spd_ptr - 
                  
RETURN VALUE  secerrno_enum_type - E_SUCCESS or failure code

SIDE EFFECTS  None
===========================================================================*/
secerrno_enum_type secipsspi_spd_new
(
  secipssp_protection_policy_type** spd_ptr
);

/*===========================================================================
FUNCTION      secipsspi_spd_free 

DESCRIPTION   
              
  
DEPENDENCIES  None
    
PARAMETERS    spd_ptr - 
                  
RETURN VALUE  secerrno_enum_type - E_SUCCESS or failure code

SIDE EFFECTS  None
===========================================================================*/
secerrno_enum_type secipsspi_spd_free
(
  secipssp_protection_policy_type* spd_ptr
);

/*===========================================================================
FUNCTION      secipsspi_spd_set_protection 

DESCRIPTION   
              
  
DEPENDENCIES  None
    
PARAMETERS    spd_ptr - 
                  
RETURN VALUE  secerrno_enum_type - E_SUCCESS or failure code

SIDE EFFECTS  None
===========================================================================*/
secerrno_enum_type secipsspi_spd_set_protection
(     
  secipssp_protection_policy_type* spd_ptr,
  const secipssp_protection_type*  prot_ptr
);    

/*===========================================================================
FUNCTION      secipsspi_spd_add_cfg_attr 

DESCRIPTION   
              
  
DEPENDENCIES  None
    
PARAMETERS    spd_ptr - 
                  
RETURN VALUE  secerrno_enum_type - E_SUCCESS or failure code

SIDE EFFECTS  None
===========================================================================*/
secerrno_enum_type secipsspi_spd_add_cfg_attr
(
  secipssp_protection_policy_type* spd_ptr,
  IPSecConfigAttrEType             attr_type,
  const void*                      attr_value_ptr,
  uint32                           attr_value_len
);


/*===========================================================================
FUNCTION       SECIPSSP_ESP_PROTECTION_ENABLED
===========================================================================*/
/*
  Returns TRUE if esp_protection_ptr is configured with a valid ESP
  policy.\ This is a utility function for backwards compatibility.
  
  @param *esp_protection_ptr
 
  @return
  A boolean value indicating if esp_protection_ptr is configured with a valid
  ESP policy.
   
  @dependencies
  None.
*/
boolean secipssp_esp_protection_enabled
(
  const secipssp_esp_protection_type *esp_protection_ptr
);

/*===========================================================================
FUNCTION       SECIPSSP_AH_PROTECTION_ENABLED
===========================================================================*/
/*
  Returns true if ah_protection_ptr is configured with a valid AH
  policy.\ This is a utility function for backwards compatibility.
  
  @param *ah_protection_ptr 
 
  @return
  A boolean value indicating if ah_protection_ptr is configured with a valid
  AH policy.
   
  @dependencies
  None.
*/
boolean secipssp_ah_protection_enabled
(
  const secipssp_ah_protection_type *ah_protection_ptr
);

/*===========================================================================

                      FUNCTION DECLARATIONS for Removal of Iface 

===========================================================================*/

void
secipsutil_destroy_id(secipssa_sa_identification *id_ptr);

void secipsutil_cfg_attr_destroy
(
  secips_cfg_attr_type * attr_ptr
);

secerrno_enum_type secipsutil_cfg_attr_create
(
  secips_cfg_attr_type**        out_attr_ptr,
  IPSecConfigAttrEType          attr_type,
  const void*                   attr_value_ptr,
  uint32                        attr_value_len
);

boolean secipsutil_cfg_attr_validate
(
  secips_cfg_attr_type*        attr_ptr
);


void secipsutil_hexdump(uint8 *buf, uint32 length);

void secipsutil_dsm_hexdump(dsm_item_type **dsm_item);



secerrno_enum_type secips_dsm_split
(
 dsm_item_type**        msg_ptr,
 dsm_item_type**        extracted_item,
 uint16                 bytes_to_get
);

secipssa_ipsec_sa_type* secipssadb_find_ipsec_sa
(
  secipssa_sa_protocol_enum_type  protocol,
  uint32                          spi,
  ps_ip_addr_type                *dest_addr
);

/*===========================================================================
FUNCTION      SECIPSSA_IPSEC_SA_UPDATE_USED_TIME

DESCRIPTION   Update last_used_time field of an IPSec SA

DEPENDENCIES  None

PARAMETERS    sa_ptr: the IPSec SA

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
secerrno_enum_type secipssa_ipsec_sa_update_used_time
(
  secipssa_sa_type* sa_ptr
);

/*===========================================================================
FUNCTION      SECIPSSA_DELETE_MANUAL_SA_CB

DESCRIPTION   This function is called when the delete timer expires for an
              IPsec Manual SA

DEPENDENCIES  None

PARAMETERS    sa_ptr: the Manual SA

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void secipssa_delete_manual_sa_cb
(
  void* sa_ptr
);

boolean secipssp_esp_protection_enabled
(
  const secipssp_esp_protection_type *esp_protection_ptr
);

boolean secipssp_ah_protection_enabled
(
  const secipssp_ah_protection_type *ah_protection_ptr
);

/*===========================================================================
 * FUNCTION:       secips_generate_ipsec_info
 *  
 * DESCRIPTION:    This method creates a new SA if the key management
 *                 is set to IKE. If set to manual keying it does nothing.
 * 
 * DEPENDENCIES:   This method is called by ps code when a filter match 
 *                 is found
 *   
 * PARAMETERS:     ipsec_handle - ptr to spd entry 
 *                 pkt_info_ptr - ptr to packet info that matched ip fltr
 *                 ipsec_info   - ptr to ipsec info used by ps code
 *                 user_id      - 
 * 
 * RETURN VALUE:   E_SUCCESS in case of success, failure otherwise
 *   
 * SIDE EFFECTS:   None
===========================================================================*/
secerrno_enum_type secips_generate_ipsec_info
(
  void                    *ipsec_handle,
  ip_pkt_info_type        *pkt_info_ptr,
  ps_ipsec_info_type      *ipsec_info,
  uint32                   user_id
);


#endif /* FEATURE_SEC_IPSEC */
#endif /* SECIPSSSPI_H */
