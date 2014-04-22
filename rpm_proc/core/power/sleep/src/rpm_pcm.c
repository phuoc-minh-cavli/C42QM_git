/*===========================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#include "rpmserver.h"
#include "ClockRPMNPA.h"
#include "ClockRPMBIMC.h"
#include "ddr_drivers.h"
#include "CoreVerify.h"
#include "HALhwio.h"
#include "HALmpm.h"
#include "mpm2_hwio.h"
#include "rpm_hwio.h"
#include "vmpm.h"
#include "mpm.h"

#include "pm_comm.h"

extern Clock_NPAResourcesType Clock_Resources;
extern int *enabled_interrupts;
extern void HAL_mpmint_All_Disable  ( void ); 

#define DDR_CLK_RATE 	120000		/* 120 Mhz */
#define DEFAULT_TIMEOUT	0x112A8800	/* 15 sec */

#define RPM_PCM_TIMEOUT	0x656d6974	/* "time" */

__asm static void halt(void)
{
    ; Lock interrupts via primask instead of basepri during sleep, or else we never wake up.
    mrs r0, basepri
    cpsid i
    mov r1, #0
    msr basepri, r1

    ; Do the halt, using ISB to clear the M3 store buffer
    dsb
    isb
    wfi

    ; Revert interrupt lock to basepri-based.
    msr basepri, r0
    cpsie i
    bx r14
}

typedef struct
{
  uint64 timeout;
} rpm_pcm_ctx;

static rpm_pcm_ctx gRpmPcmCtx;

void rpm_pcm_xlate(rpm_translation_info *info)
{
	uint32	type, length;
	uint64	*value;
		
	/* Read until we run out of KVPs. */
	while( !kvp_eof(info->new_kvps) )
	{
		kvp_get( info->new_kvps, &type, &length, (const char **)&value);
		if(length)
		{
			switch(type)
			{
				case RPM_PCM_TIMEOUT:
					CORE_VERIFY(sizeof(gRpmPcmCtx.timeout) == length);
					gRpmPcmCtx.timeout = *value;
					break;
				default:
					break;
			}
		}
	}
}

void rpm_configure_bimc(void)
{
	ClockResourceType	*pBIMCClk = &Clock_Resources.BIMCClockResource;

	/* Switch BIMC to 120M */
	Clock_BIMCSwitchFrequency(pBIMCClk, DDR_CLK_RATE);
}

void rpm_configure_ddr_self_refresh(void)
{
    /* Put DDR in self-refresh mode */
    ddr_enter_power_collapse(DDR_CLK_RATE);
}

void rpm_enter_hibernation(void)
{
	int i, num_masks = ((SCS.IntCtrlType & 0x1F) + 1);
	railway_settings cx_settings;
	railway_settings mx_settings;
	uint32_t  sleep_val;

	railway_get_current_settings(rail_id("vddcx"), &cx_settings);
    railway_get_current_settings(rail_id("vddmx"), &mx_settings);

    /* Set some dummy voltages , System is not expected to come back again */
    mpm_set_vmin_voltages(650000,mx_settings.microvolts,
                          650000,cx_settings.microvolts);

	mpm_setup_chip_sleep();

	/* Disable all MPM interrupts */
	HAL_mpmint_All_Disable();
 
	// Configure the MPM to honor the system's wakeup interrupts
	vmpm_prepare_for_sleep();

	// Save and disable all enabled interrupts
	for (i = 0; i < num_masks; i++)
	{
		enabled_interrupts[i] = SCS.NVIC.Enable[i];
		SCS.NVIC.Disable[i] = 0xFFFFFFFF;
	}

	// Enable the MPM interrupt so we can wake from sleep.
	interrupt_clear(MPM_IRQ); 
	interrupt_enable(MPM_IRQ); 
  
	//switch counter to sleep clock. 
	HWIO_OUTF(MPM2_MPM_CONTROL_CNTCR, FCREQ, 1);

	/* configure the MPM to halt the chip on wfi */
	sleep_val  = 1 << HWIO_SHFT(RPM_WFI_CONFIG, PROC_CLK_HALT);
  
	sleep_val |= 1 << HWIO_SHFT(RPM_WFI_CONFIG, CHIP_SLEEP_UPON_WFI);
	sleep_val |= 1 << HWIO_SHFT(RPM_WFI_CONFIG, BUS_CLK_HALT);
  
	HWIO_OUT(RPM_WFI_CONFIG, sleep_val);

	HAL_mpm_SetWakeUpTime(gRpmPcmCtx.timeout);

	halt();
  
	return;
}

void rpm_configure_psm_mode(void)
{
	pm_err_flag_type err_flag = PM_ERR_FLAG__INVALID;

    /* Dont let PS_HOLD toggle impact MSM */
    err_flag = pm_comm_write_byte( 0, 0x85B, 0x0, 1 );
	CORE_VERIFY(err_flag == PM_ERR_FLAG__SUCCESS);

    /* Spare register writes */
    err_flag = pm_comm_write_byte_mask( 0, 0x88E, 1, 1, 1);
	CORE_VERIFY(err_flag == PM_ERR_FLAG__SUCCESS);
}

void rpm_pcm_apply(rpm_application_info *info)
{
    /* Going into RBSC+ followed by system boot */

	/* Set BIMC to 120M */
    rpm_configure_bimc();

    /* Set DDR to self refresh */
    rpm_configure_ddr_self_refresh();

    /* program MPM command for new PMIC sequence,
		No need of other commands(ex: cx/mx reention etc) */
    /* trigger MPM sequence */
    rpm_configure_psm_mode();

	rpm_enter_hibernation();

    while(1);
}

void rpm_pcm_init(void)
{
	gRpmPcmCtx.timeout = DEFAULT_TIMEOUT;
    rpm_register_resource(RPM_PCM_REQ, 1, 4, rpm_pcm_xlate, rpm_pcm_apply, 0);
}
