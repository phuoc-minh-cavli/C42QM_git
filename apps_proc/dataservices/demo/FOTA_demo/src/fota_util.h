/*===========================================================================
                         FOTA_UTIL.H

DESCRIPTION
   This header file contains definitions used internally by the fota object test
   Module. 
  
   Copyright (c) 2021 by Qualcomm Technologies INCORPORATED.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/
#include "qapi_lwm2m.h"
#include "qapi_coap.h"

typedef struct _list_t
{
  struct _list_t *next;
  uint16_t        id;
} list_t;

#define LIST_ADD(H,N) list_add((list_t *)H, (list_t *)N);

qapi_Status_t coap_copy_client_transaction(qapi_Coap_Session_Hdl_t hndl,qapi_Coap_Transaction_t *in_clientTransaction ,
                                                       qapi_Coap_Transaction_t **out_clientTransaction);

qapi_Status_t coap_copy_pkt(qapi_Coap_Session_Hdl_t hndl,
                                  qapi_Coap_Packet_t *in_coap_pkt,
                                  qapi_Coap_Packet_t **out_coap_pkt);

int lwm2m_copy_data_(qapi_Net_LWM2M_Data_v2_t *in_lwm2m_data, 
                           qapi_Net_LWM2M_Data_v2_t **out_lwm2m_data);

int32_t app_free_id_info_(qapi_Net_LWM2M_Id_Info_v2_t *id_infoP);

