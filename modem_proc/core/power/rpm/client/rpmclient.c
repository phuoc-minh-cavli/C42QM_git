/*===========================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated.  
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.
  7/7/2014 Added it for VCE++

  ===========================================================================*/

#ifdef _MSC_VER
    // enable memory leak detection under msvc
    #define _CRTDBG_MAP_ALLOC
    #include <stdlib.h>
    #include <crtdbg.h>
#endif

#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#include "inmap.h"
#include "npa_resource.h"

#include "rpmclient.h"
#include "rpm_mq.h"
#include "rpm_resource.h"
#include "rpm_wire.h"

#include "DALSys.h"
#include "err.h"
#include "ULogFront.h"
#include "CoreVerify.h"
#include "glink.h"
#include "glink_rpm.h"
#include "stringl.h"
//#include "rpm_fake_sleep.h"
#define MSG_BUFFER_SIZE 64
//This limit is put in place because when sleep forces the NAS and sleep set across, 
//there is a finite amount of space for the acks. If we reach this limit, we need to churn.
#define SLEEP_PLUS_NAS_ACK_LIMIT 24

typedef struct
{
    inmap_link_t         link;
    unsigned             service_id;
    rpmclient_service_cb cb;
    void                *context;
} rpmclient_service_t;

typedef enum
{
  ACTIVE = 0,
  ENTERING_FAKE_SLEEP,
  FAKE_SLEEP,
  EXITING_FAKE_SLEEP,
} soft_sleep_state_t;

typedef struct
{
  uint64_t xo_accumulated_duration;
  uint64_t shutdown_timestamp;     
  uint64_t wakeup_timestamp;
  uint32_t counter_reset;   
} rpm_xo_drift_compensation_t;


typedef struct
{
    char     *buffer;
    unsigned size;
} raw_packet;
typedef struct
{
    rpm_service_type service; 
    unsigned         total_size;
    unsigned         msg_id;
    unsigned         set;
    unsigned         resource_type;
    unsigned         resource_id;
    unsigned         data_len;
} NASPacket;
static struct rpm_s
{
    unsigned          rpmclient_initialized;
    rpm_mq_t         *rpm_mq;

    unsigned          num_resources;
    rpm_resource_t  **resources;

    inmap_t          *services;

    glink_handle_type  glink_port;
    glink_link_handle_type  glink_link;
    kvp_t            *rx_kvp;
	unsigned          num_resources_dirty[RPM_NUM_SETS];
	soft_sleep_state_t soft_sleep_state;
    char              *msg_buffer;   
    unsigned          msg_buffer_size;   
    raw_packet        nas_bundle;
	bool              rpm_is_up;
} rpm;

DALSYSSyncHandle rpm_lock;
DALSYSEventHandle     rpm_sync_event; 
DALSYSEventObj        rpm_sync_event_memory;
ULogHandle       rpm_log;
extern uint32 vmpm_xo_drift_base_VA;

static const char *rpm_set_names[RPM_NUM_SETS] =
{
  "active",
  "sleep",
  "next active"
};

void rpm_mask_interrupt(bool mask)
{
    CORE_VERIFY(GLINK_STATUS_SUCCESS == glink_rpm_mask_rx_interrupt(rpm.glink_port, mask, NULL));
}

void (*rpm_fifo_full_handler)(rpm_mq_t *, unsigned);

rpm_resource_t *rpm_find_resource(rpm_resource_type resource, unsigned id)
{
    uint64_t resource_id = (((uint64_t)resource) << 32) | id;
    unsigned low = 0, high = rpm.num_resources - 1;

    if(!rpm.num_resources)
        return NULL;

    while(high < rpm.num_resources && high >= low)
    {
        unsigned mid = (low + high) / 2;
        uint64_t current_id = rpm.resources[mid]->resource_identifier;

        if(current_id < resource_id)
            low = mid + 1;
        else if(current_id > resource_id)
            high = mid - 1;
        else
            return rpm.resources[mid];
    }

    return NULL;
}


static void rpmclient_glink_tx(char * buffer, unsigned size)
{
    glink_err_type send_err;	
    do
    {
        //TODO: check if the RX fifo is filling up. If so churn a message	
        //if there's an error, keep trying to send until there is room (i.e. until the RPM has opened up space)
        send_err = glink_tx(rpm.glink_port, NULL, buffer, size, false);
    }while(send_err == GLINK_STATUS_OUT_OF_RESOURCES);

    CORE_VERIFY(GLINK_STATUS_SUCCESS == send_err);
}
/*
static void rpmclient_put_cmd(RPMCommandPacket *msg)
{
    unsigned raw_buffer_size;
    DALSYS_SyncEnter(rpm_lock);

    // raw_buffer_size = packet size + data size - kvp_ptr since there is no ptr in the raw buffer    
    raw_buffer_size = ((sizeof(RPMCommandPacket) + msg->data_len) - sizeof(unsigned));

    CORE_VERIFY(rpm.msg_buffer_size >= raw_buffer_size);
    //copy header into buffer    
    memscpy(rpm.msg_buffer, sizeof(RPMCommandPacket), msg, sizeof(RPMCommandPacket));
    //copy data into buffer, back off one unsigned since we don't have a kvp ptr as is in the header  
    memscpy(((rpm.msg_buffer+sizeof(RPMCommandPacket) - sizeof(unsigned))), msg->data_len, msg->kvp_buffer, msg->data_len);

    rpmclient_glink_tx(rpm.msg_buffer, raw_buffer_size);

    DALSYS_SyncLeave(rpm_lock);
}
*/

rpm_resource_t *rpm_add_resource(rpm_resource_type resource, unsigned id)
{
    unsigned i, old_num_resources;
    uint64_t resource_id = (((uint64_t)resource) << 32) | id;
    rpm_resource_t *new_resource;

    new_resource = (rpm_resource_t *)malloc(sizeof(rpm_resource_t));
    CORE_VERIFY_PTR(new_resource);
    memset(new_resource, 0, sizeof(rpm_resource_t));
    rpm_resource_init(new_resource, resource, id);

    old_num_resources = rpm.num_resources;
    rpm.num_resources++;
    rpm.resources = realloc(rpm.resources, rpm.num_resources * sizeof(rpm_resource_t *));
    CORE_VERIFY_PTR(rpm.resources);

    CORE_VERIFY_PTR(rpm.resources);
    for(i = 0; i < old_num_resources; ++i)
    {
        rpm_resource_t *r = rpm.resources[i];

        CORE_VERIFY_PTR(r);
        if(r->resource_identifier > resource_id)
        {
            // Found entries that belong after us.  Move them and insert here.
            memmove(&rpm.resources[i+1], &rpm.resources[i], (old_num_resources - i) * sizeof(rpm_resource_t *));

            break;
        }
    }

    // Must be largest in the list; insert at the end.
    rpm.resources[i] = new_resource;

    // Refactor the queues to update any resource pointers in pending messages
    rpm_mq_update_resource_pointers(rpm.rpm_mq);

    return new_resource;
}

unsigned rpm_get_request_size(rpm_set_type set, rpm_resource_type resource, unsigned id)
{
    rpm_resource_t *resource_data; 
	
	if(set >= RPM_NUM_SETS)
        return 0;
		
	resource_data = rpm_find_resource(resource, id);
	
    if(!resource_data)
        return 0;

    return rpm_resource_get_request_size(set, resource_data);
}

void rpm_barrier(unsigned message_id)
{
    DALSYS_SyncEnter(rpm_lock);

    ULOG_RT_PRINTF_1(rpm_log, "rpm_barrier (msg_id: 0x%08x)", message_id);
    rpm_mq_wait(rpm.rpm_mq, message_id);
    ULOG_RT_PRINTF_1(rpm_log, "\trpm_barrier_return (msg_id: 0x%08x)", message_id);

    DALSYS_SyncLeave(rpm_lock);
}
static void rpmclient_force_nas(void)
{
    rpmclient_glink_tx(rpm.nas_bundle.buffer, rpm.nas_bundle.size);

    free(rpm.nas_bundle.buffer);
    rpm.nas_bundle.buffer = NULL;
    rpm.nas_bundle.size = 0;
}

static void rpm_churn_queue_internal(unsigned message_id)
{
    bool churn_log = false;	

    ULOG_RT_PRINTF_1(rpm_log, "rpm_churn_queue (msg_id: 0x%08x)", message_id);

    while(rpm_mq_is_in_flight(rpm.rpm_mq, message_id))
    {
        	    
        if(!churn_log)	    
        {
            ULOG_RT_PRINTF_1(rpm_log, "\tchurning (msg_id: 0x%08x)", message_id);
            churn_log = true;	    
        }
        DALSYS_SyncLeave(rpm_lock);
        CORE_VERIFY(GLINK_STATUS_SUCCESS == glink_rpm_rx_poll(rpm.glink_port));
        DALSYS_SyncEnter(rpm_lock);
    }

    ULOG_RT_PRINTF_1(rpm_log, "rpm_churning_complete (msg_id: 0x%08x)", message_id);
}

unsigned rpm_force_sync(rpm_set_type set)
{
    unsigned last_msg_id = 0;

    if(set >= RPM_NUM_SETS)
        return 0;

    DALSYS_SyncEnter(rpm_lock);

    ULOG_RT_PRINTF_4(rpm_log, "rpm_force_sync (set: %d) (dirty: %d,%d,%d)", set, rpm.num_resources_dirty[0], rpm.num_resources_dirty[1], rpm.num_resources_dirty[2]);

    /*Below code is added to clear message ram in case if there is no space for NAS messages. 
	This churning before sending NAS is inducing unnecessary delays hence commented*/
    #if 0
    if((set == RPM_NEXT_ACTIVE_SET) && (rpm.num_resources_dirty[set] > 0))
    {
	    unsigned oldest_msg_id;
        //if we are sending the NAS set, churn all the active/sleep messages so we have plenty of room
        while((oldest_msg_id = rpm_mq_get_oldest_in_flight_msg_id(rpm.rpm_mq)))
        {
            rpm_churn_queue(oldest_msg_id);
        }  
    }
   #endif
     // if the set has been dirty, do sync
    if(rpm.num_resources_dirty[set])
    {
        unsigned i;

        for(i = 0; i < rpm.num_resources; ++i)
        {
            rpm_resource_t *resource = rpm.resources[i];
            if(resource->dirty[set])
            {
                rpm_resource_type type = (rpm_resource_type)(resource->resource_identifier >> 32);
                unsigned          id   = resource->resource_identifier & 0xFFFFFFFFU;
                unsigned          msg_id;

                ULOG_RT_PRINTF_2(rpm_log, "\trpm_flushing (resource: 0x%08x) (id: 0x%08x)", type, id);
                msg_id = rpm_mq_put(rpm.rpm_mq, set, type, id, resource);
                if(msg_id)
                    last_msg_id = msg_id;
            }
        }
    }
    CORE_VERIFY(!(rpm.num_resources_dirty[set]));

    if ((last_msg_id) && (set == RPM_NEXT_ACTIVE_SET))
    {
        rpmclient_force_nas();
    }

    ULOG_RT_PRINTF_2(rpm_log, "rpm_flushed (set: %d) (msg_id: 0x%08x)", set, last_msg_id);

    DALSYS_SyncLeave(rpm_lock);

    return last_msg_id;
}

void rpmclient_register_handler(rpm_service_type service, rpmclient_service_cb cb, void *context)
{
    rpmclient_service_t *service_obj;

    DALSYS_SyncEnter(rpm_lock);

    service_obj = (rpmclient_service_t *)malloc(sizeof(rpmclient_service_t));
    CORE_VERIFY_PTR(service_obj);

    service_obj->service_id = service;
    service_obj->cb         = cb;
    service_obj->context    = context;
    inmap_insert(rpm.services, service_obj);
    DALSYS_SyncLeave(rpm_lock);
}
void rpmclient_v1_put(RPMMessageHeaderV1 *msg, char *data)
{
    unsigned raw_buffer_size;
    unsigned data_len = msg->request_details.data_len;
    DALSYS_SyncEnter(rpm_lock);

    // raw_buffer_size = header + data    
    raw_buffer_size = (sizeof(RPMMessageHeaderV1) + data_len);

    ULOG_RT_PRINTF_3(rpm_log, "MDA_raw_buffer_size: 0x%08x) (data_len: 0x%08x) (header_size: 0x%08x)", raw_buffer_size, data_len, sizeof(RPMMessageHeaderV1));

    // msg size = full_buffer_size - start of header (unsigned msg_header)    
    msg->msg_header.size = (raw_buffer_size - (sizeof(unsigned)));

    CORE_VERIFY(rpm.msg_buffer_size >= raw_buffer_size);

    //copy header into buffer    
    memscpy(rpm.msg_buffer, sizeof(RPMMessageHeaderV1), msg, sizeof(RPMMessageHeaderV1));

    //copy data into buffer, back off one unsigned since we don't have a kvp ptr as is in the header  
    memscpy((rpm.msg_buffer+sizeof(RPMMessageHeaderV1)), data_len, data, data_len);

    //make sure there aren't enough messages in flight to fill up the ACK fifo    
    while(rpm_mq_check_num_in_flight(rpm.rpm_mq) > SLEEP_PLUS_NAS_ACK_LIMIT)
    {	    
        rpm_churn_queue_internal(rpm_mq_get_oldest_in_flight_msg_id(rpm.rpm_mq));
    }	    

    rpmclient_glink_tx(rpm.msg_buffer, raw_buffer_size);

    DALSYS_SyncLeave(rpm_lock);
}

void rpmclient_put_nas(RPMMessagePacket *msg)
{
    void *temp_ptr;	
    unsigned raw_buffer_size;
    DALSYS_SyncEnter(rpm_lock);

    // raw_buffer_size = packet size + data size - kvp_ptr since there is no ptr in the raw buffer  
    raw_buffer_size = ((sizeof(RPMMessagePacket) + msg->data_len) - sizeof(unsigned));

    // msg size = full_buffer_size - start of header (size field and service field)    
    msg->size = (raw_buffer_size - (sizeof(unsigned) + sizeof(rpm_service_type)));

    if(!rpm.nas_bundle.buffer)
    {
        temp_ptr = malloc(raw_buffer_size);
    }
    else
    {
        temp_ptr = realloc(rpm.nas_bundle.buffer, raw_buffer_size + rpm.nas_bundle.size);
    }

    CORE_VERIFY_PTR(temp_ptr);
    rpm.nas_bundle.buffer = temp_ptr;

    //copy new NAS packet hearder    
    memscpy(rpm.nas_bundle.buffer + rpm.nas_bundle.size, sizeof(RPMMessagePacket), msg, sizeof(RPMMessagePacket));
    //copy new NAS packet kvps into buffer, back off one unsigned since we don't have a kvp ptr as is in the header  
    memscpy(((rpm.nas_bundle.buffer + rpm.nas_bundle.size + sizeof(RPMMessagePacket) - sizeof(unsigned))), msg->data_len, msg->kvp_buffer, msg->data_len);

    //update nas bundle size to include new nas packet    
    rpm.nas_bundle.size += raw_buffer_size;

    DALSYS_SyncLeave(rpm_lock);
}

void rpmclient_v1_put_nas(RPMMessageHeaderV1 *msg, char *data)
{
    void *temp_ptr;	
    unsigned raw_buffer_size;
    unsigned data_len = msg->request_details.data_len;
    DALSYS_SyncEnter(rpm_lock);

    // raw_buffer_size = header + data    
    raw_buffer_size = (sizeof(RPMMessageHeaderV1) + data_len);

    // msg size = full_buffer_size - start of header (unsigned msg_header)    
    msg->msg_header.size = (raw_buffer_size - (sizeof(unsigned)));

    if(!rpm.nas_bundle.buffer)
    {
        temp_ptr = malloc(raw_buffer_size);
    }
    else
    {
        temp_ptr = realloc(rpm.nas_bundle.buffer, raw_buffer_size + rpm.nas_bundle.size);
    }

    rpm.nas_bundle.buffer = temp_ptr;

    //copy new NAS packet header
    memscpy(rpm.nas_bundle.buffer + rpm.nas_bundle.size, sizeof(RPMMessageHeaderV1), msg, sizeof(RPMMessageHeaderV1));
    //copy new NAS packet kvps into buffer, back off one unsigned since we don't have a kvp ptr as is in the header  
    memscpy(((rpm.nas_bundle.buffer + rpm.nas_bundle.size + sizeof(RPMMessageHeaderV1))), data_len, data, data_len);

    //update nas bundle size to include new nas packet    
    rpm.nas_bundle.size += raw_buffer_size;

    DALSYS_SyncLeave(rpm_lock);
}


void rpmclient_put(RPMMessagePacket *msg)
{
    unsigned raw_buffer_size;
    DALSYS_SyncEnter(rpm_lock);

    // raw_buffer_size = packet size + data size - kvp_ptr since there is no ptr in the raw buffer    
    raw_buffer_size = ((sizeof(RPMMessagePacket) + msg->data_len) - sizeof(unsigned));

    // msg size = full_buffer_size - start of header (size field and service field)    
    msg->size = (raw_buffer_size - (sizeof(unsigned) + sizeof(rpm_service_type)));

    CORE_VERIFY(rpm.msg_buffer_size >= raw_buffer_size);
    //copy header into buffer    
    memscpy(rpm.msg_buffer, sizeof(RPMMessagePacket), msg, sizeof(RPMMessagePacket));
    //copy data into buffer, back off one unsigned since we don't have a kvp ptr as is in the header  
    memscpy(((rpm.msg_buffer+sizeof(RPMMessagePacket) - sizeof(unsigned))), msg->data_len, msg->kvp_buffer, msg->data_len);

    //make sure there aren't enough messages in flight to fill up the ACK fifo    
    while(rpm_mq_check_num_in_flight(rpm.rpm_mq) > SLEEP_PLUS_NAS_ACK_LIMIT)
    {	    
        rpm_churn_queue_internal(rpm_mq_get_oldest_in_flight_msg_id(rpm.rpm_mq));
    }	    

    rpmclient_glink_tx(rpm.msg_buffer, raw_buffer_size);

    DALSYS_SyncLeave(rpm_lock);
}

static void memscpy_word_aligned(void * dst, size_t dst_size, void * src, size_t src_size)
{
    uint32 *dst32 = (uint32*)dst;
    uint32 *src32 = (uint32*)src;

    CORE_VERIFY(dst_size % 4 == 0); 
    CORE_VERIFY(src_size % 4 == 0); 
    /*
     * MSG RAM requires 32-bit access. memscpy does not guarantee 32-bit access.
     */
    //memscpy(dst, dst_size, src, src_size);
    while (dst_size != 0 && src_size != 0)
    {
      *dst32++ = *src32++;
      dst_size -= sizeof(uint32);
      src_size -= sizeof(uint32);
    }
}

static void rpmclient_glink_isr(char * rx_buffer, unsigned size)
{
    unsigned      service_id, length;
    const char   *msg;
    rpmclient_service_t *service;
    unsigned version = MESSAGING_UNPACK_VERSION(*(unsigned*)rx_buffer);
    //check rx_buffer for version. If version = 0, do standard, else unpack and call v1 messaging

    if(!version)
    {	    
        ULOG_RT_PRINTF_0(rpm_log, "rpm_msg_v0");
        kvp_swapbuffer(rpm.rx_kvp, rx_buffer, size);
        if(!kvp_get(rpm.rx_kvp, &service_id, &length, &msg))
            ERR_FATAL("Malformed RPM message.", 0, 0, 0); // FIXME: just log/drop

        service = inmap_search(rpm.services, service_id);
        if(!service)
            ERR_FATAL("RPM message to unknown service.", 0, 0, 0); // FIXME: just log/drop

        kvp_swapbuffer(rpm.rx_kvp, msg, length);
        service->cb(rpm.rx_kvp, service->context);	
    }
    else if(version == 1)
    {
        ULOG_RT_PRINTF_0(rpm_log, "rpm_msg_v1");
        unsigned type = MESSAGING_V1_UNPACK_TYPE(*(unsigned*)rx_buffer);
        switch(type)
        {
            case RPM_V1_ACK_SERVICE:
                rpm_wire_recv_ack((RPMAckPacketV1*)rx_buffer, rpm.rpm_mq);    
		break;
            case RPM_V1_NACK_SERVICE:
                rpm_wire_recv_nack((RPMNackPacketV1*)rx_buffer, rpm.rpm_mq);    
		break;
            default:		
                ERR_FATAL("RPM unknown V1 message type.", 0, 0, 0); // FIXME: just log/drop
        }		
    }
    else
    {
        ULOG_RT_PRINTF_1(rpm_log, "rpm_msg_ver_unknown 0x%08x)", version);
        ERR_FATAL("RPM messaging version unknown.", 0, 0, 0); 
    }
}

void rpmclient_vector_glink_isr(glink_handle_type port, void *unused_open_data, void *unused_pkt_priv, 
		         void *buffer, size_t size, size_t intent_used, 
                         glink_buffer_provider_fn vprovider, glink_buffer_provider_fn pprovider)
{
    char  rx_buffer[64];
    void  *vector;    
    size_t vector_size, rx_buffer_offset = 0;

   if((port != rpm.glink_port) || (size == 0))
        return;

    DALSYS_SyncEnter(rpm_lock);


    if(size > sizeof(rx_buffer))
        ERR_FATAL("Need larger RPM rx buffer.", size, sizeof(rx_buffer), 0);

    do
    {
        vector = vprovider(buffer, rx_buffer_offset, &vector_size);
        if(vector)
        {
            memscpy_word_aligned(rx_buffer + rx_buffer_offset, vector_size, vector, vector_size);
            rx_buffer_offset += vector_size;    
        }   
    } while(vector);

    CORE_VERIFY(rx_buffer_offset == size);   
    rpmclient_glink_isr(rx_buffer, rx_buffer_offset);


    DALSYS_SyncLeave(rpm_lock);

}
/*
static void rpm_churn_command(rpm_spm_entry_reason reason)
{
    soft_sleep_state_t end_state = (reason == SPM_SHUTDOWN_ACK) ? FAKE_SLEEP : ACTIVE; 

    while(rpm.soft_sleep_state != end_state)
    {
        DALSYS_SyncLeave(rpm_lock);
        CORE_VERIFY(GLINK_STATUS_SUCCESS == glink_rpm_rx_poll(rpm.glink_port));
        DALSYS_SyncEnter(rpm_lock);
    }
}
*/
unsigned rpm_post_request(rpm_set_type set, rpm_resource_type resource, unsigned id, kvp_t *kvps)
{
    rpm_resource_t *resource_data;
    unsigned        msg_id = 0;

    if(set >= RPM_NUM_SETS)
        return 0;

    DALSYS_SyncEnter(rpm_lock);


    ULOG_RT_PRINTF_3(rpm_log, "rpm_post_request (resource: 0x%08x) (id: 0x%08x) (set: \"%s\")", resource, id, rpm_set_names[set]);
    resource_data = rpm_find_resource(resource, id);
    if(!resource_data)
        resource_data = rpm_add_resource(resource, id);

    rpm_resource_update(resource_data, set, kvps);

    if((RPM_ACTIVE_SET == set) && resource_data->dirty[set])
    {
        msg_id = rpm_mq_put(rpm.rpm_mq, set, resource, id, resource_data);
        ULOG_RT_PRINTF_3(rpm_log, "\trpm_posted (resource: 0x%08x) (id: 0x%08x) (msg_id: 0x%08x)", resource, id, msg_id);
    }


    DALSYS_SyncLeave(rpm_lock);

    return msg_id;
}
void rpm_churn_queue(unsigned message_id)
{
    DALSYS_SyncEnter(rpm_lock);

    rpm_churn_queue_internal(message_id);   

    DALSYS_SyncLeave(rpm_lock);
}

static void rpmclient_glink_tx_done_isr(void)
{
  //empty function for tx_done cb. Nothing required here for now since we are always in 
  //"single-threaded" operation of sending GLink requests
}

static void rpmclient_glink_notify_state_isr(glink_handle_type handle, void *data, glink_channel_event_type event)
{
    //indicates channel is successfully open	
    if(event ==	 GLINK_CONNECTED)
    {
        rpm.rpm_is_up = true;    
        DALSYS_EventCtrl(rpm_sync_event, DALSYS_EVENT_CTRL_TRIGGER);
    }	    
}	

static void rpm_open_glink_port(void)
{
    if(!rpm.glink_port)
    {    	    
        glink_open_config_type glink_open_cfg = {0};
        glink_open_cfg.name = "rpm_requests";
        glink_open_cfg.remote_ss = "rpm";
        glink_open_cfg.notify_rxv = (glink_rxv_notification_cb)rpmclient_vector_glink_isr;
        glink_open_cfg.notify_tx_done = (glink_tx_notification_cb)rpmclient_glink_tx_done_isr;
        glink_open_cfg.notify_state = (glink_state_notification_cb)rpmclient_glink_notify_state_isr;
        glink_open_cfg.priv = NULL;
        CORE_VERIFY(GLINK_STATUS_SUCCESS == glink_open(&glink_open_cfg, &(rpm.glink_port)));
    }	
}

void rpmclient_glink_link_state_isr(glink_link_info_type *link_info, void* unused_param)
{
    switch(link_info->link_state)
    {	    
        case GLINK_LINK_STATE_UP:
            rpm_open_glink_port();
            break;
	case GLINK_LINK_STATE_DOWN:
            break;
    }	
}

volatile bool RPM_LOOP_FOREVER = false;

void rpmclient_init(void)
{
    static const unsigned REQ_KEY  = 0x00716572;

    if(rpm.rpmclient_initialized)
        return;
    rpm.rpmclient_initialized = 1;
    rpm_fifo_full_handler = &rpm_mq_force_churn_cb;


    if(DAL_SUCCESS != DALSYS_SyncCreate(DALSYS_RPM_SYNC_TYPE, &rpm_lock, 0))
        ERR_FATAL("Couldn't create RPM lock.", 0, 0, 0);

    if(DAL_SUCCESS != DALSYS_EventCreate(DALSYS_EVENT_ATTR_CLIENT_DEFAULT, 
                                &(rpm_sync_event),
                                &(rpm_sync_event_memory)))
        ERR_FATAL("Couldn't create RPM Sync event.", 0, 0, 0);


    CORE_DAL_VERIFY(ULogFront_RealTimeInit(&rpm_log,
                                           "RPMMasterLog",
                                           4096,
                                           ULOG_MEMORY_LOCAL,
                                           ULOG_RPM_LOCK_TYPE));

    rpm.rpm_mq = rpm_mq_create();

    rpm.services = inmap_create(offsetof(rpmclient_service_t, link), offsetof(rpmclient_service_t, service_id));
    rpmclient_register_handler((rpm_service_type)REQ_KEY, (rpmclient_service_cb)rpm_wire_recv, rpm.rpm_mq);

    rpm.rx_kvp = kvp_create(0);
    rpm.rpm_is_up = false;

    glink_link_id_type link_cfg = {1, NULL, "rpm", (glink_link_state_notif_cb)rpmclient_glink_link_state_isr, rpm.glink_link};
    CORE_VERIFY(GLINK_STATUS_SUCCESS == glink_register_link_state_cb(&link_cfg, NULL));

    rpm.msg_buffer_size = sizeof(unsigned) * MSG_BUFFER_SIZE;
    rpm.msg_buffer = malloc(rpm.msg_buffer_size);    
    rpm.nas_bundle.buffer = NULL;
    rpm.nas_bundle.size = 0;
    rpm.soft_sleep_state = ACTIVE;

    /* Wait for rpm_sync_event,
     * will be release after GLINK_CONNECTED is received */
    if(!is_standalone())
    {
        DALSYS_EventWait(rpm_sync_event);

        if(!rpm.glink_port)
            ERR_FATAL("Failed to establish communication with RPM.", (int)rpm.glink_port, 0, 0);
        else
            ULOG_RT_PRINTF_0(rpm_log, "rpm_glink_up");
    }
    else
    {
        ULOG_RT_PRINTF_0(rpm_log, "rpm_glink_not_supported");
    }

#ifndef FEATURE_BUILD_PAGEONLY_IMAGE
    npa_define_marker("/init/rpm");
#endif	
}

bool rpm_is_up(void)
{
  return rpm.rpm_is_up;
}

bool rpm_is_mq_empty(void)
{
    return rpm_mq_check_empty(rpm.rpm_mq);
}

void rpm_signal_aborted_sleep(unsigned asserted)
{
    CORE_VERIFY(GLINK_STATUS_SUCCESS == glink_sigs_set(rpm.glink_port, asserted));
}

void rpm_preallocate_resource_kvp(rpm_resource_type resource, unsigned id, unsigned key, unsigned expected_length)
{
    rpm_resource_t *resource_data = rpm_find_resource(resource, id);
    if(!resource_data)
        resource_data = rpm_add_resource(resource, id);

    rpm_resource_preallocate(resource_data, key, expected_length);
}
void rpm_update_num_dirty(rpm_set_type set, bool pre_dirty, bool post_dirty)
{
	if(set >= RPM_NUM_SETS)
        return;
		
    if(pre_dirty != post_dirty)
    {
       if(!post_dirty)
       {
           CORE_VERIFY(rpm.num_resources_dirty[set] > 0);
       }
       rpm.num_resources_dirty[set] += (post_dirty) ? 1 : -1; 
    }
}

uint64_t rpm_get_xo_accumulated_duration(void)
{
   rpm_xo_drift_compensation_t *xo_drift = (rpm_xo_drift_compensation_t *)vmpm_xo_drift_base_VA;
   
   if(xo_drift == NULL)
	   CORE_VERIFY(0);
   return xo_drift->xo_accumulated_duration;
   
}

void rpm_update_xo_drift_counter(unsigned reset)
{
   rpm_xo_drift_compensation_t *xo_drift = (rpm_xo_drift_compensation_t *)vmpm_xo_drift_base_VA;
   
   if(xo_drift == NULL)
	   CORE_VERIFY(0);
   xo_drift->counter_reset = reset;
   
}

#if 0 // used for testing on new targets
void rpm_test_task(unsigned long int unused_param)
{
  unsigned msg_id, data;
  kvp_t   *test_data = kvp_create(0);

  data = 0xf005ba11;
  kvp_put(test_data, 0x0ddba11, 4, (char *)&data);

  data = 0xfeeb1e;
  kvp_put(test_data, 0xfab1e5, 4, (char *)&data);

  msg_id = rpm_post_request(RPM_ACTIVE_SET, RPM_TEST_REQ, 0, test_data);
  rpm_barrier(msg_id);

  //while(1);
  // Call this when I'm started, I guess.  For now I'm going to ignore this
  // part since I just need to do some one-off testing.
  //rcinit_handshake_startup();
}
#endif

