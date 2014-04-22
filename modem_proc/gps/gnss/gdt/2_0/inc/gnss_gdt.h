#ifndef GNSS_GDT_H
#define GNSS_GDT_H
/*=============================================================================

                             GDT Header file

GENERAL DESCRIPTION
  This file contains the structure definitions and function prototypes 
  relevant for GDT (Generic Data Transfer) in GNSS module.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  

Copyright (c) 2014-2015 Qualcomm Atheros, Inc. All Rights Reserved. 
  Qualcomm Atheros Confidential and Proprietary.

Copyright (c) 2015 - 2017 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

=============================================================================*/


/*=============================================================================

                           EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/gdt/2_0/inc/gnss_gdt.h#1 $
  $DateTime: 2020/07/21 21:23:44 $
  $Author: pwbldsvc $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2014-07-25   ah  Initial version
  

=============================================================================*/

/*=============================================================================

                                INCLUDE FILES

=============================================================================*/

#include "customer.h"
#include "comdef.h"
#include "rfs_types.h"
#include "rfs_api.h"
#include "rfs_config.h"
#include "rfs_errno.h"
#include "rfs_fcntl.h"
#include "log.h"

#ifdef __cplusplus
extern "C"
{
#endif


/*=============================================================================

                     DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/


/*=============================================================================

       Constants

=============================================================================*/
  /* Byte 1 - Record status */
#define C_TLE_GDT_RECORD_STATUS_ENCRYPTED                (0x01)
#define C_TLE_GDT_RECORD_STATUS_LITTLE_ENDIAN_DNLD       (0x02)
  
  /* Byte 2 - GDT service ID */
#define C_TLE_GDT_RECORD_SERVICE_ID_UPLD             (0x01)
#define C_TLE_GDT_RECORD_SERVICE_ID_DNLD             (0x02)
#define C_CSM_GDT_RECORD_SERVICE_ID_UPLD             (0x03)

  /* Byte 3&4 - Msg */
#define C_TLE_GDT_RECORD_CLIENT_INFO_UPLD            (0x0001)
#define C_TLE_GDT_RECORD_MOBILE_STATUS_UPLD          (0x0002)
#define C_TLE_GDT_RECORD_CELL_ARRAY_UPLD             (0x0003)
#define C_TLE_GDT_RECORD_UPLD_END          (C_TLE_GDT_RECORD_CELL_ARRAY_UPLD)
  
#define C_TLE_GDT_RECORD_PART_HEADER_DNLD            (0x0001)
#define C_TLE_GDT_RECORD_SNA_PART_DNLD               (0x0002)
#define C_TLE_GDT_RECORD_BSA_PART_DNLD               (0x0003)
#define C_TLE_GDT_RECORD_PART_MGMT_DNLD              (0x0004)
#define C_TLE_GDT_RECORD_CLIENT_DOWNLDED_DATA_DNLD   (0x0005)
#define C_TLE_GDT_QMI_CLIENT_DNLD_DATA               (0x0006)
#define C_TLE_GDT_RECORD_END          (C_TLE_GDT_RECORD_CELL_ARRAY_UPLD + C_TLE_GDT_QMI_CLIENT_DNLD_DATA)

#define C_CSM_GDT_RECORD_RFS_SCHEMA                  (0x0001)
#define C_CSM_GDT_RECORD_END               (C_CSM_GDT_RECORD_RFS_SCHEMA)
  
#define C_TLE_GDT_BLOB_CONTENT_CLIENT_INFO           (1)
#define C_TLE_GDT_BLOB_CONTENT_MOBILE_STATUS         (2)
#define C_TLE_GDT_BLOB_CONTENT_CELL_ARRAY            (3)
#define C_TLE_GDT_BLOB_CONTENT_PART_HEADER           (4)
#define C_TLE_GDT_BLOB_CONTENT_SNA_PARTITION         (5)
#define C_TLE_GDT_BLOB_CONTENT_BSA_PARTITION         (6)
#define C_TLE_GDT_BLOB_CONTENT_PARTITION_MGMT        (7)
#define C_TLE_GDT_BLOB_CONTENT_CLIENT_DNLD_DATA      (8)
#define C_TLE_GDT_BLOB_CONTENT_QMI_CLIENT_DNLD_DATA  (9)


/*=============================================================================

       Macros

=============================================================================*/
#define C_GNSS_GDT_SIZE_OF_TAG  (4)
#define C_GNSS_GDT_SIZE_OF_LENGTH (4)

/* Swap bytes in 16 bit value.  */
#define gdt_bswap_16(x) ((((x) & 0xFF00) >> 8) | (((x) & 0x00FF) <<  8))

/* Swap bytes in 32 bit value.  */
#define gdt_bswap_32(x) \
     ((((x) & 0xFF000000) >> 24) | (((x) & 0x00FF0000) >>  8) |     \
      (((x) & 0x0000FF00) <<  8) | (((x) & 0x000000FF) << 24))
      

/* Swap bytes in 32 bit value.  */
#define gdt_bswap_64(x) \
     ((((x) & 0xFF00000000000000) >> 56) | (((x) & 0x00FF000000000000) >>  40) |     \
      (((x) & 0x0000FF0000000000) >> 24) | (((x) & 0x000000FF00000000) >>  8) |     \
      (((x) & 0x00000000FF000000) << 8) | (((x) & 0x0000000000FF0000) <<  24) |     \
      (((x) & 0x000000000000FF00) <<  40) | (((x) & 0x00000000000000FF) << 56))


#define C_GNSS_GDT_MAX_BUF_SIZE (500*1024)

#define C_GNSS_GDT_FILEPATH_LEN (255)
#define C_GDT_BIG_ENDIAN        0  /* big endian architecture */
#define C_GDT_LITTLE_ENDIAN     1  /* little endian architecture */

#define C_GDT_BLOB_MAX_DATA_SIZE (1000)
#define C_GDT_BLOB_FORMAT_BINARY (0)
#define C_GDT_BLOB_FORMAT_ASCII  (1)
#define C_GDT_PB_RAW_BLOB_SIZE   (1000)

/*=============================================================================

       Typedefs

=============================================================================*/
typedef unsigned char *gdt_MemHandle;

typedef enum
{
  e_GDT_SUCCESS = 0,
  e_GDT_FAIL,
  e_GDT_MAX = 0xFFFF
} e_GDTReturnCodesVal;
typedef uint16 e_GDTReturnCodesType;

/* Defines the service which uses GDT */
typedef enum
{
  e_GDT_SERVICE_INVALID = -1,
  e_GDT_SERVICE_GTP_WWAN = 0, /* TDP UL */
  e_GDT_SERVICE_GTP_WWAN_DL = 1,  /* TDP DL */
  e_GDT_SERVICE_GTP_CSM_UL = 2,
  /* Update last service */
  e_GDT_SERVICE_LAST = e_GDT_SERVICE_GTP_CSM_UL,
  e_GDT_SERVICE_MAX = 0xFFFF
} e_GDTServiceIDVal;
typedef int16 e_GDTServiceIDType;


/* Defines the service type which uses GDT */
typedef enum
{
  e_GDT_SERVICE_TYPE_INVALID = -1,
  e_GDT_SERVICE_TYPE_SENDER = 0,
  e_GDT_SERVICE_TYPE_RECEIVER = 1,
  /* Update last service */
  e_GDT_SERVICE_TYPE_LAST = e_GDT_SERVICE_TYPE_RECEIVER,
  e_GDT_SERVICE_TYPE_MAX = 0xFFFF
} e_GDTServiceVal;
typedef int16 e_GDTServiceType;


typedef struct
{
  e_GDTServiceIDType e_ServiceId;
  uint32             q_SessId; /* Session ID for this service */
  char               c_DataPath[C_GNSS_GDT_FILEPATH_LEN];
} gdt_SessionType;

/* Defines the request type which a service is requesting from GDT */
typedef enum
{
  e_GDT_UPLOAD_BEGIN = 0,
  e_GDT_UPLOAD_END,
  e_GDT_DOWNLOAD_BEGIN,
  e_GDT_DOWNLOAD_MAX = 0xFFFF
} e_GDTServiceRequestVal;
typedef uint16 e_GDTServiceRequestType;
/* Defines the response to an access request (gdt_UploadBegin()) */
typedef enum
{
  e_GDT_ACCESS_ALLOWED = 0,       /* AP confirmed, that access to the shared data
                                 location is allowed */
  e_GDT_ACCESS_FAILED,        /* Any type of GDT access error */
  e_GDT_ACCESS_NOT_ALLOWED,   /* AP does not allow access to the shared data
                                 location */
  e_GDT_ACCESS_MAX = 0xFFFF
} e_GDTResponseVal;
typedef uint16 e_GDTResponseType;

/* Defines the format in which GDT expects the data to be read */
typedef enum
{
  e_GDT_FORMAT_TLV = 0,
  e_GDT_FORMAT_MAX = 0xFFFF
} e_GDTFormatVal;
typedef uint16 e_GDTFormatType;


/* Defines the status of a record which was sent to GDT for upload */
typedef enum
{
  e_GDT_ACK_SUCCESS = 0, /* The sent data is accepeted by GDT */
  e_GDT_ACK_FAILED,  /* The sent data was not accepted by GDT */
  e_GDT_ACK_MAX = 0xFFFF
} e_GDTAckVal;
typedef uint16 e_GDTAckType;

typedef struct
{
  gdt_SessionType   z_SessionInfo;
  e_GDTResponseType e_StartStatus;
  int             l_Client_handle;
  uint32          q_TransactionId;
} gdt_StartResponseType;

typedef struct
{
  gdt_SessionType       z_SessionInfo;
  e_GDTReturnCodesType  e_OpenStatus;
} gdt_OpenResponseType;

typedef struct
{
  gdt_SessionType       z_SessionInfo;
  e_GDTReturnCodesType  e_CloseStatus;
  int32  l_ClientHandle;
  uint32 q_TransactionId;
} gdt_CloseResponseType;


typedef struct
{
  gdt_SessionType z_SessionInfo;
  e_GDTAckType    e_EndStatus;
  int             l_Client_handle;
  uint32          q_TransactionId;
} gdt_EndResponseType;

typedef struct
{
  gdt_SessionType z_SessionInfo;
  e_GDTAckType    e_EndStatus;
} gdt_SendAckResponseType;

typedef struct
{
  gdt_SessionType z_SessionInfo;
  gdt_MemHandle p_ByteBuffer;
  uint32        q_DataSize;
} gdt_SendBlobType;

typedef struct
{
  gdt_SessionType z_SessionInfo;
  gdt_MemHandle p_ByteBuffer;
  uint32        q_BufSize;
  uint32        q_DataSize;
  e_GDTFormatType e_Format;
} gdt_ReceiveBlobType;

typedef struct
{
  unsigned char c_RawData[C_GDT_PB_RAW_BLOB_SIZE];
  uint32        q_DataSize;
  uint16        w_TotalPacketCount;
  uint16        w_PacketIndex;
} gdt_PbRawDataType;

/* Every client of GDT needs to provide its required parameter for the call back API
   for gdt_UploadBegin() in this data type */
typedef union
{
  gdt_StartResponseType *pz_StartResponse;
} gdt_StartArgumentType;

/* Every client of GDT needs to provide its required parameter for the call back API
   for gdt_UploadSend() in this data type*/
typedef union
{
  gdt_SendAckResponseType *pz_AckResponse;
} gdt_SendAckArgumentType;

/* Every client of GDT needs to provide its required parameter for the call back API
   for gdt_UploadEnd() in this data type */
typedef union
{
  gdt_EndResponseType *pz_EndResponse;
} gdt_EndArgumentType;

/* Every client of GDT needs to provide its required parameter for the call back API
   for gdt_UploadOpen() in this data type */
typedef union
{
  gdt_OpenResponseType *pz_OpenResponse;
} gdt_OpenArgumentType;

/* Every client of GDT needs to provide its required parameter for the call back API
   for gdt_UploadClose() in this data type */
typedef union
{
  gdt_CloseResponseType *pz_CloseResponse;
} gdt_CloseArgumentType;

typedef struct
{
  gdt_SessionType z_SessInfo;
  gdt_MemHandle   z_GdtBuffer;
  uint32          q_GdtBufferSize; /* This the total buffer size */
  uint32          q_MsgLength;     /* Number of bytes in the buffer - unused for reading TLV */
  uint8           u_ArchEndian;    /* C_GDT_BIG_ENDIAN or C_GDT_LITTLE_ENDIAN*/
} gdt_InterfaceType;

/* Defines the status of the complete transfer */
typedef enum
{
  e_GDT_TRANSFER_INCOMPLETE = 0, /* More data expected */
  e_GDT_TRANSFER_COMPLETE,  /* All data expected to be transferred is complete */
  e_GDT_TRANSFER_MAX = 0xFFFF
} e_GDTTransferStatusType;

typedef struct
{
  gdt_SessionType   z_SessionInfo;
  e_GDTResponseType e_StartStatus;
  uint32            q_FilePathLen;
  char              RcvFilePath[C_GNSS_GDT_FILEPATH_LEN];
  int               l_ClientHandle; 
  uint32            q_TransactionId;
} gdt_ReceiveReadyResponseType;

typedef union
{
  gdt_ReceiveReadyResponseType *pz_ReadyResponse;
} gdt_ReceiveReadyArgumentType;

typedef struct
{
  gdt_ReceiveBlobType  z_ReadBlob;
  e_GDTReturnCodesType e_ReceiveDataStatus;
} gdt_ReceiveDataResponseType;

typedef union
{
  gdt_ReceiveDataResponseType *pz_ReceiveDataResponse;
} gdt_ReceiveDataArgumentType;

typedef union
{
  gdt_EndResponseType *pz_EndResponse;
} gdt_ReceiveEndArgumentType;

/* Packed data structures */
#ifdef FEATURE_CGPS_PLAYBACK
#pragma pack(push)
#pragma pack(1)
#endif

typedef PACKED struct PACKED_POST
{
  uint32  q_SrcTask;     /* Tthread ID of the client task */

  uint16   w_ServiceId;   /* 0 : Invalid
                             1 : GDT_SERVICE_GTP_WWAN 
                             2: GDT_SERVICE_GTP_WWAN_DL */

  uint32  q_SessionId;  /* GDT Session ID  */

  uint16 w_ContentType; /* 0:Invalid
                           1: TDP Client information
                           2: TDP Mobile status data
                           3: TDP cell array 
                           4: Partition file Header
                           5: SNA Partition 
                           6: BSA Partition 
                           7: Partition Management 
                           8: ClientDownloadedData */

  uint8  u_Format;         /* Output format:
                              0: Binary
                              1: ASCII */

  uint16 w_Part;           /* If data is larger than a single log packet it will be
                              logged in multiple log instances (PART of TOTAL_PARTS). */
  uint16 w_TotalParts;
  uint32 q_Length;         /* Number of valid bytes in data field */
  unsigned char sz_Data[C_GDT_BLOB_MAX_DATA_SIZE];
} gdt_PackedDataBlobType;


typedef PACKED struct PACKED_POST
{
  /* log header.  DO NOT ACCESS DIRECTLY! */
  log_hdr_type           z_Hdr;
  uint8                  u_Version;
  gdt_PackedDataBlobType z_Blob;
} gdt_DataBlobDmLogType;

#ifdef FEATURE_CGPS_PLAYBACK
#pragma pack(pop)
#endif
/*===========================================================================

                     DATA TYPE DEFINITIONS FOR MODULE

                        !!!!!!! ATTENTION !!!!!!!!
Size/Order of data fields in the following data types should not
be changed as they are synchronized with external tool software.
===========================================================================*/


/* SendBegin Event */
/* EVENT_GNSS_GDT_SESS_START_REQ */
typedef PACKED struct PACKED_POST
{
  uint16 w_ServiceId;
  uint32 q_SessionId;
} gdt_SessStartReqEventType;

/* EVENT_GNSS_GDT_SESS_START_RESP */
typedef PACKED struct PACKED_POST
{
  uint16 w_ServiceId;
  uint32 q_SessionId;
  uint16 w_GdtResp;
  int32 l_ClientHandle;
  uint32 q_TransactionId;
} gdt_SessStartRespEventType;

/* SendData Event */
/* EVENT_GNSS_GDT_SEND_REQ */
typedef PACKED struct PACKED_POST
{
  uint16 w_ServiceId;
  uint32 q_SessionId;
  uint32 q_DataSize;
} gdt_SendReqEventType;

/* EVENT_GNSS_GDT_SEND_RESP */
typedef PACKED struct PACKED_POST
{
  uint16 w_ServiceId;
  uint32 q_SessionId;
  uint16 w_GdtResp;
} gdt_SendRespEventType;

/* SendEnd Event */
/* EVENT_GNSS_GDT_SESS_END_REQ */
typedef PACKED struct PACKED_POST
{
  uint16 w_ServiceId;
  uint32 q_SessionId;
  uint16 w_GdtResp;
} gdt_SessEndReqEventType;

/* EVENT_GNSS_GDT_SESS_END_RESP */
typedef PACKED struct PACKED_POST
{
  uint16 w_ServiceId;
  uint32 q_SessionId;
  uint16 w_GdtAckResp;
  int32 l_ClientHandle;
  uint32 q_TransactionId;
} gdt_SessEndRespEventType;

/* SendOpen Event */
/* EVENT_GNSS_GDT_SESS_OPEN_REQ */
typedef PACKED struct PACKED_POST
{
  uint16 w_ServiceId;
  uint32 q_SessionId;
} gdt_SessOpenReqEventType;

/* SendClose Event */
/* EVENT_GNSS_GDT_SESS_CLOSE_REQ */
typedef PACKED struct PACKED_POST
{
  uint16 w_ServiceId;
  uint32 q_SessionId;
} gdt_SessCloseReqEventType;



/*=============================================================================

       Function prototypes

=============================================================================*/


/*===========================================================================
FUNCTION gdt_GetEndianArchitecture

DESCRIPTION
  This function returns the endianness of the modem processor architecture

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  C_GDT_BIG_ENDIAN   : If MP has a big endian architecture
  C_GDT_LITTLE_ENDIAN: If MP has a little endian architecture

SIDE EFFECTS
  None.

===========================================================================*/
uint8 gdt_GetEndianArchitecture(void);


/*===========================================================================
FUNCTION gdt_Deinit

DESCRIPTION
  This function de-initializes GDT and frees up all registered buffers

  WARNING - This function should *NOT* be used on target because it can be very
            dangerous if the buffer is freed incorrectly.
 
PARAMETERS 
 
DEPENDENCIES

RETURN VALUE 

SIDE EFFECTS

===========================================================================*/
void gdt_Deinit();

/*===========================================================================
FUNCTION gdt_SendBegin

DESCRIPTION
  This function starts a data transposrt session between a module in MP and
  the corresponding module in AP.
 
PARAMETERS 
 
  pz_Session - Points to the session information
  (input)
 
DEPENDENCIES

RETURN VALUE 
  e_GDT_SUCCESS: GDT has validated the request, and sent the access request
                 to AP to get the permission
 
  e_GDT_FAIL: Otherwise
 
SIDE EFFECTS

===========================================================================*/
e_GDTReturnCodesType gdt_SendBegin(gdt_SessionType *const pz_Session);

/*===========================================================================
FUNCTION gdt_SendData

DESCRIPTION
  This function receives a chunk of data that needs to be transported to AP.
  Different chuncks of data will be accumulated and will be sent to AP at once,
  when the sender calls gdp_SessionEnd().
 
PARAMETERS 
 
  cpz_GdtIface - Points to the GDT interface
  (input)

DEPENDENCIES

RETURN VALUE 
  e_GDT_SUCCESS: GDT has accepted the data to be uploaded.
 
   Note: This does not confirm that data is transferred.
 
SIDE EFFECTS

===========================================================================*/
e_GDTReturnCodesType gdt_SendData(const gdt_InterfaceType *cpz_GdtIface);


/*===========================================================================
FUNCTION gdt_SendEnd

DESCRIPTION
  This function finalizes the data transmission session between MP and AP.

DEPENDENCIES 
 
PARAMETERS 
  cpz_Session - Points to the session information from MP to AP
  (input)

  e_Status - The final status of this session
  (input)

  b_ForceCleanup - If TRUE, force cleanup of remaining session information
  (input)


RETURN VALUE 
  e_GDT_SUCCESS: GDT has received the final upload session status from AP 
 
  e_GDT_FAIL  - There is an internal error in GDT
 
SIDE EFFECTS

===========================================================================*/
e_GDTReturnCodesType gdt_SendEnd(const gdt_SessionType *const cpz_Session,
                                 e_GDTResponseType e_Status, boolean b_ForceCleanup);


/*===========================================================================
FUNCTION gdt_SendOpen

DESCRIPTION
  This function initializes data transmission session from a client to GDT.

DEPENDENCIES 
 
PARAMETERS 
  cpz_Session - Points to the session information from MP to AP
  (input)

RETURN VALUE 
  e_GDT_SUCCESS: GDT has successfuly initialized to start data transmission
 
  e_GDT_FAIL  - Otherwise
 
SIDE EFFECTS

===========================================================================*/
e_GDTReturnCodesType gdt_SendOpen(const gdt_SessionType *const cpz_Session);

/*===========================================================================
FUNCTION gdt_SendClose

DESCRIPTION
  This function finalizes data transmission session from a client to GDT.

DEPENDENCIES 
 
PARAMETERS 
  cpz_Session - Points to the session information from MP to AP
  (input)

RETURN VALUE 
  e_GDT_SUCCESS: GDT has successfuly finalized data transmission
 
  e_GDT_FAIL  - Otherwise
 
SIDE EFFECTS

===========================================================================*/
e_GDTReturnCodesType gdt_SendClose(const gdt_SessionType *const cpz_Session);

/*===========================================================================
FUNCTION gdt_ReceiveBegin

DESCRIPTION
  This function initializes data transmission session from a client to GDT.  The MP client calls this
  to initiate the receiving of data upon receiving notification that the data is available.

DEPENDENCIES 
 
PARAMETERS 
  pz_Session - Points to the session information from MP to AP
  (input)

RETURN VALUE 
  e_GDT_SUCCESS: GDT has successfuly initialized to start data transmission
 
  e_GDT_FAIL  - Otherwise
 
SIDE EFFECTS

===========================================================================*/
e_GDTReturnCodesType gdt_ReceiveBegin(gdt_SessionType *const pz_Session);

/*===========================================================================
FUNCTION gdt_ReceiveOpen

DESCRIPTION
  This function initializes data transmission session from a client to GDT.

DEPENDENCIES 
 
PARAMETERS 
  cpz_Session - Points to the session information from MP to AP
  (input)

RETURN VALUE 
  e_GDT_SUCCESS: GDT has successfuly initialized to start data transmission
 
  e_GDT_FAIL  - Otherwise
 
SIDE EFFECTS

===========================================================================*/
e_GDTReturnCodesType gdt_ReceiveOpen(const gdt_SessionType *const cpz_Session);

/*===========================================================================
FUNCTION gdt_ReceiveData

DESCRIPTION
  This function receives a chunk of data that needs to be supplied to MP.
  The data will be provided in the format the client prefers it (eg. TLV).
 
PARAMETERS 
 
  cpz_GdtIface - Points to the GDT interface
  (input)

  e_Format - Expected GDT file parsing format
  (input)

RETURN VALUE 
  e_GDT_SUCCESS: GDT has successfuly initialized to start data transmission
 
  e_GDT_FAIL  - Otherwise
 
SIDE EFFECTS

===========================================================================*/
e_GDTReturnCodesType gdt_ReceiveData(const gdt_InterfaceType *cpz_GdtIface, e_GDTFormatType e_Format);

/*===========================================================================
FUNCTION gdt_ReceiveClose

DESCRIPTION
  This function closes the current data transmission session from a client to GDT.  The MP client calls this to
  terminate interactions with the downloaded file.

DEPENDENCIES 
 
PARAMETERS 
  cpz_Session - Points to the session information from MP to AP
  (input)

  e_Status - Status of receive session
  (input)

RETURN VALUE 
  e_GDT_SUCCESS: GDT has successfuly finalized current data transmission
 
  e_GDT_FAIL  - Otherwise
 
SIDE EFFECTS

===========================================================================*/
e_GDTReturnCodesType gdt_ReceiveClose(const gdt_SessionType *const cpz_Session,
                                      e_GDTTransferStatusType e_Status);

/*===========================================================================
FUNCTION gdt_ReceiveEnd

DESCRIPTION
  This function finalizes the data transmission session between MP and AP.  This is the final message sent by
  the MP client in the callflow to notify AP the data transfer session is finished.

DEPENDENCIES 
 
PARAMETERS 
  cpz_Session - Points to the session information from MP to AP
  (input)

  e_Status - The final status of this session
  (input)

  b_ForceCleanup - If TRUE, force cleanup of remaining session information
  (input)


RETURN VALUE 
  e_GDT_SUCCESS: GDT has received the final download session status from MP 
 
  e_GDT_FAIL  - There is an internal error in GDT
 
SIDE EFFECTS

===========================================================================*/
e_GDTReturnCodesType gdt_ReceiveEnd(const gdt_SessionType *const cpz_Session,
                                    e_GDTResponseType e_Status, boolean b_ForceCleanup);

/*===========================================================================
FUNCTION gdt_BufAlloc

DESCRIPTION
  This function allocates a requested buffer from the requesting user service.  Buffer will be capped at a set
  maximum (currently 500 Kb).  At this time, each service is only allowed to hold a single buffer.  Due to this
  limitation, any subsequent calls to gdt_BufAlloc will return with an error if the service already holds a
  buffer.

DEPENDENCIES 
 
PARAMETERS 
  cpz_Session - Points to the session information from MP to AP
  (input)
  
  q_ReqBufferSize - The requested buffer size in bytes
  (input)
  
  cp_ByteBuffer - Local handle to the allocated buffer
  (output)
  
  q_BufferSize - Size of the allocated buffer
  (output)


RETURN VALUE 
  e_GDT_SUCCESS: GDT has successfuly allocated a buffer
 
  e_GDT_FAIL  - Otherwise
 
SIDE EFFECTS

===========================================================================*/
e_GDTReturnCodesType gdt_BufAlloc(const gdt_SessionType *const cpz_Session,
                                  uint32 q_ReqBufferSize,
                                  gdt_MemHandle *const p_ByteBuffer,
                                  uint32 *const q_BufferSize);


/*===========================================================================
FUNCTION gdt_BufFree

DESCRIPTION
  This function frees the buffer associated with the service

DEPENDENCIES 
 
PARAMETERS 
  cpz_Session - Points to the session information from MP to AP
  (input)

  cp_ByteBuffer - Local handle to the allocated buffer
  (input)
  

RETURN VALUE 
  e_GDT_SUCCESS: GDT has successfuly allocated a buffer
 
  e_GDT_FAIL  - Otherwise
 
SIDE EFFECTS

===========================================================================*/
e_GDTReturnCodesType gdt_BufFree(const gdt_SessionType *const cpz_Session,
                                 gdt_MemHandle p_ByteBuffer);


/*===========================================================================
FUNCTION gdt_RcvdSendBeginResponse

DESCRIPTION
  This function is the callback for the upload begin request. GDT needs
  to forward the response to the corresponding module API
 
PARAMETERS 
 
  z_Response - The recieved response
  (input)
 
DEPENDENCIES

RETURN VALUE 
  e_GDT_SUCCESS: GDT has validated the response, and sent the response to the user
                 API
 
  e_GDT_FAIL:   Otherwise

SIDE EFFECTS

===========================================================================*/
e_GDTReturnCodesType gdt_RcvdSendBeginResponse(gdt_StartArgumentType z_Response);

/*===========================================================================
FUNCTION gdt_RcvdSendEndResponse

DESCRIPTION
  This function is the callback for the upload end message. GDT needs
  to forward the response to the corresponding module API
 
PARAMETERS 
 
  z_Response - The recieved response
  (input)
 
DEPENDENCIES

RETURN VALUE 
  e_GDT_SUCCESS: GDT has validated the response, and sent the response to the 
                 user API
 
  e_GDT_FAIL:   Otherwise

SIDE EFFECTS

===========================================================================*/
e_GDTReturnCodesType gdt_RcvdSendEndResponse(gdt_EndArgumentType z_Response);

/*===========================================================================
FUNCTION gdt_RcvdSendAckResponse

DESCRIPTION
  This function is the callback for the send ACK message. GDT needs
  to forward the response to the corresponding module API. 
 
PARAMETERS 
 
  cpz_Session - Points to the session information from MP to AP
  (input)

  z_Response - The recieved response
  (input)
 
DEPENDENCIES

RETURN VALUE 
  e_GDT_SUCCESS: GDT has validated the response, and sent the response to
                 the user API
 
  e_GDT_FAIL: Otherwise

SIDE EFFECTS

===========================================================================*/
e_GDTReturnCodesType gdt_RcvdSendAckResponse(gdt_SendAckArgumentType z_Response);

/*===========================================================================
FUNCTION gdt_ReceiveReadyResponse

DESCRIPTION
  This function is the callback for getting receive ready from AP. GDT needs
  to forward the response to the corresponding module API. 
 
PARAMETERS 
  
  z_Response - The received response
  (input)
 
DEPENDENCIES

RETURN VALUE 
  e_GDT_SUCCESS: GDT has validated the response, and sent the response to
                 the user API
 
  e_GDT_FAIL: Otherwise

SIDE EFFECTS

===========================================================================*/
e_GDTReturnCodesType gdt_RcvdReceiveReadyResponse(gdt_ReceiveReadyArgumentType z_Response);

/*===========================================================================
FUNCTION gdt_ReceiveReadyResponse

DESCRIPTION
  This function is the callback for getting the ack for an MP receive done from AP. GDT needs
  to forward the response to the corresponding module API. 
 
PARAMETERS 
 
  z_Response - The received response
  (input)
 
DEPENDENCIES

RETURN VALUE 
  e_GDT_SUCCESS: GDT has validated the response, and sent the response to
                 the user API
 
  e_GDT_FAIL: Otherwise

SIDE EFFECTS

===========================================================================*/
e_GDTReturnCodesType gdt_RcvdReceiveDataResponse(gdt_ReceiveDataArgumentType z_Response);

/*===========================================================================
FUNCTION gdt_RcvdReceiveCloseResponse

DESCRIPTION
  This function is the callback for getting the ack for an MP receive close from AP. GDT needs
  to forward the response to the corresponding module API. 
 
PARAMETERS 

  z_Response - The received response
  (input)
 
DEPENDENCIES

RETURN VALUE 
  e_GDT_SUCCESS: GDT has validated the response, and sent the response to
                 the user API
 
  e_GDT_FAIL: Otherwise

SIDE EFFECTS

===========================================================================*/
e_GDTReturnCodesType gdt_RcvdReceiveCloseResponse(gdt_CloseArgumentType z_Response);


/*===========================================================================
FUNCTION gdt_RcvdReceiveEndResponse

DESCRIPTION
  This function is the callback for getting the ack for an MP receive end from AP. GDT needs
  to forward the response to the corresponding module API. 
 
PARAMETERS 
 
  z_Response - The received response
  (input)
 
DEPENDENCIES

RETURN VALUE 
  e_GDT_SUCCESS: GDT has validated the response, and sent the response to
                 the user API
 
  e_GDT_FAIL: Otherwise

SIDE EFFECTS

===========================================================================*/
e_GDTReturnCodesType gdt_RcvdReceiveEndResponse(gdt_ReceiveEndArgumentType z_Response);


/*===========================================================================

FUNCTION gdt_ReportEventSessionStartReq

DESCRIPTION
  Report event: GDT Session initiated

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void gdt_ReportEventSessionStartReq(const gdt_SessionType *const cpz_Session);

/*===========================================================================

FUNCTION gdt_ReportEventSessionStartResp

DESCRIPTION
  Report event: GDT Session initiated response

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void gdt_ReportEventSessionStartResp(gdt_StartArgumentType z_Response);

/*===========================================================================

FUNCTION gdt_ReportEventSendReq

DESCRIPTION
  Report event: GDT Session send data request

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void gdt_ReportEventSendReq(const gdt_SessionType *const cpz_Session, uint32 q_DataSize);

/*===========================================================================

FUNCTION gdt_ReportEventSendResp

DESCRIPTION
  Report event: GDT Session send data response

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void gdt_ReportEventSendResp(gdt_SendAckArgumentType z_Response);

/*===========================================================================

FUNCTION gdt_ReportEventSessionEndReq

DESCRIPTION
  Report event: GDT Session send end req

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void gdt_ReportEventSessionEndReq(const gdt_SessionType *const cpz_Session, uint16 w_Status);

/*===========================================================================

FUNCTION gdt_ReportEventSessionEndResp

DESCRIPTION
  Report event: GDT Session end response

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void gdt_ReportEventSessionEndResp(gdt_EndArgumentType z_Response);

/*===========================================================================

FUNCTION gdt_ReportEventSessionOpenReq

DESCRIPTION
  Report event: GDT Session open

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void gdt_ReportEventSessionOpenReq(const gdt_SessionType *const cpz_Session);

/*===========================================================================

FUNCTION gdt_ReportEventSessionCloseReq

DESCRIPTION
  Report event: GDT Session close

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void gdt_ReportEventSessionCloseReq(const gdt_SessionType *const cpz_Session);


/*===========================================================================

FUNCTION
  gdt_LogDataBlob

DESCRIPTION
  Logs a GDT BLOB of data

PARAMETERS
  cpz_Iface - Points to the GDT interface containing the raw data to be logged
  (input)
 
  u_Format  - The format of data that needs to be used by the log parser
  (input)     C_GDT_BLOB_FORMAT_BINARY or C_GDT_BLOB_FORMAT_BINARY
 
 w_ContentType - The content type of the raw data used by parser
 (input)

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - If the log is generated successfuly
  FALSE - Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean gdt_LogDataBlob(const gdt_InterfaceType *cpz_Iface,
                        uint8 u_Format, uint16 w_ContentType);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* GNSS_GDT_H */


