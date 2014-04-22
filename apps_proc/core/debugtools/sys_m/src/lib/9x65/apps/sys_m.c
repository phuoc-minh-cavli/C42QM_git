/*===========================================================================

           S Y S _ M . C

DESCRIPTION

Copyright (c) 2011-2016 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

===========================================================================
DESCRIPTION
High level system monitor
===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/debugtools/sys_m/src/lib/9x65/apps/sys_m.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/02/16   nk      Changes to Update the notification id to correct value.
04/03/14   psu     Update diag macros using optimized versions.

===========================================================================*/

#include "dog.h"
#include "comdef.h"
#include "sys_m.h"
#include "task.h"
#include "rex.h"
#include <stringl/stringl.h>
#include "diagpkt.h"
#include "tms_diag.h"
#include "diagcmd.h"
#include "err.h"
#include "sys_m_messages.h"
#include "DDITlmm.h"
#include "DDIChipInfo.h"
#include "DDIGPIOInt.h"
#include "DALDeviceId.h"
#include "DDIPlatformInfo.h"
#include "DALSys.h"
#include "kernel.h"
#include "rcinit.h"
#include "sio.h"
#include "diagdsm_v.h"
#include "rcevt.h"
#include "hsu_event_notifier.h"
#include "smem.h"
#include "DDIInterruptController.h"
#include "DalDevice.h"
#include "DALStdErr.h"
#include "DDIChipPower.h"
#include "tms_dload.h"
#include "rcecb.h"
#include "qmi_ssctl_client.h"
#include "sys_m_pl_info_def.h"
#include "rdevmap.h"
#include "pmapp_pon.h"
#include "timer.h"
#include "stringl.h"
#include "sys_m_diag.h"
#include "sys_m_smp2p.h"

#include "msmhwio.h"
#include "msmhwiobase.h"

#define SUBSYS_MPSS  "modem"
#define SUBSYS_WCNSS "wcnss"
#define SUBSYS_DSPS  "dsps"
#define SUBSYS_GNSS  "gnss"
#define SUBSYS_EXT_MODEM  "ext_modem"
#define SUBSYS_LPSS "lpass"
#define SUBSYS_ADSP "adsp"


//Define the timeout to decide how long (in milliseconds) error handling should wait
//for Q6SW and modem to complete error handling
#ifndef SYS_M_PERIPHEHRAL_WAIT_TIMEOUT
    #define SYS_M_PERIPHEHRAL_WAIT_TIMEOUT 2000
#endif

//Each iteration is 100 ms
#define SYS_M_MAX_ITERATIONS (SYS_M_PERIPHEHRAL_WAIT_TIMEOUT/100)

TASK_EXTERN(sys_m)

#define SYS_M_VER_STR "info:version:0:1"

typedef struct {
   char *full_ssr_event;
   char *subsys_name;
   ssctl_ssr_event_enum_type_v02 e_ssr_evt;
} ssr_event_info;

ssr_event_info accepted_messages[] = {
   {SYS_M_SSR_MODEM_BEFORE_SHUTDOWN,SUBSYS_MPSS, SSCTL_SSR_EVENT_BEFORE_SHUTDOWN_V02},
   {SYS_M_SSR_WCNSS_BEFORE_SHUTDOWN,SUBSYS_WCNSS,SSCTL_SSR_EVENT_BEFORE_SHUTDOWN_V02},
   {SYS_M_SSR_DSPS_BEFORE_SHUTDOWN,SUBSYS_DSPS, SSCTL_SSR_EVENT_BEFORE_SHUTDOWN_V02},
   {SYS_M_SSR_GSS_BEFORE_SHUTDOWN,SUBSYS_GNSS,SSCTL_SSR_EVENT_BEFORE_SHUTDOWN_V02},
   {SYS_M_SSR_EXT_MODEM_BEFORE_SHUTDOWN,SUBSYS_EXT_MODEM,SSCTL_SSR_EVENT_BEFORE_SHUTDOWN_V02},
   {SYS_M_SSR_LPASS_BEFORE_SHUTDOWN,SUBSYS_LPSS,SSCTL_SSR_EVENT_BEFORE_SHUTDOWN_V02},
   {SYS_M_SSR_ADSP_BEFORE_SHUTDOWN,SUBSYS_ADSP, SSCTL_SSR_EVENT_BEFORE_SHUTDOWN_V02},

   {SYS_M_SSR_MODEM_AFTER_POWERUP,SUBSYS_MPSS, SSCTL_SSR_EVENT_AFTER_POWERUP_V02},
   {SYS_M_SSR_WCNSS_AFTER_POWERUP,SUBSYS_WCNSS,SSCTL_SSR_EVENT_AFTER_POWERUP_V02},
   {SYS_M_SSR_DSPS_AFTER_POWERUP,SUBSYS_DSPS,SSCTL_SSR_EVENT_AFTER_POWERUP_V02},
   {SYS_M_SSR_GSS_AFTER_POWERUP,SUBSYS_GNSS,SSCTL_SSR_EVENT_AFTER_POWERUP_V02},
   {SYS_M_SSR_EXT_MODEM_AFTER_POWERUP,SUBSYS_EXT_MODEM,SSCTL_SSR_EVENT_AFTER_POWERUP_V02},
   {SYS_M_SSR_LPASS_AFTER_POWERUP,SUBSYS_LPSS, SSCTL_SSR_EVENT_AFTER_POWERUP_V02},
   {SYS_M_SSR_ADSP_AFTER_POWERUP,SUBSYS_ADSP, SSCTL_SSR_EVENT_AFTER_POWERUP_V02}
   
};
#define SYS_M_NUM_NOTIFICATION sizeof(accepted_messages)/sizeof(ssr_event_info)

char *sys_m_msg_types[] = {
    "ssr",
    "info"
};
#define SYS_M_NUM_MSGTYPES sizeof(sys_m_msg_types)/sizeof(char*)

int sys_m_notification_id = -1;
extern int sys_m_shutdown_delay_secs;

static timer_type       shutdown_delay_timer;
static timer_group_type sd_delay_timer_group = {0};



#define SYS_M_BUFFER_LEN 50
char sys_m_buffer[SYS_M_BUFFER_LEN];
timer_type sys_m_reset_timer;
static timer_group_type sys_m_timer_group = {0};
rex_timer_type sys_m_ssr_timer;
int sys_m_reset_in_progress = 0;
int sys_m_true_reset = 0;
int sys_m_apps_timeout = 0;
boolean sys_m_reset_over_ssctl_v02 = FALSE;

DalDeviceHandle *hTlmm = NULL;
DalDeviceHandle *hGPIOInt = NULL;
DalDeviceHandle *phChipPower = NULL;

static rex_tcb_type *sys_m_tcb_ptr = NULL;

typedef struct
{
    sio_open_type open_properties;
    sio_open_param_type   open_param;

} sys_m_sio_params_t;

sys_m_gpio_map_t sys_m_gpio_map;
sys_m_sio_params_t sys_m_sio_param = {0};

sio_stream_id_type sys_m_sio_handle;
dsm_watermark_type sys_m_tx_wm_queue;
q_type sys_m_tx_q;

extern char sys_m_sfr_buffer[];
int sys_m_sfr_written = FALSE;
uint32 sys_m_buffer_size; //used in smem APIs

extern void HAL_sys_m_set_gpio(uint32 gpio_num);

#define HAL_TLMM_LOW_VALUE 0

#define SYS_M_ACK_SIG 0x01000000

#ifdef SYSM_SPI_NOR_TIMEOUTS
#define SYS_M_APPS_TIMEOUT_FUSION       40000
#define SYS_M_APPS_TIMEOUT_STANDALONE   40000
#else
#define SYS_M_APPS_TIMEOUT_FUSION       9500
#define SYS_M_APPS_TIMEOUT_STANDALONE   9500
#endif


#define AP2MDM_STATUS_GPIO_LIQUID_NUM       18

#define SYS_M_MDM2AP_STATUS_OUT_GPIO    DAL_GPIO_CFG(sys_m_gpio_map.MDM2AP_STATUS_GPIO, 0, DAL_GPIO_OUTPUT, DAL_GPIO_NO_PULL, DAL_GPIO_16MA)
#define SYS_M_MDM2AP_STATUS_OUT_GPIO_INACTIVE   DAL_GPIO_CFG_OUT(sys_m_gpio_map.MDM2AP_STATUS_GPIO, 0, DAL_GPIO_OUTPUT, DAL_GPIO_NO_PULL, DAL_GPIO_16MA, HAL_TLMM_LOW_VALUE)

#define SYS_M_MDM2AP_ERR_FATAL_OUT_GPIO    DAL_GPIO_CFG(sys_m_gpio_map.MDM2AP_ERR_FATAL_GPIO, 0, DAL_GPIO_OUTPUT, DAL_GPIO_NO_PULL, DAL_GPIO_2MA)
#define SYS_M_MDM2AP_ERR_FATAL_OUT_GPIO_INACTIVE   DAL_GPIO_CFG_OUT(sys_m_gpio_map.MDM2AP_ERR_FATAL_GPIO, 0, DAL_GPIO_OUTPUT, DAL_GPIO_NO_PULL, DAL_GPIO_2MA, HAL_TLMM_LOW_VALUE)

#define SYS_M_AP2MDM_STATUS_GPIO (DALGpioSignalType)DAL_GPIO_CFG(sys_m_gpio_map.AP2MDM_STATUS_GPIO, 0, DAL_GPIO_INPUT, DAL_GPIO_PULL_DOWN, DAL_GPIO_2MA)  
#define SYS_M_AP2MDM_STATUS_GPIO_INACTIVE (DALGpioSignalType)DAL_GPIO_CFG(sys_m_gpio_map.AP2MDM_STATUS_GPIO, 0, DAL_GPIO_INPUT, DAL_GPIO_PULL_DOWN, DAL_GPIO_2MA)

#define SYS_M_AP2MDM_ERR_FATAL_GPIO (DALGpioSignalType)DAL_GPIO_CFG(sys_m_gpio_map.AP2MDM_ERR_FATAL_GPIO, 0, DAL_GPIO_INPUT, DAL_GPIO_PULL_DOWN, DAL_GPIO_2MA)  
#define SYS_M_AP2MDM_ERR_FATAL_GPIO_INACTIVE (DALGpioSignalType)DAL_GPIO_CFG(sys_m_gpio_map.AP2MDM_ERR_FATAL_GPIO, 0, DAL_GPIO_INPUT, DAL_GPIO_PULL_DOWN, DAL_GPIO_2MA)

#define MPSS_DOG_ISR        56
#define LPASS_DOG_ISR       194

void sys_m_wdog_reset_isr(uint32 param);
void sys_m_AP_kernel_panic_isr(uint32 param);
void sys_m_soft_reset_isr(uint32 param);
void sys_m_signal_modules(qmi_ssctl_subsys_event_req_msg_v02 * ss_evt);
void sys_m_unknown_msgs(void);
void sys_m_shutdown_notify_internal(void);

static void sys_m_pmapp_pon_cblpwr_cb(void);
static void sys_m_rdm_open_cb(sio_port_id_type port_id);
static void sys_m_rdm_close_cb(void);
static void sys_m_sio_open(sio_port_id_type port_id);
static void sys_m_shutdown_terminate(timer_cb_data_type delay);



/**
 * @brief sys_m_send_AP_msg
 *
 * System monitor send message over HSIC
 *
 * @param[in] msg: Char buffer to send
 * @param[in] length: size of buffer
 * 
*/
void sys_m_send_AP_msg(char* msg, int length)
{
    dsm_item_type *my_dsm;
    int dsm_item_size = DSM_POOL_ITEM_SIZE(DSM_DS_LARGE_ITEM_POOL);

    if (DalPlatformInfo_IsFusion())
    {
        my_dsm = dsm_new_buffer (DSM_DS_LARGE_ITEM_POOL);
        if (my_dsm != NULL)
        {
            my_dsm->used = length + 1;/* +1 for the NULL character in the sfr string */
            strlcpy((char*)my_dsm->data_ptr, msg, dsm_item_size);
            sio_transmit (sys_m_sio_handle, my_dsm);
        }
    }
}

/**
 * @brief sys_m_sio_rx_notify
 *
 * System monitor HSIC receive callback
 *
 * @param[in] item_ptr: Data received
 * 
*/
void sys_m_sio_rx_notify (dsm_item_type ** item_ptr)
{
    uint32 bytes_sent = 0;
    
    if ((item_ptr == NULL) || (*item_ptr == NULL))
    {
        ERR_FATAL ("Error in sys_m_sio_rx_notify: bad parameter", 0,0,0);       
        return;
    }

    bytes_sent = (*item_ptr)->used;
    
    if (bytes_sent <= SYS_M_BUFFER_LEN)
    {
        memscpy(sys_m_buffer, SYS_M_BUFFER_LEN, (*item_ptr)->data_ptr, bytes_sent);
        rex_set_sigs(sys_m_tcb_ptr, SYS_M_AP_NOTIFY_RCV); 
    }
    else
    {
        //should we move to error state or just ignore the message?
        ERR_FATAL ("Error in sys_m_sio_rx_notify: invalid data size", 0,0,0);         
    }

    dsm_free_packet(item_ptr);
}

/**
 *
 * @brief sys_m_send_sfr
 *
 * Send the SFR
 * 
*/
void sys_m_send_sfr()
{
    sys_m_send_AP_msg(sys_m_sfr_buffer, strlen(sys_m_sfr_buffer));
}

/**
 *
 * @brief sys_m_reset
 *
 * Triggers the reset sequence of the full MDM
 * 
*/
void sys_m_reset(void)
{
    sys_m_true_reset = 1;
    rex_set_sigs(sys_m_tcb_ptr, SYS_M_INITIATE_RESET);
}

/**
 *
 * @brief sys_m_shutdown
 *
 * Triggers the poweroff sequence of the full MDM
 * 
*/
void sys_m_shutdown(void)
{
    rex_set_sigs(sys_m_tcb_ptr, SYS_M_INITIATE_RESET);
}

/**
 *
 * @brief sys_m_complete_reset
 *
 * Triggers the complete reset sequence of the full MDM
 * 
*/
void sys_m_complete_reset(void)
{
    rex_set_sigs(sys_m_tcb_ptr, SYS_M_COMPLETE_RESET);
}

/**
 *
 * @brief sys_m_pmapp_pon_cblpwr_cb
 *
 * Callback for a power button interrupt from pmic
*/
static void sys_m_pmapp_pon_cblpwr_cb(void)
{
    //pmic has requested a poweroff
    if (DalPlatformInfo_IsFusion())
    {
      //use case not defined
      MSG_ERROR("pmapp_pon_cblpwr interrupt ignored (fusion)",0,0,0);
    }
    else
    {
      //enter graceful poweroff sequence
      MSG(MSG_SSID_TMS, MSG_LEGACY_HIGH,PROC_ID"pmapp_pon_cblpwr interrupt driving shutdown");
      sys_m_shutdown();
    }
}

/**
 *
 * @brief lpass_wdog_bite_isr
 *
 * ISR for LPASS watchdog bite
 * 
*/
void lpass_wdog_bite_isr()
{
    memset(sys_m_sfr_buffer, 0, SYS_M_SFR_LENGTH);
    strlcpy(sys_m_sfr_buffer, "ssr:return:MDM-LPASS:Wdog bite", SYS_M_SFR_LENGTH);
    sys_m_sfr_written = TRUE;
    ERR_FATAL("LPASS wdog bite", 0, 0, 0);
}

/**
 *
 * @brief modem_wdog_bite_isr
 *
 * ISR for modem watchdog bite
 * 
*/
void modem_wdog_bite_isr()
{
    memset(sys_m_sfr_buffer, 0, SYS_M_SFR_LENGTH);
    strlcpy(sys_m_sfr_buffer, "ssr:return:MDM-Q6SW:Wdog bite", SYS_M_SFR_LENGTH);
    sys_m_sfr_written = TRUE;
    ERR_FATAL("Modem wdog bite", 0, 0, 0);
}

/**
 *
 * @brief sys_m_kernel_panic_isr
 *
 * Kernel panic ISR
 *
 * @param[in] param: ISR 
 *
*/
void sys_m_AP_kernel_panic_isr(uint32 param)
{
    ERR_FATAL("Modem restarted due to an APQ error", 0, 0, 0);
}

/**
 *
 * @brief sys_m_wdog_reset_isr
 *
 * Watchdog reset isr
 *
 * @param[in] param: ISR 
 *
*/
void sys_m_wdog_reset_isr(uint32 param)
{
    ERR_FATAL("Modem restarted due to APQ restart", 0, 0, 0);
}

/**
 *
 * @brief sys_m_send_ack_n_dreg_sts_gpio
 *
 * Send message to AP and dereegister for SYS_M_AP2MDM_STATUS_GPIO
 *
 * @param[in] param: msg 
 *
*/
static void sys_m_send_ack_n_dreg_sts_gpio(char *msg)
{
    if(DalPlatformInfo_IsFusion()&&hTlmm){
        DalTlmm_ConfigGpio(hTlmm, SYS_M_AP2MDM_STATUS_GPIO, DAL_TLMM_GPIO_DISABLE );
    }
    sys_m_send_AP_msg(msg, strlen(msg)); 
}
/**
 *
 * @brief sys_m_init_hsic
 *
 * System monitor initialize HSIC
 * 
*/
void sys_m_init_hsic(void)
{
    /* Clear SIO info structure */
    sys_m_sio_handle = SIO_NO_STREAM_ID;

    /* Initialize watermark queue for SIO
    */
    dsm_queue_init (
      &sys_m_tx_wm_queue,  /* watermark queue */
      DSM_DIAG_SIO_TX_ITEM_CNT * DSM_DIAG_SIO_TX_ITEM_SIZ, /* dont_exceed_cnt */
      &sys_m_tx_q /* queue */);

    dsm_set_low_wm (
      &sys_m_tx_wm_queue,
      DSM_DIAG_SIO_TX_ITEM_CNT * DSM_DIAG_SIO_TX_ITEM_SIZ);

    dsm_set_hi_wm (
      &sys_m_tx_wm_queue,
      DSM_DIAG_SIO_TX_ITEM_CNT * DSM_DIAG_SIO_TX_ITEM_SIZ);

    /* Register the open/close callbacks with RDM */
    rdm_register_open_func(RDM_SYSM_SRVC, sys_m_rdm_open_cb);
    rdm_register_close_func(RDM_SYSM_SRVC, sys_m_rdm_close_cb);
    
    /* After registering func, let RDM know list of compatible device
    ** This will prevent rdm_assign_port() for other device than below
    */
    rdm_set_compatibility(RDM_SYSM_SRVC, RDM_USB_SER3_DEV);
    rdm_set_compatibility(RDM_SYSM_SRVC, RDM_MHI_SSR_DEV);

}

/**
 *
 * @brief sys_m_hsusb_init_notif_cb
 *
 * System monitor callback which is called when HSIC is initialized
 * 
*/
void sys_m_hsusb_init_notif_cb(void)
{
    sys_m_init_hsic();
}

/**
 *
 * @brief sys_m_fusion3_init
 *
 * System monitor function to init fusion configuration
 * 
*/
void sys_m_fusion3_init(void)
{
    hsu_event_notif_cb_info_struct hsu_event_info;
    hsu_event_info.event_cb = sys_m_hsusb_init_notif_cb;
    hsu_event_info.event = HSU_EVENT_INIT_COMPLETED;
   
   
    if(FALSE == hsu_register_event_notif_cb(hsu_event_info) ) 
    {
        ERR_FATAL("Sys_m hsusb notification reg failed\n", 0, 0, 0);
    }   
    
    if (strncmp(sys_m_sfr_buffer, "ssr:return:", 10) != 0)
        strlcpy(sys_m_sfr_buffer, "ssr:return:", SYS_M_SFR_LENGTH);
        
    sys_m_apps_timeout  = SYS_M_APPS_TIMEOUT_FUSION; 
}
    
void sys_m_gpio_init(void)
{
    DALResult dRes, eResult;
    DalDeviceHandle      *phPFormInfo = NULL;
    DALSYSPropertyVar    propValue;
    SYS_M_PL_INFO *plInfoStructPtr = NULL;
    DALSYS_PROPERTY_HANDLE_DECLARE(pHandle);
    DalPlatformInfoPlatformInfoType sys_m_platform_info;
    
    //initialize struct to satisfy klocwork
    sys_m_platform_info.subtype = 0;
    sys_m_platform_info.platform = DALPLATFORMINFO_TYPE_UNKNOWN;
    sys_m_platform_info.version =  0;

    // only configure GPIOs for fusion
    if (!DalPlatformInfo_IsFusion())
    {
        return;
    }

    eResult = DAL_DeviceAttach(DALDEVICEID_PLATFORMINFO, &phPFormInfo);

    if (DAL_SUCCESS == eResult)
    {
        DalDevice_Open (phPFormInfo, DAL_OPEN_SHARED);
        DalPlatformInfo_GetPlatformInfo (phPFormInfo, &sys_m_platform_info );
        DalDevice_Close (phPFormInfo);
    }
        
    DALSYS_GetDALPropertyHandleStr("tms_sys_m", pHandle);

    //read GPIO numbers from dalconfig
    if (DAL_SUCCESS == DALSYS_GetPropertyValue(pHandle, "pl_info_struct_arr", 0, &propValue))
    {
        plInfoStructPtr = (SYS_M_PL_INFO*)propValue.Val.pStruct;
        
        //Loop thru xml config to match platform, version info
        while (plInfoStructPtr->platform != 0xFF) 
        {
           if( (sys_m_platform_info.platform == plInfoStructPtr->platform) &&
               (sys_m_platform_info.version == plInfoStructPtr->version) )
           {
              break;
           }
           plInfoStructPtr++;
        }
        
        if(plInfoStructPtr->platform == 0xFF)
        {
           ERR_FATAL("Sys_m cannot configure GPIOs, no DAL match", 0, 0, 0);
        }

        sys_m_gpio_map.MDM2AP_STATUS_GPIO = plInfoStructPtr->mdm2ap_status_gpio;
        sys_m_gpio_map.MDM2AP_ERR_FATAL_GPIO = plInfoStructPtr->mdm2ap_err_fatal_gpio;
        sys_m_gpio_map.AP2MDM_STATUS_GPIO = plInfoStructPtr->ap2mdm_status_gpio;
        sys_m_gpio_map.AP2MDM_ERR_FATAL_GPIO = plInfoStructPtr->ap2mdm_err_fatal_gpio;
    }
    else
    {
       ERR_FATAL("Sys_m cannot configure GPIOs, DAL failure", 0, 0, 0);
    }
    
    dRes = DAL_DeviceAttach(DALDEVICEID_TLMM, &hTlmm);
    if (dRes != DAL_SUCCESS)
    {
        ERR_FATAL("System monitor could not register with DAL", 0, 0, 0);
    }
    DalDevice_Open(hTlmm, DAL_OPEN_SHARED);
    
    //set inactive configs
    dRes = DalTlmm_SetInactiveConfig(hTlmm, sys_m_gpio_map.MDM2AP_ERR_FATAL_GPIO, SYS_M_MDM2AP_ERR_FATAL_OUT_GPIO_INACTIVE);
    if (dRes != DAL_SUCCESS)
    {
        ERR_FATAL("System monitor could not set inactive GPIO", 0, 0, 0);
    }
    dRes = DalTlmm_SetInactiveConfig(hTlmm, sys_m_gpio_map.MDM2AP_STATUS_GPIO, SYS_M_MDM2AP_STATUS_OUT_GPIO_INACTIVE);
    if (dRes != DAL_SUCCESS)
    {
        ERR_FATAL("System monitor could not set inactive GPIO", 0, 0, 0);
    }
    
    //config GPIOs
    dRes = DalTlmm_ConfigGpio(hTlmm, (DALGpioSignalType) SYS_M_MDM2AP_ERR_FATAL_OUT_GPIO, DAL_TLMM_GPIO_ENABLE);
    if (dRes != DAL_SUCCESS)
    {
        ERR_FATAL("System monitor could not config GPIO", 0, 0, 0);
    }
    dRes = DalTlmm_ConfigGpio(hTlmm, (DALGpioSignalType) SYS_M_MDM2AP_STATUS_OUT_GPIO, DAL_TLMM_GPIO_ENABLE);
    if (dRes != DAL_SUCCESS)
    {
        ERR_FATAL("System monitor could not config GPIO", 0, 0, 0);
    }
    
    //set ERR_FATAL low
    dRes = DalTlmm_GpioOut(hTlmm, (DALGpioSignalType) SYS_M_MDM2AP_ERR_FATAL_OUT_GPIO, DAL_GPIO_LOW_VALUE);
    if (dRes != DAL_SUCCESS)
    {
        ERR_FATAL("System monitor could not set GPIO low", 0, 0, 0);
    }
    //set STATUS high
    dRes = DalTlmm_GpioOut(hTlmm, (DALGpioSignalType) SYS_M_MDM2AP_STATUS_OUT_GPIO, DAL_GPIO_HIGH_VALUE);
    if (dRes != DAL_SUCCESS)
    {
        ERR_FATAL("System monitor could not set GPIO high", 0, 0, 0);
    }
    
    //set ISRs for apps wdog/kernel panic/soft reset
    dRes = DalTlmm_ConfigGpio(hTlmm, SYS_M_AP2MDM_ERR_FATAL_GPIO, DAL_TLMM_GPIO_ENABLE );
    if (dRes != DAL_SUCCESS)
    {
        ERR_FATAL("System monitor could not config GPIO", 0, 0, 0);
    }
    dRes = DalTlmm_ConfigGpio(hTlmm, SYS_M_AP2MDM_STATUS_GPIO, DAL_TLMM_GPIO_ENABLE );
    if (dRes != DAL_SUCCESS)
    {
        ERR_FATAL("System monitor could not config GPIO", 0, 0, 0);
    }
    dRes = DalTlmm_SetInactiveConfig(hTlmm, sys_m_gpio_map.AP2MDM_ERR_FATAL_GPIO, SYS_M_AP2MDM_ERR_FATAL_GPIO_INACTIVE);
    if (dRes != DAL_SUCCESS)
    {
        ERR_FATAL("System monitor could not set inactive GPIO", 0, 0, 0);
    }
    dRes = DalTlmm_SetInactiveConfig(hTlmm, sys_m_gpio_map.AP2MDM_STATUS_GPIO, SYS_M_AP2MDM_STATUS_GPIO_INACTIVE);
    if (dRes != DAL_SUCCESS)
{
        ERR_FATAL("System monitor could not set inactive GPIO", 0, 0, 0);
    }

    dRes = DAL_DeviceAttach(DALDEVICEID_GPIOINT, &hGPIOInt);
    if (dRes != DAL_SUCCESS)
    {
        ERR_FATAL("System monitor could not register with GPIOInt DAL", 0, 0, 0);
    }

    if ( DAL_SUCCESS != GPIOInt_RegisterIsrEx(hGPIOInt, sys_m_gpio_map.AP2MDM_ERR_FATAL_GPIO, GPIOINT_TRIGGER_RISING, (GPIOINTISR) sys_m_AP_kernel_panic_isr, NULL, GPIOINT_CONFIG_PRIORITY(DALINTRCTRL_PRIO_HIGHEST))
      || DAL_SUCCESS != GPIOInt_RegisterIsrEx(hGPIOInt, sys_m_gpio_map.AP2MDM_STATUS_GPIO, GPIOINT_TRIGGER_FALLING, (GPIOINTISR) sys_m_wdog_reset_isr, NULL, GPIOINT_CONFIG_PRIORITY(DALINTRCTRL_PRIO_HIGHEST)))
    {
        ERR_FATAL("System monitor could not Configure GPIOs", 0, 0, 0);
    }
    
    // Check if APQ crashed while we were coming up
    {
        DALGpioValueType fatal_value, status_value = DAL_PLACEHOLDER_DALGpioValueType;

        if ( DAL_SUCCESS != DalTlmm_GpioIn(hTlmm, SYS_M_AP2MDM_ERR_FATAL_GPIO, &fatal_value)
          || DAL_SUCCESS != DalTlmm_GpioIn(hTlmm, SYS_M_AP2MDM_STATUS_GPIO, &status_value ))
        {
            ERR_FATAL("System monitor could not read AP2MDM GPIOs", 0, 0, 0);
        }

        if (DAL_GPIO_LOW_VALUE == status_value)
        {
            ERR_FATAL("System monitor init with low AP2MDM_STATUS_GPIO", 0,0,0);
        }

        if (DAL_GPIO_HIGH_VALUE == fatal_value)
        {
            ERR_FATAL("System monitor init with high AP2MDM_ERR_FATAL_GPIO", 0,0,0);
        }
        MSG(MSG_SSID_TMS, MSG_LEGACY_MED,PROC_ID"AP2MDM status and err_fatal gpios indicate stable boot");
    }
 
}

/**
 *
 * @brief sys_m_adsp_errfatal_cb
 *
 * Callback for adsp error fatal
 * 
*/
void sys_m_adsp_errfatal_cb(void)
{
    char *rcv_buff;
    memset(sys_m_sfr_buffer, 0, SYS_M_SFR_LENGTH);
    strlcpy(sys_m_sfr_buffer, "ssr:return:MDM-ADSP:", SYS_M_SFR_LENGTH);
    rcv_buff = smem_get_addr(SMEM_SSR_REASON_LPASS0, &sys_m_buffer_size);
    if (rcv_buff == NULL)
        strlcat(sys_m_sfr_buffer, "No SFR data available", SYS_M_SFR_LENGTH);
    else
        strlcat(sys_m_sfr_buffer, rcv_buff, SYS_M_SFR_LENGTH);
    sys_m_sfr_written = TRUE;
    ERR_FATAL("ADSP error fatal", 0, 0, 0);
}

/**
 *
 * @brief sys_m_mpss_errfatal_cb
 *
 * Callback for modem error fatal
 * 
*/
void sys_m_mpss_errfatal_cb(void)
{
    char *rcv_buff;
    memset(sys_m_sfr_buffer, 0, SYS_M_SFR_LENGTH);
    strlcpy(sys_m_sfr_buffer, "ssr:return:MDM-MPSS:", SYS_M_SFR_LENGTH);
    rcv_buff = smem_get_addr(SMEM_SSR_REASON_MSS0, &sys_m_buffer_size);
    if (rcv_buff == NULL)
        strlcat(sys_m_sfr_buffer, "No SFR data available", SYS_M_SFR_LENGTH);
    else
        strlcat(sys_m_sfr_buffer, rcv_buff, SYS_M_SFR_LENGTH);
    sys_m_sfr_written = TRUE;
    ERR_FATAL("MPSS error fatal", 0, 0, 0);
}


/**
 *
 * @brief sys_m_set_gpio
 *
 * System monitor function to raise MDM2AP_ERRFATAL in fusion configs
 * 
*/
void sys_m_set_gpio(void)
{
    if (DalPlatformInfo_IsFusion())
    {
       HAL_sys_m_set_gpio(sys_m_gpio_map.MDM2AP_ERR_FATAL_GPIO);
    }
}


/**
 *
 * @brief sys_m_get_action
 *
 * System monitor forces the action as DLOAD for fusion configs
 * 
*/
err_action_type sys_m_get_action(void)
{    
    if (DalPlatformInfo_IsFusion())
    {
      return ERR_DLOAD;
    }

    return ERR_NO_ACTION;
}

/**
 *
 * @brief sys_m_init
 *
 * System monitor initialization function
 * 
*/
void sys_m_init(void)
{
    DalDeviceHandle      *hInthandle = NULL;
    
    sys_m_apps_timeout  = SYS_M_APPS_TIMEOUT_STANDALONE;
    
    
    //register dog bite ISRs
    if ((DAL_DeviceAttach(DALDEVICEID_INTERRUPTCONTROLLER, &hInthandle) != DAL_SUCCESS) || (hInthandle == NULL))
        ERR_FATAL("Sys_m cannot register with DAL", 0, 0, 0);
    DalInterruptController_RegisterISR(hInthandle, MPSS_DOG_ISR, (DALIRQ)modem_wdog_bite_isr, 0, DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER);
    //DalInterruptController_RegisterISR(hInthandle, LPASS_DOG_ISR, (DALIRQ)lpass_wdog_bite_isr, 0, DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER);/*since on 9x45 we dont have LPASS*/
       
    //attach to platform
    if ((DAL_DeviceAttach(DALDEVICEID_CHIPPOWER,&phChipPower) != DAL_SUCCESS) || (phChipPower == NULL))
        ERR_FATAL("Sys_m cannot register with DAL", 0, 0, 0);

    //Register to receive PMIC power button interrupt (power buton pressed for one second == poweroff)
    if (PM_ERR_FLAG__SUCCESS != pmapp_pon_cblpwr_register_cb(sys_m_pmapp_pon_cblpwr_cb))
    {
      ERR_FATAL("Could not register pmapp_pon_cblpwr callback",0,0,0);
    }

    if (DalPlatformInfo_IsFusion())
    {
        sys_m_fusion3_init();
    }

    sys_m_diag_init();
}

void sys_m_update_notification_id(char *evt_str)
{
   int i;
   MSG_SPRINTF_1(MSG_SSID_TMS, MSG_LEGACY_HIGH,PROC_ID"update_notification_id = %s", evt_str);
   for (i = 0; i < SYS_M_NUM_NOTIFICATION; i++)
   {
       if (strncmp(evt_str, accepted_messages[i].full_ssr_event, SYS_M_BUFFER_LEN) == 0)
       {
           sys_m_notification_id = i;
           MSG_1(MSG_SSID_TMS, MSG_LEGACY_HIGH,PROC_ID"found sys_m_notification_id = %d",sys_m_notification_id);
           break;
       }
   }
}

/**
 *
 * @brief sys_m_task
 *
 * System Monitor main task
 *
 * @param[in] param: Task init parameter 
 *
*/
void sys_m_task(unsigned long param)
{
    rex_sigs_type sigs;
    SYS_M_SPECIAL_MSG_TYPE sys_m_msg_type;
    
    //avoid warnings
    param = param;

    //block for start signal
    rcinit_handshake_startup();
    
    sys_m_tcb_ptr = rex_self();
    
//    rex_def_timer(&sys_m_reset_timer, rex_self(), SYS_M_COMPLETE_RESET);
    timer_group_set_deferrable(&sys_m_timer_group, FALSE);
    timer_def(&sys_m_reset_timer, &sys_m_timer_group, rex_self(), SYS_M_COMPLETE_RESET, NULL, NULL);
    
    //task forever loop
    for (;;)
    {
        sigs = rex_wait(SYS_M_INITIATE_RESET | SYS_M_COMPLETE_RESET | SYS_M_AP_NOTIFY_RCV);
        //mode reset -- prepare modem for reset/shutdown
        MSG_1(MSG_SSID_TMS, MSG_LEGACY_HIGH,PROC_ID"event received at sys_m_task %d",sigs);
        if (sigs & SYS_M_INITIATE_RESET)
        {
            (void) rex_clr_sigs( rex_self(), SYS_M_INITIATE_RESET );
            if (!sys_m_reset_in_progress)
            {
                sys_m_reset_in_progress = 1; /*once set this is never cleared by design*/
                ssctl_v02_send_req(MPSS_SSCTL_SVC, SSCTL_SHUTDOWN, NULL);
                sys_m_reset_over_ssctl_v02 = TRUE;

                timer_set(&sys_m_reset_timer, sys_m_apps_timeout, 0, T_MSEC);
            }
        }
        //physically reset/shutdown chip
        else if (sigs & SYS_M_COMPLETE_RESET)
        {
            timer_clr(&sys_m_reset_timer, T_NONE);
            (void) rex_clr_sigs(rex_self(), SYS_M_COMPLETE_RESET);
            //notify internally (blocks until complete)
            sys_m_shutdown_notify_internal();

            //compete shutdown (WILL return if there is a delay)
            sys_m_shutdown_terminate(sys_m_shutdown_delay_secs);
        }
        //message received from AP over HSIC
        else if (sigs & SYS_M_AP_NOTIFY_RCV)
        {
            (void) rex_clr_sigs(rex_self(), SYS_M_AP_NOTIFY_RCV);
            sys_m_msg_type = SYS_M_UNKNOWN_MSG;
            //look for nonspecial messages
            sys_m_update_notification_id(sys_m_buffer);
            MSG_SPRINTF_1(MSG_SSID_TMS, MSG_LEGACY_HIGH,": SSUBSYS_EVENT = %s", sys_m_buffer);
            if(sys_m_notification_id!= -1){
               sys_m_msg_type = SYS_M_NONSPECIAL_MSG;
            }
            //look for special messages
            
            //reset message
            if (strncmp(sys_m_buffer, SYS_M_SHUTDOWN, SYS_M_BUFFER_LEN) == 0)
            {
                sys_m_msg_type = SYS_M_SHUTDOWN_MSG;
                sys_m_send_ack_n_dreg_sts_gpio(SYS_M_SHUTDOWN_ACK);
            }
            //reset message
            if (strncmp(sys_m_buffer, SYS_M_SYSTEM_RESET, SYS_M_BUFFER_LEN) == 0) 
            {
                sys_m_msg_type = SYS_M_SHUTDOWN_MSG;
                sys_m_send_ack_n_dreg_sts_gpio(SYS_M_SYSTEM_RESET_ACK);
            }
            //shutdown/poweroff message
            if (strncmp(sys_m_buffer, SYS_M_POWEROFF, SYS_M_BUFFER_LEN) == 0)
            {
                sys_m_msg_type = SYS_M_POWEROFF_MSG;
                sys_m_send_ack_n_dreg_sts_gpio(SYS_M_POWEROFF_ACK);
            }
            //shutdown/poweroff message
            if (strncmp(sys_m_buffer, SYS_M_SYSTEM_SHUTDOWN, SYS_M_BUFFER_LEN) == 0) 
            {
                sys_m_msg_type = SYS_M_POWEROFF_MSG;
                sys_m_send_ack_n_dreg_sts_gpio(SYS_M_SYSTEM_SHUTDOWN_ACK);
            }
            if (strncmp(sys_m_buffer, SYS_M_SSR_FUSION_SFR, SYS_M_BUFFER_LEN) == 0)
                sys_m_msg_type = SYS_M_SFR_MSG;
            switch(sys_m_msg_type)
            {
                case SYS_M_NONSPECIAL_MSG:
                  {
                     MSG(MSG_SSID_TMS, MSG_LEGACY_HIGH,PROC_ID" notify modules");

                     sys_m_signal_modules(NULL);
                     sys_m_notification_id = -1;
                      //tell AP that notification is serviced
                     sys_m_send_AP_msg(SYS_M_ACK, strlen(SYS_M_ACK));

                     }
                    break;
                case SYS_M_SHUTDOWN_MSG:
                    sys_m_reset();
                    break;
                case SYS_M_POWEROFF_MSG:
                    sys_m_shutdown();
                    break;
                case SYS_M_SFR_MSG:
                    sys_m_send_sfr();
                    break;
                case SYS_M_UNKNOWN_MSG:
                    sys_m_unknown_msgs();
                    break;
 
            }
        }
    }

}    

/**
 *
 * @brief sys_m_signal_modules
 *
 * Signal the MDM peripherals
 * 
*/
void sys_m_signal_modules(qmi_ssctl_subsys_event_req_msg_v02 * ss_evt)
{
    int orig_ack_count = 0;
    int req_count = 0;
    ssctl_status_t ret = SSCTL_FAILURE;
    //NOTIFY MPSS
    if(ss_evt!=NULL){
        /*ret = ssctl_v02_send_req(ADSP_SSCTL_SVC, SSCTL_SUBSYS_EVENT, ss_evt);*///since there is no ADSP on 9x45
        ret = ssctl_v02_send_req(MPSS_SSCTL_SVC, SSCTL_SUBSYS_EVENT, ss_evt);
        MSG_1(MSG_SSID_TMS, MSG_LEGACY_HIGH,PROC_ID"sys_m_signal_modules MPSS_SSCTL_SVC ret =%d ", ret);
    }
    else if(sys_m_notification_id!=-1){
        /*need to construct the  qmi_ssctl_subsys_event_req_msg_v02  */
        qmi_ssctl_subsys_event_req_msg_v02 t_ss_evt;
        t_ss_evt.subsys_name_len = strnlen(accepted_messages[sys_m_notification_id].subsys_name, QMI_SSCTL_SUB_SYS_NAME_LENGTH_V02);
        strlcpy(t_ss_evt.subsys_name,accepted_messages[sys_m_notification_id].subsys_name,QMI_SSCTL_SUB_SYS_NAME_LENGTH_V02);
        t_ss_evt.event = accepted_messages[sys_m_notification_id].e_ssr_evt;
        /*ret = ssctl_v02_send_req(ADSP_SSCTL_SVC, SSCTL_SUBSYS_EVENT,  &t_ss_evt);*/
        ret = ssctl_v02_send_req(MPSS_SSCTL_SVC, SSCTL_SUBSYS_EVENT, &t_ss_evt);
        MSG_1(MSG_SSID_TMS, MSG_LEGACY_HIGH,PROC_ID"sys_m_signal_modules MPSS_SSCTL_SVC ret =%d ", ret);
    }

	if(sys_m_notification_id!=-1){
       //req_count = rcevt_getcontexts_name(accepted_messages[sys_m_notification_id]);
       orig_ack_count = rcevt_getcount_name(SYS_M_ACK);
       //signal
       rcevt_signal_name(accepted_messages[sys_m_notification_id].full_ssr_event);
       //wait for all clients to ack
       rcevt_wait_count_name(SYS_M_ACK, req_count + orig_ack_count);
	}

}

/**
 *
 * @brief sys_m_unknown_msgs
 *
 * System Monitor handling of misunderstood messages
 * 
*/
void sys_m_unknown_msgs(void)
{
    char *savePtr, *retStr;
    int i = 0;
    //version
    if (strstr(sys_m_buffer, SYS_M_VER) != NULL)
    {
        sys_m_send_AP_msg(SYS_M_VER_STR, strlen(SYS_M_VER_STR));
        return;
    }
    //unknown msg
    retStr = strtok_r(sys_m_buffer, ":", &savePtr);
    if (retStr != NULL)
    {
        for (i = 0; i < SYS_M_NUM_MSGTYPES; i++)
{
            if (strncmp(retStr, sys_m_msg_types[i], SYS_M_BUFFER_LEN) == 0)
    {
                strlcat(retStr, ":unknown", SYS_M_BUFFER_LEN);
                sys_m_send_AP_msg(retStr, strlen(retStr));

            return;
            }
        }
    }
    sys_m_send_AP_msg("err:unknown", strlen("err:unknown"));
    return;
}

/**
 *
 * @brief sys_m_err_notify_peripherals
 *
 * Notify peripherals of an error fatal.  Only error services should
 * call this function
 * 
*/
void sys_m_err_notify_peripherals(void)
{
    sys_m_smp2p_set(SYS_M_AP2SUBSYS_SMP2P_ERRFATAL, SMEM_MODEM);
    //sys_m_smp2p_set(SYS_M_AP2SUBSYS_SMP2P_ERRFATAL, SMEM_ADSP); /*no ADSP on 9x55*/
}

/**
 *
 * @brief sys_m_wait_peripherals
 *
 * Wait for the Q6SW and LPASS to finish their error cleanup
 * 
*/
void sys_m_wait_peripherals(void)
{
    int i;
    for (i = 0; i < SYS_M_MAX_ITERATIONS; i++)
    {
        if (sys_m_smp2p_get_modem_shutdwn_ack() & SYS_M_SUBSYS2AP_SMP2P_SHUT_DWN_ACK)
            return;
        //wait 100 ms before trying again
        dog_force_kick();
        DALSYS_BusyWait(100000);
    }
    return;
}

/**
 *
 * @brief sys_m_shutdown_notify_internal
 *
 * Signal interested modules to prepare for shutdown.  
 * 
*/ 
void sys_m_shutdown_notify_internal(void)
{
    int req_count = 0;
    int orig_ack_count = 0;
    
    //callbacks
    rcecb_signal_name(SYS_M_SHUTDOWN);
    
    //signals
    req_count = rcevt_getcontexts_name(SYS_M_SHUTDOWN);
    orig_ack_count = rcevt_getcount_name(SYS_M_SHUTDOWN_ACK);
    rcevt_signal_name(SYS_M_SHUTDOWN);
    rcevt_wait_count_name(SYS_M_SHUTDOWN_ACK, req_count + orig_ack_count);
}

/**
 *
 * @brief sys_m_shutdown_terminate
 *  
 * Terminate shutdown processing, possibly with a delay for 
 * automated verification. 
 * 
*/ 
static void sys_m_shutdown_terminate(timer_cb_data_type delay)
{
  if (delay)
  {
    // define callback timer to call this function without delay
    timer_def(&shutdown_delay_timer, &sd_delay_timer_group, NULL, 0, sys_m_shutdown_terminate, 0);
    timer_set(&shutdown_delay_timer, (timetick_type)delay, 0, T_SEC);
  }
  else
  {
    if (sys_m_true_reset)
    {
      if(sys_m_reset_over_ssctl_v02){
        ssctl_v02_send_ind();
      }
      else{
        sys_m_send_AP_msg(SYS_M_SHUTDOWN_ACK, strlen(SYS_M_SHUTDOWN_ACK));
      }
      DalChipPower_Reset(phChipPower);
    }
    else
    {
      if(sys_m_reset_over_ssctl_v02){
        ssctl_v02_send_ind();
      }
      else{
        sys_m_send_ack_n_dreg_sts_gpio(SYS_M_POWEROFF_ACK);
      }
      DalChipPower_PowerOff(phChipPower);
    }
  }
}


/*===========================================================================

FUNCTION SYS_M_SIO_OPEN

DESCRIPTION
  This function is called to open a port on the SIO channel

PARAMETERS
  port_id - SIO port ID to open

RETURN VALUE
  None
   
===========================================================================*/
static void
sys_m_sio_open (sio_port_id_type port_id)
{  
   sio_ioctl_param_type param;
   
   sys_m_sio_param.open_param.dsm_pool_param.dsm_mempool_id = DSM_DS_LARGE_ITEM_POOL;
   sys_m_sio_param.open_properties.stream_mode = SIO_DS_AUTODETECT_MODE;
   sys_m_sio_param.open_properties.port_id = port_id;
   sys_m_sio_param.open_properties.rx_func_ptr = sys_m_sio_rx_notify;   

    sys_m_sio_param.open_properties.tx_queue = &sys_m_tx_wm_queue;

    sys_m_sio_handle = sio_open(&(sys_m_sio_param.open_properties));

    if (sys_m_sio_handle != SIO_NO_STREAM_ID)
    {
      // Set flow control to allow flow 
      sio_ioctl (sys_m_sio_handle, SIO_IOCTL_INBOUND_FLOW_ENABLE, &param);
      // Set CD line 
      sio_ioctl (sys_m_sio_handle, SIO_IOCTL_CD_ASSERT, &param);
      // Set DSR line 
      sio_ioctl (sys_m_sio_handle, SIO_IOCTL_DSR_ASSERT, &param);
      // Clear RI line  
      sio_ioctl (sys_m_sio_handle, SIO_IOCTL_RI_DEASSERT, &param);
    }
}

/*===========================================================================

FUNCTION SYS_M_RDM_OPEN_CB

DESCRIPTION
  This callback is called when RDM wants diag to open a port on the SIO channel

PARAMETERS
  port_id - SIO port ID to open

RETURN VALUE
  None
   
===========================================================================*/
static void
sys_m_rdm_open_cb (sio_port_id_type port_id)
{
  /*
  ** RDM OPEN logic:
  ** 1. If the port_id given by RDM is valid, try to open that port
  ** 2. If invalid port ID is given or sio_open fails, try MHI, then USB.
  */
  if (sys_m_sio_handle != SIO_NO_STREAM_ID)
  {
    ERR_FATAL("rdm_open_cb() invoked without close",0, 0, 0);
  }
  
  switch (port_id)
  {
    case SIO_PORT_MHI_SSR:
    {
      sys_m_sio_open(port_id);     
      if (sys_m_sio_handle == SIO_NO_STREAM_ID)
      {
        sys_m_sio_open(SIO_PORT_USB_SER3);     
      }
    }
    break;
    
    case SIO_PORT_USB_SER3:
    {
      sys_m_sio_open(port_id);     
      if (sys_m_sio_handle == SIO_NO_STREAM_ID)
      {
        sys_m_sio_open(SIO_PORT_MHI_SSR);     
      }
    }
    break;

    /* Unsupported PORT ID */
    default:
    {      
      MSG_1(MSG_SSID_TMS, MSG_LEGACY_ERROR,"unknown port %u", port_id);

      sys_m_sio_open(SIO_PORT_MHI_SSR);     
      if (sys_m_sio_handle == SIO_NO_STREAM_ID)
      {
        sys_m_sio_open(SIO_PORT_USB_SER3);     
      }
    }
    break;
  }

  if (sys_m_sio_handle == SIO_NO_STREAM_ID)
  {
    /* Optionally can be changed to ERR_FATAL if this is critical service */
    rdm_notify (RDM_SYSM_SRVC, RDM_NOT_ALLOWED_S);
  }
  else
  {
    /* RDM issue open succeeded. Notify to RDM, so RDM can commit the current 
    ** mapping to config file in EFS
    */
    rdm_notify (RDM_SYSM_SRVC, RDM_DONE_S);
  }
}

/*===========================================================================

FUNCTION SYS_M_RDM_CLOSE_CB

DESCRIPTION
  This callback is called when RDM wants RMTS to close its port.

===========================================================================*/
static void
sys_m_rdm_close_cb ()
{
  if (sys_m_sio_handle != SIO_NO_STREAM_ID)
  {
    /* Call SIO close with NULL close_func_ptr to discard the TX data 
    ** immediately 
    */
    sio_close( sys_m_sio_handle, NULL );
    sys_m_sio_handle = SIO_NO_STREAM_ID;
    
    rdm_notify (RDM_SYSM_SRVC, RDM_DONE_S);
  }
}

void *sys_m_init_sfr_buffer(void)
{
  return sys_m_sfr_buffer;
}

