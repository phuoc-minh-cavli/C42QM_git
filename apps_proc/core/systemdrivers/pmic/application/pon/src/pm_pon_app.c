/*! \file pm_pon_app.c
*  \n
*  \brief PON Application APIs implementation.
*  \n  
*  \n &copy; Copyright 2013, 2023 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                            Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/systemdrivers/pmic/application/pon/src/pm_pon_app.c#6 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
10/19/16   kc       Moved the ISR stuff to DSR (CR-1078087)
2/26/14    rh       Created file
========================================================================== */
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "DALSys.h"
#include "DDISpmi.h"
#include "pmapp_pon.h"
#include "pm_err_flags.h"
#include "pm_pon.h"
#include "timer.h"
#include "pm_target_information.h"
#include "dsr.h"

/*===========================================================================

                        GLOBAL VARIABLES DEFINITIONS

===========================================================================*/

static dsr_obj_t       *g_pon_dsr_obj;
uint32 					g_debug_pon_dsr_enque_fail;     		/* For Debugging */

typedef struct pm_misc_register_info_type
{
    pm_register_address_type    boot_option;
    pm_register_address_type    psm;
}pm_misc_register_info_type;

static pm_misc_register_info_type misc_reg =
{
    0x95D,  		//MDM_BOOT_OPTIONS
    0x960,          //PSM_ENTRY_EXIT
};
#define PM_MISC_SLAVE_ID 0
/*===========================================================================

                        FUNCTION PROTOTYPES

===========================================================================*/
/* DAL ISR callback */
static void* pon_isr(void* me, uint32 intrMask);
#ifdef FEATURE_TX_SP_AWARE_TRACKER
extern void (*qapi_pon_button_press_cb_fp)(int flag);
#endif
/* DSR Callback*/
static void pmapp_pon_dsr_callback(dsr_obj_t*, void *, void *);		

extern boolean  bPMICStub;

//Timer callback prototype
static void initiate_shutdown(timer_cb_data_type timer_cb_data);
#ifdef FEATURE_TX_SP_AWARE_TRACKER
static pmapp_pon_key_event_cb pwr_key_event_cb;
#else
static pmapp_pon_fn_cb shutdown_cb;
#endif
/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/

/* 
Graceful shutdown when power key is pressed for 1Sec and released
 
pmapp_pon_cblpwr_register_cb(): API called by sys_m_init() to initiate shutdown on 
                                long press of CBLPWR key (For 9205: KPDPWR Key)
pon_isr(): ISR for PON interrupt for falling/rising edge of CBLPWR key (For 9205: KPDPWR Key) 
initiate_shutdown(): Callback routine called when CBLPWR key (For 9205: KPDPWR Key)is long pressed for specified duration.
                     
*/
#ifdef FEATURE_TX_SP_AWARE_TRACKER
pm_err_flag_type pmapp_pon_cblpwr_register_cb(pmapp_pon_key_event_cb cb)
#else
pm_err_flag_type pmapp_pon_cblpwr_register_cb(pmapp_pon_fn_cb cb)
#endif
{
    static uint8 run_once = 0;
    static DalDeviceHandle *dalIntHandle = NULL;
    static pmapp_pon_isr_data isr_data;
    static struct timer_group_struct s_timer_group;
    static timer_type                s_timer;
    static timer_ptr_type            timer = &s_timer;
    static timer_group_ptr           timer_group = &s_timer_group; 
    static boolean                   timer_status = FALSE;
	boolean                          status   = FALSE;

    DALResult result = DAL_SUCCESS;
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

    uint32* debounce_duration = (uint32*)pm_target_information_get_specific_info(PM_PROP_CBLPWRN_DEBOUNCE_DURATION);

    if (bPMICStub == TRUE)
    {
        return err_flag;
    }

    if(debounce_duration == NULL)  
    {
        err_flag = PM_ERR_FLAG__INVALID_POINTER;
    }
    
    if(err_flag == PM_ERR_FLAG__SUCCESS && run_once == 0)
    {
#ifdef FEATURE_TX_SP_AWARE_TRACKER
        pwr_key_event_cb = cb;
		isr_data.duration = 5000;
#else
        shutdown_cb = cb;
        isr_data.duration = *debounce_duration;
#endif
        isr_data.timer = timer;
        isr_data.timer_status = &timer_status;

        timer_group_set_deferrable(timer_group, FALSE);
        timer_def_osal(timer,timer_group, TIMER_FUNC1_CB_TYPE, initiate_shutdown, (timer_cb_data_type)&isr_data); 

		
		//Imp Note.... For 9207: CBLPWR Key, For 9205: KPDPWR Key 
		
        //Configure PON interrupt for falling/rising edges
        if(err_flag == PM_ERR_FLAG__SUCCESS)
            err_flag = pm_pon_irq_set_trigger(0, PM_PON_IRQ_KPDPWR_ON, PM_IRQ_TRIGGER_DUAL_EDGE);

        //Clear PON interrupt
        if(err_flag == PM_ERR_FLAG__SUCCESS)
            err_flag = pm_pon_irq_clear(0, PM_PON_IRQ_KPDPWR_ON);

        //Enable PON interrupt
        if(err_flag == PM_ERR_FLAG__SUCCESS)
            err_flag = pm_pon_irq_enable(0, PM_PON_IRQ_KPDPWR_ON, TRUE);

        //Get DAL handle
        if(err_flag == PM_ERR_FLAG__SUCCESS)
            result = DAL_StringDeviceAttach("DALDEVICEID_SPMI_DEVICE", &dalIntHandle);

        if(result == DAL_SUCCESS)
            result = DalDevice_Open(dalIntHandle, DAL_OPEN_SHARED);

        //Configure ISR for PON module
        if(result == DAL_SUCCESS)
            result = DalSpmi_RegisterIsr(dalIntHandle, 0x008, 0xFF, pon_isr, &isr_data);

        if(result != DAL_SUCCESS)
            err_flag = PM_ERR_FLAG__DAL_SERVICE_REGISTRATION_FAILED;
			
		status = dsr_create(&g_pon_dsr_obj, (dsr_hdlr_t)pmapp_pon_dsr_callback, NULL, DSR_PRI_HIGH, DSR_CONTEXT_ISR);
		if(status != TRUE)
        {
			return PM_ERR_FLAG__DSR_SERVICE_FAIL; 
        }			

        run_once = 1;
    }

    return err_flag;
}

static void* pon_isr(void* me, uint32 intrMask)
{
    boolean dsr_status			 = FALSE;
    pmapp_pon_isr_data* isr_data = (pmapp_pon_isr_data*)me;
    dsr_status = dsr_enqueue(g_pon_dsr_obj, (void *)isr_data);

	if(TRUE !=dsr_status)
	{
		g_debug_pon_dsr_enque_fail++;
	}


    return NULL;
}

void pmapp_pon_dsr_callback(dsr_obj_t* dsr_obj, void *ctxt, void *payload)
{

	boolean status = FALSE; 
    pmapp_pon_isr_data* isr_data      = (pmapp_pon_isr_data*)payload;
	uint32 press_duration = 0;
	uint32 rem_time = 0;
#ifndef FEATURE_TX_SP_AWARE_TRACKER
        static boolean keypress = FALSE;
#endif
	
	//Imp Note.... For 9207: CBLPWR Key, For 9205: KPDPWR Key 
	
    pm_pon_irq_clear(0, PM_PON_IRQ_KPDPWR_ON);
    pm_pon_irq_status( 0, PM_PON_IRQ_KPDPWR_ON, PM_IRQ_STATUS_RT, &status);
	 
    if(status)
	{
#ifdef FEATURE_TX_SP_AWARE_TRACKER
		if (pwr_key_event_cb != NULL)
			((pmapp_pon_key_event_cb)pwr_key_event_cb)(1);

		press_duration = 0;
		if (qapi_pon_button_press_cb_fp != NULL)
			(qapi_pon_button_press_cb_fp)(PM_APP_KEY_PRESS) ; //for foxconn
#else      
        keypress = TRUE;
#endif
        //If the timer is not active this means that we've recieved an edge
        //indicating that the button mapping to CBLPWR_N (For 9205: KPDPWR Key) has been pushed.
        //Start the timer in this case
        timer_set_64(isr_data->timer, isr_data->duration, 0, T_MSEC);
        *(isr_data->timer_status) = TRUE;
    }
    else
    {
#ifdef FEATURE_TX_SP_AWARE_TRACKER
		rem_time = timer_clr(isr_data->timer, T_MSEC);
		if (pwr_key_event_cb != NULL)
			((pmapp_pon_key_event_cb)pwr_key_event_cb)(0);

		if (qapi_pon_button_press_cb_fp != NULL)
		{
			press_duration = isr_data->duration - rem_time;

			if(press_duration <= 2000)
				(qapi_pon_button_press_cb_fp)(PM_APP_KEY_RELEASE_SHORT); //for foxconn
			else 
				(qapi_pon_button_press_cb_fp)(PM_APP_KEY_RELEASE_LONG);
		}
#else
     
        //key released
        if(*(isr_data->timer_status) == TRUE)
        {
            //If the timer was already active this means that the user has let
            //go of the button and is aborting shutdown. Cancel the timer in this case.
            timer_clr(isr_data->timer, T_MSEC);
            *(isr_data->timer_status) = FALSE;
            keypress = FALSE;
         }
         else if(keypress == TRUE)
         {
            ((pmapp_pon_fn_cb)shutdown_cb)();
         }
#endif
    
    }

}

static void initiate_shutdown(timer_cb_data_type me)
{
	pmapp_pon_isr_data* isr_data = (pmapp_pon_isr_data*)me;
	//blink LED - OEMs

	//clear timer flag
	*(isr_data->timer_status) = FALSE;
			
}


pm_err_flag_type
pm_app_misc_get_cfg(pm_app_misc_cfg_type app_misc_cfg, uint8 *data)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  pm_register_address_type reg = 0;

  if(app_misc_cfg >= PM_APP_MISC_CFG_MAX || data == NULL)
  {
    return PM_ERR_FLAG__INVALID;
  }
  switch(app_misc_cfg)
  {
/*     case PM_APP_MISC_CFG_BOOT_OPTION:
         reg = misc_reg.boot_option;
         break; */                      
    case PM_APP_MISC_CFG_PSM:
         reg = misc_reg.psm;
         break;
    default:
         break;
  }
 
  err_flag = pm_comm_read_byte(PM_MISC_SLAVE_ID,reg, data, 0); 
  
  return err_flag;
}


pm_err_flag_type
pm_app_misc_set_cfg(pm_app_misc_cfg_type app_misc_cfg, uint8 mask, uint8 value)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  pm_register_address_type reg = 0;
  uint8 data = 0;
  
  if(app_misc_cfg >= PM_APP_MISC_CFG_MAX )
  {
    return PM_ERR_FLAG__INVALID;
  }
  switch(app_misc_cfg)
  {
/*     case PM_APP_MISC_CFG_BOOT_OPTION:
         reg = misc_reg.boot_option;
         break; */
    case PM_APP_MISC_CFG_PSM:
         reg = misc_reg.psm;
         break;
    default:
         break;
  }

  err_flag = pm_comm_read_byte(PM_MISC_SLAVE_ID,reg, &data, 0);         //read the MISC_MISC_PSM register
  data &= (~mask);                                                      // clear the bits which you are interested 
  data |= ( mask & value);                                              // set the cleared bits as you want
  err_flag |= pm_comm_write_byte(PM_MISC_SLAVE_ID,reg, data, 0);        // write back to MISC_MISC_PSM register
  
  return err_flag;
}

