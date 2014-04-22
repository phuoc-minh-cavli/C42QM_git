/*=============================================================================

                                MAIN

GENERAL DESCRIPTION
  This file contains the initial operations for the RPM.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2009-2011, 2019 by QUALCOMM Technologies Inc.  All Rights Reserved.

  $Header: //components/rel/rpm.bf/2.1.3/core/bsp/rpm/src/main.c#8 $
  $Author: pwbldsvc $
  $DateTime: 2019/08/30 05:43:45 $
  when       who     what, where, why
  --------   ---     -------------------------------------------------------
  02/22/19   kdey    Fixed timestamp issue
=============================================================================*/

#include <assert.h>
#include "comdef.h"

#include "cortex-m3.h"
#include "npa_init.h"
#include "comdef.h"
#include "smem.h"
#include "rpmserver.h"
#include "mpm.h"
#include "vmpm.h"
#include "dog.h"
#include "time_service.h"
#include "QDSSLite.h"
#include "icb_rpm.h"
#include "swevent.h"
#include "railway.h"
#include "svs.h"
#include "kvp.h"
#include "ddr_drivers.h"
#include "system_db_rpm.h"
#include "rbcpr_comdef.h"
#include "rpm_settling_timer.h"
#include "sleep_v.h"
#include "HALhwio.h"
#include "hw_version.h"
#include "coredump.h"
#include "Clock.h"
#include "PlatformInfo.h"
#include "version.h"
#include "timetick.h"
#include "version.h"
#include "image_layout.h"
#include "pm_err_flags.h"
#include "pm_version.h"
#include "pmapp_npa.h"
#include "rpm_subsys_state.h"

//prng stack protection init
void __init_stack_chk_guard(void);

void xpu_init(void);
void time_service_init(void);
void sched_run(void);
uint64_t sched_get_next_start(void);
unsigned int npa_num_pending_events(void);
void npa_process_event(void);
void exceptions_init(void);
void exceptions_enable(void);
void pm_init(void);
void debug_init(void);
void swevent_init(void);
void swevent_qdss_init(void);
void prevent_further_heap_frees(void);
void busywait_init(void);
void rpm_settling_timer_init(void);

void gpio_toggle_init(void);
void rpm_subsys_pc_init(void);

//Get rid of this once we're back to one railway.
void railway_init_v2(void);
void railway_init_early_proxy_votes(void);
void populate_image_header(void);
void zqcal_task_init(void);
void acc_init(void);
void debug_cookie_init(void);
void rpm_init_done(void);
void rpm_server_init_done(void);
void message_ram_init_rail_info(void);

void rpm_resources_config_init(void);
void rpm_pcm_init(void);

extern void glink_ssr_init( void );
extern void glink_init(void);
extern void xport_rpm_toc_init(void);
extern void xport_rpm_init(void);

#define ARRAY_SIZE(a) (sizeof(a)/sizeof((a)[0]))

typedef void (*init_fcn)(void);


static volatile uint32 debug_break=1;

const init_fcn init_fcns[] =
{
  populate_image_header,
  PlatformInfo_Init, /* pm_init is using PlatformInfo APIs */
  // xpu_init, /* cookie set here also indicates to SBL that railway is ready */

  npa_init,
  rpm_subsys_pc_init,
  pm_init,
  railway_init_v2,
  message_ram_init_rail_info,
  acc_init,
  railway_init_early_proxy_votes,
  ddr_init,
  (init_fcn)Clock_Init,
  __init_stack_chk_guard,
  smem_init,
  glink_init,
  xport_rpm_toc_init,
  xport_rpm_init,
  version_init,     /* Needs to be after smem_init */
  rpmserver_init,
  rpm_server_init_done,
  rpm_resources_config_init,
  railway_init_proxies_and_pins,
  rbcpr_init,
  svs_init,
  vmpm_init,
  sleep_init,
  QDSSInit,
  exceptions_enable,
  swevent_qdss_init,
  icb_init,
#ifndef MDM9x05_STUBS
  debug_init,
#endif
  system_db_init,
  zqcal_task_init,

  rpm_settling_timer_init,
  gpio_toggle_init,
  rpm_pcm_init,
  rpm_set_changer_common_init,
  railway_finalize_init,        //Must be last
};
#ifdef MDM9x05_STUBS // TODO 9x05: remove after sod
	#define DOG_BARK_DURING_BOOT  0xFFFFF// ~128ms
	#define DOG_BITE_DURING_BOOT  0xFFFFF// ~168 ms

	#define DOG_BARK_AFTER_BOOT 0xFFFF // ~31.2 ms
	#define DOG_BITE_AFTER_BOOT 0xFFFF// ~62.5 ms
#else
	#define DOG_BARK_DURING_BOOT  0x1000// ~128ms
	#define DOG_BITE_DURING_BOOT  0x1500// ~168 ms

	#define DOG_BARK_AFTER_BOOT 0x400 // ~31.2 ms
	#define DOG_BITE_AFTER_BOOT 0x800 // ~62.5 ms
#endif

#define PC_STATE 0x74617473

#define WP_ONLINE_DUMP_TZ_IRQ 12

static void init_time_proxy_votes(bool is_tiny_modem)
{
  kvp_t *kvp;
  uint32 req;
 if(is_tiny_modem == FALSE)
 {
   req = 1;
  static unsigned ENABLED_KEY = 0x62616e45;
  //init time request for CXO on behalf of apps
  extern void Clock_SetSPMStates( kvp_t *kvp );
  kvp = kvp_create(0);
  kvp_clear(kvp);
  kvp_put(kvp, ENABLED_KEY, sizeof(req), &req);
  rpm_send_init_proxy_vote(RPM_CLOCK_0_REQ, /*CXO is CLK0*/ 0, /*Apps is master 0*/ 0, kvp);
  Clock_SetSPMStates(kvp);
  kvp_destroy(kvp);
 }
 else
 {
  uint64 reqab;
  /* Tiny modem use case */
  /* Place a proxy active set nominal vote of crypto clock from TZ. */
  kvp = kvp_create(0);
  kvp_clear(kvp);
  static unsigned BW_KEY = 0x7762;
  reqab = 0x632EA00; // AB Vote for Nom freq of crypto.
  kvp_put(kvp, BW_KEY, sizeof(reqab), &reqab);
  rpm_send_init_proxy_vote( RPM_BUS_MASTER_REQ, 23, /* TZ Driver is master 2*/ 2, kvp);
 }
#ifdef MODEM_PROXY_VOTE_CX_NOM
  //init time request for CX to NOM on behalf of MODEM 
  kvp = kvp_create(0);
  kvp_clear(kvp);
  req = (uint32_t)RAIL_VOLTAGE_LEVEL_NOMINAL;
  kvp_put(kvp, PM_NPA_KEY_LEVEL_RAIL_VOLTAGE, sizeof(req), (void *)&req);
  rpm_send_init_proxy_vote(RPM_LDO_A_REQ,  // Cx is L1
                           1,
                           1,               // MODEM is 0
                           kvp);
#endif
  
  // PC proxy vote for Apps power collapse state.  This will be used bt BIMC clk driver
  req = SUBSYS_ACTIVE_STATE;
  kvp = kvp_create (0);
  kvp_clear(kvp);
  kvp_put(kvp, PC_STATE, sizeof(req), &req);
  rpm_send_init_proxy_vote(RPM_SUBSYS_STATE_REQ, 0, /* Apps is master 0*/ 0, kvp);

  req = SUBSYS_PC_STATE;
  kvp_clear(kvp);
  kvp_put(kvp, PC_STATE, sizeof(req), &req);
  rpm_send_init_proxy_vote_sleep(RPM_SUBSYS_STATE_REQ, 0, /* Apps is master 0*/ 0, kvp);
#if 0
#ifdef PMIC_S1_PC_PROXY_VOTE //enabled it for 9x07 only
  //Activer Set vote on behalf of APPS for 'S1' Shadow Resource
  req = 1;  
  kvp = kvp_create(0);
  kvp_clear(kvp);
  kvp_put(kvp,PM_NPA_KEY_SOFTWARE_ENABLE, sizeof(req), &req);
  rpm_send_init_proxy_vote(RPM_SMPS_A_S1_SH_REQ,  0, /*Apps is master 0*/ 0, kvp); 
  
  //Sleep Set vote on behalf of APPS for 'S1' Shadow Resource
  req = 0;  
  kvp = kvp_create(0);
  kvp_clear(kvp);
  kvp_put(kvp,PM_NPA_KEY_SOFTWARE_ENABLE, sizeof(req), &req);
  rpm_send_init_proxy_vote_sleep(RPM_SMPS_A_S1_SH_REQ,  0, /*Apps is master 0*/ 0, kvp);
#endif 



#ifdef RPM_GPLL0_PROXY_VOTE
  //Init time Active set request for GPLL1 behalf of Apps.
  req = 1;
  kvp_clear(kvp);
  kvp_put(kvp, ENABLED_KEY, sizeof(req), &req);
  rpm_send_init_proxy_vote(RPM_CLOCK_0_REQ,/*gpll0 in CLK0*/ 4, /*Apps is master 0*/ 0, kvp);

  //Init time Sleep set request for GPLL1 behalf of Apps.
  req = 0;
  kvp_clear(kvp);
  kvp_put(kvp, ENABLED_KEY, sizeof(req), &req);
  rpm_send_init_proxy_vote_sleep(RPM_CLOCK_0_REQ,/*gpll0 in CLK0*/ 4, /*Apps is master 0*/ 0, kvp);
#endif
  kvp_destroy(kvp);
#endif
  
}
extern void pm_post_init (void);
extern bool tiny_modem_init(void);

int main(void)
{
  unsigned i;
  bool is_tiny_modem = FALSE;

  debug_cookie_init();

  exceptions_init();
  busywait_init();
  timetick_init();
  mpm_init();
  dog_init();

  dog_set_bite_time(DOG_BITE_DURING_BOOT);
  dog_set_bark_time(DOG_BARK_DURING_BOOT);

  rpm_core_dump.hw_version.v = HWIO_IN(TCSR_SOC_HW_VERSION);
#if 0
  //MsgRAM clock are already setup by SBL
  //GCC_RPM_CLOCK_BRANCH_ENA_VOTE : 0x01800000 + 0x00036004

  // We need to have the clock to message RAM turned on *before* we intiailize
  // the rpmserver driver.  This is *before* we initialize the clock driver,
  // so we have to just do it ourselves.
  volatile uint32_t *RPM_CLOCK_BRANCH_ENA_VOTE = (volatile uint32_t *)0xFC401444;
  *RPM_CLOCK_BRANCH_ENA_VOTE |= 0x200; // sets the vote bit for msg_ram_hclk
#endif
  time_service_init();
  swevent_init();

  SWEVENT(RPM_BOOT_STARTED);

  for(i = 0; i < ARRAY_SIZE(init_fcns); ++i)
  {
    dog_kick();
    init_fcns[i]();
  }

  dog_set_bark_time(DOG_BARK_AFTER_BOOT);
  dog_set_bite_time(DOG_BITE_AFTER_BOOT);

#ifdef MDM9x05_TARGET
   is_tiny_modem = tiny_modem_init();
#endif
   init_time_proxy_votes(is_tiny_modem);
  prevent_further_heap_frees();
  SWEVENT(RPM_BOOT_FINISHED);

  rpm_init_done();

  pm_post_init();

  // enable interrupt from TZ to stop RPM to collect online dumps
  interrupt_enable(WP_ONLINE_DUMP_TZ_IRQ);

  #define ever ;;
  for(ever)
  {
    assert(!intlock_nest_level);
    do
    {
      sched_run();
      assert(!intlock_nest_level);

      if(npa_num_pending_events() > 0)
      {
        npa_process_event();
        assert(!intlock_nest_level);
      }
      else
        break; // nothing to do--try to sleep
    } while(1);

    assert(!intlock_nest_level);
    INTLOCK();
    if(!npa_num_pending_events() && (sched_get_next_start() >= time_service_now()))
      sleep_perform_lpm();
    INTFREE();
    assert(!intlock_nest_level);
  }
}

