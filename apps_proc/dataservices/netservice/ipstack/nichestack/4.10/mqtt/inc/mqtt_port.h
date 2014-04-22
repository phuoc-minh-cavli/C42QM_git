/*
 * mqtt_port.h
 */

/*
 * Copyright (c) 2019-2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#define INICHE_OS_NAME        "QuRT"

#include <stdint.h>

#include "qurt/qurt_thread.h"
#include "qurt/qurt_timer.h"
#include "qurt/qurt_mutex.h"
#include "qurt/qurt_signal.h"
#include "qurt/qurt_error.h"
#include "qapi_diag.h"

#define TK_EVENT_GROUP      qurt_signal_t
typedef qurt_signal_t*      tk_event_handle;
typedef qurt_signal_t       TK_EVENT_RET;
typedef qurt_thread_t TASK;

typedef unsigned char   u_char;
typedef unsigned short  u_short;
typedef unsigned int    u_int;
typedef unsigned long   u_long;
typedef boolean         bool_t;
typedef unsigned short  unshort;
typedef unsigned long   ulong;
typedef unsigned long   ip_addr;
typedef unsigned int    bool;

#ifndef UNUSED
#define UNUSED(x) (void)x;
#endif

#define IP_V4              1
#define IP_V6              1
#define CoAP_MQTT_LOGGING_ENABLED 1

#ifndef MOB_TX
typedef unsigned int time_t;


struct timeval
{
   time_t tv_sec;     /* seconds */
   long   tv_usec;    /* and microseconds */
};
#endif
#define htonl(l) (((((l) >> 24) & 0x000000ff)) | \
                 ((((l) >>  8) & 0x0000ff00)) | \
                 (((l) & 0x0000ff00) <<  8) | \
                 (((l) & 0x000000ff) << 24))
#define ntohl(l) htonl(l)
#define htons(s) ((((s) >> 8) & 0xff) | \
                 (((s) << 8) & 0xff00))
#define ntohs(s) htons(s)

#define TPS                 19200U  /* cticks per second */

#define IP6EQ(a,b)        (MEMCMP((a),(b),sizeof(struct in6_addr)) == 0)

#ifdef MULTI_HOMED
#define STATIC_NETS   4  /* static network interfaces to allow for... */
#else
#define STATIC_NETS   8
#endif

#ifdef DYNAMIC_IFACES
#define MAXNETS       6  /* max ifaces to support at one time */
#else
#define MAXNETS       STATIC_NETS
#endif

#ifndef BYTE_ORDER
#define LITTLE_ENDIAN 1234
#define BIG_ENDIAN    4321
#define BYTE_ORDER    LITTLE_ENDIAN
#endif

#define SOCKTYPE        long

#define MEMCPY(d, s, l)    mqtt_memcpy((void *)(d), (void *)(s), (int32_t)(l))
#define MEMCMP(d, s, l)    memcmp(d, s, l)
#define TIME_EXP(e,n) (bool_t)(((int)((e) - (n))) <= 0) 

extern bool_t iniche_net_ready;
extern bool_t  iniche_os_ready;


#define MEMSET(d, c, l)    memset((void *)(d), (int32_t)(c), (int32_t)(l))
#define IP_MULTICAST       1
#define ESUCCESS           0 /* whatever the call was, it worked */
#define EFAILURE           -1 /* whatever the call was, it failed */
//#define NET_STATS          1

#define  IN_SEM               qurt_signal_t *
#define  IN_MUTEX             void *
#define  TK_TIMEOUT           (-2)     /* semaphore/mutex pending timeout error code */
#define  TK_NOTASK            ((TASK)NULL)
#define  TK_TIMEOUT           (-2)     /* semaphore/mutex pending timeout error code */
#define  INFINITE_DELAY       0xFFFFFFFF  /* Maximum settable delay period  */

#define SEM_ALLOC()           mqtt_sem_create()
#define TK_SIGNAL(s)          mqtt_sem_post(s)
#define TK_SIGWAIT(s,n)       mqtt_sem_pend((s), (n))
#define SEM_FREE(s)           mqtt_sem_free(s)
#define TK_MQTT_BLOCK()       TK_SIGWAIT(ts_mqtt, INFINITE_DELAY)
#define TK_WAKE(tk)           mqtt_wake(tk)
#define TK_SUSPEND(tk)        mqtt_suspend(tk)
#define TK_RESUME(tk)         mqtt_wake(tk)
#define TK_MUTEX_PEND(m,t)    mqtt_mutex_pend(m,t)
#define TK_MUTEX_POST(m)      mqtt_mutex_post(m)
#define MUTEX_ALLOC()         mqtt_mutex_create()
#define SignalMqtt()          TK_SIGNAL(ts_mqtt)
#define MUTEX_FREE(m)         mqtt_mutex_free(m)

#ifdef NPDEBUG
#define DTRAP() _dtrap(__FILE__,__LINE__)
extern void _dtrap(char *, int);
#else
#define DTRAP()
#endif

#ifndef TRAP
#define TRAP /* Do nothing special */
#endif


/* 
 * Use the resource locking routines, and map LOCK/UNLOCK_NET_RESOURCE 
 * to them with macros.  Similar to the above case, we use macros to 
 * provide null definitions for ENTER/EXIT_CRITICAL_SECTION.
 */
#define WAIT_NET_RESOURCE(res, t)   mqtt_res_lock((res), (t))
#define MQTT_LOCK_MUTEX(mutex)      mqtt_mutex_lock(mutex)
#define MQTT_UNLOCK_MUTEX(mutex)    mqtt_mutex_unlock(mutex)

/* TODO: Implement crit section with mutex for QuRT */
#define ENTER_CRIT_SECTION()        enter_critical_section()
#define EXIT_CRIT_SECTION()         exit_critical_section()


#if configUSE_TICK_HOOK
    extern uint32_t cticks;     /* clock ticks since startup */
    void clock_c(void);         /* stop clock tick counting */
    #define CTICKS      cticks
#else
    uint64_t in_get_cticks(void);
    #define CTICKS      (in_get_cticks())
#endif

extern void npfree(void *ptr);
extern void *npalloc(uint32_t bytes);
extern void* mqtt_memcpy(void *d, const void *s, size_t n);

/*
 * Task utility routines
 */
extern void mqtt_mutex_free(IN_MUTEX mutex);
extern void mqtt_suspend(TASK tk);
extern IN_SEM mqtt_sem_create(void);
extern void mqtt_sem_free(IN_SEM);
extern int  mqtt_sem_pend(IN_SEM, int32_t);
extern int  mqtt_mutex_pend(IN_MUTEX, int32_t);
extern int  mqtt_mutex_post(IN_MUTEX);
extern void mqtt_wake(TASK tk);
extern IN_MUTEX mqtt_mutex_create(void);
extern int mqtt_sem_post(IN_SEM sem);
extern uint32_t mqtt_event_set(tk_event_handle event_ptr, uint32_t bitsToSet);
int mqtt_mutex_lock(qurt_mutex_t *mutex);
int mqtt_mutex_unlock(qurt_mutex_t *mutex);
uint32_t mqtt_event_wait(tk_event_handle event_ptr, uint32_t bitsToWaitFor, uint32_t all, uint32_t var1, uint32_t ticksToWait);

