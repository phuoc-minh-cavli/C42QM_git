#ifndef REMOTEFS_SAHARA_H
#define REMOTEFS_SAHARA_H

/*==============================================================================

                      Sahara Protocol for Remote Storage Server

DESCRIPTION
   This file contains definitions for Sahara Protocol transport for the
   Remote Storage Server.

Copyright (c) 2011-14 by QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
==============================================================================*/
/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.tx/6.0/storage/remotefs/src/remotefs_sahara.h#1 $
  $DateTime: 2020/01/30 22:49:35 $
  $Author: pwbldsvc $

YYYY-MM-DD       who     what, where, why
--------------   ---     -------------------------------------------------------
2014-12-05       dks     Add device serial number read support.
2014-06-04       ab      Fix racecondition between TX and RX.
2013-10-20       bb      Added force SIO flush for RESET_RSP after write iovec
2012-06-22       vk      Added a structure for holding debug information
2011-10-11       vk      Initial version
==============================================================================*/

#include "comdef.h"
#include "rex.h"
#include "remotefs_comm_api.h"

/* Sahara Protocol Version */
#define SAHARA_VERSION_MAJOR 2
#define SAHARA_VERSION_MAJOR_SUPPORTED 1
#define SAHARA_VERSION_MINOR 3

/* Sahara command IDs */
enum remotefs_sahara_cmd_id
{
   SAHARA_NO_CMD_ID          = 0x00,
   SAHARA_HELLO_ID           = 0x01, /* sent from target to host */
   SAHARA_HELLO_RESP_ID      = 0x02, /* sent from host to target */
   SAHARA_READ_DATA_ID       = 0x03, /* sent from target to host */
   SAHARA_END_IMAGE_TX_ID    = 0x04, /* sent from target to host */
   SAHARA_DONE_ID            = 0x05, /* sent from host to target */
   SAHARA_DONE_RESP_ID       = 0x06, /* sent from target to host */
   SAHARA_RESET_ID           = 0x07, /* sent from host to target */
   SAHARA_RESET_RESP_ID      = 0x08, /* sent from target to host */
   SAHARA_MEMORY_DEBUG_ID    = 0x09, /* sent from target to host */
   SAHARA_MEMORY_READ_ID     = 0x0A, /* sent from host to target */
   SAHARA_CMD_READY_ID       = 0x0B, /* sent from target to host */
   SAHARA_CMD_SWITCH_MODE_ID = 0x0C, /* sent from host to target */
   SAHARA_CMD_EXEC_ID        = 0x0D, /* sent from host to target */
   SAHARA_CMD_EXEC_RESP_ID   = 0x0E, /* sent from target to host */
   SAHARA_CMD_EXEC_DATA_ID   = 0x0F, /* sent from host to target */

   /* place all new commands above this */
   SAHARA_LAST_CMD_ID,
   SAHARA_MAX_CMD_ID         = 0x7FFFFFFF /* To ensure 32-bits wide */
};

/* Status codes for Sahara */
enum remotefs_sahara_status
{
   SAHARA_NAK_SUCCESS =                        0x00,
   SAHARA_NAK_INVALID_CMD =                    0x01,
   SAHARA_NAK_PROTOCOL_MISMATCH =              0x02,
   SAHARA_NAK_INVALID_TARGET_PROTOCOL =        0x03,
   SAHARA_NAK_INVALID_HOST_PROTOCOL =          0x04,
   SAHARA_NAK_INVALID_PACKET_SIZE =            0x05,
   SAHARA_NAK_UNEXPECTED_IMAGE_ID =            0x06,
   SAHARA_NAK_INVALID_HEADER_SIZE =            0x07,
   SAHARA_NAK_INVALID_DATA_SIZE =              0x08,
   SAHARA_NAK_INVALID_IMAGE_TYPE =             0x09,
   SAHARA_NAK_INVALID_TX_LENGTH =              0x0A,
   SAHARA_NAK_INVALID_RX_LENGTH =              0x0B,
   SAHARA_NAK_GENERAL_TX_RX_ERROR =            0x0C,
   SAHARA_NAK_READ_DATA_ERROR =                0x0D,
   SAHARA_NAK_UNSUPPORTED_NUM_PHDRS =          0x0E,
   SAHARA_NAK_INVALID_PDHR_SIZE =              0x0F,
   SAHARA_NAK_MULTIPLE_SHARED_SEG =            0x10,
   SAHARA_NAK_UNINIT_PHDR_LOC =                0x11,
   SAHARA_NAK_INVALID_DEST_ADDR =              0x12,
   SAHARA_NAK_INVALID_IMG_HDR_DATA_SIZE =      0x13,
   SAHARA_NAK_INVALID_ELF_HDR =                0x14,
   SAHARA_NAK_UNKNOWN_HOST_ERROR =             0x15,
   SAHARA_NAK_TIMEOUT_RX =                     0x16,
   SAHARA_NAK_TIMEOUT_TX =                     0x17,
   SAHARA_NAK_INVALID_HOST_MODE =              0x18,
   SAHARA_NAK_INVALID_MEMORY_READ =            0x19,
   SAHARA_NAK_INVALID_DATA_SIZE_REQUEST =      0x1A,
   SAHARA_NAK_MEMORY_DEBUG_NOT_SUPPORTED =     0x1B,
   SAHARA_NAK_INVALID_MODE_SWITCH =            0x1C,
   SAHARA_NAK_CMD_EXEC_FAILURE =               0x1D,
   SAHARA_NAK_EXEC_CMD_INVALID_PARAM =         0x1E,
   SAHARA_NAK_EXEC_CMD_UNSUPPORTED =           0x1F,
   SAHARA_NAK_EXEC_DATA_INVALID_CLIENT_CMD =   0x20,
   SAHARA_NAK_HASH_TABLE_AUTH_FAILURE =        0x21,
   SAHARA_NAK_HASH_VERIFICATION_FAILURE =      0x22,
   SAHARA_NAK_HASH_TABLE_NOT_FOUND =           0x23,
   SAHARA_NAK_TARGET_INIT_FAILURE =            0x24,
   SAHARA_NAK_LAST_CODE,
   SAHARA_NAK_MAX_CODE = 0x7FFFFFFF /* To ensure 32-bits wide */
};

enum remotefs_sahara_mode
{
   SAHARA_MODE_IMAGE_TX_PENDING  = 0x0,
   SAHARA_MODE_IMAGE_TX_COMPLETE = 0x1,
   SAHARA_MODE_MEMORY_DEBUG      = 0x2,
   SAHARA_MODE_COMMAND           = 0x3,
   /* place all new commands above this */
   SAHARA_MODE_LAST,
   SAHARA_MODE_MAX = 0x7FFFFFFF
};

enum remotefs_sahara_client_command
{
   SHARA_READ_SERIAL      = 0x1,
   SHARA_READ_MDM_HW_ID   = 0x2,
   SHARA_READ_OK_HASH     = 0x3,
   SHARA_READ_DEBUG_DATA  = 0x6,
   SHARA_READ_SBL_SW_VER  = 0x7,
};

enum remotefs_sahara_state
{
  SAHARA_READY,
  SAHARA_MEM_DBG_START,
  SAHARA_MEM_DBG,
  SAHARA_MEM_READ,
  SAHARA_RESET,
  SAHARA_END_OF_IMG_TRANSFER,
  SAHARA_CMD_EXEC_READY,
  SAHARA_CMD_EXEC,
  SAHARA_CMD_EXEC_DATA_TX,
 };

/*=============================================================================
  Sahara protocol packet defintions
=============================================================================*/
/* HELLO command packet type - sent from target to host
     indicates start of protocol on target side */
PACKED struct sahara_packet_hello
{
   uint32 command;                 /* command ID */
   uint32 length;                  /* packet length incl command and length */
   uint32 version;                 /* target protocol version number */
   uint32 version_supported;       /* minimum protocol version number supported */
                                   /*   on target */
   uint32 cmd_packet_length;       /* maximum packet size supported for command */
                                   /*   packets */
   uint32 mode;                    /* expected mode of target operation */
   uint32 reserved0;               /* reserved field */
   uint32 reserved1;               /* reserved field */
   uint32 reserved2;               /* reserved field */
   uint32 reserved3;               /* reserved field */
   uint32 reserved4;               /* reserved field */
   uint32 reserved5;               /* reserved field */
}PACKED_POST;

/* HELLO_RESP command packet type - sent from host to target
     response to hello, protocol version running on host and status sent */
PACKED struct sahara_packet_hello_resp
{
   uint32 command;                 /* command ID */
   uint32 length;                  /* packet length incl command and length */
   uint32 version;                 /* host protocol version number */
   uint32 version_supported;       /* minimum protocol version number supported */
                                   /*   on host */
   uint32 status;                  /* OK or error condition */
   uint32 mode;                    /* mode of operation for target to execute */
   uint32 reserved0;               /* reserved field */
   uint32 reserved1;               /* reserved field */
   uint32 reserved2;               /* reserved field */
   uint32 reserved3;               /* reserved field */
   uint32 reserved4;               /* reserved field */
   uint32 reserved5;               /* reserved field */
}PACKED_POST;

/* MEMORY_DEBUG packet type - sent from target to host
     sends host the location and length of memory region table */
PACKED struct sahara_packet_memory_debug
{
   uint32 command;                 /* command ID */
   uint32 length;                  /* packet length incl command and length */
   uint32 memory_table_addr;       /* location of memory region table */
   uint32 memory_table_length;     /* length of memory table */
}PACKED_POST;

/* MEMORY_READ packet type - sent from host to target
     sends memory address and length to read from target memory */
PACKED struct sahara_packet_memory_read
{
   uint32 command;                 /* command ID */
   uint32 length;                  /* packet length incl command and length */
   uint32 memory_addr;             /* memory location to read from */
   uint32 memory_length;           /* length of data to send */
}PACKED_POST;

/* READ DATA packet type - sent from target to host
     sends host the image ID to read with offset and length */
PACKED struct sahara_packet_read_data
{
   uint32 command;                 /* command ID */
   uint32 length;                  /* packet length incl command and length */
   uint32 image_id;                /* location of memory region table */
   uint32 data_offset;             /* length of memory table */
   uint32 data_length;             /* length of memory table */
}PACKED_POST;

/* End of Image Transfer packet type - sent from target to host
     sends host the image ID along with a status code */
PACKED struct sahara_packet_end_of_image_tx
{
   uint32 command;                 /* command ID */
   uint32 length;                  /* packet length incl command and length */
   uint32 image_id;                /* location of memory region table */
   uint32 status;                  /* length of memory table */
}PACKED_POST;

/* Done packet type - sent from host to target
     Sent from host when it receives a EOT with success to end the protocol */
PACKED struct sahara_packet_done
{
   uint32 command;                 /* command ID */
   uint32 length;                  /* packet length incl command and length */
}PACKED_POST;

/* Done Response packet type - sent from target to host
     sends host the image transfer status - pending or complete */
PACKED struct sahara_packet_done_resp
{
   uint32 command;                 /* command ID */
   uint32 length;                  /* packet length incl command and length */
   uint32 status;                  /* length of memory table */
}PACKED_POST;

/* Reset packet type - sent from host to target
     Sent from host when it wants to reset the packet or stop the memory dump */
PACKED struct sahara_packet_reset
{
   uint32 command;                 /* command ID */
   uint32 length;                  /* packet length incl command and length */
}PACKED_POST;

/* Reset Response packet type - sent from target to host
     Sends host the ack for the reset packet */
PACKED struct sahara_packet_reset_resp
{
   uint32 command;                 /* command ID */
   uint32 length;                  /* packet length incl command and length */
}PACKED_POST;

/* COMMAND ready type - Target indicates to host that it is ready for commands */
PACKED struct sahara_packet_cmd_ready
{
   uint32 command;                 /* command ID */
   uint32 length;                  /* packet length incl command and length */
};

/* COMMAND packet type - indicates a generic command packet */
PACKED struct sahara_packet_cmd_exec
{
   uint32 command;                 /* command ID */
   uint32 length;                  /* packet length incl command and length */
   uint32 cli_cmd;                 /* Client command to execute */
}PACKED_POST;

/* COMMAND packet type - indicates a command response packet */
PACKED struct sahara_packet_cmd_exec_resp
{
   uint32 command;                 /* command ID */
   uint32 length;                  /* packet length incl command and length */
   uint32 cli_cmd;                 /* Client command executed */
   uint32 resp_len;                /* Response length */
}PACKED_POST;

/* COMMAND packet type - indicates a command response packet */
PACKED struct sahara_packet_cmd_exec_data
{
   uint32 command;                 /* command ID */
   uint32 length;                  /* packet length incl command and length */
   uint32 cli_cmd;                 /* Client command executed */
}PACKED_POST;

PACKED struct sahara_packet_cmd_switch_mode
{
  uint32 command;                 /* command ID */
  uint32 length;                  /* packet length incl command and length */
  uint32 mode;                    /* Mode to switch to */
}PACKED_POST;

PACKED union sahara_pkt_type
{
  /* TX */
  struct sahara_packet_hello hello;
  struct sahara_packet_memory_debug mem_dbg;
  struct sahara_packet_end_of_image_tx end_img_tx;
  struct sahara_packet_cmd_ready cmd_ready;
  struct sahara_packet_cmd_exec_resp cmd_resp;
  struct sahara_packet_reset_resp reset_resp;
  struct sahara_packet_read_data read_data;

  /* RX */
  struct sahara_packet_hello_resp hello_resp;
  struct sahara_packet_memory_read mem_read;
  struct sahara_packet_cmd_switch_mode switch_mode;
  struct sahara_packet_cmd_exec cmd_exec;
  struct sahara_packet_cmd_exec_data cmd_data;
  struct sahara_packet_reset reset;
} PACKED_POST;

PACKED struct remotefs_sahara_packet
{
  uint32 cmd_id;
  uint32 pkt_len;
  union sahara_pkt_type pkt;
}PACKED_POST;

// From boot dload_debug.h
#define MEM_DEBUG_STRLEN_BYTES 20

typedef struct
{
   byte save_pref;
   uint32 mem_base;
   uint32 length;
   char desc[MEM_DEBUG_STRLEN_BYTES];
   char filename[MEM_DEBUG_STRLEN_BYTES];
} mem_debug_type;


#define REMOTEFS_SAHARA_FLUSH_TIMEOUT_MAX_MS 5000  //Milli seconds

/*===========================================================================
                      PUBLIC FUNCTION DECLARATIONS
===========================================================================*/
void remotefs_sahara_init (rex_tcb_type *task);

remotefs_status_type remotefs_sahara_memory_debug (const char *filename,
                                       uint8* data_address, uint32 size);


#endif  /* REMOTEFS_SAHARA_H */
