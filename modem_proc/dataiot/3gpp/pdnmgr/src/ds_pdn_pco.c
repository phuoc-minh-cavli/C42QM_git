/*===========================================================================
                      DS_PDN_PCO
DESCRIPTION
This file defines PDN PCO APIs.

Copyright (c) 2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/3gpp/pdnmgr/src/ds_pdn_pco.c#2 $
  $DateTime: 2023/01/19 23:06:43 $$Author: pwbldsvc $

when           who    what, where, why
--------    ---    ----------------------------------------------------------
08/09/17    sd      First checkin
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "ds_pdn_defs.h"
#include "md5.h"
#include "ds_pdn_pco.h"

/* Length of config packet header */
#define CONFIG_HDR_LEN 4

/*---------------------------------------------------------------------------
  IPCP option types
---------------------------------------------------------------------------*/
#define IPCP_COMPRESS        2
#define IPCP_1ADDRESS        3
#define IPCP_PRIMARY_DNS     129
#define IPCP_PRIMARY_NBNS    130  /* For WINS support */
#define IPCP_SECONDARY_DNS   131
#define IPCP_SECONDARY_NBNS  132 /* For WINS support */
#define IPCP_OPTION_LIMIT    132  /* highest # we can handle */

/*---------------------------------------------------------------------------
  Size of the accepted options
---------------------------------------------------------------------------*/
#define IPCP_COMPRESS_SIZ      4
#define IPCP_1ADDRESS_SIZ      6
#define IPCP_PRIMARY_DNS_SIZ   IPCP_1ADDRESS_SIZ
#define IPCP_SECONDARY_DNS_SIZ IPCP_1ADDRESS_SIZ
#define IPCP_PRIMARY_NBNS_SIZ   IPCP_1ADDRESS_SIZ /* For WINS support */
#define IPCP_SECONDARY_NBNS_SIZ IPCP_1ADDRESS_SIZ /* For WINS support */
#define IPCP_COMPRESS_VJ_SIZ   6

#define IPCP_SLOT_DEFAULT      16                /* Default # of slots     */
#define IPCP_SLOT_COMPRESS     1                 /* May compress slot id   */
#define IPCP_NO_SLOT_COMPRESS  0                 /* Don't compress slot id */


/*---------------------------------------------------------------------------
  The code values for LCP/IPCP packets
---------------------------------------------------------------------------*/
enum
{
 VENDOR_SPECIFIC =  0,
 CONFIG_REQ      =  1,
 CONFIG_ACK      =  2,
 CONFIG_NAK      =  3,
 CONFIG_REJ      =  4,
 TERM_REQ        =  5,
 TERM_ACK        =  6,
 CODE_REJ        =  7,
 PROT_REJ        =  8,
 ECHO_REQ        =  9,
 ECHO_REPLY      = 10,
 DISCARD_REQ     = 11,
 IDENTIFICATION  = 12,
 TIME_REMAINING  = 13
};



/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/*===========================================================================
                   INTERNAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
                    EXTERNAL FUNCTION DEFINITIONS
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
  const byte *user_id_ptr,
  byte  user_id_len,
  const byte *passwd_ptr,
  byte  passwd_len
)
{
  int       pap_pkt_len;
  uint8    *cp;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(pkt_buff_ptr == NULL || user_id_ptr == NULL || passwd_ptr == NULL)
  {
    return -1;
  } 

  /*-------------------------------------------------------------------------
    Find the length of the packet to be generated     
    Pkt format: This is a PAP config. request : 
    |Pkt type|Pkt ID |Len|user id len|user id|passwd len|passwd
    | 1 byte |1 byte |2  | 1 byte    |< 255  | 1        | <255   
    Len is the length of the whole pkt starting at pkt-type
    CONFIG_HDR_LEN includes pkt_type, pkt_id and length fields.

    Since user_id-len and passwd-len do not include their own size, we add 2
    ( 1 each for user_id len & passwd_len )
  -------------------------------------------------------------------------*/
  pap_pkt_len = CONFIG_HDR_LEN 
                + user_id_len 
                + passwd_len 
                + 2 ;
  
  if( pap_pkt_len > pkt_buff_len )
  {
    return -1;
  }
         
  /*-------------------------------------------------------------------------
    Construct the packet
  -------------------------------------------------------------------------*/
  cp  = pkt_buff_ptr;
  
  /*-------------------------------------------------------------------------
    Fill Pkt type  
  -------------------------------------------------------------------------*/
  *cp++ = CONFIG_REQ;  

  /*-------------------------------------------------------------------------
    Fill Identifier
  -------------------------------------------------------------------------*/
  *cp++ = identifier;           

  /*-------------------------------------------------------------------------
    Fill Pkt length    , put16 increments the ptr by two
  -------------------------------------------------------------------------*/
  cp  = put16( cp, (word)pap_pkt_len );

  if(cp == NULL)
  {
    return -1;
  }    

  /*-------------------------------------------------------------------------
    Make the user-id field in the PAP config-req.
  -------------------------------------------------------------------------*/
  /*-------------------------------------------------------------------------
    Put in the id-length field  
  -------------------------------------------------------------------------*/
  *cp++ = user_id_len; 
  
  /*-------------------------------------------------------------------------
    Put the user id in the item      
  -------------------------------------------------------------------------*/
  memscpy( cp, user_id_len, user_id_ptr,user_id_len);   
    
  /*-------------------------------------------------------------------------
    Increment the pointer to store the password 
  -------------------------------------------------------------------------*/
  cp  += user_id_len;  
    
  /*-------------------------------------------------------------------------
    Make the password field in the PAP config-req.
  -------------------------------------------------------------------------*/
     
  /*-------------------------------------------------------------------------
    Put in the passwd-length field    
  -------------------------------------------------------------------------*/
  *cp++ = passwd_len;     
     
  /*-------------------------------------------------------------------------
   Put the passsword in the packet    
  -------------------------------------------------------------------------*/
  memscpy( cp, passwd_len, passwd_ptr, passwd_len);   
  return pap_pkt_len;
} /* ds_pdn_ppp_gen_pap_auth_req_pkt() */


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
)
{
  int       chap_pkt_len;
  uint8    *cp;
  int       dest_buff_len = pkt_buff_len;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(pkt_buff_ptr == NULL || challenge_ptr == NULL || sys_name_ptr == NULL)
  {
    return -1;
  } 

  /*-------------------------------------------------------------------------
    Find the length of the packet to be generated     
    pkt format : Chap Challenge Pkt :
    Pkt type|Pkt ID |Len|value size |value  | name
    |1 byte |1 byte |2  | 1 byte    |< 255  | 
    Len is the length of the whole pkt starting at pkt-type
    CONFIG_HDR_LEN includes pkt_type, pkt_id and length fields.

    We add 1 since value_size does not include its own size
    ( 1 byte for value_size )
  -------------------------------------------------------------------------*/
  chap_pkt_len = CONFIG_HDR_LEN 
                + challenge_len 
                + sys_name_len 
                + 1 ;
  
  if( chap_pkt_len > pkt_buff_len )
  {
    return -1;
  }

  /*-------------------------------------------------------------------------
    Creating the packet
  -------------------------------------------------------------------------*/
  cp  = pkt_buff_ptr;
  
  /*-------------------------------------------------------------------------
    Fill Pkt type  
  -------------------------------------------------------------------------*/
  *cp++ = CODE_CHAP_CHALLENGE;  
  dest_buff_len --;

  /*-------------------------------------------------------------------------
    Fill Identifier
  -------------------------------------------------------------------------*/
  *cp++ = identifier;           
  dest_buff_len --;
  /*-------------------------------------------------------------------------
    Fill Pkt length : put16 increments the ptr by two
  -------------------------------------------------------------------------*/
  cp  = put16( cp, (word)chap_pkt_len );
  dest_buff_len = dest_buff_len - 2;
    
  if(cp == NULL)
  {
    return -1;
  }  

  /*-------------------------------------------------------------------------
    Make the challenge ( value) field of chap packet
  -------------------------------------------------------------------------*/
  /*-------------------------------------------------------------------------
    Put in the value size field
  -------------------------------------------------------------------------*/
  *cp++ = challenge_len; 
  dest_buff_len--;
  
  /*-------------------------------------------------------------------------
    Put in the challenge
  -------------------------------------------------------------------------*/
  memscpy( cp, dest_buff_len, challenge_ptr,challenge_len);   
    
  /*-------------------------------------------------------------------------
    Increment the pointer to store the name
  -------------------------------------------------------------------------*/
  cp  += challenge_len;  
  dest_buff_len = dest_buff_len - challenge_len;
  /*-------------------------------------------------------------------------
    Make the name field. The length is not coded separately. Rather 
    inferred from the packet length
  -------------------------------------------------------------------------*/
  memscpy( cp, dest_buff_len,sys_name_ptr, sys_name_len);   

  return chap_pkt_len; 
} /* dswpsd_ppp_gen_chap_challenge_pkt() */


/*===========================================================================
FUNCTION DSUMTSPS_PPP_GEN_CHAP_RESPONSE_PKT

DESCRIPTION
  This function generates a CHAP response  packet. It uses the parameters
  passed in to digest the challenge & generate the reponse & then construct
  the ppp chap response packet
  
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
)
{
  uint8    *cp;
  MD5_CTX   context;  /*contains the seed to be used for making MD5 output */
  unsigned char digest[16];/* array for the 128 bit result of MD5 digest   */
                           /* algorithm                                    */
  uint16    chap_response_length; /*length of the Chap Response to be sent */
  dsm_item_type *item_ptr;    /*Pointer to the Message for MD5 Digest algo */
  byte      *bp;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  if(pkt_buff_ptr == NULL || challenge_ptr == NULL || secret_ptr == NULL || 
                                                        sys_name_ptr == NULL)
  {
    return -1;
  } 

  /*-------------------------------------------------------------------------
    Check if the challenge lengths is 0. 
    If yes, then return without generating any packet
  -------------------------------------------------------------------------*/

  if( challenge_len == 0 ) 
  {
    return -1;
  }

  /*-------------------------------------------------------------------------
    Find the length of the packet to be generated     
    pkt format : Chap Response Pkt :
    |Pkt type|Pkt ID |Len|value size |value  | name
    | 1 byte |1 byte |2  | 1 byte    |< 255  | 
    Len is the length of the whole pkt starting at pkt-type
    CONFIG_HDR_LEN includes pkt_type, pkt_id and length fields.

    We add 1 since value_size does not include its own size
    ( 1 byte for value_size )
    
    Note : Value is 16 bytes if MD5 is used
  -------------------------------------------------------------------------*/
  chap_response_length =  CONFIG_HDR_LEN 
                          + MD5_DIGEST_SIZE 
                          + sys_name_len 
                          + 1 ;

  if( chap_response_length > pkt_buff_len )
  {
    return -1;
  }

  /*-------------------------------------------------------------------------
    Build message for preparing the 16 octet message-digest output. The 
    message is built by appending the ID, Secret and the Challenge. 
    Obtain the challenge from the chap_challenge_ptr. 
    Obtain the secret from secret_ptr
  
    Obtain a new dsm_item_type to build the message. This message will be 
    given to MD5 algorithm for generating the 128bit Message digest.
    The message will have a length of :
    1  ( size of idenfifier  )
    + secret_len           
    + challenge_len
            
    item_ptr will point to the new item. This item will be filled with
    chap challenge ID , Secret and Challenge.
  -------------------------------------------------------------------------*/
  if((item_ptr  = dsm_new_buffer(DSM_DS_POOL_SIZE (1
                                  + secret_len
                                  + challenge_len))) == NULL)
    {
       return -1;
    }

  /*-------------------------------------------------------------------------
    Obtain the pointer to the data in the item_ptr. This will be used to 
    fill the message for the MD5 input.
  -------------------------------------------------------------------------*/
  bp = item_ptr->data_ptr;

  /*-------------------------------------------------------------------------
    Fill the incoming challenge's ID 
    Fill the secret of the Mobile Station. 
    Fill the challenge from the challenge_ptr. 
  -------------------------------------------------------------------------*/
  *bp++ = identifier;

  memscpy( bp, secret_len,secret_ptr, secret_len);
  bp += secret_len;

  memscpy( bp,challenge_len, challenge_ptr,challenge_len);
            
  /*-------------------------------------------------------------------------
    By now, the input for the MD5 has been prepared. This should now be given
    to the MD5 algorithm to prepare the 128bit output. bp should again point
    to the beginning of the data i.e. points to the chap ID. bp, will be 
    passed to the MD5 algorithm as an input.
  -------------------------------------------------------------------------*/
  bp = item_ptr->data_ptr;

  /*-------------------------------------------------------------------------
    Initialize MD5 with a new context.  This function loads magic 
    initialization constants.
  -------------------------------------------------------------------------*/
  MD5Init(&context);

  /*-------------------------------------------------------------------------
    Update the context with the new message block.
    The inputs that are expected are:
    MD5_CTX *context for context 
    unsigned char *input for input block
    unsigned int inputLen for length of input block 
  -------------------------------------------------------------------------*/
  MD5Update(&context, 
          bp, 
          1+challenge_len+secret_len);
  
  /*-------------------------------------------------------------------------
    MD5 finalization. Ends an MD5 message-digest operation, writing the
    the message digest and zeroizing the context. The following parameters
    should be passed:
    unsigned char digest[16] for message digest 
    MD5_CTX *context for context
  -------------------------------------------------------------------------*/
  MD5Final(digest, &context);

  /*-----------------------------------------------------------------------
    Free the dsm_item which had the message to be digested
  -------------------------------------------------------------------------*/
  dsm_free_packet(&item_ptr);   
        
  /*-------------------------------------------------------------------------
    By now, all the information needed to be put in the Chap Response is
    available. So, start building the response pointed by, Chap_response_ptr 
    Make the follwing for the Response:
    Code field should be CHAP_RESPONSE
    Identifier field 
    Length field should be Chap_response_length 
    Value_size field should be MD5_Size. This will be 16 for  MD5.
    Value field should be the contents of the digest array.
    Name field should be the sys_name_ptr
        
    Use cp to point to the data_ptr in the chap_response_ptr and update the
    fields sequentially.
  -------------------------------------------------------------------------*/
  cp      = pkt_buff_ptr;                     /* cp points to byte array   */
  *cp++   = CODE_CHAP_RESPONSE;               /* Fill in response code     */
  *cp++   = identifier;                       /* Fill in the identifier    */
  cp      = put16( cp,chap_response_length ); /*Fill in length field       */

  if(cp == NULL)
  {
    return -1; 
  }  
  *cp++   = MD5_DIGEST_SIZE;                  /* Fill in value size field  */
                                              
  memscpy(cp, MD5_DIGEST_SIZE, digest, MD5_DIGEST_SIZE);        /* copy 128bit response      */
  cp += MD5_DIGEST_SIZE;                      /* Increment cp by 16bytes   */

  /*-------------------------------------------------------------------------
    Copy the sys_name into the response packet.
  -------------------------------------------------------------------------*/
  memscpy( cp, sys_name_len, sys_name_ptr,sys_name_len);
          
  return chap_response_length;
} /* ds_pdn_ppp_gen_chap_challenge_pkt() */


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
)
{
  uint8    *cp;
  uint16    chap_response_length; /*length of the Chap Response to be sent */
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(pkt_buff_ptr == NULL || hash_ptr == NULL || responder_name_ptr == NULL)
  {
    return -1;
  } 

  /*-------------------------------------------------------------------------
    Check if the hash length is 0. 
    If yes, then return without generating any packet
  -------------------------------------------------------------------------*/
  if(  hash_len == 0  ) 
  {
    return -1;
  }
  
  /*-------------------------------------------------------------------------
    Find the length of the packet to be generated     
    pkt format : Chap Response Pkt :
    |Pkt type|Pkt ID |Len|value size |value  | name
    | 1 byte |1 byte |2  | 1 byte    |< 255  | 
    Len is the length of the whole pkt starting at pkt-type
    CONFIG_HDR_LEN includes pkt_type, pkt_id and length fields.

    We add 1 since value_size does not include its own size
    ( 1 byte for value_size )
    
    Note : Value is 16 bytes if MD5 is used
  -------------------------------------------------------------------------*/
  chap_response_length =  CONFIG_HDR_LEN 
                          + hash_len 
                          + responder_name_len 
                          + 1 ;

  if( chap_response_length > pkt_buff_len )
  {
    return -1;
  }
  
  /*-------------------------------------------------------------------------
    start building the response pointed by, Chap_response_ptr 
    Make the follwing for the Response:
    Code field should be CHAP_RESPONSE
    Identifier field 
    Length field should be Chap_response_length 
    Value_size field should be MD5_Size. This will be 16 for  MD5.
    Value field should be the contents of the digest array.
    Name field should be the sys_name_ptr
        
    Use cp to point to the data_ptr in the chap_response_ptr and update the
    fields sequentially.
  -------------------------------------------------------------------------*/
  cp      = pkt_buff_ptr;                     /* cp points to byte array   */
  *cp++   = CODE_CHAP_RESPONSE;               /* Fill in response code     */
  *cp++   = identifier;                       /* Fill in the identifier    */
  cp      = put16( cp,chap_response_length ); /*Fill in length field       */

  if(cp == NULL)
  {
    return -1;
  }  

  *cp++   = hash_len;                         /* Fill in value size field  */
                                              
  memscpy(cp, hash_len, hash_ptr, hash_len);             /* copy hash response        */
  cp += MD5_DIGEST_SIZE;                      /* Increment cp by 16bytes   */

  /*-------------------------------------------------------------------------
    Copy the sys_name into the response packet.
  -------------------------------------------------------------------------*/
  memscpy( cp, responder_name_len, responder_name_ptr,responder_name_len);
          
  return chap_response_length;
} /* ds_pdn_ppp_fill_chap_response_pkt() */


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
)
{
  int       dns_pkt_len;                          /* Length of IPCP packet */  
  byte     *buf_ptr;                       /* Buffer to build packet */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(protocol_pkt_ptr == NULL)
  {
    return -1;
  } 

  /*-------------------------------------------------------------------------
    Pkt format: This is a Primary and Secondary DNS config. request packet:
    
     |Code|Identifier|Length|Type|Length|Primary DNS|Type|Len|Secondary DNS|         
       1       1        2     1     1        4        1    1      4
   
    Find the length of the packet to be generated.
    CONFIG_HDR_LEN = length of (Code + identifer + length)
    IPCP_PRIMARY_DNS_SIZ = Length of IPCP Primary DNS option
                           (Type + Len + Primary DNS)
    IPCP_SECONDARY_DNS_SIZ = Length of IPCP Secondary DNS option
                             (Type + Len + Secondary DNS)
  -------------------------------------------------------------------------*/
  dns_pkt_len = CONFIG_HDR_LEN +
                IPCP_PRIMARY_DNS_SIZ;
  /*lint -e655*/
  if (valid_fields & NET_CFG_PRM_SEC_DNS_MASK) {  
    dns_pkt_len += IPCP_SECONDARY_DNS_SIZ;
  }
  /*lint +e655*/
  
  if( dns_pkt_len > protocol_pkt_len )
  {
    return -1;
  }
         
  /*-------------------------------------------------------------------------
    Construct the packet
  -------------------------------------------------------------------------*/
  buf_ptr  = protocol_pkt_ptr;
  
  /*-------------------------------------------------------------------------
    Fill Code - 0 : CONFIG_REQ  
  -------------------------------------------------------------------------*/
  *buf_ptr++ = CONFIG_REQ;  

  /*-------------------------------------------------------------------------
    Fill Identifier
  -------------------------------------------------------------------------*/
  *buf_ptr++ = identifier;    

  /*-------------------------------------------------------------------------
    Fill Packet length
  -------------------------------------------------------------------------*/
   buf_ptr  = put16( buf_ptr, (word)dns_pkt_len );

  if(buf_ptr == NULL)
  {
    return -1; /* If ASSERT is disabled, -1 will fail call origination*/
  }  

  /*-------------------------------------------------------------------------
    Fill in the Type field for Primary DNS - 129
  -------------------------------------------------------------------------*/
  *buf_ptr++ = IPCP_PRIMARY_DNS;

  /*-------------------------------------------------------------------------
    Fill in the length field for Primary DNS
  -------------------------------------------------------------------------*/
  *buf_ptr++ = IPCP_PRIMARY_DNS_SIZ;

  /*-------------------------------------------------------------------------
   Fill in the primary DNS value(4 bytes) - Set it to the value requested
  -------------------------------------------------------------------------*/
  buf_ptr = hput32(buf_ptr,primary_dns);

  if(buf_ptr == NULL)
  {
    return -1; /* If ASSERT is disabled, -1 will fail call origination*/
  }    

  if (((uint32) valid_fields) & ((uint32) NET_CFG_PRM_SEC_DNS_MASK)) {
    /*-------------------------------------------------------------------------
      Fill in the Type field for Secondary DNS - 131
    -------------------------------------------------------------------------*/
    *buf_ptr++ = IPCP_SECONDARY_DNS;
  
    /*-------------------------------------------------------------------------
      Fill in the length field for Secondary DNS
    -------------------------------------------------------------------------*/
    *buf_ptr++ = IPCP_SECONDARY_DNS_SIZ;
  
    /*-------------------------------------------------------------------------
     Fill in the Secondary DNS value(4 bytes) - Set it to the value requeste
    -------------------------------------------------------------------------*/
    buf_ptr = hput32(buf_ptr,secondary_dns);
  
    if(buf_ptr == NULL)
    {
      return -1;
    } 
  }

  return dns_pkt_len;
} /* ds_pdn_gen_ipcp_req_pkt() */
    

#ifdef FEATURE_DATA_IMS
/*===========================================================================
FUNCTION DS3GPP_DECODE_V6_PCSCF_PKT

DESCRIPTION
  This function decodes the PCSCF packet received from the network. The
  pcscf addresses, if any sent by the network, are stored in ds instance
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
  int              protocol_pkt_len,  /* Length of packet to extract pcscf addresse frm*/
  ps_ip_addr_type *pcscf_v4_addr_array,           /* v4 pcscf addresses received */
  uint32          *num_v4_pcscf_addresses, /* number of v4 pcscf addresses processed till now */
  ps_ip_addr_type *pcscf_v6_addr_array,           /* v6 pcscf addresses received */
  uint32          *num_v6_pcscf_addresses /* number of v6 pcscf addresses processed till now */
)
{
  uint32    num_addr = 0;
  struct ps_in_addr  v4_addr;
  struct ps_in6_addr v6_addr;
  
  /*-------------------------------------------------------------------------
    Network is updating the v6 PCSCF address to zero update same thing to the client
  -------------------------------------------------------------------------*/
  if(protocol_pkt_len  == 0)
  {
   /* address is v6, store it in the v6 array */
    num_addr = *num_v6_pcscf_addresses;
    if(num_addr >= MAX_PCSCF_ADDRESSES)
    {
      DS_ERROR_LOG_0("More than max pcscf addresses not supported");
      return FALSE;
    }
    
    pcscf_v6_addr_array[num_addr].type = IPV4_ADDR;
    memscpy(pcscf_v6_addr_array[num_addr].addr.v6.ps_s6_addr,
            sizeof(pcscf_v6_addr_array[num_addr].addr.v6.ps_s6_addr),
       	    protocol_pkt_ptr, 
       	    sizeof(struct ps_in6_addr) );
    (*num_v6_pcscf_addresses)++;	
    return TRUE;   
  }

  /*-------------------------------------------------------------------------
    Make sure the length of protocol packet is equal to IPV6 address 
  -------------------------------------------------------------------------*/
  if(protocol_pkt_len != DS_IPV6_ADDR_LEN)
  {
    DS_ERROR_LOG_0("Wrong pcscf address, incorrect length!");
    return FALSE;
  }

  /*-----------------------------------------------------------------------
    Process only one pcscf addresses. If more than one pcscf addresses are
    received in context accept message, they would be in seperate
    containers. Extract the lower 64 bits and upper 64 bits into the IPv6
    IP addrdess array.
  -----------------------------------------------------------------------*/
  memscpy(&v6_addr, sizeof(struct ps_in6_addr), protocol_pkt_ptr, 
          sizeof(struct ps_in6_addr) );

  v4_addr.ps_s_addr = PS_IN6_GET_V4_FROM_V4_MAPPED_V6(&v6_addr);
  if( v4_addr.ps_s_addr == 0)
  {
    /* address is v6, store it in the v6 array */
    num_addr = *num_v6_pcscf_addresses;
    if(num_addr >= MAX_PCSCF_ADDRESSES)
    {
      DS_ERROR_LOG_0("More than max pcscf addresses not supported");
      return FALSE;
    }
    pcscf_v6_addr_array[num_addr].type = IPV6_ADDR;
    memscpy(pcscf_v6_addr_array[num_addr].addr.v6.ps_s6_addr,
            sizeof(pcscf_v6_addr_array[num_addr].addr.v6.ps_s6_addr),
       	    protocol_pkt_ptr, 
       	    sizeof(struct ps_in6_addr));
    (*num_v6_pcscf_addresses)++;
  }
  else
  {
    /* address is v4, store it in the v4 array */
    num_addr = *num_v4_pcscf_addresses;
    if(num_addr >= MAX_PCSCF_ADDRESSES)
    {
      DS_ERROR_LOG_0("More than max pcscf addresses not supported");
      return FALSE;
    }
    /* Copy the adress to v4 format */
    pcscf_v4_addr_array[num_addr].type = IPV4_ADDR;
    pcscf_v4_addr_array[num_addr].addr.v4 = v4_addr;
    (*num_v4_pcscf_addresses)++;
  }

  return TRUE;
}/* ds_pdn_decode_v6_pcscf_pkt */

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
  int              protocol_pkt_len,  /* Length of packet to extract pcscf addresse frm*/
  ps_ip_addr_type *pcscf_v4_addr_array,           /* v4 pcscf addresses received */
  uint32          *num_v4_pcscf_addresses /* number of v4 pcscf addresses processed till now */
)
{
  uint32    num_addr = 0;
  struct    ps_in_addr  v4_addr;
  
  /*-------------------------------------------------------------------------
    Network is updating the v4 P-CSCF address to zero update same thing to the client
  -------------------------------------------------------------------------*/
  if(protocol_pkt_len  == 0)
  {
   /* address is v4, store it in the v4 array */
    num_addr = *num_v4_pcscf_addresses;
    if(num_addr >= MAX_PCSCF_ADDRESSES)
    {
      DS_ERROR_LOG_0("More than max pcscf addresses not supported");
      return FALSE;
    }
    
    memscpy(&v4_addr, sizeof(struct ps_in_addr), 
    	       protocol_pkt_ptr, sizeof(struct ps_in_addr) );
    pcscf_v4_addr_array[num_addr].type = IPV4_ADDR;
    pcscf_v4_addr_array[num_addr].addr.v4 = v4_addr;
    (*num_v4_pcscf_addresses)++;
    return TRUE;  
  }
  
  /*-------------------------------------------------------------------------
    Make sure the length of protocol packet is equal to IPV6 address 
  -------------------------------------------------------------------------*/
  if(protocol_pkt_len != DS_IPV4_ADDR_LEN)
  {
    DS_ERROR_LOG_0("Wrong pcscf address, incorrect v4 length!");
    return FALSE;
  }

  /*-----------------------------------------------------------------------
    Process only one pcscf addresses. If more than one pcscf addresses are
    received in context accept message, they would be in seperate
    containers. Extract the lower 64 bits and upper 64 bits into the IPv6
    IP addrdess array.
  -----------------------------------------------------------------------*/
  memscpy(&v4_addr, sizeof(struct ps_in_addr), protocol_pkt_ptr, 
          sizeof(struct ps_in_addr) );

  if( v4_addr.ps_s_addr != 0)
  {
    num_addr = *num_v4_pcscf_addresses;
    if(num_addr >= MAX_PCSCF_ADDRESSES)
    {
      DS_ERROR_LOG_0("More than max pcscf addresses not supported");
      return FALSE;
    }
    pcscf_v4_addr_array[num_addr].type = IPV4_ADDR;
    pcscf_v4_addr_array[num_addr].addr.v4 = v4_addr;
    (*num_v4_pcscf_addresses)++;
  }
  else
  {
    DS_ERROR_LOG_0("NULL v4 address in PCO");
    return FALSE;
  }

  return TRUE;
}/* ds_pdn_decode_v4_pcscf_pkt */

#endif // FEATURE_DATA_IMS 

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
  const byte         *protocol_pkt_ptr, /* Pointer to the buffer to extract ipcp   */
                                /* options from                            */
  int                 protocol_pkt_len,  /* Length of packet to extract ipcp options from*/
  ds_ipcp4_options_s *ipcp_options             /* IPCP options received */
)
{
   dword dns_val = 0;
   word length;                                   /* Length of IPCP packet */
   byte config_type;                 /* Identifies the kind of IPCP packet */
   byte identifier;                               /* Identifies the packet */
   byte opt_type;                                           /* Option Type */
   byte opt_len = 0;                                      /* Option length */
   ds_local_err_val_e_type local_err = DS_LOCAL_ERR_VAL_NONE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  while (protocol_pkt_len > 0) 
  {
    /*-----------------------------------------------------------------------
      Get the Message Type.     
    -----------------------------------------------------------------------*/
    config_type = *protocol_pkt_ptr++;

    /*-----------------------------------------------------------------------
      Get the identifier value. 
    -----------------------------------------------------------------------*/
    identifier = *protocol_pkt_ptr++;
       
    /*-----------------------------------------------------------------------
      Get the length of the packet.
    -----------------------------------------------------------------------*/
    length = get16((byte *)protocol_pkt_ptr);
    protocol_pkt_ptr += 2;

    if (protocol_pkt_len < length) 
    {
      local_err = DS_LOCAL_ERR_VAL_0;
      break;
    }

    protocol_pkt_len -= length;

    if (length < CONFIG_HDR_LEN) 
    {
      local_err = DS_LOCAL_ERR_VAL_1;
      break;
    }
    /*-----------------------------------------------------------------------
      Subtract CONFIG_HDR_LEN from total length.
    -----------------------------------------------------------------------*/
    length -= CONFIG_HDR_LEN;

    /*-----------------------------------------------------------------------
      Based on the type of message, do appropriate processing.
      CONFIG_NAK/CONFIG_ACK/CONFIG_REJ are the only types processed. All 
      others are silently discarded.
    -----------------------------------------------------------------------*/
    switch (config_type) 
    {
      case CONFIG_REQ:        
        DS_MSG0(MSG_LEGACY_HIGH," CONFIG_REQ IPCP packet recvd");
        /*-------------------------------------------------------------------
          Received a CONFIG_REQ. Only Gatewary IP Address is processed in
          the CONFIG_REQ packet received from the network.
        -------------------------------------------------------------------*/        
        while (length) 
        {
          /*-----------------------------------------------------------------
            Get the option type.
          -----------------------------------------------------------------*/
          opt_type = *protocol_pkt_ptr++;

          /*-----------------------------------------------------------------
            Get the option length.
          -----------------------------------------------------------------*/
          opt_len = *protocol_pkt_ptr++;

          /*-----------------------------------------------------------------
            If remaining IPCP pkt Length is less than the option length, 
            it is an error. Return.
          -----------------------------------------------------------------*/
          if (length < opt_len) 
          {
            local_err = DS_LOCAL_ERR_VAL_2;
            break;
          }

          /*-----------------------------------------------------------------
            If option length is zero, the packet is bad. Return to avoid  
            entering an infinite loop.
          -----------------------------------------------------------------*/
          if (opt_len == 0) 
          {
            local_err = DS_LOCAL_ERR_VAL_4;
            break;
          }

          switch (opt_type)
          {
            case IPCP_1ADDRESS:
              /*-------------------------------------------------------------
                If the option is IPCP_1ADDRESS, and the length does not equal
                the address size(6), return. Otherwise, get the address.
              -------------------------------------------------------------*/
              if (opt_len != IPCP_1ADDRESS_SIZ) 
              {
                local_err = DS_LOCAL_ERR_VAL_5;
                break;
              }
              else
              {
                ipcp_options->gateway_addr = get32((byte *)protocol_pkt_ptr);
                protocol_pkt_ptr +=4;
                length -= opt_len;
              }
              break;

            default:
              DS_MSG1(MSG_LEGACY_HIGH,"Option type not supported %d",opt_type);
              protocol_pkt_ptr += (opt_len - 2);
              length -= opt_len;
          }
        } /* while length */
        break;

      case CONFIG_NAK:
         DS_MSG0(MSG_LEGACY_HIGH, "CONFIG NAK IPCP packet recvd.");
        /*-------------------------------------------------------------------
          Check if the identifer matches the value stored in the call table. 
          If not, discard the packet.
        -------------------------------------------------------------------*/
        if (identifier != ipcp_options->identifier) 
        {
          protocol_pkt_ptr += length;
          continue;
        }

        /*-------------------------------------------------------------------
          Received a CONFIG_NAK. The option values are different from the 
          ones requested in the PDP context activation request message.
        -------------------------------------------------------------------*/        
        while (length) 
        {
          /*-----------------------------------------------------------------
            Get the option type.
          -----------------------------------------------------------------*/
          opt_type = *protocol_pkt_ptr++;

          /*-----------------------------------------------------------------
            Get the option length.
          -----------------------------------------------------------------*/
          opt_len = *protocol_pkt_ptr++;

          /*-----------------------------------------------------------------
            If remaining IPCP pkt Length is less than the option length, 
            it is an error. Return.
          -----------------------------------------------------------------*/
          if (length < opt_len) 
          {
            local_err = DS_LOCAL_ERR_VAL_6;
            break;
          }

          /*-----------------------------------------------------------------
            If option length is zero, the packet is bad. Return to avoid  
            entering an infinite loop.
          -----------------------------------------------------------------*/
          if (opt_len == 0) 
          {
            local_err = DS_LOCAL_ERR_VAL_7;
            break;
          }

          switch (opt_type)
          {
            case IPCP_PRIMARY_DNS:
              /*-------------------------------------------------------------
                If the option is PRIMARY DNS, and the length does not equal
                the PRIMARY DNS size(6), return. Otherwise, get the 
                primary DNS address.
              -------------------------------------------------------------*/
              if (opt_len != IPCP_PRIMARY_DNS_SIZ) 
              {
                local_err = DS_LOCAL_ERR_VAL_8;
                break;
              }
              else
              {
                ipcp_options->primary_dns = get32((byte *)protocol_pkt_ptr);
                protocol_pkt_ptr +=4;
                length -= opt_len;
                DS_MSG1(MSG_LEGACY_HIGH, "CONFIG NACK: Received PDNS %lx",
                        (unsigned long)ipcp_options->primary_dns);
              }
              break;

            case IPCP_SECONDARY_DNS:
              /*-------------------------------------------------------------
                If the option is SECONDARY DNS, and the length does not equal
                the SECONDARY DNS size (6), return. Otherwise, get the 
                secondary DNS address.
              -------------------------------------------------------------*/
              if (opt_len != IPCP_SECONDARY_DNS_SIZ) 
              {
                local_err = DS_LOCAL_ERR_VAL_11;
                break;
              }
              else
              {
                ipcp_options->secondary_dns = get32((byte *)protocol_pkt_ptr);
                protocol_pkt_ptr +=4;
                length -= opt_len;
                DS_MSG1(MSG_LEGACY_HIGH, "CONFIG NACK: Received SDNS %lx",
                        (unsigned long)ipcp_options->secondary_dns);
              }
            break;

            default:
              protocol_pkt_ptr += (opt_len - 2);
              length -= opt_len;
          }
        } /* while length */
        break;

      case CONFIG_ACK:        
        DS_MSG0(MSG_LEGACY_HIGH, "CONFIG ACK IPCP packet recvd. ");
        /*-------------------------------------------------------------------
          Check if the identifer matches the value stored in the call table. 
          If not, discard the packet.
        -------------------------------------------------------------------*/
        if (identifier != ipcp_options->identifier) 
        {
          protocol_pkt_ptr += length;
          continue;
        }

        /*-------------------------------------------------------------------
          Received a CONFIG_ACk. The option values must match the ones sent 
          in the PDP context activation message.
        -------------------------------------------------------------------*/        
        while (length) 
        {
          /*-----------------------------------------------------------------
            Get the option type.
          -----------------------------------------------------------------*/
          opt_type = *protocol_pkt_ptr++;

          /*-----------------------------------------------------------------
            Get the option length.
          -----------------------------------------------------------------*/
          opt_len = *protocol_pkt_ptr++;

          /*-----------------------------------------------------------------
            If remaining IPCP pkt Length is less than the option length, 
            it is an error. Return.
          -----------------------------------------------------------------*/
          if (length < opt_len) 
          {
            local_err = DS_LOCAL_ERR_VAL_12;
            break;
          }

          /*-----------------------------------------------------------------
            If option length is zero, the packet is bad. Return to avoid  
            entering an infinite loop.
          -----------------------------------------------------------------*/
          if (opt_len == 0) 
          {
            local_err = DS_LOCAL_ERR_VAL_13;
            break;
          }

          switch (opt_type)
          {
            case IPCP_PRIMARY_DNS: 
              /*-------------------------------------------------------------
                If the option is PRIMARY DNS, and the length does not equal
                the PRIMARY DNS size(6), return. Otherwise, get the primary
                DNS address.
              -------------------------------------------------------------*/
              if (opt_len != IPCP_PRIMARY_DNS_SIZ) 
              {
                local_err = DS_LOCAL_ERR_VAL_14;
                break;
              }
              else
              {
                if ((dns_val = get32((byte *)protocol_pkt_ptr)) != 0)
                {
                  DS_MSG1(MSG_LEGACY_HIGH, 
                          "CONFIG ACK: Received non-zero PDNS %lx",
                          (unsigned long)dns_val);
                  ipcp_options->primary_dns = dns_val;
                }
                protocol_pkt_ptr +=4;
                length -= opt_len;
              }
            break;

            case IPCP_SECONDARY_DNS:
              /*-------------------------------------------------------------
                If the option is SECONDARY DNS, and the length does not equal
                the SECONDARY DNS size (6), return. Otherwise, get the 
                secondary DNS address.
              -------------------------------------------------------------*/
              if (opt_len != IPCP_SECONDARY_DNS_SIZ) 
              {
                local_err = DS_LOCAL_ERR_VAL_16;
                break;
              }
              else
              {
                if ((dns_val = get32((byte *)protocol_pkt_ptr)) != 0)
                {
                  DS_MSG1(MSG_LEGACY_HIGH,
                  	       "CONFIG ACK: Received non-zero SDNS %lx",
                          (unsigned long)dns_val);
                  ipcp_options->secondary_dns = dns_val;
                }
                protocol_pkt_ptr +=4;
                length -= opt_len;
              }
            break;

            default:
              protocol_pkt_ptr += (opt_len - 2);
              length -= opt_len;
          }
        } /* while (length) */
        break;

      case CONFIG_REJ:
        DS_MSG0(MSG_LEGACY_HIGH, "CONFIG REJ IPCP packet recvd.");
        /*-------------------------------------------------------------------
          Check if the identifer matches the value stored in the call table. 
          If not, discard the packet.
        -------------------------------------------------------------------*/
        if (identifier != ipcp_options->identifier) 
        {
          protocol_pkt_ptr += length;
          continue;
        }
        /*-------------------------------------------------------------------
          Received a CONFIG_REJ. Use the defaults stored in the
          call specific table.
        -------------------------------------------------------------------*/
        while (length) 
        {
          /*-----------------------------------------------------------------
            Get the option type.
          -----------------------------------------------------------------*/
          opt_type = *protocol_pkt_ptr++;
          DS_MSG1(MSG_LEGACY_HIGH, "CONFIG_REJ: Opt Type : %d", opt_type);

          /*-----------------------------------------------------------------
            Get the option length.
          -----------------------------------------------------------------*/
          opt_len = *protocol_pkt_ptr++;

          /*-----------------------------------------------------------------
            If remaining IPCP pkt Length is less than the option length, 
            it is an error. Return.
          -----------------------------------------------------------------*/
          if (length < opt_len) 
          {
            local_err = DS_LOCAL_ERR_VAL_17;
            break;
          }

          /*-----------------------------------------------------------------
            If option length is zero, the packet is bad. Return to avoid  
            entering an infinite loop.
          -----------------------------------------------------------------*/
          if (opt_len == 0) 
          {
            local_err = DS_LOCAL_ERR_VAL_18;
            break;
          }

          length -= opt_len;
          protocol_pkt_ptr += (opt_len - 2);
        }
        break;

      default:
        protocol_pkt_ptr += length;
    } /* switch (type) */
  } /* while (protocol_pkt_len) */

  if(local_err != DS_LOCAL_ERR_VAL_NONE)
  {
    DS_ERROR_LOG_1("ds_pdn_decode_ipcp_pkt failed with error =d",
    	              local_err);
    return FALSE;
  }

  return TRUE;
} /* ds_pdn_decode_ipcp_pkt() */

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
  const byte         *protocol_pkt_ptr, /* Pointer to the buffer to extract dns    */
                                /* addresses from                          */
  int                 protocol_pkt_len,  /* Length of packet to extract dsn addresses from*/
  ds_ipcp4_options_s *ipv4cp_options,  /* ipv4cp options received */
  uint32             *num_ipv4_dns       /*Number of V4 dns addresses already processed   */
)
{
  uint32 dns_addr = 0;
  
  if(protocol_pkt_ptr == NULL || num_ipv4_dns == NULL || 
     ipv4cp_options == NULL || protocol_pkt_len != DS_IPV4_ADDR_LEN)
  {
    DS_ERROR_LOG_0("Invalid arguments while decoding ipcp options");
    return;
  }
  
  /*-----------------------------------------------------------------------
    Process only two DNS addressess for IPV4. If more than two DNS 
    addresses are received in context accept message, store two addresses
    and ignore rest. 
  -----------------------------------------------------------------------*/    
  if(*num_ipv4_dns == 0)
  {
    if( (dns_addr = get32((byte *)protocol_pkt_ptr)) != 0 )
    {
      ipv4cp_options->primary_dns = dns_addr;
      DS_MSG1(MSG_LEGACY_HIGH,
      	       "Decoded primary v4 DNS address %x",dns_addr);
    }
    (*num_ipv4_dns)++;
  }
  else if(*num_ipv4_dns == 1)
  {
    if( (dns_addr = get32((byte *)protocol_pkt_ptr)) != 0 )
    {
      ipv4cp_options->secondary_dns = dns_addr;
      DS_MSG1(MSG_LEGACY_HIGH,
      	       "Decoded secondary v4 DNS address %x",dns_addr);
    }
    (*num_ipv4_dns)++;
  }    
  else 
  {
    DS_ERROR_LOG_0("More Than Two IPv4 DNS addresses present, Ignoring");
  }
}/* ds_pdn_decode_ipv4_pkt */

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
  const byte          *protocol_pkt_ptr, /* Pointer to the buffer to extract dns    */
                                /* addresses from                          */
  int                  protocol_pkt_len,  /* Length of packet to extract dsn addresses from*/
  ds_ipv6cp_options_s *ipv6cp_options,  /* ipv6cp options received */
  int                 *num_ipv6_dns       /*Number of V6 dns addresses already processed   */
)
{
  if(protocol_pkt_len != DS_IPV6_ADDR_LEN)
  {
    DS_ERROR_LOG_0("Wrong IPV6 DNS Address");
    return;
  }

  /*-----------------------------------------------------------------------
    Process only two DNS addressess for IPV6. If more than two DNS 
    addresses are received in context accept message, store two addresses
    and ignore rest. 
  -----------------------------------------------------------------------*/    
  if(*num_ipv6_dns == 0)
  {
    memscpy(ipv6cp_options->primary_dns.ps_s6_addr, sizeof(struct ps_in6_addr), 
            protocol_pkt_ptr, sizeof(struct ps_in6_addr) );
    (*num_ipv6_dns)++;
  }
  else if(*num_ipv6_dns == 1)
  {
    memscpy(ipv6cp_options->secondary_dns.ps_s6_addr, sizeof(struct ps_in6_addr),
            protocol_pkt_ptr, sizeof(struct ps_in6_addr) );
    (*num_ipv6_dns)++;
  }    
  else 
  {
    DS_ERROR_LOG_0("More Thans Two IPv6 DNS addresses present, Ignoring");
  }

  return;
}/* ds_pdn_decode_ipv6_pkt */

/*===========================================================================
FUNCTION DS3GPP_DECODE_OPERATOR_RESERVED_PCO_PKT

DESCRIPTION
  This function decodes the operator reserved PCO received from the network.
  
PARAMETERS   
    protocol_pkt_ptr  - Pointer to the buffer to extract ipcp options from
    protocol_pkt_len  - Length of packet to extract ipcp options from
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
)
{
  uint16 mcc;
  uint16 mnc;
  ds_pdn_mgr_inst_type_s             *v4_inst = NULL;
  ds_pdn_mgr_inst_type_s             *v6_inst = NULL;
  ds_pdn_mgr_op_reserved_pco_type     operator_reserved_pco_info={0};
  ds_pdn_mgr_modem_reserved_pco_type  modem_op_reserved_pco_info={0};
  ds_pdn_event_info_u_type            event_info;  

  memset(&event_info, 0, sizeof(ds_pdn_event_info_u_type));

  /*----------------------------------------------------------------------- 
    Currently the num of pco_containers is always 1. There could be a
    future case where num_pco_containers can increase. We will need to make
    appropriate changes to allocate memory then.
  -------------------------------------------------------------------------*/  
  //uint16 num_pco_containers = 1;

  if( protocol_pkt_ptr == NULL ||
  	   protocol_pkt_len < MCC_MNC_LENGTH || 
  	   protocol_pkt_len > MAX_PROTOCOL_ID_CONTENT )
  {
    return FALSE;
  }

  v4_inst = pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV4];
  v6_inst = pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV6];

/* Lower Byte Set  is 0x0F UPPER_BYTE_SET is 0xF0;
   Decode mcc mnc */ 
/*  3 Octets: mcc/mnc as part of Pco contents:"
     protocol_pkt_ptr[0]:-   mcc_digit_2  mcc_digit_1
     protocol_pkt_ptr[1]:-   mnc_digit_3  mcc_digit_3
     protocol_pkt_ptr[2]:-   mnc_digit_2  mnc_digit_1
*/    
  mcc = (protocol_pkt_ptr[1] & LOWER_BYTE_SET)                  // mcc_3
      + (((protocol_pkt_ptr[0] & UPPER_BYTE_SET) >> 4) * 10)    // mcc_2
      + (protocol_pkt_ptr[0] & LOWER_BYTE_SET) * 100;           // mcc_1

  /*  
     Here mnc_digit_3 is checked for 0x0F; if so we interpret
    mnc as two digit and set mnc_includes_pcs_digi to FALSE else TRUE
  */

  if((protocol_pkt_ptr[1] & UPPER_BYTE_SET) !=UPPER_BYTE_SET)
  {
    mnc = ((protocol_pkt_ptr[1] & UPPER_BYTE_SET) >> 4)            // mnc_3
           + (((protocol_pkt_ptr[2] & UPPER_BYTE_SET) >> 4) * 10)   // mnc_2
           + ((protocol_pkt_ptr[2] & LOWER_BYTE_SET) * 100);    // mnc_1
  }
  else
  {
    mnc = ((protocol_pkt_ptr[2] & UPPER_BYTE_SET) >> 4)      // mnc_2
           + ((protocol_pkt_ptr[2] & LOWER_BYTE_SET) * 10);    // mnc_1

  }
   
  protocol_pkt_len -= MCC_MNC_LENGTH;

  if((protocol_pkt_ptr[1] & UPPER_BYTE_SET) !=UPPER_BYTE_SET)
  {
    operator_reserved_pco_info.mnc_includes_pcs_digit = TRUE;
  }

  operator_reserved_pco_info.mcc = mcc;
  operator_reserved_pco_info.mnc = mnc;

  DS_MSG2(MSG_LEGACY_HIGH, "MCC %d, MNC %d", mcc, mnc);

  operator_reserved_pco_info.container_id = protocol_id;
  operator_reserved_pco_info.app_specific_info_len=protocol_pkt_len;

  memscpy(operator_reserved_pco_info.app_specific_info,
          sizeof(operator_reserved_pco_info.app_specific_info),
          protocol_pkt_ptr+MCC_MNC_LENGTH,
          protocol_pkt_len);

  /*-------------------------------------------------------------------------- 
    Currently for modem_op_pco we only have a single PCO entry, When this
    extends allocate modem_op_reserved_pco_info from heap and a logic change
    will be needed
  ----------------------------------------------------------------------------*/
  modem_op_reserved_pco_info.num_pcos = 1;
  modem_op_reserved_pco_info.pco_info = operator_reserved_pco_info;

  if((v4_inst != NULL || v6_inst != NULL) && 
  	  (profile_pco_id_configured))
  {
    if(memcmp(&pdn_mgr_p->op_reserved_pco,
              &operator_reserved_pco_info,
              sizeof(ds_pdn_mgr_op_reserved_pco_type)) != 0)
    {
      pdn_mgr_p->op_reserved_pco = operator_reserved_pco_info;
      event_info.extended_ip_config_change_mask = DS_EXT_IP_CFG_MASK_OP_RES_PCO;

      if(v4_inst != NULL)
      {
        ds_fwk_notify_events(pdn_mgr_p->fw_index,
                      ds_pdn_mgr_convert_pdn_inst_to_fw_ip_type(v4_inst->type),
                      PDN_EXTENDED_IP_CONFIG_EX_EV,
                      event_info);      
      }
      
      if(v6_inst != NULL)
      {
        ds_fwk_notify_events(pdn_mgr_p->fw_index,
                      ds_pdn_mgr_convert_pdn_inst_to_fw_ip_type(v6_inst->type),
                      PDN_EXTENDED_IP_CONFIG_EX_EV,
                      event_info);      
      }
    }
  }
  else
  {
    pdn_mgr_p->modem_reserved_pco = modem_op_reserved_pco_info;
  }

  return TRUE;
} /* ds_pdn_decode_operator_reserved_pco_pkt() */
