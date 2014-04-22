/**************************************************************************
 * FILE: devprg_hsuart_pkt.c
 *
 * Copyright (c) 2020,2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Proprietary
 *
 *************************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/0.2/QcomPkg/Library/DevPrgLib/devprg_hsuart_pkt.c#2 $
  $DateTime: 2021/06/07 21:37:16 $
  $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2021-05-25   rrwt Changed arg to bytes instead of bits in crc_16_calc
2020-07-01   as   Create.
===========================================================================*/

#include "LoaderUtils.h"
#include "devprg_hsuart_pkt.h"
#include "devprg_cobs.h"
#include "crc.h"

enum devprg_packet_error_t {
    PACKET_SUCCESS,
    PACKET_ERROR_NULL_SRC_BUFFER,
    PACKET_ERROR_NULL_DST_BUFFER,
    PACKET_ERROR_INVALID_LENGTH,
    PACKET_ERROR_INVALID_PACKET_ID,
    PACKET_ERROR_CRC,
    PACKET_ERROR_VERSION_MISMATCH,
    PACKET_ERROR_DST_BUFFER_OVERFLOW
};

#define LITTLE_TO_BIG_SHORT(x) (uint16)((x & 0xff) << 8) | ((uint16)((x >> 8) & 0xff))
static const int32 g_endian = 1;
#define IS_BIG_ENDIAN() ((*(const int8*)&g_endian) == 0)

#define VERSION_PACKET_SIZE 2

#define PACKET_LAYER_MINOR_ID 0
#define PACKET_LAYER_MAJOR_ID 1

static const uint32 packet_id_size = PACKET_ID_SIZE;
static const uint32 version_packet_size = VERSION_PACKET_SIZE;

static uint16 devprg_calculate_crc16(uint8* buf, int bytes)
{
  uint16 crc_data = 0;
  crc_data = crc_16_calc(buf, bytes);
  return crc_data;
}

/******************************************************************************
* Name: devprg_packet_encode
*
* Description:
* Encodes the HSUART packet with packet id and CRC
*
* Arguments:
*   packet_id   [IN]: Packet ID
*   *src        [IN]: Pointer to source buffer
*   src_length  [IN]: Length of data in source buffer
*   *dst        [OUT]: Pointer to destination buffer
*   dst_length  [IN]: Length of destination buffer
*   *error      [OUT]: Error code

Returns:
* Returns the length of data on the destination buffer provided the error code
* is SUCCESS.
*
******************************************************************************/
static uint32 devprg_packet_encode(enum devprg_packet_id_t packet_id,
    unsigned char *src, uint32 src_length, unsigned char *dst,
    uint32 dst_length, enum devprg_packet_error_t *error)
{
    uint16 crc_data;
    uint32 crc_data_size;
    *error = PACKET_SUCCESS;
    if (dst == NULL)
    {
        *error = PACKET_ERROR_NULL_DST_BUFFER;
        return 0;
    }
    switch (packet_id)
    {
    case ACK_PACKET:
        *dst = ACK_PACKET;
        return packet_id_size;
    case NAK_PACKET:
        *dst = NAK_PACKET;
        return packet_id_size;
    case VERSION_PACKET:
        *dst = VERSION_PACKET;
        *(dst + 1) = PACKET_LAYER_MAJOR_ID;
        *(dst + 2) = PACKET_LAYER_MINOR_ID;
        crc_data = devprg_calculate_crc16(dst, packet_id_size + 
            version_packet_size);
        if (!IS_BIG_ENDIAN())
        {
            crc_data = LITTLE_TO_BIG_SHORT(crc_data);
        }
        memscpy(dst + packet_id_size + version_packet_size, 
            dst_length - (packet_id_size + version_packet_size), &crc_data, 
            sizeof(crc_data));
        crc_data_size = sizeof(crc_data);
        return packet_id_size + version_packet_size + crc_data_size;
    case READY_TO_READ_PACKET:
        *dst = READY_TO_READ_PACKET;
        crc_data = devprg_calculate_crc16(dst, packet_id_size);
        if (!IS_BIG_ENDIAN())
        {
            crc_data = LITTLE_TO_BIG_SHORT(crc_data);
        }
        memscpy(dst + packet_id_size, dst_length - packet_id_size, &crc_data,
            sizeof(crc_data));
        crc_data_size = sizeof(crc_data);
        return packet_id_size + crc_data_size;
    case PROTOCOL_PACKET:
    case END_OF_TRANSFER_PACKET:
        if (src == NULL)
        {
            *error = PACKET_ERROR_NULL_SRC_BUFFER;
            return 0;
        }
        if (src_length == 0)
        {
            *error = PACKET_ERROR_INVALID_LENGTH;
            return 0;
        }
        *dst = packet_id;
        memscpy(dst + packet_id_size, dst_length - packet_id_size, src, 
            src_length);
        crc_data = devprg_calculate_crc16(dst, packet_id_size + src_length);
        if (!IS_BIG_ENDIAN())
        {
            crc_data = LITTLE_TO_BIG_SHORT(crc_data);
        }

        memscpy(dst + packet_id_size + src_length, dst_length - packet_id_size, 
            &crc_data, sizeof(crc_data));
        crc_data_size = sizeof(crc_data);
        return packet_id_size + src_length + crc_data_size;
    default:
        *error = PACKET_ERROR_INVALID_PACKET_ID;
    }
    return 0;
}

/******************************************************************************
* Name: devprg_packet_decode
*
* Description:
* Decode the HSUART packet and determine packet id and check CRC
*
* Arguments:
*   *packet_id  [OUT]: Packet ID
*   *src        [IN]: Pointer to source buffer
*   length      [IN]: Length of data in source buffer
*   *dst        [OUT]: Pointer to destination buffer
*   *error      [OUT]: Error code

Returns:
* Returns the length of data on the destination buffer provided the error code
* is SUCCESS.
*
******************************************************************************/
static uint32 devprg_packet_decode(enum devprg_packet_id_t *packet_id,
    unsigned char *src, uint32 src_length, unsigned char *dst, 
    uint32 dst_length, enum devprg_packet_error_t *error)
{
    uint16 crc_data;
    uint32 crc_data_size;
    uint16 rx_crc_data;
    uint32 rx_crc_data_size;
    *error = PACKET_SUCCESS;
    if (src == NULL)
    {
        *error = PACKET_ERROR_NULL_SRC_BUFFER;
        return 0;
    }
    if (dst == NULL)
    {
        *error = PACKET_ERROR_NULL_DST_BUFFER;
        return 0;
    }
    if (src_length == 0)
    {
        *error = PACKET_ERROR_INVALID_LENGTH;
        return 0;
    }
    switch (*src) {
    case ACK_PACKET:
        if (src_length != 1)
        {
            *error = PACKET_ERROR_INVALID_LENGTH;
            return 0;
        }
        *packet_id = ACK_PACKET;
        return 0;
    case NAK_PACKET:
        if (src_length != 1)
        {
            *error = PACKET_ERROR_INVALID_LENGTH;
            return 0;
        }
        *packet_id = NAK_PACKET;
        return 0;
    case VERSION_PACKET:
        if (src_length != packet_id_size + version_packet_size + 
            sizeof(crc_data))
        {
            *error = PACKET_ERROR_INVALID_LENGTH;
            return 0;
        }
        *packet_id = VERSION_PACKET;
        crc_data = devprg_calculate_crc16(src,packet_id_size + 
            version_packet_size);
        memscpy(&rx_crc_data, sizeof(rx_crc_data),
            src + packet_id_size + version_packet_size, 
            sizeof(rx_crc_data));
        if (!IS_BIG_ENDIAN())
        {
            rx_crc_data = LITTLE_TO_BIG_SHORT(rx_crc_data);
        }
        if (rx_crc_data != crc_data)
        {
            *error = PACKET_ERROR_CRC;
            return 0;
        }
        if ((PACKET_LAYER_MAJOR_ID == *(src + 1)) &&
            (PACKET_LAYER_MINOR_ID == *(src + 2)))
        {
            return 0;
        }
        else
        {
            *error = PACKET_ERROR_VERSION_MISMATCH;
            return 0;
        }
    case PROTOCOL_PACKET:
    case END_OF_TRANSFER_PACKET:
        *packet_id = *src;
        if (src_length < packet_id_size + sizeof(crc_data))
        {
            *error = PACKET_ERROR_INVALID_LENGTH;
            return 0;
        }
        crc_data_size = sizeof(crc_data);
        if (dst_length < src_length - packet_id_size - crc_data_size)
        {
            *error = PACKET_ERROR_DST_BUFFER_OVERFLOW;
            return 0;
        }
        rx_crc_data_size = sizeof(rx_crc_data);
        crc_data = devprg_calculate_crc16(src, src_length - 
            rx_crc_data_size);
        memscpy(&rx_crc_data, sizeof(rx_crc_data),
            src + src_length - sizeof(rx_crc_data), 
                sizeof(rx_crc_data));
        if (!IS_BIG_ENDIAN())
        {
            rx_crc_data = LITTLE_TO_BIG_SHORT(rx_crc_data);
        }
        if (rx_crc_data != crc_data)
        {
            *error = PACKET_ERROR_CRC;
            return 0;
        }
        memscpy(dst, dst_length, src + packet_id_size, 
            src_length - packet_id_size - sizeof(crc_data));
        crc_data_size = sizeof(crc_data);
        return src_length - packet_id_size - crc_data_size;
    case READY_TO_READ_PACKET:
        *packet_id = *src;
        crc_data_size = sizeof(crc_data);
        if (src_length != packet_id_size + crc_data_size)
        {
            *error = PACKET_ERROR_INVALID_LENGTH;
            return 0;
        }
        rx_crc_data_size = sizeof(rx_crc_data);
        crc_data = devprg_calculate_crc16(src, src_length - 
            rx_crc_data_size);
        memscpy(&rx_crc_data, sizeof(rx_crc_data), src + packet_id_size,
            sizeof(rx_crc_data));
        if (!IS_BIG_ENDIAN())
        {
            rx_crc_data = (uint16)LITTLE_TO_BIG_SHORT(rx_crc_data);
        }
        if (rx_crc_data != crc_data)
        {
            *error = PACKET_ERROR_CRC;
            return 0;
        }
        return 0;
    default:
        *error = PACKET_ERROR_INVALID_PACKET_ID;
        return 0;
    }
}

/******************************************************************************
* Name: devprg_hsuart_packet_encode
*
* Description:
* Encodes the HSUART packet with packet id and CRC and perform stuffing
*
* Arguments:
*   packet_id   [IN]: Packet ID
*   *src        [IN]: Pointer to source buffer
*   src_length  [IN]: Length of data in source buffer
*   *dst        [OUT]: Pointer to destination buffer
*   dst_length  [IN]: Length of destination buffer
*   *error      [OUT]: Error code

Returns:
* Returns the length of data on the destination buffer provided the error code
* is SUCCESS.
*
******************************************************************************/
uint32 devprg_hsuart_packet_encode(enum devprg_packet_id_t packet_id,
    unsigned char *src, uint32 src_length, unsigned char *dst, 
    uint32 dst_length, enum devprg_hsuart_packet_error_t *error)
{
    uint32 result;
    unsigned char temp_buffer[MAX_HSUART_PACKET_SIZE] = { 0 };
    enum devprg_packet_error_t packet_error = PACKET_SUCCESS;
    enum devprg_cobs_error_t cobs_error = COBS_SUCCESS;
    *error = HSUART_PACKET_SUCCESS;
    result = devprg_packet_encode(packet_id, src, src_length, temp_buffer,
        MAX_HSUART_PACKET_SIZE, &packet_error);
    if (packet_error != PACKET_SUCCESS)
    {
        *error = HSUART_INVALID_PARAM_ERROR;
        return result;
    }

    if (packet_id != ACK_PACKET && packet_id != NAK_PACKET)
    {
        result = devprg_cobs_stuff_bytes(temp_buffer, result, dst, dst_length, 
            &cobs_error);
        if (cobs_error != COBS_SUCCESS) 
        {
            *error = HSUART_INVALID_PARAM_ERROR;
        }
    }
    else
    {
        memscpy(dst, result, temp_buffer, packet_id_size);
    }

    return result;
}

/******************************************************************************
* Name: devprg_hsuart_packet_decode
*
* Description:
* Unstuff the HSUART packet and then decode to determine the packet id and
* check CRC
*
* Arguments:
*   *packet_id  [OUT]: Packet ID
*   *src        [IN]: Pointer to source buffer
*   length      [IN]: Length of data in source buffer
*   *dst        [OUT]: Pointer to destination buffer
*   *error      [OUT]: Error code

Returns:
* Returns the length of data on the destination buffer provided the error code
* is SUCCESS.
*
******************************************************************************/
uint32 devprg_hsuart_packet_decode(enum devprg_packet_id_t *packet_id,
    unsigned char *src, uint32 length, unsigned char *dst, uint32 dst_length,
    enum devprg_hsuart_packet_error_t *error)
{
    uint32 result;
    unsigned char temp_buffer[MAX_HSUART_PACKET_SIZE] = { 0 };
    enum devprg_packet_error_t packet_error;
    enum devprg_cobs_error_t cobs_error = COBS_SUCCESS;
    *error = HSUART_PACKET_SUCCESS;
    packet_error = PACKET_SUCCESS;

    if (length > 1)
    {
        result = devprg_cobs_unstuff_bytes(src, length, temp_buffer, 
            MAX_HSUART_PACKET_SIZE, &cobs_error);
        if (cobs_error != COBS_SUCCESS)
        {
            if (COBS_ERROR_INVALID_STUFFING == cobs_error)
            {
                *error = HSUART_COBS_ERROR;
            }
            else
            {
                *error = HSUART_INVALID_PARAM_ERROR;
            }
            return result;
        }
        result = devprg_packet_decode(packet_id, temp_buffer, result, dst, 
            dst_length, &packet_error);
    }
    else
    {
        result = devprg_packet_decode(packet_id, src, length, dst, dst_length,
            &packet_error);
    }

    if (packet_error != PACKET_SUCCESS)
    {
        if (PACKET_ERROR_CRC == packet_error ||
            PACKET_ERROR_VERSION_MISMATCH == packet_error)
        {
            *error = HSUART_PACKET_CORRUPTION_ERROR;
        }
        else
        {
            *error = HSUART_INVALID_PARAM_ERROR;
        }
    }

    return result;
}
