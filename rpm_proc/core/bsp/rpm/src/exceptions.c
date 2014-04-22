
#include <string.h>
#include "cortex-m3.h"
#include "comdef.h"
#include "rpm_hwio.h"
#include "HALhwio.h"
#include "coredump.h"
#include "image_layout.h"
#include "CoreVerify.h"
#include "swevent.h"
#include "pshold_hwio.h"

#define WATCHDOG_IRQ_NUM 49
#define TINY_MODEM_IMEM_INFO_ADDR 0x68628FFC

extern void xport_rpm_isrr(void) __irq;
extern void sec_wdog_init(void);
void xport_rpm_isr(void) __irq;
void dog_bark_isr(void) __irq;
__weak void vmpm_isr(void) __irq;
__weak void settling_timer_isr(void) __irq;

uint32 imem_info_data;

typedef void (*ExecFuncPtr)(void) __irq;

extern void __main(void);

void abort_isr(void) __irq
{
    abort();
}

void tiny_modem_abort_isr(void) __irq
{
	uint32 imem_info = in_dword(TINY_MODEM_IMEM_INFO_ADDR);
	uint16_t is_apps_collapse = (imem_info >> 0x5) & 0x1;
	if(is_apps_collapse)
	{
		HWIO_OUTF(MPM2_MPM_PS_HOLD,PSHOLD, 0x0);	
		abort();
	}	
}

void tz_abort_isr(void) __irq 
{
    SWEVENT(RPM_TZ_HALT_INT_RECEIVED);
    abort();
}
#ifdef MDM9x05_TARGET_CONFIG
//This needs to be aligned to at least 0x200 bytes (see arm documentation for why).
static ExecFuncPtr vector_table[16+64] __attribute__((at(0x090200))) =
{
  // RPM's standard system-level Cortex-M3 exceptions come first.
  (ExecFuncPtr)0,            				 // Initial stack pointer - not used
  (ExecFuncPtr)__main,                       // Image entry point
  abort_isr,                                 // NMI
  abort_isr,                                 // Hard fault
  abort_isr,                                 // Memory management
  abort_isr,                                 // Bus fault
  abort_isr,                                 // Usage fault
  0, 0, 0, 0,                                // Reserved
  abort_isr,                                 // SVC
  abort_isr,                                 // Debug Monitor
  0,                                         // Reserved
  abort_isr,                                 // PendSVC
  abort_isr,                                 // SysTick

  // RPM's NVIC interrupts start here.
  abort_isr,    // 0 Krait[0] shutdown_req
  abort_isr,    // 1 Krait[0] bringup_req
  abort_isr,    // 2 o_tcsr_xpu2_sec_summary_intr
  abort_isr,    // 3 q6_wdog_expired_irq
  abort_isr,    // 4 secure_wdog_bark_irq
  abort_isr,    // 5 pcnoc_sbm_mainFault
  abort_isr,    // 6 o_tcsr_xpu2_non_sec_summary_intr
  abort_isr,    // 7 o_tcsr_vmidmt_client_sec_summary_intr
  xport_rpm_isr,      // 8 Apps HLOS SMD interrupt
  vmpm_isr,     // 9 Apps HLOS VMPM interrupt
  abort_isr,    // 10 Apps HLOS reserved interrupt
  xport_rpm_isr,      // 11 TZ SMD interrupt
  tz_abort_isr, // 12 TZ HALT interrupt
  abort_isr,    // 13 TZ reserved interrupt
  abort_isr,    // 14 TZ reserved interrupt
  abort_isr,    // 15 o_tcsr_vmidmt_client_non_sec_summary
  abort_isr,    // 16 Krait dbgCorePwrUpInt
  abort_isr,    // 17 o_tcsr_vmidmt_cfg_sec_summary_intr
  abort_isr,    // 18 o_tcsr_vmidmt_cfg_non_sec_summary_intr
  abort_isr,    // 19 o_bimc_intr[0
  abort_isr,    // 20 o_bimc_intr[1]
  abort_isr,    // 21 reserved
  abort_isr,    // 22 reserved
  abort_isr,    // 23 o_bimc_intr[2]
  abort_isr,    // 24 MPSS shutdown_req
  abort_isr,    // 25 MPSS bringup_req
  xport_rpm_isr,      // 26 MPSS SMD interrupt
  vmpm_isr,     // 27 MPSS VMPM interrupt
  abort_isr,    // 28 MPSS reserved interrupt
  abort_isr,    // 29 top_cpr_irq[2]
  abort_isr,    // 30 APCS_sysWdogBiteInt[0]
  abort_isr,    // 31 qdss_tpda_gerr_irq
  abort_isr,    // 32 reserved
  abort_isr,     // 33 reserved
  abort_isr,    // 34 reserved
  abort_isr,    // 35 qdss_clk_irq
  abort_isr,    // 36 QDSS dbgSysPwrUpReq assertion
  abort_isr,    // 37 QDSS dbgSysPwrUpReq de-assertion
  abort_isr,    // 38 mmss_summary_intr
  abort_isr,    // 39 tlmm_summary_intr
  abort_isr,    // 40 tlmm_dir_conn_intr
  abort_isr,    // 41 tsense_upper_lower_intr
  abort_isr,    // 42 MPM	 summary interrupt
  abort_isr,    // 43 reserved
  abort_isr,    // 44 spdmo_realtime_irq[0]
  abort_isr,    // 45 reserved
  abort_isr,    // 46 reserved
  abort_isr,    // 47 SPMI command done
  abort_isr,    // 48 SPMI peripheral IRQ (a.k.a. PMIC interrupt)
  dog_bark_isr, // 49 WDOG bark IRQ
  abort_isr,    // 50 QTimer[0] IRQ
  settling_timer_isr,    // 51 QTimer[1] IRQ
  abort_isr,    // 52 Debug CTI[0]
  abort_isr,    // 53 Debug CTI[1]
  abort_isr,    // 54 reserved
  abort_isr,    // 55 reserved
  abort_isr,      // 56 task_done_int
  abort_isr,     // 57 crc_error_int
  abort_isr,    // 58 to_error_int
  abort_isr,    // 59 bus_timeout_irq
  abort_isr,    // 60 reserved
  abort_isr,    // 61 reserved
  abort_isr,    // 62 top_cpr_irq[0]
  abort_isr,    // 63 top_cpr_irq[1]
};

#else
//This needs to be aligned to at least 0x200 bytes (see arm documentation for why).
static ExecFuncPtr vector_table[16+64] __attribute__((at(0x090200))) =
{
  // RPM's standard system-level Cortex-M3 exceptions come first.
  (ExecFuncPtr)0,            				 // Initial stack pointer - not used
  (ExecFuncPtr)__main,                       // Image entry point
  abort_isr,                                 // NMI
  abort_isr,                                 // Hard fault
  abort_isr,                                 // Memory management
  abort_isr,                                 // Bus fault
  abort_isr,                                 // Usage fault
  0, 0, 0, 0,                                // Reserved
  abort_isr,                                 // SVC
  abort_isr,                                 // Debug Monitor
  0,                                         // Reserved
  abort_isr,                                 // PendSVC
  abort_isr,                                 // SysTick

  // RPM's NVIC interrupts start here.
  abort_isr,    // Krait[0] shutdown_req
  abort_isr,    // Krait[0] bringup_req
  abort_isr,    // Krait[1] shutdown_req
  abort_isr,    // Krait[1] bringup_req
  abort_isr,    // Krait[2] shutdown_req
  abort_isr,    // Krait[2] bringup_req
  abort_isr,    // Krait[3] shutdown_req
  abort_isr,    // Krait[3] bringup_req
  xport_rpm_isr,      // Apps HLOS SMD interrupt
  vmpm_isr,     // Apps HLOS VMPM interrupt
  abort_isr,    // Apps HLOS reserved interrupt
  xport_rpm_isr,      // TZ SMD interrupt
  tz_abort_isr, // TZ HALT interrupt
  abort_isr,    // TZ reserved interrupt
  abort_isr,    // Apps L2 Cache shutdown_req
  abort_isr,    // Apps L2 Cache bringup_req
  abort_isr,    // Krait dbgCorePwrUpInt
  abort_isr,    // <spare / reserved>
  abort_isr,    // reserved
  abort_isr,    // reserved
  abort_isr,    // reserved
  abort_isr,    // reserved
  abort_isr,    // reserved
  abort_isr,    // <spare / reserved>
  abort_isr,    // MPSS shutdown_req
  abort_isr,    // MPSS bringup_req
  xport_rpm_isr,      // MPSS SMD interrupt
  vmpm_isr,     // MPSS VMPM interrupt
  abort_isr,    // MPSS reserved interrupt
  abort_isr,    // top_cpr_irq[2]
  abort_isr,    // Pronto shutdown_req
  abort_isr,    // Pronto bringup_req
  xport_rpm_isr,      // Pronto SMD interrupt
  vmpm_isr,     // Pronto VMPM interrupt
  abort_isr,    // Pronto reserved interrupt
  abort_isr,    // qdss_clk_irq
  abort_isr,    // QDSS dbgSysPwrUpReq assertion
  abort_isr,    // QDSS dbgSysPwrUpReq de-assertion
  abort_isr,    // mmss_summary_intr
  abort_isr,    // tlmm_summary_intr
  abort_isr,    // tlmm_dir_conn_intr
  abort_isr,    // tsense_upper_lower_intr
  abort_isr,    // MPM summary interrupt
  abort_isr,    // mmss_cpr_irq[2]
  abort_isr,    // SPDM Realtime IRQ
  abort_isr,    // ce_core_irq
  abort_isr,    // ce_bam_irq
  abort_isr,    // SPMI command done
  abort_isr,    // SPMI peripheral IRQ (a.k.a. PMIC interrupt)
  dog_bark_isr, // WDOG bark IRQ
  abort_isr,    // QTimer[0] IRQ
  settling_timer_isr,    // QTimer[1] IRQ
  abort_isr,    // Debug CTI[0]
  abort_isr,    // Debug CTI[1]
  abort_isr,    // LPASS shutdown_req
  abort_isr,    // LPASS bringup_req
  xport_rpm_isr,      // LPASS SMD interrupt
  vmpm_isr,     // LPASS VMPM interrupt
  abort_isr,    // LPASS reserved interrupt
  abort_isr,    // bus_timeout_irq
  abort_isr,    // mmss_cpr_irq[0]
  abort_isr,    // mmss_cpr_irq[1]
  abort_isr,    // top_cpr_irq[0]
  abort_isr,    // top_cpr_irq[1]
};
#endif
void exceptions_init(void)
{
  int i, num_ints;

  // Disable interrupts globally.
  __disable_irq();

  // Default all RPM interrupts to rising edge.
  HWIO_OUT(RPM_INTR_POLARITY_0, 0xFFFFFFFF);
  HWIO_OUT(RPM_INTR_POLARITY_1, 0xFFFFFFFF);
  HWIO_OUT(RPM_INTR_EDG_LVL_0,  0xFFFFFFFF);
  HWIO_OUT(RPM_INTR_EDG_LVL_1,  0xFFFFFFFF);

  // Figure out how many interrupts are supported on this platform.
  num_ints = ((SCS.IntCtrlType & 0x1F) + 1) << 5;

  // Disable all interrupts.
  for(i = 0; i < num_ints / 32; ++i)
    SCS.NVIC.Disable[i] = 0xFFFFFFFF;

  // Clear all interrupts.
  for(i = 0; i < num_ints / 32; ++i)
    SCS.NVIC.Clear[i] = 0xFFFFFFFF;

  // Set all interrupt levels as pre-empting.
  SCS.AIRC = 0x05FA0000;

  // based on the size of vector_table and method of vector_table indexing (i (upto < num_ints) + 16)
  CORE_VERIFY( num_ints <= 64 );
  
  // Set all interrupts to lowest priority.
  for(i = 0; i < num_ints / 4; ++i)
    SCS.NVIC.Priority[i] = 0xFFFFFFFF;

  // Configure the exception vector table.
  SCS.ExceptionTableOffset = (unsigned)&vector_table;

  // Enable traps on divides by 0 and double-word stack alignment.
  SCS.ConfigCtrl |= 0x210;

  // Memory/Bus/Usage faults get highest user priority.
  SCS.SystemPriority[0] = 0;

  // Other system exceptions get middling priority.
  SCS.SystemPriority[1] = 0xC0000000; // SVC = 0xC0
  SCS.SystemPriority[2] = 0xFFFF0080; // SysTick and PendSV = 0xFF, Debug = 0x80

  // Enable the usage, bus, and memory management fault handlers.
  SCS.SystemHandlerCtrlAndState = 0x70000;

  // Now that we're all set, go through and re-enable interrupts that have handlers.
  for(i = 0; i < num_ints; ++i)
  {
    // Check for weakly linked handler that doesn't exist yet.
    // Note the +16 that skips system handlers.
    if(!vector_table[i + 16])
        vector_table[i + 16] = abort_isr;

    if(abort_isr != vector_table[i + 16])
      SCS.NVIC.Enable[i/32] = (1 << (i % 32));
  }

  // Give the watchdog bark IRQ the same priority as a fault.
  interrupt_set_priority(WATCHDOG_IRQ_NUM, 0);

  // Enable fault handling.
  set_basepri(0);
  INTLOCK();
#if (!defined(MPQ8092_STUBS))
  __enable_irq();
#endif
}

void exceptions_enable(void)
{
    INTFREE();
}

void interrupt_soft_trigger(unsigned num)
{
    CORE_VERIFY(num < 64);
    STIR.SoftwareTriggerInterrupt = num;
}

void interrupt_set_isr(unsigned num, isr_type isr)
{
    vector_table[16+num] = isr;
}

void interrupt_configure(unsigned num, interrupt_config config)
{
    volatile unsigned *RPM_INTERRUPT_POLARITY = (unsigned int *)HWIO_RPM_INTR_POLARITY_0_ADDR;
    volatile unsigned *RPM_INTERRUPT_EDGE = (unsigned int *)HWIO_RPM_INTR_EDG_LVL_0_ADDR;

    switch(config)
    {
        case RISING_EDGE:
            RPM_INTERRUPT_POLARITY[num/32] |=  (1 << (num % 32));
            RPM_INTERRUPT_EDGE[num/32]     |=  (1 << (num % 32));
            break;

        case FALLING_EDGE:
            RPM_INTERRUPT_POLARITY[num/32] &= ~(1 << (num % 32));
            RPM_INTERRUPT_EDGE[num/32]     |=  (1 << (num % 32));
            break;

        case LEVEL_HIGH:
            RPM_INTERRUPT_POLARITY[num/32] |=  (1 << (num % 32));
            RPM_INTERRUPT_EDGE[num/32]     &= ~(1 << (num % 32));
            break;

        case LEVEL_LOW:
            RPM_INTERRUPT_POLARITY[num/32] &= ~(1 << (num % 32));
            RPM_INTERRUPT_EDGE[num/32]     &= ~(1 << (num % 32));
            break;
    }
}

void interrupt_set_priority(unsigned num, unsigned priority)
{
    unsigned priority_word = SCS.NVIC.Priority[num/4];
    unsigned priority_offset = 8 * (num % 4);
    unsigned priority_mask = ~(0xff << priority_offset);

    SCS.NVIC.Priority[num/4] = (priority_word & priority_mask) | ((0xff & priority) << priority_offset);
}

unsigned intlock_nest_level = 0;

void lock_ints(void)
{
  unsigned _was_masked = get_basepri();
  set_basepri(0x10);
  assert(!intlock_nest_level || _was_masked);
  ++intlock_nest_level;
}

void unlock_ints(void)
{
  unsigned _is_masked = get_basepri();
  assert(intlock_nest_level && _is_masked);
  if(!--intlock_nest_level)
  {
    set_basepri(0);
  }
}

/*===========================================================================
FUNCTION: is_any_interrupt_pending

DESCRIPTION: 
ARM TRM says that ISRPENDING indicates a pending interrupt irrespective
of whether it is enabled or not. So This cannot be used for checking.
VECTPENDING gives the highest priority pending exception, but it also includes the 
effect of the BASEPRI and FAULTMASK registers, but not any effect of the PRIMASK register. 
We’re in an intlocked context, and we use BASEPRI as our intlock mechanism. 
BASEPRI will be 16, and thus VECTPENDING will effectively mask out all of the other interrupts.
Instead we check NVIC Enabled and Set registers to understand whether interrupts
are pending.

RETURN VALUE: True if any interrupt is pending
===========================================================================*/
bool is_any_interrupt_pending(void)
{
  int num_ints, i;
  bool result = false;
  num_ints = ((SCS.IntCtrlType & 0x1F) + 1) << 5;
  for(i = 0; ((i < num_ints / 32) && !result); i++)
  {
    if (SCS.NVIC.Enable[i] & SCS.NVIC.Set[i])
      result = true;
  }
  return result;
}

#define Q6_WDOG_BITE_INT 			 3
#define NOC_ERROR_INT				 5
#define BUS_TIMEOUT_INT 		     56
#define VMIDMT_INT_1				 7
#define VMIDMT_INT_2			     15
#define VMIDMT_INT_3                 17
#define VMIDMT_INT_4                 18
#define BIMC_INT_1                     19
#define BIMC_INT_2                     20
#define BIMC_INT_3                     23

/*
Handle the error Interrupts, if IMEM_TinyModemBootInfo & 0x10 == 0x10.
• Pet Secure-WDOG for Secure WDOG Bark
• For the Below Interrupts, Do a COLD Reset
	• Q6-WDOG-Bite 
	• NoC Errors
	• ABT
	• xPU/VMIDMT violations/Errors
	  BIMC Errors
*/
bool tiny_modem_init(void)
{
	imem_info_data = in_dword(TINY_MODEM_IMEM_INFO_ADDR);
	uint16_t is_tiny_modem  = imem_info_data & 0x1;                    // 0th Bit -> 1: Tiny Boot, 0: Full Boot
	uint16_t is_apps_loaded = ((imem_info_data >> 0x4) & 0x1);        // 4th Bit -> 1: APPS loaded 0: No APPS
	
	if(is_tiny_modem && (!is_apps_loaded))
	{
		sec_wdog_init();	
        interrupt_configure(Q6_WDOG_BITE_INT, LEVEL_HIGH);
        interrupt_configure(NOC_ERROR_INT, LEVEL_HIGH);
        interrupt_configure(BUS_TIMEOUT_INT, LEVEL_HIGH);
        interrupt_configure(VMIDMT_INT_1, LEVEL_HIGH);
        interrupt_configure(VMIDMT_INT_2, LEVEL_HIGH);
        interrupt_configure(VMIDMT_INT_3, LEVEL_HIGH);
        interrupt_configure(VMIDMT_INT_4, LEVEL_HIGH);
        interrupt_configure(BIMC_INT_1, LEVEL_HIGH);
        interrupt_configure(BIMC_INT_2, LEVEL_HIGH);
        interrupt_configure(BIMC_INT_3, LEVEL_HIGH);
        
        interrupt_set_isr(Q6_WDOG_BITE_INT, tiny_modem_abort_isr);
        interrupt_set_isr(NOC_ERROR_INT, tiny_modem_abort_isr);
        interrupt_set_isr(BUS_TIMEOUT_INT, tiny_modem_abort_isr);
        interrupt_set_isr(VMIDMT_INT_1, tiny_modem_abort_isr);
        interrupt_set_isr(VMIDMT_INT_2, tiny_modem_abort_isr);
        interrupt_set_isr(VMIDMT_INT_3, tiny_modem_abort_isr);
        interrupt_set_isr(VMIDMT_INT_4, tiny_modem_abort_isr);
        interrupt_set_isr(BIMC_INT_1, tiny_modem_abort_isr);
        interrupt_set_isr(BIMC_INT_2, tiny_modem_abort_isr);
        interrupt_set_isr(BIMC_INT_3, tiny_modem_abort_isr);

        interrupt_enable(Q6_WDOG_BITE_INT);
        interrupt_enable(NOC_ERROR_INT);
        interrupt_enable(BUS_TIMEOUT_INT);
        interrupt_enable(VMIDMT_INT_1);
        interrupt_enable(VMIDMT_INT_2);
        interrupt_enable(VMIDMT_INT_3);
        interrupt_enable(VMIDMT_INT_4);
        interrupt_enable(BIMC_INT_1);
        interrupt_enable(BIMC_INT_2);
        interrupt_enable(BIMC_INT_3);
		return TRUE;
	}
	return FALSE;
}   

