/******************************************************************************

  @file    sms_decode.c

  ---------------------------------------------------------------------------

  Copyright (c) 2016-2021 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------

 *****************************************************************************/

#include <string.h>
#include "liblwm2m.h"
#include "iotapp_qmi_internal.h"
#include "ds_log.h"

#define B7_MASK 0x7F
#define B8_MASK 0xFF

#define SMS_TYPE_DELIVERY 0x04
#define SMS_TYPE_SUBMIT   0x11
#define SMS_TYPE_DELIVERY_UDH  0x40
#define SMS_DCS_7_BIT          0x00
#define SMS_DCS_8_BIT          0x04

extern char sms_number_from_wms[32];

int get_sms_payload(const unsigned char *pdu_buf, int pdu_buf_len, char **response, boolean routed_msg)
{

  unsigned int bit_carry_on = 1;
  unsigned int sms_deliver_hdr = 0;
  unsigned int sms_payload_len = 0;
  unsigned int sms_payload_hdr = 0;
  unsigned int sms_dcs_hdr = 0;
  int smsc_len = 0;
  int sender_len = 0;
  int tp_dcs = 0, code_sch = 0;
  int i = 1;
  int j = 0;
  int k = 0;
  char *data = NULL;
  int payload_ix = 0;
  *response = NULL;

  if((pdu_buf == NULL) || (pdu_buf_len <=0))
  {
    ds_iot_ulog_err("SMS_LOG :QMI message buffer is NULL or buffer length is less than zero.");
    return -1;
  }

  if(routed_msg == false)
  {
    smsc_len = pdu_buf[0];

    /* Skip SMSC information */
    sms_deliver_hdr =  smsc_len + 1;
  }

  else
  {
    sms_deliver_hdr =  0;
  }

  if(sms_deliver_hdr + 1 > pdu_buf_len)
  {
    ds_iot_ulog_err("SMS_LOG :Received buffer length is not enough.");
    return -1;
  }

  /* Check whether UDHI bit is set and SMS type is SMS-DELIVER */
  if((pdu_buf[sms_deliver_hdr] & SMS_TYPE_DELIVERY_UDH) == 0)
  {
    ds_iot_ulog_err("SMS_LOG :Message does not have UDH flag set.");
    return -1;
  }

  sender_len = pdu_buf[sms_deliver_hdr + 1];

  ds_iot_ulog_high1("SMS_LOG :SMS Number length : %d", sender_len);
  
  if(sms_deliver_hdr + 3 + ((sender_len + 1) / 2) + 2 > pdu_buf_len)
  {
     return -1;
  }
  for (k = 0; k < sender_len && k < 32; ++k) 
  {
    if (k % 2 == 0)
      sms_number_from_wms[k] = (pdu_buf[(sms_deliver_hdr + 3) + (k / 2)] & 0x0F) + '0';
    else
      sms_number_from_wms[k] = ((pdu_buf[(sms_deliver_hdr + 3) + (k / 2)] & 0xF0) >> 4) + '0';
  }
  if(sender_len >= 32)
  {
     return -1;
  }
  sms_number_from_wms[sender_len] = '\0';  // Terminate C string.
  ds_iot_log_strf_high("SMS_LOG :SMS NUMBER : %s", sms_number_from_wms);

  /* Skip sender information  and TP-PID */
  sms_dcs_hdr = sms_deliver_hdr + 3 + ((sender_len + 1) / 2) + 1;

  tp_dcs = pdu_buf[sms_dcs_hdr];               //Data Coding scheme value

  ds_iot_ulog_high1("SMS_LOG :Data coding scheme %d", tp_dcs);
  code_sch = tp_dcs & 0x0F;
  if((code_sch >= 0) && (code_sch <= 3))
    code_sch = SMS_DCS_7_BIT;
  else if((code_sch >= 4) && (code_sch <= 7))
    code_sch = SMS_DCS_8_BIT;
  else
  {
    ds_iot_ulog_err("SMS_LOG :Data coding scheme other than GSM 7-bit.and 8-bit");
    return -1;
  }

  /* Skip timestamp */

  sms_payload_hdr = sms_dcs_hdr + 1 +7 ;      // Timestamp 7 octets

  if(sms_payload_hdr + 1 > pdu_buf_len)
  {
    ds_iot_ulog_err("SMS_LOG :Buffer length is not enough.");
    return -1;
  }

  sms_payload_len = pdu_buf[sms_payload_hdr];

  if(sms_payload_len == 0) 
  {
    ds_iot_ulog_err("SMS_LOG :User data length is zero ");
    return -1;
  }

  pdu_buf = pdu_buf + sms_payload_hdr + 1;
  pdu_buf_len = pdu_buf_len - (sms_payload_hdr + 1);

  if(code_sch == SMS_DCS_8_BIT)
  {
    if(sms_payload_len > pdu_buf_len)
    {
      return -1;
    }
    *response = (char *)lwm2m_malloc(sms_payload_len);

    if(*response == NULL)
    {
      ds_iot_ulog_err("SMS_LOG :LWM2M_LOG: Memory Allocation FAILED");
      return -1;
    }

    for(i=0; i<sms_payload_len; i++)
    {
      (*response)[i] = pdu_buf[i];
    }

    ds_iot_ulog_high1("SMS_LOG : User data length is %d ", sms_payload_len);
    return sms_payload_len;
  }

  payload_ix = 0;

  data = (char *)lwm2m_malloc(sms_payload_len);

  if(data == NULL)
  {
    ds_iot_ulog_err("SMS_LOG :LWM2M_LOG: Memory Allocation FAILED");
    return -1;
  }

  if(pdu_buf_len > 0)
  {
    data[payload_ix++] = B7_MASK & pdu_buf[0];
  }

  i = 1;

  while(i < pdu_buf_len)
  {
    if(payload_ix >= sms_payload_len)
    {  
       if(data)
       {
         lwm2m_free(data);
         data = NULL;
       }
       return -1;  
    }
    data[payload_ix++] = B7_MASK & ((pdu_buf[i] << bit_carry_on)
        | (pdu_buf[i - 1] >> (8 - bit_carry_on)));

    if(payload_ix == sms_payload_len)
      break;

    bit_carry_on++;

    if(bit_carry_on == 8)
    {
      bit_carry_on = 1;
      data[payload_ix++] = pdu_buf[i] & B7_MASK;
      if(payload_ix == sms_payload_len)
        break;
    }

    i++;
  }

  if(payload_ix < sms_payload_len)
  {
    data[payload_ix++] = pdu_buf[i - 1] >> (8 - bit_carry_on);
  }

  if(payload_ix != sms_payload_len) 
  {
    if(data)
    {
      lwm2m_free(data);
      data = NULL;
    }
    ds_iot_ulog_err("SMS_LOG :Error in PDU decoding.");
    return -1;
  }

  *response = (char *)lwm2m_malloc(sms_payload_len/2);

  if(*response == NULL || sms_payload_len < 2)
  {
    if(data)
    {
      lwm2m_free(data);
      data = NULL;
    }
    ds_iot_ulog_err("SMS_LOG :LWM2M_LOG: Memory Allocation FAILED");
    return -1;
  }

  for(i=0,j=0;j<sms_payload_len/2;i+=2,j++)
  {
    (*response)[j] = ((data[i] >'9' ? data[i]-=7 : data[i]) <<4) | ((data[i+1] >'9' ? data[i+1]-=7 : data[i+1]) & 0x0F);
  }

  if(data)
  {
    lwm2m_free(data);
    data = NULL;
  }

  ds_iot_ulog_high1("SMS_LOG : User data length is %d ", sms_payload_len/2);
  return sms_payload_len/2;
}


