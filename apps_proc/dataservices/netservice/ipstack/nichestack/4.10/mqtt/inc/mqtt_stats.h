/*
 * v4.1.testing mqtt/mqtt_stats.h
 * May 26, 2016
 * Distributed per InterNiche ref US2611-2
 */
#ifdef NET_STATS
enum mqtt_stats_names
{
   /* normal operation */
   connect_requests,
   clear_sessions_established,
   secure_sessions_established,
   active_clear_sessions,
   active_secure_sessions,
   connect_out,      
   conn_closed_by_srv,      
   rx_rem_length1,
   rx_rem_length2,
   rx_rem_length3,
   rx_rem_length4,
   connack_in,
   publish0_out,
   publish1_out,
   publish2_out,
   publish0_in,
   publish1_in,
   publish2_in,
   puback_out,
   puback_in,
   pubrec_out,
   pubrec_in,
   pubrel_out,
   pubrel_in,
   pubcomp_out,
   pubcomp_in,
   subscribe_out,
   suback_in,
   topic_subscription_count,
   topic_subscription_granted,
   topic_subscription_denied,
   zero_recipient_count,
   unsubscribe_out,
   unsuback_in,
   pingreq_out,
   pingresp_in,
   disconnect_out,

   /* abnormal operation */
   connect_fails,
   publish_fails,
   subscribe_fails,
   unsubscribe_fails,
   proto_timeout,
   pubrel_fails,
   puback_fails,
   pubrec_fails,
   pubcomp_fails,
   pingreq_fails,
   alloc_failures,
   unexpected_msg_in,
   unexpected_msg_out,
   mismatched_pktid_in,
   mismatched_qos_in,
   socket_failures,
   parameter_error,
   ping_failures,
   utf8_validation_failures,
   transitioned_to_dying,

   /* packet rejects */
   badconnack_in,
   badpublish_in,
   badpuback_in,
   badpubrec_in,
   badpubrel_in,
   badpubcomp_in,
   badsuback_in,
   badunsuback_in,
   badpingresp_in,
   num_mqtt_stats
} ;

extern int mqtt_stats[num_mqtt_stats];
extern const char *mqtt_stats_strings[num_mqtt_stats];

#define MQTTINC(array, element)  array[element]++
#define NSINC2(array, element)  array[element]++
#define NSDEC2(array, element)  array[element]--
#define NSINCn(array, element, n)  array[element]+=n

#else

#define MQTTINC(array, element)
#define NSINC2(array, element)
#define NSDEC2(array, element)
#define NSINCn(array, element, n)

#endif

