/*============================================================================

   FILE:       q6atl.c

   DESCRIPTION: abstract transport layer interface file.

   PUBLIC CLASSES:  Not Applicable

   INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright © 2010 - 2012, 2016 QUALCOMM Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
 ===========================================================================*/

/*===========================================================================
                      EDIT HISTORY FOR MODULE

This section contains comments describing changes made to this module.
Notice that changes are listed in reverse chronological order.

when       who      what, where, why
--------   ---      ----------------------------------------------------------
11/15/16   rk       Abstracting OSAL layer
09/01/14   rp       enhanced the csd init debug buffer 
08/02/13   vk       added a check to avoid passing NULL to DALSYS function
04/27/10   mingchen initial draft
============================================================================*/

#include <stddef.h>
#include <string.h> /* memcpy */
#include "apr_comdef.h"
#include "apr_errcodes.h"
#include "apr_api.h"
#include "aprv2_packet.h"
/* CSD header files */
#include "csd.h"
#include "csd_q6atl_main.h"
#include "csd_q6msg.h"
#include "csd_debug_msg.h"
#include "csd_q6msg_def.h"
#include "aprv2_api_inline.h"
#include "csd_debug_info.h"
#include "csd_os_dependencies.h"
#include "csd_error_map.h"

/*===========================================================================
                        STATIC VARIABLES
===========================================================================*/
struct q6atl_sync_msg_key {
  uint16_t port;           /* port num for finding sync trans */
  uint16_t addr;           /* addr */
  uint32_t client_token;   /* apr token */
};

static struct csd_q6atl* q6atl_main_data;

/*===========================================================================
                        INTERNAL FUNCTIONS
===========================================================================*/

uint32_t csd_q6atl_gen_apr_token()
{
    static uint32_t q6_trans_no = 0;
    uint32_t tmp_token;
    csd_criticalsection_enter();
    q6_trans_no++;
    if(q6_trans_no == 0xffffffff)
        q6_trans_no = 1;
    tmp_token = q6_trans_no;
    csd_criticalsection_leave();
    return tmp_token;
}




/*===========================================================================
FUNCTION csd_atl_get_service_index

DESCRIPTION

RETURN VALUE
   None

SIDE EFFECTS
===========================================================================*/
static int16_t csd_atl_get_service_index(uint16_t addr)
{
  switch(addr&0xFF)
  {
    case CSD_SERVICE_AS:
      return 0;
    case CSD_SERVICE_AC:
      return 1;
    case CSD_SERVICE_DM:
      return 2;
    default:
      return -1;
  }
}
/*===========================================================================
FUNCTION csd_atl_get_service

DESCRIPTION

RETURN VALUE
   None

SIDE EFFECTS
===========================================================================*/
static uint16_t csd_atl_get_service(uint16_t index)
{
  switch(index)
  {
    case 0:
        return CSD_SERVICE_AS;
    case 1:
        return CSD_SERVICE_AC;
    case 2:
        return CSD_SERVICE_DM;
    default:
      return 0;
  }
}
/*===========================================================================
FUNCTION csd_atl_apr_cb

DESCRIPTION

RETURN VALUE
   None

SIDE EFFECTS
===========================================================================*/
static int32_t csd_atl_apr_cb( aprv2_packet_t* packet, void* dispatch_data)
{
  int32_t rc = CSD_EOK;
  uint32_t msg_type;
  int16_t service_index;
  struct csd_q6atl_apr_service_obj* apr_client = NULL;
  struct csd_list_node* node;

  msg_type = APR_GET_FIELD(APRV2_PKT_MSGTYPE, packet->header);
  if(q6atl_main_data->lock)
  {
    csd_lock_enter(q6atl_main_data->lock);
  }
  else
  {
    CSD_MSG_ARG_0(CSD_PRIO_ERROR,
       "CSD:ATL => lock not initialized \n");
    return rc;
  }

  service_index = csd_atl_get_service_index(packet->dst_addr);
  if(service_index < 0)
  {
    CSD_MSG_ARG_1(CSD_PRIO_ERROR,
        "CSD:ATL => csd_atl_apr_cb - invalid address 0x%x\n",packet->dst_addr);
    rc = CSD_EBADPARAM;
    goto err;  /* APR free packet in error case */
  }
  apr_client = &q6atl_main_data->apr_service_obj[service_index];

  if(msg_type == Q6MSG_TYPE_RSP)
  {
    /* for response msg we need to check the sync send list */
    struct q6atl_sync_msg_key key;
    uint32_t rsp_len = APRV2_PKT_GET_PACKET_BYTE_SIZE(packet->header);

    key.client_token = packet->token;
    key.port = packet->dst_port;
    key.addr = packet->dst_addr;
    CSD_MSG_ARG_6(CSD_PRIO_HIGH,
        "CSD:ATL => csd_atl_apr_cb - sync rsp msg detail:src_addr=0x%x,src_port=0x%x,dst_addr=0x%x,dst_port=0x%x,opcode=0x%x,token=0x%x\n",
    packet->src_addr,packet->src_port,key.addr,key.port,packet->opcode,key.client_token);

    csd_list_find(&q6atl_main_data->used_sync_trans_list, &node, (uint32_t)&key);
    if(node)
    {
      struct csd_q6atl_sync_transaction* trans_obj = (struct csd_q6atl_sync_transaction*)node->data;
      struct q6msg* msg = trans_obj->rsp_buf;
      uint8_t* tmp = (uint8_t*) packet;
      uint32_t size = Q6MSG_GET_HDR_SIZE(*((uint32_t*)packet));
      uint32_t hdr_size = sizeof(msg->hdr);
	  
      csd_list_del(&q6atl_main_data->used_sync_trans_list, node);
      csd_memscpy(&msg->hdr, hdr_size, tmp, Q6MSG_PACKED_HEADER_SIZE);
      tmp += size;
      csd_memscpy(&msg->u, rsp_len-hdr_size, tmp, rsp_len-size);
      *trans_obj->rsp_buf_len = rsp_len -(size-Q6MSG_PACKED_HEADER_SIZE);
       CSD_MSG_ARG_3(CSD_PRIO_MEDIUM,
       "CSD:ATL => csd_atl_apr_cb - find sync send trans_match(local_addr=0x%x,local_port=0x%x,cmd_token=0x%x)\n",
       trans_obj->addr,trans_obj->port,trans_obj->apr_token);
      csd_event_signal(trans_obj->sync_event_handle);
      goto end;
    }
	else
	{
	  CSD_MSG_ARG_1(CSD_PRIO_ERROR, "csd_atl_apr_cb: Unable to find a node with matching token[0x%x]\n", key.client_token);
	  rc = CSD_EBADPARAM; 
	  goto err;   
	}
  }
  else
    CSD_MSG_ARG_1(CSD_PRIO_ERROR, "csd_atl_apr_cb: Unknown msg type[%d]\n", msg_type);
   /* this is async msg */
  if(dispatch_data)
  {
    if(dispatch_data != (void*)apr_client)
    {
      /* APRV2 did not return correct dispatch_data and need fix */
      CSD_MSG_ARG_3(CSD_PRIO_ERROR,
          "CSD:ATL => csd_atl_apr_cb - service_idx=%d,dispatch_data 0x%x != 0x%x\n",
      service_index,dispatch_data, apr_client);
    }
    //apr_client = (struct csd_q6atl_apr_service_obj*)dispatch_data;
    csd_list_find(&apr_client->used_session_list, &node, (uint32_t)packet->dst_port);
    if(node)
    {
      struct csd_q6atl_session_obj* client = (struct csd_q6atl_session_obj*)node->data;
      struct q6msg msg;
      uint8_t* tmp = (uint8_t*) packet;
      uint32_t size = Q6MSG_GET_HDR_SIZE(*((uint32_t*)packet));
      uint32_t rsp_len = APRV2_PKT_GET_PACKET_BYTE_SIZE(packet->header);
      uint32_t hdr_size = sizeof(msg.hdr);

      csd_memscpy(&(msg.hdr), hdr_size, tmp, Q6MSG_PACKED_HEADER_SIZE);
      tmp += size;
      csd_memscpy(&(msg.u), rsp_len-hdr_size, tmp, rsp_len-size);

      CSD_MSG_ARG_4(CSD_PRIO_MEDIUM,
          "CSD:ATL => apr_cb - async Q6 msg for msg_type=%d,dst_addr=0x%x,dst_port=0x%x,apr_token=0x%x\n",
          msg_type,packet->dst_addr, packet->dst_port,packet->token);
      (client->rx_cb)((void *)&msg, APRV2_PKT_GET_PACKET_BYTE_SIZE(packet->header)-(size-Q6MSG_PACKED_HEADER_SIZE), client->rx_cb_data);
    }
    else
    {
     CSD_MSG_ARG_1(CSD_PRIO_ERROR,
        "csd_atl_apr_cb: No client registered to receive this packet, dst_addr=0x%x\n",packet->dst_addr);
	 rc = CSD_EBADPARAM; 
     goto err;
    }
  }
  else
  {
     CSD_MSG_ARG_0(CSD_PRIO_ERROR,
       "CSD:ATL => apr_cb - error. dispatch_data is null\n");
     rc = CSD_EBADPARAM;
     goto err;
  }
end:
  __aprv2_cmd_free(apr_client->apr_handle,packet);
err:
  if(q6atl_main_data->lock)
  {
    csd_lock_leave(q6atl_main_data->lock);
  }
  /* interestingly that atl needs to free the APR packet in success case.
   * APR will free the packet in error scenarios.
   */
  return rc;
}
/*===========================================================================
FUNCTION csd_atl_malloc

DESCRIPTION

RETURN VALUE
   None

SIDE EFFECTS
===========================================================================*/
static void* csd_atl_malloc(uint32_t size)
{
  void* tmp;
  csd_malloc(size, (void**)(&tmp));
  if( tmp == NULL )
  {
    CSD_MSG_ARG_1(CSD_PRIO_ERROR,
        "CSD:ATL => malloc failed (size=%d)\n",size);
    return NULL;
  }
  csd_memset(tmp, 0, size);
  return tmp;
}
/*===========================================================================
FUNCTION csd_atl_client_list_find

DESCRIPTION

RETURN VALUE
   None

SIDE EFFECTS
===========================================================================*/
static bool_t csd_atl_client_list_find(struct csd_list_node* node, uint32_t port)
{
  struct csd_q6atl_session_obj* client = (struct csd_q6atl_session_obj*)node;
  if((uint32_t)client->port == port)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}
/*===========================================================================
FUNCTION csd_atl_client_list_find

DESCRIPTION

RETURN VALUE
   None

SIDE EFFECTS
===========================================================================*/
static bool_t csd_atl_trans_list_find(struct csd_list_node* node, uint32_t data)
{
  struct q6atl_sync_msg_key* key = (struct q6atl_sync_msg_key*)data;
  struct csd_q6atl_sync_transaction* trans = (struct csd_q6atl_sync_transaction*)node;
  if( (trans->addr == key->addr) &&
      (trans->apr_token == key->client_token) &&
      (trans->port == key->port))
  {
    return TRUE;
  }

  return FALSE;
}
/*===========================================================================
FUNCTION csd_atl_apr_service_init

DESCRIPTION

RETURN VALUE
   None

SIDE EFFECTS
===========================================================================*/
int32_t csd_atl_apr_service_init()
{
  uint16_t i;
  int32_t rc = CSD_EOK;
  uint32_t apr_rc = APR_EOK;
  struct aprv2_cmd_register_t reg;

  reg.dispatch_fn = csd_atl_apr_cb;
  for(i = 0; i < Q6ATL_APR_SERVICES_MAX_NUM; i++)
  {
    reg.ret_handle = &q6atl_main_data->apr_service_obj[i].apr_handle;
    reg.addr = (AUDIO_DOMIAN << 8) + csd_atl_get_service(i);
    reg.dispatch_data = &q6atl_main_data->apr_service_obj[i];
    apr_rc = apr_call(APRV2_CMDID_REGISTER, &reg, sizeof(reg));
    if(apr_rc != APR_EOK)
    {
      CSD_MSG_ARG_1(CSD_PRIO_ERROR,
           "CSD:ATL => APR callback registration failed. addr=0x%x\n",
           csd_atl_get_service(i)*(AUDIO_DOMIAN<<8));
      rc = csd_map_apr_error_to_csd( apr_rc );
      goto end;
    }
    CSD_MSG_ARG_2(CSD_PRIO_MEDIUM,
        "CSD:ATL => csd_atl_apr_service_init - service_idx=%d,dispatch_data=0x%x\n",
        i,reg.dispatch_data);

    q6atl_main_data->apr_service_obj[i].used = FALSE;
    rc = csd_list_init_key(&q6atl_main_data->apr_service_obj[i].used_session_list,
         csd_atl_client_list_find);
    if(rc != CSD_EOK)
    {
      CSD_MSG_ARG_2(CSD_PRIO_ERROR,
           "CSD:ATL => client list init failed. index=%d\n rc=0x%x", i, rc);
      goto end;
    }
  }
end:
  return rc;
}
/*===========================================================================
FUNCTION csd_atl_get_free_trans_obj

DESCRIPTION

RETURN VALUE
   None

SIDE EFFECTS
===========================================================================*/
struct csd_q6atl_sync_transaction* csd_atl_get_free_trans_obj()
{
  struct csd_q6atl_sync_transaction* trans_obj;
  struct csd_list_node* node = NULL;

  csd_list_get_head(&q6atl_main_data->free_sync_trans_list, &node);
  if(!node)
  {
    CSD_MSG_ARG_0(CSD_PRIO_ERROR,
        "CSD:ATL => no free trans_obj available\n");
    return NULL;
  }
  trans_obj = (struct csd_q6atl_sync_transaction*)node->data;
  return trans_obj;
}
/*===========================================================================
FUNCTION csd_atl_put_free_trans_obj

DESCRIPTION

RETURN VALUE
   None

SIDE EFFECTS
===========================================================================*/
void csd_atl_put_free_trans_obj(struct csd_q6atl_sync_transaction* trans_obj)
{
  csd_list_add_tail(&q6atl_main_data->free_sync_trans_list, &trans_obj->node);
}
/*===========================================================================
FUNCTION csd_atl_trans_init

DESCRIPTION

RETURN VALUE
   None

SIDE EFFECTS
===========================================================================*/
int32_t csd_atl_trans_init()
{
  uint16_t i;
  int32_t rc = CSD_EOK;
  struct csd_q6atl_sync_transaction* trans_obj;

  rc = csd_list_init_key(&q6atl_main_data->used_sync_trans_list,
       csd_atl_trans_list_find);
  if(rc != CSD_EOK)
  {
    CSD_MSG_ARG_1(CSD_PRIO_ERROR,
       "CSD:ATL => transaction list init failed\n. rc=0x%x", rc);
    goto end;
  }
  csd_list_init(&q6atl_main_data->free_sync_trans_list);

  for(i = 0; i < Q6ATL_Q6_SYNC_TRANS_MAX_NUM; i++)
  {
    trans_obj = (struct csd_q6atl_sync_transaction*)csd_atl_malloc(sizeof(struct csd_q6atl_sync_transaction));
    if(trans_obj == NULL)
    {
        /* no mem */
      CSD_MSG_ARG_0(CSD_PRIO_ERROR,
        "CSD:ATL => no mem for init trans\n");
      rc = CSD_ENORESOURCE;
      goto end;
    }
    if( CSD_OSAL_EOK != csd_event_create(&(trans_obj->sync_event_handle)))
    {
      CSD_MSG_ARG_0(CSD_PRIO_ERROR,
         "CSD:ATL => cannot create sync event\n");
      csd_free(trans_obj);
      goto end;
    }
    csd_list_add(&q6atl_main_data->free_sync_trans_list, &trans_obj->node, trans_obj);
  }
end:
  return rc;
}
/*===========================================================================
FUNCTION csd_atl_init

DESCRIPTION

RETURN VALUE
   None

SIDE EFFECTS
===========================================================================*/
int32_t csd_atl_init()
{
  int32_t rc = CSD_EOK;
  if(q6atl_main_data)
  {
    CSD_MSG_ARG_0(CSD_PRIO_MEDIUM,
       "CSD:ATL => already initialized. nop\n");
    return CSD_EOK;
  }
  q6atl_main_data = (struct csd_q6atl*)csd_atl_malloc(sizeof(struct csd_q6atl));
  if(!q6atl_main_data)
  {
    CSD_MSG_ARG_0(CSD_PRIO_ERROR,
       "CSD:ATL => main data malloc failed\n");
    rc = CSD_ENORESOURCE;
   // CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,rc,CSD_ATL_INIT_STAGE,
     //         CSD_INIT_ATL_MALLOC,0,0); 
    goto end;
  }
  /*csd atl malloc success*/
  //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,rc,CSD_ATL_INIT_STAGE,
    //                 CSD_INIT_ATL_MALLOC,0,0); 
  rc = csd_atl_apr_service_init();
  if(rc != CSD_EOK)
  {
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,rc,CSD_ATL_INIT_STAGE,
      //                 CSD_INIT_ATL_APR_SERV_INIT,0,0); 
    goto end;
  }
  /*csd atl APR services init success*/
  //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,rc,CSD_ATL_INIT_STAGE,
    //                 CSD_INIT_ATL_APR_SERV_INIT,0,0); 
  if( CSD_OSAL_EOK != csd_lock_create( &q6atl_main_data->lock ))
  {
    CSD_MSG_ARG_0(CSD_PRIO_ERROR,
       "CSD:ATL => lock creation failed\n");
    rc = CSD_ENORESOURCE;
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,rc,CSD_ATL_INIT_STAGE,
      //                  CSD_INIT_ATL_RESOURCE_CREATE,0,0); 
    goto end;
  }
  /*csd atl APR resource create success*/
 
  //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,rc,CSD_ATL_INIT_STAGE,
    //                 CSD_INIT_ATL_RESOURCE_CREATE,0,0); 
  rc = csd_atl_trans_init();
  if(rc != CSD_EOK)
  {
    CSD_MSG_ARG_1(CSD_PRIO_ERROR,
       "CSD:ATL => transaction list init failed\n, rc=0x%x", rc);
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,rc,CSD_ATL_INIT_STAGE,
      //                 CSD_INIT_ATL_TRANS_INIT,0,0); 
    goto end;
  }
  /*csd atl trans init success*/
   //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,rc,CSD_ATL_INIT_STAGE,
     //                 CSD_INIT_ATL_TRANS_INIT,0,0);
  q6atl_main_data->sync_send_timeout = CSD_Q6ATL_TIMEOUT_MICRO_SEC;
end:
  if(CSD_EOK != rc)	 	 
  {	 	 
    CSD_MSG_ARG_1(CSD_PRIO_ERROR,"CSD:ATL => init failed with Err=[0x%x]: Continue with DeInit and cleanup internal resources \n", rc);	 	 
    csd_atl_dinit();	 	 
  }
  return rc;
}
int32_t csd_atl_trans_deinit()
{
  struct csd_q6atl_sync_transaction* trans_obj;
  struct csd_list_node* node = NULL;

  csd_list_get_head(&q6atl_main_data->used_sync_trans_list, &node);
  while(node)
  {
    csd_list_del(&q6atl_main_data->used_sync_trans_list, node);
    trans_obj = (struct csd_q6atl_sync_transaction*)node->data;
    if(trans_obj->sync_event_handle)
    {
      csd_event_destroy(trans_obj->sync_event_handle);
    }
    csd_free(trans_obj);
    node = NULL;
    csd_list_get_head(&q6atl_main_data->used_sync_trans_list, &node);
  }
  csd_list_get_head(&q6atl_main_data->free_sync_trans_list, &node);
  while(node)
  {
    csd_list_del(&q6atl_main_data->used_sync_trans_list, node);
    trans_obj = (struct csd_q6atl_sync_transaction*)node->data;
    if(trans_obj->sync_event_handle)
    {
      csd_event_destroy(trans_obj->sync_event_handle);
    }
    csd_free(trans_obj);
    node = NULL;
    csd_list_get_head(&q6atl_main_data->free_sync_trans_list, &node);
  }
  csd_list_dinit(&q6atl_main_data->free_sync_trans_list);
  csd_list_dinit(&q6atl_main_data->used_sync_trans_list);
  return CSD_EOK;
}
/*===========================================================================
FUNCTION csd_atl_apr_service_deinit

DESCRIPTION

RETURN VALUE
   None

SIDE EFFECTS
===========================================================================*/
int32_t csd_atl_apr_service_deinit()
{
  uint16_t i;
  int32_t rc = CSD_EOK;
  uint32_t apr_rc = APR_EOK;

  for(i = 0; i < Q6ATL_APR_SERVICES_MAX_NUM; i++)
  {
    if(q6atl_main_data->apr_service_obj[i].apr_handle)
    {
      struct aprv2_cmd_deregister_t dereg;
      dereg.handle = q6atl_main_data->apr_service_obj[i].apr_handle;
      apr_rc = apr_call(APRV2_CMDID_DEREGISTER, &dereg, sizeof(dereg));
      q6atl_main_data->apr_service_obj[i].apr_handle = 0;
      if(apr_rc != APR_EOK)
      {
        rc = csd_map_apr_error_to_csd( apr_rc );
      }
    }
    q6atl_main_data->apr_service_obj[i].used = FALSE;
    rc = csd_list_dinit(&q6atl_main_data->apr_service_obj[i].used_session_list);
    if(rc != CSD_EOK)
    {
      CSD_MSG_ARG_2(CSD_PRIO_ERROR,
            "CSD:ATL => cannot deinit list for addr=0x%x\n, rc=0x%x\n",
            csd_atl_get_service(i)*(AUDIO_DOMIAN<<8), rc );
      goto end;
    }
  }
end:
  return rc;
}
/*===========================================================================
FUNCTION csd_atl_deinit

DESCRIPTION

RETURN VALUE
   None

SIDE EFFECTS
===========================================================================*/
int32_t csd_atl_dinit(void)
{
  CSD_MSG_ARG_0(CSD_PRIO_MEDIUM,
      "CSD:ATL => csd_atl_dinit() called\n");

  if(!q6atl_main_data)
  {
    CSD_MSG_ARG_0(CSD_PRIO_MEDIUM,
        "CSD:ATL => not initialized yet. csd_atl_dinit() is nop\n");
    return 0;
  }
  csd_atl_trans_deinit();
  /*csd atl trans deinit success*/
  //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,CSD_EOK,CSD_ATL_INIT_STAGE,
    //                 CSD_INIT_ATL_TRANS_INIT,0,0); 
  csd_atl_apr_service_deinit();
  /*csd atl apr services  deinit success*/
  //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,CSD_EOK,CSD_ATL_INIT_STAGE,
	//			  CSD_INIT_ATL_APR_SERV_INIT,0,0); 
  if(q6atl_main_data->lock)
  {
    csd_lock_destroy(q6atl_main_data->lock);
    q6atl_main_data->lock = NULL;
    /*csd atl resource distroy object success*/
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,CSD_EOK,CSD_ATL_INIT_STAGE,
      //                 CSD_INIT_ATL_RESOURCE_CREATE,0,0); 
   }

  csd_free(q6atl_main_data);
  /*csd atl malloc free success*/
  //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,CSD_EOK,CSD_ATL_INIT_STAGE,
        //             CSD_INIT_ATL_MALLOC,0,0); 
  q6atl_main_data = NULL;
  return CSD_EOK;
}
/*===========================================================================
FUNCTION csd_atl_async_reg

DESCRIPTION

RETURN VALUE
   None

SIDE EFFECTS
===========================================================================*/
int32_t csd_atl_async_reg(
    uint16_t addr,
    uint16_t port,            /* port */
    csd_q6atl_session_listener_type cb,   /* msg notifying cb */
    void* cb_data             /* msg cb user data */
    )
{
  struct csd_q6atl_apr_service_obj* apr_client = NULL;
  struct csd_q6atl_session_obj* client;
  int16_t service_index = csd_atl_get_service_index(addr);

  CSD_MSG_ARG_3(CSD_PRIO_MEDIUM,
      "CSD:ATL => csd_atl_async_reg - addr=0x%x,port=0x%x,client_data=0x%x\n",addr,port,cb_data);

  if(service_index < 0)
  {
    CSD_MSG_ARG_2(CSD_PRIO_ERROR,
        "CSD:ATL => q6atl_async_reg failed. invalid param.addr=0x%x,port=0x%x\n",
        addr, port);
    return CSD_EBADPARAM;
  }
  apr_client = &q6atl_main_data->apr_service_obj[service_index];
  client = (struct csd_q6atl_session_obj*)csd_atl_malloc(sizeof(struct csd_q6atl_session_obj));
  if(!client)
  {
    CSD_MSG_ARG_2(CSD_PRIO_ERROR,
        "CSD:ATL => q6atl_async_reg failed. No mem for addr=0x%x,port=0x%x\n",
        addr, port);
    /* no mem too bad */
    return CSD_ENORESOURCE;
  }
  client->port = port;
  client->rx_cb = cb;
  client->rx_cb_data = cb_data;
  csd_list_add(&apr_client->used_session_list,
      &client->node, (void*)client);
  return CSD_EOK;
}
/*===========================================================================
FUNCTION q6atl_async_dereg

DESCRIPTION

RETURN VALUE
   None

SIDE EFFECTS
===========================================================================*/
int32_t csd_atl_async_dereg(
    uint16_t addr,
    uint16_t port
    )
{
  struct csd_q6atl_apr_service_obj* apr_client = NULL;
  uint32_t key = port;
  int32_t rc = CSD_EOK;
  struct csd_list_node* node;
  int16_t service_index = csd_atl_get_service_index(addr);

  CSD_MSG_ARG_2(CSD_PRIO_MEDIUM,
      "CSD:ATL => csd_atl_async_dereg - addr=0x%x,port=0x%x\n",addr,port);

  if(service_index < 0)
  {
    CSD_MSG_ARG_2(CSD_PRIO_ERROR,
        "CSD:ATL => q6atl_async_dereg - invalid params - addr=0x%x,port=0x%x\n",
        addr, port);
    return CSD_EBADPARAM;
  }
  apr_client = &q6atl_main_data->apr_service_obj[service_index];
  csd_list_find(&apr_client->used_session_list, &node, key);
  if(!node)
  {
    CSD_MSG_ARG_2(CSD_PRIO_ERROR,
        "CSD:ATL => q6atl_async_dereg failed. Cannot find the client with addr=0x%x,port=0x%x\n",
        addr, port);
    rc = CSD_ENOTFOUND;
    goto end;
  }
  csd_list_del(&apr_client->used_session_list, node);
  csd_free(node->data);
end:
  return rc;
}
/*===========================================================================
FUNCTION csd_atl_send_buf

DESCRIPTION

RETURN VALUE
   None

SIDE EFFECTS
===========================================================================*/
static int32_t csd_atl_send_buf(
    uint16_t addr,/* registered service index */
    void* buf)
{
  struct aprv2_cmd_alloc_send_t apr_alloc_send;
  aprv2_packet_t* pkt = (aprv2_packet_t*)buf;
  int16_t service_index = csd_atl_get_service_index(addr);
  int32_t rc = CSD_EOK;
  uint32_t apr_rc = APR_EOK;
  if(service_index < 0)
  {
    CSD_MSG_ARG_1(CSD_PRIO_ERROR,
        "CSD:ATL => q6atl_send_buf failed. invalid addr=0x%x\n",
        addr);
    rc = CSD_EBADPARAM;
    goto end;
  }
  apr_alloc_send.handle = q6atl_main_data->apr_service_obj[service_index].apr_handle;
  apr_alloc_send.msg_type = APR_GET_FIELD(APRV2_PKT_MSGTYPE, pkt->header);
  apr_alloc_send.src_addr = pkt->src_addr;
  apr_alloc_send.src_port = pkt->src_port;
  apr_alloc_send.dst_addr = pkt->dst_addr;
  apr_alloc_send.dst_port = pkt->dst_port;
  apr_alloc_send.token = pkt->token;
  apr_alloc_send.opcode = pkt->opcode;
  apr_alloc_send.payload = &((struct q6msg*)pkt)->u;
  apr_alloc_send.payload_size = APRV2_PKT_GET_PAYLOAD_BYTE_SIZE(pkt->header);
  apr_rc = apr_call(APRV2_CMDID_ALLOC_SEND,&apr_alloc_send,sizeof(apr_alloc_send));
  if(apr_rc != APR_EOK)
  {
    rc = csd_map_apr_error_to_csd( apr_rc );
  }
  end:
  return rc;
}
/*===========================================================================
FUNCTION csd_atl_send

DESCRIPTION

RETURN VALUE
   None

SIDE EFFECTS
===========================================================================*/
int32_t csd_atl_send(
    void* send_buf,        /* sending buf ptr */
    void* rsp_buf,         /* rsp buf ptr for sync send only */
    uint32_t* rsp_buf_len, /* rsp buf length in bytes */
    bool_t is_sync         /* 1 = sync send, 0 = async send */
)
{
  int32_t rc = CSD_EOK;
  struct csd_q6atl_sync_transaction* trans = NULL;

  CSD_MSG_ARG_6(CSD_PRIO_MEDIUM,
      "CSD:ATL => csd_atl_send enter - src_addr=0x%x,src_port=0x%x,sync=%d,packet_size=0x%x,pkt_token=0x%x,op=0x%x\n",
      ((struct aprv2_packet_t*)send_buf)->src_addr, ((struct aprv2_packet_t*)send_buf)->src_port,is_sync,
      APRV2_PKT_GET_PACKET_BYTE_SIZE(((struct aprv2_packet_t*)send_buf)->header),
      ((struct aprv2_packet_t*)send_buf)->token, ((struct aprv2_packet_t*)send_buf)->opcode);

  /* async send is simple */
  if(TRUE != is_sync)
  {
    CSD_MSG_ARG_6(CSD_PRIO_HIGH,
        "CSD:ATL => async send. src_addr=0x%x,src_port=0x%x,dst_addr=0x%x,dst_port=0x%x,op=0x%x,token=0x%x\n",
        ((struct aprv2_packet_t*)send_buf)->src_addr,((struct aprv2_packet_t*)send_buf)->src_port,
        ((struct aprv2_packet_t*)send_buf)->dst_addr,((struct aprv2_packet_t*)send_buf)->dst_port,
        ((struct aprv2_packet_t*)send_buf)->opcode,((struct aprv2_packet_t*)send_buf)->token);
    rc = csd_atl_send_buf(((struct aprv2_packet_t*)send_buf)->src_addr,send_buf);
    goto end;
  }
  /* for sync send, trans obj is created first */
  trans = csd_atl_get_free_trans_obj();
  if(!trans)
  {
  /* no mem  */
    CSD_MSG_ARG_2(CSD_PRIO_ERROR,
        "CSD:ATL => q6atl_send-trans_obj malloc failed. addr=0x%x,port=0x%x\n",
        ((struct aprv2_packet_t*)send_buf)->src_addr,
        ((struct aprv2_packet_t*)send_buf)->src_port);
    return CSD_ENORESOURCE;
  }
  /* save trans info */
  trans->port = ((struct aprv2_packet_t*)send_buf)->src_port;
  trans->addr = ((struct aprv2_packet_t*)send_buf)->src_addr;
  trans->apr_token = ((struct aprv2_packet_t*)send_buf)->token;
  trans->rsp_buf_len = rsp_buf_len;
  trans->rsp_buf = rsp_buf;
  CSD_MSG_ARG_6(CSD_PRIO_MEDIUM,
      "CSD:ATL => sync send. src_addr=0x%x,src_port=0x%x,dst_addr=0x%x,dst_port=0x%x,op=0x%x,token=0x%x\n",
      trans->addr,trans->port,
      ((struct aprv2_packet_t*)send_buf)->dst_addr,((struct aprv2_packet_t*)send_buf)->dst_port,
      ((struct aprv2_packet_t*)send_buf)->opcode,trans->apr_token);
  rc = csd_list_add(&q6atl_main_data->used_sync_trans_list,
  &trans->node, (void*)trans);

  rc = csd_atl_send_buf(((struct aprv2_packet_t*)send_buf)->src_addr, send_buf);
  if(rc != CSD_EOK)
  {
    /* error in sending */
    CSD_MSG_ARG_4(CSD_DIAG_PRIO_ERROR,
        "CSD:ATL => csd_atl_send - sending error - src_addr=0x%x,src_port=0x%x,token=0x%x,rc=0x%x\n",
        trans->addr, trans->port, trans->apr_token, rc);
    csd_list_del( &q6atl_main_data->used_sync_trans_list, &trans->node);
    csd_atl_put_free_trans_obj(trans);
    goto end;
  }
  CSD_MSG_ARG_3(CSD_DIAG_PRIO_MEDIUM,
      "CSD:ATL => csd_atl_send sent and waiting for rsp - src_addr=0x%x,src_port=0x%x,token=0x%x\n",
      trans->addr, trans->port, trans->apr_token);

  /* wait till rsp come back from Q6 */
  rc = csd_event_wait_timed(trans->sync_event_handle, q6atl_main_data->sync_send_timeout);
  if(CSD_ETIMEOUT == rc)
  {
    /* timeout */
    csd_list_del(&q6atl_main_data->used_sync_trans_list, &trans->node);
  }
  CSD_MSG_ARG_4(CSD_DIAG_PRIO_MEDIUM,
      "CSD:ATL => csd_atl_send leave - src_addr=0x%x,src_port=0x%x,token=0x%x,rc=0x%x\n",
      trans->addr, trans->port, trans->apr_token, rc);

  csd_atl_put_free_trans_obj(trans);
end:
  return rc;
}


