#ifndef DS_PDN_PCO_H
#define DS_PDN_PCO_H

/*===========================================================================
                      DS_PDN_PCO.H
DESCRIPTION
This file exposes PDN PCO through APIs and data structures.

Copyright (c) 2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/3gpp/pdnmgr/inc/ds_pdn_pco.h#1 $
  $DateTime: 2020/03/17 08:03:16 $$Author: pwbldsvc $

when           who    what, where, why
--------    ---    ----------------------------------------------------------
08/09/17    sd      First checkin
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "ds_pdn_defs.h"
#include "ds_pdn_manager.h"

/*===========================================================================
                      PUBLIC DATA DECLARATIONS
===========================================================================*/

/*---------------------------------------------------------------------------
  Size of Challenge generated for CHAP : 16 bytes minimum for MD5
---------------------------------------------------------------------------*/
#define CHAP_CHALLENGE_LEN 16
       
/*-------------------------------------------------------------------------
  Constant definitions for PAP/CHAP pkt encoding. 
-------------------------------------------------------------------------*/
#define PPP_PROTOCOL_FIELD_LEN    2   /* length of PPP protocol field     */
#define MD5_DIGEST_SIZE           16  /* length of MD5 digest output      */
#define CODE_CHAP_CHALLENGE       1   /* Code for CHAP Challenge          */
#define CODE_CHAP_RESPONSE        2   /* code for CHAP response           */
#define MCC_MNC_LENGTH            3   /*Length of Mcc + Mnc*/

#ifdef FEATURE_DATA_IMS
/*-------------------------------------------------------------------------
  Constant definition for SIP PCSCF Address Protocol. 
  Ref 24.008 10.5.6.3
-------------------------------------------------------------------------*/
#define SIP_PCSCF_V6_ADDRESS_PROTOCOL         0X0001
#define SIP_PCSCF_V4_ADDRESS_PROTOCOL         0X000C
#define SIP_PCSCF_RESELECTION_SUPPORT_PROTOCOL  0x0012
/*-------------------------------------------------------------------------
  Constant definition for IM CN SUBSYSTEM Protocol. 
-------------------------------------------------------------------------*/
#define IM_CN_SUBSYSTEM_PROTOCOL         0X0002

/*-------------------------------------------------------------------------
  Constant definition for maximum SIP PCSCF addresses supported. This is
  limited by the length of Protocol Configuration Option.

  Total length             = 253 octets
  Reserved                 = 3 Octets
  Total Available for data = 250 octets 
  Number of octets required for an IP Address = 19 (16 octets for IP
        V6 Address + 3 octets for container ID)
        
-------------------------------------------------------------------------*/
#define MAX_PCSCF_ADDRESSES                13

/*-------------------------------------------------------------------------
  Constant definition for DHCP Unique Client ID. This ID needs to be
  passed during DHCP Client start.

  This is just a dummy number without any significance. The enterprise
  number and identifier are completely random. This has been defined to 
  provide an interim id untill we get some requirements from client or make
  this configurable. this is used by the DHCP server to identify the
  client uniquely.

  Value = 02 + Enterprise Number + Identifier
          02 + 0006 + 0C
        
-------------------------------------------------------------------------*/
#define DHCP_CLIENT_DUID_ADDR              0x0200060C

/* DUID Length */
#define DHCP_CLIENT_DUID_ADDR_LEN          16

#endif

#define DS_IPV4_ADDR_LEN 4
#define DS_IPV6_ADDR_LEN 16
#ifdef FEATURE_DATA_PS_IPV6
/*-------------------------------------------------------------------------
  IPV6 address length and Max number of IPV6 DNS addresses processed by UE
-------------------------------------------------------------------------*/
/*#define DSUMTS_IPV6_ADDR_LEN 16 [asen] was here, now outside feature def*/
#define DS_MAX_IPV6_DNS_ADDR 2
#endif

#define DS_OPERATOR_RESERVED_PCO_ID_RANGE_BEGIN 0xFF00
#define DS_OPERATOR_RESERVED_PCO_ID_RANGE_END 0xFFFF

#define LOWER_BYTE_SET 0x0F
#define UPPER_BYTE_SET 0xF0


/*---------------------------------------------------------------------------
  IPV6CP option parameters associated with a V6 data call.  
---------------------------------------------------------------------------*/

typedef struct
{
  uint8                identifier;    /* Identifies the ipcp packet. Aids  */
                                      /* in matching requests and replies  */ 
  struct ps_in6_addr   primary_dns;   /* Primary DNS server Address        */
  struct ps_in6_addr   secondary_dns; /* Secondary DNS server Address      */
  struct ps_in6_addr   gateway_addr;  /* Peer IP address                   */
}ds_ipv6cp_options_s;

/*---------------------------------------------------------------------------
  IPCP option parameters associated with a V4 data call.  
---------------------------------------------------------------------------*/

typedef struct 
{
  uint8                identifier;    /* Identifies the ipcp packet. Aids  */
                                      /* in matching requests and replies  */ 
  uint32               primary_dns;   /* Primary DNS server Address        */
  uint32               secondary_dns; /* Secondary DNS server Address      */
  uint32               gateway_addr;  /* Peer IP address                   */
}ds_ipcp4_options_s;


/*---------------------------------------------------------------------------
  IPCP option parameters associated with a V4/V6 data call.  
---------------------------------------------------------------------------*/
typedef struct 
{
 ds_ipcp4_options_s  ipcp4_options;
 ds_ipv6cp_options_s ipv6cp_options;
}ds_ipcp_options_s;


/*===========================================================================
                      PUBLIC FUNCTION DECLARATIONS
===========================================================================*/



/*===========================================================================
FUNCTION DSUMTSPS_PPP_GEN_PAP_AUTH_REQ_PKT

DESCRIPTION
  This function generates a PAP authentication request packet.
  
PARAMETERS  
    pkt_buff_ptr  - Pointer to the buffer to store the packet
    pkt_buff_len  - length of buffer to store the packet
    identifier    - ID of the PAP authentication pkt 
    user_id_ptr   - pointer to buffer which has the user_id
    user_id_len   - length of the user_id
    passwd_ptr    - pointer to the buffer which has the password
    passwd_len    - length of the password
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  > 0 - packet generated succesfully & indicates the length of the packet
  -1  - pkt_buffer too short for packet       
  
SIDE EFFECTS 
  None.
  
===========================================================================*/
int ds_pdn_ppp_gen_pap_auth_req_pkt
(
  byte  *pkt_buff_ptr, 
  int   pkt_buff_len,
  byte  identifier,
  const byte  *user_id_ptr,
  byte  user_id_len,
  const byte *passwd_ptr,
  byte  passwd_len
);


/*===========================================================================
FUNCTION DSUMTSPS_PPP_GEN_CHAP_CHALLENGE_PKT

DESCRIPTION
  This function generates a CHAP Challange packet.
  
PARAMETERS   
    pkt_buff_ptr  - Pointer to the buffer to store the packet
    pkt_buff_len  - length of buffer to store the packet
    identifier    - ID of the CHAP challenge pkt 
    challenge_ptr - pointer to buffer which has CHAP challenge
    challenge_len - length of challenge
    sys_name_ptr  - pointer to buffer which has system name 
    sys_name_len  - length of system name
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  0   - packet generated succesfully 
  -1  - pkt_buffer too short for packet       
  
SIDE EFFECTS 
  None.
  
===========================================================================*/
int ds_pdn_ppp_gen_chap_challenge_pkt
(
  byte *pkt_buff_ptr, 
  int   pkt_buff_len,
  byte  identifier,
  const byte *challenge_ptr,
  byte  challenge_len,
  const byte *sys_name_ptr,
  byte  sys_name_len
);


/*===========================================================================
FUNCTION DSUMTSPS_PPP_GEN_CHAP_RESPONSE_PKT

DESCRIPTION
  This function generates a CHAP response  packet.
  
PARAMETERS   
    pkt_buff_ptr  - Pointer to the buffer to store the packet
    pkt_buff_len  - length of buffer to store the packet
    identifier    - ID of the CHAP challenge pkt 
    challenge_ptr - pointer to buffer which has CHAP challenge
    challenge_len - length of challenge
    secret_ptr    - pointer to buffer which has the secret / password
    secret_len    - length of password
    sys_name_ptr  - pointer to buffer which has system name 
    sys_name_len  - length of system name
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  0   - packet generated succesfully 
  -1  - pkt_buffer too short for packet       
  
SIDE EFFECTS 
  None.
  
===========================================================================*/
int ds_pdn_ppp_gen_chap_response_pkt
(
  byte  *pkt_buff_ptr, 
  int   pkt_buff_len,
  byte  identifier,
  const byte  *challenge_ptr,
  byte  challenge_len,
  const byte  *secret_ptr,
  byte  secret_len,
  const byte  *sys_name_ptr,
  byte  sys_name_len
);


/*===========================================================================
FUNCTION DSUMTSPS_PPP_FILL_CHAP_RESPONSE_PKT

DESCRIPTION
  This function constructs a CHAP response  packet from the parameters passed
  in. The chap response is passed in and this function just fills in 
  all the parameters to construct the packet
  
PARAMETERS   
    pkt_buff_ptr  - Pointer to the buffer to store the packet
    pkt_buff_len  - length of buffer to store the packet
    identifier    - ID of the CHAP challenge pkt 
    hash_ptr      - pointer to buffer which has CHAP response
    hash_len      - length of challenge
    responder_name_ptr  - pointer to buffer which has responder's name 
    responder_name_len  - length of responder's name
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  0   - packet generated succesfully 
  -1  - pkt_buffer too short for packet       
  
SIDE EFFECTS 
  None.
  
===========================================================================*/
int ds_pdn_ppp_fill_chap_response_pkt
(
  byte  *pkt_buff_ptr, 
  int   pkt_buff_len,
  byte  identifier,
  const byte  *hash_ptr,
  byte  hash_len,
  const byte  *responder_name_ptr,
  byte  responder_name_len
);


/*===========================================================================
FUNCTION DSUMTSPS_GEN_IPCP_REQ_PKT

DESCRIPTION
  This function generates a IPCP request packet. The IPCP options that
  are requested are Primary and Secondary DNS addresses.
  
PARAMETERS   
  protocol_pkt_ptr  - Pointer to the buffer to store the IPCP packet
  protocol_pkt_len  - length of buffer to store the IPCP packet
  identifier - ID of the IPCP packet
  primary_dns   - primary_dns address
  secondary_dns - secondary_dns address
    
DEPENDENCIES 
  None.

RETURN VALUE  
  > 0 - packet generated succesfully & indicates the length of the packet
  -1  - pkt_buffer too short for packet       
  
SIDE EFFECTS 
  None.

===========================================================================*/
int ds_pdn_gen_ipcp_req_pkt
(
  byte  *protocol_pkt_ptr,/* Pointer to the buffer to store the IPCP packet*/
  int   protocol_pkt_len,          /* Length of buffer to store the packet */
  byte  identifier,                               /* ID of the IPCP packet */
  uint32 primary_dns,                             /* primary DNS address   */
  uint32 secondary_dns,                           /* secondary DNS address */
  net_cfg_params_mask_e_type valid_fields         /* IPCP fileds to request */
);


/*===========================================================================
FUNCTION DS3GPP_DECODE_IPCP_PKT

DESCRIPTION
  This function decodes the IPCP packet received from the network. The only
  Codes that are processed are:
  CONFIG_NAK, CONFIG_REJ and CONFIG_ACK. All other options are discarded.
  The IPCP options that are decoded are:
    - Primary DNS Address
    - Secondary DNS address
  All other options are ignored.
  The identifer that was received must match the identifier that was sent in
  the PDP context activation request.
  
PARAMETERS   
    protocol_pkt_ptr  - Pointer to the buffer to extract ipcp options from
    protocol_pkt_len  - Length of packet to extract ipcp options from
    ipcp_options      - IPCP options received
     
    
DEPENDENCIES 
  None.

RETURN VALUE
  DS3G_SUCCESS : if successful
  DS3G_FAILURE : on error
  
SIDE EFFECTS
  ipcp_options->primary_dns and ipcp_options->secondary_dns might be
  modified by this function, if valid values are received.

===========================================================================*/
boolean ds_pdn_decode_ipcp_pkt
(
  const byte *protocol_pkt_ptr,         /* Pointer to the buffer to        */
                                        /* extract options from            */
  int   protocol_pkt_len,               /* Length of packet to extract     */
                                        /* ipcp options from               */
  ds_ipcp4_options_s *ipcp_options /* IPCP options received           */  

);

#ifdef FEATURE_DATA_IMS
/*===========================================================================
FUNCTION DS3GPP_DECODE_PCSCF_PKT

DESCRIPTION
  This function decodes the PCSCF packet received from the network. The
  pcscf addresses, if any sent by the network, are stored in ps_iface
  structure. Network sends only 1 IP address in 1 packet. 

PARAMETERS   
    protocol_pkt_ptr  - Pointer to the buffer to extract pcscf address from
    protocol_pkt_len  - Length of packet to extract pcscf address from
    pcscf_addr_array   - Array to store the pcscf addresses
    num_pcscf_addresses- Stores the total pcscf addresses processed
                        till now
     
    
DEPENDENCIES 
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_decode_v6_pcscf_pkt
(
  const byte  *protocol_pkt_ptr, /* Pointer to the buffer to extract pcscf */
                                 /* addresses from                         */
  int   protocol_pkt_len,  /* Length of packet to extract pcscf addresse frm*/
  ps_ip_addr_type *pcscf_v4_addr_array,           /* v4 pcscf addresses received */
  uint32 *num_v4_pcscf_addresses, /* number of v4 pcscf addresses processed till now */
  ps_ip_addr_type *pcscf_v6_addr_array,           /* v6 pcscf addresses received */
  uint32 *num_v6_pcscf_addresses /* number of v6 pcscf addresses processed till now */
);

/*===========================================================================
FUNCTION DS3GPP_DECODE_V4_PCSCF_PKT

DESCRIPTION
  This function decodes the PCSCF packet received from the network. The
  pcscf addresses, if any sent by the network, are stored in ps_iface
  structure. Network sends only 1 IP address in 1 packet. 

PARAMETERS   
    protocol_pkt_ptr  - Pointer to the buffer to extract pcscf address from
    protocol_pkt_len  - Length of packet to extract pcscf address from
    pcscf_addr_array   - Array to store the pcscf addresses
    num_pcscf_addresses- Stores the total pcscf addresses processed
                        till now
     
    
DEPENDENCIES 
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_decode_v4_pcscf_pkt
(
  const byte  *protocol_pkt_ptr, /* Pointer to the buffer to extract pcscf */
                                 /* addresses from                         */
  int   protocol_pkt_len,  /* Length of packet to extract pcscf addresse frm*/
  ps_ip_addr_type *pcscf_v4_addr_array,           /* v4 pcscf addresses received */
  uint32 *num_v4_pcscf_addresses /* number of v4 pcscf addresses processed till now */
);

#endif /* FEATURE_DATA_IMS */

/*===========================================================================
FUNCTION ds3gpp_decode_ipv4_pkt

DESCRIPTION
  This function decodes the V4 dns container received from the network. 
  Only one dns address is decoded. If more than one dns address is sent
  by the network, they are ignored.
  
PARAMETERS   
    protocol_pkt_ptr  - Pointer to the buffer to extract V4 dns addresses 
                        from
    protocol_pkt_len  - Length of packet to extract V4 dns addresses from 
    ipv4cp_options      - IPCP options received
    num_ipv4_dns      - Number of V4 dns addresses already processed
     
    
DEPENDENCIES 
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  ipcp_options4->primary_dns and ipcp_options4->secondary_dns might be
  modified by this function, if valid values are received.

===========================================================================*/
void ds_pdn_decode_ipv4_pkt
(
  const byte *protocol_pkt_ptr, /* Pointer to the buffer to extract dns    */
                                /* addresses from                          */
  int   protocol_pkt_len,  /* Length of packet to extract dsn addresses from*/
  ds_ipcp4_options_s *ipv4cp_options,  /* ipv4cp options received */
  uint32 *num_ipv4_dns       /*Number of V4 dns addresses already processed   */
);

/*===========================================================================
FUNCTION ds3gpp_decode_ipv6_pkt

DESCRIPTION
  This function decodes the V6 dns container received from the network. 
  Only two dns addresses are decoded. If more than two dns address is sent
  by the network, they are ignored.
  
PARAMETERS   
    protocol_pkt_ptr  - Pointer to the buffer to extract V6 dns addresses 
                        from
    protocol_pkt_len  - Length of packet to extract V6 dns addresses from 
    ipv6cp_options      - IPCP options received
    num_ipv6_dns      - Number of V6 dns addresses already processed
     
    
DEPENDENCIES 
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  ipcp_options6->primary_dns and ipcp_options6->secondary_dns might be
  modified by this function, if valid values are received.

===========================================================================*/
void ds_pdn_decode_ipv6_pkt
(
  const byte *protocol_pkt_ptr, /* Pointer to the buffer to extract dns    */
                                /* addresses from                          */
  int   protocol_pkt_len,  /* Length of packet to extract dsn addresses from*/
  ds_ipv6cp_options_s *ipv6cp_options,  /* ipv6cp options received */
  int *num_ipv6_dns       /*Number of V6 dns addresses already processed   */
);


/*===========================================================================
FUNCTION DS3GPP_DECODE_OPERATOR_RESERVED_PCO_PKT

DESCRIPTION
  This function decodes the operator reserved PCO received from the network.
  
PARAMETERS   
    protocol_pkt_ptr  - Pointer to the buffer to extract operator reserved pco container from
    protocol_pkt_len  - Length of packet to extract operator reserved pco
  ..............
     
    
DEPENDENCIES 
  None.

RETURN VALUE
  DS3G_SUCCESS : if successful
  DS3G_FAILURE : on error
  
SIDE EFFECTS
  
===========================================================================*/
boolean ds_pdn_decode_operator_reserved_pco_pkt
(
 const byte           *protocol_pkt_ptr,/* Pointer to the buffer to   */
                            /* extract options from     */
 int                  protocol_pkt_len,/* Length of packet to extract */
                           /* operator's mcc, mnc info from               */
 ds_pdn_mgr_s      *pdn_mgr_p,
 uint16             protocol_id,
 boolean            profile_pco_id_configured
);


#endif /* DS_PDN_PCO_H */
