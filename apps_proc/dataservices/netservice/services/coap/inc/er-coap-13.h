/******************************************************************************

  @file    er-coap-13.h

  ---------------------------------------------------------------------------

  Copyright (c) 2016-2022 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------

 *****************************************************************************/

/*
 * Copyright (c) 2013, Institute for Pervasive Computing, ETH Zurich
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 */

/**
 * \file
 *      An implementation of the Constrained Application Protocol (draft 12)
 * \author
 *      Matthias Kovatsch <kovatsch@inf.ethz.ch>
 * \contributors
 *    David Navarro, Intel Corporation - Adapt to usage in liblwm2m
 */


#ifndef COAP_13_H_
#define COAP_13_H_

#include <stdint.h>
#include <stddef.h> /* for size_t */
#ifndef MOB_TX
#include "tx_api.h"
#else
#include "offtarget_stubs.h"
#endif

/*
 * The maximum buffer size that is provided for resource responses and must be respected due to the limited IP buffer.
 * Larger data must be handled by the resource and will be sent chunk-wise through a TCP stream or CoAP blocks.
 */
#ifndef REST_MAX_CHUNK_SIZE
#define REST_MAX_CHUNK_SIZE     512
#endif

#define COAP_DEFAULT_MAX_AGE                 60
#define COAP_RESPONSE_TIMEOUT                2
#define COAP_MAX_RETRANSMIT                  4
#define COAP_ACK_RANDOM_FACTOR               1.5
#define COAP_MAX_LATENCY                     100


#define COAP_PROCESSING_DELAY                COAP_RESPONSE_TIMEOUT

#define COAP_MAX_TRANSMIT_WAIT               ((COAP_RESPONSE_TIMEOUT * ( (1 << (COAP_MAX_RETRANSMIT + 1) ) - 1) * COAP_ACK_RANDOM_FACTOR))
#define COAP_MAX_TRANSMIT_SPAN               ((COAP_RESPONSE_TIMEOUT * ( (1 << COAP_MAX_RETRANSMIT) - 1) * COAP_ACK_RANDOM_FACTOR))
#define COAP_EXCHANGE_LIFETIME               (COAP_MAX_TRANSMIT_SPAN + (2 * COAP_MAX_LATENCY) + COAP_PROCESSING_DELAY)


#define COAP_HEADER_LEN                      4 /* | version:0x03 type:0x0C tkl:0xF0 | code | mid:0x00FF | mid:0xFF00 | */
#define COAP_ETAG_LEN                        8 /* The maximum number of bytes for the ETag */
#define COAP_TOKEN_LEN                       8 /* The maximum number of bytes for the Token */
#define COAP_MAX_ACCEPT_NUM                  2 /* The maximum number of accept preferences to parse/store */

#define COAP_MAX_OPTION_HEADER_LEN           5

#define COAP_HEADER_VERSION_MASK             0xC0
#define COAP_HEADER_VERSION_POSITION         6
#define COAP_HEADER_TYPE_MASK                0x30
#define COAP_HEADER_TYPE_POSITION            4
#define COAP_HEADER_TOKEN_LEN_MASK           0x0F
#define COAP_HEADER_TOKEN_LEN_POSITION       0

#define COAP_HEADER_OPTION_DELTA_MASK        0xF0
#define COAP_HEADER_OPTION_DELTA_RESERVED    0x0F
#define COAP_HEADER_OPTION_SHORT_LENGTH_MASK 0x0F
#define COAP_HEADER_OPTION_LENGTH_RESERVED   0x0F


/*
 * Conservative size limit, as not all options have to be set at the same time.
 */
#ifndef COAP_MAX_HEADER_SIZE
/*                            Hdr CoT Age  Tag              Obs  Tok               Blo strings */
#define COAP_MAX_HEADER_SIZE  (4 + 3 + 5 + 1+COAP_ETAG_LEN + 3 + 1+COAP_TOKEN_LEN + 4 + 30) /* 70 */
#endif /* COAP_MAX_HEADER_SIZE */

#define COAP_MAX_PACKET_SIZE  (COAP_MAX_HEADER_SIZE + REST_MAX_CHUNK_SIZE)
/*                                        0/14          48 for IPv6 (28 for IPv4) */
#if COAP_MAX_PACKET_SIZE > (UIP_BUFSIZE - UIP_LLH_LEN - UIP_IPUDPH_LEN)
//#error "UIP_CONF_BUFFER_SIZE too small for REST_MAX_CHUNK_SIZE"
#endif

#define REST_MAX_CHUNK_SIZE 512
#define BLOCK_MIN_SIZE 16 
#define BLOCK_MAX_SIZE 1024


/* Bitmap for set options */
enum { OPTION_MAP_SIZE = sizeof(uint8_t) * 8 };
#define SET_OPTION(packet, opt) ((packet)->options[opt / OPTION_MAP_SIZE] |= 1 << (opt % OPTION_MAP_SIZE))
#define IS_OPTION(packet, opt) ((packet)->options[opt / OPTION_MAP_SIZE] & (1 << (opt % OPTION_MAP_SIZE)))

#ifndef MIN
#define MIN(a, b) ((a) < (b)? (a) : (b))
#endif /* MIN */

#define IsPowerOfTwo(x) ((x != 0) && ((x & (x - 1)) == 0))
#define Is_response(code)(code >= QAPI_CREATED_2_01 && code <= QAPI_PROXYING_NOT_SUPPORTED_5_05)

#define IS_INVALID_BLOCKWISE_REQ(pkt , btype , bsize)((!(IsPowerOfTwo(bsize) && bsize >= BLOCK_MIN_SIZE && bsize <= BLOCK_MAX_SIZE))|| \
                                                     (btype == QAPI_COAP_BLOCK1 && !((pkt)->code == QAPI_COAP_POST || ((pkt)->code == QAPI_COAP_PUT))) || \
                                                     ((btype == QAPI_COAP_BLOCK2 && (pkt)->code != QAPI_CONTENT_2_05)))

#define COAP_IS_INVALIDE_PACKETS(pkt) (((pkt)->code >= QAPI_COAP_GET  && (pkt)->code <= QAPI_COAP_DELETE)  && \
                                      (((pkt)->type == QAPI_COAP_TYPE_ACK || (pkt)->type == QAPI_COAP_TYPE_RST)) || \
                                      (Is_response((pkt)->code) && (pkt)->type == QAPI_COAP_TYPE_RST) || \
                                      ((pkt)->code == QAPI_COAP_EMPTY && (pkt)->type == QAPI_COAP_TYPE_NON))

/*
 * Empty Message:
 *	A message with a Code of 0.00; neither a request nor a response.
 *	An Empty message only contains the 4-byte header.
 *
 * Tocken Lengths:
 *  9-15 are reserved, MUST NOT be sent, and MUST be processed as a
 *  message format error.
*/

#define IS_PARSED_COAP_PACKETS_INVALID(pkt) ((pkt.version != 1)                  || \
                                               ((pkt.code == QAPI_COAP_EMPTY) && (pkt.buffer_len > COAP_HEADER_LEN)) || \
                                               (pkt.token_len > COAP_TOKEN_LEN))



#define SET_COAP_OPTIONS(s_pkt , d_pkt , option)(IS_OPTION(s_pkt , option) ? SET_OPTION(d_pkt, option) : 0)


/* CoAP message types */
typedef enum {
  COAP_TYPE_CON, /* confirmables */
  COAP_TYPE_NON, /* non-confirmables */
  COAP_TYPE_ACK, /* acknowledgements */
  COAP_TYPE_RST  /* reset */
} coap_message_type_t;

/* CoAP request method codes */
typedef enum {
  COAP_EMPTY = 0,
  COAP_GET,
  COAP_POST,
  COAP_PUT,
  COAP_DELETE
} coap_method_t;

/* CoAP response codes */
typedef enum {
  NO_ERROR = 0,
  IGNORE = 1,

  CREATED_2_01 = 65,                    /* CREATED */
  DELETED_2_02 = 66,                    /* DELETED */
  VALID_2_03 = 67,                      /* NOT_MODIFIED */
  CHANGED_2_04 = 68,                    /* CHANGED */
  CONTENT_2_05 = 69,                    /* OK */

  BAD_REQUEST_4_00 = 128,               /* BAD_REQUEST */
  UNAUTHORIZED_4_01 = 129,              /* UNAUTHORIZED */
  BAD_OPTION_4_02 = 130,                /* BAD_OPTION */
  FORBIDDEN_4_03 = 131,                 /* FORBIDDEN */
  NOT_FOUND_4_04 = 132,                 /* NOT_FOUND */
  METHOD_NOT_ALLOWED_4_05 = 133,        /* METHOD_NOT_ALLOWED */
  NOT_ACCEPTABLE_4_06 = 134,            /* NOT_ACCEPTABLE */
  PRECONDITION_FAILED_4_12 = 140,       /* BAD_REQUEST */
  REQUEST_ENTITY_TOO_LARGE_4_13 = 141,  /* REQUEST_ENTITY_TOO_LARGE */
  UNSUPPORTED_MEDIA_TYPE_4_15 = 143,    /* UNSUPPORTED_MEDIA_TYPE */

  INTERNAL_SERVER_ERROR_5_00 = 160,     /* INTERNAL_SERVER_ERROR */
  NOT_IMPLEMENTED_5_01 = 161,           /* NOT_IMPLEMENTED */
  BAD_GATEWAY_5_02 = 162,               /* BAD_GATEWAY */
  SERVICE_UNAVAILABLE_5_03 = 163,       /* SERVICE_UNAVAILABLE */
  GATEWAY_TIMEOUT_5_04 = 164,           /* GATEWAY_TIMEOUT */
  PROXYING_NOT_SUPPORTED_5_05 = 165,    /* PROXYING_NOT_SUPPORTED */

  /* Erbium errors */
  MEMORY_ALLOCATION_ERROR = 192,
  PACKET_SERIALIZATION_ERROR,

  /* Erbium hooks */
  MANUAL_RESPONSE

} coap_status_t;

/* CoAP header options */
typedef enum {
  COAP_OPTION_IF_MATCH = 1,       /* 0-8 B */
  COAP_OPTION_URI_HOST = 3,       /* 1-255 B */
  COAP_OPTION_ETAG = 4,           /* 1-8 B */
  COAP_OPTION_IF_NONE_MATCH = 5,  /* 0 B */
  COAP_OPTION_OBSERVE = 6,        /* 0-3 B */
  COAP_OPTION_URI_PORT = 7,       /* 0-2 B */
  COAP_OPTION_LOCATION_PATH = 8,  /* 0-255 B */
  COAP_OPTION_URI_PATH = 11,      /* 0-255 B */
  COAP_OPTION_CONTENT_TYPE = 12,  /* 0-2 B */
  COAP_OPTION_MAX_AGE = 14,       /* 0-4 B */
  COAP_OPTION_URI_QUERY = 15,     /* 0-270 B */
  COAP_OPTION_ACCEPT = 17,        /* 0-2 B */
  COAP_OPTION_TOKEN = 19,         /* 1-8 B */
  COAP_OPTION_LOCATION_QUERY = 20, /* 1-270 B */
  COAP_OPTION_BLOCK2 = 23,        /* 1-3 B */
  COAP_OPTION_BLOCK1 = 27,        /* 1-3 B */
  COAP_OPTION_SIZE = 28,          /* 0-4 B */
  COAP_OPTION_PROXY_URI = 35,     /* 1-270 B */
  COAP_OPTION_SIZE1 = 60,          /* 0-4 B */
  COAP_OPTION_MAX                 /* MAX COAP OPTION VALUE */
} coap_option_t;

/* CoAP Content-Types */
typedef enum {
  TEXT_PLAIN = 0,
  TEXT_XML = 1, /* Indented types are not in the initial registry. */
  TEXT_CSV = 2,
  TEXT_HTML = 3,
  IMAGE_GIF = 21,
  IMAGE_JPEG = 22,
  IMAGE_PNG = 23,
  IMAGE_TIFF = 24,
  AUDIO_RAW = 25,
  VIDEO_RAW = 26,
  APPLICATION_LINK_FORMAT = 40,
  APPLICATION_XML = 41,
  APPLICATION_OCTET_STREAM = 42,
  APPLICATION_RDF_XML = 43,
  APPLICATION_SOAP_XML = 44,
  APPLICATION_ATOM_XML = 45,
  APPLICATION_XMPP_XML = 46,
  APPLICATION_EXI = 47,
  APPLICATION_FASTINFOSET = 48,
  APPLICATION_SOAP_FASTINFOSET = 49,
  APPLICATION_JSON = 50,
  APPLICATION_X_OBIX_BINARY = 51,
  // Fix for Issue: Due to memory optimization this variable was considered as 1 byt, hence storage of TLV format support was failing
  M2M_TLV = 11542,
  M2M_JSON = 11543,
} coap_content_type_t;

typedef struct _multi_option_t {
  struct _multi_option_t *next;
  uint8_t is_static;
  uint8_t len;
  uint8_t *data;
} multi_option_t;

typedef struct coap_ext_msg_option_s
{
  struct coap_ext_msg_option_s *next;
  uint16_t option_num;
  uint16_t len;
  uint8_t  *val;
}coap_ext_msg_option_t;

/* Parsed message struct */
typedef struct {
  uint8_t *buffer; /* pointer to CoAP header / incoming packet buffer / memory to serialize packet */

  uint8_t version;
  coap_message_type_t type;
  uint8_t code;
  uint16_t mid;

  uint8_t options[(COAP_OPTION_MAX -1) / OPTION_MAP_SIZE + 1]; /* Bitmap to check if option is set */
                                                         /* Update COAP_OPTION_MAX with max option value */
  coap_content_type_t content_type; /* Parse options once and store; allows setting options in random order  */
  uint32_t max_age;
  size_t proxy_uri_len;
  uint8_t *proxy_uri;
  uint8_t etag_len;
  uint8_t etag[COAP_ETAG_LEN];
  size_t uri_host_len;
  uint8_t *uri_host;
  multi_option_t *location_path;
  uint16_t uri_port;
  size_t location_query_len;
  uint8_t *location_query;
  multi_option_t *uri_path;
  uint32_t observe;
  uint8_t token_len;
  uint8_t token[COAP_TOKEN_LEN];
  uint8_t accept_num;
  coap_content_type_t accept[COAP_MAX_ACCEPT_NUM];
  uint8_t if_match_len;
  uint8_t if_match[COAP_ETAG_LEN];
  uint32_t block2_num;
  uint8_t block2_more;
  uint16_t block2_size;
  uint32_t block2_offset;
  uint32_t block1_num;
  uint8_t block1_more;
  uint16_t block1_size;
  uint32_t block1_offset;
  uint32_t size;
  uint32_t size1;
  multi_option_t *uri_query;
  uint8_t if_none_match;

  uint16_t payload_len;
  uint8_t *payload;

  uint16_t buffer_len;

} coap_packet_t;

typedef struct coap_opt_num_s
{
  struct coap_opt_num_s *next;
  uint16_t       opt_num;
}coap_opt_num_t;

typedef struct {
  uint8_t option_count;
  coap_opt_num_t *options;
  uint8_t ext_option_count;
  coap_ext_msg_option_t *ext_option;
}coap_packet_ext_opt_t;

typedef struct {
  coap_packet_t         coap_pkt;
  coap_packet_ext_opt_t coap_ext_opt;
}coap_packet_ext_t;

/* Option format serialization*/
#define COAP_SERIALIZE_INT_OPTION(number, field, text)  \
  if (IS_OPTION(&coap_pkt->coap_pkt, number)) { \
    PRINTF(text" [%u]",coap_pkt->coap_pkt.field); \
    option += coap_serialize_int_option(number, current_number, option, coap_pkt->coap_pkt.field); \
    current_number = number; \
  }
#define COAP_SERIALIZE_BYTE_OPTION(number, field, text)      \
  if (IS_OPTION(&coap_pkt->coap_pkt, number)) { \
    PRINTF(text" %u [0x%02X%02X%02X%02X%02X%02X%02X%02X]", coap_pkt->coap_pkt.field##_len, \
        coap_pkt->coap_pkt.field[0], \
        coap_pkt->coap_pkt.field[1], \
        coap_pkt->coap_pkt.field[2], \
        coap_pkt->coap_pkt.field[3], \
        coap_pkt->coap_pkt.field[4], \
        coap_pkt->coap_pkt.field[5], \
        coap_pkt->coap_pkt.field[6], \
        coap_pkt->coap_pkt.field[7] \
        ); /*FIXME always prints 8 bytes */ \
    option += coap_serialize_array_option(number, current_number, option, coap_pkt->coap_pkt.field, coap_pkt->coap_pkt.field##_len, '\0'); \
    current_number = number; \
  }
#define COAP_SERIALIZE_STRING_OPTION(number, field, splitter, text)      \
  if (IS_OPTION(&coap_pkt->coap_pkt, number)) { \
    PRINTF(text" [%.*s]", &coap_pkt->coap_pkt.field##_len, coap_pkt->coap_pkt.field); \
    option += coap_serialize_array_option(number, current_number, option, (uint8_t *) coap_pkt->coap_pkt.field, coap_pkt->coap_pkt.field##_len, splitter); \
    current_number = number; \
  }
#define COAP_SERIALIZE_MULTI_OPTION(number, field, text)      \
  if (IS_OPTION(&coap_pkt->coap_pkt, number)) { \
    PRINTF(text); \
    option += coap_serialize_multi_option(number, current_number, option, coap_pkt->coap_pkt.field); \
    current_number = number; \
  }
#define COAP_SERIALIZE_ACCEPT_OPTION(number, field, text)  \
  if (IS_OPTION(&coap_pkt->coap_pkt, number)) { \
    int i; \
    for (i=0; i<coap_pkt->coap_pkt.field##_num; ++i) \
    { \
      PRINTF(text" [%u]", coap_pkt->coap_pkt.field[i]); \
      option += coap_serialize_int_option(number, current_number, option, coap_pkt->coap_pkt.field[i]); \
      current_number = number; \
    } \
  }
#define COAP_SERIALIZE_BLOCK_OPTION(number, field, text)      \
  if (IS_OPTION(&coap_pkt->coap_pkt, number)) \
{ \
  uint32_t block = 0;\
  PRINTF(text" [%lu%s (%u B/blk)]", coap_pkt->coap_pkt.field##_num, coap_pkt->coap_pkt.field##_more ? "+" : "", coap_pkt->coap_pkt.field##_size); \
  block = coap_pkt->coap_pkt.field##_num << 4; \
  if (coap_pkt->coap_pkt.field##_more) block |= 0x8; \
  block |= 0xF & coap_log_2(coap_pkt->coap_pkt.field##_size/16); \
  PRINTF(text" encoded: 0x%lX", block); \
  option += coap_serialize_int_option(number, current_number, option, block); \
  current_number = number; \
}

#define COAP_SERIALIZE_EXT_OPTION(number)      \
  { \
    option += coap_serialize_ext_option(number, current_number, option, &coap_pkt->coap_ext_opt, ext_current); \
    current_number = number; \
    ext_current++; \
  }

#define COAP_LIST_ADD(H,N) coap_list_add((coap_list_t *)H, (coap_list_t *)N);
#define COAP_LIST_RM(H,I,N) coap_list_remove((coap_list_t *)H, I, (coap_list_t **)N);
#define COAP_LIST_FREE(H, N) coap_list_free((coap_list_t *)H, (void *)N);
#define COAP_LIST_FIND(H,I) coap_list_find((coap_list_t *)H, I)


typedef struct _coap_list_t
{
  struct _coap_list_t * next;
  uint16_t    id;
} coap_list_t;

coap_list_t * coap_list_add(coap_list_t * head, coap_list_t * node);
coap_list_t * coap_list_remove(coap_list_t * head, uint16_t id, coap_list_t ** nodeP);
void coap_list_free(coap_list_t *H, void * N);
coap_list_t * coap_list_find(coap_list_t * head, uint16_t id);


/* To store error code and human-readable payload */
extern const char *coap_error_message;


void coap_init_message(void *packet, coap_message_type_t type, uint8_t code, uint16_t mid);
size_t coap_serialize_get_size(void *packet);
size_t coap_serialize_message(void *packet, uint8_t *buffer);
coap_status_t coap_parse_message(void *request, uint8_t *data, uint16_t data_len, void *umem);
void coap_free_header(void *packet, void * umem);

char * coap_get_multi_option_as_string(multi_option_t * option , int * length);
char * coap_get_multi_option_as_string_for_uri_query(multi_option_t * option , int * length);

void coap_add_multi_option(multi_option_t **dst, uint8_t *option, size_t option_len, uint8_t is_static,void * umem);
void free_multi_option(multi_option_t *dst, void * uinfo);

/*-----------------------------------------------------------------------------------*/

int coap_set_status_code(void *packet, uint8_t code);

int coap_get_header_content_type(void *packet , int * content_type);
int coap_set_header_content_type(void *packet, unsigned int content_type);

int coap_get_header_accept(void *packet, const uint16_t **accept);
int coap_set_header_accept(void *packet, uint16_t accept);

int coap_get_header_max_age(void *packet, uint32_t *age);
int coap_set_header_max_age(void *packet, uint32_t age);

int coap_get_header_etag(void *packet, const uint8_t **etag);
int coap_set_header_etag(void *packet, const uint8_t *etag, size_t etag_len);



int coap_get_header_token(void *packet, const uint8_t **token);
int coap_set_header_token(void *packet, const uint8_t *token, size_t token_len);

int coap_get_header_uri_host(void *packet, const char **host); /* In-place string might not be 0-terminated. */
int coap_set_header_uri_host(void *packet, const char *host);

int coap_get_header_uri_path(void *packet, char **path); /* In-place string might not be 0-terminated. */
int coap_set_header_uri_path(void *packet, const char *path);

int coap_get_header_uri_query(void *packet, char **query); /* In-place string might not be 0-terminated. */
int coap_set_header_uri_query(void *packet, const char *query);

int coap_get_header_location_path(void *packet, char **path); /* In-place string might not be 0-terminated. */
int coap_set_header_location_path(void *packet, const char *path); /* Also splits optional query into Location-Query option. */

int coap_get_header_location_query(void *packet, const char **query); /* In-place string might not be 0-terminated. */
int coap_set_header_location_query(void *packet, char *query);

int coap_get_header_observe(void *packet, uint32_t *observe);
int coap_set_header_observe(void *packet, uint32_t observe);

int coap_get_header_block2(void *packet, uint32_t *num, uint8_t *more, uint16_t *size, uint32_t *offset);
int coap_set_header_block2(void *packet, uint32_t num, uint8_t more, uint16_t size);

int coap_get_header_block1(void *packet, uint32_t *num, uint8_t *more, uint16_t *size, uint32_t *offset);
int coap_set_header_block1(void *packet, uint32_t num, uint8_t more, uint16_t size);

int coap_get_header_size(void *packet, uint32_t *size);
int coap_set_header_size(void *packet, uint32_t size);

int coap_get_header_size1(void *packet, uint32_t *size);
int coap_set_header_size1(void *packet, uint32_t size);

int coap_get_payload(void *packet, const uint8_t **payload);
int coap_set_payload(void *packet, const void *payload, size_t length);

int coap_set_header_ext(void *packet, uint16_t option_num, const char *val,
                         uint16_t len, void * umem);
                         
int coap_get_header_ext(void *packet, coap_ext_msg_option_t **option , uint16_t opt_num);

int coap_get_all_header_ext(void *packet, coap_ext_msg_option_t **option);
void coap_set_option(coap_packet_ext_t *packet, uint16_t option_number,
                            void *umem);

int coap_copy_header(coap_packet_ext_t *dst_pkt , coap_packet_ext_t *src_pkt, void *umem);
int coap_set_header_pkt(coap_packet_t * src_pkt , coap_packet_t * dest_pkt);
boolean coap_is_standard_option(uint16_t option_number);


#endif /* COAP_13_H_ */
