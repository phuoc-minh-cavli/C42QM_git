#ifndef GTP_PROTOCOLS_H
#define GTP_PROTOCOLS_H

/*===========================================================================

  GTP PROTOCOLS Header File

  DESCRIPTION
  This header file contains definitions of data structure and interfaces
  for GTP protocols.

  Copyright (c) 2018 - 2020 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2018 - 2019 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2018 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

===========================================================================*/

#include <loc_util_log.h>
#include "comdef.h"    /* Definition for basic types and macros */
#include "GtpProtocolUtil.h"
#include "gps_extended_c.h"
#include <vector>
#include <string>

using namespace std;
namespace izat_gtpwwan{
/* Constants */

#define GTP_PROT_INVALID_VALUE            -1
#ifdef FEATURE_LOCATION_MCS
#define GTP_XTA_MAX_XTA_LEN 3000  /* max length of XTA PDU */
#else
#define GTP_XTA_MAX_XTA_LEN 500  /* max length of XTA PDU */
#endif

enum requestedDataTypes_Mask
{
    MOBILE_STATUS_DATA   = (1 << 0),
    SINGLE_CELL_REQUEST  = (1 << 1),
    SINGLE_CELL_UPLOAD   = (1 << 2),
    CELL_LISTS_REQUEST   = (1 << 3),
    CELL_LISTS_UPLOAD    = (1 << 4),
    WWAN_SCAN_POS_REQUEST = (1 << 5),
    WWAN_SCAN_CS_UPLOAD = (1 << 6),
    OPTIONAL_FIELDS_MASK_2 = (1<< 7)

};

typedef struct
{
    /* save inserter snapshot for updating fields later */
    gtp_xta_bit_inserter zInserterClientInfoDataLen;
    gtp_xta_bit_inserter zInserterClientRequestDataLen;
    gtp_xta_bit_inserter zInserterLocationInfoMask;
} GtpProtocolCoreBufferVars;

/* Types */
typedef struct
{
    uint32  ulLength;
    uint16  usXtProtocolVersion;
    uint8   ucXtMessageType;
} gtp_xta_core_xt_msg_hdr_type;

typedef struct
{
    /* states */
    uint8   ucEnginePreviousState;    /* protocol engine previous state */
    uint8   ucEngineCurrentState;     /* protocol engine current state */
} GtpProtocolState;

typedef struct
{
    /* message buffer for request and response */
    uint8   *p_zMsgBuf;               /* pointer to dynamically allocated message buffer */
    uint32  ulMsgBufSizeAlloc;        /* size of message buffer allocated */
    int32   ilMsgBufStart;            /* index to first byte of message buffer used */
    int32   ilMsgBufEnd;              /* index to last byte of message buffer used */
} GtpProtocolBuffer;

typedef struct
{
    /* message buffer for XTA PDU */
    uint8   *p_zMsgBuf;              /* pointer to dynamically allocated message buffer */
    uint32  ulMsgBufSizeAlloc;       /* size of message buffer allocated */
    int32   ilMsgBufStart;           /* index to first byte of message buffer used */
    int32   ilMsgBufEnd;             /* index to last byte of message buffer used */

    /* bit inserter and extractor */
    gtp_xta_bit_inserter    zBitInserter;  /* used for encoding XTA */
} GtpProtocolCoreEncodeBuffer;

typedef struct
{
    /* message buffer for XTA PDU */
    uint8   *p_zMsgBuf;              /* pointer to dynamically allocated message buffer */
    uint32  ulMsgBufSizeAlloc;       /* size of message buffer allocated */
    int32   ilMsgBufStart;           /* index to first byte of message buffer used */
    int32   ilMsgBufEnd;             /* index to last byte of message buffer used */

    gtp_xta_bit_extractor   zBitExtractor; /* used for decoding XTA */
} GtpProtocolCoreDecodeBuffer;

class GtpWwanClient;
extern uint32_t gtp_crc32(void *msg, uint32_t msgsize);
class GtpProtocol{
    public:
        GtpWwanClient* mGtpWwanClient;

        GtpProtocolCoreEncodeBuffer* gtp_prot_core_enc_buff;

        GtpProtocolCoreDecodeBuffer* gtp_prot_core_dec_buff;

        GtpProtocolCoreBufferVars* gtp_prot_core_buff_var;

        /*constructor and destructor */

        GtpProtocol(GtpWwanClient* client);

        ~GtpProtocol();
	uint8 mRequestedFieldMask;
        string mTestConfig;

        /*===========================================================================
          FUNCTION gtp_prot_init

          DESCRIPTION
          This function initializes the protocol engine (only done once on GTP task
          startup).

          PARAMETERS
          None.

          DEPENDENCIES
          None.

          RETURN VALUE
          True - Allocation succeed.
          FALSE - Allocation failed.

          SIDE EFFECTS
          Initializes all protocol engine static variables.

        ===========================================================================*/
        boolean initGtpProtocol( void );

        /*===========================================================================

          ENCODER FUNCTIONS

        ===========================================================================*/



        /*===========================================================================
          FUNCTION gtp_prot_encode

          DESCRIPTION
          This function initializes the encoder and encodes the data.

          DEPENDENCIES
          None.

          RETURN VALUE
          TRUE - Initialization succeed.
          FALSE - Initialization failed.

          SIDE EFFECTS
          None.

          ===========================================================================*/
        boolean gtp_prot_encode( void );
        /*===========================================================================
          FUNCTION gtp_prot_shutdown_encode

          DESCRIPTION
          This function initializes the encoder and encodes the data

          DEPENDENCIES
          None.

          RETURN VALUE
          True - Initialization succeed.
          FALSE - Initialization failed.

          SIDE EFFECTS
          None.

          ===========================================================================*/
        void gtp_prot_shutdown_encode( void);


        /*===========================================================================

          DECODER FUNCTIONS

          ===========================================================================*/


        /*===========================================================================
          FUNCTION gtp_prot_decode

          DESCRIPTION
          This function initializes the decode and decodes the data provide

          PARAMETERS
          data....(IN).point to buffer which need to be decoded
          length...(IN) length of buffer which need to be decoded

          DEPENDENCIES
          None.

          RETURN VALUE
          TRUE - Initialization succeed.
          FALSE - Initialization failed.

          SIDE EFFECTS
          None.

          ===========================================================================*/
        boolean gtp_prot_decode( void* data,uint32 length);
        /*===========================================================================
          FUNCTION gtp_prot_shutdown_decode

          DESCRIPTION
          This function initializes the encoder and encodes the data

          DEPENDENCIES
          None.

          RETURN VALUE
          True - Initialization succeed.
          FALSE - Initialization failed.

          SIDE EFFECTS
          None.

          ===========================================================================*/
        void gtp_prot_shutdown_decode( void);

        /*===========================================================================
          FUNCTION gtp_prot_decode_test

          DESCRIPTION
          This function does loop back testing for encoded data.

          PARAMETERS
          data....(IN).point to buffer which need to be decoded
          length...(IN) length of buffer which need to be decoded

          DEPENDENCIES
          None.

          RETURN VALUE
          TRUE - Initialization succeed.
          FALSE - Initialization failed.
          SIDE EFFECTS
          None.
        ===========================================================================*/
        #ifdef FEATURE_TEST_DECODE
        boolean gtp_prot_decode_test( void* data,uint32* length);
        #endif
    private:
        /*===========================================================================
          FUNCTION gtp_xta_initialize_encode

          DESCRIPTION
          This function initializes the XTA encoder.

          PARAMETERS
          p_ucData  - (IN) Pointer to buffer in heap where encoded data is stored.
          ulLength  - (IN) Max size of buffer.

          DEPENDENCIES
          None.

          RETURN VALUE
          TRUE - Initialization succeed.
          FALSE - Initialization failed.

          SIDE EFFECTS
          None.

          ===========================================================================*/
        boolean gtp_xta_initialize_encode( uint8 *p_ucData, uint32 ulLength );


        /*===========================================================================
          FUNCTION gtp_xta_encode_client_info

          DESCRIPTION
          This function encodes the client info into the XTA message.

          DEPENDENCIES
          None.

          RETURN VALUE
          TRUE - Encode succeed.
          FALSE - Encode failed.

          SIDE EFFECTS
          None.

        ===========================================================================*/
        boolean gtp_xta_encode_client_info( void );

        /*===========================================================================
          FUNCTION gtp_xta_encode_client_request

          DESCRIPTION
          This function encodes the fdcl list and observed cell data which we receive from the modem 
          into the XTA message.

          DEPENDENCIES
          None.

          RETURN VALUE
          TRUE - Encode succeed.
          FALSE - Encode failed.

          SIDE EFFECTS
          None.

        ===========================================================================*/
        boolean gtp_xta_encode_client_request( void );

        /*===========================================================================
          FUNCTION gtp_xta_final_encode

          DESCRIPTION
          This function encodes total length of XTA message.

          DEPENDENCIES
          None.

          RETURN VALUE
          TRUE - Encode succeed.
          FALSE - Encode failed.

          SIDE EFFECTS
          None.

          ===========================================================================*/

        boolean gtp_xta_final_encode( void );
        boolean gtp_xta_encode_lte_bands();
		#ifdef FEATURE_LOCATION_MCS
		boolean gtp_xta_encode_generic_cell_id (LocCellId* locellid);
		boolean gtp_xta_encode_wwan_scan_list(WwanScanList* wwanScanlist);
		boolean gtp_xta_encode_channel_info(ChannelInfo* channelinfo);
		boolean gtp_xta_encode_scan_OBS_list();
		boolean gtp_xta_encode_AIT_info(uint8_t AirInterfaceType,
    			GenericTimeStamp* AITScanStartTime, 
    			uint8_t numOfChannels, ChannelInfo* channelinfo);
		boolean gtp_xta_encode_Time_Location(uint8_t locationMask,
                                       LocBsBestPosition locBsBestPosition, 
                                       GenericTimeStamp timestamp);
		boolean gtp_xta_encode_generic_time(GenericTimeStamp* timestamp);
		bool gtp_prot_decode_scanCsConfig();
		bool gtp_prot_decode_scanCsActiveConfig();
		bool gtp_prot_decode_scanPosConfig();
		bool gtp_prot_decode_scanAssistance();
		boolean gtp_prot_decode_generic_time();
		#endif
		

        /*===========================================================================

          DECODER FUNCTIONS

          ===========================================================================*/


        /*===========================================================================
          FUNCTION gtp_xta_initialize_decode

          DESCRIPTION
          This function initializes the XTA decoder.

          PARAMETERS
          p_ucData    - (IN) Pointer to buffer with XTA PDU to decode.
          ulLength    - (IN) Length of XTA PDU in bytes.

          DEPENDENCIES
          None.

          RETURN VALUE
          TRUE - Initialization succeed.
          FALSE - Initialization failed.

          SIDE EFFECTS
          None.

          ===========================================================================*/
        boolean gtp_xta_initialize_decode( uint8 *p_ucData, uint32 ulLength );

        /*===========================================================================
          FUNCTION gtp_xta_decode_header

          DESCRIPTION
          This function decodes the XT_MESSAGE_HEADER which we encode, this function is called 
          when we are decoding our encode data.

          PARAMETERS
          p_zBitExtractor  - (IN) Pointer to bit extractor structure.
          p_zHdr		   - (OUT) Pointer to XT message header structure which will
          be filled from data decoded from extractor.

          DEPENDENCIES
          None.

          RETURN VALUE
          TRUE - Decode succeed.
          FALSE - Decode failed.

          SIDE EFFECTS
          None.

          ===========================================================================*/
         #ifdef FEATURE_TEST_DECODE
        boolean gtp_xta_decode_header( gtp_xta_bit_extractor *p_zBitExtractor,
                gtp_xta_core_xt_msg_hdr_type *p_zHdr);
         #endif

};
}//end of namespace
#endif /* GTP_PROTOCOLS_H */

