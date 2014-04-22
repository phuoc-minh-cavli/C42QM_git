/*
 * Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/*
 * v4.1.testing mqtt/MQTTPacket.c
 * May 26, 2016
 * Distributed per InterNiche ref US2611-2
 */
/*******************************************************************************
 * Copyright (c) 2014 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Ian Craggs - initial API and implementation and/or initial documentation
 *    Sergio R. Caprile - non-blocking packet read functions for stream transport
 *******************************************************************************/

#include "StackTrace.h"
#include "MQTTPacket.h"

#include <string.h>
#include "stringl/stringl.h"
#include "mqtt_port.h"

extern qurt_mutex_t * mqtt_mutex;
/**
 * Encodes the message length according to the MQTT algorithm
 * @param buf the buffer into which the encoded data is written
 * @param length the length to be encoded
 * @return the number of bytes written to buffer
 */
int MQTTPacket_encode(unsigned char* buf, int length)
{
  int rc = 0;

  FUNC_ENTRY;
  do
  {
    char d = length % 128;
    length /= 128;
    /* if there are more digits to encode, set the top bit of this digit */
    if (length > 0)
      d |= 0x80;
    buf[rc++] = d;
  } while (length > 0);
  FUNC_EXIT_RC(rc);
  return rc;
}


/**
 * Decodes the message length according to the MQTT algorithm
 * @param getcharfn pointer to function to read the next character from the data source
 * @param value the decoded length returned
 * @return the number of bytes read from the socket
 */
int MQTTPacket_decode(int (*getcharfn)(unsigned char*, int), int* value)
{
  unsigned char c;
  int multiplier = 1;
  int len = 0;
#define MAX_NO_OF_REMAINING_LENGTH_BYTES 4

  FUNC_ENTRY;
  *value = 0;
  do
  {
    int rc = MQTTPACKET_READ_ERROR;

    if (++len > MAX_NO_OF_REMAINING_LENGTH_BYTES)
    {
      rc = MQTTPACKET_READ_ERROR;	/* bad data */
      goto exit;
    }
    rc = (*getcharfn)(&c, 1);
    if (rc != 1)
      goto exit;
    *value += (c & 127) * multiplier;
    multiplier *= 128;
  } while ((c & 128) != 0);
exit:
  FUNC_EXIT_RC(len);
  return len;
}


int MQTTPacket_len(int rem_len)
{
  rem_len += 1; /* header byte */

  /* now remaining_length field */
  if (rem_len < 128)
    rem_len += 1;
  else if (rem_len < 16384)
    rem_len += 2;
  else if (rem_len < 2097151)
    rem_len += 3;
  else
    rem_len += 4;
  return rem_len;
}


static unsigned char* bufptr;

int bufchar(unsigned char* c, int count)
{
  int i;

  for (i = 0; i < count; ++i)
    *c = *bufptr++;
  return count;
}


int MQTTPacket_decodeBuf(unsigned char* buf, int* value)
{
    int length; 
    MQTT_LOCK_MUTEX(mqtt_mutex);
    bufptr = buf;
    length = MQTTPacket_decode(bufchar, value);
    MQTT_UNLOCK_MUTEX(mqtt_mutex);
    return length;
}


/**
 * Calculates an integer from two bytes read from the input buffer
 * @param pptr pointer to the input buffer - incremented by the number of bytes used & returned
 * @return the integer value calculated
 */
int readInt(unsigned char** pptr)
{
  unsigned char* ptr = *pptr;
  int len = 256*(*ptr) + (*(ptr+1));
  *pptr += 2;
  return len;
}


/**
 * Reads one character from the input buffer.
 * @param pptr pointer to the input buffer - incremented by the number of bytes used & returned
 * @return the character read
 */
char readChar(unsigned char** pptr)
{
  char c = **pptr;
  (*pptr)++;
  return c;
}


/**
 * Writes one character to an output buffer.
 * @param pptr pointer to the output buffer - incremented by the number of bytes used & returned
 * @param c the character to write
 */
void writeChar(unsigned char** pptr, char c)
{
  **pptr = c;
  (*pptr)++;
}


/**
 * Writes an integer as 2 bytes to an output buffer.
 * @param pptr pointer to the output buffer - incremented by the number of bytes used & returned
 * @param anInt the integer to write
 */
void writeInt(unsigned char** pptr, int anInt)
{
  **pptr = (unsigned char)(anInt / 256);
  (*pptr)++;
  **pptr = (unsigned char)(anInt % 256);
  (*pptr)++;
}


/**
 * Writes a "UTF" string to an output buffer.  Converts C string to length-delimited.
 * @param pptr pointer to the output buffer - incremented by the number of bytes used & returned
 * @param string the C string to write
 */
void writeCString(unsigned char** pptr, const char* string)
{
  int len = strlen(string);
  writeInt(pptr, len);
  memscpy(*pptr,len, string, len);
  *pptr += len;
}


int getLenStringLen(char* ptr)
{
  int len = 256*((unsigned char)(*ptr)) + (unsigned char)(*(ptr+1));
  return len;
}


void writeMQTTString(unsigned char** pptr, MQTTString mqttstring)
{
  if (mqttstring.lenstring.len > 0)
  {
    writeInt(pptr, mqttstring.lenstring.len);
    memscpy(*pptr, mqttstring.lenstring.len,mqttstring.lenstring.data, mqttstring.lenstring.len);
    *pptr += mqttstring.lenstring.len;
  }
  else if (mqttstring.cstring)
    writeCString(pptr, mqttstring.cstring);
  else
    writeInt(pptr, 0);
}


/**
 * @param mqttstring the MQTTString structure into which the data is to be read
 * @param pptr pointer to the output buffer - incremented by the number of bytes used & returned
 * @param enddata pointer to the end of the data: do not read beyond
 * @return 1 if successful, 0 if not
 */
int readMQTTLenString(MQTTString* mqttstring, unsigned char** pptr, unsigned char* enddata)
{
  int rc = 0;

  FUNC_ENTRY;
  /* the first two bytes are the length of the string */
  if (enddata - (*pptr) > 1) /* enough length to read the integer? */
  {
    mqttstring->lenstring.len = readInt(pptr); /* increments pptr to point past length */
    if (&(*pptr)[mqttstring->lenstring.len] <= enddata)
    {
      mqttstring->lenstring.data = (char*)*pptr;
      *pptr += mqttstring->lenstring.len;
      rc = 1;
    }
  }
  mqttstring->cstring = NULL;
  FUNC_EXIT_RC(rc);
  return rc;
}


/**
 * Return the length of the MQTTstring - C string if there is one, otherwise the length delimited string
 * @param mqttstring the string to return the length of
 * @return the length of the string
 */
int MQTTstrlen(MQTTString mqttstring)
{
  int rc = 0;

  if (mqttstring.cstring)
    rc = strlen(mqttstring.cstring);
  else
    rc = mqttstring.lenstring.len;
  return rc;
}

