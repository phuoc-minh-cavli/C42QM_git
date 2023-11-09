/*****************************************************************************
  Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
*****************************************************************************/


#ifndef __QCA4004_FOTA_H__
#define __QCA4004_FOTA_H__

/*-------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/
#include "qapi_types.h"

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *-----------------------------------------------------------------------*/
#define COMMAND_OTA_STRING				"OTA"

#define QCA4004_IMG_DIR_DFT				"/datatx"

#define QCA4004_IMG_PATH_DFT			"/datatx/ota.bin"

#define QCA4004_IMG_FILENAME_LEN		128

#define QCA4004_MD5_CHECKSUM_LEN		16

#define QCA4004_IMG_HDR_SIZE			24

#define QCA4004_IMG_HDR_FLAG			0x00001234

#define QCA4004_PARTITION_SIZE			0x40000

#define QCA4004_FOTA_BLOCK_SIZE			128

#define QCA4004_FOTA_BLOCK_DATA_SIZE	(QCA4004_FOTA_BLOCK_SIZE-5)

/* control codes of UART OTA data transmit */
#define QCA4004_FOTA_SOH 				0x01 /* start of header UART OTA data transmit */

#define QCA4004_FOTA_ACK 				0x06 /* acknowledge */

#define QCA4004_FOTA_NAK 				0x15 /* negative acknowledge */

/*-------------------------------------------------------------------------
 * Type Declarations
 *-----------------------------------------------------------------------*/

/* FOTA Control Flag */
typedef enum
{
    QCA4004_FOTA_IMG_CHECK		= 0x0001, 
	QCA4004_FOTA_AUTO_RESET		= 0x0002,
	QCA4004_FOTA_VERSION_CHECK	= 0x0004,
} QCA4004_FOTA_FLAG;

#define QCA4004_FOTA_FLAG_DFT = QCA4004_FOTA_IMG_CHECK | QCA4004_FOTA_AUTO_RESET

/* FOTA SM State */
typedef enum
{
    QCA4004_FOTA_STATE_IDLE = 0,
    QCA4004_FOTA_STATE_HELLO = 1,
    QCA4004_FOTA_STATE_H2T_NOTIFY = 2,
    QCA4004_FOTA_STATE_H2T_DATA = 3,
    QCA4004_FOTA_STATE_H2T_RESET = 4,
    QCA4004_FOTA_STATE_H2T_COMPLETE = 5,
    QCA4004_FOTA_STATE_WAIT_RESULT = 6,

    QCA4004_FOTA_STATE_MAX,
} QCA4004_FOTA_STATE;

typedef struct QCA4004_FOTA_Ctx_s
{
    /* FW image name from Cloud */
    char image_name[QCA4004_IMG_FILENAME_LEN];

    /* Entire FW image size */
    uint32_t image_size;
	
	/* */
	uint8_t checksum[QCA4004_MD5_CHECKSUM_LEN];
	
	/* FOTA data transmit */
	uint8_t block[QCA4004_FOTA_BLOCK_SIZE];
	
	/* */
	uint16_t seq;

	/* FOTA control flag */
	int32_t flag;
	
	/* */
	uint32_t retValue;

    /* FOTA state machine */
    QCA4004_FOTA_STATE fota_state;
	
	/* */
	qurt_signal_t signal;
	
	/* */
	qurt_signal_t signalc;
} QCA4004_FOTA_Ctx_t;

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 *-----------------------------------------------------------------------*/

int qca4004_fota_start_internal(char *filePath, int32_t flag, uint32_t *version);

#endif

