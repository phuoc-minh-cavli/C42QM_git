#include "hsu_qurt_util.h"



void hsu_qurt_sleep_ms(uint32 msec)
{
  //TODO: confirm
  uint32 tick = qurt_timer_convert_time_to_ticks(msec, QURT_TIME_MSEC);
  qurt_thread_sleep_ext(tick);
}

boolean rex_is_in_irq_mode(void)
{
  //TODO: update after api enabled 
  if(qurt_system_is_irq_mode())
  {
    return TRUE;
  }
  return FALSE;
}

void rex_init_crit_sect(void*p_crit_sect)
{
  qurt_mutex_init((qurt_mutex_t *)p_crit_sect);
  return;
}

void rex_enter_crit_sect(void*p_crit_sect)
{
  qurt_mutex_lock((qurt_mutex_t *)p_crit_sect);
  return;
}

void rex_leave_crit_sect(void *p_crit_sect)
{
  qurt_mutex_unlock((qurt_mutex_t *)p_crit_sect);
  return;
}

qurt_thread_t rex_self(void)
{
 //TODO: update after api enabled
  return qurt_thread_get_id();
//  return 0;
}


