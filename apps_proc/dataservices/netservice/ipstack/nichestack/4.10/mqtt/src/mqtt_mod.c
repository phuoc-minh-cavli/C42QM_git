/*
 * v4.1.testing mqtt/mqtt_mod.c
 * May 26, 2016
 * Distributed per InterNiche ref US2611-2
 */
/*
 * FILENAME: mqtt_mod.c
 *
 * Copyright  2015 By InterNiche Technologies Inc. All rights reserved
 *
 *
 * MODULE: MQTT
 *
 * ROUTINES: 
 * ROUTINES: 
 *
 * PORTABLE: yes
 */

#include "mqtt_port.h"
#ifdef MQTT_CLIENT /* skip file if not defined */
#include "mqtt_stats.h"
#include "MQTTPacket.h"
#include "MQTTConnect.h"
#include "mqttc.h"
#include "timer.h" 

#ifndef MOB_TX
#include "qurt_txm_qapi_fwk.h"
#else
#include "offtarget_stubs.h"
#endif

#include "qapi_mqtt.h"
#include "rcinit.h"

qurt_thread_t mqtt_thread_id = 0;
qurt_thread_t get_mqtt_thread_id(void);

/* Globals */
#ifdef NET_STATS
int mqtt_stats[num_mqtt_stats];
#ifdef SHOW_NET_STATS
const char *mqtt_stats_strings[num_mqtt_stats] =
{
   "connect API invocations",
   "clear sessions established",
   "secure sessions established",
   "active clear sessions",
   "active secure sessions",
   "connect out",
   "connection closed by server",
   "Remaining Length (1 byte) (rx)",
   "Remaining Length (2 bytes) (rx)",
   "Remaining Length (3 bytes) (rx)",
   "Remaining Length (4 bytes) (rx)",
   "connection ACKs in",
   "publish (QoS 0) out",
   "publish (QoS 1) out",
   "publish (QoS 2) out",
   "publish (QoS 0) in",
   "publish (QoS 1) in",
   "publish (QoS 2) in",
   "publish ACK out",
   "publish ACK in",
   "pubrec out",
   "pubrec in",
   "pubrel out",
   "pubrel in",
   "pubcomp out",
   "pubcomp in",
   "subscribe",
   "subscribe ACK",
   "topic subscription count",
   "topic subscription granted",
   "topic subscription denied",
   "zero recipient count",
   "unsubscribe",
   "unsubscribe ACK",
   "ping sent",
   "ping received",
   "disconnect sent",

   /* abnormal operation */
   "connect fails",
   "publish fails",
   "subscribe fails",
   "unsubscribe fails",
   "proto timeout",
   "pubrel_fails",
   "puback_fails",
   "pubrec_fails",
   "pubcomp_fails",
   "pingreq_fails",
   "alloc_failures",
   "unexpected_msg_in",
   "unexpected_msg_out",
   "mismatched_pktid_in",
   "mismatched_qos_in",
   "socket_failures",
   "parameter_error",
   "ping failures",
   "UTF-8 validation failures",
   "transitioned_to_dying",

   /* packet rejects */
   "bad connack in",
   "bad publish in",
   "bad puback in",
   "bad pubrec in",
   "bad pubrel in",
   "bad pubcomp in",
   "bad subscribe ACK in",
   "bad unsubscribed ACK in",
   "bad ping resp in",
};
#endif
#endif

IN_SEM ts_mqtt = NULL;
 qurt_mutex_t * mqtt_mutex;
extern void tk_mqttc_v2(void);

int mqtt_init(void)
{
  int err = ESUCCESS;

  if ((ts_mqtt = SEM_ALLOC()) == (IN_SEM)NULL)
  {
    err =  EFAILURE;
  }

  mqtt_mutex = (qurt_mutex_t *)malloc(sizeof(qurt_mutex_t));
  qurt_mutex_init(mqtt_mutex);
   
   return err;
}

void MQTT_thread(void *p)
{
  rcinit_handshake_startup();
#ifndef MOB_TX
  qurt_qapi_register(TXM_QAPI_MQTT_BASE + 1, TXM_QAPI_MQTT_NUM_IDS, qapi_Net_MQTT_Handler);
  qurt_qapi_register(TXM_QAPI_MQTT_BASE2 + 1, TXM_QAPI_MQTT_NUM_IDS_2, qapi_Net_MQTT_Handler);
#endif
  mqtt_thread_id = qurt_thread_get_id();
  
#ifndef SUPERLOOP
   TK_MQTT_BLOCK();
   while(!iniche_net_ready)
   {
      timer_sleep(10, T_MSEC, 1);
   }
   qurt_thread_set_priority(qurt_thread_get_id(),50);
   for (;;)
   {  
      MQTT_LOCK_MUTEX(mqtt_mutex);
      if(MQTT_connlist == NULL) {
        MQTT_UNLOCK_MUTEX(mqtt_mutex);
        TK_MQTT_BLOCK();
      } else {
        MQTT_UNLOCK_MUTEX(mqtt_mutex);
      }
  
      tk_mqttc_v2();
      timer_sleep(500, T_MSEC, 1);
   }
#else
   if (iniche_net_ready)
   {
      tk_mqttc_v2();
   }
#endif  /* SUPERLOOP */
}

qurt_thread_t get_mqtt_thread_id(void)
{
    return mqtt_thread_id;
}
#endif /* MQTT_CLIENT */
