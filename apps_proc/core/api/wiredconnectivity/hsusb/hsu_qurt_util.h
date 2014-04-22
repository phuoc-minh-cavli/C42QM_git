#ifndef __HSU_QURT_UTIL_H___
#define __HSU_QURT_UTIL_H___
#include "qurt.h"
#include "comdef.h"
#define REX_TASK_NAME_LEN                       (11)

#ifdef USB_THREADX
typedef qurt_mutex_t rex_crit_sect_type;  //from rex.h
typedef qurt_thread_t rex_tcb_type;


typedef unsigned long             timer_cb_data_type;  //from timer.h
//typedef uint32  timer_unit_type; /**< -- Various units supported by the timetick module*/
//typedef uint32  timetick_type; /**< -- Time in 32bit ticks*/ 
#endif

void hsu_qurt_sleep_ms(uint32 msec);
boolean rex_is_in_irq_mode(void);
qurt_thread_t rex_self(void);


void rex_init_crit_sect(void*p_crit_sect);

void rex_enter_crit_sect(void*p_crit_sect);

void rex_leave_crit_sect(void *p_crit_sect);

#endif /* __HSU_QURT_UTIL_H___ */
